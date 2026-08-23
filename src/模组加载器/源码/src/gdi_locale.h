#ifndef CASTLE_GDI_LOCALE_H
#define CASTLE_GDI_LOCALE_H

#include "platform.h"

/*
 * GDI Locale 子层的公开接口。
 *
 * 为什么单独放一个模块：
 *   台湾 Big5 环境不只意味着“文字按 CP950 解码”，还意味着程序在没有明确指定字体字符集时，
 *   Windows 应该优先使用 CHINESEBIG5_CHARSET(136)。Locale Emulator 会在字体创建路径里处理这个语义。
 *
 * 本工程只实现用户当前设置真正需要的这一点：
 *   - ANSI_CHARSET(0)  -> CHINESEBIG5_CHARSET(136)
 *   - DEFAULT_CHARSET(1)-> CHINESEBIG5_CHARSET(136)
 *   - 其它应用明确指定的字符集完全保留，不擅自修改。
 *
 * 这两个函数都允许重复调用。原因是 Core 很早进入进程时，GDI32 可能还没有映射；
 * 等 Windows 完成静态 DLL 初始化、即将进入 RPG.exe EntryPoint 前，Core 会再调用一次进行补挂。
 */

/* 解析 GDI32 的标准字体创建函数，并给当前已经加载的相关模块补 IAT Hook。 */
int GdiLocale_Initialize(void);

/*
 * 只检查传入的 PE32 模块是否导入 GDI32 的 CreateFont*；若有则把对应 IAT 槽改到本模块。
 * 返回值是本次真正改写的 IAT 槽数量，0 既可能表示“没有相关导入”，也可能表示“已经是我们的 Hook”。
 */
UINT GdiLocale_PatchModule(HMODULE module);

#endif
