#include "PatchUtil.h"
#include "PluginLog.h"
#include "CastleRuntime_Client.h"
#include "CastleHook_API.h"
#include "CastlePath_API.h"

// ============================================================================
// MaxGrowthAndDrop.asi
// ----------------------------------------------------------------------------
// 把旧五合一补丁里的“最大成长”和“最大掉宝”拆成两个互相独立的开关。
//
// 配置文件：MaxGrowthAndDrop.ini
// - MaxGrowth=1：最大成长（默认开启）
// - MaxGrowth=0：恢复原版随机成长
// - MaxDrop=1：最大掉宝（默认开启）
// - MaxDrop=0：恢复原版掉宝概率
//
// 如果 INI 不存在，ASI 会在自身目录自动生成一份，并默认两项都为 1。
// ============================================================================

namespace {

HMODULE gPluginModule = nullptr;

// ------------------------------- 最大掉宝 -------------------------------------
// 原版掉落判断在 0x00443A1D 附近：
//     call random
//     cdq
//     mov ecx, 100
//     idiv ecx
//     cmp edx, [esi+0x10]
//
// idiv 100 后，EDX 是 0~99 的随机余数，用它与掉落概率比较。
// 旧五合一把“计算随机余数”改成“直接令 EDX=1”，因此只要条目的概率阈值允许，
// 就稳定走向掉落成功分支。这一组改动与升级成长函数完全分离，所以可以独立开关。
const BYTE kDropOriginal0[] = {0xB9, 0x64};
const BYTE kDropPatched0[]  = {0xBA, 0x01};
const BYTE kDropOriginal1[] = {0xF7, 0xF9};
const BYTE kDropPatched1[]  = {0x90, 0x90};

const ycr::Patch kDropPatches[] = {
    {0x00043A23u, kDropOriginal0, kDropPatched0, 2},
    {0x00043A28u, kDropOriginal1, kDropPatched1, 2},
};

// ------------------------------- 最大成长 -------------------------------------
// 升级函数会多次调用同一个随机数函数 0x00451565。
// 第一、第二处原本都通过“随机数 % 3”得到 0、1、2 三种浮动；旧五合一直接把
// EDX 固定为 2，也就是永远选随机范围中的最大余数。
const BYTE kGrowthOriginal0[] = {0xB9, 0x03};
const BYTE kGrowthPatched0[]  = {0xBA, 0x02};
const BYTE kGrowthOriginal1[] = {0xF7, 0xF9};
const BYTE kGrowthPatched1[]  = {0x90, 0x90};
const BYTE kGrowthOriginal2[] = {0xB9, 0x03};
const BYTE kGrowthPatched2[]  = {0xBA, 0x02};
const BYTE kGrowthOriginal3[] = {0xF7, 0xF9};
const BYTE kGrowthPatched3[]  = {0x90, 0x90};

// 0x00443D40 是一个“按角色/成长项目查表，然后在最小值~最大值之间随机取值”的函数。
// 原版尾部：
//     idiv esi          ; EDX = 随机余数
//     mov eax, edx
//     add eax, edi      ; 最小值 + 随机余数
// 旧五合一改成：
//     lea eax, [esi+edi-1]
// 也就是直接返回“最小值 + 范围宽度 - 1”，即该表项允许的最大成长值。
const BYTE kGrowthOriginal4[] = {0xF7, 0xFE, 0x8B, 0xC2, 0x03, 0xC7};
const BYTE kGrowthPatched4[]  = {0x8D, 0x44, 0x3E, 0xFF, 0x90, 0x90};

const ycr::Patch kGrowthPatches[] = {
    {0x00043BBAu, kGrowthOriginal0, kGrowthPatched0, 2},
    {0x00043BBFu, kGrowthOriginal1, kGrowthPatched1, 2},
    {0x00043BFCu, kGrowthOriginal2, kGrowthPatched2, 2},
    {0x00043C03u, kGrowthOriginal3, kGrowthPatched3, 2},
    {0x00043D7Du, kGrowthOriginal4, kGrowthPatched4, 6},
};

// ------------------------------- INI 处理 -------------------------------------
const wchar_t kIniFileName[] = L"MaxGrowthAndDrop.ini";
const wchar_t kIniSection[]  = L"MaxGrowthAndDrop";
const wchar_t kGrowthKey[]   = L"MaxGrowth";
const wchar_t kDropKey[]     = L"MaxDrop";

// 默认 INI 使用 UTF-16LE，并在第一个字符写入 BOM(U+FEFF)。
// 这样 Windows 的 GetPrivateProfileIntW 可以稳定读取中文注释和 ASCII 配置键。
const wchar_t kDefaultIniText[] =
    L"\uFEFF; 幽城幻剑录 - 最大成长 / 最大掉宝设置\r\n"
    L"; 数值说明：1 = 开启，0 = 关闭。\r\n"
    L"; 两项默认都开启；修改后重新启动游戏即可生效。\r\n"
    L"\r\n"
    L"[MaxGrowthAndDrop]\r\n"
    L"; 最大成长：升级时把随机成长结果固定到旧五合一补丁使用的最大值。\r\n"
    L"MaxGrowth=1\r\n"
    L"\r\n"
    L"; 最大掉宝：把旧五合一的掉落随机判断固定到高掉落结果。\r\n"
    L"MaxDrop=1\r\n";

// 把 ASI 自己的完整路径改造成同目录下的 MaxGrowthAndDrop.ini。
// 例如：
//   D:\Game\MaxGrowthAndDrop.asi
// 变成：
//   D:\Game\MaxGrowthAndDrop.ini
bool BuildIniPath(HMODULE module, wchar_t* outPath, DWORD capacity) {
    const DWORD length = GetModuleFileNameW(module, outPath, capacity);
    if (length == 0u || length >= capacity) {
        return false;
    }

    // 从末尾往前找最后一个 '\\' 或 '/'，它后面就是原 ASI 文件名。
    DWORD fileNameStart = length;
    while (fileNameStart > 0u) {
        const wchar_t ch = outPath[fileNameStart - 1u];
        if (ch == L'\\' || ch == L'/') {
            break;
        }
        --fileNameStart;
    }

    const SIZE_T iniNameLength = ycr::WideLength(kIniFileName);
    if (fileNameStart + iniNameLength + 1u > capacity) {
        return false;
    }

    // 把旧文件名覆盖为固定的新文件名。
    for (SIZE_T i = 0; i < iniNameLength; ++i) {
        outPath[fileNameStart + i] = kIniFileName[i];
    }
    outPath[fileNameStart + iniNameLength] = L'\0';
    return true;
}

// INI 不存在时创建默认文件。
// CREATE_NEW 的好处是：如果用户文件恰好在我们检查后、创建前出现，也不会覆盖它。
void CreateDefaultIniIfMissing(const wchar_t* iniPath) {
    if (GetFileAttributesW(iniPath) != INVALID_FILE_ATTRIBUTES) {
        return;
    }

    HANDLE file = CreateFileW(
        iniPath,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD written = 0;
    const DWORD bytesToWrite = static_cast<DWORD>(
        ycr::WideLength(kDefaultIniText) * sizeof(wchar_t));

    WriteFile(file, kDefaultIniText, bytesToWrite, &written, nullptr);
    CloseHandle(file);
}

// 任何非 0 值都按“开启”处理，0 才是关闭。
// 如果 INI 无法读取，defaultValue=1 保证行为仍然符合“默认全部开启”的要求。
bool ReadSwitch(const wchar_t* iniPath, const wchar_t* key) {
    return GetPrivateProfileIntW(kIniSection, key, 1, iniPath) != 0u;
}

CastleStringView View(const char* text, CastleU32 length) {
    CastleStringView value{};
    value.data = text;
    value.length = length;
    return value;
}

const void* QueryInterface(const CastleRuntimeApiV1* runtimeApi,
                           const char* interfaceId, CastleU32 interfaceLength,
                           CastleU32 version, CastleU32 minimumSize) {
    CastleInterfaceQueryV1 query{};
    CastleInterfaceResultV1 result{};
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = View(interfaceId, interfaceLength);
    query.requested_version = version;
    query.minimum_struct_size = minimumSize;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    return runtimeApi && runtimeApi->QueryInterface(&query, &result) == CASTLE_OK
        ? result.api_pointer : nullptr;
}

void OpenStartupLog(const char* mode) {
    ycrlog::Open(gPluginModule, L"MaxGrowthAndDrop.log");
    ycrlog::Line("《幽城幻剑录》最大成长 / 最大掉宝插件 v0.4.0 RuntimeSDK 启动。");
    ycrlog::Line("ASI插件化 By Luminous with ChatGPT");
    ycrlog::Line("原始文件来自“汉堂之家”坛友武英仲分享的五合一补丁");
    ycrlog::Text("[启动模式] ");
    ycrlog::Line(mode);
}

void LogConfiguration(bool enableGrowth, bool enableDrop) {
    ycrlog::Text("[配置] 最大成长：");
    ycrlog::Line(enableGrowth ? "开启" : "关闭");
    ycrlog::Text("[配置] 最大掉宝：");
    ycrlog::Line(enableDrop ? "开启" : "关闭");
}

CastleResult ApplyRuntimePatchSet(const CastleHookApiV1* hookApi,
                                  CastleModule gameModule,
                                  CastlePluginHandle pluginHandle,
                                  const ycr::Patch* patches, SIZE_T patchCount,
                                  bool enable, const char* transactionLabel,
                                  CastleU32 transactionLabelLength) {
    CastleTransactionHandle transaction = 0u;
    CastleResult result = hookApi->BeginTransaction(pluginHandle,
        View(transactionLabel, transactionLabelLength), 0u, &transaction);
    if (result < 0) return result;

    for (SIZE_T index = 0u; index < patchCount; ++index) {
        CastleStatePatchClaimV1 claim{};
        CastleClaimHandle claimHandle = 0u;
        claim.magic = CASTLE_STATE_PATCH_MAGIC;
        claim.struct_size = CASTLE_SIZEOF_STATE_PATCH_V1;
        claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
        claim.flags = CASTLE_PATCH_FLAG_CODE | CASTLE_PATCH_FLAG_KEEP_ON_PROCESS_EXIT;
        claim.target.module = gameModule;
        claim.target.rva = patches[index].rva;
        claim.target.size = static_cast<CastleU32>(patches[index].size);
        claim.original_bytes = patches[index].original;
        claim.original_size = static_cast<CastleU32>(patches[index].size);
        claim.enabled_bytes = patches[index].patched;
        claim.enabled_size = static_cast<CastleU32>(patches[index].size);
        claim.desired_state = enable ? CASTLE_PATCH_STATE_ENABLED :
                                       CASTLE_PATCH_STATE_ORIGINAL;
        claim.label = View(transactionLabel, transactionLabelLength);
        result = hookApi->AddStatePatch(transaction, &claim, &claimHandle);
        if (result < 0) {
            hookApi->AbortTransaction(transaction);
            return result;
        }
    }
    result = hookApi->PreflightTransaction(transaction);
    if (result < 0) {
        hookApi->AbortTransaction(transaction);
        return result;
    }
    return hookApi->CommitTransaction(transaction);
}

CastleResult InitializeStandalone() {
    wchar_t iniPath[1024]{};
    bool enableGrowth = true;
    bool enableDrop = true;
    OpenStartupLog("Standalone：使用插件本地 Path 与双态补丁器。");
    if (BuildIniPath(gPluginModule, iniPath, 1024u)) {
        CreateDefaultIniIfMissing(iniPath);
        enableGrowth = ReadSwitch(iniPath, kGrowthKey);
        enableDrop = ReadSwitch(iniPath, kDropKey);
    } else {
        ycrlog::Line("[警告] 无法构造 INI 路径，按默认值开启两项。");
    }
    LogConfiguration(enableGrowth, enableDrop);
    const bool growthOk = ycr::SetPatchSetState(kGrowthPatches,
        sizeof(kGrowthPatches) / sizeof(kGrowthPatches[0]), enableGrowth);
    const bool dropOk = ycr::SetPatchSetState(kDropPatches,
        sizeof(kDropPatches) / sizeof(kDropPatches[0]), enableDrop);
    ycrlog::Line(growthOk ? "[补丁] 最大成长状态已应用。" :
        "[失败] 最大成长机器码未知或写入失败。");
    ycrlog::Line(dropOk ? "[补丁] 最大掉宝状态已应用。" :
        "[失败] 最大掉宝机器码未知或写入失败。");
    if (!growthOk && !dropOk) return CASTLE_ERROR_EXPECTED_BYTES;
    return growthOk && dropOk ? CASTLE_OK : CASTLE_STATUS_OPTIONAL_UNAVAILABLE;
}

CastleResult InitializeIntegrated(const CastleRuntimeApiV1* runtimeApi,
                                  CastlePluginHandle pluginHandle) {
    static const char hookId[] = CASTLE_HOOK_INTERFACE_ID;
    static const char pathId[] = CASTLE_PATH_INTERFACE_ID;
    static const char growthLabel[] = "MaxGrowth transaction";
    static const char dropLabel[] = "MaxDrop transaction";
    CastleRuntimeInfoV1 runtimeInfo{};
    wchar_t iniPath[1024]{};
    CastleU32 iniLength = 0u;
    bool enableGrowth;
    bool enableDrop;
    const auto* hookApi = static_cast<const CastleHookApiV1*>(QueryInterface(runtimeApi,
        hookId, static_cast<CastleU32>(sizeof(hookId) - 1u),
        CASTLE_HOOK_API_VERSION_1, CASTLE_SIZEOF_HOOK_API_V1));
    const auto* pathApi = static_cast<const CastlePathApiV1*>(QueryInterface(runtimeApi,
        pathId, static_cast<CastleU32>(sizeof(pathId) - 1u),
        CASTLE_PATH_API_VERSION_1, CASTLE_SIZEOF_PATH_API_V1));
    if (!ycrlog::BindRuntime(runtimeApi, pluginHandle)) {
        return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    }
    OpenStartupLog("Integrated：Runtime Path + 两个独立 Hook 事务。");
    runtimeInfo.magic = CASTLE_RUNTIME_INFO_MAGIC;
    runtimeInfo.struct_size = CASTLE_SIZEOF_RUNTIME_INFO_V1;
    runtimeInfo.info_version = CASTLE_RUNTIME_INFO_VERSION_1;
    CastleWideStringView relativeIni{};
    relativeIni.data = reinterpret_cast<const CastleU16*>(kIniFileName);
    relativeIni.length = static_cast<CastleU32>(ycr::WideLength(kIniFileName));
    if (!hookApi || !pathApi || runtimeApi->GetRuntimeInfo(&runtimeInfo) != CASTLE_OK ||
        pathApi->BuildPluginRelativePathWide(pluginHandle, relativeIni,
            reinterpret_cast<CastleU16*>(iniPath), 1024u, &iniLength) != CASTLE_OK) {
        ycrlog::Line("[失败] Runtime Hook/Path 不可用；未回退到私有游戏写入。");
        return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    }
    CreateDefaultIniIfMissing(iniPath);
    enableGrowth = ReadSwitch(iniPath, kGrowthKey);
    enableDrop = ReadSwitch(iniPath, kDropKey);
    LogConfiguration(enableGrowth, enableDrop);

    const CastleResult growthResult = ApplyRuntimePatchSet(hookApi,
        runtimeInfo.game_module, pluginHandle, kGrowthPatches,
        sizeof(kGrowthPatches) / sizeof(kGrowthPatches[0]), enableGrowth,
        growthLabel, static_cast<CastleU32>(sizeof(growthLabel) - 1u));
    const CastleResult dropResult = ApplyRuntimePatchSet(hookApi,
        runtimeInfo.game_module, pluginHandle, kDropPatches,
        sizeof(kDropPatches) / sizeof(kDropPatches[0]), enableDrop,
        dropLabel, static_cast<CastleU32>(sizeof(dropLabel) - 1u));
    ycrlog::Line(growthResult >= 0 ? "[补丁] Runtime 已提交最大成长事务。" :
        "[失败] Runtime 拒绝最大成长事务；该组未改变。");
    ycrlog::Line(dropResult >= 0 ? "[补丁] Runtime 已提交最大掉宝事务。" :
        "[失败] Runtime 拒绝最大掉宝事务；该组未改变。");
    if (growthResult < 0 && dropResult < 0) return CASTLE_ERROR_EXPECTED_BYTES;
    return growthResult >= 0 && dropResult >= 0 ? CASTLE_OK :
        CASTLE_STATUS_OPTIONAL_UNAVAILABLE;
}

void RuntimeFault(CastleResult failure) {
    (void)failure;
}

}  // namespace

static CastleResult CASTLE_RUNTIME_CALL MaxGrowth_Integrated(
    const CastleRuntimeApiV1* runtimeApi, CastlePluginHandle pluginHandle,
    void* userContext) {
    (void)userContext;
    return InitializeIntegrated(runtimeApi, pluginHandle);
}

static CastleResult CASTLE_RUNTIME_CALL MaxGrowth_Standalone(void* userContext) {
    (void)userContext;
    return InitializeStandalone();
}

static void CASTLE_RUNTIME_CALL MaxGrowth_RuntimeFault(CastleResult failure,
                                                       void* userContext) {
    (void)userContext;
    RuntimeFault(failure);
}

static void CASTLE_RUNTIME_CALL MaxGrowth_ProcessExit(void* userContext) {
    (void)userContext;
    ycrlog::Line("[退出] 最大成长 / 最大掉宝插件随进程结束。");
    ycrlog::Close();
}

static const char kPluginId[] = "org.castlereforge.extra.maxgrowthanddrop";
static const char kDisplayName[] = "Castle Max Growth And Drop";
static const char kVersionText[] = "0.4.0";
static const char kBuildId[] = "runtimesdk-v1";
static const CastlePluginDescriptorV1 gPluginDescriptor = {
    CASTLE_PLUGIN_DESC_MAGIC, CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_REQUESTS_HOOKS |
        CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE,
    0u,
    {kPluginId, static_cast<CastleU32>(sizeof(kPluginId) - 1u)},
    {kDisplayName, static_cast<CastleU32>(sizeof(kDisplayName) - 1u)},
    {kVersionText, static_cast<CastleU32>(sizeof(kVersionText) - 1u)},
    {kBuildId, static_cast<CastleU32>(sizeof(kBuildId) - 1u)}
};
static const CastleRuntimeClientConfigV1 gClientConfig = {
    CASTLE_CLIENT_CONFIG_MAGIC, CASTLE_SIZEOF_CLIENT_CONFIG_V1,
    CASTLE_CLIENT_CONFIG_VERSION_1, CASTLE_CLIENT_FLAG_REQUIRE_RUNTIME,
    MaxGrowth_Integrated, MaxGrowth_Standalone, MaxGrowth_RuntimeFault,
    MaxGrowth_ProcessExit, nullptr
};
static CastlePluginExportV1 gPluginExport = {
    CASTLE_PLUGIN_QUERY_MAGIC, CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1, 0u,
    &gPluginDescriptor, &gClientConfig, 0u, nullptr
};

extern "C" const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requestedVersion) {
    return requestedVersion == CASTLE_PLUGIN_EXPORT_VERSION_1 ? &gPluginExport : nullptr;
}

extern "C" void __cdecl InitializeASI(void) {
    CastleRuntimeClient_RunNow();
}

extern "C" BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        gPluginModule = module;
        DisableThreadLibraryCalls(module);
        CastleRuntimeClient_OnProcessAttach(
            static_cast<CastleModule>(reinterpret_cast<SIZE_T>(module)), &gPluginExport);
    } else if (reason == DLL_PROCESS_DETACH) {
        CastleRuntimeClient_OnProcessDetach(reserved);
    }
    return TRUE;
}
