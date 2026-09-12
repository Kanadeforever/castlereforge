#ifndef CASTLE_WIDESCREEN_PLATFORM_H
#define CASTLE_WIDESCREEN_PLATFORM_H

/*
 * platform.h
 *
 * 目标 RPG.exe 是 32 位旧式 Windows 程序。本 ASI 只需要很少的 Win32 API，
 * 所以这里自己声明最小类型，不包含 windows.h，也不链接 C 运行库。
 *
 * 对初学者来说可以把这个文件理解成“翻译表”：
 * C 编译器本来不知道 HANDLE、HMODULE、VirtualAlloc 是什么；
 * 我们在这里告诉它这些名字的大小、调用方式和参数，让其他源码可以安全调用 Windows。
 */

#define NULL ((void*)0)

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed int     i32;
typedef unsigned long  DWORD;
typedef unsigned long  SIZE_T;
typedef int            BOOL;
typedef void*           HANDLE;
typedef void*           HMODULE;
typedef void*           FARPROC;

/* Win32 POINT 与 GetCursorPos；宽屏只需要两个有符号屏幕坐标，不包含完整 windows.h。 */
typedef struct Point32 {
    i32 x;
    i32 y;
} Point32;

/*
 * Windows 32 位程序存在多种调用约定。
 * 调用约定决定“参数从哪里传、谁清理栈”；写错通常不是小 bug，而是函数返回时直接崩溃。
 */
#ifdef _MSC_VER
#define WINAPI   __stdcall
#define CDECL    __cdecl
#define FASTCALL __fastcall
#define THISCALL __thiscall
#else
#define WINAPI   __attribute__((stdcall))
#define CDECL    __attribute__((cdecl))
#define FASTCALL __attribute__((fastcall))
#define THISCALL __attribute__((thiscall))
#endif

#define TRUE  1
#define FALSE 0

/* VirtualAlloc：RESERVE 先占地址空间，COMMIT 再让这段内存真正可读写。 */
#define MEM_COMMIT_     0x00001000u
#define MEM_RESERVE_    0x00002000u
#define PAGE_READWRITE_ 0x04u

/* DLL 入口只关心“整个进程加载 DLL”和“整个进程卸载 DLL”。 */
#define DLL_PROCESS_DETACH_ 0u
#define DLL_PROCESS_ATTACH_ 1u

/* 下面每个 typedef 都是在描述一个真实 Win32 API 函数指针的完整签名。 */
typedef HMODULE (WINAPI *PFN_GetModuleHandleA)(const char* name);
typedef FARPROC (WINAPI *PFN_GetProcAddress)(HMODULE module, const char* name);
typedef void*   (WINAPI *PFN_VirtualAlloc)(void*, SIZE_T, DWORD, DWORD);

/*
 * GetTickCount 返回 Windows 启动以来经过的毫秒数。
 * 它是 32 位计数器，大约 49.7 天会回绕一次；只要用无符号减法计算“两个相邻时间点的差”，
 * 回绕也不会破坏本插件只有几百毫秒的过渡动画。
 */
typedef DWORD   (WINAPI *PFN_GetTickCount)(void);
typedef BOOL    (WINAPI *PFN_GetCursorPos)(Point32* point);
/* USER32 鼠标键轮询返回有符号16位；SetCursorPos 返回32位BOOL，调用约定均为stdcall。 */
typedef short   (WINAPI *PFN_KeyState)(int key);
typedef BOOL    (WINAPI *PFN_SetCursorPos)(i32 x, i32 y);

/*
 * 0x405BD0、0x405A10、0x434710 都是“ECX 里传 this、没有显式栈参数”的成员函数。
 * 因而统一用 thiscall 类型调用原入口；我们的 CALL Hook 本身用 fastcall 接住 ECX，并额外吃掉 EDX 占位。
 */
typedef void (THISCALL *PFN_ThisVoid)(void* self);

/*
 * Bink 1.x 的 _BinkCopyToBuffer@28：7 个参数、stdcall。
 * Hook 只改变 dest_x，并在有足够 pitch 时清左右黑边；返回值原样透传。
 */
typedef i32 (WINAPI *PFN_BinkCopyToBuffer)(
    void* bink, void* dest_pixels, i32 dest_pitch, u32 dest_height,
    u32 dest_x, u32 dest_y, u32 flags);

#endif /* CASTLE_WIDESCREEN_PLATFORM_H */
