@echo off
chcp 65001 >nul
setlocal EnableExtensions

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    "%VSWHERE%" -latest -products "*" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%TEMP%\ml_vswhere_path.txt" 2>nul
    set /p "VSINSTALL=" < "%TEMP%\ml_vswhere_path.txt"
    del /q "%TEMP%\ml_vswhere_path.txt" >nul 2>nul
)

if defined VSINSTALL if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 -no_logo
if not defined VSINSTALL (
    echo [提示] 未通过 vswhere 定位到 Visual Studio，将沿用当前 PATH 中的工具链。
)

where cl.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 cl.exe。
    echo 请先打开“x86 Native Tools Command Prompt for VS”，再运行本脚本。
    pause
    exit /b 1
)

where link.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 link.exe。
    pause
    exit /b 1
)

where rc.exe >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 rc.exe。
    echo 请确认当前是 Visual Studio 的 x86 Native Tools / Developer Command Prompt 环境，并安装了 Windows SDK。
    pause
    exit /b 1
)

set "ROOT=%~dp0"
set "SRC=%ROOT%src"
set "OBJ=%ROOT%.build"
set "OUT=%ROOT%编译内容"

if exist "%OBJ%" rmdir /s /q "%OBJ%"
mkdir "%OBJ%" >nul 2>nul
if not exist "%OUT%" mkdir "%OUT%"

if exist "%OUT%\CastleLocaleBootstrap.dll" del /q "%OUT%\CastleLocaleBootstrap.dll"
if exist "%OUT%\CastleModCore.dll" del /q "%OUT%\CastleModCore.dll"
if exist "%OUT%\CastleLocaleBootstrap.lib" del /q "%OUT%\CastleLocaleBootstrap.lib"
if exist "%OUT%\CastleModCore.lib" del /q "%OUT%\CastleModCore.lib"
if not exist "%OUT%\mods" mkdir "%OUT%\mods"
if not exist "%OUT%\mods\asi" mkdir "%OUT%\mods\asi"
if not exist "%OUT%\mods\overrides" mkdir "%OUT%\mods\overrides"

if not exist "%OUT%\mods\overrides\模板_复制后改名" mkdir "%OUT%\mods\overrides\模板_复制后改名"
if not exist "%OUT%\mods\overrides\模板_复制后改名\exe" mkdir "%OUT%\mods\overrides\模板_复制后改名\exe"
if not exist "%OUT%\mods\overrides\模板_复制后改名\MultiMedia" mkdir "%OUT%\mods\overrides\模板_复制后改名\MultiMedia"
for %%D in (fight Font Map menus Mov Music public save Sys) do (
    if not exist "%OUT%\mods\overrides\模板_复制后改名\MultiMedia\%%D" mkdir "%OUT%\mods\overrides\模板_复制后改名\MultiMedia\%%D"
)

copy /y "%ROOT%配置模板\CastleModLoader.ini" "%OUT%\mods\CastleModLoader.ini" >nul || goto :fail

set "CFLAGS=/nologo /c /TC /W4 /WX /utf-8 /GS- /GR- /EHsc- /O2 /Zl"

set "CXXFLAGS=/nologo /c /TP /W4 /WX /utf-8 /GS- /GR- /EHs- /EHc- /O2 /Zl"

echo [1/5] 编译 Launcher...
cl %CFLAGS% /Fo"%OBJ%\launcher.obj" "%SRC%\launcher.c" || goto :fail

cl %CFLAGS% /Fo"%OBJ%\launcher_gui.obj" "%SRC%\launcher_gui.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\launcher_mod_config.obj" "%SRC%\launcher_mod_config.c" || goto :fail
cl %CXXFLAGS% /Fo"%OBJ%\about.obj" "%SRC%\about.cpp" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\pe_import_injector.obj" "%SRC%\pe_import_injector.c" || goto :fail

echo [2/5] 编译 Launcher 图标资源...

