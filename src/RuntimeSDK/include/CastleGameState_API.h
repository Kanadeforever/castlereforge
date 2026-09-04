#ifndef CASTLE_GAME_STATE_API_H
#define CASTLE_GAME_STATE_API_H

/*
 * CastleGameState_API.h
 *
 * GameState v1 是当前游戏公共状态的只读权威快照。插件不再各自复制同一批绝对地址，
 * 也不会因为对“当前是否自由探索”的解释不同而同时显示界面、保存或消费输入。
 * 少数必须临时修改 Camera、绘制队列或对话字段的后端，要先取得资源租约。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_GAME_STATE_INTERFACE_ID "org.castlereforge.game.state"
#define CASTLE_GAME_STATE_API_VERSION_1 1u
#define CASTLE_GAME_STATE_STRUCTURE_VERSION_1 1u

#define CASTLE_GAME_STATE_API_MAGIC      0x54534743ul /* CGST */
#define CASTLE_GAME_SNAPSHOT_MAGIC       0x4E534743ul /* CGSN */
#define CASTLE_GAME_MUTATION_MAGIC       0x554D4743ul /* CGMU */
#define CASTLE_GAME_MUTATION_STATE_MAGIC 0x534D4743ul /* CGMS */

#define CASTLE_GAME_STATE_CAP_SNAPSHOT       (1ul << 0)
#define CASTLE_GAME_STATE_CAP_MUTATION_LEASE (1ul << 1)

#define CASTLE_GAME_FLAG_WORLD_READY        (1ul << 0)
#define CASTLE_GAME_FLAG_DIALOGUE_ACTIVE    (1ul << 1)
#define CASTLE_GAME_FLAG_MOVIE_ACTIVE       (1ul << 2)
#define CASTLE_GAME_FLAG_BATTLE_ACTIVE      (1ul << 3)
#define CASTLE_GAME_FLAG_MENU_ACTIVE        (1ul << 4)
#define CASTLE_GAME_FLAG_EVENT_TABLE_READY  (1ul << 5)
#define CASTLE_GAME_FLAG_FREE_ROAM_CANDIDATE (1ul << 6)

#define CASTLE_GAME_RESOURCE_CAMERA     (1ul << 0)
#define CASTLE_GAME_RESOURCE_DRAW_QUEUE (1ul << 1)
#define CASTLE_GAME_RESOURCE_DIALOGUE   (1ul << 2)
#define CASTLE_GAME_RESOURCE_WORLD      (1ul << 3)
#define CASTLE_GAME_RESOURCE_SAVE_UI    (1ul << 4)
#define CASTLE_GAME_RESOURCE_ALL        0x0000001Ful

typedef struct CastleGameStateSnapshotV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 generation;

    CastleAddress world;
    CastleAddress data_center;
    CastleAddress event_table;
    CastleAddress game_window;
    CastleAddress exploration_manager;

    CastleS32 camera_x;
    CastleS32 camera_y;
    CastleS32 camera_view_width;
    CastleS32 camera_view_height;
    CastleS32 camera_min_x;
    CastleS32 camera_min_y;
    CastleS32 camera_max_x;
    CastleS32 camera_max_y;

    CastleU32 dialogue_mode;
    CastleU32 dialogue_id;
    CastleU32 dialogue_target_state;
    CastleU32 dialogue_current_state;
    CastleU32 event_yield;
    CastleU32 event_blocked;
    CastleU32 active_event_id;
    CastleU32 map_input_gate;
    CastleU32 map_key_mode;

    CastleAddress battle_ui;
    CastleAddress result_ui;
    CastleAddress target_selector_ui;
    CastleAddress title_ui;
    CastleAddress interface_ui;
    CastleAddress save_point_ui;
    CastleAddress inn_ui;
    CastleAddress synthesis_ui;
    CastleAddress shop_ui;
    CastleAddress movie_object;
    CastleU32 movie_active;
} CastleGameStateSnapshotV1;

typedef struct CastleGameMutationRequestV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 resource_mask;
    CastleStringView label;
} CastleGameMutationRequestV1;

typedef struct CastleGameMutationStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 resource;
    CastleU32 active;
    CastleU32 generation;
    CastlePluginHandle owner_plugin;
    CastleLeaseHandle lease;
} CastleGameMutationStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetGameStateSnapshotFn)(
    CastleGameStateSnapshotV1* out_snapshot);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAcquireGameMutationFn)(
    CastlePluginHandle plugin,
    const CastleGameMutationRequestV1* request,
    CastleLeaseHandle* out_lease);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleReleaseGameMutationFn)(
    CastleLeaseHandle lease);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetGameMutationStateFn)(
    CastleU32 resource,
    CastleGameMutationStateV1* out_state);

/* 16 字节表头 + 4 个函数指针 = 32 字节。 */
typedef struct CastleGameStateApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleGetGameStateSnapshotFn GetSnapshot;
    CastleAcquireGameMutationFn AcquireMutation;
    CastleReleaseGameMutationFn ReleaseMutation;
    CastleGetGameMutationStateFn GetMutationState;
} CastleGameStateApiV1;

#define CASTLE_SIZEOF_GAME_STATE_SNAPSHOT_V1 152u
#define CASTLE_SIZEOF_GAME_MUTATION_REQUEST_V1 28u
#define CASTLE_SIZEOF_GAME_MUTATION_STATE_V1 36u
#define CASTLE_SIZEOF_GAME_STATE_API_V1 32u

#pragma pack(pop)

#endif /* CASTLE_GAME_STATE_API_H */
