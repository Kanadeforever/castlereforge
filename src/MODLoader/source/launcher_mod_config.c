#include "platform.h"
#include "launcher_mod_config.h"

/*
 * CastleModLoader.exe 的 GUI 配置层。
 *
 * 这个文件只处理“GUI 打开以后，用户在看什么、改什么、怎样安全写回 mods.ini”。
 * 游戏进程里的最终加载仍由 mod_loader.c 决定。两边刻意使用同一组规则：
 *
 *   ASI：
 *     - INI 有、文件没有：保留条目，本轮跳过；
 *     - 文件有、INI 没有：自然排序后追加，默认启用；
 *
 *   Overrides：
 *     - 模板目录永远忽略；
 *     - INI 有、目录没有：保留条目，本轮跳过；
 *     - 目录存在但递归没有普通文件：保留旧条目，但新空目录绝不自动加入；
 *     - 新的有效目录自然排序后追加，默认启用。
 *
 * GUI 不能为了“方便编辑”重新发明另一套配置格式，否则最危险的结果就是：界面看起来启用了，
 * Core 却按另一种顺序读取。因此这里宁可代码多一点，也要把自动扫描、缺失保留、空目录过滤、
 * 256 项上限和原子写回全部做成与 Core 一样明确的规则。
 */

#define DOC_MAX_LINES_ 1024u
#define DOC_LINE_CAP_ 512u
#define INI_BYTE_CAP_ 65535u
#define DIR_STACK_MAX_ 64u
#define OVERRIDE_TEMPLATE_NAME_ L"模板_复制后改名"

typedef enum DocSection_ {
    DOC_SECTION_NONE_ = 0,
    DOC_SECTION_ASI_ = 1,
    DOC_SECTION_OVERRIDE_ = 2,
    DOC_SECTION_OTHER_ = 3
} DocSection_;

typedef struct DocLine_ {
    WCHAR text[DOC_LINE_CAP_];
} DocLine_;

typedef struct ScanName_ {
    WCHAR name[MAX_PATH_];
} ScanName_;

static DocLine_ g_lines[DOC_MAX_LINES_];
static UINT g_line_count;
static LauncherModItem_ g_asi[LAUNCHER_MOD_MAX_ITEMS];
static LauncherModItem_ g_overrides[LAUNCHER_MOD_MAX_ITEMS];
static UINT g_asi_count;
static UINT g_override_count;
/* 历史配置可能不只重复 section，也可能真的重复同一个 Mod 键。Core 规定第一次为准；拖动时若仍静默重排会让后面的旧重复键意外变成第一次。 */
static int g_has_duplicate_entries;
static ScanName_ g_scan[LAUNCHER_MOD_MAX_ITEMS];
static UINT g_scan_count;
static BYTE g_ini_bytes[INI_BYTE_CAP_ + 4u];
static WCHAR g_ini_wide[INI_BYTE_CAP_ + 4u];
static char g_utf8[4096u];
static WCHAR g_last_error[512u];
static WCHAR g_dir_stack[DIR_STACK_MAX_][CASTLE_PATH_CAP];

/* ---------- 最小字符串与路径工具 ---------- */

static UINT wlen_(const WCHAR* s) {
    UINT n = 0u;
    if (!s) return 0u;
    while (s[n]) ++n;
    return n;
}

static WCHAR fold_ascii_(WCHAR c) {
    if (c >= (WCHAR)'A' && c <= (WCHAR)'Z') return (WCHAR)(c + ((WCHAR)'a' - (WCHAR)'A'));
    return c;
}

static int weq_i_(const WCHAR* a, const WCHAR* b) {
    UINT i = 0u;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        if (fold_ascii_(a[i]) != fold_ascii_(b[i])) return 0;
        ++i;
    }
    return a[i] == 0 && b[i] == 0;
}

static int wcopy_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT i = 0u;
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
    UINT d = wlen_(dst), i = 0u;
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

static WCHAR* trim_left_(WCHAR* s) {
    while (*s == (WCHAR)' ' || *s == (WCHAR)'\t') ++s;
    return s;
}

static void trim_right_(WCHAR* s) {
    UINT n = wlen_(s);
    while (n && (s[n - 1u] == (WCHAR)' ' || s[n - 1u] == (WCHAR)'\t')) s[--n] = 0;
}

static int file_exists_(const WCHAR* path) {
    /* GetFileAttributesW 成功且不是目录，才算可以当 ASI/普通文件使用；不存在或访问失败都返回 0。 */
    DWORD a = GetFileAttributesW(path);
    return a != INVALID_FILE_ATTRIBUTES_ && !(a & FILE_ATTRIBUTE_DIRECTORY_);
}

static int directory_exists_(const WCHAR* path) {
    /* 和 file_exists_ 相反，这里必须明确带 FILE_ATTRIBUTE_DIRECTORY 才算目录。 */
    DWORD a = GetFileAttributesW(path);
    return a != INVALID_FILE_ATTRIBUTES_ && (a & FILE_ATTRIBUTE_DIRECTORY_) != 0;
}

static int ensure_directory_(const WCHAR* path) {
    /* 已存在就直接成功；不存在则创建。并发情况下别人刚创建导致 ERROR_ALREADY_EXISTS 也按成功处理。 */
    if (directory_exists_(path)) return 1;
    if (CreateDirectoryW(path, NULL_PTR)) return 1;
    return GetLastError() == ERROR_ALREADY_EXISTS_ && directory_exists_(path);
}

static void clear_error_(void) {
    g_last_error[0] = 0;
}

static void set_error_(const WCHAR* text) {
    wcopy_(g_last_error, (UINT)(sizeof(g_last_error) / sizeof(g_last_error[0])), text ? text : (const WCHAR*)L"未知错误。");
}

const WCHAR* LauncherModConfig_GetLastErrorText(void) {
    return g_last_error;
}

/* ---------- 名称规则与自然排序 ---------- */

static int ends_asi_(const WCHAR* name) {
    UINT n = wlen_(name);
    if (n < 4u) return 0;
    return name[n - 4u] == (WCHAR)'.' &&
           fold_ascii_(name[n - 3u]) == (WCHAR)'a' &&
           fold_ascii_(name[n - 2u]) == (WCHAR)'s' &&
           fold_ascii_(name[n - 1u]) == (WCHAR)'i';
}

