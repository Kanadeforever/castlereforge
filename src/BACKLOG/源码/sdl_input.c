#include "sdl_input.h"
#include "runtime.h"

/*
 * sdl_input.c
 *
 * SDL3 是完全可选的运行时能力：源码不包含 SDL 头文件，链接也不导入 SDL3.dll。
 * 插件先尝试复用进程里已经由手柄插件加载的 SDL3；如果还没有，再只从本 ASI
 * 同目录寻找 SDL3.dll。所有必需导出都找到并初始化成功后，手柄输入才会启用。
 *
 * 两个插件各自打开一个 SDL_Gamepad 句柄是 SDL 的正常引用计数用法。
 * 本插件不读取/移除 SDL 事件队列，不调用 SDL_Quit，也不会关闭另一个插件的句柄。
 */

#define SDL_INIT_GAMEPAD_ 0x00002000u
#define SDL_RETRY_TICKS   625u /* 625 × 8ms 约等于 5 秒。 */
#define SDL_ENUM_DIVISOR  32u  /* 没有设备时约每 256ms 枚举一次，不每 tick 做重扫描。 */

typedef struct SDL_Gamepad_ SDL_Gamepad_;
typedef u32 SDL_JoystickID_;

typedef u8               (BACKLOG_CDECL *PFN_SDL_Init)(u32 flags);
typedef void             (BACKLOG_CDECL *PFN_SDL_UpdateGamepads)(void);
typedef SDL_JoystickID_* (BACKLOG_CDECL *PFN_SDL_GetGamepads)(int* count);
typedef SDL_Gamepad_*    (BACKLOG_CDECL *PFN_SDL_OpenGamepad)(SDL_JoystickID_ id);
typedef void             (BACKLOG_CDECL *PFN_SDL_CloseGamepad)(SDL_Gamepad_* gamepad);
typedef u8               (BACKLOG_CDECL *PFN_SDL_GamepadConnected)(SDL_Gamepad_* gamepad);
typedef u8               (BACKLOG_CDECL *PFN_SDL_GetGamepadButton)(SDL_Gamepad_* gamepad, int button);
typedef void             (BACKLOG_CDECL *PFN_SDL_free)(void* memory);

typedef struct SdlInputState {
    HMODULE module;
    SDL_Gamepad_* gamepad;

    PFN_SDL_Init init;
    PFN_SDL_UpdateGamepads update_gamepads;
    PFN_SDL_GetGamepads get_gamepads;
    PFN_SDL_OpenGamepad open_gamepad;
    PFN_SDL_CloseGamepad close_gamepad;
    PFN_SDL_GamepadConnected gamepad_connected;
    PFN_SDL_GetGamepadButton get_button;
    PFN_SDL_free free_memory;

    u32 buttons;
    u32 retry_counter;
    u32 enum_counter;
    int initialized;
    int failure_reported;
    int inhibit_until_all_released;
} SdlInputState;

static SdlInputState g_sdl;

/*
 * 名字表集中列出正式名和常见短别名。
 * loose 比较会自动忽略大小写、空格、横线与下划线，因此表里不用重复每种写法。
 */
