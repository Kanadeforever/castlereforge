#ifndef CASTLE_PAD_FRONTEND_H
#define CASTLE_PAD_FRONTEND_H

#include "platform.h"
#include "cursor.h"

/*
 * frontend.h
 *
 * refactor1 的 Frontend 范围严格停在 dev20：
 * - 标题根层三项按钮；
 * - 从标题进入的读档 SaveSlot。
 *
 * 它故意不包含 dev20 之后研究过的“游戏内主 Interface / 地图 Y / 天書”等功能。
 * 标题模块只负责识别标题对象、维护标题三项焦点，并把 SaveSlot 对象交给 save_slot.c。
 * 真正的槽位分页、选中动画、A/B 行为不在这里复制第二份。
 */

/* 安装标题三项原生 HitTest 与三个独立 ButtonEvent 调用点的 Hook。 */
int Frontend_InstallHooks(void);

/* 每个 worker tick 更新标题 Context，并在检测到标题读档时把对象交给 SaveSlot 模块。 */
void Frontend_Update(void);

/* 右摇杆或实体鼠标取得指针所有权时，清理仅属于手柄的标题导航状态。 */
void Frontend_OnPointerTakeover(CursorTakeoverEvent event);

/* 返回标题/标题读档当前是否存在，用于插件总调度避免多个 UI Context 同时处理同一输入。 */
int Frontend_AnyUiActive(void);

#endif /* CASTLE_PAD_FRONTEND_H */
