#ifndef CASTLE_LAUNCHER_APP_H
#define CASTLE_LAUNCHER_APP_H

#include "platform.h"

/*
 * Launcher 与 GUI 之间只通过这一小组接口沟通。
 *
 * 这样做的目的，是把“看得见的窗口”和“已经实机验证过的 Pre-Loader 启动链”隔开：
 * GUI 负责让用户改配置、排序 Mod、点击按钮；真正创建 RPG.exe、注入 Early Import、
 * 恢复线程的代码仍然留在 launcher.c 中。以后即使继续改界面，也不需要反复碰底层启动逻辑。
 */

/* 返回已经固定为 CastleModLoader.exe 所在目录下的 mods 相对路径，当前恒为 L"mods"。 */
const WCHAR* LauncherApp_GetModsRoot(void);

/* 返回 Loader 自己的配置文件路径，当前为 mods\CastleModLoader.ini。 */
const WCHAR* LauncherApp_GetLoaderIniPath(void);

/* 读取本轮已经解析好的两个日志开关，供“设置”窗口初始化复选框。 */
int LauncherApp_GetModLoaderLogEnabled(void);
int LauncherApp_GetGameLogEnabled(void);

/*
 * 保存 GUI 中的日志设置。
 * 成功以后同时更新当前 Launcher 内存值，这样用户不需要关闭 GUI 再重开，下一次点“启动游戏”就立即使用新值。
 */
int LauncherApp_SaveLoggingSettings(int modloader_log_enabled, int game_log_enabled);

/*
 * 真正执行 v0.2.11 已经验证过的启动链。
 * 返回 1 表示 RPG.exe 已经成功 Resume；返回 0 表示启动失败，GUI 应保持打开让用户修正问题。
 */
int LauncherApp_StartGame(void);
/*
 * 启动 CastleModLoader.exe 同目录下的 cnc-ddraw config.exe。
 * 只负责打开配置程序，不等待、不关闭 Launcher，也不进入游戏 Pre-Loader 启动链。
 */
int LauncherApp_StartCncConfig(void);

#endif
