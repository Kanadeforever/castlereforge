#include "runtime_internal.h"

/*
 * Windows 只在 DLL 被映射/卸载时调用 DllMain。
 * 这里不能打开日志、枚举插件、加载依赖或安装 Hook；真正初始化由 CastleRuntime_GetApi 触发。
 */
HMODULE g_runtime_module;

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        g_runtime_module = (HMODULE)module;
        DisableThreadLibraryCalls(module);
    }

    return TRUE;
}
