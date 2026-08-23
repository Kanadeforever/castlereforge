#include "platform.h"
#include "native_locale.h"
#include "mod_loader.h"

/*
 * 《幽城幻剑录》Native Locale 层。
 *
 * 为什么 v0.2.5 已经让“真实 GetACP/GetOEMCP = 950/950”，游戏仍可能看起来没有真正转区：
 *   Windows 的“区域环境”不只是一对代码页数字。Locale Emulator 的已验证实现还会让 ntdll 层的
 *   默认 Locale、默认 UI Language、安装 UI Language、时区查询和线程 TEB.CurrentLocale 都看到目标值。
 *
 * 从 v0.2.8 开始，本文件不再只补“能从日志看见的 Locale 数字”，而是把 Locale Emulator Core
 * 对《幽城幻剑录》这种 32 位 ANSI 游戏真正关键的 ntdll 运行语义逐层对齐：
 *   1. 中文（台湾）/ LCID 0x0404；
 *   2. 台北 UTC+8；
 *   3. 伪造系统 UI 语言；
 *   4. 与这些设置直接相关的 NLS 默认 Locale 查询；
 *   5. RtlCustomCPToUnicodeN 的“陈旧 CPTABLEINFO 修复”。
 *
 * 第 5 项是 v0.2.7 实机失败后确认的关键缺口。v0.2.7 已经能让真实 GetACP()=950，
 * 也能让真实 MultiByteToWideChar(CP_ACP) 把 Big5 字节正确变成 Unicode；但真实 CreateFileA
 * 仍然返回 ERROR_FILE_NOT_FOUND。原因是某些 Windows 版本的 KernelBase 在更早阶段就缓存了一份
 * “宿主系统代码页”的 CPTABLEINFO，文件 API 后面会继续拿这份旧表调用 ntdll!RtlCustomCPToUnicodeN。
 * 于是“公开 API 说 ACP=950”与“文件 API 内部真正使用的转换表”会发生分叉。
 *
 * Locale Emulator Core 的成熟做法不是枚举并重写每一个 CreateFileA/FindFirstFileA/CRT 文件函数，
 * 而是在 ntdll 的 RtlCustomCPToUnicodeN 这一层检查调用者传入的 CPTABLEINFO：只要它既不是 UTF-8，
 * 也不是目标 ANSI 代码页，就用目标 NLS 原始表重新初始化这份结构，再调用原函数。这样所有最终走到
 * 该原语的 ANSI -> Unicode 路径会自动得到同一套 CP950 语义，覆盖面比单独 Hook CreateFileA 更完整。
 *
 * “伪造语言相关注册表键值”不能只停留在 SHLWAPI!SHGetValueA。Locale Emulator 在启用 Registry
 * Redirection 时会继续接管 ntdll!NtQueryValueKey；这样即使 USER32/KERNEL32 或后续 DLL 绕过
 * SHLWAPI、直接通过原生注册表查询读取 NLS/语言键，也仍然看到同一套 zh-TW/CP950 数据。
 * 因此本文件只为用户明确启用的 8 个语言值提供固定重定向，不建立通用虚拟注册表。
 *
 * Hook 方式分成两类：
 *   A. NtQuery* / NtInitializeNlsFiles 等 x86 syscall stub：继续使用“严格验证后 5 字节 JMP”。只有首条
 *      完整指令恰好是 `mov eax, imm32`（B8 + 4 字节）才允许 Hook；未知布局立即 fail-closed。
 *   B. RtlCustomCPToUnicodeN：它不是 syscall stub，不能套用上面的 B8 规则。因此 v0.2.8 新增一个很小的
 *      x86 函数入口解码器，逐条计算完整指令长度，累计至少 5 字节后才建立 trampoline。解码器只接受
 *      我们明确理解、而且不需要重定位相对分支的普通 x86 指令；一旦首段出现 CALL/JMP/Jcc、16 位
 *      地址模式或无法识别的机器码，立即拒绝 Hook。这样宁可告诉用户“此 Windows 版本需要继续适配”，
 *      也绝不把未知指令劈开后继续运行。
 */

#define NATIVE_LOCALE_LCID_ 0x0404u
#define NATIVE_LOCALE_LANGID_ 0x0404u
#define SYSTEM_CURRENT_TIME_ZONE_INFORMATION_ 44u
#define STATUS_SUCCESS_ ((LONG)0)
#define STATUS_INFO_LENGTH_MISMATCH_ ((LONG)0xC0000004L)
#define MAX_NATIVE_HOOKS_ 6u
#define MAX_FUNCTION_PROLOGUE_ 16u
#define CUSTOM_CP_UTF8_ 65001u
#define STATUS_BUFFER_OVERFLOW_ ((LONG)0x80000005L)
#define STATUS_BUFFER_TOO_SMALL_ ((LONG)0xC0000023L)
#define STATUS_INVALID_PARAMETER_ ((LONG)0xC000000DL)
#define KEY_NAME_INFORMATION_CLASS_ 3u
#define KEY_VALUE_BASIC_INFORMATION_ 0u
#define KEY_VALUE_FULL_INFORMATION_ 1u
#define KEY_VALUE_PARTIAL_INFORMATION_ 2u
#define KEY_VALUE_FULL_INFORMATION_ALIGN64_ 3u
#define KEY_VALUE_PARTIAL_INFORMATION_ALIGN64_ 4u
#define REG_SZ_NATIVE_ 1u
#define REG_MULTI_SZ_NATIVE_ 7u

/* ntdll API 的最小函数签名。全部使用 NTAPI/__stdcall，保证 x86 调用栈由被调函数按系统 ABI 清理。 */
typedef LONG (WINAPI *PFN_NtQueryDefaultLocale_)(BOOL user_profile, DWORD* locale_id);
typedef LONG (WINAPI *PFN_NtQueryDefaultUILanguage_)(WORD* language_id);
typedef LONG (WINAPI *PFN_NtQueryInstallUILanguage_)(WORD* language_id);
typedef LONG (WINAPI *PFN_NtQuerySystemInformation_)(DWORD info_class, LPVOID info, DWORD info_length, DWORD* return_length);
typedef LONG (WINAPI *PFN_NtInitializeNlsFiles_)(LPVOID* base_address, DWORD* default_locale_id, LPVOID default_casing_table_size);
typedef LONG (WINAPI *PFN_NtQueryKey_)(HANDLE key, DWORD info_class, LPVOID info, DWORD length, DWORD* result_length);
typedef LONG (WINAPI *PFN_NtQueryValueKey_)(HANDLE key, LPVOID value_name, DWORD info_class, LPVOID info, DWORD length, DWORD* result_length);

/*
 * 这是 ntdll 内部使用的代码页描述结构的最小定义。
 * 可以把它理解成“Windows 把一份 .nls 文件解析以后得到的查表说明书”：
 *   CodePage              = 这份说明书现在代表哪个代码页，例如 932/936/950；
 *   MaximumCharacterSize  = 一个字符最多占多少字节；
 *   MultiByteTable 等指针 = 真正把 ANSI 字节映射到 Unicode 的查表数据。
 *
 * v0.2.7 的问题就在这里：KernelBase 可能很早就保存了一份 CodePage=宿主ACP 的说明书，
 * 后面即使 PEB/GetACP 已经变成 950，它还是会把旧说明书传给 RtlCustomCPToUnicodeN。
 */
