#include "runtime_internal.h"

static CastleResult CASTLE_RUNTIME_CALL runtime_get_info_(CastleRuntimeInfoV1* out_info);
static CastleResult CASTLE_RUNTIME_CALL runtime_bootstrap_(
    const CastleBootstrapRequestV1* request,
    CastleBootstrapResultV1* out_result);
static CastleResult CASTLE_RUNTIME_CALL runtime_register_plugin_(
    const CastlePluginDescriptorV1* descriptor,
    CastlePluginHandle* out_handle);
static CastleResult CASTLE_RUNTIME_CALL runtime_get_plugin_state_(
    CastlePluginHandle handle,
    CastlePluginStateV1* out_state);
static CastleResult CASTLE_RUNTIME_CALL runtime_query_interface_(
    const CastleInterfaceQueryV1* query,
    CastleInterfaceResultV1* out_result);
static CastleU32 CASTLE_RUNTIME_CALL runtime_get_diagnostic_generation_(void);
static CastleResult CASTLE_RUNTIME_CALL runtime_copy_diagnostics_(
    CastleU32 after_generation,
    CastleDiagnosticBufferV1* output);

/* 根能力位只宣告已经存在实现并通过测试宿主验证的公共服务。 */
static const CastleRuntimeApiV1 g_runtime_api_v1 = {
    CASTLE_RUNTIME_API_MAGIC,
    CASTLE_SIZEOF_RUNTIME_API_V1,
    CASTLE_RUNTIME_ABI_V1,
    0u, 1u, 0u,
    CASTLE_RUNTIME_CAP_PLUGIN_REGISTRY |
        CASTLE_RUNTIME_CAP_QUERY_INTERFACE |
        CASTLE_RUNTIME_CAP_DIAGNOSTICS |
        CASTLE_RUNTIME_CAP_HOOK_TRANSACTION |
        CASTLE_RUNTIME_CAP_HOOK_CHAIN |
        CASTLE_RUNTIME_CAP_PATH_V1 |
        CASTLE_RUNTIME_CAP_BACKGROUND_SCHEDULE_V1 |
        CASTLE_RUNTIME_CAP_SERVICE_PROVIDER |
        CASTLE_RUNTIME_CAP_DISPLAY_V1 |
        CASTLE_RUNTIME_CAP_WINDOW_V1 |
        CASTLE_RUNTIME_CAP_RENDER_V1 |
        CASTLE_RUNTIME_CAP_LOG_V1 |
        CASTLE_RUNTIME_CAP_CLOCK_V1 |
        CASTLE_RUNTIME_CAP_INPUT_V1 |
        CASTLE_RUNTIME_CAP_GAME_STATE_V1 |
        CASTLE_RUNTIME_CAP_SAVE_V1 |
        CASTLE_RUNTIME_CAP_OVERLAY_V1 |
        CASTLE_RUNTIME_CAP_FILE_V1 |
        CASTLE_RUNTIME_CAP_MODULE_V1 |
        CASTLE_RUNTIME_CAP_TOML_V1 |
        CASTLE_RUNTIME_CAP_GAME_PHASE_SCHEDULE_V1,
    0u,
    runtime_get_info_,
    runtime_bootstrap_,
    runtime_register_plugin_,
    runtime_get_plugin_state_,
    runtime_query_interface_,
    runtime_get_diagnostic_generation_,
    runtime_copy_diagnostics_
};

const CastleRuntimeApiV1* Runtime_GetApiV1(void) {
    return &g_runtime_api_v1;
}

