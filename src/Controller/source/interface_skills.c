#include "interface_skills.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "ui_bridge.h"

/*
 * interface_skills.c
 *
 * state3“绝学/法术”页的设计原则与法宝页一致：
 * 1. 只点击原版真实 ButtonEvent，不直接写当前页、当前行、子类型或技能结果；
 * 2. LT 永远表示视觉左侧/上一个子类型，RT 永远表示视觉右侧/下一个子类型；
 * 3. ↑/↓ 只在技能列表内部移动，到边界时调用原版上一页/下一页；
 * 4. A 点击当前技能。原版决定是不可用、直接使用还是打开使用确认框；
 * 5. 使用确认框里 A 必须服从屏幕当前真实高亮，B 永远走原版取消按钮。
 *
 * 为什么不直接复制 state2 的偏移：
 * state3 的页面对象、列表对象、子类型面板和当前页字段都位于不同位置。
 * 即使两页看起来都像“顶部分类 + 8 行列表”，代码也必须按 state3 自己的逆向证据读取。
 */

typedef struct InterfaceSkillsState {
    int enabled;
    int was_active;

    /* 翻页事务：页面数字由原版改，插件只记住方向并在新页恢复合理行焦点。 */
    int page_wait_direction;
    int page_before_request;
    int last_subtype_seen;

    /* 页面专属使用确认框：0=第一个/确定，1=第二个/取消。 */
    int popup_focus;
    int popup_nav_active;
    void* popup_owner;

    /*
     * 治疗法术“确定使用”以后，原版不是立刻结算，而是进入 page+0x768 的鼠标选人阶段。
     * 这里的三个字段全部只是插件自己的手柄焦点：
     * - target_was_active：用于识别刚进入/刚离开目标选择，配对显示与清理；
     * - target_focus：0..4 的顶部角色位置索引，不是 RPG.exe 的角色 ID；
     * - target_nav_active：手柄是否正在拥有这个目标选择。实体鼠标/右摇杆接管后会清 0。
     * 真正目标 ID 仍由 0x43C920 在原版 ButtonEvent 返回 code=2 后自己读取并写入业务字段。
     */
    int target_was_active;
    int target_focus;
    int target_nav_active;
} InterfaceSkillsState;

static InterfaceSkillsState g_skills;

/* 安全取得全局主 Interface。 */
static u8* skills_interface(void) {
    u8* i = *(u8**)GLOBAL_INTERFACE_UI;
    return Runtime_PtrOk(i) ? i : NULL;
}

/* 只有 state3、且 Interface 没有进入关闭生命周期时才返回技能页对象。 */
static u8* skills_page(void) {
    u8* i = skills_interface();
    u8* page;

    /* 第一步：全局 Interface 指针必须落在当前进程可读范围；坏指针绝不能继续加偏移。 */
    if (!Runtime_PtrOk(i)) return NULL;

    /* 第二步：+0x5BC 是主 Interface 当前大类；只有 3 才是“绝学/法术”。 */
    if (*(i32*)(i + INTERFACE_STATE) != 3) return NULL;

    /* 第三步：+0x5F8 非 0 表示主界面正在关闭；关闭动画中不再提交新按钮事件。 */
    if (*(i32*)(i + INTERFACE_CLOSE_STATE) != 0) return NULL;

    /* 最后才读取 Interface+0x644 的 state3 页面对象，并再次做指针合法性检查。 */
    page = *(u8**)(i + INTERFACE_PAGE_3);
    return Runtime_PtrOk(page) ? page : NULL;
}

/* 镜像 0x431380 的两个核心按钮可用门，禁止手柄强行点击原版禁用按钮。 */
static int skills_button_usable(void* button) {
    u8* b = (u8*)button;
    /* Button 本身必须是可读对象，否则连 +0x45 都不能碰。 */
    if (!Runtime_PtrOk(b)) return 0;

    /* 原版 +0x45==0 表示按钮当前没有启用/显示给业务层使用。 */
    if (*(u8*)(b + 0x45u) == 0) return 0;

    /* 原版 +0x04!=0 时 ButtonEvent 自己也会拒绝，本插件不能绕过这个门。 */
    if (*(u8*)(b + 0x04u) != 0) return 0;

    /* 两个原版门都通过，才把它视为“手柄可以请求的真实按钮”。 */
    return 1;
}