typedef struct CPTABLEINFO_NATIVE_ {
    WORD CodePage;
    WORD MaximumCharacterSize;
    WORD DefaultChar;
    WORD UniDefaultChar;
    WORD TransDefaultChar;
    WORD TransUniDefaultChar;
    WORD DBCSCodePage;
    BYTE LeadByte[12];
    WORD* MultiByteTable;
    LPVOID WideCharTable;
    WORD* DBCSRanges;
    WORD* DBCSOffsets;
} CPTABLEINFO_NATIVE_;

/*
 * RtlCustomCPToUnicodeN 是 ntdll 的底层“按指定代码页表把字节转成 UTF-16”函数。
 * RtlInitCodePageTable 则把一份原始 NLS 文件映射解析成上面的 CPTABLEINFO_NATIVE_。
 * 两个地址都在运行时从 ntdll 导出表取得，所以 CastleModCore.dll 仍然只静态依赖 KERNEL32。
 */
typedef LONG (WINAPI *PFN_RtlCustomCPToUnicodeN_)(CPTABLEINFO_NATIVE_* custom_cp, WCHAR* unicode_string,
                                                   DWORD unicode_size, DWORD* result_size,
                                                   char* custom_string, DWORD custom_size);
typedef void (WINAPI *PFN_RtlInitCodePageTable_)(WORD* table_base, CPTABLEINFO_NATIVE_* table_info);

typedef struct UNICODE_STRING_MIN_ {
    WORD Length;
    WORD MaximumLength;
    WCHAR* Buffer;
} UNICODE_STRING_MIN_;

typedef struct KEY_NAME_INFORMATION_MIN_ {
    DWORD NameLength;
    WCHAR Name[1];
} KEY_NAME_INFORMATION_MIN_;

/*
 * RTL_TIME_ZONE_INFORMATION 与 Win32 TIME_ZONE_INFORMATION 在这里关心的字段布局相同：
 * Bias + 两个 32 WCHAR 名称 + 两个 16 字节时间结构 + Standard/Daylight Bias。
 * platform.h 的 TIME_ZONE_INFORMATION_ 已经是这个精确尺寸，因此直接复用，避免定义两份容易不同步的结构。
 */

typedef struct NATIVE_HOOK_ {
    const char* name;
    BYTE* target;
    BYTE original[5];
    BYTE* trampoline;
    LPVOID replacement;
    int installed;
} NATIVE_HOOK_;

/*
 * 普通函数入口可能需要复制 5、6、7……字节才能保证“不截断一条机器指令”。
 * 所以它不能复用上面的固定 5 字节 syscall 结构，单独保存实际覆盖长度。
 */
typedef struct FUNCTION_HOOK_ {
    const char* name;
    BYTE* target;
    BYTE original[MAX_FUNCTION_PROLOGUE_];
    UINT original_length;
    BYTE* trampoline;
    LPVOID replacement;
    int installed;
} FUNCTION_HOOK_;

static HMODULE g_ntdll;
static PFN_NtQueryDefaultLocale_ g_real_NtQueryDefaultLocale;
static PFN_NtQueryDefaultUILanguage_ g_real_NtQueryDefaultUILanguage;
static PFN_NtQueryInstallUILanguage_ g_real_NtQueryInstallUILanguage;
static PFN_NtQuerySystemInformation_ g_real_NtQuerySystemInformation;
static PFN_NtInitializeNlsFiles_ g_real_NtInitializeNlsFiles;
static PFN_NtQueryKey_ g_real_NtQueryKey;
static PFN_NtQueryValueKey_ g_real_NtQueryValueKey;
static PFN_RtlCustomCPToUnicodeN_ g_real_RtlCustomCPToUnicodeN;
static PFN_RtlInitCodePageTable_ g_RtlInitCodePageTable;
static NATIVE_HOOK_ g_hooks[MAX_NATIVE_HOOKS_];
static FUNCTION_HOOK_ g_custom_cp_hook;
static UINT g_hook_count;
/*
 * 这个计数器只用于诊断：每当发现调用者拿着“不是 950 的旧代码页表”并被我们修成 950，就加一。
 * 它不决定游戏逻辑；真正的硬门槛仍然是后面的实体 CreateFileA Big5 自检。
 */
static volatile LONG g_custom_cp_repair_count;
/*
 * 修改 CPTABLEINFO 是共享状态操作。虽然通常只会在第一次命中旧缓存时发生，但仍必须防止两个线程同时写同一结构。
 * 这里用一个最小自旋锁，避免额外引入 CRT 或复杂同步对象。
 */
static volatile LONG g_custom_cp_lock;
static int g_initialized;

/* ---------- 极小字符串/数字日志辅助 ---------- */

static void append_ascii_to_wide_(WCHAR* dst, UINT cap, const char* src) {
    UINT pos = 0u, i = 0u;
    if (!dst || cap == 0u) return;
    while (dst[pos] && pos + 1u < cap) ++pos;
    while (src && src[i] && pos + 1u < cap) dst[pos++] = (WCHAR)(BYTE)src[i++];
    dst[pos] = 0;
}

static void log_hook_error_(const WCHAR* prefix, const char* function_name) {
    WCHAR line[256];
    UINT i = 0u;
    if (!prefix) return;
    while (prefix[i] && i + 1u < 256u) { line[i] = prefix[i]; ++i; }
    line[i] = 0;
    append_ascii_to_wide_(line, 256u, function_name);
    ModLoader_Log(line);
}

/* ---------- Locale Emulator 等价的 CPTABLEINFO 修复辅助 ---------- */

/*
 * 取当前进程的 PEB。
 * x86 Windows 约定 FS:[0x30] 指向当前进程的 PEB；Bootstrap 已把 PEB+0x58 的 ANSI NLS 指针
 * 改成我们加载的 c_950.nls 原始映射，所以这里可以直接取得“真正目标 CP950 表”的起点。
 */
static BYTE* current_peb_(void) {
#if defined(_M_IX86) || defined(__i386__)
    BYTE* peb = NULL_PTR;
    __asm {
        mov eax, fs:[30h]
        mov peb, eax
    }
    return peb;
#else
    return NULL_PTR;
#endif
}

/*
 * 读出 PEB.AnsiCodePageData。
 * 对 32 位 NT PEB 来说该字段位于 +0x58；locale_bootstrap.c 也在同一个明确偏移写入 c_950.nls。
 * 两边使用同一地址并在文档中固定，是为了避免一个模块写 A、另一个模块却从 B 读这种隐蔽错误。
 */
static WORD* current_ansi_nls_table_(void) {
    BYTE* peb = current_peb_();
    if (!peb) return NULL_PTR;
    return *(WORD**)(peb + 0x58u);
}

/*
 * 一个非常小的 x86 自旋锁。
 * “lock cmpxchg”保证多个 CPU/线程同时执行时，只有一个线程能把 g_custom_cp_lock 从 0 改成 1。
 * 如果没抢到，就继续循环；这里只保护几条 RtlInitCodePageTable 指令，持锁时间极短。
 */
