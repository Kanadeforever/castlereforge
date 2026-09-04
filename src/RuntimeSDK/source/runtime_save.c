#include "runtime_internal.h"

#define RUNTIME_SAVE_MAX_POLICIES 32u
#define RUNTIME_SAVE_MAX_SLOT 99u

#define SAVE_TITLE_UI_RVA       0x004E241Cu
#define SAVE_INTERFACE_UI_RVA   0x004DED0Cu
#define SAVE_POINT_UI_RVA       0x0049FCD0u
#define SAVE_ACTION_VTABLE_RVA  0x00060BA8u
#define SAVE_ACTION_ORIGINAL_RVA 0x000262C0u

#define SAVE_TITLE_SLOT_OFFSET     0x5B4u
#define SAVE_INTERFACE_SLOT_OFFSET 0x654u
#define SAVE_POINT_SLOT_OFFSET     0x580u
#define SAVE_SLOT_ROW_OFFSET       0x594u
#define SAVE_SLOT_PAGE_OFFSET      0x598u
#define SAVE_SLOT_ACTION_OFFSET    0x5A4u
#define SAVE_ACTION_SAVE_BUTTON    0x594u
#define SAVE_BUTTON_DISABLED       0x04u

typedef void (__fastcall *RuntimeSaveActionUpdateFn)(void* action, void* unused_edx);

typedef struct RuntimeSavePolicyRecord {
    int used;
    CastleProviderHandle handle;
    CastlePluginHandle plugin;
    CastleU32 first_slot;
    CastleU32 last_slot;
    CastleU32 allowed;
} RuntimeSavePolicyRecord;

static volatile LONG g_save_lock;
static RuntimeSavePolicyRecord g_save_policies[RUNTIME_SAVE_MAX_POLICIES];
static CastleProviderHandle g_save_next_policy;
static CastleSaveUiStateV1 g_save_ui_state;
static RuntimeSaveActionUpdateFn g_save_original_update;
static int g_save_hook_ready;

static CastleResult CASTLE_RUNTIME_CALL save_register_policy_(
    CastlePluginHandle plugin, const CastleManualSavePolicyV1* policy,
    CastleProviderHandle* out_policy);
static CastleResult CASTLE_RUNTIME_CALL save_unregister_policy_(
    CastleProviderHandle policy);
static CastleResult CASTLE_RUNTIME_CALL save_is_allowed_(
    CastleU32 slot, CastleU32* out_allowed);
static CastleResult CASTLE_RUNTIME_CALL save_get_ui_state_(
    CastleSaveUiStateV1* out_state);

static const CastleSaveApiV1 g_save_api = {
    CASTLE_SAVE_API_MAGIC,
    CASTLE_SIZEOF_SAVE_API_V1,
    CASTLE_SAVE_API_VERSION_1,
    CASTLE_SAVE_CAP_MANUAL_SLOT_POLICY | CASTLE_SAVE_CAP_UI_STATE,
    save_register_policy_,
    save_unregister_policy_,
    save_is_allowed_,
    save_get_ui_state_
};

static CastleU8* save_game_base_(void) {
    return (CastleU8*)(ULONG_PTR)Runtime_GetGameModuleValue();
}

static CastleProviderHandle save_next_policy_locked_(void) {
    CastleProviderHandle value = g_save_next_policy++;
    if (value == 0u) value = g_save_next_policy++;
    return value;
}

static int save_manual_allowed_locked_(CastleU32 slot) {
    CastleU32 index;
    int allowed = 1;
    for (index = 0u; index < RUNTIME_SAVE_MAX_POLICIES; ++index) {
        if (!g_save_policies[index].used || slot < g_save_policies[index].first_slot ||
            slot > g_save_policies[index].last_slot) continue;
        /* 多个策略重叠时禁止优先，任何插件都不能重新允许别人明确保护的槽。 */
        if (!g_save_policies[index].allowed) return 0;
        allowed = 1;
    }
    return allowed;
}

static CastleU8* save_read_pointer_(CastleU8* address) {
    CastleAddress value;
    if (!Runtime_MemoryRangeReadable(address, (CastleU32)sizeof(value))) return NULL;
    value = *(volatile CastleAddress*)address;
    if (!value || !Runtime_MemoryRangeReadable((const void*)(ULONG_PTR)value, 1u)) return NULL;
    return (CastleU8*)(ULONG_PTR)value;
}

static CastleU8* save_slot_from_owner_(CastleU32 owner_rva,
                                       CastleU32 slot_offset) {
    CastleU8* owner = save_read_pointer_(save_game_base_() + owner_rva);
    if (!owner) return NULL;
    return save_read_pointer_(owner + slot_offset);
}

