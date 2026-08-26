#include "platform.h"
#include "runtime.h"
#include "widescreen.h"

/*
 * plugin.c
 *
 * ASI Loader 会把 .asi 当作普通 32 位 DLL 加载，所以入口仍然是标准 DllMain。
 * 本插件必须在 RPG.exe 第一次建立 Display / DirectDraw 资源之前安装 Hook，
 * 因此不能“等游戏跑几秒再补丁”；DLL_PROCESS_ATTACH 到来时就立即做预检和安装。
 *
 * 这里刻意保持非常短：危险的内存修改集中在 runtime.c，宽屏业务集中在 widescreen.c；v0.11 的16:9/21:9双模式、全对白统一侧区规则、模糊/纯黑样式切换、侧画消息隔离、Battle侧区和INI配置也只属于该业务层。
 * 这样以后排查崩溃时可以快速判断问题属于“补丁基础设施”还是“多 Camera 算法”。
 */
BOOL WINAPI DllMain(HMODULE module, DWORD reason, void* reserved) {
    /* Windows 会传 reserved；本插件不需要它，但显式转 void 可以避免编译器把“未使用参数”当警告。 */
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH_) {
        /* 第一步先取得 Win32 API 并打开日志；如果连基础设施都不能建立，就什么也不改。 */
        if (!Runtime_Initialize(module)) return TRUE;

        /*
         * 第二步做 fail-closed 总预检。
         * 任何一个关键机器码或 E8 CALL 原目标不匹配，都说明不是我们研究的精确 RPG.exe。
         */
        if (!Runtime_ExactBuildProtocolOk()) return TRUE;

        /* 只有总预检完全通过才进入业务安装。安装函数自己还会对每一个 CALL 再检查一次。 */
        Widescreen_Install();
        return TRUE;
    }

    if (reason == DLL_PROCESS_DETACH_) {
        /* 进程正常退出或 DLL 被卸载时关闭日志。游戏本身即将结束，不需要再尝试恢复代码字节。 */
        Runtime_Shutdown();
    }

    /* DllMain 返回 TRUE 表示“DLL 本身可以保持加载”，不代表宽屏预检一定通过；结果以日志为准。 */
    return TRUE;
}
