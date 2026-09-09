#pragma once

#include "Win32Mini.h"

// ============================================================================
// PatchUtil.h
// ----------------------------------------------------------------------------
// 三个 Extra 插件现在只从这里取得补丁描述、RPG.exe 基址和只读字节比较。
// 所有正式游戏写入都交给 Runtime Hook 事务；本文件不再提供 VirtualProtect、手工 CALL 或
// 批量写入函数，避免后续代码无意恢复未协调的独立补丁路径。
// ============================================================================

namespace ycr {

struct Patch {
    DWORD rva;            // 相对 RPG.exe ImageBase 的位置。
    const BYTE* original; // 安装前允许的原版字节。
    const BYTE* patched;  // Runtime 事务提交后要求的启用字节。
    SIZE_T size;          // 两种状态共同的字节长度。
};

inline BYTE* GetExeBase() {
    // nullptr 表示当前主程序；在 ASI 所在进程中就是 RPG.exe。
    return reinterpret_cast<BYTE*>(GetModuleHandleW(nullptr));
}

inline bool BytesEqual(const BYTE* left, const BYTE* right, SIZE_T size) {
    if (left == nullptr || right == nullptr || size == 0u) {
        return false;
    }
    // 逐字节比较避免无 CRT 构建产生外部 memcmp 导入。
    for (SIZE_T i = 0u; i < size; ++i) {
        if (left[i] != right[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace ycr
