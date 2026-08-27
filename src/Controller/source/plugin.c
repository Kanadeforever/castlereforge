#include "platform.h"
#include "runtime.h"
#include "pad_input.h"
#include "input_router.h"
#include "pad_public_api.h"
#include "cursor.h"
#include "ui_bridge.h"
#include "battle.h"
#include "frontend.h"
#include "save_slot.h"
#include "save_point.h"
#include "exploration.h"
#include "investigation.h"
#include "control_modes.h"
#include "movie_skip.h"
#include "confirm_dialog.h"
#include "dialogue_input.h"
#include "interface_shell.h"
#include "interface_items.h"
#include "interface_skills.h"
#include "interface_equipment.h"
#include "interface_inner_stats.h"
#include "interface_formation.h"
#include "interface_tome.h"
#include "interface_options.h"
#include "inn.h"
#include "synthesis.h"
#include "shop.h"
#include "scene_choice.h"

/*
 * plugin.c
 *
 * 这是整个 ASI 唯一负责“程序生命周期”的源码文件。
 * 业务模块（Battle、Title、SaveSlot 等）都不应该自己创建线程、加载配置、决定 DLL 何时初始化。
 *
 * 可以把它想成学校活动的总调度员：
 * - Runtime 先把场地、电源、名单准备好；
 * - 各个功能模块依次安装自己需要的 Hook；
 * - SDL3 开始读取手柄；
 * - 工作线程每 8 ms 让各模块各做一次自己的事情。
 *
 * 这样以后某个菜单出错，我们不用怀疑“是不是这个菜单偷偷又启动了一套线程”。
 */

static HMODULE g_plugin_module;
static volatile int g_worker_running;

/*
 * 安装阶段按“真实依赖”组织，而不是为了形式去机械复刻某个旧版本的全部顺序：
 *
 * 1. Cursor 先建立光标/鼠标接管基础；
 * 2. UiBridge 随后安装统一 ButtonEvent / ButtonHitTest 分发点；
 * 3. 依赖 UiBridge 的 Battle、Frontend、SaveSlot 与各 Interface Adapter 再登记自己的业务 Hook；
 * 4. Exploration、ConfirmDialog、DialogueInput 只要各自依赖已经满足，就允许保持清晰的模块化顺序。
 *
 * refactor24 继续作为“已验证能力和用户效果”的 Oracle，而不是要求以后每个版本逐行照抄。
 * 如果未来证明某两个 Hook 实际存在顺序依赖，再把那一条依赖加入机械护栏即可。
 *
 * Runtime_ExactBuildOk() 在这些修改发生前已经做过精确版本预检；
 * 每一个 Runtime_PatchCall() 本身还会再次确认该 CALL 原本确实指向我们预期的原版函数。
 * 也就是说不是“先写再看会不会崩”，而是每一个改写点都 fail-closed。
 */
