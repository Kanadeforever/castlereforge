#ifndef CASTLE_PAD_PUBLIC_API_INTERNAL_H
#define CASTLE_PAD_PUBLIC_API_INTERNAL_H

#include "CastleRuntime_API.h"

/*
 * pad_public_api.h
 *
 * 这是 Castle_PadSupport.asi 内部使用的“公共 API 发布器”声明。
 *
 * 它与给第三方插件复制的 Castle_PadSupport_API.h 不同：
 * - Castle_PadSupport_API.h 是长期稳定的外部 ABI，第三方可以随自己插件一起保存；
 * - pad_public_api.h 只是 PadSupport 自己内部调用，用来把这一轮已经采样好的输入状态
 *   发布成只读快照。
 *
 * 为什么要有“发布”这一步，而不是让外部插件直接调用 PadInput_Down：
 * PadSupport 的输入 worker 和外部 ASI 的 worker 可能是两个不同线程。如果外部线程直接
 * 读 g_pad / InputRouter 正在更新的内部状态，会把内部实现细节和线程时序一起泄露出去。
 * 本模块在 PadSupport 自己的 worker 中先把结果整理成只包含 32 位标量的稳定快照，
 * 第三方随后只读取快照，因此不需要知道 SDL、g_pad、consumed mask 或 ControlModes 结构。
 */

/*
 * 每个 PadSupport worker tick 在“本轮 ControlModes 已经完成裁决”后调用一次。
 * 函数只读取现有 PadInput/InputRouter/ControlModes，不会产生输入、Consume 动作或修改模式。
 */
void CastlePad_PublicApiPublishFrame(void);

/*
 * PadSupport 初始化前/退出时调用，把公开快照清为“未就绪、未连接、禁止外部 UI 输入”。
 * 导出的 CastlePad_GetApi 本身仍然存在；调用者会从 IsReady() 得到 0，而不是拿到悬空表。
 */
void CastlePad_PublicApiReset(void);

/*
 * Integrated 初始化时把同一份稳定快照注册为 Runtime Input Provider。
 * 旧 CastlePad_GetApi 导出继续保留给仓库外第三方，官方插件只查询 Runtime 门面。
 */
int CastlePad_RegisterRuntimeInputProvider(const CastleRuntimeApiV1* runtime_api,
                                           CastlePluginHandle plugin_handle);
void CastlePad_RuntimeInputProviderShutdown(void);

#endif /* CASTLE_PAD_PUBLIC_API_INTERNAL_H */
