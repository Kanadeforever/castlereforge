#include "runtime_internal.h"

#define RUNTIME_CLOCK_MAX_LEASES 128u

typedef CastleU32 (WINAPI *RuntimeTimeGetTimeFn)(void);
typedef CastleU32 (WINAPI *RuntimeTimePeriodFn)(CastleU32 period);

typedef struct RuntimeClockLease {
    int used;
    CastleLeaseHandle handle;
    CastlePluginHandle plugin;
} RuntimeClockLease;

static volatile LONG g_clock_lock;
static RuntimeClockLease g_clock_leases[RUNTIME_CLOCK_MAX_LEASES];
static CastleU32 g_clock_next_handle;
static CastleU32 g_clock_generation;
static CastleU32 g_clock_active_count;
static HMODULE g_clock_winmm;
static RuntimeTimeGetTimeFn g_clock_time_get_time;
static RuntimeTimePeriodFn g_clock_time_begin_period;
static RuntimeTimePeriodFn g_clock_time_end_period;
static int g_clock_backend_attempted;
static int g_clock_backend_ready;

static CastleResult CASTLE_RUNTIME_CALL clock_get_ms_(CastleU32* out_milliseconds);
static CastleResult CASTLE_RUNTIME_CALL clock_acquire_(
    CastlePluginHandle plugin, CastleU32 requested_milliseconds,
    CastleLeaseHandle* out_lease);
static CastleResult CASTLE_RUNTIME_CALL clock_release_(CastleLeaseHandle lease);
static CastleResult CASTLE_RUNTIME_CALL clock_get_state_(CastleClockStateV1* out_state);

static const CastleClockApiV1 g_clock_api = {
    CASTLE_CLOCK_API_MAGIC,
    CASTLE_SIZEOF_CLOCK_API_V1,
    CASTLE_CLOCK_API_VERSION_1,
    CASTLE_CLOCK_CAP_MONOTONIC_MS | CASTLE_CLOCK_CAP_TIMER_RESOLUTION_1,
    clock_get_ms_,
    clock_acquire_,
    clock_release_,
    clock_get_state_
};

/*
 * GetProcAddress 返回 FARPROC。这里逐字节复制函数地址位模式，不使用不兼容函数类型强转，
 * 因而同一源码在 MSVC 与 clang-cl 的 /WX 下都不会触发 cast-function-type 警告。
 */
static int clock_copy_proc_(void* output, CastleU32 output_size, FARPROC address) {
    if (!output || output_size != (CastleU32)sizeof(address) || !address) return 0;
    Runtime_ByteCopy(output, &address, output_size);
    return 1;
}

static int clock_resolve_backend_locked_(void) {
    FARPROC address;
    if (g_clock_backend_attempted) return g_clock_backend_ready;
    g_clock_backend_attempted = 1;

    /* winmm 是 Windows 系统组件；Runtime 持有模块引用直到进程退出，不在热路径卸载。 */
    g_clock_winmm = LoadLibraryW(L"winmm.dll");
    if (!g_clock_winmm) return 0;

    address = GetProcAddress(g_clock_winmm, "timeGetTime");
    if (!clock_copy_proc_(&g_clock_time_get_time,
                          (CastleU32)sizeof(g_clock_time_get_time), address)) return 0;
    address = GetProcAddress(g_clock_winmm, "timeBeginPeriod");
    if (!clock_copy_proc_(&g_clock_time_begin_period,
                          (CastleU32)sizeof(g_clock_time_begin_period), address)) return 0;
    address = GetProcAddress(g_clock_winmm, "timeEndPeriod");
    if (!clock_copy_proc_(&g_clock_time_end_period,
                          (CastleU32)sizeof(g_clock_time_end_period), address)) return 0;

    g_clock_backend_ready = 1;
    return 1;
}

void Runtime_ClockInitialize(void) {
    g_clock_lock = 0;
    Runtime_ByteZero(g_clock_leases, (CastleU32)sizeof(g_clock_leases));
    g_clock_next_handle = 1u;
    g_clock_generation = 1u;
    g_clock_active_count = 0u;
    g_clock_winmm = NULL;
    g_clock_time_get_time = NULL;
    g_clock_time_begin_period = NULL;
    g_clock_time_end_period = NULL;
    g_clock_backend_attempted = 0;
    g_clock_backend_ready = 0;
}

