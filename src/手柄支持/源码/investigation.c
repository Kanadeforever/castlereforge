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
 * 6. 左杆在一次 LT 调查会话里按“持续方向”选择目标：轮盘中心始终使用当前受控角色
 *    的屏幕坐标，不再依赖隐藏/可见鼠标位置。玩家绕着左摇杆改变方向时，每个 worker tick
 *    都会重新计算角色周围最贴近当前方向的目标。这里不能简单删除旧 latch，否则 8ms worker
 *    会沿同一方向一路扫过多个目标；必须同时记住“当前方向目标”，只有目标真正变化才 warp。
 * 7. 右杆保留独立的百分之一像素余量，用于很低速度的精细移动；
 *    只有进入短吸附半径后才尝试候选，不做大范围磁吸。
 * 8. A 若遇到尚未验证的候选，只设置 pending_click；原版确认 hover 后，
 *    才通过 Cursor 的 48ms 左键桥提交一次调查。
 * 9. 本模块只执行已经获准的调查会话；Back/RT/LT 优先级、自由地图门与
 *    CaptureAll 全部由 ControlModes 统一裁决，避免这里再次猜测菜单状态。
 * 10. 可互动目标震动只允许出现在两类显式指针会话：LT 调查，或 Back/RT 鼠标模式。
 *     两者都沿用同一条规则：resolver 首次报告新的 hovered_object 时震一次，离开目标后
 *     才重新武装。普通手柄状态即使隐藏鼠标坐标碰到目标也绝不震动。
 *     SDL_RumbleGamepad 缺失则静默退化，不影响任何输入或原版键鼠。
 * 11. 任一协议签名、CALL 目标或指针合理性检查失败都发布空结果，
 *     或只关闭本能力；r36 已验收菜单与战斗底座不能被连坐。
 * 12. 快照 sequence 使用单写者/单读者协议：奇数正在写，偶数完整；
 *     reader 必须在复制前后看到同一偶数，才可以使用这一帧数据。
 * 13. 地图可互动 hover 的短震只提交低优先级请求，不能覆盖仍在播放的“激活普通手柄模式”反馈。
 * 14. 震动强度读取全局 Rumble.StrengthPercent；本模块拥有的是地图可互动 hover 的共享时长，
 *     配置键历史名称仍为 InvestigationHoverDurationMs，Back/RT 鼠标模式也复用这一时长。
 */

#define INVESTIGATION_SCREEN_WIDTH  640
#define INVESTIGATION_SCREEN_HEIGHT 480
#define INVESTIGATION_MAX_TARGETS   96
#define INVESTIGATION_CANDIDATES    25
#define INVESTIGATION_FRESH_TICKS   8u

/*
 * R39 连续方向选择的手感常量。
 *
 * AXIS_DIVISOR=1024 的作用不是降低输入精度，而是先把 SDL 的 i16 摇杆值缩到大约
 * -32..+31。这样后面可以用纯 32 位整数计算“叉积平方 / 目标距离平方”，既避免
 * sqrt/atan2，也避免 x86 /nodefaultlib 构建因为 64 位除法偷偷引入 CRT helper。
 *
 * 这个评分等价于“摇杆方向与目标方向夹角的 sin²”，不会像 R38 的
 * abs(cross)/(abs(dx)+abs(dy)) 那样对水平、垂直、对角线产生不同权重。
 *
 * HYSTERESIS_PERMILLE=2 表示只保留很轻的方向迟滞。满幅摇杆时大约相当于 2～3 度，
 * 足够挡住模拟轴在两个扇区边界上的微小噪声，但不会像 R38 的 8% 迟滞那样让玩家
 * 已经明显转向新目标后还“粘”在旧目标上。
 */
#define INVESTIGATION_DIRECTION_AXIS_DIVISOR 1024
#define INVESTIGATION_DIRECTION_HYSTERESIS_PERMILLE 2

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

    /*
     * 左摇杆轮盘的中心点直接来自当前受控角色，而不是鼠标。
     *
     * R38 把“第一次推左杆时的鼠标位置”冻结成轮盘中心。单次推一下时看起来还可以，
     * 但连续绕圈时玩家脑中想的是“角色右上方/左下方有哪些调查点”，而程序算的却是
     * “某个隐藏鼠标坐标右上方/左下方有哪些调查点”。鼠标只要之前停在角色旁边、屏幕边缘
     * 或上一次调查目标上，两个方向系就会错开，于是出现“摇杆明明指向这个目标却没有选中”。
     *
     * 因此从 R39 开始，game thread 在发布 resolver 快照时同时发布受控角色的屏幕坐标。
     * worker 只读取这两个整数快照，不保存 actor 指针，也不在 worker 解引用游戏对象。
     */
    i32 wheel_origin_x;
    i32 wheel_origin_y;

    int target_count;
    InvestigationTarget targets[INVESTIGATION_MAX_TARGETS];
} InvestigationSnapshot;

