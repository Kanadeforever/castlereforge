#ifndef CASTLE_PAD_INTERFACE_SHELL_H
#define CASTLE_PAD_INTERFACE_SHELL_H

/*
 * interface_shell.h
 *
 * 这个模块只负责游戏内主 Interface 的“外壳级”输入，不负责任何页面内部菜单。
 * 当前第一阶段范围严格限定为：
 * 1. 普通地图探索时按 Y，不伪造 Space 按键，而是在 Exploration 游戏线程安全点
 *    直接调用原版 Space 分支最终使用的地图动作事件 0x40B230(1)；
 * 2. Interface 根层按 LB/RB，复用顶部八大类真实 ButtonEvent；
 * 3. state1..5 左摇杆水平推过 50% 时，复用角色真实 ButtonEvent；
 * 4. 根层按 B，复用原版退出 ButtonEvent 回地图。
 *
 * 五内、阵形、天书、机能以及法宝/绝学/及身页面内部都必须以后各自实现 adapter。
 * 不允许为了省文件数量把它们提前塞回这个 Shell。
 */

/*
 * 安装本轮需要的原版 Interface ButtonEvent Hook。
 * 函数内部会先做整组机器码/CALL 预检；任何一项不一致就直接返回 0，不留下半安装状态。
 */
int InterfaceShell_InstallHooks(void);

/*
 * worker 每 tick 调用一次：
 * - Interface 不存在时捕获 Y 新按下沿，并等待 Exploration 游戏线程安全点执行；
 * - Interface 存在时，缓存外壳级 LB/RB、state1..5 左摇杆角色切换、根层 B，
 *   等原版动画门重新开放后按顺序提交，避免动画期间的按键直接丢失。
 */
void InterfaceShell_Update(void);


/*
 * Exploration 模块的游戏线程 Hook 每帧调用一次。
 * worker 线程捕获到地图 Y 后，只在这里把它转换成原版 0x40B230(1) 地图动作事件。
 * 这样不会从 worker 直接修改游戏状态，也不会向 Windows 发送任何假的 Space 键。
 */
void InterfaceShell_OnExplorationGameThread(void);

/* 返回原版 GLOBAL_INTERFACE_UI 当前是否指向一个有效对象。 */
int InterfaceShell_Active(void);

/*
 * state6 阵形等页面禁止角色切换。
 * 页面 Adapter 在取得独占导航所有权时调用这里，只删除此前动画期队列中尚未提交的“上一/下一角色”动作，
 * 不删除 B、LB/RB 等其它 Shell 意图。这样即使玩家在 state4->state6 动画交界处快速按方向，也不会让旧换人动作延迟落进阵形页。
 */
void InterfaceShell_DiscardQueuedRoleActions(void);

#endif /* CASTLE_PAD_INTERFACE_SHELL_H */
