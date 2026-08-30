/*
 * client_bootstrap_test.c
 *
 * 同一测试插件分别放进 integrated/standalone/fault 三个目录运行，只改变同目录
 * Castle_Runtime.dll 的状态，证明 Client 的三路选择不会互相偷换。
 */

#include "../client/client_internal.h"

typedef struct ClientBootstrapTestContext {
    volatile LONG integrated_count;
    volatile LONG standalone_count;
    volatile LONG fault_count;
    CastleResult last_fault;
    CastlePluginHandle integrated_handle;
} ClientBootstrapTestContext;

static ClientBootstrapTestContext g_test_context;
static const char g_plugin_id[] = "org.castlereforge.test.client.bootstrap";
static const char g_display_name[] = "Client Bootstrap Test";
static const char g_version_text[] = "1.0.0";
static const char g_build_id[] = "client-bootstrap-test";

static CastleResult CASTLE_RUNTIME_CALL client_test_integrated_(
    const CastleRuntimeApiV1* runtime_api,
    CastlePluginHandle plugin_handle,
    void* user_context) {
    ClientBootstrapTestContext* context = (ClientBootstrapTestContext*)user_context;
    if (!runtime_api || !context || plugin_handle == 0u) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    context->integrated_handle = plugin_handle;
    InterlockedIncrement(&context->integrated_count);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL client_test_standalone_(void* user_context) {
    ClientBootstrapTestContext* context = (ClientBootstrapTestContext*)user_context;
    if (!context) return CASTLE_ERROR_INVALID_ARGUMENT;
    InterlockedIncrement(&context->standalone_count);
    return CASTLE_OK;
}

static void CASTLE_RUNTIME_CALL client_test_fault_(CastleResult failure,
                                                   void* user_context) {
    ClientBootstrapTestContext* context = (ClientBootstrapTestContext*)user_context;
    if (!context) return;
    context->last_fault = failure;
    InterlockedIncrement(&context->fault_count);
}

static const CastlePluginDescriptorV1 g_descriptor = {
    CASTLE_PLUGIN_DESC_MAGIC,
    CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_SUPPORTS_STANDALONE,
    0u,
    {g_plugin_id, (CastleU32)(sizeof(g_plugin_id) - 1u)},
    {g_display_name, (CastleU32)(sizeof(g_display_name) - 1u)},
    {g_version_text, (CastleU32)(sizeof(g_version_text) - 1u)},
    {g_build_id, (CastleU32)(sizeof(g_build_id) - 1u)}
};

static const CastleRuntimeClientConfigV1 g_client_config = {
    CASTLE_CLIENT_CONFIG_MAGIC,
    CASTLE_SIZEOF_CLIENT_CONFIG_V1,
    CASTLE_CLIENT_CONFIG_VERSION_1,
    0u,
    client_test_integrated_,
    client_test_standalone_,
    client_test_fault_,
    NULL,
    &g_test_context
};

static CastlePluginExportV1 g_plugin_export = {
    CASTLE_PLUGIN_QUERY_MAGIC,
    CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1,
    0u,
    &g_descriptor,
    &g_client_config,
    0u,
    Client_BootstrapPlugin
};

const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requested_export_version) {
    return requested_export_version == CASTLE_PLUGIN_EXPORT_VERSION_1 ?
        &g_plugin_export : NULL;
}

static int client_wide_contains_(const WCHAR* text, const WCHAR* pattern) {
    CastleU32 start;
    CastleU32 index;
    if (!text || !pattern || !pattern[0]) return 0;
    for (start = 0u; text[start]; ++start) {
        for (index = 0u; pattern[index] && text[start + index] == pattern[index]; ++index) {
        }
        if (!pattern[index]) return 1;
    }
    return 0;
}

static void client_test_zero_(void* memory, CastleU32 size) {
    volatile CastleU8* bytes = (volatile CastleU8*)memory;
    CastleU32 index;
    for (index = 0u; index < size; ++index) bytes[index] = 0u;
}

