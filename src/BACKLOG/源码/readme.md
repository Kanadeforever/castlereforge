# Castle Backlog 构建说明

本目录只放可复现构建所需的英文名源码、默认 INI 和本说明。功能设计、地址证据、使用方法、测试结论与完整接档位于同包的 `../文档`，构建脚本会从仓库 `docs/backlog` 全量同步。

## 构建环境

- Windows x64；
- Visual Studio 2022 或 18，安装“使用 C++ 的桌面开发”；
- LLVM `clang-cl`；
- 目标固定为 32 位 `i686-pc-windows-msvc`。

不需要 SDL SDK、SDL 头文件或 SDL import library。SDL3 在运行时按函数名动态发现；没有 SDL3 时 ASI 仍可加载并完整使用键盘。

## 构建

在本目录运行：

```bat
build.bat
```

成功后输出到 `../编译内容`：

- `Castle_Backlog.asi`；
- `Castle_Backlog.ini`；
- `文档/`（截至本版本的全部中文文档与完整接档）；
- `工具/backlog_check.py` 与中文工具说明。

脚本使用 `/nodefaultlib`，最终 ASI 只静态导入稳定的 `KERNEL32.dll` 与 `USER32.dll`。请勿把 SDL3 改成链接时依赖，否则会破坏“无 SDL 仍可使用键盘”的硬要求。
