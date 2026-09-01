// RouteSearch.cpp
// 《幽城幻剑录》Castle_Quest 跨场景 Route BFS 独立翻译单元。
//
// 为什么单独放一个 .cpp：
// MSVC 14.51.36231 在 CastleQuest.cpp 的 FindSafeRoute() 上发生了内部编译器错误 C1001。
// 这是编译器自身崩溃，不是普通 C++ 语法错误。为了不降低整个插件运行时代码的优化级别，
// dev6t 中本文件与 CastleQuest.cpp 都使用 /Od。MSVC 14.51 已在 CastleQuest.cpp 的 xtree 优化阶段实机触发 C1001，所以当前不再强行保留主文件 /O1。
//
// 这里故意不用 std::map/std::set。
// 当前一次搜索只接收“当前 Stage 自己的少量 Route 边”，简单 vector 线性查找已经足够快，
// 同时模板实例化更少，对旧/有缺陷的编译器更友好。

#include "RouteSearch.h"

#include <algorithm>
#include <cctype>

namespace CastleQuest {
namespace {

// Route SceneID 全部是 ASCII，例如 mp0101。
// 这里做小写化只是为了让比较稳定，不受数据里偶发大小写差异影响。
static std::string ToLowerAsciiLocal(const std::string& text) {
    std::string out = text;
    for (char& ch : out) {
        const unsigned char value = static_cast<unsigned char>(ch);
        ch = static_cast<char>(std::tolower(value));
    }
    return out;
}

struct SearchNode {
    std::string scene;
    int previousNode = -1;
    std::size_t incomingEdgeIndex = 0;
};

// 返回 nodes 里某个 Scene 的下标；找不到返回 -1。
// 这里用线性搜索是刻意的：场景规模很小，代码简单比微小的理论性能收益更重要。
static int FindNodeIndex(const std::vector<SearchNode>& nodes, const std::string& scene) {
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].scene == scene) return static_cast<int>(i);
    }
    return -1;
}

} // namespace

RouteSearchResult FindRouteBfsUnoptimized(const std::vector<RouteSearchInputEdge>& enabledEdges,
                                          const std::string& fromScene,
                                          const std::string& goalScene) {
    RouteSearchResult result;

    const std::string start = ToLowerAsciiLocal(fromScene);
    const std::string goal = ToLowerAsciiLocal(goalScene);
    if (start.empty() || goal.empty() || start == goal || enabledEdges.empty()) return result;

    // nodes 同时承担“访问过集合”和“前驱表”两个职责。
    // queueRead 指向当前要展开的节点；nodes 尾部就是 BFS 队列的新节点。
    std::vector<SearchNode> nodes;
    nodes.push_back(SearchNode{start, -1, 0});
    std::size_t queueRead = 0;

    while (queueRead < nodes.size()) {
        const int currentNodeIndex = static_cast<int>(queueRead);
        const std::string currentScene = nodes[queueRead].scene;
        ++queueRead;

        // 找出所有从当前 Scene 出发的已启用 Route 边。
        for (const RouteSearchInputEdge& edge : enabledEdges) {
            if (ToLowerAsciiLocal(edge.fromScene) != currentScene) continue;

            const std::string nextScene = ToLowerAsciiLocal(edge.toScene);
            if (nextScene.empty()) continue;
            if (FindNodeIndex(nodes, nextScene) >= 0) continue;

            nodes.push_back(SearchNode{nextScene, currentNodeIndex, edge.originalEdgeIndex});
            const int newNodeIndex = static_cast<int>(nodes.size() - 1);

            if (nextScene != goal) continue;

            // 从目标节点沿 previousNode 倒着回到起点。
            // 最后一次覆盖 firstEdgeIndex 的，就是离起点最近的第一条 Route 边。
            int cursor = newNodeIndex;
            int length = 0;
            std::size_t firstEdgeIndex = edge.originalEdgeIndex;
            while (cursor > 0) {
                const SearchNode& node = nodes[static_cast<std::size_t>(cursor)];
                firstEdgeIndex = node.incomingEdgeIndex;
                cursor = node.previousNode;
                ++length;
                if (length > 512) return RouteSearchResult();
            }

            result.found = true;
            result.firstEdgeIndex = firstEdgeIndex;
            result.pathLength = length;
            return result;
        }
    }

    return result;
}

} // namespace CastleQuest
