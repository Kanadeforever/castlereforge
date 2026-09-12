#include "cursor.h"
#include "runtime.h"
#include "game_addresses.h"
#include "pad_input.h"

/*
 * RPG.exe 同时依赖 Windows 鼠标消息和游戏帧内 GetKeyState 轮询。
 * 所以 A/B 不只发一颗可能夹在两帧之间的瞬时消息，而是保持约 48ms，
 * 让两条原版输入链都能稳定看到一次完整的 DOWN/UP。
 *
 * 当前 Cursor 只拥有三种低层职责：显式鼠标会话、调查指针、既有菜单视觉。
 * Back/RT/LT/A/B 的业务解释全部由 ControlModes 完成；这里不再读取任何模式键。
 * refactor43 删除了“任意活动都夺权”的旧通用路径；v0.4.1 只恢复可证明用户意图的回切：
 * 数字键新按沿或左摇杆从死区内推到死区外。普通右摇杆噪声仍不能取得所有权。
 * 键鼠一旦产生真实移动，显隐与点击都交回原版；插件不替键鼠维持会话。
 * RT/LT 结束时必须释放仍在计时的模拟按键，避免把 DOWN 带回普通菜单。
 * 所有坐标 API 对外都以 640x480 客户区为准，系统 SetCursorPos 才使用屏幕坐标。
 * 两套摇杆低速余量分开保存，模式切换时清零，防止旧余量造成首帧漂移。
 */
#define CURSOR_CLICK_PULSE_MS 48u

typedef struct CursorState {
    PFN_GetKeyState game_get_key_state;
    PFN_SetCursorPos game_set_cursor_pos;

    int controller_owner;
    int target_indicator_active;
    int menu_focus_indicator_active;
    int mouse_mode_active;
    int investigation_session_active;
    int effective_visible;
    int visible_state_logged;

    Point32 last_cursor_sample;
    int last_cursor_sample_valid;
    u32 game_warp_tick;
    i32 game_warp_x;
    i32 game_warp_y;

    int mouse_left_sent;
    int mouse_right_sent;
    int left_pulse_active;
    int right_pulse_active;
    u32 left_release_tick;
    u32 right_release_tick;

    /* 三套速度各自保留不足 1 像素的百分之一像素余量。 */
    i32 mouse_left_remainder_x;
    i32 mouse_left_remainder_y;
    i32 mouse_right_remainder_x;
    i32 mouse_right_remainder_y;
    i32 investigation_remainder_x;
    i32 investigation_remainder_y;

    /*
     * 左摇杆只有从死区内真正推到死区外才算一次“明确手柄操作”。
     * 若一直按住时玩家又移动实体鼠标，不能每个 8ms tick 都把所有权抢回来。
     */
    int left_stick_activity_latched;
} CursorState;

static CursorState g_cursor;

static int cursor_abs(int value) { return value < 0 ? -value : value; }

/* 只在鼠标两键上叠加插件脉冲；真实键鼠与其它虚拟键一律原样返回。 */
static SHORT WINAPI Cursor_HookGameGetKeyState(int virtual_key) {
    SHORT original_state = 0;

    if (g_cursor.game_get_key_state) original_state = g_cursor.game_get_key_state(virtual_key);
    if (virtual_key == (int)VK_LBUTTON_ && g_cursor.mouse_left_sent) {
        return (SHORT)((u16)original_state | 0x8000u);
    }
    if (virtual_key == (int)VK_RBUTTON_ && g_cursor.mouse_right_sent) {
        return (SHORT)((u16)original_state | 0x8000u);
    }
    return original_state;
}

/*
 * 记录 RPG.exe/插件自己的 warp，防止下一 tick 被误判成实体鼠标接管。
 *
 * refactor42 以前只记“请求移动到哪里”，随后调用者又把这个请求值保存成采样基线。
 * 在 DPI 缩放、窗口边界或 ddraw 包装层存在时，Windows 最终落点可能和请求值差1像素；
 * 下一次 GetCursorPos 就会把这1像素误判为真人移动，ControlModes 因而立刻退出Back/RT鼠标。
 *
 * 现在 SetCursorPos 返回后立即再读一次真实系统落点，并把真实值保存为基线。
 * 玩家随后真的移动鼠标时，下一次采样仍会变化，所以实体鼠标接管能力不会被关闭。
 */
