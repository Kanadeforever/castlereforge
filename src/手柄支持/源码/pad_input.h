#ifndef CASTLE_PAD_INPUT_H
#define CASTLE_PAD_INPUT_H

#include "platform.h"

/*
 * pad_input.h
 *
 * 这一层只回答“手柄现在物理上是什么状态”。
 * 它不知道 A 在战斗里是确认，也不知道 B 在存档框里是取消。
 * 这样以后改键、换 SDL 版本、兼容更多手柄时，不需要碰任何菜单业务代码。
 */

typedef enum PadButton {
    PAD_SOUTH = 0,      /* SDL South：Xbox A / Nintendo B 位置。项目通用语义通常是“确认”。 */
    PAD_EAST = 1,       /* SDL East：Xbox B / Nintendo A 位置。项目通用语义通常是“取消”。 */
    PAD_WEST = 2,
    PAD_NORTH = 3,
    PAD_BACK = 4,       /* SDL Back / Xbox View：r37 由 ControlModes 解释为常驻鼠标切换键。 */
    PAD_START = 6,      /* SDL Start：系统/流程类按键；本版只在电影播放时映射为原版 ESC。 */
    PAD_R3 = 8,
    PAD_LB = 9,
    PAD_RB = 10,
    PAD_DPAD_UP = 11,
    PAD_DPAD_DOWN = 12,
    PAD_DPAD_LEFT = 13,
    PAD_DPAD_RIGHT = 14,

    /*
     * SDL3 把 LT/RT 定义成“轴”而不是普通数字按钮。
     * 为了让上层 InputRouter 仍然只处理“按下/持续/松开”三种统一按钮语义，
     * PadInput 在采样时会把触发器轴超过阈值的状态转换成下面两个内部伪按钮位。
     * 15/16 不是传给 SDL_GetGamepadButton 的枚举值，只存在于本插件自己的 bitset 里。
     */
    PAD_LT = 15,
    PAD_RT = 16
} PadButton;

typedef enum PadAxis {
    PAD_AXIS_LEFT_X = 0,
    PAD_AXIS_LEFT_Y = 1,
    PAD_AXIS_RIGHT_X = 2,
    PAD_AXIS_RIGHT_Y = 3,
    PAD_AXIS_LEFT_TRIGGER = 4,
    PAD_AXIS_RIGHT_TRIGGER = 5,
    PAD_AXIS_COUNT = 6
} PadAxis;

#define PAD_STICK_DEADZONE 7000

/*
 * SDL3 的标准 Gamepad trigger 范围是 0..32767。
 * 这里取中点 16384 作为“数字按键已经按下”的阈值：
 * - 轻微误触不会切菜单；
 * - 正常按下扳机不需要压到底才生效。
 * 这是物理输入层常量，不是菜单业务逻辑。
 */
#define PAD_TRIGGER_DIGITAL_THRESHOLD 16384

int PadInput_Initialize(void);
void PadInput_Shutdown(void);

/* 每个 worker tick 调一次。返回 1 表示 SDL 层可工作；即使暂时没手柄也不会崩。 */
int PadInput_Poll(void);

int PadInput_Down(PadButton button);
int PadInput_Pressed(PadButton button);
int PadInput_Released(PadButton button);
i16 PadInput_Axis(PadAxis axis);

/*
 * 只回答“这一刻手柄上有没有明显活动”，不赋予按钮任何业务含义。
 * Cursor 用它判断何时把光标所有权交给手柄，从而不必在 Cursor 里再抄一遍 A/B/X/Y 等物理键编号。
 * include_r3=0 时会忽略 R3。Back 由模式裁决层独占，不在通用活动里重复抢所有权。
 * 普通手柄态只让左摇杆参与活动判定；右摇杆已删除常驻功能，静止模式下不会抢回实体鼠标。
 */
int PadInput_HasAnyActivity(int include_r3);

/* 当前前台窗口是否属于 RPG.exe；所有模拟输入都必须先尊重这个 gate。 */
int PadInput_GameForeground(HWND* out_hwnd);

/* 供日志/调试使用。 */
int PadInput_GamepadConnected(void);
HMODULE PadInput_SdlModule(void);

/*
 * SDL_RumbleGamepad 是可选能力；缺少导出时只返回 0，不影响输入。
 * priority 越大越重要：模式回切(2)不能被随后经过目标的短提示(1)截断。
 */
int PadInput_Rumble(u16 low_frequency, u16 high_frequency, u32 duration_ms, int priority);

#endif /* CASTLE_PAD_INPUT_H */
