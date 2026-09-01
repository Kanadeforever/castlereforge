#include "client_internal.h"

static const BYTE g_expected_entry[CASTLE_RPG_ENTRY_PATCH_SIZE] = {
    0x55u, 0x8Bu, 0xECu, 0x6Au, 0xFFu
};

static int client_bytes_equal_(const BYTE* left, const BYTE* right, CastleU32 count) {
    CastleU32 index;
    if (!left || !right) return 0;
    for (index = 0u; index < count; ++index) {
        if (left[index] != right[index]) return 0;
    }
    return 1;
}

static void client_copy_bytes_(BYTE* output, const BYTE* input, CastleU32 count) {
    CastleU32 index;
    if (!output || !input) return;
    for (index = 0u; index < count; ++index) output[index] = input[index];
}

static void client_write_rel32_jump_(BYTE output[5], const void* target, const BYTE* site) {
    LONG relative = (LONG)((const BYTE*)target - (site + 5u));
    output[0] = 0xE9u;
    output[1] = (BYTE)((DWORD)relative & 0xFFu);
    output[2] = (BYTE)(((DWORD)relative >> 8) & 0xFFu);
    output[3] = (BYTE)(((DWORD)relative >> 16) & 0xFFu);
    output[4] = (BYTE)(((DWORD)relative >> 24) & 0xFFu);
}

static void* client_read_rel32_target_(const BYTE* site) {
    LONG relative;
    if (!site || site[0] != 0xE9u) return NULL;
    relative = (LONG)((DWORD)site[1] |
                      ((DWORD)site[2] << 8) |
                      ((DWORD)site[3] << 16) |
                      ((DWORD)site[4] << 24));
    return (void*)(site + 5u + relative);
}

static const CastlePluginExportV1* client_query_owner_export_(void* gate_target) {
    MEMORY_BASIC_INFORMATION information;
    MEMORY_BASIC_INFORMATION export_information;
    HMODULE owner_module;
    FARPROC query_address;
    CastlePluginQueryFn query;
    const CastlePluginExportV1* owner_export;

    if (!gate_target || VirtualQuery(gate_target, &information, sizeof(information)) == 0u) return NULL;
    owner_module = (HMODULE)information.AllocationBase;
    if (!owner_module) return NULL;

    query_address = GetProcAddress(owner_module, "CastlePlugin_Query");
    if (!query_address) return NULL;
    if (!Client_CopyProcedureAddress(&query, (CastleU32)sizeof(query),
            query_address)) return NULL;
    owner_export = query(CASTLE_PLUGIN_EXPORT_VERSION_1);
    if (!owner_export ||
        VirtualQuery(owner_export, &export_information,
                     sizeof(export_information)) == 0u ||
        export_information.State != MEM_COMMIT ||
        export_information.AllocationBase != owner_module ||
        (ULONG_PTR)owner_export + CASTLE_SIZEOF_PLUGIN_EXPORT_V1 <
            (ULONG_PTR)owner_export ||
        (ULONG_PTR)owner_export + CASTLE_SIZEOF_PLUGIN_EXPORT_V1 >
            (ULONG_PTR)export_information.BaseAddress + export_information.RegionSize ||
        owner_export->magic != CASTLE_PLUGIN_QUERY_MAGIC ||
        owner_export->struct_size < CASTLE_SIZEOF_PLUGIN_EXPORT_V1 ||
        owner_export->export_version != CASTLE_PLUGIN_EXPORT_VERSION_1 ||
        owner_export->entry_gate_thunk != (CastleAddress)(ULONG_PTR)gate_target) return NULL;
    return owner_export;
}

