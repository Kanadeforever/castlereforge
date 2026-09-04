#include "runtime_internal.h"

/*
 * 每个已登记插件最多拥有一个业务日志文件。句柄按 plugin_handle 查找，而不是按 ASI
 * 文件名查找，因此用户改名不会让两个已登记插件误共用同一条内存记录。
 */
typedef struct RuntimePluginLogRecord {
    int used;
    CastlePluginHandle plugin;
    HANDLE file;
    WCHAR path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 path_length;
} RuntimePluginLogRecord;

static volatile LONG g_log_lock;
static RuntimePluginLogRecord g_plugin_logs[RUNTIME_MAX_PLUGINS];
static HANDLE g_runtime_log = INVALID_HANDLE_VALUE;
static WCHAR g_log_directory_wide[RUNTIME_PATH_WIDE_CAP];
static CastleU32 g_log_directory_wide_length;
static char g_log_directory_utf8[RUNTIME_PATH_UTF8_CAP];
static CastleU32 g_log_directory_utf8_length;

static CastleResult CASTLE_RUNTIME_CALL log_write_plugin_(
    CastlePluginHandle plugin, const CastleLogRecordV1* record);
static CastleResult CASTLE_RUNTIME_CALL log_flush_plugin_(CastlePluginHandle plugin);
static CastleResult CASTLE_RUNTIME_CALL log_get_plugin_path_(
    CastlePluginHandle plugin, char* output, CastleU32 output_capacity,
    CastleU32* out_length);
static CastleResult CASTLE_RUNTIME_CALL log_get_directory_(
    char* output, CastleU32 output_capacity, CastleU32* out_length);
static CastleResult CASTLE_RUNTIME_CALL log_write_plugin_text_(
    CastlePluginHandle plugin, CastleStringView text);

static const CastleLogApiV1 g_log_api = {
    CASTLE_LOG_API_MAGIC,
    CASTLE_SIZEOF_LOG_API_V1,
    CASTLE_LOG_API_VERSION_1,
    CASTLE_LOG_CAP_SEPARATE_PLUGIN_FILES |
        CASTLE_LOG_CAP_UTF8_BOM |
        CASTLE_LOG_CAP_SYNCHRONOUS_FLUSH,
    log_write_plugin_,
    log_flush_plugin_,
    log_get_plugin_path_,
    log_get_directory_,
    log_write_plugin_text_
};

/* ASCII 文件夹名只需要不区分大小写比较；这里不受系统区域设置影响。 */
static WCHAR log_ascii_lower_(WCHAR value) {
    if (value >= L'A' && value <= L'Z') return (WCHAR)(value + (L'a' - L'A'));
    return value;
}

static int log_name_equals_ascii_(const WCHAR* text, CastleU32 length,
                                  const WCHAR* expected, CastleU32 expected_length) {
    CastleU32 index;
    if (!text || !expected || length != expected_length) return 0;
    for (index = 0u; index < length; ++index) {
        if (log_ascii_lower_(text[index]) != log_ascii_lower_(expected[index])) return 0;
    }
    return 1;
}

/* 返回最后一个路径分隔符之后的字符位置；结果就是文件名或目录名的起点。 */
static CastleU32 log_leaf_start_(const WCHAR* path, CastleU32 length) {
    CastleU32 position = length;
    while (position > 0u && path[position - 1u] != L'\\' &&
           path[position - 1u] != L'/') {
        --position;
    }
    return position;
}

/* 去掉路径最后一段；结果长度不包含末尾反斜杠。 */
static int log_parent_length_(const WCHAR* path, CastleU32 length,
                              CastleU32* out_length) {
    CastleU32 leaf;
    if (!path || !out_length || length == 0u) return 0;
    leaf = log_leaf_start_(path, length);
    if (leaf == 0u) return 0;
    *out_length = leaf - 1u;
    return 1;
}

static int log_append_wide_(WCHAR* path, CastleU32 capacity,
                            CastleU32* position, const WCHAR* suffix) {
    CastleU32 index = 0u;
    if (!path || !position || !suffix || *position >= capacity) return 0;
    while (suffix[index]) {
        if (*position + 1u >= capacity) return 0;
        path[(*position)++] = suffix[index++];
    }
    path[*position] = L'\0';
    return 1;
}

/*
 * 正式布局是 mods\asi\Castle_Runtime.dll，因此日志目录取 ASI 目录的同级 logs。
 * 测试宿主会把 DLL 放进 _build；这时日志留在 _build\logs，构建收尾删除 _build
 * 即可，绝不会污染 src\RuntimeSDK 或仓库根目录。
 */