static CastleU8* save_find_slot_for_action_(void* action) {
    static const CastleU32 owner_rvas[3] = {
        SAVE_INTERFACE_UI_RVA, SAVE_POINT_UI_RVA, SAVE_TITLE_UI_RVA
    };
    static const CastleU32 slot_offsets[3] = {
        SAVE_INTERFACE_SLOT_OFFSET, SAVE_POINT_SLOT_OFFSET, SAVE_TITLE_SLOT_OFFSET
    };
    CastleU32 index;
    for (index = 0u; index < 3u; ++index) {
        CastleU8* slot = save_slot_from_owner_(owner_rvas[index], slot_offsets[index]);
        CastleAddress current_action;
        if (!slot || !Runtime_MemoryRangeReadable(slot + SAVE_SLOT_ACTION_OFFSET, 4u)) continue;
        current_action = *(volatile CastleAddress*)(slot + SAVE_SLOT_ACTION_OFFSET);
        if (current_action == (CastleAddress)(ULONG_PTR)action) return slot;
    }
    return NULL;
}

static int save_selected_slot_(void* action, CastleU32* out_slot) {
    CastleU8* slot = save_find_slot_for_action_(action);
    CastleU32 row;
    CastleU32 page;
    if (!slot || !out_slot ||
        !Runtime_MemoryRangeReadable(slot + SAVE_SLOT_ROW_OFFSET, 4u) ||
        !Runtime_MemoryRangeReadable(slot + SAVE_SLOT_PAGE_OFFSET, 4u)) return 0;
    row = *(volatile CastleU32*)(slot + SAVE_SLOT_ROW_OFFSET);
    page = *(volatile CastleU32*)(slot + SAVE_SLOT_PAGE_OFFSET);
    if (row >= 4u || page > 96u || page + row > RUNTIME_SAVE_MAX_SLOT) return 0;
    *out_slot = page + row;
    return 1;
}

/*
 * 这是 Runtime 唯一的 SaveAction vtable 包装层。它只在原版 Update 调用的短窗口里设置
 * save Button 的 disabled 字节，返回前必定恢复。任何输入插件都只观察最终原生状态。
 */
static void __fastcall save_action_update_hook_(void* action, void* unused_edx) {
    CastleU32 selected_slot = 0u;
    CastleU32 allowed = 1u;
    CastleU8* save_button = NULL;
    CastleU8 old_disabled = 0u;
    int override_active = 0;

    if (save_selected_slot_(action, &selected_slot)) {
        Runtime_Lock(&g_save_lock);
        allowed = save_manual_allowed_locked_(selected_slot) ? 1u : 0u;
        g_save_ui_state.active = 1u;
        g_save_ui_state.selected_slot = selected_slot;
        g_save_ui_state.manual_save_allowed = allowed;
        ++g_save_ui_state.generation;
        Runtime_Unlock(&g_save_lock);

        if (!allowed) {
            save_button = save_read_pointer_((CastleU8*)action + SAVE_ACTION_SAVE_BUTTON);
            if (save_button && Runtime_MemoryRangeReadable(
                    save_button + SAVE_BUTTON_DISABLED, 1u)) {
                old_disabled = *(volatile CastleU8*)(save_button + SAVE_BUTTON_DISABLED);
                *(volatile CastleU8*)(save_button + SAVE_BUTTON_DISABLED) = 1u;
                override_active = 1;
            }
        }
    } else {
        Runtime_Lock(&g_save_lock);
        g_save_ui_state.active = 0u;
        g_save_ui_state.selected_slot = 0u;
        g_save_ui_state.manual_save_allowed = 1u;
        ++g_save_ui_state.generation;
        Runtime_Unlock(&g_save_lock);
    }

    if (g_save_original_update) g_save_original_update(action, unused_edx);
    if (override_active && Runtime_MemoryRangeReadable(
            save_button + SAVE_BUTTON_DISABLED, 1u)) {
        *(volatile CastleU8*)(save_button + SAVE_BUTTON_DISABLED) = old_disabled;
    }
}

