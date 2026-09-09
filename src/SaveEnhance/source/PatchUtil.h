#ifndef CASTLE_SAVE_ENHANCE_PATCH_UTIL_H
#define CASTLE_SAVE_ENHANCE_PATCH_UTIL_H

#include "Win32Mini.h"

// ============================================================================
// PatchUtil.h
// ----------------------------------------------------------------------------
// SaveEnhance 仍需要“补丁由哪些字节组成”和“当前字节是否匹配”的只读工具。
// 真正写入 RPG.exe 的工作已经全部迁给 Runtime Hook 事务，因此本文件不再提供 VirtualProtect、
// 写 CALL 或手工回滚函数。这样以后维护者不会误把旧独立安装路径重新接回官方插件。
// ============================================================================

namespace ycr {

struct Patch {
    DWORD rva;            // 相对 RPG.exe ImageBase 的位置，由 Runtime 与预检查共同定位。
    const BYTE* original; // 这个游戏版本在安装前必须出现的原版字节。
    const BYTE* patched;  // 事务提交后希望得到的已启用字节。
    SIZE_T size;          // 这一项的字节数；Runtime 会要求原版态和启用态长度完全一致。
};

inline BYTE* GetExeBase() {
    // 传 nullptr 给 GetModuleHandleW 会返回当前 EXE，在本插件进程中就是 RPG.exe。
    // 这里仅取得模块基址，不改变页面保护，也不修改任何游戏数据。
    return reinterpret_cast<BYTE*>(GetModuleHandleW(nullptr));
}

inline bool BytesEqual(const BYTE* actual, const BYTE* expected, SIZE_T size) {
    // 空地址和零长度都不是有效机器码证据，直接判失败，调用者随后保持 fail-closed。
    if (actual == nullptr || expected == nullptr || size == 0u) {
        return false;
    }

    // 逐字节比较可以避免无 CRT 构建生成外部 memcmp 依赖；任意一个字节不同都说明状态未知。
    for (SIZE_T i = 0u; i < size; ++i) {
        if (actual[i] != expected[i]) {
            return false;
        }
    }
    return true;
}

} // namespace ycr

#endif // CASTLE_SAVE_ENHANCE_PATCH_UTIL_H
