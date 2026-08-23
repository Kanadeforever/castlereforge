#include "frontend.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"
#include "save_slot.h"

/*
 * Frontend 不实现 SaveSlot 里面的行/分页算法。
 * 它只负责回答两件事：
 * 1. 标题根菜单现在是否可操作；
 * 2. 标题对象里是否挂着一个活动 SaveSlot，如果有就把对象交给 SaveSlot 模块。
 */
typedef enum FrontendView {
    FRONT_NONE = 0,
    FRONT_TITLE,
    FRONT_SAVE
} FrontendView;

typedef struct FrontendState {
    FrontendView view;
    FrontendView previous_view;

    int title_nav_active;
    int title_focus;
    volatile int title_confirm_pending; /* -1=没有；0..2=等对应独立 Event 调用点。 */

    int pad_transition_armed;
    u32 pad_transition_tick;
    u8* current_save;
} FrontendState;

static FrontendState g_front;

/* 从固定全局槽取得标题 UI；所有标题访问都先经过同一指针安全检查。 */
static u8* frontend_title(void) {
    u8* t = *(u8**)GLOBAL_TITLE_UI;
    return Runtime_PtrOk(t) ? t : NULL;
}

/*
 * 标题三个按钮在对象里不是连续数组，而是三个已确认字段。
 * 所以这里显式把 0/1/2 映射到各自偏移，避免“看起来连续”这种未经验证的假设。
 */
static void* frontend_title_button(int index) {
    u8* t = frontend_title();
    u32 offset;
    void* button;
    if (!t || index < 0 || index >= TITLE_BUTTON_COUNT) return NULL;
    offset = index == 0 ? TITLE_BUTTON0 : (index == 1 ? TITLE_BUTTON1 : TITLE_BUTTON2);
    button = *(void**)(t + offset);
    return Runtime_PtrOk(button) ? button : NULL;
}

/* 标题读档 SaveSlot 由 TITLE_SAVE_UI 字段挂在标题对象下；Frontend 只负责取出，不解释槽位内部结构。 */
static u8* frontend_title_save(void) {
    u8* t = frontend_title();
    u8* s;
    if (!t) return NULL;
    s = *(u8**)(t + TITLE_SAVE_UI);
    return Runtime_PtrOk(s) ? s : NULL;
}

/* 与原版 ButtonEvent 的早退规则一致，只有启用且未被其它状态占用的按钮才允许排手柄确认。 */
static int frontend_button_usable(void* button) {
    u8* b = (u8*)button;
    if (!Runtime_PtrOk(b)) return 0;
    if (*(u8*)(b + 0x45u) == 0) return 0;
    if (*(u8*)(b + 0x04u) != 0) return 0;
    return 1;
}

/*
 * 只有读档 SaveSlot 未激活且 TITLE_STATE==0 时才算标题根菜单。
 * 原版一旦开始执行某项，就立刻退出手柄强制焦点，避免动画过渡期继续注入。
 */
static int frontend_title_root_active(void) {
    u8* t = frontend_title();
    u8* s;
    if (!t) return 0;

    s = frontend_title_save();
    if (s && *(u8*)(s + SAVE_ACTIVE)) return 0;

    /* +0x58C 非 0 时，原版已经进入“执行/离开标题按钮”的状态，不能继续强制主三项。 */
    return *(i32*)(t + TITLE_STATE) == 0;
}

/* 识别顺序先 SaveSlot、后标题根层；更具体的子层必须覆盖父层。 */
static FrontendView frontend_detect_view(void) {
    u8* s = frontend_title_save();
    if (s && SaveSlot_DetectView(s) != SAVE_VIEW_NONE) return FRONT_SAVE;
    if (frontend_title_root_active()) return FRONT_TITLE;
    return FRONT_NONE;
}

/* 给总调度器提供只读状态，便于未来其它 Context 做互斥；当前 refactor1 不新增任何 dev20 之后界面。 */
int Frontend_AnyUiActive(void) {
    return frontend_detect_view() != FRONT_NONE;
}

/* ------------------------- 标题原生视觉 Hook ------------------------- */

/*
 * 手柄导航激活时，只让 g_front.title_focus 对应的原版 Button 返回 HIT。
 * 没有手柄所有权时完整调用原函数，因此实体鼠标 hover 不受影响。
 */
