#include "runtime_internal.h"

#define HOOK_MAX_TRANSACTIONS 64u
#define HOOK_MAX_CLAIMS       512u
#define HOOK_MAX_CLAIMS_PER_TRANSACTION 256u
#define HOOK_MAX_PATCH_BYTES   64u
#define HOOK_MAX_TEXT          128u
#define HOOK_MAX_SIGNATURE      96u
#define HOOK_MAX_CHAINS        256u
#define HOOK_MAX_CHAIN_NODES    64u

#define HOOK_CLAIM_EXCLUSIVE 1u
#define HOOK_CLAIM_STATE     2u
#define HOOK_CLAIM_CHAIN     3u
#define HOOK_CLAIM_NAMED     4u

typedef struct RuntimeHookTransaction {
    int used;
    CastleTransactionHandle handle;
    CastlePluginHandle plugin;
    CastleU32 state;
    CastleU32 flags;
    char label[HOOK_MAX_TEXT];
    CastleU32 label_length;
    CastleClaimHandle claims[HOOK_MAX_CLAIMS_PER_TRANSACTION];
    CastleU32 claim_count;
} RuntimeHookTransaction;

typedef struct RuntimeHookClaim {
    int used;
    int active;
    CastleClaimHandle handle;
    CastleTransactionHandle transaction;
    CastlePluginHandle plugin;
    CastleU32 type;
    CastleU32 flags;
    CastleResult result;
    CastleU32 resource_generation;
    CastleTargetAddressV1 target;
    CastleU8 expected[HOOK_MAX_PATCH_BYTES];
    CastleU8 replacement[HOOK_MAX_PATCH_BYTES];
    CastleU8 alternate[HOOK_MAX_PATCH_BYTES];
    CastleU8 saved[HOOK_MAX_PATCH_BYTES];
    CastleU32 byte_size;
    CastleU32 desired_state;
    CastleU32 hook_kind;
    CastleAddress expected_original_target;
    CastleAddress replacement_hook;
    char signature[HOOK_MAX_SIGNATURE];
    CastleU32 signature_length;
    CastleU32 phase;
    CastleU32 priority;
    char resource_id[HOOK_MAX_TEXT];
    CastleU32 resource_id_length;
    CastleU32 resource_kind;
    char label[HOOK_MAX_TEXT];
    CastleU32 label_length;
    CastleU32 order_relation;
    char order_plugin_id[RUNTIME_PLUGIN_ID_CAP];
    CastleU32 order_plugin_id_length;
    void* next_target;
    CastleS32 chain_index;
} RuntimeHookClaim;

typedef struct RuntimeHookChain {
    int used;
    CastleU32 hook_kind;
    CastleTargetAddressV1 target;
    void* site;
    CastleAddress original_target;
    char signature[HOOK_MAX_SIGNATURE];
    CastleU32 signature_length;
    CastleClaimHandle nodes[HOOK_MAX_CHAIN_NODES];
    CastleU32 node_count;
    CastleU32 generation;
} RuntimeHookChain;

static volatile LONG g_hook_lock;
static RuntimeHookTransaction g_transactions[HOOK_MAX_TRANSACTIONS];
static RuntimeHookClaim g_claims[HOOK_MAX_CLAIMS];
static RuntimeHookChain g_chains[HOOK_MAX_CHAINS];
static CastleU32 g_resource_generation;

static CastleResult CASTLE_RUNTIME_CALL hook_begin_transaction_(
    CastlePluginHandle plugin, CastleStringView label, CastleU32 flags,
    CastleTransactionHandle* out_transaction);
static CastleResult CASTLE_RUNTIME_CALL hook_add_exclusive_(
    CastleTransactionHandle transaction, const CastleExclusivePatchClaimV1* claim,
    CastleClaimHandle* out_claim);
static CastleResult CASTLE_RUNTIME_CALL hook_add_state_(
    CastleTransactionHandle transaction, const CastleStatePatchClaimV1* claim,
    CastleClaimHandle* out_claim);
static CastleResult CASTLE_RUNTIME_CALL hook_add_relative_(
    CastleTransactionHandle transaction, const CastleChainHookClaimV1* claim,
    CastleClaimHandle* out_claim);
static CastleResult CASTLE_RUNTIME_CALL hook_add_pointer_(
    CastleTransactionHandle transaction, const CastleChainHookClaimV1* claim,
    CastleClaimHandle* out_claim);
static CastleResult CASTLE_RUNTIME_CALL hook_add_named_(
    CastleTransactionHandle transaction, const CastleNamedResourceClaimV1* claim,
    CastleClaimHandle* out_claim);
static CastleResult CASTLE_RUNTIME_CALL hook_add_order_(
    CastleTransactionHandle transaction, const CastleOrderConstraintV1* constraint_value);
static CastleResult CASTLE_RUNTIME_CALL hook_preflight_(CastleTransactionHandle transaction);
static CastleResult CASTLE_RUNTIME_CALL hook_commit_(CastleTransactionHandle transaction);
static CastleResult CASTLE_RUNTIME_CALL hook_abort_(CastleTransactionHandle transaction);
static CastleResult CASTLE_RUNTIME_CALL hook_get_claim_(CastleClaimHandle claim,
                                                        CastleClaimResultV1* out_result);
