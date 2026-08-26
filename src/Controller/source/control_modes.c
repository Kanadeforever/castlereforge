#include "control_modes.h"
#include "runtime.h"
#include "game_addresses.h"
#include "pad_input.h"
#include "input_router.h"
#include "investigation.h"
#include "battle.h"
#include "frontend.h"
#include "save_point.h"
#include "interface_shell.h"
#include "inn.h"
#include "synthesis.h"
#include "shop.h"
#include "confirm_dialog.h"
#include "dialogue_input.h"
#include "scene_choice.h"
#include "ui_bridge.h"

/*
 * 本模块是三种手柄指针状态的唯一裁决者，Cursor 和 Investigation 都不能自己决定进入/退出模式。
 * 这样做不是为了多包一层，而是为了把以下互相冲突的边界放进同一个有限状态机：
 *
 * 1. Back 常驻鼠标在任何界面都能进入；再次 Back 总能退出。
 * 2. RT 临时鼠标只能由“自由地图中的新按下沿”建立，不能从菜单里带着一颗旧 RT 穿出来。
 * 3. 调查激活键由 INI 选择：0=按住A、松开确认（默认），1=按住LT、另按A确认（旧方式）。
 * 4. 两种调查方式都和 RT 共用同一道自由地图门；RT 永远覆盖当前调查。
 * 5. RT 覆盖前已经处于调查，或 RT 期间新按下当前配置的调查键，松开 RT 后都允许恢复调查。
 * 6. 默认模式中按“取消键”会立刻取消，并锁住仍按着的“确定键”，直到确定键真实松开；
 *    取消键先松开也不能重进。具体物理键由 SwapConfirmCancel 决定。
 * 7. 临时态遇到新 UI 必须当帧结束，并锁住仍按着的 RT/LT/A/B，直到物理松开。
 * 8. A/B 的鼠标脉冲若尚未松开，取消/接管指针态时必须由 Cursor_ResetClicks 收尾；
 *    但默认模式“松开确定键”已经获准的左键脉冲不能被退出清理立即抹掉。
 * 9. 实体鼠标接管不是“回到手柄”，所以结束插件会话但不播放模式切换震动。
 * 10. 只有 Back 常驻鼠标回到普通手柄才播放高优先级反馈；RT/调查始终属于手柄子态。
 *
 * free-map 也不是某一个 owner 指针为空这么简单。它同时要求：
 * - 原版地图互动 resolver 仍在持续发布新快照；
 * - 原版地图动作 busy 为零；
 * - 已知标题、战斗、主菜单、客栈、炼化、商店、存档、询问、对话、选择和电影全部不活动；
 * - UiBridge 没有等待游戏线程消费的模态事件。
 *
 * release barrier 是防穿透的第二层保险。CaptureAll 只保护当前 8ms tick；
 * barrier 会在随后每个 tick 继续消费仍按着的 RT/LT/A/B，直到真实松开，
 * 因而刚打开的商店或询问框绝不会把“触发它的同一颗键”再次解释为菜单操作。
 *
 * Back 常驻鼠标的 UI 自动回切还要求自由地图连续稳定 64ms。
 * 这是为了避开剧情句间、析构/构造交界的一帧空档；嵌套弹窗只要仍有任一 owner，计时就会重置。
 */

/* UI全部退出后要求自由地图稳定64ms，避免连续剧情各句间的一帧空档误触自动回切。 */
#define CONTROL_UI_EXIT_STABLE_MS 64u
#define CONTROL_RUMBLE_PRIORITY_MODE 2

typedef enum ControlMode {
    CONTROL_MODE_CONTROLLER = 0,
    CONTROL_MODE_BACK_MOUSE,
    CONTROL_MODE_RT_MOUSE,
    CONTROL_MODE_INVESTIGATION
} ControlMode;

typedef struct ControlModeState {
    ControlMode mode;

    int rt_inhibit_until_release;
    int lt_inhibit_until_release;
    int confirm_inhibit_until_release;
    int cancel_inhibit_until_release;

    int resume_investigation_after_rt;
    int back_mouse_saw_ui;
    int ui_exit_tracking;
    u32 ui_exit_start_tick;
} ControlModeState;

