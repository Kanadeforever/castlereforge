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
