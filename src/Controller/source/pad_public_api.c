#include "Castle_PadSupport_API.h"
#include "pad_public_api.h"
#include "pad_input.h"
#include "input_router.h"
#include "control_modes.h"
#include "runtime.h"
#include "CastleInput_API.h"

/*
 * pad_public_api.c
 *
 * 这个文件是 Castle_PadSupport.asi 的公共、版本化、只读 C ABI 实现。
 *
 * 整体数据流只有一条：
 *
 *   SDL3
 *     ↓
 *   PadInput                 —— PadSupport 自己的物理输入层
 *     ↓
 *   InputRouter/ControlModes —— PadSupport 自己的语义和模式裁决
 *     ↓
 *   本文件在 PadSupport worker 中发布一份“只读快照”
 *     ↓
 *   CastlePad_GetApi(v1)
 *     ↓
 *   Backlog / 未来其它 ASI
 *
 * 外部插件不会得到 g_pad、g_modes、SDL_Gamepad* 或任何内部结构体指针。
 * 以后 PadSupport 内部重构时，只要继续把同样的语义发布进 API v1，旧插件就无需修改。
 */

/*
 * 所有公开按钮/动作数量都远小于 32，因此可以压进一个 u32 bitset。
 * bit N 为 1，就表示公共编号 N 当前满足该状态。
 */
typedef struct CastlePadPublishedSnapshot {
    /*
     * generation 是一个很小的“写入进行中”标记：
     * - 偶数：这一帧快照已经完整发布；
     * - 奇数：PadSupport worker 正在改快照，外部读取函数应该重试。
     *
     * x86 对对齐 32 位读写不会撕裂；这里的字段全部都是 32 位，并声明 volatile，
     * 让编译器每次都真的从内存读/写，而不是把旧值长期留在寄存器里。
     */
    volatile CastlePadU32 generation;

    volatile CastlePadU32 ready;
    volatile CastlePadU32 connected;
    volatile CastlePadU32 foreground;

    volatile CastlePadU32 button_down;
    volatile CastlePadU32 button_pressed;
    volatile CastlePadU32 button_released;

    volatile CastlePadS32 axes[CASTLE_PAD_AXIS_COUNT];

    volatile CastlePadU32 action_down;
    volatile CastlePadU32 action_pressed;
    volatile CastlePadU32 action_released;

    volatile CastlePadU32 control_mode;
    volatile CastlePadU32 allows_external_ui_input;
} CastlePadPublishedSnapshot;

/* 静态区会在 DLL 加载时归零；Reset 还会把 UNKNOWN 等非零默认值补正确。 */
static CastlePadPublishedSnapshot g_public_snapshot;
static const CastleInputApiV1* g_runtime_input_api;
static CastleProviderHandle g_runtime_input_provider;

/*
 * 公共按钮编号与当前内部 PadButton 的翻译。
 * 绝不能直接 cast 两个 enum：公共编号一旦发布就要长期稳定，内部 enum 以后可以自由调整。
 */
static int public_button_to_internal(CastlePadU32 public_button, PadButton* out_button) {
    PadButton value;

    if (!out_button) return 0;

    switch (public_button) {
    case CASTLE_PAD_BUTTON_SOUTH:      value = PAD_SOUTH; break;
    case CASTLE_PAD_BUTTON_EAST:       value = PAD_EAST; break;
    case CASTLE_PAD_BUTTON_WEST:       value = PAD_WEST; break;
    case CASTLE_PAD_BUTTON_NORTH:      value = PAD_NORTH; break;
    case CASTLE_PAD_BUTTON_BACK:       value = PAD_BACK; break;
    case CASTLE_PAD_BUTTON_START:      value = PAD_START; break;
    case CASTLE_PAD_BUTTON_R3:         value = PAD_R3; break;
    case CASTLE_PAD_BUTTON_LB:         value = PAD_LB; break;
    case CASTLE_PAD_BUTTON_RB:         value = PAD_RB; break;
    case CASTLE_PAD_BUTTON_DPAD_UP:    value = PAD_DPAD_UP; break;
    case CASTLE_PAD_BUTTON_DPAD_DOWN:  value = PAD_DPAD_DOWN; break;
    case CASTLE_PAD_BUTTON_DPAD_LEFT:  value = PAD_DPAD_LEFT; break;
    case CASTLE_PAD_BUTTON_DPAD_RIGHT: value = PAD_DPAD_RIGHT; break;
    case CASTLE_PAD_BUTTON_LT:         value = PAD_LT; break;
    case CASTLE_PAD_BUTTON_RT:         value = PAD_RT; break;
    default:
        return 0;
    }

    *out_button = value;
    return 1;
}

