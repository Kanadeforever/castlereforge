#include "pad_bridge.h"
#include "runtime.h"
#include "Castle_PadSupport_API.h"

/*
 * pad_bridge.c
 *
 * v0.3.1 / v0.3.2 都尝试过“从 Castle_PadSupport.asi 内部反推状态”的兼容桥：
 * - v0.3.1 扫描 PadInput_Down / ControlModes_BlocksMapMovement 的短函数机器码；
 * - v0.3.2 扫描 PadInputState / ControlModeState 的可写数据结构。
 *
 * 两条路线都已经被实机判定为失败：编译器优化、功能增减和结构布局变化都会让“唯一识别”失效。
 * 这不是应该继续堆更多签名的地方。正确架构是由 PadSupport 自己提供稳定 ABI。
 *
 * 因此本文件现在非常小：
 *   GetModuleHandleA("Castle_PadSupport.asi")
 *        ↓
 *   GetProcAddress("CastlePad_GetApi")
 *        ↓
 *   请求 API v1 并检查 magic / size / version / 必需函数
 *        ↓
 *   后续只调用公开函数表
 *
 * Backlog 仍然没有 SDL3.dll 所有权，也不会调用 PadInput_Poll 或 InputRouter_Consume。
 */

#define PAD_BRIDGE_RETRY_MS 1000u

static HMODULE g_pad_module;
static const CastlePadApiV1* g_api;
/*
 * PadSupport 可能比 Backlog 更晚被 ASI Loader 加载，所以绑定失败后需要定期重试。
 *
 * 这里保存 Win32 GetTickCount() 的毫秒时间点。
 * 这样桥接层不会把毫秒和 Backlog 自己的 8ms worker tick 混成同一个单位。
 *
 * 比较使用 `(i32)(now - deadline) < 0`。只要单次等待远小于 2^31 毫秒，
 * 即使 GetTickCount() 发生 32 位回绕，这种差值比较仍然成立。
 */
static u32 g_next_retry_ms;
static int g_logged_no_module;
static int g_logged_old_api;
static int g_logged_bad_api;
static int g_logged_bound;

/*
 * 检查函数表是不是“真的至少包含 v1 的全部字段”。
 * 不能只看版本号：如果第三方错误导出了同名函数，magic/struct_size 能再挡一道。
 */
static int pad_bridge_api_valid(const CastlePadApiV1* api) {
    if (!api) return 0;
    if (!Runtime_MemoryRangeReadable(api, (u32)sizeof(CastlePadApiV1))) return 0;
    if (api->magic != CASTLE_PAD_API_MAGIC) return 0;
    if (api->api_version != CASTLE_PAD_API_VERSION_1) return 0;
    if (api->struct_size < (CastlePadU32)sizeof(CastlePadApiV1)) return 0;

    if ((api->capability_flags & CASTLE_PAD_CAP_PHYSICAL_INPUT) == 0u) return 0;
    if ((api->capability_flags & CASTLE_PAD_CAP_SEMANTIC_INPUT) == 0u) return 0;
    if ((api->capability_flags & CASTLE_PAD_CAP_CONTROL_GATE) == 0u) return 0;

    /* Backlog 真正会调用的每一个函数都必须存在。 */
    if (!api->IsReady || !api->IsConnected || !api->GameForeground) return 0;
    if (!api->ButtonDown || !api->ActionDown) return 0;
    if (!api->AllowsExternalUiInput) return 0;
    return 1;
}

/*
 * 尝试绑定一次。
 * 这里不 LoadLibrary：PadSupport 是用户选择安装的独立 ASI，Backlog 不应该偷偷替用户加载它。
 */
