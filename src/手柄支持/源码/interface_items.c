#include "interface_items.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "ui_bridge.h"

/*
 * interface_items.c
 *
 * 这份代码只把手柄动作转换成 state2 页面“本来就存在的原版 ButtonEvent”。
 * 它绝不直接修改：
 * - 当前子类型 INTERFACE_ITEMS_SUBINDEX；
 * - 当前行 INTERFACE_ITEMS_CURRENT_ROW；
 * - 当前页 INTERFACE_ITEMS_CURRENT_PAGE；
 * - 物品数量、库存、装备/分发结果等任何游戏数据。
 *
 * 为什么坚持这样做：
 * 原版在 ButtonEvent 返回 code=2 后，还会自己执行刷新、声音、动画、条件检查和弹窗。
 * 如果插件直接写数字，看起来可能“一瞬间切过去了”，但很容易漏掉这些隐藏副作用。
 * 因此本页和 Battle/SaveSlot 一样，只负责“替手柄点真实按钮”。
 */

typedef struct InterfaceItemsState {
    /* 1=本页协议预检与 Hook 都安装成功；0=整个页面 Adapter 保持静默。 */
    int enabled;

    /* 上一个观察到的 Interface state；用来检测进入/离开 state2 并清理瞬态状态。 */
    int was_active;

    /*
     * 翻页不是插件自己写页码，而是先点原版 Prev/Next。
     * page_wait_direction 保存“刚才请求的是上一页(-1)还是下一页(+1)”，
     * 等原版 CURRENT_PAGE 真正变化后，再把视觉选中落到新页的合理行。
     */
    int page_wait_direction;
    int page_before_request;
    int last_page_seen;

    /* 记录上一个子类型；原版切子类型会自己重建列表，看到变化时本地翻页事务必须作废。 */
    int last_subtype_seen;

    /* 页面专属双按钮弹窗的逻辑焦点：0=第一个（确定/是），1=第二个（取消/否）。 */
    int popup_focus;

    /* 只有玩家真的用过手柄方向后才强制 HitTest；否则鼠标 hover 继续按原版工作。 */
    int popup_nav_active;

    /* 当前被本页接管的弹窗对象；弹窗对象更换时必须重新读取原版原生选择。 */
    void* popup_owner;
} InterfaceItemsState;

static InterfaceItemsState g_items;

/* 读取主 Interface；任何页面字段访问都必须先从这里经过 Runtime_PtrOk。 */
static u8* items_interface(void) {
    u8* i = *(u8**)GLOBAL_INTERFACE_UI;
    return Runtime_PtrOk(i) ? i : NULL;
}

/* 只有 state==2 才返回法宝/道具页对象；其它大类一律返回 NULL。 */
static u8* items_page(void) {
    u8* i = items_interface();
    u8* page;

    if (!Runtime_PtrOk(i)) return NULL;
    if (*(i32*)(i + INTERFACE_STATE) != 2) return NULL;
    if (*(i32*)(i + INTERFACE_CLOSE_STATE) != 0) return NULL;

    page = *(u8**)(i + INTERFACE_PAGE_2);
    return Runtime_PtrOk(page) ? page : NULL;
}

/*
 * 镜像原版 0x431380 ButtonEvent 的两个最重要可用条件。
 * +0x45==0 或 +0x04!=0 时，原版自己会拒绝这个按钮；手柄必须同样拒绝。
 */
static int items_button_usable(void* button) {
    u8* b = (u8*)button;
    if (!Runtime_PtrOk(b)) return 0;
    if (*(u8*)(b + 0x45u) == 0) return 0;
    if (*(u8*)(b + 0x04u) != 0) return 0;
    return 1;
}

/* 取得 state2 的列表对象。 */
static u8* items_list(u8* page) {
    u8* list;
    if (!Runtime_PtrOk(page)) return NULL;
    list = *(u8**)(page + INTERFACE_ITEMS_LIST);
    return Runtime_PtrOk(list) ? list : NULL;
}

