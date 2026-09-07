# 独立 ASI 迁入 RuntimeSDK 指南

## 一、迁移后的目标形态

本文中的“独立 ASI”指迁移前自行管理 Hook、线程、日志、配置和依赖的旧插件。迁移完成后，
官方版必须变成：

```text
唯一业务核心
    ↓
RuntimeHost
    ├─ Hook/资源所有权
    ├─ 调度和输入
    ├─ TOML/日志/文件/依赖
    └─ Display/Render/Overlay/Save 等领域服务
```

插件仍可脱离 Castle Mod Loader 和其它业务插件运行，但不能脱离 `Castle_Runtime.dll`。迁移不是
把业务算法塞进 Runtime；Runtime 只接管多插件可能冲突或重复实现的基础设施。

## 二、迁移前审计清单

先只读列出并写入接档：

- DllMain、InitializeASI 与线程启动时序；
- 所有游戏代码/数据写入；
- E8 CALL、E9 入口、IAT、vtable 和普通数据槽；
- 每个写入的 module、RVA、大小、原始字节和调用约定；
- 工作线程、Sleep 周期、回调顺序和共享状态；
- WndProc、输入读取、焦点和消息消费；
- 配置、日志、资源、缓存、状态文件和原版存档访问；
- LoadLibrary/GetProcAddress 与依赖卸载；
- 对其它 ASI 文件名、导出或机器码的探测；
- 成功方案、失败方案、实机结论和未知项。

不能询问用户可以从源码直接查到的事实，也不能一边迁移一边猜地址。

## 三、先划清“业务”与“基础设施”

保留在插件：

- 任务判定、插值、宽屏合成、输入映射、存档策略等产品算法；
- 插件自己的配置键和业务日志文字；
- 领域私有数据结构；
- 只有本插件理解的文件格式解析。

迁入 Runtime 服务：

- 冲突游戏写入与 Hook 链；
- 进程级线程、窗口、计时器精度和焦点；
- 公共游戏状态与可变资源租约；
- 同目录路径、文件安全写入、日志和 TOML 标量读取；
- 外部 DLL 搜索、加载、导出解析和固定驻留；
- Display、Render、Overlay、Input、Save 等多插件接口。

## 四、生命周期迁移

第一步把 DllMain 缩到最小。第二步接入 `CastlePlugin_Query` 和 Client。第三步设置
`CASTLE_CLIENT_FLAG_REQUIRE_RUNTIME`。官方 `standalone_initialize` 只返回
`CASTLE_ERROR_RUNTIME_REQUIRED`。

`integrated_initialize` 的推荐顺序：

1. 查询必需接口；
2. 绑定 Log，再写第一条业务日志；
3. 通过 TOML 读取配置；
4. 验证游戏版本和只读协议；
5. 创建 Hook 事务并一次提交；
6. 事务成功后登记 Provider、Overlay、Schedule；
7. 最后把业务状态标记为 ready。

任一步失败都不能调用旧本地安装函数。

## 五、Hook 迁移映射

| 旧写法 | 新写法 |
|---|---|
| 固定 NOP/RET/E9/常量 | ExclusivePatch |
| 原版和启用态二选一 | StatePatch |
| E8 CALL | RelativeCall Hook |
| IAT/vtable 函数指针 | Pointer Hook |
| 多地址共同构成一项功能 | 同一事务 |
| 两项可独立降级 | 两个事务 |
| 旧 previous/original 指针 | HookBinding 的稳定 `next_slot` |

StatePatch 只接受明确的原版态和启用态，不能把第三种未知字节当兼容。事务预检后若地址变化，
Runtime 必须逆序回滚本事务，已提交的其它插件不受影响。

## 六、线程与调度迁移

把旧 worker 拆成：

```text
InitializeOnce()  配置、资源、Hook
PollOnce()        一个不可拆的业务 tick
```

将 `PollOnce` 登记到 Schedule。Runtime 在全部插件 Bootstrap 完成前保持闸门关闭；ModLoader
还会在第二阶段全部 `InitializeASI` 返回后发送 Loader-ready。禁止用固定延时、ASI 排序或临时
旧 worker 代替这个边界。

插件内部已经验收的顺序必须保留。例如 Controller 的“采样→裁决→发布快照→各 UI”仍应在
一个回调内，Runtime 只协调不同插件之间的顺序。

## 七、配置迁移

将官方同名配置从 INI 转为 UTF-8 TOML：

1. 文件名改为 `YourPlugin.toml`；
2. 分号注释改为 `#`；
3. 字符串放入英文双引号；
4. 布尔可使用 `true/false`，现有数值开关也可保留 `0/1`；
5. 每一个键都写独立中文说明；
6. 通过 TOML API 读取，不再调用 GetPrivateProfile；
7. 缺文件或缺键采用代码默认值，不自动覆盖用户文件；
8. 构建和 build_all 只打包 TOML，并清除同名旧 INI。

任务数据库等复杂 TOML 不应硬塞进标量接口；可先用 File 读取，再由领域解析器处理。

## 八、日志与文件迁移

所有官方插件业务日志使用 Log 服务，最终分别进入 `mods/logs`。删除 ASI 同目录的
CreateFile/WriteFile 旁路，即使 Runtime 故障也不能重新启用。

插件相对资源使用 Path/File：

- File 读写只允许 ASI 根内相对路径；
- 拒绝 `..`、盘符、UNC 和冒号逃逸；
- 写入使用临时文件、Flush 和原子替换；
- 需要把绝对路径传给 Windows API 时，用 Path 构造，不自行猜目录。

原版 TSF、ModLoader 配置和 Overrides 不进入通用 File 服务。位于原版真实存档目录的历史兼容
状态文件必须单独评审，不能为了“统一”擅自换路径或格式。

## 九、外部 DLL 迁移

SDL、winmm、user32 等通过 Module 服务加载：

- 插件相对 DLL 只能从本插件根构造；
- 系统 DLL 只能使用 Runtime 白名单；
- 导出由 Module.GetProcedure 取得；
- 跨插件仍保存函数地址时必须固定驻留；
- 不修改进程 CurrentDirectory、PATH 或 SetDllDirectory。

Runtime 只负责模块所有权，不替插件实现 SDL 输入或音频业务。

## 十、已知冲突的标准处理

- SaveEnhance 与 Controller 的 SaveAction：Runtime Save 计算 deny-wins 槽位策略；
- Widescreen 与 FPSUnlock 的 RenderQueue/Present：Runtime Render 唯一拥有中央入口；
- Quest 与其它 Overlay：Runtime Overlay 按阶段、优先级和稳定 ID 调度；
- Backlog 与其它输入界面：Runtime Input Focus 与 GameState Mutation Lease；
- 多个 1ms 请求：Runtime Clock 引用计数租约；
- 多个插件日志和文件写入：Runtime Log/File 串行化。

不要通过识别对方 ASI 文件名继续维持双边兼容。

## 十一、迁移完成条件

- 官方 Client 强制 Runtime；
- DllMain 无业务；
- 集成路径没有私有冲突写入；
- 旧 standalone 回调不能启动功能；
- 所有必需服务缺失时 fail-closed；
- 配置为 TOML且每键有注释；
- 日志只在 `mods/logs`；
- 可迁移文件和依赖已收口；
- 单插件、全插件、不同 Loader、不同加载顺序均通过；
- 子项目 readme、完整接档、公共设计和构建检查同步；
- 构建目录无 obj/lib/exp/ilk/pdb。

静态和构建通过后仍需用户实机确认；确认前不得把候选写成最终稳定结论。
