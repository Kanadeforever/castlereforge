#include "PatchUtil.h"
#include "PluginLog.h"

// ============================================================================
// NoCD.asi
// ----------------------------------------------------------------------------
// 目标：去掉启动时的 PlayDisk/CD-ROM 检查，但不破坏地图、影片等正常资源读取流程。
//
// 重要：这里没有原样照抄流传二十多年的旧免CD三字节改法。
// 旧补丁会：
// 1. 把“驱动器类型必须是 CD-ROM(5)”改成“驱动器类型 >= 2”；
// 2. 随后无条件接受第一个这样的驱动器。
// 因为扫描从 A: 开始，所以现代机器通常最终把备用光盘盘符写成 C:。
//
// 旧补丁长期能工作的真正原因，是游戏后面的资源代码会先访问：
//     ..\MultiMedia\Map\...
//     ..\MultiMedia\Mov\...
// 本地文件不存在时，才回退到：
//     %c:\MultiMedia\...
// 因此那个错误/粗糙的 C: 盘符大多没有被实际使用。
//
// 本插件采用更干净的等价方案：
// - 直接让“扫描光驱并提示插盘”的函数立刻 return；
// - 把备用盘符写成 RPG.exe 自己所在的驱动器，而不是随便选 C:；
// - 完全不修改后续地图/影片的本地优先读取逻辑。
// ============================================================================

namespace {

// 0x00402A10 是纯粹的“扫描 A:~Z:、找 CD-ROM、检查 \MultiMedia\Mov、失败时提示
// Please insert PlayDisk(4th)”函数入口。
// 原函数第一条指令是：83 EC 38  => sub esp, 0x38。
// 我们只把第一个字节 83 改成 C3(ret)，函数就会在任何扫描动作发生前直接返回。
const BYTE kScanOriginal[] = {0x83};
const BYTE kScanPatched[]  = {0xC3};
const ycr::Patch kSkipCdScan = {
    0x00002A10u,
    kScanOriginal,
    kScanPatched,
    1
};

// 因为真正写入时只需要把函数入口的第一个字节改成 RET，单看“0x83”一个字节
// 作为版本判断还不够严格。因此再保存入口后续 15 字节作为“身份指纹”。
// 旧历史免CD只改了本函数更后面的 0x00402A41/0x00402A42/0x00402A62，
// 所以这个入口指纹同时兼容原始 EXE 和旧免CD EXE。
const BYTE kCdScanTailSignature[] = {
    0xEC, 0x38, 0x53, 0x55, 0x8B, 0x2D, 0x24, 0x01,
    0x46, 0x00, 0x56, 0x57, 0x8B, 0x3D, 0x94
};

bool IsCdScanFunctionRecognized() {
    BYTE* base = ycr::GetExeBase();
    if (base == nullptr) {
        return false;
    }

    BYTE* functionStart = base + 0x00002A10u;

    // 第一个字节允许是原始 0x83，也允许已经被本插件改成 0xC3。
    if (functionStart[0] != 0x83 && functionStart[0] != 0xC3) {
        return false;
    }

    // 再核对后面 15 字节，避免仅凭一个常见的 0x83 就误判版本。
    return ycr::BytesEqual(
        functionStart + 1,
        kCdScanTailSignature,
        sizeof(kCdScanTailSignature));
}

// 原版把检测到的光盘盘符字符保存在 0x0046F5C8。
// 用 RVA 表示就是 0x0006F5C8。
const DWORD kCdDriveLetterRva = 0x0006F5C8u;

// 从 RPG.exe 的完整路径里拿到“D:\...”前面的 D。
// 如果是极少见的 UNC/特殊路径，没有标准“X:”格式，就退回到 C。
BYTE DetectExeDriveLetter() {
    wchar_t path[1024];
    path[0] = L'\0';
    const DWORD length = GetModuleFileNameW(nullptr, path, 1024u);

    if (length >= 2u && path[1] == L':') {
        wchar_t letter = path[0];

        // 把小写 a~z 转成大写 A~Z，只做最简单的 ASCII 范围转换。
        if (letter >= L'a' && letter <= L'z') {
            letter = static_cast<wchar_t>(letter - L'a' + L'A');
        }

        if (letter >= L'A' && letter <= L'Z') {
            return static_cast<BYTE>(letter);
        }
    }

    return static_cast<BYTE>('C');
}

// 把备用盘符写进 RPG.exe 的 .data 区。
// .data 本来就可写，所以这里不需要 VirtualProtect。
void StoreFallbackDriveLetter(BYTE driveLetter) {
    BYTE* base = ycr::GetExeBase();
    if (base != nullptr) {
        base[kCdDriveLetterRva] = driveLetter;
    }
}

}  // namespace

extern "C" BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);

        // 每次启动都新建/截断 NoCD.log。这样日志只反映“这一轮游戏”的状态，
        // 不会把几次测试混在一起。
        ycrlog::Open(module, L"NoCD.log");
        ycrlog::Line("《幽城幻剑录》免 CD 插件 v0.3.1a 启动。");
        ycrlog::Line("ASI插件化 By Luminous with ChatGPT");
        ycrlog::Line("原始文件来自“汉堂之家”坛友武英仲分享的三合一、五合一补丁");

        // 先用 16 字节入口指纹确认这里确实是目标 CD 扫描函数，
        // 再执行 1 字节 RET 补丁。两个条件都成功后，才允许写备用盘符。
        const bool recognized = IsCdScanFunctionRecognized();
        const bool patched = recognized && ycr::SetPatchSetState(&kSkipCdScan, 1, true);
        if (patched) {
            // 备用盘符只用于原版资源读取链的兜底路径；本地 ../MultiMedia 读取逻辑没有修改。
            const BYTE driveLetter = DetectExeDriveLetter();
            StoreFallbackDriveLetter(driveLetter);
            ycrlog::Line("[免CD] 原版光盘扫描函数已安全跳过。");
            ycrlog::Text("[免CD] 备用盘符已设为 RPG.exe 所在驱动器：");
            ycrlog::Character(static_cast<char>(driveLetter));
            ycrlog::Line(":");
            ycrlog::Line("[资源] 原版本地 MultiMedia 优先读取流程保持不变。");
        } else if (!recognized) {
            ycrlog::Line("[失败] 光盘扫描函数入口指纹不匹配；为避免误改未知版本，未安装免CD补丁。");
        } else {
            ycrlog::Line("[失败] 光盘扫描函数已识别，但内存补丁写入失败。");
        }
    } else if (reason == DLL_PROCESS_DETACH) {
        ycrlog::Line("[退出] 免 CD 插件卸载。");
        ycrlog::Close();
    }

    return TRUE;
}
