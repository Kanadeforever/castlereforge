#ifndef CASTLE_INPUT_API_H
#define CASTLE_INPUT_API_H

/*
 * CastleInput_API.h
 *
 * Input v1 让一个权威输入插件发布完整快照，其它插件只向 Runtime 读取。消费者不再按
 * ASI 文件名 GetModuleHandle/GetProcAddress，也不需要知道输入后端是 SDL3、XInput 还是别的库。
 * 输入焦点租约用于 Backlog、任务界面等覆盖层互斥，防止一颗按键同时穿透多个插件。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_INPUT_INTERFACE_ID "org.castlereforge.game.input"
#define CASTLE_INPUT_API_VERSION_1 1u
#define CASTLE_INPUT_STRUCTURE_VERSION_1 1u

#define CASTLE_INPUT_API_MAGIC      0x54504E43ul /* CNPT */
#define CASTLE_INPUT_SNAPSHOT_MAGIC 0x53504E43ul /* CNPS */
#define CASTLE_INPUT_PROVIDER_MAGIC 0x52504E43ul /* CNPR */
#define CASTLE_INPUT_FOCUS_MAGIC    0x43464E43ul /* CNFC */

#define CASTLE_INPUT_CAP_PHYSICAL_SNAPSHOT (1ul << 0)
#define CASTLE_INPUT_CAP_SEMANTIC_SNAPSHOT (1ul << 1)
#define CASTLE_INPUT_CAP_FOCUS_LEASE       (1ul << 2)
#define CASTLE_INPUT_CAP_EXTERNAL_PROVIDER (1ul << 3)

/* 编号与旧 PadSupport Public API v1 一致，迁移时不会改变已经验收的按键含义。 */
#define CASTLE_INPUT_BUTTON_SOUTH      0u
#define CASTLE_INPUT_BUTTON_EAST       1u
#define CASTLE_INPUT_BUTTON_WEST       2u
#define CASTLE_INPUT_BUTTON_NORTH      3u
#define CASTLE_INPUT_BUTTON_BACK       4u
#define CASTLE_INPUT_BUTTON_START      5u
#define CASTLE_INPUT_BUTTON_R3         6u
#define CASTLE_INPUT_BUTTON_LB         7u
#define CASTLE_INPUT_BUTTON_RB         8u
#define CASTLE_INPUT_BUTTON_DPAD_UP    9u
#define CASTLE_INPUT_BUTTON_DPAD_DOWN 10u
#define CASTLE_INPUT_BUTTON_DPAD_LEFT 11u
#define CASTLE_INPUT_BUTTON_DPAD_RIGHT 12u
#define CASTLE_INPUT_BUTTON_LT         13u
#define CASTLE_INPUT_BUTTON_RT         14u
#define CASTLE_INPUT_BUTTON_COUNT      15u

#define CASTLE_INPUT_AXIS_LEFT_X        0u
#define CASTLE_INPUT_AXIS_LEFT_Y        1u
#define CASTLE_INPUT_AXIS_RIGHT_X       2u
#define CASTLE_INPUT_AXIS_RIGHT_Y       3u
#define CASTLE_INPUT_AXIS_LEFT_TRIGGER  4u
#define CASTLE_INPUT_AXIS_RIGHT_TRIGGER 5u
#define CASTLE_INPUT_AXIS_COUNT         6u

#define CASTLE_INPUT_ACTION_CONFIRM        0u
#define CASTLE_INPUT_ACTION_CANCEL         1u
#define CASTLE_INPUT_ACTION_SPECIAL_X      2u
#define CASTLE_INPUT_ACTION_SPECIAL_Y      3u
#define CASTLE_INPUT_ACTION_NAV_UP         4u
#define CASTLE_INPUT_ACTION_NAV_DOWN       5u
#define CASTLE_INPUT_ACTION_NAV_LEFT       6u
#define CASTLE_INPUT_ACTION_NAV_RIGHT      7u
#define CASTLE_INPUT_ACTION_CATEGORY_PREV  8u
#define CASTLE_INPUT_ACTION_CATEGORY_NEXT  9u
#define CASTLE_INPUT_ACTION_SUBTYPE_PREV  10u
#define CASTLE_INPUT_ACTION_SUBTYPE_NEXT  11u
#define CASTLE_INPUT_ACTION_SYSTEM_START  12u
#define CASTLE_INPUT_ACTION_MOUSE_R3      13u
#define CASTLE_INPUT_ACTION_MODIFIER_SHIFT 14u
#define CASTLE_INPUT_ACTION_COUNT          15u

