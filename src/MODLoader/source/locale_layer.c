#include "platform.h"
#include "locale_layer.h"
#include "locale_bootstrap.h"
#include "native_locale.h"
#include "mod_loader.h"

/*
 * 《幽城幻剑录》台湾繁中 Locale Layer。
 *
 * 本层与 native_locale.c 一起组成 v0.2.8 的 Locale Emulator 行为等价层。这里不复制 LE 源码，
 * 而是按照用户明确选中的四项设置以及 v0.2.7 实机暴露出的 NLS 缓存缺口独立重写：
 *   1. 中文（台湾）：zh-TW / LCID 0x0404 / ANSI CP950 / OEM CP950；
 *   2. 台北时区：Taipei Standard Time / UTC+8 / 无夏令时；
 *   3. 伪造语言相关注册表键值；
 *   4. 伪造系统 UI 语言。
 *
 * 为什么 Hook 数量不能只看 RPG.exe 当前直接 import：
 *   Locale Emulator 的经验说明，同一个“系统区域”设置会通过多个入口被旧程序、CRT、系统 DLL 查询。
 *   所以这里覆盖这些设置真正相关的公开 API；但不会引入与上述四项无关的 LE Hook。
 *
 * v0.2.4 把“真正切换 NLS 表”的工作从普通 Mod Core 中完全拆出去：
 *   CastleLocaleBootstrap.dll 在更早的启动阶段只依赖 ntdll，先完成
 *   c_950.nls / l_intl.nls -> RtlInitNlsTables -> RtlResetRtlTranslations -> PEB NLS table pointers。
 *
 * 本文件运行时 KERNEL32 已经可以安全使用，所以这里只负责：
 *   1. 确认早期 Bootstrap 的 NLS 状态；
 *   2. 做 Windows 10 19042+ 的代码页缓存/hash 补充；
 *   3. 安装 zh-TW / 时区 / 注册表 / UI 语言相关 Win32 IAT Hook。
 *
 * 这样不会再像 v0.2.3 那样，在一个静态依赖 KERNEL32 的 DllMain 里同时尝试“pre-Kernel32 NLS 重建”。
 *
 * 真正 NLS 核心通过以后，才做两项新版 Windows 的补充动作：
 *   1. 在 x86 PEB 的现代 ANSI/OEM code-page 编号缓存字段写入 950；
 *   2. Windows 10 build 19042 及以后，尝试刷新 KernelBase 内部的 ANSI/OEM NLS hash。
 *
 * 第二项没有复制 LE 的 opcode-walker。这里独立实现了更窄的“结构锚点 + rel32 目标落在可执行节”定位器：
 * 只有每一层调用关系都能安全闭合时才调用内部刷新函数；任一条件不成立就只记警告，绝不猜地址执行。
 * 因为这仍依赖 Windows 私有实现细节，所以在 v0.2.4 仍必须作为实机验收重点，不能因为静态检查通过就宣布稳定。
 */

#define LANG_ZH_TW_ 0x0404u
#define CP_BIG5_ 950u
#define LOCALE_ZH_TW_ 0x0404u
#define LOCALE_NAME_ZH_TW_ L"zh-TW"
#define MAX_LOCALE_MODULES_ 8u
#define MAX_REG_TEXT_ 64u

/* 一些 SDK 类型的最小等价声明。 */
typedef DWORD LCID_;
typedef WORD LANGID_;
typedef DWORD LCTYPE_;

/* ---------- 真实 API 函数指针 ---------- */

typedef UINT (WINAPI *PFN_GetACP_)(void);
typedef UINT (WINAPI *PFN_GetOEMCP_)(void);
typedef BOOL (WINAPI *PFN_GetCPInfo_)(UINT, LPVOID);
typedef int (WINAPI *PFN_MultiByteToWideChar_)(UINT, DWORD, LPCSTR, int, LPWSTR, int);
typedef int (WINAPI *PFN_WideCharToMultiByte_)(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, BOOL*);
typedef LANGID_ (WINAPI *PFN_GetLangId_)(void);
typedef LCID_ (WINAPI *PFN_GetLCID_)(void);
typedef BOOL (WINAPI *PFN_SetThreadLocale_)(LCID_);
typedef LANGID_ (WINAPI *PFN_SetThreadUILanguage_)(LANGID_);
typedef int (WINAPI *PFN_GetLocaleInfoA_)(LCID_, LCTYPE_, LPSTR, int);
typedef int (WINAPI *PFN_GetLocaleInfoW_)(LCID_, LCTYPE_, LPWSTR, int);
typedef int (WINAPI *PFN_CompareStringA_)(LCID_, DWORD, LPCSTR, int, LPCSTR, int);
typedef int (WINAPI *PFN_CompareStringW_)(LCID_, DWORD, LPCWSTR, int, LPCWSTR, int);
typedef int (WINAPI *PFN_LCMapStringA_)(LCID_, DWORD, LPCSTR, int, LPSTR, int);
typedef int (WINAPI *PFN_LCMapStringW_)(LCID_, DWORD, LPCWSTR, int, LPWSTR, int);
typedef BOOL (WINAPI *PFN_GetStringTypeA_)(LCID_, DWORD, LPCSTR, int, WORD*);
typedef DWORD (WINAPI *PFN_GetTimeZoneInformation_)(TIME_ZONE_INFORMATION_*);
typedef DWORD (WINAPI *PFN_GetDynamicTimeZoneInformation_)(DYNAMIC_TIME_ZONE_INFORMATION_*);
typedef int (WINAPI *PFN_LCIDToLocaleName_)(LCID_, LPWSTR, int, DWORD);
typedef LCID_ (WINAPI *PFN_LocaleNameToLCID_)(LPCWSTR, DWORD);
typedef int (WINAPI *PFN_GetDefaultLocaleName_)(LPWSTR, int);
typedef LANGID_ (WINAPI *PFN_GetUILanguage_)(void);
typedef BOOL (WINAPI *PFN_GetPreferredUILanguages_)(DWORD, DWORD*, LPWSTR, DWORD*);
typedef LSTATUS (WINAPI *PFN_SHGetValueA_)(HKEY, LPCSTR, LPCSTR, DWORD*, LPVOID, DWORD*);
typedef LSTATUS (WINAPI *PFN_SHGetValueW_)(HKEY, LPCWSTR, LPCWSTR, DWORD*, LPVOID, DWORD*);

/*
 * v0.2.8 新增的“文件名级转区自检”必须绕过我们自己的 IAT Hook。
 * 因此 CreateFileA 和 GetFinalPathNameByHandleW 都通过 GetProcAddress 取得真实 KERNEL32 导出地址，
 * 不把测试结果建立在 Loader 自己返回的伪造值上。
 */
typedef HANDLE (WINAPI *PFN_CreateFileA_)(LPCSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE);
typedef DWORD (WINAPI *PFN_GetFinalPathNameByHandleW_)(HANDLE, LPWSTR, DWORD, DWORD);

static PFN_GetACP_ g_real_GetACP;
static PFN_GetOEMCP_ g_real_GetOEMCP;
static PFN_GetCPInfo_ g_real_GetCPInfo;
static PFN_MultiByteToWideChar_ g_real_MultiByteToWideChar;
static PFN_WideCharToMultiByte_ g_real_WideCharToMultiByte;
static PFN_SetThreadLocale_ g_real_SetThreadLocale;
static PFN_SetThreadUILanguage_ g_real_SetThreadUILanguage;
static PFN_GetLocaleInfoA_ g_real_GetLocaleInfoA;
static PFN_GetLocaleInfoW_ g_real_GetLocaleInfoW;
static PFN_CompareStringA_ g_real_CompareStringA;
static PFN_CompareStringW_ g_real_CompareStringW;
static PFN_LCMapStringA_ g_real_LCMapStringA;
static PFN_LCMapStringW_ g_real_LCMapStringW;
static PFN_GetStringTypeA_ g_real_GetStringTypeA;
static PFN_LCIDToLocaleName_ g_real_LCIDToLocaleName;
static PFN_LocaleNameToLCID_ g_real_LocaleNameToLCID;
static PFN_SHGetValueA_ g_real_SHGetValueA;
static PFN_SHGetValueW_ g_real_SHGetValueW;
static PFN_CreateFileA_ g_real_CreateFileA;
static PFN_GetFinalPathNameByHandleW_ g_real_GetFinalPathNameByHandleW;
static int g_file_name_self_test_done;
static int g_file_name_self_test_result;

/* ---------- 最小字符串工具 ---------- */

