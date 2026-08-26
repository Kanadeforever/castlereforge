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
 * 6. 左杆在一次调查会话里按“持续方向”选择目标：轮盘中心始终使用当前受控角色
 *    的屏幕坐标，不再依赖隐藏/可见鼠标位置。玩家绕着左摇杆改变方向时，每个 worker tick
 *    都会重新计算角色周围最贴近当前方向的目标。这里不能简单删除旧 latch，否则 8ms worker
 *    会沿同一方向一路扫过多个目标；必须同时记住“当前方向目标”，只有目标真正变化才 warp。
 * 7. R40 的 LB/RB 只循环这份安全快照中的目标：先按角色世界距离、再按屏幕顺时针角度、
 *    最后按原 resolver 序号稳定排序。肩键选中后仍走同一套 25 点 probe；它不直接点击、
 *    不新增震动，也不在 worker 线程解引用任何游戏对象。
 * 8. 右杆保留独立的百分之一像素余量，用于很低速度的精细移动；
 *    只有进入短吸附半径后才尝试候选，不做大范围磁吸。
 * 9. A 若遇到尚未验证的候选，只设置 pending_click；原版确认 hover 后，
 *    才通过 Cursor 的 48ms 左键桥提交一次调查。
 * 10. 本模块只执行已经获准的调查会话；Back/RT/LT 优先级、自由地图门与
 *    CaptureAll 全部由 ControlModes 统一裁决，避免这里再次猜测菜单状态。
 * 11. 可互动目标震动只允许出现在两类显式指针会话：A/LT 调查，或 Back/RT 鼠标模式。
 *     两者都沿用同一条规则：resolver 首次报告新的 hovered_object 时震一次，离开目标后
 *     才重新武装。普通手柄状态即使隐藏鼠标坐标碰到目标也绝不震动。
 *     SDL_RumbleGamepad 缺失则静默退化，不影响任何输入或原版键鼠。
 * 12. 任一协议签名、CALL 目标或指针合理性检查失败都发布空结果，
 *     或只关闭本能力；r36 已验收菜单与战斗底座不能被连坐。
 * 13. 快照 sequence 使用单写者/单读者协议：奇数正在写，偶数完整；
 *     reader 必须在复制前后看到同一偶数，才可以使用这一帧数据。
 * 14. 地图可互动 hover 的短震只提交低优先级请求，不能覆盖仍在播放的“激活普通手柄模式”反馈。
 * 15. 震动强度读取全局 Rumble.StrengthPercent；本模块拥有的是地图可互动 hover 的共享时长，
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

/*
 * 肩键锁定后，残留左杆必须明显转动才重新取得焦点所有权。
 * sin^2(12°) 约为 0.0432，所以用 43/1000 的纯整数比较；若肩键按下时左杆已经回中，
 * 下一次重新推离死区本身就是新的方向意图，会立即交还给左杆。
 */
#define INVESTIGATION_SHOULDER_RELEASE_SIN_SQ_PERMILLE 43u

typedef struct InvestigationTarget {
    u32 object;
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;
    i32 center_x;
    i32 center_y;
    u32 world_distance_squared;
    i32 resolver_index;
    u8 interaction_type;
} InvestigationTarget;

typedef enum InvestigationFocusSource {
    INVESTIGATION_FOCUS_NONE = 0,
    INVESTIGATION_FOCUS_LEFT_STICK,
    INVESTIGATION_FOCUS_SHOULDER
} InvestigationFocusSource;

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

    /*
     * R40 肩键焦点只保存原版对象身份和按键当刻的左杆方向，不保存游戏指针或排序表。
     * 每次 LB/RB 新按沿都从最新完整快照重建最多 96 项的整数排序；对象仍存在时按 object
     * 重新映射当前位置，因此目标增删或距离变化不会让旧数组下标指向错误对象。
     */
    InvestigationFocusSource focus_source;
    u32 shoulder_selected_object;
    i32 shoulder_axis_x;
    i32 shoulder_axis_y;
    int shoulder_axis_was_centered;

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

