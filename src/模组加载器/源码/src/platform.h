#ifndef CASTLE_PLATFORM_H
#define CASTLE_PLATFORM_H

/*
 * 这个文件故意不包含 windows.h。
 *
 * 原因很简单：当前构建环境并没有完整 Windows SDK 头文件，但我们仍然希望：
 *   1. 在容器里用 clang-cl 做真正的 Win32/x86 语法与链接验证；
 *   2. 在用户 Windows 机器上仍可直接用 MSVC + Windows SDK 构建；
 *   3. 每一个 Windows API 的用途都明确可见，不让一个巨大的 windows.h 把依赖关系藏起来。
 *
 * 下面只声明本工程实际需要的 Win32 类型、结构、常量和 API。
 * 如果未来新增功能，应优先在这里补最小声明，并写清为什么需要，而不是随手复制整份 SDK 头文件。
 */

#define WINAPI __stdcall
#define CALLBACK __stdcall
#define DLL_EXPORT __declspec(dllexport)

#define NULL_PTR ((void*)0)

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned short WCHAR;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef long LONG;
typedef int BOOL;
typedef unsigned long SIZE_T;
typedef unsigned long ULONG_PTR;
typedef long LSTATUS;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* HWND;
typedef void* HKEY;
typedef void* FARPROC;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR;
typedef char* LPSTR;
typedef const char* LPCSTR;

typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE_)(LPVOID);

#define TRUE_ 1
#define FALSE_ 0

#define MAX_PATH_ 260u
#define CASTLE_PATH_CAP 2048u
/* 旧 Mod Loader 模块历史上使用 YCR_PATH_CAP。保留这个别名，避免为了改名字误动已经验证过的路径逻辑。 */
#define YCR_PATH_CAP CASTLE_PATH_CAP

#define INVALID_HANDLE_VALUE_ ((HANDLE)(LONG)-1)
#define INVALID_FILE_ATTRIBUTES_ 0xFFFFFFFFu
#define INVALID_SET_FILE_POINTER_ 0xFFFFFFFFu

#define GENERIC_READ_  0x80000000u
#define GENERIC_WRITE_ 0x40000000u
#define FILE_SHARE_READ_   0x00000001u
#define FILE_SHARE_WRITE_  0x00000002u
#define FILE_SHARE_DELETE_ 0x00000004u
#define CREATE_NEW_    1u
#define CREATE_ALWAYS_ 2u
#define OPEN_EXISTING_  3u
#define OPEN_ALWAYS_    4u
#define FILE_ATTRIBUTE_DIRECTORY_ 0x00000010u
#define FILE_ATTRIBUTE_NORMAL_    0x00000080u
#define FILE_ATTRIBUTE_TEMPORARY_ 0x00000100u
/* 文件句柄关闭时自动删除。v0.2.8 的 Locale 文件名自检用它创建一次性探针文件，避免测试垃圾残留在 mods。 */
#define FILE_FLAG_DELETE_ON_CLOSE_ 0x04000000u
#define FILE_BEGIN_ 0u
#define FILE_END_   2u
#define ERROR_FILE_NOT_FOUND_ 2u
#define ERROR_PATH_NOT_FOUND_ 3u
#define ERROR_ALREADY_EXISTS_ 183u
#define ERROR_MOD_NOT_FOUND_ 126u

#define PAGE_READONLY_  0x00000002u
#define PAGE_READWRITE_ 0x00000004u
#define PAGE_EXECUTE_READWRITE_ 0x00000040u
#define MEM_COMMIT_  0x00001000u
#define MEM_RESERVE_ 0x00002000u
#define MEM_RELEASE_ 0x00008000u

#define CP_ACP_ 0u
#define CP_OEMCP_ 1u
#define CP_THREAD_ACP_ 3u
#define CP_UTF8_ 65001u
#define MB_ERR_INVALID_CHARS_ 0x00000008u

#define DONT_RESOLVE_DLL_REFERENCES_ 0x00000001u

