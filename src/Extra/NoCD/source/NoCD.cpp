#include "PatchUtil.h"
#include "PluginLog.h"
#include "CastleRuntime_Client.h"
#include "CastleHook_API.h"

// ============================================================================
// NoCD.asi
// ----------------------------------------------------------------------------
// 目标：去掉启动时的 PlayDisk/CD-ROM 检查，但不破坏地图、影片等正常资源读取流程。
//
// 重要：这里没有原样照抄流传二十多年的旧免CD三字节改法。
// 旧补丁会：
// 1. 把“驱动器类型必须是 CD-ROM(5)”改成“驱动器类型 >= 2”；
// 2. 随后无条件接受第一个这样的驱动器。
// 因为扫描从 A: 开始，所以现代机器通常最终把备用光盘盘符写成 C:。
//
// 旧补丁长期能工作的真正原因，是游戏后面的资源代码会先访问：
//     ..\MultiMedia\Map\...
//     ..\MultiMedia\Mov\...
// 本地文件不存在时，才回退到：
//     %c:\MultiMedia\...
// 因此那个错误/粗糙的 C: 盘符大多没有被实际使用。
//
// 本插件采用更干净的等价方案：
// - 直接让“扫描光驱并提示插盘”的函数立刻 return；
// - 把备用盘符写成 RPG.exe 自己所在的驱动器，而不是随便选 C:；
// - 完全不修改后续地图/影片的本地优先读取逻辑。
// ============================================================================

