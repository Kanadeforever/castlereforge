#include "spatial_neighbor.h"

/*
 * spatial_neighbor.c
 *
 * 这个文件只做一件事：在一组“已经由业务层判定为合法”的二维点里，按照玩家按下的方向找邻居。
 * 它不知道 Battle、阵形、角色、Button，也绝不决定某个对象“能不能被选”。
 *
 * refactor23 的严重问题：
 * 当时为了保证“右下方同一个点不能同时算右和下”，把方向限制成了严格 45° 扇区：
 *   左右要求 |dx| >= |dy|；
 *   上下要求 |dy| > |dx|。
 * 这在规则整齐的方格里看起来很漂亮，但《幽城幻剑录》的 Battle/阵形是斜排、错列布局。
 * 某个真正的“上一列/下一列邻居”可能同时有明显的 x 偏移，于是会被 45° 门直接排除，
 * 玩家实机就会看到“按上/下没反应”或者方向突然跳错。
 *
 * refactor24 恢复成更符合人眼菜单导航的两步规则：
 *
 * 第一步——只用方向正负决定候选半平面：
 *   ←：dx < 0；  →：dx > 0；  ↑：dy < 0；  ↓：dy > 0。
 *
 * 第二步——在这个半平面中，优先找“副轴最对齐”的对象：
 *   ←/→ 先比较 |dy|，也就是优先同一水平排；如果一样，再选 |dx| 更近的；
 *   ↑/↓ 先比较 |dx|，也就是优先同一垂直列；如果一样，再选 |dy| 更近的。
 *
 * 这样有两个直接效果：
 * 1. 斜排阵位仍然能在四个方向之间正常移动，不会被 45° 扇区卡死；
 * 2. “右”和“下”即使共享同一个大半平面候选，也会分别优先寻找水平/垂直对齐对象，
 *    因此不会再把简单的欧氏最近点误当成所有方向共同的答案。
 *
 * 这其实与 refactor22 的 Battle 真实屏幕坐标选择原则一致；r24 把它正式抽成共享算法，
 * 同时只修掉旧 Battle fallback 里真正错误的行列轴交叉。
 */

static i32 spatial_abs(i32 value) {
    return value < 0 ? -value : value;
}

/* 找 current_id 对应的数组位置。调用者的 id 可以是 target cell、阵位号等，不要求等于数组下标。 */
static int spatial_find_current(const SpatialPoint* points, int count, int current_id) {
    int i;
    if (!points || count <= 0) return -1;
    for (i = 0; i < count; ++i) {
        if (points[i].id == current_id) return i;
    }
    return -1;
}

/*
 * 只判断“这个点是不是在玩家要求的那个方向”。
 * 不再要求主轴大于副轴，因为错列/斜排界面没有义务落在 45° 扇区里。
 */
static int spatial_in_direction(i32 dx, i32 dy, SpatialDirection direction) {
    if (dx == 0 && dy == 0) return 0;

    switch (direction) {
    case SPATIAL_LEFT:  return dx < 0;
    case SPATIAL_RIGHT: return dx > 0;
    case SPATIAL_UP:    return dy < 0;
    case SPATIAL_DOWN:  return dy > 0;
    default:            return 0;
    }
}

int SpatialNeighbor_Find(const SpatialPoint* points, int count, int current_id, SpatialDirection direction) {
    int current_index;
    int i;
    int best_id = -1;
    i32 best_secondary = 0x7fffffff;
    i32 best_primary = 0x7fffffff;
    const SpatialPoint* current;

    current_index = spatial_find_current(points, count, current_id);
    if (current_index < 0) return -1;
    current = &points[current_index];

    for (i = 0; i < count; ++i) {
        i32 dx;
        i32 dy;
        i32 primary;
        i32 secondary;

        if (i == current_index) continue;
        dx = points[i].x - current->x;
        dy = points[i].y - current->y;
        if (!spatial_in_direction(dx, dy, direction)) continue;

        /*
         * “副轴对齐优先”是关键：
         * - 水平移动最在意 y 是否接近；
         * - 垂直移动最在意 x 是否接近。
         * 只有副轴一样好时，才选主轴距离更近的对象。
         */
        if (direction == SPATIAL_LEFT || direction == SPATIAL_RIGHT) {
            secondary = spatial_abs(dy);
            primary = spatial_abs(dx);
        } else {
            secondary = spatial_abs(dx);
            primary = spatial_abs(dy);
        }

        if (secondary < best_secondary ||
            (secondary == best_secondary && primary < best_primary)) {
            best_secondary = secondary;
            best_primary = primary;
            best_id = points[i].id;
        }
    }

    return best_id;
}
