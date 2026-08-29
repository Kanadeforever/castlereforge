#include "interface_shell.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "pad_input.h"
#include "cursor.h"
#include "ui_bridge.h"
#include "battle.h"
#include "frontend.h"
#include "interface_tome.h"
#include "interface_skills.h"

/*
 * interface_shell.c
 *
 * 这一文件只负责《幽城幻剑录》游戏内主 Interface 的“最外层外壳”。
 * 这里故意不提前实现法宝、绝学、及身、五内、阵形、天书、机能这些页面内部业务。
 *
 * 为什么一定要这样拆：
 * - refactor12 已经由实际研究证明，不同窗口即使看起来都是“两个按钮/一个列表”，内部协议也可能完全不同；
 * - 如果先造一个万能菜单控制器，再不停往里面加特例，最后很容易再次误吞 Battle、Title、SaveSlot 等其它 Context；
 * - 所以 Shell 只做原版本身确实统一存在的动作：普通探索 Y、LB/RB 切八大类、左摇杆水平 50% 切角色、根层 B 退出。
 *
 * 可以把 Interface 想成一个“总文件夹”：
 * - 本文件只负责在八个大文件夹之间切换、切换角色、关闭总文件夹；
 * - 每个文件夹内部以后各自写 adapter；
 * - 等所有页面都实机通过以后，再看哪些代码真的相同，届时才抽公共函数。
 */
typedef enum InterfaceShellAction {
    INTERFACE_SHELL_ACTION_NONE = 0,
    INTERFACE_SHELL_ACTION_EXIT,
    INTERFACE_SHELL_ACTION_CATEGORY_PREV,
    INTERFACE_SHELL_ACTION_CATEGORY_NEXT,
    INTERFACE_SHELL_ACTION_ROLE_PREV,
    INTERFACE_SHELL_ACTION_ROLE_NEXT
} InterfaceShellAction;

#define INTERFACE_SHELL_QUEUE_CAPACITY 8

typedef struct InterfaceShellState {
    /*
     * 地图 Y 不再猜 Interface ctor 前面的状态机门控。
     * worker 只把“一次 Y 新按下”保存成 pending；真正执行发生在 Exploration 已有的游戏线程 Hook 内。
     * 这样既不会从 worker 线程直接改游戏状态，也不会要求 Windows 收到一个假的 Space。
     */
    volatile int map_y_pending;

    /*
     * 主 Interface 自己有 +0x590>8 的动画门。原版鼠标设计允许这段时间直接忽略新点击，
     * 但手柄连续点按时这种“按了却没反应”会显得非常钝。
     * 因此这里只缓存最多 8 个“用户已经明确按下”的 Shell 动作；等原版动画门重新允许输入时再按顺序提交。
     * 队列只保存动作语义，不保存 Button 指针，避免换页后旧指针落到新页面。
     */
    InterfaceShellAction action_queue[INTERFACE_SHELL_QUEUE_CAPACITY];
    int action_queue_head;
    int action_queue_tail;
    int action_queue_count;

    /* 只用于“Interface 刚出现/刚消失”的状态变化日志。 */
    int interface_seen;
} InterfaceShellState;

static InterfaceShellState g_interface_shell;

/* 清空所有尚未提交给原版的 Shell 动作；离开 Interface 时必须调用。 */
static void interface_shell_clear_action_queue(void) {
    int i;

    for (i = 0; i < INTERFACE_SHELL_QUEUE_CAPACITY; ++i) {
        g_interface_shell.action_queue[i] = INTERFACE_SHELL_ACTION_NONE;
    }
    g_interface_shell.action_queue_head = 0;
    g_interface_shell.action_queue_tail = 0;
    g_interface_shell.action_queue_count = 0;
}

/*
 * 只删除动画缓冲队列里的 ROLE_PREV / ROLE_NEXT，保留退出与大类切换。
 *
 * 为什么 refactor24 需要这层保险：
 * Shell 当前只允许 state1..5 新捕获角色切换；但旧动作仍可能是在上一页面动画尚未完成时排进队列。
 * 正常情况下 dispatch 会再次检查当前 state 并拒绝它；然而阵形实机曾出现“页面导航期间仍执行角色切换”的严重回归，
 * 所以页面取得方向键所有权时，不再依赖后续拒绝，而是把这些旧角色动作从源头清掉。
 *
 * 算法很朴素：把当前队列按原顺序读一遍，把不是角色切换的动作临时保存，再重新排回去。
 * 队列最多 8 项，因此不需要动态内存，也不会在 2001 年游戏进程里引入复杂容器。
 */
