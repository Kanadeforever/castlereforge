#include "interface_equipment.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "ui_bridge.h"
#include "cursor.h"

/*
 * interface_equipment.c
 *
 * 这是主 Interface state4“及身/装备”页面的专属手柄适配器。
 *
 * 用户已经在旧沟通记录中明确了这一页的最终操作：
 *   X      = 兵刃 -> 护甲 -> 饰物 -> 再回兵刃，循环三个装备栏位；
 *   LT/RT  = 直接请求原版上一页 / 下一页；与上下越界共用同一套分页事务；
 *   ↑ / ↓  = 在当前栏位的候选装备中上下移动；
 *   A      = 装备当前选中的候选；
 *   ← / →  = 这一页没有占用时，继续由 InterfaceShell 切换角色；
 *   LB/RB  = 继续由 InterfaceShell 切换八大类；
 *   B      = 继续由 InterfaceShell 返回/退出。
 *
 * 最重要的安全原则：插件绝不直接写角色装备字段。
 * RPG.exe 的 state4 Update 在 0x42DEF0 已经有完整业务链：
 *   ButtonEvent -> 改当前栏位/当前行/页码 -> 0x42E160 -> 原版库存与角色装备事务 -> 刷新 UI。
 * 本模块只把手柄动作转换成“点击哪个原版真实 Button”，因此不会复制装备公式、库存返还或属性刷新。
 */

typedef struct InterfaceEquipmentState {
    /* 只有协议预检和 6 个 Hook 全部成功后才为 1；否则本模块完全静默。 */
    int enabled;

    /* 用来识别刚进入/离开 state4，只管理本模块自己的分页等待状态。 */
    int was_active;

    /*
     * 跨页不能直接写 page+0x590。
     * 我们先点击原版 Prev/Next，再等待 RPG.exe 真正修改当前页；
     * page_wait_direction=-1 表示等上一页，+1 表示等下一页。
     */
    int page_wait_direction;
    int page_before_request;

    /*
     * 记录“上一页/下一页事件”是在哪个 Runtime tick 排进去的。
     * 正常情况下 RPG.exe 很快就会改变 CURRENT_PAGE；但如果原版因为某个特殊状态拒绝了分页事件，
     * 插件绝不能无限期留在 page_wait_direction!=0 —— 否则 A/↑/↓ 会被我们自己一直挡住。
     * 这个时间戳只用于失败兜底，不参与正常页面动画。
     */
    u32 page_wait_start_tick;

    /* 栏位变化会让原版重新生成候选列表；检测到变化时必须丢掉旧分页事务。 */
    int last_slot_seen;
} InterfaceEquipmentState;

static InterfaceEquipmentState g_equipment;

/*
 * 安全取得全局主 Interface。
 * 这里每 tick 都重新读取，不缓存跨帧对象，避免关闭主菜单后拿着已经释放的旧指针。
 */
static u8* equipment_interface(void) {
    u8* i = *(u8**)GLOBAL_INTERFACE_UI;
    return Runtime_PtrOk(i) ? i : NULL;
}

/*
 * 只有主 Interface 当前 state==4、并且没有进入关闭生命周期时才返回装备页对象。
 * Interface+0x648 是 0x434DF1 构造链为 state4 保存的真实页面指针。
 */
static u8* equipment_page(void) {
    u8* i = equipment_interface();
    u8* page;

    if (!Runtime_PtrOk(i)) return NULL;
    if (*(i32*)(i + INTERFACE_STATE) != 4) return NULL;
    if (*(i32*)(i + INTERFACE_CLOSE_STATE) != 0) return NULL;

    page = *(u8**)(i + INTERFACE_PAGE_4);
    return Runtime_PtrOk(page) ? page : NULL;
}

/*
 * 镜像 0x431380 ButtonEvent 的两个最早可用性门。
 * +0x45==0 或 +0x04!=0 时，原版 ButtonEvent 自己就会返回 0；手柄也必须拒绝排事件。
 */
