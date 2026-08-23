#include "synthesis.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"
#include "ui_bridge.h"
#include "confirm_dialog.h"

/*
 * synthesis.c
 *
 * 用户界面上看起来有“左列表 -> 右列表 -> Yes/No”三层，但原版实际上已经把所有业务都做好了：
 * - 顶部 7 个 Button：index0 是“用器”右侧退出图标，index1..6 是六大类别；
 * - 第一层 9 个行 Button + 上/下页 Button；
 * - 第二层 9 个行 Button + 上/下页 Button + 独立取消 Button；
 * - 可炼化的第二层项目会由原版自己打开 +0x5C8 的 Yes/No；不可炼化项目则原版 Event 自己拒绝。
 *
 * 因此这里绝不能根据道具编号自己判断“能不能炼化”，也不能用鼠标右键假装取消。
 * B 的两种语义都使用已经静态闭合的真实原版 Button：
 * - 第一层 B -> 顶部 index0 退出图标 code=2；
 * - 第二层 B -> second+0x63C 取消 Button code=1，随后 RPG.exe 自己调用 0x410AD0 收起右列表。
 */

typedef struct SynthesisState {
    int was_active;
} SynthesisState;

static SynthesisState g_synthesis;

static u8* synthesis_owner(void) {
    u8* owner = *(u8**)GLOBAL_SYNTHESIS_UI;
    return Runtime_PtrOk(owner) ? owner : NULL;
}

int Synthesis_Active(void) {
    u8* owner = synthesis_owner();
    if (!owner) return 0;
    return *(u8*)(owner + SYNTHESIS_ACTIVE) != 0;
}

static u8* synthesis_topbar(u8* owner) {
    u8* child;
    if (!Runtime_PtrOk(owner)) return NULL;
    child = *(u8**)(owner + SYNTHESIS_TOPBAR);
    return Runtime_PtrOk(child) ? child : NULL;
}

static u8* synthesis_primary(u8* owner) {
    u8* child;
    if (!Runtime_PtrOk(owner)) return NULL;
    child = *(u8**)(owner + SYNTHESIS_PRIMARY_LIST);
    return Runtime_PtrOk(child) ? child : NULL;
}

static u8* synthesis_secondary(u8* owner) {
    u8* child;
    if (!Runtime_PtrOk(owner)) return NULL;
    child = *(u8**)(owner + SYNTHESIS_SECONDARY_LIST);
    return Runtime_PtrOk(child) ? child : NULL;
}

static int synthesis_secondary_active(u8* owner) {
    u8* child = synthesis_secondary(owner);
    return child && *(u8*)(child + SYNTHESIS_CHILD_ACTIVE) != 0;
}

/*
 * +0x5C8 是炼化自己的原版 Yes/No 对象。
 * 如果它已经 open，所有 A/B/方向必须让给 ConfirmDialog，底下两层列表本帧完全停手。
 */
static int synthesis_confirm_active(u8* owner) {
    u8* popup;
    if (!Runtime_PtrOk(owner)) return 0;
    popup = *(u8**)(owner + SYNTHESIS_CONFIRM_POPUP);
    if (!Runtime_PtrOk(popup)) return 0;
    return *(u8*)(popup + POPUP_ACTIVE) != 0;
}

