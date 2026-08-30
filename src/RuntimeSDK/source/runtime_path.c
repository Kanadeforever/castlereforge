#include "runtime_internal.h"

/*
 * Runtime 自己的完整路径和 ASI 根目录在初始化时只计算一次。
 * 后续 Path API 返回的只读视图始终指向这些静态数组，因此插件可以安全缓存视图，
 * 但绝不能修改数组内容或在进程结束后继续使用指针。
 */
static char g_runtime_path_utf8[RUNTIME_PATH_UTF8_CAP];
static CastleU32 g_runtime_path_length;
static WCHAR g_runtime_path_wide[RUNTIME_PATH_WIDE_CAP];
static CastleU32 g_runtime_path_wide_length;
static WCHAR g_asi_directory_wide[RUNTIME_PATH_WIDE_CAP];
static CastleU32 g_asi_directory_wide_length;
static char g_asi_directory_utf8[RUNTIME_PATH_UTF8_CAP];
static CastleU32 g_asi_directory_utf8_length;
static char g_game_build_id[RUNTIME_GAME_BUILD_CAP];
static CastleU32 g_game_build_length;
static HMODULE g_game_module;

static CastleResult CASTLE_RUNTIME_CALL path_get_info_(CastlePathInfoV1* out_info);
static CastleResult CASTLE_RUNTIME_CALL path_get_plugin_utf8_(CastlePluginHandle plugin,
    char* output, CastleU32 output_capacity, CastleU32* out_length);
static CastleResult CASTLE_RUNTIME_CALL path_get_plugin_wide_(CastlePluginHandle plugin,
    CastleU16* output, CastleU32 output_capacity, CastleU32* out_length);
static CastleResult CASTLE_RUNTIME_CALL path_build_utf8_(CastlePluginHandle plugin,
    CastleStringView relative_path, char* output, CastleU32 output_capacity,
    CastleU32* out_length);
static CastleResult CASTLE_RUNTIME_CALL path_build_wide_(CastlePluginHandle plugin,
    CastleWideStringView relative_path, CastleU16* output, CastleU32 output_capacity,
    CastleU32* out_length);
static CastleResult CASTLE_RUNTIME_CALL path_normalize_utf8_(CastleStringView absolute_path,
    char* output, CastleU32 output_capacity, CastleU32* out_length);
static CastleResult CASTLE_RUNTIME_CALL path_normalize_wide_(
    CastleWideStringView absolute_path, CastleU16* output,
    CastleU32 output_capacity, CastleU32* out_length);
static CastleResult CASTLE_RUNTIME_CALL path_equals_utf8_(CastleStringView left,
    CastleStringView right, CastleU32* out_equal);
static CastleResult CASTLE_RUNTIME_CALL path_equals_wide_(CastleWideStringView left,
    CastleWideStringView right, CastleU32* out_equal);

/* Path v1 是 Runtime 持有的稳定门面；函数表地址在进程存续期内不变化。 */
static const CastlePathApiV1 g_path_api = {
    CASTLE_PATH_API_MAGIC,
    CASTLE_SIZEOF_PATH_API_V1,
    CASTLE_PATH_API_VERSION_1,
    0u,
    path_get_info_,
    path_get_plugin_utf8_,
    path_get_plugin_wide_,
    path_build_utf8_,
    path_build_wide_,
    path_normalize_utf8_,
    path_normalize_wide_,
    path_equals_utf8_,
    path_equals_wide_
};

static char runtime_hex_digit_(CastleU32 value) {
    value &= 0xFu;
    return value < 10u ? (char)('0' + value) : (char)('A' + (value - 10u));
}

static void runtime_append_text_(char* output, CastleU32 capacity,
                                 CastleU32* position, const char* text) {
    CastleU32 index = 0u;
    if (!output || !position || !text || capacity == 0u) return;

    while (text[index] && *position + 1u < capacity) {
        output[*position] = text[index];
        ++*position;
        ++index;
    }
    output[*position] = '\0';
}

