#ifndef CASTLE_SCHEDULE_API_H
#define CASTLE_SCHEDULE_API_H

/*
 * CastleSchedule_API.h
 *
 * Schedule v1 把多个 ASI 各自的短周期工作线程合并成 Runtime 的一个后台线程。
 * Runtime 只负责“何时按稳定顺序调用”，不会拆开 Controller、Backlog 等插件内部
 * 已经验证的业务步骤。没有可靠游戏线程阶段源时，游戏线程函数明确报告不可用。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_SCHEDULE_INTERFACE_ID "org.castlereforge.runtime.schedule"
#define CASTLE_SCHEDULE_API_VERSION_1 1u
#define CASTLE_SCHEDULE_STRUCTURE_VERSION_1 1u

#define CASTLE_SCHEDULE_API_MAGIC        0x48435343ul /* CSCH */
#define CASTLE_SCHEDULE_TASK_MAGIC       0x4B545343ul /* CSTK */
#define CASTLE_SCHEDULE_STATS_MAGIC      0x54535343ul /* CSST */
#define CASTLE_GAME_PHASE_STATE_MAGIC    0x50475343ul /* CSGP */

/* 接口能力位。v1 首发只宣告 BACKGROUND。 */
#define CASTLE_SCHEDULE_CAP_BACKGROUND  (1ul << 0)
#define CASTLE_SCHEDULE_CAP_GAME_PHASE  (1ul << 1)

/* 有限阶段和优先级避免第三方用极端整数抢占其它插件。 */
#define CASTLE_SCHEDULE_PHASE_EARLY   0u
#define CASTLE_SCHEDULE_PHASE_NORMAL  1u
#define CASTLE_SCHEDULE_PHASE_LATE    2u
#define CASTLE_SCHEDULE_PRIORITY_EARLY   0u
#define CASTLE_SCHEDULE_PRIORITY_DEFAULT 1u
#define CASTLE_SCHEDULE_PRIORITY_LATE    2u

/* 周期任务登记后是否立即启用。未设置时先登记为禁用，由插件准备完毕后显式开启。 */
#define CASTLE_SCHEDULE_TASK_START_ENABLED (1ul << 0)

/* 可查询状态。一次性投递不创建公开句柄，因此只使用周期任务状态。 */
#define CASTLE_SCHEDULE_TASK_DISABLED   1u
#define CASTLE_SCHEDULE_TASK_WAITING    2u
#define CASTLE_SCHEDULE_TASK_RUNNING    3u
#define CASTLE_SCHEDULE_TASK_FAULT      4u

/*
 * 回调运行在 Runtime 唯一后台线程；调用时 Runtime 不持有调度锁。
 * 回调不能假定自己位于游戏线程，也不能跨调用保存 Runtime 私有指针。
 */
typedef CastleResult (CASTLE_RUNTIME_CALL *CastleScheduledTaskFn)(
    CastleTaskHandle task,
    void* user_context);

/*
 * period_ms>0 表示周期任务；PostBackgroundTask 要求 period_ms=0。
 * budget_ms=0 表示只统计、不设预算；非零表示一次回调建议完成的毫秒数上限。
 */
typedef struct CastleScheduledTaskV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 period_ms;
    CastleU32 budget_ms;
    CastleU32 phase;
    CastleU32 priority;
    CastleScheduledTaskFn callback;
    void* user_context;
    CastleStringView label;
} CastleScheduledTaskV1;

/* 统计只由 Runtime 写入；所有计数饱和后保持最大值，不允许回卷伪装成较小值。 */
typedef struct CastleScheduleTaskStatsV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleTaskHandle task_handle;
    CastlePluginHandle plugin_handle;
    CastleU32 state;
    CastleU32 enabled;
    CastleU32 period_ms;
    CastleU32 budget_ms;
    CastleU32 run_count;
    CastleU32 missed_count;
    CastleU32 over_budget_count;
    CastleU32 failure_count;
    CastleResult last_result;
    CastleU32 last_duration_ms;
    CastleU32 max_duration_ms;
    CastleU32 generation;
} CastleScheduleTaskStatsV1;

/* 游戏线程阶段仍未启用时，available=0，查询函数返回 OPTIONAL_UNAVAILABLE。 */
typedef struct CastleGamePhaseStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 available;
    CastleU32 generation;
    CastleU32 current_phase;
    CastleU32 game_thread_id;
} CastleGamePhaseStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterPeriodicTaskFn)(
    CastlePluginHandle plugin,
    const CastleScheduledTaskV1* task,
    CastleTaskHandle* out_task);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleSetPeriodicTaskEnabledFn)(
    CastleTaskHandle task,
    CastleU32 enabled);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleUnregisterPeriodicTaskFn)(
    CastleTaskHandle task);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetPeriodicTaskStatsFn)(
    CastleTaskHandle task,
    CastleScheduleTaskStatsV1* out_stats);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePostBackgroundTaskFn)(
    CastlePluginHandle plugin,
    const CastleScheduledTaskV1* task);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterGamePhaseCallbackFn)(
    CastlePluginHandle plugin,
    const CastleScheduledTaskV1* task,
    CastleTaskHandle* out_task);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePostGameThreadTaskFn)(
    CastlePluginHandle plugin,
    const CastleScheduledTaskV1* task);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetGamePhaseStateFn)(
    CastleGamePhaseStateV1* out_state);

/* 16 字节表头 + 8 个函数指针 = 48 字节。 */
typedef struct CastleScheduleApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleRegisterPeriodicTaskFn RegisterPeriodicTask;
    CastleSetPeriodicTaskEnabledFn SetPeriodicTaskEnabled;
    CastleUnregisterPeriodicTaskFn UnregisterPeriodicTask;
    CastleGetPeriodicTaskStatsFn GetPeriodicTaskStats;
    CastlePostBackgroundTaskFn PostBackgroundTask;
    CastleRegisterGamePhaseCallbackFn RegisterGamePhaseCallback;
    CastlePostGameThreadTaskFn PostGameThreadTask;
    CastleGetGamePhaseStateFn GetGamePhaseState;
} CastleScheduleApiV1;

#define CASTLE_SIZEOF_SCHEDULED_TASK_V1   48u
#define CASTLE_SIZEOF_SCHEDULE_STATS_V1   72u
#define CASTLE_SIZEOF_GAME_PHASE_STATE_V1 32u
#define CASTLE_SIZEOF_SCHEDULE_API_V1     48u

#pragma pack(pop)

#endif /* CASTLE_SCHEDULE_API_H */