static int log_build_directory_(void) {
    WCHAR runtime_path[RUNTIME_PATH_WIDE_CAP];
    DWORD runtime_length;
    CastleU32 asi_length;
    CastleU32 asi_leaf;
    CastleU32 base_length;
    CastleU32 position;
    int utf8_length;
    static const WCHAR asi_name[] = L"asi";

    runtime_length = GetModuleFileNameW(g_runtime_module, runtime_path,
                                        RUNTIME_PATH_WIDE_CAP);
    if (runtime_length == 0u || runtime_length >= RUNTIME_PATH_WIDE_CAP) return 0;
    runtime_path[runtime_length] = L'\0';
    if (!log_parent_length_(runtime_path, (CastleU32)runtime_length, &asi_length)) return 0;

    asi_leaf = log_leaf_start_(runtime_path, asi_length);
    base_length = asi_length;
    if (log_name_equals_ascii_(runtime_path + asi_leaf, asi_length - asi_leaf,
                               asi_name, 3u)) {
        if (!log_parent_length_(runtime_path, asi_length, &base_length)) return 0;
    }

    if (base_length + 6u >= RUNTIME_PATH_WIDE_CAP) return 0;
    Runtime_ByteCopy(g_log_directory_wide, runtime_path,
                     base_length * (CastleU32)sizeof(WCHAR));
    position = base_length;
    g_log_directory_wide[position] = L'\0';
    if (!log_append_wide_(g_log_directory_wide, RUNTIME_PATH_WIDE_CAP,
                          &position, L"\\logs")) return 0;
    g_log_directory_wide_length = position;

    if (!CreateDirectoryW(g_log_directory_wide, NULL) &&
        GetLastError() != ERROR_ALREADY_EXISTS) return 0;

    utf8_length = WideCharToMultiByte(CP_UTF8, 0u, g_log_directory_wide, -1,
                                      g_log_directory_utf8,
                                      RUNTIME_PATH_UTF8_CAP, NULL, NULL);
    if (utf8_length <= 1 || utf8_length > (int)RUNTIME_PATH_UTF8_CAP) return 0;
    g_log_directory_utf8_length = (CastleU32)(utf8_length - 1);
    return 1;
}

static int log_build_runtime_path_(WCHAR* output, CastleU32* out_length) {
    CastleU32 position;
    if (!output || !out_length || g_log_directory_wide_length == 0u) return 0;
    Runtime_ByteCopy(output, g_log_directory_wide,
                     g_log_directory_wide_length * (CastleU32)sizeof(WCHAR));
    position = g_log_directory_wide_length;
    output[position] = L'\0';
    if (!log_append_wide_(output, RUNTIME_PATH_WIDE_CAP, &position,
                          L"\\Castle_Runtime.log")) return 0;
    *out_length = position;
    return 1;
}

/*
 * 插件日志沿用插件文件名，只把扩展名替换成 .log。例如 Castle_Backlog.asi 对应
 * Castle_Backlog.log。名称只决定人看到的文件名，插件身份仍以 Registry 的 plugin_id 为准。
 */
static int log_build_plugin_path_(CastlePluginHandle plugin,
                                  WCHAR* output, CastleU32* out_length) {
    HMODULE module = Runtime_GetPluginModule(plugin);
    WCHAR module_path[RUNTIME_PATH_WIDE_CAP];
    DWORD module_length;
    CastleU32 leaf;
    CastleU32 dot;
    CastleU32 position;

    if (!module || !output || !out_length || g_log_directory_wide_length == 0u) return 0;
    module_length = GetModuleFileNameW(module, module_path, RUNTIME_PATH_WIDE_CAP);
    if (module_length == 0u || module_length >= RUNTIME_PATH_WIDE_CAP) return 0;
    module_path[module_length] = L'\0';
    leaf = log_leaf_start_(module_path, (CastleU32)module_length);
    if (leaf >= (CastleU32)module_length) return 0;

    dot = (CastleU32)module_length;
    while (dot > leaf && module_path[dot - 1u] != L'.') --dot;
    if (dot == leaf) dot = (CastleU32)module_length;
    else --dot;

    if (g_log_directory_wide_length + 1u + (dot - leaf) + 5u >=
        RUNTIME_PATH_WIDE_CAP) return 0;
    Runtime_ByteCopy(output, g_log_directory_wide,
                     g_log_directory_wide_length * (CastleU32)sizeof(WCHAR));
    position = g_log_directory_wide_length;
    output[position++] = L'\\';
    Runtime_ByteCopy(output + position, module_path + leaf,
                     (dot - leaf) * (CastleU32)sizeof(WCHAR));
    position += dot - leaf;
    output[position] = L'\0';
    if (!log_append_wide_(output, RUNTIME_PATH_WIDE_CAP, &position, L".log")) return 0;
    *out_length = position;
    return 1;
}

