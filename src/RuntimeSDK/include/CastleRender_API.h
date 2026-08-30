#ifndef CASTLE_RENDER_API_H
#define CASTLE_RENDER_API_H

/*
 * CastleRender_API.h
 *
 * Render v1 负责“怎样调用当前 RenderQueue/Present 后端”，Display v1 负责“这一帧
 * 看见什么以及怎样投影”。两个领域通过 Display Provider 代次绑定，避免绘制后端
 * 已切换而坐标仍来自旧后端。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_RENDER_INTERFACE_ID "org.castlereforge.game.render"
#define CASTLE_RENDER_API_VERSION_1 1u
#define CASTLE_RENDER_STRUCTURE_VERSION_1 1u

#define CASTLE_RENDER_API_MAGIC      0x444E5243ul /* CRND */
#define CASTLE_RENDER_PROVIDER_MAGIC 0x50525243ul /* CRRP */
#define CASTLE_RENDER_CALL_MAGIC     0x4C435243ul /* CRCL */
#define CASTLE_RENDER_STATE_MAGIC    0x54535243ul /* CRST */

#define CASTLE_RENDER_CAP_EXTERNAL_PROVIDER (1ul << 0)
#define CASTLE_RENDER_CAP_EXTRA_FRAME_LEASE (1ul << 1)

#define CASTLE_RENDER_PROVIDER_NOT_READY 0u
#define CASTLE_RENDER_PROVIDER_READY     1u
#define CASTLE_RENDER_PROVIDER_FAULT     2u

/* 设置此标志时 extra_frame_lease 必须是调用方当前持有的有效租约。 */
#define CASTLE_RENDER_CALL_EXTRA_WORLD_FRAME (1ul << 0)

typedef struct CastleRenderCallV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleAddress render_context;
    CastleU32 display_generation;
    CastleLeaseHandle extra_frame_lease;
} CastleRenderCallV1;

typedef struct CastleRenderStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 ready;
    CastleU32 generation;
    CastlePluginHandle backend_plugin;
    CastleProviderHandle provider_handle;
    CastleU32 display_provider_generation;
    CastlePluginHandle extra_frame_owner;
} CastleRenderStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRenderGetStateFn)(
    CastleRenderStateV1* out_state);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRenderInvokeFn)(
    const CastleRenderCallV1* call);

typedef struct CastleRenderProviderV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleRenderGetStateFn GetProviderState;
    CastleRenderInvokeFn RenderCurrentQueue;
    CastleRenderInvokeFn PresentCurrentDisplay;
} CastleRenderProviderV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleBeginExtraWorldFrameFn)(
    CastlePluginHandle plugin,
    CastleU32 requested_display_generation,
    CastleLeaseHandle* out_lease,
    CastleU32* out_display_generation);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleEndExtraWorldFrameFn)(
    CastleLeaseHandle lease);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterRenderProviderFn)(
    CastlePluginHandle plugin,
    CastleStringView provider_id,
    CastleProviderHandle display_provider,
    const CastleRenderProviderV1* provider_api,
    CastleProviderHandle* out_provider);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleSetRenderProviderReadyFn)(
    CastleProviderHandle provider,
    CastleU32 ready);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetRenderProviderStateFn)(
    CastleProviderHandle provider,
    CastleRenderStateV1* out_state);

/* 16 字节表头 + 8 个函数指针 = 48 字节。 */
typedef struct CastleRenderApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleRenderGetStateFn GetRenderState;
    CastleRenderInvokeFn RenderCurrentQueue;
    CastleRenderInvokeFn PresentCurrentDisplay;
    CastleBeginExtraWorldFrameFn BeginExtraWorldFrame;
    CastleEndExtraWorldFrameFn EndExtraWorldFrame;
    CastleRegisterRenderProviderFn RegisterRenderProvider;
    CastleSetRenderProviderReadyFn SetRenderProviderReady;
    CastleGetRenderProviderStateFn GetRenderProviderState;
} CastleRenderApiV1;

#define CASTLE_SIZEOF_RENDER_CALL_V1     28u
#define CASTLE_SIZEOF_RENDER_STATE_V1    40u
#define CASTLE_SIZEOF_RENDER_PROVIDER_V1 28u
#define CASTLE_SIZEOF_RENDER_API_V1      48u

#pragma pack(pop)

#endif /* CASTLE_RENDER_API_H */
