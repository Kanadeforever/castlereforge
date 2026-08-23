#ifndef CASTLE_MOD_LOADER_H
#define CASTLE_MOD_LOADER_H

#include "platform.h"

/*
 * Mod Loader 管理层被 Core 明确拆成三个阶段。
 * 这样“Overrides 必须早于 Locale/ASI”不是口头约定，而是调用接口本身就强迫调用者按顺序做。
 */

/* 建立外置 mods 根；是否打开 mods\modloader.log 由 CastleModLoader.ini 的 ModLoaderLog 决定。 */
int ModLoader_Begin(HMODULE self_module);

/*
 * 扫描 mods\asi / mods\overrides，生成或补全唯一 mods.ini，
 * 然后把 [Overrides] 优先级表交给文件层并安装文件 API Hook。
 * 返回 1 表示配置和 Overrides 基础层准备完成；0 表示关键初始化失败。
 */
int ModLoader_PrepareOverrides(void);

/* 只做 [ASI] 按配置顺序 LoadLibrary；必须在 Overrides 和 Locale 都准备好以后调用。 */
void ModLoader_LoadAsi(void);

/* 返回 Launcher 已经解析并传入的 GameLog 开关；1=启用原版游戏审计，0=完全不安装审计层。 */
int ModLoader_IsGameLogEnabled(void);

void ModLoader_Shutdown(void);

/* 给其它模块复用同一份中文 UTF-8 日志。 */
void ModLoader_Log(const WCHAR* line);
void ModLoader_LogTwo(const WCHAR* left, const WCHAR* right);
void ModLoader_LogError(const WCHAR* prefix, DWORD error);
void ModLoader_LogOverrideHit(const WCHAR* requested, const WCHAR* replacement);

#endif
