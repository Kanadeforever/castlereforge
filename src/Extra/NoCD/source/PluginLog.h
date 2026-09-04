#pragma once

#include "Win32Mini.h"
#include "CastleLog_API.h"

// ============================================================================
// PluginLog.h
// ----------------------------------------------------------------------------
// 四个正式 ASI 共用的极小日志工具。
//
// 设计目标：
// 1. 每次启动游戏时都使用 CREATE_ALWAYS 打开日志，因此旧日志会先被清空；
// 2. 文件开头写 UTF-8 BOM（EF BB BF），正文使用 UTF-8 字节；
// 3. 每一行都明确写入 CRLF（\r\n），不依赖编辑器自动转换换行；
// 4. 不使用 iostream、printf、std::string 等 C/C++ 运行库功能，继续只依赖
//    Windows 自带的 kernel32.dll；
// 5. 日志文件始终放在 ASI 自己所在的目录，而不是依赖进程当前工作目录。
//
// 对刚接触编程的人来说，可以把这里理解成：
// “先找到插件自己放在哪个文件夹，把插件文件名替换成 xxx.log，然后把文字一个字节
//  一个字节写进去”。UTF-8 BOM 是文件最前面的三个特殊字节，用来帮助记事本等程序
// 更稳定地识别这是一份 UTF-8 文本。
// ============================================================================

namespace ycrlog {

// 每个 ASI 都只包含自己这一份头文件实例，所以这个句柄只属于当前插件。
// INVALID_HANDLE_VALUE 表示“日志还没有成功打开”。
static HANDLE gFile = INVALID_HANDLE_VALUE;
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
    if (gRuntimeLogApi != nullptr && gRuntimeLogPlugin != 0u) {
        CastleStringView view{text, static_cast<CastleU32>(length)};
        gRuntimeLogApi->WritePluginText(gRuntimeLogPlugin, view);
        return;
    }
    if (gFile == INVALID_HANDLE_VALUE) return;
    DWORD written = 0u;
    WriteFile(gFile, text, length, &written, nullptr);
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
    if (gFile != INVALID_HANDLE_VALUE) {
        CloseHandle(gFile);
        gFile = INVALID_HANDLE_VALUE;
    }
}

// 生成“ASI 同目录下的固定日志名”，并使用 CREATE_ALWAYS 打开。
// 例如：
//   D:\Game\BUGFix.asi
// 会变成：
//   D:\Game\BUGFix.log
//
// logFileName 必须只提供文件名，不需要提供目录。
inline bool Open(HMODULE module, const wchar_t* logFileName) {
    if (gRuntimeLogApi != nullptr && gRuntimeLogPlugin != 0u) return true;
    // 如果极端情况下重复调用 Open，先关掉旧句柄，避免资源泄漏。
    Close();

    if (logFileName == nullptr) {
        return false;
    }

    wchar_t path[1024];
    path[0] = L'\0';
    const DWORD length = GetModuleFileNameW(module, path, 1024u);
    if (length == 0u || length >= 1024u) {
        return false;
    }

    // 找最后一个路径分隔符。它后面的内容就是原 ASI 文件名。
    DWORD fileNameStart = 0u;
    for (DWORD i = 0u; i < length; ++i) {
        if (path[i] == L'\\' || path[i] == L'/') {
            fileNameStart = i + 1u;
        }
    }

    // 把原文件名覆盖成调用者指定的日志名。
    DWORD i = 0u;
    while (logFileName[i] != L'\0') {
        if (fileNameStart + i + 1u >= 1024u) {
            return false;
        }
        path[fileNameStart + i] = logFileName[i];
        ++i;
    }
    path[fileNameStart + i] = L'\0';

    // CREATE_ALWAYS 的语义是：
    // - 文件不存在：新建；
    // - 文件已经存在：把长度截成 0，再从头写。
    // 这正好满足“每次运行之前清空旧日志”的要求。
    gFile = CreateFileW(
        path,
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (gFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    // UTF-8 BOM。正文仍然是标准 UTF-8；BOM 只是帮助 Windows 文本工具识别编码。
    const char bom[3] = {
        static_cast<char>(0xEF),
        static_cast<char>(0xBB),
        static_cast<char>(0xBF)
    };
    Raw(bom, 3u);
    return true;
}

} // namespace ycrlog
