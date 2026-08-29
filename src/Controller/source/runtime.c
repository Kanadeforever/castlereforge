#include "runtime.h"
#include "game_addresses.h"

/*
 * runtime.c
 *
 * 这一文件故意写得“笨但直白”：没有 CRT、没有 printf、没有 malloc。
 * 老游戏注入 DLL 最怕启动阶段多出隐式依赖，所以所有字符串拼接、十六进制格式化、
 * 文件写入都只用最基础的 Win32 API。
 */

static RuntimeApi g_api;
static RuntimeConfig g_cfg;

/*
 * 原版 Space 当前真正调用到的地图动作业务函数。
 *
 * 基线 EXE 中它是 0x40B230；refactor20a 开始不再把这个固定地址的整段机器码当作兼容硬门槛。
 * Runtime_InterfaceShellProtocolOk() 会从原版 Space 的 CALL 指令现场解析目标并写到这里。
 * InterfaceShell 后续只读这个值，因此其它补丁若把 Space CALL 改到兼容 wrapper，Y 会自然跟随。
 */
static u32 g_map_space_event_target = FN_MAP_SPACE_EVENT;
static HMODULE g_self_module;
static HANDLE g_log = INVALID_HANDLE_VALUE_;
static u32 g_tick;

/*
 * 运行日志的“相同词条抑制”缓存。
 *
 * 用户实机日志已经证明：像“RB 请求”“UI 事件已消费”这样的短句在快速操作时会成百次重复，
 * 真正有价值的信息反而被淹没。这里不改变任何业务状态，只记住最近出现过的文本指纹。
 * 同一条完全相同的 UTF-8 日志在短时间内再次出现时直接不写文件；不同文本仍照常写。
 *
 * 为什么用多槽而不是只比较上一行：
 *   [主界面] RB...
 *   [UI事件] 已消费...
 *   [主界面] RB...
 * 这种两条交替出现的情况，单纯“上一行去重”完全拦不住。多槽缓存可以把交替重复也压住。
 */
#define RUNTIME_LOG_DEDUPE_SLOTS 64
#define RUNTIME_LOG_DEDUPE_MS    2000u

typedef struct RuntimeLogDedupeSlot {
    u32 hash;
    u32 length;
    u32 last_tick;
    int used;
} RuntimeLogDedupeSlot;

static RuntimeLogDedupeSlot g_log_dedupe[RUNTIME_LOG_DEDUPE_SLOTS];

/* ------------------------- 最小字符串工具 ------------------------- */

/* 无 CRT 版本 strlen；只给内部日志使用，NULL 按空串处理。 */
static SIZE_T rt_strlen(const char* s) {
    SIZE_T n = 0;
    if (!s) return 0;
    while (s[n] != '\0') ++n;
    return n;
}

