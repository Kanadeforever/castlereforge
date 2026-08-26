#include "confirm_dialog.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"
#include "ui_bridge.h"
#include "interface_items.h"
#include "interface_skills.h"
#include "save_slot.h"

/*
 * 这段代码是 refactor10/refactor11 留下的“双按钮询问框候选实现”。
 * 用户后续已经否定“先把所有选择框公共化”的方向，所以 refactor15 也不继续扩张它；
 * 本轮只为了保持 r11 行为不变而原样保留业务逻辑，并把这条限制写清楚。
 * 这里保存两个原版询问框类真正的虚函数 Update 指针：
 * - vtable 0x460C08 的 +0x18 槽（0x460C20）原本指向 0x4272C0；
 * - vtable 0x460C28 的 +0x18 槽（0x460C40）原本指向 0x4276F0。
 *
 * 每一只真实询问框对象每帧都会通过自己的 vtable 调用 Update。
 * 因此这里看到的 owner 至少是“正在被这两种原版类更新的真实对象”，不是按钮数量/open 调用点猜测。
 * 但这绝不推出“其它 Yes/No、数量、三项窗口也是同一类”，后续必须逐窗口研究。
 */
static PFN_ThisVoid g_original_update_type1;
static PFN_ThisVoid g_original_update_type2;

typedef struct ConfirmDialogState {
    volatile void* owner;
    int focus;      /* 0=第一个按钮（通常是确定/是）；1=第二个按钮（通常是否/取消）。 */
    int nav_active; /* 1=手柄已接管该弹窗视觉；0=鼠标原生视觉。 */
} ConfirmDialogState;

static ConfirmDialogState g_confirm;

/*
 * 这两个判断只是“所有权排除”，绝不修改 Battle / SaveSlot 的任何状态。
 * refactor7 已实机通过的专属确认框继续由原模块处理；这个历史候选层即使观察到它们在 Update，
 * 也只会原样调用原版函数，不登记为候选 ConfirmDialog。
 */
static int confirm_is_battle_sealed_owner(void* owner) {
    u8* battle = *(u8**)GLOBAL_BATTLE_UI;
    u8* sub = NULL;
    u8* popup = NULL;
    int cmd;

    if (!Runtime_PtrOk(battle) || *(u8*)(battle + BATTLE_SUBMENU_ACTIVE) == 0) return 0;
    cmd = *(i32*)(battle + BATTLE_ACTIVE_COMMAND);
    if (cmd == 1) {
        sub = *(u8**)(battle + BATTLE_CMD1_SUBUI);
        if (Runtime_PtrOk(sub)) popup = *(u8**)(sub + SUB1_CONFIRM_POPUP);
    } else if (cmd == 2) {
        sub = *(u8**)(battle + BATTLE_CMD2_SUBUI);
        if (Runtime_PtrOk(sub)) popup = *(u8**)(sub + SUB2_CONFIRM_POPUP);
    }
    return Runtime_PtrOk(popup) && popup == owner;
}

static int confirm_is_save_sealed_owner(void* owner) {
    /*
     * refactor30 只扫 GLOBAL_TITLE_UI+TITLE_SAVE_UI，这个假设在存档点独立 wrapper 接入后已经不成立。
     * 现在向唯一的 SaveSlot Controller 询问当前 owner：不管它由标题、天书还是存档点持有，
     * 直接确认框都只由 save_slot.c 处理一次。
     */
    return SaveSlot_OwnsDirectPopup(owner);
}

static int confirm_is_sealed_owner(void* owner) {
    return confirm_is_battle_sealed_owner(owner) || confirm_is_save_sealed_owner(owner);
}

/*
 * 原版 +0x58C 的数字含义与本模块内部 focus 相反：
 * - 原版 1 = 第一个按钮（是/确定）；
 * - 原版 0 = 第二个按钮（否/取消）。
 *
 * 本模块内部用 0=第一个、1=第二个，便于数组和条件判断，所以必须集中转换。
 */
static int confirm_native_to_focus(i32 native_focus) {
    if (native_focus == 1) return 0;
    if (native_focus == 0) return 1;
    return 0;
}

/*
 * 判断“这只对象现在是否真的是一个可见、尚未完成选择的双按钮询问框”。
 * 关键点是 +0x579：两个原版 Update 在展开时把它设为 1，在收起结束时清为 0。
 * 这比 refactor8 的“看见 HitTest 就猜弹窗”可靠，也比 refactor9 的“必须命中某个 open CALL”完整，
 * 因为无论对象从哪里被创建/打开，只要原版正在以询问框类 Update 它，我们都能观察到。
 */
