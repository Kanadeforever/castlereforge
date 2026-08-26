#include "battle.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "cursor.h"
#include "spatial_neighbor.h"
#include "ui_bridge.h"

/*
 * battle.c
 *
 * 这不是把 dev20 大文件“剪切过来”就结束。
 * 本文件重新把战斗状态收拢为 Battle 自己拥有的状态域，并通过 InputRouter / Cursor / UiBridge
 * 与其它子系统交流。这样 Title/Save 后续继续扩展时，不能再直接碰战斗的 focus、Target 或视觉锁存。
 *
 * 用户侧目标仍然是 dev20 的战斗体验，同时修复一个 dev20 已知视觉缺陷：
 * “从子菜单按 B 返回主菜单时，逻辑焦点已经正确，但视觉会先停在第一项约 0.5~1 秒”。
 * refactor3 已经进一步闭合“顶层菜单动画每帧强制 marker 回第 1 项”的原版写入，并由实机确认顶层残留已修复。
 * refactor4 继续处理另一条不同的原版路径：CMD1/CMD2 父列表从确认框或 Target 返回时，列表自己的动画
 * 会主动把真实鼠标移回默认位置，而且动画期间正常行 HitTest 暂停。这里必须只在手柄导航拥有光标时：
 *   1. 屏蔽这次鼠标默认位置 warp；
 *   2. 在动画执行后维持父列表真正逻辑行的原版 Button+0x44 视觉状态。
 * 键鼠模式完全保留原版行为，Target 本身已经通过的鼠标坐标协议也不修改。
 */

#define VISUAL_LATCH_MAX_MS 480u
#define VISUAL_LATCH_GONE_MS 48u
#define NAV_QUEUE_CAP 16

/* Battle 内部 Context。它描述原版 UI 当前是哪一层，不等同于通用 InputContext。 */
enum {
    BCTX_NONE = 0,
    BCTX_TOP,
    BCTX_CMD0,
    BCTX_CMD1,
    BCTX_CMD2,
    BCTX_CMD3,
    BCTX_CONFIRM,
    BCTX_TARGET
};

/* dev20 原有战斗状态，全部变成 Battle 模块私有。 */
static int g_battle_was_present;
static int g_result_was_present;
static int g_nav_active;
static int g_context;
static int g_prev_context;
static int g_top_focus = -1;
static int g_cmd1_focus;
static int g_cmd2_focus;
static int g_popup_focus;
static int g_target_cell = -1;
static int g_target_filter_last_mode = -999;
static int g_target_filter_last_count = -1;
static int g_pending_row_after_page = -1;
static int g_pending_page_context;
static int g_pending_page_old = -1;
static volatile int g_target_event_code;
static int g_cancel_return_from_context;

/* 严格视觉握手：方向输入可以排队，但必须等真实原版 HitTest 至少出现指定次数才推进下一步。 */
static signed char g_nav_queue[NAV_QUEUE_CAP];
static int g_nav_q_head, g_nav_q_tail, g_nav_q_count;
static int g_nav_settle_active;
static u32 g_nav_settle_start_tick;
static u32 g_nav_settle_hit_base;
static void* g_nav_settle_button;
static int g_pending_confirm_after_nav;
/*
 * A 在视觉握手期间允许短暂缓冲，但绝不能像 r21 实机反馈那样无限等待。
 * 这个 tick 记录“缓冲从什么时候开始”，到诊断阈值还不能确认就直接丢弃本次 A。
 */
static u32 g_pending_confirm_start_tick;

/*
 * CMD1/CMD2 的确认事务看门狗。
 * 有些“当前不能使用”的技能/道具，其 Button 本身仍可 HitTest，真正的业务拒绝发生在
 * 0x41817A / 0x419BAE 之后。插件无法只靠 Button+0x45/+0x04 预先知道这种业务条件。
 * 因此我们记住“刚才给哪一行提交了 A”；如果原版消费了事件却仍留在同一列表，
 * 就把它视为原版正常的“无动作/拒绝”，立即释放手柄导航事务。
 */
static int g_confirm_watch_active;
static int g_confirm_watch_context;
static void* g_confirm_watch_button;
static u32 g_confirm_watch_start_tick;
/* 原版真正消费 UiBridge Event 的时刻单独记录；视觉宽限期从“消费完成”而不是“玩家按 A”开始算。 */
static u32 g_confirm_watch_consumed_tick;
static int g_confirm_watch_consumed_seen;
static u32 g_repeat_up_next;
static u32 g_repeat_down_next;
static u32 g_nav_settle_min_ticks;
static u32 g_nav_settle_timeout_ticks;
static u32 g_nav_repeat_initial_ticks;
static u32 g_nav_repeat_interval_ticks;
static int g_nav_settle_timeout_logged;
static volatile u32 g_forced_hit_calls;
static volatile void* g_last_forced_hit_button;

/* 提交动作后：旧菜单退场时继续保持“刚确认的那一项”，避免隐藏前闪回第一项。 */
static int g_visual_latch_active;
static int g_visual_latch_context;
static void* g_visual_latch_button;
static u32 g_visual_latch_arm_tick;
static u32 g_visual_latch_last_seen_tick;
static int g_visual_latch_seen;

/*
 * dev20 已知缺陷修复：取消返回父层的视觉恢复事务。
 * 它与“退场锁存”完全不同：退场锁存管旧层，ReturnVisual 管正在重新出现的父层。
 */
typedef struct ReturnVisualTransaction {
    int active;
    int target_context;
    void* target_button;
    int seen_target_hit;
    u32 arm_tick;
} ReturnVisualTransaction;
static ReturnVisualTransaction g_return_visual;

/* 进入确认框/Target 前保存其父列表，B 返回时才能提前知道该恢复哪个真实 Button。 */
static int g_overlay_parent_context;
static void* g_overlay_parent_button;


/*
 * 下面这些函数在文件后半段定义，但前半段的分页/快捷逻辑已经会调用它们。
 * C 语言不会像某些脚本语言那样“自动知道后面有什么函数”，所以先声明。
 * 这样既避免隐式声明，也让阅读者一眼看出 Battle 内部有哪些导航基础动作。
 */
static void nav_queue_clear(void);
static void nav_settle_clear(void);
static void nav_repeat_clear(void);
static void nav_begin_settle(void);
static void confirm_watch_clear(void);
static void confirm_watch_maintenance(void);
static void return_visual_clear(const char* reason);
static void return_visual_maintenance(void);

/*
 * 下面三个小工具只服务于中文诊断日志。
 * 项目为了保持老游戏注入环境尽量简单，不链接 CRT，所以不能直接依赖 sprintf/printf。
 */
static char battle_hex_digit(u32 v) {
    /* 0..9 映射为字符 '0'..'9'；10..15 映射为 'A'..'F'。 */
    return (char)(v < 10u ? ('0' + v) : ('A' + (v - 10u)));
}

static void append_text(char* dst, SIZE_T cap, SIZE_T* pos, const char* src) {
    SIZE_T i = 0;

    /* 任一参数无效就直接返回；日志失败不能影响游戏主逻辑。 */
    if (!dst || !pos || !src || cap == 0) return;

    /* 每次都预留 1 字节给字符串结尾的 '\0'，防止写出缓冲区。 */
    while (src[i] && (*pos + 1u) < cap) {
        dst[(*pos)++] = src[i++];
    }
    dst[*pos] = '\0';
}

static void append_hex32(char* dst, SIZE_T cap, SIZE_T* pos, u32 value) {
    int shift;

    append_text(dst, cap, pos, "0x");
    /* 从最高 4 bit 开始，一共写 8 个十六进制数字。 */
    for (shift = 28; shift >= 0; shift -= 4) {
        char one[2];
        one[0] = battle_hex_digit((value >> (u32)shift) & 0xFu);
        one[1] = '\0';
        append_text(dst, cap, pos, one);
    }
}

/* RB 组合快捷仍属于 Battle 专属叠加层，因此这里直接检查“RB 按住 + 另一个键按下沿”。 */
static int battle_chord_edge(InputAction action) {
    /*
     * dev20 的快捷命令以 RB 为修饰键。这里仍由 Battle 决定“这个组合代表哪条战斗命令”，
     * 但物理按钮关系交给 input_router；Battle 本身不再出现 SDL/PadButton 编号。
     */
    return InputRouter_ChordPressed(INPUT_CATEGORY_NEXT, action);
}

static int battle_present(void) {
    void* battle = *(void**)GLOBAL_BATTLE_UI;
    return Runtime_PtrOk(battle);
}

/* 返回当前战斗 UI 对象；无效时统一返回 NULL，后续代码就不需要重复写危险指针判断。 */
static u8* battle_ptr(void) {
    u8* battle = *(u8**)GLOBAL_BATTLE_UI;
    return Runtime_PtrOk(battle) ? battle : NULL;
}

/* Battle Result 是独立于主战斗 UI 的对象，因此单独读取自己的全局指针。 */
static u8* result_ptr(void) {
    u8* r = *(u8**)GLOBAL_RESULT_UI;
    return Runtime_PtrOk(r) ? r : NULL;
}

static int result_active(void) {
    u8* r = result_ptr();
    u8* d;
    if (!r) return 0;
    if (*(u8*)(r + RESULT_ACTIVE) != 0) return 1;
    d = *(u8**)(r + RESULT_DETAIL_PTR);
    return Runtime_PtrOk(d) && *(u8*)(d + RESULT_ACTIVE) != 0;
}

/*
 * 找出 Result 当前真正应该接受 A 的原版 Button。
 * 明细层优先于外层，因为明细弹出后外层按钮仍可能保留在对象里但已经不该响应。
 */
static void* result_active_button(void) {
    u8* r = result_ptr();
    u8* d;
    void* b;
    if (!r) return NULL;
    d = *(u8**)(r + RESULT_DETAIL_PTR);
    if (Runtime_PtrOk(d) && *(u8*)(d + RESULT_ACTIVE) != 0) {
        b = *(void**)(d + RESULT_DETAIL_BUTTON);
        return Runtime_PtrOk(b) ? b : NULL;
    }
    if (*(u8*)(r + RESULT_ACTIVE) != 0) {
        b = *(void**)(r + RESULT_OUTER_BUTTON);
        return Runtime_PtrOk(b) ? b : NULL;
    }
    return NULL;
}

/* Target Selector 有独立全局对象；只有通过基础指针检查后才允许读它的阵位表。 */
static u8* target_ptr(void) {
    u8* target = *(u8**)GLOBAL_TARGET_SELECTOR;
    return Runtime_PtrOk(target) ? target : NULL;
}

/* +TARGET_ACTIVE 是原版选择器自己的生命周期标记，比“猜当前战斗命令”更可靠。 */
static int target_active(void) {
    u8* target = target_ptr();
    return target && *(u8*)(target + TARGET_ACTIVE) != 0;
}

static int battle_top_active(void) {
    u8* battle = battle_ptr();
    int actor;
    int active_cmd;
    if (!battle) return 0;
    actor = *(i32*)(battle + BATTLE_CURRENT_ACTOR);
    active_cmd = *(i32*)(battle + BATTLE_ACTIVE_COMMAND);
    if (actor == -1) return 0;
    if (*(u8*)(battle + BATTLE_SUBMENU_ACTIVE) != 0) return 0;
    if (active_cmd != -1) return 0;
    return 1;
}

/* 把内部 Context 转成中文日志名；只用于诊断，不参与任何业务判断。 */
static const char* context_name(int ctx) {
    switch (ctx) {
    case BCTX_TOP: return "战斗主菜单";
    case BCTX_CMD0: return "命令0子层";
    case BCTX_CMD1: return "命令1列表";
    case BCTX_CMD2: return "命令2列表";
    case BCTX_CMD3: return "命令3循环层";
    case BCTX_CONFIRM: return "双按钮确认框";
    case BCTX_TARGET: return "目标选择";
    default: return "无战斗菜单";
    }
}


/*
 * Battle 的内部层级比通用 InputContext 更细，所以在这里统一做一次映射。
 * 业务代码后面只调用 battle_pressed()/battle_down()，不会绕开 PASS/MERGE/OVERRIDE/CONSUME 策略表。
 */
static InputContext battle_input_context(void) {
    if (g_context == BCTX_TOP) return INPUT_CTX_BATTLE_TOP;
    if (g_context == BCTX_CONFIRM) return INPUT_CTX_BATTLE_CONFIRM;
    if (g_context == BCTX_TARGET) return INPUT_CTX_BATTLE_TARGET;
    if (g_context == BCTX_CMD0 || g_context == BCTX_CMD1 || g_context == BCTX_CMD2 || g_context == BCTX_CMD3)
        return INPUT_CTX_BATTLE_LIST;
    return INPUT_CTX_NONE;
}

/* Battle 业务只从 OVERLAY 通道取按下沿；是否能收到由 InputRouter 策略表统一决定。 */
static int battle_pressed(InputAction action) {
    return InputRouter_PressedOn(battle_input_context(), action, INPUT_LAYER_OVERLAY);
}

/* 与 battle_pressed 相同，但读取“持续按住”，用于方向 repeat。 */
static int battle_down(InputAction action) {
    return InputRouter_DownOn(battle_input_context(), action, INPUT_LAYER_OVERLAY);
}

static u8* active_confirm_popup(void) {
    u8* battle = battle_ptr();
    u8* sub = NULL;
    u8* popup = NULL;
    int cmd;
    if (!battle) return NULL;
    if (*(u8*)(battle + BATTLE_SUBMENU_ACTIVE) == 0) return NULL;
    cmd = *(i32*)(battle + BATTLE_ACTIVE_COMMAND);
    if (cmd == 1) {
        sub = *(u8**)(battle + BATTLE_CMD1_SUBUI);
        if (Runtime_PtrOk(sub)) popup = *(u8**)(sub + SUB1_CONFIRM_POPUP);
    } else if (cmd == 2) {
        sub = *(u8**)(battle + BATTLE_CMD2_SUBUI);
        if (Runtime_PtrOk(sub)) popup = *(u8**)(sub + SUB2_CONFIRM_POPUP);
    }
    if (!Runtime_PtrOk(popup)) return NULL;
    return *(u8*)(popup + POPUP_ACTIVE) ? popup : NULL;
}

/*
 * 按“最具体的覆盖层优先”识别战斗 Context：确认框/Target 必须先于父列表。
 * 如果顺序反过来，A/B 会被父菜单提前消费，正是旧式大 if 链容易产生的耦合问题。
 */