static ControlModeState g_modes;

static int control_movie_active(void) {
    u8* movie = *(u8**)GLOBAL_MOVIE_OBJECT;
    if (!Runtime_PtrOk(movie)) return 0;
    return *(u8*)(movie + MOVIE_ACTIVE_FLAG) != 0;
}

/*
 * “非菜单”不靠单一猜测标志，而是汇总项目已经各自闭合的owner和原版消息状态。
 * 子菜单不单独触发自动回切；只要任一顶层/模态owner仍存在，UI栈就仍被视为活动。
 */
static int control_any_mouse_ui_active(void) {
    if (Battle_AnyUiActive()) return 1;
    if (Frontend_AnyUiActive()) return 1;
    if (SavePoint_Active()) return 1;
    if (InterfaceShell_Active()) return 1;
    if (Inn_Active()) return 1;
    if (Synthesis_Active()) return 1;
    if (Shop_Active()) return 1;
    if (ConfirmDialog_IsActive()) return 1;
    if (DialogueInput_IsActive()) return 1;
    if (SceneChoice_IsActive()) return 1;
    if (*(volatile u32*)GLOBAL_DIALOGUE_ID != 0u) return 1;
    if (control_movie_active()) return 1;
    if (UiBridge_EventPending()) return 1;
    return 0;
}

/* RT临时鼠标和两种调查激活方式严格共用这一道自由地图能力门。 */
static int control_free_map_active(int ui_active) {
    if (ui_active) return 0;
    if (!Investigation_MapSnapshotReady()) return 0;
    if (*(volatile u32*)GLOBAL_MAP_ACTION_BUSY != 0u) return 0;
    return 1;
}

/*
 * 下面五个小函数把“当前调查键到底是 A 还是 LT”集中翻译一次。
 *
 * 如果把同一段“ActivationMode=0时读取INPUT_CONFIRM，否则读取PAD_LT”散落在十几个分支里，
 * 以后很容易只改到“进入”却忘了改“退出”或“RT后恢复”。集中以后，每个业务分支只问：
 * “调查键刚按下了吗？”“还按着吗？”“是否被释放屏障锁住？”而不用再猜物理键。
 */
static int control_investigation_uses_hold_confirm(void) {
    /* Runtime 已把 INI 数字裁剪为0或1；0就是默认“按住确定键”模式。 */
    return Runtime_Config()->investigation_activation_mode == 0;
}

static int control_investigation_pressed(void) {
    /* 按下沿只在 0→1 的第一帧成立，所以持续按住不会每8ms反复进入。 */
    if (control_investigation_uses_hold_confirm()) return InputRouter_RawPressed(INPUT_CONFIRM);
    return PadInput_Pressed(PAD_LT);
}

static int control_investigation_down(void) {
    /* Down 表示物理键这一刻仍压着，用来决定会话是否继续以及RT松开后能否恢复。 */
    if (control_investigation_uses_hold_confirm()) return InputRouter_RawDown(INPUT_CONFIRM);
    return PadInput_Down(PAD_LT);
}

static int control_investigation_released(void) {
    /* 只有默认模式使用“松开确定键即确认”；LT模式仍由调查中的独立确定键按下确认。 */
    if (!control_investigation_uses_hold_confirm()) return 0;
    return InputRouter_RawReleased(INPUT_CONFIRM);
}

static int control_investigation_inhibited(void) {
    /*
     * 取消键结束默认模式后复用 confirm_inhibit；旧LT模式继续复用 lt_inhibit。
     * 屏障存在时，即使键还保持 Down，也不允许从短暂空白帧或新UI后自动重建会话。
     */
    if (control_investigation_uses_hold_confirm()) return g_modes.confirm_inhibit_until_release;
    return g_modes.lt_inhibit_until_release;
}