static int equipment_button_usable(void* button) {
    u8* b = (u8*)button;
    if (!Runtime_PtrOk(b)) return 0;
    if (*(u8*)(b + 0x45u) == 0) return 0;
    if (*(u8*)(b + 0x04u) != 0) return 0;
    return 1;
}

/* 取得三个栏位 Button 的父对象。 */
static u8* equipment_slot_panel(u8* page) {
    u8* panel;
    if (!Runtime_PtrOk(page)) return NULL;
    panel = *(u8**)(page + INTERFACE_EQUIPMENT_SLOT_PANEL);
    return Runtime_PtrOk(panel) ? panel : NULL;
}

/* 取得候选装备列表对象；9 行、Prev/Next 都在这个对象内。 */
static u8* equipment_list(u8* page) {
    u8* list;
    if (!Runtime_PtrOk(page)) return NULL;
    list = *(u8**)(page + INTERFACE_EQUIPMENT_LIST);
    return Runtime_PtrOk(list) ? list : NULL;
}

/*
 * 取得某个装备栏位的真实 Button。
 * 这里不根据中文名字判断“兵刃/护甲/饰物”，只使用原版内部 0/1/2 顺序；
 * 用户看到的循环顺序由原版三个按钮本身决定。
 */
static void* equipment_slot_button(u8* page, int slot) {
    u8* panel = equipment_slot_panel(page);
    void* button;

    if (!Runtime_PtrOk(panel)) return NULL;
    if (slot < 0 || slot >= INTERFACE_EQUIPMENT_SLOT_COUNT) return NULL;

    button = *(void**)(panel + INTERFACE_EQUIPMENT_SLOT_BUTTON0 + (u32)slot * 4u);
    return equipment_button_usable(button) ? button : NULL;
}

/*
 * 根据当前页与总候选数计算这一页实际有几行。
 * 原版 0x42DFFB 的索引公式是 row + page*9，因此 state4 每页固定最多 9 行。
 */
static int equipment_visible_row_count(u8* page) {
    int current_page;
    int total_count;
    int remain;

    if (!Runtime_PtrOk(page)) return 0;
    current_page = *(i32*)(page + INTERFACE_EQUIPMENT_CURRENT_PAGE);
    total_count = *(i32*)(page + INTERFACE_EQUIPMENT_TOTAL_COUNT);
    if (current_page < 0 || total_count <= 0) return 0;

    remain = total_count - current_page * INTERFACE_EQUIPMENT_ROW_COUNT;
    if (remain <= 0) return 0;
    return remain > INTERFACE_EQUIPMENT_ROW_COUNT ? INTERFACE_EQUIPMENT_ROW_COUNT : remain;
}

/* 取得第 row 行的真实候选 Button；超出当前页实际项目数时直接拒绝。 */
static void* equipment_row_button(u8* page, int row) {
    u8* list = equipment_list(page);
    void* button;
    int count;

    if (!Runtime_PtrOk(list)) return NULL;
    count = equipment_visible_row_count(page);
    if (row < 0 || row >= count) return NULL;

    button = *(void**)(list + INTERFACE_EQUIPMENT_ROW_BUTTON0 + (u32)row * 4u);
    return equipment_button_usable(button) ? button : NULL;
}

/*
 * 给现有 UiBridge 排一次属于 Interface 的原版 ButtonEvent。
 * 三道门按顺序检查：Button 可用 -> 没有别的模块事件 -> 取得手柄光标所有权。
 */
static int equipment_request_event(void* button, int code, const char* log_text) {
    if (!equipment_button_usable(button)) return 0;
    if (UiBridge_EventPending()) return 0;

    Cursor_ClaimForControllerNavigation();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, code, log_text);
    return UiBridge_EventOwner() == UI_EVENT_OWNER_INTERFACE;
}

/*
 * X 循环三个栏位。
 * 注意这里只“点击下一个栏位按钮”，绝不直接 page+0x5A0 = next；原版收到 code=2 后会自己：
 * 1. 改当前栏位；2. 清页码/行号；3. 重建候选列表；4. 刷新右侧装备信息。
 */
