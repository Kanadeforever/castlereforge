# CastleReforge

> 为《天地劫序传·幽城幻剑录》制作的一系列现代化、兼容性与 QOL 改进模组套组，以及配套的模组加载器。

CastleReforge 是一个面向 Windows 版《天地劫序传·幽城幻剑录》的非官方开源改进项目。

项目以**台湾第三版**作为主要研究与实机测试基线，目标是在尽可能保持原版游戏逻辑、资源和存档兼容性的前提下，改善现代 Windows 环境下的运行、显示和操作体验，并为后续 Mod 开发提供一个相对完整的基础设施。

目前项目主要包括：

* 模组加载器与非繁体中文环境下的台湾繁中 / CP950 运行环境；
* 为本项目内的 ASI 提供公共基础设施的 RuntimeSDK；
* 基于 SDL3 的手柄操控支持；
* 真正扩展游戏世界视野的 16:9 / 21:9 宽屏插件；
* 对话历史、任务追踪与世界任务标记；
* 快速存读档、滚动自动存档和安全扩展存档；
* 基于久经验证的三合一、五合一补丁并继续改进的 BUG 修复、免 CD、最大成长与最大掉宝插件；
* 与上述功能配套的逆向分析、地址记录、测试工具和实机验收文档。

> **本项目不会提供《幽城幻剑录》游戏本体、`RPG.exe` 或受版权保护的原版游戏资源。**

---

## 当前版本

当前所有内容均为开发版，并且尚未经过通关测试，使用前请确保已知晓目前可能出现的问题；

`main` 分支每次更新后会自动构建并覆盖发布 `dev-auto` 开发版。

<h1><a href="https://github.com/Kanadeforever/castlereforge/releases">虽然点击页面右边/最下面的release按钮就能看到，但看起来好像都不是那么想点，那就点这个吧</a></h1>

### 当前总体状态

* RuntimeSDK、8 个官方 ASI、6 份同名 TOML、Quest 数据和 Loader 已纳入十步全量构建；
* 全量构建、专项静态检查和发行目录检查已经通过；
* Backlog、手柄、宽屏和旧安全回退存档等功能各自拥有历史实机验证基线；
* RuntimeSDK、统一 TOML、统一日志、Quest 和最新 SaveEnhance 组合仍需使用最终发行目录完成全插件实机回归；
* FPSUnlock 尚未完成，不进入 `build_all.bat`，也不随 `dev-auto` 发行。

> **静态检查通过不等于实机验收通过。** 每个模块的准确状态以对应“完整接档”和实机清单为准。

| 模块 | 当前交付状态 |
|---|---|
| Castle Mod Loader | dev9 的区域环境、加载与 Overrides 业务基线已封存；Runtime/TOML/日志增量待最终组合回归 |
| RuntimeSDK | 全量构建和宿主测试通过；最新发行组合待实机回归 |
| Controller | 既有业务功能拥有实机基线；RuntimeSDK/TOML 发行组合待最终回归 |
| Backlog | v0.3.4 业务实机封版；RuntimeSDK 发行组合待最终回归 |
| Widescreen | 16:9 等既有业务拥有实机基线；21:9 和最新 RuntimeSDK 候选仍需按清单复核 |
| SaveEnhance | 旧安全回退拥有关键实机正向证据；当前快速存读档、自动档和保留槽版本待完整实机验收 |
| Quest | Windows x86 构建与 51 文件数据合同通过；游戏内完整实机回归待执行 |
| BUGFix / NoCD / MaxGrowthAndDrop | 既有功能有历史验证；当前 RuntimeSDK 发行组合待最终回归 |
| FPSUnlock | 未完成；不进入总构建和发行包 |

---

## 项目组成

### 0. 《幽城幻剑录》本体文件 & cnc-ddraw （至少7.1）

请自备，安装顺序为：

1. 台湾第三版正常安装（建议使用Locale Emulator安装，避免文件在非繁体中文系统下乱码）；
2. 复制运行盘内文件到游戏目录内并且 **不要覆盖** 任何文件（台3版文件比运行盘文件更新，是修复后的版本）；
3. 安装 `Kalpa2_200S.EXE` 补丁（也可以使用本项目内的补丁解包器而不安装，补丁解包器自带防乱码措施）；sha256: `d3c4a61fe9b4857d38fb10c7e57f1b629551e9e02fb93a261b888f2f404abfdf`
4. 安装 `200etc.EXE` 补丁（也可以使用本项目内的补丁解包器而不安装，补丁解包器自带防乱码措施）；sha256: `e9d70f9fa0b38b594ad9b7e22817327cc09f6be23e3bfd9a9768789cf04b2c19`
5. 游戏本体准备完成！此时的 `exe\RPG.exe` 的 sha256: `8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f`
    - 请待机； **千万不要**再安装别的任何内容，之后的所有内容请使用模组加载器。
