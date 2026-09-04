#ifndef CASTLE_OVERLAY_API_H
#define CASTLE_OVERLAY_API_H

/*
 * CastleOverlay_API.h
 *
 * Overlay v1 由 Runtime 唯一接管原版 renderer Present 入口。任务 Marker、调试面板和未来
 * HUD 插件只登记绘制回调，不再各自在 0x004064E0 安装 E9。回调先画到原版 back surface，
 * 随后 Runtime 始终调用原 Present，因此任何一个覆盖层失败都不能阻断游戏画面。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_OVERLAY_INTERFACE_ID "org.castlereforge.game.overlay"
#define CASTLE_OVERLAY_API_VERSION_1 1u
#define CASTLE_OVERLAY_STRUCTURE_VERSION_1 1u

#define CASTLE_OVERLAY_API_MAGIC     0x594C5643ul /* CVLY */
#define CASTLE_OVERLAY_CLIENT_MAGIC  0x4C435643ul /* CVCL */
#define CASTLE_OVERLAY_CONTEXT_MAGIC 0x58435643ul /* CVCX */
#define CASTLE_OVERLAY_STATE_MAGIC   0x54535643ul /* CVST */

#define CASTLE_OVERLAY_CAP_BEFORE_RENDERER_PRESENT (1ul << 0)

#define CASTLE_OVERLAY_PHASE_BEFORE_PRESENT 1u

#define CASTLE_OVERLAY_PRIORITY_EARLY   0u
#define CASTLE_OVERLAY_PRIORITY_DEFAULT 1u
#define CASTLE_OVERLAY_PRIORITY_LATE    2u

typedef struct CastleOverlayContextV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 phase;
    CastleAddress render_context;
    CastleU32 display_generation;
    CastleU32 frame_id;
} CastleOverlayContextV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleOverlayDrawFn)(
    const CastleOverlayContextV1* context,
    void* user_context);

typedef struct CastleOverlayClientV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 phase;
    CastleU32 priority;
    CastleOverlayDrawFn draw;
    void* user_context;
    CastleStringView label;
} CastleOverlayClientV1;

typedef struct CastleOverlayStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 ready;
    CastleU32 generation;
    CastleU32 frame_id;
    CastleU32 active_client_count;
} CastleOverlayStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterOverlayFn)(
    CastlePluginHandle plugin,
    const CastleOverlayClientV1* client,
    CastleLeaseHandle* out_client);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleSetOverlayReadyFn)(
    CastleLeaseHandle client,
    CastleU32 ready);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleUnregisterOverlayFn)(
    CastleLeaseHandle client);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetOverlayStateFn)(
    CastleOverlayStateV1* out_state);

typedef struct CastleOverlayApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleRegisterOverlayFn RegisterOverlay;
    CastleSetOverlayReadyFn SetOverlayReady;
    CastleUnregisterOverlayFn UnregisterOverlay;
    CastleGetOverlayStateFn GetOverlayState;
} CastleOverlayApiV1;

#define CASTLE_SIZEOF_OVERLAY_CONTEXT_V1 32u
#define CASTLE_SIZEOF_OVERLAY_CLIENT_V1  40u
#define CASTLE_SIZEOF_OVERLAY_STATE_V1   32u
#define CASTLE_SIZEOF_OVERLAY_API_V1     32u

#pragma pack(pop)

#endif /* CASTLE_OVERLAY_API_H */