/* 公共轴编号与 PadInput 内部 PadAxis 的翻译。 */
static int public_axis_to_internal(CastlePadU32 public_axis, PadAxis* out_axis) {
    PadAxis value;

    if (!out_axis) return 0;

    switch (public_axis) {
    case CASTLE_PAD_AXIS_LEFT_X:        value = PAD_AXIS_LEFT_X; break;
    case CASTLE_PAD_AXIS_LEFT_Y:        value = PAD_AXIS_LEFT_Y; break;
    case CASTLE_PAD_AXIS_RIGHT_X:       value = PAD_AXIS_RIGHT_X; break;
    case CASTLE_PAD_AXIS_RIGHT_Y:       value = PAD_AXIS_RIGHT_Y; break;
    case CASTLE_PAD_AXIS_LEFT_TRIGGER:  value = PAD_AXIS_LEFT_TRIGGER; break;
    case CASTLE_PAD_AXIS_RIGHT_TRIGGER: value = PAD_AXIS_RIGHT_TRIGGER; break;
    default:
        return 0;
    }

    *out_axis = value;
    return 1;
}

/*
 * 公共语义动作与 InputRouter 当前内部 InputAction 的翻译。
 * Confirm/Cancel 经过 InputRouter_Raw* 后会自动遵守 SwapConfirmCancel。
 */
static int public_action_to_internal(CastlePadU32 public_action, InputAction* out_action) {
    InputAction value;

    if (!out_action) return 0;

    switch (public_action) {
    case CASTLE_PAD_ACTION_CONFIRM:        value = INPUT_CONFIRM; break;
    case CASTLE_PAD_ACTION_CANCEL:         value = INPUT_CANCEL; break;
    case CASTLE_PAD_ACTION_SPECIAL_X:      value = INPUT_SPECIAL_X; break;
    case CASTLE_PAD_ACTION_SPECIAL_Y:      value = INPUT_SPECIAL_Y; break;
    case CASTLE_PAD_ACTION_NAV_UP:         value = INPUT_NAV_UP; break;
    case CASTLE_PAD_ACTION_NAV_DOWN:       value = INPUT_NAV_DOWN; break;
    case CASTLE_PAD_ACTION_NAV_LEFT:       value = INPUT_NAV_LEFT; break;
    case CASTLE_PAD_ACTION_NAV_RIGHT:      value = INPUT_NAV_RIGHT; break;
    case CASTLE_PAD_ACTION_CATEGORY_PREV:  value = INPUT_CATEGORY_PREV; break;
    case CASTLE_PAD_ACTION_CATEGORY_NEXT:  value = INPUT_CATEGORY_NEXT; break;
    case CASTLE_PAD_ACTION_SUBTYPE_PREV:   value = INPUT_SUBTYPE_PREV; break;
    case CASTLE_PAD_ACTION_SUBTYPE_NEXT:   value = INPUT_SUBTYPE_NEXT; break;
    case CASTLE_PAD_ACTION_SYSTEM_START:   value = INPUT_SYSTEM_START; break;
    case CASTLE_PAD_ACTION_MOUSE_R3:       value = INPUT_MOUSE_R3; break;
    case CASTLE_PAD_ACTION_MODIFIER_SHIFT: value = INPUT_MODIFIER_SHIFT; break;
    default:
        return 0;
    }

    *out_action = value;
    return 1;
}

