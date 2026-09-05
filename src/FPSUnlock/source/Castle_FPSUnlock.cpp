// ============================================================================
// Castle_FPSUnlock.cpp
// 《幽城幻剑录》Castle_FPSUnlock v1.3
// 目标：保留原版约 20 Hz Legacy Logic，同时让“普通地图自由探索”的世界画面
//      以约 60 Hz 重绘，并对 Camera / 当前 Camera-follow 对象做仅渲染层预测。
//
// 这不是“把游戏的 20 改成 60”。那样会让脚本 WAIT、战斗 Tick、动画倒计时、
// AI、RNG 等全部加速。本文件把“逻辑时钟”和“渲染时钟”严格拆开：
//
//   1. 原版 0x44A970 仍然只由原版 WM_TIMER / pending 机制约每 50 ms 调一次。
//   2. 原版 0x4345F0 Legacy Update 绝不额外调用。
//   3. 原版完整 Draw Queue（GUI、SF2、菜单、HUD 等）仍然只由原版帧消费一次。
//   4. 在原版完整队列被消费前，扫描“混合队列”，只复制其中 draw=0x40B050 的
//      Scene World Manager 条目；其它 GUI/SF2/HUD 条目允许存在，但绝不复制。
//   5. +17 ms / +33 ms 的额外帧只临时恢复 world-only synthetic queue，再临时预测
//      Camera 与当前跟随对象坐标；绘制后立即恢复真实 20 Hz 坐标。
//   6. 对话、菜单、Battle、Bink、标题、Camera shake、特殊 world overlay 等任何
//      尚未证明为纯表现层的状态，全部自动退回原版 20 Hz 绘制。
//
// v1.3 修复 v1.2 的第二个关键错误：
// v1.2 已经能从混合 Draw Queue 正确找到 Scene World Manager，但它又把
// “必须先拥有两帧可靠运动历史”错误地当成“是否允许安排额外 Present”的前提。
// 用户实机日志因此出现：找到 world，却长期 ExtraPresent=0，同时
// “等待运动历史”持续增长。进一步核对 0x40B050 机器码后还确认：world 对象
// +0x280 只是传给 0x40ADE0 的内部渲染子对象指针，没有证据能把它当作稳定的
// scene identity。v1.2 用它判断“是否换场景”，会让运动历史有机会被每 Tick 清空。
//
// v1.3 改成两条硬规则：
// 1. 只要当前是安全的普通 world 帧，就必须安排 +17/+33 ms 的额外 Present；
//    运动历史只决定“这两帧能不能做位置插值”，绝不再决定“有没有额外帧”。
// 2. 场景连续性使用 Scene World Manager 对象指针本身判断；Camera 与 follower
//    的历史分开。follower 指针变化时只让 follower 暂停一帧插值，不再把 Camera
//    历史一起清空。
//
// 为什么仍然不重放完整队列：
// v1.0 实机已经证明完整队列中的某些 GUI draw 会推进内部相位，重复 draw 会让闪烁
// 频率翻倍。v1.3 继续坚持 world-only 重放，GUI/SF2/HUD 不会因为 FPSUnlock 被额外
// 调用；只有明确的 scene world manager 才能进入额外帧。
//
// 宽屏兼容：
// 当前 Castle_Widescreen v0.11 会修改 0x44A9C6(RenderQueue CALL) 和
// 0x44A9E6(Present CALL)。本插件绝不改这两个 CALL，只在额外帧真正需要绘制时
// 动态读取它们“现在指向谁”。因此：
// - FPSUnlock 先加载：宽屏后续仍能看到自己的原版目标并正常安装；
// - 宽屏先加载：FPSUnlock 会在额外帧进入宽屏的 Render/Present Hook；
// 两个插件不再争夺相同机器码。
// ============================================================================

// ----------------------------------------------------------------------------
// 1. 最小 Win32 类型
// ----------------------------------------------------------------------------
// 不包含 windows.h，也不链接 CRT。这样最终 ASI 可以保持非常小，并避免给 2002
// 年的 32 位游戏额外增加现代运行库依赖。

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* LPVOID;
typedef const char* LPCSTR;
typedef char* LPSTR;
typedef unsigned long SIZE_T;

#include "CastleRuntime_Client.h"
#include "CastleHook_API.h"
#include "CastleClock_API.h"
#include "CastleRender_API.h"
#include "CastleDisplay_API.h"
#include "CastleGameState_API.h"
#include "CastleModule_API.h"
#include "CastlePath_API.h"
#include "CastleLog_API.h"

static const BOOL FALSE_VALUE = 0;
static const BOOL TRUE_VALUE = 1;
static const DWORD DLL_PROCESS_DETACH_VALUE = 0;
static const DWORD DLL_PROCESS_ATTACH_VALUE = 1;
static const DWORD QS_ALLINPUT_VALUE = 0x04FF;
static const DWORD MWMO_INPUTAVAILABLE_VALUE = 0x0004;

// ----------------------------------------------------------------------------
// 2. 已确认 RPG.exe 地址
// ----------------------------------------------------------------------------
// 目标基线：用户上传的台湾第三版 RPG.exe.org
// SHA-256 = 8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f
// 文件大小 = 462,848 bytes。

// 原版主循环：0x40171C 是“真正执行一整个 20 Hz Legacy Frame”的 CALL。
// 原目标是 0x44A970。v1.3 Hook 这一处，而不是去抢宽屏的 Render/Present CALL。
static const DWORD ADDR_CALL_LEGACY_FRAME = 0x0040171C;
static const DWORD ADDR_GAME_LEGACY_FRAME = 0x0044A970;

// 0x44A9BF 原版 call 0x4020A0，位于 Legacy Update 之后、RenderQueue 之前。
// 这个位置非常适合做“只读快照”：逻辑已经更新完，draw queue 也已建立，但尚未消费。
static const DWORD ADDR_CALL_PRE_DRAW = 0x0044A9BF;
static const DWORD ADDR_GAME_PRE_DRAW = 0x004020A0;

// 主 RenderQueue/Present CALL 从本版起由 Castle_Runtime 唯一拥有；本插件不再保存其地址。

// Scene World Manager 的 vtable[1] draw 方法。
// 宽屏 v0.11 同样用这个入口识别普通地图 world manager。
static const DWORD FN_SCENE_WORLD_DRAW = 0x0040B050;

// BattleManager 的 vtable[1] draw。宽屏 v0.11 已经通过 ctor/vtable/调用链闭合：
// BattleManager 出现在实时 draw queue 时，战斗画面必须维持原版 20 Hz 绘制，
// 不能让 FPSUnlock 把 world manager 在战斗中额外重放。
static const DWORD FN_BATTLE_MANAGER_DRAW = 0x004429F0;

// 排序绘制队列：count + 最多 200 项，每项 8 bytes = 排序键 + 对象指针。
static const DWORD GLOBAL_DRAW_QUEUE_COUNT = 0x008DDA38;
static const DWORD GLOBAL_DRAW_QUEUE_ENTRIES = 0x008DDA40;
static const DWORD DRAW_QUEUE_ENTRY_BYTES = 8;
static const DWORD DRAW_QUEUE_MAX_ENTRIES = 200;

// 原版主 manager 与 Display / DirectDraw manager。
static const DWORD GLOBAL_MAIN_MANAGER = 0x008E24F4;
static const DWORD GLOBAL_DISPLAY_OBJECT = 0x0089F6C0;

// 原版主循环自己的三个 gate。
static const DWORD GLOBAL_MAIN_ACTIVE_A = 0x0046F38C;
static const DWORD GLOBAL_LEGACY_PENDING = 0x0046F38D;
static const DWORD GLOBAL_MAIN_ACTIVE_B = 0x004680BC;
static const DWORD GLOBAL_FRAME_GATE_OBJECT = 0x0046F390;

// 原版特殊转场 / 抓帧状态。非零时额外 world frame 一律停用。
static const DWORD GLOBAL_TRANSITION_ACTIVE = 0x0046F3A4;
static const DWORD GLOBAL_DEBUG_DRAW = 0x00978504;
static const DWORD GLOBAL_CAPTURE_PENDING = 0x00978505;

