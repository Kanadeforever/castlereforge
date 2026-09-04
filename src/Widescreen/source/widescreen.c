#include "widescreen.h"
#include "platform.h"
#include "game_addresses.h"
#include "runtime.h"
#include "CastleDisplay_API.h"
#include "CastleRender_API.h"

/*
 * widescreen.c — v0.11-poc11 电影式模糊 / 纯黑侧区切换版
 *
 * ============================
 * 一、为什么彻底放弃 POC1 的 982 backing
 * ============================
 *
 * POC1 已经由实机证明：
 * - 854×480 的 DirectDraw/窗口输出可以工作；
 * - 标题、Bink、SaveSlot 等中央 640 的保护方向可以工作；
 * - 但把游戏真正用来绘图的 software backing 从 768×576 改成 982×576 会破坏大量旧代码的行距假设。
 *
 * 这不是“再补几个 768 常量”就能可靠解决的问题。
 * 所以本版把原版 renderer 的工作环境完整还原：
 *
 *     原版 renderer 永远只看到：768×576 = (64 + 640 + 64) × (48 + 480 + 48)
 *
 * ============================
 * 二、本版怎么得到 16:9 或 21:9 的世界
 * ============================
 *
 * 我们仍然不让 renderer 一次画宽屏，而是让它每次只画它最熟悉的 640×480：
 *
 *   第 1 次：CameraX = 安全中心值       -> 得到原版中央 640
 *   第 2 次：CameraX = 中心值 - N       -> 取左边新增的 N 像素
 *   第 3 次：CameraX = 中心值 + N       -> 取右边新增的 N 像素
 *
 * N 由 INI 选择的模式决定：
 *
 *   16:9：N = 107 -> 107 + 640 + 107 = 854×480
 *   21:9：N = 240 -> 240 + 640 + 240 = 1120×480
 *
 * 地图边缘处不能把 Camera 硬推到原版 clamp 外面，否则老 renderer 可能访问不存在的地图区域。
 * 因此读取 0x44B300 已经建立的 minX/maxX/viewportW，根据“当前 N”计算安全中心 Camera。
 * 地图本身如果比当前输出还窄，就只显示真实存在的地图内容，绝不为了填满屏幕把 Camera 推出合法边界。
 *
 * ============================
 * 三、为什么 Present staging 也跟着变宽
 * ============================
 *
 * 0x406330 会按：
 *
 *     modeWidth + extraX * 2
 *
 * 计算来源行宽，而 extraX 仍然是原版 64。
 * 所以插件自己的“只读输出 staging”必须匹配当前模式：
 *
 *   16:9：854 + 64*2 = 982 -> 982×576 staging
 *   21:9：1120 + 64*2 = 1248 -> 1248×576 staging
 *
 * 这块 staging 只在最后 Present 前由插件自己拼好，再临时交给 0x405A10 读取。
 * 原版 renderer 从头到尾仍然只写 768×576 backing，这一点和已经证伪的 POC1 完全不同。
 */

#define LOGICAL_WIDTH             640u
#define LOGICAL_HEIGHT            480u
#define OUTPUT_HEIGHT             480u

/*
 * v0.10 起把最终显示宽度从“写死 854”改成两个固定模式；v0.11 保持这套已经实机验收的几何不变。
 *
 * 为什么仍然只有两个模式，而不是让用户随便填宽度：
 * - 用户当前只需要已经验收的 16:9 和新的 21:9；
 * - 固定模式更容易验证 Camera 安全边界、Bink 居中、模糊侧区和 Present 行距；
 * - 以后如果要再加 32:9，可以继续新增一个经过实机验证的固定档位，而不是让错误数值直接进入旧游戏。
 *
 * 16:9 沿用已经通过实机验收的 854×480：
 *     107 + 640 + 107 = 854
 *
 * 21:9 使用严格数学比例的 1120×480：
 *     240 + 640 + 240 = 1120
 */
#define OUTPUT_WIDTH_16_9         854u
#define SIDE_WIDTH_16_9           107u
#define STAGING_WIDTH_16_9        982u

#define OUTPUT_WIDTH_21_9        1120u
#define SIDE_WIDTH_21_9           240u
#define STAGING_WIDTH_21_9       1248u

/*
 * 下面四个变量在插件启动时从 INI 决定，此后本次游戏运行期间保持不变。
 * 默认值就是 16:9，因此即使 INI 丢失、损坏或没有 [Display] 节，也会继续得到 v0.9 的既有行为。
 */
static u32 g_output_width = OUTPUT_WIDTH_16_9;
static u32 g_side_width = SIDE_WIDTH_16_9;
static u32 g_present_staging_w = STAGING_WIDTH_16_9;
static u32 g_cinematic_low_w = 214u;
static int g_ultrawide_enabled;
static const CastleRuntimeApiV1* g_sdk_runtime_api;
static const CastleDisplayApiV1* g_sdk_display_api;
static const CastleRenderApiV1* g_sdk_render_api;
static CastlePluginHandle g_sdk_plugin_handle;
static CastleProviderHandle g_sdk_display_provider;
static CastleProviderHandle g_sdk_render_provider;
static CastleDisplayGeometryV1 g_sdk_geometry;
static u32 g_sdk_services_ready;
/* 奇数表示游戏线程正在发布，偶数表示整张几何快照已经写完。 */
static volatile u32 g_sdk_geometry_sequence;

/*
 * v0.11 新增的侧区样式开关。
 *
 * 1 = 保持 v0.10 已经实机验收的“强模糊 + 轻压暗”电影式侧区；
 * 0 = 使用纯黑侧区。
 *
 * 这个变量只决定左右侧面板最终写入什么颜色，不改变：
 * - 哪些场景触发电影式模式；
 * - 中央 640×480 的位置和比例；
 * - 左右面板推入 / 退出的宽度、缓动和边缘柔化；
 * - EnterDurationMs / ExitDurationMs 的时间。
 *
 * 因此两种模式真正只有“模糊像素”与“黑色像素”的区别。
 */
static int g_cinematic_blurred_sides = 1;

/*
 * 这些别名让后面的渲染代码仍然保持“OUTPUT_WIDTH / SIDE_WIDTH”这种容易阅读的写法，
 * 但它们现在展开成上面的运行时变量，不再是编译期常数。
 */
#define OUTPUT_WIDTH          (g_output_width)
#define SIDE_WIDTH            (g_side_width)
#define PRESENT_STAGING_W     (g_present_staging_w)
#define CINEMATIC_LOW_W       (g_cinematic_low_w)

/*
 * 电影式侧区和传统黑边都使用同一组 INI 过渡时间。
 *
 * 默认 250 ms 来自 v0.8 已经实机认可的约 5 帧 / 20 FPS 观感。
 * v0.9 起改成 GetTickCount 毫秒计时；v0.10 继续沿用，因此帧率变化时仍尽量保持用户配置的真实时长。
 *
 * INI 允许 0..10000：
 * - 0     = 瞬间完成；
 * - 250   = 默认；
 * - 10000 = 最慢 10 秒，主要用于调试。
 */
#define BAR_DARKNESS_MAX           256u
#define TRANSITION_DEFAULT_MS      250u
#define TRANSITION_MIN_MS            0u
#define TRANSITION_MAX_MS        10000u

/*
 * v0.11 继续让“电影式侧区”统一服务：
 * 1. 任意原版消息/对话框/提示/选择 UI；
 * 2. BattleManager 正在绘制的战斗。
 *
 * 不再做“玩家主动 / NPC / 自动脚本 / 剧情”的来源区分。
 * CINEMATIC_FILL_MAX = 256 表示当前左右侧区（16:9 为107、21:9 为240）完全被当前侧区样式接管；0 表示完全显示真实宽屏世界。
 */
#define CINEMATIC_FILL_MAX          256u

/*
 * 模糊背景不直接在最终 854×480 / 1120×480 上做大卷积，那样每帧会做太多乘加。
 * 我们先把中央 640×480 按“铺满当前宽高比后上下裁掉”的电影式背景规则缩到约 1/4 尺寸：
 *
 *   16:9 使用 214×120 工作图；21:9 使用 280×120 工作图。
 *
 * 工作图宽度由当前输出宽度向上除以4得到；v0.6 起根据实机反馈把两次 box blur 从 5×5 加重到 7×7，
 * 再双线性放大回左右侧区，同时进一步压暗背景。这样人物/文字在侧区里更难辨认，观感更接近电影式环境填充。
 */
#define CINEMATIC_LOW_H         120u
#define CINEMATIC_LOW_BYTES     (CINEMATIC_LOW_W * CINEMATIC_LOW_H * BYTES_PER_PIXEL)
#define CINEMATIC_BLUR_RADIUS     3u
#define CINEMATIC_BACKGROUND_KEEP 160u
#define CINEMATIC_EDGE_FEATHER     10u

/*
 * 切地图/Camera 临时中间态时，上一版会立刻把左右清黑，造成“黑一下又回来”。
 * v0.3 最多保留 4 个 Present 的上一张有效侧画，给新场景的 Camera/bounds 几帧时间完成建立。
 * 4 帧在 20 FPS 下约 200 ms，足够遮住瞬态，又不会长时间把旧地图留在左右。
 */
#define SIDE_HISTORY_GRACE_FRAMES 4u

#define ORIGINAL_EXTRA_X      64u
#define ORIGINAL_EXTRA_Y      48u
#define ORIGINAL_BACKING_W   768u
#define ORIGINAL_BACKING_H   576u

#define PRESENT_STAGING_H    576u
#define BYTES_PER_PIXEL        2u

#define WIDE_FRAME_BYTES     (OUTPUT_WIDTH * OUTPUT_HEIGHT * BYTES_PER_PIXEL)
#define STAGING_BYTES        (PRESENT_STAGING_W * PRESENT_STAGING_H * BYTES_PER_PIXEL)
#define BACKING_BACKUP_BYTES (ORIGINAL_BACKING_W * ORIGINAL_BACKING_H * BYTES_PER_PIXEL)
#define QUEUE_SNAPSHOT_BYTES (DRAW_QUEUE_MAX_ENTRIES * DRAW_QUEUE_ENTRY_BYTES)

/* 原函数都保留入口地址；因为我们只改 CALL 点，所以从 Hook 内直接调用入口不会再次进入自己。 */
static PFN_ThisVoid g_original_rebuild = (PFN_ThisVoid)FN_DISPLAY_REBUILD;
static PFN_ThisVoid g_original_present = (PFN_ThisVoid)FN_DISPLAY_PRESENT;
static PFN_ThisVoid g_original_render_queue = (PFN_ThisVoid)FN_RENDER_QUEUE;
static PFN_BinkCopyToBuffer g_original_bink_copy;

/*
 * 三块运行时内存：
 * - g_wide_frame：当前模式的 854×480 或 1120×480 合成帧；
 * - g_present_staging：只给 0x405A10/0x406330 读取的 982×576 或 1248×576 输出源；
 * - g_backing_backup：保存第一次中央绘制完成后的整张原版 768×576 backing；
 * - g_queue_snapshot：保存 0x434710 在第一次中央绘制前的最多 200 个队列项。
 *
 * 为什么要备份“整张” backing，而不是只备份中央 640：
 * 第二/第三次侧画也可能改到原版左右 64、上下 48 的工作边界。只恢复中央会把侧画残留留给下一阶段。
 * 备份并恢复全部 768×576，才能保证 Hook 返回后游戏看到的显存工作区和“只画过中央一次”完全一致。
 *
 * 全部只在安装时 VirtualAlloc 一次；每帧不 malloc/free，避免碎片和不必要的系统调用。
 */
static u8* g_wide_frame;
static u8* g_present_staging;
static u8* g_backing_backup;
static u8* g_queue_snapshot;

/*
 * v0.4 引入、v0.7 继续使用的两块低分辨率模糊工作区。
 * g_blur_low_a 先接收从中央 640×480 缩小后的图；
 * g_blur_low_b / g_blur_low_a 轮流承担两次 box blur，最后再从 g_blur_low_a 采样。
 * 它们和其他运行时缓冲一样只在安装时 VirtualAlloc 一次，每帧绝不 malloc/free。
 */
static u16* g_blur_low_a;
static u16* g_blur_low_b;

/*
 * g_side_frame_ready：当前帧是否已经由多 Camera 生成了新的左右世界。
 * g_have_side_history：是否至少曾经成功得到过一张左右世界，可用于切图瞬态的短暂保留。
 * g_side_miss_frames：连续多少帧没有得到新侧画；超过 grace 后才允许逐渐收黑，而不是瞬间黑。
 */
static int g_side_frame_ready;
static int g_have_side_history;
static u32 g_side_miss_frames;

/*
 * 黑边不是一个“有/无”的布尔开关，而是 0..256 的暗度。
 * 0 = 完全宽屏；256 = 左右全黑；中间值就是淡入淡出中的过渡帧。
 * 初始设为全黑，避免游戏刚建立窗口、第一张世界侧画还没生成时显示未初始化内容。
 */
static u32 g_bar_darkness = BAR_DARKNESS_MAX;

/*
 * 电影式侧区动画使用 0..256 的独立“滑入进度”。
 *
 * v0.7 的这个数值代表“整块侧区从真实世界淡到模糊的透明度”；
 * v0.11 继续把这个数值解释成“左右两块侧面板已经滑进来多少”：
 * - 0   ：两块面板完全在屏幕外，当前 16:9 / 21:9 真宽屏全部可见；
 * - 128 ：大约各覆盖一半侧区；
 * - 256 ：左右当前侧区宽度（107 或 240）完全由当前选择的模糊/纯黑面板覆盖。
 *
 * 中央 640 从头到尾不缩放、不移动；只有左右侧区的覆盖边界移动，所以不会把原版立绘、
 * 对话框、Battle HUD 等 4:3 UI 挤变形。标题 / Bink / 主 Interface 仍使用独立纯黑硬 4:3。
 */
static u32 g_cinematic_fill_amount;

