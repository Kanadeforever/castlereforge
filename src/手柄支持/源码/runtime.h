#ifndef CASTLE_PAD_RUNTIME_H
#define CASTLE_PAD_RUNTIME_H

#include "platform.h"

/*
 * runtime.h
 *
 * Runtime 模块是整个 ASI 的“地基”。它不理解战斗、标题、存档这些游戏业务，
 * 只提供下面几类所有模块都会用到的基础能力：
 *
 * 1. 从 RPG.exe 的 IAT 取得最早期 Win32 API；
 * 2. 再从 KERNEL32 / USER32 动态取得其余 API；
 * 3. 读 INI、写中文日志；
 * 4. 校验目标 RPG.exe 是否是我们已经逆向确认的精确版本；
 * 5. 安全修改 CALL / JMP / IAT；
 * 6. 保存全局配置和 worker tick。
 *
 * 重要边界：这里绝不保存“当前战斗选了第几项”之类业务状态。
 * 这样以后某个菜单出问题时，不会把 Runtime 也拖进排查范围。
 */

typedef struct RuntimeConfig {
    int cursor_default_hidden;
    int target_cursor_indicator;
    int battle_shortcuts;
    int battle_remember_selection;

    /*
     * 是否交换“确定”和“取消”这两个语义所使用的物理面键。
     *
     * 0：Xbox位置布局，南键（Xbox A / PS X）=确定，东键（Xbox B / PS O）=取消。
     * 1：PS传统布局，东键（PS O / Xbox B）=确定，南键（PS X / Xbox A）=取消。
     *
     * 这个开关只交换 INPUT_CONFIRM / INPUT_CANCEL 语义。确定始终驱动鼠标左键，取消
     * 始终驱动鼠标右键，所以 Back/RT 鼠标模式也跟随交换。X、Y和 RB+ABXY 战斗
     * 快捷键继续按固定物理位置处理，不受这个开关影响。
     */
    int swap_confirm_cancel;

    /* r37 指针速度：Back常驻与地图RT共用同一套双摇杆鼠标曲线。 */
    u32 mouse_mode_left_stick_sensitivity_percent;
    u32 mouse_mode_right_stick_sensitivity_percent;

    /*
     * 调查模式有两种“怎么进入、怎么确认”的用户选择。
     *
     * 0：默认的新方式。自由地图按住“确定键”进入调查；松开确定键时，如果原版已经
     *    确认当前指针真的停在可互动目标上，就提交一次互动。按住时按“取消键”会取消。
     * 1：R40 以前的方式。自由地图按住 LT 进入调查；调查中另按“确定键”才提交互动。
     *
     * 这里只保存从 INI 读到并裁剪为 0/1 的数字。到底哪颗键进入、何时退出，仍由
     * ControlModes 统一裁决，Runtime 自己不读取手柄按钮。
     */
    int investigation_activation_mode;

    /*
     * 进入调查时是否自动选择离角色最近的可互动目标。
     *
     * 1：默认开启。调查会话刚建立时，复用R40已经实机通过的“距离从近到远”列表，
     *    自动把第0项交给原版resolver/25点probe验证。
     * 0：关闭。进入调查后保持R41行为，等玩家自己推左杆、按LB/RB或移动右杆。
     *
     * 这个值只决定“刚进入时是否自动提出一个候选”，不等于目标已经可互动，
     * 更不会绕过resolver直接点击。
     */
    int investigation_auto_focus_nearest;
    u32 investigation_right_stick_sensitivity_percent;
    u32 investigation_snap_radius_pixels;

    /*
     * StrengthPercent 是全局强度。investigation_rumble_ms 虽保留历史字段名，实际服务于
     * A/LT 调查和 Back/RT 鼠标模式共同的“碰到新可互动对象”短震；controller_mode_rumble_ms
     * 只用于真正激活普通手柄模式的长反馈。普通隐藏鼠标没有震动资格。
     */
    u32 rumble_strength_percent;
    u32 investigation_rumble_ms;
    u32 controller_mode_rumble_ms;

    u32 nav_visual_min_ms;
    u32 nav_hit_count;
    u32 nav_settle_timeout_ms;
    u32 nav_repeat_initial_ms;
    u32 nav_repeat_interval_ms;

    u32 run_threshold_percent;
} RuntimeConfig;

/*
 * RuntimeApi 只收纳“已经成功解析”的 Windows API 地址。
 * 其它模块通过 Runtime_Api() 只读访问，不自行到处 GetProcAddress。
 * 这样 API 解析失败时，错误会集中出现在启动阶段，而不是运行几分钟后才随机炸掉。
 */
typedef struct RuntimeApi {
    PFN_GetModuleHandleA get_module_handle_a;
    PFN_GetProcAddress get_proc_address;
    PFN_LoadLibraryA load_library_a;
    PFN_GetModuleFileNameA get_module_file_name_a;
    PFN_GetModuleHandleExA get_module_handle_ex_a;
    PFN_VirtualProtect virtual_protect;

    PFN_GetCursorPos get_cursor_pos;
    PFN_CreateFileA create_file_a;
    PFN_WriteFile write_file;
    PFN_CloseHandle close_handle;
    PFN_CreateThread create_thread;
    PFN_Sleep sleep;

    PFN_GetForegroundWindow get_foreground_window;
    PFN_GetClientRect get_client_rect;
    PFN_ClientToScreen client_to_screen;
    PFN_SetCursorPos set_cursor_pos;
    PFN_GetWindowThreadProcessId get_window_thread_process_id;
    PFN_GetCurrentProcessId get_current_process_id;
    PFN_GetPrivateProfileIntA get_private_profile_int_a;
    PFN_mouse_event mouse_event;
    PFN_PostMessageA post_message_a;
} RuntimeApi;

