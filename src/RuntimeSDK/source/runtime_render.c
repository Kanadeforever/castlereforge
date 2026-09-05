#include "runtime_internal.h"

#define RUNTIME_RENDER_PROVIDER_ID_CAP 128u
#define RUNTIME_RENDER_QUEUE_CALL_RVA  0x0004A9C6u
#define RUNTIME_PRESENT_CALL_RVA       0x0004A9E6u

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
static CastleLeaseHandle g_extra_game_state_lease;
static int g_render_bridge_ready;

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

/*
 * 原版主循环的两个 CALL 从本版本起只指向 Runtime。Widescreen 是可选后端，FPSUnlock 是
 * 额外帧消费者；任何插件都不再读取 CALL 当前目标来猜加载顺序。
 */
static void __fastcall render_queue_bridge_(void* render_context, void* unused_edx) {
    CastleRenderCallV1 call;
    CastleU32 display_generation = 0u;
    CastleResult result;
    (void)unused_edx;
    Runtime_ByteZero(&call, (CastleU32)sizeof(call));
    call.magic = CASTLE_RENDER_CALL_MAGIC;
    call.struct_size = CASTLE_SIZEOF_RENDER_CALL_V1;
    call.version = CASTLE_RENDER_STRUCTURE_VERSION_1;
    call.render_context = (CastleAddress)(ULONG_PTR)render_context;
    (void)Runtime_GetCurrentDisplayGeneration(&display_generation);
    call.display_generation = display_generation;
    result = render_current_queue_(&call);
    if (result < 0 && g_original_render_queue) {
        g_original_render_queue(render_context, NULL);
    }
}

static void __fastcall render_present_bridge_(void* render_context, void* unused_edx) {
    CastleRenderCallV1 call;
    CastleU32 display_generation = 0u;
    CastleResult result;
    (void)unused_edx;
    Runtime_ByteZero(&call, (CastleU32)sizeof(call));
    call.magic = CASTLE_RENDER_CALL_MAGIC;
    call.struct_size = CASTLE_SIZEOF_RENDER_CALL_V1;
    call.version = CASTLE_RENDER_STRUCTURE_VERSION_1;
    call.render_context = (CastleAddress)(ULONG_PTR)render_context;
    (void)Runtime_GetCurrentDisplayGeneration(&display_generation);
    call.display_generation = display_generation;
    result = render_present_(&call);
    if (result < 0 && g_original_present) g_original_present(render_context, NULL);
}

static int render_read_call_target_(CastleU8* site, void** out_target) {
    CastleS32 relative;
    if (!site || !out_target || !Runtime_MemoryRangeReadable(site, 5u) ||
        site[0] != 0xE8u) return 0;
    Runtime_ByteCopy(&relative, site + 1u, 4u);
    *out_target = site + 5u + relative;
    return 1;
}

static int render_patch_call_(CastleU8* site, const void* expected,
                              const void* replacement) {
    CastleU8 patch[5];
    CastleS32 relative;
    void* actual = NULL;
    if (!render_read_call_target_(site, &actual) || actual != expected) return 0;
    patch[0] = 0xE8u;
    relative = (CastleS32)((const CastleU8*)replacement - (site + 5u));
    Runtime_ByteCopy(patch + 1u, &relative, 4u);
    return Runtime_WriteMemory(site, patch, 5u, 1) >= 0;
}

