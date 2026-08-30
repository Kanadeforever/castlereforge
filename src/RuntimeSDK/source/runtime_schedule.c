#include "runtime_internal.h"

#define RUNTIME_SCHEDULE_MAX_PERIODIC 128u
#define RUNTIME_SCHEDULE_MAX_POSTED   128u
#define RUNTIME_SCHEDULE_MAX_WORK     \
    (RUNTIME_SCHEDULE_MAX_PERIODIC + RUNTIME_SCHEDULE_MAX_POSTED)
#define RUNTIME_SCHEDULE_ID_CAP       RUNTIME_PLUGIN_ID_CAP
#define RUNTIME_SCHEDULE_ISOLATE_AFTER 3u

/* 周期任务记录只由调度锁保护；回调执行时仅复制需要的字段到栈上。 */
typedef struct RuntimePeriodicTask {
    int used;
    CastleU32 generation;
    CastleTaskHandle handle;
    CastlePluginHandle plugin;
    CastleScheduledTaskFn callback;
    void* user_context;
    CastleU32 period_ms;
    CastleU32 budget_ms;
    CastleU32 phase;
    CastleU32 priority;
    CastleU32 enabled;
    CastleU32 running;
    CastleU32 state;
    CastleU32 next_due_ms;
    CastleU32 run_count;
    CastleU32 missed_count;
    CastleU32 over_budget_count;
    CastleU32 failure_count;
    CastleResult last_result;
    CastleU32 last_duration_ms;
    CastleU32 max_duration_ms;
    CastleU32 consecutive_issues;
    char plugin_id[RUNTIME_SCHEDULE_ID_CAP];
    CastleU32 plugin_id_length;
} RuntimePeriodicTask;

/* 一次性任务在工作线程取走前位于固定队列；取走后槽位立刻可以复用。 */
typedef struct RuntimePostedTask {
    int used;
    CastleU32 sequence;
    CastlePluginHandle plugin;
    CastleScheduledTaskFn callback;
    void* user_context;
    CastleU32 budget_ms;
    CastleU32 phase;
    CastleU32 priority;
    char plugin_id[RUNTIME_SCHEDULE_ID_CAP];
    CastleU32 plugin_id_length;
} RuntimePostedTask;

/* WorkItem 是回调前复制出的不可变快照，Runtime 持锁期间绝不调用 callback。 */
typedef struct RuntimeScheduleWorkItem {
    CastleU32 periodic;
    CastleU32 periodic_index;
    CastleU32 periodic_generation;
    CastleTaskHandle handle;
    CastleScheduledTaskFn callback;
    void* user_context;
    CastleU32 budget_ms;
    CastleU32 phase;
    CastleU32 priority;
    CastleU32 sequence;
    char plugin_id[RUNTIME_SCHEDULE_ID_CAP];
    CastleU32 plugin_id_length;
} RuntimeScheduleWorkItem;

static volatile LONG g_schedule_lock;
static RuntimePeriodicTask g_periodic_tasks[RUNTIME_SCHEDULE_MAX_PERIODIC];
static RuntimePostedTask g_posted_tasks[RUNTIME_SCHEDULE_MAX_POSTED];
static CastleU32 g_periodic_generation;
static CastleU32 g_post_sequence;
static HANDLE g_schedule_event;
static HANDLE g_schedule_thread;

static CastleResult CASTLE_RUNTIME_CALL schedule_register_periodic_(
    CastlePluginHandle plugin, const CastleScheduledTaskV1* task,
    CastleTaskHandle* out_task);
static CastleResult CASTLE_RUNTIME_CALL schedule_set_enabled_(CastleTaskHandle task,
                                                               CastleU32 enabled);
static CastleResult CASTLE_RUNTIME_CALL schedule_unregister_(CastleTaskHandle task);
static CastleResult CASTLE_RUNTIME_CALL schedule_get_stats_(CastleTaskHandle task,
    CastleScheduleTaskStatsV1* out_stats);
static CastleResult CASTLE_RUNTIME_CALL schedule_post_background_(
    CastlePluginHandle plugin, const CastleScheduledTaskV1* task);
