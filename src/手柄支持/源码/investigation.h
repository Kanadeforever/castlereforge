#ifndef CASTLE_PAD_INVESTIGATION_H
#define CASTLE_PAD_INVESTIGATION_H

/*
 * investigation.h
 *
 * 地图探索调查是独立 capability：它只观察原版目标 resolver、移动指针、
 * 并最终向原版左键链提交。左杆按角色方向选目标，LB/RB 按距离稳定循环；
 * 两条路线共享同一 25 点 probe。它不调用地图脚本，也不改任何互动结果字段。
 */

int Investigation_InstallHooks(void);
/* 只回答原版地图resolver快照是否新鲜；UI/自由行动门由control_modes统一判断。 */
int Investigation_MapSnapshotReady(void);

/* control_modes确认A或LT调查成立后调用；返回非0表示当前指针会话有效。 */
int Investigation_UpdateActive(void);
/*
 * A按住模式在“松开A”那一帧调用：只有当前没有未完成probe、最新原版快照仍新鲜、
 * 且 resolver 已发布真实 hovered_object 时，才排一次可靠左键脉冲。返回1表示已提交。
 */
int Investigation_ConfirmCurrentHover(void);
void Investigation_EndSession(void);
void Investigation_UpdateRumble(void);
int Investigation_Active(void);

#endif /* CASTLE_PAD_INVESTIGATION_H */