/* 取得某一行的真实 Button；行记录不可见/无效时也视为没有按钮。 */
static void* items_row_button(u8* page, int row) {
    u8* list = items_list(page);
    u8* record;
    void* button;

    if (!Runtime_PtrOk(list)) return NULL;
    if (row < 0 || row >= INTERFACE_ITEMS_ROW_COUNT) return NULL;

    record = *(u8**)(list + INTERFACE_ITEMS_ROW_RECORD0 + (u32)row * 4u);
    if (!Runtime_PtrOk(record)) return NULL;
    if (*(u8*)(record + INTERFACE_ITEMS_RECORD_ACTIVE) == 0) return NULL;

    button = *(void**)(list + INTERFACE_ITEMS_ROW_BUTTON0 + (u32)row * 4u);
    return items_button_usable(button) ? button : NULL;
}

/*
 * 返回当前页“最后一个真实可操作行 + 1”。
 * 例如只有 3 条物品时返回 3；完全没有可操作物品时返回 0。
 */
static int items_visible_row_count(u8* page) {
    int row;
    int count = 0;

    for (row = 0; row < INTERFACE_ITEMS_ROW_COUNT; ++row) {
        if (items_row_button(page, row)) count = row + 1;
    }
    return count;
}

/* 取得 6 个子类型中的真实 Button。 */
static void* items_subtype_button(u8* page, int index) {
    u8* panel;
    void* button;

    if (!Runtime_PtrOk(page)) return NULL;
    if (index < 0 || index >= INTERFACE_ITEMS_SUBTYPE_COUNT) return NULL;

    panel = *(u8**)(page + INTERFACE_ITEMS_SUBPANEL);
    if (!Runtime_PtrOk(panel)) return NULL;

    button = *(void**)(panel + INTERFACE_ITEMS_SUB_BUTTON0 + (u32)index * 4u);
    return items_button_usable(button) ? button : NULL;
}

/*
 * 排一个属于 Interface 的原版 ButtonEvent。
 * 这里统一做三道门：真实 Button、原版可用、全局桥当前没有其它 pending。
 */
static int items_request_event(void* button, int code, const char* log_text) {
    if (!items_button_usable(button)) return 0;
    if (UiBridge_EventPending()) return 0;

    Cursor_ClaimForControllerNavigation();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, code, log_text);
    return UiBridge_EventOwner() == UI_EVENT_OWNER_INTERFACE;
}

/* 返回当前活动的页面专属弹窗；主操作弹窗优先于特殊操作弹窗。 */
static u8* items_active_popup(void) {
    u8* page = items_page();
    u8* popup;

    if (!Runtime_PtrOk(page)) return NULL;

    popup = *(u8**)(page + INTERFACE_ITEMS_POPUP_PRIMARY);
    if (Runtime_PtrOk(popup) && *(u8*)(popup + POPUP_ACTIVE) != 0) return popup;

    popup = *(u8**)(page + INTERFACE_ITEMS_POPUP_SECONDARY);
    if (Runtime_PtrOk(popup) && *(u8*)(popup + POPUP_ACTIVE) != 0) return popup;

    return NULL;
}

int InterfaceItems_AnyPopupActive(void) {
    return Runtime_PtrOk(items_active_popup());
}

