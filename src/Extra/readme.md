# Extra 三功能构建与接档说明

## 当前状态

`src/Extra` 维护三个独立官方 ASI：

- `BUGFix`：原版问题修复及已验证的读档后新游戏 CrashFix；
- `NoCD`：跳过原版光盘检查，并设置安全备用盘符；
- `MaxGrowthAndDrop`：最大成长与最大掉宝，配置为 `MaxGrowthAndDrop.toml`。

三个插件都需要同目录 `Castle_Runtime.dll`，但不要求 Castle Mod Loader 或其它业务插件。
所有 RPG.exe 补丁通过 Runtime Hook 事务，日志通过 Runtime Log 分别写入 `mods/logs`。
MaxGrowthAndDrop 通过 Runtime TOML 读取配置；缺少 Runtime 时三者保持停用。

SaveEnhance 已是同级独立项目 `src/SaveEnhance`，产物为 `Castle_SaveEnhance.asi`，不属于
Extra，也不再使用历史 `AnytimeSave.asi` 名称。

## 目录与工具边界

每个功能目录包含自己的 `build.bat`、`source` 和最小头文件，可单独构建。三个
`PatchUtil.h` 只保留补丁描述、RPG.exe 基址和只读字节比较；没有 VirtualProtect、本地 CALL
安装或手工回滚。各自 `Win32Mini.h` 也只声明该插件当前真实使用的 Win32 能力：

- NoCD：模块基址、EXE 路径和 DLL 生命周期；
- MaxGrowthAndDrop：模块基址和 DLL 生命周期；
- BUGFix：只读内存检查、私有执行 stub 分配/释放与指令缓存刷新。

BUGFix 的 stub 是插件自己的短汇编路径；写入 RPG.exe 的跳转仍由 Runtime 事务执行。

## 构建

分别运行：

```text
src\Extra\BUGFix\build.bat
src\Extra\NoCD\build.bat
src\Extra\MaxGrowthAndDrop\build.bat
```

脚本通过 vswhere 定位 MSVC x86，使用 C++17、UTF-8、`/W4 /WX`、无 CRT，并在各自 `_build`
存放中间文件。单项产物先写仓库根 `build`；正式发行由根 `build_all.bat` 移到
`build/mods/asi`。构建后检查 PE32/i386、DLL 标志和非零入口。

## 验收与限制

当前只支持已确认的台湾第三版原版 RPG.exe 或机器码等价状态。每项声明都列出原版/启用态，
陌生字节由 Runtime fail-closed。原始光盘 EXE 的最小验收为 Runtime + NoCD；其余两个插件分别
加入测试，然后再验证三者及所有正式 ASI 联合加载。

历史 v0.3.2 的地址、失败路线与实机证据保存在 `docs/Extra/文档`，其中旧 INI、ASI 同目录
日志、本地 PatchUtil 写码和 AnytimeSave 描述只作为历史记录。当前架构以
`docs/runtime/运行时协调系统总体设计.md` 和完整接档为准。