void InterfaceShell_DiscardQueuedRoleActions(void) {
    InterfaceShellAction kept[INTERFACE_SHELL_QUEUE_CAPACITY];
    int kept_count = 0;
    int original_count = g_interface_shell.action_queue_count;
    int i;

    for (i = 0; i < original_count; ++i) {
        int index = (g_interface_shell.action_queue_head + i) % INTERFACE_SHELL_QUEUE_CAPACITY;
        InterfaceShellAction action = g_interface_shell.action_queue[index];

        if (action == INTERFACE_SHELL_ACTION_ROLE_PREV || action == INTERFACE_SHELL_ACTION_ROLE_NEXT) {
            continue;
        }
        if (action != INTERFACE_SHELL_ACTION_NONE && kept_count < INTERFACE_SHELL_QUEUE_CAPACITY) {
            kept[kept_count++] = action;
        }
    }

    interface_shell_clear_action_queue();
    for (i = 0; i < kept_count; ++i) {
        g_interface_shell.action_queue[g_interface_shell.action_queue_tail] = kept[i];
        g_interface_shell.action_queue_tail = (g_interface_shell.action_queue_tail + 1) % INTERFACE_SHELL_QUEUE_CAPACITY;
        ++g_interface_shell.action_queue_count;
    }
}

/*
 * 把一次已经发生的物理按下沿保存起来。
 * 队列满时宁可丢掉最新动作，也不能覆盖最早尚未执行的动作，否则玩家会看到操作顺序反转。
 */
static int interface_shell_enqueue_action(InterfaceShellAction action) {
    if (action == INTERFACE_SHELL_ACTION_NONE) return 0;
    if (g_interface_shell.action_queue_count >= INTERFACE_SHELL_QUEUE_CAPACITY) {
        Runtime_Log("[主界面] Shell 输入队列已满；本次额外按键已忽略。");
        return 0;
    }

    g_interface_shell.action_queue[g_interface_shell.action_queue_tail] = action;
    g_interface_shell.action_queue_tail = (g_interface_shell.action_queue_tail + 1) % INTERFACE_SHELL_QUEUE_CAPACITY;
    ++g_interface_shell.action_queue_count;
    return 1;
}

/* 只查看队首动作，不提前删除；只有真正提交成功或确认不适用后才弹出。 */
static InterfaceShellAction interface_shell_peek_action(void) {
    if (g_interface_shell.action_queue_count <= 0) return INTERFACE_SHELL_ACTION_NONE;
    return g_interface_shell.action_queue[g_interface_shell.action_queue_head];
}

/* 删除已经处理完的队首动作。 */
static void interface_shell_pop_action(void) {
    if (g_interface_shell.action_queue_count <= 0) return;

    g_interface_shell.action_queue[g_interface_shell.action_queue_head] = INTERFACE_SHELL_ACTION_NONE;
    g_interface_shell.action_queue_head = (g_interface_shell.action_queue_head + 1) % INTERFACE_SHELL_QUEUE_CAPACITY;
    --g_interface_shell.action_queue_count;
}

/*
 * 读取原版全局 Interface 指针。
 * Runtime_PtrOk 会先检查地址是否像一个可访问的进程内指针；无效就返回 NULL。
 * 后面的所有代码都先经过这个小函数，避免到处直接解引用 0x008DED0C。
 */
static u8* interface_shell_ptr(void) {
    u8* object = *(u8**)GLOBAL_INTERFACE_UI;
    return Runtime_PtrOk(object) ? object : NULL;
}

/*
 * 很多页面在主 Interface 上面还能再盖一层弹窗。
 * 这些弹窗对象的 +0x579 是原版 active 标记：非 0 表示这层当前正在工作。
 * Shell 看到深层弹窗 active 时必须让路，不能拿 B/LB/RB 从下面穿透过去。
 */
static int interface_shell_child_active(u8* child) {
    if (!Runtime_PtrOk(child)) return 0;
    return *(u8*)(child + POPUP_ACTIVE) != 0;
}