/* 新弹窗出现时只“观察”原版选择，不立即强制任何 HitTest。 */
static void items_sync_popup_owner(u8* popup) {
    i32 native_focus;

    if ((void*)popup == g_items.popup_owner) return;

    g_items.popup_owner = popup;
    g_items.popup_nav_active = 0;

    /*
     * 安全原则：原版构造函数会先把 +0x58C 写成 1，但用户实机已经证明弹窗真正显示出来时
     * 默认视觉焦点是“取消”。如果这里直接相信构造初值，就会产生“画面在取消，A 却执行确定”的危险窗口。
     * 所以新弹窗第一次出现时先把手柄逻辑焦点放在第二个/取消；随后 UiBridge 会把原版真实
     * HitTest 结果回报给 InterfaceItems_ObservePopupHit()，再把逻辑焦点同步到屏幕真正高亮的按钮。
     */
    g_items.popup_focus = 1;

    if (!Runtime_PtrOk(popup)) return;

    /*
     * 这里只把已经稳定为 0 的原版选择当作额外证据；构造初值 1 不再被当作“确定已高亮”。
     * 这样即使 worker 比原版第一帧 HitTest 更快，A 也只会取消，不会误删物品。
     */
    native_focus = *(i32*)(popup + POPUP_NATIVE_SELECTION);
    if (native_focus == 0) g_items.popup_focus = 1;
}

/* 弹窗手柄导航开始后，取得光标所有权并打开本页 HitTest 强制视觉。 */
static void items_claim_popup_navigation(void) {
    Cursor_ClaimForControllerNavigation();
    g_items.popup_nav_active = 1;
}

/*
 * 处理 state2 自己产生的双按钮弹窗。
 * 这不是“通用 Yes/No”：只有 child+0x5AC/+0x5B0 当前 active 时才会执行。
 */
static void items_update_popup(u8* popup) {
    void* button;
    int moved = 0;

    items_sync_popup_owner(popup);

    /*
     * 上/左选择第一个；下/右选择第二个。四方向都在本页弹窗内消费，绝不穿透去翻页/切大类。
     *
     * refactor26a 关键修复：
     * refactor25 时代 state2 的 D-Pad 左右属于 PASS，所以这里从 COMMON 通道读取没有问题；
     * refactor26 把 D-Pad 左右改成“页面专属翻页”，策略随之变成 OVERRIDE，但这里漏同步，
     * 仍然读取 COMMON。根据 InputRouter 的四态规则，OVERRIDE 动作只会进入 OVERLAY，
     * 于是弃置确认框虽然存在，左右键却永远到不了这一段。
     *
     * 现在弹窗作为当前最深层 Context，四方向全部从 OVERLAY 读取；随后本函数统一 Consume，
     * 所以弹窗打开时左右只切“是/否”，绝不会继续穿透成底层法宝翻页。
     */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_UP, INPUT_LAYER_OVERLAY) ||
        InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        items_claim_popup_navigation();
        g_items.popup_focus = 0;
        moved = 1;
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY) ||
        InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        items_claim_popup_navigation();
        g_items.popup_focus = 1;
        moved = 1;
    }
    if (moved) {
        Runtime_Log(g_items.popup_focus == 0 ?
                    "[法宝页] 弹窗焦点：第一个（确定/是）。" :
                    "[法宝页] 弹窗焦点：第二个（取消/否）。");
    }

    /* A 点击当前逻辑焦点对应的真实按钮。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        items_claim_popup_navigation();
        button = *(void**)(popup + (g_items.popup_focus == 0 ? POPUP_BUTTON_YES : POPUP_BUTTON_NO));
        items_request_event(button, 2, g_items.popup_focus == 0 ?
                            "[法宝页] A：确认弹窗第一个原版按钮。" :
                            "[法宝页] A：确认弹窗第二个原版按钮。");
    }

    /* B 永远是页面弹窗逃生键：先清本页旧 pending，再强制点击第二个/取消按钮。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        items_claim_popup_navigation();
        g_items.popup_focus = 1;
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
        button = *(void**)(popup + POPUP_BUTTON_NO);
        items_request_event(button, 2, "[法宝页] B：执行本页弹窗原版取消按钮。");
    }

    /*
     * 弹窗是最深层 Context。下面这些动作即使本 tick 没触发业务，也不能再传到 InterfaceShell：
     * 否则左/右会换角色、LB/RB 会换大类，造成“弹窗还在，上层菜单却动了”。
     */
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