/* state3 的列表对象在 page+0x5DC。 */
static u8* skills_list(u8* page) {
    u8* list;
    if (!Runtime_PtrOk(page)) return NULL;
    list = *(u8**)(page + INTERFACE_SKILLS_LIST);
    return Runtime_PtrOk(list) ? list : NULL;
}

/* 取得第 row 行的真实技能 Button，并同时确认对应记录当前确实 active。 */
static void* skills_row_button(u8* page, int row) {
    u8* list = skills_list(page);
    u8* record;
    void* button;

    if (!Runtime_PtrOk(list)) return NULL;
    if (row < 0 || row >= INTERFACE_SKILLS_ROW_COUNT) return NULL;

    /* 先取这一行的数据记录。空行虽然可能还残留 Button 指针，但不能当成真实技能。 */
    record = *(u8**)(list + INTERFACE_SKILLS_ROW_RECORD0 + (u32)row * 4u);
    if (!Runtime_PtrOk(record)) return NULL;

    /* record+0x30 是原版行有效标志；0 就代表这一槽当前没有可选技能。 */
    if (*(u8*)(record + INTERFACE_SKILLS_RECORD_ACTIVE) == 0) return NULL;

    /* 记录有效后才取得同一行的 Button，再交给统一的原版按钮可用门检查。 */
    button = *(void**)(list + INTERFACE_SKILLS_ROW_BUTTON0 + (u32)row * 4u);
    return skills_button_usable(button) ? button : NULL;
}

/* 统计当前页最后一个真实可操作行，完全空页返回 0。 */
static int skills_visible_row_count(u8* page) {
    int row;
    int count = 0;

    for (row = 0; row < INTERFACE_SKILLS_ROW_COUNT; ++row) {
        if (skills_row_button(page, row)) count = row + 1;
    }
    return count;
}

/* 取得两个顶部子类型按钮中的一个。这里不假设 index 的增减方向等于视觉左右。 */
static void* skills_subtype_button(u8* page, int index) {
    u8* panel;
    void* button;

    if (!Runtime_PtrOk(page)) return NULL;
    if (index < 0 || index >= INTERFACE_SKILLS_SUBTYPE_COUNT) return NULL;

    panel = *(u8**)(page + INTERFACE_SKILLS_SUBPANEL);
    if (!Runtime_PtrOk(panel)) return NULL;
    button = *(void**)(panel + INTERFACE_SKILLS_SUB_BUTTON0 + (u32)index * 4u);
    return skills_button_usable(button) ? button : NULL;
}

/* 所有 state3 按钮请求都标记为 Interface owner，避免和 Battle/SaveSlot 的 pending 混在一起。 */
static int skills_request_event(void* button, int code, const char* log_text) {
    /* 禁用按钮绝不创建 pending；这是 Battle“不可用 A 假死”之后统一坚持的逃生原则。 */
    if (!skills_button_usable(button)) return 0;

    /* UiBridge 一次只能有一个尚未被原版 Event 调用点消费的请求，避免两个动作互相覆盖。 */
    if (UiBridge_EventPending()) return 0;

    /* 手柄开始导航时取得光标所有权，避免实体鼠标 hover 在同一帧把视觉焦点抢回去。 */
    Cursor_ClaimForControllerNavigation();

    /* 这里只“排队请求某个真实按钮返回 code=2”，真正业务仍由 RPG.exe 原版 Event 路径执行。 */
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, code, log_text);
    return UiBridge_EventOwner() == UI_EVENT_OWNER_INTERFACE;
}

/* state3 只有一个页面专属使用确认框：page+0x5FC。 */
static u8* skills_active_popup(void) {
    u8* page = skills_page();
    u8* popup;

    if (!Runtime_PtrOk(page)) return NULL;
    popup = *(u8**)(page + INTERFACE_SKILLS_POPUP);
    if (!Runtime_PtrOk(popup)) return NULL;
    return *(u8*)(popup + POPUP_ACTIVE) != 0 ? popup : NULL;
}

