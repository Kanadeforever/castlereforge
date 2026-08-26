#include "scene_choice.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"

/*
 * scene_choice.c
 *
 * 原版剧情“是 / 否”不是 0x431380 ButtonEvent 菜单，而是公共消息引擎 mode=3 的鼠标选择器。
 * 静态逆向已经闭合原版 0x404600 的真实协议：
 *
 * 1. 鼠标必须位于内部 640x480 坐标中的 X=500..590、Y=300..375；
 * 2. Y<=340 时原版把 0x46F679 设为 10，代表第一项；
 * 3. Y>340  时原版把 0x46F679 设为 9，代表第二项；
 * 4. 原版检测到左键后：状态10提交结果1，状态9提交结果2；
 * 5. 提交、剧情变量更新、消息关闭全部仍由 RPG.exe 自己完成。
 *
 * 因此插件不写剧情结果，也不直接调用 0x44B0B0。
 * 用户补充截图确认：这个原版“是 / 否”本身就有选择框，所以手柄不应该再显示一只手形当第二套焦点。
 * 我们只把“隐藏的真实鼠标坐标”移动进第一/第二项命中矩形；0x404600 会据此把自己的 visual/hover
 * 状态写成 10/9，原版绘制链随后显示真正的选择框。A/B 仍只发送普通左键脉冲，让 RPG.exe 自己完成提交。
 */

#define SCENE_CHOICE_MODE_YES_NO 3u
#define SCENE_CHOICE_NEUTRAL_STATE 8u
#define SCENE_CHOICE_SECOND_STATE  9u
#define SCENE_CHOICE_FIRST_STATE   10u

/*
 * 选择框真实命中矩形来自 0x404600：X 只要落在 500..590 即可。
 * 这里取中间值 545；第一项取上半区 320，第二项取下半区 360。
 * 不把坐标压到边缘，避免像素取整/窗口边框造成一两个像素的误判。
 */
#define SCENE_CHOICE_CURSOR_X 545
#define SCENE_CHOICE_FIRST_Y  320
#define SCENE_CHOICE_SECOND_Y 360

/*
 * 64ms 约跨过 3~4 个 60Hz 游戏帧。
 * 单个 worker tick 只有约8ms，若只按一个 tick，RPG.exe 可能恰好两帧之间完全看不到左键。
 * 64ms 对玩家仍然只是一次普通短按，但能显著降低“选择框已经切换，A 偶尔没被原版看见”的时序风险。
 */
#define SCENE_CHOICE_CLICK_MS 64u

typedef struct SceneChoiceState {
    int enabled;            /* 静态协议通过后才置1；失败时整个模块 fail-closed。 */
    int was_active;         /* 用来识别一次新的剧情选择，不把上次焦点带进下一次。 */
    int controller_nav;     /* 只有用户真正用方向/A/B后才持续同步隐藏命中坐标，让原版选择框保持在当前项。 */
    int focus;              /* 0=第一项（是），1=第二项（否）。 */
    int left_down;          /* 我们是否已经通过 mouse_event 发出了 LEFTDOWN。 */
    u32 left_release_tick;  /* 到这个 worker tick 必须补 LEFTUP。 */
} SceneChoiceState;

static SceneChoiceState g_scene_choice;

/*
 * 读取 mode=3 是最关键的“不是普通对话”的判据。
 * 还要求消息 ID 非0、两个原版交互状态都处于 8/9/10 范围，避免消息刚建立/销毁的半帧误触发。
 */
int SceneChoice_IsActive(void) {
    u8 visual_state;
    u8 hover_state;

    if (!g_scene_choice.enabled) return 0;
    if (*(volatile u32*)GLOBAL_DIALOGUE_ID == 0u) return 0;
    if (*(volatile u32*)GLOBAL_DIALOGUE_MODE != SCENE_CHOICE_MODE_YES_NO) return 0;

    visual_state = *(volatile u8*)GLOBAL_DIALOGUE_CHOICE_VISUAL_STATE;
    hover_state = *(volatile u8*)GLOBAL_DIALOGUE_CHOICE_HOVER_STATE;

    if (visual_state < SCENE_CHOICE_NEUTRAL_STATE || visual_state > SCENE_CHOICE_FIRST_STATE) return 0;
    if (hover_state < SCENE_CHOICE_NEUTRAL_STATE || hover_state > SCENE_CHOICE_FIRST_STATE) return 0;
    return 1;
}

