#include "inn.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"
#include "ui_bridge.h"

/*
 * inn.c
 *
 * 这不是“自己实现一个客栈菜单”，而是给 RPG.exe 已经存在的三只 Button 加手柄输入。
 * 原版 0x413850 每帧会：
 * 1. 扫描 owner+0x588/+0x58C/+0x590 三只 Button 做 HitTest；
 * 2. 再扫描三只 Button 做 ButtonEvent；
 * 3. 根据 index=0/1/2 分别进入诸态、炼化、歇息原版业务。
 *
 * 因此插件只需要两件事：
 * - ↑/↓ 改变“手柄现在想聚焦哪一只真实 Button”；
 * - A 排一次 code=2，等游戏线程真正扫描到这只 Button 时由 UiBridge 返回。
 *
 * 没有直接写 owner+0x57C、没有直接 new Interface/炼化对象，也没有自己恢复 HP/MP。
 */

typedef struct InnState {
    int focus;      /* 0=诸态，1=炼化，2=歇息。 */
    int nav_active; /* 只有手柄真正开始导航后才强制 HitTest；鼠标接管时立即清零。 */
    int was_active; /* 上一 tick 是否处于根菜单；用于识别进入子界面及返回边沿。 */
    u8* session_owner; /* 当前客栈会话对象；对象销毁/更换后绝不继承旧焦点。 */
    int return_focus; /* 手柄进入子界面前的真实入口：0=诸态，1=炼化。 */
    int return_armed; /* 已通过入口 Button 排入原版 Event，等待同 owner 进入子界面。 */
    int child_seen_inactive; /* 同一 owner 已经从根菜单变为 inactive，证明子界面真实出现。 */
} InnState;

static InnState g_inn;

/* 安全取得原版客栈三项菜单对象。全局为空时返回 NULL，不猜任何备用对象。 */
static u8* inn_owner(void) {
    u8* owner = *(u8**)GLOBAL_INN_UI;
    if (!Runtime_PtrOk(owner)) return NULL;
    return owner;
}

int Inn_Active(void) {
    u8* owner = inn_owner();
    if (!owner) return 0;
    return *(u8*)(owner + INN_ACTIVE) != 0;
}