static void custom_cp_lock_(void) {
#if defined(_M_IX86) || defined(__i386__)
    for (;;) {
        LONG acquired = 0;
        __asm {
            mov eax, 0
            mov edx, 1
            mov ecx, offset g_custom_cp_lock
            lock cmpxchg dword ptr [ecx], edx
            jne lock_failed_
            mov acquired, 1
        lock_failed_:
        }
        if (acquired) return;
    }
#endif
}

/*
 * 释放自旋锁只需要把 1 写回 0。
 * x86 对对齐 DWORD 的普通写本身是原子的；前面的 locked 指令同时提供了我们需要的内存排序边界。
 */
static void custom_cp_unlock_(void) {
    g_custom_cp_lock = 0;
}

/*
 * 下面是一个“只为函数入口 trampoline 服务”的最小 x86 指令长度解码器。
 * 它不是反汇编器，也不会尝试理解整段程序；目标只有一个：从函数开头一条一条走，确认前至少 5 字节
 * 都是可以原样复制到 trampoline 的普通指令。
 *
 * 为什么不能直接复制前 5 个字节：
 *   假设机器码是 `55 8B EC 83 EC 10`，前三字节是 push/mov，后面 `83 EC 10` 是一整条 3 字节指令。
 *   如果只拿 5 字节，就会把这条指令切成 `83 EC` 和孤零零的 `10`，CPU 执行 trampoline 时必然跑飞。
 *
 * 返回值：
 *   >0  = 当前这一条完整指令的字节长度；
 *    0  = 当前机器码不在我们明确支持的安全集合内，调用者必须 fail-closed。
 *
 * 另外，CALL/JMP/Jcc 的位移是“相对于原地址”的。原样复制到 trampoline 后目标会改变，因此首段遇到任何
 * 相对控制流指令都直接返回 0，而不是偷偷复制一个已经失效的跳转。
 */
static UINT x86_modrm_length_(const BYTE* code, UINT cap, UINT offset) {
    BYTE modrm, mod, rm;
    UINT length = 1u;
    if (!code || offset >= cap) return 0u;
    modrm = code[offset];
    mod = (BYTE)((modrm >> 6) & 3u);
    rm = (BYTE)(modrm & 7u);

    /* mod=3 表示两个操作数都是寄存器，没有 SIB/位移字节。 */
    if (mod == 3u) return length;

    /* 32 位地址模式下，rm=4 表示后面还有一个 SIB 字节。 */
    if (rm == 4u) {
        BYTE sib, base;
        if (offset + length >= cap) return 0u;
        sib = code[offset + length];
        ++length;
        base = (BYTE)(sib & 7u);
        /* mod=0 且 SIB.base=5 表示没有基址寄存器，后面直接跟 disp32。 */
        if (mod == 0u && base == 5u) length += 4u;
    }

    if (mod == 0u) {
        /* rm=5 且没有 SIB 时同样是 disp32 绝对地址。 */
        if (rm == 5u) length += 4u;
    } else if (mod == 1u) {
        length += 1u; /* disp8 */
    } else if (mod == 2u) {
        length += 4u; /* disp32 */
    }

    if (offset + length > cap) return 0u;
    return length;
}

static UINT x86_instruction_length_safe_(const BYTE* code, UINT cap) {
    UINT p = 0u;
    BYTE op;
    int operand16 = 0;

    if (!code || cap == 0u) return 0u;

    /*
     * 先跳过我们理解的普通前缀。0x67 会切换到 16 位地址编码，ModR/M 规则完全不同；
     * 函数普通 prologue 几乎不会需要它，所以遇到 0x67 直接拒绝，比写一个半吊子的 16 位解码器更安全。
     */
    while (p < cap) {
        BYTE b = code[p];
        if (b == 0x66u) { operand16 = 1; ++p; continue; }
        if (b == 0xF0u || b == 0xF2u || b == 0xF3u ||
            b == 0x2Eu || b == 0x36u || b == 0x3Eu || b == 0x26u ||
            b == 0x64u || b == 0x65u) { ++p; continue; }
        if (b == 0x67u) return 0u;
        break;
    }
    if (p >= cap) return 0u;
    op = code[p++];

    /* 一字节、无 ModR/M、也没有相对地址的常见指令。 */
    if ((op >= 0x40u && op <= 0x5Fu) || /* inc/dec/push/pop r32 */
        op == 0x90u || op == 0x9Cu || op == 0x9Du || op == 0xC3u || op == 0xCCu) {
        return p;
    }

    /* mov r8, imm8 */
    if (op >= 0xB0u && op <= 0xB7u) return (p + 1u <= cap) ? p + 1u : 0u;
    /* mov r16/r32, imm */
    if (op >= 0xB8u && op <= 0xBFu) {
        UINT imm = operand16 ? 2u : 4u;
        return (p + imm <= cap) ? p + imm : 0u;
    }

    /* push imm / ret imm16 / moffs 绝对地址。它们复制到 trampoline 后含义不变。 */
    if (op == 0x6Au) return (p + 1u <= cap) ? p + 1u : 0u;
    if (op == 0x68u) { UINT imm = operand16 ? 2u : 4u; return (p + imm <= cap) ? p + imm : 0u; }
    if (op == 0xC2u) return (p + 2u <= cap) ? p + 2u : 0u;
    if (op >= 0xA0u && op <= 0xA3u) {
        /* 本解码器拒绝 0x67，所以这里地址操作数固定是 32 位绝对地址。 */
        return (p + 4u <= cap) ? p + 4u : 0u;
    }

    /*
     * 所有相对控制流都拒绝。即使长度很好算，也不能原样搬到 trampoline。
     */
    if ((op >= 0x70u && op <= 0x7Fu) || op == 0xE8u || op == 0xE9u || op == 0xEBu ||
        op == 0xE0u || op == 0xE1u || op == 0xE2u || op == 0xE3u) return 0u;

    /* 0F 两字节操作码。只接受常见的普通 ModR/M 指令，0F 80..8F 的近条件跳转明确拒绝。 */
    if (op == 0x0Fu) {
        BYTE op2;
        UINT ml;
        if (p >= cap) return 0u;
        op2 = code[p++];
        if (op2 >= 0x80u && op2 <= 0x8Fu) return 0u;
        if (op2 == 0x1Fu || op2 == 0xAFu || op2 == 0xB6u || op2 == 0xB7u ||
            op2 == 0xBEu || op2 == 0xBFu || (op2 >= 0x40u && op2 <= 0x4Fu) ||
            (op2 >= 0x90u && op2 <= 0x9Fu) || op2 == 0xA3u || op2 == 0xABu ||
            op2 == 0xB3u || op2 == 0xBBu) {
            ml = x86_modrm_length_(code, cap, p);
            return ml ? p + ml : 0u;
        }
        if (op2 == 0xBAu) {
            ml = x86_modrm_length_(code, cap, p);
            if (!ml || p + ml + 1u > cap) return 0u;
            return p + ml + 1u;
        }
        return 0u;
    }

    /*
     * 普通“opcode + ModR/M”家族。先算完寻址字节，再根据 opcode 追加立即数。
     * 这已经覆盖 MSVC/Windows 系统 DLL 函数开头最常见的 mov/sub/test/lea/push-memory 等指令。
     */
    {
        int has_modrm = 0;
        UINT imm = 0u, ml;
        if ((op <= 0x03u) || (op >= 0x08u && op <= 0x0Bu) ||
            (op >= 0x10u && op <= 0x13u) || (op >= 0x18u && op <= 0x1Bu) ||
            (op >= 0x20u && op <= 0x23u) || (op >= 0x28u && op <= 0x2Bu) ||
            (op >= 0x30u && op <= 0x33u) || (op >= 0x38u && op <= 0x3Bu) ||
            op == 0x62u || op == 0x63u || op == 0x69u || op == 0x6Bu ||
            (op >= 0x80u && op <= 0x8Fu) || op == 0xC0u || op == 0xC1u ||
            op == 0xC6u || op == 0xC7u || (op >= 0xD0u && op <= 0xD3u) ||
            op == 0xF6u || op == 0xF7u || op == 0xFEu || op == 0xFFu) {
            has_modrm = 1;
        }
        if (!has_modrm) return 0u;

        ml = x86_modrm_length_(code, cap, p);
        if (!ml) return 0u;

        if (op == 0x69u) imm = operand16 ? 2u : 4u;
        else if (op == 0x6Bu || op == 0x80u || op == 0x82u || op == 0x83u || op == 0xC0u || op == 0xC1u || op == 0xC6u) imm = 1u;
        else if (op == 0x81u || op == 0xC7u) imm = operand16 ? 2u : 4u;
        else if (op == 0xF6u || op == 0xF7u) {
            /* F6/F7 只有 /0 TEST 变体带立即数；reg 字段在 ModR/M 的 bit3..5。 */
            BYTE reg = (BYTE)((code[p] >> 3) & 7u);
            if (reg == 0u) imm = (op == 0xF6u) ? 1u : (operand16 ? 2u : 4u);
        }

        if (p + ml + imm > cap) return 0u;
        return p + ml + imm;
    }
}