static CastleResult CASTLE_RUNTIME_CALL schedule_register_game_phase_(
    CastlePluginHandle plugin, const CastleScheduledTaskV1* task,
    CastleTaskHandle* out_task);
static CastleResult CASTLE_RUNTIME_CALL schedule_post_game_(
    CastlePluginHandle plugin, const CastleScheduledTaskV1* task);
static CastleResult CASTLE_RUNTIME_CALL schedule_get_game_phase_(
    CastleGamePhaseStateV1* out_state);
static DWORD WINAPI schedule_thread_main_(LPVOID unused);

static const CastleScheduleApiV1 g_schedule_api = {
    CASTLE_SCHEDULE_API_MAGIC,
    CASTLE_SIZEOF_SCHEDULE_API_V1,
    CASTLE_SCHEDULE_API_VERSION_1,
    CASTLE_SCHEDULE_CAP_BACKGROUND,
    schedule_register_periodic_,
    schedule_set_enabled_,
    schedule_unregister_,
    schedule_get_stats_,
    schedule_post_background_,
    schedule_register_game_phase_,
    schedule_post_game_,
    schedule_get_game_phase_
};

static CastleU32 schedule_saturating_add_(CastleU32 value, CastleU32 addition) {
    CastleU32 maximum = (CastleU32)~0ul;
    if (maximum - value < addition) return maximum;
    return value + addition;
}

static CastleU32 schedule_saturating_increment_(CastleU32 value) {
    CastleU32 maximum = (CastleU32)~0ul;
    return value == maximum ? maximum : value + 1u;
}

/* 字典序比较用于同一到期点的最终稳定排序，不依赖插件实际加载顺序。 */
static int schedule_compare_text_(const char* left, CastleU32 left_length,
                                  const char* right, CastleU32 right_length) {
    CastleU32 index;
    CastleU32 shared = left_length < right_length ? left_length : right_length;
    for (index = 0u; index < shared; ++index) {
        CastleU8 left_value = (CastleU8)left[index];
        CastleU8 right_value = (CastleU8)right[index];
        if (left_value < right_value) return -1;
        if (left_value > right_value) return 1;
    }
    if (left_length < right_length) return -1;
    if (left_length > right_length) return 1;
    return 0;
}

static int schedule_copy_plugin_id_(CastlePluginHandle plugin, char* output,
                                    CastleU32* out_length) {
    CastleStringView id = Runtime_GetPluginIdView(plugin);
    CastleU32 index;
    if (!output || !out_length || !id.data || id.length == 0u ||
        id.length >= RUNTIME_SCHEDULE_ID_CAP) return 0;
    for (index = 0u; index < id.length; ++index) output[index] = id.data[index];
    output[id.length] = '\0';
    *out_length = id.length;
    return 1;
}

static int schedule_valid_descriptor_(const CastleScheduledTaskV1* task,
                                      int periodic) {
    if (!task || task->magic != CASTLE_SCHEDULE_TASK_MAGIC ||
        task->struct_size < CASTLE_SIZEOF_SCHEDULED_TASK_V1 ||
        task->version != CASTLE_SCHEDULE_STRUCTURE_VERSION_1 ||
        !task->callback || task->phase > CASTLE_SCHEDULE_PHASE_LATE ||
        task->priority > CASTLE_SCHEDULE_PRIORITY_LATE ||
        task->budget_ms > 60000u ||
        (task->label.length != 0u && !task->label.data)) return 0;
    if (periodic) return task->period_ms >= 1u && task->period_ms <= 60000u;
    return task->period_ms == 0u;
}

/* 句柄低 8 位是 index+1，高 24 位是代次；槽位复用后旧句柄必然失效。 */
static CastleTaskHandle schedule_make_handle_(CastleU32 index, CastleU32 generation) {
    return (generation << 8u) | (index + 1u);
}

