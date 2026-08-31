#include "platform.h"
#include "runtime.h"
#include "mouse_input.h"
#include "pad_bridge.h"
#include "backlog.h"
#include "CastleRuntime_Client.h"
#include "CastleSchedule_API.h"

/*
 * plugin.c
 *
 * DllMain 只安装 SDK Entry Gate。真正初始化由 Client 在 RPG 入口、Loader Lock 外完成：
 * - StandaloneHost 使用原本的本地 Hook/WndProc/8ms worker；
 * - RuntimeHost 使用 Hook/Window/Schedule 三个公共服务；
 * - 两种 Host 最终调用完全相同的 Backlog_PollInput 业务核心。
 */

static HMODULE g_plugin_module;
static volatile LONG g_worker_running;
static const CastleScheduleApiV1* g_schedule_api;
static CastleTaskHandle g_schedule_task;

static CastleStringView plugin_view_(const char* text, CastleU32 length) {
    CastleStringView view;
    view.data = text;
    view.length = length;
    return view;
}

static const CastleScheduleApiV1* query_schedule_(
    const CastleRuntimeApiV1* runtime_api) {
    static const char interface_id[] = CASTLE_SCHEDULE_INTERFACE_ID;
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 result = {0};
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = plugin_view_(interface_id,
        (CastleU32)(sizeof(interface_id) - 1u));
    query.requested_version = CASTLE_SCHEDULE_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_SCHEDULE_API_V1;
    query.required_capabilities_low = CASTLE_SCHEDULE_CAP_BACKGROUND;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (!runtime_api || runtime_api->QueryInterface(&query, &result) != CASTLE_OK) {
        return NULL;
    }
    return (const CastleScheduleApiV1*)result.api_pointer;
}

static void poll_business_once_(void) {
    MouseInput_Poll();
    PadBridge_Poll();
    Backlog_PollInput();
}

static CastleResult CASTLE_RUNTIME_CALL Backlog_ScheduledPoll(
    CastleTaskHandle task, void* user_context) {
    (void)task;
    (void)user_context;
    poll_business_once_();
    return CASTLE_OK;
}

static DWORD WINAPI StandaloneWorker(void* unused) {
    (void)unused;
    while (InterlockedCompareExchange(&g_worker_running, 1, 1) != 0) {
        poll_business_once_();
        Sleep(BACKLOG_WORKER_SLEEP_MS);
    }
    MouseInput_Shutdown();
    PadBridge_Shutdown();
    Backlog_Shutdown();
    Runtime_Log("[退出] Backlog Standalone worker 已停止。");
    return 0u;
}