/*
 * v0.9 已把对白显示规则彻底统一；v0.11 继续冻结这条已验收规则，只允许更换侧区像素样式：
 *
 * g_cinematic_latched：
 *     只要原版消息 Event 槽 0x46F670 非零，就立即进入电影式侧区。
 *     不再判断这张消息来自玩家、NPC、自动脚本还是剧情。
 *
 * g_cinematic_latch_event_id：
 *     记录进入电影式模式时的 active Event，仅用于日志和“对白之间空档仍继续保持”的生命周期。
 *
 * g_cinematic_scene_identity：
 *     来自 world manager +0x280。若非零 scene identity 真正发生变化，就认为已经换地图/场景，
 *     旧对话链的电影式锁存立即解除，避免把上一个场景的状态带过去。
 *
 * g_battle_latched：
 *     排序绘制队列里看到 BattleManager draw=0x4429F0 就置 1；普通 world manager 恢复后清 0。
 *
 * 侧画消息隔离仍然保留：
 * 即使消息恰好在 RenderQueue 中途才出现，左右 Camera 重放也会临时把 0x46F678/679 置零，
 * 这样不会再生成第二套立绘、对话框或选择 UI。
 */
static int g_cinematic_latched;
static u32 g_cinematic_latch_event_id;
static u32 g_cinematic_scene_identity;
static int g_battle_latched;

/*
 * INI 配置：两项分别控制“进入电影式/黑边状态”和“退出恢复宽屏状态”的总时长。
 * 默认都是 250 ms；读取失败或越界时 Runtime_ReadPluginIniU32 会自动回退默认值。
 */
static u32 g_transition_enter_ms = TRANSITION_DEFAULT_MS;
static u32 g_transition_exit_ms = TRANSITION_DEFAULT_MS;

/*
 * 两套视觉量各自有一份时间状态。
 * start_amount 是本次目标变化那一刻的进度；start_tick 是同一时刻的毫秒计数。
 * 以后每一帧直接按“已过去多少毫秒 / 总时长”算当前位置，不再靠每帧固定加 52。
 */
typedef struct TimedTransition {
    u32 start_amount;
    u32 target_amount;
    u32 start_tick;
    int initialized;
} TimedTransition;

static TimedTransition g_bar_transition;
static TimedTransition g_cinematic_transition;

/* 这些标志只负责“同一种日志不要每帧刷屏”。它们不参与任何游戏逻辑。 */
static int g_logged_first_rebuild;
static int g_logged_first_present;
static int g_logged_first_multipass;
static int g_logged_bad_geometry;
static int g_logged_bad_camera_bounds;
static int g_logged_bad_queue;
static int g_logged_title;
static int g_logged_movie;
static int g_logged_side_history;
static int g_logged_world_filter;
static int g_logged_interface;
static int g_logged_cinematic_fill;
static int g_logged_battle_fill;
static int g_logged_side_message_isolation;

/* 读取对象内部一个 32 位字段。用 volatile 是为了告诉编译器：这是游戏正在变化的共享内存，不要缓存旧值。 */
static u32 read_u32(void* base, u32 offset) {
    return *(volatile u32*)((u8*)base + offset);
}

/* 写对象内部一个 32 位字段。所有临时改写都在原函数返回后恢复原值。 */
static void write_u32(void* base, u32 offset, u32 value) {
    *(volatile u32*)((u8*)base + offset) = value;
}

/* Display+0x28 保存 DisplayMode 指针。没有 Display 或 mode 时必须返回 NULL，不能继续解引用。 */
static void* display_mode(void* display) {
    if (!display) return NULL;
    return *(void**)((u8*)display + DISPLAY_MODE_OBJECT);
}

/*
 * 只有当原版 backing 仍然是我们研究过的精确 768×576、16-bit、extra=64/48 时，
 * 才允许做多次 Camera 补画和 staging Present。
 *
 * 这道检查非常重要：Surface lost / 初始化中间态可能短暂没有像素指针。
 * 那种情况下宁可回退原版 Present，也绝不能拿 NULL 或错误行距去复制 480 行像素。
 */
static int original_display_geometry_ok(void* display) {
    void* mode;
    void* pixels;

    if (!display) return 0;
    mode = display_mode(display);
    if (!mode) return 0;

    pixels = *(void**)((u8*)display + DISPLAY_BACKING_PIXELS);
    if (!pixels) return 0;

    if (read_u32(display, DISPLAY_BACKING_WIDTH) != ORIGINAL_BACKING_W) return 0;
    if (read_u32(display, DISPLAY_BACKING_HEIGHT) != ORIGINAL_BACKING_H) return 0;
    if (read_u32(display, DISPLAY_BYTES_PER_PIXEL) != BYTES_PER_PIXEL) return 0;
    if (read_u32(display, DISPLAY_EXTRA_X) != ORIGINAL_EXTRA_X) return 0;
    if (read_u32(display, DISPLAY_EXTRA_Y) != ORIGINAL_EXTRA_Y) return 0;
    if (read_u32(mode, MODE_WIDTH) != LOGICAL_WIDTH) return 0;
    if (read_u32(mode, MODE_HEIGHT) != LOGICAL_HEIGHT) return 0;
    return 1;
}

/*
 * 判断是不是“绝对不能做左右世界补画”的硬 4:3 状态。
 *
 * v0.2 把任何 active Event 都放在这里，这会让普通 NPC 对话也突然出现黑边。
 * v0.3 把 Event 从硬保护中拿掉；v0.6 又进一步把“剧情/Battle电影式填充”和“硬4:3”彻底分开：
 * - 普通探索、普通 NPC 对白：允许真实 854 world-only 多 Camera；
 * - 已锁存剧情、Battle：在 Hook_RenderQueue 更早阶段就只执行中央原版队列一次，左右不再重放世界；
 *   Present 再从当前中央 640 生成电影式强模糊侧区，所以不会复制立绘、对话框、选择框或 Battle HUD；
 * - 标题 / 主 Interface / Bink：这些画面本身不是普通世界构图，仍然属于这里的硬 4:3，最后使用渐变纯黑侧区。
 *
 * 因此本函数只负责“是否必须纯黑硬4:3”，绝不能把剧情/Battle也混进来，否则会覆盖电影式模糊策略。
 */
static int frame_requires_hard_4x3(void) {
    void* movie = *(void* volatile*)GLOBAL_MOVIE_OBJECT;

    if (movie && *(volatile u8*)((u8*)movie + 0x0Au) != 0u) {
        if (!g_logged_movie) {
            g_logged_movie = 1;
            Runtime_Log("[4:3硬保护] Bink 播放中：不执行世界多 Camera 补画。");
        }
        return 1;
    }
    g_logged_movie = 0;

    if (*(void* volatile*)GLOBAL_TITLE_UI != NULL) {
        if (!g_logged_title) {
            g_logged_title = 1;
            Runtime_Log("[4:3硬保护] 标题界面：不执行世界多 Camera 补画。");
        }
        return 1;
    }
    g_logged_title = 0;

    if (*(void* volatile*)GLOBAL_INTERFACE_UI != NULL) {
        if (!g_logged_interface) {
            g_logged_interface = 1;
            Runtime_Log("[4:3硬保护] 主 Interface：不执行世界多 Camera 补画。");
        }
        return 1;
    }
    g_logged_interface = 0;
    return 0;
}

/*
 * 0x0046F670 是原版消息系统的当前 Event 槽。
 * v0.6 只在它与 active Event 同时匹配时才认定消息活跃，这会漏掉两类实机情况：
 * - Event 已先结束、对话框还留在屏幕上的尾帧；
 * - 某些提示性话语只保留消息生命周期，而 active Event 已经没有可对照值。
 *
 * v0.10 仍只问“0x46F670 是否非零”。它由 SHOW_MESSAGE/消息路径写入，消息结束路径会清零。
 * 当前规则已经统一，所以这里只回答“现在屏幕上是否存在原版消息 UI”，不再做任何来源分类。
 */
static int message_ui_is_active(void) {
    return ((*(volatile u32*)GLOBAL_MESSAGE_EVENT_ID) & 0xFFu) != 0u;
}

/*
 * 左右 Camera 侧画时临时屏蔽消息 UI，然后完整恢复。
 *
 * 0x40B050 的调用顺序里包含：
 *     ...世界背景/实体...
 *     0x403E30  消息状态推进/分支
 *     0x404800  消息主体/立绘/选择框绘制
 *     ...
 *
 * 两个函数的入口都由 0x46F678/679 控制：两个状态为 0 时会直接跳过消息工作。
 * 因此这里只在“第2/第3次侧画”的短窗口把它们置零；中央原版第一次绘制完全不动。
 * 这样同时解决：
 * - NPC 对话立绘重复；
 * - 对话框左右重复；
 * - “是/否”等选择 UI 重复；
 * - 同一消息状态被一帧推进 2~3 次的潜在副作用。
 */
static void render_side_world_without_message_ui(void* self) {
    u8 old_target = *(volatile u8*)GLOBAL_MESSAGE_TARGET_STATE;
    u8 old_current = *(volatile u8*)GLOBAL_MESSAGE_CURRENT_STATE;

    *(volatile u8*)GLOBAL_MESSAGE_TARGET_STATE = 0u;
    *(volatile u8*)GLOBAL_MESSAGE_CURRENT_STATE = 0u;

    g_original_render_queue(self);

    *(volatile u8*)GLOBAL_MESSAGE_CURRENT_STATE = old_current;
    *(volatile u8*)GLOBAL_MESSAGE_TARGET_STATE = old_target;

    if ((old_target != 0u || old_current != 0u) && !g_logged_side_message_isolation) {
        g_logged_side_message_isolation = 1;
        Runtime_Log("[侧画消息隔离] 左右 world 重放已临时屏蔽 0x46F678/679；消息 UI 只保留中央原版一次。 ");
    }
}

/*
 * 排序绘制队列的实时摘要。
 *
 * 为什么在真正调用 0x434710 之前扫描：
 * 0x434710 执行结束会清理队列。我们需要在它被消费前知道这一帧是否有：
 * - 普通 Scene world manager；
 * - BattleManager；
 * - Scene world manager 当前指向哪一块 Map/Scene runtime。
 */
typedef struct FrameQueueState {
    int valid;
    int has_scene_world;
    int has_battle_manager;
    u32 scene_identity;
} FrameQueueState;

/* 从一个队列对象取得 0x434710 真正会调用的 vtable[1]。 */
static u32 object_draw_method(void* object) {
    u32* vtable;
    if (!object) return 0u;
    vtable = *(u32**)object;
    if (!vtable) return 0u;
    return vtable[1];
}

/*
 * 扫描“还没被 0x434710 消费”的实时队列。
 *
 * Scene identity 使用 world manager +0x280：
 * 0x40B050 本身就在 0x40B072 用这个字段调用 0x40ADE0，所以这不是凭对象地址猜出来的偏移。
 *
 * BattleManager 的判定也只用已经闭合的机器协议：
 * ctor 把 vptr=0x4610D4；0x434710 调 vtable[1]；对应 draw=0x4429F0。
 */
static FrameQueueState scan_live_draw_queue(void) {
    FrameQueueState state;
    u32 count = *(volatile u32*)GLOBAL_DRAW_QUEUE_COUNT;
    u32 i;

    state.valid = 0;
    state.has_scene_world = 0;
    state.has_battle_manager = 0;
    state.scene_identity = 0u;

    if (count == 0u || count > DRAW_QUEUE_MAX_ENTRIES) return state;
    state.valid = 1;

    for (i = 0u; i < count; ++i) {
        u8* entry = (u8*)GLOBAL_DRAW_QUEUE_ENTRIES + i * DRAW_QUEUE_ENTRY_BYTES;
        void* object = *(void**)(entry + 4u);
        u32 draw = object_draw_method(object);

        if (draw == FN_SCENE_WORLD_DRAW) {
            state.has_scene_world = 1;
            if (object) {
                u32 identity = *(volatile u32*)((u8*)object + SCENE_WORLD_RUNTIME_OFFSET);
                if (identity != 0u) state.scene_identity = identity;
            }
        }

        if (draw == FN_BATTLE_MANAGER_DRAW) {
            state.has_battle_manager = 1;
        }
    }

    return state;
}

/*
 * Battle 模式不是靠一个不明语义的全局字节猜测，而是跟随真实绘制队列。
 *
 * 进入：
 *     看到 BattleManager draw=0x4429F0 -> 立即锁存。
 *
 * 退出：
 *     BattleManager 已消失，同时普通 Scene world manager 重新出现 -> 解除。
 *
 * 中间如果转场几帧两个对象都没有，就继续保持旧战斗状态，避免切换时模糊侧区闪一下。
 */
static void update_battle_latch(const FrameQueueState* queue_state) {
    if (!queue_state || !queue_state->valid) return;

    if (queue_state->has_battle_manager) {
        if (!g_battle_latched) {
            g_battle_latched = 1;
            g_logged_battle_fill = 1;
            Runtime_Log("[战斗侧区] 检测到 BattleManager draw=0x4429F0；战斗保持中央原版640，左右进入当前配置的侧区样式。");
        }
        return;
    }

    if (g_battle_latched && queue_state->has_scene_world) {
        g_battle_latched = 0;
        g_logged_battle_fill = 0;
        Runtime_Log("[战斗填充] BattleManager 已离开且普通场景 world manager 恢复；退出战斗模糊，恢复当前宽屏模式。");
    }
}

/*
 * 记录一次剧情锁存的开始。
 *
 * scene_identity 可能在 SceneRenderOverride / 切图中间态暂时为 0；
 * 这种情况下先锁剧情，等后面第一次重新看到可信 world manager 时再补记身份。
 */
static void latch_cinematic(u32 active_event, u32 scene_identity, const char* reason) {
    if (g_cinematic_latched) return;

    g_cinematic_latched = 1;
    g_cinematic_latch_event_id = active_event;
    g_cinematic_scene_identity = scene_identity;

    Runtime_LogHex("[消息锁存] 启用统一侧区模式，起始 Event=", active_event);
    if (scene_identity != 0u) Runtime_LogHex("[剧情锁存] 当前 scene identity=", scene_identity);
    if (reason) Runtime_Log(reason);
}

