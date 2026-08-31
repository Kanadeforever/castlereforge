#ifndef CASTLE_RUNTIME_INTERNAL_H
#define CASTLE_RUNTIME_INTERNAL_H

/*
 * runtime_internal.h
 *
 * 这里集中声明 Runtime DLL 各私有模块之间的连接点。
 * 业务插件永远不能包含本头；它们只能使用 include/ 下的稳定公共 ABI。
 */

#include "runtime_platform.h"
#include "../include/CastleRuntime_API.h"
#include "../include/CastlePlugin_API.h"
#include "../include/CastleHook_API.h"
#include "../include/CastlePath_API.h"
#include "../include/CastleSchedule_API.h"
#include "../include/CastleDisplay_API.h"
#include "../include/CastleWindow_API.h"
#include "../include/CastleRender_API.h"

#define RUNTIME_MAX_PLUGINS       128u
#define RUNTIME_PLUGIN_ID_CAP     128u
#define RUNTIME_DISPLAY_NAME_CAP  128u
#define RUNTIME_VERSION_TEXT_CAP   64u
#define RUNTIME_BUILD_ID_CAP        64u
#define RUNTIME_PATH_WIDE_CAP     1024u
#define RUNTIME_PATH_UTF8_CAP     3072u
#define RUNTIME_GAME_BUILD_CAP      64u
#define RUNTIME_DIAGNOSTIC_CAP   65536u

/* 每个插件记录都由 Runtime 自己持有文字副本，不能借用插件临时缓冲区。 */
typedef struct RuntimePluginRecord {
    int used;
    HMODULE module;
    CastlePluginHandle handle;
    char plugin_id[RUNTIME_PLUGIN_ID_CAP];
    CastleU32 plugin_id_length;
    char display_name[RUNTIME_DISPLAY_NAME_CAP];
    CastleU32 display_name_length;
    char version_text[RUNTIME_VERSION_TEXT_CAP];
    CastleU32 version_text_length;
    char build_id[RUNTIME_BUILD_ID_CAP];
    CastleU32 build_id_length;
    CastleU32 flags;
    CastleU32 state;
    CastleU32 state_flags;
    CastleResult last_result;
    CastleU32 generation;
    CastleU32 active_claim_count;
    CastleU32 provider_count;
} RuntimePluginRecord;

extern HMODULE g_runtime_module;

/* 初始化、锁和最小内存/字符串辅助。 */
int Runtime_EnsureInitialized(void);
void Runtime_Lock(volatile LONG* lock_value);
void Runtime_Unlock(volatile LONG* lock_value);
void Runtime_ByteCopy(void* destination, const void* source, CastleU32 size);
void Runtime_ByteZero(void* destination, CastleU32 size);
CastleU32 Runtime_StringLength(const char* text);
int Runtime_StringEquals(const char* left, CastleU32 left_length,
                         const char* right, CastleU32 right_length);
void* Runtime_ResolveTarget(const CastleTargetAddressV1* target);
int Runtime_MemoryEquals(const void* memory, const CastleU8* expected,
                         CastleU32 size);
CastleResult Runtime_WriteMemory(void* target, const CastleU8* bytes,
                                 CastleU32 size, int executable);

/* Path/进程信息。 */
int Runtime_PathInitialize(void);
CastleStringView Runtime_GetPathView(void);
CastleStringView Runtime_GetGameBuildView(void);
CastleModule Runtime_GetGameModuleValue(void);
const CastlePathApiV1* Runtime_GetPathApiV1(void);

/*
 * 单后台线程调度；游戏线程阶段能力在获得实机证据前保持不可用。
 * Bootstrap 闸门保证插件可以先登记任务，但 RPG 入口到达前绝不会启动后台回调。
 */
void Runtime_ScheduleInitialize(void);
void Runtime_ScheduleCloseBootstrapGate(void);
void Runtime_ScheduleNotifyGameEntry(void);
const CastleScheduleApiV1* Runtime_GetScheduleApiV1(void);

/* 已确认游戏 profile 与内部公共符号。未知构建时这些函数安全返回不可用。 */
void Runtime_SymbolsInitialize(void);
int Runtime_GameProfileSupported(void);
int Runtime_ReadOriginalCamera(CastleS32* out_x, CastleS32* out_y);
void* Runtime_GetOriginalRenderQueue(void);
void* Runtime_GetOriginalDisplayPresent(void);

/* Display 统一门面与专业后端注册。 */
void Runtime_DisplayInitialize(void);
const CastleDisplayApiV1* Runtime_GetDisplayApiV1(void);
CastleU32 Runtime_GetDisplayProviderGeneration(void);
int Runtime_DisplayProviderReadyForPlugin(CastleProviderHandle provider,
                                          CastlePluginHandle plugin,
                                          CastleU32* out_provider_generation,
                                          CastleU32* out_geometry_generation);
CastleResult Runtime_GetCurrentDisplayGeneration(CastleU32* out_generation);

/* 动态 USER32 主 WndProc 和消息分发。 */
int Runtime_WindowInitialize(void);
const CastleWindowApiV1* Runtime_GetWindowApiV1(void);

/* Render/Present 门面和额外世界帧独占租约。 */
void Runtime_RenderInitialize(void);
const CastleRenderApiV1* Runtime_GetRenderApiV1(void);
CastleU32 Runtime_GetRenderProviderGeneration(void);

/* 诊断环。 */
void Runtime_DiagnosticsInitialize(void);
void Runtime_DiagnosticAppend(const char* utf8_line);
CastleU32 Runtime_DiagnosticGeneration(void);
CastleResult Runtime_CopyDiagnostics(CastleU32 after_generation,
                                     CastleDiagnosticBufferV1* output);

/* 插件登记与状态。 */
void Runtime_RegistryInitialize(void);
CastleResult Runtime_RegisterPlugin(const CastlePluginDescriptorV1* descriptor,
                                    CastlePluginHandle* out_handle);
CastleResult Runtime_GetPluginStateByHandle(CastlePluginHandle handle,
                                            CastlePluginStateV1* out_state);
void Runtime_SetPluginState(CastlePluginHandle handle, CastleU32 state,
                            CastleResult result);
HMODULE Runtime_GetPluginModule(CastlePluginHandle handle);
CastleStringView Runtime_GetPluginIdView(CastlePluginHandle handle);

/* Hook/补丁事务门面。 */
void Runtime_HookInitialize(void);
const CastleHookApiV1* Runtime_GetHookApiV1(void);

/* 全量 Bootstrap 与根 API 表。 */
CastleResult Runtime_BootstrapLoadedPlugins(const CastleBootstrapRequestV1* request,
                                            CastleBootstrapResultV1* out_result);
const CastleRuntimeApiV1* Runtime_GetApiV1(void);

#endif /* CASTLE_RUNTIME_INTERNAL_H */