static void control_consume_release_barriers(void) {
    if (g_modes.rt_inhibit_until_release) {
        if (PadInput_Down(PAD_RT)) InputRouter_Consume(INPUT_SUBTYPE_NEXT);
        else g_modes.rt_inhibit_until_release = 0;
    }
    if (g_modes.lt_inhibit_until_release) {
        if (PadInput_Down(PAD_LT)) InputRouter_Consume(INPUT_SUBTYPE_PREV);
        else g_modes.lt_inhibit_until_release = 0;
    }
    if (g_modes.confirm_inhibit_until_release) {
        if (InputRouter_RawDown(INPUT_CONFIRM)) InputRouter_Consume(INPUT_CONFIRM);
        else g_modes.confirm_inhibit_until_release = 0;
    }
    if (g_modes.cancel_inhibit_until_release) {
        if (InputRouter_RawDown(INPUT_CANCEL)) InputRouter_Consume(INPUT_CANCEL);
        else g_modes.cancel_inhibit_until_release = 0;
    }
}

static void control_arm_release_barriers(int block_rt, int block_lt) {
    if (block_rt && PadInput_Down(PAD_RT)) g_modes.rt_inhibit_until_release = 1;
    if (block_lt && PadInput_Down(PAD_LT)) g_modes.lt_inhibit_until_release = 1;
    if (InputRouter_RawDown(INPUT_CONFIRM)) g_modes.confirm_inhibit_until_release = 1;
    if (InputRouter_RawDown(INPUT_CANCEL)) g_modes.cancel_inhibit_until_release = 1;
}

static void control_rumble_controller_mode(void) {
    const RuntimeConfig* cfg = Runtime_Config();
    u32 strength;

    if (cfg->rumble_strength_percent == 0u || cfg->controller_mode_rumble_ms == 0u) return;
    strength = (cfg->rumble_strength_percent * 65535u) / 100u;
    PadInput_Rumble((u16)strength, (u16)strength,
                    cfg->controller_mode_rumble_ms,
                    CONTROL_RUMBLE_PRIORITY_MODE);
}

static void control_clear_pointer_sessions(void) {
    Investigation_EndSession();
    Cursor_SetMouseModeSession(0);
    Cursor_ResetClicks();
}

static void control_enter_back_mouse(int ui_active, const char* log_text) {
    Investigation_EndSession();
    g_modes.mode = CONTROL_MODE_BACK_MOUSE;
    g_modes.resume_investigation_after_rt = 0;
    g_modes.back_mouse_saw_ui = ui_active ? 1 : 0;
    g_modes.ui_exit_tracking = 0;
    g_modes.ui_exit_start_tick = 0u;
    Cursor_SetMouseModeSession(1);
    Runtime_Log(log_text);
}

/* 只有Back常驻鼠标真正返回普通手柄，才触发1秒（可配置）模式震动。 */
static void control_exit_back_mouse(const char* log_text) {
    g_modes.mode = CONTROL_MODE_CONTROLLER;
    g_modes.resume_investigation_after_rt = 0;
    g_modes.back_mouse_saw_ui = 0;
    g_modes.ui_exit_tracking = 0;
    g_modes.ui_exit_start_tick = 0u;
    control_arm_release_barriers(1, 1);
    control_clear_pointer_sessions();
    control_rumble_controller_mode();
    InputRouter_CaptureAll();
    Runtime_Log(log_text);
}

static void control_enter_rt_mouse(int resume_investigation) {
    Investigation_EndSession();
    g_modes.mode = CONTROL_MODE_RT_MOUSE;
    g_modes.resume_investigation_after_rt = resume_investigation ? 1 : 0;
    Cursor_SetMouseModeSession(1);
    Runtime_Log("[模式] 自由地图RT临时鼠标已进入；这是手柄模式子态，不产生模式切换震动。");
}

static void control_enter_investigation(void) {
    /* 调查不使用“完整鼠标模式”的双摇杆曲线，所以先明确关闭 MouseModeSession。 */
    Cursor_SetMouseModeSession(0);

    /* 从这一行起，Exploration 会看到 BlocksMapMovement=真，左摇杆不再驱动角色走路。 */
    g_modes.mode = CONTROL_MODE_INVESTIGATION;

    /* 新进入不是“等待RT恢复”的中间状态，旧恢复标志必须清零。 */
    g_modes.resume_investigation_after_rt = 0;

    /* 日志必须告诉测试者当前到底启用了哪一种 INI 方式，避免拿错操作表验收。 */
    if (control_investigation_uses_hold_confirm()) {
        Runtime_Log("[模式] 自由地图确定键按住调查已进入；松开确定键确认真实hover，取消键立即取消。");
    } else {
        Runtime_Log("[模式] 自由地图LT按住调查已进入；A沿既有方式确认。");
    }
}

