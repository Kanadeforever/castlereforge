#ifndef CASTLE_PAD_INTERFACE_ITEMS_H
#define CASTLE_PAD_INTERFACE_ITEMS_H

#include "platform.h"
#include "cursor.h"

/*
 * interface_items.h
 *
 * 这是主 Interface 的 state2“法宝/道具”页面专属 Adapter。
 *
 * 重要边界：
 * - 它只认识 state2 自己的 6 个子类型、8 行列表、翻页、X 特殊按钮和本页产生的两个双按钮弹窗；
 * - 它不负责 LB/RB 大类、左右角色、根层 B，这些继续由已经实机验收的 InterfaceShell 负责；
 * - 它不把本页弹窗重新包装成“全游戏通用 Yes/No”。相同结构只有在以后全部页面完成后才允许重新评估是否抽公共层。
 */

/*
 * 先做 state2 独立协议预检，再安装本页 6 个列表/子类/特殊按钮 ButtonEvent CALL。
 * 本页的双按钮弃置/分发弹窗使用 RPG.exe 第二类 0x4276F0 Update；其 2 个 HitTest + 2 个 Event CALL
 * 由 plugin 启动阶段恢复的 ConfirmDialog_InstallHooks() 统一接入 UiBridge，但业务焦点/A/B 仍由本 Adapter 独占。
 */
int InterfaceItems_InstallHooks(void);

/* worker 每 tick 调用；只有 Interface state==2 时才读取本页按键。 */
void InterfaceItems_Update(void);

/* 右摇杆鼠标/实体鼠标接管时，撤销手柄强制弹窗焦点和未完成的页面导航握手。 */
void InterfaceItems_OnPointerTakeover(CursorTakeoverEvent event);

/* 给历史 ConfirmDialog 做隔离：本页自己的弹窗活动时，历史通用候选层必须完全让路。 */
int InterfaceItems_AnyPopupActive(void);

/*
 * UiBridge 的共享 ButtonHitTest CALL 会先询问本页是否要提供手柄焦点视觉。
 * handled=1 表示返回值已经由本页决定；handled=0 表示必须继续交给后面的模块/原版。
 */
u8 InterfaceItems_FilterPopupHit(void* button, int* handled);

/* UiBridge 在原版 HitTest 返回后把真实命中结果回报给本页，用于让 A 与屏幕视觉焦点严格一致。 */
void InterfaceItems_ObservePopupHit(void* button, u8 hit_value);

#endif /* CASTLE_PAD_INTERFACE_ITEMS_H */
