#ifndef CASTLE_SAVE_ENHANCE_PATCH_UTIL_H
#define CASTLE_SAVE_ENHANCE_PATCH_UTIL_H

#include "Win32Mini.h"

// ============================================================================
// PatchUtil.h
// ----------------------------------------------------------------------------
// 这是 Castle_SaveEnhance 自己携带的最小“机器码补丁工具”。
//
// 老游戏插件经常需要把 RPG.exe 某几条指令换成 CALL Hook 或新的常量。真正危险的地方
// 不是“写 5 个字节”本身，而是：如果游戏版本不对、别的 MOD 已经改过、或者只改了一半，
// 我们就可能把 CPU 引到错误位置。因此这个工具坚持三条规则：
//
// 1. 写之前逐字节确认“现在正是我们认识的原版字节”；
// 2. 用 VirtualProtect 临时开放写权限，写完恢复保护并刷新 CPU 指令缓存；
// 3. 一组互相依赖的补丁按事务处理：中途失败就尽量恢复调用前的真实字节。
//
// 对初学者来说，可以把 PatchSet 想成“同时改八个螺丝”。八个缺一个机器就不完整，所以
// 不能改完前三个、第四个失败后就装作没事继续运行。
// ============================================================================

namespace ycr {

struct Patch {
    DWORD rva;           // 相对 RPG.exe ImageBase 的位置。
    const BYTE* original;// 这个版本原来必须是什么字节。
    const BYTE* patched; // 成功后希望写成什么字节。
    SIZE_T size;         // 这一项一共多少字节。
};

inline BYTE* GetExeBase() {
    // 传 nullptr 给 GetModuleHandleW，Windows 返回“当前 EXE”，这里就是 RPG.exe。
    return reinterpret_cast<BYTE*>(GetModuleHandleW(nullptr));
}

inline bool BytesEqual(const BYTE* actual, const BYTE* expected, SIZE_T size) {
    if (actual == nullptr || expected == nullptr || size == 0u) {
        return false;
    }
    for (SIZE_T i = 0u; i < size; ++i) {
        if (actual[i] != expected[i]) {
            return false;
        }
    }
    return true;
}

inline void CopyRaw(BYTE* out, const BYTE* in, SIZE_T size) {
    // 逐字节复制，避免无 CRT 构建时优化器偷偷生成外部 memcpy 依赖。
    for (SIZE_T i = 0u; i < size; ++i) {
        out[i] = in[i];
    }
}

inline bool WriteBytes(BYTE* address, const BYTE* bytes, SIZE_T size) {
    if (address == nullptr || bytes == nullptr || size == 0u) {
        return false;
    }

    DWORD oldProtect = 0u;
    // 代码页通常不可直接写。先暂时切到 EXECUTE_READWRITE。
    if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }

    for (SIZE_T i = 0u; i < size; ++i) {
        address[i] = bytes[i];
    }

    // 写完先刷新 CPU 指令缓存。即使内存字节已经改变，如果 CPU 还缓存旧指令也会出问题。
    const BOOL flushed = FlushInstructionCache(GetCurrentProcess(), address, size);

    DWORD ignored = 0u;
    const BOOL restored = VirtualProtect(address, size, oldProtect, &ignored);
    return flushed != FALSE && restored != FALSE;
}

inline bool ValidatePatchSet(BYTE* exeBase, const Patch* patches, SIZE_T count) {
    if (exeBase == nullptr || patches == nullptr || count == 0u) {
        return false;
    }
    for (SIZE_T i = 0u; i < count; ++i) {
        const Patch& patch = patches[i];
        if (patch.original == nullptr || patch.patched == nullptr || patch.size == 0u ||
            !BytesEqual(exeBase + patch.rva, patch.original, patch.size)) {
            return false;
        }
    }
    return true;
}

