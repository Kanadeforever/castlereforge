#include "interface_options.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "ui_bridge.h"
#include "cursor.h"

/*
 * interface_options.c
 *
 * state8“机能”与五内相似的地方只有“原版没有键盘焦点，因此手柄要借原版手形鼠标告诉玩家当前操作哪一行”。
 * 业务本身完全不同，所以这里没有复用五内的按钮脉冲或动画状态机：
 *
 * - 音乐：LT -> 原版“减”Button，RT -> 原版“加”Button；
 * - 音效：LT -> 原版“减”Button，RT -> 原版“加”Button；
 * - 空明流转：A -> 原版真实 Button，随后 RPG.exe 自己打开 Yes/No；
 * - ↑/↓：只改插件私有的三行焦点；
 * - B、LB/RB：不在本页截获，继续由 InterfaceShell 负责返回/切大类。
 *
 * 用户特别要求“音乐和音效没有动画”。因此手形鼠标不会放进按钮矩形中心，而是放到每行最右侧按钮的右边，
 * Y 与按钮垂直中心齐平。这样玩家能看见焦点，又不会因为鼠标热点落在 Button 里面触发原版 hover 动画。
 */

typedef struct InterfaceOptionsState {
    int enabled;                    /* 协议和五处 Hook 全成功后才为 1。 */
    int was_active;                 /* 用来配对进入/离开页面的视觉清理。 */
    int focus_row;                  /* 0=音乐，1=音效，2=空明流转。只属于插件，不写 RPG.exe。 */
    int controller_focus_visible;   /* 实体鼠标/右摇杆接管后清 0，避免每 tick 抢回鼠标。 */
} InterfaceOptionsState;

static InterfaceOptionsState g_options;

/*
 * refactor28 实机微调：refactor27 功能已经 PASS，只需要把手形焦点稍微向左收拢、再向下移一点。
 * X 仍保持在真实 Button 矩形“右侧之外”：2 像素正间距不会让鼠标热点进入按钮，
 * 因此音乐/音效依旧不会因为焦点提示而制造原版 hover 动画。Y 仅下移 5 像素改善视觉对齐。
 */
#define OPTIONS_FOCUS_GAP_X 2
#define OPTIONS_FOCUS_OFFSET_Y 5

static u8* options_interface(void) {
    u8* interface_ui = *(u8**)GLOBAL_INTERFACE_UI;
    return Runtime_PtrOk(interface_ui) ? interface_ui : NULL;
}

static u8* options_page(void) {
    u8* interface_ui = options_interface();
    u8* page;

    if (!Runtime_PtrOk(interface_ui)) return NULL;
    if (*(i32*)(interface_ui + INTERFACE_STATE) != 8) return NULL;
    if (*(i32*)(interface_ui + INTERFACE_CLOSE_STATE) != 0) return NULL;

    page = *(u8**)(interface_ui + INTERFACE_PAGE_8);
    if (!Runtime_PtrOk(page)) return NULL;
    if (*(u8*)(page + 0x579u) == 0) return NULL;
    return page;
}

int InterfaceOptions_Active(void) {
    return options_page() != NULL;
}

/* 与 0x431380 原版门一致。不可用 Button 必须像鼠标点击一样什么都不发生。 */
static int options_button_usable(void* button) {
    u8* b = (u8*)button;
    if (!Runtime_PtrOk(b)) return 0;
    if (*(u8*)(b + 0x45u) == 0) return 0;
    if (*(u8*)(b + 0x04u) != 0) return 0;
    return 1;
}

static void* options_button_at(u8* page, u32 offset) {
    void* button;
    if (!Runtime_PtrOk(page)) return NULL;
    button = *(void**)(page + offset);
    return Runtime_PtrOk(button) ? button : NULL;
}

/* +0x5A8 是原版空明流转 Yes/No；active 时本页面必须完全让给 ConfirmDialog。 */
static int options_popup_active(u8* page) {
    u8* popup;
    if (!Runtime_PtrOk(page)) return 0;
    popup = *(u8**)(page + INTERFACE_OPTIONS_POPUP);
    return Runtime_PtrOk(popup) && *(u8*)(popup + POPUP_ACTIVE) != 0;
}

