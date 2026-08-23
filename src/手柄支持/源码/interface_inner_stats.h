#ifndef CASTLE_PAD_INTERFACE_INNER_STATS_H
#define CASTLE_PAD_INTERFACE_INNER_STATS_H

#include "platform.h"
#include "cursor.h"

/*
 * interface_inner_stats.h
 *
 * 主 Interface state5“五内”页面的独立手柄 Adapter。
 *
 * 这个页面不使用普通“上下列表”模型：五个属性点分布在画面不同位置，原版键鼠完全依靠鼠标点图标。
 * 因此本模块只暴露生命周期入口和“鼠标/右摇杆接管”通知，不允许其它模块直接读写五内临时数值。
 */

/* 校验 0x440610 原版协议并安装唯一的 11按钮循环 ButtonEvent Hook。 */
int InterfaceInnerStats_InstallHooks(void);

/* worker 每 tick 调用；只有主 Interface state==5 时处理 X、LT/RT 与 Y。 */
void InterfaceInnerStats_Update(void);

/*
 * 用户改用右摇杆鼠标或实体鼠标时，暂时关闭“手柄焦点鼠标图示”。
 * 下一次再按 X/LT/RT/Y，Adapter 会重新取得手柄所有权并把图示放回当前节点。
 *
 * refactor26c 的重要事务规则：如果某次 LT/RT/Y 已经开始原版 press->release，
 * 指针接管只隐藏焦点图示，绝不能中途取消这个按钮事务；否则按下动画已经出现、释放动画却不会执行。
 */
void InterfaceInnerStats_OnPointerTakeover(CursorTakeoverEvent event);

/*
 * state5 原版每帧在 0x44066E 调 0x431400 读取鼠标左/右键边沿。
 * 当原版鼠标只是“手柄焦点图示”时，这个调用必须被视觉模式吞掉，不能把合成鼠标误当成真实点击。
 * 没有正在进行的手柄点击事务时，真实鼠标/右摇杆接管后会自动退回原版 0x431400。
 * 如果 press->release 已经开始，则只在极短的两帧事务完成前继续独占 state5 的 11 个按钮，完成后立刻恢复真实鼠标。
 */
i32 FASTCALL InterfaceInnerStats_HookMouseEdge(void* button, void* unused_edx);

/*
 * UiBridge 在无手柄 pending 时调用它，判断当前 ButtonEvent 是否只是被“视觉鼠标”命中的五内按钮。
 * 返回 1 时必须跳过原版 ButtonEvent，并由 Adapter 同步 Button+0x28 的真实鼠标释放历史，防止合成光标或合成点击留下持续动画。
 */
int InterfaceInnerStats_FilterVisualOnlyButtonEvent(void* button);

/*
 * UiBridge 的 state5 ButtonEvent CALL 在普通 pending 之前询问这里。
 * 返回 2 表示“上一游戏帧已经完成合成按下，现在应该执行原版松开/确认阶段”；该阶段同时把 Button+0x28 恢复为0，完整结束一次鼠标点击历史。返回 0 表示没有合成释放。
 */
int InterfaceInnerStats_TrySyntheticButtonEvent(void* button);

/* 返回 state5 的按下→释放短事务是否仍在进行；UiBridge 用它统一阻止其它菜单事件并发。 */
int InterfaceInnerStats_SyntheticEventPending(void);

#endif /* CASTLE_PAD_INTERFACE_INNER_STATS_H */
