/*
 * bootstrap_plugin_test.c
 *
 * 同一源码分别构建为 A/B 两个测试 ASI。测试宿主故意先加载 B 再加载 A，Runtime
 * 必须仍按稳定 plugin_id 先激活 A。这个 DLL 不安装 Hook，只记录 Bootstrap 参数。
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "CastleRuntime_API.h"
#include "CastlePlugin_API.h"
#include "CastleSchedule_API.h"

#if defined(TEST_PLUGIN_A)
static const char g_plugin_id[] = "org.castlereforge.test.bootstrap.a";
static const char g_display_name[] = "Bootstrap Test A";
#define TEST_ORDER_TOKEN 1u
#else
static const char g_plugin_id[] = "org.castlereforge.test.bootstrap.b";
static const char g_display_name[] = "Bootstrap Test B";
#define TEST_ORDER_TOKEN 2u
#endif

static const char g_version_text[] = "1.0.0";
static const char g_build_id[] = "bootstrap-test";
static CastleU32 g_bootstrap_count;
static CastleU32 g_bootstrap_mode;
static CastlePluginHandle g_bootstrap_handle;
#if defined(TEST_PLUGIN_A)
static volatile LONG g_schedule_count;
static volatile LONG g_schedule_early_count;
static CastleTaskHandle g_schedule_task;
typedef CastleU32 (CASTLE_RUNTIME_CALL *RuntimeTestGetU32Fn)(void);
static RuntimeTestGetU32Fn g_get_bootstrap_order_count;
#endif

typedef void (CASTLE_RUNTIME_CALL *RuntimeTestRecordBootstrapFn)(CastleU32 token);

/* A/B 都要解析宿主导出，所以函数地址复制助手必须位于条件编译块之外。 */
static int test_copy_proc_(void* output, CastleU32 output_size, FARPROC address) {
    volatile CastleU8* output_bytes = (volatile CastleU8*)output;
    const volatile CastleU8* input_bytes = (const volatile CastleU8*)&address;
    CastleU32 index;
    if (!output || !address || output_size != (CastleU32)sizeof(address)) return 0;
    for (index = 0u; index < (CastleU32)sizeof(address); ++index) {
        output_bytes[index] = input_bytes[index];
    }
    return 1;
}

#if defined(TEST_PLUGIN_A)
static CastleStringView test_view_(const char* text, CastleU32 length) {
    CastleStringView view;
    view.data = text;
    view.length = length;
    return view;
}

static void test_zero_(void* memory, CastleU32 size) {
    volatile CastleU8* bytes = (volatile CastleU8*)memory;
    CastleU32 index;
    /* volatile 逐字节写入，防止无 CRT 测试 DLL 被编译器重新折叠成 memset。 */
    for (index = 0u; index < size; ++index) bytes[index] = 0u;
}

static CastleResult CASTLE_RUNTIME_CALL test_schedule_callback_(
    CastleTaskHandle task, void* user_context) {
    (void)task;
    (void)user_context;
    /* B 插件尚未登记时执行，说明 Schedule 在整批 Bootstrap 完成前抢跑。 */
    if (!g_get_bootstrap_order_count || g_get_bootstrap_order_count() < 2u) {
        InterlockedIncrement(&g_schedule_early_count);
    }
    InterlockedIncrement(&g_schedule_count);
    return CASTLE_OK;
}

/*
 * A 插件在 Bootstrap 回调内部登记一个 1ms 周期任务，B 随后故意延迟初始化。
 * 任务只能在 A/B 都返回后开始，不能依赖不会再次执行的 ModLoader Entry Gate。
 */
