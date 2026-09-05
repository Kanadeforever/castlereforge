@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
"%SystemRoot%\System32\chcp.com" 65001 >nul
cd /d "%~dp0"

set "ROOT=%~dp0"
set "OUT=%ROOT%_build"
set "BUILD_ROOT=%ROOT%..\..\build"
set "INCLUDE_DIR=%ROOT%include"
set "TEST_FILE=%ROOT%tests\abi_layout_test.c"
set "HOST_TEST=%ROOT%tests\runtime_host_test.c"
set "HOST_TEST_DEF=%ROOT%tests\runtime_host_test.def"
set "BOOTSTRAP_PLUGIN_TEST=%ROOT%tests\bootstrap_plugin_test.c"
set "BOOTSTRAP_PLUGIN_DEF=%ROOT%tests\bootstrap_plugin_test.def"
set "ENTRY_GATE_TEST=%ROOT%tests\entry_gate_test.c"
set "ENTRY_GATE_TEST_DEF=%ROOT%tests\entry_gate_test.def"
set "CLIENT_STATE_TEST=%ROOT%tests\client_state_test.c"
set "CLIENT_BOOTSTRAP_TEST=%ROOT%tests\client_bootstrap_test.c"
set "CLIENT_BOOTSTRAP_TEST_DEF=%ROOT%tests\client_bootstrap_test.def"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="
if exist "%VSWHERE%" for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
if defined VSINSTALL (
    call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -no_logo -arch=x86 -host_arch=x64 >nul
    if errorlevel 1 goto :tool_fail
)
where cl.exe >nul 2>nul || goto :tool_fail
where link.exe >nul 2>nul || goto :tool_fail

set "CLANG_CL="
for /f "delims=" %%I in ('where clang-cl.exe 2^>nul') do if not defined CLANG_CL set "CLANG_CL=%%I"
if not defined CLANG_CL goto :tool_fail

set "PYTHON_EXE="
set "PYTHON_ARGS="
for /f "delims=" %%I in ('where python.exe 2^>nul') do if not defined PYTHON_EXE set "PYTHON_EXE=%%I"
if not defined PYTHON_EXE (
    for /f "delims=" %%I in ('where py.exe 2^>nul') do if not defined PYTHON_EXE set "PYTHON_EXE=%%I"
    if defined PYTHON_EXE set "PYTHON_ARGS=-3"
)
if not defined PYTHON_EXE goto :tool_fail
if not exist "%TEST_FILE%" goto :file_fail
if not exist "%HOST_TEST%" goto :file_fail
if not exist "%HOST_TEST_DEF%" goto :file_fail
if not exist "%BOOTSTRAP_PLUGIN_TEST%" goto :file_fail
if not exist "%BOOTSTRAP_PLUGIN_DEF%" goto :file_fail
if not exist "%ENTRY_GATE_TEST%" goto :file_fail
if not exist "%ENTRY_GATE_TEST_DEF%" goto :file_fail
if not exist "%CLIENT_STATE_TEST%" goto :file_fail
if not exist "%CLIENT_BOOTSTRAP_TEST%" goto :file_fail
if not exist "%CLIENT_BOOTSTRAP_TEST_DEF%" goto :file_fail

if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%" || goto :fail

echo [1/10] MSVC x86 C布局检查...  
cl.exe /nologo /c /TC /utf-8 /W4 /WX /Zl /I"%INCLUDE_DIR%" /Fo"%OUT%\abi_msvc_c.obj" "%TEST_FILE%"
if errorlevel 1 goto :fail

echo [2/10] MSVC x86 C++17布局检查...  
cl.exe /nologo /c /TP /std:c++17 /utf-8 /W4 /WX /Zl /I"%INCLUDE_DIR%" /Fo"%OUT%\abi_msvc_cpp.obj" "%TEST_FILE%"
if errorlevel 1 goto :fail

echo [3/10] Clang x86 C布局检查...  
"%CLANG_CL%" /nologo /c /TC /W4 /WX /Zl /clang:--target=i686-pc-windows-msvc /I"%INCLUDE_DIR%" /Fo"%OUT%\abi_clang_c.obj" "%TEST_FILE%"
if errorlevel 1 goto :fail