/*
 * 清除剧情锁存，同时保留 g_cinematic_fill_amount 的渐变状态。
 *
 * 这里不会把 fill amount 直接归零；Present 会按照 INI 的 ExitDurationMs
 * 让左右面板自然滑出并露出真实宽屏，所以消息链结束/切场景不会“啪”一下切换。
 */
static void clear_cinematic_latch(const char* reason) {
    if (!g_cinematic_latched) return;

    if (g_cinematic_latch_event_id != 0u) {
        Runtime_LogHex("[剧情锁存] 结束前记录的起始 Event=", g_cinematic_latch_event_id);
    }

    g_cinematic_latched = 0;
    g_cinematic_latch_event_id = 0u;
    g_cinematic_scene_identity = 0u;

    if (reason) Runtime_Log(reason);
}

/*
 * v0.11 继续使用的统一消息侧区状态机。
 *
 * 触发：
 *     只要 0x46F670 表示原版消息 UI 正在活跃，就立即锁存电影式侧区。
 *     不再区分玩家主动、NPC、提示、自动脚本、选择框或剧情来源。
 *
 * 保持：
 *     同一个 active Event 仍在运行时，即使两句对白之间消息 UI 短暂清零，也保持模糊。
 *
 * 退出：
 * - scene identity 从一个非零值切换到另一个非零值 -> 场景切换，解除；
 * - active Event 已归零，并且消息 UI 也清零 -> 当前消息链真正结束，解除。
 */
static void update_cinematic_latch(const FrameQueueState* queue_state) {
    u32 active_event = *(volatile u32*)GLOBAL_ACTIVE_EVENT_ID;
    u32 scene_identity = 0u;
    int message_active = message_ui_is_active();

    if (queue_state) scene_identity = queue_state->scene_identity;

    if (g_cinematic_latched) {
        if (g_cinematic_scene_identity != 0u &&
            scene_identity != 0u &&
            scene_identity != g_cinematic_scene_identity) {
            Runtime_LogHex("[对白锁存] 检测到新 scene identity=", scene_identity);
            clear_cinematic_latch("[对白锁存] 场景已经切换；旧消息链的电影式侧区开始退出。 ");
            return;
        }

        /*
         * 锁存开始时 world manager 可能暂时没有入队。
         * 后续第一次拿到可信 scene identity 时再补记，避免把“暂时取不到”误当成换场景。
         */
        if (g_cinematic_scene_identity == 0u && scene_identity != 0u) {
            g_cinematic_scene_identity = scene_identity;
            Runtime_LogHex("[对白锁存] 补记 scene identity=", scene_identity);
        }

        /*
         * 只要 Event 还在，就允许消息框中间短暂消失而继续保持。
         * 只有 Event 和消息 UI 两者都结束，才认为整段对话/提示真正完成。
         */
        if (active_event == 0u && !message_active) {
            clear_cinematic_latch("[对白锁存] active Event 与消息 UI 都已结束；电影式侧区开始向左右退出。 ");
        }
        return;
    }

    if (!message_active) return;

    latch_cinematic(active_event, scene_identity,
        "[对白判定] 检测到原版消息 UI：不区分来源，统一启用左右侧面板推入。 ");
}

/*
 * 计算普通黑边目标。
 *
 * v0.11 的可选侧区样式用于“任意消息锁存”和“Battle”；标题 / Bink / 主 Interface 继续保持纯黑硬 4:3。
 * 普通世界没有可靠侧画且切图 grace 耗尽时，也仍允许安全收黑。
 */
static u32 bar_target_darkness(int hard_4x3) {
    return hard_4x3 ? BAR_DARKNESS_MAX : 0u;
}

/*
 * 计算统一侧区动画目标。
 *
 * hard 4:3 拥有最高优先级，所以标题/Bink/主 Interface 时一定返回 0；
 * 其它状态只有统一消息锁存或 Battle 锁存会返回满值。
 */
static u32 cinematic_fill_target(int hard_4x3) {
    if (hard_4x3) return 0u;
    if (g_cinematic_latched || g_battle_latched) return CINEMATIC_FILL_MAX;
    return 0u;
}

/*
 * 把原版 backing 中真正可见的 640×480 核心复制到 g_wide_frame 的指定位置。
 *
 * backing 每行 768 像素，核心左上角是 (64,48)。
 * dst_x 通常是 107，也就是让原版 640 精确位于 854 中央。
 */
static void copy_core_to_wide(void* display, u32 dst_x) {
    u8* backing = *(u8**)((u8*)display + DISPLAY_BACKING_PIXELS);
    u32 row;

    for (row = 0; row < LOGICAL_HEIGHT; ++row) {
        u8* src = backing + (((ORIGINAL_EXTRA_Y + row) * ORIGINAL_BACKING_W + ORIGINAL_EXTRA_X) * BYTES_PER_PIXEL);
        u8* dst = g_wide_frame + ((row * OUTPUT_WIDTH + dst_x) * BYTES_PER_PIXEL);
        Runtime_MemCopy(dst, src, LOGICAL_WIDTH * BYTES_PER_PIXEL);
    }
}

/*
 * 保存/恢复整张原版 768×576 backing。
 *
 * 第一次中央绘制结束后先 save_original_backing()；左右侧画都完成以后再 restore_original_backing()。
 * 这样侧画无论碰了核心、64 像素横向 extra，还是 48 像素纵向 extra，都不会污染后续原版逻辑。
 */
static void save_original_backing(void* display) {
    u8* backing = *(u8**)((u8*)display + DISPLAY_BACKING_PIXELS);
    Runtime_MemCopy(g_backing_backup, backing, BACKING_BACKUP_BYTES);
}

static void restore_original_backing(void* display) {
    u8* backing = *(u8**)((u8*)display + DISPLAY_BACKING_PIXELS);
    Runtime_MemCopy(backing, g_backing_backup, BACKING_BACKUP_BYTES);
}

/*
 * 把“向左移动 Camera 后”的画面中，真正属于原画面左边的新世界内容拿出来。
 *
 * 例：还能向左移动完整 107 像素：
 * - 新 Camera = oldCamera - 107；
 * - 新画面的 screen x=0..106 就是原画面的世界 x=-107..-1；
 * - 所以直接放到输出 x=0..106。
 *
 * 如果地图边缘只允许向左移动 30：
 * - 只有 30 像素是真实额外世界；
 * - 它们放在输出 x=77..106；
 * - 更左边 0..76 没有地图内容，保持黑色。
 */
static void copy_left_extension(void* display, u32 shift) {
    u8* backing;
    u32 row;
    u32 dst_x;

    if (shift == 0u) return;
    backing = *(u8**)((u8*)display + DISPLAY_BACKING_PIXELS);
    dst_x = SIDE_WIDTH - shift;

    for (row = 0; row < LOGICAL_HEIGHT; ++row) {
        u8* src = backing + (((ORIGINAL_EXTRA_Y + row) * ORIGINAL_BACKING_W + ORIGINAL_EXTRA_X) * BYTES_PER_PIXEL);
        u8* dst = g_wide_frame + ((row * OUTPUT_WIDTH + dst_x) * BYTES_PER_PIXEL);
        Runtime_MemCopy(dst, src, shift * BYTES_PER_PIXEL);
    }
}

/*
 * 把“向右移动 Camera 后”的画面中，属于原画面右边的新世界内容拿出来。
 *
 * 完整移动 107 时，原世界 x=640..746 在新画面中的 screen x=533..639，
 * 因为 640 - 107 = 533。
 *
 * 如果地图边缘只能右移 30，则 screen x=610..639 对应新增的 30 像素，
 * 放到宽屏输出 x=747..776；输出最右边剩余区域继续保持黑色。
 */
static void copy_right_extension(void* display, u32 shift) {
    u8* backing;
    u32 row;
    u32 src_x;

    if (shift == 0u) return;
    backing = *(u8**)((u8*)display + DISPLAY_BACKING_PIXELS);
    src_x = LOGICAL_WIDTH - shift;

    for (row = 0; row < LOGICAL_HEIGHT; ++row) {
        u8* src = backing + (((ORIGINAL_EXTRA_Y + row) * ORIGINAL_BACKING_W + ORIGINAL_EXTRA_X + src_x) * BYTES_PER_PIXEL);
        u8* dst = g_wide_frame + ((row * OUTPUT_WIDTH + (SIDE_WIDTH + LOGICAL_WIDTH)) * BYTES_PER_PIXEL);
        Runtime_MemCopy(dst, src, shift * BYTES_PER_PIXEL);
    }
}

/*
 * v0.3 的 Camera 计划。
 *
 * center_x 不是“永久写回游戏”的相机，而只是这一帧真正用于 world draw 的临时中心 Camera。
 * 左右补画全部围绕 center_x 展开；Hook 返回前仍会把 GLOBAL_CAMERA_X 恢复成游戏原值。
 */
typedef struct CameraPlan {
    i32 center_x;
    u32 left_shift;
    u32 right_shift;
} CameraPlan;

/*
 * 根据原版 0x44B300 / 0x44B360 的边界，计算“当前宽屏可见范围不能越过地图”的 Camera。
 *
 * 原版只保证 640 宽：
 *     minX <= cameraX <= maxX - 640
 *
 * v0.10 真正显示的是：
 *     [centerX - SIDE_WIDTH, centerX + 640 + SIDE_WIDTH)
 *
 * SIDE_WIDTH 由 INI 模式决定：16:9 是107，21:9 是240。
 * 因此地图宽度至少达到当前 OUTPUT_WIDTH 时，centerX 还必须满足：
 *
 *     centerX >= minX + SIDE_WIDTH
 *     centerX <= maxX - 640 - SIDE_WIDTH
 *
 * 这样角色走到地图边缘时，镜头会更早停止，而不是把地图边界拉进可见区域。
 * 21:9 因为视野更宽，所以会比16:9更早停止，这是正确的宽屏相机行为。
 *
 * 如果地图本身小于当前输出宽度，就不可能凭空制造不存在的地图。
 * 此时仍把 Camera 保持在原版合法范围，只使用左右实际存在的额外空间；剩余部分由安全回退处理。
 *
 * 切地图时 Camera 可能短暂处在旧 clamp 之外，所以这里会把“用于绘制的临时 center”夹进当前可信边界，
 * 但 Hook 返回前始终恢复游戏自己的原 Camera 值。
 */
static int calculate_camera_plan(i32 camera_x, CameraPlan* plan) {
    i32 min_x = *(volatile i32*)GLOBAL_CAMERA_MIN_X;
    i32 max_x = *(volatile i32*)GLOBAL_CAMERA_MAX_X;
    i32 viewport_w = *(volatile i32*)GLOBAL_CAMERA_VIEW_W;
    i32 map_width;
    i32 normal_max_camera;
    i32 wide_min_camera;
    i32 wide_max_camera;
    i32 center;
    i32 left_room;
    i32 right_room;

    if (!plan) return 0;
    plan->center_x = camera_x;
    plan->left_shift = 0u;
    plan->right_shift = 0u;

    /* 0 或负 viewport，以及反向 bounds，都属于场景还没建立好的瞬态，不能拿来做地址相关绘制。 */
    if (viewport_w <= 0) return 0;
    if (max_x <= min_x) return 0;

    map_width = max_x - min_x;
    normal_max_camera = max_x - viewport_w;

    /* 连原版一个 viewport 都放不下时，不尝试伪造额外地图；交给上一张侧画的 grace 机制过渡。 */
    if (normal_max_camera < min_x) return 0;

    /*
     * 地图至少有 854 像素宽时，可以完整保证两边各 107 都是真实世界。
     * 这里不是把 viewportW 改成 854；renderer 仍只画 640，我们只是改变三次 640 draw 的 Camera 位置。
     */
    if (map_width >= (i32)OUTPUT_WIDTH) {
        wide_min_camera = min_x + (i32)SIDE_WIDTH;
        wide_max_camera = max_x - viewport_w - (i32)SIDE_WIDTH;

        center = camera_x;
        if (center < wide_min_camera) center = wide_min_camera;
        if (center > wide_max_camera) center = wide_max_camera;

        plan->center_x = center;
        plan->left_shift = SIDE_WIDTH;
        plan->right_shift = SIDE_WIDTH;
        return 1;
    }

    /*
     * 地图宽度如果介于“原版640”与“当前输出宽度”之间，就不可能凭空得到完整宽屏世界。
     *
     * 旧做法会继续跟随玩家原 Camera，然后分别计算左右还能走多少；这样在21:9下很容易出现：
     * 地图本身只有900像素宽，但因为 Camera 靠左，左边240几乎全空、右边却塞满真实地图，构图很偏。
     *
     * v0.10 改成更符合“地图边界不能超过相机范围”的规则：
     * 既然整张地图已经比当前屏幕还窄，就不再让 Camera 在里面来回跟随，而是把整张可见地图尽量居中。
     *
     * total_extra 是地图在640中央之外实际还拥有多少额外像素。
     * 把它一半分到左、一半分到右；奇数像素多出来的1个放右边。
     * 例如21:9下地图宽900：900-640=260，所以左130、右130，剩余空区也是左右各110，画面完全对称。
     */
    {
        i32 total_extra = map_width - viewport_w;
        if (total_extra < 0) return 0;

        left_room = total_extra / 2;
        right_room = total_extra - left_room;

        /* 理论上 map_width < OUTPUT_WIDTH 已经保证两边不会超过 SIDE_WIDTH，这里仍做保险夹取。 */
        if (left_room < 0) left_room = 0;
        if (right_room < 0) right_room = 0;
        if (left_room > (i32)SIDE_WIDTH) left_room = (i32)SIDE_WIDTH;
        if (right_room > (i32)SIDE_WIDTH) right_room = (i32)SIDE_WIDTH;

        center = min_x + left_room;
        if (center < min_x) center = min_x;
        if (center > normal_max_camera) center = normal_max_camera;

        plan->center_x = center;
        plan->left_shift = (u32)left_room;
        plan->right_shift = (u32)right_room;
    }
    return 1;
}

