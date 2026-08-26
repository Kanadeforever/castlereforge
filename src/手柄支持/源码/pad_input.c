#include "pad_input.h"
#include "runtime.h"

/* SDL3 只在运行时动态加载，因此源码包不要求 SDL3 头文件和 import lib。 */
#define SDL_INIT_GAMEPAD_ 0x00002000u

typedef struct SDL_Gamepad_ SDL_Gamepad_;
typedef u32 SDL_JoystickID_;

typedef u8               (CDECL *PFN_SDL_Init)(u32);
typedef void             (CDECL *PFN_SDL_UpdateGamepads)(void);
typedef SDL_JoystickID_* (CDECL *PFN_SDL_GetGamepads)(int*);
typedef SDL_Gamepad_*    (CDECL *PFN_SDL_OpenGamepad)(SDL_JoystickID_);
typedef void             (CDECL *PFN_SDL_CloseGamepad)(SDL_Gamepad_*);
typedef u8               (CDECL *PFN_SDL_GamepadConnected)(SDL_Gamepad_*);
typedef u8               (CDECL *PFN_SDL_GetGamepadButton)(SDL_Gamepad_*,int);
typedef i16              (CDECL *PFN_SDL_GetGamepadAxis)(SDL_Gamepad_*,int);
typedef u8               (CDECL *PFN_SDL_RumbleGamepad)(SDL_Gamepad_*,u16,u16,u32);
typedef void             (CDECL *PFN_SDL_free)(void*);
typedef const char*      (CDECL *PFN_SDL_GetError)(void);

typedef struct PadInputState {
    HMODULE sdl_module;
    SDL_Gamepad_* gamepad;

    PFN_SDL_Init init;
    PFN_SDL_UpdateGamepads update_gamepads;
    PFN_SDL_GetGamepads get_gamepads;
    PFN_SDL_OpenGamepad open_gamepad;
    PFN_SDL_CloseGamepad close_gamepad;
    PFN_SDL_GamepadConnected gamepad_connected;
    PFN_SDL_GetGamepadButton get_button;
    PFN_SDL_GetGamepadAxis get_axis;
    PFN_SDL_RumbleGamepad rumble_gamepad;
    PFN_SDL_free free_fn;
    PFN_SDL_GetError get_error;

    u32 buttons;
    u32 prev_buttons;
    /* SDL3 标准 Gamepad 一共 6 根轴：两根左摇杆、两根右摇杆、LT、RT。 */
    i16 axes[PAD_AXIS_COUNT];
    u32 open_poll_counter;

    /*
     * 热插拔重新打开手柄后的第一帧只做“同步当前状态”，不产生 Pressed/Released 边沿。
     * 例如玩家插线时正好按着 A，如果不做这层保护，重新连接的第一帧会被误认为一次新的 A 确认。
     */
    int suppress_edges_once;

    /*
     * 只有“曾经打开过一只手柄 -> 断开 -> 又打开新句柄”时才置 1。
     * 第一帧状态同步结束后用它决定是否打印“热插拔恢复完成”，启动时第一次打开不会冒充重连。
     */
    int reconnect_sync_pending;

    /*
     * 只用于把首次打开与热插拔重新打开的中文日志区分开。
     * 它不参与设备选择，也不保存任何 SDL 对象地址。
     */
    int ever_opened_gamepad;

    /*
     * SDL3.dll 暂时不存在时，不应该每 8 ms 都 LoadLibrary + 写一行失败日志。
     * retry_after_tick 记录下一次允许重试的 worker tick；failure_reported 只让首次失败说明写入日志。
     */
    u32 retry_after_tick;
    int failure_reported;

    /* 高优先级长震期间，低优先级短提示不得覆盖 SDL 的剩余持续时间。 */
    u32 rumble_until_tick;
    int rumble_priority;
    int initialized;
} PadInputState;

static PadInputState g_pad;

