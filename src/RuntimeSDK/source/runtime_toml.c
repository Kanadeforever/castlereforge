#include "runtime_internal.h"

#define RUNTIME_TOML_MAX_DOCUMENTS 16u
#define RUNTIME_TOML_DOCUMENT_CAP 65536u
#define RUNTIME_TOML_NAME_CAP 256u

typedef struct RuntimeTomlDocument {
    int used;
    CastleTomlDocumentHandle handle;
    CastlePluginHandle plugin;
    CastleU32 size;
    CastleU8 bytes[RUNTIME_TOML_DOCUMENT_CAP];
} RuntimeTomlDocument;

typedef struct RuntimeTomlSlice {
    const char* data;
    CastleU32 length;
} RuntimeTomlSlice;

static volatile LONG g_toml_lock;
static RuntimeTomlDocument g_toml_documents[RUNTIME_TOML_MAX_DOCUMENTS];
static CastleU32 g_toml_generation;

static CastleResult CASTLE_RUNTIME_CALL toml_open_(
    CastlePluginHandle plugin, CastleStringView relative_path,
    CastleTomlDocumentHandle* out_document);
static CastleResult CASTLE_RUNTIME_CALL toml_close_(
    CastleTomlDocumentHandle document);
static CastleResult CASTLE_RUNTIME_CALL toml_get_bool_(
    CastleTomlDocumentHandle document, CastleStringView table,
    CastleStringView key, CastleU32 default_value, CastleU32* out_value);
static CastleResult CASTLE_RUNTIME_CALL toml_get_s32_(
    CastleTomlDocumentHandle document, CastleStringView table,
    CastleStringView key, CastleS32 default_value, CastleS32 minimum_value,
    CastleS32 maximum_value, CastleS32* out_value);
static CastleResult CASTLE_RUNTIME_CALL toml_get_string_(
    CastleTomlDocumentHandle document, CastleStringView table,
    CastleStringView key, CastleStringView default_value,
    char* output, CastleU32 output_capacity, CastleU32* out_length);

static const CastleTomlApiV1 g_toml_api = {
    CASTLE_TOML_API_MAGIC,
    CASTLE_SIZEOF_TOML_API_V1,
    CASTLE_TOML_API_VERSION_1,
    CASTLE_TOML_CAP_BOOL | CASTLE_TOML_CAP_S32 | CASTLE_TOML_CAP_STRING,
    toml_open_,
    toml_close_,
    toml_get_bool_,
    toml_get_s32_,
    toml_get_string_
};

static int toml_space_(char value) {
    return value == ' ' || value == '\t' || value == '\r';
}

static RuntimeTomlSlice toml_trim_(RuntimeTomlSlice value) {
    while (value.length && toml_space_(value.data[0])) {
        ++value.data;
        --value.length;
    }
    while (value.length && toml_space_(value.data[value.length - 1u])) {
        --value.length;
    }
    return value;
}

static int toml_view_equal_(RuntimeTomlSlice left, CastleStringView right) {
    CastleU32 index;
    if (!right.data || left.length != right.length) return 0;
    for (index = 0u; index < left.length; ++index) {
        if ((CastleU8)left.data[index] != (CastleU8)right.data[index]) return 0;
    }
    return 1;
}

static RuntimeTomlDocument* toml_find_locked_(CastleTomlDocumentHandle handle) {
    CastleU32 index;
    for (index = 0u; index < RUNTIME_TOML_MAX_DOCUMENTS; ++index) {
        if (g_toml_documents[index].used &&
            g_toml_documents[index].handle == handle) return &g_toml_documents[index];
    }
    return NULL;
}

/*
 * 找到注释起点。# 只有在基本字符串之外才开始注释；反斜杠只在字符串内转义下一字节。
 */