static u32 inv_abs_difference(i32 left, i32 right) {
    if ((left < 0) == (right < 0)) {
        return left >= right ? (u32)(left - right) : (u32)(right - left);
    }
    return (left < 0 ? (u32)(-(left + 1)) + 1u : (u32)left) +
           (right < 0 ? (u32)(-(right + 1)) + 1u : (u32)right);
}

/* 世界坐标差可能来自 type>=5 的远距离对象；排序距离采用饱和平方，避免 signed 溢出。 */
static u32 inv_saturated_distance_squared(i32 left_x, i32 left_y,
                                          i32 right_x, i32 right_y) {
    u32 ax = inv_abs_difference(left_x, right_x);
    u32 ay = inv_abs_difference(left_y, right_y);

    if (ax > 46340u || ay > 46340u) return 0xFFFFFFFFu;
    return ax * ax + ay * ay;
}

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
    out->world_distance_squared = inv_saturated_distance_squared(
        *(const i32*)(object + 0x10u), *(const i32*)(object + 0x14u),
        *(const i32*)(actor + 0x10u), *(const i32*)(actor + 0x14u));
    out->resolver_index = 0;
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
        target.resolver_index = i;

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
        g_published_snapshot.targets[output_count].world_distance_squared =
            target.world_distance_squared;
        g_published_snapshot.targets[output_count].resolver_index = target.resolver_index;
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
            out->targets[i].world_distance_squared =
                g_published_snapshot.targets[i].world_distance_squared;
            out->targets[i].resolver_index = g_published_snapshot.targets[i].resolver_index;
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
 * 屏幕坐标中 Y 向下，因此从角色右侧开始的顺时针角度顺序是：右、下、左、上。
 * half=0 覆盖 [0°,180°)，half=1 覆盖 [180°,360°)；同一 half 内用叉积判断先后，
 * 不需要 atan2、浮点数或数学库。
 */
static int inv_clockwise_half(i32 dx, i32 dy) {
    return (dy > 0 || (dy == 0 && dx >= 0)) ? 0 : 1;
}

static int inv_shoulder_order_before(const InvestigationSnapshot* snapshot,
                                     int left_index, int right_index) {
    const InvestigationTarget* left = &snapshot->targets[left_index];
    const InvestigationTarget* right = &snapshot->targets[right_index];
    i32 left_dx;
    i32 left_dy;
    i32 right_dx;
    i32 right_dy;
    int left_half;
    int right_half;
    i32 cross;

    if (left->world_distance_squared != right->world_distance_squared) {
        return left->world_distance_squared < right->world_distance_squared;
    }

    left_dx = left->center_x - snapshot->wheel_origin_x;
    left_dy = left->center_y - snapshot->wheel_origin_y;
    right_dx = right->center_x - snapshot->wheel_origin_x;
    right_dy = right->center_y - snapshot->wheel_origin_y;
    left_half = inv_clockwise_half(left_dx, left_dy);
    right_half = inv_clockwise_half(right_dx, right_dy);
    if (left_half != right_half) return left_half < right_half;

    cross = left_dx * right_dy - left_dy * right_dx;
    if (cross != 0) return cross > 0;

    return left->resolver_index < right->resolver_index;
}

/* 最新快照最多 96 项，插入排序足够小且完全确定，不需要 qsort/CRT。 */
static int inv_build_shoulder_order(const InvestigationSnapshot* snapshot,
                                    int order[INVESTIGATION_MAX_TARGETS]) {
    int count;
    int i;

    if (!snapshot || !order) return 0;
    count = snapshot->target_count;
    if (count < 0) count = 0;
    if (count > INVESTIGATION_MAX_TARGETS) count = INVESTIGATION_MAX_TARGETS;

    for (i = 0; i < count; ++i) {
        int insert_at = i;
        order[i] = i;
        while (insert_at > 0 &&
               inv_shoulder_order_before(snapshot, order[insert_at], order[insert_at - 1])) {
            int temporary = order[insert_at - 1];
            order[insert_at - 1] = order[insert_at];
            order[insert_at] = temporary;
            --insert_at;
        }
    }
    return count;
}