/*
 * 根据当前逻辑焦点，把“隐藏的真实鼠标”放进 0x404600 的真实命中区域。
 * 注意：这里故意不显示手形。0x404600 在下一个游戏帧会把 0x46F678/0x46F679 写成 10/9，
 * 然后 RPG.exe 自己画出截图中已经存在的“是 / 否”选择框；这才是本状态唯一的焦点视觉。
 */
static void scene_choice_sync_cursor(void) {
    i32 y = g_scene_choice.focus == 0 ? SCENE_CHOICE_FIRST_Y : SCENE_CHOICE_SECOND_Y;
    Cursor_MoveHiddenSelectionAt(SCENE_CHOICE_CURSOR_X, y);
}

/*
 * 如果前一个 A/B 左键脉冲还在按住阶段，必须先等它正常释放。
 * 否则第二次确认可能退化成“同一次长按”，原版不会产生新的点击边沿。
 */
static int scene_choice_click_busy(void) {
    return g_scene_choice.left_down != 0;
}

/* 只负责发 LEFTDOWN；LEFTUP 由每帧维护函数在 64ms 后补上。 */
static void scene_choice_begin_left_click(void) {
    const RuntimeApi* api = Runtime_Api();

    if (!api || !api->mouse_event || scene_choice_click_busy()) return;

    /* 先把隐藏命中坐标放到正确项目，再发送左键。原版随后在自己的游戏线程中完成选择框命中和剧情提交。 */
    scene_choice_sync_cursor();
    api->mouse_event(MOUSEEVENTF_LEFTDOWN_, 0u, 0u, 0u, 0u);
    g_scene_choice.left_down = 1;
    g_scene_choice.left_release_tick = Runtime_Tick() + Runtime_MsToTicks(SCENE_CHOICE_CLICK_MS);
}

/*
 * LEFTDOWN 绝不能因为剧情刚好关闭就遗留到桌面；所以这个释放维护在 active 判定之前执行。
 * 只要达到时限就发送一次 LEFTUP，并立刻清自己的状态。
 */
static void scene_choice_release_left_if_due(void) {
    const RuntimeApi* api = Runtime_Api();

    if (!g_scene_choice.left_down) return;
    if ((i32)(Runtime_Tick() - g_scene_choice.left_release_tick) < 0) return;

    if (api && api->mouse_event) api->mouse_event(MOUSEEVENTF_LEFTUP_, 0u, 0u, 0u, 0u);
    g_scene_choice.left_down = 0;
    g_scene_choice.left_release_tick = 0u;
}

/* 存档点 wrapper 出现后仍必须按时补 LEFTUP，但绝不能让旧剧情选择继续读取新按键。 */
void SceneChoice_MaintainPulse(void) {
    scene_choice_release_left_if_due();
}

/* 模块被强制中断（鼠标接管/DLL退出前场景结束）时使用的立即释放版本。 */
static void scene_choice_release_left_immediately(void) {
    const RuntimeApi* api = Runtime_Api();

    if (!g_scene_choice.left_down) return;
    if (api && api->mouse_event) api->mouse_event(MOUSEEVENTF_LEFTUP_, 0u, 0u, 0u, 0u);
    g_scene_choice.left_down = 0;
    g_scene_choice.left_release_tick = 0u;
}

/*
 * 第一次用手柄接管时，尽量尊重玩家当前真实鼠标已经悬停的项目：
 * - 原版 hover state=9  -> 第二项；
 * - 原版 hover state=10 -> 第一项；
 * - neutral=8           -> 默认第一项“是”。
 */
static void scene_choice_begin_controller_nav(void) {
    u8 hover_state = *(volatile u8*)GLOBAL_DIALOGUE_CHOICE_HOVER_STATE;

    if (!g_scene_choice.controller_nav) {
        if (hover_state == SCENE_CHOICE_SECOND_STATE) g_scene_choice.focus = 1;
        else g_scene_choice.focus = 0;
        g_scene_choice.controller_nav = 1;
    }
    scene_choice_sync_cursor();
}