/*
 * 0x431380 ButtonEvent 自己有两个已确认的早退条件：
 *   1. Button+0x45 必须非 0，表示按钮当前允许工作；
 *   2. Button+0x04 必须为 0，表示按钮没有处于原版禁止 Event 的状态。
 *
 * Interface Shell 的 Hook 可以在 CALL 点直接返回手柄请求的 code。
 * 如果我们不提前镜像这两个条件，就可能把一个“原版当前不可点击”的按钮也强行执行。
 * 所以任何大类、角色、退出请求在排队前，都必须先过这里。
 */
static int interface_shell_button_accepts_event(void* button) {
    u8* b = (u8*)button;

    /* 没有真实 Button 对象，就没有可以安全复用的原版事件。 */
    if (!Runtime_PtrOk(b)) return 0;

    /* +0x45==0：原版认为按钮当前不可用，手柄同样必须尊重。 */
    if (*(u8*)(b + 0x45u) == 0) return 0;

    /* +0x04!=0：原版 Event 会早退，插件也不能越过这个限制。 */
    if (*(u8*)(b + 0x04u) != 0) return 0;

    return 1;
}

/*
 * 镜像原版 0x435B3F..0x435BA8 的“根导航是否可以处理输入”门控。
 *
 * 这里只回答一个问题：现在是不是安全的主 Interface 外壳时刻？
 * 它不判断某个页面内部应该如何操作。
 */
static int interface_shell_modal_blocked(u8* i) {
    u8* page;
    u8* popup;

    if (!Runtime_PtrOk(i)) return 1;

    /*
     * state7 的普通槽位列表不是 modal：B 归 SaveSlot，LB/RB 仍允许 Shell 切大类。
     * 但选中槽位后出现的“存档/读档/取消”以及它的二次 Yes/No 必须阻止根层输入穿透。
     */
    if (InterfaceTome_ModalActive()) return 1;

    /*
     * state3 治疗法术的“鼠标选角色”虽然不是 Yes/No popup，但从输入层看同样是 modal：
     * 只允许左右选目标、A 使用、B 取消。这里若不阻断 Shell，B/LB/RB 或左摇杆换人可能穿透到底层主菜单，
     * 造成“治疗目标还没选完，页面却先切大类/切角色”的状态错乱。
     */
    if (InterfaceSkills_TargetSelectionActive()) return 1;

    page = *(u8**)(i + INTERFACE_PAGE_2);
    if (Runtime_PtrOk(page)) {
        popup = *(u8**)(page + 0x5ACu);
        if (interface_shell_child_active(popup)) return 1;

        popup = *(u8**)(page + 0x5B0u);
        if (interface_shell_child_active(popup)) return 1;
    }

    page = *(u8**)(i + INTERFACE_PAGE_3);
    if (Runtime_PtrOk(page)) {
        popup = *(u8**)(page + 0x5FCu);
        if (interface_shell_child_active(popup)) return 1;
    }

    page = *(u8**)(i + INTERFACE_PAGE_8);
    if (Runtime_PtrOk(page)) {
        popup = *(u8**)(page + 0x5A8u);
        if (interface_shell_child_active(popup)) return 1;
    }

    return 0;
}

static int interface_shell_root_ready(u8* i) {
    if (!Runtime_PtrOk(i)) return 0;
    if (*(i32*)(i + INTERFACE_CLOSE_STATE) != 0) return 0;

    /*
     * 原版 0x436330 明确要求 +0x590 > 8。
     * 这就是用户现在感觉“旧式 UI 有一点钝”的主要原版动画门：切页后计数会被清零，动画走过若干帧后才重新允许根导航。
     * refactor19 不粗暴跳过这个动画门，而是把动画期间的手柄按键先缓存起来。
     */
    if (*(i32*)(i + INTERFACE_READY_COUNTER) <= 8) return 0;
    if (interface_shell_modal_blocked(i)) return 0;
    return 1;
}

/*
 * 根据 1..8 的原版 state 取得顶部八大类里对应的真实 Button。
 * 这里只“找按钮”，不产生输入，也不修改 Interface state。
 */
