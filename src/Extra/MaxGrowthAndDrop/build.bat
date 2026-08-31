@echo off
chcp 65001 >nul
setlocal EnableExtensions

rem 本脚本只构建 MaxGrowthAndDrop，不依赖 src\Extra 根目录中的其它构建脚本。
set "SCRIPT_DIR=%~dp0"
set "SRC_DIR=%SCRIPT_DIR%source"
set "SDK_DIR=%SCRIPT_DIR%..\..\RuntimeSDK"
set "OUT_DIR=%SCRIPT_DIR%..\..\..\build"
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%" 2>nul

rem 所有中间文件只放在当前功能目录的 _build 中，成功或失败后都会清理。
set "OBJ_DIR=%SCRIPT_DIR%_build"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%" 2>nul

rem 通过 vswhere 定位本机最新的 Visual Studio C++ x86 工具链。
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [错误] 找不到 vswhere.exe。
    echo 请安装 Visual Studio 的“使用 C++ 的桌面开发”组件，或从“x86 Native Tools Command Prompt”运行。
    goto :fail
)
for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSROOT=%%I"
if not defined VSROOT (
    echo [错误] 没有找到可用的 MSVC x86/x64 工具链。
    echo 请安装 Visual Studio 的“使用 C++ 的桌面开发”组件。
    goto :fail
)
call "%VSROOT%\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 (
    echo [错误] VsDevCmd.bat 初始化失败，无法切换到 x86 编译环境。
    goto :fail
)

where cl.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 cl.exe，请确认已安装 Visual Studio C++ 桌面开发组件。
    goto :fail
)

rem 业务单元保持 C++17；SDK Client 以纯 C 编译，最后统一无 CRT 链接。  
set CXXFLAGS=/nologo /c /std:c++17 /utf-8 /O2 /Oi- /W4 /WX /GR- /GS- /Gs999999999 /Zl /I"%SDK_DIR%\include"
set CLIENT_CFLAGS=/nologo /c /TC /utf-8 /O2 /Oi- /W4 /WX /GS- /Gs999999999 /Zl /I"%SDK_DIR%\include" /I"%SDK_DIR%\client"
set LFLAGS=/NOLOGO /DLL /NODEFAULTLIB /ENTRY:DllMain@12 /SUBSYSTEM:WINDOWS /MACHINE:X86 /DYNAMICBASE /NXCOMPAT

echo [1/1] MaxGrowthAndDrop.asi
cl.exe %CXXFLAGS% "%SRC_DIR%\MaxGrowthAndDrop.cpp" /Fo"%OBJ_DIR%\MaxGrowthAndDrop.obj"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% "%SDK_DIR%\client\runtime_client.c" /Fo"%OBJ_DIR%\runtime_client.obj"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% "%SDK_DIR%\client\runtime_entry_gate.c" /Fo"%OBJ_DIR%\runtime_entry_gate.obj"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% "%SDK_DIR%\client\runtime_client_support.c" /Fo"%OBJ_DIR%\runtime_client_support.obj"
if errorlevel 1 goto :fail
link.exe %LFLAGS% /DEF:"%SRC_DIR%\MaxGrowthAndDrop.def" /OUT:"%OUT_DIR%\MaxGrowthAndDrop.asi" "%OBJ_DIR%\MaxGrowthAndDrop.obj" "%OBJ_DIR%\runtime_client.obj" "%OBJ_DIR%\runtime_entry_gate.obj" "%OBJ_DIR%\runtime_client_support.obj" kernel32.lib
if errorlevel 1 goto :fail
call :check_pe "%OUT_DIR%\MaxGrowthAndDrop.asi"
if errorlevel 1 goto :fail

rmdir /s /q "%OBJ_DIR%" 2>nul
echo.
echo [成功] MaxGrowthAndDrop.asi 已输出到 build 目录，并且通过 x86 / DLL / 非零入口点检查。
pause
exit /b 0

:check_pe
rem 直接读取 PE 头，防止错误架构、非 DLL 或零入口产物进入交付目录。
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command ^
  "$p='%~1'; $b=[IO.File]::ReadAllBytes($p); if($b.Length -lt 64){exit 10}; $pe=[BitConverter]::ToInt32($b,0x3C); if($pe -lt 0 -or ($pe+44) -gt $b.Length){exit 11}; $m=[BitConverter]::ToUInt16($b,$pe+4); $c=[BitConverter]::ToUInt16($b,$pe+22); $ep=[BitConverter]::ToUInt32($b,$pe+40); if($m -ne 0x014C){Write-Host '[错误] 不是 x86 PE：' $p; exit 12}; if(($c -band 0x2000) -eq 0){Write-Host '[错误] PE 没有 DLL 标志：' $p; exit 13}; if($ep -eq 0){Write-Host '[错误] AddressOfEntryPoint=0，DllMain 不会执行：' $p; exit 14}; Write-Host ('[入口检查] PASS  RVA=0x{0:X8}  {1}' -f $ep,$p)"
exit /b %errorlevel%

:fail
rmdir /s /q "%OBJ_DIR%" 2>nul
echo.
echo [失败] MaxGrowthAndDrop 构建中止，请从上方第一条错误开始检查。
pause
exit /b 1
