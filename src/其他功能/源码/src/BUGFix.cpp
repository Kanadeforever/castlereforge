#include "PatchUtil.h"
#include "PluginLog.h"

// ============================================================================
// 无 CRT 构建时给优化器提供的最小 memcpy / memset
// ----------------------------------------------------------------------------
// MSVC/Clang 在 /O2 下看到“小数组清零、复制”时，可能把我们写的普通循环自动优化成
// memset/memcpy 调用。BUGFix 故意使用 /NODEFAULTLIB，不链接 C Runtime；如果不提供这两个
// 极小实现，源码逻辑完全正确也会在链接阶段出现“找不到 memcpy/memset”。
//
// 这里使用 volatile 指针逐字节操作，目的是阻止编译器再次把函数自身优化回同名库调用。
// 它们只服务于本 DLL 内部的小块数组，不改变任何游戏业务状态。
// ============================================================================
extern "C" __declspec(noinline) void* __cdecl memset(void* destination, int value, SIZE_T count) {
    volatile BYTE* out = static_cast<volatile BYTE*>(destination);
    const BYTE byteValue = static_cast<BYTE>(value);
    for (SIZE_T i = 0u; i < count; ++i) {
        out[i] = byteValue;
    }
    return destination;
}

extern "C" __declspec(noinline) void* __cdecl memcpy(void* destination, const void* source, SIZE_T count) {
    volatile BYTE* out = static_cast<volatile BYTE*>(destination);
    const volatile BYTE* in = static_cast<const volatile BYTE*>(source);
    for (SIZE_T i = 0u; i < count; ++i) {
        out[i] = in[i];
    }
    return destination;
}

// ============================================================================
// BUGFix.asi  v0.3.2
// ----------------------------------------------------------------------------
// 这是《幽城幻剑录》的“必须启用的原版 BUG 修复”插件，所以故意没有 INI。
//
// 当前合并了三组已经确认的修复：
// 1. 修复“冥狱杀阵”错误进入可习得处理范围；
// 2. 修复进入菜单操作后，实际抗性与显示抗性发生一格错位；
// 3. 合并 CrashFix v0.1.0-test2 已稳定实机验证的双调用路径修复：
//    读档 -> 返回标题 -> 新游戏时，Legacy Background Controller 可能留下旧引用，
//    最终在 RPG.exe 0x004067A0 执行 `mov eax,[ecx+20h]` 时崩溃。
//
// 用户已经确认 CrashFix test2 运行稳定，因此合并时不再带入 CrashFix.ini、Enable、Log、
// NewGameBackgroundControllerCrash 等任何配置开关。三组 BUG 修复都属于“原版错误修正”，
// BUGFix.asi 被加载后就应该自动尝试安装；遇到陌生机器码时各组独立 fail-closed。
//
// 代码阅读提示：
// - “RVA”就是“相对 RPG.exe 载入基址的偏移”。例如 0x9541 表示实际地址
//   `RPG.exe基址 + 0x9541`；默认基址 0x00400000 时就是 0x00409541。
// - 前两组修复只是改固定字节；第三组需要在两条 CALL 路径前插入运行时检查，
//   所以会申请两小段 x86 stub，并用 E9 跳转把原路径临时接过去。
// - 所有写入前都先逐字节验证原始机器码；不匹配就拒绝，不“猜地址”。
// ============================================================================