static RuntimePeriodicTask* schedule_resolve_locked_(CastleTaskHandle handle,
                                                      CastleU32* out_index) {
    CastleU32 encoded_index = handle & 0xFFu;
    CastleU32 generation = handle >> 8u;
    CastleU32 index;
    RuntimePeriodicTask* task;
    if (encoded_index == 0u || encoded_index > RUNTIME_SCHEDULE_MAX_PERIODIC ||
        generation == 0u) return NULL;
    index = encoded_index - 1u;
    task = &g_periodic_tasks[index];
    if (!task->used || task->generation != generation || task->handle != handle) {
        return NULL;
    }
    if (out_index) *out_index = index;
    return task;
}

/* 创建动作只在调度锁内发生，保证整个进程最多拥有一个后台线程。 */
static int schedule_ensure_worker_locked_(void) {
    DWORD thread_id;
    if (g_schedule_thread && g_schedule_event) return 1;
    g_schedule_event = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!g_schedule_event) return 0;
    g_schedule_thread = CreateThread(NULL, 0u, schedule_thread_main_, NULL, 0u,
                                     &thread_id);
    if (!g_schedule_thread) {
        CloseHandle(g_schedule_event);
        g_schedule_event = NULL;
        return 0;
    }
    return 1;
}

static int schedule_work_before_(const RuntimeScheduleWorkItem* left,
                                 const RuntimeScheduleWorkItem* right) {
    int text_order;
    if (left->phase != right->phase) return left->phase < right->phase;
    if (left->priority != right->priority) return left->priority < right->priority;
    text_order = schedule_compare_text_(left->plugin_id, left->plugin_id_length,
                                        right->plugin_id, right->plugin_id_length);
    if (text_order != 0) return text_order < 0;
    return left->sequence < right->sequence;
}

static void schedule_sort_work_(RuntimeScheduleWorkItem* items, CastleU32 count) {
    CastleU32 index;
    for (index = 1u; index < count; ++index) {
        RuntimeScheduleWorkItem value = items[index];
        CastleU32 position = index;
        while (position > 0u && schedule_work_before_(&value, &items[position - 1u])) {
            items[position] = items[position - 1u];
            --position;
        }
        items[position] = value;
    }
}

static CastleU32 schedule_collect_work_(RuntimeScheduleWorkItem* work,
                                        CastleU32 now,
                                        DWORD* out_wait_ms) {
    CastleU32 count = 0u;
    CastleU32 index;
    DWORD wait_ms = INFINITE;

    Runtime_Lock(&g_schedule_lock);

    /* 一次性任务已经到期，按固定队列逐一复制并立即释放队列槽。 */
    for (index = 0u; index < RUNTIME_SCHEDULE_MAX_POSTED; ++index) {
        RuntimePostedTask* posted = &g_posted_tasks[index];
        RuntimeScheduleWorkItem* item;
        if (!posted->used || count >= RUNTIME_SCHEDULE_MAX_WORK) continue;
        item = &work[count++];
        Runtime_ByteZero(item, (CastleU32)sizeof(*item));
        item->periodic = 0u;
        item->callback = posted->callback;
        item->user_context = posted->user_context;
        item->budget_ms = posted->budget_ms;
        item->phase = posted->phase;
        item->priority = posted->priority;
        item->sequence = posted->sequence;
        Runtime_ByteCopy(item->plugin_id, posted->plugin_id,
                         posted->plugin_id_length + 1u);
        item->plugin_id_length = posted->plugin_id_length;
        Runtime_ByteZero(posted, (CastleU32)sizeof(*posted));
    }

    for (index = 0u; index < RUNTIME_SCHEDULE_MAX_PERIODIC; ++index) {
        RuntimePeriodicTask* task = &g_periodic_tasks[index];
        CastleU32 remaining;
        if (!task->used || !task->enabled || task->running) continue;
        if ((CastleS32)(now - task->next_due_ms) >= 0) {
            CastleU32 elapsed = now - task->next_due_ms;
            CastleU32 skipped = elapsed / task->period_ms;
            RuntimeScheduleWorkItem* item;
            if (count >= RUNTIME_SCHEDULE_MAX_WORK) break;
            item = &work[count++];
            Runtime_ByteZero(item, (CastleU32)sizeof(*item));
            item->periodic = 1u;
            item->periodic_index = index;
            item->periodic_generation = task->generation;
            item->handle = task->handle;
            item->callback = task->callback;
            item->user_context = task->user_context;
            item->budget_ms = task->budget_ms;
            item->phase = task->phase;
            item->priority = task->priority;
            item->sequence = task->handle;
            Runtime_ByteCopy(item->plugin_id, task->plugin_id,
                             task->plugin_id_length + 1u);
            item->plugin_id_length = task->plugin_id_length;
            task->missed_count = schedule_saturating_add_(task->missed_count, skipped);
            task->next_due_ms += (skipped + 1u) * task->period_ms;
            task->running = 1u;
            task->state = CASTLE_SCHEDULE_TASK_RUNNING;
            continue;
        }
        remaining = task->next_due_ms - now;
        if (wait_ms == INFINITE || remaining < wait_ms) wait_ms = remaining;
    }

    Runtime_Unlock(&g_schedule_lock);
    *out_wait_ms = count != 0u ? 0u : wait_ms;
    return count;
}

