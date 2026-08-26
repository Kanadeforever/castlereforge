#include "platform.h"
#include "override_loader.h"
#include "mod_loader.h"
#include "game_audit.h"

/*
 * _ReturnAddress 是 MSVC/clang-cl 提供的编译器内建函数。
 * 当 RPG.exe 调进我们的 CreateFile Hook 时，它返回“Hook 执行完以后应该回到游戏哪一条指令”的地址。
 * game_audit.c 只用这个地址判断调用者究竟属于 RPG.exe、原版 DLL，还是属于 ASI/Mod；
 * 它不会修改这个返回地址，也不会拿它跳转，所以这里只是给审计日志做来源标签。
 */
void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)

/*
 * 《幽城幻剑录》文件夹覆写层。
 *
 * 这个模块只做一件事：当游戏准备打开“原游戏文件”时，先去启用的
 * mods\overrides\<Mod名> 中找同相对路径文件；如果找到，就把打开请求改到 Mod 文件。
 *
 * 它不是“启动时把文件复制进 update”，也不会改动原游戏文件。
 * 因此关闭一个 Mod 的本质只是让这个目录不再参与查找，原文件始终保持原样。
 *
 * 优先级规则：
 *   mods.ini [Overrides] 从上往下 = 从早到晚加载；
 *   真正查找时从最后一项倒着往上找；
 *   所以后面的 Mod 若存在相同相对路径文件，会自然覆盖前面的 Mod。
 */

#define MAX_OVERRIDE_MODS_ 256u
#define MAX_HOOK_MODULES_   6u

/* Win32 PE32 常量。这里只解析导入表，不修改节、重定位或代码。 */
#define IMAGE_DOS_SIGNATURE_ 0x5A4Du
#define IMAGE_NT_SIGNATURE_  0x00004550u
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC_ 0x010Bu
#define IMAGE_ORDINAL_FLAG32_ 0x80000000u
#define IMAGE_DIRECTORY_ENTRY_IMPORT_ 1u

typedef struct OverrideEntry_ {
    WCHAR name[MAX_PATH_];
    int enabled;
} OverrideEntry_;

/*
 * 下面四个函数指针永远指向真正 KERNEL32/KERNELBASE 的实现，而不是已经被我们改写的游戏 IAT。
 * Hook 函数在“没有覆写文件”时必须调用这些真实函数，才能保持原版行为。
 */
typedef HANDLE (WINAPI *PFN_CreateFileA_)(LPCSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE);
typedef HANDLE (WINAPI *PFN_CreateFileW_)(LPCWSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE);
typedef DWORD  (WINAPI *PFN_GetFileAttributesA_)(LPCSTR);
typedef DWORD  (WINAPI *PFN_GetFileAttributesW_)(LPCWSTR);
/*
 * I/O 审计回调会写独立 game.log，日志 API 可能改变 LastError。
 * 因此文件 Hook 在返回原版调用者以前必须恢复真实文件 API 留下的错误码。
 */
typedef void  (WINAPI *PFN_SetLastError_)(DWORD);

static OverrideEntry_ g_entries[MAX_OVERRIDE_MODS_];
static UINT g_entry_count;
static WCHAR g_mod_root[YCR_PATH_CAP];
static WCHAR g_game_exe_dir[YCR_PATH_CAP];
static WCHAR g_game_root[YCR_PATH_CAP];
static WCHAR g_override_root[YCR_PATH_CAP];

static PFN_CreateFileA_ g_real_CreateFileA;
static PFN_CreateFileW_ g_real_CreateFileW;
static PFN_GetFileAttributesA_ g_real_GetFileAttributesA;
static PFN_GetFileAttributesW_ g_real_GetFileAttributesW;
static PFN_SetLastError_ g_real_SetLastError;