static void equipment_cycle_slot(u8* page) {
    int current;
    int step;
    int next;
    void* button = NULL;

    if (!Runtime_PtrOk(page) || UiBridge_EventPending()) return;

    current = *(i32*)(page + INTERFACE_EQUIPMENT_CURRENT_SLOT);
    if (current < 0 || current >= INTERFACE_EQUIPMENT_SLOT_COUNT) current = 0;

    /*
     * 正常情况下 next 就是 current+1。仍然最多尝试 3 次，是为了某个栏位按钮若临时被原版禁用时
     * 不把无效事件硬塞进去；找不到任何可用栏位就保持原样。
     */
    for (step = 1; step <= INTERFACE_EQUIPMENT_SLOT_COUNT; ++step) {
        next = (current + step) % INTERFACE_EQUIPMENT_SLOT_COUNT;
        button = equipment_slot_button(page, next);
        if (equipment_button_usable(button)) break;
        button = NULL;
    }

    if (!Runtime_PtrOk(button)) return;
    equipment_request_event(button, 2, "[及身页] X：循环到下一个原版装备栏位。");
}

/* 请求原版上一页/下一页；真正页码修改由 RPG.exe 完成。 */
static void equipment_request_page(u8* page, int direction, const char* request_log) {
    u8* list = equipment_list(page);
    int current_page;
    int total_pages;
    void* button;

    if (!Runtime_PtrOk(list) || direction == 0) return;
    if (g_equipment.page_wait_direction != 0 || UiBridge_EventPending()) return;

    current_page = *(i32*)(page + INTERFACE_EQUIPMENT_CURRENT_PAGE);
    total_pages = *(i32*)(page + INTERFACE_EQUIPMENT_TOTAL_PAGES);
    if (total_pages < 1) total_pages = 1;

    if (direction < 0) {
        if (current_page <= 0) return;
        button = *(void**)(list + INTERFACE_EQUIPMENT_PAGE_PREV_BUTTON);
    } else {
        if (current_page + 1 >= total_pages) return;
        button = *(void**)(list + INTERFACE_EQUIPMENT_PAGE_NEXT_BUTTON);
    }

    if (!equipment_button_usable(button)) return;

    g_equipment.page_wait_direction = direction < 0 ? -1 : 1;
    g_equipment.page_before_request = current_page;
    g_equipment.page_wait_start_tick = Runtime_Tick();
    if (!equipment_request_event(button, 2,
                                 request_log ? request_log :
                                 (direction < 0 ? "[及身页] 请求原版上一页。" : "[及身页] 请求原版下一页。"))) {
        /* 排队失败必须立即回滚本地等待，不能留下“永远等页码变化”的假事务。 */
        g_equipment.page_wait_direction = 0;
        g_equipment.page_before_request = -1;
        g_equipment.page_wait_start_tick = 0;
    }
}

/*
 * 等原版页码真正改变后，把手柄焦点落到符合连续导航直觉的边界行：
 * 下一页 -> 第 0 行；上一页 -> 新页最后一条真实候选。
 */
static void equipment_finish_page_landing(u8* page) {
    int current_page;
    int row_count;
    int target_row;
    void* button;

    if (!Runtime_PtrOk(page) || g_equipment.page_wait_direction == 0) return;

    current_page = *(i32*)(page + INTERFACE_EQUIPMENT_CURRENT_PAGE);
    if (current_page == g_equipment.page_before_request) {
        /*
         * 一秒只是“防止插件永远自锁”的保守保险丝，不是正常翻页动画时长。
         * 如果原版始终没有接受这次 Prev/Next，清掉的只有插件自己的等待与自己拥有的 pending Event；
         * 原版页码、当前行和装备数据一个字节都不碰。下一次玩家输入可以立即重试。
         */
        if (Runtime_Tick() - g_equipment.page_wait_start_tick >= Runtime_MsToTicks(1000u)) {
            g_equipment.page_wait_direction = 0;
            g_equipment.page_before_request = -1;
            g_equipment.page_wait_start_tick = 0;
            UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
            Runtime_Log("[及身页] 原版分页在 1 秒内没有改变页码；已只清理插件等待状态，避免手柄被永久锁住。");
        }
        return;
    }

    row_count = equipment_visible_row_count(page);
    if (row_count <= 0) {
        g_equipment.page_wait_direction = 0;
        g_equipment.page_before_request = -1;
        g_equipment.page_wait_start_tick = 0;
        return;
    }

    target_row = g_equipment.page_wait_direction > 0 ? 0 : row_count - 1;
    g_equipment.page_wait_direction = 0;
    g_equipment.page_before_request = -1;
    g_equipment.page_wait_start_tick = 0;

    button = equipment_row_button(page, target_row);
    if (equipment_button_usable(button)) {
        equipment_request_event(button, 2, "[及身页] 原版翻页完成：恢复边界候选焦点。");
    }
}