// 消息系统。Scene World Draw(0x40B050) 内部会调用 0x403E30 + 0x404800，
// 而这两个函数会推进/绘制消息状态，因此任何消息存在时都不能重复调用 world draw。
static const DWORD GLOBAL_MESSAGE_TARGET_STATE = 0x0046F678;
static const DWORD GLOBAL_MESSAGE_CURRENT_STATE = 0x0046F679;

// Camera 数据。0x44B300 建边界；0x44B360 根据 0x978550 跟随对象 +0x10/+0x14
// 更新 Camera X/Y。这里只在额外 Render 的极短窗口临时写入预测值。
static const DWORD GLOBAL_CAMERA_X = 0x00978514;
static const DWORD GLOBAL_CAMERA_Y = 0x00978518;
static const DWORD GLOBAL_CAMERA_VIEW_W = 0x0097851C;
static const DWORD GLOBAL_CAMERA_VIEW_H = 0x00978520;
static const DWORD GLOBAL_CAMERA_MIN_X = 0x00978524;
static const DWORD GLOBAL_CAMERA_MIN_Y = 0x00978528;
static const DWORD GLOBAL_CAMERA_MAX_X = 0x0097852C;
static const DWORD GLOBAL_CAMERA_MAX_Y = 0x00978530;
static const DWORD GLOBAL_CAMERA_FOLLOW_OBJECT = 0x00978550;
static const DWORD FOLLOW_OBJECT_X_OFFSET = 0x10;
static const DWORD FOLLOW_OBJECT_Y_OFFSET = 0x14;

// 0x40B050 开头会调用 0x44B500；Camera shake 开启时，0x44B500 会增加计数，
// 甚至调用游戏 RNG。为了严格保证“逻辑帧不变”，shake 时完全不插额外帧。
static const DWORD GLOBAL_CAMERA_SHAKE_ACTIVE = 0x00978534;

// 0x40B050 在 0x89F7B8 > 0 时会走另一条 0x407020 路径；0x408810 还受 0x468BB8
// 控制。它们的完整副作用边界尚未像普通 Scene runtime 一样闭合，所以 v1.3 直接跳过。
static const DWORD GLOBAL_SCENE_RENDER_OVERRIDE_COUNT = 0x0089F7B8;
static const DWORD GLOBAL_WORLD_EXTRA_OVERLAY_ACTIVE = 0x00468BB8;

// WaitMessage IAT：游戏没有 pending work 时原本睡在这里。替换这个 IAT 后，仍在主线程
// 中等待消息，但最多等到下一个 60 Hz extra frame 的 deadline。
static const DWORD ADDR_IAT_WAIT_MESSAGE = 0x00460198;

// 借用 RPG.exe 已经存在的 Win32 IAT，避免 ASI 自己产生 Import Directory。
static const DWORD ADDR_IAT_OUTPUT_DEBUG_STRING_A = 0x00460118;

// ----------------------------------------------------------------------------
// 3. Win32 / 游戏内部函数类型
// ----------------------------------------------------------------------------

typedef void (__stdcall* OutputDebugStringAFn)(LPCSTR);
typedef BOOL (__stdcall* WaitMessageFn)();
typedef DWORD (__stdcall* MsgWaitForMultipleObjectsExFn)(DWORD, const HANDLE*, DWORD, DWORD, DWORD);
typedef unsigned int (__stdcall* GetPrivateProfileIntAFn)(LPCSTR, LPCSTR, int, LPCSTR);

typedef void (__thiscall* GameThisVoidFn)(LPVOID);
typedef void (__cdecl* GameCdeclVoidFn)();

// ----------------------------------------------------------------------------
// 4. 小型 CRT 替代函数
// ----------------------------------------------------------------------------
// 优化器可能把简单循环变成 memcpy/memset/strlen 调用。我们自己提供最小实现，链接时
// 就可以继续使用 /nodefaultlib，而不需要 msvcrt/ucrt。

/* memcpy/memset 由 RuntimeSDK Client 的共用无 CRT 支持对象提供，避免每个 ASI 复制一份。 */
extern "C" void* __cdecl memcpy(void* destination, const void* source, SIZE_T count);
extern "C" void* __cdecl memset(void* destination, int value, SIZE_T count);

extern "C" SIZE_T strlen(const char* text) {
    SIZE_T length = 0;
    if (!text) return 0;
    while (text[length] != '\0') ++length;
    return length;
}

// ----------------------------------------------------------------------------
// 5. 全局状态
// ----------------------------------------------------------------------------

static HINSTANCE g_module_instance = (HINSTANCE)0;
static char g_ini_path[260];

static MsgWaitForMultipleObjectsExFn g_msg_wait_for_multiple_objects_ex = (MsgWaitForMultipleObjectsExFn)0;
static GetPrivateProfileIntAFn g_get_private_profile_int_a = (GetPrivateProfileIntAFn)0;
static WaitMessageFn g_original_wait_message = (WaitMessageFn)0;
static GameThisVoidFn g_next_legacy_frame = (GameThisVoidFn)0;
static GameCdeclVoidFn g_next_pre_draw = (GameCdeclVoidFn)0;

static volatile BOOL g_enabled = FALSE_VALUE;
static volatile BOOL g_hooks_installed = FALSE_VALUE;
static volatile BOOL g_inside_extra_render = FALSE_VALUE;
static BOOL g_timer_resolution_raised = FALSE_VALUE;
static const CastleRuntimeApiV1* g_runtime_api = (const CastleRuntimeApiV1*)0;
static CastlePluginHandle g_runtime_plugin = 0;
static const CastleHookApiV1* g_hook_api = (const CastleHookApiV1*)0;
static const CastleClockApiV1* g_clock_api = (const CastleClockApiV1*)0;
static const CastleRenderApiV1* g_render_api = (const CastleRenderApiV1*)0;
static const CastleDisplayApiV1* g_display_api = (const CastleDisplayApiV1*)0;
static const CastleGameStateApiV1* g_game_state_api = (const CastleGameStateApiV1*)0;
static const CastleModuleApiV1* g_module_api = (const CastleModuleApiV1*)0;
static const CastlePathApiV1* g_path_api = (const CastlePathApiV1*)0;
static const CastleLogApiV1* g_log_api = (const CastleLogApiV1*)0;
static CastleLeaseHandle g_clock_lease = 0;
static CastleClaimHandle g_legacy_claim = 0;
static CastleClaimHandle g_pre_draw_claim = 0;
static CastleClaimHandle g_wait_claim = 0;

// INI：默认 60 FPS = 原版 20 FPS + 每 50 ms 再补 2 帧。
static BOOL g_config_enable = TRUE_VALUE;
static BOOL g_config_log_stats = TRUE_VALUE;
static DWORD g_config_target_fps = 60;
static LONG g_config_max_prediction_step = 96;

// 一个 50 ms 周期内的额外帧调度状态。
static volatile DWORD g_base_frame_time_ms = 0;
static volatile DWORD g_next_extra_time_ms = 0;
static volatile DWORD g_extra_stage = 0; // 0=本周期无额外帧；1=等待第1帧；2=等待第2帧。

// 统计只写日志，不参与游戏判断。
// v1.3 继续保留“无 world / Battle / 运行态保护”等统计，并新增“无插值历史但仍补帧”的计数：
// - 无world：这帧根本没有 Scene World Manager，通常是标题/Bink/切换中间态。
// - Battle：实时队列明确出现 BattleManager，因此主动保持原版20Hz。
static volatile DWORD g_legacy_frame_count = 0;
static volatile DWORD g_extra_present_count = 0;
static volatile DWORD g_skip_no_world = 0;
static volatile DWORD g_skip_battle = 0;
static volatile DWORD g_skip_runtime_gate = 0;
static volatile DWORD g_extra_without_prediction = 0;
static volatile DWORD g_last_stats_log_ms = 0;

// v1.3 继续允许“完整原始队列是混合队列”。
// 这里保存的是“从混合原始队列里筛出来的所有 Scene World Manager 条目”。
// 每个条目仍然只有 8 bytes：+0 排序键、+4 对象指针。
// GUI/SF2/HUD 等其它条目从不复制进这个缓冲区，因此额外帧不会调用它们。
static BYTE g_scene_world_entries[DRAW_QUEUE_MAX_ENTRIES * DRAW_QUEUE_ENTRY_BYTES];
static DWORD g_scene_world_entry_count = 0;
static LPVOID g_scene_world_object = (LPVOID)0;
static BOOL g_frame_has_world = FALSE_VALUE;
static BOOL g_frame_has_battle = FALSE_VALUE;

