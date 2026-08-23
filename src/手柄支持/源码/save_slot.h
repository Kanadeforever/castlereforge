#ifndef CASTLE_PAD_SAVE_SLOT_H
#define CASTLE_PAD_SAVE_SLOT_H

#include "platform.h"

/*
 * SaveSlot 是“存读档槽位 UI 控制器”，而不是“标题读档控制器”。
 * 它只认识传入的原版 SaveSlot 对象，因此以后游戏内天書/存档点可以复用同一模块。
 */

typedef enum SaveSlotView {
    SAVE_VIEW_NONE = 0,
    SAVE_VIEW_SLOTS,
    /* SaveSlot 自己直接挂载的历史双按钮窗口；标题读档等旧路径继续使用。 */
    SAVE_VIEW_POPUP,
    /* mode=0/2 选中槽位后，SaveSlot+0x5A4 打开的“存档/读档/取消”三项窗口。 */
    SAVE_VIEW_ACTION,
    /* 三项窗口选择存档/读档以后，action+0x5B4 打开的第二层 Yes/No。 */
    SAVE_VIEW_ACTION_POPUP
} SaveSlotView;

int SaveSlot_InstallHooks(void);
/* 共享 SaveSlot 独立预检并安装三项动作窗口两处 CALL；不依赖天书或存档点任一 owner。 */
int SaveSlot_InstallActionHooks(void);

/* Frontend 发现一个 SaveSlot 对象开始显示时调用。from_pad 表示是否由手柄从标题菜单进入。 */
void SaveSlot_Begin(u8* save_slot, int from_pad);
void SaveSlot_End(void);
void SaveSlot_Update(u8* save_slot);

SaveSlotView SaveSlot_DetectView(u8* save_slot);
int SaveSlot_IsControllerActive(void);

/* 右摇杆/实体鼠标接管后，只清手柄导航，不改变原版 SaveSlot 自己的状态。 */
void SaveSlot_OnPointerTakeover(void);

/* 给 UiBridge 的通用双按钮 HitTest 分发使用。handled=1 表示本模块已经决定返回值。 */
u8 SaveSlot_FilterPopupHit(void* button, int* handled);

/*
 * 询问某只双按钮对象是否就是“当前共享 SaveSlot 直接挂载的确认框”。
 * ConfirmDialog 用这个所有权门让路，避免标题、天书或存档点的同一只弹窗被两套控制器同时接管。
 */
int SaveSlot_OwnsDirectPopup(void* popup);

#endif /* CASTLE_PAD_SAVE_SLOT_H */