/*
 * v0.2.9 的游戏审计只有在 Locale 实体自检通过后才开启。
 * 开启以后 CreateFile/GetFileAttributes 仍由本模块统一 Hook；本模块只负责保持 Overrides 语义，
 * 再把“原版 API 实际发生了什么”送给 game_audit.c，避免两套 IAT Hook 互相覆盖。
 */
static int g_game_audit_enabled;

/* ---------- 极小字符串/路径辅助 ---------- */

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

static int path_join_(WCHAR* out, UINT cap, const WCHAR* left, const WCHAR* right) {
    UINT n;
    if (!wcopy_(out, cap, left)) return 0;
    n = wlen_(out);
    if (n && out[n - 1u] != (WCHAR)'\\' && out[n - 1u] != (WCHAR)'/') {
        if (!wappend_(out, cap, (const WCHAR*)L"\\")) return 0;
    }
    return wappend_(out, cap, right);
}

/*
 * Windows 文件系统不区分 ASCII 大小写。这个函数检查 full 是否位于 root 目录内部。
 * 边界字符必须是 '\\'、'/' 或字符串结尾，避免把 C:\\Game 错配到 C:\\GameBackup。
 */
static int path_has_root_prefix_(const WCHAR* full, const WCHAR* root, const WCHAR** relative_out) {
    UINT i = 0;
    if (!full || !root || !relative_out) return 0;
    while (root[i] && full[i]) {
        WCHAR a = full[i] == (WCHAR)'/' ? (WCHAR)'\\' : fold_ascii_(full[i]);
        WCHAR b = root[i] == (WCHAR)'/' ? (WCHAR)'\\' : fold_ascii_(root[i]);
        if (a != b) return 0;
        ++i;
    }
    if (root[i] != 0) return 0;
    if (full[i] == 0) {
        *relative_out = full + i;
        return 1;
    }
    if (full[i] != (WCHAR)'\\' && full[i] != (WCHAR)'/') return 0;
    ++i;
    *relative_out = full + i;
    return 1;
}

/*
 * 判断路径是不是明显的绝对路径。
 * 绝对路径交给 GetFullPathNameW 规范化；相对路径则明确以 RPG.exe 所在 exe 目录作为起点。
 * 这样即使某个插件临时改了进程 CurrentDirectory，原版 '..\\MultiMedia\\...' 仍按游戏 exe 目录解释。
 */
static int is_absolute_path_(const WCHAR* path) {
    if (!path || !path[0]) return 0;
    if (((path[0] >= (WCHAR)'A' && path[0] <= (WCHAR)'Z') ||
         (path[0] >= (WCHAR)'a' && path[0] <= (WCHAR)'z')) && path[1] == (WCHAR)':') return 1;
    if ((path[0] == (WCHAR)'\\' || path[0] == (WCHAR)'/') &&
        (path[1] == (WCHAR)'\\' || path[1] == (WCHAR)'/')) return 1;
    if (path[0] == (WCHAR)'\\' || path[0] == (WCHAR)'/') return 1;
    return 0;
}

/*
 * 把游戏传进来的任意相对/绝对路径规范成“游戏安装根目录下的相对路径”。
 * 当前典型安装是：
 *   <游戏根>\\exe\\RPG.exe
 *   <游戏根>\\MultiMedia\\...
 * 所以 '..\\MultiMedia\\Mov\\2.DAT' 最终会得到 'MultiMedia\\Mov\\2.DAT'。
 *
 * 这一步同时是安全边界：只有最终绝对路径确实位于 g_game_root 内，才允许去 mods\overrides 查找。
 * 游戏访问系统 DLL、用户目录或其它盘符时，本 Loader 完全不重定向。
 */