int InterfaceSkills_AnyPopupActive(void) {
    return Runtime_PtrOk(skills_active_popup());
}

/*
 * 原版 state3 的治疗法术目标选择标志位于 page+0x768。
 * 注意：它不是 popup，所以必须单独暴露给 InterfaceShell，作为“更深层 modal”阻断根菜单输入。
 */
int InterfaceSkills_TargetSelectionActive(void) {
    u8* page = skills_page();
    if (!Runtime_PtrOk(page)) return 0;
    return *(u8*)(page + INTERFACE_SKILLS_TARGET_ACTIVE) != 0;
}

/* 取得当前主 Interface 的真实队伍人数，最多只认原版已经确认的 5 个顶部角色位置。 */
static int skills_target_role_count(void) {
    u8* interface_ui = skills_interface();
    int count;

    if (!Runtime_PtrOk(interface_ui)) return 0;
    count = *(i32*)(interface_ui + INTERFACE_ROLE_COUNT);
    if (count < 1) return 0;
    if (count > INTERFACE_SKILLS_TARGET_BUTTON_COUNT) count = INTERFACE_SKILLS_TARGET_BUTTON_COUNT;
    return count;
}

/*
 * 0x43C920 原版自己就是从 page+0x770 起连续扫描 5 个透明角色 Button。
 * 插件沿用同一张 Button 表，因此屏幕中心、左键选择与原版鼠标路径天然落在同一对象上。
 */
static void* skills_target_button(u8* page, int index) {
    void* button;
    int count = skills_target_role_count();

    if (!Runtime_PtrOk(page) || index < 0 || index >= count) return NULL;
    button = *(void**)(page + INTERFACE_SKILLS_TARGET_BUTTON0 + (u32)index * 4u);
    return skills_button_usable(button) ? button : NULL;
}

/*
 * 把原版已经切成“目标选择颜色”的手形鼠标放到顶部角色透明 Button 的严格几何中心。
 * UiBridge_GetButtonScreenCenter 会完整处理 Button 宽高 override=0 时的 Sprite frame fallback，
 * 因而这里不自己猜 100x95，也不硬编码角色头像坐标。
 */
static int skills_target_show_focus(u8* page) {
    void* button;
    i32 x;
    i32 y;

    button = skills_target_button(page, g_skills.target_focus);
    if (!Runtime_PtrOk(button)) return 0;
    if (!UiBridge_GetButtonScreenCenter(button, &x, &y)) return 0;

    Cursor_ClaimForControllerNavigation();
    Cursor_ShowMenuFocusAt(x, y);
    g_skills.target_nav_active = 1;
    return 1;
}

/*
 * 刚进入目标选择时默认对准“当前主 Interface 角色”。
 * 这是最符合用户视线的起点：刚才正在操作谁的法术，手形先落在谁的顶部头像正中心；
 * 若该索引异常或对应 Button 暂时不可用，则只在插件本地寻找第一个可用角色，不改原版 selected-role。
 */
static void skills_target_begin(u8* page) {
    u8* interface_ui = skills_interface();
    int count = skills_target_role_count();
    int start = 0;
    int i;

    g_skills.target_was_active = 1;
    g_skills.target_nav_active = 0;

    if (Runtime_PtrOk(interface_ui)) start = *(i32*)(interface_ui + INTERFACE_SELECTED_ROLE);
    if (start < 0 || start >= count) start = 0;
    g_skills.target_focus = start;

    /* 只有确认窗口刚才由手柄控制时才自动把鼠标拉到头像中心；纯键鼠路径保持原版自由鼠标。 */
    if (!Cursor_ControllerOwnsPointer()) {
        Runtime_Log("[绝学页] 治疗目标选择已进入；当前由键鼠拥有指针，保持原版鼠标路径。");
        return;
    }

    if (skills_target_show_focus(page)) {
        Runtime_Log("[绝学页] 治疗目标选择已进入；手形已对准当前角色头像中心。");
        return;
    }

    /* 极端情况下当前角色 Button 还没准备好，按从左到右顺序找一个真正可用的原版目标 Button。 */
    for (i = 0; i < count; ++i) {
        g_skills.target_focus = i;
        if (skills_target_show_focus(page)) {
            Runtime_Log("[绝学页] 当前角色目标 Button 未就绪；已回落到第一个可用角色头像。");
            return;
        }
    }

    g_skills.target_nav_active = 0;
    Runtime_Log("[绝学页] 治疗目标选择已进入，但当前没有可取得几何中心的角色 Button；保留原版键鼠路径。");
}

