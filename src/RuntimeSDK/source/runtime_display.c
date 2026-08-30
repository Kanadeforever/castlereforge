#include "runtime_internal.h"

#define RUNTIME_DISPLAY_PROVIDER_ID_CAP 128u

typedef struct RuntimeDisplayProvider {
    int used;
    CastleProviderHandle handle;
    CastlePluginHandle plugin;
    const CastleDisplayProviderV1* api;
    CastleU32 state;
    CastleU32 has_geometry;
    CastleU32 registration_generation;
    CastleDisplayGeometryV1 geometry;
    char provider_id[RUNTIME_DISPLAY_PROVIDER_ID_CAP];
    CastleU32 provider_id_length;
} RuntimeDisplayProvider;

static volatile LONG g_display_lock;
static CastleU32 g_display_generation;
static CastleU32 g_provider_generation;
static CastleU32 g_default_ready;
static CastleDisplayGeometryV1 g_default_geometry;
static RuntimeDisplayProvider g_external_provider;
static const char g_default_provider_id[] = "org.castlereforge.display.original";

static CastleResult CASTLE_RUNTIME_CALL display_get_state_(CastleDisplayStateV1* out_state);
static CastleResult CASTLE_RUNTIME_CALL display_get_geometry_(
    CastleDisplayGeometryV1* out_geometry);
static CastleResult CASTLE_RUNTIME_CALL display_world_to_screen_(
    const CastleWorldToScreenRequestV1* request,
    CastleScreenProjectionV1* out_projection);
static CastleResult CASTLE_RUNTIME_CALL display_screen_to_world_(
    const CastleScreenToWorldRequestV1* request,
    CastleWorldProjectionV1* out_projection);
static CastleResult CASTLE_RUNTIME_CALL display_get_provider_info_(
    CastleDisplayProviderInfoV1* out_info);
static CastleResult CASTLE_RUNTIME_CALL display_register_provider_(
    CastlePluginHandle plugin, CastleStringView provider_id,
    const CastleDisplayProviderV1* provider_api,
    CastleProviderHandle* out_provider);
static CastleResult CASTLE_RUNTIME_CALL display_publish_geometry_(
    CastleProviderHandle provider, const CastleDisplayGeometryV1* geometry,
    CastleU32* out_runtime_generation);
static CastleResult CASTLE_RUNTIME_CALL display_set_provider_ready_(
    CastleProviderHandle provider, CastleU32 ready);
static CastleResult CASTLE_RUNTIME_CALL display_get_provider_state_(
    CastleProviderHandle provider, CastleDisplayStateV1* out_state);

static const CastleDisplayApiV1 g_display_api = {
    CASTLE_DISPLAY_API_MAGIC,
    CASTLE_SIZEOF_DISPLAY_API_V1,
    CASTLE_DISPLAY_API_VERSION_1,
    CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD | CASTLE_DISPLAY_CAP_EXTERNAL_PROVIDER,
    display_get_state_,
    display_get_geometry_,
    display_world_to_screen_,
    display_screen_to_world_,
    display_get_provider_info_,
    display_register_provider_,
    display_publish_geometry_,
    display_set_provider_ready_,
    display_get_provider_state_
};

static CastleU32 display_next_generation_locked_(void) {
    ++g_display_generation;
    if (g_display_generation == 0u) ++g_display_generation;
    return g_display_generation;
}

static int display_valid_provider_id_(CastleStringView id) {
    CastleU32 index;
    if (!id.data || id.length == 0u || id.length >= RUNTIME_DISPLAY_PROVIDER_ID_CAP) {
        return 0;
    }
    for (index = 0u; index < id.length; ++index) {
        char value = id.data[index];
        int allowed = (value >= 'a' && value <= 'z') ||
                      (value >= '0' && value <= '9') || value == '.' ||
                      value == '_' || value == '-';
        if (!allowed) return 0;
    }
    return 1;
}