static int make_game_relative_(LPCWSTR requested, WCHAR* relative, UINT relative_cap) {
    WCHAR combined[YCR_PATH_CAP];
    WCHAR full[YCR_PATH_CAP];
    const WCHAR* rel;
    DWORD n;

    if (!requested || !requested[0] || !relative || relative_cap == 0u) return 0;

    if (is_absolute_path_(requested)) {
        if (!wcopy_(combined, YCR_PATH_CAP, requested)) return 0;
    } else {
        if (!path_join_(combined, YCR_PATH_CAP, g_game_exe_dir, requested)) return 0;
    }

    n = GetFullPathNameW(combined, YCR_PATH_CAP, full, NULL_PTR);
    if (!n || n >= YCR_PATH_CAP) return 0;
    if (!path_has_root_prefix_(full, g_game_root, &rel)) return 0;
    if (!rel[0]) return 0;
    return wcopy_(relative, relative_cap, rel);
}

static int file_exists_(LPCWSTR path) {
    DWORD attr;
    /*
     * 这里一定调用真正的 GetFileAttributesW 指针，而不是游戏 IAT 中已经被 Hook 的版本。
     * 否则“找 Overrides 候选”会再次进入 Hook，自我递归直到栈溢出。
     */
    if (!g_real_GetFileAttributesW) return 0;
    attr = g_real_GetFileAttributesW(path);
    return attr != INVALID_FILE_ATTRIBUTES_ && !(attr & FILE_ATTRIBUTE_DIRECTORY_);
}

/*
 * 根据当前 [Overrides] 优先级寻找最高优先级的替代文件。
 * 配置从上到下是加载顺序，因此这里必须从最后一项往第一项查。
 */
static int resolve_override_(LPCWSTR requested, WCHAR* replacement, UINT replacement_cap) {
    WCHAR relative[YCR_PATH_CAP];
    UINT i;

    if (!make_game_relative_(requested, relative, YCR_PATH_CAP)) return 0;

    i = g_entry_count;
    while (i > 0u) {
        WCHAR mod_root[YCR_PATH_CAP];
        WCHAR candidate[YCR_PATH_CAP];
        --i;

        if (!g_entries[i].enabled) continue;
        if (!path_join_(mod_root, YCR_PATH_CAP, g_override_root, g_entries[i].name)) continue;
        if (!path_join_(candidate, YCR_PATH_CAP, mod_root, relative)) continue;

        if (file_exists_(candidate)) {
            if (!wcopy_(replacement, replacement_cap, candidate)) return 0;
            return 1;
        }
    }
    return 0;
}


/* ---------- 被游戏调用的四个文件 API Hook ---------- */


static HANDLE WINAPI Hook_CreateFileW_(LPCWSTR path, DWORD access, DWORD share, LPVOID security,
                                       DWORD creation, DWORD attrs, HANDLE template_file) {
    WCHAR replacement[YCR_PATH_CAP];
    LPVOID caller = _ReturnAddress();
    HANDLE result;
    DWORD original_error;
    int redirected = 0;

    /* 只重定向纯读取 OPEN_EXISTING；存档、配置写入、创建文件一律保持原路径，避免 Mod 层污染用户数据。 */
    if (!(access & GENERIC_WRITE_) && creation == OPEN_EXISTING_ && resolve_override_(path, replacement, YCR_PATH_CAP)) {
        ModLoader_LogOverrideHit(path, replacement);
        result = g_real_CreateFileW(replacement, access, share, security, creation, attrs, template_file);
        redirected = 1;
    } else {
        result = g_real_CreateFileW(path, access, share, security, creation, attrs, template_file);
    }
    original_error = GetLastError();
    if (g_game_audit_enabled)
        GameAudit_RecordCreateFileW(caller, path, access, share, creation, attrs, result, original_error, redirected);
    if (g_real_SetLastError) g_real_SetLastError(original_error);
    return result;
}

