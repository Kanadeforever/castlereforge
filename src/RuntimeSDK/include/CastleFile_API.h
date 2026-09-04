#ifndef CASTLE_FILE_API_H
#define CASTLE_FILE_API_H

/*
 * CastleFile_API.h
 *
 * File v1 统一插件私有文件的安全路径、共享模式和原子替换。所有相对路径都以 ASI 目录
 * 为根并拒绝 ..、盘符和 UNC；原版 TSF、MOD Loader Overrides 与日志有各自专用所有者，
 * 不通过本接口，避免一个“万能文件 API”把完全不同的生命周期混在一起。
 */

#include "CastlePath_API.h"

#pragma pack(push, 4)

#define CASTLE_FILE_INTERFACE_ID "org.castlereforge.runtime.file"
#define CASTLE_FILE_API_VERSION_1 1u
#define CASTLE_FILE_STRUCTURE_VERSION_1 1u

#define CASTLE_FILE_API_MAGIC    0x454C4643ul /* CFLE */
#define CASTLE_FILE_BUFFER_MAGIC 0x55424643ul /* CFBU */
#define CASTLE_FILE_INFO_MAGIC   0x4E494643ul /* CFIN */

#define CASTLE_FILE_CAP_READ          (1ul << 0)
#define CASTLE_FILE_CAP_ATOMIC_WRITE  (1ul << 1)
#define CASTLE_FILE_CAP_ENUMERATE     (1ul << 2)
#define CASTLE_FILE_CAP_DIRECTORY     (1ul << 3)

#define CASTLE_FILE_TYPE_REGULAR 1u
#define CASTLE_FILE_TYPE_DIRECTORY 2u

typedef struct CastleFileBufferV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU8* data;
    CastleU32 capacity;
    CastleU32 bytes_written;
    CastleU32 required_capacity;
} CastleFileBufferV1;

typedef struct CastleFileInfoV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 file_type;
    CastleU32 size_low;
    CastleU32 size_high;
    CastleStringView name_utf8;
} CastleFileInfoV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleFileEnumerateCallbackFn)(
    const CastleFileInfoV1* file,
    void* user_context);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleReadPluginFileFn)(
    CastlePluginHandle plugin,
    CastleStringView relative_path,
    CastleFileBufferV1* output);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleWritePluginFileAtomicFn)(
    CastlePluginHandle plugin,
    CastleStringView relative_path,
    const CastleU8* data,
    CastleU32 size);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleDeletePluginFileFn)(
    CastlePluginHandle plugin,
    CastleStringView relative_path);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleEnsurePluginDirectoryFn)(
    CastlePluginHandle plugin,
    CastleStringView relative_path);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleEnumeratePluginFilesFn)(
    CastlePluginHandle plugin,
    CastleStringView relative_pattern,
    CastleFileEnumerateCallbackFn callback,
    void* user_context,
    CastleU32* out_count);

/* 16 字节表头 + 5 个函数指针 = 36 字节。 */
typedef struct CastleFileApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleReadPluginFileFn ReadPluginFile;
    CastleWritePluginFileAtomicFn WritePluginFileAtomic;
    CastleDeletePluginFileFn DeletePluginFile;
    CastleEnsurePluginDirectoryFn EnsurePluginDirectory;
    CastleEnumeratePluginFilesFn EnumeratePluginFiles;
} CastleFileApiV1;

#define CASTLE_SIZEOF_FILE_BUFFER_V1 32u
#define CASTLE_SIZEOF_FILE_INFO_V1   36u
#define CASTLE_SIZEOF_FILE_API_V1    36u

#pragma pack(pop)

#endif /* CASTLE_FILE_API_H */
