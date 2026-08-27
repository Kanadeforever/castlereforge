@echo off
setlocal EnableExtensions DisableDelayedExpansion

rem 使用系统自带 chcp.com 的完整路径，避免 PATH 被启动器精简后找不到 chcp。
"%SystemRoot%\System32\chcp.com" 65001 >nul 2>nul

rem 无论用户从哪个目录双击或调用本脚本，都先切换到 build.bat 自己所在的目录。
cd /d "%~dp0"
set "SCRIPT_DIR=%~dp0"
set "OUT_DIR=%SCRIPT_DIR%..\..\build"
set "OBJ_DIR=%SCRIPT_DIR%_build"

rem 下面的版本与路径来自 C:\Project\编译器地址记录.txt。
rem 记录文件给出的是 Hostx64\x64；本插件必须生成 32 位 ASI，所以把最后一级改为 x86。
set "VC_TOOLS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.51.36231"
set "CL_EXE=%VC_TOOLS%\bin\Hostx64\x86\cl.exe"
set "LINK_EXE=%VC_TOOLS%\bin\Hostx64\x86\link.exe"
set "DUMPBIN_EXE=%VC_TOOLS%\bin\Hostx64\x86\dumpbin.exe"

rem 当前 Windows 10/11 SDK 的 x86 kernel32.lib。源码自带 Win32Mini.h，不需要 SDK 头文件，
rem 但链接时仍需要这份导入库告诉链接器 KERNEL32.dll 中各函数的正式名称。
set "KERNEL32_LIB=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86\kernel32.lib"
set "POWERSHELL_EXE=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"

rem 在真正编译前逐项检查固定路径。这样路径过期时会直接指出缺少哪一项，而不是产生难懂的
rem “不是内部或外部命令”或“无法打开 kernel32.lib”。
if not exist "%CL_EXE%" goto :missing_cl
if not exist "%LINK_EXE%" goto :missing_link
if not exist "%DUMPBIN_EXE%" goto :missing_dumpbin
if not exist "%KERNEL32_LIB%" goto :missing_kernel32
if not exist "%POWERSHELL_EXE%" goto :missing_powershell
goto :tools_ready

:missing_cl
echo [错误] 找不到 x86 cl.exe：%CL_EXE%
echo [说明] 请依据 C:\Project\编译器地址记录.txt 更新本脚本中的 VC_TOOLS。
goto :fail

:missing_link
echo [错误] 找不到 x86 link.exe：%LINK_EXE%
goto :fail

:missing_dumpbin
echo [错误] 找不到 x86 dumpbin.exe：%DUMPBIN_EXE%
goto :fail

:missing_kernel32
echo [错误] 找不到 x86 kernel32.lib：%KERNEL32_LIB%
echo [说明] Windows SDK 升级后，请把 KERNEL32_LIB 改成新版本的 um\x86\kernel32.lib。
goto :fail

:missing_powershell
echo [错误] 找不到 Windows PowerShell，无法执行 PE 基础验证。
goto :fail

:tools_ready

rem 和仓库内其它插件保持一致：最终 ASI 进入仓库根 build，中间文件只进入本目录的 _build。
rem 只删除属于 SaveEnhance 的文件，绝不清空根 build 中其它插件已经生成的内容。
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
if exist "%OBJ_DIR%" rmdir /s /q "%OBJ_DIR%"
mkdir "%OBJ_DIR%"
del /q "%OUT_DIR%\Castle_SaveEnhance.asi" 2>nul

echo [1/3] 编译 Castle_SaveEnhance.cpp
rem /W4 /WX 把四级警告全部当错误；/Zl 与链接阶段的 /NODEFAULTLIB 一起保证不依赖 CRT。
rem /Oi- 禁止把普通函数强行改成编译器内部函数，/Gs 大值避免自动插入栈探测依赖。
"%CL_EXE%" /nologo /c /std:c++17 /utf-8 /O2 /Oi- /W4 /WX /GR- /GS- /Gs999999999 /Zl ^
  "source\Castle_SaveEnhance.cpp" /Fo:"%OBJ_DIR%\Castle_SaveEnhance.obj"
