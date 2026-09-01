# 新格式 ASI 插件制作指南

## 1. 本指南的目标

本文用于从零制作一个同时满足以下条件的 CastleReforge 新格式 ASI：

- 没有 `Castle_Runtime.dll` 时可以独立运行；
- 与 Runtime 同目录时自动进入整合模式，不提供绕过开关；
- Runtime 文件存在但损坏时停止游戏写入，不偷偷退回独立模式；
- 可以由 Castle Mod Loader 或其它普通 32 位 ASI Loader 加载；
- DllMain 不读配置、不写日志、不加载业务 DLL、不创建业务线程、不安装业务 Hook；
- 所有跨 DLL 结构使用稳定 C ABI、4 字节对齐和固定宽度类型；
- 构建产物为 PE32/i386、DLL、非零入口，公开导出名未修饰；
- 代码和注释足够让后来维护者独立判断每一步为什么安全。

本指南对应的公共头位于 `src/RuntimeSDK/include`，Client 实现位于
`src/RuntimeSDK/client`。不要复制头文件内容到插件目录；构建时直接包含 SDK 目录。

## 2. 必须先理解的三种运行状态

### 2.1 独立模式

ASI 同目录完全不存在 `Castle_Runtime.dll`。SDK Client 在 RPG 入口、Loader Lock 外枚举
已加载 SDK 插件，按稳定插件 ID 排序，然后调用每个插件的 `standalone_initialize`。

独立模式不是“无安全规则模式”。插件仍必须：

- 对目标游戏版本和机器码做精确预检；
- 失败时不写未知地址；
- 对自己写入的一组地址提供完整回滚或明确的进程寿命边界；
- 不假设其它插件会帮自己排序或串链。

独立模式没有全局协调器。两个独立插件碰到同一地址时，最多靠各自预检发现变化并拒绝；
它们不会自动生成 Runtime Hook 链。

### 2.2 整合模式

ASI 同目录存在可用的 `Castle_Runtime.dll`。任意 SDK ASI 的 Client 会主动加载它，Runtime
随后全量枚举插件、按 ID 排序、登记、固定驻留并调用 `integrated_initialize`。

整合模式中必须遵守：

- 游戏内存写入只通过 Runtime Hook 事务；
- 同目录路径优先通过 Path API；
- 周期后台任务优先通过 Schedule API；
- WndProc 观察/过滤通过 Window API；
- 显示坐标通过 Display API；
- RenderQueue/Present 通过 Render API；
- 可选接口缺失只关闭对应增强，不允许改走私有冲突路径。

### 2.3 Runtime 故障安全模式

文件存在，但 LoadLibrary、稳定导出、ABI 或 Runtime 初始化失败。Client 调用
`runtime_fault`，插件只能记录诊断，不得调用 StandaloneHost 安装补丁。

SDK Client 会在加载探测期间临时抑制 Windows 的损坏映像对话框，并在返回前恢复宿主原错误
模式。插件不要自行永久调用 `SetErrorMode`，也不要用测试程序全局关闭弹窗来掩盖 Client 回归。

这是防止同一进程同时出现“受管理 Hook”和“绕过 Runtime 的私有 Hook”的关键规则。

## 3. 推荐源码结构

```text
src/YourPlugin/
├─ source/
│  ├─ plugin.c                 生命周期、描述表、标准导出
│  ├─ business.c/.h            唯一业务核心
│  ├─ standalone_host.c/.h     本地补丁/线程/窗口实现
│  ├─ runtime_host.c/.h        Runtime API 事务和服务接线
│  └─ YourPlugin.def           未修饰导出名
├─ templete/
│  └─ YourPlugin.ini           可选默认配置
├─ tools/
│  ├─ 工具详细说明.md
│  └─ your_plugin_check.py
├─ build.bat
└─ readme.md
```

业务很小时可以把三个 C 文件合并，但概念边界仍必须存在：业务判断不能直接决定使用
VirtualProtect 还是 Runtime 事务；它只能调用插件内部 Host 接口。

## 4. 稳定插件身份

插件 ID 使用全小写反向域名格式：

```text
org.castlereforge.yourplugin
```

要求：

- 发布后永不改给另一个插件；
- 文件名改动不改变 ID；
- 测试版和正式版使用同一 ID，通过 `version_text/build_id` 区分；
- 只能使用 `a-z`、`0-9`、`.`、`_`、`-`；
- 与仓库已有 ID 不重复。

描述表是静态只读对象。`module` 在静态表中写 0，Runtime 枚举时会用真实 HMODULE
构造登记副本：

