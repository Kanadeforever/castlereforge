#include "runtime_internal.h"

#define RUNTIME_WINDOW_MAX_CLIENTS 64u
#define RUNTIME_WINDOW_ID_CAP RUNTIME_PLUGIN_ID_CAP

typedef HWND (WINAPI *RuntimeGetForegroundWindowFn)(void);
typedef BOOL (WINAPI *RuntimeEnumWindowsFn)(WNDENUMPROC callback, LPARAM parameter);
typedef BOOL (WINAPI *RuntimeIsWindowFn)(HWND window);
typedef BOOL (WINAPI *RuntimeIsWindowVisibleFn)(HWND window);
typedef DWORD (WINAPI *RuntimeGetWindowThreadProcessIdFn)(HWND window,
                                                          LPDWORD process_id);
typedef LONG_PTR (WINAPI *RuntimeGetWindowLongPtrAFn)(HWND window, int index);
typedef LONG_PTR (WINAPI *RuntimeSetWindowLongPtrAFn)(HWND window, int index,
                                                      LONG_PTR value);
typedef LRESULT (WINAPI *RuntimeCallWindowProcAFn)(WNDPROC previous, HWND window,
                                                   UINT message, WPARAM w_param,
                                                   LPARAM l_param);
typedef LRESULT (WINAPI *RuntimeDefWindowProcAFn)(HWND window, UINT message,
                                                  WPARAM w_param, LPARAM l_param);

typedef struct RuntimeWindowClient {
    int used;
    CastleU32 generation;
    CastleLeaseHandle handle;
    CastlePluginHandle plugin;
    CastleU32 kind;
    CastleU32 phase;
    CastleU32 priority;
    CastleU32 ready;
    CastleWindowObserverFn observer;
    CastleWindowFilterFn filter;
    void* user_context;
    char plugin_id[RUNTIME_WINDOW_ID_CAP];
    CastleU32 plugin_id_length;
} RuntimeWindowClient;

typedef struct RuntimeWindowDispatchItem {
    CastleU32 kind;
    CastleU32 phase;
    CastleU32 priority;
    CastleLeaseHandle handle;
    CastleWindowObserverFn observer;
    CastleWindowFilterFn filter;
    void* user_context;
    char plugin_id[RUNTIME_WINDOW_ID_CAP];
    CastleU32 plugin_id_length;
} RuntimeWindowDispatchItem;

typedef struct RuntimeWindowSearch {
    HWND foreground;
    HWND first_visible;
    DWORD process_id;
} RuntimeWindowSearch;

static volatile LONG g_window_lock;
static RuntimeWindowClient g_window_clients[RUNTIME_WINDOW_MAX_CLIENTS];
static CastleU32 g_window_client_generation;
static HWND g_game_window;
static WNDPROC g_previous_wndproc;
static CastleU32 g_window_generation;
static DWORD g_window_thread_id;
static HMODULE g_user32_module;
static RuntimeGetForegroundWindowFn g_get_foreground_window;
static RuntimeEnumWindowsFn g_enum_windows;
static RuntimeIsWindowFn g_is_window;
static RuntimeIsWindowVisibleFn g_is_window_visible;
static RuntimeGetWindowThreadProcessIdFn g_get_window_thread_process_id;
static RuntimeGetWindowLongPtrAFn g_get_window_long_ptr_a;
static RuntimeSetWindowLongPtrAFn g_set_window_long_ptr_a;
static RuntimeCallWindowProcAFn g_call_window_proc_a;
static RuntimeDefWindowProcAFn g_def_window_proc_a;

static CastleResult CASTLE_RUNTIME_CALL window_get_game_(CastleWindowStateV1* out_state);
static CastleResult CASTLE_RUNTIME_CALL window_register_observer_(
    CastlePluginHandle plugin, const CastleWindowClientV1* client,
    CastleLeaseHandle* out_client);
static CastleResult CASTLE_RUNTIME_CALL window_register_filter_(
    CastlePluginHandle plugin, const CastleWindowClientV1* client,
    CastleLeaseHandle* out_client);
static CastleResult CASTLE_RUNTIME_CALL window_set_ready_(CastleLeaseHandle client,
                                                          CastleU32 ready);
