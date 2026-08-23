#ifndef CASTLE_PAD_EXPLORATION_H
#define CASTLE_PAD_EXPLORATION_H

/*
 * exploration.h
 *
 * 这个模块只负责“角色已经在地图上自由行动”时的左摇杆移动。
 * 它不会打开菜单、不会处理战斗，也不会模拟键盘方向键。
 *
 * dev20 已经实机确认的行为只有两件：
 * 1. 左摇杆方向继续交给原版 8 方向 resolver；
 * 2. 摇杆幅度决定走/跑，并复用原版 0x52 状态转换，不自己修改速度倍率。
 *
 * 把这两件事单独放在 Exploration 中，未来即使给地图十字键增加别的功能，
 * 也不需要改 Battle / SaveSlot / Frontend 的任何代码。
 */

/* 安装地图移动与原版走跑协议所需 Hook。失败时返回 0，调用者会停止整个插件工作线程。 */
int Exploration_InstallHooks(void);

/* 每个 worker tick 调用一次：读取已经采样好的左摇杆，更新本帧希望使用的原版移动语义。 */
void Exploration_Update(void);

#endif /* CASTLE_PAD_EXPLORATION_H */
