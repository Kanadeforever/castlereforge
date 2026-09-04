#include "runtime_internal.h"

#define RUNTIME_GAME_MUTATION_LEASES 64u
#define RUNTIME_GAME_RESOURCE_COUNT 5u

/* 当前确认版 RPG.exe 的公共状态 RVA。所有地址只在 Symbols profile 通过后使用。 */
#define GS_WORLD_RVA            0x00578508u
#define GS_CAMERA_X_RVA         0x00578514u
#define GS_CAMERA_Y_RVA         0x00578518u
#define GS_CAMERA_VIEW_W_RVA    0x0057851Cu
#define GS_CAMERA_VIEW_H_RVA    0x00578520u
#define GS_CAMERA_MIN_X_RVA     0x00578524u
#define GS_CAMERA_MIN_Y_RVA     0x00578528u
#define GS_CAMERA_MAX_X_RVA     0x0057852Cu
#define GS_CAMERA_MAX_Y_RVA     0x00578530u
#define GS_DIALOGUE_MODE_RVA    0x0006F640u
#define GS_DIALOGUE_ID_RVA      0x0006F670u
#define GS_DIALOGUE_TARGET_RVA  0x0006F678u
#define GS_DIALOGUE_CURRENT_RVA 0x0006F679u
#define GS_EVENT_YIELD_RVA      0x0049F81Du
#define GS_EVENT_BLOCK_RVA      0x0049F81Eu
#define GS_EVENT_TABLE_RVA      0x0049F820u
#define GS_ACTIVE_EVENT_RVA     0x0049F808u
#define GS_MAP_INPUT_GATE_RVA   0x00068BB9u
#define GS_MAP_KEY_MODE_RVA     0x00068BF0u
#define GS_BATTLE_UI_RVA        0x0049FD74u
#define GS_RESULT_UI_RVA        0x0049FCC8u
#define GS_TARGET_UI_RVA        0x004E2410u
#define GS_TITLE_UI_RVA         0x004E241Cu
#define GS_INTERFACE_UI_RVA     0x004DED0Cu
#define GS_SAVE_POINT_UI_RVA    0x0049FCD0u
#define GS_INN_UI_RVA           0x0049FCCCu
#define GS_SYNTHESIS_UI_RVA     0x0049FCBCu
#define GS_SHOP_UI_RVA          0x0049FCD4u
#define GS_MOVIE_OBJECT_RVA     0x0006F390u
#define GS_GAME_WINDOW_RVA      0x0006F384u
#define GS_DATA_CENTER_RVA      0x004E1C48u

typedef struct RuntimeGameMutationLease {
    int used;
    CastleLeaseHandle handle;
    CastlePluginHandle plugin;
    CastleU32 resources;
} RuntimeGameMutationLease;

static volatile LONG g_game_state_lock;
static volatile LONG g_game_snapshot_generation;
static RuntimeGameMutationLease g_game_leases[RUNTIME_GAME_MUTATION_LEASES];
static CastlePluginHandle g_game_resource_owner[RUNTIME_GAME_RESOURCE_COUNT];
static CastleLeaseHandle g_game_resource_lease[RUNTIME_GAME_RESOURCE_COUNT];
static CastleU32 g_game_resource_generation[RUNTIME_GAME_RESOURCE_COUNT];
static CastleU32 g_game_next_lease;

static CastleResult CASTLE_RUNTIME_CALL game_state_snapshot_(
    CastleGameStateSnapshotV1* out_snapshot);
static CastleResult CASTLE_RUNTIME_CALL game_state_acquire_(
    CastlePluginHandle plugin, const CastleGameMutationRequestV1* request,
    CastleLeaseHandle* out_lease);
static CastleResult CASTLE_RUNTIME_CALL game_state_release_(CastleLeaseHandle lease);
static CastleResult CASTLE_RUNTIME_CALL game_state_mutation_state_(
    CastleU32 resource, CastleGameMutationStateV1* out_state);

static const CastleGameStateApiV1 g_game_state_api = {
    CASTLE_GAME_STATE_API_MAGIC,
    CASTLE_SIZEOF_GAME_STATE_API_V1,
    CASTLE_GAME_STATE_API_VERSION_1,
    CASTLE_GAME_STATE_CAP_SNAPSHOT | CASTLE_GAME_STATE_CAP_MUTATION_LEASE,
    game_state_snapshot_,
    game_state_acquire_,
    game_state_release_,
    game_state_mutation_state_
};