/* 左右在真实队伍范围内循环寻找下一个可用角色 Button；只改插件私有索引。 */
static void skills_target_move(u8* page, int delta) {
    int count = skills_target_role_count();
    int candidate = g_skills.target_focus;
    int tries;

    if (!Runtime_PtrOk(page) || count <= 0 || delta == 0) return;

    for (tries = 0; tries < count; ++tries) {
        candidate += delta < 0 ? -1 : 1;
        if (candidate < 0) candidate = count - 1;
        if (candidate >= count) candidate = 0;

        if (skills_target_button(page, candidate)) {
            g_skills.target_focus = candidate;
            if (skills_target_show_focus(page)) {
                Runtime_Log(delta < 0 ?
                    "[绝学页] 治疗目标：已选择左侧角色。" :
                    "[绝学页] 治疗目标：已选择右侧角色。");
            }
            return;
        }
    }
}

/* A 不写目标 ID；只让 0x43C933 轮到当前透明角色 Button 时返回一次原版左键 code=2。 */
static void skills_target_confirm(u8* page) {
    void* button;

    if (!Runtime_PtrOk(page) || UiBridge_EventPending()) return;
    if (!g_skills.target_nav_active) {
        /* 用户可能刚从实体鼠标切回手柄；A 本身也应当重新取得手柄目标焦点。 */
        if (!skills_target_show_focus(page)) return;
    }

    button = skills_target_button(page, g_skills.target_focus);
    if (!skills_request_event(button, 2,
        "[绝学页] A：向原版角色目标 Button 注入左键等价 code=2；治疗对象由 RPG.exe 自己解析。")) return;
}

/* B 复用 page+0x784 的原版全屏取消 Button，code=1 与真实鼠标右键路径完全一致。 */
static void skills_target_cancel(u8* page) {
    void* button;

    if (!Runtime_PtrOk(page)) return;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
    button = *(void**)(page + INTERFACE_SKILLS_TARGET_CANCEL);

    /* 取消前重新取得手柄所有权只是为了避免实体鼠标同帧 hover 干扰；业务仍只走原版 ButtonEvent。 */
    Cursor_ClaimForControllerNavigation();
    g_skills.target_nav_active = 1;
    skills_request_event(button, 1,
        "[绝学页] B：向原版全屏取消 Button 注入右键等价 code=1；返回常规技能菜单。");
}

/*
 * 治疗目标选择是 state3 当前最深层 modal：只有 ←/→、A、B 有业务，其余菜单键全部吞掉。
 * 实体鼠标和右摇杆仍可以通过原版 ButtonEvent 正常选人；只有再次按手柄目标键时才重新对准私有焦点。
 */
static void skills_update_target_selector(u8* page) {
    if (!g_skills.target_was_active) skills_target_begin(page);

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        if (!g_skills.target_nav_active) skills_target_show_focus(page);
        skills_target_move(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        if (!g_skills.target_nav_active) skills_target_show_focus(page);
        skills_target_move(page, +1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        skills_target_confirm(page);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        skills_target_cancel(page);
    }

    /* modal 期间禁止列表翻页、切 subtype、切大类或执行 X/Y；Start/R3 等系统/鼠标能力仍由各自底层处理。 */
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_CANCEL);
    InputRouter_Consume(INPUT_SPECIAL_X);
    InputRouter_Consume(INPUT_SPECIAL_Y);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
    InputRouter_Consume(INPUT_CATEGORY_PREV);
    InputRouter_Consume(INPUT_CATEGORY_NEXT);
    InputRouter_Consume(INPUT_SUBTYPE_PREV);
    InputRouter_Consume(INPUT_SUBTYPE_NEXT);
}