copy /y "%SRC%\resource.h" "%OBJ%\resource.h" >nul || goto :fail
copy /y "%ROOT%资源\RPG.ico" "%OBJ%\RPG.ico" >nul || goto :fail
(
    echo #include "resource.h"
    echo.
    echo IDI_RPG_ICON ICON "RPG.ico"
) > "%OBJ%\launcher_gen.rc"
pushd "%OBJ%" || goto :fail
rc.exe /nologo /fo "launcher.res" "launcher_gen.rc"
if errorlevel 1 (
    popd
    goto :fail
)
popd

echo [3/5] 编译最早期区域环境 Bootstrap...
cl %CFLAGS% /Fo"%OBJ%\locale_bootstrap.obj" "%SRC%\locale_bootstrap.c" || goto :fail

echo [4/5] 编译目标进程 Core...
cl %CFLAGS% /Fo"%OBJ%\core.obj" "%SRC%\core.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\entry_gate.obj" "%SRC%\entry_gate.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\mod_loader.obj" "%SRC%\mod_loader.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\override_loader.obj" "%SRC%\override_loader.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\game_audit.obj" "%SRC%\game_audit.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\locale_layer.obj" "%SRC%\locale_layer.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\native_locale.obj" "%SRC%\native_locale.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\user32_locale.obj" "%SRC%\user32_locale.c" || goto :fail
cl %CFLAGS% /Fo"%OBJ%\gdi_locale.obj" "%SRC%\gdi_locale.c" || goto :fail
cl %CFLAGS% /Oi- /Fo"%OBJ%\runtime_support.obj" "%SRC%\runtime_support.c" || goto :fail

echo [5/5] 链接 PE32/i386 成品...
link /nologo /Brepro /machine:x86 /subsystem:windows /nodefaultlib /entry:WinMainCRTStartup ^
    /out:"%OUT%\CastleModLoader.exe" ^
    "%OBJ%\launcher.obj" "%OBJ%\launcher_gui.obj" "%OBJ%\launcher_mod_config.obj" "%OBJ%\about.obj" "%OBJ%\pe_import_injector.obj" "%OBJ%\runtime_support.obj" ^
    "%OBJ%\launcher.res" ^
    kernel32.lib || goto :fail

link /nologo /Brepro /dll /machine:x86 /nodefaultlib /entry:DllMain ^
    /out:"%OUT%\mods\CastleLocaleBootstrap.dll" ^
    /implib:"%OBJ%\CastleLocaleBootstrap.lib" ^
    "%OBJ%\locale_bootstrap.obj" "%OBJ%\runtime_support.obj" ^
    ntdll.lib || goto :fail

link /nologo /Brepro /dll /machine:x86 /nodefaultlib /entry:DllMain ^
    /out:"%OUT%\mods\CastleModCore.dll" ^
    /implib:"%OBJ%\CastleModCore.lib" ^
    "%OBJ%\core.obj" "%OBJ%\entry_gate.obj" "%OBJ%\mod_loader.obj" ^
    "%OBJ%\override_loader.obj" "%OBJ%\game_audit.obj" "%OBJ%\locale_layer.obj" "%OBJ%\native_locale.obj" ^
    "%OBJ%\user32_locale.obj" "%OBJ%\gdi_locale.obj" "%OBJ%\runtime_support.obj" ^
    kernel32.lib || goto :fail

echo.
echo [完成] 已生成：
echo   编译内容\CastleModLoader.exe  ^(已嵌入 资源\RPG.ico^)
echo   编译内容\mods\CastleLocaleBootstrap.dll
echo   编译内容\mods\CastleModCore.dll
echo.
echo 建议随后运行：python 工具\preloader_check.py "你的RPG.exe完整路径"
pause
exit /b 0

:fail
echo.
echo [失败] 构建过程中出现错误，请保留完整控制台输出用于排查。
pause
exit /b 1