void InterfaceItems_ObservePopupHit(void* button, u8 hit_value) {
    u8* popup = items_active_popup();
    void* first_button;
    void* second_button;

    /* 手柄已经主动按方向后，视觉由本页逻辑焦点接管，不再让鼠标 HitTest 反向改写焦点。 */
    if (g_items.popup_nav_active) return;
    if (!Runtime_PtrOk(popup) || !Runtime_PtrOk(button)) return;

    items_sync_popup_owner(popup);
    first_button = *(void**)(popup + POPUP_BUTTON_YES);
    second_button = *(void**)(popup + POPUP_BUTTON_NO);

    /*
     * 原版每帧会分别 HitTest 两个按钮。只有返回非 0 的那个才是屏幕当前真正命中的按钮。
     * 我们只观察，不修改返回值，因此实体鼠标行为完全保持原版。
     */
    if (hit_value != 0 && button == first_button) g_items.popup_focus = 0;
    if (hit_value != 0 && button == second_button) g_items.popup_focus = 1;
}

u8 InterfaceItems_FilterPopupHit(void* button, int* handled) {
    u8* popup = items_active_popup();
    void* first_button;
    void* second_button;

    if (handled) *handled = 0;
    if (!g_items.enabled || !g_items.popup_nav_active) return 0;
    if (!Runtime_PtrOk(popup) || !Runtime_PtrOk(button)) return 0;
    if ((void*)popup != g_items.popup_owner) return 0;

    first_button = *(void**)(popup + POPUP_BUTTON_YES);
    second_button = *(void**)(popup + POPUP_BUTTON_NO);

    if (button == first_button) {
        if (handled) *handled = 1;
        return (u8)(g_items.popup_focus == 0);
    }
    if (button == second_button) {
        if (handled) *handled = 1;
        return (u8)(g_items.popup_focus == 1);
    }
    return 0;
}

/* 切换子类型。只点目标真实 Button，不写 child+0x594。 */
static void items_change_subtype(u8* page, int direction) {
    int current;
    int target = -1;
    int index;
    int current_x;
    int candidate_x;
    int best_x;
    void* current_button;
    void* candidate;

    if (!Runtime_PtrOk(page) || direction == 0) return;
    if (UiBridge_EventPending()) return;

    current = *(i32*)(page + INTERFACE_ITEMS_SUBINDEX);
    if (current < 0 || current >= INTERFACE_ITEMS_SUBTYPE_COUNT) current = 0;

    current_button = items_subtype_button(page, current);
    if (!Runtime_PtrOk(current_button)) return;
    current_x = *(i32*)((u8*)current_button + 0x34u);

    /*
     * 绝不再用“内部索引 +1 就等于屏幕向右”这种假设。用户已经实机证明 state2 的内部顺序
     * 与视觉左右相反。这里直接读取原版 Button+0x34 的 X 坐标：direction<0 永远找左边，
     * direction>0 永远找右边。这样 LT/RT 的语义由画面位置决定，不会再次被数组顺序反转。
     */
    best_x = direction < 0 ? (-2147483647 - 1) : 2147483647;
    for (index = 0; index < INTERFACE_ITEMS_SUBTYPE_COUNT; ++index) {
        if (index == current) continue;
        candidate = items_subtype_button(page, index);
        if (!Runtime_PtrOk(candidate)) continue;
        candidate_x = *(i32*)((u8*)candidate + 0x34u);

        if (direction < 0 && candidate_x < current_x && candidate_x > best_x) {
            best_x = candidate_x;
            target = index;
        }
        if (direction > 0 && candidate_x > current_x && candidate_x < best_x) {
            best_x = candidate_x;
            target = index;
        }
    }

    /* 到最左/最右后按“上一/下一”循环：LT 包到最右，RT 包到最左。 */
    if (target < 0) {
        best_x = direction < 0 ? (-2147483647 - 1) : 2147483647;
        for (index = 0; index < INTERFACE_ITEMS_SUBTYPE_COUNT; ++index) {
            candidate = items_subtype_button(page, index);
            if (!Runtime_PtrOk(candidate)) continue;
            candidate_x = *(i32*)((u8*)candidate + 0x34u);
            if (direction < 0 && candidate_x > best_x) { best_x = candidate_x; target = index; }
            if (direction > 0 && candidate_x < best_x) { best_x = candidate_x; target = index; }
        }
    }

    candidate = target >= 0 ? items_subtype_button(page, target) : NULL;
    if (items_button_usable(candidate)) {
        items_request_event(candidate, 2, direction < 0 ?
                            "[法宝页] LT：请求视觉左侧/上一个子类型。" :
                            "[法宝页] RT：请求视觉右侧/下一个子类型。");
    }
}