static CastleResult CASTLE_RUNTIME_CALL hook_get_binding_(CastleClaimHandle claim,
                                                          CastleHookBindingV1* out_binding);
static CastleResult CASTLE_RUNTIME_CALL hook_describe_(CastleClaimHandle claim,
                                                       CastleDiagnosticBufferV1* output);

static const CastleHookApiV1 g_hook_api = {
    CASTLE_HOOK_API_MAGIC,
    CASTLE_SIZEOF_HOOK_API_V1,
    CASTLE_HOOK_API_VERSION_1,
    0u,
    hook_begin_transaction_,
    hook_add_exclusive_,
    hook_add_state_,
    hook_add_relative_,
    hook_add_pointer_,
    hook_add_named_,
    hook_add_order_,
    hook_preflight_,
    hook_commit_,
    hook_abort_,
    hook_get_claim_,
    hook_get_binding_,
    hook_describe_
};

static int hook_copy_view_(char* output, CastleU32 capacity, CastleU32* output_length,
                           CastleStringView input, int allow_empty) {
    CastleU32 index;
    if (!output || !output_length || capacity == 0u) return 0;
    if (!input.data) {
        if (!allow_empty || input.length != 0u) return 0;
        output[0] = '\0';
        *output_length = 0u;
        return 1;
    }
    if ((!allow_empty && input.length == 0u) || input.length >= capacity) return 0;
    for (index = 0u; index < input.length; ++index) output[index] = input.data[index];
    output[input.length] = '\0';
    *output_length = input.length;
    return 1;
}

static RuntimeHookTransaction* hook_transaction_(CastleTransactionHandle handle) {
    RuntimeHookTransaction* transaction;
    if (handle == 0u || handle > HOOK_MAX_TRANSACTIONS) return NULL;
    transaction = &g_transactions[handle - 1u];
    return transaction->used && transaction->handle == handle ? transaction : NULL;
}

static RuntimeHookClaim* hook_claim_(CastleClaimHandle handle) {
    RuntimeHookClaim* claim;
    if (handle == 0u || handle > HOOK_MAX_CLAIMS) return NULL;
    claim = &g_claims[handle - 1u];
    return claim->used && claim->handle == handle ? claim : NULL;
}

static RuntimeHookClaim* hook_allocate_claim_(RuntimeHookTransaction* transaction,
                                              CastleU32 type,
                                              CastleClaimHandle* out_handle) {
    CastleU32 index;
    RuntimeHookClaim* claim = NULL;
    if (!transaction || !out_handle ||
        transaction->claim_count >= HOOK_MAX_CLAIMS_PER_TRANSACTION) return NULL;

    for (index = 0u; index < HOOK_MAX_CLAIMS; ++index) {
        if (!g_claims[index].used) {
            claim = &g_claims[index];
            Runtime_ByteZero(claim, (CastleU32)sizeof(*claim));
            claim->used = 1;
            claim->handle = index + 1u;
            claim->transaction = transaction->handle;
            claim->plugin = transaction->plugin;
            claim->type = type;
            claim->result = CASTLE_ERROR_NOT_READY;
            claim->chain_index = -1;
            transaction->claims[transaction->claim_count++] = claim->handle;
            *out_handle = claim->handle;
            break;
        }
    }
    return claim;
}

static int hook_flags_valid_(CastleU32 flags) {
    CastleU32 memory_flags = flags & (CASTLE_PATCH_FLAG_CODE | CASTLE_PATCH_FLAG_DATA);
    return memory_flags == CASTLE_PATCH_FLAG_CODE || memory_flags == CASTLE_PATCH_FLAG_DATA;
}

static int hook_range_overlap_(const CastleTargetAddressV1* left,
                               const CastleTargetAddressV1* right) {
    CastleU32 left_end;
    CastleU32 right_end;
    if (!left || !right || left->module != right->module) return 0;
    left_end = left->rva + left->size;
    right_end = right->rva + right->size;
    return left->rva < right_end && right->rva < left_end;
}

static int hook_claims_compatible_(RuntimeHookClaim* left, RuntimeHookClaim* right) {
    if (!left || !right) return 0;
    if (left->type == HOOK_CLAIM_NAMED || right->type == HOOK_CLAIM_NAMED) {
        if (left->type != HOOK_CLAIM_NAMED || right->type != HOOK_CLAIM_NAMED) return 1;
        return !Runtime_StringEquals(left->resource_id, left->resource_id_length,
                                     right->resource_id, right->resource_id_length);
    }
    if (!hook_range_overlap_(&left->target, &right->target)) return 1;
    return left->type == HOOK_CLAIM_CHAIN && right->type == HOOK_CLAIM_CHAIN &&
        left->target.rva == right->target.rva &&
        left->target.size == right->target.size &&
        left->hook_kind == right->hook_kind &&
        left->expected_original_target == right->expected_original_target &&
        Runtime_StringEquals(left->signature, left->signature_length,
                             right->signature, right->signature_length);
}

static RuntimeHookChain* hook_find_chain_(const RuntimeHookClaim* claim) {
    CastleU32 index;
    for (index = 0u; index < HOOK_MAX_CHAINS; ++index) {
        RuntimeHookChain* chain = &g_chains[index];
        if (chain->used && chain->hook_kind == claim->hook_kind &&
            chain->target.module == claim->target.module &&
            chain->target.rva == claim->target.rva &&
            chain->target.size == claim->target.size) return chain;
    }
    return NULL;
}