static int display_pointer_in_module_(const void* pointer, HMODULE module) {
    MEMORY_BASIC_INFORMATION information;
    if (!pointer || !module ||
        VirtualQuery(pointer, &information, sizeof(information)) == 0u) return 0;
    return information.State == MEM_COMMIT && information.AllocationBase == module;
}

/* Provider 结构和所有必需回调都必须真实位于已登记插件模块中。 */
static int display_valid_provider_api_(CastlePluginHandle plugin,
                                       const CastleDisplayProviderV1* api) {
    HMODULE module = Runtime_GetPluginModule(plugin);
    if (!module || !api || api->magic != CASTLE_DISPLAY_PROVIDER_MAGIC ||
        api->struct_size < CASTLE_SIZEOF_DISPLAY_PROVIDER_V1 ||
        api->api_version != CASTLE_DISPLAY_API_VERSION_1 ||
        !display_pointer_in_module_(api, module) ||
        !display_pointer_in_module_((const void*)api->GetProviderState, module) ||
        !display_pointer_in_module_((const void*)api->CopyCurrentGeometry, module) ||
        !display_pointer_in_module_((const void*)api->WorldToScreen, module)) return 0;
    if ((api->capability_flags & ~CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD) != 0u) return 0;
    if ((api->capability_flags & CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD) != 0u) {
        if (!display_pointer_in_module_((const void*)api->ScreenToWorld, module)) return 0;
    } else if (api->ScreenToWorld != NULL) {
        return 0;
    }
    return 1;
}

static int display_valid_geometry_(const CastleDisplayGeometryV1* geometry) {
    CastleS32 center_right;
    CastleS32 center_bottom;
    if (!geometry || geometry->magic != CASTLE_DISPLAY_GEOMETRY_MAGIC ||
        geometry->struct_size < CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1 ||
        geometry->api_version != CASTLE_DISPLAY_API_VERSION_1 ||
        geometry->output_width == 0u || geometry->output_width > 8192u ||
        geometry->output_height == 0u || geometry->output_height > 8192u ||
        geometry->logical_width == 0u || geometry->logical_height == 0u ||
        geometry->center_width <= 0 || geometry->center_height <= 0 ||
        geometry->center_x < 0 || geometry->center_y < 0 ||
        geometry->display_mode > CASTLE_DISPLAY_TRANSITION ||
        geometry->projection_scope > CASTLE_PROJECTION_FULL_OUTPUT ||
        geometry->left_world_width > geometry->output_width ||
        geometry->right_world_width > geometry->output_width) return 0;
    center_right = geometry->center_x + geometry->center_width;
    center_bottom = geometry->center_y + geometry->center_height;
    if (center_right < geometry->center_x || center_bottom < geometry->center_y ||
        (CastleU32)center_right > geometry->output_width ||
        (CastleU32)center_bottom > geometry->output_height) return 0;
    if ((geometry->display_mode == CASTLE_DISPLAY_UNKNOWN ||
         geometry->display_mode == CASTLE_DISPLAY_TRANSITION) &&
        geometry->projection_scope != CASTLE_PROJECTION_NONE) return 0;
    return 1;
}

static void display_fill_default_locked_(CastleS32 camera_x, CastleS32 camera_y) {
    int changed = !g_default_ready ||
        g_default_geometry.effective_camera_x != camera_x ||
        g_default_geometry.effective_camera_y != camera_y;
    if (changed) g_default_geometry.generation = display_next_generation_locked_();
    g_default_geometry.magic = CASTLE_DISPLAY_GEOMETRY_MAGIC;
    g_default_geometry.struct_size = CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1;
    g_default_geometry.api_version = CASTLE_DISPLAY_API_VERSION_1;
    g_default_geometry.flags = 0u;
    g_default_geometry.frame_id = 0u;
    g_default_geometry.output_width = 640u;
    g_default_geometry.output_height = 480u;
    g_default_geometry.logical_width = 640u;
    g_default_geometry.logical_height = 480u;
    g_default_geometry.center_x = 0;
    g_default_geometry.center_y = 0;
    g_default_geometry.center_width = 640;
    g_default_geometry.center_height = 480;
    g_default_geometry.effective_camera_x = camera_x;
    g_default_geometry.effective_camera_y = camera_y;
    g_default_geometry.original_camera_x = camera_x;
    g_default_geometry.original_camera_y = camera_y;
    g_default_geometry.left_world_width = 0u;
    g_default_geometry.right_world_width = 0u;
    g_default_geometry.display_mode = CASTLE_DISPLAY_ORIGINAL_4_3;
    g_default_geometry.projection_scope = CASTLE_PROJECTION_CENTER_640;
    g_default_geometry.transition_value = 0u;
    g_default_geometry.transition_max = 0u;
    g_default_ready = 1u;
}

