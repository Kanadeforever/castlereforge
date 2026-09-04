#include "runtime_internal.h"

#define RUNTIME_INPUT_PROVIDER_ID_CAP 128u

typedef struct RuntimeInputProviderRecord {
    int used;
    CastleProviderHandle handle;
    CastlePluginHandle plugin;
    CastleInputProviderV1 api;
    CastleU32 ready;
    CastleU32 generation;
    char provider_id[RUNTIME_INPUT_PROVIDER_ID_CAP];
    CastleU32 provider_id_length;
} RuntimeInputProviderRecord;

typedef struct RuntimeInputFocusRecord {
    int active;
    CastleLeaseHandle handle;
    CastlePluginHandle plugin;
    CastleU32 focus_kind;
    CastleU32 priority;
    CastleU32 generation;
} RuntimeInputFocusRecord;

static volatile LONG g_input_lock;
static RuntimeInputProviderRecord g_input_provider;
static RuntimeInputFocusRecord g_input_focus;
static CastleU32 g_input_next_provider;
static CastleU32 g_input_next_lease;

static CastleResult CASTLE_RUNTIME_CALL input_get_snapshot_(
    CastleInputSnapshotV1* out_snapshot);
static CastleResult CASTLE_RUNTIME_CALL input_acquire_focus_(
    CastlePluginHandle plugin, const CastleInputFocusRequestV1* request,
    CastleLeaseHandle* out_lease);
static CastleResult CASTLE_RUNTIME_CALL input_release_focus_(CastleLeaseHandle lease);
static CastleResult CASTLE_RUNTIME_CALL input_get_focus_state_(
    CastleInputFocusStateV1* out_state);
static CastleResult CASTLE_RUNTIME_CALL input_register_provider_(
    CastlePluginHandle plugin, CastleStringView provider_id,
    const CastleInputProviderV1* provider_api,
    CastleProviderHandle* out_provider);
static CastleResult CASTLE_RUNTIME_CALL input_set_provider_ready_(
    CastleProviderHandle provider, CastleU32 ready);

static const CastleInputApiV1 g_input_api = {
    CASTLE_INPUT_API_MAGIC,
    CASTLE_SIZEOF_INPUT_API_V1,
    CASTLE_INPUT_API_VERSION_1,
    CASTLE_INPUT_CAP_PHYSICAL_SNAPSHOT |
        CASTLE_INPUT_CAP_SEMANTIC_SNAPSHOT |
        CASTLE_INPUT_CAP_FOCUS_LEASE |
        CASTLE_INPUT_CAP_EXTERNAL_PROVIDER,
    input_get_snapshot_,
    input_acquire_focus_,
    input_release_focus_,
    input_get_focus_state_,
    input_register_provider_,
    input_set_provider_ready_
};

static CastleU32 input_next_nonzero_(CastleU32* counter) {
    CastleU32 value;
    if (!counter) return 0u;
    value = (*counter)++;
    if (value == 0u) value = (*counter)++;
    return value;
}

static int input_copy_id_(char* output, CastleU32 capacity,
                          CastleStringView input, CastleU32* out_length) {
    CastleU32 index;
    if (!output || !out_length || !input.data || input.length == 0u ||
        input.length + 1u > capacity) return 0;
    for (index = 0u; index < input.length; ++index) {
        CastleU8 value = (CastleU8)input.data[index];
        if (value == 0u || value < 0x20u) return 0;
        output[index] = (char)value;
    }
    output[input.length] = '\0';
    *out_length = input.length;
    return 1;
}

void Runtime_InputInitialize(void) {
    g_input_lock = 0;
    Runtime_ByteZero(&g_input_provider, (CastleU32)sizeof(g_input_provider));
    Runtime_ByteZero(&g_input_focus, (CastleU32)sizeof(g_input_focus));
    g_input_next_provider = 1u;
    g_input_next_lease = 1u;
    g_input_focus.generation = 1u;
}

const CastleInputApiV1* Runtime_GetInputApiV1(void) {
    return &g_input_api;
}

