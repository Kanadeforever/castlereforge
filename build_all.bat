rem ============================================================ 
rem build_all.bat - 一键编译全部子项目，输出统一到仓库根 build\ 
rem 前置：Visual Studio x86 工具链； 
rem Widescreen 需要 LLVM（clang-cl/lld-link）在 PATH 
rem 注：AnytimeSaveProbe为诊断工具，默认不参与一键编译 
rem ============================================================ 

chcp 65001 >nul
setlocal EnableExtensions
@echo off

set "ROOT=%~dp0"

echo [1/5] Backlog
call "%ROOT%src\Backlog\build.bat" || goto :fail

echo [2/5] Controller（手柄支持）
call "%ROOT%src\Controller\build.bat" || goto :fail

echo [3/5] Widescreen（宽屏）
call "%ROOT%src\Widescreen\build.bat" || goto :fail

echo [4/5] Extra（其他功能）
call "%ROOT%src\Extra\build.bat" || goto :fail

echo [5/5] MODLoader（模组加载器）
call "%ROOT%src\MODLoader\build.bat" || goto :fail

echo.
echo [UPX] 压缩全部 ASI/DLL/EXE（--best --lzma）...
set "UPXEXE="
for /f "delims=" %%I in ('where upx 2^>nul') do if not defined UPXEXE set "UPXEXE=%%I"
if not defined UPXEXE (
    echo [错误] 未找到 upx.exe，请安装 UPX 并加入 PATH。
    goto :fail
)
for %%F in ("%ROOT%build\*.asi" "%ROOT%build\*.exe" "%ROOT%build\mods\*.dll") do (
    "%UPXEXE%" --best --lzma "%%F" >nul 2>nul
    if errorlevel 1 (
        echo [UPX] 跳过（不可压缩）: %%~nxF
    )
)
echo [UPX] 压缩完成

echo.
echo [移动] 移动 ASI 与同名 INI 到 mods\asi...
if not exist "%ROOT%build\mods\asi" mkdir "%ROOT%build\mods\asi"
for %%F in ("%ROOT%build\*.asi") do (
    move /y "%%F" "%ROOT%build\mods\asi\" >nul || goto :fail
    if exist "%%~dpnF.ini" move /y "%%~dpnF.ini" "%ROOT%build\mods\asi\" >nul || goto :fail
)
echo [移动] 完成

echo.
echo [完成] 全部子项目编译完成：ASI/INI 位于 build\mods\asi，加载器位于 build\
pause
exit /b 0

:fail
echo.
echo [失败] build_all 中止，请检查上方错误
pause
exit /b 1
