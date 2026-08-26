/*
 * CastleLocaleBootstrap.dll
 *
 * 这是整个《幽城幻剑录》Mod Loader 中“最早、最窄”的区域环境启动层。
 *
 * 设计目标：
 *   1. 只负责把当前 32 位目标进程的 ANSI/OEM NLS 翻译表切到 CP950；
 *   2. 这一层不能依赖 KERNEL32，因为它要尽量接近 Locale Emulator 的 pre-Kernel32 时序；
 *   3. 只使用 ntdll 的 Nt/Rtl 原语，不扫描 Mod、不写日志、不加载 ASI、不安装普通 Win32 Hook；
 *   4. 后面的 CastleModCore.dll 会在 KERNEL32 正常初始化后读取这里的状态，再继续做
 *      zh-TW/台北时区/语言注册表/UI 语言/Overrides/ASI。
 *
 * 为什么要把它单独拆出来：
 *   v0.2.3 把“真实 NLS 重建”和普通 Mod Core 混在同一个 DllMain 里。
 *   那个 Core 静态依赖 KERNEL32，又在 DLL_PROCESS_ATTACH 里调用大量 KERNEL32 API，
 *   与我们试图模拟的 LE 早期时序互相冲突，实机直接出现 0xc0000142。
 *
 * 这里故意不复制 Locale Emulator 的实现代码，只复现本项目明确需要的行为：
 *   c_950.nls + l_intl.nls -> RtlInitNlsTables -> RtlResetRtlTranslations -> PEB NLS table pointers。
 */

/* ---------- 最小基础类型：避免包含会自动引入 KERNEL32 的 Win32 头 ---------- */

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef unsigned long ULONG_PTR;
typedef unsigned long SIZE_T;
typedef long LONG;
typedef LONG NTSTATUS;
typedef int BOOL;
typedef void* PVOID;
typedef void* HANDLE;
typedef unsigned short wchar_t;
typedef wchar_t WCHAR;
typedef const WCHAR* PCWSTR;
typedef unsigned short USHORT;
typedef unsigned char BOOLEAN;

typedef struct LARGE_INTEGER_ {
    DWORD LowPart;
    LONG HighPart;
} LARGE_INTEGER_;

typedef struct UNICODE_STRING_ {
    USHORT Length;
    USHORT MaximumLength;
    WCHAR* Buffer;
} UNICODE_STRING_;

typedef struct OBJECT_ATTRIBUTES_ {
    ULONG Length;
    HANDLE RootDirectory;
    UNICODE_STRING_* ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES_;

typedef struct IO_STATUS_BLOCK_ {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    } u;
    ULONG_PTR Information;
} IO_STATUS_BLOCK_;

typedef struct FILE_STANDARD_INFORMATION_ {
    LARGE_INTEGER_ AllocationSize;
    LARGE_INTEGER_ EndOfFile;
    ULONG NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
    WORD Padding;
} FILE_STANDARD_INFORMATION_;

typedef struct CPTABLEINFO_ {
    WORD CodePage;
    WORD MaximumCharacterSize;
    WORD DefaultChar;
    WORD UniDefaultChar;
    WORD TransDefaultChar;
    WORD TransUniDefaultChar;
    WORD DBCSCodePage;
    BYTE LeadByte[12];
    WORD* MultiByteTable;
    PVOID WideCharTable;
    WORD* DBCSRanges;
    WORD* DBCSOffsets;
} CPTABLEINFO_;

typedef struct NLSTABLEINFO_ {
    CPTABLEINFO_ OemTableInfo;
    CPTABLEINFO_ AnsiTableInfo;
    WORD* UpperCaseTable;
    WORD* LowerCaseTable;
} NLSTABLEINFO_;

typedef char CPTABLEINFO_size_must_be_44_[(sizeof(CPTABLEINFO_) == 44u) ? 1 : -1];
typedef char NLSTABLEINFO_size_must_be_96_[(sizeof(NLSTABLEINFO_) == 96u) ? 1 : -1];

