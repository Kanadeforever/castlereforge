#ifndef CASTLE_PAD_PLATFORM_H
#define CASTLE_PAD_PLATFORM_H

/*
 * platform.h
 *
 * 这个文件只负责“最底层的平台类型和调用约定”。
 * 项目故意不包含 windows.h，也不依赖 C 运行库。这样做的原因是：
 * 1. 原来的 dev20 就是一个非常小的 ASI，所有 Win32 API 都从 RPG.exe 的 IAT 或 KERNEL32/USER32 动态取得；
 * 2. 链接时使用 /nodefaultlib，可以避免把额外 CRT 依赖带进老游戏；
 * 3. 这里把真正需要的类型逐个写出来，任何人打开源码都能直接看到插件到底依赖了什么。
 *
 * 对刚学编程的读者：可以把“typedef”理解成“给一种数据类型起一个更好记的名字”。
 * 例如 u32 就是 32 位无符号整数，i32 就是 32 位有符号整数。
 */

#define NULL ((void*)0)

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed short   i16;
typedef signed int     i32;
typedef unsigned long  DWORD;
typedef unsigned int   UINT;
typedef int            BOOL;
typedef short          SHORT;
typedef long           HRESULT;
typedef void*          HANDLE;
typedef void*          HMODULE;
typedef void*          HWND;
typedef void*          FARPROC;
typedef unsigned long  SIZE_T;

/* Windows 的 POINT/RECT 结构。我们只需要坐标和客户区边界，因此自己声明最小版本。 */
typedef struct Point32 {
    i32 x;
    i32 y;
} Point32;

typedef struct Rect32 {
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;
} Rect32;

/*
 * MSVC 与 clang-cl 都认识这些调用约定。
 * __stdcall / __fastcall / __thiscall 很重要，因为 RPG.exe 是 32 位程序，
 * 如果调用约定写错，参数会从错误的位置读取，函数返回后栈也可能损坏。
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

/* 下面这些值都来自 Win32 API 定义，只保留本插件确实会用到的部分。 */
#define PAGE_READWRITE_                    0x04u
#define INVALID_HANDLE_VALUE_              ((HANDLE)(i32)-1)
#define GENERIC_WRITE_                     0x40000000u
#define FILE_SHARE_READ_                   0x00000001u
#define CREATE_ALWAYS_                     2u
#define FILE_ATTRIBUTE_NORMAL_             0x00000080u
#define MAX_PATH_                          260u
#define GET_MODULE_HANDLE_EX_FLAG_PIN_     0x00000001u
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS_ 0x00000004u
#define MOUSEEVENTF_LEFTDOWN_              0x0002u
#define MOUSEEVENTF_LEFTUP_                0x0004u
#define MOUSEEVENTF_RIGHTDOWN_             0x0008u
#define MOUSEEVENTF_RIGHTUP_               0x0010u

/*
 * Windows 消息常量。
 * 本项目不需要完整 windows.h，只保留“把 Start 当成一次 ESC 键按下”真正会用到的两个值。
 * WM_KEYDOWN 表示某个键刚被按下；VK_ESCAPE 就是键盘 ESC 的虚拟键码。
 */
#define WM_KEYDOWN_                         0x0100u
#define VK_ESCAPE_                          0x001Bu
#define VK_LBUTTON_                         0x0001u
#define VK_RBUTTON_                         0x0002u

/* Runtime 工作线程每 8 ms 跑一次。dev20 的所有毫秒配置都会换算成这个 tick。 */
#define WORKER_SLEEP_MS 8u

/*
 * Win32 函数指针类型。
 * 这里不直接“调用 Windows API 名字”，而是把 API 地址保存为函数指针后再调用。
 * 这样链接器不需要生成传统 import library 依赖。
 */
