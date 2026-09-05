@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
"%SystemRoot%\System32\chcp.com" 65001 >nul

@echo off

cd /d "%~dp0"

rem =============================================================================  
rem Castle_Quest v0.1-dev6zd - CastleReforge 主项目构建脚本。  
rem 本脚本位于 src\Quest\build.bat，要求同级 src\RuntimeSDK 已经存在。  
rem 所有编译中间文件只进入 src\Quest\_build。  
rem 主项目根目录 build 只保存发行文件，本脚本绝不会清空其他子项目的发行内容。  
rem 本脚本不依赖外置 PowerShell 校验脚本，避免 CMD 续行符与空格造成解析错误。  
rem =============================================================================  

set "ROOT=%~dp0"
set "REPOROOT=%ROOT%..\.."
set "SDK=%ROOT%..\RuntimeSDK"
set "SRC=%ROOT%source"
set "TEMPLATE=%ROOT%templete"
set "DATADIR=%TEMPLATE%\Castle_Quest"
set "MANIFEST=%DATADIR%\manifest.toml"

set "SOURCE_MAIN=%SRC%\CastleQuest.cpp"
set "SOURCE_ROUTE=%SRC%\RouteSearch.cpp"
set "SOURCE_DEF=%SRC%\CastleQuest.def"
set "INI=%TEMPLATE%\Castle_Quest.toml"

set "OBJ=%ROOT%_build"
set "RELEASE=%OBJ%\release"

set "TARGETDIR=%REPOROOT%\build"
set "TARGETASI=%TARGETDIR%\Castle_Quest.asi"
set "TARGETINI=%TARGETDIR%\Castle_Quest.toml"
set "TARGETDATA=%TARGETDIR%\Castle_Quest"

set "TARGETASI_NEW=%TARGETDIR%\Castle_Quest.asi.__new"
set "TARGETINI_NEW=%TARGETDIR%\Castle_Quest.toml.__new"
set "TARGETDATA_NEW=%TARGETDIR%\Castle_Quest.__new"

rem =============================================================================  
rem 第一阶段：检查主项目目录和 RuntimeSDK 必需文件。  
rem =============================================================================  

if not exist "%SOURCE_MAIN%" (
    echo [构建] 缺少 source\CastleQuest.cpp。  
    goto :fail
)

if not exist "%SOURCE_ROUTE%" (
    echo [构建] 缺少 source\RouteSearch.cpp。  
    goto :fail
)

if not exist "%SOURCE_DEF%" (
    echo [构建] 缺少 source\CastleQuest.def。  
    goto :fail
)

if not exist "%INI%" (
    echo [构建] 缺少 templete\Castle_Quest.toml。  
    goto :fail
)

if not exist "%MANIFEST%" (
    echo [构建] 缺少 templete\Castle_Quest\manifest.toml。  
    goto :fail
)

if not exist "%SDK%\include\CastleRuntime_Client.h" (
    echo [构建] 缺少 src\RuntimeSDK\include\CastleRuntime_Client.h。  
    goto :fail
)

if not exist "%SDK%\include\CastleHook_API.h" (
    echo [构建] 缺少 src\RuntimeSDK\include\CastleHook_API.h。  
    goto :fail
)

if not exist "%SDK%\client\runtime_client.c" (
    echo [构建] 缺少 src\RuntimeSDK\client\runtime_client.c。  
    goto :fail
)

if not exist "%SDK%\client\runtime_entry_gate.c" (
    echo [构建] 缺少 src\RuntimeSDK\client\runtime_entry_gate.c。  
    goto :fail
)

if not exist "%SDK%\client\client_internal.h" (
    echo [构建] 缺少 src\RuntimeSDK\client\client_internal.h。  
    goto :fail
)

rem =============================================================================  
rem 第二阶段：读取 manifest.toml 中的真实声明值。  
rem 不把 25、146 之类的数据重复硬编码在构建逻辑里。  
rem 以后任务数据库扩展时，只要 Manifest 与实际文件同步，构建脚本无需跟着改计数。  
rem =============================================================================  

set "MF_FORMAT="
set "MF_QUEST="
set "MF_ADDON="
set "MF_STAGE="
set "MF_CANONICAL="

call :ReadManifestNumber "format_version" MF_FORMAT
if errorlevel 1 goto :manifest_bad

call :ReadManifestNumber "quest_count" MF_QUEST
if errorlevel 1 goto :manifest_bad

call :ReadManifestNumber "addon_count" MF_ADDON
if errorlevel 1 goto :manifest_bad