static int control_update_mouse_inputs(void) {
    int moved = Cursor_MoveMouseSticks();

    /*
     * 鼠标模式也遵守确定/取消布局：确定语义永远产生左键，取消语义永远产生右键。
     * 这与RB+ABXY快捷键不同；快捷键按固定物理位置，鼠标点击按语义布局。
     */
    if (InputRouter_RawPressed(INPUT_CONFIRM)) Cursor_PulseLeftClick();
    if (InputRouter_RawPressed(INPUT_CANCEL)) Cursor_PulseRightClick();
    return moved;
}

void ControlModes_Initialize(void) {
    /* DLL刚启动时一定从普通手柄态开始，不能继承任何旧内存里的模式数字。 */
    g_modes.mode = CONTROL_MODE_CONTROLLER;

    /* 四条释放屏障都从“未锁住”开始；它们只会在真实退出/取消时按需武装。 */
    g_modes.rt_inhibit_until_release = 0;
    g_modes.lt_inhibit_until_release = 0;
    g_modes.confirm_inhibit_until_release = 0;
    g_modes.cancel_inhibit_until_release = 0;

    /* 下面三项都是跨tick状态，也必须显式清零，不能依赖静态区碰巧为0。 */
    g_modes.resume_investigation_after_rt = 0;
    g_modes.back_mouse_saw_ui = 0;
    g_modes.ui_exit_tracking = 0;
    g_modes.ui_exit_start_tick = 0u;

    /* 清掉 Cursor/Investigation 可能留下的会话和鼠标按键脉冲，建立干净起点。 */
    control_clear_pointer_sessions();

    if (control_investigation_uses_hold_confirm()) {
        Runtime_Log("[模式] 路由已启用：Back常驻鼠标 > 自由地图RT临时鼠标 > 确定键按住调查 > 原手柄操作；ActivationMode=0。");
    } else {
        Runtime_Log("[模式] 路由已启用：Back常驻鼠标 > 自由地图RT临时鼠标 > LT按住调查 > 原手柄操作；ActivationMode=1。");
    }
}