inline bool ApplyPatchSet(BYTE* exeBase, const Patch* patches, SIZE_T count) {
    // SaveEnhance 当前一组最多十几项，小固定缓冲足够，避免 heap/CRT。
    if (exeBase == nullptr || patches == nullptr || count == 0u || count > 32u) {
        return false;
    }

    struct Backup {
        BYTE bytes[16];
        SIZE_T size;
    } backups[32];

    // 单项最大 16 字节。超出说明以后有人改了结构，应该扩工具而不是悄悄截断。
    for (SIZE_T i = 0u; i < count; ++i) {
        if (patches[i].size == 0u || patches[i].size > 16u) {
            return false;
        }
        backups[i].size = patches[i].size;
        CopyRaw(backups[i].bytes, exeBase + patches[i].rva, patches[i].size);
    }

    // 必须先把“全部项”都验证完，再写第一项，避免版本不匹配造成半安装。
    if (!ValidatePatchSet(exeBase, patches, count)) {
        return false;
    }

    for (SIZE_T i = 0u; i < count; ++i) {
        if (WriteBytes(exeBase + patches[i].rva, patches[i].patched, patches[i].size)) {
            continue;
        }

        // 第 i 项虽然返回失败，也可能已经写了字节、只是 FlushInstructionCache 失败。
        // 因此回滚范围必须包含 i 自己，而不是只回滚 0..i-1。
        SIZE_T rollback = i + 1u;
        while (rollback > 0u) {
            --rollback;
            WriteBytes(exeBase + patches[rollback].rva, backups[rollback].bytes, backups[rollback].size);
        }
        return false;
    }
    return true;
}

inline bool RestorePatchSetToOriginal(BYTE* exeBase, const Patch* patches, SIZE_T count) {
    if (exeBase == nullptr || patches == nullptr) {
        return false;
    }
    bool ok = true;
    for (SIZE_T i = 0u; i < count; ++i) {
        if (!WriteBytes(exeBase + patches[i].rva, patches[i].original, patches[i].size)) {
            ok = false;
        }
    }
    return ok;
}

inline bool InstallRelativeCall(
    BYTE* exeBase,
    DWORD rva,
    const BYTE expected[5],
    const void* target) {
    if (exeBase == nullptr || expected == nullptr || target == nullptr) {
        return false;
    }

    BYTE* site = exeBase + rva;
    if (!BytesEqual(site, expected, 5u)) {
        return false;
    }

    BYTE patch[5];
    patch[0] = 0xE8u; // x86 E8 = CALL rel32。
    const SIZE_T nextInstruction = reinterpret_cast<SIZE_T>(site + 5u);
    const SIZE_T destination = reinterpret_cast<SIZE_T>(target);
    const DWORD displacement = static_cast<DWORD>(destination - nextInstruction);
    *reinterpret_cast<DWORD*>(patch + 1u) = displacement;
    return WriteBytes(site, patch, 5u);
}

inline bool InstallRelativeCall6(
    BYTE* exeBase,
    DWORD rva,
    const BYTE expected[6],
    const void* target) {
    // 有些原版指令正好 6 字节。我们用 5 字节 CALL 替换，再用 1 个 NOP 补齐长度，
    // 这样后面的原版指令地址完全不移动。
    if (exeBase == nullptr || expected == nullptr || target == nullptr) {
        return false;
    }
    BYTE* site = exeBase + rva;
    if (!BytesEqual(site, expected, 6u)) {
        return false;
    }
    BYTE patch[6];
    patch[0] = 0xE8u;
    const SIZE_T nextInstruction = reinterpret_cast<SIZE_T>(site + 5u);
    const SIZE_T destination = reinterpret_cast<SIZE_T>(target);
    *reinterpret_cast<DWORD*>(patch + 1u) = static_cast<DWORD>(destination - nextInstruction);
    patch[5] = 0x90u;
    return WriteBytes(site, patch, 6u);
}

} // namespace ycr

#endif // CASTLE_SAVE_ENHANCE_PATCH_UTIL_H