static int detect_context(void) {
    u8* battle = battle_ptr();
    int cmd;
    if (!battle) return BCTX_NONE;
    /* 确认框必须优先于列表/Target：它是技能/道具提交前的独立 UI Context。 */
    if (active_confirm_popup()) return BCTX_CONFIRM;
    if (target_active()) return BCTX_TARGET;
    if (battle_top_active()) return BCTX_TOP;
    if (*(u8*)(battle + BATTLE_SUBMENU_ACTIVE) == 0) return BCTX_NONE;
    cmd = *(i32*)(battle + BATTLE_ACTIVE_COMMAND);
    if (cmd == 0) return BCTX_CMD0;
    if (cmd == 1) return BCTX_CMD1;
    if (cmd == 2) return BCTX_CMD2;
    if (cmd == 3) return BCTX_CMD3;
    return BCTX_NONE;
}

/* 根据 Context 只返回对应原版子 UI 指针；其它 Context 明确返回 NULL。 */
static u8* battle_subui(int ctx) {
    u8* battle = battle_ptr();
    u8* p = NULL;
    if (!battle) return NULL;
    if (ctx == BCTX_CMD0) p = *(u8**)(battle + BATTLE_CMD0_SUBUI);
    else if (ctx == BCTX_CMD1) p = *(u8**)(battle + BATTLE_CMD1_SUBUI);
    else if (ctx == BCTX_CMD2) p = *(u8**)(battle + BATTLE_CMD2_SUBUI);
    else if (ctx == BCTX_CMD3) p = *(u8**)(battle + BATTLE_CMD3_SUBUI);
    return Runtime_PtrOk(p) ? p : NULL;
}

/*
 * 复刻 0x431380 已确认的按钮早退条件：+0x45 必须启用、+0x04 必须为 0。
 * 这样手柄不会给原版当前不可点击的按钮硬塞事件。
 */
static int button_usable(void* button) {
    u8* b = (u8*)button;
    if (!Runtime_PtrOk(b)) return 0;
    if (*(u8*)(b + 0x45u) == 0) return 0;
    if (*(u8*)(b + 0x04u) != 0) return 0;
    return 1;
}

/* 从原版六命令按钮数组取第 idx 项，并在返回前做指针与可用性双重校验。 */
static void* top_button(int idx) {
    u8* battle = battle_ptr();
    u8* panel;
    void* b;
    if (!battle || idx < 0 || idx >= COMMAND_COUNT) return NULL;
    panel = *(u8**)(battle + BATTLE_COMMAND_PANEL);
    if (!Runtime_PtrOk(panel)) return NULL;
    b = *(void**)(panel + COMMAND_BUTTONS + (u32)idx * 4u);
    return Runtime_PtrOk(b) ? b : NULL;
}

/*
 * 立即把战斗主菜单的“原版选中标记”移动到指定真实 Button。
 *
 * 这里不是凭经验写一个屏幕 Y 坐标，而是严格复用 RPG.exe 0x41C380 已经闭合的原版协议：
 * 1. battle+0x58C 是六个主命令 Button 指针；
 * 2. panel+0x588 是原版 ITF0026 选中 marker；
 * 3. Button+0x38 保存该按钮自己的局部 Y；
 * 4. 原版 HitTest 命中后最终执行 marker+0x24 = Button+0x38。
 *
 * dev20 的已知缺陷是：B 从子菜单返回时逻辑 g_top_focus 已经正确，但原版 marker 仍会在第 1 项
 * 停留约 0.5~1 秒，直到后续 HitTest/动画把它追上。新架构在“取消返回事务”里提前执行同一条
 * 原版最终赋值，使逻辑焦点和视觉焦点在父菜单重新出现时同步，而不是另外造一套视觉坐标。
 */
static int top_visual_sync_marker_to_button(void* button, const char* reason) {
    u8* battle = battle_ptr();
    u8* panel;
    u8* marker;
    i32 y;
    char line[256];
    SIZE_T pos = 0;

    if (!battle || !Runtime_PtrOk(button)) return 0;
    panel = *(u8**)(battle + BATTLE_COMMAND_PANEL);
    if (!Runtime_PtrOk(panel)) return 0;
    marker = *(u8**)(panel + COMMAND_MARKER_OBJECT);
    if (!Runtime_PtrOk(marker)) return 0;

    y = *(i32*)((u8*)button + BUTTON_LOCAL_Y);
    *(i32*)(marker + MARKER_LOCAL_Y) = y;

    /*
     * 只在事务建立时记录日志；maintenance 会每个 worker tick 重做同一赋值，若每次都写日志会产生噪声。
     * reason==NULL 就表示“静默维持”，因此这里不输出。
     */
    if (reason) {
        append_text(line,sizeof(line),&pos,"[战斗视觉] 已按原版 marker 协议立即同步主菜单高亮，按钮=");
        append_hex32(line,sizeof(line),&pos,(u32)(SIZE_T)button);
        append_text(line,sizeof(line),&pos,"，Y=");
        append_hex32(line,sizeof(line),&pos,(u32)y);
        append_text(line,sizeof(line),&pos,"，原因=");
        append_text(line,sizeof(line),&pos,reason);
        Runtime_Log(line);
    }
    return 1;
}

/* 确认框只有“是/否”两按钮；index=0/1 映射到原版真实 Button 指针。 */
static void* popup_button(int index) {
    u8* popup = active_confirm_popup();
    void* b;
    if (!popup || (index != 0 && index != 1)) return NULL;
    b = *(void**)(popup + (index == 0 ? POPUP_BUTTON_YES : POPUP_BUTTON_NO));
    return Runtime_PtrOk(b) ? b : NULL;
}

/*
 * 从“明确传入的 CMD1/CMD2 子菜单对象”取得某一行的真实 Button。
 *
 * 为什么 refactor4 需要这个版本，而不能只使用 battle_subui()：
 * 动画 Hook 收到的 `sub` 就是游戏主线程此刻正在展开/收起的那个对象。直接从这个对象取行 Button，
 * 可以保证我们修的是“当前这一帧真正执行动画的列表”，不会因为全局 Battle 指针恰好开始切换 Context
 * 而把另一个旧对象或新对象的视觉状态改错。
 */
static void* submenu_row_button_from_sub(int ctx, u8* sub, int row) {
    u8* panel;
    void* button;
    u32 panel_offset;

    /* 只有 CMD1/CMD2 有这种 8 行列表结构，其它 Context 传进来属于调用错误，直接不处理。 */
    if (!Runtime_PtrOk(sub) || row < 0 || row >= SUB_ROWS_COUNT) return NULL;
    if (ctx == BCTX_CMD1) panel_offset = SUB1_ROW_PANEL;
    else if (ctx == BCTX_CMD2) panel_offset = SUB2_ROW_PANEL;
    else return NULL;

    /* `sub+panel_offset` 保存原版行面板指针；面板的 +0x57C 开始是 8 个真实 Button 指针。 */
    panel = *(u8**)(sub + panel_offset);
    if (!Runtime_PtrOk(panel)) return NULL;

    button = *(void**)(panel + SUB_ROWS_BASE + (u32)row * 4u);
    return Runtime_PtrOk(button) ? button : NULL;
}

/* CMD1 每页最多 8 行；这里从行面板取原版行 Button，而不是按坐标模拟鼠标。 */
static void* sub1_row_button(int row) {
    return submenu_row_button_from_sub(BCTX_CMD1, battle_subui(BCTX_CMD1), row);
}

/* CMD2 与 CMD1 偏移不同，因此保留独立适配函数，避免把两个原版结构硬抽象成错误的同构结构。 */
static void* sub2_row_button(int row) {
    return submenu_row_button_from_sub(BCTX_CMD2, battle_subui(BCTX_CMD2), row);
}

/* 根据总项目数、当前页和每页 8 行计算这一页真正存在多少行。 */
static int visible_rows(int ctx) {
    u8* sub = battle_subui(ctx);
    int page, total, remain, n;
    if (!sub) return 0;
    if (ctx == BCTX_CMD1) {
        page = *(i32*)(sub + SUB1_CURRENT_PAGE);
        total = *(i32*)(sub + SUB1_TOTAL_ITEMS);
    } else if (ctx == BCTX_CMD2) {
        page = *(i32*)(sub + SUB2_CURRENT_PAGE);
        total = *(i32*)(sub + SUB2_TOTAL_ITEMS);
    } else return 0;
    if (page < 0 || total <= 0) return 0;
    remain = total - page * SUB_ROWS_COUNT;
    if (remain <= 0) return 0;
    n = remain > SUB_ROWS_COUNT ? SUB_ROWS_COUNT : remain;
    return n;
}

/* 只有处于本页可见范围且能取得真实 Button 的行才允许成为逻辑焦点。 */
static int row_is_valid(int ctx, int row) {
    void* b;
    int n = visible_rows(ctx);
    if (row < 0 || row >= n) return 0;
    b = (ctx == BCTX_CMD1) ? sub1_row_button(row) : sub2_row_button(row);
    return button_usable(b);
}

/* 从上往下找第一个可用行，用于新进入列表或翻到下一页后的落点。 */
static int first_valid_row(int ctx) {
    int i;
    for (i = 0; i < SUB_ROWS_COUNT; ++i) {
        if (row_is_valid(ctx, i)) return i;
    }
    return 0;
}

/* 从下往上找最后一个可用行，用于翻到上一页后的边界落点。 */
static int last_valid_row(int ctx) {
    int i;
    for (i = SUB_ROWS_COUNT - 1; i >= 0; --i) {
        if (row_is_valid(ctx, i)) return i;
    }
    return 0;
}

static void* focused_button(void) {
    u8* sub;
    if (g_context == BCTX_TOP) return top_button(g_top_focus);
    if (g_context == BCTX_CMD0) {
        sub = battle_subui(BCTX_CMD0);
        return sub ? *(void**)(sub + SUB0_MAIN_BUTTON) : NULL;
    }
    if (g_context == BCTX_CMD1) return sub1_row_button(g_cmd1_focus);
    if (g_context == BCTX_CMD2) return sub2_row_button(g_cmd2_focus);
    if (g_context == BCTX_CMD3) {
        sub = battle_subui(BCTX_CMD3);
        return sub ? *(void**)(sub + SUB3_MAIN_BUTTON) : NULL;
    }
    if (g_context == BCTX_CONFIRM) return popup_button(g_popup_focus);
    return NULL;
}

/*
 * 判断一个 HitTest 候选 Button 是否属于指定战斗层。
 * 这个集合边界非常重要：视觉锁存只能过滤自己的菜单，绝不能把其它 UI 的按钮也强制成 0。
 */
static int control_button_in_context(int ctx, void* candidate) {
    int i;
    u8* sub;
    u8* panel;
    if (!candidate) return 0;
    if (ctx == BCTX_TOP) {
        for (i = 0; i < COMMAND_COUNT; ++i) {
            if (top_button(i) == candidate) return 1;
        }
        return 0;
    }
    if (ctx == BCTX_CMD0) {
        sub = battle_subui(BCTX_CMD0);
        return sub && *(void**)(sub + SUB0_MAIN_BUTTON) == candidate;
    }
    if (ctx == BCTX_CMD1) {
        sub = battle_subui(BCTX_CMD1);
        if (!sub) return 0;
        for (i = 0; i < SUB_ROWS_COUNT; ++i) {
            if (sub1_row_button(i) == candidate) return 1;
        }
        panel = *(u8**)(sub + SUB1_ROW_PANEL);
        if (Runtime_PtrOk(panel) && (*(void**)(panel+0x5DCu)==candidate || *(void**)(panel+0x5E0u)==candidate)) return 1;
        panel = *(u8**)(sub + SUB1_CATEGORY_PANEL);
        if (Runtime_PtrOk(panel)) {
            for (i = 0; i < 4; ++i) {
                if (*(void**)(panel + 0x584u + (u32)i * 4u) == candidate) return 1;
            }
        }
        return *(void**)(sub + SUB1_CANCEL_BUTTON) == candidate;
    }
    if (ctx == BCTX_CMD2) {
        sub = battle_subui(BCTX_CMD2);
        if (!sub) return 0;
        for (i = 0; i < SUB_ROWS_COUNT; ++i) {
            if (sub2_row_button(i) == candidate) return 1;
        }
        panel = *(u8**)(sub + SUB2_ROW_PANEL);
        if (Runtime_PtrOk(panel) && (*(void**)(panel+0x5FCu)==candidate || *(void**)(panel+0x600u)==candidate)) return 1;
        panel = *(u8**)(sub + SUB2_CATEGORY_PANEL);
        if (Runtime_PtrOk(panel)) {
            for (i = 0; i < 2; ++i) {
                if (*(void**)(panel + 0x57Cu + (u32)i * 4u) == candidate) return 1;
            }
        }
        return *(void**)(sub + SUB2_CANCEL_BUTTON) == candidate;
    }
    if (ctx == BCTX_CMD3) {
        sub = battle_subui(BCTX_CMD3);
        return sub && *(void**)(sub + SUB3_MAIN_BUTTON) == candidate;
    }
    if (ctx == BCTX_CONFIRM) {
        return popup_button(0) == candidate || popup_button(1) == candidate;
    }
    return 0;
}

/* 当前 Context 的简写包装，供稳定菜单视觉焦点分发使用。 */
static int active_control_button(void* candidate) {
    return control_button_in_context(g_context, candidate);
}

static void visual_latch_clear(const char* reason) {
    if (!g_visual_latch_active) return;
    if (reason) {
        char line[192]; SIZE_T p=0;
        append_text(line,sizeof(line),&p,"[战斗视觉锁存] 已清除：");
        append_text(line,sizeof(line),&p,reason);
        Runtime_Log(line);
    }
    g_visual_latch_active = 0;
    g_visual_latch_context = BCTX_NONE;
    g_visual_latch_button = NULL;
    g_visual_latch_seen = 0;
    g_visual_latch_arm_tick = 0;
    g_visual_latch_last_seen_tick = 0;
}

static void visual_latch_arm(int ctx, void* button, const char* reason) {
    if (!Runtime_PtrOk(button) || ctx == BCTX_NONE || ctx == BCTX_TARGET) return;
    g_visual_latch_active = 1;
    g_visual_latch_context = ctx;
    g_visual_latch_button = button;
    g_visual_latch_arm_tick = Runtime_Tick();
    g_visual_latch_last_seen_tick = Runtime_Tick();
    g_visual_latch_seen = 0;
    {
        char line[224]; SIZE_T p=0;
        append_text(line,sizeof(line),&p,"[战斗视觉锁存] 已建立，层=");
        append_text(line,sizeof(line),&p,context_name(ctx));
        append_text(line,sizeof(line),&p,"，按钮=");
        append_hex32(line,sizeof(line),&p,(u32)(SIZE_T)button);
        if (reason) { append_text(line,sizeof(line),&p,"，原因="); append_text(line,sizeof(line),&p,reason); }
        Runtime_Log(line);
    }
}

