> 当前权威版本：v0.3-refactor37（R36 底座重做候选）。此前废弃的旧 R37 实验实现不作为继承基线；本版只从中移植原版互动 resolver 调查能力。下文若保留 R36 标题或历史说明，与本节冲突时一律以本节为准。

# 新 R37 构建补充

build.bat以x86 MSVC、/W4 /WX /utf-8 /GS- /Zl /nodefaultlib逐一编译29个C文件。部署只需ASI、INI和兼容x86 SDL3.dll；不得打包RPG.exe或构建中间件。

---

# 构建与部署说明

> **v0.3-refactor36 当前候选（2026-08-22）：** refactor33 的统一商店主体已经用户实机确认；refactor35 实机反馈为各商店主体可用，但上下到列表边界不会连续翻页、移动项目会自动打开道具信息且 Y 无法关闭。本轮只修 Shop：上下越界分别调用原版上一页/下一页并落到新页末项/首项；左右显式翻页尽量保留当前行；两侧列手形相对 refactor35 统一向左 10 像素、向下 5 像素。
>
> **Y 道具信息裁决：** `0x417829` 与公共 `CALL_CMD0_EVENT` 是同一条原版 close Event CALL，必须复用全局 Hook，不得再次安装；`info+0x580` 只表示打开/关闭动画，稳态必须比较 `info+0x56C` 与原版关闭/打开目标 `+0x588/+0x58C`。方向/X/翻页只更新原版项目链并抑制最后开窗步骤；仅 Y 放行打开，已打开时 Y 点击 `info+0x598` 的真实关闭按钮。信息窗及动画期间其它商店输入保持模态阻断。
>
> **边界与验证：** 不按装备店、道具店、药店名称分支，继续复用 opcode `0x36 -> 0x413FA0 -> 0x89FCD4` 的同一 Shop Adapter；价格、库存、页、行、数量与成交结果均不直接写。天书偶发跳焦点继续低优先级，Tome/SaveSlot/Cursor 六文件逐字节保持 refactor33。27/27 C 单元严格编译通过，两次无时间戳 PE32/i386 链接字节一致；无 EXE **73 PASS / 0 FAIL**，未修改 `RPG.exe.org`（SHA-256 `8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f`）**93 PASS / 0 FAIL**。ASI **153,600 bytes**，SHA-256 `05e7ef5e96f32d6a55a819a5cb590ac2302adb9441564495a3df62882af2ef0c`。机器 PASS 不冒充实机 PASS。
>
> **交付裁决：** 从本版起不再内置散装 `证据/` 树；源码、文档与检查器构成接档材料。若以后确有新增证据，只合并维护单一 `文档/验证汇总.md`。
>
> **历史阅读规则：** refactor32 的存档点完整路由与客栈原位返回、refactor33 的地图十字键仅步行和统一商店主体均已验收并冻结；refactor34 的全局 Shop 形状硬门、refactor35 对共享 close Event 的重复 Hook 与对动画 mode 的稳态误判均已作废。当前修正以本文件顶部 refactor36 段落为准。

## refactor36 构建增量

- Windows 构建仍为 27 个独立 C 单元、x86、无 CRT；`build.bat` 已升级 refactor36 标记。
- GCC 严格复核参数包含 `-m32 -std=c11 -O2 -ffreestanding -Wall -Wextra -Werror`，27/27 通过。
- 两个全新对象目录分别链接，产物逐字节一致：153,600 bytes，SHA-256 `05e7ef5e96f32d6a55a819a5cb590ac2302adb9441564495a3df62882af2ef0c`。
- 产物为 PE32/i386 DLL，固定 image base `0x10000000`，启用重定位、DynamicBase 与 NXCompat，无时间戳差异。
- 完整包仅含源码、文档、检查器和编译内容，不再打包散装 `证据/`。

## refactor35 构建增量（历史）

- 编译单元仍为 27 个；没有新增 DLL、INI 键或第三方运行时依赖；
- 主体仍安装 refactor33 的 11 个 Shop ButtonEvent；信息窗关闭的第 12 个 Event 和两处 transition 作为独立可选能力安装；
- `interface_tome.c/.h`、`save_slot.c/.h`、`cursor.c/.h` 与 refactor33 交付版 SHA 逐字节一致；
- GCC x86 27/27 在 `-Wall -Wextra -Werror` 下通过；两个独立临时目录的无时间戳 PE32/i386 链接结果逐字节一致；
- 最终 `Castle_PadSupport.asi`：**143,360 bytes**，SHA-256 `d98fe6dd526ee9f40e67953711cbb8df3a2bf11973fcb43b63efca2bc129c24e`；
- PE 为 Intel i386 / PE32 GUI DLL，保留 DYNAMIC_BASE、NX_COMPAT、NO_SEH、`.reloc` 与 1 MiB stack；部署仍只替换 ASI/INI，沿用兼容 x86 SDL3.dll。

## refactor34 构建增量（历史；实机回归失败）

