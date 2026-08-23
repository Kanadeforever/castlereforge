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
 * 本模块是三种新手柄指针状态的唯一裁决者，Cursor 和 Investigation 都不能再自行读扳机决定模式。
 * 这样做不是为了多包一层，而是为了把以下互相冲突的边界放进同一个有限状态机：
 *
 * 1. Back 常驻鼠标在任何界面都能进入；再次 Back 总能退出。
 * 2. RT 临时鼠标只能由“自由地图中的新按下沿”建立，不能从菜单里带着一颗旧 RT 穿出来。
 * 3. LT 调查与 RT 共用同一道自由地图门；两者同时存在时 RT 覆盖 LT。
 * 4. RT 覆盖前已经处于调查，或 RT 期间新按下 LT，松开 RT 后都允许恢复调查。
 * 5. 临时态遇到新 UI 必须当帧结束，并锁住仍按着的扳机直到物理松开。
 * 6. A/B 的鼠标脉冲若尚未松开，退出任何指针态时都必须由 Cursor_ResetClicks 收尾。
 * 7. 实体鼠标接管不是“回到手柄”，所以结束插件会话但不播放模式切换震动。
 * 8. 只有 Back 常驻鼠标回到普通手柄才播放高优先级反馈；RT/LT 始终属于手柄子态。
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

/* RT临时鼠标和LT调查严格共用这一道自由地图能力门。 */
static int control_free_map_active(int ui_active) {
    if (ui_active) return 0;
    if (!Investigation_MapSnapshotReady()) return 0;
    if (*(volatile u32*)GLOBAL_MAP_ACTION_BUSY != 0u) return 0;
    return 1;
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
        if (PadInput_Down(PAD_SOUTH)) InputRouter_Consume(INPUT_CONFIRM);
        else g_modes.confirm_inhibit_until_release = 0;
    }
    if (g_modes.cancel_inhibit_until_release) {
        if (PadInput_Down(PAD_EAST)) InputRouter_Consume(INPUT_CANCEL);
        else g_modes.cancel_inhibit_until_release = 0;
    }
}

static void control_arm_release_barriers(int block_rt, int block_lt) {
    if (block_rt && PadInput_Down(PAD_RT)) g_modes.rt_inhibit_until_release = 1;
    if (block_lt && PadInput_Down(PAD_LT)) g_modes.lt_inhibit_until_release = 1;
    if (PadInput_Down(PAD_SOUTH)) g_modes.confirm_inhibit_until_release = 1;
    if (PadInput_Down(PAD_EAST)) g_modes.cancel_inhibit_until_release = 1;
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
    Cursor_SetMouseModeSession(0);
    g_modes.mode = CONTROL_MODE_INVESTIGATION;
    g_modes.resume_investigation_after_rt = 0;
    Runtime_Log("[模式] 自由地图LT调查已进入。");
}

static int control_update_mouse_inputs(void) {
    int moved = Cursor_MoveMouseSticks();

    if (PadInput_Pressed(PAD_SOUTH)) Cursor_PulseLeftClick();
    if (PadInput_Pressed(PAD_EAST)) Cursor_PulseRightClick();
    return moved;
}

void ControlModes_Initialize(void) {
    g_modes.mode = CONTROL_MODE_CONTROLLER;
    g_modes.rt_inhibit_until_release = 0;
    g_modes.lt_inhibit_until_release = 0;
    g_modes.confirm_inhibit_until_release = 0;
    g_modes.cancel_inhibit_until_release = 0;
    g_modes.resume_investigation_after_rt = 0;
    g_modes.back_mouse_saw_ui = 0;
    g_modes.ui_exit_tracking = 0;
    g_modes.ui_exit_start_tick = 0u;
    control_clear_pointer_sessions();
    Runtime_Log("[模式] r36路由底座已启用：Back常驻鼠标 > 自由地图RT临时鼠标 > 自由地图LT调查 > r36原操作。");
}

CursorTakeoverEvent ControlModes_Update(void) {
    int ui_active;
    int free_map;
    int back_pressed;
    int rt_pressed;
    int rt_down;
    int lt_pressed;
    int lt_down;
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
    lt_pressed = PadInput_Pressed(PAD_LT);
    lt_down = PadInput_Down(PAD_LT);

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

        /* RT会话期间新按下LT也获得“RT松开后恢复调查”的资格。 */
        if (lt_pressed && !g_modes.lt_inhibit_until_release) {
            g_modes.resume_investigation_after_rt = 1;
        }

        if (!rt_down) {
            int resume = g_modes.resume_investigation_after_rt && lt_down &&
                         !g_modes.lt_inhibit_until_release;
            g_modes.mode = CONTROL_MODE_CONTROLLER;
            g_modes.resume_investigation_after_rt = 0;
            control_arm_release_barriers(0, 0);
            control_clear_pointer_sessions();

            if (resume) {
                control_enter_investigation();
                Investigation_UpdateActive();
                InputRouter_CaptureAll();
                Runtime_Log("[模式] RT临时鼠标松开且LT仍按住：已返回LT调查；不触发模式切换震动。");
                return CURSOR_TAKEOVER_RIGHT_STICK;
            }

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
            g_modes.mode = CONTROL_MODE_CONTROLLER;
            control_arm_release_barriers(1, 1);
            Investigation_EndSession();
            InputRouter_CaptureAll();
            Runtime_Log("[模式] LT调查遇到菜单/对话/非自由状态；已退出并等待LT松开后重新武装。");
            return CURSOR_TAKEOVER_NONE;
        }

        /* 调查中RT新按下立刻覆盖；之后松开RT且LT仍按住会恢复本调查会话。 */
        if (rt_pressed && !g_modes.rt_inhibit_until_release) {
            control_enter_rt_mouse(lt_down ? 1 : 0);
            control_update_mouse_inputs();
            InputRouter_CaptureAll();
            return CURSOR_TAKEOVER_RIGHT_STICK;
        }

        if (!lt_down) {
            g_modes.mode = CONTROL_MODE_CONTROLLER;
            Investigation_EndSession();
            InputRouter_CaptureAll();
            Runtime_Log("[模式] LT松开：调查结束，仍属于普通手柄模式，不触发模式切换震动。");
            return CURSOR_TAKEOVER_NONE;
        }

        Investigation_UpdateActive();
        InputRouter_CaptureAll();
        return CURSOR_TAKEOVER_NONE;
    }

    /* 普通手柄：RT必须在自由地图中新按下，菜单里按住后关闭菜单不会突然进入。 */
    if (free_map && rt_pressed && !g_modes.rt_inhibit_until_release) {
        control_enter_rt_mouse(lt_pressed && lt_down ? 1 : 0);
        control_update_mouse_inputs();
        InputRouter_CaptureAll();
        return CURSOR_TAKEOVER_RIGHT_STICK;
    }

    /* LT同样只接受自由地图中的新按下；唯一无新沿恢复路径是“RT覆盖调查后再松开RT”。 */
    if (free_map && lt_pressed && !g_modes.lt_inhibit_until_release) {
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