CastleResult Client_InstallOrJoinEntryGateAt(BYTE* entry) {
    BYTE patch[5];
    MEMORY_BASIC_INFORMATION entry_information;
    DWORD old_protection = 0u;
    DWORD ignored = 0u;
    void* current_target;

    if (!entry || !g_client_export) return CASTLE_ERROR_INVALID_ARGUMENT;
    g_client_entry = entry;
    g_client_entry_after = g_client_entry + CASTLE_RPG_ENTRY_PATCH_SIZE;
    if (VirtualQuery(g_client_entry, &entry_information,
                     sizeof(entry_information)) == 0u ||
        entry_information.State != MEM_COMMIT) {
        InterlockedExchange(&g_client_state, CASTLE_CLIENT_LATE_LOAD_UNSUPPORTED);
        return CASTLE_ERROR_LATE_LOAD_UNSUPPORTED;
    }

    if (client_bytes_equal_(g_client_entry, g_expected_entry,
                            CASTLE_RPG_ENTRY_PATCH_SIZE)) {
        client_write_rel32_jump_(patch, (const void*)&CastleRuntimeClient_EntryGateThunk,
                                 g_client_entry);
        if (!VirtualProtect(g_client_entry, CASTLE_RPG_ENTRY_PATCH_SIZE,
                            PAGE_EXECUTE_READWRITE, &old_protection)) {
            return CASTLE_ERROR_RUNTIME_FAULT;
        }
        client_copy_bytes_(g_client_entry, patch, CASTLE_RPG_ENTRY_PATCH_SIZE);
        FlushInstructionCache(GetCurrentProcess(), g_client_entry,
                              CASTLE_RPG_ENTRY_PATCH_SIZE);
        VirtualProtect(g_client_entry, CASTLE_RPG_ENTRY_PATCH_SIZE,
                       old_protection, &ignored);
        InterlockedExchange(&g_client_state, CASTLE_CLIENT_GATE_OWNER);
        return CASTLE_OK;
    }

    current_target = client_read_rel32_target_(g_client_entry);
    if (current_target && client_query_owner_export_(current_target)) {
        InterlockedExchange(&g_client_state, CASTLE_CLIENT_GATE_FOLLOWER);
        return CASTLE_OK;
    }

    InterlockedExchange(&g_client_state, CASTLE_CLIENT_PLUGIN_FAILED);
    return CASTLE_ERROR_ENTRY_GATE_CONFLICT;
}

CastleResult Client_InstallOrJoinEntryGate(void) {
    HMODULE game_module = GetModuleHandleW(NULL);
    if (!game_module) return CASTLE_ERROR_RUNTIME_FAULT;
    return Client_InstallOrJoinEntryGateAt(
        (BYTE*)game_module + CASTLE_RPG_ENTRY_RVA_V1);
}

CastleResult Client_RestoreKnownEntryGateAt(BYTE* entry) {
    void* target;
    MEMORY_BASIC_INFORMATION entry_information;
    DWORD old_protection = 0u;
    DWORD ignored = 0u;
    CastleU32 index;

    if (!entry) return CASTLE_ERROR_INVALID_ARGUMENT;
    g_client_entry = entry;
    g_client_entry_after = entry + CASTLE_RPG_ENTRY_PATCH_SIZE;
    if (VirtualQuery(g_client_entry, &entry_information,
                     sizeof(entry_information)) == 0u ||
        entry_information.State != MEM_COMMIT) {
        return CASTLE_ERROR_LATE_LOAD_UNSUPPORTED;
    }

    if (client_bytes_equal_(g_client_entry, g_expected_entry,
                            CASTLE_RPG_ENTRY_PATCH_SIZE)) return CASTLE_STATUS_ALREADY_DONE;

    target = client_read_rel32_target_(g_client_entry);
    if (!target || !client_query_owner_export_(target)) return CASTLE_ERROR_ENTRY_GATE_CONFLICT;

    if (!VirtualProtect(g_client_entry, CASTLE_RPG_ENTRY_PATCH_SIZE,
                        PAGE_EXECUTE_READWRITE, &old_protection)) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    for (index = 0u; index < CASTLE_RPG_ENTRY_PATCH_SIZE; ++index) {
        g_client_entry[index] = g_expected_entry[index];
    }
    FlushInstructionCache(GetCurrentProcess(), g_client_entry,
                          CASTLE_RPG_ENTRY_PATCH_SIZE);
    VirtualProtect(g_client_entry, CASTLE_RPG_ENTRY_PATCH_SIZE,
                   old_protection, &ignored);
    return CASTLE_OK;
}

CastleResult Client_RestoreKnownEntryGate(void) {
    HMODULE game_module;
    if (g_client_entry) return Client_RestoreKnownEntryGateAt(g_client_entry);
    game_module = GetModuleHandleW(NULL);
    if (!game_module) return CASTLE_ERROR_RUNTIME_FAULT;
    return Client_RestoreKnownEntryGateAt(
        (BYTE*)game_module + CASTLE_RPG_ENTRY_RVA_V1);
}

/* Entry Gate 在主线程调用这个普通 C 函数，便于裸汇编保持最小。 */
static void __cdecl client_gate_bootstrap_(void) {
    CastleRuntimeClient_BootstrapAll(CASTLE_BOOTSTRAP_TRIGGER_ENTRY_GATE,
                                     (CastleModule)(ULONG_PTR)g_client_module);
}

/*
 * 裸函数精确保存入口寄存器，调用 BootstrapAll 后重放原游戏前 5 字节。
 * 本文件只面向 x86 MSVC ABI；其它架构会在 ABI 编译阶段被拒绝。
 */
void __declspec(naked) CastleRuntimeClient_EntryGateThunk(void) {
    __asm {
        pushfd
        pushad
        call client_gate_bootstrap_
        popad
        popfd
        push ebp
        mov ebp, esp
        push 0FFFFFFFFh
        jmp dword ptr [g_client_entry_after]
    }
}
