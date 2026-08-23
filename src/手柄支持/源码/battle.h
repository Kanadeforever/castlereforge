#ifndef CASTLE_PAD_BATTLE_H
#define CASTLE_PAD_BATTLE_H

#include "platform.h"
#include "cursor.h"

/*
 * Battle 模块完整承接 dev15→dev20 已冻结的战斗手柄体验。
 * 新架构里 Battle 只吃“输入语义”和原版 UI 对象，不直接拥有 SDL3/Win32 鼠标底层。
 */
int Battle_InstallHooks(void);
void Battle_Update(void);
void Battle_OnPointerTakeover(CursorTakeoverEvent event);
int Battle_AnyUiActive(void);

/* UiBridge 在原版 HitTest 调用点询问 Battle 是否要强制某个真实 Button 的视觉命中。 */
u8 Battle_FilterButtonHit(void* button, int* handled);

#endif /* CASTLE_PAD_BATTLE_H */