static CastleAddress hook_current_target_(RuntimeHookClaim* claim, void* site) {
    if (claim->hook_kind == CASTLE_HOOK_REL32_CALL) {
        volatile CastleU8* bytes = (volatile CastleU8*)site;
        CastleS32 relative;
        if (bytes[0] != 0xE8u) return 0u;
        relative = (CastleS32)((CastleU32)bytes[1] |
                              ((CastleU32)bytes[2] << 8) |
                              ((CastleU32)bytes[3] << 16) |
                              ((CastleU32)bytes[4] << 24));
        return (CastleAddress)(ULONG_PTR)((BYTE*)site + 5u + relative);
    }
    return *(volatile CastleAddress*)site;
}

static CastleResult hook_write_chain_target_(RuntimeHookChain* chain,
                                             CastleAddress target) {
    if (chain->hook_kind == CASTLE_HOOK_REL32_CALL) {
        CastleU8 bytes[5];
        CastleS32 relative = (CastleS32)(target -
            ((CastleAddress)(ULONG_PTR)chain->site + 5u));
        bytes[0] = 0xE8u;
        bytes[1] = (CastleU8)((CastleU32)relative & 0xFFu);
        bytes[2] = (CastleU8)(((CastleU32)relative >> 8) & 0xFFu);
        bytes[3] = (CastleU8)(((CastleU32)relative >> 16) & 0xFFu);
        bytes[4] = (CastleU8)(((CastleU32)relative >> 24) & 0xFFu);
        return Runtime_WriteMemory(chain->site, bytes, 5u, 1);
    }
    return Runtime_WriteMemory(chain->site, (const CastleU8*)&target, 4u, 0);
}

static int hook_default_before_(RuntimeHookClaim* left, RuntimeHookClaim* right) {
    CastleStringView left_id;
    CastleStringView right_id;
    CastleU32 index;
    CastleU32 common;

    if (left->phase != right->phase) return left->phase < right->phase;
    if (left->priority != right->priority) return left->priority < right->priority;
    left_id = Runtime_GetPluginIdView(left->plugin);
    right_id = Runtime_GetPluginIdView(right->plugin);
    common = left_id.length < right_id.length ? left_id.length : right_id.length;
    for (index = 0u; index < common; ++index) {
        if ((CastleU8)left_id.data[index] < (CastleU8)right_id.data[index]) return 1;
        if ((CastleU8)left_id.data[index] > (CastleU8)right_id.data[index]) return 0;
    }
    return left_id.length < right_id.length;
}

static int hook_constraint_edge_(RuntimeHookClaim* from, RuntimeHookClaim* to) {
    CastleStringView to_id;
    CastleStringView from_id;
    if (from->order_relation == 0u) return 0;
    to_id = Runtime_GetPluginIdView(to->plugin);
    from_id = Runtime_GetPluginIdView(from->plugin);

    if (from->order_relation == CASTLE_ORDER_OTHER_BEFORE &&
        Runtime_StringEquals(from->order_plugin_id, from->order_plugin_id_length,
                             to_id.data, to_id.length)) return 0;
    if (from->order_relation == CASTLE_ORDER_OTHER_AFTER &&
        Runtime_StringEquals(from->order_plugin_id, from->order_plugin_id_length,
                             to_id.data, to_id.length)) return 1;

    if (to->order_relation == CASTLE_ORDER_OTHER_BEFORE &&
        Runtime_StringEquals(to->order_plugin_id, to->order_plugin_id_length,
                             from_id.data, from_id.length)) return 1;
    if (to->order_relation == CASTLE_ORDER_OTHER_AFTER &&
        Runtime_StringEquals(to->order_plugin_id, to->order_plugin_id_length,
                             from_id.data, from_id.length)) return 0;
    return 0;
}

static int hook_sort_chain_(RuntimeHookChain* chain) {
    volatile CastleU32 output[HOOK_MAX_CHAIN_NODES];
    volatile CastleU32 indegree[HOOK_MAX_CHAIN_NODES];
    volatile CastleU8 selected[HOOK_MAX_CHAIN_NODES];
    CastleU32 output_count = 0u;
    CastleU32 i;
    CastleU32 j;

    for (i = 0u; i < chain->node_count; ++i) {
        indegree[i] = 0u;
        selected[i] = 0u;
    }

    for (i = 0u; i < chain->node_count; ++i) {
        for (j = 0u; j < chain->node_count; ++j) {
            if (i != j && hook_constraint_edge_(hook_claim_(chain->nodes[i]),
                                                hook_claim_(chain->nodes[j]))) {
                ++indegree[j];
            }
        }
    }

    while (output_count < chain->node_count) {
        CastleS32 best = -1;
        for (i = 0u; i < chain->node_count; ++i) {
            if (selected[i] || indegree[i] != 0u) continue;
            if (best < 0 || hook_default_before_(hook_claim_(chain->nodes[i]),
                                                 hook_claim_(chain->nodes[(CastleU32)best]))) {
                best = (CastleS32)i;
            }
        }
        if (best < 0) return 0;
        selected[(CastleU32)best] = 1u;
        output[output_count++] = chain->nodes[(CastleU32)best];
        for (j = 0u; j < chain->node_count; ++j) {
            if (!selected[j] && hook_constraint_edge_(
                    hook_claim_(chain->nodes[(CastleU32)best]),
                    hook_claim_(chain->nodes[j])) && indegree[j] > 0u) {
                --indegree[j];
            }
        }
    }

    for (i = 0u; i < chain->node_count; ++i) chain->nodes[i] = output[i];
    return 1;
}

