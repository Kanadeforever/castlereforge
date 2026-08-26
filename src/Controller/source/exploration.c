#include "exploration.h"
#include "runtime.h"
#include "game_addresses.h"
#include "pad_input.h"
#include "input_router.h"
#include "interface_shell.h"
#include "control_modes.h"

/*
 * 这里的 move_code 不是 Windows 键盘注入。
 * 它只是保存“左摇杆希望原版移动 resolver 返回哪个方向码”，真正消费发生在游戏自己的调用点。
 */
static volatile int g_move_code;
/* 只有十字键方向把本帧锁为步行；左摇杆仍按既有径向阈值自动走/跑。 */
static volatile int g_move_force_walk;
static i32 g_run_threshold_sq;
static int g_last_run_mode_logged = -1;

static int exp_abs(int v) { return v < 0 ? -v : v; }

/*
 * 把左摇杆八方向量化为原版 0x21..0x28 移动码。
 * 死区内返回 0；斜向和直向判定沿用 dev12 已实机确认的映射。
 */
static int exploration_stick_to_direction(i16 x, i16 y) {
    int ax = exp_abs((int)x);
    int ay = exp_abs((int)y);

    if (ax < PAD_STICK_DEADZONE && ay < PAD_STICK_DEADZONE) return 0;
    if (ax * 2 < ay) return y < 0 ? 0x26 : 0x28; /* 上 / 下 */
    if (ay * 2 < ax) return x < 0 ? 0x25 : 0x27; /* 左 / 右 */
    if (x >= 0 && y < 0) return 0x21;  /* 右上 */
    if (x >= 0 && y >= 0) return 0x22; /* 右下 */
    if (x < 0 && y >= 0) return 0x23;  /* 左下 */
    return 0x24;                       /* 左上 */
}

/*
 * 地图十字键同样映射到原版八方向码。相邻两键允许斜走；相反方向同时按下则在该轴互相抵消。
 * 这里只读取语义层，不读取 SDL 物理编号；是否真的移动仍由 RPG.exe 地图 resolver 的调用时机决定。
 */
static int exploration_dpad_to_direction(void) {
    int left = InputRouter_DownOn(INPUT_CTX_EXPLORATION, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY);
    int right = InputRouter_DownOn(INPUT_CTX_EXPLORATION, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY);
    int up = InputRouter_DownOn(INPUT_CTX_EXPLORATION, INPUT_NAV_UP, INPUT_LAYER_OVERLAY);
    int down = InputRouter_DownOn(INPUT_CTX_EXPLORATION, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY);
    int x = (right ? 1 : 0) - (left ? 1 : 0);
    int y = (down ? 1 : 0) - (up ? 1 : 0);

    if (x == 0 && y == 0) return 0;
    if (x > 0 && y < 0) return 0x21;  /* 右上 */
    if (x > 0 && y > 0) return 0x22;  /* 右下 */
    if (x < 0 && y > 0) return 0x23;  /* 左下 */
    if (x < 0 && y < 0) return 0x24;  /* 左上 */
    if (x < 0) return 0x25;           /* 左 */
    if (y < 0) return 0x26;           /* 上 */
    if (x > 0) return 0x27;           /* 右 */
    return 0x28;                      /* 下 */
}

/* 用径向平方与配置阈值平方比较，避免开平方并保持任意方向的走/跑阈值一致。 */
static int exploration_left_stick_wants_run(void) {
    i32 x = (i32)PadInput_Axis(PAD_AXIS_LEFT_X);
    i32 y = (i32)PadInput_Axis(PAD_AXIS_LEFT_Y);
    i32 magnitude_sq = x * x + y * y;
    return magnitude_sq > g_run_threshold_sq;
}

/*
 * 先调用原版 resolver；只有原版没有产生键鼠方向且游戏在前台时，才把左摇杆方向补进 out_dir。
 * 因此键盘/鼠标永远拥有优先权。
 */
static i32 CDECL Exploration_HookMove(i32 a, i32 b, i32 c, i32 d, i32* out_dir) {
    PFN_ExplorationMove orig = (PFN_ExplorationMove)FN_EXPLORATION_MOVE;
    i32 real_value = orig(a, b, c, d, out_dir);
    int code = g_move_code;

    /*
     * 原游戏仍先完整执行。只有摇杆确实给出八方向时，才替换最终 resolver 返回值。
     * 因此剧情、菜单、键盘本身的内部状态机都没有被我们跳过。
     */
    if (code >= 0x21 && code <= 0x28) return (i32)code;
    return real_value;
}