/*
 * 第一次肩键输入优先从当前有效焦点继续循环：肩键对象 > 右杆当前真实 hover > 左杆对象。
 * 完全没有当前对象时由调用者执行 R40 规则：RB 取最近第0项，LB取最远末项。
 */
static u32 inv_shoulder_base_object(const InvestigationSnapshot* snapshot) {
    if (g_investigation.focus_source == INVESTIGATION_FOCUS_SHOULDER &&
        inv_find_target(snapshot, g_investigation.shoulder_selected_object)) {
        return g_investigation.shoulder_selected_object;
    }
    if (g_investigation.left_manual_override &&
        inv_find_target(snapshot, snapshot->hovered_object)) {
        return snapshot->hovered_object;
    }
    if (inv_find_target(snapshot, g_investigation.left_selected_object)) {
        return g_investigation.left_selected_object;
    }
    return 0u;
}

static void inv_record_shoulder_axis(void) {
    i32 axis_x = (i32)PadInput_Axis(PAD_AXIS_LEFT_X);
    i32 axis_y = (i32)PadInput_Axis(PAD_AXIS_LEFT_Y);

    g_investigation.shoulder_axis_was_centered =
        inv_left_stick_centered(axis_x, axis_y);
    g_investigation.shoulder_axis_x =
        axis_x / INVESTIGATION_DIRECTION_AXIS_DIVISOR;
    g_investigation.shoulder_axis_y =
        axis_y / INVESTIGATION_DIRECTION_AXIS_DIVISOR;
}

static int inv_shoulder_should_release_to_left_stick(void) {
    i32 axis_x = (i32)PadInput_Axis(PAD_AXIS_LEFT_X);
    i32 axis_y = (i32)PadInput_Axis(PAD_AXIS_LEFT_Y);
    int centered = inv_left_stick_centered(axis_x, axis_y);
    i32 current_x;
    i32 current_y;
    i32 dot;
    i32 cross;
    i32 base_energy;
    i32 current_energy;
    u32 cross_squared;
    u32 energy_product;

    if (g_investigation.focus_source != INVESTIGATION_FOCUS_SHOULDER) return 0;

    /* 肩键按下时已经回中：保持肩键循环；下一次明确推杆就是新的左杆意图。 */
    if (g_investigation.shoulder_axis_was_centered) return !centered;

    /* 肩键按下时左杆仍偏着：真正回中即完成一次明确的重新武装。 */
    if (centered) return 1;

    current_x = axis_x / INVESTIGATION_DIRECTION_AXIS_DIVISOR;
    current_y = axis_y / INVESTIGATION_DIRECTION_AXIS_DIVISOR;
    dot = g_investigation.shoulder_axis_x * current_x +
          g_investigation.shoulder_axis_y * current_y;
    if (dot <= 0) return 1;

    cross = g_investigation.shoulder_axis_x * current_y -
            g_investigation.shoulder_axis_y * current_x;
    base_energy = g_investigation.shoulder_axis_x * g_investigation.shoulder_axis_x +
                  g_investigation.shoulder_axis_y * g_investigation.shoulder_axis_y;
    current_energy = current_x * current_x + current_y * current_y;
    if (base_energy <= 0 || current_energy <= 0) return 1;

    cross_squared = (u32)(cross * cross);
    energy_product = (u32)base_energy * (u32)current_energy;
    return cross_squared * 1000u >=
           energy_product * INVESTIGATION_SHOULDER_RELEASE_SIN_SQ_PERMILLE;
}

static void inv_drop_shoulder_focus_if_missing(const InvestigationSnapshot* snapshot) {
    u32 object;

    if (g_investigation.focus_source != INVESTIGATION_FOCUS_SHOULDER) return;
    object = g_investigation.shoulder_selected_object;
    if (inv_find_target(snapshot, object)) return;

    if (g_investigation.pending_object == object) inv_cancel_probe();
    if (g_investigation.left_selected_object == object) {
        g_investigation.left_selected_object = 0u;
        g_investigation.left_failed_object = 0u;
    }
    g_investigation.shoulder_selected_object = 0u;
    g_investigation.focus_source = INVESTIGATION_FOCUS_NONE;
}

