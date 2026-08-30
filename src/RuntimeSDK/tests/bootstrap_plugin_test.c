/*
 * bootstrap_plugin_test.c
 *
 * 同一源码分别构建为 A/B 两个测试 ASI。测试宿主故意先加载 B 再加载 A，Runtime
 * 必须仍按稳定 plugin_id 先激活 A。这个 DLL 不安装 Hook，只记录 Bootstrap 参数。
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "CastleRuntime_API.h"
#include "CastlePlugin_API.h"

#if defined(TEST_PLUGIN_A)
static const char g_plugin_id[] = "org.castlereforge.test.bootstrap.a";
static const char g_display_name[] = "Bootstrap Test A";
#define TEST_ORDER_TOKEN 1u
#else
static const char g_plugin_id[] = "org.castlereforge.test.bootstrap.b";
static const char g_display_name[] = "Bootstrap Test B";
#define TEST_ORDER_TOKEN 2u
#endif

static const char g_version_text[] = "1.0.0";
static const char g_build_id[] = "bootstrap-test";
static CastleU32 g_bootstrap_count;
static CastleU32 g_bootstrap_mode;
static CastlePluginHandle g_bootstrap_handle;

typedef void (CASTLE_RUNTIME_CALL *RuntimeTestRecordBootstrapFn)(CastleU32 token);

static const CastlePluginDescriptorV1 g_descriptor = {
    CASTLE_PLUGIN_DESC_MAGIC,
    CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_SUPPORTS_STANDALONE,
    0u,
    {g_plugin_id, (CastleU32)(sizeof(g_plugin_id) - 1u)},
    {g_display_name, (CastleU32)(sizeof(g_display_name) - 1u)},
    {g_version_text, (CastleU32)(sizeof(g_version_text) - 1u)},
    {g_build_id, (CastleU32)(sizeof(g_build_id) - 1u)}
};

static const CastleRuntimeClientConfigV1 g_client_config = {
    CASTLE_CLIENT_CONFIG_MAGIC,
    CASTLE_SIZEOF_CLIENT_CONFIG_V1,
    CASTLE_CLIENT_CONFIG_VERSION_1,
    0u,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static CastleResult CASTLE_RUNTIME_CALL test_client_bootstrap_(
    CastleU32 mode,
    const CastleRuntimeApiV1* runtime_api,
    CastlePluginHandle plugin_handle,
    CastleResult reason) {
    HMODULE host_module;
    RuntimeTestRecordBootstrapFn record;
    (void)reason;
    if (mode != CASTLE_CLIENT_BOOTSTRAP_INTEGRATED || !runtime_api ||
        plugin_handle == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    g_bootstrap_mode = mode;
    g_bootstrap_handle = plugin_handle;
    ++g_bootstrap_count;
    host_module = GetModuleHandleW(NULL);
    record = host_module ? (RuntimeTestRecordBootstrapFn)GetProcAddress(
        host_module, "RuntimeTest_RecordBootstrap") : NULL;
    if (!record) return CASTLE_ERROR_RUNTIME_FAULT;
    record(TEST_ORDER_TOKEN);
    return CASTLE_OK;
}

static const CastlePluginExportV1 g_plugin_export = {
    CASTLE_PLUGIN_QUERY_MAGIC,
    CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1,
    0u,
    &g_descriptor,
    &g_client_config,
    0u,
    test_client_bootstrap_
};

const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requested_export_version) {
    return requested_export_version == CASTLE_PLUGIN_EXPORT_VERSION_1 ?
        &g_plugin_export : NULL;
}

CastleU32 CASTLE_RUNTIME_CALL RuntimeTest_GetBootstrapCount(void) {
    return g_bootstrap_count;
}

CastleU32 CASTLE_RUNTIME_CALL RuntimeTest_GetBootstrapMode(void) {
    return g_bootstrap_mode;
}

CastlePluginHandle CASTLE_RUNTIME_CALL RuntimeTest_GetBootstrapHandle(void) {
    return g_bootstrap_handle;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    (void)instance;
    (void)reason;
    (void)reserved;
    return TRUE;
}