static int valid_plain_name_(const WCHAR* name) {
    UINT i;
    if (!name || !name[0]) return 0;
    if ((name[0] == (WCHAR)'.' && name[1] == 0) ||
        (name[0] == (WCHAR)'.' && name[1] == (WCHAR)'.' && name[2] == 0)) return 0;
    for (i = 0u; name[i]; ++i)
        if (name[i] == (WCHAR)'\\' || name[i] == (WCHAR)'/' || name[i] == (WCHAR)':') return 0;
    return 1;
}

static int valid_asi_base_(const WCHAR* name) {
    return valid_plain_name_(name) && !ends_asi_(name);
}

static int valid_override_name_(const WCHAR* name) {
    return valid_plain_name_(name) && !weq_i_(name, (const WCHAR*)OVERRIDE_TEMPLATE_NAME_);
}

static int asi_to_base_(WCHAR* out, UINT cap, const WCHAR* file_name) {
    UINT n = wlen_(file_name), i;
    if (!ends_asi_(file_name) || n <= 4u || n - 4u + 1u > cap) return 0;
    for (i = 0u; i < n - 4u; ++i) out[i] = file_name[i];
    out[n - 4u] = 0;
    return 1;
}

static int natural_cmp_(const WCHAR* a, const WCHAR* b) {
    /*
     * “自然排序”让 Mod2 排在 Mod10 前，而不是按纯字符比较得到 Mod10、Mod2。数字段先跳过前导 0，
     * 再按有效数字长度和值比较；非数字字符只做 ASCII 大小写折叠，中文等字符保持 Unicode 码点顺序。
     */
    UINT ia = 0u, ib = 0u;
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
            for (k = 0u; k < lena; ++k)
                if (a[za + k] != b[zb + k]) return a[za + k] < b[zb + k] ? -1 : 1;
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

static void sort_scan_(void) {
    UINT i, j;
    WCHAR temp[MAX_PATH_];
    for (i = 1u; i < g_scan_count; ++i) {
        wcopy_(temp, MAX_PATH_, g_scan[i].name);
        j = i;
        while (j > 0u && natural_cmp_(temp, g_scan[j - 1u].name) < 0) {
            wcopy_(g_scan[j].name, MAX_PATH_, g_scan[j - 1u].name);
            --j;
        }
        wcopy_(g_scan[j].name, MAX_PATH_, temp);
    }
}

/* ---------- Overrides “递归至少含一个普通文件”判断 ---------- */

static int directory_has_regular_file_recursive_(const WCHAR* root) {
    /*
     * Overrides 是否“为空”不能只看第一层：一个正常 Mod 可能只有 MultiMedia\Map\xxx 这种深层文件。
     * 因此这里用显式目录栈做深度遍历；一旦看到任何非目录项立即返回 1，不需要把全部文件枚举完。
     */
    UINT top = 0u;
    if (!directory_exists_(root)) return 0;
    if (!wcopy_(g_dir_stack[top++], CASTLE_PATH_CAP, root)) return 0;

    while (top > 0u) {
        /* 每轮弹出一个待检查目录，枚举它的直接子项；子目录重新压栈，普通文件则证明整个 Mod 非空。 */
        WCHAR current[CASTLE_PATH_CAP];
        WCHAR pattern[CASTLE_PATH_CAP];
        WIN32_FIND_DATAW_ data;
        HANDLE find;

        --top;
        if (!wcopy_(current, CASTLE_PATH_CAP, g_dir_stack[top])) return 0;
        if (!path_join_(pattern, CASTLE_PATH_CAP, current, (const WCHAR*)L"*")) continue;

        find = FindFirstFileW(pattern, &data);
        if (find == INVALID_HANDLE_VALUE_) continue;
        do {
            if (data.cFileName[0] == (WCHAR)'.' &&
                (data.cFileName[1] == 0 || (data.cFileName[1] == (WCHAR)'.' && data.cFileName[2] == 0))) continue;

            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY_) {
                if (top < DIR_STACK_MAX_) {
                    if (path_join_(g_dir_stack[top], CASTLE_PATH_CAP, current, data.cFileName)) ++top;
                }
            } else {
                FindClose(find);
                return 1;
            }
        } while (FindNextFileW(find, &data));
        FindClose(find);
    }
    return 0;
}

/* ---------- 模板目录：只在完全不存在时建立一次 ---------- */

static int ensure_override_template_(const WCHAR* overrides_root) {
    /*
     * 模板是给用户复制的目录树，不是一个真正 Mod。规则要求“完全不存在才创建；只要根目录存在就不碰”，
     * 所以这里不能每次启动补目录，否则用户删掉某个模板子目录也会被 Loader 自作主张恢复。
     */
    static const WCHAR* dirs[] = {
        L"exe", L"MultiMedia", L"MultiMedia\\fight", L"MultiMedia\\Font", L"MultiMedia\\Map",
        L"MultiMedia\\menus", L"MultiMedia\\Mov", L"MultiMedia\\Music", L"MultiMedia\\public",
        L"MultiMedia\\save", L"MultiMedia\\Sys"
    };
    WCHAR root[CASTLE_PATH_CAP];
    WCHAR path[CASTLE_PATH_CAP];
    UINT i;

    if (!path_join_(root, CASTLE_PATH_CAP, overrides_root, (const WCHAR*)OVERRIDE_TEMPLATE_NAME_)) return 0;
    if (directory_exists_(root)) return 1;
    if (!ensure_directory_(root)) return 0;

    for (i = 0u; i < (UINT)(sizeof(dirs) / sizeof(dirs[0])); ++i) {
        if (!path_join_(path, CASTLE_PATH_CAP, root, dirs[i])) return 0;
        if (!ensure_directory_(path)) return 0;
    }
    return 1;
}

/* ---------- mods.ini 文档读取 ---------- */

