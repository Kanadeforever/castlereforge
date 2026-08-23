#ifndef CASTLE_LAUNCHER_GUI_H
#define CASTLE_LAUNCHER_GUI_H

#include "platform.h"

/*
 * 显示 Mod Loader 主 GUI，并运行消息循环。
 * 返回值直接作为 CastleModLoader.exe 的退出码：
 *   0：用户已成功启动游戏；
 *   1：用户直接关闭窗口，没有启动游戏；
 *   其它值：GUI 初始化或配置读取失败。
 */
UINT LauncherGui_Run(void);

#endif
