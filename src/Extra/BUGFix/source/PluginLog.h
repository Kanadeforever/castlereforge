#pragma once

#include "Win32Mini.h"
#include "CastleLog_API.h"

// ============================================================================
// PluginLog.h
// ----------------------------------------------------------------------------
// 四个正式 ASI 共用的极小日志工具。
//
// 官方 ASI 必须由 Castle_Runtime.dll 承载，日志只交给 Runtime Log 服务。
// 目录、UTF-8 BOM、并发锁和刷新由 Runtime 负责，插件不再持有文件句柄。
// ============================================================================

namespace ycrlog {

// 每个 ASI 都只包含自己这一份头文件实例，所以这个句柄只属于当前插件。
// INVALID_HANDLE_VALUE 表示“日志还没有成功打开”。
static const CastleLogApiV1* gRuntimeLogApi = nullptr;
static CastlePluginHandle gRuntimeLogPlugin = 0u;

inline bool BindRuntime(const CastleRuntimeApiV1* runtimeApi,
                        CastlePluginHandle plugin) {
    CastleInterfaceQueryV1 query{};
    CastleInterfaceResultV1 result{};
    static const char interfaceId[] = CASTLE_LOG_INTERFACE_ID;
    if (runtimeApi == nullptr || runtimeApi->QueryInterface == nullptr || plugin == 0u) return false;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = {interfaceId, static_cast<CastleU32>(sizeof(interfaceId) - 1u)};
    query.requested_version = CASTLE_LOG_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_LOG_API_V1;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (runtimeApi->QueryInterface(&query, &result) != CASTLE_OK) return false;
    gRuntimeLogApi = static_cast<const CastleLogApiV1*>(result.api_pointer);
    gRuntimeLogPlugin = plugin;
    return gRuntimeLogApi != nullptr && gRuntimeLogApi->WritePluginText != nullptr;
}

// 手工计算窄字符串的字节长度。
// 源码统一用 /utf-8 编译，因此中文窄字符串字面量本身就是 UTF-8 字节序列。
inline DWORD ByteLength(const char* text) {
    DWORD length = 0u;
    if (text == nullptr) {
        return 0u;
    }
    while (text[length] != '\0') {
        ++length;
    }
    return length;
}

// 最底层写入函数。
// 这里不补换行，只负责把给定的 length 个字节原样写到日志文件。
inline void Raw(const char* text, DWORD length) {
    if (text == nullptr || length == 0u) {
        return;
    }
    if (gRuntimeLogApi == nullptr || gRuntimeLogPlugin == 0u) return;
    CastleStringView view{text, static_cast<CastleU32>(length)};
    gRuntimeLogApi->WritePluginText(gRuntimeLogPlugin, view);
}

// 写一段 UTF-8 文本，不自动换行。
inline void Text(const char* text) {
    Raw(text, ByteLength(text));
}

// 写一整行文本，并明确追加 Windows 标准 CRLF。
inline void Line(const char* text) {
    Text(text);
    Text("\r\n");
}

// 把无符号十进制数字写到日志。
// 不使用 sprintf，是为了不引入 CRT。
inline void Unsigned(DWORD value) {
    char digits[16];
    DWORD count = 0u;
    do {
        digits[count++] = static_cast<char>('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u && count < sizeof(digits));

    while (count > 0u) {
        --count;
        Raw(&digits[count], 1u);
    }
}

// 写单个 ASCII 字符。盘符、0/1 等非常短的信息会用到。
inline void Character(char value) {
    Raw(&value, 1u);
}

// 把一个 32 位数写成固定 8 位十六进制，例如 0x00409541。
// CrashFix 合并进 BUGFix 后会用它记录 runtime/controller 指针和补丁地址。
// 手工逐位输出，不调用 sprintf，因此仍然不引入 C Runtime。
inline void Hex32(DWORD value) {
    Text("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        const DWORD digit = (value >> static_cast<DWORD>(shift)) & 0x0Fu;
        const char ch = static_cast<char>(digit < 10u ? ('0' + digit) : ('A' + (digit - 10u)));
        Character(ch);
    }
}

// 关闭日志。
// Windows 卸载 ASI 时调用，避免句柄一直占着文件。
inline void Close() {
    gRuntimeLogApi = nullptr;
    gRuntimeLogPlugin = 0u;
}

// 保留旧调用形状，避免业务文件因日志后端迁移而出现无关改动；参数不再参与路径构造。
inline bool Open(HMODULE module, const wchar_t* logFileName) {
    (void)module;
    (void)logFileName;
    return gRuntimeLogApi != nullptr && gRuntimeLogPlugin != 0u;
}

} // namespace ycrlog