static void display_refresh_default_locked_(void) {
    CastleS32 camera_x;
    CastleS32 camera_y;
    if (Runtime_ReadOriginalCamera(&camera_x, &camera_y)) {
        display_fill_default_locked_(camera_x, camera_y);
    }
}

/* 锁内只复制当前快照；专业后端回调永远在解锁后执行。 */
static CastleResult display_copy_current_locked_(CastleDisplayGeometryV1* geometry,
                                                 CastlePluginHandle* out_plugin,
                                                 const CastleDisplayProviderV1** out_api) {
    if (g_external_provider.used &&
        g_external_provider.state == CASTLE_DISPLAY_PROVIDER_READY &&
        g_external_provider.has_geometry) {
        *geometry = g_external_provider.geometry;
        if (out_plugin) *out_plugin = g_external_provider.plugin;
        if (out_api) *out_api = g_external_provider.api;
        return CASTLE_OK;
    }
    display_refresh_default_locked_();
    if (!g_default_ready) return CASTLE_ERROR_NOT_READY;
    *geometry = g_default_geometry;
    if (out_plugin) *out_plugin = 0u;
    if (out_api) *out_api = NULL;
    return CASTLE_STATUS_DEFAULT_BACKEND;
}

static CastleU32 display_visibility_(CastleS32 x, CastleS32 y,
                                     const CastleDisplayGeometryV1* geometry) {
    CastleS32 left;
    CastleS32 top;
    CastleS32 right;
    CastleS32 bottom;
    if (geometry->projection_scope == CASTLE_PROJECTION_NONE ||
        geometry->display_mode == CASTLE_DISPLAY_UNKNOWN ||
        geometry->display_mode == CASTLE_DISPLAY_TRANSITION) {
        return CASTLE_VISIBILITY_NOT_PROJECTABLE;
    }
    if (geometry->projection_scope == CASTLE_PROJECTION_CENTER_640) {
        left = geometry->center_x;
        top = geometry->center_y;
        right = left + geometry->center_width;
        bottom = top + geometry->center_height;
    } else {
        left = 0;
        top = 0;
        right = (CastleS32)geometry->output_width;
        bottom = (CastleS32)geometry->output_height;
    }
    if (x < left) return CASTLE_VISIBILITY_OFFSCREEN_LEFT;
    if (x >= right) return CASTLE_VISIBILITY_OFFSCREEN_RIGHT;
    if (y < top) return CASTLE_VISIBILITY_OFFSCREEN_TOP;
    if (y >= bottom) return CASTLE_VISIBILITY_OFFSCREEN_BOTTOM;
    return CASTLE_VISIBILITY_VISIBLE;
}

void Runtime_DisplayInitialize(void) {
    CastleS32 camera_x;
    CastleS32 camera_y;
    g_display_lock = 0;
    g_display_generation = 0u;
    /* 代次 1 代表 Runtime 原版默认后端；首个外部后端从 2 开始。 */
    g_provider_generation = 1u;
    g_default_ready = 0u;
    Runtime_ByteZero(&g_default_geometry, (CastleU32)sizeof(g_default_geometry));
    Runtime_ByteZero(&g_external_provider, (CastleU32)sizeof(g_external_provider));
    if (Runtime_ReadOriginalCamera(&camera_x, &camera_y)) {
        display_fill_default_locked_(camera_x, camera_y);
    }
}

