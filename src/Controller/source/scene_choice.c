#include "scene_choice.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"

/*
 * scene_choice.c
 *
 * 公共消息引擎实际有两种“剧情选项”，它们都不是 0x431380 ButtonEvent 菜单：
 *
 * 1. mode=2：MSG 文本里的 /q，多行“我的回答是”选项，原版处理函数是 0x4044F0；
 * 2. mode=3：MSG 文本里的 /z，固定“是 / 否”两项，原版处理函数是 0x404600。
 *
 * 用户截图对应 Mp0610a.msg 的 /q：周崇、朱浩战后，夏侯仪在客房回答是否继续休息。
 * 原版 mode=2 已经会按鼠标所在行绘制橙色高亮和手形焦点，所以插件只需要移动隐藏命中点，
 * 绝不能再额外显示第二只插件鼠标。上下键改变行，确定键仍通过原版左键提交。
 *
 * 静态逆向已经闭合 mode=3 的 0x404600 协议：
 *
 * 1. 鼠标必须位于内部 640x480 坐标中的 X=500..590、Y=300..375；
 * 2. Y<=340 时原版把 0x46F679 设为 10，代表第一项；
 * 3. Y>340  时原版把 0x46F679 设为 9，代表第二项；
 * 4. 原版检测到左键后：状态10提交结果1，状态9提交结果2；
 * 5. 提交、剧情变量更新、消息关闭全部仍由 RPG.exe 自己完成。
 *
 * mode=2 的 0x4044F0 同样只在真实命中行上绘制原版焦点；检测到左键后把“行号+1”写入 VAR999。
 * 因此插件对两种模式都不写剧情结果，也不直接调用 0x44B0B0。我们只移动隐藏真实鼠标并发送
 * 普通左键脉冲，让 RPG.exe 自己完成选项结果、剧情分支和消息关闭。
 */

#define SCENE_CHOICE_MODE_MULTI  2u
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
 * mode=2 每行高 22 像素。0x4044F0 对第 N 行使用严格命中区：
 *   359 + 22*N < 鼠标Y < 379 + 22*N
 * 中点正好是 370 + 22*N，离上下边界各 10 像素，最不容易受取整影响。
 */
#define SCENE_MULTI_ROW_HEIGHT 22
#define SCENE_MULTI_ROW_CENTER_BASE 370
#define SCENE_MULTI_MAX_LINE 4

/*
 * 64ms 约跨过 3~4 个 60Hz 游戏帧。
 * 单个 worker tick 只有约8ms，若只按一个 tick，RPG.exe 可能恰好两帧之间完全看不到左键。
 * 64ms 对玩家仍然只是一次普通短按，但能显著降低“选择框已经切换，A 偶尔没被原版看见”的时序风险。
 */
#define SCENE_CHOICE_CLICK_MS 64u

typedef struct SceneChoiceState {
    int enabled;            /* 静态协议通过后才置1；失败时整个模块 fail-closed。 */
    int was_active;         /* 用来识别一次新的剧情选择，不把上次焦点带进下一次。 */
    u32 active_mode;        /* 记录本次会话是mode=2还是mode=3；模式变化时必须重建焦点。 */
    int controller_nav;     /* 只有用户真正用方向/A/B后才持续同步隐藏命中坐标，让原版选择框保持在当前项。 */
    int focus;              /* 从0开始的项目编号；mode=3固定0/1，mode=2按原版first/last行计算。 */
    int left_down;          /* 我们是否已经通过 mouse_event 发出了 LEFTDOWN。 */
    u32 left_release_tick;  /* 到这个 worker tick 必须补 LEFTUP。 */
} SceneChoiceState;

static SceneChoiceState g_scene_choice;

/*
 * 读取并验证 mode=2 的行范围。
 *
 * 为什么限制到0..4：原版窗口高度只有480，最后一行的下边界是379+22*line。
 * line=4 时仍为467，line=5 就会超过窗口。这个限制不是猜“最多五个答案”，而是按
 * 0x4044F0 的真实命中公式做内存安全和坐标安全校验。
 */
static int scene_multi_line_range(i32* out_first, i32* out_last) {
    i32 first = *(volatile i32*)GLOBAL_DIALOGUE_MULTI_FIRST_LINE;
    i32 last = *(volatile i32*)GLOBAL_DIALOGUE_MULTI_LAST_LINE;

    if (first < 0 || last < first || last > SCENE_MULTI_MAX_LINE) return 0;
    if (out_first) *out_first = first;
    if (out_last) *out_last = last;
    return 1;
}