static u8 FASTCALL Frontend_HookTitleHit(void* button, void* unused_edx) {
    PFN_ButtonHitFast orig = (PFN_ButtonHitFast)FN_BUTTON_HITTEST;
    int i;
    (void)unused_edx;

    if (g_front.title_nav_active && g_front.view == FRONT_TITLE && frontend_title_root_active()) {
        for (i = 0; i < TITLE_BUTTON_COUNT; ++i) {
            if (button == frontend_title_button(i)) return (u8)(i == g_front.title_focus ? 1 : 0);
        }
    }
    return orig(button, NULL);
}

/*
 * 每个标题按钮仍走自己的真实 Event CALL；这个共同函数只减少重复的 pending 判断。
 * 原版键鼠事件优先，只有原函数本帧返回 0 时才消费手柄的一次性确认。
 */
static i32 frontend_title_event_common(void* button, int index) {
    PFN_ButtonEventThis orig = (PFN_ButtonEventThis)FN_BUTTON_EVENT;
    i32 real = orig(button);

    /*
     * 原版键鼠事件优先：如果真实鼠标已经让按钮返回事件，就取消手柄 pending，
     * 防止同一帧“鼠标点一次 + 手柄再补一次”造成双触发。
     */
    if (real != 0) {
        g_front.title_confirm_pending = -1;
        return real;
    }

    if (g_front.title_confirm_pending == index && g_front.title_nav_active &&
        g_front.view == FRONT_TITLE && frontend_title_root_active() && frontend_button_usable(button)) {
        g_front.title_confirm_pending = -1;
        g_front.pad_transition_armed = 1;
        g_front.pad_transition_tick = Runtime_Tick();
        Runtime_Log("[标题] A 确认已在对应原版独立 Event 调用点注入。");
        return 2;
    }
    return real;
}

static i32 FASTCALL Frontend_HookTitleEvent0(void* button, void* unused_edx) {
    (void)unused_edx;
    return frontend_title_event_common(button, 0);
}
static i32 FASTCALL Frontend_HookTitleEvent1(void* button, void* unused_edx) {
    (void)unused_edx;
    return frontend_title_event_common(button, 1);
}
static i32 FASTCALL Frontend_HookTitleEvent2(void* button, void* unused_edx) {
    (void)unused_edx;
    return frontend_title_event_common(button, 2);
}

/* 安装标题三处 HitTest 与三处独立 Event Hook；任何一点失败都拒绝把 Frontend 标记为可用。 */
int Frontend_InstallHooks(void) {
    if (!Runtime_PatchCall(CALL_TITLE_HIT_0, (void*)Frontend_HookTitleHit, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_TITLE_HIT_1, (void*)Frontend_HookTitleHit, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_TITLE_HIT_2, (void*)Frontend_HookTitleHit, FN_BUTTON_HITTEST)) {
        Runtime_Log("[致命] 标题三项原生视觉 HitTest Hook 安装失败。");
        return 0;
    }

    /* dev17 实机已经证明：三个按钮必须挂各自真实 Event CALL，不能合并成猜测的统一入口。 */
    if (!Runtime_PatchCall(CALL_TITLE_EVENT_0, (void*)Frontend_HookTitleEvent0, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_TITLE_EVENT_1, (void*)Frontend_HookTitleEvent1, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_TITLE_EVENT_2, (void*)Frontend_HookTitleEvent2, FN_BUTTON_EVENT)) {
        Runtime_Log("[致命] 标题三项独立原生 Event Hook 安装失败。");
        return 0;
    }

    g_front.view = FRONT_NONE;
    g_front.previous_view = FRONT_NONE;
    g_front.title_confirm_pending = -1;
    return 1;
}

/*
 * View 真变化时才重置这一层的临时状态。
 * 从手柄 A 进入读档会在 1200ms 窗口内把“手柄所有权”传给 SaveSlot；鼠标进入则保持原版键鼠控制。
 */
