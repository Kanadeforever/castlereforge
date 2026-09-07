# Castle_Quest v0.1-dev6zd 构建说明

## 1. 子项目定位

Castle_Quest 已按 CastleReforge 主项目结构迁移到 `src/Quest/`。本目录可以直接移动到主仓库的 `src/Quest/`；任务系统不再复制一份 RuntimeSDK，而是编译时引用同级 `src/RuntimeSDK/` 的公共头和 Client 源码。

当前 SDK 接入：

- 官方版强制依赖同目录 `Castle_Runtime.dll`；
- `0x00409580` ExplorationUpdate 由 Runtime Schedule GamePhase 拥有；
- `0x004064E0` Present 由 Runtime Overlay 拥有；
- Quest 使用 Display.WorldToScreen 和 GameState，不再保留本地入口 Hook；
- 配置由 Runtime TOML 读取，日志由 Runtime Log 写入 `mods/logs`；
- Runtime 缺失或故障时安全停用；
- `.state` 增强存档本版不实现，只在任务系统文档中冻结未来由主 SDK 统一管理 TOML 状态读写的接口边界。

`readme.md` 是本子项目唯一构建说明。

## 2. 主项目目录位置

合并进主项目后应为：

```text
CastleReforge/
├─ build_all.bat
├─ docs/
│  └─ 任务系统/
└─ src/
   ├─ RuntimeSDK/
   └─ Quest/
      ├─ source/
      │  ├─ CastleQuest.cpp
      │  ├─ CastleQuest.def
      │  ├─ RouteSearch.cpp
      │  └─ RouteSearch.h
      ├─ templete/
      │  ├─ Castle_Quest.toml
      │  └─ Castle_Quest/
      │     ├─ manifest.toml
      │     ├─ Q001_main_story.toml
      │     ├─ Q001_main_story_addon.toml
      │     ├─ ...
      │     ├─ Q025_supp_affection.toml
      │     └─ Q025_supp_affection_addon.toml
      ├─ tools/
      │  ├─ CastleScriptAnalyzer.py
      │  ├─ 工具详细说明.md
      │  └─ 工具截至该版本的完整接档说明.md
      ├─ build.bat
      └─ readme.md
```

## 3. 任务数据库当前冻结规模

本版 `manifest.toml` 已按实际文件机械复核：

```text
Manifest format_version = 7
Base format_version     = 7
Addon format_version    = 2
Quest                    = 25
Addon                    = 25
Base Stage               = 146
Canonical Stage          = 146
Guidance Stage           = 0
Warning                   = 0
Stage-local Route         = 556
Addon Insert              = 1
运行 TOML 总数            = 51
```

运行时没有 `routes.toml`。

Base 只描述原版真实任务链；Remastered 人工体验步骤只能写进同名 `_addon.toml`。旧 Guidance/人工 Base Stage 运行体系已经完全退役；Base v7 连 `stage_type` 字段也已删除，因为 Base 本身就天然代表 canonical 原版链。

Q004“营救阿吉”原来的 `optional_clues` 已从 Base 删除；当前人工调查步骤只存在于 `Q004_side_rescue_aji_addon.toml`，锚定 `ask_mother -> world_trigger`。

Q001 的 `guide_get_chicken_soup` 虽保留旧稳定 `stage_id`，但它对应原版真实 Event18 / Var1=0→1，所以本版已重分类为 canonical，不属于 Guidance。

## 4. Addon 热重载状态

Ctrl+F8 会事务式重新读取 TOML 配置、manifest、25 Base 和 25 Addon。任一文件失败时继续使用上一套完整有效数据库。

本版修复了一个开发期问题：**成功热重载不再清空当前进程中的 Addon Insert 完成集合**。完成身份只认：

```text
addon.quest_id + insert.stage_id
```

因此只修改文案、Marker、坐标或其他 Addon 字段，不会让同一人工步骤重新触发。dev6zd 也不再根据 Quest 的 `Available/Unavailable` 状态猜测“是否读了更早存档”并删除完成键；在 `.state` 接入前，同一进程的完成键采用只增不减规则。

这意味着当前有一个明确的临时限制：如果不退出游戏就在同一进程读取更早 TSF，人工步骤不会自动回滚。跨游戏重启/跨 TSF 的正确持久化与回滚等待主 SDK 的共享 `.state` 服务。

