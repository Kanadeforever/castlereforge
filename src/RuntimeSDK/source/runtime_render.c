#include "runtime_internal.h"

#define RUNTIME_RENDER_PROVIDER_ID_CAP 128u

/* x86 fastcall 的前两个参数进入 ECX/EDX，可安全调用“只有 ECX=this”的原版成员函数。 */
typedef void (__fastcall *RuntimeRenderOriginalFn)(void* render_context,
                                                    void* unused_edx);

typedef struct RuntimeRenderProvider {
    int used;
    CastleProviderHandle handle;
    CastlePluginHandle plugin;
    CastleProviderHandle display_provider;
    const CastleRenderProviderV1* api;
    CastleU32 state;
    CastleU32 generation;
    CastleU32 display_provider_generation;
    char provider_id[RUNTIME_RENDER_PROVIDER_ID_CAP];
    CastleU32 provider_id_length;
} RuntimeRenderProvider;

static volatile LONG g_render_lock;
static RuntimeRenderProvider g_render_provider;
static CastleU32 g_render_generation;
static RuntimeRenderOriginalFn g_original_render_queue;
static RuntimeRenderOriginalFn g_original_present;
static CastleLeaseHandle g_extra_frame_lease;
static CastlePluginHandle g_extra_frame_owner;
static CastleU32 g_extra_frame_display_generation;
static CastleU32 g_extra_frame_generation;

static CastleResult CASTLE_RUNTIME_CALL render_get_state_(CastleRenderStateV1* out_state);
static CastleResult CASTLE_RUNTIME_CALL render_current_queue_(const CastleRenderCallV1* call);
static CastleResult CASTLE_RUNTIME_CALL render_present_(const CastleRenderCallV1* call);
static CastleResult CASTLE_RUNTIME_CALL render_begin_extra_(CastlePluginHandle plugin,
    CastleU32 requested_display_generation, CastleLeaseHandle* out_lease,
    CastleU32* out_display_generation);
static CastleResult CASTLE_RUNTIME_CALL render_end_extra_(CastleLeaseHandle lease);
static CastleResult CASTLE_RUNTIME_CALL render_register_provider_(CastlePluginHandle plugin,
    CastleStringView provider_id, CastleProviderHandle display_provider,
    const CastleRenderProviderV1* provider_api, CastleProviderHandle* out_provider);
static CastleResult CASTLE_RUNTIME_CALL render_set_provider_ready_(
    CastleProviderHandle provider, CastleU32 ready);
static CastleResult CASTLE_RUNTIME_CALL render_get_provider_state_(
    CastleProviderHandle provider, CastleRenderStateV1* out_state);

static const CastleRenderApiV1 g_render_api = {
    CASTLE_RENDER_API_MAGIC,
    CASTLE_SIZEOF_RENDER_API_V1,
    CASTLE_RENDER_API_VERSION_1,
    CASTLE_RENDER_CAP_EXTERNAL_PROVIDER | CASTLE_RENDER_CAP_EXTRA_FRAME_LEASE,
    render_get_state_,
    render_current_queue_,
    render_present_,
    render_begin_extra_,
    render_end_extra_,
    render_register_provider_,
    render_set_provider_ready_,
    render_get_provider_state_
};

static CastleU32 render_next_generation_locked_(void) {
    ++g_render_generation;
    if (g_render_generation == 0u) ++g_render_generation;
    return g_render_generation;
}

