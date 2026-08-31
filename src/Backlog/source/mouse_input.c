#include "mouse_input.h"
#include "backlog.h"
#include "runtime.h"
#include "CastleRuntime_API.h"
#include "CastleWindow_API.h"

/*
 * mouse_input.c
 *
 * 鼠标滚轮没有像键盘那样可以通过 GetAsyncKeyState 查询“滚了几格”。
 * Windows 会把滚轮作为 WM_MOUSEWHEEL 消息送进窗口过程，因此最可靠的办法是
 * 链式观察 RPG.exe 自己的 WndProc。
 *
 * 这里没有安装全局低级鼠标 Hook，也没有新线程：
 * - 消息仍在游戏原来的 UI 线程处理；
 * - 我们只把“发生过几次”放进 LONG 计数器；
 * - worker 再通过 InterlockedExchange 一次取走。
 *
 * 这样既不需要消息泵，也不会把鼠标逻辑跨线程地直接写进游戏对象。
 */

static HWND g_game_window;
static WNDPROC g_previous_wndproc;
static volatile LONG g_wheel_steps;
static volatile LONG g_right_pressed;
static u32 g_retry_ticks;
static const CastleWindowApiV1* g_runtime_window_api;
static CastleLeaseHandle g_runtime_observer;
static CastleLeaseHandle g_runtime_filter;
static int g_runtime_window_mode;

static CastleStringView mouse_sdk_view(const char* text, CastleU32 length) {
    CastleStringView view;
    view.data = text;
    view.length = length;
    return view;
}

static void CASTLE_RUNTIME_CALL MouseInput_RuntimeObserver(
    const CastleWindowMessageV1* message, void* user_context) {
    (void)user_context;
    if (!message) return;
    if (message->message == WM_MOUSEWHEEL) {
        SHORT delta = (SHORT)((message->w_param >> 16u) & 0xFFFFu);
        if (delta > 0) InterlockedIncrement(&g_wheel_steps);
        else if (delta < 0) InterlockedDecrement(&g_wheel_steps);
    } else if (message->message == WM_RBUTTONDOWN) {
        InterlockedIncrement(&g_right_pressed);
    }
}

static CastleResult CASTLE_RUNTIME_CALL MouseInput_RuntimeFilter(
    const CastleWindowMessageV1* message,
    CastleWindowFilterDecisionV1* decision,
    void* user_context) {
    (void)user_context;
    if (!message || !decision) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (Backlog_IsActive() &&
        (message->message == WM_MOUSEWHEEL ||
         message->message == WM_RBUTTONDOWN ||
         message->message == WM_RBUTTONUP)) {
        decision->consume = 1u;
        decision->result = 0;
    }
    return CASTLE_OK;
}

/*
 * 判断一个窗口是不是当前 RPG.exe 自己的窗口。
 * 不能只比较 HWND 是否非空，因为桌面、Mod Loader 或其它程序也可能成为前台窗口。
 */
static int mouse_window_belongs_to_game(HWND window) {
    DWORD process_id = 0u;
    if (!window || !IsWindow(window)) return 0;
    GetWindowThreadProcessId(window, &process_id);
    return process_id == GetCurrentProcessId();
}

/*
 * 我们自己的窗口过程。
 *
 * WM_MOUSEWHEEL：
 *   HIWORD(wParam) 是一个有符号 16 位 delta；正常滚轮一格通常是 ±120。
 *   高精度滚轮可能一次给更小数值。Backlog 只需要“方向”，所以每条非零消息算一步。
 *
 * WM_RBUTTONDOWN：
 *   只记按下沿。Windows 自动重复不会像键盘那样不断产生新的 RBUTTONDOWN。
 *
 * 当 Backlog 已经打开时，滚轮和右键就是 Backlog 的专用操作，直接返回 0，
 * 不再让原版/其它鼠标业务同时收到同一条消息，避免一次右键既关闭历史又点击到底层界面。
 */