static CastleU32 toml_content_length_(const char* line, CastleU32 length) {
    CastleU32 index;
    int in_string = 0;
    int escaped = 0;
    for (index = 0u; index < length; ++index) {
        char value = line[index];
        if (in_string) {
            if (escaped) escaped = 0;
            else if (value == '\\') escaped = 1;
            else if (value == '"') in_string = 0;
        } else if (value == '"') {
            in_string = 1;
        } else if (value == '#') {
            return index;
        }
    }
    return length;
}

static int toml_bare_name_valid_(RuntimeTomlSlice name) {
    CastleU32 index;
    if (!name.data || name.length == 0u || name.length >= RUNTIME_TOML_NAME_CAP) return 0;
    for (index = 0u; index < name.length; ++index) {
        char value = name.data[index];
        if (!((value >= 'A' && value <= 'Z') ||
              (value >= 'a' && value <= 'z') ||
              (value >= '0' && value <= '9') ||
              value == '_' || value == '-' || value == '.')) return 0;
    }
    return 1;
}

/* 验证 UTF-8 的连续字节形状；不接受截断、多余 continuation 或编码代理区。 */
static int toml_utf8_valid_(const CastleU8* bytes, CastleU32 size) {
    CastleU32 index = 0u;
    while (index < size) {
        CastleU8 first = bytes[index++];
        CastleU32 needed;
        CastleU32 code;
        CastleU32 minimum;
        if (first < 0x80u) continue;
        if ((first & 0xE0u) == 0xC0u) {
            needed = 1u; code = first & 0x1Fu; minimum = 0x80u;
        } else if ((first & 0xF0u) == 0xE0u) {
            needed = 2u; code = first & 0x0Fu; minimum = 0x800u;
        } else if ((first & 0xF8u) == 0xF0u) {
            needed = 3u; code = first & 0x07u; minimum = 0x10000u;
        } else return 0;
        if (needed > size - index) return 0;
        while (needed--) {
            CastleU8 next = bytes[index++];
            if ((next & 0xC0u) != 0x80u) return 0;
            code = (code << 6u) | (next & 0x3Fu);
        }
        if (code < minimum || code > 0x10FFFFu ||
            (code >= 0xD800u && code <= 0xDFFFu)) return 0;
    }
    return 1;
}

/* 第一版配置文档逐行验证 table/key/value 的基本边界，坏文档在 Open 时整体拒绝。 */
static int toml_document_valid_(const CastleU8* bytes, CastleU32 size) {
    CastleU32 position = 0u;
    while (position < size) {
        CastleU32 begin = position;
        CastleU32 end;
        RuntimeTomlSlice line;
        RuntimeTomlSlice left;
        RuntimeTomlSlice right;
        CastleU32 equal = 0xFFFFFFFFu;
        CastleU32 index;
        int in_string = 0;
        int escaped = 0;
        while (position < size && bytes[position] != '\n') ++position;
        end = position;
        if (position < size) ++position;
        line.data = (const char*)bytes + begin;
        line.length = toml_content_length_(line.data, end - begin);
        line = toml_trim_(line);
        if (line.length == 0u) continue;
        if (line.data[0] == '[') {
            if (line.length < 3u || line.data[line.length - 1u] != ']' ||
                line.data[1] == '[') return 0;
            left.data = line.data + 1u;
            left.length = line.length - 2u;
            left = toml_trim_(left);
            if (!toml_bare_name_valid_(left)) return 0;
            continue;
        }
        for (index = 0u; index < line.length; ++index) {
            char value = line.data[index];
            if (in_string) {
                if (escaped) escaped = 0;
                else if (value == '\\') escaped = 1;
                else if (value == '"') in_string = 0;
            } else if (value == '"') in_string = 1;
            else if (value == '=') { equal = index; break; }
        }
        if (equal == 0xFFFFFFFFu) return 0;
        left.data = line.data;
        left.length = equal;
        right.data = line.data + equal + 1u;
        right.length = line.length - equal - 1u;
        left = toml_trim_(left);
        right = toml_trim_(right);
        if (!toml_bare_name_valid_(left) || right.length == 0u) return 0;
    }
    return 1;
}