```c
static const CastlePluginDescriptorV1 g_descriptor = {
    CASTLE_PLUGIN_DESC_MAGIC,
    CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_SUPPORTS_STANDALONE |
        CASTLE_PLUGIN_FLAG_REQUESTS_HOOKS,
    0u,
    {"org.castlereforge.yourplugin", 30u},
    {"Your Plugin", 11u},
    {"1.0.0", 5u},
    {"release-1", 9u}
};
```

实际长度应由 `sizeof(text)-1` 计算，不手填容易出错的示例数字。

## 5. 四个生命周期回调

`CastleRuntimeClientConfigV1` 包含：

1. `integrated_initialize`：声明 Runtime 事务、服务和任务；
2. `standalone_initialize`：建立本地 Host；
3. `runtime_fault`：只记错误，不安装功能；
4. `process_exit`：只做短标记/资源关闭，不等待线程。

推荐把两个初始化回调都交给同一业务核心：

```text
IntegratedInitialize
    -> 创建 RuntimeHost
    -> Business_Initialize(&runtime_host)

StandaloneInitialize
    -> 创建 StandaloneHost
    -> Business_Initialize(&standalone_host)
```

禁止复制 `Business_InitializeRuntime` 和 `Business_InitializeStandalone` 两套业务算法。

## 6. DllMain 的标准写法

PROCESS_ATTACH 只允许：

- 保存自身 HMODULE；
- 可选关闭线程通知；
- 调用 `CastleRuntimeClient_OnProcessAttach` 安装/加入 Entry Gate。

PROCESS_DETACH 只调用 `CastleRuntimeClient_OnProcessDetach`，独立模式如有指向本 DLL 的本地
Hook，可在明确的主动卸载路径做最小恢复。进程整体退出时不要等待线程或大规模回写。

```c
BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_module = module;
        DisableThreadLibraryCalls(module);
        CastleRuntimeClient_OnProcessAttach(
            (CastleModule)(ULONG_PTR)module, &g_plugin_export);
    } else if (reason == DLL_PROCESS_DETACH) {
        CastleRuntimeClient_OnProcessDetach(reserved);
    }
    return TRUE;
}
```

## 7. 三个标准导出

每个 SDK ASI 都必须导出：

```text
CastlePlugin_Query
InitializeASI
CastleRuntimeClient_NotifyLoaderReady
```

`InitializeASI` 只调用 `CastleRuntimeClient_RunNow()`。Castle Mod Loader 第二阶段会调用它；
普通 ASI Loader 则由 Entry Gate 触发。两条路径竞争同一个幂等状态，不会初始化两次。
MODLoader 路径会在阶段2恢复 SDK Entry Gate；Schedule 任务可以先登记，但外层所有
`InitializeASI` 返回并调用 `CastleRuntimeClient_NotifyLoaderReady` 前不会运行。这个第三导出由
SDK Client 实现，插件入口文件无需复制逻辑，也不得因为任务尚未执行就创建临时 worker。

使用 `.def` 保证 x86 导出名未修饰：

```def
EXPORTS
    CastlePlugin_Query
    InitializeASI
    CastleRuntimeClient_NotifyLoaderReady
```

如果插件已有公开 API，例如 `CastlePad_GetApi`，保留原导出并追加这两个名字。

## 8. 构建脚本标准

构建脚本必须：

- 使用 `vswhere` 或当前 PATH 定位工具链；
- 不写开发者电脑上的编译器/Python绝对路径；
- GitHub Actions 与本地走同一脚本；
- SDK Client 三个 C 单元编进 ASI：
  - `runtime_client.c`
  - `runtime_entry_gate.c`
  - C++ 插件按需链接 `runtime_client_support.c`
- 中间 `.obj/.lib/.exp` 只进入插件 `_build`；
- 最终只把 ASI、INI 和运行资源放入仓库 `build`；
- 成功/失败都清理 `_build`；
- 中文 `rem/echo` 行尾保留两个半角空格。

最终链接至少验证：

- Machine=`0x014C`；
- OptionalHeader=`PE32/0x10B`；
- DLL 标志存在；
- AddressOfEntryPoint 非零；
- 标准导出存在且未修饰；
- 没有意外 CRT 导入。

## 9. 最小复核顺序

1. 同目录无 Runtime，单独加载插件；
2. 同目录有 Runtime，单独加载插件；
3. 同目录有两个以上 SDK 插件，交换加载顺序；
4. 放入损坏的同名 Runtime，确认没有私有补丁；
5. Castle Mod Loader 两阶段路径；
6. 其它 ASI Loader Entry Gate 路径；
7. 重复 `InitializeASI`；
8. 改 ASI 文件名但 ID 不变；
9. 缺少可选服务；
10. 目标机器码变化、资源冲突和事务回滚。

只有以上检查都通过，才能把插件标为新格式 ASI。
