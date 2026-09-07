# Castle_SaveEnhance 安装与 TOML 配置说明

## 安装

```text
游戏目录\mods\asi\Castle_Runtime.dll
游戏目录\mods\asi\Castle_SaveEnhance.asi
游戏目录\mods\asi\Castle_SaveEnhance.toml
```

可选提示音放入：

```text
游戏目录\mods\asi\Castle_SaveEnhance\<文件名>.wav
```

不要同时加载旧 `AnytimeSave.asi`。官方 SaveEnhance 必须依赖 Runtime，但不要求使用 Castle Mod
Loader；其它兼容 x86 ASI Loader 也可加载 Runtime + SaveEnhance。

## 生命周期

DllMain 不读配置、不写日志、不加载声音 DLL、不安装 Hook。ModLoader 第二阶段调用
`InitializeASI`；其它 Loader 由 SDK Entry Gate 触发。正式初始化只走 RuntimeHost：

1. 绑定 Runtime Log；
2. 查询 Input、Save、Module、Path、File、TOML；
3. 读取 TOML；
4. 预检目标机器码；
5. 用 Runtime Hook 事务提交补丁；
6. 向 Runtime Save 登记 0、91～99 禁止普通写入。

缺少或损坏 Runtime 时插件安全停用，不回退私有补丁器。

## 槽位

- 0：Quick Save，普通菜单只读；
- 1～90：普通手动存档；
- 91～99：滚动自动档，普通菜单只读。

SaveAction 只有 Runtime 一个包装层。SaveEnhance 只登记策略；Controller 读取最终原生 disabled，
两者不按插件名互相识别。

## `[Quick]`

### `Enable`

1 开启 F5/F9 和可选手柄快捷键；0 关闭 Quick 功能。普通安全存档和 AutoSave 不受影响。

### `ControllerEnable`

1 读取 Runtime Input 中的 PadSupport 语义动作；0 只用键盘。PadSupport 缺失不会使 SaveEnhance
启动失败，也不会触发 ASI 文件名扫描。

### `QuickLoadPresses`

快速读档所需连续请求次数，允许 2 或 3，默认 2。F9 与 RB+Start 共用计数。

### `QuickLoadWindowMs`

连续请求窗口，默认 1200ms，有效范围 300～3000；超时后计数清零。

## `[AutoSave]`

### `Enable`

1 启用 91～99 滚动自动档；0 关闭自动写入。

### `SaveOnSceneChange`

1 在真正换图后等待新图稳定到安全锚点再保存；0 关闭。第一次看到地图或刚快速读档后只建立
基准，不立即覆盖自动档。默认 0，避免过早覆盖支线条件。

### `IntervalMinutes`

0 关闭定时自动档；1～1440 表示分钟间隔，默认 5。换图开关与定时开关独立。

## `[Sound]`

### `Volume`

SaveEnhance 自己的音量，范围 0～100，默认 50：

- 0：静音；
- 1～99：Runtime File 读取 WAV 后，在内存副本缩放 PCM；
- 100：Runtime Path 构造绝对路径，交给 Windows PlaySoundW 原样播放。

本项不改变磁盘 WAV、游戏音量或 Windows 总音量。Volume<100 推荐 16-bit PCM。

### 七个声音键

- `QuickSaveSuccess`：快速存档成功；
- `QuickSaveFailed`：快速存档真正失败；
- `QuickLoadConfirm`：还需继续确认；
- `QuickLoadSuccess`：快速读档成功；
- `QuickLoadFailed`：快速读档失败；
- `AutoSaveSuccess`：自动存档成功；
- `AutoSaveFailed`：自动存档真正失败。

每个值只允许一个 WAV 文件名，必须放在英文双引号中，例如：

```toml
[Sound]
QuickSaveSuccess = "save_ok.wav"
QuickSaveFailed = "save_fail.wav"
QuickLoadConfirm = "confirm.wav"
```

空字符串、非 WAV、带目录、文件缺失、格式不支持或 winmm 失败都只是不发声，不改变存读档结果。

## 日志

```text
mods\logs\Castle_SaveEnhance.log
```

日志由 Runtime Log 管理，不再写在 ASI 目录。

## `.NEXTAUTOSLOT`

自动槽轮换游标继续位于：

```text
游戏目录\multimedia\save\.NEXTAUTOSLOT
```

内容固定为 `091`～`099` 三个 ASCII 字节，无换行。它只是 91～99 全满后的下一覆盖槽，不含
剧情、GameVar 或 TSF 数据。空槽仍优先填最低槽；99 后回到 91；文件缺失或损坏时回到 91。

该文件不是 `SaveXXX.state`。当前没有插件实际创建共享 `.state`。由于它位于 Runtime File v1
允许的 ASI 根之外，且已有实机兼容语义，本轮保留原路径和三字节格式。

## 构建

`src/SaveEnhance/build.bat` 输出 ASI 和 TOML到仓库根 build。根 build_all 再移动到 mods/asi，
创建音效目录并复制本说明与实机测试清单。中间文件不进入发行目录。
