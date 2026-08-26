#include "platform.h"
#include "mod_loader.h"
#include "override_loader.h"
#include "locale_layer.h"

/*
 * 《幽城幻剑录》Pre-Loader Core 的 Mod 管理层。
 *
 * 最终目录规则只有一套：
 *
 *   <RPG.exe目录>\CastleModLoader.exe
 *   <RPG.exe目录>\mods\CastleLocaleBootstrap.dll
 *   <RPG.exe目录>\mods\CastleModCore.dll
 *   <RPG.exe目录>\mods\CastleModLoader.ini
 *   <RPG.exe目录>\mods\modloader.log / game.log / mods.ini
 *   <RPG.exe目录>\mods\asi\*.asi
 *   <RPG.exe目录>\mods\asi\<ASI依赖DLL>
 *   <RPG.exe目录>\mods\overrides\<文件型Mod>\...
 *
 * mods\ 根目录只允许 Loader 自身两个内部 DLL、配置和日志；根目录 *.asi、scripts、plugins、update 都不参与 Mod 加载。
 * 所有【用户 Mod】只进入 asi/overrides 两个子目录：这样删除 CastleModLoader.exe + 整个 mods\ 就能完整卸载，
 * 同时又不会把 Loader 自身 DLL 或 ddraw 图形兼容层误当成用户 Mod。
 */

#define MAX_MOD_ITEMS_ 256u
#define MAX_INI_BYTES_ 65535u
#define MAX_LOG_UTF8_  8192u
#define MAX_OVERRIDE_DETAIL_LOGS_ 128u

/* 这个名称是 Loader 保留目录：永远不写入 [Overrides]，也永远不参与文件覆盖。 */
#define OVERRIDE_TEMPLATE_NAME_ L"模板_复制后改名"

typedef struct NameItem_ {
    WCHAR name[MAX_PATH_];
} NameItem_;

typedef struct ConfigEntry_ {
    /*
     * [ASI] 保存不带 .asi 的裸文件名；[Overrides] 保存文件夹名。
     * 保持配置原始语义可以让日志、自动追加和最终实际加载顺序完全一致。
     */
    WCHAR name[MAX_PATH_];
    int enabled;
} ConfigEntry_;

typedef enum ConfigSection_ {
    CONFIG_SECTION_NONE_ = 0,
    CONFIG_SECTION_ASI_ = 1,
    CONFIG_SECTION_OVERRIDES_ = 2
} ConfigSection_;

static HMODULE g_self_module;
static WCHAR g_mod_root[YCR_PATH_CAP];
static WCHAR g_game_root[YCR_PATH_CAP];
static WCHAR g_mods_root[YCR_PATH_CAP];
static WCHAR g_asi_root[YCR_PATH_CAP];
static WCHAR g_overrides_root[YCR_PATH_CAP];
static WCHAR g_mods_ini[YCR_PATH_CAP];
static HANDLE g_log = INVALID_HANDLE_VALUE_;
static DWORD g_override_detail_logs;
/*
 * 两个日志开关由 Launcher 读取 CastleModLoader.ini 后通过环境传入。
 * Core 只消费已经确定的本轮值，不在 Loader Lock 内再次解析配置文件。
 */
static int g_modloader_log_enabled = 1;
static int g_game_log_enabled = 1;

/*
 * 扫描缓冲区是共享的，因为自动生成配置时不会同时遍历 ASI 和 Overrides。
 * 全部放静态区，避免 256*MAX_PATH 的数组压到老游戏主线程栈上。
 */
static NameItem_ g_scan_items[MAX_MOD_ITEMS_];
/*
 * 这两组数组只保存“本轮磁盘上存在、但 mods.ini 还没有登记”的新项目。
 * 之所以不能继续复用 g_scan_items，是因为 ASI 与 Overrides 要在一次启动里同时完成：
 * 扫描、写回配置、加入本轮内存顺序。把新项目单独存下来以后，即使持久化失败，
 * 本轮仍然可以按已经扫描到的事实继续加载，不会再出现 v0.2.10 那种“扫描到 5 个、只加载 4 个”的回归。
 */
static NameItem_ g_new_asi_items[MAX_MOD_ITEMS_];
static NameItem_ g_new_override_items[MAX_MOD_ITEMS_];
static UINT g_new_asi_count;
static UINT g_new_override_count;
static UINT g_last_empty_override_dirs;
static ConfigEntry_ g_asi_entries[MAX_MOD_ITEMS_];
static ConfigEntry_ g_override_entries[MAX_MOD_ITEMS_];
static UINT g_asi_count;
static UINT g_override_count;
static BYTE g_ini_bytes[MAX_INI_BYTES_ + 4u];
static WCHAR g_ini_wide[MAX_INI_BYTES_ + 4u];
static char g_utf8[MAX_LOG_UTF8_];

/* ---------- 不依赖 CRT 的基础字符串函数 ---------- */

static UINT wlen_(const WCHAR* s) {
    UINT n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}

static WCHAR fold_ascii_(WCHAR c) {
    if (c >= (WCHAR)'A' && c <= (WCHAR)'Z') return (WCHAR)(c + ((WCHAR)'a' - (WCHAR)'A'));
    return c;
}

static int weq_i_(const WCHAR* a, const WCHAR* b) {
    UINT i = 0;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        if (fold_ascii_(a[i]) != fold_ascii_(b[i])) return 0;
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
    UINT d, i = 0;
    if (!dst || !src || cap == 0u) return 0;
    d = wlen_(dst);
    if (d >= cap) return 0;
    while (src[i]) {
        if (d + i + 1u >= cap) return 0;
        dst[d + i] = src[i];
        ++i;
    }
    dst[d + i] = 0;
    return 1;
}

static int path_join_(WCHAR* out, UINT cap, const WCHAR* dir, const WCHAR* name) {
    UINT n;
    if (!wcopy_(out, cap, dir)) return 0;
    n = wlen_(out);
    if (n && out[n - 1u] != (WCHAR)'\\' && out[n - 1u] != (WCHAR)'/') {
        if (!wappend_(out, cap, (const WCHAR*)L"\\")) return 0;
    }
    return wappend_(out, cap, name);
}

static int ends_asi_(const WCHAR* name) {
    UINT n = wlen_(name);
    if (n < 4u) return 0;
    return fold_ascii_(name[n - 4u]) == (WCHAR)'.' &&
           fold_ascii_(name[n - 3u]) == (WCHAR)'a' &&
           fold_ascii_(name[n - 2u]) == (WCHAR)'s' &&
           fold_ascii_(name[n - 1u]) == (WCHAR)'i';
}

static int valid_plain_name_(const WCHAR* name) {
    UINT i;
    if (!name || !name[0]) return 0;
    if ((name[0] == (WCHAR)'.' && name[1] == 0) ||
        (name[0] == (WCHAR)'.' && name[1] == (WCHAR)'.' && name[2] == 0)) return 0;
    for (i = 0; name[i]; ++i) {
        /* 配置项只能是当前目标目录下一层名字，不允许通过 ../ 或绝对路径跑出 mods。 */
        if (name[i] == (WCHAR)'\\' || name[i] == (WCHAR)'/' || name[i] == (WCHAR)':') return 0;
    }
    return 1;
}

static int valid_asi_base_(const WCHAR* name) {
    return valid_plain_name_(name) && !ends_asi_(name);
}

static int valid_override_name_(const WCHAR* name) {
    if (!valid_plain_name_(name)) return 0;
    /* 固定模板是说明结构，不是一个真正 Mod；即使用户手工把它写进 INI，也明确拒绝。 */
    if (weq_i_(name, (const WCHAR*)OVERRIDE_TEMPLATE_NAME_)) return 0;
    return 1;
}

static int asi_to_base_(WCHAR* out, UINT cap, const WCHAR* asi_name) {
    UINT n, i;
    volatile WCHAR* dst;
    const volatile WCHAR* src;
    if (!out || !asi_name || !ends_asi_(asi_name)) return 0;
    n = wlen_(asi_name);
    if (n <= 4u || n - 4u + 1u > cap) return 0;
    dst = (volatile WCHAR*)out;
    src = (const volatile WCHAR*)asi_name;
    for (i = 0; i < n - 4u; ++i) dst[i] = src[i];
    dst[n - 4u] = 0;
    return 1;
}

static int base_to_asi_(WCHAR* out, UINT cap, const WCHAR* base) {
    if (!wcopy_(out, cap, base)) return 0;
    return wappend_(out, cap, (const WCHAR*)L".asi");
}

