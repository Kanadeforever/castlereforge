#ifndef CASTLE_PAD_CONFIRM_DIALOG_H
#define CASTLE_PAD_CONFIRM_DIALOG_H

#include "platform.h"

/*
 * confirm_dialog.h
 *
 * 本模块保留 refactor10/refactor11 的“双按钮窗口候选控制”历史实现。
 * 重要：用户后续实机已经证明“选择框不能先按外观抽象成通用控制器”；
 * refactor12 的公共 SelectionUI 路线也已经回滚。因此本文件现在只是历史候选，
 * 不能在后续主菜单开发中被当成万能 Yes/No 基类继续扩张。
 *
 * 历史教训：
 * - refactor8 用“最近看见两只按钮 HitTest”猜弹窗，误伤标题、SaveSlot、Battle；
 * - refactor9 改成只 Hook 8 个直接 open CALL，隔离成功，但实机测试的询问框并没有经过这些入口，
 *   因而新功能完全没有触发。
 *
 * refactor10 起曾改为观察两种双按钮类的明确 vtable Update 槽；refactor11 保留这一层：
 * - 0x460C20 -> 0x4272C0；
 * - 0x460C40 -> 0x4276F0。
 *
 * 只有对象自己正在通过这个真实 Update 运行，并且 +0x579 表示可见、+0x590 尚未产出结果时，
 * 它才会成为这个“历史候选 ConfirmDialog Context”。Battle 和标题 SaveSlot 的封版 owner 继续明确排除。
 * 这只能说明历史候选的隔离边界，不能证明游戏其它视觉相似窗口使用相同协议。
 */

int ConfirmDialog_InstallHooks(void);
void ConfirmDialog_Update(void);
void ConfirmDialog_OnPointerTakeover(void);

/* 当前是否存在一个被这个历史双按钮候选层登记的原版对象。 */
int ConfirmDialog_IsActive(void);

/* UiBridge 在 Battle / SaveSlot 之后调用，只维持历史候选对象自己的原版按钮高亮。 */
u8 ConfirmDialog_FilterHit(void* button, int* handled);

#endif /* CASTLE_PAD_CONFIRM_DIALOG_H */