static int confirm_owner_is_live(void* owner) {
    u8* p = (u8*)owner;
    void* yes_button;
    void* no_button;
    i32 result;

    if (!Runtime_PtrOk(p)) return 0;
    if (confirm_is_sealed_owner(owner)) return 0;

    yes_button = *(void**)(p + POPUP_BUTTON_YES);
    no_button = *(void**)(p + POPUP_BUTTON_NO);
    if (!Runtime_PtrOk(yes_button) || !Runtime_PtrOk(no_button)) return 0;

    /* +0x579=0 表示当前没有作为可见询问框参与原版交互。 */
    if (*(u8*)(p + POPUP_ACTIVE) == 0u) return 0;

    /* +0x590=-1 表示尚未选择；0/1 表示原版已经产出 No/Yes 结果。 */
    result = *(i32*)(p + POPUP_RESULT);
    if (result == 0 || result == 1) return 0;

    return 1;
}

/*
 * 每次原版 Update 前后都同步一次 owner。
 * 前同步让已经登记的对象在本帧 HitTest/Event 发生前仍可使用手柄焦点；
 * 后同步则负责捕获“刚刚在这次原版 Update 内从展开状态变成可见”的新弹窗。
 */
static void confirm_observe_owner(void* owner) {
    if (confirm_owner_is_live(owner)) {
        i32 native_focus;

        if (g_confirm.owner != owner) {
            g_confirm.owner = owner;
            g_confirm.nav_active = 0;

            native_focus = *(i32*)((u8*)owner + POPUP_NATIVE_SELECTION);
            g_confirm.focus = confirm_native_to_focus(native_focus);
            Runtime_Log("[询问框] 检测到原版双按钮对象正在真实 Update，通用手柄 Context 已登记。");
        }
        return;
    }

    if (g_confirm.owner == owner) {
        g_confirm.owner = NULL;
        g_confirm.nav_active = 0;
    }
}

/* 第一种双按钮类 vtable Update 包装器。 */
static void FASTCALL ConfirmDialog_HookUpdateType1(void* owner, void* unused_edx) {
    (void)unused_edx;

    confirm_observe_owner(owner);
    if (g_original_update_type1) g_original_update_type1(owner);
    confirm_observe_owner(owner);
}

/* 第二种同构双按钮类 vtable Update 包装器。 */
static void FASTCALL ConfirmDialog_HookUpdateType2(void* owner, void* unused_edx) {
    (void)unused_edx;

    confirm_observe_owner(owner);
    if (g_original_update_type2) g_original_update_type2(owner);
    confirm_observe_owner(owner);
}

int ConfirmDialog_IsActive(void) {
    void* owner = (void*)g_confirm.owner;

    if (!confirm_owner_is_live(owner)) return 0;
    return 1;
}

static void confirm_drop_if_finished(void) {
    void* owner = (void*)g_confirm.owner;

    if (!owner) return;
    if (!confirm_owner_is_live(owner)) {
        g_confirm.owner = NULL;
        g_confirm.nav_active = 0;
    }
}

static void confirm_claim_navigation(void) {
    Cursor_ClaimForControllerNavigation();
    g_confirm.nav_active = 1;
}

/*
 * UiBridge 在原版询问框 Update 内调用这个过滤器。
 * 只有“当前真实活跃对象 + 手柄已经接管导航”时才覆盖 HitTest；否则完全交回原版鼠标逻辑。
 */
u8 ConfirmDialog_FilterHit(void* button, int* handled) {
    u8* owner = (u8*)g_confirm.owner;
    void* yes_button;
    void* no_button;

    if (handled) *handled = 0;

    /* state2 页面弹窗由 InterfaceItems 先拥有；历史通用候选层连视觉 HitTest 也必须让路。 */
    if (InterfaceItems_AnyPopupActive() || InterfaceSkills_AnyPopupActive()) return 0;

    if (!g_confirm.nav_active || !ConfirmDialog_IsActive()) return 0;
    if (!Runtime_PtrOk(owner) || !Runtime_PtrOk(button)) return 0;

    yes_button = *(void**)(owner + POPUP_BUTTON_YES);
    no_button = *(void**)(owner + POPUP_BUTTON_NO);

    if (button == yes_button) {
        if (handled) *handled = 1;
        return (u8)(g_confirm.focus == 0);
    }
    if (button == no_button) {
        if (handled) *handled = 1;
        return (u8)(g_confirm.focus == 1);
    }
    return 0;
}

