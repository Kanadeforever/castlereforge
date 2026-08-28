#ifndef CASTLE_PADSUPPORT_PUBLIC_API_H
#define CASTLE_PADSUPPORT_PUBLIC_API_H

// ============================================================================
// Castle_PadSupport_API.h
// ----------------------------------------------------------------------------
// 这是 CastleReforge 当前公开 PadSupport API v1 的“消费端最小 ABI 副本”。
//
// ABI 可以理解成“两个独立 DLL 之间约好的插头形状”：字段顺序、数字编号和调用约定必须完全
// 一致，否则 SaveEnhance 以为自己在读 ButtonDown，实际可能跳到完全不同的函数地址。
//
// SaveEnhance 不把 Controller 当成强制依赖：
// - 没安装 Castle_PadSupport.asi：F5/F9 继续正常；
// - 装了并且公开 API v1 合法：增加 RB+R3 / RB+Start；
// - API 不合法：只关闭手柄联动，不让游戏因为辅助功能崩溃。
//
// 为什么不直接读 Controller 私有全局变量：私有实现可以随着 r37/r38 等版本重构，公开 API
// 才是给其它插件使用的稳定边界。SaveEnhance 只“读取状态”，绝不修改 PadSupport 内部数据。
// ============================================================================

#if defined(_MSC_VER) || defined(__clang__)
// __cdecl 是当前公开 API 规定的调用约定。调用约定决定“参数怎么放、谁清理栈”。
#define CASTLE_PAD_API_CALL __cdecl
#else
#define CASTLE_PAD_API_CALL
#endif

// API 只需要固定 32 位整数。这里不用 Windows DWORD，是为了让公共头保持独立。
typedef unsigned long CastlePadU32;
typedef signed long CastlePadS32;

// magic 是识别“这真的是 Pad API 表”的四字节签名，避免把错误指针当函数表使用。
#define CASTLE_PAD_API_MAGIC 0x44415043ul

// SaveEnhance 当前只认识 v1；未来 v2 若改结构，必须显式适配，而不是盲目继续调用。
#define CASTLE_PAD_API_VERSION_1 1ul

// capability flags 描述 API 能提供哪些大类能力。SaveEnhance 目前主要使用物理输入和控制门。
#define CASTLE_PAD_CAP_PHYSICAL_INPUT 0x00000001ul
#define CASTLE_PAD_CAP_SEMANTIC_INPUT 0x00000002ul
#define CASTLE_PAD_CAP_CONTROL_GATE 0x00000004ul

// 下面的数字顺序必须和 PadSupport 官方头完全一致。
// SaveEnhance 真正使用 START=5、R3=6、RB=8，但保留完整按钮枚举可以避免“手写魔法数字”。
typedef enum CastlePadButton {
    CASTLE_PAD_BUTTON_SOUTH = 0, // Xbox A / 南键。
    CASTLE_PAD_BUTTON_EAST,      // Xbox B / 东键。
    CASTLE_PAD_BUTTON_WEST,      // Xbox X / 西键。
    CASTLE_PAD_BUTTON_NORTH,     // Xbox Y / 北键。
    CASTLE_PAD_BUTTON_BACK,      // Back/View。
    CASTLE_PAD_BUTTON_START,     // Start/Menu；Quick Load 组合的第二个键。
    CASTLE_PAD_BUTTON_R3,        // 右摇杆按下；Quick Save 组合的第二个键。
    CASTLE_PAD_BUTTON_LB,        // 左肩键。
    CASTLE_PAD_BUTTON_RB,        // 右肩键；两个 SaveEnhance 组合都要求先/同时按住它。
    CASTLE_PAD_BUTTON_DPAD_UP,
    CASTLE_PAD_BUTTON_DPAD_DOWN,
    CASTLE_PAD_BUTTON_DPAD_LEFT,
    CASTLE_PAD_BUTTON_DPAD_RIGHT,
    CASTLE_PAD_BUTTON_LT,
    CASTLE_PAD_BUTTON_RT,
    CASTLE_PAD_BUTTON_COUNT
} CastlePadButton;