typedef struct InvestigationState {
    int enabled;
    int active;

    /*
     * 左摇杆轮盘选择状态。
     *
     * R39 不再保存“鼠标锚点”。轮盘中心每一帧都直接使用 InvestigationSnapshot 中的
     * wheel_origin_x / wheel_origin_y，也就是当前角色的屏幕坐标。这样即使摄像机轻微移动、
     * 角色位置变化，或者隐藏鼠标之前停在别处，摇杆方向始终表示“以角色为中心的方向”。
     *
     * selected_object 是当前这根左杆方向已经选中的对象。worker 每 8ms 都会重新算
     * 方向，但如果结果仍然是同一个对象，就什么也不做，从根本上避免同方向连跳。
     *
     * failed_object 记录“这次方向已经把 25 个候选像素都试完仍未被原版 resolver
     * 验证”的对象。只要玩家方向不变，就不反复重试；转向别的对象后再转回来时，
     * 它会重新获得一次完整 probe 机会。
     */
    u32 left_selected_object;
    u32 left_failed_object;

    /*
     * 右杆用于“我现在要手动精调”的明确意图。只要右杆动过，就把本轮左杆当前目标
     * 标记为 manual override：右杆停下后不能因为左杆仍保持原方向就自动吸回去。
     * 当左杆真正转到另一个 object，或者回中开始新轮盘时，这个标记会自动清除。
     */
    int left_manual_override;

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
    g_published_snapshot.wheel_origin_x = 0;
    g_published_snapshot.wheel_origin_y = 0;
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

    /*
     * actor+0x10 / +0x14 已由原版协议和固化研究确认是当前受控角色的世界 X/Y。
     * 目标矩形同样使用 camera_x/camera_y 转成 640x480 客户区坐标，所以这里用完全相同的
     * 相机基准把角色坐标转成轮盘中心。这样“摇杆向右”永远表示“角色右边”，不再表示
     * “隐藏鼠标右边”。这里只发布两个整数；worker 不拿 actor 指针跨线程使用。
     */
    g_published_snapshot.wheel_origin_x =
        inv_clamp(*(const i32*)(actor + 0x10u) - camera_x,
                  0, INVESTIGATION_SCREEN_WIDTH - 1);
    g_published_snapshot.wheel_origin_y =
        inv_clamp(*(const i32*)(actor + 0x14u) - camera_y,
                  0, INVESTIGATION_SCREEN_HEIGHT - 1);

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
        out->wheel_origin_x = g_published_snapshot.wheel_origin_x;
        out->wheel_origin_y = g_published_snapshot.wheel_origin_y;
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

        /*
         * 只有原版 resolver 真正确认 hover 才算 probe 成功。若它正是左杆当前方向目标，
         * 清掉失败记忆；之后同方向持续推住只保持这个选择，不会再重新 warp。
         */
        if (g_investigation.pending_object == g_investigation.left_selected_object) {
            g_investigation.left_failed_object = 0u;
        }
        inv_cancel_probe();
        if (click) Cursor_PulseLeftClick();
        return;
    }

    ++g_investigation.pending_candidate_step;
    if (!inv_warp_pending_candidate(snapshot)) {
        u32 failed_object = g_investigation.pending_object;

        /*
         * 25 点全部失败时记住本次左杆目标，避免 8ms worker 在同一方向无限重跑 25 点。
         * 玩家只要转向别的目标再转回来，就会因为 selected_object 变化而获得一次新 probe。
         */
        inv_cancel_probe();
        if (failed_object != 0u && failed_object == g_investigation.left_selected_object) {
            g_investigation.left_failed_object = failed_object;
        }
    }
}

/*
 * 判断左杆是否已经回到中心死区。
 *
 * 这里故意沿用项目原本的“方形死区”规则：X、Y 两轴都小于 PAD_STICK_DEADZONE
 * 才算回中。这样本轮只改变调查模式的选择生命周期，不偷偷改变整个项目的摇杆手感。
 */