const CastleClockApiV1* Runtime_GetClockApiV1(void) {
    return &g_clock_api;
}

static CastleLeaseHandle clock_allocate_handle_locked_(void) {
    CastleLeaseHandle candidate = g_clock_next_handle++;
    if (candidate == 0u) candidate = g_clock_next_handle++;
    return candidate;
}

static CastleResult CASTLE_RUNTIME_CALL clock_get_ms_(CastleU32* out_milliseconds) {
    RuntimeTimeGetTimeFn time_get_time = NULL;
    if (!out_milliseconds) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_clock_lock);
    if (clock_resolve_backend_locked_()) time_get_time = g_clock_time_get_time;
    Runtime_Unlock(&g_clock_lock);
    *out_milliseconds = time_get_time ? time_get_time() : (CastleU32)GetTickCount();
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL clock_acquire_(
    CastlePluginHandle plugin, CastleU32 requested_milliseconds,
    CastleLeaseHandle* out_lease) {
    CastleU32 index;
    CastleLeaseHandle handle;
    if (!out_lease || requested_milliseconds != 1u ||
        !Runtime_GetPluginModule(plugin)) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_lease = 0u;

    Runtime_Lock(&g_clock_lock);
    if (!clock_resolve_backend_locked_()) {
        Runtime_Unlock(&g_clock_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    for (index = 0u; index < RUNTIME_CLOCK_MAX_LEASES; ++index) {
        if (!g_clock_leases[index].used) break;
    }
    if (index == RUNTIME_CLOCK_MAX_LEASES) {
        Runtime_Unlock(&g_clock_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }

    /* 第一个申请者才真正改变 Windows 计时器；失败时不创建半个租约。 */
    if (g_clock_active_count == 0u && g_clock_time_begin_period(1u) != 0u) {
        Runtime_Unlock(&g_clock_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    handle = clock_allocate_handle_locked_();
    g_clock_leases[index].used = 1;
    g_clock_leases[index].handle = handle;
    g_clock_leases[index].plugin = plugin;
    ++g_clock_active_count;
    ++g_clock_generation;
    *out_lease = handle;
    Runtime_Unlock(&g_clock_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL clock_release_(CastleLeaseHandle lease) {
    CastleU32 index;
    CastleResult result = CASTLE_ERROR_INVALID_ARGUMENT;
    if (lease == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_clock_lock);
    for (index = 0u; index < RUNTIME_CLOCK_MAX_LEASES; ++index) {
        if (g_clock_leases[index].used && g_clock_leases[index].handle == lease) {
            g_clock_leases[index].used = 0;
            g_clock_leases[index].handle = 0u;
            g_clock_leases[index].plugin = 0u;
            if (g_clock_active_count > 0u) --g_clock_active_count;
            if (g_clock_active_count == 0u && g_clock_time_end_period &&
                g_clock_time_end_period(1u) != 0u) {
                result = CASTLE_ERROR_RUNTIME_FAULT;
            } else {
                result = CASTLE_OK;
            }
            ++g_clock_generation;
            break;
        }
    }
    Runtime_Unlock(&g_clock_lock);
    return result;
}

static CastleResult CASTLE_RUNTIME_CALL clock_get_state_(CastleClockStateV1* out_state) {
    if (!out_state || out_state->magic != CASTLE_CLOCK_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_CLOCK_STATE_V1 ||
        out_state->version != CASTLE_CLOCK_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_clock_lock);
    out_state->flags = 0u;
    out_state->backend_ready = clock_resolve_backend_locked_() ? 1u : 0u;
    out_state->active_resolution_ms = g_clock_active_count ? 1u : 0u;
    out_state->active_lease_count = g_clock_active_count;
    out_state->generation = g_clock_generation;
    Runtime_Unlock(&g_clock_lock);
    return out_state->backend_ready ? CASTLE_OK : CASTLE_ERROR_RUNTIME_FAULT;
}
