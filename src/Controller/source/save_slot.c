#include "save_slot.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"
#include "ui_bridge.h"

/*
 * SaveSlotState 只保存“这一份槽位 UI”的状态。
 * 标题菜单自己的 0/1/2 焦点完全不在这里，避免以后复用到游戏内时出现 owner-specific 分支。
 */
typedef struct SaveSlotState {
    u8* object;
    int nav_active;
    int focus;

    volatile int visual_apply_pending;
    int last_page_base;
    int page_wait_dir;       /* -1=等待上一页；+1=等待下一页；0=没有分页事务。 */
    u32 page_wait_tick;

    volatile int row_confirm_pending; /* -1=无；0..3=等待对应行的原版 Event 调用点。 */
    volatile int page_pending;        /* -1/0/+1：下一次分页 Event 要注入的方向。 */
    volatile int cancel_pending;

    int popup_focus;         /* 0=第一个按钮；1=第二个按钮。 */
    int popup_nav_active;

    /*
     * 天书专用的三项动作窗口仍属于 SaveSlot 自己，所以状态放在同一个控制器里。
     * action_focus 保存“真实原版 Button index”，不是视觉顺序：
     * index2=最上、index1=中间、index0=最下；index0 是不弹 Yes/No 的取消项。
     */
    int action_focus;
    int action_nav_active;
    int action_hooks_enabled; /* 共享三项窗口协议/Hook 是否独立通过。 */
    volatile int action_confirm_pending;

    /*
     * 游戏线程发布的三项按钮可用状态。
     *
     * 低三位分别对应index0取消、index1读档、index2存档；最高位表示“本数值已经由
     * SaveAction::Update里的真实Hit/Event Hook完整扫描过”。把valid和mask合并进同一个
     * 32位整数，只需要一次对齐写入，worker就不会读到“valid已更新但mask还是上一帧”的半状态。
     */
    volatile u32 action_allowed_state;
    SaveSlotView last_view;
} SaveSlotState;

static SaveSlotState g_save;

/* 三个按钮正好使用低三位；最高位单独充当跨线程发布完成标记。 */
#define SAVE_ACTION_ALLOWED_MASK_ALL ((1u << SAVE_ACTION_COUNT) - 1u)
#define SAVE_ACTION_ALLOWED_STATE_VALID 0x80000000u

/* 复刻原版 ButtonEvent 的可用条件；不可用按钮只让原版处理，手柄不硬注入。 */
static int save_button_accepts_event(void* button) {
    u8* b = (u8*)button;
    if (!Runtime_PtrOk(b)) return 0;
    /* 0x431380 的两个已确认早退条件。满足它们才值得排队手柄事件。 */
    if (*(u8*)(b + 0x45u) == 0) return 0;
    if (*(u8*)(b + 0x04u) != 0) return 0;
    return 1;
}

/* 从 SaveSlot 的 4 个行对象中再取真正的内层 Button；所有后续选择都以这个真实 Button 为准。 */
static void* save_row_button(u8* s, int row) {
    u8* line;
    void* button;
    if (!s || row < 0 || row >= SAVE_ROW_COUNT) return NULL;
    line = *(u8**)(s + SAVE_ROWS_BASE + (u32)row * 4u);
    if (!Runtime_PtrOk(line)) return NULL;
    button = *(void**)(line + SAVE_ROW_INNER_BUTTON);
    return Runtime_PtrOk(button) ? button : NULL;
}

static int save_row_available(u8* s, int row) {
    /*
     * dev20 的重要行为：只要这一行真实存在，就允许手柄聚焦。
     * +0x57C 是否已有存档数据不能用于过滤，否则空槽将无法像原版鼠标那样选择。
     */
    return save_row_button(s, row) != NULL;
}

/* 找当前页第一个真实存在的显示槽；不检查“是否已有存档”。 */
static int save_first_row(u8* s) {
    int i;
    for (i = 0; i < SAVE_ROW_COUNT; ++i) {
        if (save_row_available(s, i)) return i;
    }
    return -1;
}

/* 找当前页最后一个真实存在的显示槽，用于上一页完成后的边界落点。 */
static int save_last_row(u8* s) {
    int i;
    for (i = SAVE_ROW_COUNT - 1; i >= 0; --i) {
        if (save_row_available(s, i)) return i;
    }
    return -1;
}

/* 在当前页按方向寻找下一条存在的行；找不到就保持原索引，让调用者决定是否触发分页。 */
static int save_move_row(u8* s, int current, int dir) {
    int i = current + dir;
    while (i >= 0 && i < SAVE_ROW_COUNT) {
        if (save_row_available(s, i)) return i;
        i += dir;
    }
    return current;
}

/* 识别 SaveSlot 自己直接挂载且 ACTIVE 的历史确认框。 */
static u8* save_popup(u8* s) {
    u8* p;
    if (!s) return NULL;
    p = *(u8**)(s + SAVE_CONFIRM_POPUP);
    if (!Runtime_PtrOk(p) || *(u8*)(p + POPUP_ACTIVE) == 0) return NULL;
    return p;
}

/*
 * 这个所有权判断必须基于共享控制器当前绑定的 g_save.object，
 * 不能再像旧 ConfirmDialog 那样只从 GLOBAL_TITLE_UI 猜“SaveSlot 只会出现在标题”。
 * refactor31 已经证明至少有标题、Interface state7、0x89FCD0 独立包装层三种 owner。
 */
int SaveSlot_OwnsDirectPopup(void* popup) {
    u8* current;

    if (!Runtime_PtrOk(popup) || !Runtime_PtrOk(g_save.object)) return 0;
    current = save_popup(g_save.object);
    return Runtime_PtrOk(current) && current == popup;
}