if errorlevel 1 goto :fail

echo [2/3] 链接 PE32 Castle_SaveEnhance.asi
rem /Brepro 去掉每次构建变化的时间戳，使同一源码与工具链可以得到稳定 SHA-256。
rem 导入库和导出辅助文件放到 _build，仓库根 build 因此只增加玩家真正需要的 ASI。
"%LINK_EXE%" /nologo /Brepro /DLL /NODEFAULTLIB /ENTRY:DllMain /SUBSYSTEM:WINDOWS /MACHINE:X86 ^
  /IMPLIB:"%OBJ_DIR%\Castle_SaveEnhance.lib" /OUT:"%OUT_DIR%\Castle_SaveEnhance.asi" ^
  "%OBJ_DIR%\Castle_SaveEnhance.obj" "%KERNEL32_LIB%"
if errorlevel 1 goto :fail

echo [3/3] 验证 PE32、DLL、入口点与 InitializeASI 导出
rem 先直接读取 PE 文件头：0x014C 是 i386，0x2000 是 DLL 标志，入口 RVA 不能是 0。
"%POWERSHELL_EXE%" -NoProfile -ExecutionPolicy Bypass -Command ^
  "$p='%OUT_DIR%\Castle_SaveEnhance.asi'; $b=[IO.File]::ReadAllBytes($p); if($b.Length -lt 64){exit 10}; $pe=[BitConverter]::ToInt32($b,0x3C); if($pe -lt 0 -or ($pe+44) -gt $b.Length){exit 11}; if([Text.Encoding]::ASCII.GetString($b,$pe,4) -ne ('PE'+[char]0+[char]0)){exit 12}; $m=[BitConverter]::ToUInt16($b,$pe+4); $c=[BitConverter]::ToUInt16($b,$pe+22); $ep=[BitConverter]::ToUInt32($b,$pe+40); if($m -ne 0x014C -or ($c -band 0x2000) -eq 0 -or $ep -eq 0){exit 13}"
if errorlevel 1 (
    echo [错误] PE 基础验证失败；不要使用本次输出。
    goto :fail
)

rem dumpbin 的结果先写入中间文本，再由批处理逐行读取第 4 列。
rem 导出表中的标准行格式是“序号 提示 RVA 名称”，因此第 4 列必须精确等于 InitializeASI。
"%DUMPBIN_EXE%" /nologo /exports "%OUT_DIR%\Castle_SaveEnhance.asi" > "%OBJ_DIR%\Castle_SaveEnhance.exports.txt"
if errorlevel 1 (
    echo [错误] dumpbin 无法读取最终 ASI 的导出表。
    goto :fail
)
set "HAS_INITIALIZE_ASI="
for /f "usebackq tokens=4" %%E in ("%OBJ_DIR%\Castle_SaveEnhance.exports.txt") do (
    if "%%E"=="InitializeASI" set "HAS_INITIALIZE_ASI=1"
)
if not defined HAS_INITIALIZE_ASI (
    echo [错误] 最终 ASI 没有导出 InitializeASI；Castle Mod Loader 将不会执行正式初始化。
    goto :fail
)

rmdir /s /q "%OBJ_DIR%"
echo.
echo [成功] 已生成仓库根 build\Castle_SaveEnhance.asi。
echo [成功] 已确认 PE32、DLL、非零入口点及 InitializeASI 导出。
pause
exit /b 0

:fail
rem 失败时删除可能只写了一半的最终 ASI和全部临时文件，避免用户误用坏产物。
del /q "%OUT_DIR%\Castle_SaveEnhance.asi" 2>nul
if exist "%OBJ_DIR%" rmdir /s /q "%OBJ_DIR%"
echo.
echo [失败] Castle_SaveEnhance 构建中止，请从上方第一条错误开始检查。
pause
exit /b 1
