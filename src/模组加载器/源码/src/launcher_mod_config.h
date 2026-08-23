#ifndef CASTLE_LAUNCHER_MOD_CONFIG_H
#define CASTLE_LAUNCHER_MOD_CONFIG_H

#include "platform.h"

/*
 * GUI 最多展示 256 个 ASI + 256 个 Overrides。
 * 这个上限故意和 Core 的 MAX_MOD_ITEMS_ 完全一致，避免 GUI 能排出的配置超过游戏内 Loader 能读取的范围。
 */
#define LAUNCHER_MOD_MAX_ITEMS 256u

/* 两类 Mod 的排序语义不同，所以 UI 和配置层都明确分开。 */
typedef enum LauncherModKind_ {
    LAUNCHER_MOD_ASI = 1,
    LAUNCHER_MOD_OVERRIDE = 2
} LauncherModKind_;

typedef struct LauncherModItem_ {
    WCHAR name[MAX_PATH_];
    int enabled;

    /*
     * present=1 表示磁盘对象当前存在。
     * usable=1 表示它本轮真的满足 Loader 的加载条件：
     *   ASI：文件存在；
     *   Overrides：目录存在，并且递归至少包含一个普通文件。
     * 两个字段拆开后，GUI 可以把“目录存在但内容为空”和“目录完全缺失”显示成不同状态。
     */
    int present;
    int usable;

    /*
     * has_ini=1 只用于 ASI：表示 mods\asi 中存在与该插件同名的 .ini。
     * 例如 Controller.asi 对应 Controller.ini。GUI 只有在这个字段为 1 时才画“编辑”按钮。
     * Overrides 没有“同名 INI 编辑”语义，所以该字段固定为 0。
     */
    int has_ini;
} LauncherModItem_;

/*
 * 从 mods.ini 和磁盘重新建立 GUI 模型，并执行与 Core 同语义的自动发现：
 *   - 新 ASI / 有内容的新 Overrides 自动追加；
 *   - 缺失旧条目保留；
 *   - 空 Overrides 不新增；
 *   - 模板目录不存在时创建一次，存在时完全忽略。
 *
 * 如果发现新项目，本函数会立即用临时文件 + 原子替换把它们写回 mods.ini。
 */
int LauncherModConfig_LoadAndScan(const WCHAR* mods_root);

/* 当前模型只读访问。返回 NULL 表示索引越界。 */
UINT LauncherModConfig_GetCount(LauncherModKind_ kind);
const LauncherModItem_* LauncherModConfig_GetItem(LauncherModKind_ kind, UINT index);

/*
 * 修改启用状态或排序后立即原子保存。
 * 任何保存失败都会把内存模型回滚到修改前状态，因此 GUI 与磁盘不会长期分叉。
 */
int LauncherModConfig_SetEnabled(const WCHAR* mods_root, LauncherModKind_ kind, UINT index, int enabled);
int LauncherModConfig_Move(const WCHAR* mods_root, LauncherModKind_ kind, UINT from_index, UINT to_index);

/*
 * 删除“磁盘对象已经缺失”的陈旧配置项。
 * 这个操作只删除 mods.ini 里的对应条目，不会删除任何真实文件或目录；调用前仍会再次检查 present=0。
 * 如果历史配置里同名键重复出现，为防止删掉第一条后第二条重新接管，本函数会把该类型下所有同名有效键一并移除。
 */
int LauncherModConfig_RemoveMissing(const WCHAR* mods_root, LauncherModKind_ kind, UINT index);

/* 手工点击“重新扫描”时使用；等价于再次 LoadAndScan。 */
int LauncherModConfig_Refresh(const WCHAR* mods_root);

/* 最近一次失败的简体中文说明；成功后返回空字符串。 */
const WCHAR* LauncherModConfig_GetLastErrorText(void);

#endif