/*
 * 每一行选一个“最右侧真实按钮”作为对齐锚点：
 * - 音乐 -> music +；
 * - 音效 -> sound +；
 * - 空明流转 -> 它唯一的按钮。
 * 光标 x 放到 right+12，y 与按钮垂直中心一致，因此热点明确位于 Button 外部。
 */
static int options_focus_anchor(u8* page, int row, i32* out_x, i32* out_y) {
    void* button;
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;

    if (!Runtime_PtrOk(page) || !out_x || !out_y) return 0;
    if (row == 0) button = options_button_at(page, INTERFACE_OPTIONS_MUSIC_INC);
    else if (row == 1) button = options_button_at(page, INTERFACE_OPTIONS_SOUND_INC);
    else if (row == 2) button = options_button_at(page, INTERFACE_OPTIONS_KARMA_BUTTON);
    else return 0;

    if (!UiBridge_GetButtonScreenRect(button, &left, &top, &right, &bottom)) return 0;
    *out_x = right + OPTIONS_FOCUS_GAP_X;
    *out_y = top + (bottom - top) / 2 + OPTIONS_FOCUS_OFFSET_Y;
    return 1;
}

static void options_show_focus(u8* page) {
    i32 x;
    i32 y;
    if (!options_focus_anchor(page, g_options.focus_row, &x, &y)) return;

    Cursor_ClaimForControllerNavigation();
    Cursor_ShowMenuFocusAt(x, y);
    g_options.controller_focus_visible = 1;
}

static void options_reset_visual(void) {
    g_options.focus_row = 0;
    g_options.controller_focus_visible = 0;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
    Cursor_HideMenuFocusImmediately();
}

void InterfaceOptions_OnPointerTakeover(CursorTakeoverEvent event) {
    if (event == CURSOR_TAKEOVER_NONE || !g_options.was_active) return;
    g_options.controller_focus_visible = 0;
    Cursor_HideMenuFocusImmediately();
}

/* 上下只在三行范围内移动，不循环。到顶/到底继续按就保持原行。 */
static void options_move_focus(u8* page, int delta) {
    int next = g_options.focus_row + delta;
    if (next < 0) next = 0;
    if (next >= INTERFACE_OPTIONS_ROW_COUNT) next = INTERFACE_OPTIONS_ROW_COUNT - 1;

    g_options.focus_row = next;
    options_show_focus(page);
    Runtime_Log(delta < 0 ? "[机能页] ↑：已移动到上一行。" : "[机能页] ↓：已移动到下一行。");
}

/*
 * 给指定真实 Button 排一次 code=2。
 * 插件不在这里 ++/-- 音量：真正的数值范围、应用声量和配置写回全部位于 0x428950 原版代码中。
 */
static void options_request_button(u8* page, u32 offset, const char* log_line) {
    void* button;
    if (!Runtime_PtrOk(page) || UiBridge_EventPending()) return;
    button = options_button_at(page, offset);
    if (!options_button_usable(button)) return;

    options_show_focus(page);
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, 2, log_line);
}

