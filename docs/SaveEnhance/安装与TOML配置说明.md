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

## `[Visual]`（v0.3.0-test5）

以下六项默认均为1，不改变存档安全门、轮换或手柄控制：

| 键 | 作用 |
|---|---|
| `Notification` | 1显示原版书卷与结果文字，0关闭；完成约2.6秒，失败约4.2秒 |
| `BottomRight` | 1右下角，0右上角，按Runtime Display几何定位 |
| `SlotLabels` | 1在槽号左侧显示“自動/快速”，0关闭；不加“手動” |
| `SlotTextOutline` | 1为“新/自動/快速”加固定一逻辑像素深褐近黑描边，0关闭；独立于笔画加粗 |
| `LatestMark` | 1分别标出最新自动与手动档，0关闭；快速槽不参与 |
| `NewPulse` | 1红“新”缓慢明暗变化但不消失，0常亮红色 |

两个数值微调项，修改后重启游戏生效，均只影响槽位文字，不影响已验收的书卷提示：

| 键 | 默认 / 范围 | 含义 |
|---|---|---|
| `SlotTextWeight` | 0 / 0～200 | 每1额外0.01逻辑像素；25=0.25、50=0.5、100=上版额外1像素、200=2像素 |
| `SlotTextOffsetY` | -2 / -12～12 | 相对原槽号垂直偏移，负数上移；“新”与类型共用基线 |

字重的小数部分通过边缘与背景混色实现，不是三挡；16位色仍有量化。0仍保留原点阵细笔画，不会恢复早期丢笔画缩放。

`SlotTextOutline`改变的是字外轮廓，`SlotTextWeight`改变的是字内笔画，两者独立。描边不另设粗细或颜色参数，
修改后重启生效；未填写新键默认开启。不改变已验收的书卷提示。test5仅增加这个开关，标题右键退出再进的隐藏问题
和光标层级问题仍未修复，见完整接档的已知问题。

字形来自原版Font24，书卷从Sys.dat只读加载（25帧，100ms/帧）。test2槽位文字保笔画缩放并加粗，
提示加完整深褐近黑描边，无气泡、底板或英文。资源/可视服务失败只降级反馈，不影响保存。
原版写盘同步占用游戏线程，无法保证写盘时动画持续播放；同帧结束直接显示真实结果，不为提示推迟存档。
关闭显示仍记录最新槽状态，重新开启可以使用；读取、选中、翻页不清除“新”。

test3修复未加载Widescreen时Display默认成功状态2被误判为失败的问题，不要求安装宽屏。
日志新增一次性`[可视诊断]`，记录回调进入、默认后端及绘制跳过原因，不按每帧刷屏。

test4只在真正进入读档及原版UI关闭/loading阶段隐藏标签，不清除“新”的记录。
光标遮挡通过只读本帧原版图形并保留已绘制的不透明像素实现，不重新画光标、不推进动画、不修改手柄状态。

## `[Sound]`

### `Volume`

SaveEnhance自己的音量，范围0～100，发行模板为50；既有代码在键缺失时回退70，本轮未改变：

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

## `.SAVESTATUS`与旧状态迁移

统一状态位于：

```text
游戏目录\multimedia\save\.SAVESTATUS
```

格式为四行，写出26个ASCII字节、CRLF，读取也接受LF：

```text
SE2
M=014
A=096
N=097
```

M为最新手动槽1～90，A为最新自动槽91～99，999表示该类尚无记录；N为下一自动覆盖槽91～99。
自动与手动各有一个“新”，快速槽只标类型。保存成功且游戏文件层能再次打开该槽才更新；自动同时更新A/N，手动不改A/N。
空槽仍优先填最低编号；99后轮换到91；空槽无“新”，手工替换TSF不会自动重算最新。

有效新文件优先；缺失/无效时迁移旧`.LATESTSLOTS`与真实存档目录`.NEXTAUTOSLOT`，后者无效再读历史exe/Save游标。
新文件原子写入成功才删除旧状态文件，不删除目录或TSF；失败保留旧文件。N不能倒推A，A=999不表示自动存档槽被删除。
写入先用同目录`.SAVESTATUS-TMP`，Flush后替换，失败保留旧元数据、清理临时文件，当前TSF与内存状态不回滚。
全部状态无效时最新槽未知、N=91。不写TOML/注释，不引入通用`.state`，位置仍沿用已验收Win32文本文件路线。

## 构建

`src/SaveEnhance/build.bat` 输出 ASI 和 TOML到仓库根 build。根 build_all 再移动到 mods/asi，
创建音效目录并复制本说明与实机测试清单。中间文件不进入发行目录。
