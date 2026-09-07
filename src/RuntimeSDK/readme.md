# RuntimeSDK 构建说明

## 当前定位

`Castle_Runtime.dll` 是 CastleReforge 官方 ASI 必需的基础引擎。官方插件仍可脱离 Castle Mod
Loader和其它业务插件运行，但必须与 Runtime DLL 位于同一 ASI 目录。Runtime 不由 Loader 直接
扫描，而由 SDK Client 在 Loader Lock 外加载。

单独构建输出：

```text
build\Castle_Runtime.dll
```

根 `build_all.bat` 最终部署为：

```text
build\mods\asi\Castle_Runtime.dll
```

## 已实现服务

- 根 API、Registry、Diagnostics、稳定 ID 和固定驻留；
- Client、Entry Gate、统一 Bootstrap 和 ModLoader Loader-ready 闸门；
- Hook 独占/双态补丁、CALL/IAT/vtable 链、事务与回滚；
- Path、File、TOML 和 Log；
- Module 与 Clock；
- Schedule 与 Window；
- Input 与 GameState Mutation Lease；
- SaveAction 槽位策略；
- Display、Render 和 Overlay。

所有公共接口位于 `include`，实现只在 `source`。复杂业务仍属于各 ASI。

## 构建

运行：

```bat
build.bat
```

脚本通过 `vswhere` 或当前 PATH 定位 MSVC x86，通过 PATH 定位 clang-cl，通过 PATH 定位
Python。不得读取开发者机器的编译器地址记录或写死安装路径。

依赖：Visual Studio C++ x86、Windows SDK、clang-cl、Python 3。

## 自动测试

构建依次验证：

1. MSVC/clang-cl 的 x86 C/C++17 ABI；
2. 无 CRT Runtime DLL 编译与链接；
3. Runtime 综合宿主；
4. Hook 事务、链、回滚和资源冲突；
5. Path、Schedule、Window、Display、Render；
6. Log、File、TOML、Module、Clock；
7. Input Provider/焦点与 GameState Mutation；
8. Save/Overlay 在非 RPG 宿主中的安全 NotReady；
9. Entry Gate Owner/Follower/冲突/晚注入；
10. 正常 Runtime、无 Runtime、损坏 Runtime、官方强制 Runtime 缺失；
11. ModLoader 外层通知前零任务、通知后运行；
12. 公共头、DEF、PE32、唯一导出和 KERNEL32-only。

最近一次单独构建为 `294 PASS / 0 FAIL`。数字会随检查项增加，以实际构建输出为准。

## 目录职责

```text
include/  稳定纯 C ABI
source/   Castle_Runtime.dll 私有实现
client/   编进每个 SDK ASI 的轻量 Client
tests/    ABI、宿主、假 ASI 和故障测试
tools/    机械检查器及本说明
```

RuntimeSDK 当前只允许本 `readme.md` 和 `tools/工具详细说明.md` 位于 src。完整设计与迁移指南在：

- `docs/runtime/运行时协调系统总体设计.md`；
- `docs/runtime/截至该版本的完整接档.md`；
- `docs/common/新格式ASI插件制作指南.md`；
- `docs/common/独立ASI迁入RuntimeSDK指南.md`；
- `docs/common/独立与整合并行验收标准.md`。

## 当前边界

- FPSUnlock 已接 SDK，但仍未完成，不进入 build_all；
- `SaveXXX.state` 尚无实际消费者，本版不安装空的 State 生命周期 Hook；
- SaveEnhance 的 `.NEXTAUTOSLOT` 是既有三字节兼容游标，不是 `.state`；
- 静态/宿主测试不能替代 RPG.exe 全插件实机验收。
