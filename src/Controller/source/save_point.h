#ifndef CASTLE_PAD_SAVE_POINT_H
#define CASTLE_PAD_SAVE_POINT_H

#include "platform.h"
#include "cursor.h"

/*
 * save_point.h
 *
 * 这个模块的名字叫“存档点”，但它绝不实现存档业务。
 * 它只识别 RPG.exe 在 0x89FCD0 发布的 0x413B00 独立包装对象，
 * 从 wrapper+0x580 取出原版标准 SaveSlot，再把它交给唯一的 save_slot.c。
 *
 * 因此边界非常清楚：
 * - save_point.c：包装层生命周期与输入所有权；
 * - save_slot.c：槽位、分页、A/B、直接确认框的共享控制器；
 * - RPG.exe：真正的存档写入、读取、文件名和完成标志。
 */

int SavePoint_Install(void);
void SavePoint_Update(void);
void SavePoint_OnPointerTakeover(CursorTakeoverEvent event_type);

/*
 * 只要独立包装对象仍存在就返回 1，包括展开/收起动画。
 * 总调度器用它阻止标题、地图、客栈等底层 Context 同帧读取菜单键。
 */
int SavePoint_Active(void);

#endif /* CASTLE_PAD_SAVE_POINT_H */