call :ReadManifestNumber "stage_count" MF_STAGE
if errorlevel 1 goto :manifest_bad

call :ReadManifestNumber "canonical_stage_count" MF_CANONICAL
if errorlevel 1 goto :manifest_bad

if not "%MF_FORMAT%"=="7" (
    echo [构建] manifest.toml 的 format_version=%MF_FORMAT%，当前代码要求 7。  
    goto :manifest_bad
)

findstr /L /C:"database_version = " "%MANIFEST%" >nul
if errorlevel 1 (
    echo [构建] manifest.toml 缺少 database_version。  
    goto :manifest_bad
)

findstr /I /L /C:"guidance_stage_count" "%MANIFEST%" >nul
if not errorlevel 1 (
    echo [构建] manifest.toml 仍包含已经废止的 guidance_stage_count。  
    goto :manifest_bad
)

findstr /I /L /C:"warning_count" "%MANIFEST%" >nul
if not errorlevel 1 (
    echo [构建] manifest.toml 仍包含已经废止的 warning_count。  
    goto :manifest_bad
)

if exist "%DATADIR%\routes.toml" (
    echo [构建] 检测到已经废止的 routes.toml；Route 必须属于各 Stage。  
    goto :manifest_bad
)

rem =============================================================================  
rem 第三阶段：重新扫描全部 Q*.toml，机械核对 Base、Addon、Stage 与废止字段。  
rem 这里完全使用 Windows CMD/findstr，不调用 PowerShell。  
rem =============================================================================  

set /a BASE_COUNT=0
set /a ADDON_COUNT=0
set /a STAGE_COUNT=0
set "MANIFEST_ERROR="

for /f "delims=" %%F in ('dir /b /a-d "%DATADIR%\Q*.toml" 2^>nul') do call :ValidateQuestFile "%%F"

if defined MANIFEST_ERROR goto :manifest_bad

if not "%BASE_COUNT%"=="%MF_QUEST%" (
    echo [构建] Base 实际数量=%BASE_COUNT%，manifest quest_count=%MF_QUEST%。  
    goto :manifest_bad
)

if not "%ADDON_COUNT%"=="%MF_ADDON%" (
    echo [构建] Addon 实际数量=%ADDON_COUNT%，manifest addon_count=%MF_ADDON%。  
    goto :manifest_bad
)

if not "%STAGE_COUNT%"=="%MF_STAGE%" (
    echo [构建] Base Stage 实际数量=%STAGE_COUNT%，manifest stage_count=%MF_STAGE%。  
    goto :manifest_bad
)

if not "%STAGE_COUNT%"=="%MF_CANONICAL%" (
    echo [构建] Base Stage 实际数量=%STAGE_COUNT%，manifest canonical_stage_count=%MF_CANONICAL%。  
    goto :manifest_bad
)

echo [构建] Manifest 核验通过：Base=%BASE_COUNT% Addon=%ADDON_COUNT% Stage=%STAGE_COUNT%。  

rem =============================================================================  
rem 第四阶段：准备私有编译目录。  
rem 只有 Manifest 与任务数据全部通过以后，才开始查找编译器和生成目标文件。  
rem =============================================================================  

if exist "%OBJ%" rmdir /s /q "%OBJ%"
mkdir "%OBJ%"
if errorlevel 1 (
    echo [构建] 无法创建临时编译目录：%OBJ%。  
    goto :fail
)

mkdir "%RELEASE%"
if errorlevel 1 (
    echo [构建] 无法创建发行候选目录：%RELEASE%。  
    goto :fail
)

rem =============================================================================  
rem 第五阶段：自动定位 Visual Studio / MSVC x86 工具链。  
rem 优先使用 vswhere 找到安装了 C++ x86/x64 工具的最新 Visual Studio。  
rem 如果找不到 vswhere，则继续尝试当前命令行 PATH 中已经存在的 cl/link/dumpbin。  
rem =============================================================================  

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="
set "VSWHERE_TMP=%TEMP%\castle_quest_vswhere_%RANDOM%_%RANDOM%.txt"

if exist "%VSWHERE%" (
    "%VSWHERE%" -latest -products "*" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%VSWHERE_TMP%" 2>nul
    if exist "%VSWHERE_TMP%" (
        set /p "VSINSTALL=" < "%VSWHERE_TMP%"
        del /q "%VSWHERE_TMP%" >nul 2>nul
    )
)