static CastleResult hook_rebuild_chain_(RuntimeHookChain* chain) {
    CastleS32 index;
    CastleAddress top_target;
    CastleResult write_result;

    if (!chain || !chain->used) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (chain->node_count == 0u) {
        write_result = hook_write_chain_target_(chain, chain->original_target);
        if (write_result >= 0) Runtime_ByteZero(chain, (CastleU32)sizeof(*chain));
        return write_result;
    }
    if (!hook_sort_chain_(chain)) return CASTLE_ERROR_RESOURCE_CONFLICT;

    for (index = (CastleS32)chain->node_count - 1; index >= 0; --index) {
        RuntimeHookClaim* node = hook_claim_(chain->nodes[(CastleU32)index]);
        if (!node) return CASTLE_ERROR_RUNTIME_FAULT;
        node->next_target = index + 1 < (CastleS32)chain->node_count ?
            (void*)(ULONG_PTR)hook_claim_(chain->nodes[(CastleU32)index + 1u])->replacement_hook :
            (void*)(ULONG_PTR)chain->original_target;
        node->resource_generation = chain->generation + 1u;
    }
    top_target = hook_claim_(chain->nodes[0])->replacement_hook;
    write_result = hook_write_chain_target_(chain, top_target);
    if (write_result >= 0) ++chain->generation;
    return write_result;
}