/*
 * 从 target 开头累计完整指令，直到至少得到 5 字节可覆盖空间。
 * copied_length 最多 16 字节：超过就说明遇到了我们没预料到的异常长函数入口，直接拒绝。
 */
static UINT measure_safe_prologue_(const BYTE* target) {
    UINT total = 0u;
    while (total < 5u) {
        UINT len;
        if (total >= MAX_FUNCTION_PROLOGUE_) return 0u;
        len = x86_instruction_length_safe_(target + total, MAX_FUNCTION_PROLOGUE_ - total);
        if (len == 0u || total + len > MAX_FUNCTION_PROLOGUE_) return 0u;
        total += len;
    }
    return total;
}

/*
 * 为普通 x86 函数建立 trampoline：
 *   原函数入口：E9 -> replacement，剩余覆盖字节用 NOP 填满；
 *   trampoline：复制完整原指令 -> E9 回原函数 original_length 之后。
 *
 * 只有 measure_safe_prologue_ 明确确认过的入口才允许进入这里。
 */
static int install_function_hook_(FUNCTION_HOOK_* hook) {
    BYTE* tramp;
    BYTE patch[MAX_FUNCTION_PROLOGUE_];
    DWORD old_protect = 0u, ignored = 0u;
    LONG rel;
    UINT i, length;

    if (!hook || !hook->target || !hook->replacement) return 0;
    if (hook->installed) return 1;

    length = measure_safe_prologue_(hook->target);
    if (length < 5u || length > MAX_FUNCTION_PROLOGUE_) {
        log_hook_error_((const WCHAR*)L"[Locale/Native错误] 普通 ntdll 函数入口无法安全解码，拒绝 Hook：", hook->name);
        return 0;
    }

    tramp = (BYTE*)VirtualAlloc(NULL_PTR, (SIZE_T)(length + 5u), MEM_COMMIT_ | MEM_RESERVE_, PAGE_EXECUTE_READWRITE_);
    if (!tramp) {
        log_hook_error_((const WCHAR*)L"[Locale/Native错误] 无法申请普通函数 trampoline：", hook->name);
        return 0;
    }

    for (i = 0u; i < length; ++i) {
        hook->original[i] = hook->target[i];
        tramp[i] = hook->target[i];
    }
    hook->original_length = length;

    /* trampoline 最后跳回原函数“被我们覆盖的完整指令”之后。 */
    tramp[length] = 0xE9u;
    rel = (LONG)((hook->target + length) - (tramp + length + 5u));
    *(LONG*)(tramp + length + 1u) = rel;
    hook->trampoline = tramp;

    /* 原入口第 1..5 字节变成跳到 replacement；多出来的完整指令空间全部 NOP。 */
    patch[0] = 0xE9u;
    rel = (LONG)((BYTE*)hook->replacement - (hook->target + 5u));
    *(LONG*)(patch + 1u) = rel;
    for (i = 5u; i < length; ++i) patch[i] = 0x90u;

    if (!VirtualProtect(hook->target, length, PAGE_EXECUTE_READWRITE_, &old_protect)) {
        log_hook_error_((const WCHAR*)L"[Locale/Native错误] 无法修改普通 ntdll 函数内存保护：", hook->name);
        return 0;
    }
    for (i = 0u; i < length; ++i) hook->target[i] = patch[i];
    FlushInstructionCache(GetCurrentProcess(), hook->target, length);
    VirtualProtect(hook->target, length, old_protect, &ignored);

    hook->installed = 1;
    return 1;
}

/*
 * Locale Emulator Core 对“旧 CPTABLEINFO 缓存”的关键修复行为。
 *
 * 例子：
 *   宿主系统是日文 -> KernelBase 很早缓存 CodePage=932；
 *   Bootstrap 后进程默认 ACP 已经是 950；
 *   CreateFileA("Big5字节") -> KernelBase 仍把那份 932 表传进本函数。
 *
 * 我们看到 932 != 950，就把 PEB 当前的 c_950.nls 原始表重新解析进“同一份 CustomCP 结构”。
 * 随后仍调用原版 RtlCustomCPToUnicodeN，真正的字符转换算法完全交给 Windows 自己完成。
 */
static LONG WINAPI Hook_RtlCustomCPToUnicodeN_(CPTABLEINFO_NATIVE_* custom_cp,
                                                WCHAR* unicode_string,
                                                DWORD unicode_size,
                                                DWORD* result_size,
                                                char* custom_string,
                                                DWORD custom_size) {
    if (custom_cp && g_RtlInitCodePageTable &&
        custom_cp->CodePage != (WORD)CUSTOM_CP_UTF8_ &&
        custom_cp->CodePage != (WORD)CASTLE_CP_BIG5_) {
        WORD* ansi_table;

        custom_cp_lock_();
        /* 抢锁后必须再检查一次，因为另一个线程可能刚刚已经把这份共享表修成 950。 */
        if (custom_cp->CodePage != (WORD)CUSTOM_CP_UTF8_ &&
            custom_cp->CodePage != (WORD)CASTLE_CP_BIG5_) {
            ansi_table = current_ansi_nls_table_();
            if (ansi_table) {
                g_RtlInitCodePageTable(ansi_table, custom_cp);
                ++g_custom_cp_repair_count;
            }
        }
        custom_cp_unlock_();
    }

    return g_real_RtlCustomCPToUnicodeN
        ? g_real_RtlCustomCPToUnicodeN(custom_cp, unicode_string, unicode_size, result_size, custom_string, custom_size)
        : STATUS_INVALID_PARAMETER_;
}

/* ---------- 当前线程 TEB Locale ---------- */

