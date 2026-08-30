#ifndef CASTLE_PATH_API_H
#define CASTLE_PATH_API_H

/*
 * CastlePath_API.h
 *
 * Path v1 统一所有插件的 ASI 同目录路径构造。
 * 调用方只能提交相对路径，Runtime 拒绝盘符、UNC、根路径和 .. 逃逸；
 * 这样插件不会因为 CurrentDirectory 或 SetDllDirectory 被其它模块改变而读错文件。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_PATH_INTERFACE_ID "org.castlereforge.runtime.path"
#define CASTLE_PATH_API_VERSION_1 1u
#define CASTLE_PATH_API_MAGIC     0x48545043ul /* CPTH */
#define CASTLE_PATH_INFO_MAGIC    0x464E5043ul /* CPNF */
#define CASTLE_PATH_VERSION_1     1u

typedef struct CastleWideStringView {
    const CastleU16* data;
    CastleU32 length;
} CastleWideStringView;

/*
 * 四个只读视图都由 Runtime 持有，进程结束前地址稳定。
 * length 只统计有效字符，不包含末尾 NUL；调用方不得修改 data 指向的内存。
 */
typedef struct CastlePathInfoV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 generation;
    CastleStringView asi_directory_utf8;
    CastleWideStringView asi_directory_wide;
    CastleStringView runtime_path_utf8;
    CastleWideStringView runtime_path_wide;
} CastlePathInfoV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathGetInfoFn)(
    CastlePathInfoV1* out_info);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathBuildUtf8Fn)(
    CastlePluginHandle plugin,
    CastleStringView relative_path,
    char* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathCopyPluginPathUtf8Fn)(
    CastlePluginHandle plugin,
    char* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathCopyPluginPathWideFn)(
    CastlePluginHandle plugin,
    CastleU16* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathBuildWideFn)(
    CastlePluginHandle plugin,
    CastleWideStringView relative_path,
    CastleU16* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathNormalizeUtf8Fn)(
    CastleStringView absolute_path,
    char* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathNormalizeWideFn)(
    CastleWideStringView absolute_path,
    CastleU16* output,
    CastleU32 output_capacity,
    CastleU32* out_length);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathEqualsUtf8Fn)(
    CastleStringView left,
    CastleStringView right,
    CastleU32* out_equal);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastlePathEqualsWideFn)(
    CastleWideStringView left,
    CastleWideStringView right,
    CastleU32* out_equal);

typedef struct CastlePathApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastlePathGetInfoFn GetPathInfo;
    CastlePathCopyPluginPathUtf8Fn GetPluginModulePathUtf8;
    CastlePathCopyPluginPathWideFn GetPluginModulePathWide;
    CastlePathBuildUtf8Fn BuildPluginRelativePathUtf8;
    CastlePathBuildWideFn BuildPluginRelativePathWide;
    CastlePathNormalizeUtf8Fn NormalizeAbsolutePathUtf8;
    CastlePathNormalizeWideFn NormalizeAbsolutePathWide;
    CastlePathEqualsUtf8Fn PathsEqualUtf8;
    CastlePathEqualsWideFn PathsEqualWide;
} CastlePathApiV1;

#define CASTLE_SIZEOF_WIDE_STRING_VIEW_V1 8u
#define CASTLE_SIZEOF_PATH_INFO_V1        52u
#define CASTLE_SIZEOF_PATH_API_V1         52u

#pragma pack(pop)

#endif /* CASTLE_PATH_API_H */