/* 新弹窗刚出现时安全默认“取消”，不再相信构造函数临时写入的 +0x58C=1。 */
static void skills_sync_popup_owner(u8* popup) {
    if ((void*)popup == g_skills.popup_owner) return;

    g_skills.popup_owner = popup;
    g_skills.popup_nav_active = 0;
    g_skills.popup_focus = 1;
}

static void skills_claim_popup_navigation(void) {
    Cursor_ClaimForControllerNavigation();
    g_skills.popup_nav_active = 1;
}

/*
 * 观察原版 HitTest，而不是猜原版选择字段。
 * 原函数返回非 0 就意味着这个按钮正是屏幕本帧真正命中的按钮，因此 A 应该跟随它。
 */
void InterfaceSkills_ObservePopupHit(void* button, u8 hit_value) {
    u8* popup = skills_active_popup();
    void* yes_button;
    void* no_button;

    /* 一旦手柄方向已经主动接管 popup，就不能再让实体鼠标 HitTest 覆盖手柄逻辑焦点。 */
    if (g_skills.popup_nav_active) return;

    /* HitTest 返回 0 只代表“没命中这个按钮”，不能据此推断另一个按钮一定被选中。 */
    if (hit_value == 0) return;

    /* popup 和按钮都要能安全读取，防止关闭瞬间对象已经失效。 */
    if (!Runtime_PtrOk(popup) || !Runtime_PtrOk(button)) return;

    /* 先识别是否换了新 popup；新对象会先回到“安全取消”的逻辑默认。 */
    skills_sync_popup_owner(popup);
    yes_button = *(void**)(popup + POPUP_BUTTON_YES);
    no_button = *(void**)(popup + POPUP_BUTTON_NO);

    if (button == yes_button) g_skills.popup_focus = 0;
    if (button == no_button) g_skills.popup_focus = 1;
}

/* 手柄导航开始后，强制原版两个 HitTest 的视觉结果与逻辑焦点一致。 */
u8 InterfaceSkills_FilterPopupHit(void* button, int* handled) {
    u8* popup = skills_active_popup();
    void* yes_button;
    void* no_button;

    if (handled) *handled = 0;
    if (!g_skills.enabled || !g_skills.popup_nav_active) return 0;
    if (!Runtime_PtrOk(popup) || !Runtime_PtrOk(button)) return 0;
    if ((void*)popup != g_skills.popup_owner) return 0;

    yes_button = *(void**)(popup + POPUP_BUTTON_YES);
    no_button = *(void**)(popup + POPUP_BUTTON_NO);

    if (button == yes_button) {
        if (handled) *handled = 1;
        return (u8)(g_skills.popup_focus == 0);
    }
    if (button == no_button) {
        if (handled) *handled = 1;
        return (u8)(g_skills.popup_focus == 1);
    }
    return 0;
}

/* state3 使用确认框：方向选择、A 当前项、B 永远取消。 */
static void skills_update_popup(u8* popup) {
    void* button;

    skills_sync_popup_owner(popup);

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_UP, INPUT_LAYER_OVERLAY) ||
        InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        skills_claim_popup_navigation();
        g_skills.popup_focus = 0;
        Runtime_Log("[绝学页] 使用确认焦点：确定。");
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY) ||
        InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        skills_claim_popup_navigation();
        g_skills.popup_focus = 1;
        Runtime_Log("[绝学页] 使用确认焦点：取消。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        /* A 的第一步只是锁定“手柄现在拥有 popup 焦点”，并不会自动等价于确定。 */
        skills_claim_popup_navigation();

        /* 第二步根据当前焦点选真实 Button：focus=0 才是确定，focus=1 必须点取消。 */
        button = *(void**)(popup + (g_skills.popup_focus == 0 ? POPUP_BUTTON_YES : POPUP_BUTTON_NO));
        skills_request_event(button, 2, g_skills.popup_focus == 0 ?
                            "[绝学页] A：执行当前视觉焦点的确定按钮。" :
                            "[绝学页] A：执行当前视觉焦点的取消按钮。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        skills_claim_popup_navigation();
        g_skills.popup_focus = 1;
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
        button = *(void**)(popup + POPUP_BUTTON_NO);
        skills_request_event(button, 2, "[绝学页] B：执行原版取消按钮。");
    }

    /* 弹窗是当前最深层，所有菜单导航键都不能穿透到下面的 InterfaceShell。 */
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_CANCEL);
    InputRouter_Consume(INPUT_SPECIAL_X);
    InputRouter_Consume(INPUT_SPECIAL_Y);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
    InputRouter_Consume(INPUT_CATEGORY_PREV);
    InputRouter_Consume(INPUT_CATEGORY_NEXT);
    InputRouter_Consume(INPUT_SUBTYPE_PREV);
    InputRouter_Consume(INPUT_SUBTYPE_NEXT);
}