static int inv_select_shoulder_target(const InvestigationSnapshot* snapshot, int delta) {
    int order[INVESTIGATION_MAX_TARGETS];
    int count = inv_build_shoulder_order(snapshot, order);
    u32 base_object;
    int current = -1;
    int selected;
    int i;
    const InvestigationTarget* target;

    if (count <= 0 || (delta != -1 && delta != 1)) return 0;
    base_object = inv_shoulder_base_object(snapshot);
    for (i = 0; i < count; ++i) {
        if (snapshot->targets[order[i]].object == base_object) {
            current = i;
            break;
        }
    }

    if (current < 0) selected = delta > 0 ? 0 : count - 1;
    else {
        selected = current + delta;
        if (selected >= count) selected = 0;
        if (selected < 0) selected = count - 1;
    }

    target = &snapshot->targets[order[selected]];
    g_investigation.focus_source = INVESTIGATION_FOCUS_SHOULDER;
    g_investigation.shoulder_selected_object = target->object;
    g_investigation.left_selected_object = target->object;
    g_investigation.left_failed_object = 0u;
    g_investigation.left_manual_override = 0;
    inv_record_shoulder_axis();

    /* 每颗肩键都是一次明确重试授权；先取消旧 pending/click，再交同一25点链验证新对象。 */
    inv_cancel_probe();
    if (snapshot->hovered_object != target->object) {
        inv_begin_probe(snapshot, target, 0);
    }
    return 1;
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
        g_investigation.focus_source = INVESTIGATION_FOCUS_NONE;
        g_investigation.shoulder_selected_object = 0u;
        return NULL;
    }

    if (out_engaged) *out_engaged = 1;

    /* 如果旧目标已经不在最新快照里，就不要让失效 object 继续参与迟滞。 */
    if (g_investigation.left_selected_object != 0u &&
        !inv_find_target(snapshot, g_investigation.left_selected_object)) {
        g_investigation.left_selected_object = 0u;
        g_investigation.left_failed_object = 0u;
        g_investigation.left_manual_override = 0;
        if (g_investigation.focus_source == INVESTIGATION_FOCUS_LEFT_STICK) {
            g_investigation.focus_source = INVESTIGATION_FOCUS_NONE;
        }
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
         * 结束调查会话时丢掉当前方向对象、失败记忆和人工接管标记。
         * 轮盘中心不需要保存/清理：下一帧会直接从 resolver 快照读取当时角色的屏幕位置。
         */
        g_investigation.left_selected_object = 0u;
        g_investigation.left_failed_object = 0u;
        g_investigation.left_manual_override = 0;
        g_investigation.focus_source = INVESTIGATION_FOCUS_NONE;
        g_investigation.shoulder_selected_object = 0u;
        g_investigation.shoulder_axis_x = 0;
        g_investigation.shoulder_axis_y = 0;
        g_investigation.shoulder_axis_was_centered = 1;
        inv_cancel_probe();
    }
    Cursor_SetInvestigationSession(0);
}

