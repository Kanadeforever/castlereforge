#include "PatchUtil.h"

// ============================================================================
// 极小的编译器辅助函数
// ----------------------------------------------------------------------------
// 即使源码没有主动调用 memcpy/memset，Clang/MSVC 也可能把“小数组初始化”自动
// 优化成对这些函数的调用。我们又刻意使用 /NODEFAULTLIB，不链接完整 C 运行库，
// 因此在这里提供最小实现，避免为了几十个字节复制而引入 CRT DLL。
//
// 这两个函数没有任何游戏业务逻辑：就是逐字节复制/填充。
// volatile 指针让编译器不要再次把循环优化回 memcpy/memset，避免形成递归调用。
// ============================================================================
extern "C" void* __cdecl memcpy(void* destination, const void* source, SIZE_T size) {
    volatile BYTE* out = reinterpret_cast<volatile BYTE*>(destination);
    const volatile BYTE* in = reinterpret_cast<const volatile BYTE*>(source);
    for (SIZE_T i = 0u; i < size; ++i) {
        out[i] = in[i];
    }
    return destination;
}

extern "C" void* __cdecl memset(void* destination, int value, SIZE_T size) {
    volatile BYTE* out = reinterpret_cast<volatile BYTE*>(destination);
    const BYTE fill = static_cast<BYTE>(value);
    for (SIZE_T i = 0u; i < size; ++i) {
        out[i] = fill;
    }
    return destination;
}

// ============================================================================
// AnytimeSaveProbe.asi  v0.3.1a-probe1
// ----------------------------------------------------------------------------
// 用途：调查《幽城幻剑录》“旧随时存档在楼梯等位置可能形成死档”的真正运行时条件。
//
// 这是“只读诊断插件”，不是新的随时存档插件。
// 最重要的安全承诺：
//   * RPG.exe 原版说“可以存档”时，本插件原样返回原结果；
//   * RPG.exe 原版说“不可以存档”时，本插件仍然返回“不可以”；
//   * 本插件绝不会为了测试而把一个原版禁存点改成可存；
//   * 本插件不会调用碰撞检测函数 0x4080D0 / 0x409A50，因为这些函数可能改变
//     移动结果甚至触发 Event；这里只读取已经存在的运行时对象并写日志。
//
// 为什么要先做诊断而不是立刻再写 v0.2.2：
// v0.2.1 的实机反例已经证明，“内部地点名 + 实体卸载倒计时”不足以识别楼梯危险点。
// 继续凭猜测增加几个全局变量，只会在“误放坏点”和“误杀正常点”之间来回摆动。
// 所以这一版先把两个事实样本抓出来：
//   A. 已知楼梯危险点；
//   B. 普通、稳定、原版禁存但我们希望未来开放的探索点。
// 对比两份运行态，才能找到真正具有区分力的最小安全条件。
// ============================================================================

