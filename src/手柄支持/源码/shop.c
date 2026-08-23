#include "shop.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"
#include "ui_bridge.h"

/*
 * shop.c
 *
 * 原版 EVE opcode 0x36 无论创建药铺、兵器铺还是其它商店，都会分配同样大小的对象、调用
 * 0x413FA0，并把 owner 放到 0x89FCD4。因此所有店铺只需要这一套 Adapter：
 * - 根层：左列买入、右列卖出；
 * - 顶部：六个类别以及“用器”右侧的真实退出按钮；
 * - 深层：同一个数量窗完成买/卖数量和最终确认。
 *
 * 价格、库存、金钱、可买卖资格与成交结果都不在这里复制。手柄只请求真实 Button 返回原版
 * code=1（原版右键选中/信息链）或 code=2（点击），让 RPG.exe 自己继续完整业务状态机；
 * 方向导航仅在信息 transition 入口抑制“打开窗”这最后一步，绝不复制前面的选中与说明刷新。
 */

typedef enum ShopSide {
    SHOP_SIDE_BUY = 0,  /* 画面左列：店铺商品/买入。 */
    SHOP_SIDE_SELL = 1  /* 画面右列：玩家库存/卖出。 */
} ShopSide;

typedef enum ShopPopupFocus {
    SHOP_POPUP_CONFIRM = 0,
    SHOP_POPUP_CANCEL = 1
} ShopPopupFocus;

/* 原版物品行 code=1 同时包含“选中该行”和“打开道具信息”；手柄导航只保留前半段。 */
typedef enum ShopRowIntent {
    SHOP_ROW_INTENT_NONE = 0,
    SHOP_ROW_INTENT_NAVIGATION,
    SHOP_ROW_INTENT_ITEM_INFO
} ShopRowIntent;

typedef enum ShopPageLanding {
    SHOP_PAGE_LAND_FIRST = 0,
    SHOP_PAGE_LAND_LAST,
    SHOP_PAGE_LAND_PRESERVE
} ShopPageLanding;

typedef enum ShopItemInfoState {
    SHOP_ITEM_INFO_UNAVAILABLE = -1,
    SHOP_ITEM_INFO_CLOSED = 0,
    SHOP_ITEM_INFO_OPENING,
    SHOP_ITEM_INFO_OPEN,
    SHOP_ITEM_INFO_CLOSING,
    SHOP_ITEM_INFO_UNKNOWN
} ShopItemInfoState;

typedef void (THISCALL *PFN_ShopItemInfoTransition)(void*, i32, i32);

typedef struct ShopState {
    int enabled;
    int item_info_enabled;
    int was_active;
    int nav_active;
    int marker_visible;
    int popup_was_active;
    ShopSide focus_side;
    ShopPopupFocus popup_focus;
    volatile ShopRowIntent row_intent;
    volatile u32 row_intent_tick;
    int page_wait_direction;
    int page_before_request;
    int page_landing_row;
    int page_change_observed;
    u32 page_wait_start_tick;
    u32 page_change_tick;
    ShopPageLanding page_landing;
    ShopSide page_wait_side;
    u8* session_owner;
} ShopState;

static ShopState g_shop;

/*
 * 全局槽只提供 owner 生命周期根。任何子对象都必须从本 tick 重新取得，
 * 绝不把上一家商店的列表/数量窗指针跨生命周期缓存下来。
 */
static u8* shop_owner(void) {
    u8* owner = *(u8**)GLOBAL_SHOP_UI;
    return Runtime_PtrOk(owner) ? owner : NULL;
}

/*
 * 每次访问子对象都同时验证 parent 与 child；任一链不成立就 fail-closed。
 * 这允许原版在退出动画中按自己的顺序销毁对象，而不会让 worker 追旧指针。
 */
static u8* shop_child(u8* owner, u32 offset) {
    u8* child;
    if (!Runtime_PtrOk(owner)) return NULL;
    child = *(u8**)(owner + offset);
    return Runtime_PtrOk(child) ? child : NULL;
}

static u8* shop_topbar(u8* owner) {
    return shop_child(owner, SHOP_TOPBAR);
}

static u8* shop_list(u8* owner, ShopSide side) {
    return shop_child(owner, side == SHOP_SIDE_BUY ? SHOP_BUY_LIST : SHOP_SELL_LIST);
}

static u8* shop_quantity_popup(u8* owner) {
    return shop_child(owner, SHOP_QUANTITY_POPUP);
}

static u8* shop_item_info(u8* owner) {
    return shop_child(owner, SHOP_ITEM_INFO);
}

/*
 * refactor35 按用户实机裁决恢复 refactor33 已覆盖全部商店的唯一活动协议：
 * GLOBAL_SHOP_UI 当前 owner + 根对象 active。refactor34 把构造期 vtable/五条子对象链误升格为
 * 每 tick 的硬门槛，任一瞬态不满足都会让所有商店在读键前整体返回。
 *
 * 子对象继续在各自使用点逐条验证；它们只能让本次具体动作 fail-closed，不能再关闭整个商店 Context。
 */
