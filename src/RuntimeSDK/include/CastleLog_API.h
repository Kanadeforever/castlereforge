#ifndef CASTLE_LOG_API_H
#define CASTLE_LOG_API_H

/*
 * CastleLog_API.h
 *
 * Log v1 把“日志文件放在哪里、怎样安全并发写入、何时刷新”交给 Runtime 统一负责。
 * 每个插件仍然拥有自己的日志文件和业务文字；Runtime 不会把手柄、存档、任务等日志
 * 全部混进 Castle_Runtime.log。这样目录规整以后，排查某一个插件仍然只需打开一个文件。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_LOG_INTERFACE_ID "org.castlereforge.runtime.log"
#define CASTLE_LOG_API_VERSION_1 1u
#define CASTLE_LOG_STRUCTURE_VERSION_1 1u

#define CASTLE_LOG_API_MAGIC    0x474F4C43ul /* CLOG */
#define CASTLE_LOG_RECORD_MAGIC 0x524F4C43ul /* CLOR */

#define CASTLE_LOG_CAP_SEPARATE_PLUGIN_FILES (1ul << 0)
#define CASTLE_LOG_CAP_UTF8_BOM               (1ul << 1)
#define CASTLE_LOG_CAP_SYNCHRONOUS_FLUSH       (1ul << 2)

/* 级别只用于统一诊断语义；插件写入的正文不会被 Runtime 改写。 */
#define CASTLE_LOG_TRACE 0u
#define CASTLE_LOG_INFO  1u
#define CASTLE_LOG_WARN  2u
#define CASTLE_LOG_ERROR 3u
#define CASTLE_LOG_FATAL 4u

typedef struct CastleLogRecordV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 level;
    CastleStringView message;
} CastleLogRecordV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleWritePluginLogFn)(
    CastlePluginHandle plugin,
    const CastleLogRecordV1* record);

/* 分段日志器使用本函数拼接同一行；Runtime 不自动追加 CRLF。 */
typedef CastleResult (CASTLE_RUNTIME_CALL *CastleWritePluginLogTextFn)(
    CastlePluginHandle plugin,
    CastleStringView text);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleFlushPluginLogFn)(
    CastlePluginHandle plugin);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetPluginLogPathUtf8Fn)(
    CastlePluginHandle plugin,
    char* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetLogDirectoryUtf8Fn)(
    char* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

/* 16 字节表头 + 5 个函数指针 = 36 字节。 */
typedef struct CastleLogApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleWritePluginLogFn WritePluginLine;
    CastleFlushPluginLogFn FlushPluginLog;
    CastleGetPluginLogPathUtf8Fn GetPluginLogPathUtf8;
    CastleGetLogDirectoryUtf8Fn GetLogDirectoryUtf8;
    CastleWritePluginLogTextFn WritePluginText;
} CastleLogApiV1;

#define CASTLE_SIZEOF_LOG_RECORD_V1 24u
#define CASTLE_SIZEOF_LOG_API_V1    36u

#pragma pack(pop)

#endif /* CASTLE_LOG_API_H */
