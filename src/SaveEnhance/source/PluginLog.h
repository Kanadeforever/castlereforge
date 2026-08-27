#ifndef CASTLE_SAVE_ENHANCE_PLUGIN_LOG_H
#define CASTLE_SAVE_ENHANCE_PLUGIN_LOG_H

#include "Win32Mini.h"

// ============================================================================
// PluginLog.h
// ----------------------------------------------------------------------------
// 这是 Castle_SaveEnhance 自己携带的“最小日志工具”。
//
// 为什么不用 printf / std::ofstream：
// 老游戏 ASI 会被直接塞进 RPG.exe 进程里。为了尽量减少运行库冲突，本项目选择无 CRT
// 构建，所以这里不依赖 std::string、iostream、stdio，而是直接调用 Windows WriteFile。
//
// 对刚学编程的人，可以把整个流程想成：
// 1. Open() 先找到“这个 ASI 文件自己住在哪个文件夹”；
// 2. 在同一个文件夹创建 Castle_SaveEnhance.log；
// 3. Text()/Line()/Unsigned()/Hex() 把要看的内容变成字节，再交给 WriteFile；
// 4. 游戏退出时 Close() 把 Windows 文件句柄关掉。
//
// 中文日志字符串已经由 /utf-8 编译成 UTF-8 字节，所以不需要自己再写复杂编码转换器。
// ============================================================================

