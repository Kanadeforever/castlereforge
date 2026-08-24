@echo off
setlocal DisableDelayedExpansion
"%SystemRoot%\System32\chcp.com" 65001 >nul

rem ---------------------------------------------------------------------------
rem Castle Backlog x86 构建脚本。
rem 只在本源码目录生成临时 _build，并把最终包写到 ..\编译内容。
rem ---------------------------------------------------------------------------

set "VSDEV="
if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"

if not defined VSDEV (
  echo [错误] 没有找到 Visual Studio x86 开发环境。
  exit /b 1
)

rem Codex/精简终端的 PATH 可能没有 System32；VsDevCmd 自己会调用 findstr 等系统工具。
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;C:\Users\luminous\scoop\apps\llvm\current\bin"
set "INCLUDE="
set "LIB="
set "LIBPATH="
set "__VSCMD_PREINIT_PATH="
set "VSCMD_ARG_TGT_ARCH="
set "VSCMD_ARG_HOST_ARCH="
set "VSCMD_VER="
set "VSINSTALLDIR="
set "VCINSTALLDIR="
set "VisualStudioVersion="

call "%VSDEV%" -no_logo -arch=x86 -host_arch=x64 >nul
if errorlevel 1 (
  echo [错误] Visual Studio x86 环境初始化失败。
  exit /b 1
)

where clang-cl >nul 2>nul || goto :tool_fail
where link >nul 2>nul || goto :tool_fail

if not exist "..\编译内容" mkdir "..\编译内容"
if not exist "..\编译内容\文档" mkdir "..\编译内容\文档"
if not exist "..\编译内容\工具" mkdir "..\编译内容\工具"
if not exist "..\文档" mkdir "..\文档"

rem _build 只属于当前源码目录；先删旧临时对象，防止上一轮残留混进新 ASI。
if exist "_build" rmdir /s /q "_build"
mkdir "_build"

rem /nodefaultlib 明确禁止 CRT；kernel32/user32 只提供稳定 Win32 API import。
set "CFLAGS=/nologo /c /O2 /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc -fno-builtin -Wno-cast-function-type-mismatch"

call :compile runtime.c runtime.obj || goto :fail
call :compile sdl_input.c sdl_input.obj || goto :fail
call :compile backlog.c backlog.obj || goto :fail
call :compile plugin.c plugin.obj || goto :fail

echo [链接] Castle_Backlog.asi
link /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 ^
  "_build\runtime.obj" "_build\sdl_input.obj" "_build\backlog.obj" "_build\plugin.obj" ^
  kernel32.lib user32.lib /out:"..\编译内容\Castle_Backlog.asi"
if errorlevel 1 goto :fail

rem ASI、默认 INI、全部中文文档和最新检查器共同组成一个可独立接档的编译内容包。
copy /y "Castle_Backlog.ini" "..\编译内容\Castle_Backlog.ini" >nul || goto :fail
rem 先清理两个明确文档镜像里的旧 Markdown，避免版本改名后旧接档与新接档同时残留。
del /q "..\文档\*.md" 2>nul
del /q "..\编译内容\文档\*.md" 2>nul
copy /y "..\..\..\docs\backlog\*.md" "..\文档\" >nul || goto :fail
copy /y "..\..\..\docs\backlog\*.md" "..\编译内容\文档\" >nul || goto :fail
copy /y "..\工具\backlog_check.py" "..\编译内容\工具\backlog_check.py" >nul || goto :fail
copy /y "..\..\..\docs\backlog\工具详细说明.md" "..\编译内容\工具\工具详细说明.md" >nul || goto :fail

del /q "..\编译内容\Castle_Backlog.lib" 2>nul
del /q "..\编译内容\Castle_Backlog.exp" 2>nul
rmdir /s /q "_build"

echo [完成] ASI + INI + 全部中文文档 + 检查器已同步。
exit /b 0

:compile
echo [编译] %1
clang-cl %CFLAGS% "%1" /Fo:"_build\%2"
if errorlevel 1 exit /b 1
exit /b 0

:tool_fail
echo [错误] 缺少 clang-cl 或 link。
exit /b 1

:fail
echo [失败] 构建或打包中止；不会把半套产物报告为成功。
if exist "_build" rmdir /s /q "_build"
exit /b 1
