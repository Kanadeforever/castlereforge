#include "interface_formation.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "ui_bridge.h"
#include "cursor.h"
#include "spatial_neighbor.h"
#include "interface_shell.h"

/*
 * interface_formation.c
 *
 * 这一页是 refactor23 在修正 Battle Target 二维方向以后新增的第一个共用者。
 * 重要原则是“只共用几何，不共用业务”：
 *
 * Battle Target 的合法集合来自 selector side mode / target map；
 * 阵形的 8 个合法点击位置来自 state6 自己的 8 个 Button。
 * 两边最后都只把“若干二维点 + 当前点 + 方向”交给 SpatialNeighbor_Find。
 *
 * 这样以后如果用户指出“右方向应该更严格/更宽松”，只需修 spatial_neighbor.c；
 * 但 Battle 的敌我过滤、阵形的交换/取消生命周期仍然各自留在自己的源码里。
 */

typedef struct InterfaceFormationState {
    /* 原版协议预检和唯一 Event Hook 成功后才置 1。 */
    int enabled;

    /* 用来检测进入/离开 state6，避免把上一轮菜单焦点带进下一轮。 */
    int was_active;

    /* 当前手柄空间焦点，0..7；这是插件私有状态，不直接写 page+0x590。 */
    int focus_index;

    /*
     * 1 表示原版鼠标当前由手柄拿来当阵形焦点图示。
     * 实体鼠标/右摇杆接管后清 0，Adapter 就不会每 tick 把鼠标抢回来。
     */
    int controller_focus_visible;
} InterfaceFormationState;

static InterfaceFormationState g_formation;

/* 每 tick 重新拿主 Interface 指针，不缓存跨菜单生命周期裸对象。 */
static u8* formation_interface(void) {
    u8* interface_ui = *(u8**)GLOBAL_INTERFACE_UI;
    return Runtime_PtrOk(interface_ui) ? interface_ui : NULL;
}

/* 只有 state6 真正打开且 Interface 未进入关闭流程时才返回 page+0x650。 */
static u8* formation_page(void) {
    u8* interface_ui = formation_interface();
    u8* page;

    if (!Runtime_PtrOk(interface_ui)) return NULL;
    if (*(i32*)(interface_ui + INTERFACE_STATE) != 6) return NULL;
    if (*(i32*)(interface_ui + INTERFACE_CLOSE_STATE) != 0) return NULL;

    page = *(u8**)(interface_ui + INTERFACE_PAGE_6);
    return Runtime_PtrOk(page) ? page : NULL;
}

/* page+0x580 是真正拥有 8 个阵位 Button 与两只原版阵形指示 Sprite 的子面板。 */
static u8* formation_panel(u8* page) {
    u8* panel;
    if (!Runtime_PtrOk(page)) return NULL;
    panel = *(u8**)(page + INTERFACE_FORMATION_PANEL);
    return Runtime_PtrOk(panel) ? panel : NULL;
}

