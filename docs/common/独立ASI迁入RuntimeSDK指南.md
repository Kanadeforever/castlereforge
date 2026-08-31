# 独立 ASI 迁入 RuntimeSDK 指南

## 1. 迁移目标

迁移不是删除独立能力，也不是把插件业务搬进 `Castle_Runtime.dll`。正确结果是：

```text
一份业务核心
├─ StandaloneHost：没有 Runtime 时提供本地基础设施
└─ RuntimeHost：有 Runtime 时使用统一基础设施
```

迁移后用户功能、配置含义、存档格式和已验证算法保持不变；改变的是基础资源的所有权和启动时机。

## 2. 迁移前源码审计

先只读列出：

- DllMain 做了哪些工作；
- 所有 VirtualProtect/WriteProcessMemory/InterlockedExchangePointer 写入；
- E8 CALL、E9/JMP、IAT、vtable 和数据全局地址；
- 每个地址的原始字节、启用字节、调用约定；
- 工作线程、Sleep 周期和线程内业务顺序；
- SetWindowLongPtr/窗口消息；
- GetModuleFileName/CurrentDirectory/INI/日志/依赖 DLL 路径；
- 对其它 ASI 文件名、导出或模块地址的探测；
- 卸载恢复和进程退出路径；
- 已知成功方案、失败方案与实机结论。

不能一边迁移一边猜地址。每个写入必须先归类。

## 3. 写入类型映射

| 旧写法 | RuntimeSDK 迁移目标 |
|---|---|
| 固定 NOP/RET/常量/E9 块 | ExclusivePatch 或 StatePatch |
| 可开关、可恢复原版的字节 | StatePatch |
| E8 CALL | AddRelativeCallHook |
| IAT/vtable 指针 | AddPointerHook |
| 全局互斥资源 | NamedResource 或领域租约 |
| 多地址同一功能 | 同一事务 |
| 原 `previous` 函数指针 | `GetHookBinding().next_slot` |

### 3.1 为什么不能把所有写入都当普通字节补丁

CALL/IAT/vtable 可能需要多个插件共存。普通字节补丁只能声明“我独占”，无法描述调用签名和 next。
只有链声明才能让 Runtime 按 phase、priority、plugin ID 和显式约束计算稳定顺序。

### 3.2 何时使用 StatePatch

当前位置允许恰好两种状态：原版或已启用。Runtime 根据 `desired_state` 写目标状态。

适合：

- INI 开关；
- 兼容磁盘 EXE 已带旧补丁；
- 同一插件重复初始化。

不适合用它接受第三种未知字节。

## 4. 事务划分

事务边界按“用户能力是否必须同时成立”决定：

- 任一点失败会让功能处于半状态：放同一事务；
- 两组功能可以独立降级：拆成两个事务；
- Provider 注册必须等其所需 Hook 事务成功；
- 事务提交前不要把业务状态标记为 ready。

示例：MaxGrowthAndDrop 的成长 5 点和掉宝 2 点可独立，因此拆成两个事务；BUGFix 历史 7 点
和 Crash 双路径也可独立降级；NoCD 的入口 RET 与备用盘符必须一致，因此放同一事务。

## 5. Hook 链迁移

### 5.1 稳定签名 ID

同一调用点要串链的插件必须使用相同签名 ID。签名表示 ABI，不表示插件名称：

```text
org.castlereforge.signature.button-event-this.v1
org.castlereforge.signature.button-hit-fast.v1
```

不同调用约定不能为了“先跑起来”共用一个签名。

### 5.2 next 槽

Runtime 返回 `void* volatile* next_slot`。插件应保存槽地址，每次调用 wrapper 时读取槽内容。

原因：后续插件加入后，Runtime 只需原子更新槽内容；如果插件只在初始化时抄一次函数地址，后加入节点
可能被跳过。

暂时无法改造大量旧 wrapper 时，必须把该插件放在链尾阶段，并在文档中记录这一迁移限制；共享关键点
仍应优先改成真正的动态 next。

## 6. 线程迁移

把旧 worker 分成两部分：

```text
InitializeOnce()   配置、资源、Hook
PollOnce()         一个完整业务 tick
```

StandaloneHost 创建原线程并循环 `PollOnce + Sleep`；RuntimeHost 把 `PollOnce` 注册成周期任务。

不要让 Runtime 拆开插件内部已验证顺序。例如 Controller 的“采样→裁决→发布快照→各 UI”必须仍在
一个回调内；Runtime 只决定不同插件任务之间的排序。

## 7. WndProc 迁移

旧插件若使用 SetWindowLongPtr：

- 只观察消息：注册 Observer；
- 需要吞消息：另注册 Filter；
- Observer 先记录输入，Filter 根据当前业务状态决定 consume；
- 窗口尚未出现的 NOT_READY 不是登记失败；后续 GetGameWindow 可触发刷新；
- 独立模式保留原谨慎链式 WndProc。

## 8. 路径迁移

整合模式通过 Path API：

- 获取插件真实模块路径；
- 构造 ASI 根目录相对路径；
- 使用 UTF-8 或 UTF-16 明确版本；
- 拒绝绝对路径、盘符、UNC 和 `..` 逃逸；
- 不修改 CurrentDirectory、PATH、SetDllDirectory。

独立模式可复用同一算法的本地实现。配置键和文件名不因迁移改变。

## 9. Display/Render Provider 迁移

专业显示插件先完成 Hook，再：

1. 注册 Display Provider；
2. 发布初始几何；
3. SetDisplayProviderReady；
4. 注册绑定该 Display Provider 的 Render Provider；
5. SetRenderProviderReady。

每帧只发布真正画出的几何。Camera 临时修改后又恢复时，`effective_camera` 填绘制使用值，
`original_camera` 只作诊断。

## 10. 生命周期迁移

旧 DllMain 中的业务按以下方式搬出：

| 旧工作 | 新位置 |
|---|---|
| 保存 HMODULE | DllMain 可保留 |
| Entry Gate | Client OnProcessAttach |
| 读 INI/日志 | 两种 Initialize 回调 |
| LoadLibrary 业务依赖 | 两种 Initialize 回调 |
| 安装 Hook | StandaloneHost/RuntimeHost |
| 创建 worker | StandaloneInitialize |
| 注册 Schedule | IntegratedInitialize |
| 进程退出标记 | process_exit |

Runtime 故障回调不得调用 StandaloneInitialize。

整合模式在 `IntegratedInitialize` 中登记 Schedule 是安全的：Runtime 会在 Bootstrap 期间关闸，
普通 Loader/ModLoader 的主线程真正命中 SDK Entry Gate 后才启动唯一 worker。迁移时不要保留
“先创建旧 worker，等 Schedule 开始后再停”的过渡方案，否则会恢复双线程竞态。

## 11. 分阶段迁移方法

1. 先接入描述表、Client、标准导出，不改业务；
2. 把 DllMain 业务搬到 StandaloneInitialize，证明独立模式等价；
3. 建立 RuntimeHost，但先只做 Query/Path；
4. 逐能力迁移 Hook 事务；
5. 迁移线程和窗口；
6. 迁移 Provider/跨插件链；
7. 删除整合模式私有写入旁路；
8. 运行三模式矩阵和多插件顺序测试；
9. 更新模块完整接档和公共指南；
10. 最后进入 build_all 和发行校验。

任何阶段都不能以“有 Runtime 时还能偷偷跑旧安装函数”作为临时正式方案。