void ConfirmDialog_Update(void) {
    u8* owner;
    void* button;
    i32 native_focus;

    /*
     * state2 的两个弹窗现在由 InterfaceItems 自己按页面协议接管。
     * 即使它们复用了同一种 vtable，也不能再让历史通用 ConfirmDialog 同时消费同一组 A/B/方向。
     */
    if (InterfaceItems_AnyPopupActive() || InterfaceSkills_AnyPopupActive()) return;

    confirm_drop_if_finished();
    if (!ConfirmDialog_IsActive()) return;

    owner = (u8*)g_confirm.owner;

    /*
     * 手柄还没有主动按方向时，继续尊重原版鼠标当前选择。
     * 这样用户可以先用鼠标悬停，再直接按 A，确认的仍是屏幕上实际高亮的那个按钮。
     */
    if (!g_confirm.nav_active) {
        native_focus = *(i32*)(owner + POPUP_NATIVE_SELECTION);
        if (native_focus == 0 || native_focus == 1) {
            g_confirm.focus = confirm_native_to_focus(native_focus);
        }
    }

    if (InputRouter_PressedOn(INPUT_CTX_CONFIRM_DIALOG, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY) ||
        InputRouter_PressedOn(INPUT_CTX_CONFIRM_DIALOG, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        confirm_claim_navigation();
        g_confirm.focus = 0;
        Runtime_Log("[询问框] 方向键：选择第一个（确定/是）按钮。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_CONFIRM_DIALOG, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY) ||
        InputRouter_PressedOn(INPUT_CTX_CONFIRM_DIALOG, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        confirm_claim_navigation();
        g_confirm.focus = 1;
        Runtime_Log("[询问框] 方向键：选择第二个（取消/否）按钮。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_CONFIRM_DIALOG, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        confirm_claim_navigation();
        button = *(void**)(owner + (g_confirm.focus == 0 ? POPUP_BUTTON_YES : POPUP_BUTTON_NO));
        if (Runtime_PtrOk(button)) {
            UiBridge_RequestEventOwned(UI_EVENT_OWNER_CONFIRM_DIALOG, button, 2, "[询问框] A：确认当前原版按钮。");
        }
    }

    if (InputRouter_PressedOn(INPUT_CTX_CONFIRM_DIALOG, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        confirm_claim_navigation();
        g_confirm.focus = 1;
        button = *(void**)(owner + POPUP_BUTTON_NO);
        if (Runtime_PtrOk(button)) {
            UiBridge_RequestEventOwned(UI_EVENT_OWNER_CONFIRM_DIALOG, button, 2, "[询问框] B：执行原版取消/否按钮。");
        }
    }

    /*
     * 只有已经由真实 vtable Update 证明“当前确实活跃”的询问框才会来到这里，
     * 因此现在消费 A/B/方向不会再像 refactor8 那样把标题或战斗普通菜单吞掉。
     */
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_CANCEL);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
}

void ConfirmDialog_OnPointerTakeover(void) {
    /* 实体鼠标接管时只释放手柄强制高亮；弹窗 owner 仍由原版 vtable Update 自动维护。 */
    g_confirm.nav_active = 0;
}

int ConfirmDialog_InstallHooks(void) {
    if (!Runtime_ConfirmDialogProtocolOk()) {
        Runtime_Log("[询问框] 原版询问框 vtable/按钮协议未通过；通用询问框手柄功能已单独禁用。");
        return 1;
    }

    /*
     * 这里虽然调用名叫 PatchIatPointer，但它本质是“安全改一个 32 位函数指针槽”。
     * 两个地址都位于只读 vtable，因此 Runtime 会临时 VirtualProtect、先保存 original、再写 replacement。
     */
    if (!Runtime_PatchIatPointer(VTABLE_CONFIRM1_UPDATE,
                                 (void*)ConfirmDialog_HookUpdateType1,
                                 (void**)&g_original_update_type1) ||
        !Runtime_PatchIatPointer(VTABLE_CONFIRM2_UPDATE,
                                 (void*)ConfirmDialog_HookUpdateType2,
                                 (void**)&g_original_update_type2) ||
        !UiBridge_InstallConfirmDialogHooks()) {
        Runtime_Log("[询问框] vtable Update / 第二变体 UI Bridge Hook 安装失败；通用询问框已单独禁用。");
        g_confirm.owner = NULL;
        return 1;
    }

    g_confirm.owner = NULL;
    g_confirm.focus = 0;
    g_confirm.nav_active = 0;
    Runtime_Log("[询问框] 已启用真实 vtable Update 生命周期：只接管正在可见更新的双按钮对象。");
    return 1;
}