/*
 * 只从 ASI 同目录动态加载 SDL3.dll，并逐个解析本项目真正需要的导出。
 * 失败不是游戏致命错误：返回 0 后原版键鼠继续工作，worker 会按低频策略重试。
 */
static int pad_load_sdl(int verbose_failure) {
    const RuntimeApi* api = Runtime_Api();
    char path[MAX_PATH_];

    if (g_pad.initialized) return 1;
    if (!api->load_library_a || !api->get_proc_address) return 0;
    if (!Runtime_BuildSiblingPath("SDL3.dll", path, MAX_PATH_)) {
        if (verbose_failure) Runtime_Log("[SDL3] 无法生成 SDL3.dll 的同目录绝对路径，暂时停用手柄输入。");
        return 0;
    }

    /*
     * 只从 ASI 所在目录加载 SDL3.dll。
     * 不依赖当前工作目录，避免游戏从快捷方式/Steam 启动时找错 DLL。
     */
    /*
     * 如果前一次已经成功 LoadLibrary、只是 SDL_Init 暂时失败，就直接复用模块句柄。
     * 这样重试不会不断增加同一个 DLL 的引用计数。
     */
    if (!g_pad.sdl_module) g_pad.sdl_module = api->load_library_a(path);
    if (!g_pad.sdl_module) {
        if (verbose_failure) Runtime_Log("[SDL3] 同目录 SDL3.dll 加载失败；原版键鼠不受影响，手柄会低频重试。");
        return 0;
    }

#define PAD_RESOLVE(field, type, name) g_pad.field = (type)api->get_proc_address(g_pad.sdl_module, name)
    PAD_RESOLVE(init, PFN_SDL_Init, "SDL_Init");
    PAD_RESOLVE(update_gamepads, PFN_SDL_UpdateGamepads, "SDL_UpdateGamepads");
    PAD_RESOLVE(get_gamepads, PFN_SDL_GetGamepads, "SDL_GetGamepads");
    PAD_RESOLVE(open_gamepad, PFN_SDL_OpenGamepad, "SDL_OpenGamepad");
    PAD_RESOLVE(close_gamepad, PFN_SDL_CloseGamepad, "SDL_CloseGamepad");
    PAD_RESOLVE(gamepad_connected, PFN_SDL_GamepadConnected, "SDL_GamepadConnected");
    PAD_RESOLVE(get_button, PFN_SDL_GetGamepadButton, "SDL_GetGamepadButton");
    PAD_RESOLVE(get_axis, PFN_SDL_GetGamepadAxis, "SDL_GetGamepadAxis");
    /* 可选导出：不放进下面的必需导出门，老 SDL3.dll 仍可使用。 */
    PAD_RESOLVE(rumble_gamepad, PFN_SDL_RumbleGamepad, "SDL_RumbleGamepad");
    PAD_RESOLVE(free_fn, PFN_SDL_free, "SDL_free");
    PAD_RESOLVE(get_error, PFN_SDL_GetError, "SDL_GetError");
#undef PAD_RESOLVE

    if (!g_pad.init || !g_pad.update_gamepads || !g_pad.get_gamepads || !g_pad.open_gamepad ||
        !g_pad.close_gamepad || !g_pad.gamepad_connected || !g_pad.get_button || !g_pad.get_axis || !g_pad.free_fn) {
        if (verbose_failure) Runtime_Log("[SDL3] 必需导出函数不完整；当前 SDL3.dll 与本插件接口不兼容。");
        return 0;
    }

    Runtime_PinModuleFromAddress("SDL3 已固定驻留", (const void*)g_pad.init);
    if (!g_pad.init(SDL_INIT_GAMEPAD_)) {
        if (verbose_failure) Runtime_Log("[SDL3] SDL_Init(SDL_INIT_GAMEPAD) 失败；稍后会低频重试。");
        return 0;
    }

    g_pad.initialized = 1;
    Runtime_Log("[SDL3] 手柄子系统初始化成功。");
    return 1;
}

