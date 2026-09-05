#include "platform.h"
#include "pe_import_injector.h"
#include "launcher_app.h"
#include "launcher_gui.h"

/*
 * CastleModLoader.exe —— 《幽城幻剑录》外置 Pre-Loader / Mod Loader 启动器。
 *
 * 用户最终只需要双击这个 EXE。内部流程刻意对齐 Locale Emulator 的“启动器先运行，
 * 目标游戏由启动器创建并在正常执行以前完成 Locale 核心布置”的成熟模型：
 *
 *   CastleModLoader.exe
 *       ↓
 *   定位同目录 RPG.exe，并准备唯一 mods 目录
 *       ↓
 *   CREATE_SUSPENDED 创建 RPG.exe
 *       ↓
 *   只修改“目标进程内存中的 PE Import Directory”
 *   把 CastleLocaleBootstrap.dll 与 CastleModCore.dll 依次加到 RPG.exe 启动导入图最前面
 *       ↓
 *   ResumeThread
 *       ↓
 *   Windows Loader 先加载只依赖 ntdll 的 Locale Bootstrap，再加载普通 Mod Core；
 *   Bootstrap 只建立 CP950 NLS 表，Core 再建立 Overrides 与其余台湾 Locale Win32 层
 *       ↓
 *   Windows Loader 继续按游戏原本的规则初始化 ddraw、Bink、BASS、ijl10 等其余静态依赖
 *       ↓
 *   RPG.exe EntryPoint 前只做最终补挂、恢复原入口并加载 ASI
 *       ↓
 *   RPG.exe 开始正常业务代码
 *
 * 重要边界：
 *   - 硬盘上的 RPG.exe 不修改；
 *   - 不需要 winmm/bink/ddraw 代理来“承载 Mod Loader”；
 *   - CastleLocaleBootstrap.dll 与 CastleModCore.dll 是 Loader 自身组件，为便于卸载固定放在 mods\ 根目录；
 *   - mods\ 根目录只允许 Loader 自身两个内部 DLL、Loader/Mod 配置与日志，用户 Mod 不直接放这里；
 *   - 用户 Mod 只进入 mods\asi 与 mods\overrides；ASI 的依赖 DLL 跟 ASI 一起放在 mods\asi；
 *   - ddraw.dll 属于第三方图形兼容环境，若用户使用它，就继续按该兼容层原本要求放在 RPG.exe 同目录；Loader 不移动、不选择、不卸载它；
 *   - 但 dev9 作为“dev5 运行时回归恢复验证版”，会恢复 dev5 已实机成功的 Win32 IAT 兼容桥目标集合，其中包含已经加载的 ddraw.dll。
 *     这不等于 Hook DirectDraw API；目的是先精确恢复 dev5 的已知成功环境，再依据实机结果继续二分。
 */

#define LOG_CAP_ 4096u

typedef int (WINAPI *PFN_MessageBoxW_)(HWND, LPCWSTR, LPCWSTR, UINT);

static WCHAR g_launcher_dir[CASTLE_PATH_CAP];
static WCHAR g_mods[CASTLE_PATH_CAP];
static WCHAR g_log_path[CASTLE_PATH_CAP];
static WCHAR g_loader_ini[CASTLE_PATH_CAP];
static HANDLE g_log = INVALID_HANDLE_VALUE_;
static char g_utf8[LOG_CAP_];

/*
 * CastleModLoader.ini 是“Loader 自己的配置”，和 mods.ini 的职责严格分开：
 *   - CastleModLoader.ini：控制 Loader/审计器自己的行为；
 *   - mods.ini：只控制 ASI 和 Overrides 的启用状态与顺序。
 *
 * 这两个开关默认都为 1。配置文件不存在时 Launcher 会自动生成一份 UTF-8 + CRLF 模板。
 * 之所以在 Launcher 这里最早读取，是因为 modloader.log 是否应该创建，必须在“打开日志文件以前”就知道。
 */
static int g_modloader_log_enabled = 1;
static int g_game_log_enabled = 1;
static int g_loader_ini_had_invalid_value;
static BYTE g_loader_ini_bytes[16384u];

/* ---------- 极小字符串/路径函数：不依赖 CRT，避免额外运行库 ---------- */

static UINT wlen_(const WCHAR* s) {
    UINT n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}

static WCHAR fold_(WCHAR c) {
    if (c >= (WCHAR)'A' && c <= (WCHAR)'Z') return (WCHAR)(c + ((WCHAR)'a' - (WCHAR)'A'));
    return c;
}

static int weq_i_(const WCHAR* a, const WCHAR* b) {
    UINT i = 0;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        if (fold_(a[i]) != fold_(b[i])) return 0;
        ++i;
    }
    return a[i] == 0 && b[i] == 0;
}

static int wcopy_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT i = 0;
    if (!dst || !src || cap == 0u) return 0;
    while (src[i]) {
        if (i + 1u >= cap) return 0;
        dst[i] = src[i];
        ++i;
    }
    dst[i] = 0;
    return 1;
}