static BOOL WINAPI Cursor_HookGameSetCursorPos(i32 x, i32 y) {
    const RuntimeApi* api = Runtime_Api();
    Point32 actual;
    BOOL moved;

    g_cursor.game_warp_tick = Runtime_Tick();
    g_cursor.game_warp_x = x;
    g_cursor.game_warp_y = y;

    moved = g_cursor.game_set_cursor_pos ? g_cursor.game_set_cursor_pos(x, y) : FALSE;
    if (!moved) return FALSE;

    actual.x = x;
    actual.y = y;
    if (api && api->get_cursor_pos) {
        /* 读取失败时保留请求值兜底；成功时使用Windows最终确认的真实屏幕坐标。 */
        api->get_cursor_pos(&actual);
    }
    g_cursor.last_cursor_sample = actual;
    g_cursor.last_cursor_sample_valid = 1;
    return TRUE;
}

/* 稳定基线的摇杆曲线：死区外轻推1像素，推满每tick最多10像素。 */
static int cursor_stick_step(i16 value) {
    int v = (int)value;
    int magnitude = cursor_abs(v);
    int normalized;
    int step;

    if (magnitude < PAD_STICK_DEADZONE) return 0;
    normalized = ((magnitude - PAD_STICK_DEADZONE) * 1000) /
                 (32767 - PAD_STICK_DEADZONE);
    step = (normalized * normalized * 10) / 1000000;
    if (step < 1) step = 1;
    return v < 0 ? -step : step;
}

/* 按百分比缩放，小于 1 像素的部分留给后续 tick，因此 8%/15% 不会被整数除法吃成 0。 */
static int cursor_scale_step(int base_step, u32 percent, i32* remainder) {
    i32 total;
    i32 whole;

    if (!remainder) return 0;
    if (base_step == 0) {
        *remainder = 0;
        return 0;
    }
    total = (i32)(base_step * (int)percent) + *remainder;
    whole = total / 100;
    *remainder = total - whole * 100;
    return (int)whole;
}

static void cursor_clear_mouse_mode_remainders(void) {
    g_cursor.mouse_left_remainder_x = 0;
    g_cursor.mouse_left_remainder_y = 0;
    g_cursor.mouse_right_remainder_x = 0;
    g_cursor.mouse_right_remainder_y = 0;
}

static void cursor_clear_investigation_remainders(void) {
    g_cursor.investigation_remainder_x = 0;
    g_cursor.investigation_remainder_y = 0;
}

/* 将系统指针限制在当前 RPG.exe 客户区，并登记为手柄主动移动。 */
static int cursor_apply_delta(HWND hwnd, int dx, int dy) {
    const RuntimeApi* api = Runtime_Api();
    Point32 point;
    Point32 top_left;
    Point32 bottom_right;
    Rect32 client;

    if (dx == 0 && dy == 0) return 0;
    if (!hwnd || !api->get_cursor_pos || !api->get_cursor_pos(&point) ||
        !api->set_cursor_pos) return 0;

    point.x += dx;
    point.y += dy;

    if (api->get_client_rect && api->client_to_screen && api->get_client_rect(hwnd, &client)) {
        top_left.x = client.left;
        top_left.y = client.top;
        bottom_right.x = client.right;
        bottom_right.y = client.bottom;
        if (api->client_to_screen(hwnd, &top_left) && api->client_to_screen(hwnd, &bottom_right)) {
            if (point.x < top_left.x) point.x = top_left.x;
            if (point.y < top_left.y) point.y = top_left.y;
            if (point.x >= bottom_right.x) {
                point.x = bottom_right.x > top_left.x ? bottom_right.x - 1 : top_left.x;
            }
            if (point.y >= bottom_right.y) {
                point.y = bottom_right.y > top_left.y ? bottom_right.y - 1 : top_left.y;
            }
        }
    }

    if (!Cursor_HookGameSetCursorPos(point.x, point.y)) return 0;
    return 1;
}

