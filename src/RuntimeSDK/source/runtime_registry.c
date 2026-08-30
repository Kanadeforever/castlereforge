#include "runtime_internal.h"

static volatile LONG g_registry_lock;
static RuntimePluginRecord g_plugins[RUNTIME_MAX_PLUGINS];
static CastleU32 g_registry_generation;

static int runtime_valid_plugin_id_(CastleStringView id) {
    CastleU32 index;
    if (!id.data || id.length == 0u || id.length >= RUNTIME_PLUGIN_ID_CAP) return 0;

    for (index = 0u; index < id.length; ++index) {
        char value = id.data[index];
        int allowed = (value >= 'a' && value <= 'z') ||
                      (value >= '0' && value <= '9') ||
                      value == '.' || value == '_' || value == '-';
        if (!allowed) return 0;
    }
    return 1;
}

static int runtime_copy_view_(char* output, CastleU32 capacity,
                              CastleU32* output_length, CastleStringView view,
                              int allow_empty) {
    CastleU32 index;

    if (!output || !output_length || capacity == 0u) return 0;
    if (!view.data) {
        if (!allow_empty || view.length != 0u) return 0;
        output[0] = '\0';
        *output_length = 0u;
        return 1;
    }
    if ((!allow_empty && view.length == 0u) || view.length >= capacity) return 0;

    for (index = 0u; index < view.length; ++index) output[index] = view.data[index];
    output[view.length] = '\0';
    *output_length = view.length;
    return 1;
}

void Runtime_RegistryInitialize(void) {
    Runtime_ByteZero(g_plugins, (CastleU32)sizeof(g_plugins));
    g_registry_lock = 0;
    g_registry_generation = 0u;
}

CastleResult Runtime_RegisterPlugin(const CastlePluginDescriptorV1* descriptor,
                                    CastlePluginHandle* out_handle) {
    CastleU32 index;
    RuntimePluginRecord* free_record = NULL;
    CastleResult result = CASTLE_OK;
    HMODULE verified_module = NULL;

    if (!descriptor || !out_handle ||
        descriptor->magic != CASTLE_PLUGIN_DESC_MAGIC ||
        descriptor->struct_size < CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1 ||
        descriptor->descriptor_version != CASTLE_PLUGIN_DESCRIPTOR_V1 ||
        descriptor->module == 0u ||
        !runtime_valid_plugin_id_(descriptor->plugin_id)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Runtime 会长期保存插件的 Hook、回调和 Provider 函数地址，因此登记成功后
     * 该模块必须固定驻留到进程结束。FROM_ADDRESS 还会确认调用方提交的数值确实
     * 属于当前已加载模块，而不是伪造或已经卸载的旧 HMODULE。
     */
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_PIN,
                            (LPCWSTR)(ULONG_PTR)descriptor->module,
                            &verified_module) ||
        verified_module != (HMODULE)(ULONG_PTR)descriptor->module) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    Runtime_Lock(&g_registry_lock);
    for (index = 0u; index < RUNTIME_MAX_PLUGINS; ++index) {
        RuntimePluginRecord* current = &g_plugins[index];
        if (!current->used) {
            if (!free_record) free_record = current;
            continue;
        }

        if (Runtime_StringEquals(current->plugin_id, current->plugin_id_length,
                                 descriptor->plugin_id.data,
                                 descriptor->plugin_id.length)) {
            if ((CastleModule)(ULONG_PTR)current->module == descriptor->module) {
                *out_handle = current->handle;
                Runtime_Unlock(&g_registry_lock);
                return CASTLE_STATUS_ALREADY_DONE;
            }
            Runtime_Unlock(&g_registry_lock);
            return CASTLE_ERROR_DUPLICATE_PLUGIN_ID;
        }
    }

    if (!free_record) {
        Runtime_Unlock(&g_registry_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }

    Runtime_ByteZero(free_record, (CastleU32)sizeof(*free_record));
    if (!runtime_copy_view_(free_record->plugin_id, RUNTIME_PLUGIN_ID_CAP,
                            &free_record->plugin_id_length,
                            descriptor->plugin_id, 0) ||
        !runtime_copy_view_(free_record->display_name, RUNTIME_DISPLAY_NAME_CAP,
                            &free_record->display_name_length,
                            descriptor->display_name, 1) ||
        !runtime_copy_view_(free_record->version_text, RUNTIME_VERSION_TEXT_CAP,
                            &free_record->version_text_length,
                            descriptor->version_text, 1) ||
        !runtime_copy_view_(free_record->build_id, RUNTIME_BUILD_ID_CAP,
                            &free_record->build_id_length,
                            descriptor->build_id, 1)) {
        Runtime_ByteZero(free_record, (CastleU32)sizeof(*free_record));
        result = CASTLE_ERROR_INVALID_ARGUMENT;
    } else {
        index = (CastleU32)(free_record - g_plugins);
        free_record->used = 1;
        free_record->module = (HMODULE)(ULONG_PTR)descriptor->module;
        free_record->handle = index + 1u;
        free_record->flags = descriptor->flags;
        free_record->state = CASTLE_PLUGIN_REGISTERED;
        free_record->last_result = CASTLE_OK;
        free_record->generation = ++g_registry_generation;
        *out_handle = free_record->handle;
    }

    Runtime_Unlock(&g_registry_lock);
    if (result == CASTLE_OK) Runtime_DiagnosticAppend("[Registry] plugin registered.");
    return result;
}