static CastleResult CASTLE_RUNTIME_CALL window_unregister_(CastleLeaseHandle client);
static CastleResult CASTLE_RUNTIME_CALL window_get_generation_(CastleU32* out_generation);
static LRESULT CALLBACK window_master_proc_(HWND window, UINT message,
                                            WPARAM w_param, LPARAM l_param);

static const CastleWindowApiV1 g_window_api = {
    CASTLE_WINDOW_API_MAGIC,
    CASTLE_SIZEOF_WINDOW_API_V1,
    CASTLE_WINDOW_API_VERSION_1,
    CASTLE_WINDOW_CAP_OBSERVER | CASTLE_WINDOW_CAP_FILTER,
    window_get_game_,
    window_register_observer_,
    window_register_filter_,
    window_set_ready_,
    window_unregister_,
    window_get_generation_
};

static FARPROC window_resolve_(const char* name) {
    return g_user32_module ? GetProcAddress(g_user32_module, name) : NULL;
}

static int window_pointer_in_plugin_(CastlePluginHandle plugin, const void* pointer) {
    HMODULE module = Runtime_GetPluginModule(plugin);
    MEMORY_BASIC_INFORMATION information;
    if (!module || !pointer ||
        VirtualQuery(pointer, &information, sizeof(information)) == 0u) return 0;
    return information.State == MEM_COMMIT && information.AllocationBase == module;
}

static int window_copy_plugin_id_(CastlePluginHandle plugin, char* output,
                                  CastleU32* out_length) {
    CastleStringView id = Runtime_GetPluginIdView(plugin);
    CastleU32 index;
    if (!output || !out_length || !id.data || id.length == 0u ||
        id.length >= RUNTIME_WINDOW_ID_CAP) return 0;
    for (index = 0u; index < id.length; ++index) output[index] = id.data[index];
    output[id.length] = '\0';
    *out_length = id.length;
    return 1;
}

static int window_valid_client_(CastlePluginHandle plugin,
                                const CastleWindowClientV1* client,
                                CastleU32 kind) {
    if (!client || client->magic != CASTLE_WINDOW_CLIENT_MAGIC ||
        client->struct_size < CASTLE_SIZEOF_WINDOW_CLIENT_V1 ||
        client->version != CASTLE_WINDOW_STRUCTURE_VERSION_1 ||
        client->phase > CASTLE_WINDOW_PHASE_LATE ||
        client->priority > CASTLE_WINDOW_PRIORITY_LATE ||
        (client->label.length != 0u && !client->label.data)) return 0;
    if (kind == CASTLE_WINDOW_CLIENT_OBSERVER) {
        return client->observer && !client->filter &&
            window_pointer_in_plugin_(plugin, (const void*)client->observer);
    }
    return !client->observer && client->filter &&
        window_pointer_in_plugin_(plugin, (const void*)client->filter);
}

static CastleLeaseHandle window_make_handle_(CastleU32 index, CastleU32 generation) {
    return (generation << 8u) | (index + 1u);
}

static RuntimeWindowClient* window_resolve_locked_(CastleLeaseHandle handle) {
    CastleU32 encoded_index = handle & 0xFFu;
    CastleU32 generation = handle >> 8u;
    RuntimeWindowClient* client;
    if (encoded_index == 0u || encoded_index > RUNTIME_WINDOW_MAX_CLIENTS ||
        generation == 0u) return NULL;
    client = &g_window_clients[encoded_index - 1u];
    if (!client->used || client->generation != generation ||
        client->handle != handle) return NULL;
    return client;
}

static BOOL CALLBACK window_enum_callback_(HWND window, LPARAM parameter) {
    RuntimeWindowSearch* search = (RuntimeWindowSearch*)parameter;
    DWORD process_id = 0u;
    if (!search || !g_get_window_thread_process_id || !g_is_window_visible) return TRUE;
    g_get_window_thread_process_id(window, &process_id);
    if (process_id != search->process_id || !g_is_window_visible(window)) return TRUE;
    if (window == search->foreground) {
        search->first_visible = window;
        return FALSE;
    }
    if (!search->first_visible) search->first_visible = window;
    return TRUE;
}

