#include "platform.h"
#include "entry_gate.h"
#include "mod_loader.h"
#include "override_loader.h"
#include "locale_layer.h"
#include "native_locale.h"
#include "user32_locale.h"
#include "gdi_locale.h"
#include "game_audit.h"

/*
 * CastleModCore.dll —— 真正进入 RPG.exe 地址空间的底层运行时核心。
 *
 * 这版刻意遵守用户要求的“越早越好”，并参考 Locale Emulator 的启动模型把工作拆成两层：
 *
 * A. DLL_PROCESS_ATTACH（Windows Loader Lock 内，Core 作为新增第一项启动依赖）
 *    1. 安装一次性 RPG.exe Entry Gate；
 *    2. 立即读取外置 mods 配置并建立 Overrides；
 *    3. 读取 CastleLocaleBootstrap.dll 已经完成的 CP950 NLS 状态，再安装 zh-TW/注册表/UI语言/时区 Win32 Hook；
 *    4. 返回给 Windows，让 BASS/Bink/ijl10 等其余静态依赖继续初始化。
 *
 * B. RPG.exe 即将执行第一条业务指令（Loader Lock 已释放）
 *    1. 先恢复 RPG.exe 原入口；
 *    2. 再补挂一次此时已经完整映射的 BASS/Bink/ijl10 IAT；
 *    3. 最后才用绝对路径 LoadLibraryExW + LOAD_WITH_ALTERED_SEARCH_PATH 加载全部 ASI；
 *    4. 进入 RPG.exe 原始入口。
 *
 * 为什么 ASI 不放 A 阶段：
 *    ASI 是第三方代码，可能在自己的 DllMain 继续加载 DLL、初始化线程或做复杂工作。
 *    把它们放到 Loader Lock 外可以避免重复出现旧 UAL-Lite DllMain/worker 实验出现过的时序风险。
 *
 * 为什么 Overrides/Locale 反而必须放 A 阶段：
 *    它们是“目标进程出生时就应存在”的基础环境，不是普通插件。
 *    如果拖到 EntryPoint 才建立，某个静态依赖 DLL 的 DllMain 就有机会先读文件或先缓存错误代码页。
 */

static HMODULE g_self;
static LONG g_early_prepared;
static LONG g_initialized;
static LONG g_early_locale_ready;

/*
 * Launcher 注入 PE Import Descriptor 时必须指定一个真实导出符号，Windows 才会为这项 DLL 依赖建立 IAT。
 * 这里使用 __cdecl，确保 x86 导出表名称就是稳定的“CastleModCore_Bootstrap”，不出现 @0 装饰。
 */
DLL_EXPORT void __cdecl CastleModCore_Bootstrap(void) {
    /* 故意为空：真正初始化由 DllMain 的 Early Layer 和 Entry Gate 分阶段完成。 */
}

/*
 * Core 进入 DllMain 时，后面的静态依赖可能尚未全部完成映射/初始化。
 * 等到 RPG.exe EntryPoint 前，它们一定已经装好，所以再把相同 Hook 补挂一次。
 * PatchModule 本身是幂等的：已指向我们的 IAT 槽再次写同一地址，不改变语义。
 */
