# 存档增强构建说明

本目录构建 `Castle_SaveEnhance.asi`。官方版必须与 `Castle_Runtime.dll` 同目录运行；可以脱离
Castle Mod Loader 和其它业务插件，手柄联动在没有 PadSupport 时自动不可用，键盘功能保留。

## 编译环境

Windows、Visual Studio C++ x86 工具链及 Windows SDK。脚本通过 vswhere 自动定位 Visual Studio，
也支持已初始化的 x86 开发者命令行。配置使用 UTF-8 TOML。脚本不会读取本机编译器地址记录，
也没有固定某台电脑的安装路径，适用于本地与 GitHub Actions。

从本目录运行 `build.bat`；无人值守时可用 `build.bat < nul`。整仓发行使用根 `build_all.bat`。

## 构建过程与产物

脚本编译业务 C++ 与 RuntimeSDK Client C 单元，链接 PE32/i386、无 CRT 的 ASI，再验证 DLL 标志、
非零入口和 `InitializeASI` 导出。任一编译、链接或验证失败都返回非零退出码。

单项目产物位于仓库 `build/`：

- `Castle_SaveEnhance.asi`；
- 从 `templete/` 复制的 `Castle_SaveEnhance.toml`。

中间文件只进入本项目 `_build`，结束后清理。整仓构建把正式文件放进 `build/mods/asi`，并建立
`Castle_SaveEnhance` 音效目录，复制音效/TOML 配置说明与实机清单。用户自备 WAV；构建不制造
占位音效。日志由 Runtime 独立写入 `mods/logs/Castle_SaveEnhance.log`。

## 验证与业务边界

`tools/verify_saveenhance_candidate.py` 验证只读原版 EXE、MiscInfo 与最终 ASI；参数和检查项见
`tools/工具详细说明.md`。宿主与静态验证通过后，还需按 `docs/SaveEnhance/实机测试清单.md` 验证
真实存读档、0/91～99 保留槽与手柄导航。原始光盘 EXE 的测试组合必须带 Runtime 与 NoCD。

SaveAction 由 Runtime Save 协调；配置、日志、WAV 文件、依赖和毫秒时间分别通过 TOML、Log、
File/Path、Module、Clock。`.NEXTAUTOSLOT` 保留既有三字节存档游标，它不是共享增强 `.state`。
