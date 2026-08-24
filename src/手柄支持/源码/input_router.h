#ifndef CASTLE_PAD_INPUT_ROUTER_H
#define CASTLE_PAD_INPUT_ROUTER_H

#include "pad_input.h"

/*
 * input_router.h
 *
 * 这是本次重构新增的“语义层”。物理键只在 pad_input.c 出现；业务模块看到的是动作语义。
 * 例如默认布局的 PAD_SOUTH 会先变成 INPUT_CONFIRM，之后 Battle/Save/Title 再决定
 * 如何实现“确认”；交换布局后 INPUT_CONFIRM 改由 PAD_EAST 产生，业务层不用逐页修改。
 *
 * 每个 Context 对动作有四种组合方式：
 * PASS     - 使用通用语义，不增加专属行为；
 * MERGE    - 通用语义执行后，还要叠加当前区域的专属行为；
 * OVERRIDE - 当前区域完全取代通用行为；
 * CONSUME  - 当前区域吞掉这个动作，本身也不执行默认行为。
 *
 * 这套四态策略从第一轮重构开始沿用；refactor4 已完成实机封版。
 * refactor7 只在同一模型里新增 Start + Movie Context，不改变 Battle/Title/SaveSlot 已验证的策略。
 */

/*
 * 全项目 L/R 方向硬规则（refactor30 起写成公开契约）：
 * - 所有 L 系列输入（LB、LT、D-Pad Left）都只表示“上一个 / 左 / 减”；
 * - 所有 R 系列输入（RB、RT、D-Pad Right）都只表示“下一个 / 右 / 加”。
 *
 * 某个原版页面如果内部 index 的增长方向恰好与画面方向相反，必须由那个页面的 Adapter 做索引换算；
 * 不能反转这里的用户语义。这样以后商店、炼化、主 Interface 等页面才能共用同一套按键直觉。
 */
typedef enum InputAction {
    INPUT_CONFIRM = 0,
    INPUT_CANCEL,
    INPUT_SPECIAL_X,
    INPUT_SPECIAL_Y,
    INPUT_NAV_UP,
    INPUT_NAV_DOWN,
    INPUT_NAV_LEFT,
    INPUT_NAV_RIGHT,
    INPUT_CATEGORY_PREV,
    INPUT_CATEGORY_NEXT,
    INPUT_SUBTYPE_PREV,  /* LT：当前页面的左一个子类型；是否有效由具体页面 Adapter 决定。 */
    INPUT_SUBTYPE_NEXT,  /* RT：当前页面的右一个子类型；没有子类型的页面不会强行赋义。 */
    INPUT_SYSTEM_START,  /* 通用 Start 语义；本版只有 Movie Context 对它做专属覆盖。 */
    INPUT_MOUSE_R3,
    INPUT_MODIFIER_SHIFT, /* Back 固定映射成全插件 Shift 修饰键；单独按下无业务，用于右摇杆慢速与未来组合功能。 */
    INPUT_ACTION_COUNT
} InputAction;

typedef enum InputBindMode {
    INPUT_PASS = 0,
    INPUT_MERGE,
    INPUT_OVERRIDE,
    INPUT_CONSUME
} InputBindMode;

typedef enum InputContext {
    INPUT_CTX_NONE = 0,
    INPUT_CTX_EXPLORATION,
    INPUT_CTX_BATTLE_TOP,
    INPUT_CTX_BATTLE_LIST,
    INPUT_CTX_BATTLE_CONFIRM,
    INPUT_CTX_BATTLE_TARGET,
    INPUT_CTX_BATTLE_RESULT,
    INPUT_CTX_TITLE,
    INPUT_CTX_SAVE_SLOT,
    INPUT_CTX_SAVE_POPUP,
    /* SaveSlot 内嵌“存档/读档/取消”三项窗口；天书与地图存档点共用。 */
    INPUT_CTX_SAVE_ACTION,
    INPUT_CTX_DIALOGUE,
    INPUT_CTX_SCENE_CHOICE,
    INPUT_CTX_CONFIRM_DIALOG,
    INPUT_CTX_MOVIE,
    /* 游戏内主 Interface 根外壳；处理大类、左摇杆50%角色切换与根层返回。 */
    INPUT_CTX_INTERFACE_SHELL,
    /* 主 Interface state2“法宝/道具”页；所有内部列表/子类型/X/弹窗都只在这个 Context 中解释。 */
    INPUT_CTX_INTERFACE_ITEMS,
    /* 主 Interface state3“绝学/法术”页；内部子类型、技能列表、左右翻页和使用确认框由独立 Adapter 解释。 */
    INPUT_CTX_INTERFACE_SKILLS,
    /* 主 Interface state4“及身/装备”页；X栏位、上下候选、A装备、左右与LT/RT原版翻页由独立 Adapter 解释。 */
    INPUT_CTX_INTERFACE_EQUIPMENT,
    /* 主 Interface state5“五内”页；X选节点、LT/RT减/加、Y蕴魂；D-Pad不换人，角色由左摇杆50%统一切换。 */
    INPUT_CTX_INTERFACE_INNER_STATS,
    /* 主 Interface state6“阵形”页；四方向空间导航，A 走原版阵位事件，B 在交换事务中覆盖为取消。 */
    INPUT_CTX_INTERFACE_FORMATION,
    /* 主 Interface state8“机能”页；上下选行，LT/RT 调音量，A 只在空明流转行打开原版 Yes/No。 */
    INPUT_CTX_INTERFACE_OPTIONS,
    /* 客栈“诸态/炼化/歇息”三项根菜单：只给 ↑/↓/A 业务；B 明确没有原版动作。 */
    INPUT_CTX_INN_ROOT,
    /* 炼化第一层：上下物品、左右翻页、LB/RB类别、A进入第二层、B点真实退出图标。 */
    INPUT_CTX_SYNTHESIS_PRIMARY,
    /* 炼化第二层：同类导航；B走右列表自己的取消 Button，而不是鼠标右键。 */
    INPUT_CTX_SYNTHESIS_SECONDARY,
    /* 所有脚本商店共用的左右双列根菜单。 */
    INPUT_CTX_SHOP_ROOT,
    /* 商店买入/卖出数量窗：LT/RT数量、左右确认/取消、A提交。 */
    INPUT_CTX_SHOP_QUANTITY
} InputContext;