// MotionState 记录每个真实 20 Hz Tick 完成后的状态。
// extra frame 只根据两个已发生的真实状态估算速度，不会写回逻辑结果。
typedef struct MotionState {
    LONG camera_x;
    LONG camera_y;

    // 当前 Camera 跟随对象。普通自由探索时通常就是玩家角色；某些剧情/切换时可能为空
    // 或更换对象，所以 follower 的插值资格必须独立判断，不能拖累 Camera 本身。
    LPVOID follow_object;
    LONG follow_x;
    LONG follow_y;
    BOOL follow_valid;

    // 用真正的 Scene World Manager 对象指针作为“这一帧属于同一个世界管理器”的身份。
    // 不能再使用 object+0x280：机器码只证明 +0x280 是传给 0x40ADE0 的内部子对象指针，
    // 并没有证明它在连续 Legacy Tick 之间恒定。
    LPVOID world_object;
} MotionState;

static MotionState g_previous_motion;
static MotionState g_current_motion;
static BOOL g_have_current_motion = FALSE_VALUE;

// Camera 与 follower 分开记录插值资格。
// Camera 只要同一个 world manager 连续出现两帧就可以预测；
// follower 还额外要求前后两帧指向同一个对象。
static BOOL g_camera_prediction_ready = FALSE_VALUE;
static BOOL g_follow_prediction_ready = FALSE_VALUE;

// ----------------------------------------------------------------------------
// 6. 不依赖 CRT 的字符串 / 日志工具
// ----------------------------------------------------------------------------

static DWORD StringLength(const char* text) {
    DWORD n = 0;
    if (!text) return 0;
    while (text[n] != '\0') ++n;
    return n;
}

static DWORD CopyString(char* dst, DWORD capacity, const char* src) {
    DWORD i = 0;
    if (!dst || capacity == 0) return 0;
    if (!src) {
        dst[0] = '\0';
        return 0;
    }
    while (i + 1 < capacity && src[i] != '\0') {
        dst[i] = src[i];
        ++i;
    }
    dst[i] = '\0';
    return i;
}

static void AppendString(char* dst, DWORD capacity, const char* src) {
    DWORD length = StringLength(dst);
    DWORD i = 0;
    if (!src || length >= capacity) return;
    while (length + i + 1 < capacity && src[i] != '\0') {
        dst[length + i] = src[i];
        ++i;
    }
    dst[length + i] = '\0';
}

static void AppendUnsignedDecimal(char* dst, DWORD capacity, DWORD value) {
    char temp[16];
    DWORD count = 0;
    if (value == 0) {
        AppendString(dst, capacity, "0");
        return;
    }
    while (value != 0 && count < 15) {
        temp[count++] = (char)('0' + (value % 10));
        value /= 10;
    }
    while (count != 0) {
        char one[2];
        one[0] = temp[--count];
        one[1] = '\0';
        AppendString(dst, capacity, one);
    }
}

static void LogText(const char* text) {
    OutputDebugStringAFn debug = *(OutputDebugStringAFn*)ADDR_IAT_OUTPUT_DEBUG_STRING_A;
    if (debug) debug(text);
}

static void LogLine(const char* text) {
    if (g_log_api && g_runtime_plugin && text) {
        CastleLogRecordV1 record;
        memset(&record, 0, sizeof(record));
        record.magic = CASTLE_LOG_RECORD_MAGIC;
        record.struct_size = CASTLE_SIZEOF_LOG_RECORD_V1;
        record.version = CASTLE_LOG_STRUCTURE_VERSION_1;
        record.level = CASTLE_LOG_INFO;
        record.message.data = text;
        record.message.length = StringLength(text);
        if (record.message.length != 0u &&
            g_log_api->WritePluginLine(g_runtime_plugin, &record) >= 0) return;
    }
    LogText(text);
    LogText("\r\n");
}

static BOOL BytesEqual(DWORD address, const BYTE* expected, DWORD size) {
    const BYTE* actual = (const BYTE*)address;
    DWORD i = 0;
    while (i < size) {
        if (actual[i] != expected[i]) return FALSE_VALUE;
        ++i;
    }
    return TRUE_VALUE;
}

static BOOL TimeReached(DWORD now, DWORD deadline) {
    return ((LONG)(now - deadline) >= 0) ? TRUE_VALUE : FALSE_VALUE;
}

static DWORD RuntimeNowMilliseconds() {
    CastleU32 now = 0u;
    if (g_clock_api && g_clock_api->GetMonotonicMilliseconds(&now) >= 0) return now;
    return g_base_frame_time_ms;
}

// ----------------------------------------------------------------------------
// 7. 读取 draw queue / motion state
// ----------------------------------------------------------------------------

static DWORD ObjectDrawMethod(LPVOID object) {
    if (!object) return 0;
    DWORD* vtable = *(DWORD**)object;
    if (!vtable) return 0;
    return vtable[1];
}

// 从“混合 draw queue”中只提取 Scene World Manager 条目。
//
// v1.1 的错误就在这里：它要求原始队列里所有有效对象都必须是 world manager。
// 实机证明正常自由探索的完整队列也会带着其它常驻绘制对象，所以那个条件会把几乎
// 所有正常帧都误杀，最终日志长期 ExtraPresent=0。
//
// v1.3 延续与 Castle_Widescreen v0.11 已实机验证的协议：
// 1. 原始队列可以是“world + GUI/SF2/其它对象”的混合队列；
// 2. 这里只复制 draw=0x40B050 的 world 条目；
// 3. 其它对象既不会导致扫描失败，也绝不会被复制到额外帧；
// 4. 如果明确看到 BattleManager，则记录下来，让本帧在更上层保持原版20Hz；
// 5. 如果一个 world 条目都找不到，才说明这帧不具备普通世界补帧基础。
static BOOL CaptureWorldEntriesFromMixedQueue() {
    DWORD count = *(volatile DWORD*)GLOBAL_DRAW_QUEUE_COUNT;
    DWORD i;
    DWORD world_count = 0;
    LPVOID first_scene_object = (LPVOID)0;

    g_scene_world_entry_count = 0;
    g_frame_has_battle = FALSE_VALUE;

    if (count == 0 || count > DRAW_QUEUE_MAX_ENTRIES) return FALSE_VALUE;

    for (i = 0; i < count; ++i) {
        BYTE* entry = (BYTE*)GLOBAL_DRAW_QUEUE_ENTRIES + i * DRAW_QUEUE_ENTRY_BYTES;
        LPVOID object = *(LPVOID*)(entry + 4);
        DWORD draw_method;

        // 空对象只是队列中的无效槽位，不应该让整个扫描失败。
        if (!object) continue;

        draw_method = ObjectDrawMethod(object);

        // BattleManager 是一个明确的“本帧不要补世界帧”信号。
        // 仍然继续扫描剩余条目，是为了日志/状态快照保持完整；真正的 gate 在后面统一处理。
        if (draw_method == FN_BATTLE_MANAGER_DRAW) {
            g_frame_has_battle = TRUE_VALUE;
            continue;
        }

        if (draw_method != FN_SCENE_WORLD_DRAW) {
            // GUI、SF2、HUD、光标或其它表现对象允许存在于原始完整队列。
            // 关键是：它们绝不会被复制进 g_scene_world_entries。
            continue;
        }

        // 理论上通常只有一个 world manager，但宽屏的稳定实现是“筛出所有 world 条目”。
        // 我们沿用同样规则，避免因为某些场景短暂出现多个 world 条目再次误杀正常帧。
        if (world_count >= DRAW_QUEUE_MAX_ENTRIES) return FALSE_VALUE;
        memcpy(g_scene_world_entries + world_count * DRAW_QUEUE_ENTRY_BYTES,
               entry, DRAW_QUEUE_ENTRY_BYTES);
        ++world_count;

        // Motion 插值只需要一个主 world manager 来标识“当前连续世界对象”。
        // 选择第一个合法对象即可；额外 Render 时仍会恢复全部筛出的 world 条目。
        if (!first_scene_object) {
            // Scene World Manager 自身是长生命周期对象，直接保存它的对象指针即可。
            // 不再读取 object+0x280 充当“scene identity”。
            first_scene_object = object;
        }
    }

    if (world_count == 0 || !first_scene_object) return FALSE_VALUE;

    // 0x40B050 开头会读取 world manager +0x219。v1.1 已把非零视为特殊 world 状态；
    // 这个边界继续保留，避免在尚未闭合副作用的 world 分支里强行补帧。
    if (*((volatile BYTE*)first_scene_object + 0x219) != 0) return FALSE_VALUE;

    g_scene_world_entry_count = world_count;
    g_scene_world_object = first_scene_object;
    return TRUE_VALUE;
}