static void runtime_append_hex32_(char* output, CastleU32 capacity,
                                  CastleU32* position, CastleU32 value) {
    CastleS32 shift;
    if (!output || !position || capacity == 0u) return;

    for (shift = 28; shift >= 0; shift -= 4) {
        if (*position + 1u >= capacity) break;
        output[*position] = runtime_hex_digit_(value >> (CastleU32)shift);
        ++*position;
    }
    output[*position] = '\0';
}

static void runtime_build_game_id_(void) {
    IMAGE_DOS_HEADER* dos_header;
    IMAGE_NT_HEADERS32* nt_headers;
    CastleU32 position = 0u;

    g_game_build_id[0] = '\0';
    g_game_build_length = 0u;
    if (!g_game_module) return;

    dos_header = (IMAGE_DOS_HEADER*)g_game_module;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) return;

    nt_headers = (IMAGE_NT_HEADERS32*)((CastleU8*)g_game_module + dos_header->e_lfanew);
    if (nt_headers->Signature != IMAGE_NT_SIGNATURE ||
        nt_headers->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) return;

    /*
     * 先使用 PE 时间戳和 SizeOfImage 形成可诊断的构建 ID。
     * 正式游戏符号 profile 会再增加节哈希；这里不能把测试宿主误认成目标游戏。
     */
    runtime_append_text_(g_game_build_id, RUNTIME_GAME_BUILD_CAP, &position, "pe32-");
    runtime_append_hex32_(g_game_build_id, RUNTIME_GAME_BUILD_CAP, &position,
                          nt_headers->FileHeader.TimeDateStamp);
    runtime_append_text_(g_game_build_id, RUNTIME_GAME_BUILD_CAP, &position, "-");
    runtime_append_hex32_(g_game_build_id, RUNTIME_GAME_BUILD_CAP, &position,
                          nt_headers->OptionalHeader.SizeOfImage);
    g_game_build_length = position;
}

/* 把宽字符串复制给调用方；容量以 CastleU16 字符数计算，不以字节数计算。 */
static CastleResult path_copy_wide_(const WCHAR* source, CastleU32 source_length,
                                    CastleU16* output, CastleU32 output_capacity,
                                    CastleU32* out_length) {
    CastleU32 index;
    if (!source || !out_length) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_length = source_length;
    if (!output || output_capacity <= source_length) return CASTLE_ERROR_BUFFER_TOO_SMALL;
    for (index = 0u; index < source_length; ++index) {
        output[index] = (CastleU16)source[index];
    }
    output[source_length] = 0u;
    return CASTLE_OK;
}

/* 把已验证的宽字符串转换成 UTF-8；容量和长度都按字节计数。 */
static CastleResult path_copy_utf8_(const WCHAR* source, CastleU32 source_length,
                                    char* output, CastleU32 output_capacity,
                                    CastleU32* out_length) {
    int required;
    int written;
    if (!source || source_length == 0u || !out_length) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    required = WideCharToMultiByte(CP_UTF8, 0u, source, (int)source_length,
                                   NULL, 0, NULL, NULL);
    if (required <= 0) return CASTLE_ERROR_RUNTIME_FAULT;
    *out_length = (CastleU32)required;
    if (!output || output_capacity <= (CastleU32)required) {
        return CASTLE_ERROR_BUFFER_TOO_SMALL;
    }
    written = WideCharToMultiByte(CP_UTF8, 0u, source, (int)source_length,
                                  output, required, NULL, NULL);
    if (written != required) return CASTLE_ERROR_RUNTIME_FAULT;
    output[required] = '\0';
    return CASTLE_OK;
}

/*
 * CastleStringView 不承诺带 NUL，所以必须按显式长度转换。
 * 输入中若夹带 NUL，会让 Windows API 看见一个比 ABI 声明更短的路径，必须拒绝。
 */
