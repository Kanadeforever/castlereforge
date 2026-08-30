#ifndef CASTLE_HOOK_API_H
#define CASTLE_HOOK_API_H

/*
 * CastleHook_API.h
 *
 * 这个接口把“谁可以修改哪段游戏内存”变成 Runtime 可验证的事务。
 * 插件先完整声明，再预检，再一次提交；不能写到一半才发现第五个地址冲突。
 *
 * 本头只定义 ABI，不包含真正的 VirtualProtect 或机器码写入实现。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_HOOK_INTERFACE_ID "org.castlereforge.runtime.hook"
#define CASTLE_HOOK_API_VERSION_1 1u

#define CASTLE_HOOK_API_MAGIC          0x4B4F4843ul /* CHOK */
#define CASTLE_EXCLUSIVE_PATCH_MAGIC   0x54415043ul /* CPAT */
#define CASTLE_STATE_PATCH_MAGIC       0x41545343ul /* CSTA */
#define CASTLE_CHAIN_HOOK_MAGIC        0x4E484343ul /* CCHN */
#define CASTLE_HOOK_BINDING_MAGIC      0x444E4243ul /* CBND */
#define CASTLE_NAMED_RESOURCE_MAGIC    0x53524E43ul /* CNRS */
#define CASTLE_ORDER_CONSTRAINT_MAGIC  0x44524F43ul /* CORD */
#define CASTLE_CLAIM_RESULT_MAGIC      0x53524343ul /* CCRS */

#define CASTLE_HOOK_STRUCTURE_VERSION_1 1u

/* 独占补丁 flags。CODE 与 DATA 必须二选一。 */
#define CASTLE_PATCH_FLAG_CODE                 (1ul << 0)
#define CASTLE_PATCH_FLAG_DATA                 (1ul << 1)
#define CASTLE_PATCH_FLAG_OPTIONAL             (1ul << 2)
#define CASTLE_PATCH_FLAG_KEEP_ON_PROCESS_EXIT (1ul << 3)

/* 可串联指针 Hook 的种类。 */
#define CASTLE_HOOK_REL32_CALL     1u
#define CASTLE_HOOK_IAT_POINTER    2u
#define CASTLE_HOOK_VTABLE_POINTER 3u

/* 链阶段。FOUNDATION/FINAL 只给经过项目级评审的基础设施使用。 */
#define CASTLE_HOOK_PHASE_FOUNDATION 0u
#define CASTLE_HOOK_PHASE_PRE        1u
#define CASTLE_HOOK_PHASE_NORMAL     2u
#define CASTLE_HOOK_PHASE_POST       3u
#define CASTLE_HOOK_PHASE_FINAL      4u

/* 每个阶段只允许三个有限优先级，避免第三方使用极端数字抢占。 */
#define CASTLE_HOOK_PRIORITY_EARLY   0u
#define CASTLE_HOOK_PRIORITY_DEFAULT 1u
#define CASTLE_HOOK_PRIORITY_LATE    2u

/* 事务状态。INVALID 同时作为无效句柄查询结果。 */
#define CASTLE_TRANSACTION_INVALID      0u
#define CASTLE_TRANSACTION_BUILDING     1u
#define CASTLE_TRANSACTION_PREFLIGHTED  2u
#define CASTLE_TRANSACTION_COMMITTED    3u
#define CASTLE_TRANSACTION_ABORTED      4u
#define CASTLE_TRANSACTION_ROLLED_BACK  5u

/* 双态补丁的目标状态。 */
#define CASTLE_PATCH_STATE_ORIGINAL 0u
#define CASTLE_PATCH_STATE_ENABLED  1u

/* 顺序约束：另一个插件必须位于本节点之前或之后。 */
#define CASTLE_ORDER_OTHER_BEFORE 1u
#define CASTLE_ORDER_OTHER_AFTER  2u

/*
 * module + rva 描述目标，避免把“RPG.exe 的 RVA”和“绝对地址”混在一个裸整数里。
 * size 同时参与溢出、页面和资源范围检查。
 */
typedef struct CastleTargetAddressV1 {
    CastleModule module;
    CastleU32 rva;
    CastleU32 size;
} CastleTargetAddressV1;

/* 单向独占补丁：当前必须等于 expected，成功后变成 replacement。 */
typedef struct CastleExclusivePatchClaimV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleTargetAddressV1 target;
    const CastleU8* expected_bytes;
    CastleU32 expected_size;
    const CastleU8* replacement_bytes;
    CastleU32 replacement_size;
    CastleStringView label;
} CastleExclusivePatchClaimV1;

/*
 * 双态补丁：当前可以是原版或已启用状态，desired_state 决定最终写成哪一个。
 * 这服务 MaxGrowthAndDrop 等配置开关，不接受第三种未知机器码。
 */
typedef struct CastleStatePatchClaimV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleTargetAddressV1 target;
    const CastleU8* original_bytes;
    CastleU32 original_size;
    const CastleU8* enabled_bytes;
    CastleU32 enabled_size;
    CastleU32 desired_state;
    CastleStringView label;
} CastleStatePatchClaimV1;

/*
 * CALL/IAT/vtable 共用链声明。
 * expected_original_target 是整条链最底部的原函数，不是安装时偶然看见的上一插件地址。
 */