static CastleU8* game_base_(void) {
    return (CastleU8*)(ULONG_PTR)Runtime_GetGameModuleValue();
}

static CastleU32 game_read_u32_(CastleU32 rva) {
    return *(volatile CastleU32*)(game_base_() + rva);
}

static CastleS32 game_read_s32_(CastleU32 rva) {
    return *(volatile CastleS32*)(game_base_() + rva);
}

static CastleU8 game_read_u8_(CastleU32 rva) {
    return *(volatile CastleU8*)(game_base_() + rva);
}

static CastleU32 game_resource_index_(CastleU32 resource) {
    CastleU32 index;
    for (index = 0u; index < RUNTIME_GAME_RESOURCE_COUNT; ++index) {
        if (resource == (1u << index)) return index;
    }
    return RUNTIME_GAME_RESOURCE_COUNT;
}

void Runtime_GameStateInitialize(void) {
    CastleU32 index;
    g_game_state_lock = 0;
    g_game_snapshot_generation = 0;
    Runtime_ByteZero(g_game_leases, (CastleU32)sizeof(g_game_leases));
    Runtime_ByteZero(g_game_resource_owner, (CastleU32)sizeof(g_game_resource_owner));
    Runtime_ByteZero(g_game_resource_lease, (CastleU32)sizeof(g_game_resource_lease));
    g_game_next_lease = 1u;
    for (index = 0u; index < RUNTIME_GAME_RESOURCE_COUNT; ++index) {
        g_game_resource_generation[index] = 1u;
    }
}

const CastleGameStateApiV1* Runtime_GetGameStateApiV1(void) {
    return &g_game_state_api;
}

