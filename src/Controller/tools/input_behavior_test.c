/*
 * 输入行为测试：直接编译生产 input_router.c/cursor.c，不复制它们的判断公式。
 * 假设备只提供逐帧按钮与指针；不启动RPG、不读取存档、不写游戏地址。
 */
#include <stdio.h>
#include <string.h>
#undef NULL
#include "../source/input_router.c"
#include "../source/cursor.c"
#include "../source/exploration.c"

static RuntimeConfig test_config;
static RuntimeApi test_api;
static u32 test_tick, test_buttons, test_previous;
static i16 test_axes[6];
static int test_foreground = 1, test_connected = 1, checks;
static Point32 test_point;
const RuntimeConfig* Runtime_Config(void) { return &test_config; }
const RuntimeApi* Runtime_Api(void) { return &test_api; }
u32 Runtime_Tick(void) { return test_tick; }
u32 Runtime_MsToTicks(u32 ms) { return (ms + 7u) / 8u; }
void Runtime_Log(const char* message) { (void)message; }
int Runtime_PtrOk(const void* p) { return p != NULL; }
int Runtime_CopyDisplayGeometry(CastleDisplayGeometryV1* out) { (void)out; return 0; }
int Runtime_PatchCall(u32 a, void* b, u32 c) { (void)a;(void)b;(void)c;return 0; }
int Runtime_PatchJmp6(u32 a, void* b, const u8 c[6]) { (void)a;(void)b;(void)c;return 0; }
int Runtime_PatchIatPointer(u32 a, void* b, void** c) { (void)a;(void)b;(void)c;return 0; }
int PadInput_Down(PadButton b) { return (test_buttons & (1u << b)) != 0; }
int PadInput_Pressed(PadButton b) { return ((test_buttons & ~test_previous) & (1u << b)) != 0; }
int PadInput_Released(PadButton b) { return ((test_previous & ~test_buttons) & (1u << b)) != 0; }
i16 PadInput_Axis(PadAxis a) { return test_axes[a]; }
int PadInput_GameForeground(HWND* w) { if (w) *w = NULL; return test_foreground; }
int PadInput_GamepadConnected(void) { return test_connected; }
/* 地图方向计算复用生产exploration.c；没有游戏对象的两个外围入口只提供无副作用替身。 */
int ControlModes_BlocksMapMovement(void) { return 0; }
void InterfaceShell_OnExplorationGameThread(void) { }
static BOOL WINAPI test_get_cursor(Point32* p) { *p = test_point; return 1; }

/* 失败时报告用例所在行；保留所有断言计数，便于结果接档。 */
#define CHECK(x) do { ++checks; if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)
/* 六条基础读取和地图/战斗路由都检查，不能只证明快捷已执行而漏掉先运行的原功能。 */
#define CHECK_ACTION_BLOCKED(action) do { \
    CHECK(!InputRouter_Pressed(action)); CHECK(!InputRouter_Down(action)); \
    CHECK(!InputRouter_Released(action)); CHECK(!InputRouter_RawPressed(action)); \
    CHECK(!InputRouter_RawDown(action)); CHECK(!InputRouter_RawReleased(action)); \
    CHECK(!InputRouter_DownOn(INPUT_CTX_EXPLORATION,action,INPUT_LAYER_OVERLAY)); \
    CHECK(!InputRouter_PressedOn(INPUT_CTX_BATTLE_TOP,action,INPUT_LAYER_OVERLAY)); \
} while (0)
static void reset_case(void) {
    memset(&g_rb_chord, 0, sizeof(g_rb_chord));
    memset(test_axes, 0, sizeof(test_axes));
    test_tick = 100u; test_buttons = test_previous = 0u;
    test_foreground = test_connected = 1;
}
static void frame(u32 buttons, InputRbChordScope scope) {
    ++test_tick; test_previous = test_buttons; test_buttons = buttons;
    InputRouter_BeginFrame();
    InputRouter_SetRbChordScope(scope);
}