if defined VSINSTALL (
    if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" (
        call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 -no_logo
        if errorlevel 1 (
            echo [构建] Visual Studio x86 开发环境初始化失败。  
            goto :fail
        )
    )
)

where cl.exe >nul 2>nul
if errorlevel 1 (
    echo [构建] 找不到 cl.exe；请安装 Visual Studio C++ x86/x64 编译工具。  
    goto :fail
)

where link.exe >nul 2>nul
if errorlevel 1 (
    echo [构建] 找不到 link.exe。  
    goto :fail
)

where dumpbin.exe >nul 2>nul
if errorlevel 1 (
    echo [构建] 找不到 dumpbin.exe。  
    goto :fail
)

rem =============================================================================  
rem 第六阶段：编译 Quest C++ 与 RuntimeSDK Client。  
rem Quest 使用 STL 和 CRT，因此保持 C++17、/MT 静态 CRT、Win32/x86。  
rem RuntimeSDK Client 的三个 C 文件直接编入本 ASI，不复制 SDK 源码进 Quest。  
rem =============================================================================  

set "CPPFLAGS=/nologo /c /TP /MT /EHsc /std:c++17 /permissive- /W4 /O2 /utf-8 /DUNICODE /D_UNICODE /I%SDK%\include /I%SDK%\client"
set "CFLAGS=/nologo /c /TC /MT /W4 /O2 /utf-8 /I%SDK%\include /I%SDK%\client"

echo [构建] 编译 CastleQuest.cpp。  
cl.exe %CPPFLAGS% "%SOURCE_MAIN%" /Fo"%OBJ%\CastleQuest.obj"
if errorlevel 1 goto :compile_bad

echo [构建] 编译 RouteSearch.cpp。  
cl.exe %CPPFLAGS% "%SOURCE_ROUTE%" /Fo"%OBJ%\RouteSearch.obj"
if errorlevel 1 goto :compile_bad

echo [构建] 编译 RuntimeSDK runtime_client.c。  
cl.exe %CFLAGS% "%SDK%\client\runtime_client.c" /Fo"%OBJ%\runtime_client.obj"
if errorlevel 1 goto :compile_bad

echo [构建] 编译 RuntimeSDK runtime_entry_gate.c。  
cl.exe %CFLAGS% "%SDK%\client\runtime_entry_gate.c" /Fo"%OBJ%\runtime_entry_gate.obj"
if errorlevel 1 goto :compile_bad


rem =============================================================================  
rem 第七阶段：链接为 PE32/x86 ASI。  
rem 这里显式列出 kernel32/user32/gdi32，避免依赖隐含库推断。  
rem =============================================================================  

echo [构建] 链接 Castle_Quest.asi。  
link.exe /nologo /DLL /SUBSYSTEM:WINDOWS /MACHINE:X86 /INCREMENTAL:NO /OPT:REF /OPT:ICF ^
    /DEF:"%SOURCE_DEF%" ^
    /OUT:"%OBJ%\Castle_Quest.asi" ^
    /IMPLIB:"%OBJ%\Castle_Quest.lib" ^
    "%OBJ%\CastleQuest.obj" ^
    "%OBJ%\RouteSearch.obj" ^
    "%OBJ%\runtime_client.obj" ^
    "%OBJ%\runtime_entry_gate.obj" ^
    kernel32.lib user32.lib gdi32.lib

if errorlevel 1 goto :link_bad

rem =============================================================================  
rem 第八阶段：验证最终 ASI 的机器类型、标准导出和 CRT 依赖。  
rem =============================================================================  

dumpbin.exe /headers "%OBJ%\Castle_Quest.asi" > "%OBJ%\headers.txt"
if errorlevel 1 goto :binary_bad

findstr /I /C:"machine (x86)" /C:"14C machine" "%OBJ%\headers.txt" >nul
if errorlevel 1 (
    echo [构建] 最终 ASI 不是 PE32/x86。  
    goto :binary_bad
)

dumpbin.exe /nologo /exports "%OBJ%\Castle_Quest.asi" > "%OBJ%\exports.txt"
if errorlevel 1 goto :binary_bad

findstr /R /C:"[ ][ ]*CastlePlugin_Query$" "%OBJ%\exports.txt" >nul
if errorlevel 1 (
    echo [构建] 最终 ASI 缺少 CastlePlugin_Query 导出。  
    goto :binary_bad
)

findstr /R /C:"[ ][ ]*InitializeASI$" "%OBJ%\exports.txt" >nul
if errorlevel 1 (
    echo [构建] 最终 ASI 缺少 InitializeASI 导出。  
    goto :binary_bad
)