static void frontend_sync_view(void) {
    FrontendView now = frontend_detect_view();
    u8* save;

    if (now == g_front.view) return;
    g_front.previous_view = g_front.view;
    g_front.view = now;
    g_front.title_nav_active = 0;
    g_front.title_confirm_pending = -1;

    if (now == FRONT_TITLE) {
        u8* t = frontend_title();
        int native_visual = t ? *(i32*)(t + TITLE_SELECTED_VISUAL) : 0;
        g_front.title_focus = (native_visual >= 1 && native_visual <= 3) ? native_visual - 1 : 0;
        SaveSlot_End();
        g_front.current_save = NULL;
        Runtime_Log("[前端] 进入标题根菜单。");
        return;
    }

    if (now == FRONT_SAVE) {
        int from_pad;
        save = frontend_title_save();
        from_pad = g_front.pad_transition_armed &&
                   (Runtime_Tick() - g_front.pad_transition_tick) <= Runtime_MsToTicks(1200u);
        g_front.current_save = save;
        SaveSlot_Begin(save, from_pad);
        g_front.pad_transition_armed = 0;
        Runtime_Log("[前端] 进入标题读档 SaveSlot。");
        return;
    }

    SaveSlot_End();
    g_front.current_save = NULL;
    g_front.pad_transition_armed = 0;
    Runtime_Log("[前端] 标题/读档界面离开。");
}

/* 实体鼠标或右摇杆真实鼠标接管时，立即停止标题/SaveSlot 的强制 HitTest，避免双光标和焦点争夺。 */
void Frontend_OnPointerTakeover(CursorTakeoverEvent event) {
    int had_forced_focus;
    if (event == CURSOR_TAKEOVER_NONE) return;

    /*
     * 只有“接管前确实存在标题或 SaveSlot 手柄强制焦点”时才记一次日志。
     * 第一次实体鼠标/右摇杆接管会把这些 active 标志清零；鼠标继续移动时即使 Cursor 连续上报 takeover，
     * 这里也不会再重复写同一句。等用户重新用手柄导航、active 再次变成 1 后，下一次真实接管才会产生下一条日志。
     */
    had_forced_focus = g_front.title_nav_active || SaveSlot_IsControllerActive();

    g_front.title_nav_active = 0;
    g_front.title_confirm_pending = -1;
    SaveSlot_OnPointerTakeover();
    if (had_forced_focus) {
        Runtime_Log(event == CURSOR_TAKEOVER_RIGHT_STICK ?
            "[前端] 右摇杆开始控制真实鼠标，暂停手柄强制焦点。" :
            "[前端] 实体鼠标接管，暂停手柄强制焦点。");
    }
}

/*
 * 标题根层只实现 dev20 已有的 ↑/↓ 与 A。
 * 一旦检测到 SaveSlot，本函数不复制槽位逻辑，而是把对象交给 SaveSlot_Update。
 */
void Frontend_Update(void) {
    void* button;
    int old_focus;

    frontend_sync_view();
    if (g_front.view == FRONT_NONE) return;

    if (g_front.view == FRONT_SAVE) {
        SaveSlot_Update(g_front.current_save);
        return;
    }

    /* 标题根菜单只使用上下三项；左右/LB/RB 在 dev20 没有行为。 */
    if (InputRouter_PressedOn(INPUT_CTX_TITLE, INPUT_NAV_UP, INPUT_LAYER_OVERLAY) || InputRouter_PressedOn(INPUT_CTX_TITLE, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        old_focus = g_front.title_focus;
        if (InputRouter_PressedOn(INPUT_CTX_TITLE, INPUT_NAV_UP, INPUT_LAYER_OVERLAY) && g_front.title_focus > 0) --g_front.title_focus;
        if (InputRouter_PressedOn(INPUT_CTX_TITLE, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY) && g_front.title_focus < TITLE_BUTTON_COUNT - 1) ++g_front.title_focus;
        Cursor_ClaimForControllerNavigation();
        g_front.title_nav_active = 1;
        if (old_focus != g_front.title_focus) Runtime_Log("[标题] D-Pad 改变主菜单视觉焦点。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_TITLE, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        Cursor_ClaimForControllerNavigation();
        g_front.title_nav_active = 1;
        button = frontend_title_button(g_front.title_focus);
        if (Runtime_PtrOk(button) && frontend_button_usable(button)) {
            g_front.title_confirm_pending = g_front.title_focus;
            Runtime_Log("[标题] A：等待当前按钮自己的原版 Event 调用点。");
        }
    }
}
