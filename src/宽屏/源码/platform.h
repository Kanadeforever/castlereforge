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

/* VirtualProtect：只有改 RPG.exe 的 E8 CALL / IAT 槽那几字节时才临时开放“可执行+可写”。 */
#define PAGE_EXECUTE_READWRITE_ 0x40u

/* VirtualAlloc：RESERVE 先占地址空间，COMMIT 再让这段内存真正可读写。 */
#define MEM_COMMIT_     0x00001000u
#define MEM_RESERVE_    0x00002000u
#define PAGE_READWRITE_ 0x04u

/* CreateFileA / WriteFile 用于生成插件旁边的 UTF-8 日志。 */
#define GENERIC_WRITE_          0x40000000u
#define FILE_SHARE_READ_        0x00000001u
#define CREATE_ALWAYS_          2u
#define FILE_ATTRIBUTE_NORMAL_  0x00000080u
#define INVALID_HANDLE_VALUE_   ((HANDLE)(i32)-1)
#define MAX_PATH_               260u

/* DLL 入口只关心“整个进程加载 DLL”和“整个进程卸载 DLL”。 */
#define DLL_PROCESS_DETACH_ 0u
#define DLL_PROCESS_ATTACH_ 1u

/* 下面每个 typedef 都是在描述一个真实 Win32 API 函数指针的完整签名。 */
typedef HMODULE (WINAPI *PFN_GetModuleHandleA)(const char* name);
typedef DWORD   (WINAPI *PFN_GetModuleFileNameA)(HMODULE module, char* path, DWORD size);
typedef FARPROC (WINAPI *PFN_GetProcAddress)(HMODULE module, const char* name);
typedef HANDLE  (WINAPI *PFN_CreateFileA)(const char*, DWORD, DWORD, void*, DWORD, DWORD, HANDLE);
typedef BOOL    (WINAPI *PFN_WriteFile)(HANDLE, const void*, DWORD, DWORD*, void*);
typedef BOOL    (WINAPI *PFN_CloseHandle)(HANDLE);
typedef BOOL    (WINAPI *PFN_VirtualProtect)(void*, SIZE_T, DWORD, DWORD*);
typedef void*   (WINAPI *PFN_VirtualAlloc)(void*, SIZE_T, DWORD, DWORD);
typedef HANDLE  (WINAPI *PFN_GetCurrentProcess)(void);
typedef BOOL    (WINAPI *PFN_FlushInstructionCache)(HANDLE, const void*, SIZE_T);

/*
 * GetTickCount 返回 Windows 启动以来经过的毫秒数。
 * 它是 32 位计数器，大约 49.7 天会回绕一次；只要用无符号减法计算“两个相邻时间点的差”，
 * 回绕也不会破坏本插件只有几百毫秒的过渡动画。
 */
typedef DWORD   (WINAPI *PFN_GetTickCount)(void);

/*
 * GetPrivateProfileIntA 是 Windows 自带的传统 INI 读取函数。
 * 本插件只需要读取两个整数，因此不必自己写文本解析器，也不需要引入 C 运行库。
 */
typedef u32     (WINAPI *PFN_GetPrivateProfileIntA)(
    const char* section, const char* key, i32 default_value, const char* file_path);

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
