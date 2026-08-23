#include "movie_skip.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "pad_input.h"

/*
 * movie_skip.c
 *
 * 本版刻意采用最简单、最接近原版输入的方案：
 *
 *     当前原版电影正在播放 + Start 刚按下
 *                         ↓
 *            向 RPG.exe 前台窗口投递一次 ESC
 *                         ↓
 *       RPG.exe 原窗口过程自己决定如何关闭/跳过电影
 *
 * 没有下面这些东西：
 * - 没有“标题第一次出现过”状态；
 * - 没有“这是 LOGO 还是剧情”的资源判断；
 * - 没有 3 秒计时；
 * - 没有 BinkOpen Hook；
 * - 没有直接修改 Bink 对象、handle 或 active flag。
 *
 * 对刚学编程的读者：可以把本模块理解成一个非常小的“按键翻译器”。
 * 它先确认游戏此刻真的在放电影，然后把手柄 Start 翻译成键盘 ESC，让游戏自己的老代码继续处理。
 */

static int g_movie_skip_enabled;

/*
 * 原窗口过程使用 GLOBAL_MOVIE_OBJECT 找当前电影包装对象，并读取对象 +0x0A 的活动标记。
 * 我们完全照着这个已经逆向闭合的“只读条件”判断，不自己猜游戏状态。
 */
static int movie_is_active(void) {
    u8* movie = *(u8**)GLOBAL_MOVIE_OBJECT;

    /* 坏指针/NULL 直接视为“没有活动电影”，绝不继续解引用。 */
    if (!Runtime_PtrOk(movie)) return 0;

    /* 这里只读取一个字节，不会把原版电影状态改成 0 或 1。 */
    return *(u8*)(movie + MOVIE_ACTIVE_FLAG) != 0;
}

/*
 * 把一次 ESC KEYDOWN 放进 RPG.exe 自己的窗口消息队列。
 *
 * 为什么使用 PostMessageA：
 * - 它和原版键盘 ESC 最终会进入同一个窗口过程；
 * - 它不会在我们的 worker 线程里同步执行游戏窗口代码；
 * - 它不需要碰 Bink 内部生命周期，所以不会制造“下一段电影打不开”的残留状态。
 */
static void movie_post_native_escape(void) {
    const RuntimeApi* api = Runtime_Api();
    HWND hwnd = NULL;

    /* 游戏不在前台时不模拟键盘，避免 Alt+Tab 后 Start 操作后台 RPG.exe。 */
    if (!PadInput_GameForeground(&hwnd) || !hwnd) return;
    if (!api->post_message_a) return;

    api->post_message_a(hwnd, WM_KEYDOWN_, VK_ESCAPE_, 0);
    Runtime_Log("[动画] Start 已按原版 ESC 路径提交一次跳过输入。");
}

void MovieSkip_Initialize(void) {
    g_movie_skip_enabled = 0;

    /*
     * 这两个机器码锚点只属于动画能力，不参与 refactor4 核心功能的 fail-closed。
     * 如果不一致，只让 Start=ESC 停用，战斗、Target、标题、SaveSlot 继续使用封版实现。
     */
    if (!Runtime_MovieEscapeProtocolOk()) return;

    g_movie_skip_enabled = 1;
    Runtime_Log("[动画] Start=ESC 已启用：任何原版活动电影中，Start 新按下一次就等价于 ESC 一次。");
}

int MovieSkip_Enabled(void) {
    return g_movie_skip_enabled;
}

void MovieSkip_Update(void) {
    if (!g_movie_skip_enabled) return;

    /* 没有活动电影时完全不读取/消费 Start 的业务动作。 */
    if (!movie_is_active()) return;

    /*
     * 使用 Pressed 而不是 Down：
     * - Pressed 只在“上一 tick 没按、本 tick 刚按”时为 1；
     * - 因而一次按住只会提交一次 ESC，不会每 8 ms 连续刷几十个 ESC；
     * - 松开再按，才会产生下一次 ESC，和普通键盘按键的离散操作更接近。
     */
    if (InputRouter_PressedOn(INPUT_CTX_MOVIE, INPUT_SYSTEM_START, INPUT_LAYER_OVERLAY)) {
        movie_post_native_escape();
    }
}
