#include "interface_inner_stats.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "ui_bridge.h"
#include "cursor.h"

/*
 * interface_inner_stats.c
 *
 * 这是主 Interface state5“五内”的页面专属手柄实现。
 *
 * 先说明原版真实结构，因为本模块所有按键都建立在这套结构上，而不是凭截图猜地址：
 *
 * 1. Interface+0x64C -> state5 页面对象；
 * 2. page+0x57C -> 五内按钮面板；
 * 3. panel+0x57C..+0x5A0 是 10 个真实 Button，按“减、加”成对排列；
 * 4. panel+0x5A4 是第 11 个真实 Button，也就是用户看到的“蕴魂/确认加点”；
 * 5. 0x440610 的原版 Update 会循环扫描这 11 个 Button，并在 0x4406A8 调 0x431380；
 * 6. 偶数 index 0/2/4/6/8 是减点，奇数 index 1/3/5/7/9 是加点；
 * 7. index 10 会把五个临时值真正提交回当前角色。
 *
 * 因此手柄完全不需要写 page 内存、更不需要直接写角色属性：
 *   LT = 给当前节点的“减”Button 重放一次原版按下→松开事务；
 *   RT = 给当前节点的“加”Button 重放一次原版按下→松开事务；
 *   Y  = 给 index 10 的“蕴魂”Button 重放同样的原版按钮事务。
 *
 * X 只改变“手柄现在关注哪个节点”这一份插件私有状态，不修改游戏数据。
 * 为了让玩家看得见这个焦点，插件先求该节点左右两个按钮的几何中心，再把原版手形鼠标沿右上约60°偏移到文字旁边，避免压住数字。
 */

typedef struct InterfaceInnerStatsState {
    /* 原版协议与 Hook 都通过以后才置 1；失败时本页面完全静默。 */
    int enabled;

    /* 用于识别进入/离开 state5，只管理本模块自己的焦点提示生命周期。 */
    int was_active;

    /*
     * 当前手柄聚焦的“原版内部节点 index”，范围 0..4。
     * 原版布局顺序是：0迅、1烈、2神、3魔、4魂。
     * 用户要求 X 循环顺序是：烈、神、魔、魂、迅，所以后面使用独立顺序表。
     */
    int node_index;

    /*
     * 1 表示当前焦点是由手柄建立的，应该显示原版鼠标图示；
     * 0 表示右摇杆/实体鼠标已经接管，此时不能每 tick 把鼠标强行吸回节点。
     */
    int controller_focus_visible;

    /*
     * state5 原版把一个鼠标按钮动作拆成两个游戏帧可观察阶段：
     * 1=等待在 0x431400 注入“按下边沿 code=2”；
     * 2=按下已经由原版消费，等待下一次同一 Button 的 0x431400 扫描；
     * 3=已经跨过至少一次新的鼠标边沿扫描，本帧允许 0x431380 返回 code=2 完成松开/确认。
     *
     * 这个两阶段脉冲最初用于解决 refactor25 的视觉残留；refactor26c 又补上一个更重要的事务约束：
     * 一旦 phase1 已经开始，就必须独立于手形鼠标视觉所有权把 phase2/phase3 走完。否则中途发生指针接管时，
     * 原版可能已经执行 0x43E6F0(1) 按下动画，却永远执行不到 0x43E6A0(0) 释放动画。
     */
    void* pulse_button;
    int pulse_phase;
    u32 pulse_start_tick;

    /*
     * refactor26d：真正的“按钮恢复普通态”不能再和 release 混为一谈。
     *
     * 反汇编已经确认：
     * - 0x43E6F0(1) 把 Button+0x30 指向的 Sprite 放到按下反馈；
     * - 0x43E6A0(0) 不是 idle，而是把 Sprite 切到状态 5；
     * - Sprite 状态 5 会在 0x43E770 的更新器里循环播放，不会自己回到普通静止态；
     * - Button 构造/初始化函数 0x4312C0 最后明确调用 0x43E6F0(0)，这才是原版 Button 的普通视觉基线。
     *
     * 因此 synthetic release 完成以后，只记录“下一帧需要恢复 idle 的同一个 Button”。
     * 真正恢复动作必须等 RPG.exe 下一次 state5 游戏线程扫描到这个 Button 时再执行，
     * 这样按下/释放反馈至少完整显示一个游戏帧，不会被 worker 线程提前抹掉。
     */
    void* idle_reset_button;
} InterfaceInnerStatsState;

static InterfaceInnerStatsState g_inner;

/* 用户确认的 X 循环：烈 -> 神 -> 魔 -> 魂 -> 迅 -> 烈。 */
static const int g_inner_cycle_order[INTERFACE_INNER_STATS_NODE_COUNT] = {
    INTERFACE_INNER_NODE_FIERCE,
    INTERFACE_INNER_NODE_DIVINE,
    INTERFACE_INNER_NODE_DEMON,
    INTERFACE_INNER_NODE_SOUL,
    INTERFACE_INNER_NODE_SWIFT
};

/*
 * 五内焦点手形鼠标的视觉偏移。
 *
 * 用户实机确认“节点中心”虽然已经能表达当前焦点，但会正好盖住五内数字。
 * 新规则要求从原来的节点中心沿“右上约 60°”移动一小段距离，让手形鼠标落在文字旁边：
 * - Windows 屏幕坐标中 X 向右为正；
 * - Y 向下为正，所以“向上”必须减 Y；
 * - 这里取约 42 像素长度，整数分量使用 +21 / -36，方向约为 60°。
 *
 * 这两个常量只影响视觉指示器坐标，不参与任何 Button HitTest、属性计算或原版 Event。
 */
