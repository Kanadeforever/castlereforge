/*
 * client_state_test.c
 *
 * 独立验证每个 ASI 内嵌 Client 的单模块状态机。重点是两个线程同时进入整合初始化时，
 * 业务回调只能执行一次；另外覆盖 Standalone、Runtime Fault、重复调用和进程退出。
 */

#include "../client/client_internal.h"

typedef struct ClientStateTestContext {
    HANDLE initialize_started;
    HANDLE allow_initialize_finish;
    volatile LONG integrated_count;
    volatile LONG standalone_count;
    volatile LONG fault_count;
    volatile LONG process_exit_count;
    CastleResult last_fault;
} ClientStateTestContext;

typedef struct ClientThreadCall {
    const CastleRuntimeApiV1* runtime_api;
    CastleResult result;
} ClientThreadCall;

static CastleResult CASTLE_RUNTIME_CALL test_integrated_initialize_(
    const CastleRuntimeApiV1* runtime_api,
    CastlePluginHandle plugin_handle,
    void* user_context) {
    ClientStateTestContext* context = (ClientStateTestContext*)user_context;
    if (!runtime_api || plugin_handle == 0u || !context) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    InterlockedIncrement(&context->integrated_count);
    SetEvent(context->initialize_started);
    if (WaitForSingleObject(context->allow_initialize_finish, 2000u) != WAIT_OBJECT_0) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL test_standalone_initialize_(void* user_context) {
    ClientStateTestContext* context = (ClientStateTestContext*)user_context;
    if (!context) return CASTLE_ERROR_INVALID_ARGUMENT;
    InterlockedIncrement(&context->standalone_count);
    return CASTLE_OK;
}

static void CASTLE_RUNTIME_CALL test_runtime_fault_(CastleResult failure,
                                                    void* user_context) {
    ClientStateTestContext* context = (ClientStateTestContext*)user_context;
    if (!context) return;
    context->last_fault = failure;
    InterlockedIncrement(&context->fault_count);
}

static void CASTLE_RUNTIME_CALL test_process_exit_(void* user_context) {
    ClientStateTestContext* context = (ClientStateTestContext*)user_context;
    if (context) InterlockedIncrement(&context->process_exit_count);
}

static DWORD WINAPI test_integrated_thread_(LPVOID parameter) {
    ClientThreadCall* call = (ClientThreadCall*)parameter;
    call->result = Client_BootstrapPlugin(CASTLE_CLIENT_BOOTSTRAP_INTEGRATED,
        call->runtime_api, 77u, CASTLE_OK);
    return 0u;
}

static void test_zero_(void* memory, CastleU32 size) {
    volatile CastleU8* bytes = (volatile CastleU8*)memory;
    CastleU32 index;
    for (index = 0u; index < size; ++index) bytes[index] = 0u;
}

__declspec(noreturn) void __stdcall ClientStateTestEntry(void) {
    ClientStateTestContext context;
    CastleRuntimeClientConfigV1 config;
    CastlePluginExportV1 plugin_export;
    CastleRuntimeApiV1 fake_runtime_api;
    ClientThreadCall first_call;
    ClientThreadCall second_call;
    HANDLE first_thread;
    HANDLE second_thread;
    CastleResult result;

    test_zero_(&context, (CastleU32)sizeof(context));
    test_zero_(&config, (CastleU32)sizeof(config));
    test_zero_(&plugin_export, (CastleU32)sizeof(plugin_export));
    test_zero_(&fake_runtime_api, (CastleU32)sizeof(fake_runtime_api));
    test_zero_(&first_call, (CastleU32)sizeof(first_call));
    test_zero_(&second_call, (CastleU32)sizeof(second_call));

    context.initialize_started = CreateEventW(NULL, TRUE, FALSE, NULL);
    context.allow_initialize_finish = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!context.initialize_started || !context.allow_initialize_finish) ExitProcess(1u);
    config.magic = CASTLE_CLIENT_CONFIG_MAGIC;
    config.struct_size = CASTLE_SIZEOF_CLIENT_CONFIG_V1;
    config.config_version = CASTLE_CLIENT_CONFIG_VERSION_1;
    config.integrated_initialize = test_integrated_initialize_;
    config.standalone_initialize = test_standalone_initialize_;
    config.runtime_fault = test_runtime_fault_;
    config.process_exit = test_process_exit_;
    config.user_context = &context;
    plugin_export.magic = CASTLE_PLUGIN_QUERY_MAGIC;
    plugin_export.struct_size = CASTLE_SIZEOF_PLUGIN_EXPORT_V1;
    plugin_export.export_version = CASTLE_PLUGIN_EXPORT_VERSION_1;
    plugin_export.client_config = &config;
    g_client_module = GetModuleHandleW(NULL);
    g_client_export = &plugin_export;

    InterlockedExchange(&g_client_state, CASTLE_CLIENT_GATE_FOLLOWER);
    first_call.runtime_api = &fake_runtime_api;
    second_call.runtime_api = &fake_runtime_api;
    first_thread = CreateThread(NULL, 0u, test_integrated_thread_, &first_call, 0u, NULL);
    if (!first_thread ||
        WaitForSingleObject(context.initialize_started, 2000u) != WAIT_OBJECT_0) {
        ExitProcess(2u);
    }
    second_thread = CreateThread(NULL, 0u, test_integrated_thread_, &second_call, 0u, NULL);
    if (!second_thread) ExitProcess(3u);
    Sleep(30u);
    if (context.integrated_count != 1) ExitProcess(4u);
    SetEvent(context.allow_initialize_finish);
    if (WaitForSingleObject(first_thread, 2000u) != WAIT_OBJECT_0 ||
        WaitForSingleObject(second_thread, 2000u) != WAIT_OBJECT_0 ||
        first_call.result != CASTLE_OK ||
        second_call.result != CASTLE_STATUS_ALREADY_DONE ||
        context.integrated_count != 1 ||
        CastleRuntimeClient_GetState() != CASTLE_CLIENT_INTEGRATED) ExitProcess(5u);
    CloseHandle(first_thread);
    CloseHandle(second_thread);

    InterlockedExchange(&g_client_state, CASTLE_CLIENT_GATE_OWNER);
    result = Client_BootstrapPlugin(CASTLE_CLIENT_BOOTSTRAP_STANDALONE,
        NULL, 0u, CASTLE_OK);
    if (result != CASTLE_OK || context.standalone_count != 1 ||
        CastleRuntimeClient_GetState() != CASTLE_CLIENT_STANDALONE ||
        Client_BootstrapPlugin(CASTLE_CLIENT_BOOTSTRAP_STANDALONE,
            NULL, 0u, CASTLE_OK) != CASTLE_STATUS_ALREADY_DONE ||
        context.standalone_count != 1) ExitProcess(6u);

    InterlockedExchange(&g_client_state, CASTLE_CLIENT_GATE_FOLLOWER);
    result = Client_BootstrapPlugin(CASTLE_CLIENT_BOOTSTRAP_FAULT,
        NULL, 0u, CASTLE_ERROR_ABI_MISMATCH);
    if (result != CASTLE_ERROR_ABI_MISMATCH || context.fault_count != 1 ||
        context.last_fault != CASTLE_ERROR_ABI_MISMATCH ||
        CastleRuntimeClient_GetState() != CASTLE_CLIENT_RUNTIME_FAULT) ExitProcess(7u);

    InterlockedExchange(&g_client_state, CASTLE_CLIENT_COLD);
    if (Client_BootstrapPlugin(CASTLE_CLIENT_BOOTSTRAP_INTEGRATED,
            &fake_runtime_api, 77u, CASTLE_OK) != CASTLE_ERROR_TRANSACTION_STATE) {
        ExitProcess(8u);
    }

    InterlockedExchange(&g_client_state, CASTLE_CLIENT_INTEGRATED);
    CastleRuntimeClient_OnProcessDetach((void*)1);
    if (CastleRuntimeClient_GetState() != CASTLE_CLIENT_PROCESS_EXIT ||
        context.process_exit_count != 1) ExitProcess(9u);

    CloseHandle(context.initialize_started);
    CloseHandle(context.allow_initialize_finish);
    ExitProcess(0u);
}
