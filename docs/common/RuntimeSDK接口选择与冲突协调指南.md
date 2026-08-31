# RuntimeSDK 接口选择与冲突协调指南

## 1. 先问资源属于哪一类

| 需求 | 应使用 |
|---|---|
| 固定代码/数据改写 | Hook Exclusive/State Patch |
| E8 CALL 串链 | Hook RelativeCall |
| IAT/vtable 串链 | Hook Pointer |
| 同目录配置/资源 | Path |
| 后台周期轮询 | Schedule |
| Windows 消息 | Window |
| 世界到屏幕坐标 | Display |
| RenderQueue/Present | Render |
| 未来业务服务 | 新的版本化领域接口 |

不要因为 QueryInterface 可扩展，就把所有业务塞进 Runtime 根函数表。

## 2. Hook 选择决策

### ExclusivePatch

当前位置只能有一个所有者，且修改是单向固定替换。适合完整 E9 stub、NOP 块、常量。

### StatePatch

原版和启用态都合法，配置决定最终状态。适合开关和旧补丁兼容。

### RelativeCallHook

目标是 5 字节 E8，多个同 ABI wrapper 需要共存。必须提供：

- 原版最终函数地址；
- wrapper 地址；
- 稳定签名 ID；
- phase/priority；
- 可选显式顺序约束。

### PointerHook

用于 IAT/vtable 的 4 字节函数指针槽。数据指针不能冒充函数链。

## 3. 顺序模型

排序键依次为：

1. phase；
2. 有限 priority；
3. 显式 before/after 约束；
4. 稳定 plugin ID。

文件名、mods.ini 顺序和 LoadLibrary 顺序都不能成为最终链语义。

## 4. Provider 模型

Runtime 对消费者暴露稳定门面，专业插件注册后端：

```text
消费者 -> Runtime门面 -> 当前Provider
```

消费者不得：

- GetModuleHandle 某个业务 ASI；
- GetProcAddress 后端私有函数；
- 读取后端 INI 推测状态；
- 缓存后端模块地址；
- 读取当前 CALL 目标判断谁已安装。

## 5. 可选能力处理

QueryInterface 返回 NOT_FOUND 或 OPTIONAL_UNAVAILABLE 时：

- 关闭该联动增强；
- 保留不依赖它的核心功能；
- 记录一次明确诊断；
- 不在整合模式切回私有冲突实现。

## 6. 锁与回调

Runtime 持锁时不能调用插件代码。插件回调也不能假设：

- 自己在游戏线程；
- Runtime 会替自己序列化业务状态；
- 可以在热路径读文件或加载 DLL；
- 可以长期保存 Runtime 私有对象指针。

热路径只读稳定函数表、句柄、generation 和 next 槽。

### Schedule 启动边界

- `IntegratedInitialize` 可以登记周期/一次性后台任务；
- Runtime 在 Bootstrap 期间只保存任务，不创建或运行 worker；
- 普通 ASI Loader 在 Entry Gate Bootstrap 完成后开闸；
- ModLoader 在阶段2完成、主线程实际返回 SDK Entry Gate 后开闸；
- 禁止用固定毫秒延时、插件 ID 排序或“我应该最后初始化”替代入口闸门。

## 7. 代次

Provider 切换或几何发布会产生 generation。消费者流程：

1. 读取状态/几何；
2. 保存 generation；
3. 发起投影/渲染请求时带回；
4. STALE 时重新读取，不用旧数据重试死循环。

## 8. 常见错误

- Runtime 存在时仍直接 VirtualProtect；
- 文件存在但 ABI 失败后转 Standalone；
- 用插件文件名作为身份；
- 在 DllMain 读 INI/LoadLibrary/创建业务线程；
- 每个插件各建一个 8ms worker；
- 多个插件各自 SetWindowLongPtr；
- 固定加 107/240 代替 Display；
- 读取 Render CALL 当前目标代替 Render 门面；
- 把未知机器码当成第三种 StatePatch 状态；
- 事务失败后继续安装剩余地址；
- Provider 未发布初始状态就标 ready；
- 产物构建成功但忘记打包依赖目录和说明。

## 9. 新领域接口评审条件

只有同时满足以下条件才新增领域接口：

- 至少两个插件需要共享该事实或资源；
- 直接相互 GetProcAddress 会产生长期耦合；
- 有明确的唯一权威或可组合规则；
- 能定义版本、结构大小、线程语义、ready/fault/generation；
- 缺失时可以明确降级；
- 不把单插件实验细节固化成公共 ABI。

新增接口先写设计和 ABI 测试，再写实现，最后迁移消费者。
