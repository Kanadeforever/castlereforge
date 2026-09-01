@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
chcp 65001 >nul

rem =============================================================================  
rem Castle_Quest v0.1-dev6zd - CastleReforge 主项目构建脚本。  
rem 本脚本位于 src\Quest\build.bat，并要求同级 src\RuntimeSDK 已存在。  
rem 所有编译中间文件只允许进入 src\Quest\_build；主项目 build 只允许保留发行文件。  
rem 主项目 build 由多个子项目共同使用，因此本脚本绝不能清空整个 build，只能替换 Quest 自己的发行项。  
rem =============================================================================  

set "ROOT=%~dp0"
set "REPOROOT=%ROOT%..\.."
set "SDK=%ROOT%..\RuntimeSDK"
set "SOURCE_MAIN=%ROOT%source\CastleQuest.cpp"
set "SOURCE_ROUTE=%ROOT%source\RouteSearch.cpp"
set "SOURCE_DEF=%ROOT%source\CastleQuest.def"
set "INI=%ROOT%templete\Castle_Quest.ini"
set "DATADIR=%ROOT%templete\Castle_Quest"
set "OUT=%ROOT%_build"
set "RELEASE=%OUT%\release"
set "TARGETDIR=%REPOROOT%\build"
set "TARGETASI=%TARGETDIR%\Castle_Quest.asi"
set "TARGETINI=%TARGETDIR%\Castle_Quest.ini"
set "TARGETDATA=%TARGETDIR%\Castle_Quest"

rem 在接触旧发行文件之前，先验证主项目目录、RuntimeSDK 文件和 Quest 运行数据是否齐全。  
if not exist "%SDK%\include\CastleRuntime_Client.h" (
    echo [构建] 缺少 src\RuntimeSDK\include\CastleRuntime_Client.h；请先把 Quest 放入 CastleReforge 主仓。  
    goto :fail_keep_release
)
if not exist "%SDK%\include\CastleHook_API.h" (
    echo [构建] 缺少 RuntimeSDK Hook API。  
    goto :fail_keep_release
)
if not exist "%SDK%\client\runtime_client.c" (
    echo [构建] 缺少 RuntimeSDK Client 源码。  
    goto :fail_keep_release
)
if not exist "%SDK%\client\runtime_entry_gate.c" (
    echo [构建] 缺少 RuntimeSDK Entry Gate 源码。  
    goto :fail_keep_release
)
if not exist "%SDK%\client\runtime_client_support.c" (
    echo [构建] 缺少 RuntimeSDK Client Support 源码。  
    goto :fail_keep_release
)
if not exist "%SDK%\client\client_internal.h" (
    echo [构建] 缺少 RuntimeSDK Client 内部头文件。  
    goto :fail_keep_release
)
if not exist "%SOURCE_MAIN%" (
    echo [构建] 缺少 source\CastleQuest.cpp。  
    goto :fail_keep_release
)
if not exist "%SOURCE_ROUTE%" (
    echo [构建] 缺少 source\RouteSearch.cpp。  
    goto :fail_keep_release
)
if not exist "%SOURCE_DEF%" (
    echo [构建] 缺少 source\CastleQuest.def。  
    goto :fail_keep_release
)
if not exist "%INI%" (
    echo [构建] 缺少 templete\Castle_Quest.ini。  
    goto :fail_keep_release
)
if not exist "%DATADIR%\manifest.toml" (
    echo [构建] 缺少任务数据库 manifest.toml。  
    goto :fail_keep_release
)

rem manifest.toml 是发行硬闸门，不是装饰性版本文件；下面同时核对声明值与真实文件数量。  
findstr /X /C:"format_version = 7" "%DATADIR%\manifest.toml" >nul || goto :manifest_bad
findstr /B /L /C:"database_version = " "%DATADIR%\manifest.toml" | findstr /L /C:"research-v0.6+dev6zd-runtimesdk-migration" >nul || goto :manifest_bad
findstr /X /C:"quest_count = 25" "%DATADIR%\manifest.toml" >nul || goto :manifest_bad
findstr /X /C:"addon_count = 25" "%DATADIR%\manifest.toml" >nul || goto :manifest_bad
findstr /X /C:"stage_count = 146" "%DATADIR%\manifest.toml" >nul || goto :manifest_bad
findstr /X /C:"canonical_stage_count = 146" "%DATADIR%\manifest.toml" >nul || goto :manifest_bad
findstr /I /C:"guidance_stage_count" "%DATADIR%\manifest.toml" >nul && goto :manifest_bad
findstr /I /C:"warning_count" "%DATADIR%\manifest.toml" >nul && goto :manifest_bad