/* ---------- ntdll 导入 ---------- */

#define NTAPI __stdcall
#define DLL_EXPORT __declspec(dllexport)

__declspec(dllimport) NTSTATUS NTAPI NtOpenFile(HANDLE* file_handle, ULONG desired_access,
    OBJECT_ATTRIBUTES_* object_attributes, IO_STATUS_BLOCK_* io_status,
    ULONG share_access, ULONG open_options);
__declspec(dllimport) NTSTATUS NTAPI NtQueryInformationFile(HANDLE file_handle, IO_STATUS_BLOCK_* io_status,
    PVOID file_information, ULONG length, ULONG file_information_class);
__declspec(dllimport) NTSTATUS NTAPI NtReadFile(HANDLE file_handle, HANDLE event, PVOID apc_routine, PVOID apc_context,
    IO_STATUS_BLOCK_* io_status, PVOID buffer, ULONG length, LARGE_INTEGER_* byte_offset, ULONG* key);
__declspec(dllimport) NTSTATUS NTAPI NtAllocateVirtualMemory(HANDLE process_handle, PVOID* base_address,
    ULONG_PTR zero_bits, SIZE_T* region_size, ULONG allocation_type, ULONG protect);
__declspec(dllimport) NTSTATUS NTAPI NtProtectVirtualMemory(HANDLE process_handle, PVOID* base_address,
    SIZE_T* region_size, ULONG new_protect, ULONG* old_protect);
__declspec(dllimport) NTSTATUS NTAPI NtClose(HANDLE handle);
__declspec(dllimport) void NTAPI RtlInitUnicodeString(UNICODE_STRING_* destination, PCWSTR source);
__declspec(dllimport) void NTAPI RtlInitNlsTables(WORD* ansi, WORD* oem, WORD* casing, NLSTABLEINFO_* table_info);
__declspec(dllimport) void NTAPI RtlResetRtlTranslations(const NLSTABLEINFO_* table_info);

/* ---------- NT 常量 ---------- */

#define STATUS_SUCCESS_ ((NTSTATUS)0)
#define CURRENT_PROCESS_ ((HANDLE)(ULONG_PTR)0xFFFFFFFFu)
#define OBJ_CASE_INSENSITIVE_ 0x00000040u
#define FILE_READ_DATA_ 0x00000001u
#define FILE_READ_ATTRIBUTES_ 0x00000080u
#define SYNCHRONIZE_ 0x00100000u
#define FILE_SHARE_READ_ 0x00000001u
#define FILE_SHARE_WRITE_ 0x00000002u
#define FILE_SHARE_DELETE_ 0x00000004u
#define FILE_SYNCHRONOUS_IO_NONALERT_ 0x00000020u
#define FILE_NON_DIRECTORY_FILE_ 0x00000040u
#define FileStandardInformation_ 5u
#define MEM_COMMIT_ 0x00001000u
#define MEM_RESERVE_ 0x00002000u
#define PAGE_READWRITE_ 0x00000004u
#define PAGE_READONLY_ 0x00000002u
#define NLS_ALIGN_ 16u
#define MAX_NLS_FILE_BYTES_ (32u * 1024u * 1024u)

/*
 * 状态值是故意稳定的公开 ABI：后面的 CastleModCore.dll 会查询它。
 * 1 代表成功；负数分别表示哪一步失败，方便 Core 在普通 Win32 环境里把原因写入日志。
 */
#define BOOTSTRAP_OK_ 1
#define BOOTSTRAP_ERR_OPEN_CP_ -10
#define BOOTSTRAP_ERR_SIZE_CP_ -11
#define BOOTSTRAP_ERR_OPEN_CASE_ -12
#define BOOTSTRAP_ERR_SIZE_CASE_ -13
#define BOOTSTRAP_ERR_ALLOC_ -14
#define BOOTSTRAP_ERR_READ_CP_ -15
#define BOOTSTRAP_ERR_READ_CASE_ -16
#define BOOTSTRAP_ERR_PROTECT_ -17
#define BOOTSTRAP_ERR_PEB_ -18

