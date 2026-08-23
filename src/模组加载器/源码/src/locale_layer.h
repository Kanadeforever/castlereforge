#ifndef CASTLE_LOCALE_LAYER_H
#define CASTLE_LOCALE_LAYER_H

#include "platform.h"

/* 固定启用《幽城》台湾繁中运行环境。成功返回 1。 */
int LocaleLayer_Initialize(void);

/*
 * 在 Loader Lock 已释放、但 RPG.exe 还没有执行第一条业务指令时，做一次真正的文件名级 CP950 自检。
 * 这个测试不是看 GetACP 返回值，而是实际用 Big5 原始字节去打开一个由 CreateFileW 创建的繁体 Unicode 文件。
 * 成功返回 1；失败返回 0，Core 会拒绝让游戏在“看起来转区、实际文件名仍打不开”的状态继续运行。
 */
int LocaleLayer_RunFileNameSelfTest(void);

/* 新加载 ASI 的普通运行期也要看到同一 Locale API 语义。 */
UINT LocaleLayer_PatchModule(HMODULE module);

#endif