/*
 * 把当前采样缓存归零。
 * 设备断开时必须同时清按钮、上一帧按钮和全部轴，避免旧的摇杆/扳机值在没有手柄时继续残留。
 */
static void pad_clear_sample_state(void) {
    int axis_index;

    g_pad.buttons = 0;
    g_pad.prev_buttons = 0;
    for (axis_index = 0; axis_index < PAD_AXIS_COUNT; ++axis_index) {
        g_pad.axes[axis_index] = 0;
    }
}

/*
 * 安全关闭当前 SDL_Gamepad。
 * reset_open_poll 非零时把枚举计数器清零，使下一次 Poll 立即重新扫描设备，而不是最多再等一秒。
 */
static void pad_close_current_gamepad(int reset_open_poll, const char* log_text) {
    if (g_pad.gamepad && g_pad.close_gamepad) {
        if (g_pad.rumble_gamepad) g_pad.rumble_gamepad(g_pad.gamepad, 0u, 0u, 0u);
        g_pad.close_gamepad(g_pad.gamepad);
    }
    g_pad.gamepad = NULL;
    g_pad.suppress_edges_once = 0;
    g_pad.reconnect_sync_pending = 0;
    g_pad.rumble_until_tick = 0u;
    g_pad.rumble_priority = 0;
    if (reset_open_poll) g_pad.open_poll_counter = 0;
    pad_clear_sample_state();

    if (log_text) Runtime_Log(log_text);
}

/*
 * 枚举 SDL3 当前手柄并只打开第一只。
 * 数组由 SDL 分配，必须使用同一个 SDL3.dll 的 SDL_free 归还。
 *
 * 注意：设备刷新由 PadInput_Poll/Initialize 在调用本函数前完成；这里不再偷偷 pump 第二次，
 * 这样一个 worker tick 只有一个明确的 SDL_UpdateGamepads 时间点。
 */
static void pad_try_open_first_gamepad(void) {
    SDL_JoystickID_* ids;
    int count = 0;

    if (!g_pad.initialized || g_pad.gamepad) return;

    ids = g_pad.get_gamepads(&count);
    if (ids && count > 0) {
        g_pad.gamepad = g_pad.open_gamepad(ids[0]);
        if (g_pad.gamepad) {
            /*
             * 新句柄的第一帧只同步状态，不把“插线时已经按住的键”当成新按下沿。
             * 这也是热插拔不会误操作当前菜单的关键边界。
             */
            g_pad.suppress_edges_once = 1;
            g_pad.open_poll_counter = 0;
            g_pad.reconnect_sync_pending = g_pad.ever_opened_gamepad ? 1 : 0;

            if (g_pad.reconnect_sync_pending) {
                Runtime_Log("[SDL3] 已检测到热插拔手柄并重新打开；输入将在本帧同步后自动恢复。");
            } else {
                Runtime_Log("[SDL3] 已打开第一个可用手柄。");
                g_pad.ever_opened_gamepad = 1;
            }
        }
    }
    if (ids) g_pad.free_fn(ids);
}

/* 启动时先尝试一次 SDL3；失败记录下一次 5 秒后的重试 tick，不进入高频失败循环。 */
int PadInput_Initialize(void) {
    if (!pad_load_sdl(1)) {
        /* 5 秒约等于 625 个 8 ms worker tick。首次失败已经写日志，后续重试保持安静。 */
        g_pad.failure_reported = 1;
        g_pad.retry_after_tick = Runtime_Tick() + Runtime_MsToTicks(5000u);
        return 0;
    }
    g_pad.failure_reported = 0;

    /*
     * SDL3 官方说明：没有使用 SDL 事件循环时，应主动调用 SDL_UpdateGamepads。
     * 初始化成功后先刷新一次设备层，再做第一次枚举，保证“游戏启动时已经插着手柄”的路径与后续热插拔同构。
     */
    g_pad.update_gamepads();
    pad_try_open_first_gamepad();
    return 1;
}

