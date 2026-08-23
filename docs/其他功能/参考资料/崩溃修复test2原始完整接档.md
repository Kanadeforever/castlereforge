# 崩溃修复 test2 原始完整接档（历史证据）

> 来源：用户提供的《幽城幻剑录_CrashFix_v0.1.0-test2_双调用路径修复候选》包。
> 原文生成时仍写“候选、待实机”；在 v0.3.2 合并前，用户已明确反馈该版本运行稳定，因此当前项目把 test2 核心逻辑视为实机通过基线并并入 BUGFix。
> 本文件仅保存原始研究上下文；当前部署不再需要独立 CrashFix.asi/CrashFix.ini。

---

# 《幽城幻剑录》CrashFix 截至 v0.1.0-test2 的完整接档

> 本文件是独立接档文档。只拿到本包，也应该能够继续 CrashFix 开发，不依赖聊天历史。

## 一、项目定位

`CrashFix.asi` 是《幽城幻剑录》统一原版崩溃修复插件。

长期规则：

- 后续确认的原版崩溃修复优先继续加入 CrashFix；
- 每个崩溃一个独立 `fix_*.c` 模块；
- 每个 Fix 有独立 INI 开关；
- 不修改磁盘 `RPG.exe`；
- 不依赖 ModLoader 内部 ABI，只要求 ASI 能被加载；
- 未知 EXE / 机器码冲突一律 fail-closed；
- 代码文件名英文；源码内部必须有面向初学者的详细简体中文行内注释；
- 所有非代码文档使用简体中文文件名；
- 每个源码/编译包必须带截至该版本的完整接档和工具说明。

当前只有 Fix01。

## 二、目标程序基线

当前唯一支持的 `RPG.exe`：

```text
SHA-256 8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f
MD5      4DA680DDBC40D36F1F4AD438AB71110A
PE       PE32 / i386
ImageBase 0x00400000
SizeOfImage 0x00585000
```

如果以后出现其它 EXE，必须新增独立签名/地址表；不能放宽当前地址假设后继续硬补。

## 三、Fix01 问题定义

用户可稳定复现：

```text
进入游戏
→ 读档
→ 返回标题
→ 新游戏
→ 进入 NewGame / MP0000 生命周期
→ 崩溃
```

`game1.log` 已记录：

```text
NEW_GAME_BEGIN
LOAD_READER(NewGame.TSF)
DESERIALIZE_CORE
REBUILD_ROLE_PTRS
...
Access Violation @ 0x004067A0
```

异常核心：

```text
EIP          = 0x004067A0
ECX          = 旧 Legacy Background Controller
fault target = ECX + 0x20
```

机器码：

```asm
004067A0  8B 41 20   mov eax,[ecx+20h]
```

固化40静态研究已经闭合：

```text
0x004067A0 = Legacy Background Camera/Parallax Controller 更新器
controller+0x20 = Map SCI +0x65 派生的 background camera/parallax mode
```

因此 Fix01 的调查方向是 Map runtime 到 Legacy Background Controller 的生命周期引用，而不是视频解码器本身。

## 四、相关 Map runtime 状态

当前逆向与 game1 现场共同支持：

```text
runtime+0x04 = Legacy Background Controller*
runtime+0x35 = background ownership/enable 相关标志
```

参考边界：

```text
0x00408E80  Map runtime 构造/装载相关入口
0x00408D80  Map runtime 清理相关入口
```

这两个地址当前只作为身份/生命周期参考，没有被 test2 修改。

## 五、v0.1.0-test1 方案与实机失败

### test1 方案

test1 认为 `0x004067A0` 的已知崩溃 caller 是：

```asm
0040953A  mov al,[esi+35h]
0040953D  test al,al
0040953F  je  0040954D
00409541  mov ecx,[esi+04h]
00409544  push 0
00409546  push 0
00409548  call 004067A0
```

所以只接管 `0x00409541` 的 12 字节逻辑块，在调用前 VirtualQuery `runtime+0x04` controller。

### 用户实机结果

CrashFix.log：