static CastleResult CASTLE_RUNTIME_CALL game_state_snapshot_(
    CastleGameStateSnapshotV1* out_snapshot) {
    CastleU32 flags = 0u;
    CastleAddress movie_object;
    if (!out_snapshot || out_snapshot->magic != CASTLE_GAME_SNAPSHOT_MAGIC ||
        out_snapshot->struct_size < CASTLE_SIZEOF_GAME_STATE_SNAPSHOT_V1 ||
        out_snapshot->version != CASTLE_GAME_STATE_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!Runtime_GameProfileSupported()) return CASTLE_ERROR_NOT_READY;

    out_snapshot->generation = (CastleU32)InterlockedIncrement(&g_game_snapshot_generation);
    out_snapshot->world = game_read_u32_(GS_WORLD_RVA);
    out_snapshot->data_center = game_read_u32_(GS_DATA_CENTER_RVA);
    out_snapshot->event_table = game_read_u32_(GS_EVENT_TABLE_RVA);
    out_snapshot->game_window = game_read_u32_(GS_GAME_WINDOW_RVA);
    out_snapshot->camera_x = game_read_s32_(GS_CAMERA_X_RVA);
    out_snapshot->camera_y = game_read_s32_(GS_CAMERA_Y_RVA);
    out_snapshot->camera_view_width = game_read_s32_(GS_CAMERA_VIEW_W_RVA);
    out_snapshot->camera_view_height = game_read_s32_(GS_CAMERA_VIEW_H_RVA);
    out_snapshot->camera_min_x = game_read_s32_(GS_CAMERA_MIN_X_RVA);
    out_snapshot->camera_min_y = game_read_s32_(GS_CAMERA_MIN_Y_RVA);
    out_snapshot->camera_max_x = game_read_s32_(GS_CAMERA_MAX_X_RVA);
    out_snapshot->camera_max_y = game_read_s32_(GS_CAMERA_MAX_Y_RVA);
    out_snapshot->dialogue_mode = game_read_u32_(GS_DIALOGUE_MODE_RVA);
    out_snapshot->dialogue_id = game_read_u32_(GS_DIALOGUE_ID_RVA);
    out_snapshot->dialogue_target_state = game_read_u8_(GS_DIALOGUE_TARGET_RVA);
    out_snapshot->dialogue_current_state = game_read_u8_(GS_DIALOGUE_CURRENT_RVA);
    out_snapshot->event_yield = game_read_u8_(GS_EVENT_YIELD_RVA);
    out_snapshot->event_blocked = game_read_u8_(GS_EVENT_BLOCK_RVA);
    out_snapshot->active_event_id = game_read_u32_(GS_ACTIVE_EVENT_RVA);
    out_snapshot->map_input_gate = game_read_u8_(GS_MAP_INPUT_GATE_RVA);
    out_snapshot->map_key_mode = game_read_u32_(GS_MAP_KEY_MODE_RVA);
    out_snapshot->battle_ui = game_read_u32_(GS_BATTLE_UI_RVA);
    out_snapshot->result_ui = game_read_u32_(GS_RESULT_UI_RVA);
    out_snapshot->target_selector_ui = game_read_u32_(GS_TARGET_UI_RVA);
    out_snapshot->title_ui = game_read_u32_(GS_TITLE_UI_RVA);
    out_snapshot->interface_ui = game_read_u32_(GS_INTERFACE_UI_RVA);
    out_snapshot->save_point_ui = game_read_u32_(GS_SAVE_POINT_UI_RVA);
    out_snapshot->inn_ui = game_read_u32_(GS_INN_UI_RVA);
    out_snapshot->synthesis_ui = game_read_u32_(GS_SYNTHESIS_UI_RVA);
    out_snapshot->shop_ui = game_read_u32_(GS_SHOP_UI_RVA);
    movie_object = game_read_u32_(GS_MOVIE_OBJECT_RVA);
    out_snapshot->movie_object = movie_object;
    out_snapshot->movie_active = 0u;
    if (movie_object != 0u && Runtime_MemoryRangeReadable(
            (const void*)(ULONG_PTR)(movie_object + 0x0Au), 1u)) {
        out_snapshot->movie_active = *(volatile CastleU8*)(ULONG_PTR)(movie_object + 0x0Au);
    }

    if (out_snapshot->world) flags |= CASTLE_GAME_FLAG_WORLD_READY;
    if (out_snapshot->event_table) flags |= CASTLE_GAME_FLAG_EVENT_TABLE_READY;
    if (out_snapshot->dialogue_id || out_snapshot->dialogue_target_state ||
        out_snapshot->dialogue_current_state) flags |= CASTLE_GAME_FLAG_DIALOGUE_ACTIVE;
    if (out_snapshot->movie_active) flags |= CASTLE_GAME_FLAG_MOVIE_ACTIVE;
    if (out_snapshot->battle_ui || out_snapshot->result_ui ||
        out_snapshot->target_selector_ui) flags |= CASTLE_GAME_FLAG_BATTLE_ACTIVE;
    if (out_snapshot->title_ui || out_snapshot->interface_ui || out_snapshot->save_point_ui ||
        out_snapshot->inn_ui || out_snapshot->synthesis_ui || out_snapshot->shop_ui) {
        flags |= CASTLE_GAME_FLAG_MENU_ACTIVE;
    }
    if ((flags & (CASTLE_GAME_FLAG_WORLD_READY | CASTLE_GAME_FLAG_EVENT_TABLE_READY)) ==
            (CASTLE_GAME_FLAG_WORLD_READY | CASTLE_GAME_FLAG_EVENT_TABLE_READY) &&
        (flags & (CASTLE_GAME_FLAG_DIALOGUE_ACTIVE | CASTLE_GAME_FLAG_MOVIE_ACTIVE |
                  CASTLE_GAME_FLAG_BATTLE_ACTIVE | CASTLE_GAME_FLAG_MENU_ACTIVE)) == 0u &&
        out_snapshot->active_event_id == 0u && out_snapshot->event_yield == 0u &&
        out_snapshot->event_blocked == 0u) {
        flags |= CASTLE_GAME_FLAG_FREE_ROAM_CANDIDATE;
    }
    out_snapshot->flags = flags;
    return CASTLE_OK;
}

static CastleLeaseHandle game_next_lease_locked_(void) {
    CastleLeaseHandle value = g_game_next_lease++;
    if (value == 0u) value = g_game_next_lease++;
    return value;
}

