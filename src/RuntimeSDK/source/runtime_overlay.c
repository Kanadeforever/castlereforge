#include "runtime_internal.h"

#define RUNTIME_OVERLAY_MAX_CLIENTS 64u
#define RUNTIME_OVERLAY_ID_CAP RUNTIME_PLUGIN_ID_CAP
#define RUNTIME_OVERLAY_PRESENT_RVA 0x000064E0u

typedef void (__fastcall *RuntimeOverlayOriginalPresentFn)(void* render_context,
                                                            void* unused_edx);

typedef struct RuntimeOverlayClientRecord {
    int used;
    CastleLeaseHandle handle;
    CastlePluginHandle plugin;
    CastleOverlayDrawFn draw;
    void* user_context;
    CastleU32 phase;
    CastleU32 priority;
    CastleU32 ready;
    CastleU32 failure_count;
    char plugin_id[RUNTIME_OVERLAY_ID_CAP];
    CastleU32 plugin_id_length;
} RuntimeOverlayClientRecord;

typedef struct RuntimeOverlayWorkItem {
    CastleLeaseHandle handle;
    CastlePluginHandle plugin;
    CastleOverlayDrawFn draw;
    void* user_context;
    CastleU32 phase;
    CastleU32 priority;
    char plugin_id[RUNTIME_OVERLAY_ID_CAP];
    CastleU32 plugin_id_length;
} RuntimeOverlayWorkItem;

static volatile LONG g_overlay_lock;
static RuntimeOverlayClientRecord g_overlay_clients[RUNTIME_OVERLAY_MAX_CLIENTS];
static RuntimeOverlayWorkItem g_overlay_work[RUNTIME_OVERLAY_MAX_CLIENTS];
static CastleU32 g_overlay_next_handle;
static CastleU32 g_overlay_generation;
static CastleU32 g_overlay_frame_id;
static RuntimeOverlayOriginalPresentFn g_overlay_original_present;
static void* g_overlay_trampoline;
static int g_overlay_hook_ready;

static CastleResult CASTLE_RUNTIME_CALL overlay_register_(
    CastlePluginHandle plugin, const CastleOverlayClientV1* client,
    CastleLeaseHandle* out_client);
static CastleResult CASTLE_RUNTIME_CALL overlay_set_ready_(
    CastleLeaseHandle client, CastleU32 ready);
static CastleResult CASTLE_RUNTIME_CALL overlay_unregister_(CastleLeaseHandle client);
static CastleResult CASTLE_RUNTIME_CALL overlay_get_state_(CastleOverlayStateV1* out_state);

static const CastleOverlayApiV1 g_overlay_api = {
    CASTLE_OVERLAY_API_MAGIC,
    CASTLE_SIZEOF_OVERLAY_API_V1,
    CASTLE_OVERLAY_API_VERSION_1,
    CASTLE_OVERLAY_CAP_BEFORE_RENDERER_PRESENT,
    overlay_register_,
    overlay_set_ready_,
    overlay_unregister_,
    overlay_get_state_
};

static int overlay_copy_plugin_id_(CastlePluginHandle plugin, char* output,
                                   CastleU32* out_length) {
    CastleStringView id = Runtime_GetPluginIdView(plugin);
    CastleU32 index;
    if (!output || !out_length || !id.data || id.length == 0u ||
        id.length >= RUNTIME_OVERLAY_ID_CAP) return 0;
    for (index = 0u; index < id.length; ++index) output[index] = id.data[index];
    output[id.length] = '\0';
    *out_length = id.length;
    return 1;
}

static int overlay_text_before_(const RuntimeOverlayWorkItem* left,
                                const RuntimeOverlayWorkItem* right) {
    CastleU32 index;
    CastleU32 shared;
    if (left->phase != right->phase) return left->phase < right->phase;
    if (left->priority != right->priority) return left->priority < right->priority;
    shared = left->plugin_id_length < right->plugin_id_length ?
        left->plugin_id_length : right->plugin_id_length;
    for (index = 0u; index < shared; ++index) {
        if ((CastleU8)left->plugin_id[index] < (CastleU8)right->plugin_id[index]) return 1;
        if ((CastleU8)left->plugin_id[index] > (CastleU8)right->plugin_id[index]) return 0;
    }
    return left->plugin_id_length < right->plugin_id_length;
}

static void overlay_sort_(RuntimeOverlayWorkItem* work, CastleU32 count) {
    CastleU32 index;
    for (index = 1u; index < count; ++index) {
        RuntimeOverlayWorkItem moving = work[index];
        CastleU32 position = index;
        while (position > 0u && overlay_text_before_(&moving, &work[position - 1u])) {
            work[position] = work[position - 1u];
            --position;
        }
        work[position] = moving;
    }
}