/* 返回当前选择模式；0表示现在不是已经稳定可操作的剧情选项。 */
static u32 scene_choice_active_mode(void) {
    u32 mode;
    u8 visual_state;
    u8 hover_state;

    if (!g_scene_choice.enabled) return 0u;
    if (*(volatile u32*)GLOBAL_DIALOGUE_ID == 0u) return 0u;

    mode = *(volatile u32*)GLOBAL_DIALOGUE_MODE;
    if (mode == SCENE_CHOICE_MODE_MULTI) {
        /* mode=2 不使用8/9/10状态；只要原版行范围有效，就已经进入可操作选择器。 */
        return scene_multi_line_range(NULL, NULL) ? mode : 0u;
    }
    if (mode != SCENE_CHOICE_MODE_YES_NO) return 0u;

    visual_state = *(volatile u8*)GLOBAL_DIALOGUE_CHOICE_VISUAL_STATE;
    hover_state = *(volatile u8*)GLOBAL_DIALOGUE_CHOICE_HOVER_STATE;

    if (visual_state < SCENE_CHOICE_NEUTRAL_STATE || visual_state > SCENE_CHOICE_FIRST_STATE) return 0u;
    if (hover_state < SCENE_CHOICE_NEUTRAL_STATE || hover_state > SCENE_CHOICE_FIRST_STATE) return 0u;
    return mode;
}

int SceneChoice_IsActive(void) {
    return scene_choice_active_mode() != 0u;
}

/*
 * 根据当前逻辑焦点，把“隐藏的真实鼠标”放进 0x404600 的真实命中区域。
 * 注意：这里故意不显示手形。0x404600 在下一个游戏帧会把 0x46F678/0x46F679 写成 10/9，
 * 然后 RPG.exe 自己画出截图中已经存在的“是 / 否”选择框；这才是本状态唯一的焦点视觉。
 */