/* 完整鼠标会话：左摇杆为旧普通曲线，右摇杆为默认15%精细曲线，两者可同时叠加。 */
static int cursor_move_mouse_mode(HWND hwnd) {
    const RuntimeConfig* cfg = Runtime_Config();
    int dx;
    int dy;

    dx = cursor_scale_step(cursor_stick_step(PadInput_Axis(PAD_AXIS_LEFT_X)),
                           cfg->mouse_mode_left_stick_sensitivity_percent,
                           &g_cursor.mouse_left_remainder_x);
    dy = cursor_scale_step(cursor_stick_step(PadInput_Axis(PAD_AXIS_LEFT_Y)),
                           cfg->mouse_mode_left_stick_sensitivity_percent,
                           &g_cursor.mouse_left_remainder_y);
    dx += cursor_scale_step(cursor_stick_step(PadInput_Axis(PAD_AXIS_RIGHT_X)),
                            cfg->mouse_mode_right_stick_sensitivity_percent,
                            &g_cursor.mouse_right_remainder_x);
    dy += cursor_scale_step(cursor_stick_step(PadInput_Axis(PAD_AXIS_RIGHT_Y)),
                            cfg->mouse_mode_right_stick_sensitivity_percent,
                            &g_cursor.mouse_right_remainder_y);
    return cursor_apply_delta(hwnd, dx, dy);
}

int Cursor_MoveInvestigationRightStick(void) {
    const RuntimeConfig* cfg = Runtime_Config();
    HWND hwnd = NULL;
    int dx;
    int dy;

    if (!g_cursor.investigation_session_active || !PadInput_GameForeground(&hwnd)) {
        cursor_clear_investigation_remainders();
        return 0;
    }

    dx = cursor_scale_step(cursor_stick_step(PadInput_Axis(PAD_AXIS_RIGHT_X)),
                           cfg->investigation_right_stick_sensitivity_percent,
                           &g_cursor.investigation_remainder_x);
    dy = cursor_scale_step(cursor_stick_step(PadInput_Axis(PAD_AXIS_RIGHT_Y)),
                           cfg->investigation_right_stick_sensitivity_percent,
                           &g_cursor.investigation_remainder_y);
    return cursor_apply_delta(hwnd, dx, dy);
}

static void cursor_update_visibility(void) {
    const RuntimeConfig* cfg = Runtime_Config();
    int visible;

    /*
     * 键鼠所有权时绝不压制原版。手柄所有权时，只有明确指针业务显示光标：
     * RT 鼠标、LT 调查、Battle Target 或菜单的主动焦点指示。
     */
    if (!g_cursor.controller_owner) visible = 1;
    else if (!cfg->cursor_default_hidden) visible = 1;
    else if (g_cursor.mouse_mode_active || g_cursor.investigation_session_active) visible = 1;
    else if (g_cursor.target_indicator_active || g_cursor.menu_focus_indicator_active) visible = 1;
    else visible = 0;

    g_cursor.effective_visible = visible;
    if (visible != g_cursor.visible_state_logged) {
        if (!g_cursor.controller_owner) Runtime_Log("[光标] 键鼠所有权：完全交回原版显隐。");
        else if (g_cursor.mouse_mode_active) Runtime_Log("[光标] Back/RT完整鼠标会话：光标可见。");
        else if (g_cursor.investigation_session_active) Runtime_Log("[光标] LT探索调查模式：光标可见。");
        else if (visible) Runtime_Log("[光标] 手柄目标/菜单焦点指示可见。");
        else Runtime_Log("[光标] 手柄普通模式：原版光标已压制。");
        g_cursor.visible_state_logged = visible;
    }
}