static DWORD* current_teb_locale_ptr_(void) {
#if defined(_M_IX86) || defined(__i386__)
    BYTE* teb = NULL_PTR;
    /*
     * x86 的 FS:[0x18] 指向当前 TEB 自身。Locale Emulator 在主线程继续前直接写 CurrentLocale；
     * 这里做相同目标，但只针对本工程固定的 32 位 RPG.exe。
     * TEB.CurrentLocale 在 x86 NT TEB 的偏移为 0xC4。
     */
    __asm {
        mov eax, fs:[18h]
        mov teb, eax
    }
    if (!teb) return NULL_PTR;
    return (DWORD*)(teb + 0xC4u);
#else
    return NULL_PTR;
#endif
}

static int force_current_thread_locale_(void) {
    DWORD* locale = current_teb_locale_ptr_();
    if (!locale) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] 无法取得 x86 TEB.CurrentLocale；本版只支持 32 位 RPG.exe。");
        return 0;
    }
    *locale = NATIVE_LOCALE_LCID_;
    if (*locale != NATIVE_LOCALE_LCID_) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] TEB.CurrentLocale 写回验证失败。");
        return 0;
    }
    return 1;
}

/* ---------- 台北时区 ---------- */

static void zero_bytes_(BYTE* p, UINT count) {
    UINT i;
    for (i = 0u; i < count; ++i) p[i] = 0u;
}

static void copy_wide_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT i = 0u;
    if (!dst || cap == 0u) return;
    while (src && src[i] && i + 1u < cap) { dst[i] = src[i]; ++i; }
    dst[i] = 0;
}

static void fill_taipei_timezone_(TIME_ZONE_INFORMATION_* tzi) {
    if (!tzi) return;
    zero_bytes_((BYTE*)tzi, (UINT)sizeof(*tzi));
    /* Win32/RTL 的 Bias 定义是“本地时间 + Bias = UTC”，所以 UTC+8 对应 -480。 */
    tzi->Bias = -480;
    copy_wide_(tzi->StandardName, 32u, (const WCHAR*)L"Taipei Standard Time");
    tzi->StandardBias = 0;
    tzi->DaylightBias = 0;
}

/* ---------- ntdll Hook 本体 ---------- */

static LONG WINAPI Hook_NtQueryDefaultLocale_(BOOL user_profile, DWORD* locale_id) {
    (void)user_profile;
    if (!locale_id) return g_real_NtQueryDefaultLocale ? g_real_NtQueryDefaultLocale(user_profile, locale_id) : STATUS_SUCCESS_;
    *locale_id = NATIVE_LOCALE_LCID_;
    return STATUS_SUCCESS_;
}

static LONG WINAPI Hook_NtQueryDefaultUILanguage_(WORD* language_id) {
    if (!language_id) return g_real_NtQueryDefaultUILanguage ? g_real_NtQueryDefaultUILanguage(language_id) : STATUS_SUCCESS_;
    *language_id = (WORD)NATIVE_LOCALE_LANGID_;
    return STATUS_SUCCESS_;
}

static LONG WINAPI Hook_NtQueryInstallUILanguage_(WORD* language_id) {
    if (!language_id) return g_real_NtQueryInstallUILanguage ? g_real_NtQueryInstallUILanguage(language_id) : STATUS_SUCCESS_;
    *language_id = (WORD)NATIVE_LOCALE_LANGID_;
    return STATUS_SUCCESS_;
}

static LONG WINAPI Hook_NtQuerySystemInformation_(DWORD info_class, LPVOID info, DWORD info_length, DWORD* return_length) {
    if (info_class == SYSTEM_CURRENT_TIME_ZONE_INFORMATION_) {
        if (!info || info_length < (DWORD)sizeof(TIME_ZONE_INFORMATION_)) {
            if (return_length) *return_length = (DWORD)sizeof(TIME_ZONE_INFORMATION_);
            return STATUS_INFO_LENGTH_MISMATCH_;
        }
        fill_taipei_timezone_((TIME_ZONE_INFORMATION_*)info);
        if (return_length) *return_length = (DWORD)sizeof(TIME_ZONE_INFORMATION_);
        return STATUS_SUCCESS_;
    }
    return g_real_NtQuerySystemInformation ? g_real_NtQuerySystemInformation(info_class, info, info_length, return_length) : STATUS_SUCCESS_;
}

static LONG WINAPI Hook_NtInitializeNlsFiles_(LPVOID* base_address, DWORD* default_locale_id, LPVOID default_casing_table_size) {
    LONG status;
    if (!g_real_NtInitializeNlsFiles) return STATUS_SUCCESS_;
    status = g_real_NtInitializeNlsFiles(base_address, default_locale_id, default_casing_table_size);
    if (status >= 0 && default_locale_id) *default_locale_id = NATIVE_LOCALE_LCID_;
    return status;
}


/* ---------- “伪造语言相关注册表键值”的 ntdll 低层补充 ---------- */

static WCHAR wfold_(WCHAR c) {
    if (c >= (WCHAR)'A' && c <= (WCHAR)'Z') return (WCHAR)(c + ((WCHAR)'a' - (WCHAR)'A'));
    return c;
}

static int unicode_eq_ascii_i_(const UNICODE_STRING_MIN_* value, const char* ascii) {
    UINT chars, i;
    if (!value || !value->Buffer || !ascii) return 0;
    chars = (UINT)value->Length / 2u;
    for (i = 0u; i < chars && ascii[i]; ++i) {
        WCHAR wc = wfold_(value->Buffer[i]);
        char ac = ascii[i];
        if (ac >= 'A' && ac <= 'Z') ac = (char)(ac + ('a' - 'A'));
        if (wc != (WCHAR)(BYTE)ac) return 0;
    }
    return i == chars && ascii[i] == 0;
}

static int wide_ends_with_ascii_i_(const WCHAR* path, UINT chars, const char* suffix) {
    UINT suffix_len = 0u, i, start;
    while (suffix && suffix[suffix_len]) ++suffix_len;
    if (!path || chars < suffix_len) return 0;
    start = chars - suffix_len;
    for (i = 0u; i < suffix_len; ++i) {
        WCHAR wc = wfold_(path[start + i]);
        char ac = suffix[i];
        if (ac >= 'A' && ac <= 'Z') ac = (char)(ac + ('a' - 'A'));
        if (wc != (WCHAR)(BYTE)ac) return 0;
    }
    return 1;
}

/*
 * 返回需要伪造的注册表数据。data_chars 包含结尾 NUL；MULTI_SZ 时再包含第二个 NUL。
 * 这里的 8 个值与 Locale Emulator 的 RegistryEntriesLoader 在 Advanced 模式下完全同类，
 * 但实现是本工程独立编写的固定 zh-TW/CP950 版本。
 */
