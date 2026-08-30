#include "client_internal.h"

typedef struct ClientCandidate {
    HMODULE module;
    const CastlePluginExportV1* plugin_export;
} ClientCandidate;

HMODULE g_client_module;
CastlePluginExportV1* g_client_export;
volatile LONG g_client_state;
BYTE* g_client_entry;
BYTE* g_client_entry_after;

static int client_view_compare_(CastleStringView left, CastleStringView right) {
    CastleU32 index;
    CastleU32 common = left.length < right.length ? left.length : right.length;
    for (index = 0u; index < common; ++index) {
        CastleU8 a = (CastleU8)left.data[index];
        CastleU8 b = (CastleU8)right.data[index];
        if (a < b) return -1;
        if (a > b) return 1;
    }
    if (left.length < right.length) return -1;
    if (left.length > right.length) return 1;
    return 0;
}

static int client_valid_export_(HMODULE module,
                                const CastlePluginExportV1* plugin_export) {
    MEMORY_BASIC_INFORMATION information;
    if (!module || !plugin_export ||
        plugin_export->magic != CASTLE_PLUGIN_QUERY_MAGIC ||
        plugin_export->struct_size < CASTLE_SIZEOF_PLUGIN_EXPORT_V1 ||
        plugin_export->export_version != CASTLE_PLUGIN_EXPORT_VERSION_1 ||
        !plugin_export->plugin_descriptor || !plugin_export->client_config ||
        !plugin_export->client_bootstrap) return 0;

    if (plugin_export->plugin_descriptor->magic != CASTLE_PLUGIN_DESC_MAGIC ||
        plugin_export->plugin_descriptor->struct_size < CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1 ||
        plugin_export->client_config->magic != CASTLE_CLIENT_CONFIG_MAGIC ||
        plugin_export->client_config->struct_size < CASTLE_SIZEOF_CLIENT_CONFIG_V1) return 0;

    if (VirtualQuery((const void*)plugin_export->client_bootstrap,
                     &information, sizeof(information)) == 0u ||
        information.AllocationBase != module) return 0;
    return 1;
}

static void client_sort_candidates_(ClientCandidate* candidates, CastleU32 count) {
    CastleU32 outer;
    for (outer = 1u; outer < count; ++outer) {
        ClientCandidate moving = candidates[outer];
        CastleU32 position = outer;
        while (position > 0u &&
               client_view_compare_(moving.plugin_export->plugin_descriptor->plugin_id,
                                    candidates[position - 1u].plugin_export->plugin_descriptor->plugin_id) < 0) {
            candidates[position] = candidates[position - 1u];
            --position;
        }
        candidates[position] = moving;
    }
}

