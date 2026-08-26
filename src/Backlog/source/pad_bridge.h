#ifndef CASTLE_BACKLOG_PAD_BRIDGE_H
#define CASTLE_BACKLOG_PAD_BRIDGE_H

#include "platform.h"

/*
 * pad_bridge.h
 *
 * Backlog 本身不再加载 SDL3.dll，也不打开任何手柄。
 * 如果 Castle_PadSupport.asi 已经加载，本层只通过正式导出 CastlePad_GetApi 取得 Public API v1。
 * 它不会扫描 PadSupport 的机器码/内部结构，不会调用 SDL3，也不会维护第二份手柄状态。
 *
 * 这意味着：
 * - 单独安装 Castle_Backlog.asi：只有键盘和鼠标；
 * - 同时安装 Castle_PadSupport.asi：才会获得 LB、方向键和“取消”语义。
 */

typedef enum PadBridgeButton {
    PAD_BRIDGE_LB,
    PAD_BRIDGE_DPAD_UP,
    PAD_BRIDGE_DPAD_DOWN,
    PAD_BRIDGE_DPAD_LEFT,
    PAD_BRIDGE_DPAD_RIGHT,
    PAD_BRIDGE_CANCEL
} PadBridgeButton;

/* 初始化为“尚未绑定”，并立即尝试一次发现 PadSupport。 */
void PadBridge_Initialize(void);

/* worker 每 tick 调用；PadSupport 后加载时会自动低频重试绑定。 */
void PadBridge_Poll(void);

/* 是否已经成功绑定 PadSupport Public API v1，并且 API 当前 ready + connected。 */
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

/* 清掉本地 API 表指针；不会卸载或修改 Castle_PadSupport.asi。 */
void PadBridge_Shutdown(void);

#endif /* CASTLE_BACKLOG_PAD_BRIDGE_H */