/*
 * 下面两个值只用于诊断，不参与“成功/失败”判断。
 *
 * v0.2.4 曾经把 RtlInitNlsTables 输出结构里的 CodePage 字段强制要求为 950，
 * 但 Locale Emulator 的成熟实现并没有用这两个结构字段作为启动门槛。
 * 用户实机已经证明这条额外验证会把已经完成到 RtlResetRtlTranslations 的流程错误判成 -18。
 *
 * 因此 v0.2.5 只把它们保存下来，交给普通 Core 写进日志；
 * 真正的硬验证移动到 KERNEL32 已经完成初始化之后，直接调用未经过我们 IAT Hook 的
 * GetACP / GetOEMCP，只有真实运行态返回 950/950 才算转区核心通过。
 */
static volatile LONG g_status = 0;
static volatile WORD g_observed_ansi_codepage = 0u;
static volatile WORD g_observed_oem_codepage = 0u;
static BYTE* g_nls_block = (BYTE*)0;

/* ---------- 很小的辅助函数 ---------- */

static ULONG align16_(ULONG value) {
    return (value + (NLS_ALIGN_ - 1u)) & ~(NLS_ALIGN_ - 1u);
}

static BYTE* current_peb_(void) {
#if defined(_M_IX86)
    BYTE* peb = (BYTE*)0;
    __asm {
        mov eax, fs:[30h]
        mov peb, eax
    }
    return peb;
#else
    return (BYTE*)0;
#endif
}

static void init_object_attributes_(OBJECT_ATTRIBUTES_* oa, UNICODE_STRING_* name) {
    oa->Length = (ULONG)sizeof(*oa);
    oa->RootDirectory = (HANDLE)0;
    oa->ObjectName = name;
    oa->Attributes = OBJ_CASE_INSENSITIVE_;
    oa->SecurityDescriptor = (PVOID)0;
    oa->SecurityQualityOfService = (PVOID)0;
}

static int nt_success_(NTSTATUS status) {
    return status >= 0;
}

/*
 * 用纯 ntdll 打开文件并取得尺寸。
 * 路径使用对象管理器可识别的 \SystemRoot\System32\...，因此不需要 GetSystemDirectoryW。
 */
static int open_and_size_(PCWSTR path, HANDLE* file_out, ULONG* size_out,
    int open_error, int size_error) {
    UNICODE_STRING_ name;
    OBJECT_ATTRIBUTES_ oa;
    IO_STATUS_BLOCK_ iosb;
    FILE_STANDARD_INFORMATION_ info;
    HANDLE file = (HANDLE)0;
    NTSTATUS status;

    /*
     * 这里把“打不开文件”和“能打开但尺寸查询/尺寸合法性失败”分成两个稳定状态码。
     * v0.2.3 的 0xc0000142 最大问题之一就是把非常具体的 Locale 失败压成一个通用启动错误；
     * v0.2.4 既然已经专门做 Bootstrap，就必须让后面的 Core 能知道到底卡在哪一步。
     */
    if (!path || !file_out || !size_out) return open_error;
    *file_out = (HANDLE)0;
    *size_out = 0u;

    RtlInitUnicodeString(&name, path);
    init_object_attributes_(&oa, &name);

    status = NtOpenFile(&file,
        FILE_READ_DATA_ | FILE_READ_ATTRIBUTES_ | SYNCHRONIZE_,
        &oa, &iosb,
        FILE_SHARE_READ_ | FILE_SHARE_WRITE_ | FILE_SHARE_DELETE_,
        FILE_SYNCHRONOUS_IO_NONALERT_ | FILE_NON_DIRECTORY_FILE_);
    if (!nt_success_(status)) return open_error;

    status = NtQueryInformationFile(file, &iosb, &info, (ULONG)sizeof(info), FileStandardInformation_);
    if (!nt_success_(status) || info.EndOfFile.HighPart != 0 || info.EndOfFile.LowPart == 0u ||
        info.EndOfFile.LowPart > MAX_NLS_FILE_BYTES_) {
        NtClose(file);
        return size_error;
    }

    *file_out = file;
    *size_out = info.EndOfFile.LowPart;
    return BOOTSTRAP_OK_;
}

