#pragma once

#include "Win32Mini.h"

// ============================================================================
// PatchUtil.h  v0.3.1a
// ----------------------------------------------------------------------------
// 这是正式插件与诊断插件共用的“内存补丁小工具”。
//
// 可以把 RPG.exe 想成一本已经印好的书：
// - RVA 就像“从书的起点往后数多少字节”；
// - original/expected 是我们预期那里原本印着的内容；
// - patched/replacement 是插件想临时改成的内容。
//
// v0.3.1a 继续保留固定字节补丁与“安全替换 x86 CALL rel32”两套能力。
// 这是 Safe AnytimeSave 所需要的：目标函数在 ASI 被 Windows 装入后地址才确定，
// 所以 CALL 后面的 4 字节相对距离必须运行时计算，不能提前写成固定常量。
//
// 无论哪一种补丁，都遵守同一条最高安全规则：
// **写之前先验证当前位置。发现不是已知机器码就拒绝写，而不是赌地址没有变。**
// ============================================================================

namespace ycr {

// 一个固定字节补丁描述。
struct Patch {
    DWORD rva;
    const BYTE* original;
    const BYTE* patched;
    SIZE_T size;
};

// 比较两段字节是否完全一样。
// 不调用 C 运行库 memcmp，这样交叉构建时最终 ASI 可以继续只依赖 kernel32.dll。
inline bool BytesEqual(const BYTE* left, const BYTE* right, SIZE_T size) {
    for (SIZE_T i = 0; i < size; ++i) {
        if (left[i] != right[i]) {
            return false;
        }
    }
    return true;
}

// 获取主程序 RPG.exe 的实际加载基址。
inline BYTE* GetExeBase() {
    return reinterpret_cast<BYTE*>(GetModuleHandleW(nullptr));
}

// ---------------------------------------------------------------------------
// 最底层的“已经决定写什么之后，安全写入”函数。
// ---------------------------------------------------------------------------
// target      = 真正要修改的内存地址；
// desired     = 要写进去的新字节；
// size        = 字节数。
//
// 这个函数不负责判断版本身份；身份检查必须由上层先完成。
inline bool WriteBytes(BYTE* target, const BYTE* desired, SIZE_T size) {
    // 如果当前位置本来已经等于目标字节，不重复修改页面权限。
    if (BytesEqual(target, desired, size)) {
        return true;
    }

    // RPG.exe 的 .text 通常是“可执行+可读、不可写”。
    // VirtualProtect 暂时改成可读写执行，oldProtect 记录原权限以便稍后恢复。
    DWORD oldProtect = 0;
    if (!VirtualProtect(target, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }

    // 不使用 memcpy，一个字节一个字节复制，避免额外 C 运行库依赖。
    for (SIZE_T i = 0; i < size; ++i) {
        target[i] = desired[i];
    }

    // 恢复代码页原权限。restoreProtect 只是接收 API 要求的输出值。
    DWORD restoreProtect = 0;
    VirtualProtect(target, size, oldProtect, &restoreProtect);

    // CPU 可能缓存过旧指令。刷新指令缓存可以保证后续取到刚写入的新机器码。
    return FlushInstructionCache(GetCurrentProcess(), target, size) != FALSE;
}

// 检查固定补丁位置是不是我们认识的原版或已补丁状态。
inline bool IsPatchStateRecognized(const Patch& patch) {
    BYTE* base = GetExeBase();
    if (base == nullptr) {
        return false;
    }

    const BYTE* current = base + patch.rva;
    return BytesEqual(current, patch.original, patch.size) ||
           BytesEqual(current, patch.patched, patch.size);
}

// 按 enabled 选择“目标补丁字节”或“恢复原始字节”，然后写入。
inline bool WritePatchBytes(const Patch& patch, bool enabled) {
    BYTE* base = GetExeBase();
    if (base == nullptr) {
        return false;
    }

    BYTE* target = base + patch.rva;
    const BYTE* desired = enabled ? patch.patched : patch.original;
    return WriteBytes(target, desired, patch.size);
}

// 一次性检查一整组补丁。
// 先全检查、后全写，避免第三个地址不对时前两个已经改掉的“半成功”状态。
inline bool ValidatePatchSet(const Patch* patches, SIZE_T count) {
    for (SIZE_T i = 0; i < count; ++i) {
        if (!IsPatchStateRecognized(patches[i])) {
            return false;
        }
    }
    return true;
}

// 统一打开或关闭一整组固定字节补丁。
inline bool SetPatchSetState(const Patch* patches, SIZE_T count, bool enabled) {
    if (!ValidatePatchSet(patches, count)) {
        return false;
    }

    for (SIZE_T i = 0; i < count; ++i) {
        if (!WritePatchBytes(patches[i], enabled)) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// v0.2.0 新增：安全安装 x86 `CALL rel32`
// ---------------------------------------------------------------------------
// x86 的近 CALL 机器码是：
//   E8 xx xx xx xx
// 其中后 4 字节不是目标绝对地址，而是：
//   target_address - address_after_call
//
// 例如 CALL 本身位于 A，长度 5 字节，那么 CPU 实际跳到：
//   (A + 5) + signed_rel32
//
// ASI 每次装入地址可能不同，所以这 4 字节必须等 Windows 把 ASI 装好后再计算。
inline bool InstallRelativeCall(
    DWORD callRva,
    const BYTE expectedOriginal[5],
    const void* replacementFunction) {

    BYTE* base = GetExeBase();
    if (base == nullptr || replacementFunction == nullptr) {
        return false;
    }

    BYTE* callSite = base + callRva;

    // 构造新的 5 字节 CALL。
    BYTE desired[5];
    desired[0] = 0xE8;

    // 这里整个程序是 32 位，所以指针和 DWORD 都是 32 位。
    // 先得到“CALL 下一条指令”的地址，再做 32 位减法，就得到 x86 rel32 位模式。
    const DWORD nextInstruction =
        static_cast<DWORD>(reinterpret_cast<SIZE_T>(callSite + 5));
    const DWORD destination =
        static_cast<DWORD>(reinterpret_cast<SIZE_T>(replacementFunction));
    const DWORD relative = destination - nextInstruction;

    // 小端序机器上最低有效字节放最前面。
    desired[1] = static_cast<BYTE>((relative >> 0) & 0xFFu);
    desired[2] = static_cast<BYTE>((relative >> 8) & 0xFFu);
    desired[3] = static_cast<BYTE>((relative >> 16) & 0xFFu);
    desired[4] = static_cast<BYTE>((relative >> 24) & 0xFFu);

    // 如果当前位置已经正好是指向本函数的 CALL，说明可能发生重复装载；直接视为成功。
    if (BytesEqual(callSite, desired, 5u)) {
        return true;
    }

    // 只有当前位置仍是我们确认过的原始 CALL 才允许第一次覆盖。
    // 如果别的 MOD、不同版本 EXE 或旧实验已经改了这 5 字节，就拒绝争抢 Hook 点。
    if (!BytesEqual(callSite, expectedOriginal, 5u)) {
        return false;
    }

    return WriteBytes(callSite, desired, 5u);
}

// 计算 UTF-16 字符串长度，不包含末尾 L'\0'。
// MaxGrowthAndDrop 自动写默认 INI 时使用；继续避免依赖 wcslen。
inline SIZE_T WideLength(const wchar_t* text) {
    SIZE_T length = 0;
    while (text[length] != L'\0') {
        ++length;
    }
    return length;
}

}  // namespace ycr
