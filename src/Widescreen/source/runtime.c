#include "runtime.h"
#include "game_addresses.h"
#include "CastleHook_API.h"
#include "CastleLog_API.h"
#include "CastleToml_API.h"

/*
 * runtime.c
 *
 * 这里是“补丁基础设施”，它不知道 854×480 是怎么实现的。
 * 它只做三件事：
 * 1. 从 RPG.exe 已解析好的 IAT 取得 Kernel32 API；
 * 2. 建立 UTF-8 中文日志；
 * 3. 对每一个要修改的 CALL 做 fail-closed 校验，然后用 VirtualProtect 安全写入 5 字节。
 *
 * 把这些危险操作集中在一个文件里，可以保证 widescreen.c 只讨论显示协议，不需要到处重复写内存代码。
 */

static PFN_GetModuleHandleA      g_GetModuleHandleA;
static PFN_GetModuleFileNameA    g_GetModuleFileNameA;
static PFN_GetProcAddress        g_GetProcAddress;
static PFN_CreateFileA           g_CreateFileA;
static PFN_WriteFile             g_WriteFile;
static PFN_CloseHandle           g_CloseHandle;
static PFN_VirtualProtect        g_VirtualProtect;
static PFN_VirtualAlloc          g_VirtualAlloc;
static PFN_GetCurrentProcess     g_GetCurrentProcess;
static PFN_FlushInstructionCache g_FlushInstructionCache;
static PFN_GetTickCount            g_GetTickCount;
static PFN_GetPrivateProfileIntA   g_GetPrivateProfileIntA;
static HANDLE g_log = INVALID_HANDLE_VALUE_;
static HMODULE g_self_module;
static const CastleRuntimeApiV1* g_sdk_runtime_api;
static const CastleHookApiV1* g_sdk_hook_api;
static CastlePluginHandle g_sdk_plugin_handle;
static CastleModule g_sdk_game_module;
static CastleTransactionHandle g_sdk_transaction;
static CastleClaimHandle g_sdk_pointer_claim;
static void** g_sdk_pointer_output;
static int g_sdk_transaction_building;
static const CastleLogApiV1* g_runtime_log_api;
static CastlePluginHandle g_runtime_log_plugin;
static const CastleTomlApiV1* g_runtime_toml_api;
static CastleTomlDocumentHandle g_runtime_toml_document;

static SIZE_T text_len(const char* s) {
    SIZE_T n = 0;
    if (!s) return 0;
    while (s[n] != '\0') ++n;
    return n;
}

void Runtime_MemCopy(void* dst, const void* src, SIZE_T size) {
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    SIZE_T i;
    for (i = 0; i < size; ++i) d[i] = s[i];
}

void Runtime_MemZero(void* dst, SIZE_T size) {
    u8* d = (u8*)dst;
    SIZE_T i;
    for (i = 0; i < size; ++i) d[i] = 0;
}

static int bytes_equal(const u8* a, const u8* b, SIZE_T n) {
    SIZE_T i;
    for (i = 0; i < n; ++i) if (a[i] != b[i]) return 0;
    return 1;
}

static char hex_digit(u8 v) {
    v &= 0x0Fu;
    return (char)(v < 10u ? ('0' + v) : ('A' + (v - 10u)));
}

static void append_text(char* out, SIZE_T cap, SIZE_T* pos, const char* text) {
    SIZE_T i = 0;
    if (!out || !pos || !text || cap == 0) return;
    while (text[i] && (*pos + 1u) < cap) out[(*pos)++] = text[i++];
    out[*pos] = '\0';
}

static void append_hex32(char* out, SIZE_T cap, SIZE_T* pos, u32 value) {
    int shift;
    append_text(out, cap, pos, "0x");
    for (shift = 28; shift >= 0; shift -= 4) {
        if ((*pos + 1u) >= cap) break;
        out[(*pos)++] = hex_digit((u8)(value >> shift));
        out[*pos] = '\0';
    }
}

/*
 * 把无符号整数转换成十进制文本。
 * 这里自己做最小转换，不调用 sprintf；这样最终 ASI 仍然可以 /nodefaultlib，不依赖 CRT。
 */
