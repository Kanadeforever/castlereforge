#include "ui_bridge.h"
#include "runtime.h"
#include "game_addresses.h"
#include "battle.h"
#include "save_slot.h"
#include "confirm_dialog.h"
#include "interface_items.h"
#include "interface_skills.h"
#include "interface_inner_stats.h"
#include "inn.h"

static volatile void* g_pending_button;
static volatile int g_pending_code;
static volatile UiEventOwner g_pending_owner;

/* 清空一次性按钮事件；Context 离开/鼠标接管时必须调用，防止旧事件落到新菜单。 */
void UiBridge_ClearEvent(void) {
    g_pending_button = NULL;
    g_pending_code = 0;
    g_pending_owner = UI_EVENT_OWNER_NONE;
}

/*
 * 只清理指定模块自己排进去的事件。
 * 例如 Battle 离开时只能清 BATTLE owner，绝不能再把 Interface/Save 的事件一起抹掉。
 */
void UiBridge_ClearEventOwned(UiEventOwner owner) {
    if (owner == UI_EVENT_OWNER_NONE) return;
    if (g_pending_code == 0 || g_pending_owner != owner) return;
    UiBridge_ClearEvent();
}

/* 给导航队列查询“是否还有原版 Event 未消费”，有的话就暂缓下一步。 */
int UiBridge_EventPending(void) {
    /*
     * state5 五内的按下→释放脉冲虽然不占 g_pending_button，但它同样是一条尚未完成的原版按钮事务。
     * 把它计入统一“忙”门，可以阻止 Shell 在 LT/RT 动画尚未释放时并发切角色/切大类，避免目标 Button 生命周期被换页打断。
     */
    return g_pending_code != 0 || InterfaceInnerStats_SyntheticEventPending();
}

UiEventOwner UiBridge_EventOwner(void) {
    return g_pending_code != 0 ? g_pending_owner : UI_EVENT_OWNER_NONE;
}

/*
 * 排一个“一次性原版 Button 返回码”。只允许已确认的 1/2，且一次只保留一个 pending。
 * owner 只是告诉生命周期清理代码“这是谁的事件”，不会被传给游戏。
 */
void UiBridge_RequestEventOwned(UiEventOwner owner, void* button, int code, const char* chinese_tag) {
    if (owner == UI_EVENT_OWNER_NONE) return;
    if (!Runtime_PtrOk(button)) return;
    if (code != 1 && code != 2) return;
    if (g_pending_code != 0) return;

    g_pending_button = button;
    g_pending_code = code;
    g_pending_owner = owner;
    Runtime_Log(chinese_tag ? chinese_tag : "[UI事件] 已排队一个原版按钮事件。");
}

/*
 * 这里只做“一次性返回值覆盖”。如果游戏真实鼠标已经让原函数返回非零，
 * 由于我们的 hook 是直接替换调用点，所以原函数会在没有 pending 时正常执行。
 */
static i32 FASTCALL UiBridge_HookButtonEvent(void* button, void* unused_edx) {
    PFN_ButtonEventThis orig = (PFN_ButtonEventThis)FN_BUTTON_EVENT;
    int code = g_pending_code;
    int inner_code;
    (void)unused_edx;

    /*
     * state5 五内的按钮反馈必须严格按原版“0x431400按下 -> 下一帧0x431380松开/确认”执行。
     * 这条页面专属短事务不占全局 UiBridge pending，先于普通 pending 查询，且只会命中当前五内目标 Button 一次。
     */
    inner_code = InterfaceInnerStats_TrySyntheticButtonEvent(button);
    if (inner_code != 0) return inner_code;

    if (code != 0 && button == (void*)g_pending_button) {
        g_pending_button = NULL;
        g_pending_code = 0;
        g_pending_owner = UI_EVENT_OWNER_NONE;
        Runtime_Log(code == 2 ? "[UI事件] 手柄确认事件已由原版按钮消费。" : "[UI事件] 手柄取消事件已由原版按钮消费。");
        return code;
    }

    /*
     * state5 五内的“手柄焦点鼠标”只是视觉牌，不是真鼠标业务输入。
     * 因为原版 ButtonEvent 0x431380 内部还会再次调用 HitTest，并维护 Button+0x28 的鼠标历史状态，
     * 所以只拦 0x431400 还不够：在手柄视觉焦点模式下，五内自己的 11 个 Button 还必须跳过
     * 这次“无 pending 的原版 ButtonEvent 扫描”。真正由手柄请求的 code=1/2 已在上面的 pending 分支先消费；
     * 实体鼠标/右摇杆接管后页面过滤器会返回 0，本函数立即恢复完整原版路径。
     */
    if (InterfaceInnerStats_FilterVisualOnlyButtonEvent(button)) return 0;

    return orig(button);
}

