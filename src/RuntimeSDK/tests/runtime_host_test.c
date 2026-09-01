/*
 * runtime_host_test.c
 *
 * 这是一个独立 x86 无 CRT 测试进程，不是游戏插件。
 * 它从自身目录加载刚构建的 Castle_Runtime.dll，检查根 ABI、插件登记、诊断和 Bootstrap 幂等性。
 * 任一步失败都用不同 ExitProcess 代码退出，build.bat 可以直接定位失败阶段。
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "CastleRuntime_API.h"
#include "CastlePlugin_API.h"
#include "CastleHook_API.h"
#include "CastlePath_API.h"
#include "CastleSchedule_API.h"
#include "CastleDisplay_API.h"
#include "CastleWindow_API.h"
#include "CastleRender_API.h"

typedef int (__cdecl *TestFunction)(int value);
typedef CastleU32 (CASTLE_RUNTIME_CALL *RuntimeTestGetU32Fn)(void);

static CastleU8 g_patch_data[4] = {1u, 2u, 3u, 4u};
static CastleU8 g_state_data[2] = {0x10u, 0x20u};
static CastleU8 g_rollback_a[2] = {0x31u, 0x32u};
static CastleU8 g_rollback_b[2] = {0x41u, 0x42u};
static TestFunction g_pointer_slot;
static void* volatile* g_hook_one_next;
static void* volatile* g_hook_two_next;
static CastleDisplayGeometryV1 g_test_display_geometry;
static CastleU32 g_test_display_ready;
static CastleU32 g_test_render_ready;
static volatile LONG g_test_render_count;
static volatile LONG g_test_present_count;
static CastleRenderCallV1 g_test_last_render_call;
static volatile LONG g_bootstrap_order_count;
static CastleU32 g_bootstrap_order[4];

typedef struct ScheduleTestContext {
    HANDLE event_handle;
    volatile LONG call_count;
    DWORD callback_thread_id;
    LONG signal_after;
    CastleResult callback_result;
    DWORD sleep_ms;
} ScheduleTestContext;

typedef struct WindowTestContext {
    volatile LONG sequence;
    volatile LONG observer_order;
    volatile LONG filter_order;
    volatile LONG observer_count;
    volatile LONG filter_count;
    CastleU32 target_message;
    CastleS32 consumed_result;
} WindowTestContext;

static int __cdecl test_original_(int value) {
    return value + 1;
}

static int __cdecl test_hook_one_(int value) {
    TestFunction next = g_hook_one_next ? (TestFunction)*g_hook_one_next : NULL;
    return next ? next(value) + 10 : -1000;
}

static int __cdecl test_hook_two_(int value) {
    TestFunction next = g_hook_two_next ? (TestFunction)*g_hook_two_next : NULL;
    return next ? next(value) + 100 : -2000;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_test_callback_(
    CastleTaskHandle task, void* user_context) {
    ScheduleTestContext* context = (ScheduleTestContext*)user_context;
    LONG count;
    (void)task;
    if (!context) return CASTLE_ERROR_INVALID_ARGUMENT;
    context->callback_thread_id = GetCurrentThreadId();
    if (context->sleep_ms != 0u) Sleep(context->sleep_ms);
    count = InterlockedIncrement(&context->call_count);
    if (context->event_handle && count >= context->signal_after) {
        SetEvent(context->event_handle);
    }
    return context->callback_result;
}

static CastleResult CASTLE_RUNTIME_CALL test_display_get_state_(
    CastleDisplayStateV1* out_state) {
    if (!out_state) return CASTLE_ERROR_INVALID_ARGUMENT;
    out_state->flags = 0u;
    out_state->ready = g_test_display_ready;
    out_state->generation = g_test_display_geometry.generation;
    out_state->backend_plugin = 0u;
    out_state->display_mode = g_test_display_geometry.display_mode;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL test_display_copy_geometry_(
    CastleDisplayGeometryV1* out_geometry) {
    if (!out_geometry) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_geometry = g_test_display_geometry;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL test_display_world_to_screen_(
    const CastleWorldToScreenRequestV1* request,
    CastleScreenProjectionV1* out_projection) {
    if (!request || !out_projection ||
        request->requested_generation != g_test_display_geometry.generation) {
        return CASTLE_ERROR_STALE_GENERATION;
    }
    out_projection->flags = 0u;
    out_projection->actual_generation = g_test_display_geometry.generation;
    out_projection->screen_x = request->world_x -
        g_test_display_geometry.effective_camera_x + g_test_display_geometry.center_x;
    out_projection->screen_y = request->world_y -
        g_test_display_geometry.effective_camera_y + g_test_display_geometry.center_y;
    out_projection->visibility = out_projection->screen_x >= 0 &&
        (CastleU32)out_projection->screen_x < g_test_display_geometry.output_width &&
        out_projection->screen_y >= 0 &&
        (CastleU32)out_projection->screen_y < g_test_display_geometry.output_height ?
        CASTLE_VISIBILITY_VISIBLE : CASTLE_VISIBILITY_NOT_PROJECTABLE;
    out_projection->projection_scope = g_test_display_geometry.projection_scope;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL test_display_screen_to_world_(
    const CastleScreenToWorldRequestV1* request,
    CastleWorldProjectionV1* out_projection) {
    if (!request || !out_projection ||
        request->requested_generation != g_test_display_geometry.generation) {
        return CASTLE_ERROR_STALE_GENERATION;
    }
    out_projection->flags = 0u;
    out_projection->actual_generation = g_test_display_geometry.generation;
    out_projection->world_x = request->screen_x - g_test_display_geometry.center_x +
                              g_test_display_geometry.effective_camera_x;
    out_projection->world_y = request->screen_y - g_test_display_geometry.center_y +
                              g_test_display_geometry.effective_camera_y;
    out_projection->visibility = CASTLE_VISIBILITY_VISIBLE;
    out_projection->projection_scope = g_test_display_geometry.projection_scope;
    return CASTLE_OK;
}

static const CastleDisplayProviderV1 g_test_display_provider = {
    CASTLE_DISPLAY_PROVIDER_MAGIC,
    CASTLE_SIZEOF_DISPLAY_PROVIDER_V1,
    CASTLE_DISPLAY_API_VERSION_1,
    CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD,
    test_display_get_state_,
    test_display_copy_geometry_,
    test_display_world_to_screen_,
    test_display_screen_to_world_
};

static CastleResult CASTLE_RUNTIME_CALL test_render_get_state_(
    CastleRenderStateV1* out_state) {
    if (!out_state) return CASTLE_ERROR_INVALID_ARGUMENT;
    out_state->flags = 0u;
    out_state->ready = g_test_render_ready;
    out_state->generation = 1u;
    out_state->backend_plugin = 0u;
    out_state->provider_handle = 0u;
    out_state->display_provider_generation = 0u;
    out_state->extra_frame_owner = 0u;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL test_render_queue_(const CastleRenderCallV1* call) {
    if (!call) return CASTLE_ERROR_INVALID_ARGUMENT;
    g_test_last_render_call = *call;
    InterlockedIncrement(&g_test_render_count);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL test_render_present_(const CastleRenderCallV1* call) {
    if (!call) return CASTLE_ERROR_INVALID_ARGUMENT;
    g_test_last_render_call = *call;
    InterlockedIncrement(&g_test_present_count);
    return CASTLE_OK;
}

static const CastleRenderProviderV1 g_test_render_provider = {
    CASTLE_RENDER_PROVIDER_MAGIC,
    CASTLE_SIZEOF_RENDER_PROVIDER_V1,
    CASTLE_RENDER_API_VERSION_1,
    0u,
    test_render_get_state_,
    test_render_queue_,
    test_render_present_
};

/* 由两个测试 ASI 通过宿主导出调用，用于记录跨模块真实激活顺序。 */
void CASTLE_RUNTIME_CALL RuntimeTest_RecordBootstrap(CastleU32 token) {
    LONG position = InterlockedIncrement(&g_bootstrap_order_count) - 1;
    if (position >= 0 && position < 4) g_bootstrap_order[position] = token;
}

CastleU32 CASTLE_RUNTIME_CALL RuntimeTest_GetBootstrapOrderCount(void) {
    return (CastleU32)InterlockedCompareExchange(&g_bootstrap_order_count, 0, 0);
}

static void CASTLE_RUNTIME_CALL test_window_observer_(
    const CastleWindowMessageV1* message, void* user_context) {
    WindowTestContext* context = (WindowTestContext*)user_context;
    if (!message || !context || message->message != context->target_message) return;
    InterlockedIncrement(&context->observer_count);
    context->observer_order = InterlockedIncrement(&context->sequence);
}

static CastleResult CASTLE_RUNTIME_CALL test_window_filter_(
    const CastleWindowMessageV1* message,
    CastleWindowFilterDecisionV1* decision,
    void* user_context) {
    WindowTestContext* context = (WindowTestContext*)user_context;
    if (!message || !decision || !context) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (message->message == context->target_message) {
        InterlockedIncrement(&context->filter_count);
        context->filter_order = InterlockedIncrement(&context->sequence);
        decision->consume = 1u;
        decision->result = context->consumed_result;
    }
    return CASTLE_OK;
}

static int build_sibling_path_(WCHAR* output, DWORD capacity,
                               const WCHAR* file_name) {
    DWORD length;
    DWORD index;
    DWORD file_index;

    if (!output || !file_name || capacity < 32u) return 0;
    length = GetModuleFileNameW(NULL, output, capacity);
    if (length == 0u || length >= capacity) return 0;

    index = length;
    while (index > 0u && output[index - 1u] != L'\\' && output[index - 1u] != L'/') --index;
    if (index == 0u) return 0;

    file_index = 0u;
    while (file_name[file_index]) {
        if (index + file_index + 1u >= capacity) return 0;
        output[index + file_index] = file_name[file_index];
        ++file_index;
    }
    output[index + file_index] = L'\0';
    return 1;
}