static UINT alen_(const char* s) {
    UINT n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}

static UINT wlen_(const WCHAR* s) {
    UINT n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}

static char afold_(char c) {
    if (c >= 'A' && c <= 'Z') return (char)(c + ('a' - 'A'));
    return c;
}

static WCHAR wfold_(WCHAR c) {
    if (c >= (WCHAR)'A' && c <= (WCHAR)'Z') return (WCHAR)(c + ((WCHAR)'a' - (WCHAR)'A'));
    return c;
}

static int aeq_i_(const char* a, const char* b) {
    UINT i = 0;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        if (afold_(a[i]) != afold_(b[i])) return 0;
        ++i;
    }
    return a[i] == 0 && b[i] == 0;
}

static int weq_i_(const WCHAR* a, const WCHAR* b) {
    UINT i = 0;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        if (wfold_(a[i]) != wfold_(b[i])) return 0;
        ++i;
    }
    return a[i] == 0 && b[i] == 0;
}

static void wcopy_fixed_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT i = 0;
    if (!dst || !cap) return;
    while (src && src[i] && i + 1u < cap) { dst[i] = src[i]; ++i; }
    dst[i] = 0;
}

static UINT normalize_cp_(UINT cp) {
    if (cp == CP_ACP_ || cp == CP_OEMCP_ || cp == CP_THREAD_ACP_) return CP_BIG5_;
    return cp;
}

static LCID_ normalize_lcid_(LCID_ lcid) {
    if (lcid == LOCALE_SYSTEM_DEFAULT_ || lcid == LOCALE_USER_DEFAULT_ || lcid == 0u)
        return LOCALE_ZH_TW_;
    return lcid;
}

/* ---------- CP950 / Locale API ---------- */

static UINT WINAPI Hook_GetACP_(void) { return CP_BIG5_; }
static UINT WINAPI Hook_GetOEMCP_(void) { return CP_BIG5_; }

static BOOL WINAPI Hook_GetCPInfo_(UINT cp, LPVOID info) {
    return g_real_GetCPInfo ? g_real_GetCPInfo(normalize_cp_(cp), info) : FALSE_;
}

static int WINAPI Hook_MultiByteToWideChar_(UINT cp, DWORD flags, LPCSTR src, int src_len, LPWSTR dst, int dst_len) {
    if (!g_real_MultiByteToWideChar) return 0;
    return g_real_MultiByteToWideChar(normalize_cp_(cp), flags, src, src_len, dst, dst_len);
}

static int WINAPI Hook_WideCharToMultiByte_(UINT cp, DWORD flags, LPCWSTR src, int src_len, LPSTR dst, int dst_len,
                                             LPCSTR defch, BOOL* used) {
    if (!g_real_WideCharToMultiByte) return 0;
    return g_real_WideCharToMultiByte(normalize_cp_(cp), flags, src, src_len, dst, dst_len, defch, used);
}

static LANGID_ WINAPI Hook_GetSystemDefaultLangID_(void) { return (LANGID_)LANG_ZH_TW_; }
static LANGID_ WINAPI Hook_GetUserDefaultLangID_(void) { return (LANGID_)LANG_ZH_TW_; }
static LCID_ WINAPI Hook_GetSystemDefaultLCID_(void) { return LOCALE_ZH_TW_; }
static LCID_ WINAPI Hook_GetUserDefaultLCID_(void) { return LOCALE_ZH_TW_; }
static LCID_ WINAPI Hook_GetThreadLocale_(void) { return LOCALE_ZH_TW_; }

static int WINAPI Hook_GetLocaleInfoA_(LCID_ lcid, LCTYPE_ type, LPSTR out, int cap) {
    return g_real_GetLocaleInfoA ? g_real_GetLocaleInfoA(normalize_lcid_(lcid), type, out, cap) : 0;
}
static int WINAPI Hook_GetLocaleInfoW_(LCID_ lcid, LCTYPE_ type, LPWSTR out, int cap) {
    return g_real_GetLocaleInfoW ? g_real_GetLocaleInfoW(normalize_lcid_(lcid), type, out, cap) : 0;
}
static int WINAPI Hook_CompareStringA_(LCID_ lcid, DWORD flags, LPCSTR a, int ac, LPCSTR b, int bc) {
    return g_real_CompareStringA ? g_real_CompareStringA(normalize_lcid_(lcid), flags, a, ac, b, bc) : 0;
}
static int WINAPI Hook_CompareStringW_(LCID_ lcid, DWORD flags, LPCWSTR a, int ac, LPCWSTR b, int bc) {
    return g_real_CompareStringW ? g_real_CompareStringW(normalize_lcid_(lcid), flags, a, ac, b, bc) : 0;
}
static int WINAPI Hook_LCMapStringA_(LCID_ lcid, DWORD flags, LPCSTR src, int count, LPSTR dst, int cap) {
    return g_real_LCMapStringA ? g_real_LCMapStringA(normalize_lcid_(lcid), flags, src, count, dst, cap) : 0;
}
static int WINAPI Hook_LCMapStringW_(LCID_ lcid, DWORD flags, LPCWSTR src, int count, LPWSTR dst, int cap) {
    return g_real_LCMapStringW ? g_real_LCMapStringW(normalize_lcid_(lcid), flags, src, count, dst, cap) : 0;
}
static BOOL WINAPI Hook_GetStringTypeA_(LCID_ lcid, DWORD type, LPCSTR src, int count, WORD* out) {
    return g_real_GetStringTypeA ? g_real_GetStringTypeA(normalize_lcid_(lcid), type, src, count, out) : FALSE_;
}

static int WINAPI Hook_LCIDToLocaleName_(LCID_ lcid, LPWSTR out, int cap, DWORD flags) {
    if (normalize_lcid_(lcid) == LOCALE_ZH_TW_) {
        const WCHAR* name = (const WCHAR*)LOCALE_NAME_ZH_TW_;
        UINT len = wlen_(name) + 1u, i;
        (void)flags;
        if (!out || cap == 0) return (int)len;
        if ((UINT)cap < len) return 0;
        for (i = 0u; i < len; ++i) out[i] = name[i];
        return (int)len;
    }
    return g_real_LCIDToLocaleName ? g_real_LCIDToLocaleName(lcid, out, cap, flags) : 0;
}

static LCID_ WINAPI Hook_LocaleNameToLCID_(LPCWSTR name, DWORD flags) {
    if (name && weq_i_(name, (const WCHAR*)LOCALE_NAME_ZH_TW_)) return LOCALE_ZH_TW_;
    return g_real_LocaleNameToLCID ? g_real_LocaleNameToLCID(name, flags) : 0u;
}

static int copy_locale_name_(LPWSTR out, int cap) {
    const WCHAR* name = (const WCHAR*)LOCALE_NAME_ZH_TW_;
    UINT len = wlen_(name) + 1u, i;
    if (!out || cap <= 0 || (UINT)cap < len) return 0;
    for (i = 0; i < len; ++i) out[i] = name[i];
    return (int)len;
}
static int WINAPI Hook_GetUserDefaultLocaleName_(LPWSTR out, int cap) { return copy_locale_name_(out, cap); }
static int WINAPI Hook_GetSystemDefaultLocaleName_(LPWSTR out, int cap) { return copy_locale_name_(out, cap); }

/* ---------- “伪造系统 UI 语言” ---------- */

static LANGID_ WINAPI Hook_GetUserDefaultUILanguage_(void) { return (LANGID_)LANG_ZH_TW_; }
static LANGID_ WINAPI Hook_GetSystemDefaultUILanguage_(void) { return (LANGID_)LANG_ZH_TW_; }
static LANGID_ WINAPI Hook_GetThreadUILanguage_(void) { return (LANGID_)LANG_ZH_TW_; }

static BOOL fill_preferred_ui_(DWORD* count, LPWSTR buffer, DWORD* cch) {
    /* MULTI_SZ: 'z','h','-','T','W','\0','\0'，共 7 个 WCHAR。 */
    static const WCHAR value[7] = {(WCHAR)'z',(WCHAR)'h',(WCHAR)'-',(WCHAR)'T',(WCHAR)'W',0,0};
    UINT i;
    if (!count || !cch) return FALSE_;
    *count = 1u;
    if (!buffer || *cch < 7u) {
        *cch = 7u;
        return buffer ? FALSE_ : TRUE_;
    }
    for (i = 0u; i < 7u; ++i) buffer[i] = value[i];
    *cch = 7u;
    return TRUE_;
}
static BOOL WINAPI Hook_GetUserPreferredUILanguages_(DWORD flags, DWORD* count, LPWSTR buffer, DWORD* cch) {
    (void)flags; return fill_preferred_ui_(count, buffer, cch);
}
static BOOL WINAPI Hook_GetSystemPreferredUILanguages_(DWORD flags, DWORD* count, LPWSTR buffer, DWORD* cch) {
    (void)flags; return fill_preferred_ui_(count, buffer, cch);
}
static BOOL WINAPI Hook_GetThreadPreferredUILanguages_(DWORD flags, DWORD* count, LPWSTR buffer, DWORD* cch) {
    (void)flags; return fill_preferred_ui_(count, buffer, cch);
}

