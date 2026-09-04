#ifndef CASTLE_BACKLOG_PAD_BRIDGE_H
#define CASTLE_BACKLOG_PAD_BRIDGE_H

#include "platform.h"
#include "CastleRuntime_API.h"

/*
 * pad_bridge.h
 *
 * Backlog 本身不再加载 SDL3.dll，也不打开任何手柄。
 * 本层只查询 Runtime Input v1，不识别 Castle_PadSupport.asi 文件名或私有导出。
 * 它不会扫描 PadSupport 的机器码/内部结构，不会调用 SDL3，也不会维护第二份手柄状态。
 *
 * 这意味着：
 * - Runtime 没有 Input Provider：只有键盘和鼠标；
 * - Controller 发布 Input Provider：获得 LB、方向键和“取消”语义。
 */

typedef enum PadBridgeButton {
    PAD_BRIDGE_LB,
    PAD_BRIDGE_DPAD_UP,
    PAD_BRIDGE_DPAD_DOWN,
    PAD_BRIDGE_DPAD_LEFT,
    PAD_BRIDGE_DPAD_RIGHT,
    PAD_BRIDGE_CANCEL
} PadBridgeButton;

/* 查询 Runtime Input 门面并准备第一份只读快照。 */
void PadBridge_Initialize(const CastleRuntimeApiV1* runtime_api);

/* worker 每 tick 调用；Provider 尚未就绪时只把本帧标记为不可用。 */
void PadBridge_Poll(void);

/* 是否取得 Runtime 输入快照，并且 Provider 当前 ready + connected。 */
int PadBridge_Available(void);

/*
 * 读取 PadSupport 已采样好的“当前物理按住”状态。
 * 不调用 SDL、不更新手柄、不修改 PadSupport 的 consumed mask。
 */
int PadBridge_Down(PadBridgeButton button);

/*
 * PadSupport 的调查模式、RT 临时鼠标模式、Back 常驻鼠标模式都会返回 1。
 * 返回 1 时 Backlog 必须完全忽略手柄输入，让模式插件独占这些按钮。
 */
int PadBridge_BlocksBacklogInput(void);

/* 清掉本地 Runtime 门面和快照；不会卸载或修改输入 Provider。 */
void PadBridge_Shutdown(void);

#endif /* CASTLE_BACKLOG_PAD_BRIDGE_H */