/*
 * 点击原版上一页/下一页，并只在本地记住“翻页后要把焦点落在哪里”。
 * 页码本身完全由 0x43820E/0x438274 后面的原版代码修改。
 */
static void items_request_page(u8* page, int direction) {
    u8* list = items_list(page);
    int current_page;
    int total_pages;
    void* button;

    if (!Runtime_PtrOk(list) || direction == 0) return;
    if (g_items.page_wait_direction != 0 || UiBridge_EventPending()) return;

    current_page = *(i32*)(page + INTERFACE_ITEMS_CURRENT_PAGE);
    total_pages = *(i32*)(page + INTERFACE_ITEMS_TOTAL_PAGES);
    if (total_pages < 1) total_pages = 1;

    if (direction < 0) {
        if (current_page <= 0) return;
        button = *(void**)(list + INTERFACE_ITEMS_PAGE_PREV_BUTTON);
    } else {
        if (current_page + 1 >= total_pages) return;
        button = *(void**)(list + INTERFACE_ITEMS_PAGE_NEXT_BUTTON);
    }

    if (!items_button_usable(button)) return;

    g_items.page_wait_direction = direction < 0 ? -1 : 1;
    g_items.page_before_request = current_page;
    if (!items_request_event(button, 2, direction < 0 ?
                             "[法宝页] 请求原版上一页。" :
                             "[法宝页] 请求原版下一页。")) {
        /* 没真正排进去就立即撤销本地等待，避免页面永远以为自己正在翻页。 */
        g_items.page_wait_direction = 0;
        g_items.page_before_request = -1;
    }
}

/*
 * 原版翻页真正生效以后再执行“新页落点”。
 * - 下一页：落到第 0 行；
 * - 上一页：落到该页最后一条真实可用行。
 */
static void items_finish_page_landing(u8* page) {
    int current_page;
    int row_count;
    int target_row;
    void* button;

    if (!Runtime_PtrOk(page) || g_items.page_wait_direction == 0) return;

    current_page = *(i32*)(page + INTERFACE_ITEMS_CURRENT_PAGE);
    if (current_page == g_items.page_before_request) return;

    row_count = items_visible_row_count(page);
    if (row_count <= 0) {
        g_items.page_wait_direction = 0;
        g_items.page_before_request = -1;
        g_items.last_page_seen = current_page;
        return;
    }

    target_row = g_items.page_wait_direction > 0 ? 0 : row_count - 1;
    g_items.page_wait_direction = 0;
    g_items.page_before_request = -1;
    g_items.last_page_seen = current_page;

    button = items_row_button(page, target_row);
    if (items_button_usable(button)) {
        items_request_event(button, 2, "[法宝页] 原版翻页完成：恢复新页手柄焦点。");
    }
}

