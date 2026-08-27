rem ============================================================ 
rem build_all.bat - 一键编译全部子项目，输出统一到仓库根 build\ 
rem 前置：Visual Studio x86 工具链； 
rem Widescreen 需要 LLVM（clang-cl/lld-link）在 PATH 
rem 注：SaveEnhance 为独立主功能目录，当前构建产物仍名为 AnytimeSave.asi
rem ============================================================ 

@echo off

chcp 65001 >nul
setlocal EnableExtensions

@echo off

set "ROOT=%~dp0"

echo [1/8] Backlog
call "%ROOT%src\Backlog\build.bat" < nul || goto :fail

echo [2/8] Controller（手柄支持）
call "%ROOT%src\Controller\build.bat" < nul || goto :fail

echo [3/8] Widescreen（宽屏）
call "%ROOT%src\Widescreen\build.bat" < nul || goto :fail

echo [4/8] SaveEnhance（安全扩展存档）
call "%ROOT%src\SaveEnhance\build.bat" < nul || goto :fail

echo [5/8] BUGFix（原版问题修复）
call "%ROOT%src\Extra\BUGFix\build.bat" < nul || goto :fail

echo [6/8] NoCD（免 CD）
call "%ROOT%src\Extra\NoCD\build.bat" < nul || goto :fail

echo [7/8] MaxGrowthAndDrop（最大成长与掉宝）
call "%ROOT%src\Extra\MaxGrowthAndDrop\build.bat" < nul || goto :fail

echo [8/8] MODLoader（模组加载器）
call "%ROOT%src\MODLoader\build.bat" < nul || goto :fail

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
