#ifndef CASTLE_BACKLOG_MOUSE_INPUT_H
#define CASTLE_BACKLOG_MOUSE_INPUT_H

#include "platform.h"
#include "CastleRuntime_API.h"

/*
 * mouse_input.h
 *
 * 这一层只负责“看见 Windows 鼠标消息”，不决定这些消息在 Backlog 里代表什么。
 *
 * 它做三件事：
 * 1. 找到属于 RPG.exe 的前台窗口；
 * 2. 把自己的 WndProc 链到游戏当前 WndProc 前面；
 * 3. 只累计滚轮方向和右键按下次数，真正的打开/滚动/退出由 backlog.c 决定。
 *
 * 因为我们保存并继续调用安装前的 WndProc，所以不会直接吞掉其它插件已经安装的窗口过程。
 */

/* 尝试安装窗口过程观察器。窗口还没出现时返回 0，后续可以继续重试。 */
int MouseInput_Initialize(void);

/* 整合模式向 Runtime Window 注册 Observer/Filter，不再私自 SetWindowLongPtr。 */
int MouseInput_InitializeIntegrated(const CastleRuntimeApiV1* runtime_api,
                                    CastlePluginHandle plugin_handle);

/* worker 每个 tick 调一次；如果游戏窗口稍后才出现，会在这里自动补装。 */
void MouseInput_Poll(void);

/*
 * 取走自上次读取以来累计的滚轮格数。
 * 正数 = 向上；负数 = 向下；读取后计数清零。
 */
i32 MouseInput_TakeWheelSteps(void);

/* 取走右键“按下”次数；只关心按下沿，不把按住当成连续退出。 */
u32 MouseInput_TakeRightPressed(void);

/* ASI 正常卸载时尽力把 WndProc 恢复为安装前的链目标。 */
void MouseInput_Shutdown(void);

#endif /* CASTLE_BACKLOG_MOUSE_INPUT_H */