/* mouse_event 与 GetKeyState Hook 共用同一份“希望按住”状态。 */
static void cursor_send_mouse_state(int want_left, int want_right) {
    const RuntimeApi* api = Runtime_Api();

    if (want_left != g_cursor.mouse_left_sent) {
        if (api->mouse_event) {
            api->mouse_event(want_left ? MOUSEEVENTF_LEFTDOWN_ : MOUSEEVENTF_LEFTUP_, 0, 0, 0, 0);
        }
        g_cursor.mouse_left_sent = want_left;
    }
    if (want_right != g_cursor.mouse_right_sent) {
        if (api->mouse_event) {
            api->mouse_event(want_right ? MOUSEEVENTF_RIGHTDOWN_ : MOUSEEVENTF_RIGHTUP_, 0, 0, 0, 0);
        }
        g_cursor.mouse_right_sent = want_right;
    }
}

static void cursor_update_click_pulses(void) {
    u32 now = Runtime_Tick();
    int changed = 0;

    if (g_cursor.left_pulse_active && (i32)(now - g_cursor.left_release_tick) >= 0) {
        g_cursor.left_pulse_active = 0;
        g_cursor.left_release_tick = 0;
        changed = 1;
    }
    if (g_cursor.right_pulse_active && (i32)(now - g_cursor.right_release_tick) >= 0) {
        g_cursor.right_pulse_active = 0;
        g_cursor.right_release_tick = 0;
        changed = 1;
    }
    if (changed) cursor_send_mouse_state(g_cursor.left_pulse_active, g_cursor.right_pulse_active);
}

void Cursor_PulseLeftClick(void) {
    g_cursor.left_pulse_active = 1;
    g_cursor.left_release_tick = Runtime_Tick() + Runtime_MsToTicks(CURSOR_CLICK_PULSE_MS);
    cursor_send_mouse_state(1, g_cursor.right_pulse_active);
}

void Cursor_PulseRightClick(void) {
    g_cursor.right_pulse_active = 1;
    g_cursor.right_release_tick = Runtime_Tick() + Runtime_MsToTicks(CURSOR_CLICK_PULSE_MS);
    cursor_send_mouse_state(g_cursor.left_pulse_active, 1);
}

void Cursor_ResetClicks(void) {
    g_cursor.left_pulse_active = 0;
    g_cursor.right_pulse_active = 0;
    g_cursor.left_release_tick = 0;
    g_cursor.right_release_tick = 0;
    cursor_send_mouse_state(0, 0);
}

void Cursor_ClaimForControllerNavigation(void) {
    if (!g_cursor.controller_owner) Runtime_Log("[光标] 手柄取得指针所有权。");
    g_cursor.controller_owner = 1;
    cursor_update_visibility();
}

void Cursor_SetInitialControllerMode(int gamepad_connected) {
    /*
     * 这一步发生在 PadInput_Initialize 之后、RPG.exe 原入口之前。
     * 已连接手柄代表本次启动默认按手柄产品规格工作；没有手柄则不能把键鼠用户的光标藏起来。
     */
    g_cursor.controller_owner = gamepad_connected ? 1 : 0;
    g_cursor.target_indicator_active = 0;
    g_cursor.menu_focus_indicator_active = 0;
    g_cursor.left_stick_activity_latched = 0;
    cursor_update_visibility();
    Runtime_Log(gamepad_connected ?
        "[模式] 启动时检测到手柄：默认进入普通手柄模式，鼠标只在规定场景显示。" :
        "[模式] 启动时没有可用手柄：保留键鼠所有权；首次明确手柄操作会自动切回手柄。");
}

void Cursor_ReleaseForUnavailableGamepad(void) {
    if (g_cursor.controller_owner) {
        Runtime_Log("[模式] 手柄不可用：已交还键鼠所有权并恢复原版鼠标显隐。");
    }
    g_cursor.controller_owner = 0;
    g_cursor.target_indicator_active = 0;
    g_cursor.menu_focus_indicator_active = 0;
    g_cursor.left_stick_activity_latched = 0;
    cursor_update_visibility();
}

static void cursor_release_to_physical_mouse(void) {
    if (g_cursor.controller_owner) Runtime_Log("[光标] 检测到实体鼠标移动，恢复键鼠所有权。");
    g_cursor.controller_owner = 0;
    g_cursor.target_indicator_active = 0;
    g_cursor.menu_focus_indicator_active = 0;
    cursor_update_visibility();
}

