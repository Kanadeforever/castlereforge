#ifndef CASTLE_PAD_INTERFACE_SKILLS_H
#define CASTLE_PAD_INTERFACE_SKILLS_H

#include "platform.h"
#include "cursor.h"

/*
 * interface_skills.h
 *
 * 这是主 Interface state3“绝学/法术”页面的独立手柄 Adapter。
 * 它负责这个页面自己的两种子类型、8 行技能、分页、A 使用、页面确认框，
 * 以及治疗法术确认后的“顶部角色头像目标选择”。
 * 普通列表状态下 LB/RB 大类与根层 B 仍由 InterfaceShell 负责；角色切换已按 refactor26 统一改成左摇杆水平50%。
 * 一旦进入治疗目标选择，页面会成为最深 modal，只允许 ←/→ 选目标、A 使用、B 取消。
 *
 * 特别注意：这里不会把 state3 的确认框抽成“全游戏通用 Yes/No”。
 * 用户已经明确要求先把每个页面分别做对，全部稳定以后才允许重新评估是否存在真正公共协议。
 */

/* 预检 state3 的 6 个原版 ButtonEvent CALL + 治疗目标状态短锚点，全部匹配后才安装页面 Hook。 */
int InterfaceSkills_InstallHooks(void);

/* worker 每 tick 调用；只有主 Interface 当前 state==3 时才执行任何技能页业务。 */
void InterfaceSkills_Update(void);

/* 实体鼠标/右摇杆鼠标取得控制权时，清掉本页只属于手柄的等待与强制焦点。 */
void InterfaceSkills_OnPointerTakeover(CursorTakeoverEvent event);

/* 给历史 ConfirmDialog 做所有权隔离：state3 自己的使用确认框活动时，通用候选层必须让路。 */
int InterfaceSkills_AnyPopupActive(void);

/*
 * 治疗法术“确认使用”后，原版会进入角色头像鼠标选择阶段。
 * Shell 只需要知道“现在有更深的 modal 正在工作”，不需要知道 +0x768/+0x770 等内部字段。
 */
int InterfaceSkills_TargetSelectionActive(void);

/* 手柄主动导航弹窗后，UiBridge 用这个过滤器强制原版 HitTest 返回与逻辑焦点一致。 */
u8 InterfaceSkills_FilterPopupHit(void* button, int* handled);

/* 手柄尚未导航时，UiBridge 把原版真实 HitTest 返回值交给本页，用来同步屏幕视觉焦点。 */
void InterfaceSkills_ObservePopupHit(void* button, u8 hit_value);

#endif /* CASTLE_PAD_INTERFACE_SKILLS_H */