/* ---------- “台北 UTC+8” ---------- */

static void clear_systemtime_(SYSTEMTIME_* t) {
    BYTE* p = (BYTE*)t;
    UINT i;
    for (i = 0u; i < (UINT)sizeof(*t); ++i) p[i] = 0u;
}

static void fill_taipei_tzi_(TIME_ZONE_INFORMATION_* tzi) {
    BYTE* p;
    UINT i;
    if (!tzi) return;
    p = (BYTE*)tzi;
    for (i = 0u; i < (UINT)sizeof(*tzi); ++i) p[i] = 0u;
    /* Win32 Bias 的定义是“本地时间加 Bias = UTC”，所以 UTC+8 是 -480 分钟。 */
    tzi->Bias = -480;
    wcopy_fixed_(tzi->StandardName, 32u, (const WCHAR*)L"Taipei Standard Time");
    tzi->StandardBias = 0;
    clear_systemtime_(&tzi->StandardDate);
    tzi->DaylightBias = 0;
    clear_systemtime_(&tzi->DaylightDate);
}

static DWORD WINAPI Hook_GetTimeZoneInformation_(TIME_ZONE_INFORMATION_* tzi) {
    fill_taipei_tzi_(tzi);
    return TIME_ZONE_ID_UNKNOWN_;
}

static DWORD WINAPI Hook_GetDynamicTimeZoneInformation_(DYNAMIC_TIME_ZONE_INFORMATION_* dtzi) {
    TIME_ZONE_INFORMATION_* tzi = (TIME_ZONE_INFORMATION_*)dtzi;
    if (!dtzi) return TIME_ZONE_ID_UNKNOWN_;
    fill_taipei_tzi_(tzi);
    wcopy_fixed_(dtzi->TimeZoneKeyName, 128u, (const WCHAR*)L"Taipei Standard Time");
    dtzi->DynamicDaylightTimeDisabled = TRUE_;
    return TIME_ZONE_ID_UNKNOWN_;
}

/* ---------- “伪造语言相关注册表键值” ---------- */

static int asubkey_eq_(LPCSTR a, LPCSTR b) { return aeq_i_(a ? a : "", b); }
static int aval_eq_(LPCSTR a, LPCSTR b) { return aeq_i_(a ? a : "", b); }

static int registry_fake_a_(HKEY root, LPCSTR subkey, LPCSTR value, DWORD* type, LPVOID data, DWORD* bytes) {
    const char* text = NULL_PTR;
    DWORD out_type = REG_SZ_;
    DWORD need;
    UINT i;

    if (!bytes) return 0;

    if (root == HKEY_LOCAL_MACHINE_ && asubkey_eq_(subkey, "System\\CurrentControlSet\\Control\\Nls\\CodePage")) {
        if (aval_eq_(value, "InstallLanguage") || aval_eq_(value, "Default")) text = "1028";
        else if (aval_eq_(value, "OEMCP") || aval_eq_(value, "ACP")) text = "950";
    } else if (root == HKEY_CURRENT_USER_ && asubkey_eq_(subkey, "Control Panel\\International")) {
        if (aval_eq_(value, "Locale")) text = "00000404";
        else if (aval_eq_(value, "LocaleName")) text = "zh-TW";
    } else if (root == HKEY_CURRENT_USER_ && asubkey_eq_(subkey, "Control Panel\\Desktop") &&
               aval_eq_(value, "PreferredUILanguages")) {
        text = "zh-TW"; out_type = REG_MULTI_SZ_;
    } else if (root == HKEY_CURRENT_USER_ && asubkey_eq_(subkey, "Control Panel\\Desktop\\MuiCached") &&
               aval_eq_(value, "MachinePreferredUILanguages")) {
        text = "zh-TW"; out_type = REG_MULTI_SZ_;
    }
    if (!text) return 0;

    need = (DWORD)(alen_(text) + 1u);
    if (out_type == REG_MULTI_SZ_) ++need; /* 额外一个 NUL 形成 MULTI_SZ 双 NUL 结尾。 */
    if (type) *type = out_type;
    if (!data || *bytes < need) { *bytes = need; return 2; }
    for (i = 0u; i < need; ++i) ((char*)data)[i] = 0;
    for (i = 0u; text[i]; ++i) ((char*)data)[i] = text[i];
    *bytes = need;
    return 1;
}

static int registry_fake_w_(HKEY root, LPCWSTR subkey, LPCWSTR value, DWORD* type, LPVOID data, DWORD* bytes) {
    WCHAR sub_a[MAX_REG_TEXT_], val_a[MAX_REG_TEXT_];
    char sub8[MAX_REG_TEXT_], val8[MAX_REG_TEXT_];
    UINT i;
    if (!subkey || !value) return 0;
    for (i = 0u; i + 1u < MAX_REG_TEXT_ && subkey[i]; ++i) sub_a[i] = subkey[i];
    sub_a[i] = 0;
    for (i = 0u; i + 1u < MAX_REG_TEXT_ && value[i]; ++i) val_a[i] = value[i];
    val_a[i] = 0;
    for (i = 0u; sub_a[i] && i + 1u < MAX_REG_TEXT_; ++i) sub8[i] = (char)(sub_a[i] & 0x7Fu);
    sub8[i] = 0;
    for (i = 0u; val_a[i] && i + 1u < MAX_REG_TEXT_; ++i) val8[i] = (char)(val_a[i] & 0x7Fu);
    val8[i] = 0;

    /* 先用 ANSI 规则得到字符串，再按类型转换成 WCHAR。 */
    {
        char tmp[64] = {0};
        DWORD tmp_bytes = sizeof(tmp), tmp_type = 0;
        int r = registry_fake_a_(root, sub8, val8, &tmp_type, tmp, &tmp_bytes);
        DWORD chars, need_bytes;
        if (!r) return 0;
        if (r == 2) {
            /* 最大文本很短；这里只需要向调用者报告宽字符版本所需大小。 */
            tmp_bytes = sizeof(tmp);
            registry_fake_a_(root, sub8, val8, &tmp_type, tmp, &tmp_bytes);
        }
        chars = 0u;
        while (chars < tmp_bytes && tmp[chars]) ++chars;
        need_bytes = (chars + 1u + (tmp_type == REG_MULTI_SZ_ ? 1u : 0u)) * (DWORD)sizeof(WCHAR);
        if (type) *type = tmp_type;
        if (!data || !bytes || *bytes < need_bytes) { if (bytes) *bytes = need_bytes; return 2; }
        for (i = 0u; i < need_bytes / sizeof(WCHAR); ++i) ((WCHAR*)data)[i] = 0;
        for (i = 0u; i < chars; ++i) ((WCHAR*)data)[i] = (WCHAR)(BYTE)tmp[i];
        *bytes = need_bytes;
        return 1;
    }
}

static LSTATUS WINAPI Hook_SHGetValueA_(HKEY root, LPCSTR subkey, LPCSTR value, DWORD* type, LPVOID data, DWORD* bytes) {
    int r = registry_fake_a_(root, subkey, value, type, data, bytes);
    if (r == 1) return ERROR_SUCCESS_;
    if (r == 2) return ERROR_MORE_DATA_;
    return g_real_SHGetValueA ? g_real_SHGetValueA(root, subkey, value, type, data, bytes) : 2L;
}
static LSTATUS WINAPI Hook_SHGetValueW_(HKEY root, LPCWSTR subkey, LPCWSTR value, DWORD* type, LPVOID data, DWORD* bytes) {
    int r = registry_fake_w_(root, subkey, value, type, data, bytes);
    if (r == 1) return ERROR_SUCCESS_;
    if (r == 2) return ERROR_MORE_DATA_;
    return g_real_SHGetValueW ? g_real_SHGetValueW(root, subkey, value, type, data, bytes) : 2L;
}