/*
 * LT/RT 用真实 Button 的 X 坐标决定方向。
 * direction<0 永远代表视觉左侧/上一项；direction>0 永远代表视觉右侧/下一项。
 */
static void skills_change_subtype(u8* page, int direction) {
    u8* panel;
    int current;
    int other;
    void* current_button;
    void* other_button;
    int current_x;
    int other_x;

    if (!Runtime_PtrOk(page) || direction == 0 || UiBridge_EventPending()) return;
    panel = *(u8**)(page + INTERFACE_SKILLS_SUBPANEL);
    if (!Runtime_PtrOk(panel)) return;

    current = *(i32*)(panel + INTERFACE_SKILLS_SUBINDEX);
    if (current < 0 || current >= INTERFACE_SKILLS_SUBTYPE_COUNT) current = 0;
    other = current == 0 ? 1 : 0;

    current_button = skills_subtype_button(page, current);
    other_button = skills_subtype_button(page, other);
    if (!Runtime_PtrOk(current_button) || !Runtime_PtrOk(other_button)) return;

    /* 读取按钮自己的屏幕 X，而不是把内部 index 当成视觉方向。 */
    current_x = *(i32*)((u8*)current_button + 0x34u);
    other_x = *(i32*)((u8*)other_button + 0x34u);

    /*
     * 这里只有两个子类型，所以循环序列中“上一项”和“下一项”最终都会是另一个按钮。
     * 仍然读取 X 是为了把规则写死：以后若数量变化，direction<0/ >0 的视觉含义不能反转。
     */
    if ((direction < 0 && other_x < current_x) ||
        (direction > 0 && other_x > current_x) ||
        INTERFACE_SKILLS_SUBTYPE_COUNT == 2) {
        skills_request_event(other_button, 2, direction < 0 ?
                            "[绝学页] LT：切到视觉左侧/上一个子类型。" :
                            "[绝学页] RT：切到视觉右侧/下一个子类型。");
    }
}

/* 请求原版技能上一页/下一页。页码由原版修改，插件只等待变化后恢复行焦点。 */
static void skills_request_page(u8* page, int direction) {
    u8* list = skills_list(page);
    int current_page;
    int total_pages;
    void* button;

    if (!Runtime_PtrOk(list) || direction == 0) return;
    if (g_skills.page_wait_direction != 0 || UiBridge_EventPending()) return;

    current_page = *(i32*)(page + INTERFACE_SKILLS_CURRENT_PAGE);
    total_pages = *(i32*)(page + INTERFACE_SKILLS_TOTAL_PAGES);
    if (total_pages < 1) total_pages = 1;

    if (direction < 0) {
        /* 已经是第一页时，“上一页”没有合法目标，直接保持当前页。 */
        if (current_page <= 0) return;

        /* 取原版上一页 Button；页码绝不由插件自己 current_page--。 */
        button = *(void**)(list + INTERFACE_SKILLS_PAGE_PREV_BUTTON);
    } else {
        if (current_page + 1 >= total_pages) return;
        button = *(void**)(list + INTERFACE_SKILLS_PAGE_NEXT_BUTTON);
    }

    if (!skills_button_usable(button)) return;
    g_skills.page_wait_direction = direction < 0 ? -1 : 1;
    g_skills.page_before_request = current_page;
    if (!skills_request_event(button, 2, direction < 0 ?
                              "[绝学页] 请求原版上一页。" :
                              "[绝学页] 请求原版下一页。")) {
        g_skills.page_wait_direction = 0;
        g_skills.page_before_request = -1;
    }
}