static void sdk_publish_geometry(u32 display_mode_value, u32 projection_scope,
                                 i32 effective_camera_x,
                                 i32 original_camera_x,
                                 u32 left_world_width,
                                 u32 right_world_width) {
    CastleU32 runtime_generation = 0u;
    if (!g_sdk_services_ready || !g_sdk_display_api || !g_sdk_display_provider) return;
    /* 单一游戏线程先把序号改成奇数，读线程看到奇数就不会使用中间状态。 */
    ++g_sdk_geometry_sequence;
    Runtime_MemZero(&g_sdk_geometry, sizeof(g_sdk_geometry));
    g_sdk_geometry.magic = CASTLE_DISPLAY_GEOMETRY_MAGIC;
    g_sdk_geometry.struct_size = CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1;
    g_sdk_geometry.api_version = CASTLE_DISPLAY_API_VERSION_1;
    g_sdk_geometry.output_width = OUTPUT_WIDTH;
    g_sdk_geometry.output_height = OUTPUT_HEIGHT;
    g_sdk_geometry.logical_width = LOGICAL_WIDTH;
    g_sdk_geometry.logical_height = LOGICAL_HEIGHT;
    g_sdk_geometry.center_x = (i32)SIDE_WIDTH;
    g_sdk_geometry.center_y = 0;
    g_sdk_geometry.center_width = (i32)LOGICAL_WIDTH;
    g_sdk_geometry.center_height = (i32)LOGICAL_HEIGHT;
    g_sdk_geometry.effective_camera_x = effective_camera_x;
    g_sdk_geometry.effective_camera_y = *(volatile i32*)GLOBAL_CAMERA_Y;
    g_sdk_geometry.original_camera_x = original_camera_x;
    g_sdk_geometry.original_camera_y = g_sdk_geometry.effective_camera_y;
    g_sdk_geometry.left_world_width = left_world_width;
    g_sdk_geometry.right_world_width = right_world_width;
    g_sdk_geometry.display_mode = display_mode_value;
    g_sdk_geometry.projection_scope = projection_scope;
    g_sdk_geometry.transition_value = g_cinematic_fill_amount;
    g_sdk_geometry.transition_max = CINEMATIC_FILL_MAX;
    if (g_sdk_display_api->PublishDisplayGeometry(g_sdk_display_provider,
            &g_sdk_geometry, &runtime_generation) == CASTLE_OK) {
        g_sdk_geometry.generation = runtime_generation;
    }
    /* 全部字段（包括 Runtime 分配的 generation）完成后再恢复偶数。 */
    ++g_sdk_geometry_sequence;
}

static int sdk_copy_geometry_snapshot(CastleDisplayGeometryV1* output) {
    u32 attempt;
    /* 调用方没有提供接收结构时，不能写内存，也不能声称复制成功。 */
    if (!output) return 0;
    /*
     * 不在游戏绘制路径和其它插件之间加一把会睡眠的锁。这里最多尝试八次：通常第一次
     * 就成功；如果恰好撞上游戏线程发布画面，则短暂重试。八次后仍不稳定就返回“未就绪”，
     * 让消费者跳过这一帧，避免为了一个 Marker 卡住整场游戏。
     */
    for (attempt = 0u; attempt < 8u; ++attempt) {
        u32 before = g_sdk_geometry_sequence;
        u32 after;
        /* 奇数说明游戏线程仍在逐字段写入；此时不能复制。 */
        if ((before & 1u) != 0u) continue;
        /*
         * 先复制整张表，再重新读取序号。复制途中即使发布线程开始更新，下面的第二次
         * 序号检查也会发现变化并丢弃这份半旧半新的结果。
         */
        Runtime_MemCopy(output, &g_sdk_geometry, sizeof(*output));
        after = g_sdk_geometry_sequence;
        /* 前后同为同一个偶数，才能证明整张快照来自一次完整发布。 */
        if (before == after && (after & 1u) == 0u) return 1;
    }
    /* 连续八次都撞上发布时不返回猜测值，由上层转换成 CASTLE_ERROR_NOT_READY。 */
    return 0;
}

/*
 * 从队列对象取得 vtable[1]，也就是 0x434710 真正会 CALL 的 draw 方法地址。
 *
 * 队列条目格式已经静态闭合：
 *     +0：排序键
 *     +4：对象指针
 * 对象第一个 DWORD 是 vtable；vtable 第二个 DWORD（+4）就是 draw。
 */
static u32 snapshot_entry_draw_method(u32 index) {
    u8* entry;
    void* object;
    u32* vtable;

    if (index >= DRAW_QUEUE_MAX_ENTRIES) return 0u;
    entry = g_queue_snapshot + index * DRAW_QUEUE_ENTRY_BYTES;
    object = *(void**)(entry + 4u);
    if (!object) return 0u;

    vtable = *(u32**)object;
    if (!vtable) return 0u;
    return vtable[1];
}

/*
 * 中央原版绘制仍然使用完整队列；左右侧画则只恢复“世界总管理对象”。
 *
 * 这是 v0.3 修复 Battle HUD 重复的关键：
 * - 0x40B150 把场景管理对象用 key=0xBB8 放入队列；
 * - 它的 vtable[1] = 0x40B050；
 * - 0x40B050 内部自己会画背景、实体、Camera 相关世界层；
 * - 战斗 HUD、命令菜单、手形光标、普通对白框等外层 SF2/UI 是别的队列对象。
 *
 * 所以侧画只重放 0x40B050，可以让“世界跟 Camera 一起多画”，同时 UI 永远只保留中央原版那一次。
 * 这比维护一张越来越长的 UI 黑名单更稳：新 UI 默认不会被重复，只有明确的 world manager 才被允许进入侧画。
 *
 * 返回值是实际恢复了多少个 world 条目。
 * v0.11 只会在“没有任何消息 UI、也不在 Battle”的普通探索路径调用它；
 * 任意消息锁存和 Battle 都会在更上层直接退出多 Camera。
 */
static u32 restore_world_only_draw_queue(u32 count) {
    u32 src_index;
    u32 dst_count = 0u;

    for (src_index = 0u; src_index < count; ++src_index) {
        u32 draw_method = snapshot_entry_draw_method(src_index);
        if (draw_method == FN_SCENE_WORLD_DRAW) {
            u8* src = g_queue_snapshot + src_index * DRAW_QUEUE_ENTRY_BYTES;
            u8* dst = (u8*)GLOBAL_DRAW_QUEUE_ENTRIES + dst_count * DRAW_QUEUE_ENTRY_BYTES;
            Runtime_MemCopy(dst, src, DRAW_QUEUE_ENTRY_BYTES);
            ++dst_count;
        }
    }

    *(volatile u32*)GLOBAL_DRAW_QUEUE_COUNT = dst_count;
    return dst_count;
}

/*
 * v0.11 当前 RenderQueue Hook。
 *
 * 普通探索仍然沿用已经实机成功的三段 world-only 多 Camera：
 *     16:9：左107 + 中央640 + 右107 = 854
 *     21:9：左240 + 中央640 + 右240 = 1120
 *
 * 但在真正绘制前先扫描当前排序队列，得到两个新的高层状态：
 * - 统一对白锁存：任意原版消息一旦出现，就在同一 Event/场景内持续保持同一种侧区构图；
 * - Battle 锁存：BattleManager 出现在队列时，战斗只执行中央原版一次，左右交给当前侧区样式。
 *
 * 这样 Battle 不再重放任何 world/UI；所有对白都使用同一种中央640构图，普通探索无消息时才恢复实时真宽屏。
 */
static void FASTCALL Hook_RenderQueue(void* self, void* unused_edx) {
    void* display;
    i32 original_camera_x;
    u32 count;
    CameraPlan plan;
    u32 world_count;
    FrameQueueState queue_state;

    (void)unused_edx;
    g_side_frame_ready = 0;

    if (!self) return;
    original_camera_x = *(volatile i32*)GLOBAL_CAMERA_X;

    /*
     * 一定要在第一次 g_original_render_queue() 之前扫描。
     * 0x434710 运行后会消费/清理排序队列，再扫就看不到 BattleManager 和 scene world manager 了。
     */
    queue_state = scan_live_draw_queue();
    update_battle_latch(&queue_state);
    update_cinematic_latch(&queue_state);

    /* 标题/Bink/主 Interface 没有普通世界补画意义，直接保持原版中央绘制一次。 */
    if (frame_requires_hard_4x3()) {
        sdk_publish_geometry(CASTLE_DISPLAY_HARD_4_3, CASTLE_PROJECTION_NONE,
            original_camera_x, original_camera_x, 0u, 0u);
        g_original_render_queue(self);
        return;
    }

    /*
     * 任意消息锁存或 Battle 期间绝不再做左右 Camera 重放。
     *
     * 这条规则同时解决两类实机问题：
     * - 任意消息 UI 都应该只保留中央原版一次，不需要再为主动/自动来源维护两套视觉规则；
     * - 战斗里继续重放 world manager 仍有场景/对象层风险，当前继续用中央640 + 左右侧面板稳定收口。
     *
     * 中央完整队列仍由原版正常执行一次；Present 再从中央 640 生成模糊侧区。
     */
    if (g_cinematic_latched || g_battle_latched) {
        sdk_publish_geometry(g_battle_latched ? CASTLE_DISPLAY_BATTLE_4_3 :
            CASTLE_DISPLAY_CINEMATIC_4_3, CASTLE_PROJECTION_NONE,
            original_camera_x, original_camera_x, 0u, 0u);
        g_original_render_queue(self);
        return;
    }

    display = *(void* volatile*)GLOBAL_DISPLAY_OBJECT;
    if (!original_display_geometry_ok(display)) {
        if (!g_logged_bad_geometry) {
            g_logged_bad_geometry = 1;
            Runtime_Log("[多Camera] Display 暂时不是完整 768×576/640×480 几何；本帧走中央，侧边交给历史帧过渡。");
        }
        sdk_publish_geometry(CASTLE_DISPLAY_TRANSITION, CASTLE_PROJECTION_NONE,
            original_camera_x, original_camera_x, 0u, 0u);
        g_original_render_queue(self);
        return;
    }
    g_logged_bad_geometry = 0;

    count = *(volatile u32*)GLOBAL_DRAW_QUEUE_COUNT;
    if (count == 0u || count > DRAW_QUEUE_MAX_ENTRIES) {
        if (count > DRAW_QUEUE_MAX_ENTRIES && !g_logged_bad_queue) {
            g_logged_bad_queue = 1;
            Runtime_Log("[多Camera] 绘制队列超过 200 项静态上限；本帧不重放，避免写越未知队列区。");
        }
        sdk_publish_geometry(CASTLE_DISPLAY_TRANSITION, CASTLE_PROJECTION_NONE,
            original_camera_x, original_camera_x, 0u, 0u);
        g_original_render_queue(self);
        return;
    }
    g_logged_bad_queue = 0;

    if (!calculate_camera_plan(original_camera_x, &plan)) {
        if (!g_logged_bad_camera_bounds) {
            g_logged_bad_camera_bounds = 1;
            Runtime_Log("[多Camera] 当前地图 Camera bounds 尚未建立；本帧只画中央，不再瞬间强制清黑左右。");
        }
        sdk_publish_geometry(CASTLE_DISPLAY_TRANSITION, CASTLE_PROJECTION_NONE,
            original_camera_x, original_camera_x, 0u, 0u);
        g_original_render_queue(self);
        return;
    }
    g_logged_bad_camera_bounds = 0;

    /*
     * 0x434710 第一次执行完会清空队列，因此必须在任何 draw 发生前保存完整条目。
     * 中央仍然需要“原版完整队列”，所以第一次调用前不做过滤。
     */
    Runtime_MemCopy(g_queue_snapshot, (const void*)GLOBAL_DRAW_QUEUE_ENTRIES,
                    count * DRAW_QUEUE_ENTRY_BYTES);

    /*
     * 先把用于绘制的 Camera 临时换成“当前输出宽度安全”的 center。
     * 如果原 Camera 已经处在地图边缘，plan.center_x 会按当前 SIDE_WIDTH（107或240）提前向地图内部夹住。
     */
    *(volatile i32*)GLOBAL_CAMERA_X = plan.center_x;
    g_original_render_queue(self);

    /* 中央 640 来自刚刚真正用 wide-safe Camera 画好的原版 backing。 */
    copy_core_to_wide(display, SIDE_WIDTH);

    /* 侧画会覆盖 backing，所以先保存“正确中央帧”的整张 768×576。 */
    save_original_backing(display);

    /*
     * 先试着只恢复 world manager。返回 0 说明这一帧队列里没有已确认的世界总管理对象；
     * 这种情况不要退回“整队列重放”，否则 Battle/UI 重复问题会重新出现。
     */
    world_count = restore_world_only_draw_queue(count);
    if (world_count == 0u) {
        *(volatile i32*)GLOBAL_CAMERA_X = original_camera_x;
        restore_original_backing(display);
        if (!g_logged_world_filter) {
            g_logged_world_filter = 1;
            Runtime_Log("[世界队列过滤] 当前帧没有找到 draw=0x40B050 的 world manager；保留中央，侧边等待下一张有效世界帧。");
        }
        sdk_publish_geometry(CASTLE_DISPLAY_TRANSITION, CASTLE_PROJECTION_NONE,
            plan.center_x, original_camera_x, 0u, 0u);
        return;
    }
    g_logged_world_filter = 0;

    /*
     * 从这里开始已经确定可以画真实 world，因此清空左右旧内容，避免当前地图和上一地图混在同一帧。
     * 对于 >=854 的正常地图，plan.left/right 都是完整 107，所以最终不会留下边界黑条。
     */
    {
        u32 row;
        for (row = 0u; row < OUTPUT_HEIGHT; ++row) {
            u8* line = g_wide_frame + row * OUTPUT_WIDTH * BYTES_PER_PIXEL;
            Runtime_MemZero(line, SIDE_WIDTH * BYTES_PER_PIXEL);
            Runtime_MemZero(line + (SIDE_WIDTH + LOGICAL_WIDTH) * BYTES_PER_PIXEL,
                            SIDE_WIDTH * BYTES_PER_PIXEL);
        }
    }

    /* 第 2 次：world-only 队列，Camera 向左。restore_world_only_draw_queue 已经为第一次侧画准备好队列。 */
    if (plan.left_shift > 0u) {
        *(volatile i32*)GLOBAL_CAMERA_X = plan.center_x - (i32)plan.left_shift;
        render_side_world_without_message_ui(self);
        copy_left_extension(display, plan.left_shift);
    } else {
        /* 如果左边不需要画，刚恢复的队列还没被消费；下一次 right 前会重新覆盖 count。 */
        *(volatile u32*)GLOBAL_DRAW_QUEUE_COUNT = 0u;
    }

    /* 第 3 次：重新从快照筛一次 world-only，因为上一次 0x434710 已经把队列清空。 */
    if (plan.right_shift > 0u) {
        restore_world_only_draw_queue(count);
        *(volatile i32*)GLOBAL_CAMERA_X = plan.center_x + (i32)plan.right_shift;
        render_side_world_without_message_ui(self);
        copy_right_extension(display, plan.right_shift);
    }

    /*
     * 绝对不能把“侧画最后一次的 backing”留给游戏。
     * 先恢复进入 Hook 时游戏自己的 Camera 值，再逐字节恢复第一次中央绘制后的 768×576。
     */
    *(volatile i32*)GLOBAL_CAMERA_X = original_camera_x;
    restore_original_backing(display);

    g_side_frame_ready = 1;
    g_have_side_history = 1;
    g_side_miss_frames = 0u;

    sdk_publish_geometry(g_cinematic_fill_amount != 0u ?
        CASTLE_DISPLAY_TRANSITION : CASTLE_DISPLAY_WIDE_WORLD,
        g_cinematic_fill_amount != 0u ? CASTLE_PROJECTION_NONE :
                                        CASTLE_PROJECTION_FULL_OUTPUT,
        plan.center_x, original_camera_x, plan.left_shift, plan.right_shift);

    if (!g_logged_first_multipass) {
        g_logged_first_multipass = 1;
        Runtime_Log("[多Camera] world-only 三段合成首次成功；普通 UI 不进入左右重放队列。");
        Runtime_LogHex("[多Camera] 游戏原 CameraX=", (u32)original_camera_x);
        Runtime_LogHex("[多Camera] 当前宽屏安全中央 CameraX=", (u32)plan.center_x);
        Runtime_LogHex("[多Camera] 左补画像素=", plan.left_shift);
        Runtime_LogHex("[多Camera] 右补画像素=", plan.right_shift);
    }
}