const CastleDisplayApiV1* Runtime_GetDisplayApiV1(void) {
    return &g_display_api;
}

CastleU32 Runtime_GetDisplayProviderGeneration(void) {
    CastleU32 generation;
    Runtime_Lock(&g_display_lock);
    generation = g_provider_generation;
    Runtime_Unlock(&g_display_lock);
    return generation;
}

int Runtime_DisplayProviderReadyForPlugin(CastleProviderHandle provider,
                                          CastlePluginHandle plugin,
                                          CastleU32* out_provider_generation,
                                          CastleU32* out_geometry_generation) {
    int ready = 0;
    Runtime_Lock(&g_display_lock);
    if (g_external_provider.used && g_external_provider.handle == provider &&
        g_external_provider.plugin == plugin &&
        g_external_provider.state == CASTLE_DISPLAY_PROVIDER_READY &&
        g_external_provider.has_geometry) {
        if (out_provider_generation) {
            *out_provider_generation = g_external_provider.registration_generation;
        }
        if (out_geometry_generation) {
            *out_geometry_generation = g_external_provider.geometry.generation;
        }
        ready = 1;
    }
    Runtime_Unlock(&g_display_lock);
    return ready;
}

CastleResult Runtime_GetCurrentDisplayGeneration(CastleU32* out_generation) {
    CastleDisplayGeometryV1 geometry;
    CastleResult result;
    if (!out_generation) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_display_lock);
    result = display_copy_current_locked_(&geometry, NULL, NULL);
    if (result >= 0) *out_generation = geometry.generation;
    Runtime_Unlock(&g_display_lock);
    return result;
}

static CastleResult CASTLE_RUNTIME_CALL display_get_state_(CastleDisplayStateV1* out_state) {
    CastleDisplayGeometryV1 geometry;
    CastlePluginHandle plugin = 0u;
    CastleResult result;
    if (!out_state || out_state->magic != CASTLE_DISPLAY_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_DISPLAY_STATE_V1 ||
        out_state->version != CASTLE_DISPLAY_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_display_lock);
    result = display_copy_current_locked_(&geometry, &plugin, NULL);
    out_state->flags = 0u;
    out_state->ready = result < 0 ? 0u : 1u;
    out_state->generation = result < 0 ? g_display_generation : geometry.generation;
    out_state->backend_plugin = plugin;
    out_state->display_mode = result < 0 ? CASTLE_DISPLAY_UNKNOWN : geometry.display_mode;
    Runtime_Unlock(&g_display_lock);
    return result;
}

static CastleResult CASTLE_RUNTIME_CALL display_get_geometry_(
    CastleDisplayGeometryV1* out_geometry) {
    CastleDisplayGeometryV1 geometry;
    CastleResult result;
    if (!out_geometry || out_geometry->magic != CASTLE_DISPLAY_GEOMETRY_MAGIC ||
        out_geometry->struct_size < CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1 ||
        out_geometry->api_version != CASTLE_DISPLAY_API_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_display_lock);
    result = display_copy_current_locked_(&geometry, NULL, NULL);
    if (result >= 0) *out_geometry = geometry;
    Runtime_Unlock(&g_display_lock);
    return result;
}