static int save_install_hook_(void) {
    CastleU8* slot;
    CastleAddress current;
    CastleAddress replacement;
    if (!Runtime_GameProfileSupported()) return 0;
    slot = save_game_base_() + SAVE_ACTION_VTABLE_RVA;
    if (!Runtime_MemoryRangeReadable(slot, 4u)) return 0;
    current = *(volatile CastleAddress*)slot;
    if (current != (CastleAddress)(ULONG_PTR)(save_game_base_() +
                                               SAVE_ACTION_ORIGINAL_RVA)) {
        Runtime_DiagnosticAppend("[Save] SaveAction vtable is not original; central UI policy disabled.");
        return 0;
    }
    g_save_original_update = (RuntimeSaveActionUpdateFn)(ULONG_PTR)current;
    replacement = (CastleAddress)(ULONG_PTR)&save_action_update_hook_;
    if (Runtime_WriteMemory(slot, (const CastleU8*)&replacement, 4u, 0) < 0) {
        g_save_original_update = NULL;
        return 0;
    }
    Runtime_DiagnosticAppend("[Save] central SaveAction policy bridge installed.");
    return 1;
}

void Runtime_SaveInitialize(void) {
    g_save_lock = 0;
    Runtime_ByteZero(g_save_policies, (CastleU32)sizeof(g_save_policies));
    Runtime_ByteZero(&g_save_ui_state, (CastleU32)sizeof(g_save_ui_state));
    g_save_ui_state.magic = CASTLE_SAVE_UI_STATE_MAGIC;
    g_save_ui_state.struct_size = CASTLE_SIZEOF_SAVE_UI_STATE_V1;
    g_save_ui_state.version = CASTLE_SAVE_STRUCTURE_VERSION_1;
    g_save_ui_state.generation = 1u;
    g_save_ui_state.manual_save_allowed = 1u;
    g_save_next_policy = 1u;
    g_save_original_update = NULL;
    g_save_hook_ready = save_install_hook_();
}

const CastleSaveApiV1* Runtime_GetSaveApiV1(void) {
    return &g_save_api;
}

static CastleResult CASTLE_RUNTIME_CALL save_register_policy_(
    CastlePluginHandle plugin, const CastleManualSavePolicyV1* policy,
    CastleProviderHandle* out_policy) {
    CastleU32 index;
    if (!policy || !out_policy || policy->magic != CASTLE_SAVE_POLICY_MAGIC ||
        policy->struct_size < CASTLE_SIZEOF_MANUAL_SAVE_POLICY_V1 ||
        policy->version != CASTLE_SAVE_STRUCTURE_VERSION_1 ||
        policy->first_slot > policy->last_slot ||
        policy->last_slot > RUNTIME_SAVE_MAX_SLOT ||
        policy->manual_save_allowed > 1u || !Runtime_GetPluginModule(plugin)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!g_save_hook_ready) return CASTLE_ERROR_NOT_READY;
    *out_policy = 0u;
    Runtime_Lock(&g_save_lock);
    for (index = 0u; index < RUNTIME_SAVE_MAX_POLICIES; ++index) {
        if (!g_save_policies[index].used) break;
    }
    if (index == RUNTIME_SAVE_MAX_POLICIES) {
        Runtime_Unlock(&g_save_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_save_policies[index].used = 1;
    g_save_policies[index].handle = save_next_policy_locked_();
    g_save_policies[index].plugin = plugin;
    g_save_policies[index].first_slot = policy->first_slot;
    g_save_policies[index].last_slot = policy->last_slot;
    g_save_policies[index].allowed = policy->manual_save_allowed;
    *out_policy = g_save_policies[index].handle;
    Runtime_Unlock(&g_save_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL save_unregister_policy_(
    CastleProviderHandle policy) {
    CastleU32 index;
    if (policy == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_save_lock);
    for (index = 0u; index < RUNTIME_SAVE_MAX_POLICIES; ++index) {
        if (g_save_policies[index].used && g_save_policies[index].handle == policy) {
            Runtime_ByteZero(&g_save_policies[index],
                             (CastleU32)sizeof(g_save_policies[index]));
            Runtime_Unlock(&g_save_lock);
            return CASTLE_OK;
        }
    }
    Runtime_Unlock(&g_save_lock);
    return CASTLE_ERROR_INVALID_ARGUMENT;
}

static CastleResult CASTLE_RUNTIME_CALL save_is_allowed_(
    CastleU32 slot, CastleU32* out_allowed) {
    if (!out_allowed || slot > RUNTIME_SAVE_MAX_SLOT) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_save_lock);
    *out_allowed = save_manual_allowed_locked_(slot) ? 1u : 0u;
    Runtime_Unlock(&g_save_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL save_get_ui_state_(
    CastleSaveUiStateV1* out_state) {
    if (!out_state || out_state->magic != CASTLE_SAVE_UI_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_SAVE_UI_STATE_V1 ||
        out_state->version != CASTLE_SAVE_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_save_lock);
    *out_state = g_save_ui_state;
    Runtime_Unlock(&g_save_lock);
    return g_save_hook_ready ? CASTLE_OK : CASTLE_ERROR_NOT_READY;
}