static void* interface_shell_category_button(u8* i, int state) {
    u8* nav;
    void* button;

    /* state 超出 1..8 就说明调用者给了无效目标。 */
    if (!Runtime_PtrOk(i) || state < 1 || state > INTERFACE_CATEGORY_COUNT) return NULL;

    /* Interface+0x620 指向统一的顶部主导航对象。 */
    nav = *(u8**)(i + INTERFACE_MAIN_NAV);
    if (!Runtime_PtrOk(nav)) return NULL;

    /* 第一个大类 Button 在 nav+0x57C，后面每一项都是 4 字节指针。 */
    button = *(void**)(nav + INTERFACE_MAIN_BUTTON0 + (u32)(state - 1) * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/* 取得顶部主导航里的真实“退出”Button；原版把它放在 nav+0x59C。 */
static void* interface_shell_exit_button(u8* i) {
    u8* nav;
    void* button;

    if (!Runtime_PtrOk(i)) return NULL;

    nav = *(u8**)(i + INTERFACE_MAIN_NAV);
    if (!Runtime_PtrOk(nav)) return NULL;

    button = *(void**)(nav + INTERFACE_EXIT_BUTTON);
    return Runtime_PtrOk(button) ? button : NULL;
}

/*
 * 判断某个大类此刻是否真的可以切过去。
 * state7“天书”比其它大类多一个原版 DataCenter+0x108==0 的硬门；
 * 这条限制来自原版，不是插件自己发明的，所以手柄也必须照做。
 */
static int interface_shell_category_available(u8* i, int state) {
    u8* data_center;

    /* 先确认真实 Button 本身处于原版可 Event 状态。 */
    if (!interface_shell_button_accepts_event(interface_shell_category_button(i, state))) return 0;

    /* 不是天书就没有这条额外条件。 */
    if (state != 7) return 1;

    /* 天书需要读取原版 DataCenter。 */
    data_center = *(u8**)GLOBAL_DATA_CENTER;
    if (!Runtime_PtrOk(data_center)) return 0;

    return *(i32*)(data_center + 0x108u) == 0;
}

/*
 * LB/RB 请求切顶部大类。
 *
 * 关键原则：只排目标真实 Button 的 code=2，绝不直接写 Interface+0x5BC。
 * 这样页面析构、初始化、动画、音效和原版可用性判断仍由 RPG.exe 自己完成。
 */
static int interface_shell_request_category(u8* i, int delta) {
    int current;
    int candidate;
    int tries;
    void* button;

    /* 已经有别的原版 ButtonEvent 在等待消费时，不能叠第二个事件。 */
    if (!Runtime_PtrOk(i) || delta == 0 || UiBridge_EventPending()) return 0;

    /* 原版当前大类 state 正常范围是 1..8；异常值只用于安全回退，不写回游戏。 */
    current = *(i32*)(i + INTERFACE_STATE);
    if (current < 1 || current > INTERFACE_CATEGORY_COUNT) current = 1;

    candidate = current;

    /*
     * 最多尝试八次，允许跳过当前被原版禁用的大类。
     * LB 在 1 左边循环到 8，RB 在 8 右边循环到 1。
     */
    for (tries = 0; tries < INTERFACE_CATEGORY_COUNT; ++tries) {
        candidate += delta;

        if (candidate < 1) candidate = INTERFACE_CATEGORY_COUNT;
        if (candidate > INTERFACE_CATEGORY_COUNT) candidate = 1;

        /* 回到自己或目标不可用，就继续找下一项。 */
        if (candidate == current || !interface_shell_category_available(i, candidate)) continue;

        button = interface_shell_category_button(i, candidate);

        /* 手柄明确开始菜单导航，因此 Cursor 所有权切给手柄。 */
        Cursor_ClaimForControllerNavigation();

        /* UiBridge 会等原版恰好调用这个 Button 时，再一次性返回 code=2。 */
        UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, 2,
            delta < 0 ? "[主界面] LB：请求原版切到左侧大类。" : "[主界面] RB：请求原版切到右侧大类。");
        return 1;
    }

    /* 八项都没有合法目标时什么也不做。 */
    return 0;
}

/*
 * 根据队伍索引取得角色对象里的真实角色 Button。
 * Interface+0x60C 开始保存最多 5 个角色对象指针；每个角色对象 +0x5A0 是自己的 Button。
 */
static void* interface_shell_role_button(u8* i, int index) {
    u8* role;
    void* button;

    if (!Runtime_PtrOk(i) || index < 0 || index >= INTERFACE_ROLE_MAX) return NULL;

    role = *(u8**)(i + INTERFACE_ROLE_OBJECTS + (u32)index * 4u);
    if (!Runtime_PtrOk(role)) return NULL;

    button = *(void**)(role + INTERFACE_ROLE_BUTTON);
    return Runtime_PtrOk(button) ? button : NULL;
}

/*
 * 角色切换统一开放 state1..5，但物理操作从 refactor26 起不再使用 D-Pad。
 *
 * 用户的新总规则是“左摇杆水平推过 50% 才切角色”：
 * - state1..5：Shell 消费 InputRouter 提供的左摇杆水平单次方向沿；
 * - state2/state3/state4 的 D-Pad ←/→ 已释放给页面翻页；
 * - state5 的 D-Pad 不再有换人含义，LT/RT 继续加减；
 * - state6 阵形仍完整拥有 D-Pad 四方向，左摇杆也不切角色；
 * - state7/8 是否允许角色切换等页面实现时再由实际业务裁决。
 *
 * 角色按钮查找、原版动画门和事件提交仍只有这一处，页面 Adapter 不复制换人业务。
 */
static int interface_shell_request_role(u8* i, int delta) {
    int state;
    int count;
    int current;
    int target;
    int tries;
    void* button;

    if (!Runtime_PtrOk(i) || delta == 0 || UiBridge_EventPending()) return 0;

    /* state1..5 允许 Shell 使用左右切角色；state6 起必须由各页面自己明确拥有。 */
    state = *(i32*)(i + INTERFACE_STATE);
    if (state < 1 || state > 5) return 0;

    /* 队伍人数由原版 Interface 自己提供；再硬限制到已确认最大 5 人。 */
    count = *(i32*)(i + INTERFACE_ROLE_COUNT);
    if (count < 1) return 0;
    if (count > INTERFACE_ROLE_MAX) count = INTERFACE_ROLE_MAX;

    /* 当前角色索引异常时只在本地把起点当 0，不直接修游戏内存。 */
    current = *(i32*)(i + INTERFACE_SELECTED_ROLE);
    if (current < 0 || current >= count) current = 0;

    target = current;

    /* 向指定方向循环寻找下一个真正可 Event 的角色 Button。 */
    for (tries = 0; tries < count; ++tries) {
        target += delta;

        if (target < 0) target = count - 1;
        if (target >= count) target = 0;

        /* 队伍只有一个角色时最终会回到 current，此时不制造无意义事件。 */
        if (target == current) continue;

        button = interface_shell_role_button(i, target);
        if (!interface_shell_button_accepts_event(button)) continue;

        Cursor_ClaimForControllerNavigation();
        UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, 2,
            delta < 0 ? "[主界面] 左摇杆向左超过50%：请求原版切到上一角色。" :
                        "[主界面] 左摇杆向右超过50%：请求原版切到下一角色。");
        return 1;
    }

    return 0;
}

