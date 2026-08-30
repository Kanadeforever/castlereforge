#ifndef CASTLE_PLUGIN_API_H
#define CASTLE_PLUGIN_API_H

/*
 * CastlePlugin_API.h
 *
 * 这个头定义“一个 ASI 怎样向 Runtime 介绍自己”，以及 Runtime/Standalone Bootstrap
 * 怎样让该 ASI 在自己的模块内部完成状态转换。
 *
 * 插件文件名可以被用户修改，所以 Runtime 绝不能把 Castle_Xxx.asi 文件名当永久身份。
 * 稳定身份来自 plugin_id；文件名只用于部署和日志。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_PLUGIN_EXPORT_VERSION_1 1u
#define CASTLE_CLIENT_CONFIG_VERSION_1 1u

#define CASTLE_CLIENT_CONFIG_MAGIC 0x47464343ul /* CCFG */
#define CASTLE_PLUGIN_QUERY_MAGIC  0x52515043ul /* CPQR */

/* 插件能力事实。它们只描述边界，不代表对应领域接口一定已经注册成功。 */
#define CASTLE_PLUGIN_FLAG_SUPPORTS_STANDALONE (1ul << 0)
#define CASTLE_PLUGIN_FLAG_REQUESTS_HOOKS      (1ul << 1)
#define CASTLE_PLUGIN_FLAG_PROVIDES_BACKEND    (1ul << 2)
#define CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE     (1ul << 3)

/* ClientBootstrap 的模式。数值与 Runtime 根 Bootstrap 模式保持一致。 */
#define CASTLE_CLIENT_BOOTSTRAP_STANDALONE CASTLE_BOOTSTRAP_MODE_STANDALONE
#define CASTLE_CLIENT_BOOTSTRAP_INTEGRATED CASTLE_BOOTSTRAP_MODE_INTEGRATED
#define CASTLE_CLIENT_BOOTSTRAP_FAULT      CASTLE_BOOTSTRAP_MODE_FAULT

/*
 * 插件描述全部是只读元数据。
 * module 在静态表初始时可以为 0；Bootstrap 枚举模块时使用真实 HMODULE 构造登记副本。
 * 直接调用 RegisterPlugin 的代码则必须填入真实 module。
 */
typedef struct CastlePluginDescriptorV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 descriptor_version;
    CastleU32 flags;
    CastleModule module;
    CastleStringView plugin_id;
    CastleStringView display_name;
    CastleStringView version_text;
    CastleStringView build_id;
} CastlePluginDescriptorV1;

/*
 * IntegratedInitialize 在 Runtime 已登记插件、完成依赖计划后调用。
 * 插件在这里声明事务和后端，但不能回到 DllMain 规则之外偷偷修改游戏。
 */
typedef CastleResult (CASTLE_RUNTIME_CALL *CastleIntegratedInitializeFn)(
    const CastleRuntimeApiV1* runtime_api,
    CastlePluginHandle plugin_handle,
    void* user_context);

/*
 * StandaloneInitialize 只在同目录完全没有 Castle_Runtime.dll 时调用。
 * 这里启动的是同一份业务核心对应的 StandaloneHost，不是另一套复制业务。
 */
typedef CastleResult (CASTLE_RUNTIME_CALL *CastleStandaloneInitializeFn)(
    void* user_context);

/* Runtime 文件存在但无法安全使用时，只允许错误回调记录诊断，不能安装本地 Hook。 */
typedef void (CASTLE_RUNTIME_CALL *CastleRuntimeFaultFn)(
    CastleResult failure,
    void* user_context);

/* 进程退出回调必须很短，不能等待 Runtime 调度线程或重新加载 DLL。 */
typedef void (CASTLE_RUNTIME_CALL *CastleProcessExitFn)(
    void* user_context);

/*
 * 每个插件持有一份静态 Client 配置。
 * user_context 是插件自己的静态上下文指针；Runtime 不解释也不释放。
 */
typedef struct CastleRuntimeClientConfigV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 config_version;
    CastleU32 flags;
    CastleIntegratedInitializeFn integrated_initialize;
    CastleStandaloneInitializeFn standalone_initialize;
    CastleRuntimeFaultFn runtime_fault;
    CastleProcessExitFn process_exit;
    void* user_context;
} CastleRuntimeClientConfigV1;

/*
 * client_bootstrap 由每个 ASI 内嵌的 Client 实现。
 * BootstrapAll 调它，而不是直接调业务函数，这样每个模块能先更新自己的原子 Client 状态。
 */
typedef CastleResult (CASTLE_RUNTIME_CALL *CastleClientBootstrapFn)(
    CastleU32 mode,
    const CastleRuntimeApiV1* runtime_api,
    CastlePluginHandle plugin_handle,
    CastleResult reason);

/*
 * CastlePlugin_Query 返回这张静态表。
 * entry_gate_thunk 仅用于确认 RPG 入口现有 E9 确实来自兼容 SDK Client，不能直接调用。
 */
typedef struct CastlePluginExportV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 export_version;
    CastleU32 flags;
    const CastlePluginDescriptorV1* plugin_descriptor;
    const CastleRuntimeClientConfigV1* client_config;
    CastleAddress entry_gate_thunk;
    CastleClientBootstrapFn client_bootstrap;
} CastlePluginExportV1;

/* 标准未修饰导出 CastlePlugin_Query 的函数类型。未知版本返回空指针。 */
typedef const CastlePluginExportV1* (CASTLE_RUNTIME_CALL *CastlePluginQueryFn)(
    CastleU32 requested_export_version);

#define CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1 52u
#define CASTLE_SIZEOF_CLIENT_CONFIG_V1     36u
#define CASTLE_SIZEOF_PLUGIN_EXPORT_V1     32u

#pragma pack(pop)

#endif /* CASTLE_PLUGIN_API_H */