static HANDLE log_open_new_file_(const WCHAR* path) {
    HANDLE file;
    DWORD written = 0u;
    static const CastleU8 bom[3] = {0xEFu, 0xBBu, 0xBFu};
    if (!path) return INVALID_HANDLE_VALUE;
    file = CreateFileW(path, GENERIC_WRITE,
                       FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return INVALID_HANDLE_VALUE;
    if (!WriteFile(file, bom, 3u, &written, NULL) || written != 3u) {
        CloseHandle(file);
        return INVALID_HANDLE_VALUE;
    }
    FlushFileBuffers(file);
    return file;
}

static RuntimePluginLogRecord* log_find_record_(CastlePluginHandle plugin) {
    CastleU32 index;
    for (index = 0u; index < RUNTIME_MAX_PLUGINS; ++index) {
        if (g_plugin_logs[index].used && g_plugin_logs[index].plugin == plugin) {
            return &g_plugin_logs[index];
        }
    }
    return NULL;
}

static RuntimePluginLogRecord* log_get_or_open_record_(CastlePluginHandle plugin) {
    RuntimePluginLogRecord* record = log_find_record_(plugin);
    CastleU32 index;
    if (record) return record;
    if (!Runtime_GetPluginModule(plugin)) return NULL;

    for (index = 0u; index < RUNTIME_MAX_PLUGINS; ++index) {
        if (!g_plugin_logs[index].used) {
            CastleU32 length = 0u;
            if (!log_build_plugin_path_(plugin, g_plugin_logs[index].path, &length)) return NULL;
            g_plugin_logs[index].file = log_open_new_file_(g_plugin_logs[index].path);
            if (g_plugin_logs[index].file == INVALID_HANDLE_VALUE) return NULL;
            g_plugin_logs[index].path_length = length;
            g_plugin_logs[index].plugin = plugin;
            g_plugin_logs[index].used = 1;
            return &g_plugin_logs[index];
        }
    }
    return NULL;
}

static CastleResult log_write_line_handle_(HANDLE file, CastleStringView message) {
    DWORD written = 0u;
    static const char newline[2] = {'\r', '\n'};
    if (file == INVALID_HANDLE_VALUE || !message.data || message.length == 0u) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!WriteFile(file, message.data, message.length, &written, NULL) ||
        written != message.length) return CASTLE_ERROR_RUNTIME_FAULT;
    written = 0u;
    if (!WriteFile(file, newline, 2u, &written, NULL) || written != 2u) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    if (!FlushFileBuffers(file)) return CASTLE_ERROR_RUNTIME_FAULT;
    return CASTLE_OK;
}

static CastleResult log_write_text_handle_(HANDLE file, CastleStringView text) {
    DWORD written = 0u;
    if (file == INVALID_HANDLE_VALUE || !text.data || text.length == 0u) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!WriteFile(file, text.data, text.length, &written, NULL) ||
        written != text.length || !FlushFileBuffers(file)) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    return CASTLE_OK;
}

int Runtime_LogInitialize(void) {
    WCHAR runtime_path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 runtime_path_length = 0u;
    g_log_lock = 0;
    Runtime_ByteZero(g_plugin_logs, (CastleU32)sizeof(g_plugin_logs));
    g_runtime_log = INVALID_HANDLE_VALUE;
    g_log_directory_wide_length = 0u;
    g_log_directory_utf8_length = 0u;
    if (!log_build_directory_()) return 0;
    if (!log_build_runtime_path_(runtime_path, &runtime_path_length)) return 0;
    (void)runtime_path_length;
    g_runtime_log = log_open_new_file_(runtime_path);
    return g_runtime_log != INVALID_HANDLE_VALUE;
}

const CastleLogApiV1* Runtime_GetLogApiV1(void) {
    return &g_log_api;
}

