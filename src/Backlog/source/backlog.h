#ifndef CASTLE_BACKLOG_H
#define CASTLE_BACKLOG_H

#include "platform.h"
#include "CastleRuntime_API.h"

/*
 * backlog.h
 *
 * Backlog 模块拥有历史记录、原版对话框只读展示和按键命令。
 * plugin.c 通过 Runtime 安装一次并登记每 8ms 输入采样；插件与游戏进程同寿命。
 */

/* 使用 Runtime Hook 事务统一安装五个绘制 CALL 与场景 vtable 包装器。 */
CastleResult Backlog_InstallIntegrated(const CastleRuntimeApiV1* runtime_api,
                                       CastlePluginHandle plugin_handle);

/* worker 调用：读取键盘、鼠标事件和可选 PadSupport 桥，把动作记成线程安全的计数请求。 */
void Backlog_PollInput(void);

/* 只读状态，供 plugin 生命周期和诊断使用。 */
int Backlog_IsActive(void);

#endif /* CASTLE_BACKLOG_H */
