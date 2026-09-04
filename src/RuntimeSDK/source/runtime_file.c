#include "runtime_internal.h"

#define RUNTIME_FILE_MAX_BYTES (16u * 1024u * 1024u)

static volatile LONG g_file_write_lock;
static volatile LONG g_file_temp_counter;

static CastleResult CASTLE_RUNTIME_CALL file_read_(
    CastlePluginHandle plugin, CastleStringView relative_path,
    CastleFileBufferV1* output);
static CastleResult CASTLE_RUNTIME_CALL file_write_atomic_(
    CastlePluginHandle plugin, CastleStringView relative_path,
    const CastleU8* data, CastleU32 size);
static CastleResult CASTLE_RUNTIME_CALL file_delete_(
    CastlePluginHandle plugin, CastleStringView relative_path);
static CastleResult CASTLE_RUNTIME_CALL file_ensure_directory_(
    CastlePluginHandle plugin, CastleStringView relative_path);
static CastleResult CASTLE_RUNTIME_CALL file_enumerate_(
    CastlePluginHandle plugin, CastleStringView relative_pattern,
    CastleFileEnumerateCallbackFn callback, void* user_context,
    CastleU32* out_count);

static const CastleFileApiV1 g_file_api = {
    CASTLE_FILE_API_MAGIC,
    CASTLE_SIZEOF_FILE_API_V1,
    CASTLE_FILE_API_VERSION_1,
    CASTLE_FILE_CAP_READ | CASTLE_FILE_CAP_ATOMIC_WRITE |
        CASTLE_FILE_CAP_ENUMERATE | CASTLE_FILE_CAP_DIRECTORY,
    file_read_,
    file_write_atomic_,
    file_delete_,
    file_ensure_directory_,
    file_enumerate_
};

static CastleResult file_build_path_(CastlePluginHandle plugin,
                                     CastleStringView relative_path,
                                     WCHAR* output,
                                     CastleU32* out_length) {
    CastleWideStringView unused;
    const CastlePathApiV1* path_api = Runtime_GetPathApiV1();
    CastleU16 wide[RUNTIME_PATH_WIDE_CAP];
    CastleU32 index;
    CastleResult result;
    (void)unused;
    if (!path_api || !output || !out_length) return CASTLE_ERROR_RUNTIME_FAULT;
    result = path_api->BuildPluginRelativePathUtf8(plugin, relative_path,
        (char*)NULL, 0u, out_length);
    /* 上面的 UTF-8 查询只验证路径；真正 Windows 调用需要宽字符输出。 */
    if (result != CASTLE_ERROR_BUFFER_TOO_SMALL && result < 0) return result;

    /* 先把显式长度 UTF-8 转成临时宽视图，再复用 Path 的完整根边界检查。 */
    if (!relative_path.data || relative_path.length == 0u ||
        relative_path.length >= RUNTIME_PATH_WIDE_CAP) return CASTLE_ERROR_INVALID_ARGUMENT;
    {
        int converted = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
            relative_path.data, (int)relative_path.length, (WCHAR*)wide,
            RUNTIME_PATH_WIDE_CAP - 1u);
        if (converted <= 0) return CASTLE_ERROR_INVALID_ARGUMENT;
        wide[converted] = 0u;
        unused.data = wide;
        unused.length = (CastleU32)converted;
    }
    result = path_api->BuildPluginRelativePathWide(plugin, unused, wide,
        RUNTIME_PATH_WIDE_CAP, out_length);
    if (result < 0) return result;
    for (index = 0u; index <= *out_length; ++index) output[index] = (WCHAR)wide[index];
    return CASTLE_OK;
}

static int file_append_hex8_(WCHAR* path, CastleU32 capacity,
                             CastleU32* position, CastleU32 value) {
    CastleS32 shift;
    static const WCHAR digits[] = L"0123456789ABCDEF";
    for (shift = 28; shift >= 0; shift -= 4) {
        if (*position + 1u >= capacity) return 0;
        path[(*position)++] = digits[(value >> (CastleU32)shift) & 0xFu];
    }
    path[*position] = L'\0';
    return 1;
}

