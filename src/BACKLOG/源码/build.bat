@echo off
setlocal DisableDelayedExpansion
"%SystemRoot%\System32\chcp.com" 65001 >nul

rem This script builds the 32-bit ASI from a clean object directory.
rem It intentionally uses only ASCII source text because cmd.exe on some Chinese Windows
rem versions can mis-parse UTF-8 multibyte characters before CHCP takes full effect.
rem All human documentation is kept in the sibling docs directory instead.

rem Locate a Visual Studio developer environment. The project is x86 even on x64 Windows.
set "VSDEV="
if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
if not defined VSDEV if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"

if not defined VSDEV (
  echo [ERROR] Visual Studio developer environment was not found.
  pause
  exit /b 1
)

rem Keep the system tools in PATH before entering VsDevCmd. Some stripped terminals do not
rem inherit System32, while VsDevCmd itself needs tools such as findstr.exe.
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
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
  echo [ERROR] Visual Studio x86 environment initialization failed.
  pause
  exit /b 1
)

where clang-cl >nul 2>nul || goto :tool_fail
where link >nul 2>nul || goto :tool_fail

rem release is recreated logically from the current source. _build contains temporary .obj
rem files only and is always removed on both success and failure.
if not exist "..\release" mkdir "..\release"
if not exist "..\release\docs" mkdir "..\release\docs"
if not exist "..\release\tools" mkdir "..\release\tools"
if exist "_build" rmdir /s /q "_build"
mkdir "_build"

rem /nodefaultlib keeps the ASI independent of the CRT. kernel32/user32 are the only Win32
rem import libraries needed by runtime, mouse window subclassing, and the optional PadSupport bridge.
set "CFLAGS=/nologo /c /O2 /GS- /Zl /W4 /WX /utf-8 /TC --target=i686-pc-windows-msvc -fno-builtin -Wno-cast-function-type-mismatch"

call :compile runtime.c runtime.obj || goto :fail
call :compile mouse_input.c mouse_input.obj || goto :fail
call :compile pad_bridge.c pad_bridge.obj || goto :fail
call :compile backlog.c backlog.obj || goto :fail
call :compile plugin.c plugin.obj || goto :fail
call :compile name_panel_pool.c name_panel_pool.obj || goto :fail

echo [LINK] Castle_Backlog.asi
link /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 ^
  "_build\runtime.obj" "_build\mouse_input.obj" "_build\pad_bridge.obj" ^
  "_build\backlog.obj" "_build\plugin.obj" "_build\name_panel_pool.obj" ^
  kernel32.lib user32.lib /out:"..\release\Castle_Backlog.asi"
if errorlevel 1 goto :fail

rem The runtime INI must sit beside the ASI. All Markdown documents use Chinese filenames and
rem are copied into the compiled release so that the binary package can be resumed independently.
copy /y "Castle_Backlog.ini" "..\release\Castle_Backlog.ini" >nul || goto :fail

copy /y "..\文档\*.md" "..\release\docs\" >nul || goto :fail
copy /y "..\工具\backlog_check.py" "..\release\tools\backlog_check.py" >nul || goto :fail

rem The linker may emit import-library side products next to the ASI; they are not runtime files.
del /q "..\release\Castle_Backlog.lib" 2>nul
del /q "..\release\Castle_Backlog.exp" 2>nul
rmdir /s /q "_build"

echo [DONE] ASI, INI, Chinese documents, and the latest checker were written to ..\release
pause
exit /b 0

:compile
rem Compile one C file. %1 is the source filename and %2 is the temporary object filename.
echo [COMPILE] %1
clang-cl %CFLAGS% "%1" /Fo:"_build\%2"
if errorlevel 1 exit /b 1
exit /b 0

:tool_fail
echo [ERROR] clang-cl.exe or link.exe was not found after VsDevCmd.
pause
exit /b 1

:fail
echo [ERROR] Build stopped. No partial result is reported as a successful release.
if exist "_build" rmdir /s /q "_build"
pause
exit /b 1
