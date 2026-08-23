#ifndef CASTLE_PAD_INN_H
#define CASTLE_PAD_INN_H

#include "platform.h"
#include "cursor.h"

/*
 * inn.h
 *
 * 客栈根菜单只负责“诸态 / 炼化 / 歇息”这三个原版按钮的手柄导航。
 * 它绝对不接管三项按钮背后的业务：
 * - 诸态仍由 RPG.exe 自己创建主 Interface；
 * - 炼化仍由 RPG.exe 自己创建炼化对象；
 * - 歇息仍由 RPG.exe 自己进入剧情、恢复 HP/MP 等流程。
 *
 * 用户已经明确：这个三项根菜单没有原版 B 取消入口。
 * 因此本模块只实现 ↑/↓ + A，B 在这里仅被吞掉，不能凭空制造“退出客栈”。
 */

int Inn_InstallHooks(void);
void Inn_Update(void);
void Inn_OnPointerTakeover(CursorTakeoverEvent event_type);
int Inn_Active(void);

/* UiBridge 的统一 HitTest 分发器会调用本函数，让手柄焦点拥有稳定的原版高亮。 */
u8 Inn_FilterButtonHit(void* button, int* handled);

#endif /* CASTLE_PAD_INN_H */