/* ---------- PE32 IAT 分发 ---------- */

static DWORD hook_address_for_(const char* dll, const char* fn) {
    int kernel = aeq_i_(dll, "KERNEL32.dll") || aeq_i_(dll, "KERNELBASE.dll");
    if (kernel) {
        if (aeq_i_(fn, "GetACP")) return (DWORD)(SIZE_T)&Hook_GetACP_;
        if (aeq_i_(fn, "GetOEMCP")) return (DWORD)(SIZE_T)&Hook_GetOEMCP_;
        if (aeq_i_(fn, "GetCPInfo")) return (DWORD)(SIZE_T)&Hook_GetCPInfo_;
        if (aeq_i_(fn, "MultiByteToWideChar")) return (DWORD)(SIZE_T)&Hook_MultiByteToWideChar_;
        if (aeq_i_(fn, "WideCharToMultiByte")) return (DWORD)(SIZE_T)&Hook_WideCharToMultiByte_;
        if (aeq_i_(fn, "GetSystemDefaultLangID")) return (DWORD)(SIZE_T)&Hook_GetSystemDefaultLangID_;
        if (aeq_i_(fn, "GetUserDefaultLangID")) return (DWORD)(SIZE_T)&Hook_GetUserDefaultLangID_;
        if (aeq_i_(fn, "GetSystemDefaultLCID")) return (DWORD)(SIZE_T)&Hook_GetSystemDefaultLCID_;
        if (aeq_i_(fn, "GetUserDefaultLCID")) return (DWORD)(SIZE_T)&Hook_GetUserDefaultLCID_;
        if (aeq_i_(fn, "GetThreadLocale")) return (DWORD)(SIZE_T)&Hook_GetThreadLocale_;
        if (aeq_i_(fn, "GetLocaleInfoA")) return (DWORD)(SIZE_T)&Hook_GetLocaleInfoA_;
        if (aeq_i_(fn, "GetLocaleInfoW")) return (DWORD)(SIZE_T)&Hook_GetLocaleInfoW_;
        if (aeq_i_(fn, "CompareStringA")) return (DWORD)(SIZE_T)&Hook_CompareStringA_;
        if (aeq_i_(fn, "CompareStringW")) return (DWORD)(SIZE_T)&Hook_CompareStringW_;
        if (aeq_i_(fn, "LCMapStringA")) return (DWORD)(SIZE_T)&Hook_LCMapStringA_;
        if (aeq_i_(fn, "LCMapStringW")) return (DWORD)(SIZE_T)&Hook_LCMapStringW_;
        if (aeq_i_(fn, "GetStringTypeA")) return (DWORD)(SIZE_T)&Hook_GetStringTypeA_;
        if (aeq_i_(fn, "LCIDToLocaleName")) return (DWORD)(SIZE_T)&Hook_LCIDToLocaleName_;
        if (aeq_i_(fn, "LocaleNameToLCID")) return (DWORD)(SIZE_T)&Hook_LocaleNameToLCID_;
        if (aeq_i_(fn, "GetUserDefaultLocaleName")) return (DWORD)(SIZE_T)&Hook_GetUserDefaultLocaleName_;
        if (aeq_i_(fn, "GetSystemDefaultLocaleName")) return (DWORD)(SIZE_T)&Hook_GetSystemDefaultLocaleName_;
        if (aeq_i_(fn, "GetUserDefaultUILanguage")) return (DWORD)(SIZE_T)&Hook_GetUserDefaultUILanguage_;
        if (aeq_i_(fn, "GetSystemDefaultUILanguage")) return (DWORD)(SIZE_T)&Hook_GetSystemDefaultUILanguage_;
        if (aeq_i_(fn, "GetThreadUILanguage")) return (DWORD)(SIZE_T)&Hook_GetThreadUILanguage_;
        if (aeq_i_(fn, "GetUserPreferredUILanguages")) return (DWORD)(SIZE_T)&Hook_GetUserPreferredUILanguages_;
        if (aeq_i_(fn, "GetSystemPreferredUILanguages")) return (DWORD)(SIZE_T)&Hook_GetSystemPreferredUILanguages_;
        if (aeq_i_(fn, "GetThreadPreferredUILanguages")) return (DWORD)(SIZE_T)&Hook_GetThreadPreferredUILanguages_;
        if (aeq_i_(fn, "GetTimeZoneInformation")) return (DWORD)(SIZE_T)&Hook_GetTimeZoneInformation_;
        if (aeq_i_(fn, "GetDynamicTimeZoneInformation")) return (DWORD)(SIZE_T)&Hook_GetDynamicTimeZoneInformation_;
    }
    if (aeq_i_(dll, "SHLWAPI.dll")) {
        if (aeq_i_(fn, "SHGetValueA")) return (DWORD)(SIZE_T)&Hook_SHGetValueA_;
        if (aeq_i_(fn, "SHGetValueW")) return (DWORD)(SIZE_T)&Hook_SHGetValueW_;
    }
    return 0u;
}

UINT LocaleLayer_PatchModule(HMODULE module) {
    BYTE* base = (BYTE*)module;
    DWORD pe_off, optional_off, import_rva, desc_rva;
    WORD magic;
    UINT patched = 0u;

    if (!module || *(WORD*)base != IMAGE_DOS_SIGNATURE_) return 0u;
    pe_off = *(DWORD*)(base + 0x3Cu);
    if (*(DWORD*)(base + pe_off) != IMAGE_NT_SIGNATURE_) return 0u;
    optional_off = pe_off + 24u;
    magic = *(WORD*)(base + optional_off);
    if (magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC_) return 0u;
    import_rva = *(DWORD*)(base + optional_off + 96u + IMAGE_DIRECTORY_ENTRY_IMPORT_ * 8u);
    if (!import_rva) return 0u;

    desc_rva = import_rva;
    for (;;) {
        DWORD* desc = (DWORD*)(base + desc_rva);
        DWORD oft = desc[0], name_rva = desc[3], ft = desc[4];
        const char* dll;
        DWORD idx = 0u;
        if (!desc[0] && !desc[1] && !desc[2] && !desc[3] && !desc[4]) break;
        desc_rva += 20u;
        if (!oft || !name_rva || !ft) continue;
        dll = (const char*)(base + name_rva);
        for (;;) {
            DWORD name_thunk = *(DWORD*)(base + oft + idx * 4u);
            DWORD* slot = (DWORD*)(base + ft + idx * 4u);
            const char* fn;
            DWORD hook, oldp = 0u, ignored = 0u;
            if (!name_thunk) break;
            ++idx;
            if (name_thunk & IMAGE_ORDINAL_FLAG32_) continue;
            fn = (const char*)(base + name_thunk + 2u);
            hook = hook_address_for_(dll, fn);
            if (!hook) continue;
            if (!VirtualProtect(slot, 4u, PAGE_READWRITE_, &oldp)) continue;
            *slot = hook;
            VirtualProtect(slot, 4u, oldp, &ignored);
            ++patched;
        }
    }
    if (patched) FlushInstructionCache(GetCurrentProcess(), module, 1u);
    return patched;
}


/* ---------- Windows 10 20H2+：刷新 ANSI/OEM NLS hash ---------- */

/*
 * Locale Emulator 对 CP950 设置还有一个非常重要的现代 Windows 分支：
 * Windows 10 build 19042 之后，仅修改 x86 PEB 的 ANSI/OEM code-page 字段可能仍留下 KernelBase 的 NLS hash 缓存。
 * LE 会定位 KernelBase 内部的 SetupAnsiOemCodeHashNodes 并主动刷新一次。
 *
 * 这里独立实现“相同目标”，但刻意不复制 LE 的 opcode-walker：
 *   - 只接受 KernelBase 自身可执行节内的 E8 rel32 调用目标；
 *   - 每一级都必须落在可执行节；
 *   - 必须找到 BaseNls 初始化附近的 `mov eax, 0x190` 固定结构标记；
 *   - 任一步不成立就返回 0，仅记录警告，绝不拿一个“差不多的地址”去调用。
 *
 * 这是一个 Windows 私有实现定位，因此永远只能作为兼容增强，不能把它当公开 ABI。
 */
#define IMAGE_SCN_MEM_EXECUTE_ 0x20000000u

typedef struct RTL_OSVERSIONINFOW_MIN_ {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR szCSDVersion[128];
} RTL_OSVERSIONINFOW_MIN_;

typedef LONG (WINAPI *PFN_RtlGetVersion_)(RTL_OSVERSIONINFOW_MIN_*);
typedef DWORD (WINAPI *PFN_SetupAnsiOemCodeHashNodes_)(void);