static int read_exact_(HANDLE file, BYTE* dst, ULONG size) {
    IO_STATUS_BLOCK_ iosb;
    LARGE_INTEGER_ offset;
    NTSTATUS status;
    offset.LowPart = 0u;
    offset.HighPart = 0;
    status = NtReadFile(file, (HANDLE)0, (PVOID)0, (PVOID)0, &iosb, dst, size, &offset, (ULONG*)0);
    return nt_success_(status) && iosb.Information == (ULONG_PTR)size;
}

static int initialize_cp950_(void) {
    static const WCHAR cp_path[] = L"\\SystemRoot\\System32\\c_950.nls";
    static const WCHAR case_path[] = L"\\SystemRoot\\System32\\l_intl.nls";
    HANDLE cp_file = (HANDLE)0;
    HANDLE case_file = (HANDLE)0;
    ULONG cp_size = 0u, case_size = 0u;
    ULONG ansi_off, oem_off, case_off;
    SIZE_T total;
    PVOID base = (PVOID)0;
    BYTE* peb;
    WORD *ansi, *oem, *casing;
    NLSTABLEINFO_ table_info;
    BYTE* table_bytes = (BYTE*)&table_info;
    ULONG i;
    ULONG old_protect = 0u;
    PVOID protect_base;
    SIZE_T protect_size;
    NTSTATUS status;

    {
        int step = open_and_size_(cp_path, &cp_file, &cp_size,
            BOOTSTRAP_ERR_OPEN_CP_, BOOTSTRAP_ERR_SIZE_CP_);
        if (step != BOOTSTRAP_OK_) return step;
    }
    {
        int step = open_and_size_(case_path, &case_file, &case_size,
            BOOTSTRAP_ERR_OPEN_CASE_, BOOTSTRAP_ERR_SIZE_CASE_);
        if (step != BOOTSTRAP_OK_) {
            NtClose(cp_file);
            return step;
        }
    }

    ansi_off = 0u;
    oem_off = align16_(cp_size);
    case_off = oem_off + align16_(cp_size);
    total = (SIZE_T)case_off + (SIZE_T)case_size;
    if (total <= (SIZE_T)case_off || total > (SIZE_T)(MAX_NLS_FILE_BYTES_ * 3u)) {
        NtClose(cp_file);
        NtClose(case_file);
        return BOOTSTRAP_ERR_SIZE_CASE_;
    }

    status = NtAllocateVirtualMemory(CURRENT_PROCESS_, &base, 0u, &total,
        MEM_COMMIT_ | MEM_RESERVE_, PAGE_READWRITE_);
    if (!nt_success_(status) || !base) {
        NtClose(cp_file);
        NtClose(case_file);
        return BOOTSTRAP_ERR_ALLOC_;
    }
    g_nls_block = (BYTE*)base;

    if (!read_exact_(cp_file, g_nls_block + ansi_off, cp_size)) {
        NtClose(cp_file);
        NtClose(case_file);
        return BOOTSTRAP_ERR_READ_CP_;
    }
    NtClose(cp_file);

    /* ANSI 与 OEM 都固定为 CP950，但保持两份独立表，和 LE 的目标状态一致。 */
    for (i = 0u; i < cp_size; ++i)
        g_nls_block[oem_off + i] = g_nls_block[ansi_off + i];

    if (!read_exact_(case_file, g_nls_block + case_off, case_size)) {
        NtClose(case_file);
        return BOOTSTRAP_ERR_READ_CASE_;
    }
    NtClose(case_file);

    protect_base = base;
    protect_size = total;
    status = NtProtectVirtualMemory(CURRENT_PROCESS_, &protect_base, &protect_size, PAGE_READONLY_, &old_protect);
    if (!nt_success_(status)) return BOOTSTRAP_ERR_PROTECT_;

    for (i = 0u; i < (ULONG)sizeof(table_info); ++i) table_bytes[i] = 0u;

    ansi = (WORD*)(g_nls_block + ansi_off);
    oem = (WORD*)(g_nls_block + oem_off);
    casing = (WORD*)(g_nls_block + case_off);

    RtlInitNlsTables(ansi, oem, casing, &table_info);
    RtlResetRtlTranslations(&table_info);

    /*
     * 记录 RtlInitNlsTables 暴露出来的 CodePage 字段，但不再把它当成成功门槛。
     * 这两个字段在不同 Windows 内部实现/初始化阶段下并不是我们应该依赖的 ABI；
     * LE 自己的成熟流程也是完成 Reset 后直接切换 PEB NLS 数据指针，而不是先检查这里。
     */
    g_observed_ansi_codepage = table_info.AnsiTableInfo.CodePage;
    g_observed_oem_codepage = table_info.OemTableInfo.CodePage;

    /*
     * x86 PEB 中这三个字段保存的是“整份 NLS 原始数据映射”的起始地址，
     * 不是 CPTABLEINFO 里解析后的 MultiByteTable/WideCharTable。
     * 这里写入后马上读回，只验证我们真正能控制、并且 LE 也会设置的 PEB 状态。
     */
    peb = current_peb_();
    if (!peb) return BOOTSTRAP_ERR_PEB_;
    *(PVOID*)(peb + 0x58u) = (PVOID)ansi;
    *(PVOID*)(peb + 0x5Cu) = (PVOID)oem;
    *(PVOID*)(peb + 0x60u) = (PVOID)casing;

    if (*(PVOID*)(peb + 0x58u) != (PVOID)ansi ||
        *(PVOID*)(peb + 0x5Cu) != (PVOID)oem ||
        *(PVOID*)(peb + 0x60u) != (PVOID)casing)
        return BOOTSTRAP_ERR_PEB_;

    return BOOTSTRAP_OK_;
}