static int lookup_registry_fake_(HANDLE key, const UNICODE_STRING_MIN_* value_name, WCHAR* data, UINT cap_chars, DWORD* type, UINT* data_chars) {
    BYTE key_buffer[2048];
    KEY_NAME_INFORMATION_MIN_* info = (KEY_NAME_INFORMATION_MIN_*)key_buffer;
    DWORD returned = 0u;
    LONG status;
    UINT chars;
    const WCHAR* text = NULL_PTR;
    DWORD out_type = REG_SZ_NATIVE_;
    int multi = 0;

    if (!g_real_NtQueryKey || !value_name || !data || !type || !data_chars) return 0;
    status = g_real_NtQueryKey(key, KEY_NAME_INFORMATION_CLASS_, info, (DWORD)sizeof(key_buffer), &returned);
    if (status < 0 || info->NameLength > (DWORD)(sizeof(key_buffer) - sizeof(DWORD))) return 0;
    chars = info->NameLength / 2u;

    if (wide_ends_with_ascii_i_(info->Name, chars, "\\System\\CurrentControlSet\\Control\\Nls\\CodePage")) {
        if (unicode_eq_ascii_i_(value_name, "InstallLanguage") || unicode_eq_ascii_i_(value_name, "Default")) text = (const WCHAR*)L"1028";
        else if (unicode_eq_ascii_i_(value_name, "OEMCP") || unicode_eq_ascii_i_(value_name, "ACP")) text = (const WCHAR*)L"950";
    } else if (wide_ends_with_ascii_i_(info->Name, chars, "\\Control Panel\\International")) {
        if (unicode_eq_ascii_i_(value_name, "Locale")) text = (const WCHAR*)L"00000404";
        else if (unicode_eq_ascii_i_(value_name, "LocaleName")) text = (const WCHAR*)L"zh-TW";
    } else if (wide_ends_with_ascii_i_(info->Name, chars, "\\Control Panel\\Desktop")) {
        if (unicode_eq_ascii_i_(value_name, "PreferredUILanguages")) { text = (const WCHAR*)L"zh-TW"; out_type = REG_MULTI_SZ_NATIVE_; multi = 1; }
    } else if (wide_ends_with_ascii_i_(info->Name, chars, "\\Control Panel\\Desktop\\MuiCached")) {
        if (unicode_eq_ascii_i_(value_name, "MachinePreferredUILanguages")) { text = (const WCHAR*)L"zh-TW"; out_type = REG_MULTI_SZ_NATIVE_; multi = 1; }
    }
    if (!text) return 0;

    chars = 0u;
    while (text[chars]) ++chars;
    if (chars + 1u + (multi ? 1u : 0u) > cap_chars) return 0;
    {
        UINT i;
        for (i = 0u; i < chars; ++i) data[i] = text[i];
        data[chars++] = 0;
        if (multi) data[chars++] = 0;
    }
    *type = out_type;
    *data_chars = chars;
    return 1;
}

static DWORD round_up_dword_(DWORD value, DWORD alignment) {
    /*
     * KEY_VALUE_* 结构里的 DataOffset 必须满足系统要求的对齐。
     * alignment 在这里只会是 4 或 8，所以“先加 alignment-1，再清低位”既简单又不会涉及除法。
     */
    return (value + alignment - 1u) & ~(alignment - 1u);
}

static LONG fill_registry_result_(const UNICODE_STRING_MIN_* value_name,
                                  DWORD info_class,
                                  LPVOID out,
                                  DWORD length,
                                  DWORD* result_length,
                                  DWORD type,
                                  const WCHAR* data,
                                  UINT data_chars) {
    DWORD data_bytes = data_chars * 2u;
    DWORD required = 0u;
    BYTE* bytes = (BYTE*)out;
    UINT i;

    /*
     * Locale Emulator 的重定向核心真正需要返回数据的是 Full / Partial 两大类。
     * BasicInformation 本身不包含 Value Data；对于这种查询继续交给真实注册表即可，避免伪造无意义元数据。
     * 这里若收到未知 information class，则与 NT API 约定一致返回 STATUS_INVALID_PARAMETER。
     */
    if (info_class == KEY_VALUE_PARTIAL_INFORMATION_ ||
        info_class == KEY_VALUE_PARTIAL_INFORMATION_ALIGN64_) {
        DWORD header_size;
        DWORD alignment;

        if (info_class == KEY_VALUE_PARTIAL_INFORMATION_ALIGN64_) {
            /* KEY_VALUE_PARTIAL_INFORMATION_ALIGN64：Type + DataLength + Data。 */
            header_size = 8u;
            alignment = 8u;
        } else {
            /* KEY_VALUE_PARTIAL_INFORMATION：TitleIndex + Type + DataLength + Data。 */
            header_size = 12u;
            alignment = 4u;
        }

        required = round_up_dword_(header_size + data_bytes, alignment);
        if (result_length) *result_length = required;

        /*
         * 和 NtQueryValueKey 一样：连固定头部都放不下时不写数据，直接告诉调用者最小所需长度。
         */
        if (!out || length < header_size) return STATUS_BUFFER_TOO_SMALL_;

        if (info_class == KEY_VALUE_PARTIAL_INFORMATION_ALIGN64_) {
            *(DWORD*)(bytes + 0u) = type;
            *(DWORD*)(bytes + 4u) = data_bytes;
            if (length >= required) {
                for (i = 0u; i < data_bytes; ++i) bytes[8u + i] = ((const BYTE*)data)[i];
                /* 对齐尾巴不是有效数据，清零只是让诊断更稳定，不改变 DataLength。 */
                for (i = 8u + data_bytes; i < required; ++i) bytes[i] = 0u;
            }
        } else {
            *(DWORD*)(bytes + 0u) = 0u;          /* TitleIndex：Windows 保留字段，固定 0。 */
            *(DWORD*)(bytes + 4u) = type;
            *(DWORD*)(bytes + 8u) = data_bytes;
            if (length >= required) {
                for (i = 0u; i < data_bytes; ++i) bytes[12u + i] = ((const BYTE*)data)[i];
                for (i = 12u + data_bytes; i < required; ++i) bytes[i] = 0u;
            }
        }
        return length < required ? STATUS_BUFFER_OVERFLOW_ : STATUS_SUCCESS_;
    }

    if (info_class == KEY_VALUE_FULL_INFORMATION_ ||
        info_class == KEY_VALUE_FULL_INFORMATION_ALIGN64_) {
        DWORD name_bytes = value_name ? (DWORD)value_name->Length : 0u;
        DWORD header_size = 20u; /* TitleIndex/Type/DataOffset/DataLength/NameLength。 */
        DWORD data_offset;

        /*
         * LE 的实现对 FullInformation 与 FullInformationAlign64 都把数据区按 8 字节对齐。
         * 这里保持同一行为，避免某些原生调用者按 64 位边界访问 Data 时得到未对齐地址。
         */
        data_offset = round_up_dword_(header_size + name_bytes, 8u);
        required = round_up_dword_(data_offset + data_bytes, 4u);
        if (result_length) *result_length = required;

        if (!out || length < header_size) return STATUS_BUFFER_TOO_SMALL_;

        *(DWORD*)(bytes + 0u) = 0u;          /* TitleIndex */
        *(DWORD*)(bytes + 4u) = type;
        *(DWORD*)(bytes + 8u) = data_offset;
        *(DWORD*)(bytes + 12u) = data_bytes;
        *(DWORD*)(bytes + 16u) = name_bytes;

        if (length >= required) {
            /* Name 区不要求 NUL 终止，NameLength 就是有效 UTF-16 字节数。 */
            if (value_name && value_name->Buffer) {
                for (i = 0u; i < name_bytes; ++i) bytes[20u + i] = ((const BYTE*)value_name->Buffer)[i];
            }
            for (i = 20u + name_bytes; i < data_offset; ++i) bytes[i] = 0u;
            for (i = 0u; i < data_bytes; ++i) bytes[data_offset + i] = ((const BYTE*)data)[i];
            for (i = data_offset + data_bytes; i < required; ++i) bytes[i] = 0u;
        }
        return length < required ? STATUS_BUFFER_OVERFLOW_ : STATUS_SUCCESS_;
    }

    /* BasicInformation/未知类别不应该由本函数“猜一个结构”出来。 */
    return STATUS_INVALID_PARAMETER_;
}

