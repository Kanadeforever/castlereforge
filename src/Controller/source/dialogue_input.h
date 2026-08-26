#ifndef CASTLE_PAD_DIALOGUE_INPUT_H
#define CASTLE_PAD_DIALOGUE_INPUT_H

/*
 * dialogue_input.h
 *
 * 本模块只负责“普通文字消息里的 A 确定键”。
 *
 * refactor10 已由实机证明：把公共消息引擎的两个局部 GetAsyncKeyState 装载点 Hook 住，
 * 确实能让 RPG.exe 真正读到手柄 A；因此 refactor11 不再换入口，只修正“一颗 A 应该被哪一阶段消费”。
 *
 * 原版公共消息有两个不同阶段：
 *
 * 1. 逐字显示阶段（0x4041D7）
 *    文字还没完全显示时，鼠标左键的原版作用是“把当前句立刻显示完整”。
 *
 * 2. 整句等待阶段（0x40447E）
 *    当前句已经完整显示后，再进行一次原版确认才会进入下一句。
 *
 * refactor10 用同一个一次性 pending 同时喂两个阶段，因此前面的逐字阶段总会先把 A 拿走；
 * 实机表现就是“A 每次都有日志，也能让文字瞬间完整，但之后按 A 仍无法真正进入下一句”。
 *
 * refactor11 把两处机器码分别接到两个不同包装函数：
 * - 如果文字没显示完，第一处消费 A，只做“补全本句”；
 * - 如果文字已经显示完，第一处故意不消费 A，让第二处拿到同一颗 A并按原版路径推进下一句。
 *
 * 这样没有模拟鼠标坐标，也没有直接改消息 ID、字符串指针或剧情状态机；
 * 插件仍只是把 A 翻译成正确阶段的原版确认输入。
 */

/* 安装两阶段公共消息局部输入桥；失败只禁用对话 A，不影响其它封版功能。 */
int DialogueInput_InstallHook(void);

/* worker 每帧调用：活动普通消息中把一次 A 按下登记成一次待消费输入。 */
void DialogueInput_Update(void);

/* 只读 RPG.exe 公共消息活动槽，判断当前是否有普通消息正在显示/等待推进。 */
int DialogueInput_IsActive(void);

#endif /* CASTLE_PAD_DIALOGUE_INPUT_H */