static HANDLE WINAPI Hook_CreateFileA_(LPCSTR path, DWORD access, DWORD share, LPVOID security,
                                       DWORD creation, DWORD attrs, HANDLE template_file) {
    WCHAR wide_path[YCR_PATH_CAP];
    WCHAR replacement[YCR_PATH_CAP];
    int chars;
    HANDLE result;
    DWORD original_error;
    int redirected = 0;
    LPVOID caller = _ReturnAddress();

    if (path) {
        /*
         * RPG.exe 是老 ANSI 程序，所以 CreateFileA 是主路径。
         * 这里只为了 Overrides 查找固定按 CP950 把路径翻成 Unicode；没有 Overrides 命中时，
         * 原始 ANSI 字节仍一字不改交回 Windows 真正 CreateFileA，保持转区层的真实语义。
         */
        chars = MultiByteToWideChar(CASTLE_CP_BIG5_, 0, path, -1, wide_path, (int)YCR_PATH_CAP);
        if (chars > 0 && !(access & GENERIC_WRITE_) && creation == OPEN_EXISTING_ &&
            resolve_override_(wide_path, replacement, YCR_PATH_CAP)) {
            ModLoader_LogOverrideHit(wide_path, replacement);
            result = g_real_CreateFileW(replacement, access, share, security, creation, attrs, template_file);
            redirected = 1;
            original_error = GetLastError();
            if (g_game_audit_enabled)
                GameAudit_RecordCreateFileA(caller, path, access, share, creation, attrs, result, original_error, redirected);
            if (g_real_SetLastError) g_real_SetLastError(original_error);
            return result;
        }
    }

    result = g_real_CreateFileA(path, access, share, security, creation, attrs, template_file);
    original_error = GetLastError();
    if (g_game_audit_enabled)
        GameAudit_RecordCreateFileA(caller, path, access, share, creation, attrs, result, original_error, redirected);
    if (g_real_SetLastError) g_real_SetLastError(original_error);
    return result;
}

static DWORD WINAPI Hook_GetFileAttributesW_(LPCWSTR path) {
    WCHAR replacement[YCR_PATH_CAP];
    LPVOID caller = _ReturnAddress();
    DWORD result, original_error;
    int redirected = 0;
    if (resolve_override_(path, replacement, YCR_PATH_CAP)) {
        result = g_real_GetFileAttributesW(replacement);
        redirected = 1;
    } else {
        result = g_real_GetFileAttributesW(path);
    }
    original_error = GetLastError();
    if (g_game_audit_enabled) GameAudit_RecordGetFileAttributesW(caller, path, result, original_error, redirected);
    if (g_real_SetLastError) g_real_SetLastError(original_error);
    return result;
}

static DWORD WINAPI Hook_GetFileAttributesA_(LPCSTR path) {
    WCHAR wide_path[YCR_PATH_CAP];
    WCHAR replacement[YCR_PATH_CAP];
    int chars;
    LPVOID caller = _ReturnAddress();
    DWORD result, original_error;
    int redirected = 0;
    if (path) {
        chars = MultiByteToWideChar(CASTLE_CP_BIG5_, 0, path, -1, wide_path, (int)YCR_PATH_CAP);
        if (chars > 0 && resolve_override_(wide_path, replacement, YCR_PATH_CAP)) {
            result = g_real_GetFileAttributesW(replacement);
            redirected = 1;
            original_error = GetLastError();
            if (g_game_audit_enabled) GameAudit_RecordGetFileAttributesA(caller, path, result, original_error, redirected);
            if (g_real_SetLastError) g_real_SetLastError(original_error);
            return result;
        }
    }
    result = g_real_GetFileAttributesA(path);
    original_error = GetLastError();
    if (g_game_audit_enabled) GameAudit_RecordGetFileAttributesA(caller, path, result, original_error, redirected);
    if (g_real_SetLastError) g_real_SetLastError(original_error);
    return result;
}

/* ---------- PE32 IAT 修改 ---------- */

static int ascii_eq_i_(const char* a, const char* b) {
    UINT i = 0;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca = (char)(ca + ('a' - 'A'));
        if (cb >= 'A' && cb <= 'Z') cb = (char)(cb + ('a' - 'A'));
        if (ca != cb) return 0;
        ++i;
    }
    return a[i] == 0 && b[i] == 0;
}