/* topbar index0=退出，index1..6=六种类别。 */
static void* synthesis_top_button(u8* owner, int raw_index) {
    u8* top = synthesis_topbar(owner);
    void* button;
    if (!top || raw_index < 0 || raw_index > SYNTHESIS_CATEGORY_COUNT) return NULL;
    button = *(void**)(top + SYNTHESIS_TOP_BUTTON0 + (u32)raw_index * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/*
 * 行是否“存在”只看原版当前页已经填好的 row record +0x30。
 * 这只是为了不把方向键送到空白行，不等于判断“能否炼化”。
 * 特别是第二层可炼化资格在另一套原版记录 +0x34；本模块故意完全不读取它。
 */
static int synthesis_row_present(u8* list, int secondary, int row) {
    u8* record;
    u32 base;
    if (!Runtime_PtrOk(list) || row < 0 || row >= SYNTHESIS_ROW_COUNT) return 0;

    base = secondary ? SYNTHESIS_SECONDARY_RECORD0 : SYNTHESIS_PRIMARY_RECORD0;
    record = *(u8**)(list + base + (u32)row * 4u);
    if (!Runtime_PtrOk(record)) return 0;
    return *(u8*)(record + SYNTHESIS_RECORD_ACTIVE) != 0;
}

static void* synthesis_row_button(u8* list, int row) {
    void* button;
    if (!Runtime_PtrOk(list) || row < 0 || row >= SYNTHESIS_ROW_COUNT) return NULL;
    button = *(void**)(list + SYNTHESIS_ROW_BUTTON0 + (u32)row * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/* 从当前行向一个方向找同页下一个真正存在的行；没有就保持原行。 */
static int synthesis_find_row(u8* list, int secondary, int current, int delta) {
    int row = current;
    int tries;
    if (row < 0 || row >= SYNTHESIS_ROW_COUNT) row = 0;

    for (tries = 0; tries < SYNTHESIS_ROW_COUNT; ++tries) {
        row += delta;
        if (row < 0 || row >= SYNTHESIS_ROW_COUNT) break;
        if (synthesis_row_present(list, secondary, row)) return row;
    }
    return current;
}

static void synthesis_claim(void) {
    Cursor_ClaimForControllerNavigation();
}

/*
 * ↑/↓不是直接改 parent+current_row。
 * 我们点击目标行的原版 Button，让 0x40FBE0 自己完成 current_row、说明区、图片等同步。
 */
static void synthesis_move_row(u8* owner, int secondary, int delta) {
    u8* list = secondary ? synthesis_secondary(owner) : synthesis_primary(owner);
    u32 row_offset = secondary ? SYNTHESIS_SECONDARY_CURRENT_ROW : SYNTHESIS_PRIMARY_CURRENT_ROW;
    int current;
    int target;
    void* button;

    if (!list || UiBridge_EventPending()) return;
    current = *(i32*)(owner + row_offset);
    if (current < 0 || current >= SYNTHESIS_ROW_COUNT) current = 0;
    target = synthesis_find_row(list, secondary, current, delta);
    if (target == current) return;

    button = synthesis_row_button(list, target);
    if (!button) return;

    synthesis_claim();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SYNTHESIS, button, 2,
        delta < 0 ? "[炼化] ↑：请求原版选择上一项。" : "[炼化] ↓：请求原版选择下一项。");
}

/* LEFT/RIGHT 明确只做上一页/下一页，不把“到列表边缘自动翻页”的其它菜单规则强塞进炼化。 */
static void synthesis_request_page(u8* owner, int secondary, int delta) {
    u8* list = secondary ? synthesis_secondary(owner) : synthesis_primary(owner);
    u32 current_offset = secondary ? SYNTHESIS_SECONDARY_CURRENT_PAGE : SYNTHESIS_PRIMARY_CURRENT_PAGE;
    u32 total_offset = secondary ? SYNTHESIS_SECONDARY_TOTAL_PAGES : SYNTHESIS_PRIMARY_TOTAL_PAGES;
    u32 button_offset;
    int current;
    int total;
    void* button;

    if (!list || UiBridge_EventPending()) return;
    current = *(i32*)(owner + current_offset);
    total = *(i32*)(owner + total_offset);
    if (total <= 0) return;

    if (delta < 0) {
        if (current <= 0) return;
        button_offset = secondary ? SYNTHESIS_SECONDARY_PAGE_PREV : SYNTHESIS_PRIMARY_PAGE_PREV;
    } else {
        if (current + 1 >= total) return;
        button_offset = secondary ? SYNTHESIS_SECONDARY_PAGE_NEXT : SYNTHESIS_PRIMARY_PAGE_NEXT;
    }

    button = *(void**)(list + button_offset);
    if (!Runtime_PtrOk(button)) return;

    synthesis_claim();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SYNTHESIS, button, 2,
        delta < 0 ? "[炼化] ←：请求原版上一页。" : "[炼化] →：请求原版下一页。");
}

/*
 * 六大类别使用循环语义，并严格遵守项目全局 L/R 方向：
 * - LB 永远是“上一个 / 左一个类别”；在最左类别继续回到最右；
 * - RB 永远是“下一个 / 右一个类别”；在最右类别继续回到最左。
 *
 * 注意：炼化的原版类别字段编号方向与画面左右相反，所以本函数必须做 native index 反向映射；
 * 绝不能为了迁就原版编号而把用户侧 LB/RB 语义反过来。最终仍然点击 topbar 的真实分类 Button，
 * 不直接写 +0x5A0/+0x5B4，也不修改 RPG.exe 自己的类别切换业务。
 */
static void synthesis_request_category(u8* owner, int secondary, int delta) {
    u32 current_offset = secondary ? SYNTHESIS_SECONDARY_CATEGORY : SYNTHESIS_PRIMARY_CATEGORY;
    int current;
    int target;
    void* button;

    if (UiBridge_EventPending()) return;
    current = *(i32*)(owner + current_offset);
    if (current < 0 || current >= SYNTHESIS_CATEGORY_COUNT) current = 0;

    /*
     * 这里非常容易写反，必须把“用户看到的方向”和“RPG.exe 内部类别编号方向”分开理解。
     *
     * InputRouter 已经保证：
     *   delta = -1 代表 LB，也就是“视觉上的上一个 / 左一个类别”；
     *   delta = +1 代表 RB，也就是“视觉上的下一个 / 右一个类别”。
     *
     * 但是炼化 owner+category 的原版编号增长方向，和屏幕顶部六个类别从左到右的视觉方向恰好相反。
     * refactor29 直接写成 current + delta，于是把 LB/RB 的用户语义整个翻转了。
     * Adapter 的职责就是消化这种原版内部编号差异：外部 L/R 语义永远不变，只在这里反向换算 native index。
     * 因此：视觉向左(-1) -> 原版编号 +1；视觉向右(+1) -> 原版编号 -1。
     */
    target = current - delta;
    if (target < 0) target = SYNTHESIS_CATEGORY_COUNT - 1;
    if (target >= SYNTHESIS_CATEGORY_COUNT) target = 0;

    /* topbar 的第0项是退出；类别0..5 对应真实 Button index1..6。 */
    button = synthesis_top_button(owner, target + 1);
    if (!button) return;

    synthesis_claim();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SYNTHESIS, button, 2,
        delta < 0 ? "[炼化] LB：请求原版切到左侧类别。" : "[炼化] RB：请求原版切到右侧类别。");
}

/* A 点击当前行；能不能进入下一层/弹 YesNo 完全交给 RPG.exe 自己判断。 */
static void synthesis_confirm_row(u8* owner, int secondary) {
    u8* list = secondary ? synthesis_secondary(owner) : synthesis_primary(owner);
    u32 current_offset = secondary ? SYNTHESIS_SECONDARY_CURRENT_ROW : SYNTHESIS_PRIMARY_CURRENT_ROW;
    int row;
    void* button;

    if (!list || UiBridge_EventPending()) return;
    row = *(i32*)(owner + current_offset);
    if (row < 0 || row >= SYNTHESIS_ROW_COUNT) return;
    if (!synthesis_row_present(list, secondary, row)) return;

    button = synthesis_row_button(list, row);
    if (!button) return;

    synthesis_claim();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_SYNTHESIS, button, 2,
        secondary
            ? "[炼化] A：请求第二层当前原版物品 Event；不可炼化时由原版自行拒绝。"
            : "[炼化] A：请求第一层当前原版物品 Event，合法时由原版展开第二层。");
}