CursorTakeoverEvent ControlModes_Update(void) {
    int ui_active;
    int free_map;
    int back_pressed;
    int rt_pressed;
    int rt_down;
    int investigation_pressed;
    int investigation_down;
    int investigation_released;
    int investigation_inhibited;
    int cancel_pressed;
    int moved;

    if (!PadInput_GameForeground(NULL) || !PadInput_GamepadConnected()) {
        g_modes.mode = CONTROL_MODE_CONTROLLER;
        g_modes.resume_investigation_after_rt = 0;
        g_modes.back_mouse_saw_ui = 0;
        g_modes.ui_exit_tracking = 0;
        control_clear_pointer_sessions();
        return CURSOR_TAKEOVER_NONE;
    }

    control_consume_release_barriers();

    ui_active = control_any_mouse_ui_active();
    free_map = control_free_map_active(ui_active);
    back_pressed = PadInput_Pressed(PAD_BACK);
    rt_pressed = PadInput_Pressed(PAD_RT);
    rt_down = PadInput_Down(PAD_RT);

    /*
     * 这一组值是同一物理采样帧的“调查键快照”。ActivationMode=0 时它们来自 A，
     * ActivationMode=1 时来自 LT。后面的状态分支只使用这些语义值，保证进入、退出、
     * RT覆盖恢复和释放屏障永远解释成同一颗配置键。
     */
    investigation_pressed = control_investigation_pressed();
    investigation_down = control_investigation_down();
    investigation_released = control_investigation_released();
    investigation_inhibited = control_investigation_inhibited();

    /* 取消语义只在默认调查本体中结束会话；在Back/RT鼠标模式里，它仍产生鼠标右键。 */
    cancel_pressed = InputRouter_RawPressed(INPUT_CANCEL);

    /* Back在任何界面都是唯一常驻模式切换键，并且必须先于其余模式裁决。 */
    if (back_pressed) {
        if (g_modes.mode == CONTROL_MODE_BACK_MOUSE) {
            control_exit_back_mouse("[模式] Back：常驻鼠标退出，已回到普通手柄模式。");
            return CURSOR_TAKEOVER_NONE;
        }

        control_enter_back_mouse(ui_active,
            g_modes.mode == CONTROL_MODE_RT_MOUSE
                ? "[模式] RT临时鼠标中按Back：已转为常驻鼠标；松开RT不会退出。"
                : "[模式] Back：已进入任何界面可用的常驻鼠标模式。");
        InputRouter_CaptureAll();
        return CURSOR_TAKEOVER_RIGHT_STICK;
    }

    if (g_modes.mode == CONTROL_MODE_BACK_MOUSE) {
        if (ui_active) {
            g_modes.back_mouse_saw_ui = 1;
            g_modes.ui_exit_tracking = 0;
        } else if (g_modes.back_mouse_saw_ui && free_map) {
            if (!g_modes.ui_exit_tracking) {
                g_modes.ui_exit_tracking = 1;
                g_modes.ui_exit_start_tick = Runtime_Tick();
            } else if ((Runtime_Tick() - g_modes.ui_exit_start_tick) >=
                       Runtime_MsToTicks(CONTROL_UI_EXIT_STABLE_MS)) {
                control_exit_back_mouse("[模式] 全部菜单/对话已退出并稳定回到地图；常驻鼠标自动回到手柄模式。");
                return CURSOR_TAKEOVER_NONE;
            }
        } else {
            g_modes.ui_exit_tracking = 0;
        }

        /* 地图中的一颗新RT可以退出常驻鼠标；本次RT必须锁到松开，不能立即重进临时态。 */
        if (g_modes.mode == CONTROL_MODE_BACK_MOUSE && free_map && rt_pressed &&
            !g_modes.rt_inhibit_until_release) {
            control_exit_back_mouse("[模式] 自由地图RT：常驻鼠标退出，已回到普通手柄模式。");
            g_modes.rt_inhibit_until_release = 1;
            return CURSOR_TAKEOVER_NONE;
        }

        moved = control_update_mouse_inputs();
        InputRouter_CaptureAll();
        return moved ? CURSOR_TAKEOVER_RIGHT_STICK : CURSOR_TAKEOVER_NONE;
    }

    if (g_modes.mode == CONTROL_MODE_RT_MOUSE) {
        if (!free_map) {
            g_modes.mode = CONTROL_MODE_CONTROLLER;
            g_modes.resume_investigation_after_rt = 0;
            control_arm_release_barriers(1, 1);
            control_clear_pointer_sessions();
            InputRouter_CaptureAll();
            Runtime_Log("[模式] RT临时鼠标期间出现菜单/对话/非自由状态；已无震动退出并锁住RT到松开。");
            return CURSOR_TAKEOVER_NONE;
        }

        /*
         * RT鼠标期间又新按下当前调查键，就记住“RT松开后想回调查”。
         * - 默认模式：这颗确定键同时仍会由 control_update_mouse_inputs 当成鼠标左键；
         * - LT模式：行为与R40以前完全相同。
         * 屏障已锁住时不记录，避免刚被UI/取消键结束的旧按键偷偷恢复。
         */
        if (investigation_pressed && !investigation_inhibited) {
            g_modes.resume_investigation_after_rt = 1;
        }

        if (!rt_down) {
            /*
             * 只有三个条件同时成立才恢复：之前确实提出过恢复、调查键此刻还按着、
             * 该键没有被释放屏障锁住。A/LT如果已经松开，RT结束后就回普通手柄。
             */
            int resume = g_modes.resume_investigation_after_rt &&
                         investigation_down && !investigation_inhibited;

            /* 先离开RT状态并清旧鼠标会话，后面再按 resume 决定新状态。 */
            g_modes.mode = CONTROL_MODE_CONTROLLER;
            g_modes.resume_investigation_after_rt = 0;
            control_arm_release_barriers(0, 0);
            control_clear_pointer_sessions();

            if (resume) {
                /*
                 * control_arm_release_barriers 会把仍按着的 A 当成“鼠标左键残留”暂时锁住。
                 * 但这里已经明确裁决为恢复 A 调查，这颗 A 是合法的模式保持键，不是残留点击，
                 * 所以默认模式必须撤掉刚才那条 confirm 屏障；LT模式没有这一步，因为 block_lt=0。
                 */
                if (control_investigation_uses_hold_confirm()) {
                    g_modes.confirm_inhibit_until_release = 0;
                }

                /* 调查键仍按着：重建调查会话并立即让它读取一次当前安全快照。 */
                control_enter_investigation();
                Investigation_UpdateActive();
                InputRouter_CaptureAll();
                Runtime_Log("[模式] RT临时鼠标松开且当前调查键仍按住：已返回调查；不触发模式切换震动。");
                return CURSOR_TAKEOVER_RIGHT_STICK;
            }

            /* 调查键已经松开或被锁住：本tick吞掉残余输入，下一tick回普通手柄业务。 */
            InputRouter_CaptureAll();
            Runtime_Log("[模式] RT临时鼠标松开：已返回普通手柄；不触发模式切换震动。");
            return CURSOR_TAKEOVER_NONE;
        }

        moved = control_update_mouse_inputs();
        InputRouter_CaptureAll();
        return moved ? CURSOR_TAKEOVER_RIGHT_STICK : CURSOR_TAKEOVER_NONE;
    }

    if (g_modes.mode == CONTROL_MODE_INVESTIGATION) {
        if (!free_map) {
            /*
             * 菜单、对话、电影或地图busy一出现，调查就不再拥有自由地图。
             * 先把仍按着的RT/LT/A/B放进释放屏障，再结束会话；这样新UI不会把
             * “刚才用于调查的同一颗键”解释成一次确认、翻页或鼠标点击。
             */
            g_modes.mode = CONTROL_MODE_CONTROLLER;
            control_arm_release_barriers(1, 1);
            Investigation_EndSession();
            InputRouter_CaptureAll();
            Runtime_Log("[模式] 调查遇到菜单/对话/非自由状态；已退出并等待当前调查键松开后重新武装。");
            return CURSOR_TAKEOVER_NONE;
        }

        /*
         * 默认模式的“取消键”是明确取消，优先级高于 RT 和“确定键松开确认”。
         * control_arm_release_barriers 会看到确定键仍然 Down，于是把 confirm_inhibit 置1。
         * 即使玩家马上松开取消键、却一直不松确定键，这条屏障仍会保留，普通手柄分支
         * 也不会把旧确定键当成一次新的进入请求。只有确定键真实松开，屏障才会清除。
         */
        if (control_investigation_uses_hold_confirm() && cancel_pressed) {
            g_modes.mode = CONTROL_MODE_CONTROLLER;
            g_modes.resume_investigation_after_rt = 0;
            control_arm_release_barriers(0, 0);
            control_clear_pointer_sessions();
            InputRouter_CaptureAll();
            Runtime_Log("[模式] 确定键按住调查中收到取消键：已取消，并锁住确定键直到物理松开。");
            return CURSOR_TAKEOVER_NONE;
        }

        /*
         * 调查中 RT 新按下立刻覆盖。传入的 resume 标志只表示“进入RT这一刻调查键还按着”；
         * 真正恢复时还会再检查一次，保证玩家在RT期间松开确定键/LT后不会错误返回调查。
         */
        if (rt_pressed && !g_modes.rt_inhibit_until_release) {
            control_enter_rt_mouse(investigation_down ? 1 : 0);
            control_update_mouse_inputs();
            InputRouter_CaptureAll();
            return CURSOR_TAKEOVER_RIGHT_STICK;
        }

        /*
         * 当前配置的调查键一旦不再 Down，本次调查就结束。
         * 默认模式必须先在会话仍 active 时询问原版 hover，再结束会话；顺序反过来会让
         * Investigation_ConfirmCurrentHover 因 active=0 安全拒绝，造成“松开确定键永远不互动”。
         */
        if (!investigation_down) {
            int interacted = 0;

            /* 只有默认模式把确定键松开沿解释成确认；LT模式继续保持“松LT只退出”。 */
            if (control_investigation_uses_hold_confirm() && investigation_released) {
                interacted = Investigation_ConfirmCurrentHover();
            }

            /* 模式先回普通手柄，再清调查私有对象/probe；不调用ResetClicks以保留已获准脉冲。 */
            g_modes.mode = CONTROL_MODE_CONTROLLER;
            g_modes.resume_investigation_after_rt = 0;
            Investigation_EndSession();
            InputRouter_CaptureAll();

            if (control_investigation_uses_hold_confirm()) {
                Runtime_Log(interacted
                    ? "[模式] 确定键松开：原版已确认目标，已提交一次调查并退出。"
                    : "[模式] 确定键松开：没有已确认目标或probe未完成，已安全退出且不点击。");
            } else {
                Runtime_Log("[模式] LT松开：调查结束；A确认语义保持旧方式，不触发模式切换震动。");
            }
            return CURSOR_TAKEOVER_NONE;
        }

        /* 激活键仍按着、没有取消也没有RT覆盖：推进本帧方向/LB/RB/右杆调查逻辑。 */
        Investigation_UpdateActive();
        InputRouter_CaptureAll();
        return CURSOR_TAKEOVER_NONE;
    }

    /* 普通手柄：RT必须在自由地图中新按下，菜单里按住后关闭菜单不会突然进入。 */
    if (free_map && rt_pressed && !g_modes.rt_inhibit_until_release) {
        /*
         * RT 与调查键同帧按下时，RT按既定优先级先进入鼠标模式；同时记住调查意图。
         * 之后松开RT、调查键仍按着，就会进入所配置的A或LT调查。
         */
        control_enter_rt_mouse(investigation_pressed && investigation_down ? 1 : 0);
        control_update_mouse_inputs();
        InputRouter_CaptureAll();
        return CURSOR_TAKEOVER_RIGHT_STICK;
    }

    /*
     * 调查只接受自由地图中的“新按下沿”。这条规则同时解决两种穿透：
     * 1. 菜单里一直按着A/LT，关闭菜单后不能突然进入调查；
     * 2. 默认模式被取消后，即使取消键先松开、确定键仍按着，也因屏障和没有新按下沿不能重进。
     *
     * 默认模式还要求取消键当前没有按着。若先按住取消再按确定，这不是合法开始手势；
     * 必须把两键都整理好，再重新按一次确定键。
     */
    if (free_map && investigation_pressed && !investigation_inhibited &&
        (!control_investigation_uses_hold_confirm() || !InputRouter_RawDown(INPUT_CANCEL))) {
        control_enter_investigation();
        Investigation_UpdateActive();
        InputRouter_CaptureAll();
        return CURSOR_TAKEOVER_RIGHT_STICK;
    }

    return CURSOR_TAKEOVER_NONE;
}