static int plugin_install_all_hooks(void) {
    if (!Cursor_InstallHooks()) return 0;
    if (!UiBridge_InstallHooks()) return 0;
    if (!Battle_InstallHooks()) return 0;
    if (!Frontend_InstallHooks()) return 0;
    if (!SaveSlot_InstallHooks()) return 0;
    /* 三项动作窗口是天书/地图存档点共享的 SaveSlot 子层，必须独立于两个 owner 安装一次。 */
    if (!SaveSlot_InstallActionHooks()) return 0;
    /*
     * refactor31：存档点不是 Interface state7，而是 0x89FCD0 -> wrapper+0x580 -> SaveSlot。
     * SavePoint 只安装自己的协议能力门；基础五处槽位 Hook 与共享三项动作 Hook 都由 SaveSlot 各自安装一次。
     */
    if (!SavePoint_Install()) return 0;
    /*
     * InterfaceShell 先安装已经打开后的根导航 ButtonEvent；地图 Y 本身不再新增机器码 Hook。
     * refactor19 只把 Y 新按下沿交给 Exploration_HookMouseAction 的游戏线程安全点，
     * 再调用 Runtime 从原版 Space CALL 现场解析到的当前业务目标(action=1)，因此既不发送 Space，也不借用休整菜单 0x413933。
     */
    if (!InterfaceShell_InstallHooks()) return 0;
    /* state2 页面使用独立预检/Hook；失败不会反向修改 r19 已验收的 Shell 协议。 */
    if (!InterfaceItems_InstallHooks()) return 0;
    /*
     * state3 的绝学/法术核心业务已经用户实机通过；refactor26 又按新统一操控合法增加 D-Pad 左右翻页。
     * 因此这里继续安装 state3 独立 Adapter，但不再把整个源码文件误标成“字节级冻结”。
     */
    if (!InterfaceSkills_InstallHooks()) return 0;
    /* state4 及身页与 state5 五内页都使用独立 capability；任一页失败都不会连坐已验收页面。 */
    if (!InterfaceEquipment_InstallHooks()) return 0;
    if (!InterfaceInnerStats_InstallHooks()) return 0;
    /* state6 阵形页建立在本轮先修好的共享二维邻居算法上；页面业务仍完全走自己的原版 Event。 */
    if (!InterfaceFormation_InstallHooks()) return 0;
    /*
     * state7/8 是 refactor27 已经由用户实机验收通过的页面，各自继续使用独立 capability：
     * - 天书只负责 state7 owner 生命周期，三项动作窗口已经提升为共享 SaveSlot capability；
     * - 机能只接五个本页 ButtonEvent CALL。
     * 任一页面协议不匹配只会 fail-closed 禁用自己，不允许连坐前面已实机 PASS 的 state2~6。
     * refactor28 不重写这两个页面，只对 state8 的焦点坐标做很小的视觉微调。
     */
    if (!InterfaceTome_InstallHooks()) return 0;
    if (!InterfaceOptions_InstallHooks()) return 0;

    /*
     * refactor29 新增的客栈/炼化都只接各自已经静态闭合的原版 ButtonEvent/HitTest CALL。
     * 客栈根层不创造 B 退出；炼化根层 B 明确点击“用器”右侧退出图标，第二层 B 则走专属取消 Button。
     */
    if (!Inn_InstallHooks()) return 0;
    if (!Synthesis_InstallHooks()) return 0;
    /* 所有脚本店铺共用 0x413FA0；一套 Shop Adapter 覆盖左右列、顶部类别和数量窗。 */
    if (!Shop_InstallHooks()) return 0;

    if (!Exploration_InstallHooks()) return 0;
    /* LT 调查使用独立能力门；协议不匹配时只关闭自身。 */
    if (!Investigation_InstallHooks()) return 0;

    /*
     * refactor21 曾在这里安装 ConfirmDialog 的两种原版双按钮 Update Hook。
     * refactor22 之后的大重构把这一行遗漏了，结果是：
     * - 第一种 0x4272C0 的按钮 CALL 因为 Battle 本来就共用，所以绝学页碰巧还能正常；
     * - 法宝/道具页的弃置、分发确认使用第二种 0x4276F0，只有 ConfirmDialog_InstallHooks()
     *   才会把它的两个 HitTest 和两个 ButtonEvent CALL 接到 UiBridge；
     * - 页面 Adapter 虽然已经收到 A/B/方向键，也能排 pending，但原版第二种确认框里没有 Hook 去消费，
     *   因而用户看到的就是“确定/取消始终没修复”。
     *
     * 这里恢复 refactor21 已经存在的安装步骤。ConfirmDialog 本身不会抢 state2/state3 的业务，
     * 因为 ConfirmDialog_Update/FilterHit 在 InterfaceItems_AnyPopupActive() 或
     * InterfaceSkills_AnyPopupActive() 时会主动让路；这里真正需要的是低层第二类按钮 CALL 接线。
     */
    if (!ConfirmDialog_InstallHooks()) return 0;

    /*
     * refactor26e：恢复 refactor21/refactor22 已经实机使用过的普通剧情对话输入桥。
     *
     * refactor25 的大重构在整理安装链时漏掉了 DialogueInput_InstallHook()。
     * dialogue_input.c 本身从 refactor21 至今字节级没有变化，worker 里也一直保留
     * DialogueInput_Update()，所以最新版出现“剧情中按 A 完全没反应”并不是对话算法坏了，
     * 而是这两个局部 GetAsyncKeyState 包装函数根本没有被安装到 RPG.exe。
     *
     * 必须注意：这里恢复的是“安装步骤”，不是重新设计剧情输入。
     * - 逐字显示阶段：A 只补全本句；
     * - 整句等待阶段：再次按 A 才沿原版左键路径推进；
     * - ConfirmDialog 活动时仍由 dialogue_input.c 自己让出 A，避免一键同时确认弹窗和推进文字。
     */
    if (!DialogueInput_InstallHook()) return 0;

    /*
     * 公共剧情选项不是菜单 ConfirmDialog：
     * - mode=2 的 /q 多行回答走 0x4044F0；
     * - mode=3 的 /z“是/否”走 0x404600。
     * SceneChoice 不修改剧情变量；隐藏鼠标只负责原版命中坐标，视觉焦点和结果提交都交给 RPG.exe。
     */
    if (!SceneChoice_Install()) return 0;

    /*
     * refactor29 只在 refactor28 用户实机 PASS 基线上增加三块能力：
     * 1. 客栈“诸态/炼化/歇息”根菜单：↑/↓聚焦、A执行，B没有原版业务所以只吞掉；
     * 2. 炼化两层列表：复用上下/翻页/类别语义；根层 B 触发真实退出图标，第二层 B 触发真实取消按钮；
     * 3. 客栈/存档点等公共剧情 mode=3“是/否”：方向选择、A确认、B选择否，仍走原版鼠标提交路径。
     *
     * refactor28 的治疗目标选择以及更早的 Battle、主 Interface、标题/SaveSlot、剧情 A、SDL3 热切换全部按 PASS 基线冻结。
     */
    /*
     * 这行不是装饰性的版本号：综合检查器会在最终 ASI 的只读字符串区寻找这些词，
     * 用来拒绝“源码已经是R41，但编译内容仍误放R40旧二进制”的打包错误。
     * 两种模式的数字含义也写进成品，现场只拿到日志时仍能判断用户到底应按 A 还是 LT。
     */
    Runtime_Log("[启动] refactor43 + Public API v1：剧情mode=2多选支持上下/确定；剧情可用RT鼠标；Back不再自动回切；手柄warp不再误判实体鼠标；普通活动自动隐藏路径已删除；Battle Target结束显式隐藏。");
    return 1;
}