static CastleResult CASTLE_RUNTIME_CALL input_get_snapshot_(
    CastleInputSnapshotV1* out_snapshot) {
    CastleInputCopySnapshotFn copy_snapshot = NULL;
    CastleU32 provider_ready = 0u;
    CastleResult result;
    if (!out_snapshot || out_snapshot->magic != CASTLE_INPUT_SNAPSHOT_MAGIC ||
        out_snapshot->struct_size < CASTLE_SIZEOF_INPUT_SNAPSHOT_V1 ||
        out_snapshot->version != CASTLE_INPUT_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    Runtime_Lock(&g_input_lock);
    if (g_input_provider.used) {
        copy_snapshot = g_input_provider.api.CopySnapshot;
        provider_ready = g_input_provider.ready;
    }
    Runtime_Unlock(&g_input_lock);
    if (!copy_snapshot || !provider_ready) return CASTLE_ERROR_NOT_READY;

    /* Provider 回调在锁外执行，避免 Controller 读取快照时反向查询 Runtime 造成死锁。 */
    result = copy_snapshot(out_snapshot);
    if (result < 0) return result;
    if (out_snapshot->magic != CASTLE_INPUT_SNAPSHOT_MAGIC ||
        out_snapshot->struct_size < CASTLE_SIZEOF_INPUT_SNAPSHOT_V1 ||
        out_snapshot->version != CASTLE_INPUT_STRUCTURE_VERSION_1 ||
        out_snapshot->control_mode > CASTLE_INPUT_CONTROL_INVESTIGATION &&
        out_snapshot->control_mode != CASTLE_INPUT_CONTROL_UNKNOWN) {
        return CASTLE_ERROR_ABI_MISMATCH;
    }
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL input_acquire_focus_(
    CastlePluginHandle plugin, const CastleInputFocusRequestV1* request,
    CastleLeaseHandle* out_lease) {
    if (!out_lease || !request || request->magic != CASTLE_INPUT_FOCUS_MAGIC ||
        request->struct_size < CASTLE_SIZEOF_INPUT_FOCUS_REQUEST_V1 ||
        request->version != CASTLE_INPUT_STRUCTURE_VERSION_1 ||
        request->focus_kind < CASTLE_INPUT_FOCUS_OVERLAY ||
        request->focus_kind > CASTLE_INPUT_FOCUS_POINTER ||
        request->priority > CASTLE_INPUT_PRIORITY_LATE ||
        !Runtime_GetPluginModule(plugin)) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_lease = 0u;

    Runtime_Lock(&g_input_lock);
    if (g_input_focus.active) {
        if (g_input_focus.plugin == plugin &&
            g_input_focus.focus_kind == request->focus_kind) {
            *out_lease = g_input_focus.handle;
            Runtime_Unlock(&g_input_lock);
            return CASTLE_STATUS_ALREADY_DONE;
        }
        Runtime_Unlock(&g_input_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_input_focus.active = 1;
    g_input_focus.handle = input_next_nonzero_(&g_input_next_lease);
    g_input_focus.plugin = plugin;
    g_input_focus.focus_kind = request->focus_kind;
    g_input_focus.priority = request->priority;
    ++g_input_focus.generation;
    *out_lease = g_input_focus.handle;
    Runtime_Unlock(&g_input_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL input_release_focus_(CastleLeaseHandle lease) {
    if (lease == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_input_lock);
    if (!g_input_focus.active || g_input_focus.handle != lease) {
        Runtime_Unlock(&g_input_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    g_input_focus.active = 0;
    g_input_focus.handle = 0u;
    g_input_focus.plugin = 0u;
    g_input_focus.focus_kind = 0u;
    g_input_focus.priority = 0u;
    ++g_input_focus.generation;
    Runtime_Unlock(&g_input_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL input_get_focus_state_(
    CastleInputFocusStateV1* out_state) {
    if (!out_state || out_state->magic != CASTLE_INPUT_FOCUS_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_INPUT_FOCUS_STATE_V1 ||
        out_state->version != CASTLE_INPUT_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_input_lock);
    out_state->flags = 0u;
    out_state->active = g_input_focus.active ? 1u : 0u;
    out_state->generation = g_input_focus.generation;
    out_state->owner_plugin = g_input_focus.plugin;
    out_state->focus_lease = g_input_focus.handle;
    out_state->focus_kind = g_input_focus.focus_kind;
    out_state->priority = g_input_focus.priority;
    Runtime_Unlock(&g_input_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL input_register_provider_(
    CastlePluginHandle plugin, CastleStringView provider_id,
    const CastleInputProviderV1* provider_api,
    CastleProviderHandle* out_provider) {
    if (!provider_api || !out_provider ||
        provider_api->magic != CASTLE_INPUT_PROVIDER_MAGIC ||
        provider_api->struct_size < CASTLE_SIZEOF_INPUT_PROVIDER_V1 ||
        provider_api->api_version != CASTLE_INPUT_API_VERSION_1 ||
        !provider_api->CopySnapshot || !Runtime_GetPluginModule(plugin)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    *out_provider = 0u;
    Runtime_Lock(&g_input_lock);
    if (g_input_provider.used) {
        Runtime_Unlock(&g_input_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    if (!input_copy_id_(g_input_provider.provider_id,
                        RUNTIME_INPUT_PROVIDER_ID_CAP, provider_id,
                        &g_input_provider.provider_id_length)) {
        Runtime_Unlock(&g_input_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    g_input_provider.used = 1;
    g_input_provider.handle = input_next_nonzero_(&g_input_next_provider);
    g_input_provider.plugin = plugin;
    g_input_provider.api = *provider_api;
    g_input_provider.ready = 0u;
    g_input_provider.generation = 1u;
    *out_provider = g_input_provider.handle;
    Runtime_Unlock(&g_input_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL input_set_provider_ready_(
    CastleProviderHandle provider, CastleU32 ready) {
    if (provider == 0u || ready > 1u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_input_lock);
    if (!g_input_provider.used || g_input_provider.handle != provider) {
        Runtime_Unlock(&g_input_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    g_input_provider.ready = ready;
    ++g_input_provider.generation;
    Runtime_Unlock(&g_input_lock);
    return CASTLE_OK;
}
