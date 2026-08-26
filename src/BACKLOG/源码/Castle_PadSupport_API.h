#ifndef CASTLE_PADSUPPORT_PUBLIC_API_H
#define CASTLE_PADSUPPORT_PUBLIC_API_H

/*
 * Castle_PadSupport_API.h
 *
 * 这是 Castle_PadSupport.asi 对所有外部 ASI 提供的“稳定公共接口”头文件。
 * 它不是 Backlog 私有协议；任何独立插件都可以只复制这一份头文件，然后通过
 * GetModuleHandleA + GetProcAddress 取得 CastlePad_GetApi。
 *
 * 为什么不用外部插件直接读取 PadSupport 的全局变量：
 * ------------------------------------------------------
 * C 结构体内部字段会随着功能增加而变化。例如手柄插件后来增加震动函数指针时，
 * PadInputState 的 buttons 偏移就发生过变化。外部插件如果记住某个偏移，升级 PadSupport
 * 后就会读错内存。公共 API 只暴露函数表，因此 PadSupport 内部可以继续重构，API v1
 * 的字段顺序和含义保持不变即可。
 *
 * ABI 规则：
 * ---------
 * 1. 当前游戏和 ASI 都是 32 位 x86 Windows，所以所有整数都使用明确的 32/16 位别名。
 * 2. 所有函数统一使用 __cdecl；导出名由 .def 固定成未修饰的 CastlePad_GetApi。
 * 3. API v1 只提供“读取”能力，不允许外部插件注入按钮、修改 ControlMode 或直接操作 SDL。
 * 4. 新功能只能在未来 API v2 中追加；v1 结构体已有字段的顺序、类型和语义不得改变。
 * 5. 查询函数允许从其它 ASI 的 worker 线程调用；provider 返回的是 PadSupport worker 已发布的
 *    只读快照，不要求第三方和 PadSupport 使用同一个线程，也不把内部 g_pad/g_modes 指针暴露出去。
 */

#if defined(_MSC_VER) || defined(__clang__)
#define CASTLE_PAD_API_CALL __cdecl
#else
#define CASTLE_PAD_API_CALL
#endif

typedef unsigned long CastlePadU32;
typedef signed long CastlePadS32;

/* 用来快速拒绝“碰巧取得了错误地址/错误结构”的固定魔数：ASCII 'CPAD'。 */
#define CASTLE_PAD_API_MAGIC 0x44415043ul
#define CASTLE_PAD_API_VERSION_1 1ul

/*
 * capability_flags 告诉调用者当前 API 表具备哪些类别。
 * v1 provider 会同时设置下面三位；未来可以增加新位，但不能重新解释旧位。
 */
#define CASTLE_PAD_CAP_PHYSICAL_INPUT 0x00000001ul
#define CASTLE_PAD_CAP_SEMANTIC_INPUT 0x00000002ul
#define CASTLE_PAD_CAP_CONTROL_GATE   0x00000004ul

/*
 * 公共物理按钮编号是 ABI 自己的编号，不等于 SDL 枚举，也不要求等于 PadSupport 内部 PadButton。
 * provider 内部会做 switch 翻译。这样未来即使 PadSupport 重排内部 enum，外部插件也不受影响。
 */
typedef enum CastlePadButton {
    CASTLE_PAD_BUTTON_SOUTH = 0,
    CASTLE_PAD_BUTTON_EAST,
    CASTLE_PAD_BUTTON_WEST,
    CASTLE_PAD_BUTTON_NORTH,
    CASTLE_PAD_BUTTON_BACK,
    CASTLE_PAD_BUTTON_START,
    CASTLE_PAD_BUTTON_R3,
    CASTLE_PAD_BUTTON_LB,
    CASTLE_PAD_BUTTON_RB,
    CASTLE_PAD_BUTTON_DPAD_UP,
    CASTLE_PAD_BUTTON_DPAD_DOWN,
    CASTLE_PAD_BUTTON_DPAD_LEFT,
    CASTLE_PAD_BUTTON_DPAD_RIGHT,
    CASTLE_PAD_BUTTON_LT,
    CASTLE_PAD_BUTTON_RT,
    CASTLE_PAD_BUTTON_COUNT
} CastlePadButton;

/* 轴统一返回有符号 32 位数；当前 PadSupport 的 SDL3 原始值实际落在 -32768..32767。 */
typedef enum CastlePadAxis {
    CASTLE_PAD_AXIS_LEFT_X = 0,
    CASTLE_PAD_AXIS_LEFT_Y,
    CASTLE_PAD_AXIS_RIGHT_X,
    CASTLE_PAD_AXIS_RIGHT_Y,
    CASTLE_PAD_AXIS_LEFT_TRIGGER,
    CASTLE_PAD_AXIS_RIGHT_TRIGGER,
    CASTLE_PAD_AXIS_COUNT
} CastlePadAxis;

/*
 * 语义动作来自 PadSupport 的 InputRouter，而不是固定物理位置。
 * 最重要的例子是 CONFIRM/CANCEL：它们会自动遵守 SwapConfirmCancel。
 * 外部插件如果想表达“取消”，应优先查询 CASTLE_PAD_ACTION_CANCEL，而不是硬写 East/B。
 */
