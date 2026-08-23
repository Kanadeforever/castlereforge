#include "interface_tome.h"
#include "runtime.h"
#include "game_addresses.h"
#include "save_slot.h"
#include "cursor.h"

/*
 * interface_tome.c
 *
 * 天书页最容易犯的错误，是看到“存档/读档/取消”以后重新写一套存档菜单。
 * 这会立刻制造两份槽位分页、两份焦点、两份确认框，后面任何一个 bug 都要修两次。
 *
 * 反汇编已经证明原版根本没有第二套：
 *   0x434E93 call 0x4245B0
 *   0x434E9C mov [Interface+0x654], eax
 *
 * 也就是说 state7 直接持有标准 SaveSlot。用户已经验收过标题 SaveSlot，所以这里的正确实现不是“仿照它”，
 * 而是“真的调用同一个 SaveSlot Controller”。只有槽位之后多出来的三项动作窗口需要补手柄导航。
 *
 * refactor31 根据用户实机失败重新取证后，明确修正了 refactor30 的误判：
 * - 天书：GLOBAL_INTERFACE_UI -> Interface+0x654 -> SaveSlot，生命周期是 state7；
 * - 存档点：GLOBAL_SAVE_POINT_UI(0x89FCD0) -> wrapper+0x580 -> SaveSlot，生命周期是 0x413B00 包装层。
 *
 * 两条路的 owner/lifecycle 不同，所以必须分别由 interface_tome.c 和 save_point.c 配对 Begin/End；
 * 但它们解包后得到的都是同一种 0x4245B0 SaveSlot，槽位、分页、A/B 与存档 I/O 仍只有 save_slot.c 一份。
 * 这才是本项目的真正复用边界：“owner Adapter 可以多个，业务 Controller 始终唯一”。
 */

typedef struct InterfaceTomeState {
    /* 独立协议预检通过后才置 1；失败时只让原版键鼠继续工作。 */
    int enabled;

    /* 记录上一 tick 是否真的处于 state7，用来精确配对 SaveSlot_Begin/End。 */
    int was_active;

    /* 当前绑定给共享 SaveSlot Controller 的原版对象；页面重建时会变化，不能假定永远不变。 */
    u8* object;
} InterfaceTomeState;

static InterfaceTomeState g_tome;

/* 每 tick 重新取得 Interface，避免菜单关闭后继续拿一根已经失效的旧指针。 */
static u8* tome_interface(void) {
    u8* interface_ui = *(u8**)GLOBAL_INTERFACE_UI;
    return Runtime_PtrOk(interface_ui) ? interface_ui : NULL;
}

/*
 * 只有 state==7、Interface 没在关闭、page+0x579 仍是 active 时才把对象交给 SaveSlot。
 * 这些条件都来自原版对象生命周期；任何一项不成立都宁可本 tick 不处理，也不猜对象是否还能使用。
 */
static u8* tome_save_slot(void) {
    u8* interface_ui = tome_interface();
    u8* save;

    if (!Runtime_PtrOk(interface_ui)) return NULL;
    if (*(i32*)(interface_ui + INTERFACE_STATE) != 7) return NULL;
    if (*(i32*)(interface_ui + INTERFACE_CLOSE_STATE) != 0) return NULL;

    save = *(u8**)(interface_ui + INTERFACE_PAGE_7);
    if (!Runtime_PtrOk(save)) return NULL;
    if (*(u8*)(save + SAVE_ACTIVE) == 0) return NULL;
    return save;
}

int InterfaceTome_Active(void) {
    return tome_save_slot() != NULL;
}

/*
 * 三项动作窗口或任一 Yes/No 出现时，天书已经不再是“根层 SaveSlot”。
 * 这时 LB/RB/B 若继续落进 Shell，会造成“一边确认存档、一边切大类/退出”的穿透，所以必须报告 modal。
 * 普通槽位列表返回 0：那里 B 仍由 SaveSlot 自己处理，LB/RB 仍允许 Shell 切主大类。
 */
int InterfaceTome_ModalActive(void) {
    u8* save = tome_save_slot();
    SaveSlotView view;

    if (!Runtime_PtrOk(save)) return 0;
    view = SaveSlot_DetectView(save);
    return view == SAVE_VIEW_POPUP || view == SAVE_VIEW_ACTION || view == SAVE_VIEW_ACTION_POPUP;
}

/* 指针接管只需要通知共享 SaveSlot；它会清掉自己建立的强制导航视觉。 */
void InterfaceTome_OnPointerTakeover(CursorTakeoverEvent event) {
    if (event == CURSOR_TAKEOVER_NONE || !g_tome.was_active) return;
    SaveSlot_OnPointerTakeover();
}

void InterfaceTome_Update(void) {
    u8* save = tome_save_slot();

    /* 离开 state7：一定先结束共享 Controller，不能让天书焦点残留到标题读档或下一次打开菜单。 */
    if (!Runtime_PtrOk(save)) {
        if (g_tome.was_active) {
            SaveSlot_End();
            g_tome.object = NULL;
            g_tome.was_active = 0;
            Runtime_Log("[天书页] 已离开 state7；共享 SaveSlot 会话已结束。");
        }
        return;
    }

    /* 协议失败时页面仍归 RPG.exe 原版键鼠；这里只保持静默，不碰共享 SaveSlot。 */
    if (!g_tome.enabled) return;

    /*
     * 第一次进入，或者 RPG.exe 因重建页面换了对象地址，都重新 Begin。
     * from_pad 取当前“控制器是否拥有指针”：
     * - 用 Y/LB/RB 进入时 Shell 已经取得控制器所有权，SaveSlot 会立刻建立手柄焦点；
     * - 实体鼠标点进来时保持鼠标原样，不强行抢焦点。
     */
    if (!g_tome.was_active || g_tome.object != save) {
        if (g_tome.was_active) SaveSlot_End();
        g_tome.object = save;
        g_tome.was_active = 1;
        SaveSlot_Begin(save, Cursor_ControllerOwnsPointer() ? 1 : 0);
        Runtime_Log("[天书页] 已进入 state7；槽位与三项动作窗口直接复用共享 SaveSlot Controller。");
    }

    /* 所有槽位、翻页、三项动作窗口的输入都由同一个共享 Controller 处理。 */
    SaveSlot_Update(save);
}

int InterfaceTome_InstallHooks(void) {
    g_tome.enabled = 0;
    g_tome.was_active = 0;
    g_tome.object = NULL;

    if (!Runtime_InterfaceTomeProtocolOk()) {
        Runtime_Log("[天书页] 原版 state7 协议不匹配；天书手柄 Adapter 已 fail-closed，已 PASS 页面不受影响。");
        return 1;
    }

    g_tome.enabled = 1;
    Runtime_Log("[天书页] state7 owner Adapter 已启用；共享动作窗口能力由 SaveSlot 独立安装。");
    return 1;
}
