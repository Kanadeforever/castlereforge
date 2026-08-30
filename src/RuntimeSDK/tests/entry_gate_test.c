/*
 * entry_gate_test.c
 *
 * 在 VirtualAlloc 的 5 字节测试入口上验证 Owner、Follower、恢复、未知冲突和
 * 未提交地址。正式 Client 仍只能从 GetModuleHandle(NULL)+固定 RPG RVA 调用这些私有逻辑。
 */

#include "../client/client_internal.h"

HMODULE g_client_module;
CastlePluginExportV1* g_client_export;
volatile LONG g_client_state;
BYTE* g_client_entry;
BYTE* g_client_entry_after;

static CastlePluginExportV1 g_test_export;

static void entry_copy_(BYTE* output, const BYTE* input, CastleU32 count) {
    CastleU32 index;
    for (index = 0u; index < count; ++index) output[index] = input[index];
}

static int entry_equal_(const BYTE* left, const BYTE* right, CastleU32 count) {
    CastleU32 index;
    for (index = 0u; index < count; ++index) {
        if (left[index] != right[index]) return 0;
    }
    return 1;
}

static void entry_write_jump_(BYTE* site, const void* target) {
    LONG relative = (LONG)((const BYTE*)target - (site + 5u));
    site[0] = 0xE9u;
    site[1] = (BYTE)((DWORD)relative & 0xFFu);
    site[2] = (BYTE)(((DWORD)relative >> 8u) & 0xFFu);
    site[3] = (BYTE)(((DWORD)relative >> 16u) & 0xFFu);
    site[4] = (BYTE)(((DWORD)relative >> 24u) & 0xFFu);
}

static void* entry_read_jump_(const BYTE* site) {
    LONG relative = (LONG)((DWORD)site[1] | ((DWORD)site[2] << 8u) |
        ((DWORD)site[3] << 16u) | ((DWORD)site[4] << 24u));
    return site[0] == 0xE9u ? (void*)(site + 5u + relative) : NULL;
}

void __cdecl RuntimeTest_FollowerThunk(void) {
}

const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requested_export_version) {
    return requested_export_version == CASTLE_PLUGIN_EXPORT_VERSION_1 ?
        &g_test_export : NULL;
}

CastleResult CASTLE_RUNTIME_CALL CastleRuntimeClient_BootstrapAll(
    CastleU32 trigger_kind, CastleModule trigger_module) {
    (void)trigger_kind;
    (void)trigger_module;
    return CASTLE_OK;
}

__declspec(noreturn) void __stdcall EntryGateTestEntry(void) {
    static const BYTE original[CASTLE_RPG_ENTRY_PATCH_SIZE] = {
        0x55u, 0x8Bu, 0xECu, 0x6Au, 0xFFu
    };
    BYTE* entry = (BYTE*)VirtualAlloc(NULL, 4096u, MEM_RESERVE | MEM_COMMIT,
                                      PAGE_EXECUTE_READWRITE);
    CastleResult result;
    if (!entry) ExitProcess(1u);
    g_client_module = GetModuleHandleW(NULL);
    g_test_export.magic = CASTLE_PLUGIN_QUERY_MAGIC;
    g_test_export.struct_size = CASTLE_SIZEOF_PLUGIN_EXPORT_V1;
    g_test_export.export_version = CASTLE_PLUGIN_EXPORT_VERSION_1;
    g_test_export.entry_gate_thunk =
        (CastleAddress)(ULONG_PTR)&CastleRuntimeClient_EntryGateThunk;
    g_client_export = &g_test_export;

    entry_copy_(entry, original, CASTLE_RPG_ENTRY_PATCH_SIZE);
    InterlockedExchange(&g_client_state, CASTLE_CLIENT_COLD);
    result = Client_InstallOrJoinEntryGateAt(entry);
    if (result != CASTLE_OK || g_client_state != CASTLE_CLIENT_GATE_OWNER ||
        entry_read_jump_(entry) != (void*)&CastleRuntimeClient_EntryGateThunk) {
        ExitProcess(2u);
    }
    result = Client_RestoreKnownEntryGateAt(entry);
    if (result != CASTLE_OK ||
        !entry_equal_(entry, original, CASTLE_RPG_ENTRY_PATCH_SIZE)) ExitProcess(3u);
    if (Client_RestoreKnownEntryGateAt(entry) != CASTLE_STATUS_ALREADY_DONE) {
        ExitProcess(4u);
    }

    g_test_export.entry_gate_thunk =
        (CastleAddress)(ULONG_PTR)&RuntimeTest_FollowerThunk;
    entry_write_jump_(entry, (const void*)&RuntimeTest_FollowerThunk);
    InterlockedExchange(&g_client_state, CASTLE_CLIENT_COLD);
    result = Client_InstallOrJoinEntryGateAt(entry);
    if (result != CASTLE_OK || g_client_state != CASTLE_CLIENT_GATE_FOLLOWER) {
        ExitProcess(5u);
    }
    if (Client_RestoreKnownEntryGateAt(entry) != CASTLE_OK ||
        !entry_equal_(entry, original, CASTLE_RPG_ENTRY_PATCH_SIZE)) ExitProcess(6u);

    entry[0] = 0xCCu;
    entry[1] = 0xCCu;
    entry[2] = 0xCCu;
    entry[3] = 0xCCu;
    entry[4] = 0xCCu;
    InterlockedExchange(&g_client_state, CASTLE_CLIENT_COLD);
    if (Client_InstallOrJoinEntryGateAt(entry) != CASTLE_ERROR_ENTRY_GATE_CONFLICT ||
        g_client_state != CASTLE_CLIENT_PLUGIN_FAILED) ExitProcess(7u);

    InterlockedExchange(&g_client_state, CASTLE_CLIENT_COLD);
    if (Client_InstallOrJoinEntryGateAt((BYTE*)(ULONG_PTR)1u) !=
            CASTLE_ERROR_LATE_LOAD_UNSUPPORTED ||
        g_client_state != CASTLE_CLIENT_LATE_LOAD_UNSUPPORTED) ExitProcess(8u);

    VirtualFree(entry, 0u, MEM_RELEASE);
    ExitProcess(0u);
}
