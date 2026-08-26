#ifndef CASTLE_BACKLOG_PLATFORM_H
#define CASTLE_BACKLOG_PLATFORM_H

/*
 * platform.h
 *
 * 这是 Backlog 插件最底层的“Windows 与 32 位游戏约定”文件。
 *
 * 初学者可以把头文件理解为一本小字典：别的 .c 文件只要包含这本字典，
 * 就能使用这里统一规定的整数类型、函数调用方式和常量，而不必每个文件重复写一遍。
 *
 * 本插件是给 2002 年的 32 位 RPG.exe 使用的，所以构建目标必须是 x86。
 * windows.h 提供 Windows API 的正式声明；我们仍然不使用 C 运行库，避免给老游戏
 * 增加 msvcrt/ucrt 版本依赖。字符串复制、比较和清零都在项目代码里用简单循环完成。
 */

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

/*
 * 给常用的定长整数起短名字。
 * u8 是 0..255 的一个字节；u32 是 32 位无符号整数；i32 是 32 位有符号整数。
 * 这些类型的大小在 x86 Windows 上固定，适合精确描述 RPG.exe 的内存字段。
 */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed short   i16;
typedef signed int     i32;

/*
 * RPG.exe 是 32 位程序，内部函数并不全用同一种传参方法。
 * CDECL 表示参数由调用者清理；THISCALL 表示对象指针放在 ECX 寄存器中。
 * 如果这里写错，即使函数地址正确，返回时也可能破坏栈并让游戏崩溃。
 */
#define BACKLOG_CDECL    __cdecl
#define BACKLOG_STDCALL  __stdcall
#define BACKLOG_THISCALL __thiscall

/* Backlog 工作线程每 8 毫秒采样一次键盘，并读取鼠标/PadSupport 已经准备好的输入状态。 */
#define BACKLOG_WORKER_SLEEP_MS 8u

#endif /* CASTLE_BACKLOG_PLATFORM_H */