static int inv_left_stick_centered(i32 axis_x, i32 axis_y) {
    return inv_abs(axis_x) < PAD_STICK_DEADZONE &&
           inv_abs(axis_y) < PAD_STICK_DEADZONE;
}

/*
 * 给一个目标计算“它和当前摇杆方向有多接近”。
 *
 * 这一版的中心点由调用者传入，实际就是 snapshot.wheel_origin_x/y，也就是角色位置。
 * 我们先把摇杆从 i16 大范围缩小到约 -32..+31，再计算：
 *
 *     dot   = 目标向量 · 摇杆向量
 *     cross = 目标向量 × 摇杆向量
 *
 * dot <= 0 说明目标位于摇杆的侧后方（夹角 >= 90°），这种目标不应该因为“别处没有目标”
 * 被错误吸过来；因此只保留摇杆前方 180°。和 R38 不同，这里不再额外套 +/-63° 硬扇区，
 * 所以前方不会出现人为制造的“空白角度”。
 *
 * 对前方目标，用 cross² / distance² 作为角度误差。因为：
 *
 *     cross² / distance² = |stick|² * sin²(angle)
 *
 * 对同一帧的所有目标，|stick|² 是共同常数，所以数值越小就代表目标方向越贴近摇杆。
 * 这种算法对水平、垂直、45° 对角线一视同仁，不会产生 R38 的 L1 距离方向偏差。
 *
 * out_distance 仍保存屏幕距离平方，只在两个目标角度误差完全相同时选择离角色更近的那个。
 */
static int inv_direction_score(const InvestigationTarget* target,
                               i32 origin_x, i32 origin_y,
                               i32 axis_x, i32 axis_y,
                               i32* out_error, i32* out_distance,
                               i32* out_axis_energy) {
    i32 scaled_axis_x;
    i32 scaled_axis_y;
    i32 dx;
    i32 dy;
    i32 dot;
    i32 cross;
    i32 distance_squared;
    i32 cross_squared;
    i32 axis_energy;

    if (!target || !out_error || !out_distance || !out_axis_energy) return 0;

    /*
     * 除以 1024 后，-32768..32767 变成大约 -32..31。
     * 这让最坏情况下的 cross 仍小于约 36000，cross*cross 安全落在 signed i32 内。
     */
    scaled_axis_x = axis_x / INVESTIGATION_DIRECTION_AXIS_DIVISOR;
    scaled_axis_y = axis_y / INVESTIGATION_DIRECTION_AXIS_DIVISOR;
    if (scaled_axis_x == 0 && scaled_axis_y == 0) return 0;

    dx = target->center_x - origin_x;
    dy = target->center_y - origin_y;
    if (dx == 0 && dy == 0) return 0;

    dot = dx * scaled_axis_x + dy * scaled_axis_y;
    if (dot <= 0) return 0;

    distance_squared = dx * dx + dy * dy;
    if (distance_squared <= 0) return 0;

    cross = dx * scaled_axis_y - dy * scaled_axis_x;
    cross_squared = cross * cross;
    axis_energy = scaled_axis_x * scaled_axis_x + scaled_axis_y * scaled_axis_y;

    *out_error = cross_squared / distance_squared;
    *out_distance = distance_squared;
    *out_axis_energy = axis_energy;
    return 1;
}

/*
 * 连续左杆“角色中心轮盘”目标选择。
 *
 * R38 已经解决“必须松杆/回中后才能再切一次”的旧 latch，但实机进一步暴露：它把第一次
 * 推杆时的鼠标位置当成固定锚点。鼠标可能是隐藏的，也可能停在上一个目标或屏幕任意位置，
 * 因此玩家绕杆时看到的是“角色周围一圈目标”，程序却按“鼠标周围一圈目标”分扇区。
 *
 * R39 把这个几何基准彻底改正：每个 tick 都以 resolver 快照中当前角色的屏幕位置为中心，
 * 对所有可互动目标计算角度，并选择摇杆前方 180° 内角度最接近的目标。没有额外的硬扇区，
 * 所以只要某个目标确实位于当前摇杆方向附近，它就有机会成为该方向的扇区拥有者。
 *
 * out_engaged 用来告诉调用者：NULL 到底是“摇杆回中了”还是“摇杆仍推着、只是前方没有目标”。
 * 后一种情况若旧 probe 还在进行，就必须取消旧 probe，避免玩家已经转开后旧目标晚确认。
 */