static void scene_choice_sync_cursor(void) {
    i32 y;

    if (g_scene_choice.active_mode == SCENE_CHOICE_MODE_MULTI) {
        i32 first;
        i32 last;
        i32 line;

        if (!scene_multi_line_range(&first, &last)) return;

        /* 动态台词若重建了行范围，先把旧焦点夹回新范围，禁止算出窗口外坐标。 */
        if (g_scene_choice.focus < 0) g_scene_choice.focus = 0;
        if (g_scene_choice.focus > last - first) g_scene_choice.focus = last - first;

        line = first + g_scene_choice.focus;
        y = SCENE_MULTI_ROW_CENTER_BASE + SCENE_MULTI_ROW_HEIGHT * line;
    } else {
        /* mode=3 继续使用已经实机验证过的固定“是/否”命中中点。 */
        y = g_scene_choice.focus == 0 ? SCENE_CHOICE_FIRST_Y : SCENE_CHOICE_SECOND_Y;
    }

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

/* 第一次接管 mode=2 时，若真实鼠标已经落在某一行，就从那一行继续；否则从第一项开始。 */
static int scene_multi_focus_from_pointer(void) {
    i32 first;
    i32 last;
    i32 x;
    i32 y;
    i32 line;

    if (!scene_multi_line_range(&first, &last)) return 0;
    if (!Cursor_GetPointerPosition(&x, &y)) return 0;

    /* 0x4044F0 对X使用严格的500<X<590；不在这条窄列中就视为尚无原版焦点。 */
    if (x <= 500 || x >= 590) return 0;

    for (line = first; line <= last; ++line) {
        i32 top = 359 + SCENE_MULTI_ROW_HEIGHT * line;
        i32 bottom = 379 + SCENE_MULTI_ROW_HEIGHT * line;

        if (y > top && y < bottom) return line - first;
    }
    return 0;
}

/*
 * 第一次用手柄接管时，尽量尊重玩家当前真实鼠标已经悬停的项目：
 * - mode=2：按原版动态行命中矩形还原当前行；
 * - mode=3：hover state=9是第二项，10是第一项，neutral=8默认第一项。
 */
static void scene_choice_begin_controller_nav(void) {
    if (!g_scene_choice.controller_nav) {
        if (g_scene_choice.active_mode == SCENE_CHOICE_MODE_MULTI) {
            g_scene_choice.focus = scene_multi_focus_from_pointer();
        } else {
            u8 hover_state = *(volatile u8*)GLOBAL_DIALOGUE_CHOICE_HOVER_STATE;
            g_scene_choice.focus = hover_state == SCENE_CHOICE_SECOND_STATE ? 1 : 0;
        }
        g_scene_choice.controller_nav = 1;
    }
    scene_choice_sync_cursor();
}

static void scene_choice_select(int focus) {
    int last_focus = 1;

    if (g_scene_choice.active_mode == SCENE_CHOICE_MODE_MULTI) {
        i32 first;
        i32 last;
        if (!scene_multi_line_range(&first, &last)) return;
        last_focus = last - first;
    }

    if (focus < 0) focus = 0;
    if (focus > last_focus) focus = last_focus;

    scene_choice_begin_controller_nav();
    g_scene_choice.focus = focus;
    scene_choice_sync_cursor();

    if (g_scene_choice.active_mode == SCENE_CHOICE_MODE_MULTI) {
        Runtime_Log("[剧情多选] 原版焦点已移动到新的选项行。");
    } else {
        Runtime_Log(focus == 0 ? "[剧情选择] 焦点：是。" : "[剧情选择] 焦点：否。");
    }
}

/* mode=2 用相对方向移动；先接管并同步当前真实鼠标焦点，再在该焦点上加减一行。 */
static void scene_choice_move_multi(int delta) {
    scene_choice_begin_controller_nav();
    scene_choice_select(g_scene_choice.focus + delta);
}

void SceneChoice_Update(void) {
    u32 active_mode;

    scene_choice_release_left_if_due();
    active_mode = scene_choice_active_mode();

    if (active_mode == 0u) {
        if (g_scene_choice.was_active) {
            /* 剧情已经接受结果或被其它流程关闭：停止强制命中坐标，并先保证 LEFTUP 不遗留。 */
            scene_choice_release_left_immediately();
        }
        g_scene_choice.was_active = 0;
        g_scene_choice.active_mode = 0u;
        g_scene_choice.controller_nav = 0;
        g_scene_choice.focus = 0;
        return;
    }

    if (!g_scene_choice.was_active || g_scene_choice.active_mode != active_mode) {
        /* 同一条消息可以从普通文字切进mode=2/3；模式变化必须视为新会话，不能沿用旧行号。 */
        scene_choice_release_left_immediately();
        g_scene_choice.was_active = 1;
        g_scene_choice.active_mode = active_mode;
        g_scene_choice.controller_nav = 0;
        g_scene_choice.focus = 0;
        Runtime_Log(active_mode == SCENE_CHOICE_MODE_MULTI
            ? "[剧情多选] 检测到公共消息mode=2：上下移动原版焦点，确定键提交，取消键不选择。"
            : "[剧情选择] 检测到公共消息mode=3：方向驱动原版选择框，确定键确认，取消键直接选择‘否’。");
    }

    /* 手柄已经接管后，每帧同步隐藏命中坐标，保证原版选择框稳定停在当前逻辑焦点。 */
    if (g_scene_choice.controller_nav) scene_choice_sync_cursor();

    if (active_mode == SCENE_CHOICE_MODE_MULTI) {
        /* 截图的选项是纵向列表；只接受上下，左右被策略层吞掉但不改变焦点。 */
        if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
            scene_choice_move_multi(-1);
            InputRouter_Consume(INPUT_NAV_UP);
        }
        if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
            scene_choice_move_multi(1);
            InputRouter_Consume(INPUT_NAV_DOWN);
        }
        if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY) ||
            InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
            InputRouter_Consume(INPUT_NAV_LEFT);
            InputRouter_Consume(INPUT_NAV_RIGHT);
        }
    } else {
        /* “是/否”继续允许四方向：左/上到第一项，右/下到第二项。 */
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
    }

    /* 确定键只点当前焦点；0x4044F0/0x404600 自己把命中行变成剧情结果。 */
    if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        scene_choice_begin_controller_nav();
        scene_choice_begin_left_click();
        Runtime_Log("[剧情选择] 确定键：按当前原版焦点对应命中位置发送一次左键确认。");
        InputRouter_Consume(INPUT_CONFIRM);
    }

    /* mode=3 的取消键保持“选择否”；mode=2 没有取消结果，只吞键防止穿透到底层剧情。 */
    if (InputRouter_PressedOn(INPUT_CTX_SCENE_CHOICE, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        if (active_mode == SCENE_CHOICE_MODE_YES_NO) {
            scene_choice_select(1);
            scene_choice_begin_left_click();
            Runtime_Log("[剧情选择] 取消键：选择‘否’，并通过原版左键路径提交。");
        } else {
            Runtime_Log("[剧情多选] 取消键没有原版分支结果；本次只阻止按键穿透，不改变选项。");
        }
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
        Runtime_Log("[剧情选择] mode=2/mode=3 协议未通过；该能力已单独禁用。普通对话确定键不受影响。");
        return 1;
    }

    g_scene_choice.enabled = 1;
    g_scene_choice.was_active = 0;
    g_scene_choice.active_mode = 0u;
    g_scene_choice.controller_nav = 0;
    g_scene_choice.focus = 0;
    g_scene_choice.left_down = 0;
    g_scene_choice.left_release_tick = 0u;
    Runtime_Log("[剧情选择] 已启用公共消息mode=2多行选项与mode=3是/否：只驱动原版焦点和左键提交，不直接写剧情结果。");
    return 1;
}