static MotionState ReadMotionState() {
    MotionState state;
    state.camera_x = *(volatile LONG*)GLOBAL_CAMERA_X;
    state.camera_y = *(volatile LONG*)GLOBAL_CAMERA_Y;
    state.follow_object = *(LPVOID volatile*)GLOBAL_CAMERA_FOLLOW_OBJECT;
    state.follow_x = 0;
    state.follow_y = 0;
    state.world_object = g_scene_world_object;
    state.follow_valid = FALSE_VALUE;

    if (state.follow_object) {
        state.follow_x = *(volatile LONG*)((BYTE*)state.follow_object + FOLLOW_OBJECT_X_OFFSET);
        state.follow_y = *(volatile LONG*)((BYTE*)state.follow_object + FOLLOW_OBJECT_Y_OFFSET);
        state.follow_valid = TRUE_VALUE;
    }
    return state;
}

static void UpdateMotionHistory(const MotionState* newest) {
    if (!newest) return;

    // 第一次看到普通 world 时只有“当前值”，没有“前一帧”，因此不能算速度。
    // 但这只影响插值，不影响额外 Present：LegacyFrameHook 仍会安排 60 Hz 补帧。
    if (!g_have_current_motion) {
        g_previous_motion = *newest;
        g_current_motion = *newest;
        g_have_current_motion = TRUE_VALUE;
        g_camera_prediction_ready = FALSE_VALUE;
        g_follow_prediction_ready = FALSE_VALUE;
        return;
    }

    // 真正换了 Scene World Manager 对象时，前一张地图/场景的速度不能带进来。
    // 这里只重置一次历史；下一 Legacy Tick 就能重新建立 Camera 速度。
    if (g_current_motion.world_object != newest->world_object) {
        g_previous_motion = *newest;
        g_current_motion = *newest;
        g_camera_prediction_ready = FALSE_VALUE;
        g_follow_prediction_ready = FALSE_VALUE;
        return;
    }

    // 保存上一真实 20 Hz 状态，再写入这一 Tick 的最终状态。
    g_previous_motion = g_current_motion;
    g_current_motion = *newest;

    // Camera 的连续性只依赖 world manager 连续存在，因此从第二个有效 Tick 开始即可预测。
    g_camera_prediction_ready = TRUE_VALUE;

    // follower 还要求两帧都是有效对象，而且指针完全相同。
    // 如果剧情把 Camera 从玩家切到另一个对象，只让 follower 这一帧停止预测；
    // Camera 仍然可以继续使用自己的历史，不会再被 follower 拖成 20 Hz。
    g_follow_prediction_ready =
        g_previous_motion.follow_valid &&
        g_current_motion.follow_valid &&
        g_previous_motion.follow_object == g_current_motion.follow_object;
}

static LONG AbsLong(LONG value) {
    return value < 0 ? -value : value;
}

