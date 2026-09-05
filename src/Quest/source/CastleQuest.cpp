// CastleQuest.cpp
// 《幽城幻剑录》现代任务系统 ASI 插件 v0.1-dev6zd
//
// 这份源码故意把“为什么这样做”写得非常详细。
// 目标读者是假设只学过一天编程、知道变量/if/函数是什么，但还不了解逆向、Hook、DirectDraw。
// 所以你会看到很多看起来“啰嗦”的中文注释。它们不是装饰，而是这个项目的长期接档资料的一部分。
//
// dev6zd 当前安全边界：
// 0. dev6zd 接入主项目 RuntimeSDK Client：DllMain 只登记 Client，正式业务初始化由 SDK 选择 Integrated 或 Standalone。
// 1. 任务系统只读取原版剧情状态，不修改 GameVar、不替原版触发 EVE，也不改变存档。
// 2. 任务系统仍只需要两个 6 字节入口 Hook：Present 用来画 Overlay；0x409580 用来只读捕获探索管理器 this。
//    Integrated 模式下这两处游戏代码写入必须交给 Runtime Hook 事务；Standalone 才允许沿用本地 VirtualProtect 路径。
// 3. 两个 Hook 前都会核对原版函数开头字节。如果不是我们确认过的版本，就拒绝 Hook，而不是猜地址硬写。
// 4. dev4 已加入河州镇第一批“多源攻略定位 + 原版程序状态约束”的真实试验任务；
//    仍然禁止把攻略中的次数/时限措辞直接当成最终程序事实。未闭合的条件会明确标注为候选，并通过运行日志继续取证。
//
// 编译目标：32 位 x86 ASI（PE 类型仍是 DLL）；build.bat 直接输出 .asi，并固定 /MT 静态 CRT。
// 当前 build.bat 沿用已经验证过的 Visual Studio 自动发现思路：先调用 VS Installer 自带的 vswhere.exe，
// 找到安装了 x86/x64 C++ 工具的最新 Visual Studio，再调用该实例的 VsDevCmd.bat 建立明确的 x86 编译环境。
// 脚本同时引用主仓同级 src\RuntimeSDK 的公共头和 Client 源码，不在 Quest 子项目里复制 SDK。
// 原版 RPG.exe 是 PE32 且固定 ImageBase=0x00400000，因此本文中的地址都是该确认版本的绝对 VA。

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <ddraw.h>

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cwctype>
#include <cwchar>
#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include "RouteSearch.h"
#include "CastleRuntime_Client.h"
#include "CastleHook_API.h"
#include "CastleDisplay_API.h"
#include "CastleOverlay_API.h"
#include "CastleSchedule_API.h"
#include "CastleGameState_API.h"
#include "CastleLog_API.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// dev6zd 起同时支持主项目 RuntimeSDK 与无 Runtime 的独立 ASI 模式。
// build.bat 使用 CastleQuest.def 导出三个稳定 ABI 名称：CastlePlugin_Query、InitializeASI、
// CastleRuntimeClient_NotifyLoaderReady。这里仍在编译期强制要求 Win32/x86；如果误用 x64 编译器，
// 立刻停止，避免生成 RPG.exe 无法加载的 64 位 ASI。
#if !defined(_M_IX86)
#error Castle_Quest must be built as Win32/x86 because RPG.exe and Castle Mod Loader are 32-bit.
#endif

namespace CastleQuest {

// ============================================================================
// 1. 版本号与原版地址
// ============================================================================

static const wchar_t* kVersion = L"v0.1-dev6zd";

// 下面这些地址都来自“固化68 + 原版 RPG.exe.org”的静态复核。
// dev6d 继续不使用模糊猜测，也不会自动扫描一堆相似代码然后随便选一个。
namespace Address {
    // World/GameVariable block 的全局指针。
    // [0x00978508] = 指向 0x27D8 字节 World block 的指针。
    static constexpr uintptr_t kWorldPtr = 0x00978508;

    // 当前 Camera/world origin。dev6 以后正式保留 WorldCamera 与 ControllerCamera 双轨诊断；
    // dev6g/dev6h 默认 Marker 仍以“RuntimeEntity 世界坐标 - Camera”作为普通实体锚点，并在世界地图
    // 出入口存在原版 Section 0 TouchBox 时优先取触发矩形中心。ControllerCamera 继续只作为 A/B 证据。
    static constexpr uintptr_t kCameraX = 0x00978514;
    static constexpr uintptr_t kCameraY = 0x00978518;

    // dev6d 宽屏临时兼容层直接复刻 Castle_Widescreen v0.11-poc11 已验证的 CameraPlan 所需边界。
    // 这些值不是 Quest 猜出来的：宽屏源码明确记录 0x44B300 会写 min/max/viewport，
    // 0x44B360 再把游戏自己的 Camera 夹在这些边界中。Quest 只读取它们，绝不修改。
    static constexpr uintptr_t kCameraViewW = 0x0097851C;
    static constexpr uintptr_t kCameraViewH = 0x00978520;
    static constexpr uintptr_t kCameraMinX = 0x00978524;
    static constexpr uintptr_t kCameraMinY = 0x00978528;
    static constexpr uintptr_t kCameraMaxX = 0x0097852C;
    static constexpr uintptr_t kCameraMaxY = 0x00978530;

    // dev6d 可视门控来自 Castle_Backlog 已经在实机使用的“自由探索”判定基础。
    // Quest 比 Backlog 更严格：Backlog 可以在稳定对话上打开，Quest 则只要有对话/菜单/战斗/电影，
    // 所有可视内容（HUD、Marker、Ctrl+F7 面板、诊断层）一律隐藏。
    static constexpr uintptr_t kDialogueMode = 0x0046F640;
    static constexpr uintptr_t kDialogueId = 0x0046F670;
    static constexpr uintptr_t kDialogueTargetState = 0x0046F678;
    static constexpr uintptr_t kDialogueCurrentState = 0x0046F679;
    static constexpr uintptr_t kEventYieldFlag = 0x0089F81D;
    static constexpr uintptr_t kEventBlockFlag = 0x0089F81E;
    static constexpr uintptr_t kEventTable = 0x0089F820;
    static constexpr uintptr_t kMapInputGate = 0x00468BB9;
    static constexpr uintptr_t kMapKeyMode = 0x00468BF0;
    static constexpr uintptr_t kBattleUi = 0x0089FD74;
    static constexpr uintptr_t kResultUi = 0x0089FCC8;
    static constexpr uintptr_t kTargetSelectorUi = 0x008E2410;
    static constexpr uintptr_t kTitleUi = 0x008E241C;
    static constexpr uintptr_t kInterfaceUi = 0x008DED0C;
    static constexpr uintptr_t kSavePointUi = 0x0089FCD0;
    static constexpr uintptr_t kInnUi = 0x0089FCCC;
    static constexpr uintptr_t kSynthesisUi = 0x0089FCBC;
    static constexpr uintptr_t kShopUi = 0x0089FCD4;
    static constexpr uintptr_t kMovieObject = 0x0046F390;
    static constexpr size_t kMovieActiveFlagOffset = 0x0A;

    // 0x89F7F0 是原版“固定角色/受控 Actor 槽”之一，不是当前场景全部 NPC 列表。
    // dev3a 实机日志在 mp0000/mp0102*/mp0101 都只看到飛龍/小夏侯儀/dead/大夏侯儀/霍雍这五个槽，
    // 因此 dev4 只继续用它读取玩家逻辑坐标，不再用它解析 NPC Marker。
    static constexpr uintptr_t kRuntimeEntityBasePtr = 0x0089F7F0;

    // 0x409580 是探索场景管理器每轮更新函数。原版函数通过 this+0x2C 访问“当前场景对象指针表”，
    // 通过 this+0x1C 读取场景对象数量；随后才把受控角色与额外 Actor 追加到表尾。
    // dev4 只在函数入口捕获 this 指针，然后在 Present 阶段只读该对象表，不改变原版管理器内容。
    static constexpr uintptr_t kExplorationUpdate = 0x00409580;

    // 当前受控角色在运行时 Entity 数组里的索引。
    static constexpr uintptr_t kControlledEntityIndex = 0x00468BF0;

    // 原版全局 DataCenter 指针。EVE 的 ADD_ITEM / CONSUME_ITEM 等库存命令都先读这里，
    // 再从 DataCenter+0x90 取得当前激活 Inventory Bank。dev4 只读这个 Bank，
    // 用于 item[451] 这类任务条件，不调用任何会修改库存的原版函数。
    static constexpr uintptr_t kDataCenterPtr = 0x008E1C48;

    // 当前活动 Event ID。固化23 对 +0xEB consumer 的闭合链明确使用了这个全局值。
    // dev4 只读并在变化时记录，方便把“阿铃第几次对话”映射回真实 Event/GameVar。
    static constexpr uintptr_t kActiveEventId = 0x0089F808;

    // 原版探索输入函数 0x00408830 每帧先调用 GetCursorPos，然后把经过原版输入环境处理后的
    // 鼠标 X/Y 加上当前 Camera，并最终把“游戏自己用于场景交互判定的世界坐标”写到这里。
    // dev6x 之前的 F11 调试悬停又单独调用了一次 Win32 GetCursorPos，并自行按客户区比例缩放，
    // 在 cnc-ddraw / 宽屏输入变换存在时会和游戏真正看到的鼠标位置发生明显错位。
    // dev6y 起 F11 首选直接读取这两个原版世界坐标；只读，不修改，也不改变原版输入。
    static constexpr uintptr_t kMouseWorldX = 0x0089F7C0;
    static constexpr uintptr_t kMouseWorldY = 0x0089F7C4;

    // 游戏 HWND。这个地址仍用于 F11 判断鼠标是否还位于游戏客户区；正式 Overlay 完全不对
    // DirectDraw Surface 调 GetDC，也不靠窗口 DC 绘制任务内容。中文文字仅在插件私有 DIB 中栅格化。
    // 也不靠窗口 DC 绘制任务内容。中文文字仅在插件自建的内存 DIB 中栅格化。
    static constexpr uintptr_t kGameWindow = 0x0046F384;

    // 原版常规呈现函数。全屏路径调用 Flip，窗口路径调用 Blt。
    // dev4 在这里安装 6 字节 detour。Overlay 会先画到 renderer+8 的本帧渲染面，再调用原版 Present。
    static constexpr uintptr_t kPresent = 0x004064E0;

    // 只用于版本字节核对；任务读取本身不调用这个函数。
    // 原版 GET_VAR(index)，index 0 恒返回 0，否则读取 World+0xC8+index*2 的 signed int16。
    static constexpr uintptr_t kGetGameVar = 0x0044B0D0;

    // 当前受控 Entity getter。dev4 不直接 call 它，而是用它的机器码作为版本护栏之一。
    static constexpr uintptr_t kGetControlledEntity = 0x00408C10;
}

// World block 内偏移。这里和存档 TSF 的 World/GameVariable block 对应。
namespace WorldOffset {
    static constexpr size_t kSceneDescriptor = 0x0000; // 50 字节 LegacySceneLoadDescriptor
    static constexpr size_t kSceneDescriptorSize = 50;
    static constexpr size_t kGameVars = 0x00C8;        // 5000 × int16
    static constexpr size_t kGameVarCount = 5000;
    static constexpr size_t kBlockSize = 0x27D8;
}

// Runtime Entity 与 source SCI record 的关键偏移。
// Runtime record 每条 0x74 字节；runtime+0x70 指向 0x227 字节的 source Entity SCI record。
namespace RuntimeEntityOffset {
    static constexpr size_t kRecordSize = 0x74;
    static constexpr size_t kWorldX = 0x10;
    static constexpr size_t kWorldY = 0x14;

    // runtime+0x6C 是原版 Entity 自己持有的 SF2 controller 指针。
    // 0x40A160 构造 Entity 时把该 controller 建出来；0x40A2B0 更新实体时持续更新它。
    // controller+0/+4 正是原版 Sprite 绘制链使用的“渲染基点”。
    // dev4 用它减 Camera 得到实体真实屏幕锚点，正式废止 dev2 的 world-camera 猜法。
    static constexpr size_t kSf2Controller = 0x6C;
    static constexpr size_t kSourceSciPtr = 0x70;
}

// 0x409580 探索管理器的已确认字段。这里不是凭字段名猜出来的：
// - 0x409600 把受控角色写入 [this+0x2C + count*4]；
// - 0x409615 又把额外 Actor 写入下一格；
// - 0x409640/0x409690/0x409860 都按 this+0x1C 的数量遍历 this+0x2C 指针表。
// 因而 dev4 把“前 count 个指针”视为场景原生对象，把后面追加的受控角色/额外 Actor排除在 NPC 列表之外。
namespace ExplorationManagerOffset {
    static constexpr size_t kSceneObjectCount = 0x1C;
    static constexpr size_t kSceneObjectPointers = 0x2C;
}

namespace SourceEntityOffset {
    static constexpr size_t kName = 0x00;              // 最多约 100 字节的旧编码名字/调试名
    static constexpr size_t kNameCapacity = 0x64;
    static constexpr size_t kInitialWorldX = 0x64;
    static constexpr size_t kInitialWorldY = 0x68;

    // 这两个 i16 是 Entity 的 SF2 anchor。原版 0x40A160 明确计算：
    // controllerX = runtimeX + sourceAnchorX；controllerY = runtimeY + sourceAnchorY。
    // 当 controller 指针暂时不可读时，dev4 才用这条已由原版代码证明的公式做回退。
    static constexpr size_t kSf2AnchorX = 0x6C;
    static constexpr size_t kSf2AnchorY = 0x6E;
    static constexpr size_t kPresenceEnabled = 0x70;
    static constexpr size_t kInteractionMode = 0x73;
    static constexpr size_t kCollisionProximityFlag = 0x74;
    static constexpr size_t kRenderUpdateSuppression = 0x75;
    static constexpr size_t kBehaviorType = 0x76;
    static constexpr size_t kTouchEventId = 0xEA;      // 接触/接近路径使用的事件 ID，u8
    static constexpr size_t kManualEventId = 0xEB;     // 主动交互路径使用的事件 ID，u8
    static constexpr size_t kPresenceGameVar = 0xFF;
    static constexpr size_t kDefaultOrientation = 0x103;
    static constexpr size_t kRecordSize = 0x227;
}

// 原版 SF2 Controller 在实体接触判定中还携带一套“真正用于碰撞/触发”的矩形。
// 这组字段不是凭截图猜出来的，而是直接来自原版 0x004080D0 -> 0x00408320：
// 1. 0x004080D0 会先把参与接触判定的 Controller 切到 Section 0；
// 2. 然后读取 Controller+0x48 指向的 Section 头；
// 3. Section 头 +0/+4 是相对 Controller 根坐标的左/上偏移，+8/+0x0C 是宽/高；
// 4. 0x00408320 就拿这四个值做第一层 AABB 重叠判断。
//
// Quest 不调用 0x00407200 去“切 Section”，因为那会修改原版 Controller 状态。
// 我们只读 Controller+0x34 的 SF2 数据基址与 +0x38 的 Section 偏移表，自己算出 Section 0
// 的头地址，得到和原版触发判定同源的矩形。因此这里仍然遵守“任务系统只读原版状态”。
namespace Sf2ControllerOffset {
    static constexpr size_t kWorldX = 0x00;            // Controller 根世界 X。
    static constexpr size_t kWorldY = 0x04;            // Controller 根世界 Y。
    static constexpr size_t kSf2DataBase = 0x34;       // 指向 SF2 已加载数据基址。
    static constexpr size_t kSectionOffsets = 0x38;    // 指向每个 Section 相对 SF2 基址的 DWORD 偏移表。
}

// SF2 文件头自己也记录了 Section 数量。原版 0x40720C~0x407217 会读取
// [controller+0x34]+0x14 的 WORD，并在 sectionIndex 越界时拒绝切换。
// Quest 固定读取 Section 0，所以确认 sectionCount>0 就是对应的只读边界检查。
namespace Sf2FileHeaderOffset {
    static constexpr size_t kSectionCount = 0x14;
}

namespace Sf2SectionHeaderOffset {
    static constexpr size_t kLocalLeft = 0x00;         // 原版 0x4080D0 读取：矩形左边相对 Controller X 的偏移。
    static constexpr size_t kLocalTop = 0x04;          // 原版 0x4080D0 读取：矩形上边相对 Controller Y 的偏移。
    static constexpr size_t kWidth = 0x08;             // 原版 0x4080D0 读取：矩形宽。
    static constexpr size_t kHeight = 0x0C;            // 原版 0x4080D0 读取：矩形高。
    static constexpr size_t kEntryCount = 0x54;        // 原版 0x407224 检查的 Section entry 数；0 代表该 Section 不可用。
    static constexpr size_t kMinimumReadableSize = 0x56;
}

// ============================================================================
// 2. 全局运行状态
// ============================================================================

static HMODULE g_module = nullptr;

// dev6zd 起，ASI 的“到底由谁负责写游戏代码段”不再靠隐含约定，而是明确记录。
// None：业务还没完成初始化，任何代码都不能写 RPG.exe。
// Standalone：同目录没有 Castle_Runtime.dll，允许沿用本插件自己验证过的 VirtualProtect Hook。
// Integrated：存在可用 Castle_Runtime.dll，两个入口补丁全部由 Runtime Hook 事务拥有。
// 这个枚举只记录所有权，绝不允许 Integrated 失败以后偷偷改成 Standalone。
enum class HookOwnerMode {
    None,
    Standalone,
    Integrated
};
static HookOwnerMode g_hookOwnerMode = HookOwnerMode::None;
static const CastleDisplayApiV1* g_runtimeDisplayApi = nullptr;
static const CastleOverlayApiV1* g_runtimeOverlayApi = nullptr;
static const CastleScheduleApiV1* g_runtimeScheduleApi = nullptr;
static const CastleGameStateApiV1* g_runtimeGameStateApi = nullptr;
static CastleLeaseHandle g_runtimeOverlayClient = 0u;
static CastleTaskHandle g_runtimeExplorationTask = 0u;
static const CastleLogApiV1* g_runtimeLogApi = nullptr;
static CastlePluginHandle g_runtimeLogPlugin = 0u;

// SDK Client 自己已经用原子状态机保证业务初始化只执行一次；这里的布尔值只是防止
// 公共初始化辅助函数被本文件内部重复调用，不承担跨线程 Bootstrap 仲裁职责。
static bool g_businessInitialized = false;
static std::wstring g_moduleDir;
static HANDLE g_log = INVALID_HANDLE_VALUE;
static bool g_enabled = true;
static bool g_diagnosticHud = true;
static bool g_worldMarkers = true;
static bool g_questHud = true;
static bool g_debugShowRuntimeEntities = false;
// dev4 默认关闭“全实体调试层”：正式 Quest Marker 已开始使用真实渲染锚点，
// 不再让一屏蓝框妨碍正常游玩。Ctrl+F11 仍可即时打开，用于核对 NPC/出口实体。
static bool g_debugLegacyShowAllInteractiveEntities = false;
static bool g_debugTestWorldMarker = false;
static int32_t g_debugWorldX = 0;
static int32_t g_debugWorldY = 0;
static bool g_questPanel = false;
static bool g_logGameVarChanges = true;
static int g_markerSize = 14;
static int g_markerYOffset = -34;
static int g_markerOffsetX = 0;
static int g_markerOffsetY = 0;

// dev6q：Marker 的任务类型颜色继续允许从 INI 直接使用常见的 R,G,B 三整数格式配置。
// 这里故意不用“R/G/B 各占一行”的配置方式，因为用户通常会从取色器或图片软件直接复制完整 RGB。
// 这四组颜色只影响世界 Marker 与任务清单左侧的小图标，不改变任务名称本身已经验收的文字颜色。
static int g_markerMainR = 255;
static int g_markerMainG = 205;
static int g_markerMainB = 70;
static int g_markerSideR = 80;
static int g_markerSideG = 220;
static int g_markerSideB = 220;
static int g_markerTimedR = 255;
static int g_markerTimedG = 150;
static int g_markerTimedB = 60;
static int g_markerEndingTimedR = 190;
static int g_markerEndingTimedG = 75;
static int g_markerEndingTimedB = 75;

// dev6b 新增：实体 Marker 的最终屏幕投影模式。
//
// WorldCamera：
//   MarkerX = runtime.worldX - CameraX
//   MarkerY = runtime.worldY - CameraY
//
// ControllerCamera：
//   MarkerX = SF2ControllerX - CameraX
//   MarkerY = SF2ControllerY - CameraY
//
// dev4/dev5 曾把 ControllerCamera 当成正式答案，但完整实机日志反复证明 controller 根坐标
// 相对 world 坐标稳定少了大约 (320,260)。例如 mp3001 的“救小鬼”：
//   World=(2801,1710), Camera=(2231,1320) -> WorldCamera=(570,390)；
//   Controller=(2481,1450)              -> ControllerCamera=(250,130)。
// 用户截图里的真实触发位置与 (570,390) 一致，而旧紫色 Marker 漂在错误位置。
// 所以 dev6b 默认改回 WorldCamera，同时保留 ControllerCamera 作为诊断回退，绝不把尚未完全闭合的
// 原版 SF2 layer 变换硬编码成不可逆的“真公式”。
enum class EntityMarkerProjection {
    WorldCamera,
    ControllerCamera
};
static EntityMarkerProjection g_entityMarkerProjection = EntityMarkerProjection::WorldCamera;

static int g_fontSize = 14;
static int g_questHudMaxWidth = 440;
static int g_questHudMarginLeft = 12;

// dev6f：把任务 HUD 的两种“纵向留白”从硬编码常量改成 INI 配置。
//
// g_questItemSpacing：
//   一个任务的最后一行说明画完后，到下一个任务标题之间额外空出的像素。
//   以前这里固定写死为 9，所以用户想把任务排得更紧或更松时必须改源码。
//
// g_questObjectiveSpacing：
//   同一个任务内部，任务标题画完后，到第一行任务说明之间额外空出的像素。
//   以前没有独立配置；标题行自己的行高仍由 FontSize 决定。
//
// 两个值都只影响任务 Tracker 的排版，不影响 Marker、坐标、Route、GameVar 或任何原版逻辑。
static int g_questItemSpacing = 0;
static int g_questObjectiveSpacing = 0;

// dev6zc：player_notes 是任务正文下面的“任务注释”。
// QuestNoteSpacing 控制 objective 与第一条注释之间的额外像素；
// QuestNoteLineSpacing 控制多条注释/注释自动换行之间的额外像素。
// 两项都只影响 HUD 排版，可在运行中用 Ctrl+F8 热重载。
static int g_questNoteSpacing = 3;
static int g_questNoteLineSpacing = 2;

static std::wstring g_fontName = L"Microsoft JhengHei UI";
static HFONT g_font = nullptr;

// dev4 延续已冻结结论：废止 DirectDraw Surface::GetDC / ReleaseDC。
// 用户实机确认 dev1e/dev2 的 GDI-on-DirectDraw 路径会让人物 Sprite 比例异常。
// 新后端只 Lock renderer+8，直接写本帧 backbuffer 像素，再 Unlock 后交给原版 Present。
static bool g_softwareOverlay = true;
static IDirectDrawSurface* g_lastOverlaySurface = nullptr;
static HRESULT g_lastOverlayLockResult = S_OK;
static DWORD g_lastOverlayBpp = 0;
static bool g_loggedFirstOverlaySuccess = false;

// 场景实体表只在“场景/实体布局发生变化”时写一次日志。
// 这样用户游戏开着就能直接打开日志，看 NPC 名称、EA/EB 和真实屏幕锚点，
// 又不会因为 Present 每帧执行而生成巨量日志。
static std::string g_lastEntityDumpKey;

// Hook 安装时保存被覆盖的 6 字节，便于插件被显式卸载时恢复。
static BYTE g_presentOriginalBytes[6] = {};
static bool g_presentHookInstalled = false;
static void* g_presentTrampoline = nullptr;

// dev4 新增：捕获探索场景管理器 this。
// 这个指针不是 Quest 自己分配的，它属于原版游戏；Hook 只把地址抄进来，绝不改管理器字段。
// Present 线程读取前会用 VirtualQuery/范围检查，所以场景切换时短暂的旧指针只会被判无效，不会硬解引用。
static PVOID volatile g_explorationManager = nullptr;

// dev6b 新增“探索管理器捕获序号”。每次原版 0x409580 真正进入一次，Hook 都把这个序号 +1。
// CaptureSnapshot() 因此可以区分：
//   1) 这是场景 ID 改变之前留下来的旧 Manager；
//   2) 这是场景 ID 改变之后，原版至少又执行过一次探索更新后捕获到的 Manager。
//
// LONG 在 32 位 Windows 上是 32 位整数，InterlockedIncrement/CompareExchange 对它提供原子访问；
// 这里不需要锁，也不会在 Hook 内分配内存。即使运行极长时间发生整数回绕，比较只用于很短的场景切换窗口，
// 对实际游戏会话没有可见影响。
static volatile LONG g_explorationCaptureSerial = 0;

// 以下四个字段只在 Present/Overlay 线程的 CaptureSnapshot() 中读写，不在 Hook 里碰 std::string。
// 它们共同组成“场景代际”：Scene ID 一变，旧 Manager 立刻作废；只有新的 0x409580 捕获才能重新启用对象表。
static std::string g_snapshotSceneId;
static uint32_t g_snapshotSceneGeneration = 0;
static LONG g_sceneChangeCaptureSerial = 0;
static PVOID g_managerRejectedAtSceneChange = nullptr;

static BYTE g_explorationUpdateOriginalBytes[6] = {};
static bool g_explorationUpdateHookInstalled = false;
static void* g_explorationUpdateTrampoline = nullptr;

// __thiscall 的原函数：this 放在 ECX，没有普通参数，返回 void。
using PresentFn = void(__thiscall*)(void* self);
static PresentFn g_originalPresent = nullptr;
using ExplorationUpdateFn = void(__thiscall*)(void* self);
static ExplorationUpdateFn g_originalExplorationUpdate = nullptr;

// ============================================================================
// 3. 很基础的字符串/日志辅助函数
// ============================================================================

static std::wstring GetModuleDirectory(HMODULE module) {
    wchar_t path[MAX_PATH] = {};
    DWORD len = GetModuleFileNameW(module, path, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        return L".";
    }
    std::wstring full(path, len);
    size_t slash = full.find_last_of(L"\\/");
    return slash == std::wstring::npos ? L"." : full.substr(0, slash);
}

static std::wstring JoinPath(const std::wstring& a, const std::wstring& b) {
    if (a.empty()) return b;
    if (a.back() == L'\\' || a.back() == L'/') return a + b;
    return a + L"\\" + b;
}

static std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) return L"";
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0);
    if (count <= 0) {
        // 如果配置里出现了坏 UTF-8，不让整个插件崩溃；改成替换式转换。
        count = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    }
    if (count <= 0) return L"";
    std::wstring out(static_cast<size_t>(count), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), &out[0], count);
    return out;
}

static std::string WideToUtf8(const std::wstring& text) {
    if (text.empty()) return "";
    int count = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    if (count <= 0) return "";
    std::string out(static_cast<size_t>(count), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), &out[0], count, nullptr, nullptr);
    return out;
}

static std::string Trim(std::string s) {
    auto notSpace = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

static std::string ToLowerAscii(std::string s) {
    for (char& c : s) {
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }
    return s;
}

static bool IEqualsAscii(const std::string& a, const std::string& b) {
    return ToLowerAscii(a) == ToLowerAscii(b);
}

static void LogRaw(const std::string& line) {
    if (g_runtimeLogApi && g_runtimeLogPlugin && !line.empty()) {
        CastleLogRecordV1 record = {};
        record.magic = CASTLE_LOG_RECORD_MAGIC;
        record.struct_size = CASTLE_SIZEOF_LOG_RECORD_V1;
        record.version = CASTLE_LOG_STRUCTURE_VERSION_1;
        record.level = CASTLE_LOG_INFO;
        record.message.data = line.data();
        record.message.length = static_cast<CastleU32>(line.size());
        g_runtimeLogApi->WritePluginLine(g_runtimeLogPlugin, &record);
        return;
    }
    // dev4 不再使用 fopen/_wfopen，因为 C Runtime 默认的共享模式可能让外部编辑器在游戏运行时打不开日志。
    // 这里直接使用 Win32 文件句柄，并在 OpenLog() 中显式声明 FILE_SHARE_READ | FILE_SHARE_WRITE |
    // FILE_SHARE_DELETE。这样 ASI 保持日志句柄打开时，记事本、Notepad++ 等仍可以同时读取该文件。
    if (g_log == INVALID_HANDLE_VALUE) return;

    DWORD written = 0;
    if (!line.empty()) {
        WriteFile(g_log, line.data(), static_cast<DWORD>(line.size()), &written, nullptr);
    }
    static const char newline[] = "\r\n";
    WriteFile(g_log, newline, 2, &written, nullptr);

    // 诊断期优先保证“马上打开日志就能看到最新内容”。
    // FlushFileBuffers 会比只刷 C Runtime 缓冲更明确地把当前写入推到文件系统。
    // 任务插件日志频率很低，不在 Present 每帧写，因此这点开销可以接受。
    FlushFileBuffers(g_log);
}

static void Log(const char* format, ...) {
    char buffer[2048] = {};
    va_list args;
    va_start(args, format);
    _vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
    va_end(args);
    LogRaw(buffer);
}

static void OpenLog() {
    if (g_runtimeLogApi && g_runtimeLogPlugin) {
        LogRaw("《幽城幻剑录》Castle_Quest v0.1-dev6zd 启动。");
        LogRaw("[边界] Quest通过Runtime GamePhase/Overlay/Display/GameState运行，不再拥有Present与探索入口Hook。");
        LogRaw("[构建] RuntimeSDK Client 与版本化服务已接入；任务业务仍保持独立。");
        return;
    }
    std::wstring path = JoinPath(g_moduleDir, L"Castle_Quest.log");

    // CREATE_ALWAYS 与旧版行为一致：每次启动清空上一轮日志。
    // 三个 FILE_SHARE_* 是 dev4 延续的关键修复：插件写日志时不再独占文件。
    g_log = CreateFileW(path.c_str(), GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (g_log == INVALID_HANDLE_VALUE) return;

    // UTF-8 BOM 让 Windows 记事本也能稳定识别中文日志。
    const unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    DWORD written = 0;
    WriteFile(g_log, bom, static_cast<DWORD>(sizeof(bom)), &written, nullptr);
    FlushFileBuffers(g_log);

    LogRaw("《幽城幻剑录》Castle_Quest v0.1-dev6zd 启动。");
    LogRaw("[边界] 剧情/GameVar/原版存档只读；Present 与探索管理器入口 Hook 在整合模式由 RuntimeSDK 事务拥有，独立模式才使用本地 Hook。");
    LogRaw("[构建] dev6zd 已迁入主项目 src/Quest 结构，并把 RuntimeSDK Client 源码编入 ASI；Quest C++ 仍保持 /Od 稳定构建策略。");
    LogRaw("[任务数据] dev6zd：每条任务固定为 Qxxx.toml 原版 Base + 同名 Qxxx_addon.toml；Base v7 只允许原版 canonical Stage。");
    LogRaw("[任务数据] dev6zd 已删除旧 Warning 与 Guidance 运行体系；人工体验步骤只允许由 Addon [[insert]] 提供。");
    LogRaw("[日志] 共享模式已启用：游戏运行中允许外部程序读取/刷新 Castle_Quest.log。");
    LogRaw("[渲染] dev1e/dev2 的 DirectDraw GetDC/GDI 后端已废止；dev6zd 继续不对游戏 Surface 取得 HDC。");
    LogRaw("[调试] dev6zd 继承 dev6y Ctrl+F11 修复：MouseWorld 首选原版 0x89F7C0/0x89F7C4；Win32 自算坐标仅作显式 Fallback。");
}

// ============================================================================
// 4. 安全读取内存
// ============================================================================

// 逆向插件最怕“地址不对还硬读”。VirtualQuery 可以先问 Windows：
// “这一小段地址当前是不是已提交、且允许读？”
// 它不能证明业务语义正确，但能避免最直接的访问无效页崩溃。
static bool IsReadableRange(const void* ptr, size_t size) {
    if (!ptr || size == 0) return false;
    uintptr_t begin = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t end = begin + size;
    if (end < begin) return false; // 整数溢出保护

    uintptr_t cursor = begin;
    while (cursor < end) {
        MEMORY_BASIC_INFORMATION mbi = {};
        if (VirtualQuery(reinterpret_cast<const void*>(cursor), &mbi, sizeof(mbi)) != sizeof(mbi)) {
            return false;
        }
        if (mbi.State != MEM_COMMIT) return false;
        if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
        DWORD p = mbi.Protect & 0xFF;
        bool readable = p == PAGE_READONLY || p == PAGE_READWRITE || p == PAGE_WRITECOPY ||
                        p == PAGE_EXECUTE_READ || p == PAGE_EXECUTE_READWRITE || p == PAGE_EXECUTE_WRITECOPY;
        if (!readable) return false;
        uintptr_t regionEnd = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        if (regionEnd <= cursor) return false;
        cursor = std::min(regionEnd, end);
    }
    return true;
}

template <typename T>
static bool SafeRead(uintptr_t address, T& out) {
    const void* ptr = reinterpret_cast<const void*>(address);
    if (!IsReadableRange(ptr, sizeof(T))) return false;
    std::memcpy(&out, ptr, sizeof(T));
    return true;
}

template <typename T>
static bool SafeReadPtr(const void* base, size_t offset, T& out) {
    uintptr_t address = reinterpret_cast<uintptr_t>(base) + offset;
    return SafeRead(address, out);
}

static bool BytesEqual(uintptr_t address, const BYTE* expected, size_t count) {
    if (!IsReadableRange(reinterpret_cast<const void*>(address), count)) return false;
    return std::memcmp(reinterpret_cast<const void*>(address), expected, count) == 0;
}

// ============================================================================
// 4A. dev6b 临时显示兼容层：宽屏 CameraPlan + Quest 严格可视门控
// ============================================================================

// 这层代码是“公共 Runtime API 尚未落地前的临时兼容实现”。
// 它只读取原版全局量，并复刻当前 Castle_Widescreen v0.11-poc11 的 CameraPlan 数学。
// 等项目公共 API 可用后，这一层应替换成正式的 WorldToScreen/VisualState 服务调用；
// Quest 业务层不应该永久复制 Widescreen 内部实现。
struct DisplayGeometry {
    bool valid = false;
    bool widescreen = false;
    uint32_t generation = 0;
    int outputWidth = 640;
    int outputHeight = 480;
    int logicalWidth = 640;
    int logicalHeight = 480;
    int sideWidth = 0;
    int effectiveCameraX = 0;
    int effectiveCameraY = 0;
    int leftWorldPixels = 0;
    int rightWorldPixels = 0;
};

enum class VisualGateReason {
    Visible,
    InvalidSnapshot,
    Dialogue,
    KnownUi,
    Movie,
    NoEventTable,
    EventBusy,
    EventYieldOrBlock,
    InputClosed,
    MapModeInactive,
    NoExplorationManager
};

static const char* VisualGateReasonName(VisualGateReason reason) {
    switch (reason) {
        case VisualGateReason::Visible: return "FreeExploration";
        case VisualGateReason::InvalidSnapshot: return "InvalidSnapshot";
        case VisualGateReason::Dialogue: return "Dialogue";
        case VisualGateReason::KnownUi: return "KnownUI";
        case VisualGateReason::Movie: return "Movie";
        case VisualGateReason::NoEventTable: return "NoEventTable";
        case VisualGateReason::EventBusy: return "EventBusy";
        case VisualGateReason::EventYieldOrBlock: return "EventYieldOrBlock";
        case VisualGateReason::InputClosed: return "InputClosed";
        case VisualGateReason::MapModeInactive: return "MapModeInactive";
        case VisualGateReason::NoExplorationManager: return "NoExplorationManager";
        default: return "Unknown";
    }
}

// Quest 的规则比 Backlog 更严格：
// - Backlog 在“稳定展开的真实对话”上仍允许打开；
// - Quest 在任何对话、提示、选择、菜单、战斗、标题、电影里都完全不画。
//
// 注意：这只控制“画不画”。任务条件、GameVar 读取、日志取证仍可继续在后台执行，
// 所以退出对话/菜单的下一帧就能恢复到正确任务状态。
static VisualGateReason QueryQuestVisualGate(bool requireExplorationManager, bool sceneObjectListValid) {
    CastleGameStateSnapshotV1 runtimeState = {};
    if (!g_runtimeGameStateApi) return VisualGateReason::InvalidSnapshot;
    runtimeState.magic = CASTLE_GAME_SNAPSHOT_MAGIC;
    runtimeState.struct_size = CASTLE_SIZEOF_GAME_STATE_SNAPSHOT_V1;
    runtimeState.version = CASTLE_GAME_STATE_STRUCTURE_VERSION_1;
    if (g_runtimeGameStateApi->GetSnapshot(&runtimeState) < 0) {
        return VisualGateReason::InvalidSnapshot;
    }
    if ((runtimeState.flags & CASTLE_GAME_FLAG_DIALOGUE_ACTIVE) != 0u)
        return VisualGateReason::Dialogue;
    if ((runtimeState.flags & (CASTLE_GAME_FLAG_BATTLE_ACTIVE |
                               CASTLE_GAME_FLAG_MENU_ACTIVE)) != 0u)
        return VisualGateReason::KnownUi;
    if ((runtimeState.flags & CASTLE_GAME_FLAG_MOVIE_ACTIVE) != 0u)
        return VisualGateReason::Movie;
    if ((runtimeState.flags & CASTLE_GAME_FLAG_EVENT_TABLE_READY) == 0u)
        return VisualGateReason::NoEventTable;
    if (runtimeState.active_event_id != 0u) return VisualGateReason::EventBusy;
    if (runtimeState.event_yield != 0u || runtimeState.event_blocked != 0u)
        return VisualGateReason::EventYieldOrBlock;
    if (runtimeState.map_input_gate == 0u) return VisualGateReason::InputClosed;
    if (runtimeState.map_key_mode == 0u) return VisualGateReason::MapModeInactive;
    if (requireExplorationManager && !sceneObjectListValid)
        return VisualGateReason::NoExplorationManager;
    return VisualGateReason::Visible;

#if 0
    // 旧版逐地址门控已经由 Runtime GameState 快照替代，保留在 Git 历史中即可。
    // 对话是否“真的正在画”不能只看 DialogueMode。Backlog 的已验证逻辑证明：
    // 当 DialogueId==0（没有真实对话）时，mode 并不参与“自由探索”资格判断；
    // 因此 Quest 也绝不能擅自断言“mode 非 0 = 有对话”，否则某些正常探索状态可能被永久隐藏。
    //
    // 这里使用更直接的三个消息生命周期信号：
    // - DialogueId：Widescreen 当前也把它作为“屏幕存在原版消息 UI”的主信号；
    // - TargetState / CurrentState：覆盖消息框展开、收起和选择状态的过渡帧。
    // 三者任一非 0 就隐藏；DialogueMode 目前只保留地址常量，供后续诊断/API 适配使用。
    uint32_t dialogueId = 0;
    uint8_t dialogueTarget = 0;
    uint8_t dialogueCurrent = 0;
    SafeRead(Address::kDialogueId, dialogueId);
    SafeRead(Address::kDialogueTargetState, dialogueTarget);
    SafeRead(Address::kDialogueCurrentState, dialogueCurrent);
    if (dialogueId != 0 || dialogueTarget != 0 || dialogueCurrent != 0)
        return VisualGateReason::Dialogue;

    if (GlobalPointerPresent(Address::kBattleUi) || GlobalPointerPresent(Address::kResultUi) ||
        GlobalPointerPresent(Address::kTargetSelectorUi) || GlobalPointerPresent(Address::kTitleUi) ||
        GlobalPointerPresent(Address::kInterfaceUi) || GlobalPointerPresent(Address::kSavePointUi) ||
        GlobalPointerPresent(Address::kInnUi) || GlobalPointerPresent(Address::kSynthesisUi) ||
        GlobalPointerPresent(Address::kShopUi)) {
        return VisualGateReason::KnownUi;
    }

    uint8_t* movie = nullptr;
    if (SafeRead(Address::kMovieObject, movie) && movie &&
        IsReadableRange(movie, Address::kMovieActiveFlagOffset + 1)) {
        uint8_t active = 0;
        if (SafeRead(reinterpret_cast<uintptr_t>(movie) + Address::kMovieActiveFlagOffset, active) && active != 0)
            return VisualGateReason::Movie;
    }

    if (!GlobalPointerPresent(Address::kEventTable)) return VisualGateReason::NoEventTable;

    uint32_t actionBusy = 0;
    SafeRead(Address::kActiveEventId, actionBusy);
    if (actionBusy != 0) return VisualGateReason::EventBusy;

    uint8_t eventYield = 0, eventBlock = 0;
    SafeRead(Address::kEventYieldFlag, eventYield);
    SafeRead(Address::kEventBlockFlag, eventBlock);
    if (eventYield != 0 || eventBlock != 0) return VisualGateReason::EventYieldOrBlock;

    uint8_t inputGate = 0;
    SafeRead(Address::kMapInputGate, inputGate);
    if (inputGate == 0) return VisualGateReason::InputClosed;

    int32_t mapMode = 0;
    SafeRead(Address::kMapKeyMode, mapMode);
    if (mapMode == 0) return VisualGateReason::MapModeInactive;

    // Quest 还额外要求探索管理器当前确实可用。这样即使某个未知菜单尚未加入 owner 表，
    // 只要它让探索更新停止，Quest 也会 fail-closed，而不是把上一张地图 Marker 留在界面上。
    if (requireExplorationManager && !sceneObjectListValid)
        return VisualGateReason::NoExplorationManager;

    return VisualGateReason::Visible;
#endif
}

static void MaybeLogVisualGate(VisualGateReason reason) {
    static int previous = -1;
    const int current = static_cast<int>(reason);
    if (current == previous) return;
    previous = current;
    Log("[可视门控] %s：%s", reason == VisualGateReason::Visible ? "VISIBLE" : "HIDDEN", VisualGateReasonName(reason));
}

// 复刻 Widescreen v0.11-poc11 的 calculate_camera_plan()。
// 最关键的点：输出是 854/1120 时，真正显示中心 Camera 可能已经被宽屏插件提前 clamp，
// 原版 0x978514 在 Hook 返回后却会恢复成游戏自己的 Camera。Quest 不能再只用那个恢复后的值。
static DisplayGeometry CalculateDisplayGeometry(int originalCameraX, int originalCameraY, int surfaceWidth, int surfaceHeight) {
    DisplayGeometry out;
    CastleDisplayGeometryV1 runtimeGeometry = {};
    (void)originalCameraX;
    (void)originalCameraY;
    (void)surfaceWidth;
    (void)surfaceHeight;
    if (!g_runtimeDisplayApi) return out;
    runtimeGeometry.magic = CASTLE_DISPLAY_GEOMETRY_MAGIC;
    runtimeGeometry.struct_size = CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1;
    runtimeGeometry.api_version = CASTLE_DISPLAY_API_VERSION_1;
    if (g_runtimeDisplayApi->GetGeometry(&runtimeGeometry) < 0) return out;
    out.valid = runtimeGeometry.projection_scope != CASTLE_PROJECTION_NONE;
    out.widescreen = runtimeGeometry.display_mode == CASTLE_DISPLAY_WIDE_WORLD;
    out.generation = runtimeGeometry.generation;
    out.outputWidth = static_cast<int>(runtimeGeometry.output_width);
    out.outputHeight = static_cast<int>(runtimeGeometry.output_height);
    out.logicalWidth = static_cast<int>(runtimeGeometry.logical_width);
    out.logicalHeight = static_cast<int>(runtimeGeometry.logical_height);
    out.sideWidth = runtimeGeometry.center_x;
    out.effectiveCameraX = runtimeGeometry.effective_camera_x;
    out.effectiveCameraY = runtimeGeometry.effective_camera_y;
    out.leftWorldPixels = static_cast<int>(runtimeGeometry.left_world_width);
    out.rightWorldPixels = static_cast<int>(runtimeGeometry.right_world_width);
    return out;

#if 0
    // 已删除的 dev6d 临时算法保留在版本历史中；正式构建绝不能再复制 Widescreen CameraPlan。
    out.outputWidth = surfaceWidth > 0 ? surfaceWidth : 640;
    out.outputHeight = surfaceHeight > 0 ? surfaceHeight : 480;
    out.effectiveCameraX = originalCameraX;
    out.effectiveCameraY = originalCameraY;

    int32_t viewportW = 0, viewportH = 0, minX = 0, maxX = 0;
    const bool haveBounds = SafeRead(Address::kCameraViewW, viewportW) && SafeRead(Address::kCameraViewH, viewportH) &&
                            SafeRead(Address::kCameraMinX, minX) && SafeRead(Address::kCameraMaxX, maxX);

    // 原版通常是 640x480。若当前 Surface 没有横向扩展，直接回退原版坐标，不做任何宽屏推断。
    if (!haveBounds || viewportW <= 0 || viewportH <= 0 || surfaceWidth <= viewportW || maxX <= minX) {
        out.logicalWidth = viewportW > 0 ? viewportW : 640;
        out.logicalHeight = viewportH > 0 ? viewportH : 480;
        out.valid = true;
        return out;
    }

    const int32_t mapWidth = maxX - minX;
    const int32_t normalMaxCamera = maxX - viewportW;
    if (normalMaxCamera < minX) {
        // 场景尚未建立好的瞬态与 Widescreen 一样 fail-closed 到原版 Camera。
        out.logicalWidth = viewportW;
        out.logicalHeight = viewportH;
        out.valid = true;
        return out;
    }

    out.logicalWidth = viewportW;
    out.logicalHeight = viewportH;
    out.sideWidth = std::max(0, (surfaceWidth - viewportW) / 2);
    out.widescreen = out.sideWidth > 0;

    if (mapWidth >= surfaceWidth) {
        const int32_t wideMinCamera = minX + out.sideWidth;
        const int32_t wideMaxCamera = maxX - viewportW - out.sideWidth;
        int32_t center = originalCameraX;
        if (center < wideMinCamera) center = wideMinCamera;
        if (center > wideMaxCamera) center = wideMaxCamera;
        out.effectiveCameraX = center;
        out.leftWorldPixels = out.sideWidth;
        out.rightWorldPixels = out.sideWidth;
    } else {
        // 地图宽度介于原版 viewport 与当前宽屏输出之间时，Widescreen 会把有限额外世界平均分左右。
        int32_t totalExtra = mapWidth - viewportW;
        if (totalExtra < 0) totalExtra = 0;
        int32_t leftRoom = totalExtra / 2;
        int32_t rightRoom = totalExtra - leftRoom;
        leftRoom = std::max(0, std::min(leftRoom, out.sideWidth));
        rightRoom = std::max(0, std::min(rightRoom, out.sideWidth));
        int32_t center = minX + leftRoom;
        if (center < minX) center = minX;
        if (center > normalMaxCamera) center = normalMaxCamera;
        out.effectiveCameraX = center;
        out.leftWorldPixels = leftRoom;
        out.rightWorldPixels = rightRoom;
    }

    out.valid = true;
    return out;
#endif
}

// marker.screenX 目前是“按原版 Camera 算出的 640 逻辑坐标”。
// 先还原成世界/Controller 坐标，再换算到宽屏真正显示的 effectiveCameraX。
// 这样 640 模式自然得到原值；854/1120 模式也不会把固定 +107/+240 当成万能补丁。
static POINT ProjectLogicalPointToSurface(int logicalScreenX, int logicalScreenY,
                                          int originalCameraX, int originalCameraY,
                                          const DisplayGeometry& geometry) {
    const int worldLikeX = logicalScreenX + originalCameraX;
    const int worldLikeY = logicalScreenY + originalCameraY;
    POINT out;
    CastleWorldToScreenRequestV1 request = {};
    CastleScreenProjectionV1 projection = {};
    request.magic = CASTLE_WORLD_TO_SCREEN_MAGIC;
    request.struct_size = CASTLE_SIZEOF_WORLD_TO_SCREEN_V1;
    request.request_version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    request.requested_generation = geometry.generation;
    request.world_x = worldLikeX;
    request.world_y = worldLikeY;
    projection.magic = CASTLE_SCREEN_PROJECTION_MAGIC;
    projection.struct_size = CASTLE_SIZEOF_SCREEN_PROJECTION_V1;
    projection.result_version = CASTLE_DISPLAY_STRUCTURE_VERSION_1;
    if (g_runtimeDisplayApi &&
        g_runtimeDisplayApi->WorldToScreen(&request, &projection) >= 0) {
        out.x = projection.screen_x;
        out.y = projection.screen_y;
        return out;
    }
    // 代次过期或当前模式不可投影时返回远离画布的点，调用方会按既有规则隐藏/夹边。
    out.x = -0x4000;
    out.y = -0x4000;
    return out;
}

// ============================================================================
// 5. 原版运行时快照
// ============================================================================

struct RuntimeEntityView {
    size_t index = 0;
    int32_t worldX = 0;
    int32_t worldY = 0;

    // worldAnchorValid 表示 runtime Entity 自己的 WorldX/WorldY 两个字段都成功读取。
    // dev6d 会把 world-camera 作为默认 Marker 坐标，但仍把“读取是否真的成功”单独记录，
    // 这样某个动态对象如果正处于构造/销毁中，就不会拿默认的 (0,0) 冒充合法锚点。
    bool worldAnchorValid = false;
    int32_t worldScreenX = 0;
    int32_t worldScreenY = 0;

    // renderBaseX/renderBaseY 来自原版 SF2 controller 根坐标。
    // 它仍然非常有价值：一方面用于和原版 0x4075E0 绘制链继续核对；另一方面可通过 INI
    // 一键切回旧 dev5 投影，做同一编译版 A/B 测试。因此 dev6d 没有删除这套证据。
    bool renderAnchorValid = false;
    int32_t renderBaseX = 0;
    int32_t renderBaseY = 0;
    int32_t controllerScreenX = 0;
    int32_t controllerScreenY = 0;
    const uint8_t* sf2Controller = nullptr;

    // dev6e 新增：原版“接触事件”并不是拿 runtime World 点直接做距离判断，
    // 而是通过 0x4080D0 使用 SF2 Controller 的 Section 0 碰撞矩形。
    // 因此这里把这个矩形完整保留下来。对世界地图的 Route Exit Marker，我们可以直接指向
    // 原版真正会触发 EA 的区域中心，而不是继续把“实体参考点”误当成“入口触发点”。
    bool touchBoxAnchorValid = false;
    int32_t touchBoxLeftWorld = 0;
    int32_t touchBoxTopWorld = 0;
    int32_t touchBoxWidth = 0;
    int32_t touchBoxHeight = 0;
    int32_t touchBoxWorldX = 0;
    int32_t touchBoxWorldY = 0;
    int32_t touchBoxScreenX = 0;
    int32_t touchBoxScreenY = 0;

    // markerAnchorValid + screenX/screenY 才是本帧真正提供给普通 Quest Marker 的最终选择。
    // 默认优先 WorldCamera；若 World 字段暂时不可读，则自动回退到 ControllerCamera，反之亦然。
    bool markerAnchorValid = false;
    int32_t screenX = 0;
    int32_t screenY = 0;

    uint8_t touchEventId = 0;
    uint8_t manualEventId = 0;
    uint8_t interactionMode = 0;
    uint8_t suppression = 0;
    const uint8_t* source = nullptr;
    int sourceOrdinal = -1;
    std::wstring name;
};

struct GameSnapshot {
    bool valid = false;
    const uint8_t* world = nullptr;
    std::string scenePath;
    std::string sceneId;
    int32_t cameraX = 0;
    int32_t cameraY = 0;
    int32_t playerX = 0;
    int32_t playerY = 0;

    // dev4 起，entityCount/objects 表示“探索管理器当前场景对象表”的对象，而不是 0x89F7F0 的固定角色槽。
    // sceneObjectListValid=false 时说明探索管理器这一帧尚未捕获或已处于非探索状态；此时 Quest Marker 不猜 NPC。
    uint32_t entityCount = 0;
    bool sceneObjectListValid = false;
    const uint8_t* explorationManager = nullptr;

    // dev6d 场景代际诊断。sceneGeneration 每次 Scene ID 真正变化时 +1；
    // managerCaptureSerial 是当前被接受的 0x409580 捕获序号。managerRejectedAsStale=1 表示
    // 这一帧特意拒绝了“场景已经变了、Manager 还是旧的”快照，因此 Marker 会宁可短暂不显示也不指错对象。
    uint32_t sceneGeneration = 0;
    LONG managerCaptureSerial = 0;
    bool managerRejectedAsStale = false;

    // fixedActorCount/controlledEntityIndex 只保留给玩家坐标与诊断。它们不再参与 NPC/出口 Marker 解析。
    uint32_t fixedActorCount = 0;
    uint32_t controlledEntityIndex = 0;

    // 当前激活 Inventory Bank。只在地址可读时保存；任务条件 item[N] 会从这里做六类只读统计。
    const uint8_t* activeInventoryBank = nullptr;
    uint32_t activeEventId = 0;
    uint32_t battleLayoutGroup = 0;
    uint32_t battleResult = 0;
    std::vector<RuntimeEntityView> entities;
};

static std::wstring DecodeCp950Name(const uint8_t* bytes, size_t capacity) {
    if (!bytes || capacity == 0 || !IsReadableRange(bytes, capacity)) return L"";
    size_t len = 0;
    while (len < capacity && bytes[len] != 0) ++len;
    if (len == 0) return L"";
    int count = MultiByteToWideChar(950, 0, reinterpret_cast<const char*>(bytes), static_cast<int>(len), nullptr, 0);
    if (count <= 0) return L"";
    std::wstring out(static_cast<size_t>(count), L'\0');
    MultiByteToWideChar(950, 0, reinterpret_cast<const char*>(bytes), static_cast<int>(len), &out[0], count);
    return out;
}

static std::string NormalizeSceneId(const std::string& path) {
    // World descriptor 常见形态：MP0102B\MPMP0102B.SCI
    // 我们只取第一个目录名作为稳定 Scene ID：mp0102b。
    std::string s = path;
    std::replace(s.begin(), s.end(), '/', '\\');
    size_t slash = s.find('\\');
    std::string first = slash == std::string::npos ? s : s.substr(0, slash);
    // 如果拿到的不是目录而是文件名，顺手去掉扩展名。
    size_t dot = first.find('.');
    if (dot != std::string::npos) first = first.substr(0, dot);
    return ToLowerAscii(Trim(first));
}

static int16_t ReadGameVar(const GameSnapshot& snapshot, int index, bool* ok = nullptr) {
    if (ok) *ok = false;
    // 原版 GET_VAR(0) 的特殊规则是恒返回 0，因此插件也必须照做。
    if (index == 0) {
        if (ok) *ok = true;
        return 0;
    }
    if (!snapshot.world || index < 0 || index >= static_cast<int>(WorldOffset::kGameVarCount)) return 0;
    int16_t value = 0;
    uintptr_t address = reinterpret_cast<uintptr_t>(snapshot.world) + WorldOffset::kGameVars + static_cast<size_t>(index) * 2;
    if (!SafeRead(address, value)) return 0;
    if (ok) *ok = true;
    return value;
}

// 原版 0x4391C0 跨六类统计某个 item_id。dev4 不直接 call 原函数，而是把同一只读布局
// 重写成下面的安全读取：每类最多 500 格，quantity 与 item_id 分开存，最后一个 u32 是该类有效格数。
// 这样任务系统可以写 item[451] > 0 来判断“彩石弹珠已经进背包”，同时绝不会修改库存。
static int ReadItemCount(const GameSnapshot& snapshot, int itemId, bool* ok = nullptr) {
    if (ok) *ok = false;
    if (!snapshot.activeInventoryBank || itemId < 0) return 0;

    struct CategoryLayout { size_t quantityOffset; size_t idOffset; size_t countOffset; };
    static const CategoryLayout layouts[6] = {
        {0x0000, 0x07D0, 0x0FA0},
        {0x0FA4, 0x1774, 0x1F44},
        {0x1F48, 0x2718, 0x2EE8},
        {0x2EEC, 0x36BC, 0x3E8C},
        {0x3E90, 0x4660, 0x4E30},
        {0x4E34, 0x5604, 0x5DD4}
    };

    int total = 0;
    for (const CategoryLayout& layout : layouts) {
        uint32_t count = 0;
        if (!SafeReadPtr(snapshot.activeInventoryBank, layout.countOffset, count) || count > 500) return 0;
        for (uint32_t i = 0; i < count; ++i) {
            uint32_t id = 0;
            uint32_t quantity = 0;
            if (!SafeReadPtr(snapshot.activeInventoryBank, layout.idOffset + static_cast<size_t>(i) * 4, id)) return 0;
            if (!SafeReadPtr(snapshot.activeInventoryBank, layout.quantityOffset + static_cast<size_t>(i) * 4, quantity)) return 0;
            if (id == static_cast<uint32_t>(itemId)) {
                if (quantity > static_cast<uint32_t>(0x7FFFFFFF - total)) total = 0x7FFFFFFF;
                else total += static_cast<int>(quantity);
            }
        }
    }
    if (ok) *ok = true;
    return total;
}

// 只读计算一个 SF2 Controller 的“Section 0 接触矩形”。
//
// 为什么这里专门读取 Section 0：
// - 原版 0x00409A50 的 EA 接触事件路径会调用 0x004080D0；
// - 0x004080D0 在做碰撞前明确对两个参与对象执行 0x00407200(controller, 0)；
// - 也就是说，原版决定“玩家是不是碰到这个出口/触发物”时，使用的就是 Section 0 的矩形与像素遮罩。
//
// Quest 只需要 Marker 锚点，不需要复制整套像素级碰撞算法，所以取同一矩形的中心点已经足够：
// 它至少来自原版真实触发区域，而不是 runtime World 这个“实体参考点”。
//
// 特别重要：本函数绝不调用 0x00407200。那是会改 Controller 当前 Section 的原版函数。
// 我们只把它的地址计算过程“照着读出来”，因此不会改变动画、碰撞或剧情状态。
static bool TryReadControllerSection0TouchBox(const uint8_t* controller,
                                               int32_t cameraX,
                                               int32_t cameraY,
                                               RuntimeEntityView& e) {
    if (!controller) return false;

    // Controller 根点就是原版绘制/碰撞坐标系的基准。
    int32_t controllerX = 0;
    int32_t controllerY = 0;
    if (!SafeReadPtr(controller, Sf2ControllerOffset::kWorldX, controllerX) ||
        !SafeReadPtr(controller, Sf2ControllerOffset::kWorldY, controllerY)) {
        return false;
    }

    // 0x407200(0) 的地址计算：
    //   sf2Base = [controller+0x34]
    //   sectionOffsets = [controller+0x38]
    //   section0 = sf2Base + sectionOffsets[0]
    uint8_t* sf2Base = nullptr;
    uint32_t* sectionOffsets = nullptr;
    if (!SafeReadPtr(controller, Sf2ControllerOffset::kSf2DataBase, sf2Base) || !sf2Base ||
        !SafeReadPtr(controller, Sf2ControllerOffset::kSectionOffsets, sectionOffsets) || !sectionOffsets) {
        return false;
    }

    // 复刻 0x40720C~0x407217 的 Section 索引边界检查，但绝不调用会改状态的 0x407200。
    // 如果 SF2 连 Section 0 都没有，后面的 table[0] 就不能被当成合法偏移读取。
    uint16_t sectionCount = 0;
    if (!SafeReadPtr(sf2Base, Sf2FileHeaderOffset::kSectionCount, sectionCount) || sectionCount == 0) return false;
    if (!IsReadableRange(sectionOffsets, sizeof(uint32_t))) return false;

    uint32_t section0Offset = 0;
    if (!SafeReadPtr(sectionOffsets, 0, section0Offset)) return false;

    const uintptr_t sf2BaseAddress = reinterpret_cast<uintptr_t>(sf2Base);
    const uintptr_t sectionAddress = sf2BaseAddress + static_cast<uintptr_t>(section0Offset);
    if (sectionAddress < sf2BaseAddress) return false; // 防止整数回绕。
    const uint8_t* section = reinterpret_cast<const uint8_t*>(sectionAddress);
    if (!IsReadableRange(section, Sf2SectionHeaderOffset::kMinimumReadableSize)) return false;

    // 原版 0x407224 会先看 +0x54 的 entry 数；0 表示这个 Section 没有可用内容。
    uint16_t entryCount = 0;
    if (!SafeReadPtr(section, Sf2SectionHeaderOffset::kEntryCount, entryCount) || entryCount == 0) return false;

    int32_t localLeft = 0;
    int32_t localTop = 0;
    int32_t width = 0;
    int32_t height = 0;
    if (!SafeReadPtr(section, Sf2SectionHeaderOffset::kLocalLeft, localLeft) ||
        !SafeReadPtr(section, Sf2SectionHeaderOffset::kLocalTop, localTop) ||
        !SafeReadPtr(section, Sf2SectionHeaderOffset::kWidth, width) ||
        !SafeReadPtr(section, Sf2SectionHeaderOffset::kHeight, height)) {
        return false;
    }

    // 宽高必须是正常正数。这里给一个很宽松的 8192 上限，只是防止坏指针把几百 MB 的随机数当矩形。
    // 《幽城》实际 640/854/1120 画面与世界地图远小于这个范围，所以不会误杀正常触发区。
    if (width <= 0 || height <= 0 || width > 8192 || height > 8192) return false;

    const int64_t left64 = static_cast<int64_t>(controllerX) + static_cast<int64_t>(localLeft);
    const int64_t top64 = static_cast<int64_t>(controllerY) + static_cast<int64_t>(localTop);
    const int64_t centerX64 = left64 + static_cast<int64_t>(width) / 2;
    const int64_t centerY64 = top64 + static_cast<int64_t>(height) / 2;
    if (left64 < INT32_MIN || left64 > INT32_MAX || top64 < INT32_MIN || top64 > INT32_MAX ||
        centerX64 < INT32_MIN || centerX64 > INT32_MAX || centerY64 < INT32_MIN || centerY64 > INT32_MAX) {
        return false;
    }

    e.touchBoxAnchorValid = true;
    e.touchBoxLeftWorld = static_cast<int32_t>(left64);
    e.touchBoxTopWorld = static_cast<int32_t>(top64);
    e.touchBoxWidth = width;
    e.touchBoxHeight = height;
    e.touchBoxWorldX = static_cast<int32_t>(centerX64);
    e.touchBoxWorldY = static_cast<int32_t>(centerY64);
    e.touchBoxScreenX = e.touchBoxWorldX - cameraX;
    e.touchBoxScreenY = e.touchBoxWorldY - cameraY;
    return true;
}

// 把一个“原版 runtime Entity 指针”转换成 Quest 只读视图。
// dev4 把这段逻辑独立出来，是因为场景 NPC 不再来自连续的 0x89F7F0 固定角色数组，
// 而是来自探索管理器 this+0x2C 的“指针表”。两条路径最终都指向相同的 0x74 runtime record，
// 因此坐标、source SCI、SF2 controller 的读取规则可以共用，避免两份实现日后不同步。
static bool BuildRuntimeEntityView(const uint8_t* record, size_t logicalIndex,
                                   int32_t cameraX, int32_t cameraY,
                                   RuntimeEntityView& e) {
    if (!record || !IsReadableRange(record, RuntimeEntityOffset::kRecordSize)) return false;

    e = RuntimeEntityView{};
    e.index = logicalIndex;

    // 第一步只读 runtime record 自己的世界坐标。
    // 两个 SafeReadPtr 都成功才把 worldAnchorValid 设为 true；这样绝不会把读取失败后默认留下的 0
    // 当成“对象就在世界原点”。这也是 dev6b 能安全把 WorldCamera 升为默认投影的前提。
    const bool gotWorldX = SafeReadPtr(record, RuntimeEntityOffset::kWorldX, e.worldX);
    const bool gotWorldY = SafeReadPtr(record, RuntimeEntityOffset::kWorldY, e.worldY);
    if (gotWorldX && gotWorldY) {
        e.worldAnchorValid = true;
        e.worldScreenX = e.worldX - cameraX;
        e.worldScreenY = e.worldY - cameraY;
    }

    // 第二步读取 runtime+0x70 指向的 source SCI record。
    // EA/EB、名字、SF2 初始 anchor 等静态属性都来自这里；这条链已经被 dev4/dev5 实机日志大量验证。
    uint8_t* source = nullptr;
    if (SafeReadPtr(record, RuntimeEntityOffset::kSourceSciPtr, source) && source &&
        IsReadableRange(source, SourceEntityOffset::kRecordSize)) {
        e.source = source;
        SafeReadPtr(source, SourceEntityOffset::kTouchEventId, e.touchEventId);
        SafeReadPtr(source, SourceEntityOffset::kManualEventId, e.manualEventId);
        SafeReadPtr(source, SourceEntityOffset::kInteractionMode, e.interactionMode);
        SafeReadPtr(source, SourceEntityOffset::kRenderUpdateSuppression, e.suppression);
        e.name = DecodeCp950Name(source + SourceEntityOffset::kName, SourceEntityOffset::kNameCapacity);
    }

    // 第三步继续保留 dev5 的 SF2 controller 根坐标证据。
    // 0x4075E0 的实际 sprite layer 绘制确实会用到 controller+0/+4，但完整 dev5 日志证明 controller 根点
    // 还不是 Quest Marker 想要的“实体世界位置”。因此 dev6b 把它降为并行诊断/回退坐标，而不是删除。
    uint8_t* controller = nullptr;
    if (SafeReadPtr(record, RuntimeEntityOffset::kSf2Controller, controller) && controller &&
        IsReadableRange(controller, 8)) {
        int32_t baseX = 0;
        int32_t baseY = 0;
        if (SafeReadPtr(controller, 0, baseX) && SafeReadPtr(controller, 4, baseY)) {
            e.sf2Controller = controller;
            e.renderBaseX = baseX;
            e.renderBaseY = baseY;
            e.controllerScreenX = baseX - cameraX;
            e.controllerScreenY = baseY - cameraY;
            e.renderAnchorValid = true;

            // dev6e：只要真实 Controller 已经存在，就顺手读取原版 Section 0 接触矩形。
            // 失败不会影响普通 Marker；它只意味着本帧无法使用“真实触发区中心”这个更精确的锚点。
            TryReadControllerSection0TouchBox(controller, cameraX, cameraY, e);
        }
    }

    // controller 在对象刚建立时可能暂时还没准备好。此时沿用已经静态闭合的 0x40A160 公式：
    // controller 初始根点 = runtime World + source +0x6C/+0x6E。
    // 这仍然只作为 ControllerCamera 的回退计算，不改变 WorldCamera 的默认地位。
    if (!e.renderAnchorValid && e.source && e.worldAnchorValid) {
        int16_t anchorX = 0;
        int16_t anchorY = 0;
        if (SafeReadPtr(e.source, SourceEntityOffset::kSf2AnchorX, anchorX) &&
            SafeReadPtr(e.source, SourceEntityOffset::kSf2AnchorY, anchorY)) {
            e.renderBaseX = e.worldX + static_cast<int32_t>(anchorX);
            e.renderBaseY = e.worldY + static_cast<int32_t>(anchorY);
            e.controllerScreenX = e.renderBaseX - cameraX;
            e.controllerScreenY = e.renderBaseY - cameraY;
            e.renderAnchorValid = true;
        }
    }

    // 最后才选“本帧 Marker 实际使用哪个坐标”。
    // 这里故意做双向回退：如果用户在 INI 选择某模式，但某个特殊对象恰好没有那种锚点，
    // 插件会尝试另一种已知坐标，而不是让任务目标凭空消失。
    if (g_entityMarkerProjection == EntityMarkerProjection::WorldCamera) {
        if (e.worldAnchorValid) {
            e.screenX = e.worldScreenX;
            e.screenY = e.worldScreenY;
            e.markerAnchorValid = true;
        } else if (e.renderAnchorValid) {
            e.screenX = e.controllerScreenX;
            e.screenY = e.controllerScreenY;
            e.markerAnchorValid = true;
        }
    } else {
        if (e.renderAnchorValid) {
            e.screenX = e.controllerScreenX;
            e.screenY = e.controllerScreenY;
            e.markerAnchorValid = true;
        } else if (e.worldAnchorValid) {
            e.screenX = e.worldScreenX;
            e.screenY = e.worldScreenY;
            e.markerAnchorValid = true;
        }
    }

    return true;
}

// 给同一场景中的 source SCI 指针计算“相对 record 序号”。
// 当前仍没有完整 SCI buffer base 的正式全局地址，因此 UI 使用 S~N 而不是伪装成绝对 SCI#N。
// 但场景对象表中的 source records 通常连续，0x227 对齐关系可用于快速人工比对。
static void AssignRelativeSourceOrdinals(std::vector<RuntimeEntityView>& entities) {
    uintptr_t minSourceAddress = 0;
    for (const RuntimeEntityView& e : entities) {
        if (!e.source) continue;
        const uintptr_t address = reinterpret_cast<uintptr_t>(e.source);
        if (minSourceAddress == 0 || address < minSourceAddress) minSourceAddress = address;
    }
    if (minSourceAddress == 0) return;

    for (RuntimeEntityView& e : entities) {
        if (!e.source) continue;
        const uintptr_t address = reinterpret_cast<uintptr_t>(e.source);
        if (address < minSourceAddress) continue;
        const uintptr_t delta = address - minSourceAddress;
        if ((delta % SourceEntityOffset::kRecordSize) != 0) continue;
        const uintptr_t ordinal = delta / SourceEntityOffset::kRecordSize;
        if (ordinal <= 0x7FFF) e.sourceOrdinal = static_cast<int>(ordinal);
    }
}

static GameSnapshot CaptureSnapshot() {
    GameSnapshot out;

    uint8_t* world = nullptr;
    if (!SafeRead(Address::kWorldPtr, world) || !world) return out;
    if (!IsReadableRange(world, WorldOffset::kBlockSize)) return out;
    out.world = world;

    // Scene descriptor 的开头是 NUL 结尾路径。只读前 50 字节，不越过已确认结构。
    const char* scene = reinterpret_cast<const char*>(world + WorldOffset::kSceneDescriptor);
    size_t sceneLen = 0;
    while (sceneLen < WorldOffset::kSceneDescriptorSize && scene[sceneLen] != '\0') ++sceneLen;
    out.scenePath.assign(scene, scene + sceneLen);
    out.sceneId = NormalizeSceneId(out.scenePath);

    // dev6d 场景代际：这是修复 dev4/dev5 已经实机复现的“新 Scene + 旧 Manager”竞态的核心。
    // 原版切图时，World block 里的 Scene 字符串会比探索 Manager 的完整替换更早发生变化。
    // 如果这里继续盲信 g_explorationManager，就会出现“日志写着 mp0701，却枚举出 mp0102b 的桌子/出口”，
    // Quest Marker 也可能在一两帧内绑定到上一张地图的同号 Event。
    //
    // 第一次看到 Scene 时只建立 generation=1，不主动丢掉已经捕获的 Manager；
    // 后续每次 Scene ID 改变则立刻把全局 Manager 交换成 nullptr，并记住刚丢掉的旧地址。
    // 这样本帧一定不会继续使用上一场景对象表。
    const LONG captureSerialNow = InterlockedCompareExchange(&g_explorationCaptureSerial, 0, 0);
    if (g_snapshotSceneId.empty()) {
        g_snapshotSceneId = out.sceneId;
        g_snapshotSceneGeneration = 1;
        g_sceneChangeCaptureSerial = captureSerialNow;
        g_managerRejectedAtSceneChange = nullptr;
    } else if (!IEqualsAscii(g_snapshotSceneId, out.sceneId)) {
        PVOID oldManager = InterlockedExchangePointer(&g_explorationManager, nullptr);
        const std::string oldScene = g_snapshotSceneId;
        g_snapshotSceneId = out.sceneId;
        ++g_snapshotSceneGeneration;
        if (g_snapshotSceneGeneration == 0) g_snapshotSceneGeneration = 1; // 极端整数回绕保护。
        g_sceneChangeCaptureSerial = InterlockedCompareExchange(&g_explorationCaptureSerial, 0, 0);
        g_managerRejectedAtSceneChange = oldManager;
        g_lastEntityDumpKey.clear();
        Log("[场景代际] %s -> %s Generation=%u：立即作废旧Manager=%p，等待新场景0x409580捕获。",
            oldScene.c_str(), out.sceneId.c_str(), g_snapshotSceneGeneration, oldManager);
    }
    out.sceneGeneration = g_snapshotSceneGeneration;

    SafeRead(Address::kCameraX, out.cameraX);
    SafeRead(Address::kCameraY, out.cameraY);

    // EVE 0x7E/0x7F 的原版代码链： [0x008E1C48] -> DataCenter+0x90 -> 当前 Inventory Bank。
    // 这里只读指针，不调用增加/删除物品函数。
    uint8_t* dataCenter = nullptr;
    uint8_t* activeBank = nullptr;
    if (SafeRead(Address::kDataCenterPtr, dataCenter) && dataCenter &&
        SafeReadPtr(dataCenter, 0x90, activeBank) && activeBank && IsReadableRange(activeBank, 0x5DD8)) {
        out.activeInventoryBank = activeBank;
        SafeReadPtr(dataCenter, 0xD8, out.battleLayoutGroup);
        SafeReadPtr(dataCenter, 0x118, out.battleResult);
    }
    SafeRead(Address::kActiveEventId, out.activeEventId);

    // ---------------------------------------------------------------------
    // A. 玩家逻辑坐标：继续从 0x89F7F0 固定角色槽读取。
    // ---------------------------------------------------------------------
    // dev3a 的用户实机已经证明这个数组在不同河州场景始终是五个固定角色槽，不能再称为“场景 Entity 数组”。
    // 但受控角色索引与玩家 X/Y 仍然稳定可用，所以 dev4 只保留这一项用途。
    uint8_t* fixedActorBase = nullptr;
    uint32_t controlledIndex = 0;
    SafeRead(Address::kRuntimeEntityBasePtr, fixedActorBase);
    SafeRead(Address::kControlledEntityIndex, controlledIndex);
    out.controlledEntityIndex = controlledIndex;

    uint32_t fixedActorCount = 0;
    if (fixedActorBase && IsReadableRange(fixedActorBase - 4, sizeof(uint32_t))) {
        std::memcpy(&fixedActorCount, fixedActorBase - 4, sizeof(fixedActorCount));
    }
    if (!fixedActorBase || fixedActorCount > 512) fixedActorCount = 0;
    out.fixedActorCount = fixedActorCount;

    if (controlledIndex < fixedActorCount && fixedActorBase) {
        uint8_t* player = fixedActorBase + static_cast<size_t>(controlledIndex) * RuntimeEntityOffset::kRecordSize;
        SafeReadPtr(player, RuntimeEntityOffset::kWorldX, out.playerX);
        SafeReadPtr(player, RuntimeEntityOffset::kWorldY, out.playerY);
    }

    // ---------------------------------------------------------------------
    // B. 当前场景 NPC/物件：改从 0x409580 的探索管理器对象表读取。
    // ---------------------------------------------------------------------
    // 0x409580 每轮探索更新时把 this 保存在 g_explorationManager。
    // 原版反汇编明确：
    //   this+0x1C = 场景原生对象数量；
    //   this+0x2C = 指向 runtime Entity* 指针表；
    //   0x409600/0x409615 会在 count 与 count+1 位置额外追加受控角色/特殊 Actor。
    // 因此这里只读取前 count 项，正好绕开 dev3a 看到的那五个固定角色槽。
    void* manager = InterlockedCompareExchangePointer(&g_explorationManager, nullptr, nullptr);
    const LONG managerSerial = InterlockedCompareExchange(&g_explorationCaptureSerial, 0, 0);

    // 如果 Scene 已经变了，但 0x409580 还没有在变更后重新执行，managerSerial 不会前进，直接拒绝。
    // 另外，原版可能在 Scene 字符串改变后又短暂调用几次“旧 Manager”的 0x409580。
    // dev6d 会对“和刚丢掉的旧地址完全相同”的 Manager 再给 4 次捕获的保护窗口。
    // 4 次是有意的安全折中：足以覆盖实机日志中的短竞态，又不会在 allocator 恰好复用同一地址时长期丢 Marker。
    const bool capturedAfterSceneChange = managerSerial > g_sceneChangeCaptureSerial;
    const bool sameAsRejectedOldManager = manager && manager == g_managerRejectedAtSceneChange;
    const bool insideOldManagerGrace = sameAsRejectedOldManager &&
                                       managerSerial <= g_sceneChangeCaptureSerial + 4;
    if (manager && (!capturedAfterSceneChange || insideOldManagerGrace)) {
        out.managerRejectedAsStale = true;
        manager = nullptr;
    }

    if (manager && IsReadableRange(manager, ExplorationManagerOffset::kSceneObjectPointers + sizeof(void*))) {
        out.managerCaptureSerial = managerSerial;
        int32_t rawCount = -1;
        uint8_t** pointerTable = nullptr;
        if (SafeReadPtr(manager, ExplorationManagerOffset::kSceneObjectCount, rawCount) &&
            SafeReadPtr(manager, ExplorationManagerOffset::kSceneObjectPointers, pointerTable) &&
            rawCount >= 0 && rawCount <= 512 && pointerTable &&
            (rawCount == 0 || IsReadableRange(pointerTable, static_cast<size_t>(rawCount) * sizeof(void*)))) {
            out.sceneObjectListValid = true;
            out.explorationManager = static_cast<const uint8_t*>(manager);
            out.entities.reserve(static_cast<size_t>(rawCount));

            for (int32_t i = 0; i < rawCount; ++i) {
                uint8_t* record = nullptr;
                if (!SafeRead(reinterpret_cast<uintptr_t>(pointerTable + i), record) || !record) continue;
                RuntimeEntityView e;
                if (BuildRuntimeEntityView(record, static_cast<size_t>(i), out.cameraX, out.cameraY, e)) {
                    out.entities.push_back(e);
                }
            }
            AssignRelativeSourceOrdinals(out.entities);
            out.entityCount = static_cast<uint32_t>(out.entities.size());
        }
    }

    out.valid = !out.sceneId.empty();
    return out;
}

// ============================================================================
// 6. UTF-8 INI 读取器
// ============================================================================

// Windows 自带 GetPrivateProfileString 对现代 UTF-8 中文配置不够直观，所以这里写一个很小的只读 INI 解析器。
// 它只支持我们需要的： [Section] 与 key=value。没有“神秘兼容行为”，出错更容易诊断。
class IniFile {
public:
    bool Load(const std::wstring& path) {
        sections_.clear();
        HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) return false;
        LARGE_INTEGER size = {};
        if (!GetFileSizeEx(h, &size) || size.QuadPart < 0 || size.QuadPart > 16 * 1024 * 1024) {
            CloseHandle(h);
            return false;
        }
        std::string data(static_cast<size_t>(size.QuadPart), '\0');
        DWORD read = 0;
        BOOL ok = data.empty() || ReadFile(h, &data[0], static_cast<DWORD>(data.size()), &read, nullptr);
        CloseHandle(h);
        if (!ok) return false;
        data.resize(read);
        if (data.size() >= 3 && static_cast<unsigned char>(data[0]) == 0xEF &&
            static_cast<unsigned char>(data[1]) == 0xBB && static_cast<unsigned char>(data[2]) == 0xBF) {
            data.erase(0, 3);
        }

        std::string current;
        std::istringstream stream(data);
        std::string line;
        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            std::string t = Trim(line);
            if (t.empty() || t[0] == ';' || t[0] == '#') continue;
            if (t.size() >= 2 && t.front() == '[' && t.back() == ']') {
                current = Trim(t.substr(1, t.size() - 2));
                continue;
            }
            size_t eq = t.find('=');
            if (eq == std::string::npos || current.empty()) continue;
            std::string key = Trim(t.substr(0, eq));
            std::string value = Trim(t.substr(eq + 1));
            sections_[current][key] = value;
        }
        return true;
    }

    std::string Get(const std::string& section, const std::string& key, const std::string& def = "") const {
        auto s = FindSection(section);
        if (s == sections_.end()) return def;
        for (const auto& kv : s->second) {
            if (IEqualsAscii(kv.first, key)) return kv.second;
        }
        return def;
    }

    int GetInt(const std::string& section, const std::string& key, int def) const {
        std::string v = Get(section, key, "");
        if (v.empty()) return def;
        char* end = nullptr;
        long n = std::strtol(v.c_str(), &end, 0);
        return end && *end == '\0' ? static_cast<int>(n) : def;
    }

    bool GetBool(const std::string& section, const std::string& key, bool def) const {
        std::string v = ToLowerAscii(Get(section, key, ""));
        if (v.empty()) return def;
        if (v == "1" || v == "true" || v == "yes" || v == "on") return true;
        if (v == "0" || v == "false" || v == "no" || v == "off") return false;
        return def;
    }

    const std::map<std::string, std::map<std::string, std::string>>& Sections() const { return sections_; }

private:
    using SectionMap = std::map<std::string, std::map<std::string, std::string>>;
    SectionMap sections_;

    SectionMap::const_iterator FindSection(const std::string& section) const {
        for (auto it = sections_.begin(); it != sections_.end(); ++it) {
            if (IEqualsAscii(it->first, section)) return it;
        }
        return sections_.end();
    }
};

// dev6q：继续把 INI 中的“255,205,70”解析成三个 0~255 整数。
// 解析规则故意非常严格：必须正好有三个十进制整数，不能多一个也不能少一个。
// 如果用户手误写成“255,205”或“300,0,0”，函数返回 false，调用方会记录日志并继续使用安全默认色。
static bool ParseRgbTriplet(const std::string& text, int& r, int& g, int& b) {
    std::istringstream stream(text);
    std::string part;
    int values[3] = {0, 0, 0};
    for (int i = 0; i < 3; ++i) {
        if (!std::getline(stream, part, ',')) return false;
        part = Trim(part);
        if (part.empty()) return false;
        char* end = nullptr;
        long value = std::strtol(part.c_str(), &end, 10);
        if (!end || *end != '\0' || value < 0 || value > 255) return false;
        values[i] = static_cast<int>(value);
    }
    if (std::getline(stream, part, ',')) return false;
    r = values[0];
    g = values[1];
    b = values[2];
    return true;
}

static void LoadMarkerRgb(const IniFile& ini, const char* key,
                          int defaultR, int defaultG, int defaultB,
                          int& outR, int& outG, int& outB) {
    const std::string raw = ini.Get("MarkerColor", key, "");
    if (raw.empty()) {
        outR = defaultR; outG = defaultG; outB = defaultB;
        return;
    }
    int r = 0, g = 0, b = 0;
    if (!ParseRgbTriplet(raw, r, g, b)) {
        outR = defaultR; outG = defaultG; outB = defaultB;
        Log("[配置] MarkerColor.%s=%s 不是有效的 R,G,B（每项必须为0~255），已回退默认值 %d,%d,%d。",
            key, raw.c_str(), defaultR, defaultG, defaultB);
        return;
    }
    outR = r; outG = g; outB = b;
}

// ============================================================================
// 7. 条件表达式：只读 GameVar 与 Inventory
// ============================================================================

// dev4 的任务条件支持两种“左值”：
//   var[123]  —— 原版 World block 中的 int16 GameVar；
//   item[451] —— 当前激活 Inventory Bank 六类合计数量。
// 它们都只有读取能力，没有任何 SET/ADD/REMOVE 操作，所以 Quest Resolver 永远不能反向改变剧情。
// 语法支持：true/false、!、&&、||、括号，以及 == != < <= > >=。
class ConditionParser {
public:
    ConditionParser(const std::string& text, const GameSnapshot& snapshot, bool syntaxOnly = false)
        : text_(text), snapshot_(snapshot), syntaxOnly_(syntaxOnly) {}

    bool Evaluate(bool* syntaxOk = nullptr) {
        pos_ = 0;
        ok_ = true;
        SkipSpaces();
        if (text_.empty()) {
            if (syntaxOk) *syntaxOk = true;
            return true;
        }
        bool value = ParseOr();
        SkipSpaces();
        if (pos_ != text_.size()) ok_ = false;
        if (syntaxOk) *syntaxOk = ok_;
        return ok_ ? value : false;
    }

private:
    const std::string& text_;
    const GameSnapshot& snapshot_;
    size_t pos_ = 0;
    bool ok_ = true;
    bool syntaxOnly_ = false;

    void SkipSpaces() { while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) ++pos_; }

    bool Match(const char* token) {
        SkipSpaces();
        const size_t len = std::strlen(token);
        if (text_.compare(pos_, len, token) != 0) return false;
        pos_ += len;
        return true;
    }

    bool MatchWord(const char* word) {
        SkipSpaces();
        const size_t len = std::strlen(word);
        if (text_.compare(pos_, len, word) != 0) return false;
        if (pos_ + len < text_.size()) {
            const char c = text_[pos_ + len];
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') return false;
        }
        pos_ += len;
        return true;
    }

    bool ParseOr() {
        bool value = ParseAnd();
        while (ok_ && Match("||")) { const bool rhs = ParseAnd(); value = value || rhs; }
        return value;
    }

    bool ParseAnd() {
        bool value = ParseUnary();
        while (ok_ && Match("&&")) { const bool rhs = ParseUnary(); value = value && rhs; }
        return value;
    }

    bool ParseUnary() {
        SkipSpaces();
        if (Match("!")) return !ParseUnary();
        if (Match("(")) {
            const bool value = ParseOr();
            if (!Match(")")) ok_ = false;
            return value;
        }
        if (MatchWord("true")) return true;
        if (MatchWord("false")) return false;
        return ParseComparison();
    }

    bool ParseInteger(int& out) {
        SkipSpaces();
        if (pos_ >= text_.size()) return false;
        const size_t start = pos_;
        if (text_[pos_] == '+' || text_[pos_] == '-') ++pos_;
        if (pos_ >= text_.size() || !std::isdigit(static_cast<unsigned char>(text_[pos_]))) { pos_ = start; return false; }
        while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
        out = std::atoi(text_.substr(start, pos_ - start).c_str());
        return true;
    }

    // 读 var[N] 或 item[N]。这里返回“值”，而不是把类别信息泄漏给后面的比较器。
    bool ParseReadOnlyValue(int& out) {
        SkipSpaces();
        size_t start = pos_;
        bool isVar = false;
        bool isItem = false;
        if (text_.compare(pos_, 3, "var") == 0 || text_.compare(pos_, 3, "VAR") == 0 || text_.compare(pos_, 3, "Var") == 0) {
            isVar = true; pos_ += 3;
        } else if (text_.compare(pos_, 4, "item") == 0 || text_.compare(pos_, 4, "ITEM") == 0 || text_.compare(pos_, 4, "Item") == 0) {
            isItem = true; pos_ += 4;
        } else {
            return false;
        }
        if (!Match("[")) { pos_ = start; return false; }
        int index = 0;
        if (!ParseInteger(index) || !Match("]")) { pos_ = start; return false; }

        // TOML 在事务提交前必须先检查条件表达式语法，但这时游戏甚至可能还没进入可读的 World/Inventory 状态。
        // syntaxOnly 模式只验证“var/item 语法和明显范围”，不去碰原版内存；正式运行求值才读取真实值。
        if (syntaxOnly_) {
            if (isVar && (index < 0 || index >= static_cast<int>(WorldOffset::kGameVarCount))) { ok_ = false; return false; }
            if (isItem && (index < 0 || index > 65535)) { ok_ = false; return false; }
            out = 0;
            return true;
        }
        bool readOk = false;
        if (isVar) out = static_cast<int>(ReadGameVar(snapshot_, index, &readOk));
        else if (isItem) out = ReadItemCount(snapshot_, index, &readOk);
        if (!readOk) { ok_ = false; return false; }
        return true;
    }

    bool ParseComparison() {
        int lhs = 0;
        if (!ParseReadOnlyValue(lhs)) { ok_ = false; return false; }

        enum class Op { Eq, Ne, Lt, Le, Gt, Ge } op;
        if (Match("==")) op = Op::Eq;
        else if (Match("!=")) op = Op::Ne;
        else if (Match("<=")) op = Op::Le;
        else if (Match(">=")) op = Op::Ge;
        else if (Match("<")) op = Op::Lt;
        else if (Match(">")) op = Op::Gt;
        else { ok_ = false; return false; }

        int rhs = 0;
        if (!ParseInteger(rhs)) { ok_ = false; return false; }
        switch (op) {
            case Op::Eq: return lhs == rhs;
            case Op::Ne: return lhs != rhs;
            case Op::Lt: return lhs < rhs;
            case Op::Le: return lhs <= rhs;
            case Op::Gt: return lhs > rhs;
            case Op::Ge: return lhs >= rhs;
        }
        return false;
    }
};

static bool EvaluateCondition(const std::string& expression, const GameSnapshot& snapshot, bool* syntaxOk = nullptr) {
    ConditionParser parser(expression, snapshot);
    return parser.Evaluate(syntaxOk);
}

static bool ValidateConditionSyntax(const std::string& expression) {
    GameSnapshot dummy;
    bool syntaxOk = false;
    ConditionParser parser(expression, dummy, true);
    parser.Evaluate(&syntaxOk);
    return syntaxOk;
}

// ============================================================================
// 8. 任务数据库的数据结构
// ============================================================================

enum class QuestState {
    Unavailable,
    Available,   // 已经可以触发，但玩家还没有正式触发任务；NPC Offer Marker 就在这个阶段出现。
    Active,
    Completed,
    Failed,      // dev6h：正式冻结表里有独立 failed_when，不能再把“失败”混进“已错过”。
    Expired
};

enum QuestTag : uint32_t {
    TagNone = 0,
    TagMissable = 1u << 0,
    TagTimed = 1u << 1,
    TagEndingRelevant = 1u << 2,
    TagImportant = 1u << 3
};

enum class MarkerRole {
    Exit,
    Talk,
    Investigate,
    Region,
    Destination,
    Offer
};

// dev6l：Marker 的“业务语义范围”与 MarkerRole 分开保存。
// MarkerRole 仍描述交互动作（出口、对话、调查等）；MarkerScope 则描述玩家应该怎样理解这个点：
// target   = 精确人物/物件；area = 只提示一个探索范围；entrance = 为跨场景导航服务的入口/出口。
// 两层分开后，未来换图标或颜色时不需要从 Event/名字反推“这个点到底代表什么”。
enum class MarkerScope {
    Target,
    Area,
    Entrance
};

enum class TargetType {
    Invalid,
    World,
    EventAny,
    EventManual,
    EventTouch,
    EntityIndex,
    EntityName,
    EntityNameContainsAny
};

struct MarkerTarget {
    TargetType type = TargetType::Invalid;
    int32_t x = 0;
    int32_t y = 0;
    int value = 0;            // Event ID 或 Entity index
    std::wstring name;        // EntityName 单名目标
    std::vector<std::wstring> names; // namecontainsany:a|b|c 的别名列表
};

struct MarkerDefinition {
    std::string sceneId;      // 当前场景匹配才显示。空字符串表示任意场景，主要供调试。
    std::string when = "true";
    MarkerTarget target;
    MarkerRole role = MarkerRole::Destination;
    MarkerScope scope = MarkerScope::Target;
    std::wstring locationName;
    std::wstring markerName;
    std::wstring markerHint;
    std::string style = "auto";
};

struct RouteNativeBinding {
    // 原版当前场景真正存在的物理 Event 绑定。自动 Marker 只能依附这些运行时实体；
    // 这里保存的 worldX/worldY 只是诊断/取证坐标，不能在实体消失时偷偷回退成自动 Marker。
    int eventId = -1;
    TargetType triggerType = TargetType::EventAny;
    std::wstring targetObject;
    int32_t worldX = 0;
    int32_t worldY = 0;
};

struct RouteEdge {
    // Route ID 仍沿用研究库 Rxxxxxx，目的只是让日志能回到研究证据定位；
    // 同一个 R ID 可以出现在不同 Stage，因为现在“任务 TOML”才是运行时所有者。
    std::string id;
    std::string fromScene;
    std::string toScene;

    // 自动检测层：保留原研究 Event/触发类型/对象与坐标，方便人工比对。
    int eventId = -1;
    TargetType eventTargetType = TargetType::EventAny;
    std::wstring targetObject;

    // 人工修正层：存在时优先于自动检测层。
    bool manualEventIdSet = false;
    int manualEventId = -1;
    bool manualTriggerTypeSet = false;
    TargetType manualEventTargetType = TargetType::EventAny;
    bool manualTargetObjectSet = false;
    std::wstring manualTargetObject;

    // 人类维护信息：直接跟在 Stage 目标下面，修改任务路线时不需要跳到别的文件。
    std::wstring fromName;
    std::wstring toName;
    std::wstring markerName;
    std::wstring routeHint;
    MarkerScope markerScope = MarkerScope::Entrance;
    std::wstring usage;
    std::wstring verificationStatus;
    std::wstring correctionReason;
    std::wstring testNotes;

    bool worldValid = false;
    bool worldXSet = false;
    bool worldYSet = false;
    int32_t worldX = 0;
    int32_t worldY = 0;
    bool manualWorldValid = false;
    bool manualWorldXSet = false;
    bool manualWorldYSet = false;
    int32_t manualWorldX = 0;
    int32_t manualWorldY = 0;

    // dev6w 已闭合的“原版事件空间适配”继续保留；dev6y 只是改变数据归属，不降低 Marker 安全门控。
    std::vector<RouteNativeBinding> nativeBindings;
    bool nativeNavigable = false;

    std::string when = "true";
    bool advanceSafe = false;
    bool runtimeOnly = false;
};

struct QuestStage {
    std::string id;
    int order = 0;
    std::string when = "true";
    std::wstring objective;
    // 玩家可见的多行补充备注。研究/维护说明仍使用后面的 notes 字符串，二者绝不混用。
    std::vector<std::wstring> playerNotes;

    // dev6y：每条任务链是单独 TOML；Stage 后面紧跟 [stage.target] / [stage.marker] / [stage.navigation]。
    // 解析器仍把这些相邻子表装回同一个 QuestStage，再派生成内部 MarkerDefinition。这样人类修改 Objective 后，
    // 向下几行就能看到最终目标、Marker 文案/范围与导航模式，不需要跨 CSV 或跨文件寻找。
    std::string conditionStatus;
    std::wstring locationName;
    std::wstring markerName;
    std::wstring markerHint;
    MarkerScope markerScope = MarkerScope::Target;
    std::string role;
    bool showInJournal = true;
    bool optional = false;
    std::string routeMode;
    // dev6y：跨场景导航边属于当前 Stage 自己。
    // 修改任务 Objective 后继续向下看，就能直接看到该阶段每一跳出口及 Marker 绑定。
    std::vector<RouteEdge> routes;
    std::string targetKind;
    std::string targetScene;
    std::string runtimeEventText;
    std::string triggerType;
    std::wstring targetObject;
    std::string worldXText;
    std::string worldYText;
    // dev6l：人工修正只允许填写“世界坐标”，不允许保存最终屏幕像素。
    // 两个字段为空时继续使用自动 Event/World 证据；成对填写时只覆盖 Marker 锚点，不覆盖原研究字段。
    std::string manualWorldXText;
    std::string manualWorldYText;
    // 少数 Stage 在 dev6g 已经明确采用了“冻结 Event 元数据 != 实际 Marker 锚点”的兼容策略。
    // 例如某些战斗触发点要继续用已验证 world:x,y，另一些 Stage 则明确不画 Marker。
    // 这里为空表示完全自动派生；"none" 表示禁止画；其他值使用旧 target 语法显式覆盖。
    std::string runtimeMarkerOverride;
    std::string battleGroup;
    std::wstring itemDependency;
    std::string readsVar;
    std::string writesVar;
    std::wstring stageResult;
    std::string evidence;
    std::string notes;

    // dev6zc：下面这些字段只会出现在“内存中的 addon 插入阶段”，不会写回原始 Qxxx.toml。
    // addonInserted=true 表示这个 Stage 来自同名 _addon.toml；它只改变任务系统的展示/引导顺序，
    // 绝不会修改原版 GameVar、Event、Battle 或存档。after/before 是原始任务链中的两个相邻 Stage ID，
    // 插件只有在原始求值已经走到 beforeStageId 时，才暂时把这个人工步骤放到玩家面前。
    bool addonInserted = false;
    std::string addonId;
    std::string addonAfterStageId;
    std::string addonBeforeStageId;
    std::string addonCompletionScene;
    int addonCompletionEvent = -1;

    // 这仍然是运行时真正消费的 Marker 数组，由 [stage.target] 与 [stage.marker] 派生。
    std::vector<MarkerDefinition> routeMarkers;
};


struct QuestDefinition {
    // questNo 是 Castle_Quest 的稳定维护编号，例如 Q001。
    // 当前证据没有证明原版存在一张能直接对应这 25 条现代任务链的原生 Quest ID 表，因此绝不冒充原版编号。
    std::string questNo;
    std::string id;
    bool enabled = true;
    std::wstring title;
    std::string category = "Side";
    uint32_t tags = TagNone;
    std::string discoverMode;
    // discoverMode 是研究层分类，不足以精确推导 dev6g 实际是否画 Offer Marker。
    // 因此 dev6h 显式保存旧运行时已经采用的 Offer 场景和 target；两者同时为空表示不画 Offer。
    std::string runtimeOfferScene;
    std::string runtimeOfferTarget;
    std::string availableWhen = "false";
    // offerMarkerWhen 只决定“地图/NPC 上是否显示可接任务 Marker”。
    // 它不改变 Available 状态本身，因此可以安全地把主线阶段作为支线发现时机的交叉参照。
    std::string offerMarkerWhen = "false";
    std::wstring availableObjective;
    // journalWhen 控制“什么时候允许进入任务记录”。它只读原版状态，和 offer_when 分开，
    // 因此地图上可以先有可接 Marker，而任务本身仍不提前塞进 Journal/Tracker。
    std::string journalWhen = "false";
    bool availableInTracker = false;
    std::string activeWhen = "false";
    std::string completedWhen = "false";
    std::string failedWhen = "false";
    std::string expiredWhen = "false";
    bool tracked = false;
    bool primary = false;
    std::vector<MarkerDefinition> offerMarkers;
    // stages 永远保存详细 Qxxx.toml 中的原始任务链；addonInserts 单独保存人工追加步骤。
    // 这样调试时可以随时区分“游戏原始链”与“Remastered 体验层”，不会把两者揉成第二套剧情真值。
    std::vector<QuestStage> stages;
    std::vector<QuestStage> addonInserts;
};

struct EvaluatedQuest {
    const QuestDefinition* def = nullptr;
    QuestState state = QuestState::Unavailable;
    const QuestStage* stage = nullptr;
    // addonActive 只表示当前 HUD/Marker 正在显示一条人工插入步骤；QuestState 仍来自原版 GameState。
    bool addonActive = false;
};

static std::vector<QuestDefinition> g_quests;

// 人工插入阶段的“已完成”暂时只属于本次游戏进程的体验层状态。
// 它不会写入原版 TSF/GameVar，也不会反过来决定剧情。dev6zd 起 Ctrl+F8 成功热重载保留该集合；
// 未来跨存档持久化由主项目 SDK 的增强状态服务负责，本插件本版不自行创建 .state。
static std::set<std::string> g_completedAddonStageKeys;

// ============================================================================
// 8B. Stage 局部 Route：运行时不再存在全局 Route 数据库
// ============================================================================
//
// dev6y 按最终维护需求把跨场景导航收回到每个 Quest 的每个 Stage。
// 运行时不会读取 routes.toml，也不会维护一张“全任务共享 1116 边”的第二份运行数据库。
// 每个 [[stage.route]] 仍然只是解释原版 GameState 的只读导航规则：
//   1. 当前 Stage 的最终目标就在本 Scene -> 直接解析 [stage.target]；
//   2. 最终目标在别的 Scene -> 只在当前 Stage 自己的 routes 中做条件 BFS；
//   3. 切图后重新读取原版 GameVar，再在同一 Stage 的 routes 中重算第一跳。
//
// 完整 1116 条研究图仍保存在 docs/任务系统/研究/冻结任务数据库/正式条件化路由图.csv，
// 只作为证据和后续人工修订参考，不再被 ASI 直接读取。

// dev6j：这里不再保留旧的 ParseMarkerRole()。
// dev6y 起 Marker 的 role 直接写在每个任务 TOML 的 [stage.marker] 中；当前 C++ 只读取这一份运行数据。
// 如果把一个“完全没人调用”的 static 函数留在源码里，MSVC /W4 会给出 C4505。
// 删除它不会改变任何运行逻辑，只是让构建日志保持干净，避免真正的编译问题被无关警告淹没。

static bool ParseTarget(const std::string& raw, MarkerTarget& out) {
    std::string value = Trim(raw);
    std::string lower = ToLowerAscii(value);
    if (lower.rfind("world:", 0) == 0) {
        std::string rest = value.substr(6);
        size_t comma = rest.find(',');
        if (comma == std::string::npos) return false;
        out.type = TargetType::World;
        out.x = std::atoi(Trim(rest.substr(0, comma)).c_str());
        out.y = std::atoi(Trim(rest.substr(comma + 1)).c_str());
        return true;
    }
    if (lower.rfind("event:any:", 0) == 0) {
        out.type = TargetType::EventAny;
        out.value = std::atoi(value.substr(10).c_str());
        return out.value >= 0 && out.value <= 255;
    }
    if (lower.rfind("event:manual:", 0) == 0) {
        out.type = TargetType::EventManual;
        out.value = std::atoi(value.substr(13).c_str());
        return out.value >= 0 && out.value <= 255;
    }
    if (lower.rfind("event:touch:", 0) == 0) {
        out.type = TargetType::EventTouch;
        out.value = std::atoi(value.substr(12).c_str());
        return out.value >= 0 && out.value <= 255;
    }
    if (lower.rfind("entity:", 0) == 0) {
        out.type = TargetType::EntityIndex;
        out.value = std::atoi(value.substr(7).c_str());
        return out.value >= 0;
    }
    if (lower.rfind("namecontainsany:", 0) == 0) {
        out.type = TargetType::EntityNameContainsAny;
        std::string rest = value.substr(16);
        std::istringstream ss(rest);
        std::string token;
        while (std::getline(ss, token, '|')) {
            std::wstring w = Utf8ToWide(Trim(token));
            if (!w.empty()) out.names.push_back(w);
        }
        return !out.names.empty();
    }
    if (lower.rfind("name:", 0) == 0) {
        out.type = TargetType::EntityName;
        out.name = Utf8ToWide(value.substr(5));
        return !out.name.empty();
    }
    return false;
}

// ============================================================================
// 8C. dev6zd 数据布局：一条任务链固定由详细 Base + 同名 Addon 成对组成
// ============================================================================
//
// 运行目录固定为一层：
//   Castle_Quest/manifest.toml
//   Castle_Quest/Q001_*.toml ... Q025_*.toml
//   Castle_Quest/Q001_*_addon.toml ... Q025_*_addon.toml
//
// 不存在 routes.toml。每条任务链都有一份“详细 Base”和一份“同名 Addon”：
//   * Base 保存已经研究闭合的原版任务链、程序条件、原始目标和 Stage-local Route；正常调整尽量不改它。
//   * Addon 保存 Remastered 人工插入步骤与局部覆盖；只覆盖自己明确写出的字段，优先级高于 Base。
// Ctrl+F8 会把 25 对 Base+Addon 作为一个完整事务重新读取，任何一对出错都保留上一份有效数据库。
//
// Base 的每个 [[stage]] 后仍可紧跟 [stage.target]、[stage.marker]、[stage.navigation] 和 [[stage.route]]。
// Addon 不复制整条 Stage；它只用稳定 Quest ID / Stage ID 指定修改位置，因此原版任务链可以长期保持原貌。
// manifest 核对 Base/Addon 数量和 Base Stage 数量；局部 Route 随 Base Quest 一起严格校验。

// ============================================================================
// 8D. 详细 Quest TOML v7 严格读取器（dev6zd）
// ============================================================================
//
// 详细 Base Quest TOML 使用 format_version=7；Addon 使用独立的 format_version=2；manifest 使用 format_version=7。
// 每个 Base 文件只有一条任务，因此使用 [quest] 保存任务本体；
// 阶段使用 [[stage]]，并把目标、Marker 显示元数据、导航意图分别紧跟在 [stage.target]、
// [stage.marker]、[stage.navigation]。dev6zd 中 Base v7 只允许 canonical Stage；旧 Warning/Guidance 运行体系均已退役。
// 内部仍派生 MarkerDefinition；跨场景导航直接读取当前 Stage 的 [[stage.route]]。
//
// 为避免引入外部 DLL/庞大第三方头文件，仍使用项目自带的严格 TOML 子集解析：
//   * UTF-8 basic string（"..."）与常用转义；
//   * 十进制整数；
//   * true / false；
//   * 字符串数组；
//   * [[stage]] / [[stage.route]]；
//   * 字符串外 # 注释；
//   * 字段类型、重复 ID、Qxxx/Rxxxxxx 编号、跨文件数量与条件语法检查；
//   * 失败时输出文件、行号、字段和原因，并继续保留上一份完整有效数据库。
//
// dev6j：VS18 / MSVC 14.51 的已知实机编译现象是：语法已经通过以后，编译器会在 STL 的 xtree 内部
// 触发 fatal error C1001。错误位置落在编译器自己的 main.cpp/p2 优化阶段，而不是我们的 C++ 语法行。
// 这一整块代码只在启动/Ctrl+F8 热重载时解析 TOML 和做唯一性检查，不在每帧 Present/Marker 绘制热路径里。
// 因此这里对 MSVC 单独关闭优化：用最小运行时代价绕开编译器优化器缺陷，同时不牺牲每帧渲染代码的优化。
// 其他编译器会直接跳过这两个 pragma；数据库格式、条件判断和 GameState 解释完全不变。
#if defined(_MSC_VER)
#pragma optimize("", off)
#endif

// dev6i 编译修复：
// dev6h 在把旧“单文件 TOML Loader”改写成多文件 Loader 时，误删了下面三个通用基础定义，
// 但后面的 ParseTomlScalar、ReadUtf8TextFile、LoadQuestManifestFromToml 等函数仍然在使用它们。
// C++ 和 Python 不一样：一个类型在函数参数里出现以前，编译器必须已经见过它的完整声明。
// 因此 dev6h 会从 ParseTomlScalar(TomlScalar& ...) 这一行开始出现 C2061，随后因为函数签名没解析成功，
// 又连锁报出 out/why/error 未声明等一百多个假错误。下面恢复的是 dev6g 已使用过的同一组小型结构，
// 它们只负责“保存一个 TOML 标量”和“保存一条加载错误”，不改变任何 Quest/Route 运行逻辑。
struct TomlScalar {
    // Type 告诉后面的 schema 代码：当前 value 到底是字符串、整数、布尔值还是字符串数组。
    // None 只表示“还没有成功解析”，不会作为合法数据写入 Quest。
    enum class Type { None, String, Integer, Boolean, StringArray } type = Type::None;

    // 四种数据分别放在自己的成员里。只有 type 指向的那个成员才有意义。
    std::string stringValue;
    long long intValue = 0;
    bool boolValue = false;
    std::vector<std::string> stringArray;
};

struct TomlLoadError {
    // line=0 表示错误不属于某个具体文本行，例如“文件打不开”或“总数量不一致”。
    int line = 0;
    // field 保存出错字段名，日志可以直接告诉维护者应该去找哪个 key。
    std::string field;
    // message 保存适合人阅读的原因。这里只存文本，不弹窗口，也不修改游戏状态。
    std::string message;
};

static std::string StripTomlComment(const std::string& line) {
    // TOML 的 # 只有在字符串外才是注释。
    // 例如 objective = "找到 #1 房间" 中的 # 属于正文，绝不能从那里截断。
    bool inString = false;
    bool escaped = false;
    for (size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (inString) {
            // 上一个字符如果是反斜杠，当前字符就是转义内容；即使它是双引号，也不能结束字符串。
            if (escaped) { escaped = false; continue; }
            if (ch == '\\') { escaped = true; continue; }
            if (ch == '"') inString = false;
            continue;
        }
        if (ch == '"') { inString = true; continue; }
        if (ch == '#') return line.substr(0, i);
    }
    return line;
}

static bool ParseTomlBasicString(const std::string& raw, std::string& out, std::string& why) {
    const std::string value = Trim(raw);
    if (value.size() < 2 || value.front() != '"' || value.back() != '"') {
        why = "expected a quoted basic string";
        return false;
    }
    out.clear();
    const size_t closingQuote = value.size() - 1;
    for (size_t i = 1; i < closingQuote; ++i) {
        char ch = value[i];
        if (ch != '\\') { out.push_back(ch); continue; }
        // 反斜杠后必须还有一个“位于最终结束引号之前”的字符。
        // 如果反斜杠直接贴着行尾结束引号，那么那个引号其实已经被转义，整条字符串没有真正闭合。
        if (i + 1 >= closingQuote) { why = "unfinished string escape / missing closing quote"; return false; }
        const char e = value[++i];
        if (e == 'n') out.push_back('\n');
        else if (e == 'r') out.push_back('\r');
        else if (e == 't') out.push_back('\t');
        else if (e == '"') out.push_back('"');
        else if (e == '\\') out.push_back('\\');
        else { why = std::string("unsupported escape: \\") + e; return false; }
    }
    return true;
}

static bool ParseTomlStringArray(const std::string& raw, std::vector<std::string>& out, std::string& why) {
    const std::string value = Trim(raw);
    if (value.size() < 2 || value.front() != '[' || value.back() != ']') {
        why = "expected [\"...\", \"...\"]";
        return false;
    }
    out.clear();
    size_t pos = 1;
    while (pos + 1 < value.size()) {
        while (pos + 1 < value.size() && std::isspace(static_cast<unsigned char>(value[pos]))) ++pos;
        if (pos + 1 >= value.size()) break;
        if (value[pos] == ',') { ++pos; continue; }
        if (value[pos] != '"') { why = "string array elements must be quoted strings"; return false; }
        const size_t begin = pos;
        bool escaped = false;
        ++pos;
        for (; pos < value.size(); ++pos) {
            const char ch = value[pos];
            if (escaped) { escaped = false; continue; }
            if (ch == '\\') { escaped = true; continue; }
            if (ch == '"') break;
        }
        if (pos >= value.size()) { why = "unterminated string in array"; return false; }
        std::string decoded;
        if (!ParseTomlBasicString(value.substr(begin, pos - begin + 1), decoded, why)) return false;
        out.push_back(decoded);
        ++pos;
        while (pos + 1 < value.size() && std::isspace(static_cast<unsigned char>(value[pos]))) ++pos;
        if (pos + 1 < value.size() && value[pos] != ',') { why = "expected ',' between array elements"; return false; }
    }
    return true;
}

static bool ParseTomlScalar(const std::string& raw, TomlScalar& out, std::string& why) {
    const std::string value = Trim(raw);
    if (value.empty()) { why = "value is empty"; return false; }
    if (value.front() == '"') {
        out.type = TomlScalar::Type::String;
        return ParseTomlBasicString(value, out.stringValue, why);
    }
    if (value.front() == '[') {
        out.type = TomlScalar::Type::StringArray;
        return ParseTomlStringArray(value, out.stringArray, why);
    }
    const std::string lower = ToLowerAscii(value);
    if (lower == "true" || lower == "false") {
        out.type = TomlScalar::Type::Boolean;
        out.boolValue = (lower == "true");
        return true;
    }
    char* end = nullptr;
    const long long number = _strtoi64(value.c_str(), &end, 10);
    if (end && *end == '\0' && end != value.c_str()) {
        out.type = TomlScalar::Type::Integer;
        out.intValue = number;
        return true;
    }
    why = "unsupported TOML value type";
    return false;
}

// dev6zc：player_notes 经常需要多条说明。标准 TOML 允许字符串数组跨多行书写。
// 旧读取器逐“物理行”解析，读到 player_notes = [ 就会把单独的 '[' 当成完整值而报错。
// 这里仅把一个跨行字符串数组拼回单个“逻辑值”，然后仍交给原有严格字符串数组解析器。
// 因此表结构、字段白名单、字符串转义规则都没有被放宽。
static bool IsTomlArrayValueComplete(const std::string& raw) {
    const std::string value = Trim(raw);
    if (value.empty() || value.front() != '[') return true;

    int depth = 0;
    bool inString = false;
    bool escaped = false;
    for (char ch : value) {
        if (inString) {
            if (escaped) {
                escaped = false;
                continue;
            }
            if (ch == '\\') {
                escaped = true;
                continue;
            }
            if (ch == '"') inString = false;
            continue;
        }
        if (ch == '"') {
            inString = true;
            continue;
        }
        if (ch == '[') ++depth;
        else if (ch == ']') {
            --depth;
            if (depth <= 0) return true;
        }
    }
    return false;
}

static bool ReadTomlValueAllowMultilineStringArray(std::istringstream& input,
                                                     int& lineNo,
                                                     const std::string& firstValue,
                                                     const std::string& key,
                                                     std::string& fullValue,
                                                     TomlLoadError& error) {
    fullValue = Trim(firstValue);
    if (fullValue.empty() || fullValue.front() != '[' || IsTomlArrayValueComplete(fullValue)) return true;

    std::string rawLine;
    while (std::getline(input, rawLine)) {
        ++lineNo;
        if (!rawLine.empty() && rawLine.back() == '\r') rawLine.pop_back();
        const std::string continuation = Trim(StripTomlComment(rawLine));
        if (continuation.empty()) continue;

        // 物理换行只是 TOML 排版，不属于玩家可见文本；所以用一个空格连接即可。
        fullValue.push_back(' ');
        fullValue += continuation;
        if (IsTomlArrayValueComplete(fullValue)) return true;
    }

    error = {lineNo, key, "unterminated multiline string array"};
    return false;
}

static uint32_t ParseTagArray(const std::vector<std::string>& values, bool& ok, std::string& why) {
    // TOML 里的 tags 既包含“运行时真的会改变 HUD/Marker 样式”的标签，也包含研究数据库为了
    // 人工检索而保存的语义标签。例如 primary、mandatory、craft、puzzle、story_window 都很有用，
    // 但当前 ASI 并不需要给它们分别分配一个 bit。旧版在遇到这些标签时会把整份任务文件判错，
    // 这会导致“数据明明完整，只因为多写了一个研究标签就整库回退”。这里把两类用途明确分开：
    //
    // 1. missable / timed / ending_relevant* / ending_related / important：映射到真正的运行时 bit，供颜色和图标使用；
    // 2. 其他非空标签：允许存在并保留在 TOML 中，但当前运行时只把它们当元数据，不参与逻辑；
    // 3. 空字符串仍视为数据错误，因为它通常意味着人工编辑 TOML 时留下了空数组元素或格式错误。
    //
    // 这样做并不是“忽略未知错误”，而是让 tags 具备向前兼容能力。真正影响任务状态的字段仍然
    // 由 when/condition 等严格解析器检查，绝不会因为一个未知标签而偷偷改变 GameState 解释结果。
    uint32_t tags = TagNone;
    ok = true;
    for (const std::string& raw : values) {
        const std::string token = ToLowerAscii(Trim(raw));
        if (token.empty()) {
            ok = false;
            why = "quest tag cannot be empty";
            return TagNone;
        }

        if (token == "missable") {
            tags |= TagMissable;
        } else if (token == "timed") {
            tags |= TagTimed;
        } else if (token == "ending_relevant" || token == "endingrelevant" || token == "ending" ||
                   token == "ending_related" || token.rfind("ending_relevant_", 0) == 0) {
            // 冻结任务表里除了 ending_relevant，还存在 ending_relevant_material 和 ending_related。
            // 它们在剧情研究层含义略有区别，但用户已确认 Marker 视觉只需要判断“是否与结局相关”，
            // 所以这里统一映射到 TagEndingRelevant，不修改原始 tags 文本，也不改变任何任务状态条件。
            tags |= TagEndingRelevant;
        } else if (token == "important") {
            tags |= TagImportant;
        } else {
            // 例如 primary / mandatory / craft / puzzle / hidden / long_chain 等都走这里。
            // 它们仍原样保存在直接维护的 Quest TOML 中；ASI 当前没有视觉 bit 需求，所以不做任何位运算。
        }
    }
    return tags;
}

static bool ReadUtf8TextFile(const std::wstring& path, std::string& bytes, TomlLoadError& error) {
    // Win32 游戏目录经常包含中文。这里直接使用 _wfopen_s 的宽字符路径打开文件，
    // 不把路径先转成系统 ANSI 代码页，因此“幽城幻剑录”这类目录名不会在繁中/简中系统上被破坏。
    FILE* fp = nullptr;
    if (_wfopen_s(&fp, path.c_str(), L"rb") != 0 || !fp) {
        error = {0, "", "file cannot be opened"};
        return false;
    }

    // 先读进 std::string，再交给下面的逐行 TOML 子集解析器。
    // 这里不假设文件大小，也不把整个文件塞进固定数组，避免任一任务 TOML 变大以后越界。
    bytes.clear();
    char chunk[4096];
    for (;;) {
        const size_t got = std::fread(chunk, 1, sizeof(chunk), fp);
        if (got != 0) bytes.append(chunk, got);
        if (got < sizeof(chunk)) {
            if (std::ferror(fp)) {
                std::fclose(fp);
                error = {0, "", "file read failed"};
                return false;
            }
            break;
        }
    }
    std::fclose(fp);

    // Python 转换器默认写 UTF-8 无 BOM，但我们仍兼容手工编辑器加上的 UTF-8 BOM。
    if (bytes.size() >= 3 &&
        static_cast<unsigned char>(bytes[0]) == 0xEF &&
        static_cast<unsigned char>(bytes[1]) == 0xBB &&
        static_cast<unsigned char>(bytes[2]) == 0xBF) {
        bytes.erase(0, 3);
    }
    return true;
}

static bool TryParseStrictIntText(const std::string& text, int& value) {
    // Stage 的 runtime_event / world_x / world_y 在现行 Base Quest TOML 中故意保存成字符串，
    // 因为历史研究资料中的某些“研究坐标”可能是“306 / 362 / 400”这种说明文本，运行时不能把它偷转成整数。
    // 运行时只有遇到真正纯整数时才把它当坐标；有说明文字就只保留元数据，不胡乱 atoi 成 306。
    const std::string trimmed = Trim(text);
    if (trimmed.empty()) return false;
    char* end = nullptr;
    errno = 0;
    const long parsed = std::strtol(trimmed.c_str(), &end, 10);
    if (errno != 0 || !end || *end != '\0' || parsed < INT_MIN || parsed > INT_MAX) return false;
    value = static_cast<int>(parsed);
    return true;
}

static bool IsStableQuestNumber(const std::string& value) {
    // 当前维护编号固定 Q + 三位数字。这个检查只保证 Castle_Quest 自己的编号稳定，
    // 绝不把 Qxxx 宣称成原版任务 ID；原版是否存在独立任务编号表仍属于未证实项。
    if (value.size() != 4 || value[0] != 'Q') return false;
    return std::isdigit(static_cast<unsigned char>(value[1])) != 0 &&
           std::isdigit(static_cast<unsigned char>(value[2])) != 0 &&
           std::isdigit(static_cast<unsigned char>(value[3])) != 0;
}

static bool IsStableRouteId(const std::string& value) {
    // Route 编号固定 R + 六位数字。日志打印 R000842 后，可直接在当前任务 Stage 的 [[stage.route]] 搜索定位。
    if (value.size() != 7 || value[0] != 'R') return false;
    for (size_t i = 1; i < value.size(); ++i) {
        if (std::isdigit(static_cast<unsigned char>(value[i])) == 0) return false;
    }
    return true;
}

struct QuestManifest {
    int formatVersion = -1;
    std::string databaseVersion;
    int questCount = -1;
    int stageCount = -1;
    int canonicalStageCount = -1;
    int addonCount = -1;
};

static bool LoadQuestManifestFromToml(const std::wstring& path, QuestManifest& manifest, TomlLoadError& error) {
    std::string bytes;
    if (!ReadUtf8TextFile(path, bytes, error)) return false;

    // manifest.toml 故意只允许根级 key=value，不允许表。它的职责只是告诉 ASI：
    // “同目录应该有多少个 Q*.toml，以及最终应该看到多少条 Base Stage”。
    // Route 已包含在 Quest 文件里，所以 manifest 不再指向任何独立路由文件。
    std::set<std::string> seenKeys;
    std::istringstream input(bytes);
    std::string rawLine;
    int lineNo = 0;
    while (std::getline(input, rawLine)) {
        ++lineNo;
        if (!rawLine.empty() && rawLine.back() == '\r') rawLine.pop_back();
        const std::string line = Trim(StripTomlComment(rawLine));
        if (line.empty()) continue;
        if (line.front() == '[') {
            error = {lineNo, "", "manifest does not use TOML tables"};
            return false;
        }

        bool inString = false;
        bool escaped = false;
        size_t equalPos = std::string::npos;
        for (size_t i = 0; i < line.size(); ++i) {
            const char ch = line[i];
            if (inString) {
                if (escaped) escaped = false;
                else if (ch == '\\') escaped = true;
                else if (ch == '"') inString = false;
            } else {
                if (ch == '"') inString = true;
                else if (ch == '=') { equalPos = i; break; }
            }
        }
        if (equalPos == std::string::npos) {
            error = {lineNo, "", "expected key = value"};
            return false;
        }
        const std::string key = ToLowerAscii(Trim(line.substr(0, equalPos)));
        if (!seenKeys.insert(key).second) {
            error = {lineNo, key, "duplicate manifest key"};
            return false;
        }

        TomlScalar value;
        std::string why;
        if (!ParseTomlScalar(Trim(line.substr(equalPos + 1)), value, why)) {
            error = {lineNo, key, why};
            return false;
        }
        auto needString = [&](std::string& dst) -> bool {
            if (value.type != TomlScalar::Type::String) { error = {lineNo, key, "expected string"}; return false; }
            dst = value.stringValue;
            return true;
        };
        auto needInt = [&](int& dst) -> bool {
            if (value.type != TomlScalar::Type::Integer || value.intValue < INT_MIN || value.intValue > INT_MAX) {
                error = {lineNo, key, "expected 32-bit integer"};
                return false;
            }
            dst = static_cast<int>(value.intValue);
            return true;
        };

        if (key == "format_version") { if (!needInt(manifest.formatVersion)) return false; }
        else if (key == "database_version") { if (!needString(manifest.databaseVersion)) return false; }
        else if (key == "quest_count") { if (!needInt(manifest.questCount)) return false; }
        else if (key == "stage_count") { if (!needInt(manifest.stageCount)) return false; }
        else if (key == "canonical_stage_count") { if (!needInt(manifest.canonicalStageCount)) return false; }
        else if (key == "addon_count") { if (!needInt(manifest.addonCount)) return false; }
        else {
            error = {lineNo, key, "unknown manifest field"};
            return false;
        }
    }

    if (manifest.formatVersion != 7) { error = {0, "format_version", "expected format_version = 7"}; return false; }
    // database_version 是接档和发行追踪使用的稳定文字，不能允许空字符串。
    // 否则日志只能知道“有多少任务”，却无法判断玩家实际加载了哪一版数据库。
    if (Trim(manifest.databaseVersion).empty()) {
        error = {0, "database_version", "database_version must not be empty"};
        return false;
    }
    if (manifest.questCount < 1 || manifest.stageCount < 1 || manifest.canonicalStageCount < 0 ||
        manifest.canonicalStageCount != manifest.stageCount || manifest.addonCount < 0) {
        error = {0, "counts", "manifest counts are missing or invalid"};
        return false;
    }
    return true;
}

static bool ParseMarkerScopeText(const std::string& raw, MarkerScope& out) {
    const std::string value = ToLowerAscii(Trim(raw));
    if (value == "target") { out = MarkerScope::Target; return true; }
    if (value == "area") { out = MarkerScope::Area; return true; }
    if (value == "entrance") { out = MarkerScope::Entrance; return true; }
    return false;
}

static const char* MarkerScopeName(MarkerScope scope) {
    switch (scope) {
        case MarkerScope::Area: return "Area";
        case MarkerScope::Entrance: return "Entrance";
        case MarkerScope::Target:
        default: return "Target";
    }
}

static bool ParseRouteTriggerTypeText(const std::string& raw, TargetType& out) {
    // Route 的触发类型只允许三种机器语义：任意 Event、主动 EB、接触 EA。
    // [[stage.route]] 可以写 MANUAL_EB / TOUCH_EA，读取时统一折叠到现有 TargetType。
    const std::string trigger = ToLowerAscii(Trim(raw));
    if (trigger.empty() || trigger == "any") { out = TargetType::EventAny; return true; }
    if (trigger == "manual_eb" || trigger == "manual") { out = TargetType::EventManual; return true; }
    if (trigger == "touch_ea" || trigger == "touch") { out = TargetType::EventTouch; return true; }
    return false;
}

static bool ParseRouteNativeBindings(const std::string& raw,
                                     std::vector<RouteNativeBinding>& out) {
    out.clear();
    const std::string text = Trim(raw);
    if (text.empty()) return true;

    std::istringstream list(text);
    std::string item;
    while (std::getline(list, item, ';')) {
        item = Trim(item);
        if (item.empty()) continue;

        std::vector<std::string> parts;
        std::istringstream fields(item);
        std::string field;
        while (std::getline(fields, field, '|')) parts.push_back(Trim(field));
        if (parts.size() != 5) return false;

        RouteNativeBinding binding;
        if (!TryParseStrictIntText(parts[0], binding.eventId) || binding.eventId < 0 || binding.eventId > 255) return false;
        if (!ParseRouteTriggerTypeText(parts[1], binding.triggerType)) return false;
        if (binding.triggerType != TargetType::EventTouch && binding.triggerType != TargetType::EventManual) return false;
        binding.targetObject = Utf8ToWide(parts[2]);
        int x = 0;
        int y = 0;
        if (!TryParseStrictIntText(parts[3], x) || !TryParseStrictIntText(parts[4], y)) return false;
        binding.worldX = x;
        binding.worldY = y;
        out.push_back(std::move(binding));
    }
    return true;
}

static bool RouteHasUsableNavigationBinding(const RouteEdge& edge) {
    // 人工维护仍是最高优先级：哪怕原版导出没有物理绑定，只要维护者明确写了人工世界坐标或人工 Event，
    // 这条 Route 就仍然允许进入 BFS。反之，完全没有原版物理绑定的自动 Route 不再靠旧 Event 猜测导航。
    return edge.manualWorldValid || edge.manualEventIdSet || edge.nativeNavigable;
}

static int EffectiveRouteEventId(const RouteEdge& edge) {
    return edge.manualEventIdSet ? edge.manualEventId : edge.eventId;
}

static TargetType EffectiveRouteTriggerType(const RouteEdge& edge) {
    return edge.manualTriggerTypeSet ? edge.manualEventTargetType : edge.eventTargetType;
}

static const std::wstring& EffectiveRouteTargetObject(const RouteEdge& edge) {
    return edge.manualTargetObjectSet ? edge.manualTargetObject : edge.targetObject;
}


static void ApplyStageMarkerMetadata(const QuestStage& stage, MarkerDefinition& marker) {
    marker.scope = stage.markerScope;
    marker.locationName = stage.locationName;
    marker.markerName = stage.markerName;
    marker.markerHint = stage.markerHint;
}

static MarkerRole ParseStageMarkerRole(const QuestStage& stage) {
    // [stage.marker].role 是给人看的任务语义，不等于 MarkerRole 枚举名，所以这里做一次很小的映射。
    // 没有明确对应关系时宁可落到 Destination，也不凭空制造新的 Marker 类型。
    const std::string role = ToLowerAscii(Trim(stage.role));
    if (role == "talk_or_interact" || role == "optional_clue") return MarkerRole::Talk;
    if (role == "trigger") return MarkerRole::Region;
    if (role == "investigate") return MarkerRole::Investigate;
    return MarkerRole::Destination;
}

static bool BuildMarkerFromStageTarget(const QuestStage& stage, MarkerDefinition& marker, std::string& why) {
    // 没有目标场景的炼化/道具使用 Stage 本来就不应该在地图上画 Marker。
    if (Trim(stage.targetScene).empty()) return false;

    MarkerTarget target;

    // dev6l：显式人工世界坐标的优先级最高，但它只改变“Marker 画在哪里”。
    // runtime_event、自动 world_x/world_y 等证据字段仍完整保留在数据中，不会被人工值覆盖掉。
    int manualWorldX = 0;
    int manualWorldY = 0;
    const bool manualXOk = TryParseStrictIntText(stage.manualWorldXText, manualWorldX);
    const bool manualYOk = TryParseStrictIntText(stage.manualWorldYText, manualWorldY);
    if (manualXOk != manualYOk) {
        why = "manual_world_x/manual_world_y must both be strict integers";
        return false;
    }
    if (manualXOk && manualYOk) {
        target.type = TargetType::World;
        target.x = manualWorldX;
        target.y = manualWorldY;
        marker = MarkerDefinition();
        marker.sceneId = ToLowerAscii(Trim(stage.targetScene));
        marker.when = "true";
        marker.target = target;
        marker.role = ParseStageMarkerRole(stage);
        marker.style = "auto";
        ApplyStageMarkerMetadata(stage, marker);
        return true;
    }

    // dev6h 不能因为“把完整冻结 Stage 全部重新生成 TOML”就破坏 dev6g 已经采用的运行时锚点。
    // 因此只允许极少数 Stage 写 marker_override：
    //   none          -> 明确不生成 Marker；
    //   world:x,y     -> 继续使用旧版已经冻结的世界点；
    //   event:* / ... -> 继续复用项目既有 ParseTarget 的严格 target 语法。
    // 大多数 Stage 没有这个字段，继续走下面 runtime_event + trigger_type 的自动派生路径。
    const std::string markerOverride = Trim(stage.runtimeMarkerOverride);
    if (!markerOverride.empty()) {
        if (IEqualsAscii(markerOverride, "none")) return false;
        if (!ParseTarget(markerOverride, target)) {
            why = "invalid marker_override target expression";
            return false;
        }

        marker = MarkerDefinition();
        marker.sceneId = ToLowerAscii(Trim(stage.targetScene));
        marker.when = "true";
        marker.target = target;
        marker.role = ParseStageMarkerRole(stage);
        marker.style = "auto";
        ApplyStageMarkerMetadata(stage, marker);
        return true;
    }

    int eventId = -1;
    if (TryParseStrictIntText(stage.runtimeEventText, eventId)) {
        if (eventId < 0 || eventId > 255) {
            why = "runtime_event must be 0..255";
            return false;
        }

        // trigger_type 是最精确的程序证据；如果它为空，再用 target_kind 做保守回退。
        const std::string trigger = ToLowerAscii(Trim(stage.triggerType));
        const std::string kind = ToLowerAscii(Trim(stage.targetKind));
        if (trigger == "manual_eb" || trigger == "manual" || kind == "event_manual") {
            target.type = TargetType::EventManual;
        } else if (trigger == "touch_ea" || trigger == "touch" || kind == "event_touch") {
            target.type = TargetType::EventTouch;
        } else {
            target.type = TargetType::EventAny;
        }
        target.value = eventId;
    } else {
        // 只有“没有 RuntimeEvent、同时 X/Y 都是严格整数”时才退回显式世界坐标。
        // 例如维护表里的“306 / 362 / 400”只是研究说明，不会被 atoi 偷偷变成 306。
        int worldX = 0;
        int worldY = 0;
        const bool xOk = TryParseStrictIntText(stage.worldXText, worldX);
        const bool yOk = TryParseStrictIntText(stage.worldYText, worldY);
        if (!xOk && !yOk) return false;
        if (xOk != yOk) {
            why = "stage world_x/world_y must both be strict integers";
            return false;
        }
        target.type = TargetType::World;
        target.x = worldX;
        target.y = worldY;
    }

    marker = MarkerDefinition();
    marker.sceneId = ToLowerAscii(Trim(stage.targetScene));
    marker.when = "true"; // Stage.when 已经在选中当前 Stage 时求过一次，Marker 本身无需重复同一条件。
    marker.target = target;
    marker.role = ParseStageMarkerRole(stage);
    marker.style = "auto";
    ApplyStageMarkerMetadata(stage, marker);
    return true;
}

static bool LoadQuestFileV7(const std::wstring& path, QuestDefinition& quest, TomlLoadError& error) {
    std::string bytes;
    if (!ReadUtf8TextFile(path, bytes, error)) return false;

    quest = QuestDefinition();
    QuestStage* currentStage = nullptr;
    RouteEdge* currentRoute = nullptr;
    enum class Context { Root, Quest, Stage, StageTarget, StageMarker, StageNavigation, StageRoute } context = Context::Root;
    int formatVersion = -1;
    std::set<std::string> keysInCurrentTable;

    std::istringstream input(bytes);
    std::string rawLine;
    int lineNo = 0;
    while (std::getline(input, rawLine)) {
        ++lineNo;
        if (!rawLine.empty() && rawLine.back() == '\r') rawLine.pop_back();
        const std::string line = Trim(StripTomlComment(rawLine));
        if (line.empty()) continue;

        if (line.size() >= 4 && line.substr(0, 2) == "[[" && line.substr(line.size() - 2) == "]]" ) {
            const std::string header = ToLowerAscii(Trim(line.substr(2, line.size() - 4)));
            keysInCurrentTable.clear();
            if (header == "stage") {
                quest.stages.emplace_back();
                currentStage = &quest.stages.back();
                currentRoute = nullptr;
                context = Context::Stage;
            } else if (header == "stage.route") {
                if (!currentStage) { error = {lineNo, header, "[[stage.route]] must follow a [[stage]]"}; return false; }
                currentStage->routes.emplace_back();
                currentRoute = &currentStage->routes.back();
                context = Context::StageRoute;
            } else {
                error = {lineNo, header, "quest v7 only supports [[stage]] and [[stage.route]] arrays"};
                return false;
            }
            continue;
        }
        if (line.size() >= 3 && line.front() == '[' && line.back() == ']') {
            const std::string header = ToLowerAscii(Trim(line.substr(1, line.size() - 2)));
            keysInCurrentTable.clear();
            if (header == "quest") {
                currentStage = nullptr;
                currentRoute = nullptr;
                context = Context::Quest;
            } else if (header == "stage.target") {
                if (!currentStage) { error = {lineNo, header, "[stage.target] must follow a [[stage]]"}; return false; }
                context = Context::StageTarget;
            } else if (header == "stage.marker") {
                if (!currentStage) { error = {lineNo, header, "[stage.marker] must follow a [[stage]]"}; return false; }
                context = Context::StageMarker;
            } else if (header == "stage.navigation") {
                if (!currentStage) { error = {lineNo, header, "[stage.navigation] must follow a [[stage]]"}; return false; }
                context = Context::StageNavigation;
            } else {
                error = {lineNo, header, "unknown quest v7 table"};
                return false;
            }
            continue;
        }

        bool inString = false;
        bool escaped = false;
        size_t equalPos = std::string::npos;
        for (size_t i = 0; i < line.size(); ++i) {
            const char ch = line[i];
            if (inString) {
                if (escaped) escaped = false;
                else if (ch == '\\') escaped = true;
                else if (ch == '"') inString = false;
            } else {
                if (ch == '"') inString = true;
                else if (ch == '=') { equalPos = i; break; }
            }
        }
        if (equalPos == std::string::npos) { error = {lineNo, "", "expected key = value"}; return false; }
        const std::string key = ToLowerAscii(Trim(line.substr(0, equalPos)));
        if (key.empty()) { error = {lineNo, "", "key must not be empty"}; return false; }
        if (!keysInCurrentTable.insert(key).second) { error = {lineNo, key, "duplicate key in same table"}; return false; }

        std::string logicalValue;
        if (!ReadTomlValueAllowMultilineStringArray(input, lineNo, line.substr(equalPos + 1), key, logicalValue, error)) return false;

        TomlScalar value;
        std::string why;
        if (!ParseTomlScalar(logicalValue, value, why)) { error = {lineNo, key, why}; return false; }
        auto needString = [&](std::string& dst) -> bool {
            if (value.type != TomlScalar::Type::String) { error = {lineNo, key, "expected string"}; return false; }
            dst = value.stringValue;
            return true;
        };
        auto needWString = [&](std::wstring& dst) -> bool {
            if (value.type != TomlScalar::Type::String) { error = {lineNo, key, "expected string"}; return false; }
            dst = Utf8ToWide(value.stringValue);
            return true;
        };
        auto needBool = [&](bool& dst) -> bool {
            if (value.type != TomlScalar::Type::Boolean) { error = {lineNo, key, "expected boolean"}; return false; }
            dst = value.boolValue;
            return true;
        };
        auto needInt = [&](int& dst) -> bool {
            if (value.type != TomlScalar::Type::Integer || value.intValue < INT_MIN || value.intValue > INT_MAX) {
                error = {lineNo, key, "expected 32-bit integer"};
                return false;
            }
            dst = static_cast<int>(value.intValue);
            return true;
        };

        if (context == Context::Root) {
            // format_version 放在文件根级；任务本体必须进入 [quest]，避免与 Stage 子表混在同一命名空间。
            if (key == "format_version") { if (!needInt(formatVersion)) return false; }
            else { error = {lineNo, key, "base quest root only allows format_version; put quest fields under [quest]"}; return false; }
            continue;
        }

        if (context == Context::Quest) {
            std::string ignored;
            if (key == "quest_no") { if (!needString(quest.questNo)) return false; }
            else if (key == "quest_id") { if (!needString(quest.id)) return false; }
            else if (key == "enabled") { if (!needBool(quest.enabled)) return false; }
            else if (key == "title") { if (!needWString(quest.title)) return false; }
            else if (key == "category") { if (!needString(quest.category)) return false; }
            else if (key == "primary") { if (!needBool(quest.primary)) return false; }
            else if (key == "tracked_default") { if (!needBool(quest.tracked)) return false; }
            else if (key == "tags") {
                if (value.type != TomlScalar::Type::StringArray) { error = {lineNo, key, "expected string array"}; return false; }
                bool ok = false;
                quest.tags = ParseTagArray(value.stringArray, ok, why);
                if (!ok) { error = {lineNo, key, why}; return false; }
            }
            else if (key == "discover_mode") { if (!needString(quest.discoverMode)) return false; }
            else if (key == "offer_scene") { if (!needString(quest.runtimeOfferScene)) return false; }
            else if (key == "offer_target") { if (!needString(quest.runtimeOfferTarget)) return false; }
            else if (key == "offer_when") { if (!needString(quest.availableWhen)) return false; }
            else if (key == "offer_marker_when") { if (!needString(quest.offerMarkerWhen)) return false; }
            else if (key == "journal_when") { if (!needString(quest.journalWhen)) return false; }
            else if (key == "available_in_tracker") { if (!needBool(quest.availableInTracker)) return false; }
            else if (key == "active_when") { if (!needString(quest.activeWhen)) return false; }
            else if (key == "completed_when") { if (!needString(quest.completedWhen)) return false; }
            else if (key == "failed_when") { if (!needString(quest.failedWhen)) return false; }
            else if (key == "expired_when") { if (!needString(quest.expiredWhen)) return false; }
            else if (key == "available_objective") { if (!needWString(quest.availableObjective)) return false; }
            // 下面这些是为了让单份任务 TOML 同时保留研究/审计元数据。当前 ASI 不需要执行它们，
            // 但仍要求它们是字符串，避免文件被手工写成完全不同的类型后悄悄通过。
            else if (key == "missable" || key == "ending_relevant" || key == "authoritative_state" ||
                     key == "formalization_status" || key == "evidence" || key == "notes") {
                if (!needString(ignored)) return false;
            } else {
                error = {lineNo, key, "unknown quest root field"};
                return false;
            }
            continue;
        }

        if (context == Context::Stage) {
            if (!currentStage) { error = {lineNo, key, "internal stage context error"}; return false; }
            if (key == "stage_id") { if (!needString(currentStage->id)) return false; }
            else if (key == "order") { if (!needInt(currentStage->order)) return false; }
            else if (key == "when") { if (!needString(currentStage->when)) return false; }
            else if (key == "condition_status") { if (!needString(currentStage->conditionStatus)) return false; }
            else if (key == "objective") { if (!needWString(currentStage->objective)) return false; }
            else if (key == "player_notes") {
                if (value.type != TomlScalar::Type::StringArray) { error = {lineNo, key, "expected string array"}; return false; }
                currentStage->playerNotes.clear();
                for (const std::string& note : value.stringArray) currentStage->playerNotes.push_back(Utf8ToWide(note));
            }
            else if (key == "show_in_journal") { if (!needBool(currentStage->showInJournal)) return false; }
            else if (key == "optional") { if (!needBool(currentStage->optional)) return false; }
            else if (key == "battle_group") { if (!needString(currentStage->battleGroup)) return false; }
            else if (key == "item_dependency") { if (!needWString(currentStage->itemDependency)) return false; }
            else if (key == "reads_var") { if (!needString(currentStage->readsVar)) return false; }
            else if (key == "writes_var") { if (!needString(currentStage->writesVar)) return false; }
            else if (key == "stage_result") { if (!needWString(currentStage->stageResult)) return false; }
            else if (key == "evidence") { if (!needString(currentStage->evidence)) return false; }
            else if (key == "notes") { if (!needString(currentStage->notes)) return false; }
            else { error = {lineNo, key, "unknown stage field"}; return false; }
            continue;
        }

        if (context == Context::StageTarget) {
            if (!currentStage) { error = {lineNo, key, "internal stage target context error"}; return false; }
            if (key == "kind") { if (!needString(currentStage->targetKind)) return false; }
            else if (key == "scene") { if (!needString(currentStage->targetScene)) return false; currentStage->targetScene = ToLowerAscii(Trim(currentStage->targetScene)); }
            else if (key == "runtime_event") { if (!needString(currentStage->runtimeEventText)) return false; }
            else if (key == "trigger_type") { if (!needString(currentStage->triggerType)) return false; }
            else if (key == "object") { if (!needWString(currentStage->targetObject)) return false; }
            else if (key == "world_x") { if (!needString(currentStage->worldXText)) return false; }
            else if (key == "world_y") { if (!needString(currentStage->worldYText)) return false; }
            else if (key == "manual_world_x") { if (!needString(currentStage->manualWorldXText)) return false; }
            else if (key == "manual_world_y") { if (!needString(currentStage->manualWorldYText)) return false; }
            else if (key == "marker_override") { if (!needString(currentStage->runtimeMarkerOverride)) return false; }
            else { error = {lineNo, key, "unknown stage.target field"}; return false; }
            continue;
        }

        if (context == Context::StageMarker) {
            if (!currentStage) { error = {lineNo, key, "internal stage marker context error"}; return false; }
            if (key == "location_name") { if (!needWString(currentStage->locationName)) return false; }
            else if (key == "name") { if (!needWString(currentStage->markerName)) return false; }
            else if (key == "hint") { if (!needWString(currentStage->markerHint)) return false; }
            else if (key == "scope") {
                std::string scopeText;
                if (!needString(scopeText)) return false;
                if (!ParseMarkerScopeText(scopeText, currentStage->markerScope)) { error = {lineNo, key, "expected target/area/entrance"}; return false; }
            }
            else if (key == "role") { if (!needString(currentStage->role)) return false; }
            else { error = {lineNo, key, "unknown stage.marker field"}; return false; }
            continue;
        }

        if (context == Context::StageNavigation) {
            if (!currentStage) { error = {lineNo, key, "internal stage navigation context error"}; return false; }
            if (key == "mode") { if (!needString(currentStage->routeMode)) return false; }
            else { error = {lineNo, key, "unknown stage.navigation field"}; return false; }
            continue;
        }
        if (context == Context::StageRoute) {
            if (!currentStage || !currentRoute) { error = {lineNo, key, "internal stage.route context error"}; return false; }
            std::string ignored;
            if (key == "id") { if (!needString(currentRoute->id)) return false; }
            else if (key == "from_scene") { if (!needString(currentRoute->fromScene)) return false; currentRoute->fromScene = ToLowerAscii(Trim(currentRoute->fromScene)); }
            else if (key == "to_scene") { if (!needString(currentRoute->toScene)) return false; currentRoute->toScene = ToLowerAscii(Trim(currentRoute->toScene)); }
            else if (key == "event_id") { if (!needInt(currentRoute->eventId)) return false; }
            else if (key == "manual_event_id") { if (!needInt(currentRoute->manualEventId)) return false; currentRoute->manualEventIdSet = true; }
            else if (key == "trigger_type") {
                std::string trigger; if (!needString(trigger)) return false;
                if (!ParseRouteTriggerTypeText(trigger, currentRoute->eventTargetType)) { error = {lineNo, key, "unknown stage.route trigger_type"}; return false; }
            }
            else if (key == "manual_trigger_type") {
                std::string trigger; if (!needString(trigger)) return false;
                if (!ParseRouteTriggerTypeText(trigger, currentRoute->manualEventTargetType)) { error = {lineNo, key, "unknown stage.route manual_trigger_type"}; return false; }
                currentRoute->manualTriggerTypeSet = true;
            }
            else if (key == "target_object") { if (!needWString(currentRoute->targetObject)) return false; }
            else if (key == "manual_target_object") { if (!needWString(currentRoute->manualTargetObject)) return false; currentRoute->manualTargetObjectSet = true; }
            else if (key == "from_name") { if (!needWString(currentRoute->fromName)) return false; }
            else if (key == "to_name") { if (!needWString(currentRoute->toName)) return false; }
            else if (key == "marker_name") { if (!needWString(currentRoute->markerName)) return false; }
            else if (key == "route_hint") { if (!needWString(currentRoute->routeHint)) return false; }
            else if (key == "usage") { if (!needWString(currentRoute->usage)) return false; }
            else if (key == "verification_status") { if (!needWString(currentRoute->verificationStatus)) return false; }
            else if (key == "correction_reason") { if (!needWString(currentRoute->correctionReason)) return false; }
            else if (key == "test_notes") { if (!needWString(currentRoute->testNotes)) return false; }
            else if (key == "marker_scope") {
                std::string scopeText; if (!needString(scopeText)) return false;
                if (!ParseMarkerScopeText(scopeText, currentRoute->markerScope)) { error = {lineNo, key, "expected target/area/entrance"}; return false; }
            }
            else if (key == "world_x") { int v = 0; if (!needInt(v)) return false; currentRoute->worldX = v; currentRoute->worldXSet = true; }
            else if (key == "world_y") { int v = 0; if (!needInt(v)) return false; currentRoute->worldY = v; currentRoute->worldYSet = true; }
            else if (key == "manual_world_x") { int v = 0; if (!needInt(v)) return false; currentRoute->manualWorldX = v; currentRoute->manualWorldXSet = true; }
            else if (key == "manual_world_y") { int v = 0; if (!needInt(v)) return false; currentRoute->manualWorldY = v; currentRoute->manualWorldYSet = true; }
            else if (key == "native_event_bindings") {
                std::string bindings; if (!needString(bindings)) return false;
                if (!ParseRouteNativeBindings(bindings, currentRoute->nativeBindings)) { error = {lineNo, key, "invalid stage.route native_event_bindings"}; return false; }
            }
            else if (key == "native_navigable") { if (!needBool(currentRoute->nativeNavigable)) return false; }
            else if (key == "when") { if (!needString(currentRoute->when)) return false; }
            else if (key == "runtime_only") { if (!needBool(currentRoute->runtimeOnly)) return false; }
            else if (key == "advance_safe") { if (!needBool(currentRoute->advanceSafe)) return false; }
            else { error = {lineNo, key, "unknown stage.route field"}; return false; }
            continue;
        }

    }

    if (formatVersion != 7) { error = {0, "format_version", "missing or unsupported format_version; expected 7"}; return false; }
    quest.questNo = Trim(quest.questNo);
    quest.id = Trim(quest.id);
    if (!IsStableQuestNumber(quest.questNo)) { error = {0, "quest_no", "quest_no must match Q000"}; return false; }
    if (quest.id.empty()) { error = {0, "quest_id", "quest_id must not be empty"}; return false; }
    if (quest.title.empty()) { error = {0, "title", "quest title must not be empty"}; return false; }

    // 状态优先级的每个条件都必须能被现有只读条件解释器理解。
    if (!ValidateConditionSyntax(quest.availableWhen)) { error = {0, "offer_when", "invalid condition expression"}; return false; }
    if (!ValidateConditionSyntax(quest.offerMarkerWhen)) { error = {0, "offer_marker_when", "invalid condition expression"}; return false; }
    if (!ValidateConditionSyntax(quest.journalWhen)) { error = {0, "journal_when", "invalid condition expression"}; return false; }
    if (!ValidateConditionSyntax(quest.activeWhen)) { error = {0, "active_when", "invalid condition expression"}; return false; }
    if (!ValidateConditionSyntax(quest.completedWhen)) { error = {0, "completed_when", "invalid condition expression"}; return false; }
    if (!ValidateConditionSyntax(quest.failedWhen)) { error = {0, "failed_when", "invalid condition expression"}; return false; }
    if (!ValidateConditionSyntax(quest.expiredWhen)) { error = {0, "expired_when", "invalid condition expression"}; return false; }

    std::set<std::string> stageIds;
    for (QuestStage& stage : quest.stages) {
        stage.id = Trim(stage.id);
        if (stage.id.empty()) { error = {0, "stage.stage_id", "stage_id must not be empty"}; return false; }
        if (!stageIds.insert(ToLowerAscii(stage.id)).second) { error = {0, "stage.stage_id", "duplicate stage_id: " + stage.id}; return false; }
        if (!ValidateConditionSyntax(stage.when)) { error = {0, "stage.when", "invalid condition in " + quest.id + "/" + stage.id}; return false; }
        const bool manualX = !Trim(stage.manualWorldXText).empty();
        const bool manualY = !Trim(stage.manualWorldYText).empty();
        if (manualX != manualY) { error = {0, "stage.manual_world", "manual_world_x/manual_world_y must appear together"}; return false; }

        // 这里根据 Stage 自己的目标字段生成 0 或 1 个内部 Marker。
        // CRAFT / ITEM_USE 等没有地图目标的阶段自然得到 0 个 Marker，并不算错误。
        MarkerDefinition marker;
        std::string markerWhy;
        if (BuildMarkerFromStageTarget(stage, marker, markerWhy)) {
            stage.routeMarkers.push_back(marker);
        } else if (!markerWhy.empty()) {
            error = {0, "stage.target", "invalid target in " + quest.id + "/" + stage.id + ": " + markerWhy};
            return false;
        }
        // dev6y：Stage Route 是运行时数据，因此在 Quest 文件事务内一起严格校验。
        // R ID 只要求在“当前 Stage 内”唯一；同一物理边被多个任务/阶段复用时允许重复出现。
        std::set<std::string> routeIds;
        for (RouteEdge& edge : stage.routes) {
            edge.id = Trim(edge.id);
            if (!IsStableRouteId(edge.id)) { error = {0, "stage.route.id", "route id must match R000000 in " + quest.id + "/" + stage.id}; return false; }
            if (!routeIds.insert(edge.id).second) { error = {0, "stage.route.id", "duplicate route id in same stage: " + edge.id}; return false; }
            if (edge.fromScene.empty() || edge.toScene.empty()) { error = {0, "stage.route.scene", "from_scene/to_scene must not be empty in " + edge.id}; return false; }
            if (edge.eventId < 0 || edge.eventId > 255) { error = {0, "stage.route.event_id", "event_id must be 0..255 in " + edge.id}; return false; }
            if (edge.manualEventIdSet && (edge.manualEventId < 0 || edge.manualEventId > 255)) { error = {0, "stage.route.manual_event_id", "manual_event_id must be 0..255 in " + edge.id}; return false; }
            if (edge.worldXSet != edge.worldYSet) { error = {0, "stage.route.world", "world_x/world_y must appear together in " + edge.id}; return false; }
            edge.worldValid = edge.worldXSet && edge.worldYSet;
            if (edge.manualWorldXSet != edge.manualWorldYSet) { error = {0, "stage.route.manual_world", "manual_world_x/manual_world_y must appear together in " + edge.id}; return false; }
            edge.manualWorldValid = edge.manualWorldXSet && edge.manualWorldYSet;
            if (edge.nativeNavigable && edge.nativeBindings.empty()) { error = {0, "stage.route.native_navigable", "native_navigable=true but no native_event_bindings in " + edge.id}; return false; }
            if (!edge.advanceSafe || edge.runtimeOnly) { error = {0, "stage.route", "runtime Stage Route must be advance_safe=true and runtime_only=false in " + edge.id}; return false; }
            if (!ValidateConditionSyntax(edge.when)) { error = {0, "stage.route.when", "invalid condition in " + edge.id}; return false; }
            if (!RouteHasUsableNavigationBinding(edge)) { error = {0, "stage.route", "route has no usable current-event/manual binding in " + edge.id}; return false; }
        }
        const std::string routeMode = ToLowerAscii(Trim(stage.routeMode));
        if (routeMode != "direct" && routeMode != "stage_route" && routeMode != "unresolved") {
            error = {0, "stage.navigation.mode", "mode must be direct/stage_route/unresolved in " + quest.id + "/" + stage.id};
            return false;
        }
        if (!stage.routes.empty() && routeMode != "stage_route") { error = {0, "stage.navigation.mode", "stage with [[stage.route]] must use mode=stage_route in " + quest.id + "/" + stage.id}; return false; }
        if (stage.routes.empty() && routeMode == "stage_route") { error = {0, "stage.navigation.mode", "mode=stage_route requires at least one [[stage.route]] in " + quest.id + "/" + stage.id}; return false; }
    }
    std::sort(quest.stages.begin(), quest.stages.end(), [](const QuestStage& a, const QuestStage& b) {
        if (a.order != b.order) return a.order < b.order;
        return a.id < b.id;
    });

    // Offer Marker 也不再需要 [[offer]] 子表，但不能只根据 discover_mode 猜。
    // dev6g 的真实运行数据库里存在：
    //   * discover_mode=when_active 但仍有 Offer 的任务；
    //   * discover_mode=hidden_meta 但仍有 Offer 的任务；
    //   * discover_mode=offer_target_current_scene 却明确没有 Offer 的任务。
    // 所以 dev6h 在每个 Quest 顶层显式保存 offer_scene / offer_target，完整继承旧运行语义。
    quest.runtimeOfferScene = ToLowerAscii(Trim(quest.runtimeOfferScene));
    quest.runtimeOfferTarget = Trim(quest.runtimeOfferTarget);
    if (quest.runtimeOfferScene.empty() != quest.runtimeOfferTarget.empty()) {
        error = {0, "offer", "offer_scene and offer_target must both exist or both be empty"};
        return false;
    }
    if (!quest.runtimeOfferScene.empty()) {
        MarkerTarget target;
        if (!ParseTarget(quest.runtimeOfferTarget, target)) {
            error = {0, "offer_target", "invalid offer target expression"};
            return false;
        }
        MarkerDefinition offer;
        offer.sceneId = quest.runtimeOfferScene;
        // Available 负责“任务机会存在”，offer_marker_when 再决定“现在是否让玩家看见叹号”。
        offer.when = quest.offerMarkerWhen;
        offer.target = target;
        offer.role = MarkerRole::Offer;
        offer.scope = MarkerScope::Target;
        offer.markerName = quest.title;
        offer.style = "auto";
        quest.offerMarkers.push_back(offer);
    }

    return true;
}


// ============================================================================
// 8E. dev6zd 同名 _addon.toml：原始任务链不动，人工体验层在内存中叠加
// ============================================================================
//
// 设计目标故意保持简单：
//   * Qxxx_name.toml       = 详细原始任务链，保存已经研究闭合的游戏事实；
//   * Qxxx_name_addon.toml = Remastered 人工调整层，只做局部覆盖和人工插入。
//
// addon 永远后加载，因此它的“已填写字段”优先级高于原始任务数据；没有填写的字段继续继承原始值。
// 这里绝不把 addon 写回原文件，也绝不允许 addon 去写 GameVar。人工插入步骤只等待“指定 Scene 的指定 Event
// 实际开始执行”这一只读事实，然后在本次游戏进程里记为完成，继续显示原始 before Stage。

struct QuestAddonOverride {
    std::string stageId;

    bool objectiveSet = false;
    std::wstring objective;
    bool playerNotesSet = false;
    std::vector<std::wstring> playerNotes;
    bool showInJournalSet = false;
    bool showInJournal = true;
    bool optionalSet = false;
    bool optional = false;

    bool sceneSet = false;
    std::string scene;
    bool eventSet = false;
    int eventId = -1;
    bool objectSet = false;
    std::wstring object;
    bool worldXSet = false;
    bool worldYSet = false;
    int worldX = 0;
    int worldY = 0;

    bool locationNameSet = false;
    std::wstring locationName;
    bool markerNameSet = false;
    std::wstring markerName;
    bool markerHintSet = false;
    std::wstring markerHint;
};

struct QuestAddonInsert {
    std::string stageId;
    std::string afterStageId;
    std::string beforeStageId;
    std::wstring objective;
    std::vector<std::wstring> playerNotes;
    bool showInJournal = true;
    bool optional = true;
    std::string scene;
    int eventId = -1;
    std::wstring object;
    int worldX = 0;
    int worldY = 0;
    bool worldXSet = false;
    bool worldYSet = false;
    std::wstring locationName;
    std::wstring markerName;
    std::wstring markerHint;
};

struct QuestAddonDefinition {
    int formatVersion = -1;
    std::string questId;
    std::vector<QuestAddonOverride> overrides;
    std::vector<QuestAddonInsert> inserts;
};

static bool WideEndsWithInsensitive(const std::wstring& value, const std::wstring& suffix) {
    if (suffix.size() > value.size()) return false;
    const size_t start = value.size() - suffix.size();
    for (size_t i = 0; i < suffix.size(); ++i) {
        if (std::towlower(value[start + i]) != std::towlower(suffix[i])) return false;
    }
    return true;
}

static std::wstring AddonPathForQuestPath(const std::wstring& questPath) {
    const std::wstring suffix = L".toml";
    if (!WideEndsWithInsensitive(questPath, suffix)) return questPath + L"_addon.toml";
    return questPath.substr(0, questPath.size() - suffix.size()) + L"_addon.toml";
}

static bool LoadQuestAddonFileV2(const std::wstring& path, QuestAddonDefinition& addon, TomlLoadError& error) {
    std::string bytes;
    if (!ReadUtf8TextFile(path, bytes, error)) return false;

    addon = QuestAddonDefinition();
    QuestAddonOverride* currentOverride = nullptr;
    QuestAddonInsert* currentInsert = nullptr;
    enum class Context { Root, Addon, Override, Insert } context = Context::Root;
    std::set<std::string> keysInCurrentTable;

    std::istringstream input(bytes);
    std::string rawLine;
    int lineNo = 0;
    while (std::getline(input, rawLine)) {
        ++lineNo;
        if (!rawLine.empty() && rawLine.back() == '\r') rawLine.pop_back();
        const std::string line = Trim(StripTomlComment(rawLine));
        if (line.empty()) continue;

        if (line.size() >= 4 && line.substr(0, 2) == "[[" && line.substr(line.size() - 2) == "]]" ) {
            const std::string header = ToLowerAscii(Trim(line.substr(2, line.size() - 4)));
            keysInCurrentTable.clear();
            if (header == "override") {
                addon.overrides.emplace_back();
                currentOverride = &addon.overrides.back();
                currentInsert = nullptr;
                context = Context::Override;
            } else if (header == "insert") {
                addon.inserts.emplace_back();
                currentInsert = &addon.inserts.back();
                currentOverride = nullptr;
                context = Context::Insert;
            } else {
                error = {lineNo, header, "addon v2 only supports [[override]] and [[insert]] arrays"};
                return false;
            }
            continue;
        }
        if (line.size() >= 3 && line.front() == '[' && line.back() == ']') {
            const std::string header = ToLowerAscii(Trim(line.substr(1, line.size() - 2)));
            keysInCurrentTable.clear();
            if (header == "addon") {
                currentOverride = nullptr;
                currentInsert = nullptr;
                context = Context::Addon;
            } else {
                error = {lineNo, header, "addon v2 only supports [addon] plus [[override]]/[[insert]]"};
                return false;
            }
            continue;
        }

        bool inString = false;
        bool escaped = false;
        size_t equalPos = std::string::npos;
        for (size_t i = 0; i < line.size(); ++i) {
            const char ch = line[i];
            if (inString) {
                if (escaped) escaped = false;
                else if (ch == '\\') escaped = true;
                else if (ch == '"') inString = false;
            } else {
                if (ch == '"') inString = true;
                else if (ch == '=') { equalPos = i; break; }
            }
        }
        if (equalPos == std::string::npos) { error = {lineNo, "", "expected key = value"}; return false; }
        const std::string key = ToLowerAscii(Trim(line.substr(0, equalPos)));
        if (key.empty()) { error = {lineNo, "", "key must not be empty"}; return false; }
        if (!keysInCurrentTable.insert(key).second) { error = {lineNo, key, "duplicate key in same table"}; return false; }

        std::string logicalValue;
        if (!ReadTomlValueAllowMultilineStringArray(input, lineNo, line.substr(equalPos + 1), key, logicalValue, error)) return false;

        TomlScalar value;
        std::string why;
        if (!ParseTomlScalar(logicalValue, value, why)) { error = {lineNo, key, why}; return false; }
        auto needString = [&](std::string& dst) -> bool {
            if (value.type != TomlScalar::Type::String) { error = {lineNo, key, "expected string"}; return false; }
            dst = value.stringValue;
            return true;
        };
        auto needWString = [&](std::wstring& dst) -> bool {
            if (value.type != TomlScalar::Type::String) { error = {lineNo, key, "expected string"}; return false; }
            dst = Utf8ToWide(value.stringValue);
            return true;
        };
        auto needBool = [&](bool& dst) -> bool {
            if (value.type != TomlScalar::Type::Boolean) { error = {lineNo, key, "expected boolean"}; return false; }
            dst = value.boolValue;
            return true;
        };
        auto needInt = [&](int& dst) -> bool {
            if (value.type != TomlScalar::Type::Integer || value.intValue < INT_MIN || value.intValue > INT_MAX) {
                error = {lineNo, key, "expected 32-bit integer"};
                return false;
            }
            dst = static_cast<int>(value.intValue);
            return true;
        };
        auto needStrictIntString = [&](int& dst) -> bool {
            std::string text;
            if (!needString(text)) return false;
            if (!TryParseStrictIntText(text, dst)) {
                error = {lineNo, key, "expected quoted integer string, same as base TOML"};
                return false;
            }
            return true;
        };
        auto needNotes = [&](std::vector<std::wstring>& dst) -> bool {
            if (value.type != TomlScalar::Type::StringArray) { error = {lineNo, key, "expected string array"}; return false; }
            dst.clear();
            for (const std::string& note : value.stringArray) dst.push_back(Utf8ToWide(note));
            return true;
        };

        if (context == Context::Root) {
            if (key == "format_version") { if (!needInt(addon.formatVersion)) return false; }
            else { error = {lineNo, key, "addon root only allows format_version"}; return false; }
            continue;
        }
        if (context == Context::Addon) {
            if (key == "quest_id") { if (!needString(addon.questId)) return false; }
            else { error = {lineNo, key, "unknown addon field"}; return false; }
            continue;
        }
        if (context == Context::Override) {
            if (!currentOverride) { error = {lineNo, key, "internal override context error"}; return false; }
            if (key == "stage_id") { if (!needString(currentOverride->stageId)) return false; }
            else if (key == "objective") { if (!needWString(currentOverride->objective)) return false; currentOverride->objectiveSet = true; }
            else if (key == "player_notes") { if (!needNotes(currentOverride->playerNotes)) return false; currentOverride->playerNotesSet = true; }
            else if (key == "show_in_journal") { if (!needBool(currentOverride->showInJournal)) return false; currentOverride->showInJournalSet = true; }
            else if (key == "optional") { if (!needBool(currentOverride->optional)) return false; currentOverride->optionalSet = true; }
            else if (key == "scene") { if (!needString(currentOverride->scene)) return false; currentOverride->sceneSet = true; }
            else if (key == "runtime_event") { if (!needStrictIntString(currentOverride->eventId)) return false; currentOverride->eventSet = true; }
            else if (key == "object") { if (!needWString(currentOverride->object)) return false; currentOverride->objectSet = true; }
            else if (key == "world_x") { if (!needStrictIntString(currentOverride->worldX)) return false; currentOverride->worldXSet = true; }
            else if (key == "world_y") { if (!needStrictIntString(currentOverride->worldY)) return false; currentOverride->worldYSet = true; }
            else if (key == "location_name") { if (!needWString(currentOverride->locationName)) return false; currentOverride->locationNameSet = true; }
            else if (key == "name") { if (!needWString(currentOverride->markerName)) return false; currentOverride->markerNameSet = true; }
            else if (key == "hint") { if (!needWString(currentOverride->markerHint)) return false; currentOverride->markerHintSet = true; }
            else { error = {lineNo, key, "unknown override field"}; return false; }
            continue;
        }
        if (context == Context::Insert) {
            if (!currentInsert) { error = {lineNo, key, "internal insert context error"}; return false; }
            if (key == "stage_id") { if (!needString(currentInsert->stageId)) return false; }
            else if (key == "after") { if (!needString(currentInsert->afterStageId)) return false; }
            else if (key == "before") { if (!needString(currentInsert->beforeStageId)) return false; }
            else if (key == "objective") { if (!needWString(currentInsert->objective)) return false; }
            else if (key == "player_notes") { if (!needNotes(currentInsert->playerNotes)) return false; }
            else if (key == "show_in_journal") { if (!needBool(currentInsert->showInJournal)) return false; }
            else if (key == "optional") { if (!needBool(currentInsert->optional)) return false; }
            else if (key == "scene") { if (!needString(currentInsert->scene)) return false; }
            else if (key == "runtime_event") { if (!needStrictIntString(currentInsert->eventId)) return false; }
            else if (key == "object") { if (!needWString(currentInsert->object)) return false; }
            else if (key == "world_x") { if (!needStrictIntString(currentInsert->worldX)) return false; currentInsert->worldXSet = true; }
            else if (key == "world_y") { if (!needStrictIntString(currentInsert->worldY)) return false; currentInsert->worldYSet = true; }
            else if (key == "location_name") { if (!needWString(currentInsert->locationName)) return false; }
            else if (key == "name") { if (!needWString(currentInsert->markerName)) return false; }
            else if (key == "hint") { if (!needWString(currentInsert->markerHint)) return false; }
            else { error = {lineNo, key, "unknown insert field"}; return false; }
            continue;
        }
        error = {lineNo, key, "key is outside a supported addon table"};
        return false;
    }

    addon.questId = Trim(addon.questId);
    if (addon.formatVersion != 2) { error = {0, "format_version", "missing or unsupported addon format_version; expected 2"}; return false; }
    if (addon.questId.empty()) { error = {0, "quest_id", "addon quest_id must not be empty"}; return false; }

    std::set<std::string> insertIds;
    for (QuestAddonOverride& ov : addon.overrides) {
        ov.stageId = Trim(ov.stageId);
        if (ov.stageId.empty()) { error = {0, "override.stage_id", "stage_id must not be empty"}; return false; }
        if (ov.eventSet && (ov.eventId < 0 || ov.eventId > 255)) { error = {0, "override.runtime_event", "runtime_event must be 0..255"}; return false; }
        if (ov.worldXSet != ov.worldYSet) { error = {0, "override.world", "world_x/world_y must appear together"}; return false; }
        if (ov.sceneSet) ov.scene = ToLowerAscii(Trim(ov.scene));
    }
    for (QuestAddonInsert& ins : addon.inserts) {
        ins.stageId = Trim(ins.stageId);
        ins.afterStageId = Trim(ins.afterStageId);
        ins.beforeStageId = Trim(ins.beforeStageId);
        ins.scene = ToLowerAscii(Trim(ins.scene));
        if (ins.stageId.empty()) { error = {0, "insert.stage_id", "stage_id must not be empty"}; return false; }
        if (!insertIds.insert(ToLowerAscii(ins.stageId)).second) { error = {0, "insert.stage_id", "duplicate stage_id: " + ins.stageId}; return false; }
        if (ins.afterStageId.empty() || ins.beforeStageId.empty()) { error = {0, "insert.anchor", "after/before must both be present"}; return false; }
        if (ins.objective.empty()) { error = {0, "insert.objective", "objective must not be empty"}; return false; }
        if (ins.scene.empty()) { error = {0, "insert.scene", "scene must not be empty"}; return false; }
        if (ins.eventId < 0 || ins.eventId > 255) { error = {0, "insert.runtime_event", "runtime_event must be 0..255"}; return false; }
        if (!ins.worldXSet || !ins.worldYSet) { error = {0, "insert.world_x/world_y", "world_x/world_y are required for an inserted step"}; return false; }
    }
    return true;
}

static QuestStage* FindMutableStageById(QuestDefinition& quest, const std::string& stageId) {
    for (QuestStage& stage : quest.stages) {
        if (IEqualsAscii(stage.id, stageId)) return &stage;
    }
    return nullptr;
}

static const QuestStage* FindStageById(const QuestDefinition& quest, const std::string& stageId, size_t* indexOut = nullptr) {
    for (size_t i = 0; i < quest.stages.size(); ++i) {
        if (IEqualsAscii(quest.stages[i].id, stageId)) {
            if (indexOut) *indexOut = i;
            return &quest.stages[i];
        }
    }
    return nullptr;
}

static bool RebuildStageMarkerAfterAddon(QuestDefinition& quest, QuestStage& stage, TomlLoadError& error) {
    stage.routeMarkers.clear();
    MarkerDefinition marker;
    std::string why;
    if (BuildMarkerFromStageTarget(stage, marker, why)) {
        stage.routeMarkers.push_back(marker);
        return true;
    }
    if (!why.empty()) {
        error = {0, "addon.override", "invalid final target after addon in " + quest.id + "/" + stage.id + ": " + why};
        return false;
    }
    return true;
}

static bool ApplyQuestAddon(const QuestAddonDefinition& addon, QuestDefinition& quest, TomlLoadError& error) {
    if (!IEqualsAscii(addon.questId, quest.id)) {
        error = {0, "addon.quest_id", "addon quest_id=" + addon.questId + " does not match base quest id=" + quest.id};
        return false;
    }

    // 先应用局部覆盖。只覆盖 addon 明确写出的字段；特别是 world_x/world_y 被解释成运行 Marker 的人工坐标，
    // 不会抹掉详细 TOML 中保存的研究原始坐标。这样 F11/文档仍能追溯“原证据是多少、体验修正是多少”。
    for (const QuestAddonOverride& ov : addon.overrides) {
        QuestStage* stage = FindMutableStageById(quest, ov.stageId);
        if (!stage) {
            error = {0, "override.stage_id", "stage not found in quest " + quest.id + ": " + ov.stageId};
            return false;
        }
        if (ov.objectiveSet) stage->objective = ov.objective;
        if (ov.playerNotesSet) stage->playerNotes = ov.playerNotes;
        if (ov.showInJournalSet) stage->showInJournal = ov.showInJournal;
        if (ov.optionalSet) stage->optional = ov.optional;
        if (ov.sceneSet) stage->targetScene = ov.scene;
        if (ov.eventSet) stage->runtimeEventText = std::to_string(ov.eventId);
        if (ov.objectSet) stage->targetObject = ov.object;
        if (ov.worldXSet && ov.worldYSet) {
            stage->manualWorldXText = std::to_string(ov.worldX);
            stage->manualWorldYText = std::to_string(ov.worldY);
        }
        if (ov.locationNameSet) stage->locationName = ov.locationName;
        if (ov.markerNameSet) stage->markerName = ov.markerName;
        if (ov.markerHintSet) stage->markerHint = ov.markerHint;
        if (!RebuildStageMarkerAfterAddon(quest, *stage, error)) return false;
    }

    // 再校验人工插入点。after/before 必须是详细原始链里相邻的两个 Stage；这条严格规则用于防止详细库日后
    // 修订后旧 addon 悄悄插错位置。一个边可以连续写多个 [[insert]]，运行时按 addon 文件出现顺序逐个完成。
    std::set<std::string> allIds;
    for (const QuestStage& stage : quest.stages) allIds.insert(ToLowerAscii(stage.id));
    for (const QuestAddonInsert& ins : addon.inserts) {
        if (!allIds.insert(ToLowerAscii(ins.stageId)).second) {
            error = {0, "insert.stage_id", "stage_id collides with another stage/addon stage_id in quest " + quest.id + ": " + ins.stageId};
            return false;
        }
        size_t afterIndex = 0;
        size_t beforeIndex = 0;
        if (!FindStageById(quest, ins.afterStageId, &afterIndex)) {
            error = {0, "insert.after", "after stage not found in quest " + quest.id + ": " + ins.afterStageId};
            return false;
        }
        if (!FindStageById(quest, ins.beforeStageId, &beforeIndex)) {
            error = {0, "insert.before", "before stage not found in quest " + quest.id + ": " + ins.beforeStageId};
            return false;
        }
        if (afterIndex + 1 != beforeIndex) {
            error = {0, "insert.anchor", "after/before are not adjacent in the base chain: " + ins.afterStageId + " -> " + ins.beforeStageId};
            return false;
        }

        QuestStage stage;
        stage.id = ins.stageId;
        stage.objective = ins.objective;
        stage.playerNotes = ins.playerNotes;
        stage.showInJournal = ins.showInJournal;
        stage.optional = ins.optional;
        stage.targetKind = "EVENT_ANY";
        stage.targetScene = ins.scene;
        stage.runtimeEventText = std::to_string(ins.eventId);
        stage.targetObject = ins.object;
        // 人工插入明确要求提供世界坐标，所以直接走现有 manual_world 最高优先级。Event 只负责完成判定，
        // 坐标只负责 Marker 位置，两件事不再互相绑死。
        stage.manualWorldXText = std::to_string(ins.worldX);
        stage.manualWorldYText = std::to_string(ins.worldY);
        stage.locationName = ins.locationName;
        stage.markerName = !ins.markerName.empty() ? ins.markerName : ins.object;
        stage.markerHint = !ins.markerHint.empty() ? ins.markerHint : ins.objective;
        stage.markerScope = MarkerScope::Target;
        stage.role = "talk_or_interact";
        stage.routeMode = "direct";
        stage.addonInserted = true;
        stage.addonId = ins.stageId;
        stage.addonAfterStageId = ins.afterStageId;
        stage.addonBeforeStageId = ins.beforeStageId;
        stage.addonCompletionScene = ins.scene;
        stage.addonCompletionEvent = ins.eventId;

        if (!RebuildStageMarkerAfterAddon(quest, stage, error)) return false;
        quest.addonInserts.push_back(std::move(stage));
    }
    return true;
}

static std::string AddonCompletionKey(const QuestDefinition& quest, const QuestStage& stage) {
    return ToLowerAscii(quest.id) + "|" + ToLowerAscii(stage.addonId.empty() ? stage.id : stage.addonId);
}

static bool EnumerateQuestTomlFiles(const std::wstring& dataRoot,
                                    std::vector<std::wstring>& baseFiles,
                                    std::vector<std::wstring>& addonFiles,
                                    std::string& why) {
    // dev6zc 仍然只用 Win32 API，但现在 Q*.toml 中有两种文件。
    // 文件名以 _addon.toml 结束的放进 addonFiles；其余 Q*.toml 才是详细原始任务链。
    // 两组都按文件名排序，稍后还会逐个检查“每个原始文件必须存在同名 addon”，所以不会把 Q004 的 addon
    // 错配到 Q005，也不会因为目录枚举顺序不同而出现不稳定行为。
    baseFiles.clear();
    addonFiles.clear();
    const std::wstring pattern = JoinPath(dataRoot, L"Q*.toml");
    WIN32_FIND_DATAW data = {};
    HANDLE find = FindFirstFileW(pattern.c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) {
        why = "task data directory has no readable Q*.toml files";
        return false;
    }
    do {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) continue;
        const std::wstring name = data.cFileName;
        if (name == L"." || name == L"..") continue;
        if (WideEndsWithInsensitive(name, L"_addon.toml")) addonFiles.push_back(JoinPath(dataRoot, name));
        else baseFiles.push_back(JoinPath(dataRoot, name));
    } while (FindNextFileW(find, &data));
    FindClose(find);
    std::sort(baseFiles.begin(), baseFiles.end());
    std::sort(addonFiles.begin(), addonFiles.end());
    if (baseFiles.empty()) {
        why = "task data directory contains zero base Q*.toml files";
        return false;
    }
    return true;
}

static bool LoadQuestDatabase() {
    // dev6zd 运行时目录仍然只有一层，但每条任务固定成对：
    // mods\asi\Castle_Quest\Qxxx_name.toml       = 详细原始任务链
    // mods\asi\Castle_Quest\Qxxx_name_addon.toml = Remastered 局部调整/人工插入
    // 整套内容仍然是一个事务：任一 Base/Addon/Stage/Route 失败，都继续使用上一份完整数据库。
    const std::wstring dataRoot = JoinPath(g_moduleDir, L"Castle_Quest");
    const std::wstring manifestPath = JoinPath(dataRoot, L"manifest.toml");

    QuestManifest manifest;
    TomlLoadError error;
    if (!LoadQuestManifestFromToml(manifestPath, manifest, error)) {
        Log("[任务数据错误] 文件=%s 行=%d 字段=%s 原因=%s；事务回退：继续使用上一份有效数据库。",
            WideToUtf8(manifestPath).c_str(), error.line, error.field.c_str(), error.message.c_str());
        return false;
    }

    std::vector<std::wstring> questFiles;
    std::vector<std::wstring> addonFiles;
    std::string enumerateWhy;
    if (!EnumerateQuestTomlFiles(dataRoot, questFiles, addonFiles, enumerateWhy)) {
        Log("[任务数据错误] 目录=%s 原因=%s；事务回退。", WideToUtf8(dataRoot).c_str(), enumerateWhy.c_str());
        return false;
    }
    if (static_cast<int>(questFiles.size()) != manifest.questCount ||
        static_cast<int>(addonFiles.size()) != manifest.addonCount) {
        Log("[任务数据错误] manifest 声明 Quest=%d Addon=%d，但目录实际 BaseQuest=%u Addon=%u；事务回退。",
            manifest.questCount, manifest.addonCount,
            static_cast<unsigned>(questFiles.size()), static_cast<unsigned>(addonFiles.size()));
        return false;
    }

    // 每个详细任务都必须有严格同名的 _addon.toml。即使 addon 当前只有注释和 [addon] 任务 ID，文件也必须存在，
    // 这样发布包永远是“一条任务链 = 详细文件 + 调整文件”成对出现，不会让用户猜某个任务到底有没有调整层。
    for (const std::wstring& questPath : questFiles) {
        const std::wstring addonPath = AddonPathForQuestPath(questPath);
        const DWORD attrs = GetFileAttributesW(addonPath.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            Log("[任务数据错误] 详细任务缺少同名 addon：Base=%s ExpectedAddon=%s；事务回退。",
                WideToUtf8(questPath).c_str(), WideToUtf8(addonPath).c_str());
            return false;
        }
    }

    std::vector<QuestDefinition> temporaryQuests;
    temporaryQuests.reserve(questFiles.size());
    std::set<std::string> questIds;
    std::set<std::string> questNumbers;
    size_t stageCount = 0;
    size_t canonicalStageCount = 0;
    size_t addonOverrideCount = 0;
    size_t addonInsertCount = 0;

    for (const std::wstring& questPath : questFiles) {
        QuestDefinition quest;
        TomlLoadError questError;
        if (!LoadQuestFileV7(questPath, quest, questError)) {
            Log("[任务数据错误] 文件=%s 行=%d 字段=%s 原因=%s；事务回退。",
                WideToUtf8(questPath).c_str(), questError.line, questError.field.c_str(), questError.message.c_str());
            return false;
        }
        if (!questIds.insert(ToLowerAscii(quest.id)).second) {
            Log("[任务数据错误] 跨文件重复 quest_id=%s；事务回退。", quest.id.c_str());
            return false;
        }
        if (!questNumbers.insert(quest.questNo).second) {
            Log("[任务数据错误] 跨文件重复 quest_no=%s；事务回退。", quest.questNo.c_str());
            return false;
        }

        // 详细链已经独立通过校验后，才读取同名 addon。addon 任一字段、Stage ID、after/before 锚点出错，
        // 整个 25 任务事务都不提交；运行中继续使用上一份完整有效数据库。
        const std::wstring addonPath = AddonPathForQuestPath(questPath);
        QuestAddonDefinition addon;
        TomlLoadError addonError;
        if (!LoadQuestAddonFileV2(addonPath, addon, addonError)) {
            Log("[任务数据错误] Addon文件=%s 行=%d 字段=%s 原因=%s；事务回退。",
                WideToUtf8(addonPath).c_str(), addonError.line, addonError.field.c_str(), addonError.message.c_str());
            return false;
        }
        if (!ApplyQuestAddon(addon, quest, addonError)) {
            Log("[任务数据错误] Addon文件=%s 行=%d 字段=%s 原因=%s；事务回退。",
                WideToUtf8(addonPath).c_str(), addonError.line, addonError.field.c_str(), addonError.message.c_str());
            return false;
        }
        addonOverrideCount += addon.overrides.size();
        addonInsertCount += addon.inserts.size();

        stageCount += quest.stages.size();
        // Base v7 已彻底取消 stage_type：所有 Base Stage 天然都是原版 canonical Stage。
        // 因此 canonical 数量直接等于本文件成功解析出的 Stage 数，不再维护第二套类型状态机。
        canonicalStageCount += quest.stages.size();
        temporaryQuests.push_back(std::move(quest));
    }

    if (static_cast<int>(stageCount) != manifest.stageCount ||
        static_cast<int>(canonicalStageCount) != manifest.canonicalStageCount) {
        Log("[任务数据错误] manifest 声明 Stage=%d(Canonical=%d)，实际 Stage=%u(Canonical=%u)；事务回退。",
            manifest.stageCount, manifest.canonicalStageCount,
            static_cast<unsigned>(stageCount), static_cast<unsigned>(canonicalStageCount));
        return false;
    }

    // Route 已经随每个 Quest/Stage 一起解析完成。这里统计总数只用于日志，
    // 不再存在独立 Route 文件，也不再让 manifest 维护第二套 Route 数量。
    size_t stageRouteCount = 0;
    for (const QuestDefinition& quest : temporaryQuests) {
        for (const QuestStage& stage : quest.stages) stageRouteCount += stage.routes.size();
    }

    // 最后一步才交换 Quest 容器。详细链、addon 覆盖、addon 插入与 Stage Route 已全部校验完成，
    // 所以前面任何一个文件失败都不会留下“半套新数据”。dev6zd 特别保留 g_completedAddonStageKeys：
    // 热重载只替换定义，不抹掉本进程已经完成的稳定 quest_id+stage_id；删除/恢复同 ID 也能继续匹配。
    g_quests.swap(temporaryQuests);

    Log("[任务数据] Base+Addon TOML原子事务提交成功：DB=%s Quest=%u BaseStage=%u(Canonical=%u) Addon=%u Override=%u Insert=%u StageRoute=%u。目录=%s",
        manifest.databaseVersion.c_str(), static_cast<unsigned>(g_quests.size()), static_cast<unsigned>(stageCount),
        static_cast<unsigned>(canonicalStageCount), static_cast<unsigned>(addonFiles.size()), static_cast<unsigned>(addonOverrideCount),
        static_cast<unsigned>(addonInsertCount), static_cast<unsigned>(stageRouteCount), WideToUtf8(dataRoot).c_str());
    return true;
}

#if defined(_MSC_VER)
// dev6l：继续沿用 dev6j 的装载区无优化边界；从这里恢复 build.bat 选择的正常优化级别。
// 后面的 Quest 求值、Route BFS、Marker 解算和软件 Overlay 都会在游戏运行过程中频繁执行，
// 所以只把“启动时数据库装载器”隔离为无优化区，而不是粗暴地让整个插件都用 /Od。
#pragma optimize("", on)
#endif


// ============================================================================
// 9. 任务状态求值
// ============================================================================

static QuestState EvaluateQuestState(const QuestDefinition& q, const GameSnapshot& snapshot) {
    // dev6h 按冻结数据库正式语义恢复完整优先级：Completed -> Failed -> Expired -> Active -> Available。
    // 以前运行时没有 Failed 枚举，只能把部分 failed_when 临时塞进 expired_when；这会把“做失败了”
    // 和“没来得及做、窗口关闭了”混成同一种状态。现在数据已经完整进入分任务 TOML，就不再做这种降维。
    bool ok = false;
    if (EvaluateCondition(q.completedWhen, snapshot, &ok) && ok) return QuestState::Completed;
    if (EvaluateCondition(q.failedWhen, snapshot, &ok) && ok) return QuestState::Failed;
    if (EvaluateCondition(q.expiredWhen, snapshot, &ok) && ok) return QuestState::Expired;
    if (EvaluateCondition(q.activeWhen, snapshot, &ok) && ok) return QuestState::Active;
    if (EvaluateCondition(q.availableWhen, snapshot, &ok) && ok) return QuestState::Available;
    return QuestState::Unavailable;
}

static const QuestStage* FindPendingAddonInsert(const QuestDefinition& quest, const QuestStage* baseStage) {
    if (!baseStage) return nullptr;
    // addon 文件中可以在同一 after->before 边连续写多个 [[insert]]。它们不需要互相引用 ID；
    // 这里严格按文件顺序找到第一条尚未完成的人工步骤，全部完成后才把原始 before Stage 还给 UI。
    for (const QuestStage& stage : quest.addonInserts) {
        if (!stage.addonInserted || !IEqualsAscii(stage.addonBeforeStageId, baseStage->id)) continue;
        if (g_completedAddonStageKeys.find(AddonCompletionKey(quest, stage)) == g_completedAddonStageKeys.end()) {
            return &stage;
        }
    }
    return nullptr;
}

// dev6zd 不再根据 QuestState 自动删除 Addon 完成键。
// 原因很直接：在没有 per-save .state 之前，Quest 根本不知道一次 Available/Unavailable
// 是“玩家真的读取了更早存档”，还是维护者刚改了 Base/Addon 条件并执行 Ctrl+F8。
// 如果仅凭当前任务状态就删除完成键，普通热重载仍可能把已经做过的人工步骤复活。
// 因此本版采用最小且可预测的临时规则：同一进程内完成键只增不减；进程退出自然清空。
// 等主 SDK 提供与 TSF 一一绑定的 .state 生命周期后，再由“加载了哪个存档”这一真实事件决定
// 应该装入哪一组完成键，而不是继续用 QuestState 猜测存档身份。

static std::vector<EvaluatedQuest> EvaluateAllQuests(const GameSnapshot& snapshot) {
    std::vector<EvaluatedQuest> out;
    for (const auto& q : g_quests) {
        if (!q.enabled) continue;
        EvaluatedQuest e;
        e.def = &q;
        e.state = EvaluateQuestState(q, snapshot);

        // 第一步永远只根据详细 Qxxx.toml 求“原始当前 Stage”。addon 不参与 GameState 条件判定，
        // 因此无论人工层怎么改，都不可能把 Var1/Var504 等原版状态重新解释成另一条剧情链。
        const QuestStage* baseStage = nullptr;
        if (e.state == QuestState::Active) {
            for (const auto& stage : q.stages) {
                if (!stage.showInJournal) continue;
                bool ok = false;
                if (EvaluateCondition(stage.when, snapshot, &ok) && ok) {
                    baseStage = &stage;
                    break;
                }
            }
        }

        // 第二步才叠加人工插入层。只有原始 Quest 已 Active 且原始求值已经走到 before Stage，
        // 才把 after->before 边上的第一条未完成人工步骤显示出来。原版 before Stage 本身没有被删除或改条件。
        if (e.state == QuestState::Active && baseStage) {
            if (const QuestStage* addonStage = FindPendingAddonInsert(q, baseStage)) {
                e.stage = addonStage;
                e.addonActive = true;
            } else {
                e.stage = baseStage;
            }
        }
        out.push_back(e);
    }
    return out;
}

static bool ObserveAddonStageCompletion(const GameSnapshot& snapshot, const std::vector<EvaluatedQuest>& evaluated) {
    static bool initialized = false;
    static uint32_t previousEvent = 0;
    static std::string previousScene;
    if (!snapshot.valid) return false;

    if (!initialized) {
        previousEvent = snapshot.activeEventId;
        previousScene = snapshot.sceneId;
        initialized = true;
        return false;
    }

    // Scene 切换时先同步基线，避免把上一张地图最后一个 Event 当成新场景刚触发的人工交互。
    if (!IEqualsAscii(previousScene, snapshot.sceneId)) {
        previousScene = snapshot.sceneId;
        previousEvent = snapshot.activeEventId;
        return false;
    }

    const bool newEventStarted = snapshot.activeEventId != 0 && snapshot.activeEventId != previousEvent;
    previousEvent = snapshot.activeEventId;
    if (!newEventStarted) return false;

    bool changed = false;
    for (const EvaluatedQuest& qv : evaluated) {
        if (!qv.def || !qv.addonActive || !qv.stage || !qv.stage->addonInserted) continue;
        const QuestStage& stage = *qv.stage;
        if (!IEqualsAscii(stage.addonCompletionScene, snapshot.sceneId)) continue;
        if (stage.addonCompletionEvent < 0 || static_cast<uint32_t>(stage.addonCompletionEvent) != snapshot.activeEventId) continue;

        const std::string key = AddonCompletionKey(*qv.def, stage);
        if (g_completedAddonStageKeys.insert(key).second) {
            Log("[Addon完成] Quest=%s Insert=%s Edge=%s->%s Scene=%s Event=%u；继续原始任务链。",
                qv.def->id.c_str(), stage.addonId.c_str(), stage.addonAfterStageId.c_str(), stage.addonBeforeStageId.c_str(),
                snapshot.sceneId.c_str(), static_cast<unsigned>(snapshot.activeEventId));
            changed = true;
        }
    }
    return changed;
}

// ============================================================================
// 10. Marker 解析：实体目标直接使用原版 SF2 渲染锚点
// ============================================================================

enum class MarkerAnchorKind {
    DefaultEntity,      // 普通实体锚点：沿用 WorldCamera / ControllerCamera 选择。
    TouchCollisionBox,  // 原版 0x4080D0 Section 0 接触矩形中心。
    ExplicitWorld       // TOML 中人工明确写出的 world:x,y。
};

static const char* MarkerAnchorKindName(MarkerAnchorKind kind) {
    switch (kind) {
        case MarkerAnchorKind::TouchCollisionBox: return "TouchBox";
        case MarkerAnchorKind::ExplicitWorld: return "ExplicitWorld";
        case MarkerAnchorKind::DefaultEntity:
        default: return "Entity";
    }
}

struct ResolvedTargetPoint {
    bool valid = false;
    bool fromEntity = false;
    size_t entityIndex = 0;
    int32_t screenX = 0;
    int32_t screenY = 0;
    MarkerAnchorKind anchorKind = MarkerAnchorKind::DefaultEntity;

    // dev6e 额外保留原版接触矩形，方便实机日志直接回答“Marker 为什么在这里”。
    bool touchBoxAnchorValid = false;
    int32_t touchBoxLeftWorld = 0;
    int32_t touchBoxTopWorld = 0;
    int32_t touchBoxWidth = 0;
    int32_t touchBoxHeight = 0;
    int32_t touchBoxScreenX = 0;
    int32_t touchBoxScreenY = 0;

    // 下面这些字段不是绘制必需，而是 dev6d 为“Marker 到底绑定到谁、两种投影各算出什么”增加的诊断证据。
    // 它们只从已经匹配成功的 RuntimeEntityView 复制，不会反向影响 Quest 选择。
    std::wstring entityName;
    uint8_t touchEventId = 0;
    uint8_t manualEventId = 0;
    bool worldAnchorValid = false;
    bool controllerAnchorValid = false;
    int32_t worldScreenX = 0;
    int32_t worldScreenY = 0;
    int32_t controllerScreenX = 0;
    int32_t controllerScreenY = 0;
};

struct ResolvedMarker {
    const QuestDefinition* quest = nullptr;
    MarkerRole role = MarkerRole::Destination;
    MarkerScope scope = MarkerScope::Target;
    uint32_t tags = TagNone;
    int32_t screenX = 0;
    int32_t screenY = 0;
    bool isOffer = false;
    std::wstring label;
    std::wstring locationName;
    std::wstring markerName;
    std::wstring markerHint;
    MarkerAnchorKind anchorKind = MarkerAnchorKind::DefaultEntity;
    bool touchBoxAnchorValid = false;
    int32_t touchBoxLeftWorld = 0;
    int32_t touchBoxTopWorld = 0;
    int32_t touchBoxWidth = 0;
    int32_t touchBoxHeight = 0;
    int32_t touchBoxScreenX = 0;
    int32_t touchBoxScreenY = 0;

    // dev6b 诊断字段。保存最终 Marker 对应的实体身份，MaybeLogQuestResolution() 才能在状态变化时
    // 输出“Quest/Role/Event/Name/WorldCamera/ControllerCamera/最终坐标”，不再只有一个 MarkerCount。
    bool fromEntity = false;
    size_t entityIndex = 0;
    std::wstring entityName;
    uint8_t touchEventId = 0;
    uint8_t manualEventId = 0;
    bool worldAnchorValid = false;
    bool controllerAnchorValid = false;
    int32_t worldScreenX = 0;
    int32_t worldScreenY = 0;
    int32_t controllerScreenX = 0;
    int32_t controllerScreenY = 0;

    // dev6d Route 诊断。普通最终目标/Offer 保持 false；只有 BFS 产生的“当前地图第一跳”才填写。
    bool fromRoute = false;
    // routeId 是 当前 Quest TOML 的 [[stage.route]] 中的稳定研究边编号。
    // 它只用于诊断和维护定位：看到 R000842 就可以在当前 Stage 下搜索同名 [[stage.route]]，不参与寻路算法本身。
    std::string routeId;
    std::string routeNextScene;
    int routePathLength = 0;
    int routeEventId = -1;
};

static bool WideIEquals(const std::wstring& a, const std::wstring& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) if (towlower(a[i]) != towlower(b[i])) return false;
    return true;
}

static bool WideIContains(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty() || haystack.size() < needle.size()) return false;
    for (size_t i = 0; i + needle.size() <= haystack.size(); ++i) {
        bool same = true;
        for (size_t j = 0; j < needle.size(); ++j) {
            if (towlower(haystack[i + j]) != towlower(needle[j])) { same = false; break; }
        }
        if (same) return true;
    }
    return false;
}

static std::vector<ResolvedTargetPoint> ResolveTarget(const MarkerTarget& target, const GameSnapshot& snapshot) {
    std::vector<ResolvedTargetPoint> points;

    // world:x,y 仍保留给“人工已经验证过的纯世界点”。它不是实体定位的正式路径。
    if (target.type == TargetType::World) {
        ResolvedTargetPoint p;
        p.valid = true;
        p.anchorKind = MarkerAnchorKind::ExplicitWorld;
        p.screenX = target.x - snapshot.cameraX;
        p.screenY = target.y - snapshot.cameraY;
        points.push_back(p);
        return points;
    }

    for (const RuntimeEntityView& e : snapshot.entities) {
        bool match = false;
        switch (target.type) {
            case TargetType::EventAny: match = e.manualEventId == target.value || e.touchEventId == target.value; break;
            case TargetType::EventManual: match = e.manualEventId == target.value; break;
            case TargetType::EventTouch: match = e.touchEventId == target.value; break;
            case TargetType::EntityIndex: match = static_cast<int>(e.index) == target.value; break;
            case TargetType::EntityName: match = WideIEquals(e.name, target.name); break;
            case TargetType::EntityNameContainsAny:
                for (const std::wstring& alias : target.names) {
                    if (WideIContains(e.name, alias)) { match = true; break; }
                }
                break;
            default: break;
        }
        if (!match) continue;
        // dev6y：同场景最终目标与 Route 使用同一条物理锚点纪律。
        // 显式 EVENT_TOUCH 只能使用当前原版 Section0 TouchBox；TouchBox 无效时本帧不画，
        // 绝不能退回 RuntimeEntity 的普通 World/Controller 锚点。
        // EventAny 只有在“真的存在可用 Manual EB”时才允许实体锚点；仅命中 Touch EA 且 TouchBox 无效时同样不画。
        const bool matchedTouch = (target.type == TargetType::EventTouch) ||
                                  (target.type == TargetType::EventAny && e.touchEventId == target.value);
        const bool matchedManual = (target.type == TargetType::EventManual) ||
                                   (target.type == TargetType::EventAny && e.manualEventId == target.value);
        bool useTouchBox = false;
        if (target.type == TargetType::EventTouch) {
            if (!e.touchBoxAnchorValid) continue;
            useTouchBox = true;
        } else if (target.type == TargetType::EventManual) {
            if (!e.markerAnchorValid) continue;
        } else if (target.type == TargetType::EventAny) {
            if (matchedTouch && e.touchBoxAnchorValid) {
                useTouchBox = true;
            } else if (matchedManual && e.markerAnchorValid) {
                useTouchBox = false;
            } else {
                continue;
            }
        } else if (!e.markerAnchorValid) {
            continue;
        }
        ResolvedTargetPoint p;
        p.valid = true;
        p.fromEntity = true;
        p.entityIndex = e.index;
        p.anchorKind = useTouchBox ? MarkerAnchorKind::TouchCollisionBox : MarkerAnchorKind::DefaultEntity;
        p.screenX = useTouchBox ? e.touchBoxScreenX : e.screenX;
        p.screenY = useTouchBox ? e.touchBoxScreenY : e.screenY;
        p.entityName = e.name;
        p.touchEventId = e.touchEventId;
        p.manualEventId = e.manualEventId;
        p.worldAnchorValid = e.worldAnchorValid;
        p.controllerAnchorValid = e.renderAnchorValid;
        p.worldScreenX = e.worldScreenX;
        p.worldScreenY = e.worldScreenY;
        p.controllerScreenX = e.controllerScreenX;
        p.controllerScreenY = e.controllerScreenY;
        p.touchBoxAnchorValid = e.touchBoxAnchorValid;
        p.touchBoxLeftWorld = e.touchBoxLeftWorld;
        p.touchBoxTopWorld = e.touchBoxTopWorld;
        p.touchBoxWidth = e.touchBoxWidth;
        p.touchBoxHeight = e.touchBoxHeight;
        p.touchBoxScreenX = e.touchBoxScreenX;
        p.touchBoxScreenY = e.touchBoxScreenY;
        points.push_back(p);
    }
    return points;
}

static bool MarkerMatchesSceneAndCondition(const MarkerDefinition& marker, const GameSnapshot& snapshot) {
    if (!marker.sceneId.empty() && !IEqualsAscii(marker.sceneId, snapshot.sceneId)) return false;
    bool ok = false;
    const bool cond = EvaluateCondition(marker.when, snapshot, &ok);
    return ok && cond;
}

// 只检查 Marker 自己的 when，不检查 scene。
// 跨场景导航要先知道“这个 Stage 最终想去哪张地图”，因此不能因为目标 Scene 不是当前 Scene 就把它过滤掉。
static bool MarkerConditionMatches(const MarkerDefinition& marker, const GameSnapshot& snapshot) {
    bool ok = false;
    const bool cond = EvaluateCondition(marker.when, snapshot, &ok);
    return ok && cond;
}

// 从当前 Stage 的正式 Marker 中找“最终目标 Scene”。
// 当前数据库每个激活 Stage 通常只有一个满足条件的最终目标；若未来出现条件分支，则谁的 when 当前成立就选谁。
// 返回 false 表示这个 Stage 没有可用于跨场景导航的目标，例如 terminal 阶段；这种情况宁可不画也不猜。
static bool FindStageGoalScene(const QuestStage& stage, const GameSnapshot& snapshot, std::string& goalScene) {
    goalScene.clear();
    for (const MarkerDefinition& marker : stage.routeMarkers) {
        if (marker.sceneId.empty()) continue;
        if (!MarkerConditionMatches(marker, snapshot)) continue;
        goalScene = ToLowerAscii(marker.sceneId);
        return !goalScene.empty();
    }
    return false;
}

// 对当前原版状态做一次条件 BFS。
//
// dev6t 重要编译器兼容修正：
// MSVC 14.51.36231 在旧版这个函数内部同时优化 std::set、std::map、局部 PreviousStep 和 BFS 时，
// 会触发编译器自身 C1001。这里不再让主翻译单元承担复杂 BFS 模板代码。
// 主程序只做自己最擅长、也必须留在这里做的事情：读取当前 GameSnapshot，判断每条 Route 的 when 是否成立。
// 条件成立的安全边被压缩成 RouteSearchInputEdge，再交给独立 RouteSearch.cpp 做纯 BFS。
// dev6t 为规避 MSVC 14.51 在主翻译单元 STL 优化阶段触发的 C1001，CastleQuest.cpp 与 RouteSearch.cpp 都使用 /Od。当前优先保证稳定编译；后续只有在新的 MSVC 实机证据确认安全后才考虑恢复局部优化。
static RouteSearchResult FindSafeRoute(const GameSnapshot& snapshot,
                                       const std::vector<RouteEdge>& stageRoutes,
                                       const std::string& fromScene,
                                       const std::string& goalScene) {
    RouteSearchResult result;
    if (fromScene.empty() || goalScene.empty() || IEqualsAscii(fromScene, goalScene)) return result;
    if (stageRoutes.empty()) return result;

    std::vector<RouteSearchInputEdge> enabledEdges;
    enabledEdges.reserve(stageRoutes.size());

    for (size_t edgeIndex = 0; edgeIndex < stageRoutes.size(); ++edgeIndex) {
        const RouteEdge& edge = stageRoutes[edgeIndex];

        // runtimeOnly 表示结果只能在某个瞬时事件执行期间才能知道。
        // 任务导航不能预言这种瞬时结果，所以和旧版一样坚决排除。
        if (!edge.advanceSafe || edge.runtimeOnly) continue;
        // dev6w：自动跨场景导航必须有“原版物理事件绑定”或明确人工增补。
        // 旧冻结 Event/World 继续保留作研究证据，但不再凭猜测把玩家引向一个并不存在的触发点。
        if (!RouteHasUsableNavigationBinding(edge)) continue;

        bool conditionOk = false;
        const bool enabledNow = EvaluateCondition(edge.when, snapshot, &conditionOk);
        if (!conditionOk || !enabledNow) continue;

        RouteSearchInputEdge input;
        input.fromScene = edge.fromScene;
        input.toScene = edge.toScene;
        input.originalEdgeIndex = edgeIndex;
        enabledEdges.push_back(input);
    }

    return FindRouteBfsUnoptimized(enabledEdges, fromScene, goalScene);
}

// dev6e：Route 边不再只返回“第一个匹配出口”。
//
// dev6d 的单点策略在“同一个 RouteEvent 有两个物理出入口”时会天然漏掉一个 Marker，
// 而用户实机已经确认这种地图结构真实存在。因此 dev6e 把 Route 绑定结果改成“0..N 个点”：
// - BFS 仍然只决定“下一张 Scene / 第一条 RouteEdge”；
// - 物理绑定阶段枚举当前 Scene 里所有匹配该物理 Event 的 RuntimeEntity；
// - 同一个 Event 对应两个门，就画两个门；不会为了凑一个“唯一答案”丢掉原版可走出口。
//
// 另外，世界地图 mp3001 的主线出口还有第二个独立问题：RuntimeEntity World 点不是玩家真正碰到
// EA 的触发区位置。原版 0x4080D0 明确使用 SF2 Section 0 的碰撞矩形，因此 mp3001 Route Exit
// 优先使用该矩形中心作为 Marker 锚点。这是“跟随原版触发判定”，不是固定加减像素的截图补丁。
static bool CopyEntityToResolvedPoint(const RuntimeEntityView& entity,
                                      bool preferTouchBox,
                                      ResolvedTargetPoint& out) {
    if (!entity.markerAnchorValid && !(preferTouchBox && entity.touchBoxAnchorValid)) return false;

    out = ResolvedTargetPoint();
    out.valid = true;
    out.fromEntity = true;
    out.entityIndex = entity.index;
    out.entityName = entity.name;
    out.touchEventId = entity.touchEventId;
    out.manualEventId = entity.manualEventId;

    // 这些并行坐标全部保留给日志，便于继续验证触发区中心、runtime World 和 Controller 根点之间的关系。
    out.worldAnchorValid = entity.worldAnchorValid;
    out.controllerAnchorValid = entity.renderAnchorValid;
    out.worldScreenX = entity.worldScreenX;
    out.worldScreenY = entity.worldScreenY;
    out.controllerScreenX = entity.controllerScreenX;
    out.controllerScreenY = entity.controllerScreenY;
    out.touchBoxAnchorValid = entity.touchBoxAnchorValid;
    out.touchBoxLeftWorld = entity.touchBoxLeftWorld;
    out.touchBoxTopWorld = entity.touchBoxTopWorld;
    out.touchBoxWidth = entity.touchBoxWidth;
    out.touchBoxHeight = entity.touchBoxHeight;
    out.touchBoxScreenX = entity.touchBoxScreenX;
    out.touchBoxScreenY = entity.touchBoxScreenY;

    if (preferTouchBox && entity.touchBoxAnchorValid) {
        out.anchorKind = MarkerAnchorKind::TouchCollisionBox;
        out.screenX = entity.touchBoxScreenX;
        out.screenY = entity.touchBoxScreenY;
        return true;
    }

    // 普通场景继续沿用 dev6b/dev6d 已通过实机验证的默认实体投影。
    // 这样“夏侯仪家 -> 高老丈 -> 河州镇出口”这段已经正确的路线不会因为世界地图修复而被一起改动。
    if (!entity.markerAnchorValid) return false;
    out.anchorKind = MarkerAnchorKind::DefaultEntity;
    out.screenX = entity.screenX;
    out.screenY = entity.screenY;
    return true;
}

// 收集一个物理 Event 对应的全部实体。
// target_object 只用于“把精确命中的对象排在前面”，不再作为过滤条件。
// 原因是冻结 Route 里的对象名在部分分发链上已经被实机证明会滞后一层；如果继续把对象名当硬过滤，
// 同 Event 的第二个合法门口永远没有机会显示。
static std::vector<const RuntimeEntityView*> FindEntitiesForRouteEvent(const RouteEdge& edge,
                                                                       const GameSnapshot& snapshot,
                                                                       int physicalEventId,
                                                                       bool preferFrozenObjectName) {
    std::vector<const RuntimeEntityView*> exactNameMatches;
    std::vector<const RuntimeEntityView*> otherMatches;

    const TargetType effectiveTriggerType = EffectiveRouteTriggerType(edge);
    const std::wstring& effectiveTargetObject = EffectiveRouteTargetObject(edge);
    for (const RuntimeEntityView& entity : snapshot.entities) {
        bool eventMatch = false;
        if (effectiveTriggerType == TargetType::EventManual) {
            eventMatch = entity.manualEventId == physicalEventId;
        } else if (effectiveTriggerType == TargetType::EventTouch) {
            eventMatch = entity.touchEventId == physicalEventId;
        } else {
            eventMatch = entity.manualEventId == physicalEventId || entity.touchEventId == physicalEventId;
        }
        if (!eventMatch) continue;

        // 普通实体锚点和 TouchBox 至少有一种可用即可；世界地图后面会优先使用 TouchBox。
        if (!entity.markerAnchorValid && !entity.touchBoxAnchorValid) continue;

        if (preferFrozenObjectName && !effectiveTargetObject.empty() && WideIEquals(entity.name, effectiveTargetObject)) {
            exactNameMatches.push_back(&entity);
        } else {
            otherMatches.push_back(&entity);
        }
    }

    exactNameMatches.insert(exactNameMatches.end(), otherMatches.begin(), otherMatches.end());
    return exactNameMatches;
}

static void AppendResolvedEntityPoints(const std::vector<const RuntimeEntityView*>& entities,
                                       bool requireTouchBox,
                                       std::vector<ResolvedTargetPoint>& out) {
    for (const RuntimeEntityView* entity : entities) {
        if (!entity) continue;
        // dev6y：只要调用方明确说这是 TOUCH_EA，就必须真的存在 TouchBox。
        // “优先 TouchBox，失败再退实体”会把地点触发区重新错误画到对象锚点，因此这里改成硬门控。
        if (requireTouchBox && !entity->touchBoxAnchorValid) continue;
        ResolvedTargetPoint point;
        if (!CopyEntityToResolvedPoint(*entity, requireTouchBox, point) || !point.valid) continue;
        if (requireTouchBox && point.anchorKind != MarkerAnchorKind::TouchCollisionBox) continue;
        out.push_back(point);
    }
}

static std::vector<ResolvedTargetPoint> ResolveRouteEdgePoints(const RouteEdge& edge,
                                                               const GameSnapshot& snapshot) {
    std::vector<ResolvedTargetPoint> out;

    // ---------------------------------------------------------------------
    // 最高优先级：人工世界坐标。
    // ---------------------------------------------------------------------
    // 这是维护者在实机上明确确认过的 Marker 点，dev6w 完全保留原有语义，不让自动适配覆盖它。
    if (edge.manualWorldValid) {
        ResolvedTargetPoint point;
        point.valid = true;
        point.anchorKind = MarkerAnchorKind::ExplicitWorld;
        point.screenX = edge.manualWorldX - snapshot.cameraX;
        point.screenY = edge.manualWorldY - snapshot.cameraY;
        point.worldAnchorValid = true;
        point.worldScreenX = point.screenX;
        point.worldScreenY = point.screenY;
        out.push_back(point);
        return out;
    }

    // ---------------------------------------------------------------------
    // 第二优先级：人工 Event / 触发类型 / 对象名。
    // ---------------------------------------------------------------------
    // 人工 Event 是维护者明确指定的原版物理目标，因此绝不再被 native_binding 反向覆盖。
    if (edge.manualEventIdSet) {
        const int physicalEventId = edge.manualEventId;
        const TargetType physicalType = EffectiveRouteTriggerType(edge);
        const bool preferTouchBox = physicalType == TargetType::EventTouch;
        const std::vector<const RuntimeEntityView*> matches =
            FindEntitiesForRouteEvent(edge, snapshot, physicalEventId, true);
        if (!matches.empty()) {
            AppendResolvedEntityPoints(matches, preferTouchBox, out);
            return out;
        }
        // 明确人工绑定尚未出现在 RuntimeEntity 时宁可本帧不画，不偷偷退回另一套自动事件。
        return out;
    }

    // ---------------------------------------------------------------------
    // 第三优先级：完整游戏导出的原版事件空间绑定。
    // ---------------------------------------------------------------------
    // dev6w 把这里收紧为“当前运行时真实存在才允许画”：
    //
    // 1. 自动 TOUCH_EA：
    //    - 当前场景必须真的存在 EA 相同的 RuntimeEntity；
    //    - 这个实体还必须成功解析出 Section0 TouchBox；
    //    - Marker 只能落在这个正在生效的 TouchBox 上。
    //
    // 2. 自动 MANUAL_EB / NPC：
    //    - 当前场景必须真的存在 EB 相同的 RuntimeEntity；
    //    - Marker 跟随这个运行时实体；
    //    - NPC 移动时，Marker 自然跟着当前实体位置移动。
    //
    // 3. 完整游戏导出的 binding.worldX/worldY：
    //    - 继续保留在当前 [[stage.route]] 中作为“绑定证据”和 F11 的 A= 自动坐标证据；
    //    - 绝不再在 RuntimeEntity 尚未出现、已经消失、或 TouchBox 无效时直接拿来画 Marker。
    //
    // 这条门控专门解决 dev6w 实机看到的“空地上凭空出现自动 Marker”：
    // 自动 Marker 必须依附当前场景真实事件；只有维护者明确写的 manual_world 才允许脱离原版事件独立存在。
    for (const RouteNativeBinding& binding : edge.nativeBindings) {
        RouteEdge probe = edge;
        probe.manualEventIdSet = false;
        probe.manualTriggerTypeSet = false;
        probe.manualTargetObjectSet = false;
        probe.eventId = binding.eventId;
        probe.eventTargetType = binding.triggerType;
        probe.targetObject = binding.targetObject;

        const std::vector<const RuntimeEntityView*> matches =
            FindEntitiesForRouteEvent(probe, snapshot, binding.eventId, true);

        for (const RuntimeEntityView* entity : matches) {
            if (!entity) continue;

            if (binding.triggerType == TargetType::EventTouch) {
                // TOUCH_EA 的“原版判定点”就是 TouchBox。
                // 即使同 EA 的实体存在，只要当前帧没有有效 TouchBox，也绝不能退回 Entity World，
                // 否则又会重新出现“看起来附近没有触发点，但 Marker 却在那里”的错误。
                if (!entity->touchBoxAnchorValid) continue;

                ResolvedTargetPoint point;
                if (!CopyEntityToResolvedPoint(*entity, true, point) || !point.valid) continue;
                if (point.anchorKind != MarkerAnchorKind::TouchCollisionBox) continue;
                out.push_back(point);
                continue;
            }

            if (binding.triggerType == TargetType::EventManual) {
                // MANUAL_EB / NPC 使用当前实体自身的位置。
                // 这里要求 markerAnchorValid，是为了避免只有残缺 controller/碰撞信息的对象被错误当成可追踪人物。
                if (!entity->markerAnchorValid) continue;

                ResolvedTargetPoint point;
                if (!CopyEntityToResolvedPoint(*entity, false, point) || !point.valid) continue;
                out.push_back(point);
            }
        }
    }
    if (!out.empty()) return out;

    // dev6w：自动原版事件在当前运行时没有真实锚点时，本帧就不画。
    // 旧 world_x/world_y 与 native binding 的离线 worldX/worldY 都只保留为研究/诊断证据，
    // 不再充当运行时坐标替身。人工 manual_world 仍在函数最前面保持最高优先级。
    return out;
}

// 把 ResolvedTargetPoint 复制成真正要绘制的 ResolvedMarker。
// 单独做成函数的意义只是避免“最终目标 Marker”和“Route 第一跳 Marker”各自复制一大段字段，未来改诊断字段时漏一边。
static ResolvedMarker MakeResolvedMarker(const QuestDefinition& quest,
                                         MarkerRole role,
                                         bool isOffer,
                                         const ResolvedTargetPoint& point,
                                         const MarkerDefinition* definition = nullptr) {
    ResolvedMarker marker;
    marker.quest = &quest;
    marker.role = role;
    marker.tags = quest.tags;
    marker.screenX = point.screenX;
    marker.screenY = point.screenY;
    marker.isOffer = isOffer;
    marker.label = quest.title;
    if (definition) {
        marker.scope = definition->scope;
        marker.locationName = definition->locationName;
        marker.markerName = definition->markerName;
        marker.markerHint = definition->markerHint;
    }
    marker.anchorKind = point.anchorKind;
    marker.touchBoxAnchorValid = point.touchBoxAnchorValid;
    marker.touchBoxLeftWorld = point.touchBoxLeftWorld;
    marker.touchBoxTopWorld = point.touchBoxTopWorld;
    marker.touchBoxWidth = point.touchBoxWidth;
    marker.touchBoxHeight = point.touchBoxHeight;
    marker.touchBoxScreenX = point.touchBoxScreenX;
    marker.touchBoxScreenY = point.touchBoxScreenY;
    marker.fromEntity = point.fromEntity;
    marker.entityIndex = point.entityIndex;
    marker.entityName = point.entityName;
    marker.touchEventId = point.touchEventId;
    marker.manualEventId = point.manualEventId;
    marker.worldAnchorValid = point.worldAnchorValid;
    marker.controllerAnchorValid = point.controllerAnchorValid;
    marker.worldScreenX = point.worldScreenX;
    marker.worldScreenY = point.worldScreenY;
    marker.controllerScreenX = point.controllerScreenX;
    marker.controllerScreenY = point.controllerScreenY;
    return marker;
}

// 诊断 HUD 只显示主线导航摘要，不参与任务状态判断，也不会写入原版状态。
//
// dev6t 将纯 BFS 拆到 RouteSearch.cpp 时，误把这个诊断结构体及其全局实例一并删掉，
// 但 BuildResolvedMarkers() 和诊断 HUD 仍然继续引用它，于是 MSVC 正确报告 C2065/C3861。
// dev6t 只恢复这段原有定义，不改变 RouteSearch.cpp 拆分和所有任务跨场景追踪逻辑。
struct PrimaryNavigationDiagnostic {
    bool active = false;
    std::string stageId;
    std::string goalScene;
    std::string status = "-";
    std::string nextScene;
    // 当前 BFS 第一跳对应的稳定 Route 编号；最终目标在当前场景时保持空字符串。
    std::string routeId;
    int routeEventId = -1;
    int pathLength = 0;
    size_t bindCount = 0;
    MarkerRole markerRole = MarkerRole::Destination;
};

static PrimaryNavigationDiagnostic g_primaryNavigationDiagnostic;

// Route 决策日志只在“同一任务的场景/Stage/结果”变化时写一次。
//
// dev6p 曾为了给多任务分别去重，直接在这个高频函数里新增
// std::map<std::string, std::string>。逻辑上没有问题，但用户使用同一套
// MSVC 14.51.36231 编译 dev6p 时，编译器重新在 STL/xlocale 附近触发 C1001。
// dev6o 已经在同一机器成功编译，因此 dev6q 不回退跨场景支线功能，
// 只把这一个新增的红黑树模板改成最多 25 项的简单线性表。
//
// 任务总数当前固定只有 25 条；每次写 Route 日志时最多线性检查 25 项，
// 这点开销远小于日志本身，同时能减少 MSVC 需要实例化的复杂 STL 模板。
struct RouteLogPreviousKey {
    std::string questId;
    std::string key;
};

static void MaybeLogRouteDecision(const QuestDefinition& quest,
                                  const QuestStage& stage,
                                  const GameSnapshot& snapshot,
                                  const std::string& goalScene,
                                  const char* status,
                                  const RouteEdge* edge,
                                  int pathLength,
                                  size_t bindCount) {
    static std::vector<RouteLogPreviousKey> previousKeys;
    std::ostringstream key;
    key << quest.id << '|' << stage.id << '|' << snapshot.sceneId << '|' << goalScene << '|'
        << (status ? status : "-") << '|'
        << (edge ? edge->id : "-") << '|'
        << (edge ? edge->toScene : "-") << '|'
        << (edge ? EffectiveRouteEventId(*edge) : -1) << '|' << pathLength << '|' << bindCount;

    const std::string currentKey = key.str();
    RouteLogPreviousKey* previous = nullptr;
    for (RouteLogPreviousKey& item : previousKeys) {
        if (item.questId == quest.id) {
            previous = &item;
            break;
        }
    }
    if (previous) {
        if (currentKey == previous->key) return;
        previous->key = currentKey;
    } else {
        RouteLogPreviousKey item;
        item.questId = quest.id;
        item.key = currentKey;
        previousKeys.push_back(std::move(item));
    }

    Log("[Route解析] QuestNo=%s Quest=%s Stage=%s From=%s Goal=%s Status=%s RouteId=%s Next=%s RouteEvent=%d AutoEvent=%d ManualEvent=%d PathLen=%d Bind=%u FromName=%s ToName=%s Marker=%s Scope=%s Hint=%s Verify=%s",
        quest.questNo.c_str(), quest.id.c_str(), stage.id.c_str(), snapshot.sceneId.c_str(), goalScene.c_str(),
        status ? status : "-", edge ? edge->id.c_str() : "-", edge ? edge->toScene.c_str() : "-",
        edge ? EffectiveRouteEventId(*edge) : -1, edge ? edge->eventId : -1,
        (edge && edge->manualEventIdSet) ? edge->manualEventId : -1, pathLength, static_cast<unsigned>(bindCount),
        edge ? WideToUtf8(edge->fromName).c_str() : "-", edge ? WideToUtf8(edge->toName).c_str() : "-",
        edge ? WideToUtf8(edge->markerName).c_str() : "-", edge ? MarkerScopeName(edge->markerScope) : "-",
        edge ? WideToUtf8(edge->routeHint).c_str() : "-", edge ? WideToUtf8(edge->verificationStatus).c_str() : "-");
}


static std::vector<ResolvedMarker> BuildResolvedMarkers(const GameSnapshot& snapshot,
                                                         const std::vector<EvaluatedQuest>& evaluated) {
    std::vector<ResolvedMarker> out;

    // 每帧都从空白诊断重新计算，绝不让上一张地图的 Route 状态残留到当前画面。
    g_primaryNavigationDiagnostic = PrimaryNavigationDiagnostic();

    for (const auto& qv : evaluated) {
        if (!qv.def) continue;
        const QuestDefinition& q = *qv.def;

        if (qv.state == QuestState::Available) {
            for (const auto& marker : q.offerMarkers) {
                if (!MarkerMatchesSceneAndCondition(marker, snapshot)) continue;
                for (const ResolvedTargetPoint& point : ResolveTarget(marker.target, snapshot)) {
                    if (!point.valid) continue;
                    out.push_back(MakeResolvedMarker(q, MarkerRole::Offer, true, point, &marker));
                }
            }
        }

        if (qv.state != QuestState::Active || !qv.stage) continue;

        // 主线诊断只取真正的 Primary Quest。当前数据库只有一条主线，但这里仍按标志判断，
        // 避免将来任务库扩展后把普通支线误写进顶部导航状态栏。
        if (q.primary) {
            g_primaryNavigationDiagnostic.active = true;
            g_primaryNavigationDiagnostic.stageId = qv.stage->id;
            g_primaryNavigationDiagnostic.markerRole = MarkerRole::Destination;
        }

        // -----------------------------------------------------------------
        // 第一步：最终目标就在当前 Scene 时，永远优先画原来的 Stage Marker。
        // -----------------------------------------------------------------
        bool stageHasCurrentSceneGoal = false;
        bool directMarkerResolved = false;
        size_t directMarkerCount = 0;
        MarkerRole firstDirectRole = MarkerRole::Destination;

        for (const MarkerDefinition& marker : qv.stage->routeMarkers) {
            if (!MarkerConditionMatches(marker, snapshot)) continue;
            if (!marker.sceneId.empty() && IEqualsAscii(marker.sceneId, snapshot.sceneId)) {
                stageHasCurrentSceneGoal = true;
            }
            if (!MarkerMatchesSceneAndCondition(marker, snapshot)) continue;

            for (const ResolvedTargetPoint& point : ResolveTarget(marker.target, snapshot)) {
                if (!point.valid) continue;
                out.push_back(MakeResolvedMarker(q, marker.role, false, point, &marker));
                if (!directMarkerResolved) firstDirectRole = marker.role;
                directMarkerResolved = true;
                ++directMarkerCount;
            }
        }

        if (q.primary) {
            std::string goalScene;
            if (FindStageGoalScene(*qv.stage, snapshot, goalScene)) {
                g_primaryNavigationDiagnostic.goalScene = goalScene;
            }

            if (stageHasCurrentSceneGoal || directMarkerResolved) {
                g_primaryNavigationDiagnostic.status = directMarkerResolved ? "Destination" : "DestinationUnbound";
                g_primaryNavigationDiagnostic.nextScene = "-";
                g_primaryNavigationDiagnostic.routeEventId = -1;
                g_primaryNavigationDiagnostic.pathLength = 0;
                g_primaryNavigationDiagnostic.bindCount = directMarkerCount;
                g_primaryNavigationDiagnostic.markerRole = firstDirectRole;
            }
        }

        // 已经在最终 Scene 时，即使 RuntimeEntity 暂时没捕获成功也绝不能启动 BFS 把 Marker 指离目标地图。
        if (stageHasCurrentSceneGoal || directMarkerResolved) continue;

        // -----------------------------------------------------------------
        // 第二步：最终目标在其他 Scene 时，只用当前 Stage 自己的局部 Route 寻找第一跳。
        // -----------------------------------------------------------------
        // dev6p 起这里不再区分主线和支线。
        // 只要任务已经进入正在追踪的 Active 状态，并且当前 Base/Addon Stage 能给出目标 Scene，
        // 就使用该 Stage TOML 内明确列出的局部条件 Route；不会扫描其他任务的路线。
        //
        // 这是现代任务系统的基本职责：
        // 1. 目标就在本 Scene -> 直接画最终目标；
        // 2. 目标在别的 Scene -> 画当前 Scene 通往目标的下一出口；
        // 3. 切图后重新求路，直到真正进入目标 Scene。
        //
        // 尚未接取、只处于 Available / Offer 的任务不会走到这里，所以不会产生跨场景追踪。
        std::string goalScene;
        if (!FindStageGoalScene(*qv.stage, snapshot, goalScene)) {
            if (q.primary) {
                g_primaryNavigationDiagnostic.status = "NoGoal";
                g_primaryNavigationDiagnostic.bindCount = 0;
            }
            continue;
        }
        if (q.primary) {
            g_primaryNavigationDiagnostic.goalScene = goalScene;
        }
        if (IEqualsAscii(goalScene, snapshot.sceneId)) {
            if (q.primary) {
                g_primaryNavigationDiagnostic.status = "DestinationUnbound";
                g_primaryNavigationDiagnostic.bindCount = 0;
            }
            continue;
        }

        const RouteSearchResult route = FindSafeRoute(snapshot, qv.stage->routes, snapshot.sceneId, goalScene);
        if (!route.found || route.firstEdgeIndex >= qv.stage->routes.size()) {
            if (q.primary) {
                g_primaryNavigationDiagnostic.status = "NoSafePath";
                g_primaryNavigationDiagnostic.bindCount = 0;
            }
            MaybeLogRouteDecision(q, *qv.stage, snapshot, goalScene, "NoSafePath", nullptr, 0, 0);
            continue;
        }

        const RouteEdge& edge = qv.stage->routes[route.firstEdgeIndex];
        if (q.primary) {
            g_primaryNavigationDiagnostic.nextScene = edge.toScene;
            g_primaryNavigationDiagnostic.routeId = edge.id;
            // 这里先只记录“这条 Route 已经选中”。真正用于 Marker 的物理 Event 可能来自 dev6w 原版绑定，
            // 与旧检测 event_id 不同，所以必须等 ResolveRouteEdgePoints() 得到实际物理点以后再填写 EventID。
            g_primaryNavigationDiagnostic.routeEventId = -1;
            g_primaryNavigationDiagnostic.pathLength = route.pathLength;
            g_primaryNavigationDiagnostic.markerRole = MarkerRole::Exit;
        }

        // Route 绑定返回 0..N 个物理入口。主线、支线、限时任务以及 Addon 插入步骤在这里共用同一套寻路处理。
        const std::vector<ResolvedTargetPoint> routePoints = ResolveRouteEdgePoints(edge, snapshot);
        if (routePoints.empty()) {
            if (q.primary) {
                g_primaryNavigationDiagnostic.status = "FirstEdgeUnbound";
                g_primaryNavigationDiagnostic.bindCount = 0;
            }
            MaybeLogRouteDecision(q, *qv.stage, snapshot, goalScene,
                                  "FirstEdgeUnbound", &edge, route.pathLength, 0);
            continue;
        }

        if (q.primary) {
            g_primaryNavigationDiagnostic.status = "Bound";
            g_primaryNavigationDiagnostic.bindCount = routePoints.size();
            // 顶部诊断显示本帧真正解析出的第一个原版物理 Event，而不是旧检测层 Event。
            const ResolvedTargetPoint& firstPoint = routePoints.front();
            if (firstPoint.touchEventId != 0) {
                g_primaryNavigationDiagnostic.routeEventId = static_cast<int>(firstPoint.touchEventId);
            } else if (firstPoint.manualEventId != 0) {
                g_primaryNavigationDiagnostic.routeEventId = static_cast<int>(firstPoint.manualEventId);
            } else {
                g_primaryNavigationDiagnostic.routeEventId = EffectiveRouteEventId(edge);
            }
        }
        MaybeLogRouteDecision(q, *qv.stage, snapshot, goalScene,
                              "Bound", &edge, route.pathLength, routePoints.size());

        for (const ResolvedTargetPoint& point : routePoints) {
            if (!point.valid) continue;
            ResolvedMarker routeMarker = MakeResolvedMarker(q, MarkerRole::Exit, false, point);
            routeMarker.scope = edge.markerScope;
            routeMarker.locationName = edge.toName;
            routeMarker.markerName = edge.markerName;
            routeMarker.markerHint = edge.routeHint;
            routeMarker.fromRoute = true;
            routeMarker.routeId = edge.id;
            routeMarker.routeNextScene = edge.toScene;
            routeMarker.routePathLength = route.pathLength;
            // dev6w 的 native binding 可能纠正旧冻结 Event（兰州 R001067 就是典型）。
            // 因此 Marker 自己保存的 RouteEvent 必须来自实际解析出的物理点，不能继续写旧检测 Event。
            if (point.touchEventId != 0) routeMarker.routeEventId = static_cast<int>(point.touchEventId);
            else if (point.manualEventId != 0) routeMarker.routeEventId = static_cast<int>(point.manualEventId);
            else routeMarker.routeEventId = EffectiveRouteEventId(edge);
            out.push_back(routeMarker);
        }
    }
    return out;
}

// 把“任务状态存在，但为什么画面没有 Marker”也变成日志可回答的问题。
// 这个函数只在任务状态或当前场景发生变化时写一次摘要，不会每帧刷屏。
// 例如阿铃支线处于 Available，但 source SCI 名称没有命中“阿鈴/阿铃/阿婷”，日志就会明确看到
// Quest=hezhou_colored_marble State=Available MarkerCount=0。用户无需再猜是任务条件错还是实体匹配错。
static const char* EntityMarkerProjectionName() {
    return g_entityMarkerProjection == EntityMarkerProjection::WorldCamera ? "WorldCamera" : "ControllerCamera";
}

static const char* MarkerRoleName(MarkerRole role) {
    switch (role) {
        case MarkerRole::Offer: return "Offer";
        case MarkerRole::Exit: return "Exit";
        case MarkerRole::Talk: return "Talk";
        case MarkerRole::Investigate: return "Investigate";
        case MarkerRole::Region: return "Region";
        case MarkerRole::Destination: return "Destination";
        default: return "Unknown";
    }
}

static void MaybeLogQuestResolution(const GameSnapshot& snapshot,
                                    const std::vector<EvaluatedQuest>& evaluated,
                                    const std::vector<ResolvedMarker>& markers) {
    static std::string previousKey;
    if (!snapshot.valid) return;

    std::ostringstream key;
    key << snapshot.sceneId << '|';
    for (const EvaluatedQuest& qv : evaluated) {
        if (!qv.def) continue;
        key << qv.def->id << '=' << static_cast<int>(qv.state) << ':' << (qv.stage ? qv.stage->id : "-") << ';';
    }
    key << "M=" << markers.size();
    for (const ResolvedMarker& marker : markers) {
        key << ':' << (marker.quest ? marker.quest->id : "-")
            << ':' << static_cast<int>(marker.role)
            << ':' << static_cast<int>(marker.scope)
            << ':' << WideToUtf8(marker.markerName)
            << ':' << marker.fromEntity
            << ':' << marker.entityIndex
            << ':' << static_cast<unsigned>(marker.touchEventId)
            << ':' << static_cast<unsigned>(marker.manualEventId)
            << ':' << WideToUtf8(marker.entityName)
            // dev6e：锚点种类属于“Marker 身份”的一部分。比如同一个 E88 从 Entity 改成
            // TouchBox，哪怕 Quest/Role/Event 都没变，也必须重新写一条日志让实机验证看见。
            << ':' << static_cast<int>(marker.anchorKind)
            << ':' << (marker.fromRoute ? marker.routeId : "-")
            << ':' << (marker.fromRoute ? marker.routeNextScene : "-")
            << ':' << marker.routePathLength
            << ':' << marker.routeEventId;
    }
    if (key.str() == previousKey) return;
    previousKey = key.str();

    Log("[任务解析] Scene=%s MarkerCount=%u", snapshot.sceneId.c_str(), static_cast<unsigned>(markers.size()));
    for (const EvaluatedQuest& qv : evaluated) {
        if (!qv.def) continue;
        unsigned ownMarkers = 0;
        for (const ResolvedMarker& marker : markers) {
            if (marker.quest == qv.def) ++ownMarkers;
        }
        const char* stateText = "Unavailable";
        switch (qv.state) {
            case QuestState::Available: stateText = "Available"; break;
            case QuestState::Active: stateText = "Active"; break;
            case QuestState::Completed: stateText = "Completed"; break;
            case QuestState::Failed: stateText = "Failed"; break;
            case QuestState::Expired: stateText = "Expired"; break;
            default: break;
        }
        Log("[任务解析] QuestNo=%s Quest=%s State=%s Addon=%d Stage=%s Tracked=%d Primary=%d MarkerCount=%u",
            qv.def->questNo.c_str(), qv.def->id.c_str(), stateText, qv.addonActive ? 1 : 0,
            qv.stage ? qv.stage->id.c_str() : "-", qv.def->tracked ? 1 : 0, qv.def->primary ? 1 : 0, ownMarkers);
    }

    // MarkerCount 只能回答“画了几个”，不能回答“画到谁身上”。
    // dev6e 进一步把 Anchor 和原版 Section 0 触发矩形一起记下来：世界地图如果仍然偏，下一份日志就能直接看见
    // “E88 的 RuntimeEntity 点在哪里、真正 TouchBox 在哪里、最终选了哪一个”，不再靠截图反推固定偏移。
    // Camera 滚动产生的最终屏幕坐标仍然不进入 previousKey，因此玩家走动不会让日志按帧刷屏。
    for (const ResolvedMarker& marker : markers) {
        Log("[Marker解析] QuestNo=%s Quest=%s Role=%s Scope=%s Marker=%s Anchor=%s E%u Name=%s EA=%02X EB=%02X Final=(%d,%d) "
            "WorldCamera=%s(%d,%d) ControllerCamera=%s(%d,%d) "
            "TouchBox=%s Center=(%d,%d) BoxWorld=(%d,%d,%d,%d) Projection=%s Generation=%u ManagerSerial=%ld "
            "Route=%d RouteId=%s NextScene=%s PathLen=%d RouteEvent=%d",
            marker.quest ? marker.quest->questNo.c_str() : "-", marker.quest ? marker.quest->id.c_str() : "-",
            MarkerRoleName(marker.role), MarkerScopeName(marker.scope), WideToUtf8(marker.markerName).c_str(), MarkerAnchorKindName(marker.anchorKind),
            static_cast<unsigned>(marker.entityIndex), WideToUtf8(marker.entityName).c_str(),
            static_cast<unsigned>(marker.touchEventId), static_cast<unsigned>(marker.manualEventId),
            marker.screenX, marker.screenY,
            marker.worldAnchorValid ? "OK" : "NO", marker.worldScreenX, marker.worldScreenY,
            marker.controllerAnchorValid ? "OK" : "NO", marker.controllerScreenX, marker.controllerScreenY,
            marker.touchBoxAnchorValid ? "OK" : "NO", marker.touchBoxScreenX, marker.touchBoxScreenY,
            marker.touchBoxLeftWorld, marker.touchBoxTopWorld, marker.touchBoxWidth, marker.touchBoxHeight,
            EntityMarkerProjectionName(), snapshot.sceneGeneration, static_cast<long>(snapshot.managerCaptureSerial),
            marker.fromRoute ? 1 : 0, marker.fromRoute ? marker.routeId.c_str() : "-",
            marker.fromRoute ? marker.routeNextScene.c_str() : "-", marker.routePathLength, marker.routeEventId);
    }
}

// ============================================================================
// 11. Overlay 软件像素后端：Lock / 写像素 / Unlock
// ============================================================================

// dev4 不再让 DirectDraw Surface 进入 GDI GetDC 路径。
// 下面这个 LockedCanvas 只描述“锁住以后的一块像素内存”：宽、高、每行字节数、像素位数和 RGB mask。
// 所有矩形、线、任务符号最后都只会变成对这块内存的普通写入。
struct LockedCanvas {
    uint8_t* pixels = nullptr;
    int width = 0;
    int height = 0;
    LONG pitch = 0;
    DWORD bitsPerPixel = 0;
    DWORD rMask = 0;
    DWORD gMask = 0;
    DWORD bMask = 0;
};

struct OverlayColor { uint8_t r, g, b; };

static int MaskShift(DWORD mask) {
    if (!mask) return 0;
    int shift = 0;
    while ((mask & 1u) == 0u) { mask >>= 1; ++shift; }
    return shift;
}

static int MaskBits(DWORD mask) {
    int bits = 0;
    while (mask) { bits += static_cast<int>(mask & 1u); mask >>= 1; }
    return bits;
}

static DWORD PackMaskedChannel(uint8_t value, DWORD mask) {
    if (!mask) return 0;
    const int shift = MaskShift(mask);
    const int bits = MaskBits(mask);
    const DWORD maxValue = bits >= 31 ? 0x7FFFFFFFu : ((1u << bits) - 1u);
    const DWORD scaled = (static_cast<DWORD>(value) * maxValue + 127u) / 255u;
    return (scaled << shift) & mask;
}

static DWORD PackColor(const LockedCanvas& c, OverlayColor color) {
    return PackMaskedChannel(color.r, c.rMask) |
           PackMaskedChannel(color.g, c.gMask) |
           PackMaskedChannel(color.b, c.bMask);
}

static void PutPixel(LockedCanvas& c, int x, int y, OverlayColor color) {
    if (!c.pixels || x < 0 || y < 0 || x >= c.width || y >= c.height) return;
    uint8_t* row = c.pitch >= 0 ? c.pixels + static_cast<ptrdiff_t>(y) * c.pitch
                                : c.pixels + static_cast<ptrdiff_t>(c.height - 1 - y) * (-c.pitch);
    const DWORD packed = PackColor(c, color);
    if (c.bitsPerPixel == 16) {
        *reinterpret_cast<uint16_t*>(row + x * 2) = static_cast<uint16_t>(packed);
    } else if (c.bitsPerPixel == 24) {
        uint8_t* p = row + x * 3;
        p[0] = static_cast<uint8_t>(packed & 0xFF);
        p[1] = static_cast<uint8_t>((packed >> 8) & 0xFF);
        p[2] = static_cast<uint8_t>((packed >> 16) & 0xFF);
    } else if (c.bitsPerPixel == 32) {
        *reinterpret_cast<uint32_t*>(row + x * 4) = packed;
    }
}

static void FillRectSoftware(LockedCanvas& c, int left, int top, int right, int bottom, OverlayColor color) {
    left = std::max(0, left); top = std::max(0, top);
    right = std::min(c.width, right); bottom = std::min(c.height, bottom);
    for (int y = top; y < bottom; ++y) for (int x = left; x < right; ++x) PutPixel(c, x, y, color);
}

static void DrawLineSoftware(LockedCanvas& c, int x0, int y0, int x1, int y1, OverlayColor color) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        PutPixel(c, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        const int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

static void DrawRectSoftware(LockedCanvas& c, int left, int top, int right, int bottom, OverlayColor color) {
    DrawLineSoftware(c, left, top, right, top, color);
    DrawLineSoftware(c, right, top, right, bottom, color);
    DrawLineSoftware(c, right, bottom, left, bottom, color);
    DrawLineSoftware(c, left, bottom, left, top, color);
}

// 中文文字仍然需要 Windows 字体，但我们只在“独立内存 DIB”里调用 GDI。
// 这与 dev1e/dev2 的问题有本质区别：这里绝不会 backSurface->GetDC()。
// GDI 只负责把一个字形变成一小块 RAM 像素，然后我们自己把非背景像素复制到 LockedCanvas。
static void DrawTextShadowSoftware(LockedCanvas& c, int x, int y, const std::wstring& text, OverlayColor color);

static int MeasureTextWidth(const std::wstring& text) {
    // 自动换行必须按“实际像素宽度”而不是“字符个数”判断。
    // 中文汉字、英文字母、数字、全角标点的宽度并不完全相同，尤其用户以后如果换字体，
    // 固定“每行20个字”会马上失真。这里使用与实际绘制完全相同的 g_font 做测量。
    if (text.empty() || !g_font) return 0;
    HDC dc=CreateCompatibleDC(nullptr);
    if(!dc) return static_cast<int>(text.size())*g_fontSize;
    HGDIOBJ old=SelectObject(dc,g_font);
    SIZE size={};
    const BOOL ok=GetTextExtentPoint32W(dc,text.c_str(),static_cast<int>(text.size()),&size);
    SelectObject(dc,old); DeleteDC(dc);
    return ok?static_cast<int>(size.cx):static_cast<int>(text.size())*g_fontSize;
}

static bool IsForbiddenLineStart(wchar_t ch) {
    // 这些中文/全角标点不应该孤零零出现在新行开头。这里只实现任务 HUD 需要的基本禁则，
    // 不是排版引擎；目标是避免最常见的“，。！？）」》跑到下一行第一格”。
    static const wchar_t* chars=L"，。！？；：、）》】』」〉…％%！？,.!?;:) ]}";
    return ch!=0 && std::wcschr(chars,ch)!=nullptr;
}

static bool IsGoodBreakAfter(wchar_t ch) {
    // 英文单词/数字尽量整体保留；中文本身可以按汉字边界换行。空格和常见标点是最优断点。
    return std::iswspace(ch) || std::wcschr(L"，。！？；：、,.;:!?/)-]}",ch)!=nullptr;
}

static std::vector<std::wstring> WrapTextPixels(const std::wstring& text,int maxWidth) {
    std::vector<std::wstring> lines;
    if(maxWidth<20){ lines.push_back(text); return lines; }
    size_t paragraphStart=0;
    while(paragraphStart<=text.size()) {
        size_t nl=text.find(L'\n',paragraphStart);
        std::wstring paragraph=text.substr(paragraphStart,nl==std::wstring::npos?std::wstring::npos:nl-paragraphStart);
        if(paragraph.empty()) lines.push_back(L"");
        size_t begin=0;
        while(begin<paragraph.size()) {
            size_t best=begin, preferred=std::wstring::npos;
            for(size_t end=begin+1; end<=paragraph.size(); ++end) {
                const std::wstring candidate=paragraph.substr(begin,end-begin);
                if(MeasureTextWidth(candidate)>maxWidth) break;
                best=end;
                if(IsGoodBreakAfter(paragraph[end-1])) preferred=end;
            }
            if(best==begin) best=begin+1;
            // dev6zc 修复：preferred 断点只在“后面还有文字但本行已经放不下”时才有意义。
            // 旧代码即使整段短句完全能放进一行，也会因为“· ”里的空格被记录成 preferred，
            // 最后强行切成“·”一行、正文下一行，造成用户看到的任务注释异常大间距。
            // best 已经到达段尾时，说明整段能放下，此时必须直接使用 best，绝不能主动截断。
            size_t cut=best;
            if(best<paragraph.size() && preferred!=std::wstring::npos && preferred>begin) cut=preferred;
            // 如果下一行会以禁则标点开始，而且当前行还有空间容纳这个标点，就把它一起带上。
            while(cut<paragraph.size() && IsForbiddenLineStart(paragraph[cut])) {
                const std::wstring withPunct=paragraph.substr(begin,cut-begin+1);
                if(MeasureTextWidth(withPunct)>maxWidth) break;
                ++cut;
            }
            std::wstring line=paragraph.substr(begin,cut-begin);
            while(!line.empty() && std::iswspace(line.back())) line.pop_back();
            while(cut<paragraph.size() && std::iswspace(paragraph[cut])) ++cut;
            lines.push_back(line);
            begin=cut;
        }
        if(nl==std::wstring::npos) break;
        paragraphStart=nl+1;
    }
    return lines;
}

static void DrawTextMemoryGdi(LockedCanvas& canvas, int x, int y, const std::wstring& text, OverlayColor color) {
    if (text.empty() || !g_font) return;
    HDC memDc = CreateCompatibleDC(nullptr);
    if (!memDc) return;
    HGDIOBJ oldFont = SelectObject(memDc, g_font);
    SIZE size = {};
    if (!GetTextExtentPoint32W(memDc, text.c_str(), static_cast<int>(text.size()), &size)) {
        SelectObject(memDc, oldFont); DeleteDC(memDc); return;
    }
    // SIZE::cx / SIZE::cy 的 Win32 类型是 LONG，而左边常量 1 是 int。
    // 新版 MSVC 对 std::max 的两个参数要求推导出完全相同的模板类型，
    // 因此直接写 std::max(1, size.cx + 4) 会形成 int 与 LONG 的类型冲突。
    // 这里先显式转换成 int，再做普通的下限判断。文字位图尺寸远小于 int 范围，
    // 所以这个转换在本插件的实际使用范围内是安全的，也避免依赖编译器的隐式推导规则。
    const int textWidth = static_cast<int>(size.cx);
    const int textHeight = static_cast<int>(size.cy);
    const int w = ((textWidth + 4) > 1) ? (textWidth + 4) : 1;
    const int h = ((textHeight + 4) > 1) ? (textHeight + 4) : 1;
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h; // 负高度表示 top-down，内存第0行就是屏幕上方。
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        if (dib) DeleteObject(dib);
        SelectObject(memDc, oldFont); DeleteDC(memDc); return;
    }
    HGDIOBJ oldBitmap = SelectObject(memDc, dib);

    // 选一个任务 UI 不会使用的 key 色。NONANTIALIASED_QUALITY 避免边缘与 key 色混合出紫边。
    const COLORREF key = RGB(1, 2, 3);
    HBRUSH keyBrush = CreateSolidBrush(key);
    RECT r = {0, 0, w, h};
    FillRect(memDc, &r, keyBrush);
    DeleteObject(keyBrush);
    SetBkMode(memDc, TRANSPARENT);
    SetTextColor(memDc, RGB(color.r, color.g, color.b));
    TextOutW(memDc, 2, 2, text.c_str(), static_cast<int>(text.size()));

    // BI_RGB 的 32 位 DIB 在小端内存里按 B,G,R,0 排列，作为 uint32 读取时数值是 0x00RRGGBB。
    // key=RGB(1,2,3) 因此对应数值 0x00010203；这里不能写成 COLORREF 的 0x00BBGGRR。
    const uint32_t keyPixel = (1u << 16) | (2u << 8) | 3u;
    const uint32_t* src = static_cast<const uint32_t*>(bits);
    for (int yy = 0; yy < h; ++yy) {
        for (int xx = 0; xx < w; ++xx) {
            const uint32_t px = src[static_cast<size_t>(yy) * w + xx] & 0x00FFFFFFu;
            if (px == keyPixel) continue;
            OverlayColor c {
                static_cast<uint8_t>((px >> 16) & 0xFF),
                static_cast<uint8_t>((px >> 8) & 0xFF),
                static_cast<uint8_t>(px & 0xFF)
            };
            PutPixel(canvas, x + xx, y + yy, c);
        }
    }

    SelectObject(memDc, oldBitmap);
    SelectObject(memDc, oldFont);
    DeleteObject(dib);
    DeleteDC(memDc);
}

static void DrawTextShadowSoftware(LockedCanvas& c, int x, int y, const std::wstring& text, OverlayColor color) {
    DrawTextMemoryGdi(c, x + 1, y + 1, text, {0, 0, 0});
    DrawTextMemoryGdi(c, x, y, text, color);
}

enum class QuestVisualKind {
    Main,
    Side,
    Timed,
    EndingTimed
};

// dev6p 的视觉分类继续按用户已经确认的规则：
// 1. 主线永远使用“不限时任务”主体，也就是菱形加感叹号；
// 2. 普通支线使用青色菱形加感叹号；
// 3. 限时/易错过支线使用橙色沙漏；
// 4. 同时限时且结局相关的支线使用红色沙漏；
// 5. “结局相关但不限时”仍然是普通支线视觉，不额外发明第五种类型。
static QuestVisualKind GetQuestVisualKind(const QuestDefinition* quest, uint32_t tags) {
    if (quest && (quest->primary || IEqualsAscii(quest->category, "Main"))) {
        return QuestVisualKind::Main;
    }
    const bool timed = (tags & (TagTimed | TagMissable)) != 0;
    const bool ending = (tags & TagEndingRelevant) != 0;
    if (timed && ending) return QuestVisualKind::EndingTimed;
    if (timed) return QuestVisualKind::Timed;
    return QuestVisualKind::Side;
}

static OverlayColor MarkerColorForKind(QuestVisualKind kind) {
    switch (kind) {
        case QuestVisualKind::Main:
            return {static_cast<uint8_t>(g_markerMainR), static_cast<uint8_t>(g_markerMainG), static_cast<uint8_t>(g_markerMainB)};
        case QuestVisualKind::Timed:
            return {static_cast<uint8_t>(g_markerTimedR), static_cast<uint8_t>(g_markerTimedG), static_cast<uint8_t>(g_markerTimedB)};
        case QuestVisualKind::EndingTimed:
            return {static_cast<uint8_t>(g_markerEndingTimedR), static_cast<uint8_t>(g_markerEndingTimedG), static_cast<uint8_t>(g_markerEndingTimedB)};
        case QuestVisualKind::Side:
        default:
            return {static_cast<uint8_t>(g_markerSideR), static_cast<uint8_t>(g_markerSideG), static_cast<uint8_t>(g_markerSideB)};
    }
}

static OverlayColor MarkerColor(const ResolvedMarker& marker) {
    return MarkerColorForKind(GetQuestVisualKind(marker.quest, marker.tags));
}

// 用中点圆算法画一个空心圆环。area 只是在任务主体 Marker 外增加这个范围环；
// target 与 entrance 都不增加额外图形，因为跨场景时 Marker 本来就会落到当前场景的出口。
static void DrawCircleSoftware(LockedCanvas& c, int cx, int cy, int radius, OverlayColor color) {
    if (radius <= 0) return;
    int x = radius;
    int y = 0;
    int error = 1 - x;
    while (x >= y) {
        PutPixel(c, cx + x, cy + y, color);
        PutPixel(c, cx + y, cy + x, color);
        PutPixel(c, cx - y, cy + x, color);
        PutPixel(c, cx - x, cy + y, color);
        PutPixel(c, cx - x, cy - y, color);
        PutPixel(c, cx - y, cy - x, color);
        PutPixel(c, cx + y, cy - x, color);
        PutPixel(c, cx + x, cy - y, color);
        ++y;
        if (error < 0) {
            error += 2 * y + 1;
        } else {
            --x;
            error += 2 * (y - x + 1);
        }
    }
}

// 不限时世界 Marker：正菱形 + 中间感叹号。
// 这里全部用软件线段与像素绘制，不依赖字体中的特殊 Unicode 图标字形。
static void DrawDiamondExclamationIcon(LockedCanvas& c, int x, int y, int size, OverlayColor color) {
    const int r = std::max(4, size / 2);
    DrawLineSoftware(c, x, y-r, x+r, y, color);
    DrawLineSoftware(c, x+r, y, x, y+r, color);
    DrawLineSoftware(c, x, y+r, x-r, y, color);
    DrawLineSoftware(c, x-r, y, x, y-r, color);

    // 感叹号自己用像素线画，避免字体差异导致世界 Marker 中心字符大小不一致。
    const int top = y - std::max(3, r / 2);
    const int bottom = y + std::max(1, r / 5);
    DrawLineSoftware(c, x, top, x, bottom, color);
    PutPixel(c, x, y + std::max(2, r / 2), color);
}

// 限时 Marker：固定像素模板沙漏。
//
// dev6o 的线框沙漏虽然增加了上下框、玻璃壁和沙堆，但实机仍然像漏斗/蝴蝶结，辨识度不够。
// dev6p 不再继续用几根斜线“拼”沙漏，而是使用一个固定 7x9 像素模板。
// 这种做法和老游戏里的像素 UI 图标更接近：
// - 小尺寸任务清单直接画 1 倍模板；
// - 世界 Marker 在 MarkerSize 足够大时画 2 倍模板；
// - 不依赖 Unicode 字体，所以不会因为字体、系统或字形缺失而变化。
//
// 模板中：
// '#' 表示要绘制的像素；
// '.' 表示透明位置。
// 上半部留空、下半部填出沙堆，使它看起来更像“正在流动的沙漏”，而不是单纯的 X。
static void DrawHourglassIcon(LockedCanvas& c, int x, int y, int size, OverlayColor color) {
    static const char* kHourglass[9] = {
        "#######",
        "#.....#",
        ".#...#.",
        "..#.#..",
        "...#...",
        "..###..",
        ".#####.",
        "#.....#",
        "#######"
    };

    // 任务清单一般只有约 8~12 像素可用，使用 1 倍模板。
    // 世界 Marker 默认 MarkerSize=14，使用 2 倍模板后得到 14x18 像素，远距离也更容易辨认。
    const int pixelScale = (size >= 14) ? 2 : 1;
    const int iconW = 7 * pixelScale;
    const int iconH = 9 * pixelScale;
    const int left = x - iconW / 2;
    const int top = y - iconH / 2;

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 7; ++col) {
            if (kHourglass[row][col] != '#') continue;

            // 一个模板像素可以对应 1x1 或 2x2 的实际像素块。
            // 逐像素填充虽然代码比画线多几行，但结果稳定、可预测，也更适合低分辨率游戏。
            const int px = left + col * pixelScale;
            const int py = top + row * pixelScale;
            for (int dy = 0; dy < pixelScale; ++dy) {
                for (int dx = 0; dx < pixelScale; ++dx) {
                    PutPixel(c, px + dx, py + dy, color);
                }
            }
        }
    }
}

static void DrawMarkerShapeSoftware(LockedCanvas& c, int x, int y, const ResolvedMarker& marker) {
    const QuestVisualKind kind = GetQuestVisualKind(marker.quest, marker.tags);
    const OverlayColor color = MarkerColorForKind(kind);

    if (kind == QuestVisualKind::Timed || kind == QuestVisualKind::EndingTimed) {
        DrawHourglassIcon(c, x, y, g_markerSize, color);
    } else {
        DrawDiamondExclamationIcon(c, x, y, g_markerSize, color);
    }

    // dev6p：只有 area 需要轻量的第二层提示。Offer 与 entrance 都直接沿用任务本身的主体 Marker。
    if (marker.scope == MarkerScope::Area) {
        DrawCircleSoftware(c, x, y, std::max(7, g_markerSize / 2 + 5), color);
    }
}

// 任务清单左侧图标和世界 Marker 共用同一视觉分类，但主线按用户要求继续保留原来的五角星。
// taskTitleColor 仍由原有 HUD/Ctrl+F7 代码决定；这里只画左边的小图标，不改变任务名称颜色。
static void DrawQuestListIconSoftware(LockedCanvas& c, int centerX, int centerY, const QuestDefinition& quest) {
    const QuestVisualKind kind = GetQuestVisualKind(&quest, quest.tags);
    const OverlayColor color = MarkerColorForKind(kind);
    const int iconSize = std::max(8, std::min(12, g_fontSize - 2));
    if (kind == QuestVisualKind::Main) {
        DrawTextShadowSoftware(c, centerX - 6, centerY - g_fontSize / 2 - 1, L"★", color);
    } else if (kind == QuestVisualKind::Timed || kind == QuestVisualKind::EndingTimed) {
        DrawHourglassIcon(c, centerX, centerY, iconSize, color);
    } else {
        DrawDiamondExclamationIcon(c, centerX, centerY, iconSize, color);
    }
}

static POINT ClampOffscreenPoint(int sx, int sy, int width, int height, bool& offscreen) {
    const int margin = 18;
    offscreen = sx < margin || sx >= width - margin || sy < margin || sy >= height - margin;
    if (!offscreen) return {sx, sy};
    const int cx = width / 2, cy = height / 2;
    const double dx = static_cast<double>(sx - cx), dy = static_cast<double>(sy - cy);
    if (std::abs(dx) < 0.001 && std::abs(dy) < 0.001) return {cx, cy};
    double scale = 1.0;
    if (dx > 0) scale = std::min(scale, (width - margin - cx) / dx);
    else if (dx < 0) scale = std::min(scale, (margin - cx) / dx);
    if (dy > 0) scale = std::min(scale, (height - margin - cy) / dy);
    else if (dy < 0) scale = std::min(scale, (margin - cy) / dy);
    return {static_cast<LONG>(cx + dx * scale), static_cast<LONG>(cy + dy * scale)};
}

static void DrawOffscreenDirectionSoftware(LockedCanvas& c, int x, int y, int targetSx, int targetSy, OverlayColor color) {
    const double dx0 = static_cast<double>(targetSx - x), dy0 = static_cast<double>(targetSy - y);
    const double len = std::sqrt(dx0*dx0 + dy0*dy0);
    if (len < 1.0) return;
    const double dx = dx0 / len, dy = dy0 / len;
    DrawLineSoftware(c, x - static_cast<int>(dx*7), y - static_cast<int>(dy*7),
                     x + static_cast<int>(dx*9), y + static_cast<int>(dy*9), color);
}

static const wchar_t* QuestStateText(QuestState state) {
    switch (state) {
        case QuestState::Available: return L"可触发";
        case QuestState::Active: return L"进行中";
        case QuestState::Completed: return L"已完成";
        case QuestState::Failed: return L"失败";
        case QuestState::Expired: return L"已错过";
        default: return L"未开放";
    }
}

// 每次场景实体集合变化时，把真实屏幕锚点写入共享日志。
// 这是 dev4 的“激进取证”设计：直接枚举真实场景对象，一版里同时推进功能和逆向。
static void MaybeDumpSceneEntities(const GameSnapshot& snapshot) {
    if (!snapshot.valid) return;
    std::ostringstream key;
    // dev6b 性能修复：这里绝对不能把 managerCaptureSerial 放进“对象集合是否变化”的键。
    // 0x409580 在探索场景中几乎每个逻辑更新都会执行，因此 serial 会不断递增。
    // dev6 把 serial 放进 key 后，哪怕对象一个都没变，也会被误判为“场景对象变化”，
    // 于是每个 Present 都把整张对象表逐行写日志。用户实机日志在一次测试里出现了 5 万多条
    // [场景对象]，日志膨胀到约 9.8MB，并且探索场景直接掉到个位数 FPS。
    // dev6d 的 key 只保留真正决定“对象集合身份”的字段；serial 仍然打印在日志正文里用于诊断，
    // 但不再触发重复 dump。这样对象表只会在 Scene / generation / count / valid 状态或对象指针变化时重写。
    key << snapshot.sceneId << ':' << snapshot.sceneGeneration << ':' << snapshot.entityCount << ':'
        << snapshot.sceneObjectListValid << ':' << snapshot.managerRejectedAsStale;
    for (const RuntimeEntityView& e : snapshot.entities) {
        // uint8_t* 在 iostream 里可能被当成“C 字符串地址”而读取指针后的内容。
        // 我们这里只需要把地址本身加入变化键，所以必须显式转成 const void*。
        key << ':' << static_cast<const void*>(e.source) << ':' << static_cast<const void*>(e.sf2Controller);
    }
    if (key.str() == g_lastEntityDumpKey) return;
    g_lastEntityDumpKey = key.str();

    Log("[场景对象] Scene=%s Generation=%u Manager=%s%s Count=%u FixedActors=%u Camera=(%d,%d) CaptureSerial=%ld",
        snapshot.sceneId.c_str(), snapshot.sceneGeneration, snapshot.sceneObjectListValid ? "OK" : "NO",
        snapshot.managerRejectedAsStale ? "(STALE_REJECTED)" : "", snapshot.entityCount, snapshot.fixedActorCount,
        snapshot.cameraX, snapshot.cameraY, static_cast<long>(snapshot.managerCaptureSerial));
    for (const RuntimeEntityView& e : snapshot.entities) {
        Log("[场景对象] E%u S~%d Name=%s World=(%d,%d) WorldScreen=%s(%d,%d) Render=(%d,%d) "
            "ControllerScreen=%s(%d,%d) TouchBox=%s Center=(%d,%d) BoxWorld=(%d,%d,%d,%d) "
            "Marker=%s(%d,%d) Mode=%s EA=%02X EB=%02X",
            static_cast<unsigned>(e.index), e.sourceOrdinal, WideToUtf8(e.name).c_str(),
            e.worldX, e.worldY, e.worldAnchorValid ? "OK" : "NO", e.worldScreenX, e.worldScreenY,
            e.renderBaseX, e.renderBaseY, e.renderAnchorValid ? "OK" : "NO", e.controllerScreenX, e.controllerScreenY,
            e.touchBoxAnchorValid ? "OK" : "NO", e.touchBoxScreenX, e.touchBoxScreenY,
            e.touchBoxLeftWorld, e.touchBoxTopWorld, e.touchBoxWidth, e.touchBoxHeight,
            e.markerAnchorValid ? "OK" : "NO", e.screenX, e.screenY, EntityMarkerProjectionName(),
            static_cast<unsigned>(e.touchEventId), static_cast<unsigned>(e.manualEventId));
    }
}

// 当一个 Event 真正变成 active 时，立刻把“哪个场景对象的 +EA/+EB 等于这个 Event”写进日志。
// 这相当于自动完成“玩家点了谁 -> 原版触发哪个 Event -> 该对象当前屏幕坐标在哪里”的第一轮反向绑定。
// 如果 Event 23 在 mp0101 能命中阿鈴对象，那么彩石弹珠 Offer Marker 就不需要任何 NPC 名字猜测。
static void LogEventObjectBinding(const GameSnapshot& snapshot, uint32_t eventId) {
    if (!snapshot.sceneObjectListValid || eventId == 0 || eventId > 255) return;
    unsigned matches = 0;
    for (const RuntimeEntityView& e : snapshot.entities) {
        const bool manual = e.manualEventId == eventId;
        const bool touch = e.touchEventId == eventId;
        if (!manual && !touch) continue;
        ++matches;
        Log("[Event绑定] Scene=%s Event=%u E%u S~%d Name=%s Kind=%s%s Final=(%d,%d) "
            "WorldCamera=%s(%d,%d) ControllerCamera=%s(%d,%d) Projection=%s Source=%p",
            snapshot.sceneId.c_str(), eventId, static_cast<unsigned>(e.index), e.sourceOrdinal,
            WideToUtf8(e.name).c_str(), manual ? "Manual" : "", touch ? (manual ? "+Touch" : "Touch") : "",
            e.screenX, e.screenY, e.worldAnchorValid ? "OK" : "NO", e.worldScreenX, e.worldScreenY,
            e.renderAnchorValid ? "OK" : "NO", e.controllerScreenX, e.controllerScreenY,
            EntityMarkerProjectionName(), static_cast<const void*>(e.source));
    }
    if (matches == 0) {
        Log("[Event绑定] Scene=%s Event=%u 当前场景对象表中没有找到 EA/EB 对应对象。", snapshot.sceneId.c_str(), eventId);
    }
}

// GameVar/Event/Battle 变化日志是 dev4 为真实河州支线准备的“随运行取证器”。
// 它没有 Hook EVE，也不写 GameVar；每帧只是把当前 5000 个 i16 和上一帧比较。
// 当用户真的去和阿铃连续对话、找阿平、触发阿吉巨蝎时，我们可以直接从日志看到哪些原版状态发生了跃迁。
static void MaybeLogGameStateChanges(const GameSnapshot& snapshot) {
    static bool initialized = false;
    static int16_t previousVars[WorldOffset::kGameVarCount] = {};
    static uint32_t previousEvent = 0;
    static uint32_t stickyEventForDiff = 0;
    static uint32_t previousBattleGroup = 0;
    static uint32_t previousBattleResult = 0;
    static std::string previousScene;
    if (!snapshot.valid || !snapshot.world) return;

    int16_t currentVars[WorldOffset::kGameVarCount] = {};
    bool allReadable = true;
    for (size_t i = 0; i < WorldOffset::kGameVarCount; ++i) {
        if (!SafeRead(reinterpret_cast<uintptr_t>(snapshot.world) + WorldOffset::kGameVars + i * 2, currentVars[i])) {
            allReadable = false;
            break;
        }
    }
    if (!allReadable) return;

    if (!initialized) {
        std::memcpy(previousVars, currentVars, sizeof(previousVars));
        previousEvent = snapshot.activeEventId;
        stickyEventForDiff = snapshot.activeEventId;
        previousBattleGroup = snapshot.battleLayoutGroup;
        previousBattleResult = snapshot.battleResult;
        previousScene = snapshot.sceneId;
        initialized = true;
        return;
    }

    // 新场景开始后不能把上一场景最后一次 Event 错套到下一张地图的变量变化上。
    if (snapshot.sceneId != previousScene) {
        stickyEventForDiff = snapshot.activeEventId;
        previousScene = snapshot.sceneId;
    }

    // Event 非零时先保存“最近真正执行的 Event”。原版经常在变量写入被我们下一帧观察到之前就把 0x89F808 清回 0，
    // dev3a 因而把阿鈴 Var4501 变化误记成 Event=0。dev4 用这个 sticky 值只补这一拍诊断语义。
    if (snapshot.activeEventId != 0) stickyEventForDiff = snapshot.activeEventId;
    const uint32_t eventForDiff = snapshot.activeEventId != 0 ? snapshot.activeEventId : stickyEventForDiff;

    // 先收集变化索引，再按规模决定日志粒度。
    // dev5 在读取存档/切换 GameVar bank 时会一次出现 162、251 项变化，旧代码把前64项全部刷出来，
    // 既淹没真正的剧情差异，也容易让人误以为“某个 Event 一次改了两百多个 Flag”。
    std::vector<size_t> changedIndices;
    changedIndices.reserve(32);
    for (size_t i = 0; i < WorldOffset::kGameVarCount; ++i) {
        if (previousVars[i] != currentVars[i]) changedIndices.push_back(i);
    }
    const int emitted = static_cast<int>(changedIndices.size());

    if (g_logGameVarChanges && emitted > 0) {
        if (emitted <= 16) {
            // 小规模变化最有研究价值：逐项完整写出，不丢任何 Flag。
            for (size_t i : changedIndices) {
                Log("[GameVar变化] Scene=%s Event=%u Var[%u]: %d -> %d",
                    snapshot.sceneId.c_str(), eventForDiff, static_cast<unsigned>(i),
                    static_cast<int>(previousVars[i]), static_cast<int>(currentVars[i]));
            }
        } else if (emitted <= 64) {
            // 中规模变化保留前16项样本，同时明确总数。这样仍能看出“发生了什么类型的批量变动”，
            // 但不会让几十行日志把后面的 Event/Marker 诊断挤走。
            for (size_t n = 0; n < 16; ++n) {
                const size_t i = changedIndices[n];
                Log("[GameVar变化] Scene=%s Event=%u Var[%u]: %d -> %d",
                    snapshot.sceneId.c_str(), eventForDiff, static_cast<unsigned>(i),
                    static_cast<int>(previousVars[i]), static_cast<int>(currentVars[i]));
            }
            Log("[GameVar批量变化] Scene=%s Event=%u 共%d项变化；上面仅列前16项。",
                snapshot.sceneId.c_str(), eventForDiff, emitted);
        } else {
            // 大于64项时，当前证据更符合“读档/初始化/整个 GameVar bank 替换”，而不是单一剧情脚本。
            // 这里故意使用“候选”措辞，不把尚未逆完的 bank 机制写成绝对事实。
            // 同时打印任务研究中最关键的一组变量的当前值，便于判断到底载入了哪类存档状态。
            static const int importantVars[] = {
                1, 25, 503, 504, 505, 515, 518, 522, 550, 551, 562, 651, 652,
                660, 663, 667, 668, 669, 1008, 1009, 1010, 1013, 1212, 1217, 1226, 1227, 4501
            };
            std::ostringstream summary;
            summary << "[GameVar初始化/Bank切换候选] Scene=" << snapshot.sceneId
                    << " Event=" << eventForDiff << " 变化数=" << emitted << " 重点=";
            for (size_t n = 0; n < sizeof(importantVars) / sizeof(importantVars[0]); ++n) {
                const int index = importantVars[n];
                if (n != 0) summary << ',';
                summary << "V" << index << '=' << static_cast<int>(currentVars[index]);
            }
            LogRaw(summary.str());
        }
    }

    if (snapshot.activeEventId != previousEvent) {
        Log("[Event变化] Scene=%s ActiveEvent: %u -> %u", snapshot.sceneId.c_str(), previousEvent, snapshot.activeEventId);
        if (snapshot.activeEventId != 0) LogEventObjectBinding(snapshot, snapshot.activeEventId);
    }
    if (snapshot.battleLayoutGroup != previousBattleGroup || snapshot.battleResult != previousBattleResult) {
        Log("[Battle状态] LayoutGroup=%u->%u Result=%u->%u%s",
            previousBattleGroup, snapshot.battleLayoutGroup, previousBattleResult, snapshot.battleResult,
            snapshot.battleLayoutGroup == 101 ? " [命中已确认的‘拯救阿吉’LayoutGR组101]" : "");
    }

    // 如果 Event 已经归零，而这一拍确实观察到了变量变化，就认为 sticky Event 的归属任务已经完成，避免长期误标后续无关变化。
    if (snapshot.activeEventId == 0 && emitted > 0) stickyEventForDiff = 0;

    std::memcpy(previousVars, currentVars, sizeof(previousVars));
    previousEvent = snapshot.activeEventId;
    previousBattleGroup = snapshot.battleLayoutGroup;
    previousBattleResult = snapshot.battleResult;
}

// dev6u：Ctrl+F11 是纯开发工具，所以这里优先服务“快速找世界坐标”，而不是面向玩家做复杂交互。
//
// 用户实机指出两个问题：
// 1. 旧 F11 把 E编号、来源序号、EA、EB、名称全部塞在一条长字符串里，640 宽画面很容易被截断；
// 2. 河州镇等对象很多的场景会同时出现几十条文字，真正想看的那个点反而被别的调试文字盖住。
//
// dev6u 的处理办法分成两层：
// - 平时：每个对象只画一个很短的标签；对象少时额外给一行 World/EA/EB，对象多时只保留 E编号；
// - 鼠标悬停：把鼠标附近最近的“任务 Marker 或实体调试点”放进一个固定三行详情框，并高亮目标点。
//
// F11 是开发阶段工具，所以这里明确只实现鼠标，不设计手柄焦点。它不会改变任务状态、场景对象或原版输入。
struct DebugHoverSelection {
    // mouseValid 只说明“成功取得鼠标在当前 DirectDraw Surface 上的位置”。
    // valid 则表示鼠标半径内确实命中了一个可展开的 Marker/实体。两个概念必须分开，
    // 否则“鼠标正常但附近没有对象”会被误画成 MouseWorld=(0,0)。
    bool mouseValid = false;
    bool valid = false;
    bool isMarker = false;
    size_t entityIndex = 0;
    size_t markerIndex = 0;
    int surfaceX = 0;
    int surfaceY = 0;
    int mouseSurfaceX = 0;
    int mouseSurfaceY = 0;
    int32_t mouseWorldX = 0;
    int32_t mouseWorldY = 0;

    // true 表示本帧直接使用原版 0x408830 已经计算好的游戏鼠标世界坐标。
    // false 只会出现在原版坐标暂时不可读、不得不使用 Win32 诊断回退时。
    bool mouseFromGameInput = false;
};

// F11 调试悬停需要的鼠标位置必须和“原版游戏自己认为鼠标在哪里”保持一致。
//
// dev6x 之前的错误路线是：再次调用 Win32 GetCursorPos -> ScreenToClient -> 按客户区比例缩放到 Surface。
// 这个办法在纯原版 640x480 下通常看不出问题，但 cnc-ddraw / 宽屏补丁可能已经对原版输入做了
// 坐标虚拟化；此时屏幕上绘制出来的游戏鼠标、原版交互判定使用的位置，与我们自己重新换算的
// Win32 坐标会分裂。用户实机截图已经直接证明这种分裂存在。
//
// 原版 0x00408830 恰好已经替我们完成了正确工作：它每帧取得鼠标，经过当前原版输入环境处理后，
// 再加上 Camera，并把最终“交互世界坐标”写到 0x0089F7C0/0x0089F7C4。dev6y 首选直接读取它们。
// 我们只把该世界点投影回 Quest 当前 Surface，用于算 18 像素悬停距离；绝不反过来修改原版鼠标。
static bool GetDebugMouseSurfacePoint(const LockedCanvas& c, const GameSnapshot& snapshot,
                                      const DisplayGeometry& geometry, int& sx, int& sy,
                                      int32_t& worldX, int32_t& worldY, bool& fromGameInput) {
    fromGameInput = false;

    // 重要：主路径这里完全不再读取 Win32 光标。
    // cnc-ddraw 或其他显示层可能把 Win32 客户区坐标和原版输入坐标放在不同的虚拟坐标系里；
    // 如果哪怕只拿 Win32 坐标做“是否在窗口内”的前置门控，都仍有可能把原版明明认为有效的鼠标误判掉。
    // 因此 dev6y 的主路径只相信原版自己已经算好的 MouseWorld。Win32 只存在于最下面的明确诊断回退。
    HWND hwnd = nullptr;

    int32_t gameWorldX = 0;
    int32_t gameWorldY = 0;
    if (SafeRead(Address::kMouseWorldX, gameWorldX) && SafeRead(Address::kMouseWorldY, gameWorldY)) {
        // 把“原版交互世界坐标”走和实体/Marker 一样的 World -> Surface 投影。
        // 这里先减 snapshot.camera 得到原版 640 逻辑坐标，再由 ProjectLogicalPointToSurface()
        // 处理 16:9/21:9 的 effectiveCameraX 与 sideWidth；因此没有任何硬编码 +107/+240 补丁。
        const int logicalX = gameWorldX - snapshot.cameraX;
        const int logicalY = gameWorldY - snapshot.cameraY;
        const POINT projected = ProjectLogicalPointToSurface(
            logicalX, logicalY, snapshot.cameraX, snapshot.cameraY, geometry);

        // 正常探索鼠标应该落在当前输出 Surface 附近。允许 32 像素小余量，是为了兼容原版在
        // 窗口边缘/裁剪前一帧可能短暂出现的轻微越界值；离谱值则进入下面的诊断回退。
        if (projected.x >= -32 && projected.y >= -32
            && projected.x < c.width + 32 && projected.y < c.height + 32) {
            worldX = gameWorldX;
            worldY = gameWorldY;
            sx = projected.x;
            sy = projected.y;
            fromGameInput = true;
            return true;
        }
    }

    // 诊断回退：只有原版 MouseWorld 暂时不可读/明显异常时才使用旧 Win32 路线。
    // 这条路线不再被称为“游戏鼠标坐标”，详情框会明确标成 Win32Fallback，方便实机发现异常。
    POINT cursor = {};
    if (!GetCursorPos(&cursor)) return false;
    if (!hwnd || !IsWindow(hwnd)) {
        if (!SafeRead(Address::kGameWindow, hwnd) || !hwnd || !IsWindow(hwnd)) hwnd = GetForegroundWindow();
    }
    if (!hwnd || !IsWindow(hwnd) || !ScreenToClient(hwnd, &cursor)) return false;

    RECT client = {};
    if (!GetClientRect(hwnd, &client)) return false;
    const int clientW = client.right - client.left;
    const int clientH = client.bottom - client.top;
    if (clientW <= 0 || clientH <= 0 || c.width <= 0 || c.height <= 0) return false;
    if (cursor.x < 0 || cursor.y < 0 || cursor.x >= clientW || cursor.y >= clientH) return false;

    sx = static_cast<int>((static_cast<long long>(cursor.x) * c.width) / clientW);
    sy = static_cast<int>((static_cast<long long>(cursor.y) * c.height) / clientH);
    sx = std::max(0, std::min(c.width - 1, sx));
    sy = std::max(0, std::min(c.height - 1, sy));
    worldX = sx + geometry.effectiveCameraX - geometry.sideWidth;
    worldY = sy + snapshot.cameraY;
    return true;
}

// 计算“这个 RuntimeEntity 的 F11 小方框画在 Surface 的哪里”。
// 这条链必须和正式 Marker 保持一致：World -> 640逻辑 -> 当前宽屏 Surface -> 用户配置 Offset。
static POINT RuntimeEntityDebugSurfacePoint(const RuntimeEntityView& e, const GameSnapshot& snapshot,
                                            const DisplayGeometry& geometry) {
    int logicalX = e.screenX;
    int logicalY = e.screenY;
    if (e.worldAnchorValid) {
        logicalX = e.worldX - snapshot.cameraX;
        logicalY = e.worldY - snapshot.cameraY;
    }
    POINT p = ProjectLogicalPointToSurface(
        logicalX, logicalY, snapshot.cameraX, snapshot.cameraY, geometry);
    p.x += g_markerOffsetX;
    p.y += g_markerYOffset + g_markerOffsetY;
    return p;
}

// 调试时按稳定 Route ID 从“这个 Marker 所属任务的当前 Stage”找回维护边。
// 每个 Stage 只有自己的少量 Route，因此线性查找非常小，也不会跨任务误命中同一个重复 R ID。
static const RouteEdge* FindRouteEdgeForDebug(const ResolvedMarker& marker,
                                                   const std::vector<EvaluatedQuest>& evaluated) {
    if (marker.routeId.empty() || !marker.quest) return nullptr;
    for (const EvaluatedQuest& qv : evaluated) {
        if (qv.def != marker.quest || !qv.stage) continue;
        for (const RouteEdge& edge : qv.stage->routes) {
            if (edge.id == marker.routeId) return &edge;
        }
    }
    return nullptr;
}

// Marker 只保存 QuestDefinition 指针。若需要显示当前 Stage 的自动/人工坐标，就从本帧已经完成的求值结果里找回它。
static const EvaluatedQuest* FindEvaluatedQuestForDebug(const QuestDefinition* quest,
                                                         const std::vector<EvaluatedQuest>& evaluated) {
    if (!quest) return nullptr;
    for (const EvaluatedQuest& qv : evaluated) {
        if (qv.def == quest) return &qv;
    }
    return nullptr;
}

// 把实体的事件绑定类型变成开发者一眼能读懂的短文本。
static std::wstring DebugKindText(const RuntimeEntityView& e) {
    if (e.manualEventId && e.touchEventId) return L"Manual+Touch";
    if (e.manualEventId) return L"Manual";
    if (e.touchEventId) return L"Touch";
    return L"None";
}

// 平时的实体标签刻意非常短。
// crowded=true 表示当前场景对象太多：这时只画 E编号；对象少时才额外画名称和第二行 World/EA/EB。
// focused=true 时颜色改成亮黄，告诉开发者“详情框正在解释的就是这个点”。
static void DrawRuntimeEntityDebugCompact(LockedCanvas& c, const RuntimeEntityView& e,
                                          const GameSnapshot& snapshot, const DisplayGeometry& geometry,
                                          bool focused, bool crowded) {
    if (!e.markerAnchorValid) return;
    const POINT p = RuntimeEntityDebugSurfacePoint(e, snapshot, geometry);
    const OverlayColor color = focused ? OverlayColor{255,245,120}
        : ((e.touchEventId || e.manualEventId) ? OverlayColor{80,220,255} : OverlayColor{150,150,150});
    DrawRectSoftware(c, p.x-4, p.y-4, p.x+4, p.y+4, color);

    wchar_t firstLine[96] = {};
    if (crowded) {
        _snwprintf_s(firstLine, _countof(firstLine), _TRUNCATE, L"E%u", static_cast<unsigned>(e.index));
    } else {
        // 名称最多取前 10 个字符，避免一个异常长对象名把下一组调试点整片盖住。
        std::wstring shortName = e.name;
        if (shortName.size() > 10) shortName = shortName.substr(0, 10) + L"…";
        _snwprintf_s(firstLine, _countof(firstLine), _TRUNCATE, L"E%u %s",
                     static_cast<unsigned>(e.index), shortName.c_str());
    }
    DrawTextShadowSoftware(c, p.x+6, p.y-8, firstLine, color);

    if (!crowded) {
        wchar_t secondLine[96] = {};
        _snwprintf_s(secondLine, _countof(secondLine), _TRUNCATE, L"W=(%d,%d) EA:%02X EB:%02X",
                     e.worldX, e.worldY,
                     static_cast<unsigned>(e.touchEventId), static_cast<unsigned>(e.manualEventId));
        DrawTextShadowSoftware(c, p.x+6, p.y-8 + std::max(14, g_fontSize+1), secondLine, {220,225,230});
    }
}

// 返回某个实体当前是否正被任务 Marker 使用。
// 同一实体理论上可能被多个任务同时指向；F11 详情框只展示第一条，并用 boundCount 告知还有多少条重叠绑定。
static const ResolvedMarker* FindMarkerBoundToEntityForDebug(const RuntimeEntityView& e,
                                                              const std::vector<ResolvedMarker>& markers,
                                                              size_t& boundCount) {
    boundCount = 0;
    const ResolvedMarker* first = nullptr;
    for (const ResolvedMarker& marker : markers) {
        if (!marker.fromEntity || marker.entityIndex != e.index) continue;
        ++boundCount;
        if (!first) first = &marker;
    }
    return first;
}

// 把任务/Route 的“自动坐标、人工覆写坐标、最终生效坐标”压缩成一行。
// 这正是人工修导航时最容易混淆的三层：Auto 是研究/自动检测证据，Override 是维护者手填值，Final 是本帧真的画在哪里。
static std::wstring BuildMarkerCoordinateSourceLine(const ResolvedMarker& marker,
                                                     const std::vector<EvaluatedQuest>& evaluated,
                                                     int32_t finalWorldX, int32_t finalWorldY) {
    bool autoValid = false;
    int autoX = 0, autoY = 0;
    bool overrideValid = false;
    int overrideX = 0, overrideY = 0;

    if (marker.fromRoute) {
        if (const RouteEdge* edge = FindRouteEdgeForDebug(marker, evaluated)) {
            // dev6w 起 A=“原版自动事件坐标”，不再默认显示旧检测 world_x/world_y。
            // 一条 Route 可能有多个原版入口，所以优先选和本帧最终 Marker 最近、且 EventID 一致的绑定。
            // 这样 R001067 的 A 会显示兰州原版判定点，而不是旧检测层的凉州对象 World。
            const RouteNativeBinding* bestBinding = nullptr;
            long long bestDistance = 0;
            for (const RouteNativeBinding& binding : edge->nativeBindings) {
                if (marker.routeEventId >= 0 && binding.eventId != marker.routeEventId) continue;
                const long long dx = static_cast<long long>(binding.worldX) - finalWorldX;
                const long long dy = static_cast<long long>(binding.worldY) - finalWorldY;
                const long long distance = dx * dx + dy * dy;
                if (!bestBinding || distance < bestDistance) {
                    bestBinding = &binding;
                    bestDistance = distance;
                }
            }
            // 如果人工 Event 把物理 Event 改成了报告里没有的特殊目标，仍允许显示旧检测 World 作为“自动证据”；
            // 但只要存在 dev6w 原版绑定，就绝不再把旧检测 World 冒充当前权威自动坐标。
            if (bestBinding) {
                autoValid = true;
                autoX = bestBinding->worldX;
                autoY = bestBinding->worldY;
            } else if (edge->nativeBindings.empty() && edge->worldValid) {
                autoValid = true;
                autoX = edge->worldX;
                autoY = edge->worldY;
            }
            if (edge->manualWorldValid) {
                overrideValid = true;
                overrideX = edge->manualWorldX;
                overrideY = edge->manualWorldY;
            }
        }
    } else if (const EvaluatedQuest* qv = FindEvaluatedQuestForDebug(marker.quest, evaluated)) {
        if (qv->stage) {
            autoValid = TryParseStrictIntText(qv->stage->worldXText, autoX)
                     && TryParseStrictIntText(qv->stage->worldYText, autoY);
            overrideValid = TryParseStrictIntText(qv->stage->manualWorldXText, overrideX)
                         && TryParseStrictIntText(qv->stage->manualWorldYText, overrideY);
        }
    }

    std::wstringstream line;
    line << L"A=";
    if (autoValid) line << L"(" << autoX << L"," << autoY << L")";
    else line << L"无";
    line << L"  O=";
    if (overrideValid) line << L"(" << overrideX << L"," << overrideY << L")";
    else line << L"无";
    line << L"  F=(" << finalWorldX << L"," << finalWorldY << L")";
    return line.str();
}

// 在鼠标附近找最近目标。
// 命中半径使用 Surface 像素，所以 4:3、16:9、21:9 最终都是“视觉上约 18 像素”的手感。
// Marker 与实体都参与比较；距离相同才让 Marker 胜出，这样不会发生“远处 Marker 抢走近处实体焦点”。
static DebugHoverSelection SelectDebugHover(const LockedCanvas& c, const GameSnapshot& snapshot,
                                            const DisplayGeometry& geometry,
                                            const std::vector<ResolvedMarker>& markers) {
    DebugHoverSelection out;
    if (!GetDebugMouseSurfacePoint(c, snapshot, geometry, out.mouseSurfaceX, out.mouseSurfaceY,
                                   out.mouseWorldX, out.mouseWorldY, out.mouseFromGameInput)) return out;
    out.mouseValid = true;

    const int radius = 18;
    const int maxDist2 = radius * radius;
    int bestDist2 = maxDist2 + 1;
    bool bestIsMarker = false;

    // 先检查实体。后面的 Marker 在“距离更近或距离相同”时可以覆盖它，所以相同距离下 Marker 优先。
    for (size_t i = 0; i < snapshot.entities.size(); ++i) {
        const RuntimeEntityView& e = snapshot.entities[i];
        if (!e.markerAnchorValid) continue;
        const POINT sp = RuntimeEntityDebugSurfacePoint(e, snapshot, geometry);
        const int dx = sp.x - out.mouseSurfaceX;
        const int dy = sp.y - out.mouseSurfaceY;
        const int d2 = dx*dx + dy*dy;
        if (d2 > maxDist2 || d2 >= bestDist2) continue;
        bestDist2 = d2;
        bestIsMarker = false;
        out.valid = true;
        out.isMarker = false;
        out.entityIndex = i;
        out.surfaceX = sp.x;
        out.surfaceY = sp.y;
    }

    for (size_t i = 0; i < markers.size(); ++i) {
        const ResolvedMarker& marker = markers[i];
        POINT sp = ProjectLogicalPointToSurface(
            marker.screenX, marker.screenY, snapshot.cameraX, snapshot.cameraY, geometry);
        sp.x += g_markerOffsetX;
        sp.y += g_markerYOffset + g_markerOffsetY;
        bool off = false;
        sp = ClampOffscreenPoint(sp.x, sp.y, c.width, c.height, off);

        const int dx = sp.x - out.mouseSurfaceX;
        const int dy = sp.y - out.mouseSurfaceY;
        const int d2 = dx*dx + dy*dy;
        if (d2 > maxDist2) continue;
        if (d2 > bestDist2) continue;
        if (d2 == bestDist2 && bestIsMarker) continue;

        bestDist2 = d2;
        bestIsMarker = true;
        out.valid = true;
        out.isMarker = true;
        out.markerIndex = i;
        out.surfaceX = sp.x;
        out.surfaceY = sp.y;
    }
    return out;
}

// F11 详情框固定三行，不再在每个对象旁边展开长字符串。
// 第一行用亮黄色突出 World 坐标；第二行解释 Camera/Logical/Surface 与事件；第三行解释任务/Route 坐标来源。
static void DrawDebugHoverPanel(LockedCanvas& c, const GameSnapshot& snapshot,
                                const DisplayGeometry& geometry,
                                const std::vector<EvaluatedQuest>& evaluated,
                                const std::vector<ResolvedMarker>& markers,
                                const DebugHoverSelection& hover) {
    const int lineH = std::max(16, g_fontSize + 2);
    const int panelH = lineH * 3 + 10;
    const int panelW = std::min(700, std::max(360, c.width - 20));
    const int left = std::max(6, c.width - panelW - 6);

    // 顶部普通诊断栏开启时占到约 100 像素。F11 面板自动放到它下面，避免两块开发信息互相覆盖。
    int top = g_diagnosticHud ? 106 : 6;
    if (top + panelH >= c.height) top = std::max(6, c.height - panelH - 6);
    const int right = std::min(c.width - 6, left + panelW);
    const int bottom = std::min(c.height - 6, top + panelH);
    FillRectSoftware(c, left, top, right, bottom, {18,18,22});
    DrawRectSoftware(c, left, top, right, bottom, {110,210,255});

    std::wstringstream line1;
    std::wstringstream line2;
    std::wstringstream line3;

    if (!hover.mouseValid) {
        line1 << L"F11：鼠标当前不在游戏客户区";
        line2 << L"PlayerWorld=(" << snapshot.playerX << L"," << snapshot.playerY << L")  Camera=("
              << snapshot.cameraX << L"," << snapshot.cameraY << L")";
        line3 << L"把鼠标移回游戏，再悬停 E编号 或任务 Marker 查看目标世界坐标。";
    } else if (!hover.valid) {
        line1 << L"PlayerWorld=(" << snapshot.playerX << L"," << snapshot.playerY << L")  MouseWorld=("
              << hover.mouseWorldX << L"," << hover.mouseWorldY << L") ["
              << (hover.mouseFromGameInput ? L"GameInput" : L"Win32Fallback") << L"]";
        line2 << L"MouseSurface=(" << hover.mouseSurfaceX << L"," << hover.mouseSurfaceY << L")  Camera=("
              << snapshot.cameraX << L"," << snapshot.cameraY << L")";
        line3 << L"悬停 E编号/任务 Marker；人工 TOML 坐标只抄 World，不抄 Surface。";
    } else if (!hover.isMarker && hover.entityIndex < snapshot.entities.size()) {
        const RuntimeEntityView& e = snapshot.entities[hover.entityIndex];
        const POINT sp = RuntimeEntityDebugSurfacePoint(e, snapshot, geometry);
        const int logicalX = e.worldAnchorValid ? e.worldX - snapshot.cameraX : e.screenX;
        const int logicalY = e.worldAnchorValid ? e.worldY - snapshot.cameraY : e.screenY;

        line1 << L"E" << e.index << L" " << e.name << L"  WORLD=(" << e.worldX << L"," << e.worldY
              << L")  MouseWorld=(" << hover.mouseWorldX << L"," << hover.mouseWorldY << L") ["
              << (hover.mouseFromGameInput ? L"GameInput" : L"Win32Fallback") << L"]";
        line2 << L"Cam=(" << snapshot.cameraX << L"," << snapshot.cameraY << L")  Logic=("
              << logicalX << L"," << logicalY << L")  Surf=(" << sp.x << L"," << sp.y << L")  EA="
              << static_cast<unsigned>(e.touchEventId) << L" EB=" << static_cast<unsigned>(e.manualEventId)
              << L" Mode=" << static_cast<unsigned>(e.interactionMode) << L" " << DebugKindText(e);

        size_t boundCount = 0;
        const ResolvedMarker* bound = FindMarkerBoundToEntityForDebug(e, markers, boundCount);
        if (bound) {
            const int32_t finalWorldX = bound->screenX + snapshot.cameraX;
            const int32_t finalWorldY = bound->screenY + snapshot.cameraY;
            line3 << L"Bind=" << (bound->quest ? Utf8ToWide(bound->quest->questNo) : L"-") << L" "
                  << (bound->quest ? bound->quest->title : L"-");
            if (boundCount > 1) line3 << L" +" << (boundCount - 1);
            if (!bound->routeId.empty()) line3 << L"  R=" << Utf8ToWide(bound->routeId);
            line3 << L"  " << BuildMarkerCoordinateSourceLine(*bound, evaluated, finalWorldX, finalWorldY);
        } else {
            line3 << L"Bind=无  SourceOrdinal=" << e.sourceOrdinal
                  << L"  Ctrl+F11 只读；要修坐标请复制第一行 WORLD。";
        }
    } else if (hover.markerIndex < markers.size()) {
        const ResolvedMarker& m = markers[hover.markerIndex];
        const int32_t finalWorldX = m.screenX + snapshot.cameraX;
        const int32_t finalWorldY = m.screenY + snapshot.cameraY;

        line1 << (m.quest ? Utf8ToWide(m.quest->questNo) : L"-") << L" "
              << (m.quest ? m.quest->title : L"-") << L"  WORLD=(" << finalWorldX << L"," << finalWorldY
              << L")  MouseWorld=(" << hover.mouseWorldX << L"," << hover.mouseWorldY << L") ["
              << (hover.mouseFromGameInput ? L"GameInput" : L"Win32Fallback") << L"]";
        line2 << L"Cam=(" << snapshot.cameraX << L"," << snapshot.cameraY << L")  Logic=("
              << m.screenX << L"," << m.screenY << L")  Surf=(" << hover.surfaceX << L"," << hover.surfaceY
              << L")  Anchor=" << Utf8ToWide(MarkerAnchorKindName(m.anchorKind));
        if (m.fromEntity) {
            line2 << L"  E" << m.entityIndex << L" EA=" << static_cast<unsigned>(m.touchEventId)
                  << L" EB=" << static_cast<unsigned>(m.manualEventId);
        }

        if (m.fromRoute) {
            line3 << L"R=" << Utf8ToWide(m.routeId) << L" -> " << Utf8ToWide(m.routeNextScene)
                  << L" Evt=" << m.routeEventId << L"  ";
        } else {
            line3 << L"Role=" << Utf8ToWide(MarkerRoleName(m.role)) << L"  ";
        }
        line3 << BuildMarkerCoordinateSourceLine(m, evaluated, finalWorldX, finalWorldY);
    }

    DrawTextShadowSoftware(c, left+7, top+4, line1.str(), {255,245,150});
    DrawTextShadowSoftware(c, left+7, top+4+lineH, line2.str(), {225,235,245});
    DrawTextShadowSoftware(c, left+7, top+4+lineH*2, line3.str(), {190,215,235});
}

// 只有聚焦目标变化时才写一条日志，绝不能每个 Present 都写。
// 这让开发者可以在游戏里依次把鼠标移到几个入口，随后直接从 Castle_Quest.log 复制坐标。
static void MaybeLogDebugHover(const GameSnapshot& snapshot,
                               const std::vector<EvaluatedQuest>& evaluated,
                               const std::vector<ResolvedMarker>& markers,
                               const DebugHoverSelection& hover) {
    static std::string previousKey;
    std::ostringstream key;
    key << snapshot.sceneId << ':';
    if (!hover.valid) {
        key << "none";
    } else if (hover.isMarker && hover.markerIndex < markers.size()) {
        const ResolvedMarker& m = markers[hover.markerIndex];
        key << "marker:" << (m.quest ? m.quest->id : "-") << ':' << m.routeId << ':'
            << m.screenX << ':' << m.screenY;
    } else if (hover.entityIndex < snapshot.entities.size()) {
        const RuntimeEntityView& e = snapshot.entities[hover.entityIndex];
        key << "entity:" << e.index << ':' << static_cast<const void*>(e.source);
    }
    if (key.str() == previousKey) return;
    previousKey = key.str();
    if (!hover.valid) return;

    if (hover.isMarker && hover.markerIndex < markers.size()) {
        const ResolvedMarker& m = markers[hover.markerIndex];
        const int32_t finalWorldX = m.screenX + snapshot.cameraX;
        const int32_t finalWorldY = m.screenY + snapshot.cameraY;
        const std::wstring sourceLine = BuildMarkerCoordinateSourceLine(m, evaluated, finalWorldX, finalWorldY);
        Log("[F11聚焦] Marker Quest=%s Title=%s Route=%s Next=%s FinalWorld=(%d,%d) MouseWorld=(%d,%d) Source=%s",
            m.quest ? m.quest->questNo.c_str() : "-",
            m.quest ? WideToUtf8(m.quest->title).c_str() : "-",
            m.routeId.c_str(), m.routeNextScene.c_str(), finalWorldX, finalWorldY,
            hover.mouseWorldX, hover.mouseWorldY, WideToUtf8(sourceLine).c_str());
    } else if (hover.entityIndex < snapshot.entities.size()) {
        const RuntimeEntityView& e = snapshot.entities[hover.entityIndex];
        size_t boundCount = 0;
        const ResolvedMarker* bound = FindMarkerBoundToEntityForDebug(e, markers, boundCount);
        if (bound) {
            const int32_t finalWorldX = bound->screenX + snapshot.cameraX;
            const int32_t finalWorldY = bound->screenY + snapshot.cameraY;
            const std::wstring sourceLine = BuildMarkerCoordinateSourceLine(*bound, evaluated, finalWorldX, finalWorldY);
            Log("[F11聚焦] Entity E%u Name=%s World=(%d,%d) EA=%02X EB=%02X Kind=%s BindQuest=%s Route=%s BindCount=%u Source=%s",
                static_cast<unsigned>(e.index), WideToUtf8(e.name).c_str(), e.worldX, e.worldY,
                static_cast<unsigned>(e.touchEventId), static_cast<unsigned>(e.manualEventId),
                WideToUtf8(DebugKindText(e)).c_str(), bound->quest ? bound->quest->questNo.c_str() : "-",
                bound->routeId.c_str(), static_cast<unsigned>(boundCount), WideToUtf8(sourceLine).c_str());
        } else {
            Log("[F11聚焦] Entity E%u Name=%s World=(%d,%d) EA=%02X EB=%02X Kind=%s BindQuest=-",
                static_cast<unsigned>(e.index), WideToUtf8(e.name).c_str(), e.worldX, e.worldY,
                static_cast<unsigned>(e.touchEventId), static_cast<unsigned>(e.manualEventId),
                WideToUtf8(DebugKindText(e)).c_str());
        }
    }
}

static bool ShouldShowQuestInJournal(const EvaluatedQuest& qv, const GameSnapshot& snapshot) {
    if (!qv.def) return false;
    // addon 的 show_in_journal 是人工作者直接控制的显示字段；它不影响该步骤是否在后台等待 Event 完成。
    if (qv.addonActive) return qv.stage && qv.stage->showInJournal;
    bool ok = false;
    return EvaluateCondition(qv.def->journalWhen, snapshot, &ok) && ok;
}

static std::vector<std::wstring> CollectQuestDisplayNotes(const EvaluatedQuest& qv, const GameSnapshot&) {
    // dev6zc：任务 HUD 的备注来源现在只有当前 Stage 的 player_notes。
    // 旧 [[warning]] 机制已经彻底从运行格式和 UI 中移除，因此这里不再偷偷把任何“错过提醒”混进任务注释。
    std::vector<std::wstring> notes;
    if (qv.stage) {
        notes.insert(notes.end(), qv.stage->playerNotes.begin(), qv.stage->playerNotes.end());
    }
    return notes;
}

static void DrawQuestPanel(LockedCanvas& c, const GameSnapshot& snapshot, const std::vector<EvaluatedQuest>& evaluated) {
    if (!g_questPanel) return;
    const int left = std::max(20, c.width / 2 - 300);
    const int top = 55;
    const int right = std::min(c.width - 20, left + 600);
    const int bottom = std::min(c.height - 20, top + 340);
    FillRectSoftware(c, left, top, right, bottom, {18,18,22});
    DrawRectSoftware(c, left, top, right, bottom, {210,210,225});
    DrawTextShadowSoftware(c, left+14, top+10, L"任务记录  [Ctrl+F7关闭]", {255,245,190});
    int y = top + 38;
    for (const EvaluatedQuest& qv : evaluated) {
        if (!qv.def || !ShouldShowQuestInJournal(qv, snapshot)) continue;
        const wchar_t* displayState = qv.addonActive ? L"追加步骤" : QuestStateText(qv.state);
        const int panelIconCenterX = left + 23;
        const int panelIconCenterY = y + g_fontSize / 2;
        DrawQuestListIconSoftware(c, panelIconCenterX, panelIconCenterY, *qv.def);
        std::wstring questTitleLine = L"[" + std::wstring(displayState) + L"] " + qv.def->title;
        // 任务名称继续使用 dev6m 已验收的原颜色；只有左侧任务类型图标使用 MarkerColor 配色。
        DrawTextShadowSoftware(c, left+36, y, questTitleLine, {235,235,235});
        y += g_fontSize + 1;
        std::wstring objective;
        if (qv.addonActive && qv.stage) objective = qv.stage->objective;
        else if (qv.state == QuestState::Available) objective = qv.def->availableObjective;
        else if (qv.state == QuestState::Active && qv.stage) objective = qv.stage->objective;
        if (!objective.empty()) {
            // dev5 的自动换行器不仅服务轻量 Tracker，Ctrl+F7 任务记录面板也复用同一套像素测宽，
            // 避免长目标在面板右边被直接裁掉。
            const std::vector<std::wstring> lines = WrapTextPixels(objective, std::max(80, right - (left + 34) - 14));
            for (const std::wstring& wrappedLine : lines) {
                if (y > bottom - 30) break;
                DrawTextShadowSoftware(c, left+34, y, wrappedLine, {205,205,215});
                y += g_fontSize + 2;
            }
            y += 2;
        } else y += 3;

        const std::vector<std::wstring> displayNotes = CollectQuestDisplayNotes(qv, snapshot);
        if (!displayNotes.empty()) y += g_questNoteSpacing;
        bool firstPanelNoteLine = true;
        for (const std::wstring& note : displayNotes) {
            const std::vector<std::wstring> noteLines = WrapTextPixels(L"· " + note, std::max(80, right - (left + 50) - 14));
            for (const std::wstring& noteLine : noteLines) {
                if (y > bottom - 30) break;
                if (!firstPanelNoteLine) y += g_questNoteLineSpacing;
                DrawTextShadowSoftware(c, left+50, y, noteLine, {80,220,220});
                y += g_fontSize + 2;
                firstPanelNoteLine = false;
            }
            if (y > bottom - 30) break;
        }
        if (!displayNotes.empty()) y += 4;
        if (y > bottom - 30) break;
    }

}

static void DrawOverlayContentSoftware(LockedCanvas& c) {
    GameSnapshot snapshot = CaptureSnapshot();
    std::vector<EvaluatedQuest> evaluated;
    std::vector<ResolvedMarker> markers;
    if (snapshot.valid) {
        MaybeDumpSceneEntities(snapshot);
        MaybeLogGameStateChanges(snapshot);
        evaluated = EvaluateAllQuests(snapshot);
        // 人工插入步骤以“指定 Event 实际开始执行”为完成信号。若本帧刚完成，立刻再求值一次，
        // 这样对话开始的同一帧就能把任务显示切回原始 before Stage，而不需要等下一次 GameVar 变化。
        if (ObserveAddonStageCompletion(snapshot, evaluated)) evaluated = EvaluateAllQuests(snapshot);
        markers = BuildResolvedMarkers(snapshot, evaluated);
        MaybeLogQuestResolution(snapshot, evaluated, markers);
    }

    // 先完成只读任务求值/日志，再决定本帧是否允许画任何 Quest 像素。
    // 这保证对话中完成任务时，后台状态仍会立刻更新；但玩家绝不会在原版对话框/菜单上看到 Quest HUD。
    const VisualGateReason visualGate = snapshot.valid
        ? QueryQuestVisualGate(true, snapshot.sceneObjectListValid)
        : VisualGateReason::InvalidSnapshot;
    MaybeLogVisualGate(visualGate);
    if (visualGate != VisualGateReason::Visible) return;

    const DisplayGeometry geometry = CalculateDisplayGeometry(snapshot.cameraX, snapshot.cameraY, c.width, c.height);

    if (g_diagnosticHud) {
        // dev6e：诊断栏只保留“能帮助定位导航错误”的信息。
        // 旧版把“彩石弹珠=1”这类单一支线库存变量常驻在这里，既占空间，也无法解释主线为什么画错 Marker。
        // 现在第四行直接显示主线 Stage -> Goal -> Next -> Event -> 实际绑定数量 -> Marker 类型 -> Route 状态。
        // 用户只要给一张截图，就能同时告诉我们 BFS 选了哪条边、当前物理 Event 找到了几个出口。
        const int diagnosticWidth = std::min(c.width, 760);
        const int diagnosticHeight = std::min(c.height, 100);
        FillRectSoftware(c, 0, 0, diagnosticWidth, diagnosticHeight, {18,18,18});
        DrawRectSoftware(c, 0, 0, std::max(0, diagnosticWidth - 1), std::max(0, diagnosticHeight - 1), {80,255,120});
        DrawTextShadowSoftware(c, 7, 5, L"Castle_Quest v0.1-dev6zd  SOFTWARE OVERLAY", {100,255,140});

        std::wstringstream line1;
        line1 << L"Scene=" << Utf8ToWide(snapshot.sceneId) << L" Obj=" << snapshot.entityCount
              << L" Mgr=" << (snapshot.sceneObjectListValid ? 1 : 0) << L" Fixed=" << snapshot.fixedActorCount
              << L" Surface=" << c.width << L"x" << c.height << L"/" << c.bitsPerPixel << L"bpp";
        DrawTextShadowSoftware(c, 7, 5 + g_fontSize + 2, line1.str(), {225,225,225});

        std::wstringstream line2;
        line2 << L"Player(" << snapshot.playerX << L"," << snapshot.playerY << L") Camera("
              << snapshot.cameraX << L"," << snapshot.cameraY << L") DisplayCamX=" << geometry.effectiveCameraX
              << L" Side=" << geometry.sideWidth << L" Evt=" << snapshot.activeEventId
              << L" Battle=" << snapshot.battleLayoutGroup << L" Markers=" << markers.size();
        DrawTextShadowSoftware(c, 7, 5 + (g_fontSize + 2)*2, line2.str(), {225,225,225});

        std::wstringstream line3;
        if (g_primaryNavigationDiagnostic.active) {
            line3 << L"Main=" << Utf8ToWide(g_primaryNavigationDiagnostic.stageId)
                  << L" Goal=" << Utf8ToWide(g_primaryNavigationDiagnostic.goalScene)
                  << L" Next=" << (g_primaryNavigationDiagnostic.nextScene.empty()
                                      ? L"-" : Utf8ToWide(g_primaryNavigationDiagnostic.nextScene))
                  << L" Rid=" << (g_primaryNavigationDiagnostic.routeId.empty()
                                      ? L"-" : Utf8ToWide(g_primaryNavigationDiagnostic.routeId))
                  << L" Evt=" << g_primaryNavigationDiagnostic.routeEventId
                  << L" Bind=" << g_primaryNavigationDiagnostic.bindCount
                  << L" Type=" << Utf8ToWide(MarkerRoleName(g_primaryNavigationDiagnostic.markerRole))
                  << L" Route=" << Utf8ToWide(g_primaryNavigationDiagnostic.status);
        } else {
            line3 << L"Main=- Goal=- Next=- Rid=- Evt=- Bind=0 Type=- Route=Inactive";
        }
        DrawTextShadowSoftware(c, 7, 5 + (g_fontSize + 2)*3, line3.str(), {225,225,225});
    }

    // dev5：Tracker 正式移动到屏幕“9点方向”——X轴最左、Y轴整体居中。
    // 这里先收集所有真正要显示的任务，计算每条标题/正文经过像素换行后的高度，最后再一次性求 top。
    // 这样不是“从屏幕中央开始往下堆”，而是整个任务组围绕屏幕垂直中心展开。
    if (g_questHud && snapshot.valid) {
        struct HudQuestLayout {
            const EvaluatedQuest* qv=nullptr;
            std::wstring title;
            std::wstring objective;
            std::vector<std::wstring> objectiveLines;
            std::vector<std::wstring> noteLines;
            int height=0;
        };
        std::vector<HudQuestLayout> layouts;
        const int left=std::max(0,g_questHudMarginLeft);
        const int titleIndent=0;
        const int objectiveIndent=18;
        const int noteIndent=36;
        const int maxWidth=std::max(180,std::min(g_questHudMaxWidth,c.width/2-24));
        const int titleHeight=g_fontSize+2;
        const int lineHeight=g_fontSize+2;
        int totalHeight=0;
        for(const EvaluatedQuest& qv:evaluated){
            if(!qv.def || (!qv.def->tracked && !qv.def->primary)) continue;
            const bool showTracker = qv.addonActive ? (qv.stage && qv.stage->showInJournal) :
                                     (qv.state==QuestState::Active ||
                                      (qv.state==QuestState::Available && qv.def->availableInTracker));
            if(!showTracker) continue;
            HudQuestLayout item; item.qv=&qv;
            // dev6p：标题字符串只保留任务名称。任务类型符号继续由软件图标单独绘制，
            // 因此图标可以使用 INI MarkerColor，而任务名称继续保持原来已经验收的文字颜色。
            item.title=qv.def->title;
            item.objective=(qv.addonActive && qv.stage) ? qv.stage->objective :
                           (qv.state==QuestState::Available ? qv.def->availableObjective : (qv.stage?qv.stage->objective:L""));
            item.objectiveLines=WrapTextPixels(item.objective,maxWidth-objectiveIndent);
            for (const std::wstring& note : CollectQuestDisplayNotes(qv, snapshot)) {
                const std::vector<std::wstring> wrapped = WrapTextPixels(L"· " + note, std::max(80, maxWidth-noteIndent));
                item.noteLines.insert(item.noteLines.end(), wrapped.begin(), wrapped.end());
            }

            // 一个任务块自己的高度计算“标题 + 当前目标 + 多行备注”。
            // 标题和第一行说明之间只有在确实存在说明时才加入 QuestObjectiveSpacing；
            // 这样没有说明文字的 Available/Active 任务不会凭空多出一段空白。
            item.height=titleHeight;
            if(!item.objectiveLines.empty()) {
                item.height+=g_questObjectiveSpacing;
                item.height+=static_cast<int>(item.objectiveLines.size())*lineHeight;
            }
            if(!item.noteLines.empty()) {
                item.height+=g_questNoteSpacing;
                item.height+=static_cast<int>(item.noteLines.size())*lineHeight;
                if(item.noteLines.size()>1) {
                    item.height+=static_cast<int>(item.noteLines.size()-1)*g_questNoteLineSpacing;
                }
            }
            totalHeight+=item.height;
            layouts.push_back(std::move(item));
        }

        // QuestItemSpacing 的语义是“任务 A 与任务 B 之间的距离”，所以 N 个任务只存在 N-1 段间距。
        // 旧代码把 gap 也加在最后一个任务后面，虽然肉眼差异很小，但会让整组垂直居中时多算一段空白。
        // dev6f 在把该值开放到 INI 的同时顺手把这个语义修正为真正的“任务之间”。
        if(layouts.size()>1) totalHeight+=static_cast<int>(layouts.size()-1)*g_questItemSpacing;

        const int safeTop=8, safeBottom=8;
        int y=(c.height-totalHeight)/2;
        y=std::max(safeTop,std::min(y,c.height-safeBottom-std::min(totalHeight,c.height-safeTop-safeBottom)));
        for(size_t layoutIndex=0; layoutIndex<layouts.size(); ++layoutIndex){
            const HudQuestLayout& item=layouts[layoutIndex];
            const int hudIconCenterX = left + 7;
            const int hudIconCenterY = y + g_fontSize / 2;
            DrawQuestListIconSoftware(c, hudIconCenterX, hudIconCenterY, *item.qv->def);
            DrawTextShadowSoftware(c,left+titleIndent+18,y,item.title,{255,245,180});
            y+=titleHeight;

            // 只有“标题下面真的要画说明”时才应用任务-说明间距。
            if(!item.objectiveLines.empty()) y+=g_questObjectiveSpacing;

            for(const std::wstring& objectiveLine:item.objectiveLines){
                if(y+lineHeight>c.height-safeBottom) break;
                DrawTextShadowSoftware(c,left+objectiveIndent,y,objectiveLine,{245,245,245});
                y+=lineHeight;
            }
            if(!item.noteLines.empty()) y+=g_questNoteSpacing;
            for(size_t noteIndex=0; noteIndex<item.noteLines.size(); ++noteIndex){
                const std::wstring& noteLine=item.noteLines[noteIndex];
                if(y+lineHeight>c.height-safeBottom) break;
                // 青色专门表示 player_notes，和白色 objective 正文形成稳定视觉分层。
                DrawTextShadowSoftware(c,left+noteIndent,y,noteLine,{80,220,220});
                y+=lineHeight;
                if(noteIndex+1<item.noteLines.size()) y+=g_questNoteLineSpacing;
            }

            // 最后一项后面不再追加任务间距；这样配置值严格等于用户看到的“两个任务块之间的空白”。
            if(layoutIndex+1<layouts.size()) y+=g_questItemSpacing;
            if(y>=c.height-safeBottom) break;
        }
    }

    if (g_worldMarkers && snapshot.valid) {
        for (const ResolvedMarker& marker : markers) {
            // marker.screenX/Y 是 resolver 产生的“原版 640 逻辑坐标”。
            // dev6d 在最后一刻才投影到当前 Surface，避免任务数据库/实体匹配层知道宽屏实现细节。
            const POINT surfacePoint = ProjectLogicalPointToSurface(
                marker.screenX, marker.screenY, snapshot.cameraX, snapshot.cameraY, geometry);
            const int sx = surfacePoint.x + g_markerOffsetX;
            const int sy = surfacePoint.y + g_markerYOffset + g_markerOffsetY;
            bool off = false;
            const POINT p = ClampOffscreenPoint(sx, sy, c.width, c.height, off);
            DrawMarkerShapeSoftware(c, p.x, p.y, marker);
            if (off) DrawOffscreenDirectionSoftware(c, p.x, p.y, sx, sy, MarkerColor(marker));
        }
        if (g_debugShowRuntimeEntities || g_debugLegacyShowAllInteractiveEntities) {
            const DebugHoverSelection hover = SelectDebugHover(c, snapshot, geometry, markers);

            // 对象数量较少时显示两行紧凑信息；对象超过 14 个以后自动退化成仅 E编号。
            // 这个阈值只影响调试文字密度，不影响实体枚举和 Marker 解析。
            const bool crowded = snapshot.entities.size() > 14;
            for (size_t i = 0; i < snapshot.entities.size(); ++i) {
                const bool focused = hover.valid && !hover.isMarker && hover.entityIndex == i;
                DrawRuntimeEntityDebugCompact(c, snapshot.entities[i], snapshot, geometry, focused, crowded);
            }

            // Marker 本身已经在上一段用任务颜色画过；这里再套一个亮黄色外框，让“鼠标当前选中的是哪一个”更醒目。
            if (hover.valid) {
                DrawRectSoftware(c, hover.surfaceX-7, hover.surfaceY-7, hover.surfaceX+7, hover.surfaceY+7, {255,245,120});
            }
            DrawDebugHoverPanel(c, snapshot, geometry, evaluated, markers, hover);
            MaybeLogDebugHover(snapshot, evaluated, markers, hover);
        }
        if (g_debugTestWorldMarker) {
            ResolvedMarker test;
            test.screenX = g_debugWorldX - snapshot.cameraX;
            test.screenY = g_debugWorldY - snapshot.cameraY;
            const POINT surfacePoint = ProjectLogicalPointToSurface(
                test.screenX, test.screenY, snapshot.cameraX, snapshot.cameraY, geometry);
            const int sx = surfacePoint.x + g_markerOffsetX;
            const int sy = surfacePoint.y + g_markerYOffset + g_markerOffsetY;
            bool off = false;
            POINT p = ClampOffscreenPoint(sx, sy, c.width, c.height, off);
            DrawMarkerShapeSoftware(c, p.x, p.y, test);
            DrawTextShadowSoftware(c, p.x+16, p.y-8, L"WORLD TEST", {255,255,255});
        }
    }

    DrawQuestPanel(c, snapshot, evaluated);
}

static bool DrawOverlayToBackSurface(void* renderer) {
    if (!g_enabled || !g_softwareOverlay || !renderer) return false;
    IDirectDrawSurface* back = nullptr;
    if (!SafeReadPtr(renderer, 0x8, back) || !back) return false;

    DDSURFACEDESC desc = {};
    desc.dwSize = sizeof(desc);
    const HRESULT lockHr = back->Lock(nullptr, &desc, DDLOCK_WAIT, nullptr);
    const DWORD bpp = desc.ddpfPixelFormat.dwRGBBitCount;
    if (back != g_lastOverlaySurface || lockHr != g_lastOverlayLockResult || bpp != g_lastOverlayBpp) {
        Log("[Overlay] BackSurface=%p Lock=0x%08lX size=%lux%lu pitch=%ld bpp=%lu masks=%08lX/%08lX/%08lX",
            back, static_cast<unsigned long>(lockHr), static_cast<unsigned long>(desc.dwWidth),
            static_cast<unsigned long>(desc.dwHeight), static_cast<long>(desc.lPitch), static_cast<unsigned long>(bpp),
            static_cast<unsigned long>(desc.ddpfPixelFormat.dwRBitMask),
            static_cast<unsigned long>(desc.ddpfPixelFormat.dwGBitMask),
            static_cast<unsigned long>(desc.ddpfPixelFormat.dwBBitMask));
        g_lastOverlaySurface = back; g_lastOverlayLockResult = lockHr; g_lastOverlayBpp = bpp;
    }
    if (FAILED(lockHr) || !desc.lpSurface) return false;

    LockedCanvas c;
    c.pixels = static_cast<uint8_t*>(desc.lpSurface);
    c.width = static_cast<int>(desc.dwWidth);
    c.height = static_cast<int>(desc.dwHeight);
    c.pitch = desc.lPitch;
    c.bitsPerPixel = bpp;
    c.rMask = desc.ddpfPixelFormat.dwRBitMask;
    c.gMask = desc.ddpfPixelFormat.dwGBitMask;
    c.bMask = desc.ddpfPixelFormat.dwBBitMask;

    // 少数 DirectDraw wrapper 在 Lock 返回的旧版 DDSURFACEDESC 中不填 RGB mask。
    // 这种情况下按最常见的真彩格式给出保守 fallback；如果实机颜色不对，日志里的 bpp/mask 会直接暴露出来。
    if (!c.rMask && !c.gMask && !c.bMask) {
        if (c.bitsPerPixel == 16) { c.rMask = 0xF800; c.gMask = 0x07E0; c.bMask = 0x001F; }
        else if (c.bitsPerPixel == 24 || c.bitsPerPixel == 32) { c.rMask = 0x00FF0000; c.gMask = 0x0000FF00; c.bMask = 0x000000FF; }
    }

    const bool supported = c.width > 0 && c.height > 0 && (c.bitsPerPixel == 16 || c.bitsPerPixel == 24 || c.bitsPerPixel == 32)
                           && c.rMask && c.gMask && c.bMask;
    if (supported) DrawOverlayContentSoftware(c);
    back->Unlock(desc.lpSurface);

    if (supported && !g_loggedFirstOverlaySuccess) {
        g_loggedFirstOverlaySuccess = true;
        Log("[Overlay] SUCCESS：renderer+8 已使用 Lock/Unlock 软件像素后端；DirectDraw GetDC 路线不再执行。");
    }
    return supported;
}

// ============================================================================
// 12. 配置与热键
// ============================================================================

static void RecreateFont() {
    if (g_font) {
        DeleteObject(g_font);
        g_font = nullptr;
    }
    g_font = CreateFontW(-std::max(10, g_fontSize), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY,
                         DEFAULT_PITCH | FF_DONTCARE, g_fontName.c_str());
}

static bool LoadMainConfig() {
    IniFile ini;
    std::wstring path = JoinPath(g_moduleDir, L"Castle_Quest.ini");
    if (!ini.Load(path)) {
        Log("[配置] Castle_Quest.ini 不存在或无法读取，使用内置 dev6zd 默认值。");
        RecreateFont();
        return false;
    }
    g_enabled = ini.GetBool("General", "Enable", true);
    // dev6l：调试窗口有独立 INI 开关。开发包默认 1；发行配置只需改成 0，不删代码。
    g_diagnosticHud = ini.GetBool("Debug", "ShowDiagnosticWindow", true);
    g_worldMarkers = ini.GetBool("General", "WorldMarkers", true);
    g_questHud = ini.GetBool("General", "QuestHud", true);
    g_fontName = Utf8ToWide(ini.Get("Visual", "FontName", "Microsoft JhengHei UI"));
    g_fontSize = std::max(10, ini.GetInt("Visual", "FontSize", 14));
    g_markerSize = std::max(6, ini.GetInt("Visual", "MarkerSize", 14));

    // dev6q：四类任务图标颜色继续从 [MarkerColor] 读取 R,G,B。
    // 配置错误不会中断插件，只回退当前发行默认值并写日志，方便用户自己调色。
    LoadMarkerRgb(ini, "Main", 255, 205, 70, g_markerMainR, g_markerMainG, g_markerMainB);
    LoadMarkerRgb(ini, "Side", 80, 220, 220, g_markerSideR, g_markerSideG, g_markerSideB);
    LoadMarkerRgb(ini, "Timed", 255, 150, 60, g_markerTimedR, g_markerTimedG, g_markerTimedB);
    LoadMarkerRgb(ini, "EndingTimed", 190, 75, 75,
                  g_markerEndingTimedR, g_markerEndingTimedG, g_markerEndingTimedB);
    g_questHudMaxWidth = std::max(180, ini.GetInt("Visual", "QuestHudMaxWidth", 440));
    g_questHudMarginLeft = std::max(0, ini.GetInt("Visual", "QuestHudMarginLeft", 12));

    // 任务 HUD 的任务块、正文和任务注释间距都允许用户直接从 INI 调整。
    // 下限 0 表示完全不额外留白；上限 64 防止手误填入几千像素后把整个 Tracker 推出屏幕。
    // dev6q 默认把任务间额外留白改为 0 像素，同时保留 0 像素额外标题/说明间距；用户仍可在 INI 自行调大。
    g_questItemSpacing = std::clamp(ini.GetInt("Visual", "QuestItemSpacing", 0), 0, 64);
    g_questObjectiveSpacing = std::clamp(ini.GetInt("Visual", "QuestObjectiveSpacing", 0), 0, 64);
    g_questNoteSpacing = std::clamp(ini.GetInt("Visual", "QuestNoteSpacing", 3), 0, 64);
    g_questNoteLineSpacing = std::clamp(ini.GetInt("Visual", "QuestNoteLineSpacing", 2), 0, 32);

    g_markerYOffset = ini.GetInt("Visual", "MarkerYOffset", -34);
    g_markerOffsetX = ini.GetInt("Visual", "MarkerOffsetX", 0);
    g_markerOffsetY = ini.GetInt("Visual", "MarkerOffsetY", 0);

    // EntityProjection 是 dev6b 专门为坐标闭环增加的 A/B 开关。
    // 拼写不认识时安全回到 WorldCamera，并写日志；这样手改 INI 出错不会悄悄变成旧算法。
    const std::string projection = ToLowerAscii(Trim(ini.Get("Visual", "EntityProjection", "WorldCamera")));
    if (projection == "controllercamera" || projection == "controller") {
        g_entityMarkerProjection = EntityMarkerProjection::ControllerCamera;
    } else {
        g_entityMarkerProjection = EntityMarkerProjection::WorldCamera;
        if (projection != "worldcamera" && projection != "world") {
            Log("[配置] 未识别 EntityProjection=%s，已回退 WorldCamera。", projection.c_str());
        }
    }

    g_debugShowRuntimeEntities = ini.GetBool("Debug", "ShowRuntimeEntities", false);
    g_logGameVarChanges = ini.GetBool("Debug", "LogGameVarChanges", true);
    g_debugLegacyShowAllInteractiveEntities = ini.GetBool("Debug", "ShowAllInteractiveEntities", false);
    g_softwareOverlay = ini.GetBool("Overlay", "SoftwareBackend", true);
    g_questPanel = ini.GetBool("General", "QuestPanelInitiallyOpen", false);
    g_debugTestWorldMarker = ini.GetBool("Debug", "TestWorldMarker", false);
    g_debugWorldX = ini.GetInt("Debug", "WorldX", 0);
    g_debugWorldY = ini.GetInt("Debug", "WorldY", 0);
    RecreateFont();
    Log("[配置] 已重载。Enable=%d DiagnosticWindow=%d Markers=%d QuestHud=%d QuestPanel=%d EntityDebug=%d VarLog=%d "
        "Software=%d EntityProjection=%s QuestItemSpacing=%d QuestObjectiveSpacing=%d QuestNoteSpacing=%d QuestNoteLineSpacing=%d "
        "MarkerColor(Main=%d,%d,%d Side=%d,%d,%d Timed=%d,%d,%d EndingTimed=%d,%d,%d)",
        g_enabled, g_diagnosticHud, g_worldMarkers, g_questHud, g_questPanel,
        (g_debugShowRuntimeEntities || g_debugLegacyShowAllInteractiveEntities) ? 1 : 0, g_logGameVarChanges,
        g_softwareOverlay, EntityMarkerProjectionName(), g_questItemSpacing, g_questObjectiveSpacing,
        g_questNoteSpacing, g_questNoteLineSpacing,
        g_markerMainR, g_markerMainG, g_markerMainB,
        g_markerSideR, g_markerSideG, g_markerSideB,
        g_markerTimedR, g_markerTimedG, g_markerTimedB,
        g_markerEndingTimedR, g_markerEndingTimedG, g_markerEndingTimedB);
    return true;
}

static bool CtrlFunctionKeyPressedOnce(int vk) {
    // dev6p 起所有功能键都必须配合 Ctrl。这样做的原因很简单：
    // 原游戏、宽屏补丁、截图软件或其他插件也可能使用 F 键；直接占用 F7/F8/F9... 很容易发生冲突。
    //
    // GetAsyncKeyState 的最高位表示“此刻正在按住”。这里把“Ctrl 正在按住”和“指定 F 键正在按住”
    // 合成一个组合键状态，然后只在组合状态从 false 变成 true 的那一帧返回 true。
    // 因此一直按住 Ctrl+F7 不会每帧反复开关面板，只会切换一次。
    static std::map<int, bool> previousCombo;
    const bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    const bool functionDown = (GetAsyncKeyState(vk) & 0x8000) != 0;
    const bool comboDown = ctrlDown && functionDown;
    const bool wasDown = previousCombo[vk];
    previousCombo[vk] = comboDown;
    return comboDown && !wasDown;
}

static void ProcessHotkeys() {
    // Ctrl+F6：只隐藏/显示左侧常驻轻量任务区域。
    // Ctrl+F7 的完整任务清单是另一套界面，两者互不影响。这样玩家可以长期关掉左侧 Tracker，
    // 需要查看任务时再打开完整任务清单，而不必把整个 Quest Overlay 一起关闭。
    if (CtrlFunctionKeyPressedOnce(VK_F6)) {
        g_questHud = !g_questHud;
        Log("[热键] Ctrl+F6：QuestHud=%d", g_questHud);
    }
    if (CtrlFunctionKeyPressedOnce(VK_F7)) {
        g_questPanel = !g_questPanel;
        Log("[热键] Ctrl+F7：QuestPanel=%d", g_questPanel);
    }
    if (CtrlFunctionKeyPressedOnce(VK_F8)) {
        LoadMainConfig();
        LoadQuestDatabase();
        Log("[热键] Ctrl+F8：配置与 Castle_Quest Base+Addon 任务数据库已事务式热重载；同一 quest_id+stage_id 的 Addon 完成记录继续保留。");
    }
    if (CtrlFunctionKeyPressedOnce(VK_F9)) {
        g_diagnosticHud = !g_diagnosticHud;
        Log("[热键] Ctrl+F9：ShowDiagnosticWindow=%d", g_diagnosticHud);
    }
    if (CtrlFunctionKeyPressedOnce(VK_F10)) {
        g_worldMarkers = !g_worldMarkers;
        Log("[热键] Ctrl+F10：WorldMarkers=%d", g_worldMarkers);
    }
    if (CtrlFunctionKeyPressedOnce(VK_F11)) {
        g_debugShowRuntimeEntities = !g_debugShowRuntimeEntities;
        // 一旦用户主动按 Ctrl+F11，就不再让旧配置键强制保持开启，避免出现“按了却关不掉”的错觉。
        g_debugLegacyShowAllInteractiveEntities = false;
        Log("[热键] Ctrl+F11：SceneObjectDebug=%d", g_debugShowRuntimeEntities);
    }
}

// ============================================================================
// 13. Present Hook
// ============================================================================

// 这两组 6 字节既是本插件建立 trampoline 的原始指令，也是 RuntimeSDK ExclusivePatch 的 expected_bytes。
// 把它们放成唯一常量可以避免“本地护栏是一套字节、SDK 声明又手抄成另一套字节”的维护风险。
static const BYTE kPresentEntryBytes[6] = {0x83, 0xEC, 0x14, 0x56, 0x8B, 0xF1};
static const BYTE kExplorationEntryBytes[6] = {0x83, 0xEC, 0x08, 0x56, 0x8B, 0xF1};

// x86 的 E9 指令由 1 字节操作码 + 4 字节相对位移组成。
// 这里用 32 位无符号减法计算位移，正好对应 32 位地址空间的回绕语义；最后的 4 个字节
// 只是机器码，不需要把它当 C++ 有符号整数继续做算术，因此也避开了有符号溢出的未定义行为。
static void BuildRelativeJump5(uintptr_t from, uintptr_t to, BYTE out[5]) {
    const uint32_t from32 = static_cast<uint32_t>(from);
    const uint32_t to32 = static_cast<uint32_t>(to);
    const uint32_t displacement = to32 - (from32 + 5u);
    out[0] = 0xE9;
    std::memcpy(out + 1, &displacement, sizeof(displacement));
}

// Quest 两个原版入口都覆盖 6 字节，而 E9 只需要 5 字节，所以第 6 字节统一填 NOP。
// Standalone 与 Integrated 都调用同一个编码函数，保证两种模式最终写入 RPG.exe 的补丁完全相同。
static void BuildRelativeJump6(uintptr_t from, uintptr_t to, BYTE out[6]) {
    BuildRelativeJump5(from, to, out);
    out[5] = 0x90;
}

// Standalone 模式才允许直接改游戏代码页。Integrated 模式绝不能调用这个函数；
// 那种情况下相同的 6 字节 replacement 会作为 ExclusivePatch 声明交给 Castle_Runtime.dll。
static bool WriteRelativeJumpStandalone(void* from, void* to, size_t overwriteSize) {
    if (overwriteSize < 5) return false;
    DWORD oldProtect = 0;
    if (!VirtualProtect(from, overwriteSize, PAGE_EXECUTE_READWRITE, &oldProtect)) return false;

    BYTE patch[6] = {};
    BuildRelativeJump6(reinterpret_cast<uintptr_t>(from), reinterpret_cast<uintptr_t>(to), patch);
    std::memcpy(from, patch, overwriteSize >= sizeof(patch) ? sizeof(patch) : overwriteSize);
    if (overwriteSize > sizeof(patch)) {
        std::memset(static_cast<BYTE*>(from) + sizeof(patch), 0x90, overwriteSize - sizeof(patch));
    }

    FlushInstructionCache(GetCurrentProcess(), from, overwriteSize);
    DWORD ignored = 0;
    VirtualProtect(from, overwriteSize, oldProtect, &ignored);
    return true;
}

// trampoline 的作用可以理解为“被我们占掉函数门口以后，给原函数留一条侧门”。
// 前 6 字节先执行被覆盖的原版指令，第 7 字节开始再用 E9 跳回原函数 +6。
// Integrated 模式虽然不亲自写原函数门口，Hook 函数仍然需要这条 trampoline 去调用原函数。
static void* CreateEntryTrampoline(uintptr_t targetAddress, const BYTE originalBytes[6]) {
    BYTE* trampoline = static_cast<BYTE*>(VirtualAlloc(
        nullptr, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!trampoline) return nullptr;

    std::memcpy(trampoline, originalBytes, 6);
    BYTE jumpBack[5] = {};
    BuildRelativeJump5(reinterpret_cast<uintptr_t>(trampoline + 6), targetAddress + 6u, jumpBack);
    std::memcpy(trampoline + 6, jumpBack, sizeof(jumpBack));
    FlushInstructionCache(GetCurrentProcess(), trampoline, 11);
    return trampoline;
}

static bool InstallPresentHookStandalone();
static bool InstallExplorationUpdateHookStandalone();

// 0x409580 是原版探索管理器的一帧更新入口。
// Hook 内只发布 this 指针和序号，不枚举对象，不改任何原版字段。
static void __fastcall ExplorationUpdateHook(void* self, void*) {
    InterlockedExchangePointer(&g_explorationManager, self);
    InterlockedIncrement(&g_explorationCaptureSerial);
    if (g_originalExplorationUpdate) g_originalExplorationUpdate(self);
}

// 0x4064E0 是每帧 Present 入口。Overlay 先画到 renderer+8 的本帧 backbuffer，随后始终调用原版 Present。
// 任何任务插件绘制失败都不能阻断游戏自己的画面提交。
static void __fastcall PresentHook(void* self, void*) {
    ProcessHotkeys();
    DrawOverlayToBackSurface(self);
    if (g_originalPresent) g_originalPresent(self);
}

static bool ValidateOriginalBinarySurface() {
    // 两个入口字节与 SDK expected_bytes 共用同一组常量；另外两个只读函数继续做版本护栏。
    const BYTE getVarBytes[11] = {0x8B, 0x4C, 0x24, 0x04, 0x85, 0xC9, 0x75, 0x03, 0x33, 0xC0, 0xC3};
    const BYTE controlledBytes[5] = {0xA1, 0xF0, 0x8B, 0x46, 0x00};
    const bool presentOk = BytesEqual(Address::kPresent, kPresentEntryBytes, sizeof(kPresentEntryBytes));
    const bool explorationOk = BytesEqual(Address::kExplorationUpdate, kExplorationEntryBytes, sizeof(kExplorationEntryBytes));
    const bool getVarOk = BytesEqual(Address::kGetGameVar, getVarBytes, sizeof(getVarBytes));
    const bool controlledOk = BytesEqual(Address::kGetControlledEntity, controlledBytes, sizeof(controlledBytes));
    Log("[版本护栏] Present=%s ExplorationUpdate=%s GET_VAR=%s ControlledEntity=%s",
        presentOk ? "PASS" : "FAIL", explorationOk ? "PASS" : "FAIL",
        getVarOk ? "PASS" : "FAIL", controlledOk ? "PASS" : "FAIL");
    return presentOk && explorationOk && getVarOk && controlledOk;
}

// Integrated 模式下，Present 与 ExplorationUpdate 的入口已经由 Runtime 中央桥接，
// 因而这里不能再要求它们保持原版字节。Quest 自己只保留两条只读协议护栏。
static bool ValidateIntegratedReadProtocol() {
    const BYTE getVarBytes[11] = {0x8B, 0x4C, 0x24, 0x04, 0x85, 0xC9, 0x75, 0x03, 0x33, 0xC0, 0xC3};
    const BYTE controlledBytes[5] = {0xA1, 0xF0, 0x8B, 0x46, 0x00};
    const bool getVarOk = BytesEqual(Address::kGetGameVar, getVarBytes, sizeof(getVarBytes));
    const bool controlledOk = BytesEqual(Address::kGetControlledEntity, controlledBytes, sizeof(controlledBytes));
    Log("[版本护栏] Runtime已拥有Present/Exploration入口；Quest只读GET_VAR=%s ControlledEntity=%s。",
        getVarOk ? "PASS" : "FAIL", controlledOk ? "PASS" : "FAIL");
    return getVarOk && controlledOk;
}

// Standalone 探索入口安装。这里只有在 SDK Client 明确判定“同目录没有 Castle_Runtime.dll”后才会被调用。
static bool InstallExplorationUpdateHookStandalone() {
    if (g_explorationUpdateHookInstalled) return true;
    BYTE* target = reinterpret_cast<BYTE*>(Address::kExplorationUpdate);
    std::memcpy(g_explorationUpdateOriginalBytes, target, sizeof(g_explorationUpdateOriginalBytes));

    void* trampoline = CreateEntryTrampoline(Address::kExplorationUpdate, g_explorationUpdateOriginalBytes);
    if (!trampoline) {
        Log("[失败] Standalone ExplorationUpdate trampoline VirtualAlloc 失败。");
        return false;
    }
    if (!WriteRelativeJumpStandalone(target, reinterpret_cast<void*>(&ExplorationUpdateHook), 6)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        Log("[失败] Standalone ExplorationUpdate detour 写入失败。");
        return false;
    }

    g_explorationUpdateTrampoline = trampoline;
    g_originalExplorationUpdate = reinterpret_cast<ExplorationUpdateFn>(trampoline);
    g_explorationUpdateHookInstalled = true;
    Log("[Standalone] ExplorationUpdate Hook 已安装：0x00409580。");
    return true;
}

// Standalone Present 入口安装。写法与探索入口完全一致，避免两个 Hook 形成两套微妙不同的实现。
static bool InstallPresentHookStandalone() {
    if (g_presentHookInstalled) return true;
    BYTE* target = reinterpret_cast<BYTE*>(Address::kPresent);
    std::memcpy(g_presentOriginalBytes, target, sizeof(g_presentOriginalBytes));

    void* trampoline = CreateEntryTrampoline(Address::kPresent, g_presentOriginalBytes);
    if (!trampoline) {
        Log("[失败] Standalone Present trampoline VirtualAlloc 失败。");
        return false;
    }
    if (!WriteRelativeJumpStandalone(target, reinterpret_cast<void*>(&PresentHook), 6)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        Log("[失败] Standalone Present detour 写入失败。");
        return false;
    }

    g_presentTrampoline = trampoline;
    g_originalPresent = reinterpret_cast<PresentFn>(trampoline);
    g_presentHookInstalled = true;
    Log("[Standalone] Present Hook 已安装：0x004064E0。");
    return true;
}

// 下面两个恢复函数也只属于 Standalone。Integrated 的游戏代码由 Runtime 事务所有，
// Quest 绝不能在卸载路径再 VirtualProtect 抢回这两段地址，否则会破坏 Runtime 的所有权账本。
static void RemoveExplorationUpdateHookStandalone() {
    if (!g_explorationUpdateHookInstalled) return;
    void* target = reinterpret_cast<void*>(Address::kExplorationUpdate);
    DWORD oldProtect = 0;
    if (VirtualProtect(target, sizeof(g_explorationUpdateOriginalBytes), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::memcpy(target, g_explorationUpdateOriginalBytes, sizeof(g_explorationUpdateOriginalBytes));
        FlushInstructionCache(GetCurrentProcess(), target, sizeof(g_explorationUpdateOriginalBytes));
        DWORD ignored = 0;
        VirtualProtect(target, sizeof(g_explorationUpdateOriginalBytes), oldProtect, &ignored);
    }
    if (g_explorationUpdateTrampoline) VirtualFree(g_explorationUpdateTrampoline, 0, MEM_RELEASE);
    g_explorationUpdateTrampoline = nullptr;
    g_originalExplorationUpdate = nullptr;
    g_explorationUpdateHookInstalled = false;
    InterlockedExchangePointer(&g_explorationManager, nullptr);
    InterlockedExchange(&g_explorationCaptureSerial, 0);
    g_snapshotSceneId.clear();
    g_snapshotSceneGeneration = 0;
    g_sceneChangeCaptureSerial = 0;
    g_managerRejectedAtSceneChange = nullptr;
}

static void RemovePresentHookStandalone() {
    if (!g_presentHookInstalled) return;
    void* target = reinterpret_cast<void*>(Address::kPresent);
    DWORD oldProtect = 0;
    if (VirtualProtect(target, sizeof(g_presentOriginalBytes), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::memcpy(target, g_presentOriginalBytes, sizeof(g_presentOriginalBytes));
        FlushInstructionCache(GetCurrentProcess(), target, sizeof(g_presentOriginalBytes));
        DWORD ignored = 0;
        VirtualProtect(target, sizeof(g_presentOriginalBytes), oldProtect, &ignored);
    }
    if (g_presentTrampoline) VirtualFree(g_presentTrampoline, 0, MEM_RELEASE);
    g_presentTrampoline = nullptr;
    g_originalPresent = nullptr;
    g_presentHookInstalled = false;
}

// SDK 的字符串不是零结尾字符串指针，而是 data+length 的 UTF-8 视图。
// Runtime 会在需要长期保存时自行复制，所以这里传静态文字或调用期间仍存活的 label 即可。
static CastleStringView SdkStringView(const char* text, CastleU32 length) {
    CastleStringView view = {};
    view.data = text;
    view.length = length;
    return view;
}

// 查询 Runtime 子接口的统一小助手。每张函数表仍由调用点继续检查 magic/size，
// 这里只消除重复的 Query/Result 填充代码，不隐藏版本协商失败。
static const void* QueryRuntimeInterface(const CastleRuntimeApiV1* runtimeApi,
                                         const char* interfaceId,
                                         CastleU32 interfaceIdLength,
                                         CastleU32 version,
                                         CastleU32 minimumSize,
                                         CastleU32 requiredCapabilities) {
    CastleInterfaceQueryV1 query = {};
    CastleInterfaceResultV1 result = {};
    if (!runtimeApi || !runtimeApi->QueryInterface) return nullptr;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = SdkStringView(interfaceId, interfaceIdLength);
    query.requested_version = version;
    query.minimum_struct_size = minimumSize;
    query.required_capabilities_low = requiredCapabilities;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    return runtimeApi->QueryInterface(&query, &result) == CASTLE_OK ?
        result.api_pointer : nullptr;
}

// Runtime 的探索入口桥已经先保存本帧 manager，再调用这里。Quest 只复制一个已验证快照，
// 不再拥有 0x00409580 的 E9，也不会与相邻 BUGFix/Controller 补丁形成连续入口链。
static CastleResult CASTLE_RUNTIME_CALL QuestExplorationPhase(
    CastleTaskHandle task, void*) {
    CastleGameStateSnapshotV1 snapshot = {};
    (void)task;
    if (!g_runtimeGameStateApi) return CASTLE_ERROR_NOT_READY;
    snapshot.magic = CASTLE_GAME_SNAPSHOT_MAGIC;
    snapshot.struct_size = CASTLE_SIZEOF_GAME_STATE_SNAPSHOT_V1;
    snapshot.version = CASTLE_GAME_STATE_STRUCTURE_VERSION_1;
    CastleResult result = g_runtimeGameStateApi->GetSnapshot(&snapshot);
    if (result < 0 || snapshot.exploration_manager == 0u) return result;
    InterlockedExchangePointer(&g_explorationManager,
        reinterpret_cast<void*>(static_cast<uintptr_t>(snapshot.exploration_manager)));
    InterlockedIncrement(&g_explorationCaptureSerial);
    return CASTLE_OK;
}

// 所有正式叠加层都从 Runtime 的 renderer Present 桥进入。Quest 只画自己的像素，
// 返回后 Runtime 必定继续调用原版 Present，所以任务系统失败不会导致游戏黑屏。
static CastleResult CASTLE_RUNTIME_CALL QuestOverlayDraw(
    const CastleOverlayContextV1* context, void*) {
    if (!context || context->magic != CASTLE_OVERLAY_CONTEXT_MAGIC ||
        context->render_context == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    ProcessHotkeys();
    DrawOverlayToBackSurface(reinterpret_cast<void*>(
        static_cast<uintptr_t>(context->render_context)));
    return CASTLE_OK;
}

static CastleResult InstallIntegratedServices(const CastleRuntimeApiV1* runtimeApi,
                                              CastlePluginHandle pluginHandle) {
    static const char displayId[] = CASTLE_DISPLAY_INTERFACE_ID;
    static const char overlayId[] = CASTLE_OVERLAY_INTERFACE_ID;
    static const char scheduleId[] = CASTLE_SCHEDULE_INTERFACE_ID;
    static const char gameStateId[] = CASTLE_GAME_STATE_INTERFACE_ID;
    static const char overlayLabel[] = "Castle Quest overlay";
    static const char explorationLabel[] = "Castle Quest exploration capture";
    CastleOverlayClientV1 overlay = {};
    CastleScheduledTaskV1 phaseTask = {};
    CastleResult result;

    g_runtimeDisplayApi = static_cast<const CastleDisplayApiV1*>(QueryRuntimeInterface(
        runtimeApi, displayId, static_cast<CastleU32>(sizeof(displayId) - 1u),
        CASTLE_DISPLAY_API_VERSION_1, CASTLE_SIZEOF_DISPLAY_API_V1, 0u));
    g_runtimeOverlayApi = static_cast<const CastleOverlayApiV1*>(QueryRuntimeInterface(
        runtimeApi, overlayId, static_cast<CastleU32>(sizeof(overlayId) - 1u),
        CASTLE_OVERLAY_API_VERSION_1, CASTLE_SIZEOF_OVERLAY_API_V1,
        CASTLE_OVERLAY_CAP_BEFORE_RENDERER_PRESENT));
    g_runtimeScheduleApi = static_cast<const CastleScheduleApiV1*>(QueryRuntimeInterface(
        runtimeApi, scheduleId, static_cast<CastleU32>(sizeof(scheduleId) - 1u),
        CASTLE_SCHEDULE_API_VERSION_1, CASTLE_SIZEOF_SCHEDULE_API_V1,
        CASTLE_SCHEDULE_CAP_GAME_PHASE));
    g_runtimeGameStateApi = static_cast<const CastleGameStateApiV1*>(QueryRuntimeInterface(
        runtimeApi, gameStateId, static_cast<CastleU32>(sizeof(gameStateId) - 1u),
        CASTLE_GAME_STATE_API_VERSION_1, CASTLE_SIZEOF_GAME_STATE_API_V1,
        CASTLE_GAME_STATE_CAP_SNAPSHOT));
    if (!g_runtimeDisplayApi || !g_runtimeOverlayApi || !g_runtimeScheduleApi ||
        !g_runtimeGameStateApi) {
        Log("[RuntimeSDK] Display/Overlay/Schedule.GamePhase/GameState 任一接口缺失，Quest安全停用。");
        return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    }

    overlay.magic = CASTLE_OVERLAY_CLIENT_MAGIC;
    overlay.struct_size = CASTLE_SIZEOF_OVERLAY_CLIENT_V1;
    overlay.version = CASTLE_OVERLAY_STRUCTURE_VERSION_1;
    overlay.phase = CASTLE_OVERLAY_PHASE_BEFORE_PRESENT;
    overlay.priority = CASTLE_OVERLAY_PRIORITY_DEFAULT;
    overlay.draw = QuestOverlayDraw;
    overlay.label = SdkStringView(overlayLabel,
        static_cast<CastleU32>(sizeof(overlayLabel) - 1u));
    result = g_runtimeOverlayApi->RegisterOverlay(pluginHandle, &overlay,
                                                   &g_runtimeOverlayClient);
    if (result < 0) return result;

    phaseTask.magic = CASTLE_SCHEDULE_TASK_MAGIC;
    phaseTask.struct_size = CASTLE_SIZEOF_SCHEDULED_TASK_V1;
    phaseTask.version = CASTLE_SCHEDULE_STRUCTURE_VERSION_1;
    phaseTask.flags = CASTLE_SCHEDULE_TASK_START_ENABLED |
                      CASTLE_SCHEDULE_TASK_GAME_EXPLORATION;
    phaseTask.period_ms = 0u;
    phaseTask.budget_ms = 2u;
    phaseTask.phase = CASTLE_SCHEDULE_PHASE_NORMAL;
    phaseTask.priority = CASTLE_SCHEDULE_PRIORITY_DEFAULT;
    phaseTask.callback = QuestExplorationPhase;
    phaseTask.label = SdkStringView(explorationLabel,
        static_cast<CastleU32>(sizeof(explorationLabel) - 1u));
    result = g_runtimeScheduleApi->RegisterGamePhaseCallback(
        pluginHandle, &phaseTask, &g_runtimeExplorationTask);
    if (result < 0) {
        g_runtimeOverlayApi->UnregisterOverlay(g_runtimeOverlayClient);
        g_runtimeOverlayClient = 0u;
        return result;
    }
    result = g_runtimeOverlayApi->SetOverlayReady(g_runtimeOverlayClient, 1u);
    if (result < 0) {
        g_runtimeScheduleApi->UnregisterPeriodicTask(g_runtimeExplorationTask);
        g_runtimeExplorationTask = 0u;
        g_runtimeOverlayApi->UnregisterOverlay(g_runtimeOverlayClient);
        g_runtimeOverlayClient = 0u;
        return result;
    }
    g_hookOwnerMode = HookOwnerMode::Integrated;
    Log("[RuntimeSDK] Quest 已接入 GamePhase/Overlay/Display/GameState；不再拥有两个入口 Hook。");
    return CASTLE_OK;
}

// ============================================================================
// 14. RuntimeSDK 生命周期与独立模式
// ============================================================================

// 文件/任务数据库这部分业务在 Integrated 与 Standalone 完全共用。
// 模式差别只应该体现在“游戏代码写入由谁拥有”，不能复制出两套 Quest 业务逻辑。
static bool InitializeBusinessCore() {
    if (g_businessInitialized) return true;
    g_moduleDir = GetModuleDirectory(g_module);
    OpenLog();
    Log("[装载] RuntimeSDK Client 已离开 Loader Lock，开始 Castle_Quest 业务初始化。");
    Log("[基线] 目标原版 RPG.exe.org SHA-256 = 8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f");
    Log("[架构] WorldPtr=0x00978508 Camera=(0x00978514,0x00978518)；任务系统不写 GameVar/Item/Event/TSF。");
    Log("[任务格式] manifest/Base=format_version 7；Addon=format_version 2；Base [[stage]] 天然表示原版真实 Stage，不再存在 stage_type。");
    LoadMainConfig();
    if (!LoadQuestDatabase()) {
        // 与旧版行为一致：数据库读取失败时保留空/上一份有效数据库，Overlay Hook 本身仍可安全安装。
        // 这样 Ctrl+F8 修复 TOML 后可以在不重启游戏的情况下恢复，而不是让插件永久失去入口。
        Log("[任务数据库] 启动读取失败；Hook 仍可启动，等待用户修复后 Ctrl+F8 事务重载。");
    }
    g_businessInitialized = true;
    return true;
}

static CastleResult CASTLE_RUNTIME_CALL QuestStandaloneInitialize(void*) {
    if (!InitializeBusinessCore()) return CASTLE_ERROR_RUNTIME_FAULT;
    Log("[模式] Standalone：同目录没有 Castle_Runtime.dll；允许使用 Quest 自己的两处已验证入口 Hook。");
    if (!ValidateOriginalBinarySurface()) {
        Log("[失败] Standalone 关键机器码不匹配；两个本地 Hook 均不安装。");
        return CASTLE_ERROR_EXPECTED_BYTES;
    }

    if (!InstallExplorationUpdateHookStandalone()) return CASTLE_ERROR_EXPECTED_BYTES;
    if (!InstallPresentHookStandalone()) {
        // 第二个 Hook 失败时必须立即恢复第一个，不能留下“半安装”状态。
        RemoveExplorationUpdateHookStandalone();
        return CASTLE_ERROR_EXPECTED_BYTES;
    }
    g_hookOwnerMode = HookOwnerMode::Standalone;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL QuestIntegratedInitialize(
    const CastleRuntimeApiV1* runtimeApi, CastlePluginHandle pluginHandle, void*) {
    g_runtimeLogApi = static_cast<const CastleLogApiV1*>(QueryRuntimeInterface(
        runtimeApi, CASTLE_LOG_INTERFACE_ID,
        static_cast<CastleU32>(sizeof(CASTLE_LOG_INTERFACE_ID) - 1u),
        CASTLE_LOG_API_VERSION_1, CASTLE_SIZEOF_LOG_API_V1, 0u));
    g_runtimeLogPlugin = pluginHandle;
    if (!g_runtimeLogApi) return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    if (!InitializeBusinessCore()) return CASTLE_ERROR_RUNTIME_FAULT;
    Log("[模式] Integrated：Castle_Runtime.dll 可用；Quest 不直接修改 RPG.exe 代码段。");
    if (!ValidateIntegratedReadProtocol()) {
        Log("[失败] Integrated 只读协议不匹配；Quest安全停用。");
        return CASTLE_ERROR_EXPECTED_BYTES;
    }
    return InstallIntegratedServices(runtimeApi, pluginHandle);
}

static void CASTLE_RUNTIME_CALL QuestRuntimeFault(CastleResult failure, void*) {
    (void)failure;
    // Runtime 不可用时没有合法的 mods\logs 出口；官方 Quest 保持停用，不创建旁路日志。
}

static void CloseBusinessResources() {
    if (g_font) {
        DeleteObject(g_font);
        g_font = nullptr;
    }
    if (g_runtimeLogApi && g_runtimeLogPlugin) {
        LogRaw("[结束] Castle_Quest 进入进程退出/卸载收尾。");
    } else if (g_log != INVALID_HANDLE_VALUE) {
        LogRaw("[结束] Castle_Quest 进入进程退出/卸载收尾。");
        CloseHandle(g_log);
        g_log = INVALID_HANDLE_VALUE;
    }
    g_runtimeLogApi = nullptr;
    g_runtimeLogPlugin = 0u;
}

static void CASTLE_RUNTIME_CALL QuestProcessExit(void*) {
    // SDK Client 的 process_exit 回调不区分“整个进程退出”和“显式卸载”。
    // Standalone 的代码补丁归 Quest 自己所有，所以必须恢复，避免显式 FreeLibrary 后 RPG.exe 仍跳进已卸载模块。
    if (g_hookOwnerMode == HookOwnerMode::Standalone) {
        RemovePresentHookStandalone();
        RemoveExplorationUpdateHookStandalone();
    }
    // Integrated 的补丁属于 Runtime 事务；这里绝不 VirtualProtect 回写，也不释放仍可能被入口引用的 trampoline。
    // 主 Runtime 采用固定驻留模型，进程结束时由 Windows 一次回收模块和执行页。
    CloseBusinessResources();
}

// 稳定 plugin_id 是 Runtime 的真正身份；ASI 文件名以后即使被用户改名也不能改变这个 ID。
static const char g_pluginId[] = "org.castlereforge.quest";
static const char g_displayName[] = "Castle Quest";
static const char g_versionText[] = "0.1-dev6zd";
static const char g_buildId[] = "quest-dev6zd-runtimesdk-v1";

static const CastlePluginDescriptorV1 g_pluginDescriptor = {
    CASTLE_PLUGIN_DESC_MAGIC,
    CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE,
    0u,
    {g_pluginId, static_cast<CastleU32>(sizeof(g_pluginId) - 1u)},
    {g_displayName, static_cast<CastleU32>(sizeof(g_displayName) - 1u)},
    {g_versionText, static_cast<CastleU32>(sizeof(g_versionText) - 1u)},
    {g_buildId, static_cast<CastleU32>(sizeof(g_buildId) - 1u)}
};

static const CastleRuntimeClientConfigV1 g_clientConfig = {
    CASTLE_CLIENT_CONFIG_MAGIC,
    CASTLE_SIZEOF_CLIENT_CONFIG_V1,
    CASTLE_CLIENT_CONFIG_VERSION_1,
    CASTLE_CLIENT_FLAG_REQUIRE_RUNTIME,
    QuestIntegratedInitialize,
    QuestStandaloneInitialize,
    QuestRuntimeFault,
    QuestProcessExit,
    nullptr
};

// 这张表不是 const，因为 Client 在 PROCESS_ATTACH 会把自己的 Entry Gate thunk 和 Bootstrap 函数地址填进去。
static CastlePluginExportV1 g_pluginExport = {
    CASTLE_PLUGIN_QUERY_MAGIC,
    CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1,
    0u,
    &g_pluginDescriptor,
    &g_clientConfig,
    0u,
    nullptr
};

} // namespace CastleQuest

// RuntimeSDK 标准插件查询入口。extern "C" 禁止 C++ 名字改编，.def 再把最终导出名固定为无修饰 ABI 名称。
extern "C" const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(CastleU32 requestedVersion) {
    using namespace CastleQuest;
    return requestedVersion == CASTLE_PLUGIN_EXPORT_VERSION_1 ? &g_pluginExport : nullptr;
}

// 旧 Castle Mod Loader 与 RuntimeSDK 两阶段 Loader 都会调用 InitializeASI。
// 正式初始化是否 Integrated/Standalone/Fault 全由 Client 状态机决定，这里不能自己再安装任何 Hook。
extern "C" void __cdecl InitializeASI(void) {
    CastleRuntimeClient_RunNow();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    using namespace CastleQuest;
    if (reason == DLL_PROCESS_ATTACH) {
        // 与主项目官方 SDK ASI 统一：Loader Lock 内只登记 Client/Entry Gate，不读文件、不写日志、不装业务 Hook。
        g_module = module;
        CastleRuntimeClient_OnProcessAttach(static_cast<CastleModule>(reinterpret_cast<uintptr_t>(module)),
                                            &g_pluginExport);
    } else if (reason == DLL_PROCESS_DETACH) {
        // Client 负责把 detach 转成统一的 process_exit 回调；Quest 不在这里绕开 SDK 自己清理游戏代码。
        CastleRuntimeClient_OnProcessDetach(reserved);
    }
    return TRUE;
}
