# 构建与验证说明

## 1. Windows 正式构建

在 Visual Studio 的 x86 Native Tools / Developer Command Prompt 运行 `build.bat`。需要 `cl.exe`、`link.exe`、`rc.exe` 和 Windows SDK。

正式输出：

```text
编译内容\CastleModLoader.exe
编译内容\mods\CastleLocaleBootstrap.dll
编译内容\mods\CastleModCore.dll
编译内容\mods\CastleModLoader.ini
编译内容\mods\asi\
编译内容\mods\overrides\...
```

`资源\RPG.ico` 通过 `launcher.rc` 编译进入 Launcher。

## 2. About C++ 单元

```text
源码\about.h
源码\about.cpp
```

`about.cpp` 只保存 About 标题、正文和链接数据。`build.bat` 以 C++ 编译它，但禁止 RTTI、异常、默认库和 STL/CRT 依赖；C/C++ 之间通过 `extern "C"` ABI 连接。

About 链接所需 `ShellExecuteW` 在运行时动态从 `shell32.dll` 解析，因此正式 Launcher 不需要静态链接 `shell32.lib`。

## 3. PE 约束

- Launcher：PE32/i386，静态依赖仅 `KERNEL32.dll`；
- LocaleBootstrap：PE32/i386，静态依赖仅 `ntdll.dll`；
- ModCore：PE32/i386，静态依赖仅 `KERNEL32.dll`；
- Launcher 保持 dev9 的 `SetDllDirectoryW` 启动期语义；
- ModCore 保持 dev9 的 `LoadLibraryExW` 与 `SetDllDirectoryW`；
- about5 不修改两个运行时 DLL。

正式 dev9 / about5 随包运行时二进制 SHA-256：

```text
CastleLocaleBootstrap.dll
923474730de3eb4587134b063c76c030ab816a1f6c8004fa06ff96e9f28be29e

CastleModCore.dll
2f1e54db006ce942ed66a3bfae502df7a39e63f64413ad2e7766c442ea887703
```

运行时源码聚合 SHA-256：

```text
369acf08a4353b8c083af989711d237c0a20fe193a7a81083ce709e56aab41a9
```

## 4. 本轮构建与交叉验证

当前容器没有完整 Windows SDK 的 `kernel32.lib` / `rc.exe` 正式链接环境，因此 about5 没有伪装成一次“完整 Windows 正式重建”。处理方式严格限制为：

1. 源码把 About 的 `CreateWindowExW` 扩展风格从 `WS_EX_CONTROLPARENT | WS_EX_TOOLWINDOW` 改为 `WS_EX_CONTROLPARENT`；
2. 全部 15 个 C 单元 + `about.cpp` 使用 `clang-cl` 的 i686/MSVC ABI 严格重新编译检查；
3. 随包 `CastleModLoader.exe` 直接继承 about4 已构建且带 RPG.ico 的 PE，只做两个等长字节修订：About `dwExStyle` 立即数和用户可见 `about4 → about5` 版本字符；
4. `CastleLocaleBootstrap.dll` / `CastleModCore.dll` 逐字节继承 dev9/about4。

因此 about4 → about5 的 Launcher 二进制应只有 **2 个字节**发生变化，证据见 `证据\当前版本验证\启动器最小二进制差异验证.txt`。从源码进行 Windows 正式完整重建仍以包内 `build.bat`（MSVC x86 Native Tools + Windows SDK）为权威。

本轮至少要求：

- 全部 15 个 C 单元 `/W4 /WX` 风格严格交叉编译通过；
- `about.cpp` 严格 C++ 编译通过；
- 本轮修改的 `launcher_gui.c` 与 `about.cpp` Clang Static Analyzer 0 diagnostic；
- `preloader_check.py` 当前版本结构检查全部通过；
- 最终 PE 再检查导入与图标资源；
- 两个运行时 DLL 与正式 dev9 SHA-256 精确一致。

静态检查不能替代 Windows 实机 About 链接、RichEdit 自动换行和 GUI 烟雾验收。