## 5. RuntimeSDK 运行路径

正常情况下 Client 登记 Quest 后，由 Runtime 提供 Schedule、Overlay、Display、GameState、TOML
和 Log。Quest 不直接修改 RPG.exe 的 Present 或探索入口。

官方 Client 设置 `CASTLE_CLIENT_FLAG_REQUIRE_RUNTIME`。同目录完全没有 Runtime，或 Runtime 的
PE/ABI/初始化失败时，Quest 不加载任务业务、不安装本地 Hook，也不在 ASI 目录创建旁路日志。
保留的 standalone ABI 回调只返回 `CASTLE_ERROR_RUNTIME_REQUIRED`。

## 6. 构建

在主项目结构内运行：

```text
src\Quest\build.bat
```

构建脚本遵循主项目的 Visual Studio 自动发现方式，通过 `vswhere + VsDevCmd -arch=x86` 建立 x86 Windows SDK/库环境，然后使用 MSVC `cl + link + dumpbin`。

Quest 是 C++17/STL 项目，所以与 RuntimeSDK/Widescreen 的纯 C `/NODEFAULTLIB` 构建不同：Quest 继续使用已经稳定验证的 `/MT /std:c++17 /Od`，同时把 RuntimeSDK Client 的三个 C 文件编进同一 ASI。

脚本会编译：

```text
CastleQuest.cpp
RouteSearch.cpp
RuntimeSDK/client/runtime_client.c
RuntimeSDK/client/runtime_entry_gate.c
```

并通过 `CastleQuest.def` 固定三个无修饰导出：

```text
CastlePlugin_Query
InitializeASI
CastleRuntimeClient_NotifyLoaderReady
```

脚本还会机械检查：

- PE32/x86；
- 三个稳定导出存在且没有前导下划线修饰名；
- `/MT` 发行目标没有 `MSVCP` / `VCRUNTIME` 动态依赖；
- 任务运行目录恰好 51 个平铺 TOML；
- 没有 `routes.toml`；
- 没有运行数据子目录。

所有 `.obj/.lib/exp/测试文本` 只放 `src/Quest/_build`，成功或失败后都会删除，不进入发行 `build/`。

## 7. build 输出和 build_all 合并

Quest 自己的 `build.bat` 成功后只替换它自己拥有的三个发行项：

```text
build/
├─ Castle_Quest.asi
├─ Castle_Quest.toml
└─ Castle_Quest/
   ├─ manifest.toml
   ├─ 25 个 Base
   └─ 25 个 Addon
```

它**不会清空整个主项目 `build/`**，因此不会删除其他子项目已经产生的发行文件。

主项目 `build_all.bat` 会把根 `build\*.asi` 和同名 TOML 移入 `build\mods\asi\`，并移动任务数据目录。

```bat
call "%ROOT%src\Quest\build.bat" < nul || goto :fail
```

最终打包阶段再把 Quest 数据目录移动到 ASI 同目录：

```text
build\Castle_Quest\
    ↓
build\mods\asi\Castle_Quest\
```

这样最终运行布局为：

```text
build\mods\asi\
├─ Castle_Quest.asi
├─ Castle_Quest.toml
├─ Castle_Runtime.dll
└─ Castle_Quest\
   └─ 51 个 TOML
```

当前主项目已经完成上述 build_all 接入，并用发行检查器验证 Quest ASI、TOML 与 manifest。

## 8. `.state` 后续边界

本版**不创建、不读写 `.state`**。

当前没有任何插件实际创建 `.state`。未来真实需求落地时，原版 `TSF` 永远不修改；Remastered
独有进度由同名 `.state` 承担，并由 Runtime 专用 State 服务统一负责 TOML、命名空间、写入顺序
和原子落盘。Quest 只提交自己的模块状态，不直接拥有共享文件。

任务系统未来首先需要保存的是 Addon Insert 完成集合；能从原版 GameState/TSF 推导出来的 Base 任务进度不重复保存。

## 9. 当前验证边界

当前 Windows x86 构建、51 文件数据合同和 RuntimeSDK 接口编译已经通过。游戏实机仍需按
`docs/Quest/实机验证清单.md` 与公共联合验收标准确认，机器构建不能冒充实机结果。