/* 数字段按数值排序，避免 10_Mod 排到 2_Mod 前面。 */
static int natural_cmp_(const WCHAR* a, const WCHAR* b) {
    UINT ia = 0, ib = 0;
    while (a[ia] && b[ib]) {
        if (a[ia] >= (WCHAR)'0' && a[ia] <= (WCHAR)'9' && b[ib] >= (WCHAR)'0' && b[ib] <= (WCHAR)'9') {
            UINT za = ia, zb = ib, ea, eb, lena, lenb, k;
            while (a[za] == (WCHAR)'0') ++za;
            while (b[zb] == (WCHAR)'0') ++zb;
            ea = za; eb = zb;
            while (a[ea] >= (WCHAR)'0' && a[ea] <= (WCHAR)'9') ++ea;
            while (b[eb] >= (WCHAR)'0' && b[eb] <= (WCHAR)'9') ++eb;
            lena = ea - za; lenb = eb - zb;
            if (lena != lenb) return lena < lenb ? -1 : 1;
            for (k = 0; k < lena; ++k) if (a[za + k] != b[zb + k]) return a[za + k] < b[zb + k] ? -1 : 1;
            if ((za - ia) != (zb - ib)) return (za - ia) < (zb - ib) ? -1 : 1;
            ia = ea; ib = eb;
            continue;
        }
        {
            WCHAR ca = fold_ascii_(a[ia]), cb = fold_ascii_(b[ib]);
            if (ca != cb) return ca < cb ? -1 : 1;
        }
        ++ia; ++ib;
    }
    if (a[ia]) return 1;
    if (b[ib]) return -1;
    return 0;
}

static void sort_scan_items_(UINT count) {
    UINT i, j;
    WCHAR temp[MAX_PATH_];
    for (i = 1u; i < count; ++i) {
        wcopy_(temp, MAX_PATH_, g_scan_items[i].name);
        j = i;
        while (j > 0u && natural_cmp_(temp, g_scan_items[j - 1u].name) < 0) {
            wcopy_(g_scan_items[j].name, MAX_PATH_, g_scan_items[j - 1u].name);
            --j;
        }
        wcopy_(g_scan_items[j].name, MAX_PATH_, temp);
    }
}

/* ---------- UTF-8 中文日志 ---------- */

static void u32_to_w_(DWORD value, WCHAR* out, UINT cap) {
    WCHAR temp[16];
    UINT n = 0, i;
    if (!out || cap == 0u) return;
    do {
        temp[n++] = (WCHAR)('0' + (value % 10u));
        value /= 10u;
    } while (value && n < 15u);
    if (n + 1u > cap) { out[0] = 0; return; }
    for (i = 0; i < n; ++i) out[i] = temp[n - 1u - i];
    out[n] = 0;
}

void ModLoader_Log(const WCHAR* line) {
    int bytes;
    DWORD wrote = 0;
    if (g_log == INVALID_HANDLE_VALUE_ || !line) return;
    bytes = WideCharToMultiByte(CP_UTF8_, 0, line, -1, g_utf8, (int)(MAX_LOG_UTF8_ - 3u), NULL_PTR, NULL_PTR);
    if (bytes <= 1) return;
    WriteFile(g_log, g_utf8, (DWORD)(bytes - 1), &wrote, NULL_PTR);
    WriteFile(g_log, "\r\n", 2u, &wrote, NULL_PTR);
}

void ModLoader_LogTwo(const WCHAR* left, const WCHAR* right) {
    WCHAR line[YCR_PATH_CAP + 512u];
    line[0] = 0;
    if (!wcopy_(line, (UINT)(sizeof(line) / sizeof(line[0])), left)) return;
    if (!wappend_(line, (UINT)(sizeof(line) / sizeof(line[0])), right)) return;
    ModLoader_Log(line);
}

void ModLoader_LogError(const WCHAR* prefix, DWORD error) {
    WCHAR num[32];
    WCHAR line[512];
    u32_to_w_(error, num, 32u);
    line[0] = 0;
    wcopy_(line, 512u, prefix);
    wappend_(line, 512u, (const WCHAR*)L"，Win32错误码=");
    wappend_(line, 512u, num);
    ModLoader_Log(line);
}

void ModLoader_LogOverrideHit(const WCHAR* requested, const WCHAR* replacement) {
    /*
     * 文件型 Mod 可能在一秒内反复读取同一个资源。无限逐文件日志会让磁盘 I/O 反过来影响老游戏。
     * 因此只记录前 128 次详细命中；超过后继续正常覆盖文件，只停止刷详细路径。
     */
    if (g_override_detail_logs < MAX_OVERRIDE_DETAIL_LOGS_) {
        ModLoader_LogTwo((const WCHAR*)L"[覆盖命中] 原请求：", requested);
        ModLoader_LogTwo((const WCHAR*)L"[覆盖命中] 实际来源：", replacement);
        ++g_override_detail_logs;
        if (g_override_detail_logs == MAX_OVERRIDE_DETAIL_LOGS_)
            ModLoader_Log((const WCHAR*)L"[覆盖日志] 已记录 128 次详细命中；后续仍正常覆写，但不再逐文件刷日志。");
    }
}

/* ---------- 根目录和固定目录结构 ---------- */

static int get_mod_root_(HMODULE self_module) {
    DWORD n;
    (void)self_module;
    /*
     * Pre-Loader 时代，Core DLL 的物理位置不再用于猜 Mod 根目录。
     * Launcher 已经把目标进程工作目录固定为 RPG.exe 所在目录，并把相对路径 "mods" 写入 CASTLE_MOD_ROOT。
     * Core 直接使用这个相对目录，因此整个 Mod 目录可以随游戏整体搬迁，不需要保存盘符或绝对安装路径。
     */
    n = GetEnvironmentVariableW((const WCHAR*)L"CASTLE_MOD_ROOT", g_mod_root, YCR_PATH_CAP);
    return n > 0u && n < YCR_PATH_CAP;
}

static int directory_exists_(const WCHAR* path) {
    DWORD attr = GetFileAttributesW(path);
    return attr != INVALID_FILE_ATTRIBUTES_ && (attr & FILE_ATTRIBUTE_DIRECTORY_) != 0;
}

static int file_exists_(const WCHAR* path) {
    DWORD attr = GetFileAttributesW(path);
    return attr != INVALID_FILE_ATTRIBUTES_ && !(attr & FILE_ATTRIBUTE_DIRECTORY_);
}

static int ensure_directory_(const WCHAR* path) {
    if (directory_exists_(path)) return 1;
    if (CreateDirectoryW(path, NULL_PTR)) return 1;
    if (GetLastError() == ERROR_ALREADY_EXISTS_) return directory_exists_(path);
    ModLoader_LogError((const WCHAR*)L"[目录错误] 无法创建目录", GetLastError());
    ModLoader_LogTwo((const WCHAR*)L"[目录错误] 路径：", path);
    return 0;
}

static void prepare_roots_(void) {
    DWORD n;
    g_game_root[0] = 0;
    g_mods_root[0] = 0;
    g_asi_root[0] = 0;
    g_overrides_root[0] = 0;
    g_mods_ini[0] = 0;

    /*
     * 游戏安装根由 Launcher 明确传入，但 v0.2.1 起传的是相对位置：标准 exe 布局为 ".."，
     * 非 exe 布局则为 "."。真正需要做“是否越出游戏根”的安全比较时，OverrideLoader 内部才临时规范化。
     */
    n = GetEnvironmentVariableW((const WCHAR*)L"CASTLE_GAME_ROOT", g_game_root, YCR_PATH_CAP);
    if (!n || n >= YCR_PATH_CAP) return;

    /* CASTLE_MOD_ROOT 已经就是唯一 mods\ 目录，因此这里绝不能再拼一次 "mods"。 */
    wcopy_(g_mods_root, YCR_PATH_CAP, g_mod_root);
    path_join_(g_asi_root, YCR_PATH_CAP, g_mods_root, (const WCHAR*)L"asi");
    path_join_(g_overrides_root, YCR_PATH_CAP, g_mods_root, (const WCHAR*)L"overrides");
    path_join_(g_mods_ini, YCR_PATH_CAP, g_mods_root, (const WCHAR*)L"mods.ini");
}

/*
 * 每次启动都检查模板目录。用户删掉模板，下次会自动恢复；用户往模板里放文件也不会参与实际覆盖。
 *
 * 模板结构来自当前精确 RPG.exe 路径字符串与用户提供 multimedia 样本：
 *   - exe：表示与 RPG.exe 同目录的文件；
 *   - MultiMedia：表示游戏根目录下的真实资源树；
 *   - MultiMedia 下的各空目录是当前样本真实出现过的主要逻辑目录。
 *
 * 模板里不创建示例文件，避免用户复制模板后还要手工删除无关说明文件。
 */
