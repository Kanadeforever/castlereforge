#pragma once

// MaxGrowthAndDrop 只需要取得 RPG.exe 基址并处理 DLL 生命周期；配置与补丁均由 Runtime 提供。
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned long SIZE_T;
typedef void* HMODULE;
typedef void* LPVOID;
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

#ifndef WINAPI
#define WINAPI __stdcall
#endif

// 交叉验证构建使用与 Win32 导入表一致的未修饰 API 名；正式 MSVC 构建直接走 kernel32.lib。
#ifdef YCR_CROSS_BUILD
#define YCR_IMPORT_ALIAS(name) __asm__("_" #name)
#else
#define YCR_IMPORT_ALIAS(name)
#endif

extern "C" {
__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(LPCWSTR moduleName)
    YCR_IMPORT_ALIAS(GetModuleHandleW);
__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE module)
    YCR_IMPORT_ALIAS(DisableThreadLibraryCalls);
}