/*
 * 每次 Get 扫描固定内存文档。配置只在插件初始化/热重载时读取，线性扫描比跨 DLL
 * 暴露树节点更安全，也避免 Runtime 为小配置维护复杂堆对象。
 */
static int toml_find_value_locked_(RuntimeTomlDocument* document,
                                   CastleStringView wanted_table,
                                   CastleStringView wanted_key,
                                   RuntimeTomlSlice* out_value) {
    CastleU32 position = 0u;
    RuntimeTomlSlice current_table = {NULL, 0u};
    while (position < document->size) {
        CastleU32 begin = position;
        CastleU32 end;
        RuntimeTomlSlice line;
        CastleU32 equal = 0xFFFFFFFFu;
        CastleU32 index;
        int in_string = 0;
        int escaped = 0;
        while (position < document->size && document->bytes[position] != '\n') ++position;
        end = position;
        if (position < document->size) ++position;
        line.data = (const char*)document->bytes + begin;
        line.length = toml_content_length_(line.data, end - begin);
        line = toml_trim_(line);
        if (!line.length) continue;
        if (line.data[0] == '[') {
            current_table.data = line.data + 1u;
            current_table.length = line.length - 2u;
            current_table = toml_trim_(current_table);
            continue;
        }
        if (!toml_view_equal_(current_table, wanted_table)) continue;
        for (index = 0u; index < line.length; ++index) {
            char value = line.data[index];
            if (in_string) {
                if (escaped) escaped = 0;
                else if (value == '\\') escaped = 1;
                else if (value == '"') in_string = 0;
            } else if (value == '"') in_string = 1;
            else if (value == '=') { equal = index; break; }
        }
        if (equal == 0xFFFFFFFFu) continue;
        {
            RuntimeTomlSlice key = {line.data, equal};
            key = toml_trim_(key);
            if (!toml_view_equal_(key, wanted_key)) continue;
        }
        out_value->data = line.data + equal + 1u;
        out_value->length = line.length - equal - 1u;
        *out_value = toml_trim_(*out_value);
        return 1;
    }
    return 0;
}

void Runtime_TomlInitialize(void) {
    g_toml_lock = 0;
    g_toml_generation = 1u;
    Runtime_ByteZero(g_toml_documents, (CastleU32)sizeof(g_toml_documents));
}

const CastleTomlApiV1* Runtime_GetTomlApiV1(void) {
    return &g_toml_api;
}