/*
 * 0x405BD0 负责建立真正显示到屏幕上的 DirectDraw surface / 窗口客户区。
 * 这里沿用已经实机通过的办法：只在进入 0x405BD0 的短时间内把 mode 改成当前 854×480 或 1120×480。
 *
 * POC1 有一个小工程缺陷：只保存/恢复了 width，没有恢复 height。
 * 本版把 width 和 height 都保存并完整恢复，保证游戏逻辑长期仍只看到 640×480。
 */
static void FASTCALL Hook_DisplayRebuild(void* self, void* unused_edx) {
    void* mode;
    u32 old_width;
    u32 old_height;

    (void)unused_edx;
    if (!self) return;

    mode = display_mode(self);
    if (!mode) {
        g_original_rebuild(self);
        return;
    }

    old_width = read_u32(mode, MODE_WIDTH);
    old_height = read_u32(mode, MODE_HEIGHT);

    write_u32(mode, MODE_WIDTH, OUTPUT_WIDTH);
    write_u32(mode, MODE_HEIGHT, OUTPUT_HEIGHT);
    g_original_rebuild(self);
    write_u32(mode, MODE_HEIGHT, old_height);
    write_u32(mode, MODE_WIDTH, old_width);

    if (!g_logged_first_rebuild) {
        g_logged_first_rebuild = 1;
        Runtime_Log("[显示] DirectDraw/窗口物理目标已按当前 INI 宽屏模式建立；返回后游戏逻辑 mode 已恢复 640×480。");
    }
}

/*
 * 用真实毫秒把一个 0..256 的视觉量推进到目标。
 *
 * 关键点不是“每帧走多少”，而是记录目标发生变化的时刻：
 *     amount = start + (target - start) * elapsed / duration
 *
 * 这样 20 FPS、30 FPS 或临时掉帧时，总动画时长仍由 INI 的毫秒值决定。
 * GetTickCount 发生 32 位回绕时，无符号 `now - start_tick` 仍能得到正确的短时间差。
 */
static u32 advance_timed_amount(
    u32 current, u32 target,
    TimedTransition* transition,
    u32 enter_ms, u32 exit_ms)
{
    u32 now;
    u32 duration;
    u32 elapsed;
    u32 distance;
    u32 moved;

    if (!transition) return target;
    if (target > 256u) target = 256u;

    now = Runtime_GetTickCountMs();

    /*
     * 第一次调用或目标发生变化时，从“当前已经走到的位置”重新开始。
     * 因此如果动画进行到一半突然反向，不会跳回 0 或 256，而是从当前画面平滑反向。
     */
    if (!transition->initialized || transition->target_amount != target) {
        transition->initialized = 1;
        transition->start_amount = current;
        transition->target_amount = target;
        transition->start_tick = now;
    }

    if (current == target) {
        transition->start_amount = current;
        transition->start_tick = now;
        return current;
    }

    duration = (target > transition->start_amount) ? enter_ms : exit_ms;
    if (duration == 0u) return target;

    elapsed = now - transition->start_tick;
    if (elapsed >= duration) return target;

    if (target >= transition->start_amount) {
        distance = target - transition->start_amount;
        moved = (distance * elapsed) / duration;
        return transition->start_amount + moved;
    }

    distance = transition->start_amount - target;
    moved = (distance * elapsed) / duration;
    return transition->start_amount - moved;
}

/* 标题/Bink/主 Interface 的纯黑侧边也使用同一组 INI 进入/退出时间。 */
static void advance_bar_darkness(u32 target) {
    if (target > BAR_DARKNESS_MAX) target = BAR_DARKNESS_MAX;
    g_bar_darkness = advance_timed_amount(
        g_bar_darkness, target, &g_bar_transition,
        g_transition_enter_ms, g_transition_exit_ms);
}

/*
 * 把一个 RGB565 像素按 0..256 的“保留亮度”缩暗。
 *
 * 原游戏 backing 是 16-bit；这里把红 5bit、绿 6bit、蓝 5bit 分开乘比例，再拼回去。
 * 只做乘法和位移，不需要浮点数，也不需要 CRT。
 * keep=256 时原样返回；keep=0 时就是纯黑。
 */
static u16 darken_rgb565(u16 pixel, u32 keep) {
    u32 r = (u32)((pixel >> 11) & 0x1Fu);
    u32 g = (u32)((pixel >> 5) & 0x3Fu);
    u32 b = (u32)(pixel & 0x1Fu);

    r = (r * keep) >> 8;
    g = (g * keep) >> 8;
    b = (b * keep) >> 8;
    return (u16)((r << 11) | (g << 5) | b);
}

/*
 * 电影式左右面板和纯黑侧边共用相同毫秒配置，但状态独立。
 * 因此标题黑边正在退出时，Battle 模糊仍可以从自己的当前位置正常进入，不会互相抢进度。
 */
static void advance_cinematic_fill(u32 target) {
    if (target > CINEMATIC_FILL_MAX) target = CINEMATIC_FILL_MAX;
    g_cinematic_fill_amount = advance_timed_amount(
        g_cinematic_fill_amount, target, &g_cinematic_transition,
        g_transition_enter_ms, g_transition_exit_ms);
}

/*
 * 在 RGB565 上按 0..256 比例混合两个像素。
 * amount=0 返回 a；amount=256 返回 b。
 * 必须把 R/G/B 三个通道拆开混合，不能直接拿整个 16 位整数做线性插值，
 * 否则低位蓝色进位会污染绿色和红色，看起来会出现奇怪的色带。
 */
static u16 blend_rgb565(u16 a, u16 b, u32 amount) {
    u32 inv;
    u32 ar, ag, ab;
    u32 br, bg, bb;
    u32 r, g, blue;

    if (amount == 0u) return a;
    if (amount >= 256u) return b;
    inv = 256u - amount;

    ar = (a >> 11) & 0x1Fu;
    ag = (a >> 5) & 0x3Fu;
    ab = a & 0x1Fu;
    br = (b >> 11) & 0x1Fu;
    bg = (b >> 5) & 0x3Fu;
    bb = b & 0x1Fu;

    r = (ar * inv + br * amount) >> 8;
    g = (ag * inv + bg * amount) >> 8;
    blue = (ab * inv + bb * amount) >> 8;
    return (u16)((r << 11) | (g << 5) | blue);
}

/*
 * 把最终 staging 中的中央 640×480 缩成“电影背景源”。
 *
 * v0.9 只有 854×480，所以可以把 16:9 的裁切关系写死成：
 *     640×480 -> 取中央约 640×360 -> 缩成 214×120
 *
 * v0.10 增加 1120×480 后，如果还写死 214×120，21:9 两侧就会把模糊图横向拉得太厉害。
 * 所以现在低分辨率宽度也跟着当前输出宽度变化：
 *
 *     854  -> ceil(854 / 4)  = 214
 *     1120 -> ceil(1120 / 4) = 280
 *
 * 高度仍固定 120。为了保持“铺满当前宽高比后上下裁切”的电影式 fill 语义，
 * 我们先计算：如果 640 像素源宽要对应当前 OUTPUT_WIDTH，那么源画面应该保留多高：
 *
 *     crop_height = 640 * 480 / OUTPUT_WIDTH
 *
 * 16:9 时约 360；21:9 时约 274。然后从原版 480 高画面的正中央取这段区域。
 *
 * 注意：这里仍然只读取中央原版 640×480，不会把左右真实世界重新带进模糊背景。
 * 这样对白、头像和 Battle 的构图始终以原版 4:3 中央内容为视觉中心。
 */
static void build_cinematic_low_source(void) {
    const u16* visible = (const u16*)(g_present_staging +
        ((ORIGINAL_EXTRA_Y * PRESENT_STAGING_W + ORIGINAL_EXTRA_X) * BYTES_PER_PIXEL));
    u32 crop_height;
    u32 crop_top;
    u32 ly;

    /* 四舍五入得到当前目标宽高比对应的源裁切高度，并保证至少有 1 行。 */
    crop_height = (LOGICAL_WIDTH * OUTPUT_HEIGHT + OUTPUT_WIDTH / 2u) / OUTPUT_WIDTH;
    if (crop_height < 1u) crop_height = 1u;
    if (crop_height > LOGICAL_HEIGHT) crop_height = LOGICAL_HEIGHT;
    crop_top = (LOGICAL_HEIGHT - crop_height) / 2u;

    for (ly = 0u; ly < CINEMATIC_LOW_H; ++ly) {
        u32 sy;
        u32 lx;

        /*
         * 把低分辨率 y=0..119 均匀映射到裁切后的源画面。
         * 使用 (N-1) 可以保证第一行和最后一行都真正落到裁切区域边缘。
         */
        if (CINEMATIC_LOW_H <= 1u || crop_height <= 1u) {
            sy = crop_top;
        } else {
            sy = crop_top + (ly * (crop_height - 1u)) / (CINEMATIC_LOW_H - 1u);
        }
        if (sy >= LOGICAL_HEIGHT) sy = LOGICAL_HEIGHT - 1u;

        for (lx = 0u; lx < CINEMATIC_LOW_W; ++lx) {
            u32 sx;

            /* 同理，把低分辨率横坐标完整铺满原版中央 640 像素。 */
            if (CINEMATIC_LOW_W <= 1u) {
                sx = 0u;
            } else {
                sx = (lx * (LOGICAL_WIDTH - 1u)) / (CINEMATIC_LOW_W - 1u);
            }
            if (sx >= LOGICAL_WIDTH) sx = LOGICAL_WIDTH - 1u;

            /*
             * 先在源图附近做一次很小的 3×3 平均，相当于“缩图前预滤波”。
             * 这样高对比文字/头像边缘不会在低分辨率工作图里留下特别尖锐的锯齿。
             */
            {
                u32 sum_r = 0u, sum_g = 0u, sum_b = 0u, count = 0u;
                i32 dy, dx;
                for (dy = -1; dy <= 1; ++dy) {
                    i32 yy = (i32)sy + dy;
                    if (yy < 0 || yy >= (i32)LOGICAL_HEIGHT) continue;
                    for (dx = -1; dx <= 1; ++dx) {
                        i32 xx = (i32)sx + dx;
                        u16 pixel;
                        if (xx < 0 || xx >= (i32)LOGICAL_WIDTH) continue;

                        /*
                         * visible 每一行的真实步长是当前 Present staging 宽度，
                         * 中央 640 的起点则永远是当前 SIDE_WIDTH。
                         */
                        pixel = visible[(u32)yy * PRESENT_STAGING_W + SIDE_WIDTH + (u32)xx];
                        sum_r += (pixel >> 11) & 0x1Fu;
                        sum_g += (pixel >> 5) & 0x3Fu;
                        sum_b += pixel & 0x1Fu;
                        ++count;
                    }
                }

                if (count == 0u) {
                    g_blur_low_a[ly * CINEMATIC_LOW_W + lx] = 0u;
                } else {
                    g_blur_low_a[ly * CINEMATIC_LOW_W + lx] =
                        (u16)(((sum_r / count) << 11) |
                              ((sum_g / count) << 5) |
                              (sum_b / count));
                }
            }
        }
    }
}