static int render_valid_provider_id_(CastleStringView id) {
    CastleU32 index;
    if (!id.data || id.length == 0u || id.length >= RUNTIME_RENDER_PROVIDER_ID_CAP) {
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

static int render_pointer_in_module_(const void* pointer, HMODULE module) {
    MEMORY_BASIC_INFORMATION information;
    if (!pointer || !module ||
        VirtualQuery(pointer, &information, sizeof(information)) == 0u) return 0;
    return information.State == MEM_COMMIT && information.AllocationBase == module;
}

static int render_valid_provider_api_(CastlePluginHandle plugin,
                                      const CastleRenderProviderV1* api) {
    HMODULE module = Runtime_GetPluginModule(plugin);
    return module && api && api->magic == CASTLE_RENDER_PROVIDER_MAGIC &&
        api->struct_size >= CASTLE_SIZEOF_RENDER_PROVIDER_V1 &&
        api->api_version == CASTLE_RENDER_API_VERSION_1 &&
        api->capability_flags == 0u && render_pointer_in_module_(api, module) &&
        render_pointer_in_module_((const void*)api->GetProviderState, module) &&
        render_pointer_in_module_((const void*)api->RenderCurrentQueue, module) &&
        render_pointer_in_module_((const void*)api->PresentCurrentDisplay, module);
}

static int render_valid_call_(const CastleRenderCallV1* call) {
    if (!call || call->magic != CASTLE_RENDER_CALL_MAGIC ||
        call->struct_size < CASTLE_SIZEOF_RENDER_CALL_V1 ||
        call->version != CASTLE_RENDER_STRUCTURE_VERSION_1 ||
        call->render_context == 0u ||
        (call->flags & ~CASTLE_RENDER_CALL_EXTRA_WORLD_FRAME) != 0u) return 0;
    if ((call->flags & CASTLE_RENDER_CALL_EXTRA_WORLD_FRAME) != 0u) {
        return call->extra_frame_lease != 0u;
    }
    return call->extra_frame_lease == 0u;
}

void Runtime_RenderInitialize(void) {
    g_render_lock = 0;
    Runtime_ByteZero(&g_render_provider, (CastleU32)sizeof(g_render_provider));
    g_render_generation = 1u;
    g_original_render_queue = (RuntimeRenderOriginalFn)Runtime_GetOriginalRenderQueue();
    g_original_present = (RuntimeRenderOriginalFn)Runtime_GetOriginalDisplayPresent();
    g_extra_frame_lease = 0u;
    g_extra_frame_owner = 0u;
    g_extra_frame_display_generation = 0u;
    g_extra_frame_generation = 0u;
}

const CastleRenderApiV1* Runtime_GetRenderApiV1(void) {
    return &g_render_api;
}

CastleU32 Runtime_GetRenderProviderGeneration(void) {
    CastleU32 generation;
    Runtime_Lock(&g_render_lock);
    generation = g_render_generation;
    Runtime_Unlock(&g_render_lock);
    return generation;
}

static CastleResult render_validate_display_generation_(CastleU32 requested,
                                                        CastleU32* out_actual) {
    CastleResult result = Runtime_GetCurrentDisplayGeneration(out_actual);
    if (result < 0) return result;
    if (requested != 0u && requested != *out_actual) {
        return CASTLE_ERROR_STALE_GENERATION;
    }
    return CASTLE_OK;
}

static CastleResult render_copy_state_(CastleRenderStateV1* out_state,
                                       CastleU32 provider_only,
                                       CastleProviderHandle requested_provider) {
    CastleU32 display_provider_generation = 0u;
    CastleU32 geometry_generation = 0u;
    int linked_ready = 0;
    if (!out_state || out_state->magic != CASTLE_RENDER_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_RENDER_STATE_V1 ||
        out_state->version != CASTLE_RENDER_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_render_lock);
    if (provider_only && (!g_render_provider.used ||
        g_render_provider.handle != requested_provider)) {
        Runtime_Unlock(&g_render_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (g_render_provider.used && g_render_provider.state == CASTLE_RENDER_PROVIDER_READY) {
        linked_ready = Runtime_DisplayProviderReadyForPlugin(
            g_render_provider.display_provider, g_render_provider.plugin,
            &display_provider_generation, &geometry_generation);
    }
    out_state->flags = 0u;
    out_state->generation = g_render_generation;
    out_state->extra_frame_owner = g_extra_frame_owner;
    if (g_render_provider.used && (provider_only ||
        g_render_provider.state == CASTLE_RENDER_PROVIDER_READY)) {
        out_state->ready = g_render_provider.state == CASTLE_RENDER_PROVIDER_READY &&
                           linked_ready ? 1u : 0u;
        out_state->backend_plugin = g_render_provider.plugin;
        out_state->provider_handle = g_render_provider.handle;
        out_state->display_provider_generation =
            linked_ready ? display_provider_generation :
                           g_render_provider.display_provider_generation;
        Runtime_Unlock(&g_render_lock);
        return out_state->ready ? CASTLE_OK : CASTLE_ERROR_NOT_READY;
    }
    out_state->ready = g_original_render_queue && g_original_present ? 1u : 0u;
    out_state->backend_plugin = 0u;
    out_state->provider_handle = 0u;
    out_state->display_provider_generation = Runtime_GetDisplayProviderGeneration();
    Runtime_Unlock(&g_render_lock);
    return out_state->ready ? CASTLE_STATUS_DEFAULT_BACKEND : CASTLE_ERROR_NOT_READY;
}

static CastleResult CASTLE_RUNTIME_CALL render_get_state_(CastleRenderStateV1* out_state) {
    return render_copy_state_(out_state, 0u, 0u);
}

static CastleResult render_invoke_(const CastleRenderCallV1* call, int present) {
    const CastleRenderProviderV1* provider_api = NULL;
    CastleProviderHandle display_provider = 0u;
    CastlePluginHandle provider_plugin = 0u;
    RuntimeRenderOriginalFn original = NULL;
    CastleRenderCallV1 provider_call;
    CastleU32 actual_display_generation = 0u;
    CastleU32 linked_geometry_generation = 0u;
    CastleResult result;
    if (!render_valid_call_(call)) return CASTLE_ERROR_INVALID_ARGUMENT;
    result = render_validate_display_generation_(call->display_generation,
                                                 &actual_display_generation);
    if (result < 0) return result;

    Runtime_Lock(&g_render_lock);
    if ((call->flags & CASTLE_RENDER_CALL_EXTRA_WORLD_FRAME) != 0u) {
        if (g_extra_frame_lease == 0u || call->extra_frame_lease != g_extra_frame_lease ||
            g_extra_frame_display_generation != actual_display_generation) {
            Runtime_Unlock(&g_render_lock);
            return CASTLE_ERROR_RESOURCE_CONFLICT;
        }
    }
    if (g_render_provider.used &&
        g_render_provider.state == CASTLE_RENDER_PROVIDER_READY) {
        provider_api = g_render_provider.api;
        display_provider = g_render_provider.display_provider;
        provider_plugin = g_render_provider.plugin;
    } else {
        original = present ? g_original_present : g_original_render_queue;
    }
    Runtime_Unlock(&g_render_lock);

    if (provider_api) {
        if (!Runtime_DisplayProviderReadyForPlugin(display_provider, provider_plugin,
                NULL, &linked_geometry_generation)) return CASTLE_ERROR_NOT_READY;
        if (linked_geometry_generation != actual_display_generation) {
            return CASTLE_ERROR_STALE_GENERATION;
        }
        provider_call = *call;
        provider_call.display_generation = actual_display_generation;
        return present ? provider_api->PresentCurrentDisplay(&provider_call) :
                         provider_api->RenderCurrentQueue(&provider_call);
    }
    if (!original) return CASTLE_ERROR_NOT_READY;
    original((void*)(ULONG_PTR)call->render_context, NULL);
    return CASTLE_STATUS_DEFAULT_BACKEND;
}

static CastleResult CASTLE_RUNTIME_CALL render_current_queue_(
    const CastleRenderCallV1* call) {
    return render_invoke_(call, 0);
}

static CastleResult CASTLE_RUNTIME_CALL render_present_(const CastleRenderCallV1* call) {
    return render_invoke_(call, 1);
}

static CastleResult CASTLE_RUNTIME_CALL render_begin_extra_(CastlePluginHandle plugin,
    CastleU32 requested_display_generation, CastleLeaseHandle* out_lease,
    CastleU32* out_display_generation) {
    CastleU32 actual_generation = 0u;
    CastleResult result;
    if (!out_lease || !out_display_generation || !Runtime_GetPluginModule(plugin)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    result = render_validate_display_generation_(requested_display_generation,
                                                 &actual_generation);
    if (result < 0) return result;
    Runtime_Lock(&g_render_lock);
    if (g_extra_frame_lease != 0u) {
        Runtime_Unlock(&g_render_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    ++g_extra_frame_generation;
    if (g_extra_frame_generation == 0u) ++g_extra_frame_generation;
    g_extra_frame_lease = g_extra_frame_generation;
    g_extra_frame_owner = plugin;
    g_extra_frame_display_generation = actual_generation;
    *out_lease = g_extra_frame_lease;
    *out_display_generation = actual_generation;
    Runtime_Unlock(&g_render_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL render_end_extra_(CastleLeaseHandle lease) {
    if (lease == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_render_lock);
    if (g_extra_frame_lease == 0u || lease != g_extra_frame_lease) {
        Runtime_Unlock(&g_render_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    g_extra_frame_lease = 0u;
    g_extra_frame_owner = 0u;
    g_extra_frame_display_generation = 0u;
    Runtime_Unlock(&g_render_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL render_register_provider_(CastlePluginHandle plugin,
    CastleStringView provider_id, CastleProviderHandle display_provider,
    const CastleRenderProviderV1* provider_api, CastleProviderHandle* out_provider) {
    CastleU32 display_provider_generation = 0u;
    CastleU32 geometry_generation = 0u;
    CastleU32 index;
    if (!out_provider || display_provider == 0u ||
        !render_valid_provider_id_(provider_id) ||
        !render_valid_provider_api_(plugin, provider_api) ||
        !Runtime_DisplayProviderReadyForPlugin(display_provider, plugin,
            &display_provider_generation, &geometry_generation)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    (void)geometry_generation;
    Runtime_Lock(&g_render_lock);
    if (g_render_provider.used) {
        Runtime_Unlock(&g_render_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    Runtime_ByteZero(&g_render_provider, (CastleU32)sizeof(g_render_provider));
    g_render_provider.used = 1;
    g_render_provider.handle = 1u;
    g_render_provider.plugin = plugin;
    g_render_provider.display_provider = display_provider;
    g_render_provider.api = provider_api;
    g_render_provider.state = CASTLE_RENDER_PROVIDER_NOT_READY;
    g_render_provider.generation = render_next_generation_locked_();
    g_render_provider.display_provider_generation = display_provider_generation;
    for (index = 0u; index < provider_id.length; ++index) {
        g_render_provider.provider_id[index] = provider_id.data[index];
    }
    g_render_provider.provider_id[provider_id.length] = '\0';
    g_render_provider.provider_id_length = provider_id.length;
    *out_provider = g_render_provider.handle;
    Runtime_Unlock(&g_render_lock);
    Runtime_DiagnosticAppend("[Render] external provider registered.");
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL render_set_provider_ready_(
    CastleProviderHandle provider, CastleU32 ready) {
    const CastleRenderProviderV1* provider_api;
    CastlePluginHandle plugin;
    CastleProviderHandle display_provider;
    CastleRenderStateV1 provider_state;
    CastleU32 display_provider_generation = 0u;
    CastleResult result;
    if (ready > 1u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_render_lock);
    if (!g_render_provider.used || g_render_provider.handle != provider) {
        Runtime_Unlock(&g_render_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!ready) {
        g_render_provider.state = CASTLE_RENDER_PROVIDER_NOT_READY;
        render_next_generation_locked_();
        Runtime_Unlock(&g_render_lock);
        return CASTLE_OK;
    }
    provider_api = g_render_provider.api;
    plugin = g_render_provider.plugin;
    display_provider = g_render_provider.display_provider;
    Runtime_Unlock(&g_render_lock);

    if (!Runtime_DisplayProviderReadyForPlugin(display_provider, plugin,
            &display_provider_generation, NULL)) return CASTLE_ERROR_NOT_READY;
    Runtime_ByteZero(&provider_state, (CastleU32)sizeof(provider_state));
    provider_state.magic = CASTLE_RENDER_STATE_MAGIC;
    provider_state.struct_size = CASTLE_SIZEOF_RENDER_STATE_V1;
    provider_state.version = CASTLE_RENDER_STRUCTURE_VERSION_1;
    result = provider_api->GetProviderState(&provider_state);
    if (result < 0 || !provider_state.ready) return CASTLE_ERROR_NOT_READY;

    Runtime_Lock(&g_render_lock);
    if (!g_render_provider.used || g_render_provider.handle != provider ||
        g_render_provider.api != provider_api ||
        g_render_provider.display_provider != display_provider) {
        Runtime_Unlock(&g_render_lock);
        return CASTLE_ERROR_STALE_GENERATION;
    }
    g_render_provider.state = CASTLE_RENDER_PROVIDER_READY;
    g_render_provider.display_provider_generation = display_provider_generation;
    g_render_provider.generation = render_next_generation_locked_();
    Runtime_Unlock(&g_render_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL render_get_provider_state_(
    CastleProviderHandle provider, CastleRenderStateV1* out_state) {
    return render_copy_state_(out_state, 1u, provider);
}
