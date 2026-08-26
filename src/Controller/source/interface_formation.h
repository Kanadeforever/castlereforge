#ifndef CASTLE_PAD_INTERFACE_FORMATION_H
#define CASTLE_PAD_INTERFACE_FORMATION_H

#include "platform.h"
#include "cursor.h"

/*
 * interface_formation.h
 *
 * 主 Interface state6“阵形”页面的独立手柄 Adapter。
 *
 * 页面业务完全复用 RPG.exe 自己的 8 个阵位 Button：
 * - D-Pad：只改变插件的空间焦点，并移动原版鼠标图示；
 * - A：给当前阵位 Button 注入原版 code=2；第一次选择角色，第二次选择目标阵位并由原版交换；
 * - B：若原版正处于“已选来源、等待目标”的状态，则给当前阵位 Button 注入 code=1 取消；
 *      若没有进行中的阵形交换，则不吃 B，继续交给 InterfaceShell 返回上一层/退出。
 *
 * 真正的二维方向算法在 spatial_neighbor.c，不复制 Battle Target 的另一套实现。
 */

int InterfaceFormation_InstallHooks(void);
void InterfaceFormation_Update(void);
void InterfaceFormation_OnPointerTakeover(CursorTakeoverEvent event);

#endif /* CASTLE_PAD_INTERFACE_FORMATION_H */