static CastleResult display_project_default_(const CastleWorldToScreenRequestV1* request,
                                              CastleScreenProjectionV1* output,
                                              const CastleDisplayGeometryV1* geometry) {
    output->flags = 0u;
    output->actual_generation = geometry->generation;
    output->screen_x = request->world_x - geometry->effective_camera_x + geometry->center_x;
    output->screen_y = request->world_y - geometry->effective_camera_y + geometry->center_y;
    output->visibility = display_visibility_(output->screen_x, output->screen_y, geometry);
    output->projection_scope = geometry->projection_scope;
    return output->visibility == CASTLE_VISIBILITY_NOT_PROJECTABLE ?
        CASTLE_ERROR_NOT_READY : CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL display_world_to_screen_(
    const CastleWorldToScreenRequestV1* request,
    CastleScreenProjectionV1* out_projection) {
    CastleDisplayGeometryV1 geometry;
    const CastleDisplayProviderV1* provider_api = NULL;
    CastlePluginHandle provider_plugin = 0u;
    CastleScreenProjectionV1 provider_output;
    CastleWorldToScreenRequestV1 provider_request;
    CastleResult result;
    if (!request || !out_projection ||
        request->magic != CASTLE_WORLD_TO_SCREEN_MAGIC ||
        request->struct_size < CASTLE_SIZEOF_WORLD_TO_SCREEN_V1 ||
        request->request_version != CASTLE_DISPLAY_STRUCTURE_VERSION_1 ||
        out_projection->magic != CASTLE_SCREEN_PROJECTION_MAGIC ||
        out_projection->struct_size < CASTLE_SIZEOF_SCREEN_PROJECTION_V1 ||
        out_projection->result_version != CASTLE_DISPLAY_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_display_lock);
    result = display_copy_current_locked_(&geometry, &provider_plugin, &provider_api);
    if (result >= 0 && request->requested_generation != 0u &&
        request->requested_generation != geometry.generation) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_STALE_GENERATION;
    }
    Runtime_Unlock(&g_display_lock);
    if (result < 0) return result;
    if (!provider_api) return display_project_default_(request, out_projection, &geometry);

    provider_request = *request;
    provider_request.requested_generation = geometry.generation;
    provider_output = *out_projection;
    result = provider_api->WorldToScreen(&provider_request, &provider_output);
    if (result < 0) return result;
    if (provider_output.magic != CASTLE_SCREEN_PROJECTION_MAGIC ||
        provider_output.struct_size < CASTLE_SIZEOF_SCREEN_PROJECTION_V1 ||
        provider_output.result_version != CASTLE_DISPLAY_STRUCTURE_VERSION_1 ||
        provider_output.actual_generation != geometry.generation ||
        provider_output.visibility > CASTLE_VISIBILITY_NOT_PROJECTABLE ||
        provider_output.projection_scope > CASTLE_PROJECTION_FULL_OUTPUT) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    Runtime_Lock(&g_display_lock);
    if (!g_external_provider.used ||
        g_external_provider.plugin != provider_plugin ||
        g_external_provider.state != CASTLE_DISPLAY_PROVIDER_READY ||
        g_external_provider.geometry.generation != geometry.generation) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_STALE_GENERATION;
    }
    Runtime_Unlock(&g_display_lock);
    *out_projection = provider_output;
    return result;
}