static void visual_latch_arm_top(const char* reason) {
    void* b;
    if (g_top_focus < 0 || g_top_focus >= COMMAND_COUNT) return;
    b = top_button(g_top_focus);
    visual_latch_arm(BCTX_TOP,b,reason);
}

static void visual_latch_maintenance(void) {
    u32 age, unseen;
    if (!g_visual_latch_active) return;
    age = Runtime_Tick() - g_visual_latch_arm_tick;
    unseen = Runtime_Tick() - g_visual_latch_last_seen_tick;
    if (g_visual_latch_seen && unseen >= Runtime_MsToTicks(VISUAL_LATCH_GONE_MS)) {
        visual_latch_clear("锁存菜单已停止 HitTest（已经隐藏）");
        return;
    }
    if (age >= Runtime_MsToTicks(VISUAL_LATCH_MAX_MS)) {
        visual_latch_clear("达到安全最大时限");
    }
}


/*
 * 清除“取消返回父层”的视觉恢复事务。
 *
 * 注意：这不是 g_visual_latch 的别名。
 * - visual_latch 负责“旧菜单正在消失时还要保持刚才选中的旧按钮”；
 * - return_visual 负责“父菜单重新出现时第一帧就显示真正的逻辑焦点”。
 * 两者可能在同一个过渡阶段同时存在，而且分别处理不同菜单对象。
 */
static void return_visual_clear(const char* reason) {
    if (!g_return_visual.active) return;
    if (reason) {
        char line[224]; SIZE_T pos=0;
        append_text(line,sizeof(line),&pos,"[战斗视觉] 返回恢复事务结束：");
        append_text(line,sizeof(line),&pos,reason);
        Runtime_Log(line);
    }
    g_return_visual.active = 0;
    g_return_visual.target_context = BCTX_NONE;
    g_return_visual.target_button = NULL;
    g_return_visual.seen_target_hit = 0;
    g_return_visual.arm_tick = 0;
}

/*
 * 在真正发送 B/取消事件“之前”就保存父层按钮。
 * 这是修复 dev20 视觉延迟的关键：如果等 detect_context() 已经回到父层后才保存，
 * 原版父层可能已经先执行过一轮 HitTest，于是第一轮仍会显示默认第 1 项。
 */
static void return_visual_arm(int target_context, void* target_button, const char* reason) {
    char line[256]; SIZE_T pos=0;
    if (target_context == BCTX_NONE || target_context == BCTX_TARGET || !Runtime_PtrOk(target_button)) return;
    g_return_visual.active = 1;
    g_return_visual.target_context = target_context;
    g_return_visual.target_button = target_button;
    g_return_visual.seen_target_hit = 0;
    g_return_visual.arm_tick = Runtime_Tick();

    append_text(line,sizeof(line),&pos,"[战斗视觉] 已预先锁定取消返回后的父层焦点，层=");
    append_text(line,sizeof(line),&pos,context_name(target_context));
    append_text(line,sizeof(line),&pos,"，按钮=");
    append_hex32(line,sizeof(line),&pos,(u32)(SIZE_T)target_button);
    if (reason) {
        append_text(line,sizeof(line),&pos,"，原因=");
        append_text(line,sizeof(line),&pos,reason);
    }
    Runtime_Log(line);
}

/* 返回到战斗顶层时，父层按钮就是当前保存的 g_top_focus。 */
static void return_visual_arm_top(const char* reason) {
    void* button;
    if (g_top_focus < 0 || g_top_focus >= COMMAND_COUNT) return;
    button = top_button(g_top_focus);
    if (!Runtime_PtrOk(button)) return;

    /*
     * 先建立事务，再立即同步原版 marker。建立事务负责后续 HitTest 过滤；直接 marker 同步负责消除
     * dev20 已知的“逻辑已经恢复、视觉还在第 1 项等待 0.5~1 秒”的空窗。两者缺一不可。
     */
    return_visual_arm(BCTX_TOP, button, reason);
    top_visual_sync_marker_to_button(button, reason);
}

/*
 * CONFIRM / TARGET 是覆盖在列表或顶层之上的“临时层”。
 * 进入它们之前会保存真实父层对象；B 或“否”返回时直接恢复这份父层视觉。
 */
static void return_visual_arm_overlay_parent(const char* reason) {
    if (g_overlay_parent_context == BCTX_NONE || !Runtime_PtrOk(g_overlay_parent_button)) return;
    return_visual_arm(g_overlay_parent_context, g_overlay_parent_button, reason);
}

/*
 * 返回事务不依赖固定延时来“等菜单稳定”。只要：
 * 1. 已经看见父层目标 Button 的真实 HitTest；
 * 2. worker 也确认当前 Context 已回到目标父层；
 * 就说明视觉与逻辑已经在真实原版生命周期内重新汇合，可以结束。
 *
 * 1200 ms 只是保险丝：若原版异常中止过渡，不允许一个旧事务永久劫持未来菜单。
 */
static void return_visual_maintenance(void) {
    u32 age;
    if (!g_return_visual.active) return;
    age = Runtime_Tick() - g_return_visual.arm_tick;

    /*
     * 顶层返回期间原版其它动画/初始化代码仍可能再次把 marker 写回第 1 项。
     * 因此在事务真正闭合以前，每个 worker tick 都重复一次“原版最终赋值”。最多只持续 1200ms，
     * 正常情况下在父层首个真实 HitTest 后很快结束，不会长期接管原版视觉。
     */
    if (g_return_visual.target_context == BCTX_TOP && Runtime_PtrOk(g_return_visual.target_button)) {
        top_visual_sync_marker_to_button(g_return_visual.target_button, NULL);
    }

    if (g_return_visual.seen_target_hit && g_context == g_return_visual.target_context &&
        age >= Runtime_MsToTicks(16u)) {
        return_visual_clear("父层首帧 HitTest 已命中正确按钮，逻辑 Context 也已恢复");
        return;
    }
    if (age >= Runtime_MsToTicks(1200u)) {
        return_visual_clear("安全超时，避免旧事务污染未来菜单");
    }
}

/*
 * 原版 Target mode 决定允许扫描哪一侧阵位：0=0..7，1=20..27，2=0..27。
 * 这里严格使用原版边界，避免“方向导航方便”反而跨阵营选到非法对象。
 */
static int target_mode_bounds(u8* t, int* out_begin, int* out_end) {
    int mode;
    if (!t || !out_begin || !out_end) return 0;
    mode = *(i32*)(t + TARGET_MODE);
    if (mode == 0) {
        *out_begin = 0;
        *out_end = 8;
        return 1;
    }
    if (mode == 1) {
        *out_begin = 20;
        *out_end = 28;
        return 1;
    }
    if (mode == 2) {
        *out_begin = 0;
        *out_end = 28;
        return 1;
    }
    return 0;
}

/*
 * 对单个阵位执行 dev15 的合法性硬限制：模式范围、角色 ID、生命/可选状态、特殊排除状态都必须通过。
 * 方向导航只在这个合法集合里移动。
 */
static int target_cell_is_legal(u8* t, int cell) {
    u8* roles;
    u8* role;
    int begin, end, id;
    if (!t || cell < 0 || cell >= TARGET_MAP_COUNT) return 0;
    if (!target_mode_bounds(t, &begin, &end)) return 0;
    if (cell < begin || cell >= end) return 0;

    id = *(i32*)(t + TARGET_MAP_BASE + (u32)cell * 4u);
    if (id < 0 || id >= MAX_BATTLE_ROLE_ID) return 0;
    roles = *(u8**)(t + TARGET_ROLE_BASE_PTR);
    if (!Runtime_PtrOk(roles)) return 0;
    role = roles + (u32)id * FIGHTROLE_STRIDE;
    if (!Runtime_PtrOk(role)) return 0;

    /* 与原版 0x445FB0 -> 0x445EA0 构建 +0xFB4 map 的基础资格保持一致。 */
    if (*(i32*)(role + 0x72Cu) <= 0) return 0;
    if (*(u32*)(role + FIGHTROLE_SPECIAL_STATE) == FIGHTROLE_SPECIAL_EXCLUDE) return 0;
    return 1;
}

static void target_log_filter_state(u8* t, int count) {
    char line[200]; SIZE_T p=0;
    int mode, begin=-1, end=-1;
    if (!t) return;
    mode=*(i32*)(t+TARGET_MODE);
    if (mode == g_target_filter_last_mode && count == g_target_filter_last_count) return;
    g_target_filter_last_mode=mode;
    g_target_filter_last_count=count;
    target_mode_bounds(t,&begin,&end);
    append_text(line,sizeof(line),&p,"[战斗目标过滤] 模式="); append_hex32(line,sizeof(line),&p,(u32)mode);
    append_text(line,sizeof(line),&p,"，扫描范围="); append_hex32(line,sizeof(line),&p,(u32)begin);
    append_text(line,sizeof(line),&p,".."); append_hex32(line,sizeof(line),&p,(u32)end);
    append_text(line,sizeof(line),&p,"，合法候选数="); append_hex32(line,sizeof(line),&p,(u32)count);
    Runtime_Log(line);
}

/*
 * 从原版 +0xFB4 map 构建去重后的合法阵位列表。
 * 同一角色如果在映射中重复出现，只保留第一个，避免方向键在同一对象之间“原地跳”。
 */
static int target_build_cells(int* cells, int cap) {
    u8* t = target_ptr();
    int count = 0, i, j, id, dup, begin, end;
    if (!t || !cells || cap <= 0) return 0;
    if (!target_mode_bounds(t, &begin, &end)) {
        target_log_filter_state(t, 0);
        return 0;
    }
    for (i = begin; i < end && count < cap; ++i) {
        if (!target_cell_is_legal(t, i)) continue;
        id = *(i32*)(t + TARGET_MAP_BASE + (u32)i * 4u);
        dup = 0;
        for (j = 0; j < count; ++j) {
            int oldid = *(i32*)(t + TARGET_MAP_BASE + (u32)cells[j] * 4u);
            if (oldid == id) { dup = 1; break; }
        }
        if (!dup) cells[count++] = i;
    }
    target_log_filter_state(t, count);
    return count;
}

/*
 * 进入 Target 时先把手柄 cell 与原版当前 TARGET_CURRENT_ID 对齐。
 * 找不到当前角色时才回落到合法集合第一项，绝不凭空制造不存在的 cell。
 */
static void target_sync_initial_cell(void) {
    u8* t = target_ptr();
    int cells[TARGET_MAP_COUNT];
    int n, i, current;
    if (!t) { g_target_cell = -1; return; }
    n = target_build_cells(cells, TARGET_MAP_COUNT);
    if (n <= 0) { g_target_cell = -1; return; }
    current = *(i32*)(t + TARGET_CURRENT_ID);
    for (i = 0; i < n; ++i) {
        if (*(i32*)(t + TARGET_MAP_BASE + (u32)cells[i]*4u) == current) {
            g_target_cell = cells[i];
            return;
        }
    }
    g_target_cell = cells[0];
}

static void log_context_transition(int from, int to) {
    char line[180]; SIZE_T p = 0;
    append_text(line,sizeof(line),&p,"[战斗层级] ");
    append_text(line,sizeof(line),&p,context_name(from));
    append_text(line,sizeof(line),&p," -> ");
    append_text(line,sizeof(line),&p,context_name(to));
    Runtime_Log(line);
}

/*
 * 这是 Target 最关键的游戏线程 Hook。先调用原版保留鼠标路径，再消费一次性 A/B 动作；
 * 最后仅在手柄导航激活且当前 cell 合法时覆盖网格索引。
 */
static i32 FASTCALL Battle_HookTargetGridIndex(void* selector, void* unused_edx, i32 a, i32 b) {
    PFN_TargetGridThis orig = (PFN_TargetGridThis)FN_TARGET_GRID_INDEX;
    i32 real_index;
    (void)unused_edx;
    /* 先跑原函数，保留实体鼠标/R3 鼠标通道的原生事务。
     * 手柄 A/B 不再制造鼠标事件，而在同一游戏线程把 selector 的一次性动作写入 +0x10E0。 */
    real_index = orig(selector, a, b);

    /*
     * 一次性 A/B 事务必须由“真正消费它的游戏线程对象”决定是否还有效，不能再依赖 worker 每 8ms
     * 采样出来的 g_context。Target 选择器自己的 +TARGET_ACTIVE 才是这里最权威的生命周期标记。
     *
     * refactor1 仍用 g_context==BCTX_TARGET 作为消费门槛，一旦 Cursor/worker 在两个线程之间恰好先改变了
     * Context，已经排队的确认/取消就可能永远到不了 selector+TARGET_EVENT_CODE。这里改为：只要当前这个真实 selector
     * 仍处于 active，就让它消费一次 pending；离开 Target 后的清理仍由 Context 转换负责。
     */
    if (g_target_event_code != 0 && Runtime_PtrOk(selector) &&
        *(u8*)((u8*)selector + TARGET_ACTIVE) != 0) {
        int code = g_target_event_code;
        g_target_event_code = 0;
        *(i32*)((u8*)selector + TARGET_EVENT_CODE) = code;
        Runtime_Log(code > 0 ?
            "[战斗目标事件] 游戏线程已消费 A 确认事务并写入 selector+TARGET_EVENT_CODE。" :
            "[战斗目标事件] 游戏线程已消费 B 取消事务并写入 selector+TARGET_EVENT_CODE。");
    }
    if (g_nav_active && g_context == BCTX_TARGET) {
        u8* t = (u8*)selector;
        /* dev15：任何 mode/阵营/对象状态变化后都重新验证当前 cell。
         * 非法时只允许回落到原版当前合法集合；若集合为空则完全不覆盖 real_index。 */
        if (!target_cell_is_legal(t, g_target_cell)) target_sync_initial_cell();
        if (target_cell_is_legal(t, g_target_cell)) return g_target_cell;
    }
    return real_index;
}

static void log_native_hit_activity(void) {
    static u32 last_count;
    if (g_forced_hit_calls != last_count) {
        if ((g_forced_hit_calls - last_count) >= 8u || last_count == 0u) {
            char line[160]; SIZE_T p = 0;
            append_text(line,sizeof(line),&p,"[战斗原生视觉] 强制命中累计次数=");
            append_hex32(line,sizeof(line),&p,g_forced_hit_calls);
            Runtime_Log(line);
            last_count = g_forced_hit_calls;
        }
    }
}

