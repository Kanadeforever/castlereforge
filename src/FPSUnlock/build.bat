@echo off
setlocal EnableExtensions

rem ============================================================================  
rem build.bat  
rem Castle_FPSUnlock v1.3 可重现的 Windows 构建脚本。  
rem  
rem 此批处理文件特意仅使用 ASCII 可执行文本和注释。  
rem 旧版 cmd.exe 在 CHCP 生效之前可能会错误解析 UTF-8 多字节文本。  
rem 保持批处理文件本身为纯ASCII可避免简体 / 繁体 Windows 上整个这一类构建失败。  
rem 
rem C++ 源代码采用 UTF-8 编码，包含详细的中文注释。  
rem /utf-8 告诉 clang-cl 正确读取它。  
rem 未链接 Visual C++ 运行时：源代码提供了  
rem 所需的微型 memcpy/memset/strlen 辅助函数，  
rem Win32 API 在运行时通过游戏现有的 IAT 进行解析。  
rem ============================================================================  

chcp 65001 >nul
@echo off

rem 统一输出目录：仓库根 build\（基于本脚本位置定位，与运行目录无关）。  
set "ROOT=%~dp0"
set "OUT=%ROOT%..\..\build"
set "OBJ_DIR=%ROOT%_build"
set "SDK=%ROOT%..\RuntimeSDK"

rem 仅删除已知的临时对象/库文件，以免过期的输出被误认为成功的新构建。  
if exist "%OBJ_DIR%" rmdir /s /q "%OBJ_DIR%"
mkdir "%OBJ_DIR%"
if not exist "%OUT%" mkdir "%OUT%"
del /q "%OUT%\Castle_FPSUnlock.asi" 2>nul
del /q "%OUT%\Castle_FPSUnlock.lib" 2>nul
del /q "%OUT%\Castle_FPSUnlock.exp" 2>nul

rem 验证可重现构建所使用的两个 LLVM/MSVC 兼容工具。   
rem 用户正常的 Visual Studio/LLVM 环境可能已将它们置于 PATH 中。   
rem 如果没有，请从开发者命令提示符启动此 BAT，或将 LLVM 添加到 PATH。   
rem 我们会立即停止，而不是静默生成旧二进制文件。   
where clang-cl >nul 2>nul
if errorlevel 1 (
    echo [错误] 在 PATH 中未找到 clang-cl。  
    pause
    exit /b 1
)
where lld-link >nul 2>nul
if errorlevel 1 (
    echo [错误] 在 PATH 中未找到 lld-link。  
    pause
    exit /b 1
)

rem 编译一个32位Windows目标文件。   
rem --target=i686-pc-windows-msvc : 强制32位x86，与RPG.exe匹配。   
rem /O2                           : 优化常规运行时代码。   
rem /GS-                          : 避免CRT安全cookie依赖。   
rem /GR-                          : 禁用C++ RTTI，此功能未使用。   
rem /Oi-                          : 避免编译器替换，可能增加   
rem                                 意外的运行时辅助函数。   
rem /utf-8                        : 源代码/注释为UTF-8。   
rem /W4 /WX                       : 高警告级别，并将每个警告视为   
rem                                 错误，以便可疑代码无法通过构建。   
clang-cl --target=i686-pc-windows-msvc /nologo /c /TP /O2 /GS- /GR- /Oi- /utf-8 /W4 /WX /I"%SDK%\include" /I"%SDK%\client" /Fo"%OBJ_DIR%\Castle_FPSUnlock.obj" "%ROOT%source\Castle_FPSUnlock.cpp" || goto :fail
clang-cl --target=i686-pc-windows-msvc /nologo /c /TC /O2 /GS- /Oi- /utf-8 /W4 /WX /I"%SDK%\include" /I"%SDK%\client" /Fo"%OBJ_DIR%\runtime_client.obj" "%SDK%\client\runtime_client.c" || goto :fail
clang-cl --target=i686-pc-windows-msvc /nologo /c /TC /O2 /GS- /Oi- /utf-8 /W4 /WX /I"%SDK%\include" /I"%SDK%\client" /Fo"%OBJ_DIR%\runtime_entry_gate.obj" "%SDK%\client\runtime_entry_gate.c" || goto :fail
clang-cl --target=i686-pc-windows-msvc /nologo /c /TC /O2 /GS- /Oi- /utf-8 /W4 /WX /I"%SDK%\include" /Fo"%OBJ_DIR%\runtime_client_support.obj" "%SDK%\client\runtime_client_support.c" || goto :fail

rem 链接真实的 x86 DLL，但使用 ASI 加载器所期望的 .asi 扩展名。   
rem /nodefaultlib 使二进制文件独立于 MSVCRT/UCRT。   
rem /entry:DllMain@12 直接指向 stdcall 32 位 DLL 入口函数。   
rem /dynamicbase + /nxcompat 在旧游戏加载器允许的情况下保留现代 ASLR/NX 安全性。   
rem /timestamp:0 使 PE 头具有确定性，因此从相同源码进行的两次   
rem 干净构建可以逐字节进行比较。   
lld-link /dll /machine:x86 /nodefaultlib /entry:DllMain@12 /dynamicbase /nxcompat /timestamp:0 /def:"%ROOT%source\Castle_FPSUnlock.def" /out:"%OUT%\Castle_FPSUnlock.asi" /implib:"%OBJ_DIR%\Castle_FPSUnlock.lib" "%OBJ_DIR%\Castle_FPSUnlock.obj" "%OBJ_DIR%\runtime_client.obj" "%OBJ_DIR%\runtime_entry_gate.obj" "%OBJ_DIR%\runtime_client_support.obj" kernel32.lib || goto :fail

rem 清理中间对象目录；lib 便于高级调试但实际游戏只需要  
rem Castle_FPSUnlock.asi 和 Castle_FPSUnlock.ini。  
rmdir /s /q "%OBJ_DIR%" 2>nul

rem 将运行时 INI（templete 模板）复制到 ASI 旁边，输出统一到仓库根 build\。  
copy /y "%ROOT%templete\Castle_FPSUnlock.toml" "%OUT%\Castle_FPSUnlock.toml" >nul || goto :fail

echo [成功] build\Castle_FPSUnlock.asi 和 Castle_FPSUnlock.toml 已成功构建。  
pause
exit /b 0

:fail
rmdir /s /q "%OBJ_DIR%" 2>nul
echo [错误] 构建失败，已停止；不会把半套产物报告为成功。  
pause
exit /b 1