static LONG ClampLong(LONG value, LONG minimum, LONG maximum) {
    if (maximum < minimum) return value;
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

// 预测不是新的“逻辑坐标”。它只回答：如果过去两个 20 Hz Tick 的速度继续保持，
// 那么当前 +17/+33 ms 这一张视觉帧大概应该画在哪里。
// 若单 Tick 位移过大（传送、切图、剧情强制跳位），直接返回 current，避免飞屏。
static LONG PredictAxis(LONG previous, LONG current, DWORD elapsed_ms) {
    LONG delta = current - previous;
    if (AbsLong(delta) > g_config_max_prediction_step) return current;
    if (elapsed_ms > 49) elapsed_ms = 49;
    return current + (delta * (LONG)elapsed_ms) / 50;
}

static void BuildPredictedMotion(DWORD elapsed_ms, MotionState* predicted) {
    if (!predicted) return;

    // 缺少历史时直接复制当前真实状态。这仍然会产生真实的额外 world draw + Present，
    // 只是这一两帧暂时没有位置插值。这样“是否60FPS”和“插值是否准备好”被彻底解耦。
    *predicted = g_current_motion;

    if (g_camera_prediction_ready) {
        predicted->camera_x = PredictAxis(
            g_previous_motion.camera_x, g_current_motion.camera_x, elapsed_ms);
        predicted->camera_y = PredictAxis(
            g_previous_motion.camera_y, g_current_motion.camera_y, elapsed_ms);

        // Camera 仍遵守原版 0x44B300 建立的地图边界，预测绝不能把 renderer 推出合法地图。
        LONG min_x = *(volatile LONG*)GLOBAL_CAMERA_MIN_X;
        LONG min_y = *(volatile LONG*)GLOBAL_CAMERA_MIN_Y;
        LONG max_x = *(volatile LONG*)GLOBAL_CAMERA_MAX_X;
        LONG max_y = *(volatile LONG*)GLOBAL_CAMERA_MAX_Y;
        LONG view_w = *(volatile LONG*)GLOBAL_CAMERA_VIEW_W;
        LONG view_h = *(volatile LONG*)GLOBAL_CAMERA_VIEW_H;
        predicted->camera_x = ClampLong(predicted->camera_x, min_x, max_x - view_w);
        predicted->camera_y = ClampLong(predicted->camera_y, min_y, max_y - view_h);
    }

    if (g_follow_prediction_ready && predicted->follow_valid) {
        predicted->follow_x = PredictAxis(
            g_previous_motion.follow_x, g_current_motion.follow_x, elapsed_ms);
        predicted->follow_y = PredictAxis(
            g_previous_motion.follow_y, g_current_motion.follow_y, elapsed_ms);
    }
}

// ----------------------------------------------------------------------------
// 9. 额外帧的安全 gate
// ----------------------------------------------------------------------------

static BOOL RuntimeStillAllowsExtraFrame() {
    CastleGameStateSnapshotV1 state;
    if (!g_frame_has_world || !g_scene_world_object || g_scene_world_entry_count == 0) return FALSE_VALUE;
    if (g_frame_has_battle) return FALSE_VALUE;
    if (*(volatile BYTE*)GLOBAL_MAIN_ACTIVE_A == 0) return FALSE_VALUE;
    if (*(volatile BYTE*)GLOBAL_MAIN_ACTIVE_B == 0) return FALSE_VALUE;
    if (*(volatile BYTE*)GLOBAL_LEGACY_PENDING != 0) return FALSE_VALUE;
    if (*(volatile BYTE*)GLOBAL_TRANSITION_ACTIVE != 0) return FALSE_VALUE;
    if (*(volatile BYTE*)GLOBAL_DEBUG_DRAW != 0) return FALSE_VALUE;
    if (*(volatile BYTE*)GLOBAL_CAPTURE_PENDING != 0) return FALSE_VALUE;

    // 对话、电影、战斗和菜单状态只读取 Runtime 的同一份权威快照。
    memset(&state, 0, sizeof(state));
    state.magic = CASTLE_GAME_SNAPSHOT_MAGIC;
    state.struct_size = CASTLE_SIZEOF_GAME_STATE_SNAPSHOT_V1;
    state.version = CASTLE_GAME_STATE_STRUCTURE_VERSION_1;
    if (!g_game_state_api || g_game_state_api->GetSnapshot(&state) < 0 ||
        (state.flags & CASTLE_GAME_FLAG_FREE_ROAM_CANDIDATE) == 0u) {
        return FALSE_VALUE;
    }

    // Camera shake 的 draw 路径会推进计数 / RNG，所以 shake 期间严格保持 20 Hz。
    if (*(volatile BYTE*)GLOBAL_CAMERA_SHAKE_ACTIVE != 0) return FALSE_VALUE;

    // 尚未闭合副作用边界的特殊世界分支也不重放。
    if (*(volatile LONG*)GLOBAL_SCENE_RENDER_OVERRIDE_COUNT > 0) return FALSE_VALUE;
    if (*(volatile BYTE*)GLOBAL_WORLD_EXTRA_OVERLAY_ACTIVE != 0) return FALSE_VALUE;

    LPVOID gate_object = *(LPVOID volatile*)GLOBAL_FRAME_GATE_OBJECT;
    if (gate_object && *((volatile BYTE*)gate_object + 0x0A) != 0) return FALSE_VALUE;

    // 原版完整 draw queue 应该已经被本 Tick 的正常 Render 消费干净。
    // 若此时又出现新项，说明有其它代码在空闲期登记对象；不能用 synthetic queue 覆盖它。
    if (*(volatile DWORD*)GLOBAL_DRAW_QUEUE_COUNT != 0) return FALSE_VALUE;

    // follower 在没有新 Legacy Tick 的 17~33ms 内不应该突然变化。
    // 如果它真的被其它代码换掉，就放弃这一张额外帧，避免把预测值写到旧对象。
    // Scene World Manager 不再通过 +0x280 做“身份”校验，因为该字段并非已证实的稳定身份。
    if (g_have_current_motion &&
        *(LPVOID volatile*)GLOBAL_CAMERA_FOLLOW_OBJECT != g_current_motion.follow_object)
        return FALSE_VALUE;

    return TRUE_VALUE;
}

// ----------------------------------------------------------------------------
// 10. 两个正式代码 Hook
// ----------------------------------------------------------------------------

// 原版 0x44A9BF 的 0x4020A0 没有参数，所以这个 Hook 也没有参数。
// 顺序必须是“先执行原 helper，再看最终 live draw queue”，因为 helper 理论上可能对本帧表现
// 做准备；我们要判断的是 0x44A9C6 真正即将消费的那份队列。
extern "C" void __cdecl PreDrawHook() {
    if (g_next_pre_draw) g_next_pre_draw();

    g_frame_has_world = FALSE_VALUE;
    g_frame_has_battle = FALSE_VALUE;
    g_scene_world_entry_count = 0;
    g_scene_world_object = (LPVOID)0;

    if (!CaptureWorldEntriesFromMixedQueue()) {
        ++g_skip_no_world;
        return;
    }

    // 这里只负责保存“这一帧将要被原版消费”的 world-only 队列。
    // Camera / follower 的真实状态改到完整 Legacy Frame 返回以后再采样，确保拿到本 Tick
    // 所有 update/draw 后最终稳定值，避免 PreDraw 时序与实际 Camera 更新路径错位。
    g_frame_has_world = TRUE_VALUE;
}

// 原版主循环在 0x40171C 用 ECX=main manager 调 0x44A970。
// __fastcall 的第一个参数同样放 ECX，因此可以无损接住 this。
extern "C" void __fastcall LegacyFrameHook(LPVOID manager, LPVOID /*unused_edx*/) {
    // 每个真实 Legacy Tick 开始时先清空“本帧捕获结果”。
    // 如果 0x44A970 因某个早退分支根本没有走到 PreDrawHook，就不会误用上一 Tick 的 world 队列。
    g_frame_has_world = FALSE_VALUE;
    g_frame_has_battle = FALSE_VALUE;
    g_scene_world_entry_count = 0;
    g_scene_world_object = (LPVOID)0;

    // 完整执行原版这一帧：Logic、完整 GUI draw、宽屏 Hook（若有）、Present 全部照旧。
    if (g_next_legacy_frame) g_next_legacy_frame(manager);
    ++g_legacy_frame_count;

    if (!g_enabled || !g_clock_api || !g_frame_has_world) {
        g_extra_stage = 0;
        return;
    }

    if (g_frame_has_battle) {
        ++g_skip_battle;
        g_extra_stage = 0;
        return;
    }

    // 运动状态现在在“完整 Legacy Frame 已经结束”后采样。
    // 这才是下一组 +17/+33 ms 视觉预测应该基于的真实 20 Hz 最终状态。
    MotionState newest = ReadMotionState();
    UpdateMotionHistory(&newest);

    if (!RuntimeStillAllowsExtraFrame()) {
        ++g_skip_runtime_gate;
        g_extra_stage = 0;
        return;
    }

    // v1.3 的核心修复：
    // 即使这是第一张 world 帧、还没有两帧运动历史，也绝不能禁止 extra Present。
    // 这种情况下额外帧只是画“当前真实位置”，等下一 Tick 历史建立后自然开始插值。
    if (!g_camera_prediction_ready) {
        ++g_extra_without_prediction;
    }

    // 原版 20 Hz Present 已经发生，把它当作本 50ms 三帧组的第1张。
    // 再安排约 +17ms / +33ms 两张，外部 FPS 计数器首先必须能实际看到接近 60 FPS。
    DWORD now = RuntimeNowMilliseconds();
    g_base_frame_time_ms = now;
    g_next_extra_time_ms = now + 17;
    g_extra_stage = 1;
}

// ----------------------------------------------------------------------------
// 11. 真正的 Render-only 额外帧
// ----------------------------------------------------------------------------

static void RestoreRealMotion(const MotionState* real_state) {
    if (!real_state) return;
    *(volatile LONG*)GLOBAL_CAMERA_X = real_state->camera_x;
    *(volatile LONG*)GLOBAL_CAMERA_Y = real_state->camera_y;
    if (real_state->follow_valid && real_state->follow_object) {
        *(volatile LONG*)((BYTE*)real_state->follow_object + FOLLOW_OBJECT_X_OFFSET) = real_state->follow_x;
        *(volatile LONG*)((BYTE*)real_state->follow_object + FOLLOW_OBJECT_Y_OFFSET) = real_state->follow_y;
    }
}

static void ApplyPredictedMotion(const MotionState* predicted) {
    if (!predicted) return;
    *(volatile LONG*)GLOBAL_CAMERA_X = predicted->camera_x;
    *(volatile LONG*)GLOBAL_CAMERA_Y = predicted->camera_y;
    if (predicted->follow_valid && predicted->follow_object) {
        *(volatile LONG*)((BYTE*)predicted->follow_object + FOLLOW_OBJECT_X_OFFSET) = predicted->follow_x;
        *(volatile LONG*)((BYTE*)predicted->follow_object + FOLLOW_OBJECT_Y_OFFSET) = predicted->follow_y;
    }
}

static void RenderExtraWorldFrame() {
    CastleRenderCallV1 render_call;
    CastleLeaseHandle render_lease = 0u;
    CastleU32 display_generation = 0u;
    CastleResult render_result;
    if (g_inside_extra_render) return;
    if (!RuntimeStillAllowsExtraFrame()) {
        ++g_skip_runtime_gate;
        return;
    }

    LPVOID main_manager = *(LPVOID volatile*)GLOBAL_MAIN_MANAGER;
    LPVOID display = *(LPVOID volatile*)GLOBAL_DISPLAY_OBJECT;
    if (!g_render_api || !g_game_state_api || !main_manager || !display) {
        ++g_skip_runtime_gate;
        return;
    }

    if (g_render_api->BeginExtraWorldFrame(g_runtime_plugin, 0u, &render_lease,
            &display_generation) < 0) {
        ++g_skip_runtime_gate;
        return;
    }

    g_inside_extra_render = TRUE_VALUE;

    DWORD now = RuntimeNowMilliseconds();
    DWORD elapsed = now - g_base_frame_time_ms;
    if (elapsed > 49) elapsed = 49;

    MotionState predicted;
    BuildPredictedMotion(elapsed, &predicted);

    // 保存“此刻真正的20Hz状态”。正常情况下它应等于 g_current_motion；再读一次可以防止
    // 以后其它插件在表现层临时改值时，我们误把自己的缓存写回去。
    MotionState real_now = ReadMotionState();

    // 写入仅本次 draw 可见的预测位置。
    ApplyPredictedMotion(&predicted);

    // 重新建立一个只包含 Scene World Manager 的 synthetic draw queue。
    // 注意：原始完整队列可以是混合队列；我们在 PreDrawHook 中已经只筛出了 world 条目。
    // GUI / SF2 / HUD 根本没有被复制进来，所以它们不可能因为 FPSUnlock 被多调用。
    DWORD synthetic_bytes = g_scene_world_entry_count * DRAW_QUEUE_ENTRY_BYTES;
    memcpy((void*)GLOBAL_DRAW_QUEUE_ENTRIES, g_scene_world_entries, synthetic_bytes);
    *(volatile DWORD*)GLOBAL_DRAW_QUEUE_COUNT = g_scene_world_entry_count;

    // 额外保险：虽然 gate 已要求消息状态为0，仍在 world draw 短窗口强制保持0。
    // 这是因为 0x40B050 内部自己会调用 0x403E30 / 0x404800。
    BYTE old_message_target = *(volatile BYTE*)GLOBAL_MESSAGE_TARGET_STATE;
    BYTE old_message_current = *(volatile BYTE*)GLOBAL_MESSAGE_CURRENT_STATE;
    *(volatile BYTE*)GLOBAL_MESSAGE_TARGET_STATE = 0;
    *(volatile BYTE*)GLOBAL_MESSAGE_CURRENT_STATE = 0;

    memset(&render_call, 0, sizeof(render_call));
    render_call.magic = CASTLE_RENDER_CALL_MAGIC;
    render_call.struct_size = CASTLE_SIZEOF_RENDER_CALL_V1;
    render_call.version = CASTLE_RENDER_STRUCTURE_VERSION_1;
    render_call.flags = CASTLE_RENDER_CALL_EXTRA_WORLD_FRAME;
    render_call.render_context = (CastleAddress)(SIZE_T)main_manager;
    render_call.display_generation = display_generation;
    render_call.extra_frame_lease = render_lease;
    render_result = g_render_api->RenderCurrentQueue(&render_call);

    *(volatile BYTE*)GLOBAL_MESSAGE_CURRENT_STATE = old_message_current;
    *(volatile BYTE*)GLOBAL_MESSAGE_TARGET_STATE = old_message_target;

    // world draw 结束立刻恢复真实逻辑坐标。Present 只消费已经画好的像素，不需要看到预测值。
    RestoreRealMotion(&real_now);

    // 原版 0x434710 正常会把队列清零；宽屏最终也会进入原版队列消费。
    // 若未来其它 Hook 没清，我们只清自己 synthetic 的残留 count，不能把它带进下一 Legacy Tick。
    if (*(volatile DWORD*)GLOBAL_DRAW_QUEUE_COUNT != 0) {
        *(volatile DWORD*)GLOBAL_DRAW_QUEUE_COUNT = 0;
        // 只清理本插件刚才写入的 synthetic world 条目，绝不去擦完整200项区域。
        memset((void*)GLOBAL_DRAW_QUEUE_ENTRIES, 0,
               g_scene_world_entry_count * DRAW_QUEUE_ENTRY_BYTES);
    }

    if (render_result >= 0) {
        render_call.render_context = (CastleAddress)(SIZE_T)display;
        if (g_render_api->PresentCurrentDisplay(&render_call) >= 0) {
            ++g_extra_present_count;
        }
    } else {
        ++g_skip_runtime_gate;
    }

    g_inside_extra_render = FALSE_VALUE;
    g_render_api->EndExtraWorldFrame(render_lease);
}

// ----------------------------------------------------------------------------
// 12. 60 Hz 主线程等待器
// ----------------------------------------------------------------------------

static void MaybeLogStats();

extern "C" BOOL __stdcall FrameWaitHook() {
    if (!g_enabled || !g_original_wait_message ||
        !g_msg_wait_for_multiple_objects_ex || !g_clock_api) {
        return g_original_wait_message ? g_original_wait_message() : TRUE_VALUE;
    }

    MaybeLogStats();

    // 没有计划额外帧时，完全恢复原版 WaitMessage 行为。
    // 这意味着菜单/对话/Battle 等没有安排额外 world frame 的状态不会被 FPSUnlock 周期性唤醒。
    if (g_extra_stage == 0) {
        return g_original_wait_message();
    }

    DWORD now = RuntimeNowMilliseconds();
    if ((g_extra_stage == 1 || g_extra_stage == 2) && TimeReached(now, g_next_extra_time_ms)) {
        RenderExtraWorldFrame();

        if (g_extra_stage == 1) {
            g_extra_stage = 2;
            g_next_extra_time_ms = g_base_frame_time_ms + 33;
        } else {
            g_extra_stage = 0;
            g_next_extra_time_ms = 0;
        }
        return TRUE_VALUE;
    }

    DWORD timeout_ms = 50;
    if (g_extra_stage == 1 || g_extra_stage == 2) {
        LONG remaining = (LONG)(g_next_extra_time_ms - now);
        timeout_ms = remaining <= 0 ? 0 : (DWORD)remaining;
    }

    DWORD wait_result = g_msg_wait_for_multiple_objects_ex(
        0, (const HANDLE*)0, timeout_ms, QS_ALLINPUT_VALUE, MWMO_INPUTAVAILABLE_VALUE);

    if (wait_result == 0xFFFFFFFFUL) {
        return g_original_wait_message();
    }
    return TRUE_VALUE;
}

// ----------------------------------------------------------------------------
// 13. 日志统计
// ----------------------------------------------------------------------------

static void MaybeLogStats() {
    if (!g_config_log_stats || !g_clock_api) return;
    DWORD now = RuntimeNowMilliseconds();
    if ((LONG)(now - g_last_stats_log_ms) < 10000) return;
    g_last_stats_log_ms = now;

    char line[320];
    CopyString(line, 320, "[统计] LegacyFrame=");
    AppendUnsignedDecimal(line, 320, g_legacy_frame_count);
    AppendString(line, 320, "，ExtraPresent=");
    AppendUnsignedDecimal(line, 320, g_extra_present_count);
    AppendString(line, 320, "，无world跳过=");
    AppendUnsignedDecimal(line, 320, g_skip_no_world);
    AppendString(line, 320, "，Battle跳过=");
    AppendUnsignedDecimal(line, 320, g_skip_battle);
    AppendString(line, 320, "，运行态保护跳过=");
    AppendUnsignedDecimal(line, 320, g_skip_runtime_gate);
    AppendString(line, 320, "，无插值历史但仍补帧=");
    AppendUnsignedDecimal(line, 320, g_extra_without_prediction);
    LogLine(line);
}

// ----------------------------------------------------------------------------
// 14. API / INI / 版本验证
// ----------------------------------------------------------------------------

static CastleStringView RuntimeView(const char* text) {
    CastleStringView view;
    view.data = text;
    view.length = StringLength(text);
    return view;
}

static const void* QueryRuntimeInterface(const CastleRuntimeApiV1* runtime_api,
                                         const char* interface_id,
                                         CastleU32 version,
                                         CastleU32 minimum_size,
                                         CastleU32 capabilities) {
    CastleInterfaceQueryV1 query;
    CastleInterfaceResultV1 result;
    memset(&query, 0, sizeof(query));
    memset(&result, 0, sizeof(result));
    if (!runtime_api || !runtime_api->QueryInterface) return (const void*)0;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = RuntimeView(interface_id);
    query.requested_version = version;
    query.minimum_struct_size = minimum_size;
    query.required_capabilities_low = capabilities;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    return runtime_api->QueryInterface(&query, &result) == CASTLE_OK ?
        result.api_pointer : (const void*)0;
}

static BOOL ResolveApis(const CastleRuntimeApiV1* runtime_api,
                        CastlePluginHandle plugin) {
    CastleModule kernel32 = 0u;
    CastleModule user32 = 0u;
    CastleAddress address = 0u;
    CastleU32 path_length = 0u;
    static const char config_name[] = "Castle_FPSUnlock.ini";
    g_runtime_api = runtime_api;
    g_runtime_plugin = plugin;
    g_hook_api = (const CastleHookApiV1*)QueryRuntimeInterface(runtime_api,
        CASTLE_HOOK_INTERFACE_ID, CASTLE_HOOK_API_VERSION_1,
        CASTLE_SIZEOF_HOOK_API_V1, 0u);
    g_clock_api = (const CastleClockApiV1*)QueryRuntimeInterface(runtime_api,
        CASTLE_CLOCK_INTERFACE_ID, CASTLE_CLOCK_API_VERSION_1,
        CASTLE_SIZEOF_CLOCK_API_V1, CASTLE_CLOCK_CAP_TIMER_RESOLUTION_1);
    g_render_api = (const CastleRenderApiV1*)QueryRuntimeInterface(runtime_api,
        CASTLE_RENDER_INTERFACE_ID, CASTLE_RENDER_API_VERSION_1,
        CASTLE_SIZEOF_RENDER_API_V1, CASTLE_RENDER_CAP_EXTRA_FRAME_LEASE);
    g_display_api = (const CastleDisplayApiV1*)QueryRuntimeInterface(runtime_api,
        CASTLE_DISPLAY_INTERFACE_ID, CASTLE_DISPLAY_API_VERSION_1,
        CASTLE_SIZEOF_DISPLAY_API_V1, 0u);
    g_game_state_api = (const CastleGameStateApiV1*)QueryRuntimeInterface(runtime_api,
        CASTLE_GAME_STATE_INTERFACE_ID, CASTLE_GAME_STATE_API_VERSION_1,
        CASTLE_SIZEOF_GAME_STATE_API_V1, CASTLE_GAME_STATE_CAP_MUTATION_LEASE);
    g_module_api = (const CastleModuleApiV1*)QueryRuntimeInterface(runtime_api,
        CASTLE_MODULE_INTERFACE_ID, CASTLE_MODULE_API_VERSION_1,
        CASTLE_SIZEOF_MODULE_API_V1, 0u);
    g_path_api = (const CastlePathApiV1*)QueryRuntimeInterface(runtime_api,
        CASTLE_PATH_INTERFACE_ID, CASTLE_PATH_API_VERSION_1,
        CASTLE_SIZEOF_PATH_API_V1, 0u);
    g_log_api = (const CastleLogApiV1*)QueryRuntimeInterface(runtime_api,
        CASTLE_LOG_INTERFACE_ID, CASTLE_LOG_API_VERSION_1,
        CASTLE_SIZEOF_LOG_API_V1, 0u);
    if (!g_hook_api || !g_clock_api || !g_render_api || !g_display_api ||
        !g_game_state_api || !g_module_api || !g_path_api || !g_log_api) {
        return FALSE_VALUE;
    }

    if (g_path_api->BuildPluginRelativePathUtf8(plugin, RuntimeView(config_name),
            g_ini_path, 260u, &path_length) != CASTLE_OK || path_length == 0u) {
        return FALSE_VALUE;
    }
    if (g_module_api->LoadSystemModule(plugin, RuntimeView("kernel32.dll"),
            CASTLE_MODULE_LOAD_PIN, &kernel32) < 0 ||
        g_module_api->LoadSystemModule(plugin, RuntimeView("user32.dll"),
            CASTLE_MODULE_LOAD_PIN, &user32) < 0) return FALSE_VALUE;
    if (g_module_api->GetProcedure(kernel32, RuntimeView("GetPrivateProfileIntA"),
            &address) < 0) return FALSE_VALUE;
    memcpy(&g_get_private_profile_int_a, &address,
           sizeof(g_get_private_profile_int_a));
    if (g_module_api->GetProcedure(user32,
            RuntimeView("MsgWaitForMultipleObjectsEx"), &address) < 0) {
        return FALSE_VALUE;
    }
    memcpy(&g_msg_wait_for_multiple_objects_ex, &address,
           sizeof(g_msg_wait_for_multiple_objects_ex));
    return g_get_private_profile_int_a && g_msg_wait_for_multiple_objects_ex ?
        TRUE_VALUE : FALSE_VALUE;
}

static void ReadConfig() {
    if (!g_get_private_profile_int_a) return;

    g_config_enable = g_get_private_profile_int_a(
        "FrameRate", "Enable", 1, g_ini_path) ? TRUE_VALUE : FALSE_VALUE;
    g_config_log_stats = g_get_private_profile_int_a(
        "FrameRate", "LogStats", 1, g_ini_path) ? TRUE_VALUE : FALSE_VALUE;

    // v1.3 当前正式实现只支持 60。读取这个键主要为了以后扩展时保持 INI 兼容；非法值回60。
    DWORD fps = g_get_private_profile_int_a("FrameRate", "TargetFPS", 60, g_ini_path);
    g_config_target_fps = (fps == 60) ? 60 : 60;

    LONG max_step = (LONG)g_get_private_profile_int_a(
        "Interpolation", "MaxPredictionStep", 96, g_ini_path);
    if (max_step < 8) max_step = 8;
    if (max_step > 512) max_step = 512;
    g_config_max_prediction_step = max_step;
}

static BOOL ValidateGameVersion() {
    // 只严格验证“我们自己会写”的地址仍是原版，以及几个核心函数协议。
    // 故意不要求 0x44A9C6 / 0x44A9E6 仍指向原版，因为宽屏可能已经合法 Hook 它们。
    static const BYTE expected_wait_call[6] = {0xFF, 0x15, 0x98, 0x01, 0x46, 0x00};
    static const BYTE expected_legacy_call[5] = {0xE8, 0x4F, 0x92, 0x04, 0x00};
    static const BYTE expected_pre_draw_call[5] = {0xE8, 0xDC, 0x76, 0xFB, 0xFF};
    static const BYTE expected_legacy_head[8] = {0xA1, 0x90, 0xF3, 0x46, 0x00, 0x83, 0xEC, 0x34};
    static const BYTE expected_scene_draw_head[8] = {0x56, 0x8B, 0xF1, 0x8A, 0x86, 0x19, 0x02, 0x00};

    if (!BytesEqual(0x0040179C, expected_wait_call, 6)) return FALSE_VALUE;
    if (!BytesEqual(ADDR_CALL_LEGACY_FRAME, expected_legacy_call, 5)) return FALSE_VALUE;
    if (!BytesEqual(ADDR_CALL_PRE_DRAW, expected_pre_draw_call, 5)) return FALSE_VALUE;
    if (!BytesEqual(ADDR_GAME_LEGACY_FRAME, expected_legacy_head, 8)) return FALSE_VALUE;
    if (!BytesEqual(FN_SCENE_WORLD_DRAW, expected_scene_draw_head, 8)) return FALSE_VALUE;

    return TRUE_VALUE;
}

// ----------------------------------------------------------------------------
// 15. 安装 / 恢复 Hook
// ----------------------------------------------------------------------------

static void RestoreHooks();

static BOOL InstallHooks() {
    static const char transaction_label[] = "FPSUnlock coordinated hooks";
    static const char legacy_signature[] = "org.castlereforge.signature.legacy-frame.v1";
    static const char pre_draw_signature[] = "org.castlereforge.signature.pre-draw.v1";
    static const char wait_signature[] = "org.castlereforge.signature.wait-message.v1";
    CastleRuntimeInfoV1 info;
    CastleTransactionHandle transaction = 0u;
    CastleChainHookClaimV1 claim;
    CastleHookBindingV1 binding;
    CastleResult result;
    if (!g_hook_api || !g_runtime_api || !g_runtime_plugin) return FALSE_VALUE;
    memset(&info, 0, sizeof(info));
    info.magic = CASTLE_RUNTIME_INFO_MAGIC;
    info.struct_size = CASTLE_SIZEOF_RUNTIME_INFO_V1;
    info.info_version = CASTLE_RUNTIME_INFO_VERSION_1;
    if (g_runtime_api->GetRuntimeInfo(&info) != CASTLE_OK || !info.game_module) {
        return FALSE_VALUE;
    }
    g_original_wait_message = *(WaitMessageFn*)ADDR_IAT_WAIT_MESSAGE;
    if (!g_original_wait_message) return FALSE_VALUE;
    result = g_hook_api->BeginTransaction(g_runtime_plugin,
        RuntimeView(transaction_label), 0u, &transaction);
    if (result < 0) return FALSE_VALUE;

    memset(&claim, 0, sizeof(claim));
    claim.magic = CASTLE_CHAIN_HOOK_MAGIC;
    claim.struct_size = CASTLE_SIZEOF_CHAIN_HOOK_V1;
    claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    claim.hook_kind = CASTLE_HOOK_REL32_CALL;
    claim.target.module = info.game_module;
    claim.target.rva = ADDR_CALL_LEGACY_FRAME - 0x00400000u;
    claim.target.size = 5u;
    claim.expected_original_target = ADDR_GAME_LEGACY_FRAME;
    claim.replacement_hook = (CastleAddress)(SIZE_T)&LegacyFrameHook;
    claim.signature_id = RuntimeView(legacy_signature);
    claim.phase = CASTLE_HOOK_PHASE_NORMAL;
    claim.priority = CASTLE_HOOK_PRIORITY_DEFAULT;
    claim.label = claim.signature_id;
    result = g_hook_api->AddRelativeCallHook(transaction, &claim, &g_legacy_claim);
    if (result < 0) goto install_failed;

    claim.target.rva = ADDR_CALL_PRE_DRAW - 0x00400000u;
    claim.expected_original_target = ADDR_GAME_PRE_DRAW;
    claim.replacement_hook = (CastleAddress)(SIZE_T)&PreDrawHook;
    claim.signature_id = RuntimeView(pre_draw_signature);
    claim.label = claim.signature_id;
    result = g_hook_api->AddRelativeCallHook(transaction, &claim, &g_pre_draw_claim);
    if (result < 0) goto install_failed;

    claim.hook_kind = CASTLE_HOOK_IAT_POINTER;
    claim.target.rva = ADDR_IAT_WAIT_MESSAGE - 0x00400000u;
    claim.target.size = 4u;
    claim.expected_original_target = (CastleAddress)(SIZE_T)g_original_wait_message;
    claim.replacement_hook = (CastleAddress)(SIZE_T)&FrameWaitHook;
    claim.signature_id = RuntimeView(wait_signature);
    claim.label = claim.signature_id;
    result = g_hook_api->AddPointerHook(transaction, &claim, &g_wait_claim);
    if (result < 0) goto install_failed;
    result = g_hook_api->PreflightTransaction(transaction);
    if (result >= 0) result = g_hook_api->CommitTransaction(transaction);
    if (result < 0) return FALSE_VALUE;

    memset(&binding, 0, sizeof(binding));
    binding.magic = CASTLE_HOOK_BINDING_MAGIC;
    binding.struct_size = CASTLE_SIZEOF_HOOK_BINDING_V1;
    binding.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    if (g_hook_api->GetHookBinding(g_legacy_claim, &binding) != CASTLE_OK ||
        !binding.next_slot) return FALSE_VALUE;
    g_next_legacy_frame = (GameThisVoidFn)*binding.next_slot;
    if (g_hook_api->GetHookBinding(g_pre_draw_claim, &binding) != CASTLE_OK ||
        !binding.next_slot) return FALSE_VALUE;
    g_next_pre_draw = (GameCdeclVoidFn)*binding.next_slot;
    if (g_hook_api->GetHookBinding(g_wait_claim, &binding) != CASTLE_OK ||
        !binding.next_slot) return FALSE_VALUE;
    g_original_wait_message = (WaitMessageFn)*binding.next_slot;
    g_hooks_installed = TRUE_VALUE;
    return TRUE_VALUE;

install_failed:
    g_hook_api->AbortTransaction(transaction);
    return FALSE_VALUE;
}

static void RestoreHooks() {
    if (!g_hooks_installed) return;
    /* Runtime 拥有补丁页；插件退出只停止业务，绝不私自把链中其它插件一起覆盖掉。 */
    g_enabled = FALSE_VALUE;
    g_extra_stage = 0;
    g_hooks_installed = FALSE_VALUE;
}

// ----------------------------------------------------------------------------
// 16. RuntimeSDK 生命周期与 DLL / ASI 入口
// ----------------------------------------------------------------------------

static CastleResult CASTLE_RUNTIME_CALL FPSUnlockIntegrated(
    const CastleRuntimeApiV1* runtime_api, CastlePluginHandle plugin,
    void* /*user_context*/) {
    if (!ResolveApis(runtime_api, plugin)) return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    LogLine("《幽城幻剑录》Castle_FPSUnlock v1.4 RuntimeSDK 启动。");
    LogLine("By Luminous with ChatGPT");
    LogLine("[架构] 原版约20Hz Legacy Logic保持不变；普通地图世界使用约60Hz Render-only补帧。");
    LogLine("[协调] RenderQueue/Present只通过Runtime Render调用；Camera/队列/对话/World写入持有统一租约。");
    ReadConfig();
    if (!g_config_enable) {
        LogLine("[配置] Enable=0，本次不安装Hook。");
        return CASTLE_OK;
    }
    if (!ValidateGameVersion()) {
        LogLine("[失败] FPSUnlock Hook锚点/核心协议与目标RPG.exe不一致；本次不安装。");
        return CASTLE_ERROR_EXPECTED_BYTES;
    }
    if (g_clock_api->AcquireTimerResolution(plugin, 1u, &g_clock_lease) < 0 ||
        !g_clock_lease) {
        LogLine("[失败] Runtime Clock无法授予1ms计时器租约。");
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_timer_resolution_raised = TRUE_VALUE;
    if (!InstallHooks()) {
        LogLine("[失败] Runtime Hook事务未能完整提交。");
        g_clock_api->ReleaseTimerResolution(g_clock_lease);
        g_clock_lease = 0u;
        g_timer_resolution_raised = FALSE_VALUE;
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    g_last_stats_log_ms = RuntimeNowMilliseconds();
    g_enabled = TRUE_VALUE;
    LogLine("[成功] v1.4已启用：world安全帧安排额外Present；插值算法仍完全属于FPSUnlock。");
    LogLine("[保护] 对话/菜单/Battle/Bink/标题/Camera shake/特殊world overlay自动保持原版20Hz。");
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL FPSUnlockStandalone(void* /*user_context*/) {
    return CASTLE_ERROR_RUNTIME_REQUIRED;
}

static void CASTLE_RUNTIME_CALL FPSUnlockRuntimeFault(
    CastleResult /*failure*/, void* /*user_context*/) {
    /* Runtime 缺失/损坏时官方插件必须静默停用；不能重新打开旧私有补丁路径。 */
}

static void CASTLE_RUNTIME_CALL FPSUnlockProcessExit(void* /*user_context*/) {
    RestoreHooks();
    MaybeLogStats();
    LogLine("[退出] Castle_FPSUnlock v1.4 随进程结束。");
    if (g_clock_api && g_clock_lease) g_clock_api->ReleaseTimerResolution(g_clock_lease);
    g_clock_lease = 0u;
    g_timer_resolution_raised = FALSE_VALUE;
}

static const char g_plugin_id[] = "org.castlereforge.fpsunlock";
static const char g_display_name[] = "Castle FPSUnlock";
static const char g_version_text[] = "1.4.0";
static const char g_build_id[] = "runtimesdk-coordinated-render";

static const CastlePluginDescriptorV1 g_plugin_descriptor = {
    CASTLE_PLUGIN_DESC_MAGIC,
    CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_REQUESTS_HOOKS | CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE,
    0u,
    {g_plugin_id, sizeof(g_plugin_id) - 1u},
    {g_display_name, sizeof(g_display_name) - 1u},
    {g_version_text, sizeof(g_version_text) - 1u},
    {g_build_id, sizeof(g_build_id) - 1u}
};

static const CastleRuntimeClientConfigV1 g_client_config = {
    CASTLE_CLIENT_CONFIG_MAGIC,
    CASTLE_SIZEOF_CLIENT_CONFIG_V1,
    CASTLE_CLIENT_CONFIG_VERSION_1,
    CASTLE_CLIENT_FLAG_REQUIRE_RUNTIME,
    FPSUnlockIntegrated,
    FPSUnlockStandalone,
    FPSUnlockRuntimeFault,
    FPSUnlockProcessExit,
    (void*)0
};

static CastlePluginExportV1 g_plugin_export = {
    CASTLE_PLUGIN_QUERY_MAGIC,
    CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1,
    0u,
    &g_plugin_descriptor,
    &g_client_config,
    0u,
    (CastleClientBootstrapFn)0
};

extern "C" const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requested_version) {
    return requested_version == CASTLE_PLUGIN_EXPORT_VERSION_1 ?
        &g_plugin_export : (const CastlePluginExportV1*)0;
}

extern "C" void __cdecl InitializeASI(void) {
    CastleRuntimeClient_RunNow();
}

extern "C" BOOL __stdcall DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH_VALUE) {
        g_module_instance = module;
        CastleRuntimeClient_OnProcessAttach((CastleModule)(SIZE_T)module,
                                             &g_plugin_export);
    } else if (reason == DLL_PROCESS_DETACH_VALUE) {
        CastleRuntimeClient_OnProcessDetach(reserved);
    }
    return TRUE_VALUE;
}