static void log_focus(const char* tag, int value) {
    char line[160]; SIZE_T p=0;
    append_text(line,sizeof(line),&p,"[战斗导航] ");
    append_text(line,sizeof(line),&p,tag);
    append_text(line,sizeof(line),&p,"=");
    append_hex32(line,sizeof(line),&p,(u32)value);
    Runtime_Log(line);
}

/*
 * “激活手柄导航”只做状态切换与焦点边界修正，不直接触发原版业务事件。
 * 这样导航获得光标所有权与 A/B 业务动作仍保持可独立排查。
 */
static void activate_nav(void) {
    /*
     * 主动导航一定结束“旧菜单退场锁存”，因为用户已经开始新一轮操作。
     * 但这里故意不清 return_visual：B 本身也会调用 activate_nav()，而返回事务正是在 B 时建立。
     * 真正的新方向/分类导航会在各自入口明确清掉返回事务。
     */
    visual_latch_clear("开始新的手柄导航");

    /*
     * “用户刚刚按了战斗导航键”本身就是最强的所有权证据。
     * refactor4 在这里统一声明光标归手柄，而不是要求每个 CMD1/CMD2/Target 分支自己记得再调用一次。
     * 这样从 Target/确认框按 B 返回父列表的同一 tick，列表动画若尝试把真实鼠标 warp 到默认行，
     * 新增的专属 Hook 就能可靠识别“此刻仍是手柄导航”，从而只在这一 Context 下覆盖原版鼠标行为。
     */
    Cursor_ClaimForControllerNavigation();
    g_nav_active = 1;
    if (g_context == BCTX_TOP && (g_top_focus < 0 || g_top_focus >= COMMAND_COUNT)) g_top_focus = 0;
    if (g_context == BCTX_CMD1 && !row_is_valid(BCTX_CMD1,g_cmd1_focus)) g_cmd1_focus = first_valid_row(BCTX_CMD1);
    if (g_context == BCTX_CMD2 && !row_is_valid(BCTX_CMD2,g_cmd2_focus)) g_cmd2_focus = first_valid_row(BCTX_CMD2);
    if (g_context == BCTX_CONFIRM && (g_popup_focus < 0 || g_popup_focus > 1)) g_popup_focus = 0;
    if (g_context == BCTX_TARGET && g_target_cell < 0) target_sync_initial_cell();
}

static void* category_button(int ctx, int desired) {
    u8* sub = battle_subui(ctx);
    u8* panel;
    void* b;
    if (!sub) return NULL;
    if (ctx == BCTX_CMD1) {
        if (desired < 0 || desired >= 4) return NULL;
        panel = *(u8**)(sub + SUB1_CATEGORY_PANEL);
        if (!Runtime_PtrOk(panel)) return NULL;
        b = *(void**)(panel + 0x584u + (u32)desired*4u);
    } else if (ctx == BCTX_CMD2) {
        if (desired < 0 || desired >= 2) return NULL;
        panel = *(u8**)(sub + SUB2_CATEGORY_PANEL);
        if (!Runtime_PtrOk(panel)) return NULL;
        b = *(void**)(panel + 0x57Cu + (u32)desired*4u);
    } else return NULL;
    return button_usable(b) ? b : NULL;
}

static int current_category(int ctx) {
    u8* sub = battle_subui(ctx);
    u8* panel;
    if (!sub) return 0;
    if (ctx == BCTX_CMD1) return *(i32*)(sub + SUB1_CURRENT_CATEGORY);
    if (ctx == BCTX_CMD2) {
        panel = *(u8**)(sub + SUB2_CATEGORY_PANEL);
        if (Runtime_PtrOk(panel)) return *(i32*)(panel + 0x584u);
    }
    return 0;
}

/*
 * LB/RB 切大类时仍然点击原版分类 Button。
 * 会跳过不可用分类，并把列表焦点重置到第一行，真正换类仍由原版 Event 完成。
 */
static void request_category_delta(int ctx, int delta) {
    int count = ctx == BCTX_CMD1 ? 4 : 2;
    int cur = current_category(ctx);
    int step;
    int desired;
    void* b = NULL;
    if (UiBridge_EventPending()) return;
    for (step = 1; step <= count; ++step) {
        desired = (cur + (delta > 0 ? step : -step)) % count;
        if (desired < 0) desired += count;
        b = category_button(ctx,desired);
        if (b) break;
    }
    if (!b) return;
    if (ctx == BCTX_CMD1) {
        g_cmd1_focus = 0;
    } else {
        g_cmd2_focus = 0;
    }
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, b, 2, delta > 0 ? "切换到下一个大类" : "切换到上一个大类");
}

static void* page_button(int ctx, int next) {
    u8* sub = battle_subui(ctx);
    u8* panel;
    void* b;
    if (!sub) return NULL;
    if (ctx == BCTX_CMD1) {
        panel = *(u8**)(sub + SUB1_ROW_PANEL);
        if (!Runtime_PtrOk(panel)) return NULL;
        b = *(void**)(panel + (next ? 0x5E0u : 0x5DCu));
    } else if (ctx == BCTX_CMD2) {
        panel = *(u8**)(sub + SUB2_ROW_PANEL);
        if (!Runtime_PtrOk(panel)) return NULL;
        b = *(void**)(panel + (next ? 0x600u : 0x5FCu));
    } else return NULL;
    return Runtime_PtrOk(b) ? b : NULL;
}

static int current_page(int ctx) {
    u8* sub = battle_subui(ctx);
    if (!sub) return 0;
    return *(i32*)(sub + (ctx==BCTX_CMD1?SUB1_CURRENT_PAGE:SUB2_CURRENT_PAGE));
}

static int total_pages(int ctx) {
    u8* sub = battle_subui(ctx);
    if (!sub) return 0;
    return *(i32*)(sub + (ctx==BCTX_CMD1?SUB1_TOTAL_PAGES:SUB2_TOTAL_PAGES));
}

/*
 * D-Pad 左右直接翻页只在 CMD1/CMD2 的技能/道具列表 Context 生效。
 *
 * 这里绝不能直接给 current-page 加减：原版的上一页/下一页 ButtonEvent 还负责刷新列表内容、
 * 更新页码、播放声音以及同步其它 UI 字段。插件只点击原版分页 Button，并沿用现有 pending-page 握手。
 *
 * 与“↑/↓ 到边界自动翻页”的差别只有新页落点：
 * - 自动下一页从第 1 行继续，自动上一页从最后可用行继续；
 * - 用户明确按 ←/→ 翻页时，尽量保留当前行号，这样查看长技能/道具列表时不会每翻一页都跳回顶部；
 * - 如果新页没有那么多行，apply_pending_page_focus() 会把行号夹到该页最后一个真实可用行。
 */
static int request_direct_page_delta(int ctx, int delta) {
    int page;
    int pages;
    int* focus;
    void* pb;

    if (ctx != BCTX_CMD1 && ctx != BCTX_CMD2) return 0;
    if (delta == 0 || UiBridge_EventPending() || g_pending_page_context != 0) return 0;

    page = current_page(ctx);
    pages = total_pages(ctx);
    if (page < 0 || pages <= 0) return 0;

    /* 第一页再按 ←、最后一页再按 → 时什么都不做，不伪造循环翻页。 */
    if (delta < 0) {
        if (page <= 0) return 0;
        pb = page_button(ctx, 0);
    } else {
        if (page + 1 >= pages) return 0;
        pb = page_button(ctx, 1);
    }
    if (!Runtime_PtrOk(pb)) return 0;

    focus = (ctx == BCTX_CMD1) ? &g_cmd1_focus : &g_cmd2_focus;
    if (!row_is_valid(ctx, *focus)) *focus = first_valid_row(ctx);

    /*
     * 保存“翻页前所在行”，真正页码变化以后再决定新页焦点。
     * 这和旧的自动跨页使用同一个事务变量，因此整个 Battle 永远只有一套分页状态机。
     */
    g_pending_row_after_page = *focus;
    g_pending_page_context = ctx;
    g_pending_page_old = page;
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, pb, 2, delta < 0 ? "D-Pad 左：原版上一页" : "D-Pad 右：原版下一页");
    return 1;
}

/*
 * 纵向列表先尝试同页下一/上一合法行；到边界后才请求原版分页。
 * 返回 1=同页移动，2=已发分页请求，0=没有可执行动作，调用者据此决定是否开始视觉握手。
 */
static int move_list_row(int ctx, int delta) {
    int* focus = (ctx == BCTX_CMD1) ? &g_cmd1_focus : &g_cmd2_focus;
    int row = *focus;
    int n = visible_rows(ctx);
    int i;
    int candidate;
    int page = current_page(ctx);
    int pages = total_pages(ctx);
    void* pb;
    if (n <= 0) return 0;
    if (!row_is_valid(ctx, row)) row = first_valid_row(ctx);

    /* 先在本页内部找下一条真实可用行。 */
    for (i = 1; i <= SUB_ROWS_COUNT; ++i) {
        candidate = row + (delta > 0 ? i : -i);
        if (candidate >= 0 && candidate < n && row_is_valid(ctx, candidate)) {
            *focus = candidate;
            log_focus(ctx == BCTX_CMD1 ? "命令1列表行" : "命令2列表行", candidate);
            return 1;
        }
    }

    /* 本页已经到底/到顶，再请求原版分页 Button。 */
    if (delta > 0 && page + 1 < pages) {
        pb = page_button(ctx, 1);
        if (pb) {
            *focus = 0;
            g_pending_row_after_page = 0;
            g_pending_page_context = ctx;
            g_pending_page_old = page;
            UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, pb, 2, "列表到底后自动下一页");
            return 2;
        }
    } else if (delta < 0 && page > 0) {
        pb = page_button(ctx, 0);
        if (pb) {
            *focus = SUB_ROWS_COUNT - 1;
            g_pending_row_after_page = -2;
            g_pending_page_context = ctx;
            g_pending_page_old = page;
            UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, pb, 2, "列表到顶后自动上一页");
            return 2;
        }
    }
    return 0;
}

/*
 * 分页请求发出后不立即假定成功；观察原版 current-page 真变化后再把焦点落到新页边界。
 * 这使第一页/最后一页的拒绝行为完全由原版决定。
 */
static void apply_pending_page_focus(void) {
    int nowpage;
    if (g_pending_page_context == 0) return;
    if (g_context != g_pending_page_context) {
        /* 分页过程中离开了原列表，旧分页事务必须立即作废。 */
        g_pending_page_context = 0;
        g_pending_page_old = -1;
        return;
    }
    nowpage = current_page(g_pending_page_context);
    if (nowpage == g_pending_page_old) return;
    if (g_pending_page_context==BCTX_CMD1) {
        if (g_pending_row_after_page == -2) {
            g_cmd1_focus = last_valid_row(BCTX_CMD1);
        } else if (g_pending_row_after_page >= 0 && row_is_valid(BCTX_CMD1, g_pending_row_after_page)) {
            g_cmd1_focus = g_pending_row_after_page;
        } else {
            /* 新页行数少于旧页时，保留行号失败就退到本页最后一个真实可用行。 */
            g_cmd1_focus = last_valid_row(BCTX_CMD1);
            if (g_cmd1_focus < 0) g_cmd1_focus = first_valid_row(BCTX_CMD1);
        }
    } else if (g_pending_page_context==BCTX_CMD2) {
        if (g_pending_row_after_page == -2) {
            g_cmd2_focus = last_valid_row(BCTX_CMD2);
        } else if (g_pending_row_after_page >= 0 && row_is_valid(BCTX_CMD2, g_pending_row_after_page)) {
            g_cmd2_focus = g_pending_row_after_page;
        } else {
            g_cmd2_focus = last_valid_row(BCTX_CMD2);
            if (g_cmd2_focus < 0) g_cmd2_focus = first_valid_row(BCTX_CMD2);
        }
    }
    g_pending_page_context = 0;
    g_pending_row_after_page = -1;
    g_pending_page_old = -1;
    /* 翻页后的新页首/页尾也必须完成一次原版可见 hover，再继续处理排队输入。 */
    nav_begin_settle();
}

static int target_cursor_position_for_cell(int cell, i32* out_x, i32* out_y) {
    u8* t = target_ptr();
    u8* role_base;
    u8* role;
    i32* xs;
    i32* ys;
    int role_id;
    int slot;
    i32 x;
    i32 y;
    if (!t || cell < 0 || cell >= TARGET_MAP_COUNT || !out_x || !out_y) return 0;

    /* 复用原版 Target 初始化/切换使用的真实鼠标定位链：
       target cell -> role id -> FightRole +0xB9C formation slot
       -> 0x89FDF0[slot] X / 0x89FDEC[slot] Y -> 0x43DF30(SetCursorPos)。
       这比 selector 的特效绘制 X/Y 表更准确，也正是游戏自己把鼠标移到可选角色上的协议。 */
    role_id=*(i32*)(t+TARGET_MAP_BASE+(u32)cell*4u);
    if (role_id < 0 || role_id >= MAX_BATTLE_ROLE_ID) return 0;
    role_base=*(u8**)(t+TARGET_ROLE_BASE_PTR);
    if (!Runtime_PtrOk(role_base)) return 0;
    role=role_base + (u32)role_id * FIGHTROLE_STRIDE;
    if (!Runtime_PtrOk(role)) return 0;
    slot=*(i32*)(role+FIGHTROLE_FORMATION_SLOT);
    if (slot < 0 || slot > 63) return 0;
    xs=*(i32**)GLOBAL_BATTLE_CURSOR_X_TABLE;
    ys=*(i32**)GLOBAL_BATTLE_CURSOR_Y_TABLE;
    if (!Runtime_PtrOk(xs) || !Runtime_PtrOk(ys)) return 0;
    x=xs[slot]; y=ys[slot];
    if (x < -4096 || x > 8192 || y < -4096 || y > 8192) return 0;
    *out_x=x; *out_y=y;
    return 1;
}

/* 仅在“配置允许 + 手柄拥有光标 + 当前是 Target”三条件同时成立时显示目标指示光标。 */
static void target_cursor_sync(void) {
    i32 x=0, y=0;
    /*
     * 只有“手柄仍拥有指针 + 当前真的是 Target + 配置允许”三个条件同时成立，
     * 才显示原版目标指示光标。普通战斗菜单永远不会从这里把鼠标重新显示出来。
     */
    if (!Runtime_Config()->target_cursor_indicator) return;
    if (!Cursor_ControllerOwnsPointer()) return;
    if (g_context != BCTX_TARGET) return;
    if (!target_cursor_position_for_cell(g_target_cell,&x,&y)) return;
    Cursor_ShowTargetAt(x,y);
}