static void synthesis_cancel(u8* owner, int secondary) {
    void* button = NULL;

    if (UiBridge_EventPending()) return;

    if (secondary) {
        u8* list = synthesis_secondary(owner);
        if (list) button = *(void**)(list + SYNTHESIS_SECONDARY_CANCEL_BUTTON);
        if (Runtime_PtrOk(button)) {
            synthesis_claim();
            UiBridge_RequestEventOwned(UI_EVENT_OWNER_SYNTHESIS, button, 1,
                "[炼化] B：请求第二层原版取消 Button；由 RPG.exe 收起右侧列表。");
        }
        return;
    }

    /*
     * 根层 B 必须点击“用器”右侧的真实退出图标。
     * 用户已经明确禁止模拟鼠标右键；因此这里只允许 topbar index0 的 code=2。
     */
    button = synthesis_top_button(owner, 0);
    if (Runtime_PtrOk(button)) {
        synthesis_claim();
        UiBridge_RequestEventOwned(UI_EVENT_OWNER_SYNTHESIS, button, 2,
            "[炼化] B：请求‘用器’右侧原版退出图标；没有使用鼠标右键。");
    }
}

void Synthesis_Update(void) {
    u8* owner;
    int secondary;
    InputContext context;

    if (!Synthesis_Active()) {
        if (g_synthesis.was_active) {
            UiBridge_ClearEventOwned(UI_EVENT_OWNER_SYNTHESIS);
        }
        g_synthesis.was_active = 0;
        return;
    }

    owner = synthesis_owner();
    if (!owner) return;

    if (!g_synthesis.was_active) {
        g_synthesis.was_active = 1;
        Runtime_Log("[炼化] 检测到原版炼化界面；启用两层列表手柄 Adapter。");
    }

    /* 最深层 Yes/No 永远先处理；底下列表这一帧完全不读 A/B/方向/肩键。 */
    if (synthesis_confirm_active(owner) || ConfirmDialog_IsActive()) return;

    secondary = synthesis_secondary_active(owner);
    context = secondary ? INPUT_CTX_SYNTHESIS_SECONDARY : INPUT_CTX_SYNTHESIS_PRIMARY;

    if (InputRouter_PressedOn(context, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        synthesis_move_row(owner, secondary, -1);
        InputRouter_Consume(INPUT_NAV_UP);
    }
    if (InputRouter_PressedOn(context, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        synthesis_move_row(owner, secondary, +1);
        InputRouter_Consume(INPUT_NAV_DOWN);
    }
    if (InputRouter_PressedOn(context, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        synthesis_request_page(owner, secondary, -1);
        InputRouter_Consume(INPUT_NAV_LEFT);
    }
    if (InputRouter_PressedOn(context, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        synthesis_request_page(owner, secondary, +1);
        InputRouter_Consume(INPUT_NAV_RIGHT);
    }
    if (InputRouter_PressedOn(context, INPUT_CATEGORY_PREV, INPUT_LAYER_OVERLAY)) {
        synthesis_request_category(owner, secondary, -1);
        InputRouter_Consume(INPUT_CATEGORY_PREV);
    }
    if (InputRouter_PressedOn(context, INPUT_CATEGORY_NEXT, INPUT_LAYER_OVERLAY)) {
        synthesis_request_category(owner, secondary, +1);
        InputRouter_Consume(INPUT_CATEGORY_NEXT);
    }
    if (InputRouter_PressedOn(context, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        synthesis_confirm_row(owner, secondary);
        InputRouter_Consume(INPUT_CONFIRM);
    }
    if (InputRouter_PressedOn(context, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        synthesis_cancel(owner, secondary);
        InputRouter_Consume(INPUT_CANCEL);
    }
}

void Synthesis_OnPointerTakeover(CursorTakeoverEvent event_type) {
    (void)event_type;
    /* 让真实鼠标重新拥有原版按钮状态；只撤掉还没被消费的炼化手柄事件。 */
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_SYNTHESIS);
}

int Synthesis_InstallHooks(void) {
    if (!Runtime_SynthesisProtocolOk()) {
        Runtime_Log("[炼化] 原版两层列表协议未通过；炼化手柄功能已单独禁用。");
        return 1;
    }
    if (!UiBridge_InstallSynthesisHooks()) {
        Runtime_Log("[炼化] ButtonEvent Hook 安装失败；炼化手柄功能已单独禁用。");
        return 1;
    }

    g_synthesis.was_active = 0;
    Runtime_Log("[炼化] Adapter 已启用：↑↓选物、←→翻页、LB/RB类别、A确认、B按原版层级退出。");
    return 1;
}
