/*
 * runtime_client_support.c
 *
 * C++ 插件在 /O2 下可能把结构零初始化和小块复制优化成 memset/memcpy。官方 ASI
 * 均使用 /NODEFAULTLIB，所以 SDK Client 提供最小逐字节实现，避免每个插件重复造轮子。
 * volatile 阻止编译器把实现本身再次折叠成同名 CRT 调用。
 */

#include "../include/CastleRuntime_API.h"

__declspec(noinline) void* __cdecl memset(void* destination, int value,
                                          CastleU32 count) {
    volatile CastleU8* output = (volatile CastleU8*)destination;
    CastleU8 byte_value = (CastleU8)value;
    CastleU32 index;
    for (index = 0u; index < count; ++index) output[index] = byte_value;
    return destination;
}

__declspec(noinline) void* __cdecl memcpy(void* destination, const void* source,
                                          CastleU32 count) {
    volatile CastleU8* output = (volatile CastleU8*)destination;
    const volatile CastleU8* input = (const volatile CastleU8*)source;
    CastleU32 index;
    for (index = 0u; index < count; ++index) output[index] = input[index];
    return destination;
}