CastleResult Runtime_GetPluginStateByHandle(CastlePluginHandle handle,
                                            CastlePluginStateV1* out_state) {
    RuntimePluginRecord* record;

    if (!out_state || out_state->magic != CASTLE_PLUGIN_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_PLUGIN_STATE_V1 ||
        out_state->state_version != CASTLE_PLUGIN_STATE_VERSION_1 ||
        handle == 0u || handle > RUNTIME_MAX_PLUGINS) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    Runtime_Lock(&g_registry_lock);
    record = &g_plugins[handle - 1u];
    if (!record->used || record->handle != handle) {
        Runtime_Unlock(&g_registry_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    out_state->flags = 0u;
    out_state->plugin_handle = record->handle;
    out_state->state = record->state;
    out_state->state_flags = record->state_flags;
    out_state->last_result = record->last_result;
    out_state->plugin_generation = record->generation;
    out_state->active_claim_count = record->active_claim_count;
    out_state->provider_count = record->provider_count;
    Runtime_Unlock(&g_registry_lock);
    return CASTLE_OK;
}

void Runtime_SetPluginState(CastlePluginHandle handle, CastleU32 state,
                            CastleResult result) {
    RuntimePluginRecord* record;
    if (handle == 0u || handle > RUNTIME_MAX_PLUGINS) return;

    Runtime_Lock(&g_registry_lock);
    record = &g_plugins[handle - 1u];
    if (record->used && record->handle == handle) {
        record->state = state;
        record->last_result = result;
        record->generation = ++g_registry_generation;
    }
    Runtime_Unlock(&g_registry_lock);
}

HMODULE Runtime_GetPluginModule(CastlePluginHandle handle) {
    HMODULE module = NULL;
    if (handle == 0u || handle > RUNTIME_MAX_PLUGINS) return NULL;

    Runtime_Lock(&g_registry_lock);
    if (g_plugins[handle - 1u].used) module = g_plugins[handle - 1u].module;
    Runtime_Unlock(&g_registry_lock);
    return module;
}

CastleStringView Runtime_GetPluginIdView(CastlePluginHandle handle) {
    CastleStringView view;
    view.data = NULL;
    view.length = 0u;
    if (handle == 0u || handle > RUNTIME_MAX_PLUGINS) return view;

    Runtime_Lock(&g_registry_lock);
    if (g_plugins[handle - 1u].used) {
        view.data = g_plugins[handle - 1u].plugin_id;
        view.length = g_plugins[handle - 1u].plugin_id_length;
    }
    Runtime_Unlock(&g_registry_lock);
    return view;
}