/*
 * 完整复刻 0x431310 在进入最终矩形命中函数 0x430D90 之前的坐标/尺寸计算。
 *
 * 为什么这个函数必须存在：
 * refactor23 的五内和阵形各自写了一份“Button 中心”代码，但那两份代码都犯了同一个错误：
 * 看到 Button+0x3C 或 +0x40 为 0，就直接认定“没有几何”。实际上原版恰恰把 0 当作合法值，
 * 意思是“不要使用覆盖尺寸，去 Sprite 当前 frame 里拿真实宽/高”。因此：
 * - 五内 X 已经改变逻辑 node，却因为算不出中心，根本没有把手形鼠标移动过去；
 * - 阵形构建 8 点集合时，只要其中一个按钮使用 Sprite 尺寸，就会整次 fail-closed，方向键看起来没有反应。
 *
 * 下面逐步照着原版做：
 * 1. Button+0x20/+0x24 是两个“屏幕基准坐标指针”，先解引用；
 * 2. 再加 Button+0x34/+0x38 的局部偏移，得到左上角；
 * 3. 如果 +0x3C/+0x40 非 0，原版直接把它当宽/高；
 * 4. 如果某一项为 0，只对那一项沿 Button+0x30 -> +0x35C -> +0x34 取 frame geometry；
 * 5. frame geometry+0x08 是宽，+0x0C 是高。
 *
 * 任何指针链不成立时返回 0。调用者此时只忽略这一次“手柄视觉/空间导航”，绝不写原版对象猜值。
 */
int UiBridge_GetButtonScreenRect(void* button, i32* out_left, i32* out_top, i32* out_right, i32* out_bottom) {
    u8* b = (u8*)button;
    i32* base_x;
    i32* base_y;
    i32 left;
    i32 top;
    i32 width;
    i32 height;

    if (!Runtime_PtrOk(b) || !out_left || !out_top || !out_right || !out_bottom) return 0;

    base_x = *(i32**)(b + BUTTON_BASE_X_PTR);
    base_y = *(i32**)(b + BUTTON_BASE_Y_PTR);
    if (!Runtime_PtrOk(base_x) || !Runtime_PtrOk(base_y)) return 0;

    left = *base_x + *(i32*)(b + BUTTON_LOCAL_X);
    top = *base_y + *(i32*)(b + BUTTON_LOCAL_Y);
    width = *(i32*)(b + BUTTON_WIDTH);
    height = *(i32*)(b + BUTTON_HEIGHT);

    /*
     * 只有需要 Sprite 回退时才走这条较深的指针链。
     * 这和原版一致：如果宽高 override 都非 0，就完全不要求 Button+0x30 的 Sprite 必须存在。
     */
    if (width == 0 || height == 0) {
        u8* sprite = *(u8**)(b + BUTTON_SPRITE_PTR);
        u8* frame_owner;
        u8* frame_geometry;

        if (!Runtime_PtrOk(sprite)) return 0;
        frame_owner = *(u8**)(sprite + BUTTON_SPRITE_FRAME_OWNER);
        if (!Runtime_PtrOk(frame_owner)) return 0;
        frame_geometry = *(u8**)(frame_owner + BUTTON_SPRITE_FRAME_GEOMETRY);
        if (!Runtime_PtrOk(frame_geometry)) return 0;

        if (width == 0) width = *(i32*)(frame_geometry + BUTTON_SPRITE_FRAME_WIDTH);
        if (height == 0) height = *(i32*)(frame_geometry + BUTTON_SPRITE_FRAME_HEIGHT);
    }

    /* 原版最终构造的是 left/right、top/bottom；非正尺寸对焦点算法没有意义，安全拒绝。 */
    if (width <= 0 || height <= 0) return 0;

    *out_left = left;
    *out_top = top;
    *out_right = left + width;
    *out_bottom = top + height;
    return 1;
}

/*
 * 页面通常只需要把“唯一原版手形鼠标”放到按钮中间，所以提供一个薄薄的中心点包装。
 * 所有尺寸回退仍由上面的唯一实现完成，避免未来某页面再次忘记 Sprite fallback。
 */
int UiBridge_GetButtonScreenCenter(void* button, i32* out_x, i32* out_y) {
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;

    if (!out_x || !out_y) return 0;
    if (!UiBridge_GetButtonScreenRect(button, &left, &top, &right, &bottom)) return 0;

    *out_x = left + (right - left) / 2;
    *out_y = top + (bottom - top) / 2;
    return 1;
}