static CastleResult path_utf8_to_wide_(CastleStringView input, WCHAR* output,
                                       CastleU32 output_capacity,
                                       CastleU32* out_length) {
    int converted;
    CastleU32 index;
    if (!input.data || input.length == 0u || !output || output_capacity == 0u ||
        !out_length || input.length >= output_capacity) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    converted = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        input.data, (int)input.length, output, (int)(output_capacity - 1u));
    if (converted <= 0) return CASTLE_ERROR_INVALID_ARGUMENT;
    for (index = 0u; index < (CastleU32)converted; ++index) {
        if (output[index] == L'\0') return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    output[converted] = L'\0';
    *out_length = (CastleU32)converted;
    return CASTLE_OK;
}

/* 宽视图同样复制到私有数组，确保后续 Windows API 永远获得可靠的 NUL 结尾。 */
static CastleResult path_wide_view_copy_(CastleWideStringView input, WCHAR* output,
                                         CastleU32 output_capacity,
                                         CastleU32* out_length) {
    CastleU32 index;
    if (!input.data || input.length == 0u || !output || !out_length ||
        input.length >= output_capacity) return CASTLE_ERROR_INVALID_ARGUMENT;
    for (index = 0u; index < input.length; ++index) {
        if (input.data[index] == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
        output[index] = (WCHAR)input.data[index];
    }
    output[input.length] = L'\0';
    *out_length = input.length;
    return CASTLE_OK;
}

/* 找出一个完整路径的父目录长度；返回值不包含末尾斜杠。 */
static int path_parent_length_(const WCHAR* path, CastleU32 path_length,
                               CastleU32* out_parent_length) {
    CastleU32 position = path_length;
    if (!path || path_length == 0u || !out_parent_length) return 0;
    while (position > 0u && path[position - 1u] != L'\\' &&
           path[position - 1u] != L'/') --position;
    if (position == 0u) return 0;
    *out_parent_length = position - 1u;
    return 1;
}

/* 只有真正与 Castle_Runtime.dll 同目录的已登记模块才可使用插件相对路径。 */
static CastleResult path_get_plugin_module_(CastlePluginHandle plugin,
                                            WCHAR* output,
                                            CastleU32* out_length) {
    HMODULE module = Runtime_GetPluginModule(plugin);
    DWORD length;
    CastleU32 parent_length;
    if (!module || !output || !out_length) return CASTLE_ERROR_INVALID_ARGUMENT;
    length = GetModuleFileNameW(module, output, RUNTIME_PATH_WIDE_CAP);
    if (length == 0u || length >= RUNTIME_PATH_WIDE_CAP) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    output[length] = L'\0';
    if (!path_parent_length_(output, (CastleU32)length, &parent_length) ||
        parent_length != g_asi_directory_wide_length ||
        CompareStringOrdinal(output, (int)parent_length, g_asi_directory_wide,
                             (int)g_asi_directory_wide_length, TRUE) != CSTR_EQUAL) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    *out_length = (CastleU32)length;
    return CASTLE_OK;
}

/*
 * 相对路径不能从根开始、不能带盘符/流名称，也不能含有独立的 .. 段。
 * 单个点和重复分隔符允许输入，稍后会由 GetFullPathNameW 统一清理。
 */
static int path_relative_safe_(const WCHAR* relative, CastleU32 length) {
    CastleU32 index;
    CastleU32 segment_start = 0u;
    if (!relative || length == 0u || relative[0] == L'\\' || relative[0] == L'/' ||
        (length >= 2u && relative[1] == L':')) return 0;

    for (index = 0u; index <= length; ++index) {
        WCHAR value = index < length ? relative[index] : L'\\';
        if (value == L'\0' || value == L':') return 0;
        if (value == L'\\' || value == L'/') {
            CastleU32 segment_length = index - segment_start;
            if (segment_length == 2u && relative[segment_start] == L'.' &&
                relative[segment_start + 1u] == L'.') return 0;
            segment_start = index + 1u;
        }
    }
    return 1;
}

/* 检查规范化结果仍位于 ASI 根目录内，避免相似前缀目录蒙混通过。 */
static int path_under_asi_root_(const WCHAR* path, CastleU32 length) {
    if (!path || length < g_asi_directory_wide_length) return 0;
    if (CompareStringOrdinal(path, (int)g_asi_directory_wide_length,
        g_asi_directory_wide, (int)g_asi_directory_wide_length, TRUE) != CSTR_EQUAL) {
        return 0;
    }
    if (length == g_asi_directory_wide_length) return 1;
    return path[g_asi_directory_wide_length] == L'\\' ||
           path[g_asi_directory_wide_length] == L'/';
}

/*
 * 规范化绝对宽路径，不读取或修改进程 CurrentDirectory。
 * 尾部多余斜杠统一去掉，使 PathsEqual 对常见目录写法给出稳定结果。
 */
static CastleResult path_normalize_wide_core_(const WCHAR* source,
                                              CastleU32 source_length,
                                              WCHAR* output,
                                              CastleU32* out_length) {
    DWORD normalized_length;
    CastleU32 index;
    if (!source || source_length == 0u || !output || !out_length) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!((source_length >= 3u && source[1] == L':' &&
           (source[2] == L'\\' || source[2] == L'/')) ||
          (source_length >= 2u && source[0] == L'\\' && source[1] == L'\\'))) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    normalized_length = GetFullPathNameW(source, RUNTIME_PATH_WIDE_CAP, output, NULL);
    if (normalized_length == 0u) return CASTLE_ERROR_RUNTIME_FAULT;
    if (normalized_length >= RUNTIME_PATH_WIDE_CAP) return CASTLE_ERROR_BUFFER_TOO_SMALL;
    for (index = 0u; index < (CastleU32)normalized_length; ++index) {
        if (output[index] == L'/') output[index] = L'\\';
    }
    while (normalized_length > 3u && output[normalized_length - 1u] == L'\\') {
        --normalized_length;
    }
    output[normalized_length] = L'\0';
    *out_length = (CastleU32)normalized_length;
    return CASTLE_OK;
}