static u8* shop_active_owner(void) {
    u8* owner;
    if (!g_shop.enabled) return NULL;
    owner = shop_owner();
    if (!owner || *(u8*)(owner + SHOP_ACTIVE) == 0) return NULL;
    return owner;
}

int Shop_Active(void) {
    return shop_active_owner() != NULL;
}

/* 数量窗 active 是根层/深层的唯一模态分界；插件不根据上一次 A 自己猜窗口是否已打开。 */
static int shop_quantity_active(u8* owner) {
    u8* popup = shop_quantity_popup(owner);
    return popup && *(u8*)(popup + SHOP_QUANTITY_ACTIVE) != 0;
}

/*
 * +0x580 只记录十帧开/关动画，动画完成后会重新归零，不能把 mode==0 误判为“窗口关闭”。
 * 0x417810 原版 Update 自己以 current-position==open-position 作为关闭按钮的活动门，
 * 所以稳态开/关必须沿用同一对坐标判断。
 */
static ShopItemInfoState shop_item_info_state(u8* owner) {
    u8* info;
    int mode;
    int current_position;
    int closed_position;
    int open_position;

    if (!g_shop.item_info_enabled) return SHOP_ITEM_INFO_UNAVAILABLE;
    info = shop_item_info(owner);
    if (!info) return SHOP_ITEM_INFO_UNAVAILABLE;

    mode = *(i32*)(info + SHOP_ITEM_INFO_MODE);
    if (mode == 1) return SHOP_ITEM_INFO_OPENING;
    if (mode == 2) return SHOP_ITEM_INFO_CLOSING;
    if (mode != 0) return SHOP_ITEM_INFO_UNKNOWN;

    current_position = *(i32*)(info + SHOP_ITEM_INFO_CURRENT_POSITION);
    closed_position = *(i32*)(info + SHOP_ITEM_INFO_CLOSED_POSITION);
    open_position = *(i32*)(info + SHOP_ITEM_INFO_OPEN_POSITION);
    if (current_position == open_position) return SHOP_ITEM_INFO_OPEN;
    if (current_position == closed_position) return SHOP_ITEM_INFO_CLOSED;
    return SHOP_ITEM_INFO_UNKNOWN;
}

static ShopSide shop_native_side(u8* owner) {
    /* 原版正好和插件枚举相反：1=左买入、0=右卖出。这里只读，不直接写。 */
    return *(u8*)(owner + SHOP_LAST_TRANSACTION_SIDE) != 0 ? SHOP_SIDE_BUY : SHOP_SIDE_SELL;
}