static int decode_ini_(DWORD bytes, WCHAR** out) {
    /*
     * 输入配置可能来自旧版：先识别 UTF-16LE BOM，再尝试严格 UTF-8，最后才用系统 ACP 兼容旧文件。
     * 一旦解码成功，后续所有解析都只处理 WCHAR，避免在“Mod 名字”层继续混用多种字节编码。
     */
    int chars;
    DWORD i;
    if (!out) return 0;
    *out = NULL_PTR;
    if (bytes == 0u) {
        g_ini_wide[0] = 0;
        *out = g_ini_wide;
        return 1;
    }

    if (bytes >= 2u && g_ini_bytes[0] == 0xFFu && g_ini_bytes[1] == 0xFEu) {
        DWORD n = 0u;
        for (i = 2u; i + 1u < bytes && n + 1u < INI_BYTE_CAP_; i += 2u)
            g_ini_wide[n++] = (WCHAR)((UINT)g_ini_bytes[i] | ((UINT)g_ini_bytes[i + 1u] << 8));
        g_ini_wide[n] = 0;
        *out = g_ini_wide;
        return 1;
    }

    {
        DWORD skip = (bytes >= 3u && g_ini_bytes[0] == 0xEFu && g_ini_bytes[1] == 0xBBu && g_ini_bytes[2] == 0xBFu) ? 3u : 0u;
        chars = MultiByteToWideChar(CP_UTF8_, MB_ERR_INVALID_CHARS_, (LPCSTR)(g_ini_bytes + skip),
                                    (int)(bytes - skip), g_ini_wide, (int)(INI_BYTE_CAP_ - 1u));
        if (chars <= 0)
            chars = MultiByteToWideChar(CP_ACP_, 0u, (LPCSTR)(g_ini_bytes + skip),
                                        (int)(bytes - skip), g_ini_wide, (int)(INI_BYTE_CAP_ - 1u));
        if (chars <= 0) return 0;
        g_ini_wide[chars] = 0;
        *out = g_ini_wide;
        return 1;
    }
}

static int load_document_(const WCHAR* ini_path) {
    /*
     * 读取时把 mods.ini 保存成“原始行数组”，而不是只提取键值。这是为了 GUI 改顺序时仍能保留用户注释、
     * 空行、未知 section 和历史内容；真正的 Mod 模型稍后再从这些行里解析出来。
     */
    HANDLE file;
    DWORD size, got = 0u;
    WCHAR* text;
    WCHAR* p;

    g_line_count = 0u;
    if (!file_exists_(ini_path)) return 1;

    file = CreateFileW(ini_path, GENERIC_READ_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_ | FILE_SHARE_DELETE_,
                       NULL_PTR, OPEN_EXISTING_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) return 0;
    size = GetFileSize(file, NULL_PTR);
    if (size == 0xFFFFFFFFu || size > INI_BYTE_CAP_) { CloseHandle(file); return 0; }
    if (size && (!ReadFile(file, g_ini_bytes, size, &got, NULL_PTR) || got != size)) { CloseHandle(file); return 0; }
    CloseHandle(file);
    if (!decode_ini_(size, &text)) return 0;

    p = text;
    while (*p) {
        /* 一次找到一行的结尾，复制正文但不保存 CR/LF；写回时统一使用 CRLF，避免混合换行越来越乱。 */
        WCHAR* line = p;
        UINT n;
        while (*p && *p != (WCHAR)'\r' && *p != (WCHAR)'\n') ++p;
        n = (UINT)(p - line);
        if (g_line_count >= DOC_MAX_LINES_ || n + 1u > DOC_LINE_CAP_) return 0;
        {
            UINT i;
            for (i = 0u; i < n; ++i) g_lines[g_line_count].text[i] = line[i];
            g_lines[g_line_count].text[n] = 0;
        }
        ++g_line_count;
        if (*p == (WCHAR)'\r') ++p;
        if (*p == (WCHAR)'\n') ++p;
    }

    /* 文件若以换行结尾，不额外制造一个“虚构空行”；保存时每一行统一写 CRLF。 */
    return 1;
}

static DocSection_ section_from_header_(const WCHAR* line) {
    WCHAR temp[DOC_LINE_CAP_];
    WCHAR* t;
    if (!wcopy_(temp, DOC_LINE_CAP_, line)) return DOC_SECTION_OTHER_;
    t = trim_left_(temp);
    trim_right_(t);
    if (weq_i_(t, (const WCHAR*)L"[ASI]")) return DOC_SECTION_ASI_;
    if (weq_i_(t, (const WCHAR*)L"[Overrides]")) return DOC_SECTION_OVERRIDE_;
    if (t[0] == (WCHAR)'[') return DOC_SECTION_OTHER_;
    return DOC_SECTION_NONE_;
}

static int item_has_name_(LauncherModItem_* items, UINT count, const WCHAR* name) {
    UINT i;
    for (i = 0u; i < count; ++i) if (weq_i_(items[i].name, name)) return 1;
    return 0;
}

static int add_item_(LauncherModItem_* items, UINT* count, const WCHAR* name, int enabled) {
    if (*count >= LAUNCHER_MOD_MAX_ITEMS) return 0;
    if (item_has_name_(items, *count, name)) {
        /*
         * Core 的既定规则是“最上面第一次为准”。GUI 仍保留后续重复原文，但记住这里存在重复键；
         * 后面如果用户尝试拖动，我们会拒绝重排，防止旧重复行因为位置关系变化而突然接管配置。
         */
        g_has_duplicate_entries = 1;
        return 1;
    }
    if (!wcopy_(items[*count].name, MAX_PATH_, name)) return 0;
    items[*count].enabled = enabled ? 1 : 0;
    items[*count].present = 0;
    items[*count].usable = 0;
    items[*count].has_toml = 0;
    ++*count;
    return 1;
}

static int parse_entry_line_(const WCHAR* line, DocSection_ section, WCHAR* name_out, UINT name_cap, int* enabled_out) {
    WCHAR temp[DOC_LINE_CAP_];
    WCHAR* p;
    WCHAR* eq;
    WCHAR* value;
    if (!line || !name_out || !enabled_out || section == DOC_SECTION_NONE_ || section == DOC_SECTION_OTHER_) return 0;
    if (!wcopy_(temp, DOC_LINE_CAP_, line)) return 0;
    p = trim_left_(temp);
    trim_right_(p);
    if (!p[0] || p[0] == (WCHAR)';' || p[0] == (WCHAR)'#' || p[0] == (WCHAR)'[') return 0;

    eq = p;
    while (*eq && *eq != (WCHAR)'=') ++eq;
    if (*eq != (WCHAR)'=') return 0;
    *eq++ = 0;
    trim_right_(p);
    value = trim_left_(eq);
    trim_right_(value);
    if (!((value[0] == (WCHAR)'0' || value[0] == (WCHAR)'1') && value[1] == 0)) return 0;

    if (section == DOC_SECTION_ASI_) {
        if (!valid_asi_base_(p)) return 0;
    } else {
        if (!valid_override_name_(p)) return 0;
    }
    if (!wcopy_(name_out, name_cap, p)) return 0;
    *enabled_out = value[0] == (WCHAR)'1';
    return 1;
}

