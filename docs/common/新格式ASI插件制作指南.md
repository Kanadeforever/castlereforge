# 新格式 ASI 插件制作指南

## 一、适用范围与强制结论

本文是 CastleReforge 官方 ASI 的施工标准。自当前版本起，官方 ASI 必须与
`Castle_Runtime.dll` 同目录运行；没有 Runtime 时必须保持停用，不能再安装本地 Hook、
创建旁路线程或自行连接其它官方插件。

“插件可以单独运行”的准确含义是：

```text
任意兼容的 x86 ASI Loader
    + Castle_Runtime.dll
    + 一个官方 ASI
```

它不要求 Castle Mod Loader，也不要求其它业务 ASI。Runtime 是官方插件包的基础引擎，
不是可选业务插件。第三方仍可自行决定是否使用 SDK Client 的可选兼容模式，但不得把这种
第三方能力写成官方发布标准。

## 二、标准目录

```text
mods\asi\
├─ Castle_Runtime.dll
├─ YourPlugin.asi
├─ YourPlugin.toml          可选的人类配置
└─ YourPlugin\             可选的只读资源目录

mods\logs\
├─ Castle_Runtime.log
└─ YourPlugin.log
```

Runtime 和 ASI 必须同目录，且 Runtime 保持 `.dll` 扩展名。ModLoader 和普通 ASI Loader
都不应扫描或直接加载这个 DLL；任一 SDK ASI 会在 Loader Lock 外按同目录相对位置加载它。

## 三、推荐源码结构

```text
src\YourPlugin\
├─ source\
│  ├─ plugin.c/.cpp          描述表、Client 回调、导出和 DllMain
│  ├─ business.c/.cpp        唯一业务核心
│  ├─ runtime_host.c/.cpp    Runtime 服务接线
│  └─ YourPlugin.def         未修饰导出名
├─ templete\
│  ├─ YourPlugin.toml        每个键都有独立中文解释
│  └─ YourPlugin\            可选资源
├─ tools\
│  └─ 工具详细说明.md
├─ build.bat
└─ readme.md
```

`src` 中不新增普通设计文档；构建说明只使用同级 `readme.md`，其它文档进入 `docs`。

## 四、稳定身份

插件 ID 使用全小写反向域名，例如：

```text
org.castlereforge.yourplugin
```

要求：发布后不复用、不随文件名变化；测试版与正式版使用同一 ID，以 `version_text` 和
`build_id` 区分。ID 只允许 ASCII 小写字母、数字、点、下划线和短横线。

描述表必须声明 `CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE`，按需要增加 Hook 或 Provider 能力，
但不再声明 `CASTLE_PLUGIN_FLAG_SUPPORTS_STANDALONE`：

```c
static const CastlePluginDescriptorV1 g_descriptor = {
    CASTLE_PLUGIN_DESC_MAGIC,
    CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE | CASTLE_PLUGIN_FLAG_REQUESTS_HOOKS,
    0u,
    {g_plugin_id, (CastleU32)(sizeof(g_plugin_id) - 1u)},
    {g_display_name, (CastleU32)(sizeof(g_display_name) - 1u)},
    {g_version_text, (CastleU32)(sizeof(g_version_text) - 1u)},
    {g_build_id, (CastleU32)(sizeof(g_build_id) - 1u)}
};
```

## 五、Client 配置

官方配置必须设置：

```c
CASTLE_CLIENT_FLAG_REQUIRE_RUNTIME
```

四个回调职责如下：

1. `integrated_initialize`：查询服务、读取配置、提交 Hook 事务、登记任务或 Provider；
2. `standalone_initialize`：为 ABI 保留，但官方实现只返回 `CASTLE_ERROR_RUNTIME_REQUIRED`；
3. `runtime_fault`：保持业务停用，不创建旁路日志；
4. `process_exit`：注销自己持有的服务句柄，不回写 Runtime 拥有的游戏入口。

Runtime 文件不存在、损坏、位数错误或 ABI 不匹配时，官方插件都必须 fail-closed。不能以
“让功能尽量运行”为理由回到私有 `VirtualProtect` 路径。

## 六、DllMain 与导出