static int render_install_bridges_(void) {
    CastleU8* base;
    CastleU8* queue_site;
    CastleU8* present_site;
    CastleU8 original_queue_call[5];
    if (!Runtime_GameProfileSupported() || !g_original_render_queue ||
        !g_original_present) return 0;
    base = (CastleU8*)(ULONG_PTR)Runtime_GetGameModuleValue();
    queue_site = base + RUNTIME_RENDER_QUEUE_CALL_RVA;
    present_site = base + RUNTIME_PRESENT_CALL_RVA;
    if (!Runtime_MemoryRangeReadable(queue_site, 5u) ||
        !Runtime_MemoryRangeReadable(present_site, 5u)) return 0;
    Runtime_ByteCopy(original_queue_call, queue_site, 5u);
    if (!render_patch_call_(queue_site, (const void*)g_original_render_queue,
                            (const void*)&render_queue_bridge_)) return 0;
    if (!render_patch_call_(present_site, (const void*)g_original_present,
                            (const void*)&render_present_bridge_)) {
        /* 两个公共点必须同成同败；第二项失败时恢复第一项，不能留下半套渲染路径。 */
        (void)Runtime_WriteMemory(queue_site, original_queue_call, 5u, 1);
        return 0;
    }
    Runtime_DiagnosticAppend("[Render] central RenderQueue/Present bridges installed.");
    return 1;
}

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
    g_extra_game_state_lease = 0u;
    g_render_bridge_ready = render_install_bridges_();
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
    out_state->ready = g_render_bridge_ready && g_original_render_queue &&
                       g_original_present ? 1u : 0u;
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
    CastleLeaseHandle transient_game_lease = 0u;
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
        if (!present && (call->flags & CASTLE_RENDER_CALL_EXTRA_WORLD_FRAME) == 0u) {
            static const char label[] = "Runtime render provider frame";
            CastleGameMutationRequestV1 mutation;
            const CastleGameStateApiV1* game_state_api = Runtime_GetGameStateApiV1();
            Runtime_ByteZero(&mutation, (CastleU32)sizeof(mutation));
            mutation.magic = CASTLE_GAME_MUTATION_MAGIC;
            mutation.struct_size = CASTLE_SIZEOF_GAME_MUTATION_REQUEST_V1;
            mutation.version = CASTLE_GAME_STATE_STRUCTURE_VERSION_1;
            mutation.resource_mask = CASTLE_GAME_RESOURCE_CAMERA |
                                     CASTLE_GAME_RESOURCE_DRAW_QUEUE |
                                     CASTLE_GAME_RESOURCE_DIALOGUE;
            mutation.label.data = label;
            mutation.label.length = (CastleU32)(sizeof(label) - 1u);
            if (game_state_api->AcquireMutation(provider_plugin, &mutation,
                    &transient_game_lease) < 0) return CASTLE_ERROR_RESOURCE_CONFLICT;
        }
        if (!Runtime_DisplayProviderReadyForPlugin(display_provider, provider_plugin,
                NULL, &linked_geometry_generation)) {
            if (transient_game_lease) {
                Runtime_GetGameStateApiV1()->ReleaseMutation(transient_game_lease);
            }
            return CASTLE_ERROR_NOT_READY;
        }
        if (linked_geometry_generation != actual_display_generation) {
            if (transient_game_lease) {
                Runtime_GetGameStateApiV1()->ReleaseMutation(transient_game_lease);
            }
            return CASTLE_ERROR_STALE_GENERATION;
        }
        provider_call = *call;
        provider_call.display_generation = actual_display_generation;
        result = present ? provider_api->PresentCurrentDisplay(&provider_call) :
                           provider_api->RenderCurrentQueue(&provider_call);
        if (transient_game_lease) {
            Runtime_GetGameStateApiV1()->ReleaseMutation(transient_game_lease);
        }
        return result;
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
    CastleLeaseHandle game_state_lease = 0u;
    CastleGameMutationRequestV1 mutation;
    static const char label[] = "Runtime extra world frame";
    if (!out_lease || !out_display_generation || !Runtime_GetPluginModule(plugin)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    result = render_validate_display_generation_(requested_display_generation,
                                                 &actual_generation);
    if (result < 0) return result;
    Runtime_ByteZero(&mutation, (CastleU32)sizeof(mutation));
    mutation.magic = CASTLE_GAME_MUTATION_MAGIC;
    mutation.struct_size = CASTLE_SIZEOF_GAME_MUTATION_REQUEST_V1;
    mutation.version = CASTLE_GAME_STATE_STRUCTURE_VERSION_1;
    mutation.resource_mask = CASTLE_GAME_RESOURCE_CAMERA |
                             CASTLE_GAME_RESOURCE_DRAW_QUEUE |
                             CASTLE_GAME_RESOURCE_DIALOGUE |
                             CASTLE_GAME_RESOURCE_WORLD;
    mutation.label.data = label;
    mutation.label.length = (CastleU32)(sizeof(label) - 1u);
    result = Runtime_GetGameStateApiV1()->AcquireMutation(plugin, &mutation,
                                                          &game_state_lease);
    if (result < 0) return result;
    Runtime_Lock(&g_render_lock);
    if (g_extra_frame_lease != 0u) {
        Runtime_Unlock(&g_render_lock);
        Runtime_GetGameStateApiV1()->ReleaseMutation(game_state_lease);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    ++g_extra_frame_generation;
    if (g_extra_frame_generation == 0u) ++g_extra_frame_generation;
    g_extra_frame_lease = g_extra_frame_generation;
    g_extra_frame_owner = plugin;
    g_extra_frame_display_generation = actual_generation;
    g_extra_game_state_lease = game_state_lease;
    *out_lease = g_extra_frame_lease;
    *out_display_generation = actual_generation;
    Runtime_Unlock(&g_render_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL render_end_extra_(CastleLeaseHandle lease) {
    CastleLeaseHandle game_state_lease;
    if (lease == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_render_lock);
    if (g_extra_frame_lease == 0u || lease != g_extra_frame_lease) {
        Runtime_Unlock(&g_render_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    g_extra_frame_lease = 0u;
    g_extra_frame_owner = 0u;
    g_extra_frame_display_generation = 0u;
    game_state_lease = g_extra_game_state_lease;
    g_extra_game_state_lease = 0u;
    Runtime_Unlock(&g_render_lock);
    if (game_state_lease) {
        Runtime_GetGameStateApiV1()->ReleaseMutation(game_state_lease);
    }
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