/*
 * 一个统一 HitTest 分发点按优先级问 Battle、SaveSlot 是否要接管这个具体 Button。
 * 谁都不处理就调用原版，保证键鼠行为是默认路径。
 */
u8 FASTCALL UiBridge_HookButtonHitTest(void* button, void* unused_edx) {
    PFN_ButtonHitFast orig = (PFN_ButtonHitFast)FN_BUTTON_HITTEST;
    int handled = 0;
    u8 value;
    (void)unused_edx;

    /*
     * Battle 优先是硬性回归保护：即使其它 UI 恰巧复用了相同对象结构，
     * 只要战斗 Context 存在，就先让战斗自己的视觉契约决定结果。
     */
    value = Battle_FilterButtonHit(button, &handled);
    if (handled) return value;

    value = SaveSlot_FilterPopupHit(button, &handled);
    if (handled) return value;

    /* 主 Interface 的具体页面必须先于历史通用询问框决定自己的弹窗视觉。 */
    value = InterfaceItems_FilterPopupHit(button, &handled);
    if (handled) return value;

    value = InterfaceSkills_FilterPopupHit(button, &handled);
    if (handled) return value;

    /*
     * 客栈根层自己的 0x413909 HitTest CALL 也接到这个统一分发器。
     * Inn_FilterButtonHit 只有在客栈真的 active 且手柄已经取得导航焦点时才 handled=1；
     * 实体鼠标状态下会直接让出，因此不会把客栈三项强制锁在手柄焦点。
     */
    value = Inn_FilterButtonHit(button, &handled);
    if (handled) return value;

    /*
     * 只有 ConfirmDialog 自己登记过“真实 open 的父对象”时才可能 handled=1。
     * 因此标题/战斗后台按钮不会再像 refactor8 那样仅凭 HitTest 就抢走焦点。
     */
    value = ConfirmDialog_FilterHit(button, &handled);
    if (handled) return value;

    /*
     * 谁都没有主动覆盖时，先让原版做真实 HitTest，再把结果“旁听”给具体页面。
     * 旁听不会修改返回值，只用于解决“屏幕视觉焦点与页面 A 逻辑焦点不同步”的安全问题。
     */
    value = orig(button, NULL);
    InterfaceItems_ObservePopupHit(button, value);
    InterfaceSkills_ObservePopupHit(button, value);
    return value;
}

/*
 * 把 dev20 已确认的共享 ButtonEvent/HitTest CALL 一次性改到桥接器。
 * 业务模块不重复 patch 同一个 CALL，从结构上消除 Hook 覆盖顺序类回归。
 */