echo [4/10] Clang x86 C++17布局检查...  
"%CLANG_CL%" /nologo /c /TP /std:c++17 /W4 /WX /Zl /clang:--target=i686-pc-windows-msvc /I"%INCLUDE_DIR%" /Fo"%OUT%\abi_clang_cpp.obj" "%TEST_FILE%"
if errorlevel 1 goto :fail

echo [5/10] 编译Castle_Runtime核心对象...  
set "RUNTIME_CFLAGS=/nologo /c /TC /O2 /Oi- /utf-8 /W4 /WX /GS- /Gs999999999 /Zl /I%INCLUDE_DIR% /I%ROOT%source"
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_entry.obj" "%ROOT%source\runtime_entry.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_support.obj" "%ROOT%source\runtime_support.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_path.obj" "%ROOT%source\runtime_path.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_log.obj" "%ROOT%source\runtime_log.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_clock.obj" "%ROOT%source\runtime_clock.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_input.obj" "%ROOT%source\runtime_input.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_game_state.obj" "%ROOT%source\runtime_game_state.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_save.obj" "%ROOT%source\runtime_save.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_overlay.obj" "%ROOT%source\runtime_overlay.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_file.obj" "%ROOT%source\runtime_file.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_module.obj" "%ROOT%source\runtime_module.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_toml.obj" "%ROOT%source\runtime_toml.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_schedule.obj" "%ROOT%source\runtime_schedule.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_symbols.obj" "%ROOT%source\runtime_symbols.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_display.obj" "%ROOT%source\runtime_display.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_window.obj" "%ROOT%source\runtime_window.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_render.obj" "%ROOT%source\runtime_render.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_memory.obj" "%ROOT%source\runtime_memory.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_diagnostics.obj" "%ROOT%source\runtime_diagnostics.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_registry.obj" "%ROOT%source\runtime_registry.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_bootstrap.obj" "%ROOT%source\runtime_bootstrap.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_hook.obj" "%ROOT%source\runtime_hook.c"
if errorlevel 1 goto :fail
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_api.obj" "%ROOT%source\runtime_api.c"
if errorlevel 1 goto :fail
rem 不同 MSVC 版本可能把结构赋值折叠成 memcpy；无 CRT DLL 必须显式提供本地实现。  
cl.exe %RUNTIME_CFLAGS% /Fo"%OUT%\runtime_crt_support.obj" "%ROOT%client\runtime_client_support.c"
if errorlevel 1 goto :fail

echo [6/10] 链接PE32无CRT Castle_Runtime.dll...  
link.exe /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 /subsystem:windows /dynamicbase /nxcompat /def:"%ROOT%source\CastleRuntime.def" /out:"%OUT%\Castle_Runtime.dll" "%OUT%\runtime_entry.obj" "%OUT%\runtime_support.obj" "%OUT%\runtime_path.obj" "%OUT%\runtime_log.obj" "%OUT%\runtime_clock.obj" "%OUT%\runtime_input.obj" "%OUT%\runtime_game_state.obj" "%OUT%\runtime_save.obj" "%OUT%\runtime_overlay.obj" "%OUT%\runtime_file.obj" "%OUT%\runtime_module.obj" "%OUT%\runtime_toml.obj" "%OUT%\runtime_schedule.obj" "%OUT%\runtime_symbols.obj" "%OUT%\runtime_display.obj" "%OUT%\runtime_window.obj" "%OUT%\runtime_render.obj" "%OUT%\runtime_memory.obj" "%OUT%\runtime_diagnostics.obj" "%OUT%\runtime_registry.obj" "%OUT%\runtime_bootstrap.obj" "%OUT%\runtime_hook.obj" "%OUT%\runtime_api.obj" "%OUT%\runtime_crt_support.obj" kernel32.lib
if errorlevel 1 goto :fail

