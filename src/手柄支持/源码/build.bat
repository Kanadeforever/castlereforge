@echo off
setlocal DisableDelayedExpansion
"%SystemRoot%\System32\chcp.com" 65001 >nul

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

if not exist "..\编译内容" mkdir "..\编译内容"

::
:: “源码包”和“编译内容包”都必须能独立告诉接手者当前发生过什么。
:: 仓库里的权威中文文档位于顶层 docs\手柄支持；这里先准备两个镜像目录：
:: - ..\文档                 属于手柄源码包；
:: - ..\编译内容\文档       属于可以直接交给测试者的编译内容包。
:: 编译内容还携带最新检查器，所以另建 ..\编译内容\工具。
:: mkdir 只在目录不存在时执行，不会删除用户已有文件。
::
if not exist "..\文档" mkdir "..\文档"
if not exist "..\编译内容\文档" mkdir "..\编译内容\文档"
if not exist "..\编译内容\工具" mkdir "..\编译内容\工具"
if exist "_build" rmdir /s /q "_build"
mkdir "_build"

set "CFLAGS=/nologo /c /O2 /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc -fno-builtin -Wno-void-pointer-to-int-cast -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast"

call :compile runtime.c runtime.obj || goto :fail
call :compile pad_input.c pad_input.obj || goto :fail
call :compile input_router.c input_router.obj || goto :fail
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

echo [链接] Castle_PadSupport.asi
link /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 ^
  "_build\runtime.obj" "_build\pad_input.obj" "_build\input_router.obj" "_build\movie_skip.obj" ^
  "_build\confirm_dialog.obj" "_build\dialogue_input.obj" ^
  "_build\cursor.obj" "_build\exploration.obj" "_build\investigation.obj" "_build\control_modes.obj" "_build\ui_bridge.obj" "_build\interface_shell.obj" "_build\interface_items.obj" "_build\interface_skills.obj" "_build\interface_equipment.obj" "_build\interface_inner_stats.obj" "_build\spatial_neighbor.obj" "_build\interface_formation.obj" "_build\interface_tome.obj" "_build\interface_options.obj" ^
  "_build\inn.obj" "_build\synthesis.obj" "_build\shop.obj" "_build\scene_choice.obj" ^
  "_build\save_slot.obj" "_build\save_point.obj" "_build\frontend.obj" "_build\battle.obj" "_build\plugin.obj" ^
  /out:"..\编译内容\Castle_PadSupport.asi"
if errorlevel 1 goto :fail

::
:: ASI 和 INI 必须成对进入编译内容。
:: ASI 是程序本体；INI 告诉用户当前默认使用哪一种调查激活方式，并保存全部公开参数。
:: 如果这里只生成 ASI、不复制 INI，用户即使拿到新代码也看不到 ActivationMode=0 的默认契约。
:: copy 失败必须和编译失败一样中止，不能把缺配置的半套产物标成成功。
::
copy /y "Castle_PadSupport.ini" "..\编译内容\Castle_PadSupport.ini" >nul
if errorlevel 1 goto :fail

::
:: 同步全部现行中文文档，而不是只复制本轮更新记录。
:: 这样完整接档、架构、地址、失败方案、测试和工具说明不会出现“有的包是R41，
:: 有的包还停在R40”的半同步状态。任一 copy 失败都中止构建。
::
copy /y "..\..\..\docs\手柄支持\*.md" "..\文档\" >nul
if errorlevel 1 goto :fail
copy /y "..\..\..\docs\手柄支持\*.md" "..\编译内容\文档\" >nul
if errorlevel 1 goto :fail

:: 最新稳定检查器和它的简体中文说明必须跟随编译内容包。
copy /y "..\工具\refactor_check.py" "..\编译内容\工具\refactor_check.py" >nul
if errorlevel 1 goto :fail
copy /y "..\..\..\docs\手柄支持\工具详细说明.md" "..\编译内容\工具\工具详细说明.md" >nul
if errorlevel 1 goto :fail

del /q "..\编译内容\Castle_PadSupport.lib" 2>nul
del /q "..\编译内容\Castle_PadSupport.exp" 2>nul
rmdir /s /q "_build"

echo.
echo done
echo [打包] ASI + INI + 全部中文文档 + 最新检查器已同步到编译内容。
pause
exit /b 0

:compile
echo [编译] %1
clang-cl %CFLAGS% "%1" /Fo:"_build\%2"
if errorlevel 1 exit /b 1
exit /b 0

:tool_fail
echo faild
pause
exit /b 1

:fail
echo.
echo faild
if exist "_build" rmdir /s /q "_build"
pause
exit /b 1