static int build_runtime_path_(WCHAR* output, DWORD capacity) {
    static const WCHAR file_name[] = L"Castle_Runtime.dll";
    return build_sibling_path_(output, capacity, file_name);
}

static CastleStringView view_(const char* data, CastleU32 length) {
    CastleStringView value;
    value.data = data;
    value.length = length;
    return value;
}

static CastleWideStringView wide_view_(const WCHAR* data, CastleU32 length) {
    CastleWideStringView value;
    value.data = (const CastleU16*)data;
    value.length = length;
    return value;
}

/* 只比较已知长度，不依赖 CRT strlen/strcmp。 */
static int text_ends_with_(const char* text, CastleU32 text_length,
                           const char* suffix, CastleU32 suffix_length) {
    CastleU32 index;
    if (!text || !suffix || suffix_length > text_length) return 0;
    for (index = 0u; index < suffix_length; ++index) {
        if (text[text_length - suffix_length + index] != suffix[index]) return 0;
    }
    return 1;
}

static void byte_zero_(void* memory, CastleU32 size) {
    /* 测试宿主同样无 CRT，volatile 防止编译器生成 memset 导入。 */
    volatile CastleU8* bytes = (volatile CastleU8*)memory;
    CastleU32 index;
    if (!bytes) return;
    for (index = 0u; index < size; ++index) bytes[index] = 0u;
}

static void byte_copy_(void* destination, const void* source, CastleU32 size) {
    volatile CastleU8* output = (volatile CastleU8*)destination;
    const volatile CastleU8* input = (const volatile CastleU8*)source;
    CastleU32 index;
    if (!output || !input) return;
    for (index = 0u; index < size; ++index) output[index] = input[index];
}

static CastleTargetAddressV1 target_from_address_(HMODULE module, void* address,
                                                  CastleU32 size) {
    CastleTargetAddressV1 target;
    target.module = (CastleModule)(ULONG_PTR)module;
    target.rva = (CastleU32)((BYTE*)address - (BYTE*)module);
    target.size = size;
    return target;
}

