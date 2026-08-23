#include "input_router.h"

/*
 * 物理键映射只在这里集中出现一次。
 * 如果以后增加用户改键，优先改这一层，而不是在 Battle/Save 里到处找 PAD_SOUTH。
 */
/*
 * consumed mask 是“同一 worker tick 内的输入所有权”。
 * 位为 1 表示这个语义动作已经被更高优先级 Context 使用，后面的模块本 tick 不能重复吃它。
 * refactor7 的已有模块不主动设置它，所以新功能没有出现时，旧行为与封版基线完全相同。
 */
static u32 g_consumed_actions;

/*
 * 左摇杆水平轴的“越过 50%”边沿状态。
 * g_left_stick_horizontal_latched=1 表示这一次推杆动作已经触发过，必须回中后才重新武装。
 * g_left_stick_horizontal_step 只活一个 worker tick，业务层读到后不会持续重复。
 */
static int g_left_stick_horizontal_latched;
static int g_left_stick_horizontal_step;

#define INPUT_LEFT_STICK_50_THRESHOLD 16384

/*
 * 在每个 worker tick 的物理采样完成后更新一次模拟轴边沿。
 * 触发门严格是 50%：绝对值未达到 16384 时不会产生角色切换意图。
 * 重新武装使用现有左摇杆死区 7000，而不是 50%，目的是给阈值附近留出迟滞，避免手抖造成左右反复触发。
 */
static void input_update_left_stick_horizontal_step(void) {
    i16 axis = PadInput_Axis(PAD_AXIS_LEFT_X);

    g_left_stick_horizontal_step = 0;

    if (g_left_stick_horizontal_latched) {
        if (axis > -PAD_STICK_DEADZONE && axis < PAD_STICK_DEADZONE) {
            g_left_stick_horizontal_latched = 0;
        }
        return;
    }

    if ((i32)axis <= -INPUT_LEFT_STICK_50_THRESHOLD) {
        g_left_stick_horizontal_step = -1;
        g_left_stick_horizontal_latched = 1;
    } else if ((i32)axis >= INPUT_LEFT_STICK_50_THRESHOLD) {
        g_left_stick_horizontal_step = 1;
        g_left_stick_horizontal_latched = 1;
    }
}

void InputRouter_BeginFrame(void) {
    g_consumed_actions = 0u;
    input_update_left_stick_horizontal_step();
}

void InputRouter_Consume(InputAction action) {
    if ((int)action < 0 || action >= INPUT_ACTION_COUNT) return;
    g_consumed_actions |= (1u << (u32)action);
}

/* r37 指针模态的全语义捕获；下一 tick 由 BeginFrame 自动解除。 */
void InputRouter_CaptureAll(void) {
    g_consumed_actions = (1u << (u32)INPUT_ACTION_COUNT) - 1u;
    g_left_stick_horizontal_step = 0;
}

static int input_action_consumed(InputAction action) {
    if ((int)action < 0 || action >= INPUT_ACTION_COUNT) return 1;
    return (g_consumed_actions & (1u << (u32)action)) != 0u;
}

/* 所有“语义动作 → 默认物理键”只在这一张表出现；业务模块不再散落 SDL 按钮编号。 */
static PadButton input_action_button(InputAction action) {
    switch (action) {
    case INPUT_CONFIRM:       return PAD_SOUTH;
    case INPUT_CANCEL:        return PAD_EAST;
    case INPUT_SPECIAL_X:     return PAD_WEST;
    case INPUT_SPECIAL_Y:     return PAD_NORTH;
    case INPUT_NAV_UP:        return PAD_DPAD_UP;
    case INPUT_NAV_DOWN:      return PAD_DPAD_DOWN;
    case INPUT_NAV_LEFT:      return PAD_DPAD_LEFT;
    case INPUT_NAV_RIGHT:     return PAD_DPAD_RIGHT;
    case INPUT_CATEGORY_PREV: return PAD_LB;
    case INPUT_CATEGORY_NEXT: return PAD_RB;
    case INPUT_SUBTYPE_PREV:  return PAD_LT;
    case INPUT_SUBTYPE_NEXT:  return PAD_RT;
    case INPUT_SYSTEM_START:  return PAD_START;
    case INPUT_MOUSE_R3:      return PAD_R3;
    case INPUT_MODIFIER_SHIFT:return PAD_BACK;
    default:                  return PAD_SOUTH;
    }
}