/*
 * 根层 B 使用主导航自己的真实退出 Button。
 * 0x435C9C 对这只按钮使用的是 code=1，因此这里不能像大类/角色那样发 code=2。
 * 同样禁止直接写 Interface+0x5F8，让原版自己执行完整关闭动画和地图恢复。
 */
static int interface_shell_request_exit(u8* i) {
    void* button;

    if (!Runtime_PtrOk(i) || UiBridge_EventPending()) return 0;

    button = interface_shell_exit_button(i);
    if (!interface_shell_button_accepts_event(button)) return 0;

    Cursor_ClaimForControllerNavigation();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE, button, 1, "[主界面] 根层 B：请求原版退出按钮返回地图。");
    return 1;
}

/*
 * 给页面Adapter使用的窄入口：请求“退出整个主Interface”，不是退出页面内部子窗口。
 *
 * 为什么需要公开这一小步：state7天书的根内容本身就是一份SaveSlot。共享SaveSlot会把B解释成
 * 自己的取消按钮并先消费，Shell因此永远看不到退出主菜单的意图。天书Adapter必须在调用
 * SaveSlot_Update之前把这颗B交给Shell，但又不能复制退出Button地址或直接写Interface状态。
 *
 * 这里复用Shell原有队列：动画期间也不会丢键，等root_ready后仍由interface_shell_request_exit()
 * 点击原版真实退出Button。深层弹窗存在时直接拒绝，B继续留给页面自己的模态控制器。
 */