/* Target 一结束就立刻撤销目标指示器，不等待右摇杆的普通显示超时。 */
static void target_cursor_hide_immediate(void) {
    /* 离开 Target、取消 Target、或实体鼠标接管时，都立即撤销目标指示状态。 */
    Cursor_HideTargetImmediately();
}


/*
 * 把 Battle Target 的四方向输入转换成纯几何层方向枚举。
 * 业务层仍然只知道“玩家按了哪个方向”；具体怎样选邻居统一交给 spatial_neighbor.c。
 */
static SpatialDirection target_spatial_direction(int dx, int dy) {
    if (dx < 0) return SPATIAL_LEFT;
    if (dx > 0) return SPATIAL_RIGHT;
    if (dy < 0) return SPATIAL_UP;
    return SPATIAL_DOWN;
}

/*
 * 如果某些目标暂时无法取得原版鼠标阵位坐标，就退回 selector 的 4×7 逻辑格。
 *
 * 注意这里也必须使用“正常二维语义”：
 * - cell / 4 是列；
 * - cell % 4 是行；
 * - 左右改变列；
 * - 上下改变行。
 *
 * 旧代码故意把左右与 row、上下与 column 交叉，这是用户现在实机指出的错误行为来源之一。
 * r23 起删除交叉轴；r24 的 shared SpatialNeighbor 改为“方向半平面 + 副轴对齐优先”，因此 fallback 与屏幕路径语义一致。
 */
static int target_move_grid_fallback(int dx, int dy, const int* cells, int n) {
    SpatialPoint points[TARGET_MAP_COUNT];
    int i;

    if (!cells || n <= 0 || n > TARGET_MAP_COUNT || g_target_cell < 0) return -1;

    for (i = 0; i < n; ++i) {
        int cell = cells[i];
        points[i].id = cell;
        points[i].x = (i32)(cell / TARGET_GRID_ROWS);
        points[i].y = (i32)(cell % TARGET_GRID_ROWS);
    }

    return SpatialNeighbor_Find(points, n, g_target_cell, target_spatial_direction(dx, dy));
}

/*
 * Battle Target 的方向选择：恢复 r22 已经成熟的“真实屏幕空间优先”，只修真正错误的 fallback。
 *
 * r23 的回归来自两点：
 * 1. shared SpatialNeighbor 被收紧成 45° 互斥扇区，斜排目标的正常上/下邻居会被直接排除；
 * 2. 只要合法集合里有任意一个 cell 暂时取不到鼠标坐标，就把整组目标全部切到 4×7 逻辑网格。
 *    这会让本来已经有可靠屏幕坐标的目标也突然换一套方向体系。
 *
 * r24 的处理顺序因此非常明确：
 * 1. 仍先调用 dev15 已验收的 target_build_cells()，敌我侧、角色状态、合法目标集合完全不改；
 * 2. 当前 cell 能取得原版鼠标坐标时，只把“同样能取得真实坐标”的合法目标交给 SpatialNeighbor_Find；
 * 3. shared 算法使用方向半平面 + 副轴对齐优先，与 r22 的成熟屏幕选择规则一致；
 * 4. 如果屏幕空间已经明确告诉我们“这个方向没有邻居”，就保持不动，不为了凑结果再跳进逻辑网格；
 * 5. 只有当前坐标拿不到，或候选坐标确实存在缺口且屏幕空间无法给答案时，才使用正常轴语义的逻辑格兜底。
 *
 * 这样修的是“方向怎么选”，不是 Battle Target 的其它业务协议。
 * A/B、目标鼠标、selector one-shot、dev15 side mode 与合法集合都保持原样。
 */
static void move_target_direction(int dx, int dy) {
    u8* target = target_ptr();
    int cells[TARGET_MAP_COUNT];
    SpatialPoint points[TARGET_MAP_COUNT];
    int count;
    int point_count = 0;
    int missing_positions = 0;
    int current_position_valid = 0;
    int i;
    int best = -1;
    int role_id;
    const char* source_text = "，来源=原版阵位坐标";
    i32 current_x = 0;
    i32 current_y = 0;

    if (!target || (dx == 0 && dy == 0)) return;

    count = target_build_cells(cells, TARGET_MAP_COUNT);
    if (count <= 0) return;

    if (g_target_cell < 0) target_sync_initial_cell();
    if (g_target_cell < 0) return;

    current_position_valid = target_cursor_position_for_cell(g_target_cell, &current_x, &current_y);

    if (current_position_valid) {
        for (i = 0; i < count; ++i) {
            i32 x;
            i32 y;
            int cell = cells[i];

            if (!target_cursor_position_for_cell(cell, &x, &y)) {
                /*
                 * 这个 cell 仍然属于合法集合，只是这一次没有可靠屏幕坐标。
                 * 不把猜坐标混入真实点集；先让其它真实坐标继续参与本次导航。
                 */
                missing_positions = 1;
                continue;
            }

            points[point_count].id = cell;
            points[point_count].x = x;
            points[point_count].y = y;
            ++point_count;
        }

        if (point_count > 0) {
            best = SpatialNeighbor_Find(points, point_count, g_target_cell,
                                        target_spatial_direction(dx, dy));
        }

        /*
         * 所有合法目标都有真实坐标、但这个方向找不到邻居：这就是屏幕边界，正确行为是“不动”。
         * 不能再像旧 fallback 那样从另一套逻辑格里硬找一个看起来方向不一致的目标。
         */
        if (best < 0 && !missing_positions) return;
    }

    if (best < 0) {
        best = target_move_grid_fallback(dx, dy, cells, count);
        source_text = "，来源=正常轴4x7逻辑格兜底";
    }
    if (best < 0 || best == g_target_cell) return;

    g_target_cell = best;
    role_id = *(i32*)(target + TARGET_MAP_BASE + (u32)best * 4u);

    {
        char line[320];
        SIZE_T pos = 0;
        i32 x = 0;
        i32 y = 0;

        append_text(line, sizeof(line), &pos, "[战斗目标] 二维方向导航：cell=");
        append_hex32(line, sizeof(line), &pos, (u32)best);
        append_text(line, sizeof(line), &pos, "，角色ID=");
        append_hex32(line, sizeof(line), &pos, (u32)role_id);
        append_text(line, sizeof(line), &pos, source_text);

        if (target_cursor_position_for_cell(best, &x, &y)) {
            append_text(line, sizeof(line), &pos, "，原版光标坐标=(");
            append_hex32(line, sizeof(line), &pos, (u32)x);
            append_text(line, sizeof(line), &pos, ",");
            append_hex32(line, sizeof(line), &pos, (u32)y);
            append_text(line, sizeof(line), &pos, ")");
        }
        Runtime_Log(line);
    }

    target_cursor_sync();
}


static void mark_cancel_return_expected(void) {
    /* 逻辑层仍保留 dev14 的“这是取消返回，不是全新进入父层”标记。 */
    g_cancel_return_from_context = g_context;
}

static void cancel_cmd0_direct(void) {
    u8* battle = battle_ptr();
    u8* sub = battle_subui(BCTX_CMD0);
    PFN_ThisVoid closefn = (PFN_ThisVoid)FN_CMD0_CLOSE;
    if (!battle || !sub) return;
    closefn(sub);
    *(u8*)(battle+BATTLE_SUBMENU_ACTIVE)=0;
    *(i32*)(battle+BATTLE_ACTIVE_COMMAND)=-1;
    Runtime_Log("[战斗取消] CMD0 已调用原版关闭函数，并恢复父层状态字段。");
}

/*
 * B 的最终业务协议按 Context 分派：Target 写选择器取消码、Popup 点否、列表点原版取消、CMD0 调专属关闭函数。
 * 这里同时在真正取消前建立父层视觉恢复事务，修复 dev20 返回时短暂显示第一项的问题。
 */
static void request_cancel(void) {
    u8* sub; void* b=NULL;
    activate_nav();

    /*
     * 先建立视觉返回事务，再触发原版取消。
     * 这样原版一旦让父层 Button 在同一帧重新参加 HitTest，我们已经知道该命中哪一项。
     */
    if (g_context==BCTX_TARGET) {
        return_visual_arm_overlay_parent("Target 按 B 返回父层");
        mark_cancel_return_expected();
        if (!g_target_event_code) {
            g_target_event_code=-1;
            Runtime_Log("[战斗目标事件] B 取消事务已排队，等待 Target 游戏线程消费。");
        }
        target_cursor_hide_immediate();
        return;
    }
    if (g_context==BCTX_CONFIRM) {
        return_visual_arm_overlay_parent("确认框按 B 返回父层");
        mark_cancel_return_expected();
        b=popup_button(1);
        g_popup_focus=1;
        if(Runtime_PtrOk(b)) UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, b,2,"[战斗确认框] B：执行原版取消按钮。");
        return;
    }
    if (g_context==BCTX_CMD0) {
        return_visual_arm_top("CMD0 按 B 返回战斗主菜单");
        mark_cancel_return_expected();
        cancel_cmd0_direct();
        return;
    }
    if (g_context==BCTX_CMD1) { sub=battle_subui(BCTX_CMD1); if(sub) b=*(void**)(sub+SUB1_CANCEL_BUTTON); }
    else if (g_context==BCTX_CMD2) { sub=battle_subui(BCTX_CMD2); if(sub) b=*(void**)(sub+SUB2_CANCEL_BUTTON); }
    else if (g_context==BCTX_CMD3) { sub=battle_subui(BCTX_CMD3); if(sub) b=*(void**)(sub+SUB3_MAIN_BUTTON); }

    if (Runtime_PtrOk(b)) {
        return_visual_arm_top("子菜单按 B 返回战斗主菜单");
        mark_cancel_return_expected();
        UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, b,1,"[战斗取消] 已请求子菜单原版取消事件。");
    }
}

/*
 * 清掉一次“普通列表 A 确认”的看门狗。
 * 这里只清插件的观察状态，不撤销已经被 RPG.exe 接受的任何游戏业务。
 */
static void confirm_watch_clear(void) {
    g_confirm_watch_active = 0;
    g_confirm_watch_context = BCTX_NONE;
    g_confirm_watch_button = NULL;
    g_confirm_watch_start_tick = 0;
    g_confirm_watch_consumed_tick = 0;
    g_confirm_watch_consumed_seen = 0;
}

/*
 * 只有 CMD1/CMD2 的行确认需要这个看门狗。
 *
 * 为什么不能把“原版没有换 Context”简单当成错误：
 * 灰色/条件不足的技能与道具，ButtonEvent 可能被正常扫描、code=2 也可能被页面 Update 正常收到，
 * 但页面后续业务条件会决定“什么都不做”。这正是原版键鼠的合法行为。
 *
 * 手柄插件真正要保证的是：无动作以后不能留下 UiBridge pending、视觉 settle 或 deferred A。
 * 所以这里分两种失败出口：
 * 1. 事件一直没被原版 CALL 消费：超过 settle timeout 后撤销 Battle 自己的 pending；
 * 2. 事件已经被消费，但经过最小视觉握手时间后仍停在原列表：把它视为原版拒绝/无动作，解锁导航。
 */
static void confirm_watch_maintenance(void) {
    u32 age;

    if (!g_confirm_watch_active) return;

    /* Context 变化说明原版已经接受这次 A 并进入下一层；观察任务自然结束。 */
    if (g_context != g_confirm_watch_context) {
        confirm_watch_clear();
        return;
    }

    age = Runtime_Tick() - g_confirm_watch_start_tick;

    /*
     * 如果玩家在观察期间已经把逻辑焦点移到另一行，旧 A 无论如何都不应该稍后“追上来”点击旧行。
     * 这也是一次输入事务的基本原子性：确认对象必须和按 A 那一刻的对象一致。
     */
    if (Runtime_PtrOk(g_confirm_watch_button) && focused_button() != g_confirm_watch_button) {
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE);
        g_pending_confirm_after_nav = 0;
        nav_settle_clear();
        confirm_watch_clear();
        Runtime_Log("[战斗确认] A 观察期间焦点已改变；旧确认已撤销，避免延迟点击旧项目。");
        return;
    }

    /*
     * 情况一：UiBridge 里仍是 Battle 的同一次事件。
     * 这意味着原版当前帧根本没有扫描到这个 Button；无限保留会挡住后续 B/方向事件。
     */
    if (UiBridge_EventOwner() == UI_EVENT_OWNER_BATTLE) {
        if (age >= g_nav_settle_timeout_ticks) {
            UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE);
            g_pending_confirm_after_nav = 0;
            nav_queue_clear();
            nav_settle_clear();
            confirm_watch_clear();
            Runtime_Log("[战斗确认] A 事务未被原版按钮扫描到；已按超时安全规则撤销，菜单继续可操作。");
        }
        return;
    }

    /*
     * 情况二：UiBridge Event 已经被页面 Update 消费。
     *
     * 这里不能从“玩家按 A 的时刻”直接算视觉宽限期：如果游戏正好晚一帧才扫描 Button，
     * 那么 72ms 宽限可能大半已经耗在“等 Event 被消费”上，正常技能会被过早判成无动作。
     * 因此第一次看到 owner 不再是 BATTLE 时，只记录 consumed_tick；从下一轮开始才计算真正的迁移宽限。
     */
    if (!g_confirm_watch_consumed_seen) {
        g_confirm_watch_consumed_seen = 1;
        g_confirm_watch_consumed_tick = Runtime_Tick();
        return;
    }

    /*
     * 事件已经被页面 Update 消费，但经过完整的最小视觉握手时间仍停在同一个 CMD1/CMD2。
     * 这时才把它视为原版合法的“不可用/业务拒绝/无动作”，只释放插件自己的等待状态。
     */
    if ((Runtime_Tick() - g_confirm_watch_consumed_tick) >= g_nav_settle_min_ticks) {
        g_pending_confirm_after_nav = 0;
        nav_settle_clear();
        confirm_watch_clear();
        Runtime_Log("[战斗确认] 原版消费 A 后经过完整握手窗口仍未进入下一层；按不可用/无动作项处理，手柄导航已解锁。");
    }
}

/*
 * A 不“统一返回 2”粗暴处理，而是按当前 Context 选择对应真实原版协议。
 * Target 写选择器动作码；Popup 点击当前按钮；普通菜单则给当前真实 Button 排一次 Event。
 */