namespace {

HMODULE gPluginModule = nullptr;

// 0x00402A10 是纯粹的“扫描 A:~Z:、找 CD-ROM、检查 \MultiMedia\Mov、失败时提示
// Please insert PlayDisk(4th)”函数入口。
// 原函数第一条指令是：83 EC 38  => sub esp, 0x38。
// 我们只把第一个字节 83 改成 C3(ret)，函数就会在任何扫描动作发生前直接返回。
const BYTE kScanOriginal[] = {0x83};
const BYTE kScanPatched[]  = {0xC3};
const ycr::Patch kSkipCdScan = {
    0x00002A10u,
    kScanOriginal,
    kScanPatched,
    1
};

// 因为真正写入时只需要把函数入口的第一个字节改成 RET，单看“0x83”一个字节
// 作为版本判断还不够严格。因此再保存入口后续 15 字节作为“身份指纹”。
// 旧历史免CD只改了本函数更后面的 0x00402A41/0x00402A42/0x00402A62，
// 所以这个入口指纹同时兼容原始 EXE 和旧免CD EXE。
const BYTE kCdScanTailSignature[] = {
    0xEC, 0x38, 0x53, 0x55, 0x8B, 0x2D, 0x24, 0x01,
    0x46, 0x00, 0x56, 0x57, 0x8B, 0x3D, 0x94
};

/* Runtime 双态事务一次声明完整 16 字节，既保留严格指纹，也允许幂等识别已启用状态。 */
const BYTE kCdScanOriginalFull[] = {
    0x83, 0xEC, 0x38, 0x53, 0x55, 0x8B, 0x2D, 0x24,
    0x01, 0x46, 0x00, 0x56, 0x57, 0x8B, 0x3D, 0x94
};
const BYTE kCdScanEnabledFull[] = {
    0xC3, 0xEC, 0x38, 0x53, 0x55, 0x8B, 0x2D, 0x24,
    0x01, 0x46, 0x00, 0x56, 0x57, 0x8B, 0x3D, 0x94
};

bool IsCdScanFunctionRecognized() {
    BYTE* base = ycr::GetExeBase();
    if (base == nullptr) {
        return false;
    }

    BYTE* functionStart = base + 0x00002A10u;

    // 第一个字节允许是原始 0x83，也允许已经被本插件改成 0xC3。
    if (functionStart[0] != 0x83 && functionStart[0] != 0xC3) {
        return false;
    }

    // 再核对后面 15 字节，避免仅凭一个常见的 0x83 就误判版本。
    return ycr::BytesEqual(
        functionStart + 1,
        kCdScanTailSignature,
        sizeof(kCdScanTailSignature));
}

// 原版把检测到的光盘盘符字符保存在 0x0046F5C8。
// 用 RVA 表示就是 0x0006F5C8。
const DWORD kCdDriveLetterRva = 0x0006F5C8u;

// 从 RPG.exe 的完整路径里拿到“D:\...”前面的 D。
// 如果是极少见的 UNC/特殊路径，没有标准“X:”格式，就退回到 C。
BYTE DetectExeDriveLetter() {
    wchar_t path[1024];
    path[0] = L'\0';
    const DWORD length = GetModuleFileNameW(nullptr, path, 1024u);

    if (length >= 2u && path[1] == L':') {
        wchar_t letter = path[0];

        // 把小写 a~z 转成大写 A~Z，只做最简单的 ASCII 范围转换。
        if (letter >= L'a' && letter <= L'z') {
            letter = static_cast<wchar_t>(letter - L'a' + L'A');
        }

        if (letter >= L'A' && letter <= L'Z') {
            return static_cast<BYTE>(letter);
        }
    }

    return static_cast<BYTE>('C');
}

// 把备用盘符写进 RPG.exe 的 .data 区。
// .data 本来就可写，所以这里不需要 VirtualProtect。
void StoreFallbackDriveLetter(BYTE driveLetter) {
    BYTE* base = ycr::GetExeBase();
    if (base != nullptr) {
        base[kCdDriveLetterRva] = driveLetter;
    }
}

void OpenStartupLog(const char* mode) {
    ycrlog::Open(gPluginModule, L"NoCD.log");
    ycrlog::Line("《幽城幻剑录》免 CD 插件 v0.4.0 RuntimeSDK 启动。");
    ycrlog::Line("ASI插件化 By Luminous with ChatGPT");
    ycrlog::Line("原始文件来自“汉堂之家”坛友武英仲分享的三合一、五合一补丁");
    ycrlog::Text("[启动模式] ");
    ycrlog::Line(mode);
}

void LogSuccess(BYTE driveLetter) {
    ycrlog::Line("[免CD] 原版光盘扫描函数已安全跳过。");
    ycrlog::Text("[免CD] 备用盘符已设为 RPG.exe 所在驱动器：");
    ycrlog::Character(static_cast<char>(driveLetter));
    ycrlog::Line(":");
    ycrlog::Line("[资源] 原版本地 MultiMedia 优先读取流程保持不变。");
}

CastleStringView View(const char* text, CastleU32 length) {
    CastleStringView value{};
    value.data = text;
    value.length = length;
    return value;
}

const CastleHookApiV1* QueryHookApi(const CastleRuntimeApiV1* runtimeApi) {
    static const char interfaceId[] = CASTLE_HOOK_INTERFACE_ID;
    CastleInterfaceQueryV1 query{};
    CastleInterfaceResultV1 result{};
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = View(interfaceId,
        static_cast<CastleU32>(sizeof(interfaceId) - 1u));
    query.requested_version = CASTLE_HOOK_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_HOOK_API_V1;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (!runtimeApi || runtimeApi->QueryInterface(&query, &result) != CASTLE_OK) {
        return nullptr;
    }
    return static_cast<const CastleHookApiV1*>(result.api_pointer);
}

CastleResult InitializeStandalone() {
    OpenStartupLog("Standalone：同目录不存在 Castle_Runtime.dll，使用插件本地安全补丁器。");
    const bool recognized = IsCdScanFunctionRecognized();
    const bool patched = recognized && ycr::SetPatchSetState(&kSkipCdScan, 1, true);
    if (!patched) {
        ycrlog::Line(recognized
            ? "[失败] 光盘扫描函数已识别，但本地内存补丁写入失败。"
            : "[失败] 光盘扫描函数入口指纹不匹配；未安装免CD补丁。");
        return CASTLE_ERROR_EXPECTED_BYTES;
    }
    const BYTE driveLetter = DetectExeDriveLetter();
    StoreFallbackDriveLetter(driveLetter);
    LogSuccess(driveLetter);
    return CASTLE_OK;
}

CastleResult InitializeIntegrated(const CastleRuntimeApiV1* runtimeApi,
                                  CastlePluginHandle pluginHandle) {
    static const BYTE driveOriginal[] = {0x00u};
    static const char transactionLabel[] = "NoCD code/data transaction";
    static const char codeLabel[] = "skip PlayDisk scan";
    static const char driveLabel[] = "fallback drive letter";
    CastleRuntimeInfoV1 runtimeInfo{};
    CastleTransactionHandle transaction = 0u;
    CastleClaimHandle codeClaim = 0u;
    CastleClaimHandle driveClaim = 0u;
    CastleStatePatchClaimV1 codePatch{};
    CastleStatePatchClaimV1 drivePatch{};
    const CastleHookApiV1* hookApi = QueryHookApi(runtimeApi);
    const BYTE driveLetter = DetectExeDriveLetter();

    if (!ycrlog::BindRuntime(runtimeApi, pluginHandle)) {
        return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    }
    OpenStartupLog("Integrated：Castle_Runtime.dll 统一拥有补丁事务。");
    runtimeInfo.magic = CASTLE_RUNTIME_INFO_MAGIC;
    runtimeInfo.struct_size = CASTLE_SIZEOF_RUNTIME_INFO_V1;
    runtimeInfo.info_version = CASTLE_RUNTIME_INFO_VERSION_1;
    if (!hookApi || runtimeApi->GetRuntimeInfo(&runtimeInfo) != CASTLE_OK) {
        ycrlog::Line("[失败] Runtime Hook/Info 接口不可用；未执行任何私有写入。");
        return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    }
    if (hookApi->BeginTransaction(pluginHandle,
            View(transactionLabel, static_cast<CastleU32>(sizeof(transactionLabel) - 1u)),
            0u, &transaction) != CASTLE_OK) {
        return CASTLE_ERROR_TRANSACTION_STATE;
    }

    codePatch.magic = CASTLE_STATE_PATCH_MAGIC;
    codePatch.struct_size = CASTLE_SIZEOF_STATE_PATCH_V1;
    codePatch.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    codePatch.flags = CASTLE_PATCH_FLAG_CODE | CASTLE_PATCH_FLAG_KEEP_ON_PROCESS_EXIT;
    codePatch.target.module = runtimeInfo.game_module;
    codePatch.target.rva = 0x00002A10u;
    codePatch.target.size = static_cast<CastleU32>(sizeof(kCdScanOriginalFull));
    codePatch.original_bytes = kCdScanOriginalFull;
    codePatch.original_size = static_cast<CastleU32>(sizeof(kCdScanOriginalFull));
    codePatch.enabled_bytes = kCdScanEnabledFull;
    codePatch.enabled_size = static_cast<CastleU32>(sizeof(kCdScanEnabledFull));
    codePatch.desired_state = CASTLE_PATCH_STATE_ENABLED;
    codePatch.label = View(codeLabel, static_cast<CastleU32>(sizeof(codeLabel) - 1u));

    drivePatch.magic = CASTLE_STATE_PATCH_MAGIC;
    drivePatch.struct_size = CASTLE_SIZEOF_STATE_PATCH_V1;
    drivePatch.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    drivePatch.flags = CASTLE_PATCH_FLAG_DATA | CASTLE_PATCH_FLAG_KEEP_ON_PROCESS_EXIT;
    drivePatch.target.module = runtimeInfo.game_module;
    drivePatch.target.rva = kCdDriveLetterRva;
    drivePatch.target.size = 1u;
    drivePatch.original_bytes = driveOriginal;
    drivePatch.original_size = 1u;
    drivePatch.enabled_bytes = &driveLetter;
    drivePatch.enabled_size = 1u;
    drivePatch.desired_state = CASTLE_PATCH_STATE_ENABLED;
    drivePatch.label = View(driveLabel, static_cast<CastleU32>(sizeof(driveLabel) - 1u));

    CastleResult result = hookApi->AddStatePatch(transaction, &codePatch, &codeClaim);
    if (result >= 0) result = hookApi->AddStatePatch(transaction, &drivePatch, &driveClaim);
    if (result >= 0) result = hookApi->PreflightTransaction(transaction);
    if (result >= 0) result = hookApi->CommitTransaction(transaction);
    if (result < 0) {
        hookApi->AbortTransaction(transaction);
        ycrlog::Line("[失败] Runtime 拒绝 NoCD 事务；两处目标保持提交前状态。");
        return result;
    }
    LogSuccess(driveLetter);
    return CASTLE_OK;
}

void RuntimeFault(CastleResult failure) {
    (void)failure;
}

}  // namespace