/*
 * 取得 SaveSlot 内嵌的三项动作窗口。
 * 0x4245B0 构造函数无论标题/游戏内都会创建这个对象；只有 +0x579 真正置 1 时才说明窗口当前展开。
 */
static u8* save_action(u8* s) {
    u8* a;
    if (!Runtime_PtrOk(s)) return NULL;
    a = *(u8**)(s + SAVE_ACTION_UI);
    if (!Runtime_PtrOk(a) || *(u8*)(a + SAVE_ACTION_ACTIVE) == 0) return NULL;
    return a;
}

/* 取得三项动作窗口里的第二层原版 Yes/No；它由 ConfirmDialog 处理，不在 SaveSlot 内再复制一套。 */
static u8* save_action_popup(u8* s) {
    u8* a = save_action(s);
    u8* p;
    if (!a) return NULL;
    p = *(u8**)(a + SAVE_ACTION_POPUP);
    if (!Runtime_PtrOk(p) || *(u8*)(p + POPUP_ACTIVE) == 0) return NULL;
    return p;
}

/* 按真实内存 index 取得“存档/读档/取消”三只原版 Button。 */
static void* save_action_button(u8* s, int index) {
    u8* a = save_action(s);
    void* button;
    if (!a || index < 0 || index >= SAVE_ACTION_COUNT) return NULL;
    button = *(void**)(a + SAVE_ACTION_BUTTONS + (u32)index * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/*
 * 只在游戏线程的SaveAction Hit/Event Hook中调用：一次扫描三只真实Button，生成低三位掩码。
 * 这里复用save_button_accepts_event()，所以判据和原版0x431380完全相同：+0x45非0且+0x04为0。
 * 不询问是谁改了disabled，也不认识保留槽号；任何插件或游戏本体造成的原生状态都会得到同样结果。
 */
static u32 save_action_collect_allowed_mask(u8* s) {
    u32 mask = 0u;
    int index;

    for (index = 0; index < SAVE_ACTION_COUNT; ++index) {
        void* button = save_action_button(s, index);
        if (save_button_accepts_event(button)) mask |= 1u << (u32)index;
    }
    return mask;
}

/* worker只读取一次32位发布值；最高位没置上时，说明游戏线程尚未完成本窗口的第一次扫描。 */
static int save_action_read_allowed_mask(u32* out_mask) {
    u32 state;

    if (!out_mask) return 0;
    state = g_save.action_allowed_state;
    if ((state & SAVE_ACTION_ALLOWED_STATE_VALID) == 0u) return 0;
    *out_mask = state & SAVE_ACTION_ALLOWED_MASK_ALL;
    return 1;
}

/* 单独封装位判断，避免每个导航分支重复写移位表达式并忘记范围检查。 */
static int save_action_focus_allowed(u32 mask, int focus) {
    if (focus < 0 || focus >= SAVE_ACTION_COUNT) return 0;
    return (mask & (1u << (u32)focus)) != 0u;
}

/*
 * 当前项被禁用时，选择数字距离最近的可用index。
 * 原版屏幕顺序是2→1→0，所以index差的绝对值也正好等于相隔几行。
 * 循环从小index开始，只有距离严格更小时才替换；因此同距离自然保留较小index，
 * 也就是需求指定的屏幕下方项目。
 */
static int save_action_nearest_allowed(u32 mask, int from_focus) {
    int best = -1;
    int best_distance = 999;
    int index;

    for (index = 0; index < SAVE_ACTION_COUNT; ++index) {
        int distance;
        if (!save_action_focus_allowed(mask, index)) continue;
        distance = index - from_focus;
        if (distance < 0) distance = -distance;
        if (distance < best_distance) {
            best = index;
            best_distance = distance;
        }
    }
    return best;
}

/* 当前焦点仍可用就不动；不可用时才迁移。返回1表示焦点真的发生了变化。 */
static int save_action_normalize_focus(u32 mask, int fallback_focus) {
    int old_focus = g_save.action_focus;
    int origin = old_focus;
    int next;

    if (save_action_focus_allowed(mask, old_focus)) return 0;
    if (origin < 0 || origin >= SAVE_ACTION_COUNT) origin = fallback_focus;

    next = save_action_nearest_allowed(mask, origin);
    if (next < 0) return 0;
    g_save.action_focus = next;
    return next != old_focus;
}

/*
 * 游戏线程把完整mask作为一次32位状态发布，并立即修正强制焦点。
 *
 * 为什么在Hook里也做焦点归一化：外部插件可能只在原版SaveAction::Update调用期间临时设置disabled，
 * worker在Update外重新读Button会错过状态；而原版本帧马上就会根据HitTest画高亮。游戏线程先修正后，
 * 保留槽第一次展开就能直接高亮“读档”，不会先画一个空焦点再等下一次方向输入。
 */
static void save_action_publish_allowed_mask(u8* s) {
    u32 old_state = g_save.action_allowed_state;
    u32 old_mask = old_state & SAVE_ACTION_ALLOWED_MASK_ALL;
    u32 mask = save_action_collect_allowed_mask(s);
    int state_changed =
        (old_state & SAVE_ACTION_ALLOWED_STATE_VALID) == 0u || old_mask != mask;
    int focus_changed = 0;
    int pending = g_save.action_confirm_pending;

    /* 对齐32位单次写入就是发布点；worker只在看到VALID后使用低三位。 */
    g_save.action_allowed_state = SAVE_ACTION_ALLOWED_STATE_VALID | mask;

    /* pending指向的按钮一旦不再可用，立刻作废，禁止按钮恢复后补发旧确认。 */
    if (pending >= 0 && !save_action_focus_allowed(mask, pending)) {
        g_save.action_confirm_pending = -1;
        if (state_changed) Runtime_Log("[存读档动作] 已拒绝指向禁用按钮的陈旧确认。");
    }

    if (mask == 0u) {
        /* 没有任何可用项时必须退出强制HitTest，让原版鼠标逻辑自行处理，不能伪造一条高亮。 */
        g_save.action_nav_active = 0;
        if (state_changed) Runtime_Log("[存读档动作] 当前没有可用按钮，停止强制焦点。");
        return;
    }

    /* 从手柄槽位层刚进入Action时默认仍从index2存档开始，再按mask向最近项归一化。 */
    if (!g_save.action_nav_active && g_save.nav_active) {
        g_save.action_focus = 2;
        g_save.action_nav_active = 1;
        g_save.action_confirm_pending = -1;
    }

    if (g_save.action_nav_active) {
        focus_changed = save_action_normalize_focus(mask, 2);
    }

    if (state_changed && focus_changed) {
        Runtime_Log(g_save.action_focus == 1
            ? "[存读档动作] 当前焦点按钮已禁用，自动迁移到读档。"
            : "[存读档动作] 当前焦点按钮已禁用，自动迁移到最近可用按钮。");
    }
}

/*
 * 按屏幕方向寻找下一项：上就是index递增，下就是index递减。
 * while会跳过任意数量的disabled；到边界仍找不到时返回原焦点，不循环、不制造空焦点。
 */
static int save_action_move_allowed(u32 mask, int current, int direction) {
    int candidate = current + direction;

    while (candidate >= 0 && candidate < SAVE_ACTION_COUNT) {
        if (save_action_focus_allowed(mask, candidate)) return candidate;
        candidate += direction;
    }
    return current;
}

/*
 * SaveSlot 现在暴露五种明确视图。识别顺序必须从最深子层往外：
 * action popup > action > direct popup > slots。
 * 这样 worker 永远不会在一个模态窗口已经打开时继续把 ↑↓ 当槽位导航。
 */
SaveSlotView SaveSlot_DetectView(u8* save_slot) {
    if (!Runtime_PtrOk(save_slot) || *(u8*)(save_slot + SAVE_ACTIVE) == 0) return SAVE_VIEW_NONE;
    if (save_action_popup(save_slot)) return SAVE_VIEW_ACTION_POPUP;
    if (save_action(save_slot)) return SAVE_VIEW_ACTION;
    if (save_popup(save_slot)) return SAVE_VIEW_POPUP;
    return SAVE_VIEW_SLOTS;
}

/*
 * worker 线程只登记“想选哪一行”，不直接在 worker 写游戏视觉字段。
 * 真正 +0x5BC/+0x5C0/+0x594 与 refresh 会等到游戏线程的 0x424BE2 安全点执行。
 */
static void save_request_native_selection(u8* s, int row) {
    if (!s || row < 0 || row >= SAVE_ROW_COUNT) return;
    g_save.focus = row;
    g_save.object = s;
    g_save.visual_apply_pending = 1;
}

static void save_claim_navigation(void) {
    Cursor_ClaimForControllerNavigation();
    g_save.nav_active = 1;
}

/*
 * 新接管一份 SaveSlot 时清空所有旧 pending，并记录当前页。
 * from_pad 只决定是否立即取得手柄导航所有权；鼠标主动进入读档不会被插件抢焦点。
 */
void SaveSlot_Begin(u8* save_slot, int from_pad) {
    int first;
    g_save.object = save_slot;
    g_save.nav_active = from_pad ? 1 : 0;
    g_save.popup_nav_active = 0;
    g_save.action_nav_active = 0;
    g_save.action_focus = 2;
    g_save.action_confirm_pending = -1;
    g_save.action_allowed_state = 0u;
    g_save.last_view = SAVE_VIEW_SLOTS;
    g_save.row_confirm_pending = -1;
    g_save.page_pending = 0;
    g_save.cancel_pending = 0;
    g_save.page_wait_dir = 0;
    g_save.page_wait_tick = 0;
    g_save.last_page_base = Runtime_PtrOk(save_slot) ? *(i32*)(save_slot + SAVE_PAGE_BASE) : 0;

    first = save_first_row(save_slot);
    g_save.focus = first >= 0 ? first : 0;
    g_save.visual_apply_pending = (from_pad && first >= 0) ? 1 : 0;

    if (from_pad) Cursor_ClaimForControllerNavigation();
    Runtime_Log(from_pad ? "[存读档] 由手柄进入 SaveSlot，启用原生槽位导航。" : "[存读档] SaveSlot 出现；保持键鼠原生控制，等待手柄输入。");
}

/* SaveSlot 离开后彻底清空对象和一次性事务，防止旧指针污染下一次读档。 */
void SaveSlot_End(void) {
    g_save.object = NULL;
    g_save.nav_active = 0;
    g_save.popup_nav_active = 0;
    g_save.action_nav_active = 0;
    g_save.action_confirm_pending = -1;
    g_save.action_allowed_state = 0u;
    g_save.last_view = SAVE_VIEW_NONE;
    g_save.visual_apply_pending = 0;
    g_save.row_confirm_pending = -1;
    g_save.page_pending = 0;
    g_save.cancel_pending = 0;
    g_save.page_wait_dir = 0;
}

/* 键鼠接管只取消“插件强制状态”，不修改原版 SaveSlot 自己的 selected row/page。 */
void SaveSlot_OnPointerTakeover(void) {
    g_save.nav_active = 0;
    g_save.popup_nav_active = 0;
    g_save.action_nav_active = 0;
    g_save.action_confirm_pending = -1;
    g_save.action_allowed_state = 0u;
    g_save.row_confirm_pending = -1;
    g_save.page_pending = 0;
    g_save.cancel_pending = 0;
    g_save.visual_apply_pending = 0;
}

int SaveSlot_IsControllerActive(void) {
    return g_save.nav_active || g_save.popup_nav_active || g_save.action_nav_active;
}

/*
 * Popup 手柄导航激活时，视觉只在两个真实按钮之间切换。
 * handled=1 只对属于当前 Popup 的按钮返回，避免覆盖战斗或其它 UI 的 HitTest。
 */
u8 SaveSlot_FilterPopupHit(void* button, int* handled) {
    u8* p;
    void* yes;
    void* no;
    if (handled) *handled = 0;
    if (!g_save.popup_nav_active || SaveSlot_DetectView(g_save.object) != SAVE_VIEW_POPUP) return 0;

    p = save_popup(g_save.object);
    if (!p) return 0;
    yes = *(void**)(p + POPUP_BUTTON_YES);
    no = *(void**)(p + POPUP_BUTTON_NO);

    if (button == yes) {
        if (handled) *handled = 1;
        return (u8)(g_save.popup_focus == 0);
    }
    if (button == no) {
        if (handled) *handled = 1;
        return (u8)(g_save.popup_focus == 1);
    }
    return 0;
}

/* ------------------------- 游戏线程专用 Hook ------------------------- */

static void FASTCALL SaveSlot_HookAnim(void* save_slot, void* unused_edx) {
    PFN_SaveSlotAnimThis orig = (PFN_SaveSlotAnimThis)FN_SAVE_SLOT_ANIM;
    u8* s = (u8*)save_slot;
    int page;
    int landing;
    (void)unused_edx;

    if (g_save.nav_active && s && s == g_save.object && SaveSlot_DetectView(s) == SAVE_VIEW_SLOTS) {
        page = *(i32*)(s + SAVE_PAGE_BASE);

        /*
         * 分页 Event 已经发出后，不猜测“应该翻页成功”。只有看到原版 +0x598 真变化，
         * 才把焦点落到新页边界。这可以保留第一页/最后一页由原版自行拒绝的行为。
         */
        if (page != g_save.last_page_base) {
            int direction = g_save.page_wait_dir;
            g_save.last_page_base = page;
            g_save.page_wait_dir = 0;
            landing = direction < 0 ? save_last_row(s) : save_first_row(s);
            if (landing >= 0) save_request_native_selection(s, landing);
            Runtime_Log(direction < 0 ? "[存读档] 原版上一页完成，焦点落到新页最后槽。" : "[存读档] 原版下一页完成，焦点落到新页第一槽。");
        } else if (g_save.page_wait_dir != 0 &&
                   (Runtime_Tick() - g_save.page_wait_tick) >= Runtime_MsToTicks(360u)) {
            g_save.page_wait_dir = 0;
            Runtime_Log("[存读档] 原版未发生页变化：已位于分页边界，保持当前页。");
        }

        if (g_save.visual_apply_pending) {
            int row = g_save.focus;
            int old_row;
            if (!save_row_available(s, row)) row = save_first_row(s);
            if (row >= 0) {
                old_row = *(i32*)(s + SAVE_SELECTED_ROW);
                g_save.focus = row;

                /*
                 * 这是 dev19 实机验证成功的“原鼠标点击前半段”。
                 * 只写 selected-row 会出现“逻辑选中了但看不到动画”的 dev18 回归，
                 * 所以旧行不同必须先设置 4 帧动画计数和目标行，再 refresh。
                 */
                if (old_row != row) {
                    *(i32*)(s + SAVE_ANIM_COUNTDOWN) = 4;
                    *(i32*)(s + SAVE_ANIM_ROW) = row;
                }
                *(i32*)(s + SAVE_SELECTED_ROW) = row;
                ((PFN_SaveSlotRefreshThis)FN_SAVE_SLOT_REFRESH)(s, 0);
            }
            g_save.visual_apply_pending = 0;
        }
    }

    /* 最后一定进入原函数，让游戏自己的 0x424F00 在同一帧推进动画。 */
    orig(save_slot);
}

/* 行 Event 先跑原版；只有原版未产生键鼠事件且 pending 恰好对应当前行时，才返回一次确认码 2。 */
static i32 FASTCALL SaveSlot_HookRowEvent(void* button, void* unused_edx) {
    PFN_ButtonEventThis orig = (PFN_ButtonEventThis)FN_BUTTON_EVENT;
    i32 real;
    int row;
    (void)unused_edx;

    real = orig(button);
    if (real != 0) {
        g_save.row_confirm_pending = -1;
        return real;
    }

    row = g_save.row_confirm_pending;
    if (row >= 0 && row < SAVE_ROW_COUNT && g_save.nav_active && SaveSlot_DetectView(g_save.object) == SAVE_VIEW_SLOTS &&
        button == save_row_button(g_save.object, row) && save_button_accepts_event(button)) {
        g_save.row_confirm_pending = -1;
        Runtime_Log("[存读档] 当前槽位确认已注入原版行按钮事件。");
        return 2;
    }
    return real;
}

/* B 的取消使用原版取消按钮 CALL，返回码 1 保持 dev20 已验证协议。 */
static i32 FASTCALL SaveSlot_HookCancelEvent(void* button, void* unused_edx) {
    PFN_ButtonEventThis orig = (PFN_ButtonEventThis)FN_BUTTON_EVENT;
    i32 real;
    (void)unused_edx;
    real = orig(button);
    if (real != 0) {
        g_save.cancel_pending = 0;
        return real;
    }
    if (g_save.cancel_pending && g_save.nav_active && SaveSlot_DetectView(g_save.object) == SAVE_VIEW_SLOTS && save_button_accepts_event(button)) {
        g_save.cancel_pending = 0;
        Runtime_Log("[存读档] B 取消已注入原版取消按钮事件。");
        return 1;
    }
    return real;
}

/* 上一页 pending 只在原版对应 Event CALL 上消费；真正是否换页仍由游戏决定。 */
static i32 FASTCALL SaveSlot_HookPagePrev(void* button, void* unused_edx) {
    PFN_ButtonEventThis orig = (PFN_ButtonEventThis)FN_BUTTON_EVENT;
    i32 real;
    (void)button;
    (void)unused_edx;
    real = orig(button);
    if (real != 0) {
        g_save.page_pending = 0;
        return real;
    }
    if (g_save.page_pending < 0 && g_save.nav_active && SaveSlot_DetectView(g_save.object) == SAVE_VIEW_SLOTS) {
        g_save.page_pending = 0;
        return 2;
    }
    return real;
}

/* 下一页与上一页完全对称；这里不直接写页码。 */
static i32 FASTCALL SaveSlot_HookPageNext(void* button, void* unused_edx) {
    PFN_ButtonEventThis orig = (PFN_ButtonEventThis)FN_BUTTON_EVENT;
    i32 real;
    (void)button;
    (void)unused_edx;
    real = orig(button);
    if (real != 0) {
        g_save.page_pending = 0;
        return real;
    }
    if (g_save.page_pending > 0 && g_save.nav_active && SaveSlot_DetectView(g_save.object) == SAVE_VIEW_SLOTS) {
        g_save.page_pending = 0;
        return 2;
    }
    return real;
}

/*
 * 三项动作窗口的视觉 HitTest 只在“手柄已经接管该窗口”时覆盖。
 * 这个 CALL 只存在于 0x4262C0 自己的三按钮循环，因此不会碰 Battle、标题或其它菜单。
 */
static u8 FASTCALL SaveSlot_HookActionHit(void* button, void* unused_edx) {
    PFN_ButtonHitFast orig = (PFN_ButtonHitFast)FN_BUTTON_HITTEST;
    u32 allowed_mask = 0u;
    int index;
    (void)unused_edx;

    if (SaveSlot_DetectView(g_save.object) == SAVE_VIEW_ACTION) {
        /*
         * disabled只在本次原版Update窗口内可靠存在，所以每个Hit调用都先扫描三只按钮并发布完整mask。
         * 同一帧后两次调用得到相同mask，publish函数会识别“状态未变化”并保持静默。
         */
        save_action_publish_allowed_mask(g_save.object);

        if (!g_save.action_nav_active || !save_action_read_allowed_mask(&allowed_mask) || allowed_mask == 0u) {
            return orig(button, NULL);
        }

        for (index = 0; index < SAVE_ACTION_COUNT; ++index) {
            if (button == save_action_button(g_save.object, index)) {
                /* disabled项即使碰巧等于旧focus也必须返回0，绝不能画出一个原版Event会拒绝的假焦点。 */
                return (u8)(save_action_focus_allowed(allowed_mask, index) && index == g_save.action_focus);
            }
        }
    }
    return orig(button, NULL);
}

/*
 * A/B 不直接调用 Save/Load。这里只在 0x426387 的原版 ButtonEvent CALL 上补一次 code=2。
 * 点击 index1/2 后，RPG.exe 自己打开 action+0x5B4 的 Yes/No；点击 index0 则走原版取消关闭。
 */
static i32 FASTCALL SaveSlot_HookActionEvent(void* button, void* unused_edx) {
    PFN_ButtonEventThis orig = (PFN_ButtonEventThis)FN_BUTTON_EVENT;
    u32 allowed_mask = 0u;
    i32 real;
    int index;
    (void)unused_edx;

    /* Event阶段再次捕获同一Update窗口内的真实状态，不能只相信稍早Hit阶段的快照。 */
    if (SaveSlot_DetectView(g_save.object) == SAVE_VIEW_ACTION) {
        save_action_publish_allowed_mask(g_save.object);
    }

    real = orig(button);
    if (real != 0) {
        g_save.action_confirm_pending = -1;
        return real;
    }

    index = g_save.action_confirm_pending;
    if (index >= 0 && index < SAVE_ACTION_COUNT && g_save.action_nav_active &&
        SaveSlot_DetectView(g_save.object) == SAVE_VIEW_ACTION &&
        save_action_read_allowed_mask(&allowed_mask) &&
        save_action_focus_allowed(allowed_mask, index) &&
        button == save_action_button(g_save.object, index) && save_button_accepts_event(button)) {
        g_save.action_confirm_pending = -1;
        Runtime_Log(index == SAVE_ACTION_CANCEL_INDEX
                    ? "[存读档动作] 三项窗口：原版取消按钮已由手柄触发。"
                    : "[存读档动作] 三项窗口：存档/读档原版按钮已由手柄触发，等待原版二次询问。");
        return 2;
    }

    /*
     * 只有扫描到pending真正对应的Button时才判断失败；循环前两个其它按钮不能提前清掉后面的目标。
     * 若目标此刻disabled或窗口已经结束，清除陈旧pending，禁止它在未来恢复可用后补发。
     */
    if (index >= 0 && index < SAVE_ACTION_COUNT &&
        (SaveSlot_DetectView(g_save.object) != SAVE_VIEW_ACTION ||
         button == save_action_button(g_save.object, index))) {
        g_save.action_confirm_pending = -1;
        Runtime_Log("[存读档动作] 已拒绝指向禁用或失效按钮的陈旧确认。");
    }
    return real;
}

/*
 * 标题/共享槽位的五处稳定 Hook 继续保持独立。
 * 三项窗口由天书与地图存档点共享；它保持独立 capability，不能让基础槽位 Hook 反向依赖它。
 */
int SaveSlot_InstallHooks(void) {
    if (!Runtime_PatchCall(CALL_SAVE_TICK_ANIM, (void*)SaveSlot_HookAnim, FN_SAVE_SLOT_ANIM) ||
        !Runtime_PatchCall(CALL_SAVE_ROW_EVENT, (void*)SaveSlot_HookRowEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SAVE_CANCEL_EVENT, (void*)SaveSlot_HookCancelEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SAVE_PAGE_PREV, (void*)SaveSlot_HookPagePrev, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SAVE_PAGE_NEXT, (void*)SaveSlot_HookPageNext, FN_BUTTON_EVENT)) {
        Runtime_Log("[致命] SaveSlot 原版槽位/分页 Hook 安装失败。");
        return 0;
    }
    g_save.row_confirm_pending = -1;
    g_save.action_confirm_pending = -1;
    g_save.action_allowed_state = 0u;
    g_save.action_hooks_enabled = 0;
    return 1;
}

/*
 * 0x426365/0x426387 属于共享 SaveSlot 子窗口，必须独立于任何 owner 安装一次。
 * 协议不匹配时只禁用三项窗口手柄输入；标题/天书/存档点的基础槽位仍保留原版键鼠。
 */
int SaveSlot_InstallActionHooks(void) {
    if (!Runtime_SaveSlotActionProtocolOk()) {
        g_save.action_hooks_enabled = 0;
        Runtime_Log("[共享存读档动作] 三项窗口协议不匹配；该子层手柄输入已 fail-closed。");
        return 1;
    }
    if (!Runtime_PatchCall(CALL_SAVE_ACTION_HIT, (void*)SaveSlot_HookActionHit, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_SAVE_ACTION_EVENT, (void*)SaveSlot_HookActionEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[共享存读档动作] 三项窗口 HitTest/Event Hook 安装失败。");
        return 0;
    }
    g_save.action_hooks_enabled = 1;
    Runtime_Log("[共享存读档动作] 三项窗口 Hook 已独立安装：天书/存档点共用同一输入路径。");
    return 1;
}

/* ------------------------- worker 线程输入处理 ------------------------- */

/* 确认框只处理上下二选一、A当前项、B强制取消；视觉仍由两个原版 Button 的 HitTest 呈现。 */
static void save_update_popup(u8* s) {
    u8* p = save_popup(s);
    void* button;

    if (!p) return;

    /* 上下都只是切换两个按钮；这是一个二项选择，不做循环 repeat。 */
    if (InputRouter_PressedOn(INPUT_CTX_SAVE_POPUP, INPUT_NAV_UP, INPUT_LAYER_OVERLAY) || InputRouter_PressedOn(INPUT_CTX_SAVE_POPUP, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        g_save.popup_focus = g_save.popup_focus ? 0 : 1;
        Cursor_ClaimForControllerNavigation();
        g_save.popup_nav_active = 1;
        Runtime_Log(g_save.popup_focus == 0 ? "[存读档确认框] 视觉焦点：第一个按钮。" : "[存读档确认框] 视觉焦点：第二个按钮。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_SAVE_POPUP, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        Cursor_ClaimForControllerNavigation();
        g_save.popup_nav_active = 1;
        button = *(void**)(p + (g_save.popup_focus == 0 ? POPUP_BUTTON_YES : POPUP_BUTTON_NO));
        if (Runtime_PtrOk(button)) UiBridge_RequestEventOwned(UI_EVENT_OWNER_SAVE_SLOT, button, 2, "[存读档确认框] A：确认当前视觉高亮按钮。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_SAVE_POPUP, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        Cursor_ClaimForControllerNavigation();
        g_save.popup_focus = 1;
        g_save.popup_nav_active = 1;
        button = *(void**)(p + POPUP_BUTTON_NO);
        if (Runtime_PtrOk(button)) UiBridge_RequestEventOwned(UI_EVENT_OWNER_SAVE_SLOT, button, 2, "[存读档确认框] B：直接执行取消按钮。");
    }

    /* SaveSlot 自己的直接确认框同样是模态层，阻止按键继续落到 InterfaceShell。 */
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_CANCEL);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
}

/*
 * 共享 SaveSlot 的三项动作窗口；state7 天书与地图存档点 mode=2 都会进入这里。
 *
 * 原版真实 index 与屏幕顺序：2(最上) -> 1(中间) -> 0(最下)。
 * 这不是猜标签得到的：0x425FE0 构造三只 Button 时 Y 坐标依次为 0x5A、0x3C、0x1E，
 * 而 index0 又是唯一“点击后不打开 Yes/No”的项目，所以它就是底部取消项。
 *
 * 上下只在这个三项序列里移动；A 点击当前真实 Button；B 无条件点击 index0。
 */
static void save_update_action(u8* s) {
    u8* a = save_action(s);
    u32 allowed_mask = 0u;
    int old_focus;
    void* button;

    if (!a) return;

    /*
     * worker绝不重新解引用Button的disabled字段，只读取游戏线程发布的单个32位状态。
     * 如果第一帧Hit Hook还没来得及发布，就暂时不建立强制焦点；输入仍在函数末尾被模态消费，
     * 下一帧拿到真实mask后再开始导航，避免用猜测的111覆盖短暂disabled。
     */
    if (!save_action_read_allowed_mask(&allowed_mask) || allowed_mask == 0u) {
        g_save.action_nav_active = 0;
        g_save.action_confirm_pending = -1;
        goto consume_inputs;
    }

    /*
     * 从槽位用手柄确定键进入时，游戏线程通常已经把action_nav_active和正确默认焦点准备好。
     * 这里仍保留worker兜底：手柄父层默认从index2开始，再按allowed mask迁到最近可用项；
     * 鼠标打开则先尊重原版+0x598，只有用户真正按手柄后才接管。
     */
    if (!g_save.action_nav_active) {
        int native_index = *(i32*)(a + SAVE_ACTION_SELECTED_INDEX);
        if (save_action_focus_allowed(allowed_mask, native_index)) g_save.action_focus = native_index;
        else save_action_normalize_focus(allowed_mask, 2);
        if (g_save.nav_active) {
            g_save.action_focus = 2;
            save_action_normalize_focus(allowed_mask, 2);
            g_save.action_nav_active = 1;
            g_save.action_confirm_pending = -1;
            Cursor_ClaimForControllerNavigation();
        }
    } else {
        /* 按钮可能在获得焦点后变disabled；每个worker tick再按最新快照做一次无指针归一化。 */
        save_action_normalize_focus(allowed_mask, 2);
    }

    if (InputRouter_PressedOn(INPUT_CTX_SAVE_ACTION, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        old_focus = g_save.action_focus;
        if (!g_save.action_nav_active) {
            int native_index = *(i32*)(a + SAVE_ACTION_SELECTED_INDEX);
            if (save_action_focus_allowed(allowed_mask, native_index)) g_save.action_focus = native_index;
            else save_action_normalize_focus(allowed_mask, 2);
            g_save.action_nav_active = 1;
            g_save.action_confirm_pending = -1;
        }
        g_save.action_focus = save_action_move_allowed(allowed_mask, g_save.action_focus, +1);
        Cursor_ClaimForControllerNavigation();
        if (old_focus != g_save.action_focus) Runtime_Log("[存读档动作] 三项窗口：焦点向上移动。");
    }
    if (InputRouter_PressedOn(INPUT_CTX_SAVE_ACTION, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        old_focus = g_save.action_focus;
        if (!g_save.action_nav_active) {
            int native_index = *(i32*)(a + SAVE_ACTION_SELECTED_INDEX);
            if (save_action_focus_allowed(allowed_mask, native_index)) g_save.action_focus = native_index;
            else save_action_normalize_focus(allowed_mask, 2);
            g_save.action_nav_active = 1;
            g_save.action_confirm_pending = -1;
        }
        g_save.action_focus = save_action_move_allowed(allowed_mask, g_save.action_focus, -1);
        Cursor_ClaimForControllerNavigation();
        if (old_focus != g_save.action_focus) Runtime_Log("[存读档动作] 三项窗口：焦点向下移动。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_SAVE_ACTION, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        if (!g_save.action_nav_active) {
            int native_index = *(i32*)(a + SAVE_ACTION_SELECTED_INDEX);
            if (save_action_focus_allowed(allowed_mask, native_index)) g_save.action_focus = native_index;
            else save_action_normalize_focus(allowed_mask, 2);
            g_save.action_nav_active = 1;
            g_save.action_confirm_pending = -1;
        }
        Cursor_ClaimForControllerNavigation();
        button = save_action_button(s, g_save.action_focus);
        if (save_action_focus_allowed(allowed_mask, g_save.action_focus) &&
            button && save_button_accepts_event(button)) {
            g_save.action_confirm_pending = g_save.action_focus;
            Runtime_Log("[存读档动作] A：等待三项窗口当前原版 ButtonEvent。");
        }
    }

    if (InputRouter_PressedOn(INPUT_CTX_SAVE_ACTION, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        g_save.action_nav_active = 1;
        g_save.action_confirm_pending = -1;
        Cursor_ClaimForControllerNavigation();
        button = save_action_button(s, SAVE_ACTION_CANCEL_INDEX);
        if (save_action_focus_allowed(allowed_mask, SAVE_ACTION_CANCEL_INDEX) &&
            button && save_button_accepts_event(button)) {
            g_save.action_focus = SAVE_ACTION_CANCEL_INDEX;
            g_save.action_confirm_pending = SAVE_ACTION_CANCEL_INDEX;
            Runtime_Log("[存读档动作] B：等待三项窗口原版取消 ButtonEvent。");
        } else {
            /* 取消按钮也必须遵守同一原生disabled规则；不可用时保持当前最近可用焦点且不注入。 */
            save_action_normalize_focus(allowed_mask, g_save.action_focus);
        }
    }

    /*
     * 三项窗口是模态层。除 Start/R3/Back 系统能力外，当前帧所有菜单键都不能继续穿透给 InterfaceShell。
     * 这里显式 Consume 而不是依赖“调用顺序碰巧在前”，以后即使调度顺序调整也仍然安全。
     */
consume_inputs:
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

/*
 * worker 侧的槽位导航总入口：↑↓连续跨页、←→快速翻页、A确认、B取消。
 * owner 打开的 SaveAction 仍由同一个 SaveSlot 控制器继续处理；二层 Yes/No 则明确让给 ConfirmDialog。
 * 所有游戏对象业务写入都留在相应原版游戏线程 Hook。
 */
void SaveSlot_Update(u8* s) {
    void* button;
    int old_focus;
    int dir;
    SaveSlotView view;

    if (s != g_save.object) return;
    view = SaveSlot_DetectView(s);
    if (view == SAVE_VIEW_NONE) return;

    /*
     * SaveAction 的二层 Yes/No 由 ConfirmDialog 的真实 vtable Update 接管。
     * 本模块此时必须完全停手，否则同一个 A/B 会既点询问框又继续点下面三项/槽位。
     */
    if (view == SAVE_VIEW_ACTION_POPUP) {
        g_save.last_view = view;
        return;
    }

    if (view == SAVE_VIEW_ACTION) {
        /* 从二层 No 返回三项窗口时保留上一次 action 焦点；视觉 Hook 会继续指向同一真实按钮。 */
        if (g_save.action_hooks_enabled) {
            save_update_action(s);
        } else {
            /* 深层仍是模态窗口；能力关闭时阻止按键穿透，只保留 RPG.exe 原版键鼠。 */
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
        g_save.last_view = view;
        return;
    }

    if (view == SAVE_VIEW_POPUP) {
        /* 第一次看到弹窗时从原版 selection 同步视觉，而不是强行默认“确定”。 */
        u8* p = save_popup(s);
        if (!g_save.popup_nav_active && p) {
            g_save.popup_focus = (*(i32*)(p + POPUP_NATIVE_SELECTION) == 0) ? 1 : 0;
        }
        save_update_popup(s);
        g_save.last_view = view;
        return;
    }

    /* 离开任何弹窗/三项窗口回槽位时，对应的强制视觉立即失效。 */
    g_save.popup_nav_active = 0;
    g_save.action_nav_active = 0;
    g_save.action_confirm_pending = -1;
    g_save.action_allowed_state = 0u;
    g_save.last_view = view;

    if (InputRouter_PressedOn(INPUT_CTX_SAVE_SLOT, INPUT_NAV_UP, INPUT_LAYER_OVERLAY) || InputRouter_PressedOn(INPUT_CTX_SAVE_SLOT, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        int was_active = g_save.nav_active;
        dir = InputRouter_PressedOn(INPUT_CTX_SAVE_SLOT, INPUT_NAV_UP, INPUT_LAYER_OVERLAY) ? -1 : +1;
        old_focus = g_save.focus;
        save_claim_navigation();

        if (dir < 0 && g_save.focus <= 0) {
            button = *(void**)(s + SAVE_PAGE_PREV_BUTTON);
            if (Runtime_PtrOk(button) && g_save.page_wait_dir == 0) {
                g_save.page_pending = -1;
                g_save.page_wait_dir = -1;
                g_save.page_wait_tick = Runtime_Tick();
                Runtime_Log("[存读档] ↑ 越过本页第一槽：请求原版上一页。");
            }
        } else if (dir > 0 && g_save.focus >= SAVE_ROW_COUNT - 1) {
            button = *(void**)(s + SAVE_PAGE_NEXT_BUTTON);
            if (Runtime_PtrOk(button) && g_save.page_wait_dir == 0) {
                g_save.page_pending = 1;
                g_save.page_wait_dir = 1;
                g_save.page_wait_tick = Runtime_Tick();
                Runtime_Log("[存读档] ↓ 越过本页最后槽：请求原版下一页。");
            }
        } else {
            g_save.focus = save_move_row(s, g_save.focus, dir);
            if (old_focus != g_save.focus || !was_active) save_request_native_selection(s, g_save.focus);
        }
    }

    /* dev20 新增：左右是快速翻页；LB/RB 在 SaveSlot 中明确不占用。 */
    if (InputRouter_PressedOn(INPUT_CTX_SAVE_SLOT, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY) && g_save.page_wait_dir == 0) {
        save_claim_navigation();
        button = *(void**)(s + SAVE_PAGE_PREV_BUTTON);
        if (Runtime_PtrOk(button)) {
            g_save.page_pending = -1;
            g_save.page_wait_dir = -1;
            g_save.page_wait_tick = Runtime_Tick();
            Runtime_Log("[存读档] ←：请求原版上一页。");
        }
    }
    if (InputRouter_PressedOn(INPUT_CTX_SAVE_SLOT, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY) && g_save.page_wait_dir == 0) {
        save_claim_navigation();
        button = *(void**)(s + SAVE_PAGE_NEXT_BUTTON);
        if (Runtime_PtrOk(button)) {
            g_save.page_pending = 1;
            g_save.page_wait_dir = 1;
            g_save.page_wait_tick = Runtime_Tick();
            Runtime_Log("[存读档] →：请求原版下一页。");
        }
    }

    if (InputRouter_PressedOn(INPUT_CTX_SAVE_SLOT, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        save_claim_navigation();
        button = save_row_button(s, g_save.focus);
        if (Runtime_PtrOk(button) && save_row_available(s, g_save.focus) && save_button_accepts_event(button)) {
            g_save.row_confirm_pending = g_save.focus;
            Runtime_Log("[存读档] A：等待当前槽位的原版 Event 调用点。");
        }
    }

    if (InputRouter_PressedOn(INPUT_CTX_SAVE_SLOT, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        save_claim_navigation();
        button = *(void**)(s + SAVE_CANCEL_BUTTON);
        if (Runtime_PtrOk(button) && save_button_accepts_event(button)) {
            g_save.cancel_pending = 1;
            Runtime_Log("[存读档] B：等待原版取消 Event 调用点。");
        }
    }

    /* 槽位层拥有 A/B 与四方向；LB/RB 不消费，state7 仍可让 InterfaceShell 切主大类。 */
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_CANCEL);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
}