static CastleResult client_collect_candidates_(ClientCandidate* candidates,
                                               CastleU32 capacity,
                                               CastleU32* out_count) {
    HANDLE snapshot;
    MODULEENTRY32W entry;
    CastleU32 count = 0u;
    CastleU32 index;

    if (!candidates || !out_count || capacity == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_count = 0u;
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
                                        GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE) return CASTLE_ERROR_RUNTIME_FAULT;

    /* 使用 volatile 逐字节清零，避免 Client 无 CRT 构建生成 memset。 */
    {
        volatile BYTE* bytes = (volatile BYTE*)&entry;
        for (index = 0u; index < (CastleU32)sizeof(entry); ++index) bytes[index] = 0u;
    }
    entry.dwSize = sizeof(entry);

    if (Module32FirstW(snapshot, &entry)) {
        do {
            FARPROC query_address = GetProcAddress(entry.hModule, "CastlePlugin_Query");
            if (query_address) {
                CastlePluginQueryFn query = (CastlePluginQueryFn)query_address;
                const CastlePluginExportV1* plugin_export = query(CASTLE_PLUGIN_EXPORT_VERSION_1);
                if (client_valid_export_(entry.hModule, plugin_export)) {
                    if (count >= capacity) {
                        CloseHandle(snapshot);
                        return CASTLE_ERROR_RESOURCE_CONFLICT;
                    }
                    candidates[count].module = entry.hModule;
                    candidates[count].plugin_export = plugin_export;
                    ++count;
                }
            }
        } while (Module32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    client_sort_candidates_(candidates, count);

    for (index = 1u; index < count; ++index) {
        if (client_view_compare_(candidates[index - 1u].plugin_export->plugin_descriptor->plugin_id,
                                 candidates[index].plugin_export->plugin_descriptor->plugin_id) == 0) {
            return CASTLE_ERROR_DUPLICATE_PLUGIN_ID;
        }
    }
    *out_count = count;
    return CASTLE_OK;
}

static int client_build_runtime_path_(WCHAR* output, DWORD capacity) {
    static const WCHAR runtime_name[] = L"Castle_Runtime.dll";
    DWORD length;
    DWORD directory_length;
    DWORD name_index = 0u;

    if (!output || capacity < 32u || !g_client_module) return 0;
    length = GetModuleFileNameW(g_client_module, output, capacity);
    if (length == 0u || length >= capacity) return 0;

    directory_length = length;
    while (directory_length > 0u && output[directory_length - 1u] != L'\\' &&
           output[directory_length - 1u] != L'/') --directory_length;
    if (directory_length == 0u) return 0;

    while (runtime_name[name_index]) {
        if (directory_length + name_index + 1u >= capacity) return 0;
        output[directory_length + name_index] = runtime_name[name_index];
        ++name_index;
    }
    output[directory_length + name_index] = L'\0';
    return 1;
}

static CastleResult client_bootstrap_local_(CastleU32 mode,
                                            const CastleRuntimeApiV1* runtime_api,
                                            CastleResult fault_result) {
    ClientCandidate candidates[CLIENT_MAX_PLUGINS];
    CastleU32 count = 0u;
    CastleU32 index;
    CastleResult collect_result = client_collect_candidates_(candidates,
                                                             CLIENT_MAX_PLUGINS,
                                                             &count);
    CastleResult final_result = CASTLE_OK;

    if (collect_result < 0) return collect_result;
    for (index = 0u; index < count; ++index) {
        CastleResult result;
        CastlePluginHandle handle = 0u;
        if (mode == CASTLE_CLIENT_BOOTSTRAP_INTEGRATED && runtime_api) {
            CastlePluginDescriptorV1 descriptor;
            const CastlePluginDescriptorV1* source =
                candidates[index].plugin_export->plugin_descriptor;
            descriptor.magic = source->magic;
            descriptor.struct_size = source->struct_size;
            descriptor.descriptor_version = source->descriptor_version;
            descriptor.flags = source->flags;
            descriptor.module = (CastleModule)(ULONG_PTR)candidates[index].module;
            descriptor.plugin_id = source->plugin_id;
            descriptor.display_name = source->display_name;
            descriptor.version_text = source->version_text;
            descriptor.build_id = source->build_id;
            result = runtime_api->RegisterPlugin(&descriptor, &handle);
            if (result < 0) {
                final_result = result;
                continue;
            }
        }

        if (mode == CASTLE_CLIENT_BOOTSTRAP_FAULT) {
            result = candidates[index].plugin_export->client_bootstrap(
                mode, NULL, 0u, fault_result);
        } else {
            result = candidates[index].plugin_export->client_bootstrap(
                mode, runtime_api, handle, CASTLE_OK);
        }
        if (result < 0) final_result = result;
    }
    return final_result;
}

CastleResult CASTLE_RUNTIME_CALL Client_BootstrapPlugin(
    CastleU32 mode,
    const CastleRuntimeApiV1* runtime_api,
    CastlePluginHandle plugin_handle,
    CastleResult reason) {
    const CastleRuntimeClientConfigV1* config;
    LONG state;
    CastleResult result;

    if (!g_client_export || !g_client_export->client_config) return CASTLE_ERROR_INVALID_ARGUMENT;
    config = g_client_export->client_config;

    for (;;) {
        state = InterlockedCompareExchange(&g_client_state, 0, 0);
        if (state == CASTLE_CLIENT_STANDALONE || state == CASTLE_CLIENT_INTEGRATED ||
            state == CASTLE_CLIENT_RUNTIME_FAULT || state == CASTLE_CLIENT_PLUGIN_FAILED) {
            return CASTLE_STATUS_ALREADY_DONE;
        }
        /*
         * BOOTSTRAPPING/JOINING 说明另一个线程已经夺得初始化所有权。
         * 当前线程只能等待终态，绝不能第二次调用插件业务初始化函数。
         */
        if (state == CASTLE_CLIENT_BOOTSTRAPPING || state == CASTLE_CLIENT_JOINING) {
            Sleep(0u);
            continue;
        }
        if (state != CASTLE_CLIENT_GATE_OWNER && state != CASTLE_CLIENT_GATE_FOLLOWER) {
            return CASTLE_ERROR_TRANSACTION_STATE;
        }
        if (InterlockedCompareExchange(&g_client_state,
                CASTLE_CLIENT_BOOTSTRAPPING, state) == state) break;
    }

    if (mode == CASTLE_CLIENT_BOOTSTRAP_INTEGRATED) {
        if (!runtime_api || plugin_handle == 0u || !config->integrated_initialize) {
            InterlockedExchange(&g_client_state, CASTLE_CLIENT_PLUGIN_FAILED);
            return CASTLE_ERROR_INVALID_ARGUMENT;
        }
        InterlockedExchange(&g_client_state, CASTLE_CLIENT_JOINING);
        result = config->integrated_initialize(runtime_api, plugin_handle,
                                               config->user_context);
        InterlockedExchange(&g_client_state,
                            result < 0 ? CASTLE_CLIENT_PLUGIN_FAILED :
                                         CASTLE_CLIENT_INTEGRATED);
        return result;
    }

    if (mode == CASTLE_CLIENT_BOOTSTRAP_STANDALONE) {
        if (!config->standalone_initialize) {
            InterlockedExchange(&g_client_state, CASTLE_CLIENT_PLUGIN_FAILED);
            return CASTLE_ERROR_INVALID_ARGUMENT;
        }
        result = config->standalone_initialize(config->user_context);
        InterlockedExchange(&g_client_state,
                            result < 0 ? CASTLE_CLIENT_PLUGIN_FAILED :
                                         CASTLE_CLIENT_STANDALONE);
        return result;
    }

    if (mode == CASTLE_CLIENT_BOOTSTRAP_FAULT) {
        if (config->runtime_fault) config->runtime_fault(reason,
                                                        config->user_context);
        InterlockedExchange(&g_client_state, CASTLE_CLIENT_RUNTIME_FAULT);
        return reason;
    }

    InterlockedExchange(&g_client_state, CASTLE_CLIENT_PLUGIN_FAILED);
    return CASTLE_ERROR_INVALID_ARGUMENT;
}

CastleResult CASTLE_RUNTIME_CALL CastleRuntimeClient_OnProcessAttach(
    CastleModule self_module,
    CastlePluginExportV1* plugin_export) {
    if (self_module == 0u || !plugin_export ||
        plugin_export->magic != CASTLE_PLUGIN_QUERY_MAGIC ||
        plugin_export->struct_size < CASTLE_SIZEOF_PLUGIN_EXPORT_V1 ||
        plugin_export->export_version != CASTLE_PLUGIN_EXPORT_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    g_client_module = (HMODULE)(ULONG_PTR)self_module;
    g_client_export = plugin_export;
    g_client_export->entry_gate_thunk =
        (CastleAddress)(ULONG_PTR)&CastleRuntimeClient_EntryGateThunk;
    g_client_export->client_bootstrap = &Client_BootstrapPlugin;
    InterlockedExchange(&g_client_state, CASTLE_CLIENT_COLD);
    return Client_InstallOrJoinEntryGate();
}

CastleResult CASTLE_RUNTIME_CALL CastleRuntimeClient_BootstrapAll(
    CastleU32 trigger_kind,
    CastleModule trigger_module) {
    WCHAR runtime_path[CLIENT_PATH_CAP];
    DWORD attributes;
    HMODULE runtime_module;
    FARPROC get_api_address;
    CastleRuntimeGetApiFn get_api;
    const CastleRuntimeApiV1* runtime_api;
    CastleBootstrapRequestV1 request;
    CastleBootstrapResultV1 result_value;
    CastleResult result;
    CastleResult restore_result;
    volatile BYTE* clear_bytes;
    CastleU32 clear_index;

    (void)trigger_module;
    restore_result = Client_RestoreKnownEntryGate();
    if (restore_result < 0) {
        return client_bootstrap_local_(CASTLE_CLIENT_BOOTSTRAP_FAULT, NULL,
                                       restore_result);
    }
    if (!client_build_runtime_path_(runtime_path, CLIENT_PATH_CAP)) {
        return client_bootstrap_local_(CASTLE_CLIENT_BOOTSTRAP_FAULT, NULL,
                                       CASTLE_ERROR_RUNTIME_FAULT);
    }

    attributes = GetFileAttributesW(runtime_path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
            return client_bootstrap_local_(CASTLE_CLIENT_BOOTSTRAP_STANDALONE,
                                           NULL, CASTLE_OK);
        }
        return client_bootstrap_local_(CASTLE_CLIENT_BOOTSTRAP_FAULT, NULL,
                                       CASTLE_ERROR_RUNTIME_FAULT);
    }
    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        return client_bootstrap_local_(CASTLE_CLIENT_BOOTSTRAP_FAULT, NULL,
                                       CASTLE_ERROR_RUNTIME_FAULT);
    }

    runtime_module = LoadLibraryW(runtime_path);
    if (!runtime_module) {
        return client_bootstrap_local_(CASTLE_CLIENT_BOOTSTRAP_FAULT, NULL,
                                       CASTLE_ERROR_RUNTIME_FAULT);
    }
    get_api_address = GetProcAddress(runtime_module, "CastleRuntime_GetApi");
    if (!get_api_address) {
        return client_bootstrap_local_(CASTLE_CLIENT_BOOTSTRAP_FAULT, NULL,
                                       CASTLE_ERROR_ABI_MISMATCH);
    }
    get_api = (CastleRuntimeGetApiFn)get_api_address;
    runtime_api = get_api(CASTLE_RUNTIME_ABI_V1);
    if (!runtime_api || runtime_api->magic != CASTLE_RUNTIME_API_MAGIC ||
        runtime_api->struct_size < CASTLE_SIZEOF_RUNTIME_API_V1 ||
        !runtime_api->BootstrapLoadedPlugins) {
        return client_bootstrap_local_(CASTLE_CLIENT_BOOTSTRAP_FAULT, NULL,
                                       CASTLE_ERROR_ABI_MISMATCH);
    }

    clear_bytes = (volatile BYTE*)&request;
    for (clear_index = 0u; clear_index < (CastleU32)sizeof(request); ++clear_index) {
        clear_bytes[clear_index] = 0u;
    }
    clear_bytes = (volatile BYTE*)&result_value;
    for (clear_index = 0u; clear_index < (CastleU32)sizeof(result_value); ++clear_index) {
        clear_bytes[clear_index] = 0u;
    }

    request.magic = CASTLE_BOOTSTRAP_REQUEST_MAGIC;
    request.struct_size = CASTLE_SIZEOF_BOOTSTRAP_REQUEST_V1;
    request.request_version = CASTLE_BOOTSTRAP_VERSION_1;
    request.trigger_kind = trigger_kind;
    request.trigger_module = (CastleModule)(ULONG_PTR)g_client_module;
    request.gate_entry = CASTLE_RPG_ENTRY_VA_V1;
    result_value.magic = CASTLE_BOOTSTRAP_RESULT_MAGIC;
    result_value.struct_size = CASTLE_SIZEOF_BOOTSTRAP_RESULT_V1;
    result_value.result_version = CASTLE_BOOTSTRAP_VERSION_1;

    result = runtime_api->BootstrapLoadedPlugins(&request, &result_value);
    return result;
}

CastleResult CASTLE_RUNTIME_CALL CastleRuntimeClient_RunNow(void) {
    return CastleRuntimeClient_BootstrapAll(
        CASTLE_BOOTSTRAP_TRIGGER_INITIALIZE_ASI,
        (CastleModule)(ULONG_PTR)g_client_module);
}

void CASTLE_RUNTIME_CALL CastleRuntimeClient_OnProcessDetach(void* reserved) {
    const CastleRuntimeClientConfigV1* config =
        g_client_export ? g_client_export->client_config : NULL;
    (void)reserved;
    InterlockedExchange(&g_client_state, CASTLE_CLIENT_PROCESS_EXIT);
    if (config && config->process_exit) config->process_exit(config->user_context);
}

CastleU32 CASTLE_RUNTIME_CALL CastleRuntimeClient_GetState(void) {
    return (CastleU32)InterlockedCompareExchange(&g_client_state, 0, 0);
}