set /a BASE_COUNT=0
set /a ADDON_COUNT=0
set /a STAGE_COUNT=0

rem Addon 文件固定带 _addon 后缀，因此单独计数并逐文件验证格式版本，避免和 Base 混在一起。  
for %%F in ("%DATADIR%\Q*_addon.toml") do (
    set /a ADDON_COUNT+=1
    findstr /X /C:"format_version = 2" "%%~fF" >nul || goto :manifest_bad
)

rem Base 文件通过目录清单排除 _addon 后取得；同时逐文件拒绝已经废止的 Guidance 与 Warning 字段。  
for /f "delims=" %%F in ('dir /b /a-d "%DATADIR%\Q*.toml" ^| findstr /I /V /E "_addon.toml"') do (
    set /a BASE_COUNT+=1
    findstr /X /C:"format_version = 7" "%DATADIR%\%%F" >nul || goto :manifest_bad
    findstr /B /C:"stage_type" "%DATADIR%\%%F" >nul && goto :manifest_bad
    findstr /B /C:"guidance_scene" "%DATADIR%\%%F" >nul && goto :manifest_bad
    findstr /L /B /C:"[[warning]]" "%DATADIR%\%%F" >nul && goto :manifest_bad
    for /f %%N in ('findstr /B /L /C:"[[stage]]" "%DATADIR%\%%F" ^| find /c /v ""') do set /a STAGE_COUNT+=%%N
)
if not "%BASE_COUNT%"=="25" goto :manifest_bad
if not "%ADDON_COUNT%"=="25" goto :manifest_bad
if not "%STAGE_COUNT%"=="146" goto :manifest_bad

rem 私有中间目录每次从空目录开始；只有全部验证通过后才替换主项目中已有的 Quest 发行文件。  
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%" || goto :fail_keep_release
mkdir "%RELEASE%" || goto :fail_keep_release

rem 沿用主项目的 Visual Studio 自动发现方式；Quest 使用 STL/CRT，所以保持 MSVC C++17 与 /MT。  
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="
if exist "%VSWHERE%" for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
if defined VSINSTALL (
    call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -no_logo -arch=x86 -host_arch=x64 >nul
    if errorlevel 1 goto :fail_keep_release
)
where cl >nul 2>nul || (
    echo [构建] 未找到 MSVC cl.exe x86 工具链。  
    goto :fail_keep_release
)
where link >nul 2>nul || goto :fail_keep_release
where dumpbin >nul 2>nul || goto :fail_keep_release

set "CPPFLAGS=/nologo /c /MT /EHsc /std:c++17 /permissive- /W4 /Od /utf-8 /DUNICODE /D_UNICODE /I%SDK%\include /I%SDK%\client"
set "CFLAGS=/nologo /c /MT /W4 /Od /utf-8 /TC /I%SDK%\include /I%SDK%\client"

echo [构建] 编译 Castle_Quest 与 RuntimeSDK Client，目标 Win32/x86。  
cl %CPPFLAGS% "%SOURCE_MAIN%" /Fo"%OUT%\CastleQuest.obj" || goto :fail_keep_release
cl %CPPFLAGS% "%SOURCE_ROUTE%" /Fo"%OUT%\RouteSearch.obj" || goto :fail_keep_release
cl %CFLAGS% "%SDK%\client\runtime_client.c" /Fo"%OUT%\runtime_client.obj" || goto :fail_keep_release
cl %CFLAGS% "%SDK%\client\runtime_entry_gate.c" /Fo"%OUT%\runtime_entry_gate.obj" || goto :fail_keep_release
cl %CFLAGS% "%SDK%\client\runtime_client_support.c" /Fo"%OUT%\runtime_client_support.obj" || goto :fail_keep_release

link /nologo /DLL /SUBSYSTEM:WINDOWS /MACHINE:X86 /INCREMENTAL:NO ^
    /DEF:"%SOURCE_DEF%" /OUT:"%OUT%\Castle_Quest.asi" /IMPLIB:"%OUT%\Castle_Quest.lib" ^
    "%OUT%\CastleQuest.obj" "%OUT%\RouteSearch.obj" ^
    "%OUT%\runtime_client.obj" "%OUT%\runtime_entry_gate.obj" "%OUT%\runtime_client_support.obj" ^
    user32.lib gdi32.lib || goto :fail_keep_release

