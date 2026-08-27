# Extra 三功能源码完整接档说明

## 1. 当前状态

`Extra` 现只负责三个相互独立的小型功能：

- `BUGFix`：修复原版 BUG，并包含已稳定验证的 CrashFix test2 双调用路径修复；
- `NoCD`：跳过原版光盘检查；
- `MaxGrowthAndDrop`：最大成长与最大掉宝，可由同名 INI 控制。

安全存档增强功能已在 2026-08-28 移到同级的 `src/SaveEnhance`，作为单独主功能维护和构建；正式产物仍名为 `AnytimeSave.asi`。本次整理只改变文件位置与构建路径，没有修改任何 C++ 实现、补丁地址、机器码、配置键或运行行为。

## 2. 当前目录

```text
src/Extra/
  BUGFix/
    build.bat
    source/
      BUGFix.cpp
      PatchUtil.h
      PluginLog.h
      Win32Mini.h
  NoCD/
    build.bat
    source/
      NoCD.cpp
      PatchUtil.h
      PluginLog.h
      Win32Mini.h
  MaxGrowthAndDrop/
    build.bat
    source/
      MaxGrowthAndDrop.cpp
      PatchUtil.h
      PluginLog.h
      Win32Mini.h
  完整接档说明.md
```

三个功能各自保留完整 `build.bat`、源码和一套同版本公共头文件，使任一功能目录都能独立阅读、独立构建，不依赖 Extra 根脚本或已经移出的旧 `source` 汇总目录。`src/Extra` 不再提供 `build.bat`；仓库根 `build_all.bat` 会直接调用三个功能脚本。以后若修改公共头文件，必须同步核对三个目录以及 `src/SaveEnhance/source` 中的副本。

## 3. 构建方法

单独构建某个功能时，直接运行对应脚本：

```text
BUGFix/build.bat
NoCD/build.bat
MaxGrowthAndDrop/build.bat
```

每个功能脚本都能自行定位 Visual Studio x86 MSVC 工具链、管理自己的 `_build` 中间目录，并把唯一目标输出到仓库根 `build` 目录：

```text
BUGFix.asi
NoCD.asi
MaxGrowthAndDrop.asi
```

构建参数和整理前保持一致：C++17、UTF-8、`/W4 /WX`、不链接 CRT、目标为 x86 DLL，并显式使用 `DllMain` 作为入口。每个产物链接完成后仍会检查：

- PE 机器类型为 `0x014C`，即 x86；
- PE 带 DLL 标志；
- `AddressOfEntryPoint` 不为 0。

`MaxGrowthAndDrop.ini` 的既有生成与部署方式没有在本次目录整理中改变。

## 4. 已确认方案与历史结论

- `BUGFix` 原有固定补丁和 Crash 双路径修复在 v0.3.2 已完成静态复核；
- `NoCD` 与 `MaxGrowthAndDrop` 在 v0.3.2 沿用已核对实现；
- 三个插件继续只依赖 `KERNEL32.dll`；
- 所有内存写入仍先验证已知机器码，陌生版本会安全拒绝；
- 更完整的地址、架构、成功方案、失败方案、实机结论和风险记录位于 `docs/Extra/文档`。

## 5. 已知限制与当前阻塞

- 功能仍只支持已经确认的 32 位 `RPG.exe` 基线或机器码等价状态；
- 目录拆分不会扩大兼容范围，也不会替代实机回归；
- 本次没有发现新的代码阻塞项。

2026-08-28 已使用当前 MSVC x86 工具链实编译通过：`BUGFix.asi / NoCD.asi / MaxGrowthAndDrop.asi` 均生成成功，并分别通过 x86、DLL、非零入口检查。入口 RVA 依次为 `0x00001DE0 / 0x00001140 / 0x000011D0`。

三个脚本分别独立通过后，又在 LLVM 已加入 `PATH` 的环境下运行仓库根 `build_all.bat`；八个步骤全部成功，根脚本确认会直接调用三个功能入口，不需要 `src/Extra/build.bat`。

## 6. 下一步

1. 每次修改任一公共头文件时，同步四套正式源码副本并做差异检查；
2. 后续改动后先运行被修改功能自己的 `build.bat`，再运行仓库根 `build_all.bat` 做全项目回归；
3. 功能变化仍按 `docs/Extra/文档/测试说明.md` 做实机回归；
4. 不把安全存档增强源码重新并回 `Extra`，其独立入口为 `src/SaveEnhance/build.bat`。