/* 优先选择本进程前台窗口；没有时选择第一个可见顶层窗口。 */
static HWND window_find_candidate_(void) {
    RuntimeWindowSearch search;
    Runtime_ByteZero(&search, (CastleU32)sizeof(search));
    search.process_id = GetCurrentProcessId();
    search.foreground = g_get_foreground_window ? g_get_foreground_window() : NULL;
    if (!g_enum_windows || !g_enum_windows(window_enum_callback_, (LPARAM)&search)) {
        /* EnumWindows 因找到前台窗口而返回 FALSE 也属于成功结果。 */
    }
    return search.first_visible;
}

static CastleU32 window_next_generation_locked_(void) {
    ++g_window_generation;
    if (g_window_generation == 0u) ++g_window_generation;
    return g_window_generation;
}

/*
 * 刷新函数不调用插件代码。若其它组件后来包在 Runtime WndProc 前面，Runtime 不抢回
 * 链头；只要对方正常 CallWindowProc，分发仍然有效，也不会破坏其顺序。
 */
static CastleResult window_refresh_(void) {
    HWND candidate = window_find_candidate_();
    WNDPROC current;
    LONG_PTR previous;
    DWORD process_id = 0u;

    Runtime_Lock(&g_window_lock);
    if (g_game_window && g_is_window(g_game_window)) {
        if (!candidate || candidate == g_game_window) {
            Runtime_Unlock(&g_window_lock);
            return CASTLE_OK;
        }
        /* 老窗口仍有效时不跳到另一个辅助窗口，避免错误接管启动器或调试 UI。 */
        Runtime_Unlock(&g_window_lock);
        return CASTLE_OK;
    }
    if (g_game_window) {
        g_game_window = NULL;
        g_previous_wndproc = NULL;
        g_window_thread_id = 0u;
        window_next_generation_locked_();
    }
    if (!candidate || !g_is_window(candidate)) {
        Runtime_Unlock(&g_window_lock);
        return CASTLE_ERROR_NOT_READY;
    }
    g_get_window_thread_process_id(candidate, &process_id);
    if (process_id != GetCurrentProcessId()) {
        Runtime_Unlock(&g_window_lock);
        return CASTLE_ERROR_NOT_READY;
    }
    current = (WNDPROC)g_get_window_long_ptr_a(candidate, GWLP_WNDPROC);
    if (!current) {
        Runtime_Unlock(&g_window_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    SetLastError(ERROR_SUCCESS);
    previous = g_set_window_long_ptr_a(candidate, GWLP_WNDPROC,
                                       (LONG_PTR)window_master_proc_);
    if (previous == 0 && GetLastError() != ERROR_SUCCESS) {
        Runtime_Unlock(&g_window_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_game_window = candidate;
    g_previous_wndproc = (WNDPROC)previous;
    g_window_thread_id = g_get_window_thread_process_id(candidate, NULL);
    window_next_generation_locked_();
    Runtime_Unlock(&g_window_lock);
    Runtime_DiagnosticAppend("[Window] Runtime master WndProc installed.");
    return CASTLE_OK;
}

static int window_text_order_(const RuntimeWindowDispatchItem* left,
                              const RuntimeWindowDispatchItem* right) {
    CastleU32 index;
    CastleU32 shared;
    if (left->phase != right->phase) return left->phase < right->phase;
    if (left->priority != right->priority) return left->priority < right->priority;
    shared = left->plugin_id_length < right->plugin_id_length ?
        left->plugin_id_length : right->plugin_id_length;
    for (index = 0u; index < shared; ++index) {
        CastleU8 left_value = (CastleU8)left->plugin_id[index];
        CastleU8 right_value = (CastleU8)right->plugin_id[index];
        if (left_value != right_value) return left_value < right_value;
    }
    if (left->plugin_id_length != right->plugin_id_length) {
        return left->plugin_id_length < right->plugin_id_length;
    }
    return left->handle < right->handle;
}

static void window_sort_dispatch_(RuntimeWindowDispatchItem* items, CastleU32 count) {
    CastleU32 index;
    for (index = 1u; index < count; ++index) {
        RuntimeWindowDispatchItem value = items[index];
        CastleU32 position = index;
        while (position > 0u && window_text_order_(&value, &items[position - 1u])) {
            items[position] = items[position - 1u];
            --position;
        }
        items[position] = value;
    }
}

static CastleU32 window_collect_dispatch_(CastleU32 kind,
                                          RuntimeWindowDispatchItem* output,
                                          CastleU32 capacity,
                                          WNDPROC* out_previous,
                                          CastleU32* out_generation) {
    CastleU32 index;
    CastleU32 count = 0u;
    Runtime_Lock(&g_window_lock);
    *out_previous = g_previous_wndproc;
    *out_generation = g_window_generation;
    for (index = 0u; index < RUNTIME_WINDOW_MAX_CLIENTS && count < capacity; ++index) {
        RuntimeWindowClient* client = &g_window_clients[index];
        RuntimeWindowDispatchItem* item;
        if (!client->used || !client->ready || client->kind != kind) continue;
        item = &output[count++];
        Runtime_ByteZero(item, (CastleU32)sizeof(*item));
        item->kind = client->kind;
        item->phase = client->phase;
        item->priority = client->priority;
        item->handle = client->handle;
        item->observer = client->observer;
        item->filter = client->filter;
        item->user_context = client->user_context;
        Runtime_ByteCopy(item->plugin_id, client->plugin_id,
                         client->plugin_id_length + 1u);
        item->plugin_id_length = client->plugin_id_length;
    }
    Runtime_Unlock(&g_window_lock);
    window_sort_dispatch_(output, count);
    return count;
}

static LRESULT CALLBACK window_master_proc_(HWND window, UINT message,
                                            WPARAM w_param, LPARAM l_param) {
    RuntimeWindowDispatchItem observers[RUNTIME_WINDOW_MAX_CLIENTS];
    RuntimeWindowDispatchItem filters[RUNTIME_WINDOW_MAX_CLIENTS];
    CastleWindowMessageV1 public_message;
    WNDPROC previous = NULL;
    WNDPROC ignored_previous = NULL;
    CastleU32 generation = 0u;
    CastleU32 ignored_generation = 0u;
    CastleU32 observer_count;
    CastleU32 filter_count;
    CastleU32 index;
    int consumed = 0;
    LRESULT final_result = 0;
    int structural = message == WM_DESTROY || message == WM_NCDESTROY;

    observer_count = window_collect_dispatch_(CASTLE_WINDOW_CLIENT_OBSERVER,
        observers, RUNTIME_WINDOW_MAX_CLIENTS, &previous, &generation);
    filter_count = window_collect_dispatch_(CASTLE_WINDOW_CLIENT_FILTER,
        filters, RUNTIME_WINDOW_MAX_CLIENTS, &ignored_previous, &ignored_generation);
    (void)ignored_previous;
    (void)ignored_generation;

    Runtime_ByteZero(&public_message, (CastleU32)sizeof(public_message));
    public_message.magic = CASTLE_WINDOW_MESSAGE_MAGIC;
    public_message.struct_size = CASTLE_SIZEOF_WINDOW_MESSAGE_V1;
    public_message.version = CASTLE_WINDOW_STRUCTURE_VERSION_1;
    public_message.window = (CastleAddress)(ULONG_PTR)window;
    public_message.message = (CastleU32)message;
    public_message.w_param = (CastleAddress)(ULONG_PTR)w_param;
    public_message.l_param = (CastleAddress)(ULONG_PTR)l_param;
    public_message.window_generation = generation;

    for (index = 0u; index < observer_count; ++index) {
        observers[index].observer(&public_message, observers[index].user_context);
    }
    for (index = 0u; index < filter_count && !consumed; ++index) {
        CastleWindowFilterDecisionV1 decision;
        CastleResult result;
        Runtime_ByteZero(&decision, (CastleU32)sizeof(decision));
        decision.magic = CASTLE_WINDOW_DECISION_MAGIC;
        decision.struct_size = CASTLE_SIZEOF_WINDOW_DECISION_V1;
        decision.version = CASTLE_WINDOW_STRUCTURE_VERSION_1;
        result = filters[index].filter(&public_message, &decision,
                                       filters[index].user_context);
        if (result < 0 || decision.magic != CASTLE_WINDOW_DECISION_MAGIC ||
            decision.struct_size < CASTLE_SIZEOF_WINDOW_DECISION_V1 ||
            decision.version != CASTLE_WINDOW_STRUCTURE_VERSION_1 ||
            decision.consume > 1u) {
            Runtime_DiagnosticAppend("[Window] filter returned an invalid decision.");
            continue;
        }
        if (!structural && decision.consume) {
            consumed = 1;
            final_result = (LRESULT)decision.result;
        }
    }
    if (!consumed) {
        final_result = previous ? g_call_window_proc_a(previous, window, message,
            w_param, l_param) : g_def_window_proc_a(window, message, w_param, l_param);
    }

    if (message == WM_NCDESTROY) {
        Runtime_Lock(&g_window_lock);
        if (g_game_window == window && g_window_generation == generation) {
            g_game_window = NULL;
            g_previous_wndproc = NULL;
            g_window_thread_id = 0u;
            window_next_generation_locked_();
        }
        Runtime_Unlock(&g_window_lock);
    }
    return final_result;
}

int Runtime_WindowInitialize(void) {
    g_window_lock = 0;
    g_window_client_generation = 0u;
    g_game_window = NULL;
    g_previous_wndproc = NULL;
    g_window_generation = 1u;
    g_window_thread_id = 0u;
    Runtime_ByteZero(g_window_clients, (CastleU32)sizeof(g_window_clients));
    g_user32_module = LoadLibraryW(L"user32.dll");
    if (!g_user32_module) return 0;
    g_get_foreground_window = (RuntimeGetForegroundWindowFn)
        window_resolve_("GetForegroundWindow");
    g_enum_windows = (RuntimeEnumWindowsFn)window_resolve_("EnumWindows");
    g_is_window = (RuntimeIsWindowFn)window_resolve_("IsWindow");
    g_is_window_visible = (RuntimeIsWindowVisibleFn)window_resolve_("IsWindowVisible");
    g_get_window_thread_process_id = (RuntimeGetWindowThreadProcessIdFn)
        window_resolve_("GetWindowThreadProcessId");
    g_get_window_long_ptr_a = (RuntimeGetWindowLongPtrAFn)
        window_resolve_("GetWindowLongPtrA");
    if (!g_get_window_long_ptr_a) {
        /* 32 位 USER32 常只导出 GetWindowLongA，Ptr 名由 windows.h 宏映射。 */
        g_get_window_long_ptr_a = (RuntimeGetWindowLongPtrAFn)
            window_resolve_("GetWindowLongA");
    }
    g_set_window_long_ptr_a = (RuntimeSetWindowLongPtrAFn)
        window_resolve_("SetWindowLongPtrA");
    if (!g_set_window_long_ptr_a) {
        g_set_window_long_ptr_a = (RuntimeSetWindowLongPtrAFn)
            window_resolve_("SetWindowLongA");
    }
    g_call_window_proc_a = (RuntimeCallWindowProcAFn)window_resolve_("CallWindowProcA");
    g_def_window_proc_a = (RuntimeDefWindowProcAFn)window_resolve_("DefWindowProcA");
    return g_get_foreground_window && g_enum_windows && g_is_window &&
        g_is_window_visible && g_get_window_thread_process_id &&
        g_get_window_long_ptr_a && g_set_window_long_ptr_a &&
        g_call_window_proc_a && g_def_window_proc_a;
}

const CastleWindowApiV1* Runtime_GetWindowApiV1(void) {
    return &g_window_api;
}

static CastleResult CASTLE_RUNTIME_CALL window_get_game_(CastleWindowStateV1* out_state) {
    CastleResult refresh_result;
    CastleU32 index;
    if (!out_state || out_state->magic != CASTLE_WINDOW_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_WINDOW_STATE_V1 ||
        out_state->version != CASTLE_WINDOW_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    refresh_result = window_refresh_();
    Runtime_Lock(&g_window_lock);
    out_state->flags = 0u;
    out_state->window = (CastleAddress)(ULONG_PTR)g_game_window;
    out_state->generation = g_window_generation;
    out_state->ready = g_game_window && g_is_window(g_game_window) ? 1u : 0u;
    out_state->window_thread_id = g_window_thread_id;
    out_state->observer_count = 0u;
    out_state->filter_count = 0u;
    for (index = 0u; index < RUNTIME_WINDOW_MAX_CLIENTS; ++index) {
        if (!g_window_clients[index].used || !g_window_clients[index].ready) continue;
        if (g_window_clients[index].kind == CASTLE_WINDOW_CLIENT_OBSERVER) {
            ++out_state->observer_count;
        } else {
            ++out_state->filter_count;
        }
    }
    Runtime_Unlock(&g_window_lock);
    return out_state->ready ? CASTLE_OK : refresh_result;
}

static CastleResult window_register_client_(CastlePluginHandle plugin,
    const CastleWindowClientV1* descriptor, CastleLeaseHandle* out_client,
    CastleU32 kind) {
    RuntimeWindowClient* record = NULL;
    CastleU32 index;
    CastleU32 id_length = 0u;
    char plugin_id[RUNTIME_WINDOW_ID_CAP];
    if (!out_client || !window_valid_client_(plugin, descriptor, kind) ||
        !window_copy_plugin_id_(plugin, plugin_id, &id_length)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_window_lock);
    for (index = 0u; index < RUNTIME_WINDOW_MAX_CLIENTS; ++index) {
        if (!g_window_clients[index].used) {
            record = &g_window_clients[index];
            break;
        }
    }
    if (!record) {
        Runtime_Unlock(&g_window_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_window_client_generation = (g_window_client_generation + 1u) & 0x00FFFFFFu;
    if (g_window_client_generation == 0u) g_window_client_generation = 1u;
    Runtime_ByteZero(record, (CastleU32)sizeof(*record));
    record->used = 1;
    record->generation = g_window_client_generation;
    record->handle = window_make_handle_(index, record->generation);
    record->plugin = plugin;
    record->kind = kind;
    record->phase = descriptor->phase;
    record->priority = descriptor->priority;
    record->observer = descriptor->observer;
    record->filter = descriptor->filter;
    record->user_context = descriptor->user_context;
    Runtime_ByteCopy(record->plugin_id, plugin_id, id_length + 1u);
    record->plugin_id_length = id_length;
    *out_client = record->handle;
    Runtime_Unlock(&g_window_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL window_register_observer_(
    CastlePluginHandle plugin, const CastleWindowClientV1* client,
    CastleLeaseHandle* out_client) {
    return window_register_client_(plugin, client, out_client,
                                   CASTLE_WINDOW_CLIENT_OBSERVER);
}

static CastleResult CASTLE_RUNTIME_CALL window_register_filter_(
    CastlePluginHandle plugin, const CastleWindowClientV1* client,
    CastleLeaseHandle* out_client) {
    return window_register_client_(plugin, client, out_client,
                                   CASTLE_WINDOW_CLIENT_FILTER);
}

static CastleResult CASTLE_RUNTIME_CALL window_set_ready_(CastleLeaseHandle handle,
                                                          CastleU32 ready) {
    RuntimeWindowClient* client;
    CastleResult refresh_result = CASTLE_OK;
    if (ready > 1u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_window_lock);
    client = window_resolve_locked_(handle);
    if (!client) {
        Runtime_Unlock(&g_window_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    client->ready = ready;
    Runtime_Unlock(&g_window_lock);
    if (ready) refresh_result = window_refresh_();
    return refresh_result;
}

static CastleResult CASTLE_RUNTIME_CALL window_unregister_(CastleLeaseHandle handle) {
    RuntimeWindowClient* client;
    Runtime_Lock(&g_window_lock);
    client = window_resolve_locked_(handle);
    if (!client) {
        Runtime_Unlock(&g_window_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_ByteZero(client, (CastleU32)sizeof(*client));
    Runtime_Unlock(&g_window_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL window_get_generation_(CastleU32* out_generation) {
    if (!out_generation) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_window_lock);
    *out_generation = g_window_generation;
    Runtime_Unlock(&g_window_lock);
    return CASTLE_OK;
}