/* 关闭当前 SDL_Gamepad 句柄并清零输入；SDL 模块本身已 PIN，进程结束时由系统回收。 */
void PadInput_Shutdown(void) {
    pad_close_current_gamepad(0, NULL);
}

/*
 * 每 tick 只采样项目需要的数字键、四根摇杆轴和两根 LT/RT 触发器轴，并把数字语义压成 bitset。
 * 这里不解释“确认/取消”等意义，那是 input_router 的职责。
 */
static u32 pad_read_buttons(void) {
    u32 bits = 0;
    int b;
    static const int buttons_to_read[] = {
        PAD_SOUTH, PAD_EAST, PAD_WEST, PAD_NORTH, PAD_BACK, PAD_START, PAD_R3,
        PAD_LB, PAD_RB, PAD_DPAD_UP, PAD_DPAD_DOWN, PAD_DPAD_LEFT, PAD_DPAD_RIGHT
    };

    if (!g_pad.gamepad) return 0;

    /*
     * 连接状态已经在 PadInput_Poll 里、紧跟 SDL_UpdateGamepads 之后检查过。
     * 这里不再先查旧状态再更新 SDL，否则 Windows 热拔插时可能永远看到上一轮缓存的“仍连接”。
     */
    for (b = 0; b < (int)(sizeof(buttons_to_read) / sizeof(buttons_to_read[0])); ++b) {
        int id = buttons_to_read[b];
        if (g_pad.get_button(g_pad.gamepad, id)) bits |= (1u << (u32)id);
    }

    g_pad.axes[PAD_AXIS_LEFT_X] = g_pad.get_axis(g_pad.gamepad, PAD_AXIS_LEFT_X);
    g_pad.axes[PAD_AXIS_LEFT_Y] = g_pad.get_axis(g_pad.gamepad, PAD_AXIS_LEFT_Y);
    g_pad.axes[PAD_AXIS_RIGHT_X] = g_pad.get_axis(g_pad.gamepad, PAD_AXIS_RIGHT_X);
    g_pad.axes[PAD_AXIS_RIGHT_Y] = g_pad.get_axis(g_pad.gamepad, PAD_AXIS_RIGHT_Y);

    /*
     * SDL3 的 LT/RT 是轴，不会出现在 SDL_GetGamepadButton() 的按钮枚举里。
     * 这里先读取标准 axis 4/5，再在本地转换成 PAD_LT/PAD_RT 两个数字状态位。
     * 这样 InputRouter 后面仍然可以统一使用 Pressed/Down/Released，菜单代码不需要理解模拟量。
     */
    g_pad.axes[PAD_AXIS_LEFT_TRIGGER] = g_pad.get_axis(g_pad.gamepad, PAD_AXIS_LEFT_TRIGGER);
    g_pad.axes[PAD_AXIS_RIGHT_TRIGGER] = g_pad.get_axis(g_pad.gamepad, PAD_AXIS_RIGHT_TRIGGER);
    if (g_pad.axes[PAD_AXIS_LEFT_TRIGGER] >= PAD_TRIGGER_DIGITAL_THRESHOLD) bits |= (1u << (u32)PAD_LT);
    if (g_pad.axes[PAD_AXIS_RIGHT_TRIGGER] >= PAD_TRIGGER_DIGITAL_THRESHOLD) bits |= (1u << (u32)PAD_RT);

    return bits;
}

