@echo off
chcp 65001 >nul
setlocal

@echo off
cd /d "%~dp0"

where cl.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 cl.exe。请从 x86 Native Tools Command Prompt 运行本脚本。
    pause
    exit /b 1
)
where link.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 link.exe。请确认 Visual Studio C++ 工具已经安装。
    pause
    exit /b 1
)

if not exist build mkdir build
if not exist release mkdir release
del /q build\Castle_SaveEnhance.obj 2>nul
del /q release\Castle_SaveEnhance.asi 2>nul

cl.exe /nologo /c /std:c++17 /utf-8 /O2 /Oi- /W4 /WX /GR- /GS- /Gs999999999 /Zl ^
  source\Castle_SaveEnhance.cpp /Fo:build\Castle_SaveEnhance.obj
if errorlevel 1 exit /b 1

link.exe /nologo /DLL /NODEFAULTLIB /ENTRY:DllMain /SUBSYSTEM:WINDOWS /MACHINE:X86 ^
  /OUT:release\Castle_SaveEnhance.asi build\Castle_SaveEnhance.obj kernel32.lib
if errorlevel 1 exit /b 1

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$p='release\Castle_SaveEnhance.asi';$b=[IO.File]::ReadAllBytes($p);if($b.Length-lt64-or$b[0]-ne0x4D-or$b[1]-ne0x5A){exit 10};$pe=[BitConverter]::ToInt32($b,0x3C);if([Text.Encoding]::ASCII.GetString($b,$pe,4)-ne('PE'+[char]0+[char]0)){exit 11};$m=[BitConverter]::ToUInt16($b,$pe+4);$ch=[BitConverter]::ToUInt16($b,$pe+22);$op=$pe+24;$ep=[BitConverter]::ToUInt32($b,$op+16);if($m-ne0x14C-or($ch-band0x2000)-eq0-or$ep-eq0){exit 12}"
if errorlevel 1 (
    echo [错误] PE 基础验证失败；不要使用本次输出。
    pause
    exit /b 1
)

where dumpbin.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 dumpbin.exe，无法验证 InitializeASI 导出。
    pause
    exit /b 1
)
dumpbin.exe /nologo /exports release\Castle_SaveEnhance.asi | findstr /C:"InitializeASI" >nul
if errorlevel 1 (
    echo [错误] 最终 ASI 没有导出 InitializeASI；Castle Mod Loader 将不会执行正式初始化。
    pause
    exit /b 1
)

echo [成功] 已生成 release\Castle_SaveEnhance.asi，并确认导出 InitializeASI。
pause
exit /b 0