- 编译单元仍为 27 个；本轮没有新增 DLL、配置键或第三方依赖；
- `shop.c` 新增两处信息 transition Hook，`ui_bridge.c` 的 Shop ButtonEvent Hook 从 11 增到 12；`runtime.c` 增加信息窗与 vtable 精确预检；
- `interface_tome.c/.h`、`save_slot.c/.h`、`cursor.c/.h` 与 refactor33 交付版 SHA 逐字节一致；
- GCC x86 27/27 在 `-Wall -Wextra -Werror` 下通过；GNU `ld -m i386pe` 两个独立临时目录无时间戳链接结果逐字节一致；
- 最终 `Castle_PadSupport.asi`：**142,848 bytes**，SHA-256 `fa3c994fcc71795a6457e098924ba18c519f50f5134e221cbd5133234ecabe63`；该产物静态检查通过但实机所有商店无响应，不得部署；
- PE 仍为 Intel i386 / PE32 GUI DLL，保留 DYNAMIC_BASE、NX_COMPAT、NO_SEH、`.reloc` 与 1 MiB stack；部署仍只替换 ASI/INI，继续使用现有兼容 x86 SDL3.dll。

## refactor33 构建增量（历史）

- Windows `build.bat` 现逐个编译 27 个独立 C 单元，新增 `shop.c`，仍使用 x86 `/W4 /WX /utf-8 /GS- /Zl /nodefaultlib`；
- 容器复核使用 `gcc -m32 -std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -Werror`，27/27 PASS；
- GNU `ld -m i386pe` 以 DLL/GUI、入口 `DllMain`、image base `0x10000000`、DYNAMIC_BASE/NX_COMPAT/NO_SEH、重定位与 1 MiB stack 链接；固定时间戳后连续两次输出字节一致；
- 最终 `Castle_PadSupport.asi`：139,776 bytes，SHA-256 `5c56706471d71b921145151159591d804b5e1c85ea68aac4277c65eabe932ee8`；
- 部署文件仍只有 ASI、INI 与兼容的 x86 `SDL3.dll`；不部署逆向资料或 `RPG.exe.org`。

## refactor32 构建增量（历史，已验收冻结）

- 源码仍为 **26 个独立 C 编译单元**；refactor32 没有新增单元，只调整 Inn、SaveSlot、Runtime、Plugin、SceneChoice 与边界文档。
- 包内 Windows 正式入口仍是 `源码/build.bat`：MSVC x86 `/O2 /GS- /Zl /W4 /WX /utf-8 /TC`，链接 `/dll /nodefaultlib /machine:x86 /entry:DllMain@12`。
- `build.bat` 保持 UTF-8 BOM + CRLF，并逐个列出 26 个单元；任一编译/链接失败立即停止，不能保留旧 ASI 冒充新产物。
- 当前容器没有 MSVC/clang-cl，交付 ASI 使用同一 32 位 ABI 的 `gcc -m32` 严格编译，并由 GNU `ld -m i386pe` 以 DLL、无 CRT、`DllMain` stdcall 入口、ASLR/NX/NO_SEH 与重定位表链接。
- 产物已确认 `DllMain` 以 `ret 0x0C` 返回，格式为 PE32 / Intel i386 DLL，不存在 CRT DLL 依赖；综合工具验证 refactor32、wrapper Adapter、客栈原位返回与存档点深层询问标记，拒绝旧二进制。
- refactor32 ASI（历史）：131,584 bytes；SHA-256 `1eac7488d4088f8a8fe6e71ce9feb45147fdaa6d2daed2380f90de95617dedd8`。

部署内容仍只有 `Castle_PadSupport.asi`、`Castle_PadSupport.ini` 与既有兼容 x86 `SDL3.dll`。把 ASI/INI 放进游戏目录即可；本轮没有新增运行时 DLL 或配置键。

## refactor31 构建增量（历史）

refactor31 首次增加 `save_point.c`，总数变为 26 个单元；其 ASI 为 131,072 bytes、SHA-256 `5cc5e3bbf89c2a02953d3157dd505821703271c1d67d7c403f209bb5bc50d40e`，不包含 refactor32 的深层询问与来源项返回修正。

## refactor30 构建增量（历史）

refactor30 当时为 25 个单元，并因错误的 state7 假设没有建立存档点 Adapter；该构建记录只用于历史比对。

## refactor29 构建增量

- 源码现在有 **25 个独立 `.c` 编译单元**；新增：`inn.c`、`synthesis.c`、`scene_choice.c`。
- `build.bat` 使用 UTF-8 BOM + CRLF，开头 `chcp 65001 >nul`；MSVC x86 `/W4 /WX /utf-8 /GS- /Zl`，链接 `/machine:x86 /nodefaultlib /entry:DllMain@12`。
- 本轮 Linux 容器复核使用 `clang-cl --target=i686-pc-windows-msvc` 逐个编译 25/25，再用 `lld-link /dll /machine:x86 /nodefaultlib` 链接；正式 Windows 用户仍可直接运行包内 `build.bat`。
- 部署文件仍只有 `Castle_PadSupport.asi`、`Castle_PadSupport.ini` 和兼容 x86 SDL3.dll；refactor29 没有新增运行时第三方 DLL。


