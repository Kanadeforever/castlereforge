#ifndef CASTLE_SAVE_ENHANCE_WIN32_MINI_H
#define CASTLE_SAVE_ENHANCE_WIN32_MINI_H

// ============================================================================
// Win32Mini.h
// ----------------------------------------------------------------------------
// 这个项目故意不包含 <windows.h>，原因不是 Windows 头文件不好，而是本插件希望维持
// “无 CRT、只依赖极少 Windows API”的老游戏 ASI 结构，同时还要能在没有 Windows SDK
// 头文件的交叉编译环境里做第二套语法/ABI 检查。
//
// 对刚接触编程的人，可以把这个文件理解成一份“我们实际会用到的 Windows 名词表”：
// - DWORD / BYTE 这些是固定大小的数字类型；
// - HMODULE / HANDLE 是 Windows 返回给程序的“对象把手”；
// - extern "C" 下面是 Windows 已经提供的函数，我们这里只告诉编译器它们长什么样；
// - 真正的实现仍然在 Windows 自带的 kernel32.dll 里，不是我们重新实现一遍。
//
// 这里每加一个声明都应该有明确用途，不要把整个 Windows SDK 手工复制进来。
// ============================================================================

#if defined(_MSC_VER) || defined(__clang__)
#define WINAPI __stdcall
#define CALLBACK __stdcall
#else
#define WINAPI
#define CALLBACK
#endif

// 32 位目标下这些基础类型的大小与 Win32 SDK 保持一致。
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef signed long LONG;
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned int SIZE_T;
typedef void* HANDLE;
typedef void* HINSTANCE;
typedef void* HMODULE;
typedef void* HWND;
typedef void* FARPROC;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef const char* LPCSTR;
typedef const wchar_t* LPCWSTR;
typedef wchar_t* LPWSTR;
typedef DWORD* LPDWORD;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// DllMain 的 reason 值。ASI 被装进进程时收到 ATTACH，游戏退出/卸载时收到 DETACH。
#define DLL_PROCESS_DETACH 0u
#define DLL_PROCESS_ATTACH 1u

// VirtualQuery / VirtualProtect 相关常量。只保留本插件真正检查/修改内存时需要的值。
#define MEM_COMMIT 0x00001000u
#define PAGE_NOACCESS 0x00000001u
#define PAGE_READONLY 0x00000002u
#define PAGE_READWRITE 0x00000004u
#define PAGE_WRITECOPY 0x00000008u
#define PAGE_EXECUTE 0x00000010u
#define PAGE_EXECUTE_READ 0x00000020u
#define PAGE_EXECUTE_READWRITE 0x00000040u
#define PAGE_EXECUTE_WRITECOPY 0x00000080u
#define PAGE_GUARD 0x00000100u

// CreateFileW / ReadFile / WriteFile 的最小文件常量。
// - 日志只需要 GENERIC_WRITE + CREATE_ALWAYS；
// - 外置 WAV 的音量缩放需要 GENERIC_READ + OPEN_EXISTING，只读原文件后在内存副本里改振幅。
#define GENERIC_READ 0x80000000u
#define GENERIC_WRITE 0x40000000u
#define FILE_SHARE_READ 0x00000001u
#define OPEN_EXISTING 3u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define ERROR_ALREADY_EXISTS 183u
#define INVALID_FILE_SIZE 0xFFFFFFFFu
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG)-1)

// 键盘虚拟键。用户已经固定 F5=快速存档、F9=快速读档，所以不需要整张 VK 表。
#define VK_F5 0x74
#define VK_F9 0x78

// PlaySoundW 的标志。它来自 winmm.dll；本插件运行时动态获取，不形成静态依赖。
// SND_MEMORY 表示第一个参数不是文件名，而是一整块仍然存活的 WAV 文件内存。
// SaveEnhance 用它播放“已经在内存里按 Volume 缩放过”的 WAV 副本。
#define SND_ASYNC 0x0001u
#define SND_NODEFAULT 0x0002u
#define SND_MEMORY 0x0004u
#define SND_FILENAME 0x00020000u