static void pad_bridge_try_bind(void) {
    CastlePadGetApiFn get_api;
    const CastlePadApiV1* api;

    if (g_api) return;

    g_pad_module = GetModuleHandleA("Castle_PadSupport.asi");
    if (!g_pad_module) {
        if (!g_logged_no_module) {
            Runtime_Log("[手柄桥] 未发现 Castle_PadSupport.asi；Backlog 保持键盘/鼠标模式。");
            g_logged_no_module = 1;
        }
        return;
    }

    /* 模块已经出现，后续日志不再说“没安装”。 */
    g_logged_no_module = 0;

    get_api = (CastlePadGetApiFn)GetProcAddress(g_pad_module, "CastlePad_GetApi");
    if (!get_api) {
        if (!g_logged_old_api) {
            Runtime_Log(
                "[手柄桥] 已发现 Castle_PadSupport.asi，但没有 CastlePad_GetApi；"
                "请使用带 Public API v1 的 PadSupport。"
            );
            g_logged_old_api = 1;
        }
        return;
    }

    api = get_api(CASTLE_PAD_API_VERSION_1);
    if (!pad_bridge_api_valid(api)) {
        if (!g_logged_bad_api) {
            Runtime_Log(
                "[手柄桥] CastlePad_GetApi 已存在，但返回的 Public API v1 表不完整或版本不匹配；"
                "本轮手柄输入已安全关闭。"
            );
            g_logged_bad_api = 1;
        }
        return;
    }

    g_api = api;
    g_logged_old_api = 0;
    g_logged_bad_api = 0;
    if (!g_logged_bound) {
        Runtime_Log(
            "[手柄桥] 已通过 CastlePad_GetApi 接入 PadSupport Public API v1；"
            "不再扫描内部结构或机器码。"
        );
        g_logged_bound = 1;
    }
}

void PadBridge_Initialize(void) {
    g_pad_module = NULL;
    g_api = NULL;
    g_next_retry_ms = 0u;
    g_logged_no_module = 0;
    g_logged_old_api = 0;
    g_logged_bad_api = 0;
    g_logged_bound = 0;

    pad_bridge_try_bind();
    /*
     * GetTickCount() 本身就是毫秒，因此这里直接加 1000ms 重试间隔。
     * 不再经过 Runtime_MsToTicks()，避免把单位转换成 8ms worker tick。
     */
    g_next_retry_ms = (u32)GetTickCount() + PAD_BRIDGE_RETRY_MS;
}

void PadBridge_Poll(void) {
    u32 now;

    /* 已经绑定后函数表在 PadSupport 模块生命周期内固定，不需要每 8ms 重做 GetProcAddress。 */
    if (g_api) return;

    now = (u32)GetTickCount();
    if ((i32)(now - g_next_retry_ms) < 0) return;

    pad_bridge_try_bind();
    g_next_retry_ms = now + PAD_BRIDGE_RETRY_MS;
}

int PadBridge_Available(void) {
    if (!g_api) return 0;

    /*
     * API 存在和“现在能输入”是两回事：SDL3 可能仍在重试，手柄也可能临时拔掉。
     * 这里把 ready/connected 作为 availability；前台和 ControlMode 由 Blocks 函数另行判断。
     */
    if (!g_api->IsReady()) return 0;
    if (!g_api->IsConnected()) return 0;
    return 1;
}

int PadBridge_Down(PadBridgeButton button) {
    if (!PadBridge_Available()) return 0;

    switch (button) {
    case PAD_BRIDGE_LB:
        return g_api->ButtonDown((CastlePadU32)CASTLE_PAD_BUTTON_LB);
    case PAD_BRIDGE_DPAD_UP:
        return g_api->ButtonDown((CastlePadU32)CASTLE_PAD_BUTTON_DPAD_UP);
    case PAD_BRIDGE_DPAD_DOWN:
        return g_api->ButtonDown((CastlePadU32)CASTLE_PAD_BUTTON_DPAD_DOWN);
    case PAD_BRIDGE_DPAD_LEFT:
        return g_api->ButtonDown((CastlePadU32)CASTLE_PAD_BUTTON_DPAD_LEFT);
    case PAD_BRIDGE_DPAD_RIGHT:
        return g_api->ButtonDown((CastlePadU32)CASTLE_PAD_BUTTON_DPAD_RIGHT);
    case PAD_BRIDGE_CANCEL:
        /*
         * Cancel 不能写死 East/B：PadSupport 的 SwapConfirmCancel 会改变“取消”的物理位置。
         * 公共语义动作正是为这种跨插件一致性准备的。
         */
        return g_api->ActionDown((CastlePadU32)CASTLE_PAD_ACTION_CANCEL);
    default:
        return 0;
    }
}

int PadBridge_BlocksBacklogInput(void) {
    if (!g_api) return 1;

    /*
     * AllowsExternalUiInput 同时检查：ready、connected、游戏前台、ControlModes 非独占。
     * 因而调查、RT 鼠标、Back 常驻鼠标都会让这里返回“阻止 Backlog”。
     */
    return g_api->AllowsExternalUiInput() ? 0 : 1;
}

void PadBridge_Shutdown(void) {
    g_api = NULL;
    g_pad_module = NULL;
    g_next_retry_ms = 0u;
}