/* 上下在 8 行中移动；到页面边界时继续走原版 Prev/Next。 */
static void items_move_vertical(u8* page, int direction) {
    int current_row;
    int row_count;
    int target_row;
    void* button;

    if (!Runtime_PtrOk(page) || direction == 0) return;
    if (g_items.page_wait_direction != 0 || UiBridge_EventPending()) return;

    row_count = items_visible_row_count(page);
    if (row_count <= 0) return;

    current_row = *(i32*)(page + INTERFACE_ITEMS_CURRENT_ROW);
    if (current_row < 0 || current_row >= row_count) current_row = 0;

    target_row = current_row + (direction < 0 ? -1 : 1);
    if (target_row < 0) {
        items_request_page(page, -1);
        return;
    }
    if (target_row >= row_count) {
        items_request_page(page, 1);
        return;
    }

    button = items_row_button(page, target_row);
    if (items_button_usable(button)) {
        items_request_event(button, 2, direction < 0 ?
                            "[法宝页] ↑：请求原版上一条物品。" :
                            "[法宝页] ↓：请求原版下一条物品。");
    }
}

/* A 再次点击当前行；原版自己决定这是选择、使用，还是打开本页确认弹窗。 */
static void items_confirm_current_row(u8* page) {
    int row;
    void* button;

    if (!Runtime_PtrOk(page) || g_items.page_wait_direction != 0) return;
    if (UiBridge_EventPending()) return;

    row = *(i32*)(page + INTERFACE_ITEMS_CURRENT_ROW);
    button = items_row_button(page, row);
    if (!items_button_usable(button)) {
        Runtime_Log("[法宝页] A：当前物品不可用/不存在，本次确认已忽略。");
        return;
    }

    items_request_event(button, 2, "[法宝页] A：执行当前物品的原版行事件。");
}

/*
 * X 不根据“暂置/杂类/饰物……”的中文名称猜内部 index。
 * 原版在 0x4380D0 自己动态设置两个真实特殊 Button 的 +0x45：
 * - +0x5E8 当前可用时，截图对应“分发”；
 * - 否则 +0x5EC 当前可用时，截图对应“弃置”；
 * - 两个都不可用就什么也不做。
 */
static void items_special_x(u8* page) {
    u8* list = items_list(page);
    void* distribute;
    void* discard;

    if (!Runtime_PtrOk(list) || UiBridge_EventPending()) return;

    distribute = *(void**)(list + INTERFACE_ITEMS_SPECIAL_BUTTON_A);
    if (items_button_usable(distribute)) {
        items_request_event(distribute, 2, "[法宝页] X：执行原版“分发”按钮。");
        return;
    }

    discard = *(void**)(list + INTERFACE_ITEMS_SPECIAL_BUTTON_B);
    if (items_button_usable(discard)) {
        items_request_event(discard, 2, "[法宝页] X：执行原版“弃置”按钮。");
        return;
    }

    Runtime_Log("[法宝页] X：当前子类型没有可用的分发/弃置按钮，本次输入已忽略。");
}

/* 离开 state2 或实体指针接管时，所有“只属于手柄”的瞬态状态都必须清空。 */
static void items_reset_transient(void) {
    g_items.page_wait_direction = 0;
    g_items.page_before_request = -1;
    g_items.last_page_seen = -1;
    g_items.last_subtype_seen = -1;
    g_items.popup_focus = 0;
    g_items.popup_nav_active = 0;
    g_items.popup_owner = NULL;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
}

void InterfaceItems_OnPointerTakeover(CursorTakeoverEvent event) {
    if (event == CURSOR_TAKEOVER_NONE) return;

    /* 指针接管只撤销本页的手柄视觉/等待事务；原版页面和真实鼠标状态完全不碰。 */
    g_items.popup_nav_active = 0;
    g_items.popup_owner = NULL;
    g_items.page_wait_direction = 0;
    g_items.page_before_request = -1;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
}