/*
 * 把某个模块 IAT 中指定的 KERNEL32 函数改到我们的 Hook。
 *
 * 只修改 IAT 数据槽，不改模块代码：
 *   - OriginalFirstThunk 用来读取原始函数名；
 *   - FirstThunk 是 Windows Loader 已经填好的真实函数指针槽；
 *   - 找到目标以后临时 PAGE_READWRITE，写入 Hook 地址，再恢复原保护。
 */
static UINT patch_module_iat_(HMODULE module) {
    BYTE* base = (BYTE*)module;
    DWORD pe_off, signature, opt_off, import_rva;
    WORD magic;
    DWORD descriptor_rva;
    UINT patched = 0;

    if (!module) return 0;
    if (*(WORD*)(base + 0x00u) != IMAGE_DOS_SIGNATURE_) return 0;
    pe_off = *(DWORD*)(base + 0x3Cu);
    signature = *(DWORD*)(base + pe_off);
    if (signature != IMAGE_NT_SIGNATURE_) return 0;

    /* IMAGE_FILE_HEADER 长度固定 20 字节，所以 OptionalHeader 从 PE签名+4+20 开始。 */
    opt_off = pe_off + 4u + 20u;
    magic = *(WORD*)(base + opt_off);
    if (magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC_) return 0;

    /* PE32 的 DataDirectory 从 OptionalHeader + 96 开始，Import 是第 1 项，每项 8 字节。 */
    import_rva = *(DWORD*)(base + opt_off + 96u + IMAGE_DIRECTORY_ENTRY_IMPORT_ * 8u);
    if (!import_rva) return 0;

    descriptor_rva = import_rva;
    for (;;) {
        DWORD* desc = (DWORD*)(base + descriptor_rva);
        DWORD original_first_thunk = desc[0];
        DWORD name_rva = desc[3];
        DWORD first_thunk = desc[4];
        const char* dll_name;
        DWORD thunk_index = 0;

        /* 五个 DWORD 全为 0 就是 IMAGE_IMPORT_DESCRIPTOR 终止项。 */
        if (!desc[0] && !desc[1] && !desc[2] && !desc[3] && !desc[4]) break;
        descriptor_rva += 20u;
        if (!name_rva || !first_thunk) continue;

        dll_name = (const char*)(base + name_rva);
        if (!ascii_eq_i_(dll_name, "KERNEL32.dll") && !ascii_eq_i_(dll_name, "KERNELBASE.dll")) continue;

        /*
         * 没有 OriginalFirstThunk 时，FirstThunk 在进程装载后已经是函数地址，无法再安全读取导入名。
         * 这种罕见模块直接跳过，不为了“多 Hook 一个模块”去猜地址。
         */
        if (!original_first_thunk) continue;

        for (;;) {
            DWORD name_thunk = *(DWORD*)(base + original_first_thunk + thunk_index * 4u);
            DWORD* iat_slot = (DWORD*)(base + first_thunk + thunk_index * 4u);
            const char* func_name;
            DWORD hook = 0;
            DWORD old_protect = 0;
            DWORD ignored = 0;

            if (!name_thunk) break;
            ++thunk_index;
            if (name_thunk & IMAGE_ORDINAL_FLAG32_) continue;

            /* IMAGE_IMPORT_BY_NAME 前两个字节是 Hint，真正 ASCII 名称从 +2 开始。 */
            func_name = (const char*)(base + name_thunk + 2u);
            /*
             * 没有启用 Overrides 时，v0.2.9 在独立游戏审计启用时接管 CreateFileA；
             * 审计启用后四个路径 API 都送进 game.log；未启用时仍只按 Overrides 实际需要接管。
             */
            if (ascii_eq_i_(func_name, "CreateFileA")) {
                if (g_game_audit_enabled || OverrideLoader_EnabledCount())
                    hook = (DWORD)(SIZE_T)&Hook_CreateFileA_;
                else
                    continue;
            }
            else if (ascii_eq_i_(func_name, "CreateFileW")) {
                if (g_game_audit_enabled || OverrideLoader_EnabledCount()) hook = (DWORD)(SIZE_T)&Hook_CreateFileW_; else continue;
            }
            else if (ascii_eq_i_(func_name, "GetFileAttributesA")) {
                if (g_game_audit_enabled || OverrideLoader_EnabledCount()) hook = (DWORD)(SIZE_T)&Hook_GetFileAttributesA_; else continue;
            }
            else if (ascii_eq_i_(func_name, "GetFileAttributesW")) {
                if (g_game_audit_enabled || OverrideLoader_EnabledCount()) hook = (DWORD)(SIZE_T)&Hook_GetFileAttributesW_; else continue;
            }
            else continue;

            if (!VirtualProtect(iat_slot, 4u, PAGE_READWRITE_, &old_protect)) continue;
            *iat_slot = hook;
            /* 恢复原来的页属性。第二次返回的旧属性没有业务用途，只需要给 API 一个合法地址。 */
            VirtualProtect(iat_slot, 4u, old_protect, &ignored);
            ++patched;
        }
    }

    if (patched) FlushInstructionCache(GetCurrentProcess(), module, 1u);
    return patched;
}

