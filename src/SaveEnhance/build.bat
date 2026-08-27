```bat
@echo off
chcp 65001 >nul
setlocal EnableExtensions DisableDelayedExpansion

cd /d "%~dp0"

rem ============================================================  
rem 目录  
rem ============================================================  

set "ROOT=%~dp0"
set "SRC=%ROOT%source"
set "OUT=%ROOT%..\..\build"
set "OBJ=%ROOT%_build"
set "TOOLS_CACHE=%ROOT%tools\__pycache__"
set "TEMPLATE_INI=%ROOT%templete\Castle_SaveEnhance.ini"

set "POWERSHELL_EXE=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"

rem ============================================================  
rem 自动定位 Visual Studio / MSVC x86 工具链  
rem ============================================================  

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="

if exist "%VSWHERE%" (
    "%VSWHERE%" -latest -products "*" ^
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
        -property installationPath ^
        > "%TEMP%\castle_saveenhance_vswhere.txt" 2>nul

    set /p "VSINSTALL=" < "%TEMP%\castle_saveenhance_vswhere.txt"
    del /q "%TEMP%\castle_saveenhance_vswhere.txt" >nul 2>nul
)

if defined VSINSTALL (
    if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" (
        call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 -no_logo
        if errorlevel 1 (
            echo [错误] Visual Studio 开发环境初始化失败。
            goto :fail
        )
    )
)

if not defined VSINSTALL (
    echo [提示] 未通过 vswhere 定位到 Visual Studio，将尝试使用当前 PATH 中的工具链。  
)

rem ============================================================  
rem 检查构建工具
rem ============================================================  

where cl.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 cl.exe。  
    echo [说明] 请安装 Visual Studio C++ x86/x64 编译工具，  
    echo        或从“x86 Native Tools Command Prompt for VS”运行本脚本。  
    goto :fail
)

where link.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 link.exe。
    goto :fail
)

where dumpbin.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 dumpbin.exe。
    goto :fail
)

if not exist "%POWERSHELL_EXE%" (
    echo [错误] 找不到 Windows PowerShell：
    echo        %POWERSHELL_EXE%
    goto :fail
)

if not exist "%TEMPLATE_INI%" (
    echo [错误] 找不到：
    echo        templete\Castle_SaveEnhance.ini
    goto :fail
)

if not exist "%SRC%\Castle_SaveEnhance.cpp" (
    echo [错误] 找不到：
    echo        source\Castle_SaveEnhance.cpp
    goto :fail
)

rem ============================================================
rem 准备构建目录
rem ============================================================

if not exist "%OUT%" mkdir "%OUT%"
if errorlevel 1 (
    echo [错误] 无法创建输出目录：
    echo        %OUT%
    goto :fail
)

if exist "%OBJ%" rmdir /s /q "%OBJ%"
if exist "%TOOLS_CACHE%" rmdir /s /q "%TOOLS_CACHE%"

mkdir "%OBJ%"
if errorlevel 1 (
    echo [错误] 无法创建临时编译目录：
    echo        %OBJ%
    goto :fail
)

del /q "%OUT%\Castle_SaveEnhance.asi" 2>nul
del /q "%OUT%\Castle_SaveEnhance.ini" 2>nul

rem ============================================================
rem 编译
rem ============================================================

echo [1/4] 编译 Castle_SaveEnhance.cpp...

cl.exe /nologo /c /TP /std:c++17 /utf-8 ^
    /O2 /Oi- ^
    /W4 /WX ^
    /GR- ^
    /GS- ^
    /Gs999999999 ^
    /Zl ^
    /Fo"%OBJ%\Castle_SaveEnhance.obj" ^
    "%SRC%\Castle_SaveEnhance.cpp"

if errorlevel 1 goto :fail

rem ============================================================
rem 链接
rem ============================================================

echo [2/4] 链接 PE32 Castle_SaveEnhance.asi...

link.exe /nologo ^
    /Brepro ^
    /DLL ^
    /NODEFAULTLIB ^
    /ENTRY:DllMain ^
    /SUBSYSTEM:WINDOWS ^
    /MACHINE:X86 ^
    /IMPLIB:"%OBJ%\Castle_SaveEnhance.lib" ^
    /OUT:"%OUT%\Castle_SaveEnhance.asi" ^
    "%OBJ%\Castle_SaveEnhance.obj" ^
    kernel32.lib

if errorlevel 1 goto :fail

rem ============================================================
rem PE 基础验证
rem ============================================================

echo [3/4] 验证 PE32、DLL、入口点与 InitializeASI 导出...

"%POWERSHELL_EXE%" -NoProfile -ExecutionPolicy Bypass -Command ^
    "$p='%OUT%\Castle_SaveEnhance.asi';" ^
    "$b=[IO.File]::ReadAllBytes($p);" ^
    "if($b.Length -lt 64){exit 10};" ^
    "$pe=[BitConverter]::ToInt32($b,0x3C);" ^
    "if($pe -lt 0 -or ($pe+44) -gt $b.Length){exit 11};" ^
    "if([Text.Encoding]::ASCII.GetString($b,$pe,4) -ne ('PE'+[char]0+[char]0)){exit 12};" ^
    "$m=[BitConverter]::ToUInt16($b,$pe+4);" ^
    "$c=[BitConverter]::ToUInt16($b,$pe+22);" ^
    "$ep=[BitConverter]::ToUInt32($b,$pe+40);" ^
    "if($m -ne 0x014C){exit 13};" ^
    "if(($c -band 0x2000) -eq 0){exit 14};" ^
    "if($ep -eq 0){exit 15};" ^
    "exit 0"

if errorlevel 1 (
    echo [错误] PE 基础验证失败。
    goto :fail
)

dumpbin.exe /nologo /exports "%OUT%\Castle_SaveEnhance.asi" ^
    > "%OBJ%\Castle_SaveEnhance.exports.txt"

if errorlevel 1 (
    echo [错误] dumpbin 无法读取最终 ASI。
    goto :fail
)

findstr /R /C:"[ ][ ]*InitializeASI$" "%OBJ%\Castle_SaveEnhance.exports.txt" >nul
if errorlevel 1 (
    echo [错误] 最终 ASI 没有导出 InitializeASI。
    echo.
    echo [dumpbin 导出表]
    type "%OBJ%\Castle_SaveEnhance.exports.txt"
    goto :fail
)

rem ============================================================
rem 复制配置
rem ============================================================

echo [4/4] 复制 Castle_SaveEnhance.ini...

copy /y "%TEMPLATE_INI%" "%OUT%\Castle_SaveEnhance.ini" >nul
if errorlevel 1 (
    echo [错误] 复制 INI 失败。
    goto :fail
)

rem ============================================================
rem 清理
rem ============================================================

if exist "%OBJ%" rmdir /s /q "%OBJ%"
if exist "%TOOLS_CACHE%" rmdir /s /q "%TOOLS_CACHE%"

echo.
echo [成功] 已生成：
echo   build\Castle_SaveEnhance.asi
echo   build\Castle_SaveEnhance.ini
echo.
echo [验证] PE32 / i386
echo [验证] DLL 标志存在
echo [验证] PE 入口点非零
echo [验证] InitializeASI 已导出
echo.
echo [成功] 临时编译目录与 tools\__pycache__ 已清理。
pause
exit /b 0


:fail

del /q "%OUT%\Castle_SaveEnhance.asi" 2>nul
del /q "%OUT%\Castle_SaveEnhance.ini" 2>nul

if exist "%OBJ%" rmdir /s /q "%OBJ%"
if exist "%TOOLS_CACHE%" rmdir /s /q "%TOOLS_CACHE%"

echo.
echo [失败] Castle_SaveEnhance 构建中止。
echo [说明] 请检查上方第一条编译、链接或验证错误。
pause
exit /b 1
```
