#pragma once

// ============================================================================
// Win32Mini.h  v0.3.2
// ----------------------------------------------------------------------------
// 这个头文件只声明四个正式 ASI 真正会使用的少量 Windows 基础类型和 API。
// v0.3.2 因 BUGFix 合并 CrashFix test2，新增 VirtualAlloc / VirtualFree / GetLastError，
// 供双路径 x86 stub 的申请、释放和失败日志使用；其它插件不因此增加运行时行为。
//
// 为什么不用 <windows.h>：
// 1. 《幽城幻剑录》的 RPG.exe 是 32 位程序，本插件只需要 kernel32.dll 的少数功能；
// 2. 把最小 API 全部写在一个短文件里，初学者可以直接看到插件向 Windows 请求什么；
// 3. 同一份源码既能在 Windows + MSVC 下编译，也能在研究环境用 Clang 做第二套
//    32 位 PE 交叉构建验证，不依赖完整 Windows SDK 头文件。
//
// 这里“只是声明”，并没有自己实现 Windows。最终这些函数仍由系统 kernel32.dll 提供。
// ============================================================================

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned long SIZE_T;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef const char* LPCSTR;
typedef char* LPSTR;
typedef const wchar_t* LPCWSTR;
typedef wchar_t* LPWSTR;
typedef HMODULE HINSTANCE;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DLL_PROCESS_DETACH 0u
#define DLL_PROCESS_ATTACH 1u

#define PAGE_NOACCESS 0x01u
#define PAGE_READONLY 0x02u
#define PAGE_READWRITE 0x04u
#define PAGE_WRITECOPY 0x08u
#define PAGE_EXECUTE 0x10u
#define PAGE_EXECUTE_READ 0x20u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u
#define PAGE_GUARD 0x100u
#define MEM_COMMIT 0x1000u
#define MEM_RESERVE 0x2000u
#define MEM_RELEASE 0x8000u

#define GENERIC_READ 0x80000000u
#define GENERIC_WRITE 0x40000000u
#define FILE_SHARE_READ 0x00000001u
#define FILE_SHARE_WRITE 0x00000002u
#define FILE_SHARE_DELETE 0x00000004u
#define CREATE_NEW 1u
#define CREATE_ALWAYS 2u
#define OPEN_EXISTING 3u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define INVALID_FILE_SIZE 0xFFFFFFFFu
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFFu
#define INVALID_HANDLE_VALUE ((HANDLE)(long)-1)

#ifndef WINAPI
#define WINAPI __stdcall
#endif

// 32 位 Windows 的 VirtualQuery 输出结构。
// 插件在读取游戏运行时指针前会先确认目标内存页已经提交且可访问，避免因为某个对象
// 尚未创建、已经销毁或指针暂时为空而直接解引用导致游戏崩溃。
struct MEMORY_BASIC_INFORMATION_MINI {
    LPVOID BaseAddress;
    LPVOID AllocationBase;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
};

// 研究环境的 Clang 没有微软官方 kernel32.lib。交叉构建时会临时生成只用于链接验证
// 的 import library；这个别名让最终 PE 导入表仍使用 Windows 的真实 API 名。
#ifdef YCR_CROSS_BUILD
#define YCR_IMPORT_ALIAS(name) __asm__("_" #name)
#else
#define YCR_IMPORT_ALIAS(name)
#endif

extern "C" {

__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(LPCWSTR moduleName)
    YCR_IMPORT_ALIAS(GetModuleHandleW);

__declspec(dllimport) BOOL WINAPI VirtualProtect(
    LPVOID address,
    SIZE_T size,
    DWORD newProtect,
    DWORD* oldProtect)
    YCR_IMPORT_ALIAS(VirtualProtect);

__declspec(dllimport) BOOL WINAPI FlushInstructionCache(
    HANDLE process,
    LPCVOID baseAddress,
    SIZE_T size)
    YCR_IMPORT_ALIAS(FlushInstructionCache);

__declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void)
    YCR_IMPORT_ALIAS(GetCurrentProcess);

__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE module)
    YCR_IMPORT_ALIAS(DisableThreadLibraryCalls);

__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(
    HMODULE module,
    LPWSTR fileName,
    DWORD size)
    YCR_IMPORT_ALIAS(GetModuleFileNameW);

__declspec(dllimport) HANDLE WINAPI CreateFileW(
    LPCWSTR fileName,
    DWORD desiredAccess,
    DWORD shareMode,
    LPVOID securityAttributes,
    DWORD creationDisposition,
    DWORD flagsAndAttributes,
    HANDLE templateFile)
    YCR_IMPORT_ALIAS(CreateFileW);

__declspec(dllimport) BOOL WINAPI WriteFile(
    HANDLE file,
    LPCVOID buffer,
    DWORD bytesToWrite,
    DWORD* bytesWritten,
    LPVOID overlapped)
    YCR_IMPORT_ALIAS(WriteFile);

__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE object)
    YCR_IMPORT_ALIAS(CloseHandle);


__declspec(dllimport) DWORD WINAPI GetFileAttributesW(LPCWSTR fileName)
    YCR_IMPORT_ALIAS(GetFileAttributesW);

__declspec(dllimport) unsigned int WINAPI GetPrivateProfileIntW(
    LPCWSTR section,
    LPCWSTR key,
    int defaultValue,
    LPCWSTR fileName)
    YCR_IMPORT_ALIAS(GetPrivateProfileIntW);

__declspec(dllimport) SIZE_T WINAPI VirtualQuery(
    LPCVOID address,
    MEMORY_BASIC_INFORMATION_MINI* buffer,
    SIZE_T length)
    YCR_IMPORT_ALIAS(VirtualQuery);

// BUGFix v0.3.2 合并 CrashFix 后需要为两条 x86 跳转路径建立极小的可执行 stub。
// VirtualAlloc 负责申请 stub 内存；VirtualFree 在插件被正常卸载时释放它。
__declspec(dllimport) LPVOID WINAPI VirtualAlloc(
    LPVOID address,
    SIZE_T size,
    DWORD allocationType,
    DWORD protect)
    YCR_IMPORT_ALIAS(VirtualAlloc);

__declspec(dllimport) BOOL WINAPI VirtualFree(
    LPVOID address,
    SIZE_T size,
    DWORD freeType)
    YCR_IMPORT_ALIAS(VirtualFree);

// GetLastError 只用于把极少数安装失败的 Win32 原因写进 BUGFix.log。
// 修复逻辑本身不会依赖错误码作危险的“猜测式继续”。
__declspec(dllimport) DWORD WINAPI GetLastError(void)
    YCR_IMPORT_ALIAS(GetLastError);
}

#ifndef YCR_CROSS_BUILD
#pragma comment(lib, "kernel32.lib")
#endif
