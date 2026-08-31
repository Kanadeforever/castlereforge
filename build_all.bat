setlocal EnableExtensions DisableDelayedExpansion
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
chcp 65001 >nul

@echo off

rem 先补齐 Windows 自带工具目录，同时保留 GitHub Actions 或用户已经加入 PATH 的 LLVM、Python 等工具。  
rem 每个子项目都从这份完整环境继承工具路径，避免前一个构建初始化 Visual Studio 后影响下一个构建。  
set "ROOT=%~dp0"

echo [1/9] RuntimeSDK 核心  
call "%ROOT%src\RuntimeSDK\build.bat" < nul || goto :fail
cls
echo [2/9] 对话历史  
call "%ROOT%src\Backlog\build.bat" < nul || goto :fail
cls
echo [3/9] 控制器支持  
call "%ROOT%src\Controller\build.bat" < nul || goto :fail
cls
echo [4/9] 宽屏  
call "%ROOT%src\Widescreen\build.bat" < nul || goto :fail
cls
echo [5/9] 安全扩展存档  
call "%ROOT%src\SaveEnhance\build.bat" < nul || goto :fail
cls
echo [6/9] 问题修复  
call "%ROOT%src\Extra\BUGFix\build.bat" < nul || goto :fail
cls
echo [7/9] 免CD  
call "%ROOT%src\Extra\NoCD\build.bat" < nul || goto :fail
cls
echo [8/9] 最大成长和最大掉宝  
call "%ROOT%src\Extra\MaxGrowthAndDrop\build.bat" < nul || goto :fail
cls
echo [9/9] 模组加载器  
call "%ROOT%src\MODLoader\build.bat" < nul || goto :fail
cls
echo [移动] 移动 Runtime、ASI 与同名 INI 到 mods\asi...  
@echo off
if not exist "%ROOT%build\mods\asi" mkdir "%ROOT%build\mods\asi"
for %%F in ("%ROOT%build\*.asi") do (
    move /y "%%F" "%ROOT%build\mods\asi\" >nul || goto :fail
    if exist "%%~dpnF.ini" move /y "%%~dpnF.ini" "%ROOT%build\mods\asi\" >nul || goto :fail
)
if not exist "%ROOT%build\Castle_Runtime.dll" goto :fail
move /y "%ROOT%build\Castle_Runtime.dll" "%ROOT%build\mods\asi\Castle_Runtime.dll" >nul || goto :fail

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
echo [完成] Runtime/ASI/INI 位于 build\mods\asi，加载器位于 build\。FPSUnlock 仍未纳入。  
pause
exit /b 0

:fail
cls
echo.
echo [失败] build_all 中止，请检查上方错误。  
pause
exit /b 1