#define CASTLE_INPUT_CONTROL_CONTROLLER       0u
#define CASTLE_INPUT_CONTROL_PERSISTENT_MOUSE 1u
#define CASTLE_INPUT_CONTROL_TEMP_MOUSE       2u
#define CASTLE_INPUT_CONTROL_INVESTIGATION    3u
#define CASTLE_INPUT_CONTROL_UNKNOWN          0x7FFFFFFFul

#define CASTLE_INPUT_FOCUS_OVERLAY 1u
#define CASTLE_INPUT_FOCUS_MODAL   2u
#define CASTLE_INPUT_FOCUS_POINTER 3u

#define CASTLE_INPUT_PRIORITY_EARLY   0u
#define CASTLE_INPUT_PRIORITY_DEFAULT 1u
#define CASTLE_INPUT_PRIORITY_LATE    2u

typedef struct CastleInputSnapshotV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 generation;
    CastleU32 ready;
    CastleU32 connected;
    CastleU32 game_foreground;
    CastleU32 control_mode;
    CastleU32 allows_external_ui_input;
    CastleU32 button_down;
    CastleU32 button_pressed;
    CastleU32 button_released;
    CastleU32 action_down;
    CastleU32 action_pressed;
    CastleU32 action_released;
    CastleS32 axes[CASTLE_INPUT_AXIS_COUNT];
} CastleInputSnapshotV1;

typedef struct CastleInputFocusRequestV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 focus_kind;
    CastleU32 priority;
    CastleStringView label;
} CastleInputFocusRequestV1;

typedef struct CastleInputFocusStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 active;
    CastleU32 generation;
    CastlePluginHandle owner_plugin;
    CastleLeaseHandle focus_lease;
    CastleU32 focus_kind;
    CastleU32 priority;
} CastleInputFocusStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleInputCopySnapshotFn)(
    CastleInputSnapshotV1* out_snapshot);

typedef struct CastleInputProviderV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleInputCopySnapshotFn CopySnapshot;
} CastleInputProviderV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAcquireInputFocusFn)(
    CastlePluginHandle plugin,
    const CastleInputFocusRequestV1* request,
    CastleLeaseHandle* out_lease);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleReleaseInputFocusFn)(
    CastleLeaseHandle lease);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetInputFocusStateFn)(
    CastleInputFocusStateV1* out_state);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterInputProviderFn)(
    CastlePluginHandle plugin,
    CastleStringView provider_id,
    const CastleInputProviderV1* provider_api,
    CastleProviderHandle* out_provider);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleSetInputProviderReadyFn)(
    CastleProviderHandle provider,
    CastleU32 ready);

/* 16 字节表头 + 6 个函数指针 = 40 字节。 */
typedef struct CastleInputApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleInputCopySnapshotFn GetSnapshot;
    CastleAcquireInputFocusFn AcquireFocus;
    CastleReleaseInputFocusFn ReleaseFocus;
    CastleGetInputFocusStateFn GetFocusState;
    CastleRegisterInputProviderFn RegisterInputProvider;
    CastleSetInputProviderReadyFn SetInputProviderReady;
} CastleInputApiV1;

#define CASTLE_SIZEOF_INPUT_SNAPSHOT_V1 88u
#define CASTLE_SIZEOF_INPUT_FOCUS_REQUEST_V1 32u
#define CASTLE_SIZEOF_INPUT_FOCUS_STATE_V1 40u
#define CASTLE_SIZEOF_INPUT_PROVIDER_V1 20u
#define CASTLE_SIZEOF_INPUT_API_V1 40u

#pragma pack(pop)

#endif /* CASTLE_INPUT_API_H */