static CastleResult CASTLE_RUNTIME_CALL runtime_get_info_(CastleRuntimeInfoV1* out_info) {
    if (!Runtime_EnsureInitialized()) return CASTLE_ERROR_RUNTIME_FAULT;
    if (!out_info || out_info->magic != CASTLE_RUNTIME_INFO_MAGIC ||
        out_info->struct_size < CASTLE_SIZEOF_RUNTIME_INFO_V1 ||
        out_info->info_version != CASTLE_RUNTIME_INFO_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    out_info->flags = 0u;
    out_info->runtime_version_major = g_runtime_api_v1.runtime_version_major;
    out_info->runtime_version_minor = g_runtime_api_v1.runtime_version_minor;
    out_info->runtime_version_patch = g_runtime_api_v1.runtime_version_patch;
    out_info->abi_version = g_runtime_api_v1.abi_version;
    out_info->capability_flags_low = g_runtime_api_v1.capability_flags_low;
    out_info->capability_flags_high = g_runtime_api_v1.capability_flags_high;
    out_info->process_id = GetCurrentProcessId();
    out_info->runtime_module = (CastleModule)(ULONG_PTR)g_runtime_module;
    out_info->game_module = Runtime_GetGameModuleValue();
    out_info->game_build_id = Runtime_GetGameBuildView();
    out_info->runtime_path = Runtime_GetPathView();
    out_info->diagnostic_generation = Runtime_DiagnosticGeneration();
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL runtime_bootstrap_(
    const CastleBootstrapRequestV1* request,
    CastleBootstrapResultV1* out_result) {
    if (!Runtime_EnsureInitialized()) return CASTLE_ERROR_RUNTIME_FAULT;
    return Runtime_BootstrapLoadedPlugins(request, out_result);
}

static CastleResult CASTLE_RUNTIME_CALL runtime_register_plugin_(
    const CastlePluginDescriptorV1* descriptor,
    CastlePluginHandle* out_handle) {
    if (!Runtime_EnsureInitialized()) return CASTLE_ERROR_RUNTIME_FAULT;
    return Runtime_RegisterPlugin(descriptor, out_handle);
}

static CastleResult CASTLE_RUNTIME_CALL runtime_get_plugin_state_(
    CastlePluginHandle handle,
    CastlePluginStateV1* out_state) {
    if (!Runtime_EnsureInitialized()) return CASTLE_ERROR_RUNTIME_FAULT;
    return Runtime_GetPluginStateByHandle(handle, out_state);
}

static CastleResult CASTLE_RUNTIME_CALL runtime_query_interface_(
    const CastleInterfaceQueryV1* query,
    CastleInterfaceResultV1* out_result) {
    if (!Runtime_EnsureInitialized()) return CASTLE_ERROR_RUNTIME_FAULT;
    if (!query || !out_result ||
        query->magic != CASTLE_QUERY_MAGIC ||
        query->struct_size < CASTLE_SIZEOF_INTERFACE_QUERY_V1 ||
        query->request_version != CASTLE_QUERY_VERSION_1 ||
        !query->interface_id.data || query->interface_id.length == 0u ||
        out_result->magic != CASTLE_INTERFACE_API_MAGIC ||
        out_result->struct_size < CASTLE_SIZEOF_INTERFACE_RESULT_V1 ||
        out_result->result_version != CASTLE_QUERY_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    out_result->flags = 0u;
    out_result->api_pointer = NULL;
    out_result->actual_version = 0u;
    out_result->actual_struct_size = 0u;
    out_result->capabilities_low = 0u;
    out_result->capabilities_high = 0u;
    out_result->provider_generation = 0u;

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_HOOK_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_HOOK_INTERFACE_ID) - 1u))) {
        const CastleHookApiV1* hook_api = Runtime_GetHookApiV1();
        if (query->requested_version != CASTLE_HOOK_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > hook_api->struct_size ||
            query->required_capabilities_low != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = hook_api;
        out_result->actual_version = hook_api->api_version;
        out_result->actual_struct_size = hook_api->struct_size;
        out_result->capabilities_low = hook_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_PATH_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_PATH_INTERFACE_ID) - 1u))) {
        const CastlePathApiV1* path_api = Runtime_GetPathApiV1();
        if (query->requested_version != CASTLE_PATH_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > path_api->struct_size ||
            query->required_capabilities_low != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = path_api;
        out_result->actual_version = path_api->api_version;
        out_result->actual_struct_size = path_api->struct_size;
        out_result->capabilities_low = path_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_SCHEDULE_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_SCHEDULE_INTERFACE_ID) - 1u))) {
        const CastleScheduleApiV1* schedule_api = Runtime_GetScheduleApiV1();
        if (query->requested_version != CASTLE_SCHEDULE_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > schedule_api->struct_size ||
            (query->required_capabilities_low & ~schedule_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = schedule_api;
        out_result->actual_version = schedule_api->api_version;
        out_result->actual_struct_size = schedule_api->struct_size;
        out_result->capabilities_low = schedule_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_DISPLAY_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_DISPLAY_INTERFACE_ID) - 1u))) {
        const CastleDisplayApiV1* display_api = Runtime_GetDisplayApiV1();
        if (query->requested_version != CASTLE_DISPLAY_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > display_api->struct_size ||
            (query->required_capabilities_low & ~display_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = display_api;
        out_result->actual_version = display_api->api_version;
        out_result->actual_struct_size = display_api->struct_size;
        out_result->capabilities_low = display_api->capability_flags;
        out_result->provider_generation = Runtime_GetDisplayProviderGeneration();
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_WINDOW_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_WINDOW_INTERFACE_ID) - 1u))) {
        const CastleWindowApiV1* window_api = Runtime_GetWindowApiV1();
        if (query->requested_version != CASTLE_WINDOW_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > window_api->struct_size ||
            (query->required_capabilities_low & ~window_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = window_api;
        out_result->actual_version = window_api->api_version;
        out_result->actual_struct_size = window_api->struct_size;
        out_result->capabilities_low = window_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_RENDER_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_RENDER_INTERFACE_ID) - 1u))) {
        const CastleRenderApiV1* render_api = Runtime_GetRenderApiV1();
        if (query->requested_version != CASTLE_RENDER_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > render_api->struct_size ||
            (query->required_capabilities_low & ~render_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = render_api;
        out_result->actual_version = render_api->api_version;
        out_result->actual_struct_size = render_api->struct_size;
        out_result->capabilities_low = render_api->capability_flags;
        out_result->provider_generation = Runtime_GetRenderProviderGeneration();
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_LOG_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_LOG_INTERFACE_ID) - 1u))) {
        const CastleLogApiV1* log_api = Runtime_GetLogApiV1();
        if (query->requested_version != CASTLE_LOG_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > log_api->struct_size ||
            (query->required_capabilities_low & ~log_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = log_api;
        out_result->actual_version = log_api->api_version;
        out_result->actual_struct_size = log_api->struct_size;
        out_result->capabilities_low = log_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_CLOCK_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_CLOCK_INTERFACE_ID) - 1u))) {
        const CastleClockApiV1* clock_api = Runtime_GetClockApiV1();
        if (query->requested_version != CASTLE_CLOCK_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > clock_api->struct_size ||
            (query->required_capabilities_low & ~clock_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = clock_api;
        out_result->actual_version = clock_api->api_version;
        out_result->actual_struct_size = clock_api->struct_size;
        out_result->capabilities_low = clock_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_INPUT_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_INPUT_INTERFACE_ID) - 1u))) {
        const CastleInputApiV1* input_api = Runtime_GetInputApiV1();
        if (query->requested_version != CASTLE_INPUT_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > input_api->struct_size ||
            (query->required_capabilities_low & ~input_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = input_api;
        out_result->actual_version = input_api->api_version;
        out_result->actual_struct_size = input_api->struct_size;
        out_result->capabilities_low = input_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_GAME_STATE_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_GAME_STATE_INTERFACE_ID) - 1u))) {
        const CastleGameStateApiV1* game_state_api = Runtime_GetGameStateApiV1();
        if (query->requested_version != CASTLE_GAME_STATE_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > game_state_api->struct_size ||
            (query->required_capabilities_low & ~game_state_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = game_state_api;
        out_result->actual_version = game_state_api->api_version;
        out_result->actual_struct_size = game_state_api->struct_size;
        out_result->capabilities_low = game_state_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_SAVE_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_SAVE_INTERFACE_ID) - 1u))) {
        const CastleSaveApiV1* save_api = Runtime_GetSaveApiV1();
        if (query->requested_version != CASTLE_SAVE_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > save_api->struct_size ||
            (query->required_capabilities_low & ~save_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = save_api;
        out_result->actual_version = save_api->api_version;
        out_result->actual_struct_size = save_api->struct_size;
        out_result->capabilities_low = save_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_OVERLAY_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_OVERLAY_INTERFACE_ID) - 1u))) {
        const CastleOverlayApiV1* overlay_api = Runtime_GetOverlayApiV1();
        if (query->requested_version != CASTLE_OVERLAY_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > overlay_api->struct_size ||
            (query->required_capabilities_low & ~overlay_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = overlay_api;
        out_result->actual_version = overlay_api->api_version;
        out_result->actual_struct_size = overlay_api->struct_size;
        out_result->capabilities_low = overlay_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_FILE_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_FILE_INTERFACE_ID) - 1u))) {
        const CastleFileApiV1* file_api = Runtime_GetFileApiV1();
        if (query->requested_version != CASTLE_FILE_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > file_api->struct_size ||
            (query->required_capabilities_low & ~file_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = file_api;
        out_result->actual_version = file_api->api_version;
        out_result->actual_struct_size = file_api->struct_size;
        out_result->capabilities_low = file_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_MODULE_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_MODULE_INTERFACE_ID) - 1u))) {
        const CastleModuleApiV1* module_api = Runtime_GetModuleApiV1();
        if (query->requested_version != CASTLE_MODULE_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > module_api->struct_size ||
            (query->required_capabilities_low & ~module_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = module_api;
        out_result->actual_version = module_api->api_version;
        out_result->actual_struct_size = module_api->struct_size;
        out_result->capabilities_low = module_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    if (Runtime_StringEquals(query->interface_id.data, query->interface_id.length,
                             CASTLE_TOML_INTERFACE_ID,
                             (CastleU32)(sizeof(CASTLE_TOML_INTERFACE_ID) - 1u))) {
        const CastleTomlApiV1* toml_api = Runtime_GetTomlApiV1();
        if (query->requested_version != CASTLE_TOML_API_VERSION_1) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        if (query->minimum_struct_size > toml_api->struct_size ||
            (query->required_capabilities_low & ~toml_api->capability_flags) != 0u ||
            query->required_capabilities_high != 0u) {
            return CASTLE_ERROR_INTERFACE_VERSION;
        }
        out_result->api_pointer = toml_api;
        out_result->actual_version = toml_api->api_version;
        out_result->actual_struct_size = toml_api->struct_size;
        out_result->capabilities_low = toml_api->capability_flags;
        out_result->provider_generation = 1u;
        return CASTLE_OK;
    }

    /* 未实现领域明确返回 NOT_FOUND，绝不能返回一张函数全为空的假接口。 */
    return CASTLE_ERROR_INTERFACE_NOT_FOUND;
}

static CastleU32 CASTLE_RUNTIME_CALL runtime_get_diagnostic_generation_(void) {
    if (!Runtime_EnsureInitialized()) return 0u;
    return Runtime_DiagnosticGeneration();
}

static CastleResult CASTLE_RUNTIME_CALL runtime_copy_diagnostics_(
    CastleU32 after_generation,
    CastleDiagnosticBufferV1* output) {
    if (!Runtime_EnsureInitialized()) return CASTLE_ERROR_RUNTIME_FAULT;
    return Runtime_CopyDiagnostics(after_generation, output);
}

/*
 * 唯一稳定导出。
 * 未知 ABI 直接返回 NULL，调用方不能拿 v1 表冒充另一个版本继续调用。
 */
__declspec(dllexport)
const CastleRuntimeApiV1* CASTLE_RUNTIME_CALL CastleRuntime_GetApi(
    CastleU32 requested_abi_version) {
    if (requested_abi_version != CASTLE_RUNTIME_ABI_V1) return NULL;
    if (!Runtime_EnsureInitialized()) return NULL;
    return &g_runtime_api_v1;
}
