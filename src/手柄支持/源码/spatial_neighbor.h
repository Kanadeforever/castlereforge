#ifndef CASTLE_PAD_SPATIAL_NEIGHBOR_H
#define CASTLE_PAD_SPATIAL_NEIGHBOR_H

#include "platform.h"

/*
 * spatial_neighbor.h
 *
 * 这是“二维空间焦点找邻居”的纯算法层。
 *
 * 它故意不知道 Battle、阵形、角色、Button、SDL3 是什么，只认识三件事：
 * 1. 当前点在哪里；
 * 2. 其它合法点在哪里；
 * 3. 玩家要求向左、右、上、下中的哪一个方向移动。
 *
 * 这样 Battle Target 与后续阵形可以共用同一条几何规则，但仍保留完全独立的业务 Adapter。
 * 如果以后二维方向有 bug，只修这里；不会再出现 Battle 修一套、阵形又复制另一套的情况。
 */

typedef struct SpatialPoint {
    /* id 由调用者自己解释。Battle 可以放 target cell，阵形可以放阵位/角色索引。 */
    int id;
    /* x/y 是调用者提供的真实二维坐标。算法不假设分辨率，也不写死 1280x960。 */
    i32 x;
    i32 y;
} SpatialPoint;

typedef enum SpatialDirection {
    SPATIAL_LEFT = 0,
    SPATIAL_RIGHT,
    SPATIAL_UP,
    SPATIAL_DOWN
} SpatialDirection;

/*
 * 在 points[0..count) 中寻找 current_id 指定方向上的“最合理真实邻居”。
 * 返回找到的 point.id；如果这个方向没有合格邻居，返回 -1，调用者保持原焦点不动。
 */
int SpatialNeighbor_Find(const SpatialPoint* points, int count, int current_id, SpatialDirection direction);

#endif /* CASTLE_PAD_SPATIAL_NEIGHBOR_H */