int InterfaceShell_RequestRootExitFromPage(void) {
    u8* i = interface_shell_ptr();

    if (!Runtime_PtrOk(i)) return 0;
    if (*(i32*)(i + INTERFACE_CLOSE_STATE) != 0) return 0;
    if (interface_shell_modal_blocked(i)) return 0;

    /* 退出优先级最高：丢弃尚未提交的切大类/切角色意图，只保留这一颗明确的返回。 */
    interface_shell_clear_action_queue();
    if (!interface_shell_enqueue_action(INTERFACE_SHELL_ACTION_EXIT)) return 0;

    InputRouter_Consume(INPUT_CANCEL);
    Cursor_ClaimForControllerNavigation();
    Runtime_Log("[主界面] 页面根层取消：已排队原版退出Button，等待主界面动画门开放。");
    return 1;
}

/*
 * 判断“普通探索 Y 打开主 Interface”现在是否允许。
 *
 * 这里只做插件自己的 Context 隔离；真正与键盘 Space 完全相同的地图业务门控，
 * 会在游戏线程提交时继续检查 0x89F808 与 0x468BF0，并最终由原版 0x40B230 自己再次校验。
 */
static int interface_shell_normal_map_open_allowed(void) {
    u8* movie;

    if (!PadInput_GameForeground(NULL)) return 0;
    if (interface_shell_ptr()) return 0;
    if (Battle_AnyUiActive() || Frontend_AnyUiActive()) return 0;
    if (*(u32*)GLOBAL_DIALOGUE_ID != 0u) return 0;

    movie = *(u8**)GLOBAL_MOVIE_OBJECT;
    if (Runtime_PtrOk(movie) && *(u8*)(movie + MOVIE_ACTIVE_FLAG)) return 0;

    return 1;
}

/*
 * 由 worker 捕获地图 Y 的新按下沿。
 *
 * 为什么只置 pending：
 * - 原版 Space 当前业务 CALL 会切换地图脚本/动作状态，必须让它在游戏线程执行；
 * - worker 只负责 SDL 轮询和语义路由，不直接调用会改游戏状态的业务函数；
 * - Exploration_HookMouseAction 本来就在普通地图每帧的游戏线程路径上，因此下一帧一定有安全消费点。
 */
static void interface_shell_capture_map_y(void) {
    if (!interface_shell_normal_map_open_allowed()) return;
    if (g_interface_shell.map_y_pending) return;

    if (!InputRouter_PressedOn(INPUT_CTX_EXPLORATION, INPUT_SPECIAL_Y, INPUT_LAYER_COMMON)) return;

    g_interface_shell.map_y_pending = 1;
    InputRouter_Consume(INPUT_SPECIAL_Y);
    Cursor_ClaimForControllerNavigation();
    Runtime_Log("[主界面] 地图 Y：已捕获新按下沿，等待 Exploration 游戏线程提交原版 Space 业务事件。");
}

/*
 * Exploration 模块在自己的 0x409597 游戏线程 Hook 内调用这里。
 * 这就是 refactor19 对“地图 Y”的最终入口：不发送 VK_SPACE，不 PostMessage，不伪造键盘状态。
 *
 * 原版 0x44A4D0 的 Space 分支已经静态闭合为：
 *   key == VK_SPACE
 *   -> [0x89F808] == 0
 *   -> [0x468BF0] != 0
 *   -> push 1
 *   -> call <运行时解析的原版Space当前目标>
 *
 * 所以手柄 Y 只重放最后那条“地图动作事件”，目标地址由 Runtime 从原版 Space CALL 现场解析，并保留同样的两个前置条件。
 */