```text
[启动] CrashFix v0.1.0-test1 已进入进程
[目标] 当前 RPG.exe 已识别
[Fix01安装] 已接管 0x00409541
[结果] CrashFix 初始化完成
```

之后用户严格复现，**仍然崩溃**。

关键点：整个日志没有任何：

```text
[Fix01命中]
```

因此 test1 不是“命中了但清理不够”，而是已知补丁路径根本没有经过。

### test1 正式结论

**FAIL。**

单调用点假设不成立。

## 六、test1 失败后的新静态发现

重新扫描当前 RPG.exe 对 `0x004067A0` 的相对 CALL，确认同一区域至少存在两条调用路径。

### Route A

```asm
0040953A  mov al,[esi+35h]
0040953D  test al,al
0040953F  je  0040954D
00409541  mov ecx,[esi+04h]
00409544  push 0
00409546  push 0
00409548  call 004067A0
0040954D  ...
```

Route A patch block：

```text
VA     0x00409541
RVA    0x00009541
长度   12
字节   8B 4E 04 6A 00 6A 00 E8 53 D2 FF FF
resume 0x0040954D
```

### Route B

```asm
00409587  mov ecx,[esi+04h]
0040958A  test ecx,ecx
0040958C  je  00409597
0040958E  push 0
00409590  push 0
00409592  call 004067A0
00409597  ...
```

Route B patch block：

```text
VA     0x00409587
RVA    0x00009587
长度   16
字节   8B 4E 04 85 C9 74 09 6A 00 6A 00 E8 09 D2 FF FF
resume 0x00409597
```

这能完整解释 test1：如果 game1 走 Route B，则 `0x00409541` 从未执行，自然没有任何 Fix01 命中。

## 七、v0.1.0-test2 方案

### 1. 全部签名先核对，再安装

安装 Fix01 前同时核对：

```text
0x004067A0 updater
0x00409541 Route A
0x00409587 Route B
```

只要任何一处不匹配：

```text
fail-closed
不修改任何一条 Route
```

避免半安装。

### 2. 两条路径都执行前置校验

动态 stub：

```text
push esi
call route-specific helper
→ EAX = NULL 或合法 controller

test eax,eax
jz skip

mov ecx,eax
push 0
push 0
call 0x004067A0

skip:
jmp route resume
```

### 3. controller 有效条件

```text
非 NULL
4 字节对齐
至少 0x30 字节全部 MEM_COMMIT
页面可读
非 PAGE_GUARD
```

有效时完全调用原版 updater。

### 4. Route A 语义

Route A 进入 patch block 前原版已经判断：

```text
runtime+0x35 != 0
```

所以：

- controller NULL：状态矛盾；
- controller 非 NULL 但不可读：stale pointer。

都尝试：

```text
runtime+0x04 = NULL
runtime+0x35 = 0
```

然后跳过本次 updater。

### 5. Route B 语义

原版自己有：

```asm
test ecx,ecx
je 00409597
```

所以 Route B 的 NULL controller 是正常分支：

```text
NULL → 安静跳过，不算 CrashFix 命中
```

非 NULL 但不可读才算 stale，清 +0x04/+0x35。

### 6. 日志增强

每条路径第一次经过 helper 时只记录一次：

```text
[Fix01路径] Route A / 0x00409548 ...
[Fix01路径] Route B / 0x00409592 ...
```

真正修复：

```text
[Fix01/A命中]
[Fix01/B命中]
[Fix01处理]
```

这样以后不会再遇到“安装成功但不知道游戏是否经过这条路径”的证据缺口。

### 7. 双路径安装失败回滚

顺序：

```text
构建 A stub
构建 B stub
安装 A
安装 B
```

如果 B 安装失败：

```text
立即恢复 A 原始字节
释放两个 stub
Fix01 返回失败
```

不会留下只补一半的状态。

## 八、不采用的方案

### 1. VEH 在 0x004067A0 吞 Access Violation

不采用。原因：

- `0x004067A0` 是公共 updater；
- 会把其它未知 caller 的内存损坏也伪装成 Fix01；
- 会改变异常控制流而不是修引用；
- 当前已经有更窄的两个 Map runtime caller 可以处理。