static int file_build_temp_path_(const WCHAR* final_path, CastleU32 final_length,
                                 WCHAR* output) {
    static const WCHAR suffix[] = L".castle.tmp.";
    CastleU32 position = 0u;
    CastleU32 index;
    CastleU32 token = (CastleU32)InterlockedIncrement(&g_file_temp_counter) ^
                      (CastleU32)GetCurrentProcessId();
    if (!final_path || !output || final_length + 22u >= RUNTIME_PATH_WIDE_CAP) return 0;
    for (index = 0u; index < final_length; ++index) output[position++] = final_path[index];
    for (index = 0u; suffix[index]; ++index) output[position++] = suffix[index];
    return file_append_hex8_(output, RUNTIME_PATH_WIDE_CAP, &position, token);
}

void Runtime_FileInitialize(void) {
    g_file_write_lock = 0;
    g_file_temp_counter = 0;
}

const CastleFileApiV1* Runtime_GetFileApiV1(void) {
    return &g_file_api;
}

static CastleResult CASTLE_RUNTIME_CALL file_read_(
    CastlePluginHandle plugin, CastleStringView relative_path,
    CastleFileBufferV1* output) {
    WCHAR path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 path_length = 0u;
    HANDLE file;
    LARGE_INTEGER size;
    DWORD read = 0u;
    CastleResult result;
    if (!output || output->magic != CASTLE_FILE_BUFFER_MAGIC ||
        output->struct_size < CASTLE_SIZEOF_FILE_BUFFER_V1 ||
        output->version != CASTLE_FILE_STRUCTURE_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    output->bytes_written = 0u;
    output->required_capacity = 0u;
    result = file_build_path_(plugin, relative_path, path, &path_length);
    if (result < 0) return result;
    (void)path_length;
    file = CreateFileW(path, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return CASTLE_ERROR_NOT_READY;
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0 ||
        size.QuadPart > (LONGLONG)RUNTIME_FILE_MAX_BYTES) {
        CloseHandle(file);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    output->required_capacity = (CastleU32)size.QuadPart;
    if (output->required_capacity > output->capacity ||
        (output->required_capacity != 0u && !output->data)) {
        CloseHandle(file);
        return CASTLE_ERROR_BUFFER_TOO_SMALL;
    }
    if (output->required_capacity != 0u &&
        (!ReadFile(file, output->data, output->required_capacity, &read, NULL) ||
         read != output->required_capacity)) {
        CloseHandle(file);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    CloseHandle(file);
    output->bytes_written = output->required_capacity;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL file_write_atomic_(
    CastlePluginHandle plugin, CastleStringView relative_path,
    const CastleU8* data, CastleU32 size) {
    WCHAR final_path[RUNTIME_PATH_WIDE_CAP];
    WCHAR temp_path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 final_length = 0u;
    HANDLE file = INVALID_HANDLE_VALUE;
    DWORD written = 0u;
    CastleResult result;
    if ((size != 0u && !data) || size > RUNTIME_FILE_MAX_BYTES) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    result = file_build_path_(plugin, relative_path, final_path, &final_length);
    if (result < 0) return result;
    if (!file_build_temp_path_(final_path, final_length, temp_path)) {
        return CASTLE_ERROR_BUFFER_TOO_SMALL;
    }

    Runtime_Lock(&g_file_write_lock);
    file = CreateFileW(temp_path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        Runtime_Unlock(&g_file_write_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    if ((size != 0u && (!WriteFile(file, data, size, &written, NULL) || written != size)) ||
        !FlushFileBuffers(file)) {
        CloseHandle(file);
        DeleteFileW(temp_path);
        Runtime_Unlock(&g_file_write_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    CloseHandle(file);
    if (!MoveFileExW(temp_path, final_path,
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(temp_path);
        Runtime_Unlock(&g_file_write_lock);
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    Runtime_Unlock(&g_file_write_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL file_delete_(
    CastlePluginHandle plugin, CastleStringView relative_path) {
    WCHAR path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 length = 0u;
    CastleResult result = file_build_path_(plugin, relative_path, path, &length);
    (void)length;
    if (result < 0) return result;
    Runtime_Lock(&g_file_write_lock);
    if (!DeleteFileW(path)) {
        DWORD error = GetLastError();
        Runtime_Unlock(&g_file_write_lock);
        return error == ERROR_FILE_NOT_FOUND ? CASTLE_STATUS_ALREADY_DONE :
                                               CASTLE_ERROR_RUNTIME_FAULT;
    }
    Runtime_Unlock(&g_file_write_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL file_ensure_directory_(
    CastlePluginHandle plugin, CastleStringView relative_path) {
    WCHAR path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 length = 0u;
    CastleResult result = file_build_path_(plugin, relative_path, path, &length);
    (void)length;
    if (result < 0) return result;
    if (CreateDirectoryW(path, NULL)) return CASTLE_OK;
    return GetLastError() == ERROR_ALREADY_EXISTS ? CASTLE_STATUS_ALREADY_DONE :
                                                    CASTLE_ERROR_RUNTIME_FAULT;
}

static CastleResult CASTLE_RUNTIME_CALL file_enumerate_(
    CastlePluginHandle plugin, CastleStringView relative_pattern,
    CastleFileEnumerateCallbackFn callback, void* user_context,
    CastleU32* out_count) {
    WCHAR pattern[RUNTIME_PATH_WIDE_CAP];
    CastleU32 pattern_length = 0u;
    WIN32_FIND_DATAW data;
    HANDLE find;
    CastleU32 count = 0u;
    CastleResult result;
    if (!callback || !out_count) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_count = 0u;
    result = file_build_path_(plugin, relative_pattern, pattern, &pattern_length);
    (void)pattern_length;
    if (result < 0) return result;
    find = FindFirstFileW(pattern, &data);
    if (find == INVALID_HANDLE_VALUE) return CASTLE_ERROR_NOT_READY;
    do {
        char name_utf8[RUNTIME_PATH_UTF8_CAP];
        int name_length;
        CastleFileInfoV1 info;
        if ((data.cFileName[0] == L'.' && data.cFileName[1] == L'\0') ||
            (data.cFileName[0] == L'.' && data.cFileName[1] == L'.' &&
             data.cFileName[2] == L'\0')) continue;
        name_length = WideCharToMultiByte(CP_UTF8, 0u, data.cFileName, -1,
                                          name_utf8, RUNTIME_PATH_UTF8_CAP,
                                          NULL, NULL);
        if (name_length <= 1) {
            FindClose(find);
            return CASTLE_ERROR_RUNTIME_FAULT;
        }
        Runtime_ByteZero(&info, (CastleU32)sizeof(info));
        info.magic = CASTLE_FILE_INFO_MAGIC;
        info.struct_size = CASTLE_SIZEOF_FILE_INFO_V1;
        info.version = CASTLE_FILE_STRUCTURE_VERSION_1;
        info.file_type = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ?
            CASTLE_FILE_TYPE_DIRECTORY : CASTLE_FILE_TYPE_REGULAR;
        info.size_low = data.nFileSizeLow;
        info.size_high = data.nFileSizeHigh;
        info.name_utf8.data = name_utf8;
        info.name_utf8.length = (CastleU32)(name_length - 1);
        result = callback(&info, user_context);
        if (result < 0) {
            FindClose(find);
            return result;
        }
        ++count;
    } while (FindNextFileW(find, &data));
    FindClose(find);
    *out_count = count;
    return CASTLE_OK;
}
