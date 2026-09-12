/*
 * 输入行为测试：直接编译生产 input_router.c/cursor.c，不复制它们的判断公式。
 * 假设备只提供逐帧按钮与指针；不启动RPG、不读取存档、不写游戏地址。
 */
#include <stdio.h>
#include <string.h>
#undef NULL
#include "../source/input_router.c"
#include "../source/cursor.c"

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
static BOOL WINAPI test_get_cursor(Point32* p) { *p = test_point; return 1; }

/* 失败时报告用例所在行；保留所有断言计数，便于结果接档。 */
#define CHECK(x) do { ++checks; if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)
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
    static const PadButton keys[] = {PAD_SOUTH,PAD_EAST,PAD_WEST,PAD_NORTH,PAD_DPAD_UP,PAD_DPAD_DOWN};
    static const InputAction actions[] = {INPUT_CONFIRM,INPUT_CANCEL,INPUT_SPECIAL_X,INPUT_SPECIAL_Y,INPUT_NAV_UP,INPUT_NAV_DOWN};
    unsigned int i, swap, order;
    const u32 rb = 1u << PAD_RB;
    for (swap = 0; swap < 2; ++swap) for (i = 0; i < 6; ++i) for (order = 0; order < 3; ++order) {
        u32 key = 1u << keys[i];
        InputAction semantic = i < 2 && swap ? (i == 0 ? INPUT_CANCEL : INPUT_CONFIRM) : actions[i];
        reset_case(); test_config.swap_confirm_cancel = (int)swap;
        /* 先按RB，经过多帧后按动作：必须保留武装。 */
        frame(rb,INPUT_RB_CHORD_BATTLE_TOP);
        frame(rb,INPUT_RB_CHORD_BATTLE_TOP);
        frame(rb|key,INPUT_RB_CHORD_BATTLE_TOP);
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
        reset_case(); frame(rb,INPUT_RB_CHORD_BATTLE_TOP);
        frame(0,INPUT_RB_CHORD_BATTLE_TOP); frame(key,INPUT_RB_CHORD_BATTLE_TOP);
        CHECK(InputRouter_RbChordPressed(actions[i]));
        reset_case(); frame(rb,INPUT_RB_CHORD_BATTLE_TOP); frame(0,INPUT_RB_CHORD_BATTLE_TOP);
        test_tick += 9u; frame(key,INPUT_RB_CHORD_BATTLE_TOP);
        CHECK(!InputRouter_RbChordPressed(actions[i]));
        /* 已有模态层捕获时，不得通过组合查询绕过。 */
        reset_case(); frame(rb|key,INPUT_RB_CHORD_BATTLE_TOP); InputRouter_CaptureAll();
        CHECK(!InputRouter_RbChordPressed(actions[i]));
    }
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