typedef struct CastleChainHookClaimV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 hook_kind;
    CastleTargetAddressV1 target;
    CastleAddress expected_original_target;
    CastleAddress replacement_hook;
    CastleStringView signature_id;
    CastleU32 phase;
    CastleU32 priority;
    CastleStringView label;
} CastleChainHookClaimV1;

/*
 * Runtime 为每个链节点分配一个地址稳定的 next_slot。
 * 槽里的函数地址可以因后加载节点改变，但槽本身不能移动。
 */
typedef struct CastleHookBindingV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleClaimHandle claim_handle;
    void* volatile* next_slot;
    CastleU32 chain_generation;
} CastleHookBindingV1;

/* 非地址型全局资源，例如额外帧租约、主 WndProc 或输入所有权。 */
typedef struct CastleNamedResourceClaimV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleStringView resource_id;
    CastleU32 resource_kind;
    CastleU32 phase;
    CastleU32 priority;
    CastleStringView label;
} CastleNamedResourceClaimV1;

/* 插件 ID 约束会被复制到 Runtime，调用后原字符串缓冲区可以释放。 */
typedef struct CastleOrderConstraintV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleClaimHandle claim_handle;
    CastleU32 relation;
    CastleStringView other_plugin_id;
} CastleOrderConstraintV1;

/* 每项声明的可查询结果。owner_plugin_id 为空表示没有现有所有者。 */
typedef struct CastleClaimResultV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleClaimHandle claim_handle;
    CastleResult result;
    CastleU32 resource_generation;
    CastleStringView owner_plugin_id;
} CastleClaimResultV1;

/* Hook API 函数指针类型。Add 函数返回的 claim 只在所属事务/插件范围内有效。 */
typedef CastleResult (CASTLE_RUNTIME_CALL *CastleBeginTransactionFn)(
    CastlePluginHandle plugin,
    CastleStringView label,
    CastleU32 flags,
    CastleTransactionHandle* out_transaction);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAddExclusivePatchFn)(
    CastleTransactionHandle transaction,
    const CastleExclusivePatchClaimV1* claim,
    CastleClaimHandle* out_claim);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAddStatePatchFn)(
    CastleTransactionHandle transaction,
    const CastleStatePatchClaimV1* claim,
    CastleClaimHandle* out_claim);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAddRelativeCallHookFn)(
    CastleTransactionHandle transaction,
    const CastleChainHookClaimV1* claim,
    CastleClaimHandle* out_claim);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAddPointerHookFn)(
    CastleTransactionHandle transaction,
    const CastleChainHookClaimV1* claim,
    CastleClaimHandle* out_claim);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAddNamedResourceFn)(
    CastleTransactionHandle transaction,
    const CastleNamedResourceClaimV1* claim,
    CastleClaimHandle* out_claim);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAddOrderConstraintFn)(
    CastleTransactionHandle transaction,
    const CastleOrderConstraintV1* constraint_value);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePreflightTransactionFn)(
    CastleTransactionHandle transaction);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleCommitTransactionFn)(
    CastleTransactionHandle transaction);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAbortTransactionFn)(
    CastleTransactionHandle transaction);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetClaimResultFn)(
    CastleClaimHandle claim,
    CastleClaimResultV1* out_result);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetHookBindingFn)(
    CastleClaimHandle claim,
    CastleHookBindingV1* out_binding);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleDescribeResourceFn)(
    CastleClaimHandle claim,
    CastleDiagnosticBufferV1* output);

/* Hook v1 门面：16 字节表头 + 13 个 4 字节函数指针 = 68 字节。 */
typedef struct CastleHookApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleBeginTransactionFn BeginTransaction;
    CastleAddExclusivePatchFn AddExclusivePatch;
    CastleAddStatePatchFn AddStatePatch;
    CastleAddRelativeCallHookFn AddRelativeCallHook;
    CastleAddPointerHookFn AddPointerHook;
    CastleAddNamedResourceFn AddNamedResource;
    CastleAddOrderConstraintFn AddOrderConstraint;
    CastlePreflightTransactionFn PreflightTransaction;
    CastleCommitTransactionFn CommitTransaction;
    CastleAbortTransactionFn AbortTransaction;
    CastleGetClaimResultFn GetClaimResult;
    CastleGetHookBindingFn GetHookBinding;
    CastleDescribeResourceFn DescribeResource;
} CastleHookApiV1;

#define CASTLE_SIZEOF_TARGET_ADDRESS_V1       12u
#define CASTLE_SIZEOF_EXCLUSIVE_PATCH_V1      52u
#define CASTLE_SIZEOF_STATE_PATCH_V1          56u
#define CASTLE_SIZEOF_CHAIN_HOOK_V1           64u
#define CASTLE_SIZEOF_HOOK_BINDING_V1         28u
#define CASTLE_SIZEOF_NAMED_RESOURCE_V1       44u
#define CASTLE_SIZEOF_ORDER_CONSTRAINT_V1     32u
#define CASTLE_SIZEOF_CLAIM_RESULT_V1         36u
#define CASTLE_SIZEOF_HOOK_API_V1             68u

#pragma pack(pop)

#endif /* CASTLE_HOOK_API_H */
