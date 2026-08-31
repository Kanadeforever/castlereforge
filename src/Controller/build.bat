setlocal DisableDelayedExpansion
chcp 65001 >nul
@echo off
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
  echo [错误] 没有找到支持的 Visual Studio VsDevCmd.bat。  
  echo [说明] 可编辑本文件中的 VSDEV 搜索路径，或者在已初始化的 x86 Developer Command Prompt 中自行执行同等命令。  
  pause
  exit /b 1
)

set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0"
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
  echo [错误] Visual Studio x86 开发环境初始化失败。  
  pause
  exit /b 1
)

where clang-cl >nul 2>nul || goto :tool_fail
where link >nul 2>nul || goto :tool_fail

if not exist "%OUT%" mkdir "%OUT%"

if exist "%ROOT%_build" rmdir /s /q "%ROOT%_build"
mkdir "%ROOT%_build"

set "CFLAGS=/nologo /c /O2 /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc -fno-builtin -Wno-void-pointer-to-int-cast -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast /I%SDK%\include /I%SDK%\client"

call :compile runtime.c runtime.obj || goto :fail
call :compile pad_input.c pad_input.obj || goto :fail
call :compile input_router.c input_router.obj || goto :fail
call :compile pad_public_api.c pad_public_api.obj || goto :fail
call :compile movie_skip.c movie_skip.obj || goto :fail
call :compile confirm_dialog.c confirm_dialog.obj || goto :fail
call :compile dialogue_input.c dialogue_input.obj || goto :fail
call :compile cursor.c cursor.obj || goto :fail
call :compile exploration.c exploration.obj || goto :fail
call :compile investigation.c investigation.obj || goto :fail
call :compile control_modes.c control_modes.obj || goto :fail
call :compile ui_bridge.c ui_bridge.obj || goto :fail
call :compile interface_shell.c interface_shell.obj || goto :fail
call :compile interface_items.c interface_items.obj || goto :fail
call :compile interface_skills.c interface_skills.obj || goto :fail
call :compile interface_equipment.c interface_equipment.obj || goto :fail
call :compile interface_inner_stats.c interface_inner_stats.obj || goto :fail
call :compile spatial_neighbor.c spatial_neighbor.obj || goto :fail
call :compile interface_formation.c interface_formation.obj || goto :fail
call :compile interface_tome.c interface_tome.obj || goto :fail
call :compile interface_options.c interface_options.obj || goto :fail
call :compile inn.c inn.obj || goto :fail
call :compile synthesis.c synthesis.obj || goto :fail
call :compile shop.c shop.obj || goto :fail
call :compile scene_choice.c scene_choice.obj || goto :fail
call :compile save_slot.c save_slot.obj || goto :fail
call :compile save_point.c save_point.obj || goto :fail
call :compile frontend.c frontend.obj || goto :fail
call :compile battle.c battle.obj || goto :fail
call :compile plugin.c plugin.obj || goto :fail
echo [编译] RuntimeSDK Client  
clang-cl %CFLAGS% "%SDK%\client\runtime_client.c" /Fo:"%ROOT%_build\runtime_client.obj"
if errorlevel 1 goto :fail
clang-cl %CFLAGS% "%SDK%\client\runtime_entry_gate.c" /Fo:"%ROOT%_build\runtime_entry_gate.obj"
if errorlevel 1 goto :fail
clang-cl %CFLAGS% "%SDK%\client\runtime_client_support.c" /Fo:"%ROOT%_build\runtime_client_support.obj"
if errorlevel 1 goto :fail

echo [链接] Castle_PadSupport.asi  
link /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 /def:"%ROOT%source\Castle_PadSupport.def" ^
  "%ROOT%_build\runtime.obj" "%ROOT%_build\pad_input.obj" "%ROOT%_build\input_router.obj" "%ROOT%_build\pad_public_api.obj" "%ROOT%_build\movie_skip.obj" ^
  "%ROOT%_build\confirm_dialog.obj" "%ROOT%_build\dialogue_input.obj" ^
  "%ROOT%_build\cursor.obj" "%ROOT%_build\exploration.obj" "%ROOT%_build\investigation.obj" "%ROOT%_build\control_modes.obj" "%ROOT%_build\ui_bridge.obj" "%ROOT%_build\interface_shell.obj" "%ROOT%_build\interface_items.obj" "%ROOT%_build\interface_skills.obj" "%ROOT%_build\interface_equipment.obj" "%ROOT%_build\interface_inner_stats.obj" "%ROOT%_build\spatial_neighbor.obj" "%ROOT%_build\interface_formation.obj" "%ROOT%_build\interface_tome.obj" "%ROOT%_build\interface_options.obj" ^
  "%ROOT%_build\inn.obj" "%ROOT%_build\synthesis.obj" "%ROOT%_build\shop.obj" "%ROOT%_build\scene_choice.obj" ^
  "%ROOT%_build\save_slot.obj" "%ROOT%_build\save_point.obj" "%ROOT%_build\frontend.obj" "%ROOT%_build\battle.obj" "%ROOT%_build\plugin.obj" ^
  "%ROOT%_build\runtime_client.obj" "%ROOT%_build\runtime_entry_gate.obj" "%ROOT%_build\runtime_client_support.obj" ^
  kernel32.lib ^
  /out:"%OUT%\Castle_PadSupport.asi"
if errorlevel 1 goto :fail

copy /y "%ROOT%templete\Castle_PadSupport.ini" "%OUT%\Castle_PadSupport.ini" >nul
if errorlevel 1 goto :fail


del /q "%OUT%\Castle_PadSupport.lib" 2>nul
del /q "%OUT%\Castle_PadSupport.exp" 2>nul
rmdir /s /q "%ROOT%_build"

echo.
echo done
echo [打包] ASI、INI 已同步到 build 目录。  
pause
exit /b 0

:compile
echo [编译] %1  
clang-cl %CFLAGS% "%ROOT%source\%1" /Fo:"%ROOT%_build\%2"
if errorlevel 1 exit /b 1
exit /b 0

:tool_fail
echo faild
pause
exit /b 1

:fail
echo.
echo faild
if exist "%ROOT%_build" rmdir /s /q "%ROOT%_build"
pause
exit /b 1
