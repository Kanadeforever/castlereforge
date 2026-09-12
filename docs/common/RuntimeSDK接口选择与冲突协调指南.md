# RuntimeSDK 接口选择与冲突协调指南

## 一、总原则

Runtime 是官方 ASI 的底层协调引擎，不是万能业务插件。只有“多个插件会争用、重复实现或必须
共享同一事实”的部分进入公共服务；任务内容、宽屏像素算法、手柄映射和存档产品策略继续留在
各自插件。

## 二、接口总表

| 需求 | 接口 | 关键规则 |
|---|---|---|
| 固定代码/数据写入 | Hook Exclusive/State | 未知字节拒绝 |
| E8 CALL 链 | Hook RelativeCall | 同签名、稳定 next |
| IAT/vtable 链 | Hook Pointer | 函数指针才可串链 |
| 插件目录 | Path | 不改变进程环境 |
| 插件文件 | File | 根边界、锁、原子替换 |
| 标量配置 | TOML | UTF-8、类型明确 |
| 独立业务日志 | Log | `mods/logs`、每插件一份 |
| 外部 DLL | Module | 相对路径/系统白名单/固定驻留 |
| 计时器精度 | Clock | 成对租约、引用计数 |
| 周期任务 | Schedule | 单线程、闸门、失败隔离 |
| 窗口消息 | Window | Observer 先于 Filter |
| 输入快照和焦点 | Input | 单 Provider、焦点互斥 |
| 游戏公共状态 | GameState | 只读快照、generation |
| 临时改游戏状态 | GameState Mutation | 按资源位互斥 |
| 存档槽保护 | Save | deny-wins |
| 世界/屏幕投影 | Display | 本帧最终几何权威 |
| RenderQueue/Present | Render | 单后端、额外帧租约 |
| Present 前 HUD | Overlay | 排序回调、失败隔离 |

## 三、Hook 决策

### ExclusivePatch

只有一个所有者的固定替换，例如完整入口跳转、NOP 块或常量。冲突时拒绝，不尝试猜兼容。

### StatePatch

当前位置允许“原版”和“启用”两种明确字节，配置决定目标态。第三种状态一律未知，不能为了兼容
扩大接受范围。

### RelativeCallHook

目标必须是 E8 CALL。声明原版目标、wrapper、签名 ID、phase、priority 和可选约束。Runtime
返回稳定 `next_slot`；wrapper 每次调用时读取槽内容，不只在初始化时抄一份地址。

### PointerHook

用于 IAT/vtable 函数指针槽。普通数据指针不能冒充函数链；不同调用约定不能共用签名。

## 四、事务边界

- 一个功能缺任何地址都会半坏：同一事务；
- 两项功能可独立关闭：拆事务；
- Provider 依赖 Hook：先提交事务，再登记并 ready；
- 事务失败：停止该功能，不继续盲装后续地址；
- Runtime 调插件回调时不持内部锁。

## 五、Provider 与消费者

```text
消费者 → Runtime稳定门面 → 当前权威Provider
```

消费者禁止：按 ASI 文件名 GetModuleHandle、读取后端配置推测状态、缓存后端私有结构、扫描
CALL 当前目标。Provider 未 ready 时返回 NotReady；generation 变化后消费者重新取快照。

## 六、资源租约

适用于 Input 焦点、GameState 可变资源、Render 额外帧和 Clock 精度。租约必须：

- 由已登记插件申请；
- 句柄非零且不可伪造；
- 冲突返回 ResourceConflict；
- 释放后旧句柄无效；
- process_exit 尽量主动释放；
- Runtime 固定驻留，避免回调或函数地址指向已卸载模块。

## 七、文件、TOML 与日志

Path 只负责路径事实；File 负责安全读写；TOML 负责类型化配置；Log 负责日志文件。不要把四者
混成一个“万能路径函数”。

File v1 不允许越出 ASI 根，因此不用于原版 TSF、Overrides、ModLoader 配置和真实存档目录的
历史兼容文件。跨项目 `SaveXXX.state` 目前仍是规划，没有当前消费者；未来落地时应新增专用
State 服务，统一 TOML 内存树、模块命名空间、Save/Load 生命周期和原子落盘，而不是让多个
插件各自读改写。

## 八、已闭合冲突

### SaveEnhance / Controller

Runtime Save 唯一包装 SaveAction。SaveEnhance登记 0、91～99 禁止普通写入；Controller 只读
原生 disabled。任何 deny 优先，加载顺序不影响结果。

### Widescreen / FPSUnlock

Runtime Render 唯一接管 RenderQueue/Present。Widescreen 发布 Display/Render Provider；
FPSUnlock 请求额外 world frame 租约。Camera、Draw Queue、Dialogue、World 临时修改由
GameState Mutation Lease 保护。

### Quest / Widescreen

Widescreen 发布本帧有效 Camera 和投影；Quest 调 WorldToScreen。Quest 通过 Overlay 绘制，
不拥有 Present，不复制 CameraPlan，也不读宽屏 TOML。

### Backlog / Controller / SaveEnhance

Controller 发布 Runtime Input；其它插件读取快照。Backlog 打开时取得 Input Focus 和 Dialogue
资源租约；SaveEnhance读取语义动作，不查询 PadSupport 文件名。

### Controller / Widescreen 鼠标

Controller 决定手柄/键鼠所有权与哪些业务场景允许显示原版手形；Widescreen 决定最终输出坐标、
实际内容区域和世界投影。Widescreen 的 GetCursorPos 使用 Runtime PointerHook 链，不能覆盖
Controller 或第三方兼容层；最终鼠标只在宽屏 staging 绘制一次。自由探索真实侧区可命中，
对话、战斗、菜单、电影、过渡和空白侧区只允许经过、不接受点击。

## 九、何时新增接口

满足下列条件才新增版本化领域接口：

- 至少两个插件需要同一事实或资源，或当前单一所有者未来明确会有消费者；
- 有唯一权威、可组合规则或明确互斥模型；
- 能说明版本、结构大小、线程、ready、fault、generation；
- 缺失时有确定行为；
- 不把某个实验插件的内部结构冻结成公共 ABI；
- 先写 ABI 和测试，再迁移生产插件。

## 十、禁止模式

- Runtime 存在时直接修改公共冲突地址；
- Runtime 缺失/损坏后启动官方 standalone 业务；
- 在 DllMain 读配置、写日志、加载依赖或创建线程；
- 多个插件各建高频 worker；
- 多个 WndProc、Present 或 SaveAction 私有包装层；
- 固定加 107/240 代替 Display；
- 当前 CALL 目标代替 Render；
- 写入非原子共享状态；
- 把所有业务日志合并进 Runtime.log；
- 构建成功但漏打包配置、资源、说明或 Quest 数据。