/* 原版页码变化后，把焦点落到下一页第0行或上一页最后有效行。 */
static void skills_finish_page_landing(u8* page) {
    int current_page;
    int row_count;
    int target_row;
    void* button;

    if (!Runtime_PtrOk(page) || g_skills.page_wait_direction == 0) return;
    current_page = *(i32*)(page + INTERFACE_SKILLS_CURRENT_PAGE);
    if (current_page == g_skills.page_before_request) return;

    row_count = skills_visible_row_count(page);
    if (row_count <= 0) {
        g_skills.page_wait_direction = 0;
        g_skills.page_before_request = -1;
        return;
    }

    target_row = g_skills.page_wait_direction > 0 ? 0 : row_count - 1;
    g_skills.page_wait_direction = 0;
    g_skills.page_before_request = -1;

    button = skills_row_button(page, target_row);
    if (skills_button_usable(button)) {
        skills_request_event(button, 2, "[绝学页] 原版翻页完成：恢复新页技能焦点。");
    }
}

/* ↑/↓ 在当前8行内移动；越过首尾则走原版上一页/下一页。 */
static void skills_move_vertical(u8* page, int direction) {
    int current_row;
    int row_count;
    int target_row;
    void* button;

    if (!Runtime_PtrOk(page) || direction == 0) return;
    if (g_skills.page_wait_direction != 0 || UiBridge_EventPending()) return;

    row_count = skills_visible_row_count(page);
    if (row_count <= 0) return;
    current_row = *(i32*)(page + INTERFACE_SKILLS_CURRENT_ROW);
    if (current_row < 0 || current_row >= row_count) current_row = 0;

    /* 先计算“如果还在本页，下一步应该落在哪一行”。 */
    target_row = current_row + (direction < 0 ? -1 : 1);

    /* 小于 0 说明从首行继续按 ↑，此时才请求原版上一页。 */
    if (target_row < 0) {
        skills_request_page(page, -1);
        return;
    }
    if (target_row >= row_count) {
        skills_request_page(page, 1);
        return;
    }

    button = skills_row_button(page, target_row);
    if (skills_button_usable(button)) {
        skills_request_event(button, 2, direction < 0 ?
                            "[绝学页] ↑：选择上一条技能。" :
                            "[绝学页] ↓：选择下一条技能。");
    }
}

/* A 点击当前选中行；不可用技能直接忽略，不留下 pending。 */
static void skills_confirm_current(u8* page) {
    int row;
    void* button;

    if (!Runtime_PtrOk(page) || g_skills.page_wait_direction != 0 || UiBridge_EventPending()) return;
    row = *(i32*)(page + INTERFACE_SKILLS_CURRENT_ROW);
    button = skills_row_button(page, row);
    if (!skills_button_usable(button)) {
        Runtime_Log("[绝学页] A：当前技能不可用/不存在，本次确认已忽略。");
        return;
    }
    skills_request_event(button, 2, "[绝学页] A：执行当前技能原版行事件。");
}

static void skills_reset_transient(void) {
    g_skills.page_wait_direction = 0;
    g_skills.page_before_request = -1;
    g_skills.last_subtype_seen = -1;
    g_skills.popup_focus = 1;
    g_skills.popup_nav_active = 0;
    g_skills.popup_owner = NULL;
    g_skills.target_was_active = 0;
    g_skills.target_focus = 0;
    g_skills.target_nav_active = 0;
    Cursor_HideMenuFocusImmediately();
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
}

void InterfaceSkills_OnPointerTakeover(CursorTakeoverEvent event) {
    if (event == CURSOR_TAKEOVER_NONE) return;
    g_skills.popup_nav_active = 0;
    g_skills.popup_owner = NULL;
    g_skills.page_wait_direction = 0;
    g_skills.page_before_request = -1;

    /*
     * 治疗目标模式本身是 RPG.exe 的 +0x768 状态，实体鼠标/右摇杆接管时绝不能把它取消。
     * 这里只撤掉插件私有的“当前目标索引 + 强制手形位置”，让用户立即恢复原版自由鼠标选择。
     */
    if (g_skills.target_was_active) {
        g_skills.target_nav_active = 0;
        Cursor_HideMenuFocusImmediately();
    }
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
}