void InterfaceOptions_Update(void) {
    u8* page = options_page();

    if (!Runtime_PtrOk(page)) {
        if (g_options.was_active) {
            options_reset_visual();
            g_options.was_active = 0;
            Runtime_Log("[机能页] 已离开 state8；手柄焦点与菜单鼠标提示已清理。");
        }
        return;
    }

    /* state8 即使 capability 失败也要把页面专属键留在本页，不能误穿透成其它页面业务。 */
    if (!g_options.enabled) {
        InputRouter_Consume(INPUT_CONFIRM);
        InputRouter_Consume(INPUT_SPECIAL_X);
        InputRouter_Consume(INPUT_SPECIAL_Y);
        InputRouter_Consume(INPUT_NAV_UP);
        InputRouter_Consume(INPUT_NAV_DOWN);
        InputRouter_Consume(INPUT_NAV_LEFT);
        InputRouter_Consume(INPUT_NAV_RIGHT);
        InputRouter_Consume(INPUT_SUBTYPE_PREV);
        InputRouter_Consume(INPUT_SUBTYPE_NEXT);
        return;
    }

    if (!g_options.was_active) {
        options_reset_visual();
        g_options.was_active = 1;
        g_options.focus_row = 0;
        if (Cursor_ControllerOwnsPointer()) options_show_focus(page);
        Runtime_Log("[机能页] 已进入 state8；↑/↓三行焦点、LT/RT音量与A空明流转已启用。");
    }

    /*
     * Yes/No 一旦打开，本页只隐藏底层手形焦点并停止读菜单键。
     * ConfirmDialog 在同一 worker tick 稍后运行，会读取/消费 A/B/方向，因此不会一键穿透到下面。
     */
    if (options_popup_active(page)) {
        if (g_options.controller_focus_visible) {
            Cursor_HideMenuFocusImmediately();
            g_options.controller_focus_visible = 0;
        }
        return;
    }

    /* 从询问框返回后，如果控制器仍拥有指针，就恢复刚才那一行的右侧手形焦点。 */
    if (!g_options.controller_focus_visible && Cursor_ControllerOwnsPointer()) options_show_focus(page);

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_OPTIONS, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        options_move_focus(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_OPTIONS, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        options_move_focus(page, +1);
    }

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_OPTIONS, INPUT_SUBTYPE_PREV, INPUT_LAYER_OVERLAY)) {
        if (g_options.focus_row == 0) {
            options_request_button(page, INTERFACE_OPTIONS_MUSIC_DEC, "[机能页] LT：请求原版音乐减少 ButtonEvent。");
        } else if (g_options.focus_row == 1) {
            options_request_button(page, INTERFACE_OPTIONS_SOUND_DEC, "[机能页] LT：请求原版音效减少 ButtonEvent。");
        }
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_OPTIONS, INPUT_SUBTYPE_NEXT, INPUT_LAYER_OVERLAY)) {
        if (g_options.focus_row == 0) {
            options_request_button(page, INTERFACE_OPTIONS_MUSIC_INC, "[机能页] RT：请求原版音乐增加 ButtonEvent。");
        } else if (g_options.focus_row == 1) {
            options_request_button(page, INTERFACE_OPTIONS_SOUND_INC, "[机能页] RT：请求原版音效增加 ButtonEvent。");
        }
    }

    /* 音乐/音效行的 A 没有业务；只有第三行 A 才点击“空明流转”。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_OPTIONS, INPUT_CONFIRM, INPUT_LAYER_OVERLAY) && g_options.focus_row == 2) {
        options_request_button(page, INTERFACE_OPTIONS_KARMA_BUTTON,
            "[机能页] A：请求原版空明流转 ButtonEvent；后续 Yes/No 由 RPG.exe + ConfirmDialog 接管。");
    }

    /*
     * 这些键属于 state8 自己：不管本帧有没有实际改值，都明确消费，避免底层业务看到同一个边沿。
     * B 和 LB/RB 故意不消费，让 InterfaceShell 保持全局“返回/切大类”语义。
     */
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_SPECIAL_X);
    InputRouter_Consume(INPUT_SPECIAL_Y);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
    InputRouter_Consume(INPUT_SUBTYPE_PREV);
    InputRouter_Consume(INPUT_SUBTYPE_NEXT);
}

int InterfaceOptions_InstallHooks(void) {
    g_options.enabled = 0;
    g_options.was_active = 0;
    options_reset_visual();

    if (!Runtime_InterfaceOptionsProtocolOk()) {
        Runtime_Log("[机能页] 原版 state8 协议不匹配；本页 Adapter 已 fail-closed，其它页面继续工作。");
        return 1;
    }
    if (!UiBridge_InstallInterfaceOptionsHooks()) {
        Runtime_Log("[机能页] state8 五个 ButtonEvent Hook 安装失败；拒绝启用本页 Adapter。");
        return 0;
    }

    g_options.enabled = 1;
    Runtime_Log("[机能页] state8 Adapter 已启用：三行右侧原版手形焦点 + LT/RT音量 + A空明流转。");
    return 1;
}