/* index0 是真实退出图标，index1..6 才是六个类别；调用者不能把两种语义混在一起。 */
static void* shop_top_button(u8* owner, int index) {
    u8* top = shop_topbar(owner);
    void* button;
    if (!top || index < 0 || index > SHOP_CATEGORY_COUNT) return NULL;
    button = *(void**)(top + SHOP_TOP_BUTTON0 + (u32)index * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

static void* shop_list_button(u8* list, int row) {
    void* button;
    if (!Runtime_PtrOk(list) || row < 0 || row >= SHOP_ROW_COUNT) return NULL;
    button = *(void**)(list + SHOP_ROW_BUTTON0 + (u32)row * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/* 当前页行记录 +0x30 只表示这一行是否实际存在，不判断能否买卖。 */
static int shop_row_present(u8* list, int row) {
    u8* record;
    if (!Runtime_PtrOk(list) || row < 0 || row >= SHOP_ROW_COUNT) return 0;
    record = *(u8**)(list + SHOP_ROW_RECORD0 + (u32)row * 4u);
    if (!Runtime_PtrOk(record)) return 0;
    return *(u8*)(record + SHOP_RECORD_ACTIVE) != 0;
}

static u32 shop_row_offset(ShopSide side) {
    return side == SHOP_SIDE_BUY ? SHOP_BUY_CURRENT_ROW : SHOP_SELL_CURRENT_ROW;
}

static u32 shop_page_offset(ShopSide side) {
    return side == SHOP_SIDE_BUY ? SHOP_BUY_CURRENT_PAGE : SHOP_SELL_CURRENT_PAGE;
}

static u32 shop_total_pages_offset(ShopSide side) {
    return side == SHOP_SIDE_BUY ? SHOP_BUY_TOTAL_PAGES : SHOP_SELL_TOTAL_PAGES;
}

/*
 * 新页或新类别可能暂时没有有效 current row。
 * 这时只寻找原版已经填好的第一条记录，随后仍靠 code=1 让原版真正建立选中状态。
 */
static int shop_first_present_row(u8* list) {
    int row;
    for (row = 0; row < SHOP_ROW_COUNT; ++row) {
        if (shop_row_present(list, row)) return row;
    }
    return -1;
}

static int shop_last_present_row(u8* list) {
    int row;
    for (row = SHOP_ROW_COUNT - 1; row >= 0; --row) {
        if (shop_row_present(list, row)) return row;
    }
    return -1;
}

static int shop_find_row(u8* list, int current, int delta) {
    int row;
    int tries;

    if (current < 0 || current >= SHOP_ROW_COUNT || !shop_row_present(list, current)) {
        return shop_first_present_row(list);
    }

    row = current;
    for (tries = 0; tries < SHOP_ROW_COUNT; ++tries) {
        row += delta;
        if (row < 0 || row >= SHOP_ROW_COUNT) break;
        if (shop_row_present(list, row)) return row;
    }
    return current;
}

/* 只有明确的菜单键动作才能开始手柄导航会话；实体鼠标接管会在独立回调里撤销这面旗。 */
static void shop_claim_navigation(void) {
    Cursor_ClaimForControllerNavigation();
    g_shop.nav_active = 1;
}

/* 共享 Cursor 只有一只手；Shop 只撤销自己确实显示过的列标记，不能替其它页面做无条件 Hide。 */
static void shop_hide_own_marker(void) {
    if (!g_shop.marker_visible) return;
    Cursor_HideMenuFocusImmediately();
    g_shop.marker_visible = 0;
}

/*
 * 游戏没有“当前聚焦哪一列”的图示，所以沿用户参考图把原版手形放到该列标题右上方。
 * 位置从 row0 的真实屏幕矩形动态推导，不写死 854x480 坐标：
 * - X：列表右缘向左退约 1/6 列宽后，再按实机反馈向左微调 10 像素；
 * - Y：第一行上方一个行高后，再向下微调 5 像素，落在用户图示的列标题框内部。
 * 这个点位于物品行命中区之外，因此只充当视觉牌，不会偷偷改变某一行的鼠标业务。
 */
static void shop_show_column_marker(u8* owner) {
    u8* list;
    void* anchor;
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;
    i32 width;
    i32 height;
    i32 x;
    i32 y;

    if (!g_shop.nav_active) {
        shop_hide_own_marker();
        return;
    }
    list = shop_list(owner, g_shop.focus_side);
    if (!list) {
        shop_hide_own_marker();
        return;
    }
    anchor = shop_list_button(list, 0);
    if (!anchor || !UiBridge_GetButtonScreenRect(anchor, &left, &top, &right, &bottom)) {
        shop_hide_own_marker();
        return;
    }

    width = right - left;
    height = bottom - top;
    x = right - width / 6 - 10;
    y = top - height + 5;
    Cursor_ShowMenuFocusAt(x, y);
    g_shop.marker_visible = 1;
}

/*
 * 数量窗自己会根据真实 HitTest 画“确定/取消”方框，所以这里只移动隐藏命中点，不再显示第二只手形。
 * 这样方向左右仍有原版视觉反馈，又不会让鼠标指针压住文字。
 */
static void shop_sync_popup_selection(u8* popup) {
    void* button;
    i32 x;
    i32 y;

    if (!g_shop.nav_active || !Runtime_PtrOk(popup)) return;
    button = *(void**)(popup + (g_shop.popup_focus == SHOP_POPUP_CONFIRM
        ? SHOP_QUANTITY_CONFIRM_BUTTON : SHOP_QUANTITY_CANCEL_BUTTON));
    if (!Runtime_PtrOk(button)) return;
    if (!UiBridge_GetButtonScreenCenter(button, &x, &y)) return;
    shop_hide_own_marker();
    Cursor_MoveHiddenSelectionAt(x, y);
    g_shop.marker_visible = 0;
}

static void shop_clear_row_intent(void) {
    g_shop.row_intent = SHOP_ROW_INTENT_NONE;
    g_shop.row_intent_tick = 0;
}

/*
 * pending 若被原版拒绝且没走到 transition，短暂保留意图后自动回收。
 * 不能像 r35 那样只留一个 worker tick：ButtonEvent 消费和随后 transition 虽在同一游戏线程，
 * worker 仍可能恰好在两者之间抢占；64ms 保险窗避免这种偶发自动开窗。
 */
static void shop_reap_row_intent(void) {
    if (g_shop.row_intent == SHOP_ROW_INTENT_NONE) return;
    if (UiBridge_EventOwner() == UI_EVENT_OWNER_SHOP) return;
    if ((u32)(Runtime_Tick() - g_shop.row_intent_tick) < Runtime_MsToTicks(64u)) return;
    shop_clear_row_intent();
}

static void shop_clear_page_wait(void) {
    g_shop.page_wait_direction = 0;
    g_shop.page_before_request = -1;
    g_shop.page_landing_row = -1;
    g_shop.page_change_observed = 0;
    g_shop.page_wait_start_tick = 0;
    g_shop.page_change_tick = 0;
    g_shop.page_landing = SHOP_PAGE_LAND_FIRST;
    g_shop.page_wait_side = SHOP_SIDE_BUY;
}

/*
 * 行事件的统一出口同时验证“记录存在”和“真实 Button 存在”。
 * 原版 code=1 会同步行/说明/交易侧并继续打开道具信息；导航意图由 transition hook 只抑制最后一步。
 * code=2 才允许 RPG.exe 尝试打开买卖数量窗。
 */
static void shop_request_row_event(
    u8* owner,
    ShopSide side,
    int row,
    int code,
    ShopRowIntent intent,
    const char* tag
) {
    u8* list;
    void* button;

    if (UiBridge_EventPending()) return;
    list = shop_list(owner, side);
    if (!list || !shop_row_present(list, row)) return;
    button = shop_list_button(list, row);
    if (!button) return;

    shop_claim_navigation();
    if (code == 1 && g_shop.item_info_enabled) {
        g_shop.row_intent = intent;
        g_shop.row_intent_tick = Runtime_Tick();
    } else {
        shop_clear_row_intent();
    }
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SHOP, button, code, tag);
}

/*
 * 0x414CB3 / 0x415102 是左右物品行 code=1 分支中唯一真正打开信息窗的调用。
 * 导航仍完整执行此前的行同步和说明刷新，只在这里拦下打开动画；Y 意图和真实鼠标右键则原样放行。
 */
static void FASTCALL Shop_HookItemInfoTransition(
    void* info,
    void* unused_edx,
    i32 mode,
    i32 side
) {
    PFN_ShopItemInfoTransition orig = (PFN_ShopItemInfoTransition)FN_SHOP_ITEM_INFO_TRANSITION;
    ShopRowIntent intent = g_shop.row_intent;
    u8* expected_info = NULL;
    (void)unused_edx;

    if (Runtime_PtrOk(g_shop.session_owner)) expected_info = shop_item_info(g_shop.session_owner);
    if (mode == 1 && intent != SHOP_ROW_INTENT_NONE) {
        shop_clear_row_intent();
        if (info == expected_info && intent == SHOP_ROW_INTENT_NAVIGATION) {
            Runtime_Log("[商店] 行导航已完成原版选中/说明同步；按规格抑制非Y触发的道具信息窗。");
            return;
        }
        if (info == expected_info && intent == SHOP_ROW_INTENT_ITEM_INFO) {
            Runtime_Log("[商店] Y：放行原版右键道具信息打开事务。");
        }
    }

    orig(info, mode, side);
}

/* X 只切换插件的左右列焦点；再用目标列当前行的原版 code=1 同步说明区和原版交易侧。 */
static void shop_switch_side(u8* owner) {
    u8* list;
    int row;

    if (g_shop.page_wait_direction != 0 || UiBridge_EventPending()) return;
    g_shop.focus_side = g_shop.focus_side == SHOP_SIDE_BUY ? SHOP_SIDE_SELL : SHOP_SIDE_BUY;
    shop_claim_navigation();

    list = shop_list(owner, g_shop.focus_side);
    if (!list) return;
    row = *(i32*)(owner + shop_row_offset(g_shop.focus_side));
    if (row < 0 || row >= SHOP_ROW_COUNT || !shop_row_present(list, row)) row = shop_first_present_row(list);
    if (row < 0) return;

    shop_request_row_event(owner, g_shop.focus_side, row, 1, SHOP_ROW_INTENT_NAVIGATION,
        g_shop.focus_side == SHOP_SIDE_BUY
            ? "[商店] X：聚焦左侧买入列，并请求当前行原版选中 Event。"
            : "[商店] X：聚焦右侧卖出列，并请求当前行原版选中 Event。");
}

static void shop_request_page(
    u8* owner,
    int delta,
    ShopPageLanding landing,
    int landing_row,
    const char* tag
);

static void shop_move_row(u8* owner, int delta) {
    u8* list;
    int current;
    int target;

    if (g_shop.page_wait_direction != 0 || UiBridge_EventPending()) return;
    list = shop_list(owner, g_shop.focus_side);
    if (!list) return;
    current = *(i32*)(owner + shop_row_offset(g_shop.focus_side));
    target = shop_find_row(list, current, delta);
    if (target < 0) {
        shop_claim_navigation();
        return;
    }

    if (target == current && shop_row_present(list, current)) {
        /*
         * 与主 Interface/Battle 的长列表一致：同页已经到顶/到底时继续点击原版分页按钮，
         * 下一页落第一条，上一页落最后一条。第一页/末页仍由原版边界拒绝。
         */
        shop_request_page(owner, delta,
            delta < 0 ? SHOP_PAGE_LAND_LAST : SHOP_PAGE_LAND_FIRST,
            -1,
            delta < 0
                ? "[商店] ↑到顶：请求当前列原版上一页，完成后落最后一件。"
                : "[商店] ↓到底：请求当前列原版下一页，完成后落第一件。");
        return;
    }

    shop_request_row_event(owner, g_shop.focus_side, target, 1, SHOP_ROW_INTENT_NAVIGATION,
        delta < 0 ? "[商店] ↑：请求当前列上一件物品的原版选中 Event。"
                  : "[商店] ↓：请求当前列下一件物品的原版选中 Event。");
}

/*
 * 翻页只投递左右列各自的真实 Prev/Next ButtonEvent，并登记一个私有“新页落点”事务。
 * 页码与列表刷新完全交给 RPG.exe；worker 观察到原版页码真的变化、并再等待一 tick 后，
 * 才向新页目标行投递 code=1，以避免读到翻页函数尚未完成重建的旧行对象。
 */
static void shop_request_page(
    u8* owner,
    int delta,
    ShopPageLanding landing,
    int landing_row,
    const char* tag
) {
    u8* list;
    u32 button_offset;
    int current;
    int total;
    void* button;

    if (delta == 0 || g_shop.page_wait_direction != 0 || UiBridge_EventPending()) return;
    list = shop_list(owner, g_shop.focus_side);
    if (!list) return;
    /* 即使已经在第一页/最后一页，方向键仍然代表手柄已接管，列焦点必须出现。 */
    shop_claim_navigation();
    current = *(i32*)(owner + shop_page_offset(g_shop.focus_side));
    total = *(i32*)(owner + shop_total_pages_offset(g_shop.focus_side));
    if (total <= 0) return;

    if (delta < 0) {
        if (current <= 0) return;
        button_offset = g_shop.focus_side == SHOP_SIDE_BUY
            ? SHOP_BUY_PAGE_PREV_BUTTON : SHOP_SELL_PAGE_PREV_BUTTON;
    } else {
        if (current + 1 >= total) return;
        button_offset = g_shop.focus_side == SHOP_SIDE_BUY
            ? SHOP_BUY_PAGE_NEXT_BUTTON : SHOP_SELL_PAGE_NEXT_BUTTON;
    }

    button = *(void**)(list + button_offset);
    if (!Runtime_PtrOk(button)) return;

    g_shop.page_wait_direction = delta < 0 ? -1 : 1;
    g_shop.page_before_request = current;
    g_shop.page_landing = landing;
    g_shop.page_landing_row = landing_row;
    g_shop.page_wait_side = g_shop.focus_side;
    g_shop.page_wait_start_tick = Runtime_Tick();
    g_shop.page_change_observed = 0;
    g_shop.page_change_tick = 0;

    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SHOP, button, 2, tag);
    if (UiBridge_EventOwner() != UI_EVENT_OWNER_SHOP) {
        shop_clear_page_wait();
    }
}

static void shop_finish_page_landing(u8* owner) {
    u8* list;
    ShopSide side;
    ShopPageLanding landing;
    int landing_row;
    int current_page;
    int target_row;

    if (!Runtime_PtrOk(owner) || g_shop.page_wait_direction == 0) return;
    if (g_shop.page_wait_side != g_shop.focus_side) {
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_SHOP);
        shop_clear_page_wait();
        return;
    }

    current_page = *(i32*)(owner + shop_page_offset(g_shop.page_wait_side));
    if (current_page == g_shop.page_before_request) {
        if ((u32)(Runtime_Tick() - g_shop.page_wait_start_tick) >= Runtime_MsToTicks(1000u)) {
            UiBridge_ClearEventOwned(UI_EVENT_OWNER_SHOP);
            shop_clear_page_wait();
            Runtime_Log("[商店] 原版分页在1秒内未改变页码；已只清理插件等待事务。");
        }
        return;
    }

    /* 页码变化可能发生在原版刷新函数中段；至少跨一个 worker tick 再读取新页 Button/记录。 */
    if (!g_shop.page_change_observed) {
        g_shop.page_change_observed = 1;
        g_shop.page_change_tick = Runtime_Tick();
        return;
    }
    if ((u32)(Runtime_Tick() - g_shop.page_change_tick) == 0u) return;

    side = g_shop.page_wait_side;
    landing = g_shop.page_landing;
    landing_row = g_shop.page_landing_row;
    list = shop_list(owner, side);
    if (!list) {
        if ((u32)(Runtime_Tick() - g_shop.page_wait_start_tick) < Runtime_MsToTicks(1000u)) return;
        shop_clear_page_wait();
        return;
    }

    if (landing == SHOP_PAGE_LAND_FIRST) {
        target_row = shop_first_present_row(list);
    } else if (landing == SHOP_PAGE_LAND_LAST) {
        target_row = shop_last_present_row(list);
    } else if (landing == SHOP_PAGE_LAND_PRESERVE &&
               landing_row >= 0 && shop_row_present(list, landing_row)) {
        target_row = landing_row;
    } else {
        target_row = shop_last_present_row(list);
        if (target_row < 0) target_row = shop_first_present_row(list);
    }

    if (target_row < 0) {
        if ((u32)(Runtime_Tick() - g_shop.page_wait_start_tick) < Runtime_MsToTicks(1000u)) return;
        shop_clear_page_wait();
        return;
    }

    shop_clear_page_wait();
    shop_request_row_event(owner, side, target_row, 1, SHOP_ROW_INTENT_NAVIGATION,
        "[商店] 原版翻页已完成：请求新页目标物品的原版选中 Event。");
}

/* 顶部 raw 类别为 1..6，且增长方向与画面左右相反；Adapter 在这里消化反向编号。 */
static void shop_request_category(u8* owner, int visual_delta) {
    int current;
    int target;
    void* button;

    if (UiBridge_EventPending()) return;
    shop_claim_navigation();
    current = *(i32*)(owner + SHOP_CATEGORY);
    if (current < 1 || current > SHOP_CATEGORY_COUNT) current = 1;
    target = current - visual_delta;
    if (target < 1) target = SHOP_CATEGORY_COUNT;
    if (target > SHOP_CATEGORY_COUNT) target = 1;

    button = shop_top_button(owner, target);
    if (!button) return;
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SHOP, button, 2,
        visual_delta < 0 ? "[商店] LB：请求原版切到视觉左侧类别。"
                         : "[商店] RB：请求原版切到视觉右侧类别。");
}