static CastleResult CASTLE_RUNTIME_CALL game_state_acquire_(
    CastlePluginHandle plugin, const CastleGameMutationRequestV1* request,
    CastleLeaseHandle* out_lease) {
    CastleU32 resource_index;
    CastleU32 lease_index;
    CastleLeaseHandle handle;
    if (!request || !out_lease || request->magic != CASTLE_GAME_MUTATION_MAGIC ||
        request->struct_size < CASTLE_SIZEOF_GAME_MUTATION_REQUEST_V1 ||
        request->version != CASTLE_GAME_STATE_STRUCTURE_VERSION_1 ||
        request->resource_mask == 0u ||
        (request->resource_mask & ~CASTLE_GAME_RESOURCE_ALL) != 0u ||
        !Runtime_GetPluginModule(plugin)) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_lease = 0u;

    Runtime_Lock(&g_game_state_lock);
    for (resource_index = 0u; resource_index < RUNTIME_GAME_RESOURCE_COUNT;
         ++resource_index) {
        if ((request->resource_mask & (1u << resource_index)) != 0u &&
            g_game_resource_owner[resource_index] != 0u) {
            Runtime_Unlock(&g_game_state_lock);
            return CASTLE_ERROR_RESOURCE_CONFLICT;
        }
    }
    for (lease_index = 0u; lease_index < RUNTIME_GAME_MUTATION_LEASES; ++lease_index) {
        if (!g_game_leases[lease_index].used) break;
    }
    if (lease_index == RUNTIME_GAME_MUTATION_LEASES) {
        Runtime_Unlock(&g_game_state_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }

    handle = game_next_lease_locked_();
    g_game_leases[lease_index].used = 1;
    g_game_leases[lease_index].handle = handle;
    g_game_leases[lease_index].plugin = plugin;
    g_game_leases[lease_index].resources = request->resource_mask;
    for (resource_index = 0u; resource_index < RUNTIME_GAME_RESOURCE_COUNT;
         ++resource_index) {
        if ((request->resource_mask & (1u << resource_index)) != 0u) {
            g_game_resource_owner[resource_index] = plugin;
            g_game_resource_lease[resource_index] = handle;
            ++g_game_resource_generation[resource_index];
        }
    }
    *out_lease = handle;
    Runtime_Unlock(&g_game_state_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL game_state_release_(CastleLeaseHandle lease) {
    CastleU32 lease_index;
    CastleU32 resource_index;
    if (lease == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_game_state_lock);
    for (lease_index = 0u; lease_index < RUNTIME_GAME_MUTATION_LEASES; ++lease_index) {
        if (g_game_leases[lease_index].used &&
            g_game_leases[lease_index].handle == lease) break;
    }
    if (lease_index == RUNTIME_GAME_MUTATION_LEASES) {
        Runtime_Unlock(&g_game_state_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    for (resource_index = 0u; resource_index < RUNTIME_GAME_RESOURCE_COUNT;
         ++resource_index) {
        if ((g_game_leases[lease_index].resources & (1u << resource_index)) != 0u &&
            g_game_resource_lease[resource_index] == lease) {
            g_game_resource_owner[resource_index] = 0u;
            g_game_resource_lease[resource_index] = 0u;
            ++g_game_resource_generation[resource_index];
        }
    }
    Runtime_ByteZero(&g_game_leases[lease_index],
                     (CastleU32)sizeof(g_game_leases[lease_index]));
    Runtime_Unlock(&g_game_state_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL game_state_mutation_state_(
    CastleU32 resource, CastleGameMutationStateV1* out_state) {
    CastleU32 index = game_resource_index_(resource);
    if (index >= RUNTIME_GAME_RESOURCE_COUNT || !out_state ||
        out_state->magic != CASTLE_GAME_MUTATION_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_GAME_MUTATION_STATE_V1 ||
        out_state->version != CASTLE_GAME_STATE_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_game_state_lock);
    out_state->flags = 0u;
    out_state->resource = resource;
    out_state->active = g_game_resource_owner[index] ? 1u : 0u;
    out_state->generation = g_game_resource_generation[index];
    out_state->owner_plugin = g_game_resource_owner[index];
    out_state->lease = g_game_resource_lease[index];
    Runtime_Unlock(&g_game_state_lock);
    return CASTLE_OK;
}
