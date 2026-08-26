#ifndef CASTLE_BACKLOG_H
#define CASTLE_BACKLOG_H

#include "platform.h"

/*
 * backlog.h
 *
 * Backlog 模块拥有历史记录、原版对话框只读展示和按键命令。
 * plugin.c 只需要安装一次、每 8ms 调一次输入采样，并在退出时通知清理。
 */

/* 把场景 vtable[0] 换成链式包装器；失败时不进入输入循环。 */
int Backlog_Install(void);

/* worker 调用：读取键盘、鼠标事件和可选 PadSupport 桥，把动作记成线程安全的计数请求。 */
void Backlog_PollInput(void);

/* 停止接收新动作并尽力恢复 vtable；正常游戏退出时由操作系统最终回收内存。 */
void Backlog_Shutdown(void);

/* 只读状态，供 plugin 生命周期和诊断使用。 */
int Backlog_IsActive(void);

#endif /* CASTLE_BACKLOG_H */