static CastleResult CASTLE_RUNTIME_CALL display_screen_to_world_(
    const CastleScreenToWorldRequestV1* request,
    CastleWorldProjectionV1* out_projection) {
    CastleDisplayGeometryV1 geometry;
    const CastleDisplayProviderV1* provider_api = NULL;
    CastlePluginHandle provider_plugin = 0u;
    CastleWorldProjectionV1 provider_output;
    CastleScreenToWorldRequestV1 provider_request;
    CastleResult result;
    if (!request || !out_projection ||
        request->magic != CASTLE_SCREEN_TO_WORLD_MAGIC ||
        request->struct_size < CASTLE_SIZEOF_SCREEN_TO_WORLD_V1 ||
        request->request_version != CASTLE_DISPLAY_STRUCTURE_VERSION_1 ||
        out_projection->magic != CASTLE_WORLD_PROJECTION_MAGIC ||
        out_projection->struct_size < CASTLE_SIZEOF_WORLD_PROJECTION_V1 ||
        out_projection->result_version != CASTLE_DISPLAY_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_display_lock);
    result = display_copy_current_locked_(&geometry, &provider_plugin, &provider_api);
    if (result >= 0 && request->requested_generation != 0u &&
        request->requested_generation != geometry.generation) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_STALE_GENERATION;
    }
    Runtime_Unlock(&g_display_lock);
    if (result < 0) return result;
    if (provider_api) {
        if ((provider_api->capability_flags & CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD) == 0u ||
            !provider_api->ScreenToWorld) return CASTLE_STATUS_OPTIONAL_UNAVAILABLE;
        provider_request = *request;
        provider_request.requested_generation = geometry.generation;
        provider_output = *out_projection;
        result = provider_api->ScreenToWorld(&provider_request, &provider_output);
        if (result < 0) return result;
        if (provider_output.magic != CASTLE_WORLD_PROJECTION_MAGIC ||
            provider_output.struct_size < CASTLE_SIZEOF_WORLD_PROJECTION_V1 ||
            provider_output.result_version != CASTLE_DISPLAY_STRUCTURE_VERSION_1 ||
            provider_output.actual_generation != geometry.generation ||
            provider_output.visibility > CASTLE_VISIBILITY_NOT_PROJECTABLE ||
            provider_output.projection_scope > CASTLE_PROJECTION_FULL_OUTPUT) {
            return CASTLE_ERROR_RUNTIME_FAULT;
        }
        Runtime_Lock(&g_display_lock);
        if (!g_external_provider.used || g_external_provider.plugin != provider_plugin ||
            g_external_provider.state != CASTLE_DISPLAY_PROVIDER_READY ||
            g_external_provider.geometry.generation != geometry.generation) {
            Runtime_Unlock(&g_display_lock);
            return CASTLE_ERROR_STALE_GENERATION;
        }
        Runtime_Unlock(&g_display_lock);
        *out_projection = provider_output;
        return result;
    }

    out_projection->flags = 0u;
    out_projection->actual_generation = geometry.generation;
    out_projection->world_x = request->screen_x - geometry.center_x +
                              geometry.effective_camera_x;
    out_projection->world_y = request->screen_y - geometry.center_y +
                              geometry.effective_camera_y;
    out_projection->visibility = display_visibility_(request->screen_x,
                                                      request->screen_y, &geometry);
    out_projection->projection_scope = geometry.projection_scope;
    return out_projection->visibility == CASTLE_VISIBILITY_NOT_PROJECTABLE ?
        CASTLE_ERROR_NOT_READY : CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL display_get_provider_info_(
    CastleDisplayProviderInfoV1* out_info) {
    if (!out_info || out_info->magic != CASTLE_DISPLAY_PROVIDER_INFO_MAGIC ||
        out_info->struct_size < CASTLE_SIZEOF_DISPLAY_PROVIDER_INFO_V1 ||
        out_info->version != CASTLE_DISPLAY_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_display_lock);
    out_info->flags = 0u;
    if (g_external_provider.used) {
        out_info->provider_handle = g_external_provider.handle;
        out_info->plugin_handle = g_external_provider.plugin;
        out_info->generation = g_external_provider.registration_generation;
        out_info->state = g_external_provider.state;
        out_info->provider_id.data = g_external_provider.provider_id;
        out_info->provider_id.length = g_external_provider.provider_id_length;
        Runtime_Unlock(&g_display_lock);
        return CASTLE_OK;
    }
    display_refresh_default_locked_();
    out_info->provider_handle = 0u;
    out_info->plugin_handle = 0u;
    out_info->generation = 1u;
    out_info->state = g_default_ready ? CASTLE_DISPLAY_PROVIDER_READY :
                                        CASTLE_DISPLAY_PROVIDER_NOT_READY;
    out_info->provider_id.data = g_default_provider_id;
    out_info->provider_id.length = (CastleU32)(sizeof(g_default_provider_id) - 1u);
    Runtime_Unlock(&g_display_lock);
    return CASTLE_STATUS_DEFAULT_BACKEND;
}