// Windows 的 MEMORY_BASIC_INFORMATION 在 32 位进程中的最小布局。
// VirtualQuery 会把某段地址所在内存区域的信息写进这里。
typedef struct MEMORY_BASIC_INFORMATION_MINI {
    LPVOID BaseAddress;
    LPVOID AllocationBase;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION_MINI;

extern "C" {
// ---- 模块和函数地址 ---------------------------------------------------------
HMODULE WINAPI GetModuleHandleW(LPCWSTR moduleName);
HMODULE WINAPI GetModuleHandleA(LPCSTR moduleName);
DWORD WINAPI GetModuleFileNameW(HMODULE module, LPWSTR filename, DWORD size);
FARPROC WINAPI GetProcAddress(HMODULE module, LPCSTR procName);
HMODULE WINAPI LoadLibraryW(LPCWSTR filename);
BOOL WINAPI FreeLibrary(HMODULE module);

// ---- INI --------------------------------------------------------------------
UINT WINAPI GetPrivateProfileIntW(LPCWSTR section, LPCWSTR key, int defaultValue, LPCWSTR filename);
DWORD WINAPI GetPrivateProfileStringW(
    LPCWSTR section,
    LPCWSTR key,
    LPCWSTR defaultValue,
    LPWSTR returnedString,
    DWORD size,
    LPCWSTR filename);
// ---- 时间 / 进程 ------------------------------------------------------------
DWORD WINAPI GetTickCount(void);
DWORD WINAPI GetCurrentProcessId(void);
HANDLE WINAPI GetCurrentProcess(void);
BOOL WINAPI DisableThreadLibraryCalls(HMODULE module);

// ---- 内存 -------------------------------------------------------------------
SIZE_T WINAPI VirtualQuery(LPCVOID address, MEMORY_BASIC_INFORMATION_MINI* info, SIZE_T length);
BOOL WINAPI VirtualProtect(LPVOID address, SIZE_T size, DWORD newProtect, LPDWORD oldProtect);
BOOL WINAPI FlushInstructionCache(HANDLE process, LPCVOID baseAddress, SIZE_T size);

// ---- 外置 WAV 只读装载 ------------------------------------------------------
// SaveEnhance 不改磁盘上的 WAV。它只读完整文件，复制到进程堆，再对内存副本的 PCM 样本缩放。
BOOL WINAPI ReadFile(
    HANDLE file,
    LPVOID buffer,
    DWORD bytesToRead,
    LPDWORD bytesRead,
    LPVOID overlapped);
DWORD WINAPI GetFileSize(HANDLE file, LPDWORD fileSizeHigh);
HANDLE WINAPI GetProcessHeap(void);
LPVOID WINAPI HeapAlloc(HANDLE heap, DWORD flags, SIZE_T bytes);
BOOL WINAPI HeapFree(HANDLE heap, DWORD flags, LPVOID memory);

// ---- 日志文件 ---------------------------------------------------------------
BOOL WINAPI CreateDirectoryW(LPCWSTR pathName, LPVOID securityAttributes);
DWORD WINAPI GetLastError(void);
HANDLE WINAPI CreateFileW(
    LPCWSTR filename,
    DWORD desiredAccess,
    DWORD shareMode,
    LPVOID securityAttributes,
    DWORD creationDisposition,
    DWORD flagsAndAttributes,
    HANDLE templateFile);
BOOL WINAPI WriteFile(
    HANDLE file,
    LPCVOID buffer,
    DWORD bytesToWrite,
    LPDWORD bytesWritten,
    LPVOID overlapped);
BOOL WINAPI CloseHandle(HANDLE handle);
}

#endif // CASTLE_SAVE_ENHANCE_WIN32_MINI_H
