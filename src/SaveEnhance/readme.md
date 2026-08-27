# Castle_SaveEnhance v0.1.0-test4 构建与静态验证结果

## 0. test4 本轮结果

针对 test3 实机结果，本轮源码已加入：

- 91~99 文件检查改为原版 `Save\SaveNNN.TSF` 相对工作目录语义；
- 自动存档成功后增加相同路径语义的可见性诊断；
- F5 在严格自由行动且当前地图已有冻结安全锚点时，原版 gate=0 允许 Writer fallback；
- `[Sound] Volume=0~100`；Volume<100 对 PCM WAV 内存副本缩放，100 原样播放；
- 新增 `ReadFile/GetFileSize/GetProcessHeap/HeapAlloc/HeapFree` KERNEL32 导入，仍无 CRT。

当前源码重新执行 x86 `/W4 /WX /O2` 编译通过；PE32/i386 链接通过；`InitializeASI` 导出、目标 EXE 地址、MiscInfo、KERNEL32 导入、无 CRT 静态验证全部 PASS。

## 1. 本轮修复背景

用户实机运行 test2 时，日志只输出到：

```text
[配置] QuickLoadPresses=2 WindowMs=1200 AutoIntervalMin=5。
```

之后既没有“完整 Hook 安装成功”，也没有 fail-closed 错误；0~99、随时安全存档、Quick/Auto 全部没有生效。

对照 Castle Mod Loader 当前源码确认：Loader 在 `LoadLibraryExW` 返回以后，先给 ASI 补 `OverrideLoader_PatchModule` / `LocaleLayer_PatchModule`，随后通过 `GetProcAddress(module, "InitializeASI")` 调用可选正式初始化。test2 却把 INI、模块查询、VirtualQuery/VirtualProtect 和全部机器码 Hook 安装放在 `DllMain` 的 `DLL_PROCESS_ATTACH` 中。

**test3 修正：** `DllMain` 只保存 HMODULE 并调用 `DisableThreadLibraryCalls`；正式初始化全部移动到导出的 `InitializeASI()`。

## 2. 构建参数

```text
C++17
/utf-8
/O2
/Oi-
/W4 /WX
/GR-
/GS-
/Gs999999999
/Zl
/NODEFAULTLIB
/MACHINE:X86
/ENTRY:DllMain
```

当前 `source/Castle_SaveEnhance.cpp` 已使用 x86 clang-cl 从零重新编译，`/W4 /WX` 零告警通过。

正式 Windows 构建请在 VS x86 Native Tools 运行包根 `build.bat`。test4 的 `build.bat` 除 PE32/i386 基础检查外，还使用 `dumpbin /exports` **强制确认 `InitializeASI` 导出存在**。

## 3. 容器交叉链接说明

研究环境没有完整 Windows SDK，因此随包测试候选用最小 KERNEL32 import library 做第二套 x86 ABI/PE 链接。临时库产生 `_Foo@N` 名称后，只对 PE 的 IMAGE_IMPORT_BY_NAME 文本原地规范化成真实 KERNEL32 `Foo` 名称，不移动任何 RVA。正式 Windows `build.bat` 使用真实 `kernel32.lib`，不需要这一处理。

## 4. 最终 test4 ASI

SHA-256：

`34c0fe8fb78cad1e4273bfccc9a72cea97c7c7d38662920ea8063b6e3b4bb0d0`

静态验证：

- PE32/i386：PASS；
- DLL flag：PASS；
- EntryPoint 非零：PASS；
- **Export Table 含 `InitializeASI`：PASS；**
- 静态只导入 KERNEL32：PASS；
- KERNEL32 import 均为未装饰真实名称：PASS；
- 新增 `ReadFile/GetFileSize/GetProcessHeap/HeapAlloc/HeapFree`：PASS；
- 无 CRT/UCRT/VCRUNTIME：PASS；
- user32/winmm 保持运行时可选动态依赖：PASS。

## 5. RPG 原版地址验证

全部 PASS：

- 10 个固定菜单补丁；
- 2 个循环分页点；
- 5 个 CALL Hook；
- SaveAction Update vtable；
- 原版 save gate；
- SaveSlot/LoadSlot；
- `%03d` / `Save` / `.TSF` 命名字符串。

## 6. MiscInfo

全部 PASS：

- size 1054；
- decoded SHA-256 `cd18d110d458fe60928eb874f76453f751489cf2d0341d2c7c250c4ece13e6e9`；
- `+0x3C2 = 8`。

## 7. 结论边界

静态验证已经证明 test4 的生命周期导出、PE/ABI、目标原版机器码与新增 KERNEL32 导入一致。

**test3 已经实机证明 `InitializeASI` 与完整 Hook 安装能够执行。** test4 尚未实机证明的是本轮三个修复：91~99 相对路径轮换、Quick fallback、WAV Volume。