/* 把已经验证的相对宽路径接到 ASI 根目录，并再次检查规范化后的根边界。 */
static CastleResult path_build_wide_core_(CastlePluginHandle plugin,
                                          const WCHAR* relative,
                                          CastleU32 relative_length,
                                          WCHAR* output,
                                          CastleU32* out_length) {
    WCHAR plugin_path[RUNTIME_PATH_WIDE_CAP];
    WCHAR combined[RUNTIME_PATH_WIDE_CAP];
    CastleU32 plugin_path_length = 0u;
    CastleU32 position;
    CastleU32 index;
    CastleResult result;

    if (!relative || !output || !out_length ||
        !path_relative_safe_(relative, relative_length)) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    result = path_get_plugin_module_(plugin, plugin_path, &plugin_path_length);
    (void)plugin_path_length;
    if (result < 0) return result;
    if (g_asi_directory_wide_length + 1u + relative_length + 1u >
        RUNTIME_PATH_WIDE_CAP) return CASTLE_ERROR_BUFFER_TOO_SMALL;

    position = 0u;
    for (index = 0u; index < g_asi_directory_wide_length; ++index) {
        combined[position++] = g_asi_directory_wide[index];
    }
    combined[position++] = L'\\';
    for (index = 0u; index < relative_length; ++index) {
        combined[position++] = relative[index] == L'/' ? L'\\' : relative[index];
    }
    combined[position] = L'\0';

    result = path_normalize_wide_core_(combined, position, output, out_length);
    if (result < 0) return result;
    if (!path_under_asi_root_(output, *out_length)) return CASTLE_ERROR_INVALID_ARGUMENT;
    return CASTLE_OK;
}

