#include "platform.h"
#include "pe_import_injector.h"

/*
 * 这个模块是 Pre-Loader 最关键的“像 LEProc 一样先创建目标，再把 Core 放进装载链”的实现。
 *
 * Locale Emulator 的公开源码使用 CreateProcessWithDll(..., CPWD_BEFORE_KERNEL32, ...)。
 * 我们没有复制它的实现，而是针对 32 位 PE 独立实现一个更小的办法：
 *   1. Launcher 用 CREATE_SUSPENDED 创建 RPG.exe；此时磁盘映像已经映射进目标进程，但主线程还没开始正常跑。
 *   2. 找到目标 EXE 的导入目录和 .rdata 一类节末尾的“文件对齐填充区”。
 *   3. 在这块原本无业务数据的填充区里复制原导入描述符，并在最前面加入调用方指定的两个 Loader DLL。
 *      dev9 为了精确恢复 dev5 的已知成功装载语义，实际传入的是纯文件名
 *      CastleLocaleBootstrap.dll 与 CastleModCore.dll；Launcher 在 CreateProcess 前临时建立 mods\ DLL 搜索环境。
 *   4. 只改目标进程内存中的 Import Directory RVA；硬盘 RPG.exe 一个字节都不动。
 *   5. ResumeThread 后，Windows 自己照常解析新的导入目录，于是两个 Loader DLL 作为启动依赖进入目标进程。
 *
 * 注意：Injector 只负责把调用方给的 DLL 名字原样写进 Import Directory，不负责 DLL 搜索。
 * dev9 的 Launcher 会在 CreateProcess 前临时 SetDllDirectoryW(mods)，因此目标进程创建时继承 dev5 的启动搜索环境；
 * Core 进入目标进程后再执行一次 SetDllDirectoryW(mods)，恢复 dev5 的进程级兼容环境。两层职责要分开理解。
 *
 * 为什么不再 CreateRemoteThread(LoadLibrary)：
 *   那会把 Core 放到“进程已经开始初始化以后”的另一个时点，和用户要求的 LE 同型启动顺序不一致。
 *   新实现让 Core 由 Windows Loader 自己加载，属于目标进程正常装载图的一部分。
 */

#define MAX_HEADER_BYTES_ 4096u
#define MAX_IMPORT_DESCRIPTORS_ 64u
#define MAX_PAYLOAD_BYTES_ 4096u
#define PROCESS_BASIC_INFORMATION_CLASS_ 0u
#define STATUS_SUCCESS_ 0L

/* PE32 Optional Header 中 DataDirectory 的固定起点。 */
#define PE32_DATA_DIRECTORY_OFFSET_ 96u
#define PE_IMPORT_DIRECTORY_INDEX_ 1u

/* IMAGE_SECTION_HEADER.Characteristics 中“包含可读初始化数据”的常见位。我们这里只用来优先选择 .rdata 类节。 */
#define IMAGE_SCN_CNT_INITIALIZED_DATA_ 0x00000040u
#define IMAGE_SCN_MEM_READ_             0x40000000u

/* Windows 的 IMAGE_IMPORT_DESCRIPTOR 正好由五个 DWORD 组成。 */
typedef struct IMPORT_DESCRIPTOR_ {
    DWORD OriginalFirstThunk;
    DWORD TimeDateStamp;
    DWORD ForwarderChain;
    DWORD Name;
    DWORD FirstThunk;
} IMPORT_DESCRIPTOR_;

/* x86 PROCESS_BASIC_INFORMATION；这里只真正使用 PebBaseAddress。 */
typedef struct PROCESS_BASIC_INFORMATION32_ {
    LPVOID Reserved1;
    LPVOID PebBaseAddress;
    LPVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    LPVOID Reserved3;
} PROCESS_BASIC_INFORMATION32_;

typedef LONG (WINAPI *PFN_NtQueryInformationProcess_)(HANDLE, DWORD, LPVOID, DWORD, DWORD*);

static BYTE g_headers[MAX_HEADER_BYTES_];
static IMPORT_DESCRIPTOR_ g_old_desc[MAX_IMPORT_DESCRIPTORS_ + 1u];
static BYTE g_payload[MAX_PAYLOAD_BYTES_];

static UINT cstrlen_(const char* s) {
    UINT n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}

static void zero_bytes_(BYTE* p, UINT count) {
    UINT i;
    for (i = 0; i < count; ++i) p[i] = 0;
}

static UINT align_up_(UINT value, UINT alignment) {
    return (value + alignment - 1u) & ~(alignment - 1u);
}