/* DllMain 里只允许调用这个“轻量绑定”。它不会读文件，也不会 LoadLibrary。 */
void Runtime_BindEarlyApi(void);

/* Worker 线程启动后调用，完成剩余 API、配置和日志初始化。 */
int Runtime_Initialize(HMODULE self_module);
void Runtime_Shutdown(void);

const RuntimeApi* Runtime_Api(void);
const RuntimeConfig* Runtime_Config(void);
HMODULE Runtime_SelfModule(void);

/* Worker tick 每 8 ms 增加 1。所有模块统一用同一时钟，避免各自重新计时。 */
u32 Runtime_Tick(void);
void Runtime_AdvanceTick(void);
u32 Runtime_MsToTicks(u32 ms);

/* 常用的安全辅助。 */
int Runtime_PtrOk(const void* p);
int Runtime_MemEq(const u8* a, const u8* b, SIZE_T n);
int Runtime_BuildSiblingPath(const char* leaf, char* out, SIZE_T cap);

/* 日志只接收 UTF-8 文本；本项目后续运行日志统一简体中文。 */
void Runtime_Log(const char* utf8_line);
void Runtime_LogHexPair(const char* prefix, u32 a, const char* middle, u32 b);
void Runtime_LogModule(const char* label, HMODULE module, const char* fallback_name);

/* 精确版本预检。任何一个关键锚点不一致，都拒绝安装 Hook。 */
int Runtime_ExactBuildOk(void);

/*
 * 动画跳过是 refactor4 之后新增的可选能力。
 * 它不应该成为战斗/标题/SaveSlot 的全局硬门槛，所以单独校验窗口过程中的两个原版协议锚点。
 */
int Runtime_MovieEscapeProtocolOk(void);

/* refactor9 两项可选能力各自独立预检，失败不能拖垮 refactor7 封版主线。 */
int Runtime_DialogueProtocolOk(void);
int Runtime_SceneChoiceProtocolOk(void);
int Runtime_ConfirmDialogProtocolOk(void);

/*
 * 主 Interface 第一阶段是 refactor16 之后新增的可选协议组。
 * 它在任何 Interface CALL 被改写之前一次性核对地图入口、八大类、退出与角色 Event，
 * 防止“前几个 Hook 已写、后一个地址才失败”的半安装状态。
 */
int Runtime_InterfaceShellProtocolOk(void);

/*
 * 返回“原版 Space 当前实际调用的地图动作业务函数”地址。
 * refactor20a 不再把 0x40B230 的整段函数体当作硬前提；预检会从原版 Space 的 CALL
 * 现场解析真实目标。这样如果其它兼容补丁把 CALL 改到自己的 wrapper，手柄 Y 也会跟随
 * 原版 Space 的当前业务链，而不是继续死盯固定 0x40B230。
 */
u32 Runtime_MapSpaceEventTarget(void);

/* state2“法宝/道具”页是独立页面协议组；失败只禁用该页手柄功能，不影响已验收的 Interface Shell。 */
int Runtime_InterfaceItemsProtocolOk(void);

/* state3“绝学/法术”页独立协议预检；失败只禁用该页面 Adapter。 */
int Runtime_InterfaceSkillsProtocolOk(void);

/* state4“及身/装备”页独立协议预检；失败只禁用装备页 Adapter，不影响已验收 state3。 */
int Runtime_InterfaceEquipmentProtocolOk(void);
int Runtime_InterfaceInnerStatsProtocolOk(void);
int Runtime_InterfaceFormationProtocolOk(void);
/* state7 天书与 state8 机能各自独立预检；失败只关闭对应页面，不连坐已 PASS 页面。 */
int Runtime_InterfaceTomeProtocolOk(void);
/* SaveSlot+0x5A4 三项动作窗口由天书和地图存档点共享，不归任何单一 owner。 */
int Runtime_SaveSlotActionProtocolOk(void);
int Runtime_InterfaceOptionsProtocolOk(void);
int Runtime_InnProtocolOk(void);
int Runtime_SynthesisProtocolOk(void);
int Runtime_ShopProtocolOk(void);
int Runtime_ShopItemInfoProtocolOk(void);
/* 独立存档包装层的构造/绑定/Update 协议；失败只禁用存档点 Adapter。 */
int Runtime_SavePointProtocolOk(void);
/* A/LT 调查共用的地图互动 resolver 和对象布局独立协议门。 */
int Runtime_InvestigationProtocolOk(void);

/*
 * 修改机器码的三个统一入口。
 * 每次修改前都会确认原始目标/字节，没有“地址差不多就写”的宽松模式。
 */
int Runtime_PatchIatPointer(u32 slot, void* replacement, void** original_out);
int Runtime_PatchCall(u32 call_address, void* replacement, u32 expected_target);
int Runtime_PatchJmp6(u32 address, void* replacement, const u8 expected[6]);

/*
 * 把精确的 6 字节“mov esi,[abs32]”改成“mov esi,imm32 + nop”。
 * 用于 refactor10/refactor11 公共消息引擎两处局部 GetAsyncKeyState 函数指针装载。
 */
int Runtime_PatchMovEsiFunction(u32 address, void* replacement, const u8 expected[6]);

/* 供 SDL3 等运行时依赖在成功加载后 PIN，防止被意外 FreeLibrary。 */
void Runtime_PinModuleFromAddress(const char* label, const void* address);

#endif /* CASTLE_PAD_RUNTIME_H */