static int rebuild_model_from_document_(void) {
    /*
     * 行数组是“原文”，模型数组是 GUI 真正显示的顺序。只有 [ASI]/[Overrides] 下合法的 name=0/1 才进入模型；
     * 同名重复键沿用 Core 的“第一次有效条目为准”，后面的重复行仍保留在原文，但不制造第二个 GUI 项目。
     */
    DocSection_ section = DOC_SECTION_NONE_;
    UINT i;
    WCHAR name[MAX_PATH_];
    int enabled;

    g_asi_count = 0u;
    g_override_count = 0u;
    g_has_duplicate_entries = 0;

    for (i = 0u; i < g_line_count; ++i) {
        DocSection_ header = section_from_header_(g_lines[i].text);
        if (header != DOC_SECTION_NONE_) {
            section = header;
            continue;
        }
        if (!parse_entry_line_(g_lines[i].text, section, name, MAX_PATH_, &enabled)) continue;
        if (section == DOC_SECTION_ASI_) {
            if (!add_item_(g_asi, &g_asi_count, name, enabled)) return 0;
        } else if (section == DOC_SECTION_OVERRIDE_) {
            if (!add_item_(g_overrides, &g_override_count, name, enabled)) return 0;
        }
    }
    return 1;
}

/* ---------- 文档增补：只向最后一个已有同名节插入；没有节才创建一次 ---------- */

static int insert_line_(UINT at, const WCHAR* text) {
    /* 在固定容量数组中插入一行：先从末尾倒着搬，最后写新行，防止前向移动把还没复制的数据覆盖掉。 */
    UINT i;
    if (g_line_count >= DOC_MAX_LINES_ || at > g_line_count) return 0;
    for (i = g_line_count; i > at; --i) g_lines[i] = g_lines[i - 1u];
    if (!wcopy_(g_lines[at].text, DOC_LINE_CAP_, text)) return 0;
    ++g_line_count;
    return 1;
}

static int line_is_blank_(const WCHAR* text) {
    /*
     * g_lines 已经把换行符去掉，所以这里只需要跳过空格和制表符。
     * 跳完就到字符串结尾，说明这一行在人眼看来是空行，也应该继续留在两个 INI 节之间充当分隔符。
     */
    if (!text) return 0;
    while (*text == (WCHAR)' ' || *text == (WCHAR)'\t') ++text;
    return *text == 0;
}

static int append_entry_to_section_(LauncherModKind_ kind, const WCHAR* name, int enabled) {
    /*
     * 新自动发现项必须复用“最后一个已有同名 section”。只有整份文件都没有该 section 才创建表头，
     * 这样可以停止旧版本每次发现新 Mod 都追加一个新 [ASI]/[Overrides] 的遗留问题。
     */
    const DocSection_ target = kind == LAUNCHER_MOD_ASI ? DOC_SECTION_ASI_ : DOC_SECTION_OVERRIDE_;
    const WCHAR* header_text = kind == LAUNCHER_MOD_ASI ? (const WCHAR*)L"[ASI]" : (const WCHAR*)L"[Overrides]";
    int last_header = -1;
    UINT i, insert_at;
    WCHAR line[DOC_LINE_CAP_];

    for (i = 0u; i < g_line_count; ++i) {
        if (section_from_header_(g_lines[i].text) == target) last_header = (int)i;
    }

    if (last_header < 0) {
        if (g_line_count && g_lines[g_line_count - 1u].text[0]) {
            if (!insert_line_(g_line_count, (const WCHAR*)L"")) return 0;
        }
        if (!insert_line_(g_line_count, header_text)) return 0;
        last_header = (int)(g_line_count - 1u);
    }

    /* 从最后一个表头向下走到下一个 section，先找到当前节在文档中的完整物理范围。 */
    insert_at = (UINT)last_header + 1u;
    while (insert_at < g_line_count) {
        DocSection_ h = section_from_header_(g_lines[insert_at].text);
        if (h != DOC_SECTION_NONE_) break;
        ++insert_at;
    }

    /*
     * 再从范围末尾越过连续空行，把条目放到这些分隔空行之前。
     * 例如“[ASI]、空行、[Overrides]”会变成“[ASI]、新条目、空行、[Overrides]”，而不是把空行留在表头下。
     * 每次追加都重新做这一步，因此一次扫描发现多个新 Mod 时，它们仍按扫描顺序连续排列在空行前面。
     */
    while (insert_at > (UINT)last_header + 1u && line_is_blank_(g_lines[insert_at - 1u].text)) --insert_at;

    line[0] = 0;
    if (!wcopy_(line, DOC_LINE_CAP_, name) || !wappend_(line, DOC_LINE_CAP_, enabled ? (const WCHAR*)L"=1" : (const WCHAR*)L"=0")) return 0;
    return insert_line_(insert_at, line);
}

/* ---------- 磁盘扫描 ---------- */

static UINT scan_asi_(const WCHAR* asi_root) {
    /* 枚举磁盘真实 *.asi，只收普通文件；去掉扩展名后进入扫描表，最后做自然排序再和 INI 模型比较。 */
    WCHAR pattern[CASTLE_PATH_CAP];
    WIN32_FIND_DATAW_ data;
    HANDLE find;
    g_scan_count = 0u;
    if (!path_join_(pattern, CASTLE_PATH_CAP, asi_root, (const WCHAR*)L"*.asi")) return 0u;
    find = FindFirstFileW(pattern, &data);
    if (find == INVALID_HANDLE_VALUE_) return 0u;
    do {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY_) && ends_asi_(data.cFileName) && g_scan_count < LAUNCHER_MOD_MAX_ITEMS) {
            if (asi_to_base_(g_scan[g_scan_count].name, MAX_PATH_, data.cFileName)) ++g_scan_count;
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    sort_scan_();
    return g_scan_count;
}