static int wappend_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT d = wlen_(dst), i = 0;
    if (!dst || !src || d >= cap) return 0;
    while (src[i]) {
        if (d + i + 1u >= cap) return 0;
        dst[d + i] = src[i];
        ++i;
    }
    dst[d + i] = 0;
    return 1;
}

static int path_join_(WCHAR* out, UINT cap, const WCHAR* left, const WCHAR* right) {
    UINT n;
    if (!wcopy_(out, cap, left)) return 0;
    n = wlen_(out);
    if (n && out[n - 1u] != (WCHAR)'\\' && out[n - 1u] != (WCHAR)'/') {
        if (!wappend_(out, cap, (const WCHAR*)L"\\")) return 0;
    }
    return wappend_(out, cap, right);
}

static int parent_dir_(WCHAR* out, UINT cap, const WCHAR* path) {
    UINT n;
    if (!wcopy_(out, cap, path)) return 0;
    n = wlen_(out);
    while (n) {
        if (out[n - 1u] == (WCHAR)'\\' || out[n - 1u] == (WCHAR)'/') {
            out[n - 1u] = 0;
            return out[0] != 0;
        }
        --n;
    }
    return 0;
}

static const WCHAR* basename_(const WCHAR* path) {
    UINT i, last = 0;
    for (i = 0; path && path[i]; ++i)
        if (path[i] == (WCHAR)'\\' || path[i] == (WCHAR)'/') last = i + 1u;
    return path ? path + last : path;
}


static int file_exists_(const WCHAR* path) {
    DWORD a = GetFileAttributesW(path);
    return a != INVALID_FILE_ATTRIBUTES_ && !(a & FILE_ATTRIBUTE_DIRECTORY_);
}

static void ensure_dir_(const WCHAR* path) {
    if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES_)
        CreateDirectoryW(path, NULL_PTR);
}

/* ---------- Loader 自己的 CastleModLoader.ini ---------- */

/*
 * 配置键全部使用英文 ASCII，既方便以后保持稳定，也避免“为了读取配置本身还要先决定代码页”的循环依赖。
 * 中文只出现在以 ';' 开头的注释行里；解析器看到注释起始符后直接忽略整行，因此不会逐字节处理中文。
 */
static char lower_ascii_(char c) {
    if (c >= 'A' && c <= 'Z') return (char)(c + ('a' - 'A'));
    return c;
}

static int ascii_span_eq_i_(const char* begin, const char* end, const char* literal) {
    UINT i = 0u;
    if (!begin || !end || !literal || end < begin) return 0;
    while (begin + i < end && literal[i]) {
        if (lower_ascii_(begin[i]) != lower_ascii_(literal[i])) return 0;
        ++i;
    }
    return begin + i == end && literal[i] == 0;
}

static const char* trim_ascii_left_(const char* begin, const char* end) {
    while (begin < end && (*begin == ' ' || *begin == '	')) ++begin;
    return begin;
}

static const char* trim_ascii_right_(const char* begin, const char* end) {
    while (end > begin && (end[-1] == ' ' || end[-1] == '	')) --end;
    return end;
}

/*
 * 只接受清晰的布尔值。1/0 是文档中的正式写法；on/off、true/false、yes/no 只是为了容错。
 * 返回 1 表示“成功识别”，并把结果写到 value_out；返回 0 表示该值不合法，调用者保持默认值。
 */
static int parse_bool_ascii_(const char* begin, const char* end, int* value_out) {
    begin = trim_ascii_left_(begin, end);
    end = trim_ascii_right_(begin, end);
    if (!value_out) return 0;
    if (ascii_span_eq_i_(begin, end, "1") || ascii_span_eq_i_(begin, end, "on") ||
        ascii_span_eq_i_(begin, end, "true") || ascii_span_eq_i_(begin, end, "yes")) {
        *value_out = 1;
        return 1;
    }
    if (ascii_span_eq_i_(begin, end, "0") || ascii_span_eq_i_(begin, end, "off") ||
        ascii_span_eq_i_(begin, end, "false") || ascii_span_eq_i_(begin, end, "no")) {
        *value_out = 0;
        return 1;
    }
    return 0;
}

static int write_utf8_config_line_(HANDLE file, const WCHAR* line) {
    int bytes;
    DWORD wrote = 0u;
    if (!file || file == INVALID_HANDLE_VALUE_ || !line) return 0;
    bytes = WideCharToMultiByte(CP_UTF8_, 0u, line, -1, g_utf8, (int)(LOG_CAP_ - 3u), NULL_PTR, NULL_PTR);
    if (bytes <= 0) return 0;
    if (bytes > 1 && (!WriteFile(file, g_utf8, (DWORD)(bytes - 1), &wrote, NULL_PTR) || wrote != (DWORD)(bytes - 1))) return 0;
    if (!WriteFile(file, "\r\n", 2u, &wrote, NULL_PTR) || wrote != 2u) return 0;
    return 1;
}

