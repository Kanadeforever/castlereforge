# Castle_SaveEnhance v0.1.0-test7 安装与 INI 配置说明

## 安装

把包里的 `mods` 合并到游戏目录，最终：

```text
游戏目录\mods\asi\Castle_SaveEnhance.asi
游戏目录\mods\asi\Castle_SaveEnhance.ini
```

如果要声音，自己准备 WAV：

```text
游戏目录\mods\asi\Castle_SaveEnhance\SaveOK.wav
```

不要同时保留旧 `AnytimeSave.asi`，SaveEnhance 已经包含它的安全保存主线。

从源码构建时直接运行 `src\SaveEnhance\build.bat`。最终只向仓库根 `build` 输出：

```text
Castle_SaveEnhance.asi
Castle_SaveEnhance.ini
```

INI 来自 `src\SaveEnhance\templete\Castle_SaveEnhance.ini`。脚本不复制 Markdown；
`src\SaveEnhance\_build` 和 `src\SaveEnhance\tools\__pycache__` 在成功或失败后都会清理。

## test3 启动生命周期

从 RuntimeSDK 适配版开始，`DllMain` 不执行正式业务初始化。Castle Mod Loader 在全部 ASI
完成 LoadLibrary/IAT 后第二遍调用下列导出；其它普通 ASI Loader 由 SDK Entry Gate 触发同一
幂等 Bootstrap：

```text
InitializeASI
```

只有进入这个正式入口以后，SaveEnhance 才读取 INI、预检查机器码并安装 Hook。正常日志必须依次看到：

```text
[装载] 已由 Castle Mod Loader 的 InitializeASI 生命周期进入正式初始化。
[启动] 开始目标机器码预检查与兼容性检查。
[启动] SaveEnhance 完整 Hook 安装成功：...
[状态] SaveEnhance 已完整安装，可以开始实机功能测试。
```

如果日志在这些行之前停止，不要继续拿存档功能做测试，应先按“插件未完整安装”处理。

## 槽位

- 0：Quick Save，普通菜单只读；
- 1~90：普通手动存档；
- 91~99：滚动自动档，普通菜单只读。

## `[Quick]`

### `Enable`

- `1`：F5/F9 与可选 Controller 快捷操作开启；
- `0`：关闭 Quick 功能；普通安全存档和 AutoSave 不受影响。

### `ControllerEnable`

- `1`：尝试 Castle_PadSupport API v1；
- `0`：只使用键盘。

Controller 不存在不会导致插件启动失败。

### `QuickLoadPresses`

允许 2 或 3，默认 2。

F9 与 RB+Start 共用计数。

### `QuickLoadWindowMs`

默认 1200ms；有效范围 300~3000。

超过窗口，前面确认次数作废。

## `[AutoSave]`

### `Enable`

是否启用 91~99 自动档。

### `SaveOnSceneChange`

真正换图后，等新图建立安全锚点再保存。

第一次看见地图或刚快速读档后只建立 baseline，不会立即覆盖自动档。

### `IntervalMinutes`

- `0`：关闭定时；
- `1~1440`：分钟间隔。

## `[Sound]`

字段：


### `Volume`

SaveEnhance 自己的外置 WAV 音量，默认 `70`，范围 `0~100`：

- `0`：静音；
- `1~99`：对 PCM WAV 的内存副本做振幅缩放，不改磁盘文件；
- `100`：不解析文件，直接由 Windows 原样播放。

`Volume<100` 推荐使用 16-bit PCM WAV。PCM 8/16/24/32-bit 和 extensible PCM 支持缩放；其它压缩/float WAV 为避免误改音频数据会静默禁用。音量只影响 SaveEnhance，不影响游戏 BGM/SFX 和 Windows 总音量。

```ini
QuickSaveSuccess=
QuickSaveFailed=
QuickLoadConfirm=
QuickLoadSuccess=
QuickLoadFailed=
AutoSaveSuccess=
AutoSaveFailed=
```

只填单个 WAV 文件名，例如：

```ini
QuickSaveSuccess=save_ok.wav
QuickSaveFailed=save_fail.wav
QuickLoadConfirm=confirm.wav
```

文件放：

```text
mods\asi\Castle_SaveEnhance\save_ok.wav
```

以下情况全部静默：

- 空白；
- 非 `.wav`；
- 带路径/绝对路径；
- 文件不存在；
- winmm 不可用；
- 播放失败。

声音不会改变存档/读档结果。

## 日志

运行后同目录生成：

```text
mods\asi\Castle_SaveEnhance.log
```

实机反馈时请同时提供日志和复现步骤。


## test7 自动槽内部状态文件

`NextAutoSlot` **不写入 INI**。插件把它保存到游戏存档目录：

```text
游戏目录\multimedia\save\.NEXTAUTOSLOT
```

- `RPG.exe` 位于 `游戏目录\exe`，因此代码从 EXE 目录追加 `..\multimedia\save\.NEXTAUTOSLOT`；
- 写入前用 `CreateDirectoryW` 创建缺失的 `multimedia\save` 最后一级目录；
- 文件名以点开头、没有后缀；不会额外设置 Windows Hidden 属性；
- 内容固定为 `091`~`099` 三个 ASCII 字节，没有换行；
- 91~99 有空槽时忽略游标，优先填最低空槽；
- 91~99 全满时覆盖文件记录的下一槽，成功并经游戏文件层回读确认后推进；
- 99 后回到 91；
- 文件缺失、损坏或值超范围时安全回到 91；
- 状态文件写失败不会否定刚完成的 TSF，但日志会提示重启后可能从 91 开始。

test6 已实机确认 Win32 小文本写入本身成功，但错误写到了 `exe\Save`。test7 会在新路径无
有效文件时兼容读取该旧游标；下一次正确写入后删除旧 `.NEXTAUTOSLOT`，但不会删除旧目录。
成功时日志应出现：

```text
[自动槽状态] 已写入 multimedia\save\.NEXTAUTOSLOT，下一候选=92。
```

这个文件只是自动档轮换顺序，不包含 TSF、剧情或 GameVar。不要再向 INI 添加
`[Internal]` 或 `NextAutoSlot`。