static CastleResult CASTLE_RUNTIME_CALL display_register_provider_(
    CastlePluginHandle plugin, CastleStringView provider_id,
    const CastleDisplayProviderV1* provider_api,
    CastleProviderHandle* out_provider) {
    CastleU32 index;
    if (!out_provider || !display_valid_provider_id_(provider_id) ||
        !display_valid_provider_api_(plugin, provider_api)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_display_lock);
    if (g_external_provider.used) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    Runtime_ByteZero(&g_external_provider, (CastleU32)sizeof(g_external_provider));
    ++g_provider_generation;
    if (g_provider_generation == 0u) ++g_provider_generation;
    g_external_provider.used = 1;
    g_external_provider.handle = 1u;
    g_external_provider.plugin = plugin;
    g_external_provider.api = provider_api;
    g_external_provider.state = CASTLE_DISPLAY_PROVIDER_NOT_READY;
    g_external_provider.registration_generation = g_provider_generation;
    for (index = 0u; index < provider_id.length; ++index) {
        g_external_provider.provider_id[index] = provider_id.data[index];
    }
    g_external_provider.provider_id[provider_id.length] = '\0';
    g_external_provider.provider_id_length = provider_id.length;
    *out_provider = g_external_provider.handle;
    Runtime_Unlock(&g_display_lock);
    Runtime_DiagnosticAppend("[Display] external provider registered.");
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL display_publish_geometry_(
    CastleProviderHandle provider, const CastleDisplayGeometryV1* geometry,
    CastleU32* out_runtime_generation) {
    if (!out_runtime_generation || !display_valid_geometry_(geometry)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_display_lock);
    if (!g_external_provider.used || g_external_provider.handle != provider) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    g_external_provider.geometry = *geometry;
    g_external_provider.geometry.generation = display_next_generation_locked_();
    g_external_provider.has_geometry = 1u;
    *out_runtime_generation = g_external_provider.geometry.generation;
    Runtime_Unlock(&g_display_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL display_set_provider_ready_(
    CastleProviderHandle provider, CastleU32 ready) {
    const CastleDisplayProviderV1* provider_api;
    CastleDisplayStateV1 provider_state;
    CastleResult result;
    if (ready > 1u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_display_lock);
    if (!g_external_provider.used || g_external_provider.handle != provider) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!ready) {
        g_external_provider.state = CASTLE_DISPLAY_PROVIDER_NOT_READY;
        Runtime_Unlock(&g_display_lock);
        return CASTLE_OK;
    }
    if (!g_external_provider.has_geometry) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_NOT_READY;
    }
    provider_api = g_external_provider.api;
    Runtime_Unlock(&g_display_lock);

    Runtime_ByteZero(&provider_state, (CastleU32)sizeof(provider_state));
    provider_state.magic = CASTLE_DISPLAY_STATE_MAGIC;
    provider_state.struct_size = CASTLE_SIZEOF_DISPLAY_STATE_V1;
    provider_state.version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    result = provider_api->GetProviderState(&provider_state);
    if (result < 0 || provider_state.ready == 0u) return CASTLE_ERROR_NOT_READY;

    Runtime_Lock(&g_display_lock);
    if (!g_external_provider.used || g_external_provider.handle != provider ||
        g_external_provider.api != provider_api || !g_external_provider.has_geometry) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_STALE_GENERATION;
    }
    g_external_provider.state = CASTLE_DISPLAY_PROVIDER_READY;
    Runtime_Unlock(&g_display_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL display_get_provider_state_(
    CastleProviderHandle provider, CastleDisplayStateV1* out_state) {
    if (!out_state || out_state->magic != CASTLE_DISPLAY_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_DISPLAY_STATE_V1 ||
        out_state->version != CASTLE_DISPLAY_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_display_lock);
    if (!g_external_provider.used || g_external_provider.handle != provider) {
        Runtime_Unlock(&g_display_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    out_state->flags = 0u;
    out_state->ready = g_external_provider.state == CASTLE_DISPLAY_PROVIDER_READY;
    out_state->generation = g_external_provider.has_geometry ?
        g_external_provider.geometry.generation : g_external_provider.registration_generation;
    out_state->backend_plugin = g_external_provider.plugin;
    out_state->display_mode = g_external_provider.has_geometry ?
        g_external_provider.geometry.display_mode : CASTLE_DISPLAY_UNKNOWN;
    Runtime_Unlock(&g_display_lock);
    return CASTLE_OK;
}