static UINT scan_overrides_(const WCHAR* overrides_root) {
    /*
     * Overrides 只收目录；模板、点目录和递归没有普通文件的空目录全部过滤。真正有效的新目录再自然排序，
     * 这样同时放进多个 Mod 时写进 INI 的结果稳定、可预测。
     */
    WCHAR pattern[CASTLE_PATH_CAP];
    WCHAR full[CASTLE_PATH_CAP];
    WIN32_FIND_DATAW_ data;
    HANDLE find;
    g_scan_count = 0u;
    if (!path_join_(pattern, CASTLE_PATH_CAP, overrides_root, (const WCHAR*)L"*")) return 0u;
    find = FindFirstFileW(pattern, &data);
    if (find == INVALID_HANDLE_VALUE_) return 0u;
    do {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY_)) continue;
        if (data.cFileName[0] == (WCHAR)'.' &&
            (data.cFileName[1] == 0 || (data.cFileName[1] == (WCHAR)'.' && data.cFileName[2] == 0))) continue;
        if (!valid_override_name_(data.cFileName)) continue;
        if (!path_join_(full, CASTLE_PATH_CAP, overrides_root, data.cFileName)) continue;
        /* 新空目录不算 Mod，所以扫描阶段直接略过。已有空条目稍后由状态刷新保留。 */
        if (!directory_has_regular_file_recursive_(full)) continue;
        if (g_scan_count < LAUNCHER_MOD_MAX_ITEMS && wcopy_(g_scan[g_scan_count].name, MAX_PATH_, data.cFileName)) ++g_scan_count;
    } while (FindNextFileW(find, &data));
    FindClose(find);
    sort_scan_();
    return g_scan_count;
}

static void refresh_item_presence_(const WCHAR* asi_root, const WCHAR* overrides_root) {
    /*
     * “enabled”来自用户配置；“present/usable”来自当前磁盘，三者故意分开。缺失项目仍能保持 enabled=1，
     * 等文件以后重新放回去即可按原设置恢复，而不是 Loader 擅自把它改成 0。
     */
    UINT i;
    WCHAR path[CASTLE_PATH_CAP];
    WCHAR file_name[MAX_PATH_];

    for (i = 0u; i < g_asi_count; ++i) {
        /*
         * 第一步检查真正的 ASI 文件。name 在配置里不带扩展名，所以这里临时拼出“名称.asi”。
         * present/usable 仍保持原有语义：只有主 ASI 文件存在时，本轮才可能加载。
         */
        file_name[0] = 0;
        wcopy_(file_name, MAX_PATH_, g_asi[i].name);
        wappend_(file_name, MAX_PATH_, (const WCHAR*)L".asi");
        if (path_join_(path, CASTLE_PATH_CAP, asi_root, file_name)) {
            g_asi[i].present = file_exists_(path);
            g_asi[i].usable = g_asi[i].present;
        }

        /*
         * 第二步只检查“同名 TOML”：Castle_PadSupport.asi 只对应 Castle_PadSupport.toml。
         * 不能为了显示编辑按钮去猜别的配置文件名，否则一个目录里有多个插件时很容易把 A 插件的设置打开成 B 插件的。
         * 即使 ASI 主文件暂时缺失，也仍把 has_toml 按磁盘事实记录下来；GUI 会在绘制时要求 present=1 才显示编辑按钮，
         * 这样模型信息完整，同时不会在“文件缺失”行上堆无意义动作。
         */
        g_asi[i].has_toml = 0;
        file_name[0] = 0;
        if (wcopy_(file_name, MAX_PATH_, g_asi[i].name) &&
            wappend_(file_name, MAX_PATH_, (const WCHAR*)L".toml") &&
            path_join_(path, CASTLE_PATH_CAP, asi_root, file_name)) {
            g_asi[i].has_toml = file_exists_(path);
        }
    }

    for (i = 0u; i < g_override_count; ++i) {
        /* Overrides 只代表一个目录树，不参与“同名 TOML 插件设置”功能。 */
        g_overrides[i].has_toml = 0;
        if (path_join_(path, CASTLE_PATH_CAP, overrides_root, g_overrides[i].name)) {
            g_overrides[i].present = directory_exists_(path);
            g_overrides[i].usable = g_overrides[i].present && directory_has_regular_file_recursive_(path);
        }
    }
}

/* ---------- 把当前模型顺序/开关映射回原文中的“有效条目槽” ---------- */

static int rewrite_section_entry_slots_(LauncherModKind_ kind) {
    /*
     * 拖动或切换开关后，模型顺序已经改变。这里把模型重新写回“第一批有效条目槽”，不碰注释和未知行。
     * 历史重复键仍遵循 Core 的第一次有效项语义；当前 GUI 不主动清洗旧配置，避免未经用户同意的大范围整理。
     */
    DocSection_ section = DOC_SECTION_NONE_;
    const DocSection_ target = kind == LAUNCHER_MOD_ASI ? DOC_SECTION_ASI_ : DOC_SECTION_OVERRIDE_;
    LauncherModItem_* items = kind == LAUNCHER_MOD_ASI ? g_asi : g_overrides;
    UINT count = kind == LAUNCHER_MOD_ASI ? g_asi_count : g_override_count;
    WCHAR seen[LAUNCHER_MOD_MAX_ITEMS][MAX_PATH_];
    UINT seen_count = 0u;
    UINT model_index = 0u;
    UINT i, j;

    for (i = 0u; i < g_line_count; ++i) {
        WCHAR name[MAX_PATH_];
        WCHAR out[DOC_LINE_CAP_];
        int enabled;
        int duplicate = 0;
        DocSection_ h = section_from_header_(g_lines[i].text);
        if (h != DOC_SECTION_NONE_) {
            section = h;
            continue;
        }
        if (section != target || !parse_entry_line_(g_lines[i].text, section, name, MAX_PATH_, &enabled)) continue;
        for (j = 0u; j < seen_count; ++j) if (weq_i_(seen[j], name)) { duplicate = 1; break; }
        if (duplicate) continue;
        if (seen_count < LAUNCHER_MOD_MAX_ITEMS) wcopy_(seen[seen_count++], MAX_PATH_, name);

        if (model_index >= count) return 0;
        out[0] = 0;
        if (!wcopy_(out, DOC_LINE_CAP_, items[model_index].name) ||
            !wappend_(out, DOC_LINE_CAP_, items[model_index].enabled ? (const WCHAR*)L"=1" : (const WCHAR*)L"=0")) return 0;
        if (!wcopy_(g_lines[i].text, DOC_LINE_CAP_, out)) return 0;
        ++model_index;
    }
    return model_index == count;
}


/*
 * 从原始行文档中删除某一种 Mod 的指定名称。
 *
 * 这里不能只删“当前第一条”，因为旧 mods.ini 可能有同名重复键，而 Core 一直遵循“第一次有效条目为准”。
 * 如果只删第一条，后面的重复键会立刻变成新的第一条，GUI 看起来就像“移除了又自己回来”。因此只要名称匹配，
 * 该类型 section 下所有合法的同名键都会一起删除；注释、空行、未知 section、其它 Mod 条目都原样保留。
 */