/* 从 panel+0x59C 开始按 4 字节指针连续取得 8 个原版阵位 Button。 */
static void* formation_button(u8* page, int index) {
    u8* panel = formation_panel(page);
    void* button;

    if (!Runtime_PtrOk(panel)) return NULL;
    if (index < 0 || index >= INTERFACE_FORMATION_SLOT_COUNT) return NULL;

    button = *(void**)(panel + INTERFACE_FORMATION_BUTTON0 + (u32)index * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/*
 * 阵位按钮构造于 0x44024D..0x4402A4，宽高固定来自原版构造参数，但这里仍读取对象自己的宽高。
 * 这样如果以后另一个兼容 EXE 改了布局，只要对象协议不变，二维坐标仍然跟着原版走，而不是跟着硬编码走。
 */
static int formation_button_center(void* button, i32* out_x, i32* out_y) {
    /*
     * 这里必须使用 UiBridge 对 0x431310 的完整几何复刻。
     * r23 直接读取 +0x3C/+0x40，遇到原版合法的“0=从 Sprite frame 取尺寸”就失败，
     * 因而鼠标焦点和 8 阵位几何都可能整页失效。
     */
    return UiBridge_GetButtonScreenCenter(button, out_x, out_y);
}
/* page+0x584 是原版自己的“已经选择来源阵位，正在等目标阵位”标志。 */
static int formation_swap_pending(u8* page) {
    if (!Runtime_PtrOk(page)) return 0;
    return *(u8*)(page + INTERFACE_FORMATION_SWAP_PENDING) != 0;
}

/* 让原版手形鼠标停在当前阵位中心；原版自己的 hover 图示也会自然跟随这一真实鼠标坐标。 */
static void formation_show_focus(u8* page) {
    i32 x;
    i32 y;
    void* button;

    if (!Runtime_PtrOk(page)) return;
    if (g_formation.focus_index < 0 || g_formation.focus_index >= INTERFACE_FORMATION_SLOT_COUNT) return;

    button = formation_button(page, g_formation.focus_index);
    if (!formation_button_center(button, &x, &y)) return;

    Cursor_ClaimForControllerNavigation();
    Cursor_ShowMenuFocusAt(x, y);
    g_formation.controller_focus_visible = 1;
}

/*
 * 把 RPG.exe 自己的 8 项阵形布局表组成 SpatialPoint 数组。
 * 这里不依赖 Button 宽高，也不依赖 Windows 鼠标坐标，因此是稳定的“业务空间拓扑”。
 */
static int formation_build_points(u8* page, SpatialPoint* points, int cap) {
    const i32* layout_x = (const i32*)INTERFACE_FORMATION_LAYOUT_X_TABLE;
    const i32* layout_y = (const i32*)INTERFACE_FORMATION_LAYOUT_Y_TABLE;
    int i;

    if (!Runtime_PtrOk(page) || !points || cap < INTERFACE_FORMATION_SLOT_COUNT) return 0;

    /*
     * 原版 0x43F9A0 在“来源阵位已选中”后，就是用这两张 8 项表移动自己的阵位指示图示。
     * 所以它们才是 state6 的权威空间拓扑。相对邻居计算不需要父窗口屏幕原点，直接使用局部坐标即可。
     *
     * 鼠标显示仍然由 formation_button_center() 读取真实 Button 屏幕中心；
     * 也就是说“空间关系”和“Windows 鼠标坐标”从这里开始明确分工，不再绑在同一套猜测上。
     */
    for (i = 0; i < INTERFACE_FORMATION_SLOT_COUNT; ++i) {
        points[i].id = i;
        points[i].x = layout_x[i];
        points[i].y = layout_y[i];
    }
    return INTERFACE_FORMATION_SLOT_COUNT;
}
static SpatialDirection formation_direction(InputAction action) {
    switch (action) {
    case INPUT_NAV_LEFT:  return SPATIAL_LEFT;
    case INPUT_NAV_RIGHT: return SPATIAL_RIGHT;
    case INPUT_NAV_UP:    return SPATIAL_UP;
    default:              return SPATIAL_DOWN;
    }
}

/* D-Pad：只改变插件空间焦点，不直接改 page+0x588/+0x58C 的原版交换状态。 */
static void formation_move_focus(u8* page, InputAction action) {
    SpatialPoint points[INTERFACE_FORMATION_SLOT_COUNT];
    int count;
    int next;

    count = formation_build_points(page, points, INTERFACE_FORMATION_SLOT_COUNT);
    if (count != INTERFACE_FORMATION_SLOT_COUNT) {
        Runtime_Log("[阵形页] 无法取得完整 8 阵位几何；本次方向输入已忽略。");
        return;
    }

    next = SpatialNeighbor_Find(points, count, g_formation.focus_index, formation_direction(action));
    if (next < 0 || next == g_formation.focus_index) return;

    g_formation.focus_index = next;
    formation_show_focus(page);
    Runtime_Log("[阵形页] D-Pad：已按共享二维邻居规则移动到新的原版阵位。");
}

/*
 * A 不自己做“谁和谁交换”。只把 code=2 交给当前阵位真实 Button。
 * 原版 0x43F9A0 会根据 page+0x584 自动区分：
 * - 第一次 code=2：选择来源阵位；
 * - 第二次 code=2：选择目标阵位并交换；
 * - 空来源/不允许的状态：原版自己拒绝。
 */
static void formation_confirm(u8* page) {
    void* button;
    if (!Runtime_PtrOk(page) || UiBridge_EventPending()) return;

    button = formation_button(page, g_formation.focus_index);
    if (!Runtime_PtrOk(button)) return;

    formation_show_focus(page);
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, 2,
        "[阵形页] A：请求当前阵位原版 code=2；来源选择/目标交换均由 RPG.exe 自己处理。");
}

/*
 * B 有条件覆盖 Shell：
 * - 原版正在交换事务（page+0x584!=0）时，code=1 是页面自己的“取消来源选择”；
 * - 没有交换事务时，本 Adapter 完全不处理 B，让后面的 InterfaceShell 做正常返回。
 */
static void formation_cancel_swap(u8* page) {
    void* button;
    if (!Runtime_PtrOk(page) || UiBridge_EventPending()) return;
    if (!formation_swap_pending(page)) return;

    button = formation_button(page, g_formation.focus_index);
    if (!Runtime_PtrOk(button)) return;

    formation_show_focus(page);
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, 1,
        "[阵形页] B：当前存在原版阵位交换事务，已请求 code=1 取消来源选择。");
    InputRouter_Consume(INPUT_CANCEL);
}