## refactor28 历史封存（当时现行）构建与部署

- 架构：Win32 / x86；RPG.exe 为 32 位，禁止输出 x64 ASI。
- 源码：22 个 `.c` 独立编译单元；refactor28 **没有新增编译单元**，治疗目标仍属于 `interface_skills.c` 的 state3 页面生命周期。
- 严格参数：`/W4 /WX /utf-8 /GS- /Zl`，链接 `/machine:x86 /dll /nodefaultlib /entry:DllMain@12`。
- 正式 Windows 构建入口：`源码/build.bat`；BAT 使用 UTF-8 BOM + CRLF，并先执行 `chcp 65001 >nul`。
- 当前容器交叉结构验证：22/22 `clang-cl --target=i686-pc-windows-msvc` 编译 PASS，`lld-link` 输出 PE32/i386 DLL PASS。
- 当前 ASI：140,800 bytes；SHA-256 `82da61da9aeb3edce7c1e6797949034e93e5a26d8fcfcf0439e9472fba8a6c6e`。
- 部署：`编译内容/Castle_PadSupport.asi` 与 `Castle_PadSupport.ini` 放游戏目录，并使用兼容 x86 SDL3.dll；现行实机加载基线仍为 Ultimate ASI Loader 7.2 的 `binkw32.dll` 入口。

refactor28 没有新增运行库和外部 DLL。

---

## 历史累计记录（只作因果证据）

> 以下内容保留各历史版本当时的设计、失败方案、地址和测试记录。里面出现的“当前/待验/下一步”不再代表 refactor28 现行状态。

## 0. refactor27 构建现状

源码现在共有 **22 个独立 `.c` 编译单元**。新增的两个单元为：

```text
interface_tome.c
interface_options.c
```

正式 Windows 构建继续运行：

```bat
源码\build.bat
```

脚本保持 x86、`/W4 /WX /utf-8`、`/GS- /Zl /nodefaultlib` 等既有约束，并已同步 refactor27 版本文案。本次容器侧使用 `clang-cl --target=i686-pc-windows-msvc` 对 22 个单元逐一严格编译，再用 `lld-link /machine:x86 /dll /nodefaultlib /entry:DllMain@12` 做结构验证，结果 PASS。

部署内容仍只有：

```text
Castle_PadSupport.asi
Castle_PadSupport.ini
```

SDL3.dll 由既有部署环境提供；refactor27 没有增加第三方运行库。

## 1. Windows 正式构建

在 `源码\` 运行 `build.bat`。脚本：

- 使用 x86 / Win32；
- 20 个 `.c` 独立编译；
- `/O2 /GS- /Zl /W4 /WX /utf-8 /TC`；
- `/dll /nodefaultlib /machine:x86 /entry:DllMain@12`；
- 任一编译失败立即停止；
- 链接后删除 `.lib/.exp`，`编译内容\` 最终只保留 ASI + INI。

`build.bat` 是 UTF-8 BOM + CRLF，并先执行 `chcp 65001 >nul`。

## 2. refactor26d 历史交叉构建记录

容器侧使用 clang-cl/lld-link 以 i686 MSVC ABI 验证：20/20 编译 PASS，链接 PASS，输出 PE32/i386。

ASI SHA-256：`25158c7bb6524d72e9ec77084393693d518a32325b2d1b03a49bbdf4b48f9b94`，大小 128,512 bytes。

这不冒充用户 Windows 本机 MSVC 实编；Windows `build.bat` 仍是正式复现路线。

## 3. 部署

把 `Castle_PadSupport.asi`、`Castle_PadSupport.ini` 放入现有 ASI Loader 使用位置，并提供兼容 x86 `SDL3.dll`。本包不夹带 SDL3。

## 4. refactor27 目标 EXE 支持策略

当前接受两个协议兼容的原版 SHA：

- `b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf`：历史精确目标；
- `8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f`：`RPG.exe.org` 完全未修改原版。

二者现在都属于支持白名单并判 PASS，但仍必须逐 capability 校验 CALL 与机器码；未知 SHA 或协议失配继续 fail-closed。

## 5. refactor27 静态检查

```text
python 工具/refactor_check.py
python 工具/refactor_check.py --exe <RPG.exe>
```

本版最终对 `RPG.exe.org`：**80 PASS / 0 FAIL**。无 EXE 模式只用于源码/包结构预检，不能替代目标 EXE 协议检查。

## refactor26c 容器严格构建结果

20 个 C 使用 clang-cl 17、`--target=i686-pc-windows-msvc`、`/O2 /GS- /Zl /W4 /WX /utf-8 /TC` 逐个编译通过；lld-link `/dll /nodefaultlib /machine:x86 /entry:DllMain@12` 链接通过，产物为 PE32/i386。正式用户可复现路线仍是本包 `源码/build.bat` 的 Windows MSVC x86。

