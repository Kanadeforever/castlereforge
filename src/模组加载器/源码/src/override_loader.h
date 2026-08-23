#ifndef YCR_OVERRIDE_LOADER_H
#define YCR_OVERRIDE_LOADER_H

#include "platform.h"

/*
 * 文件夹覆写层（Overrides）对外接口。
 *
 * 设计原则：
 *   1. mods.ini 的 [Overrides] 从上到下表示从低到高的加载顺序；
 *   2. 真正查文件时反过来从最下面往最上面查，所以“后面的 Mod 覆盖前面的 Mod”；
 *   3. 所有 Mod 根目录固定为 mods\overrides\<Mod名>；
 *   4. Mod 里面必须镜像游戏安装根目录的相对结构，而不是把文件复制到原游戏目录。
 */

/* 清空上一轮配置，并记录 Loader 所在 exe 目录与游戏安装根目录。 */
void OverrideLoader_Reset(const WCHAR* mod_root, const WCHAR* game_exe_dir, const WCHAR* game_root);

/* 按 mods.ini 中出现的物理顺序加入一个覆盖 Mod。enabled=0 的项也保留顺序，但不会参与查找。 */
int OverrideLoader_Add(const WCHAR* mod_name, int enabled);

/*
 * 给 RPG.exe 和几个原版资源 DLL 的 IAT 安装 CreateFile/GetFileAttributes Hook。
 * 返回值是实际改写成功的 IAT 槽位数量；0 不等于一定失败，也可能只是当前没有启用 Overrides。
 */
UINT OverrideLoader_Install(void);

/*
 * v0.2.9 独立游戏运行审计入口。
 * Locale 实体自检通过后调用；让 CreateFileA/W 与 GetFileAttributesA/W 在保持 Overrides 语义的同时，
 * 把原版游戏真实 I/O 送到 mods\game.log。返回本次新增/确认的 IAT 槽位数量。
 */
UINT OverrideLoader_EnableGameAudit(void);

/* 新加载的 ASI 若之后会读取游戏资源，可以把它自己的 IAT 也接到同一 Overrides 层。 */
UINT OverrideLoader_PatchModule(HMODULE module);

/* 供启动日志显示实际启用的文件夹 Mod 数量。 */
UINT OverrideLoader_EnabledCount(void);

#endif /* YCR_OVERRIDE_LOADER_H */