/* worker 每轮唯一采样入口：处理 SDL 重试、低频热插拔枚举、上一帧按钮备份和本帧新状态。 */
int PadInput_Poll(void) {
    if (!g_pad.initialized) {
        /*
         * SDL3 在启动瞬间缺失并不是致命错误：Hook 已经安全安装，原版键鼠仍正常。
         * 这里只每 5 秒重试一次，避免日志刷屏，也允许用户在调试时补上 SDL3.dll 后恢复手柄。
         */
        if ((i32)(Runtime_Tick() - g_pad.retry_after_tick) < 0) return 0;
        if (!pad_load_sdl(g_pad.failure_reported ? 0 : 1)) {
            g_pad.failure_reported = 1;
            g_pad.retry_after_tick = Runtime_Tick() + Runtime_MsToTicks(5000u);
            return 0;
        }
        g_pad.failure_reported = 0;
    }

    /*
     * 热插拔修复的核心顺序：
     *
     *   SDL_UpdateGamepads
     *        ↓
     *   检查旧句柄是否仍连接
     *        ↓
     *   没有句柄时再 SDL_GetGamepads / SDL_OpenGamepad
     *        ↓
     *   最后读取按钮与轴
     *
     * r21 的代码把 SDL_GamepadConnected 放在 SDL_UpdateGamepads 之前，而且“没有 gamepad 句柄”时根本不调用 UpdateGamepads。
     * 在没有 SDL 事件循环的本插件里，这会让设备层停留在旧快照：拔掉后旧句柄可能迟迟不失效，
     * 重新插入后 SDL_GetGamepads 也可能一直看不到新设备。
     * SDL3 官方 API 明确要求这种使用方式主动 pump SDL_UpdateGamepads，所以这里每个 worker tick 先更新一次。
     */
    g_pad.update_gamepads();

    if (g_pad.gamepad && !g_pad.gamepad_connected(g_pad.gamepad)) {
        pad_close_current_gamepad(1, "[SDL3] 手柄已断开；输入状态已清零，开始等待热插拔重新连接。");
    }

    if (!g_pad.gamepad) {
        /*
         * SDL_UpdateGamepads 仍然每 tick 执行，真正较重的 SDL_GetGamepads 只约每 256 ms 做一次。
         * 断开时 pad_close_current_gamepad 会把计数器清零，因此同一 tick 就会先尝试一次重新枚举。
         */
        if ((g_pad.open_poll_counter++ & 0x1Fu) == 0u) {
            pad_try_open_first_gamepad();
        }
    }

    g_pad.prev_buttons = g_pad.buttons;
    g_pad.buttons = pad_read_buttons();

    if (!g_pad.gamepad) {
        /* 没有设备时绝不能保留上一只手柄的任何模拟量。 */
        pad_clear_sample_state();
    } else if (g_pad.suppress_edges_once) {
        /*
         * 新连接第一帧只同步“现在按着什么”：prev=current 后 Pressed/Released 都为 0。
         * 第二帧起恢复正常边沿，因此热插拔本身不会在菜单里误按 A/B/X/Y。
         */
        g_pad.prev_buttons = g_pad.buttons;
        g_pad.suppress_edges_once = 0;
        if (g_pad.reconnect_sync_pending) {
            g_pad.reconnect_sync_pending = 0;
            Runtime_Log("[SDL3] 热插拔手柄状态同步完成；正常按键边沿已恢复。");
        }
    }
    return 1;
}

/* 查询本帧是否持续按住一个物理按钮。 */
int PadInput_Down(PadButton button) {
    return (g_pad.buttons & (1u << (u32)button)) != 0;
}

/* 本帧为 1、上一帧为 0 才算按下沿，保证 A 一次按压只触发一次业务动作。 */
int PadInput_Pressed(PadButton button) {
    u32 bit = 1u << (u32)button;
    return (g_pad.buttons & bit) != 0 && (g_pad.prev_buttons & bit) == 0;
}

/* 本帧为 0、上一帧为 1 才算松开沿；模式层用它建立防穿透释放屏障。 */
int PadInput_Released(PadButton button) {
    u32 bit = 1u << (u32)button;
    return (g_pad.buttons & bit) == 0 && (g_pad.prev_buttons & bit) != 0;
}