echo [7/10] 编译Client、Entry Gate和无CRT x86测试宿主...  
set "CLIENT_CFLAGS=/nologo /c /TC /O2 /Oi- /utf-8 /W4 /WX /GS- /Gs999999999 /Zl /I%INCLUDE_DIR% /I%ROOT%client"
cl.exe %CLIENT_CFLAGS% /Fo"%OUT%\runtime_client.obj" "%ROOT%client\runtime_client.c"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% /Fo"%OUT%\runtime_entry_gate.obj" "%ROOT%client\runtime_entry_gate.c"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% /Fo"%OUT%\entry_gate_test.obj" "%ENTRY_GATE_TEST%"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% /Fo"%OUT%\client_state_test.obj" "%CLIENT_STATE_TEST%"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% /Fo"%OUT%\client_bootstrap_test.obj" "%CLIENT_BOOTSTRAP_TEST%"
if errorlevel 1 goto :fail
cl.exe /nologo /c /TC /O2 /Oi- /utf-8 /W4 /WX /GS- /Gs999999999 /Zl /I"%INCLUDE_DIR%" /Fo"%OUT%\runtime_host_test.obj" "%HOST_TEST%"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% /DTEST_PLUGIN_A /Fo"%OUT%\bootstrap_plugin_a.obj" "%BOOTSTRAP_PLUGIN_TEST%"
if errorlevel 1 goto :fail
cl.exe %CLIENT_CFLAGS% /DTEST_PLUGIN_B /Fo"%OUT%\bootstrap_plugin_b.obj" "%BOOTSTRAP_PLUGIN_TEST%"
if errorlevel 1 goto :fail
link.exe /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 /subsystem:windows /dynamicbase /nxcompat /def:"%BOOTSTRAP_PLUGIN_DEF%" /out:"%OUT%\runtime_bootstrap_a.asi" "%OUT%\bootstrap_plugin_a.obj" kernel32.lib
if errorlevel 1 goto :fail
link.exe /nologo /Brepro /dll /nodefaultlib /machine:x86 /entry:DllMain@12 /subsystem:windows /dynamicbase /nxcompat /def:"%BOOTSTRAP_PLUGIN_DEF%" /out:"%OUT%\runtime_bootstrap_b.asi" "%OUT%\bootstrap_plugin_b.obj" kernel32.lib
if errorlevel 1 goto :fail
link.exe /nologo /Brepro /nodefaultlib /machine:x86 /entry:EntryGateTestEntry@0 /subsystem:console /dynamicbase /nxcompat /def:"%ENTRY_GATE_TEST_DEF%" /out:"%OUT%\entry_gate_test.exe" "%OUT%\entry_gate_test.obj" "%OUT%\runtime_entry_gate.obj" kernel32.lib
if errorlevel 1 goto :fail
link.exe /nologo /Brepro /nodefaultlib /machine:x86 /entry:ClientStateTestEntry@0 /subsystem:console /dynamicbase /nxcompat /out:"%OUT%\client_state_test.exe" "%OUT%\client_state_test.obj" "%OUT%\runtime_client.obj" "%OUT%\runtime_entry_gate.obj" kernel32.lib
if errorlevel 1 goto :fail
link.exe /nologo /Brepro /nodefaultlib /machine:x86 /entry:ClientBootstrapTestEntry@0 /subsystem:console /dynamicbase /nxcompat /def:"%CLIENT_BOOTSTRAP_TEST_DEF%" /out:"%OUT%\client_bootstrap_test.exe" "%OUT%\client_bootstrap_test.obj" "%OUT%\runtime_client.obj" "%OUT%\runtime_entry_gate.obj" kernel32.lib
if errorlevel 1 goto :fail
mkdir "%OUT%\client_integrated" || goto :fail
mkdir "%OUT%\client_standalone" || goto :fail
mkdir "%OUT%\client_fault" || goto :fail
copy /y "%OUT%\client_bootstrap_test.exe" "%OUT%\client_integrated\client_bootstrap_test.exe" >nul
if errorlevel 1 goto :fail
copy /y "%OUT%\client_bootstrap_test.exe" "%OUT%\client_standalone\client_bootstrap_test.exe" >nul
if errorlevel 1 goto :fail
copy /y "%OUT%\client_bootstrap_test.exe" "%OUT%\client_fault\client_bootstrap_test.exe" >nul
if errorlevel 1 goto :fail
copy /y "%OUT%\Castle_Runtime.dll" "%OUT%\client_integrated\Castle_Runtime.dll" >nul
if errorlevel 1 goto :fail
copy /y "%CLIENT_BOOTSTRAP_TEST%" "%OUT%\client_fault\Castle_Runtime.dll" >nul
if errorlevel 1 goto :fail