static void scene_choice_select(int focus) {
    if (focus < 0) focus = 0;
    if (focus > 1) focus = 1;

    scene_choice_begin_controller_nav();
    g_scene_choice.focus = focus;
    scene_choice_sync_cursor();
    Runtime_Log(focus == 0 ? "[剧情选择] 焦点：是。" : "[剧情选择] 焦点：否。");
}

void SceneChoice_Update(void) {
    int active;

    scene_choice_release_left_if_due();
    active = SceneChoice_IsActive();

    if (!active) {
        if (g_scene_choice.was_active) {
            /* 剧情已经接受结果或被其它流程关闭：停止强制命中坐标，并先保证 LEFTUP 不遗留。 */
            scene_choice_release_left_immediately();
        }
        g_scene_choice.was_active = 0;
        g_scene_choice.controller_nav = 0;
        g_scene_choice.focus = 0;
        return;
    }

    if (!g_scene_choice.was_active) {
        g_scene_choice.was_active = 1;
        g_scene_choice.controller_nav = 0;
        g_scene_choice.focus = 0;
        Runtime_Log("[剧情选择] 检测到公共消息 mode=3 的原版两项选择。手柄方向驱动原版选择框，A确认，B直接选择‘否’。");
    }

    /* 手柄已经接管后，每帧同步隐藏命中坐标，保证原版选择框稳定停在当前逻辑焦点。 */
    if (g_scene_choice.controller_nav) scene_choice_sync_cursor();

    /* 截图中的“是 / 否”是二选一；左右最符合手柄直觉，同时允许上下兼容原版纵向命中矩形。 */
    if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY) ||
        InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        scene_choice_select(0);
        InputRouter_Consume(INPUT_NAV_LEFT);
        InputRouter_Consume(INPUT_NAV_UP);
    }
    if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY) ||
        InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        scene_choice_select(1);
        InputRouter_Consume(INPUT_NAV_RIGHT);
        InputRouter_Consume(INPUT_NAV_DOWN);
    }

    /* A 只点当前焦点；原版 0x404600 自己把第一/第二项变成剧情结果 1/2。 */
    if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        scene_choice_begin_controller_nav();
        scene_choice_begin_left_click();
        Runtime_Log("[剧情选择] A：按当前原版选择框对应命中位置发送一次左键确认。");
        InputRouter_Consume(INPUT_CONFIRM);
    }

    /* B 按项目统一惯例直接选择“否”，仍然只是把隐藏鼠标命中坐标移动到第二项并左键，不伪造剧情返回值。 */
    if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        scene_choice_select(1);
        scene_choice_begin_left_click();
        Runtime_Log("[剧情选择] B：选择‘否’，并通过原版左键路径提交。");
        InputRouter_Consume(INPUT_CANCEL);
    }
}

void SceneChoice_OnPointerTakeover(CursorTakeoverEvent event_type) {
    (void)event_type;

    /* 实体鼠标/右摇杆接管后，不再强制选择框对应的命中坐标；未完成的人工左键也立即抬起。 */
    scene_choice_release_left_immediately();
    g_scene_choice.controller_nav = 0;
}

int SceneChoice_Install(void) {
    if (!Runtime_SceneChoiceProtocolOk()) {
        g_scene_choice.enabled = 0;
        Runtime_Log("[剧情选择] mode=3 两项选择协议未通过；该能力已单独禁用。普通对话 A 不受影响。");
        return 1;
    }

    g_scene_choice.enabled = 1;
    g_scene_choice.was_active = 0;
    g_scene_choice.controller_nav = 0;
    g_scene_choice.focus = 0;
    g_scene_choice.left_down = 0;
    g_scene_choice.left_release_tick = 0u;
    Runtime_Log("[剧情选择] 已启用公共消息 mode=3 两项选择：方向驱动原版选择框，A确认，B选择否；隐藏鼠标只负责原版命中坐标。");
    return 1;
}