static void schedule_finish_periodic_(const RuntimeScheduleWorkItem* item,
                                      CastleResult result,
                                      CastleU32 duration) {
    RuntimePeriodicTask* task;
    int isolated = 0;
    Runtime_Lock(&g_schedule_lock);
    task = &g_periodic_tasks[item->periodic_index];
    if (task->used && task->generation == item->periodic_generation &&
        task->handle == item->handle) {
        int issue = 0;
        task->running = 0u;
        task->run_count = schedule_saturating_increment_(task->run_count);
        task->last_result = result;
        task->last_duration_ms = duration;
        if (duration > task->max_duration_ms) task->max_duration_ms = duration;
        if (result < 0) {
            task->failure_count = schedule_saturating_increment_(task->failure_count);
            issue = 1;
        }
        if (task->budget_ms != 0u && duration > task->budget_ms) {
            task->over_budget_count =
                schedule_saturating_increment_(task->over_budget_count);
            issue = 1;
        }
        task->consecutive_issues = issue ?
            schedule_saturating_increment_(task->consecutive_issues) : 0u;
        if (task->consecutive_issues >= RUNTIME_SCHEDULE_ISOLATE_AFTER) {
            task->enabled = 0u;
            task->state = CASTLE_SCHEDULE_TASK_FAULT;
            isolated = 1;
        } else {
            task->state = task->enabled ? CASTLE_SCHEDULE_TASK_WAITING :
                                          CASTLE_SCHEDULE_TASK_DISABLED;
        }
    }
    Runtime_Unlock(&g_schedule_lock);
    if (isolated) {
        Runtime_DiagnosticAppend("[Schedule] task isolated after repeated fault/budget overrun.");
    }
}

static DWORD WINAPI schedule_thread_main_(LPVOID unused) {
    RuntimeScheduleWorkItem work[RUNTIME_SCHEDULE_MAX_WORK];
    (void)unused;
    for (;;) {
        CastleU32 now = GetTickCount();
        DWORD wait_ms = INFINITE;
        CastleU32 count = schedule_collect_work_(work, now, &wait_ms);
        CastleU32 index;
        if (count == 0u) {
            WaitForSingleObject(g_schedule_event, wait_ms);
            continue;
        }
        schedule_sort_work_(work, count);
        for (index = 0u; index < count; ++index) {
            RuntimeScheduleWorkItem* item = &work[index];
            CastleU32 started = GetTickCount();
            CastleResult result = item->callback(item->handle, item->user_context);
            CastleU32 duration = GetTickCount() - started;
            if (item->periodic) schedule_finish_periodic_(item, result, duration);
            else if (result < 0 || (item->budget_ms != 0u && duration > item->budget_ms)) {
                Runtime_DiagnosticAppend("[Schedule] posted background task reported a fault/overrun.");
            }
        }
    }
}