static LRESULT CALLBACK MouseInput_WndProc(HWND window, UINT message,
                                            WPARAM w_param, LPARAM l_param) {
    if (message == WM_MOUSEWHEEL) {
        SHORT delta = (SHORT)HIWORD(w_param);
        if (delta > 0) InterlockedIncrement(&g_wheel_steps);
        else if (delta < 0) InterlockedDecrement(&g_wheel_steps);

        if (Backlog_IsActive()) return 0;
    } else if (message == WM_RBUTTONDOWN) {
        InterlockedIncrement(&g_right_pressed);
        if (Backlog_IsActive()) return 0;
    } else if (message == WM_RBUTTONUP) {
        /*
         * Backlog 打开时，按下消息已经被我们消费。
         * 松开也一起消费，避免底层界面只看到一个“凭空出现的松开”。
         */
        if (Backlog_IsActive()) return 0;
    }

    if (g_previous_wndproc) {
        return CallWindowProcA(g_previous_wndproc, window, message, w_param, l_param);
    }
    return DefWindowProcA(window, message, w_param, l_param);
}

/*
 * 真正安装链式 WndProc。
 *
 * SetWindowLongPtrA 返回的是“安装前的 WndProc”。我们必须保存它，之后所有未消费消息
 * 都继续调用它。这样如果 cnc-ddraw 或别的插件已经先挂了 WndProc，我们会排在它前面，
 * 而不是把它覆盖掉。
 */
static int mouse_try_install(void) {
    HWND window;
    LONG_PTR previous;

    if (g_game_window && IsWindow(g_game_window) && g_previous_wndproc) return 1;

    window = GetForegroundWindow();
    if (!mouse_window_belongs_to_game(window)) return 0;

    SetLastError(0u);
    previous = SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)MouseInput_WndProc);
    if (previous == 0 && GetLastError() != 0u) return 0;

    g_game_window = window;
    g_previous_wndproc = (WNDPROC)previous;
    Runtime_Log("[鼠标] 已链式接入 RPG.exe 窗口过程：滚轮和右键输入可用。");
    return 1;
}

int MouseInput_Initialize(void) {
    g_game_window = NULL;
    g_previous_wndproc = NULL;
    g_wheel_steps = 0;
    g_right_pressed = 0;
    g_retry_ticks = 0u;
    g_runtime_window_api = NULL;
    g_runtime_observer = 0u;
    g_runtime_filter = 0u;
    g_runtime_window_mode = 0;

    /*
     * 初始化时窗口可能还没成为前台，所以第一次失败不是错误。
     * MouseInput_Poll 会继续低频重试。
     */
    return mouse_try_install();
}

int MouseInput_InitializeIntegrated(const CastleRuntimeApiV1* runtime_api,
                                    CastlePluginHandle plugin_handle) {
    static const char interface_id[] = CASTLE_WINDOW_INTERFACE_ID;
    static const char observer_label[] = "Backlog mouse observer";
    static const char filter_label[] = "Backlog active mouse filter";
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 query_result = {0};
    CastleWindowClientV1 client = {0};
    CastleResult ready_result;
    g_game_window = NULL;
    g_previous_wndproc = NULL;
    g_wheel_steps = 0;
    g_right_pressed = 0;
    g_retry_ticks = 0u;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = mouse_sdk_view(interface_id,
        (CastleU32)(sizeof(interface_id) - 1u));
    query.requested_version = CASTLE_WINDOW_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_WINDOW_API_V1;
    query.required_capabilities_low = CASTLE_WINDOW_CAP_OBSERVER |
                                      CASTLE_WINDOW_CAP_FILTER;
    query_result.magic = CASTLE_INTERFACE_API_MAGIC;
    query_result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    query_result.result_version = CASTLE_QUERY_VERSION_1;
    if (!runtime_api || runtime_api->QueryInterface(&query, &query_result) != CASTLE_OK) {
        return 0;
    }
    g_runtime_window_api = (const CastleWindowApiV1*)query_result.api_pointer;
    client.magic = CASTLE_WINDOW_CLIENT_MAGIC;
    client.struct_size = CASTLE_SIZEOF_WINDOW_CLIENT_V1;
    client.version = CASTLE_WINDOW_STRUCTURE_VERSION_1;
    client.phase = CASTLE_WINDOW_PHASE_NORMAL;
    client.priority = CASTLE_WINDOW_PRIORITY_DEFAULT;
    client.observer = MouseInput_RuntimeObserver;
    client.label = mouse_sdk_view(observer_label,
        (CastleU32)(sizeof(observer_label) - 1u));
    if (g_runtime_window_api->RegisterMessageObserver(plugin_handle, &client,
            &g_runtime_observer) != CASTLE_OK) return 0;
    client.observer = NULL;
    client.filter = MouseInput_RuntimeFilter;
    client.label = mouse_sdk_view(filter_label,
        (CastleU32)(sizeof(filter_label) - 1u));
    if (g_runtime_window_api->RegisterMessageFilter(plugin_handle, &client,
            &g_runtime_filter) != CASTLE_OK) return 0;
    ready_result = g_runtime_window_api->SetWindowClientReady(g_runtime_observer, 1u);
    if (ready_result < 0 && ready_result != CASTLE_ERROR_NOT_READY) return 0;
    ready_result = g_runtime_window_api->SetWindowClientReady(g_runtime_filter, 1u);
    if (ready_result < 0 && ready_result != CASTLE_ERROR_NOT_READY) return 0;
    g_runtime_window_mode = 1;
    Runtime_Log("[鼠标] 已登记 Runtime Window Observer/Filter；窗口稍后出现时自动接入。");
    return 1;
}