static void append_u32_decimal(char* out, SIZE_T cap, SIZE_T* pos, u32 value) {
    char reversed[10];
    SIZE_T count = 0;
    SIZE_T i;

    if (!out || !pos || cap == 0) return;

    if (value == 0u) {
        if ((*pos + 1u) < cap) {
            out[(*pos)++] = '0';
            out[*pos] = '\0';
        }
        return;
    }

    /* 先从个位开始拆，所以暂存在 reversed 中，最后再倒着写出去。 */
    while (value != 0u && count < sizeof(reversed)) {
        reversed[count++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    for (i = count; i > 0u; --i) {
        if ((*pos + 1u) >= cap) break;
        out[(*pos)++] = reversed[i - 1u];
        out[*pos] = '\0';
    }
}

/*
 * 生成“和 ASI 位于同一目录”的兄弟文件路径。
 *
 * 例如 ASI 在：
 *     C:\\Game\\asi\\Castle_Widescreen.asi
 *
 * 传入 "Castle_Widescreen.ini" 后得到：
 *     C:\\Game\\asi\\Castle_Widescreen.ini
 *
 * 日志和 INI 都复用这一条路径规则，避免一个跟着工作目录走、另一个跟着插件目录走。
 */
static int build_sibling_path(const char* file_name, char* out, SIZE_T cap) {
    char module_path[MAX_PATH_];
    DWORD n;
    i32 last_slash = -1;
    DWORD i;
    SIZE_T p = 0;

    if (!g_GetModuleFileNameA || !g_self_module || !file_name || !out || cap == 0) return 0;
    n = g_GetModuleFileNameA(g_self_module, module_path, MAX_PATH_);
    if (n == 0 || n >= MAX_PATH_) return 0;

    /* 找到 ASI 自己路径中的最后一个目录分隔符，只保留所在目录。 */
    for (i = 0; i < n; ++i) {
        if (module_path[i] == '\\' || module_path[i] == '/') last_slash = (i32)i;
    }
    if (last_slash < 0) return 0;

    for (i = 0; i <= (DWORD)last_slash && (p + 1u) < cap; ++i) out[p++] = module_path[i];
    out[p] = '\0';
    append_text(out, cap, &p, file_name);
    return out[0] != '\0';
}

static int build_log_path(char* out, SIZE_T cap) {
    return build_sibling_path("Castle_Widescreen.log", out, cap);
}

void Runtime_Log(const char* text) {
    static const char crlf[2] = {'\r','\n'};
    DWORD written = 0;
    if (!text) return;
    if (g_runtime_log_api && g_runtime_log_plugin) {
        CastleLogRecordV1 record = {0};
        record.magic = CASTLE_LOG_RECORD_MAGIC;
        record.struct_size = CASTLE_SIZEOF_LOG_RECORD_V1;
        record.version = CASTLE_LOG_STRUCTURE_VERSION_1;
        record.level = CASTLE_LOG_INFO;
        record.message.data = text;
        record.message.length = (CastleU32)text_len(text);
        if (record.message.length != 0u) {
            (void)g_runtime_log_api->WritePluginLine(g_runtime_log_plugin, &record);
        }
        return;
    }
    if (!g_WriteFile || g_log == INVALID_HANDLE_VALUE_) return;
    g_WriteFile(g_log, text, (DWORD)text_len(text), &written, NULL);
    g_WriteFile(g_log, crlf, 2u, &written, NULL);
}

void Runtime_LogHex(const char* prefix, u32 value) {
    char line[256];
    SIZE_T p = 0;
    line[0] = '\0';
    append_text(line, sizeof(line), &p, prefix ? prefix : "");
    append_hex32(line, sizeof(line), &p, value);
    Runtime_Log(line);
}

void Runtime_LogU32(const char* prefix, u32 value) {
    char line[256];
    SIZE_T p = 0;
    line[0] = '\0';
    append_text(line, sizeof(line), &p, prefix ? prefix : "");
    append_u32_decimal(line, sizeof(line), &p, value);
    Runtime_Log(line);
}

u32 Runtime_GetTickCountMs(void) {
    /*
     * GetTickCount 在所有目标 Windows 版本上都存在。
     * 如果极端情况下 API 解析失败，返回 0；业务层会把 0 当成“本帧时间未推进”，
     * 不会因为空函数指针直接崩溃。
     */
    if (!g_GetTickCount) return 0u;
    return (u32)g_GetTickCount();
}

u32 Runtime_ReadPluginIniU32(
    const char* section, const char* key,
    u32 default_value, u32 min_value, u32 max_value)
{
    CastleS32 value;
    CastleStringView table_view;
    CastleStringView key_view;
    if (!section || !key || min_value > max_value) return default_value;
    if (!g_runtime_toml_api || !g_runtime_toml_document) return default_value;
    table_view.data = section;
    table_view.length = (CastleU32)text_len(section);
    key_view.data = key;
    key_view.length = (CastleU32)text_len(key);
    if (g_runtime_toml_api->GetS32(g_runtime_toml_document,
            table_view, key_view,
            (CastleS32)default_value, (CastleS32)min_value,
            (CastleS32)max_value, &value) < 0) return default_value;
    return (u32)value;
}

static int call_target_is(u32 call_address, u32 expected_target) {
    const u8* code = (const u8*)call_address;
    i32 rel;
    if (code[0] != 0xE8u) return 0;
    rel = *(const i32*)(code + 1u);
    return (call_address + 5u + (u32)rel) == expected_target;
}

static int check_call(const char* label, u32 address, u32 target) {
    if (call_target_is(address, target)) return 1;
    {
        char line[320];
        SIZE_T p = 0;
        line[0] = '\0';
        append_text(line, sizeof(line), &p, "[预检] CALL 失配：");
        append_text(line, sizeof(line), &p, label ? label : "?");
        append_text(line, sizeof(line), &p, " 地址=");
        append_hex32(line, sizeof(line), &p, address);
        append_text(line, sizeof(line), &p, " 期望目标=");
        append_hex32(line, sizeof(line), &p, target);
        Runtime_Log(line);
    }
    return 0;
}

static int check_bytes(const char* label, u32 address, const u8* expected, SIZE_T size) {
    if (bytes_equal((const u8*)address, expected, size)) return 1;
    {
        char line[320];
        SIZE_T p = 0;
        line[0] = '\0';
        append_text(line, sizeof(line), &p, "[预检] 机器码失配：");
        append_text(line, sizeof(line), &p, label ? label : "?");
        append_text(line, sizeof(line), &p, " 地址=");
        append_hex32(line, sizeof(line), &p, address);
        Runtime_Log(line);
    }
    return 0;
}

int Runtime_ExactBuildProtocolOk(void) {
    int ok = 1;

    /*
     * 下面不是“完整 EXE 哈希器”，而是运行时的精确协议闸门。
     * 每个字节片段都选在本 v0.11-poc11 真正依赖的函数入口；再配合所有 Hook CALL 的原目标检查，
     * 可以避免把本 DLL 装到另一个版本的 RPG.exe 上以后继续盲写绝对地址。
     */
    static const u8 mz[] = {0x4D,0x5A};
    static const u8 sig_alloc[] = {0x56,0x8B,0xF1,0x57,0x8B,0x46,0x28};
    static const u8 sig_present[] = {0x56,0x8B,0xF1,0x6A,0x01,0x8B,0x46,0x70};
    static const u8 sig_render_queue[] = {0x53,0x56,0x6A,0x00,0x8B,0xD9,0x6A,0x01};
    static const u8 sig_camera_bounds[] = {0x8B,0x44,0x24,0x04,0x8B,0x4C,0x24,0x08};
    static const u8 sig_camera_update[] = {0xA1,0x50,0x85,0x97,0x00,0x53,0x56,0x57};
    static const u8 sig_event_loop[] = {0xA1,0x08,0xF8,0x89,0x00};
    static const u8 sig_message_event_write[] = {
        0x81,0xE1,0xFF,0x00,0x00,0x00,  /* and ecx,0xFF：只保留 Event ID 低8位 */
        0x89,0x0D,0x70,0xF6,0x46,0x00   /* mov [0x0046F670],ecx */
    };
    static const u8 sig_message_event_clear[] = {
        0x89,0x35,0x40,0xF6,0x46,0x00,  /* 邻接状态先清零 */
        0x89,0x35,0x70,0xF6,0x46,0x00   /* mov [0x0046F670],esi；此路径 esi=0 */
    };

    /*
     * v0.11 当前真正依赖的附加锚点：
     *
     * 1. 消息侧画隔离：
     *    0x403E30 先读 target=0x46F678，再读 current=0x46F679；两者都为 0 时立即返回。
     *    0x404800 入口直接读取 current=0x46F679；为 0 时跳到函数尾。
     *    左右 Camera 重放时临时把两个字节清零，正是依赖这两条精确机器分支。
     *
     * 2. Scene identity：
     *    0x40B072 从 world manager +0x280 取 Map/Scene runtime，用来判断剧情锁存是否换场景。
     *
     * 3. Battle draw：
     *    BattleManager 的 vtable[1] 是 0x4429F0，用来稳定识别战斗帧。
     */
    static const u8 sig_message_update_draw[] = {
        0xA0,0x78,0xF6,0x46,0x00,0x84,0xC0,0x75,0x09,0xA0,0x79,0xF6,0x46,0x00
    };
    static const u8 sig_message_render[] = {
        0x83,0xEC,0x08,0xA0,0x79,0xF6,0x46,0x00,0x56,0x84,0xC0
    };
    static const u8 sig_scene_runtime_read[] = {
        0x8B,0x8E,0x80,0x02,0x00,0x00
    };
    static const u8 sig_battle_draw[] = {
        0xA0,0x0C,0x24,0x8E,0x00,0x53,0x33,0xDB,0x55,0x3A,0xC3,0x8B,0xE9
    };

    /* PE 头最基本检查：如果 0x400000 连 MZ 都不是，下面所有绝对地址都没有意义。 */
    if (!check_bytes("PE MZ", 0x00400000u, mz, sizeof(mz))) ok = 0;

    /* 原版 backing 分配器虽然本版不 Hook，但它必须仍然是我们确认的 768×576 几何实现。 */
    if (!check_bytes("原版 backing 分配函数", FN_DISPLAY_ALLOCATE, sig_alloc, sizeof(sig_alloc))) ok = 0;

    /* Present 是最后把 staging 送进 DirectDraw 的关键函数。 */
    if (!check_bytes("显示 Present 函数", FN_DISPLAY_PRESENT, sig_present, sizeof(sig_present))) ok = 0;

    /* 0x434710 是本版会重放 2~3 次的排序绘制队列，入口必须精确一致。 */
    if (!check_bytes("主绘制队列", FN_RENDER_QUEUE, sig_render_queue, sizeof(sig_render_queue))) ok = 0;

    /* Camera 边界设置与跟随/clamp 入口用于证明 0x978514..0x978530 的字段语义没有换版。 */
    if (!check_bytes("Camera 边界设置", FN_CAMERA_SET_BOUNDS, sig_camera_bounds, sizeof(sig_camera_bounds))) ok = 0;
    if (!check_bytes("Camera 跟随与 Clamp", FN_CAMERA_UPDATE_CLAMP, sig_camera_update, sizeof(sig_camera_update))) ok = 0;

    /*
     * v0.9 不再区分消息来源，但仍读取 active Event ID：
     * 任意对话框出现后，若同一 Event 还在继续，即使两句对白之间消息 UI 短暂清零，也继续保持电影式侧区。
     * 因此 active Event 语义仍属于当前版本的必要协议。
     */
    if (!check_bytes("Event VM active Event 读取", 0x0040B2D0u, sig_event_loop, sizeof(sig_event_loop))) ok = 0;

    /*
     * 消息生命周期与侧画隔离协议都必须闭合：
     * - 0x46F670 告诉我们当前到底有没有消息 UI，以及它来源于哪个 Event 低 8 位；
     * - 0x403E30 / 0x404800 必须仍以 0x46F678/679 为入口门控，侧画临时清零才是安全的。
     */
    if (!check_bytes("消息 Event 槽写入", ADDR_MESSAGE_EVENT_WRITE,
                     sig_message_event_write, sizeof(sig_message_event_write))) ok = 0;
    if (!check_bytes("消息 Event 槽清零", ADDR_MESSAGE_EVENT_CLEAR,
                     sig_message_event_clear, sizeof(sig_message_event_clear))) ok = 0;
    if (!check_bytes("消息更新/绘制入口", FN_MESSAGE_UPDATE_DRAW,
                     sig_message_update_draw, sizeof(sig_message_update_draw))) ok = 0;
    if (!check_bytes("消息主体绘制入口", FN_MESSAGE_RENDER,
                     sig_message_render, sizeof(sig_message_render))) ok = 0;

    /*
     * 剧情锁存要用 world manager +0x280 判断场景切换；Battle 模糊要用 0x4429F0 识别 BattleManager。
     * 两个机器锚点任意失配，都不能继续用绝对地址解释运行时队列对象。
     */
    if (!check_bytes("Scene world runtime +0x280", ADDR_SCENE_WORLD_RUNTIME_READ,
                     sig_scene_runtime_read, sizeof(sig_scene_runtime_read))) ok = 0;
    if (!check_bytes("BattleManager draw", FN_BATTLE_MANAGER_DRAW,
                     sig_battle_draw, sizeof(sig_battle_draw))) ok = 0;

#define CHECK_CALL(label,address,target) do { if (!check_call(label,address,target)) ok = 0; } while (0)
    /*
     * v0.9 取消“玩家主动 / 非玩家”来源分类，因此不再 Hook 0x409982 主动交互 Event。
     * 当前只修改 4 个 E8 CALL；任何一个原目标失配都整版拒绝安装。
     */
    CHECK_CALL("初次 DirectDraw 重建", CALL_DISPLAY_REBUILD_INIT, FN_DISPLAY_REBUILD);
    CHECK_CALL("Surface lost DirectDraw 重建", CALL_DISPLAY_REBUILD_LOST, FN_DISPLAY_REBUILD);
    /*
     * 主帧 RenderQueue/Present 的 CALL 已由 Castle_Runtime 在任何官方插件初始化前接管。
     * Widescreen 现在只注册 Render Provider，不能再把“CALL 仍指向原函数”当成插件预检条件。
     */
#undef CHECK_CALL

    if (ok) {
        Runtime_Log("[预检] v0.11-poc11 所需 Display/Camera/绘制队列/Event/消息生命周期/侧画消息门控/Scene身份/BattleManager 协议全部一致，可以安装 Hook。");
    } else {
        Runtime_Log("[预检] 目标 RPG.exe 协议不一致：本次启动不会安装 v0.11-poc11 宽屏 Hook。");
    }
    return ok;
}

static CastleStringView sdk_view_(const char* text, CastleU32 length) {
    CastleStringView view;
    view.data = text;
    view.length = length;
    return view;
}

int Runtime_BeginSdkHookTransaction(const CastleRuntimeApiV1* runtime_api,
                                    CastlePluginHandle plugin_handle) {
    static const char interface_id[] = CASTLE_HOOK_INTERFACE_ID;
    static const char transaction_label[] = "Widescreen hook transaction";
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 result = {0};
    CastleRuntimeInfoV1 info = {0};
    g_sdk_runtime_api = runtime_api;
    g_sdk_plugin_handle = plugin_handle;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = sdk_view_(interface_id,
        (CastleU32)(sizeof(interface_id) - 1u));
    query.requested_version = CASTLE_HOOK_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_HOOK_API_V1;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    info.magic = CASTLE_RUNTIME_INFO_MAGIC;
    info.struct_size = CASTLE_SIZEOF_RUNTIME_INFO_V1;
    info.info_version = CASTLE_RUNTIME_INFO_VERSION_1;
    if (!runtime_api || runtime_api->QueryInterface(&query, &result) != CASTLE_OK ||
        runtime_api->GetRuntimeInfo(&info) != CASTLE_OK) return 0;
    g_sdk_hook_api = (const CastleHookApiV1*)result.api_pointer;
    g_sdk_game_module = info.game_module;
    g_sdk_pointer_claim = 0u;
    g_sdk_pointer_output = NULL;
    if (!g_sdk_hook_api || g_sdk_hook_api->BeginTransaction(plugin_handle,
            sdk_view_(transaction_label,
                (CastleU32)(sizeof(transaction_label) - 1u)),
            0u, &g_sdk_transaction) != CASTLE_OK) return 0;
    g_sdk_transaction_building = 1;
    return 1;
}

int Runtime_CommitSdkHookTransaction(void) {
    CastleResult result;
    if (!g_sdk_transaction_building || !g_sdk_hook_api) return 0;
    result = g_sdk_hook_api->PreflightTransaction(g_sdk_transaction);
    if (result >= 0) result = g_sdk_hook_api->CommitTransaction(g_sdk_transaction);
    if (result < 0) {
        g_sdk_transaction_building = 0;
        return 0;
    }
    if (g_sdk_pointer_claim && g_sdk_pointer_output) {
        CastleHookBindingV1 binding = {0};
        binding.magic = CASTLE_HOOK_BINDING_MAGIC;
        binding.struct_size = CASTLE_SIZEOF_HOOK_BINDING_V1;
        binding.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
        if (g_sdk_hook_api->GetHookBinding(g_sdk_pointer_claim, &binding) != CASTLE_OK ||
            !binding.next_slot) {
            g_sdk_transaction_building = 0;
            return 0;
        }
        *g_sdk_pointer_output = *binding.next_slot;
    }
    g_sdk_transaction_building = 0;
    Runtime_Log("[RuntimeSDK] Widescreen Hook 事务已提交。");
    return 1;
}

void Runtime_AbortSdkHookTransaction(void) {
    if (g_sdk_transaction_building && g_sdk_hook_api) {
        g_sdk_hook_api->AbortTransaction(g_sdk_transaction);
    }
    g_sdk_transaction_building = 0;
}

int Runtime_PatchCall(u32 call_address, u32 expected_target, const void* replacement, const char* label) {
    u8 patch[5];
    i32 rel;
    DWORD old_protect = 0;
    DWORD ignored = 0;
    HANDLE process;

    if (g_sdk_runtime_api) {
        static const char rebuild_signature[] =
            "org.castlereforge.signature.display-rebuild.v1";
        static const char render_signature[] =
            "org.castlereforge.signature.render-queue.v1";
        static const char present_signature[] =
            "org.castlereforge.signature.display-present.v1";
        CastleChainHookClaimV1 claim = {0};
        CastleClaimHandle claim_handle = 0u;
        const char* signature_text = expected_target == FN_DISPLAY_REBUILD ?
            rebuild_signature : (expected_target == FN_RENDER_QUEUE ?
            render_signature : present_signature);
        CastleU32 signature_length = expected_target == FN_DISPLAY_REBUILD ?
            (CastleU32)(sizeof(rebuild_signature) - 1u) :
            (expected_target == FN_RENDER_QUEUE ?
            (CastleU32)(sizeof(render_signature) - 1u) :
            (CastleU32)(sizeof(present_signature) - 1u));
        if (!g_sdk_transaction_building || !g_sdk_hook_api || !replacement) return 0;
        claim.magic = CASTLE_CHAIN_HOOK_MAGIC;
        claim.struct_size = CASTLE_SIZEOF_CHAIN_HOOK_V1;
        claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
        claim.hook_kind = CASTLE_HOOK_REL32_CALL;
        claim.target.module = g_sdk_game_module;
        claim.target.rva = call_address - (u32)g_sdk_game_module;
        claim.target.size = 5u;
        claim.expected_original_target = expected_target;
        claim.replacement_hook = (CastleAddress)(SIZE_T)replacement;
        claim.signature_id = sdk_view_(signature_text, signature_length);
        claim.phase = CASTLE_HOOK_PHASE_NORMAL;
        claim.priority = CASTLE_HOOK_PRIORITY_DEFAULT;
        claim.label = label ? sdk_view_(label, (CastleU32)text_len(label)) :
                              claim.signature_id;
        return g_sdk_hook_api->AddRelativeCallHook(g_sdk_transaction, &claim,
            &claim_handle) >= 0;
    }
    if (!g_VirtualProtect || !replacement) return 0;
    if (!call_target_is(call_address, expected_target)) {
        Runtime_Log("[Hook] 安装前 CALL 已发生变化，拒绝覆盖。 ");
        Runtime_LogHex("[Hook] 失败地址=", call_address);
        return 0;
    }

    patch[0] = 0xE8u;
    rel = (i32)((u32)replacement - (call_address + 5u));
    *(i32*)(patch + 1u) = rel;

    if (!g_VirtualProtect((void*)call_address, 5u, PAGE_EXECUTE_READWRITE_, &old_protect)) return 0;
    Runtime_MemCopy((void*)call_address, patch, 5u);
    g_VirtualProtect((void*)call_address, 5u, old_protect, &ignored);

    if (g_FlushInstructionCache && g_GetCurrentProcess) {
        process = g_GetCurrentProcess();
        if (process) g_FlushInstructionCache(process, (const void*)call_address, 5u);
    }

    if (label) {
        char line[256]; SIZE_T p = 0;
        line[0] = '\0';
        append_text(line, sizeof(line), &p, "[Hook] 已安装：");
        append_text(line, sizeof(line), &p, label);
        Runtime_Log(line);
    }
    return 1;
}


/*
 * Runtime_RestoreCall 与 Runtime_PatchCall 使用同一套 E8 rel32 计算规则，但参数全部是整数地址。
 * 这个函数只用于“安装到一半失败”的回滚：例如前四个 CALL 已经改好，最后 Bink IAT 包装失败，
 * 此时必须把前面的 CALL 全部恢复，不能让游戏运行在半安装状态。
 */
int Runtime_RestoreCall(u32 call_address, u32 expected_current_target, u32 restore_target) {
    u8 patch[5];
    i32 rel;
    DWORD old_protect = 0;
    DWORD ignored = 0;
    HANDLE process;

    if (g_sdk_runtime_api) return 1;
    if (!g_VirtualProtect) return 0;
    if (!call_target_is(call_address, expected_current_target)) return 0;

    patch[0] = 0xE8u;
    rel = (i32)(restore_target - (call_address + 5u));
    *(i32*)(patch + 1u) = rel;

    if (!g_VirtualProtect((void*)call_address, 5u, PAGE_EXECUTE_READWRITE_, &old_protect)) return 0;
    Runtime_MemCopy((void*)call_address, patch, 5u);
    g_VirtualProtect((void*)call_address, 5u, old_protect, &ignored);

    if (g_FlushInstructionCache && g_GetCurrentProcess) {
        process = g_GetCurrentProcess();
        if (process) g_FlushInstructionCache(process, (const void*)call_address, 5u);
    }
    return 1;
}

int Runtime_PatchPointer(u32 slot_address, const void* replacement, void** old_value, const char* label) {
    DWORD old_protect = 0;
    DWORD ignored = 0;
    void* current;
    HANDLE process;

    if (g_sdk_runtime_api) {
        static const char signature[] =
            "org.castlereforge.signature.bink-copy-to-buffer.v1";
        CastleChainHookClaimV1 claim = {0};
        void* original;
        if (!g_sdk_transaction_building || !g_sdk_hook_api || !replacement ||
            !old_value) return 0;
        original = *(void**)slot_address;
        if (!original) return 0;
        claim.magic = CASTLE_CHAIN_HOOK_MAGIC;
        claim.struct_size = CASTLE_SIZEOF_CHAIN_HOOK_V1;
        claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
        claim.hook_kind = CASTLE_HOOK_IAT_POINTER;
        claim.target.module = g_sdk_game_module;
        claim.target.rva = slot_address - (u32)g_sdk_game_module;
        claim.target.size = 4u;
        claim.expected_original_target = (CastleAddress)(SIZE_T)original;
        claim.replacement_hook = (CastleAddress)(SIZE_T)replacement;
        claim.signature_id = sdk_view_(signature,
            (CastleU32)(sizeof(signature) - 1u));
        claim.phase = CASTLE_HOOK_PHASE_NORMAL;
        claim.priority = CASTLE_HOOK_PRIORITY_DEFAULT;
        claim.label = label ? sdk_view_(label, (CastleU32)text_len(label)) :
                              claim.signature_id;
        if (g_sdk_hook_api->AddPointerHook(g_sdk_transaction, &claim,
                &g_sdk_pointer_claim) < 0) return 0;
        g_sdk_pointer_output = old_value;
        return 1;
    }
    if (!g_VirtualProtect || !replacement || !old_value) return 0;
    current = *(void**)slot_address;
    if (!current) {
        Runtime_Log("[Hook] 函数指针槽为空，拒绝安装。");
        Runtime_LogHex("[Hook] 失败槽地址=", slot_address);
        return 0;
    }

    /*
     * 一定先保存旧值，再修改 IAT。Bink 可能由真实 binkw32.dll 或 ASI Loader 的代理 DLL 提供，
     * 所以不能把“旧函数必须在某个固定地址”作为条件；只要槽非空，我们就包装当前实际链路。
     */
    *old_value = current;
    if (!g_VirtualProtect((void*)slot_address, 4u, PAGE_EXECUTE_READWRITE_, &old_protect)) return 0;
    *(void**)slot_address = (void*)replacement;
    g_VirtualProtect((void*)slot_address, 4u, old_protect, &ignored);

    if (g_FlushInstructionCache && g_GetCurrentProcess) {
        process = g_GetCurrentProcess();
        if (process) g_FlushInstructionCache(process, (const void*)slot_address, 4u);
    }

    if (label) {
        char line[256]; SIZE_T p = 0;
        line[0] = '\0';
        append_text(line, sizeof(line), &p, "[Hook] 已安装：");
        append_text(line, sizeof(line), &p, label);
        Runtime_Log(line);
    }
    return 1;
}

void* Runtime_Alloc(SIZE_T size) {
    if (!g_VirtualAlloc || size == 0) return NULL;
    return g_VirtualAlloc(NULL, size, MEM_COMMIT_ | MEM_RESERVE_, PAGE_READWRITE_);
}

int Runtime_BindSdkLog(const CastleRuntimeApiV1* runtime_api,
                       CastlePluginHandle plugin_handle) {
    static const char interface_id[] = CASTLE_LOG_INTERFACE_ID;
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 result = {0};
    CastleStringView config_path;
    if (!runtime_api || !runtime_api->QueryInterface || plugin_handle == 0u) return 0;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id.data = interface_id;
    query.interface_id.length = (CastleU32)(sizeof(interface_id) - 1u);
    query.requested_version = CASTLE_LOG_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_LOG_API_V1;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (runtime_api->QueryInterface(&query, &result) != CASTLE_OK ||
        !result.api_pointer) return 0;
    g_runtime_log_api = (const CastleLogApiV1*)result.api_pointer;
    g_runtime_log_plugin = plugin_handle;
    query.interface_id.data = CASTLE_TOML_INTERFACE_ID;
    query.interface_id.length = (CastleU32)(sizeof(CASTLE_TOML_INTERFACE_ID) - 1u);
    query.requested_version = CASTLE_TOML_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_TOML_API_V1;
    result.api_pointer = NULL;
    if (runtime_api->QueryInterface(&query, &result) != CASTLE_OK ||
        !result.api_pointer) return 0;
    g_runtime_toml_api = (const CastleTomlApiV1*)result.api_pointer;
    config_path.data = "Castle_Widescreen.toml";
    config_path.length = 22u;
    (void)g_runtime_toml_api->OpenPluginDocument(plugin_handle,
        config_path, &g_runtime_toml_document);
    return 1;
}

int Runtime_Initialize(HMODULE self_module) {
    HMODULE kernel32;
    char log_path[MAX_PATH_];
    DWORD written = 0;
    static const u8 utf8_bom[3] = {0xEF,0xBB,0xBF};

    g_self_module = self_module;

    /*
     * 这些 IAT 槽属于 RPG.exe 本身；Windows Loader 在开始运行 EXE 之前已经把它们填成真实 API 地址。
     * 因此 ASI 不需要链接 Kernel32.lib，就能先拿到最基本的函数。
     */
    g_GetModuleHandleA   = *(PFN_GetModuleHandleA*)IAT_GETMODULEHANDLEA;
    g_GetModuleFileNameA = *(PFN_GetModuleFileNameA*)IAT_GETMODULEFILENAMEA;
    g_GetProcAddress     = *(PFN_GetProcAddress*)IAT_GETPROCADDRESS;
    g_CreateFileA        = *(PFN_CreateFileA*)IAT_CREATEFILEA;
    g_WriteFile          = *(PFN_WriteFile*)IAT_WRITEFILE;
    g_CloseHandle        = *(PFN_CloseHandle*)IAT_CLOSEHANDLE;
    g_VirtualAlloc       = *(PFN_VirtualAlloc*)IAT_VIRTUALALLOC;
    g_GetCurrentProcess  = *(PFN_GetCurrentProcess*)IAT_GETCURRENTPROCESS;

    if (!g_GetModuleHandleA || !g_GetProcAddress || !g_CreateFileA || !g_WriteFile || !g_CloseHandle) return 0;

    /* VirtualProtect/FlushInstructionCache 没有出现在这份 RPG.exe 的 IAT，因此按名字从 Kernel32 取得。 */
    kernel32 = g_GetModuleHandleA("kernel32.dll");
    if (!kernel32) return 0;
    g_VirtualProtect = (PFN_VirtualProtect)g_GetProcAddress(kernel32, "VirtualProtect");
    g_FlushInstructionCache = (PFN_FlushInstructionCache)g_GetProcAddress(kernel32, "FlushInstructionCache");
    g_GetTickCount = (PFN_GetTickCount)g_GetProcAddress(kernel32, "GetTickCount");
    g_GetPrivateProfileIntA = (PFN_GetPrivateProfileIntA)g_GetProcAddress(kernel32, "GetPrivateProfileIntA");
    if (!g_VirtualProtect || !g_GetTickCount || !g_GetPrivateProfileIntA) return 0;

    if (!g_runtime_log_api && build_log_path(log_path, sizeof(log_path))) {
        g_log = g_CreateFileA(log_path, GENERIC_WRITE_, FILE_SHARE_READ_, NULL,
                              CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL);
        if (g_log != INVALID_HANDLE_VALUE_) {
            g_WriteFile(g_log, utf8_bom, 3u, &written, NULL);
        }
    }

    Runtime_Log("[启动] Castle_Widescreen v0.11-poc11：电影式模糊 / 纯黑侧区切换版。");
    Runtime_Log("[启动] by Luminous with ChatGPT。");
    Runtime_Log("[规格] 所有对话框/提示/选择消息统一保持中央640；左右面板按 BlurredSides 选择强模糊或纯黑，触发与动画规则完全一致。");
    Runtime_Log("[规格] Battle继续使用同一侧区样式；普通探索无消息时由 INI 选择 854×480 或 1120×480。");
    return 1;
}

void Runtime_Shutdown(void) {
    if (g_runtime_log_api) Runtime_Log("[结束] Castle_Widescreen 卸载。");
    if (g_CloseHandle && g_log != INVALID_HANDLE_VALUE_) {
        Runtime_Log("[结束] Castle_Widescreen 卸载。");
        g_CloseHandle(g_log);
        g_log = INVALID_HANDLE_VALUE_;
    }
    g_runtime_log_api = NULL;
    g_runtime_log_plugin = 0u;
    if (g_runtime_toml_api && g_runtime_toml_document) {
        g_runtime_toml_api->CloseDocument(g_runtime_toml_document);
    }
    g_runtime_toml_api = NULL;
    g_runtime_toml_document = 0u;
}
