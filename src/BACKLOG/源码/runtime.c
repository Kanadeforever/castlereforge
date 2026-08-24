#include "runtime.h"
#include "game_addresses.h"
#include "sdl_input.h"

/*
 * runtime.c
 *
 * 这里集中处理五件基础工作：
 * 1. 找到插件自身目录；
 * 2. 读取并校验 INI；
 * 3. 写中文运行日志；
 * 4. 判断游戏是否在前台；
 * 5. 在任何地址写入发生前，确认当前 RPG.exe 仍符合已知机器协议。
 *
 * 业务层不重复实现这些工作。这样以后修改配置格式或增加一个版本门时，
 * 不会误碰历史记录环形队列、原版对话框切换或 SDL 热插拔逻辑。
 */

static HMODULE g_plugin_module;
static RuntimeConfig g_config;
static char g_plugin_path[MAX_PATH];
static HANDLE g_log_file = INVALID_HANDLE_VALUE;

/* 用最普通的循环计算 NUL 结尾字符串长度，避免链接 C 运行库 strlen。 */
static u32 runtime_text_length(const char* text) {
    u32 length = 0u;
    if (!text) return 0u;
    while (text[length] != '\0') ++length;
    return length;
}

/*
 * 把 source 复制到固定大小 output。
 * 最后永远补 NUL，所以即使 INI 中有人写了过长文本，也不会让后续代码越界读取。
 */
static void runtime_copy_text(char* output, u32 output_size, const char* source) {
    u32 index = 0u;
    if (!output || output_size == 0u) return;
    if (source) {
        while (index + 1u < output_size && source[index] != '\0') {
            output[index] = source[index];
            ++index;
        }
    }
    output[index] = '\0';
}

/* 小写化只处理 INI 键名会出现的 ASCII 字母，不碰中文或 CP950 正文。 */
static char runtime_ascii_lower(char value) {
    if (value >= 'A' && value <= 'Z') return (char)(value + ('a' - 'A'));
    return value;
}

/*
 * loose 比较会忽略空格、横线和下划线。
 * 因此 left_shoulder、left-shoulder、Left Shoulder 都会被认成同一个 SDL 按钮名。
 */
int Runtime_TextEqualsLoose(const char* left, const char* right) {
    u32 li = 0u;
    u32 ri = 0u;
    char lc;
    char rc;

    if (!left || !right) return 0;
    for (;;) {
        while (left[li] == ' ' || left[li] == '\t' || left[li] == '_' || left[li] == '-') ++li;
        while (right[ri] == ' ' || right[ri] == '\t' || right[ri] == '_' || right[ri] == '-') ++ri;

        lc = runtime_ascii_lower(left[li]);
        rc = runtime_ascii_lower(right[ri]);
        if (lc != rc) return 0;
        if (lc == '\0') return 1;
        ++li;
        ++ri;
    }
}

/*
 * 解析十进制或 0x 开头的十六进制无符号数。
 * 键盘虚拟键码常写成 0x42；MaxEntries 一类普通配置常写成 128，两种都可以直接使用。
 */
int Runtime_ParseU32(const char* text, u32* output) {
    u32 index = 0u;
    u32 base = 10u;
    u32 value = 0u;
    int saw_digit = 0;

    if (!text || !output) return 0;
    while (text[index] == ' ' || text[index] == '\t') ++index;

    if (text[index] == '0' && (text[index + 1u] == 'x' || text[index + 1u] == 'X')) {
        base = 16u;
        index += 2u;
    }

    for (;;) {
        char character = text[index];
        u32 digit;

        if (character >= '0' && character <= '9') digit = (u32)(character - '0');
        else if (character >= 'a' && character <= 'f') digit = 10u + (u32)(character - 'a');
        else if (character >= 'A' && character <= 'F') digit = 10u + (u32)(character - 'A');
        else break;

        if (digit >= base) return 0;
        if (value > (0xFFFFFFFFu - digit) / base) return 0;
        value = value * base + digit;
        saw_digit = 1;
        ++index;
    }

    while (text[index] == ' ' || text[index] == '\t') ++index;
    if (!saw_digit || text[index] != '\0') return 0;
    *output = value;
    return 1;
}

/* 给毫秒配置向上取整；例如 1ms 也会变成一个 8ms tick，而不是错误地变成 0。 */
u32 Runtime_MsToTicks(u32 milliseconds) {
    if (milliseconds == 0u) return 1u;
    return (milliseconds + BACKLOG_WORKER_SLEEP_MS - 1u) / BACKLOG_WORKER_SLEEP_MS;
}

/*
 * 生成与 ASI 同目录的文件路径。
 * 先复制完整 ASI 路径，再从尾部找到最后一个斜杠，把文件名替换掉。
 */