/* ↑/↓ 在当前 9 行候选内移动；越界时才请求原版翻页。 */
static void equipment_move_vertical(u8* page, int direction) {
    int current_row;
    int row_count;
    int target_row;
    void* button;

    if (!Runtime_PtrOk(page) || direction == 0) return;
    if (g_equipment.page_wait_direction != 0 || UiBridge_EventPending()) return;

    row_count = equipment_visible_row_count(page);
    if (row_count <= 0) return;

    current_row = *(i32*)(page + INTERFACE_EQUIPMENT_CURRENT_ROW);
    if (current_row < 0 || current_row >= row_count) current_row = 0;
    target_row = current_row + (direction < 0 ? -1 : 1);

    if (target_row < 0) {
        equipment_request_page(page, -1, "[及身页] ↑越界：请求原版上一页。");
        return;
    }
    if (target_row >= row_count) {
        equipment_request_page(page, 1, "[及身页] ↓越界：请求原版下一页。");
        return;
    }

    button = equipment_row_button(page, target_row);
    if (equipment_button_usable(button)) {
        equipment_request_event(button, 2, direction < 0 ?
                                "[及身页] ↑：选择上一件候选装备。" :
                                "[及身页] ↓：选择下一件候选装备。");
    }
}

/*
 * A 只点击当前原版选中行。
 * 0x42DFFB 后面的 RPG.exe 业务会自己判断：当前行是否真实存在、是否已经处于选中状态、
 * 是否需要执行 0x42E160 装备事务。插件不调用 0x42E160，也不改任何角色槽。
 */
static void equipment_confirm_current(u8* page) {
    int row;
    void* button;

    if (!Runtime_PtrOk(page) || g_equipment.page_wait_direction != 0 || UiBridge_EventPending()) return;

    row = *(i32*)(page + INTERFACE_EQUIPMENT_CURRENT_ROW);
    button = equipment_row_button(page, row);
    if (!equipment_button_usable(button)) {
        Runtime_Log("[及身页] A：当前候选不存在或原版不可用，本次确认已忽略。");
        return;
    }

    equipment_request_event(button, 2, "[及身页] A：执行当前候选装备的原版行事件。");
}

/* 清理的只有插件自己的等待状态；绝不重置原版 page/slot/row 字段。 */
static void equipment_reset_transient(void) {
    g_equipment.page_wait_direction = 0;
    g_equipment.page_before_request = -1;
    g_equipment.page_wait_start_tick = 0;
    g_equipment.last_slot_seen = -1;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
}