static void refresh_static_modules_(void) {
    HMODULE modules[6];
    UINT i;

    /*
     * dev6～dev8 曾把 ddraw.dll 从补挂列表中排除，理由是“第三方图形兼容层不属于游戏业务模块”。
     * 这个推理在静态上看似干净，但用户实机给出了更高优先级的反证：
     *   - dev5：台湾繁中区域环境正常，cnc-ddraw 自带截图正常；
     *   - dev6 起：区域环境表象失效，同时 cnc-ddraw 截图失效。
     *
     * 说明《幽城》+ cnc-ddraw 的实际窗口/输入链确实会经过 ddraw.dll 自己的 USER32/KERNEL32/GDI 导入。
     * 因此 dev9 不再把 ddraw 当成“完全隔离的黑盒”，而是恢复 dev5 已实机成功的兼容边界：
     * 只对它的【导入表】安装本项目既有 Locale/Overrides 桥，不加载、不卸载、不替换 DirectDraw 接口本身。
     *
     * 这里仍然没有任何 DirectDraw COM Hook、DirectDrawCreate Hook 或 SetDisplayMode Hook。
     * 我们恢复的只是 dev5 已证明必要的 Win32 API 兼容桥。
     */
    modules[0] = GetModuleHandleW(NULL_PTR);
    modules[1] = GetModuleHandleW((const WCHAR*)L"BASS.dll");
    modules[2] = GetModuleHandleW((const WCHAR*)L"binkw32.dll");
    modules[3] = GetModuleHandleW((const WCHAR*)L"ijl10.dll");
    modules[4] = GetModuleHandleW((const WCHAR*)L"ddraw.dll");
    modules[5] = GetModuleHandleW((const WCHAR*)L"SHLWAPI.dll");

    for (i = 0u; i < 6u; ++i) {
        if (!modules[i]) continue;
        OverrideLoader_PatchModule(modules[i]);
        LocaleLayer_PatchModule(modules[i]);
        User32Locale_PatchModule(modules[i]);
        GdiLocale_PatchModule(modules[i]);
    }
}