/*
 * LoadLibraryExW 的 LOAD_WITH_ALTERED_SEARCH_PATH 标志。
 *
 * 这个标志只用于加载 mods\asi 里的 ASI。调用时必须传入 ASI 的完整绝对路径，
 * Windows 才能把“这个 ASI 自己所在的目录”加入该次 DLL 依赖解析。
 * 例如 Controller.asi 与 SDL3.dll 都放在 mods\asi 时，Controller.asi 在装载过程中
 * 就能优先解析身边的 SDL3.dll。
 *
 * 注意：dev9 重新保留了 dev5 已由用户实机证明兼容的 SetDllDirectoryW(mods) 全局环境。
 * 这里的 LOAD_WITH_ALTERED_SEARCH_PATH 仍然有独立价值：它确保“某个 ASI 自己所在目录”
 * 对该 ASI 的依赖解析优先有效，因此用户插件依赖仍然可以和 ASI 一起放在 mods\asi。
 */
#define LOAD_WITH_ALTERED_SEARCH_PATH_ 0x00000008u

#define CREATE_SUSPENDED_           0x00000004u
#define CREATE_UNICODE_ENVIRONMENT_ 0x00000400u
#define DEBUG_ONLY_THIS_PROCESS_    0x00000002u
#define CREATE_NEW_PROCESS_GROUP_   0x00000200u

#define INFINITE_ 0xFFFFFFFFu
#define WAIT_OBJECT_0_ 0u

#define EXCEPTION_DEBUG_EVENT_      1u
#define CREATE_THREAD_DEBUG_EVENT_  2u
#define CREATE_PROCESS_DEBUG_EVENT_ 3u
#define EXIT_THREAD_DEBUG_EVENT_    4u
#define EXIT_PROCESS_DEBUG_EVENT_   5u
#define LOAD_DLL_DEBUG_EVENT_       6u
#define UNLOAD_DLL_DEBUG_EVENT_     7u
#define OUTPUT_DEBUG_STRING_EVENT_  8u
#define RIP_EVENT_                  9u

#define EXCEPTION_BREAKPOINT_ 0x80000003u
#define DBG_CONTINUE_ 0x00010002u
#define DBG_EXCEPTION_NOT_HANDLED_ 0x80010001u

#define TH32CS_SNAPMODULE_   0x00000008u
#define TH32CS_SNAPMODULE32_ 0x00000010u

#define DLL_PROCESS_DETACH_ 0u
#define DLL_PROCESS_ATTACH_ 1u
#define DLL_THREAD_ATTACH_  2u
#define DLL_THREAD_DETACH_  3u

#define IMAGE_DOS_SIGNATURE_ 0x5A4Du
#define IMAGE_NT_SIGNATURE_  0x00004550u
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC_ 0x010Bu
#define IMAGE_ORDINAL_FLAG32_ 0x80000000u
#define IMAGE_DIRECTORY_ENTRY_EXPORT_ 0u
#define IMAGE_DIRECTORY_ENTRY_IMPORT_ 1u

#define REG_NONE_ 0u
#define REG_SZ_ 1u
#define REG_EXPAND_SZ_ 2u
#define REG_BINARY_ 3u
#define REG_DWORD_ 4u
#define REG_MULTI_SZ_ 7u
#define ERROR_SUCCESS_ 0L
#define ERROR_MORE_DATA_ 234L

#define KEY_QUERY_VALUE_ 0x0001u
#define KEY_READ_ 0x20019u

/* 预定义 HKEY 的 Win32 固定伪句柄。 */
#define HKEY_CURRENT_USER_  ((HKEY)(ULONG_PTR)0x80000001u)
#define HKEY_LOCAL_MACHINE_ ((HKEY)(ULONG_PTR)0x80000002u)

#define MB_OK_ 0x00000000u
#define MB_ICONERROR_ 0x00000010u

#define LOCALE_SYSTEM_DEFAULT_ 0x0800u
#define LOCALE_USER_DEFAULT_   0x0400u
#define LOCALE_NAME_MAX_LENGTH_ 85u

