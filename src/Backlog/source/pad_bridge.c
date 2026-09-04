#include "pad_bridge.h"
#include "runtime.h"
#include "CastleInput_API.h"

/*
 * pad_bridge.c
 *
 * 官方插件现在只向 Castle_Runtime 查询 Input v1。这里不再知道 PadSupport 的文件名、导出名
 * 或 SDL3；Controller 缺失/尚未就绪时 GetSnapshot 返回不可用，Backlog 自动保留键鼠操作。
 */

static const CastleInputApiV1* g_input_api;
static CastleInputSnapshotV1 g_snapshot;
static int g_snapshot_valid;

static void pad_zero_snapshot_(void) {
    u8* bytes = (u8*)&g_snapshot;
    u32 index;
    for (index = 0u; index < (u32)sizeof(g_snapshot); ++index) bytes[index] = 0u;
}

static const CastleInputApiV1* pad_query_input_(
    const CastleRuntimeApiV1* runtime_api) {
    static const char interface_id[] = CASTLE_INPUT_INTERFACE_ID;
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 result = {0};
    if (!runtime_api || !runtime_api->QueryInterface) return NULL;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id.data = interface_id;
    query.interface_id.length = (CastleU32)(sizeof(interface_id) - 1u);
    query.requested_version = CASTLE_INPUT_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_INPUT_API_V1;
    query.required_capabilities_low = CASTLE_INPUT_CAP_PHYSICAL_SNAPSHOT |
                                      CASTLE_INPUT_CAP_SEMANTIC_SNAPSHOT;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (runtime_api->QueryInterface(&query, &result) != CASTLE_OK) return NULL;
    return (const CastleInputApiV1*)result.api_pointer;
}

void PadBridge_Initialize(const CastleRuntimeApiV1* runtime_api) {
    g_input_api = pad_query_input_(runtime_api);
    g_snapshot_valid = 0;
    pad_zero_snapshot_();
    g_snapshot.magic = CASTLE_INPUT_SNAPSHOT_MAGIC;
    g_snapshot.struct_size = CASTLE_SIZEOF_INPUT_SNAPSHOT_V1;
    g_snapshot.version = CASTLE_INPUT_STRUCTURE_VERSION_1;
    Runtime_Log(g_input_api ?
        "[输入] 已接入 Runtime Input v1；不再按文件名查找 PadSupport。" :
        "[输入] Runtime Input v1 不可用；Backlog 本轮只保留键盘/鼠标。");
}

void PadBridge_Poll(void) {
    if (!g_input_api) {
        g_snapshot_valid = 0;
        return;
    }
    g_snapshot_valid = g_input_api->GetSnapshot(&g_snapshot) == CASTLE_OK;
}

int PadBridge_Available(void) {
    return g_snapshot_valid && g_snapshot.ready && g_snapshot.connected;
}

static int pad_bit_down_(CastleU32 bit) {
    return PadBridge_Available() && (g_snapshot.button_down & (1u << bit)) != 0u;
}

int PadBridge_Down(PadBridgeButton button) {
    switch (button) {
    case PAD_BRIDGE_LB:
        return pad_bit_down_(CASTLE_INPUT_BUTTON_LB);
    case PAD_BRIDGE_DPAD_UP:
        return pad_bit_down_(CASTLE_INPUT_BUTTON_DPAD_UP);
    case PAD_BRIDGE_DPAD_DOWN:
        return pad_bit_down_(CASTLE_INPUT_BUTTON_DPAD_DOWN);
    case PAD_BRIDGE_DPAD_LEFT:
        return pad_bit_down_(CASTLE_INPUT_BUTTON_DPAD_LEFT);
    case PAD_BRIDGE_DPAD_RIGHT:
        return pad_bit_down_(CASTLE_INPUT_BUTTON_DPAD_RIGHT);
    case PAD_BRIDGE_CANCEL:
        return PadBridge_Available() &&
            (g_snapshot.action_down & (1u << CASTLE_INPUT_ACTION_CANCEL)) != 0u;
    default:
        return 0;
    }
}

int PadBridge_BlocksBacklogInput(void) {
    if (!PadBridge_Available()) return 1;
    return g_snapshot.allows_external_ui_input ? 0 : 1;
}

void PadBridge_Shutdown(void) {
    g_input_api = NULL;
    g_snapshot_valid = 0;
    pad_zero_snapshot_();
}