static void formation_reset_transient(void) {
    g_formation.focus_index = 0;
    g_formation.controller_focus_visible = 0;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
    Cursor_HideMenuFocusImmediately();
}

void InterfaceFormation_OnPointerTakeover(CursorTakeoverEvent event) {
    if (event == CURSOR_TAKEOVER_NONE || !g_formation.was_active) return;

    /* 真实鼠标/右摇杆一动，就停止手柄强制焦点；下一次 D-Pad/A 会再从当前插件焦点接管。 */
    g_formation.controller_focus_visible = 0;
    Cursor_HideMenuFocusImmediately();
}

/*
 * “阵形页面是否拥有四方向”必须与 Adapter 能不能完成几何导航分开判断。
 * 只要主 Interface 的原版 state 已经是 6，这四个键就绝不能再漏给 Shell 的角色切换。
 */
static int formation_owns_directional_input(void) {
    u8* interface_ui = formation_interface();
    if (!Runtime_PtrOk(interface_ui)) return 0;
    if (*(i32*)(interface_ui + INTERFACE_CLOSE_STATE) != 0) return 0;
    return *(i32*)(interface_ui + INTERFACE_STATE) == 6;
}

/* 即使 state6 Adapter 因协议预检失败，也要 fail-closed 吞掉页面专属方向，绝不把错误变成“换角色”。 */
static void formation_consume_owned_directions(void) {
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
}

void InterfaceFormation_Update(void) {
    u8* page;
    int owns_directions = formation_owns_directional_input();

    if (!owns_directions) {
        if (g_formation.was_active) {
            formation_reset_transient();
            g_formation.was_active = 0;
            Runtime_Log("[阵形页] 已离开 state6；二维焦点与菜单鼠标提示已清理。");
        }
        return;
    }

    /*
     * state6 一成立，就先清掉上一页面动画期间可能缓存的角色切换动作。
     * 然后本 tick 无条件取得四方向所有权；后面的 Shell 即使执行，也已经看不到这些方向边沿。
     */
    InterfaceShell_DiscardQueuedRoleActions();

    if (!g_formation.enabled) {
        formation_consume_owned_directions();
        return;
    }

    page = formation_page();

    if (!Runtime_PtrOk(page)) {
        /* state6 已拥有方向，即使页面对象还在构造动画中，也不能把四方向泄漏给 Shell。 */
        formation_consume_owned_directions();
        if (g_formation.was_active) {
            formation_reset_transient();
            g_formation.was_active = 0;
            Runtime_Log("[阵形页] 已离开 state6；二维焦点与菜单鼠标提示已清理。");
        }
        return;
    }

    if (!g_formation.was_active) {
        formation_reset_transient();
        g_formation.was_active = 1;
        g_formation.focus_index = 0;
        /* 和五内一样：鼠标进入页面时不抢鼠标；只有本来就是手柄所有权时才立刻显示默认焦点。 */
        if (Cursor_ControllerOwnsPointer()) formation_show_focus(page);
        Runtime_Log("[阵形页] 已进入 state6；D-Pad 二维移动、A原版交换、B事务取消已启用。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_FORMATION, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        formation_move_focus(page, INPUT_NAV_LEFT);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_FORMATION, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        formation_move_focus(page, INPUT_NAV_RIGHT);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_FORMATION, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        formation_move_focus(page, INPUT_NAV_UP);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_FORMATION, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        formation_move_focus(page, INPUT_NAV_DOWN);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_FORMATION, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        formation_confirm(page);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_FORMATION, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        formation_cancel_swap(page);
    }

    /* 本页拥有 A + 四方向；X/Y/LT/RT 暂无产品定义，明确吞掉，防止其它页面规则穿透。 */
    InputRouter_Consume(INPUT_CONFIRM);
    formation_consume_owned_directions();
    InputRouter_Consume(INPUT_SPECIAL_X);
    InputRouter_Consume(INPUT_SPECIAL_Y);
    InputRouter_Consume(INPUT_SUBTYPE_PREV);
    InputRouter_Consume(INPUT_SUBTYPE_NEXT);
}

int InterfaceFormation_InstallHooks(void) {
    g_formation.enabled = 0;
    g_formation.was_active = 0;
    formation_reset_transient();

    if (!Runtime_InterfaceFormationProtocolOk()) {
        Runtime_Log("[阵形页] 原版 state6 协议不匹配；本页 Adapter 已 fail-closed，其它页面继续工作。");
        return 1;
    }
    if (!UiBridge_InstallInterfaceFormationHooks()) {
        Runtime_Log("[阵形页] state6 8阵位 ButtonEvent Hook 安装失败；拒绝启用本页 Adapter。");
        return 0;
    }

    g_formation.enabled = 1;
    Runtime_Log("[阵形页] state6 Adapter 已启用：8阵位二维焦点 + 原版鼠标图示 + A交换/B事务取消。");
    return 1;
}
