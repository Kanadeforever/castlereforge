#ifndef CASTLE_USER32_LOCALE_H
#define CASTLE_USER32_LOCALE_H

#include "platform.h"

/*
 * USER32 Locale 子层。
 *
 * Locale/NLS 已经变成 CP950 后，旧 ANSI 游戏仍可能经过 USER32 自己的 A/W 文本入口。
 * 如果这些入口在系统原语言环境下已经形成了缓存，仅仅让 GetACP() 返回 950 仍可能不够。
 * 因此本模块对《幽城》实际使用、且携带 ANSI 文本的几个入口做“Big5 -> UTF-16 -> W API”桥接。
 *
 * 当前只处理用户指定台湾繁中环境直接需要的路径：
 *   - CreateWindowExA：窗口类名/标题；
 *   - MessageBoxA：错误框/提示框文字；
 *   - DefWindowProcA 的 WM_SETTEXT：运行中更新窗口标题。
 *
 * 不相关的鼠标、键盘、绘图、剪贴板、列表框等 USER32 行为完全不碰。
 */

/*
 * 第一次调用时解析 USER32 A/W 函数并扫描当前模块；若 USER32 尚未加载，安全返回成功等待第二阶段。
 * EntryPoint 前会再次调用，因此“早期暂时没有 USER32”不是错误。
 */
int User32Locale_Initialize(void);

/*
 * 扫描一个 PE32 模块的导入表，只改写 USER32.dll 上面三个文本入口。
 * 返回本次真正改写的槽位数量；函数是幂等的，已经指向 Hook 的槽不会重复写。
 */
UINT User32Locale_PatchModule(HMODULE module);

#endif
