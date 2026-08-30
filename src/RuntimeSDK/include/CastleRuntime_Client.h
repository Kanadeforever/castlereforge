#ifndef CASTLE_RUNTIME_CLIENT_H
#define CASTLE_RUNTIME_CLIENT_H

/*
 * CastleRuntime_Client.h
 *
 * 每个 SDK ASI 都会把 Client 的 C 源码编进自己的二进制。
 * Client 不是第二个 Runtime：它只负责最早期 Entry Gate、同目录 Runtime 发现、
 * InitializeASI 兼容入口和本插件的原子状态。
 *
 * 真正的跨插件所有权、Hook 事务、后端和调度只存在于 Castle_Runtime.dll。
 */

#include "CastlePlugin_API.h"

#pragma pack(push, 4)

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Client 状态只允许单向推进。
 * GATE_OWNER/FOLLOWER 说明本插件在启动 Gate 中的角色，不代表业务已经初始化。
 */
#define CASTLE_CLIENT_COLD                  0u
#define CASTLE_CLIENT_GATE_OWNER            1u
#define CASTLE_CLIENT_GATE_FOLLOWER         2u
#define CASTLE_CLIENT_BOOTSTRAPPING         3u
#define CASTLE_CLIENT_STANDALONE            4u
#define CASTLE_CLIENT_JOINING               5u
#define CASTLE_CLIENT_INTEGRATED            6u
#define CASTLE_CLIENT_RUNTIME_FAULT         7u
#define CASTLE_CLIENT_PLUGIN_FAILED         8u
#define CASTLE_CLIENT_LATE_LOAD_UNSUPPORTED 9u
#define CASTLE_CLIENT_PROCESS_EXIT          10u

/* 已确认台湾第三版 RPG.exe 的入口 Gate 常量。未知构建不能使用这些数字盲写。 */
#define CASTLE_RPG_ENTRY_VA_V1       0x00452C19ul
#define CASTLE_RPG_ENTRY_RVA_V1      0x00052C19ul
#define CASTLE_RPG_ENTRY_AFTER_VA_V1 0x00452C1Eul
#define CASTLE_RPG_ENTRY_PATCH_SIZE  5u

/*
 * 插件 DllMain 的 PROCESS_ATTACH 路径调用这个函数。
 * 实现只能安装/验证 Entry Gate，不能读取 INI、加载 Runtime 或安装业务 Hook。
 */
CastleResult CASTLE_RUNTIME_CALL CastleRuntimeClient_OnProcessAttach(
    CastleModule self_module,
    CastlePluginExportV1* plugin_export);

/*
 * 标准 InitializeASI 只调用 RunNow。
 * MODLoader 两阶段流程中，第一个 SDK 插件会触发全进程 BootstrapAll，后续调用幂等返回。
 */
CastleResult CASTLE_RUNTIME_CALL CastleRuntimeClient_RunNow(void);

/*
 * Entry Gate Owner 在 RPG.exe 原入口调用 BootstrapAll。
 * 这个函数会恢复 Gate，然后选择 Runtime 整合模式或无 Runtime 的 Standalone 模式。
 */
CastleResult CASTLE_RUNTIME_CALL CastleRuntimeClient_BootstrapAll(
    CastleU32 trigger_kind,
    CastleModule trigger_module);

/* DllMain PROCESS_DETACH 调用；reserved 是否为空决定主动卸载还是整个进程退出。 */
void CASTLE_RUNTIME_CALL CastleRuntimeClient_OnProcessDetach(void* reserved);

/* 只读取得本插件当前 Client 状态，主要供日志和机械测试。 */
CastleU32 CASTLE_RUNTIME_CALL CastleRuntimeClient_GetState(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#pragma pack(pop)

#endif /* CASTLE_RUNTIME_CLIENT_H */