static UINT remove_document_entries_by_name_(LauncherModKind_ kind, const WCHAR* target_name) {
    DocSection_ section = DOC_SECTION_NONE_;
    const DocSection_ target = kind == LAUNCHER_MOD_ASI ? DOC_SECTION_ASI_ : DOC_SECTION_OVERRIDE_;
    UINT removed = 0u;
    UINT i = 0u;

    while (i < g_line_count) {
        WCHAR name[MAX_PATH_];
        int enabled;
        DocSection_ h = section_from_header_(g_lines[i].text);

        if (h != DOC_SECTION_NONE_) {
            section = h;
            ++i;
            continue;
        }

        if (section == target &&
            parse_entry_line_(g_lines[i].text, section, name, MAX_PATH_, &enabled) &&
            weq_i_(name, target_name)) {
            UINT j;
            /*
             * 删除固定数组中的一行只能把后面的行整体向前搬一格。搬完以后当前索引 i 已经装入“下一行”，
             * 所以这里故意不 ++i，下一轮继续检查同一个索引，避免相邻重复键漏删。
             */
            for (j = i + 1u; j < g_line_count; ++j) g_lines[j - 1u] = g_lines[j];
            --g_line_count;
            ++removed;
            continue;
        }
        ++i;
    }
    return removed;
}

/* ---------- UTF-8 + CRLF 原子保存 ---------- */

static int write_utf8_line_(HANDLE file, const WCHAR* line) {
    /*
     * WideCharToMultiByte 返回值包含结尾 NUL，所以空字符串的合法返回值就是 1。这里 bytes<=0 才是错误，
     * bytes==1 时正文长度为 0，只写 CRLF。这条边界专门防止 v0.2.10“空行被误判失败”再次出现。
     */
    int bytes;
    DWORD wrote = 0u;
    if (!file || file == INVALID_HANDLE_VALUE_ || !line) return 0;
    bytes = WideCharToMultiByte(CP_UTF8_, 0u, line, -1, g_utf8, (int)(sizeof(g_utf8) - 3u), NULL_PTR, NULL_PTR);
    if (bytes <= 0) return 0;
    if (bytes > 1 && (!WriteFile(file, g_utf8, (DWORD)(bytes - 1), &wrote, NULL_PTR) || wrote != (DWORD)(bytes - 1))) return 0;
    return WriteFile(file, "\r\n", 2u, &wrote, NULL_PTR) && wrote == 2u;
}

typedef BOOL (WINAPI *PFN_MoveFileExW_)(LPCWSTR, LPCWSTR, DWORD);

static int save_document_atomic_(const WCHAR* mods_root) {
    /*
     * 永远不直接 CREATE_ALWAYS 主 mods.ini。先把 UTF-8 BOM 和全部行写进同目录临时文件，Flush 成功后再原子替换；
     * 这样即使中途断电/写失败，用户原来的主配置仍在。
     */
    WCHAR ini_path[CASTLE_PATH_CAP];
    WCHAR temp_path[CASTLE_PATH_CAP];
    HANDLE file;
    DWORD wrote = 0u;
    UINT i;
    static const BYTE bom[3] = {0xEFu, 0xBBu, 0xBFu};
    HMODULE kernel;
    PFN_MoveFileExW_ move_file_ex;

    if (!path_join_(ini_path, CASTLE_PATH_CAP, mods_root, (const WCHAR*)L"mods.ini") ||
        !path_join_(temp_path, CASTLE_PATH_CAP, mods_root, (const WCHAR*)L"mods.ini.castle.gui.tmp")) return 0;

    /* 临时文件允许覆盖旧的残留 .tmp；真正主配置要等最后 MoveFileExW 成功才会发生变化。 */
    file = CreateFileW(temp_path, GENERIC_WRITE_, FILE_SHARE_READ_, NULL_PTR, CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) return 0;

    if (!WriteFile(file, bom, 3u, &wrote, NULL_PTR) || wrote != 3u) { CloseHandle(file); return 0; }
    for (i = 0u; i < g_line_count; ++i) {
        if (!write_utf8_line_(file, g_lines[i].text)) { CloseHandle(file); return 0; }
    }
    if (!FlushFileBuffers(file)) { CloseHandle(file); return 0; }
    CloseHandle(file);

    kernel = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    move_file_ex = kernel ? (PFN_MoveFileExW_)GetProcAddress(kernel, "MoveFileExW") : NULL_PTR;
    if (!move_file_ex || !move_file_ex(temp_path, ini_path, 0x00000001u | 0x00000008u)) return 0;
    return 1;
}

/*
 * 为了不为了一个清理 API 扩大 platform.h，这里用动态 DeleteFileW 负责失败时删除临时文件。
 */
typedef BOOL (WINAPI *PFN_DeleteFileW_)(LPCWSTR);
static void delete_temp_if_present_(const WCHAR* path) {
    HMODULE kernel = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    PFN_DeleteFileW_ fn = kernel ? (PFN_DeleteFileW_)GetProcAddress(kernel, "DeleteFileW") : NULL_PTR;
    if (fn) fn(path);
}

static int save_current_model_(const WCHAR* mods_root) {
    /*
     * 保存分两阶段：先把内存模型映射回原文行，再原子写盘。任何阶段失败都给 GUI 一条可读错误，
     * 并尽量删除临时文件；主 mods.ini 因为从未被直接截断，所以仍保持原样。
     */
    WCHAR temp_path[CASTLE_PATH_CAP];
    if (!rewrite_section_entry_slots_(LAUNCHER_MOD_ASI) || !rewrite_section_entry_slots_(LAUNCHER_MOD_OVERRIDE)) {
        set_error_((const WCHAR*)L"mods.ini 内部条目与 GUI 模型无法对应；为避免破坏原配置，本次没有写盘。");
        return 0;
    }
    if (!save_document_atomic_(mods_root)) {
        if (path_join_(temp_path, CASTLE_PATH_CAP, mods_root, (const WCHAR*)L"mods.ini.castle.gui.tmp")) delete_temp_if_present_(temp_path);
        set_error_((const WCHAR*)L"无法原子保存 mods\\mods.ini。原配置保持不变，请检查文件是否只读或被其它程序占用。");
        return 0;
    }
    return 1;
}

/* ---------- 对外主流程 ---------- */