static CastleResult test_register_schedule_(const CastleRuntimeApiV1* runtime_api,
                                            CastlePluginHandle plugin_handle) {
    static const char interface_id[] = CASTLE_SCHEDULE_INTERFACE_ID;
    static const char task_label[] = "Bootstrap gate schedule probe";
    CastleInterfaceQueryV1 query;
    CastleInterfaceResultV1 result;
    CastleScheduledTaskV1 task;
    const CastleScheduleApiV1* schedule_api;
    HMODULE host_module;
    FARPROC get_order_address;

    /* 测试 DLL 同样按无 CRT 规则清零，避免把测试通过建立在隐式运行库上。 */
    test_zero_(&query, (CastleU32)sizeof(query));
    test_zero_(&result, (CastleU32)sizeof(result));
    test_zero_(&task, (CastleU32)sizeof(task));
    host_module = GetModuleHandleW(NULL);
    get_order_address = host_module ? GetProcAddress(host_module,
        "RuntimeTest_GetBootstrapOrderCount") : NULL;
    if (!test_copy_proc_(&g_get_bootstrap_order_count,
            (CastleU32)sizeof(g_get_bootstrap_order_count), get_order_address)) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = test_view_(interface_id,
        (CastleU32)(sizeof(interface_id) - 1u));
    query.requested_version = CASTLE_SCHEDULE_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_SCHEDULE_API_V1;
    query.required_capabilities_low = CASTLE_SCHEDULE_CAP_BACKGROUND;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (runtime_api->QueryInterface(&query, &result) != CASTLE_OK ||
        !result.api_pointer) return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    schedule_api = (const CastleScheduleApiV1*)result.api_pointer;

    task.magic = CASTLE_SCHEDULE_TASK_MAGIC;
    task.struct_size = CASTLE_SIZEOF_SCHEDULED_TASK_V1;
    task.version = CASTLE_SCHEDULE_STRUCTURE_VERSION_1;
    task.flags = CASTLE_SCHEDULE_TASK_START_ENABLED;
    task.period_ms = 1u;
    task.budget_ms = 10u;
    task.phase = CASTLE_SCHEDULE_PHASE_NORMAL;
    task.priority = CASTLE_SCHEDULE_PRIORITY_DEFAULT;
    task.callback = test_schedule_callback_;
    task.label = test_view_(task_label,
        (CastleU32)(sizeof(task_label) - 1u));
    return schedule_api->RegisterPeriodicTask(plugin_handle, &task,
                                               &g_schedule_task);
}
#endif

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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static CastleResult CASTLE_RUNTIME_CALL test_client_bootstrap_(
    CastleU32 mode,
    const CastleRuntimeApiV1* runtime_api,
    CastlePluginHandle plugin_handle,
    CastleResult reason) {
    HMODULE host_module;
    RuntimeTestRecordBootstrapFn record;
    (void)reason;
    if (mode != CASTLE_CLIENT_BOOTSTRAP_INTEGRATED || !runtime_api ||
        plugin_handle == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    g_bootstrap_mode = mode;
    g_bootstrap_handle = plugin_handle;
    ++g_bootstrap_count;
    host_module = GetModuleHandleW(NULL);
    record = NULL;
    if (!host_module || !test_copy_proc_(&record, (CastleU32)sizeof(record),
            GetProcAddress(host_module, "RuntimeTest_RecordBootstrap"))) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
#if !defined(TEST_PLUGIN_A)
    /* 故意拉长 A、B 之间的窗口，让任何“登记即运行”的旧调度器稳定暴露抢跑。 */
    Sleep(100u);
#endif
    record(TEST_ORDER_TOKEN);
#if defined(TEST_PLUGIN_A)
    if (test_register_schedule_(runtime_api, plugin_handle) != CASTLE_OK) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
#endif
    return CASTLE_OK;
}

static const CastlePluginExportV1 g_plugin_export = {
    CASTLE_PLUGIN_QUERY_MAGIC,
    CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1,
    0u,
    &g_descriptor,
    &g_client_config,
    0u,
    test_client_bootstrap_
};

const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requested_export_version) {
    return requested_export_version == CASTLE_PLUGIN_EXPORT_VERSION_1 ?
        &g_plugin_export : NULL;
}

CastleU32 CASTLE_RUNTIME_CALL RuntimeTest_GetBootstrapCount(void) {
    return g_bootstrap_count;
}

CastleU32 CASTLE_RUNTIME_CALL RuntimeTest_GetBootstrapMode(void) {
    return g_bootstrap_mode;
}

CastlePluginHandle CASTLE_RUNTIME_CALL RuntimeTest_GetBootstrapHandle(void) {
    return g_bootstrap_handle;
}

CastleU32 CASTLE_RUNTIME_CALL RuntimeTest_GetScheduleCount(void) {
#if defined(TEST_PLUGIN_A)
    return (CastleU32)InterlockedCompareExchange(&g_schedule_count, 0, 0);
#else
    return 0u;
#endif
}

CastleU32 CASTLE_RUNTIME_CALL RuntimeTest_GetScheduleEarlyCount(void) {
#if defined(TEST_PLUGIN_A)
    return (CastleU32)InterlockedCompareExchange(&g_schedule_early_count, 0, 0);
#else
    return 0u;
#endif
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    (void)instance;
    (void)reason;
    (void)reserved;
    return TRUE;
}