/*
 * worker 每一轮的处理顺序也有明确目的：
 *
 * A. 先采样一次 SDL3，保证这一轮所有模块看到的是同一份手柄状态；
 * B. Exploration 只计算左摇杆方向/走跑状态，不直接移动角色；
 * C. Cursor 判断右摇杆或实体鼠标是否接管；
 * D. 如果发生指针接管，立刻通知 Battle 与 Frontend 清除它们的强制视觉状态；
 * E. Battle 总是先更新，因为它还有独立 Result 生命周期；
 * F. 只有没有任何战斗/结果 UI 时，Title/SaveSlot 才读取同一轮的菜单按键；
 * G. 最后统一推进 Runtime tick，再休眠 8 ms。
 *
 * 这个顺序避免“一次 A 同时被战斗和标题吃到”这类跨 Context 污染。
 */
static DWORD WINAPI PluginWorker(void* unused) {
    const RuntimeApi* api;
    CursorTakeoverEvent takeover;
    CursorTakeoverEvent cursor_takeover;
    int save_point_active;
    (void)unused;

    if (!Runtime_Initialize(g_plugin_module)) return 0;
    api = Runtime_Api();

    if (!plugin_install_all_hooks()) {
        Runtime_Log("[致命] Hook 安装过程中出现失败，工作线程不会进入输入循环。");
        return 0;
    }

    /*
     * Start=ESC 没有任何机器码 Hook，只做独立协议校验。
     * 即使电影能力失败，refactor4 已封版的 Hook 和其它手柄功能仍然继续运行。
     */
    MovieSkip_Initialize();

    /*
     * SDL3 缺失时不撤销已经安装的原版 Hook：Hook 本身在没有 pending 输入时会继续调用原函数。
     * 这样日志仍然能明确告诉用户“插件加载成功，但 SDL3 失败”，而不是表现成神秘的整 DLL 未加载。
     */
    if (!PadInput_Initialize()) {
        Runtime_Log("[警告] SDL3 当前不可用；原版键鼠仍保持工作，手柄功能等待 SDL3 可用。");
    }
    ControlModes_Initialize();

    /*
     * Public API 的导出入口从 ASI 被加载时就存在，但内部输入线程可能还没完成初始化。
     * 这里先发布一份“未就绪”的全零快照。这样其它 ASI 即使更早找到 CastlePad_GetApi，
     * 也只会得到 IsReady()==0，而不会读取尚未初始化的 PadInput / ControlModes 内部状态。
     */
    CastlePad_PublicApiReset();
    Runtime_Log("[公共API] CastlePad_GetApi v1 已启用；第三方 ASI 只读取版本化只读快照。");

    g_worker_running = 1;
    Runtime_Log("[启动] 手柄工作线程已开始运行。");

    while (g_worker_running) {
        PadInput_Poll();
        /* 每个 tick 先清空叠加层消费标记；只有真实活动的 overlay 才会在本帧写入。 */
        InputRouter_BeginFrame();

        /*
         * 单一优先级裁决先运行：Back常驻 > 地图RT临时 > 地图LT调查 > r36原业务。
         * Cursor 随后只做实体鼠标仲裁；真实鼠标接管必须无震动结束任何手柄指针会话。
         */
        takeover = ControlModes_Update();
        cursor_takeover = Cursor_Update();
        if (cursor_takeover == CURSOR_TAKEOVER_PHYSICAL_MOUSE) {
            ControlModes_OnPhysicalMouseTakeover();
            takeover = cursor_takeover;
        } else if (takeover == CURSOR_TAKEOVER_NONE) {
            takeover = cursor_takeover;
        }

        /*
         * 到这里为止，本 tick 的 SDL 物理采样、InputRouter 映射和 ControlModes 裁决都已完成。
         * 现在把这些结果复制到 Public API 的只读快照里。第三方读取的是快照，而不是 g_pad、
         * g_modes 或 SDL_Gamepad*，所以 PadSupport 内部结构以后继续变化也不会破坏外部 ABI。
         */
        CastlePad_PublicApiPublishFrame();

        Investigation_UpdateRumble();
        if (takeover != CURSOR_TAKEOVER_NONE) {
            Battle_OnPointerTakeover(takeover);
            Frontend_OnPointerTakeover(takeover);
            ConfirmDialog_OnPointerTakeover();
            SceneChoice_OnPointerTakeover(takeover);
            Inn_OnPointerTakeover(takeover);
            Synthesis_OnPointerTakeover(takeover);
            Shop_OnPointerTakeover(takeover);
            InterfaceItems_OnPointerTakeover(takeover);
            InterfaceSkills_OnPointerTakeover(takeover);
            InterfaceInnerStats_OnPointerTakeover(takeover);
            InterfaceFormation_OnPointerTakeover(takeover);
            InterfaceTome_OnPointerTakeover(takeover);
            InterfaceOptions_OnPointerTakeover(takeover);
            SavePoint_OnPointerTakeover(takeover);
        }

        MovieSkip_Update();
        Exploration_Update();

        /*
         * 0x89FCD0 独立存档 wrapper 是真正模态层，优先级高于标题、主 Interface、客栈、对话和 Battle。
         * 必须每 tick 先让 SavePoint_Update() 处理“消失时 End”，再读 SavePoint_Active()；
         * 否则包装层收起后可能留下一根旧 SaveSlot 指针。
         *
         * 包装层存在时，底层菜单 Context 都不再读输入；只有包装层内部真正活动的深层 ConfirmDialog
         * 以及前置剧情左键脉冲的安全释放维护可以继续。这不依赖“谁先 Consume”的偶然顺序。
         */
        SavePoint_Update();
        save_point_active = SavePoint_Active();

        if (save_point_active) {
            /*
             * 存档点内部按深度优先继续处理真正的子模态层：
             * - SaveSlot_Update 已在上面先处理槽位、三项动作和直接确认框；
             * - action+0x5B4 的二次 Yes/No 属于通用 ConfirmDialog，必须在 wrapper 存在时仍可读手柄；
             * - 进入存档点前的剧情 mode=2/mode=3 可能还有一个 64ms LEFTDOWN 脉冲，只维护到期 LEFTUP，
             *   不允许旧剧情窗口在存档点背后继续读取新按键。
             */
            ConfirmDialog_Update();
            SceneChoice_MaintainPulse();
        } else {
        /*
         * 叠加层必须先于底层菜单处理：
         * - 真实 open 的询问框会消费 A/B/方向，阻止穿透；
         * - 普通对话只消费 A；
         * - 如果没有这两种 Context，消费表保持全 0，refactor7 行为完全不变。
         */
        /* 具体页面弹窗优先于历史通用 ConfirmDialog；页面内部自己决定 A/B/方向语义。 */
        InterfaceItems_Update();
        InterfaceSkills_Update();
        InterfaceEquipment_Update();
        InterfaceInnerStats_Update();
        InterfaceFormation_Update();
        /*
         * state7/8 必须在 ConfirmDialog 之前更新：页面 Adapter 先识别“底层三项/空明流转”，
         * 如果二次 Yes/No 已经 open，则立刻让出；随后 ConfirmDialog 才消费真正的 A/B/方向。
         */
        InterfaceTome_Update();
        InterfaceOptions_Update();

        /*
         * 模态层优先级：菜单 ConfirmDialog -> 剧情 mode=2/mode=3 选择 -> 客栈/炼化根业务 -> 普通对话确定键。
         * 特别是“歇息”后客栈对象可能短时间仍存在；SceneChoice 先消费 A/B/方向可防止同一颗键再次误点客栈按钮。
         */
        ConfirmDialog_Update();
        SceneChoice_Update();
        Shop_Update();
        Inn_Update();
        Synthesis_Update();
        DialogueInput_Update();

        Battle_Update();
        if (!Battle_AnyUiActive()) {
            /* 主 Interface 比标题 Frontend 更具体；若 Interface 存在，本 tick 不再让标题层读取相同输入。 */
            InterfaceShell_Update();
            if (!InterfaceShell_Active()) Frontend_Update();
        }
        }

        Runtime_AdvanceTick();
        if (api->sleep) api->sleep(WORKER_SLEEP_MS);
        else {
            /*
             * 理论上 KERNEL32.Sleep 必定存在；这里仍保留极端兜底。
             * volatile 防止编译器把这个空循环整个优化掉。
             */
            volatile u32 spin;
            for (spin = 0; spin < 500000u; ++spin) { }
        }
    }

    /*
     * 退出时先把公开快照清成“未就绪”，然后才释放 SDL 和光标资源。
     * 这样外部插件不会在 PadSupport 正在退出时继续把上一帧状态当成有效输入。
     */
    CastlePad_PublicApiReset();
    PadInput_Shutdown();
    Cursor_Shutdown();
    Runtime_Log("[退出] 手柄工作线程已停止。");
    Runtime_Shutdown();
    return 0;
}

