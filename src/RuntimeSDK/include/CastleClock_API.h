#ifndef CASTLE_CLOCK_API_H
#define CASTLE_CLOCK_API_H

/*
 * CastleClock_API.h
 *
 * Clock v1 统一管理 FPSUnlock 所需的 1ms Windows 计时器精度。timeBeginPeriod/timeEndPeriod
 * 是成对的进程级资源；如果多个插件各自调用，任何一个插件提前结束都会破坏其它插件。
 * Runtime 因而给每个请求分配租约，只有最后一个租约释放时才真正结束 1ms 精度。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_CLOCK_INTERFACE_ID "org.castlereforge.runtime.clock"
#define CASTLE_CLOCK_API_VERSION_1 1u
#define CASTLE_CLOCK_STRUCTURE_VERSION_1 1u

#define CASTLE_CLOCK_API_MAGIC   0x4B4C4343ul /* CCLK */
#define CASTLE_CLOCK_STATE_MAGIC 0x54534C43ul /* CLST */

#define CASTLE_CLOCK_CAP_MONOTONIC_MS       (1ul << 0)
#define CASTLE_CLOCK_CAP_TIMER_RESOLUTION_1 (1ul << 1)

typedef struct CastleClockStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 backend_ready;
    CastleU32 active_resolution_ms;
    CastleU32 active_lease_count;
    CastleU32 generation;
} CastleClockStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetMonotonicMillisecondsFn)(
    CastleU32* out_milliseconds);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleAcquireTimerResolutionFn)(
    CastlePluginHandle plugin,
    CastleU32 requested_milliseconds,
    CastleLeaseHandle* out_lease);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleReleaseTimerResolutionFn)(
    CastleLeaseHandle lease);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetTimerResolutionStateFn)(
    CastleClockStateV1* out_state);

/* 16 字节表头 + 4 个函数指针 = 32 字节。 */
typedef struct CastleClockApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleGetMonotonicMillisecondsFn GetMonotonicMilliseconds;
    CastleAcquireTimerResolutionFn AcquireTimerResolution;
    CastleReleaseTimerResolutionFn ReleaseTimerResolution;
    CastleGetTimerResolutionStateFn GetTimerResolutionState;
} CastleClockApiV1;

#define CASTLE_SIZEOF_CLOCK_STATE_V1 32u
#define CASTLE_SIZEOF_CLOCK_API_V1   32u

#pragma pack(pop)

#endif /* CASTLE_CLOCK_API_H */