void ControlModes_OnPhysicalMouseTakeover(void) {
    if (g_modes.mode == CONTROL_MODE_CONTROLLER) return;

    /* 这是转交键鼠所有权，不是回到手柄，因此绝不触发模式震动。 */
    control_arm_release_barriers(1, 1);
    g_modes.mode = CONTROL_MODE_CONTROLLER;
    g_modes.resume_investigation_after_rt = 0;
    g_modes.back_mouse_saw_ui = 0;
    g_modes.ui_exit_tracking = 0;
    control_clear_pointer_sessions();
    Runtime_Log("[模式] 实体鼠标接管：手柄指针会话已结束；键鼠继续完全由原版处理。");
}

int ControlModes_BlocksMapMovement(void) {
    return g_modes.mode != CONTROL_MODE_CONTROLLER;
}

/*
 * Public API 的模式编号是已经发布给第三方的稳定语义，不能简单 return (u32)g_modes.mode。
 * 这里逐项 switch，是为了允许未来内部 ControlMode enum 自由增加/重排而不破坏 ABI。
 */
u32 ControlModes_PublicMode(void) {
    switch (g_modes.mode) {
    case CONTROL_MODE_CONTROLLER:
        return 0u;
    case CONTROL_MODE_BACK_MOUSE:
        return 1u;
    case CONTROL_MODE_RT_MOUSE:
        return 2u;
    case CONTROL_MODE_INVESTIGATION:
        return 3u;
    default:
        return 0x7FFFFFFFu;
    }
}