/* 把语义动作翻译成物理键后读取按下沿；这仍是不带 Context 的基础通道。 */
int InputRouter_Pressed(InputAction action) {
    if ((int)action < 0 || action >= INPUT_ACTION_COUNT || input_action_consumed(action)) return 0;
    return PadInput_Pressed(input_action_button(action));
}

/* 语义版持续按住查询，用于 repeat 和组合键。 */
int InputRouter_Down(InputAction action) {
    if ((int)action < 0 || action >= INPUT_ACTION_COUNT || input_action_consumed(action)) return 0;
    return PadInput_Down(input_action_button(action));
}

/* 语义版松开沿查询。 */
int InputRouter_Released(InputAction action) {
    if ((int)action < 0 || action >= INPUT_ACTION_COUNT || input_action_consumed(action)) return 0;
    return PadInput_Released(input_action_button(action));
}

int InputRouter_ChordPressed(InputAction modifier, InputAction action) {
    PadButton modifier_button;
    PadButton action_button;

    if ((int)modifier < 0 || modifier >= INPUT_ACTION_COUNT) return 0;
    if ((int)action < 0 || action >= INPUT_ACTION_COUNT) return 0;
    if (input_action_consumed(modifier) || input_action_consumed(action)) return 0;

    modifier_button = input_action_button(modifier);
    action_button = input_action_button(action);

    /*
     * 情况一：修饰键已经按住，这一帧动作键刚按下。
     * 情况二：动作键已经按住，这一帧修饰键刚按下。
     * 两种顺序都只在“新产生组合”的那个边沿返回 1，不会按住后每帧重复触发。
     */
    return PadInput_Down(modifier_button) &&
           (PadInput_Pressed(action_button) ||
            (PadInput_Pressed(modifier_button) && PadInput_Down(action_button)));
}


/*
 * 返回本 tick 已经由 InputRouter_BeginFrame 计算好的左摇杆水平 50% 单次方向沿。
 * 这里不再读取 SDL/轴，确保同一帧所有业务模块看到的是同一份稳定快照。
 */
int InputRouter_LeftStickHorizontalStep50(void) {
    return g_left_stick_horizontal_step;
}

/*
 * 策略表不是“功能代码”，而是对当前 Context 的契约说明。
 * 以后某个页面出现冲突，可以先看这里确认这个键究竟应继承、合并还是覆盖。
 */