static int resolve_real_file_apis_(void) {
    HMODULE kernel32 = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    if (!kernel32) return 0;

    g_real_CreateFileA = (PFN_CreateFileA_)GetProcAddress(kernel32, "CreateFileA");
    g_real_CreateFileW = (PFN_CreateFileW_)GetProcAddress(kernel32, "CreateFileW");
    g_real_GetFileAttributesA = (PFN_GetFileAttributesA_)GetProcAddress(kernel32, "GetFileAttributesA");
    g_real_GetFileAttributesW = (PFN_GetFileAttributesW_)GetProcAddress(kernel32, "GetFileAttributesW");
    g_real_SetLastError = (PFN_SetLastError_)GetProcAddress(kernel32, "SetLastError");

    /* SetLastError 是透明 Hook 的必需条件：审计写日志以后必须恢复原 API 的错误码。 */
    return g_real_CreateFileA && g_real_CreateFileW && g_real_GetFileAttributesA && g_real_GetFileAttributesW && g_real_SetLastError;
}

void OverrideLoader_Reset(const WCHAR* mod_root, const WCHAR* game_exe_dir, const WCHAR* game_root) {
    UINT i;
    g_entry_count = 0u;
    for (i = 0; i < MAX_OVERRIDE_MODS_; ++i) {
        g_entries[i].name[0] = 0;
        g_entries[i].enabled = 0;
    }
    g_mod_root[0] = 0;
    g_game_exe_dir[0] = 0;
    g_game_root[0] = 0;
    g_override_root[0] = 0;
    g_game_audit_enabled = 0;

    if (!mod_root || !game_exe_dir || !game_root) return;

    /*
     * Loader 的用户可见路径统一保持相对：mod_root 通常就是 "mods"。
     * Overrides 本身也继续用相对路径打开，不把盘符和安装目录写进配置或长期状态。
     */
    if (!wcopy_(g_mod_root, YCR_PATH_CAP, mod_root)) return;

    /*
     * 只有“安全边界比较”必须知道规范化后的真实位置：
     *   - 游戏可能传入绝对路径；
     *   - 也可能传入 ..\MultiMedia\...；
     *   - 还可能被别的插件临时改变 CurrentDirectory。
     *
     * 因此这里只把 Launcher 传来的 "." / ".." 临时展开到两个私有缓冲区。
     * GetFullPathNameW 的输入和输出严格使用不同缓冲区，避免 v0.2 那种同一缓冲区既当输入又当输出导致路径被重复拼接。
     * 这些绝对路径只用于判断“请求是否仍位于游戏安装根内”，不会写回 mods.ini，也不会成为 Loader 的业务路径。
     */
    {
        DWORD n_exe = GetFullPathNameW(game_exe_dir, YCR_PATH_CAP, g_game_exe_dir, NULL_PTR);
        DWORD n_root = GetFullPathNameW(game_root, YCR_PATH_CAP, g_game_root, NULL_PTR);
        if (!n_exe || n_exe >= YCR_PATH_CAP || !n_root || n_root >= YCR_PATH_CAP) {
            g_game_exe_dir[0] = 0;
            g_game_root[0] = 0;
            return;
        }
    }

    /*
     * 固定唯一位置：RPG.exe 同目录的 mods\overrides。
     * 传入的 mod_root 已经是相对路径 "mods"，所以这里只追加 overrides；再拼一层 mods 会得到错误的 mods\mods\overrides。
     * update/scripts/plugins 等旧入口全部不存在。
     */
    if (!path_join_(g_override_root, YCR_PATH_CAP, g_mod_root, (const WCHAR*)L"overrides"))
        g_override_root[0] = 0;
}