typedef enum CastlePadAction {
    CASTLE_PAD_ACTION_CONFIRM = 0,
    CASTLE_PAD_ACTION_CANCEL,
    CASTLE_PAD_ACTION_SPECIAL_X,
    CASTLE_PAD_ACTION_SPECIAL_Y,
    CASTLE_PAD_ACTION_NAV_UP,
    CASTLE_PAD_ACTION_NAV_DOWN,
    CASTLE_PAD_ACTION_NAV_LEFT,
    CASTLE_PAD_ACTION_NAV_RIGHT,
    CASTLE_PAD_ACTION_CATEGORY_PREV,
    CASTLE_PAD_ACTION_CATEGORY_NEXT,
    CASTLE_PAD_ACTION_SUBTYPE_PREV,
    CASTLE_PAD_ACTION_SUBTYPE_NEXT,
    CASTLE_PAD_ACTION_SYSTEM_START,
    CASTLE_PAD_ACTION_MOUSE_R3,
    CASTLE_PAD_ACTION_MODIFIER_SHIFT,
    CASTLE_PAD_ACTION_COUNT
} CastlePadAction;

/*
 * 公共控制模式使用自己的稳定编号，不直接暴露 PadSupport 内部 ControlMode enum。
 * provider 内部通过显式 switch 映射，所以以后内部 enum 即使重排，这些公开数值也保持不变。
 *
 * 普通外部 UI 一般不需要自己判断四种模式，优先调用 AllowsExternalUiInput()；
 * GetControlMode() 是给调试器、提示 UI 或确实需要区分“为什么被阻断”的插件使用。
 */
typedef enum CastlePadControlMode {
    CASTLE_PAD_CONTROL_CONTROLLER       = 0,
    CASTLE_PAD_CONTROL_PERSISTENT_MOUSE = 1,
    CASTLE_PAD_CONTROL_TEMP_MOUSE       = 2,
    CASTLE_PAD_CONTROL_INVESTIGATION    = 3,
    CASTLE_PAD_CONTROL_UNKNOWN          = 0x7FFFFFFF
} CastlePadControlMode;

/*
 * API v1 函数表。
 *
 * struct_size 必须由 provider 填 sizeof(CastlePadApiV1)。调用者先检查它，再访问后面的字段。
 * 这样未来即使 API v2 在尾部增加新字段，旧插件仍能安全读取 v1 已知部分。
 */
typedef struct CastlePadApiV1 {
    CastlePadU32 magic;
    CastlePadU32 struct_size;
    CastlePadU32 api_version;
    CastlePadU32 capability_flags;

    /* SDL 输入层是否已经成功初始化。没有 SDL3 时返回 0，但 ASI 本身仍可能已经加载。 */
    int (CASTLE_PAD_API_CALL *IsReady)(void);

    /* 当前是否有已打开的 SDL Gamepad。拔掉手柄后返回 0。 */
    int (CASTLE_PAD_API_CALL *IsConnected)(void);

    /* RPG.exe 是否为当前前台进程。外部插件做主动 UI 操作前应尊重这一层。 */
    int (CASTLE_PAD_API_CALL *GameForeground)(void);

    /* 物理按钮查询：Down=持续按住，Pressed=本采样帧按下沿，Released=本采样帧松开沿。 */
    int (CASTLE_PAD_API_CALL *ButtonDown)(CastlePadU32 button);
    int (CASTLE_PAD_API_CALL *ButtonPressed)(CastlePadU32 button);
    int (CASTLE_PAD_API_CALL *ButtonReleased)(CastlePadU32 button);

    /* 返回标准化为 32 位有符号整数的物理轴值；非法 axis 返回 0。 */
    CastlePadS32 (CASTLE_PAD_API_CALL *GetAxis)(CastlePadU32 axis);

    /*
     * 语义动作查询使用 InputRouter_Raw*：
     * - 会遵守 SwapConfirmCancel；
     * - 不受 PadSupport 同 tick consumed mask 影响；
     * - 只读取，不会 Consume，也不会改变 PadSupport 的业务状态。
     */
    int (CASTLE_PAD_API_CALL *ActionDown)(CastlePadU32 action);
    int (CASTLE_PAD_API_CALL *ActionPressed)(CastlePadU32 action);
    int (CASTLE_PAD_API_CALL *ActionReleased)(CastlePadU32 action);

    /* 当前控制模式的稳定公开编号；未知状态返回 CASTLE_PAD_CONTROL_UNKNOWN。 */
    CastlePadU32 (CASTLE_PAD_API_CALL *GetControlMode)(void);

    /*
     * 推荐给外部菜单/覆盖层使用的总门：只有“API ready + 手柄连接 + 游戏前台 + 普通手柄模式”
     * 才返回 1。调查、RT 鼠标、Back 常驻鼠标都会返回 0，让这些模式独占输入。
     */
    int (CASTLE_PAD_API_CALL *AllowsExternalUiInput)(void);
} CastlePadApiV1;

/*
 * 唯一公开导出。
 *
 * 外部插件不要静态链接 Castle_PadSupport.lib；应在运行时：
 *   1. GetModuleHandleA("Castle_PadSupport.asi")；
 *   2. GetProcAddress(..., "CastlePad_GetApi")；
 *   3. 调 get_api(CASTLE_PAD_API_VERSION_1)。
 *
 * 返回 NULL 表示该版本不支持。这样不同插件可以和不同版本 PadSupport 安全共存。
 */
typedef const CastlePadApiV1* (CASTLE_PAD_API_CALL *CastlePadGetApiFn)(CastlePadU32 requested_version);

#endif /* CASTLE_PADSUPPORT_PUBLIC_API_H */