static void zero_small_(BYTE* p, UINT count) {
    UINT i;
    for (i = 0u; i < count; ++i) p[i] = 0u;
}

static int module_exec_bounds_(HMODULE module, BYTE** image_begin, DWORD* image_size) {
    BYTE* base = (BYTE*)module;
    DWORD pe, opt;
    if (!base || !image_begin || !image_size || *(WORD*)base != IMAGE_DOS_SIGNATURE_) return 0;
    pe = *(DWORD*)(base + 0x3Cu);
    if (*(DWORD*)(base + pe) != IMAGE_NT_SIGNATURE_) return 0;
    opt = pe + 24u;
    if (*(WORD*)(base + opt) != IMAGE_NT_OPTIONAL_HDR32_MAGIC_) return 0;
    *image_begin = base;
    *image_size = *(DWORD*)(base + opt + 56u); /* OptionalHeader.SizeOfImage */
    return *image_size != 0u;
}

static int pointer_in_exec_section_(HMODULE module, BYTE* ptr) {
    BYTE* base = (BYTE*)module;
    DWORD pe, opt, sec_off;
    WORD count, opt_size;
    UINT i;
    if (!base || !ptr || *(WORD*)base != IMAGE_DOS_SIGNATURE_) return 0;
    pe = *(DWORD*)(base + 0x3Cu);
    if (*(DWORD*)(base + pe) != IMAGE_NT_SIGNATURE_) return 0;
    count = *(WORD*)(base + pe + 6u);
    opt_size = *(WORD*)(base + pe + 20u);
    opt = pe + 24u;
    sec_off = opt + opt_size;
    for (i = 0u; i < count; ++i) {
        BYTE* sh = base + sec_off + i * 40u;
        DWORD vs = *(DWORD*)(sh + 8u);
        DWORD va = *(DWORD*)(sh + 12u);
        DWORD raw = *(DWORD*)(sh + 16u);
        DWORD chars = *(DWORD*)(sh + 36u);
        DWORD span = vs > raw ? vs : raw;
        if (!(chars & IMAGE_SCN_MEM_EXECUTE_)) continue;
        if (ptr >= base + va && ptr < base + va + span) return 1;
    }
    return 0;
}

static BYTE* decode_rel32_call_target_(HMODULE module, BYTE* call) {
    DWORD u;
    LONG rel;
    BYTE* target;
    if (!call || call[0] != 0xE8u) return NULL_PTR;
    u = (DWORD)call[1] | ((DWORD)call[2] << 8) | ((DWORD)call[3] << 16) | ((DWORD)call[4] << 24);
    rel = (LONG)u;
    target = call + 5u + rel;
    return pointer_in_exec_section_(module, target) ? target : NULL_PTR;
}

/*
 * 在很短的已确认初始化函数片段内寻找“第几个有效 direct CALL”。
 * 这里不是通用反汇编器，因此必须配合 pointer_in_exec_section_ 的目标验证；
 * 如果某次 Windows 更新破坏了结构，本函数宁可找不到并安全退出。
 */
static BYTE* find_kth_valid_call_(HMODULE module, BYTE* start, UINT range_each, UINT kth) {
    BYTE* cursor = start;
    UINT k;
    for (k = 0u; k < kth; ++k) {
        UINT i;
        BYTE* found = NULL_PTR;
        if (!pointer_in_exec_section_(module, cursor)) return NULL_PTR;
        for (i = 0u; i + 5u <= range_each; ++i) {
            BYTE* p = cursor + i;
            BYTE* target;
            if (p[0] != 0xE8u) continue;
            target = decode_rel32_call_target_(module, p);
            if (!target) continue;
            found = target;
            cursor = p + 5u;
            break;
        }
        if (!found) return NULL_PTR;
        if (k + 1u == kth) return found;
    }
    return NULL_PTR;
}

static int refresh_modern_nls_hash_(void) {
    HMODULE ntdll = GetModuleHandleW((const WCHAR*)L"ntdll.dll");
    HMODULE kernelbase = GetModuleHandleW((const WCHAR*)L"kernelbase.dll");
    PFN_RtlGetVersion_ rtl_get_version;
    RTL_OSVERSIONINFOW_MIN_ os;
    BYTE* base;
    DWORD size, pe, opt, entry_rva;
    BYTE *entry, *kernel_init, *base_init, *base_nls = NULL_PTR, *nls_process, *setup;
    UINT i;

    if (!ntdll || !kernelbase) return 0;
    rtl_get_version = (PFN_RtlGetVersion_)GetProcAddress(ntdll, "RtlGetVersion");
    if (!rtl_get_version) return 0;
    zero_small_((BYTE*)&os, (UINT)sizeof(os));
    os.dwOSVersionInfoSize = (DWORD)sizeof(os);
    if (rtl_get_version(&os) != 0L) return 0;

    /* LE 的公开实现同样只对 Windows 10 build 19042+执行这条内部刷新。 */
    if (os.dwMajorVersion < 10u || os.dwBuildNumber < 19042u) return 1;
    if (!module_exec_bounds_(kernelbase, &base, &size)) return 0;

    pe = *(DWORD*)(base + 0x3Cu);
    opt = pe + 24u;
    entry_rva = *(DWORD*)(base + opt + 16u);
    if (!entry_rva || entry_rva >= size) return 0;
    entry = base + entry_rva;
    if (!pointer_in_exec_section_(kernelbase, entry)) return 0;

    kernel_init = find_kth_valid_call_(kernelbase, entry, 0x30u, 1u);
    if (!kernel_init) return 0;
    base_init = find_kth_valid_call_(kernelbase, kernel_init, 0x20u, 2u);
    if (!base_init) return 0;

    /* 结构锚点：LE 经过验证的分支会在 BaseNls 初始化前出现 mov eax, 0x190。 */
    for (i = 0u; i + 5u <= 0x400u; ++i) {
        BYTE* p = base_init + i;
        if (p < base || p + 5u > base + size) break;
        if (p[0] == 0xB8u && p[1] == 0x90u && p[2] == 0x01u && p[3] == 0x00u && p[4] == 0x00u) {
            base_nls = find_kth_valid_call_(kernelbase, p, 0x20u, 1u);
            if (base_nls) break;
        }
    }
    if (!base_nls) return 0;

    nls_process = find_kth_valid_call_(kernelbase, base_nls, 0x30u, 1u);
    if (!nls_process) return 0;
    setup = find_kth_valid_call_(kernelbase, nls_process, 0x30u, 3u);
    if (!setup) return 0;

    ((PFN_SetupAnsiOemCodeHashNodes_)setup)();
    return 1;
}

/* ---------- 初始化真实 API 与底层 Locale 状态 ---------- */

static int resolve_real_apis_(void) {
    HMODULE k32 = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    HMODULE shlw = GetModuleHandleW((const WCHAR*)L"shlwapi.dll");
    if (!k32) return 0;

    g_real_GetACP = (PFN_GetACP_)GetProcAddress(k32, "GetACP");
    g_real_GetOEMCP = (PFN_GetOEMCP_)GetProcAddress(k32, "GetOEMCP");
    g_real_GetCPInfo = (PFN_GetCPInfo_)GetProcAddress(k32, "GetCPInfo");
    g_real_MultiByteToWideChar = (PFN_MultiByteToWideChar_)GetProcAddress(k32, "MultiByteToWideChar");
    g_real_WideCharToMultiByte = (PFN_WideCharToMultiByte_)GetProcAddress(k32, "WideCharToMultiByte");
    g_real_SetThreadLocale = (PFN_SetThreadLocale_)GetProcAddress(k32, "SetThreadLocale");
    g_real_SetThreadUILanguage = (PFN_SetThreadUILanguage_)GetProcAddress(k32, "SetThreadUILanguage");
    g_real_GetLocaleInfoA = (PFN_GetLocaleInfoA_)GetProcAddress(k32, "GetLocaleInfoA");
    g_real_GetLocaleInfoW = (PFN_GetLocaleInfoW_)GetProcAddress(k32, "GetLocaleInfoW");
    g_real_CompareStringA = (PFN_CompareStringA_)GetProcAddress(k32, "CompareStringA");
    g_real_CompareStringW = (PFN_CompareStringW_)GetProcAddress(k32, "CompareStringW");
    g_real_LCMapStringA = (PFN_LCMapStringA_)GetProcAddress(k32, "LCMapStringA");
    g_real_LCMapStringW = (PFN_LCMapStringW_)GetProcAddress(k32, "LCMapStringW");
    g_real_GetStringTypeA = (PFN_GetStringTypeA_)GetProcAddress(k32, "GetStringTypeA");
    g_real_LCIDToLocaleName = (PFN_LCIDToLocaleName_)GetProcAddress(k32, "LCIDToLocaleName");
    g_real_LocaleNameToLCID = (PFN_LocaleNameToLCID_)GetProcAddress(k32, "LocaleNameToLCID");
    /* 文件名级自检只在 EntryPoint 前执行，但真实函数地址可以在这里和其它 KERNEL32 API 一次解析完成。 */
    g_real_CreateFileA = (PFN_CreateFileA_)GetProcAddress(k32, "CreateFileA");
    g_real_GetFinalPathNameByHandleW = (PFN_GetFinalPathNameByHandleW_)GetProcAddress(k32, "GetFinalPathNameByHandleW");
    if (shlw) {
        g_real_SHGetValueA = (PFN_SHGetValueA_)GetProcAddress(shlw, "SHGetValueA");
        g_real_SHGetValueW = (PFN_SHGetValueW_)GetProcAddress(shlw, "SHGetValueW");
    }
    return g_real_GetACP && g_real_GetOEMCP && g_real_MultiByteToWideChar && g_real_WideCharToMultiByte;
}

