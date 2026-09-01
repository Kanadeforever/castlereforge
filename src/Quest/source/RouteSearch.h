#pragma once

// RouteSearch.h
//
// 这个头文件只定义“跨场景最短路搜索”需要的最小数据。
// 它故意不包含 Quest、GameVar、TOML 等任何复杂结构，原因有两个：
// 1. 让 RouteSearch.cpp 可以单独用 /Od 编译，规避 MSVC 14.51 在大翻译单元中触发的 C1001；
// 2. 让这里保持非常简单，后续维护者一眼就能看懂“输入是当前 Stage 的可走边，输出是第一条边”。

#include <cstddef>
#include <string>
#include <vector>

namespace CastleQuest {

// 一条已经由主程序判断为“当前原版状态下可以安全提前导航”的边。
// originalEdgeIndex 保存它在当前 Stage routes 数组中的下标，BFS 找到路径后会把这个下标还回去。
struct RouteSearchInputEdge {
    std::string fromScene;
    std::string toScene;
    std::size_t originalEdgeIndex = 0;
};

// BFS 的最小结果。
// found=false 表示当前没有找到安全路径；这种情况下主程序宁可不画 Marker，也绝不猜出口。
struct RouteSearchResult {
    bool found = false;
    std::size_t firstEdgeIndex = 0;
    int pathLength = 0;
};

// 纯路径搜索函数。
// 注意：这里完全不知道 GameVar，也不会读取游戏内存。
// 主程序会先把“不安全/条件不成立”的 Route 边过滤掉，再把剩下的边交给这里。
RouteSearchResult FindRouteBfsUnoptimized(const std::vector<RouteSearchInputEdge>& enabledEdges,
                                          const std::string& fromScene,
                                          const std::string& goalScene);

} // namespace CastleQuest
