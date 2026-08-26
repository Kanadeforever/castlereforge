#ifndef CASTLE_PAD_SHOP_H
#define CASTLE_PAD_SHOP_H

#include "platform.h"
#include "cursor.h"

/*
 * shop.h
 *
 * RPG.exe 的各种脚本商店都共用同一个 0x413FA0 控制器；商品差异只是数据，不是菜单结构。
 * 本 Adapter 只把手柄语义投递到原版 ButtonEvent，并提供游戏本来没有的左右列手形焦点。
 */

int Shop_InstallHooks(void);
void Shop_Update(void);
void Shop_OnPointerTakeover(CursorTakeoverEvent event_type);
int Shop_Active(void);

#endif /* CASTLE_PAD_SHOP_H */
