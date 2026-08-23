#ifndef CASTLE_PAD_CONTROL_MODES_H
#define CASTLE_PAD_CONTROL_MODES_H

#include "cursor.h"

/*
 * Back常驻鼠标、自由地图RT临时鼠标、自由地图LT调查的唯一状态裁决层。
 * 既有菜单只看到“未被模式层捕获”的r36语义，不能自行猜测当前指针模式。
 */
void ControlModes_Initialize(void);
CursorTakeoverEvent ControlModes_Update(void);
void ControlModes_OnPhysicalMouseTakeover(void);
int ControlModes_BlocksMapMovement(void);

#endif /* CASTLE_PAD_CONTROL_MODES_H */