static const InputPolicy g_policy_none = {{
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_exploration = {{
    INPUT_PASS, INPUT_PASS, INPUT_PASS, INPUT_PASS,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_battle_top = {{
    /* A=确认；B/单独X/Y/左右/LB/RB没有 dev20 单键动作。RB 组合键由 Battle 专属 chord 处理。 */
    INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_battle_list = {{
    /* refactor14 起技能/道具列表的左右改为直接上一页/下一页；A/B、上下、LB/RB 仍保留各自专属行为。 */
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_battle_confirm = {{
    /* 双按钮确认框：上下选项，A 当前项，B 强制取消。 */
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_battle_target = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_battle_result = {{
    /* Result/Reward 只需要 A 逐层继续；其它数字键不额外接管。 */
    INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_title = {{
    INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_save_slot = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

static const InputPolicy g_policy_save_popup = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

/*
 * 天书选槽以后出现的“存档 / 读档 / 取消”三项动作窗口。
 * A/B/上下属于窗口本身；左右、肩键在这个模态窗口里没有业务，避免穿透到底层 Interface。
 * Start/R3/Back 仍保留系统层能力，其中 R3 可以让用户主动切回鼠标。
 */
static const InputPolicy g_policy_save_action = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 普通对话框只新增 A=推进。其它方向和 B 不在这一阶段凭空发明业务意义。
 * A 使用 OVERRIDE，是因为它最终被映射到原版消息引擎已经支持的“左键推进”协议。
 */
static const InputPolicy g_policy_dialogue = {{
    INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

/*
 * 通用 Yes/No 询问框：A 当前项、B 直接取消；四个方向都可以明确选择 Yes/No。
 * X/Y、肩键和 Start 不属于询问框业务，因此不让它们意外穿透到被遮住的菜单。
 */
static const InputPolicy g_policy_confirm_dialog = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_PASS, INPUT_PASS
}};

/*
 * Movie Context 只覆盖 Start。
 * 其它按钮在这个 Context 里没有新增业务；R3 仍保留基础鼠标能力，避免动画模块顺手改变旧的鼠标设计。
 * 这里没有“立即/长按/资源分类”三套策略：电影 active 时 Start 就只有一个含义——原版 ESC。
 */
static const InputPolicy g_policy_movie = {{
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_PASS, INPUT_PASS
}};

/*
 * 主 Interface 根外壳。
 * refactor26 起“切角色”已经迁到左摇杆水平 50% 单次方向沿，不再占用 D-Pad ←/→。
 * 因此 Shell 的数字按键只拥有 B 与 LB/RB；D-Pad 在没有页面专属业务时明确吞掉，防止旧的换人语义回流。
 * Start/R3/Back 继续 PASS，保持已验收的系统与鼠标底层能力。
 */
static const InputPolicy g_policy_interface_shell = {{
    INPUT_CONSUME, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 主 Interface state2“法宝/道具”页。
 * - A：当前物品的原版行事件；
 * - X：暂置时分发、其它原版允许的类型时弃置；
 * - ↑/↓：8 行物品列表，边界走原版翻页；
 * - LT/RT：6 个子类型；
 * - ←/→：原版上一页/下一页；B、LB/RB 仍交给 InterfaceShell。
 * - 页面自己的弹窗出现后，Adapter 会显式消费 B/方向，阻止 Shell 从下层穿透。
 */
static const InputPolicy g_policy_interface_items = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 主 Interface state3“绝学/法术”页。
 * A、↑/↓、LT/RT 与 ←/→翻页都是页面专属；B、LB/RB 在没有使用确认框时继续交给 InterfaceShell。
 * 这样“页面内部业务”与“主 Interface 外壳业务”仍然保持清晰分层。
 */
static const InputPolicy g_policy_interface_skills = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 主 Interface state4“及身/装备”页。
 *
 * refactor26 用户侧键位：
 * - X：兵刃/护甲/饰物三个栏位循环；
 * - ↑/↓：右侧候选装备列表，越过当前页边界时仍可连续翻页；
 * - ←/→：直接上一页/下一页；
 * - LT/RT：保留 refactor25 已有的上一页/下一页快捷；
 * - A：装备当前候选；
 * - B、LB/RB：继续由 InterfaceShell 返回/切主大类。
 *
 * D-Pad 与 LT/RT 两组翻页入口都只能调用同一个 equipment_request_page()，
 * 页码仍由 RPG.exe 的真实 Prev/Next ButtonEvent 修改，插件绝不直接写 page 字段。
 */
static const InputPolicy g_policy_interface_equipment = {{
    INPUT_OVERRIDE, INPUT_PASS, INPUT_OVERRIDE, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 主 Interface state5“五内”页。
 *
 * refactor26 保持五内页面自己的核心键位：
 * - X：在“烈/神/魔/魂/迅”五个节点之间循环焦点；
 * - LT：当前节点减少；RT：当前节点增加；
 * - Y：点击原版“蕴魂”，提交本轮加点；
 * - D-Pad：本页没有额外业务，明确吞掉；角色切换已经统一迁到左摇杆水平 50% 单次方向沿；
 * - B、LB/RB：继续交给 InterfaceShell 做返回和大类切换。
 */
static const InputPolicy g_policy_interface_inner_stats = {{
    INPUT_CONSUME, INPUT_PASS, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 主 Interface state6“阵形”页。
 *
 * 这里最重要的是 B 使用 MERGE，而不是简单 OVERRIDE：
 * - 页面处于“已经选了来源阵位，等待目标阵位”时，Formation Adapter 在 OVERLAY 通道执行原版 code=1，
 *   随后 Consume(INPUT_CANCEL)，所以 Shell 看不到同一个 B；
 * - 页面没有进行中的交换事务时，Adapter 不消费 B，COMMON/后续 Shell 仍能把 B 当普通返回。
 *
 * 四方向与 A 完全属于阵形；X/Y/LT/RT 当前无页面业务，明确吞掉；LB/RB 继续 PASS 给 InterfaceShell 切大类。
 */
static const InputPolicy g_policy_interface_formation = {{
    INPUT_OVERRIDE, INPUT_MERGE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 主 Interface state8“机能”页。
 *
 * 用户最终规格：
 * - ↑/↓：音乐 -> 音效 -> 空明流转三行焦点；
 * - 音乐/音效：LT 减、RT 加；
 * - 空明流转：A 点击原版按钮，后续 Yes/No 交给 ConfirmDialog；
 * - 本页 D-Pad 左右没有业务，明确吞掉；
 * - B、LB/RB 仍由 InterfaceShell 负责返回/切大类；
 * - 左摇杆不在 state8 切角色，因为 Shell 只允许 state1~5。
 */
static const InputPolicy g_policy_interface_options = {{
    INPUT_OVERRIDE, INPUT_PASS, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 公共剧情消息 mode=3 的两项选择。
 * 这不是 0x4272C0/0x4276F0 菜单 ConfirmDialog，而是 0x404600 的原版鼠标选择器。
 * A/B/四方向全部由 SceneChoice Adapter 接管；其它菜单键不能穿透到客栈、地图或主 Interface。
 * Start/R3/Back 继续 PASS：Start 保留系统语义，R3/Back 仍可主动进入鼠标模式。
 */
static const InputPolicy g_policy_scene_choice = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 客栈根层只有三只竖排按钮。A/↑/↓属于客栈自己；B没有原版业务，所以 CONSUME。
 * 其它菜单键也不应穿透到底下地图；Start/R3/Back 继续保留系统层能力。
 */
static const InputPolicy g_policy_inn_root = {{
    INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 炼化两层的按键契约相同，区别只在 B 的原版目标 Button：
 * 第一层是顶部退出图标，第二层是右列表自己的取消 Button。
 * LT/RT、X/Y 当前没有炼化业务，明确吞掉，避免未来其它 Context 的含义穿透。
 */
static const InputPolicy g_policy_synthesis = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 商店根层是左右双列的一套统一菜单：X 换列，↑↓选物，←→翻页，LB/RB 换类别，A 进入数量窗，B 点真实退出图标。
 * Y 独占原版右键的道具信息开/关语义；LT/RT 在根层没有业务；Start/R3/Back 继续保留系统/鼠标能力。
 */
static const InputPolicy g_policy_shop_root = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/*
 * 数量窗完全模态：LT 减、RT 加，←/→选择确认或取消，A 点击当前项，B 直接取消。
 * 其它菜单动作明确吞掉，不能穿透到底下两列。
 */
static const InputPolicy g_policy_shop_quantity = {{
    INPUT_OVERRIDE, INPUT_OVERRIDE, INPUT_CONSUME, INPUT_CONSUME,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_CONSUME, INPUT_CONSUME, INPUT_OVERRIDE, INPUT_OVERRIDE,
    INPUT_PASS, INPUT_PASS, INPUT_PASS
}};

/* 每个 Context 只返回一张只读策略表；新增界面应新增表，而不是往现有业务 if 中塞例外。 */
const InputPolicy* InputRouter_Policy(InputContext context) {
    switch (context) {
    case INPUT_CTX_EXPLORATION:     return &g_policy_exploration;
    case INPUT_CTX_BATTLE_TOP:      return &g_policy_battle_top;
    case INPUT_CTX_BATTLE_LIST:     return &g_policy_battle_list;
    case INPUT_CTX_BATTLE_CONFIRM:  return &g_policy_battle_confirm;
    case INPUT_CTX_BATTLE_TARGET:   return &g_policy_battle_target;
    case INPUT_CTX_BATTLE_RESULT:   return &g_policy_battle_result;
    case INPUT_CTX_TITLE:           return &g_policy_title;
    case INPUT_CTX_SAVE_SLOT:       return &g_policy_save_slot;
    case INPUT_CTX_SAVE_POPUP:      return &g_policy_save_popup;
    case INPUT_CTX_SAVE_ACTION:     return &g_policy_save_action;
    case INPUT_CTX_DIALOGUE:        return &g_policy_dialogue;
    case INPUT_CTX_SCENE_CHOICE:    return &g_policy_scene_choice;
    case INPUT_CTX_CONFIRM_DIALOG:  return &g_policy_confirm_dialog;
    case INPUT_CTX_MOVIE:           return &g_policy_movie;
    case INPUT_CTX_INTERFACE_SHELL: return &g_policy_interface_shell;
    case INPUT_CTX_INTERFACE_ITEMS:    return &g_policy_interface_items;
    case INPUT_CTX_INTERFACE_SKILLS:   return &g_policy_interface_skills;
    case INPUT_CTX_INTERFACE_EQUIPMENT:return &g_policy_interface_equipment;
    case INPUT_CTX_INTERFACE_INNER_STATS:return &g_policy_interface_inner_stats;
    case INPUT_CTX_INTERFACE_FORMATION: return &g_policy_interface_formation;
    case INPUT_CTX_INTERFACE_OPTIONS:   return &g_policy_interface_options;
    case INPUT_CTX_INN_ROOT:            return &g_policy_inn_root;
    case INPUT_CTX_SYNTHESIS_PRIMARY:   return &g_policy_synthesis;
    case INPUT_CTX_SYNTHESIS_SECONDARY: return &g_policy_synthesis;
    case INPUT_CTX_SHOP_ROOT:            return &g_policy_shop_root;
    case INPUT_CTX_SHOP_QUANTITY:        return &g_policy_shop_quantity;
    default:                            return &g_policy_none;
    }
}

/* 安全读取某 Context 某动作的 PASS/MERGE/OVERRIDE/CONSUME 模式；非法动作默认吞掉。 */
InputBindMode InputRouter_Mode(InputContext context, InputAction action) {
    const InputPolicy* policy = InputRouter_Policy(context);
    if ((int)action < 0 || action >= INPUT_ACTION_COUNT) return INPUT_CONSUME;
    return policy->mode[(int)action];
}

/* 根据 PASS/MERGE/OVERRIDE/CONSUME 判断某条执行通道是否应该收到动作。 */
static int input_route_layer_enabled(InputBindMode mode, InputRouteLayer layer) {
    if (mode == INPUT_CONSUME) return 0;
    if (mode == INPUT_PASS) return layer == INPUT_LAYER_COMMON;
    if (mode == INPUT_OVERRIDE) return layer == INPUT_LAYER_OVERLAY;
    /* MERGE：通用和区域专属两边都执行。 */
    return layer == INPUT_LAYER_COMMON || layer == INPUT_LAYER_OVERLAY;
}

/* 先判断该通道是否被策略允许，再读取同一物理按下沿。 */
int InputRouter_PressedOn(InputContext context, InputAction action, InputRouteLayer layer) {
    if (!input_route_layer_enabled(InputRouter_Mode(context, action), layer)) return 0;
    return InputRouter_Pressed(action);
}

/* 与 PressedOn 对称，但服务持续按住。 */
int InputRouter_DownOn(InputContext context, InputAction action, InputRouteLayer layer) {
    if (!input_route_layer_enabled(InputRouter_Mode(context, action), layer)) return 0;
    return InputRouter_Down(action);
}

/* 与 PressedOn 对称，但服务松开沿。 */
int InputRouter_ReleasedOn(InputContext context, InputAction action, InputRouteLayer layer) {
    if (!input_route_layer_enabled(InputRouter_Mode(context, action), layer)) return 0;
    return InputRouter_Released(action);
}