/* 相邻采样坐标发生变化，且不是游戏/插件刚才的 warp，才算实体鼠标。 */
static int cursor_observe_physical_mouse(void) {
    const RuntimeApi* api = Runtime_Api();
    Point32 now;

    if (!api->get_cursor_pos || !api->get_cursor_pos(&now)) return 0;
    if (!g_cursor.last_cursor_sample_valid) {
        g_cursor.last_cursor_sample = now;
        g_cursor.last_cursor_sample_valid = 1;
        return 0;
    }
    if (now.x == g_cursor.last_cursor_sample.x && now.y == g_cursor.last_cursor_sample.y) return 0;

    g_cursor.last_cursor_sample = now;
    if ((Runtime_Tick() - g_cursor.game_warp_tick) <= 2u &&
        now.x == g_cursor.game_warp_x && now.y == g_cursor.game_warp_y) return 0;

    cursor_release_to_physical_mouse();
    return 1;
}

/*
 * 判断本帧是否出现“玩家明确想用手柄”的普通输入。
 *
 * 数字键只看 Pressed，避免玩家按住某键时移动实体鼠标后，下一 tick 又被旧 Down 状态抢回。
 * 左摇杆使用死区出沿；右摇杆在普通态完全忽略，因为摇杆静止噪声不应让键鼠模式闪回手柄。
 * Back/合法 RT/调查键会在更早的 ControlModes 中建立显式可见鼠标会话，不走这里的普通隐藏路径。
 */
static int cursor_has_intentional_controller_activity(void) {
    static const PadButton digital_buttons[] = {
        PAD_SOUTH, PAD_EAST, PAD_WEST, PAD_NORTH,
        PAD_START, PAD_R3, PAD_LB, PAD_RB,
        PAD_DPAD_UP, PAD_DPAD_DOWN, PAD_DPAD_LEFT, PAD_DPAD_RIGHT,
        PAD_LT, PAD_RT
    };
    i16 left_x = PadInput_Axis(PAD_AXIS_LEFT_X);
    i16 left_y = PadInput_Axis(PAD_AXIS_LEFT_Y);
    int left_active = cursor_abs((int)left_x) >= PAD_STICK_DEADZONE ||
                      cursor_abs((int)left_y) >= PAD_STICK_DEADZONE;
    int left_edge = left_active && !g_cursor.left_stick_activity_latched;
    SIZE_T index;

    if (!PadInput_GamepadConnected()) {
        g_cursor.left_stick_activity_latched = 0;
        return 0;
    }

    /* 每帧都记轴状态，即使已是手柄所有权或同帧按了数字键；否则持杆后的鼠标接管会被误抢回。 */
    g_cursor.left_stick_activity_latched = left_active;
    for (index = 0u; index < sizeof(digital_buttons) / sizeof(digital_buttons[0]); ++index) {
        if (PadInput_Pressed(digital_buttons[index])) return 1;
    }

    return left_edge;
}

/* 主鼠标 draw hook：插件只能额外压制，从不强迫原版本来不画的光标出现。 */
static void FASTCALL Cursor_HookMouseDraw(void* mouse, void* unused_edx) {
    u8* m = (u8*)mouse;
    u8* sprite;
    PFN_ThisVoid draw = (PFN_ThisVoid)FN_MOUSE_SPRITE_DRAW;
    i32 x;
    i32 y;
    (void)unused_edx;

    if (!Runtime_PtrOk(m)) return;
    if (Runtime_Config()->cursor_default_hidden && !g_cursor.effective_visible) return;
    if (*(u8*)(m + MOUSE_DRAW_ENABLE) == 0) return;

    sprite = *(u8**)(m + MOUSE_SPRITE);
    if (!Runtime_PtrOk(sprite)) return;
    x = *(i32*)(m + MOUSE_POS_X) - 0x13F;
    y = *(i32*)(m + MOUSE_POS_Y) - 0x104;
    *(i32*)(sprite + 0x24u) = x;
    *(i32*)(sprite + 0x28u) = y;
    draw(sprite);
}