static void shop_confirm_current_row(u8* owner) {
    u8* list = shop_list(owner, g_shop.focus_side);
    int row;

    if (!list || g_shop.page_wait_direction != 0 || UiBridge_EventPending()) return;
    shop_claim_navigation();
    row = *(i32*)(owner + shop_row_offset(g_shop.focus_side));
    if (row < 0 || row >= SHOP_ROW_COUNT || !shop_row_present(list, row)) return;
    shop_request_row_event(owner, g_shop.focus_side, row, 2, SHOP_ROW_INTENT_NONE,
        g_shop.focus_side == SHOP_SIDE_BUY
            ? "[商店] A：请求左列当前物品原版买入 Event。"
            : "[商店] A：请求右列当前物品原版卖出 Event。");
}

/* Y 完整复用原版右键信息链：关闭时点真实 close Button，打开时让当前物品行返回一次 code=1。 */
static void shop_toggle_item_info(u8* owner) {
    u8* info;
    u8* list;
    void* close_button;
    ShopItemInfoState state;
    int row;

    if (!g_shop.item_info_enabled) return;
    info = shop_item_info(owner);
    if (!info || g_shop.page_wait_direction != 0 || UiBridge_EventPending()) return;
    state = shop_item_info_state(owner);
    shop_claim_navigation();

    if (state == SHOP_ITEM_INFO_OPENING || state == SHOP_ITEM_INFO_CLOSING) {
        Runtime_Log("[商店] Y：道具信息开关动画尚未完成，本次等待原版动画收口。");
        return;
    }
    if (state == SHOP_ITEM_INFO_OPEN) {
        close_button = *(void**)(info + SHOP_ITEM_INFO_CLOSE_BUTTON);
        if (!Runtime_PtrOk(close_button)) return;
        shop_clear_row_intent();
        UiBridge_RequestEventOwned(UI_EVENT_OWNER_SHOP, close_button, 2,
            "[商店] Y：请求原版道具信息关闭 Button。");
        return;
    }
    if (state != SHOP_ITEM_INFO_CLOSED) return;

    list = shop_list(owner, g_shop.focus_side);
    if (!list) return;
    row = *(i32*)(owner + shop_row_offset(g_shop.focus_side));
    if (row < 0 || row >= SHOP_ROW_COUNT || !shop_row_present(list, row)) row = shop_first_present_row(list);
    if (row < 0) return;
    shop_request_row_event(owner, g_shop.focus_side, row, 1, SHOP_ROW_INTENT_ITEM_INFO,
        "[商店] Y：请求当前物品原版右键/道具信息 Event。");
}