void InterfaceShell_OnExplorationGameThread(void) {
    PFN_MapSpaceEvent map_space_event = (PFN_MapSpaceEvent)Runtime_MapSpaceEventTarget();

    if (!g_interface_shell.map_y_pending) return;

    /* 一次 pending 只尝试一次，先清标记可以防止原版调用链内部重入时重复提交。 */
    g_interface_shell.map_y_pending = 0;

    if (!interface_shell_normal_map_open_allowed()) {
        Runtime_Log("[主界面] 地图 Y：已到 Exploration 游戏线程，但 Context 已变化；本次请求安全取消。");
        return;
    }

    /*
     * 完整镜像原版 Space 分支在调用 0x40B230 前的两个条件。
     * 这里把每一种拒绝原因分别记日志：如果实机仍打不开菜单，我们能立刻知道是 Y 没捕获、
     * 游戏线程没有跑到、还是原版自己认为当前地图动作忙/模式不允许，而不需要再猜入口地址。
     */
    if (*(i32*)GLOBAL_MAP_ACTION_BUSY != 0) {
        Runtime_Log("[主界面] 地图 Y：已到 Exploration 游戏线程，但原版地图动作忙（0x89F808!=0），本次不提交。");
        return;
    }
    if (*(i32*)GLOBAL_MAP_KEY_MODE == 0) {
        Runtime_Log("[主界面] 地图 Y：已到 Exploration 游戏线程，但原版键位模式为0（0x468BF0==0），本次不提交。");
        return;
    }

    map_space_event(1);
    Runtime_Log("[主界面] 地图 Y：已触发原版 Space 当前实际使用的地图动作业务事件(action=1)。");
}

/*
 * 安装主 Interface 第一阶段 Hook。
 * refactor19 已删除 r18 对 0x40CC73 的错误入口 Hook；地图 Y 现在不需要新增机器码修改。
 * 这里只安装“Interface 已经打开以后”的真实 ButtonEvent 桥。
 */
int InterfaceShell_InstallHooks(void) {
    if (!Runtime_InterfaceShellProtocolOk()) return 0;
    if (!UiBridge_InstallInterfaceShellHooks()) return 0;

    g_interface_shell.map_y_pending = 0;
    g_interface_shell.interface_seen = 0;
    interface_shell_clear_action_queue();

    Runtime_Log("[主界面] r20a兼容Shell Hook 已安装：地图Y跟随原版Space当前业务CALL + 根层Shell动画期输入缓冲。");
    return 1;
}

/* 其它模块只需要知道 Interface 是否存在，不应该直接读取 0x008DED0C。 */
int InterfaceShell_Active(void) {
    return interface_shell_ptr() != NULL;
}

/*
 * 把本 tick 的 Shell 新按下沿放进短队列。
 *
 * 注意：这里故意不要求 +0x590>8。这样玩家在原版切页动画进行中按一次 RB、推一次左摇杆或按 B，
 * 这个意图也不会像 refactor18 那样被直接丢掉；等动画门重新开放时会自动提交。
 * 已知深层弹窗 active 时则完全不捕获，避免把 B/LB/RB 从弹窗下面“记账”到根层。
 */
static void interface_shell_capture_root_actions(u8* i) {
    int state;

    if (!Runtime_PtrOk(i)) return;
    if (*(i32*)(i + INTERFACE_CLOSE_STATE) != 0) return;
    if (interface_shell_modal_blocked(i)) return;

    /* B 优先级最高：同一个 tick 如果同时出现多个新按下沿，只记录最符合“返回”直觉的 B。 */
    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SHELL, INPUT_CANCEL, INPUT_LAYER_OVERLAY)) {
        /*
         * B 是“返回/退出”，优先级必须高于此前尚未执行的导航意图。
         * 例如玩家在切页动画中快速按了两次 RB，随后立刻按 B，如果继续 FIFO 执行两个 RB 再退出，
         * 体感会像 B 完全没反应。这里先丢弃尚未提交的大类/角色动作，再只保留这一次退出请求。
         * 已经提交给原版并正在播放的那一次动画不会被强行打断，仍等原版安全门重新开放。
         */
        InterfaceShell_RequestRootExitFromPage();
        return;
    }

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SHELL, INPUT_CATEGORY_PREV, INPUT_LAYER_OVERLAY)) {
        if (interface_shell_enqueue_action(INTERFACE_SHELL_ACTION_CATEGORY_PREV)) {
            InputRouter_Consume(INPUT_CATEGORY_PREV);
            Cursor_ClaimForControllerNavigation();
        }
        return;
    }

    if (InputRouter_PressedOn(INPUT_CTX_INTERFACE_SHELL, INPUT_CATEGORY_NEXT, INPUT_LAYER_OVERLAY)) {
        if (interface_shell_enqueue_action(INTERFACE_SHELL_ACTION_CATEGORY_NEXT)) {
            InputRouter_Consume(INPUT_CATEGORY_NEXT);
            Cursor_ClaimForControllerNavigation();
        }
        return;
    }

    /*
     * refactor26：角色切换完全从 D-Pad 解耦。
     * InputRouter_BeginFrame 已经把左摇杆水平轴转换成“越过 50% 的单次方向沿”：
     * - -1：明确向左推过半程；
     * - +1：明确向右推过半程；
     * - 0：没有新的推杆动作。
     *
     * state1..5 才允许换人；state6 阵形以及未来 state7/8 都不会因为左摇杆误切角色。
     */
    state = *(i32*)(i + INTERFACE_STATE);
    if (state >= 1 && state <= 5) {
        int role_step = InputRouter_LeftStickHorizontalStep50();

        if (role_step < 0) {
            if (interface_shell_enqueue_action(INTERFACE_SHELL_ACTION_ROLE_PREV)) {
                Cursor_ClaimForControllerNavigation();
            }
            return;
        }
        if (role_step > 0) {
            if (interface_shell_enqueue_action(INTERFACE_SHELL_ACTION_ROLE_NEXT)) {
                Cursor_ClaimForControllerNavigation();
            }
            return;
        }
    }
}