int main(void) {
    static const PadButton keys[] = {PAD_SOUTH,PAD_EAST,PAD_WEST,PAD_NORTH,
        PAD_DPAD_UP,PAD_DPAD_DOWN,PAD_DPAD_LEFT,PAD_DPAD_RIGHT};
    static const InputAction actions[] = {INPUT_CONFIRM,INPUT_CANCEL,INPUT_SPECIAL_X,INPUT_SPECIAL_Y,
        INPUT_NAV_UP,INPUT_NAV_DOWN,INPUT_NAV_LEFT,INPUT_NAV_RIGHT};
    unsigned int i, swap, order, scope_index;
    const u32 rb = 1u << PAD_RB;
    /*
     * 持续快捷层：不松RB，先点一键，再点其它键，再重按第一键。第一键未松也不能妨碍其它键。
     * 同帧重复查询、跨帧一直按住都不能重复触发；两种范围、两种布局与八个物理位置逐一验证。
     */
    for (scope_index = 0; scope_index < 2; ++scope_index)
    for (swap = 0; swap < 2; ++swap) for (i = 0; i < 8; ++i) {
        InputRbChordScope scope = scope_index ? INPUT_RB_CHORD_FREE_IDLE : INPUT_RB_CHORD_BATTLE_TOP;
        unsigned int next = (i + 1u) % 8u;
        u32 key = 1u << keys[i], next_key = 1u << keys[next], deadline;
        InputAction semantic = i < 2 && swap ? (i == 0 ? INPUT_CANCEL : INPUT_CONFIRM) : actions[i];
        InputAction next_semantic = next < 2 && swap ? (next == 0 ? INPUT_CANCEL : INPUT_CONFIRM) : actions[next];
        reset_case(); test_config.swap_confirm_cancel = (int)swap;
        frame(rb,scope); frame(rb|key,scope);
        CHECK(InputRouter_RbChordPressed(actions[i])); CHECK(!InputRouter_RbChordPressed(actions[i]));
        frame(rb|key,scope); CHECK(!InputRouter_RbChordPressed(actions[i]));
        frame(rb|key|next_key,scope);
        CHECK(InputRouter_RbChordPressed(actions[next])); CHECK(!InputRouter_RbChordPressed(actions[i]));
        CHECK_ACTION_BLOCKED(semantic); CHECK_ACTION_BLOCKED(next_semantic);
        frame(rb|next_key,scope); CHECK(!InputRouter_RbChordPressed(actions[i]));
        frame(rb|next_key|key,scope);
        CHECK(InputRouter_RbChordPressed(actions[i])); CHECK(!InputRouter_RbChordPressed(actions[next]));

        /* 普通不允许页面暂停派发，但已开启的层在RB保持时返回合法页面仍能继续。 */
        frame(rb,INPUT_RB_CHORD_NONE); CHECK(!InputRouter_RbChordPressed(actions[i]));
        frame(rb|key,scope); CHECK(InputRouter_RbChordPressed(actions[i]));

        /* 触发过快捷后释放RB也有同一份容错；窗口内再次触发不得延长截止时间。 */
        frame(key,scope); deadline = g_rb_chord.grace_until_tick;
        CHECK(deadline != 0u); frame(0,scope); frame(next_key,scope);
        CHECK(InputRouter_RbChordPressed(actions[next])); CHECK(g_rb_chord.grace_until_tick == deadline);
        test_tick = deadline - 1u; frame(next_key|key,scope);
        CHECK(!InputRouter_RbChordPressed(actions[i])); CHECK(InputRouter_Pressed(semantic));
        CHECK_ACTION_BLOCKED(next_semantic);
        frame(0,scope); CHECK(InputRouter_Released(semantic)); CHECK_ACTION_BLOCKED(next_semantic);
        frame(next_key,scope); CHECK(InputRouter_Pressed(next_semantic));

        /* 显式模态捕获取消资格；即使RB未松、仍在合法范围也不再接受新快捷。 */
        reset_case(); frame(rb|key,scope); CHECK(InputRouter_RbChordPressed(actions[i]));
        InputRouter_CaptureAll(); frame(rb|next_key,scope);
        CHECK(!InputRouter_RbChordPressed(actions[next])); CHECK_ACTION_BLOCKED(next_semantic);
    }

    for (scope_index = 0; scope_index < 2; ++scope_index)
    for (swap = 0; swap < 2; ++swap) for (i = 0; i < 8; ++i) for (order = 0; order < 3; ++order) {
        InputRbChordScope scope = scope_index ? INPUT_RB_CHORD_FREE_IDLE : INPUT_RB_CHORD_BATTLE_TOP;
        u32 key = 1u << keys[i];
        InputAction semantic = i < 2 && swap ? (i == 0 ? INPUT_CANCEL : INPUT_CONFIRM) : actions[i];
        reset_case(); test_config.swap_confirm_cancel = (int)swap;
        /* 先按RB，经过多帧后按动作：必须保留武装。 */
        frame(rb,scope);
        frame(rb,scope);
        frame(rb|key,scope);
        CHECK_ACTION_BLOCKED(semantic);
        CHECK(!exploration_dpad_to_direction());
        CHECK(InputRouter_RbChordPressed(actions[i]));
        CHECK(!InputRouter_RbChordPressed(actions[i]));
        CHECK(!InputRouter_Pressed(semantic));
        /* 原版已打开子菜单：不同松键顺序都不能冒出取消/确认/方向。 */
        frame(order == 0 ? key : (order == 1 ? rb : 0u),INPUT_RB_CHORD_NONE);
        CHECK(!InputRouter_Down(semantic));
        CHECK(!InputRouter_Released(semantic));
        frame(0,INPUT_RB_CHORD_NONE);
        CHECK(!InputRouter_Released(semantic));
        frame(0,INPUT_RB_CHORD_NONE);
        frame(key,INPUT_RB_CHORD_NONE);
        CHECK(InputRouter_Pressed(semantic));
        /* RB提前松一帧仍能组成快捷；超出容错窗则不能。 */
        reset_case(); frame(rb,scope);
        frame(0,scope); frame(key,scope);
        CHECK_ACTION_BLOCKED(semantic);
        CHECK(InputRouter_RbChordPressed(actions[i]));
        reset_case(); frame(rb,scope); frame(0,scope);
        test_tick += 9u; frame(key,scope);
        CHECK(!InputRouter_RbChordPressed(actions[i]));
        CHECK(InputRouter_Pressed(semantic));
        /* 已有模态层捕获时，不得通过组合查询绕过。 */
        reset_case(); frame(rb|key,scope); InputRouter_CaptureAll();
        CHECK(!InputRouter_RbChordPressed(actions[i]));
    }

    /*
     * 不调用任何组合消费者，模拟未来尚未绑定的八个快捷槽：仍必须先拦普通功能。
     * 两套布局、两个合法范围都覆盖；重点验证松RB/换页面/容错到期不会把旧Down补给地图。
     */
    for (scope_index = 0; scope_index < 2; ++scope_index)
    for (swap = 0; swap < 2; ++swap) for (i = 0; i < 8; ++i) {
        InputRbChordScope scope = scope_index ? INPUT_RB_CHORD_FREE_IDLE : INPUT_RB_CHORD_BATTLE_TOP;
        u32 key = 1u << keys[i];
        InputAction semantic = i < 2 && swap ? (i == 0 ? INPUT_CANCEL : INPUT_CONFIRM) : actions[i];
        reset_case(); test_config.swap_confirm_cancel = (int)swap;
        frame(rb,scope); frame(rb|key,scope);
        CHECK_ACTION_BLOCKED(semantic);
        Exploration_Update(); CHECK(g_move_code == 0);
        frame(key,INPUT_RB_CHORD_NONE); CHECK_ACTION_BLOCKED(semantic);
        test_tick += 10u; frame(key,INPUT_RB_CHORD_NONE); CHECK_ACTION_BLOCKED(semantic);
        frame(0,INPUT_RB_CHORD_NONE); CHECK_ACTION_BLOCKED(semantic);
        frame(key,INPUT_RB_CHORD_NONE); CHECK(InputRouter_RawPressed(semantic));

        /* 动作键先按住、再按RB：旧动作立即停止，但不能伪造动作键的新沿来触发快捷。 */
        reset_case(); frame(key,INPUT_RB_CHORD_NONE); CHECK(InputRouter_Down(semantic));
        frame(rb|key,scope); CHECK_ACTION_BLOCKED(semantic);
        CHECK(!InputRouter_RbChordPressed(actions[i]));
        frame(rb,scope); CHECK_ACTION_BLOCKED(semantic);
        frame(rb|key,scope); CHECK_ACTION_BLOCKED(semantic);
        CHECK(InputRouter_RbChordPressed(actions[i]));

        /* 在列表/调查等不允许范围重新按RB，不应吞掉普通八键或RB原功能。 */
        reset_case(); frame(rb|key,INPUT_RB_CHORD_NONE);
        CHECK(InputRouter_Pressed(semantic)); CHECK(InputRouter_RawDown(semantic));
        CHECK(InputRouter_Pressed(INPUT_CATEGORY_NEXT)); CHECK(!InputRouter_RbChordPressed(actions[i]));
        frame(rb,INPUT_RB_CHORD_NONE); CHECK(InputRouter_Released(semantic));
    }

    /* 一个快捷已打开页面，RB仍按住时追加其它七键：不允许它们漏给新页面。 */
    reset_case(); test_config.swap_confirm_cancel = 0;
    frame(rb,INPUT_RB_CHORD_FREE_IDLE); frame(rb|(1u<<PAD_EAST),INPUT_RB_CHORD_FREE_IDLE);
    CHECK(InputRouter_RbChordPressed(INPUT_CANCEL));
    frame(rb|INPUT_RB_ACTION_BUTTONS,INPUT_RB_CHORD_NONE);
    for (i = 0; i < 8; ++i) CHECK_ACTION_BLOCKED(actions[i]);
    frame(INPUT_RB_ACTION_BUTTONS,INPUT_RB_CHORD_NONE);
    for (i = 0; i < 8; ++i) CHECK_ACTION_BLOCKED(actions[i]);
    frame(0,INPUT_RB_CHORD_NONE);
    for (i = 0; i < 8; ++i) CHECK_ACTION_BLOCKED(actions[i]);
    frame(rb|INPUT_RB_ACTION_BUTTONS,INPUT_RB_CHORD_NONE);
    for (i = 0; i < 8; ++i) CHECK(InputRouter_Pressed(actions[i]));
    CHECK(InputRouter_Pressed(INPUT_CATEGORY_NEXT));

    /* 未列入八键的输入不被扩展屏蔽：Start/LB和左摇杆保持自己的既有职责。 */
    reset_case(); frame(rb|(1u<<PAD_START)|(1u<<PAD_LB),INPUT_RB_CHORD_FREE_IDLE);
    CHECK(InputRouter_Pressed(INPUT_SYSTEM_START)); CHECK(InputRouter_Pressed(INPUT_CATEGORY_PREV));
    test_axes[PAD_AXIS_LEFT_X] = 20000; Exploration_Update(); CHECK(g_move_code == 0x27);
    reset_case(); frame(rb,INPUT_RB_CHORD_NONE);
    frame(rb|(1u<<PAD_EAST),INPUT_RB_CHORD_BATTLE_TOP);
    CHECK(!InputRouter_RbChordPressed(INPUT_CANCEL));
    reset_case(); frame(rb,INPUT_RB_CHORD_FREE_IDLE);
    test_foreground = 0; frame(rb,INPUT_RB_CHORD_NONE);
    test_foreground = 1; frame(rb|(1u<<PAD_EAST),INPUT_RB_CHORD_FREE_IDLE);
    CHECK(!InputRouter_RbChordPressed(INPUT_CANCEL));

    /* 默认隐藏、鼠标同帧优先、持杆不回抢、回中后新推杆回切、Start回切。 */
    reset_case(); memset(&g_cursor,0,sizeof(g_cursor)); test_config.cursor_default_hidden = 1;
    test_api.get_cursor_pos = test_get_cursor;
    Cursor_SetInitialControllerMode(1); CHECK(!g_cursor.effective_visible);
    Cursor_Update();
    test_axes[PAD_AXIS_LEFT_X] = 12000; Cursor_Update();
    ++test_point.x; Cursor_Update(); CHECK(!Cursor_ControllerOwnsPointer());
    Cursor_Update(); CHECK(!Cursor_ControllerOwnsPointer());
    test_axes[PAD_AXIS_LEFT_X] = 0; Cursor_Update();
    test_axes[PAD_AXIS_LEFT_X] = 12000; Cursor_Update(); CHECK(Cursor_ControllerOwnsPointer());
    ++test_point.x; Cursor_Update();
    test_axes[PAD_AXIS_RIGHT_X] = 32000; Cursor_Update(); CHECK(!Cursor_ControllerOwnsPointer());
    test_buttons = 1u<<PAD_START; Cursor_Update(); CHECK(Cursor_ControllerOwnsPointer());
    Cursor_SetMouseModeSession(1); CHECK(g_cursor.effective_visible);
    Cursor_SetMouseModeSession(0); CHECK(!g_cursor.effective_visible);
    Cursor_SetInvestigationSession(1); CHECK(g_cursor.effective_visible);
    Cursor_SetInvestigationSession(0); CHECK(!g_cursor.effective_visible);
    Cursor_SetInitialControllerMode(0); CHECK(g_cursor.effective_visible);
    printf("PASS %d input assertions\n", checks);
    return 0;
}
