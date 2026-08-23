#ifndef CASTLE_PE_IMPORT_INJECTOR_H
#define CASTLE_PE_IMPORT_INJECTOR_H

#include "platform.h"

/*
 * 一次把两个 Loader 内部 DLL 放到 RPG.exe 的临时内存 Import Directory 最前面。
 *
 * dev9 的发布布局仍把这两个 DLL 放在 mods\ 根目录，但为了精确恢复 dev5 的启动装载语义，调用方传入纯文件名：
 *   1. CastleLocaleBootstrap.dll —— ntdll-only 的最早期 CP950 NLS 层；
 *   2. CastleModCore.dll —— KERNEL32 初始化后接管 Overrides/Win32 Locale/ASI。
 *
 * Launcher 在 CreateProcessW 以前临时 SetDllDirectoryW(mods)，因此目标进程的 Windows Loader 能按 dev5 方式找到它们。
 * Injector 的职责仍然只是把调用方给出的 ASCII 字节串原样写入 IMAGE_IMPORT_DESCRIPTOR.Name；它自己不修改搜索目录。
 *
 * 必须一次构造，不能连续调用“加一个 import”的旧函数：两次调用会争用同一段 .rdata padding。
 */
int PeImportInjector_AddEarlyImports2(HANDLE process,
    const char* first_dll, const char* first_import,
    const char* second_dll, const char* second_import);

#endif