static CastleU32 overlay_collect_work_(void) {
    CastleU32 source;
    CastleU32 count = 0u;
    Runtime_Lock(&g_overlay_lock);
    for (source = 0u; source < RUNTIME_OVERLAY_MAX_CLIENTS; ++source) {
        RuntimeOverlayClientRecord* record = &g_overlay_clients[source];
        RuntimeOverlayWorkItem* item;
        if (!record->used || !record->ready) continue;
        item = &g_overlay_work[count++];
        item->handle = record->handle;
        item->plugin = record->plugin;
        item->draw = record->draw;
        item->user_context = record->user_context;
        item->phase = record->phase;
        item->priority = record->priority;
        Runtime_ByteCopy(item->plugin_id, record->plugin_id,
                         record->plugin_id_length + 1u);
        item->plugin_id_length = record->plugin_id_length;
    }
    Runtime_Unlock(&g_overlay_lock);
    overlay_sort_(g_overlay_work, count);
    return count;
}

static void overlay_record_result_(CastleLeaseHandle handle, CastleResult result) {
    CastleU32 index;
    if (result >= 0) return;
    Runtime_Lock(&g_overlay_lock);
    for (index = 0u; index < RUNTIME_OVERLAY_MAX_CLIENTS; ++index) {
        RuntimeOverlayClientRecord* record = &g_overlay_clients[index];
        if (!record->used || record->handle != handle) continue;
        ++record->failure_count;
        if (record->failure_count >= 3u) {
            record->ready = 0u;
            ++g_overlay_generation;
        }
        break;
    }
    Runtime_Unlock(&g_overlay_lock);
}

static void overlay_dispatch_(void* render_context) {
    CastleOverlayContextV1 context;
    CastleU32 count;
    CastleU32 index;
    CastleU32 display_generation = 0u;
    (void)Runtime_GetCurrentDisplayGeneration(&display_generation);
    count = overlay_collect_work_();

    Runtime_ByteZero(&context, (CastleU32)sizeof(context));
    context.magic = CASTLE_OVERLAY_CONTEXT_MAGIC;
    context.struct_size = CASTLE_SIZEOF_OVERLAY_CONTEXT_V1;
    context.version = CASTLE_OVERLAY_STRUCTURE_VERSION_1;
    context.phase = CASTLE_OVERLAY_PHASE_BEFORE_PRESENT;
    context.render_context = (CastleAddress)(ULONG_PTR)render_context;
    context.display_generation = display_generation;
    context.frame_id = ++g_overlay_frame_id;

    for (index = 0u; index < count; ++index) {
        CastleResult result = g_overlay_work[index].draw(
            &context, g_overlay_work[index].user_context);
        overlay_record_result_(g_overlay_work[index].handle, result);
    }
}

static void __fastcall overlay_present_hook_(void* render_context, void* unused_edx) {
    (void)unused_edx;
    overlay_dispatch_(render_context);
    if (g_overlay_original_present) g_overlay_original_present(render_context, NULL);
}

static int overlay_install_hook_(void) {
    CastleU8* target;
    CastleU8* trampoline;
    CastleU8 patch[6];
    CastleS32 relative;
    static const CastleU8 expected[6] = {0x83u,0xECu,0x14u,0x56u,0x8Bu,0xF1u};
    if (!Runtime_GameProfileSupported()) return 0;
    target = (CastleU8*)(ULONG_PTR)Runtime_GetGameModuleValue() +
             RUNTIME_OVERLAY_PRESENT_RVA;
    if (!Runtime_MemoryRangeReadable(target, 6u) ||
        !Runtime_MemoryEquals(target, expected, 6u)) return 0;
    trampoline = (CastleU8*)VirtualAlloc(NULL, 32u, MEM_RESERVE | MEM_COMMIT,
                                         PAGE_EXECUTE_READWRITE);
    if (!trampoline) return 0;
    Runtime_ByteCopy(trampoline, expected, 6u);
    trampoline[6] = 0xE9u;
    relative = (CastleS32)((target + 6u) - (trampoline + 11u));
    Runtime_ByteCopy(trampoline + 7u, &relative, 4u);
    FlushInstructionCache(GetCurrentProcess(), trampoline, 11u);

    patch[0] = 0xE9u;
    relative = (CastleS32)((CastleU8*)(void*)&overlay_present_hook_ - (target + 5u));
    Runtime_ByteCopy(patch + 1u, &relative, 4u);
    patch[5] = 0x90u;
    if (Runtime_WriteMemory(target, patch, 6u, 1) < 0) {
        VirtualFree(trampoline, 0u, MEM_RELEASE);
        return 0;
    }
    g_overlay_trampoline = trampoline;
    g_overlay_original_present = (RuntimeOverlayOriginalPresentFn)trampoline;
    Runtime_DiagnosticAppend("[Overlay] central renderer Present bridge installed.");
    return 1;
}

void Runtime_OverlayInitialize(void) {
    g_overlay_lock = 0;
    Runtime_ByteZero(g_overlay_clients, (CastleU32)sizeof(g_overlay_clients));
    Runtime_ByteZero(g_overlay_work, (CastleU32)sizeof(g_overlay_work));
    g_overlay_next_handle = 1u;
    g_overlay_generation = 1u;
    g_overlay_frame_id = 0u;
    g_overlay_original_present = NULL;
    g_overlay_trampoline = NULL;
    g_overlay_hook_ready = overlay_install_hook_();
}