static CastleResult initialize_standalone_(void) {
    HANDLE thread;
    const RuntimeConfig* config;
    if (!Runtime_Initialize(g_plugin_module)) return CASTLE_ERROR_UNKNOWN_GAME_BUILD;
    config = Runtime_Config();
    if (!config->enabled) return CASTLE_OK;
    if (!Backlog_Install()) return CASTLE_ERROR_EXPECTED_BYTES;
    MouseInput_Initialize();
    PadBridge_Initialize();
    InterlockedExchange(&g_worker_running, 1);
    thread = CreateThread(NULL, 0u, StandaloneWorker, NULL, 0u, NULL);
    if (!thread) {
        InterlockedExchange(&g_worker_running, 0);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    CloseHandle(thread);
    Runtime_Log("[启动] Backlog StandaloneHost 已就绪。");
    return CASTLE_OK;
}

static CastleResult initialize_integrated_(const CastleRuntimeApiV1* runtime_api,
                                           CastlePluginHandle plugin_handle) {
    static const char task_label[] = "Backlog 8ms input poll";
    CastleScheduledTaskV1 task = {0};
    const RuntimeConfig* config;
    if (!Runtime_InitializeIntegrated(g_plugin_module, runtime_api, plugin_handle)) {
        return CASTLE_ERROR_UNKNOWN_GAME_BUILD;
    }
    config = Runtime_Config();
    if (!config->enabled) return CASTLE_OK;
    if (Backlog_InstallIntegrated(runtime_api, plugin_handle) < 0) {
        return CASTLE_ERROR_EXPECTED_BYTES;
    }
    if (!MouseInput_InitializeIntegrated(runtime_api, plugin_handle)) {
        Runtime_Log("[警告] Runtime Window 注册失败；键盘/Pad 保持可用，鼠标不可用。");
    }
    PadBridge_Initialize();
    g_schedule_api = query_schedule_(runtime_api);
    if (!g_schedule_api) return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    task.magic = CASTLE_SCHEDULE_TASK_MAGIC;
    task.struct_size = CASTLE_SIZEOF_SCHEDULED_TASK_V1;
    task.version = CASTLE_SCHEDULE_STRUCTURE_VERSION_1;
    task.flags = CASTLE_SCHEDULE_TASK_START_ENABLED;
    task.period_ms = BACKLOG_WORKER_SLEEP_MS;
    task.budget_ms = 4u;
    task.phase = CASTLE_SCHEDULE_PHASE_NORMAL;
    task.priority = CASTLE_SCHEDULE_PRIORITY_DEFAULT;
    task.callback = Backlog_ScheduledPoll;
    task.label = plugin_view_(task_label,
        (CastleU32)(sizeof(task_label) - 1u));
    if (g_schedule_api->RegisterPeriodicTask(plugin_handle, &task,
            &g_schedule_task) != CASTLE_OK) {
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    Runtime_Log("[启动] Backlog RuntimeHost 已就绪：Hook/Window/Schedule 统一协调。");
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL Backlog_Integrated(
    const CastleRuntimeApiV1* runtime_api, CastlePluginHandle plugin_handle,
    void* user_context) {
    (void)user_context;
    return initialize_integrated_(runtime_api, plugin_handle);
}

static CastleResult CASTLE_RUNTIME_CALL Backlog_Standalone(void* user_context) {
    (void)user_context;
    return initialize_standalone_();
}

static void CASTLE_RUNTIME_CALL Backlog_RuntimeFault(CastleResult failure,
                                                     void* user_context) {
    (void)user_context;
    Runtime_Initialize(g_plugin_module);
    Runtime_Log("[失败] Castle_Runtime.dll 存在但不可用；Backlog 未安装私有 Hook/线程。");
    (void)failure;
}

static void CASTLE_RUNTIME_CALL Backlog_ProcessExit(void* user_context) {
    (void)user_context;
    InterlockedExchange(&g_worker_running, 0);
}

static const char g_plugin_id[] = "org.castlereforge.backlog";
static const char g_display_name[] = "Castle Backlog";
static const char g_version_text[] = "0.4.0";
static const char g_build_id[] = "runtimesdk-v1";
static const CastlePluginDescriptorV1 g_plugin_descriptor = {
    CASTLE_PLUGIN_DESC_MAGIC,
    CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_SUPPORTS_STANDALONE |
        CASTLE_PLUGIN_FLAG_REQUESTS_HOOKS |
        CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE,
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
    Backlog_Integrated,
    Backlog_Standalone,
    Backlog_RuntimeFault,
    Backlog_ProcessExit,
    NULL
};
static CastlePluginExportV1 g_plugin_export = {
    CASTLE_PLUGIN_QUERY_MAGIC,
    CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1,
    0u,
    &g_plugin_descriptor,
    &g_client_config,
    0u,
    NULL
};

const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requested_version) {
    return requested_version == CASTLE_PLUGIN_EXPORT_VERSION_1 ?
        &g_plugin_export : NULL;
}

void __cdecl InitializeASI(void) {
    CastleRuntimeClient_RunNow();
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_plugin_module = (HMODULE)instance;
        DisableThreadLibraryCalls(instance);
        CastleRuntimeClient_OnProcessAttach((CastleModule)(SIZE_T)instance,
                                             &g_plugin_export);
    } else if (reason == DLL_PROCESS_DETACH) {
        CastleRuntimeClient_OnProcessDetach(reserved);
    }
    return TRUE;
}
