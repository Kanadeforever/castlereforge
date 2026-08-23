#include "PatchUtil.h"
#include "PluginLog.h"

// ============================================================================
// MaxGrowthAndDrop.asi
// ----------------------------------------------------------------------------
// 把旧五合一补丁里的“最大成长”和“最大掉宝”拆成两个互相独立的开关。
//
// 配置文件：MaxGrowthAndDrop.ini
// - MaxGrowth=1：最大成长（默认开启）
// - MaxGrowth=0：恢复原版随机成长
// - MaxDrop=1：最大掉宝（默认开启）
// - MaxDrop=0：恢复原版掉宝概率
//
// 如果 INI 不存在，ASI 会在自身目录自动生成一份，并默认两项都为 1。
// ============================================================================

namespace {

// ------------------------------- 最大掉宝 -------------------------------------
// 原版掉落判断在 0x00443A1D 附近：
//     call random
//     cdq
//     mov ecx, 100
//     idiv ecx
//     cmp edx, [esi+0x10]
//
// idiv 100 后，EDX 是 0~99 的随机余数，用它与掉落概率比较。
// 旧五合一把“计算随机余数”改成“直接令 EDX=1”，因此只要条目的概率阈值允许，
// 就稳定走向掉落成功分支。这一组改动与升级成长函数完全分离，所以可以独立开关。
const BYTE kDropOriginal0[] = {0xB9, 0x64};
const BYTE kDropPatched0[]  = {0xBA, 0x01};
const BYTE kDropOriginal1[] = {0xF7, 0xF9};
const BYTE kDropPatched1[]  = {0x90, 0x90};

const ycr::Patch kDropPatches[] = {
    {0x00043A23u, kDropOriginal0, kDropPatched0, 2},
    {0x00043A28u, kDropOriginal1, kDropPatched1, 2},
};

// ------------------------------- 最大成长 -------------------------------------
// 升级函数会多次调用同一个随机数函数 0x00451565。
// 第一、第二处原本都通过“随机数 % 3”得到 0、1、2 三种浮动；旧五合一直接把
// EDX 固定为 2，也就是永远选随机范围中的最大余数。
const BYTE kGrowthOriginal0[] = {0xB9, 0x03};
const BYTE kGrowthPatched0[]  = {0xBA, 0x02};
const BYTE kGrowthOriginal1[] = {0xF7, 0xF9};
const BYTE kGrowthPatched1[]  = {0x90, 0x90};
const BYTE kGrowthOriginal2[] = {0xB9, 0x03};
const BYTE kGrowthPatched2[]  = {0xBA, 0x02};
const BYTE kGrowthOriginal3[] = {0xF7, 0xF9};
const BYTE kGrowthPatched3[]  = {0x90, 0x90};

// 0x00443D40 是一个“按角色/成长项目查表，然后在最小值~最大值之间随机取值”的函数。
// 原版尾部：
//     idiv esi          ; EDX = 随机余数
//     mov eax, edx
//     add eax, edi      ; 最小值 + 随机余数
// 旧五合一改成：
//     lea eax, [esi+edi-1]
// 也就是直接返回“最小值 + 范围宽度 - 1”，即该表项允许的最大成长值。
const BYTE kGrowthOriginal4[] = {0xF7, 0xFE, 0x8B, 0xC2, 0x03, 0xC7};
const BYTE kGrowthPatched4[]  = {0x8D, 0x44, 0x3E, 0xFF, 0x90, 0x90};

const ycr::Patch kGrowthPatches[] = {
    {0x00043BBAu, kGrowthOriginal0, kGrowthPatched0, 2},
    {0x00043BBFu, kGrowthOriginal1, kGrowthPatched1, 2},
    {0x00043BFCu, kGrowthOriginal2, kGrowthPatched2, 2},
    {0x00043C03u, kGrowthOriginal3, kGrowthPatched3, 2},
    {0x00043D7Du, kGrowthOriginal4, kGrowthPatched4, 6},
};

// ------------------------------- INI 处理 -------------------------------------
const wchar_t kIniFileName[] = L"MaxGrowthAndDrop.ini";
const wchar_t kIniSection[]  = L"MaxGrowthAndDrop";
const wchar_t kGrowthKey[]   = L"MaxGrowth";
const wchar_t kDropKey[]     = L"MaxDrop";

// 默认 INI 使用 UTF-16LE，并在第一个字符写入 BOM(U+FEFF)。
// 这样 Windows 的 GetPrivateProfileIntW 可以稳定读取中文注释和 ASCII 配置键。
const wchar_t kDefaultIniText[] =
    L"\uFEFF; 幽城幻剑录 - 最大成长 / 最大掉宝设置\r\n"
    L"; 数值说明：1 = 开启，0 = 关闭。\r\n"
    L"; 两项默认都开启；修改后重新启动游戏即可生效。\r\n"
    L"\r\n"
    L"[MaxGrowthAndDrop]\r\n"
    L"; 最大成长：升级时把随机成长结果固定到旧五合一补丁使用的最大值。\r\n"
    L"MaxGrowth=1\r\n"
    L"\r\n"
    L"; 最大掉宝：把旧五合一的掉落随机判断固定到高掉落结果。\r\n"
    L"MaxDrop=1\r\n";

// 把 ASI 自己的完整路径改造成同目录下的 MaxGrowthAndDrop.ini。
// 例如：
//   D:\Game\MaxGrowthAndDrop.asi
// 变成：
//   D:\Game\MaxGrowthAndDrop.ini
bool BuildIniPath(HMODULE module, wchar_t* outPath, DWORD capacity) {
    const DWORD length = GetModuleFileNameW(module, outPath, capacity);
    if (length == 0u || length >= capacity) {
        return false;
    }

    // 从末尾往前找最后一个 '\\' 或 '/'，它后面就是原 ASI 文件名。
    DWORD fileNameStart = length;
    while (fileNameStart > 0u) {
        const wchar_t ch = outPath[fileNameStart - 1u];
        if (ch == L'\\' || ch == L'/') {
            break;
        }
        --fileNameStart;
    }

    const SIZE_T iniNameLength = ycr::WideLength(kIniFileName);
    if (fileNameStart + iniNameLength + 1u > capacity) {
        return false;
    }

    // 把旧文件名覆盖为固定的新文件名。
    for (SIZE_T i = 0; i < iniNameLength; ++i) {
        outPath[fileNameStart + i] = kIniFileName[i];
    }
    outPath[fileNameStart + iniNameLength] = L'\0';
    return true;
}

// INI 不存在时创建默认文件。
// CREATE_NEW 的好处是：如果用户文件恰好在我们检查后、创建前出现，也不会覆盖它。
void CreateDefaultIniIfMissing(const wchar_t* iniPath) {
    if (GetFileAttributesW(iniPath) != INVALID_FILE_ATTRIBUTES) {
        return;
    }

    HANDLE file = CreateFileW(
        iniPath,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD written = 0;
    const DWORD bytesToWrite = static_cast<DWORD>(
        ycr::WideLength(kDefaultIniText) * sizeof(wchar_t));

    WriteFile(file, kDefaultIniText, bytesToWrite, &written, nullptr);
    CloseHandle(file);
}

// 任何非 0 值都按“开启”处理，0 才是关闭。
// 如果 INI 无法读取，defaultValue=1 保证行为仍然符合“默认全部开启”的要求。
bool ReadSwitch(const wchar_t* iniPath, const wchar_t* key) {
    return GetPrivateProfileIntW(kIniSection, key, 1, iniPath) != 0u;
}

}  // namespace

extern "C" BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);

        // 与其它插件统一：每次启动先清空 MaxGrowthAndDrop.log，再写本轮配置和补丁结果。
        ycrlog::Open(module, L"MaxGrowthAndDrop.log");
        ycrlog::Line("《幽城幻剑录》最大成长 / 最大掉宝插件 v0.3.1a 启动。");
        ycrlog::Line("ASI插件化 By Luminous with ChatGPT");
        ycrlog::Line("原始文件来自“汉堂之家”坛友武英仲分享的五合一补丁");

        wchar_t iniPath[1024];
        iniPath[0] = L'\0';
        if (BuildIniPath(module, iniPath, 1024u)) {
            // INI 不存在时自动生成；已经存在时绝不覆盖用户设置。
            CreateDefaultIniIfMissing(iniPath);

            const bool enableGrowth = ReadSwitch(iniPath, kGrowthKey);
            const bool enableDrop   = ReadSwitch(iniPath, kDropKey);

            ycrlog::Text("[配置] 最大成长：");
            ycrlog::Line(enableGrowth ? "开启" : "关闭");
            ycrlog::Text("[配置] 最大掉宝：");
            ycrlog::Line(enableDrop ? "开启" : "关闭");

            // 这里不是“开启时才写，关闭时什么都不做”，而是主动切换状态：
            // - 开启：把原始机器码改成旧五合一的目标机器码；
            // - 关闭：如果用户的 EXE 本身已经带旧五合一修改，也会在内存中恢复原版。
            // 因此两个开关在原始 EXE 和旧五合一 EXE 上都具有明确语义。
            const bool growthOk = ycr::SetPatchSetState(
                kGrowthPatches,
                sizeof(kGrowthPatches) / sizeof(kGrowthPatches[0]),
                enableGrowth);

            const bool dropOk = ycr::SetPatchSetState(
                kDropPatches,
                sizeof(kDropPatches) / sizeof(kDropPatches[0]),
                enableDrop);

            ycrlog::Line(growthOk
                ? "[补丁] 最大成长状态已按 INI 成功应用。"
                : "[失败] 最大成长目标机器码不匹配或写入失败，未盲目覆盖。");
            ycrlog::Line(dropOk
                ? "[补丁] 最大掉宝状态已按 INI 成功应用。"
                : "[失败] 最大掉宝目标机器码不匹配或写入失败，未盲目覆盖。");
        } else {
            // 极端情况下拿不到 ASI 路径时，仍遵守“默认两项开启”。
            ycrlog::Line("[警告] 无法构造 MaxGrowthAndDrop.ini 路径，按默认值：两项全部开启。");
            const bool growthOk = ycr::SetPatchSetState(
                kGrowthPatches,
                sizeof(kGrowthPatches) / sizeof(kGrowthPatches[0]),
                true);
            const bool dropOk = ycr::SetPatchSetState(
                kDropPatches,
                sizeof(kDropPatches) / sizeof(kDropPatches[0]),
                true);
            ycrlog::Line(growthOk ? "[补丁] 最大成长已按默认值开启。" : "[失败] 最大成长补丁未能安全应用。");
            ycrlog::Line(dropOk ? "[补丁] 最大掉宝已按默认值开启。" : "[失败] 最大掉宝补丁未能安全应用。");
        }
    } else if (reason == DLL_PROCESS_DETACH) {
        ycrlog::Line("[退出] 最大成长 / 最大掉宝插件卸载。");
        ycrlog::Close();
    }

    return TRUE;
}
