#include "platform.h"
#include "entry_gate.h"

/*
 * 这份模块只服务已经确认的台湾第三版 RPG.exe。
 * 精确入口 RVA 与入口前 5 字节来自当前项目固定 RPG.exe 的 PE/反汇编证据。
 *
 * 0x00452C19:
 *   55       push ebp
 *   8B EC    mov  ebp, esp
 *   6A FF    push -1
 *
 * 三条指令刚好 5 字节，所以可以安全放一个 x86 E9 rel32 JMP，而不会把一条指令劈成两半。
 * Gate 运行后会先恢复这 5 个原字节，再进入任何 Mod 初始化；因此后续 ASI 做精确机器码预检时看到的是原始游戏代码。
 */
#define RPG_ENTRY_RVA_ 0x00052C19u
#define ENTRY_PATCH_SIZE_ 5u

static BYTE g_original[ENTRY_PATCH_SIZE_] = {0x55u, 0x8Bu, 0xECu, 0x6Au, 0xFFu};
static BYTE* g_entry;
static BYTE* g_entry_after;
static int g_installed;

/* core.c 提供这个函数。这里前置声明，避免 entry_gate.c 反过来包含 Core 私有实现。 */
int __cdecl CastleModCore_Initialize(void);

/*
 * 失败时不能“勉强继续游戏”：如果 Core 已经被插入装载图却无法安全建立入口门，
 * RPG.exe 可能处于半初始化 Mod 环境。直接 ExitProcess 比留下随机状态更容易诊断，也更安全。
 */
static void __declspec(noreturn) fail_exit_(void) {
    ExitProcess(4u);
    for (;;) { /* ExitProcess 按 Win32 约定不返回；这个死循环只用于让编译器明确知道控制流终止。 */ }
}

/*
 * x86 裸函数不能让编译器自动生成函数序言，否则我们就无法精确重放 RPG.exe 被覆盖的三条入口指令。
 * pushfd/pushad 先保存 Windows Loader 交给 EXE EntryPoint 的寄存器/EFLAGS；完整初始化返回后再恢复。
 */
__declspec(naked) static void EntryGate_Thunk_(void) {
    __asm {
        pushfd
        pushad
        call CastleModCore_Initialize
        test eax, eax
        jnz gate_ok
        call fail_exit_
    gate_ok:
        popad
        popfd

        /* 精确重放原 RPG.exe 前 5 字节。 */
        push ebp
        mov ebp, esp
        push 0FFFFFFFFh
        jmp dword ptr [g_entry_after]
    }
}

static int bytes_equal_(const BYTE* a, const BYTE* b, UINT count) {
    UINT i;
    for (i = 0u; i < count; ++i) if (a[i] != b[i]) return 0;
    return 1;
}

int EntryGate_Install(void) {
    BYTE patch[ENTRY_PATCH_SIZE_];
    BYTE* base = (BYTE*)GetModuleHandleW(NULL_PTR);
    DWORD oldp = 0u, ignored = 0u;
    LONG rel;

    if (g_installed) return 1;
    if (!base) return 0;

    g_entry = base + RPG_ENTRY_RVA_;
    g_entry_after = g_entry + ENTRY_PATCH_SIZE_;

    /* 精确版本保护：入口不是已确认的三条指令就绝不写。 */
    if (!bytes_equal_(g_entry, g_original, ENTRY_PATCH_SIZE_)) return 0;

    rel = (LONG)((BYTE*)&EntryGate_Thunk_ - (g_entry + ENTRY_PATCH_SIZE_));
    patch[0] = 0xE9u;
    /*
     * x86 rel32 在 PE/CPU 中按小端字节保存。逐字节写不仅避免未对齐 LONG 指针，
     * 也让静态分析器可以明确证明 patch[1..4] 每一项都已初始化。
     */
    patch[1] = (BYTE)((DWORD)rel & 0xFFu);
    patch[2] = (BYTE)(((DWORD)rel >> 8) & 0xFFu);
    patch[3] = (BYTE)(((DWORD)rel >> 16) & 0xFFu);
    patch[4] = (BYTE)(((DWORD)rel >> 24) & 0xFFu);

    if (!VirtualProtect(g_entry, ENTRY_PATCH_SIZE_, PAGE_EXECUTE_READWRITE_, &oldp)) return 0;
    {
        UINT i;
        for (i = 0u; i < ENTRY_PATCH_SIZE_; ++i) g_entry[i] = patch[i];
    }
    FlushInstructionCache(GetCurrentProcess(), g_entry, ENTRY_PATCH_SIZE_);
    VirtualProtect(g_entry, ENTRY_PATCH_SIZE_, oldp, &ignored);
    g_installed = 1;
    return 1;
}

int EntryGate_Restore(void) {
    DWORD oldp = 0u, ignored = 0u;
    UINT i;
    if (!g_installed) return 1;
    if (!g_entry) return 0;
    if (!VirtualProtect(g_entry, ENTRY_PATCH_SIZE_, PAGE_EXECUTE_READWRITE_, &oldp)) return 0;
    for (i = 0u; i < ENTRY_PATCH_SIZE_; ++i) g_entry[i] = g_original[i];
    FlushInstructionCache(GetCurrentProcess(), g_entry, ENTRY_PATCH_SIZE_);
    VirtualProtect(g_entry, ENTRY_PATCH_SIZE_, oldp, &ignored);
    g_installed = 0;
    return 1;
}

void EntryGate_FinalizeInjectedImportPage(void) {
    BYTE* base = (BYTE*)GetModuleHandleW(NULL_PTR);
    DWORD pe_off, optional_off, import_rva;
    BYTE* page;
    DWORD oldp = 0u, ignored = 0u;

    if (!base || *(WORD*)base != IMAGE_DOS_SIGNATURE_) return;
    pe_off = *(DWORD*)(base + 0x3Cu);
    if (*(DWORD*)(base + pe_off) != IMAGE_NT_SIGNATURE_) return;
    optional_off = pe_off + 24u;
    if (*(WORD*)(base + optional_off) != IMAGE_NT_OPTIONAL_HDR32_MAGIC_) return;

    import_rva = *(DWORD*)(base + optional_off + 96u + IMAGE_DIRECTORY_ENTRY_IMPORT_ * 8u);
    if (!import_rva) return;

    /*
     * Launcher 为了让 Windows Loader 写入新 IAT，把承载新 Import Directory 的 .rdata 尾页临时设成可写。
     * 到 EntryPoint 时装载器已经解析完导入，可以把这页恢复为只读，避免长期扩大可写代码/元数据面积。
     */
    page = base + (import_rva & ~0xFFFu);
    if (VirtualProtect(page, 0x1000u, PAGE_READONLY_, &oldp)) {
        VirtualProtect(page, 0x1000u, PAGE_READONLY_, &ignored);
    }
}
