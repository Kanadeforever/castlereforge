@echo off
setlocal DisableDelayedExpansion
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
"%SystemRoot%\System32\chcp.com" 65001 >nul
rem 不能假定启动本脚本的终端已经包含 System32；VsDevCmd 自己也会使用 findstr 等系统工具。  
rem 只在 PATH 前面补齐系统目录，不覆盖 GitHub Actions 或本机已经配置好的 LLVM 路径。  

set "ROOT=%~dp0"
set "OUT=%ROOT%_build"
set "TARGET=%ROOT%..\..\build\Castle_Widescreen.asi"
set "SDK=%ROOT%..\RuntimeSDK"

if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%" || goto :fail
if not exist "%ROOT%..\..\build\" mkdir "%ROOT%..\..\build\" || goto :fail

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="
if exist "%VSWHERE%" for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
if defined VSINSTALL (
    call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -no_logo -arch=x86 -host_arch=x64 >nul
    if errorlevel 1 goto :fail
)

set "CLANG_CL="
set "LLD_LINK="
for /f "delims=" %%I in ('where clang-cl 2^>nul') do if not defined CLANG_CL set "CLANG_CL=%%I"
for /f "delims=" %%I in ('where lld-link 2^>nul') do if not defined LLD_LINK set "LLD_LINK=%%I"

if not defined CLANG_CL (
    echo [构建] 未找到 clang-cl。请确认 LLVM 已安装并加入 PATH。  
    goto :fail
)
if not defined LLD_LINK (
    echo [构建] 未找到 lld-link。请确认 LLVM 已安装并加入 PATH。  
    goto :fail
)

set "CFLAGS=/nologo /c /O2 /Oi- /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc /I%SDK%\include /I%SDK%\client"

"%CLANG_CL%" %CFLAGS% "%ROOT%source\runtime.c" /Fo"%OUT%\runtime.obj" || goto :fail

"%CLANG_CL%" %CFLAGS% "%ROOT%source\widescreen.c" /Fo"%OUT%\widescreen.obj" || goto :fail

"%CLANG_CL%" %CFLAGS% "%ROOT%source\plugin.c" /Fo"%OUT%\plugin.obj" || goto :fail
"%CLANG_CL%" %CFLAGS% "%SDK%\client\runtime_client.c" /Fo"%OUT%\runtime_client.obj" || goto :fail
"%CLANG_CL%" %CFLAGS% "%SDK%\client\runtime_entry_gate.c" /Fo"%OUT%\runtime_entry_gate.obj" || goto :fail
"%CLANG_CL%" %CFLAGS% "%SDK%\client\runtime_client_support.c" /Fo"%OUT%\runtime_client_support.obj" || goto :fail

"%LLD_LINK%" /nologo /dll /nodefaultlib /machine:x86 /entry:DllMain@12 ^
    "%OUT%\runtime.obj" "%OUT%\widescreen.obj" "%OUT%\plugin.obj" ^
    "%OUT%\runtime_client.obj" "%OUT%\runtime_entry_gate.obj" "%OUT%\runtime_client_support.obj" ^
    /def:"%ROOT%source\Widescreen.def" kernel32.lib ^
    /implib:"%OUT%\Castle_Widescreen.lib" /out:"%TARGET%" || goto :fail

copy /y "%ROOT%templete\Castle_Widescreen.ini" "%ROOT%..\..\build\Castle_Widescreen.ini" >nul || goto :fail

if exist "%OUT%" rmdir /s /q "%OUT%"
echo [构建] 完成： %TARGET%  
pause
exit /b 0

:fail
if exist "%OUT%" rmdir /s /q "%OUT%"
echo [构建] 失败。未替换旧二进制文件。  
pause
exit /b 1