static void request_confirm(void) {
    void* b;

    /* Target/Popup 自己需要立刻取得手柄焦点；普通列表必须先确认 Button 可用再激活，避免“无效 A”改变导航状态。 */
    if (g_context==BCTX_TARGET) {
        activate_nav();
        if (!g_target_event_code) {
            g_target_event_code=1;
            Runtime_Log("[战斗目标事件] A 确认事务已排队，等待 Target 游戏线程消费。");
        }
        target_cursor_hide_immediate();
        return;
    }
    if (g_context==BCTX_CONFIRM) {
        activate_nav();
        /* 双按钮弹窗的两个按钮都要求原版 code=2；A 必须确认当前视觉高亮项。 */
        if (g_popup_focus < 0 || g_popup_focus > 1) g_popup_focus = 0;
        b=popup_button(g_popup_focus);
        if (g_popup_focus==1) {
            return_visual_arm_overlay_parent("确认框选择‘否’后返回父层");
            mark_cancel_return_expected();
        }
        if(Runtime_PtrOk(b)) UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, b,2,g_popup_focus==0 ?
            "[战斗确认框] A：执行当前‘是/确定’按钮。" :
            "[战斗确认框] A：执行当前‘否/取消’按钮。");
        return;
    }
    b=focused_button();

    /*
     * refactor20 修复：禁用技能/道具上按 A 不能产生一个“永远等不到消费”的 pending。
     *
     * 原版 0x431380 本身已经有两条明确的按钮可用性门：
     *   - Button+0x45 == 0：这个按钮当前被原版禁用；
     *   - Button+0x04 != 0：这个按钮当前处于原版不接受 Event 的状态。
     *
     * 旧代码这里只检查“指针看起来有效”，然后直接把 code=2 排到 UiBridge。
     * 对正常按钮没有问题；但某些灰色技能/道具根本不会进入对应 ButtonEvent CALL，
     * pending 就会一直留在 UiBridge。之后玩家按 B 时，取消事件因为“已经有 pending”而排不进去，
     * 体感就是整套手柄输入假死。
     *
     * 这里必须在排队之前复用已经存在的 button_usable()，让手柄和原版键鼠遵守同一套禁用规则。
     * 不可用项按 A 时只忽略这次确认，不修改游戏内存，也不建立任何 pending。
     */
    if (!button_usable(b)) {
        /*
         * 这条是最便宜的第一层拒绝：连原版 Button 自己的基础门都没过，就绝不能调用 activate_nav()、
         * 不能建立 deferred confirm，更不能向 UiBridge 排事件。用户侧结果就是“按 A 完全没反应”。
         */
        Runtime_Log("[战斗确认] 当前项目原版按钮不可用；A 已完全忽略，未改变导航/确认状态。");
        return;
    }

    activate_nav();
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, b,2,"[战斗确认] A：请求当前原版按钮确认事件。");

    /*
     * 只有技能/道具两种纵向列表需要观察“原版业务拒绝但 Context 不变”的情况。
     * 顶层/CMD0/CMD3 的成功路径不同，不把它们硬套进这个列表专用规则。
     */
    if ((g_context == BCTX_CMD1 || g_context == BCTX_CMD2) &&
        UiBridge_EventOwner() == UI_EVENT_OWNER_BATTLE) {
        g_confirm_watch_active = 1;
        g_confirm_watch_context = g_context;
        g_confirm_watch_button = b;
        g_confirm_watch_start_tick = Runtime_Tick();
        g_confirm_watch_consumed_tick = 0;
        g_confirm_watch_consumed_seen = 0;
    }
}


static void request_top_shortcut(int index, const char* tag) {
    void* b;
    if (g_context != BCTX_TOP || index < 0 || index >= COMMAND_COUNT) return;
    b=top_button(index); if(!Runtime_PtrOk(b)) return;
    Cursor_ClaimForControllerNavigation();
    visual_latch_clear("顶层快捷命令");
    return_visual_clear("用户执行了新的顶层快捷命令");
    nav_queue_clear();
    nav_settle_clear();
    nav_repeat_clear();
    g_pending_confirm_after_nav = 0;
    g_nav_active=1; g_top_focus=index;
    log_focus(tag,index);
    UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, b,2,tag);
}

/*
 * RB 组合快捷只允许在战斗顶层六命令存在时执行。
 * 一旦进入列表/Target/确认框，Input Context 会隔离这组组合，避免快捷键穿透子菜单。
 */
static int handle_top_shortcuts(void) {
    if (!Runtime_Config()->battle_shortcuts || g_context != BCTX_TOP) return 0;
    /* dev13 实机业务校正：top index 2=技能，index 3=状态。
       因此 RB+X 必须发 2，RB+下必须发 3。 */
    if (battle_chord_edge(INPUT_CONFIRM)) { request_top_shortcut(0,"快捷命令：攻击"); return 1; }
    if (battle_chord_edge(INPUT_CANCEL))  { request_top_shortcut(1,"快捷命令：道具"); return 1; }
    if (battle_chord_edge(INPUT_SPECIAL_X))  { request_top_shortcut(2,"快捷命令：技能"); return 1; }
    if (battle_chord_edge(INPUT_SPECIAL_Y)) { request_top_shortcut(4,"快捷命令：防御"); return 1; }
    if (battle_chord_edge(INPUT_NAV_UP))   { request_top_shortcut(5,"快捷命令：逃跑"); return 1; }
    if (battle_chord_edge(INPUT_NAV_DOWN)) { request_top_shortcut(3,"快捷命令：状态"); return 1; }
    return 0;
}

static void nav_queue_clear(void) {
    g_nav_q_head = g_nav_q_tail = g_nav_q_count = 0;
}

static void nav_settle_clear(void) {
    g_nav_settle_active = 0;
    g_nav_settle_button = NULL;
    g_nav_settle_start_tick = 0;
    g_nav_settle_hit_base = 0;
    g_nav_settle_timeout_logged = 0;
}

static void nav_repeat_clear(void) {
    g_repeat_up_next = 0;
    g_repeat_down_next = 0;
}

/* 把一次上/下步骤加入固定长度队列；队列满时宁可丢最新一步，也不允许无界增长拖垮老游戏。 */
static void nav_queue_push(int delta) {
    if (delta != -1 && delta != 1) return;
    if (g_nav_q_count >= NAV_QUEUE_CAP) {
        Runtime_Log("[战斗导航] 纵向输入队列已满；丢弃最新一步，避免一次按键跨过多个选项。");
        return;
    }
    g_nav_queue[g_nav_q_tail] = (signed char)delta;
    g_nav_q_tail = (g_nav_q_tail + 1) % NAV_QUEUE_CAP;
    ++g_nav_q_count;
}

static int nav_queue_pop(void) {
    int delta;
    if (g_nav_q_count <= 0) return 0;
    delta = (int)g_nav_queue[g_nav_q_head];
    g_nav_q_head = (g_nav_q_head + 1) % NAV_QUEUE_CAP;
    --g_nav_q_count;
    return delta;
}

static void nav_begin_settle(void) {
    g_nav_settle_active = 1;
    g_nav_settle_start_tick = Runtime_Tick();
    g_nav_settle_hit_base = g_forced_hit_calls;
    g_nav_settle_button = focused_button();
    g_nav_settle_timeout_logged = 0;
}

/*
 * 严格视觉握手只有同时满足“最短可见时间、足够 Hit 次数、最后 Hit 的确是当前按钮”才放行下一步。
 * 超时只记录诊断，绝不为了流畅强行跳过，这正是 dev11 修复视觉与逻辑错拍的核心。
 */
static int nav_settle_ready(void) {
    u32 age;
    if (!g_nav_settle_active) return 1;
    age = Runtime_Tick() - g_nav_settle_start_tick;
    if (age >= g_nav_settle_min_ticks &&
        (g_forced_hit_calls - g_nav_settle_hit_base) >= Runtime_Config()->nav_hit_count &&
        g_last_forced_hit_button == g_nav_settle_button) {
        g_nav_settle_active = 0;
        return 1;
    }
    if (age >= g_nav_settle_timeout_ticks && !g_nav_settle_timeout_logged) {
        Runtime_Log("[战斗导航] 原生视觉稳定等待超过诊断阈值；严格握手继续等待，不绕过视觉确认。");
        g_nav_settle_timeout_logged = 1;
    }
    return 0;
}

/*
 * 把“按住方向键”转换成离散步进：首次立即一步，等待 RepeatInitialMs，再按 RepeatIntervalMs 重复。
 * 计时统一使用 worker tick，业务层不自行读取系统时间。
 */
static int repeat_pulse(InputAction action, u32* next_tick) {
    /*
     * Repeat 只关心“通用语义动作是否按住”，不再读取 SDL 按钮位图。
     * 所以后面即使换手柄库或改默认键位，Battle 的列表 repeat 算法也不用改。
     */
    if (!battle_down(action)) { *next_tick = 0; return 0; }
    if (battle_pressed(action)) {
        *next_tick = Runtime_Tick() + g_nav_repeat_initial_ticks;
        return 1;
    }
    if (*next_tick != 0 && (i32)(Runtime_Tick() - *next_tick) >= 0) {
        *next_tick = Runtime_Tick() + g_nav_repeat_interval_ticks;
        return 1;
    }
    return 0;
}

static void queue_vertical_input_if_any(void) {
    if (g_context != BCTX_TOP && g_context != BCTX_CMD1 && g_context != BCTX_CMD2 && g_context != BCTX_CONFIRM) {
        nav_repeat_clear();
        return;
    }
    if (repeat_pulse(INPUT_NAV_UP, &g_repeat_up_next)) {
        return_visual_clear("用户开始新的纵向导航");
        activate_nav(); nav_queue_push(-1);
    }
    if (repeat_pulse(INPUT_NAV_DOWN, &g_repeat_down_next)) {
        return_visual_clear("用户开始新的纵向导航");
        activate_nav(); nav_queue_push(1);
    }
}

static void process_vertical_nav_queue(void) {
    int delta, changed = 0;
    if (g_nav_q_count <= 0) return;
    if (g_pending_page_context != 0 || UiBridge_EventPending()) return;
    if (!nav_settle_ready()) return;
    delta = nav_queue_pop();
    if (g_context == BCTX_TOP) {
        g_top_focus = (g_top_focus < 0) ? (delta > 0 ? 0 : COMMAND_COUNT - 1)
                                        : (g_top_focus + (delta > 0 ? 1 : COMMAND_COUNT - 1)) % COMMAND_COUNT;
        log_focus("战斗主菜单焦点", g_top_focus);
        changed = 1;
    } else if (g_context == BCTX_CMD1 || g_context == BCTX_CMD2) {
        changed = move_list_row(g_context, delta);
    } else if (g_context == BCTX_CONFIRM) {
        int old = g_popup_focus;
        if (delta < 0) g_popup_focus = 0;
        else g_popup_focus = 1;
        if (g_popup_focus != old) {
            log_focus("确认框焦点", g_popup_focus);
            changed = 1;
        }
    }
    if (changed == 1) nav_begin_settle();
}

static int nav_busy_for_confirm(void) {
    if (g_context == BCTX_TARGET) return 0;
    if (g_nav_q_count > 0 || g_pending_page_context != 0 || UiBridge_EventPending()) return 1;
    if (!nav_settle_ready()) return 1;
    return 0;
}

static int context_is_cancel_return(int from, int to) {
    int parent_ok = 0;
    /* RememberSelection=0 只控制“新进入”该层；必须由明确的取消动作标记，不能把普通流程误判成返回。 */
    if (g_cancel_return_from_context != from) return 0;
    if (to == BCTX_TOP && (from == BCTX_CMD0 || from == BCTX_CMD1 || from == BCTX_CMD2 || from == BCTX_CMD3)) parent_ok = 1;
    if ((to == BCTX_CMD1 || to == BCTX_CMD2) && (from == BCTX_CONFIRM || from == BCTX_TARGET)) parent_ok = 1;
    return parent_ok;
}

/*
 * 每个 worker tick 只在原版战斗层级真正变化时执行一次迁移事务。
 * 这里负责保存覆盖层父按钮、建立退场锁存、按 RememberSelection 决定新进入焦点，并清理不能跨层遗留的 pending。
 */
static void sync_context(void) {
    int now=detect_context();
    void* old_visual = NULL;
    int old_ctx, cancel_return;
    if (now==g_context) return;

    old_ctx = g_context;
    cancel_return = context_is_cancel_return(old_ctx, now);
    if (g_nav_active && old_ctx != BCTX_NONE && old_ctx != BCTX_TARGET)
        old_visual = focused_button();

    /*
     * 进入确认框或 Target 之前保存“覆盖层下面的父菜单”。
     * 这里保存的是原版真实 Button 指针，不保存屏幕坐标，因此父层回来时可以直接参与 HitTest。
     */
    if ((now == BCTX_CONFIRM || now == BCTX_TARGET) && old_ctx != BCTX_NONE && old_ctx != BCTX_CONFIRM && old_ctx != BCTX_TARGET) {
        g_overlay_parent_context = old_ctx;
        g_overlay_parent_button = old_visual;
    }

    /* 先锁存“正在消失的旧菜单”视觉，再切逻辑 Context。
       如果动作提交进入 NONE，额外锁存顶层最后命令，专门消除菜单退场前闪回第一项。 */
    if (Runtime_PtrOk(old_visual)) visual_latch_arm(old_ctx, old_visual, "旧菜单 Context 正在离开");
    if (g_nav_active && now == BCTX_NONE) visual_latch_arm_top("战斗动作提交或菜单退场");

    g_prev_context=g_context;
    g_context=now;
    log_context_transition(g_prev_context,g_context);
    /* 取消标记只消费一次；如果目标不是合法父层也丢弃，避免污染后续新回合。 */
    if (g_cancel_return_from_context == old_ctx) g_cancel_return_from_context = BCTX_NONE;
    if (g_prev_context==BCTX_TARGET && g_context!=BCTX_TARGET) target_cursor_hide_immediate();
    UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE); g_target_event_code = 0;
    g_pending_page_context=0;
    g_pending_page_old=-1;
    nav_queue_clear();
    nav_settle_clear();
    nav_repeat_clear();
    g_pending_confirm_after_nav = 0;
    if (g_context==BCTX_TOP) {
        if (!cancel_return && !Runtime_Config()->battle_remember_selection) g_top_focus=0;
        if (g_top_focus < 0 || g_top_focus >= COMMAND_COUNT) g_top_focus=0;
        if (cancel_return) Runtime_Log("[战斗导航] 取消返回：已保留先前的主菜单逻辑焦点。");
    } else if (g_context==BCTX_CMD1) {
        u8* s=battle_subui(BCTX_CMD1);
        if (!cancel_return) {
            if (Runtime_Config()->battle_remember_selection && s) g_cmd1_focus=*(i32*)(s+SUB1_CURRENT_ROW);
            else g_cmd1_focus=first_valid_row(BCTX_CMD1);
        }
        if(!row_is_valid(BCTX_CMD1,g_cmd1_focus)) g_cmd1_focus=first_valid_row(BCTX_CMD1);
        if (cancel_return) Runtime_Log("[战斗导航] 取消返回：已保留先前的 CMD1 列表行。");
    } else if (g_context==BCTX_CMD2) {
        u8* s=battle_subui(BCTX_CMD2);
        if (!cancel_return) {
            if (Runtime_Config()->battle_remember_selection && s) g_cmd2_focus=*(i32*)(s+SUB2_CURRENT_ROW);
            else g_cmd2_focus=first_valid_row(BCTX_CMD2);
        }
        if(!row_is_valid(BCTX_CMD2,g_cmd2_focus)) g_cmd2_focus=first_valid_row(BCTX_CMD2);
        if (cancel_return) Runtime_Log("[战斗导航] 取消返回：已保留先前的 CMD2 列表行。");
    } else if (g_context==BCTX_CONFIRM) {
        if (!Runtime_Config()->battle_remember_selection) g_popup_focus=0;
        else {
            u8* popup=active_confirm_popup();
            if (popup) g_popup_focus = (*(i32*)(popup+POPUP_NATIVE_SELECTION) == 0) ? 1 : 0;
            else g_popup_focus = 0;
        }
    } else if (g_context==BCTX_TARGET) {
        g_target_filter_last_mode=-999; g_target_filter_last_count=-1;
        target_sync_initial_cell();
        if (g_nav_active && Cursor_ControllerOwnsPointer()) target_cursor_sync();
    }
}


