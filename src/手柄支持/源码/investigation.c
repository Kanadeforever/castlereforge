#include "investigation.h"
#include "runtime.h"
#include "game_addresses.h"
#include "pad_input.h"
#include "input_router.h"
#include "cursor.h"

/*
 * 本模块的安全边界：
 *
 * 1. 0x409B50 仍是“是否真的可互动”的唯一裁判；Hook 必须先调用原函数，
 *    必须原样返回结果，绝不替 RPG.exe 发明新的互动判定。
 * 2. scene/object/collision/frame 都只能在 resolver 所在游戏线程读取；
 *    8ms worker 线程只能读取下面的值快照，不能保存后再解引用游戏指针。
 * 3. type 1..4 的距离门和 type>=5 的绕过语义来自原版机器码；
 *    这里复制的是候选过滤条件，不是最终命中结论。
 * 4. 640x480 是游戏逻辑客户区。候选矩形先裁切到这个空间，
 *    Cursor 层再负责 client/screen 坐标转换，避免窗口位置污染算法。
 * 5. Sprite 中心可能透明，所以硬切/吸附只先选 5x5 候选像素；
 *    每次 warp 后都等待下一次 resolver 快照验证，未验证时绝不点击。
 * 6. 左杆是一次离开死区只切换一个方向目标；回中以后才重新武装，
 *    防止 worker 高频循环让焦点在多个近邻之间连续跳动。
 * 7. 右杆保留独立的百分之一像素余量，用于很低速度的精细移动；
 *    只有进入短吸附半径后才尝试候选，不做大范围磁吸。
 * 8. A 若遇到尚未验证的候选，只设置 pending_click；原版确认 hover 后，
 *    才通过 Cursor 的 48ms 左键桥提交一次调查。
 * 9. 本模块只执行已经获准的调查会话；Back/RT/LT 优先级、自由地图门与
 *    CaptureAll 全部由 ControlModes 统一裁决，避免这里再次猜测菜单状态。
 * 10. 震动只由 resolver 报告的新 hovered_object 触发一次；离开目标后重置，
 *     SDL_RumbleGamepad 缺失则静默退化，不影响任何输入或原版键鼠。
 * 11. 任一协议签名、CALL 目标或指针合理性检查失败都发布空结果，
 *     或只关闭本能力；r36 已验收菜单与战斗底座不能被连坐。
 * 12. 快照 sequence 使用单写者/单读者协议：奇数正在写，偶数完整；
 *     reader 必须在复制前后看到同一偶数，才可以使用这一帧数据。
 * 13. 调查命中震动只提交低优先级请求，不能覆盖仍在播放的“常驻鼠标回手柄”反馈。
 * 14. 震动强度读取全局 Rumble.StrengthPercent；本模块只拥有调查命中的独立时长。
 */

#define INVESTIGATION_SCREEN_WIDTH  640
#define INVESTIGATION_SCREEN_HEIGHT 480
#define INVESTIGATION_MAX_TARGETS   96
#define INVESTIGATION_CANDIDATES    25
#define INVESTIGATION_FRESH_TICKS   8u

typedef struct InvestigationTarget {
    u32 object;
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;
    i32 center_x;
    i32 center_y;
    u8 interaction_type;
} InvestigationTarget;

typedef struct InvestigationSnapshot {
    u32 tick;
    u32 serial;
    u32 hovered_object;
    int target_count;
    InvestigationTarget targets[INVESTIGATION_MAX_TARGETS];
} InvestigationSnapshot;

typedef struct InvestigationState {
    int enabled;
    int active;
    int left_stick_latched;

    u32 pending_object;
    int pending_candidate_start;
    int pending_candidate_step;
    u32 pending_snapshot_serial;
    int pending_click;

    u32 last_rumble_object;
} InvestigationState;

/*
 * game thread 是唯一写者，worker 是唯一读者。sequence 奇数表示正在发布，偶数表示完整；
 * worker 只在前后序号一致时接受快照，从不在 worker 解引用游戏场景指针。
 */
static volatile u32 g_snapshot_sequence;
static volatile InvestigationSnapshot g_published_snapshot;
static InvestigationState g_investigation;