/*
 * 发布开始/结束各把 generation 加 1。
 * 单写线程是 PadSupport 自己的 worker，因此不需要多个 writer 之间再抢锁。
 */
static void public_publish_begin(void) {
    g_public_snapshot.generation += 1u;
}

static void public_publish_end(void) {
    g_public_snapshot.generation += 1u;
}

/*
 * 读取一个 32 位公开字段。
 * 如果恰好撞上 worker 正在发布，就重新读；这样不会把“上一帧的一半 + 下一帧的一半”混在一起。
 */
static CastlePadU32 public_read_u32(volatile CastlePadU32* field) {
    CastlePadU32 before;
    CastlePadU32 after;
    CastlePadU32 value;

    do {
        before = g_public_snapshot.generation;
        if ((before & 1u) != 0u) continue;
        value = *field;
        after = g_public_snapshot.generation;
    } while (before != after || (after & 1u) != 0u);

    return value;
}

/* i32 与 u32 在当前 x86 ABI 下同为 32 位，读取方法相同，只保留有符号返回类型。 */
static CastlePadS32 public_read_s32(volatile CastlePadS32* field) {
    CastlePadU32 before;
    CastlePadU32 after;
    CastlePadS32 value;

    do {
        before = g_public_snapshot.generation;
        if ((before & 1u) != 0u) continue;
        value = *field;
        after = g_public_snapshot.generation;
    } while (before != after || (after & 1u) != 0u);

    return value;
}

/*
 * Runtime 一次要取完整快照，所以这里不能逐字段调用 public_read_u32：字段之间可能跨过
 * 一次 worker 发布。我们在同一对偶数 generation 之间复制全部标量，保证整帧一致。
 */