typedef HMODULE (WINAPI *PFN_GetModuleHandleA)(const char*);
typedef FARPROC (WINAPI *PFN_GetProcAddress)(HMODULE,const char*);
typedef HMODULE (WINAPI *PFN_LoadLibraryA)(const char*);
typedef DWORD   (WINAPI *PFN_GetModuleFileNameA)(HMODULE,char*,DWORD);
typedef BOOL    (WINAPI *PFN_GetModuleHandleExA)(DWORD,const char*,HMODULE*);
typedef BOOL    (WINAPI *PFN_VirtualProtect)(void*,SIZE_T,DWORD,DWORD*);
typedef BOOL    (WINAPI *PFN_GetCursorPos)(Point32*);
typedef HANDLE  (WINAPI *PFN_CreateFileA)(const char*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
typedef BOOL    (WINAPI *PFN_WriteFile)(HANDLE,const void*,DWORD,DWORD*,void*);
typedef BOOL    (WINAPI *PFN_CloseHandle)(HANDLE);
typedef DWORD   (WINAPI *PFN_ThreadProc)(void*);
typedef HANDLE  (WINAPI *PFN_CreateThread)(void*,SIZE_T,PFN_ThreadProc,void*,DWORD,DWORD*);
typedef void    (WINAPI *PFN_Sleep)(DWORD);
typedef HWND    (WINAPI *PFN_GetForegroundWindow)(void);
typedef BOOL    (WINAPI *PFN_GetClientRect)(HWND,Rect32*);
typedef BOOL    (WINAPI *PFN_ClientToScreen)(HWND,Point32*);
typedef BOOL    (WINAPI *PFN_SetCursorPos)(i32,i32);
typedef DWORD   (WINAPI *PFN_GetWindowThreadProcessId)(HWND,DWORD*);
typedef DWORD   (WINAPI *PFN_GetCurrentProcessId)(void);
typedef UINT    (WINAPI *PFN_GetPrivateProfileIntA)(const char*,const char*,int,const char*);
typedef void    (WINAPI *PFN_mouse_event)(DWORD,DWORD,DWORD,DWORD,SIZE_T);

/*
 * PostMessageA 不会在 worker 线程里直接调用游戏窗口过程。
 * 它只是把消息放进 RPG.exe 自己的窗口消息队列，之后仍由游戏原本的窗口过程处理。
 * 这正适合“Start 等价于用户按了一次 ESC”：插件不碰 Bink 对象，只把原版已经认识的输入交回原版。
 */
typedef BOOL    (WINAPI *PFN_PostMessageA)(HWND,UINT,u32,i32);
typedef SHORT   (WINAPI *PFN_GetAsyncKeyState)(int);
typedef SHORT   (WINAPI *PFN_GetKeyState)(int);

/* RPG.exe 内部函数的调用约定。 */
typedef u8      (FASTCALL *PFN_ButtonHitFast)(void*,void*);
typedef i32     (THISCALL *PFN_ButtonEventThis)(void*);
typedef i32     (THISCALL *PFN_TargetGridThis)(void*,i32,i32);
typedef void    (THISCALL *PFN_SaveSlotAnimThis)(void*);
typedef void    (THISCALL *PFN_SaveSlotRefreshThis)(void*,i32);
typedef i32     (CDECL *PFN_ExplorationMove)(i32,i32,i32,i32,i32*);
typedef i32     (CDECL *PFN_MouseAction)(void);
typedef i32     (THISCALL *PFN_ExplorationTargetResolve)(void*,i32);
/*
 * 地图动作业务事件的调用约定。
 * 基线 EXE 中原版 Space 最终是 push action / call 0x40B230；refactor20a 开始由 Runtime
 * 从 Space 的 CALL 现场解析“当前实际目标”，所以其它兼容补丁即使把目标换成 wrapper，也能继续跟随。
 * Y 仍只在 Exploration 游戏线程中调用，绝不从 worker 直接改状态。
 */
typedef void    (CDECL *PFN_MapSpaceEvent)(i32);
typedef void*   (CDECL *PFN_GetControlledActor)(void);
typedef void    (THISCALL *PFN_ThisVoid)(void*);
typedef void    (THISCALL *PFN_ExploreCursorDraw)(void*,i32);

#endif /* CASTLE_PAD_PLATFORM_H */