static BYTE* current_peb_(void) {
#if defined(_M_IX86)
    BYTE* peb = (BYTE*)NULL_PTR;
    __asm {
        mov eax, fs:[30h]
        mov peb, eax
    }
    return peb;
#else
    return (BYTE*)NULL_PTR;
#endif
}

/*
 * Locale Emulator 在 Windows 10 20H2+ 还会更新进程当前 ANSI/OEM 代码页编号缓存，
 * 然后刷新 KernelBase 内部 hash。真正的 NLS 表已经由 CastleLocaleBootstrap.dll 提前切换，
 * 所以这里写的是“现代缓存编号”，不是把代码页编号误当成 PEB NLS table pointer。
 */
static int update_modern_codepage_cache_(void) {
    BYTE* peb = current_peb_();
    if (!peb) return 0;
    *(WORD*)(peb + 0x228u) = (WORD)CP_BIG5_;
    *(WORD*)(peb + 0x22Au) = (WORD)CP_BIG5_;
    return 1;
}

/*
 * 把一个很小的无符号十进制数写入 WCHAR 缓冲区。
 * 这里专门给 Locale 自检日志使用，避免为了打印 950 引入 CRT。
 */
static void uint_to_wtext_(UINT value, WCHAR* out, UINT cap) {
    WCHAR temp[16];
    UINT n = 0u, i;
    if (!out || cap == 0u) return;
    do {
        if (n + 1u >= (UINT)(sizeof(temp) / sizeof(temp[0]))) break;
        temp[n++] = (WCHAR)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u);
    if (n + 1u > cap) { out[0] = 0; return; }
    for (i = 0u; i < n; ++i) out[i] = temp[n - 1u - i];
    out[n] = 0;
}

/*
 * v0.2.5 的第二道硬验证。
 *
 * 这里调用的是 GetProcAddress 得到的真实 KERNEL32 导出地址，不经过本模块稍后安装的 IAT Hook。
 * 因此只有 Windows 当前进程级 ANSI/OEM 运行态真的已经切到 950/950，才会返回成功。
 * 这比 v0.2.4 检查 NLSTABLEINFO 结构里的内部字段更接近最终目标，也更容易从实机日志判读。
 */
static int verify_real_win32_codepage_(void) {
    UINT ansi_cp, oem_cp;
    WCHAR text[16];
    if (!g_real_GetACP || !g_real_GetOEMCP) {
        ModLoader_Log((const WCHAR*)L"[Locale错误] 无法取得真实 GetACP/GetOEMCP，不能验证 CP950 运行态。");
        return 0;
    }

    ansi_cp = g_real_GetACP();
    oem_cp = g_real_GetOEMCP();

    uint_to_wtext_(ansi_cp, text, 16u);
    ModLoader_LogTwo((const WCHAR*)L"[Locale/NLS自检] 真实 GetACP=", text);
    uint_to_wtext_(oem_cp, text, 16u);
    ModLoader_LogTwo((const WCHAR*)L"[Locale/NLS自检] 真实 GetOEMCP=", text);

    if (ansi_cp != CP_BIG5_ || oem_cp != CP_BIG5_) {
        ModLoader_Log((const WCHAR*)L"[Locale错误] Windows 真实 ANSI/OEM 代码页仍未达到 950/950；拒绝把 IAT Hook 的返回值伪装成转区成功。");
        return 0;
    }

    ModLoader_Log((const WCHAR*)L"[Locale/NLS自检] Windows 真实 ANSI/OEM 运行态已达到 CP950/CP950。");
    return 1;
}

/*
 * 读取最早期 CastleLocaleBootstrap.dll 的状态。
 * 这里已经位于普通 KERNEL32 可用阶段，因此可以安全使用 GetModuleHandleW/GetProcAddress。
 * Bootstrap 自己不写日志；这样 pre-Kernel32 阶段不会为了诊断反过来引入 KERNEL32 文件 I/O。
 */
static int bootstrap_nls_ready_(void) {
    HMODULE module;
    FARPROC proc;
    typedef int (__cdecl *PFN_GetBootstrapStatus_)(void);
    typedef unsigned int (__cdecl *PFN_GetObservedCodePage_)(void);
    PFN_GetObservedCodePage_ get_observed_ansi;
    PFN_GetObservedCodePage_ get_observed_oem;
    int status;
    WCHAR text[16];
    UINT n = 0u, pos, value;

    module = GetModuleHandleW((const WCHAR*)CASTLE_LOCALE_BOOTSTRAP_DLL_);
    if (!module) {
        ModLoader_Log((const WCHAR*)L"[Locale错误] CastleLocaleBootstrap.dll 未进入目标进程；CP950 早期 NLS 层不存在。");
        return 0;
    }
    proc = GetProcAddress(module, CASTLE_LOCALE_BOOTSTRAP_STATUS_EXPORT_);
    if (!proc) {
        ModLoader_Log((const WCHAR*)L"[Locale错误] 无法读取 CastleLocaleBootstrap 早期状态导出。");
        return 0;
    }
    status = ((PFN_GetBootstrapStatus_)proc)();

    /*
     * v0.2.5 把 v0.2.4 那两个“只用于内部观察”的 RtlInitNlsTables CodePage 字段写进日志。
     * 它们不再决定启动成败，真正成败由后面的真实 GetACP/GetOEMCP 负责。
     */
    get_observed_ansi = (PFN_GetObservedCodePage_)GetProcAddress(module, "CastleLocaleBootstrap_GetObservedAnsiCodePage");
    get_observed_oem = (PFN_GetObservedCodePage_)GetProcAddress(module, "CastleLocaleBootstrap_GetObservedOemCodePage");
    if (get_observed_ansi) {
        uint_to_wtext_(get_observed_ansi(), text, 16u);
        ModLoader_LogTwo((const WCHAR*)L"[Locale/诊断] RtlInitNlsTables 观察到的 ANSI CodePage 字段=", text);
    }
    if (get_observed_oem) {
        uint_to_wtext_(get_observed_oem(), text, 16u);
        ModLoader_LogTwo((const WCHAR*)L"[Locale/诊断] RtlInitNlsTables 观察到的 OEM CodePage 字段=", text);
    }

    if (status == CASTLE_LOCALE_BOOTSTRAP_OK_) {
        ModLoader_Log((const WCHAR*)L"[Locale/NLS] pre-Kernel32 Bootstrap 已完成 RtlResetRtlTranslations 与 PEB NLS 指针切换。");
        return 1;
    }

    /* 把负数错误码写成十进制，方便实机日志直接告诉我们失败阶段。 */
    text[0] = 0;
    value = (UINT)(status < 0 ? -status : status);
    do { ++n; value /= 10u; } while (value && n < 14u);
    pos = 0u;
    if (status < 0) text[pos++] = (WCHAR)'-';
    value = (UINT)(status < 0 ? -status : status);
    {
        UINT i;
        for (i = 0u; i < n; ++i) {
            UINT divisor = 1u, j;
            for (j = i + 1u; j < n; ++j) divisor *= 10u;
            text[pos++] = (WCHAR)('0' + ((value / divisor) % 10u));
        }
    }
    text[pos] = 0;
    ModLoader_LogTwo((const WCHAR*)L"[Locale错误] pre-Kernel32 CP950 Bootstrap 失败，状态码=", text);

    /*
     * 状态码是给程序判断的稳定 ABI；下面这层中文说明是给实机测试的人看的。
     * 这样用户不需要拿着 -15 再去翻源码，日志本身就能告诉我们失败发生在哪一步。
     */
    switch (status) {
        case -10: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：无法打开 System32\\c_950.nls。"); break;
        case -11: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：CP950 NLS 文件尺寸查询或尺寸合法性检查失败。"); break;
        case -12: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：无法打开 System32\\l_intl.nls。"); break;
        case -13: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：Unicode case NLS 文件尺寸或总内存布局检查失败。"); break;
        case -14: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：无法为 CP950 NLS 表申请目标进程内存。"); break;
        case -15: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：读取 CP950 NLS 文件失败。"); break;
        case -16: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：读取 Unicode case NLS 文件失败。"); break;
        case -17: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：NLS 表内存改为只读保护失败。"); break;
        case -18: ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：无法取得/写回 x86 PEB NLS 数据指针。"); break;
        default:  ModLoader_Log((const WCHAR*)L"[Locale错误] 失败阶段：未知 Bootstrap 状态，请保留完整日志与版本。"); break;
    }
    return 0;
}