void InterfaceSkills_Update(void) {
    u8* page;
    u8* popup;
    u8* panel;
    int subtype;

    /* 协议预检失败时 enabled=0，本模块彻底静默，不影响 r19/r20a 稳定底座。 */
    if (!g_skills.enabled) return;

    /* 每个 worker tick 都重新确认当前确实处于 state3，不能缓存旧页面指针跨生命周期使用。 */
    page = skills_page();
    if (!Runtime_PtrOk(page)) {
        if (g_skills.was_active) {
            skills_reset_transient();
            g_skills.was_active = 0;
            Runtime_Log("[绝学页] 已离开 state3；页面专属手柄状态已清理。");
        }
        return;
    }

    panel = *(u8**)(page + INTERFACE_SKILLS_SUBPANEL);
    if (!Runtime_PtrOk(panel)) return;

    if (!g_skills.was_active) {
        skills_reset_transient();
        g_skills.was_active = 1;
        g_skills.last_subtype_seen = *(i32*)(panel + INTERFACE_SKILLS_SUBINDEX);
        Runtime_Log("[绝学页] 已进入 state3；列表/分页/确认以及治疗法术角色目标选择 Adapter 开始工作。");
    }

    /*
     * 原版 0x43C160 Update 也是先检查 +0x768 目标选择，再检查 +0x5FC 使用确认 popup。
     * 保持同一优先级可以避免“确认窗口已经关了，但 A/B 又被底层列表提前读取”的一帧穿透。
     */
    if (*(u8*)(page + INTERFACE_SKILLS_TARGET_ACTIVE) != 0) {
        skills_update_target_selector(page);
        return;
    }

    if (g_skills.target_was_active) {
        g_skills.target_was_active = 0;
        g_skills.target_nav_active = 0;
        Cursor_HideMenuFocusImmediately();
        Runtime_Log("[绝学页] 治疗目标选择已结束；恢复常规技能菜单手柄控制。");
    }

    popup = skills_active_popup();
    if (Runtime_PtrOk(popup)) {
        skills_update_popup(popup);
        return;
    }
    g_skills.popup_owner = NULL;
    g_skills.popup_nav_active = 0;

    subtype = *(i32*)(panel + INTERFACE_SKILLS_SUBINDEX);
    if (subtype != g_skills.last_subtype_seen) {
        g_skills.last_subtype_seen = subtype;
        g_skills.page_wait_direction = 0;
        g_skills.page_before_request = -1;
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
    }

    skills_finish_page_landing(page);

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_SUBTYPE_PREV, INPUT_LAYER_OVERLAY)) {
        skills_change_subtype(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_SUBTYPE_NEXT, INPUT_LAYER_OVERLAY)) {
        skills_change_subtype(page, 1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        skills_move_vertical(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        skills_move_vertical(page, 1);
    }
    /* refactor26：D-Pad 左右专门翻页，角色切换统一改由左摇杆水平 50%。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        skills_request_page(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        skills_request_page(page, 1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        skills_confirm_current(page);
    }

    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
    InputRouter_Consume(INPUT_SUBTYPE_PREV);
    InputRouter_Consume(INPUT_SUBTYPE_NEXT);

    /* B、LB/RB 不消费：没有弹窗时继续交给 InterfaceShell；角色切换走左摇杆，不再走 D-Pad。 */
}

int InterfaceSkills_InstallHooks(void) {
    g_skills.enabled = 0;
    g_skills.was_active = 0;
    skills_reset_transient();

    if (!Runtime_InterfaceSkillsProtocolOk()) {
        Runtime_Log("[绝学页] 原版 state3 协议不匹配；本页 Adapter 已 fail-closed，其它已验收功能继续工作。");
        return 1;
    }
    if (!UiBridge_InstallInterfaceSkillsHooks()) {
        Runtime_Log("[绝学页] state3 ButtonEvent Hook 安装失败；拒绝启用本页 Adapter。");
        return 0;
    }

    g_skills.enabled = 1;
    Runtime_Log("[绝学页] state3 页面专属 Adapter 已启用：列表/分页/A/使用确认 + 治疗目标左右选择/A使用/B取消。");
    return 1;
}