findstr /R /C:"[ ][ ]*CastleRuntimeClient_NotifyLoaderReady$" "%OBJ%\exports.txt" >nul
if errorlevel 1 (
    echo [构建] 最终 ASI 缺少 CastleRuntimeClient_NotifyLoaderReady 导出。  
    goto :binary_bad
)

findstr /R /C:"[ ][ ]*_CastlePlugin_Query$" /C:"[ ][ ]*_InitializeASI$" /C:"[ ][ ]*_CastleRuntimeClient_NotifyLoaderReady$" "%OBJ%\exports.txt" >nul
if not errorlevel 1 (
    echo [构建] 检测到带下划线修饰的错误导出名。  
    goto :binary_bad
)

dumpbin.exe /dependents "%OBJ%\Castle_Quest.asi" > "%OBJ%\dependents.txt"
if errorlevel 1 goto :binary_bad

findstr /I /C:"MSVCP" /C:"VCRUNTIME" /C:"UCRTBASE" "%OBJ%\dependents.txt" >nul
if not errorlevel 1 (
    echo [构建] 最终 ASI 出现动态 MSVC/UCRT 依赖，/MT 要求未满足。  
    goto :binary_bad
)

rem =============================================================================  
rem 第九阶段：在私有目录组装完整发行候选。  
rem 发行数据固定是 manifest + Base + Addon，不允许把编译残留混进去。  
rem =============================================================================  

copy /y "%OBJ%\Castle_Quest.asi" "%RELEASE%\Castle_Quest.asi" >nul
if errorlevel 1 goto :release_bad

copy /y "%INI%" "%RELEASE%\Castle_Quest.toml" >nul
if errorlevel 1 goto :release_bad

xcopy "%DATADIR%" "%RELEASE%\Castle_Quest\" /E /I /Y /Q >nul
if errorlevel 1 goto :release_bad

set /a EXPECTED_DATA_COUNT=MF_QUEST+MF_ADDON+1
set /a RELEASE_DATA_COUNT=0
set /a RELEASE_DIR_COUNT=0

for /f "delims=" %%F in ('dir /b /a-d "%RELEASE%\Castle_Quest" 2^>nul') do set /a RELEASE_DATA_COUNT+=1
for /f "delims=" %%D in ('dir /b /ad "%RELEASE%\Castle_Quest" 2^>nul') do set /a RELEASE_DIR_COUNT+=1

if not "%RELEASE_DATA_COUNT%"=="%EXPECTED_DATA_COUNT%" (
    echo [构建] 发行任务数据实际文件数=%RELEASE_DATA_COUNT%，预期=%EXPECTED_DATA_COUNT%。  
    goto :release_bad
)

if not "%RELEASE_DIR_COUNT%"=="0" (
    echo [构建] Castle_Quest 发行数据目录中出现了不应存在的子目录。  
    goto :release_bad
)

rem =============================================================================  
rem 第十阶段：发布到主项目根 build。  
rem 先写 __new 临时发行项，全部准备成功后才替换 Quest 自己的旧发行文件。  
rem 不会删除 build 中 RuntimeSDK、其他 ASI、INI 或其它子项目资源。  
rem =============================================================================  

if not exist "%TARGETDIR%" (
    mkdir "%TARGETDIR%"
    if errorlevel 1 goto :release_bad
)

if exist "%TARGETASI_NEW%" del /q "%TARGETASI_NEW%" >nul 2>nul
if exist "%TARGETINI_NEW%" del /q "%TARGETINI_NEW%" >nul 2>nul
if exist "%TARGETDATA_NEW%" rmdir /s /q "%TARGETDATA_NEW%"

copy /y "%RELEASE%\Castle_Quest.asi" "%TARGETASI_NEW%" >nul
if errorlevel 1 goto :release_bad

copy /y "%RELEASE%\Castle_Quest.toml" "%TARGETINI_NEW%" >nul
if errorlevel 1 goto :release_bad

xcopy "%RELEASE%\Castle_Quest" "%TARGETDATA_NEW%\" /E /I /Y /Q >nul
if errorlevel 1 goto :release_bad

move /y "%TARGETASI_NEW%" "%TARGETASI%" >nul
if errorlevel 1 goto :release_bad

move /y "%TARGETINI_NEW%" "%TARGETINI%" >nul
if errorlevel 1 goto :release_bad