static int ensure_override_template_(void) {
    static const WCHAR* dirs[] = {
        L"exe",
        L"MultiMedia",
        L"MultiMedia\\fight",
        L"MultiMedia\\Font",
        L"MultiMedia\\Map",
        L"MultiMedia\\menus",
        L"MultiMedia\\Mov",
        L"MultiMedia\\Music",
        L"MultiMedia\\public",
        L"MultiMedia\\save",
        L"MultiMedia\\Sys"
    };
    WCHAR template_root[YCR_PATH_CAP];
    WCHAR path[YCR_PATH_CAP];
    UINT i;

    if (!path_join_(template_root, YCR_PATH_CAP, g_overrides_root, (const WCHAR*)OVERRIDE_TEMPLATE_NAME_)) return 0;

    /*
     * 用户要求模板是“一次性保底”：不存在时创建标准骨架；已经存在时完全尊重现状，不再补目录、
     * 不扫描其中内容、更不会把模板写进 mods.ini。这样用户若自行删减/调整模板结构，Loader 不会每次启动又改回来。
     */
    if (directory_exists_(template_root)) return 1;
    if (!ensure_directory_(template_root)) return 0;

    for (i = 0; i < (UINT)(sizeof(dirs) / sizeof(dirs[0])); ++i) {
        if (!path_join_(path, YCR_PATH_CAP, template_root, dirs[i])) return 0;
        if (!ensure_directory_(path)) return 0;
    }
    return 1;
}

static int ensure_mod_directories_(void) {
    if (!ensure_directory_(g_mods_root)) return 0;
    if (!ensure_directory_(g_asi_root)) return 0;
    if (!ensure_directory_(g_overrides_root)) return 0;
    if (!ensure_override_template_()) return 0;
    return 1;
}

/* ---------- 扫描 ASI 与 Overrides 文件夹 ---------- */

static UINT scan_asi_(void) {
    WCHAR pattern[YCR_PATH_CAP];
    WIN32_FIND_DATAW_ data;
    HANDLE find;
    UINT count = 0;
    if (!path_join_(pattern, YCR_PATH_CAP, g_asi_root, (const WCHAR*)L"*.asi")) return 0;
    find = FindFirstFileW(pattern, &data);
    if (find == INVALID_HANDLE_VALUE_) return 0;
    do {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY_) && ends_asi_(data.cFileName)) {
            if (count < MAX_MOD_ITEMS_ && wcopy_(g_scan_items[count].name, MAX_PATH_, data.cFileName)) ++count;
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    sort_scan_items_(count);
    return count;
}

/*
 * 判断一个 Overrides Mod 目录里是否“真的有文件”。
 *
 * Overrides 的正常结构通常是：Mod名\MultiMedia\Map\...，所以不能只看 Mod 根目录的第一层；
 * 根目录只有子文件夹、真正资源在更深层时仍然必须算有效 Mod。这里递归搜索普通文件：
 *   - 找到任意一个普通文件立即返回 1；
 *   - 只有空目录/空子目录时返回 0；
 *   - 跳过重解析点，避免目录联接或符号链接把扫描带出 mods\overrides 或形成循环；
 *   - 深度上限 32 层，防止恶意或损坏目录把老游戏主线程栈无限吃掉。
 */