int UiBridge_InstallHooks(void) {
    /* 战斗各子菜单共用一个事件注入桥。 */
    if (!Runtime_PatchCall(CALL_CMD0_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD1_ROW_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD1_PAGE_PREV, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD1_PAGE_NEXT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD1_CATEGORY_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD1_CANCEL_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD2_ROW_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD2_CATEGORY_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD2_CANCEL_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD2_PAGE_PREV, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD2_PAGE_NEXT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_TOP_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CMD3_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CONFIRM_EVENT_YES, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CONFIRM_EVENT_NO, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_RESULT_OUTER_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_RESULT_DETAIL_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[致命] 通用 ButtonEvent 桥接 Hook 安装失败。");
        return 0;
    }

    /* 视觉 HitTest 也由一个统一分发器接住，但业务判断仍完全属于各自模块。 */
    if (!Runtime_PatchCall(CALL_TOP_VISUAL_HIT, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_CMD1_VISUAL_HIT_A, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_CMD1_VISUAL_HIT_B, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_CMD2_VISUAL_HIT, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_CONFIRM_HIT_YES, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_CONFIRM_HIT_NO, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST)) {
        Runtime_Log("[致命] 通用 ButtonHitTest 分发 Hook 安装失败。");
        return 0;
    }

    UiBridge_ClearEvent();
    return 1;
}


/*
 * 第二种同构双按钮 update（0x4276F0）不属于 refactor7 封版调用点。
 * ConfirmDialog_InstallHooks 会先做 8 个 open + 4 个 CALL 的独立预检；只有通过后才调用这里。
 */
int UiBridge_InstallConfirmDialogHooks(void) {
    if (!Runtime_PatchCall(CALL_CONFIRM2_HIT_YES, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_CONFIRM2_HIT_NO, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_CONFIRM2_EVENT_YES, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_CONFIRM2_EVENT_NO, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[询问框] 第二种双按钮 update 的 UI Bridge Hook 安装失败。");
        return 0;
    }
    return 1;
}


/*
 * 主 Interface 第一阶段只接“已经打开后的根导航” ButtonEvent，不碰 HitTest。
 * 八大类 8 个 + 退出 1 个 + 角色 1 个，共 10 个原版 CALL。
 * 地图 Y 从 refactor19 起不在 UiBridge 内处理：它由 Exploration 游戏线程安全点直接复用原版
 * Space 分支的 0x40B230(1) 业务事件。UiBridge这里只管“Interface 已经打开以后”的十个真实按钮 CALL。
 */
int UiBridge_InstallInterfaceShellHooks(void) {
    if (!Runtime_PatchCall(CALL_INTERFACE_CATEGORY_1, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_CATEGORY_2, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_CATEGORY_3, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_CATEGORY_4, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_CATEGORY_5, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_CATEGORY_6, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_CATEGORY_7, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_CATEGORY_8, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_EXIT_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_ROLE_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[主界面] Interface Shell 原版 ButtonEvent Hook 安装失败。");
        return 0;
    }
    return 1;
}


/*
 * state2“法宝/道具”页只把自己 6 个页面专属 ButtonEvent CALL 接入现有一次性事件桥。
 * 这些 CALL 已经由 Runtime_InterfaceItemsProtocolOk() 先整体预检；这里每一个 PatchCall 仍会再次核对原目标。
 */
int UiBridge_InstallInterfaceItemsHooks(void) {
    if (!Runtime_PatchCall(CALL_INTERFACE_ITEMS_SUBTYPE,   (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_ITEMS_SPECIAL_A, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_ITEMS_SPECIAL_B, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_ITEMS_PAGE_PREV, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_ITEMS_PAGE_NEXT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_ITEMS_ROW,       (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[法宝页] state2 的 6 个 ButtonEvent CALL Hook 安装失败。");
        return 0;
    }
    return 1;
}


/*
 * state3“绝学/法术”页共有 6 个需要手柄注入的原版 ButtonEvent CALL：
 * 前 4 个是 refactor21 起已经实机通过的子类型/分页/技能行；后 2 个是 refactor28 新闭合的治疗目标角色/取消。
 * 六处都只共享“把某个真实 Button 在原版 CALL 到来时返回一次指定 code”的低层桥，业务状态仍归 RPG.exe。
 */
int UiBridge_InstallInterfaceSkillsHooks(void) {
    if (!Runtime_PatchCall(CALL_INTERFACE_SKILLS_SUBTYPE,      (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_SKILLS_PAGE_PREV,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_SKILLS_PAGE_NEXT,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_SKILLS_ROW,          (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_SKILLS_TARGET_ROW,   (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_SKILLS_TARGET_CANCEL,(void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        /*
         * 后两处正是治疗法术原版鼠标选人链：角色透明 Button 的左键 code=2 与全屏取消 Button 的右键 code=1。
         * 都接入同一个 owner-aware pending 桥后，键鼠没有 pending 时仍完整调用原函数，不改变鼠标行为。
         */
        Runtime_Log("[绝学页] state3 的 6 个 ButtonEvent CALL Hook 安装失败。");
        return 0;
    }
    return 1;
}

/*
 * state4“及身/装备”页同样复用全局一次性 ButtonEvent 桥。
 * 六个 CALL 只是“接线点”；当前栏位、当前页、当前行和真正装备结果全部继续由原版 Update 修改。
 */
int UiBridge_InstallInterfaceEquipmentHooks(void) {
    if (!Runtime_PatchCall(CALL_INTERFACE_EQUIPMENT_PAGE_PREV, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_EQUIPMENT_PAGE_NEXT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_EQUIPMENT_ROW,       (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_EQUIPMENT_SLOT_0,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_EQUIPMENT_SLOT_1,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_EQUIPMENT_SLOT_2,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[及身页] state4 的 6 个 ButtonEvent CALL Hook 安装失败。");
        return 0;
    }
    return 1;
}

/*
 * state5“五内”页原版在一个循环 CALL 中依次扫描 11 个 Button。
 * 因此只需要把这一处接到 UiBridge；Adapter 排队时仍指定“目标 Button 指针”，
 * Hook 只有循环走到那个真实按钮时才返回一次 code=2，其余 10 个按钮继续执行原函数。
 */
int UiBridge_InstallInterfaceInnerStatsHooks(void) {
    if (!Runtime_PatchCall(CALL_INTERFACE_INNER_STATS_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[五内页] state5 的 ButtonEvent 循环 CALL Hook 安装失败。");
        return 0;
    }
    return 1;
}



/*
 * state6“阵形”页的 8 个阵位共享同一处循环 ButtonEvent CALL。
 * Adapter 只在指定真实 Button 轮到 Update 扫描时注入一次 code=1/2；
 * 来源选择、目标交换、角色映射刷新等业务全部继续由 0x43F9A0 原版状态机完成。
 */
int UiBridge_InstallInterfaceFormationHooks(void) {
    if (!Runtime_PatchCall(CALL_INTERFACE_FORMATION_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[阵形页] state6 的 8阵位 ButtonEvent 循环 CALL Hook 安装失败。");
        return 0;
    }
    return 1;
}


/*
 * state8“机能”页只把自己的五个 ButtonEvent CALL 接进统一一次性事件桥。
 * 这里绝不处理“音乐=几”或“空明流转=开/关”：那些都是 0x428950 原版状态机在收到 code=2 后自己修改。
 */
int UiBridge_InstallInterfaceOptionsHooks(void) {
    if (!Runtime_PatchCall(CALL_INTERFACE_OPTIONS_MUSIC_DEC, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_OPTIONS_MUSIC_INC, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_OPTIONS_SOUND_DEC, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_OPTIONS_SOUND_INC, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_INTERFACE_OPTIONS_KARMA,     (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[机能页] state8 的 5 个 ButtonEvent CALL Hook 安装失败。");
        return 0;
    }
    return 1;
}


/*
 * 客栈根层只有一处共享 HitTest 和一处共享 ButtonEvent。
 * 两处都先由 Runtime_InnProtocolOk() 整组验证；这里只负责真正改 CALL。
 */
int UiBridge_InstallInnHooks(void) {
    if (!Runtime_PatchCall(CALL_INN_BUTTON_HIT, (void*)UiBridge_HookButtonHitTest, FN_BUTTON_HITTEST) ||
        !Runtime_PatchCall(CALL_INN_BUTTON_EVENT, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[客栈] 根菜单 HitTest/ButtonEvent UI Bridge Hook 安装失败。");
        return 0;
    }
    return 1;
}

/*
 * 炼化两层所有业务按钮都使用同一个原版 0x431380 ButtonEvent。
 * 这里接入的 8 处分别覆盖顶部类别/退出、两层翻页、第二层取消和两层物品行。
 * 不接 HitTest：炼化列表的手形/高亮由原版当前选择状态负责，手柄只投递原版事件。
 */
int UiBridge_InstallSynthesisHooks(void) {
    if (!Runtime_PatchCall(CALL_SYNTHESIS_TOP_EVENT,        (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SYNTHESIS_PRIMARY_PREV,     (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SYNTHESIS_PRIMARY_NEXT,     (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SYNTHESIS_SECONDARY_PREV,   (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SYNTHESIS_SECONDARY_NEXT,   (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SYNTHESIS_SECONDARY_CANCEL, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SYNTHESIS_SECONDARY_ROW,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SYNTHESIS_PRIMARY_ROW,      (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[炼化] 两层菜单 8 个 ButtonEvent UI Bridge Hook 安装失败。");
        return 0;
    }
    return 1;
}

/*
 * 所有脚本店铺最终都进入同一个 0x413FA0 主对象；这里只需为这套统一菜单接一次线。
 * refactor33 的 11 个调用点完整覆盖顶部类别/退出、左右两列的行与翻页，以及数量窗确认/取消/增减。
 * Shop Adapter 只把一次性 code=1/2 送回这些原版 ButtonEvent；价格、库存、金钱和成交结果仍全由 RPG.exe 处理。
 */
int UiBridge_InstallShopHooks(void) {
    if (!Runtime_PatchCall(CALL_SHOP_TOP_EVENT,        (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_SELL_PAGE_PREV,   (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_SELL_PAGE_NEXT,   (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_BUY_PAGE_PREV,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_BUY_PAGE_NEXT,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_SELL_ROW_EVENT,   (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_BUY_ROW_EVENT,    (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_QUANTITY_CONFIRM, (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_QUANTITY_CANCEL,  (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_QUANTITY_INC,     (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT) ||
        !Runtime_PatchCall(CALL_SHOP_QUANTITY_DEC,     (void*)UiBridge_HookButtonEvent, FN_BUTTON_EVENT)) {
        Runtime_Log("[商店] refactor33 主体 11 个 ButtonEvent UI Bridge Hook 安装失败。");
        return 0;
    }
    return 1;
}

/* 信息窗关闭调用点与 CALL_CMD0_EVENT 同址，已由 UiBridge_InstallHooks 统一安装，禁止重复 Patch。 */
