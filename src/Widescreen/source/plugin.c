#include "platform.h"
#include "runtime.h"
#include "widescreen.h"
#include "CastleRuntime_Client.h"

/* DllMain 只安装 Entry Gate；显示资源创建前的正式初始化仍发生在 RPG 原入口。 */
static HMODULE g_plugin_module;

static CastleResult initialize_standalone(void) {
    if (!Runtime_Initialize(g_plugin_module)) return CASTLE_ERROR_RUNTIME_FAULT;
    if (!Runtime_ExactBuildProtocolOk()) return CASTLE_ERROR_UNKNOWN_GAME_BUILD;
    return Widescreen_Install() ? CASTLE_OK : CASTLE_ERROR_EXPECTED_BYTES;
}

static CastleResult initialize_integrated(const CastleRuntimeApiV1* runtime_api,
                                          CastlePluginHandle plugin_handle) {
    if (!Runtime_BindSdkLog(runtime_api, plugin_handle)) {
        return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    }
    if (!Runtime_Initialize(g_plugin_module)) return CASTLE_ERROR_RUNTIME_FAULT;
    if (!Runtime_ExactBuildProtocolOk()) return CASTLE_ERROR_UNKNOWN_GAME_BUILD;
    if (!Runtime_BeginSdkHookTransaction(runtime_api, plugin_handle)) {
        return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    }
    if (!Widescreen_Install()) {
        Runtime_AbortSdkHookTransaction();
        return CASTLE_ERROR_EXPECTED_BYTES;
    }
    if (!Runtime_CommitSdkHookTransaction()) return CASTLE_ERROR_RESOURCE_CONFLICT;
    if (!Widescreen_RegisterRuntimeServices(runtime_api, plugin_handle)) {
        Runtime_Log("[RuntimeSDK] Hook 已启用，但 Display/Render 服务未完整注册。");
        return CASTLE_STATUS_OPTIONAL_UNAVAILABLE;
    }
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL Widescreen_Integrated(
    const CastleRuntimeApiV1* runtime_api, CastlePluginHandle plugin_handle,
    void* user_context) {
    (void)user_context;
    return initialize_integrated(runtime_api, plugin_handle);
}

static CastleResult CASTLE_RUNTIME_CALL Widescreen_Standalone(void* user_context) {
    (void)user_context;
    return initialize_standalone();
}

static void CASTLE_RUNTIME_CALL Widescreen_RuntimeFault(CastleResult failure,
                                                        void* user_context) {
    (void)failure;
    (void)user_context;
    /* Runtime 不可用时官方插件保持停用，不在 ASI 目录创建旁路日志。 */
}

static void CASTLE_RUNTIME_CALL Widescreen_ProcessExit(void* user_context) {
    (void)user_context;
    Runtime_Shutdown();
}

static const char g_plugin_id[] = "org.castlereforge.widescreen";
static const char g_display_name[] = "Castle Widescreen";
static const char g_version_text[] = "0.12.0";
static const char g_build_id[] = "runtimesdk-v1";
static const CastlePluginDescriptorV1 g_plugin_descriptor = {
    CASTLE_PLUGIN_DESC_MAGIC, CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_REQUESTS_HOOKS |
        CASTLE_PLUGIN_FLAG_PROVIDES_BACKEND | CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE,
    0u,
    {g_plugin_id, (CastleU32)(sizeof(g_plugin_id) - 1u)},
    {g_display_name, (CastleU32)(sizeof(g_display_name) - 1u)},
    {g_version_text, (CastleU32)(sizeof(g_version_text) - 1u)},
    {g_build_id, (CastleU32)(sizeof(g_build_id) - 1u)}
};
static const CastleRuntimeClientConfigV1 g_client_config = {
    CASTLE_CLIENT_CONFIG_MAGIC, CASTLE_SIZEOF_CLIENT_CONFIG_V1,
    CASTLE_CLIENT_CONFIG_VERSION_1, CASTLE_CLIENT_FLAG_REQUIRE_RUNTIME,
    Widescreen_Integrated, Widescreen_Standalone, Widescreen_RuntimeFault,
    Widescreen_ProcessExit, NULL
};
static CastlePluginExportV1 g_plugin_export = {
    CASTLE_PLUGIN_QUERY_MAGIC, CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1, 0u,
    &g_plugin_descriptor, &g_client_config, 0u, NULL
};

const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requested_version) {
    return requested_version == CASTLE_PLUGIN_EXPORT_VERSION_1 ?
        &g_plugin_export : NULL;
}

void __cdecl InitializeASI(void) {
    CastleRuntimeClient_RunNow();
}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, void* reserved) {
    if (reason == DLL_PROCESS_ATTACH_) {
        g_plugin_module = module;
        CastleRuntimeClient_OnProcessAttach((CastleModule)(SIZE_T)module,
                                             &g_plugin_export);
    } else if (reason == DLL_PROCESS_DETACH_) {
        CastleRuntimeClient_OnProcessDetach(reserved);
    }
    return TRUE;
}