int Runtime_BuildSiblingPath(const char* file_name, char* output, u32 output_size) {
    u32 path_length;
    u32 cut;
    u32 name_index = 0u;

    if (!file_name || !output || output_size == 0u || g_plugin_path[0] == '\0') return 0;
    runtime_copy_text(output, output_size, g_plugin_path);
    path_length = runtime_text_length(output);
    cut = path_length;

    while (cut > 0u && output[cut - 1u] != '\\' && output[cut - 1u] != '/') --cut;
    if (cut == 0u) return 0;

    while (file_name[name_index] != '\0') {
        if (cut + name_index + 1u >= output_size) return 0;
        output[cut + name_index] = file_name[name_index];
        ++name_index;
    }
    output[cut + name_index] = '\0';
    return 1;
}

/*
 * 每条日志使用一次 WriteFile 写正文、一次写 CRLF。
 * 不使用 printf，所以日志文字里没有格式字符串风险，也不会引入 CRT。
 */
void Runtime_Log(const char* text) {
    DWORD written = 0u;
    static const char newline[] = "\r\n";
    u32 length;

    if (!text || g_log_file == INVALID_HANDLE_VALUE) return;
    length = runtime_text_length(text);
    if (length != 0u) WriteFile(g_log_file, text, (DWORD)length, &written, NULL);
    WriteFile(g_log_file, newline, 2u, &written, NULL);
    FlushFileBuffers(g_log_file);
}

/*
 * GetPrivateProfileStringA 在键不存在时会直接写 fallback。
 * 我们仍然再补一次末尾 NUL，防止极端长值正好填满整个缓冲区。
 */
void Runtime_ReadIniText(const char* section, const char* key, const char* fallback,
                         char* output, u32 output_size) {
    char ini_path[MAX_PATH];
    if (!output || output_size == 0u) return;
    output[0] = '\0';
    if (!Runtime_BuildSiblingPath("Castle_Backlog.ini", ini_path, MAX_PATH)) {
        runtime_copy_text(output, output_size, fallback);
        return;
    }
    GetPrivateProfileStringA(section, key, fallback, output, (DWORD)output_size, ini_path);
    output[output_size - 1u] = '\0';
}

/* 读取整数配置，解析失败时写日志并回到默认值，绝不把坏字符串直接带进业务层。 */
static u32 runtime_read_ini_u32(const char* section, const char* key, u32 fallback) {
    char text[64];
    char fallback_text[16];
    u32 value;
    u32 divisor = 1000000000u;
    u32 index = 0u;
    int started = 0;

    /* 手工把 fallback 写成十进制字符串，避免 sprintf。 */
    while (divisor != 0u) {
        u32 digit = (fallback / divisor) % 10u;
        if (digit != 0u || started || divisor == 1u) {
            fallback_text[index++] = (char)('0' + digit);
            started = 1;
        }
        divisor /= 10u;
    }
    fallback_text[index] = '\0';

    Runtime_ReadIniText(section, key, fallback_text, text, (u32)sizeof(text));
    if (!Runtime_ParseU32(text, &value)) {
        Runtime_Log("[配置] 发现无法解析的整数，已回退该项默认值。");
        return fallback;
    }
    return value;
}