int Runtime_PathInitialize(void) {
    DWORD wide_length;
    int utf8_bytes;
    CastleU32 directory_length;

    if (!g_runtime_module) return 0;

    wide_length = GetModuleFileNameW(g_runtime_module, g_runtime_path_wide,
                                     RUNTIME_PATH_WIDE_CAP);
    if (wide_length == 0u || wide_length >= RUNTIME_PATH_WIDE_CAP) return 0;
    g_runtime_path_wide[wide_length] = L'\0';
    g_runtime_path_wide_length = (CastleU32)wide_length;

    utf8_bytes = WideCharToMultiByte(CP_UTF8, 0u, g_runtime_path_wide, -1,
                                     g_runtime_path_utf8, RUNTIME_PATH_UTF8_CAP,
                                     NULL, NULL);
    if (utf8_bytes <= 1 || utf8_bytes > (int)RUNTIME_PATH_UTF8_CAP) return 0;
    g_runtime_path_length = (CastleU32)(utf8_bytes - 1);

    if (!path_parent_length_(g_runtime_path_wide, g_runtime_path_wide_length,
                             &directory_length)) return 0;
    g_asi_directory_wide_length = directory_length;
    Runtime_ByteCopy(g_asi_directory_wide, g_runtime_path_wide,
        g_asi_directory_wide_length * (CastleU32)sizeof(WCHAR));
    g_asi_directory_wide[g_asi_directory_wide_length] = L'\0';

    utf8_bytes = WideCharToMultiByte(CP_UTF8, 0u, g_asi_directory_wide, -1,
                                     g_asi_directory_utf8, RUNTIME_PATH_UTF8_CAP,
                                     NULL, NULL);
    if (utf8_bytes <= 1 || utf8_bytes > (int)RUNTIME_PATH_UTF8_CAP) return 0;
    g_asi_directory_utf8_length = (CastleU32)(utf8_bytes - 1);

    g_game_module = GetModuleHandleW(NULL);
    if (!g_game_module) return 0;
    runtime_build_game_id_();
    if (g_game_build_length == 0u) return 0;
    return 1;
}

CastleStringView Runtime_GetPathView(void) {
    CastleStringView view;
    view.data = g_runtime_path_utf8;
    view.length = g_runtime_path_length;
    return view;
}

CastleStringView Runtime_GetGameBuildView(void) {
    CastleStringView view;
    view.data = g_game_build_id;
    view.length = g_game_build_length;
    return view;
}

CastleModule Runtime_GetGameModuleValue(void) {
    return (CastleModule)(ULONG_PTR)g_game_module;
}

const CastlePathApiV1* Runtime_GetPathApiV1(void) {
    return &g_path_api;
}

