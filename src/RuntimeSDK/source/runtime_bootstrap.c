#include "runtime_internal.h"

typedef struct RuntimeBootstrapCandidate {
    HMODULE module;
    const CastlePluginExportV1* plugin_export;
} RuntimeBootstrapCandidate;

static volatile LONG g_bootstrap_state;
static CastleBootstrapResultV1 g_last_bootstrap_result;

static int runtime_view_compare_(CastleStringView left, CastleStringView right) {
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

static int runtime_range_belongs_to_module_(const void* address, CastleU32 size,
                                            HMODULE module) {
    MEMORY_BASIC_INFORMATION information;
    ULONG_PTR start;
    ULONG_PTR end;
    ULONG_PTR region_end;
    if (!address || !module || size == 0u) return 0;
    if (VirtualQuery(address, &information, sizeof(information)) == 0u) return 0;
    start = (ULONG_PTR)address;
    end = start + size;
    region_end = (ULONG_PTR)information.BaseAddress + information.RegionSize;
    return end >= start && information.State == MEM_COMMIT &&
        information.AllocationBase == module && end <= region_end;
}

static int runtime_view_belongs_to_module_(CastleStringView view, HMODULE module,
                                           int allow_empty) {
    if (view.length == 0u) {
        return allow_empty && (!view.data ||
            runtime_range_belongs_to_module_(view.data, 1u, module));
    }
    return runtime_range_belongs_to_module_(view.data, view.length, module);
}

static int runtime_valid_export_(HMODULE module,
                                 const CastlePluginExportV1* plugin_export) {
    const CastlePluginDescriptorV1* descriptor;
    const CastleRuntimeClientConfigV1* config;

    if (!module || !runtime_range_belongs_to_module_(plugin_export,
            CASTLE_SIZEOF_PLUGIN_EXPORT_V1, module)) return 0;
    if (plugin_export->magic != CASTLE_PLUGIN_QUERY_MAGIC ||
        plugin_export->struct_size < CASTLE_SIZEOF_PLUGIN_EXPORT_V1 ||
        plugin_export->export_version != CASTLE_PLUGIN_EXPORT_VERSION_1 ||
        !plugin_export->plugin_descriptor || !plugin_export->client_config ||
        !plugin_export->client_bootstrap) return 0;

    descriptor = plugin_export->plugin_descriptor;
    config = plugin_export->client_config;
    if (!runtime_range_belongs_to_module_(descriptor,
            CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1, module) ||
        !runtime_range_belongs_to_module_(config,
            CASTLE_SIZEOF_CLIENT_CONFIG_V1, module) ||
        descriptor->magic != CASTLE_PLUGIN_DESC_MAGIC ||
        descriptor->struct_size < CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1 ||
        descriptor->descriptor_version != CASTLE_PLUGIN_DESCRIPTOR_V1 ||
        config->magic != CASTLE_CLIENT_CONFIG_MAGIC ||
        config->struct_size < CASTLE_SIZEOF_CLIENT_CONFIG_V1 ||
        config->config_version != CASTLE_CLIENT_CONFIG_VERSION_1) return 0;

    if (!runtime_view_belongs_to_module_(descriptor->plugin_id, module, 0) ||
        !runtime_view_belongs_to_module_(descriptor->display_name, module, 1) ||
        !runtime_view_belongs_to_module_(descriptor->version_text, module, 1) ||
        !runtime_view_belongs_to_module_(descriptor->build_id, module, 1) ||
        !runtime_range_belongs_to_module_((const void*)plugin_export->client_bootstrap,
                                          1u, module)) return 0;
    if (plugin_export->entry_gate_thunk != 0u &&
        !runtime_range_belongs_to_module_(
            (const void*)(ULONG_PTR)plugin_export->entry_gate_thunk, 1u, module)) return 0;
    return 1;
}

static void runtime_sort_candidates_(RuntimeBootstrapCandidate* candidates,
                                     CastleU32 count) {
    CastleU32 outer;
    for (outer = 1u; outer < count; ++outer) {
        RuntimeBootstrapCandidate moving = candidates[outer];
        CastleU32 position = outer;
        while (position > 0u &&
               runtime_view_compare_(moving.plugin_export->plugin_descriptor->plugin_id,
                                     candidates[position - 1u].plugin_export->plugin_descriptor->plugin_id) < 0) {
            candidates[position] = candidates[position - 1u];
            --position;
        }
        candidates[position] = moving;
    }
}

static CastleResult runtime_collect_candidates_(RuntimeBootstrapCandidate* candidates,
                                                CastleU32 capacity,
                                                CastleU32* out_count) {
    HANDLE snapshot;
    MODULEENTRY32W entry;
    CastleU32 count = 0u;

    if (!candidates || !out_count || capacity == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_count = 0u;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
                                        GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE) return CASTLE_ERROR_RUNTIME_FAULT;

    Runtime_ByteZero(&entry, (CastleU32)sizeof(entry));
    entry.dwSize = sizeof(entry);
    if (Module32FirstW(snapshot, &entry)) {
        do {
            FARPROC query_address = GetProcAddress(entry.hModule, "CastlePlugin_Query");
            if (query_address) {
                CastlePluginQueryFn query = (CastlePluginQueryFn)query_address;
                const CastlePluginExportV1* plugin_export = query(CASTLE_PLUGIN_EXPORT_VERSION_1);
                if (runtime_valid_export_(entry.hModule, plugin_export)) {
                    if (count >= capacity) {
                        CloseHandle(snapshot);
                        return CASTLE_ERROR_RESOURCE_CONFLICT;
                    }
                    candidates[count].module = entry.hModule;
                    candidates[count].plugin_export = plugin_export;
                    ++count;
                } else {
                    Runtime_DiagnosticAppend("[Bootstrap] invalid CastlePlugin_Query export skipped.");
                }
            }
        } while (Module32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    runtime_sort_candidates_(candidates, count);
    *out_count = count;
    return CASTLE_OK;
}

static void runtime_copy_bootstrap_result_(CastleBootstrapResultV1* output) {
    output->flags = g_last_bootstrap_result.flags;
    output->mode = g_last_bootstrap_result.mode;
    output->total_plugins = g_last_bootstrap_result.total_plugins;
    output->active_plugins = g_last_bootstrap_result.active_plugins;
    output->degraded_plugins = g_last_bootstrap_result.degraded_plugins;
    output->failed_plugins = g_last_bootstrap_result.failed_plugins;
    output->bootstrap_generation = g_last_bootstrap_result.bootstrap_generation;
}

CastleResult Runtime_BootstrapLoadedPlugins(const CastleBootstrapRequestV1* request,
                                            CastleBootstrapResultV1* out_result) {
    RuntimeBootstrapCandidate candidates[RUNTIME_MAX_PLUGINS];
    CastleU32 candidate_count = 0u;
    CastleU32 index;
    LONG old_state;
    CastleResult collect_result;

    if (!request || !out_result ||
        request->magic != CASTLE_BOOTSTRAP_REQUEST_MAGIC ||
        request->struct_size < CASTLE_SIZEOF_BOOTSTRAP_REQUEST_V1 ||
        request->request_version != CASTLE_BOOTSTRAP_VERSION_1 ||
        out_result->magic != CASTLE_BOOTSTRAP_RESULT_MAGIC ||
        out_result->struct_size < CASTLE_SIZEOF_BOOTSTRAP_RESULT_V1 ||
        out_result->result_version != CASTLE_BOOTSTRAP_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    old_state = InterlockedCompareExchange(&g_bootstrap_state, 1, 0);
    if (old_state == 2) {
        /* ModLoader 的第二阶段已经做完；真实 Entry Gate 到达时才放行后台任务。 */
        if (request->trigger_kind == CASTLE_BOOTSTRAP_TRIGGER_ENTRY_GATE) {
            Runtime_ScheduleNotifyGameEntry();
        }
        runtime_copy_bootstrap_result_(out_result);
        return CASTLE_STATUS_ALREADY_DONE;
    }
    if (old_state != 0) return CASTLE_ERROR_TRANSACTION_STATE;

    /* 测试宿主不模拟 RPG 入口；真实两种启动来源都必须先关闭后台回调闸门。 */
    if (request->trigger_kind != CASTLE_BOOTSTRAP_TRIGGER_TEST_HOST) {
        Runtime_ScheduleCloseBootstrapGate();
    }

    Runtime_ByteZero(&g_last_bootstrap_result,
                     (CastleU32)sizeof(g_last_bootstrap_result));
    g_last_bootstrap_result.magic = CASTLE_BOOTSTRAP_RESULT_MAGIC;
    g_last_bootstrap_result.struct_size = CASTLE_SIZEOF_BOOTSTRAP_RESULT_V1;
    g_last_bootstrap_result.result_version = CASTLE_BOOTSTRAP_VERSION_1;
    g_last_bootstrap_result.mode = CASTLE_BOOTSTRAP_MODE_INTEGRATED;
    g_last_bootstrap_result.bootstrap_generation = 1u;

    collect_result = runtime_collect_candidates_(candidates, RUNTIME_MAX_PLUGINS,
                                                 &candidate_count);
    if (collect_result < 0) {
        g_last_bootstrap_result.failed_plugins = 1u;
        runtime_copy_bootstrap_result_(out_result);
        InterlockedExchange(&g_bootstrap_state, -1);
        return collect_result;
    }

    g_last_bootstrap_result.total_plugins = candidate_count;
    for (index = 0u; index < candidate_count; ++index) {
        const CastlePluginDescriptorV1* source =
            candidates[index].plugin_export->plugin_descriptor;
        CastlePluginDescriptorV1 descriptor;
        CastlePluginHandle handle = 0u;
        CastleResult register_result;
        CastleResult initialize_result;
        HMODULE pinned_module = NULL;

        descriptor.magic = source->magic;
        descriptor.struct_size = source->struct_size;
        descriptor.descriptor_version = source->descriptor_version;
        descriptor.flags = source->flags;
        descriptor.module = (CastleModule)(ULONG_PTR)candidates[index].module;
        descriptor.plugin_id = source->plugin_id;
        descriptor.display_name = source->display_name;
        descriptor.version_text = source->version_text;
        descriptor.build_id = source->build_id;

        register_result = Runtime_RegisterPlugin(&descriptor, &handle);
        if (register_result < 0) {
            ++g_last_bootstrap_result.failed_plugins;
            continue;
        }

        /*
         * client_bootstrap 会成为 Runtime 可达函数指针，所以先固定所属模块。
         * PIN 失败时不能继续发布一个可能在运行中卸载的函数地址。
         */
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_PIN,
                                (LPCWSTR)(const void*)candidates[index].plugin_export->client_bootstrap,
                                &pinned_module)) {
            Runtime_SetPluginState(handle, CASTLE_PLUGIN_FAILED,
                                   CASTLE_ERROR_RUNTIME_FAULT);
            ++g_last_bootstrap_result.failed_plugins;
            continue;
        }

        Runtime_SetPluginState(handle, CASTLE_PLUGIN_ACTIVATING, CASTLE_OK);
        initialize_result = candidates[index].plugin_export->client_bootstrap(
            CASTLE_CLIENT_BOOTSTRAP_INTEGRATED, Runtime_GetApiV1(), handle,
            CASTLE_OK);
        if (initialize_result < 0) {
            Runtime_SetPluginState(handle, CASTLE_PLUGIN_FAILED, initialize_result);
            ++g_last_bootstrap_result.failed_plugins;
        } else if (initialize_result > 0) {
            Runtime_SetPluginState(handle, CASTLE_PLUGIN_DEGRADED, initialize_result);
            ++g_last_bootstrap_result.degraded_plugins;
        } else {
            Runtime_SetPluginState(handle, CASTLE_PLUGIN_ACTIVE, CASTLE_OK);
            ++g_last_bootstrap_result.active_plugins;
        }
    }

    runtime_copy_bootstrap_result_(out_result);
    Runtime_DiagnosticAppend("[Bootstrap] loaded SDK plugins processed.");
    InterlockedExchange(&g_bootstrap_state, 2);
    if (request->trigger_kind == CASTLE_BOOTSTRAP_TRIGGER_ENTRY_GATE) {
        Runtime_ScheduleNotifyGameEntry();
    }
    return g_last_bootstrap_result.failed_plugins ?
        CASTLE_STATUS_OPTIONAL_UNAVAILABLE : CASTLE_OK;
}
