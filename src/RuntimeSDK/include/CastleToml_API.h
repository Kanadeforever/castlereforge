#ifndef CASTLE_TOML_API_H
#define CASTLE_TOML_API_H

/*
 * CastleToml_API.h
 *
 * TOML v1 是 Runtime 内置的配置文档服务。第一版公开所有官方 ASI 配置实际需要的
 * bool、32位整数和 UTF-8 字符串；文档句柄与解析缓冲都由 Runtime 持有，插件不再各自
 * 调 GetPrivateProfile* 或手写一套 INI/TOML 词法。任务数据库的数组表仍可通过 File API
 * 读取，但其通用数组遍历将在不破坏本 v1 标量 ABI 的后续版本追加。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_TOML_INTERFACE_ID "org.castlereforge.runtime.toml"
#define CASTLE_TOML_API_VERSION_1 1u

#define CASTLE_TOML_API_MAGIC 0x4C4D5443ul /* CTML */

#define CASTLE_TOML_CAP_BOOL   (1ul << 0)
#define CASTLE_TOML_CAP_S32    (1ul << 1)
#define CASTLE_TOML_CAP_STRING (1ul << 2)

typedef CastleU32 CastleTomlDocumentHandle;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleOpenPluginTomlFn)(
    CastlePluginHandle plugin,
    CastleStringView relative_path,
    CastleTomlDocumentHandle* out_document);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleCloseTomlFn)(
    CastleTomlDocumentHandle document);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleTomlGetBoolFn)(
    CastleTomlDocumentHandle document,
    CastleStringView table,
    CastleStringView key,
    CastleU32 default_value,
    CastleU32* out_value);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleTomlGetS32Fn)(
    CastleTomlDocumentHandle document,
    CastleStringView table,
    CastleStringView key,
    CastleS32 default_value,
    CastleS32 minimum_value,
    CastleS32 maximum_value,
    CastleS32* out_value);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleTomlGetStringFn)(
    CastleTomlDocumentHandle document,
    CastleStringView table,
    CastleStringView key,
    CastleStringView default_value,
    char* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

typedef struct CastleTomlApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleOpenPluginTomlFn OpenPluginDocument;
    CastleCloseTomlFn CloseDocument;
    CastleTomlGetBoolFn GetBool;
    CastleTomlGetS32Fn GetS32;
    CastleTomlGetStringFn GetString;
} CastleTomlApiV1;

#define CASTLE_SIZEOF_TOML_API_V1 36u

#pragma pack(pop)

#endif /* CASTLE_TOML_API_H */
