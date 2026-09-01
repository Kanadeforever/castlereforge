# RuntimeSDK 构建说明

## 当前状态

RuntimeSDK v1 已实现并进入静态/构建候选阶段，输出固定文件：

```text
build\Castle_Runtime.dll
```

根 `build_all.bat` 随后把它移动到：

```text
build\mods\asi\Castle_Runtime.dll
```

Runtime 与 ASI 同目录，但 MODLoader/ASI Loader 不直接扫描或加载 `.dll`；SDK ASI 的 Client
在 Loader Lock 外主动加载。没有 DLL 时插件独立运行，文件损坏时故障安全停用。

## 已实现内容

- 根 API、插件登记、稳定 ID 排序、固定驻留和诊断；
- Client、Entry Gate Owner/Follower、统一 BootstrapAll；
- Hook 独占/双态补丁、CALL/IAT/vtable 链、事务回滚和稳定 next；
- Path UTF-8/UTF-16；
- Schedule 单后台线程；
- Window Observer/Filter；
- Display 默认/专业后端和投影；
- Render 默认/专业后端和额外帧租约；
- 七个首批官方 ASI 适配；
- MODLoader 两阶段兼容；
- Clock 按未完成 FPSUnlock 延期。

## 构建

运行：

```bat
build.bat
```

脚本通过 `vswhere` 或当前开发环境定位 MSVC x86，通过 PATH 定位 `clang-cl`，通过 PATH
定位 `python.exe` 或 `py.exe -3`。脚本禁止读取开发者机器上的私人编译器路径记录，也不写
固定 VS/LLVM/Python 安装目录，因此本地和 GitHub Actions 使用同一入口。

依赖：

- Visual Studio C++ x86 工具；
- clang-cl；
- Python 3；
- Windows SDK。

## 自动测试

构建会依次执行：

1. MSVC x86 C ABI；
2. MSVC x86 C++17 ABI；
3. clang-cl x86 C ABI；
4. clang-cl x86 C++17 ABI；
5. 无 CRT Runtime DLL 编译/链接；
6. Client/Entry Gate 编译；
7. Runtime 综合宿主；
8. 两个逆序测试 ASI 的稳定 ID Bootstrap；
9. Entry Gate Owner/Follower/冲突/晚注入测试；
10. Client 并发状态机测试；
11. 正常 Runtime、无 Runtime、损坏 Runtime 三目录测试，包括损坏映像无系统弹窗和线程错误模式恢复；
12. ModLoader 两阶段 Schedule 闸门：A 登记任务、B 延迟初始化及外层通知前零执行，Loader-ready 后才放行；
13. Clang 严格检查 GetProcAddress/FARPROC 到 SDK cdecl 函数指针的统一安全转换；
14. 公共头、DEF、PE 和静态导入机械检查。

当前 RuntimeSDK 机械检查为 `165 PASS / 0 FAIL`。完整 `build_all.bat` 的发行检查为
`195 PASS / 0 FAIL`，也可以单独运行：

```bat
python tools\runtime_sdk_check.py --require-release
```

它还会检查七个 ASI 的标准导出、全部发行 PE、SaveEnhance 随包说明和 `build` 零编译垃圾。

## 目录职责

```text
include/   官方插件和第三方 Mod 共用的纯 C ABI
source/    Castle_Runtime.dll 私有实现
client/    编进每个 SDK ASI 的轻量 Client
tests/     ABI、宿主、假 ASI、Entry Gate 和三模式测试
tools/     机械检查器及简体中文工具说明
```

`src/RuntimeSDK` 内只保留本 `readme.md` 和 `tools/工具详细说明.md` 两类允许文档。
设计、迁移和验收文档位于：

- [`../../docs/runtime/运行时协调系统总体设计.md`](../../docs/runtime/运行时协调系统总体设计.md)
- [`../../docs/runtime/截至该版本的完整接档.md`](../../docs/runtime/截至该版本的完整接档.md)
- [`../../docs/common/新格式ASI插件制作指南.md`](../../docs/common/新格式ASI插件制作指南.md)
- [`../../docs/common/独立ASI迁入RuntimeSDK指南.md`](../../docs/common/独立ASI迁入RuntimeSDK指南.md)
- [`../../docs/common/独立与整合并行验收标准.md`](../../docs/common/独立与整合并行验收标准.md)

## 当前边界

静态、ABI、故障注入和完整构建已经通过。目标 RPG.exe 的七插件联合实机尚未执行，因此当前
产物是发布候选，不应跳过实机清单直接宣称最终稳定版。FPSUnlock 仍不属于本轮构建。
