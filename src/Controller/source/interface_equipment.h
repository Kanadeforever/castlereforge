#ifndef CASTLE_PAD_INTERFACE_EQUIPMENT_H
#define CASTLE_PAD_INTERFACE_EQUIPMENT_H

#include "platform.h"

/*
 * interface_equipment.h
 *
 * 主 Interface state4“及身/装备”页面的独立手柄 Adapter。
 *
 * 这份头文件故意只暴露两个生命周期入口：安装 Hook 与每帧 Update。
 * 外部模块不能直接读取/修改装备页的当前栏位、页码或行号；那些细节全部留在
 * interface_equipment.c 内部，防止以后别的页面为了“省几行代码”跨模块碰 state4 私有状态。
 */

/* 校验 state4 原版协议并安装 6 个页面专属 ButtonEvent CALL。 */
int InterfaceEquipment_InstallHooks(void);

/* worker 每 tick 调用；只有主 Interface 当前 state==4 时才真正处理 A/X/↑/↓。 */
void InterfaceEquipment_Update(void);

#endif /* CASTLE_PAD_INTERFACE_EQUIPMENT_H */
