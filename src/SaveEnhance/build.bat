@echo off
setlocal EnableExtensions DisableDelayedExpansion
chcp 65001 >nul

@echo off

cd /d "%~dp0"
set "SCRIPT_DIR=%~dp0"
set "OUT_DIR=%SCRIPT_DIR%..\..\build"
set "OBJ_DIR=%SCRIPT_DIR%_build"
set "TOOLS_CACHE=%SCRIPT_DIR%tools\__pycache__"
set "TEMPLATE_INI=%SCRIPT_DIR%templete\Castle_SaveEnhance.ini"
set "VC_TOOLS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.51.36231"
set "CL_EXE=%VC_TOOLS%\bin\Hostx64\x86\cl.exe"
set "LINK_EXE=%VC_TOOLS%\bin\Hostx64\x86\link.exe"
set "DUMPBIN_EXE=%VC_TOOLS%\bin\Hostx64\x86\dumpbin.exe"
set "KERNEL32_LIB=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86\kernel32.lib"
set "POWERSHELL_EXE=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"
if not exist "%CL_EXE%" goto :missing_cl
if not exist "%LINK_EXE%" goto :missing_link
if not exist "%DUMPBIN_EXE%" goto :missing_dumpbin
if not exist "%KERNEL32_LIB%" goto :missing_kernel32
if not exist "%POWERSHELL_EXE%" goto :missing_powershell
if not exist "%TEMPLATE_INI%" goto :missing_ini
goto :tools_ready

:missing_cl
echo [错误] 找不到 x86 cl.exe：%CL_EXE% 
echo [说明] 请依据 C:\Project\编译器地址记录.txt 更新 VC_TOOLS。 
goto :fail

:missing_link
echo [错误] 找不到 x86 link.exe：%LINK_EXE% 
goto :fail

:missing_dumpbin
echo [错误] 找不到 x86 dumpbin.exe：%DUMPBIN_EXE% 
goto :fail

:missing_kernel32
echo [错误] 找不到 x86 kernel32.lib：%KERNEL32_LIB% 
goto :fail

:missing_powershell
echo [错误] 找不到 Windows PowerShell。 
goto :fail

:missing_ini
echo [错误] 找不到 templete\Castle_SaveEnhance.ini。 
goto :fail

:tools_ready
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
if exist "%OBJ_DIR%" rmdir /s /q "%OBJ_DIR%"
if exist "%TOOLS_CACHE%" rmdir /s /q "%TOOLS_CACHE%"
mkdir "%OBJ_DIR%"
del /q "%OUT_DIR%\Castle_SaveEnhance.asi" 2>nul
del /q "%OUT_DIR%\Castle_SaveEnhance.ini" 2>nul

echo [1/4] 编译 Castle_SaveEnhance.cpp 
"%CL_EXE%" /nologo /c /std:c++17 /utf-8 /O2 /Oi- /W4 /WX /GR- /GS- /Gs999999999 /Zl ^
  "source\Castle_SaveEnhance.cpp" /Fo:"%OBJ_DIR%\Castle_SaveEnhance.obj"
if errorlevel 1 goto :fail

echo [2/4] 链接 PE32 Castle_SaveEnhance.asi 
"%LINK_EXE%" /nologo /Brepro /DLL /NODEFAULTLIB /ENTRY:DllMain /SUBSYSTEM:WINDOWS /MACHINE:X86 ^
  /IMPLIB:"%OBJ_DIR%\Castle_SaveEnhance.lib" /OUT:"%OUT_DIR%\Castle_SaveEnhance.asi" ^
  "%OBJ_DIR%\Castle_SaveEnhance.obj" "%KERNEL32_LIB%"
if errorlevel 1 goto :fail

echo [3/4] 验证 PE32、DLL、入口点与 InitializeASI 导出 
"%POWERSHELL_EXE%" -NoProfile -ExecutionPolicy Bypass -Command ^
  "$p='%OUT_DIR%\Castle_SaveEnhance.asi'; $b=[IO.File]::ReadAllBytes($p); if($b.Length -lt 64){exit 10}; $pe=[BitConverter]::ToInt32($b,0x3C); if($pe -lt 0 -or ($pe+44) -gt $b.Length){exit 11}; if([Text.Encoding]::ASCII.GetString($b,$pe,4) -ne ('PE'+[char]0+[char]0)){exit 12}; $m=[BitConverter]::ToUInt16($b,$pe+4); $c=[BitConverter]::ToUInt16($b,$pe+22); $ep=[BitConverter]::ToUInt32($b,$pe+40); if($m -ne 0x014C -or ($c -band 0x2000) -eq 0 -or $ep -eq 0){exit 13}"
if errorlevel 1 (
    echo [错误] PE 基础验证失败。 
    goto :fail
)
"%DUMPBIN_EXE%" /nologo /exports "%OUT_DIR%\Castle_SaveEnhance.asi" > "%OBJ_DIR%\Castle_SaveEnhance.exports.txt"
if errorlevel 1 (
    echo [错误] dumpbin 无法读取最终 ASI。 
    goto :fail
)
set "HAS_INITIALIZE_ASI="
for /f "usebackq tokens=4" %%E in ("%OBJ_DIR%\Castle_SaveEnhance.exports.txt") do (
    if "%%E"=="InitializeASI" set "HAS_INITIALIZE_ASI=1"
)
if not defined HAS_INITIALIZE_ASI (
    echo [错误] 最终 ASI 没有导出 InitializeASI。 
    goto :fail
)

echo [4/4] 复制 INI 
copy /y "%TEMPLATE_INI%" "%OUT_DIR%\Castle_SaveEnhance.ini" >nul
if errorlevel 1 goto :package_fail

rmdir /s /q "%OBJ_DIR%"
if exist "%TOOLS_CACHE%" rmdir /s /q "%TOOLS_CACHE%"
echo.
echo [成功] ASI 与 INI 已输出到仓库根 build。  
echo [成功] 已确认 PE32、DLL、非零入口点及 InitializeASI 导出。  
echo [成功] 临时编译目录与 tools\__pycache__ 已清理。  
pause
exit /b 0

:package_fail
echo [错误] 复制 INI 失败。 
goto :fail

:fail
del /q "%OUT_DIR%\Castle_SaveEnhance.asi" 2>nul
del /q "%OUT_DIR%\Castle_SaveEnhance.ini" 2>nul
if exist "%OBJ_DIR%" rmdir /s /q "%OBJ_DIR%"
if exist "%TOOLS_CACHE%" rmdir /s /q "%TOOLS_CACHE%"
echo.
echo [失败] Castle_SaveEnhance 构建中止，请检查上方第一条错误。 
pause
exit /b 1
