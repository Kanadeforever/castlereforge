#ifndef CASTLE_PAD_INTERFACE_TOME_H
#define CASTLE_PAD_INTERFACE_TOME_H

#include "cursor.h"

/*
 * interface_tome.h
 *
 * state7“天书”本身不重新实现存读档界面。
 * RPG.exe 在 Interface+0x654 放的就是与标题读档相同的 SaveSlot 对象，所以本模块只是一个很薄的适配器：
 * 1. 判断 state7 何时进入/离开；
 * 2. 把那个原版 SaveSlot 对象交给已经实机验证过的 save_slot.c；
 * 3. 额外启用 SaveSlot 内嵌“存档/读档/取消”三项窗口的两处原版调用点；
 * 4. 二次 Yes/No 仍交给 ConfirmDialog，绝不复制询问框业务。
 */

int InterfaceTome_InstallHooks(void);
void InterfaceTome_Update(void);
void InterfaceTome_OnPointerTakeover(CursorTakeoverEvent event);

/* Shell 用它判断当前是不是处在天书的三项窗口/二次确认里；这些层级必须阻止 LB/RB/B 穿透到底层。 */
int InterfaceTome_ModalActive(void);
int InterfaceTome_Active(void);

#endif /* CASTLE_PAD_INTERFACE_TOME_H */