int SdlInput_ButtonFromName(const char* name) {
    if (Runtime_TextEqualsLoose(name, "south") || Runtime_TextEqualsLoose(name, "a")) return SDL_BUTTON_SOUTH;
    if (Runtime_TextEqualsLoose(name, "east") || Runtime_TextEqualsLoose(name, "b")) return SDL_BUTTON_EAST;
    if (Runtime_TextEqualsLoose(name, "west") || Runtime_TextEqualsLoose(name, "x")) return SDL_BUTTON_WEST;
    if (Runtime_TextEqualsLoose(name, "north") || Runtime_TextEqualsLoose(name, "y")) return SDL_BUTTON_NORTH;
    if (Runtime_TextEqualsLoose(name, "back") || Runtime_TextEqualsLoose(name, "select")) return SDL_BUTTON_BACK;
    if (Runtime_TextEqualsLoose(name, "guide")) return SDL_BUTTON_GUIDE;
    if (Runtime_TextEqualsLoose(name, "start")) return SDL_BUTTON_START;
    if (Runtime_TextEqualsLoose(name, "leftstick") || Runtime_TextEqualsLoose(name, "l3")) return SDL_BUTTON_LEFT_STICK;
    if (Runtime_TextEqualsLoose(name, "rightstick") || Runtime_TextEqualsLoose(name, "r3")) return SDL_BUTTON_RIGHT_STICK;
    if (Runtime_TextEqualsLoose(name, "leftshoulder") || Runtime_TextEqualsLoose(name, "lb")) return SDL_BUTTON_LEFT_SHOULDER;
    if (Runtime_TextEqualsLoose(name, "rightshoulder") || Runtime_TextEqualsLoose(name, "rb")) return SDL_BUTTON_RIGHT_SHOULDER;
    if (Runtime_TextEqualsLoose(name, "dpadup")) return SDL_BUTTON_DPAD_UP;
    if (Runtime_TextEqualsLoose(name, "dpaddown")) return SDL_BUTTON_DPAD_DOWN;
    if (Runtime_TextEqualsLoose(name, "dpadleft")) return SDL_BUTTON_DPAD_LEFT;
    if (Runtime_TextEqualsLoose(name, "dpadright")) return SDL_BUTTON_DPAD_RIGHT;
    if (Runtime_TextEqualsLoose(name, "misc1")) return SDL_BUTTON_MISC1;
    if (Runtime_TextEqualsLoose(name, "rightpaddle1")) return SDL_BUTTON_RIGHT_PADDLE1;
    if (Runtime_TextEqualsLoose(name, "leftpaddle1")) return SDL_BUTTON_LEFT_PADDLE1;
    if (Runtime_TextEqualsLoose(name, "rightpaddle2")) return SDL_BUTTON_RIGHT_PADDLE2;
    if (Runtime_TextEqualsLoose(name, "leftpaddle2")) return SDL_BUTTON_LEFT_PADDLE2;
    if (Runtime_TextEqualsLoose(name, "touchpad")) return SDL_BUTTON_TOUCHPAD;
    if (Runtime_TextEqualsLoose(name, "misc2")) return SDL_BUTTON_MISC2;
    return -1;
}

/* 关闭的只会是本插件自己 OpenGamepad 得到的引用；不会调用 SDL_Quit 或 FreeLibrary。 */
static void sdl_close_gamepad(void) {
    if (g_sdl.gamepad && g_sdl.close_gamepad) g_sdl.close_gamepad(g_sdl.gamepad);
    g_sdl.gamepad = NULL;
    g_sdl.buttons = 0u;
    g_sdl.enum_counter = 0u;
    g_sdl.inhibit_until_all_released = 0;
}

/*
 * 逐个取导出地址。这里用显式赋值而不是链接 import library，所以 SDL3.dll 缺失时
 * Windows 仍能正常加载 Castle_Backlog.asi，键盘功能照常工作。
 */
static int sdl_load_optional_module(void) {
    char sibling_path[MAX_PATH];

    if (g_sdl.initialized) return 1;
    if (!g_sdl.module) g_sdl.module = GetModuleHandleA("SDL3.dll");
    if (!g_sdl.module && Runtime_BuildSiblingPath("SDL3.dll", sibling_path, MAX_PATH)) {
        g_sdl.module = LoadLibraryA(sibling_path);
    }
    if (!g_sdl.module) return 0;

    g_sdl.init = (PFN_SDL_Init)GetProcAddress(g_sdl.module, "SDL_Init");
    g_sdl.update_gamepads = (PFN_SDL_UpdateGamepads)GetProcAddress(g_sdl.module, "SDL_UpdateGamepads");
    g_sdl.get_gamepads = (PFN_SDL_GetGamepads)GetProcAddress(g_sdl.module, "SDL_GetGamepads");
    g_sdl.open_gamepad = (PFN_SDL_OpenGamepad)GetProcAddress(g_sdl.module, "SDL_OpenGamepad");
    g_sdl.close_gamepad = (PFN_SDL_CloseGamepad)GetProcAddress(g_sdl.module, "SDL_CloseGamepad");
    g_sdl.gamepad_connected = (PFN_SDL_GamepadConnected)GetProcAddress(g_sdl.module, "SDL_GamepadConnected");
    g_sdl.get_button = (PFN_SDL_GetGamepadButton)GetProcAddress(g_sdl.module, "SDL_GetGamepadButton");
    g_sdl.free_memory = (PFN_SDL_free)GetProcAddress(g_sdl.module, "SDL_free");

    if (!g_sdl.init || !g_sdl.update_gamepads || !g_sdl.get_gamepads || !g_sdl.open_gamepad ||
        !g_sdl.close_gamepad || !g_sdl.gamepad_connected || !g_sdl.get_button || !g_sdl.free_memory) {
        return 0;
    }
    if (!g_sdl.init(SDL_INIT_GAMEPAD_)) return 0;

    g_sdl.initialized = 1;
    Runtime_Log("[SDL3] 可选手柄能力已初始化；未链接任何强制 SDL 依赖。");
    return 1;
}

