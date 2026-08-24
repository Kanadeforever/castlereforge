#include "platform.h"
#include "runtime.h"
#include "sdl_input.h"
#include "backlog.h"

/*
 * plugin.c
 *
 * 这是 ASI 的生命周期入口。它不保存历史文本，也不解释方向键；只负责：
 * - 记住 DLL 模块句柄；
 * - 启动一个很轻的 8ms 输入线程；
 * - 按 Runtime -> Backlog Hook -> 可选 SDL 的顺序初始化；
 * - 在进程退出时发出停止信号。
 *
 * DllMain 运行在 Windows Loader Lock 内，不能在这里读 INI、写日志、加载 SDL 或修改游戏。
 * 所以这些真正工作全部放进 PluginWorker。CreateThread 返回后立即关闭“句柄副本”，
 * 线程本身仍会继续运行，直到 g_worker_running 变成 0。
 */

static HMODULE g_plugin_module;
static volatile LONG g_worker_running;

static DWORD WINAPI PluginWorker(void* unused) {
    const RuntimeConfig* config;
    (void)unused;

    if (!Runtime_Initialize(g_plugin_module)) {
        InterlockedExchange(&g_worker_running, 0);
        return 0u;
    }
    config = Runtime_Config();
    if (!config->enabled) {
        InterlockedExchange(&g_worker_running, 0);
        return 0u;
    }

    if (!Backlog_Install()) {
        Runtime_Log("[致命] Backlog 场景更新链安装失败；输入线程不会继续运行。");
        InterlockedExchange(&g_worker_running, 0);
        return 0u;
    }

    /* SDL 初始化失败不是致命错误；SdlInput 会保留键盘并每 5 秒低频重试。 */
    SdlInput_Initialize();
    Runtime_Log("[启动] Backlog v0.2.0 已就绪：同屏四条原版对话框、无人物图；B/LB 打开，方向浏览，B/East 退出。");

    while (InterlockedCompareExchange(&g_worker_running, 1, 1) != 0) {
        SdlInput_Poll();
        Backlog_PollInput();
        Sleep(BACKLOG_WORKER_SLEEP_MS);
    }

    SdlInput_Shutdown();
    Backlog_Shutdown();
    Runtime_Log("[退出] Backlog 输入线程已停止。");
    return 0u;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    HANDLE thread;
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        g_plugin_module = (HMODULE)instance;
        DisableThreadLibraryCalls(instance);
        InterlockedExchange(&g_worker_running, 1);

        /*
         * 这里只创建线程，不等待线程，也不在 Loader Lock 中访问游戏资源。
         * CloseHandle 释放的是当前代码持有的内核句柄，不会终止已经创建的线程。
         */
        thread = CreateThread(NULL, 0u, PluginWorker, NULL, 0u, NULL);
        if (thread) CloseHandle(thread);
        else InterlockedExchange(&g_worker_running, 0);
    } else if (reason == DLL_PROCESS_DETACH) {
        /* 正常情况是 RPG.exe 整体结束；主动卸载时 worker 下一轮也会看到 0 并收尾。 */
        InterlockedExchange(&g_worker_running, 0);
    }
    return TRUE;
}