static LONG WINAPI Hook_NtQueryValueKey_(HANDLE key,
                                         LPVOID value_name_raw,
                                         DWORD info_class,
                                         LPVOID info,
                                         DWORD length,
                                         DWORD* result_length) {
    /* 先整体清零，既保证 REG_MULTI_SZ 的第二个 NUL 明确存在，也让静态分析器能证明返回缓冲每个字节都有定义。 */
    WCHAR fake[64] = {0};
    DWORD type = 0u;
    UINT chars = 0u;
    UNICODE_STRING_MIN_* value_name = (UNICODE_STRING_MIN_*)value_name_raw;

    if (lookup_registry_fake_(key, value_name, fake, 64u, &type, &chars)) {
        /*
         * BasicInformation 只询问值名/类型，不含 Data。语言伪造真正关心的是数据内容，
         * 所以 Basic 继续走真实 NtQueryValueKey，避免无端改变元数据行为。
         */
        if (info_class == KEY_VALUE_BASIC_INFORMATION_) {
            return g_real_NtQueryValueKey
                ? g_real_NtQueryValueKey(key, value_name_raw, info_class, info, length, result_length)
                : STATUS_SUCCESS_;
        }
        return fill_registry_result_(value_name, info_class, info, length, result_length, type, fake, chars);
    }

    /* 不属于那 8 个语言值时，必须完全透明地回到真实系统调用。 */
    return g_real_NtQueryValueKey
        ? g_real_NtQueryValueKey(key, value_name_raw, info_class, info, length, result_length)
        : STATUS_SUCCESS_;
}

/* ---------- x86 五字节 syscall stub Hook ---------- */

static int install_one_hook_(NATIVE_HOOK_* hook) {
    BYTE* tramp;
    BYTE patch[5];
    LONG rel;
    DWORD old_protect = 0u, ignored = 0u;
    UINT i;

    if (!hook || !hook->target || !hook->replacement) return 0;
    if (hook->installed) return 1;

    /*
     * 这里是最重要的安全门：只接受第一条完整指令恰好是 5 字节 `mov eax, imm32`。
     * 如果遇到 E9、8B、FF 等其它布局，说明 Windows 版本/第三方组件改变了 stub，立即停止而不是截断未知指令。
     */
    if (hook->target[0] != 0xB8u) {
        log_hook_error_((const WCHAR*)L"[Locale/Native错误] ntdll syscall stub 不是预期的 B8 mov-eax，拒绝 Hook：", hook->name);
        return 0;
    }

    tramp = (BYTE*)VirtualAlloc(NULL_PTR, 16u, MEM_COMMIT_ | MEM_RESERVE_, PAGE_EXECUTE_READWRITE_);
    if (!tramp) {
        log_hook_error_((const WCHAR*)L"[Locale/Native错误] 无法申请 syscall trampoline：", hook->name);
        return 0;
    }

    for (i = 0u; i < 5u; ++i) {
        hook->original[i] = hook->target[i];
        tramp[i] = hook->target[i];
    }

    /* trampoline：先执行原来的 5 字节 mov eax, syscall-id，再跳回 target+5。 */
    tramp[5] = 0xE9u;
    rel = (LONG)((BYTE*)hook->target + 5u - (tramp + 10u));
    *(LONG*)(tramp + 6u) = rel;
    hook->trampoline = tramp;

    /* target：改成 JMP 我们自己的 replacement。 */
    patch[0] = 0xE9u;
    rel = (LONG)((BYTE*)hook->replacement - (hook->target + 5u));
    *(LONG*)(patch + 1u) = rel;

    if (!VirtualProtect(hook->target, 5u, PAGE_EXECUTE_READWRITE_, &old_protect)) {
        log_hook_error_((const WCHAR*)L"[Locale/Native错误] 无法修改 ntdll stub 内存保护：", hook->name);
        return 0;
    }
    for (i = 0u; i < 5u; ++i) hook->target[i] = patch[i];
    FlushInstructionCache(GetCurrentProcess(), hook->target, 5u);
    VirtualProtect(hook->target, 5u, old_protect, &ignored);

    hook->installed = 1;
    return 1;
}

static void add_hook_(const char* name, FARPROC target, LPVOID replacement) {
    NATIVE_HOOK_* h;
    if (g_hook_count >= MAX_NATIVE_HOOKS_) return;
    h = &g_hooks[g_hook_count++];
    h->name = name;
    h->target = (BYTE*)target;
    h->replacement = replacement;
    h->trampoline = NULL_PTR;
    h->installed = 0;
}

