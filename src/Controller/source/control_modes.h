#ifndef CASTLE_PAD_CONTROL_MODES_H
#define CASTLE_PAD_CONTROL_MODES_H

#include "cursor.h"

/*
 * Back常驻鼠标、自由地图RT临时鼠标、自由地图调查的唯一状态裁决层。
 * 调查激活键由 INI 选择：默认按住A，或兼容旧版按住LT。
 *
 * “唯一裁决层”可以理解为学校里只有一个值日班长负责分配教室：
 * Cursor 只负责移动鼠标，Investigation 只负责寻找目标，它们都不能自己宣布
 * “现在进入调查”。只有本模块确认游戏在自由地图、没有菜单冲突、按键边沿合法后，
 * 才会建立模式；这样同一颗A/LT/RT不会被两个模块同时解释成不同动作。
 *
 * 既有菜单只看到“没有被模式层捕获”的输入，不能自行猜测当前指针模式。
 */
void ControlModes_Initialize(void);
CursorTakeoverEvent ControlModes_Update(void);
void ControlModes_OnPhysicalMouseTakeover(void);
int ControlModes_BlocksMapMovement(void);

/*
 * 给 Public API 发布器使用的稳定模式编号。
 * 返回值固定为：0=普通手柄、1=Back常驻鼠标、2=RT临时鼠标、3=调查模式。
 * 这里不把内部 ControlMode enum 的地址或结构体暴露出去；未来内部 enum 即使重排，
 * 只要本函数继续做显式映射，外部 ABI 的数字含义就不会变化。
 */
u32 ControlModes_PublicMode(void);

#endif /* CASTLE_PAD_CONTROL_MODES_H */