/* index 只允许 0..2；每一项都是 RPG.exe 构造好的真实 Button 指针。 */
static void* inn_button(u8* owner, int index) {
    void* button;
    if (!Runtime_PtrOk(owner) || index < 0 || index >= INN_BUTTON_COUNT) return NULL;
    button = *(void**)(owner + INN_BUTTON0 + (u32)index * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/*
 * 手柄一旦改变焦点，就取得“菜单导航的光标所有权”。
 * 这只会隐藏普通鼠标/停止鼠标视觉干扰，不会把 Windows 鼠标真的移动到按钮上。
 */
static void inn_claim_navigation(void) {
    g_inn.nav_active = 1;
    Cursor_ClaimForControllerNavigation();
}

/* 只有会真正打开子界面的“诸态/炼化”才建立原位返回事务；“歇息”走剧情流程，不套用此协议。 */
static void inn_arm_return_to_origin(void) {
    if (g_inn.focus != 0 && g_inn.focus != 1) {
        g_inn.return_armed = 0;
        g_inn.child_seen_inactive = 0;
        return;
    }

    g_inn.return_focus = g_inn.focus;
    g_inn.return_armed = 1;
    g_inn.child_seen_inactive = 0;
}

/* 上下移动采用普通三项列菜单规则：到顶/到底就停，不越界，也不暗自触发任何业务。 */
static void inn_move_focus(int delta) {
    int next = g_inn.focus + delta;
    if (next < 0) next = 0;
    if (next >= INN_BUTTON_COUNT) next = INN_BUTTON_COUNT - 1;
    if (next == g_inn.focus) return;

    /* Event 已消费却没有进入子界面时，下一次明确导航会撤销那次陈旧入口事务。 */
    if (!g_inn.child_seen_inactive) g_inn.return_armed = 0;
    inn_claim_navigation();
    g_inn.focus = next;
    Runtime_Log(delta < 0 ? "[客栈] ↑：移动到上一项。" : "[客栈] ↓：移动到下一项。");
}

/*
 * UiBridge 只在 0x413909 这条客栈专属 HitTest CALL 中调用本过滤器。
 * 手柄没接管时 handled=0，原版鼠标照常工作；手柄接管后只有当前焦点返回命中。
 */
u8 Inn_FilterButtonHit(void* button, int* handled) {
    u8* owner;
    int i;

    if (handled) *handled = 0;
    if (!g_inn.nav_active || !Inn_Active()) return 0;

    owner = inn_owner();
    if (!owner) return 0;

    for (i = 0; i < INN_BUTTON_COUNT; ++i) {
        if (button == inn_button(owner, i)) {
            if (handled) *handled = 1;
            return (u8)(i == g_inn.focus ? 1 : 0);
        }
    }
    return 0;
}

void Inn_Update(void) {
    u8* owner;
    void* button;

    owner = inn_owner();
    if (!owner) {
        /* owner 销毁表示整个客栈会话结束；入口焦点绝不能泄漏到下一次新建的客栈。 */
        if (g_inn.was_active) UiBridge_ClearEventOwned(UI_EVENT_OWNER_INN);
        g_inn.focus = 0;
        g_inn.nav_active = 0;
        g_inn.was_active = 0;
        g_inn.session_owner = NULL;
        g_inn.return_focus = 0;
        g_inn.return_armed = 0;
        g_inn.child_seen_inactive = 0;
        return;
    }

    /* 即使分配器复用了业务字段，也只要 owner 地址变化就视为新的独立客栈会话。 */
    if (g_inn.session_owner != owner) {
        if (g_inn.was_active) UiBridge_ClearEventOwned(UI_EVENT_OWNER_INN);
        g_inn.session_owner = owner;
        g_inn.focus = 0;
        g_inn.nav_active = 0;
        g_inn.was_active = 0;
        g_inn.return_focus = 0;
        g_inn.return_armed = 0;
        g_inn.child_seen_inactive = 0;
    }

    if (*(u8*)(owner + INN_ACTIVE) == 0) {
        /*
         * 0x413850 的同一 owner 在打开诸态/炼化时不会销毁，只把 +0x579 清零。
         * 因此“同 owner + 已排入口 Event + inactive”才是可靠的子界面证据。
         */
        if (g_inn.was_active) UiBridge_ClearEventOwned(UI_EVENT_OWNER_INN);
        if (g_inn.return_armed) g_inn.child_seen_inactive = 1;
        g_inn.was_active = 0;
        g_inn.nav_active = 0;
        return;
    }

    if (!g_inn.was_active) {
        /*
         * 新 owner 第一次进入仍以 index0 作为内部默认，但不抢鼠标 hover。
         * 同一 owner 从子界面返回时，则恢复进入前保存的 return_focus：
         * - 诸态(index0) -> 主 Interface -> 诸态(index0)；
         * - 炼化(index1) -> 炼化界面 -> 炼化(index1)。
         *
         * refactor31 把逻辑焦点硬重置到 index0，再让视觉追随逻辑，方向正好做反。
         * 这里由父层在进入前保存唯一来源，返回时让逻辑和既有 HitTest 视觉共同恢复该来源。
         * 不依赖子模块回调，不移动 Windows 鼠标，也不私写 Button Sprite。
         */
        g_inn.was_active = 1;
        if (g_inn.return_armed && g_inn.child_seen_inactive) {
            g_inn.focus = g_inn.return_focus;
            inn_claim_navigation();
            g_inn.return_armed = 0;
            g_inn.child_seen_inactive = 0;
            Runtime_Log(g_inn.focus == 0
                        ? "[客栈] 子界面原位返回：逻辑/视觉焦点恢复到‘诸态’。"
                        : "[客栈] 子界面原位返回：逻辑/视觉焦点恢复到‘炼化’。");
        } else {
            g_inn.nav_active = 0;
            g_inn.return_armed = 0;
            g_inn.child_seen_inactive = 0;
            Runtime_Log("[客栈] 检测到原版三项根菜单：诸态 / 炼化 / 歇息；当前保留鼠标原生 hover。");
        }
    }

    /* 有别的模块的一次性原版事件还没消费时，本模块不能再叠第二张“事件纸条”。 */
    if (UiBridge_EventPending()) return;

    if (InputRouter_PressedOn(INPUT_CTX_INN_ROOT, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        inn_move_focus(-1);
        InputRouter_Consume(INPUT_NAV_UP);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INN_ROOT, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        inn_move_focus(+1);
        InputRouter_Consume(INPUT_NAV_DOWN);
    }

    if (InputRouter_PressedOn(INPUT_CTX_INN_ROOT, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        inn_claim_navigation();
        button = inn_button(owner, g_inn.focus);
        if (button) {
            inn_arm_return_to_origin();
            UiBridge_RequestEventOwned(UI_EVENT_OWNER_INN, button, 2,
                "[客栈] A：请求当前原版按钮执行自己的业务。");
        }
        InputRouter_Consume(INPUT_CONFIRM);
    }

    /*
     * 客栈根层原版没有 B 取消，也没有“返回”图标。
     * 这里显式消费 B 是为了防止同一按键穿透给下面的地图/对话层，但绝不产生任何客栈业务。
     */
    if (InputRouter_Pressed(INPUT_CANCEL)) InputRouter_Consume(INPUT_CANCEL);
}

void Inn_OnPointerTakeover(CursorTakeoverEvent event_type) {
    (void)event_type;
    /* 鼠标/右摇杆接管以后立刻恢复原版 HitTest；尚未消费的手柄事件也只清本模块自己的。 */
    g_inn.nav_active = 0;
    g_inn.return_armed = 0;
    g_inn.child_seen_inactive = 0;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INN);
}

int Inn_InstallHooks(void) {
    if (!Runtime_InnProtocolOk()) {
        Runtime_Log("[客栈] 原版三项菜单协议未通过；客栈根层手柄功能已单独禁用。");
        return 1;
    }
    if (!UiBridge_InstallInnHooks()) {
        Runtime_Log("[客栈] HitTest/ButtonEvent Hook 安装失败；客栈根层手柄功能已单独禁用。");
        return 1;
    }

    g_inn.focus = 0;
    g_inn.nav_active = 0;
    g_inn.was_active = 0;
    g_inn.session_owner = NULL;
    g_inn.return_focus = 0;
    g_inn.return_armed = 0;
    g_inn.child_seen_inactive = 0;
    Runtime_Log("[客栈] 三项根菜单 Adapter 已启用：↑/↓选择，A执行，诸态/炼化子界面原位返回；B无原版业务。");
    return 1;
}
