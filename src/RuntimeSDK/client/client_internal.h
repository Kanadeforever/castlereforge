#ifndef CASTLE_RUNTIME_CLIENT_INTERNAL_H
#define CASTLE_RUNTIME_CLIENT_INTERNAL_H

/*
 * client_internal.h
 *
 * 这个头只连接编进同一个 ASI 的 runtime_client.c 与 runtime_entry_gate.c。
 * 每个 ASI 都有自己的一份全局变量；跨插件协调仍通过导出查询和 Runtime 完成。
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <tlhelp32.h>

#include "../include/CastleRuntime_Client.h"

#define CLIENT_PATH_CAP 1024u
#define CLIENT_MAX_PLUGINS 128u

/*
 * GetProcAddress 返回 Windows 定义的 FARPROC。x86 SDK 的稳定导出使用 __cdecl，较新的 Clang
 * 会把“FARPROC 直接强转为 __cdecl 函数指针”诊断为不兼容调用约定。Windows PE 保证这些
 * 指针具有相同位宽，所以这里把指针变量本身的字节复制到目标函数指针对象，不执行函数、
 * 不经过整数，也不要求每个调用点关闭 -Wcast-function-type-mismatch。
 */
static int Client_CopyProcedureAddress(void* output, CastleU32 output_size,
                                       FARPROC address) {
    volatile BYTE* output_bytes = (volatile BYTE*)output;
    const volatile BYTE* input_bytes = (const volatile BYTE*)&address;
    CastleU32 index;
    if (!output || !address || output_size != (CastleU32)sizeof(address)) return 0;
    for (index = 0u; index < output_size; ++index) {
        output_bytes[index] = input_bytes[index];
    }
    return 1;
}

extern HMODULE g_client_module;
extern CastlePluginExportV1* g_client_export;
extern volatile LONG g_client_state;
extern BYTE* g_client_entry;
extern BYTE* g_client_entry_after;

void CastleRuntimeClient_EntryGateThunk(void);
CastleResult Client_InstallOrJoinEntryGate(void);
CastleResult Client_RestoreKnownEntryGate(void);
CastleResult Client_InstallOrJoinEntryGateAt(BYTE* entry);
CastleResult Client_RestoreKnownEntryGateAt(BYTE* entry);
CastleResult CASTLE_RUNTIME_CALL Client_BootstrapPlugin(
    CastleU32 mode,
    const CastleRuntimeApiV1* runtime_api,
    CastlePluginHandle plugin_handle,
    CastleResult reason);

#endif /* CASTLE_RUNTIME_CLIENT_INTERNAL_H */