static int resolve_and_install_(void) {
    UINT i;
    if (!g_ntdll) g_ntdll = GetModuleHandleW((const WCHAR*)L"ntdll.dll");
    if (!g_ntdll) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] 找不到 ntdll.dll。");
        return 0;
    }

    if (g_hook_count == 0u) {
        FARPROC p_locale = GetProcAddress(g_ntdll, "NtQueryDefaultLocale");
        FARPROC p_ui = GetProcAddress(g_ntdll, "NtQueryDefaultUILanguage");
        FARPROC p_install_ui = GetProcAddress(g_ntdll, "NtQueryInstallUILanguage");
        FARPROC p_tz = GetProcAddress(g_ntdll, "NtQuerySystemInformation");
        FARPROC p_nls = GetProcAddress(g_ntdll, "NtInitializeNlsFiles");
        FARPROC p_query_key = GetProcAddress(g_ntdll, "NtQueryKey");
        FARPROC p_query_value = GetProcAddress(g_ntdll, "NtQueryValueKey");
        FARPROC p_custom_cp = GetProcAddress(g_ntdll, "RtlCustomCPToUnicodeN");
        FARPROC p_init_cp_table = GetProcAddress(g_ntdll, "RtlInitCodePageTable");

        if (!p_locale || !p_ui || !p_install_ui || !p_tz || !p_nls || !p_query_key || !p_query_value ||
            !p_custom_cp || !p_init_cp_table) {
            ModLoader_Log((const WCHAR*)L"[Locale/Native错误] ntdll 缺少 Locale Emulator 等价层所需的 Locale/UI/时区/NLS/CPTABLEINFO 入口。");
            return 0;
        }

        add_hook_("NtQueryDefaultLocale", p_locale, (LPVOID)&Hook_NtQueryDefaultLocale_);
        add_hook_("NtQueryDefaultUILanguage", p_ui, (LPVOID)&Hook_NtQueryDefaultUILanguage_);
        add_hook_("NtQueryInstallUILanguage", p_install_ui, (LPVOID)&Hook_NtQueryInstallUILanguage_);
        add_hook_("NtQuerySystemInformation", p_tz, (LPVOID)&Hook_NtQuerySystemInformation_);
        add_hook_("NtInitializeNlsFiles", p_nls, (LPVOID)&Hook_NtInitializeNlsFiles_);
        add_hook_("NtQueryValueKey", p_query_value, (LPVOID)&Hook_NtQueryValueKey_);
        g_real_NtQueryKey = (PFN_NtQueryKey_)p_query_key;
        g_RtlInitCodePageTable = (PFN_RtlInitCodePageTable_)p_init_cp_table;

        /*
         * RtlCustomCPToUnicodeN 不是 syscall，所以单独登记到普通函数 Hook 结构。
         * 此时只保存地址；真正写入口要等下面所有 syscall trampoline 都准备好之后统一完成。
         */
        g_custom_cp_hook.name = "RtlCustomCPToUnicodeN";
        g_custom_cp_hook.target = (BYTE*)p_custom_cp;
        g_custom_cp_hook.replacement = (LPVOID)&Hook_RtlCustomCPToUnicodeN_;
        g_custom_cp_hook.original_length = 0u;
        g_custom_cp_hook.trampoline = NULL_PTR;
        g_custom_cp_hook.installed = 0;
    }

    for (i = 0u; i < g_hook_count; ++i) {
        if (!install_one_hook_(&g_hooks[i])) return 0;
    }

    /*
     * 再安装 Locale Emulator Core 同类的 RtlCustomCPToUnicodeN 修复层。
     * 如果当前 Windows 的函数入口无法被我们的安全解码器确认，就直接让整个 Locale 初始化失败。
     */
    if (!install_function_hook_(&g_custom_cp_hook)) return 0;
    g_real_RtlCustomCPToUnicodeN = (PFN_RtlCustomCPToUnicodeN_)g_custom_cp_hook.trampoline;

    /*
     * 所有 trampoline 地址现在已经建立，再把 real 指针改成 trampoline。
     * 这样 Hook 内调用“原函数”时不会重新跳回自己形成递归。
     */
    /*
     * 数组顺序由上面的 add_hook_ 固定；这里显式绑定每个 trampoline，
     * 避免用字符串下标猜函数名这种难懂且容易写错的做法。
     */
    if (g_hook_count >= 6u) {
        g_real_NtQueryDefaultLocale = (PFN_NtQueryDefaultLocale_)g_hooks[0].trampoline;
        g_real_NtQueryDefaultUILanguage = (PFN_NtQueryDefaultUILanguage_)g_hooks[1].trampoline;
        g_real_NtQueryInstallUILanguage = (PFN_NtQueryInstallUILanguage_)g_hooks[2].trampoline;
        g_real_NtQuerySystemInformation = (PFN_NtQuerySystemInformation_)g_hooks[3].trampoline;
        g_real_NtInitializeNlsFiles = (PFN_NtInitializeNlsFiles_)g_hooks[4].trampoline;
        g_real_NtQueryValueKey = (PFN_NtQueryValueKey_)g_hooks[5].trampoline;
    }
    return 1;
}

static int self_test_(void) {
    DWORD locale = 0u;
    WORD ui = 0u, install_ui = 0u;
    DWORD* teb_locale;
    TIME_ZONE_INFORMATION_ timezone;
    DWORD timezone_bytes = 0u;
    PFN_NtQuerySystemInformation_ query_system_info;

    /* 调用当前 ntdll 导出，此时目标入口已经跳到我们的 Hook，验证实际运行路径。 */
    if (((PFN_NtQueryDefaultLocale_)GetProcAddress(g_ntdll, "NtQueryDefaultLocale"))(TRUE_, &locale) < 0 || locale != NATIVE_LOCALE_LCID_) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] NtQueryDefaultLocale 自检未返回 0x0404。");
        return 0;
    }
    if (((PFN_NtQueryDefaultUILanguage_)GetProcAddress(g_ntdll, "NtQueryDefaultUILanguage"))(&ui) < 0 || ui != (WORD)NATIVE_LOCALE_LANGID_) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] NtQueryDefaultUILanguage 自检未返回 0x0404。");
        return 0;
    }
    if (((PFN_NtQueryInstallUILanguage_)GetProcAddress(g_ntdll, "NtQueryInstallUILanguage"))(&install_ui) < 0 || install_ui != (WORD)NATIVE_LOCALE_LANGID_) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] NtQueryInstallUILanguage 自检未返回 0x0404。");
        return 0;
    }
    teb_locale = current_teb_locale_ptr_();
    if (!teb_locale || *teb_locale != NATIVE_LOCALE_LCID_) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] TEB.CurrentLocale 自检未达到 0x0404。");
        return 0;
    }

    /*
     * 时区同样走已经被我们 Hook 的 ntdll 导出做运行态验证。
     * 这里不只看 Hook 是否“安装成功”，而是确认真正返回给调用者的 Bias 已经是台北 UTC+8 的 -480。
     */
    zero_bytes_((BYTE*)&timezone, (UINT)sizeof(timezone));
    query_system_info = (PFN_NtQuerySystemInformation_)GetProcAddress(g_ntdll, "NtQuerySystemInformation");
    if (!query_system_info ||
        query_system_info(SYSTEM_CURRENT_TIME_ZONE_INFORMATION_, &timezone, (DWORD)sizeof(timezone), &timezone_bytes) < 0 ||
        timezone.Bias != -480) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] NtQuerySystemInformation 时区自检未达到 Taipei UTC+8。");
        return 0;
    }

    if (!g_custom_cp_hook.installed || !g_real_RtlCustomCPToUnicodeN || !g_RtlInitCodePageTable || !current_ansi_nls_table_()) {
        ModLoader_Log((const WCHAR*)L"[Locale/Native错误] RtlCustomCPToUnicodeN / CP950 CPTABLEINFO 修复层没有进入可用状态。");
        return 0;
    }

    ModLoader_Log((const WCHAR*)L"[Locale/Native] ntdll 默认 Locale/UI Language/安装语言、主线程 TEB.CurrentLocale 与 Taipei 时区均已通过运行态自检。");
    ModLoader_Log((const WCHAR*)L"[Locale/Native] NtQueryValueKey 语言注册表重定向已安装；只覆盖用户指定的 8 个 zh-TW/CP950 值。");
    ModLoader_Log((const WCHAR*)L"[Locale/Native] Locale Emulator 等价 CPTABLEINFO 修复层已安装：RtlCustomCPToUnicodeN 遇到陈旧宿主代码页表时会重建为 CP950。");
    return 1;
}

LONG NativeLocale_GetCustomCpRepairCount(void) {
    /*
     * 只返回一个诊断计数，不修改任何状态。locale_layer.c 会在实体 CreateFileA 自检之后读取它，
     * 这样用户能直接知道本机是否真的命中了“陈旧 CPTABLEINFO -> CP950”这条修复路径。
     */
    return g_custom_cp_repair_count;
}

int NativeLocale_Initialize(void) {
    if (!force_current_thread_locale_()) return 0;
    if (!resolve_and_install_()) return 0;
    if (!self_test_()) return 0;
    g_initialized = 1;
    return 1;
}

int NativeLocale_Refresh(void) {
    if (!g_initialized) return NativeLocale_Initialize();
    /* KERNEL32/USER32 后续初始化可能再次写线程 Locale，所以 EntryPoint 前再强制一次并自检。 */
    if (!force_current_thread_locale_()) return 0;
    return self_test_();
}
