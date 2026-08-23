# 《幽城幻剑录》功能拆分 ASI v0.3.2 构建说明

## 1. Windows / MSVC 正式构建

运行：

```text
源码\build.bat
```

脚本开头固定：

```bat
chcp 65001 >nul
```

BAT 使用 UTF-8 + CRLF。

构建目标固定 x86，因为 `RPG.exe` 是 32 位程序。

关键参数：

```text
/std:c++17 /utf-8 /O2 /W4 /WX /GR- /GS- /Zl /LD
/NODEFAULTLIB /ENTRY:DllMain /SUBSYSTEM:WINDOWS /MACHINE:X86
```

输出到 `插件` 目录。

输出后脚本会直接读取 PE 文件头并强制验证三项：

- `Machine == 0x014C`（x86/i386）；
- PE Characteristics 包含 DLL 标志；
- `AddressOfEntryPoint != 0`。

第三项是 v0.3.1a 新增、v0.3.2 继续强制执行的硬门。先前 v0.3.1 最终容器封包误用了零入口产物，文件仍是 PE32 DLL，但 Windows 不会调用 `DllMain`。以后任何一个 ASI 入口为 0，构建都必须直接失败，不能进入交付包。

## 2. 无 CRT 原则

正式 ASI 不链接 C/C++ 运行库，只依赖 `KERNEL32.dll`。

`AnytimeSave.cpp` 仍提供非常小的逐字节 `memcpy/memset` 兜底。v0.3.2 的 `BUGFix.cpp` 因 Crash 双路径 stub 使用了小数组复制/清零，也提供自己的 `volatile` 逐字节 `memcpy/memset` 最小实现，避免 /O2 优化器生成 CRT 外部符号。

v0.3.1a 已删除 TSA 路径、文件重开、CRC、ReadFile/GetFileSize/GetLastError/DeleteFileW 等正式存档 I/O 链；`AnytimeSave.asi` 现在仅为日志使用 `CreateFileW/WriteFile/CloseHandle`，其余 Kernel32 API 用于内存校验和 Hook。

## 3. 本轮第二套交叉验证

当前容器无 Visual Studio / MinGW-w64 GCC，本轮使用：

```text
clang-cl --target=i686-pc-windows-msvc
lld-link
```

以 `/W4 /WX /O2 /GS- /Zl /NODEFAULTLIB` 复核语法、32 位 ABI 和最终导入表。

四个正式 ASI + `AnytimeSaveProbe.asi` 均生成 PE32/i386，导入 DLL 只有 `KERNEL32.dll`。

**这仍不等价于 MinGW-w64 GCC PASS。** 后续环境具备 MinGW-w64 时，应继续补做项目长期要求的 MinGW x86 验证。

## 4. 本轮静态策略结果

`证据/安全回退存档静态策略测试.json`：**24/24 PASS**。

## 5. v0.3.2 BUGFix 合并后的额外构建注意

BUGFix 现在会额外使用：

```text
VirtualAlloc
VirtualFree
VirtualQuery
GetLastError
```

仍全部来自 `KERNEL32.dll`，没有新增随包 DLL。

正式链接仍必须显式 `/ENTRY:DllMain`；Crash Hook helper 使用 `__stdcall`，构建后应确认 Route A/B helper 以 `ret 4` 返回。