/* 把配置限制在 min_value..max_value，防止极端数值造成大循环或超出静态历史容量。 */
static u32 runtime_clamp_u32(u32 value, u32 min_value, u32 max_value) {
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

/* 键盘配置就是 Win32 Virtual-Key 数字；只接受 1..255。 */
static int runtime_read_virtual_key(const char* key, u32 fallback) {
    u32 value = runtime_read_ini_u32("Keyboard", key, fallback);
    if (value < 1u || value > 255u) {
        Runtime_Log("[配置] 键盘 KB 码超出 1..255，已回退默认键位。");
        value = fallback;
    }
    return (int)value;
}

/*
 * SDL 名字由纯文本解析器转换成枚举值。
 * 解析发生在 SDL3.dll 加载之前，因此“没有 SDL”不会妨碍 INI 或键盘功能初始化。
 */
static int runtime_read_gamepad_button(const char* key, const char* fallback_name,
                                       int fallback_button) {
    char text[64];
    int parsed;
    Runtime_ReadIniText("Gamepad", key, fallback_name, text, (u32)sizeof(text));
    parsed = SdlInput_ButtonFromName(text);
    if (parsed < 0) {
        Runtime_Log("[配置] SDL 手柄按钮名无法识别，已回退该项默认键位。");
        return fallback_button;
    }
    return parsed;
}

/* 汇总全部配置并把数值裁剪到实现可以安全支持的范围。 */
static void runtime_load_config(void) {
    u32 delay_ms;
    u32 interval_ms;

    g_config.enabled = runtime_read_ini_u32("Backlog", "Enabled", 1u) != 0u;
    g_config.max_entries = runtime_clamp_u32(
        runtime_read_ini_u32("Backlog", "MaxEntries", 128u), 16u, 256u);
    g_config.page_size = runtime_clamp_u32(
        runtime_read_ini_u32("Backlog", "PageSize", 4u), 2u, 32u);

    delay_ms = runtime_clamp_u32(
        runtime_read_ini_u32("Backlog", "RepeatDelayMs", 350u), 100u, 2000u);
    interval_ms = runtime_clamp_u32(
        runtime_read_ini_u32("Backlog", "RepeatIntervalMs", 100u), 40u, 1000u);
    g_config.repeat_delay_ticks = Runtime_MsToTicks(delay_ms);
    g_config.repeat_interval_ticks = Runtime_MsToTicks(interval_ms);

    g_config.keyboard_open = runtime_read_virtual_key("Open", 0x42u);
    g_config.keyboard_exit = runtime_read_virtual_key("Exit", 0x42u);
    g_config.keyboard_up = runtime_read_virtual_key("Up", 0x26u);
    g_config.keyboard_down = runtime_read_virtual_key("Down", 0x28u);
    g_config.keyboard_left = runtime_read_virtual_key("Left", 0x25u);
    g_config.keyboard_right = runtime_read_virtual_key("Right", 0x27u);

    g_config.gamepad_open = runtime_read_gamepad_button(
        "Open", "left_shoulder", SDL_BUTTON_LEFT_SHOULDER);
    g_config.gamepad_exit = runtime_read_gamepad_button(
        "Exit", "east", SDL_BUTTON_EAST);
    g_config.gamepad_up = runtime_read_gamepad_button(
        "Up", "dpad_up", SDL_BUTTON_DPAD_UP);
    g_config.gamepad_down = runtime_read_gamepad_button(
        "Down", "dpad_down", SDL_BUTTON_DPAD_DOWN);
    g_config.gamepad_left = runtime_read_gamepad_button(
        "Left", "dpad_left", SDL_BUTTON_DPAD_LEFT);
    g_config.gamepad_right = runtime_read_gamepad_button(
        "Right", "dpad_right", SDL_BUTTON_DPAD_RIGHT);
}

const RuntimeConfig* Runtime_Config(void) {
    return &g_config;
}

/*
 * 32 位 RPG.exe 的正常用户对象位于 64 KiB 以上、2 GiB 以下。
 * 这里只做第一层保守过滤；它不能代替对象自己的 active/状态检查。
 */
int Runtime_PointerLooksReadable(const void* pointer) {
    u32 address = (u32)(SIZE_T)pointer;
    return address >= 0x00010000u && address < 0x7FFF0000u;
}

/* 前台窗口 PID 必须等于当前 RPG.exe PID，Alt+Tab 后不会继续响应 B 或方向键。 */
int Runtime_GameIsForeground(void) {
    HWND window = GetForegroundWindow();
    DWORD process_id = 0u;
    if (!window) return 0;
    GetWindowThreadProcessId(window, &process_id);
    return process_id == GetCurrentProcessId();
}

/* 逐字节比较一段机器码。任何一个字节不同都返回失败。 */
static int runtime_bytes_equal(u32 address, const u8* expected, u32 size) {
    u32 index;
    const volatile u8* actual = (const volatile u8*)address;
    for (index = 0u; index < size; ++index) {
        if (actual[index] != expected[index]) return 0;
    }
    return 1;
}

/*
 * 精确能力预检只验证本插件真正依赖的协议：
 * - PE 是固定基址 0x400000 的 i386 RPG.exe；
 * - 场景 vtable 的绘制项仍是 0x40B050；
 * - 说话人切换、消息更新和消息绘制入口仍有已确认签名；
 * - 关键消息缓冲区大小/终止约定依赖的构造代码没有变化。
 *
 * vtable[0] 允许已经被另一个兼容插件换成同调用约定的包装函数；Backlog 安装时会保存
 * 当前值并链式调用。这样不会因为加载顺序而覆盖别人的场景更新 Hook。
 */
static int runtime_exact_game_protocol_ok(void) {
    HMODULE game = GetModuleHandleA(NULL);
    IMAGE_DOS_HEADER* dos;
    IMAGE_NT_HEADERS32* nt;
    static const u8 speaker_signature[] = {
        0x64u,0xA1u,0x00u,0x00u,0x00u,0x00u,0x6Au,0xFFu,0x68u,0xB6u,0xCFu,0x45u,0x00u
    };
    static const u8 message_update_signature[] = {
        0xA0u,0x78u,0xF6u,0x46u,0x00u,0x84u,0xC0u,0x75u,0x09u,0xA0u,0x79u,0xF6u,0x46u,0x00u
    };
    static const u8 message_render_signature[] = {
        0x83u,0xECu,0x08u,0xA0u,0x79u,0xF6u,0x46u,0x00u,0x56u,0x84u,0xC0u
    };
    static const u8 scene_update_signature[] = {
        0x56u,0x8Bu,0xF1u,0xE8u,0x78u,0x01u,0x00u,0x00u,0xE8u,0x43u,0x84u,0xFFu,0xFFu
    };
    static const u8 panel_draw_call[] = {0xE8u,0x9Du,0x2Cu,0x00u,0x00u};
    static const u8 text_draw_call[] = {0xE8u,0xDCu,0xE4u,0xFFu,0xFFu};

    if ((u32)(SIZE_T)game != 0x00400000u) {
        Runtime_Log("[预检失败] RPG.exe 没有加载在已确认的 0x00400000 基址。");
        return 0;
    }

    dos = (IMAGE_DOS_HEADER*)game;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        Runtime_Log("[预检失败] 主模块没有合法 DOS/PE 头。");
        return 0;
    }
    nt = (IMAGE_NT_HEADERS32*)((u8*)game + (u32)dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) {
        Runtime_Log("[预检失败] 主模块不是目标 32 位 i386 PE。");
        return 0;
    }

    if (*(volatile u32*)(VTABLE_SCENE_WORLD + 4u) != FN_SCENE_WORLD_DRAW) {
        Runtime_Log("[预检失败] 场景 vtable[1] 不再指向原版 0x40B050 绘制函数。");
        return 0;
    }
    if (!runtime_bytes_equal(FN_SCENE_WORLD_UPDATE, scene_update_signature,
                             (u32)sizeof(scene_update_signature))) {
        Runtime_Log("[预检失败] 场景更新函数 0x40B150 的入口协议不一致。");
        return 0;
    }
    if (!runtime_bytes_equal(FN_DIALOGUE_SET_SPEAKER, speaker_signature,
                             (u32)sizeof(speaker_signature))) {
        Runtime_Log("[预检失败] 原版说话人切换函数 0x403C60 身份不一致。");
        return 0;
    }
    if (!runtime_bytes_equal(0x00403E30u, message_update_signature,
                             (u32)sizeof(message_update_signature))) {
        Runtime_Log("[预检失败] 原版消息更新入口 0x403E30 身份不一致。");
        return 0;
    }
    if (!runtime_bytes_equal(0x00404800u, message_render_signature,
                             (u32)sizeof(message_render_signature))) {
        Runtime_Log("[预检失败] 原版消息绘制入口 0x404800 身份不一致。");
        return 0;
    }
    if (!runtime_bytes_equal(CALL_DIALOGUE_PANEL_DRAW, panel_draw_call,
                             (u32)sizeof(panel_draw_call))) {
        Runtime_Log("[预检失败] F-Talk 绘制 CALL 0x40486E 已变化，不能安全建立多框列表。");
        return 0;
    }
    if (!runtime_bytes_equal(CALL_DIALOGUE_TEXT_DRAW, text_draw_call,
                             (u32)sizeof(text_draw_call))) {
        Runtime_Log("[预检失败] 正文绘制 CALL 0x4049FF 已变化，不能安全建立多条文字列表。");
        return 0;
    }
    return 1;
}

