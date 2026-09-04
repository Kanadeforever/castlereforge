#include "runtime.h"
#include "game_addresses.h"
#include "CastlePath_API.h"
#include "CastleLog_API.h"

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
 * 不会误碰历史记录环形队列、原版对话框切换、鼠标窗口过程或手柄协作桥。
 */

static HMODULE g_plugin_module;
static RuntimeConfig g_config;
static char g_plugin_path[MAX_PATH];
static HANDLE g_log_file = INVALID_HANDLE_VALUE;
static const CastleLogApiV1* g_runtime_log_api;
static CastlePluginHandle g_runtime_log_plugin;

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
 * 例如 PAGE_UP、Page-Up、Page Up 都可以被当成同一个 Virtual-Key 名称。
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
 * 新版键盘配置优先使用 B / UP 这类名字；这个数字解析器主要服务 MaxEntries、间距、
 * 重复延迟等普通数值，同时保留旧版 0x42 键位配置的向后兼容。
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

    if (!text) return;
    length = runtime_text_length(text);
    if (g_runtime_log_api && g_runtime_log_plugin && length != 0u) {
        CastleLogRecordV1 record = {0};
        record.magic = CASTLE_LOG_RECORD_MAGIC;
        record.struct_size = CASTLE_SIZEOF_LOG_RECORD_V1;
        record.version = CASTLE_LOG_STRUCTURE_VERSION_1;
        record.level = CASTLE_LOG_INFO;
        record.message.data = text;
        record.message.length = length;
        (void)g_runtime_log_api->WritePluginLine(g_runtime_log_plugin, &record);
        return;
    }
    if (g_log_file == INVALID_HANDLE_VALUE) return;
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

/*
 * 把一个常见的 Win32 Virtual-Key 名称翻译成真正的 VK 数字。
 *
 * 用户在 INI 里写的是 Windows 文档里的名字，但省略最前面的 VK_：
 *
 *   B       -> VK_B
 *   UP      -> VK_UP
 *   RETURN  -> VK_RETURN
 *   F5      -> VK_F5
 *   NUMPAD0 -> VK_NUMPAD0
 *
 * 这样 INI 不再出现“0x42 到底是什么键”的问题。
 *
 * 为了兼容旧配置：
 * - 写 VK_B 也可以；函数会先去掉 VK_；
 * - 写 0x42 或 66 仍然可以，但新默认 INI 不再这么写。
 */