static int build_roots_(const WCHAR* mods_root, WCHAR* asi_root, WCHAR* overrides_root, WCHAR* ini_path) {
    /* 统一准备 GUI 需要的三个固定位置，并确保基础目录/缺失模板存在，避免调用方各自拼路径造成规则分叉。 */
    if (!mods_root || !mods_root[0]) return 0;
    if (!ensure_directory_(mods_root)) return 0;
    if (!path_join_(asi_root, CASTLE_PATH_CAP, mods_root, (const WCHAR*)L"asi") ||
        !path_join_(overrides_root, CASTLE_PATH_CAP, mods_root, (const WCHAR*)L"overrides") ||
        !path_join_(ini_path, CASTLE_PATH_CAP, mods_root, (const WCHAR*)L"mods.ini")) return 0;
    if (!ensure_directory_(asi_root) || !ensure_directory_(overrides_root) || !ensure_override_template_(overrides_root)) return 0;
    return 1;
}

static int load_and_scan_internal_(const WCHAR* mods_root) {
    /*
     * 这是“打开 GUI”和“重新扫描”共用的完整收敛流程：读原配置 → 建模型 → 扫磁盘 → 自动增补新项 →
     * 刷新存在状态 → 只有确实发生增补时才原子保存。没有变化时不无意义重写用户文件。
     */
    WCHAR asi_root[CASTLE_PATH_CAP];
    WCHAR overrides_root[CASTLE_PATH_CAP];
    WCHAR ini_path[CASTLE_PATH_CAP];
    UINT i;
    int changed = 0;

    clear_error_();
    if (!build_roots_(mods_root, asi_root, overrides_root, ini_path)) {
        set_error_((const WCHAR*)L"无法准备 mods、asi、overrides 或模板目录。");
        return 0;
    }
    if (!load_document_(ini_path)) {
        set_error_((const WCHAR*)L"无法读取或解码 mods\\mods.ini。支持 UTF-8、UTF-16LE 和系统 ANSI。");
        return 0;
    }

    /* 新安装没有 mods.ini 时先建立两个标准节；后续扫描再把实际项目填进去。 */
    if (g_line_count == 0u) {
        if (!insert_line_(0u, (const WCHAR*)L"[ASI]") ||
            !insert_line_(1u, (const WCHAR*)L"") ||
            !insert_line_(2u, (const WCHAR*)L"[Overrides]")) {
            set_error_((const WCHAR*)L"mods.ini 文档容量不足，无法生成初始配置。");
            return 0;
        }
        changed = 1;
    }

    if (!rebuild_model_from_document_()) {
        set_error_((const WCHAR*)L"mods.ini 有效 Mod 条目超过 256 项，GUI 无法安全管理。");
        return 0;
    }

    /* 先处理 ASI：磁盘有但模型没有的项目按扫描后的自然顺序追加到模型和文档尾部。 */
    scan_asi_(asi_root);
    for (i = 0u; i < g_scan_count; ++i) {
        if (!item_has_name_(g_asi, g_asi_count, g_scan[i].name)) {
            if (g_asi_count >= LAUNCHER_MOD_MAX_ITEMS) break;
            if (!append_entry_to_section_(LAUNCHER_MOD_ASI, g_scan[i].name, 1) ||
                !add_item_(g_asi, &g_asi_count, g_scan[i].name, 1)) {
                set_error_((const WCHAR*)L"自动加入新 ASI 时配置文档容量不足。");
                return 0;
            }
            changed = 1;
        }
    }

    /* 再处理 Overrides；scan_overrides_ 已经把模板和新空目录过滤掉，所以这里只面对真正有效的新 Mod。 */
    scan_overrides_(overrides_root);
    for (i = 0u; i < g_scan_count; ++i) {
        if (!item_has_name_(g_overrides, g_override_count, g_scan[i].name)) {
            if (g_override_count >= LAUNCHER_MOD_MAX_ITEMS) break;
            if (!append_entry_to_section_(LAUNCHER_MOD_OVERRIDE, g_scan[i].name, 1) ||
                !add_item_(g_overrides, &g_override_count, g_scan[i].name, 1)) {
                set_error_((const WCHAR*)L"自动加入新 Overrides 时配置文档容量不足。");
                return 0;
            }
            changed = 1;
        }
    }

    refresh_item_presence_(asi_root, overrides_root);

    if (changed) {
        /*
         * 新发现项目已经同时进入文档和模型。这里的 rewrite 只会把现有槽按当前顺序写回，
         * 不会删除注释/未知节；随后再做原子替换，避免重演 v0.2.10 的截断回归。
         */
        if (!save_current_model_(mods_root)) return 0;
    }
    clear_error_();
    return 1;
}

/*
 * 某次“启停/排序/清理缺失项”写盘失败后，我们需要重新读取磁盘，把 GUI 内存恢复成真实配置。
 * 但 load_and_scan_internal_() 的正常职责是从一次全新操作开始，所以它开头和成功结尾都会 clear_error_()。
 * 如果直接调用它，真正的“为什么保存失败”就会被清空，GUI 最后只能弹出“没有更多错误信息”。
 *
 * 这个小辅助函数先复制原错误，再执行回读，最后把原错误放回去。这样同时得到两个目标：
 *   1. 内存模型回到磁盘真相，不会继续显示一个实际上没有保存成功的顺序/开关；
 *   2. 用户仍能看到最初的保存失败原因。
 * 这里故意不把回读过程当成新的用户操作，也不改成功路径。
 */
static void reload_after_failed_edit_preserving_error_(const WCHAR* mods_root) {
    WCHAR saved_error[512];
    saved_error[0] = 0;
    wcopy_(saved_error, (UINT)(sizeof(saved_error) / sizeof(saved_error[0])), g_last_error);
    load_and_scan_internal_(mods_root);
    if (saved_error[0]) set_error_(saved_error);
}

int LauncherModConfig_LoadAndScan(const WCHAR* mods_root) {
    return load_and_scan_internal_(mods_root);
}

int LauncherModConfig_Refresh(const WCHAR* mods_root) {
    return load_and_scan_internal_(mods_root);
}

UINT LauncherModConfig_GetCount(LauncherModKind_ kind) {
    return kind == LAUNCHER_MOD_ASI ? g_asi_count : g_override_count;
}

const LauncherModItem_* LauncherModConfig_GetItem(LauncherModKind_ kind, UINT index) {
    if (kind == LAUNCHER_MOD_ASI) return index < g_asi_count ? &g_asi[index] : NULL_PTR;
    return index < g_override_count ? &g_overrides[index] : NULL_PTR;
}