/* 内存诊断环调用本函数。磁盘不可写时静默保留内存诊断，不能递归写错误日志。 */
void Runtime_LogRuntimeLine(const char* utf8_line) {
    CastleStringView view;
    if (!utf8_line || g_runtime_log == INVALID_HANDLE_VALUE) return;
    view.data = utf8_line;
    view.length = Runtime_StringLength(utf8_line);
    if (view.length == 0u) return;
    Runtime_Lock(&g_log_lock);
    (void)log_write_line_handle_(g_runtime_log, view);
    Runtime_Unlock(&g_log_lock);
}

static CastleResult CASTLE_RUNTIME_CALL log_write_plugin_(
    CastlePluginHandle plugin, const CastleLogRecordV1* record) {
    RuntimePluginLogRecord* log_record;
    CastleResult result;
    if (!record || record->magic != CASTLE_LOG_RECORD_MAGIC ||
        record->struct_size < CASTLE_SIZEOF_LOG_RECORD_V1 ||
        record->version != CASTLE_LOG_STRUCTURE_VERSION_1 ||
        record->level > CASTLE_LOG_FATAL || !record->message.data ||
        record->message.length == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;

    Runtime_Lock(&g_log_lock);
    log_record = log_get_or_open_record_(plugin);
    result = log_record ? log_write_line_handle_(log_record->file, record->message) :
                          CASTLE_ERROR_RUNTIME_FAULT;
    Runtime_Unlock(&g_log_lock);
    return result;
}

static CastleResult CASTLE_RUNTIME_CALL log_flush_plugin_(CastlePluginHandle plugin) {
    RuntimePluginLogRecord* record;
    CastleResult result;
    Runtime_Lock(&g_log_lock);
    record = log_find_record_(plugin);
    if (!record) result = Runtime_GetPluginModule(plugin) ? CASTLE_STATUS_ALREADY_DONE :
                                                            CASTLE_ERROR_INVALID_ARGUMENT;
    else result = FlushFileBuffers(record->file) ? CASTLE_OK : CASTLE_ERROR_RUNTIME_FAULT;
    Runtime_Unlock(&g_log_lock);
    return result;
}

static CastleResult log_copy_path_utf8_(const WCHAR* path, CastleU32 path_length,
                                        char* output, CastleU32 output_capacity,
                                        CastleU32* out_length) {
    int required;
    int written;
    if (!path || path_length == 0u || !out_length) return CASTLE_ERROR_INVALID_ARGUMENT;
    required = WideCharToMultiByte(CP_UTF8, 0u, path, (int)path_length,
                                   NULL, 0, NULL, NULL);
    if (required <= 0) return CASTLE_ERROR_RUNTIME_FAULT;
    *out_length = (CastleU32)required;
    if (!output || output_capacity <= (CastleU32)required) {
        return CASTLE_ERROR_BUFFER_TOO_SMALL;
    }
    written = WideCharToMultiByte(CP_UTF8, 0u, path, (int)path_length,
                                  output, required, NULL, NULL);
    if (written != required) return CASTLE_ERROR_RUNTIME_FAULT;
    output[required] = '\0';
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL log_get_plugin_path_(
    CastlePluginHandle plugin, char* output, CastleU32 output_capacity,
    CastleU32* out_length) {
    WCHAR path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 length = 0u;
    if (!Runtime_GetPluginModule(plugin)) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (!log_build_plugin_path_(plugin, path, &length)) return CASTLE_ERROR_RUNTIME_FAULT;
    return log_copy_path_utf8_(path, length, output, output_capacity, out_length);
}

static CastleResult CASTLE_RUNTIME_CALL log_get_directory_(
    char* output, CastleU32 output_capacity, CastleU32* out_length) {
    if (!out_length) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_length = g_log_directory_utf8_length;
    if (!output || output_capacity <= g_log_directory_utf8_length) {
        return CASTLE_ERROR_BUFFER_TOO_SMALL;
    }
    Runtime_ByteCopy(output, g_log_directory_utf8, g_log_directory_utf8_length);
    output[g_log_directory_utf8_length] = '\0';
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL log_write_plugin_text_(
    CastlePluginHandle plugin, CastleStringView text) {
    RuntimePluginLogRecord* record;
    CastleResult result;
    if (!text.data || text.length == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_log_lock);
    record = log_get_or_open_record_(plugin);
    result = record ? log_write_text_handle_(record->file, text) :
                      CASTLE_ERROR_RUNTIME_FAULT;
    Runtime_Unlock(&g_log_lock);
    return result;
}