/*
 * UiBridge 每次截获一个原版 ButtonHitTest 调用后都会先询问这里。
 * 返回值本身是“这个按钮现在算不算被鼠标命中”；handled=1 表示 Battle 已经做出决定，
 * UiBridge 不得再让 SaveSlot 或原函数覆盖这个决定。
 */
u8 Battle_FilterButtonHit(void* button, int* handled) {
    void* focus;
    if (handled) *handled = 0;
    if (!button) return 0;

    /*
     * 第一优先级：取消返回父层的“首帧恢复事务”。
     * 它只过滤目标父层自己的 Button，不会影响仍在退场的旧子菜单。
     */
    if (g_return_visual.active && control_button_in_context(g_return_visual.target_context, button)) {
        if (handled) *handled = 1;
        if (button == g_return_visual.target_button) {
            g_return_visual.seen_target_hit = 1;
            ++g_forced_hit_calls;
            g_last_forced_hit_button = button;
            return 1;
        }
        return 0;
    }

    /* 第二优先级：提交动作后旧菜单退场的 dev13 锁存。 */
    if (g_visual_latch_active && control_button_in_context(g_visual_latch_context, button)) {
        if (handled) *handled = 1;
        g_visual_latch_seen = 1;
        g_visual_latch_last_seen_tick = Runtime_Tick();
        if (button == g_visual_latch_button) {
            ++g_forced_hit_calls;
            g_last_forced_hit_button = button;
            return 1;
        }
        return 0;
    }

    /* 第三优先级：当前正在由手柄导航的稳定菜单层。Target 不靠 ButtonHitTest 做选择。 */
    if (g_nav_active && g_context != BCTX_TARGET && g_context != BCTX_NONE) {
        focus = focused_button();
        if (active_control_button(button)) {
            if (handled) *handled = 1;
            if (button == focus) {
                ++g_forced_hit_calls;
                g_last_forced_hit_button = button;
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

/* 给总调度器回答“战斗或战斗结果是否占用 UI”，用于阻止标题控制器在同一帧吃到同一按键。 */
int Battle_AnyUiActive(void) {
    return battle_present() || result_active();
}

void Battle_OnPointerTakeover(CursorTakeoverEvent event) {
    int had_forced_focus;
    if (event == CURSOR_TAKEOVER_NONE) return;

    /*
     * 日志只记录“所有权真的发生了状态变化”的那一次。
     * Cursor_Update 在实体鼠标持续移动、或右摇杆连续移动时可能连续上报 takeover；如果这里每 tick 都写同一句，
     * 日志会被几十上百行重复文本淹没。先记住接管前是否还有手柄强制焦点，第一次接管清掉以后，后续重复上报就静默。
     */
    had_forced_focus = g_nav_active || g_visual_latch_active || g_return_visual.active;

    /*
     * 鼠标或右摇杆鼠标一旦接管，Battle 必须立即停止所有“强制视觉命中”。
     * 这条边界正是防止普通菜单莫名显示/锁住 Target 鼠标的关键之一。
     */
    g_nav_active = 0;
    g_target_cell = -1;
    g_pending_confirm_after_nav = 0;
    g_pending_confirm_start_tick = 0;
    confirm_watch_clear();
    nav_queue_clear();
    nav_settle_clear();
    nav_repeat_clear();
    visual_latch_clear("鼠标/右摇杆接管");
    return_visual_clear("鼠标/右摇杆接管");
    target_cursor_hide_immediate();

    /*
     * 这里刻意“不清” g_target_event_code，也不调用 UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE)。
     * 原因是这两项都代表已经排给游戏线程的一次性业务事务，而不是“当前手柄视觉焦点”。
     * dev20 的稳定实现也只撤销导航/视觉状态，不会因为鼠标所有权变化把已经按下的 A/B 事务抹掉。
     * 真正离开 Context 时，sync_context() 会在确认生命周期切换后统一清理陈旧事务。
     */

    if (had_forced_focus) {
        Runtime_Log(event == CURSOR_TAKEOVER_RIGHT_STICK ?
            "[战斗输入] 右摇杆开始控制真实鼠标，暂停战斗菜单手柄强制焦点。" :
            "[战斗输入] 实体鼠标接管，暂停战斗菜单手柄强制焦点。");
    }
}

/*
 * Battle 自己只安装 Target 网格索引专属 Hook；共用 ButtonEvent/HitTest 调用点统一由 UiBridge 安装。
 * 这样同一 CALL 不会被两个模块重复 patch。
 */

/*
 * 找出 CMD1/CMD2 列表动画这一帧应该保持高亮的“真实 Button”。
 *
 * 优先级与 refactor3 的顶层视觉修正保持同一思想，但这里处理的是“列表行 Button+0x44”：
 *   1. 旧列表正在收起：使用 visual latch 保存的最后确认行；
 *   2. 从确认框/Target 取消返回：使用 return_visual 保存的父列表行；
 *   3. 当前本来就在这个列表并由手柄导航：使用 g_cmd1_focus / g_cmd2_focus。
 *
 * 特别注意：绝不能只凭 g_cmd1_focus/g_cmd2_focus 永远写。RememberSelection=0 时，全新进入列表应该允许原版从
 * 第 1 行开始；只有上面三种“有生命周期证据”的情况才允许插件覆盖动画期间的视觉。
 */
static void* submenu_animation_visual_target(int ctx, u8* sub) {
    void* target = NULL;
    int row;
    int i;

    if (!Runtime_PtrOk(sub) || (ctx != BCTX_CMD1 && ctx != BCTX_CMD2)) return NULL;

    if (g_visual_latch_active && g_visual_latch_context == ctx && Runtime_PtrOk(g_visual_latch_button)) {
        target = g_visual_latch_button;
    } else if (g_return_visual.active && g_return_visual.target_context == ctx && Runtime_PtrOk(g_return_visual.target_button)) {
        target = g_return_visual.target_button;
    } else if (g_nav_active && g_context == ctx) {
        row = (ctx == BCTX_CMD1) ? g_cmd1_focus : g_cmd2_focus;
        target = submenu_row_button_from_sub(ctx, sub, row);
    }

    /*
     * latch/return_visual 保存的是指针。为了防止对象生命周期切换后旧指针碰巧仍可读，
     * 这里再确认它确实属于“本次动画传入的 sub”的 8 个行 Button 之一。
     */
    if (!Runtime_PtrOk(target)) return NULL;
    for (i = 0; i < SUB_ROWS_COUNT; ++i) {
        if (submenu_row_button_from_sub(ctx, sub, i) == target) return target;
    }
    return NULL;
}

/*
 * 按原版自己的行视觉协议，把 CMD1/CMD2 的 8 个 Button+0x44 同步到一个明确目标。
 * 反汇编已确认：正常 HitTest 命中某一行时，原版就是把其它行 +0x44 写 0、当前行写 1。
 * 因此这里不是发明新的绘制状态，只是在“列表动画暂时跳过 HitTest”的几帧里维持同一份原版状态。
 */
static void submenu_animation_sync_visual(int ctx, u8* sub) {
    void* target = submenu_animation_visual_target(ctx, sub);
    int i;

    if (!Runtime_PtrOk(target)) return;

    for (i = 0; i < SUB_ROWS_COUNT; ++i) {
        void* button = submenu_row_button_from_sub(ctx, sub, i);
        if (!Runtime_PtrOk(button)) continue;
        *(u8*)((u8*)button + BUTTON_HOVER_ACTIVE) = (button == target) ? 1u : 0u;
    }
}

/*
 * CMD1/CMD2 的原版列表动画 Hook。
 * 规则和顶层动画 Hook 一样：先完整执行原版，插件最后只修“手柄视觉一致性”这一件事。
 * 这样列表 X/Y 动画、帧计数、显隐与业务状态都仍由 RPG.exe 自己管理。
 */
static void FASTCALL Battle_HookCmd1PanelAnim(void* sub, void* unused_edx) {
    PFN_ThisVoid orig = (PFN_ThisVoid)FN_CMD1_PANEL_ANIM;
    (void)unused_edx;
    orig(sub);
    submenu_animation_sync_visual(BCTX_CMD1, (u8*)sub);
}

static void FASTCALL Battle_HookCmd2PanelAnim(void* sub, void* unused_edx) {
    PFN_ThisVoid orig = (PFN_ThisVoid)FN_CMD2_PANEL_ANIM;
    (void)unused_edx;
    orig(sub);
    submenu_animation_sync_visual(BCTX_CMD2, (u8*)sub);
}

/*
 * 原版 CMD1/CMD2 在“列表展开第 9 帧以后”会把真实 Windows 鼠标移动到默认列表位置。
 * 对鼠标玩家，这是原版用鼠标继续操作列表所需要的行为；对手柄玩家，这会把刚恢复的父列表重新变成
 * “鼠标默认第 1 行”，同时让系统鼠标指针露出来。
 *
 * 因此这里只在两个条件同时成立时返回 TRUE 而不真的 SetCursorPos：
 *   - Battle 目前已经有手柄导航状态（g_nav_active）；
 *   - Cursor 模块确认真实鼠标所有权仍在手柄（ControllerOwnsPointer）。
 * 任一条件不成立都立即调用原版 0x43DF30，键鼠体验不会被插件篡改。
 */
static BOOL WINAPI Battle_HookSubmenuDefaultCursorWarp(i32 x, i32 y) {
    PFN_SetCursorPos orig = (PFN_SetCursorPos)FN_MOUSE_SET_CURSOR_WRAPPER;

    if (g_nav_active && Cursor_ControllerOwnsPointer()) {
        (void)x;
        (void)y;
        return TRUE;
    }
    return orig(x, y);
}

/*
 * 战斗顶层展开/收起动画修正。
 *
 * 原版 0x41C380 在 panel+0x57C 非零（正在展开/收起）时不会执行六按钮 HitTest，
 * 而是直接调用 0x41C470。0x41C470 在 0x41C4CE~0x41C4D9 每一帧都会取第 1 个按钮，
 * 把 panel+0x588 的 ITF0026 marker 的 +0x24(Y) 强制写成第 1 项的 Y。
 *
 * 鼠标原设计下这通常不显眼；但手柄模式会保留一个稳定的逻辑焦点，所以会出现：
 * - A 选择“道具”等非第1项后，收起动画先闪回第1项，再展开子菜单；
 * - B 从子菜单返回时，展开动画先显示第1项，动画结束后 HitTest 才跳回真实焦点。
 *
 * 修法严格保持原版动画：
 * 1. 先完整调用原版 0x41C470，让 X、计时器、显隐状态等全部照常更新；
 * 2. 只有当手柄导航仍拥有主菜单逻辑焦点时，才把 marker Y 改回 g_top_focus 对应真实 Button 的 Y；
 * 3. 不修改 g_top_focus，不修改动画状态，不硬编码任何屏幕坐标；
 * 4. 若鼠标已经接管（g_nav_active=0），完全不干预，保留原版鼠标视觉。
 *
 * 这个 Hook 位于 0x41C3B2 -> 0x41C470 的唯一 CALL 点，因此只影响战斗顶层 panel 的动画分支，
 * 不会污染其它使用相似 SF2/marker 结构的菜单。
 */
static void FASTCALL Battle_HookTopPanelAnim(void* panel, void* unused_edx) {
    PFN_ThisVoid orig = (PFN_ThisVoid)FN_TOP_PANEL_ANIM;
    void* button;
    void* marker;
    (void)unused_edx;

    /* 原版必须先执行；我们只修它最后那一笔“marker Y = 第1项 Y”。 */
    orig(panel);

    /*
     * “动画期应该显示哪一项”必须来自当前真实生命周期，而不能无条件使用旧 g_top_focus：
     *
     * 1. 旧顶层正在收起：优先使用 dev13 的 visual latch，表示“刚刚确认的那一项”；
     * 2. B 正在返回顶层：优先使用 return_visual，表示“应该恢复的父层那一项”；
     * 3. 顶层当前就是活动 Context：使用正常 g_top_focus；
     * 4. 其它情况（例如新角色/新回合刚开始展开，worker 还没完成 Context 同步）不强行覆盖原版。
     *
     * 这一层级非常重要：否则 RememberSelection=0 时，上一回合的旧 focus 可能被错误带进下一次全新展开。
     */
    button = NULL;
    if (g_visual_latch_active && g_visual_latch_context == BCTX_TOP && Runtime_PtrOk(g_visual_latch_button)) {
        button = g_visual_latch_button;
    } else if (g_return_visual.active && g_return_visual.target_context == BCTX_TOP && Runtime_PtrOk(g_return_visual.target_button)) {
        button = g_return_visual.target_button;
    } else if (g_nav_active && g_context == BCTX_TOP && g_top_focus >= 0 && g_top_focus < COMMAND_COUNT && Runtime_PtrOk(panel)) {
        button = *(void**)((u8*)panel + COMMAND_BUTTONS + (u32)g_top_focus * 4u);
    }

    if (!Runtime_PtrOk(button) || !Runtime_PtrOk(panel)) return;
    marker = *(void**)((u8*)panel + COMMAND_MARKER_OBJECT);
    if (!Runtime_PtrOk(marker)) return;

    *(i32*)((u8*)marker + MARKER_LOCAL_Y) = *(i32*)((u8*)button + BUTTON_LOCAL_Y);
}

int Battle_InstallHooks(void) {
    g_nav_settle_min_ticks = Runtime_MsToTicks(Runtime_Config()->nav_visual_min_ms);
    g_nav_settle_timeout_ticks = Runtime_MsToTicks(Runtime_Config()->nav_settle_timeout_ms);
    g_nav_repeat_initial_ticks = Runtime_MsToTicks(Runtime_Config()->nav_repeat_initial_ms);
    g_nav_repeat_interval_ticks = Runtime_MsToTicks(Runtime_Config()->nav_repeat_interval_ms);

    /*
     * refactor4 的 CMD1/CMD2 两组 Hook 必须成对安装：
     * - 动画 Hook 负责动画期正确行视觉；
     * - cursor-warp Hook 负责阻止手柄模式下原版把真实鼠标拉回默认行。
     * 任一条失败都 fail-closed，避免只修一半造成更难诊断的混合状态。
     */
    if (!Runtime_PatchCall(CALL_CMD1_PANEL_ANIM, (void*)Battle_HookCmd1PanelAnim, FN_CMD1_PANEL_ANIM)) {
        Runtime_Log("[致命] 战斗 CMD1 列表动画视觉 Hook 安装失败。");
        return 0;
    }
    if (!Runtime_PatchCall(CALL_CMD2_PANEL_ANIM, (void*)Battle_HookCmd2PanelAnim, FN_CMD2_PANEL_ANIM)) {
        Runtime_Log("[致命] 战斗 CMD2 列表动画视觉 Hook 安装失败。");
        return 0;
    }
    if (!Runtime_PatchCall(CALL_CMD1_PANEL_CURSOR_WARP, (void*)Battle_HookSubmenuDefaultCursorWarp, FN_MOUSE_SET_CURSOR_WRAPPER)) {
        Runtime_Log("[致命] 战斗 CMD1 列表默认鼠标定位 Hook 安装失败。");
        return 0;
    }
    if (!Runtime_PatchCall(CALL_CMD2_PANEL_CURSOR_WARP, (void*)Battle_HookSubmenuDefaultCursorWarp, FN_MOUSE_SET_CURSOR_WRAPPER)) {
        Runtime_Log("[致命] 战斗 CMD2 列表默认鼠标定位 Hook 安装失败。");
        return 0;
    }

    if (!Runtime_PatchCall(CALL_TOP_PANEL_ANIM, (void*)Battle_HookTopPanelAnim, FN_TOP_PANEL_ANIM)) {
        Runtime_Log("[致命] 战斗顶层展开/收起动画视觉修正 Hook 安装失败。");
        return 0;
    }

    if (!Runtime_PatchCall(CALL_TARGET_GRID_INDEX, (void*)Battle_HookTargetGridIndex, FN_TARGET_GRID_INDEX)) {
        Runtime_Log("[致命] 战斗 Target 原版网格索引 Hook 安装失败。");
        return 0;
    }

    /* 其余 Battle ButtonEvent/HitTest CALL 统一由 UiBridge 安装，避免同一调用点被多个模块重复改写。 */
    Runtime_Log("[战斗] 战斗专属 Hook、顶层/CMD1/CMD2 动画视觉一致性与手柄鼠标所有权修正初始化完成。");
    return 1;
}

/* Result 生命周期与 battle_ptr 分离，因此单独轮询并只处理 A 继续。 */
static void battle_update_result(void) {
    int active = result_active();
    void* b;
    if (!active) {
        if (g_result_was_present) Runtime_Log("[战斗结果] 结果界面已离开。");
        g_result_was_present = 0;
        return;
    }
    if (!g_result_was_present) {
        /* 战斗主 UI 与结果 UI 生命周期分离。进入 Result 的第一帧先清理理论上不应残留的旧菜单事件，
           避免一个未消费的旧 button request 阻塞结果界面的 A。 */
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE);
        Runtime_Log("[战斗结果] 检测到结果界面。");
        g_result_was_present = 1;
    }
    if (InputRouter_PressedOn(INPUT_CTX_BATTLE_RESULT, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        b = result_active_button();
        if (Runtime_PtrOk(b)) UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, b,2,"战斗结果：A 确认当前原版继续按钮");
    }
}

/*
 * Battle 的 worker 主循环：先维护 Result/视觉事务，再同步 Context，最后按优先级处理快捷、B、方向队列、分类、Target 和 A。
 * 所有动作最终仍落到原版 Event/Selector/关闭协议，而不是模拟一套平行菜单。
 */
void Battle_Update(void) {
    int present=battle_present();

    /* Result 不属于 battle_ptr 生命周期，因此无论主战斗 UI 是否还在，都先单独更新。 */
    battle_update_result();
    visual_latch_maintenance();
    return_visual_maintenance();

    if(!present){
        if(g_battle_was_present) Runtime_Log("[战斗] 战斗主 UI 已离开，清理导航状态。");
        visual_latch_clear("战斗主 UI 已离开");
        return_visual_clear("战斗主 UI 已离开");
        target_cursor_hide_immediate();
        /* 战斗已经结束：把“本场战斗才有意义”的状态全部清回初始值。
           这里特意一项一项写开，避免以后新增状态时误以为这些变量彼此等价。 */
        g_battle_was_present = 0;
        g_nav_active = 0;
        g_context = BCTX_NONE;
        g_prev_context = BCTX_NONE;
        g_top_focus = -1;
        g_target_cell = -1;
        g_target_filter_last_mode = -999;
        g_target_filter_last_count = -1;
        g_overlay_parent_context=BCTX_NONE; g_overlay_parent_button=NULL;
        /* Battle Result UI 与战斗主 UI 生命周期分离；结果按钮事件不能在这里被清掉。 */
        if (!result_active()) UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE);
        g_target_event_code = 0;
        g_cancel_return_from_context = BCTX_NONE;
        nav_queue_clear();
    nav_settle_clear();
    nav_repeat_clear();
    g_pending_confirm_after_nav = 0;
    g_pending_confirm_start_tick = 0;
    confirm_watch_clear();
    return;
    }
    if (!g_battle_was_present) {
        /* 只在“本场战斗第一次看到主 UI”时记一次日志。
           如果每帧都写日志，不但文件会暴涨，还可能反过来影响老游戏的时序。 */
        char line[128];
        SIZE_T p = 0;
        append_text(line, sizeof(line), &p, "[战斗] 检测到战斗主 UI，指针=");
        append_hex32(line, sizeof(line), &p, (u32)*(void**)GLOBAL_BATTLE_UI);
        Runtime_Log(line);

        g_battle_was_present = 1;

        /* dev12 之后允许用户决定是否记住上一次主命令。
           关闭记忆时，新战斗从第 1 项开始；取消返回仍由单独的返回事务恢复原项。 */
        if (!Runtime_Config()->battle_remember_selection) {
            g_top_focus = 0;
        }
    }
    /* dev11：不再用 GetCursorPos 位移自动释放手柄焦点。老游戏/DirectDraw/菜单会主动 warp 光标，
       dev10 日志已证明这会产生大量“physical mouse moved”假接管并破坏视觉握手。右摇杆仍可自由移动真实鼠标。 */
    sync_context();
    apply_pending_page_focus();
    /*
     * 在读取本帧新按键之前先处理上一轮 A 的结局。
     * 这样原版若把“不可用项 A”正常判成无动作，本帧方向键立刻就能继续工作，不必等 B 退出重进。
     */
    confirm_watch_maintenance();

    if(g_context==BCTX_NONE) return;

    /* RB 组合快捷仅在顶层 Context 生效，并优先于普通 A/B/D-Pad。 */
    if (handle_top_shortcuts()) { log_native_hit_activity(); return; }

    /* B 始终优先且不等待视觉队列；确认框 B 映射到其第二个真实按钮的 code=2。 */
    if(battle_pressed(INPUT_CANCEL)) {
        nav_queue_clear();
        nav_settle_clear();
        g_pending_confirm_after_nav = 0;
        g_pending_confirm_start_tick = 0;
        confirm_watch_clear();

        /*
         * B 是战斗里的最高优先级“逃生键”。
         * 即使未来又有某条异常 A 路径留下了 Battle 自己的未消费 pending，
         * 玩家按 B 时也必须先把这条旧确认事务丢掉，再排当前层真正的取消事件。
         *
         * 这里只清 UI_EVENT_OWNER_BATTLE，绝不会误删 Interface、SaveSlot 等其它模块的事件。
         * 这和 refactor18 引入的 owner 隔离原则完全一致。
         */
        UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE);
        request_cancel();
    }

    /* 先收集纵向输入，再按“native hit + 最小可见时间”逐步执行。 */
    queue_vertical_input_if_any();

    if(g_context==BCTX_CMD1 || g_context==BCTX_CMD2){
        /*
         * 技能/道具列表的四组横向输入现在分工明确：
         * - LB/RB 仍然切原版最右侧的大类/子类；
         * - D-Pad ←/→ 原本没有业务，现在直接点击原版上一页/下一页 Button。
         *
         * 左右翻页只存在于 CMD1/CMD2，绝不会抢走 Target、顶层命令或其它战斗 Context 的方向键。
         */
        if(battle_pressed(INPUT_CATEGORY_PREV)){
            return_visual_clear("用户切换战斗分类");
            nav_queue_clear();
            nav_settle_clear();
            g_pending_confirm_after_nav = 0;
            activate_nav();
            request_category_delta(g_context,-1);
        }
        if(battle_pressed(INPUT_CATEGORY_NEXT)){
            return_visual_clear("用户切换战斗分类");
            nav_queue_clear();
            nav_settle_clear();
            g_pending_confirm_after_nav = 0;
            activate_nav();
            request_category_delta(g_context,1);
        }
        if(battle_pressed(INPUT_NAV_LEFT)){
            return_visual_clear("用户直接翻战斗列表页");
            nav_queue_clear();
            nav_settle_clear();
            g_pending_confirm_after_nav = 0;
            activate_nav();
            request_direct_page_delta(g_context,-1);
        }
        if(battle_pressed(INPUT_NAV_RIGHT)){
            return_visual_clear("用户直接翻战斗列表页");
            nav_queue_clear();
            nav_settle_clear();
            g_pending_confirm_after_nav = 0;
            activate_nav();
            request_direct_page_delta(g_context,1);
        }
    } else if(g_context==BCTX_CMD3){
        if(battle_pressed(INPUT_NAV_DOWN)){
            return_visual_clear("用户在 CMD3 执行新的导航/确认");
            activate_nav(); request_confirm();
        }
    } else if(g_context==BCTX_TARGET){
        /*
         * Target 是二维空间导航，不是普通的“上一项/下一项”列表。
         * 每次方向输入先调用 activate_nav()：它已经统一完成“手柄取得光标所有权 + 导航状态激活”，
         * 所以这里不再重复调用 Cursor_ClaimForControllerNavigation()。随后只负责寻找空间上最合适的合法目标。
         */
        if (battle_pressed(INPUT_NAV_LEFT)) {
            return_visual_clear("用户继续 Target 导航");
            activate_nav();
            move_target_direction(-1, 0);
        }
        if (battle_pressed(INPUT_NAV_RIGHT)) {
            return_visual_clear("用户继续 Target 导航");
            activate_nav();
            move_target_direction(1, 0);
        }
        if (battle_pressed(INPUT_NAV_UP)) {
            return_visual_clear("用户继续 Target 导航");
            activate_nav();
            move_target_direction(0, -1);
        }
        if (battle_pressed(INPUT_NAV_DOWN)) {
            return_visual_clear("用户继续 Target 导航");
            activate_nav();
            move_target_direction(0, 1);
        }
    } else if(g_context==BCTX_CMD0){
        if(battle_pressed(INPUT_NAV_UP) || battle_pressed(INPUT_NAV_DOWN)) {
            return_visual_clear("用户在 CMD0 开始新的导航");
            activate_nav();
        }
    }

    process_vertical_nav_queue();

    if(battle_pressed(INPUT_CONFIRM)) {
        if (g_context == BCTX_CONFIRM || g_context == BCTX_TARGET) {
            request_confirm();
        } else {
            void* current_button = focused_button();

            /*
             * 先做基础可用性门，再决定是否进入“视觉握手后确认”。
             * r21 的顺序相反：A 可能先被记成 deferred，之后才发现按钮不可用，于是留下等待状态。
             */
            if (!button_usable(current_button)) {
                g_pending_confirm_after_nav = 0;
                g_pending_confirm_start_tick = 0;
                Runtime_Log("[战斗确认] 当前焦点按钮不可用；A 已在建立任何等待事务之前被吞掉。");
            } else if (nav_busy_for_confirm()) {
                g_pending_confirm_after_nav = 1;
                g_pending_confirm_start_tick = Runtime_Tick();
                Runtime_Log("[战斗确认] A 已短暂缓冲，等待原版视觉焦点握手；超时会自动丢弃而不会锁住菜单。");
            } else {
                request_confirm();
            }
        }
    }

    if (g_pending_confirm_after_nav) {
        if (!nav_busy_for_confirm()) {
            g_pending_confirm_after_nav = 0;
            g_pending_confirm_start_tick = 0;
            request_confirm();
        } else if ((Runtime_Tick() - g_pending_confirm_start_tick) >= g_nav_settle_timeout_ticks) {
            /*
             * 严格视觉握手本身仍不降级；这里只放弃这一次过早按下的 A。
             * 同时清掉 settle，防止一个永远不会产生 Hit 的不可交互项目把后续方向键一起卡住。
             */
            g_pending_confirm_after_nav = 0;
            g_pending_confirm_start_tick = 0;
            nav_queue_clear();
            nav_settle_clear();
            Runtime_Log("[战斗确认] 缓冲 A 等待视觉握手超时；本次 A 已忽略并解锁导航，不向原版伪造确认。");
        }
    }
    log_native_hit_activity();
}