static const InvestigationTarget* inv_left_stick_target(const InvestigationSnapshot* snapshot,
                                                        int* out_engaged) {
    i32 axis_x = (i32)PadInput_Axis(PAD_AXIS_LEFT_X);
    i32 axis_y = (i32)PadInput_Axis(PAD_AXIS_LEFT_Y);
    const InvestigationTarget* best = NULL;
    const InvestigationTarget* current = NULL;
    i32 best_error = 0x7FFFFFFF;
    i32 best_distance = 0x7FFFFFFF;
    i32 current_error = 0x7FFFFFFF;
    i32 axis_energy = 0;
    i32 current_axis_energy = 0;
    int current_eligible = 0;
    int i;

    if (out_engaged) *out_engaged = 0;
    if (!snapshot) return NULL;

    if (inv_left_stick_centered(axis_x, axis_y)) {
        /*
         * 回中只代表“结束这次连续方向意图”，不再承担重新武装的职责。
         * 清掉当前对象/失败记忆后，下一次向任意方向推杆都会直接按角色中心重新选目标。
         */
        g_investigation.left_selected_object = 0u;
        g_investigation.left_failed_object = 0u;
        g_investigation.left_manual_override = 0;
        return NULL;
    }

    if (out_engaged) *out_engaged = 1;

    /* 如果旧目标已经不在最新快照里，就不要让失效 object 继续参与迟滞。 */
    if (g_investigation.left_selected_object != 0u &&
        !inv_find_target(snapshot, g_investigation.left_selected_object)) {
        g_investigation.left_selected_object = 0u;
        g_investigation.left_failed_object = 0u;
        g_investigation.left_manual_override = 0;
    }

    for (i = 0; i < snapshot->target_count; ++i) {
        const InvestigationTarget* target = &snapshot->targets[i];
        i32 error;
        i32 distance;
        i32 target_axis_energy;

        if (!inv_direction_score(target,
                                 snapshot->wheel_origin_x,
                                 snapshot->wheel_origin_y,
                                 axis_x, axis_y,
                                 &error, &distance,
                                 &target_axis_energy)) {
            continue;
        }

        if (target->object == g_investigation.left_selected_object) {
            current = target;
            current_error = error;
            current_axis_energy = target_axis_energy;
            current_eligible = 1;
        }

        /*
         * 第一优先级只看角度误差；完全同角度才看离角色的屏幕距离。
         * 这意味着同一条射线上的两个互动对象会自然选择较近者，而不会因为远近改变扇区角度。
         */
        if (error < best_error ||
            (error == best_error && distance < best_distance)) {
            best_error = error;
            best_distance = distance;
            axis_energy = target_axis_energy;
            best = target;
        }
    }

    if (!best) return NULL;

    /*
     * 只保留非常轻的边界迟滞。
     *
     * error 的量纲约等于 |stick|²*sin²(angle)。因此用 axis_energy 的千分之二作为阈值，
     * 满幅摇杆时只相当于约 2～3°。玩家明确把杆转向另一个目标时会立即切换；只有恰好
     * 压在两个目标的扇区分界线上，才允许当前目标多保留一点点，防止模拟轴噪声来回闪。
     */
    if (current_eligible && current && current->object != best->object) {
        i32 energy = current_axis_energy > 0 ? current_axis_energy : axis_energy;
        i32 hysteresis =
            (energy * INVESTIGATION_DIRECTION_HYSTERESIS_PERMILLE) / 1000;
        if (hysteresis < 1) hysteresis = 1;

        if (current_error <= best_error + hysteresis) {
            return current;
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

        /*
         * 结束 LT 会话时丢掉当前方向对象、失败记忆和人工接管标记。
         * 轮盘中心不需要保存/清理：下一帧会直接从 resolver 快照读取当时角色的屏幕位置。
         */
        g_investigation.left_selected_object = 0u;
        g_investigation.left_failed_object = 0u;
        g_investigation.left_manual_override = 0;
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
    Runtime_Log("[调查] LT独立能力已安装：640x480可互动快照、左杆角色中心连续轮盘选择、右杆低速短吸附、A原版左键。");
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
        g_investigation.left_selected_object = 0u;
        g_investigation.left_failed_object = 0u;
        g_investigation.left_manual_override = 0;
        inv_cancel_probe();
    }
    Cursor_SetInvestigationSession(1);

    /*
     * 右杆自由移动优先：玩家一旦主动精调，就取消上一次尚未验证的自动候选，并记住
     * “当前左杆目标已被人工接管”。这样右杆停下后，左杆若还保持原方向也不会把鼠标
     * 自动吸回；只有左杆真的转向另一个目标，自动轮盘选择才重新取得控制权。
     */
    right_moved = Cursor_MoveInvestigationRightStick();
    if (right_moved) {
        g_investigation.left_manual_override = 1;
        inv_cancel_probe();
    }

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
        int left_engaged = 0;
        const InvestigationTarget* direction_target;

        /*
         * 左杆连续方向选择必须在“推进旧 probe”之前读取。原因不是单纯追求更快，而是
         * 防止一个真实竞态：假如旧目标 A 还在等待 resolver，玩家此刻已经把摇杆转向 B，
         * 若我们先 inv_progress_probe()，这一帧可能先确认 A，甚至把先前 pending_click 的 A
         * 点击真正提交出去；随后才发现方向已经变成 B 就太晚了。
         *
         * 所以顺序固定为：
         *   1. 先根据最新摇杆方向算 wanted object；
         *   2. 如果目标变了，立刻用 inv_begin_probe() 取消 A 的 probe/pending_click 并换成 B；
         *   3. 最后才推进“现在仍然有效”的 probe。
         * 新 probe 刚建立时 pending_snapshot_serial 就等于当前快照 serial，因此紧接着调用
         * inv_progress_probe() 也不会误把同一帧当成验证结果。
         */
        direction_target =
            inv_left_stick_target(&snapshot, &left_engaged);

        if (direction_target) {
            u32 wanted_object = direction_target->object;

            if (wanted_object != g_investigation.left_selected_object) {
                /*
                 * 方向真正跨到另一个目标：先记住新目标，再开 probe。inv_begin_probe()
                 * 会取消旧 pending_object/pending_click，因此旧目标绝不会在转向后误点击。
                 * 新 object 也代表玩家重新明确了自动选择意图，所以右杆的 manual override
                 * 到这里结束。
                 */
                g_investigation.left_selected_object = wanted_object;
                g_investigation.left_failed_object = 0u;
                g_investigation.left_manual_override = 0;
                inv_begin_probe(&snapshot, direction_target, 0);
            } else if (!g_investigation.left_manual_override &&
                       g_investigation.left_failed_object != wanted_object &&
                       g_investigation.pending_object == 0u &&
                       snapshot.hovered_object != wanted_object) {
                /*
                 * 同一方向目标通常什么都不做；这里只处理“目标仍应是它，但先前 probe 因
                 * 临时快照/几何变化被取消，而且不是 25 点明确失败”的恢复情况。
                 * failed_object 会阻止真正失败的目标每 8ms 无限重试；manual_override 会阻止
                 * 右杆精调之后自动吸回。
                 */
                inv_begin_probe(&snapshot, direction_target, 0);
            }
        } else if (left_engaged && g_investigation.pending_object != 0u &&
                   g_investigation.pending_object == g_investigation.left_selected_object) {
            /*
             * 摇杆仍推着，但当前方向已经没有合格目标。取消旧方向 probe，防止几帧之后
             * 原 resolver 才确认旧坐标，导致光标在玩家已经转开的情况下“自己跳回去”。
             * selected_object 暂时保留；只要随后转到别的目标，就会正常发生 object 变化。
             */
            inv_cancel_probe();
        }

        /* 只推进经过上面“最新方向裁决”后仍然有效的候选。 */
        inv_progress_probe(&snapshot);
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

    /*
     * 只有“显式鼠标会话”或“LT调查会话”允许把地图 hover 变成震动。
     *
     * 这里绝不能再用 Cursor_ControllerOwnsPointer() 作为门：普通手柄操作本来就会
     * 取得指针所有权，而且为了视觉整洁普通态鼠标通常是隐藏的。原版 resolver 仍可能
     * 因那个隐藏坐标碰到可互动对象；如果只看 controller_owner，就会出现“玩家根本没有
     * 进入调查/鼠标模式，走路时却莫名震一下”的假反馈。
     *
     * MouseModeActive 覆盖 Back 常驻鼠标和自由地图 RT 临时鼠标；
     * InvestigationSessionActive 只在 LT 调查真正建立后为真。这样三种显式用鼠标寻找
     * 可互动对象的场景共用同一条“首次碰到新对象才震一次”的规则，而普通隐藏鼠标、
     * Battle/UI 导航和其它手柄业务全部静默。
     */
    if (g_investigation.enabled &&
        (Cursor_MouseModeActive() || Cursor_InvestigationSessionActive()) &&
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