/* 逐字节比较机器码签名；不依赖 memcmp，失败只返回 0。 */
int Runtime_MemEq(const u8* a, const u8* b, SIZE_T n) {
    SIZE_T i;
    if (!a || !b) return 0;
    for (i = 0; i < n; ++i) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

int Runtime_PtrOk(const void* p) {
    u32 v = (u32)p;
    /*
     * RPG.exe 是 32 位进程。小于 0x10000 的地址通常是 NULL 附近的坏指针；
     * 0x7FFF0000 以上则不属于我们要操作的普通用户态对象范围。
     */
    return v >= 0x00010000u && v < 0x7FFF0000u;
}

/* 把 0..15 转成一个大写十六进制字符，供地址日志拼接。 */
static char rt_hex_digit(u32 v) {
    return (char)(v < 10u ? ('0' + v) : ('A' + (v - 10u)));
}

/* 向固定缓冲区安全追加文本，并始终保留结尾 NUL；日志绝不能越界影响游戏。 */
static void rt_append_text(char* dst, SIZE_T cap, SIZE_T* pos, const char* src) {
    SIZE_T i = 0;
    if (!dst || !pos || !src || cap == 0) return;
    while (src[i] && (*pos + 1u) < cap) dst[(*pos)++] = src[i++];
    dst[*pos] = '\0';
}

/* 以固定 0xXXXXXXXX 形式追加 32 位地址，方便用户把日志地址直接放进 x64dbg/Ghidra。 */
static void rt_append_hex32(char* dst, SIZE_T cap, SIZE_T* pos, u32 value) {
    int shift;
    rt_append_text(dst, cap, pos, "0x");
    for (shift = 28; shift >= 0; shift -= 4) {
        char one[2];
        one[0] = rt_hex_digit((value >> (u32)shift) & 0xFu);
        one[1] = '\0';
        rt_append_text(dst, cap, pos, one);
    }
}

/* ------------------------- API 解析 ------------------------- */

void Runtime_BindEarlyApi(void) {
    /*
     * 这些 IAT 槽在目标 RPG.exe 中是固定的，因此 DllMain 尚处于 loader lock 时
     * 也可以直接读取函数指针。这里只“拿地址”，不做文件 I/O 和 LoadLibrary。
     */
    g_api.get_module_handle_a = *(PFN_GetModuleHandleA*)IAT_GETMODULEHANDLEA;
    g_api.get_proc_address = *(PFN_GetProcAddress*)IAT_GETPROCADDRESS;
    g_api.load_library_a = *(PFN_LoadLibraryA*)IAT_LOADLIBRARYA;
    g_api.create_file_a = *(PFN_CreateFileA*)IAT_CREATEFILEA;
    g_api.write_file = *(PFN_WriteFile*)IAT_WRITEFILE;
    g_api.close_handle = *(PFN_CloseHandle*)IAT_CLOSEHANDLE;
    g_api.get_cursor_pos = *(PFN_GetCursorPos*)IAT_GETCURSORPOS;

    if (g_api.get_module_handle_a && g_api.get_proc_address) {
        HMODULE k32 = g_api.get_module_handle_a("KERNEL32.dll");
        if (k32) {
            g_api.virtual_protect = (PFN_VirtualProtect)g_api.get_proc_address(k32, "VirtualProtect");
            g_api.get_module_file_name_a = (PFN_GetModuleFileNameA)g_api.get_proc_address(k32, "GetModuleFileNameA");
            g_api.get_module_handle_ex_a = (PFN_GetModuleHandleExA)g_api.get_proc_address(k32, "GetModuleHandleExA");
            g_api.create_thread = (PFN_CreateThread)g_api.get_proc_address(k32, "CreateThread");
            g_api.sleep = (PFN_Sleep)g_api.get_proc_address(k32, "Sleep");
            g_api.get_current_process_id = (PFN_GetCurrentProcessId)g_api.get_proc_address(k32, "GetCurrentProcessId");
            g_api.get_private_profile_int_a = (PFN_GetPrivateProfileIntA)g_api.get_proc_address(k32, "GetPrivateProfileIntA");
        }

        {
            HMODULE u32m = g_api.get_module_handle_a("USER32.dll");
            if (u32m) {
                g_api.get_foreground_window = (PFN_GetForegroundWindow)g_api.get_proc_address(u32m, "GetForegroundWindow");
                g_api.get_client_rect = (PFN_GetClientRect)g_api.get_proc_address(u32m, "GetClientRect");
                g_api.client_to_screen = (PFN_ClientToScreen)g_api.get_proc_address(u32m, "ClientToScreen");
                g_api.set_cursor_pos = (PFN_SetCursorPos)g_api.get_proc_address(u32m, "SetCursorPos");
                g_api.get_window_thread_process_id = (PFN_GetWindowThreadProcessId)g_api.get_proc_address(u32m, "GetWindowThreadProcessId");
                g_api.mouse_event = (PFN_mouse_event)g_api.get_proc_address(u32m, "mouse_event");
                g_api.post_message_a = (PFN_PostMessageA)g_api.get_proc_address(u32m, "PostMessageA");
            }
        }
    }
}

const RuntimeApi* Runtime_Api(void) { return &g_api; }
const RuntimeConfig* Runtime_Config(void) { return &g_cfg; }
HMODULE Runtime_SelfModule(void) { return g_self_module; }
u32 Runtime_Tick(void) { return g_tick; }
void Runtime_AdvanceTick(void) { ++g_tick; }

/* 所有毫秒配置统一按 8ms worker 周期向上取整，至少返回 1 tick。 */
u32 Runtime_MsToTicks(u32 ms) {
    u32 ticks = (ms + WORKER_SLEEP_MS - 1u) / WORKER_SLEEP_MS;
    return ticks ? ticks : 1u;
}

/* ------------------------- 同目录路径 / 配置 ------------------------- */

int Runtime_BuildSiblingPath(const char* leaf, char* out, SIZE_T cap) {
    DWORD n;
    SIZE_T i;
    if (!leaf || !out || cap < 16u || !g_api.get_module_file_name_a || !g_self_module) return 0;
    n = g_api.get_module_file_name_a(g_self_module, out, (DWORD)cap);
    if (!n || n >= cap) return 0;

    /* 从 ASI 自己的完整路径末尾向前找最后一个反斜杠。 */
    i = (SIZE_T)n;
    while (i > 0 && out[i - 1u] != '\\' && out[i - 1u] != '/') --i;
    if (i == 0) return 0;

    while (*leaf && i + 1u < cap) out[i++] = *leaf++;
    out[i] = '\0';
    return *leaf == '\0';
}

/* 读取一个 INI 整数并强制夹在已验证安全范围，防止错误配置产生极端计时/阈值。 */
static int rt_cfg_int(const char* section, const char* key, int defv, int minv, int maxv) {
    char path[MAX_PATH_];
    int v;
    if (!g_api.get_private_profile_int_a || !Runtime_BuildSiblingPath("Castle_PadSupport.ini", path, MAX_PATH_)) return defv;
    v = (int)g_api.get_private_profile_int_a(section, key, defv, path);
    if (v < minv) v = minv;
    if (v > maxv) v = maxv;
    return v;
}

static void rt_load_config(void) {
    /*
     * 绝大多数既有默认值继续沿用稳定基线。R41 只增加调查激活方式选择，
     * R40左杆/LB/RB手感、指针速度和震动边界继续保持：
     * 已移除业务的延时隐藏、修饰键精细档与摇杆按键长按配置不再读取。
     * 所有配置仍集中由 Runtime 读取，Cursor / Battle 等业务模块只拿已经裁剪过安全范围的结果。
     */
    /*
     * SwapConfirmCancel 只允许0/1。默认0保持项目从最早版本开始的Xbox位置语义；
     * 写1时由InputRouter统一交换确定/取消，业务页面仍只看到语义，不需要逐页改代码。
     * 鼠标模式也读取同一语义，所以“确定=左键、取消=右键”在两种布局下都成立。
     */
    g_cfg.swap_confirm_cancel =
        rt_cfg_int("Controls", "SwapConfirmCancel", 0, 0, 1);

    g_cfg.cursor_default_hidden = rt_cfg_int("Mouse", "DefaultHidden", 1, 0, 1);
    g_cfg.target_cursor_indicator = rt_cfg_int("Mouse", "TargetSelectionCursor", 1, 0, 1);
    g_cfg.mouse_mode_left_stick_sensitivity_percent =
        (u32)rt_cfg_int("Mouse", "MouseModeLeftStickSensitivityPercent", 100, 1, 300);
    g_cfg.mouse_mode_right_stick_sensitivity_percent =
        (u32)rt_cfg_int("Mouse", "MouseModeRightStickSensitivityPercent", 15, 1, 300);

    /*
     * ActivationMode 的安全范围只有 0 和 1：
     * - 缺少 INI、缺少键、写成负数或其它无效内容时，rt_cfg_int 会回到默认 0；
     * - 写成大于 1 的整数会被夹到 1，绝不会把未知数字交给模式状态机猜测。
     *
     * 这样旧用户没有配置文件时会自然得到“按住 A、松开互动”的新默认方式；
     * 想继续使用 R40 的“按住 LT、A确认”时，只要显式写 ActivationMode=1。
     */
    g_cfg.investigation_activation_mode =
        rt_cfg_int("Investigation", "ActivationMode", 0, 0, 1);

    /*
     * AutoFocusNearest 默认1，表示每次真正建立调查会话时自动选择最近候选。
     * rt_cfg_int 仍把用户输入夹在0..1：缺键或无效负数回到默认1，大于1夹到1。
     * 这里只保存开关，不读取角色、目标或手柄；真正选择仍在 Investigation 的安全快照层。
     */
    g_cfg.investigation_auto_focus_nearest =
        rt_cfg_int("Investigation", "AutoFocusNearest", 1, 0, 1);
    g_cfg.investigation_right_stick_sensitivity_percent =
        (u32)rt_cfg_int("Investigation", "RightStickSensitivityPercent", 8, 1, 100);
    g_cfg.investigation_snap_radius_pixels =
        (u32)rt_cfg_int("Investigation", "SnapRadiusPixels", 12, 1, 64);
    /*
     * 所有震动事件共享一个强度百分比；持续时间按事件拆开，便于逐项调手感。
     * InvestigationHoverDurationMs 是历史键名：现在它表示“显式 A/LT 调查或 Back/RT 鼠标模式中，
     * 指针首次碰到新的可互动对象”的共享短震时长，不允许普通隐藏鼠标使用。
     */
    g_cfg.rumble_strength_percent =
        (u32)rt_cfg_int("Rumble", "StrengthPercent", 100, 0, 100);
    g_cfg.investigation_rumble_ms =
        (u32)rt_cfg_int("Rumble", "InvestigationHoverDurationMs", 80, 0, 5000);
    g_cfg.controller_mode_rumble_ms =
        (u32)rt_cfg_int("Rumble", "ControllerModeDurationMs", 1000, 0, 5000);

    g_cfg.nav_visual_min_ms = (u32)rt_cfg_int("BattleNavigation", "VisualMinMs", 72, 16, 500);
    g_cfg.nav_hit_count = (u32)rt_cfg_int("BattleNavigation", "VisualHitCount", 2, 1, 8);
    g_cfg.nav_settle_timeout_ms = (u32)rt_cfg_int("BattleNavigation", "SettleTimeoutMs", 240, 64, 1000);
    g_cfg.nav_repeat_initial_ms = (u32)rt_cfg_int("BattleNavigation", "RepeatInitialMs", 176, 64, 1000);
    g_cfg.nav_repeat_interval_ms = (u32)rt_cfg_int("BattleNavigation", "RepeatIntervalMs", 96, 32, 500);
    g_cfg.battle_remember_selection = rt_cfg_int("BattleNavigation", "RememberSelection", 0, 0, 1);
    g_cfg.battle_shortcuts = rt_cfg_int("BattleNavigation", "EnableShortcuts", 1, 0, 1);

    g_cfg.run_threshold_percent = (u32)rt_cfg_int("Movement", "RunThresholdPercent", 70, 20, 95);
}

/* ------------------------- 日志 ------------------------- */

/* 日志固定创建在 ASI 同目录，并允许用户在游戏运行时以只读方式打开查看。 */
static void rt_open_log(void) {
    char path[MAX_PATH_];
    if (!g_api.create_file_a || g_log != INVALID_HANDLE_VALUE_) return;
    if (!Runtime_BuildSiblingPath("Castle_PadSupport.log", path, MAX_PATH_)) return;
    g_log = g_api.create_file_a(path, GENERIC_WRITE_, FILE_SHARE_READ_, NULL, CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL);
}

/*
 * 对 UTF-8 文本做一个很小的 FNV-1a 32 位指纹。
 * 这里只用于日志去重，不用于安全判断；即使理论上发生哈希碰撞，最多也只是少写一条诊断文本。
 */
static u32 rt_log_hash(const char* text, SIZE_T length) {
    SIZE_T i;
    u32 hash = 2166136261u;

    for (i = 0; i < length; ++i) {
        hash ^= (u8)text[i];
        hash *= 16777619u;
    }
    return hash;
}

/*
 * 返回 1 表示“这条完全相同的词条最近刚写过，应当抑制”；返回 0 表示允许写。
 *
 * 缓存采用 64 个固定槽，不 malloc，也不会无限增长。
 * 如果 64 个槽都用满，就覆盖最久没有出现过的槽。这个机制只影响日志文件大小，绝不参与输入逻辑。
 */
static int rt_log_should_suppress(const char* text, SIZE_T length) {
    u32 hash;
    u32 now;
    u32 window;
    int i;
    int free_slot = -1;
    int oldest_slot = 0;
    u32 oldest_age = 0u;

    if (!text || length == 0u) return 0;

    hash = rt_log_hash(text, length);
    now = Runtime_Tick();
    window = Runtime_MsToTicks(RUNTIME_LOG_DEDUPE_MS);

    for (i = 0; i < RUNTIME_LOG_DEDUPE_SLOTS; ++i) {
        RuntimeLogDedupeSlot* slot = &g_log_dedupe[i];

        if (!slot->used) {
            if (free_slot < 0) free_slot = i;
            continue;
        }

        if (slot->hash == hash && slot->length == (u32)length) {
            u32 elapsed = now - slot->last_tick;

            /*
             * 每次重复都刷新 last_tick。这样玩家持续按住/快速重复同一动作时，
             * 整段连续操作期间都只保留第一次；停止超过 2 秒后再次操作，日志才重新出现一次。
             */
            slot->last_tick = now;
            if (elapsed <= window) return 1;
            return 0;
        }

        {
            u32 age = now - slot->last_tick;
            if (age >= oldest_age) {
                oldest_age = age;
                oldest_slot = i;
            }
        }
    }

    i = free_slot >= 0 ? free_slot : oldest_slot;
    g_log_dedupe[i].hash = hash;
    g_log_dedupe[i].length = (u32)length;
    g_log_dedupe[i].last_tick = now;
    g_log_dedupe[i].used = 1;
    return 0;
}

/*
 * 每条日志原样写 UTF-8，再补 CRLF；不做英文翻译层，调用者必须直接提供简体中文说明。
 * 写文件前先经过全局“完全相同词条”抑制，所以交替出现的重复日志也不会继续堆满文件。
 */
void Runtime_Log(const char* utf8_line) {
    DWORD written;
    SIZE_T n;
    static const char crlf[2] = {'\r','\n'};

    if (!utf8_line || !g_api.write_file || g_log == INVALID_HANDLE_VALUE_) return;

    n = rt_strlen(utf8_line);
    if (n && rt_log_should_suppress(utf8_line, n)) return;

    if (n) g_api.write_file(g_log, utf8_line, (DWORD)n, &written, NULL);
    g_api.write_file(g_log, crlf, 2u, &written, NULL);
}

void Runtime_LogHexPair(const char* prefix, u32 a, const char* middle, u32 b) {
    char line[224];
    SIZE_T p = 0;
    rt_append_text(line, sizeof(line), &p, prefix ? prefix : "");
    rt_append_hex32(line, sizeof(line), &p, a);
    rt_append_text(line, sizeof(line), &p, middle ? middle : "");
    rt_append_hex32(line, sizeof(line), &p, b);
    Runtime_Log(line);
}

void Runtime_LogModule(const char* label, HMODULE module, const char* fallback_name) {
    char line[512];
    char path[MAX_PATH_];
    SIZE_T p = 0;
    DWORD n = 0;
    rt_append_text(line, sizeof(line), &p, "[模块] ");
    rt_append_text(line, sizeof(line), &p, label ? label : "未命名");
    rt_append_text(line, sizeof(line), &p, " 句柄=");
    rt_append_hex32(line, sizeof(line), &p, (u32)module);
    if (module && g_api.get_module_file_name_a) n = g_api.get_module_file_name_a(module, path, MAX_PATH_);
    if (n && n < MAX_PATH_) {
        rt_append_text(line, sizeof(line), &p, " 路径=");
        rt_append_text(line, sizeof(line), &p, path);
    } else if (fallback_name) {
        rt_append_text(line, sizeof(line), &p, " 名称=");
        rt_append_text(line, sizeof(line), &p, fallback_name);
    }
    Runtime_Log(line);
}

void Runtime_PinModuleFromAddress(const char* label, const void* address) {
    HMODULE pinned = NULL;
    if (!g_api.get_module_handle_ex_a || !address) return;
    if (g_api.get_module_handle_ex_a(GET_MODULE_HANDLE_EX_FLAG_PIN_ | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS_, (const char*)address, &pinned)) {
        Runtime_LogModule(label ? label : "PIN 模块", pinned, NULL);
    }
}

/* ------------------------- 精确版本验证 ------------------------- */

/* 解码 x86 E8 rel32 的真实绝对目标；首字节不是 E8 时返回 0 表示预检失败。 */
static u32 rt_call_target(u32 call_address) {
    const u8* p = (const u8*)call_address;
    i32 rel;

    /* x86 的 CALL rel32 一共 5 字节：E8 + 一个有符号 32 位相对位移。 */
    if (p[0] != 0xE8u) return 0u;
    rel = *(const i32*)(p + 1);
    return call_address + 5u + (u32)rel;
}

typedef struct RtCallCheck {
    u32 call_address;
    u32 expected_target;
    const char* chinese_name;
} RtCallCheck;

typedef struct RtByteCheck {
    u32 address;
    const u8* expected;
    SIZE_T size;
    const char* chinese_name;
} RtByteCheck;

static int rt_check_call_item(const RtCallCheck* item) {
    u32 actual;
    if (!item) return 0;
    actual = rt_call_target(item->call_address);
    if (actual == item->expected_target) return 1;

    /*
     * 失败时把“哪一个协议、地址、实际目标、期望目标”全部写进中文日志。
     * 这样实机若遇到其它补丁提前改写 RPG.exe，不需要用户自己看英文机器码猜原因。
     */
    Runtime_Log(item->chinese_name);
    Runtime_LogHexPair("[预检失败] CALL 地址=", item->call_address, "，实际目标=", actual);
    Runtime_LogHexPair("[预检失败] 期望目标=", item->expected_target, "，CALL 首地址=", item->call_address);
    return 0;
}

static int rt_check_byte_item(const RtByteCheck* item) {
    if (!item) return 0;
    if (Runtime_MemEq((const u8*)item->address, item->expected, item->size)) return 1;
    Runtime_Log(item->chinese_name);
    Runtime_LogHexPair("[预检失败] 字节签名地址=", item->address, "，长度=", (u32)item->size);
    return 0;
}

/*
 * 按 mask 比较“语义特征”，而不是要求整段机器码逐字节完全相同。
 * mask[i] == 0xFF：这一字节必须与 expected 完全一致；
 * mask[i] == 0x00：这一字节属于跳转位移等非语义细节，允许不同。
 *
 * 这适合检查 x86 条件跳转：0F 85 / 0F 84 说明“是什么判断”，后面的 rel32 只说明
 * “跳多远”，只要代码布局或别的兼容补丁轻微调整，rel32 就可能变化，不应该因此判死刑。
 */
static int rt_mem_eq_masked(const u8* actual, const u8* expected, const u8* mask, SIZE_T n) {
    SIZE_T i;
    if (!actual || !expected || !mask) return 0;
    for (i = 0; i < n; ++i) {
        if (mask[i] && actual[i] != expected[i]) return 0;
    }
    return 1;
}

int Runtime_ExactBuildOk(void) {
    SIZE_T i;

    /*
     * 这里完整复用 dev20 静态校验器的 41 个 CALL 和 8 个字节签名，
     * 再追加 refactor3 的顶层动画 CALL 与 refactor4 的四个 CMD1/CMD2 阶段切换 CALL。
     * 与旧实现不同的是：现在这些检查发生在“任何 Hook 写入之前”。
     * 因此只要目标 EXE 或某个关键调用点不一致，整个插件会零修改退出，绝不会留下半安装状态。
     */
    static const u8 sig_mouse_draw[] = {0x8A,0x81,0x48,0x02,0x00,0x00,0x84,0xC0};
    static const u8 sig_mouse_wrapper[] = {0x8B,0x44,0x24,0x08,0x8B,0x4C,0x24,0x04,0x50,0x51,0xFF,0x15,0x9C,0x01,0x46,0x00};
    static const u8 sig_target_mode[] = {0x8B,0x8E,0x28,0x10,0x00,0x00,0x83,0xC8,0xFF,0x2B,0xCF,0x89,0x44,0x24,0x14};
    static const u8 sig_target_mode2[] = {0x33,0xC0,0xC7,0x44,0x24,0x14,0x1C,0x00,0x00,0x00};
    static const u8 sig_target_mode1[] = {0xB8,0x14,0x00,0x00,0x00,0xC7,0x44,0x24,0x14,0x1C,0x00,0x00,0x00};
    static const u8 sig_target_mode0[] = {0x33,0xC0,0xC7,0x44,0x24,0x14,0x08,0x00,0x00,0x00};
    static const u8 sig_save_anim[] = {0x3B,0xC5,0x74,0x10,0xC7,0x86,0xBC,0x05,0x00,0x00,0x04,0x00,0x00,0x00,0x89,0xAE,0xC0,0x05,0x00,0x00};
    static const u8 sig_save_refresh[] = {0x3B,0x91,0x94,0x05,0x00,0x00,0x75,0x1A};

    static const RtByteCheck byte_checks[] = {
        {0x0043E1B0u, sig_mouse_draw, sizeof(sig_mouse_draw), "[预检失败] 原版游戏内光标绘制入口不一致。"},
        {0x0043DF30u, sig_mouse_wrapper, sizeof(sig_mouse_wrapper), "[预检失败] 原版 MouseManager SetCursorPos 包装器不一致。"},
        {0x004461F4u, sig_target_mode, sizeof(sig_target_mode), "[预检失败] Target 模式分派入口不一致。"},
        {0x0044620Bu, sig_target_mode2, sizeof(sig_target_mode2), "[预检失败] Target mode2 扫描范围协议不一致。"},
        {0x00446217u, sig_target_mode1, sizeof(sig_target_mode1), "[预检失败] Target mode1 扫描范围协议不一致。"},
        {0x00446226u, sig_target_mode0, sizeof(sig_target_mode0), "[预检失败] Target mode0 扫描范围协议不一致。"},
        {0x00424CB5u, sig_save_anim, sizeof(sig_save_anim), "[预检失败] SaveSlot 换槽四步选中动画协议不一致。"},
        {0x00425478u, sig_save_refresh, sizeof(sig_save_refresh), "[预检失败] SaveSlot refresh selected-row 比较协议不一致。"}
    };

    static const RtCallCheck call_checks[] = {
        {CALL_TOP_VISUAL_HIT, FN_BUTTON_HITTEST, "[预检失败] 战斗顶层六命令视觉 HitTest CALL 不一致。"},
        {CALL_TOP_PANEL_ANIM, FN_TOP_PANEL_ANIM, "[预检失败] 战斗顶层展开/收起动画 CALL 不一致。"},
        {CALL_CMD1_PANEL_ANIM, FN_CMD1_PANEL_ANIM, "[预检失败] 战斗 CMD1 列表动画 CALL 不一致。"},
        {CALL_CMD2_PANEL_ANIM, FN_CMD2_PANEL_ANIM, "[预检失败] 战斗 CMD2 列表动画 CALL 不一致。"},
        {CALL_CMD1_PANEL_CURSOR_WARP, FN_MOUSE_SET_CURSOR_WRAPPER, "[预检失败] 战斗 CMD1 展开后默认鼠标定位 CALL 不一致。"},
        {CALL_CMD2_PANEL_CURSOR_WARP, FN_MOUSE_SET_CURSOR_WRAPPER, "[预检失败] 战斗 CMD2 展开后默认鼠标定位 CALL 不一致。"},
        {CALL_CMD1_VISUAL_HIT_A, FN_BUTTON_HITTEST, "[预检失败] 战斗 CMD1 行视觉 HitTest A CALL 不一致。"},
        {CALL_CMD1_VISUAL_HIT_B, FN_BUTTON_HITTEST, "[预检失败] 战斗 CMD1 行视觉 HitTest B CALL 不一致。"},
        {CALL_CMD2_VISUAL_HIT, FN_BUTTON_HITTEST, "[预检失败] 战斗 CMD2 行视觉 HitTest CALL 不一致。"},
        {CALL_CONFIRM_HIT_YES, FN_BUTTON_HITTEST, "[预检失败] 战斗确认框确定按钮 HitTest CALL 不一致。"},
        {CALL_CONFIRM_HIT_NO, FN_BUTTON_HITTEST, "[预检失败] 战斗确认框取消按钮 HitTest CALL 不一致。"},

        {CALL_CMD0_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD0 主按钮 Event CALL 不一致。"},
        {CALL_CMD1_ROW_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD1 行 Event CALL 不一致。"},
        {CALL_CMD1_PAGE_PREV, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD1 上一页 Event CALL 不一致。"},
        {CALL_CMD1_PAGE_NEXT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD1 下一页 Event CALL 不一致。"},
        {CALL_CMD1_CATEGORY_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD1 大类 Event CALL 不一致。"},
        {CALL_CMD1_CANCEL_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD1 取消 Event CALL 不一致。"},
        {CALL_CMD2_ROW_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD2 行 Event CALL 不一致。"},
        {CALL_CMD2_CATEGORY_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD2 大类 Event CALL 不一致。"},
        {CALL_CMD2_CANCEL_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD2 取消 Event CALL 不一致。"},
        {CALL_CMD2_PAGE_PREV, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD2 上一页 Event CALL 不一致。"},
        {CALL_CMD2_PAGE_NEXT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD2 下一页 Event CALL 不一致。"},
        {CALL_TOP_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗顶层命令 Event CALL 不一致。"},
        {CALL_CMD3_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗 CMD3 Event CALL 不一致。"},
        {CALL_CONFIRM_EVENT_YES, FN_BUTTON_EVENT, "[预检失败] 战斗确认框确定 Event CALL 不一致。"},
        {CALL_CONFIRM_EVENT_NO, FN_BUTTON_EVENT, "[预检失败] 战斗确认框取消 Event CALL 不一致。"},

        {CALL_TARGET_GRID_INDEX, FN_TARGET_GRID_INDEX, "[预检失败] Target 网格索引 CALL 不一致。"},
        {CALL_EXPLORATION_MOVE, FN_EXPLORATION_MOVE, "[预检失败] 探索八方向移动 resolver CALL 不一致。"},
        {CALL_MOUSE_ACTION, FN_MOUSE_ACTION, "[预检失败] 探索原版 0x52 走跑协议 CALL 不一致。"},
        {CALL_EXPLORE_CURSOR_DRAW, FN_EXPLORE_CURSOR_DRAW, "[预检失败] 探索 CURSOR.SCI 绘制 CALL 不一致。"},
        {CALL_RESULT_OUTER_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗结果外层继续 Event CALL 不一致。"},
        {CALL_RESULT_DETAIL_EVENT, FN_BUTTON_EVENT, "[预检失败] 战斗结果明细继续 Event CALL 不一致。"},
        {CALL_TARGET_MOUSE_POS_A, FN_MOUSE_SET_CURSOR_WRAPPER, "[预检失败] Target 原版阵位鼠标定位 A CALL 不一致。"},
        {CALL_TARGET_MOUSE_POS_B, FN_MOUSE_SET_CURSOR_WRAPPER, "[预检失败] Target 原版阵位鼠标定位 B CALL 不一致。"},
        {CALL_TARGET_MOUSE_POS_C, FN_MOUSE_SET_CURSOR_WRAPPER, "[预检失败] Target 原版阵位鼠标定位 C CALL 不一致。"},

        {CALL_TITLE_HIT_0, FN_BUTTON_HITTEST, "[预检失败] 标题按钮 0 HitTest CALL 不一致。"},
        {CALL_TITLE_HIT_1, FN_BUTTON_HITTEST, "[预检失败] 标题按钮 1 HitTest CALL 不一致。"},
        {CALL_TITLE_HIT_2, FN_BUTTON_HITTEST, "[预检失败] 标题按钮 2 HitTest CALL 不一致。"},
        {CALL_TITLE_EVENT_0, FN_BUTTON_EVENT, "[预检失败] 标题按钮 0 Event CALL 不一致。"},
        {CALL_TITLE_EVENT_1, FN_BUTTON_EVENT, "[预检失败] 标题按钮 1 Event CALL 不一致。"},
        {CALL_TITLE_EVENT_2, FN_BUTTON_EVENT, "[预检失败] 标题按钮 2 Event CALL 不一致。"},

        {CALL_SAVE_TICK_ANIM, FN_SAVE_SLOT_ANIM, "[预检失败] SaveSlot 游戏线程动画同步点 CALL 不一致。"},
        {CALL_SAVE_ROW_EVENT, FN_BUTTON_EVENT, "[预检失败] SaveSlot 行 Event CALL 不一致。"},
        {CALL_SAVE_CANCEL_EVENT, FN_BUTTON_EVENT, "[预检失败] SaveSlot 取消 Event CALL 不一致。"},
        {CALL_SAVE_PAGE_PREV, FN_BUTTON_EVENT, "[预检失败] SaveSlot 上一页 Event CALL 不一致。"},
        {CALL_SAVE_PAGE_NEXT, FN_BUTTON_EVENT, "[预检失败] SaveSlot 下一页 Event CALL 不一致。"}
    };

    /* 先确认 PE 的 DOS 头，防止完全错误的进程布局被误当作 RPG.exe。 */
    if (*(const u16*)0x00400000u != 0x5A4Du) {
        Runtime_Log("[预检失败] 0x00400000 不是 MZ 头，当前进程不是目标 RPG.exe 布局。");
        return 0;
    }

    for (i = 0; i < sizeof(byte_checks) / sizeof(byte_checks[0]); ++i) {
        if (!rt_check_byte_item(&byte_checks[i])) return 0;
    }
    for (i = 0; i < sizeof(call_checks) / sizeof(call_checks[0]); ++i) {
        if (!rt_check_call_item(&call_checks[i])) return 0;
    }

    Runtime_Log("[预检] dev20 冻结基线 41/41 CALL + refactor3 顶层动画 1/1 + refactor4 CMD1/CMD2 阶段切换 4/4 + 8/8 字节签名全部一致。");
    return 1;
}

/* A/LT 调查共用的 resolver 依赖是独立能力门，不连坐既有已验收能力。 */
int Runtime_InvestigationProtocolOk(void) {
    static const u8 sig_caller[] = {0xA1,0x08,0xF8,0x89,0x00,0x85,0xC0,0x0F,0x87,0x77,0x01,0x00,0x00,0x8B,0x46,0x20,0x8B,0xCE,0x50};
    static const u8 sig_resolver[] = {0x53,0x55,0x8B,0x6C,0x24,0x0C,0x56,0x8B,0xF1,0x57,0x8D,0x7D,0xFF,0x8B,0x46,0x1C,0x8B,0x4E,0x2C};
    static const u8 sig_table[] = {0x8B,0x46,0x30,0x8B,0x0C,0xB8,0x8B,0x51,0x70,0x8A,0x42,0x73,0x84,0xC0,0x74,0x22};
    static const u8 sig_type[] = {0x83,0xF8,0x05,0x55,0x7D,0x50,0x8B,0x56,0x1C,0x8B,0x46,0x2C,0x8B,0x4E,0x30,0x8B,0x14,0x90};
    static const u8 sig_distance[] = {0x8B,0x6A,0x10,0x8B,0x41,0x10,0x8B,0x49,0x14,0x2B,0xC5,0x8B,0x6A,0x14,0x2B,0xCD,0x8B,0xD1,0x0F,0xAF,0xD1,0x8B,0xC8,0x0F,0xAF,0xC8,0x03,0xD1};
    static const u8 distance_100[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x59,0x40};
    static const RtByteCheck bytes[] = {
        {SIG_EXPLORATION_TARGET_CALLER, sig_caller, sizeof(sig_caller), "[调查预检失败] resolver调用上下文不一致。"},
        {SIG_EXPLORATION_TARGET_RESOLVER, sig_resolver, sizeof(sig_resolver), "[调查预检失败] resolver入口不一致。"},
        {SIG_EXPLORATION_TARGET_TABLE, sig_table, sizeof(sig_table), "[调查预检失败] 对象表/互动类型布局不一致。"},
        {SIG_EXPLORATION_TARGET_TYPE, sig_type, sizeof(sig_type), "[调查预检失败] 互动类型分流不一致。"},
        {SIG_EXPLORATION_TARGET_DISTANCE, sig_distance, sizeof(sig_distance), "[调查预检失败] 距离计算不一致。"}
    };
    static const RtCallCheck resolver_call = {CALL_EXPLORATION_TARGET_RESOLVE, FN_EXPLORATION_TARGET_RESOLVE, "[调查预检失败] resolver CALL不一致。"};
    SIZE_T i;
    for (i = 0; i < sizeof(bytes) / sizeof(bytes[0]); ++i) if (!rt_check_byte_item(&bytes[i])) return 0;
    if (!Runtime_MemEq((const u8*)CONST_EXPLORATION_DISTANCE_100, distance_100, sizeof(distance_100))) {
        Runtime_Log("[调查预检失败] 距离常量100.0不一致。");
        return 0;
    }
    if (!rt_check_call_item(&resolver_call)) return 0;
    Runtime_Log("[调查] 原版互动协议通过：结构签名5/5 + 100.0常量1/1 + resolver CALL 1/1。");
    return 1;
}

/* ------------------------- 机器码修改 ------------------------- */

/*
 * IAT 改写先保存原函数，再临时放开 4 字节页面写权限。
 * original_out 让 Hook 可以随时调用真正 USER32/KERNEL32 实现，不形成递归。
 */
/*
 * 主 Interface 第一阶段的独立协议预检。
 *
 * refactor19 不再猜普通地图“菜单构造前门控”，而是严格复用原版 Space 键已经确认的业务事件：
 *
 *   0x44A4DF  cmp eax, 0x20              ; 原版收到 VK_SPACE
 *   0x44A4E8  cmp [0x89F808], 0          ; 地图动作当前空闲
 *   0x44A4F4  cmp [0x468BF0], 0          ; 当前地图模式允许该动作
 *   0x44A500  push 1
 *   0x44A57B  call 0x40B230              ; 真正的地图动作业务事件
 *
 * 手柄 Y 不发送 Space，也不调用窗口消息处理器；它只在 Exploration 游戏线程安全点调用
 * “原版 Space 当前实际 CALL 的业务目标”(action=1)。refactor20a 因此验证 Space 业务特征、
 * 动态 CALL 目标、Interface 根动画门与真正要 Hook 的根按钮 CALL，不再校验 0x40B230 整段函数体。
 */
int Runtime_InterfaceShellProtocolOk(void) {
    SIZE_T i;
    u32 map_event_target;

    /*
     * refactor20a 的兼容原则：
     *
     * 过去 r19/r20 在这里把 0x40B230 开头连续 0x23 字节全部当成“必须完全一样”。
     * 这实际上验证得过头了。我们的 Y 根本不修改 0x40B230；真正需要知道的是：
     * “原版 Space 现在仍然经过同一组地图条件，并最终 CALL 一个地图动作业务函数吗？”
     *
     * 因此现在只抓下面这些语义特征：
     *   cmp eax, 0x20             -> 当前键是 Space
     *   cmp [0x89F808], 0         -> 地图动作 busy 门
     *   cmp [0x468BF0], 0         -> 地图模式门
     *   push 1                    -> action=1
     * 条件跳转后面的 rel32 位移不属于业务语义，全部用 mask 忽略。
     */
    static const u8 sig_map_space_branch[] = {
        0x83,0xF8,0x20,0x0F,0x85,0x00,0x00,0x00,0x00,
        0x39,0x1D,0x08,0xF8,0x89,0x00,0x0F,0x85,0x00,0x00,0x00,0x00,
        0x39,0x1D,0xF0,0x8B,0x46,0x00,0x0F,0x84,0x00,0x00,0x00,0x00,
        0x6A,0x01,0xEB,0x00
    };
    static const u8 mask_map_space_branch[] = {
        0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
        0xFF,0xFF,0xFF,0x00
    };

    /* 原版根导航动画门：只有 Interface+0x590 > 8 才扫描顶部按钮。 */
    static const u8 sig_interface_ready[] = {
        0x83,0xB9,0x90,0x05,0x00,0x00,0x08,0x0F,0x9F,0xC0,0xC3
    };

    static const RtCallCheck calls[] = {
        {CALL_INTERFACE_CATEGORY_1, FN_BUTTON_EVENT, "[主界面预检失败] 大类1 Event CALL 不一致。"},
        {CALL_INTERFACE_CATEGORY_2, FN_BUTTON_EVENT, "[主界面预检失败] 大类2 Event CALL 不一致。"},
        {CALL_INTERFACE_CATEGORY_3, FN_BUTTON_EVENT, "[主界面预检失败] 大类3 Event CALL 不一致。"},
        {CALL_INTERFACE_CATEGORY_4, FN_BUTTON_EVENT, "[主界面预检失败] 大类4 Event CALL 不一致。"},
        {CALL_INTERFACE_CATEGORY_5, FN_BUTTON_EVENT, "[主界面预检失败] 大类5 Event CALL 不一致。"},
        {CALL_INTERFACE_CATEGORY_6, FN_BUTTON_EVENT, "[主界面预检失败] 大类6 Event CALL 不一致。"},
        {CALL_INTERFACE_CATEGORY_7, FN_BUTTON_EVENT, "[主界面预检失败] 大类7 Event CALL 不一致。"},
        {CALL_INTERFACE_CATEGORY_8, FN_BUTTON_EVENT, "[主界面预检失败] 大类8 Event CALL 不一致。"},
        {CALL_INTERFACE_EXIT_EVENT, FN_BUTTON_EVENT, "[主界面预检失败] 根层退出 Event CALL 不一致。"},
        {CALL_INTERFACE_ROLE_EVENT, FN_BUTTON_EVENT, "[主界面预检失败] 角色 Event CALL 不一致。"}
    };

    /*
     * 第一步：只验证 Space 业务分支的“结构特征”，不再拿整个函数体做指纹。
     * 这能容忍条件跳转目标位移变化，同时仍要求三个真正的业务判断和 action=1 全部存在。
     */
    if (!rt_mem_eq_masked((const u8*)SIG_MAP_SPACE_BRANCH,
                          sig_map_space_branch,
                          mask_map_space_branch,
                          sizeof(sig_map_space_branch))) {
        Runtime_Log("[主界面预检失败] 原版 Space 地图业务特征不匹配；未找到预期的 Space/busy/mode/action=1 结构。");
        Runtime_LogHexPair("[预检失败] 特征起点=", SIG_MAP_SPACE_BRANCH, "，特征长度=", (u32)sizeof(sig_map_space_branch));
        return 0;
    }

    /*
     * 第二步：从原版 Space 的 CALL 现场解析“它现在真正调用谁”。
     *
     * 基线是 0x40B230，但这里故意不要求 target 必须等于这个常量：
     * - 若其它兼容补丁把 CALL 改到自己的 wrapper，原版键盘 Space 本来就会先进入那个 wrapper；
     * - 手柄 Y 最合理的做法也是跟着原版当前 CALL，而不是绕开对方补丁硬跳旧地址；
     * - 只要 CALL 指令仍存在、目标是一个正常用户态地址，我们就认为“业务特征仍兼容”。
     */
    map_event_target = rt_call_target(CALL_MAP_SPACE_EVENT);
    if (!map_event_target || !Runtime_PtrOk((const void*)map_event_target)) {
        Runtime_Log("[主界面预检失败] 原版 Space 的地图动作 CALL 已不存在或目标地址无效。");
        Runtime_LogHexPair("[预检失败] CALL 地址=", CALL_MAP_SPACE_EVENT, "，解析目标=", map_event_target);
        return 0;
    }
    g_map_space_event_target = map_event_target;

    /* 第三步：已打开 Interface 的根动画门仍是我们真正读取的结构，因此保留精确短锚点。 */
    if (!Runtime_MemEq((const u8*)0x00436330u, sig_interface_ready, sizeof(sig_interface_ready))) {
        Runtime_Log("[主界面预检失败] Interface +0x590 > 8 根导航动画门不一致。");
        Runtime_LogHexPair("[预检失败] 字节签名地址=", 0x00436330u, "，长度=", (u32)sizeof(sig_interface_ready));
        return 0;
    }

    /* 第四步：根 Shell 会真正 Hook 的十个 ButtonEvent CALL 仍必须逐一精确匹配。 */
    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }

    if (map_event_target == FN_MAP_SPACE_EVENT) {
        Runtime_Log("[主界面] Space地图业务特征预检通过：当前CALL目标仍为基线0x40B230；不再校验其整段函数体。");
    } else {
        Runtime_LogHexPair("[主界面兼容] 原版Space地图业务CALL已被改写：基线目标=", FN_MAP_SPACE_EVENT,
                           "，当前目标=", map_event_target);
        Runtime_Log("[主界面兼容] 特征链仍成立；手柄Y将跟随原版Space当前CALL目标，不因函数体差异拒绝启动。");
    }
    Runtime_Log("[主界面] 根Shell协议预检通过：Space业务特征 + 动态CALL目标 + 1个短锚点 + 10/10 ButtonEvent CALL。");
    return 1;
}

u32 Runtime_MapSpaceEventTarget(void) {
    return g_map_space_event_target;
}

/*
 * 只检查“原版当前电影判定”和“ESC 关闭条件”两段窗口过程机器码。
 *
 * 为什么不把它塞进 Runtime_ExactBuildOk()：
 * refactor4 的战斗/标题/SaveSlot 已经实机封版。即使未来有别的补丁只改了电影窗口过程，
 * 我们也只应该禁用 Start->ESC，而不是让整个手柄 DLL 一起停掉。
 */
/*
 * 主 Interface state2“法宝/道具”页的独立协议预检。
 *
 * 这一页故意不并入 Runtime_InterfaceShellProtocolOk()：
 * - r19 的地图 Y、LB/RB、角色切换和根层 B 已经实机验收；
 * - 如果未来某个 MOD 只改了法宝页，我们只应关闭 state2 adapter，不能把整个主菜单 Shell 一起判死。
 *
 * 这里同时验证两类事实：
 * 1. 六个页面专属 CALL 的原目标仍然都是 0x431380 ButtonEvent；
 * 2. 0x4380D0 的原版逻辑仍然根据子类型动态写 Button+0x45，证明 +0x5E8/+0x5EC
 *    真的是“由原版决定是否可用”的两个特殊按钮。这样 X 可以只点击当前可用按钮，而不用插件猜子类型名称。
 */
int Runtime_InterfaceItemsProtocolOk(void) {
    SIZE_T i;

    static const u8 sig_special_enable[] = {
        0x83,0xFF,0x05,                         /* cmp edi,5 */
        0x0F,0x94,0xC2,                         /* sete dl */
        0x8B,0x88,0xE8,0x05,0x00,0x00,          /* mov ecx,[eax+5E8] */
        0x83,0xFF,0x05,                         /* cmp edi,5 */
        0x88,0x51,0x45,                         /* mov [ecx+45],dl */
        0x74,0x09,                              /* je ... */
        0x83,0xFF,0x04,                         /* cmp edi,4 */
        0x74,0x04,                              /* je ... */
        0xB0,0x01,                              /* mov al,1 */
        0xEB,0x02,                              /* jmp ... */
        0x32,0xC0,                              /* xor al,al */
        0x8B,0x96,0x90,0x05,0x00,0x00,          /* mov edx,[esi+590] */
        0x8B,0x8A,0xEC,0x05,0x00,0x00,          /* mov ecx,[edx+5EC] */
        0x88,0x41,0x45                          /* mov [ecx+45],al */
    };

    static const RtCallCheck calls[] = {
        {CALL_INTERFACE_ITEMS_SUBTYPE,   FN_BUTTON_EVENT, "[法宝页预检失败] 子类型 Event CALL 不一致。"},
        {CALL_INTERFACE_ITEMS_SPECIAL_A, FN_BUTTON_EVENT, "[法宝页预检失败] 特殊按钮A Event CALL 不一致。"},
        {CALL_INTERFACE_ITEMS_SPECIAL_B, FN_BUTTON_EVENT, "[法宝页预检失败] 特殊按钮B Event CALL 不一致。"},
        {CALL_INTERFACE_ITEMS_PAGE_PREV, FN_BUTTON_EVENT, "[法宝页预检失败] 上一页 Event CALL 不一致。"},
        {CALL_INTERFACE_ITEMS_PAGE_NEXT, FN_BUTTON_EVENT, "[法宝页预检失败] 下一页 Event CALL 不一致。"},
        {CALL_INTERFACE_ITEMS_ROW,       FN_BUTTON_EVENT, "[法宝页预检失败] 物品行 Event CALL 不一致。"}
    };

    if (!Runtime_MemEq((const u8*)SIG_INTERFACE_ITEMS_SPECIAL_ENABLE,
                       sig_special_enable,
                       sizeof(sig_special_enable))) {
        Runtime_Log("[法宝页预检失败] +0x5E8/+0x5EC 特殊按钮启用逻辑机器码不一致；state2 手柄功能已单独禁用。");
        return 0;
    }

    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) {
            Runtime_Log("[法宝页] 6 个页面专属 ButtonEvent CALL 未全部通过；state2 手柄功能已单独禁用。");
            return 0;
        }
    }

    Runtime_Log("[法宝页] 原版 state2 协议预检通过：特殊按钮机器码 1/1 + ButtonEvent 6/6。");
    return 1;
}

/*
 * 主 Interface state3“绝学/法术”页独立协议预检。
 * 这里只验证本页真正依赖的四个原版 ButtonEvent CALL，避免把 state3 的风险扩散到 r19 已验收 Shell。
 */
int Runtime_InterfaceSkillsProtocolOk(void) {
    SIZE_T i;
    /*
     * 0x43C19E 是原版 Update 进入治疗目标选择的门：读取 page+0x768 后立即决定是否调用 0x43C920 扫描角色 Button。
     * 短锚点只验证我们真正依赖的“+0x768 active”布局，不把整段函数机器码升级成脆弱的全局硬门槛。
     */
    static const u8 sig_target_mode[] = {0x8A,0x86,0x68,0x07,0x00,0x00,0x84,0xC0,0x74,0x7E};
    static const RtCallCheck calls[] = {
        {CALL_INTERFACE_SKILLS_SUBTYPE,       FN_BUTTON_EVENT, "[绝学页预检失败] 子类型 Event CALL 不一致。"},
        {CALL_INTERFACE_SKILLS_PAGE_PREV,     FN_BUTTON_EVENT, "[绝学页预检失败] 上一页 Event CALL 不一致。"},
        {CALL_INTERFACE_SKILLS_PAGE_NEXT,     FN_BUTTON_EVENT, "[绝学页预检失败] 下一页 Event CALL 不一致。"},
        {CALL_INTERFACE_SKILLS_ROW,           FN_BUTTON_EVENT, "[绝学页预检失败] 技能行 Event CALL 不一致。"},
        {CALL_INTERFACE_SKILLS_TARGET_ROW,    FN_BUTTON_EVENT, "[绝学页预检失败] 治疗目标角色 Event CALL 不一致。"},
        {CALL_INTERFACE_SKILLS_TARGET_CANCEL, FN_BUTTON_EVENT, "[绝学页预检失败] 治疗目标取消 Event CALL 不一致。"}
    };

    if (!Runtime_MemEq((const u8*)SIG_INTERFACE_SKILLS_TARGET_UPDATE, sig_target_mode, sizeof(sig_target_mode))) {
        Runtime_Log("[绝学页预检失败] 治疗目标选择 +0x768 状态锚点不一致；state3 手柄功能已单独禁用。");
        return 0;
    }

    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) {
            Runtime_Log("[绝学页] 6 个页面专属 ButtonEvent CALL 未全部通过；state3 手柄功能已单独禁用。");
            return 0;
        }
    }

    Runtime_Log("[绝学页] 原版 state3 协议预检通过：治疗目标状态锚点1/1 + ButtonEvent 6/6。");
    return 1;
}

/*
 * 主 Interface state4“及身/装备”页独立协议预检。
 *
 * 为什么除了 6 个 CALL 还要检查 Update 开头：
 * 这页的手柄逻辑会读取 page+0x580/+0x584/+0x590..+0x5A0。
 * 仅仅看到 CALL 仍然指向 0x431380，不足以证明页面对象布局没有被其它补丁重写。
 * 所以先核对 0x42DEF0 的短入口锚点，确认它仍然依次读取 +0x580/+0x579 这套 state4 布局；
 * 再核对 6 个 ButtonEvent CALL。任一失败都只让本页 fail-closed。
 */
int Runtime_InterfaceEquipmentProtocolOk(void) {
    SIZE_T i;
    static const u8 sig_update[] = {
        0x53,0x55,0x56,0x8B,0xF1,0x57,
        0x8B,0x86,0x80,0x05,0x00,0x00,
        0x8A,0x8E,0x79,0x05,0x00,0x00
    };
    static const RtCallCheck calls[] = {
        {CALL_INTERFACE_EQUIPMENT_PAGE_PREV, FN_BUTTON_EVENT, "[及身页预检失败] 上一页 Event CALL 不一致。"},
        {CALL_INTERFACE_EQUIPMENT_PAGE_NEXT, FN_BUTTON_EVENT, "[及身页预检失败] 下一页 Event CALL 不一致。"},
        {CALL_INTERFACE_EQUIPMENT_ROW,       FN_BUTTON_EVENT, "[及身页预检失败] 候选装备行 Event CALL 不一致。"},
        {CALL_INTERFACE_EQUIPMENT_SLOT_0,    FN_BUTTON_EVENT, "[及身页预检失败] 第0栏位 Event CALL 不一致。"},
        {CALL_INTERFACE_EQUIPMENT_SLOT_1,    FN_BUTTON_EVENT, "[及身页预检失败] 第1栏位 Event CALL 不一致。"},
        {CALL_INTERFACE_EQUIPMENT_SLOT_2,    FN_BUTTON_EVENT, "[及身页预检失败] 第2栏位 Event CALL 不一致。"}
    };

    if (!Runtime_MemEq((const u8*)SIG_INTERFACE_EQUIPMENT_UPDATE, sig_update, sizeof(sig_update))) {
        Runtime_Log("[及身页预检失败] state4 Update 入口布局锚点不一致；本页 Adapter 已单独禁用。");
        return 0;
    }

    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) {
            Runtime_Log("[及身页] 6 个页面专属 ButtonEvent CALL 未全部通过；state4 手柄功能已单独禁用。");
            return 0;
        }
    }

    Runtime_Log("[及身页] 原版 state4 协议预检通过：布局短锚点 1/1 + ButtonEvent 6/6。");
    return 1;
}

/*
 * 主 Interface state5“五内”页独立协议预检。
 *
 * 这一页原版用同一个 11 按钮循环，里面先调用 0x431400 读取鼠标按键边沿，再调用 0x431380 处理 ButtonEvent。
 * refactor23 为了把“可见的合成焦点鼠标”和“真正鼠标业务”彻底隔离，两处 CALL 都必须精确预检。
 * Adapter 还依赖 page+0x57C 指向的面板以及 panel+0x57C 起始的连续按钮表，
 * 所以仍然先检查 0x440610 Update 开头的布局锚点，再检查这两处循环 CALL。
 * 任一项不一致都只禁用 state5，不影响已经实机通过的 state1~3 或本轮 state4。
 */
int Runtime_InterfaceInnerStatsProtocolOk(void) {
    static const u8 sig_update[] = {
        0x51,0x56,0x8B,0xF1,
        0x8B,0x86,0x7C,0x05,0x00,0x00,
        0x8A,0x8E,0x79,0x05,0x00,0x00
    };
    static const RtCallCheck calls[] = {
        {CALL_INTERFACE_INNER_STATS_MOUSE, FN_BUTTON_MOUSE_EDGE,
         "[五内页预检失败] 11按钮循环的鼠标边沿 CALL 不一致。"},
        {CALL_INTERFACE_INNER_STATS_EVENT, FN_BUTTON_EVENT,
         "[五内页预检失败] 11按钮循环的 Event CALL 不一致。"}
    };
    /*
     * 0x43E6F0 是本轮唯一新增的主动调用地址，因此必须独立验证入口机器码。
     * 这 8 字节只证明“push esi / this 保存 / 检查 Sprite active”入口仍与当前 Oracle 一致；
     * 如果另一个 EXE 在这里不同，state5 直接 fail-closed，绝不能盲调一个同地址但不同语义的函数。
     */
    static const u8 sig_sprite_feedback[] = {0x56,0x8B,0xF1,0x8A,0x46,0x08,0x84,0xC0};

    if (!Runtime_MemEq((const u8*)SIG_INTERFACE_INNER_STATS_UPDATE, sig_update, sizeof(sig_update))) {
        Runtime_Log("[五内页预检失败] state5 Update 入口布局锚点不一致；本页 Adapter 已单独禁用。");
        return 0;
    }
    if (!rt_check_call_item(&calls[0]) || !rt_check_call_item(&calls[1])) {
        Runtime_Log("[五内页] 页面专属鼠标/事件 CALL 未通过；state5 手柄功能已单独禁用。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)FN_BUTTON_SPRITE_FEEDBACK, sig_sprite_feedback, sizeof(sig_sprite_feedback))) {
        Runtime_Log("[五内页预检失败] 0x43E6F0 Button Sprite反馈函数入口不一致；拒绝执行refactor26d idle恢复。");
        return 0;
    }

    Runtime_Log("[五内页] 原版 state5 协议预检通过：布局1/1 + 鼠标边沿/ButtonEvent 2/2 + Button Sprite反馈入口1/1。");
    return 1;
}

/*
 * 主 Interface state6“阵形”页独立协议预检。
 *
 * 本页 Adapter 依赖三件已经由反汇编闭合的事实：
 * 1. 0x43F9A0 是 state6 Update；入口先同步 page+0x57C 的外层对象，随后从 page+0x580 取得真正阵形面板；
 * 2. 0x43F9E4 开始用 panel+0x59C 连续扫描 8 个真实阵位 Button；
 * 3. 0x43FA05 对每个阵位调用 0x431380，并由返回的 code=1/2 驱动原版取消/来源选择/目标交换状态机。
 *
 * 任意一项不一致都只禁用 state6。插件绝不直接写 page+0x584/+0x588/+0x58C 或阵位映射表，
 * 因此即使另一个 EXE 仅仅“看起来布局相似”，也不能在证据不足时强行启用。
 */
int Runtime_InterfaceFormationProtocolOk(void) {
    static const u8 sig_update[] = {
        0x51,0x56,0x8B,0xF1,0x8B,0x86,0x7C,0x05,0x00,0x00,
        0x8A,0x8E,0x79,0x05,0x00,0x00
    };
    static const u8 sig_button_loop[] = {
        0xBD,0x9C,0x05,0x00,0x00,0x33,0xDB,
        0x81,0xED,0xF8,0xBF,0x46,0x00
    };
    static const RtCallCheck call = {
        CALL_INTERFACE_FORMATION_EVENT, FN_BUTTON_EVENT,
        "[阵形页预检失败] 8阵位 ButtonEvent 循环 CALL 不一致。"
    };

    /*
     * 这两张表是 0x43F9A0 自己移动阵形来源/目标图示时读取的局部坐标。
     * r24 的二维邻居直接依赖它们，因此和 Update/CALL 一样必须做机器级预检，不能默默拿错表。
     */
    static const i32 expected_layout_x[INTERFACE_FORMATION_SLOT_COUNT] = {
        227, 171, 115, 59, 283, 227, 171, 115
    };
    static const i32 expected_layout_y[INTERFACE_FORMATION_SLOT_COUNT] = {
        28, 56, 84, 112, 56, 84, 112, 142
    };

    if (!Runtime_MemEq((const u8*)SIG_INTERFACE_FORMATION_UPDATE, sig_update, sizeof(sig_update))) {
        Runtime_Log("[阵形页预检失败] state6 Update 入口布局锚点不一致；本页 Adapter 已单独禁用。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)SIG_INTERFACE_FORMATION_BUTTON_LOOP, sig_button_loop, sizeof(sig_button_loop))) {
        Runtime_Log("[阵形页预检失败] panel+0x59C 的 8阵位循环锚点不一致；本页 Adapter 已单独禁用。");
        return 0;
    }
    if (!rt_check_call_item(&call)) {
        Runtime_Log("[阵形页] 原版阵位 Event CALL 未通过；state6 手柄功能已单独禁用。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)INTERFACE_FORMATION_LAYOUT_X_TABLE,
                       (const u8*)expected_layout_x, sizeof(expected_layout_x)) ||
        !Runtime_MemEq((const u8*)INTERFACE_FORMATION_LAYOUT_Y_TABLE,
                       (const u8*)expected_layout_y, sizeof(expected_layout_y))) {
        Runtime_Log("[阵形页预检失败] 原版 8 阵位 X/Y 布局表不一致；二维导航已单独禁用。");
        return 0;
    }

    Runtime_Log("[阵形页] 原版 state6 协议预检通过：Update/8阵位循环锚点 2/2 + ButtonEvent 1/1 + 布局表 2/2。");
    return 1;
}


/*
 * SaveSlot+0x5A4 三项动作窗口是共享 SaveSlot 的子对象，不是 state7 天书私有能力。
 * 原版 mode=0/2 都会进入它，其中 mode=2 正是地图存档点。因此其短签名与两处 CALL
 * 必须独立预检、独立安装，不能再挂在 Interface state7 构造入口之下。
 */
int Runtime_SaveSlotActionProtocolOk(void) {
    static const u8 sig_action_update[] = {
        0x53,0x55,0x56,0x8B,0xF1,0xB9,0x03,0x00,
        0x00,0x00,0x8D,0xAE,0x8C,0x05,0x00,0x00
    };
    static const RtCallCheck calls[] = {
        {CALL_SAVE_ACTION_HIT, FN_BUTTON_HITTEST, "[共享存读档动作预检失败] 三项窗口 HitTest CALL 不一致。"},
        {CALL_SAVE_ACTION_EVENT, FN_BUTTON_EVENT, "[共享存读档动作预检失败] 三项窗口 ButtonEvent CALL 不一致。"}
    };
    SIZE_T i;

    if (!Runtime_MemEq((const u8*)SIG_SAVE_ACTION_UPDATE, sig_action_update, sizeof(sig_action_update))) {
        Runtime_Log("[共享存读档动作预检失败] 0x4262C0 三项窗口短签名不一致；该子能力已单独禁用。");
        return 0;
    }
    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }
    Runtime_Log("[共享存读档动作] 原版协议预检通过：三项窗口短签名1/1 + Hit/Event CALL 2/2。");
    return 1;
}

/* state7 Adapter 只需证明自己的 owner 入口仍持有标准 SaveSlot；共享子窗口另行校验。 */
int Runtime_InterfaceTomeProtocolOk(void) {
    static const RtCallCheck owner_call = {
        CALL_INTERFACE_TOME_CTOR,
        FN_SAVE_SLOT_CTOR,
        "[天书页预检失败] Interface state7 构造 CALL 不再指向原版 SaveSlot。"
    };

    if (!rt_check_call_item(&owner_call)) return 0;
    Runtime_Log("[天书页] 原版 owner 协议预检通过：state7 SaveSlot 构造 CALL 1/1。");
    return 1;
}

/*
 * 地图存档点/事件 opcode 0x3E 的独立 SaveSlot 包装层预检。
 *
 * 这组预检不并入 Runtime_ExactBuildOk()：标题、Battle、主 Interface 等旧功能已经用户验收，
 * 即使某个兼容 EXE 只修改了 0x413B00 这个独立包装层，也只应让 save_point.c fail-closed，
 * 不能连坐让整个 ASI 拒绝启动。
 *
 * 我们真正依赖四个事实：
 * 1. 地图事件状态机在 0x40DB2D 构造 0x413B00 包装层；
 * 2. 构造结果仍发布到全局 0x89FCD0；
 * 3. 包装层在 0x413C3F 真正构造标准 0x4245B0 SaveSlot，并写到 wrapper+0x580；
 * 4. 包装层 Update 仍从 +0x580 取 SaveSlot，再读 SaveSlot+0x5B8 完成标志。
 *
 * 不在这里检查存档文件格式：文件 I/O 始终由 RPG.exe 的原版 SaveSlot 完成。
 */
int Runtime_SavePointProtocolOk(void) {
    static const u8 sig_publish[] = {
        0xA3,0xD0,0xFC,0x89,0x00,
        0xC7,0x05,0x18,0xF8,0x89,0x00,0x02,0x00,0x00,0x00
    };
    static const u8 sig_bind[] = {
        0x89,0x86,0x80,0x05,0x00,0x00,
        0xC6,0x80,0x79,0x05,0x00,0x00,0x00,
        0x8B,0x86,0x80,0x05,0x00,0x00
    };
    static const u8 sig_update[] = {
        0x8B,0x86,0x80,0x05,0x00,0x00,
        0x8A,0x88,0xB8,0x05,0x00,0x00,
        0x84,0xC9,0x74,0x33
    };
    static const RtCallCheck calls[] = {
        {CALL_MAP_SAVE_POINT_WRAPPER_CTOR, FN_SAVE_POINT_WRAPPER_CTOR,
         "[存档点预检失败] 地图事件状态机不再构造 0x413B00 包装层。"},
        {CALL_SAVE_POINT_SLOT_CTOR, FN_SAVE_SLOT_CTOR,
         "[存档点预检失败] 独立包装层不再构造标准 SaveSlot。"}
    };
    SIZE_T i;

    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }
    if (!Runtime_MemEq((const u8*)SIG_SAVE_POINT_WRAPPER_PUBLISH, sig_publish, sizeof(sig_publish))) {
        Runtime_Log("[存档点预检失败] 包装对象发布到 0x89FCD0 的机器码锚点不一致。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)SIG_SAVE_POINT_WRAPPER_BIND, sig_bind, sizeof(sig_bind))) {
        Runtime_Log("[存档点预检失败] wrapper+0x580 绑定 SaveSlot 的布局锚点不一致。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)SIG_SAVE_POINT_WRAPPER_UPDATE, sig_update, sizeof(sig_update))) {
        Runtime_Log("[存档点预检失败] 包装层 Update 的 +0x580/+0x5B8 生命周期锚点不一致。");
        return 0;
    }

    Runtime_Log("[存档点] 原版协议预检通过：包装构造1/1 + SaveSlot构造1/1 + 发布/绑定/Update锚点3/3。");
    return 1;
}

/*
 * state8“机能”页面的独立协议预检。
 *
 * 原版 0x428950 按固定顺序处理五个真实按钮：音乐减、音乐加、音效减、音效加、空明流转。
 * 手柄只会在这五处 ButtonEvent 调用点注入一次 code=2；0..9 数值改变、音量应用、
 * 配置写回以及“空明流转”打开 Yes/No 全部继续由 RPG.exe 自己完成。
 */
int Runtime_InterfaceOptionsProtocolOk(void) {
    static const u8 sig_options_update[] = {
        0x53,0x56,0x8B,0xF1,0x32,0xDB,0x8B,0x8E,
        0xA8,0x05,0x00,0x00,0x8A,0x81,0x79,0x05,
        0x00,0x00
    };
    static const RtCallCheck calls[] = {
        {CALL_INTERFACE_OPTIONS_CTOR, FN_INTERFACE_OPTIONS_CTOR, "[机能页预检失败] Interface state8 构造 CALL 不一致。"},
        {CALL_INTERFACE_OPTIONS_MUSIC_DEC, FN_BUTTON_EVENT, "[机能页预检失败] 音乐减少 Event CALL 不一致。"},
        {CALL_INTERFACE_OPTIONS_MUSIC_INC, FN_BUTTON_EVENT, "[机能页预检失败] 音乐增加 Event CALL 不一致。"},
        {CALL_INTERFACE_OPTIONS_SOUND_DEC, FN_BUTTON_EVENT, "[机能页预检失败] 音效减少 Event CALL 不一致。"},
        {CALL_INTERFACE_OPTIONS_SOUND_INC, FN_BUTTON_EVENT, "[机能页预检失败] 音效增加 Event CALL 不一致。"},
        {CALL_INTERFACE_OPTIONS_KARMA, FN_BUTTON_EVENT, "[机能页预检失败] 空明流转 Event CALL 不一致。"}
    };
    SIZE_T i;

    if (!Runtime_MemEq((const u8*)SIG_INTERFACE_OPTIONS_UPDATE, sig_options_update, sizeof(sig_options_update))) {
        Runtime_Log("[机能页预检失败] 0x428950 页面 Update 短签名不一致；state8 已单独禁用。");
        return 0;
    }
    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }
    Runtime_Log("[机能页] 原版协议预检通过：页面构造1/1 + Update短签名1/1 + ButtonEvent 5/5。");
    return 1;
}


/*
 * 客栈三项根菜单的独立协议预检。
 *
 * 这个对象曾在早期研究里被笼统叫作 MapAction，但 0x413850 的真实分支已经闭合：
 * index0 创建主 Interface，index1 创建炼化，index2 进入歇息/剧情流程。
 * refactor29 只依赖它的 Update 身份以及三按钮共用的 HitTest/Event 两处 CALL。
 */
int Runtime_InnProtocolOk(void) {
    static const u8 sig_inn_update[] = {
        0x6A,0xFF,0x68,0xB7,0xD6,0x45,0x00,0x64,
        0xA1,0x00,0x00,0x00,0x00,0x50,0x64,0x89
    };
    static const RtCallCheck calls[] = {
        {CALL_INN_BUTTON_HIT, FN_BUTTON_HITTEST, "[客栈预检失败] 三项按钮 HitTest CALL 不一致。"},
        {CALL_INN_BUTTON_EVENT, FN_BUTTON_EVENT, "[客栈预检失败] 三项按钮 Event CALL 不一致。"}
    };
    SIZE_T i;

    if (!Runtime_MemEq((const u8*)SIG_INN_UPDATE, sig_inn_update, sizeof(sig_inn_update))) {
        Runtime_Log("[客栈预检失败] 0x413850 根菜单 Update 身份不一致；客栈 Adapter 已单独禁用。");
        return 0;
    }
    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }
    Runtime_Log("[客栈] 原版协议预检通过：Update短签名1/1 + Hit/Event CALL 2/2。");
    return 1;
}

/*
 * 炼化两层菜单的独立协议预检。
 *
 * 这里校验的是我们真正会改写的 8 个 ButtonEvent CALL，以及构造/Update 的短身份锚点。
 * 任何一项不匹配都只让炼化手柄功能 fail-closed；客栈、主 Interface、Battle 等其它模块不受影响。
 */
int Runtime_SynthesisProtocolOk(void) {
    static const u8 sig_ctor[] = {
        0x6A,0xFF,0x68,0xC0,0xD2,0x45,0x00,0x64,
        0xA1,0x00,0x00,0x00,0x00,0x50,0x64,0x89
    };
    static const u8 sig_update[] = {
        0x64,0xA1,0x00,0x00,0x00,0x00,0x6A,0xFF,
        0x68,0xFB,0xD2,0x45,0x00,0x50,0x64,0x89
    };
    static const RtCallCheck calls[] = {
        {CALL_SYNTHESIS_TOP_EVENT, FN_BUTTON_EVENT, "[炼化预检失败] 顶部类别/退出 Event CALL 不一致。"},
        {CALL_SYNTHESIS_PRIMARY_PREV, FN_BUTTON_EVENT, "[炼化预检失败] 第一层上一页 Event CALL 不一致。"},
        {CALL_SYNTHESIS_PRIMARY_NEXT, FN_BUTTON_EVENT, "[炼化预检失败] 第一层下一页 Event CALL 不一致。"},
        {CALL_SYNTHESIS_SECONDARY_PREV, FN_BUTTON_EVENT, "[炼化预检失败] 第二层上一页 Event CALL 不一致。"},
        {CALL_SYNTHESIS_SECONDARY_NEXT, FN_BUTTON_EVENT, "[炼化预检失败] 第二层下一页 Event CALL 不一致。"},
        {CALL_SYNTHESIS_SECONDARY_CANCEL, FN_BUTTON_EVENT, "[炼化预检失败] 第二层取消 Event CALL 不一致。"},
        {CALL_SYNTHESIS_SECONDARY_ROW, FN_BUTTON_EVENT, "[炼化预检失败] 第二层物品行 Event CALL 不一致。"},
        {CALL_SYNTHESIS_PRIMARY_ROW, FN_BUTTON_EVENT, "[炼化预检失败] 第一层物品行 Event CALL 不一致。"}
    };
    SIZE_T i;

    if (!Runtime_MemEq((const u8*)FN_SYNTHESIS_CTOR, sig_ctor, sizeof(sig_ctor))) {
        Runtime_Log("[炼化预检失败] 0x40F6D0 构造函数短签名不一致；炼化 Adapter 已单独禁用。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)SIG_SYNTHESIS_UPDATE, sig_update, sizeof(sig_update))) {
        Runtime_Log("[炼化预检失败] 0x40FBE0 Update 短签名不一致；炼化 Adapter 已单独禁用。");
        return 0;
    }
    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }

    Runtime_Log("[炼化] 原版协议预检通过：Ctor/Update锚点2/2 + 两层 ButtonEvent CALL 8/8。");
    return 1;
}

/*
 * 商店统一界面的独立协议预检。
 *
 * 0x40D120 的 EVE opcode 0x36 无论脚本给哪一种店铺 mode，都只构造 0x413FA0 并写入
 * 0x89FCD4；mode 在 0x415912 只选择一条 0x288-byte 商店记录，菜单控制器不分店型。
 * refactor35 按 refactor33 全商店实机 PASS 基线恢复主体协议：
 * - 主对象 ctor / Update；
 * - 数量窗 ctor / Update；
 * - 顶部、两列翻页/行、数量增减/确认/取消共 11 个真实 ButtonEvent CALL；
 * 必须整组精确一致，才启用 Shop Adapter。失败只关闭商店手柄能力，不连坐已验收模块。
 */
int Runtime_ShopProtocolOk(void) {
    static const u8 sig_ctor[] = {
        0x6A,0xFF,0x68,0xB1,0xD7,0x45,0x00,0x64,
        0xA1,0x00,0x00,0x00,0x00,0x50,0x64,0x89
    };
    static const u8 sig_update[] = {
        0x83,0xEC,0x1C,0x53,0x55,0x56,0x8B,0xF1,
        0x83,0xCB,0xFF,0x57,0x8B,0x86,0x9C,0x05,0x00,0x00
    };
    static const u8 sig_quantity_ctor[] = {
        0x6A,0xFF,0x68,0xDB,0xE6,0x45,0x00,0x64,
        0xA1,0x00,0x00,0x00,0x00,0x50,0x64,0x89
    };
    static const u8 sig_quantity_update[] = {
        0x51,0x55,0x56,0x8B,0xF1,0x33,0xED,0x57,
        0x8B,0x86,0x7C,0x05,0x00,0x00,0x3B,0xC5
    };
    static const RtCallCheck calls[] = {
        {CALL_SHOP_TOP_EVENT,        FN_BUTTON_EVENT, "[商店预检失败] 顶部类别/退出 Event CALL 不一致。"},
        {CALL_SHOP_SELL_PAGE_PREV,   FN_BUTTON_EVENT, "[商店预检失败] 右侧卖出列上一页 Event CALL 不一致。"},
        {CALL_SHOP_SELL_PAGE_NEXT,   FN_BUTTON_EVENT, "[商店预检失败] 右侧卖出列下一页 Event CALL 不一致。"},
        {CALL_SHOP_BUY_PAGE_PREV,    FN_BUTTON_EVENT, "[商店预检失败] 左侧买入列上一页 Event CALL 不一致。"},
        {CALL_SHOP_BUY_PAGE_NEXT,    FN_BUTTON_EVENT, "[商店预检失败] 左侧买入列下一页 Event CALL 不一致。"},
        {CALL_SHOP_SELL_ROW_EVENT,   FN_BUTTON_EVENT, "[商店预检失败] 右侧卖出列物品 Event CALL 不一致。"},
        {CALL_SHOP_BUY_ROW_EVENT,    FN_BUTTON_EVENT, "[商店预检失败] 左侧买入列物品 Event CALL 不一致。"},
        {CALL_SHOP_QUANTITY_CONFIRM, FN_BUTTON_EVENT, "[商店预检失败] 数量窗确认 Event CALL 不一致。"},
        {CALL_SHOP_QUANTITY_CANCEL,  FN_BUTTON_EVENT, "[商店预检失败] 数量窗取消 Event CALL 不一致。"},
        {CALL_SHOP_QUANTITY_INC,     FN_BUTTON_EVENT, "[商店预检失败] 数量增加 Event CALL 不一致。"},
        {CALL_SHOP_QUANTITY_DEC,     FN_BUTTON_EVENT, "[商店预检失败] 数量减少 Event CALL 不一致。"}
    };
    SIZE_T i;

    if (!Runtime_MemEq((const u8*)FN_SHOP_CTOR, sig_ctor, sizeof(sig_ctor))) {
        Runtime_Log("[商店预检失败] 0x413FA0 统一商店构造函数短签名不一致；Shop Adapter 已单独禁用。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)SIG_SHOP_UPDATE, sig_update, sizeof(sig_update))) {
        Runtime_Log("[商店预检失败] 0x4145B0 统一商店 Update 短签名不一致；Shop Adapter 已单独禁用。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)FN_SHOP_QUANTITY_CTOR, sig_quantity_ctor, sizeof(sig_quantity_ctor)) ||
        !Runtime_MemEq((const u8*)SIG_SHOP_QUANTITY_UPDATE, sig_quantity_update, sizeof(sig_quantity_update))) {
        Runtime_Log("[商店预检失败] 0x427920/0x427E80 数量窗协议不一致；Shop Adapter 已单独禁用。");
        return 0;
    }
    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }

    Runtime_Log("[商店] refactor33 主体协议预检通过：统一主界面/数量窗锚点4/4 + ButtonEvent 11/11。");
    return 1;
}

/*
 * Y 道具信息协议独立预检。它只决定 Y 与导航抑制能力，任何失败都不得关闭主体。
 * 信息窗关闭 Event 的 0x417829 就是全局 CALL_CMD0_EVENT：ExactBuild 已在任何改写前验证，
 * UiBridge_InstallHooks 又先于 Shop 把它接入公共桥。这里运行时已经不能再拿它和原目标比较，
 * 否则会把“公共桥已正确安装”误判成协议失败，这正是 r35 一移动就开窗且 Y 无效的根因。
 */
int Runtime_ShopItemInfoProtocolOk(void) {
    static const u8 sig_item_info_update[] = {
        0x56,0x8B,0xF1,0x8B,0x86,0x8C,0x05,0x00,
        0x00,0x8B,0x8E,0x6C,0x05,0x00,0x00,0x3B,0xC1
    };
    static const RtCallCheck calls[] = {
        {CALL_SHOP_SELL_ITEM_INFO_OPEN, FN_SHOP_ITEM_INFO_TRANSITION, "[商店信息预检失败] 右侧行 transition CALL 不一致。"},
        {CALL_SHOP_BUY_ITEM_INFO_OPEN, FN_SHOP_ITEM_INFO_TRANSITION, "[商店信息预检失败] 左侧行 transition CALL 不一致。"}
    };
    SIZE_T i;

    if (!Runtime_MemEq((const u8*)SIG_SHOP_ITEM_INFO_UPDATE,
            sig_item_info_update, sizeof(sig_item_info_update))) {
        Runtime_Log("[商店信息预检失败] 0x417810 信息窗 Update 短签名不一致。");
        return 0;
    }
    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }

    Runtime_Log("[商店信息] 可选协议预检通过：信息窗锚点1/1 + 复用公共Event + transition 2/2。");
    return 1;
}

int Runtime_MovieEscapeProtocolOk(void) {
    static const u8 sig_movie_active[] = {0xA1,0x90,0xF3,0x46,0x00,0x53,0x56,0x57,0x85,0xC0};
    static const u8 sig_movie_esc[] = {0x8B,0x7D,0x10,0x83,0xFF,0x1B,0x74,0x16};

    if (!Runtime_MemEq((const u8*)SIG_MOVIE_ACTIVE_CHECK, sig_movie_active, sizeof(sig_movie_active))) {
        Runtime_Log("[动画] 原版电影活动判定机器码不一致；Start=ESC 功能已单独禁用。");
        return 0;
    }

    if (!Runtime_MemEq((const u8*)SIG_MOVIE_ESC_CHECK, sig_movie_esc, sizeof(sig_movie_esc))) {
        Runtime_Log("[动画] 原版 ESC 关闭电影条件机器码不一致；Start=ESC 功能已单独禁用。");
        return 0;
    }

    if (!g_api.post_message_a) {
        Runtime_Log("[动画] USER32.PostMessageA 不可用；Start=ESC 功能已单独禁用。");
        return 0;
    }

    Runtime_Log("[动画] 原版电影活动判定与 ESC 关闭协议校验通过。");
    return 1;
}


/*
 * 公共剧情消息 mode=2/mode=3 的独立协议校验。
 *
 * 这一层和普通对话 A、菜单 ConfirmDialog 都不同。0x403E30 的公共消息分发器会按 mode 选择：
 * - mode=2 调用 0x4044F0，处理截图中“我的回答是”这类两行或多行剧情选项；
 * - mode=3 调用 0x404600，处理固定的“是/否”两项选择。
 *
 * 两条原版路径都依靠真实鼠标命中并在检测到左键后自行写 VAR999。SceneChoice 不改机器码、
 * 不直接写剧情结果，只把隐藏鼠标放进正确行并发送左键。因此必须同时核对两条选择器和提交 CALL；
 * 任一锚点不一致都只禁用剧情选择手柄，不连坐普通对话推进。
 */
int Runtime_SceneChoiceProtocolOk(void) {
    static const u8 sig_dispatch[] = {
        0xA1,0x40,0xF6,0x46,0x00,0x83,0xF8,0x03,0x77,0x21,0xFF,0x24,0x85,0x8C,0x3E,0x40,0x00
    };
    static const u8 sig_multi_update[] = {
        0x83,0xEC,0x08,0x8D,0x44,0x24,0x00,0x53,0x50,0xFF,0x15,0x04,0x02,0x46,0x00
    };
    static const u8 sig_yes_no_update[] = {
        0x83,0xEC,0x08,0x8D,0x44,0x24,0x00,0x50,0xFF,0x15,0x04,0x02,0x46,0x00
    };
    static const RtCallCheck calls[] = {
        {CALL_DIALOGUE_MULTI_CHOICE_UPDATE, FN_DIALOGUE_MULTI_CHOICE_UPDATE, "[剧情选择预检失败] mode=2 分发 CALL 不一致。"},
        {CALL_DIALOGUE_MULTI_CHOICE_RESULT, 0x0044B0B0u, "[剧情选择预检失败] mode=2 原版结果提交 CALL 不一致。"},
        {CALL_DIALOGUE_CHOICE_UPDATE, FN_DIALOGUE_CHOICE_UPDATE, "[剧情选择预检失败] mode=3 分发 CALL 不一致。"},
        {0x00404697u, 0x0044B0B0u, "[剧情选择预检失败] 第二项/结果2原版提交 CALL 不一致。"},
        {0x004046AFu, 0x0044B0B0u, "[剧情选择预检失败] 第一项/结果1原版提交 CALL 不一致。"}
    };
    SIZE_T i;

    if (!Runtime_MemEq((const u8*)SIG_DIALOGUE_CHOICE_DISPATCH, sig_dispatch, sizeof(sig_dispatch))) {
        Runtime_Log("[剧情选择预检失败] 0x403E30 mode 分发器身份不一致；剧情选项手柄已单独禁用。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)SIG_DIALOGUE_MULTI_CHOICE_CURSOR,
                       sig_multi_update, sizeof(sig_multi_update))) {
        Runtime_Log("[剧情选择预检失败] 0x4044F0 多行鼠标选择器入口不一致；剧情选项手柄已单独禁用。");
        return 0;
    }
    if (!Runtime_MemEq((const u8*)SIG_DIALOGUE_CHOICE_CURSOR,
                       sig_yes_no_update, sizeof(sig_yes_no_update))) {
        Runtime_Log("[剧情选择预检失败] 0x404600 是/否鼠标选择器入口不一致；剧情选项手柄已单独禁用。");
        return 0;
    }
    for (i = 0; i < sizeof(calls) / sizeof(calls[0]); ++i) {
        if (!rt_check_call_item(&calls[i])) return 0;
    }
    if (!g_api.mouse_event || !g_api.set_cursor_pos) {
        Runtime_Log("[剧情选择预检失败] USER32 mouse_event/SetCursorPos 不可用；剧情选项手柄已单独禁用。");
        return 0;
    }

    Runtime_Log("[剧情选择] 原版 mode=2/mode=3 协议预检通过：分发器 + 两个选择器 + 原版 CALL 5/5。");
    return 1;
}

/*
 * 公共消息引擎 A 推进的独立协议校验。
 * refactor11 延续“不 Hook 全局 IAT”的局部方案，并把两个公共消息函数分别接到逐字阶段/整句阶段两个不同包装函数。
 */
int Runtime_DialogueProtocolOk(void) {
    static const u8 sig_active_begin[] = {0xA1,0x70,0xF6,0x46,0x00,0x53,0x33,0xDB,0x3B,0xC3};
    static const u8 sig_input_a[] = {0x8B,0x35,0xA4,0x01,0x46,0x00};
    static const u8 sig_input_b[] = {0x8B,0x35,0xA4,0x01,0x46,0x00};
    static const u8 sig_active_end[] = {0x89,0x35,0x70,0xF6,0x46,0x00};

    if (!Runtime_MemEq((const u8*)SIG_DIALOGUE_ACTIVE_BEGIN, sig_active_begin, sizeof(sig_active_begin)) ||
        !Runtime_MemEq((const u8*)SIG_DIALOGUE_INPUT_A, sig_input_a, sizeof(sig_input_a)) ||
        !Runtime_MemEq((const u8*)SIG_DIALOGUE_INPUT_B, sig_input_b, sizeof(sig_input_b)) ||
        !Runtime_MemEq((const u8*)SIG_DIALOGUE_ACTIVE_END, sig_active_end, sizeof(sig_active_end))) {
        Runtime_Log("[对话] 公共消息引擎局部输入机器码不一致；A 推进已单独禁用。");
        return 0;
    }
    if (!Runtime_PtrOk(*(void**)IAT_GETASYNCKEYSTATE)) {
        Runtime_Log("[对话] GetAsyncKeyState IAT 不是有效函数指针；A 推进已单独禁用。");
        return 0;
    }
    Runtime_Log("[对话] 公共消息活动槽与两处局部 GetAsyncKeyState 装载协议校验通过。");
    return 1;
}

/*
 * 这里校验的是 refactor10/refactor11 遗留“双按钮候选层”所依赖的两种 vtable Update 槽。
 * 用户已经否定把它扩张成通用选择框架构；refactor15 仍只保持 r11 行为并做隔离护栏。
 * 第二种 update 内四个 HitTest/Event CALL 仍需校验，因为现有 UiBridge 历史代码会改写它们。
 */
int Runtime_ConfirmDialogProtocolOk(void) {
    static const RtCallCheck checks[] = {
        {CALL_CONFIRM2_HIT_YES, FN_BUTTON_HITTEST, "[询问框] 第二变体 Yes HitTest CALL 不一致。"},
        {CALL_CONFIRM2_HIT_NO, FN_BUTTON_HITTEST, "[询问框] 第二变体 No HitTest CALL 不一致。"},
        {CALL_CONFIRM2_EVENT_YES, FN_BUTTON_EVENT, "[询问框] 第二变体 Yes Event CALL 不一致。"},
        {CALL_CONFIRM2_EVENT_NO, FN_BUTTON_EVENT, "[询问框] 第二变体 No Event CALL 不一致。"}
    };
    SIZE_T i;

    if (*(u32*)VTABLE_CONFIRM1_UPDATE != FN_CONFIRM1_UPDATE) {
        Runtime_Log("[询问框] 第一种双按钮 vtable Update 槽不一致；历史双按钮候选层已单独禁用。");
        return 0;
    }
    if (*(u32*)VTABLE_CONFIRM2_UPDATE != FN_CONFIRM2_UPDATE) {
        Runtime_Log("[询问框] 第二种双按钮 vtable Update 槽不一致；历史双按钮候选层已单独禁用。");
        return 0;
    }

    for (i = 0; i < sizeof(checks) / sizeof(checks[0]); ++i) {
        if (!rt_check_call_item(&checks[i])) {
            Runtime_Log("[询问框] vtable/第二变体按钮协议未通过；历史双按钮候选层已单独禁用。");
            return 0;
        }
    }
    Runtime_Log("[询问框] 两种 vtable Update 槽 + 第二变体 4/4 CALL 校验通过。");
    return 1;
}

int Runtime_PatchIatPointer(u32 slot, void* replacement, void** original_out) {
    void** p = (void**)slot;
    DWORD old_protect = 0, ignored = 0;
    void* original;
    if (!g_api.virtual_protect || !replacement) return 0;
    original = *p;
    if (!Runtime_PtrOk(original)) return 0;
    if (!g_api.virtual_protect(p, 4u, PAGE_READWRITE_, &old_protect)) return 0;
    /* 先把 original 交给 Hook，再让 replacement 对游戏线程可见，避免半安装竞态。 */
    if (original_out) *original_out = original;
    *p = replacement;
    g_api.virtual_protect(p, 4u, old_protect, &ignored);
    return 1;
}

/* CALL patch 在写入前再次核对“原 CALL 目标==预期原函数”，即使启动预检之后内存被改也会 fail-closed。 */
int Runtime_PatchCall(u32 call_address, void* replacement, u32 expected_target) {
    u8* p = (u8*)call_address;
    i32 old_rel, new_rel;
    u32 old_target;
    DWORD old_protect = 0, ignored = 0;
    if (!g_api.virtual_protect || !replacement || p[0] != 0xE8u) return 0;
    old_rel = *(i32*)(p + 1);
    old_target = call_address + 5u + (u32)old_rel;
    if (old_target != expected_target) return 0;
    new_rel = (i32)((u32)replacement - (call_address + 5u));
    if (!g_api.virtual_protect(p, 5u, PAGE_READWRITE_, &old_protect)) return 0;
    *(i32*)(p + 1) = new_rel;
    g_api.virtual_protect(p, 5u, old_protect, &ignored);
    return 1;
}

/* 6 字节入口改成 E9 rel32 + NOP；只有原始 6 字节完全一致才允许写。 */
int Runtime_PatchJmp6(u32 address, void* replacement, const u8 expected[6]) {
    u8* p = (u8*)address;
    i32 rel;
    DWORD old_protect = 0, ignored = 0;
    if (!g_api.virtual_protect || !replacement || !expected) return 0;
    if (!Runtime_MemEq(p, expected, 6u)) return 0;
    rel = (i32)((u32)replacement - (address + 5u));
    if (!g_api.virtual_protect(p, 6u, PAGE_READWRITE_, &old_protect)) return 0;
    p[0] = 0xE9u;
    *(i32*)(p + 1) = rel;
    p[5] = 0x90u;
    g_api.virtual_protect(p, 6u, old_protect, &ignored);
    return 1;
}

/*
 * 把 6 字节“mov esi,[abs32]”局部装载改成“mov esi,imm32 + nop”。
 *
 * 为什么专门做一个函数而不是让业务模块自己 VirtualProtect：
 * - 所有机器码修改都必须集中在 Runtime；
 * - 写之前必须逐字节核对 expected，防止错误 EXE 或其它补丁已经改过该位置；
 * - VirtualProtect 的恢复也只写一份，减少忘记恢复页面权限的风险。
 */
int Runtime_PatchMovEsiFunction(u32 address, void* replacement, const u8 expected[6]) {
    u8* p = (u8*)address;
    DWORD old_protect = 0, ignored = 0;

    if (!g_api.virtual_protect || !replacement || !expected) return 0;
    if (!Runtime_MemEq(p, expected, 6u)) return 0;
    if (!g_api.virtual_protect(p, 6u, PAGE_READWRITE_, &old_protect)) return 0;

    /* BE imm32 = mov esi,imm32；第 6 字节补 NOP，保持后续原指令地址完全不移动。 */
    p[0] = 0xBEu;
    *(u32*)(p + 1) = (u32)replacement;
    p[5] = 0x90u;

    g_api.virtual_protect(p, 6u, old_protect, &ignored);
    return 1;
}

/* ------------------------- 生命周期 ------------------------- */

/* worker 的 Runtime 启动事务：读取配置、建立中文日志、解析 API、完整预检，全部通过后业务模块才可安装 Hook。 */
int Runtime_Initialize(HMODULE self_module) {
    char line[320];
    SIZE_T p = 0;
    g_self_module = self_module;
    Runtime_BindEarlyApi();
    rt_load_config();
    rt_open_log();

    Runtime_Log("[启动] 幽城幻剑录手柄支持：v0.3-refactor44（SaveAction原生disabled导航 + R43稳定基线）");
    Runtime_Log("[启动] By Luminou with ChatGPT");
    Runtime_LogModule("ASI 插件", g_self_module, NULL);

    if (!g_api.virtual_protect || !g_api.get_cursor_pos || !g_api.set_cursor_pos ||
        !g_api.mouse_event || !g_api.get_private_profile_int_a) {
        Runtime_Log("[致命] 必需的 Win32 API 解析失败，拒绝安装 Hook。");
        return 0;
    }

    if (!Runtime_ExactBuildOk()) {
        Runtime_Log("[致命] RPG.exe 精确版本/调用点预检失败，拒绝安装 Hook。");
        return 0;
    }

    rt_append_text(line, sizeof(line), &p, "[配置] 默认隐藏光标=");
    rt_append_text(line, sizeof(line), &p, g_cfg.cursor_default_hidden ? "1" : "0");
    rt_append_text(line, sizeof(line), &p, " 目标选择光标=");
    rt_append_text(line, sizeof(line), &p, g_cfg.target_cursor_indicator ? "1" : "0");
    rt_append_text(line, sizeof(line), &p, " 战斗快捷键=");
    rt_append_text(line, sizeof(line), &p, g_cfg.battle_shortcuts ? "1" : "0");
    rt_append_text(line, sizeof(line), &p, " 记住选择=");
    rt_append_text(line, sizeof(line), &p, g_cfg.battle_remember_selection ? "1" : "0");
    Runtime_Log(line);

    Runtime_Log("[预检] RPG.exe 运行时关键调用点与协议签名：通过。");
    return 1;
}

void Runtime_Shutdown(void) {
    if (g_log != INVALID_HANDLE_VALUE_ && g_api.close_handle) {
        Runtime_Log("[退出] ASI 正在卸载，关闭日志。");
        g_api.close_handle(g_log);
        g_log = INVALID_HANDLE_VALUE_;
    }
}
