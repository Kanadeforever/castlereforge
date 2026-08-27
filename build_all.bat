chcp 65001 >nul
setlocal EnableExtensions
set "ROOT=%~dp0"

@echo off
echo [1/8] 对话历史
call "%ROOT%src\Backlog\build.bat" < nul || goto :fail
cls
echo [2/8] 控制器支持
call "%ROOT%src\Controller\build.bat" < nul || goto :fail
cls
echo [3/8] 宽屏
call "%ROOT%src\Widescreen\build.bat" < nul || goto :fail
cls
echo [4/8] 安全扩展存档
call "%ROOT%src\SaveEnhance\build.bat" < nul || goto :fail
cls
echo [5/8] 问题修复
call "%ROOT%src\Extra\BUGFix\build.bat" < nul || goto :fail
cls
echo [6/8] 免CD
call "%ROOT%src\Extra\NoCD\build.bat" < nul || goto :fail
cls
echo [7/8] 最大成长&最大掉宝
call "%ROOT%src\Extra\MaxGrowthAndDrop\build.bat" < nul || goto :fail
cls
echo [8/8] 模组加载器
call "%ROOT%src\MODLoader\build.bat" < nul || goto :fail
cls
echo [移动] 移动 ASI 与同名 INI 到 mods\asi...
@echo off
if not exist "%ROOT%build\mods\asi" mkdir "%ROOT%build\mods\asi"
for %%F in ("%ROOT%build\*.asi") do (
    move /y "%%F" "%ROOT%build\mods\asi\" >nul || goto :fail
    if exist "%%~dpnF.ini" move /y "%%~dpnF.ini" "%ROOT%build\mods\asi\" >nul || goto :fail
)
cls
echo [移动] 完成
cls
echo.
echo [完成] 全部子项目编译完成：ASI/INI 位于 build\mods\asi，加载器位于 build\
pause
exit /b 0

:fail
cls
echo.
echo [失败] build_all 中止，请检查上方错误
pause
exit /b 1
