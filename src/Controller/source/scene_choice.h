#ifndef CASTLE_PAD_SCENE_CHOICE_H
#define CASTLE_PAD_SCENE_CHOICE_H

#include "cursor.h"

/*
 * scene_choice.h
 *
 * 这一模块只负责公共剧情消息引擎的两种原版选项：
 * - mode=2：MSG /q 产生的纵向多行回答；
 * - mode=3：MSG /z 产生的固定“是 / 否”。
 *
 * 两种窗口都由 RPG.exe 自己绘制焦点并提交剧情结果。插件只移动隐藏命中坐标，
 * 所以画面上不会额外出现第二套插件鼠标指针。
 *
 * 它和菜单里的 ConfirmDialog 必须分开：
 * - ConfirmDialog 是 0x4272C0 / 0x4276F0 两种 Button 类；
 * - 本模块是 0x403E30 公共消息分发器进入 0x404600 的剧情鼠标选择状态。
 *
 * 用户截图确认 mode=2/mode=3 都会绘制原版焦点。手柄只移动隐藏命中坐标，不能额外显示一只手形。
 * 两者视觉上都像“是/否”，但底层协议完全不同，所以这里只共享手柄语义，不强行共享业务代码。
 */

int SceneChoice_Install(void);
void SceneChoice_Update(void);
/* 更高模态层出现时只维护已发出的 LEFTUP，不读取新的选择输入。 */
void SceneChoice_MaintainPulse(void);
void SceneChoice_OnPointerTakeover(CursorTakeoverEvent event_type);
int SceneChoice_IsActive(void);

#endif /* CASTLE_PAD_SCENE_CHOICE_H */