static int get_remote_image_base_(HANDLE process, BYTE** image_base) {
    HMODULE ntdll;
    PFN_NtQueryInformationProcess_ nt_query;
    PROCESS_BASIC_INFORMATION32_ pbi;
    DWORD returned = 0;
    SIZE_T read = 0;
    DWORD remote_base = 0;

    if (!process || !image_base) return 0;
    *image_base = NULL_PTR;

    ntdll = GetModuleHandleW((const WCHAR*)L"ntdll.dll");
    if (!ntdll) return 0;
    nt_query = (PFN_NtQueryInformationProcess_)GetProcAddress(ntdll, "NtQueryInformationProcess");
    if (!nt_query) return 0;

    zero_bytes_((BYTE*)&pbi, (UINT)sizeof(pbi));
    if (nt_query(process, PROCESS_BASIC_INFORMATION_CLASS_, &pbi, (DWORD)sizeof(pbi), &returned) != STATUS_SUCCESS_)
        return 0;

    /* x86 PEB 的 ImageBaseAddress 位于偏移 0x08。目标固定为 32 位 RPG.exe。 */
    if (!ReadProcessMemory(process, (BYTE*)pbi.PebBaseAddress + 8u, &remote_base, 4u, &read) || read != 4u)
        return 0;

    *image_base = (BYTE*)(ULONG_PTR)remote_base;
    return remote_base != 0u;
}

static int section_name_is_rdata_(const BYTE* section) {
    return section[0] == (BYTE)'.' &&
           section[1] == (BYTE)'r' &&
           section[2] == (BYTE)'d' &&
           section[3] == (BYTE)'a' &&
           section[4] == (BYTE)'t' &&
           section[5] == (BYTE)'a';
}