static int runtime_parse_virtual_key_name(const char* input, int* output) {
    const char* text = input;
    u32 numeric;
    u32 length = 0u;

    /* 这张表只放不能用简单规则算出来的常见 VK。 */
    typedef struct VkName {
        const char* name;
        int value;
    } VkName;

    static const VkName names[] = {
        {"LBUTTON", VK_LBUTTON}, {"RBUTTON", VK_RBUTTON}, {"MBUTTON", VK_MBUTTON},
        {"XBUTTON1", VK_XBUTTON1}, {"XBUTTON2", VK_XBUTTON2},
        {"BACK", VK_BACK}, {"BACKSPACE", VK_BACK}, {"TAB", VK_TAB},
        {"CLEAR", VK_CLEAR}, {"RETURN", VK_RETURN}, {"ENTER", VK_RETURN},
        {"SHIFT", VK_SHIFT}, {"CONTROL", VK_CONTROL}, {"CTRL", VK_CONTROL},
        {"MENU", VK_MENU}, {"ALT", VK_MENU}, {"PAUSE", VK_PAUSE},
        {"CAPITAL", VK_CAPITAL}, {"CAPSLOCK", VK_CAPITAL},
        {"ESCAPE", VK_ESCAPE}, {"ESC", VK_ESCAPE}, {"SPACE", VK_SPACE},
        {"PRIOR", VK_PRIOR}, {"PAGEUP", VK_PRIOR}, {"PGUP", VK_PRIOR},
        {"NEXT", VK_NEXT}, {"PAGEDOWN", VK_NEXT}, {"PGDN", VK_NEXT},
        {"END", VK_END}, {"HOME", VK_HOME}, {"LEFT", VK_LEFT},
        {"UP", VK_UP}, {"RIGHT", VK_RIGHT}, {"DOWN", VK_DOWN},
        {"SELECT", VK_SELECT}, {"PRINT", VK_PRINT}, {"EXECUTE", VK_EXECUTE},
        {"SNAPSHOT", VK_SNAPSHOT}, {"PRINTSCREEN", VK_SNAPSHOT},
        {"INSERT", VK_INSERT}, {"DELETE", VK_DELETE}, {"DEL", VK_DELETE},
        {"HELP", VK_HELP},
        {"LWIN", VK_LWIN}, {"RWIN", VK_RWIN}, {"APPS", VK_APPS},
        {"SLEEP", VK_SLEEP},
        {"MULTIPLY", VK_MULTIPLY}, {"ADD", VK_ADD}, {"SEPARATOR", VK_SEPARATOR},
        {"SUBTRACT", VK_SUBTRACT}, {"DECIMAL", VK_DECIMAL}, {"DIVIDE", VK_DIVIDE},
        {"NUMLOCK", VK_NUMLOCK}, {"SCROLL", VK_SCROLL}, {"SCROLLLOCK", VK_SCROLL},
        {"LSHIFT", VK_LSHIFT}, {"RSHIFT", VK_RSHIFT},
        {"LCONTROL", VK_LCONTROL}, {"LCTRL", VK_LCONTROL},
        {"RCONTROL", VK_RCONTROL}, {"RCTRL", VK_RCONTROL},
        {"LMENU", VK_LMENU}, {"LALT", VK_LMENU},
        {"RMENU", VK_RMENU}, {"RALT", VK_RMENU},
        {"BROWSERBACK", VK_BROWSER_BACK}, {"BROWSERFORWARD", VK_BROWSER_FORWARD},
        {"BROWSERREFRESH", VK_BROWSER_REFRESH}, {"BROWSERSTOP", VK_BROWSER_STOP},
        {"BROWSERSEARCH", VK_BROWSER_SEARCH}, {"BROWSERFAVORITES", VK_BROWSER_FAVORITES},
        {"BROWSERHOME", VK_BROWSER_HOME},
        {"VOLUMEMUTE", VK_VOLUME_MUTE}, {"VOLUMEDOWN", VK_VOLUME_DOWN},
        {"VOLUMEUP", VK_VOLUME_UP}, {"MEDIANEXTTRACK", VK_MEDIA_NEXT_TRACK},
        {"MEDIAPREVTRACK", VK_MEDIA_PREV_TRACK}, {"MEDIASTOP", VK_MEDIA_STOP},
        {"MEDIAPLAYPAUSE", VK_MEDIA_PLAY_PAUSE},
        {"LAUNCHMAIL", VK_LAUNCH_MAIL}, {"LAUNCHMEDIASELECT", VK_LAUNCH_MEDIA_SELECT},
        {"LAUNCHAPP1", VK_LAUNCH_APP1}, {"LAUNCHAPP2", VK_LAUNCH_APP2}
    };
    u32 index;

    if (!text || !output) return 0;

    /* 跳过首尾空白。首部先处理；尾部由各解析分支自己验证。 */
    while (*text == ' ' || *text == '\t') ++text;

    /* 允许用户照 Windows 文档习惯误写 VK_B；内部自动把 VK_ 去掉。 */
    if ((text[0] == 'V' || text[0] == 'v') &&
        (text[1] == 'K' || text[1] == 'k') &&
        (text[2] == '_' || text[2] == '-' || text[2] == ' ')) {
        text += 3;
    }

    /* 单个英文字母的 VK 值与大写 ASCII 完全相同。 */
    while (text[length] != '\0' && text[length] != ' ' && text[length] != '\t') ++length;
    if (length == 1u) {
        u32 tail = length;
        char c = text[0];

        /*
         * 允许 B 后面只有空格，但不接受 “B something” 这种半截配置。
         * 这样拼错的 INI 会明确回退默认值，而不是悄悄把第一个字母当成合法键。
         */
        while (text[tail] == ' ' || text[tail] == '\t') ++tail;
        if (text[tail] == '\0') {
            if (c >= 'a' && c <= 'z') c = (char)(c - ('a' - 'A'));
            if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
                *output = (int)(u8)c;
                return 1;
            }
        }
    }

    /* F1..F24 是连续编号，可以直接计算。 */
    if (runtime_ascii_lower(text[0]) == 'f') {
        u32 number = 0u;
        u32 i = 1u;
        int saw = 0;
        while (text[i] >= '0' && text[i] <= '9') {
            number = number * 10u + (u32)(text[i] - '0');
            saw = 1;
            ++i;
        }
        while (text[i] == ' ' || text[i] == '\t') ++i;
        if (saw && text[i] == '\0' && number >= 1u && number <= 24u) {
            *output = VK_F1 + (int)(number - 1u);
            return 1;
        }
    }

    /* NUMPAD0..NUMPAD9 也是连续编号。 */
    if (length == 7u &&
        (runtime_ascii_lower(text[0]) == 'n') &&
        (runtime_ascii_lower(text[1]) == 'u') &&
        (runtime_ascii_lower(text[2]) == 'm') &&
        (runtime_ascii_lower(text[3]) == 'p') &&
        (runtime_ascii_lower(text[4]) == 'a') &&
        (runtime_ascii_lower(text[5]) == 'd') &&
        text[6] >= '0' && text[6] <= '9') {
        u32 i = 7u;
        while (text[i] == ' ' || text[i] == '\t') ++i;
        if (text[i] == '\0') {
            *output = VK_NUMPAD0 + (int)(text[6] - '0');
            return 1;
        }
    }

    /* 其它常见键在表中按 loose 规则比较。 */
    for (index = 0u; index < (u32)(sizeof(names) / sizeof(names[0])); ++index) {
        if (Runtime_TextEqualsLoose(text, names[index].name)) {
            *output = names[index].value;
            return 1;
        }
    }

    /* 最后保留旧版十进制/0x 数字配置兼容。 */
    if (Runtime_ParseU32(text, &numeric) && numeric >= 1u && numeric <= 255u) {
        *output = (int)numeric;
        return 1;
    }
    return 0;
}