#define INNER_FOCUS_OFFSET_X  21
#define INNER_FOCUS_OFFSET_Y -36

/*
 * 0x431380 / 0x431400 各自使用 Button 内两份“上一帧鼠标键状态”。
 * 真实鼠标点击时，RPG.exe 会维护这两个字段；手柄合成点击也必须逐帧同步，
 * 否则虽然外层能收到 code=2，Button 内部仍会留下不完整的按下/释放历史，最终表现成持续动画残留。
 */
#define INNER_BUTTON_RELEASE_HISTORY 0x28u
#define INNER_BUTTON_PRESS_HISTORY   0x2Cu

/* 每一帧重新取得主 Interface，不缓存可能在关闭菜单后失效的跨帧裸指针。 */
static u8* inner_interface(void) {
    u8* i = *(u8**)GLOBAL_INTERFACE_UI;
    return Runtime_PtrOk(i) ? i : NULL;
}

/* 只有 state5 活动且 Interface 没进入关闭生命周期时，才返回五内页对象。 */
static u8* inner_page(void) {
    u8* i = inner_interface();
    u8* page;

    if (!Runtime_PtrOk(i)) return NULL;
    if (*(i32*)(i + INTERFACE_STATE) != 5) return NULL;
    if (*(i32*)(i + INTERFACE_CLOSE_STATE) != 0) return NULL;

    page = *(u8**)(i + INTERFACE_PAGE_5);
    return Runtime_PtrOk(page) ? page : NULL;
}

/* page+0x57C 保存真正拥有 11 个 Button 的面板对象。 */
static u8* inner_panel(u8* page) {
    u8* panel;
    if (!Runtime_PtrOk(page)) return NULL;
    panel = *(u8**)(page + INTERFACE_INNER_STATS_PANEL);
    return Runtime_PtrOk(panel) ? panel : NULL;
}

/*
 * 与 0x431380 最前面的原版门保持一致。
 * Button+0x45==0 或 +0x04!=0 时，原版自己也会拒绝；手柄不能绕开这些禁用条件。
 */
static int inner_button_usable(void* button) {
    u8* b = (u8*)button;
    if (!Runtime_PtrOk(b)) return 0;
    if (*(u8*)(b + 0x45u) == 0) return 0;
    if (*(u8*)(b + 0x04u) != 0) return 0;
    return 1;
}