namespace {

// ------------------------------ 一、Hook 地址 ---------------------------------

// 正常菜单里读取 MapSCI +0x380 的原版 CALL：
//   0040CCC0  mov ecx,eax
//   0040CCC2  call 0040A0C0
// 本插件只把这一条 CALL 临时改为 ProbeSaveGate；ProbeSaveGate 最后仍返回原版结果。
const DWORD kNormalMenuSaveGateCallRva = 0x0000CCC2u;
const BYTE kOriginalNormalMenuCall[5] = {0xE8, 0xF9, 0xD3, 0xFF, 0xFF};

// 原版存档许可访问器：读取传入 runtime entry 指向的 source MapSCI +0x380。
const DWORD kOriginalSaveGateFunctionRva = 0x0000A0C0u;
const BYTE kOriginalSaveGateFunctionBytes[9] = {
    0x8B, 0x01,
    0x8B, 0x80, 0x80, 0x03, 0x00, 0x00,
    0xC3
};

// 历史“随时存档”补丁会把读取 +0x380 改成固定返回 1。
// 诊断的目标是观察“真正原版会不会允许”，所以如果只精确命中这个已知旧补丁，
// 会先把这 9 字节恢复为原版；遇到任何其它陌生机器码则拒绝安装 Hook。
const BYTE kOldUnsafeSaveGateFunctionBytes[9] = {
    0x8B, 0x01,
    0xB8, 0x01, 0x00, 0x00, 0x00, 0x90,
    0xC3
};

typedef DWORD (__fastcall *OriginalSaveGateFunction)(void* runtimeEntry);

// -------------------------- 二、当前 EXE 已复核的 RVA --------------------------
// 这些都是“当前参考 RPG.exe 的模块内偏移”，不是跨版本 ABI。
// 安装 Hook 前仍会验证关键机器码；日志里的地址只是为了本轮调查。

const DWORD kDataCenterPointerRva = 0x0049F804u;       // 绝对 0x0089F804
const DWORD kControlledActorIndexRva = 0x00068BF0u;    // 绝对 0x00468BF0
const DWORD kMovementResultRva = 0x00068BF4u;          // 绝对 0x00468BF4
const DWORD kInputNavigationGateRva = 0x00068BB9u;     // 绝对 0x00468BB9
const DWORD kActorRuntimeArrayPointerRva = 0x0049F7F0u;// 绝对 0x0089F7F0
const DWORD kCurrentLocationNamePointerRva = 0x0006F3BCu; // 绝对 0x0046F3BC

// Event VM / deferred 协议附近的运行时全局区。
// 这些值这次只记录、不做“非零就危险”的先验判断。
const DWORD kEventRuntimeBlockRva = 0x0049F7F0u;
const SIZE_T kEventRuntimeBlockBytes = 0x50u;

const DWORD kEventSavedIdRva = 0x0049F7F8u;
const DWORD kEventSavedKeyARva = 0x0049F7FCu;
const DWORD kEventSavedKeyBRva = 0x0049F800u;
const DWORD kActiveEventIdRva = 0x0049F808u;
const DWORD kEventRuntime10Rva = 0x0049F810u;
const DWORD kEventRuntime18Rva = 0x0049F818u;
const DWORD kVmContinueByteRva = 0x0049F81Du;
const DWORD kVmBlockedByteRva = 0x0049F81Eu;
const DWORD kEntityTransitionCountdownRva = 0x0049F828u;
const DWORD kCurrentCommandPointerRva = 0x0049F830u;

const SIZE_T kActorStride = 0x74u;
const SIZE_T kMapRuntimeEntryStride = 0x38u;
const SIZE_T kMapSciRecordSize = 0x473u;

// 控制器里当前还存在大量作者语义未闭合字段。
// 与其猜字段名，不如把一小段原始字节保留下来，之后对“楼梯坏点/普通好点”做差分。
const SIZE_T kControllerDumpBytes = 0x84u;

// 活动实体很多时日志会非常长，所以最多记录 96 个槽。
// 这个上限只影响日志体积，不影响游戏逻辑；诊断插件绝不会修改实体表。
const DWORD kMaxEntitySlotsToInspect = 96u;

// ------------------------------- 三、全局状态 ----------------------------------

BYTE* gExeBase = nullptr;
OriginalSaveGateFunction gOriginalSaveGate = nullptr;
HANDLE gLogFile = INVALID_HANDLE_VALUE;
DWORD gSnapshotSequence = 0u;

// ---------------------------- 四、安全只读内存辅助 ------------------------------

// 判断 [address, address+size) 是否全部位于 Windows 当前标记为“已提交、可访问”的内存页。
// 这是诊断代码非常重要的一层保险：运行时对象可能暂时为空，不能看到一个指针就直接读。
bool IsReadableRange(const void* address, SIZE_T size) {
    if (address == nullptr || size == 0u) {
        return false;
    }

    const SIZE_T begin = reinterpret_cast<SIZE_T>(address);
    const SIZE_T end = begin + size;
    if (end < begin) {
        // 32 位加法回绕，说明范围本身非法。
        return false;
    }

    SIZE_T cursor = begin;
    while (cursor < end) {
        MEMORY_BASIC_INFORMATION_MINI info;
        const SIZE_T queried = VirtualQuery(
            reinterpret_cast<LPCVOID>(cursor),
            &info,
            sizeof(info));
        if (queried == 0u) {
            return false;
        }

        if (info.State != MEM_COMMIT ||
            (info.Protect & PAGE_GUARD) != 0u ||
            (info.Protect & PAGE_NOACCESS) != 0u ||
            info.RegionSize == 0u) {
            return false;
        }

        const SIZE_T regionBegin = reinterpret_cast<SIZE_T>(info.BaseAddress);
        const SIZE_T regionEnd = regionBegin + info.RegionSize;
        if (regionEnd <= cursor) {
            return false;
        }

        cursor = regionEnd;
    }

    return true;
}

bool ReadByteChecked(const BYTE* address, BYTE* value) {
    if (value == nullptr || !IsReadableRange(address, 1u)) {
        return false;
    }
    *value = *reinterpret_cast<const volatile BYTE*>(address);
    return true;
}

bool ReadDwordChecked(const BYTE* address, DWORD* value) {
    if (value == nullptr || !IsReadableRange(address, sizeof(DWORD))) {
        return false;
    }
    *value = *reinterpret_cast<const volatile DWORD*>(address);
    return true;
}

bool ReadExeDword(DWORD rva, DWORD* value) {
    if (gExeBase == nullptr) {
        return false;
    }
    return ReadDwordChecked(gExeBase + rva, value);
}

bool ReadExeByte(DWORD rva, BYTE* value) {
    if (gExeBase == nullptr) {
        return false;
    }
    return ReadByteChecked(gExeBase + rva, value);
}

// ------------------------------- 五、日志写入器 --------------------------------
// 不使用 sprintf / iostream / STL。每个数字都手工转换成 ASCII，然后直接 WriteFile。
// 这样最终 ASI 不需要 C/C++ 运行库，导入表可以继续只有 kernel32.dll。

void LogRaw(const char* text, DWORD length) {
    if (gLogFile == INVALID_HANDLE_VALUE || text == nullptr || length == 0u) {
        return;
    }
    DWORD written = 0u;
    WriteFile(gLogFile, text, length, &written, nullptr);
}

DWORD AsciiLength(const char* text) {
    DWORD length = 0u;
    if (text == nullptr) {
        return 0u;
    }
    while (text[length] != '\0') {
        ++length;
    }
    return length;
}

void LogText(const char* text) {
    LogRaw(text, AsciiLength(text));
}

void LogHexNibble(BYTE value) {
    const char digits[] = "0123456789ABCDEF";
    const char ch = digits[value & 0x0Fu];
    LogRaw(&ch, 1u);
}

void LogHex8(BYTE value) {
    LogHexNibble(static_cast<BYTE>(value >> 4));
    LogHexNibble(value);
}

void LogHex32(DWORD value) {
    LogText("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        LogHexNibble(static_cast<BYTE>((value >> shift) & 0x0Fu));
    }
}

void LogUnsigned(DWORD value) {
    char digits[16];
    DWORD count = 0u;
    do {
        digits[count++] = static_cast<char>('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u && count < sizeof(digits));

    while (count > 0u) {
        --count;
        LogRaw(&digits[count], 1u);
    }
}

void LogSigned(long value) {
    if (value < 0) {
        LogText("-");
        // 先转成 unsigned long 的“绝对值位模式”，避免依赖运行库 abs()。
        const unsigned long positive = static_cast<unsigned long>(-(value + 1L)) + 1u;
        LogUnsigned(static_cast<DWORD>(positive));
    } else {
        LogUnsigned(static_cast<DWORD>(value));
    }
}

void LogCrLf() {
    LogText("\r\n");
}

void LogPointer(const void* pointer) {
    LogHex32(static_cast<DWORD>(reinterpret_cast<SIZE_T>(pointer)));
}

void LogHexRange(const BYTE* address, SIZE_T size) {
    if (!IsReadableRange(address, size)) {
        LogText("<不可读>");
        return;
    }
    for (SIZE_T i = 0u; i < size; ++i) {
        if (i != 0u) {
            LogText(" ");
        }
        LogHex8(address[i]);
    }
}

// 固定长度旧式 C 字符串既可能是 ASCII 路径，也可能含 Big5 中文。
// 为避免在日志阶段错误“解码”而丢信息，这里输出原始十六进制字节，遇到 NUL 停止。
void LogFixedStringHex(const BYTE* address, SIZE_T maxBytes) {
    if (address == nullptr) {
        LogText("<空指针>");
        return;
    }
    for (SIZE_T i = 0u; i < maxBytes; ++i) {
        BYTE value = 0u;
        if (!ReadByteChecked(address + i, &value)) {
            LogText("<不可读>");
            return;
        }
        if (value == 0u) {
            return;
        }
        if (i != 0u) {
            LogText(" ");
        }
        LogHex8(value);
    }
}

void LogNamedDword(const char* name, DWORD value) {
    LogText(name);
    LogText("=");
    LogHex32(value);
    LogText(" (");
    LogUnsigned(value);
    LogText(")\r\n");
}

void LogNamedPointer(const char* name, const void* value) {
    LogText(name);
    LogText("=");
    LogPointer(value);
    LogCrLf();
}

void LogExeDword(const char* name, DWORD rva) {
    DWORD value = 0u;
    LogText(name);
    LogText("=");
    if (ReadExeDword(rva, &value)) {
        LogHex32(value);
        LogText(" (");
        LogUnsigned(value);
        LogText(")");
    } else {
        LogText("<不可读>");
    }
    LogCrLf();
}

void LogExeByte(const char* name, DWORD rva) {
    BYTE value = 0u;
    LogText(name);
    LogText("=");
    if (ReadExeByte(rva, &value)) {
        LogHex8(value);
        LogText(" (");
        LogUnsigned(static_cast<DWORD>(value));
        LogText(")");
    } else {
        LogText("<不可读>");
    }
    LogCrLf();
}

// ----------------------------- 六、日志文件创建 --------------------------------

bool OpenProbeLog(HMODULE module) {
    wchar_t path[1024];
    DWORD length = GetModuleFileNameW(module, path, 1024u);
    if (length == 0u || length >= 1024u) {
        return false;
    }

    // 找到 ASI 文件名开始的位置。日志与 ASI 放在同一个目录，方便用户直接回传。
    DWORD fileNameStart = 0u;
    for (DWORD i = 0u; i < length; ++i) {
        if (path[i] == L'\\' || path[i] == L'/') {
            fileNameStart = i + 1u;
        }
    }

    const wchar_t logName[] = L"AnytimeSaveProbe.log";
    DWORD nameIndex = 0u;
    while (logName[nameIndex] != L'\0') {
        if ((fileNameStart + nameIndex + 1u) >= 1024u) {
            return false;
        }
        path[fileNameStart + nameIndex] = logName[nameIndex];
        ++nameIndex;
    }
    path[fileNameStart + nameIndex] = L'\0';

    gLogFile = CreateFileW(
        path,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (gLogFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    // 与正式插件统一：CREATE_ALWAYS 已经清空旧日志；文件开头写 UTF-8 BOM。
    const char bom[3] = {
        static_cast<char>(0xEF),
        static_cast<char>(0xBB),
        static_cast<char>(0xBF)
    };
    LogRaw(bom, 3u);

    LogText("《幽城幻剑录》随时存档只读诊断插件 v0.3.1a-probe1 启动。\r\n");
    LogText("[日志] 本次启动已清空旧日志；编码=UTF-8，换行=CRLF。\r\n");
    LogText("[模式] 只读存档许可诊断；绝不扩展开放任何原版禁存点。\r\n");
    LogText("[保证] 原版存档许可结果原样返回；下面只展开原版拒绝时的运行态快照。\r\n");
    LogText("============================================================\r\n");
    return true;
}

// ----------------------------- 七、MapSCI 快照 ---------------------------------

void LogMapSciRecord(const BYTE* source) {
    LogNamedPointer("地图.源MapSCI记录", source);
    if (!IsReadableRange(source, kMapSciRecordSize)) {
        LogText("地图.源MapSCI记录=<无法完整读取0x473字节>\r\n");
        return;
    }

    BYTE enabled = source[0x00];
    const short originX = *reinterpret_cast<const volatile short*>(source + 0x66);
    const short originY = *reinterpret_cast<const volatile short*>(source + 0x68);
    const WORD width = *reinterpret_cast<const volatile WORD*>(source + 0x6A);
    const WORD height = *reinterpret_cast<const volatile WORD*>(source + 0x6C);

    LogText("地图.记录byte0="); LogHex8(enabled); LogCrLf();
    LogText("地图.原点X="); LogSigned(originX); LogCrLf();
    LogText("地图.原点Y="); LogSigned(originY); LogCrLf();
    LogText("地图.宽度="); LogUnsigned(width); LogCrLf();
    LogText("地图.高度="); LogUnsigned(height); LogCrLf();

    LogText("地图.SF2路径D9原始字节="); LogFixedStringHex(source + 0x0D9, 100u); LogCrLf();
    LogText("地图.SF2路径13D原始字节="); LogFixedStringHex(source + 0x13D, 100u); LogCrLf();
    LogText("地图.实体SCI路径原始字节="); LogFixedStringHex(source + 0x205, 200u); LogCrLf();
    LogText("地图.EVE路径原始字节="); LogFixedStringHex(source + 0x2CD, 100u); LogCrLf();

    LogNamedDword("地图.遇敌启用_378", *reinterpret_cast<const volatile DWORD*>(source + 0x378));
    LogNamedDword("地图.遇敌选择器_37C", *reinterpret_cast<const volatile DWORD*>(source + 0x37C));
    LogNamedDword("地图.原版存档许可_380", *reinterpret_cast<const volatile DWORD*>(source + 0x380));
    LogNamedDword("地图.code12备用许可_384", *reinterpret_cast<const volatile DWORD*>(source + 0x384));
}

void LogMapRuntimeEntry(const BYTE* entry, const char* prefix) {
    LogText(prefix); LogText(".pointer="); LogPointer(entry); LogCrLf();
    if (!IsReadableRange(entry, kMapRuntimeEntryStride)) {
        LogText(prefix); LogText("=<无法读取0x38字节运行时记录>\r\n");
        return;
    }

    const SIZE_T offsets[] = {
        0x00u, 0x04u, 0x08u, 0x0Cu, 0x10u, 0x14u, 0x18u, 0x1Cu,
        0x24u, 0x28u, 0x2Cu, 0x30u
    };
    for (SIZE_T i = 0u; i < (sizeof(offsets) / sizeof(offsets[0])); ++i) {
        LogText(prefix);
        LogText("+0x");
        LogHex8(static_cast<BYTE>(offsets[i]));
        LogText("=");
        const DWORD value = *reinterpret_cast<const volatile DWORD*>(entry + offsets[i]);
        LogHex32(value);
        LogText(" ("); LogUnsigned(value); LogText(")\r\n");
    }

    LogText(prefix); LogText("+0x35=");
    LogHexRange(entry + 0x35u, 3u);
    LogCrLf();
}

// ---------------------------- 八、角色与实体快照 -------------------------------

void LogActorRuntime(const BYTE* actor) {
    LogNamedPointer("玩家.运行时对象", actor);
    if (!IsReadableRange(actor, kActorStride)) {
        LogText("玩家.运行时对象=<无法读取0x74字节角色对象>\r\n");
        return;
    }

    const DWORD packed0C = *reinterpret_cast<const volatile DWORD*>(actor + 0x0C);
    const long x = *reinterpret_cast<const volatile long*>(actor + 0x10);
    const long y = *reinterpret_cast<const volatile long*>(actor + 0x14);
    const DWORD packed18 = *reinterpret_cast<const volatile DWORD*>(actor + 0x18);

    LogNamedDword("玩家.packed_0C", packed0C);
    LogText("玩家.X="); LogSigned(x); LogText(" ["); LogHex32(static_cast<DWORD>(x)); LogText("]\r\n");
    LogText("玩家.Y="); LogSigned(y); LogText(" ["); LogHex32(static_cast<DWORD>(y)); LogText("]\r\n");
    LogNamedDword("玩家.packed_18", packed18);

    const SIZE_T offsets[] = {0x3Cu, 0x40u, 0x54u, 0x60u, 0x64u, 0x68u, 0x6Cu, 0x70u};
    for (SIZE_T i = 0u; i < (sizeof(offsets) / sizeof(offsets[0])); ++i) {
        LogText("玩家+0x"); LogHex8(static_cast<BYTE>(offsets[i])); LogText("=");
        const DWORD value = *reinterpret_cast<const volatile DWORD*>(actor + offsets[i]);
        LogHex32(value); LogText(" ("); LogUnsigned(value); LogText(")\r\n");
    }

    const BYTE* sourceEntity = reinterpret_cast<const BYTE*>(
        static_cast<SIZE_T>(*reinterpret_cast<const volatile DWORD*>(actor + 0x70)));
    LogNamedPointer("玩家.源实体", sourceEntity);
    if (IsReadableRange(sourceEntity, 0xECu)) {
        LogText("玩家.源实体+73="); LogHex8(sourceEntity[0x73]); LogCrLf();
        LogText("玩家.源实体+74="); LogHex8(sourceEntity[0x74]); LogCrLf();
        LogText("玩家.源实体+75="); LogHex8(sourceEntity[0x75]); LogCrLf();
        LogText("玩家.源实体+76="); LogHex8(sourceEntity[0x76]); LogCrLf();
        LogText("玩家.源实体+77="); LogHex8(sourceEntity[0x77]); LogCrLf();
        LogText("玩家.源实体+78="); LogHex8(sourceEntity[0x78]); LogCrLf();
        LogText("玩家.源实体+79="); LogHex8(sourceEntity[0x79]); LogCrLf();
        LogText("玩家.源实体+7A="); LogHex8(sourceEntity[0x7A]); LogCrLf();
        LogText("玩家.源实体+EA="); LogHex8(sourceEntity[0xEA]); LogCrLf();
        LogText("玩家.源实体+EB="); LogHex8(sourceEntity[0xEB]); LogCrLf();
    }

    const BYTE* playerController = reinterpret_cast<const BYTE*>(
        static_cast<SIZE_T>(*reinterpret_cast<const volatile DWORD*>(actor + 0x6C)));
    LogNamedPointer("玩家.碰撞控制器", playerController);
    LogText("玩家.碰撞控制器原始字节=");
    if (IsReadableRange(playerController, kControllerDumpBytes)) {
        LogHexRange(playerController, kControllerDumpBytes);
    } else {
        LogText("<空指针或不可读>");
    }
    LogCrLf();
}

bool IsNearPlayer(long x, long y, long playerX, long playerY) {
    const long long dx = static_cast<long long>(x) - static_cast<long long>(playerX);
    const long long dy = static_cast<long long>(y) - static_cast<long long>(playerY);
    return dx >= -320 && dx <= 320 && dy >= -320 && dy <= 320;
}

void LogRelevantEntities(const BYTE* selectedEntry, const BYTE* player) {
    if (!IsReadableRange(selectedEntry, kMapRuntimeEntryStride) ||
        !IsReadableRange(player, kActorStride)) {
        LogText("实体=<当前地图记录或玩家对象不可用>\r\n");
        return;
    }

    const DWORD listAddress = *reinterpret_cast<const volatile DWORD*>(selectedEntry + 0x2C);
    const DWORD lastIndex = *reinterpret_cast<const volatile DWORD*>(selectedEntry + 0x1C);
    const BYTE* const* list = reinterpret_cast<const BYTE* const*>(static_cast<SIZE_T>(listAddress));

    LogText("实体.列表指针="); LogHex32(listAddress); LogCrLf();
    LogNamedDword("实体.最后索引原始值", lastIndex);

    // 构造/清理阶段可能用 0xFFFFFFFF 表示“没有最后一个有效槽”。
    // 这种情况应该记录为空表，而不是因为 +1 回绕成 0 后误读 96 个槽。
    if (lastIndex == 0xFFFFFFFFu) {
        LogText("实体=<没有活动槽位>\r\n");
        return;
    }

    DWORD slotCount = lastIndex + 1u;
    if (slotCount > kMaxEntitySlotsToInspect) {
        slotCount = kMaxEntitySlotsToInspect;
    }
    if (slotCount == 0u ||
        !IsReadableRange(list, static_cast<SIZE_T>(slotCount) * sizeof(void*))) {
        LogText("实体.列表=<不可读或为空>\r\n");
        return;
    }

    const long playerX = *reinterpret_cast<const volatile long*>(player + 0x10);
    const long playerY = *reinterpret_cast<const volatile long*>(player + 0x14);

    for (DWORD i = 0u; i < slotCount; ++i) {
        const BYTE* entity = list[i];
        if (!IsReadableRange(entity, kActorStride)) {
            continue;
        }

        const long x = *reinterpret_cast<const volatile long*>(entity + 0x10);
        const long y = *reinterpret_cast<const volatile long*>(entity + 0x14);
        const DWORD sourceAddress = *reinterpret_cast<const volatile DWORD*>(entity + 0x70);
        const BYTE* source = reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(sourceAddress));

        BYTE source74 = 0u;
        BYTE sourceEA = 0u;
        bool sourceReadable = IsReadableRange(source, 0xECu);
        if (sourceReadable) {
            source74 = source[0x74];
            sourceEA = source[0xEA];
        }

        // 重点记录：玩家自己、玩家附近实体、或带碰撞/接触 Event 配置的实体。
        if (entity != player &&
            !IsNearPlayer(x, y, playerX, playerY) &&
            source74 == 0u &&
            sourceEA == 0u) {
            continue;
        }

        LogText("实体["); LogUnsigned(i); LogText("] 指针="); LogPointer(entity);
        LogText(" X="); LogSigned(x); LogText(" Y="); LogSigned(y);
        LogText(" 打包状态0C="); LogHex32(*reinterpret_cast<const volatile DWORD*>(entity + 0x0C));
        LogText(" 打包状态18="); LogHex32(*reinterpret_cast<const volatile DWORD*>(entity + 0x18));
        LogText(" 控制器="); LogHex32(*reinterpret_cast<const volatile DWORD*>(entity + 0x6C));
        LogText(" 源实体="); LogHex32(sourceAddress);

        if (sourceReadable) {
            LogText(" 源+73="); LogHex8(source[0x73]);
            LogText(" 源+74="); LogHex8(source[0x74]);
            LogText(" 源+75="); LogHex8(source[0x75]);
            LogText(" 源+76="); LogHex8(source[0x76]);
            LogText(" 源+EA="); LogHex8(source[0xEA]);
            LogText(" 源+EB="); LogHex8(source[0xEB]);
        }
        LogCrLf();
    }
}

// ------------------------------- 九、完整快照 ----------------------------------

void CaptureSnapshot(void* runtimeEntry, DWORD originalResult) {
    ++gSnapshotSequence;

    LogText("\r\n================ 快照 #");
    LogUnsigned(gSnapshotSequence);
    LogText(" ================\r\n");
    LogNamedDword("原版存档许可结果", originalResult);
    LogNamedPointer("Hook.运行时记录参数", runtimeEntry);

    // 原版许可函数真正读取的是 runtimeEntry 的第一个 DWORD -> source MapSCI，
    // 再读取 source+0x380。把这个“真正原版判定输入”单独记下来，避免后面只看
    // selected record 时忘记：正常菜单的许可门实际取的是 first runtime entry。
    if (IsReadableRange(runtimeEntry, sizeof(DWORD))) {
        const DWORD gateSourceAddress =
            *reinterpret_cast<const volatile DWORD*>(runtimeEntry);
        const BYTE* gateSource =
            reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(gateSourceAddress));
        LogNamedPointer("Hook.原版许可源记录", gateSource);
        if (IsReadableRange(gateSource, 0x384u)) {
            LogNamedDword(
                "Hook.原版许可源记录+380",
                *reinterpret_cast<const volatile DWORD*>(gateSource + 0x380u));
        }
    }

    // 1) Event / movement 全局只记录，不预先把任何非零值解释成“必坏”。
    LogExeDword("全局.移动结果_468BF4", kMovementResultRva);
    LogExeByte("全局.输入导航门_468BB9", kInputNavigationGateRva);
    LogExeDword("事件.保存ID_89F7F8", kEventSavedIdRva);
    LogExeDword("事件.保存键A_89F7FC", kEventSavedKeyARva);
    LogExeDword("事件.保存键B_89F800", kEventSavedKeyBRva);
    LogExeDword("事件.活动ID_89F808", kActiveEventIdRva);
    LogExeDword("事件.运行态_89F810", kEventRuntime10Rva);
    LogExeDword("事件.运行态_89F818", kEventRuntime18Rva);
    LogExeByte("事件.VM继续_89F81D", kVmContinueByteRva);
    LogExeByte("事件.VM阻塞_89F81E", kVmBlockedByteRva);
    LogExeDword("事件.实体过渡计数_89F828", kEntityTransitionCountdownRva);
    LogExeDword("事件.当前命令_89F830", kCurrentCommandPointerRva);
    LogText("事件.89F7F0至89F83F原始字节=");
    LogHexRange(gExeBase + kEventRuntimeBlockRva, kEventRuntimeBlockBytes);
    LogCrLf();

    // 2) 当前真正写进存档列表的地点名原始字节。
    DWORD displayNameAddress = 0u;
    if (ReadExeDword(kCurrentLocationNamePointerRva, &displayNameAddress)) {
        const BYTE* displayName = reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(displayNameAddress));
        LogText("存档显示地点名.指针="); LogHex32(displayNameAddress); LogCrLf();
        LogText("存档显示地点名.前30字节=");
        LogHexRange(displayName, 30u);
        LogCrLf();
    } else {
        LogText("存档显示地点名=<指针不可读>\r\n");
    }

    // 3) DataCenter -> 当前 Map manager -> first / selected runtime entry。
    DWORD dataCenterAddress = 0u;
    const BYTE* mapManager = nullptr;
    const BYTE* firstEntry = nullptr;
    const BYTE* selectedEntry = nullptr;

    if (ReadExeDword(kDataCenterPointerRva, &dataCenterAddress)) {
        const BYTE* dataCenter = reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(dataCenterAddress));
        LogNamedPointer("数据中心", dataCenter);

        DWORD mapManagerAddress = 0u;
        // 只有 DataCenter 指针本身非空时才做 +0x280 指针运算。
        // C++ 对 nullptr 做指针加法属于未定义行为，即使最后不解引用也不应该这样写。
        if (dataCenter != nullptr && IsReadableRange(dataCenter + 0x280u, sizeof(DWORD))) {
            mapManagerAddress = *reinterpret_cast<const volatile DWORD*>(dataCenter + 0x280u);
            mapManager = reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(mapManagerAddress));
        }
        LogNamedPointer("地图.管理器", mapManager);

        if (IsReadableRange(mapManager, 0x14u)) {
            const DWORD recordCount = *reinterpret_cast<const volatile DWORD*>(mapManager + 0x00);
            const DWORD selectedIndex = *reinterpret_cast<const volatile DWORD*>(mapManager + 0x04);
            const DWORD rawSource = *reinterpret_cast<const volatile DWORD*>(mapManager + 0x0C);
            const DWORD runtimeArray = *reinterpret_cast<const volatile DWORD*>(mapManager + 0x10);

            LogNamedDword("地图.记录数量", recordCount);
            LogNamedDword("地图.当前记录索引", selectedIndex);
            LogNamedDword("地图.原始源缓冲", rawSource);
            LogNamedDword("地图.运行时记录数组", runtimeArray);

            firstEntry = reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(runtimeArray));
            // runtimeArray 为 0 时不能对空指针做“selectedIndex * 0x38”运算。
            if (firstEntry != nullptr && recordCount > 0u && selectedIndex < recordCount) {
                selectedEntry = firstEntry + static_cast<SIZE_T>(selectedIndex) * kMapRuntimeEntryStride;
            }
        }
    }

    LogMapRuntimeEntry(firstEntry, "地图.首运行时记录");
    LogMapRuntimeEntry(selectedEntry, "地图.当前运行时记录");

    // runtime entry 的第一个 DWORD 就是对应 source MapSCI record 指针。
    if (IsReadableRange(selectedEntry, sizeof(DWORD))) {
        const DWORD sourceAddress = *reinterpret_cast<const volatile DWORD*>(selectedEntry + 0x00);
        LogMapSciRecord(reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(sourceAddress)));

        // selected entry +0x10 已经在原 EXE 0x409A50 的碰撞链中直接传给 0x4080D0。
        // 这里只把控制器原始字节写到日志，绝对不调用碰撞函数。
        const DWORD mapCollisionControllerAddress =
            *reinterpret_cast<const volatile DWORD*>(selectedEntry + 0x10);
        const BYTE* mapCollisionController =
            reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(mapCollisionControllerAddress));
        LogNamedPointer("地图.当前记录+10碰撞控制器", mapCollisionController);
        LogText("地图.碰撞控制器原始字节=");
        if (IsReadableRange(mapCollisionController, kControllerDumpBytes)) {
            LogHexRange(mapCollisionController, kControllerDumpBytes);
        } else {
            LogText("<空指针或不可读>");
        }
        LogCrLf();
    }

    // 4) 当前受控角色 runtime。
    DWORD actorIndex = 0u;
    DWORD actorArrayAddress = 0u;
    const BYTE* player = nullptr;
    if (ReadExeDword(kControlledActorIndexRva, &actorIndex)) {
        LogNamedDword("玩家.受控角色索引", actorIndex);
    }
    if (ReadExeDword(kActorRuntimeArrayPointerRva, &actorArrayAddress)) {
        LogNamedDword("玩家.角色运行时数组", actorArrayAddress);
        const BYTE* actorArray = reinterpret_cast<const BYTE*>(static_cast<SIZE_T>(actorArrayAddress));
        // 同样先确认数组基址非空，再计算第 N 个 0x74-byte actor runtime。
        if (actorArray != nullptr && actorIndex < 256u) {
            player = actorArray + static_cast<SIZE_T>(actorIndex) * kActorStride;
        }
    }
    LogActorRuntime(player);

    // 5) 当前 selected map 的活动实体，重点保留玩家附近与碰撞/Event 相关实体。
    LogRelevantEntities(selectedEntry, player);

    LogText("================ 快照结束 #");
    LogUnsigned(gSnapshotSequence);
    LogText(" ================\r\n");
}

