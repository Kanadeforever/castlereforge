#include "runtime_internal.h"

#define RUNTIME_MODULE_MAX_RECORDS 64u
#define RUNTIME_MODULE_NAME_CAP 128u

typedef struct RuntimeModuleRecord {
    int used;
    HMODULE module;
    CastlePluginHandle first_owner;
    CastleU32 pinned;
    CastleU32 generation;
} RuntimeModuleRecord;

static volatile LONG g_module_lock;
static RuntimeModuleRecord g_module_records[RUNTIME_MODULE_MAX_RECORDS];
static CastleU32 g_module_generation;

static CastleResult CASTLE_RUNTIME_CALL module_load_plugin_(
    CastlePluginHandle plugin, CastleStringView relative_path,
    CastleU32 flags, CastleModule* out_module);
static CastleResult CASTLE_RUNTIME_CALL module_load_system_(
    CastlePluginHandle plugin, CastleStringView module_name,
    CastleU32 flags, CastleModule* out_module);
static CastleResult CASTLE_RUNTIME_CALL module_get_procedure_(
    CastleModule module, CastleStringView procedure_name,
    CastleAddress* out_procedure);
static CastleResult CASTLE_RUNTIME_CALL module_pin_(CastleModule module);
static CastleResult CASTLE_RUNTIME_CALL module_get_state_(
    CastleModule module, CastleModuleStateV1* out_state);

static const CastleModuleApiV1 g_module_api = {
    CASTLE_MODULE_API_MAGIC,
    CASTLE_SIZEOF_MODULE_API_V1,
    CASTLE_MODULE_API_VERSION_1,
    CASTLE_MODULE_CAP_PLUGIN_RELATIVE | CASTLE_MODULE_CAP_SYSTEM_ALLOWLIST |
        CASTLE_MODULE_CAP_PIN,
    module_load_plugin_,
    module_load_system_,
    module_get_procedure_,
    module_pin_,
    module_get_state_
};

static RuntimeModuleRecord* module_find_locked_(HMODULE module) {
    CastleU32 index;
    for (index = 0u; index < RUNTIME_MODULE_MAX_RECORDS; ++index) {
        if (g_module_records[index].used && g_module_records[index].module == module) {
            return &g_module_records[index];
        }
    }
    return NULL;
}

static RuntimeModuleRecord* module_record_locked_(HMODULE module,
                                                   CastlePluginHandle owner) {
    RuntimeModuleRecord* record = module_find_locked_(module);
    CastleU32 index;
    if (record) return record;
    for (index = 0u; index < RUNTIME_MODULE_MAX_RECORDS; ++index) {
        if (!g_module_records[index].used) {
            record = &g_module_records[index];
            Runtime_ByteZero(record, (CastleU32)sizeof(*record));
            record->used = 1;
            record->module = module;
            record->first_owner = owner;
            record->generation = ++g_module_generation;
            return record;
        }
    }
    return NULL;
}

static int module_pin_handle_(HMODULE module) {
    HMODULE pinned = NULL;
    return module && GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCWSTR)(const void*)module, &pinned);
}