/* 探索场景还有 CURSOR.SCI 代理绘制，必须与主鼠标共用显隐判定。 */
static void FASTCALL Cursor_HookExploreCursorDraw(void* cursor, void* unused_edx, i32 arg) {
    PFN_ExploreCursorDraw orig = (PFN_ExploreCursorDraw)FN_EXPLORE_CURSOR_DRAW;
    (void)unused_edx;
    if (Runtime_Config()->cursor_default_hidden && !g_cursor.effective_visible) return;
    orig(cursor, arg);
}

int Cursor_InstallHooks(void) {
    static const u8 mouse_draw_expected[6] = {0x8A,0x81,0x48,0x02,0x00,0x00};

    if (!Runtime_PatchIatPointer(IAT_GETKEYSTATE, (void*)Cursor_HookGameGetKeyState,
                                 (void**)&g_cursor.game_get_key_state)) {
        Runtime_Log("[致命] GetKeyState IAT 鼠标按键可靠桥 Hook 安装失败。");
        return 0;
    }
    if (!Runtime_PatchIatPointer(IAT_SETCURSORPOS, (void*)Cursor_HookGameSetCursorPos,
                                 (void**)&g_cursor.game_set_cursor_pos)) {
        Runtime_Log("[致命] SetCursorPos IAT 所有权 Hook 安装失败。");
        return 0;
    }
    if (!Runtime_PatchJmp6(FN_MOUSE_DRAW, (void*)Cursor_HookMouseDraw, mouse_draw_expected)) {
        Runtime_Log("[致命] 主鼠标绘制 Hook 安装失败。");
        return 0;
    }
    if (!Runtime_PatchCall(CALL_EXPLORE_CURSOR_DRAW, (void*)Cursor_HookExploreCursorDraw,
                           FN_EXPLORE_CURSOR_DRAW)) {
        Runtime_Log("[致命] 地图 CURSOR.SCI 绘制 Hook 安装失败。");
        return 0;
    }

    g_cursor.controller_owner = 0;
    g_cursor.effective_visible = 1;
    g_cursor.visible_state_logged = -1;
    g_cursor.left_stick_activity_latched = 0;
    cursor_update_visibility();
    Runtime_Log("[鼠标] 普通手柄所有权默认隐藏；实体鼠标可接管，明确手柄操作可重新取得所有权，右杆漂移不会误切模式。");
    return 1;
}

/*
 * Cursor_Update 只维护按键脉冲、实体鼠标接管与显式会话所有权。
 * 显式 Back/RT/LT 会话已经在同 tick 更早由 ControlModes 建立；此处绝不再次解释扳机，
 * 也不使用“任意轴变化都隐藏鼠标”的旧通用机制。
 * 菜单/战斗真正需要手柄焦点时会明确调用 Cursor_ClaimForControllerNavigation；
 * 地图移动本身不再偷偷改变鼠标显隐或所有权。
 */
CursorTakeoverEvent Cursor_Update(void) {
    int foreground = PadInput_GameForeground(NULL);
    int intentional_activity = cursor_has_intentional_controller_activity();
    int physical_moved;

    cursor_update_click_pulses();

    if (!foreground) {
        /* 窗口外的鼠标移动不能在重新切回时误判为游戏内真实输入。 */
        g_cursor.last_cursor_sample_valid = 0;
        g_cursor.mouse_mode_active = 0;
        g_cursor.investigation_session_active = 0;
        cursor_clear_mouse_mode_remainders();
        cursor_clear_investigation_remainders();
        Cursor_ResetClicks();
        cursor_update_visibility();
        return CURSOR_TAKEOVER_NONE;
    }

    /* 实体鼠标永远优先；模式层收到事件后会无震动结束自己的会话。 */
    physical_moved = cursor_observe_physical_mouse();
    if (physical_moved) return CURSOR_TAKEOVER_PHYSICAL_MOUSE;

    if (g_cursor.mouse_mode_active || g_cursor.investigation_session_active) {
        g_cursor.controller_owner = 1;
        cursor_update_visibility();
        return CURSOR_TAKEOVER_NONE;
    }

    /*
     * 实体鼠标接管后，下一次明确数字键或左摇杆出沿会恢复普通手柄所有权。
     * 特定菜单稍后仍会按自己的视觉规则显示唯一手形；其它普通状态在这里立即隐藏鼠标。
     */
    if (!g_cursor.controller_owner && intentional_activity) {
        Runtime_Log("[模式] 检测到明确手柄操作：已从键鼠切回普通手柄所有权。");
        g_cursor.controller_owner = 1;
    }
    cursor_update_visibility();
    return CURSOR_TAKEOVER_NONE;
}