// -------------------------- 十、恢复真正原版访问器 ------------------------------

bool EnsureOriginalSaveGateFunction() {
    if (gExeBase == nullptr) {
        return false;
    }

    BYTE* functionStart = gExeBase + kOriginalSaveGateFunctionRva;
    if (ycr::BytesEqual(
            functionStart,
            kOriginalSaveGateFunctionBytes,
            sizeof(kOriginalSaveGateFunctionBytes))) {
        return true;
    }

    if (ycr::BytesEqual(
            functionStart,
            kOldUnsafeSaveGateFunctionBytes,
            sizeof(kOldUnsafeSaveGateFunctionBytes))) {
        LogText("[启动] 检测到历史无条件随时存档补丁；正在内存中恢复原版0x40A0C0。\r\n");
        return ycr::WriteBytes(
            functionStart,
            kOriginalSaveGateFunctionBytes,
            sizeof(kOriginalSaveGateFunctionBytes));
    }

    LogText("[启动失败] 原版存档许可函数位置出现未知机器码；未安装诊断Hook。\r\n");
    return false;
}

} // namespace

// ============================================================================
// ProbeSaveGate
// ----------------------------------------------------------------------------
// ABI 必须和原调用点匹配：runtimeEntry 从 ECX 进入，所以使用单参数 __fastcall。
//
// 关键安全点在最后一行：return originalResult。
// 无论日志写成什么、发现什么，本函数都不会把 0 改成 1，也不会把 1 改成 0。
// ============================================================================
extern "C" DWORD __fastcall ProbeSaveGate(void* runtimeEntry) {
    if (gOriginalSaveGate == nullptr || runtimeEntry == nullptr) {
        // 插件内部状态异常时也不能发明一个“允许存档”。
        return 0u;
    }

    const DWORD originalResult = gOriginalSaveGate(runtimeEntry);

    // 研究目标是“原版为什么拒绝、其中哪些点其实能安全开放”，所以只对原版拒绝点
    // 抓完整快照。原版允许点完全不纳入插件判定，也不需要制造大量日志。
    if (originalResult == 0u) {
        CaptureSnapshot(runtimeEntry, originalResult);
    }

    // 最重要的一行：原版结果原样返回。
    return originalResult;
}

