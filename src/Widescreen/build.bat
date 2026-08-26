@echo off
setlocal DisableDelayedExpansion
chcp 65001 >nul

set "ROOT=%~dp0"
set "OUT=%ROOT%_build"
set "TARGET=%ROOT%..\..\build\Castle_Widescreen.asi"

if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%" || goto :fail
if not exist "%ROOT%..\..\build\" mkdir "%ROOT%..\..\build\" || goto :fail

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

"%CLANG_CL%" /nologo /c /O2 /Oi- /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc "%ROOT%source\runtime.c" /Fo"%OUT%\runtime.obj" || goto :fail

"%CLANG_CL%" /nologo /c /O2 /Oi- /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc "%ROOT%source\widescreen.c" /Fo"%OUT%\widescreen.obj" || goto :fail

"%CLANG_CL%" /nologo /c /O2 /Oi- /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc "%ROOT%source\plugin.c" /Fo"%OUT%\plugin.obj" || goto :fail

"%LLD_LINK%" /nologo /dll /nodefaultlib /machine:x86 /entry:DllMain@12 ^
    "%OUT%\runtime.obj" "%OUT%\widescreen.obj" "%OUT%\plugin.obj" ^
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
