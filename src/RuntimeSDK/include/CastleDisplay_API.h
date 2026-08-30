#ifndef CASTLE_DISPLAY_API_H
#define CASTLE_DISPLAY_API_H

/*
 * CastleDisplay_API.h
 *
 * Display API 回答“玩家这一帧实际看见了什么”，而不是“游戏逻辑 Camera 当前保存了什么”。
 * Widescreen 会临时修改 Camera 画中央/左/右三次，再把游戏 Camera 恢复；因此 Quest 等消费者
 * 不能读取原 Camera 后固定加 107/240，必须通过 Runtime 的统一投影门面。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_DISPLAY_INTERFACE_ID "org.castlereforge.game.display"
#define CASTLE_DISPLAY_API_VERSION_1 1u

#define CASTLE_DISPLAY_API_MAGIC       0x50534443ul /* CDSP */
#define CASTLE_DISPLAY_PROVIDER_MAGIC  0x56504443ul /* CDPV */
#define CASTLE_DISPLAY_GEOMETRY_MAGIC  0x4D474443ul /* CDGM */
#define CASTLE_WORLD_TO_SCREEN_MAGIC   0x53325743ul /* CW2S */
#define CASTLE_SCREEN_PROJECTION_MAGIC 0x52505343ul /* CSPR */
#define CASTLE_SCREEN_TO_WORLD_MAGIC   0x57325343ul /* CS2W */
#define CASTLE_WORLD_PROJECTION_MAGIC  0x52505743ul /* CWPR */
#define CASTLE_DISPLAY_STATE_MAGIC     0x54534443ul /* CDST */
#define CASTLE_DISPLAY_PROVIDER_INFO_MAGIC 0x49504443ul /* CDPI */

#define CASTLE_DISPLAY_STRUCTURE_VERSION_1 1u

/* 门面能力与 Provider 能力使用同一位；ScreenToWorld 缺失时函数返回可选不可用。 */
#define CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD   (1ul << 0)
#define CASTLE_DISPLAY_CAP_EXTERNAL_PROVIDER (1ul << 1)

/* 当前最终画面模式。UNKNOWN/TRANSITION 时消费者不能拿上一帧坐标猜测。 */
#define CASTLE_DISPLAY_UNKNOWN        0u
#define CASTLE_DISPLAY_ORIGINAL_4_3   1u
#define CASTLE_DISPLAY_WIDE_WORLD     2u
#define CASTLE_DISPLAY_CINEMATIC_4_3  3u
#define CASTLE_DISPLAY_HARD_4_3       4u
#define CASTLE_DISPLAY_BATTLE_4_3     5u
#define CASTLE_DISPLAY_TRANSITION     6u

/* 世界叠加层本帧允许使用的最终输出区域。 */
#define CASTLE_PROJECTION_NONE        0u
#define CASTLE_PROJECTION_CENTER_640  1u
#define CASTLE_PROJECTION_FULL_OUTPUT 2u

/* WorldToScreen 的结果分类。 */
#define CASTLE_VISIBILITY_VISIBLE         0u
#define CASTLE_VISIBILITY_OFFSCREEN_LEFT  1u
#define CASTLE_VISIBILITY_OFFSCREEN_RIGHT 2u
#define CASTLE_VISIBILITY_OFFSCREEN_TOP   3u
#define CASTLE_VISIBILITY_OFFSCREEN_BOTTOM 4u
#define CASTLE_VISIBILITY_NOT_PROJECTABLE 5u

/* Display provider 的只读状态。 */
#define CASTLE_DISPLAY_PROVIDER_NOT_READY 0u
#define CASTLE_DISPLAY_PROVIDER_READY     1u
#define CASTLE_DISPLAY_PROVIDER_FAULT     2u

/*
 * 本帧最终显示几何。
 * effective_camera 是最终合成计划的权威 Camera；original_camera 只用于诊断。
 */
typedef struct CastleDisplayGeometryV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 flags;
    CastleU32 generation;
    CastleU32 frame_id;
    CastleU32 output_width;
    CastleU32 output_height;
    CastleU32 logical_width;
    CastleU32 logical_height;
    CastleS32 center_x;
    CastleS32 center_y;
    CastleS32 center_width;
    CastleS32 center_height;
    CastleS32 effective_camera_x;
    CastleS32 effective_camera_y;
    CastleS32 original_camera_x;
    CastleS32 original_camera_y;
    CastleU32 left_world_width;
    CastleU32 right_world_width;
    CastleU32 display_mode;
    CastleU32 projection_scope;
    CastleU32 transition_value;
    CastleU32 transition_max;
} CastleDisplayGeometryV1;

/* requested_generation=0 表示接受当前快照；非零表示调用方要求严格使用指定代次。 */
typedef struct CastleWorldToScreenRequestV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 request_version;
    CastleU32 flags;
    CastleU32 requested_generation;
    CastleS32 world_x;
    CastleS32 world_y;
    CastleU32 reserved_zero;
} CastleWorldToScreenRequestV1;