DLL_EXPORT int __cdecl CastleModCore_Initialize(void) {
    if (g_initialized) return g_initialized > 0;
    if (!g_early_prepared) return 0;
    if (!g_early_locale_ready) {
        ModLoader_Log((const WCHAR*)L"[致命] Locale 早期 Bootstrap/Win32 层未通过，拒绝进入 RPG.exe 业务代码。请查看前面的 Locale 状态码。");
        return 0;
    }

    /* 第一步必须恢复原 EXE 入口，之后任何 ASI 做机器码预检都只能看到原始 RPG.exe。 */
    if (!EntryGate_Restore()) {
        g_initialized = -1;
        return 0;
    }

    /*
     * 重新执行 Locale 初始化有两个目的：
     *   1. 此时 SHLWAPI/Bink/BASS/ijl10 已完整装入，可以解析真实 API 并补挂它们的 IAT；
     *   2. 再次确认当前主线程 LCID/UI Language 仍是 0x0404。
     * 这不是“晚才建立 Locale”，Early Layer 已经在 DllMain 先建立过一次。
     */
    /*
     * v0.2.6 起把 Locale Emulator 在用户指定设置下真正需要的层次分开确认：
     *   NativeLocale：ntdll 默认 Locale/UI/时区 + TEB.CurrentLocale；
     *   LocaleLayer：CP950/Win32 Locale/注册表/UI Language；
     *   User32Locale：ANSI 窗口文字 A->W 桥；
     *   GdiLocale：DEFAULT/ANSI_CHARSET -> CHINESEBIG5_CHARSET。
     * 任意一层失败都不允许在“半转区”状态进入 RPG.exe。
     */
    if (!NativeLocale_Refresh() || !LocaleLayer_Initialize() || !User32Locale_Initialize() || !GdiLocale_Initialize()) {
        ModLoader_Log((const WCHAR*)L"[致命] EntryPoint 前 Locale Emulator 等价语义二次确认失败，停止启动。");
        g_initialized = -1;
        return 0;
    }
    refresh_static_modules_();

    /*
     * v0.2.8 不再只用“标题栏正常 / GetACP=950”作为转区验收。
     * Loader Lock 已经释放，现在可以安全创建一个带繁体中文名称的一次性文件，
     * 再用真实 CreateFileA + Big5 原始字节重新打开它，直接验证游戏最依赖的 ANSI 文件名链。
     */
    if (!LocaleLayer_RunFileNameSelfTest()) {
        ModLoader_Log((const WCHAR*)L"[致命] CP950 文件名实体自检失败；拒绝进入 RPG.exe，避免繁体资源文件在半转区状态下随机读取失败。");
        g_initialized = -1;
        return 0;
    }

    /*
     * v0.2.8 额外记录 Locale Emulator 等价 CPTABLEINFO 修复层到底有没有真正命中。
     * 某些 Windows 版本在 Bootstrap 后可能已经自然刷新全部表，此时计数为 0 也不算失败；
     * 但在 v0.2.7 已复现“GetACP=950、CreateFileA 仍失败”的机器上，我们预期这里至少为 1。
     * 最终硬标准仍然是上面的实体文件名自检 PASS，而不是这个诊断数字。
     */
    {
        LONG repairs = NativeLocale_GetCustomCpRepairCount();
        WCHAR number[16];
        WCHAR temp[16];
        UINT n = 0u, i;
        unsigned long value = repairs > 0 ? (unsigned long)repairs : 0u;
        do { temp[n++] = (WCHAR)('0' + (value % 10u)); value /= 10u; } while (value && n < 15u);
        for (i = 0u; i < n; ++i) number[i] = temp[n - 1u - i];
        number[n] = 0;
        ModLoader_LogTwo((const WCHAR*)L"[Locale/Native] 本次启动已修复的陈旧 CPTABLEINFO 次数=", number);
    }

    /*
     * v0.2.9 起把“Loader 日志”和“原版游戏日志”彻底分流；v0.2.11 再把是否启用 game.log
     * 交给 Loader 自己的 CastleModLoader.ini。GameLog=0 时必须连审计 Hook 一起关闭，不能只是“不写文件”。
     */
    if (ModLoader_IsGameLogEnabled()) {
        if (!GameAudit_Initialize()) {
            ModLoader_Log((const WCHAR*)L"[致命] GameLog=1，但无法建立 mods\\game.log 原版游戏审计层；为避免用户误以为本轮已经留证，停止进入游戏。");
            g_initialized = -1;
            return 0;
        }
        {
            UINT path_slots = OverrideLoader_EnableGameAudit();
            UINT io_slots = GameAudit_PatchOriginalIoModules();
            UINT state_hooks = GameAudit_StateHookCount();
            WCHAR number[16], temp[16];
            UINT n, value, i;

            /*
             * 下面三组代码只把整数计数转换成十进制文字，没有使用 printf/CRT。
             * 做法都一样：先用“除以 10 取余数”从个位往高位写进 temp，所以 temp 里的数字是反着的；
             * 再用第二个循环把它倒回来放进 number，最后补一个字符串结尾 0。
             * 这些数字只说明审计基础设施实际安装了多少槽位/断点，详细游戏行为不会写进 modloader.log。
             */
            n = 0u;
            value = path_slots;
            do { temp[n++] = (WCHAR)('0' + (value % 10u)); value /= 10u; } while (value && n < 15u);
            for (i = 0u; i < n; ++i) number[i] = temp[n - 1u - i];
            number[n] = 0;
            ModLoader_LogTwo((const WCHAR*)L"[游戏审计] CreateFile/GetFileAttributes IAT 槽位数量=", number);

            n = 0u;
            value = io_slots;
            do { temp[n++] = (WCHAR)('0' + (value % 10u)); value /= 10u; } while (value && n < 15u);
            for (i = 0u; i < n; ++i) number[i] = temp[n - 1u - i];
            number[n] = 0;
            ModLoader_LogTwo((const WCHAR*)L"[游戏审计] 其余原版 I/O IAT 槽位数量=", number);

            n = 0u;
            value = state_hooks;
            do { temp[n++] = (WCHAR)('0' + (value % 10u)); value /= 10u; } while (value && n < 15u);
            for (i = 0u; i < n; ++i) number[i] = temp[n - 1u - i];
            number[n] = 0;
            ModLoader_LogTwo((const WCHAR*)L"[游戏审计] 已确认生命周期状态断点数量=", number);
            ModLoader_Log((const WCHAR*)L"[游戏审计] GameLog=1：已启用独立 mods\\game.log；原版 I/O、状态与严重异常只写入该日志。");
        }
    } else {
        /*
         * 这里什么审计函数都不调用，因此不会创建 game.log，不会让 OverrideLoader 打开 audit 旁路，
         * 也不会安装 ReadFile/WriteFile 等 IAT Hook、13 个状态 JMP 或 VEH。关闭开关后是真正的零审计路径。
         */
        ModLoader_Log((const WCHAR*)L"[游戏审计] GameLog=0：本轮不建立 game.log，也不安装仅用于原版游戏审计的 I/O/状态/异常 Hook。");
    }

    /* Windows 已经消费完 Launcher 添加的新 Import Directory；把临时可写 .rdata 页恢复成只读。 */
    EntryGate_FinalizeInjectedImportPage();

    ModLoader_Log((const WCHAR*)L"[启动阶段] 最后加载 mods\\asi 中启用的代码 Mod。");
    ModLoader_LoadAsi();
    ModLoader_Log((const WCHAR*)L"[完成] Pre-Loader 全部前置工作已完成，现在进入 RPG.exe 原始入口。");

    g_initialized = 1;
    return 1;
}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH_) {
        g_self = module;
        DisableThreadLibraryCalls(module);

        /* 精确入口不匹配就 fail-closed，绝不向未知 RPG.exe 写 JMP。 */
        if (!EntryGate_Install()) return FALSE_;

        /*
         * Launcher 已经第一时间清空 mods\\modloader.log；Core 在这里以追加方式接管同一份日志。
         * 下面的顺序就是项目最高优先级：Overrides 比 Locale、ASI 和游戏业务都更早。
         */
        if (!ModLoader_Begin(g_self)) return FALSE_;
        ModLoader_Log((const WCHAR*)L"[Early 1/2] 建立 Overrides 文件虚拟层。此阶段早于其余普通 Mod 与 RPG.exe 业务代码。");
        if (!ModLoader_PrepareOverrides()) return FALSE_;

        ModLoader_Log((const WCHAR*)L"[Early 2/2] 建立 Locale Emulator 等价台湾环境：Native Locale → CP950 Win32 → USER32/GDI 桥。");
        g_early_locale_ready = (NativeLocale_Initialize() && LocaleLayer_Initialize() &&
                                User32Locale_Initialize() && GdiLocale_Initialize()) ? 1 : 0;

        /*
         * Locale 失败时这里仍然返回 TRUE，让 Windows 把 DllMain 正常完成。
         * 否则系统只会把失败折叠成 0xc0000142，用户拿不到 mods\modloader.log 里的精确状态码。
         * Entry Gate 在 RPG.exe 真正入口前会检查 g_early_locale_ready；失败就记录后安全退出，绝不让游戏在半转区状态继续。
         */
        g_early_prepared = 1;
        if (g_early_locale_ready)
            ModLoader_Log((const WCHAR*)L"[Early完成] Overrides 与 Native/Win32 Locale 已准备；USER32/GDI 若尚未映射会在 EntryPoint 前自动补挂。ASI 仍等待最后加载。");
        else
            ModLoader_Log((const WCHAR*)L"[Early警告] Locale 早期层未通过；保留进程到 EntryPoint 仅用于输出完整诊断，届时不会进入 RPG.exe 业务代码。");
    } else if (reason == DLL_PROCESS_DETACH_) {
        /*
         * 只有真正启用了 game.log 才可能有审计句柄。GameAudit_Shutdown 自己也会再次判断状态，
         * 因此即使 GameLog=0，或者初始化在中途失败，这里调用仍然是安全的空操作。
         */
        GameAudit_Shutdown();
    }
    return TRUE_;
}