static CastleResult module_finish_load_(HMODULE module, CastlePluginHandle owner,
                                        CastleU32 flags, CastleModule* out_module) {
    RuntimeModuleRecord* record;
    if (!module || !out_module) return CASTLE_ERROR_RUNTIME_FAULT;
    Runtime_Lock(&g_module_lock);
    record = module_record_locked_(module, owner);
    if (!record) {
        Runtime_Unlock(&g_module_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    if ((flags & CASTLE_MODULE_LOAD_PIN) != 0u && !record->pinned) {
        if (!module_pin_handle_(module)) {
            Runtime_Unlock(&g_module_lock);
            return CASTLE_ERROR_RUNTIME_FAULT;
        }
        record->pinned = 1u;
        record->generation = ++g_module_generation;
    }
    *out_module = (CastleModule)(ULONG_PTR)module;
    Runtime_Unlock(&g_module_lock);
    return CASTLE_OK;
}

static int module_view_to_ascii_(CastleStringView view, char* output,
                                 CastleU32 capacity) {
    CastleU32 index;
    if (!view.data || view.length == 0u || view.length + 1u > capacity) return 0;
    for (index = 0u; index < view.length; ++index) {
        CastleU8 value = (CastleU8)view.data[index];
        if (value < 0x20u || value > 0x7Eu || value == '/' || value == '\\') return 0;
        output[index] = (char)value;
    }
    output[view.length] = '\0';
    return 1;
}

static int module_ascii_equal_ci_(const char* left, const char* right) {
    CastleU32 index = 0u;
    if (!left || !right) return 0;
    while (left[index] && right[index]) {
        char a = left[index];
        char b = right[index];
        if (a >= 'A' && a <= 'Z') a = (char)(a + ('a' - 'A'));
        if (b >= 'A' && b <= 'Z') b = (char)(b + ('a' - 'A'));
        if (a != b) return 0;
        ++index;
    }
    return left[index] == right[index];
}

void Runtime_ModuleInitialize(void) {
    g_module_lock = 0;
    g_module_generation = 1u;
    Runtime_ByteZero(g_module_records, (CastleU32)sizeof(g_module_records));
}

const CastleModuleApiV1* Runtime_GetModuleApiV1(void) {
    return &g_module_api;
}

static CastleResult CASTLE_RUNTIME_CALL module_load_plugin_(
    CastlePluginHandle plugin, CastleStringView relative_path,
    CastleU32 flags, CastleModule* out_module) {
    const CastlePathApiV1* path_api = Runtime_GetPathApiV1();
    CastleU16 wide_path[RUNTIME_PATH_WIDE_CAP];
    CastleU16 relative_wide[RUNTIME_PATH_WIDE_CAP];
    CastleWideStringView relative;
    CastleU32 path_length = 0u;
    int converted;
    HMODULE module;
    if (!out_module || (flags & ~CASTLE_MODULE_LOAD_PIN) != 0u ||
        !Runtime_GetPluginModule(plugin) || !path_api || !relative_path.data ||
        relative_path.length == 0u || relative_path.length >= RUNTIME_PATH_WIDE_CAP) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    *out_module = 0u;
    converted = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        relative_path.data, (int)relative_path.length, (WCHAR*)relative_wide,
        RUNTIME_PATH_WIDE_CAP - 1u);
    if (converted <= 0) return CASTLE_ERROR_INVALID_ARGUMENT;
    relative_wide[converted] = 0u;
    relative.data = relative_wide;
    relative.length = (CastleU32)converted;
    if (path_api->BuildPluginRelativePathWide(plugin, relative, wide_path,
            RUNTIME_PATH_WIDE_CAP, &path_length) < 0) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    (void)path_length;
    module = LoadLibraryW((const WCHAR*)wide_path);
    return module_finish_load_(module, plugin, flags, out_module);
}

static CastleResult CASTLE_RUNTIME_CALL module_load_system_(
    CastlePluginHandle plugin, CastleStringView module_name,
    CastleU32 flags, CastleModule* out_module) {
    char name[RUNTIME_MODULE_NAME_CAP];
    WCHAR wide[RUNTIME_MODULE_NAME_CAP];
    CastleU32 index;
    HMODULE module;
    if (!out_module || (flags & ~CASTLE_MODULE_LOAD_PIN) != 0u ||
        !Runtime_GetPluginModule(plugin) ||
        !module_view_to_ascii_(module_name, name, RUNTIME_MODULE_NAME_CAP)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!module_ascii_equal_ci_(name, "kernel32.dll") &&
        !module_ascii_equal_ci_(name, "user32.dll") &&
        !module_ascii_equal_ci_(name, "gdi32.dll") &&
        !module_ascii_equal_ci_(name, "winmm.dll") &&
        !module_ascii_equal_ci_(name, "ddraw.dll")) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    for (index = 0u; name[index] && index + 1u < RUNTIME_MODULE_NAME_CAP; ++index) {
        wide[index] = (WCHAR)(CastleU8)name[index];
    }
    wide[index] = L'\0';
    *out_module = 0u;
    module = LoadLibraryW(wide);
    return module_finish_load_(module, plugin, flags, out_module);
}

static CastleResult CASTLE_RUNTIME_CALL module_get_procedure_(
    CastleModule module, CastleStringView procedure_name,
    CastleAddress* out_procedure) {
    char name[RUNTIME_MODULE_NAME_CAP];
    FARPROC address;
    if (!module || !out_procedure ||
        !module_view_to_ascii_(procedure_name, name, RUNTIME_MODULE_NAME_CAP)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    *out_procedure = 0u;
    Runtime_Lock(&g_module_lock);
    if (!module_find_locked_((HMODULE)(ULONG_PTR)module)) {
        Runtime_Unlock(&g_module_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Unlock(&g_module_lock);
    address = GetProcAddress((HMODULE)(ULONG_PTR)module, name);
    if (!address) return CASTLE_ERROR_NOT_READY;
    *out_procedure = (CastleAddress)(ULONG_PTR)address;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL module_pin_(CastleModule module) {
    RuntimeModuleRecord* record;
    if (!module) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_module_lock);
    record = module_find_locked_((HMODULE)(ULONG_PTR)module);
    if (!record) {
        Runtime_Unlock(&g_module_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (record->pinned) {
        Runtime_Unlock(&g_module_lock);
        return CASTLE_STATUS_ALREADY_DONE;
    }
    if (!module_pin_handle_(record->module)) {
        Runtime_Unlock(&g_module_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    record->pinned = 1u;
    record->generation = ++g_module_generation;
    Runtime_Unlock(&g_module_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL module_get_state_(
    CastleModule module, CastleModuleStateV1* out_state) {
    RuntimeModuleRecord* record;
    if (!module || !out_state || out_state->magic != CASTLE_MODULE_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_MODULE_STATE_V1 ||
        out_state->version != CASTLE_MODULE_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_module_lock);
    record = module_find_locked_((HMODULE)(ULONG_PTR)module);
    if (!record) {
        Runtime_Unlock(&g_module_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    out_state->flags = 0u;
    out_state->module = (CastleModule)(ULONG_PTR)record->module;
    out_state->first_owner = record->first_owner;
    out_state->pinned = record->pinned;
    out_state->generation = record->generation;
    Runtime_Unlock(&g_module_lock);
    return CASTLE_OK;
}
