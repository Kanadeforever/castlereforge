#ifndef CASTLE_MODULE_API_H
#define CASTLE_MODULE_API_H

/*
 * CastleModule_API.h
 *
 * Module v1 统一外部 DLL 的安全定位、加载、导出查询和固定驻留。Controller 仍然决定怎样
 * 使用 SDL3，SaveEnhance 仍然决定何时播放声音；Runtime 只保证多个插件不会用不同搜索
 * 目录加载同名依赖，也不会在另一个插件仍保存函数地址时提前 FreeLibrary。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_MODULE_INTERFACE_ID "org.castlereforge.runtime.module"
#define CASTLE_MODULE_API_VERSION_1 1u
#define CASTLE_MODULE_STRUCTURE_VERSION_1 1u

#define CASTLE_MODULE_API_MAGIC   0x444F4D43ul /* CMOD */
#define CASTLE_MODULE_STATE_MAGIC 0x534D4F43ul /* COMS */

#define CASTLE_MODULE_CAP_PLUGIN_RELATIVE (1ul << 0)
#define CASTLE_MODULE_CAP_SYSTEM_ALLOWLIST (1ul << 1)
#define CASTLE_MODULE_CAP_PIN             (1ul << 2)

#define CASTLE_MODULE_LOAD_PIN (1ul << 0)

typedef struct CastleModuleStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleModule module;
    CastlePluginHandle first_owner;
    CastleU32 pinned;
    CastleU32 generation;
} CastleModuleStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleLoadPluginDependencyFn)(
    CastlePluginHandle plugin,
    CastleStringView relative_path,
    CastleU32 flags,
    CastleModule* out_module);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleLoadSystemModuleFn)(
    CastlePluginHandle plugin,
    CastleStringView module_name,
    CastleU32 flags,
    CastleModule* out_module);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetModuleProcedureFn)(
    CastleModule module,
    CastleStringView procedure_name,
    CastleAddress* out_procedure);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePinModuleFn)(
    CastleModule module);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetModuleStateFn)(
    CastleModule module,
    CastleModuleStateV1* out_state);

typedef struct CastleModuleApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleLoadPluginDependencyFn LoadPluginDependency;
    CastleLoadSystemModuleFn LoadSystemModule;
    CastleGetModuleProcedureFn GetProcedure;
    CastlePinModuleFn PinModule;
    CastleGetModuleStateFn GetModuleState;
} CastleModuleApiV1;

#define CASTLE_SIZEOF_MODULE_STATE_V1 32u
#define CASTLE_SIZEOF_MODULE_API_V1   36u

#pragma pack(pop)

#endif /* CASTLE_MODULE_API_H */