/*
 * Windows 在 DLL 被加载时会调用 DllMain。
 * reason==1 表示“进程正在加载这个 DLL”，reason==0 表示“进程正在卸载”。
 *
 * Loader Lock 是 Windows 在加载 DLL 时持有的一把全局锁。
 * 在这把锁里面做 LoadLibrary、文件 I/O、SDL 初始化都可能死锁，所以 DllMain 只做三件非常小的事：
 * 1. 记住自己的模块句柄；
 * 2. 从 RPG.exe 已有 IAT 读取最早期 API 地址；
 * 3. 创建独立 worker，真正初始化工作在线程里完成。
 */
BOOL WINAPI DllMain(void* module, DWORD reason, void* reserved) {
    HANDLE thread;
    const RuntimeApi* api;
    (void)reserved;

    if (reason == 1u) {
        g_plugin_module = (HMODULE)module;
        Runtime_BindEarlyApi();
        api = Runtime_Api();

        if (api->create_thread) {
            thread = api->create_thread(NULL, 0u, PluginWorker, NULL, 0u, NULL);
            /*
             * CloseHandle 只关闭“我们手里的线程句柄”，不会结束线程。
             * 真正线程仍会继续执行 PluginWorker。
             */
            if (thread && api->close_handle) api->close_handle(thread);
        }
        /*
         * 不在这里做“没有 CreateThread 就直接安装 Hook”的 fallback。
         * 因为那会重新把 LoadLibrary/日志/文件 I/O 拉回 Loader Lock，风险比少一个兼容兜底更大。
         */
    } else if (reason == 0u) {
        /* ASI 正常生命周期通常跟 RPG.exe 一样长；这里主要为理论上的主动卸载留下停止信号。 */
        g_worker_running = 0;
    }
    return TRUE;
}