__declspec(noreturn) void __stdcall ClientBootstrapTestEntry(void) {
    static const BYTE original[CASTLE_RPG_ENTRY_PATCH_SIZE] = {
        0x55u, 0x8Bu, 0xECu, 0x6Au, 0xFFu
    };
    static const WCHAR integrated_marker[] = L"client_integrated";
    static const WCHAR standalone_marker[] = L"client_standalone";
    static const WCHAR fault_marker[] = L"client_fault";
    WCHAR module_path[1024];
    BYTE* entry;
    CastleU32 index;
    CastleU32 expected_mode;
    CastleResult result;

    if (GetModuleFileNameW(NULL, module_path, 1024u) == 0u) ExitProcess(1u);
    if (client_wide_contains_(module_path, integrated_marker)) {
        expected_mode = CASTLE_CLIENT_BOOTSTRAP_INTEGRATED;
    } else if (client_wide_contains_(module_path, standalone_marker)) {
        expected_mode = CASTLE_CLIENT_BOOTSTRAP_STANDALONE;
    } else if (client_wide_contains_(module_path, fault_marker)) {
        expected_mode = CASTLE_CLIENT_BOOTSTRAP_FAULT;
    } else {
        ExitProcess(2u);
    }

    client_test_zero_(&g_test_context, (CastleU32)sizeof(g_test_context));
    entry = (BYTE*)VirtualAlloc(NULL, 4096u, MEM_RESERVE | MEM_COMMIT,
                                PAGE_EXECUTE_READWRITE);
    if (!entry) ExitProcess(3u);
    for (index = 0u; index < CASTLE_RPG_ENTRY_PATCH_SIZE; ++index) {
        entry[index] = original[index];
    }
    g_client_module = GetModuleHandleW(NULL);
    g_client_export = &g_plugin_export;
    g_plugin_export.entry_gate_thunk =
        (CastleAddress)(ULONG_PTR)&CastleRuntimeClient_EntryGateThunk;
    InterlockedExchange(&g_client_state, CASTLE_CLIENT_COLD);
    if (Client_InstallOrJoinEntryGateAt(entry) != CASTLE_OK ||
        CastleRuntimeClient_GetState() != CASTLE_CLIENT_GATE_OWNER) ExitProcess(4u);

    result = CastleRuntimeClient_BootstrapAll(CASTLE_BOOTSTRAP_TRIGGER_ENTRY_GATE,
        (CastleModule)(ULONG_PTR)g_client_module);
    if (expected_mode == CASTLE_CLIENT_BOOTSTRAP_INTEGRATED) {
        if (result < 0 || g_test_context.integrated_count != 1 ||
            g_test_context.standalone_count != 0 || g_test_context.fault_count != 0 ||
            g_test_context.integrated_handle == 0u ||
            CastleRuntimeClient_GetState() != CASTLE_CLIENT_INTEGRATED ||
            !GetModuleHandleW(L"Castle_Runtime.dll")) ExitProcess(5u);
    } else if (expected_mode == CASTLE_CLIENT_BOOTSTRAP_STANDALONE) {
        if (result < 0 || g_test_context.integrated_count != 0 ||
            g_test_context.standalone_count != 1 || g_test_context.fault_count != 0 ||
            CastleRuntimeClient_GetState() != CASTLE_CLIENT_STANDALONE ||
            GetModuleHandleW(L"Castle_Runtime.dll")) ExitProcess(6u);
    } else {
        if (result >= 0 || g_test_context.integrated_count != 0 ||
            g_test_context.standalone_count != 0 || g_test_context.fault_count != 1 ||
            g_test_context.last_fault >= 0 ||
            CastleRuntimeClient_GetState() != CASTLE_CLIENT_RUNTIME_FAULT ||
            GetModuleHandleW(L"Castle_Runtime.dll")) ExitProcess(7u);
    }

    /* 再次触发不能重复业务回调，也不能从 Fault 偷换成 Standalone。 */
    CastleRuntimeClient_RunNow();
    if (g_test_context.integrated_count > 1 || g_test_context.standalone_count > 1 ||
        g_test_context.fault_count > 1) ExitProcess(8u);
    VirtualFree(entry, 0u, MEM_RELEASE);
    ExitProcess(0u);
}