int PeImportInjector_AddEarlyImports2(HANDLE process,
    const char* first_dll, const char* first_import,
    const char* second_dll, const char* second_import) {
    BYTE* base;
    SIZE_T read = 0, written = 0;
    DWORD pe_off, optional_off, section_off;
    WORD section_count, optional_size, magic;
    DWORD import_rva, import_size;
    UINT old_count = 0;
    UINT i;
    DWORD slack_rva = 0, slack_size = 0;
    UINT dll_len[2], func_len[2];
    UINT desc_bytes, cursor;
    UINT dll_off[2], ibn_off[2], int_off[2], iat_off[2];
    UINT payload_size;
    DWORD old_protect = 0;
    DWORD ignored = 0;
    IMPORT_DESCRIPTOR_* new_desc;
    DWORD* thunk;
    DWORD new_import_rva;
    DWORD new_import_size;
    DWORD directory_patch[2];
    const char* dlls[2];
    const char* funcs[2];

    /*
     * DLL 名称按调用方给出的字节串原样保存。它既可以是传统的 "Name.dll"，也可以是 dev9 使用的
     * "mods\\Name.dll"。这里绝不能只截 basename，否则就会再次迫使 Launcher 用全局 SetDllDirectoryW(mods)
     * 才能找到内部组件，重新引入 DirectDraw 兼容层所暴露出的搜索环境污染。
     */
    dlls[0] = first_dll; dlls[1] = second_dll;
    funcs[0] = first_import; funcs[1] = second_import;

    if (!process || !first_dll || !first_import || !second_dll || !second_import) return 0;
    for (i = 0u; i < 2u; ++i) {
        dll_len[i] = cstrlen_(dlls[i]);
        func_len[i] = cstrlen_(funcs[i]);
        if (!dll_len[i] || !func_len[i] || dll_len[i] > 240u || func_len[i] > 240u) return 0;
    }

    if (!get_remote_image_base_(process, &base)) return 0;

    zero_bytes_(g_headers, MAX_HEADER_BYTES_);
    if (!ReadProcessMemory(process, base, g_headers, MAX_HEADER_BYTES_, &read) || read < 512u) return 0;
    if (*(WORD*)(g_headers + 0x00u) != IMAGE_DOS_SIGNATURE_) return 0;

    pe_off = *(DWORD*)(g_headers + 0x3Cu);
    if (pe_off + 256u >= MAX_HEADER_BYTES_) return 0;
    if (*(DWORD*)(g_headers + pe_off) != IMAGE_NT_SIGNATURE_) return 0;

    section_count = *(WORD*)(g_headers + pe_off + 6u);
    optional_size = *(WORD*)(g_headers + pe_off + 20u);
    optional_off = pe_off + 24u;
    magic = *(WORD*)(g_headers + optional_off);
    if (magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC_) return 0;

    import_rva = *(DWORD*)(g_headers + optional_off + PE32_DATA_DIRECTORY_OFFSET_ + PE_IMPORT_DIRECTORY_INDEX_ * 8u);
    import_size = *(DWORD*)(g_headers + optional_off + PE32_DATA_DIRECTORY_OFFSET_ + PE_IMPORT_DIRECTORY_INDEX_ * 8u + 4u);
    if (!import_rva || import_size < (DWORD)sizeof(IMPORT_DESCRIPTOR_)) return 0;

    for (old_count = 0u; old_count < MAX_IMPORT_DESCRIPTORS_; ++old_count) {
        IMPORT_DESCRIPTOR_ d;
        if (!ReadProcessMemory(process,
                               base + import_rva + old_count * (DWORD)sizeof(IMPORT_DESCRIPTOR_),
                               &d, sizeof(d), &read) || read != sizeof(d))
            return 0;
        g_old_desc[old_count] = d;
        if (!d.OriginalFirstThunk && !d.TimeDateStamp && !d.ForwarderChain && !d.Name && !d.FirstThunk)
            break;
    }
    if (old_count == MAX_IMPORT_DESCRIPTORS_) return 0;

    /* 2 个 Loader import + 原有有效描述符 + 最后的全 0 终止项。 */
    desc_bytes = (old_count + 3u) * (UINT)sizeof(IMPORT_DESCRIPTOR_);
    cursor = align_up_(desc_bytes, 4u);

    for (i = 0u; i < 2u; ++i) {
        dll_off[i] = cursor;
        cursor += dll_len[i] + 1u;
        cursor = align_up_(cursor, 2u);

        ibn_off[i] = cursor;
        cursor += 2u + func_len[i] + 1u;
        cursor = align_up_(cursor, 4u);

        int_off[i] = cursor; cursor += 8u;
        iat_off[i] = cursor; cursor += 8u;
    }

    payload_size = align_up_(cursor, 16u);
    if (payload_size > MAX_PAYLOAD_BYTES_) return 0;

    section_off = optional_off + optional_size;
    for (i = 0u; i < section_count; ++i) {
        BYTE* sh = g_headers + section_off + i * 40u;
        DWORD virtual_size = *(DWORD*)(sh + 8u);
        DWORD virtual_address = *(DWORD*)(sh + 12u);
        DWORD raw_size = *(DWORD*)(sh + 16u);
        DWORD characteristics = *(DWORD*)(sh + 36u);
        DWORD start, available;

        if (section_off + (i + 1u) * 40u > MAX_HEADER_BYTES_) return 0;
        if (raw_size <= virtual_size) continue;
        start = align_up_(virtual_size, 16u);
        if (start >= raw_size) continue;
        available = raw_size - start;
        if (available < payload_size) continue;

        if (section_name_is_rdata_(sh) ||
            ((characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA_) && (characteristics & IMAGE_SCN_MEM_READ_))) {
            slack_rva = virtual_address + start;
            slack_size = available;
            if (section_name_is_rdata_(sh)) break;
        }
    }
    if (!slack_rva || slack_size < payload_size) return 0;

    zero_bytes_(g_payload, payload_size);
    new_desc = (IMPORT_DESCRIPTOR_*)g_payload;
    new_import_rva = slack_rva;
    new_import_size = desc_bytes;

    /*
     * 顺序是硬约束：第 0 项 LocaleBootstrap，第 1 项普通 Mod Core。
     * Bootstrap 自己不依赖 KERNEL32；Core 才依赖 KERNEL32。
     */
    for (i = 0u; i < 2u; ++i) {
        new_desc[i].OriginalFirstThunk = slack_rva + int_off[i];
        new_desc[i].TimeDateStamp = 0u;
        new_desc[i].ForwarderChain = 0u;
        new_desc[i].Name = slack_rva + dll_off[i];
        new_desc[i].FirstThunk = slack_rva + iat_off[i];
    }

    for (i = 0u; i < old_count; ++i)
        new_desc[i + 2u] = g_old_desc[i];

    for (i = 0u; i < 2u; ++i) {
        UINT j;
        for (j = 0u; j < dll_len[i]; ++j) g_payload[dll_off[i] + j] = (BYTE)dlls[i][j];
        g_payload[dll_off[i] + dll_len[i]] = 0u;

        g_payload[ibn_off[i] + 0u] = 0u;
        g_payload[ibn_off[i] + 1u] = 0u;
        for (j = 0u; j < func_len[i]; ++j) g_payload[ibn_off[i] + 2u + j] = (BYTE)funcs[i][j];
        g_payload[ibn_off[i] + 2u + func_len[i]] = 0u;

        thunk = (DWORD*)(g_payload + int_off[i]);
        thunk[0] = slack_rva + ibn_off[i];
        thunk[1] = 0u;
        thunk = (DWORD*)(g_payload + iat_off[i]);
        thunk[0] = slack_rva + ibn_off[i];
        thunk[1] = 0u;
    }

    if (!VirtualProtectEx(process, base + slack_rva, payload_size, PAGE_READWRITE_, &old_protect)) return 0;
    if (!WriteProcessMemory(process, base + slack_rva, g_payload, payload_size, &written) || written != payload_size)
        return 0;

    directory_patch[0] = new_import_rva;
    directory_patch[1] = new_import_size;
    if (!VirtualProtectEx(process,
                          base + optional_off + PE32_DATA_DIRECTORY_OFFSET_ + PE_IMPORT_DIRECTORY_INDEX_ * 8u,
                          8u, PAGE_READWRITE_, &ignored))
        return 0;
    if (!WriteProcessMemory(process,
                            base + optional_off + PE32_DATA_DIRECTORY_OFFSET_ + PE_IMPORT_DIRECTORY_INDEX_ * 8u,
                            directory_patch, 8u, &written) || written != 8u)
        return 0;
    VirtualProtectEx(process,
                     base + optional_off + PE32_DATA_DIRECTORY_OFFSET_ + PE_IMPORT_DIRECTORY_INDEX_ * 8u,
                     8u, PAGE_READONLY_, &ignored);
    FlushInstructionCache(process, base, 1u);
    return 1;
}