/*
 * 先保留原版鼠标动作码；只有原版返回 0 且左摇杆正在移动时，才在走/跑 profile 需要切换时返回原版 0x52。
 * 插件不直接改速度倍率。
 */
static i32 CDECL Exploration_HookMouseAction(void) {
    PFN_MouseAction orig = (PFN_MouseAction)FN_MOUSE_ACTION;
    PFN_GetControlledActor get_actor = (PFN_GetControlledActor)FN_GET_CONTROLLED_ACTOR;
    i32 real_code;
    u8* actor;
    int desired_run;
    int current_run;

    /*
     * 原版鼠标动作 resolver 永远先执行。
     * refactor19 在它返回以后立刻给 InterfaceShell 一个“游戏线程安全点”：
     * 如果 worker 已经捕获地图 Y，这里只调用原版 Space 分支最终使用的 0x40B230(1)。
     * 注意：这不是发送 Space，也不修改 real_code；探索鼠标/走跑业务仍保持原样。
     */
    real_code = orig();
    InterfaceShell_OnExplorationGameThread();

    /* 真实鼠标/键盘本帧已经产生动作时绝不覆盖。 */
    if (real_code != 0) return real_code;
    if (!PadInput_GameForeground(NULL) || g_move_code < 0x21 || g_move_code > 0x28) return real_code;

    actor = (u8*)get_actor();
    if (!Runtime_PtrOk(actor)) return real_code;

    desired_run = g_move_force_walk ? 0 : exploration_left_stick_wants_run();
    current_run = (*(u32*)(actor + 0x0Cu) & 0x0000FF00u) != 0;
    if (current_run != desired_run) {
        if (g_last_run_mode_logged != desired_run) {
            if (g_move_force_walk) Runtime_Log("[移动] 地图十字键方向强制使用步行档。");
            else Runtime_Log(desired_run ? "[移动] 左摇杆幅度进入跑步区间。" : "[移动] 左摇杆幅度进入步行区间。");
            g_last_run_mode_logged = desired_run;
        }
        /* 原版 0x40A510 会消费 0x52 来切换 Walk/Run 状态。 */
        return 0x52;
    }

    g_last_run_mode_logged = desired_run;
    return real_code;
}

/* 将百分比阈值预先换成摇杆平方阈值，并安装“方向 resolver + 0x52 状态转换”两条 Hook。 */
int Exploration_InstallHooks(void) {
    u32 threshold = Runtime_Config()->run_threshold_percent;
    i32 raw = (i32)((32767u * threshold) / 100u);
    g_run_threshold_sq = raw * raw;

    if (!Runtime_PatchCall(CALL_EXPLORATION_MOVE, (void*)Exploration_HookMove, FN_EXPLORATION_MOVE)) {
        Runtime_Log("[致命] 地图左摇杆移动 Hook 安装失败。");
        return 0;
    }
    if (!Runtime_PatchCall(CALL_MOUSE_ACTION, (void*)Exploration_HookMouseAction, FN_MOUSE_ACTION)) {
        Runtime_Log("[致命] 地图走跑状态 Hook 安装失败。");
        return 0;
    }
    return 1;
}

/*
 * worker 每轮只缓存方向意图；真正返回移动码仍等游戏线程调用 Exploration_HookMove。
 * 十字键优先于左摇杆，而且只给“步行”；十字键松开后立即恢复左摇杆八方向量化与幅度走/跑逻辑。
 */
void Exploration_Update(void) {
    int dpad_code;

    if (!PadInput_GameForeground(NULL)) {
        g_move_code = 0;
        g_move_force_walk = 0;
        return;
    }

    /* Back常驻、地图RT临时鼠标与LT调查中，左杆/十字键都不得保留角色移动意图。 */
    if (ControlModes_BlocksMapMovement()) {
        g_move_code = 0;
        g_move_force_walk = 0;
        return;
    }

    dpad_code = exploration_dpad_to_direction();
    if (dpad_code != 0) {
        g_move_code = dpad_code;
        g_move_force_walk = 1;
        return;
    }

    g_move_force_walk = 0;
    g_move_code = exploration_stick_to_direction(PadInput_Axis(PAD_AXIS_LEFT_X), PadInput_Axis(PAD_AXIS_LEFT_Y));
}