i16 PadInput_Axis(PadAxis axis) {
    if ((int)axis < 0 || (int)axis >= PAD_AXIS_COUNT) return 0;
    return g_pad.axes[(int)axis];
}

int PadInput_HasAnyActivity(int include_r3) {
    u32 mask = g_pad.buttons;
    int i;

    /* r37 删除普通态R3鼠标复合操作；保留参数只为兼容既有调用边界。 */
    if (!include_r3) mask &= ~(1u << (u32)PAD_R3);

    /*
     * Back 已提升为 ControlModes 的显式模式键。这里继续排除它，避免物理鼠标接管后
     * 同一颗尚未松开的 Back 又从底层通用活动路径抢回所有权。
     */
    mask &= ~(1u << (u32)PAD_BACK);

    /*
     * Start 是“流程按键”，不是“指针/菜单导航活动”。
     * refactor4 封版时根本没有采样 Start；现在新增动画跳过时，如果把 Start 也算成通用手柄活动，
     * 就会顺手改变 Cursor 的所有权和显隐，这是本功能完全不需要的副作用。
     * 所以这里始终把 Start 从 Cursor 活动判断里排除：Start 只交给输入语义层和 Movie Context。
     */
    mask &= ~(1u << (u32)PAD_START);
    if (mask != 0u) return 1;

    /* 普通手柄态只有左摇杆仍有业务；右摇杆必须保持完全无功能、也不参与所有权回抢。 */
    for (i = PAD_AXIS_LEFT_X; i <= PAD_AXIS_LEFT_Y; ++i) {
        int value = (int)g_pad.axes[i];
        if (value < 0) value = -value;
        if (value >= PAD_STICK_DEADZONE) return 1;
    }
    return 0;
}

/* 通过前台窗口 PID 与当前进程 PID 比较，阻止游戏在后台时模拟鼠标或角色移动。 */
int PadInput_GameForeground(HWND* out_hwnd) {
    const RuntimeApi* api = Runtime_Api();
    HWND hwnd;
    DWORD pid = 0;

    if (out_hwnd) *out_hwnd = NULL;
    if (!api->get_foreground_window || !api->get_window_thread_process_id || !api->get_current_process_id) return 0;
    hwnd = api->get_foreground_window();
    if (!hwnd) return 0;
    api->get_window_thread_process_id(hwnd, &pid);
    if (pid != api->get_current_process_id()) return 0;
    if (out_hwnd) *out_hwnd = hwnd;
    return 1;
}

/*
 * 只报告 SDL 输入层是否完成 pad_load_sdl() 的全部初始化步骤。
 * 与“DLL 是否被 LoadLibrary 成功”不同：缺导出或 SDL_Init 失败时模块句柄可能已经存在，
 * 但 initialized 仍为 0，因此公共 API 不会误报 ready。
 */
int PadInput_Ready(void) { return g_pad.initialized != 0; }
int PadInput_GamepadConnected(void) { return g_pad.gamepad != NULL; }
HMODULE PadInput_SdlModule(void) { return g_pad.sdl_module; }

int PadInput_Rumble(u16 low_frequency, u16 high_frequency, u32 duration_ms, int priority) {
    u32 now = Runtime_Tick();
    int accepted;

    if (!g_pad.gamepad || !g_pad.rumble_gamepad || duration_ms == 0u) return 0;
    if ((i32)(now - g_pad.rumble_until_tick) >= 0) {
        g_pad.rumble_until_tick = 0u;
        g_pad.rumble_priority = 0;
    }
    if (priority < g_pad.rumble_priority) return 0;

    accepted = g_pad.rumble_gamepad(g_pad.gamepad, low_frequency, high_frequency,
                                    duration_ms) ? 1 : 0;
    if (accepted) {
        g_pad.rumble_priority = priority;
        g_pad.rumble_until_tick = now + Runtime_MsToTicks(duration_ms);
    }
    return accepted;
}