void InterfaceItems_Update(void) {
    u8* page;
    u8* popup;
    int subtype;
    int current_page;

    if (!g_items.enabled) return;

    page = items_page();
    if (!Runtime_PtrOk(page)) {
        if (g_items.was_active) {
            items_reset_transient();
            g_items.was_active = 0;
            Runtime_Log("[法宝页] 已离开 state2；页面专属手柄状态已清理。");
        }
        return;
    }

    if (!g_items.was_active) {
        items_reset_transient();
        g_items.was_active = 1;
        g_items.last_subtype_seen = *(i32*)(page + INTERFACE_ITEMS_SUBINDEX);
        g_items.last_page_seen = *(i32*)(page + INTERFACE_ITEMS_CURRENT_PAGE);
        Runtime_Log("[法宝页] 已进入 state2；LT/RT子类型、上下列表、左右翻页、A、X 页面 Adapter 开始工作。");
    }

    /* 页面自己的弹窗拥有最高优先级；出现时不再执行列表/子类型业务。 */
    popup = items_active_popup();
    if (Runtime_PtrOk(popup)) {
        items_update_popup(popup);
        return;
    }

    /* 弹窗刚关闭时撤销强制 HitTest，恢复普通页面焦点。 */
    g_items.popup_owner = NULL;
    g_items.popup_nav_active = 0;

    subtype = *(i32*)(page + INTERFACE_ITEMS_SUBINDEX);
    current_page = *(i32*)(page + INTERFACE_ITEMS_CURRENT_PAGE);

    /* 原版子类型变化会重建列表；旧翻页等待不允许跨子类型残留。 */
    if (g_items.last_subtype_seen != subtype) {
        g_items.last_subtype_seen = subtype;
        g_items.last_page_seen = current_page;
        g_items.page_wait_direction = 0;
        g_items.page_before_request = -1;
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
    }

    items_finish_page_landing(page);

    /* LT/RT 是 state2 的页面专属子类型切换。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_SUBTYPE_PREV, INPUT_LAYER_OVERLAY)) {
        items_change_subtype(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_SUBTYPE_NEXT, INPUT_LAYER_OVERLAY)) {
        items_change_subtype(page, 1);
    }

    /* ↑/↓ 在物品列表中移动，并在上下边界自动翻原版页面。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        items_move_vertical(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        items_move_vertical(page, 1);
    }

    /* refactor26：D-Pad ←/→ 专门翻页；角色切换已经迁到左摇杆水平 50%。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        items_request_page(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        items_request_page(page, 1);
    }

    /* A 当前物品；X 当前子类型原版特殊按钮。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        items_confirm_current_row(page);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_SPECIAL_X, INPUT_LAYER_OVERLAY)) {
        items_special_x(page);
    }

    /* 本页真正拥有的动作必须消费，防止同一 tick 被别的模块重复解释。 */
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_SPECIAL_X);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
    InputRouter_Consume(INPUT_SUBTYPE_PREV);
    InputRouter_Consume(INPUT_SUBTYPE_NEXT);

    /*
     * B、LB/RB 故意不消费：
     * - B 继续由 Shell 在根层退出主菜单；
     * - LB/RB 继续切八大类。
     * 角色切换已迁到左摇杆水平 50%，不再占用 D-Pad。
     */
}

int InterfaceItems_InstallHooks(void) {
    g_items.enabled = 0;
    g_items.was_active = 0;
    items_reset_transient();

    if (!Runtime_InterfaceItemsProtocolOk()) {
        Runtime_Log("[法宝页] 原版 state2 协议不匹配；本页 Adapter 已 fail-closed，r19 Shell 仍继续工作。");
        return 1;
    }

    if (!UiBridge_InstallInterfaceItemsHooks()) {
        Runtime_Log("[法宝页] state2 ButtonEvent Hook 安装失败；拒绝继续启用本页 Adapter。");
        return 0;
    }

    g_items.enabled = 1;
    Runtime_Log("[法宝页] state2 页面专属 Adapter 已启用：LT/RT 子类型、上下列表、左右翻页、A、X、页面弹窗。");
    return 1;
}