__declspec(noreturn) void __stdcall TestEntry(void) {
    WCHAR runtime_path[1024];
    WCHAR bootstrap_plugin_a_path[1024];
    WCHAR bootstrap_plugin_b_path[1024];
    HMODULE runtime_module;
    HMODULE bootstrap_plugin_a = NULL;
    HMODULE bootstrap_plugin_b = NULL;
    CastleRuntimeGetApiFn get_api;
    const CastleRuntimeApiV1* api;
    CastleRuntimeInfoV1 info;
    CastlePluginDescriptorV1 descriptor;
    CastlePluginDescriptorV1 duplicate_descriptor;
    CastlePluginDescriptorV1 second_descriptor;
    CastlePluginDescriptorV1 external_descriptor;
    CastlePluginHandle handle = 0u;
    CastlePluginHandle repeated_handle = 0u;
    CastlePluginHandle duplicate_handle = 0u;
    CastlePluginHandle second_handle = 0u;
    CastlePluginHandle external_handle = 0u;
    CastlePluginStateV1 state;
    CastleInterfaceQueryV1 query;
    CastleInterfaceResultV1 query_result;
    const CastleHookApiV1* hook_api;
    const CastlePathApiV1* path_api;
    const CastleScheduleApiV1* schedule_api;
    const CastleDisplayApiV1* display_api;
    const CastleWindowApiV1* window_api;
    const CastleRenderApiV1* render_api;
    CastlePathInfoV1 path_info;
    CastleScheduledTaskV1 scheduled_task;
    CastleScheduleTaskStatsV1 schedule_stats;
    CastleGamePhaseStateV1 game_phase_state;
    CastleDisplayStateV1 display_state;
    CastleDisplayGeometryV1 display_geometry;
    CastleDisplayProviderInfoV1 display_provider_info;
    CastleWorldToScreenRequestV1 world_request;
    CastleScreenProjectionV1 screen_projection;
    CastleScreenToWorldRequestV1 screen_request;
    CastleWorldProjectionV1 world_projection;
    CastleProviderHandle display_provider = 0u;
    CastleProviderHandle duplicate_display_provider = 0u;
    CastleU32 display_generation = 0u;
    CastleU32 initial_display_provider_generation = 0u;
    CastleWindowClientV1 window_client;
    CastleWindowStateV1 window_state;
    CastleLeaseHandle observer_client = 0u;
    CastleLeaseHandle filter_client = 0u;
    WindowTestContext window_context;
    HWND test_window = NULL;
    LRESULT window_message_result = 0;
    CastleU32 window_generation_before = 0u;
    CastleU32 window_generation_after = 0u;
    CastleRenderStateV1 render_state;
    CastleRenderCallV1 render_call;
    CastleProviderHandle render_provider = 0u;
    CastleProviderHandle duplicate_render_provider = 0u;
    CastleLeaseHandle extra_frame_lease = 0u;
    CastleLeaseHandle duplicate_extra_frame_lease = 0u;
    CastleU32 extra_frame_display_generation = 0u;
    CastleU32 initial_render_provider_generation = 0u;
    CastleU32 gate_wait_attempt;
    CastleTaskHandle schedule_task = 0u;
    ScheduleTestContext periodic_context;
    ScheduleTestContext posted_context;
    CastleTransactionHandle transaction = 0u;
    CastleTransactionHandle conflict_transaction = 0u;
    CastleTransactionHandle rollback_transaction = 0u;
    CastleTransactionHandle chain_transaction_one = 0u;
    CastleTransactionHandle chain_transaction_two = 0u;
    CastleClaimHandle exclusive_claim_handle = 0u;
    CastleClaimHandle state_claim_handle = 0u;
    CastleClaimHandle named_claim_handle = 0u;
    CastleClaimHandle conflict_claim_handle = 0u;
    CastleClaimHandle rollback_claim_a = 0u;
    CastleClaimHandle rollback_claim_b = 0u;
    CastleClaimHandle chain_claim_one = 0u;
    CastleClaimHandle chain_claim_two = 0u;
    CastleExclusivePatchClaimV1 exclusive_claim;
    CastleStatePatchClaimV1 state_claim;
    CastleNamedResourceClaimV1 named_claim;
    CastleExclusivePatchClaimV1 conflict_claim;
    CastleExclusivePatchClaimV1 rollback_a_claim;
    CastleExclusivePatchClaimV1 rollback_b_claim;
    CastleChainHookClaimV1 chain_claim;
    CastleHookBindingV1 binding_one;
    CastleHookBindingV1 binding_two;
    CastleClaimResultV1 claim_result;
    CastleDiagnosticBufferV1 diagnostics;
    CastleBootstrapRequestV1 bootstrap_request;
    CastleBootstrapResultV1 bootstrap_result;
    char diagnostic_text[4096];
    char plugin_path[1024];
    char built_path[1024];
    char alternate_path[1024];
    char normalized_path[1024];
    CastleU16 built_wide[1024];
    CastleU16 normalized_wide[1024];
    CastleU32 path_length = 0u;
    CastleU32 alternate_length = 0u;
    CastleU32 normalized_length = 0u;
    CastleU32 paths_equal = 0u;
    LONG paused_count = 0;
    CastleResult result;
    RuntimeTestGetU32Fn get_a_count;
    RuntimeTestGetU32Fn get_a_mode;
    RuntimeTestGetU32Fn get_a_handle;
    RuntimeTestGetU32Fn get_a_schedule_count;
    RuntimeTestGetU32Fn get_a_schedule_early_count;
    RuntimeTestGetU32Fn get_b_count;
    RuntimeTestGetU32Fn get_b_mode;
    RuntimeTestGetU32Fn get_b_handle;
    static const char plugin_id[] = "org.castlereforge.test.host";
    static const char display_name[] = "Runtime Host Test";
    static const char version_text[] = "1.0.0";
    static const char build_id[] = "abi-core";
    static const char second_plugin_id[] = "org.castlereforge.test.host.second";
    static const char external_plugin_id[] = "org.castlereforge.test.external";
    static const char unknown_interface[] = "org.castlereforge.test.unknown";
    static const char hook_interface[] = CASTLE_HOOK_INTERFACE_ID;
    static const char path_interface[] = CASTLE_PATH_INTERFACE_ID;
    static const char schedule_interface[] = CASTLE_SCHEDULE_INTERFACE_ID;
    static const char display_interface[] = CASTLE_DISPLAY_INTERFACE_ID;
    static const char display_provider_id[] = "org.castlereforge.test.display";
    static const char window_interface[] = CASTLE_WINDOW_INTERFACE_ID;
    static const char render_interface[] = CASTLE_RENDER_INTERFACE_ID;
    static const char render_provider_id[] = "org.castlereforge.test.render";
    static const char transaction_label[] = "host transaction";
    static const char patch_label[] = "data patch";
    static const char state_label[] = "state patch";
    static const char named_id[] = "org.castlereforge.test.named";
    static const char signature_id[] = "org.castlereforge.test.function.v1";
    static const char relative_path[] = "folder/file.ini";
    static const char alternate_relative_path[] = "folder/./file.ini";
    static const char built_suffix[] = "folder\\file.ini";
    static const char escape_relative_path[] = "../escape.ini";
    static const WCHAR relative_wide_path[] = L"wide/./file.ini";
    static const WCHAR escape_wide_path[] = L"..\\escape.ini";
    static const WCHAR bootstrap_plugin_a_name[] = L"runtime_bootstrap_a.asi";
    static const WCHAR bootstrap_plugin_b_name[] = L"runtime_bootstrap_b.asi";
    static const CastleU8 patch_expected[4] = {1u, 2u, 3u, 4u};
    static const CastleU8 patch_replacement[4] = {5u, 6u, 7u, 8u};
    static const CastleU8 state_original[2] = {0x10u, 0x20u};
    static const CastleU8 state_enabled[2] = {0xAAu, 0xBBu};
    static const CastleU8 rollback_a_expected[2] = {0x31u, 0x32u};
    static const CastleU8 rollback_a_replacement[2] = {0x71u, 0x72u};
    static const CastleU8 rollback_b_expected[2] = {0x41u, 0x42u};
    static const CastleU8 rollback_b_replacement[2] = {0x81u, 0x82u};
    HMODULE self_module;

    if (!build_runtime_path_(runtime_path, 1024u)) ExitProcess(10u);
    runtime_module = LoadLibraryW(runtime_path);
    if (!runtime_module) ExitProcess(11u);

    get_api = (CastleRuntimeGetApiFn)GetProcAddress(runtime_module, "CastleRuntime_GetApi");
    if (!get_api) ExitProcess(12u);
    if (get_api(999u) != NULL) ExitProcess(13u);

    api = get_api(CASTLE_RUNTIME_ABI_V1);
    if (!api || api->magic != CASTLE_RUNTIME_API_MAGIC ||
        api->struct_size != CASTLE_SIZEOF_RUNTIME_API_V1 ||
        api->abi_version != CASTLE_RUNTIME_ABI_V1) ExitProcess(14u);
    if ((api->capability_flags_low & CASTLE_RUNTIME_CAP_PLUGIN_REGISTRY) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_DIAGNOSTICS) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_HOOK_TRANSACTION) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_HOOK_CHAIN) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_PATH_V1) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_BACKGROUND_SCHEDULE_V1) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_SERVICE_PROVIDER) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_DISPLAY_V1) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_WINDOW_V1) == 0u ||
        (api->capability_flags_low & CASTLE_RUNTIME_CAP_RENDER_V1) == 0u) {
        ExitProcess(15u);
    }

    byte_zero_(&info, (CastleU32)sizeof(info));
    info.magic = CASTLE_RUNTIME_INFO_MAGIC;
    info.struct_size = CASTLE_SIZEOF_RUNTIME_INFO_V1;
    info.info_version = CASTLE_RUNTIME_INFO_VERSION_1;
    if (api->GetRuntimeInfo(&info) != CASTLE_OK ||
        info.runtime_module == 0u || info.game_module == 0u ||
        !info.runtime_path.data || info.runtime_path.length == 0u ||
        !info.game_build_id.data || info.game_build_id.length == 0u) ExitProcess(16u);

    byte_zero_(&query, (CastleU32)sizeof(query));
    byte_zero_(&query_result, (CastleU32)sizeof(query_result));
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = view_(unknown_interface, (CastleU32)(sizeof(unknown_interface) - 1u));
    query.requested_version = 1u;
    query_result.magic = CASTLE_INTERFACE_API_MAGIC;
    query_result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    query_result.result_version = CASTLE_QUERY_VERSION_1;
    if (api->QueryInterface(&query, &query_result) != CASTLE_ERROR_INTERFACE_NOT_FOUND ||
        query_result.api_pointer != NULL) ExitProcess(17u);

    query.interface_id = view_(hook_interface, (CastleU32)(sizeof(hook_interface) - 1u));
    query.requested_version = CASTLE_HOOK_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_HOOK_API_V1;
    if (api->QueryInterface(&query, &query_result) != CASTLE_OK ||
        !query_result.api_pointer ||
        query_result.actual_struct_size != CASTLE_SIZEOF_HOOK_API_V1) ExitProcess(26u);
    hook_api = (const CastleHookApiV1*)query_result.api_pointer;
    if (hook_api->magic != CASTLE_HOOK_API_MAGIC ||
        !hook_api->BeginTransaction || !hook_api->CommitTransaction) ExitProcess(27u);

    query.interface_id = view_(path_interface, (CastleU32)(sizeof(path_interface) - 1u));
    query.requested_version = CASTLE_PATH_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_PATH_API_V1;
    if (api->QueryInterface(&query, &query_result) != CASTLE_OK ||
        !query_result.api_pointer ||
        query_result.actual_struct_size != CASTLE_SIZEOF_PATH_API_V1) ExitProcess(60u);
    path_api = (const CastlePathApiV1*)query_result.api_pointer;
    if (path_api->magic != CASTLE_PATH_API_MAGIC || !path_api->GetPathInfo ||
        !path_api->BuildPluginRelativePathUtf8 || !path_api->PathsEqualWide) {
        ExitProcess(61u);
    }

    query.interface_id = view_(schedule_interface,
        (CastleU32)(sizeof(schedule_interface) - 1u));
    query.requested_version = CASTLE_SCHEDULE_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_SCHEDULE_API_V1;
    query.required_capabilities_low = CASTLE_SCHEDULE_CAP_BACKGROUND;
    if (api->QueryInterface(&query, &query_result) != CASTLE_OK ||
        !query_result.api_pointer ||
        query_result.actual_struct_size != CASTLE_SIZEOF_SCHEDULE_API_V1 ||
        (query_result.capabilities_low & CASTLE_SCHEDULE_CAP_BACKGROUND) == 0u ||
        (query_result.capabilities_low & CASTLE_SCHEDULE_CAP_GAME_PHASE) != 0u) {
        ExitProcess(73u);
    }
    schedule_api = (const CastleScheduleApiV1*)query_result.api_pointer;
    if (schedule_api->magic != CASTLE_SCHEDULE_API_MAGIC ||
        !schedule_api->RegisterPeriodicTask || !schedule_api->PostBackgroundTask ||
        !schedule_api->GetGamePhaseState) ExitProcess(74u);

    query.interface_id = view_(display_interface,
        (CastleU32)(sizeof(display_interface) - 1u));
    query.requested_version = CASTLE_DISPLAY_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_DISPLAY_API_V1;
    query.required_capabilities_low = CASTLE_DISPLAY_CAP_EXTERNAL_PROVIDER |
                                      CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD;
    if (api->QueryInterface(&query, &query_result) != CASTLE_OK ||
        !query_result.api_pointer ||
        query_result.actual_struct_size != CASTLE_SIZEOF_DISPLAY_API_V1 ||
        (query_result.capabilities_low & CASTLE_DISPLAY_CAP_EXTERNAL_PROVIDER) == 0u) {
        ExitProcess(87u);
    }
    display_api = (const CastleDisplayApiV1*)query_result.api_pointer;
    initial_display_provider_generation = query_result.provider_generation;
    if (display_api->magic != CASTLE_DISPLAY_API_MAGIC ||
        !display_api->GetGeometry || !display_api->WorldToScreen ||
        !display_api->RegisterDisplayProvider) ExitProcess(88u);

    query.interface_id = view_(window_interface,
        (CastleU32)(sizeof(window_interface) - 1u));
    query.requested_version = CASTLE_WINDOW_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_WINDOW_API_V1;
    query.required_capabilities_low = CASTLE_WINDOW_CAP_OBSERVER |
                                      CASTLE_WINDOW_CAP_FILTER;
    if (api->QueryInterface(&query, &query_result) != CASTLE_OK ||
        !query_result.api_pointer ||
        query_result.actual_struct_size != CASTLE_SIZEOF_WINDOW_API_V1) ExitProcess(104u);
    window_api = (const CastleWindowApiV1*)query_result.api_pointer;
    if (window_api->magic != CASTLE_WINDOW_API_MAGIC ||
        !window_api->RegisterMessageObserver || !window_api->RegisterMessageFilter ||
        !window_api->UnregisterWindowClient) ExitProcess(105u);

    query.interface_id = view_(render_interface,
        (CastleU32)(sizeof(render_interface) - 1u));
    query.requested_version = CASTLE_RENDER_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_RENDER_API_V1;
    query.required_capabilities_low = CASTLE_RENDER_CAP_EXTERNAL_PROVIDER |
                                      CASTLE_RENDER_CAP_EXTRA_FRAME_LEASE;
    if (api->QueryInterface(&query, &query_result) != CASTLE_OK ||
        !query_result.api_pointer ||
        query_result.actual_struct_size != CASTLE_SIZEOF_RENDER_API_V1) ExitProcess(114u);
    render_api = (const CastleRenderApiV1*)query_result.api_pointer;
    initial_render_provider_generation = query_result.provider_generation;
    if (render_api->magic != CASTLE_RENDER_API_MAGIC ||
        !render_api->RenderCurrentQueue || !render_api->BeginExtraWorldFrame ||
        !render_api->RegisterRenderProvider) ExitProcess(115u);

    self_module = GetModuleHandleW(NULL);
    if (!self_module) ExitProcess(28u);
    byte_zero_(&descriptor, (CastleU32)sizeof(descriptor));
    descriptor.magic = CASTLE_PLUGIN_DESC_MAGIC;
    descriptor.struct_size = CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1;
    descriptor.descriptor_version = CASTLE_PLUGIN_DESCRIPTOR_V1;
    descriptor.flags = CASTLE_PLUGIN_FLAG_SUPPORTS_STANDALONE;
    descriptor.module = (CastleModule)(ULONG_PTR)self_module;
    descriptor.plugin_id = view_(plugin_id, (CastleU32)(sizeof(plugin_id) - 1u));
    descriptor.display_name = view_(display_name, (CastleU32)(sizeof(display_name) - 1u));
    descriptor.version_text = view_(version_text, (CastleU32)(sizeof(version_text) - 1u));
    descriptor.build_id = view_(build_id, (CastleU32)(sizeof(build_id) - 1u));

    result = api->RegisterPlugin(&descriptor, &handle);
    if (result != CASTLE_OK || handle == 0u) ExitProcess(18u);
    result = api->RegisterPlugin(&descriptor, &repeated_handle);
    if (result != CASTLE_STATUS_ALREADY_DONE || repeated_handle != handle) ExitProcess(19u);

    byte_copy_(&duplicate_descriptor, &descriptor, (CastleU32)sizeof(descriptor));
    duplicate_descriptor.module = (CastleModule)(ULONG_PTR)GetModuleHandleW(L"kernel32.dll");
    result = api->RegisterPlugin(&duplicate_descriptor, &duplicate_handle);
    if (result != CASTLE_ERROR_DUPLICATE_PLUGIN_ID) ExitProcess(20u);

    byte_copy_(&second_descriptor, &descriptor, (CastleU32)sizeof(descriptor));
    second_descriptor.plugin_id = view_(second_plugin_id,
        (CastleU32)(sizeof(second_plugin_id) - 1u));
    result = api->RegisterPlugin(&second_descriptor, &second_handle);
    if (result != CASTLE_OK || second_handle == 0u || second_handle == handle) ExitProcess(29u);

    /* 注册一个系统 DLL 伪装的测试记录，确认 Path 服务不会允许它越过 ASI 根目录。 */
    byte_copy_(&external_descriptor, &descriptor, (CastleU32)sizeof(descriptor));
    external_descriptor.plugin_id = view_(external_plugin_id,
        (CastleU32)(sizeof(external_plugin_id) - 1u));
    external_descriptor.module = (CastleModule)(ULONG_PTR)GetModuleHandleW(L"kernel32.dll");
    result = api->RegisterPlugin(&external_descriptor, &external_handle);
    if (result != CASTLE_OK || external_handle == 0u) ExitProcess(62u);

    byte_zero_(&state, (CastleU32)sizeof(state));
    state.magic = CASTLE_PLUGIN_STATE_MAGIC;
    state.struct_size = CASTLE_SIZEOF_PLUGIN_STATE_V1;
    state.state_version = CASTLE_PLUGIN_STATE_VERSION_1;
    if (api->GetPluginState(handle, &state) != CASTLE_OK ||
        state.state != CASTLE_PLUGIN_REGISTERED) ExitProcess(21u);

    /* Path 信息必须同时提供地址稳定的 UTF-8 与 UTF-16 根目录/Runtime 路径。 */
    byte_zero_(&path_info, (CastleU32)sizeof(path_info));
    path_info.magic = CASTLE_PATH_INFO_MAGIC;
    path_info.struct_size = CASTLE_SIZEOF_PATH_INFO_V1;
    path_info.version = CASTLE_PATH_VERSION_1;
    if (path_api->GetPathInfo(&path_info) != CASTLE_OK ||
        !path_info.asi_directory_utf8.data || path_info.asi_directory_utf8.length == 0u ||
        !path_info.asi_directory_wide.data || path_info.asi_directory_wide.length == 0u ||
        !path_info.runtime_path_utf8.data || path_info.runtime_path_utf8.length == 0u ||
        !path_info.runtime_path_wide.data || path_info.runtime_path_wide.length == 0u) {
        ExitProcess(63u);
    }

    /* 所有可变长度输出先返回精确所需长度，再由调用方提供包含 NUL 的容量。 */
    path_length = 0u;
    if (path_api->GetPluginModulePathUtf8(handle, NULL, 0u, &path_length) !=
            CASTLE_ERROR_BUFFER_TOO_SMALL || path_length == 0u ||
        path_api->GetPluginModulePathUtf8(handle, plugin_path,
            (CastleU32)sizeof(plugin_path), &path_length) != CASTLE_OK ||
        plugin_path[path_length] != '\0') ExitProcess(64u);

    path_length = 0u;
    if (path_api->GetPluginModulePathUtf8(external_handle, NULL, 0u, &path_length) !=
        CASTLE_ERROR_INVALID_ARGUMENT) ExitProcess(65u);

    path_length = 0u;
    if (path_api->BuildPluginRelativePathUtf8(handle,
            view_(relative_path, (CastleU32)(sizeof(relative_path) - 1u)),
            NULL, 0u, &path_length) != CASTLE_ERROR_BUFFER_TOO_SMALL ||
        path_length == 0u ||
        path_api->BuildPluginRelativePathUtf8(handle,
            view_(relative_path, (CastleU32)(sizeof(relative_path) - 1u)),
            built_path, (CastleU32)sizeof(built_path), &path_length) != CASTLE_OK ||
        !text_ends_with_(built_path, path_length, built_suffix,
                         (CastleU32)(sizeof(built_suffix) - 1u))) ExitProcess(66u);

    alternate_length = 0u;
    if (path_api->BuildPluginRelativePathUtf8(handle,
            view_(alternate_relative_path,
                  (CastleU32)(sizeof(alternate_relative_path) - 1u)),
            alternate_path, (CastleU32)sizeof(alternate_path),
            &alternate_length) != CASTLE_OK ||
        path_api->PathsEqualUtf8(view_(built_path, path_length),
            view_(alternate_path, alternate_length), &paths_equal) != CASTLE_OK ||
        paths_equal != 1u) ExitProcess(67u);

    normalized_length = 0u;
    if (path_api->NormalizeAbsolutePathUtf8(view_(alternate_path, alternate_length),
            normalized_path, (CastleU32)sizeof(normalized_path),
            &normalized_length) != CASTLE_OK ||
        path_api->PathsEqualUtf8(view_(built_path, path_length),
            view_(normalized_path, normalized_length), &paths_equal) != CASTLE_OK ||
        paths_equal != 1u) ExitProcess(68u);

    if (path_api->BuildPluginRelativePathUtf8(handle,
            view_(escape_relative_path,
                  (CastleU32)(sizeof(escape_relative_path) - 1u)),
            built_path, (CastleU32)sizeof(built_path), &path_length) !=
        CASTLE_ERROR_INVALID_ARGUMENT) ExitProcess(69u);

    path_length = 0u;
    if (path_api->BuildPluginRelativePathWide(handle,
            wide_view_(relative_wide_path,
                (CastleU32)(sizeof(relative_wide_path) / sizeof(relative_wide_path[0]) - 1u)),
            NULL, 0u, &path_length) != CASTLE_ERROR_BUFFER_TOO_SMALL ||
        path_api->BuildPluginRelativePathWide(handle,
            wide_view_(relative_wide_path,
                (CastleU32)(sizeof(relative_wide_path) / sizeof(relative_wide_path[0]) - 1u)),
            built_wide, (CastleU32)(sizeof(built_wide) / sizeof(built_wide[0])),
            &path_length) != CASTLE_OK) ExitProcess(70u);

    normalized_length = 0u;
    if (path_api->NormalizeAbsolutePathWide(
            wide_view_((const WCHAR*)built_wide, path_length), normalized_wide,
            (CastleU32)(sizeof(normalized_wide) / sizeof(normalized_wide[0])),
            &normalized_length) != CASTLE_OK ||
        path_api->PathsEqualWide(wide_view_((const WCHAR*)built_wide, path_length),
            wide_view_((const WCHAR*)normalized_wide, normalized_length),
            &paths_equal) != CASTLE_OK || paths_equal != 1u) ExitProcess(71u);

    if (path_api->BuildPluginRelativePathWide(handle,
            wide_view_(escape_wide_path,
                (CastleU32)(sizeof(escape_wide_path) / sizeof(escape_wide_path[0]) - 1u)),
            built_wide, (CastleU32)(sizeof(built_wide) / sizeof(built_wide[0])),
            &path_length) != CASTLE_ERROR_INVALID_ARGUMENT) ExitProcess(72u);

    /* 周期任务和一次性任务必须共用同一个 Runtime 后台线程。 */
    byte_zero_(&periodic_context, (CastleU32)sizeof(periodic_context));
    periodic_context.event_handle = CreateEventW(NULL, FALSE, FALSE, NULL);
    periodic_context.signal_after = 3;
    periodic_context.callback_result = CASTLE_OK;
    if (!periodic_context.event_handle) ExitProcess(75u);

    byte_zero_(&scheduled_task, (CastleU32)sizeof(scheduled_task));
    scheduled_task.magic = CASTLE_SCHEDULE_TASK_MAGIC;
    scheduled_task.struct_size = CASTLE_SIZEOF_SCHEDULED_TASK_V1;
    scheduled_task.version = CASTLE_SCHEDULE_STRUCTURE_VERSION_1;
    scheduled_task.flags = CASTLE_SCHEDULE_TASK_START_ENABLED;
    scheduled_task.period_ms = 5u;
    scheduled_task.budget_ms = 50u;
    scheduled_task.phase = CASTLE_SCHEDULE_PHASE_NORMAL;
    scheduled_task.priority = CASTLE_SCHEDULE_PRIORITY_DEFAULT;
    scheduled_task.callback = schedule_test_callback_;
    scheduled_task.user_context = &periodic_context;
    scheduled_task.label = view_(transaction_label,
        (CastleU32)(sizeof(transaction_label) - 1u));
    if (schedule_api->RegisterPeriodicTask(handle, &scheduled_task,
            &schedule_task) != CASTLE_OK || schedule_task == 0u ||
        WaitForSingleObject(periodic_context.event_handle, 2000u) != WAIT_OBJECT_0 ||
        periodic_context.callback_thread_id == GetCurrentThreadId()) ExitProcess(76u);

    if (schedule_api->SetPeriodicTaskEnabled(schedule_task, 0u) != CASTLE_OK) {
        ExitProcess(77u);
    }
    Sleep(20u);
    paused_count = InterlockedCompareExchange(&periodic_context.call_count, 0, 0);
    Sleep(30u);
    if (InterlockedCompareExchange(&periodic_context.call_count, 0, 0) != paused_count) {
        ExitProcess(78u);
    }

    byte_zero_(&schedule_stats, (CastleU32)sizeof(schedule_stats));
    schedule_stats.magic = CASTLE_SCHEDULE_STATS_MAGIC;
    schedule_stats.struct_size = CASTLE_SIZEOF_SCHEDULE_STATS_V1;
    schedule_stats.version = CASTLE_SCHEDULE_STRUCTURE_VERSION_1;
    if (schedule_api->GetPeriodicTaskStats(schedule_task, &schedule_stats) != CASTLE_OK ||
        schedule_stats.run_count < 3u || schedule_stats.enabled != 0u ||
        schedule_stats.failure_count != 0u || schedule_stats.over_budget_count != 0u ||
        schedule_stats.state != CASTLE_SCHEDULE_TASK_DISABLED) ExitProcess(79u);

    byte_zero_(&posted_context, (CastleU32)sizeof(posted_context));
    posted_context.event_handle = CreateEventW(NULL, FALSE, FALSE, NULL);
    posted_context.signal_after = 1;
    posted_context.callback_result = CASTLE_OK;
    if (!posted_context.event_handle) ExitProcess(80u);
    scheduled_task.flags = 0u;
    scheduled_task.period_ms = 0u;
    scheduled_task.user_context = &posted_context;
    if (schedule_api->PostBackgroundTask(second_handle, &scheduled_task) != CASTLE_OK ||
        WaitForSingleObject(posted_context.event_handle, 2000u) != WAIT_OBJECT_0 ||
        posted_context.callback_thread_id != periodic_context.callback_thread_id) {
        ExitProcess(81u);
    }

    /* 尚无实机阶段源时，游戏线程能力必须清楚返回可选不可用。 */
    byte_zero_(&game_phase_state, (CastleU32)sizeof(game_phase_state));
    game_phase_state.magic = CASTLE_GAME_PHASE_STATE_MAGIC;
    game_phase_state.struct_size = CASTLE_SIZEOF_GAME_PHASE_STATE_V1;
    game_phase_state.version = CASTLE_SCHEDULE_STRUCTURE_VERSION_1;
    if (schedule_api->GetGamePhaseState(&game_phase_state) !=
            CASTLE_STATUS_OPTIONAL_UNAVAILABLE || game_phase_state.available != 0u ||
        schedule_api->PostGameThreadTask(handle, &scheduled_task) !=
            CASTLE_STATUS_OPTIONAL_UNAVAILABLE) ExitProcess(82u);

    if (schedule_api->UnregisterPeriodicTask(schedule_task) != CASTLE_OK ||
        schedule_api->GetPeriodicTaskStats(schedule_task, &schedule_stats) !=
            CASTLE_ERROR_INVALID_ARGUMENT) ExitProcess(83u);
    CloseHandle(periodic_context.event_handle);
    CloseHandle(posted_context.event_handle);

    /* 连续三次回调失败后只隔离该任务，调度线程本身仍继续服务其它插件。 */
    byte_zero_(&periodic_context, (CastleU32)sizeof(periodic_context));
    periodic_context.event_handle = CreateEventW(NULL, FALSE, FALSE, NULL);
    periodic_context.signal_after = 3;
    periodic_context.callback_result = CASTLE_ERROR_RUNTIME_FAULT;
    if (!periodic_context.event_handle) ExitProcess(84u);
    scheduled_task.flags = CASTLE_SCHEDULE_TASK_START_ENABLED;
    scheduled_task.period_ms = 3u;
    scheduled_task.user_context = &periodic_context;
    if (schedule_api->RegisterPeriodicTask(handle, &scheduled_task,
            &schedule_task) != CASTLE_OK ||
        WaitForSingleObject(periodic_context.event_handle, 2000u) != WAIT_OBJECT_0) {
        ExitProcess(85u);
    }
    Sleep(20u);
    byte_zero_(&schedule_stats, (CastleU32)sizeof(schedule_stats));
    schedule_stats.magic = CASTLE_SCHEDULE_STATS_MAGIC;
    schedule_stats.struct_size = CASTLE_SIZEOF_SCHEDULE_STATS_V1;
    schedule_stats.version = CASTLE_SCHEDULE_STRUCTURE_VERSION_1;
    if (schedule_api->GetPeriodicTaskStats(schedule_task, &schedule_stats) != CASTLE_OK ||
        schedule_stats.state != CASTLE_SCHEDULE_TASK_FAULT ||
        schedule_stats.enabled != 0u || schedule_stats.failure_count < 3u ||
        schedule_api->UnregisterPeriodicTask(schedule_task) != CASTLE_OK) ExitProcess(86u);
    CloseHandle(periodic_context.event_handle);

    /* 测试宿主不是已确认 RPG.exe，因此注册专业后端前默认 Display 必须保持未就绪。 */
    byte_zero_(&display_state, (CastleU32)sizeof(display_state));
    display_state.magic = CASTLE_DISPLAY_STATE_MAGIC;
    display_state.struct_size = CASTLE_SIZEOF_DISPLAY_STATE_V1;
    display_state.version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    if (display_api->GetDisplayState(&display_state) != CASTLE_ERROR_NOT_READY ||
        display_state.ready != 0u) ExitProcess(89u);

    byte_zero_(&display_provider_info, (CastleU32)sizeof(display_provider_info));
    display_provider_info.magic = CASTLE_DISPLAY_PROVIDER_INFO_MAGIC;
    display_provider_info.struct_size = CASTLE_SIZEOF_DISPLAY_PROVIDER_INFO_V1;
    display_provider_info.version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    if (display_api->GetProviderInfo(&display_provider_info) !=
            CASTLE_STATUS_DEFAULT_BACKEND ||
        display_provider_info.plugin_handle != 0u ||
        display_provider_info.state != CASTLE_DISPLAY_PROVIDER_NOT_READY) ExitProcess(90u);

    if (display_api->RegisterDisplayProvider(handle,
            view_(display_provider_id,
                  (CastleU32)(sizeof(display_provider_id) - 1u)),
            &g_test_display_provider, &display_provider) != CASTLE_OK ||
        display_provider == 0u ||
        display_api->SetDisplayProviderReady(display_provider, 1u) !=
            CASTLE_ERROR_NOT_READY) ExitProcess(91u);

    /* 注册后重新查询接口，provider_generation 必须变化而门面地址保持不变。 */
    query.interface_id = view_(display_interface,
        (CastleU32)(sizeof(display_interface) - 1u));
    query.requested_version = CASTLE_DISPLAY_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_DISPLAY_API_V1;
    query.required_capabilities_low = CASTLE_DISPLAY_CAP_EXTERNAL_PROVIDER |
                                      CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD;
    if (api->QueryInterface(&query, &query_result) != CASTLE_OK ||
        query_result.api_pointer != display_api ||
        query_result.provider_generation <= initial_display_provider_generation) {
        ExitProcess(92u);
    }

    byte_zero_(&display_geometry, (CastleU32)sizeof(display_geometry));
    display_geometry.magic = CASTLE_DISPLAY_GEOMETRY_MAGIC;
    display_geometry.struct_size = CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1;
    display_geometry.api_version = CASTLE_DISPLAY_API_VERSION_1;
    display_geometry.output_width = 854u;
    display_geometry.output_height = 480u;
    display_geometry.logical_width = 640u;
    display_geometry.logical_height = 480u;
    display_geometry.center_x = 107;
    display_geometry.center_y = 0;
    display_geometry.center_width = 640;
    display_geometry.center_height = 480;
    display_geometry.effective_camera_x = 1000;
    display_geometry.effective_camera_y = 2000;
    display_geometry.original_camera_x = 990;
    display_geometry.original_camera_y = 2000;
    display_geometry.left_world_width = 107u;
    display_geometry.right_world_width = 107u;
    display_geometry.display_mode = CASTLE_DISPLAY_WIDE_WORLD;
    display_geometry.projection_scope = CASTLE_PROJECTION_FULL_OUTPUT;
    if (display_api->PublishDisplayGeometry(display_provider, &display_geometry,
            &display_generation) != CASTLE_OK || display_generation == 0u) ExitProcess(93u);
    g_test_display_geometry = display_geometry;
    g_test_display_geometry.generation = display_generation;
    g_test_display_ready = 1u;
    if (display_api->SetDisplayProviderReady(display_provider, 1u) != CASTLE_OK) {
        ExitProcess(94u);
    }

    byte_zero_(&display_state, (CastleU32)sizeof(display_state));
    display_state.magic = CASTLE_DISPLAY_STATE_MAGIC;
    display_state.struct_size = CASTLE_SIZEOF_DISPLAY_STATE_V1;
    display_state.version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    if (display_api->GetDisplayState(&display_state) != CASTLE_OK ||
        display_state.ready != 1u || display_state.backend_plugin != handle ||
        display_state.generation != display_generation ||
        display_state.display_mode != CASTLE_DISPLAY_WIDE_WORLD) ExitProcess(95u);

    byte_zero_(&display_geometry, (CastleU32)sizeof(display_geometry));
    display_geometry.magic = CASTLE_DISPLAY_GEOMETRY_MAGIC;
    display_geometry.struct_size = CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1;
    display_geometry.api_version = CASTLE_DISPLAY_API_VERSION_1;
    if (display_api->GetGeometry(&display_geometry) != CASTLE_OK ||
        display_geometry.output_width != 854u ||
        display_geometry.effective_camera_x != 1000 ||
        display_geometry.generation != display_generation) ExitProcess(96u);

    byte_zero_(&world_request, (CastleU32)sizeof(world_request));
    world_request.magic = CASTLE_WORLD_TO_SCREEN_MAGIC;
    world_request.struct_size = CASTLE_SIZEOF_WORLD_TO_SCREEN_V1;
    world_request.request_version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    world_request.requested_generation = display_generation;
    world_request.world_x = 1000;
    world_request.world_y = 2000;
    byte_zero_(&screen_projection, (CastleU32)sizeof(screen_projection));
    screen_projection.magic = CASTLE_SCREEN_PROJECTION_MAGIC;
    screen_projection.struct_size = CASTLE_SIZEOF_SCREEN_PROJECTION_V1;
    screen_projection.result_version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    if (display_api->WorldToScreen(&world_request, &screen_projection) != CASTLE_OK ||
        screen_projection.screen_x != 107 || screen_projection.screen_y != 0 ||
        screen_projection.visibility != CASTLE_VISIBILITY_VISIBLE ||
        screen_projection.actual_generation != display_generation) ExitProcess(97u);
    /* 0 表示“不锁定代次”，所以不能用 generation-1：当前代次为1时会误变成合法0。 */
    world_request.requested_generation = display_generation + 1u;
    if (display_api->WorldToScreen(&world_request, &screen_projection) !=
        CASTLE_ERROR_STALE_GENERATION) ExitProcess(98u);

    byte_zero_(&screen_request, (CastleU32)sizeof(screen_request));
    screen_request.magic = CASTLE_SCREEN_TO_WORLD_MAGIC;
    screen_request.struct_size = CASTLE_SIZEOF_SCREEN_TO_WORLD_V1;
    screen_request.request_version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    screen_request.requested_generation = display_generation;
    screen_request.screen_x = 107;
    screen_request.screen_y = 0;
    byte_zero_(&world_projection, (CastleU32)sizeof(world_projection));
    world_projection.magic = CASTLE_WORLD_PROJECTION_MAGIC;
    world_projection.struct_size = CASTLE_SIZEOF_WORLD_PROJECTION_V1;
    world_projection.result_version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    if (display_api->ScreenToWorld(&screen_request, &world_projection) != CASTLE_OK ||
        world_projection.world_x != 1000 || world_projection.world_y != 2000 ||
        world_projection.actual_generation != display_generation) ExitProcess(99u);

    byte_zero_(&display_provider_info, (CastleU32)sizeof(display_provider_info));
    display_provider_info.magic = CASTLE_DISPLAY_PROVIDER_INFO_MAGIC;
    display_provider_info.struct_size = CASTLE_SIZEOF_DISPLAY_PROVIDER_INFO_V1;
    display_provider_info.version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    if (display_api->GetProviderInfo(&display_provider_info) != CASTLE_OK ||
        display_provider_info.provider_handle != display_provider ||
        display_provider_info.plugin_handle != handle ||
        display_provider_info.state != CASTLE_DISPLAY_PROVIDER_READY) ExitProcess(100u);

    if (display_api->RegisterDisplayProvider(second_handle,
            view_(display_provider_id,
                  (CastleU32)(sizeof(display_provider_id) - 1u)),
            &g_test_display_provider, &duplicate_display_provider) !=
        CASTLE_ERROR_RESOURCE_CONFLICT) ExitProcess(101u);

    g_test_display_ready = 0u;
    if (display_api->SetDisplayProviderReady(display_provider, 0u) != CASTLE_OK ||
        display_api->GetDisplayState(&display_state) != CASTLE_ERROR_NOT_READY ||
        display_state.ready != 0u) ExitProcess(102u);
    g_test_display_ready = 1u;
    if (display_api->SetDisplayProviderReady(display_provider, 1u) != CASTLE_OK) {
        ExitProcess(103u);
    }

    /* Render 专业后端必须与同一插件已经就绪的 Display Provider 绑定。 */
    byte_zero_(&render_state, (CastleU32)sizeof(render_state));
    render_state.magic = CASTLE_RENDER_STATE_MAGIC;
    render_state.struct_size = CASTLE_SIZEOF_RENDER_STATE_V1;
    render_state.version = CASTLE_RENDER_STRUCTURE_VERSION_1;
    if (render_api->GetRenderState(&render_state) != CASTLE_ERROR_NOT_READY ||
        render_state.ready != 0u) ExitProcess(116u);
    if (render_api->RegisterRenderProvider(second_handle,
            view_(render_provider_id, (CastleU32)(sizeof(render_provider_id) - 1u)),
            display_provider, &g_test_render_provider,
            &duplicate_render_provider) != CASTLE_ERROR_INVALID_ARGUMENT) ExitProcess(117u);
    if (render_api->RegisterRenderProvider(handle,
            view_(render_provider_id, (CastleU32)(sizeof(render_provider_id) - 1u)),
            display_provider, &g_test_render_provider,
            &render_provider) != CASTLE_OK || render_provider == 0u) ExitProcess(118u);

    query.interface_id = view_(render_interface,
        (CastleU32)(sizeof(render_interface) - 1u));
    query.requested_version = CASTLE_RENDER_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_RENDER_API_V1;
    query.required_capabilities_low = CASTLE_RENDER_CAP_EXTERNAL_PROVIDER |
                                      CASTLE_RENDER_CAP_EXTRA_FRAME_LEASE;
    if (api->QueryInterface(&query, &query_result) != CASTLE_OK ||
        query_result.api_pointer != render_api ||
        query_result.provider_generation <= initial_render_provider_generation) {
        ExitProcess(119u);
    }

    g_test_render_ready = 0u;
    if (render_api->SetRenderProviderReady(render_provider, 1u) !=
        CASTLE_ERROR_NOT_READY) ExitProcess(120u);
    g_test_render_ready = 1u;
    if (render_api->SetRenderProviderReady(render_provider, 1u) != CASTLE_OK ||
        render_api->GetRenderState(&render_state) != CASTLE_OK ||
        render_state.ready != 1u || render_state.backend_plugin != handle ||
        render_state.provider_handle != render_provider) ExitProcess(121u);

    byte_zero_(&render_call, (CastleU32)sizeof(render_call));
    render_call.magic = CASTLE_RENDER_CALL_MAGIC;
    render_call.struct_size = CASTLE_SIZEOF_RENDER_CALL_V1;
    render_call.version = CASTLE_RENDER_STRUCTURE_VERSION_1;
    render_call.render_context = 0x12345678u;
    render_call.display_generation = display_generation;
    if (render_api->RenderCurrentQueue(&render_call) != CASTLE_OK ||
        render_api->PresentCurrentDisplay(&render_call) != CASTLE_OK ||
        g_test_render_count != 1 || g_test_present_count != 1 ||
        g_test_last_render_call.display_generation != display_generation) ExitProcess(122u);

    if (render_api->BeginExtraWorldFrame(second_handle, display_generation,
            &extra_frame_lease, &extra_frame_display_generation) != CASTLE_OK ||
        extra_frame_lease == 0u || extra_frame_display_generation != display_generation ||
        render_api->BeginExtraWorldFrame(handle, display_generation,
            &duplicate_extra_frame_lease, &extra_frame_display_generation) !=
            CASTLE_ERROR_RESOURCE_CONFLICT) ExitProcess(123u);
    render_call.flags = CASTLE_RENDER_CALL_EXTRA_WORLD_FRAME;
    render_call.extra_frame_lease = extra_frame_lease;
    if (render_api->RenderCurrentQueue(&render_call) != CASTLE_OK ||
        render_api->PresentCurrentDisplay(&render_call) != CASTLE_OK ||
        g_test_render_count != 2 || g_test_present_count != 2 ||
        g_test_last_render_call.extra_frame_lease != extra_frame_lease) ExitProcess(124u);
    if (render_api->GetRenderState(&render_state) != CASTLE_OK ||
        render_state.extra_frame_owner != second_handle ||
        render_api->EndExtraWorldFrame(extra_frame_lease) != CASTLE_OK ||
        render_api->EndExtraWorldFrame(extra_frame_lease) !=
            CASTLE_ERROR_INVALID_ARGUMENT) ExitProcess(125u);
    if (render_api->RenderCurrentQueue(&render_call) !=
        CASTLE_ERROR_RESOURCE_CONFLICT) ExitProcess(126u);

    render_call.flags = 0u;
    render_call.extra_frame_lease = 0u;
    render_call.display_generation = display_generation + 1u;
    if (render_api->RenderCurrentQueue(&render_call) !=
        CASTLE_ERROR_STALE_GENERATION) ExitProcess(127u);
    if (render_api->RegisterRenderProvider(handle,
            view_(render_provider_id, (CastleU32)(sizeof(render_provider_id) - 1u)),
            display_provider, &g_test_render_provider,
            &duplicate_render_provider) != CASTLE_ERROR_RESOURCE_CONFLICT) ExitProcess(128u);

    g_test_render_ready = 0u;
    if (render_api->SetRenderProviderReady(render_provider, 0u) != CASTLE_OK ||
        render_api->GetRenderState(&render_state) != CASTLE_ERROR_NOT_READY ||
        render_state.ready != 0u) ExitProcess(129u);
    g_test_render_ready = 1u;
    if (render_api->SetRenderProviderReady(render_provider, 1u) != CASTLE_OK) {
        ExitProcess(130u);
    }

    /* 创建本进程可见测试窗口，验证 Runtime 主 WndProc 的观察、过滤和销毁代次。 */
    test_window = CreateWindowExA(0u, "STATIC", "CastleRuntimeWindowTest",
        WS_OVERLAPPEDWINDOW, 0, 0, 160, 120, NULL, NULL, self_module, NULL);
    if (!test_window) ExitProcess(106u);
    ShowWindow(test_window, SW_SHOWNA);
    UpdateWindow(test_window);

    byte_zero_(&window_state, (CastleU32)sizeof(window_state));
    window_state.magic = CASTLE_WINDOW_STATE_MAGIC;
    window_state.struct_size = CASTLE_SIZEOF_WINDOW_STATE_V1;
    window_state.version = CASTLE_WINDOW_STRUCTURE_VERSION_1;
    if (window_api->GetGameWindow(&window_state) != CASTLE_OK ||
        window_state.ready != 1u ||
        window_state.window != (CastleAddress)(ULONG_PTR)test_window) ExitProcess(107u);
    window_generation_before = window_state.generation;

    byte_zero_(&window_context, (CastleU32)sizeof(window_context));
    window_context.target_message = WM_APP + 17u;
    window_context.consumed_result = 4321;
    byte_zero_(&window_client, (CastleU32)sizeof(window_client));
    window_client.magic = CASTLE_WINDOW_CLIENT_MAGIC;
    window_client.struct_size = CASTLE_SIZEOF_WINDOW_CLIENT_V1;
    window_client.version = CASTLE_WINDOW_STRUCTURE_VERSION_1;
    window_client.phase = CASTLE_WINDOW_PHASE_NORMAL;
    window_client.priority = CASTLE_WINDOW_PRIORITY_DEFAULT;
    window_client.observer = test_window_observer_;
    window_client.user_context = &window_context;
    window_client.label = view_(transaction_label,
        (CastleU32)(sizeof(transaction_label) - 1u));
    if (window_api->RegisterMessageObserver(handle, &window_client,
            &observer_client) != CASTLE_OK || observer_client == 0u ||
        window_api->SetWindowClientReady(observer_client, 1u) != CASTLE_OK) {
        ExitProcess(108u);
    }

    window_client.observer = NULL;
    window_client.filter = test_window_filter_;
    if (window_api->RegisterMessageFilter(second_handle, &window_client,
            &filter_client) != CASTLE_OK || filter_client == 0u ||
        window_api->SetWindowClientReady(filter_client, 1u) != CASTLE_OK) {
        ExitProcess(109u);
    }
    if (window_api->GetGameWindow(&window_state) != CASTLE_OK ||
        window_state.observer_count != 1u || window_state.filter_count != 1u) {
        ExitProcess(110u);
    }

    window_message_result = SendMessageA(test_window,
        (UINT)window_context.target_message, 7u, 9);
    if (window_message_result != 4321 || window_context.observer_count != 1 ||
        window_context.filter_count != 1 || window_context.observer_order != 1 ||
        window_context.filter_order != 2) ExitProcess(111u);

    if (!DestroyWindow(test_window) ||
        window_api->GetWindowGeneration(&window_generation_after) != CASTLE_OK ||
        window_generation_after <= window_generation_before) ExitProcess(112u);
    test_window = NULL;
    if (window_api->UnregisterWindowClient(observer_client) != CASTLE_OK ||
        window_api->UnregisterWindowClient(filter_client) != CASTLE_OK ||
        window_api->SetWindowClientReady(observer_client, 1u) !=
            CASTLE_ERROR_INVALID_ARGUMENT) ExitProcess(113u);

    /* 一个事务同时验证普通数据补丁、双态补丁和命名资源。 */
    if (hook_api->BeginTransaction(handle,
            view_(transaction_label, (CastleU32)(sizeof(transaction_label) - 1u)),
            0u, &transaction) != CASTLE_OK) ExitProcess(30u);

    byte_zero_(&exclusive_claim, (CastleU32)sizeof(exclusive_claim));
    exclusive_claim.magic = CASTLE_EXCLUSIVE_PATCH_MAGIC;
    exclusive_claim.struct_size = CASTLE_SIZEOF_EXCLUSIVE_PATCH_V1;
    exclusive_claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    exclusive_claim.flags = CASTLE_PATCH_FLAG_DATA;
    exclusive_claim.target = target_from_address_(self_module, g_patch_data, 4u);
    exclusive_claim.expected_bytes = patch_expected;
    exclusive_claim.expected_size = 4u;
    exclusive_claim.replacement_bytes = patch_replacement;
    exclusive_claim.replacement_size = 4u;
    exclusive_claim.label = view_(patch_label, (CastleU32)(sizeof(patch_label) - 1u));
    if (hook_api->AddExclusivePatch(transaction, &exclusive_claim,
                                    &exclusive_claim_handle) != CASTLE_OK) ExitProcess(31u);

    byte_zero_(&state_claim, (CastleU32)sizeof(state_claim));
    state_claim.magic = CASTLE_STATE_PATCH_MAGIC;
    state_claim.struct_size = CASTLE_SIZEOF_STATE_PATCH_V1;
    state_claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    state_claim.flags = CASTLE_PATCH_FLAG_DATA;
    state_claim.target = target_from_address_(self_module, g_state_data, 2u);
    state_claim.original_bytes = state_original;
    state_claim.original_size = 2u;
    state_claim.enabled_bytes = state_enabled;
    state_claim.enabled_size = 2u;
    state_claim.desired_state = CASTLE_PATCH_STATE_ENABLED;
    state_claim.label = view_(state_label, (CastleU32)(sizeof(state_label) - 1u));
    if (hook_api->AddStatePatch(transaction, &state_claim,
                                &state_claim_handle) != CASTLE_OK) ExitProcess(32u);

    byte_zero_(&named_claim, (CastleU32)sizeof(named_claim));
    named_claim.magic = CASTLE_NAMED_RESOURCE_MAGIC;
    named_claim.struct_size = CASTLE_SIZEOF_NAMED_RESOURCE_V1;
    named_claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    named_claim.resource_id = view_(named_id, (CastleU32)(sizeof(named_id) - 1u));
    named_claim.resource_kind = 1u;
    named_claim.phase = CASTLE_HOOK_PHASE_NORMAL;
    named_claim.priority = CASTLE_HOOK_PRIORITY_DEFAULT;
    named_claim.label = named_claim.resource_id;
    if (hook_api->AddNamedResource(transaction, &named_claim,
                                   &named_claim_handle) != CASTLE_OK) ExitProcess(33u);

    if (hook_api->PreflightTransaction(transaction) != CASTLE_OK) ExitProcess(34u);
    if (hook_api->CommitTransaction(transaction) != CASTLE_OK) ExitProcess(35u);
    if (g_patch_data[0] != 5u || g_patch_data[3] != 8u ||
        g_state_data[0] != 0xAAu || g_state_data[1] != 0xBBu) ExitProcess(36u);

    byte_zero_(&claim_result, (CastleU32)sizeof(claim_result));
    claim_result.magic = CASTLE_CLAIM_RESULT_MAGIC;
    claim_result.struct_size = CASTLE_SIZEOF_CLAIM_RESULT_V1;
    claim_result.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    if (hook_api->GetClaimResult(exclusive_claim_handle, &claim_result) != CASTLE_OK ||
        claim_result.result != CASTLE_OK || claim_result.resource_generation == 0u) ExitProcess(37u);

    /* 已被活动事务占用的范围必须在预检阶段明确冲突。 */
    if (hook_api->BeginTransaction(second_handle,
            view_(transaction_label, (CastleU32)(sizeof(transaction_label) - 1u)),
            0u, &conflict_transaction) != CASTLE_OK) ExitProcess(38u);
    byte_copy_(&conflict_claim, &exclusive_claim, (CastleU32)sizeof(conflict_claim));
    conflict_claim.expected_bytes = patch_replacement;
    if (hook_api->AddExclusivePatch(conflict_transaction, &conflict_claim,
                                    &conflict_claim_handle) != CASTLE_OK) ExitProcess(39u);
    if (hook_api->PreflightTransaction(conflict_transaction) !=
        CASTLE_ERROR_RESOURCE_CONFLICT) ExitProcess(40u);
    if (hook_api->AbortTransaction(conflict_transaction) != CASTLE_OK) ExitProcess(41u);

    /* 预检后故意破坏第二地址，确认第一地址写入会被逆序恢复。 */
    if (hook_api->BeginTransaction(handle,
            view_(transaction_label, (CastleU32)(sizeof(transaction_label) - 1u)),
            0u, &rollback_transaction) != CASTLE_OK) ExitProcess(42u);
    byte_zero_(&rollback_a_claim, (CastleU32)sizeof(rollback_a_claim));
    rollback_a_claim.magic = CASTLE_EXCLUSIVE_PATCH_MAGIC;
    rollback_a_claim.struct_size = CASTLE_SIZEOF_EXCLUSIVE_PATCH_V1;
    rollback_a_claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    rollback_a_claim.flags = CASTLE_PATCH_FLAG_DATA;
    rollback_a_claim.target = target_from_address_(self_module, g_rollback_a, 2u);
    rollback_a_claim.expected_bytes = rollback_a_expected;
    rollback_a_claim.expected_size = 2u;
    rollback_a_claim.replacement_bytes = rollback_a_replacement;
    rollback_a_claim.replacement_size = 2u;
    rollback_a_claim.label = view_(patch_label, (CastleU32)(sizeof(patch_label) - 1u));
    byte_copy_(&rollback_b_claim, &rollback_a_claim,
               (CastleU32)sizeof(rollback_b_claim));
    rollback_b_claim.target = target_from_address_(self_module, g_rollback_b, 2u);
    rollback_b_claim.expected_bytes = rollback_b_expected;
    rollback_b_claim.replacement_bytes = rollback_b_replacement;
    if (hook_api->AddExclusivePatch(rollback_transaction, &rollback_a_claim,
                                    &rollback_claim_a) != CASTLE_OK ||
        hook_api->AddExclusivePatch(rollback_transaction, &rollback_b_claim,
                                    &rollback_claim_b) != CASTLE_OK) ExitProcess(43u);
    if (hook_api->PreflightTransaction(rollback_transaction) != CASTLE_OK) ExitProcess(44u);
    g_rollback_b[0] = 0x99u;
    if (hook_api->CommitTransaction(rollback_transaction) !=
        CASTLE_ERROR_TRANSACTION_ROLLED_BACK) ExitProcess(45u);
    if (g_rollback_a[0] != 0x31u || g_rollback_a[1] != 0x32u ||
        g_rollback_b[0] != 0x99u) ExitProcess(46u);
    g_rollback_b[0] = 0x41u;

    /* 两个插件接入同一 vtable/IAT 风格指针槽，检查确定顺序和稳定 next 槽。 */
    g_pointer_slot = &test_original_;
    if (hook_api->BeginTransaction(handle,
            view_(transaction_label, (CastleU32)(sizeof(transaction_label) - 1u)),
            0u, &chain_transaction_one) != CASTLE_OK) ExitProcess(47u);
    byte_zero_(&chain_claim, (CastleU32)sizeof(chain_claim));
    chain_claim.magic = CASTLE_CHAIN_HOOK_MAGIC;
    chain_claim.struct_size = CASTLE_SIZEOF_CHAIN_HOOK_V1;
    chain_claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    chain_claim.hook_kind = CASTLE_HOOK_VTABLE_POINTER;
    chain_claim.target = target_from_address_(self_module, &g_pointer_slot, 4u);
    chain_claim.expected_original_target = (CastleAddress)(ULONG_PTR)&test_original_;
    chain_claim.replacement_hook = (CastleAddress)(ULONG_PTR)&test_hook_one_;
    chain_claim.signature_id = view_(signature_id, (CastleU32)(sizeof(signature_id) - 1u));
    chain_claim.phase = CASTLE_HOOK_PHASE_NORMAL;
    chain_claim.priority = CASTLE_HOOK_PRIORITY_DEFAULT;
    chain_claim.label = chain_claim.signature_id;
    if (hook_api->AddPointerHook(chain_transaction_one, &chain_claim,
                                 &chain_claim_one) != CASTLE_OK ||
        hook_api->PreflightTransaction(chain_transaction_one) != CASTLE_OK ||
        hook_api->CommitTransaction(chain_transaction_one) != CASTLE_OK) ExitProcess(48u);

    byte_zero_(&binding_one, (CastleU32)sizeof(binding_one));
    binding_one.magic = CASTLE_HOOK_BINDING_MAGIC;
    binding_one.struct_size = CASTLE_SIZEOF_HOOK_BINDING_V1;
    binding_one.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    if (hook_api->GetHookBinding(chain_claim_one, &binding_one) != CASTLE_OK ||
        !binding_one.next_slot || *binding_one.next_slot != (void*)&test_original_) ExitProcess(49u);
    g_hook_one_next = binding_one.next_slot;

    if (hook_api->BeginTransaction(second_handle,
            view_(transaction_label, (CastleU32)(sizeof(transaction_label) - 1u)),
            0u, &chain_transaction_two) != CASTLE_OK) ExitProcess(50u);
    chain_claim.replacement_hook = (CastleAddress)(ULONG_PTR)&test_hook_two_;
    if (hook_api->AddPointerHook(chain_transaction_two, &chain_claim,
                                 &chain_claim_two) != CASTLE_OK ||
        hook_api->PreflightTransaction(chain_transaction_two) != CASTLE_OK ||
        hook_api->CommitTransaction(chain_transaction_two) != CASTLE_OK) ExitProcess(51u);

    byte_zero_(&binding_two, (CastleU32)sizeof(binding_two));
    binding_two.magic = CASTLE_HOOK_BINDING_MAGIC;
    binding_two.struct_size = CASTLE_SIZEOF_HOOK_BINDING_V1;
    binding_two.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    if (hook_api->GetHookBinding(chain_claim_two, &binding_two) != CASTLE_OK ||
        !binding_two.next_slot || *binding_two.next_slot != (void*)&test_original_) ExitProcess(52u);
    g_hook_two_next = binding_two.next_slot;
    if (g_pointer_slot != &test_hook_one_ ||
        *g_hook_one_next != (void*)&test_hook_two_ ||
        g_pointer_slot(5) != 116) ExitProcess(53u);

    byte_zero_(&diagnostics, (CastleU32)sizeof(diagnostics));
    diagnostics.magic = CASTLE_DIAGNOSTIC_BUFFER_MAGIC;
    diagnostics.struct_size = CASTLE_SIZEOF_DIAGNOSTIC_BUFFER_V1;
    diagnostics.buffer_version = CASTLE_DIAGNOSTIC_BUFFER_VERSION_1;
    if (api->CopyDiagnostics(0u, &diagnostics) != CASTLE_ERROR_BUFFER_TOO_SMALL ||
        diagnostics.required_capacity <= 1u) ExitProcess(22u);

    diagnostics.utf8_buffer = diagnostic_text;
    diagnostics.buffer_capacity = (CastleU32)sizeof(diagnostic_text);
    if (api->CopyDiagnostics(0u, &diagnostics) != CASTLE_OK ||
        diagnostics.bytes_written == 0u || diagnostic_text[0] == '\0') ExitProcess(23u);

    /* 故意先加载 B 再加载 A；Bootstrap 必须无视加载顺序，按 plugin_id 激活 A、B。 */
    if (!build_sibling_path_(bootstrap_plugin_a_path, 1024u,
            bootstrap_plugin_a_name) ||
        !build_sibling_path_(bootstrap_plugin_b_path, 1024u,
            bootstrap_plugin_b_name)) ExitProcess(131u);
    bootstrap_plugin_b = LoadLibraryW(bootstrap_plugin_b_path);
    bootstrap_plugin_a = LoadLibraryW(bootstrap_plugin_a_path);
    if (!bootstrap_plugin_a || !bootstrap_plugin_b) ExitProcess(132u);
    get_a_count = (RuntimeTestGetU32Fn)GetProcAddress(bootstrap_plugin_a,
        "RuntimeTest_GetBootstrapCount");
    get_a_mode = (RuntimeTestGetU32Fn)GetProcAddress(bootstrap_plugin_a,
        "RuntimeTest_GetBootstrapMode");
    get_a_handle = (RuntimeTestGetU32Fn)GetProcAddress(bootstrap_plugin_a,
        "RuntimeTest_GetBootstrapHandle");
    get_a_schedule_count = (RuntimeTestGetU32Fn)GetProcAddress(bootstrap_plugin_a,
        "RuntimeTest_GetScheduleCount");
    get_a_schedule_early_count = (RuntimeTestGetU32Fn)GetProcAddress(
        bootstrap_plugin_a, "RuntimeTest_GetScheduleEarlyCount");
    get_b_count = (RuntimeTestGetU32Fn)GetProcAddress(bootstrap_plugin_b,
        "RuntimeTest_GetBootstrapCount");
    get_b_mode = (RuntimeTestGetU32Fn)GetProcAddress(bootstrap_plugin_b,
        "RuntimeTest_GetBootstrapMode");
    get_b_handle = (RuntimeTestGetU32Fn)GetProcAddress(bootstrap_plugin_b,
        "RuntimeTest_GetBootstrapHandle");
    if (!get_a_count || !get_a_mode || !get_a_handle || !get_a_schedule_count ||
        !get_a_schedule_early_count ||
        !get_b_count || !get_b_mode || !get_b_handle) ExitProcess(133u);

    byte_zero_(&bootstrap_request, (CastleU32)sizeof(bootstrap_request));
    byte_zero_(&bootstrap_result, (CastleU32)sizeof(bootstrap_result));
    bootstrap_request.magic = CASTLE_BOOTSTRAP_REQUEST_MAGIC;
    bootstrap_request.struct_size = CASTLE_SIZEOF_BOOTSTRAP_REQUEST_V1;
    bootstrap_request.request_version = CASTLE_BOOTSTRAP_VERSION_1;
    /*
     * 模拟 ModLoader 两阶段：A 先登记 1ms 周期任务，B 故意延迟 100ms 才登记自己。
     * Runtime 必须等 A/B 的 Bootstrap 回调全部返回后才开闸，不能让 A 在 B 初始化中途运行。
     */
    bootstrap_request.trigger_kind = CASTLE_BOOTSTRAP_TRIGGER_INITIALIZE_ASI;
    bootstrap_request.trigger_module = descriptor.module;
    bootstrap_result.magic = CASTLE_BOOTSTRAP_RESULT_MAGIC;
    bootstrap_result.struct_size = CASTLE_SIZEOF_BOOTSTRAP_RESULT_V1;
    bootstrap_result.result_version = CASTLE_BOOTSTRAP_VERSION_1;

    result = api->BootstrapLoadedPlugins(&bootstrap_request, &bootstrap_result);
    if (result != CASTLE_OK || bootstrap_result.mode != CASTLE_BOOTSTRAP_MODE_INTEGRATED ||
        bootstrap_result.total_plugins != 2u || bootstrap_result.active_plugins != 2u ||
        bootstrap_result.failed_plugins != 0u || g_bootstrap_order_count != 2 ||
        g_bootstrap_order[0] != 1u || g_bootstrap_order[1] != 2u ||
        get_a_count() != 1u || get_b_count() != 1u ||
        get_a_mode() != CASTLE_CLIENT_BOOTSTRAP_INTEGRATED ||
        get_b_mode() != CASTLE_CLIENT_BOOTSTRAP_INTEGRATED ||
        get_a_handle() == 0u || get_b_handle() == 0u ||
        get_a_handle() == get_b_handle()) ExitProcess(24u);

    /*
     * Runtime 内部整批已返回，但模拟的 ModLoader 外层阶段2尚未发通知；任务仍必须保持0。
     * B 的100ms延迟同时保证旧“登记即运行”实现会稳定暴露。
     */
    Sleep(100u);
    if (get_a_schedule_count() != 0u ||
        get_a_schedule_early_count() != 0u) ExitProcess(135u);

    /* 模拟 ModLoader 在所有 InitializeASI 返回后调用 Client 通知桥。 */
    bootstrap_request.trigger_kind = CASTLE_BOOTSTRAP_TRIGGER_LOADER_READY;
    result = api->BootstrapLoadedPlugins(&bootstrap_request, &bootstrap_result);
    if (result != CASTLE_STATUS_ALREADY_DONE || get_a_count() != 1u ||
        get_b_count() != 1u || g_bootstrap_order_count != 2) ExitProcess(25u);

    /* 通知后任务必须启动；CI 繁忙时最多等待 1 秒获得首次时间片。 */
    for (gate_wait_attempt = 0u;
         gate_wait_attempt < 100u && get_a_schedule_count() == 0u;
         ++gate_wait_attempt) {
        Sleep(10u);
    }
    if (get_a_schedule_count() == 0u) ExitProcess(136u);
    if (get_a_schedule_early_count() != 0u) ExitProcess(135u);

    /* FreeLibrary 只能减少普通引用；已登记插件必须因 PIN 仍保持映射。 */
    FreeLibrary(bootstrap_plugin_a);
    FreeLibrary(bootstrap_plugin_b);
    if (!GetModuleHandleW(bootstrap_plugin_a_name) ||
        !GetModuleHandleW(bootstrap_plugin_b_name)) ExitProcess(134u);

    FreeLibrary(runtime_module);
    ExitProcess(0u);
}
