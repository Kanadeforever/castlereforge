#pragma once

// BUGFix 只声明当前真正使用的 Kernel32 能力：读取游戏内存布局、建立两段私有执行 stub、
// 刷新该 stub 的指令缓存，以及处理 DLL 生命周期。RPG.exe 补丁写入仍由 Runtime Hook 负责。
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned long SIZE_T;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef const wchar_t* LPCWSTR;
typedef HMODULE HINSTANCE;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define DLL_PROCESS_DETACH 0u
#define DLL_PROCESS_ATTACH 1u

#define PAGE_READONLY 0x02u
#define PAGE_READWRITE 0x04u
#define PAGE_WRITECOPY 0x08u
#define PAGE_EXECUTE_READ 0x20u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u
#define PAGE_GUARD 0x100u
#define MEM_COMMIT 0x1000u
#define MEM_RESERVE 0x2000u
#define MEM_RELEASE 0x8000u

#ifndef WINAPI
#define WINAPI __stdcall
#endif

struct MEMORY_BASIC_INFORMATION_MINI {
    LPVOID BaseAddress;
    LPVOID AllocationBase;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
};

// 交叉验证构建使用与 Win32 导入表一致的未修饰 API 名；正式 MSVC 构建直接走 kernel32.lib。
#ifdef YCR_CROSS_BUILD
#define YCR_IMPORT_ALIAS(name) __asm__("_" #name)
#else
#define YCR_IMPORT_ALIAS(name)
#endif

extern "C" {
__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(LPCWSTR moduleName)
    YCR_IMPORT_ALIAS(GetModuleHandleW);
__declspec(dllimport) SIZE_T WINAPI VirtualQuery(
    LPCVOID address, MEMORY_BASIC_INFORMATION_MINI* buffer, SIZE_T length)
    YCR_IMPORT_ALIAS(VirtualQuery);
__declspec(dllimport) LPVOID WINAPI VirtualAlloc(
    LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect)
    YCR_IMPORT_ALIAS(VirtualAlloc);
__declspec(dllimport) BOOL WINAPI VirtualFree(
    LPVOID address, SIZE_T size, DWORD freeType)
    YCR_IMPORT_ALIAS(VirtualFree);
__declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void)
    YCR_IMPORT_ALIAS(GetCurrentProcess);
__declspec(dllimport) BOOL WINAPI FlushInstructionCache(
    HANDLE process, LPCVOID baseAddress, SIZE_T size)
    YCR_IMPORT_ALIAS(FlushInstructionCache);
__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE module)
    YCR_IMPORT_ALIAS(DisableThreadLibraryCalls);
}