// 语义动作编号来自 PadSupport 的公开 InputRouter 快照。
// 与物理按钮相比，CONFIRM/CANCEL 会自动遵守 SwapConfirmCancel，所以保留槽动作窗口必须用它。
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

// 这里只需要识别普通手柄模式。其它三个模式都属于鼠标/调查独占，不应强制动作焦点。
typedef enum CastlePadControlMode {
    CASTLE_PAD_CONTROL_CONTROLLER = 0,
    CASTLE_PAD_CONTROL_PERSISTENT_MOUSE = 1,
    CASTLE_PAD_CONTROL_TEMP_MOUSE = 2,
    CASTLE_PAD_CONTROL_INVESTIGATION = 3,
    CASTLE_PAD_CONTROL_UNKNOWN = 0x7FFFFFFF
} CastlePadControlMode;

// 这个结构体本身不保存某一帧的按钮值，而是一张“函数地址表”。
// SaveEnhance 取得指针后，通过 api->ButtonDown(...) 去询问 Controller 当前快照。
typedef struct CastlePadApiV1 {
    CastlePadU32 magic;            // 必须等于 CASTLE_PAD_API_MAGIC。
    CastlePadU32 struct_size;      // 实际结构大小；防止旧 DLL 返回过短表。
    CastlePadU32 api_version;      // 当前必须是 1。
    CastlePadU32 capability_flags; // 功能能力位；保留给调用者判断。

    // IsReady：PadSupport 是否完成初始化。模块存在不代表内部已经准备好。
    int (CASTLE_PAD_API_CALL *IsReady)(void);

    // IsConnected：当前是否真的有受支持 Controller 连接。
    int (CASTLE_PAD_API_CALL *IsConnected)(void);

    // GameForeground：游戏不在前台时 SaveEnhance 不应该响应后台手柄组合。
    int (CASTLE_PAD_API_CALL *GameForeground)(void);

    // ButtonDown：当前这一刻按钮是否按住。SaveEnhance 用它组合 RB+R3 / RB+Start，
    // 再由自己记录“上一帧组合是否已经按住”，只在 false->true 时触发一次。
    int (CASTLE_PAD_API_CALL *ButtonDown)(CastlePadU32 button);

    // ButtonPressed / ButtonReleased 是单个采样周期的边沿。SaveEnhance 当前不依赖它们，
    // 但保留字段是因为 ABI 后续字段的位置取决于它们仍然存在。
    int (CASTLE_PAD_API_CALL *ButtonPressed)(CastlePadU32 button);
    int (CASTLE_PAD_API_CALL *ButtonReleased)(CastlePadU32 button);

    // GetAxis 返回摇杆/扳机轴。SaveEnhance 不使用，但不能从结构中删掉。
    CastlePadS32 (CASTLE_PAD_API_CALL *GetAxis)(CastlePadU32 axis);

    // Action* 是 PadSupport 自己的语义动作层；SaveEnhance 使用物理组合，因此当前不调用。
    int (CASTLE_PAD_API_CALL *ActionDown)(CastlePadU32 action);
    int (CASTLE_PAD_API_CALL *ActionPressed)(CastlePadU32 action);
    int (CASTLE_PAD_API_CALL *ActionReleased)(CastlePadU32 action);

    // GetControlMode 可以知道当前键鼠/手柄模式；字段必须保留以维持 ABI。
    CastlePadU32 (CASTLE_PAD_API_CALL *GetControlMode)(void);

    // AllowsExternalUiInput 是最重要的控制门之一：PadSupport 认为外部插件现在不该接收
    // UI/Controller 输入时，SaveEnhance 就不接受 RB 组合，避免和菜单/模式切换抢输入。
    int (CASTLE_PAD_API_CALL *AllowsExternalUiInput)(void);
} CastlePadApiV1;

// PadSupport 导出 CastlePad_GetApi。GetProcAddress 得到的函数地址转换成这个类型后，
// 传入 requestedVersion=1，就能请求上面的 v1 函数表。
typedef const CastlePadApiV1* (CASTLE_PAD_API_CALL *CastlePadGetApiFn)(
    CastlePadU32 requestedVersion);

#endif // CASTLE_PADSUPPORT_PUBLIC_API_H
