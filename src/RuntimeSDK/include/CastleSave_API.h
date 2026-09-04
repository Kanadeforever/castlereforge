#ifndef CASTLE_SAVE_API_H
#define CASTLE_SAVE_API_H

/*
 * CastleSave_API.h
 *
 * Save v1 首先解决 SaveEnhance 与 Controller 对同一个 SaveAction 的重复接管。存档增强插件
 * 只登记“哪些槽禁止普通手动覆盖”；Runtime 在原版 SaveAction 更新期间统一设置 disabled，
 * 原版鼠标/键盘和 Controller 随后看到的是同一份最终状态，不再互相识别插件名或 Hook 顺序。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_SAVE_INTERFACE_ID "org.castlereforge.game.save"
#define CASTLE_SAVE_API_VERSION_1 1u
#define CASTLE_SAVE_STRUCTURE_VERSION_1 1u

#define CASTLE_SAVE_API_MAGIC       0x56415343ul /* CSAV */
#define CASTLE_SAVE_POLICY_MAGIC    0x4C505343ul /* CSPL */
#define CASTLE_SAVE_UI_STATE_MAGIC  0x49555343ul /* CSUI */

#define CASTLE_SAVE_CAP_MANUAL_SLOT_POLICY (1ul << 0)
#define CASTLE_SAVE_CAP_UI_STATE          (1ul << 1)

#define CASTLE_SAVE_POLICY_ALLOW 1u
#define CASTLE_SAVE_POLICY_DENY  0u

typedef struct CastleManualSavePolicyV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 first_slot;
    CastleU32 last_slot;
    CastleU32 manual_save_allowed;
    CastleStringView label;
} CastleManualSavePolicyV1;

typedef struct CastleSaveUiStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 generation;
    CastleU32 active;
    CastleU32 selected_slot;
    CastleU32 manual_save_allowed;
} CastleSaveUiStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterManualSavePolicyFn)(
    CastlePluginHandle plugin,
    const CastleManualSavePolicyV1* policy,
    CastleProviderHandle* out_policy);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleUnregisterManualSavePolicyFn)(
    CastleProviderHandle policy);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleIsManualSaveAllowedFn)(
    CastleU32 slot,
    CastleU32* out_allowed);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetSaveUiStateFn)(
    CastleSaveUiStateV1* out_state);

/* 16 字节表头 + 4 个函数指针 = 32 字节。 */
typedef struct CastleSaveApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleRegisterManualSavePolicyFn RegisterManualSavePolicy;
    CastleUnregisterManualSavePolicyFn UnregisterManualSavePolicy;
    CastleIsManualSaveAllowedFn IsManualSaveAllowed;
    CastleGetSaveUiStateFn GetSaveUiState;
} CastleSaveApiV1;

#define CASTLE_SIZEOF_MANUAL_SAVE_POLICY_V1 36u
#define CASTLE_SIZEOF_SAVE_UI_STATE_V1      32u
#define CASTLE_SIZEOF_SAVE_API_V1           32u

#pragma pack(pop)

#endif /* CASTLE_SAVE_API_H */