static CastleResult CASTLE_RUNTIME_CALL path_get_info_(CastlePathInfoV1* out_info) {
    if (!out_info || out_info->magic != CASTLE_PATH_INFO_MAGIC ||
        out_info->struct_size < CASTLE_SIZEOF_PATH_INFO_V1 ||
        out_info->version != CASTLE_PATH_VERSION_1) return CASTLE_ERROR_INVALID_ARGUMENT;
    out_info->flags = 0u;
    out_info->generation = 1u;
    out_info->asi_directory_utf8.data = g_asi_directory_utf8;
    out_info->asi_directory_utf8.length = g_asi_directory_utf8_length;
    out_info->asi_directory_wide.data = (const CastleU16*)g_asi_directory_wide;
    out_info->asi_directory_wide.length = g_asi_directory_wide_length;
    out_info->runtime_path_utf8.data = g_runtime_path_utf8;
    out_info->runtime_path_utf8.length = g_runtime_path_length;
    out_info->runtime_path_wide.data = (const CastleU16*)g_runtime_path_wide;
    out_info->runtime_path_wide.length = g_runtime_path_wide_length;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL path_get_plugin_utf8_(CastlePluginHandle plugin,
    char* output, CastleU32 output_capacity, CastleU32* out_length) {
    WCHAR path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 length = 0u;
    CastleResult result = path_get_plugin_module_(plugin, path, &length);
    if (result < 0) return result;
    return path_copy_utf8_(path, length, output, output_capacity, out_length);
}

static CastleResult CASTLE_RUNTIME_CALL path_get_plugin_wide_(CastlePluginHandle plugin,
    CastleU16* output, CastleU32 output_capacity, CastleU32* out_length) {
    WCHAR path[RUNTIME_PATH_WIDE_CAP];
    CastleU32 length = 0u;
    CastleResult result = path_get_plugin_module_(plugin, path, &length);
    if (result < 0) return result;
    return path_copy_wide_(path, length, output, output_capacity, out_length);
}

static CastleResult CASTLE_RUNTIME_CALL path_build_utf8_(CastlePluginHandle plugin,
    CastleStringView relative_path, char* output, CastleU32 output_capacity,
    CastleU32* out_length) {
    WCHAR relative_wide[RUNTIME_PATH_WIDE_CAP];
    WCHAR complete_wide[RUNTIME_PATH_WIDE_CAP];
    CastleU32 relative_length = 0u;
    CastleU32 complete_length = 0u;
    CastleResult result;
    if (!out_length) return CASTLE_ERROR_INVALID_ARGUMENT;
    result = path_utf8_to_wide_(relative_path, relative_wide,
                                RUNTIME_PATH_WIDE_CAP, &relative_length);
    if (result < 0) return result;
    result = path_build_wide_core_(plugin, relative_wide, relative_length,
                                   complete_wide, &complete_length);
    if (result < 0) return result;
    return path_copy_utf8_(complete_wide, complete_length, output,
                           output_capacity, out_length);
}

static CastleResult CASTLE_RUNTIME_CALL path_build_wide_(CastlePluginHandle plugin,
    CastleWideStringView relative_path, CastleU16* output, CastleU32 output_capacity,
    CastleU32* out_length) {
    WCHAR relative_wide[RUNTIME_PATH_WIDE_CAP];
    WCHAR complete_wide[RUNTIME_PATH_WIDE_CAP];
    CastleU32 relative_length = 0u;
    CastleU32 complete_length = 0u;
    CastleResult result;
    if (!out_length) return CASTLE_ERROR_INVALID_ARGUMENT;
    result = path_wide_view_copy_(relative_path, relative_wide,
                                  RUNTIME_PATH_WIDE_CAP, &relative_length);
    if (result < 0) return result;
    result = path_build_wide_core_(plugin, relative_wide, relative_length,
                                   complete_wide, &complete_length);
    if (result < 0) return result;
    return path_copy_wide_(complete_wide, complete_length, output,
                           output_capacity, out_length);
}

static CastleResult CASTLE_RUNTIME_CALL path_normalize_utf8_(CastleStringView input,
    char* output, CastleU32 output_capacity, CastleU32* out_length) {
    WCHAR source[RUNTIME_PATH_WIDE_CAP];
    WCHAR normalized[RUNTIME_PATH_WIDE_CAP];
    CastleU32 source_length = 0u;
    CastleU32 normalized_length = 0u;
    CastleResult result;
    if (!out_length) return CASTLE_ERROR_INVALID_ARGUMENT;
    result = path_utf8_to_wide_(input, source, RUNTIME_PATH_WIDE_CAP, &source_length);
    if (result < 0) return result;
    result = path_normalize_wide_core_(source, source_length,
                                       normalized, &normalized_length);
    if (result < 0) return result;
    return path_copy_utf8_(normalized, normalized_length, output,
                           output_capacity, out_length);
}

static CastleResult CASTLE_RUNTIME_CALL path_normalize_wide_(
    CastleWideStringView input, CastleU16* output,
    CastleU32 output_capacity, CastleU32* out_length) {
    WCHAR source[RUNTIME_PATH_WIDE_CAP];
    WCHAR normalized[RUNTIME_PATH_WIDE_CAP];
    CastleU32 source_length = 0u;
    CastleU32 normalized_length = 0u;
    CastleResult result;
    if (!out_length) return CASTLE_ERROR_INVALID_ARGUMENT;
    result = path_wide_view_copy_(input, source, RUNTIME_PATH_WIDE_CAP, &source_length);
    if (result < 0) return result;
    result = path_normalize_wide_core_(source, source_length,
                                       normalized, &normalized_length);
    if (result < 0) return result;
    return path_copy_wide_(normalized, normalized_length, output,
                           output_capacity, out_length);
}

static CastleResult CASTLE_RUNTIME_CALL path_equals_utf8_(CastleStringView left,
    CastleStringView right, CastleU32* out_equal) {
    WCHAR left_source[RUNTIME_PATH_WIDE_CAP];
    WCHAR right_source[RUNTIME_PATH_WIDE_CAP];
    WCHAR left_normalized[RUNTIME_PATH_WIDE_CAP];
    WCHAR right_normalized[RUNTIME_PATH_WIDE_CAP];
    CastleU32 left_source_length = 0u;
    CastleU32 right_source_length = 0u;
    CastleU32 left_length = 0u;
    CastleU32 right_length = 0u;
    CastleResult result;
    if (!out_equal) return CASTLE_ERROR_INVALID_ARGUMENT;
    result = path_utf8_to_wide_(left, left_source, RUNTIME_PATH_WIDE_CAP,
                                &left_source_length);
    if (result < 0) return result;
    result = path_utf8_to_wide_(right, right_source, RUNTIME_PATH_WIDE_CAP,
                                &right_source_length);
    if (result < 0) return result;
    result = path_normalize_wide_core_(left_source, left_source_length,
                                       left_normalized, &left_length);
    if (result < 0) return result;
    result = path_normalize_wide_core_(right_source, right_source_length,
                                       right_normalized, &right_length);
    if (result < 0) return result;
    *out_equal = left_length == right_length &&
        CompareStringOrdinal(left_normalized, (int)left_length, right_normalized,
                             (int)right_length, TRUE) == CSTR_EQUAL ? 1u : 0u;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL path_equals_wide_(CastleWideStringView left,
    CastleWideStringView right, CastleU32* out_equal) {
    WCHAR left_source[RUNTIME_PATH_WIDE_CAP];
    WCHAR right_source[RUNTIME_PATH_WIDE_CAP];
    WCHAR left_normalized[RUNTIME_PATH_WIDE_CAP];
    WCHAR right_normalized[RUNTIME_PATH_WIDE_CAP];
    CastleU32 left_source_length = 0u;
    CastleU32 right_source_length = 0u;
    CastleU32 left_length = 0u;
    CastleU32 right_length = 0u;
    CastleResult result;
    if (!out_equal) return CASTLE_ERROR_INVALID_ARGUMENT;
    result = path_wide_view_copy_(left, left_source, RUNTIME_PATH_WIDE_CAP,
                                  &left_source_length);
    if (result < 0) return result;
    result = path_wide_view_copy_(right, right_source, RUNTIME_PATH_WIDE_CAP,
                                  &right_source_length);
    if (result < 0) return result;
    result = path_normalize_wide_core_(left_source, left_source_length,
                                       left_normalized, &left_length);
    if (result < 0) return result;
    result = path_normalize_wide_core_(right_source, right_source_length,
                                       right_normalized, &right_length);
    if (result < 0) return result;
    *out_equal = left_length == right_length &&
        CompareStringOrdinal(left_normalized, (int)left_length, right_normalized,
                             (int)right_length, TRUE) == CSTR_EQUAL ? 1u : 0u;
    return CASTLE_OK;
}
