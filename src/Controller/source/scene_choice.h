#ifndef CASTLE_PAD_SCENE_CHOICE_H
#define CASTLE_PAD_SCENE_CHOICE_H

#include "cursor.h"

/*
 * scene_choice.h
 *
 * 这一模块只负责“公共剧情消息引擎 mode=3 的两项选择”。
 * 用户实机截图里的客栈“是 / 否”，以及存档点等同类剧情选择，都属于我们本轮要覆盖的范围。
 *
 * 它和菜单里的 ConfirmDialog 必须分开：
 * - ConfirmDialog 是 0x4272C0 / 0x4276F0 两种 Button 类；
 * - 本模块是 0x403E30 公共消息分发器进入 0x404600 的剧情鼠标选择状态。
 *
 * 用户补充截图确认 mode=3 自己会画“是/否”的选择框。手柄只移动隐藏命中坐标，不能额外显示一只手形。
 * 两者视觉上都像“是/否”，但底层协议完全不同，所以这里只共享手柄语义，不强行共享业务代码。
 */

int SceneChoice_Install(void);
void SceneChoice_Update(void);
/* 更高模态层出现时只维护已发出的 LEFTUP，不读取新的选择输入。 */
void SceneChoice_MaintainPulse(void);
void SceneChoice_OnPointerTakeover(CursorTakeoverEvent event_type);
int SceneChoice_IsActive(void);

#endif /* CASTLE_PAD_SCENE_CHOICE_H */
