#ifndef CASTLE_PAD_CURSOR_H
#define CASTLE_PAD_CURSOR_H

#include "platform.h"

/*
 * cursor.h
 *
 * Cursor 模块只管理“鼠标/光标所有权”，不管理任何菜单焦点。
 * 业务模块通过返回事件得知实体鼠标是否接管；模式裁决由 ControlModes 单独负责，
 * 然后各自清理自己的导航状态。这样 Cursor 不需要 include Battle/Save/Title。
 */

typedef enum CursorTakeoverEvent {
    CURSOR_TAKEOVER_NONE = 0,
    CURSOR_TAKEOVER_RIGHT_STICK,
    CURSOR_TAKEOVER_PHYSICAL_MOUSE
} CursorTakeoverEvent;

int Cursor_InstallHooks(void);

/* 每个 worker tick 在 PadInput_Poll() 之后调用。 */
CursorTakeoverEvent Cursor_Update(void);

/* Back/RT/LT 共用的可靠鼠标桥：mouse_event + 原版 GetKeyState，保持约 48ms。 */
void Cursor_PulseLeftClick(void);
void Cursor_PulseRightClick(void);
void Cursor_ResetClicks(void);

/* ControlModes 已经裁决状态后，才调用这些不解释按键语义的低层会话接口。 */
void Cursor_SetMouseModeSession(int active);
int Cursor_MoveMouseSticks(void);
int Cursor_MouseModeActive(void);
int Cursor_InvestigationSessionActive(void);
void Cursor_SetInvestigationSession(int active);
int Cursor_MoveInvestigationRightStick(void);
int Cursor_GetPointerPosition(i32* x, i32* y);
int Cursor_MoveControllerAt(i32 x, i32 y);

/* 任何真正的手柄菜单/Target 导航发生时调用，取得光标所有权并按配置隐藏普通鼠标。 */
void Cursor_ClaimForControllerNavigation(void);

/* Target 专用：同步原版 MouseManager 与 Windows cursor，并保持目标提示光标可见。 */
void Cursor_ShowTargetAt(i32 x, i32 y);
void Cursor_HideTargetImmediately(void);

/*
 * 某些原版界面（例如“五内”）完全依赖鼠标位置告诉玩家“现在操作的是哪一个图标”。
 * 手柄 Adapter 可以把普通原版鼠标临时显示在一个明确的屏幕坐标，用作可见焦点指示。
 * 这和 Battle Target 指示器分开记状态，避免离开菜单时误把战斗目标光标一起关掉。
 */
void Cursor_ShowMenuFocusAt(i32 x, i32 y);

/*
 * 有些原版界面虽然用真实鼠标坐标做命中，但自己已经会画“当前选择框”。
 * 这种界面只需要把隐藏的原版鼠标移动到命中区，不能再额外显示手形，否则会出现两套焦点视觉。
 * 典型用途就是公共剧情 mode=2 多行回答和 mode=3“是/否”。
 */
void Cursor_MoveHiddenSelectionAt(i32 x, i32 y);

void Cursor_HideMenuFocusImmediately(void);

/* 实体鼠标接管时，插件不再强制菜单视觉；供业务模块判断/日志。 */
int Cursor_ControllerOwnsPointer(void);

/* DLL 卸载时必须把人工按住的鼠标键全部释放，避免系统留下“鼠标一直按着”的状态。 */
void Cursor_Shutdown(void);

#endif /* CASTLE_PAD_CURSOR_H */