/*
 * 同一个动作在 MERGE 模式下会同时出现在 COMMON 与 OVERLAY 两条通道；
 * OVERRIDE 只进入 OVERLAY；PASS 只进入 COMMON；CONSUME 两边都没有。
 * 这就是“通用基础 + 区域叠加/覆盖”在代码里的真实落点。
 */
typedef enum InputRouteLayer {
    INPUT_LAYER_COMMON = 0,
    INPUT_LAYER_OVERLAY
} InputRouteLayer;

typedef struct InputPolicy {
    InputBindMode mode[INPUT_ACTION_COUNT];
} InputPolicy;

/* 返回动作对应的物理键边沿；业务层不再直接读 SDL 数字键。 */
int InputRouter_Pressed(InputAction action);
int InputRouter_Down(InputAction action);
int InputRouter_Released(InputAction action);

/*
 * 模式层专用的原始语义读取：遵守 SwapConfirmCancel，但不受本tick consumed mask影响。
 * 普通页面仍应使用上面的三个接口；只有ControlModes/Investigation这类必须维护物理按住
 * 生命周期的底层状态机才使用 Raw 版本。
 */
int InputRouter_RawPressed(InputAction action);
int InputRouter_RawDown(InputAction action);
int InputRouter_RawReleased(InputAction action);

/*
 * 此组合接口当前专用于 RB+ABXY 战斗快捷键，刻意保持固定物理面键位置，
 * 不跟随 SwapConfirmCancel。普通确认/取消业务不要通过这个接口读取。
 * 判定同时支持“先按住修饰键再按动作键”和“先按住动作键再按修饰键”两种自然手势。
 */
int InputRouter_ChordPressed(InputAction modifier, InputAction action);

/*
 * 左摇杆水平 50% 单次方向沿。
 *
 * 返回值：-1=本帧刚越过左侧 50%；+1=本帧刚越过右侧 50%；0=没有新的方向沿。
 * 这个接口只表达“明显向左/向右推了一次”的模拟输入语义，不知道它最终是切角色还是别的业务。
 * 一次越阈只触发一次；摇杆必须先回到中间死区，才允许下一次触发。这样玩家持续推住不会每 8ms 疯狂换人。
 */
int InputRouter_LeftStickHorizontalStep50(void);

/*
 * 每个 worker tick 开始时清空“本帧已被更高层 Context 消费”的动作标记。
 * 例如 Yes/No 询问框已经用掉 A 后，底下的战斗列表就不应该在同一个 8 ms tick 再收到同一个 A。
 * 只有新叠加层主动调用 Consume 才会产生影响；没有叠加层时 refactor7 原有读取行为完全不变。
 */
void InputRouter_BeginFrame(void);
void InputRouter_Consume(InputAction action);
void InputRouter_CaptureAll(void);

/* Context 策略用于明确“通用+叠加”还是“专属覆盖”。 */
const InputPolicy* InputRouter_Policy(InputContext context);
InputBindMode InputRouter_Mode(InputContext context, InputAction action);

/*
 * 业务模块应优先使用这三个“带 Context + 通道”的入口。
 * 例如 Battle 列表的 A 属于 OVERLAY；未来若某 Context 把 A 设为 MERGE，
 * 通用处理器和专属处理器就可以分别从 COMMON / OVERLAY 各收到一次同一物理边沿。
 */
int InputRouter_PressedOn(InputContext context, InputAction action, InputRouteLayer layer);
int InputRouter_DownOn(InputContext context, InputAction action, InputRouteLayer layer);
int InputRouter_ReleasedOn(InputContext context, InputAction action, InputRouteLayer layer);

#endif /* CASTLE_PAD_INPUT_ROUTER_H */