namespace {

// ============================================================================
// 第一部分：两个历史固定字节 BUG 修复
// ============================================================================

// -------------------- BUG 1：冥狱杀阵习得范围 ---------------------------------
// 原版在 0x00423590 附近每次让 ESI 增加 8，然后与 0x28 比较。
// 0x28 / 8 = 5，所以原版边界只覆盖 5 组。
// 旧修复把 0x28 改成 0x30，让边界变成 6 组；这是历史三合一补丁里独立确认的改动。
const BYTE kLearnBugOriginal[] = {0x28};
const BYTE kLearnBugPatched[]  = {0x30};

// -------------------- BUG 2：抗性字段整体错位 ---------------------------------
// 角色结构中第一项抗性已经位于 +0x6C，但后六项原版从 +0x74 开始写，跳过 +0x70，
// 于是后面的真实数值整体向后错一格。只需要把六条 mov 的结构偏移各向前挪 4 字节。
const BYTE kResistance0Original[] = {0x74};
const BYTE kResistance0Patched[]  = {0x70};
const BYTE kResistance1Original[] = {0x78};
const BYTE kResistance1Patched[]  = {0x74};
const BYTE kResistance2Original[] = {0x7C};
const BYTE kResistance2Patched[]  = {0x78};
const BYTE kResistance3Original[] = {0x80};
const BYTE kResistance3Patched[]  = {0x7C};
const BYTE kResistance4Original[] = {0x84};
const BYTE kResistance4Patched[]  = {0x80};
const BYTE kResistance5Original[] = {0x88};
const BYTE kResistance5Patched[]  = {0x84};

const ycr::Patch kHistoricalBugFixPatches[] = {
    {0x00023592u, kLearnBugOriginal,       kLearnBugPatched,       1u},
    {0x00040E93u, kResistance0Original,    kResistance0Patched,    1u},
    {0x00040EB9u, kResistance1Original,    kResistance1Patched,    1u},
    {0x00040EDFu, kResistance2Original,    kResistance2Patched,    1u},
    {0x00040F05u, kResistance3Original,    kResistance3Patched,    1u},
    {0x00040F2Eu, kResistance4Original,    kResistance4Patched,    1u},
    {0x00040F57u, kResistance5Original,    kResistance5Patched,    1u},
};

// ============================================================================
// 第二部分：CrashFix test2 双调用路径修复
// ============================================================================

// 0x004067A0 是 Legacy Background Camera/Parallax Controller 的更新器。
// 崩溃现场是 ECX 已经指向失效的旧 controller，却仍执行 `mov eax,[ecx+20h]`。
constexpr DWORD kBackgroundUpdaterRva = 0x000067A0u;
constexpr SIZE_T kBackgroundControllerReadableBytes = 0x30u;

// Map runtime 内已经确认的两个字段：
// +0x04 = Legacy Background Controller*；
// +0x35 = 与该 background 生命周期/ownership 相关的启用标志。
constexpr SIZE_T kRuntimeBackgroundPointerOffset = 0x04u;
constexpr SIZE_T kRuntimeBackgroundFlagOffset = 0x35u;

// Route A：原版在进入这个 12 字节逻辑块之前已经判断 runtime+0x35 != 0。
// 因此 A 里如果 controller 为 NULL，本身就是“标志说有对象、指针却没对象”的矛盾状态。
constexpr DWORD kRouteAPatchRva  = 0x00009541u;
constexpr DWORD kRouteAResumeRva = 0x0000954Du;
constexpr SIZE_T kRouteAPatchBytes = 12u;

// Route B：原版本来就有 `test ecx,ecx / je 0x00409597`。
// 所以 B 路 controller==NULL 属于原版允许的正常分支，不应该被记为 CrashFix 命中。
constexpr DWORD kRouteBPatchRva  = 0x00009587u;
constexpr DWORD kRouteBResumeRva = 0x00009597u;
constexpr SIZE_T kRouteBPatchBytes = 16u;

// 0x004067A0 的入口签名。只用来确认我们面对的确实是已经研究过的 updater。
const BYTE kUpdaterExpected[8] = {
    0x8B, 0x41, 0x20, 0x57, 0x85, 0xC0, 0x0F, 0x8E
};

// Route A 被接管前的完整 12 字节。覆盖范围严格落在完整指令边界上。
const BYTE kRouteAExpected[kRouteAPatchBytes] = {
    0x8B,0x4E,0x04,             // mov ecx,[esi+04]
    0x6A,0x00,                  // push 0
    0x6A,0x00,                  // push 0
    0xE8,0x53,0xD2,0xFF,0xFF   // call 004067A0
};

// Route B 被接管前的完整 16 字节，其中包含原版自己的 NULL 检查。
const BYTE kRouteBExpected[kRouteBPatchBytes] = {
    0x8B,0x4E,0x04,             // mov ecx,[esi+04]
    0x85,0xC9,                  // test ecx,ecx
    0x74,0x09,                  // je 00409597
    0x6A,0x00,                  // push 0
    0x6A,0x00,                  // push 0
    0xE8,0x09,0xD2,0xFF,0xFF   // call 004067A0
};

// 每条路径都保存自己的补丁状态。original[] 用于正常 FreeLibrary 卸载时恢复原代码，
// 避免 DLL 被卸载后 RPG.exe 还继续跳向已经失效的 BUGFix helper。
struct CrashRoute {
    const char* logName;
    DWORD patchRva;
    DWORD resumeRva;
    SIZE_T patchBytes;
    const BYTE* expected;
    BYTE original[16];
    BYTE* target;
    BYTE* stub;
    bool installed;
    DWORD observedCount;
    DWORD hitCount;
};

CrashRoute gRouteA = {
    "Route A / 0x00409548", kRouteAPatchRva, kRouteAResumeRva,
    kRouteAPatchBytes, kRouteAExpected, {0}, nullptr, nullptr, false, 0u, 0u
};

CrashRoute gRouteB = {
    "Route B / 0x00409592", kRouteBPatchRva, kRouteBResumeRva,
    kRouteBPatchBytes, kRouteBExpected, {0}, nullptr, nullptr, false, 0u, 0u
};

// 判断 VirtualQuery 返回的页面权限是否允许读取。
// PAGE_GUARD 明确拒绝；低 8 位再判断普通可读/可执行可读类型。
bool IsReadableProtection(DWORD protect) {
    if ((protect & PAGE_GUARD) != 0u) {
        return false;
    }
    protect &= 0xFFu;
    return protect == PAGE_READONLY ||
           protect == PAGE_READWRITE ||
           protect == PAGE_WRITECOPY ||
           protect == PAGE_EXECUTE_READ ||
           protect == PAGE_EXECUTE_READWRITE ||
           protect == PAGE_EXECUTE_WRITECOPY;
}

// 判断页面是否允许直接写入。这里只在确认 runtime 真的可写时才清理 +0x04/+0x35，
// 不会为了修旧指针又强行 VirtualProtect 一个来源不明的 runtime 对象。
bool IsWritableProtection(DWORD protect) {
    if ((protect & PAGE_GUARD) != 0u) {
        return false;
    }
    protect &= 0xFFu;
    return protect == PAGE_READWRITE ||
           protect == PAGE_WRITECOPY ||
           protect == PAGE_EXECUTE_READWRITE ||
           protect == PAGE_EXECUTE_WRITECOPY;
}

// VirtualQuery 以“内存区域”为单位返回信息，所以不能只验证第一个字节。
// 这个函数会一路向后检查，直到 size 个字节全部处于 MEM_COMMIT + 可读页面。
bool IsReadableRange(const void* address, SIZE_T size) {
    if (address == nullptr || size == 0u) {
        return false;
    }

    const BYTE* cursor = static_cast<const BYTE*>(address);
    const BYTE* end = cursor + size;
    if (end < cursor) {
        return false; // 指针加法溢出，立即拒绝。
    }

    while (cursor < end) {
        MEMORY_BASIC_INFORMATION_MINI info{};
        if (VirtualQuery(cursor, &info, sizeof(info)) != sizeof(info)) {
            return false;
        }
        if (info.State != MEM_COMMIT || !IsReadableProtection(info.Protect)) {
            return false;
        }

        const BYTE* regionEnd = static_cast<const BYTE*>(info.BaseAddress) + info.RegionSize;
        if (regionEnd <= cursor) {
            return false;
        }
        cursor = regionEnd < end ? regionEnd : end;
    }
    return true;
}

// 与上面相同，但要求整个范围都在“当前本来就可写”的页面里。
bool IsWritableRange(void* address, SIZE_T size) {
    if (address == nullptr || size == 0u) {
        return false;
    }

    BYTE* cursor = static_cast<BYTE*>(address);
    BYTE* end = cursor + size;
    if (end < cursor) {
        return false;
    }

    while (cursor < end) {
        MEMORY_BASIC_INFORMATION_MINI info{};
        if (VirtualQuery(cursor, &info, sizeof(info)) != sizeof(info)) {
            return false;
        }
        if (info.State != MEM_COMMIT || !IsWritableProtection(info.Protect)) {
            return false;
        }

        BYTE* regionEnd = static_cast<BYTE*>(info.BaseAddress) + info.RegionSize;
        if (regionEnd <= cursor) {
            return false;
        }
        cursor = regionEnd < end ? regionEnd : end;
    }
    return true;
}

// 把“路径名 + runtime + background”写进统一 BUGFix.log。
// 这里只是诊断信息，日志写失败不会改变修复是否执行。
void LogCrashRoutePointers(const char* category, const CrashRoute& route,
                           const void* runtime, const void* background) {
    ycrlog::Text(category);
    ycrlog::Text(" ");
    ycrlog::Text(route.logName);
    ycrlog::Text("，runtime=");
    ycrlog::Hex32(static_cast<DWORD>(reinterpret_cast<SIZE_T>(runtime)));
    ycrlog::Text("，background=");
    ycrlog::Hex32(static_cast<DWORD>(reinterpret_cast<SIZE_T>(background)));
    ycrlog::Text("\r\n");
}

// 两条路径共用的 controller 校验器。
// routeBHasOriginalNullGuard=true 代表 Route B 原版自己允许 NULL，因此 NULL 时只安静返回。
void* ValidateBackgroundController(void* runtimePointer, CrashRoute& route,
                                   bool routeBHasOriginalNullGuard) {
    BYTE* runtime = static_cast<BYTE*>(runtimePointer);

    // 我们第一步只需要读取 runtime+0x04 的 4 字节指针，所以至少要求前 8 字节可读。
    if (!IsReadableRange(runtime, kRuntimeBackgroundPointerOffset + sizeof(void*))) {
        if (route.hitCount == 0u) {
            ycrlog::Line("[Crash修复] Map runtime 连 +0x04 指针都不可安全读取；本次跳过背景更新。 ");
        }
        ++route.hitCount;
        return nullptr;
    }

    void* background = *reinterpret_cast<void**>(runtime + kRuntimeBackgroundPointerOffset);

    // 每条路径第一次真正经过 helper 时记录一次，证明运行时确实走到了哪一路。
    if (route.observedCount == 0u) {
        LogCrashRoutePointers("[Crash路径] 首次观察", route, runtime, background);
    }
    ++route.observedCount;

    if (background == nullptr) {
        if (routeBHasOriginalNullGuard) {
            // Route B 原版就是 NULL -> 跳过更新，所以完全保留这个语义，不算修复命中。
            return nullptr;
        }

        ++route.hitCount;
        LogCrashRoutePointers("[Crash命中] Route A 标志启用但 controller 为 NULL；将清除矛盾状态。",
                              route, runtime, background);
    } else {
        // 非 NULL 对象必须 4 字节对齐，并且至少 0x30 字节完整可读。
        const SIZE_T addressBits = reinterpret_cast<SIZE_T>(background);
        if ((addressBits & 3u) == 0u &&
            IsReadableRange(background, kBackgroundControllerReadableBytes)) {
            return background; // 对象有效：继续完全调用原版 0x004067A0。
        }

        ++route.hitCount;
        LogCrashRoutePointers("[Crash命中] 检测到 stale Legacy Background Controller；将清旧引用。",
                              route, runtime, background);
    }

    // 确认 runtime 至少到 +0x35 都可写后，才永久清掉旧 controller 和关联 flag。
    if (IsWritableRange(runtime, kRuntimeBackgroundFlagOffset + 1u)) {
        *reinterpret_cast<void**>(runtime + kRuntimeBackgroundPointerOffset) = nullptr;
        *(runtime + kRuntimeBackgroundFlagOffset) = 0u;
        ycrlog::Line("[Crash处理] 已写 runtime+0x04=NULL、runtime+0x35=0，并跳过本次背景更新。 ");
    } else {
        ycrlog::Line("[Crash警告] runtime 当前不可写；只跳过本次背景更新，没有强行写坏对象。 ");
    }

    return nullptr;
}

// stub 使用 `push esi` 传入 Map runtime，然后用 __stdcall helper 自己清理这 4 字节参数。
// 这里必须明确 __stdcall；如果错误使用 __cdecl，ESP 会每次多留下 4 字节，最终破坏游戏栈。
void* __stdcall ValidateRouteA(void* runtimePointer) {
    return ValidateBackgroundController(runtimePointer, gRouteA, false);
}

void* __stdcall ValidateRouteB(void* runtimePointer) {
    return ValidateBackgroundController(runtimePointer, gRouteB, true);
}

// 把一条 x86 `CALL/JMP rel32` 写到我们自己刚申请的 stub 中。
// at 指向 5 字节指令开头；relative = destination - (at + 5)。
void WriteRelativeBranch(BYTE* at, BYTE opcode, const BYTE* destination) {
    at[0] = opcode;
    const DWORD next = static_cast<DWORD>(reinterpret_cast<SIZE_T>(at + 5));
    const DWORD dest = static_cast<DWORD>(reinterpret_cast<SIZE_T>(destination));
    const DWORD relative = dest - next;
    at[1] = static_cast<BYTE>((relative >> 0) & 0xFFu);
    at[2] = static_cast<BYTE>((relative >> 8) & 0xFFu);
    at[3] = static_cast<BYTE>((relative >> 16) & 0xFFu);
    at[4] = static_cast<BYTE>((relative >> 24) & 0xFFu);
}

// 两条 route 的 stub 结构相同：
//   push esi
//   call helper
//   test eax,eax
//   jz skip
//   mov ecx,eax
//   push 0
//   push 0
//   call 原版 0x004067A0
// skip:
//   jmp route resume
// helper 返回 NULL 时就跳过 updater；返回合法 controller 时完全调用原版 updater。
bool BuildRouteStub(CrashRoute& route, const BYTE* helper) {
    BYTE* exeBase = ycr::GetExeBase();
    if (exeBase == nullptr || helper == nullptr) {
        return false;
    }

    BYTE* originalUpdater = exeBase + kBackgroundUpdaterRva;
    BYTE* resume = exeBase + route.resumeRva;

    route.stub = static_cast<BYTE*>(VirtualAlloc(
        nullptr, 64u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (route.stub == nullptr) {
        return false;
    }

    SIZE_T i = 0u;
    route.stub[i++] = 0x56u; // push esi
    WriteRelativeBranch(route.stub + i, 0xE8u, helper); i += 5u;
    route.stub[i++] = 0x85u; route.stub[i++] = 0xC0u; // test eax,eax
    route.stub[i++] = 0x74u; route.stub[i++] = 0x10u; // jz 到最后一个 jmp resume
    route.stub[i++] = 0x8Bu; route.stub[i++] = 0xC8u; // mov ecx,eax
    route.stub[i++] = 0x6Au; route.stub[i++] = 0x00u; // push 0
    route.stub[i++] = 0x6Au; route.stub[i++] = 0x00u; // push 0
    WriteRelativeBranch(route.stub + i, 0xE8u, originalUpdater); i += 5u;
    WriteRelativeBranch(route.stub + i, 0xE9u, resume); i += 5u;
    WriteRelativeBranch(route.stub + i, 0xE9u, resume); i += 5u;

    FlushInstructionCache(GetCurrentProcess(), route.stub, i);
    return true;
}

void FreeRouteStub(CrashRoute& route) {
    if (route.stub != nullptr) {
        VirtualFree(route.stub, 0u, MEM_RELEASE);
        route.stub = nullptr;
    }
}

// 把原始 route 整块改成：E9 <stub>，剩余字节填 NOP。
// expected 已经在安装总流程里检查过，这里再次检查，防止“检查后到写入前”被其它插件改变。
bool InstallRouteJump(CrashRoute& route) {
    BYTE* exeBase = ycr::GetExeBase();
    if (exeBase == nullptr || route.stub == nullptr || route.patchBytes < 5u) {
        return false;
    }

    route.target = exeBase + route.patchRva;
    if (!ycr::BytesEqual(route.target, route.expected, route.patchBytes)) {
        return false;
    }

    for (SIZE_T i = 0u; i < route.patchBytes; ++i) {
        route.original[i] = route.target[i];
    }

    // 先在栈上构造完整目标字节：前 5 字节是 E9 rel32，后面全部 NOP。
    BYTE replacement[16]{};
    for (SIZE_T i = 0u; i < route.patchBytes; ++i) {
        replacement[i] = 0x90u;
    }

    // 注意：rel32 必须以“真正会执行这条 E9 的地址 route.target”为基准计算。
    // 如果错误地拿临时数组 replacement 自己的栈地址计算，复制到 RPG.exe 后跳转目标会完全错误。
    replacement[0] = 0xE9u;
    const DWORD nextInstruction =
        static_cast<DWORD>(reinterpret_cast<SIZE_T>(route.target + 5));
    const DWORD destination =
        static_cast<DWORD>(reinterpret_cast<SIZE_T>(route.stub));
    const DWORD relative = destination - nextInstruction;
    replacement[1] = static_cast<BYTE>((relative >> 0) & 0xFFu);
    replacement[2] = static_cast<BYTE>((relative >> 8) & 0xFFu);
    replacement[3] = static_cast<BYTE>((relative >> 16) & 0xFFu);
    replacement[4] = static_cast<BYTE>((relative >> 24) & 0xFFu);

    if (!ycr::WriteBytes(route.target, replacement, route.patchBytes)) {
        return false;
    }

    route.installed = true;
    return true;
}

// 正常 FreeLibrary 时恢复 CrashFix 的 E9 Hook。
// 只有当前位置仍然是“指向我们自己 stub 的 E9”才恢复，避免覆盖后加载 MOD 的修改。
void RestoreRoute(CrashRoute& route) {
    if (!route.installed || route.target == nullptr || route.stub == nullptr) {
        return;
    }

    if (route.target[0] != 0xE9u) {
        return;
    }

    const DWORD relative =
        static_cast<DWORD>(route.target[1]) |
        (static_cast<DWORD>(route.target[2]) << 8) |
        (static_cast<DWORD>(route.target[3]) << 16) |
        (static_cast<DWORD>(route.target[4]) << 24);
    BYTE* installedDestination = route.target + 5 + static_cast<long>(relative);
    if (installedDestination != route.stub) {
        return;
    }

    if (ycr::WriteBytes(route.target, route.original, route.patchBytes)) {
        route.installed = false;
    }
}

// 安装已实机稳定的 CrashFix test2。
// 三处签名必须全部匹配后才开始申请/写入；Route B 写失败时还会立即回滚 Route A。
bool InstallMergedCrashFix() {
    BYTE* exeBase = ycr::GetExeBase();
    if (exeBase == nullptr) {
        return false;
    }

    BYTE* updater = exeBase + kBackgroundUpdaterRva;
    BYTE* routeA = exeBase + kRouteAPatchRva;
    BYTE* routeB = exeBase + kRouteBPatchRva;

    if (!ycr::BytesEqual(updater, kUpdaterExpected, sizeof(kUpdaterExpected))) {
        ycrlog::Line("[Crash修复拒绝] 0x004067A0 updater 入口机器码不匹配；不安装双路径 Hook。 ");
        return false;
    }
    if (!ycr::BytesEqual(routeA, kRouteAExpected, kRouteAPatchBytes)) {
        ycrlog::Line("[Crash修复拒绝] Route A / 0x00409541 机器码不匹配；不安装双路径 Hook。 ");
        return false;
    }
    if (!ycr::BytesEqual(routeB, kRouteBExpected, kRouteBPatchBytes)) {
        ycrlog::Line("[Crash修复拒绝] Route B / 0x00409587 机器码不匹配；不安装双路径 Hook。 ");
        return false;
    }

    if (!BuildRouteStub(gRouteA, reinterpret_cast<const BYTE*>(&ValidateRouteA)) ||
        !BuildRouteStub(gRouteB, reinterpret_cast<const BYTE*>(&ValidateRouteB))) {
        ycrlog::Text("[Crash修复失败] 无法分配双路径 x86 stub，Win32错误码=");
        ycrlog::Unsigned(GetLastError());
        ycrlog::Text("\r\n");
        FreeRouteStub(gRouteA);
        FreeRouteStub(gRouteB);
        return false;
    }

    if (!InstallRouteJump(gRouteA)) {
        ycrlog::Text("[Crash修复失败] 无法写入 Route A / 0x00409541，Win32错误码=");
        ycrlog::Unsigned(GetLastError());
        ycrlog::Text("\r\n");
        FreeRouteStub(gRouteA);
        FreeRouteStub(gRouteB);
        return false;
    }

    if (!InstallRouteJump(gRouteB)) {
        ycrlog::Text("[Crash修复失败] 无法写入 Route B / 0x00409587；已回滚 Route A。Win32错误码=");
        ycrlog::Unsigned(GetLastError());
        ycrlog::Text("\r\n");
        RestoreRoute(gRouteA);
        FreeRouteStub(gRouteA);
        FreeRouteStub(gRouteB);
        return false;
    }

    ycrlog::Line("[Crash修复] 已同时接管 Route A(0x00409548) 与 Route B(0x00409592)；有效 controller 仍完全走原版 0x004067A0。 ");
    return true;
}

void UninstallMergedCrashFix() {
    // 先恢复 B 再恢复 A，顺序与安装相反，属于最普通的资源栈式清理。
    RestoreRoute(gRouteB);
    RestoreRoute(gRouteA);
    FreeRouteStub(gRouteB);
    FreeRouteStub(gRouteA);
}

} // namespace

extern "C" BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        // 不需要线程 attach/detach 通知。关闭它们可以减少 DllMain 额外调用。
        DisableThreadLibraryCalls(module);

        // BUGFix.log 每次启动都 CREATE_ALWAYS 清空；正文 UTF-8 BOM + CRLF + 简体中文。
        // 即使日志打开失败，下面修复仍继续执行，日志永远不是核心功能前置条件。
        ycrlog::Open(module, L"BUGFix.log");
        ycrlog::Line("《幽城幻剑录》BUG 修复插件 v0.3.2 启动。");
        ycrlog::Line("ASI插件化 By Luminous with ChatGPT");
        ycrlog::Line("BUG修复来自“汉堂之家”坛友武英仲分享的三合一补丁");
        ycrlog::Line("新增win11上出现的读档后返回标题再开始新游戏必定崩溃的修复");
        ycrlog::Line("[日志] 本次启动已清空旧日志；编码=UTF-8，换行=CRLF。");

        // 第一组：历史两个固定字节 BUG 修复。
        const bool historicalApplied = ycr::SetPatchSetState(
            kHistoricalBugFixPatches,
            sizeof(kHistoricalBugFixPatches) / sizeof(kHistoricalBugFixPatches[0]),
            true);

        if (historicalApplied) {
            ycrlog::Line("[修复] ‘冥狱杀阵可被习得’修复：已生效。");
            ycrlog::Line("[修复] 菜单操作后实际抗性与显示抗性不一致修复：已生效。");
        } else {
            ycrlog::Line("[失败] 历史 7 个固定补丁点出现未知机器码；两个固定 BUG 修复均未盲目写入。");
        }

        // 第二组：原 CrashFix test2。它有自己的三重签名和双路径回滚，不受上面结果影响。
        if (InstallMergedCrashFix()) {
            ycrlog::Line("[修复] 读档返回标题后再新游戏的 Legacy Background Controller 崩溃修复：已生效。");
        } else {
            ycrlog::Line("[失败] Crash 双调用路径修复未安装；保持 fail-closed，不影响其它已确认 BUG 修复。");
        }
    } else if (reason == DLL_PROCESS_DETACH) {
        // reserved==nullptr 表示正常 FreeLibrary：这时 DLL 代码真的可能被卸载，所以必须先把
        // 指向 DLL helper/stub 的 E9 Hook 恢复，避免 RPG.exe 之后跳进已经不存在的代码。
        // reserved!=nullptr 表示整个进程正在终止，整个地址空间马上销毁，不再修改代码页。
        if (reserved == nullptr) {
            UninstallMergedCrashFix();
            ycrlog::Line("[退出] BUGFix 正常卸载；Crash 双路径 Hook 已尝试恢复，固定字节 BUG 修复保留到进程结束。");
        } else {
            ycrlog::Line("[退出] 游戏进程正在结束；不做没有意义的 Hook 回写。");
        }
        ycrlog::Close();
    }

    return TRUE;
}