static int apply_process_locale_state_(void) {
    /*
     * v0.2.4 的关键边界：普通 Mod Core 不再重建 NLS 表。
     * 它只确认早期 ntdll-only Bootstrap 已经成功，然后补现代缓存和 Win32 层语义。
     */
    if (!bootstrap_nls_ready_()) return 0;

    if (!update_modern_codepage_cache_()) {
        ModLoader_Log((const WCHAR*)L"[Locale错误] 无法写入 Windows 10 ANSI/OEM 代码页缓存。");
        return 0;
    }

    if (refresh_modern_nls_hash_())
        ModLoader_Log((const WCHAR*)L"[Locale] ANSI/OEM NLS hash：已刷新或当前 Windows 不需要额外刷新。");
    else
        ModLoader_Log((const WCHAR*)L"[Locale警告] 无法安全闭合 KernelBase 的现代 NLS hash 刷新链；早期 CP950 NLS 表已经建立，但现代 hash 增强未执行。");

    /*
     * 只有现在真实 GetACP/GetOEMCP 已经返回 950/950，才允许继续安装 IAT Hook。
     * 这样日志中的“Locale 成功”不可能再由 Hook_GetACP/Hook_GetOEMCP 自己制造出来。
     */
    if (!verify_real_win32_codepage_()) return 0;

    if (g_real_SetThreadLocale) g_real_SetThreadLocale(LOCALE_ZH_TW_);
    if (g_real_SetThreadUILanguage) g_real_SetThreadUILanguage((LANGID_)LANG_ZH_TW_);
    return 1;
}


/*
 * 把十进制 PID 追加到一个窄字符路径末尾。
 * 测试文件名必须包含 PID，避免两个游戏实例同时运行时争用同一个探针文件。
 * 这里只处理 32 位无符号整数，完全不需要 CRT 的 sprintf。
 */
static int append_u32_a_(char* dst, UINT cap, DWORD value) {
    char temp[16];
    UINT n = 0u, d = 0u, i;
    if (!dst || cap == 0u) return 0;
    while (dst[d]) {
        if (d + 1u >= cap) return 0;
        ++d;
    }
    do {
        if (n >= (UINT)(sizeof(temp) / sizeof(temp[0]))) return 0;
        temp[n++] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u);
    if (d + n + 1u > cap) return 0;
    for (i = 0u; i < n; ++i) dst[d + i] = temp[n - 1u - i];
    dst[d + n] = 0;
    return 1;
}

/* WCHAR 版本与上面的窄字符版本完全对应，用来构造 CreateFileW 的预期 Unicode 文件名。 */
static int append_u32_w_(WCHAR* dst, UINT cap, DWORD value) {
    WCHAR temp[16];
    UINT n = 0u, d = 0u, i;
    if (!dst || cap == 0u) return 0;
    while (dst[d]) {
        if (d + 1u >= cap) return 0;
        ++d;
    }
    do {
        if (n >= (UINT)(sizeof(temp) / sizeof(temp[0]))) return 0;
        temp[n++] = (WCHAR)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u);
    if (d + n + 1u > cap) return 0;
    for (i = 0u; i < n; ++i) dst[d + i] = temp[n - 1u - i];
    dst[d + n] = 0;
    return 1;
}

/* 精确比较两个 Unicode 字符串。文件名自检不能做大小写折叠，否则会降低验证强度。 */
static int weq_exact_(const WCHAR* a, const WCHAR* b) {
    UINT i = 0u;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        ++i;
    }
    return a[i] == 0 && b[i] == 0;
}

/*
 * v0.2.8 文件名级 CP950 实体自检。
 *
 * 为什么还需要它：
 *   v0.2.6 已经证明真实 GetACP/GetOEMCP 都是 950，但用户真正依赖的是“繁体中文文件名能否打开”。
 *   仅看代码页数字，仍不能直接证明 CreateFileA 最终把 Big5 字节映射到了正确的 NTFS Unicode 文件名。
 *
 * 测试分两步，而且两步都绕过 Loader 自己的 Locale/Overrides IAT Hook：
 *   1. 把一串预先写死的 Big5 原始字节交给真实 MultiByteToWideChar(CP_ACP)，结果必须精确等于预期繁体 Unicode；
 *   2. 先用 CreateFileW 创建该 Unicode 文件，再用真实 CreateFileA + 同一串 Big5 字节重新打开它。
 *
 * 测试文件带 FILE_FLAG_DELETE_ON_CLOSE，并包含当前 PID，所以正常情况下既不会覆盖用户文件，也不会留下垃圾。
 */