// ============================================================================
// DllMain
// ============================================================================
extern "C" BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        OpenProbeLog(module);

        gExeBase = ycr::GetExeBase();
        if (gExeBase == nullptr) {
            LogText("[启动失败] 无法取得RPG.exe模块基址。\r\n");
            return TRUE;
        }

        LogText("[启动] RPG.exe模块基址="); LogPointer(gExeBase); LogCrLf();

        if (!EnsureOriginalSaveGateFunction()) {
            return TRUE;
        }

        gOriginalSaveGate = reinterpret_cast<OriginalSaveGateFunction>(
            gExeBase + kOriginalSaveGateFunctionRva);

        const bool installed = ycr::InstallRelativeCall(
            kNormalMenuSaveGateCallRva,
            kOriginalNormalMenuCall,
            reinterpret_cast<const void*>(&ProbeSaveGate));

        LogText(installed
            ? "[启动] 只读诊断CALL Hook已安装；存档许可仍完全使用原版结果。\r\n"
            : "[启动失败] Hook位置机器码不匹配；未安装诊断插件。\r\n");
    }

    if (reason == DLL_PROCESS_DETACH) {
        if (gLogFile != INVALID_HANDLE_VALUE) {
            LogText("[退出] 随时存档只读诊断插件卸载。\r\n");
            CloseHandle(gLogFile);
            gLogFile = INVALID_HANDLE_VALUE;
        }
    }

    return TRUE;
}