void Cursor_SetMouseModeSession(int active) {
    active = active ? 1 : 0;
    if (active == g_cursor.mouse_mode_active) {
        if (active) {
            g_cursor.controller_owner = 1;
            cursor_update_visibility();
        }
        return;
    }

    g_cursor.mouse_mode_active = active;
    cursor_clear_mouse_mode_remainders();
    if (active) {
        g_cursor.controller_owner = 1;
        Runtime_Log("[鼠标] 显式完整鼠标会话已建立：左杆100%、右杆15%，A左键/B右键。");
    } else {
        Runtime_Log("[鼠标] 显式完整鼠标会话已结束。");
    }
    cursor_update_visibility();
}

int Cursor_MoveMouseSticks(void) {
    HWND hwnd = NULL;
    if (!g_cursor.mouse_mode_active || !PadInput_GameForeground(&hwnd)) {
        cursor_clear_mouse_mode_remainders();
        return 0;
    }
    g_cursor.controller_owner = 1;
    return cursor_move_mouse_mode(hwnd);
}

int Cursor_MouseModeActive(void) { return g_cursor.mouse_mode_active; }
int Cursor_InvestigationSessionActive(void) { return g_cursor.investigation_session_active; }

void Cursor_SetInvestigationSession(int active) {
    active = active ? 1 : 0;
    if (active == g_cursor.investigation_session_active) {
        if (active) {
            /* 调查 warp 后重申手柄所有权，不让采样顺序造成偶发键鼠接管。 */
            g_cursor.controller_owner = 1;
            cursor_update_visibility();
        }
        return;
    }

    g_cursor.investigation_session_active = active;
    cursor_clear_investigation_remainders();
    if (active) {
        g_cursor.controller_owner = 1;
        Runtime_Log("[调查] 指针所有权进入地图调查会话。");
    } else {
        Runtime_Log("[调查] 地图调查指针会话结束。");
    }
    cursor_update_visibility();
}

int Cursor_GetOutputPointerPosition(i32* x, i32* y) {
    const RuntimeApi* api = Runtime_Api();
    HWND hwnd = NULL;
    Point32 point;
    Point32 origin;

    if (!x || !y || !PadInput_GameForeground(&hwnd) ||
        !api->get_cursor_pos || !api->get_cursor_pos(&point)) return 0;
    origin.x = 0;
    origin.y = 0;
    if (api->client_to_screen) api->client_to_screen(hwnd, &origin);
    *x = point.x - origin.x;
    *y = point.y - origin.y;
    return 1;
}

int Cursor_GetPointerPosition(i32* x, i32* y) {
    CastleDisplayGeometryV1 geometry = {0};
    if (!Cursor_GetOutputPointerPosition(x,y)) return 0;
    if (Runtime_CopyDisplayGeometry(&geometry)) {
        *x -= geometry.center_x;
        *y -= geometry.center_y;
    }
    return 1;
}

/* 菜单给出原版局部坐标，在最终输出中只平移中央区；不改变按钮几何和页面焦点。 */
static int cursor_set_ui_position(i32 x, i32 y) {
    CastleDisplayGeometryV1 geometry = {0};
    if (!Runtime_CopyDisplayGeometry(&geometry)) return 0;
    return Cursor_HookGameSetCursorPos(x + geometry.center_x, y + geometry.center_y);
}