/* SDL_GetGamepads 返回 SDL 分配的数组；无论是否成功打开设备，都必须用 SDL_free 归还。 */
static void sdl_try_open_first_gamepad(void) {
    SDL_JoystickID_* ids;
    int count = 0;

    if (!g_sdl.initialized || g_sdl.gamepad) return;
    ids = g_sdl.get_gamepads(&count);
    if (ids && count > 0) {
        g_sdl.gamepad = g_sdl.open_gamepad(ids[0]);
        if (g_sdl.gamepad) {
            /*
             * 新连接时先等所有键松开，避免玩家插线时正按着 LB，第一帧就误打开 Backlog。
             */
            g_sdl.inhibit_until_all_released = 1;
            Runtime_Log("[SDL3] 已打开第一只标准手柄；释放当前按键后开始接收 Backlog 输入。");
        }
    }
    if (ids) g_sdl.free_memory(ids);
}

void SdlInput_Initialize(void) {
    g_sdl.buttons = 0u;
    g_sdl.retry_counter = 0u;
    g_sdl.enum_counter = 0u;
    if (!sdl_load_optional_module()) {
        g_sdl.failure_reported = 1;
        Runtime_Log("[SDL3] 当前没有可用 SDL3；Backlog 键盘功能保持完整，稍后会低频重试。");
    }
}

/*
 * 每个 worker tick 的固定顺序与手柄插件一致：
 * UpdateGamepads -> 检查旧句柄 -> 必要时低频枚举 -> 采样按钮。
 * 这能处理 Windows 热拔插，也不会读取或清空 SDL 事件队列。
 */
void SdlInput_Poll(void) {
    u32 bits = 0u;
    int button;

    if (!g_sdl.initialized) {
        ++g_sdl.retry_counter;
        if (g_sdl.retry_counter < SDL_RETRY_TICKS) return;
        g_sdl.retry_counter = 0u;
        if (!sdl_load_optional_module()) return;
        g_sdl.failure_reported = 0;
    }

    g_sdl.update_gamepads();
    if (g_sdl.gamepad && !g_sdl.gamepad_connected(g_sdl.gamepad)) {
        Runtime_Log("[SDL3] 手柄已断开；Backlog 手柄状态已清零，键盘不受影响。");
        sdl_close_gamepad();
    }

    if (!g_sdl.gamepad) {
        if ((g_sdl.enum_counter++ % SDL_ENUM_DIVISOR) == 0u) sdl_try_open_first_gamepad();
    }
    if (!g_sdl.gamepad) {
        g_sdl.buttons = 0u;
        return;
    }

    for (button = 0; button < SDL_BUTTON_COUNT; ++button) {
        if (g_sdl.get_button(g_sdl.gamepad, button)) bits |= (1u << (u32)button);
    }

    if (g_sdl.inhibit_until_all_released) {
        g_sdl.buttons = 0u;
        if (bits == 0u) g_sdl.inhibit_until_all_released = 0;
        return;
    }
    g_sdl.buttons = bits;
}

int SdlInput_Down(int button) {
    if (button < 0 || button >= SDL_BUTTON_COUNT) return 0;
    return (g_sdl.buttons & (1u << (u32)button)) != 0u;
}

void SdlInput_Shutdown(void) {
    sdl_close_gamepad();
}