/*
 * 只在原版明确允许根导航、且 UiBridge 没有上一条事件等待消费时，提交队首动作。
 * 一次最多提交一条，让 RPG.exe 自己完成当前页的关闭/打开动画后再处理下一条。
 */
static void interface_shell_dispatch_root_action(u8* i) {
    InterfaceShellAction action;
    int handled = 0;

    if (!interface_shell_root_ready(i) || UiBridge_EventPending()) return;

    action = interface_shell_peek_action();
    if (action == INTERFACE_SHELL_ACTION_NONE) return;

    switch (action) {
    case INTERFACE_SHELL_ACTION_EXIT:
        handled = interface_shell_request_exit(i);
        break;
    case INTERFACE_SHELL_ACTION_CATEGORY_PREV:
        handled = interface_shell_request_category(i, -1);
        break;
    case INTERFACE_SHELL_ACTION_CATEGORY_NEXT:
        handled = interface_shell_request_category(i, +1);
        break;
    case INTERFACE_SHELL_ACTION_ROLE_PREV:
        handled = interface_shell_request_role(i, -1);
        break;
    case INTERFACE_SHELL_ACTION_ROLE_NEXT:
        handled = interface_shell_request_role(i, +1);
        break;
    default:
        break;
    }

    /*
     * 到达“root ready”后仍无法提交，通常说明这个动作在当前页面已经不再适用
     * （例如动作排队期间大类已经从 state4 切到 state5，旧的“切角色”请求就应该作废）。
     * 此时必须弹出，不能让一条失效动作永久堵住后面的队列。
     */
    (void)handled;
    interface_shell_pop_action();
}

/*
 * worker 每 8ms 调一次这里。
 * - Interface 不存在：只捕获一次地图 Y，并等待 Exploration 游戏线程安全点执行原版 Space 业务事件；
 * - Interface 存在：先记录用户按键，再等原版动画门允许时顺序提交。
 */
void InterfaceShell_Update(void) {
    u8* i = interface_shell_ptr();

    /* ---------------- 情况一：现在还没有主 Interface ---------------- */
    if (!i) {
        if (g_interface_shell.interface_seen) {
            g_interface_shell.interface_seen = 0;
            UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE);
            interface_shell_clear_action_queue();
            Runtime_Log("[主界面] Interface 已关闭，控制权返回地图。");
        }

        interface_shell_capture_map_y();
        return;
    }

    /* ---------------- 情况二：主 Interface 已经存在 ---------------- */
    g_interface_shell.map_y_pending = 0;

    if (!g_interface_shell.interface_seen) {
        g_interface_shell.interface_seen = 1;
        interface_shell_clear_action_queue();
        Runtime_Log("[主界面] 已检测到原版主 Interface；Shell 已启用动画期输入缓冲。");
    }

    /* 先记住本帧的新按键，再尝试提交队首；这样即使当前正处于动画门内，按键也不会丢。 */
    interface_shell_capture_root_actions(i);
    interface_shell_dispatch_root_action(i);
}