int OverrideLoader_Add(const WCHAR* mod_name, int enabled) {
    if (!mod_name || !mod_name[0] || g_entry_count >= MAX_OVERRIDE_MODS_) return 0;
    if (!wcopy_(g_entries[g_entry_count].name, MAX_PATH_, mod_name)) return 0;
    g_entries[g_entry_count].enabled = enabled ? 1 : 0;
    ++g_entry_count;
    return 1;
}

UINT OverrideLoader_EnabledCount(void) {
    UINT i, count = 0;
    for (i = 0; i < g_entry_count; ++i) if (g_entries[i].enabled) ++count;
    return count;
}

UINT OverrideLoader_Install(void) {
    UINT patched = 0;
    HMODULE modules[MAX_HOOK_MODULES_];
    UINT i;

    /*
     * 即使当前没有 Overrides，也先把真实文件 API 地址准备好。
     * v0.2.9 到 EntryPoint 前会启用独立 game.log 审计；CreateFile/GetFileAttributes 仍复用本模块这一套 Hook。
     */
    if (!resolve_real_file_apis_()) {
        ModLoader_Log((const WCHAR*)L"[Overrides错误] 无法解析真实 CreateFile/GetFileAttributes，文件覆写与 ANSI 文件名实测均不可用。");
        return 0;
    }

    if (!OverrideLoader_EnabledCount()) {
        if (ModLoader_IsGameLogEnabled())
            ModLoader_Log((const WCHAR*)L"[Overrides] 当前没有启用的文件夹 Mod，不安装文件覆写 Hook；GameLog=1，本轮会在 EntryPoint 前仅为原版游戏审计接入文件 API。");
        else
            ModLoader_Log((const WCHAR*)L"[Overrides] 当前没有启用的文件夹 Mod，且 GameLog=0；本轮不需要安装 CreateFile/GetFileAttributes Hook。");
        return 0;
    }

    /*
     * dev9 恢复 dev5 的目标集合，包括 ddraw.dll。
     * 原因不是让 Overrides 去“管理 DirectDraw”，而是 cnc-ddraw 本身会通过 Win32 文件/窗口链参与游戏运行，
     * dev5 的整套 IAT 桥已经由用户证明同时满足转区与截图；dev6 起把 ddraw 从这些桥中抽掉后两项一起回归。
     *
     * patch_module_iat_ 仍然只按 CreateFile/GetFileAttributes 等精确函数名改 IAT，不会改 ddraw 导出、
     * 不会替换 DirectDrawCreate，也不会把 screenshot 文件强行当成游戏 Overrides 资源。
     */
    modules[0] = GetModuleHandleW(NULL_PTR);
    modules[1] = GetModuleHandleW((const WCHAR*)L"binkw32.dll");
    modules[2] = GetModuleHandleW((const WCHAR*)L"BASS.dll");
    modules[3] = GetModuleHandleW((const WCHAR*)L"ijl10.dll");
    modules[4] = GetModuleHandleW((const WCHAR*)L"ddraw.dll");
    modules[5] = GetModuleHandleW((const WCHAR*)L"SHLWAPI.dll");

    for (i = 0; i < MAX_HOOK_MODULES_; ++i) {
        if (modules[i]) patched += patch_module_iat_(modules[i]);
    }
    return patched;
}