/* 台湾繁中的固定目标环境。 */
#define CASTLE_LCID_ZH_TW_ 0x0404u
#define CASTLE_CP_BIG5_ 950u
#define CASTLE_CHARSET_BIG5_ 136u

/* GetTimeZoneInformation 返回值。台湾没有夏令时，通常返回 TIME_ZONE_ID_UNKNOWN。 */
#define TIME_ZONE_ID_UNKNOWN_ 0u

/* ---------- Windows 基础结构 ---------- */

typedef struct FILETIME_ {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME_;

typedef struct SYSTEMTIME_ {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME_;

typedef struct TIME_ZONE_INFORMATION_ {
    LONG Bias;
    WCHAR StandardName[32];
    SYSTEMTIME_ StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[32];
    SYSTEMTIME_ DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION_;

typedef struct DYNAMIC_TIME_ZONE_INFORMATION_ {
    LONG Bias;
    WCHAR StandardName[32];
    SYSTEMTIME_ StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[32];
    SYSTEMTIME_ DaylightDate;
    LONG DaylightBias;
    WCHAR TimeZoneKeyName[128];
    BOOL DynamicDaylightTimeDisabled;
} DYNAMIC_TIME_ZONE_INFORMATION_;

typedef struct WIN32_FIND_DATAW_ {
    DWORD dwFileAttributes;
    FILETIME_ ftCreationTime;
    FILETIME_ ftLastAccessTime;
    FILETIME_ ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR cFileName[MAX_PATH_];
    WCHAR cAlternateFileName[14];
} WIN32_FIND_DATAW_;

typedef struct STARTUPINFOW_ {
    DWORD cb;
    LPWSTR lpReserved;
    LPWSTR lpDesktop;
    LPWSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    BYTE* lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOW_;

typedef struct PROCESS_INFORMATION_ {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION_;

typedef struct MODULEENTRY32W_ {
    DWORD dwSize;
    DWORD th32ModuleID;
    DWORD th32ProcessID;
    DWORD GlblcntUsage;
    DWORD ProccntUsage;
    BYTE* modBaseAddr;
    DWORD modBaseSize;
    HMODULE hModule;
    WCHAR szModule[256];
    WCHAR szExePath[MAX_PATH_];
} MODULEENTRY32W_;

/*
 * DEBUG_EVENT 的 union 在不同事件下结构不同。
 * 我们只需要读取 CREATE_PROCESS / LOAD_DLL / EXCEPTION 的前几个字段。
 * 为避免自己错误计算整个 Win32 union 的最大尺寸，这里给 256 字节保守空间；
 * WaitForDebugEvent 只会写系统定义的 DEBUG_EVENT 大小，这个缓冲区明显更大，不会越界。
 */
typedef struct DEBUG_EVENT_ {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    BYTE payload[256];
} DEBUG_EVENT_;

typedef struct EXCEPTION_RECORD32_ {
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    DWORD ExceptionRecord;
    DWORD ExceptionAddress;
    DWORD NumberParameters;
    DWORD ExceptionInformation[15];
} EXCEPTION_RECORD32_;

typedef struct EXCEPTION_DEBUG_INFO_ {
    EXCEPTION_RECORD32_ ExceptionRecord;
    DWORD dwFirstChance;
} EXCEPTION_DEBUG_INFO_;

typedef struct CREATE_PROCESS_DEBUG_INFO_ {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPVOID lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO_;

typedef struct LOAD_DLL_DEBUG_INFO_ {
    HANDLE hFile;
    LPVOID lpBaseOfDll;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpImageName;
    WORD fUnicode;
} LOAD_DLL_DEBUG_INFO_;

/* ---------- KERNEL32 静态导入 ---------- */

__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HMODULE module, LPWSTR path, DWORD size);
__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(LPCWSTR name);
__declspec(dllimport) HMODULE WINAPI LoadLibraryW(LPCWSTR path);
__declspec(dllimport) HMODULE WINAPI LoadLibraryExW(LPCWSTR path, HANDLE file, DWORD flags);
__declspec(dllimport) BOOL WINAPI FreeLibrary(HMODULE module);
__declspec(dllimport) FARPROC WINAPI GetProcAddress(HMODULE module, LPCSTR name);
__declspec(dllimport) DWORD WINAPI GetLastError(void);
/*
 * 运行审计 Hook 在调用真实 Win32 API 以后会写日志；写日志本身可能改变线程 LastError。
 * 因此每个透明 Hook 都必须在返回游戏以前用 SetLastError 把原 API 的错误码恢复。
 */
__declspec(dllimport) void WINAPI SetLastError(DWORD error);
__declspec(dllimport) DWORD WINAPI GetTickCount(void);
__declspec(dllimport) void WINAPI Sleep(DWORD milliseconds);
__declspec(dllimport) LONG WINAPI InterlockedExchange(volatile LONG* target, LONG value);
__declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void);
__declspec(dllimport) DWORD WINAPI GetCurrentProcessId(void);
__declspec(dllimport) DWORD WINAPI GetCurrentThreadId(void);
__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE module);
__declspec(dllimport) DWORD WINAPI GetEnvironmentVariableW(LPCWSTR name, LPWSTR value, DWORD size);
__declspec(dllimport) BOOL WINAPI SetEnvironmentVariableW(LPCWSTR name, LPCWSTR value);
__declspec(dllimport) DWORD WINAPI GetFullPathNameW(LPCWSTR path, DWORD size, LPWSTR full, LPWSTR* file_part);
__declspec(dllimport) DWORD WINAPI GetCurrentDirectoryW(DWORD size, LPWSTR buffer);
__declspec(dllimport) DWORD WINAPI GetCurrentDirectoryA(DWORD size, LPSTR buffer);
__declspec(dllimport) BOOL WINAPI SetCurrentDirectoryW(LPCWSTR path);
__declspec(dllimport) BOOL WINAPI SetDllDirectoryW(LPCWSTR path);
__declspec(dllimport) DWORD WINAPI GetFileAttributesW(LPCWSTR path);
__declspec(dllimport) BOOL WINAPI SetFileAttributesA(LPCSTR path, DWORD attributes);
__declspec(dllimport) BOOL WINAPI DeleteFileA(LPCSTR path);
__declspec(dllimport) BOOL WINAPI CreateDirectoryW(LPCWSTR path, LPVOID security);
__declspec(dllimport) HANDLE WINAPI CreateFileW(LPCWSTR path, DWORD access, DWORD share, LPVOID security, DWORD creation, DWORD attrs, HANDLE template_file);
__declspec(dllimport) BOOL WINAPI ReadFile(HANDLE file, LPVOID buffer, DWORD bytes, DWORD* read, LPVOID overlapped);
__declspec(dllimport) BOOL WINAPI WriteFile(HANDLE file, LPCVOID buffer, DWORD bytes, DWORD* written, LPVOID overlapped);
__declspec(dllimport) BOOL WINAPI FlushFileBuffers(HANDLE file);
__declspec(dllimport) BOOL WINAPI SetEndOfFile(HANDLE file);
__declspec(dllimport) DWORD WINAPI GetFinalPathNameByHandleW(HANDLE file, LPWSTR path, DWORD size, DWORD flags);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE handle);
__declspec(dllimport) DWORD WINAPI GetFileSize(HANDLE file, DWORD* high);
__declspec(dllimport) DWORD WINAPI SetFilePointer(HANDLE file, LONG distance, LONG* high, DWORD method);
__declspec(dllimport) HANDLE WINAPI FindFirstFileW(LPCWSTR pattern, WIN32_FIND_DATAW_* data);
__declspec(dllimport) BOOL WINAPI FindNextFileW(HANDLE find, WIN32_FIND_DATAW_* data);
__declspec(dllimport) BOOL WINAPI FindClose(HANDLE find);
__declspec(dllimport) BOOL WINAPI VirtualProtect(LPVOID address, SIZE_T size, DWORD new_protect, DWORD* old_protect);
__declspec(dllimport) BOOL WINAPI VirtualProtectEx(HANDLE process, LPVOID address, SIZE_T size, DWORD new_protect, DWORD* old_protect);
__declspec(dllimport) LPVOID WINAPI VirtualAlloc(LPVOID address, SIZE_T size, DWORD allocation_type, DWORD protect);
__declspec(dllimport) LPVOID WINAPI VirtualAllocEx(HANDLE process, LPVOID address, SIZE_T size, DWORD allocation_type, DWORD protect);
__declspec(dllimport) BOOL WINAPI VirtualFreeEx(HANDLE process, LPVOID address, SIZE_T size, DWORD free_type);
__declspec(dllimport) BOOL WINAPI WriteProcessMemory(HANDLE process, LPVOID base, LPCVOID buffer, SIZE_T size, SIZE_T* written);
__declspec(dllimport) BOOL WINAPI ReadProcessMemory(HANDLE process, LPCVOID base, LPVOID buffer, SIZE_T size, SIZE_T* read);
__declspec(dllimport) BOOL WINAPI FlushInstructionCache(HANDLE process, LPCVOID address, SIZE_T size);
/*
 * VEH 只用于旁路记录严重异常；回调永远返回 EXCEPTION_CONTINUE_SEARCH，
 * 因此不会把原版游戏本来应该崩溃/处理的异常“修掉”。
 */
__declspec(dllimport) LPVOID WINAPI AddVectoredExceptionHandler(ULONG_PTR first, LPVOID handler);
__declspec(dllimport) DWORD WINAPI ResumeThread(HANDLE thread);
__declspec(dllimport) BOOL WINAPI TerminateProcess(HANDLE process, UINT exit_code);
__declspec(dllimport) BOOL WINAPI CreateProcessW(LPCWSTR app, LPWSTR cmdline, LPVOID proc_attr, LPVOID thread_attr, BOOL inherit, DWORD flags, LPVOID env, LPCWSTR cwd, STARTUPINFOW_* startup, PROCESS_INFORMATION_* process);
__declspec(dllimport) BOOL WINAPI WaitForDebugEvent(DEBUG_EVENT_* event, DWORD milliseconds);
__declspec(dllimport) BOOL WINAPI ContinueDebugEvent(DWORD pid, DWORD tid, DWORD status);
__declspec(dllimport) BOOL WINAPI DebugActiveProcessStop(DWORD pid);
__declspec(dllimport) HANDLE WINAPI CreateRemoteThread(HANDLE process, LPVOID attrs, SIZE_T stack, LPTHREAD_START_ROUTINE_ start, LPVOID param, DWORD flags, DWORD* tid);
__declspec(dllimport) DWORD WINAPI WaitForSingleObject(HANDLE handle, DWORD milliseconds);
__declspec(dllimport) BOOL WINAPI GetExitCodeThread(HANDLE thread, DWORD* code);
__declspec(dllimport) HANDLE WINAPI CreateToolhelp32Snapshot(DWORD flags, DWORD pid);
__declspec(dllimport) BOOL WINAPI Module32FirstW(HANDLE snapshot, MODULEENTRY32W_* entry);
__declspec(dllimport) BOOL WINAPI Module32NextW(HANDLE snapshot, MODULEENTRY32W_* entry);
__declspec(dllimport) void WINAPI ExitProcess(UINT code);
__declspec(dllimport) int WINAPI MultiByteToWideChar(UINT cp, DWORD flags, LPCSTR src, int src_len, LPWSTR dst, int dst_len);
__declspec(dllimport) int WINAPI WideCharToMultiByte(UINT cp, DWORD flags, LPCWSTR src, int src_len, LPSTR dst, int dst_len, LPCSTR default_char, BOOL* used_default);
__declspec(dllimport) UINT WINAPI GetPrivateProfileStringW(LPCWSTR section, LPCWSTR key, LPCWSTR def, LPWSTR out, DWORD size, LPCWSTR file);
__declspec(dllimport) BOOL WINAPI WritePrivateProfileStringW(LPCWSTR section, LPCWSTR key, LPCWSTR value, LPCWSTR file);
__declspec(dllimport) DWORD WINAPI GetSystemDirectoryW(LPWSTR out, UINT size);

#endif
