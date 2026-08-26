# 构建配置与工具说明

> 当前候选：v0.11-poc11

## 1. 部署

从仓库根 `build/` 复制：

```text
Castle_Widescreen.asi
Castle_Widescreen.ini
```

两者放在 ASI Loader 实际加载目录。

## 2. Windows 一键构建

运行：

```text
build.bat
```

脚本会：

1. 切 UTF-8 代码页；
2. 找 Visual Studio C++ x86 工具链；
3. 使用 `vcvars32.bat`；
4. `/W4 /WX` 编译三个 C 文件；
5. `/nodefaultlib /machine:x86` 链接；
6. 生成 `build\Castle_Widescreen.asi`；
7. 同步同名 INI；
8. 清理中间 OBJ/LIB。

## 3. 本轮交叉构建

当前环境使用：

```text
clang-cl --target=i686-pc-windows-msvc
lld-link /machine:x86
```

最终 ASI 必须是：

- PE32；
- Intel i386；
- DLL；
- 无静态 Import Directory。

## 4. 配置说明

配置文件为 `Castle_Widescreen.ini`，必须与 `Castle_Widescreen.asi` 放在同一目录。

### 4.1 默认配置

```ini
[Display]
Ultrawide=0

[Cinematic]
BlurredSides=1

[Transition]
EnterDurationMs=250
ExitDurationMs=250
```

### 4.2 Display.Ultrawide

```text
0 = 854×480，16:9，默认
1 = 1120×480，严格21:9
```

其它值回退到 `0`。

### 4.3 Cinematic.BlurredSides

这是 v0.11 新增的开关。

```text
1 = 强电影式模糊侧区，默认
0 = 纯黑侧区
```

它只影响当前“消息 / Battle 侧面板”的像素内容。

不会改变：

- 进入或退出的时机；
- 左右推入/退出动画；
- 中央 640×480；
- 对话框、立绘、Battle HUD 的位置；
- 16:9 / 21:9 输出宽度；
- 过渡时间。

### 4.4 模糊模式

```ini
[Cinematic]
BlurredSides=1
```

侧面板使用已经在 v0.10 验收通过的：

- 当前中央画面缩图；
- 两遍 7×7 box blur；
- 双线性放大；
- 亮度保留 160/256。

### 4.5 黑边模式

```ini
[Cinematic]
BlurredSides=0
```

侧面板仍然按完全相同的空间动画推入和退出，但最终像素为纯黑 `RGB565 0x0000`。

纯黑模式还会跳过模糊图生成，所以计算成本更低。

### 4.6 Transition.EnterDurationMs

控制侧面板 / 硬 4:3 黑边进入时间，单位毫秒。

合法范围：

```text
0..10000
```

默认：

```text
250
```

### 4.7 Transition.ExitDurationMs

控制侧面板退出并重新露出宽屏世界的时间，单位毫秒。

合法范围同样是 `0..10000`，默认 `250`。

### 4.8 配置读取规则

插件启动时只读取一次 INI。

缺文件、缺键、无法解析或越界时回退默认值。

日志会明确输出：

```text
[配置] Ultrawide=...
[配置] OutputWidth=...
[配置] SideWidth=...
[配置] BlurredSides=...
[配置] EnterDurationMs=...
[配置] ExitDurationMs=...
```

修改 INI 后需要重新启动游戏。

## 5. 静态检查工具

### 5.1 当前工具

```text
工具/widescreen_check.py
```

### 5.2 用途

当前检查器面向 v0.11，主要验证：

- 精确 RPG.exe SHA-256；
- PE32 / i386；
- 四个当前 E8 CALL 原目标；
- Display / Camera / Scene / Event / Battle / 消息路径机器码；
- 16:9 与 21:9 两套几何；
- 854-safe Camera；
- v0.7 侧画消息隔离；
- v0.9 全消息统一规则没有回退；
- v0.11 `BlurredSides` 默认值、INI、源码读取；
- 模糊 / 纯黑分支只位于侧面板像素路径；
- 纯黑模式可跳过模糊计算；
- 两种样式继续共享同一推入/退出与柔化函数；
- 毫秒过渡；
- 事务式 Hook 回滚；
- 旧 POC1 失败路线没有复活；
- ASI 没有静态 Import Directory。

### 5.3 使用

```text
python 工具/widescreen_check.py --root . --exe 证据/RPG.exe
```

返回码：

```text
0 = 全部通过
1 = 至少一项失败
```

当前发布候选必须达到：

```text
65 PASS / 0 FAIL
```

### 5.4 限制

静态检查不能代替：

- 模糊/黑边主观观感；
- 实机动画是否顺滑；
- 特殊剧情 / Battle 场景回归。

## 6. 编码 / 换行

- C/H/BAT/Markdown：UTF-8 BOM + CRLF；
- Python：UTF-8 + LF；
- INI：ASCII + CRLF；
- 源码文件名英文；
- 包内文档文件名简体中文。