int Runtime_Initialize(HMODULE plugin_module) {
    char log_path[MAX_PATH];
    DWORD path_length;

    g_plugin_module = plugin_module;
    g_plugin_path[0] = '\0';
    path_length = GetModuleFileNameA(g_plugin_module, g_plugin_path, MAX_PATH);
    if (path_length == 0u || path_length >= MAX_PATH) return 0;
    g_plugin_path[MAX_PATH - 1] = '\0';

    if (Runtime_BuildSiblingPath("Castle_Backlog.log", log_path, MAX_PATH)) {
        g_log_file = CreateFileA(log_path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    Runtime_Log("[启动] Castle Backlog v0.2.0 现代整屏列表正在初始化。");
    Runtime_Log("[启动] By Luminous with ChatGPT");

    runtime_load_config();
    if (!g_config.enabled) {
        Runtime_Log("[配置] Enabled=0；插件保持加载但不会安装或处理输入。");
        return 1;
    }

    if (!runtime_exact_game_protocol_ok()) {
        Runtime_Log("[致命] 当前 RPG.exe 未通过能力预检；未写入任何 Hook。");
        return 0;
    }

    Runtime_Log("[预检] 对话文本、NameList 姓名、现代四框、场景更新和绘制协议全部一致。");
    return 1;
}