/* 根据 0..10 的真实原版 index 取得对应 Button 指针。 */
static void* inner_button(u8* page, int button_index) {
    u8* panel = inner_panel(page);
    void* button;

    if (!Runtime_PtrOk(panel)) return NULL;
    if (button_index < 0 || button_index >= INTERFACE_INNER_STATS_BUTTON_COUNT) return NULL;

    button = *(void**)(panel + INTERFACE_INNER_STATS_BUTTON0 + (u32)button_index * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/*
 * 读取一个原版 Button 的完整屏幕矩形，而不只拿中心点。
 *
 * 为什么 refactor23 必须知道“完整矩形”：
 * refactor22 把焦点鼠标放在“减/加两个按钮的中心之间”。这个坐标虽然看起来接近五内图标，
 * 但它仍可能落在真实 Button 的可点击范围内。RPG.exe 随后会把这只“仅用于显示的手柄鼠标”
 * 当成真正鼠标，触发 0x431400 的按键/动画状态。玩家连续按 X 后，多个历史节点便会留下闪烁动画。
 *
 * 所以新版的规则是：
 * 1. 先从原版 Button 自己的 x/y/width/height 算出真实矩形；
 * 2. 焦点鼠标的热点放到当前“减/加”组合框左边一点，明确离开真实点击区域；
 * 3. 同时还在 0x44066E 的专属鼠标边沿调用点做第二层保险，确保视觉鼠标永远不能产生业务点击。
 *
 * 这样“鼠标图标在哪里”和“哪个按钮真的被点击”成为两件完全独立的事。
 */
static int inner_button_rect(void* button, i32* out_left, i32* out_top, i32* out_right, i32* out_bottom) {
    /*
     * refactor24 不再自己解释 Button+0x3C/+0x40。
     * 这里直接调用 UI Bridge 对 0x431310 的完整复刻，因此即使原版把宽/高 override 留成 0，
     * 也会沿 Sprite frame 取得真正尺寸。这样 X 改 node 后一定有机会算出新的手形鼠标坐标。
     */
    return UiBridge_GetButtonScreenRect(button, out_left, out_top, out_right, out_bottom);
}
/*
 * 判断一个 Button 是否就是当前 state5 面板里的 11 个真实按钮之一。
 *
 * 这里每次都从当前 page 重新取指针，不缓存跨帧对象。主 Interface 关闭/切页时页面对象会销毁，
 * 如果把这些裸指针长期存在全局变量里，下一次打开菜单就可能拿到已经失效的旧地址。
 */
static int inner_is_owned_button(u8* page, void* candidate) {
    int index;
    if (!Runtime_PtrOk(page) || !Runtime_PtrOk(candidate)) return 0;
    for (index = 0; index < INTERFACE_INNER_STATS_BUTTON_COUNT; ++index) {
        if (inner_button(page, index) == candidate) return 1;
    }
    return 0;
}

/*
 * 把一个“当前仍被 state5 面板拥有”的 Button 鼠标历史恢复成完全松开。
 *
 * 这不是清游戏业务状态，只对应 0x431380/+0x28 与 0x431400/+0x2C 的两份鼠标键历史。
 * 之所以一定先做 owned 检查，是因为主 Interface 切页会销毁页面对象；插件绝不能拿缓存裸指针
 * 在页面已经离开后继续写。只要无法重新证明这个 Button 仍属于当前 state5，就什么都不做。
 */
static void inner_clear_button_mouse_history_if_owned(u8* page, void* button) {
    if (!Runtime_PtrOk(page) || !inner_is_owned_button(page, button)) return;
    *(i32*)((u8*)button + INNER_BUTTON_RELEASE_HISTORY) = 0;
    *(i32*)((u8*)button + INNER_BUTTON_PRESS_HISTORY) = 0;
}

/*
 * 把一个仍属于当前 state5 的真实 Button 恢复到 RPG.exe 自己的普通视觉状态。
 *
 * 这里刻意不直接写 Sprite+0x1C、+0x10 或任何动画帧字段。原因是那些字段属于 Sprite 内部状态机：
 * 如果插件只把某个数字硬改回去，很容易在不同资源帧范围、不同按钮皮肤或别的 EXE 变体上制造新的残留。
 *
 * 我们调用的 0x43E6F0(0) 不是猜出来的“清动画函数”，而是原版 Button 构造函数 0x4312C0
 * 创建 Button Sprite 后实际使用的初始视觉调用。也就是说，这一步等价于“让这个按钮回到原版刚初始化后的普通态”。
 *
 * 返回 1 表示本次确实完成了 idle 恢复；返回 0 表示页面/按钮/Sprite 已失效，此时只让调用者清插件私有指针，
 * 绝不能拿旧页面的悬空地址继续写或调用。
 */
static int inner_restore_button_idle_if_owned(u8* page, void* button) {
    typedef void (THISCALL *PFN_SpriteButtonFeedback)(void*, i32);
    PFN_SpriteButtonFeedback set_feedback = (PFN_SpriteButtonFeedback)FN_BUTTON_SPRITE_FEEDBACK;
    u8* sprite;

    if (!Runtime_PtrOk(page) || !inner_is_owned_button(page, button)) return 0;
    sprite = *(u8**)((u8*)button + BUTTON_SPRITE_PTR);
    if (!Runtime_PtrOk(sprite)) return 0;

    /* 参数 0 是 0x4312C0 初始化 Button 时使用的原版普通视觉索引。 */
    set_feedback(sprite, 0);
    inner_clear_button_mouse_history_if_owned(page, button);
    return 1;
}

/*
 * 找当前五内节点的“原版鼠标焦点坐标”。
 *
 * 用户给出的旧界面截图已经明确：这里要看到的焦点提示就是游戏自己的手形鼠标，
 * 而不是把某个五内图标本身做成一闪一闪的伪焦点。原版一个节点由“减”和“加”两个 Button
 * 夹着节点图示，因此最自然的视觉位置就是两个真实按钮矩形中心之间。
 *
 * refactor22 的问题不是这个坐标本身，而是合成鼠标处在 Button 几何范围附近时，
 * 原版 0x431400 / 0x431380 仍把它当成真正鼠标业务输入，导致按钮内部历史状态被逐个留下。
 * refactor23 起已在同一页面的两个原版调用点做“视觉鼠标隔离”；refactor24 又修正了 Button 宽高为 0 时必须走 Sprite frame 的几何回退，所以这里可以恢复到截图期望的
 * 节点中心作为逻辑锚点，再向右上偏移显示，同时不再让这个视觉光标触发/累积任何 Button 动画状态。
 */
static int inner_focus_anchor(u8* page, int node, i32* out_x, i32* out_y) {
    void* minus_button;
    void* plus_button;
    i32 ml, mt, mr, mb;
    i32 pl, pt, pr, pb;
    i32 minus_center_x;
    i32 minus_center_y;
    i32 plus_center_x;
    i32 plus_center_y;

    if (!Runtime_PtrOk(page) || !out_x || !out_y) return 0;
    if (node < 0 || node >= INTERFACE_INNER_STATS_NODE_COUNT) return 0;

    minus_button = inner_button(page, node * 2);
    plus_button = inner_button(page, node * 2 + 1);
    if (!inner_button_rect(minus_button, &ml, &mt, &mr, &mb)) return 0;
    if (!inner_button_rect(plus_button, &pl, &pt, &pr, &pb)) return 0;

    minus_center_x = ml + (mr - ml) / 2;
    minus_center_y = mt + (mb - mt) / 2;
    plus_center_x = pl + (pr - pl) / 2;
    plus_center_y = pt + (pb - pt) / 2;

    /*
     * 先得到“减/加按钮之间”的逻辑节点中心，再把可见手形鼠标移到文字右上方。
     * 这样 node_index 的逻辑焦点仍然精确指向当前五内，而鼠标图标不会压住数值文本。
     * 注意这里只改显示坐标；真正 LT/RT/Y 的业务目标仍由 inner_button() 直接取得。
     */
    *out_x = (minus_center_x + plus_center_x) / 2 + INNER_FOCUS_OFFSET_X;
    *out_y = (minus_center_y + plus_center_y) / 2 + INNER_FOCUS_OFFSET_Y;
    return 1;
}

/*
 * 显示唯一的“手柄五内焦点鼠标”。
 *
 * 注意：这里不再把鼠标放进任何真实 Button 的中心，也不调用任何 ButtonEvent。
 * node_index 是唯一逻辑焦点；原版鼠标图标只是把这个状态画给玩家看。
 * X 换节点时只移动同一只鼠标，因此屏幕上不应该再出现“历史节点一起闪”的累积视觉。
 */
static void inner_show_node_focus(u8* page) {
    i32 x;
    i32 y;
    int node = g_inner.node_index;

    if (!inner_focus_anchor(page, node, &x, &y)) return;

    Cursor_ClaimForControllerNavigation();
    Cursor_ShowMenuFocusAt(x, y);
    g_inner.controller_focus_visible = 1;
}

/*
 * 这是 refactor23 为五内新增的“视觉鼠标隔离器”。
 *
 * 原版 state5 Update 在 0x44066E 对 11 个按钮逐个调用 0x431400。
 * 0x431400 会综合：
 * - 鼠标当前是否位于按钮矩形；
 * - 左/右键当前与上一帧状态；
 * 最终返回 1/2 之类的鼠标按键边沿，并让 0x440610 后续启动按钮/图标动画。
 *
 * 当 controller_focus_visible==1 时，当前鼠标坐标是插件主动合成出来的“焦点图示”，
 * 绝不能让它参与真实鼠标业务。没有按钮 pulse 时，本页 11 个按钮因此只做视觉隔离；
 * 实体鼠标/右摇杆接管后就恢复原版。
 *
 * 但 refactor26c 进一步区分“视觉焦点”和“已经开始的按钮点击事务”：
 * 一旦 LT/RT/Y 已进入 press->release，事务必须先完成原版释放，不能因为焦点图示被隐藏而中途丢弃。
 * 这两个游戏帧结束后，真实鼠标路径立即恢复。
 */
i32 FASTCALL InterfaceInnerStats_HookMouseEdge(void* button, void* unused_edx) {
    typedef i32 (FASTCALL *PFN_ButtonMouseEdge)(void*, void*);
    PFN_ButtonMouseEdge orig = (PFN_ButtonMouseEdge)FN_BUTTON_MOUSE_EDGE;
    u8* page = inner_page();
    int owned;
    (void)unused_edx;

    owned = g_inner.enabled && Runtime_PtrOk(page) && inner_is_owned_button(page, button);

    /*
     * refactor26d：release 结束后的下一次 state5 游戏帧，先把同一个真实 Button 恢复为原版 idle。
     *
     * 为什么放在 0x44066E -> 0x431400 这个游戏线程调用点，而不是 InterfaceInnerStats_Update worker：
     * 1. synthetic release 是本帧稍后的 0x4406A8 -> 0x431380 才完成；
     * 2. 到下一次 0x431400 扫描同一 Button，天然就已经跨过一个完整游戏帧；
     * 3. 此时调用 0x43E6F0(0) 不会把刚刚那一下按键反馈提前吃掉；
     * 4. 所有 Sprite 操作仍发生在 RPG.exe 自己更新 Button 的游戏线程里，避免 worker 与渲染线程争抢动画状态。
     *
     * 如果用户极快地再次按 LT/RT，新 pulse 也允许在这一帧继续：先恢复旧点击的 idle，再在下面 phase1 分支开始新按下。
     */
    if (owned && g_inner.idle_reset_button == button) {
        if (inner_restore_button_idle_if_owned(page, button)) {
            Runtime_Log("[五内页] 上一次手柄点击的释放反馈已播放一帧；当前按钮已按原版0x43E6F0(0)恢复普通态。");
        }
        g_inner.idle_reset_button = NULL;
    }

    /*
     * refactor26c 的关键修正：
     * “一次已经开始的按钮点击事务”与“手形鼠标现在是否还作为焦点提示显示”是两件完全不同的事。
     *
     * refactor26b 把 pulse 的推进包在：
     *   controller_focus_visible && Cursor_ControllerOwnsPointer()
     * 里面。这样只要两帧之间发生右摇杆/实体鼠标接管，或者焦点提示因为别的原因被隐藏，
     * phase1/phase2 就可能永远不再前进。最危险的情况是 phase1 已经让原版执行了
     * 0x43E6F0(1)“按下动画”，但下一帧没有机会到 phase3，也就永远到不了外层
     * 0x4406B2 -> 0x43E6A0(0)“释放/恢复动画”。玩家看到的正是按钮一直闪/一直保持选择动画。
     *
     * 所以现在规则改成：
     * 1. 只要 pulse_phase!=0，说明这次 LT/RT/Y 点击已经开始；
     * 2. 在当前 state5 页面仍然拥有目标 Button 的前提下，这个短事务必须优先完成；
     * 3. 它不再依赖 controller_focus_visible，也不再依赖 Cursor_ControllerOwnsPointer()；
     * 4. 这两个游戏帧内，本页其它 10 个 Button 也暂时返回 0，防止实体鼠标恰好在同一瞬间制造第二个点击；
     * 5. pulse 完成后，实体鼠标/右摇杆立即恢复 100% 原版路径。
     *
     * 注意：这段只影响“点击反馈事务”。用户已经验收通过的手形鼠标位置、X 切换焦点和右上偏移
     * 完全由 inner_show_node_focus()/inner_focus_anchor() 管理，这里没有修改任何相关坐标。
     */
    if (owned && g_inner.pulse_phase != 0) {
        if (button == g_inner.pulse_button) {
            if (g_inner.pulse_phase == 1) {
                /*
                 * 第 1 个 state5 游戏帧：完整模拟真实鼠标“左键刚按下”。
                 * 原版 0x431400 返回 2 时，会把当前鼠标键掩码同步保存到 Button+0x2C。
                 * 外层 0x440610 看到 code=2 后会调用 0x43E6F0(1)，只让这个按钮短暂进入按下反馈。
                 */
                *(i32*)((u8*)button + INNER_BUTTON_PRESS_HISTORY) = 2;
                g_inner.pulse_phase = 2;
                return 2;
            }

            if (g_inner.pulse_phase == 2) {
                /*
                 * 第 2 个 state5 游戏帧：鼠标已经松开。
                 * 0x431400 本帧本来应返回 0，并把 +0x2C 从 2 更新成 0；
                 * 随后同一帧的 0x431380 才根据 +0x28 的“上一帧按下”历史产生 release code=2。
                 */
                *(i32*)((u8*)button + INNER_BUTTON_PRESS_HISTORY) = 0;
                g_inner.pulse_phase = 3;
                return 0;
            }

            /*
             * phase3 表示“释放事件已经准备好，只等同一帧后面的 0x431380 扫到这个 Button”。
             * 理论上 phase3 只存在极短时间；即使线程调度让它跨到下一次 0x431400，也必须保持松开状态，
             * 不能重新制造第二次按下。
             */
            *(i32*)((u8*)button + INNER_BUTTON_PRESS_HISTORY) = 0;
            return 0;
        }

        /*
         * 一次 pulse 只允许目标 Button 有反馈。本页其它 Button 在这两个游戏帧内全部保持“未按下”，
         * 这样即使用户同时移动实体鼠标，也不会让第二个按钮掺进这次手柄事务。
         */
        *(i32*)((u8*)button + INNER_BUTTON_PRESS_HISTORY) = 0;
        return 0;
    }

    /*
     * 没有业务 pulse 时，才应用“已经封版的五内手形鼠标仅作视觉指示”规则。
     * 这部分与 refactor26b 保持相同语义：当焦点鼠标由手柄持有时，11 个真实 Button 不允许把
     * 那只合成鼠标当成真实点击；一旦实体鼠标/右摇杆接管，立即完整调用原版 0x431400。
     */
    if (owned && g_inner.controller_focus_visible && Cursor_ControllerOwnsPointer()) {
        *(i32*)((u8*)button + INNER_BUTTON_PRESS_HISTORY) = 0;
        return 0;
    }

    return orig(button, NULL);
}

/*
 * UiBridge 的 ButtonEvent Hook 会在“没有手柄 pending”时询问这个过滤器。
 *
 * 为什么还要有第二层过滤：
 * 0x431380 并不是一个纯“读取事件”函数，它内部自己又会调用 0x431310 HitTest，并更新 Button+0x28
 * 的上一帧鼠标状态。只拦 0x431400 只能阻止鼠标按键边沿，却仍可能让合成焦点在多个节点之间移动时
 * 给这些按钮留下 hover/历史状态。用户实机看到“X 绕一圈后所有图标都闪”正是这种状态污染的表现。
 *
 * 返回 1 的条件非常窄：
 * - state5 Adapter 已启用；
 * - 当前确实在 state5；
 * - 当前显示的是插件合成的手柄焦点鼠标；
 * - 光标所有权仍属于手柄；
 * - 被扫描的正是本页 11 个真实 Button 之一。
 *
 * 手柄真正要执行减/加/蕴魂时，UiBridge 会先在 pending 分支返回 code=2，因此不会被这里挡掉。
 * 一旦实体鼠标或右摇杆接管，controller_focus_visible 会清 0，真实鼠标路径立刻 100% 恢复原版。
 */
/*
 * state5 的合成“松开/确认”阶段。
 * UiBridge 会在全局 pending 之前调用这里；只有 pulse_phase==3 且 Button 完全匹配时返回一次 code=2。
 * 返回后立刻清空 pulse，所以下一帧又恢复纯视觉隔离，不可能让按钮永久保持按下动画。
 */
int InterfaceInnerStats_SyntheticEventPending(void) {
    return g_inner.pulse_phase != 0;
}

int InterfaceInnerStats_TrySyntheticButtonEvent(void* button) {
    u8* page = inner_page();

    if (!g_inner.enabled || g_inner.pulse_phase != 3) return 0;
    if (!Runtime_PtrOk(page) || !inner_is_owned_button(page, button)) return 0;
    if (button != g_inner.pulse_button) return 0;

    /*
     * 真实 0x431380 在检测到“上一帧 bit2=1、本帧 bit2=0”并返回 code=2 时，
     * 会把 Button+0x28 同步写成当前的 0。这里必须做同一件事，否则 release history 会残留。
     */
    *(i32*)((u8*)button + INNER_BUTTON_RELEASE_HISTORY) = 0;

    /*
     * 这里只完成“释放业务/释放反馈”，不能把它误当成“视觉已经恢复 idle”。
     * 0x4406B2 在本函数返回 code=2 后还会调用 0x43E6A0(0)，把 Sprite 切到会循环的 release 状态 5。
     * 因此先记住同一个 Button；下一帧它再次经过 0x44066E 时再调用原版 0x43E6F0(0) 收回普通态。
     */
    g_inner.idle_reset_button = button;

    g_inner.pulse_button = NULL;
    g_inner.pulse_phase = 0;
    g_inner.pulse_start_tick = 0;
    Runtime_Log("[五内页] 原版按钮脉冲已完成：按下/释放历史均已回到鼠标松开状态。");
    return 2;
}

int InterfaceInnerStats_FilterVisualOnlyButtonEvent(void* button) {
    u8* page = inner_page();
    int owned;

    if (!g_inner.enabled) return 0;
    if (!Runtime_PtrOk(page)) return 0;

    owned = inner_is_owned_button(page, button);
    if (!owned) return 0;

    /*
     * 与 HookMouseEdge 相同：已经开始的 press->release 事务必须优先于鼠标视觉所有权。
     *
     * 原版 0x431380 会维护 Button+0x28：
     * - “上一帧左键按着”时保存 2；
     * - “本帧松开”时保存 0，并返回 release code=2。
     *
     * phase2 正好处在“第1帧刚按下、同一帧尚未松开”的位置，因此目标 Button 的 +0x28 必须写 2；
     * 其它 Button 必须写 0。这样下一游戏帧 phase2->phase3 后，UiBridge 前面的
     * InterfaceInnerStats_TrySyntheticButtonEvent() 才会返回一次 code=2，让原版外层真正调用
     * 0x43E6A0(0) 收尾动画和业务，而不是只清插件自己的变量。
     */
    if (g_inner.pulse_phase != 0) {
        if (button == g_inner.pulse_button && g_inner.pulse_phase == 2) {
            *(i32*)((u8*)button + INNER_BUTTON_RELEASE_HISTORY) = 2;
        } else {
            *(i32*)((u8*)button + INNER_BUTTON_RELEASE_HISTORY) = 0;
        }
        return 1;
    }

    /*
     * 没有点击事务时，继续保持 refactor26b 已验收的“手形鼠标只显示焦点、不参与 ButtonEvent”。
     * 这就是防止 X 多次切换后历史节点全部闪烁的第二层隔离。
     */
    if (!g_inner.controller_focus_visible) return 0;
    if (!Cursor_ControllerOwnsPointer()) return 0;

    *(i32*)((u8*)button + INNER_BUTTON_RELEASE_HISTORY) = 0;
    return 1;
}

/*
 * 把当前 node_index 转成 g_inner_cycle_order 中的位置。
 * node_index 是原版布局 index，而 X 的循环顺序是用户层顺序，两者不能混为一谈。
 */
static int inner_cycle_position(int node_index) {
    int pos;
    for (pos = 0; pos < INTERFACE_INNER_STATS_NODE_COUNT; ++pos) {
        if (g_inner_cycle_order[pos] == node_index) return pos;
    }
    return 0;
}

/* X：只切插件焦点，不点击任何游戏按钮，也不改变五内数值。 */
static void inner_cycle_focus(u8* page) {
    int pos = inner_cycle_position(g_inner.node_index);
    pos = (pos + 1) % INTERFACE_INNER_STATS_NODE_COUNT;
    g_inner.node_index = g_inner_cycle_order[pos];
    inner_show_node_focus(page);
    Runtime_Log("[五内页] X：焦点已循环到下一个五内节点。");
}

/*
 * 开始一次严格复刻原版鼠标点击节奏的“按下→松开”脉冲。
 * 这里只保存目标 Button，不直接写 Sprite/动画字段，也不直接改五内数值。
 * 真正的两个阶段都在 RPG.exe 原来的 state5 游戏线程扫描里发生。
 */
static int inner_begin_button_pulse(void* button, const char* chinese_tag) {
    u8* page = inner_page();

    if (!inner_button_usable(button)) return 0;
    if (UiBridge_EventPending() || g_inner.pulse_phase != 0) return 0;

    /*
     * 一次新的合成点击必须从“鼠标完全松开”的原版历史开始。
     * 真实鼠标在用户没有按键时这两个字段也会被原版持续归零；先标准化基线可以避免旧版/异常中断留下的 1/2 位影响本次边沿。
     */
    inner_clear_button_mouse_history_if_owned(page, button);

    g_inner.pulse_button = button;
    g_inner.pulse_phase = 1;
    g_inner.pulse_start_tick = Runtime_Tick();
    Runtime_Log(chinese_tag ? chinese_tag : "[五内页] 已开始原版按钮按下/释放脉冲。");
    return 1;
}

/*
 * 给当前五内节点的“减/加”真实 Button 排一次 Event。
 * direction<0 选偶数 index（减），direction>0 选奇数 index（加）。
 * refactor26 继续保持 LT=减、RT=加；本轮只补完整的原版按下→释放反馈链；
 * 原版 Button 索引、Event code 和真正数值事务完全不变。
 */
static void inner_adjust_current(u8* page, int direction) {
    int button_index;
    void* button;

    if (!Runtime_PtrOk(page) || direction == 0) return;
    if (UiBridge_EventPending()) return;
    if (g_inner.node_index < 0 || g_inner.node_index >= INTERFACE_INNER_STATS_NODE_COUNT) return;

    button_index = g_inner.node_index * 2 + (direction > 0 ? 1 : 0);
    button = inner_button(page, button_index);
    if (!inner_button_usable(button)) {
        Runtime_Log("[五内页] 当前节点的原版加减按钮不可用；本次 LT/RT 已忽略。");
        return;
    }

    /* 手柄重新产生业务输入时，立即从右摇杆/实体鼠标状态切回可见节点焦点。 */
    inner_show_node_focus(page);
    inner_begin_button_pulse(button,
        direction < 0 ? "[五内页] LT：开始当前节点减点的原版按下/释放脉冲。" :
                        "[五内页] RT：开始当前节点加点的原版按下/释放脉冲。");
}

/* Y：点击第 11 个真实按钮，让 RPG.exe 自己完成“蕴魂/提交加点”业务。 */
static void inner_commit(u8* page) {
    void* button;

    if (!Runtime_PtrOk(page) || UiBridge_EventPending()) return;

    button = inner_button(page, INTERFACE_INNER_STATS_COMMIT_INDEX);
    if (!inner_button_usable(button)) {
        Runtime_Log("[五内页] Y：原版蕴魂按钮当前不可用；本次提交已忽略。");
        return;
    }

    inner_show_node_focus(page);
    inner_begin_button_pulse(button, "[五内页] Y：开始原版蕴魂按钮的按下/释放脉冲。");
}

/* 离开页面时只清插件自己的焦点和待处理 Interface 事件，不写任何原版五内字段。 */
static void inner_reset_transient(void) {
    g_inner.node_index = INTERFACE_INNER_NODE_FIERCE;
    g_inner.controller_focus_visible = 0;
    g_inner.pulse_button = NULL;
    g_inner.pulse_phase = 0;
    g_inner.pulse_start_tick = 0;
    g_inner.idle_reset_button = NULL;
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
    Cursor_HideMenuFocusImmediately();
}

void InterfaceInnerStats_OnPointerTakeover(CursorTakeoverEvent event) {
    if (event == CURSOR_TAKEOVER_NONE) return;
    if (!g_inner.was_active) return;

    /*
     * 用户主动移动右摇杆鼠标/实体鼠标后，真正鼠标位置必须立即交还给用户。
     * 因此“手形鼠标焦点提示”仍然和 refactor26b 一样马上隐藏；node_index 也不改变，
     * 下一次 X/LT/RT/Y 手柄输入会自然恢复已封版的焦点位置。
     *
     * 但这里绝对不能再把 pulse_button/pulse_phase 一起清掉。
     * 一次 LT/RT/Y 如果已经在原版 state5 游戏线程执行过 press，就必须让下一帧 release 走完。
     * refactor26b 在这里直接取消 pulse，正是“按钮按下动画开始了，但释放动画永远没有机会执行”的
     * 主要中断路径之一。
     *
     * pulse 最多只占用两个 state5 游戏帧；在它完成以前，HookMouseEdge/FilterVisualOnlyButtonEvent
     * 会短暂吞掉本页其它 Button 的鼠标边沿，完成后真实鼠标立刻恢复原版，所以不会长期妨碍用户接管。
     */
    g_inner.controller_focus_visible = 0;
    Cursor_HideMenuFocusImmediately();

    if (g_inner.pulse_phase != 0) {
        Runtime_Log("[五内页] 指针已被实体鼠标/右摇杆接管；已隐藏焦点提示，但当前按钮press/release事务继续完成，不再中途取消。");
    }
}

void InterfaceInnerStats_Update(void) {
    u8* page;

    if (!g_inner.enabled) return;

    page = inner_page();
    if (!Runtime_PtrOk(page)) {
        if (g_inner.was_active) {
            inner_reset_transient();
            g_inner.was_active = 0;
            Runtime_Log("[五内页] 已离开 state5；手柄节点焦点与菜单鼠标提示已清理。");
        }
        return;
    }

    /*
     * 正常情况下 press->release 只跨两个 state5 游戏帧，所以 500ms 已经远远超过正常时间。
     * refactor26b 的超时策略是“直接把 pulse 清掉”，这在 phase2 时非常危险：
     * 原版 0x43E6F0(1) 的按下动画可能已经执行，但 0x43E6A0(0) 的释放动画还没执行；
     * 直接丢事务只会让视觉永远停在按下态。
     *
     * 新策略按 phase 区分：
     * - phase1：连按下都还没被原版扫描到，没有可见按下动画，可以安全取消；
     * - phase2：按下已经发生，强制推进到 phase3，并把两份鼠标历史整理成“等待释放”；
     * - phase3：已经在等待 0x431380 消费，不再丢事务，只刷新 watchdog 起点继续等待原版游戏线程收尾。
     *
     * 只要当前 page 还能重新证明 pulse_button 确实属于 state5，就绝不拿悬空地址写内存。
     */
    if (g_inner.pulse_phase != 0 &&
        Runtime_Tick() - g_inner.pulse_start_tick >= Runtime_MsToTicks(500u)) {
        if (!inner_is_owned_button(page, g_inner.pulse_button)) {
            /* 页面/按钮已经不再属于当前 state5：此时可见页面也已经切走，只清插件私有事务。 */
            g_inner.pulse_button = NULL;
            g_inner.pulse_phase = 0;
            g_inner.pulse_start_tick = 0;
            Runtime_Log("[五内页] 按钮事务超时时目标已不属于当前 state5；仅清插件私有状态，不写旧对象。");
        } else if (g_inner.pulse_phase == 1) {
            /* 尚未执行原版按下动画，取消不会留下视觉半事务。 */
            inner_clear_button_mouse_history_if_owned(page, g_inner.pulse_button);
            g_inner.pulse_button = NULL;
            g_inner.pulse_phase = 0;
            g_inner.pulse_start_tick = 0;
            Runtime_Log("[五内页] 按钮事务在真正按下前超时；已安全取消，不存在待释放动画。");
        } else {
            /*
             * phase2/3 都说明按下阶段已经发生或至少已经跨过按下帧。
             * 把 +0x2C 设为 0、+0x28 设为 2，等价于告诉下一次 0x431380：
             * “上一帧左键是按下，本帧已经松开”。然后强制保持 phase3，直到游戏线程真正返回 code=2。
             */
            *(i32*)((u8*)g_inner.pulse_button + INNER_BUTTON_PRESS_HISTORY) = 0;
            *(i32*)((u8*)g_inner.pulse_button + INNER_BUTTON_RELEASE_HISTORY) = 2;
            g_inner.pulse_phase = 3;
            g_inner.pulse_start_tick = Runtime_Tick();
            Runtime_Log("[五内页] 按钮事务超过500ms仍未收尾；已强制整理为release阶段，等待原版0x431380执行恢复动画。");
        }
    }

    if (!g_inner.was_active) {
        inner_reset_transient();
        g_inner.was_active = 1;
        g_inner.node_index = INTERFACE_INNER_NODE_FIERCE;

        /*
         * 进入页面本身不等于“用户正在用手柄”。
         * 如果玩家是用实体鼠标点顶部“五内”标签进入，这里绝不能突然抢走系统鼠标并 warp 到“烈”。
         * r19 Shell 的 LB/RB 是手柄动作，执行时已经调用 Cursor_ClaimForControllerNavigation()，
         * 所以通过手柄切进 state5 时 controller owner 一定成立，仍会立即显示默认“烈”焦点。
         * 通过鼠标进入时则保持原版鼠标自由；等第一次 X/LT/RT/Y 手柄输入出现后再显示节点焦点。
         */
        if (Cursor_ControllerOwnsPointer()) inner_show_node_focus(page);
        Runtime_Log("[五内页] 已进入 state5；X循环、LT减、RT加、Y蕴魂；原版按钮反馈按“按下→释放→下一游戏帧恢复idle”三段原版视觉链重放；角色切换统一使用左摇杆50%。");
    }

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_INNER_STATS, INPUT_SPECIAL_X, INPUT_LAYER_OVERLAY)) {
        inner_cycle_focus(page);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_INNER_STATS, INPUT_SUBTYPE_PREV, INPUT_LAYER_OVERLAY)) {
        inner_adjust_current(page, -1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_INNER_STATS, INPUT_SUBTYPE_NEXT, INPUT_LAYER_OVERLAY)) {
        inner_adjust_current(page, +1);
    }
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_INNER_STATS, INPUT_SPECIAL_Y, INPUT_LAYER_OVERLAY)) {
        inner_commit(page);
    }

    /*
     * state5 现在只拥有 X/Y/LT/RT：
     * - LT/RT 已在本页完成减/加，所以必须消费；
     * - D-Pad 四方向当前没有五内业务，全部吞掉；角色切换已统一迁到左摇杆水平 50%；
     * - A 当前没有五内业务，继续吞掉；
     * - B 与 LB/RB 不消费，继续由 Shell 负责返回和切大类。
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

int InterfaceInnerStats_InstallHooks(void) {
    g_inner.enabled = 0;
    g_inner.was_active = 0;
    inner_reset_transient();

    if (!Runtime_InterfaceInnerStatsProtocolOk()) {
        Runtime_Log("[五内页] 原版 state5 协议不匹配；本页 Adapter 已 fail-closed，其它主 Interface 页面继续工作。");
        return 1;
    }
    /*
     * 先接住 state5 自己的鼠标边沿调用点。这个 CALL 不属于通用 UiBridge，
     * 因为“把合成焦点鼠标与真实鼠标业务隔离”只对五内页面成立。
     */
    if (!Runtime_PatchCall(CALL_INTERFACE_INNER_STATS_MOUSE,
                           (void*)InterfaceInnerStats_HookMouseEdge, FN_BUTTON_MOUSE_EDGE)) {
        Runtime_Log("[五内页] state5 合成焦点鼠标隔离 Hook 安装失败；拒绝启用本页 Adapter。");
        return 0;
    }
    if (!UiBridge_InstallInterfaceInnerStatsHooks()) {
        Runtime_Log("[五内页] state5 ButtonEvent Hook 安装失败；拒绝启用本页 Adapter。");
        return 0;
    }

    g_inner.enabled = 1;
    Runtime_Log("[五内页] state5 Adapter 已启用：X循环节点、LT减、RT加、Y蕴魂；按钮反馈按原版按下→释放→下一游戏帧idle恢复；D-Pad不换人，角色切换统一用左摇杆50%。");
    return 1;
}