int Investigation_InstallHooks(void) {
    if (!Runtime_InvestigationProtocolOk()) {
        g_investigation.enabled = 0;
        Runtime_Log("[调查] 原版协议不匹配；只禁用A/LT探索调查，既有菜单与战斗功能继续运行。");
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
    Runtime_Log("[调查] 独立能力已安装：A/LT双激活、左杆角色中心轮盘、LB/RB距离循环、右杆短吸附、resolver确认后互动。");
    return 1;
}

int Investigation_MapSnapshotReady(void) {
    InvestigationSnapshot snapshot;

    if (!g_investigation.enabled || !PadInput_GameForeground(NULL)) return 0;
    return inv_read_snapshot(&snapshot) && inv_snapshot_fresh(&snapshot);
}

/*
 * 默认的“按住确定键调查”把同一语义键同时当作“保持模式”和“松开后确认”。
 * 因此不能继续沿用旧方案的确定键按下沿：按下沿现在只是进入模式，真正的互动请求
 * 必须等 ControlModes 看到确定键已经松开以后，再调用本函数。物理键由布局选项决定。
 *
 * 这个函数看起来只有几个判断，但每一道都在阻止一种危险误点：
 *
 * 1. enabled/active：插件能力或调查会话已经结束时，旧按钮松开不能补点一次；
 * 2. GameForeground：玩家 Alt+Tab 到别的程序时，绝不能向后台游戏发鼠标点击；
 * 3. pending_object==0：自动选择还在25个像素中试探时，旧 hovered_object 可能仍是
 *    上一个目标。此时松开 A 必须安全退出，不能把旧目标误当作新焦点点击；
 * 4. fresh snapshot：resolver 已经停止更新时，缓存中的 hover 不能继续当作当前真值；
 * 5. hovered_object!=0：只有原版 resolver 真正认可的互动对象才允许提交。
 *
 * Cursor_PulseLeftClick() 只排一个48ms可靠脉冲。ControlModes 随后结束调查会话，
 * 但不会调用 Cursor_ResetClicks()，所以这个已经获准的脉冲仍能在后续 tick 正常松开。
 */
int Investigation_ConfirmCurrentHover(void) {
    InvestigationSnapshot snapshot;

    /* 能力没有安装，或会话根本没有建立：直接回答“没有提交”。 */
    if (!g_investigation.enabled || !g_investigation.active) return 0;

    /* 游戏不在前台时不制造任何模拟鼠标输入。 */
    if (!PadInput_GameForeground(NULL)) return 0;

    /* 仍在probe表示“新焦点尚未被原版确认”；这时宁可不互动，也不能点旧对象。 */
    if (g_investigation.pending_object != 0u) return 0;

    /* 读取一次跨线程安全快照；读不到完整新鲜帧就安全放弃。 */
    if (!inv_read_snapshot(&snapshot) || !inv_snapshot_fresh(&snapshot)) return 0;

    /* hovered_object==0 是原版明确告诉我们“当前位置没有互动目标”。 */
    if (snapshot.hovered_object == 0u) return 0;

    /* 所有安全门都通过后，才排唯一一次原版左键互动。 */
    Cursor_PulseLeftClick();
    return 1;
}

int Investigation_UpdateActive(void) {
    InvestigationSnapshot snapshot;
    int have_snapshot;
    int session_started = 0;
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
        /*
         * active 从0变成1只发生在“这次调查会话的第一帧”。先记下 session_started，
         * 后面自动最近目标必须只运行这一次；如果每个8ms tick都重新执行，光标会不断
         * 被最近目标吸回，左杆、LB/RB和右杆就全部失去意义。
         */
        session_started = 1;
        g_investigation.active = 1;
        g_investigation.left_selected_object = 0u;
        g_investigation.left_failed_object = 0u;
        g_investigation.left_manual_override = 0;
        g_investigation.focus_source = INVESTIGATION_FOCUS_NONE;
        g_investigation.shoulder_selected_object = 0u;
        g_investigation.shoulder_axis_x = 0;
        g_investigation.shoulder_axis_y = 0;
        g_investigation.shoulder_axis_was_centered = 1;
        inv_cancel_probe();
    }
    Cursor_SetInvestigationSession(1);

    /*
     * AutoFocusNearest=1 时，在会话第一帧自动提出“距离排序第0项”。
     *
     * 这里故意复用已经实机通过的 inv_select_shoulder_target(+1)，而不再写第二套最近算法：
     * - 会话刚重置，当前 object 为0，所以 +1 会按既有规则选择最近项；
     * - 它仍调用同一25点probe，不能直接把几何候选当作互动真值；
     * - 它把焦点记为 SHOULDER，能挡住进入前残留的左杆，避免下一帧马上抢回；
     * - 左杆回中/明显转向、LB/RB或右杆仍会按R40规则接管。
     *
     * 列表为空时函数返回0，状态保持NONE；关闭配置时完全跳过，行为逐字回到R41。
     */
    if (session_started && Runtime_Config()->investigation_auto_focus_nearest) {
        inv_select_shoulder_target(&snapshot, 1);
    }

    /*
     * 右杆自由移动优先：玩家一旦主动精调，就取消上一次尚未验证的自动候选，并记住
     * “当前左杆目标已被人工接管”。这样右杆停下后，左杆若还保持原方向也不会把鼠标
     * 自动吸回；只有左杆真的转向另一个目标，自动轮盘选择才重新取得控制权。
     */
    right_moved = Cursor_MoveInvestigationRightStick();
    if (right_moved) {
        g_investigation.left_manual_override = 1;
        g_investigation.focus_source = INVESTIGATION_FOCUS_NONE;
        g_investigation.shoulder_selected_object = 0u;
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
        int shoulder_delta =
            (PadInput_Pressed(PAD_RB) ? 1 : 0) -
            (PadInput_Pressed(PAD_LB) ? 1 : 0);
        int left_engaged = 0;
        const InvestigationTarget* direction_target;

        /*
         * 肩键和快照增删先于左杆裁决。LB/RB 同帧同时按下时 delta=0，明确视为无方向，
         * 避免靠读取顺序随机决定“上一/下一”。右杆在本 tick 已经移动时不会进入这里，
         * 因而继续保持 R39 的人工精调优先级。
         */
        inv_drop_shoulder_focus_if_missing(&snapshot);
        if (shoulder_delta != 0) {
            inv_select_shoulder_target(&snapshot, shoulder_delta);
            inv_progress_probe(&snapshot);
        } else {
            if (g_investigation.focus_source == INVESTIGATION_FOCUS_SHOULDER &&
                inv_shoulder_should_release_to_left_stick()) {
                g_investigation.focus_source = INVESTIGATION_FOCUS_NONE;
                g_investigation.shoulder_selected_object = 0u;
                g_investigation.left_manual_override = 0;
            }

            /* 肩键仍持有焦点时只推进它自己的 probe；残留左杆不能在下一帧抢回。 */
            if (g_investigation.focus_source == INVESTIGATION_FOCUS_SHOULDER) {
                inv_progress_probe(&snapshot);
            } else {

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
                        g_investigation.focus_source = INVESTIGATION_FOCUS_LEFT_STICK;
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
                        g_investigation.focus_source = INVESTIGATION_FOCUS_LEFT_STICK;
                        inv_begin_probe(&snapshot, direction_target, 0);
                    } else if (!g_investigation.left_manual_override) {
                        g_investigation.focus_source = INVESTIGATION_FOCUS_LEFT_STICK;
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
        }
    }

    /*
     * ActivationMode=1 才是旧的“按住LT、另按确定键确认”。默认的 ActivationMode=0
     * 已把确定键用作模式保持键；它的确认发生在松开沿，由上面的
     * Investigation_ConfirmCurrentHover() 处理。这里若不加模式门，按下确定键进入调查的
     * 同一个 tick 就可能立刻点击隐藏鼠标下面的旧对象，完全违背“松开才互动”。
     */
    if (Runtime_Config()->investigation_activation_mode == 1 &&
        InputRouter_RawPressed(INPUT_CONFIRM)) {
        if (g_investigation.pending_object != 0u) {
            /*
             * LT模式中，确定键按下时新目标还在probe：只记住“用户想确认”，不立即点击。
             * 后续某一帧原 resolver 确认 pending_object 后，inv_progress_probe 才会提交。
             */
            g_investigation.pending_click = 1;
        } else if (snapshot.hovered_object != 0u && !right_moved) {
            /* 已经有真实hover、且本tick不是右杆刚移动造成的中间态：提交一次可靠左键。 */
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
     * 只有“显式鼠标会话”或“A/LT调查会话”允许把地图 hover 变成震动。
     *
     * 这里绝不能再用 Cursor_ControllerOwnsPointer() 作为门：普通手柄操作本来就会
     * 取得指针所有权，而且为了视觉整洁普通态鼠标通常是隐藏的。原版 resolver 仍可能
     * 因那个隐藏坐标碰到可互动对象；如果只看 controller_owner，就会出现“玩家根本没有
     * 进入调查/鼠标模式，走路时却莫名震一下”的假反馈。
     *
     * MouseModeActive 覆盖 Back 常驻鼠标和自由地图 RT 临时鼠标；
     * InvestigationSessionActive 只在 A 或 LT 调查真正建立后为真。这样三种显式用鼠标寻找
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