`DllMain` 只允许保存模块句柄、关闭线程通知，并调用 Client 的 attach/detach。禁止在其中：

- 打开 TOML 或日志；
- 加载 SDL、winmm 等业务依赖；
- 创建线程；
- 安装 Hook；
- 枚举其它 ASI；
- 调用插件业务初始化。

每个官方 SDK ASI 必须导出：

```text
CastlePlugin_Query
InitializeASI
CastleRuntimeClient_NotifyLoaderReady
```

`InitializeASI` 只调用 `CastleRuntimeClient_RunNow()`。ModLoader 使用两阶段初始化；普通 ASI
Loader 则由 Entry Gate 触发。两条路径共享同一原子状态机，不会重复初始化。

## 七、服务选择

| 需求 | 必须使用 |
|---|---|
| 游戏代码、数据、CALL、IAT、vtable | Hook |
| ASI 同目录和资源子目录 | Path / File |
| 插件 TOML 标量配置 | TOML |
| 插件独立日志 | Log |
| 外部 DLL 与系统模块 | Module |
| 周期/一次性后台任务 | Schedule |
| 游戏窗口消息 | Window |
| 手柄和语义输入 | Input |
| 当前游戏公共状态 | GameState |
| 临时修改 Camera/Draw/World 等 | GameState Mutation Lease |
| 存档按钮和槽位保护 | Save |
| 世界坐标投影 | Display |
| RenderQueue/Present | Render |
| Marker、HUD 等 Present 前绘制 | Overlay |
| 1ms 计时器精度 | Clock |

不要用 `GetModuleHandle("另一个插件.asi")`、读取其它插件 TOML、扫描当前 CALL 目标或猜加载
顺序替代服务查询。

## 八、TOML 配置标准

官方配置使用 UTF-8 TOML。当前 Runtime TOML v1 面向配置，正式支持：

- 普通 `[表]`；
- ASCII 裸键；
- `true/false`；
- 32 位十进制整数；
- 英文双引号 UTF-8 字符串。

每个键正上方必须有独立简体中文注释，写清单位、范围、默认值、开关含义和与其它键的关系。
配置模板随 ASI 一起发布；插件不得在启动时重写用户配置。ModLoader 的同名 TOML 编辑器只
负责语法检查和原子保存，不理解每个插件的业务范围。

复杂任务数据库可以继续使用完整 TOML 结构和插件自己的领域解析器；不要谎称 Runtime 标量
接口已经覆盖数组表。未来扩展必须新增版本或追加兼容函数，不能改变 v1 现有语义。

## 九、日志标准

插件通过 Log 服务写自己的业务日志。Runtime 自动生成：

```text
mods\logs\<ASI文件名>.log
```

每个插件仍保留独立日志，Runtime 不把所有业务堆进一个文件。插件不得自行 `CreateFile` 打开
旁路日志；Runtime 日志失败也不能改变安全判断。

## 十、构建标准

构建脚本必须自动使用 `vswhere` 或 PATH，不得写开发者电脑的物理编译器/Python路径。中间
文件只进入子项目 `_build`，仓库根 `build` 只接受发行文件。中文 `rem` 和带文字的 `echo`
行末保留两个半角空格。

必须验证：PE32/i386、DLL 标志、非零入口、未修饰导出、预期静态依赖和零编译垃圾。正式插件
由 `build_all.bat` 发行；未完成项目可保持单独构建，不得冒充正式发行组件。

## 十一、最低验收

1. Runtime + 单插件 + NoCD（原始光盘 EXE 必需）可启动；
2. 不使用 ModLoader、改由其它 ASI Loader 时仍可启动；
3. 缺少 Runtime 时插件无业务写入；
4. 损坏 Runtime 不弹阻塞系统对话框、不回退私有 Hook；
5. 与全部官方插件联合运行；
6. 交换 ASI 加载顺序，最终 Hook/Provider/任务顺序不变；
7. 重复调用 `InitializeASI` 不重复安装；
8. TOML 缺失、损坏、越界值安全回退；
9. 日志只出现在 `mods/logs`；
10. 构建、静态合同与实机清单全部通过。

完成静态和构建测试只能称为候选；涉及游戏内地址、输入、存档和显示的变化仍需实机联合验收。