void MouseInput_Poll(void) {
    if (g_runtime_window_mode) {
        ++g_retry_ticks;
        if ((g_retry_ticks & 63u) == 0u && g_runtime_window_api) {
            CastleWindowStateV1 state = {0};
            state.magic = CASTLE_WINDOW_STATE_MAGIC;
            state.struct_size = CASTLE_SIZEOF_WINDOW_STATE_V1;
            state.version = CASTLE_WINDOW_STRUCTURE_VERSION_1;
            g_runtime_window_api->GetGameWindow(&state);
        }
        return;
    }
    if (g_game_window && IsWindow(g_game_window) && g_previous_wndproc) return;

    /* 8ms worker 下每 64 tick 约 0.5 秒重试一次，不需要每帧调用 SetWindowLongPtr。 */
    ++g_retry_ticks;
    if ((g_retry_ticks & 63u) == 0u) mouse_try_install();
}

i32 MouseInput_TakeWheelSteps(void) {
    return (i32)InterlockedExchange(&g_wheel_steps, 0);
}

u32 MouseInput_TakeRightPressed(void) {
    LONG value = InterlockedExchange(&g_right_pressed, 0);
    if (value <= 0) return 0u;
    return (u32)value;
}

void MouseInput_Shutdown(void) {
    if (g_runtime_window_mode && g_runtime_window_api) {
        if (g_runtime_observer) {
            g_runtime_window_api->SetWindowClientReady(g_runtime_observer, 0u);
            g_runtime_window_api->UnregisterWindowClient(g_runtime_observer);
        }
        if (g_runtime_filter) {
            g_runtime_window_api->SetWindowClientReady(g_runtime_filter, 0u);
            g_runtime_window_api->UnregisterWindowClient(g_runtime_filter);
        }
        g_runtime_observer = 0u;
        g_runtime_filter = 0u;
        g_runtime_window_mode = 0;
        g_runtime_window_api = NULL;
        g_wheel_steps = 0;
        g_right_pressed = 0;
        return;
    }
    if (g_game_window && IsWindow(g_game_window) && g_previous_wndproc) {
        /*
         * 只有当前 WndProc 仍然是我们自己时才恢复。
         * 如果后来又有别的插件链在我们前面，强行恢复会把后装插件截断，所以宁可不动。
         */
        WNDPROC current = (WNDPROC)GetWindowLongPtrA(g_game_window, GWLP_WNDPROC);
        if (current == MouseInput_WndProc) {
            SetWindowLongPtrA(g_game_window, GWLP_WNDPROC, (LONG_PTR)g_previous_wndproc);
        }
    }

    g_game_window = NULL;
    g_previous_wndproc = NULL;
    g_wheel_steps = 0;
    g_right_pressed = 0;
}
