@echo off
chcp 65001 >nul
setlocal EnableExtensions

rem ============================================================================ 
rem build_probe.bat - AnytimeSaveProbe v0.3.1a-probe1 
rem ---------------------------------------------------------------------------- 
rem 构建只读诊断插件 AnytimeSaveProbe.asi。 
rem Probe 与正式 AnytimeSave 使用同一个菜单 Hook 点，所以测试时绝不能同时加载。 
rem
rem v0.3.1a 增加 PE 入口点硬检查，避免再次产出 AddressOfEntryPoint=0 的空壳 DLL。 
rem ============================================================================ 

set "SCRIPT_DIR=%~dp0"
set "SRC_DIR=%SCRIPT_DIR%src"
rem 输出统一到仓库根 build\（向上 4 级：AnytimeSaveProbe→tools→AnytimeSave→src→仓库根）
set "OUT_DIR=%SCRIPT_DIR%..\..\..\..\build"
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%" 2>nul
rem 中间对象目录（不散落到运行目录）
set "OBJ_DIR=%SCRIPT_DIR%_build"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%" 2>nul

where cl.exe >nul 2>nul
if not errorlevel 1 goto :have_cl

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [错误] 找不到 cl.exe，也找不到 vswhere.exe。
    echo [解决] 请安装带“使用 C++ 的桌面开发”组件的 Visual Studio。
    goto :fail
)
for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSROOT=%%I"
if not defined VSROOT (
    echo [错误] 没有找到可用的 MSVC x86/x64 工具链。
    goto :fail
)
call "%VSROOT%\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 goto :fail

:have_cl
cl.exe /nologo /std:c++17 /utf-8 /O2 /W4 /WX /GR- /GS- /Zl /LD ^
    "%SRC_DIR%\AnytimeSaveProbe.cpp" /Fo"%OBJ_DIR%\AnytimeSaveProbe.obj" ^
    /link /NOLOGO /NODEFAULTLIB /ENTRY:DllMain /SUBSYSTEM:WINDOWS /MACHINE:X86 ^
    kernel32.lib /OUT:"%OUT_DIR%\AnytimeSaveProbe.asi"
if errorlevel 1 goto :fail

call :check_pe "%OUT_DIR%\AnytimeSaveProbe.asi"
if errorlevel 1 goto :fail

echo.
echo [成功] AnytimeSaveProbe.asi 已生成，并通过 x86 / DLL / 非零入口点检查。
echo 注意：不要与正式 AnytimeSave.asi 同时加载。
exit /b 0

:check_pe
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command ^
  "$p='%~1'; $b=[IO.File]::ReadAllBytes($p); if($b.Length -lt 64){exit 10}; $pe=[BitConverter]::ToInt32($b,0x3C); if($pe -lt 0 -or ($pe+44) -gt $b.Length){exit 11}; $m=[BitConverter]::ToUInt16($b,$pe+4); $c=[BitConverter]::ToUInt16($b,$pe+22); $ep=[BitConverter]::ToUInt32($b,$pe+40); if($m -ne 0x014C){Write-Host '[错误] 不是 x86 PE：' $p; exit 12}; if(($c -band 0x2000) -eq 0){Write-Host '[错误] PE 没有 DLL 标志：' $p; exit 13}; if($ep -eq 0){Write-Host '[错误] AddressOfEntryPoint=0，DllMain 不会执行：' $p; exit 14}; Write-Host ('[入口检查] PASS  RVA=0x{0:X8}  {1}' -f $ep,$p)"
exit /b %errorlevel%

:fail
rmdir /s /q "%OBJ_DIR%" 2>nul
echo.
echo [失败] 构建没有完成；不要把失败产物当成可用插件。
exit /b 1