UINT OverrideLoader_EnableGameAudit(void) {
    UINT patched = 0u;
    HMODULE modules[MAX_HOOK_MODULES_];
    UINT i;

    /*
     * 第一步先确认“真正的 Windows 文件 API”地址已经保存好。
     * Hook 函数最终必须调用这些真实地址才能完成原版文件操作；如果它们都找不到，
     * 继续安装 IAT Hook 只会让游戏调用一个没有后端的壳，所以这里直接返回 0 并留下错误日志。
     */
    if (!g_real_CreateFileA || !g_real_CreateFileW || !g_real_SetLastError) {
        if (!resolve_real_file_apis_()) {
            ModLoader_Log((const WCHAR*)L"[游戏审计错误] 无法解析真实文件 API；game.log 的 CreateFile/GetFileAttributes 部分未接入。");
            return 0u;
        }
    }

    /*
     * 只有 Locale 的 Big5 实体文件硬自检通过以后 Core 才会调用本函数。
     * 把这个开关设为 1 后，已有的 CreateFile/GetFileAttributes Hook 才会把调用事实旁路送进 game.log；
     * 在此之前 Bootstrap/Loader 自己的文件操作不会被误写成“原版游戏 I/O”。
     */
    g_game_audit_enabled = 1;

    /*
     * 下面列出的模块都是当前原版游戏装载图里已经确认的模块。
     * GetModuleHandleW 只查询“是否已经加载”，不会为了审计额外加载 DLL；因此某个可选模块不存在时得到 NULL，
     * 后面的循环自然跳过它，不会改变游戏原本会加载哪些组件。
     */
    /*
     * game.log 审计继续复用同一套文件 API IAT 桥，并恢复 dev5 对 ddraw.dll 的补挂。
     * GameAudit 自己仍按 caller 白名单过滤，因此 ddraw 内部截图文件不会被记录成“原版 RPG.exe 资源访问”。
     * 这里恢复的是 dev5 的调用链兼容性，不是扩大审计范围。
     */
    modules[0] = GetModuleHandleW(NULL_PTR);
    modules[1] = GetModuleHandleW((const WCHAR*)L"binkw32.dll");
    modules[2] = GetModuleHandleW((const WCHAR*)L"BASS.dll");
    modules[3] = GetModuleHandleW((const WCHAR*)L"ijl10.dll");
    modules[4] = GetModuleHandleW((const WCHAR*)L"ddraw.dll");
    modules[5] = GetModuleHandleW((const WCHAR*)L"SHLWAPI.dll");

    /*
     * 逐个模块修改它自己的 IAT 槽。patch_module_iat_ 会再次按函数名精确匹配，
     * 所以这里的 patched 只是“实际找到并确认写入了多少个槽位”的计数，供 modloader.log 做基础设施验收。
     */
    for (i = 0u; i < MAX_HOOK_MODULES_; ++i) {
        if (modules[i]) patched += patch_module_iat_(modules[i]);
    }
    return patched;
}

UINT OverrideLoader_PatchModule(HMODULE module) {
    /*
     * ASI 的 DllMain 已经在 LoadLibrary 返回以前执行，因此这个函数无法追溯它在 DllMain 中的文件访问。
     * 但从 InitializeASI 和后续正常运行开始，它会和 RPG.exe 一样遵守 Overrides。
     */
    if (!module || !g_real_CreateFileA || !g_real_CreateFileW || !g_real_GetFileAttributesA || !g_real_GetFileAttributesW)
        return 0u;
    return patch_module_iat_(module);
}