static CastleResult CASTLE_RUNTIME_CALL toml_open_(
    CastlePluginHandle plugin, CastleStringView relative_path,
    CastleTomlDocumentHandle* out_document) {
    const CastleFileApiV1* file_api = Runtime_GetFileApiV1();
    CastleU32 index;
    CastleFileBufferV1 buffer;
    CastleResult result;
    RuntimeTomlDocument* document = NULL;
    if (!out_document || !Runtime_GetPluginModule(plugin)) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_document = 0u;
    Runtime_Lock(&g_toml_lock);
    for (index = 0u; index < RUNTIME_TOML_MAX_DOCUMENTS; ++index) {
        if (!g_toml_documents[index].used) {
            document = &g_toml_documents[index];
            break;
        }
    }
    if (!document) {
        Runtime_Unlock(&g_toml_lock);
        return CASTLE_ERROR_RESOURCE_CONFLICT;
    }
    Runtime_ByteZero(&buffer, (CastleU32)sizeof(buffer));
    buffer.magic = CASTLE_FILE_BUFFER_MAGIC;
    buffer.struct_size = CASTLE_SIZEOF_FILE_BUFFER_V1;
    buffer.version = CASTLE_FILE_STRUCTURE_VERSION_1;
    buffer.data = document->bytes;
    buffer.capacity = RUNTIME_TOML_DOCUMENT_CAP;
    result = file_api->ReadPluginFile(plugin, relative_path, &buffer);
    if (result < 0 || buffer.bytes_written == 0u ||
        !toml_utf8_valid_(document->bytes, buffer.bytes_written)) {
        Runtime_Unlock(&g_toml_lock);
        return result < 0 ? result : CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (buffer.bytes_written >= 3u && document->bytes[0] == 0xEFu &&
        document->bytes[1] == 0xBBu && document->bytes[2] == 0xBFu) {
        Runtime_ByteCopy(document->bytes, document->bytes + 3u,
                         buffer.bytes_written - 3u);
        buffer.bytes_written -= 3u;
    }
    if (!toml_document_valid_(document->bytes, buffer.bytes_written)) {
        Runtime_Unlock(&g_toml_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    document->used = 1;
    document->plugin = plugin;
    document->size = buffer.bytes_written;
    document->handle = (++g_toml_generation << 8u) | (index + 1u);
    if (document->handle == 0u) document->handle = (++g_toml_generation << 8u) | (index + 1u);
    *out_document = document->handle;
    Runtime_Unlock(&g_toml_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL toml_close_(
    CastleTomlDocumentHandle document_handle) {
    RuntimeTomlDocument* document;
    if (!document_handle) return CASTLE_ERROR_INVALID_ARGUMENT;
    Runtime_Lock(&g_toml_lock);
    document = toml_find_locked_(document_handle);
    if (!document) {
        Runtime_Unlock(&g_toml_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_ByteZero(document, (CastleU32)sizeof(*document));
    Runtime_Unlock(&g_toml_lock);
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL toml_get_bool_(
    CastleTomlDocumentHandle document_handle, CastleStringView table,
    CastleStringView key, CastleU32 default_value, CastleU32* out_value) {
    RuntimeTomlDocument* document;
    RuntimeTomlSlice value;
    CastleResult result = CASTLE_STATUS_DEFAULT_BACKEND;
    if (!out_value || default_value > 1u || !toml_bare_name_valid_((RuntimeTomlSlice){table.data, table.length}) ||
        !toml_bare_name_valid_((RuntimeTomlSlice){key.data, key.length})) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_value = default_value;
    Runtime_Lock(&g_toml_lock);
    document = toml_find_locked_(document_handle);
    if (!document) {
        Runtime_Unlock(&g_toml_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (toml_find_value_locked_(document, table, key, &value)) {
        if (value.length == 4u && Runtime_StringEquals(value.data, 4u, "true", 4u)) {
            *out_value = 1u; result = CASTLE_OK;
        } else if (value.length == 5u && Runtime_StringEquals(value.data, 5u, "false", 5u)) {
            *out_value = 0u; result = CASTLE_OK;
        } else result = CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Unlock(&g_toml_lock);
    return result;
}

static int toml_parse_s32_(RuntimeTomlSlice value, CastleS32* out_value) {
    CastleU32 index = 0u;
    CastleU32 magnitude = 0u;
    int negative = 0;
    int digit_seen = 0;
    if (!out_value || !value.length) return 0;
    if (value.data[index] == '-' || value.data[index] == '+') {
        negative = value.data[index] == '-';
        if (++index == value.length) return 0;
    }
    for (; index < value.length; ++index) {
        CastleU32 digit;
        char ch = value.data[index];
        if (ch == '_') continue;
        if (ch < '0' || ch > '9') return 0;
        digit = (CastleU32)(ch - '0');
        if (magnitude > 214748364u ||
            (magnitude == 214748364u && digit > (negative ? 8u : 7u))) return 0;
        magnitude = magnitude * 10u + digit;
        digit_seen = 1;
    }
    if (!digit_seen) return 0;
    *out_value = negative ? (CastleS32)(0u - magnitude) : (CastleS32)magnitude;
    return 1;
}

static CastleResult CASTLE_RUNTIME_CALL toml_get_s32_(
    CastleTomlDocumentHandle document_handle, CastleStringView table,
    CastleStringView key, CastleS32 default_value, CastleS32 minimum_value,
    CastleS32 maximum_value, CastleS32* out_value) {
    RuntimeTomlDocument* document;
    RuntimeTomlSlice value;
    CastleS32 parsed;
    CastleResult result = CASTLE_STATUS_DEFAULT_BACKEND;
    if (!out_value || minimum_value > maximum_value ||
        default_value < minimum_value || default_value > maximum_value ||
        !toml_bare_name_valid_((RuntimeTomlSlice){table.data, table.length}) ||
        !toml_bare_name_valid_((RuntimeTomlSlice){key.data, key.length})) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_value = default_value;
    Runtime_Lock(&g_toml_lock);
    document = toml_find_locked_(document_handle);
    if (!document) {
        Runtime_Unlock(&g_toml_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (toml_find_value_locked_(document, table, key, &value)) {
        if (!toml_parse_s32_(value, &parsed) || parsed < minimum_value ||
            parsed > maximum_value) result = CASTLE_ERROR_INVALID_ARGUMENT;
        else { *out_value = parsed; result = CASTLE_OK; }
    }
    Runtime_Unlock(&g_toml_lock);
    return result;
}

static CastleResult toml_copy_default_(CastleStringView value, char* output,
                                       CastleU32 output_capacity,
                                       CastleU32* out_length) {
    if (!out_length || (value.length && !value.data)) return CASTLE_ERROR_INVALID_ARGUMENT;
    *out_length = value.length;
    if (!output || output_capacity <= value.length) return CASTLE_ERROR_BUFFER_TOO_SMALL;
    if (value.length) Runtime_ByteCopy(output, value.data, value.length);
    output[value.length] = '\0';
    return CASTLE_STATUS_DEFAULT_BACKEND;
}

static CastleResult CASTLE_RUNTIME_CALL toml_get_string_(
    CastleTomlDocumentHandle document_handle, CastleStringView table,
    CastleStringView key, CastleStringView default_value,
    char* output, CastleU32 output_capacity, CastleU32* out_length) {
    RuntimeTomlDocument* document;
    RuntimeTomlSlice value;
    CastleU32 source;
    CastleU32 written = 0u;
    CastleResult result;
    /*
     * out_length 是跨 DLL ABI 的必填输出参数。这里必须在拿锁和扫描文档之前检查，
     * 否则调用者误传空指针时，函数会在最后写回长度的位置直接触发访问异常。
     */
    if (!out_length ||
        !toml_bare_name_valid_((RuntimeTomlSlice){table.data, table.length}) ||
        !toml_bare_name_valid_((RuntimeTomlSlice){key.data, key.length})) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    Runtime_Lock(&g_toml_lock);
    document = toml_find_locked_(document_handle);
    if (!document) {
        Runtime_Unlock(&g_toml_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    if (!toml_find_value_locked_(document, table, key, &value)) {
        Runtime_Unlock(&g_toml_lock);
        return toml_copy_default_(default_value, output, output_capacity, out_length);
    }
    if (value.length < 2u || value.data[0] != '"' ||
        value.data[value.length - 1u] != '"') {
        Runtime_Unlock(&g_toml_lock);
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }
    for (source = 1u; source + 1u < value.length; ++source) {
        char ch = value.data[source];
        if (ch == '\\') {
            if (++source + 1u >= value.length) {
                Runtime_Unlock(&g_toml_lock);
                return CASTLE_ERROR_INVALID_ARGUMENT;
            }
            ch = value.data[source];
            if (ch == 'n') ch = '\n';
            else if (ch == 'r') ch = '\r';
            else if (ch == 't') ch = '\t';
            else if (ch != '\\' && ch != '"') {
                Runtime_Unlock(&g_toml_lock);
                return CASTLE_ERROR_INVALID_ARGUMENT;
            }
        }
        if (output && written + 1u < output_capacity) output[written] = ch;
        ++written;
    }
    *out_length = written;
    result = (!output || output_capacity <= written) ?
        CASTLE_ERROR_BUFFER_TOO_SMALL : CASTLE_OK;
    if (result == CASTLE_OK) output[written] = '\0';
    Runtime_Unlock(&g_toml_lock);
    return result;
}