/*
 * 从 [Keyboard] 读取一个 Virtual-Key 名字。
 * 解析失败时回退到代码给出的默认 VK，而不是把坏值带到 GetAsyncKeyState。
 */
static int runtime_read_virtual_key(const char* key, const char* fallback_name, int fallback_vk) {
    char text[64];
    int parsed = 0;

    Runtime_ReadIniText("Keyboard", key, fallback_name, text, (u32)sizeof(text));
    if (!runtime_parse_virtual_key_name(text, &parsed)) {
        Runtime_Log("[配置] 键盘 Virtual-Key 名称无法识别，已回退默认键位。");
        return fallback_vk;
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

    /*
     * 布局会按间距自适应同屏条数：
     * - 80..110 使用最多四条；
     * - 111..160 使用最多三条。
     * 三条且 stride=160 时，最上姓名 Y=380-35-2*160=25，仍在屏幕内。
     */
    g_config.panel_stride_y = runtime_clamp_u32(
        runtime_read_ini_u32("Backlog", "PanelStrideY", 160u), 80u, 160u);

    delay_ms = runtime_clamp_u32(
        runtime_read_ini_u32("Backlog", "RepeatDelayMs", 350u), 100u, 2000u);
    interval_ms = runtime_clamp_u32(
        runtime_read_ini_u32("Backlog", "RepeatIntervalMs", 100u), 40u, 1000u);
    g_config.repeat_delay_ticks = Runtime_MsToTicks(delay_ms);
    g_config.repeat_interval_ticks = Runtime_MsToTicks(interval_ms);

    g_config.keyboard_open = runtime_read_virtual_key("Open", "B", (int)'B');
    g_config.keyboard_exit = runtime_read_virtual_key("Exit", "B", (int)'B');
    g_config.keyboard_up = runtime_read_virtual_key("Up", "UP", VK_UP);
    g_config.keyboard_down = runtime_read_virtual_key("Down", "DOWN", VK_DOWN);
    g_config.keyboard_left = runtime_read_virtual_key("Left", "LEFT", VK_LEFT);
    g_config.keyboard_right = runtime_read_virtual_key("Right", "RIGHT", VK_RIGHT);
}

const RuntimeConfig* Runtime_Config(void) {
    return &g_config;
}

/*
 * 32 位 RPG.exe 的正常用户对象位于 64 KiB 以上、2 GiB 以下。
 * 这里只做第一层保守过滤；它不能证明页面仍然 MEM_COMMIT。
 */
int Runtime_PointerLooksReadable(const void* pointer) {
    u32 address = (u32)(SIZE_T)pointer;
    return address >= 0x00010000u && address < 0x7FFF0000u;
}

/*
 * 检查一整段地址是否真的可以读。
 *
 * 为什么不能只检查“地址看起来像 0x12345678”：
 * NPC 对话结束后，原版可能已经 VirtualFree/回收某个资源；全局槽里却仍暂时留下旧数值。
 * 旧 v0.3 只看地址范围，随后直接解引用，正是“记了几条 NPC 对话再打开”崩溃的重要风险。
 *
 * VirtualQuery 会告诉我们每一页当前是否：
 *   1. MEM_COMMIT：页面真的存在；
 *   2. 不是 PAGE_GUARD / PAGE_NOACCESS；
 *   3. 从起点一直覆盖到 size 个字节的末尾。
 *
 * 注意：堆里“已经 free 但页面仍 commit”的小块，VirtualQuery 仍无法识别。
 * v0.3.2 因此不再把这个函数当成“原版当前 F-Name 对象仍存活”的证明；历史姓名框改为
 * Backlog 自己在游戏线程创建并持有，直到关闭时按原版析构协议释放。
 */
int Runtime_MemoryRangeReadable(const void* pointer, u32 size) {
    SIZE_T current;
    SIZE_T end;

    if (!pointer || size == 0u || !Runtime_PointerLooksReadable(pointer)) return 0;
    current = (SIZE_T)pointer;
    end = current + (SIZE_T)size;
    if (end <= current || end > (SIZE_T)0x7FFF0000u) return 0;

    while (current < end) {
        MEMORY_BASIC_INFORMATION info;
        SIZE_T region_end;
        DWORD protection;

        if (VirtualQuery((const void*)current, &info, sizeof(info)) != sizeof(info)) return 0;
        if (info.State != MEM_COMMIT) return 0;

        protection = info.Protect;
        if ((protection & PAGE_GUARD) != 0u || (protection & PAGE_NOACCESS) != 0u) return 0;

        /*
         * PAGE_* 的低 8 位是基本访问方式；执行页只要带 READ 也允许读取。
         * PAGE_EXECUTE（只有执行、没有读）按不可读处理。
         */
        protection &= 0xFFu;
        if (protection != PAGE_READONLY &&
            protection != PAGE_READWRITE &&
            protection != PAGE_WRITECOPY &&
            protection != PAGE_EXECUTE_READ &&
            protection != PAGE_EXECUTE_READWRITE &&
            protection != PAGE_EXECUTE_WRITECOPY) {
            return 0;
        }

        region_end = (SIZE_T)info.BaseAddress + info.RegionSize;
        if (region_end <= current) return 0;
        current = region_end;
    }
    return 1;
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
 * - 说话人字段来源、消息更新、消息绘制和绘制队列登记入口仍有已确认签名；
 * - 关键消息缓冲区大小/终止约定依赖的构造代码没有变化。
 *
 * vtable[0] 允许已经被另一个兼容插件换成同调用约定的包装函数；Backlog 安装时会保存
 * 当前值并链式调用。这样不会因为加载顺序而覆盖别人的场景更新 Hook。
 */
static int runtime_exact_game_protocol_ok(int managed_hook_sites) {
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
    static const u8 speaker_portrait_draw_call[] = {0xE8u,0xB2u,0x2Cu,0x00u,0x00u};
    static const u8 panel_draw_call[] = {0xE8u,0x9Du,0x2Cu,0x00u,0x00u};
    /* 0x404899 -> 0x407510，相对位移 = 0x2C72。 */
    static const u8 name_panel_draw_call[] = {0xE8u,0x72u,0x2Cu,0x00u,0x00u};
    static const u8 name_text_draw_call[] = {0xE8u,0xF5u,0xE5u,0xFFu,0xFFu};
    static const u8 text_draw_call[] = {0xE8u,0xDCu,0xE4u,0xFFu,0xFFu};
    static const u8 draw_queue_register_signature[] = {
        0xA1u,0x38u,0xDAu,0x8Du,0x00u,0x3Du,0xC8u,0x00u,0x00u,0x00u,0x7Cu,0x17u
    };

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
    if (!managed_hook_sites && !runtime_bytes_equal(CALL_DIALOGUE_PANEL_DRAW, panel_draw_call,
                             (u32)sizeof(panel_draw_call))) {
        Runtime_Log("[预检失败] F-Talk 绘制 CALL 0x40486E 已变化，不能安全建立多框列表。");
        return 0;
    }
    if (!managed_hook_sites && !runtime_bytes_equal(CALL_DIALOGUE_NAME_PANEL_DRAW, name_panel_draw_call,
                             (u32)sizeof(name_panel_draw_call))) {
        Runtime_Log("[预检失败] F-Name 绘制 CALL 0x404899 已变化，不能安全建立带姓名历史框。");
        return 0;
    }
    if (!managed_hook_sites && !runtime_bytes_equal(CALL_DIALOGUE_TEXT_DRAW, text_draw_call,
                             (u32)sizeof(text_draw_call))) {
        Runtime_Log("[预检失败] 正文绘制 CALL 0x4049FF 已变化，不能安全建立多条文字列表。");
        return 0;
    }
    if (!managed_hook_sites && !runtime_bytes_equal(CALL_DIALOGUE_SPEAKER_PORTRAIT_DRAW,
                             speaker_portrait_draw_call,
                             (u32)sizeof(speaker_portrait_draw_call))) {
        Runtime_Log("[预检失败] 人物图绘制 CALL 0x404859 已变化，不能安全只屏蔽人物图。");
        return 0;
    }
    if (!runtime_bytes_equal(FN_DRAW_QUEUE_REGISTER, draw_queue_register_signature,
                             (u32)sizeof(draw_queue_register_signature))) {
        Runtime_Log("[预检失败] 绘制队列登记函数 0x434500 身份不一致；不能安全冻结逻辑并保留显示。");
        return 0;
    }
    if (!managed_hook_sites && !runtime_bytes_equal(CALL_DIALOGUE_NAME_TEXT_DRAW, name_text_draw_call,
                             (u32)sizeof(name_text_draw_call))) {
        Runtime_Log("[预检失败] 姓名文字绘制 CALL 0x4048E6 已变化，不能安全复用原版姓名布局。");
        return 0;
    }
    return 1;
}

static int runtime_finish_initialize(int managed_hook_sites) {
    char log_path[MAX_PATH];

    if (!g_runtime_log_api && Runtime_BuildSiblingPath("Castle_Backlog.log", log_path, MAX_PATH)) {
        g_log_file = CreateFileA(log_path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    Runtime_Log("[启动] Castle Backlog v0.3.4 正在初始化。");
    Runtime_Log("[启动] By Luminous with ChatGPT");

    runtime_load_config();
    if (!g_config.enabled) {
        Runtime_Log("[配置] Enabled=0；插件保持加载但不会安装或处理输入。");
        return 1;
    }

    if (!runtime_exact_game_protocol_ok(managed_hook_sites)) {
        Runtime_Log("[致命] 当前 RPG.exe 未通过能力预检；未写入任何 Hook。");
        return 0;
    }

    Runtime_Log("[预检] 对话文本、原版 F-Name/F-Talk 组合框、绘制队列登记、场景更新和输入协议全部一致。");
    return 1;
}

int Runtime_Initialize(HMODULE plugin_module) {
    DWORD path_length;
    g_plugin_module = plugin_module;
    g_plugin_path[0] = '\0';
    path_length = GetModuleFileNameA(g_plugin_module, g_plugin_path, MAX_PATH);
    if (path_length == 0u || path_length >= MAX_PATH) return 0;
    g_plugin_path[MAX_PATH - 1] = '\0';
    return runtime_finish_initialize(0);
}

int Runtime_InitializeIntegrated(HMODULE plugin_module,
                                 const CastleRuntimeApiV1* runtime_api,
                                 CastlePluginHandle plugin_handle) {
    static const char interface_id[] = CASTLE_PATH_INTERFACE_ID;
    static const char log_interface_id[] = CASTLE_LOG_INTERFACE_ID;
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 result = {0};
    const CastlePathApiV1* path_api;
    CastleU32 path_length = 0u;
    g_plugin_module = plugin_module;
    g_plugin_path[0] = '\0';
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id.data = interface_id;
    query.interface_id.length = (CastleU32)(sizeof(interface_id) - 1u);
    query.requested_version = CASTLE_PATH_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_PATH_API_V1;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (!runtime_api || runtime_api->QueryInterface(&query, &result) != CASTLE_OK) {
        return 0;
    }
    path_api = (const CastlePathApiV1*)result.api_pointer;
    if (!path_api || path_api->GetPluginModulePathUtf8(plugin_handle,
            g_plugin_path, MAX_PATH, &path_length) != CASTLE_OK ||
        path_length == 0u) return 0;
    g_plugin_path[MAX_PATH - 1] = '\0';
    query.interface_id.data = log_interface_id;
    query.interface_id.length = (CastleU32)(sizeof(log_interface_id) - 1u);
    query.requested_version = CASTLE_LOG_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_LOG_API_V1;
    result.api_pointer = NULL;
    if (runtime_api->QueryInterface(&query, &result) != CASTLE_OK ||
        !result.api_pointer) return 0;
    g_runtime_log_api = (const CastleLogApiV1*)result.api_pointer;
    g_runtime_log_plugin = plugin_handle;
    return runtime_finish_initialize(1);
}