/*
 * 在低分辨率图上做一次 7×7 box blur。
 * v0.6 把 radius 从 2 提到 3，所以一个输出像素最多平均 49 个输入像素。
 * 两次连续执行以后侧区细节会比 v0.5 更难辨认；工作图只有最终宽度约1/4 × 120，成本仍远低于直接在最终画面上做大核卷积。
 */
static void blur_low_pass(const u16* src, u16* dst) {
    u32 y;

    for (y = 0u; y < CINEMATIC_LOW_H; ++y) {
        u32 x;
        for (x = 0u; x < CINEMATIC_LOW_W; ++x) {
            u32 sum_r = 0u, sum_g = 0u, sum_b = 0u, count = 0u;
            i32 dy;
            for (dy = -(i32)CINEMATIC_BLUR_RADIUS; dy <= (i32)CINEMATIC_BLUR_RADIUS; ++dy) {
                i32 yy = (i32)y + dy;
                i32 dx;
                if (yy < 0 || yy >= (i32)CINEMATIC_LOW_H) continue;

                for (dx = -(i32)CINEMATIC_BLUR_RADIUS; dx <= (i32)CINEMATIC_BLUR_RADIUS; ++dx) {
                    i32 xx = (i32)x + dx;
                    u16 pixel;
                    if (xx < 0 || xx >= (i32)CINEMATIC_LOW_W) continue;

                    pixel = src[(u32)yy * CINEMATIC_LOW_W + (u32)xx];
                    sum_r += (pixel >> 11) & 0x1Fu;
                    sum_g += (pixel >> 5) & 0x3Fu;
                    sum_b += pixel & 0x1Fu;
                    ++count;
                }
            }

            dst[y * CINEMATIC_LOW_W + x] =
                (u16)(((sum_r / count) << 11) |
                      ((sum_g / count) << 5) |
                      (sum_b / count));
        }
    }
}

/*
 * 按当前最终输出坐标，从动态宽度×120 的模糊图中做双线性采样。
 * nearest-neighbor 会让低分辨率背景出现明显 4×4 方块；双线性只多几次整数混合，观感会柔和很多。
 */
static u16 sample_blur_background(u32 out_x, u32 out_y) {
    u32 fx = (out_x * (CINEMATIC_LOW_W - 1u) * 256u) / (OUTPUT_WIDTH - 1u);
    u32 fy = (out_y * (CINEMATIC_LOW_H - 1u) * 256u) / (OUTPUT_HEIGHT - 1u);
    u32 x0 = fx >> 8;
    u32 y0 = fy >> 8;
    u32 tx = fx & 0xFFu;
    u32 ty = fy & 0xFFu;
    u32 x1 = (x0 + 1u < CINEMATIC_LOW_W) ? x0 + 1u : x0;
    u32 y1 = (y0 + 1u < CINEMATIC_LOW_H) ? y0 + 1u : y0;
    u16 p00 = g_blur_low_a[y0 * CINEMATIC_LOW_W + x0];
    u16 p10 = g_blur_low_a[y0 * CINEMATIC_LOW_W + x1];
    u16 p01 = g_blur_low_a[y1 * CINEMATIC_LOW_W + x0];
    u16 p11 = g_blur_low_a[y1 * CINEMATIC_LOW_W + x1];
    u16 top = blend_rgb565(p00, p10, tx);
    u16 bottom = blend_rgb565(p01, p11, tx);
    return blend_rgb565(top, bottom, ty);
}

/*
 * 把 0..256 的线性动画进度变成 0..256 的 smoothstep 缓动进度。
 *
 * 线性移动会像“机械匀速门”一样生硬。smoothstep 的特点是：
 * - 刚开始移动时速度慢一点；
 * - 中段较快；
 * - 接近最终位置时再减速。
 *
 * 公式是 3t² - 2t³。这里完全用整数计算，不依赖浮点数或 CRT：
 * amount=0 -> 0；amount=256 -> 256，中间值保持单调递增。
 */
static u32 cinematic_smoothstep(u32 amount) {
    u32 t;
    u32 t2;
    if (amount >= 256u) return 256u;
    t = amount;
    t2 = t * t;
    return (t2 * (768u - 2u * t)) >> 16;
}

/*
 * 计算左侧推入面板某个像素的边缘透明度。
 *
 * panel_width 是当前已经进入屏幕的宽度：16:9 为0..107，21:9 为0..240。
 * 左面板从 x=0 向中央生长；最靠近“正在移动的内侧边缘”保留 10 像素柔化带，
 * 避免出现一条像剪刀裁出来的硬直线。面板完全进入时不再保留柔边，当前整块侧区（107或240）都是完整模糊。
 */
static u32 cinematic_left_edge_alpha(u32 x, u32 panel_width, u32 full_amount) {
    u32 depth;
    if (panel_width == 0u || x >= panel_width) return 0u;
    if (full_amount >= CINEMATIC_FILL_MAX) return 256u;

    depth = panel_width - x;
    if (depth >= CINEMATIC_EDGE_FEATHER) return 256u;
    return (depth * 256u) / CINEMATIC_EDGE_FEATHER;
}

/*
 * 右侧与左侧完全镜像：面板从屏幕最右边向中央移动。
 * x 是当前右侧区域内部的坐标；start_x 是本帧右面板的左边界。
 */
static u32 cinematic_right_edge_alpha(u32 x, u32 start_x, u32 full_amount) {
    u32 depth;
    if (x < start_x) return 0u;
    if (full_amount >= CINEMATIC_FILL_MAX) return 256u;

    depth = x - start_x + 1u;
    if (depth >= CINEMATIC_EDGE_FEATHER) return 256u;
    return (depth * 256u) / CINEMATIC_EDGE_FEATHER;
}

/*
 * v0.11 的统一“左右推入 / 左右退出”侧面板实现。
 *
 * v0.8 已经实机确认空间运动本身的观感很好；v0.11 不改动画，只增加像素来源选择：
 * - BlurredSides=1：面板像素来自现有强模糊电影式背景；
 * - BlurredSides=0：面板像素直接使用 RGB565 黑色 0x0000。
 *
 * 两种模式完全共用同一套：
 * - 0..256 动画进度；
 * - smoothstep 缓动；
 * - 0..SIDE_WIDTH 的推入宽度；
 * - 10 像素移动边缘柔化；
 * - EnterDurationMs / ExitDurationMs。
 *
 * 中央 640×480 绝不缩放、绝不移动。所谓“挤入”只是左右侧面板向内覆盖，
 * 所以原版立绘、消息框、文字、Battle HUD 的几何位置不会因为切换侧区样式而变化。
 *
 * 纯黑模式还会跳过低分辨率缩图和两遍 7×7 模糊计算，因为最后根本不会读取模糊图。
 */
static void apply_cinematic_slide_to_staging(u32 amount) {
    u32 eased;
    u32 panel_width;
    u32 right_start;
    u32 left_source_offset;
    u32 row;

    if (amount == 0u) return;
    if (amount > CINEMATIC_FILL_MAX) amount = CINEMATIC_FILL_MAX;

    /*
     * 只有“模糊侧区”才需要真正生成模糊背景。
     * 纯黑模式最终像素永远是 0x0000，所以这里完全跳过缩图和两遍 7×7 box blur。
     * 这样黑边模式不仅视觉更传统，计算量也比模糊模式更低。
     */
    if (g_cinematic_blurred_sides) {
        build_cinematic_low_source();
        blur_low_pass(g_blur_low_a, g_blur_low_b);
        blur_low_pass(g_blur_low_b, g_blur_low_a);
    }

    /*
     * 先缓动，再把 0..256 映射成 0..SIDE_WIDTH（16:9为107、21:9为240）像素。
     * +255 是向上取整：动画只要已经开始，就至少让最外侧 1 像素进入，避免第一步完全看不见。
     */
    eased = cinematic_smoothstep(amount);
    panel_width = (SIDE_WIDTH * eased + 255u) >> 8;
    if (panel_width > SIDE_WIDTH) panel_width = SIDE_WIDTH;
    if (panel_width == 0u) return;

    right_start = SIDE_WIDTH - panel_width;
    left_source_offset = SIDE_WIDTH - panel_width;

    for (row = 0u; row < OUTPUT_HEIGHT; ++row) {
        u16* visible = (u16*)(g_present_staging +
            (((ORIGINAL_EXTRA_Y + row) * PRESENT_STAGING_W + ORIGINAL_EXTRA_X) * BYTES_PER_PIXEL));
        u32 x;

        /*
         * 左侧：只处理已经滑进来的 0..panel_width-1。
         * source_x 从“完整左模糊面板”的右侧开始取样，所以部分进入时看到的是面板真正露进屏幕的那一段。
         */
        for (x = 0u; x < panel_width; ++x) {
            u32 source_x = left_source_offset + x;
            u32 alpha = cinematic_left_edge_alpha(x, panel_width, amount);
            u16 panel_pixel;

            /*
             * 这里就是 v0.11 真正的样式分支。
             * - 模糊模式：沿用已经验收的采样 + 压暗结果；
             * - 黑边模式：RGB565 的 0 就是纯黑。
             *
             * 后面的 blend_rgb565 完全共用，所以移动中的 10px 柔化边缘也保持一致。
             */
            if (g_cinematic_blurred_sides) {
                panel_pixel = darken_rgb565(
                    sample_blur_background(source_x, row), CINEMATIC_BACKGROUND_KEEP);
            } else {
                panel_pixel = 0u;
            }
            visible[x] = blend_rgb565(visible[x], panel_pixel, alpha);
        }

        /*
         * 右侧：从 right_start 开始才被面板覆盖。
         * local_x 是面板自身从左到右的坐标；把它映射到当前完整模糊背景的右侧 SideWidth 区域。
         */
        for (x = right_start; x < SIDE_WIDTH; ++x) {
            u32 local_x = x - right_start;
            u32 output_x = SIDE_WIDTH + LOGICAL_WIDTH + x;
            u32 source_x = SIDE_WIDTH + LOGICAL_WIDTH + local_x;
            u32 alpha = cinematic_right_edge_alpha(x, right_start, amount);
            u16 panel_pixel;

            if (g_cinematic_blurred_sides) {
                panel_pixel = darken_rgb565(
                    sample_blur_background(source_x, row), CINEMATIC_BACKGROUND_KEEP);
            } else {
                panel_pixel = 0u;
            }
            visible[output_x] = blend_rgb565(visible[output_x], panel_pixel, alpha);
        }
    }
}

/*
 * 对“已经放进当前 staging 的左右侧区”施加黑边暗度。
 *
 * 为什么不直接改 g_wide_frame：
 * 切地图 grace 会短暂复用上一张有效侧画。如果把淡黑后的像素写回 g_wide_frame，下一帧再淡一次会累乘，
 * 结果会比设定速度更快变黑，而且淡出时也恢复不了原始亮度。
 * staging 每帧都会重建，是一次性输出副本，所以在这里做视觉后处理最安全。
 */
static void apply_bar_darkness_to_staging(u32 darkness) {
    u32 row;
    u32 keep;

    if (darkness == 0u) return;
    if (darkness > BAR_DARKNESS_MAX) darkness = BAR_DARKNESS_MAX;
    keep = BAR_DARKNESS_MAX - darkness;

    for (row = 0u; row < OUTPUT_HEIGHT; ++row) {
        u16* visible = (u16*)(g_present_staging +
            (((ORIGINAL_EXTRA_Y + row) * PRESENT_STAGING_W + ORIGINAL_EXTRA_X) * BYTES_PER_PIXEL));
        u32 x;

        if (darkness == BAR_DARKNESS_MAX) {
            Runtime_MemZero(visible, SIDE_WIDTH * BYTES_PER_PIXEL);
            Runtime_MemZero(visible + SIDE_WIDTH + LOGICAL_WIDTH, SIDE_WIDTH * BYTES_PER_PIXEL);
            continue;
        }

        for (x = 0u; x < SIDE_WIDTH; ++x) {
            visible[x] = darken_rgb565(visible[x], keep);
            visible[SIDE_WIDTH + LOGICAL_WIDTH + x] =
                darken_rgb565(visible[SIDE_WIDTH + LOGICAL_WIDTH + x], keep);
        }
    }
}

/*
 * 把 g_wide_frame 的当前 854×480 / 1120×480 画面放进对应 staging 的 (64,48)。
 *
 * 为什么不直接把连续的宽屏帧指针交给 0x405A10？
 * 因为 0x406330 会按 modeWidth + 2*extraX 计算“来源完整行宽”。
 * 0x405A10/0x406330 会按 modeWidth + extraX*2 读取每一行；因此16:9要982行距，21:9要1248行距。
 * 所以必须提供与当前模式匹配的 staging 行距，否则第二行开始就会读错地址。
 */
static void build_present_staging(void) {
    u32 row;

    /* 整张 staging 先清 0。虽然最终只裁中央区域，但这样任何额外边界读取也只会得到黑色。 */
    Runtime_MemZero(g_present_staging, STAGING_BYTES);

    for (row = 0; row < OUTPUT_HEIGHT; ++row) {
        u8* src = g_wide_frame + row * OUTPUT_WIDTH * BYTES_PER_PIXEL;
        u8* dst = g_present_staging + (((ORIGINAL_EXTRA_Y + row) * PRESENT_STAGING_W + ORIGINAL_EXTRA_X) * BYTES_PER_PIXEL);
        Runtime_MemCopy(dst, src, OUTPUT_WIDTH * BYTES_PER_PIXEL);
    }
}

/*
 * 每帧最后的 Present Hook。
 *
 * v0.6 在这里完成三件“只属于最终显示”的工作：
 * 1. 普通探索切图/Camera 瞬态没有新侧画时，短暂保留上一张有效侧画，而不是立刻黑一下；
 * 2. 消息锁存或 Battle 锁存时，让当前左右侧区（107或240）的侧面板从屏幕外向内推入；像素由 BlurredSides 选择模糊或纯黑；
 * 3. 标题/Bink/主 Interface 与无可靠世界侧画的安全回退继续使用平滑黑边。
 *
 * 这些后处理都不改变原版 backing；renderer 依旧只知道自己的 768×576。
 */