const CastleOverlayApiV1* Runtime_GetOverlayApiV1(void) {
    return &g_overlay_api;
}

static CastleResult CASTLE_RUNTIME_CALL overlay_register_(
    CastlePluginHandle plugin, const CastleOverlayClientV1* client,
    CastleLeaseHandle* out_client) {
    CastleU32 index;
    if (!client || !out_client || client->magic != CASTLE_OVERLAY_CLIENT_MAGIC ||
        client->struct_size < CASTLE_SIZEOF_OVERLAY_CLIENT_V1 ||
        client->version != CASTLE_OVERLAY_STRUCTURE_VERSION_1 || !client->draw ||
        client->phase != CASTLE_OVERLAY_PHASE_BEFORE_PRESENT ||
        client->priority > CASTLE_OVERLAY_PRIORITY_LATE ||
        !Runtime_GetPluginModule(plugin) || !g_overlay_hook_ready) {
        return g_overlay_hook_ready ? CASTLE_ERROR_INVALID_ARGUMENT : CASTLE_ERROR_NOT_READY;
    }
    *out_client = 0u;
    Runtime_Lock(&g_overlay_lock);
    for (index = 0u; index < RUNTIME_OVERLAY_MAX_CLIENTS; ++index) {
        if (!g_overlay_clients[index].used) break;
    }
    if (index == RUNTIME_OVERLAY_MAX_CLIENTS ||
        !overlay_copy_plugin_id_(plugin, g_overlay_clients[index].plugin_id,
                                 &g_overlay_clients[index].plugin_id_length)) {
        Runtime_Unlock(&g_overlay_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_overlay_clients[index].used = 1;
    g_overlay_clients[index].handle = g_overlay_next_handle++;
    if (g_overlay_clients[index].handle == 0u) {
        g_overlay_clients[index].handle = g_overlay_next_handle++;
    }
    g_overlay_clients[index].plugin = plugin;
    g_overlay_clients[index].draw = client->draw;
    g_overlay_clients[index].user_context = client->user_context;
    g_overlay_clients[index].phase = client->phase;
    g_overlay_clients[index].priority = client->priority;
    g_overlay_clients[index].ready = 0u;
    g_overlay_clients[index].failure_count = 0u;
    *out_client = g_overlay_clients[index].handle;
    ++g_overlay_generation;
    Runtime_Unlock(&g_overlay_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL overlay_set_ready_(
    CastleLeaseHandle client, CastleU32 ready) {
    CastleU32 index;
    if (client == 0u || ready > 1u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_overlay_lock);
    for (index = 0u; index < RUNTIME_OVERLAY_MAX_CLIENTS; ++index) {
        if (g_overlay_clients[index].used && g_overlay_clients[index].handle == client) {
            g_overlay_clients[index].ready = ready;
            g_overlay_clients[index].failure_count = 0u;
            ++g_overlay_generation;
            Runtime_Unlock(&g_overlay_lock);
            return CASTLE_OK;
        }
    }
    Runtime_Unlock(&g_overlay_lock);
    return CASTLE_ERROR_INVALID_ARGUMENT;
}

static CastleResult CASTLE_RUNTIME_CALL overlay_unregister_(CastleLeaseHandle client) {
    CastleU32 index;
    if (client == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_overlay_lock);
    for (index = 0u; index < RUNTIME_OVERLAY_MAX_CLIENTS; ++index) {
        if (g_overlay_clients[index].used && g_overlay_clients[index].handle == client) {
            Runtime_ByteZero(&g_overlay_clients[index],
                             (CastleU32)sizeof(g_overlay_clients[index]));
            ++g_overlay_generation;
            Runtime_Unlock(&g_overlay_lock);
            return CASTLE_OK;
        }
    }
    Runtime_Unlock(&g_overlay_lock);
    return CASTLE_ERROR_INVALID_ARGUMENT;
}

static CastleResult CASTLE_RUNTIME_CALL overlay_get_state_(CastleOverlayStateV1* out_state) {
    CastleU32 index;
    CastleU32 active = 0u;
    if (!out_state || out_state->magic != CASTLE_OVERLAY_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_OVERLAY_STATE_V1 ||
        out_state->version != CASTLE_OVERLAY_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_overlay_lock);
    for (index = 0u; index < RUNTIME_OVERLAY_MAX_CLIENTS; ++index) {
        if (g_overlay_clients[index].used && g_overlay_clients[index].ready) ++active;
    }
    out_state->flags = 0u;
    out_state->ready = g_overlay_hook_ready ? 1u : 0u;
    out_state->generation = g_overlay_generation;
    out_state->frame_id = g_overlay_frame_id;
    out_state->active_client_count = active;
    Runtime_Unlock(&g_overlay_lock);
    return g_overlay_hook_ready ? CASTLE_OK : CASTLE_ERROR_NOT_READY;
}
