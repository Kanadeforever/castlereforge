#ifndef CASTLE_BACKLOG_SDL_INPUT_H
#define CASTLE_BACKLOG_SDL_INPUT_H

#include "platform.h"

/*
 * 这些数字是 SDL3 的 SDL_GamepadButton 标准枚举值。
 * 配置文件使用 south/east/left_shoulder/dpad_up 之类的可读名字，
 * 只有本输入模块会把名字转换成这些数字；Backlog 业务不直接理解品牌按键布局。
 */
enum SdlStandardButton {
    SDL_BUTTON_SOUTH = 0,
    SDL_BUTTON_EAST = 1,
    SDL_BUTTON_WEST = 2,
    SDL_BUTTON_NORTH = 3,
    SDL_BUTTON_BACK = 4,
    SDL_BUTTON_GUIDE = 5,
    SDL_BUTTON_START = 6,
    SDL_BUTTON_LEFT_STICK = 7,
    SDL_BUTTON_RIGHT_STICK = 8,
    SDL_BUTTON_LEFT_SHOULDER = 9,
    SDL_BUTTON_RIGHT_SHOULDER = 10,
    SDL_BUTTON_DPAD_UP = 11,
    SDL_BUTTON_DPAD_DOWN = 12,
    SDL_BUTTON_DPAD_LEFT = 13,
    SDL_BUTTON_DPAD_RIGHT = 14,
    SDL_BUTTON_MISC1 = 15,
    SDL_BUTTON_RIGHT_PADDLE1 = 16,
    SDL_BUTTON_LEFT_PADDLE1 = 17,
    SDL_BUTTON_RIGHT_PADDLE2 = 18,
    SDL_BUTTON_LEFT_PADDLE2 = 19,
    SDL_BUTTON_TOUCHPAD = 20,
    SDL_BUTTON_MISC2 = 21,
    SDL_BUTTON_COUNT = 22
};

/* 纯文本解析器不要求 SDL3.dll 已经存在；无法识别时返回 -1。 */
int SdlInput_ButtonFromName(const char* name);

/* 动态加载/轮询接口。没有 SDL 或没有手柄都只返回 0，不影响键盘。 */
void SdlInput_Initialize(void);
void SdlInput_Poll(void);
int SdlInput_Down(int button);
void SdlInput_Shutdown(void);

#endif /* CASTLE_BACKLOG_SDL_INPUT_H */