void Runtime_ScheduleInitialize(void) {
    g_schedule_lock = 0;
    g_periodic_generation = 0u;
    g_post_sequence = 0u;
    g_schedule_event = NULL;
    g_schedule_thread = NULL;
    Runtime_ByteZero(g_periodic_tasks, (CastleU32)sizeof(g_periodic_tasks));
    Runtime_ByteZero(g_posted_tasks, (CastleU32)sizeof(g_posted_tasks));
}

const CastleScheduleApiV1* Runtime_GetScheduleApiV1(void) {
    return &g_schedule_api;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_register_periodic_(
    CastlePluginHandle plugin, const CastleScheduledTaskV1* descriptor,
    CastleTaskHandle* out_task) {
    CastleU32 index;
    CastleU32 id_length = 0u;
    char plugin_id[RUNTIME_SCHEDULE_ID_CAP];
    RuntimePeriodicTask* task = NULL;
    CastleTaskHandle handle;
    if (!out_task || !Runtime_GetPluginModule(plugin) ||
        !schedule_valid_descriptor_(descriptor, 1) ||
        !schedule_copy_plugin_id_(plugin, plugin_id, &id_length)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    Runtime_Lock(&g_schedule_lock);
    if (!schedule_ensure_worker_locked_()) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    for (index = 0u; index < RUNTIME_SCHEDULE_MAX_PERIODIC; ++index) {
        if (!g_periodic_tasks[index].used) {
            task = &g_periodic_tasks[index];
            break;
        }
    }
    if (!task) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_periodic_generation = (g_periodic_generation + 1u) & 0x00FFFFFFu;
    if (g_periodic_generation == 0u) g_periodic_generation = 1u;
    handle = schedule_make_handle_(index, g_periodic_generation);
    Runtime_ByteZero(task, (CastleU32)sizeof(*task));
    task->used = 1;
    task->generation = g_periodic_generation;
    task->handle = handle;
    task->plugin = plugin;
    task->callback = descriptor->callback;
    task->user_context = descriptor->user_context;
    task->period_ms = descriptor->period_ms;
    task->budget_ms = descriptor->budget_ms;
    task->phase = descriptor->phase;
    task->priority = descriptor->priority;
    task->enabled = (descriptor->flags & CASTLE_SCHEDULE_TASK_START_ENABLED) != 0u;
    task->state = task->enabled ? CASTLE_SCHEDULE_TASK_WAITING :
                                  CASTLE_SCHEDULE_TASK_DISABLED;
    task->next_due_ms = GetTickCount() + task->period_ms;
    task->last_result = CASTLE_OK;
    Runtime_ByteCopy(task->plugin_id, plugin_id, id_length + 1u);
    task->plugin_id_length = id_length;
    *out_task = handle;
    Runtime_Unlock(&g_schedule_lock);
    SetEvent(g_schedule_event);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_set_enabled_(CastleTaskHandle handle,
                                                               CastleU32 enabled) {
    RuntimePeriodicTask* task;
    if (enabled > 1u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_schedule_lock);
    task = schedule_resolve_locked_(handle, NULL);
    if (!task) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (task->state == CASTLE_SCHEDULE_TASK_FAULT && enabled != 0u) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_NOT_READY;
    }
    task->enabled = enabled;
    if (!task->running) {
        task->state = enabled ? CASTLE_SCHEDULE_TASK_WAITING :
                                CASTLE_SCHEDULE_TASK_DISABLED;
    }
    if (enabled) task->next_due_ms = GetTickCount() + task->period_ms;
    Runtime_Unlock(&g_schedule_lock);
    SetEvent(g_schedule_event);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_unregister_(CastleTaskHandle handle) {
    RuntimePeriodicTask* task;
    Runtime_Lock(&g_schedule_lock);
    task = schedule_resolve_locked_(handle, NULL);
    if (!task) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (task->running) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    Runtime_ByteZero(task, (CastleU32)sizeof(*task));
    Runtime_Unlock(&g_schedule_lock);
    SetEvent(g_schedule_event);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_get_stats_(CastleTaskHandle handle,
    CastleScheduleTaskStatsV1* out_stats) {
    RuntimePeriodicTask* task;
    if (!out_stats || out_stats->magic != CASTLE_SCHEDULE_STATS_MAGIC ||
        out_stats->struct_size < CASTLE_SIZEOF_SCHEDULE_STATS_V1 ||
        out_stats->version != CASTLE_SCHEDULE_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_schedule_lock);
    task = schedule_resolve_locked_(handle, NULL);
    if (!task) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    out_stats->flags = 0u;
    out_stats->task_handle = task->handle;
    out_stats->plugin_handle = task->plugin;
    out_stats->state = task->state;
    out_stats->enabled = task->enabled;
    out_stats->period_ms = task->period_ms;
    out_stats->budget_ms = task->budget_ms;
    out_stats->run_count = task->run_count;
    out_stats->missed_count = task->missed_count;
    out_stats->over_budget_count = task->over_budget_count;
    out_stats->failure_count = task->failure_count;
    out_stats->last_result = task->last_result;
    out_stats->last_duration_ms = task->last_duration_ms;
    out_stats->max_duration_ms = task->max_duration_ms;
    out_stats->generation = task->generation;
    Runtime_Unlock(&g_schedule_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_post_background_(
    CastlePluginHandle plugin, const CastleScheduledTaskV1* descriptor) {
    CastleU32 index;
    CastleU32 id_length = 0u;
    char plugin_id[RUNTIME_SCHEDULE_ID_CAP];
    RuntimePostedTask* task = NULL;
    if (!Runtime_GetPluginModule(plugin) || !schedule_valid_descriptor_(descriptor, 0) ||
        !schedule_copy_plugin_id_(plugin, plugin_id, &id_length)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_schedule_lock);
    if (!schedule_ensure_worker_locked_()) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    for (index = 0u; index < RUNTIME_SCHEDULE_MAX_POSTED; ++index) {
        if (!g_posted_tasks[index].used) {
            task = &g_posted_tasks[index];
            break;
        }
    }
    if (!task) {
        Runtime_Unlock(&g_schedule_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_post_sequence = schedule_saturating_increment_(g_post_sequence);
    Runtime_ByteZero(task, (CastleU32)sizeof(*task));
    task->used = 1;
    task->sequence = g_post_sequence;
    task->plugin = plugin;
    task->callback = descriptor->callback;
    task->user_context = descriptor->user_context;
    task->budget_ms = descriptor->budget_ms;
    task->phase = descriptor->phase;
    task->priority = descriptor->priority;
    Runtime_ByteCopy(task->plugin_id, plugin_id, id_length + 1u);
    task->plugin_id_length = id_length;
    Runtime_Unlock(&g_schedule_lock);
    SetEvent(g_schedule_event);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_register_game_phase_(
    CastlePluginHandle plugin, const CastleScheduledTaskV1* task,
    CastleTaskHandle* out_task) {
    if (!out_task || !Runtime_GetPluginModule(plugin) ||
        !schedule_valid_descriptor_(task, 1)) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_task = 0u;
    return CASTLE_STATUS_OPTIONAL_UNAVAILABLE;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_post_game_(
    CastlePluginHandle plugin, const CastleScheduledTaskV1* task) {
    if (!Runtime_GetPluginModule(plugin) || !schedule_valid_descriptor_(task, 0)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    return CASTLE_STATUS_OPTIONAL_UNAVAILABLE;
}

static CastleResult CASTLE_RUNTIME_CALL schedule_get_game_phase_(
    CastleGamePhaseStateV1* out_state) {
    if (!out_state || out_state->magic != CASTLE_GAME_PHASE_STATE_MAGIC ||
        out_state->struct_size < CASTLE_SIZEOF_GAME_PHASE_STATE_V1 ||
        out_state->version != CASTLE_SCHEDULE_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    out_state->flags = 0u;
    out_state->available = 0u;
    out_state->generation = 0u;
    out_state->current_phase = 0u;
    out_state->game_thread_id = 0u;
    return CASTLE_STATUS_OPTIONAL_UNAVAILABLE;
}
