/*
 * runtime_client_support.c
 *
 * C/C++ 代码在 /O2 下可能把普通的结构赋值、清零循环优化成 memset/memcpy。Runtime
 * 核心与官方 ASI 都使用 /NODEFAULTLIB，所以不能指望微软 C 运行库替它们提供这两个符号。
 * 本文件给 Runtime 和 SDK Client 共用一个最小逐字节实现，避免每个模块重复造轮子。
 *
 * 这里故意使用 volatile 字节指针并禁止函数内联。这样编译器必须真的执行下面的逐字节
 * 循环，不会把“memcpy 的实现”再次优化成对 memcpy 自己的调用而产生无限递归。
 */

#include "../include/CastleRuntime_API.h"

__declspec(noinline) void* __cdecl memset(void* destination, int value,
                                          CastleU32 count) {
    /* 把目标地址看成一个个字节，才能准确写入任意结构或数组。 */
    volatile CastleU8* output = (volatile CastleU8*)destination;
    /* 标准 memset 只使用 value 的最低 8 位，所以先明确截成一个字节。 */
    CastleU8 byte_value = (CastleU8)value;
    CastleU32 index;
    /* 从第 0 个字节写到第 count-1 个字节；count 为 0 时循环不会进入。 */
    for (index = 0u; index < count; ++index) output[index] = byte_value;
    /* 按照 memset 的约定，把调用方传入的原目标地址交还给它。 */
    return destination;
}

__declspec(noinline) void* __cdecl memcpy(void* destination, const void* source,
                                          CastleU32 count) {
    /* output 是要写入的字节序列，input 是只能读取的源字节序列。 */
    volatile CastleU8* output = (volatile CastleU8*)destination;
    const volatile CastleU8* input = (const volatile CastleU8*)source;
    CastleU32 index;
    /* memcpy 的合同要求两块区域不重叠；按从前到后的顺序复制每一个字节即可。 */
    for (index = 0u; index < count; ++index) output[index] = input[index];
    /* 与标准 memcpy 相同，返回最初的目标地址，方便编译器生成的调用继续使用。 */
    return destination;
}
