@echo off
setlocal DisableDelayedExpansion
"%SystemRoot%\System32\chcp.com" 65001 >nul

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

if not exist "..\release" mkdir "..\release"
if exist "_build" rmdir /s /q "_build"
mkdir "_build"

set "CFLAGS=/nologo /c /O2 /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc -fno-builtin -Wno-cast-function-type-mismatch"

call :compile runtime.c runtime.obj || goto :fail
call :compile mouse_input.c mouse_input.obj || goto :fail
call :compile pad_bridge.c pad_bridge.obj || goto :fail
call :compile backlog.c backlog.obj || goto :fail
call :compile plugin.c plugin.obj || goto :fail

echo [链接] Castle_Backlog.asi
link /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 ^
  "_build\runtime.obj" "_build\mouse_input.obj" "_build\pad_bridge.obj" ^
  "_build\backlog.obj" "_build\plugin.obj" ^
  kernel32.lib user32.lib /out:"..\release\Castle_Backlog.asi"
if errorlevel 1 goto :fail

copy /y "Castle_Backlog.ini" "..\release\Castle_Backlog.ini" >nul || goto :fail

del /q "..\release\Castle_Backlog.lib" 2>nul
del /q "..\release\Castle_Backlog.exp" 2>nul
rmdir /s /q "_build"

echo [完成] ASI、INI、全部中文文档和最新检查器已写入 ..\release
exit /b 0

:compile
echo [编译] %1
clang-cl %CFLAGS% "%1" /Fo:"_build\%2"
if errorlevel 1 exit /b 1
exit /b 0

:tool_fail
echo [错误] VsDevCmd 后仍找不到 clang-cl.exe 或 link.exe。
exit /b 1

:fail
echo [错误] 构建已停止；不会把半套产物报告为成功。
if exist "_build" rmdir /s /q "_build"
exit /b 1