if exist "%TARGETDATA%" rmdir /s /q "%TARGETDATA%"
move "%TARGETDATA_NEW%" "%TARGETDATA%" >nul
if errorlevel 1 goto :release_bad

rem =============================================================================  
rem 第十一阶段：成功后清理所有私有中间文件。  
rem =============================================================================  

if exist "%OBJ%" rmdir /s /q "%OBJ%"

echo [构建] 成功：build\Castle_Quest.asi。  
echo [构建] 成功：build\Castle_Quest.toml。  
echo [构建] 成功：build\Castle_Quest 数据目录，共 %EXPECTED_DATA_COUNT% 个任务数据文件。  
echo [构建] Manifest：Base=%BASE_COUNT% Addon=%ADDON_COUNT% Stage=%STAGE_COUNT%。  
pause
exit /b 0

rem =============================================================================  
rem 下面是 Manifest 扫描辅助函数。  
rem =============================================================================  

:ReadManifestNumber
set "%~2="
for /f "tokens=3" %%V in ('findstr /L /C:"%~1 = " "%MANIFEST%" 2^>nul') do set "%~2=%%V"
if not defined %~2 (
    echo [构建] manifest.toml 缺少数值字段：%~1。  
    exit /b 1
)
exit /b 0

:ValidateQuestFile
set "QNAME=%~1"
set "QFULL=%DATADIR%\%~1"

echo(%QNAME%| findstr /I /E /L /C:"_addon.toml" >nul
if not errorlevel 1 goto :ValidateAddonFile

goto :ValidateBaseFile

:ValidateAddonFile
set /a ADDON_COUNT+=1

findstr /L /C:"format_version = 2" "%QFULL%" >nul
if errorlevel 1 (
    echo [构建] %QNAME% 的 Addon format_version 不是 2。  
    set "MANIFEST_ERROR=1"
)

exit /b 0

:ValidateBaseFile
set /a BASE_COUNT+=1

findstr /L /C:"format_version = 7" "%QFULL%" >nul
if errorlevel 1 (
    echo [构建] %QNAME% 的 Base format_version 不是 7。  
    set "MANIFEST_ERROR=1"
)

findstr /I /L /C:"stage_type =" "%QFULL%" >nul
if not errorlevel 1 (
    echo [构建] %QNAME% 仍包含已经废止的 stage_type。  
    set "MANIFEST_ERROR=1"
)

findstr /I /L /C:"guidance_scene =" "%QFULL%" >nul
if not errorlevel 1 (
    echo [构建] %QNAME% 仍包含已经废止的 guidance_scene。  
    set "MANIFEST_ERROR=1"
)

findstr /I /L /C:"[[warning]]" "%QFULL%" >nul
if not errorlevel 1 (
    echo [构建] %QNAME% 仍包含已经废止的 [[warning]]。  
    set "MANIFEST_ERROR=1"
)

for /f "delims=" %%L in ('findstr /L /C:"[[stage]]" "%QFULL%" 2^>nul') do set /a STAGE_COUNT+=1

if not exist "%DATADIR%\%~n1_addon.toml" (
    echo [构建] %QNAME% 缺少同名 _addon.toml。  
    set "MANIFEST_ERROR=1"
)

exit /b 0

rem =============================================================================  
rem 下面是统一失败出口。  
rem =============================================================================  

:manifest_bad
echo [构建] manifest.toml / Base / Addon 数据硬闸门检查失败。  
goto :fail

:compile_bad
echo [构建] C/C++ 编译失败，请查看上方第一条 error Cxxxx。  
goto :fail

:link_bad
echo [构建] 链接 Castle_Quest.asi 失败，请查看上方第一条 LNK 错误。  
goto :fail

:binary_bad
echo [构建] PE32、导出表或运行库依赖验证失败。  
goto :fail

:release_bad
echo [构建] 发行候选组装或发布失败。  
goto :fail

:fail
if exist "%VSWHERE_TMP%" del /q "%VSWHERE_TMP%" >nul 2>nul
if exist "%OBJ%" rmdir /s /q "%OBJ%"
if exist "%TARGETASI_NEW%" del /q "%TARGETASI_NEW%" >nul 2>nul
if exist "%TARGETINI_NEW%" del /q "%TARGETINI_NEW%" >nul 2>nul
if exist "%TARGETDATA_NEW%" rmdir /s /q "%TARGETDATA_NEW%"
echo [构建] 失败；CastleReforge build 中其它模块的发行文件不会被清理。  
pause
exit /b 1
