#ifndef CASTLE_PAD_SYNTHESIS_H
#define CASTLE_PAD_SYNTHESIS_H

#include "platform.h"
#include "cursor.h"

/*
 * synthesis.h
 *
 * 炼化拥有两层物品列表，但两层都继续使用 RPG.exe 自己的 ButtonEvent 状态机。
 * 本模块只负责把手柄语义映射到“哪个真实按钮应该返回 code=1/2”。
 * 不直接写当前页、当前行、当前类别、可炼化标志或炼化结果。
 */

int Synthesis_InstallHooks(void);
void Synthesis_Update(void);
void Synthesis_OnPointerTakeover(CursorTakeoverEvent event_type);
int Synthesis_Active(void);

#endif /* CASTLE_PAD_SYNTHESIS_H */