static void FASTCALL Hook_DisplayPresent(void* self, void* unused_edx) {
    void* mode;
    void* old_pixels;
    u32 old_width;
    u32 old_height;
    u32 old_extra_x;
    u32 old_extra_y;
    u32 target_darkness;
    u32 target_cinematic_fill;
    int hard_4x3;

    (void)unused_edx;
    if (!self) return;

    mode = display_mode(self);
    if (!mode || !original_display_geometry_ok(self)) {
        g_side_frame_ready = 0;
        g_original_present(self);
        return;
    }

    /*
     * Hook_RenderQueue 后还有 0x4020F0 和一些原版 overlay，所以 Present 前再抄一次中央 640。
     * 这保证对白框、Battle HUD 等“只画一次”的 UI 最终一定出现在中央原版区域。
     */
    copy_core_to_wide(self, SIDE_WIDTH);

    /*
     * 第二次对白来源判定放在 Present，而不是只放在 RenderQueue 之前。
     * 原因是某些 SHOW_MESSAGE/提示路径会在本帧中央绘制过程中才把 0x46F670/678/679 切到可见状态；
     * 只在 RenderQueue 入口判断就会晚一帧，于是用户会先看到一帧不该出现的侧区内容。
     * Present 是最终输出前最后机会，这里重新判定后可以让“同一张刚出现的任意对话框/提示”立刻进入电影式侧区。
     */
    update_cinematic_latch(NULL);

    if (g_side_frame_ready) {
        /* 当前帧有新鲜 world 侧画：立即清掉 miss 计数，后续可以正常宽屏/淡黑。 */
        g_have_side_history = 1;
        g_side_miss_frames = 0u;
        g_logged_side_history = 0;
    } else if (g_have_side_history && g_side_miss_frames < SIDE_HISTORY_GRACE_FRAMES) {
        /*
         * Camera/bounds/queue 在切图时常有 1~几帧的中间态。
         * g_wide_frame 的左右没有被本帧覆盖，所以这里刻意保留上一张有效左右，中央则已经换成当前帧。
         */
        ++g_side_miss_frames;
        if (!g_logged_side_history) {
            g_logged_side_history = 1;
            Runtime_Log("[切图过渡] 当前帧没有新侧画；短暂保留上一张左右世界，避免瞬时黑边闪烁。");
        }
    } else {
        /*
         * 连续太久没有新侧画就不能无限展示旧地图。
         * 不直接 MemZero，而是把黑边目标强制设成 256，让旧侧画按同一套渐变逻辑平滑收黑。
         */
        if (g_side_miss_frames < 0xFFFFFFFFu) ++g_side_miss_frames;
    }

    /*
     * v0.4 起把“硬4:3黑边”和“消息/Battle侧面板”拆成两套独立目标；v0.11 只切换后者的像素样式。
     * hard_4x3 只算一次，避免标题/Bink/Interface 判定在同一帧重复做日志和全局读取。
     */
    hard_4x3 = frame_requires_hard_4x3();
    target_darkness = bar_target_darkness(hard_4x3);
    target_cinematic_fill = cinematic_fill_target(hard_4x3);

    /*
     * 没有可靠侧画且 grace 已耗尽时，普通世界仍然必须安全收黑，不能无限展示旧地图。
     * 但“消息锁存 / Battle”是例外：侧区像素完全可以只由当前中央 640 生成或直接使用纯黑，
     * 它不依赖左右 world multipass，所以此时不能再因为 side miss 强制黑边。
     */
    if (!g_side_frame_ready &&
        (!g_have_side_history || g_side_miss_frames > SIDE_HISTORY_GRACE_FRAMES) &&
        target_cinematic_fill == 0u) {
        target_darkness = BAR_DARKNESS_MAX;
    }

    advance_bar_darkness(target_darkness);
    advance_cinematic_fill(target_cinematic_fill);

    if (target_cinematic_fill != 0u) {
        if (!g_logged_cinematic_fill) {
            g_logged_cinematic_fill = 1;
            Runtime_Log("[侧区推入] 消息 UI 或 Battle 生效：左右侧面板开始从屏幕外向内滑入；中央640/UI不移动。");
        }
    } else if (g_logged_cinematic_fill && g_cinematic_fill_amount == 0u) {
        g_logged_cinematic_fill = 0;
        Runtime_Log("[侧区退出] 左右侧面板已经滑出屏幕：恢复当前宽屏世界或当前硬4:3策略。");
    }

    /*
     * 先构建未处理 staging；消息/Battle侧面板按当前滑动进度覆盖左右，再让硬4:3黑边作为最高优先级后处理压在最上层。
     * 因此如果游戏突然进入标题/Bink/Interface，即使上一帧还残留一点 blur fade，最终也仍是可靠黑边。
     */
    build_present_staging();
    if (!hard_4x3 && g_cinematic_fill_amount != 0u) {
        apply_cinematic_slide_to_staging(g_cinematic_fill_amount);
    }
    apply_bar_darkness_to_staging(g_bar_darkness);

    old_pixels = *(void**)((u8*)self + DISPLAY_BACKING_PIXELS);
    old_width = read_u32(mode, MODE_WIDTH);
    old_height = read_u32(mode, MODE_HEIGHT);
    old_extra_x = read_u32(self, DISPLAY_EXTRA_X);
    old_extra_y = read_u32(self, DISPLAY_EXTRA_Y);

    /* 只在原版 Present 读取像素的这一小段时间，把它指向 982 staging 并告诉它可见宽度是 854。 */
    *(void**)((u8*)self + DISPLAY_BACKING_PIXELS) = g_present_staging;
    write_u32(mode, MODE_WIDTH, OUTPUT_WIDTH);
    write_u32(mode, MODE_HEIGHT, OUTPUT_HEIGHT);
    write_u32(self, DISPLAY_EXTRA_X, ORIGINAL_EXTRA_X);
    write_u32(self, DISPLAY_EXTRA_Y, ORIGINAL_EXTRA_Y);

    g_original_present(self);

    /* 所有临时字段完整恢复；游戏下一帧继续只看到原版 640/768 协议。 */
    write_u32(self, DISPLAY_EXTRA_Y, old_extra_y);
    write_u32(self, DISPLAY_EXTRA_X, old_extra_x);
    write_u32(mode, MODE_HEIGHT, old_height);
    write_u32(mode, MODE_WIDTH, old_width);
    *(void**)((u8*)self + DISPLAY_BACKING_PIXELS) = old_pixels;

    g_side_frame_ready = 0;

    if (!g_logged_first_present) {
        g_logged_first_present = 1;
        Runtime_Log("[显示] v0.11 首次动态宽屏 staging Present 完成；所有消息统一侧面板推入/退出，比例、样式与过渡时间均来自 Castle_Widescreen.ini。");
    }
}

/*
 * Bink 仍然是独立路径，因为电影直接把 640 画进锁定的物理 surface，不经过普通 software backing。
 *
 * v0.2 每一帧都直接把左右 MemZero，所以从世界切入电影时黑边会“啪”地出现。
 * Bink 始终用 dest_x + SIDE_WIDTH 把原版640内容放在当前输出中央；左右仍按 g_bar_darkness 平滑变暗：
 * - 电影刚开始时，如果上一状态还是宽屏，左右旧世界会在约 250ms 内淡黑；
 * - 电影稳定后 darkness=256，效果和原版中央 640 + 纯黑两侧完全一致。
 */
static i32 WINAPI Hook_BinkCopyToBuffer(
    void* bink, void* dest_pixels, i32 dest_pitch, u32 dest_height,
    u32 dest_x, u32 dest_y, u32 flags)
{
    i32 result;
    u32 row;
    u32 rows_to_process;
    u32 pitch;
    u32 keep;
    u8* base = (u8*)dest_pixels;

    if (!g_original_bink_copy) return 0;

    result = g_original_bink_copy(bink, dest_pixels, dest_pitch, dest_height,
                                  dest_x + SIDE_WIDTH, dest_y, flags);

    /* Bink 自己就是硬 4:3，直接把黑边目标推进到全黑，但仍使用同一套渐变步长。 */
    advance_bar_darkness(BAR_DARKNESS_MAX);
    keep = BAR_DARKNESS_MAX - g_bar_darkness;

    if (base && dest_pitch > 0) {
        pitch = (u32)dest_pitch;
        if (pitch >= OUTPUT_WIDTH * BYTES_PER_PIXEL && dest_y < dest_height) {
            rows_to_process = dest_height - dest_y;
            if (rows_to_process > OUTPUT_HEIGHT) rows_to_process = OUTPUT_HEIGHT;

            for (row = 0u; row < rows_to_process; ++row) {
                u16* line = (u16*)(base + (dest_y + row) * pitch);
                u32 x;

                if (g_bar_darkness == BAR_DARKNESS_MAX) {
                    Runtime_MemZero(line, SIDE_WIDTH * BYTES_PER_PIXEL);
                    Runtime_MemZero(line + SIDE_WIDTH + LOGICAL_WIDTH, SIDE_WIDTH * BYTES_PER_PIXEL);
                } else if (g_bar_darkness != 0u) {
                    for (x = 0u; x < SIDE_WIDTH; ++x) {
                        line[x] = darken_rgb565(line[x], keep);
                        line[SIDE_WIDTH + LOGICAL_WIDTH + x] =
                            darken_rgb565(line[SIDE_WIDTH + LOGICAL_WIDTH + x], keep);
                    }
                }
            }
        }
    }

    return result;
}

/*
 * RuntimeSDK Display/Render Provider
 *
 * 下面这些函数不重新计算 CameraPlan。Hook_RenderQueue 已经在真正绘制发生的那一刻
 * 把权威结果复制进 g_sdk_geometry；Provider 只负责把这份同帧快照交给其它插件。
 * 这样 Quest 等消费者不会各自复制宽屏算法，也不会读到游戏随后恢复的原 Camera。
 */
