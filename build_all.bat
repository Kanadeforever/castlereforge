@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
chcp 65001 >nul

@echo off
rem 上面这个echo off是必要的，不要删除  


rem 先补齐 Windows 自带工具目录，同时保留 GitHub Actions 或用户已经加入 PATH 的 LLVM、Python 等工具。  
rem 每个子项目都从这份完整环境继承工具路径，避免前一个构建初始化 Visual Studio 后影响下一个构建。  
set "ROOT=%~dp0"

rem 总构建必须证明本轮源码真的产生了每个正式文件，不能让上一次留下的 ASI 冒充成功。  
rem 这里只删除 build 发行树里由本脚本重新生成的精确文件名，不触碰用户源码或其它目录。  
if not exist "%ROOT%build" mkdir "%ROOT%build" || goto :fail
for %%F in (Castle_Runtime.dll Castle_Backlog.asi Castle_Backlog.toml Castle_PadSupport.asi Castle_PadSupport.toml Castle_Widescreen.asi Castle_Widescreen.toml Castle_SaveEnhance.asi Castle_SaveEnhance.toml BUGFix.asi NoCD.asi MaxGrowthAndDrop.asi MaxGrowthAndDrop.toml Castle_Quest.asi Castle_Quest.toml) do (
    if exist "%ROOT%build\%%F" del /q "%ROOT%build\%%F" >nul 2>nul
    if exist "%ROOT%build\mods\asi\%%F" del /q "%ROOT%build\mods\asi\%%F" >nul 2>nul
)
if exist "%ROOT%build\Castle_Quest" rmdir /s /q "%ROOT%build\Castle_Quest"
if exist "%ROOT%build\mods\asi\Castle_Quest" rmdir /s /q "%ROOT%build\mods\asi\Castle_Quest"

echo [1/10] RuntimeSDK 核心  
call "%ROOT%src\RuntimeSDK\build.bat" < nul || goto :fail
cls
echo [2/10] 对话历史  
call "%ROOT%src\Backlog\build.bat" < nul || goto :fail
cls
echo [3/10] 控制器支持  
call "%ROOT%src\Controller\build.bat" < nul || goto :fail
cls
echo [4/10] 宽屏  
call "%ROOT%src\Widescreen\build.bat" < nul || goto :fail
cls
echo [5/10] 安全扩展存档  
call "%ROOT%src\SaveEnhance\build.bat" < nul || goto :fail
cls
echo [6/10] 问题修复  
call "%ROOT%src\Extra\BUGFix\build.bat" < nul || goto :fail
cls
echo [7/10] 免CD  
call "%ROOT%src\Extra\NoCD\build.bat" < nul || goto :fail
cls
echo [8/10] 最大成长和最大掉宝  
call "%ROOT%src\Extra\MaxGrowthAndDrop\build.bat" < nul || goto :fail
cls
echo [9/10] 任务系统  
call "%ROOT%src\Quest\build.bat" < nul || goto :fail
cls
echo [10/10] 模组加载器  
call "%ROOT%src\MODLoader\build.bat" < nul || goto :fail
cls
echo [移动] 移动 Runtime、ASI 与同名 TOML 到 mods\asi...  
@echo off
if not exist "%ROOT%build\mods\asi" mkdir "%ROOT%build\mods\asi"
for %%F in ("%ROOT%build\*.asi") do (
    move /y "%%F" "%ROOT%build\mods\asi\" >nul || goto :fail
    if exist "%%~dpnF.ini" move /y "%%~dpnF.ini" "%ROOT%build\mods\asi\" >nul || goto :fail
    if exist "%%~dpnF.toml" move /y "%%~dpnF.toml" "%ROOT%build\mods\asi\" >nul || goto :fail
)
if not exist "%ROOT%build\Castle_Runtime.dll" goto :fail
move /y "%ROOT%build\Castle_Runtime.dll" "%ROOT%build\mods\asi\Castle_Runtime.dll" >nul || goto :fail
if exist "%ROOT%build\Castle_Quest" (
    if exist "%ROOT%build\mods\asi\Castle_Quest" rmdir /s /q "%ROOT%build\mods\asi\Castle_Quest"
    move /y "%ROOT%build\Castle_Quest" "%ROOT%build\mods\asi\Castle_Quest" >nul || goto :fail
)
if not exist "%ROOT%build\mods\logs" mkdir "%ROOT%build\mods\logs" || goto :fail

rem 官方 ASI 已统一改用 TOML；主动清除旧版残留 INI，避免发行目录同时出现两份互相矛盾的配置。  
for %%F in (Castle_Backlog Castle_PadSupport Castle_SaveEnhance Castle_Widescreen MaxGrowthAndDrop Castle_Quest) do (
    if exist "%ROOT%build\mods\asi\%%F.ini" del /q "%ROOT%build\mods\asi\%%F.ini" >nul 2>nul
)

rem SaveEnhance 的外置 WAV 固定从 ASI 同目录下 Castle_SaveEnhance 子目录读取。  
set "SAVE_SOUND_DIR=%ROOT%build\mods\asi\Castle_SaveEnhance"
if not exist "%SAVE_SOUND_DIR%" mkdir "%SAVE_SOUND_DIR%" || goto :fail
copy /y "%ROOT%docs\SaveEnhance\安装与INI配置说明.md" "%SAVE_SOUND_DIR%\音效放置与INI配置说明.md" >nul || goto :fail
copy /y "%ROOT%docs\SaveEnhance\实机测试清单.md" "%SAVE_SOUND_DIR%\SaveEnhance实机测试清单.md" >nul || goto :fail

rem build 是发行目录；任何链接导入库、对象、调试数据库都必须在打包前清除。  
for /r "%ROOT%build" %%F in (*.obj *.lib *.exp *.ilk *.pdb) do if exist "%%F" del /q "%%F" >nul 2>nul

set "PYTHON_EXE="
set "PYTHON_ARGS="
for /f "delims=" %%I in ('where python.exe 2^>nul') do if not defined PYTHON_EXE set "PYTHON_EXE=%%I"
if not defined PYTHON_EXE (
    for /f "delims=" %%I in ('where py.exe 2^>nul') do if not defined PYTHON_EXE set "PYTHON_EXE=%%I"
    if defined PYTHON_EXE set "PYTHON_ARGS=-3"
)
if not defined PYTHON_EXE goto :fail
echo [校验] 检查发行文件、标准导出和零编译垃圾...  
"%PYTHON_EXE%" %PYTHON_ARGS% "%ROOT%src\RuntimeSDK\tools\runtime_sdk_check.py" --require-release
if errorlevel 1 goto :fail
cls
echo [移动] 完成  
cls
echo.
echo [完成] Runtime、全部官方ASI及配置位于 build\mods\asi，统一日志目录为 build\mods\logs，加载器位于 build\。  
pause
exit /b 0

:fail
cls
echo.
echo [失败] build_all 中止，请检查上方错误。  
pause
exit /b 1