void InterfaceEquipment_Update(void) {
    u8* page;
    int slot;

    /* 协议不匹配时本模块彻底静默，其它已经验收的主 Interface 页面继续工作。 */
    if (!g_equipment.enabled) return;

    page = equipment_page();
    if (!Runtime_PtrOk(page)) {
        if (g_equipment.was_active) {
            equipment_reset_transient();
            g_equipment.was_active = 0;
            Runtime_Log("[及身页] 已离开 state4；页面专属等待状态已清理。");
        }
        return;
    }

    if (!g_equipment.was_active) {
        equipment_reset_transient();
        g_equipment.was_active = 1;
        g_equipment.last_slot_seen = *(i32*)(page + INTERFACE_EQUIPMENT_CURRENT_SLOT);
        Runtime_Log("[及身页] 已进入 state4；X栏位、上下候选、左右/LT/RT翻页与A装备 Adapter 开始工作。");
    }

    /* 栏位一旦被原版切换，旧页等待已经没有意义，必须只清插件事务，不碰原版新列表。 */
    slot = *(i32*)(page + INTERFACE_EQUIPMENT_CURRENT_SLOT);
    if (slot != g_equipment.last_slot_seen) {
        g_equipment.last_slot_seen = slot;
        g_equipment.page_wait_direction = 0;
        g_equipment.page_before_request = -1;
        g_equipment.page_wait_start_tick = 0;
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
    }

    equipment_finish_page_landing(page);

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_SPECIAL_X, INPUT_LAYER_OVERLAY)) {
        equipment_cycle_slot(page);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_NAV_UP, INPUT_LAYER_OVERLAY)) {
        equipment_move_vertical(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_NAV_DOWN, INPUT_LAYER_OVERLAY)) {
        equipment_move_vertical(page, 1);
    }

    /* refactor26：D-Pad ←/→ 也直接翻页；它和 LT/RT 共用完全相同的原版分页事务。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)) {
        equipment_request_page(page, -1, "[及身页] ←：请求原版上一页。");
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)) {
        equipment_request_page(page, 1, "[及身页] →：请求原版下一页。");
    }

    /*
     * LT/RT 继续保留“直接翻整页”，不会先把焦点硬移到页边界。
     * 两个快捷键与 ↑/↓ 越界严格调用同一个原版 Prev/Next ButtonEvent：
     * - 页码由 RPG.exe 自己改变；
     * - 插件只记录等待方向，并在页真正变化后落到新页边界行；
     * - 原版拒绝翻页时仍由同一个 1 秒保险丝只清理插件等待，不碰游戏状态。
     */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_SUBTYPE_PREV, INPUT_LAYER_OVERLAY)) {
        equipment_request_page(page, -1, "[及身页] LT：请求原版上一页。");
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_SUBTYPE_NEXT, INPUT_LAYER_OVERLAY)) {
        equipment_request_page(page, 1, "[及身页] RT：请求原版下一页。");
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        equipment_confirm_current(page);
    }

    /* 本页真正拥有的动作在这里消费，防止同一 tick 被下层/其它 Adapter 再读一次。 */
    InputRouter_Consume(INPUT_CONFIRM);
    InputRouter_Consume(INPUT_SPECIAL_X);
    InputRouter_Consume(INPUT_SPECIAL_Y);
    InputRouter_Consume(INPUT_NAV_UP);
    InputRouter_Consume(INPUT_NAV_DOWN);
    InputRouter_Consume(INPUT_NAV_LEFT);
    InputRouter_Consume(INPUT_NAV_RIGHT);
    InputRouter_Consume(INPUT_SUBTYPE_PREV);
    InputRouter_Consume(INPUT_SUBTYPE_NEXT);

    /* B、LB/RB 故意不消费：继续交给 InterfaceShell；角色切换已经迁到左摇杆水平 50%。 */
}

int InterfaceEquipment_InstallHooks(void) {
    g_equipment.enabled = 0;
    g_equipment.was_active = 0;
    equipment_reset_transient();

    if (!Runtime_InterfaceEquipmentProtocolOk()) {
        Runtime_Log("[及身页] 原版 state4 协议不匹配；本页 Adapter 已 fail-closed，其它已验收功能继续工作。");
        return 1;
    }
    if (!UiBridge_InstallInterfaceEquipmentHooks()) {
        Runtime_Log("[及身页] state4 ButtonEvent Hook 安装失败；拒绝启用本页 Adapter。");
        return 0;
    }

    g_equipment.enabled = 1;
    Runtime_Log("[及身页] state4 页面专属 Adapter 已启用：X循环栏位、上下候选/跨页、左右/LT/RT原版翻页、A装备。");
    return 1;
}