/* ---------- 稳定导出 ---------- */

DLL_EXPORT void __cdecl CastleLocaleBootstrap_Bootstrap(void) {
    /* 仅用于 PE Import-by-Name。真正工作在 DllMain 中完成。 */
}

DLL_EXPORT int __cdecl CastleLocaleBootstrap_GetStatus(void) {
    return (int)g_status;
}

/*
 * 这两个导出只给 CastleModCore.dll 写诊断日志使用。
 * 它们不是稳定性判断依据，也不会被第三方 Mod 调用。
 */
DLL_EXPORT unsigned int __cdecl CastleLocaleBootstrap_GetObservedAnsiCodePage(void) {
    return (unsigned int)g_observed_ansi_codepage;
}

DLL_EXPORT unsigned int __cdecl CastleLocaleBootstrap_GetObservedOemCodePage(void) {
    return (unsigned int)g_observed_oem_codepage;
}

BOOL NTAPI DllMain(PVOID module, DWORD reason, PVOID reserved) {
    (void)module;
    (void)reserved;

    if (reason == 1u) { /* DLL_PROCESS_ATTACH */
        g_status = (LONG)initialize_cp950_();

        /*
         * 与 v0.2.3 不同：这里不因为 Locale 初始化失败就让 Windows 报 0xc0000142。
         * Bootstrap 只负责记录明确状态；后面的 Core 会在 KERNEL32 可用后把失败原因写进日志，
         * 并决定是否继续。这样启动失败不会再表现成一个完全没有诊断信息的通用 DLL 初始化错误。
         */
    }
    return 1;
}