static CastleResult CASTLE_RUNTIME_CALL public_copy_runtime_snapshot(
    CastleInputSnapshotV1* out_snapshot) {
    CastlePadU32 before;
    CastlePadU32 after;
    CastlePadU32 axis;
    if (!out_snapshot || out_snapshot->magic != CASTLE_INPUT_SNAPSHOT_MAGIC ||
        out_snapshot->struct_size < CASTLE_SIZEOF_INPUT_SNAPSHOT_V1 ||
        out_snapshot->version != CASTLE_INPUT_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    do {
        before = g_public_snapshot.generation;
        if ((before & 1u) != 0u) continue;
        out_snapshot->flags = 0u;
        out_snapshot->generation = before;
        out_snapshot->ready = g_public_snapshot.ready;
        out_snapshot->connected = g_public_snapshot.connected;
        out_snapshot->game_foreground = g_public_snapshot.foreground;
        out_snapshot->control_mode = g_public_snapshot.control_mode;
        out_snapshot->allows_external_ui_input =
            g_public_snapshot.allows_external_ui_input;
        out_snapshot->button_down = g_public_snapshot.button_down;
        out_snapshot->button_pressed = g_public_snapshot.button_pressed;
        out_snapshot->button_released = g_public_snapshot.button_released;
        out_snapshot->action_down = g_public_snapshot.action_down;
        out_snapshot->action_pressed = g_public_snapshot.action_pressed;
        out_snapshot->action_released = g_public_snapshot.action_released;
        for (axis = 0u; axis < CASTLE_INPUT_AXIS_COUNT; ++axis) {
            out_snapshot->axes[axis] = g_public_snapshot.axes[axis];
        }
        after = g_public_snapshot.generation;
    } while (before != after || (after & 1u) != 0u);
    out_snapshot->generation = after;
    return CASTLE_OK;
}

static const CastleInputProviderV1 g_runtime_input_provider_api = {
    CASTLE_INPUT_PROVIDER_MAGIC,
    CASTLE_SIZEOF_INPUT_PROVIDER_V1,
    CASTLE_INPUT_API_VERSION_1,
    CASTLE_INPUT_CAP_PHYSICAL_SNAPSHOT | CASTLE_INPUT_CAP_SEMANTIC_SNAPSHOT,
    public_copy_runtime_snapshot
};

int CastlePad_RegisterRuntimeInputProvider(const CastleRuntimeApiV1* runtime_api,
                                           CastlePluginHandle plugin_handle) {
    static const char interface_id[] = CASTLE_INPUT_INTERFACE_ID;
    static const char provider_id_text[] = "org.castlereforge.padsupport.input";
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 result = {0};
    CastleStringView provider_id;
    if (!runtime_api || !runtime_api->QueryInterface || plugin_handle == 0u) return 0;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id.data = interface_id;
    query.interface_id.length = (CastleU32)(sizeof(interface_id) - 1u);
    query.requested_version = CASTLE_INPUT_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_INPUT_API_V1;
    query.required_capabilities_low = CASTLE_INPUT_CAP_EXTERNAL_PROVIDER;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (runtime_api->QueryInterface(&query, &result) != CASTLE_OK ||
        !result.api_pointer) return 0;
    g_runtime_input_api = (const CastleInputApiV1*)result.api_pointer;
    provider_id.data = provider_id_text;
    provider_id.length = (CastleU32)(sizeof(provider_id_text) - 1u);
    if (g_runtime_input_api->RegisterInputProvider(plugin_handle, provider_id,
            &g_runtime_input_provider_api, &g_runtime_input_provider) != CASTLE_OK ||
        g_runtime_input_provider == 0u) {
        g_runtime_input_api = NULL;
        return 0;
    }
    if (g_runtime_input_api->SetInputProviderReady(g_runtime_input_provider, 1u) !=
        CASTLE_OK) {
        g_runtime_input_api = NULL;
        g_runtime_input_provider = 0u;
        return 0;
    }
    return 1;
}

void CastlePad_RuntimeInputProviderShutdown(void) {
    if (g_runtime_input_api && g_runtime_input_provider) {
        g_runtime_input_api->SetInputProviderReady(g_runtime_input_provider, 0u);
    }
    g_runtime_input_provider = 0u;
    g_runtime_input_api = NULL;
}

void CastlePad_PublicApiReset(void) {
    CastlePadU32 index;

    public_publish_begin();

    g_public_snapshot.ready = 0u;
    g_public_snapshot.connected = 0u;
    g_public_snapshot.foreground = 0u;

    g_public_snapshot.button_down = 0u;
    g_public_snapshot.button_pressed = 0u;
    g_public_snapshot.button_released = 0u;

    for (index = 0u; index < (CastlePadU32)CASTLE_PAD_AXIS_COUNT; ++index) {
        g_public_snapshot.axes[index] = 0;
    }

    g_public_snapshot.action_down = 0u;
    g_public_snapshot.action_pressed = 0u;
    g_public_snapshot.action_released = 0u;

    g_public_snapshot.control_mode = (CastlePadU32)CASTLE_PAD_CONTROL_UNKNOWN;
    g_public_snapshot.allows_external_ui_input = 0u;

    public_publish_end();
}

void CastlePad_PublicApiPublishFrame(void) {
    CastlePadU32 public_button;
    CastlePadU32 public_axis;
    CastlePadU32 public_action;
    CastlePadU32 button_down = 0u;
    CastlePadU32 button_pressed = 0u;
    CastlePadU32 button_released = 0u;
    CastlePadU32 action_down = 0u;
    CastlePadU32 action_pressed = 0u;
    CastlePadU32 action_released = 0u;
    CastlePadS32 axes[CASTLE_PAD_AXIS_COUNT];
    CastlePadU32 ready;
    CastlePadU32 connected;
    CastlePadU32 foreground;
    CastlePadU32 blocked;

    /*
     * 先在局部变量里完整收集一帧，再一次性发布。
     * 这样 generation 处于奇数的时间很短，外部线程通常一次就能读成功。
     */
    ready = PadInput_Ready() ? 1u : 0u;
    connected = PadInput_GamepadConnected() ? 1u : 0u;
    foreground = PadInput_GameForeground(NULL) ? 1u : 0u;
    blocked = ControlModes_BlocksMapMovement() ? 1u : 0u;

    for (public_button = 0u;
         public_button < (CastlePadU32)CASTLE_PAD_BUTTON_COUNT;
         ++public_button) {
        PadButton internal_button;
        CastlePadU32 bit = 1u << public_button;

        if (!public_button_to_internal(public_button, &internal_button)) continue;
        if (PadInput_Down(internal_button)) button_down |= bit;
        if (PadInput_Pressed(internal_button)) button_pressed |= bit;
        if (PadInput_Released(internal_button)) button_released |= bit;
    }

    for (public_axis = 0u;
         public_axis < (CastlePadU32)CASTLE_PAD_AXIS_COUNT;
         ++public_axis) {
        PadAxis internal_axis;
        axes[public_axis] = 0;
        if (public_axis_to_internal(public_axis, &internal_axis)) {
            axes[public_axis] = (CastlePadS32)PadInput_Axis(internal_axis);
        }
    }

    for (public_action = 0u;
         public_action < (CastlePadU32)CASTLE_PAD_ACTION_COUNT;
         ++public_action) {
        InputAction internal_action;
        CastlePadU32 bit = 1u << public_action;

        if (!public_action_to_internal(public_action, &internal_action)) continue;

        /*
         * Raw* 只表示“本轮映射后的语义状态”，不会写 consumed mask。
         * 因而发布公共快照不会抢走 PadSupport 自己页面的 A/B/方向输入。
         */
        if (InputRouter_RawDown(internal_action)) action_down |= bit;
        if (InputRouter_RawPressed(internal_action)) action_pressed |= bit;
        if (InputRouter_RawReleased(internal_action)) action_released |= bit;
    }

    public_publish_begin();

    g_public_snapshot.ready = ready;
    g_public_snapshot.connected = connected;
    g_public_snapshot.foreground = foreground;

    g_public_snapshot.button_down = button_down;
    g_public_snapshot.button_pressed = button_pressed;
    g_public_snapshot.button_released = button_released;

    for (public_axis = 0u;
         public_axis < (CastlePadU32)CASTLE_PAD_AXIS_COUNT;
         ++public_axis) {
        g_public_snapshot.axes[public_axis] = axes[public_axis];
    }

    g_public_snapshot.action_down = action_down;
    g_public_snapshot.action_pressed = action_pressed;
    g_public_snapshot.action_released = action_released;

    /*
     * ControlModes_PublicMode() 已把内部状态显式映射成 Public API 的稳定 0..3 编号。
     * 不直接把内部 enum 强转出去，避免未来内部重排破坏第三方。
     */
    g_public_snapshot.control_mode = (CastlePadU32)ControlModes_PublicMode();

    g_public_snapshot.allows_external_ui_input =
        ready && connected && foreground && !blocked ? 1u : 0u;

    public_publish_end();
}

/* 下面开始都是第三方真正会调用的只读查询。 */
static int CASTLE_PAD_API_CALL public_is_ready(void) {
    return public_read_u32(&g_public_snapshot.ready) != 0u;
}

static int CASTLE_PAD_API_CALL public_is_connected(void) {
    return public_read_u32(&g_public_snapshot.connected) != 0u;
}

static int CASTLE_PAD_API_CALL public_game_foreground(void) {
    return public_read_u32(&g_public_snapshot.foreground) != 0u;
}

static int CASTLE_PAD_API_CALL public_button_down(CastlePadU32 button) {
    CastlePadU32 bits;
    if (button >= (CastlePadU32)CASTLE_PAD_BUTTON_COUNT) return 0;
    bits = public_read_u32(&g_public_snapshot.button_down);
    return (bits & (1u << button)) != 0u;
}

static int CASTLE_PAD_API_CALL public_button_pressed(CastlePadU32 button) {
    CastlePadU32 bits;
    if (button >= (CastlePadU32)CASTLE_PAD_BUTTON_COUNT) return 0;
    bits = public_read_u32(&g_public_snapshot.button_pressed);
    return (bits & (1u << button)) != 0u;
}

static int CASTLE_PAD_API_CALL public_button_released(CastlePadU32 button) {
    CastlePadU32 bits;
    if (button >= (CastlePadU32)CASTLE_PAD_BUTTON_COUNT) return 0;
    bits = public_read_u32(&g_public_snapshot.button_released);
    return (bits & (1u << button)) != 0u;
}

static CastlePadS32 CASTLE_PAD_API_CALL public_get_axis(CastlePadU32 axis) {
    if (axis >= (CastlePadU32)CASTLE_PAD_AXIS_COUNT) return 0;
    return public_read_s32(&g_public_snapshot.axes[axis]);
}

static int CASTLE_PAD_API_CALL public_action_down(CastlePadU32 action) {
    CastlePadU32 bits;
    if (action >= (CastlePadU32)CASTLE_PAD_ACTION_COUNT) return 0;
    bits = public_read_u32(&g_public_snapshot.action_down);
    return (bits & (1u << action)) != 0u;
}

static int CASTLE_PAD_API_CALL public_action_pressed(CastlePadU32 action) {
    CastlePadU32 bits;
    if (action >= (CastlePadU32)CASTLE_PAD_ACTION_COUNT) return 0;
    bits = public_read_u32(&g_public_snapshot.action_pressed);
    return (bits & (1u << action)) != 0u;
}

static int CASTLE_PAD_API_CALL public_action_released(CastlePadU32 action) {
    CastlePadU32 bits;
    if (action >= (CastlePadU32)CASTLE_PAD_ACTION_COUNT) return 0;
    bits = public_read_u32(&g_public_snapshot.action_released);
    return (bits & (1u << action)) != 0u;
}

static CastlePadU32 CASTLE_PAD_API_CALL public_get_control_mode(void) {
    return public_read_u32(&g_public_snapshot.control_mode);
}

static int CASTLE_PAD_API_CALL public_allows_external_ui_input(void) {
    return public_read_u32(&g_public_snapshot.allows_external_ui_input) != 0u;
}

/*
 * 这张函数表本身是 const，地址在 ASI 整个生命周期内固定。
 * 第三方只保存指针并调用，不拥有这块内存，也不允许修改表中的函数地址。
 */
static const CastlePadApiV1 g_public_api_v1 = {
    CASTLE_PAD_API_MAGIC,
    (CastlePadU32)sizeof(CastlePadApiV1),
    CASTLE_PAD_API_VERSION_1,
    CASTLE_PAD_CAP_PHYSICAL_INPUT | CASTLE_PAD_CAP_SEMANTIC_INPUT | CASTLE_PAD_CAP_CONTROL_GATE,

    public_is_ready,
    public_is_connected,
    public_game_foreground,

    public_button_down,
    public_button_pressed,
    public_button_released,
    public_get_axis,

    public_action_down,
    public_action_pressed,
    public_action_released,

    public_get_control_mode,
    public_allows_external_ui_input
};

/*
 * 唯一公共导出。
 *
 * v1 调用者传 1；未来如果增加 v2，也必须永久保留 requested_version==1 的旧分支。
 * 未支持的版本直接返回 NULL，调用者即可安全降级，而不是猜结构体长度。
 *
 * PE 导出名由 Castle_PadSupport.def 固定成未修饰的 CastlePad_GetApi。
 */
const CastlePadApiV1* CASTLE_PAD_API_CALL CastlePad_GetApi(CastlePadU32 requested_version) {
    if (requested_version != CASTLE_PAD_API_VERSION_1) return NULL;
    return &g_public_api_v1;
}