int LauncherModConfig_SetEnabled(const WCHAR* mods_root, LauncherModKind_ kind, UINT index, int enabled) {
    /*
     * GUI 点击复选框时先只改内存中的这一项，再尝试原子保存。保存失败就恢复旧 enabled 并重新读取磁盘，
     * 这样“界面显示”不会和真正配置文件长期分叉。
     */
    LauncherModItem_* items = kind == LAUNCHER_MOD_ASI ? g_asi : g_overrides;
    UINT count = kind == LAUNCHER_MOD_ASI ? g_asi_count : g_override_count;
    int old;
    clear_error_();
    if (index >= count) return 0;
    old = items[index].enabled;
    items[index].enabled = enabled ? 1 : 0;
    if (!save_current_model_(mods_root)) {
        items[index].enabled = old;
        /*
         * 磁盘原文件没有被原子保存流程破坏；重新读取一次恢复模型。
         * 专用辅助函数会保留 save_current_model_ 刚刚写入的失败原因，避免回读成功后把错误文字清空。
         */
        reload_after_failed_edit_preserving_error_(mods_root);
        return 0;
    }
    clear_error_();
    return 1;
}

int LauncherModConfig_Move(const WCHAR* mods_root, LauncherModKind_ kind, UINT from_index, UINT to_index) {
    /*
     * 拖动排序只在同一种 Mod 内移动。先把被拖项目临时保存，再把中间区间整体平移一格，最后放到目标槽；
     * 保存失败时不尝试手工反向搬数组，而是重新读仍完好的磁盘配置，回滚更可靠。
     */
    LauncherModItem_* items = kind == LAUNCHER_MOD_ASI ? g_asi : g_overrides;
    UINT count = kind == LAUNCHER_MOD_ASI ? g_asi_count : g_override_count;
    LauncherModItem_ moving;
    UINT i;

    clear_error_();
    if (from_index >= count || to_index >= count || from_index == to_index) return 1;

    /*
     * 重复 section 本身不影响拖动；真正危险的是重复“键”。例如旧文件同时有 A=1、A=0，Core 取第一条。
     * 若 GUI 把其它名称搬到第一条 A 的槽位，后面的旧 A=0 就可能变成新的第一次，从而悄悄改变开关。
     * 与其擅自清洗用户历史配置，这里直接拒绝拖动并给出明确错误；用户手工去掉重复键后重新扫描即可。
     */
    if (g_has_duplicate_entries) {
        set_error_((const WCHAR*)L"mods.ini 存在同名重复 Mod 条目。为避免拖动后改变‘最上面第一次为准’的历史语义，本次不执行排序；请先手工删除重复键后再重新扫描。");
        return 0;
    }
    moving = items[from_index];
    if (from_index < to_index) {
        for (i = from_index; i < to_index; ++i) items[i] = items[i + 1u];
    } else {
        for (i = from_index; i > to_index; --i) items[i] = items[i - 1u];
    }
    items[to_index] = moving;

    if (!save_current_model_(mods_root)) {
        /* 保存失败时最可靠的回滚方式是重新读原 mods.ini，同时必须保留真正的写盘失败原因给 GUI。 */
        reload_after_failed_edit_preserving_error_(mods_root);
        return 0;
    }
    clear_error_();
    return 1;
}

int LauncherModConfig_RemoveMissing(const WCHAR* mods_root, LauncherModKind_ kind, UINT index) {
    /*
     * GUI 的“移除”按钮只在 present=0 时出现，但配置层仍必须自己做一次硬检查，不能把 UI 当安全边界。
     * 这个操作只是清理已经不存在的陈旧 INI 引用，不碰磁盘 Mod 文件。
     */
    LauncherModItem_* items = kind == LAUNCHER_MOD_ASI ? g_asi : g_overrides;
    UINT count = kind == LAUNCHER_MOD_ASI ? g_asi_count : g_override_count;
    WCHAR name[MAX_PATH_];
    WCHAR asi_root[CASTLE_PATH_CAP];
    WCHAR overrides_root[CASTLE_PATH_CAP];
    WCHAR ini_path[CASTLE_PATH_CAP];
    UINT removed;

    clear_error_();
    if (!mods_root || index >= count) {
        set_error_((const WCHAR*)L"要移除的 Mod 索引无效；本次没有修改 mods.ini。");
        return 0;
    }
    if (items[index].present) {
        set_error_((const WCHAR*)L"该 Mod 的文件或目录当前仍然存在；“移除”只用于清理已经缺失的陈旧配置项。");
        return 0;
    }
    if (!wcopy_(name, MAX_PATH_, items[index].name)) {
        set_error_((const WCHAR*)L"无法复制要移除的 Mod 名称；本次没有修改 mods.ini。");
        return 0;
    }
    if (!build_roots_(mods_root, asi_root, overrides_root, ini_path)) {
        set_error_((const WCHAR*)L"无法准备 mods 目录结构；本次没有修改 mods.ini。");
        return 0;
    }

    removed = remove_document_entries_by_name_(kind, name);
    if (removed == 0u) {
        set_error_((const WCHAR*)L"没有在 mods.ini 中找到对应的缺失 Mod 条目；请先重新扫描后再试。");
        reload_after_failed_edit_preserving_error_(mods_root);
        return 0;
    }

    /*
     * 主文件仍采用“先写同目录临时文件，再原子替换”的既有安全路径。若保存失败，主 mods.ini 没有被破坏；
     * 随即重新读取磁盘原文件，就能同时把 g_lines 与 GUI 模型恢复到操作前的真实状态。
     */
    if (!save_document_atomic_(mods_root)) {
        WCHAR temp_path[CASTLE_PATH_CAP];
        if (path_join_(temp_path, CASTLE_PATH_CAP, mods_root, (const WCHAR*)L"mods.ini.castle.gui.tmp"))
            delete_temp_if_present_(temp_path);
        set_error_((const WCHAR*)L"无法原子保存 mods\\mods.ini；缺失 Mod 没有被移除，原配置保持不变。");
        reload_after_failed_edit_preserving_error_(mods_root);
        return 0;
    }

    /*
     * 保存成功后不需要再扫描磁盘并自动发现新 Mod；这里只把刚刚写盘的文档重新变成模型，再刷新存在状态。
     * 这样一次“移除陈旧条目”的动作不会顺便偷偷登记用户刚放进去但尚未点击重新扫描的其它文件。
     */
    if (!rebuild_model_from_document_()) {
        set_error_((const WCHAR*)L"缺失 Mod 已写入配置，但 GUI 无法重建列表；请点击重新扫描。");
        return 0;
    }
    refresh_item_presence_(asi_root, overrides_root);
    clear_error_();
    return 1;
}