rem 生成的 ASI 必须是 PE32/x86，并且三个 RuntimeSDK/旧加载器 ABI 导出名都必须保持无修饰形式。  
dumpbin /headers "%OUT%\Castle_Quest.asi" | findstr /I /C:"machine (x86)" /C:"14C machine" >nul || goto :binary_bad
dumpbin /nologo /exports "%OUT%\Castle_Quest.asi" > "%OUT%\exports.txt" || goto :binary_bad
findstr /R /C:" CastlePlugin_Query$" "%OUT%\exports.txt" >nul || goto :binary_bad
findstr /R /C:" InitializeASI$" "%OUT%\exports.txt" >nul || goto :binary_bad
findstr /R /C:" CastleRuntimeClient_NotifyLoaderReady$" "%OUT%\exports.txt" >nul || goto :binary_bad
findstr /R /C:" _CastlePlugin_Query$" /C:" _InitializeASI$" /C:" _CastleRuntimeClient_NotifyLoaderReady$" "%OUT%\exports.txt" >nul && goto :binary_bad

rem /MT 是明确的发行要求：接入 RuntimeSDK Client 后也不能让 Quest ASI 新增外部 MSVC 运行库 DLL 依赖。  
dumpbin /dependents "%OUT%\Castle_Quest.asi" | findstr /I /C:"MSVCP" /C:"VCRUNTIME" >nul && goto :binary_bad

rem 先在私有目录组装完整发行候选；候选未完成前绝不修改主项目 build 中已有的 Quest 发行文件。  
copy /y "%OUT%\Castle_Quest.asi" "%RELEASE%\Castle_Quest.asi" >nul || goto :fail_keep_release
copy /y "%INI%" "%RELEASE%\Castle_Quest.ini" >nul || goto :fail_keep_release
xcopy "%DATADIR%" "%RELEASE%\Castle_Quest\" /E /I /Y /Q >nul || goto :fail_keep_release
for /f %%N in ('dir /b /a-d "%RELEASE%\Castle_Quest" 2^>nul ^| find /c /v ""') do set "RELEASE_DATA_COUNT=%%N"
if not "%RELEASE_DATA_COUNT%"=="51" goto :binary_bad
for /f %%N in ('dir /b /ad "%RELEASE%\Castle_Quest" 2^>nul ^| find /c /v ""') do set "RELEASE_DATA_DIR_COUNT=%%N"
if not "%RELEASE_DATA_DIR_COUNT%"=="0" goto :binary_bad

rem 发布时只替换 Quest 自己拥有的输出；绝不删除或清理主项目里其他子模块的发行文件。  
if not exist "%TARGETDIR%" mkdir "%TARGETDIR%" || goto :fail_keep_release
copy /y "%RELEASE%\Castle_Quest.asi" "%TARGETASI%" >nul || goto :fail_keep_release
copy /y "%RELEASE%\Castle_Quest.ini" "%TARGETINI%" >nul || goto :fail_keep_release
if exist "%TARGETDATA%" rmdir /s /q "%TARGETDATA%"
xcopy "%RELEASE%\Castle_Quest" "%TARGETDATA%\" /E /I /Y /Q >nul || goto :fail_keep_release

rem 主项目 build 不允许出现编译残留；发行候选验证并发布成功后删除私有 _build。  
if exist "%OUT%" rmdir /s /q "%OUT%"
echo [构建] 完成：%TARGETASI%  
echo [构建] 任务数据：%TARGETDATA%（manifest + 25 Base + 25 Addon）。  
pause
exit /b 0

:manifest_bad
echo [构建] manifest.toml 与实际任务文件不一致，或检测到已经废止的 Guidance/Warning 字段。  
goto :fail_keep_release

:binary_bad
echo [构建] 二进制、导出表或发行数据验证失败；不会把私有中间文件当作发行版。  
goto :fail_keep_release

:fail_keep_release
if exist "%OUT%" rmdir /s /q "%OUT%"
echo [构建] 失败；已存在的 CastleReforge build 中其它模块文件不会被清理。  
pause
exit /b 1
