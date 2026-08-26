#ifndef CASTLE_PAD_INTERFACE_OPTIONS_H
#define CASTLE_PAD_INTERFACE_OPTIONS_H

#include "cursor.h"

/*
 * interface_options.h
 *
 * 这是主 Interface state8“机能”页面的公开接口。
 * 这个模块只做“把手柄动作交给原版机能页面”这一层工作，不负责真正修改游戏设置。
 *
 * 可以把它理解成一个翻译员：
 *   手柄 ↑/↓       -> 选择音乐 / 音效 / 空明流转三行之一；
 *   手柄 LT/RT     -> 点击原版音乐或音效的减/加 Button；
 *   手柄 A          -> 只在空明流转行点击原版 Button；
 *   原版 Yes/No     -> 继续交给 confirm_dialog.c；
 *   真正的音量数值 -> 始终由 RPG.exe 自己修改。
 *
 * 为什么头文件里也要把边界写清楚：
 * 其它模块只应该通过下面这些函数和机能页交流。如果以后有人想在 Shell、Cursor 或 InputRouter
 * 里直接改“音乐值”，看到这里就能立刻知道那属于越权，应该回到本页面 Adapter 或原版事件链处理。
 */

/*
 * 安装 state8 所需的五个原版 ButtonEvent Hook。
 * 返回非 0 表示安装流程可以继续；如果目标 EXE 的 state8 协议不匹配，模块会 fail-closed，
 * 只禁用机能页手柄 Adapter，不应该连带关闭已经实机 PASS 的其它页面。
 */
int InterfaceOptions_InstallHooks(void);

/*
 * 每个控制器 worker tick 调用一次。
 * 函数会先确认当前真的在 state8，再按照 InputRouter 的语义处理三行焦点、LT/RT 和 A。
 * 它不会直接读取 SDL 数字键，也不会直接写音量字段。
 */
void InterfaceOptions_Update(void);

/*
 * 当实体鼠标或右摇杆精细鼠标取得指针所有权时调用。
 * 机能页会立即停止强制显示自己的手形焦点，避免“玩家想用鼠标，插件却每帧把鼠标抢回去”。
 */
void InterfaceOptions_OnPointerTakeover(CursorTakeoverEvent event);

/*
 * 只回答“原版 state8 页面现在是否真的活动”。
 * Shell/调度层可以用这个布尔结果判断页面存在性，但不能借此直接操作页面内部 Button。
 */
int InterfaceOptions_Active(void);

#endif /* CASTLE_PAD_INTERFACE_OPTIONS_H */
