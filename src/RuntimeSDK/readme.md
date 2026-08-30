# RuntimeSDK 构建说明

## 当前里程碑

当前只完成公共 ABI v1 的第一批头文件、导出表和机械布局检查器。

本阶段明确不会：

- 生成 `Castle_Runtime.dll`；
- 安装 RPG.exe Entry Gate；
- 修改任何游戏内存；
- 接入现有 ASI；
- 修改 MODLoader；
- 把未完成 FPSUnlock 纳入构建。

施工权威文档位于：

- [`../../docs/runtime/运行时协调系统总体设计.md`](../../docs/runtime/运行时协调系统总体设计.md)
- [`../../docs/runtime/截至该版本的完整接档.md`](../../docs/runtime/截至该版本的完整接档.md)

## 构建用途

运行：

```bat
build.bat
```

脚本会把同一个 `tests/abi_layout_test.c` 分别按四种模式编译：

1. MSVC x86 C；
2. MSVC x86 C++17；
3. Clang x86 C；
4. Clang x86 C++17。

然后运行 `tools/runtime_sdk_check.py` 检查：

- magic、版本和固定结构大小；
- 根函数顺序；
- 插件 Query/Client 合同；
- Hook/双态补丁合同；
- Display/WorldToScreen 合同；
- 公共头没有 Windows/CRT/STL 泄漏；
- `.def` 只有唯一稳定导出；
- build.bat 中文行尾规则。

成功后 `_build` 会清理。因为当前还没有 Runtime 实现，所以不会向仓库根 `build` 输出 DLL。

## 编译器路径

本机路径依据 `C:\Project\编译器地址记录.txt`：

- Visual Studio 18 / MSVC 19.51 x86；
- `C:\msys64\clang64\bin\clang.exe`；
- `C:\msys64\clang64\bin\clang++.exe`；
- Python 3.14。

MSVC 由 `VsDevCmd.bat -arch=x86 -host_arch=x64` 初始化，不能直接使用 Hostx64/x64 编译器冒充 x86。

## 目录职责

```text
include/   第三方和官方插件共同包含的纯 C ABI
source/    未来 Castle_Runtime.dll 私有实现与导出表
client/    未来编进每个 ASI 的轻量 Client
tests/     编译期布局与以后运行期测试
tools/     RuntimeSDK 自制检查工具
```

`src/RuntimeSDK` 内除本 `readme.md` 和 `tools/工具详细说明.md` 外不放普通文档；设计、测试结论和完整接档统一放在 `docs/runtime`。

## 第一批 ABI 文件

- `CastleRuntime_API.h`：根类型、错误码、Bootstrap、QueryInterface 和根函数表；
- `CastlePlugin_API.h`：插件描述、ClientConfig、`CastlePlugin_Query` 返回表；
- `CastleRuntime_Client.h`：Entry Gate/InitializeASI Client 状态与入口；
- `CastleHook_API.h`：事务、独占/双态补丁、CALL/IAT/vtable 链；
- `CastleDisplay_API.h`：显示几何、WorldToScreen 和 Display Provider；
- `CastleRuntime.def`：未来 DLL 唯一稳定导出 `CastleRuntime_GetApi`。

## 下一步

只有本里程碑在全部四种编译模式和文本检查中通过后，才开始实现：

1. Runtime 最小 DLL；
2. 插件登记与诊断；
3. Client 同目录发现；
4. SDK Entry Gate 测试宿主；
5. MODLoader 两阶段接口测试。

现有插件必须等 RuntimeSDK 发布候选验收完成后再适配。