static CastleResult CASTLE_RUNTIME_CALL NoCD_Integrated(
    const CastleRuntimeApiV1* runtimeApi, CastlePluginHandle pluginHandle,
    void* userContext) {
    (void)userContext;
    return InitializeIntegrated(runtimeApi, pluginHandle);
}

static CastleResult CASTLE_RUNTIME_CALL NoCD_Standalone(void* userContext) {
    (void)userContext;
    return InitializeStandalone();
}

static void CASTLE_RUNTIME_CALL NoCD_RuntimeFault(CastleResult failure,
                                                  void* userContext) {
    (void)userContext;
    RuntimeFault(failure);
}

static void CASTLE_RUNTIME_CALL NoCD_ProcessExit(void* userContext) {
    (void)userContext;
    ycrlog::Line("[退出] 免 CD 插件随进程结束。");
    ycrlog::Close();
}

static const char kPluginId[] = "org.castlereforge.extra.nocd";
static const char kDisplayName[] = "Castle NoCD";
static const char kVersionText[] = "0.4.0";
static const char kBuildId[] = "runtimesdk-v1";

static const CastlePluginDescriptorV1 gPluginDescriptor = {
    CASTLE_PLUGIN_DESC_MAGIC,
    CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
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
    CASTLE_CLIENT_CONFIG_MAGIC,
    CASTLE_SIZEOF_CLIENT_CONFIG_V1,
    CASTLE_CLIENT_CONFIG_VERSION_1,
    CASTLE_CLIENT_FLAG_REQUIRE_RUNTIME,
    NoCD_Integrated,
    NoCD_Standalone,
    NoCD_RuntimeFault,
    NoCD_ProcessExit,
    nullptr
};

static CastlePluginExportV1 gPluginExport = {
    CASTLE_PLUGIN_QUERY_MAGIC,
    CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1,
    0u,
    &gPluginDescriptor,
    &gClientConfig,
    0u,
    nullptr
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