static int override_dir_contains_file_(const WCHAR* dir, UINT depth) {
    WCHAR pattern[YCR_PATH_CAP];
    WCHAR child[YCR_PATH_CAP];
    WIN32_FIND_DATAW_ data;
    HANDLE find;
    if (!dir || !dir[0] || depth >= 32u) return 0;
    if (!path_join_(pattern, YCR_PATH_CAP, dir, (const WCHAR*)L"*")) return 0;
    find = FindFirstFileW(pattern, &data);
    if (find == INVALID_HANDLE_VALUE_) return 0;
    do {
        if ((data.cFileName[0] == (WCHAR)'.' && data.cFileName[1] == 0) ||
            (data.cFileName[0] == (WCHAR)'.' && data.cFileName[1] == (WCHAR)'.' && data.cFileName[2] == 0))
            continue;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY_) {
            if (data.dwFileAttributes & 0x00000400u) continue; /* FILE_ATTRIBUTE_REPARSE_POINT */
            if (!path_join_(child, YCR_PATH_CAP, dir, data.cFileName)) continue;
            if (override_dir_contains_file_(child, depth + 1u)) { FindClose(find); return 1; }
        } else {
            FindClose(find);
            return 1;
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    return 0;
}

static UINT scan_override_dirs_(void) {
    WCHAR pattern[YCR_PATH_CAP];
    WCHAR candidate[YCR_PATH_CAP];
    WIN32_FIND_DATAW_ data;
    HANDLE find;
    UINT count = 0;
    g_last_empty_override_dirs = 0u;
    if (!path_join_(pattern, YCR_PATH_CAP, g_overrides_root, (const WCHAR*)L"*")) return 0;
    find = FindFirstFileW(pattern, &data);
    if (find == INVALID_HANDLE_VALUE_) return 0;
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY_) {
            if (!valid_override_name_(data.cFileName)) continue;
            if (!path_join_(candidate, YCR_PATH_CAP, g_overrides_root, data.cFileName)) continue;
            if (!override_dir_contains_file_(candidate, 0u)) {
                ++g_last_empty_override_dirs;
                continue;
            }
            if (count < MAX_MOD_ITEMS_ && wcopy_(g_scan_items[count].name, MAX_PATH_, data.cFileName)) ++count;
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    sort_scan_items_(count);
    return count;
}

/* ---------- mods.ini UTF-8/UTF-16/ANSI 读取 ---------- */

static WCHAR* trim_left_(WCHAR* s) {
    while (*s == (WCHAR)' ' || *s == (WCHAR)'\t') ++s;
    return s;
}

static void trim_right_(WCHAR* s) {
    UINT n = wlen_(s);
    while (n && (s[n - 1u] == (WCHAR)' ' || s[n - 1u] == (WCHAR)'\t')) s[--n] = 0;
}

static int decode_ini_(DWORD bytes, WCHAR** text_out) {
    int chars;
    DWORD i;
    if (!text_out) return 0;
    *text_out = NULL_PTR;
    if (bytes == 0u) { g_ini_wide[0] = 0; *text_out = g_ini_wide; return 1; }

    if (bytes >= 2u && g_ini_bytes[0] == 0xFFu && g_ini_bytes[1] == 0xFEu) {
        DWORD out = 0;
        for (i = 2u; i + 1u < bytes && out + 1u < MAX_INI_BYTES_; i += 2u)
            g_ini_wide[out++] = (WCHAR)((UINT)g_ini_bytes[i] | ((UINT)g_ini_bytes[i + 1u] << 8));
        g_ini_wide[out] = 0;
        *text_out = g_ini_wide;
        return 1;
    }

    {
        DWORD skip = (bytes >= 3u && g_ini_bytes[0] == 0xEFu && g_ini_bytes[1] == 0xBBu && g_ini_bytes[2] == 0xBFu) ? 3u : 0u;
        chars = MultiByteToWideChar(CP_UTF8_, MB_ERR_INVALID_CHARS_, (LPCSTR)(g_ini_bytes + skip),
                                    (int)(bytes - skip), g_ini_wide, (int)(MAX_INI_BYTES_ - 1u));
        if (chars <= 0) {
            /* 兼容少量旧工具生成的 ANSI 配置；严格 UTF-8 失败后才回退。 */
            chars = MultiByteToWideChar(CP_ACP_, 0, (LPCSTR)(g_ini_bytes + skip),
                                        (int)(bytes - skip), g_ini_wide, (int)(MAX_INI_BYTES_ - 1u));
        }
        if (chars <= 0) return 0;
        g_ini_wide[chars] = 0;
        *text_out = g_ini_wide;
        return 1;
    }
}

static int config_has_(ConfigEntry_* entries, UINT count, const WCHAR* name) {
    UINT i;
    for (i = 0; i < count; ++i) if (weq_i_(entries[i].name, name)) return 1;
    return 0;
}

static int add_config_entry_(ConfigEntry_* entries, UINT* count, const WCHAR* name, int enabled, const WCHAR* kind) {
    if (*count >= MAX_MOD_ITEMS_) {
        ModLoader_LogTwo((const WCHAR*)L"[配置警告] 条目达到 256 项上限，后续不再读取：", kind);
        return 0;
    }
    if (config_has_(entries, *count, name)) {
        ModLoader_LogTwo((const WCHAR*)L"[配置警告] 重复条目按最上面第一次为准，忽略：", name);
        return 0;
    }
    if (!wcopy_(entries[*count].name, MAX_PATH_, name)) return 0;
    entries[*count].enabled = enabled ? 1 : 0;
    ++*count;
    return 1;
}

static int read_mods_ini_(void) {
    HANDLE file;
    DWORD size, read = 0;
    WCHAR* text;
    WCHAR* p;
    ConfigSection_ section = CONFIG_SECTION_NONE_;

    g_asi_count = 0u;
    g_override_count = 0u;

    file = CreateFileW(g_mods_ini, GENERIC_READ_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_ | FILE_SHARE_DELETE_,
                       NULL_PTR, OPEN_EXISTING_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) return 0;
    size = GetFileSize(file, NULL_PTR);
    if (size == 0xFFFFFFFFu || size > MAX_INI_BYTES_) { CloseHandle(file); return 0; }
    if (size && (!ReadFile(file, g_ini_bytes, size, &read, NULL_PTR) || read != size)) { CloseHandle(file); return 0; }
    CloseHandle(file);
    if (!decode_ini_(size, &text)) return 0;

    p = text;
    while (*p) {
        WCHAR* line = p;
        WCHAR* name;
        WCHAR* eq;
        WCHAR* value;

        while (*p && *p != (WCHAR)'\r' && *p != (WCHAR)'\n') ++p;
        if (*p) {
            WCHAR end = *p;
            *p++ = 0;
            if (end == (WCHAR)'\r' && *p == (WCHAR)'\n') ++p;
        }

        name = trim_left_(line);
        trim_right_(name);
        if (!name[0] || name[0] == (WCHAR)';' || name[0] == (WCHAR)'#') continue;

        if (name[0] == (WCHAR)'[') {
            if (weq_i_(name, (const WCHAR*)L"[ASI]")) section = CONFIG_SECTION_ASI_;
            else if (weq_i_(name, (const WCHAR*)L"[Overrides]")) section = CONFIG_SECTION_OVERRIDES_;
            else {
                section = CONFIG_SECTION_NONE_;
                ModLoader_LogTwo((const WCHAR*)L"[配置提示] 未知节已忽略：", name);
            }
            continue;
        }

        if (section == CONFIG_SECTION_NONE_) {
            ModLoader_LogTwo((const WCHAR*)L"[配置警告] 条目不在 [ASI]/[Overrides] 内，忽略：", name);
            continue;
        }

        eq = name;
        while (*eq && *eq != (WCHAR)'=') ++eq;
        if (*eq != (WCHAR)'=') {
            ModLoader_LogTwo((const WCHAR*)L"[配置警告] 缺少 =，忽略：", name);
            continue;
        }
        *eq++ = 0;
        trim_right_(name);
        value = trim_left_(eq);
        trim_right_(value);

        if (!((value[0] == (WCHAR)'0' || value[0] == (WCHAR)'1') && value[1] == 0)) {
            ModLoader_LogTwo((const WCHAR*)L"[配置警告] 开关只能是 1 或 0，忽略：", name);
            continue;
        }

        if (section == CONFIG_SECTION_ASI_) {
            if (!valid_asi_base_(name)) {
                if (ends_asi_(name))
                    ModLoader_LogTwo((const WCHAR*)L"[配置警告] [ASI] 只写裸文件名，不要写 .asi，忽略：", name);
                else
                    ModLoader_LogTwo((const WCHAR*)L"[配置警告] [ASI] 文件名非法，忽略：", name);
                continue;
            }
            add_config_entry_(g_asi_entries, &g_asi_count, name, value[0] == (WCHAR)'1', (const WCHAR*)L"ASI");
        } else {
            if (!valid_override_name_(name)) {
                if (weq_i_(name, (const WCHAR*)OVERRIDE_TEMPLATE_NAME_))
                    ModLoader_Log((const WCHAR*)L"[配置提示] 模板_复制后改名 是保留模板，永远不会作为 Overrides Mod 加载。");
                else
                    ModLoader_LogTwo((const WCHAR*)L"[配置警告] [Overrides] 文件夹名非法，忽略：", name);
                continue;
            }
            add_config_entry_(g_override_entries, &g_override_count, name, value[0] == (WCHAR)'1', (const WCHAR*)L"Overrides");
        }
    }
    return 1;
}

/* ---------- mods.ini 自动生成/自动追加 ---------- */

static int write_ini_line_(HANDLE file, const WCHAR* line) {
    int bytes;
    DWORD wrote = 0;
    if (!file || file == INVALID_HANDLE_VALUE_ || !line) return 0;

    /*
     * WideCharToMultiByte(..., -1, ...) 的返回值包含结尾 NUL：
     *   - 普通文字至少返回 2；
     *   - 空字符串 L"" 正好返回 1。
     *
     * v0.2.10 错把“1”当成转换失败，因此任何正常空行都会让 mods.ini 重写中断；旧实现又已经先
     * CREATE_ALWAYS 截断原文件，最终就出现“自动扫描到了新 Mod，但配置被清掉/新条目消失”的实机回归。
     * v0.2.11 明确允许空行：只有 bytes<=0 才算转换失败；bytes==1 时不写正文，只写 CRLF。
     */
    bytes = WideCharToMultiByte(CP_UTF8_, 0, line, -1, g_utf8, (int)(MAX_LOG_UTF8_ - 3u), NULL_PTR, NULL_PTR);
    if (bytes <= 0) return 0;
    if (bytes > 1) {
        if (!WriteFile(file, g_utf8, (DWORD)(bytes - 1), &wrote, NULL_PTR) || wrote != (DWORD)(bytes - 1)) return 0;
    }
    if (!WriteFile(file, "\r\n", 2u, &wrote, NULL_PTR) || wrote != 2u) return 0;
    return 1;
}

static int generate_mods_ini_(void) {
    HANDLE file;
    DWORD wrote = 0;
    UINT count, i;
    static const BYTE bom[3] = {0xEFu, 0xBBu, 0xBFu};

    file = CreateFileW(g_mods_ini, GENERIC_WRITE_, FILE_SHARE_READ_, NULL_PTR,
                       CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) {
        ModLoader_LogError((const WCHAR*)L"[配置错误] 无法创建 mods\\mods.ini", GetLastError());
        return 0;
    }

    if (!WriteFile(file, bom, 3u, &wrote, NULL_PTR) || wrote != 3u ||
        !write_ini_line_(file, (const WCHAR*)L"; 《幽城幻剑录》Mod Loader 自动生成") ||
        !write_ini_line_(file, (const WCHAR*)L"; 1=启用，0=禁用；每个节都严格按从上到下的顺序处理。") ||
        !write_ini_line_(file, (const WCHAR*)L"; [ASI] 只写裸文件名，不要写 .asi；实际文件固定放在 mods\\asi。") ||
        !write_ini_line_(file, (const WCHAR*)L"; [Overrides] 写 mods\\overrides 下的文件夹名；越靠下优先级越高。") ||
        !write_ini_line_(file, (const WCHAR*)L"; 模板_复制后改名 永远不会进入配置，也不会被加载。") ||
        !write_ini_line_(file, (const WCHAR*)L"") ||
        !write_ini_line_(file, (const WCHAR*)L"[ASI]")) {
        CloseHandle(file);
        return 0;
    }

    count = scan_asi_();
    for (i = 0; i < count; ++i) {
        WCHAR base[MAX_PATH_];
        WCHAR line[MAX_PATH_ + 8u];
        if (!asi_to_base_(base, MAX_PATH_, g_scan_items[i].name)) continue;
        line[0] = 0;
        if (!wcopy_(line, (UINT)(sizeof(line) / sizeof(line[0])), base) ||
            !wappend_(line, (UINT)(sizeof(line) / sizeof(line[0])), (const WCHAR*)L"=1") ||
            !write_ini_line_(file, line)) {
            CloseHandle(file);
            return 0;
        }
    }

    if (!write_ini_line_(file, (const WCHAR*)L"") ||
        !write_ini_line_(file, (const WCHAR*)L"[Overrides]")) {
        CloseHandle(file);
        return 0;
    }

    count = scan_override_dirs_();
    for (i = 0; i < count; ++i) {
        WCHAR line[MAX_PATH_ + 8u];
        line[0] = 0;
        if (!wcopy_(line, (UINT)(sizeof(line) / sizeof(line[0])), g_scan_items[i].name) ||
            !wappend_(line, (UINT)(sizeof(line) / sizeof(line[0])), (const WCHAR*)L"=1") ||
            !write_ini_line_(file, line)) {
            CloseHandle(file);
            return 0;
        }
    }

    CloseHandle(file);
    ModLoader_LogTwo((const WCHAR*)L"[配置] 已自动生成唯一配置：", g_mods_ini);
    return 1;
}

/*
 * 如果用户已经有 mods.ini，又往 mods\asi 或 mods\overrides 放了新内容，Loader 只做“增补”，绝不把
 * 用户现有配置当成可以随意重建的数据库。
 *
 * v0.2.10 的根本错误是：为了把新条目塞进已有节，直接 CREATE_ALWAYS 打开原 mods.ini。
 * 一旦后续任意写入步骤失败，原配置已经被截断；同时旧函数还把“没有新项目”和“写回失败”都返回 0，
 * 上层根本无法知道配置已经受损。v0.2.11 改成以下强约束：
 *   1. 先把原配置完整读入内存；
 *   2. 新内容写到同目录临时文件；
 *   3. 临时文件全部写完并 FlushFileBuffers 成功以后，才用 MoveFileExW(REPLACE_EXISTING|WRITE_THROUGH)
 *      原子替换 mods.ini；
 *   4. 任何一步失败，只删除临时文件，原 mods.ini 一个字节都不碰；
 *   5. 即使持久化失败，磁盘扫描到的新 Mod 仍加入“本轮内存顺序”并立即生效，下一次启动再重试写回。
 *
 * 新发现顺序也固定：
 *   - 旧配置中的 ASI / Overrides 顺序和 0/1 原样保留；
 *   - 新 ASI 先按文件名排序，再追加到最后一个 [ASI] 节的末尾，因此本轮也在所有旧 ASI 后加载；
 *   - 新 Overrides 只接受递归至少含一个普通文件的目录，按文件夹名排序后追加到最后一个 [Overrides] 节；
 *   - 用户历史上若已经有重复表头，不擅自合并，只复用最后一个同名节，从本版开始不再制造新重复表头。
 */
static int line_span_eq_i_(const WCHAR* begin, const WCHAR* end, const WCHAR* literal) {
    UINT i = 0u;
    if (!begin || !end || !literal || end < begin) return 0;
    while (begin < end && (*begin == (WCHAR)' ' || *begin == (WCHAR)'\t')) ++begin;
    while (end > begin && (end[-1] == (WCHAR)' ' || end[-1] == (WCHAR)'\t')) --end;
    while (begin + i < end && literal[i]) {
        if (fold_ascii_(begin[i]) != fold_ascii_(literal[i])) return 0;
        ++i;
    }
    return begin + i == end && literal[i] == 0;
}

static int load_ini_text_for_rewrite_(WCHAR** text_out) {
    HANDLE file;
    DWORD size, read = 0u;
    if (!text_out) return 0;
    *text_out = NULL_PTR;
    file = CreateFileW(g_mods_ini, GENERIC_READ_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_ | FILE_SHARE_DELETE_,
                       NULL_PTR, OPEN_EXISTING_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) return 0;
    size = GetFileSize(file, NULL_PTR);
    if (size == 0xFFFFFFFFu || size > MAX_INI_BYTES_) { CloseHandle(file); return 0; }
    if (size && (!ReadFile(file, g_ini_bytes, size, &read, NULL_PTR) || read != size)) { CloseHandle(file); return 0; }
    CloseHandle(file);
    return decode_ini_(size, text_out);
}

static int write_ini_span_(HANDLE file, WCHAR* begin, WCHAR* end) {
    WCHAR saved;
    int ok;
    if (!file || file == INVALID_HANDLE_VALUE_ || !begin || !end || end < begin) return 0;
    saved = *end;
    *end = 0;
    ok = write_ini_line_(file, begin);
    *end = saved;
    return ok;
}

/*
 * 收集“磁盘存在但配置未登记”的 ASI。
 * scan_asi_() 已按文件名排序；这里把 .asi 扩展名去掉后保存，因此 g_new_asi_items 本身就是最终追加顺序。
 */
static void collect_new_asi_items_(void) {
    UINT count = scan_asi_(), i;
    g_new_asi_count = 0u;
    for (i = 0u; i < count; ++i) {
        WCHAR base[MAX_PATH_];
        if (!asi_to_base_(base, MAX_PATH_, g_scan_items[i].name)) continue;
        if (config_has_(g_asi_entries, g_asi_count, base)) continue;
        if (g_asi_count + g_new_asi_count >= MAX_MOD_ITEMS_) {
            ModLoader_LogTwo((const WCHAR*)L"[自动发现警告] ASI 配置已达到 256 项上限，本轮不写入/不加载：", base);
            continue;
        }
        if (wcopy_(g_new_asi_items[g_new_asi_count].name, MAX_PATH_, base)) ++g_new_asi_count;
    }
}

/*
 * 收集新的 Overrides。
 * scan_override_dirs_() 已经：排除模板、排除空目录、递归确认至少存在一个普通文件、按名称排序。
 */
static void collect_new_override_items_(void) {
    UINT count = scan_override_dirs_(), i;
    g_new_override_count = 0u;
    for (i = 0u; i < count; ++i) {
        if (config_has_(g_override_entries, g_override_count, g_scan_items[i].name)) continue;
        if (g_override_count + g_new_override_count >= MAX_MOD_ITEMS_) {
            ModLoader_LogTwo((const WCHAR*)L"[自动发现警告] Overrides 配置已达到 256 项上限，本轮不写入/不加载：", g_scan_items[i].name);
            continue;
        }
        if (wcopy_(g_new_override_items[g_new_override_count].name, MAX_PATH_, g_scan_items[i].name)) ++g_new_override_count;
    }
}

static int write_new_item_list_(HANDLE file, const NameItem_* items, UINT count) {
    UINT i;
    for (i = 0u; i < count; ++i) {
        WCHAR line[MAX_PATH_ + 8u];
        line[0] = 0;
        if (!wcopy_(line, (UINT)(sizeof(line) / sizeof(line[0])), items[i].name) ||
            !wappend_(line, (UINT)(sizeof(line) / sizeof(line[0])), (const WCHAR*)L"=1") ||
            !write_ini_line_(file, line)) return 0;
    }
    return 1;
}

/*
 * 临时文件清理也用 Kernel32 的宽字符 API，但为了不再给最早期 Core 增加一个静态导入，运行时解析即可。
 * DeleteFileW 在所有本项目支持的 Windows 上都存在；解析失败时只会留下一个无害 .tmp，不会伤害主配置。
 */
static void delete_ini_temp_(const WCHAR* path) {
    typedef BOOL (WINAPI *PFN_DeleteFileW_)(LPCWSTR);
    HMODULE kernel = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    PFN_DeleteFileW_ fn;
    if (!kernel || !path) return;
    fn = (PFN_DeleteFileW_)GetProcAddress(kernel, "DeleteFileW");
    if (fn) fn(path);
}

/*
 * 原子替换必须发生在临时文件已经完整写完、关闭以后。
 * MOVEFILE_REPLACE_EXISTING 允许覆盖旧 mods.ini；MOVEFILE_WRITE_THROUGH 要求系统把移动/替换尽量同步刷到磁盘。
 */
static int replace_ini_atomically_(const WCHAR* temp_path) {
    typedef BOOL (WINAPI *PFN_MoveFileExW_)(LPCWSTR, LPCWSTR, DWORD);
    HMODULE kernel = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    PFN_MoveFileExW_ fn;
    if (!kernel || !temp_path) return 0;
    fn = (PFN_MoveFileExW_)GetProcAddress(kernel, "MoveFileExW");
    if (!fn) return 0;
    return fn(temp_path, g_mods_ini, 0x00000001u | 0x00000008u) ? 1 : 0;
}

/*
 * 把本轮新发现项目写进临时文件，然后一次性替换主配置。
 * 返回：1=成功持久化；0=失败但原配置保持不变。
 */
static int persist_new_items_atomic_(void) {
    WCHAR* text;
    WCHAR* p;
    WCHAR* asi_insert_before = NULL_PTR;
    WCHAR* override_insert_before = NULL_PTR;
    WCHAR temp_path[YCR_PATH_CAP];
    ConfigSection_ active = CONFIG_SECTION_NONE_;
    int asi_section_exists = 0;
    int override_section_exists = 0;
    int asi_written = 0;
    int override_written = 0;
    HANDLE file;
    DWORD wrote = 0u;
    DWORD error = 0u;
    static const BYTE bom[3] = {0xEFu, 0xBBu, 0xBFu};

    if (!g_new_asi_count && !g_new_override_count) return 1;
    if (!load_ini_text_for_rewrite_(&text)) {
        ModLoader_Log((const WCHAR*)L"[配置错误] 发现新 Mod，但无法读取原 mods.ini；本轮仍加载新 Mod，原配置保持不变。");
        return 0;
    }

    /* 第一遍定位最后一个同名节的结束位置；若该节就在 EOF，则 insert_before 保持 NULL。 */
    p = text;
    while (*p) {
        WCHAR* line = p;
        WCHAR* end = p;
        while (*end && *end != (WCHAR)'\r' && *end != (WCHAR)'\n') ++end;
        if (end > line) {
            WCHAR* left = line;
            WCHAR* right = end;
            while (left < right && (*left == (WCHAR)' ' || *left == (WCHAR)'\t')) ++left;
            while (right > left && (right[-1] == (WCHAR)' ' || right[-1] == (WCHAR)'\t')) --right;
            if (left < right && *left == (WCHAR)'[') {
                if (active == CONFIG_SECTION_ASI_) asi_insert_before = line;
                else if (active == CONFIG_SECTION_OVERRIDES_) override_insert_before = line;
                if (line_span_eq_i_(left, right, (const WCHAR*)L"[ASI]")) {
                    asi_section_exists = 1;
                    active = CONFIG_SECTION_ASI_;
                    asi_insert_before = NULL_PTR;
                } else if (line_span_eq_i_(left, right, (const WCHAR*)L"[Overrides]")) {
                    override_section_exists = 1;
                    active = CONFIG_SECTION_OVERRIDES_;
                    override_insert_before = NULL_PTR;
                } else {
                    active = CONFIG_SECTION_NONE_;
                }
            }
        }
        p = end;
        if (*p == (WCHAR)'\r') ++p;
        if (*p == (WCHAR)'\n') ++p;
    }

    if (!path_join_(temp_path, YCR_PATH_CAP, g_mods_root, (const WCHAR*)L"mods.ini.castle.tmp")) return 0;
    delete_ini_temp_(temp_path);
    file = CreateFileW(temp_path, GENERIC_WRITE_, FILE_SHARE_READ_, NULL_PTR,
                       CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) {
        ModLoader_LogError((const WCHAR*)L"[配置错误] 无法创建 mods.ini 临时文件", GetLastError());
        return 0;
    }
    if (!WriteFile(file, bom, 3u, &wrote, NULL_PTR) || wrote != 3u) goto write_failed;

    /* 第二遍按原物理行顺序写回；在最后一个对应节结束以前插入本轮新项目。 */
    p = text;
    while (*p) {
        WCHAR* line = p;
        WCHAR* end = p;
        while (*end && *end != (WCHAR)'\r' && *end != (WCHAR)'\n') ++end;

        if (!asi_written && g_new_asi_count && asi_section_exists && asi_insert_before == line) {
            if (!write_new_item_list_(file, g_new_asi_items, g_new_asi_count)) goto write_failed;
            asi_written = 1;
        }
        if (!override_written && g_new_override_count && override_section_exists && override_insert_before == line) {
            if (!write_new_item_list_(file, g_new_override_items, g_new_override_count)) goto write_failed;
            override_written = 1;
        }
        if (!write_ini_span_(file, line, end)) goto write_failed;
        p = end;
        if (*p == (WCHAR)'\r') ++p;
        if (*p == (WCHAR)'\n') ++p;
    }

    if (!asi_written && g_new_asi_count) {
        if (!asi_section_exists) {
            if (!write_ini_line_(file, (const WCHAR*)L"") || !write_ini_line_(file, (const WCHAR*)L"[ASI]")) goto write_failed;
        }
        if (!write_new_item_list_(file, g_new_asi_items, g_new_asi_count)) goto write_failed;
    }
    if (!override_written && g_new_override_count) {
        if (!override_section_exists) {
            if (!write_ini_line_(file, (const WCHAR*)L"") || !write_ini_line_(file, (const WCHAR*)L"[Overrides]")) goto write_failed;
        }
        if (!write_new_item_list_(file, g_new_override_items, g_new_override_count)) goto write_failed;
    }

    if (!FlushFileBuffers(file)) goto write_failed;
    CloseHandle(file);
    if (!replace_ini_atomically_(temp_path)) {
        error = GetLastError();
        delete_ini_temp_(temp_path);
        ModLoader_LogError((const WCHAR*)L"[配置错误] 临时 mods.ini 已写完，但原子替换主配置失败；原配置保持不变", error);
        return 0;
    }
    return 1;

write_failed:
    error = GetLastError();
    CloseHandle(file);
    delete_ini_temp_(temp_path);
    ModLoader_LogError((const WCHAR*)L"[配置错误] 写入 mods.ini 临时文件失败；原配置保持不变", error);
    return 0;
}

/*
 * 把新发现项目加入本轮内存顺序。
 * 这一步与磁盘持久化故意解耦：配置写回失败不能让已经发现的 Mod 在当前启动“凭空消失”。
 */
static void activate_new_items_for_this_run_(void) {
    UINT i;
    for (i = 0u; i < g_new_asi_count; ++i) {
        if (add_config_entry_(g_asi_entries, &g_asi_count, g_new_asi_items[i].name, 1, (const WCHAR*)L"ASI"))
            ModLoader_LogTwo((const WCHAR*)L"[自动发现] 新 ASI 本轮追加到现有顺序末尾并默认启用：", g_new_asi_items[i].name);
    }
    for (i = 0u; i < g_new_override_count; ++i) {
        if (add_config_entry_(g_override_entries, &g_override_count, g_new_override_items[i].name, 1, (const WCHAR*)L"Overrides"))
            ModLoader_LogTwo((const WCHAR*)L"[自动发现] 新 Overrides 本轮按名称顺序追加并默认启用：", g_new_override_items[i].name);
    }
}

/*
 * 把“自动发现”做成用户能直接从日志确认的启动阶段，而不是只在内部悄悄扫描。
 *
 * 这里仅报告磁盘上真正存在的候选数量：
 *   - ASI 只看 mods\asi\*.asi；
 *   - Overrides 只看 mods\overrides\下一层文件夹，并递归确认其中至少存在一个普通文件；
 *   - 空文件夹只保留在磁盘，不进入自动配置；“模板_复制后改名”永远排除。
 *
 * 扫描、配置比较、原子持久化和本轮激活四个职责彼此分开，今后从日志即可判断坏在哪一层。
 */
static void log_discovery_snapshot_(void) {
    WCHAR num[32];
    UINT asi_disk_count = scan_asi_();
    UINT override_disk_count = scan_override_dirs_();

    u32_to_w_(asi_disk_count, num, 32u);
    ModLoader_LogTwo((const WCHAR*)L"[自动发现] mods\\asi 中发现 ASI 数量=", num);

    u32_to_w_(override_disk_count, num, 32u);
    ModLoader_LogTwo((const WCHAR*)L"[自动发现] mods\\overrides 中发现非空文件夹 Mod 数量=", num);
    if (g_last_empty_override_dirs) {
        u32_to_w_(g_last_empty_override_dirs, num, 32u);
        ModLoader_LogTwo((const WCHAR*)L"[自动发现] 已忽略没有任何文件的 Overrides 空目录数量=", num);
    }
}

static int ensure_and_read_config_(void) {
    DWORD size = 0;
    HANDLE f;

    if (!file_exists_(g_mods_ini)) {
        if (!generate_mods_ini_()) return 0;
    } else {
        /* 一个 0 字节配置没有任何可解释信息，直接按当前 mods 内容生成完整标准模板。 */
        f = CreateFileW(g_mods_ini, GENERIC_READ_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_, NULL_PTR,
                        OPEN_EXISTING_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
        if (f != INVALID_HANDLE_VALUE_) {
            size = GetFileSize(f, NULL_PTR);
            CloseHandle(f);
        }
        if (size == 0u && !generate_mods_ini_()) return 0;
    }

    if (!read_mods_ini_()) {
        ModLoader_Log((const WCHAR*)L"[配置错误] mods\\mods.ini 无法读取/解码。为避免失控加载，本轮不加载 ASI，也不启用 Overrides。");
        return 0;
    }

    /*
     * 先根据磁盘事实收集“INI 里还没有”的项目，再尝试持久化；无论写回是否成功，
     * 本轮都把这些新项目追加到内存顺序末尾。这样自动扫描不会再被配置写入成败绑架。
     */
    collect_new_asi_items_();
    collect_new_override_items_();
    if (g_new_asi_count || g_new_override_count) {
        if (persist_new_items_atomic_()) {
            ModLoader_Log((const WCHAR*)L"[配置] 新发现 Mod 已原子写回 mods.ini；原有条目、顺序、开关和注释均保留。");
        } else {
            ModLoader_Log((const WCHAR*)L"[配置警告] 新发现 Mod 未能持久化到 mods.ini，但原配置未受损；本轮仍按扫描结果加载。");
        }
        activate_new_items_for_this_run_();
    }
    return 1;
}

/* ---------- ASI 加载 ---------- */

static void load_one_asi_(const ConfigEntry_* entry, UINT* loaded, UINT* skipped, UINT* failed) {
    WCHAR file_name[MAX_PATH_];
    WCHAR path[YCR_PATH_CAP];
    HMODULE module;
    DWORD error;

    if (!entry->enabled) {
        ModLoader_LogTwo((const WCHAR*)L"[ASI禁用] 配置保留但本轮不加载：", entry->name);
        if (skipped) ++*skipped;
        return;
    }

    if (!base_to_asi_(file_name, MAX_PATH_, entry->name) || !path_join_(path, YCR_PATH_CAP, g_asi_root, file_name)) {
        ModLoader_LogTwo((const WCHAR*)L"[ASI失败] 文件名/路径过长：", entry->name);
        if (failed) ++*failed;
        return;
    }

    if (!file_exists_(path)) {
        /*
         * 配置条目是用户长期保存的排序/开关记录，文件暂时不存在不等于配置损坏。
         * 因此这里只“跳过本轮加载”，不删除 INI、不移动顺序，也不把它算成 LoadLibrary 失败。
         */
        ModLoader_LogTwo((const WCHAR*)L"[ASI跳过] mods.ini 中有条目，但磁盘文件不存在：", path);
        if (skipped) ++*skipped;
        return;
    }

    /*
     * ASI 主文件在用户界面和 mods.ini 中继续保持便携相对路径，例如：
     *
     *     mods\asi\Controller.asi
     *     mods\asi\SDL3.dll
     *
     * 但是 LOAD_WITH_ALTERED_SEARCH_PATH 有一个重要前提：传给 LoadLibraryExW 的 DLL 路径
     * 必须是完整路径。因此这里只在真正调用 Windows Loader 的最后一步，把相对 ASI 路径转换成绝对路径。
     * 这个绝对路径不会写进配置，也不会改变用户看到的便携目录结构。
     *
     * 为什么这样做：
     *   1. Windows 加载 Controller.asi 时，会把“Controller.asi 自己所在的目录”用于解析它的依赖；
     *   2. 所以 SDL3.dll 等依赖只要和 ASI 一起放在 mods\asi 即可；
     *   3. 不需要把 ASI 的依赖 DLL 放到 mods\ 根目录；Loader 自身两个内部 DLL 的发布位置是另一回事；
     *   4. 即使 dev9 为恢复 dev5 实机兼容性重新保留了 SetDllDirectoryW(mods)，ASI 也不依赖那个全局目录寻找自己的依赖；
     *      每个 ASI 仍优先从自身所在的 mods\asi 解析同目录 DLL。
     *
     * v0.2.1 曾经出现错误 126，是因为当时把相对路径直接交给 LOAD_WITH_ALTERED_SEARCH_PATH。
     * dev9 继续保留这个历史教训：先 GetFullPathNameW，再 LoadLibraryExW。
     */
    {
        WCHAR absolute_path[YCR_PATH_CAP];
        DWORD full_length = GetFullPathNameW(path, YCR_PATH_CAP, absolute_path, NULL_PTR);

        /*
         * GetFullPathNameW 返回 0 表示转换失败；返回值大于等于缓冲区容量表示缓冲区装不下完整路径。
         * 两种情况都不能继续调用 Loader，否则错误日志会把“路径构造失败”和“依赖 DLL 缺失”混在一起。
         */
        if (full_length == 0u || full_length >= YCR_PATH_CAP) {
            ModLoader_LogTwo((const WCHAR*)L"[ASI失败] 无法生成完整加载路径：", path);
            if (failed) ++*failed;
            return;
        }

        module = LoadLibraryExW(absolute_path, NULL_PTR, LOAD_WITH_ALTERED_SEARCH_PATH_);
        if (!module) {
            error = GetLastError();
            ModLoader_LogError((const WCHAR*)L"[ASI失败] LoadLibraryExW 失败", error);
            ModLoader_LogTwo((const WCHAR*)L"[ASI失败] 文件：", path);
            ModLoader_LogTwo((const WCHAR*)L"[ASI失败] 完整路径：", absolute_path);
            if (error == ERROR_MOD_NOT_FOUND_) {
                ModLoader_Log((const WCHAR*)L"[ASI诊断] ASI 主文件已经确认存在；错误 126 通常表示依赖 DLL（或它的下级依赖）缺失。请把该依赖 DLL 放在 mods\\asi，与 ASI 放在同一目录。");
            }
            if (failed) ++*failed;
            return;
        }
    }

    /*
     * LoadLibraryExW 返回时 ASI 的 DllMain 已执行。随后立刻把它的普通运行期 IAT 接入 Locale/Overrides，
     * 再调用可选 InitializeASI；因此插件正式业务初始化会看到和游戏一致的环境。
     */
    OverrideLoader_PatchModule(module);
    LocaleLayer_PatchModule(module);
    ModLoader_LogTwo((const WCHAR*)L"[ASI成功] ", path);
    {
        FARPROC init = GetProcAddress(module, "InitializeASI");
        if (init) {
            typedef void (__cdecl *PFN_InitializeASI_)(void);
            ((PFN_InitializeASI_)init)();
            ModLoader_LogTwo((const WCHAR*)L"[ASI兼容] 已调用 InitializeASI：", entry->name);
        }
    }
    if (loaded) ++*loaded;
}

static void log_config_order_(void) {
    UINT i;
    WCHAR num[32], line[512];

    ModLoader_Log((const WCHAR*)L"[ASI顺序] 以下顺序就是实际 LoadLibrary 顺序：");
    for (i = 0; i < g_asi_count; ++i) {
        u32_to_w_(i + 1u, num, 32u);
        line[0] = 0;
        wcopy_(line, 512u, (const WCHAR*)L"  ");
        wappend_(line, 512u, num);
        wappend_(line, 512u, g_asi_entries[i].enabled ? (const WCHAR*)L" [1] " : (const WCHAR*)L" [0] ");
        wappend_(line, 512u, g_asi_entries[i].name);
        ModLoader_Log(line);
    }

    ModLoader_Log((const WCHAR*)L"[Overrides顺序] 从上到下优先级递增；最下面的启用 Mod 优先级最高：");
    for (i = 0; i < g_override_count; ++i) {
        u32_to_w_(i + 1u, num, 32u);
        line[0] = 0;
        wcopy_(line, 512u, (const WCHAR*)L"  ");
        wappend_(line, 512u, num);
        wappend_(line, 512u, g_override_entries[i].enabled ? (const WCHAR*)L" [1] " : (const WCHAR*)L" [0] ");
        wappend_(line, 512u, g_override_entries[i].name);
        ModLoader_Log(line);
    }
}

/* ---------- 对外生命周期 ---------- */

static int env_flag_enabled_(const WCHAR* name, int default_value) {
    WCHAR value[8];
    DWORD n = GetEnvironmentVariableW(name, value, 8u);
    if (n == 0u || n >= 8u) return default_value;
    if (value[0] == (WCHAR)'0' && value[1] == 0) return 0;
    if (value[0] == (WCHAR)'1' && value[1] == 0) return 1;
    return default_value;
}

int ModLoader_IsGameLogEnabled(void) {
    return g_game_log_enabled;
}

int ModLoader_Begin(HMODULE self_module) {
    WCHAR log_path[YCR_PATH_CAP];
    DWORD wrote = 0;
    static const BYTE bom[3] = {0xEFu, 0xBBu, 0xBFu};

    g_self_module = self_module;
    g_override_detail_logs = 0u;
    g_modloader_log_enabled = env_flag_enabled_((const WCHAR*)L"CASTLE_MODLOADER_LOG_ENABLED", 1);
    g_game_log_enabled = env_flag_enabled_((const WCHAR*)L"CASTLE_GAME_LOG_ENABLED", 1);

    if (!get_mod_root_(self_module)) return 0;
    prepare_roots_();
    if (!g_game_root[0] || !g_mods_root[0] || !g_asi_root[0] || !g_overrides_root[0] || !g_mods_ini[0]) return 0;

    /*
     * dev9 恢复 dev5 已由用户实机验证的进程级 DLL 搜索环境：mods\。
     *
     * dev6～dev8 为了追求“完全不影响 ddraw”删除了这一步，但同一台机器、同一套 cnc-ddraw 的 A/B 结果已经证明：
     * dev5 的转区和截图都正常，而删除后两者同时回归。因此这里优先恢复已验证行为，不再用理论上的“更纯净”
     * 覆盖实机 Oracle。
     *
     * 这并不改变用户 Mod 的目录规范：用户 DLL 仍只允许放在 mods\asi；mods\ 根目录只保留 Loader 自身 DLL、
     * INI 与日志。SetDllDirectoryW(mods) 在这里是兼容环境的一部分，不是允许用户把任意 DLL 塞进根目录。
     *
     * ASI 仍然使用绝对路径 + LOAD_WITH_ALTERED_SEARCH_PATH，所以 ASI 的依赖优先从它自己所在的 mods\asi 解析。
     * 两套机制同时存在：全局环境保持 dev5 兼容性，单个 ASI 又能可靠找到身边的依赖。
     */
    if (!SetDllDirectoryW(g_mods_root)) return 0;

    /*
     * 日志继续归入唯一的外置 mods 根目录。
     *
     * 注意这里仍要确保 mods 目录存在：正常发布包已经在这里放 Bootstrap/Core，
     * 但源码调试或手工整理时目录也可能尚未建立。
     * 如果目录还没建立就直接 CreateFileW("mods\\modloader.log")，Windows 会因为父目录不存在而失败。
     * 这里仍然不创建 asi/overrides 子目录，那些完整目录结构由后面的 ensure_mod_directories_() 统一维护。
     */
    if (!directory_exists_(g_mods_root)) {
        if (!CreateDirectoryW(g_mods_root, NULL_PTR) && GetLastError() != ERROR_ALREADY_EXISTS_) return 0;
    }

    if (!path_join_(log_path, YCR_PATH_CAP, g_mods_root, (const WCHAR*)L"modloader.log")) return 0;

    /*
     * ModLoaderLog=0 时 Core 也完全不打开 modloader.log。ModLoader_Log() 本来就会在 g_log 无效时静默返回，
     * 因此其余初始化代码不需要到处添加“如果日志开启”的分支，业务逻辑和日志逻辑仍然保持分离。
     */
    if (!g_modloader_log_enabled) {
        g_log = INVALID_HANDLE_VALUE_;
        return 1;
    }

    /*
     * 正常启动时，CastleModLoader.exe 已经把本轮日志用 CREATE_ALWAYS 清空并写入 BOM。
     * Core 必须接着写同一份日志，绝不能再次截断，否则 Launcher 阶段的诊断信息会消失。
     * 只有在开发者单独加载 Core、没有 CASTLE_PRELOADER_LOG_READY=1 时，才由 Core 自己创建全新日志。
     */
    {
        WCHAR ready[8];
        DWORD ready_n = GetEnvironmentVariableW((const WCHAR*)L"CASTLE_PRELOADER_LOG_READY", ready, 8u);
        DWORD creation = (ready_n > 0u && ready_n < 8u && ready[0] == (WCHAR)'1') ? OPEN_ALWAYS_ : CREATE_ALWAYS_;
        g_log = CreateFileW(log_path, GENERIC_WRITE_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_, NULL_PTR,
                            creation, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
        if (g_log != INVALID_HANDLE_VALUE_) {
            if (creation == OPEN_ALWAYS_) {
                SetFilePointer(g_log, 0, NULL_PTR, FILE_END_);
            } else {
                WriteFile(g_log, bom, 3u, &wrote, NULL_PTR);
            }
            ModLoader_Log((const WCHAR*)L"《幽城幻剑录》Mod Core v0.3.0-dev9 已进入 RPG.exe。");
        } else {
            return 0;
        }
    }
    return 1;
}

int ModLoader_PrepareOverrides(void) {
    UINT i;
    UINT hook_slots;
    WCHAR pid[32], num[32];

    ModLoader_LogTwo((const WCHAR*)L"[目录] 唯一 Mod 根：", g_mod_root);
    ModLoader_LogTwo((const WCHAR*)L"[目录] 游戏安装根：", g_game_root);
    u32_to_w_(GetCurrentProcessId(), pid, 32u);
    ModLoader_LogTwo((const WCHAR*)L"[进程] PID=", pid);

    if (!ensure_mod_directories_()) {
        ModLoader_Log((const WCHAR*)L"[错误] 无法建立 mods/asi/overrides/模板目录，停止 Mod 初始化。");
        return 0;
    }
    ModLoader_LogTwo((const WCHAR*)L"[模板] Overrides 保留模板：", (const WCHAR*)OVERRIDE_TEMPLATE_NAME_);
    ModLoader_Log((const WCHAR*)L"[路径规则] ASI 只从 mods\\asi 加载；文件 Mod 只从 mods\\overrides 加载；根目录/scripts/plugins/update 均无效。");

    /*
     * “自动发现”是正式启动步骤，不是只有首次生成 INI 才做。
     * 现有 mods.ini 存在时，也先扫描磁盘，再把遗漏的新 ASI/Overrides 追加到对应节最末尾，默认 =1。
     */
    log_discovery_snapshot_();
    if (!ensure_and_read_config_()) {
        ModLoader_Log((const WCHAR*)L"[错误] mods.ini 生成/读取/自动补全失败；不继续加载 Mod。");
        return 0;
    }
    log_config_order_();

    {
        WCHAR game_exe_dir[YCR_PATH_CAP];
        DWORD game_dir_n = GetEnvironmentVariableW((const WCHAR*)L"CASTLE_GAME_EXE_DIR", game_exe_dir, YCR_PATH_CAP);
        if (game_dir_n > 0u && game_dir_n < YCR_PATH_CAP)
            OverrideLoader_Reset(g_mods_root, game_exe_dir, g_game_root);
        else
            OverrideLoader_Reset(g_mods_root, g_game_root, g_game_root);
    }
    /*
     * 配置文件是“用户的长期排序/开关记录”，磁盘目录则是“本轮真正可用的 Mod”。
     * 两者不能混为一谈：用户把一个 Overrides Mod 暂时移走时，我们不应该擅自删掉 INI 条目，
     * 但也绝不能继续把这个不存在的目录算进“启用文件夹 Mod 数量”。
     *
     * 因此这里逐项检查真实目录：
     *   - enabled=0：继续保留排序信息，但本轮不加入活跃表；
     *   - enabled=1 且目录存在：加入活跃表；
     *   - enabled=1 但目录不存在：只写清楚缺失日志，保留 INI，等待用户以后把目录放回来。
     */
    for (i = 0; i < g_override_count; ++i) {
        WCHAR override_path[YCR_PATH_CAP];
        if (!g_override_entries[i].enabled) {
            OverrideLoader_Add(g_override_entries[i].name, 0);
            continue;
        }
        if (!path_join_(override_path, YCR_PATH_CAP, g_overrides_root, g_override_entries[i].name) ||
            !directory_exists_(override_path)) {
            ModLoader_LogTwo((const WCHAR*)L"[Overrides跳过] mods.ini 中有条目，但磁盘目录不存在：", g_override_entries[i].name);
            continue;
        }
        if (!override_dir_contains_file_(override_path, 0u)) {
            ModLoader_LogTwo((const WCHAR*)L"[Overrides跳过] 目录存在但递归没有任何普通文件：", g_override_entries[i].name);
            continue;
        }
        OverrideLoader_Add(g_override_entries[i].name, 1);
    }

    hook_slots = OverrideLoader_Install();
    u32_to_w_(OverrideLoader_EnabledCount(), num, 32u);
    ModLoader_LogTwo((const WCHAR*)L"[Overrides] 启用文件夹 Mod 数量=", num);
    u32_to_w_(hook_slots, num, 32u);
    ModLoader_LogTwo((const WCHAR*)L"[Overrides] 已改写文件 API IAT 槽位数量=", num);
    ModLoader_Log((const WCHAR*)L"[阶段完成] Overrides 基础层已先于 Locale 与 ASI 准备完成。");
    return 1;
}

void ModLoader_LoadAsi(void) {
    UINT i;
    UINT loaded = 0u, skipped = 0u, failed = 0u;
    WCHAR num[32], line[512];

    for (i = 0; i < g_asi_count; ++i)
        load_one_asi_(&g_asi_entries[i], &loaded, &skipped, &failed);

    line[0] = 0;
    wcopy_(line, 512u, (const WCHAR*)L"[完成] ASI 加载：成功=");
    u32_to_w_(loaded, num, 32u); wappend_(line, 512u, num);
    wappend_(line, 512u, (const WCHAR*)L"，跳过=");
    u32_to_w_(skipped, num, 32u); wappend_(line, 512u, num);
    wappend_(line, 512u, (const WCHAR*)L"，失败=");
    u32_to_w_(failed, num, 32u); wappend_(line, 512u, num);
    ModLoader_Log(line);
}

void ModLoader_Shutdown(void) {
    if (g_log != INVALID_HANDLE_VALUE_) {
        ModLoader_Log((const WCHAR*)L"[退出] Mod Loader 随进程结束，关闭日志。");
        CloseHandle(g_log);
        g_log = INVALID_HANDLE_VALUE_;
    }
}
