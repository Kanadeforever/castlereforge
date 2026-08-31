@echo off
setlocal DisableDelayedExpansion
"%SystemRoot%\System32\chcp.com" 65001 >nul

rem 统一输出目录：仓库根 build\
set "ROOT=%~dp0"
set "OUT=%ROOT%..\..\build"
set "SDK=%ROOT%..\RuntimeSDK"

set "VSDEV="
if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"

if not defined VSDEV (
  echo [错误] 没有找到 Visual Studio 开发环境。
  exit /b 1
)

set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
set "INCLUDE="
set "LIB="
set "LIBPATH="
set "__VSCMD_PREINIT_PATH="
set "VSCMD_ARG_TGT_ARCH="
set "VSCMD_ARG_HOST_ARCH="
set "VSCMD_VER="
set "VSINSTALLDIR="
set "VCINSTALLDIR="
set "VisualStudioVersion="

call "%VSDEV%" -no_logo -arch=x86 -host_arch=x64 >nul
if errorlevel 1 (
  echo [错误] Visual Studio x86 环境初始化失败。
  exit /b 1
)

where clang-cl >nul 2>nul || goto :tool_fail
where link >nul 2>nul || goto :tool_fail

if not exist "%OUT%" mkdir "%OUT%"
if exist "%ROOT%_build" rmdir /s /q "%ROOT%_build"
mkdir "%ROOT%_build"

set "CFLAGS=/nologo /c /O2 /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc -fno-builtin -Wno-cast-function-type-mismatch /I%SDK%\include /I%SDK%\client"

call :compile runtime.c runtime.obj || goto :fail
call :compile mouse_input.c mouse_input.obj || goto :fail
call :compile pad_bridge.c pad_bridge.obj || goto :fail
call :compile backlog.c backlog.obj || goto :fail
call :compile plugin.c plugin.obj || goto :fail
echo [编译] RuntimeSDK Client
clang-cl %CFLAGS% "%SDK%\client\runtime_client.c" /Fo:"%ROOT%_build\runtime_client.obj"
if errorlevel 1 goto :fail
clang-cl %CFLAGS% "%SDK%\client\runtime_entry_gate.c" /Fo:"%ROOT%_build\runtime_entry_gate.obj"
if errorlevel 1 goto :fail
clang-cl %CFLAGS% "%SDK%\client\runtime_client_support.c" /Fo:"%ROOT%_build\runtime_client_support.obj"
if errorlevel 1 goto :fail

echo [链接] Castle_Backlog.asi
link /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 ^
  "%ROOT%_build\runtime.obj" "%ROOT%_build\mouse_input.obj" "%ROOT%_build\pad_bridge.obj" ^
  "%ROOT%_build\backlog.obj" "%ROOT%_build\plugin.obj" "%ROOT%_build\runtime_client.obj" ^
  "%ROOT%_build\runtime_entry_gate.obj" "%ROOT%_build\runtime_client_support.obj" ^
  /def:"%ROOT%source\Backlog.def" ^
  kernel32.lib user32.lib /out:"%OUT%\Castle_Backlog.asi"
if errorlevel 1 goto :fail

copy /y "%ROOT%templete\Castle_Backlog.ini" "%OUT%\Castle_Backlog.ini" >nul || goto :fail

del /q "%OUT%\Castle_Backlog.lib" 2>nul
del /q "%OUT%\Castle_Backlog.exp" 2>nul
rmdir /s /q "%ROOT%_build"

echo [完成] ASI、INI 已写入 build 目录
pause
exit /b 0

:compile
echo [编译] %1
clang-cl %CFLAGS% "%ROOT%source\%1" /Fo:"%ROOT%_build\%2"
if errorlevel 1 exit /b 1
exit /b 0

:tool_fail
echo [错误] VsDevCmd 后仍找不到 clang-cl.exe 或 link.exe。
exit /b 1

:fail
echo [错误] 构建已停止；不会把半套产物报告为成功。
if exist "%ROOT%_build" rmdir /s /q "%ROOT%_build"
exit /b 1