/* 信息窗在手柄路径中是真正模态层：可见/关闭动画期间只有 Y 会被解释。 */
static void shop_handle_item_info(u8* owner) {
    if (InputRouter_PressedOn(INPUT_CTX_SHOP_ROOT, INPUT_SPECIAL_Y, INPUT_LAYER_OVERLAY)) {
        shop_toggle_item_info(owner);
    }
    shop_show_column_marker(owner);
}

/* 根层 B 与炼化保持同一退出规则：点击顶部 index0；绝不模拟右键或直接清 active。 */
static void shop_exit(u8* owner) {
    void* button;
    if (UiBridge_EventPending()) return;
    shop_claim_navigation();
    button = shop_top_button(owner, 0);
    if (!button) return;
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SHOP, button, 2,
        "[商店] B：请求‘用器’右侧原版退出图标；没有使用鼠标右键。");
}

/* 数量窗四个动作也共用一次性事件出口；LT/RT 不需要额外伪造 Sprite 按压动画。 */
static void shop_request_popup_button(u8* popup, u32 offset, const char* tag) {
    void* button;
    if (!Runtime_PtrOk(popup) || UiBridge_EventPending()) return;
    button = *(void**)(popup + offset);
    if (!Runtime_PtrOk(button)) return;
    shop_claim_navigation();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SHOP, button, 2, tag);
}