echo [8/10] 链接并运行Runtime根API测试...  
link.exe /nologo /Brepro /nodefaultlib /machine:x86 /entry:TestEntry@0 /subsystem:console /dynamicbase /nxcompat /def:"%HOST_TEST_DEF%" /out:"%OUT%\runtime_host_test.exe" "%OUT%\runtime_host_test.obj" kernel32.lib user32.lib
if errorlevel 1 goto :fail
"%OUT%\runtime_host_test.exe"
if errorlevel 1 goto :host_fail
"%OUT%\entry_gate_test.exe"
if errorlevel 1 goto :entry_gate_fail
"%OUT%\client_state_test.exe"
if errorlevel 1 goto :client_state_fail
"%OUT%\client_integrated\client_bootstrap_test.exe"
if errorlevel 1 goto :client_integrated_fail
"%OUT%\client_standalone\client_bootstrap_test.exe"
if errorlevel 1 goto :client_standalone_fail
"%OUT%\client_fault\client_bootstrap_test.exe"
if errorlevel 1 goto :client_fault_fail

echo [9/10] RuntimeSDK文本与PE合同检查...  
"%PYTHON_EXE%" %PYTHON_ARGS% "%ROOT%tools\runtime_sdk_check.py" --require-dll
if errorlevel 1 goto :fail

echo [10/10] 复制Castle_Runtime.dll到仓库build目录...  
if not exist "%BUILD_ROOT%" mkdir "%BUILD_ROOT%"
if errorlevel 1 goto :fail
copy /y "%OUT%\Castle_Runtime.dll" "%BUILD_ROOT%\Castle_Runtime.dll" >nul
if errorlevel 1 goto :fail

if exist "%OUT%" rmdir /s /q "%OUT%"
echo [成功] RuntimeSDK核心ABI、最小DLL、测试宿主和PE合同全部通过。  
echo [产物] build\Castle_Runtime.dll  
exit /b 0

:tool_fail
if exist "%OUT%" rmdir /s /q "%OUT%"
echo [失败] 找不到 MSVC x86、clang-cl 或 Python。请先安装构建依赖并加入 PATH。  
exit /b 1

:file_fail
if exist "%OUT%" rmdir /s /q "%OUT%"
echo [失败] RuntimeSDK ABI测试文件缺失。  
exit /b 1

:host_fail
echo [失败] Runtime测试宿主退出码=%errorlevel%。  
if exist "%OUT%" rmdir /s /q "%OUT%"
exit /b 1

:entry_gate_fail
echo [失败] Entry Gate测试退出码=%errorlevel%。  
if exist "%OUT%" rmdir /s /q "%OUT%"
exit /b 1

:client_state_fail
echo [失败] Client状态机测试退出码=%errorlevel%。  
if exist "%OUT%" rmdir /s /q "%OUT%"
exit /b 1

:client_integrated_fail
echo [失败] Client正常Runtime整合测试退出码=%errorlevel%。  
if exist "%OUT%" rmdir /s /q "%OUT%"
exit /b 1

:client_standalone_fail
echo [失败] Client无Runtime独立测试退出码=%errorlevel%。  
if exist "%OUT%" rmdir /s /q "%OUT%"
exit /b 1

:client_fault_fail
echo [失败] Client损坏Runtime故障安全测试退出码=%errorlevel%。  
if exist "%OUT%" rmdir /s /q "%OUT%"
exit /b 1

:fail
if exist "%OUT%" rmdir /s /q "%OUT%"
echo [失败] RuntimeSDK第一里程碑ABI检查未通过。  
exit /b 1