/* 5x5 候选点从中心向外扩展；透明中心失败时，下一个游戏帧继续试周围点。 */
static const u8 g_candidate_grid_x[INVESTIGATION_CANDIDATES] = {
    2,1,3,2,2, 1,3,1,3, 0,4,2,2, 0,0,4,4,1,3,1,3, 0,4,0,4
};
static const u8 g_candidate_grid_y[INVESTIGATION_CANDIDATES] = {
    2,2,2,1,3, 1,1,3,3, 2,2,0,4, 1,3,1,3,0,0,4,4, 0,0,4,4
};

static int inv_abs(int value) { return value < 0 ? -value : value; }

static int inv_clamp(int value, int minimum, int maximum) {
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

/* type 1..4 严格小于 100；type>=5 完全沿用原版“绕过距离门”语义。 */
static int inv_original_distance_eligible(const u8* object, const u8* actor, u8 type) {
    i32 dx;
    i32 dy;
    i32 distance_squared;

    if (type == 0u) return 0;
    if (type >= 5u) return 1;
    if (!Runtime_PtrOk(object) || !Runtime_PtrOk(actor)) return 0;

    dx = *(const i32*)(object + 0x10u) - *(const i32*)(actor + 0x10u);
    dy = *(const i32*)(object + 0x14u) - *(const i32*)(actor + 0x14u);
    distance_squared = dx * dx + dy * dy;
    return distance_squared < 10000;
}

/*
 * 从原版 collision/frame 几何生成 640x480 屏幕矩形。frame+0x54==0 时原版 0x4080D0
 * 也会立即返回未命中，因此这里先执行同一像素遮罩存在门；真正的透明像素
 * 仍交给下一帧原版 resolver 验证，插件不复制原版像素碰撞算法。
 */
static int inv_build_target(const u8* object, const u8* actor,
                            i32 camera_x, i32 camera_y,
                            InvestigationTarget* out) {
    const u8* record;
    const u8* collision;
    const u8* frame;
    u8 type;
    i32 left;
    i32 top;
    i32 width;
    i32 height;
    i32 right;
    i32 bottom;

    if (!Runtime_PtrOk(object) || !out) return 0;
    record = *(const u8* const*)(object + 0x70u);
    collision = *(const u8* const*)(object + 0x6Cu);
    if (!Runtime_PtrOk(record) || !Runtime_PtrOk(collision)) return 0;

    type = *(const u8*)(record + 0x73u);
    if (!inv_original_distance_eligible(object, actor, type)) return 0;

    frame = *(const u8* const*)(collision + 0x48u);
    if (!Runtime_PtrOk(frame) || *(const u16*)(frame + 0x54u) == 0u) return 0;

    width = *(const i32*)(frame + 0x08u);
    height = *(const i32*)(frame + 0x0Cu);
    if (width <= 0 || height <= 0 || width > 4096 || height > 4096) return 0;

    left = *(const i32*)(collision + 0x00u) + *(const i32*)(frame + 0x00u) - camera_x;
    top = *(const i32*)(collision + 0x04u) + *(const i32*)(frame + 0x04u) - camera_y;
    right = left + width;
    bottom = top + height;

    if (right <= 0 || bottom <= 0 || left >= INVESTIGATION_SCREEN_WIDTH ||
        top >= INVESTIGATION_SCREEN_HEIGHT) return 0;

    left = inv_clamp(left, 0, INVESTIGATION_SCREEN_WIDTH - 1);
    top = inv_clamp(top, 0, INVESTIGATION_SCREEN_HEIGHT - 1);
    right = inv_clamp(right, left + 1, INVESTIGATION_SCREEN_WIDTH);
    bottom = inv_clamp(bottom, top + 1, INVESTIGATION_SCREEN_HEIGHT);

    out->object = (u32)object;
    out->left = left;
    out->top = top;
    out->right = right;
    out->bottom = bottom;
    out->center_x = left + (right - left) / 2;
    out->center_y = top + (bottom - top) / 2;
    out->interaction_type = type;
    return 1;
}

/* 原版 resolver 完整执行后，在同一游戏线程中发布“当前真正可互动”快照。 */
static void inv_publish_snapshot(void* scene_ptr, i32 resolver_count, i32 original_result) {
    u8* scene = (u8*)scene_ptr;
    u8** objects;
    u8** sorted;
    u8* actor;
    u8* mouse_proxy;
    i32 object_count;
    i32 sorted_count;
    i32 camera_x;
    i32 camera_y;
    int i;
    int output_count = 0;
    u32 original_hover = 0u;

    ++g_snapshot_sequence; /* odd: writer active */
    g_published_snapshot.tick = Runtime_Tick();
    g_published_snapshot.hovered_object = 0u;
    g_published_snapshot.target_count = 0;

    if (!Runtime_PtrOk(scene)) goto publish_done;
    object_count = *(i32*)(scene + 0x1Cu);
    sorted_count = *(i32*)(scene + 0x20u);
    objects = *(u8***)(scene + 0x2Cu);
    sorted = *(u8***)(scene + 0x30u);
    if (object_count < 0 || object_count > 1024 || sorted_count < 0 || sorted_count > 1026 ||
        !Runtime_PtrOk(objects) || !Runtime_PtrOk(sorted)) goto publish_done;

    actor = objects[object_count];
    mouse_proxy = objects[object_count + 1];
    if (!Runtime_PtrOk(actor)) goto publish_done;

    /* CALL 实参就是 scene+0x20；两者不一致时宁可发布空快照，不猜测改版结构。 */
    if (resolver_count != sorted_count) goto publish_done;
    if (original_result >= 0 && original_result < sorted_count && Runtime_PtrOk(sorted[original_result])) {
        original_hover = (u32)sorted[original_result];
    }

    camera_x = *(const i32*)GLOBAL_MAP_CAMERA_X;
    camera_y = *(const i32*)GLOBAL_MAP_CAMERA_Y;

    for (i = 0; i < sorted_count && output_count < INVESTIGATION_MAX_TARGETS; ++i) {
        u8* object = sorted[i];
        InvestigationTarget target;
        int duplicate = 0;
        int j;

        if (!Runtime_PtrOk(object) || object == actor || object == mouse_proxy) continue;
        if (!inv_build_target(object, actor, camera_x, camera_y, &target)) continue;

        for (j = 0; j < output_count; ++j) {
            if (g_published_snapshot.targets[j].object == target.object) {
                duplicate = 1;
                break;
            }
        }
        if (duplicate) continue;

        /* 逐字段发布，保持无 CRT 构建不会因为结构赋值引入 memcpy。 */
        g_published_snapshot.targets[output_count].object = target.object;
        g_published_snapshot.targets[output_count].left = target.left;
        g_published_snapshot.targets[output_count].top = target.top;
        g_published_snapshot.targets[output_count].right = target.right;
        g_published_snapshot.targets[output_count].bottom = target.bottom;
        g_published_snapshot.targets[output_count].center_x = target.center_x;
        g_published_snapshot.targets[output_count].center_y = target.center_y;
        g_published_snapshot.targets[output_count].interaction_type = target.interaction_type;
        ++output_count;
    }

    g_published_snapshot.target_count = output_count;
    for (i = 0; i < output_count; ++i) {
        if (g_published_snapshot.targets[i].object == original_hover) {
            g_published_snapshot.hovered_object = original_hover;
            break;
        }
    }

publish_done:
    g_published_snapshot.serial = g_snapshot_sequence + 1u;
    ++g_snapshot_sequence; /* even: complete */
}

static i32 THISCALL Investigation_HookResolve(void* scene, i32 count) {
    PFN_ExplorationTargetResolve original =
        (PFN_ExplorationTargetResolve)FN_EXPLORATION_TARGET_RESOLVE;
    i32 result = original(scene, count);
    inv_publish_snapshot(scene, count, result);
    return result;
}

/* 不用 struct 整体赋值，避免无 CRT 构建意外生成 memcpy 外部依赖。 */
static int inv_read_snapshot(InvestigationSnapshot* out) {
    int attempt;

    if (!out) return 0;
    for (attempt = 0; attempt < 3; ++attempt) {
        u32 before = g_snapshot_sequence;
        int count;
        int i;
        u32 after;

        if (before & 1u) continue;
        out->tick = g_published_snapshot.tick;
        out->serial = g_published_snapshot.serial;
        out->hovered_object = g_published_snapshot.hovered_object;
        count = g_published_snapshot.target_count;
        if (count < 0) count = 0;
        if (count > INVESTIGATION_MAX_TARGETS) count = INVESTIGATION_MAX_TARGETS;
        out->target_count = count;
        for (i = 0; i < count; ++i) {
            out->targets[i].object = g_published_snapshot.targets[i].object;
            out->targets[i].left = g_published_snapshot.targets[i].left;
            out->targets[i].top = g_published_snapshot.targets[i].top;
            out->targets[i].right = g_published_snapshot.targets[i].right;
            out->targets[i].bottom = g_published_snapshot.targets[i].bottom;
            out->targets[i].center_x = g_published_snapshot.targets[i].center_x;
            out->targets[i].center_y = g_published_snapshot.targets[i].center_y;
            out->targets[i].interaction_type = g_published_snapshot.targets[i].interaction_type;
        }
        after = g_snapshot_sequence;
        if (before == after && !(after & 1u)) {
            out->serial = after;
            return 1;
        }
    }
    return 0;
}

static int inv_snapshot_fresh(const InvestigationSnapshot* snapshot) {
    if (!snapshot) return 0;
    return (Runtime_Tick() - snapshot->tick) <= INVESTIGATION_FRESH_TICKS;
}

static const InvestigationTarget* inv_find_target(const InvestigationSnapshot* snapshot,
                                                   u32 object) {
    int i;
    if (!snapshot || !object) return NULL;
    for (i = 0; i < snapshot->target_count; ++i) {
        if (snapshot->targets[i].object == object) return &snapshot->targets[i];
    }
    return NULL;
}

static void inv_candidate_point(const InvestigationTarget* target, int candidate,
                                i32* out_x, i32* out_y) {
    int index = candidate % INVESTIGATION_CANDIDATES;
    int width = target->right - target->left;
    int height = target->bottom - target->top;
    i32 x = target->left + ((int)g_candidate_grid_x[index] * 2 + 1) * width / 10;
    i32 y = target->top + ((int)g_candidate_grid_y[index] * 2 + 1) * height / 10;

    if (x >= target->right) x = target->right - 1;
    if (y >= target->bottom) y = target->bottom - 1;
    if (x < target->left) x = target->left;
    if (y < target->top) y = target->top;
    *out_x = x;
    *out_y = y;
}

static int inv_nearest_candidate(const InvestigationTarget* target, i32 x, i32 y) {
    int best = 0;
    i32 best_distance = 0x7FFFFFFF;
    int i;

    for (i = 0; i < INVESTIGATION_CANDIDATES; ++i) {
        i32 candidate_x;
        i32 candidate_y;
        i32 dx;
        i32 dy;
        i32 distance;
        inv_candidate_point(target, i, &candidate_x, &candidate_y);
        dx = candidate_x - x;
        dy = candidate_y - y;
        distance = dx * dx + dy * dy;
        if (distance < best_distance) {
            best_distance = distance;
            best = i;
        }
    }
    return best;
}

static void inv_cancel_probe(void) {
    g_investigation.pending_object = 0u;
    g_investigation.pending_candidate_start = 0;
    g_investigation.pending_candidate_step = 0;
    g_investigation.pending_snapshot_serial = 0u;
    g_investigation.pending_click = 0;
}

static int inv_warp_pending_candidate(const InvestigationSnapshot* snapshot) {
    const InvestigationTarget* target;
    int candidate;
    i32 x;
    i32 y;

    target = inv_find_target(snapshot, g_investigation.pending_object);
    if (!target || g_investigation.pending_candidate_step >= INVESTIGATION_CANDIDATES) return 0;
    candidate = (g_investigation.pending_candidate_start +
                 g_investigation.pending_candidate_step) % INVESTIGATION_CANDIDATES;
    inv_candidate_point(target, candidate, &x, &y);
    if (!Cursor_MoveControllerAt(x, y)) return 0;
    g_investigation.pending_snapshot_serial = snapshot->serial;
    Cursor_SetInvestigationSession(1);
    return 1;
}

static void inv_begin_probe(const InvestigationSnapshot* snapshot,
                            const InvestigationTarget* target,
                            int start_candidate) {
    if (!snapshot || !target) return;
    inv_cancel_probe();
    g_investigation.pending_object = target->object;
    g_investigation.pending_candidate_start = start_candidate;
    if (!inv_warp_pending_candidate(snapshot)) inv_cancel_probe();
}

/* 每次 warp 后必须等原版 resolver 真正跑过新坐标，才接受或尝试下一颗像素点。 */
static void inv_progress_probe(const InvestigationSnapshot* snapshot) {
    if (!g_investigation.pending_object || !snapshot) return;
    if (snapshot->serial == g_investigation.pending_snapshot_serial) return;

    if (snapshot->hovered_object == g_investigation.pending_object) {
        int click = g_investigation.pending_click;
        inv_cancel_probe();
        if (click) Cursor_PulseLeftClick();
        return;
    }

    ++g_investigation.pending_candidate_step;
    if (!inv_warp_pending_candidate(snapshot)) inv_cancel_probe();
}

/* 左摇杆只在每次“离开死区”时硬切一次；持续推住不会每 8ms 重复跳目标。 */
static const InvestigationTarget* inv_left_stick_target(const InvestigationSnapshot* snapshot,
                                                        i32 pointer_x, i32 pointer_y) {
    i32 axis_x = (i32)PadInput_Axis(PAD_AXIS_LEFT_X);
    i32 axis_y = (i32)PadInput_Axis(PAD_AXIS_LEFT_Y);
    const InvestigationTarget* best = NULL;
    i32 best_distance = 0x7FFFFFFF;
    int i;

    if (inv_abs(axis_x) < PAD_STICK_DEADZONE && inv_abs(axis_y) < PAD_STICK_DEADZONE) {
        g_investigation.left_stick_latched = 0;
        return NULL;
    }
    if (g_investigation.left_stick_latched) return NULL;
    g_investigation.left_stick_latched = 1;

    for (i = 0; i < snapshot->target_count; ++i) {
        const InvestigationTarget* target = &snapshot->targets[i];
        i32 dx;
        i32 dy;
        i32 dot;
        i32 cross;
        i32 distance;

        if (target->object == snapshot->hovered_object) continue;
        dx = target->center_x - pointer_x;
        dy = target->center_y - pointer_y;
        dot = dx * axis_x + dy * axis_y;
        if (dot <= 0) continue;
        cross = dx * axis_y - dy * axis_x;
        if (inv_abs(cross) > dot) continue; /* 推杆方向两侧各45度，总共90度扇区。 */

        distance = dx * dx + dy * dy;
        if (distance < best_distance) {
            best_distance = distance;
            best = target;
        }
    }
    return best;
}

static const InvestigationTarget* inv_right_stick_snap_target(
    const InvestigationSnapshot* snapshot, i32 pointer_x, i32 pointer_y) {
    i32 radius = (i32)Runtime_Config()->investigation_snap_radius_pixels;
    i32 radius_squared = radius * radius;
    i32 best_distance = radius_squared + 1;
    const InvestigationTarget* best = NULL;
    int i;

    for (i = 0; i < snapshot->target_count; ++i) {
        const InvestigationTarget* target = &snapshot->targets[i];
        i32 nearest_x = inv_clamp(pointer_x, target->left, target->right - 1);
        i32 nearest_y = inv_clamp(pointer_y, target->top, target->bottom - 1);
        i32 dx = nearest_x - pointer_x;
        i32 dy = nearest_y - pointer_y;
        i32 distance = dx * dx + dy * dy;
        if (distance <= radius_squared && distance < best_distance) {
            best_distance = distance;
            best = target;
        }
    }
    return best;
}

void Investigation_EndSession(void) {
    if (g_investigation.active) {
        g_investigation.active = 0;
        g_investigation.left_stick_latched = 0;
        inv_cancel_probe();
    }
    Cursor_SetInvestigationSession(0);
}

int Investigation_InstallHooks(void) {
    if (!Runtime_InvestigationProtocolOk()) {
        g_investigation.enabled = 0;
        Runtime_Log("[调查] 原版协议不匹配；只禁用LT探索调查，r36已验收功能继续运行。");
        return 1;
    }
    if (!Runtime_PatchCall(CALL_EXPLORATION_TARGET_RESOLVE,
                           (void*)Investigation_HookResolve,
                           FN_EXPLORATION_TARGET_RESOLVE)) {
        g_investigation.enabled = 0;
        Runtime_Log("[调查] resolver CALL 安装时已变化；本能力单独 fail-closed。");
        return 1;
    }

    g_investigation.enabled = 1;
    Runtime_Log("[调查] LT独立能力已安装：640x480可互动快照、左杆方向硬切、右杆低速短吸附、A原版左键。");
    return 1;
}

int Investigation_MapSnapshotReady(void) {
    InvestigationSnapshot snapshot;

    if (!g_investigation.enabled || !PadInput_GameForeground(NULL)) return 0;
    return inv_read_snapshot(&snapshot) && inv_snapshot_fresh(&snapshot);
}

int Investigation_UpdateActive(void) {
    InvestigationSnapshot snapshot;
    int have_snapshot;
    int right_moved;
    i32 pointer_x;
    i32 pointer_y;

    if (!g_investigation.enabled || !PadInput_GameForeground(NULL)) {
        Investigation_EndSession();
        return 0;
    }

    have_snapshot = inv_read_snapshot(&snapshot) && inv_snapshot_fresh(&snapshot);
    if (!have_snapshot) {
        Investigation_EndSession();
        return 0;
    }

    if (!g_investigation.active) {
        g_investigation.active = 1;
        g_investigation.left_stick_latched = 0;
        inv_cancel_probe();
    }
    Cursor_SetInvestigationSession(1);

    /* 右杆自由移动优先：玩家一旦主动精调，就取消上一次尚未验证的硬切候选。 */
    right_moved = Cursor_MoveInvestigationRightStick();
    if (right_moved) inv_cancel_probe();

    if (!Cursor_GetPointerPosition(&pointer_x, &pointer_y)) {
        return 1;
    }

    if (right_moved) {
        const InvestigationTarget* snap_target =
            inv_right_stick_snap_target(&snapshot, pointer_x, pointer_y);
        if (snap_target) {
            inv_begin_probe(&snapshot, snap_target,
                            inv_nearest_candidate(snap_target, pointer_x, pointer_y));
        }
    } else {
        inv_progress_probe(&snapshot);
    }

    if (!g_investigation.pending_object) {
        const InvestigationTarget* direction_target =
            inv_left_stick_target(&snapshot, pointer_x, pointer_y);
        if (direction_target) inv_begin_probe(&snapshot, direction_target, 0);
    } else {
        /* 摇杆回中仍要重新武装下一次方向沿。 */
        i32 left_x = (i32)PadInput_Axis(PAD_AXIS_LEFT_X);
        i32 left_y = (i32)PadInput_Axis(PAD_AXIS_LEFT_Y);
        if (inv_abs(left_x) < PAD_STICK_DEADZONE && inv_abs(left_y) < PAD_STICK_DEADZONE) {
            g_investigation.left_stick_latched = 0;
        }
    }

    if (PadInput_Pressed(PAD_SOUTH)) {
        if (g_investigation.pending_object != 0u) {
            /* 已warp但原版尚未验证：先排队，只在真正hover命中后点击。 */
            g_investigation.pending_click = 1;
        } else if (snapshot.hovered_object != 0u && !right_moved) {
            Cursor_PulseLeftClick();
        }
    }

    Cursor_SetInvestigationSession(1);
    return 1;
}

/* 原版 resolver 报告指针首次碰到新的当前可互动对象时，只震一次；离开后才重新武装。 */
void Investigation_UpdateRumble(void) {
    InvestigationSnapshot snapshot;
    u32 hovered = 0u;

    if (g_investigation.enabled && Cursor_ControllerOwnsPointer() &&
        inv_read_snapshot(&snapshot) && inv_snapshot_fresh(&snapshot)) {
        hovered = snapshot.hovered_object;
    }

    if (hovered == 0u) {
        g_investigation.last_rumble_object = 0u;
        return;
    }
    if (hovered == g_investigation.last_rumble_object) return;

    g_investigation.last_rumble_object = hovered;
    if (Runtime_Config()->rumble_strength_percent > 0u &&
        Runtime_Config()->investigation_rumble_ms > 0u) {
        u32 strength = (Runtime_Config()->rumble_strength_percent * 65535u) / 100u;
        PadInput_Rumble((u16)strength, (u16)strength,
                        Runtime_Config()->investigation_rumble_ms, 1);
    }
}

int Investigation_Active(void) { return g_investigation.active; }