typedef struct CastleScreenProjectionV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 result_version;
    CastleU32 flags;
    CastleU32 actual_generation;
    CastleS32 screen_x;
    CastleS32 screen_y;
    CastleU32 visibility;
    CastleU32 projection_scope;
} CastleScreenProjectionV1;

/* ScreenToWorld 是可选能力，结构仍提前冻结，避免后来在 Display v1 中插字段。 */
typedef struct CastleScreenToWorldRequestV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 request_version;
    CastleU32 flags;
    CastleU32 requested_generation;
    CastleS32 screen_x;
    CastleS32 screen_y;
    CastleU32 reserved_zero;
} CastleScreenToWorldRequestV1;

typedef struct CastleWorldProjectionV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 result_version;
    CastleU32 flags;
    CastleU32 actual_generation;
    CastleS32 world_x;
    CastleS32 world_y;
    CastleU32 visibility;
    CastleU32 projection_scope;
} CastleWorldProjectionV1;

/* Runtime Display 门面的当前状态，不把专业后端私有对象暴露给消费者。 */
typedef struct CastleDisplayStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 ready;
    CastleU32 generation;
    CastlePluginHandle backend_plugin;
    CastleU32 display_mode;
} CastleDisplayStateV1;

/* 提供者信息只用于诊断和 UI，不允许消费者据此直接调用后端模块。 */
typedef struct CastleDisplayProviderInfoV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleProviderHandle provider_handle;
    CastlePluginHandle plugin_handle;
    CastleU32 generation;
    CastleU32 state;
    CastleStringView provider_id;
} CastleDisplayProviderInfoV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleDisplayGetStateFn)(
    CastleDisplayStateV1* out_state);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleDisplayGetGeometryFn)(
    CastleDisplayGeometryV1* out_geometry);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleDisplayWorldToScreenFn)(
    const CastleWorldToScreenRequestV1* request,
    CastleScreenProjectionV1* out_projection);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleDisplayScreenToWorldFn)(
    const CastleScreenToWorldRequestV1* request,
    CastleWorldProjectionV1* out_projection);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleDisplayGetProviderInfoFn)(
    CastleDisplayProviderInfoV1* out_info);

/* 专业后端函数表。Runtime 调用这些函数时不持有全局锁。 */
typedef struct CastleDisplayProviderV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleDisplayGetStateFn GetProviderState;
    CastleDisplayGetGeometryFn CopyCurrentGeometry;
    CastleDisplayWorldToScreenFn WorldToScreen;
    CastleDisplayScreenToWorldFn ScreenToWorld;
} CastleDisplayProviderV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterDisplayProviderFn)(
    CastlePluginHandle plugin,
    CastleStringView provider_id,
    const CastleDisplayProviderV1* provider_api,
    CastleProviderHandle* out_provider);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePublishDisplayGeometryFn)(
    CastleProviderHandle provider,
    const CastleDisplayGeometryV1* geometry,
    CastleU32* out_runtime_generation);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleSetDisplayProviderReadyFn)(
    CastleProviderHandle provider,
    CastleU32 ready);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetDisplayProviderStateFn)(
    CastleProviderHandle provider,
    CastleDisplayStateV1* out_state);

/* Display v1：16 字节表头 + 5 个消费函数 + 4 个提供者函数 = 52 字节。 */
typedef struct CastleDisplayApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleDisplayGetStateFn GetDisplayState;
    CastleDisplayGetGeometryFn GetGeometry;
    CastleDisplayWorldToScreenFn WorldToScreen;
    CastleDisplayScreenToWorldFn ScreenToWorld;
    CastleDisplayGetProviderInfoFn GetProviderInfo;
    CastleRegisterDisplayProviderFn RegisterDisplayProvider;
    CastlePublishDisplayGeometryFn PublishDisplayGeometry;
    CastleSetDisplayProviderReadyFn SetDisplayProviderReady;
    CastleGetDisplayProviderStateFn GetDisplayProviderState;
} CastleDisplayApiV1;

#define CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1      96u
#define CASTLE_SIZEOF_WORLD_TO_SCREEN_V1       32u
#define CASTLE_SIZEOF_SCREEN_PROJECTION_V1     36u
#define CASTLE_SIZEOF_SCREEN_TO_WORLD_V1       32u
#define CASTLE_SIZEOF_WORLD_PROJECTION_V1      36u
#define CASTLE_SIZEOF_DISPLAY_STATE_V1         32u
#define CASTLE_SIZEOF_DISPLAY_PROVIDER_INFO_V1 40u
#define CASTLE_SIZEOF_DISPLAY_PROVIDER_V1      32u
#define CASTLE_SIZEOF_DISPLAY_API_V1           52u

#pragma pack(pop)

#endif /* CASTLE_DISPLAY_API_H */