static CastleResult CASTLE_RUNTIME_CALL sdk_display_get_state(CastleDisplayStateV1* state) {
    CastleDisplayGeometryV1 geometry;
    if (!state) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (!sdk_copy_geometry_snapshot(&geometry)) return CASTLE_ERROR_NOT_READY;
    state->flags = 0u;
    state->ready = g_sdk_services_ready;
    state->generation = geometry.generation;
    state->backend_plugin = g_sdk_plugin_handle;
    state->display_mode = geometry.display_mode;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL sdk_display_copy_geometry(CastleDisplayGeometryV1* geometry) {
    if (!geometry || !g_sdk_services_ready) return CASTLE_ERROR_NOT_READY;
    return sdk_copy_geometry_snapshot(geometry) ? CASTLE_OK : CASTLE_ERROR_NOT_READY;
}

static CastleResult CASTLE_RUNTIME_CALL sdk_world_to_screen(
    const CastleWorldToScreenRequestV1* request, CastleScreenProjectionV1* output) {
    CastleDisplayGeometryV1 geometry;
    if (!request || !output || !g_sdk_services_ready) return CASTLE_ERROR_NOT_READY;
    if (!sdk_copy_geometry_snapshot(&geometry)) return CASTLE_ERROR_NOT_READY;
    /* 非零代次表示调用方要求“必须还是我刚才读取的那一帧”，变化后不能硬算。 */
    if (request->requested_generation && request->requested_generation != geometry.generation)
        return CASTLE_ERROR_STALE_GENERATION;
    output->flags = 0u;
    output->actual_generation = geometry.generation;
    /* 世界坐标先减掉本帧真正使用的安全 Camera，再加中央 640 在输出中的左上角。 */
    output->screen_x = request->world_x - geometry.effective_camera_x + geometry.center_x;
    output->screen_y = request->world_y - geometry.effective_camera_y + geometry.center_y;
    output->projection_scope = geometry.projection_scope;
    /* 对白、Battle、硬4:3和过渡帧主动返回不可投影，Marker 应隐藏而不是猜。 */
    if (geometry.projection_scope == CASTLE_PROJECTION_NONE)
        output->visibility = CASTLE_VISIBILITY_NOT_PROJECTABLE;
    else if (output->screen_x < 0) output->visibility = CASTLE_VISIBILITY_OFFSCREEN_LEFT;
    else if ((u32)output->screen_x >= geometry.output_width) output->visibility = CASTLE_VISIBILITY_OFFSCREEN_RIGHT;
    else if (output->screen_y < 0) output->visibility = CASTLE_VISIBILITY_OFFSCREEN_TOP;
    else if ((u32)output->screen_y >= geometry.output_height) output->visibility = CASTLE_VISIBILITY_OFFSCREEN_BOTTOM;
    else output->visibility = CASTLE_VISIBILITY_VISIBLE;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL sdk_screen_to_world(
    const CastleScreenToWorldRequestV1* request, CastleWorldProjectionV1* output) {
    CastleDisplayGeometryV1 geometry;
    if (!request || !output || !g_sdk_services_ready) return CASTLE_ERROR_NOT_READY;
    if (!sdk_copy_geometry_snapshot(&geometry)) return CASTLE_ERROR_NOT_READY;
    if (request->requested_generation && request->requested_generation != geometry.generation)
        return CASTLE_ERROR_STALE_GENERATION;
    output->flags = 0u;
    output->actual_generation = geometry.generation;
    /* 这是上面公式的严格逆运算，只在同一代次内成立。 */
    output->world_x = request->screen_x - geometry.center_x + geometry.effective_camera_x;
    output->world_y = request->screen_y - geometry.center_y + geometry.effective_camera_y;
    output->projection_scope = geometry.projection_scope;
    output->visibility = geometry.projection_scope == CASTLE_PROJECTION_NONE ?
        CASTLE_VISIBILITY_NOT_PROJECTABLE : CASTLE_VISIBILITY_VISIBLE;
    return CASTLE_OK;
}

static const CastleDisplayProviderV1 g_sdk_display_provider_api = {
    CASTLE_DISPLAY_PROVIDER_MAGIC, CASTLE_SIZEOF_DISPLAY_PROVIDER_V1,
    CASTLE_DISPLAY_API_VERSION_1, CASTLE_DISPLAY_CAP_SCREEN_TO_WORLD,
    sdk_display_get_state, sdk_display_copy_geometry, sdk_world_to_screen, sdk_screen_to_world
};

static CastleResult CASTLE_RUNTIME_CALL sdk_render_get_state(CastleRenderStateV1* state) {
    CastleDisplayGeometryV1 geometry;
    if (!state) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (!sdk_copy_geometry_snapshot(&geometry)) return CASTLE_ERROR_NOT_READY;
    state->flags = 0u;
    state->ready = g_sdk_services_ready;
    state->generation = geometry.generation;
    state->backend_plugin = g_sdk_plugin_handle;
    state->provider_handle = g_sdk_render_provider;
    /* Provider 不伪造 Runtime 的租约所有者；这两个字段由 Runtime 门面统一补充。 */
    state->display_provider_generation = 0u;
    state->extra_frame_owner = 0u;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL sdk_render_queue(const CastleRenderCallV1* call) {
    if (!call || !call->render_context || !g_sdk_services_ready) return CASTLE_ERROR_NOT_READY;
    /* 复用正式 Hook 的多 Camera 业务；这里不复制第二套宽屏绘制算法。 */
    Hook_RenderQueue((void*)(SIZE_T)call->render_context, NULL);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL sdk_render_present(const CastleRenderCallV1* call) {
    if (!call || !call->render_context || !g_sdk_services_ready) return CASTLE_ERROR_NOT_READY;
    Hook_DisplayPresent((void*)(SIZE_T)call->render_context, NULL);
    return CASTLE_OK;
}

static const CastleRenderProviderV1 g_sdk_render_provider_api = {
    CASTLE_RENDER_PROVIDER_MAGIC, CASTLE_SIZEOF_RENDER_PROVIDER_V1,
    CASTLE_RENDER_API_VERSION_1, 0u,
    sdk_render_get_state, sdk_render_queue, sdk_render_present
};

static const void* sdk_query_interface(const CastleRuntimeApiV1* runtime_api,
    const char* id, CastleU32 id_length, CastleU32 version, CastleU32 minimum_size) {
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 result_value = {0};
    /* 调用方填写 magic/大小/版本，Runtime 才能确认双方看到的是同一 ABI 结构。 */
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id.data = id;
    query.interface_id.length = id_length;
    query.requested_version = version;
    query.minimum_struct_size = minimum_size;
    result_value.magic = CASTLE_INTERFACE_API_MAGIC;
    result_value.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result_value.result_version = CASTLE_QUERY_VERSION_1;
    if (!runtime_api || runtime_api->QueryInterface(&query, &result_value) != CASTLE_OK) return NULL;
    return result_value.api_pointer;
}

int Widescreen_RegisterRuntimeServices(const CastleRuntimeApiV1* runtime_api,
                                       CastlePluginHandle plugin_handle) {
    static const char display_id[] = CASTLE_DISPLAY_INTERFACE_ID;
    static const char render_id[] = CASTLE_RENDER_INTERFACE_ID;
    static const char display_provider_id[] = "org.castlereforge.widescreen.display";
    static const char render_provider_id[] = "org.castlereforge.widescreen.render";
    CastleStringView provider_id;
    CastleResult result_value;
    g_sdk_runtime_api = runtime_api;
    g_sdk_plugin_handle = plugin_handle;
    /* 第一步只取得 Runtime 的稳定门面，绝不缓存具体后端插件私有地址。 */
    g_sdk_display_api = (const CastleDisplayApiV1*)sdk_query_interface(runtime_api,
        display_id, (CastleU32)(sizeof(display_id) - 1u),
        CASTLE_DISPLAY_API_VERSION_1, CASTLE_SIZEOF_DISPLAY_API_V1);
    g_sdk_render_api = (const CastleRenderApiV1*)sdk_query_interface(runtime_api,
        render_id, (CastleU32)(sizeof(render_id) - 1u),
        CASTLE_RENDER_API_VERSION_1, CASTLE_SIZEOF_RENDER_API_V1);
    if (!g_sdk_display_api || !g_sdk_render_api) return 0;
    /* 第二步先登记 Display，并发布一份“过渡/不可投影”初始快照后再标记就绪。 */
    provider_id.data = display_provider_id;
    provider_id.length = (CastleU32)(sizeof(display_provider_id) - 1u);
    result_value = g_sdk_display_api->RegisterDisplayProvider(plugin_handle, provider_id,
        &g_sdk_display_provider_api, &g_sdk_display_provider);
    if (result_value < 0) return 0;
    g_sdk_services_ready = 1u;
    sdk_publish_geometry(CASTLE_DISPLAY_TRANSITION, CASTLE_PROJECTION_NONE,
        *(volatile i32*)GLOBAL_CAMERA_X, *(volatile i32*)GLOBAL_CAMERA_X, 0u, 0u);
    if (g_sdk_display_api->SetDisplayProviderReady(g_sdk_display_provider, 1u) < 0) return 0;
    /* 第三步把 Render Provider 与刚才同插件的 Display Provider 句柄绑定。 */
    provider_id.data = render_provider_id;
    provider_id.length = (CastleU32)(sizeof(render_provider_id) - 1u);
    result_value = g_sdk_render_api->RegisterRenderProvider(plugin_handle, provider_id,
        g_sdk_display_provider, &g_sdk_render_provider_api, &g_sdk_render_provider);
    if (result_value < 0 ||
        g_sdk_render_api->SetRenderProviderReady(g_sdk_render_provider, 1u) < 0) {
        Runtime_Log("[RuntimeSDK] Display 已发布，但 Render Provider 注册失败。");
        return 0;
    }
    Runtime_Log("[RuntimeSDK] Widescreen Display/Render 权威后端已就绪。");
    return 1;
}

/*
 * 安装顺序：
 * 1. 先把六块运行时缓冲区一次性分配好：宽屏帧、Present staging、原 backing 备份、队列快照、两块模糊工作区；任何一个失败都不安装 Hook。
 * 2. Runtime_ExactBuildProtocolOk 已经做过总预检；这里只安装两个 DirectDraw 重建 CALL 和 Bink 指针。
 *    主 RenderQueue/Present CALL 由 Castle_Runtime 唯一拥有，本插件通过 Render Provider 被调用。
 * 3. 本版不碰 backing 分配、不碰 11 个低层 blitter；这正是和 POC1 的关键区别。
 */
int Widescreen_Install(void) {
    int ok = 1;
    int patched_rebuild_init = 0;
    int patched_rebuild_lost = 0;

    /*
     * 配置只在插件安装时读取一次。
     * 这样用户能从日志明确看到本次启动采用的参数，同时每帧完全不访问磁盘。
     *
     * Ultrawide 默认是 0，也就是继续使用已经验收通过的 854×480。
     * 只有用户明确写成 1 时才切换到严格 21:9 的 1120×480。
     * 这里先决定所有几何变量，再分配缓冲区；因为 WIDE_FRAME_BYTES / STAGING_BYTES / CINEMATIC_LOW_BYTES
     * 都会读取这些变量，顺序不能反过来。
     */
    g_ultrawide_enabled = Runtime_ReadPluginIniU32(
        "Display", "Ultrawide", 0u, 0u, 1u) ? 1 : 0;

    if (g_ultrawide_enabled) {
        g_output_width = OUTPUT_WIDTH_21_9;
        g_side_width = SIDE_WIDTH_21_9;
        g_present_staging_w = STAGING_WIDTH_21_9;
    } else {
        g_output_width = OUTPUT_WIDTH_16_9;
        g_side_width = SIDE_WIDTH_16_9;
        g_present_staging_w = STAGING_WIDTH_16_9;
    }

    /*
     * 电影式模糊工作图大约保持最终宽度的 1/4。
     * +3 后除以 4 是整数“向上取整”：854 得 214，1120 得 280。
     */
    g_cinematic_low_w = (g_output_width + 3u) / 4u;

    Runtime_LogU32("[配置] Ultrawide=", (u32)g_ultrawide_enabled);
    Runtime_LogU32("[配置] OutputWidth=", g_output_width);
    Runtime_LogU32("[配置] SideWidth=", g_side_width);

    /*
     * 默认值 1 保持 v0.10 封版时已经验收的电影式模糊观感。
     * 只有用户明确写 BlurredSides=0 时才切换成纯黑；其它非法值由 Runtime 层回退为 1。
     */
    g_cinematic_blurred_sides = Runtime_ReadPluginIniU32(
        "Cinematic", "BlurredSides", 1u, 0u, 1u) ? 1 : 0;
    Runtime_LogU32("[配置] BlurredSides=", (u32)g_cinematic_blurred_sides);
    if (g_cinematic_blurred_sides) {
        Runtime_Log("[配置] 侧区样式=电影式强模糊；对话/Battle左右使用模糊背景推入退出。");
    } else {
        Runtime_Log("[配置] 侧区样式=纯黑；对话/Battle左右使用纯黑面板推入退出。");
    }

    g_transition_enter_ms = Runtime_ReadPluginIniU32(
        "Transition", "EnterDurationMs",
        TRANSITION_DEFAULT_MS, TRANSITION_MIN_MS, TRANSITION_MAX_MS);
    g_transition_exit_ms = Runtime_ReadPluginIniU32(
        "Transition", "ExitDurationMs",
        TRANSITION_DEFAULT_MS, TRANSITION_MIN_MS, TRANSITION_MAX_MS);

    Runtime_LogU32("[配置] EnterDurationMs=", g_transition_enter_ms);
    Runtime_LogU32("[配置] ExitDurationMs=", g_transition_exit_ms);

    g_wide_frame = (u8*)Runtime_Alloc(WIDE_FRAME_BYTES);
    g_present_staging = (u8*)Runtime_Alloc(STAGING_BYTES);
    g_backing_backup = (u8*)Runtime_Alloc(BACKING_BACKUP_BYTES);
    g_queue_snapshot = (u8*)Runtime_Alloc(QUEUE_SNAPSHOT_BYTES);
    g_blur_low_a = (u16*)Runtime_Alloc(CINEMATIC_LOW_BYTES);
    g_blur_low_b = (u16*)Runtime_Alloc(CINEMATIC_LOW_BYTES);

    if (!g_wide_frame || !g_present_staging || !g_backing_backup || !g_queue_snapshot ||
        !g_blur_low_a || !g_blur_low_b) {
        Runtime_Log("[初始化] 宽屏合成缓冲区分配失败；本次不安装任何宽屏 Hook。 ");
        return 0;
    }

    Runtime_MemZero(g_wide_frame, WIDE_FRAME_BYTES);
    Runtime_MemZero(g_present_staging, STAGING_BYTES);
    Runtime_MemZero(g_backing_backup, BACKING_BACKUP_BYTES);
    Runtime_MemZero(g_queue_snapshot, QUEUE_SNAPSHOT_BYTES);
    Runtime_MemZero(g_blur_low_a, CINEMATIC_LOW_BYTES);
    Runtime_MemZero(g_blur_low_b, CINEMATIC_LOW_BYTES);

    /*
     * Runtime_ExactBuildProtocolOk 已经一次性验证过所有原始 CALL；这里每次真正写入时仍再核对原目标。
     * 任何一步失败都跳到 rollback，把前面已经改过的 CALL 恢复，避免“半安装”状态继续运行游戏。
     */
    if (!Runtime_PatchCall(CALL_DISPLAY_REBUILD_INIT, FN_DISPLAY_REBUILD,
                           Hook_DisplayRebuild, "初次宽屏 DirectDraw 重建")) { ok = 0; goto rollback; }
    patched_rebuild_init = 1;

    if (!Runtime_PatchCall(CALL_DISPLAY_REBUILD_LOST, FN_DISPLAY_REBUILD,
                           Hook_DisplayRebuild, "Surface lost 后宽屏重建")) { ok = 0; goto rollback; }
    patched_rebuild_lost = 1;

    /*
     * Bink 是函数指针槽，不是 E8 CALL。RenderQueue/Present 已由 Runtime 桥接，所以这里
     * 不再出现任何“读取当前 CALL 目标后覆盖”的双边兼容代码。
     */
    if (!Runtime_PatchPointer(IAT_BINK_COPYTOBUFFER, Hook_BinkCopyToBuffer,
                              (void**)&g_original_bink_copy,
                              "Bink 640 居中与渐变黑边")) { ok = 0; goto rollback; }

    Runtime_Log("[初始化] v0.11-poc11 电影式模糊 / 纯黑侧区切换版 Hook 全部安装完成。");
    Runtime_Log("[初始化] 当前策略：普通探索按 INI 输出 854×480 或 1120×480；所有消息框与 Battle 保持中央640，左右侧区按 BlurredSides 选择强模糊或纯黑，并使用完全相同的推入/退出动画。");
    return 1;

rollback:
    /*
     * 倒序恢复，和安装顺序相反。Runtime_RestoreCall 还会确认“当前目标仍然是我们的 Hook”才写回，
     * 所以如果别的插件在极短窗口内接管了同一 CALL，也不会被我们粗暴覆盖。
     */
    if (patched_rebuild_lost) {
        Runtime_RestoreCall(CALL_DISPLAY_REBUILD_LOST, (u32)Hook_DisplayRebuild, FN_DISPLAY_REBUILD);
    }
    if (patched_rebuild_init) {
        Runtime_RestoreCall(CALL_DISPLAY_REBUILD_INIT, (u32)Hook_DisplayRebuild, FN_DISPLAY_REBUILD);
    }

    Runtime_Log("[初始化] Hook 安装出现失败；已尝试回滚本轮已安装 CALL。请退出游戏并提交日志。 ");
    return ok;
}