### 2. 直接把 0x004067A0 改成“ECX 无效就 ret”

当前不采用。test2 只扩大到静态确认遗漏的第二个 caller，不直接泛化整个公共函数。

### 3. 修改 NewGame.TSF

不采用。现有证据没有说明 NewGame.TSF 文件损坏；崩溃现场是 runtime controller 引用。

### 4. 把 game1 当成 Bink 视频崩溃

不采用。视觉上可能发生在视频阶段，但已捕获的 CPU 异常位于 RPG.exe `0x004067A0` Map background updater。

## 九、Game2 旁支状态

另一个独立问题：

```text
启动后很快按 ESC 跳开场动画
→ 假死
```

已有 game2 日志：

- 没有严重异常；
- `binkw32.dll` 正常打开/读取/关闭 `Mov\1.DAT`；
- 影片句柄结束后，游戏状态没有推进到当前审计器可见的下一生命周期；
- 更像 Movie/ESC 完成状态机停滞。

**v0.1.0-test2 没有处理 game2。**

如果以后确认需要修复，应作为 CrashFix 的独立 Fix02 模块，不能混进 Fix01。

## 十、cnc-ddraw 状态

用户已经决定放弃当前 cnc-ddraw 旁支，不纳入 CrashFix，也不继续在本包研究。

## 十一、配置

```ini
[General]
Enable=1
Log=1

[Fixes]
NewGameBackgroundControllerCrash=1
```

未来新增 Fix：

```ini
[Fixes]
NewGameBackgroundControllerCrash=1
FutureCrash02=1
FutureCrash03=1
```

## 十二、文件布局

```text
CrashFix
├─ build.bat
├─ build_mingw.bat
├─ 配置模板\CrashFix.ini
├─ 源码\*.c / *.h
├─ 工具\crashfix_check.py
├─ 编译内容\mods\asi\CrashFix.asi
├─ 编译内容\mods\asi\CrashFix.ini
├─ 文档\截至v0.1.0-test2的完整接档.md
├─ 文档\使用说明.md
├─ 文档\架构与修复说明.md
├─ 文档\构建说明.md
├─ 文档\工具详细信息.md
├─ 文档\文件哈希清单.md
└─ 证据\...
```

## 十三、构建与静态验证

v0.1.0-test2 当前候选：

```text
6 / 6 C 单元 x86 clang-cl /W4 /WX PASS
0 warning
6 / 6 Clang Static Analyzer PASS
0 diagnostic
CrashFix.asi = PE32 / i386
静态 DLL 依赖 = KERNEL32.dll only
crashfix_check.py = 54 PASS / 0 FAIL；预封包 ZIP 已独立解压并从解压副本重跑，同样 54 PASS / 0 FAIL
```

最终 ZIP 必须独立解压后再跑同一检查器。

## 十四、当前阻塞项

只剩 Windows 实机重新复现 game1：

```text
读档 → 返回标题 → 新游戏
```

必须拿：

```text
mods\asi\CrashFix.log
```

最好同时拿：

```text
mods\game.log
```

## 十五、下一步判定树

### A. test2 成功、不再崩溃

检查日志是：

```text
Fix01/A命中
```

还是：

```text
Fix01/B命中
```

然后把 Fix01 升为实机 PASS 基线。

### B. 仍崩溃，但出现 A/B 命中

说明 stale pointer 被清理后又推进到下一处生命周期错误。必须分析新的异常地址，不能继续扩大同一补丁。

### C. 仍崩溃，A/B 都只有“路径观察”但没有命中

说明 VirtualQuery 语义不足以判断对象有效性，可能是“内存页仍可读但对象已经逻辑释放”。下一步应转向 0x00408E80 / 0x00408D80 构造/析构生命周期跟踪，而不是继续靠页面可读性。

### D. 仍崩溃，A/B 两条路径都完全没有观察

说明还有第三个 caller / 间接调用 / 不同异常链。重新以新 game.log/EIP 做证据，不再猜。

---

**当前正式结论：test1 FAIL；test2 为双调用路径修复候选，尚待用户实机。**