6. 下载 [cnc-ddraw.zip](https://github.com/FunkyFr3sh/cnc-ddraw/releases/latest) ，并解压`Shaders`文件夹、`cnc-ddraw config.exe`、`ddraw.dll`、`ddraw.ini`到游戏的exe目录内，也就是和 `RPG.exe` 在同一个目录。
7. 准备完毕！

补丁解包、已知乱码文件名修复和游戏目录校验的使用方法见[项目工具箱详细说明](docs/_toolbox/工具详细说明.md)。这些工具不会提供任何原版游戏文件；运行会写入或重命名文件的工具前必须先制作目录副本。

### 1. Castle Mod Loader

CastleReforge 自带为《幽城幻剑录》设计的专用 Mod Loader。

它不只是一个 ASI 加载器，同时负责建立游戏在现代 Windows 下所需要的运行环境。

主要功能：

* 无需修改 Windows 系统区域设置即可建立 **zh-TW / CP950 台湾繁中运行环境**；
* 加载 `mods\asi` 中的 ASI 插件；
* 支持 ASI 加载顺序；
* 支持启用 / 禁用 Mod；
* 支持拖动排序；
* 支持 `mods\overrides` 文件覆写；
* 文件型 Mod 不需要直接覆盖原版游戏文件；
* 自动识别 ASI 的同名 TOML；
* 内置 TOML 编辑器；
* TOML 语法分色；
* 自动换行；
* 保存前结构检查；
* 接受 UTF-8 或 UTF-8 BOM，并保持原 BOM 策略；
* 缺失 Mod 检测与移除；
* 游戏运行审计日志；
* 独立 Launcher GUI。

目录结构：

```text
RPG.exe
CastleModLoader.exe

mods\
├─ CastleLocaleBootstrap.dll
├─ CastleModCore.dll
├─ CastleModLoader.ini
├─ mods.ini                    启停与顺序配置，首次运行时可自动建立
├─ logs\                      Loader、Runtime 与各 ASI 的统一日志目录
├─ asi\
│  ├─ Castle_Runtime.dll
│  ├─ *.asi
│  ├─ 同名 *.toml
│  └─ ASI 自身需要的依赖 DLL
└─ overrides\
```

其中：

```text
mods\asi\
```

用于放置 ASI Mod、对应 TOML、`Castle_Runtime.dll`，以及 ASI 自身需要的依赖 DLL。

Loader 自己的 `CastleModLoader.ini` 和记录 Mod 启停/顺序的 `mods.ini` 仍使用 INI； ASI 的业务配置已经统一迁移为 TOML。

```text
mods\overrides\
```

用于放置文件覆写型 Mod，也就是原本要求覆盖游戏文件的Mod。

Loader 自身不会接管或实现 DirectDraw。

目前项目主要使用 **cnc-ddraw** 作为已验证的 DirectDraw 兼容环境。
并且只推荐使用 [cnc-ddraw](https://github.com/FunkyFr3sh/cnc-ddraw/releases/latest)。

详细资料：

* [Mod Loader 使用说明](docs/MODLoader/使用说明.md)
* [架构与启动时序](docs/MODLoader/架构与启动时序.md)
* [台湾繁中区域环境层说明](docs/MODLoader/台湾繁中区域环境层说明.md)
* [模组目录与配置说明](docs/MODLoader/模组目录与配置说明.md)
* [文件覆写说明](docs/MODLoader/文件覆写说明.md)
* [插件 TOML 编辑器说明](docs/MODLoader/插件TOML编辑器说明.md)
* [关于页面与第三方说明](docs/MODLoader/关于页面说明.md)

---

### 2. RuntimeSDK

`Castle_Runtime.dll` 是全部 CastleReforge 官方 ASI 必需的公共基础引擎。

它统一提供：

* Hook 所有权、链、事务与失败回滚；
* Loader-ready 闸门和公共调度；
* 路径、文件、TOML、日志、外部模块和高精度时钟；
* 输入、游戏状态、存档动作、显示、渲染和 Overlay 协调；
* 插件冲突检测、诊断与安全停用。

ASI 可以脱离 Castle Mod Loader 和其它业务插件，由兼容的 ASI Loader 加载；但必须与 `Castle_Runtime.dll` 位于同一个 ASI 目录。Runtime 缺失、损坏或 ABI 不兼容时，官方插件会安全停用，不会退回各自写入游戏内存的旧路径。

详细资料：

* [运行时协调系统总体设计](docs/runtime/运行时协调系统总体设计.md)
* [RuntimeSDK 完整接档](docs/runtime/截至该版本的完整接档.md)
* [新格式 ASI 插件制作指南](docs/common/新格式ASI插件制作指南.md)
* [RuntimeSDK 接口选择与冲突协调指南](docs/common/RuntimeSDK接口选择与冲突协调指南.md)

---

### 3. 手柄支持

操作见 [手柄控制说明](docs/Controller/手柄控制说明.md) 。

`Castle_PadSupport.asi` 为游戏增加基于 **SDL3** 的现代手柄操控。

它并不是简单地把手柄映射成键盘，而是针对《幽城幻剑录》不同 UI 和游戏状态分别建立操作逻辑。

目前代码已经覆盖包括：

* 标题与主菜单；
* 普通地图探索；
* 对话；
* 是 / 否确认框；
* 多层选择框；
* 战斗；
* 道具；
* 装备；
* 技能；
* 阵型；
* 人物属性；
* 天书；
* 系统选项；
* 商店；
* 合成；
* 客栈；
* 存档点；
* 存档槽；
* 剧情选择；
* Bink / Movie 跳过；
* 鼠标模拟；
* 调查模式；
* 手柄震动。

当前新一代控制模式还包括：

**常驻鼠标模式**

可在不同界面中使用手柄控制游戏鼠标。

**临时鼠标模式**

按住进入，松开退出；双摇杆用于鼠标操作，A / B 对应鼠标左右键。

**调查模式**

用于辅助地图互动与可调查对象操作。

普通状态则继续使用针对原版 UI 逐项适配的手柄操作。

手柄项目对游戏内部大量 UI 状态、ButtonEvent、Scene Event 与原版交互流程进行了单独适配，因此开发版之间可能存在尚待实机验证的改动。

**请不要把“静态检查通过”理解为“实机验收通过”。**

具体版本状态请始终查看：

* [手柄支持完整接档](docs/Controller/截至本版本的完整接档.md)
* [架构设计与模块边界](docs/Controller/架构设计与模块边界.md)
* [已知问题与实机验收说明](docs/Controller/已知问题与实机验收说明.md)
* [测试与回归清单](docs/Controller/测试与回归清单.md)

运行时需要兼容的 **x86 SDL3.dll**。`dev-auto` 发行包会自动附带经过构建流程校验的 32 位 SDL3；手工构建或单独部署 Controller 时需要自行准备。

---

### 4. 对话历史

`Castle_Backlog.asi` 可以在游戏内查看最近的对话历史，并针对原版对话框、姓名框和剧情状态复用原版绘制流程。

目前支持键盘、鼠标滚轮和 Runtime Input 提供的手柄输入；没有 PadSupport 时，键盘和鼠标功能仍可使用。历史业务版本 v0.3.4 已完成剧情中打开、剧情后打开、多条目滚动、自适应间距和关闭恢复的实机回归。当前官方版已经迁入 RuntimeSDK，仍需随最终全插件组合复核。

详细资料：

* [Backlog 使用与配置说明](docs/Backlog/使用与配置说明.md)
* [Backlog 架构与实现说明](docs/Backlog/架构与实现说明.md)
* [Backlog 完整接档](docs/Backlog/完整接档说明.md)
* [Backlog 实机回归清单](docs/Backlog/实机回归清单.md)

---

### 5. 真宽屏插件

`Castle_Widescreen.asi` 为《幽城幻剑录》增加真正的宽屏世界显示。

这不是简单拉伸原版 640×480 画面。

插件保持原版 GUI 的 4:3 坐标和比例，只扩展游戏世界本身的可见范围。

目前支持：

| 模式   |    游戏渲染分辨率 |   比例 |
| ---- | -------: | ---: |
| 默认宽屏 |  854×480 | 16:9 |
| 超宽屏  | 1120×480 | 21:9 |

普通地图中，左右区域会绘制真实的额外游戏世界。

而在：

* 对话；
* 剧情消息；
* 提示；
* 选择框；
* Battle；

等需要保持原版构图的场景中，则恢复中央 **640×480** 的原版 UI。

侧区可以选择：

```toml
[Cinematic]
BlurredSides = 1
```

使用电影式模糊侧区；

或者：

```toml
[Cinematic]
BlurredSides = 0
```

使用纯黑侧区。

进入和退出动画时间同样可以通过 `Castle_Widescreen.toml` 调整。

宽屏插件始终遵循一个基本原则：

> **扩展世界，而不是拉伸 UI。**

因此人物头像、对话框、Battle HUD 和其它原版界面仍保持设计时的比例。

详细资料：

* [宽屏完整接档](docs/Widescreen/截至本版本的完整接档.md)
* [宽屏技术设计与协议](docs/Widescreen/宽屏技术设计与协议.md)
* [实机测试与已知问题](docs/Widescreen/实机测试与已知问题.md)

---

### 6. 存档增强

`Castle_SaveEnhance.asi` 是旧 `AnytimeSave.asi` 的完整升级替代，二者不应同时加载。

当前功能包括：

* 槽 0 快速存档与快速读档；
* F5 快速存档、F9 二次或三次确认快速读档；
* 可选的手柄快速操作；
* 91～99 滚动自动存档；
* 原版 8 页×4 槽扩展为 25 页×4，即 0～99；
* 换图或定时自动保存，并在状态暂时不安全时等待安全锚点；
* 可选的外置 WAV 提示音。

安全扩展存档不会修改原版 TSF 格式，也不会把危险现场强行当成完整世界快照。原版禁止存档时，插件只有在取得可靠安全锚点后才允许写入；再次读取时可能回到当前场景入口或稍早的安全点，而不是保存瞬间的精确坐标。

槽 0 和 91～99 是保留槽，普通存档菜单只允许读取，不允许覆盖。自动轮换状态保存在游戏存档目录的 `.NEXTAUTOSLOT` 三字节文件中；它不是游戏进度，也不是未来规划中的增强 `.state`。

详细资料：

* [SaveEnhance 安装与 TOML 配置说明](docs/SaveEnhance/安装与TOML配置说明.md)
* [SaveEnhance 完整接档](docs/SaveEnhance/完整接档.md)
* [SaveEnhance 实机测试清单](docs/SaveEnhance/实机测试清单.md)

---

### 7. 任务系统

`Castle_Quest.asi` 为原版增加只读任务追踪、任务面板和世界 Marker，不修改原版 GameVar，也不改变 TSF 格式。

当前任务数据库包括：

* 25 个 Quest；
* 146 个原版 Canonical Stage；
* 556 条阶段内 Route；
* 25 份 Base、25 份 Addon 和 1 份 manifest，共 51 个运行 TOML；
* Ctrl+F6～F11 的界面、Marker、热重载和诊断快捷键。

Base 只描述有原版证据的真实任务链；Remastered 新增的提示、Marker 和人工体验步骤放在同名 Addon 中。当前 Addon 完成记录只在本次游戏进程内保存；同一进程读取更早存档时不会自动回滚，跨存档持久化等待未来由 RuntimeSDK 统一提供的 `.state` 服务。

详细资料：

* [Quest 完整接档](docs/Quest/截至该版本的完整接档说明.md)
* [Quest 实机验证清单](docs/Quest/实机验证清单.md)
* [任务制作逐步入门教程](docs/Quest/任务制作逐步入门教程.md)
* [任务与路由维护完整教程](docs/Quest/任务与路由维护完整教程.md)

---

### 8. 独立 QOL / BUG 修复 ASI

这些业务插件可以按需启用，彼此不构成硬依赖，但当前官方版本都需要同目录 `Castle_Runtime.dll`。

#### `BUGFix.asi`

修复已确认的原版问题，目前包括：

* 继承自“汉堂之家”坛友“武英仲”制作的三合一补丁中的“冥狱杀阵”习得问题和抗性显示/数据错位问题；
* 已在 Windows 11 验证的“读档 → 返回标题 → 新游戏”流程中 Legacy Background Controller 双调用路径崩溃。

#### `NoCD.asi`

移除游戏的 CD 检查，继承自“汉堂之家”坛友“武英仲”制作的三合一补丁。

#### `MaxGrowthAndDrop.asi`

继承自“汉堂之家”坛友“武英仲”制作的五合一补丁中的最大成长和最大掉宝。

两个功能可以通过 `MaxGrowthAndDrop.toml` 独立开启或关闭。

详细资料：

* [Extra 当前构建与接档说明](src/Extra/readme.md)
* [其他功能历史使用说明](docs/Extra/文档/使用说明.md)
* [已知限制与风险](docs/Extra/文档/已知限制与风险.md)
* [逆向分析与验证记录](docs/Extra/文档/逆向分析与验证记录.md)

---

## 安装（Windows）

### 推荐方式

建议下载 [dev-auto 开发版](https://github.com/Kanadeforever/castlereforge/releases/tag/dev)，先备份原版存档，再将发布包中的：

```text
CastleModLoader.exe
mods\
```

复制到 `RPG.exe` 所在目录。

发行包已经内置 `Castle_Runtime.dll`、8 个 ASI、6 份同名 TOML、Quest 数据、SaveEnhance 说明和 x86 `SDL3.dll`。打开 `CastleModLoader.exe` 后检查各 Mod 的启用状态与顺序，再启动游戏。

用户自己的覆盖文件类型的 Mod 放在：

```text
mods\overrides\
```

该目录内已经提供空目录结构模板。复制模板、把副本改成 Mod 名称，再将文件按原游戏目录结构放入副本；最后通过 `CastleModLoader.exe` 管理和启动游戏。**绝对不要**直接覆盖原版文件。

## 安装（Android，实验性）

### 推荐方式

目前在 **AYN Odin 3** 上使用[GameNative](https://github.com/utkarshdalal/GameNative/releases/latest)测试，可正常运行，MOD加载器也已经修复，可正常使用。

若你的安卓设备可以运行GameNative并且顺畅使用，那么本模组则理论上没有使用障碍。

配置要求上，本模组并未增加幽城的配置要求。

综上，软件上暂时只支持GameNative，其余如Winlator等组件请自行测试。

另外本插件不对**盖世游戏**及其相关修改版的运行情况做任何支持，任何来自**盖世游戏**及其修改版的bug汇报将会**无差别关闭**。

### GameNative设置说明

> 强烈建议先在电脑上完成游戏安装、文件名处理和显示调试，再把完整目录移至移动设备；移动设备理论上也可以安装，但注意最流行的台三版镜像都是mds/mdf格式，并非ISO。

1. Android上现阶段GameNative内置的cnc-ddraw并不是那么好用，依然需要自备cnc-ddraw；
2. 在GameNative内添加游戏后，点击游戏页面右侧的齿轮，打开选项菜单的 `编辑容器` ，进入容器编辑界面；
3. 在`通用` 页面中，将wine版本切换至带有 `X86_64`字样的内容，这很重要，arm字样无法使用；将 `可执行文件路径` 修改为 `exe/RPG.exe` 是启动游戏，修改为 `exe/cnc-ddraw config.exe` 是修改cnc-ddraw的设置；将 `语言` 改为 `Traditional Chinese`；
4. 切换到 `控制器` 页面，关闭所有打开的开关（虽然不关好像也没事）；
5. 切换到 `Win组件` 页面中，将所有项目的选项全部改为 `Native (Windows)` ，是的，全部选项都要改；
6. 切换到 `环境` 页面，并移到最下面，点击 `+` 按钮，然后点击右侧的清单按钮，找到 `WINEDLLOVERRIDES`（应该是倒数第三个），然后点击新出现的第二个清单按钮，找到 `dinput8=n,b` 然后点击添加，然后点击这个选项开始编辑，将 `dinput8` 改成 `winmm` ，然后点击“确认”保存结果
7. 点击右上角的软盘图标保存，准备完毕。
8. 点击绿色按钮开始游戏吧，进游戏后移动鼠标到屏幕靠中间的任意位置并点一下屏幕，让游戏接收鼠标输入，然后移到边缘隐藏。

---

## 目标游戏版本

CastleReforge 的主要研究、开发与实机验收基线为：

> **《天地劫序传·幽城幻剑录》台湾第三版**

项目中的部分 Hook 会检查目标 EXE 的哈希、机器码或调用协议。

如果目标版本无法确认，部分模块会选择 **fail-closed**，即拒绝安装对应 Hook，而不是在未知 EXE 上强行修改内存。

因此不建议直接把本项目用于来源未知或经过大量二次修改的 `RPG.exe`。

---

## 模组系统要求

| 环境 | 当前支持级别 |
|---|---|
| Windows 11 | 主要开发、维护和实机测试平台 |
| Windows 10 | 预期兼容，但不是当前主要维护基线 |
| Windows XP / 7 / 8 / 8.1 | 不支持；需要自行修改和构建 |
| Wine / Proton | 实验性兼容，没有统一发行验收结论 |
| Android / GameNative | 只有特定设备的实验记录 |

综上，CastleReforge 目前只对win11做维护支持，理论上win10也正常运行。

Linux下，wine/proton7以上的版本应该可以正常运行；安卓已实测GameNative+proton的x86_64下可以运行；

因为编译工具、模组加载等原因，不支持win10以下的系统，xp/win7/8/8.1请自行fork代码修改。

但手柄支持是例外，手柄支持因为依赖SDL，所以极大概率还需要比如这些依赖：

```
VCRUNTIME140.dll
VCRUNTIME140_1.dll
MSVCP140.dll
ucrtbase.dll
api-ms-win-crt-*.dll
```

如果你打开游戏的时候，报 `0xc0000142` ，那么有可能是缺少依赖，请安装常见的vc运行库，具体请参见[SDL项目主页](https://github.com/libsdl-org/SDL)。

---

## 为什么仓库里有这么多文档？

《幽城幻剑录》是一个没有公开源代码的旧游戏。

CastleReforge 的很多功能都建立在：

* 静态逆向；
* 反汇编；
* 游戏状态分析；
* Hook；
* 原版函数协议复用；
* 实机 A/B 测试；

之上。

因此这个仓库除了最终源码，也尽量保存开发过程中已经确认的重要信息，包括：

```text
docs/
├─ Backlog/
├─ Controller/
├─ Extra/
├─ FPSUnlock/
├─ MODLoader/
├─ Quest/
├─ SaveEnhance/
├─ runtime/
├─ Widescreen/
├─ common/
└─ _toolbox/
```

其中包含：

* 架构设计；
* 游戏地址与协议；
* 已知机器码；
* 实机测试结果；
* 成功方案；
* 已废弃方案；
* 已知问题；
* 回归检查；
* 文件哈希；
* 静态检查工具说明；
* 完整开发接档。

这些资料的目的不是单纯记录开发历史。

它们也是项目的一部分。

如果未来需要继续逆向、修复回归或移植到其它版本，应优先阅读对应模块的：

> `截至本版本的完整接档.md`

以及架构、地址和测试文档。

项目级运行时协调 SDK 的当前架构、边界、降级规则和迁移状态见：

> [`docs/runtime/运行时协调系统总体设计.md`](docs/runtime/运行时协调系统总体设计.md)

RuntimeSDK v1、Entry Gate、通用服务、MODLoader 两阶段和 8 个官方 ASI 适配均已完成静态/构建验证。

官方 ASI 强制依赖同目录 `Castle_Runtime.dll`；Runtime 缺失或损坏时安全停用，不回退到旧本地 Hook。

早期 RuntimeSDK 联动曾通过完整资源环境联合验收，但最新 TOML、Clock、Quest 和冲突收口后的最终发行目录仍待全插件实机回归，因此当前属于开发候选。

---

## 仓库结构

```text
castlereforge/
├─ src/
│  ├─ Backlog/
│  ├─ Controller/
│  ├─ Extra/
│  ├─ FPSUnlock/
│  ├─ MODLoader/
│  ├─ Quest/
│  ├─ RuntimeSDK/
│  ├─ SaveEnhance/
│  ├─ Widescreen/
│  └─ _toolbox/
│
├─ docs/
│  ├─ Backlog/
│  ├─ Controller/
│  ├─ Extra/
│  ├─ FPSUnlock/
│  ├─ MODLoader/
│  ├─ Quest/
│  ├─ SaveEnhance/
│  ├─ runtime/
│  ├─ Widescreen/
│  └─ common/
│
├─ build/          构建输出（git 忽略）
├─ build_all.bat   一键编译全部子项目
├─ LICENSE
└─ README.md
```

`src/` 保存源码、构建脚本以及相关开发工具。

`docs/` 保存逆向分析、设计说明、测试记录和接档资料。

公共 SDK 的规范源码位于 `src/RuntimeSDK/`，构建输出 `Castle_Runtime.dll`。第三方开发与迁移指南见
`docs/common/`；FPSUnlock 仍未完成且没有纳入本轮 build_all。

---

## 从源码构建

所有游戏内 DLL / ASI 都必须构建为：

> **Win32 / x86**

因为原版 `RPG.exe` 是 32 位程序。

项目各模块均提供自己的 `build.bat`。

正式 Windows 构建主要使用：

* Visual Studio C/C++ x86 工具链与 Windows SDK；
* `cl.exe`、`link.exe`、`dumpbin.exe`；
* LLVM 的 `clang-cl` 与 `lld-link`；
* Python 3；
* 模组加载器还需要 `rc.exe`。

进入对应源码目录后执行：

```bat
build.bat
```

即可按照该模块规定的参数构建。

也可以在仓库根运行 `build_all.bat`，依次构建 Runtime、Backlog、Controller、Widescreen、SaveEnhance、BUGFix、NoCD、MaxGrowthAndDrop、Quest 和 Mod Loader。

Runtime、8 个官方 ASI、6 份同名 TOML 和插件资源位于 `build\mods\asi`；Loader 位于 `build\`；统一日志目录为 `build\mods\logs`。

构建会拒绝保留 obj/lib/exp/ilk/pdb 等编译垃圾，并主动排除尚未完成的 FPSUnlock。

项目中的游戏内插件普遍采用较严格的最小依赖策略，很多模块：

* 不链接 CRT；
* 使用 `/W4 /WX`；
* 固定 x86 ABI；
* 对生成 PE 再做结构检查；
* 对关键游戏地址在运行时做机器码预检。

部分模块还会使用 GCC 做历史或补充交叉验证，但当前 Windows 全量构建要求 LLVM 的 `clang-cl` / `lld-link`。

具体要求以各源码目录下的 `readme.md` 为准。

---

## 开发原则

CastleReforge 在开发时尽量遵循以下原则：

### 尽量复用原版逻辑

能够调用游戏自己的：

* Event；
* ButtonEvent；
* Writer；
* Scene；
* UI；
* 状态转换；

时，优先复用原版流程，而不是在插件中重新实现一套游戏规则。

### 不直接修改游戏 EXE

正式功能尽量通过运行时 ASI / DLL Hook 实现。

仓库不提供修改后的 `RPG.exe`。

### 未知环境优先拒绝 Hook

如果关键地址、机器码或目标协议不符合预期，优先关闭该能力，而不是继续向未知地址写入补丁。

### 静态验证不等于实机验证

编译成功、哈希正确、机器码匹配、自动检查通过，都不能自动视为游戏实机 PASS。

仓库文档会尽量明确区分：

* 静态 PASS；
* 自动化 PASS；
* 实机 PASS；
* 实机失败；
* 尚待验证。

---

## 项目状态

CastleReforge 仍在持续开发。

不同模块的成熟度并不完全相同：

* 一些运行时已经经过长期实机验证并封存；
* 一些功能拥有稳定基线，但仓库中同时存在更新的开发候选；
* 一些逆向工作仍在继续。

因此不要只根据文件名中的版本数字判断哪个版本“最好”。

请以各模块文档顶部标记的：

* **当前权威版本**
* **已实机验收稳定基线**
* **当前候选**
* **历史 / 废弃**

为准。

---

## 第三方项目与致谢

CastleReforge 是独立实现的项目，但开发过程中研究、参考或使用了多个优秀的开源项目。

### Locale Emulator

感谢 [Locale Emulator / Locale-Emulator-Core](https://github.com/xupefei/Locale-Emulator-Core)。

Castle Mod Loader 的台湾繁中运行环境层在研究过程中参考了 Locale Emulator 在 Windows NLS、代码页、PEB / TEB Locale 状态以及相关系统 Locale 行为方面的成熟实现与技术思路。

CastleReforge 针对《幽城幻剑录》的固定 zh-TW / CP950 环境重新实现自身所需的运行层。

Locale Emulator / Locale-Emulator-Core 的相关源码拥有其自己的许可证。

### Ultimate ASI Loader

感谢 [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader)。

Castle Mod Loader 的 `mods\overrides` 文件覆写机制在设计上参考了 Ultimate ASI Loader `update` / Overload From Folder 的思路：

> 在文件 API 层提供替代资源，而不是直接覆盖游戏原始文件。

CastleReforge 的具体 Mod 管理、文件 Hook、加载顺序和 Launcher 架构为本项目自己的实现。

### cnc-ddraw

感谢 [cnc-ddraw](https://github.com/FunkyFr3sh/cnc-ddraw)。

cnc-ddraw 是 CastleReforge 当前主要进行实机兼容性验证的 DirectDraw wrapper。

CastleReforge 不包含或复制 cnc-ddraw 的 DirectDraw 实现，也不会在本仓库中代为分发它。

### SDL

手柄支持使用 [SDL](https://github.com/libsdl-org/SDL) / SDL3 作为现代控制器输入层。

SDL 本身按照其自己的许可证发布。

---

## 第三方组件说明

上述“致谢”可能代表：

* 技术研究参考；
* 设计思路参考；
* 运行时依赖；
* 兼容性测试环境；

并不意味着这些项目的作者参与、认可或为 CastleReforge 提供技术支持。

第三方项目继续适用其各自的版权和许可证。

关于各个项目的详细信息请参阅各项目的主页。

---

## 问题反馈

如果遇到问题，建议提交 Issue 时至少附上：

* 你的操作系统版本（不支持Windows 10 1903以前的所有系统；Linux/Steam OS/安卓转译视情况支持）；
* 使用的《幽城幻剑录》版本；
* `RPG.exe` 来源或 SHA-256；
* CastleReforge 模块版本；
* 使用的 `dev-auto` 提交号或下载日期；
* 使用的 ASI 列表；
* 是否使用 cnc-ddraw；
* 复现步骤；
* `mods/logs/` 中对应日志；
* 与问题模块同名的 TOML 配置；
* 如果是操作问题，说明所在地图 / 菜单 / Battle / 剧情状态。

对于手柄问题，最好同时说明：

* 手柄型号；
* SDL 是否识别；
* 出问题的具体 UI；
* 按键输入；
* 预期行为；
* 实际行为。

这样通常比单纯描述“不能用”更容易定位问题。

---

## 参与开发

欢迎：

* BUG 报告；
* 实机测试；
* 逆向结果复核；
* 文档修正；
* 地址 / 协议补充；
* 兼容性测试；
* 代码改进。

由于项目大量依赖已经验证过的游戏内部协议，修改核心 Hook 前请优先阅读对应模块中的完整接档和历史因果记录。

一些看起来可以“简化”的旧代码，可能实际上是在修复已经通过实机 A/B 确认的问题。

因此对于关键运行时：

> **已经实机验收的行为优先于理论上更漂亮的重构。**

---

## 许可证

CastleReforge 自有源码采用 **MIT License**。

详见 [LICENSE](LICENSE)。

第三方代码、库、工具和参考项目不因本仓库的 MIT License 而改变其原有许可证。

《天地劫序传·幽城幻剑录》及其名称、程序、美术、音乐、文字和其它原始游戏内容的权利归其各自权利人所有。

本项目为非官方爱好者项目，与游戏原开发者、发行商及相关权利人无隶属或授权关系。

---

## 关于 AI

本项目全部代码、文档内容使用ChatGPT完成，AI 生成或整理的内容不会因此自动视为正确；项目仍以可审查源码、机器检查、原版证据和明确标记的实机结果作为判断依据。

如果你**抗拒**使用 AI 完成的成果，那么**请不要使用本项目**；

如果你是致谢中的某个项目的**作者**/**主要**贡献者并且你很讨厌自己的项目被ai借鉴，亦或是对本项目的参考方式、署名或许可证处理有疑问，请在issues发帖并指出具体文件与问题，届时维护者将会按证据核对和处理借鉴的部分。

---

## 最后

CastleReforge 的目标不是把《幽城幻剑录》重新做成另一个游戏或是移植到其他平台。

它更希望做的是：

> **尽量保留原版的规则、表现和味道，同时把那些属于20多年前的限制留在过去。**
```text
作为模组设计者的话：
    这其实和同人创作没有任何区别
    我个人就是那种原著即天书/圣经/制作指南的那类阿宅
    所以至少在游戏的gameplay风味上我是绝对倾向完全保留原汁原味的
    最大的妥协也就是五合一补丁里的最大掉率与最大成长，以及遇敌率的调整了
    所以就我个人来说，我只希望这款游戏的体验在26年
    能够最终不需要切出游戏翻攻略就能体验真结局以及八成以上的游戏内容
    目前做的所有的一切都是为了这个目标前进
    至少无论是怀旧也好还是初次体验也罢
    不要被时代的门槛拦住
    这，才是我追求的
```