int LocaleLayer_RunFileNameSelfTest(void) {
    /*
     * “繁體中文轉區驗證”的 CP950/Big5 原始字节：
     *   繁=C1 63，體=C5 E9，中=A4 A4，文=A4 E5，轉=C2 E0，區=B0 CF，驗=C5 E7，證=C3 D2。
     * 前后目录、前缀、PID 和扩展名都只使用 ASCII，便于人工核对日志与源码。
     */
    static const BYTE big5_name_bytes[] = {
        0xC1u,0x63u,0xC5u,0xE9u,0xA4u,0xA4u,0xA4u,0xE5u,
        0xC2u,0xE0u,0xB0u,0xCFu,0xC5u,0xE7u,0xC3u,0xD2u
    };
    char ansi_path[256];
    WCHAR unicode_path[256];
    WCHAR converted[256];
    WCHAR final_path[YCR_PATH_CAP];
    UINT pos = 0u, i;
    int converted_chars;
    HANDLE unicode_handle = INVALID_HANDLE_VALUE_;
    HANDLE ansi_handle = INVALID_HANDLE_VALUE_;
    DWORD pid;
    DWORD final_chars = 0u;

    /* 同一个进程只需要做一次。第二次 LocaleLayer_Initialize 不应重复创建探针文件。 */
    if (g_file_name_self_test_done) return g_file_name_self_test_result;
    g_file_name_self_test_done = 1;
    g_file_name_self_test_result = 0;

    if (!g_real_MultiByteToWideChar || !g_real_CreateFileA) {
        ModLoader_Log((const WCHAR*)L"[Locale/文件自检] FAIL：无法取得真实 MultiByteToWideChar/CreateFileA，不能做实体文件名验收。");
        return 0;
    }

    /* 先写 ASCII 前缀。这里不用 strcpy，避免为了一个测试路径引入 CRT。 */
    {
        static const char prefix[] = "mods\\.__CastleLocaleProbe_";
        for (i = 0u; prefix[i]; ++i) {
            if (pos + 1u >= (UINT)sizeof(ansi_path)) return 0;
            ansi_path[pos++] = prefix[i];
        }
    }
    /* 再原样复制 Big5 字节；char 是否有符号不影响字节值，真正转换时 API 按原始内存读取。 */
    for (i = 0u; i < (UINT)sizeof(big5_name_bytes); ++i) {
        if (pos + 1u >= (UINT)sizeof(ansi_path)) return 0;
        ansi_path[pos++] = (char)big5_name_bytes[i];
    }
    ansi_path[pos++] = '_';
    ansi_path[pos] = 0;

    pid = GetCurrentProcessId();
    if (!append_u32_a_(ansi_path, (UINT)sizeof(ansi_path), pid)) return 0;
    /* 追加扩展名；路径只作为一次性测试文件使用。 */
    {
        static const char suffix[] = ".tmp";
        UINT d = 0u;
        while (ansi_path[d]) ++d;
        for (i = 0u; suffix[i]; ++i) {
            if (d + i + 1u >= (UINT)sizeof(ansi_path)) return 0;
            ansi_path[d + i] = suffix[i];
        }
        ansi_path[d + i] = 0;
    }

    /* 构造完全对应的 Unicode 预期路径。 */
    {
        static const WCHAR prefix_w[] = L"mods\\.__CastleLocaleProbe_繁體中文轉區驗證_";
        static const WCHAR suffix_w[] = L".tmp";
        UINT d = 0u;
        while (prefix_w[d]) {
            if (d + 1u >= 256u) return 0;
            unicode_path[d] = prefix_w[d];
            ++d;
        }
        unicode_path[d] = 0;
        if (!append_u32_w_(unicode_path, 256u, pid)) return 0;
        while (unicode_path[d]) ++d;
        for (i = 0u; suffix_w[i]; ++i) {
            if (d + i + 1u >= 256u) return 0;
            unicode_path[d + i] = suffix_w[i];
        }
        unicode_path[d + i] = 0;
    }

    /*
     * 第一层：真实 CP_ACP 转换。
     * 这里故意传 CP_ACP，而不是直接传 950；因为要验收的是“目标进程默认 ANSI 代码页真的已经变成 Big5”。
     */
    converted_chars = g_real_MultiByteToWideChar(CP_ACP_, 0u, ansi_path, -1, converted, 256);
    if (converted_chars <= 0 || !weq_exact_(converted, unicode_path)) {
        ModLoader_Log((const WCHAR*)L"[Locale/文件自检 1/2] FAIL：真实 MultiByteToWideChar(CP_ACP) 没有把预置 Big5 字节转换成预期繁体 Unicode 文件名。");
        if (converted_chars > 0) ModLoader_LogTwo((const WCHAR*)L"[Locale/文件自检] 实际转换结果：", converted);
        return 0;
    }
    ModLoader_Log((const WCHAR*)L"[Locale/文件自检 1/2] PASS：真实 CP_ACP 已把预置 Big5 原始字节精确转换为预期繁体 Unicode 文件名。");

    /*
     * 第二层：真实文件系统打开链。
     * 先用 W 版本创建“正确 Unicode 名”的实体文件；再用 A 版本和 Big5 原始字节打开。
     * 如果 A 路径被错误地按 CP936/UTF-8 等其它编码解释，它就找不到刚刚创建的这个 Unicode 文件。
     */
    unicode_handle = CreateFileW(unicode_path,
                                 GENERIC_READ_ | GENERIC_WRITE_,
                                 FILE_SHARE_READ_ | FILE_SHARE_WRITE_ | FILE_SHARE_DELETE_,
                                 NULL_PTR,
                                 CREATE_NEW_,
                                 FILE_ATTRIBUTE_TEMPORARY_ | FILE_FLAG_DELETE_ON_CLOSE_,
                                 NULL_PTR);
    if (unicode_handle == INVALID_HANDLE_VALUE_) {
        ModLoader_LogError((const WCHAR*)L"[Locale/文件自检 2/2] FAIL：CreateFileW 无法创建一次性 Unicode 探针文件", GetLastError());
        return 0;
    }

    ansi_handle = g_real_CreateFileA(ansi_path,
                                     GENERIC_READ_,
                                     FILE_SHARE_READ_ | FILE_SHARE_WRITE_ | FILE_SHARE_DELETE_,
                                     NULL_PTR,
                                     OPEN_EXISTING_,
                                     FILE_ATTRIBUTE_NORMAL_,
                                     NULL_PTR);
    if (ansi_handle == INVALID_HANDLE_VALUE_) {
        DWORD error = GetLastError();
        CloseHandle(unicode_handle); /* 首个句柄带 DELETE_ON_CLOSE；关闭后探针自动消失。 */
        ModLoader_LogError((const WCHAR*)L"[Locale/文件自检 2/2] FAIL：真实 CreateFileA 无法用 Big5 ANSI 文件名打开同一个 Unicode 实体文件", error);
        return 0;
    }

    ModLoader_Log((const WCHAR*)L"[Locale/文件自检 2/2] PASS：真实 CreateFileA 已用 Big5 ANSI 文件名打开由 CreateFileW 创建的同一个 Unicode 实体文件。");

    /* 最终路径只是增强证据；老系统若没有这个 API，不影响前面“句柄确实打开成功”的核心结论。 */
    if (g_real_GetFinalPathNameByHandleW) {
        final_chars = g_real_GetFinalPathNameByHandleW(ansi_handle, final_path, YCR_PATH_CAP, 0u);
        if (final_chars > 0u && final_chars < YCR_PATH_CAP)
            ModLoader_LogTwo((const WCHAR*)L"[Locale/文件自检] 实际打开的 NTFS Unicode 路径：", final_path);
    }

    CloseHandle(ansi_handle);
    CloseHandle(unicode_handle); /* 最后一个相关句柄关闭后 Windows 自动删除探针文件。 */

    g_file_name_self_test_result = 1;
    ModLoader_Log((const WCHAR*)L"[Locale/文件自检] PASS：CP950 Big5 字节 → CP_ACP → CreateFileA → NTFS Unicode 文件名整条链已通过实体文件验证。");
    return 1;
}

int LocaleLayer_Initialize(void) {
    HMODULE modules[MAX_LOCALE_MODULES_];
    UINT i, patched = 0u;
    WCHAR num[32];
    UINT n = 0u, v;

    if (!resolve_real_apis_()) {
        ModLoader_Log((const WCHAR*)L"[Locale错误] 无法解析必要的 KERNEL32 NLS API；Locale 层未启用。");
        return 0;
    }

    if (!apply_process_locale_state_()) {
        /*
         * IAT Hook 本身不能替代真正 NLS 表。这里 fail-closed，避免再次产生
         * “日志说 Locale 成功，但系统内部 CP_ACP 仍是宿主代码页”的假阳性。
         */
        return 0;
    }

    /*
     * dev9 恢复 dev5 的 ddraw.dll Locale 桥。
     *
     * 用户已经用同一套 cnc-ddraw 做了直接 A/B：dev5 区域环境正常，而 dev6 起在排除 ddraw 后
     * 窗口标题出现明显 ACP 乱码。cnc-ddraw 自己会参与窗口创建/消息处理，因此它内部的 ANSI API
     * 也必须看到和 RPG.exe 一致的 CP950 语义。
     *
     * 这里只改 ddraw.dll 的 Win32 Locale/NLS 导入，不碰任何 DirectDraw 导出或 COM 对象。
     */
    modules[0] = GetModuleHandleW(NULL_PTR);
    modules[1] = GetModuleHandleW((const WCHAR*)L"BASS.dll");
    modules[2] = GetModuleHandleW((const WCHAR*)L"binkw32.dll");
    modules[3] = GetModuleHandleW((const WCHAR*)L"ijl10.dll");
    modules[4] = GetModuleHandleW((const WCHAR*)L"ddraw.dll");
    modules[5] = GetModuleHandleW((const WCHAR*)L"SHLWAPI.dll");
    modules[6] = NULL_PTR;
    modules[7] = NULL_PTR;

    for (i = 0u; i < MAX_LOCALE_MODULES_; ++i) if (modules[i]) patched += LocaleLayer_PatchModule(modules[i]);

    v = patched;
    do { ++n; v /= 10u; } while (v);
    if (n < 32u) {
        UINT pos = n;
        num[pos] = 0;
        v = patched;
        do { num[--pos] = (WCHAR)('0' + (v % 10u)); v /= 10u; } while (v && pos);
        ModLoader_LogTwo((const WCHAR*)L"[Locale] 已改写相关 Locale/NLS/UI/时区/注册表 IAT 槽位数量=", num);
    }
    ModLoader_Log((const WCHAR*)L"[Locale] 固定环境：zh-TW / LCID 0x0404 / ANSI+OEM CP950 / CHINESEBIG5_CHARSET 语义。");
    ModLoader_Log((const WCHAR*)L"[Locale] 固定时区：Taipei Standard Time / UTC+8 / 无夏令时。");
    ModLoader_Log((const WCHAR*)L"[Locale] 高级：语言相关注册表伪造=启用；系统 UI 语言伪造=启用。");
    return 1;
}
