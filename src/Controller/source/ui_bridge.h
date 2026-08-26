#ifndef CASTLE_PAD_UI_BRIDGE_H
#define CASTLE_PAD_UI_BRIDGE_H

#include "platform.h"

/*
 * ui_bridge.h
 *
 * RPG.exe 很多菜单最终都会调用同一个 0x431380 ButtonEvent。
 * dev20 用一个全局 button/code 临时变量解决“一次性手柄事件注入”。
 * 新架构把它独立成桥接模块：Battle/Save/Result 只提出“请让这个原版按钮返回 code=2”，
 * 桥接层负责等到游戏线程真的调用该按钮时再消费。
 */

int UiBridge_InstallHooks(void);

/* 主 Interface 第一阶段额外的根导航 ButtonEvent CALL；普通地图 Y 已改走独立原版门控，不在这里 Hook。 */
int UiBridge_InstallInterfaceShellHooks(void);

/* 主 Interface state2“法宝/道具”页的 6 个页面专属 ButtonEvent CALL。 */
int UiBridge_InstallInterfaceItemsHooks(void);
/* state3“绝学/法术”页的 4 个 ButtonEvent CALL Hook。 */
int UiBridge_InstallInterfaceSkillsHooks(void);
/* state4“及身/装备”页的 6 个 ButtonEvent CALL Hook。 */
int UiBridge_InstallInterfaceEquipmentHooks(void);
int UiBridge_InstallInterfaceInnerStatsHooks(void);
/* state6 阵形 8阵位循环共用一个 ButtonEvent CALL。 */
int UiBridge_InstallInterfaceFormationHooks(void);
/* state8 机能页五个原版 ButtonEvent CALL。 */
int UiBridge_InstallInterfaceOptionsHooks(void);
/* 客栈三项根菜单：一处 HitTest + 一处 Event。 */
int UiBridge_InstallInnHooks(void);
/* 炼化：顶部类别/退出 + 两层翻页/行/第二层取消，共8处 Event CALL。 */
int UiBridge_InstallSynthesisHooks(void);
/* 商店统一界面：refactor33 顶部、左右列与数量窗主体，共11处 Event CALL。 */
int UiBridge_InstallShopHooks(void);

/* refactor9：在严格询问框 open 协议预检通过后，再安装第二种双按钮 update 的四个 CALL。 */
int UiBridge_InstallConfirmDialogHooks(void);

/*
 * pending 事件必须记录“是谁排的”。
 * refactor17 的迟钝根因就是 Battle 在自己不活动时每 8ms 清一次全局 pending，
 * 把 Interface 刚排进去的 LB/RB/角色/B 事件一起误删了。
 * owner 只用于生命周期清理，不改变 RPG.exe 的 ButtonEvent 协议。
 */
typedef enum UiEventOwner {
    UI_EVENT_OWNER_NONE = 0,
    UI_EVENT_OWNER_BATTLE,
    UI_EVENT_OWNER_SAVE_SLOT,
    UI_EVENT_OWNER_CONFIRM_DIALOG,
    UI_EVENT_OWNER_INTERFACE,
    UI_EVENT_OWNER_INN,
    UI_EVENT_OWNER_SYNTHESIS,
    UI_EVENT_OWNER_SHOP
} UiEventOwner;

void UiBridge_RequestEventOwned(UiEventOwner owner, void* button, int code, const char* chinese_tag);
void UiBridge_ClearEvent(void);
void UiBridge_ClearEventOwned(UiEventOwner owner);
int UiBridge_EventPending(void);
UiEventOwner UiBridge_EventOwner(void);

/*
 * 这个 HitTest hook 同时服务战斗视觉和通用双按钮弹窗。
 * 分发顺序固定为 Battle -> SaveSlot -> 主Interface具体页面 -> 严格活动的历史通用询问框 -> 原版，避免非战斗模块污染已冻结战斗路径。
 */
u8 FASTCALL UiBridge_HookButtonHitTest(void* button, void* unused_edx);

/*
 * 复刻 RPG.exe 0x431310 的 Button 屏幕几何计算。
 * 不能只读取 Button+0x3C/+0x40：原版允许这两个 override 为 0，并在这种情况下从 Sprite frame 取得真实宽高。
 * 五内和阵形都需要“原版认为这个 Button 在屏幕哪里”，因此统一放在 UI 基础桥，而不是各页面各猜一套。
 */
int UiBridge_GetButtonScreenRect(void* button, i32* out_left, i32* out_top, i32* out_right, i32* out_bottom);
int UiBridge_GetButtonScreenCenter(void* button, i32* out_x, i32* out_y);

#endif /* CASTLE_PAD_UI_BRIDGE_H */