static CastleResult hook_preflight_claim_(RuntimeHookClaim* claim) {
    void* site;
    CastleU32 index;
    HMODULE plugin_module;

    if (!claim) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (claim->type == HOOK_CLAIM_NAMED) {
        for (index = 0u; index < HOOK_MAX_CLAIMS; ++index) {
            RuntimeHookClaim* active = &g_claims[index];
            if (active->used && active->active && active->type == HOOK_CLAIM_NAMED &&
                Runtime_StringEquals(active->resource_id, active->resource_id_length,
                                     claim->resource_id, claim->resource_id_length)) {
                return CASTLE_ERROR_RESOURCE_CONFLICT;
            }
        }
        return CASTLE_OK;
    }

    site = Runtime_ResolveTarget(&claim->target);
    if (!site) return CASTLE_ERROR_INVALID_ARGUMENT;
    for (index = 0u; index < HOOK_MAX_CLAIMS; ++index) {
        RuntimeHookClaim* active = &g_claims[index];
        if (!active->used || !active->active || active->type == HOOK_CLAIM_NAMED) continue;
        if (!hook_range_overlap_(&active->target, &claim->target)) continue;
        if (active->type == HOOK_CLAIM_CHAIN && claim->type == HOOK_CLAIM_CHAIN &&
            active->target.rva == claim->target.rva &&
            active->hook_kind == claim->hook_kind &&
            Runtime_StringEquals(active->signature, active->signature_length,
                                 claim->signature, claim->signature_length)) continue;
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }

    if (claim->type == HOOK_CLAIM_EXCLUSIVE) {
        return Runtime_MemoryEquals(site, claim->expected, claim->byte_size) ?
            CASTLE_OK : CASTLE_ERROR_EXPECTED_BYTES;
    }
    if (claim->type == HOOK_CLAIM_STATE) {
        return Runtime_MemoryEquals(site, claim->expected, claim->byte_size) ||
               Runtime_MemoryEquals(site, claim->alternate, claim->byte_size) ?
            CASTLE_OK : CASTLE_ERROR_EXPECTED_BYTES;
    }

    plugin_module = Runtime_GetPluginModule(claim->plugin);
    if (!plugin_module || !claim->replacement_hook) return CASTLE_ERROR_INVALID_ARGUMENT;
    {
        MEMORY_BASIC_INFORMATION information;
        if (VirtualQuery((const void*)(ULONG_PTR)claim->replacement_hook,
                         &information, sizeof(information)) == 0u ||
            information.AllocationBase != plugin_module) return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    {
        RuntimeHookChain* chain = hook_find_chain_(claim);
        CastleAddress current = hook_current_target_(claim, site);
        if (chain) {
            if (!Runtime_StringEquals(chain->signature, chain->signature_length,
                                      claim->signature, claim->signature_length) ||
                chain->original_target != claim->expected_original_target) {
                return CASTLE_ERROR_SIGNATURE_MISMATCH;
            }
            return CASTLE_OK;
        }
        return current == claim->expected_original_target ?
            CASTLE_OK : CASTLE_ERROR_EXPECTED_BYTES;
    }
}

static CastleResult hook_activate_chain_(RuntimeHookClaim* claim) {
    RuntimeHookChain* chain = hook_find_chain_(claim);
    CastleU32 index;
    CastleResult result;

    if (!chain) {
        for (index = 0u; index < HOOK_MAX_CHAINS; ++index) {
            if (!g_chains[index].used) {
                chain = &g_chains[index];
                Runtime_ByteZero(chain, (CastleU32)sizeof(*chain));
                chain->used = 1;
                chain->hook_kind = claim->hook_kind;
                chain->target = claim->target;
                chain->site = Runtime_ResolveTarget(&claim->target);
                chain->original_target = claim->expected_original_target;
                Runtime_ByteCopy(chain->signature, claim->signature,
                                 claim->signature_length + 1u);
                chain->signature_length = claim->signature_length;
                break;
            }
        }
    }
    if (!chain || chain->node_count >= HOOK_MAX_CHAIN_NODES) {
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }

    chain->nodes[chain->node_count++] = claim->handle;
    claim->chain_index = (CastleS32)(chain - g_chains);
    result = hook_rebuild_chain_(chain);
    if (result < 0) {
        --chain->node_count;
        claim->chain_index = -1;
        if (chain->node_count == 0u) Runtime_ByteZero(chain, (CastleU32)sizeof(*chain));
    }
    return result;
}

static void hook_deactivate_claim_(RuntimeHookClaim* claim) {
    if (!claim || !claim->active) return;
    if (claim->type == HOOK_CLAIM_EXCLUSIVE || claim->type == HOOK_CLAIM_STATE) {
        void* site = Runtime_ResolveTarget(&claim->target);
        if (site) Runtime_WriteMemory(site, claim->saved, claim->byte_size,
                                      (claim->flags & CASTLE_PATCH_FLAG_CODE) != 0u);
    } else if (claim->type == HOOK_CLAIM_CHAIN && claim->chain_index >= 0 &&
               (CastleU32)claim->chain_index < HOOK_MAX_CHAINS) {
        RuntimeHookChain* chain = &g_chains[(CastleU32)claim->chain_index];
        CastleU32 index;
        for (index = 0u; index < chain->node_count; ++index) {
            if (chain->nodes[index] == claim->handle) {
                CastleU32 move;
                for (move = index + 1u; move < chain->node_count; ++move) {
                    chain->nodes[move - 1u] = chain->nodes[move];
                }
                --chain->node_count;
                hook_rebuild_chain_(chain);
                break;
            }
        }
    }
    claim->active = 0;
}

void Runtime_HookInitialize(void) {
    g_hook_lock = 0;
    g_resource_generation = 0u;
    Runtime_ByteZero(g_transactions, (CastleU32)sizeof(g_transactions));
    Runtime_ByteZero(g_claims, (CastleU32)sizeof(g_claims));
    Runtime_ByteZero(g_chains, (CastleU32)sizeof(g_chains));
}

const CastleHookApiV1* Runtime_GetHookApiV1(void) {
    return &g_hook_api;
}

static CastleResult CASTLE_RUNTIME_CALL hook_begin_transaction_(
    CastlePluginHandle plugin, CastleStringView label, CastleU32 flags,
    CastleTransactionHandle* out_transaction) {
    CastleU32 index;
    if (!out_transaction || !Runtime_GetPluginModule(plugin)) return CASTLE_ERROR_INVALID_ARGUMENT;

    Runtime_Lock(&g_hook_lock);
    for (index = 0u; index < HOOK_MAX_TRANSACTIONS; ++index) {
        RuntimeHookTransaction* transaction = &g_transactions[index];
        if (!transaction->used) {
            Runtime_ByteZero(transaction, (CastleU32)sizeof(*transaction));
            transaction->used = 1;
            transaction->handle = index + 1u;
            transaction->plugin = plugin;
            transaction->state = CASTLE_TRANSACTION_BUILDING;
            transaction->flags = flags;
            if (!hook_copy_view_(transaction->label, HOOK_MAX_TEXT,
                                 &transaction->label_length, label, 1)) {
                Runtime_ByteZero(transaction, (CastleU32)sizeof(*transaction));
                Runtime_Unlock(&g_hook_lock);
                return CASTLE_ERROR_INVALID_ARGUMENT;
            }
            *out_transaction = transaction->handle;
            Runtime_Unlock(&g_hook_lock);
            return CASTLE_OK;
        }
    }
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_ERROR_RESOURCE_CONFLICT;
}

static CastleResult CASTLE_RUNTIME_CALL hook_add_exclusive_(
    CastleTransactionHandle transaction_handle,
    const CastleExclusivePatchClaimV1* input,
    CastleClaimHandle* out_claim) {
    RuntimeHookTransaction* transaction;
    RuntimeHookClaim* claim;
    if (!input || input->magic != CASTLE_EXCLUSIVE_PATCH_MAGIC ||
        input->struct_size < CASTLE_SIZEOF_EXCLUSIVE_PATCH_V1 ||
        input->version != CASTLE_HOOK_STRUCTURE_VERSION_1 ||
        !hook_flags_valid_(input->flags) || input->target.size == 0u ||
        input->target.size > HOOK_MAX_PATCH_BYTES ||
        input->expected_size != input->target.size ||
        input->replacement_size != input->target.size ||
        !input->expected_bytes || !input->replacement_bytes) return CASTLE_ERROR_INVALID_ARGUMENT;

    Runtime_Lock(&g_hook_lock);
    transaction = hook_transaction_(transaction_handle);
    if (!transaction || transaction->state != CASTLE_TRANSACTION_BUILDING) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_STATE;
    }
    claim = hook_allocate_claim_(transaction, HOOK_CLAIM_EXCLUSIVE, out_claim);
    if (!claim) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    claim->flags = input->flags;
    claim->target = input->target;
    claim->byte_size = input->target.size;
    Runtime_ByteCopy(claim->expected, input->expected_bytes, claim->byte_size);
    Runtime_ByteCopy(claim->replacement, input->replacement_bytes, claim->byte_size);
    if (!hook_copy_view_(claim->label, HOOK_MAX_TEXT, &claim->label_length,
                         input->label, 1)) {
        Runtime_ByteZero(claim, (CastleU32)sizeof(*claim));
        --transaction->claim_count;
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL hook_add_state_(
    CastleTransactionHandle transaction_handle, const CastleStatePatchClaimV1* input,
    CastleClaimHandle* out_claim) {
    RuntimeHookTransaction* transaction;
    RuntimeHookClaim* claim;
    if (!input || input->magic != CASTLE_STATE_PATCH_MAGIC ||
        input->struct_size < CASTLE_SIZEOF_STATE_PATCH_V1 ||
        input->version != CASTLE_HOOK_STRUCTURE_VERSION_1 ||
        !hook_flags_valid_(input->flags) || input->target.size == 0u ||
        input->target.size > HOOK_MAX_PATCH_BYTES ||
        input->original_size != input->target.size ||
        input->enabled_size != input->target.size ||
        !input->original_bytes || !input->enabled_bytes ||
        (input->desired_state != CASTLE_PATCH_STATE_ORIGINAL &&
         input->desired_state != CASTLE_PATCH_STATE_ENABLED)) return CASTLE_ERROR_INVALID_ARGUMENT;

    Runtime_Lock(&g_hook_lock);
    transaction = hook_transaction_(transaction_handle);
    if (!transaction || transaction->state != CASTLE_TRANSACTION_BUILDING) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_STATE;
    }
    claim = hook_allocate_claim_(transaction, HOOK_CLAIM_STATE, out_claim);
    if (!claim) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    claim->flags = input->flags;
    claim->target = input->target;
    claim->byte_size = input->target.size;
    claim->desired_state = input->desired_state;
    Runtime_ByteCopy(claim->expected, input->original_bytes, claim->byte_size);
    Runtime_ByteCopy(claim->alternate, input->enabled_bytes, claim->byte_size);
    if (!hook_copy_view_(claim->label, HOOK_MAX_TEXT, &claim->label_length,
                         input->label, 1)) {
        Runtime_ByteZero(claim, (CastleU32)sizeof(*claim));
        --transaction->claim_count;
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}

static CastleResult hook_add_chain_common_(CastleTransactionHandle transaction_handle,
                                          const CastleChainHookClaimV1* input,
                                          CastleClaimHandle* out_claim,
                                          int require_call) {
    RuntimeHookTransaction* transaction;
    RuntimeHookClaim* claim;
    CastleU32 expected_size = require_call ? 5u : 4u;
    if (!input || input->magic != CASTLE_CHAIN_HOOK_MAGIC ||
        input->struct_size < CASTLE_SIZEOF_CHAIN_HOOK_V1 ||
        input->version != CASTLE_HOOK_STRUCTURE_VERSION_1 ||
        input->target.size != expected_size || input->replacement_hook == 0u ||
        input->expected_original_target == 0u || !input->signature_id.data ||
        input->signature_id.length == 0u || input->signature_id.length >= HOOK_MAX_SIGNATURE ||
        input->phase > CASTLE_HOOK_PHASE_FINAL ||
        input->priority > CASTLE_HOOK_PRIORITY_LATE) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (require_call && input->hook_kind != CASTLE_HOOK_REL32_CALL) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (!require_call && input->hook_kind != CASTLE_HOOK_IAT_POINTER &&
        input->hook_kind != CASTLE_HOOK_VTABLE_POINTER) return CASTLE_ERROR_INVALID_ARGUMENT;

    Runtime_Lock(&g_hook_lock);
    transaction = hook_transaction_(transaction_handle);
    if (!transaction || transaction->state != CASTLE_TRANSACTION_BUILDING) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_STATE;
    }
    claim = hook_allocate_claim_(transaction, HOOK_CLAIM_CHAIN, out_claim);
    if (!claim) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    claim->flags = input->flags;
    claim->target = input->target;
    claim->hook_kind = input->hook_kind;
    claim->expected_original_target = input->expected_original_target;
    claim->replacement_hook = input->replacement_hook;
    claim->phase = input->phase;
    claim->priority = input->priority;
    hook_copy_view_(claim->signature, HOOK_MAX_SIGNATURE, &claim->signature_length,
                    input->signature_id, 0);
    if (!hook_copy_view_(claim->label, HOOK_MAX_TEXT, &claim->label_length,
                         input->label, 1)) {
        Runtime_ByteZero(claim, (CastleU32)sizeof(*claim));
        --transaction->claim_count;
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL hook_add_relative_(
    CastleTransactionHandle transaction, const CastleChainHookClaimV1* claim,
    CastleClaimHandle* out_claim) {
    return hook_add_chain_common_(transaction, claim, out_claim, 1);
}

static CastleResult CASTLE_RUNTIME_CALL hook_add_pointer_(
    CastleTransactionHandle transaction, const CastleChainHookClaimV1* claim,
    CastleClaimHandle* out_claim) {
    return hook_add_chain_common_(transaction, claim, out_claim, 0);
}

static CastleResult CASTLE_RUNTIME_CALL hook_add_named_(
    CastleTransactionHandle transaction_handle, const CastleNamedResourceClaimV1* input,
    CastleClaimHandle* out_claim) {
    RuntimeHookTransaction* transaction;
    RuntimeHookClaim* claim;
    if (!input || input->magic != CASTLE_NAMED_RESOURCE_MAGIC ||
        input->struct_size < CASTLE_SIZEOF_NAMED_RESOURCE_V1 ||
        input->version != CASTLE_HOOK_STRUCTURE_VERSION_1 ||
        !input->resource_id.data || input->resource_id.length == 0u ||
        input->resource_id.length >= HOOK_MAX_TEXT) return CASTLE_ERROR_INVALID_ARGUMENT;

    Runtime_Lock(&g_hook_lock);
    transaction = hook_transaction_(transaction_handle);
    if (!transaction || transaction->state != CASTLE_TRANSACTION_BUILDING) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_STATE;
    }
    claim = hook_allocate_claim_(transaction, HOOK_CLAIM_NAMED, out_claim);
    if (!claim) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    claim->flags = input->flags;
    claim->resource_kind = input->resource_kind;
    claim->phase = input->phase;
    claim->priority = input->priority;
    hook_copy_view_(claim->resource_id, HOOK_MAX_TEXT, &claim->resource_id_length,
                    input->resource_id, 0);
    hook_copy_view_(claim->label, HOOK_MAX_TEXT, &claim->label_length,
                    input->label, 1);
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL hook_add_order_(
    CastleTransactionHandle transaction_handle,
    const CastleOrderConstraintV1* input) {
    RuntimeHookTransaction* transaction;
    RuntimeHookClaim* claim;
    if (!input || input->magic != CASTLE_ORDER_CONSTRAINT_MAGIC ||
        input->struct_size < CASTLE_SIZEOF_ORDER_CONSTRAINT_V1 ||
        input->version != CASTLE_HOOK_STRUCTURE_VERSION_1 ||
        (input->relation != CASTLE_ORDER_OTHER_BEFORE &&
         input->relation != CASTLE_ORDER_OTHER_AFTER) ||
        !input->other_plugin_id.data || input->other_plugin_id.length == 0u ||
        input->other_plugin_id.length >= RUNTIME_PLUGIN_ID_CAP) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    Runtime_Lock(&g_hook_lock);
    transaction = hook_transaction_(transaction_handle);
    claim = hook_claim_(input->claim_handle);
    if (!transaction || transaction->state != CASTLE_TRANSACTION_BUILDING ||
        !claim || claim->transaction != transaction_handle ||
        claim->type != HOOK_CLAIM_CHAIN || claim->order_relation != 0u) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_STATE;
    }
    claim->order_relation = input->relation;
    hook_copy_view_(claim->order_plugin_id, RUNTIME_PLUGIN_ID_CAP,
                    &claim->order_plugin_id_length, input->other_plugin_id, 0);
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL hook_preflight_(CastleTransactionHandle handle) {
    RuntimeHookTransaction* transaction;
    CastleU32 index;
    CastleU32 other_index;
    CastleResult result = CASTLE_OK;
    int optional_failed = 0;

    Runtime_Lock(&g_hook_lock);
    transaction = hook_transaction_(handle);
    if (!transaction || transaction->state != CASTLE_TRANSACTION_BUILDING) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_STATE;
    }

    for (index = 0u; index < transaction->claim_count; ++index) {
        for (other_index = index + 1u; other_index < transaction->claim_count; ++other_index) {
            RuntimeHookClaim* left = hook_claim_(transaction->claims[index]);
            RuntimeHookClaim* right = hook_claim_(transaction->claims[other_index]);
            if (!hook_claims_compatible_(left, right)) {
                left->result = CASTLE_ERROR_RESOURCE_CONFLICT;
                right->result = CASTLE_ERROR_RESOURCE_CONFLICT;
                Runtime_Unlock(&g_hook_lock);
                return CASTLE_ERROR_RESOURCE_CONFLICT;
            }
        }
    }

    for (index = 0u; index < transaction->claim_count; ++index) {
        RuntimeHookClaim* claim = hook_claim_(transaction->claims[index]);
        result = hook_preflight_claim_(claim);
        claim->result = result;
        if (result < 0) {
            if ((claim->flags & CASTLE_PATCH_FLAG_OPTIONAL) == 0u) break;
            optional_failed = 1;
            result = CASTLE_OK;
        }
    }
    if (result >= 0) transaction->state = CASTLE_TRANSACTION_PREFLIGHTED;
    Runtime_Unlock(&g_hook_lock);
    return result < 0 ? result :
        (optional_failed ? CASTLE_STATUS_OPTIONAL_UNAVAILABLE : CASTLE_OK);
}

static CastleResult CASTLE_RUNTIME_CALL hook_commit_(CastleTransactionHandle handle) {
    RuntimeHookTransaction* transaction;
    CastleU32 index;
    CastleResult result = CASTLE_OK;

    Runtime_Lock(&g_hook_lock);
    transaction = hook_transaction_(handle);
    if (!transaction || transaction->state != CASTLE_TRANSACTION_PREFLIGHTED) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_STATE;
    }

    for (index = 0u; index < transaction->claim_count; ++index) {
        RuntimeHookClaim* claim = hook_claim_(transaction->claims[index]);
        void* site;
        if (claim->result < 0 && (claim->flags & CASTLE_PATCH_FLAG_OPTIONAL)) continue;
        result = hook_preflight_claim_(claim);
        if (result < 0) break;

        if (claim->type == HOOK_CLAIM_EXCLUSIVE || claim->type == HOOK_CLAIM_STATE) {
            const CastleU8* desired;
            site = Runtime_ResolveTarget(&claim->target);
            Runtime_ByteCopy(claim->saved, site, claim->byte_size);
            desired = claim->type == HOOK_CLAIM_EXCLUSIVE ? claim->replacement :
                (claim->desired_state == CASTLE_PATCH_STATE_ENABLED ?
                    claim->alternate : claim->expected);
            result = Runtime_WriteMemory(site, desired, claim->byte_size,
                (claim->flags & CASTLE_PATCH_FLAG_CODE) != 0u);
        } else if (claim->type == HOOK_CLAIM_CHAIN) {
            result = hook_activate_chain_(claim);
        } else {
            result = CASTLE_OK;
        }

        if (result < 0) break;
        claim->active = 1;
        claim->result = CASTLE_OK;
        claim->resource_generation = ++g_resource_generation;
    }

    if (result < 0) {
        while (index > 0u) {
            --index;
            hook_deactivate_claim_(hook_claim_(transaction->claims[index]));
        }
        transaction->state = CASTLE_TRANSACTION_ROLLED_BACK;
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_ROLLED_BACK;
    }

    transaction->state = CASTLE_TRANSACTION_COMMITTED;
    Runtime_Unlock(&g_hook_lock);
    Runtime_DiagnosticAppend("[Hook] transaction committed.");
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL hook_abort_(CastleTransactionHandle handle) {
    RuntimeHookTransaction* transaction;
    Runtime_Lock(&g_hook_lock);
    transaction = hook_transaction_(handle);
    if (!transaction || (transaction->state != CASTLE_TRANSACTION_BUILDING &&
                         transaction->state != CASTLE_TRANSACTION_PREFLIGHTED)) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_TRANSACTION_STATE;
    }
    transaction->state = CASTLE_TRANSACTION_ABORTED;
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL hook_get_claim_(CastleClaimHandle handle,
                                                        CastleClaimResultV1* output) {
    RuntimeHookClaim* claim;
    CastleStringView owner;
    if (!output || output->magic != CASTLE_CLAIM_RESULT_MAGIC ||
        output->struct_size < CASTLE_SIZEOF_CLAIM_RESULT_V1 ||
        output->version != CASTLE_HOOK_STRUCTURE_VERSION_1) return CASTLE_ERROR_INVALID_ARGUMENT;

    Runtime_Lock(&g_hook_lock);
    claim = hook_claim_(handle);
    if (!claim) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    owner = Runtime_GetPluginIdView(claim->plugin);
    output->flags = claim->active ? 1u : 0u;
    output->claim_handle = claim->handle;
    output->result = claim->result;
    output->resource_generation = claim->resource_generation;
    output->owner_plugin_id = owner;
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL hook_get_binding_(CastleClaimHandle handle,
                                                          CastleHookBindingV1* output) {
    RuntimeHookClaim* claim;
    if (!output || output->magic != CASTLE_HOOK_BINDING_MAGIC ||
        output->struct_size < CASTLE_SIZEOF_HOOK_BINDING_V1 ||
        output->version != CASTLE_HOOK_STRUCTURE_VERSION_1) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_hook_lock);
    claim = hook_claim_(handle);
    if (!claim || !claim->active || claim->type != HOOK_CLAIM_CHAIN) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_NOT_READY;
    }
    output->flags = 0u;
    output->claim_handle = claim->handle;
    output->next_slot = &claim->next_target;
    output->chain_generation = claim->resource_generation;
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL hook_describe_(CastleClaimHandle handle,
                                                       CastleDiagnosticBufferV1* output) {
    RuntimeHookClaim* claim;
    if (!output || output->magic != CASTLE_DIAGNOSTIC_BUFFER_MAGIC ||
        output->struct_size < CASTLE_SIZEOF_DIAGNOSTIC_BUFFER_V1 ||
        output->buffer_version != CASTLE_DIAGNOSTIC_BUFFER_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_hook_lock);
    claim = hook_claim_(handle);
    if (!claim) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    output->required_capacity = claim->label_length + 1u;
    output->bytes_written = 0u;
    if (!output->utf8_buffer || output->buffer_capacity < output->required_capacity) {
        Runtime_Unlock(&g_hook_lock);
        return CASTLE_ERROR_BUFFER_TOO_SMALL;
    }
    Runtime_ByteCopy(output->utf8_buffer, claim->label, claim->label_length + 1u);
    output->bytes_written = claim->label_length;
    output->first_generation = claim->resource_generation;
    output->last_generation = claim->resource_generation;
    Runtime_Unlock(&g_hook_lock);
    return CASTLE_OK;
}