static int create_default_loader_ini_(void) {
    HANDLE file;
    DWORD wrote = 0u;
    static const BYTE bom[3] = {0xEFu, 0xBBu, 0xBFu};

    file = CreateFileW(g_loader_ini, GENERIC_WRITE_, FILE_SHARE_READ_, NULL_PTR,
                       CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) return 0;

    if (!WriteFile(file, bom, 3u, &wrote, NULL_PTR) || wrote != 3u ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; 《幽城幻剑录》Mod Loader 自身配置") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; 这个文件只控制 Loader 自己；ASI/Overrides 的顺序仍只由 mods.ini 管理。") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; 1=启用，0=禁用。修改后重新启动游戏生效。") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"[Logging]") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; Mod Loader / Mod 初始化日志：mods\\modloader.log") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"ModLoaderLog=1") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; 原版游戏 I/O / 生命周期 / 异常审计日志：mods\\game.log") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"GameLog=1")) {
        CloseHandle(file);
        return 0;
    }
    CloseHandle(file);
    return 1;
}

static int read_loader_ini_(void) {
    HANDLE file;
    DWORD size, got = 0u, pos = 0u;
    int in_logging = 0;

    g_modloader_log_enabled = 1;
    g_game_log_enabled = 1;
    g_loader_ini_had_invalid_value = 0;

    if (!file_exists_(g_loader_ini)) {
        if (!create_default_loader_ini_()) return 0;
    }

    file = CreateFileW(g_loader_ini, GENERIC_READ_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_ | FILE_SHARE_DELETE_,
                       NULL_PTR, OPEN_EXISTING_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) return 0;
    size = GetFileSize(file, NULL_PTR);
    if (size == 0u) {
        CloseHandle(file);
        if (!create_default_loader_ini_()) return 0;
        file = CreateFileW(g_loader_ini, GENERIC_READ_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_ | FILE_SHARE_DELETE_,
                           NULL_PTR, OPEN_EXISTING_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
        if (file == INVALID_HANDLE_VALUE_) return 0;
        size = GetFileSize(file, NULL_PTR);
    }
    if (size == 0xFFFFFFFFu || size >= (DWORD)sizeof(g_loader_ini_bytes)) {
        CloseHandle(file);
        return 0;
    }
    if (size && (!ReadFile(file, g_loader_ini_bytes, size, &got, NULL_PTR) || got != size)) {
        CloseHandle(file);
        return 0;
    }
    CloseHandle(file);

    /* UTF-8 BOM 是三字节 EF BB BF。没有 BOM 也照样能读，因为所有正式键名都是 ASCII。 */
    if (size >= 3u && g_loader_ini_bytes[0] == 0xEFu && g_loader_ini_bytes[1] == 0xBBu && g_loader_ini_bytes[2] == 0xBFu)
        pos = 3u;

    while (pos < size) {
        DWORD line_start = pos, line_end, i;
        const char* begin;
        const char* end;
        const char* eq = NULL_PTR;

        while (pos < size && g_loader_ini_bytes[pos] != '\r' && g_loader_ini_bytes[pos] != '\n') ++pos;
        line_end = pos;
        if (pos < size && g_loader_ini_bytes[pos] == '\r') ++pos;
        if (pos < size && g_loader_ini_bytes[pos] == '\n') ++pos;

        begin = (const char*)g_loader_ini_bytes + line_start;
        end = (const char*)g_loader_ini_bytes + line_end;
        begin = trim_ascii_left_(begin, end);
        end = trim_ascii_right_(begin, end);
        if (begin >= end || *begin == ';' || *begin == '#') continue;

        if (*begin == '[') {
            in_logging = ascii_span_eq_i_(begin, end, "[Logging]");
            continue;
        }
        if (!in_logging) continue;

        for (i = line_start; i < line_end; ++i) {
            if (g_loader_ini_bytes[i] == '=') { eq = (const char*)g_loader_ini_bytes + i; break; }
        }
        if (!eq || eq < begin || eq >= end) continue;

        {
            const char* key_end = trim_ascii_right_(begin, eq);
            const char* value_begin = trim_ascii_left_(eq + 1, end);
            int parsed;
            if (ascii_span_eq_i_(begin, key_end, "ModLoaderLog")) {
                if (!parse_bool_ascii_(value_begin, end, &parsed)) g_loader_ini_had_invalid_value = 1;
                else g_modloader_log_enabled = parsed;
            } else if (ascii_span_eq_i_(begin, key_end, "GameLog")) {
                if (!parse_bool_ascii_(value_begin, end, &parsed)) g_loader_ini_had_invalid_value = 1;
                else g_game_log_enabled = parsed;
            }
        }
    }
    return 1;
}

/* ---------- 中文 UTF-8 日志 ---------- */

static void log_line_(const WCHAR* line) {
    int n;
    DWORD wrote = 0;
    if (g_log == INVALID_HANDLE_VALUE_ || !line) return;
    n = WideCharToMultiByte(CP_UTF8_, 0, line, -1, g_utf8, (int)(LOG_CAP_ - 3u), NULL_PTR, NULL_PTR);
    if (n <= 0) return;
    if (n > 1) WriteFile(g_log, g_utf8, (DWORD)(n - 1), &wrote, NULL_PTR);
    WriteFile(g_log, "\r\n", 2u, &wrote, NULL_PTR);
}

static void log_two_(const WCHAR* a, const WCHAR* b) {
    WCHAR line[CASTLE_PATH_CAP];
    line[0] = 0;
    if (!wcopy_(line, CASTLE_PATH_CAP, a)) return;
    if (!wappend_(line, CASTLE_PATH_CAP, b ? b : (const WCHAR*)L"")) return;
    log_line_(line);
}

static void show_error_(const WCHAR* text) {
    HMODULE user32 = LoadLibraryW((const WCHAR*)L"user32.dll");
    if (user32) {
        PFN_MessageBoxW_ fn = (PFN_MessageBoxW_)GetProcAddress(user32, "MessageBoxW");
        if (fn) fn(NULL_PTR, text, (const WCHAR*)L"《幽城幻剑录》Mod Loader", MB_OK_ | MB_ICONERROR_);
        FreeLibrary(user32);
    }
}

static int initialize_paths_and_config_(void) {
    WCHAR module_path[CASTLE_PATH_CAP];
    WCHAR log_directory[CASTLE_PATH_CAP];

    /*
     * GUI 时代仍然沿用 v0.2.1 已经实机验证过的相对路径规则。
     * 唯一一次读取完整物理路径，只为了把当前工作目录切到 CastleModLoader.exe 所在目录；
     * 之后 RPG.exe、mods、配置、日志全部继续使用相对路径。
     */
    if (!GetModuleFileNameW(NULL_PTR, module_path, CASTLE_PATH_CAP)) return 0;
    if (!parent_dir_(g_launcher_dir, CASTLE_PATH_CAP, module_path)) return 0;
    if (!SetCurrentDirectoryW(g_launcher_dir)) return 0;

    if (!wcopy_(g_mods, CASTLE_PATH_CAP, (const WCHAR*)L"mods")) return 0;
    ensure_dir_(g_mods);
    if (!path_join_(log_directory, CASTLE_PATH_CAP, g_mods,
                    (const WCHAR*)L"logs")) return 0;
    ensure_dir_(log_directory);
    if (!path_join_(g_loader_ini, CASTLE_PATH_CAP, g_mods, (const WCHAR*)L"CastleModLoader.ini")) return 0;
    if (!read_loader_ini_()) {
        show_error_((const WCHAR*)L"无法读取或生成 mods\\CastleModLoader.ini。请检查文件是否被占用、只读或尺寸异常。");
        return 0;
    }
    if (!path_join_(g_log_path, CASTLE_PATH_CAP, g_mods,
                    (const WCHAR*)L"logs\\modloader.log")) return 0;

    /*
     * 关键 GUI 行为：仅打开管理界面绝不能清空上一轮 modloader.log。
     * 真正 CREATE_ALWAYS 打开日志延迟到用户点击“启动游戏”的那一刻。
     */
    g_log = INVALID_HANDLE_VALUE_;
    return 1;
}

static int open_log_for_launch_(void) {
    static const BYTE bom[3] = {0xEFu, 0xBBu, 0xBFu};
    DWORD wrote = 0u;

    if (g_log != INVALID_HANDLE_VALUE_) {
        CloseHandle(g_log);
        g_log = INVALID_HANDLE_VALUE_;
    }
    if (!g_modloader_log_enabled) return 1;

    g_log = CreateFileW(g_log_path, GENERIC_WRITE_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_, NULL_PTR,
                        CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (g_log == INVALID_HANDLE_VALUE_) return 0;
    WriteFile(g_log, bom, 3u, &wrote, NULL_PTR);
    log_line_((const WCHAR*)L"《幽城幻剑录》Mod Loader Pre-Loader v0.3.0-dev9 启动。旧日志已清空。");
    log_two_((const WCHAR*)L"[Loader配置] 配置文件：", g_loader_ini);
    log_line_(g_game_log_enabled ?
              (const WCHAR*)L"[Loader配置] GameLog=1：本轮启用 mods\\logs\\game.log 原版游戏审计。" :
              (const WCHAR*)L"[Loader配置] GameLog=0：本轮不建立 mods\\logs\\game.log，也不安装仅用于游戏审计的 I/O/状态 Hook。");
    if (g_loader_ini_had_invalid_value)
        log_line_((const WCHAR*)L"[Loader配置警告] CastleModLoader.ini 存在无法识别的日志开关值；该项已保持安全默认值 1。正式值请使用 1 或 0。");
    return 1;
}

/* ---------- 固定同目录游戏路径：不再需要额外 Launcher 配置 ---------- */

static int resolve_game_path_(WCHAR* game_exe, WCHAR* game_dir, WCHAR* game_root) {
    /*
     * 这里故意不再生成 RPG.exe 的绝对路径。
     * initialize_paths_and_config_() 已经把当前工作目录固定到 Launcher 所在目录，因此：
     *   RPG.exe = 同目录游戏 EXE
     *   .       = RPG.exe 所在目录
     *   ..      = 当目录名为 exe 时的游戏安装根
     *
     * v0.2 的实机失败正是因为“完整路径”和“相对路径”混用：
     * 完整 F:\...\RPG.exe 又被重复拼进 F:\... 根目录，最后当然判定文件不存在。
     * v0.2.1 从这里开始彻底禁止这种二次拼接。
     */
    if (!wcopy_(game_exe, CASTLE_PATH_CAP, (const WCHAR*)L"RPG.exe")) return 0;
    if (!file_exists_(game_exe)) {
        log_line_((const WCHAR*)L"[停止] 当前目录找不到 RPG.exe。");
        show_error_((const WCHAR*)L"当前目录找不到 RPG.exe。请把 CastleModLoader.exe 和 mods 文件夹一起放到 RPG.exe 同目录，然后重新启动。");
        return 0;
    }

    if (!wcopy_(game_dir, CASTLE_PATH_CAP, (const WCHAR*)L".")) return 0;

    /*
     * 台湾版标准布局是 <游戏根>\exe\RPG.exe。
     * 判断只看 Launcher 物理目录最后一级名称，不拿这个绝对目录继续参与任何文件打开。
     * 如果最后一级不是 exe，就把当前目录本身视为游戏根，方便其它整理方式继续工作。
     */
    if (weq_i_(basename_(g_launcher_dir), (const WCHAR*)L"exe")) {
        if (!wcopy_(game_root, CASTLE_PATH_CAP, (const WCHAR*)L"..")) return 0;
    } else {
        if (!wcopy_(game_root, CASTLE_PATH_CAP, (const WCHAR*)L".")) return 0;
    }
    return 1;
}

/* ---------- LEProc 同型：创建挂起目标 → 加入 Core 静态导入 → 恢复 ---------- */

static int create_and_prepare_game_(const WCHAR* game_exe, const WCHAR* game_dir, const WCHAR* game_root) {
    STARTUPINFOW_ si;
    PROCESS_INFORMATION_ pi;
    WCHAR cmdline[CASTLE_PATH_CAP * 2u];
    WCHAR bootstrap_path[CASTLE_PATH_CAP];
    WCHAR core_path[CASTLE_PATH_CAP];
    DWORD resume_result;

    /*
     * 这两个 DLL 是 Mod Loader 自己的内部组件，不是用户 Mod，但发布布局仍把它们放在 mods\ 根目录。
     * 这样卸载时只要删除 CastleModLoader.exe 和整个 mods\ 目录，就不会在游戏目录遗留 Loader DLL。
     *
     * 当前仓库为了让内存 Import Directory 与发布目录一一对应，Early Import 名直接写成游戏根目录下的
     * mods 相对路径：
     *
     *     mods\CastleLocaleBootstrap.dll
     *     mods\CastleModCore.dll
     *
     * 在 ResumeThread 以前，Launcher 已经用 SetDllDirectoryW(mods) 建立子进程会继承的启动搜索目录，
     * 同时这里仍先用完整路径做存在性检查。显式 mods 相对路径避免只看文件名时误中游戏根目录里的
     * 同名旧 DLL；SetDllDirectoryW 则继续保留 dev5 已实机验证的其它依赖搜索兼容环境。
     */
    if (!path_join_(bootstrap_path, CASTLE_PATH_CAP, g_mods, (const WCHAR*)L"CastleLocaleBootstrap.dll")) return 0;
    if (!file_exists_(bootstrap_path)) {
        log_two_((const WCHAR*)L"[启动失败] 缺少早期区域环境核心：", bootstrap_path);
        show_error_((const WCHAR*)L"缺少 mods\\CastleLocaleBootstrap.dll；请重新复制完整 Mod Loader 文件。");
        return 0;
    }
    if (!path_join_(core_path, CASTLE_PATH_CAP, g_mods, (const WCHAR*)L"CastleModCore.dll")) return 0;
    if (!file_exists_(core_path)) {
        log_two_((const WCHAR*)L"[启动失败] 缺少运行时核心：", core_path);
        show_error_((const WCHAR*)L"缺少 mods\\CastleModCore.dll；请重新复制完整 Mod Loader 文件。");
        return 0;
    }

    /*
     * Core 不再猜任何目录。Launcher 在创建子进程以前把三类根目录写进环境，RPG.exe 会完整继承。
     * Launcher 会把唯一 mods\、游戏 EXE 目录和游戏安装根写入子进程环境，Core 不需要从自身 DLL 路径猜位置。
     */
    SetEnvironmentVariableW((const WCHAR*)L"CASTLE_MOD_ROOT", g_mods);
    SetEnvironmentVariableW((const WCHAR*)L"CASTLE_GAME_EXE_DIR", game_dir);
    SetEnvironmentVariableW((const WCHAR*)L"CASTLE_GAME_ROOT", game_root);
    /*
     * Core 在 RPG.exe 内不重新猜配置。Launcher 把本轮已经解析好的两个日志开关通过环境传进去，
     * 这样“是否建立日志/审计 Hook”的决定从父进程到子进程始终一致。
     */
    SetEnvironmentVariableW((const WCHAR*)L"CASTLE_MODLOADER_LOG_ENABLED", g_modloader_log_enabled ? (const WCHAR*)L"1" : (const WCHAR*)L"0");
    SetEnvironmentVariableW((const WCHAR*)L"CASTLE_GAME_LOG_ENABLED", g_game_log_enabled ? (const WCHAR*)L"1" : (const WCHAR*)L"0");
    SetEnvironmentVariableW((const WCHAR*)L"CASTLE_PRELOADER_LOG_READY", g_modloader_log_enabled ? (const WCHAR*)L"1" : (const WCHAR*)L"0");

    /*
     * dev9 的最高优先级不是继续“理论优化”，而是先恢复用户已经实机证明成功的 dev5 启动环境。
     * dev5 在 CreateProcessW 以前把 mods\ 设为附加 DLL 搜索目录，子进程会继承这项启动期搜索环境；
     * dev6 删除这一步以后，用户同时观察到“台湾繁中转区失效 + cnc-ddraw 截图失效”。
     *
     * 因此本验证版恢复 dev5 的这一步，但仍严格保持两个边界：
     *   1. mods\ 根目录只允许 Loader 自身 DLL、配置和日志，用户 Mod 仍只进入 asi/overrides；
     *   2. Launcher 不探测、不复制、不选择 mods\ddraw.dll，第三方 ddraw 仍按组件自己的要求放 RPG.exe 同目录。
     *
     * 这里的 SetDllDirectoryW 只是恢复“启动期 DLL 搜索环境”这个实测变量。CreateProcessW 返回以后，
     * Launcher 立刻把自己的搜索目录恢复为默认；RPG.exe 内部随后由 Core 再恢复 dev5 的长期 mods 搜索环境。
     */
    if (!SetDllDirectoryW(g_mods)) {
        log_line_((const WCHAR*)L"[启动失败] 无法建立 dev5 兼容的 mods DLL 搜索环境。");
        return 0;
    }

    si.cb = (DWORD)sizeof(si);
    si.lpReserved = NULL_PTR; si.lpDesktop = NULL_PTR; si.lpTitle = NULL_PTR;
    si.dwX = si.dwY = si.dwXSize = si.dwYSize = si.dwXCountChars = si.dwYCountChars = 0u;
    si.dwFillAttribute = si.dwFlags = 0u; si.wShowWindow = si.cbReserved2 = 0u;
    si.lpReserved2 = NULL_PTR; si.hStdInput = si.hStdOutput = si.hStdError = NULL_PTR;

    cmdline[0] = 0;
    wcopy_(cmdline, CASTLE_PATH_CAP * 2u, (const WCHAR*)L"\"");
    wappend_(cmdline, CASTLE_PATH_CAP * 2u, game_exe);
    wappend_(cmdline, CASTLE_PATH_CAP * 2u, (const WCHAR*)L"\"");

    /*
     * 这里强制 CREATE_SUSPENDED，和 LE 的 LeCreateProcess2 同一设计原则：
     * 先让 Windows 建好目标进程映像，但不给初始线程机会执行用户代码；Launcher 完成装载图修改后再恢复。
     */
    /*
     * lpCurrentDirectory 传 NULL：Windows 会让子进程继承 Launcher 当前工作目录。
     * 由于我们已经在最开头 SetCurrentDirectoryW 到 Launcher 所在目录，这里不需要再提供绝对 cwd。
     */
    if (!CreateProcessW(game_exe, cmdline, NULL_PTR, NULL_PTR, FALSE_,
                        CREATE_SUSPENDED_ | CREATE_UNICODE_ENVIRONMENT_ | CREATE_NEW_PROCESS_GROUP_,
                        NULL_PTR, NULL_PTR, &si, &pi)) {
        /*
         * SetDllDirectoryW 是进程级状态。CreateProcessW 失败时也必须恢复，
         * 否则 Launcher 后续打开设置窗口或再次尝试启动时会继续带着 mods 搜索目录。
         */
        SetDllDirectoryW(NULL_PTR);
        log_line_((const WCHAR*)L"[启动失败] CreateProcessW(CREATE_SUSPENDED) 无法创建 RPG.exe。");
        return 0;
    }

    /*
     * 子进程已经在创建瞬间继承了 dev5 的启动期 DLL 搜索环境。
     * 父进程从这里开始不再需要 mods 搜索目录，所以立即恢复默认，避免污染 Launcher 自己。
     */
    SetDllDirectoryW(NULL_PTR);

    log_two_((const WCHAR*)L"[启动] 已挂起创建：", game_exe);

    /*
     * 不使用远程线程、不等游戏 EntryPoint、不碰磁盘 EXE。
     * 一次性加入两个早期导入：Locale Bootstrap 必须在前，Mod Core 紧随其后。
     * Bootstrap 不依赖 KERNEL32，用来承担 v0.2.3 证明不能塞进普通 Core DllMain 的 CP950 NLS 重建。
     */
    if (!PeImportInjector_AddEarlyImports2(pi.hProcess,
            "mods\\CastleLocaleBootstrap.dll", "CastleLocaleBootstrap_Bootstrap",
            "mods\\CastleModCore.dll", "CastleModCore_Bootstrap")) {
        log_line_((const WCHAR*)L"[启动失败] 无法把 Locale Bootstrap + Mod Core 一起加入 RPG.exe 的内存导入目录。目标进程已终止。");
        TerminateProcess(pi.hProcess, 2u);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return 0;
    }
    log_line_((const WCHAR*)L"[Pre-Loader] CastleLocaleBootstrap.dll 已排在启动装载图最前；CastleModCore.dll 紧随其后；两者通过显式 mods 相对路径加入启动装载图；磁盘 RPG.exe 未修改。");

    /*
     * 这是 Launcher 对 modloader.log 的最后一条写入。必须放在 ResumeThread 之前：
     * 一旦恢复目标线程，CastleModCore.dll 就可能立刻打开同一日志并追加；
     * 如果 Launcher 还继续写，就会出现两个独立文件指针抢同一个 EOF 的竞态。
     */
    log_line_((const WCHAR*)L"[Launcher完成] 所有外部准备已完成，下一步 ResumeThread 放行 RPG.exe。后续日志由 CastleModCore.dll 接管。");

    resume_result = ResumeThread(pi.hThread);
    if (resume_result == 0xFFFFFFFFu) {
        /* Resume 失败时目标根本不会运行 Core，所以此处仍可安全写 Launcher 日志。 */
        log_line_((const WCHAR*)L"[启动失败] ResumeThread 失败。目标进程已终止。");
        TerminateProcess(pi.hProcess, 3u);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return 0;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 1;
}

const WCHAR* LauncherApp_GetModsRoot(void) {
    return g_mods;
}

const WCHAR* LauncherApp_GetLoaderIniPath(void) {
    return g_loader_ini;
}

int LauncherApp_GetModLoaderLogEnabled(void) {
    return g_modloader_log_enabled;
}

int LauncherApp_GetGameLogEnabled(void) {
    return g_game_log_enabled;
}

int LauncherApp_SaveLoggingSettings(int modloader_log_enabled, int game_log_enabled) {
    HANDLE file;
    DWORD wrote = 0u;
    WCHAR temp_path[CASTLE_PATH_CAP];
    HMODULE kernel;
    typedef BOOL (WINAPI *PFN_MoveFileExW_)(LPCWSTR, LPCWSTR, DWORD);
    typedef BOOL (WINAPI *PFN_DeleteFileW_)(LPCWSTR);
    PFN_MoveFileExW_ move_file_ex;
    PFN_DeleteFileW_ delete_file;
    static const BYTE bom[3] = {0xEFu, 0xBBu, 0xBFu};

    /*
     * CastleModLoader.ini 是 Loader 自己拥有的小配置，目前只有两个正式键。
     * GUI 保存时直接输出规范模板，不经过 Win32 Profile API：Profile API 对 UTF-8 BOM 老 INI 的编码行为不稳定，
     * 有机会把中文注释按宿主 ANSI 代码页重写。
     *
     * 即便只是这两个开关，也继续遵守 v0.2.11 已经确认的“先写临时文件，再原子替换”原则：
     * 任何磁盘写入失败都不能先把用户原配置截断。
     */
    if (!path_join_(temp_path, CASTLE_PATH_CAP, g_mods, (const WCHAR*)L"CastleModLoader.ini.castle.tmp")) return 0;
    kernel = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    move_file_ex = kernel ? (PFN_MoveFileExW_)GetProcAddress(kernel, "MoveFileExW") : NULL_PTR;
    delete_file = kernel ? (PFN_DeleteFileW_)GetProcAddress(kernel, "DeleteFileW") : NULL_PTR;
    if (!move_file_ex) return 0;
    if (delete_file) delete_file(temp_path);

    file = CreateFileW(temp_path, GENERIC_WRITE_, FILE_SHARE_READ_, NULL_PTR,
                       CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) return 0;
    if (!WriteFile(file, bom, 3u, &wrote, NULL_PTR) || wrote != 3u ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; 《幽城幻剑录》Mod Loader 自身配置") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; 这个文件只控制 Loader 自己；ASI/Overrides 的顺序仍只由 mods.ini 管理。") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; GUI 中“设置”窗口保存后会把本文件整理成这份标准格式。") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"[Logging]") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; Mod Loader / Mod 初始化日志：mods\\modloader.log") ||
        !write_utf8_config_line_(file, modloader_log_enabled ? (const WCHAR*)L"ModLoaderLog=1" : (const WCHAR*)L"ModLoaderLog=0") ||
        !write_utf8_config_line_(file, (const WCHAR*)L"; 原版游戏 I/O / 生命周期 / 异常审计日志：mods\\game.log") ||
        !write_utf8_config_line_(file, game_log_enabled ? (const WCHAR*)L"GameLog=1" : (const WCHAR*)L"GameLog=0") ||
        !FlushFileBuffers(file)) {
        CloseHandle(file);
        if (delete_file) delete_file(temp_path);
        return 0;
    }
    CloseHandle(file);

    if (!move_file_ex(temp_path, g_loader_ini, 0x00000001u | 0x00000008u)) {
        if (delete_file) delete_file(temp_path);
        return 0;
    }

    g_modloader_log_enabled = modloader_log_enabled ? 1 : 0;
    g_game_log_enabled = game_log_enabled ? 1 : 0;
    g_loader_ini_had_invalid_value = 0;
    return 1;
}

int LauncherApp_StartCncConfig(void) {
    WCHAR exe_path[CASTLE_PATH_CAP];
    WCHAR cmdline[CASTLE_PATH_CAP * 2u];
    STARTUPINFOW_ si;
    PROCESS_INFORMATION_ pi;

    /*
     * cnc-ddraw 配置程序固定与 CastleModLoader.exe 同目录。
     * 使用 Launcher 的物理目录构造完整路径，不依赖调用时的当前目录状态。
     */
    if (!path_join_(exe_path,
                    CASTLE_PATH_CAP,
                    g_launcher_dir,
                    (const WCHAR*)L"cnc-ddraw config.exe")) {
        return 0;
    }

    if (!file_exists_(exe_path)) {
        return 0;
    }

    /*
     * CreateProcessW 的命令行缓冲区必须可写。
     * 文件名包含空格，因此明确加双引号。
     */
    cmdline[0] = 0;
    if (!wcopy_(cmdline,
                CASTLE_PATH_CAP * 2u,
                (const WCHAR*)L"\"")) {
        return 0;
    }
    if (!wappend_(cmdline,
                  CASTLE_PATH_CAP * 2u,
                  exe_path)) {
        return 0;
    }
    if (!wappend_(cmdline,
                  CASTLE_PATH_CAP * 2u,
                  (const WCHAR*)L"\"")) {
        return 0;
    }

    /*
     * 与现有 create_and_prepare_game_() 一样，
     * 手工初始化自定义 STARTUPINFOW_，不依赖 CRT memset。
     */
    si.cb = (DWORD)sizeof(si);
    si.lpReserved = NULL_PTR;
    si.lpDesktop = NULL_PTR;
    si.lpTitle = NULL_PTR;
    si.dwX = 0u;
    si.dwY = 0u;
    si.dwXSize = 0u;
    si.dwYSize = 0u;
    si.dwXCountChars = 0u;
    si.dwYCountChars = 0u;
    si.dwFillAttribute = 0u;
    si.dwFlags = 0u;
    si.wShowWindow = 0u;
    si.cbReserved2 = 0u;
    si.lpReserved2 = NULL_PTR;
    si.hStdInput = NULL_PTR;
    si.hStdOutput = NULL_PTR;
    si.hStdError = NULL_PTR;

    pi.hProcess = NULL_PTR;
    pi.hThread = NULL_PTR;
    pi.dwProcessId = 0u;
    pi.dwThreadId = 0u;

    /*
     * 普通启动辅助程序：
     * - 不 CREATE_SUSPENDED
     * - 不走 Mod Loader Pre-Loader
     * - 不注入任何 DLL
     * - 不等待配置程序退出
     * - 不关闭 CastleModLoader
     */
    if (!CreateProcessW(
            exe_path,
            cmdline,
            NULL_PTR,
            NULL_PTR,
            FALSE_,
            CREATE_UNICODE_ENVIRONMENT_,
            NULL_PTR,
            g_launcher_dir,
            &si,
            &pi)) {
        return 0;
    }

    /*
     * 这里只释放 Launcher 持有的进程/线程句柄。
     * cnc-ddraw config.exe 本身继续独立运行。
     */
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 1;
}

int LauncherApp_StartGame(void) {
    WCHAR game_exe[CASTLE_PATH_CAP];
    WCHAR game_dir[CASTLE_PATH_CAP];
    WCHAR game_root[CASTLE_PATH_CAP];
    int ok = 0;

    if (!open_log_for_launch_()) {
        show_error_((const WCHAR*)L"无法创建 mods\\modloader.log。请检查目录权限，或在“设置”中关闭该日志后重试。");
        return 0;
    }

    log_line_((const WCHAR*)L"[GUI] 用户点击“启动游戏”；开始执行 Pre-Loader 启动链。");
    log_line_((const WCHAR*)L"[路径模式] 相对路径模式已启用；工作目录固定为 CastleModLoader.exe 所在目录。");
    log_two_((const WCHAR*)L"[目录] Mods 根：", g_mods);

    if (resolve_game_path_(game_exe, game_dir, game_root)) {
        log_two_((const WCHAR*)L"[目录] 游戏 EXE：", game_exe);
        log_two_((const WCHAR*)L"[目录] 游戏根：", game_root);
        ok = create_and_prepare_game_(game_exe, game_dir, game_root);
    }

    if (g_log != INVALID_HANDLE_VALUE_) {
        CloseHandle(g_log);
        g_log = INVALID_HANDLE_VALUE_;
    }
    return ok;
}

void WINAPI WinMainCRTStartup(void) {
    UINT code;

    if (!initialize_paths_and_config_()) ExitProcess(10u);
    code = LauncherGui_Run();

    if (g_log != INVALID_HANDLE_VALUE_) {
        CloseHandle(g_log);
        g_log = INVALID_HANDLE_VALUE_;
    }
    ExitProcess(code);
}