int Cursor_MoveControllerAt(i32 x, i32 y) {
    const RuntimeApi* api = Runtime_Api();
    HWND hwnd = NULL;
    u8* mouse = *(u8**)GLOBAL_MOUSE_MANAGER;
    Point32 screen;
    CastleDisplayGeometryV1 geometry = {0};

    if (!PadInput_GameForeground(&hwnd) || !g_cursor.game_set_cursor_pos) return 0;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (!Runtime_CopyDisplayGeometry(&geometry) || !geometry.output_width || !geometry.output_height) return 0;
    if (x >= (i32)geometry.output_width) x = (i32)geometry.output_width - 1;
    if (y >= (i32)geometry.output_height) y = (i32)geometry.output_height - 1;

    screen.x = x;
    screen.y = y;
    if (api->client_to_screen) api->client_to_screen(hwnd, &screen);

    if (Runtime_PtrOk(mouse)) {
        *(i32*)(mouse + MOUSE_POS_X) = screen.x;
        *(i32*)(mouse + MOUSE_POS_Y) = screen.y;
    }
    if (!Cursor_HookGameSetCursorPos(screen.x, screen.y)) return 0;
    g_cursor.controller_owner = 1;
    return 1;
}

/* Battle Target 已验收接口保留：同步 MouseManager 与 Windows 真实指针。 */
void Cursor_ShowTargetAt(i32 x, i32 y) {
    u8* mouse = *(u8**)GLOBAL_MOUSE_MANAGER;
    if (!Runtime_Config()->target_cursor_indicator || !g_cursor.controller_owner) return;
    if (!Runtime_PtrOk(mouse) || !g_cursor.game_set_cursor_pos) return;

    *(i32*)(mouse + MOUSE_POS_X) = x;
    *(i32*)(mouse + MOUSE_POS_Y) = y;
    cursor_set_ui_position(x, y);
    g_cursor.target_indicator_active = 1;
    cursor_update_visibility();
}

void Cursor_HideTargetImmediately(void) {
    if (!g_cursor.target_indicator_active) return;
    g_cursor.target_indicator_active = 0;
    cursor_update_visibility();
}

void Cursor_ShowMenuFocusAt(i32 x, i32 y) {
    u8* mouse = *(u8**)GLOBAL_MOUSE_MANAGER;
    if (!g_cursor.controller_owner) return;
    if (!Runtime_PtrOk(mouse) || !g_cursor.game_set_cursor_pos) return;

    *(i32*)(mouse + MOUSE_POS_X) = x;
    *(i32*)(mouse + MOUSE_POS_Y) = y;
    cursor_set_ui_position(x, y);
    g_cursor.menu_focus_indicator_active = 1;
    cursor_update_visibility();
}

/* 原版自己有选择框的两项窗口只移动命中点，不额外画手形。 */
void Cursor_MoveHiddenSelectionAt(i32 x, i32 y) {
    u8* mouse = *(u8**)GLOBAL_MOUSE_MANAGER;

    Cursor_ClaimForControllerNavigation();
    if (!Runtime_PtrOk(mouse) || !g_cursor.game_set_cursor_pos) return;

    *(i32*)(mouse + MOUSE_POS_X) = x;
    *(i32*)(mouse + MOUSE_POS_Y) = y;
    cursor_set_ui_position(x, y);
    g_cursor.menu_focus_indicator_active = 0;
    cursor_update_visibility();
}

void Cursor_HideMenuFocusImmediately(void) {
    if (!g_cursor.menu_focus_indicator_active) return;
    g_cursor.menu_focus_indicator_active = 0;
    cursor_update_visibility();
}

int Cursor_ControllerOwnsPointer(void) { return g_cursor.controller_owner; }

void Cursor_Shutdown(void) {
    g_cursor.controller_owner = 0;
    g_cursor.target_indicator_active = 0;
    g_cursor.menu_focus_indicator_active = 0;
    g_cursor.mouse_mode_active = 0;
    g_cursor.investigation_session_active = 0;
    g_cursor.left_stick_activity_latched = 0;
    g_cursor.effective_visible = 1;
    Cursor_ResetClicks();
}
