#ifndef CASTLE_ENTRY_GATE_H
#define CASTLE_ENTRY_GATE_H

#include "platform.h"

/*
 * Entry Gate 是“早加载 Core”和“不要在 DllMain 里做复杂 Mod 初始化”之间的桥梁。
 *
 * CastleModCore.dll 会作为 RPG.exe 的启动依赖非常早进入进程，此时 Windows 仍持有 Loader Lock。
 * 在这个时点直接加载十几个 ASI、扫描大量文件并不安全，所以 DllMain 只安装一个一次性入口门。
 * 等 Windows 完成全部静态 DLL 初始化、准备真正执行 RPG.exe EntryPoint 时，入口门才调用完整初始化。
 */
int EntryGate_Install(void);
int EntryGate_Restore(void);
void EntryGate_FinalizeInjectedImportPage(void);

#endif
