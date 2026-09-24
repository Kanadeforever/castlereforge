# 存档增强构建说明

本目录构建 `Castle_SaveEnhance.asi`，当前候选为v0.3.0-test7。官方版必须与 `Castle_Runtime.dll` 同目录运行；可以脱离
Castle Mod Loader 和其它业务插件，手柄联动在没有 PadSupport 时自动不可用，键盘功能保留。

## 编译环境

Windows、Visual Studio C++ x86 工具链及 Windows SDK。脚本通过 vswhere 自动定位 Visual Studio，
也支持已初始化的 x86 开发者命令行。配置使用 UTF-8 TOML。脚本不会读取本机编译器地址记录，
也没有固定某台电脑的安装路径，适用于本地与 GitHub Actions。

从本目录运行 `build.bat`；无人值守时可用 `build.bat < nul`。整仓发行使用根 `build_all.bat`。

## 构建过程与产物

脚本编译存档核心、SaveVisual.cpp与RuntimeSDK Client C单元，链接PE32/i386、无CRT的ASI，再验证DLL标志、
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
File/Path、Module、Clock。`.SAVESTATUS`合并最新手动槽、最新自动槽及轮换游标，不是共享增强`.state`。

## 可视反馈与测试

可视层只注册Runtime Overlay/Display回调，不新增Present Hook，不改其它插件。书卷和Font24字形在运行时
只读加载，无需提取资源随ASI分发；Visual参数见模板。真实存档目录的`.SAVESTATUS`从旧`.LATESTSLOTS`和
`.NEXTAUTOSLOT`安全迁移；写入成功才清理旧文件。快速槽不占“新”标记，不写回配置。
槽位字重-100～200细调（负数减轻边缘、正数额外加粗）、垂直偏移默认-2；SlotTextOutline为独立一像素描边开关。
保留test6标题阶段2复用修复，test7增加565运行时光标解码；负字重、标题重进和光标遮挡均获用户实机验收，未改Controller或书卷提示。
批处理不含注释、不复制Markdown，echo原有尾随空格保留。

从仓库根运行宿主测试（需要MSVC x86，不启动游戏）：

```text
python -B -X utf8 src/SaveEnhance/tools/test_savevisual.py --game-directory "参考资料/Castle"
```

参考资源只读，测试状态和编译物在本模块`_build`下隔离并清理。test7有1359项宿主检查、0失败。
新增重进回归在旧代码复现4项失败，修复后通过。用户已验收书卷、提示描边、无宽屏提示及自动“新”，本轮不改这些效果。
运行时565回归在旧解码方式复现13项失败，修复后通过；用户随后独立确认光标覆盖、负字重和标题重进实机通过。