static void shop_handle_quantity(u8* popup) {
    InputContext context = INPUT_CTX_SHOP_QUANTITY;

    /* B 是最高优先级的安全退回；随后才解释同一 tick 的其它按钮。 */
    if (InputRouter_PressedOn(context, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        g_shop.popup_focus = SHOP_POPUP_CANCEL;
        shop_request_popup_button(popup, SHOP_QUANTITY_CANCEL_BUTTON,
            "[商店数量] B：请求原版取消 Button。");
    } else if (InputRouter_PressedOn(context, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        shop_claim_navigation();
        g_shop.popup_focus = SHOP_POPUP_CONFIRM;
        Runtime_Log("[商店数量] ←：聚焦确认买入/卖出。");
    } else if (InputRouter_PressedOn(context, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        shop_claim_navigation();
        g_shop.popup_focus = SHOP_POPUP_CANCEL;
        Runtime_Log("[商店数量] →：聚焦取消。");
    } else if (InputRouter_PressedOn(context, INPUT_SUBTYPE_PREV, INPUT_LAYER_OVERLAY)) {
        shop_request_popup_button(popup, SHOP_QUANTITY_DEC_BUTTON,
            "[商店数量] LT：请求原版数量减少；不伪造按压动画。");
    } else if (InputRouter_PressedOn(context, INPUT_SUBTYPE_NEXT, INPUT_LAYER_OVERLAY)) {
        shop_request_popup_button(popup, SHOP_QUANTITY_INC_BUTTON,
            "[商店数量] RT：请求原版数量增加；不伪造按压动画。");
    } else if (InputRouter_PressedOn(context, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        shop_request_popup_button(popup,
            g_shop.popup_focus == SHOP_POPUP_CONFIRM
                ? SHOP_QUANTITY_CONFIRM_BUTTON : SHOP_QUANTITY_CANCEL_BUTTON,
            g_shop.popup_focus == SHOP_POPUP_CONFIRM
                ? "[商店数量] A：请求原版确认买入/卖出 Button。"
                : "[商店数量] A：请求原版取消 Button。");
    }

    shop_sync_popup_selection(popup);
}

/*
 * 根层一次只接受一个动作，明确固定 B > Y > X > 类别 > 翻页 > 行导航 > A 的冲突优先级。
 * 正常单键操作不受影响；意外同时按键时不会在同一 tick 排两个互相竞争的原版事件。
 */
static void shop_handle_root(u8* owner) {
    InputContext context = INPUT_CTX_SHOP_ROOT;
    u8* list;
    int row;

    if (InputRouter_PressedOn(context, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        shop_exit(owner);
    } else if (InputRouter_PressedOn(context, INPUT_SPECIAL_Y, INPUT_LAYER_OVERLAY)) {
        shop_toggle_item_info(owner);
    } else if (InputRouter_PressedOn(context, INPUT_SPECIAL_X, INPUT_LAYER_OVERLAY)) {
        shop_switch_side(owner);
    } else if (InputRouter_PressedOn(context, INPUT_CATEGORY_PREV, INPUT_LAYER_OVERLAY)) {
        shop_request_category(owner, -1);
    } else if (InputRouter_PressedOn(context, INPUT_CATEGORY_NEXT, INPUT_LAYER_OVERLAY)) {
        shop_request_category(owner, +1);
    } else if (InputRouter_PressedOn(context, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        list = shop_list(owner, g_shop.focus_side);
        row = list ? *(i32*)(owner + shop_row_offset(g_shop.focus_side)) : -1;
        if (!list || !shop_row_present(list, row)) row = shop_first_present_row(list);
        shop_request_page(owner, -1, SHOP_PAGE_LAND_PRESERVE, row,
            "[商店] ←：请求当前列原版上一页，并尽量保留当前行。");
    } else if (InputRouter_PressedOn(context, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        list = shop_list(owner, g_shop.focus_side);
        row = list ? *(i32*)(owner + shop_row_offset(g_shop.focus_side)) : -1;
        if (!list || !shop_row_present(list, row)) row = shop_first_present_row(list);
        shop_request_page(owner, +1, SHOP_PAGE_LAND_PRESERVE, row,
            "[商店] →：请求当前列原版下一页，并尽量保留当前行。");
    } else if (InputRouter_PressedOn(context, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        shop_move_row(owner, -1);
    } else if (InputRouter_PressedOn(context, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        shop_move_row(owner, +1);
    } else if (InputRouter_PressedOn(context, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        shop_confirm_current_row(owner);
    }

    shop_show_column_marker(owner);
}

/* 商店是模态界面；除 Start/R3/Back 外，所有菜单动作都必须在这里止住。 */
static void shop_consume_modal_actions(void) {
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_CANCEL);
    InputRouter_Consume(INPUT_SPECIAL_X);
    InputRouter_Consume(INPUT_SPECIAL_Y);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
    InputRouter_Consume(INPUT_CATEGORY_PREV);
    InputRouter_Consume(INPUT_CATEGORY_NEXT);
    InputRouter_Consume(INPUT_SUBTYPE_PREV);
    InputRouter_Consume(INPUT_SUBTYPE_NEXT);
}

static void shop_end_session(void) {
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_SHOP);
    shop_hide_own_marker();
    shop_clear_row_intent();
    shop_clear_page_wait();
    g_shop.was_active = 0;
    g_shop.nav_active = 0;
    g_shop.marker_visible = 0;
    g_shop.popup_was_active = 0;
    g_shop.session_owner = NULL;
}

/*
 * 每 tick 先重新确认 owner/active，再按数量窗深度分发。
 * 深层存在时根层完全不读键；深层关闭后保留原交易列，并恢复列标题手形。
 */
void Shop_Update(void) {
    u8* owner;
    u8* popup;
    int popup_active;
    ShopItemInfoState info_state;

    owner = shop_active_owner();
    if (!owner) {
        if (g_shop.was_active) shop_end_session();
        return;
    }

    if (!g_shop.was_active || g_shop.session_owner != owner) {
        if (g_shop.was_active) {
            UiBridge_ClearEventOwned(UI_EVENT_OWNER_SHOP);
            shop_hide_own_marker();
        }
        g_shop.was_active = 1;
        g_shop.session_owner = owner;
        g_shop.focus_side = shop_native_side(owner);
        g_shop.popup_focus = SHOP_POPUP_CONFIRM;
        g_shop.popup_was_active = 0;
        g_shop.marker_visible = 0;
        shop_clear_row_intent();
        shop_clear_page_wait();
        g_shop.nav_active = Cursor_ControllerOwnsPointer() ? 1 : 0;
        Runtime_Log("[商店] refactor33 owner+active 活动协议确认；统一双列、连续分页与数量窗 Adapter 已进入当前商店。");
    }

    shop_reap_row_intent();
    shop_finish_page_landing(owner);

    /* 鼠标会话中不强制手形；但持续旁听原版最后 hover 的列，下一次手柄接管就从那里继续。 */
    if (!g_shop.nav_active) g_shop.focus_side = shop_native_side(owner);

    popup = shop_quantity_popup(owner);
    popup_active = shop_quantity_active(owner);
    info_state = shop_item_info_state(owner);
    if (info_state == SHOP_ITEM_INFO_OPENING ||
        info_state == SHOP_ITEM_INFO_OPEN ||
        info_state == SHOP_ITEM_INFO_CLOSING) {
        shop_handle_item_info(owner);
    } else if (popup_active) {
        if (!g_shop.popup_was_active) {
            g_shop.popup_was_active = 1;
            g_shop.popup_focus = SHOP_POPUP_CONFIRM;
            shop_hide_own_marker();
            Runtime_Log("[商店数量] 原版买入/卖出数量窗已打开；默认聚焦确认。");
        }
        shop_handle_quantity(popup);
    } else if (g_shop.page_wait_direction != 0) {
        /* 分页 Button 已发出但原版尚未完成新页落点；此时不接受第二条菜单事务。 */
        shop_show_column_marker(owner);
    } else {
        if (g_shop.popup_was_active) {
            g_shop.popup_was_active = 0;
            g_shop.popup_focus = SHOP_POPUP_CONFIRM;
            Runtime_Log("[商店数量] 数量窗已关闭；返回原交易列。");
        }
        shop_handle_root(owner);
    }

    shop_consume_modal_actions();
}

void Shop_OnPointerTakeover(CursorTakeoverEvent event_type) {
    (void)event_type;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_SHOP);
    shop_clear_row_intent();
    shop_clear_page_wait();
    g_shop.nav_active = 0;
    shop_hide_own_marker();
}

int Shop_InstallHooks(void) {
    if (!Runtime_ShopProtocolOk()) {
        g_shop.enabled = 0;
        Runtime_Log("[商店] 原版统一商店协议未通过；商店手柄能力已单独禁用。");
        return 1;
    }
    if (!UiBridge_InstallShopHooks()) {
        g_shop.enabled = 0;
        Runtime_Log("[商店] ButtonEvent Hook 安装失败；商店手柄能力已单独禁用。");
        return 1;
    }

    /*
     * Y 信息窗是可选子能力，绝不能反向成为 r33 商店主体的启动门。
     * 0x417829 与全局 CALL_CMD0_EVENT 是同一个 ButtonEvent 调用点，UiBridge_InstallHooks()
     * 已在 Shop 之前安装；这里必须复用它，不能像 r35 那样再次 Patch 同一地址并触发 fail-closed。
     */
    g_shop.item_info_enabled = 0;
    if (Runtime_ShopItemInfoProtocolOk()) {
        if (Runtime_PatchCall(CALL_SHOP_SELL_ITEM_INFO_OPEN,
                (void*)Shop_HookItemInfoTransition, FN_SHOP_ITEM_INFO_TRANSITION) &&
            Runtime_PatchCall(CALL_SHOP_BUY_ITEM_INFO_OPEN,
                (void*)Shop_HookItemInfoTransition, FN_SHOP_ITEM_INFO_TRANSITION)) {
            g_shop.item_info_enabled = 1;
            Runtime_Log("[商店信息] 已复用公共 ButtonEvent 桥；Y开关与导航抑制 transition 独立能力已启用。");
        } else {
            Runtime_Log("[商店信息] 可选 Hook 安装失败；仅禁用 Y 信息窗，r33 商店主体继续启用。");
        }
    } else {
        Runtime_Log("[商店信息] 可选协议不匹配；仅禁用 Y 信息窗，r33 商店主体继续启用。");
    }

    g_shop.enabled = 1;
    g_shop.was_active = 0;
    g_shop.nav_active = 0;
    g_shop.marker_visible = 0;
    g_shop.popup_was_active = 0;
    g_shop.focus_side = SHOP_SIDE_BUY;
    g_shop.popup_focus = SHOP_POPUP_CONFIRM;
    shop_clear_row_intent();
    shop_clear_page_wait();
    g_shop.session_owner = NULL;
    Runtime_Log(g_shop.item_info_enabled
        ? "[商店] 主体已启用：X换列、Y独占开关信息、↑↓连续跨页、←→翻页、LB/RB类别、A买卖、B退出、LT/RT调数量。"
        : "[商店] 主体已启用：X换列、↑↓选物、←→翻页、LB/RB类别、A买卖、B退出、LT/RT调数量；Y信息能力单独关闭。");
    return 1;
}