namespace ycrlog {

// Windows 打开文件以后会返回一个 HANDLE，可以理解为“这个已经打开文件的编号/把手”。
// INVALID_HANDLE_VALUE 表示目前没有可用日志文件。
static HANDLE gLogFile = INVALID_HANDLE_VALUE;

inline SIZE_T StringLengthA(const char* text) {
    // char 字符串结尾有一个值为 0 的 '\0'。这里从第 0 个字符开始数，直到遇到结尾。
    // 之所以自己数长度，是因为无 CRT 构建不能直接依赖 strlen。
    if (text == nullptr) {
        return 0u;
    }
    SIZE_T n = 0u;
    while (text[n] != '\0') {
        ++n;
    }
    return n;
}

inline SIZE_T StringLengthW(const wchar_t* text) {
    // wchar_t 版本和上面完全同理，只是 Windows 路径 API 使用宽字符 L"..."。
    // null 指针直接返回 0，避免后面因为误传空地址而崩溃。
    if (text == nullptr) {
        return 0u;
    }
    SIZE_T n = 0u;
    while (text[n] != L'\0') {
        ++n;
    }
    return n;
}

inline void WriteRaw(const char* text, SIZE_T size) {
    // 这是所有文本输出最后都会经过的最底层函数。
    // 没有打开日志、没有文本、或者长度为 0 时什么都不做，日志失败不能拖垮游戏。
    if (gLogFile == INVALID_HANDLE_VALUE || text == nullptr || size == 0u) {
        return;
    }

    // WriteFile 需要一个 DWORD 接收“Windows 实际写了多少字节”。
    // 当前日志很短，SIZE_T 转 DWORD 不会溢出；即使写失败，也只损失诊断信息，不影响功能。
    DWORD written = 0u;
    WriteFile(gLogFile, text, static_cast<DWORD>(size), &written, nullptr);
}

inline void Text(const char* text) {
    // Text 只写文字本身，不自动换行；例如先写“槽号=”再调用 Unsigned(91)。
    WriteRaw(text, StringLengthA(text));
}

inline void Line(const char* text) {
    // Line 在 Text 后补 Windows 常见的 CRLF 两字节换行，记事本打开会正常一行一行显示。
    Text(text);
    WriteRaw("\r\n", 2u);
}

inline void Unsigned(DWORD value) {
    // 不能用 sprintf，所以手工把无符号整数拆成十进制字符。
    // buffer 最多放 10 位 DWORD 数字，16 字节已经很宽裕。
    char buffer[16];
    SIZE_T used = 0u;
    if (value == 0u) {
        // 0 是特殊情况：循环除以 10 会一次都不执行，所以直接写字符 '0'。
        buffer[used++] = '0';
    } else {
        // 我们从“个位”开始用 value % 10 取数字，因此得到的顺序是反的。
        // 例如 123 会先得到 3、2、1，先临时放进 reversed。
        char reversed[16];
        SIZE_T count = 0u;
        while (value != 0u && count < 15u) {
            reversed[count++] = static_cast<char>('0' + (value % 10u));
            value /= 10u;
        }

        // 再从 reversed 尾部往前拿，就恢复成正常的 1、2、3。
        while (count > 0u) {
            buffer[used++] = reversed[--count];
        }
    }
    WriteRaw(buffer, used);
}

inline void HexByte(BYTE value) {
    // 单个机器码字节最适合写成两位十六进制，例如 E8、90、FF。
    // 预检查失败时我们要把“实际字节”和“期望字节”完整列出来，所以不能只输出 DWORD。
    static const char kHex[] = "0123456789ABCDEF";
    char buffer[2];
    buffer[0] = kHex[(value >> 4u) & 0x0Fu];
    buffer[1] = kHex[value & 0x0Fu];
    WriteRaw(buffer, 2u);
}

inline void Bytes(const BYTE* bytes, SIZE_T size) {
    // 逐个输出形如“E8 29 65 01 00”的机器码序列。
    // 每个字节之间放一个空格，最后一个字节后面不多放空格，日志更容易复制到反汇编器里搜索。
    if (bytes == nullptr || size == 0u) {
        Text("<empty>");
        return;
    }
    for (SIZE_T i = 0u; i < size; ++i) {
        if (i != 0u) {
            WriteRaw(" ", 1u);
        }
        HexByte(bytes[i]);
    }
}

inline void Hex(DWORD value) {
    // 十六进制地址常写成 0x00424DF2。这里固定输出 0x + 8 位，便于和反汇编地址对齐。
    static const char kHex[] = "0123456789ABCDEF";
    char buffer[10];
    buffer[0] = '0';
    buffer[1] = 'x';

    // DWORD 有 32 位，每 4 位对应一个十六进制数字，所以一共正好 8 个数字。
    // 从最高 4 位开始依次右移，&0xF 只保留当前这一位的 0~15。
    for (SIZE_T i = 0u; i < 8u; ++i) {
        const DWORD shift = static_cast<DWORD>((7u - i) * 4u);
        buffer[2u + i] = kHex[(value >> shift) & 0xFu];
    }
    WriteRaw(buffer, 10u);
}

inline bool Open(HMODULE module, const wchar_t* filename) {
    // path 先装 ASI 自己的完整路径，例如：
    // C:\Game\mods\asi\Castle_SaveEnhance.asi
    wchar_t path[520];
    const DWORD length = GetModuleFileNameW(module, path, 520u);
    if (length == 0u || length >= 520u) {
        return false;
    }

    // 从末尾向前找最后一个斜杠，只保留目录部分：
    // C:\Game\mods\asi\  （这里只表示目录，下一行再继续处理）
    SIZE_T cut = static_cast<SIZE_T>(length);
    while (cut > 0u && path[cut - 1u] != L'\\' && path[cut - 1u] != L'/') {
        --cut;
    }

    // 先确认“目录长度 + 新文件名 + 结尾 0”不会超过固定 path 数组。
    // 宁可没有日志，也不能因为日志路径太长写坏栈内存。
    const SIZE_T nameLength = StringLengthW(filename);
    if (cut + nameLength + 1u >= 520u) {
        return false;
    }

    // 把目录后的原 ASI 文件名覆盖成日志文件名。
    for (SIZE_T i = 0u; i < nameLength; ++i) {
        path[cut + i] = filename[i];
    }
    path[cut + nameLength] = L'\0';

    // CREATE_ALWAYS 表示每次启动都重新创建日志，旧日志不会和本轮结果混在一起。
    // FILE_SHARE_READ 允许用户在游戏运行中直接打开日志查看，而不会阻止插件继续写。
    gLogFile = CreateFileW(
        path,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    return gLogFile != INVALID_HANDLE_VALUE;
}

inline void Close() {
    // 只关闭真正打开成功的句柄。关闭以后立刻恢复 INVALID_HANDLE_VALUE，防止以后误写旧句柄。
    if (gLogFile != INVALID_HANDLE_VALUE) {
        CloseHandle(gLogFile);
        gLogFile = INVALID_HANDLE_VALUE;
    }
}

} // namespace ycrlog

#endif // CASTLE_SAVE_ENHANCE_PLUGIN_LOG_H
