#include "PatchUtil.h"
#include "PluginLog.h"
#include "CastleRuntime_Client.h"
#include "CastleHook_API.h"
#include "CastlePath_API.h"
#include "Castle_PadSupport_API.h"

// ============================================================================
// Castle_SaveEnhance.cpp  v0.1.0-test7
// ----------------------------------------------------------------------------
// 《幽城幻剑录》存档增强插件第一版完整实机候选。
//
// 本文件把此前 AnytimeSave 已经有实机正向结果的“安全扩展存档”继续保留下来，同时把
// 用户确定的新 SaveEnhance 规则全部合并到一个插件：
//
//   槽 0       = Quick Save，只允许 F5 / Controller 快速存档写入，普通菜单只能读；
//   槽 1~90    = Manual Save，普通保存菜单正常读写；
//   槽 91~99   = 9 个滚动 Auto Save，普通菜单只能读；
//
//   F5         = 快速存档到 0；
//   F9 × N     = 快速读取 0，默认 1200ms 内连续按两次；
//   RB + R3    = Controller 快速存档；
//   RB + Start = Controller 快速读档请求，同样进入 N 次确认状态机。
//
//   自动存档  = 真正换地图 + 可配置分钟间隔；91~99 有空槽先填空，全满后按持久化环形游标覆盖；
//   存档页面  = 原版 8 页×4 扩成 25 页×4，编号改成 0~99；第一页上一页跳最后一页，
//               最后一页下一页跳第一页；
//   保留槽 UI = 0、91~99 打开“取消/读档/存档”三项窗口时，把“存档”按钮标为原版 disabled，
//               所以原版鼠标/键盘和 Castle_PadSupport 当前 SaveAction Event 都拒绝它；
//               另外 SaveSlot 调用层再做一次二次保险，避免未知输入路径绕过 UI。
//
//   声音       = 只支持可选外置 WAV。INI 填文件名才尝试播放；空、非法、文件不存在或
//               winmm 播放失败都只是“没有声音”，绝不能让保存/读档本身失败；
//               [Sound] Volume=0~100 只缩放 SaveEnhance 自己的 WAV，不动游戏/Windows 总音量。
//
// 重要安全边界：
// 1. 普通保存菜单继续保留 AnytimeSave 的安全 fallback：原版禁存但当前地图已有入口级
//    安全锚点时，只在原版 Writer 调用期间临时替换 World 前 0xC8 字节；
// 2. Quick Save 仍然只允许严格自由行动；但实机证明《幽城》即使在自由行动时原版 save gate
//    也可能返回拒绝。因此 test5 在“严格自由行动 + 当前地图已冻结安全锚点”时允许复用安全
//    fallback 写 0 号；楼梯/剧情 transient 本身过不了严格自由行动门，仍然不会被 Quick Save 写入；
// 3. Auto Save 可以延后。换图/到时如果当前不安全，就等新地图建立安全锚点以后再补存；
// 4. Quick Load 同样只接受自由行动状态下的请求；第一次看到地图、刚刚读档后的第一张图
//    都只建立基准，不马上自动覆盖 91~99；
// 5. Save001~Save159 流程档是外部人工编号样本，绝不能再拿来证明原版槽位容量。
//
// 当前状态：所有下面写死的地址都基于台湾第三版原版 RPG.exe：
// SHA-256 8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f
// 本源码会在写任何 Hook 前验证关键机器码；未知 EXE / 冲突 MOD 一律 fail-closed。
// ============================================================================

// ============================================================================
// 一、无 CRT 编译器兜底
// ============================================================================
// /NODEFAULTLIB 构建时，编译器仍可能把结构体复制优化成 memcpy/memset。因此自己提供最小
// 逐字节实现，并使用 volatile 阻止“这个实现本身又被优化回 memcpy”的递归陷阱。
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

namespace {

// ============================================================================
// 二、目标 EXE 地址：所有 RVA 都以 RPG.exe ImageBase 0x00400000 为基准
// ============================================================================

// ---- 旧安全随时存档三条核心 Hook -------------------------------------------
const DWORD kNormalMenuSaveGateCallRva = 0x0000CCC2u; // 0x40CCC2
const BYTE kNormalMenuSaveGateCallBytes[5] = {0xE8, 0xF9, 0xD3, 0xFF, 0xFF};
const DWORD kMapTickCallRva = 0x0000B078u;             // 0x40B078
const BYTE kMapTickCallBytes[5] = {0xE8, 0x63, 0xFD, 0xFF, 0xFF};
const DWORD kSaveWriterCallRva = 0x0003B34Cu;          // 0x43B34C
const BYTE kSaveWriterCallBytes[5] = {0xE8, 0x0F, 0x00, 0x00, 0x00};

const DWORD kOriginalSaveGateFunctionRva = 0x0000A0C0u; // 0x40A0C0
const BYTE kOriginalSaveGateFunctionBytes[9] = {
    0x8B, 0x01, 0x8B, 0x80, 0x80, 0x03, 0x00, 0x00, 0xC3};
const BYTE kOldUnsafeSaveGateFunctionBytes[9] = {
    0x8B, 0x01, 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90, 0xC3};

const DWORD kOriginalMapTickFunctionRva = 0x0000ADE0u;    // 0x40ADE0
const DWORD kOriginalSaveWriterFunctionRva = 0x0003B360u;// 0x43B360
const DWORD kOriginalSaveSlotFunctionRva = 0x0003B320u;  // 0x43B320
const DWORD kOriginalLoadSlotFunctionRva = 0x0003B4D0u;  // 0x43B4D0
const DWORD kOriginalSavePrepareFunctionRva = 0x0004B150u;// 0x44B150
const DWORD kOriginalPostLoadFunctionRva = 0x0004B1F0u;  // 0x44B1F0

// ---- 原版文件系统对象：自动档轮换必须和游戏自己看到同一套 Save 文件 ------------
// test4 实机已经证明：即使字符串是同一个 `Save\Save091.TSF`，Win32 的
// GetFileAttributesExW 仍然看不到原版成功写出的文件。原因是 0x43B360 Writer 并不
// 直接调用 Win32 文件 API，而是 new 一个 0x14 字节的游戏 File 对象，再经过这里三条函数。
// 因此 test5 检查 91~99 是否存在时，直接复用 LoadSlot 相同的 File::Open(mode=1) 路径。
const DWORD kGameFileCtorFunctionRva = 0x000416F0u; // 0x4416F0，this + 一个栈参数 archiveAware
const DWORD kGameFileDtorFunctionRva = 0x00041710u; // 0x441710，析构并关闭文件
const DWORD kGameFileOpenFunctionRva = 0x000417C0u; // 0x4417C0，this + path + mode + flags
const SIZE_T kGameFileObjectBytes = 0x14u;

// ---- 普通/隐藏手动保存入口：用于保留槽二次写保护 -----------------------------
const DWORD kMenuSaveCallRva = 0x00024DF2u; // 0x424DF2
const BYTE kMenuSaveCallBytes[5] = {0xE8, 0x29, 0x65, 0x01, 0x00};
const DWORD kCommandSaveCallRva = 0x0004A82Du; // 0x44A82D
const BYTE kCommandSaveCallBytes[5] = {0xE8, 0xEE, 0x0A, 0xFF, 0xFF};

// ---- SaveSlot owner 的只读定位 ------------------------------------------------
// test1 为了知道 SaveAction 属于哪个 SaveSlot，曾 Hook 三个 SaveSlot constructor CALL。
// 用户实机启动时“整组预检查失败”，而旧日志没有指出具体地址；这三个 Hook 本身并非功能所必需。
// 当前 Controller 工程已经把三个稳定 owner 路径闭合，因此 test2 改为在 SaveAction Update 当帧
// 只读这三个 owner，比较 SaveSlot+0x5A4 是否等于当前 action。这样不改 constructor，不抢其它
// 插件可能已经使用的创建路径，也减少三个必须保持原版机器码的启动前提。
const SIZE_T kTitleSaveSlotOffset = 0x5B4u;      // [0x008E241C] + 0x5B4
const SIZE_T kInterfaceSaveSlotOffset = 0x654u;  // [0x008DED0C] + 0x654，Interface state7 的 SaveSlot
const SIZE_T kSavePointSaveSlotOffset = 0x580u;  // [0x0089FCD0] + 0x580

// ---- SaveAction vtable Update ------------------------------------------------
// SaveAction constructor 0x425FE0 写 vtable=0x460B90。
// vtable+0x18 = 0x460BA8 原来指向 0x4262C0。我们只改这个虚函数指针，不碰 Controller
// 已经 Hook 的 0x426365/0x426387 Button HitTest/Event 调用点。
const DWORD kSaveActionUpdateVtableRva = 0x00060BA8u; // abs 0x460BA8
const DWORD kSaveActionUpdateOriginalVa = 0x004262C0u;
const BYTE kSaveActionUpdateVtableOriginalBytes[4] = {0xC0, 0x62, 0x42, 0x00};

// SaveAction 内部两个 CALL 是保留槽手柄视觉/确认的唯一游戏级接入点。
// SaveEnhance 不修改 PadSupport 模块；只在确认 Public API v1 可用后，把“当前 CALL 目标”保存为
// next，再让自己的 wrapper 排在外层。普通槽始终完整调用 next，插件之间仍通过公开边界协作。
const DWORD kSaveActionHitCallRva = 0x00026365u;   // abs 0x426365
const DWORD kSaveActionEventCallRva = 0x00026387u; // abs 0x426387
const DWORD kOriginalButtonHitVa = 0x00431310u;
const DWORD kOriginalButtonEventVa = 0x00431380u;

// ---- 100 槽固定机器码 -------------------------------------------------------
const BYTE kLoadSlotIndexOriginal[4]       = {0x8D, 0x54, 0x01, 0x01};
const BYTE kLoadSlotIndexZeroBased[4]      = {0x8D, 0x54, 0x01, 0x00};
const BYTE kSaveSlotIndexOriginal[4]       = {0x8D, 0x4C, 0x02, 0x01};
const BYTE kSaveSlotIndexZeroBased[4]      = {0x8D, 0x4C, 0x02, 0x00};
const BYTE kFocusedIndexOriginal[4]        = {0x8D, 0x54, 0x02, 0x01};
const BYTE kFocusedIndexZeroBased[4]       = {0x8D, 0x54, 0x02, 0x00};
const BYTE kMetadataIndexOriginal[4]       = {0x8D, 0x54, 0x10, 0x01};
const BYTE kMetadataIndexZeroBased[4]      = {0x8D, 0x54, 0x10, 0x00};
const BYTE kSelectedLabelOriginal[4]       = {0x8D, 0x4C, 0x10, 0x01};
const BYTE kSelectedLabelZeroBased[4]      = {0x8D, 0x4C, 0x10, 0x00};
const BYTE kRowLabelOriginal[4]            = {0x8D, 0x54, 0x11, 0x01};
const BYTE kRowLabelZeroBased[4]           = {0x8D, 0x54, 0x11, 0x00};
const BYTE kPageCountNextOriginal[6]       = {0x8B, 0x8A, 0xC2, 0x03, 0x00, 0x00};
const BYTE kPageCountNext25[6]             = {0xB9, 0x19, 0x00, 0x00, 0x00, 0x90};
const BYTE kPageCountArrowOriginal[6]      = {0x8B, 0x91, 0xC2, 0x03, 0x00, 0x00};
const BYTE kPageCountArrow25Fixed[6]       = {0xBA, 0x19, 0x00, 0x00, 0x00, 0x90};
const BYTE kPrevArrowBoundaryOriginal[2]   = {0x7E, 0x11};
const BYTE kPrevArrowBoundaryLoop[2]       = {0x90, 0x90};
const BYTE kNextArrowBoundaryOriginal[2]   = {0x7D, 0x11};
const BYTE kNextArrowBoundaryLoop[2]       = {0x90, 0x90};

const ycr::Patch kFixedMenuPatches[] = {
    {0x00024D8Du, kLoadSlotIndexOriginal,      kLoadSlotIndexZeroBased,      4u},
    {0x00024DE7u, kSaveSlotIndexOriginal,      kSaveSlotIndexZeroBased,      4u},
    {0x00025324u, kFocusedIndexOriginal,       kFocusedIndexZeroBased,       4u},
    {0x000253C0u, kMetadataIndexOriginal,      kMetadataIndexZeroBased,      4u},
    {0x00025486u, kSelectedLabelOriginal,      kSelectedLabelZeroBased,      4u},
    {0x000255BAu, kRowLabelOriginal,           kRowLabelZeroBased,           4u},
    {0x00024ECBu, kPageCountNextOriginal,      kPageCountNext25,             6u},
    {0x0002565Au, kPageCountArrowOriginal,     kPageCountArrow25Fixed,       6u},
    {0x00025626u, kPrevArrowBoundaryOriginal,  kPrevArrowBoundaryLoop,       2u},
    {0x00025665u, kNextArrowBoundaryOriginal,  kNextArrowBoundaryLoop,       2u},
};
const SIZE_T kFixedMenuPatchCount = sizeof(kFixedMenuPatches) / sizeof(kFixedMenuPatches[0]);

// ---- 循环分页的两个 6 字节原版 mov eax,[esi+598] ----------------------------
const DWORD kPrevPageBaseReadRva = 0x00024E90u; // 0x424E90
const BYTE kPrevPageBaseReadBytes[6] = {0x8B, 0x86, 0x98, 0x05, 0x00, 0x00};
const DWORD kNextPageBaseReadRva = 0x00024EC2u; // 0x424EC2
const BYTE kNextPageBaseReadBytes[6] = {0x8B, 0x86, 0x98, 0x05, 0x00, 0x00};

// ============================================================================
// 三、运行时全局与结构偏移
// ============================================================================
const DWORD kDataCenterPointerRva = 0x0049F804u;       // abs 0x0089F804
const DWORD kActorRuntimeArrayPointerRva = 0x0049F7F0u;// abs 0x0089F7F0
const DWORD kControlledActorIndexRva = 0x00068BF0u;    // abs 0x00468BF0
const DWORD kWorldGlobalPointerRva = 0x00578508u;      // abs 0x00978508
const DWORD kRuntimeManagerPointerRva = 0x004E1C48u;   // abs 0x008E1C48

// 严格自由行动门使用的已确认全局。
const DWORD kMapBusyRva = 0x0049F808u;           // abs 0x0089F808；原版 Space handler 也检查它==0。
const DWORD kBattleUiRva = 0x0049FD74u;          // abs 0x0089FD74
const DWORD kTitleUiRva = 0x004E241Cu;           // abs 0x008E241C
const DWORD kInterfaceUiRva = 0x004DED0Cu;       // abs 0x008DED0C
const DWORD kSavePointUiRva = 0x0049FCD0u;       // abs 0x0089FCD0
const DWORD kMovieObjectRva = 0x0006F390u;       // abs 0x0046F390；是指针，object+0x0A 为 active byte。
const DWORD kDialogueIdRva = 0x0006F670u;        // abs 0x0046F670；0 表示当前没有对话 ID。

const SIZE_T kDataCenterSceneContainerOffset = 0x280u;
const SIZE_T kMapRecordCountOffset = 0x00u;
const SIZE_T kMapSelectedIndexOffset = 0x04u;
const SIZE_T kMapRawSourceOffset = 0x0Cu;
const SIZE_T kMapRuntimeEntryOffset = 0x10u;
const SIZE_T kMapSciRecordSize = 0x473u;
const DWORD kMaxReasonableMapRecords = 128u;

const SIZE_T kActorStride = 0x74u;
const SIZE_T kActorXOffset = 0x10u;
const SIZE_T kActorYOffset = 0x14u;
const SIZE_T kActorStateByteOffset = 0x1Au;

const SIZE_T kRuntimeManagerWorldLengthOffset = 0x13Cu;
const SIZE_T kRuntimeManagerWorldPointerOffset = 0x140u;
const DWORD kExpectedWorldLength = 0x27D8u;
const SIZE_T kWorldHeaderBytes = 0x00C8u;
const SIZE_T kWorldXOffset = 0x32u;
const SIZE_T kWorldYOffset = 0x36u;
const SIZE_T kWorldStateOffset = 0x3Au;
const SIZE_T kWorldActorIndexOffset = 0x3Eu;

// SaveSlot 对象内部存档页字段。
const SIZE_T kSaveSlotSelectedRowOffset = 0x594u;
const SIZE_T kSaveSlotPageBaseOffset = 0x598u;
const SIZE_T kSaveSlotActionPointerOffset = 0x5A4u;

// SaveAction 的三个按钮指针从 +0x58C 开始；index2 = “存档”，所以它就在 +0x594。
const SIZE_T kSaveActionButtonsOffset = 0x58Cu;
const DWORD kSaveActionButtonCount = 3u;
const DWORD kSaveActionCancelIndex = 0u;
const DWORD kSaveActionLoadIndex = 1u;
const DWORD kSaveActionSaveIndex = 2u;
const SIZE_T kSaveActionSaveButtonPointerOffset = 0x594u;
const SIZE_T kButtonDisabledOffset = 0x04u;

// 固定槽位策略。
const DWORD kQuickSaveSlot = 0u;
const DWORD kAutoSlotFirst = 91u;
const DWORD kAutoSlotLast = 99u;
const DWORD kLastPageBase = 96u;

// ============================================================================
// 四、原版函数类型
// ============================================================================
typedef DWORD (__fastcall *OriginalSaveGateFunction)(void* runtimeEntry);
typedef void (__fastcall *OriginalMapTickFunction)(void* sceneContainer, void* unusedEdx);
typedef BOOL (__fastcall *OriginalSaveWriterFunction)(void* runtimeManager, void* unusedEdx, const char* path);
typedef BOOL (__fastcall *OriginalSaveSlotFunction)(void* runtimeManager, void* unusedEdx, DWORD slot);
typedef BOOL (__fastcall *OriginalLoadSlotFunction)(void* runtimeManager, void* unusedEdx, DWORD slot);
typedef void (__cdecl *OriginalNoArgFunction)(void);
typedef void (__fastcall *OriginalSaveActionUpdateFunction)(void* action, void* unusedEdx);
typedef BYTE (__fastcall *SaveActionHitFunction)(void* button, void* unusedEdx);
typedef LONG (__fastcall *SaveActionEventFunction)(void* button, void* unusedEdx);

// 游戏 File 对象是 MSVC x86 thiscall。源码用 __fastcall + 一个占位 EDX 来表达：
// 第一个参数进 ECX，第二个 unusedEdx 进 EDX，剩余参数仍按原版顺序压栈。
typedef void* (__fastcall *GameFileCtorFunction)(void* fileObject, void* unusedEdx, DWORD archiveAware);
typedef void (__fastcall *GameFileDtorFunction)(void* fileObject, void* unusedEdx);
typedef BOOL (__fastcall *GameFileOpenFunction)(
    void* fileObject, void* unusedEdx, const char* path, DWORD mode, DWORD flags);

BYTE* gExeBase = nullptr;
HMODULE gSelfModule = nullptr;

// Castle Mod Loader 当前会在 LoadLibraryExW 返回后主动调用导出的 InitializeASI。
// 这个标记只防止调试环境/其它兼容加载器重复调用正式初始化，避免同一批 Hook 被打两遍。
// Mod Loader 的 ASI 加载是按配置顺序单线程执行，所以这里不需要额外的线程同步原语。
bool gInitializationAttempted = false;
bool gStandaloneMode = false;
OriginalSaveGateFunction gOriginalSaveGate = nullptr;
OriginalMapTickFunction gOriginalMapTick = nullptr;
OriginalSaveWriterFunction gOriginalSaveWriter = nullptr;
OriginalSaveSlotFunction gOriginalSaveSlot = nullptr;
OriginalLoadSlotFunction gOriginalLoadSlot = nullptr;
OriginalNoArgFunction gOriginalSavePrepare = nullptr;
OriginalNoArgFunction gOriginalPostLoad = nullptr;
OriginalSaveActionUpdateFunction gOriginalSaveActionUpdate = nullptr;
GameFileCtorFunction gGameFileCtor = nullptr;
GameFileDtorFunction gGameFileDtor = nullptr;
GameFileOpenFunction gGameFileOpen = nullptr;

// 91~99 全部已存在后，NextAutoSlot 指示“下一次应该覆盖哪一个物理槽”。
// 这个值只写入真实存档目录 ..\multimedia\save\.NEXTAUTOSLOT，不再污染玩家可能复制、
// 重装或替换的 INI。
// 文件只保存 091~099 三个 ASCII 字节，不含任何游戏进度；丢失或损坏时安全回到 91。
// 如果用户删掉任一自动档，游戏文件层扫描仍优先填空槽，游标不会强行覆盖其它档。
DWORD gNextAutoSlot = kAutoSlotFirst;

// ============================================================================
// 五、动态 user32 / winmm API
// ============================================================================
typedef signed short (WINAPI *GetAsyncKeyStateFunction)(int key);
typedef HWND (WINAPI *GetForegroundWindowFunction)(void);
typedef DWORD (WINAPI *GetWindowThreadProcessIdFunction)(HWND window, DWORD* processId);
typedef BOOL (WINAPI *PlaySoundWFunction)(LPCWSTR sound, HMODULE module, DWORD flags);

HMODULE gUser32Module = nullptr;
GetAsyncKeyStateFunction gGetAsyncKeyState = nullptr;
GetForegroundWindowFunction gGetForegroundWindow = nullptr;
GetWindowThreadProcessIdFunction gGetWindowThreadProcessId = nullptr;
HMODULE gWinmmModule = nullptr;
PlaySoundWFunction gPlaySoundW = nullptr;

bool ResolveUser32() {
    if (gGetAsyncKeyState != nullptr && gGetForegroundWindow != nullptr &&
        gGetWindowThreadProcessId != nullptr) {
        return true;
    }
    if (gUser32Module == nullptr) {
        // 在 Map Tick 里才第一次执行这里，不在 DllMain loader lock 中主动 LoadLibrary。
        gUser32Module = LoadLibraryW(L"user32.dll");
    }
    if (gUser32Module == nullptr) {
        return false;
    }
    gGetAsyncKeyState = reinterpret_cast<GetAsyncKeyStateFunction>(
        GetProcAddress(gUser32Module, "GetAsyncKeyState"));
    gGetForegroundWindow = reinterpret_cast<GetForegroundWindowFunction>(
        GetProcAddress(gUser32Module, "GetForegroundWindow"));
    gGetWindowThreadProcessId = reinterpret_cast<GetWindowThreadProcessIdFunction>(
        GetProcAddress(gUser32Module, "GetWindowThreadProcessId"));
    return gGetAsyncKeyState != nullptr && gGetForegroundWindow != nullptr &&
           gGetWindowThreadProcessId != nullptr;
}

bool GameIsForegroundForKeyboard() {
    if (!ResolveUser32()) {
        return false;
    }
    HWND foreground = gGetForegroundWindow();
    if (foreground == nullptr) {
        return false;
    }
    DWORD processId = 0u;
    gGetWindowThreadProcessId(foreground, &processId);
    return processId != 0u && processId == GetCurrentProcessId();
}

// ============================================================================
// 六、字符串、路径、INI 配置
// ============================================================================
SIZE_T WLen(const wchar_t* text) {
    SIZE_T n = 0u;
    if (text != nullptr) {
        while (text[n] != L'\0') {
            ++n;
        }
    }
    return n;
}

bool WEqualsIgnoreCaseAscii(wchar_t a, wchar_t b) {
    if (a >= L'A' && a <= L'Z') a = static_cast<wchar_t>(a - L'A' + L'a');
    if (b >= L'A' && b <= L'Z') b = static_cast<wchar_t>(b - L'A' + L'a');
    return a == b;
}


bool GetSelfDirectory(wchar_t* out, SIZE_T capacity) {
    if (out == nullptr || capacity < 4u || gSelfModule == nullptr) {
        return false;
    }
    const DWORD length = GetModuleFileNameW(gSelfModule, out, static_cast<DWORD>(capacity));
    if (length == 0u || static_cast<SIZE_T>(length) >= capacity) {
        return false;
    }
    SIZE_T cut = static_cast<SIZE_T>(length);
    while (cut > 0u && out[cut - 1u] != L'\\' && out[cut - 1u] != L'/') {
        --cut;
    }
    out[cut] = L'\0';
    return true;
}

bool AppendW(wchar_t* path, SIZE_T capacity, const wchar_t* suffix) {
    if (path == nullptr || suffix == nullptr) {
        return false;
    }
    const SIZE_T base = WLen(path);
    const SIZE_T extra = WLen(suffix);
    if (base + extra + 1u > capacity) {
        return false;
    }
    for (SIZE_T i = 0u; i <= extra; ++i) {
        path[base + i] = suffix[i];
    }
    return true;
}

bool BuildIniPath(wchar_t* out, SIZE_T capacity) {
    return GetSelfDirectory(out, capacity) && AppendW(out, capacity, L"Castle_SaveEnhance.ini");
}

bool BuildAutoRingStatePath(wchar_t* out, SIZE_T capacity) {
    // 和日志共用同一条“模块路径 -> 所在目录 -> 追加目标名”路线。
    // RPG.exe 实际位于 exe 子目录，游戏存档位于它的 ../multimedia/save，所以不能再写 exe/Save。
    return ycrlog::BuildModuleFilePath(
        nullptr, L"..\\multimedia\\save\\.NEXTAUTOSLOT", out, capacity);
}

bool BuildLegacyAutoRingStatePath(wchar_t* out, SIZE_T capacity) {
    // test6 曾把状态误写到 RPG.exe 旁的 Save。这里只为一次性兼容读取/清理旧文件，
    // 新状态绝不能再写回这个路径。
    return ycrlog::BuildModuleFilePath(nullptr, L"Save\\.NEXTAUTOSLOT", out, capacity);
}

bool EnsureAutoRingStateDirectory() {
    // CreateFileW 不会自动创建父目录。RPG.exe 在 exe 下，而真实存档目录是
    // ../multimedia/save；multimedia 已随游戏存在，这里只确保最后一级 save 存在。
    wchar_t directory[520];
    if (!ycrlog::BuildModuleFilePath(nullptr, L"..\\multimedia\\save", directory, 520u)) {
        ycrlog::Line("[自动槽状态] 无法构造 multimedia\\save 目录路径。");
        return false;
    }

    if (CreateDirectoryW(directory, nullptr) != FALSE) {
        // 返回非零表示本次刚刚成功创建目录，可以继续创建状态文件。
        return true;
    }

    // 目录原本就存在时 CreateDirectoryW 也返回失败，但错误码 183 表示这是正常情况。
    const DWORD error = GetLastError();
    if (error == ERROR_ALREADY_EXISTS) {
        return true;
    }
    ycrlog::Text("[自动槽状态] 无法准备 multimedia\\save 目录，Win32错误码=");
    ycrlog::Unsigned(error);
    ycrlog::Line("。");
    return false;
}

bool IsValidWavFilename(const wchar_t* name) {
    // 只接受“单个文件名”，不允许用户通过 ..\ 或绝对路径把播放范围跳出插件资源目录。
    const SIZE_T length = WLen(name);
    if (length < 5u || length > 180u) {
        return false;
    }
    for (SIZE_T i = 0u; i < length; ++i) {
        const wchar_t c = name[i];
        if (c == L'\\' || c == L'/' || c == L':' || c == L'*' || c == L'?' ||
            c == L'"' || c == L'<' || c == L'>' || c == L'|') {
            return false;
        }
        if (c == L'.' && i + 1u < length && name[i + 1u] == L'.') {
            return false;
        }
    }
    const wchar_t* extension = name + length - 4u;
    return extension[0] == L'.' &&
           WEqualsIgnoreCaseAscii(extension[1], L'w') &&
           WEqualsIgnoreCaseAscii(extension[2], L'a') &&
           WEqualsIgnoreCaseAscii(extension[3], L'v');
}

struct SoundConfig {
    wchar_t quickSaveSuccess[192];
    wchar_t quickSaveFailed[192];
    wchar_t quickLoadConfirm[192];
    wchar_t quickLoadSuccess[192];
    wchar_t quickLoadFailed[192];
    wchar_t autoSaveSuccess[192];
    wchar_t autoSaveFailed[192];
};

struct Config {
    bool quickEnable;
    bool controllerEnable;
    DWORD quickLoadPresses;
    DWORD quickLoadWindowMs;
    bool autoEnable;
    bool saveOnSceneChange;
    DWORD intervalMinutes;
    DWORD soundVolume;
    SoundConfig sound;
};

Config gConfig = {};
wchar_t gIniPath[520] = {};

DWORD ClampDword(DWORD value, DWORD low, DWORD high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

void ReadSoundValue(const wchar_t* key, wchar_t* out, DWORD capacity) {
    GetPrivateProfileStringW(L"Sound", key, L"", out, capacity, gIniPath);
    if (out[0] != L'\0' && !IsValidWavFilename(out)) {
        // 无效内容直接变成空配置。这样“填错名字”不会在每次存档时反复尝试危险路径。
        out[0] = L'\0';
    }
}

void LoadConfig() {
    gConfig.quickEnable = GetPrivateProfileIntW(L"Quick", L"Enable", 1, gIniPath) != 0u;
    gConfig.controllerEnable =
        GetPrivateProfileIntW(L"Quick", L"ControllerEnable", 1, gIniPath) != 0u;
    gConfig.quickLoadPresses = ClampDword(
        GetPrivateProfileIntW(L"Quick", L"QuickLoadPresses", 2, gIniPath), 2u, 3u);
    gConfig.quickLoadWindowMs = ClampDword(
        GetPrivateProfileIntW(L"Quick", L"QuickLoadWindowMs", 1200, gIniPath), 300u, 3000u);

    gConfig.autoEnable = GetPrivateProfileIntW(L"AutoSave", L"Enable", 1, gIniPath) != 0u;
    gConfig.saveOnSceneChange =
        GetPrivateProfileIntW(L"AutoSave", L"SaveOnSceneChange", 1, gIniPath) != 0u;
    gConfig.intervalMinutes = ClampDword(
        GetPrivateProfileIntW(L"AutoSave", L"IntervalMinutes", 5, gIniPath), 0u, 1440u);

    // Volume 是 SaveEnhance 外置 WAV 自己的音量百分比。
    // 0=静音，100=原 WAV 振幅；默认 70，避免提示音比老游戏本体突然响很多。
    gConfig.soundVolume = ClampDword(
        GetPrivateProfileIntW(L"Sound", L"Volume", 70, gIniPath), 0u, 100u);
    ReadSoundValue(L"QuickSaveSuccess", gConfig.sound.quickSaveSuccess, 192u);
    ReadSoundValue(L"QuickSaveFailed", gConfig.sound.quickSaveFailed, 192u);
    ReadSoundValue(L"QuickLoadConfirm", gConfig.sound.quickLoadConfirm, 192u);
    ReadSoundValue(L"QuickLoadSuccess", gConfig.sound.quickLoadSuccess, 192u);
    ReadSoundValue(L"QuickLoadFailed", gConfig.sound.quickLoadFailed, 192u);
    ReadSoundValue(L"AutoSaveSuccess", gConfig.sound.autoSaveSuccess, 192u);
    ReadSoundValue(L"AutoSaveFailed", gConfig.sound.autoSaveFailed, 192u);
}

bool BuildSaveAnsiPath(DWORD slot, char* out, SIZE_T capacity) {
    // 这就是原版 Writer 最终送进游戏 File::Open 的路径：Save\SaveNNN.TSF。
    // 这里使用 ANSI char 而不是 wchar_t，因为 0x4417C0 本身接收 char*。
    if (out == nullptr || capacity < 18u || slot > 999u) return false;

    const char prefix[] = "Save\\Save";
    const char suffix[] = ".TSF";
    SIZE_T cursor = 0u;
    for (SIZE_T i = 0u; prefix[i] != '\0'; ++i) out[cursor++] = prefix[i];

    out[cursor++] = static_cast<char>('0' + ((slot / 100u) % 10u));
    out[cursor++] = static_cast<char>('0' + ((slot / 10u) % 10u));
    out[cursor++] = static_cast<char>('0' + (slot % 10u));

    for (SIZE_T i = 0u; suffix[i] != '\0'; ++i) out[cursor++] = suffix[i];
    out[cursor] = '\0';
    return cursor + 1u <= capacity;
}

bool GameSaveFileExists(DWORD slot) {
    // test3/test5 的错误都来自“插件自己猜 Windows 实际路径”。test5 完全不再这样做。
    // 这里创建一个和原版 0x43B360/0x43B510 一样的 0x14 字节 File 对象：
    // 1. ctor(1) 让它使用游戏自己的搜索/重定向文件层；
    // 2. Open(path, mode=1, flags=0) 与 LoadSlot 读取 TSF 的模式一致；
    // 3. 无论打开成功还是失败都调用析构，释放 File 内部可能分配的路径缓冲。
    if (gGameFileCtor == nullptr || gGameFileDtor == nullptr || gGameFileOpen == nullptr) {
        return false;
    }

    char path[32];
    if (!BuildSaveAnsiPath(slot, path, sizeof(path))) return false;

    BYTE fileObject[kGameFileObjectBytes];
    memset(fileObject, 0, sizeof(fileObject));
    gGameFileCtor(fileObject, nullptr, 1u);
    const BOOL opened = gGameFileOpen(fileObject, nullptr, path, 1u, 0u);
    gGameFileDtor(fileObject, nullptr);
    return opened != FALSE;
}

DWORD ClampAutoRingSlot(DWORD value) {
    return (value >= kAutoSlotFirst && value <= kAutoSlotLast) ? value : kAutoSlotFirst;
}

enum AutoRingStateReadResult {
    kAutoRingStateMissing = 0,
    kAutoRingStateInvalid = 1,
    kAutoRingStateValid = 2
};

AutoRingStateReadResult ReadAutoRingStateFile(const wchar_t* path, DWORD* slotOut) {
    // 这个读取器只判断“指定完整路径里是否有合法三字节游标”，不决定它是新路径还是旧路径。
    // 因此新旧两个位置可以复用完全相同的长度、数字和 91~99 范围验证。
    if (path == nullptr || slotOut == nullptr) return kAutoRingStateInvalid;

    HANDLE file = CreateFileW(
        path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return kAutoRingStateMissing;

    BYTE raw[4] = {};
    DWORD bytesRead = 0u;
    const BOOL readOk = ReadFile(file, raw, 4u, &bytesRead, nullptr);
    CloseHandle(file);
    if (readOk == FALSE || bytesRead != 3u ||
        raw[0] < static_cast<BYTE>('0') || raw[0] > static_cast<BYTE>('9') ||
        raw[1] < static_cast<BYTE>('0') || raw[1] > static_cast<BYTE>('9') ||
        raw[2] < static_cast<BYTE>('0') || raw[2] > static_cast<BYTE>('9')) {
        return kAutoRingStateInvalid;
    }

    const DWORD parsed = static_cast<DWORD>(raw[0] - static_cast<BYTE>('0')) * 100u +
                         static_cast<DWORD>(raw[1] - static_cast<BYTE>('0')) * 10u +
                         static_cast<DWORD>(raw[2] - static_cast<BYTE>('0'));
    if (parsed < kAutoSlotFirst || parsed > kAutoSlotLast) return kAutoRingStateInvalid;
    *slotOut = parsed;
    return kAutoRingStateValid;
}

void LoadAutoRingState() {
    // 主路径固定为 ../multimedia/save。若它还没有合法状态，再兼容读取 test6 错写到
    // exe/Save 的旧文件，避免升级以后明明已有 092 游标却重新从 91 开始。
    gNextAutoSlot = kAutoSlotFirst;

    wchar_t path[520];
    if (!BuildAutoRingStatePath(path, 520u)) {
        ycrlog::Line("[自动槽状态] 无法构造 multimedia\\save 状态路径；本轮从91开始。");
        return;
    }

    DWORD parsed = kAutoSlotFirst;
    const AutoRingStateReadResult current = ReadAutoRingStateFile(path, &parsed);
    if (current == kAutoRingStateValid) {
        gNextAutoSlot = parsed;
        ycrlog::Line("[自动槽状态] 已从 multimedia\\save 读取环形游标。");
        return;
    }

    wchar_t legacyPath[520];
    if (BuildLegacyAutoRingStatePath(legacyPath, 520u) &&
        ReadAutoRingStateFile(legacyPath, &parsed) == kAutoRingStateValid) {
        gNextAutoSlot = parsed;
        ycrlog::Line("[自动槽状态] 已读取 test6 旧 exe\\Save 游标；下次成功写入后迁移到 multimedia\\save。");
        return;
    }

    ycrlog::Line(current == kAutoRingStateInvalid
        ? "[自动槽状态] multimedia\\save 中的状态内容无效；本轮从91开始。"
        : "[自动槽状态] multimedia\\save 中不存在状态文件；本轮从91开始。");
}

void CleanupLegacyAutoRingStateFile() {
    // 只有正确位置已经成功写入后才删旧错误文件。旧 exe/Save 目录本身绝不删除，
    // 因为目录内可能还有用户自己放入的其它文件。
    wchar_t legacyPath[520];
    if (!BuildLegacyAutoRingStatePath(legacyPath, 520u)) return;
    if (DeleteFileW(legacyPath) != FALSE) {
        ycrlog::Line("[自动槽状态] 已删除 test6 旧 exe\\Save 状态文件。");
        return;
    }
    const DWORD error = GetLastError();
    if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND) {
        ycrlog::Text("[自动槽状态] 正确状态已写入，但旧 exe\\Save 文件清理失败，Win32错误码=");
        ycrlog::Unsigned(error);
        ycrlog::Line("。");
    }
}

void SaveAutoRingState(DWORD nextSlot) {
    gNextAutoSlot = ClampAutoRingSlot(nextSlot);

    if (!EnsureAutoRingStateDirectory()) {
        ycrlog::Line("[自动槽状态] 本次存档仍有效；multimedia\\save 不可用，重启后从91开始。");
        return;
    }

    wchar_t path[520];
    if (!BuildAutoRingStatePath(path, 520u)) {
        ycrlog::Line("[自动槽状态] 无法构造 multimedia\\save 状态路径；本次存档仍有效，重启后从91开始。");
        return;
    }

    HANDLE file = CreateFileW(
        path, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        ycrlog::Text("[自动槽状态] 无法创建 multimedia\\save\\.NEXTAUTOSLOT，Win32错误码=");
        ycrlog::Unsigned(error);
        ycrlog::Line("；本次存档仍有效，重启后从91开始。");
        return;
    }

    // 文件固定只有三个 ASCII 字节，例如 92 写成“092”。不用 sprintf，可以继续维持无 CRT。
    const BYTE raw[3] = {
        static_cast<BYTE>('0' + ((gNextAutoSlot / 100u) % 10u)),
        static_cast<BYTE>('0' + ((gNextAutoSlot / 10u) % 10u)),
        static_cast<BYTE>('0' + (gNextAutoSlot % 10u))};
    DWORD bytesWritten = 0u;
    const BOOL writeOk = WriteFile(file, raw, 3u, &bytesWritten, nullptr);
    const DWORD writeError = writeOk == FALSE ? GetLastError() : 0u;
    CloseHandle(file);
    if (writeOk == FALSE || bytesWritten != 3u) {
        // 状态写失败不会把刚完成的游戏存档判成失败；当前进程仍继续使用内存里的正确游标。
        ycrlog::Text("[自动槽状态] 写入 multimedia\\save\\.NEXTAUTOSLOT 失败，Win32错误码=");
        ycrlog::Unsigned(writeError);
        ycrlog::Text("，实际字节数=");
        ycrlog::Unsigned(bytesWritten);
        ycrlog::Line("；本次存档仍有效，重启后可能从91开始。");
        return;
    }
    CleanupLegacyAutoRingStateFile();
    ycrlog::Text("[自动槽状态] 已写入 multimedia\\save\\.NEXTAUTOSLOT，下一候选=");
    ycrlog::Unsigned(gNextAutoSlot);
    ycrlog::Line("。");
}

DWORD NextAutoRingSlot(DWORD slot) {
    return (slot >= kAutoSlotLast) ? kAutoSlotFirst : (slot + 1u);
}

bool ChooseAutoSaveSlot(DWORD* slotOut, bool* usedEmptySlot) {
    if (slotOut == nullptr || usedEmptySlot == nullptr) return false;

    // 第一优先级永远是“游戏自己的文件层认为不存在的最低槽”。
    // 因此第一次自然是 91，第二次会看到 91 已存在而选择 92，直到 99。
    for (DWORD slot = kAutoSlotFirst; slot <= kAutoSlotLast; ++slot) {
        if (!GameSaveFileExists(slot)) {
            *slotOut = slot;
            *usedEmptySlot = true;
            return true;
        }
    }

    // 九个都存在后才进入持久化环形覆盖。由于每次成功后都把游标推进一格，覆盖顺序就是：
    // 91→92→...→99→91...。这比文件重命名安全得多，每次只写一个 TSF。
    *slotOut = ClampAutoRingSlot(gNextAutoSlot);
    *usedEmptySlot = false;
    return true;
}

// ============================================================================
// 六-B、外置 WAV 的独立音量缩放
// ============================================================================
// PlaySoundW 本身没有“只改这一声 WAV 音量”的参数。直接调用 waveOutSetVolume 又会改整个
// 输出设备/游戏声音，不符合用户要求。因此 test5 的做法是：
// 1. Volume=100：仍直接让 PlaySoundW 播文件，任何 Windows 支持的 WAV 编码都能照常尝试；
// 2. Volume=1~99：只读 WAV 到私有内存，解析 RIFF 的 fmt/data chunk；
// 3. 对 PCM 样本乘 Volume/100，再用 SND_MEMORY 异步播放这份副本；
// 4. 内存副本缓存到进程结束，确保 SND_ASYNC 播放期间指针不会失效；
// 5. 不认识的压缩格式在 Volume<100 时宁可静默禁用，也绝不误改压缩字节造成爆音。
//
// 这套音量只影响 Castle_SaveEnhance 自己的提示 WAV，不调用游戏音量设置，也不改系统音量。

const DWORD kMaxExternalWavBytes = 16u * 1024u * 1024u; // 提示音上限 16 MiB，防止误选超大文件。
const SIZE_T kSoundCacheCapacity = 8u;                   // 当前最多 7 种提示音，留 1 个余量。

struct SoundCacheEntry {
    bool used;
    bool playable;
    wchar_t filename[192];
    BYTE* image;
    DWORD imageBytes;
};
SoundCacheEntry gSoundCache[kSoundCacheCapacity] = {};

WORD ReadLe16(const BYTE* p) {
    return static_cast<WORD>(static_cast<WORD>(p[0]) |
                             static_cast<WORD>(static_cast<WORD>(p[1]) << 8u));
}

DWORD ReadLe32(const BYTE* p) {
    return static_cast<DWORD>(p[0]) |
           (static_cast<DWORD>(p[1]) << 8u) |
           (static_cast<DWORD>(p[2]) << 16u) |
           (static_cast<DWORD>(p[3]) << 24u);
}

void WriteLe16(BYTE* p, WORD value) {
    p[0] = static_cast<BYTE>(value & 0xFFu);
    p[1] = static_cast<BYTE>((value >> 8u) & 0xFFu);
}

void WriteLe32(BYTE* p, DWORD value) {
    p[0] = static_cast<BYTE>(value & 0xFFu);
    p[1] = static_cast<BYTE>((value >> 8u) & 0xFFu);
    p[2] = static_cast<BYTE>((value >> 16u) & 0xFFu);
    p[3] = static_cast<BYTE>((value >> 24u) & 0xFFu);
}

bool WStringEquals(const wchar_t* a, const wchar_t* b) {
    if (a == nullptr || b == nullptr) return false;
    SIZE_T i = 0u;
    while (a[i] != L'\0' && b[i] != L'\0') {
        if (a[i] != b[i]) return false;
        ++i;
    }
    return a[i] == b[i];
}

bool CopyWString(wchar_t* out, SIZE_T capacity, const wchar_t* text) {
    if (out == nullptr || text == nullptr || capacity == 0u) return false;
    const SIZE_T length = WLen(text);
    if (length + 1u > capacity) return false;
    for (SIZE_T i = 0u; i <= length; ++i) out[i] = text[i];
    return true;
}

bool BuildExternalWavPath(const wchar_t* filename, wchar_t* out, SIZE_T capacity) {
    return IsValidWavFilename(filename) && GetSelfDirectory(out, capacity) &&
           AppendW(out, capacity, L"Castle_SaveEnhance\\") &&
           AppendW(out, capacity, filename);
}

bool FourCCEquals(const BYTE* p, char a, char b, char c, char d) {
    return p != nullptr && p[0] == static_cast<BYTE>(a) && p[1] == static_cast<BYTE>(b) &&
           p[2] == static_cast<BYTE>(c) && p[3] == static_cast<BYTE>(d);
}

bool ScalePcmSamples(BYTE* data, DWORD dataBytes, WORD bitsPerSample, DWORD volume) {
    if (data == nullptr) return false;

    if (bitsPerSample == 8u) {
        // 8-bit PCM 是无符号 0~255，静音中心在 128，而不是 0。
        for (DWORD i = 0u; i < dataBytes; ++i) {
            const int centered = static_cast<int>(data[i]) - 128;
            int scaled = 128 + (centered * static_cast<int>(volume)) / 100;
            if (scaled < 0) scaled = 0;
            if (scaled > 255) scaled = 255;
            data[i] = static_cast<BYTE>(scaled);
        }
        return true;
    }

    if (bitsPerSample == 16u) {
        if ((dataBytes % 2u) != 0u) return false;
        for (DWORD i = 0u; i < dataBytes; i += 2u) {
            const signed short sample = static_cast<signed short>(ReadLe16(data + i));
            const int scaled = (static_cast<int>(sample) * static_cast<int>(volume)) / 100;
            WriteLe16(data + i, static_cast<WORD>(static_cast<signed short>(scaled)));
        }
        return true;
    }

    if (bitsPerSample == 24u) {
        if ((dataBytes % 3u) != 0u) return false;
        for (DWORD i = 0u; i < dataBytes; i += 3u) {
            int sample = static_cast<int>(data[i]) |
                         (static_cast<int>(data[i + 1u]) << 8) |
                         (static_cast<int>(data[i + 2u]) << 16);
            if ((sample & 0x00800000) != 0) sample |= ~0x00FFFFFF;
            // 24-bit 最大绝对值约 838 万，乘 100 仍远小于 32-bit signed 上限，所以这里
            // 可以完全用 int 算术，不需要会引入 __alldiv CRT helper 的 64 位除法。
            int scaled = (sample * static_cast<int>(volume)) / 100;
            if (scaled < -8388608) scaled = -8388608;
            if (scaled > 8388607) scaled = 8388607;
            const DWORD raw = static_cast<DWORD>(scaled) & 0x00FFFFFFu;
            data[i] = static_cast<BYTE>(raw & 0xFFu);
            data[i + 1u] = static_cast<BYTE>((raw >> 8u) & 0xFFu);
            data[i + 2u] = static_cast<BYTE>((raw >> 16u) & 0xFFu);
        }
        return true;
    }

    if (bitsPerSample == 32u) {
        if ((dataBytes % 4u) != 0u) return false;
        for (DWORD i = 0u; i < dataBytes; i += 4u) {
            const LONG sample = static_cast<LONG>(ReadLe32(data + i));
            // 32-bit PCM 不能直接 sample*100，会溢出。先把 sample 拆成“整百 + 余数”：
            //   sample = (sample/100)*100 + sample%100
            // 再分别乘 volume。两个中间值都不会超过原 sample 的数量级，因此不需要 64 位除法。
            const LONG hundreds = sample / 100;
            const LONG remainder = sample % 100;
            const LONG scaled =
                hundreds * static_cast<LONG>(volume) +
                (remainder * static_cast<LONG>(volume)) / 100;
            WriteLe32(data + i, static_cast<DWORD>(scaled));
        }
        return true;
    }

    return false;
}

bool ScaleWaveImageInPlace(BYTE* image, DWORD bytes, DWORD volume) {
    if (image == nullptr || bytes < 44u || !FourCCEquals(image, 'R', 'I', 'F', 'F') ||
        !FourCCEquals(image + 8u, 'W', 'A', 'V', 'E')) {
        return false;
    }

    const BYTE* fmt = nullptr;
    DWORD fmtBytes = 0u;
    BYTE* audioData = nullptr;
    DWORD audioBytes = 0u;

    DWORD cursor = 12u;
    while (cursor + 8u <= bytes) {
        BYTE* chunk = image + cursor;
        const DWORD chunkBytes = ReadLe32(chunk + 4u);
        const DWORD payload = cursor + 8u;
        if (payload > bytes || chunkBytes > bytes - payload) return false;

        if (FourCCEquals(chunk, 'f', 'm', 't', ' ') && fmt == nullptr) {
            fmt = image + payload;
            fmtBytes = chunkBytes;
        } else if (FourCCEquals(chunk, 'd', 'a', 't', 'a') && audioData == nullptr) {
            audioData = image + payload;
            audioBytes = chunkBytes;
        }

        // RIFF chunk 按偶数字节对齐。+1 再清最低位就是“向上取偶数”。
        const DWORD padded = (chunkBytes + 1u) & ~1u;
        if (padded > bytes - payload) break;
        cursor = payload + padded;
    }

    if (fmt == nullptr || fmtBytes < 16u || audioData == nullptr) return false;

    WORD formatTag = ReadLe16(fmt + 0u);
    const WORD bitsPerSample = ReadLe16(fmt + 14u);

    // WAVE_FORMAT_EXTENSIBLE(0xFFFE) 的真正编码类型在 SubFormat GUID 的 Data1。
    // test5 的 Volume<100 只缩放 PCM，所以只接受 SubFormat.Data1=1。
    // IEEE-float/压缩 WAV 若需要保持原格式，可以把 Volume 设为 100 让 Windows 直接播放。
    if (formatTag == 0xFFFEu) {
        if (fmtBytes < 40u) return false;
        const DWORD subFormatData1 = ReadLe32(fmt + 24u);
        if (subFormatData1 == 1u) {
            formatTag = 1u;
        } else {
            return false;
        }
    }

    if (formatTag == 1u) {
        return ScalePcmSamples(audioData, audioBytes, bitsPerSample, volume);
    }
    return false;
}

SoundCacheEntry* FindSoundCache(const wchar_t* filename) {
    for (SIZE_T i = 0u; i < kSoundCacheCapacity; ++i) {
        if (gSoundCache[i].used && WStringEquals(gSoundCache[i].filename, filename)) {
            return &gSoundCache[i];
        }
    }
    return nullptr;
}

SoundCacheEntry* AllocateSoundCacheEntry(const wchar_t* filename) {
    for (SIZE_T i = 0u; i < kSoundCacheCapacity; ++i) {
        if (!gSoundCache[i].used) {
            gSoundCache[i].used = true;
            gSoundCache[i].playable = false;
            gSoundCache[i].image = nullptr;
            gSoundCache[i].imageBytes = 0u;
            if (!CopyWString(gSoundCache[i].filename, 192u, filename)) {
                gSoundCache[i].used = false;
                return nullptr;
            }
            return &gSoundCache[i];
        }
    }
    return nullptr;
}

SoundCacheEntry* LoadScaledSound(const wchar_t* filename) {
    SoundCacheEntry* cached = FindSoundCache(filename);
    if (cached != nullptr) return cached;

    SoundCacheEntry* entry = AllocateSoundCacheEntry(filename);
    if (entry == nullptr) return nullptr;

    wchar_t path[520];
    if (!BuildExternalWavPath(filename, path, 520u)) return entry;

    HANDLE file = CreateFileW(
        path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return entry;

    DWORD high = 0u;
    const DWORD fileBytes = GetFileSize(file, &high);
    if (high != 0u || fileBytes == INVALID_FILE_SIZE || fileBytes < 44u ||
        fileBytes > kMaxExternalWavBytes) {
        CloseHandle(file);
        return entry;
    }

    HANDLE heap = GetProcessHeap();
    if (heap == nullptr) {
        CloseHandle(file);
        return entry;
    }
    BYTE* image = reinterpret_cast<BYTE*>(HeapAlloc(heap, 0u, fileBytes));
    if (image == nullptr) {
        CloseHandle(file);
        return entry;
    }

    DWORD readBytes = 0u;
    const BOOL readOk = ReadFile(file, image, fileBytes, &readBytes, nullptr);
    CloseHandle(file);
    if (readOk == FALSE || readBytes != fileBytes ||
        !ScaleWaveImageInPlace(image, fileBytes, gConfig.soundVolume)) {
        HeapFree(heap, 0u, image);
        ycrlog::Text("[声音] WAV 无法按当前 Volume 安全缩放，已静默禁用：");
        // 日志工具只有 UTF-8 窄字符串接口；文件名本身通常是 ASCII/拉丁。这里不冒险做编码转换，
        // 只给固定诊断，详细文件名仍可从 INI 对照。
        ycrlog::Line("请检查对应 [Sound] 文件是否为 PCM WAV；或把 Volume=100 交给 Windows 原样播放。");
        return entry;
    }

    entry->image = image;
    entry->imageBytes = fileBytes;
    entry->playable = true;
    return entry;
}

bool ResolvePlaySound() {
    if (gPlaySoundW != nullptr) return true;
    if (gWinmmModule == nullptr) gWinmmModule = LoadLibraryW(L"winmm.dll");
    if (gWinmmModule == nullptr) return false;
    gPlaySoundW = reinterpret_cast<PlaySoundWFunction>(GetProcAddress(gWinmmModule, "PlaySoundW"));
    return gPlaySoundW != nullptr;
}

bool PlayConfiguredSound(const wchar_t* filename) {
    if (filename == nullptr || filename[0] == L'\0' || !IsValidWavFilename(filename) ||
        gConfig.soundVolume == 0u || !ResolvePlaySound()) {
        return false;
    }

    wchar_t path[520];
    if (!BuildExternalWavPath(filename, path, 520u)) return false;

    if (gConfig.soundVolume >= 100u) {
        // 100% 时完全不解析 WAV，直接保留 Windows 原来能播放的格式兼容性。
        return gPlaySoundW(path, nullptr, SND_FILENAME | SND_ASYNC | SND_NODEFAULT) != FALSE;
    }

    SoundCacheEntry* entry = LoadScaledSound(filename);
    if (entry == nullptr || !entry->playable || entry->image == nullptr) return false;

    // SND_ASYNC 要求内存一直有效，所以 entry->image 不在播放后释放，而是保留到进程结束。
    return gPlaySoundW(
               reinterpret_cast<LPCWSTR>(entry->image),
               nullptr,
               SND_MEMORY | SND_ASYNC | SND_NODEFAULT) != FALSE;
}

// ============================================================================
// 七、最小内存安全读取
// ============================================================================
bool IsReadableRange(const void* address, SIZE_T size) {
    if (address == nullptr || size == 0u) return false;
    const SIZE_T begin = reinterpret_cast<SIZE_T>(address);
    const SIZE_T end = begin + size;
    if (end < begin) return false;

    SIZE_T cursor = begin;
    while (cursor < end) {
        MEMORY_BASIC_INFORMATION_MINI info;
        const SIZE_T queried = VirtualQuery(reinterpret_cast<LPCVOID>(cursor), &info, sizeof(info));
        if (queried == 0u || info.State != MEM_COMMIT || info.RegionSize == 0u ||
            (info.Protect & PAGE_GUARD) != 0u || (info.Protect & PAGE_NOACCESS) != 0u) {
            return false;
        }
        const SIZE_T regionEnd = reinterpret_cast<SIZE_T>(info.BaseAddress) + info.RegionSize;
        if (regionEnd <= cursor) return false;
        cursor = regionEnd;
    }
    return true;
}

bool ReadDword(const BYTE* address, DWORD* value) {
    if (value == nullptr || !IsReadableRange(address, sizeof(DWORD))) return false;
    *value = *reinterpret_cast<const volatile DWORD*>(address);
    return true;
}

bool ReadByte(const BYTE* address, BYTE* value) {
    if (value == nullptr || !IsReadableRange(address, 1u)) return false;
    *value = *reinterpret_cast<const volatile BYTE*>(address);
    return true;
}

bool ReadExeDword(DWORD rva, DWORD* value) {
    return gExeBase != nullptr && ReadDword(gExeBase + rva, value);
}

void CopyBytes(BYTE* out, const BYTE* in, SIZE_T size) {
    volatile BYTE* destination = reinterpret_cast<volatile BYTE*>(out);
    const volatile BYTE* source = reinterpret_cast<const volatile BYTE*>(in);
    for (SIZE_T i = 0u; i < size; ++i) destination[i] = source[i];
}

// ============================================================================
// 八、地图身份、受控角色和严格自由行动判断
// ============================================================================
struct MapState {
    BYTE* sceneContainer;
    BYTE* rawSource;
    BYTE* runtimeEntry;
    DWORD recordCount;
    DWORD selectedIndex;
    DWORD defaultIndex;
    DWORD sourceFingerprint;
};

DWORD BuildSourceFingerprint(const BYTE* rawSource, DWORD recordCount) {
    const SIZE_T total = static_cast<SIZE_T>(recordCount) * kMapSciRecordSize;
    if (!IsReadableRange(rawSource, total)) return 0u;
    DWORD hash = 2166136261u;
    for (SIZE_T i = 0u; i < total; ++i) {
        hash ^= static_cast<DWORD>(rawSource[i]);
        hash *= 16777619u;
    }
    return hash;
}

bool ReadMapState(MapState* state) {
    if (state == nullptr) return false;
    DWORD dataCenterAddress = 0u;
    if (!ReadExeDword(kDataCenterPointerRva, &dataCenterAddress) || dataCenterAddress == 0u) return false;
    BYTE* dataCenter = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(dataCenterAddress));

    DWORD sceneAddress = 0u;
    if (!ReadDword(dataCenter + kDataCenterSceneContainerOffset, &sceneAddress) || sceneAddress == 0u) {
        return false;
    }
    BYTE* scene = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(sceneAddress));
    if (!IsReadableRange(scene, kMapRuntimeEntryOffset + sizeof(DWORD))) return false;

    DWORD recordCount = 0u, selected = 0u, rawAddress = 0u, runtimeAddress = 0u;
    if (!ReadDword(scene + kMapRecordCountOffset, &recordCount) ||
        !ReadDword(scene + kMapSelectedIndexOffset, &selected) ||
        !ReadDword(scene + kMapRawSourceOffset, &rawAddress) ||
        !ReadDword(scene + kMapRuntimeEntryOffset, &runtimeAddress)) {
        return false;
    }
    if (recordCount == 0u || recordCount > kMaxReasonableMapRecords || selected >= recordCount ||
        rawAddress == 0u || runtimeAddress == 0u) {
        return false;
    }

    BYTE* raw = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(rawAddress));
    if (!IsReadableRange(raw, static_cast<SIZE_T>(recordCount) * kMapSciRecordSize)) return false;

    DWORD defaultIndex = 0u;
    for (DWORD i = 0u; i < recordCount; ++i) {
        // 0x40AC00 初始化 scene+4 时会把“最后一个 record byte0 !=0”的索引留下。
        if (raw[static_cast<SIZE_T>(i) * kMapSciRecordSize] != 0u) defaultIndex = i;
    }

    state->sceneContainer = scene;
    state->rawSource = raw;
    state->runtimeEntry = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(runtimeAddress));
    state->recordCount = recordCount;
    state->selectedIndex = selected;
    state->defaultIndex = defaultIndex;
    state->sourceFingerprint = BuildSourceFingerprint(raw, recordCount);
    return state->sourceFingerprint != 0u && IsReadableRange(state->runtimeEntry, 4u);
}

struct ActorState {
    DWORD actorIndex;
    DWORD x;
    DWORD y;
    DWORD stateByte;
};

bool ReadControlledActorState(ActorState* state) {
    if (state == nullptr) return false;
    DWORD actorIndex = 0u, arrayAddress = 0u;
    if (!ReadExeDword(kControlledActorIndexRva, &actorIndex) ||
        !ReadExeDword(kActorRuntimeArrayPointerRva, &arrayAddress) ||
        actorIndex == 0u || actorIndex >= 256u || arrayAddress == 0u) {
        // 原版 Space handler 也把 controlled actor index==0 当成不能自由探索。
        return false;
    }
    BYTE* actor = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(arrayAddress)) +
                  static_cast<SIZE_T>(actorIndex) * kActorStride;
    if (!IsReadableRange(actor, kActorStateByteOffset + 1u)) return false;
    state->actorIndex = actorIndex;
    state->x = *reinterpret_cast<const volatile DWORD*>(actor + kActorXOffset);
    state->y = *reinterpret_cast<const volatile DWORD*>(actor + kActorYOffset);
    state->stateByte = static_cast<DWORD>(
        *reinterpret_cast<const volatile BYTE*>(actor + kActorStateByteOffset));
    return true;
}

bool GlobalPointerIsNull(DWORD rva) {
    DWORD value = 0u;
    return ReadExeDword(rva, &value) && value == 0u;
}

bool IsMovieInactive() {
    DWORD movieAddress = 0u;
    if (!ReadExeDword(kMovieObjectRva, &movieAddress)) return false;
    if (movieAddress == 0u) return true;
    BYTE active = 1u;
    BYTE* movie = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(movieAddress));
    return ReadByte(movie + 0x0Au, &active) && active == 0u;
}

bool IsStrictFreeRoamCore(const MapState& map) {
    // 第一层：必须是当前地图默认 Scene Mode。楼梯、剧情事件、临时脚本 mode 一律不算自由行动。
    if (map.selectedIndex != map.defaultIndex) return false;

    DWORD busy = 1u;
    DWORD dialogue = 1u;
    if (!ReadExeDword(kMapBusyRva, &busy) || busy != 0u ||
        !ReadExeDword(kDialogueIdRva, &dialogue) || dialogue != 0u) {
        return false;
    }

    // 第二层：任何已经确认的主 UI/战斗/标题/存档点对象存在，就拒绝 Quick Save/Load。
    if (!GlobalPointerIsNull(kBattleUiRva) || !GlobalPointerIsNull(kTitleUiRva) ||
        !GlobalPointerIsNull(kInterfaceUiRva) || !GlobalPointerIsNull(kSavePointUiRva) ||
        !IsMovieInactive()) {
        return false;
    }

    ActorState actor;
    DWORD worldAddress = 0u;
    return ReadControlledActorState(&actor) &&
           ReadExeDword(kWorldGlobalPointerRva, &worldAddress) && worldAddress != 0u &&
           IsReadableRange(reinterpret_cast<BYTE*>(static_cast<SIZE_T>(worldAddress)), kWorldHeaderBytes);
}

bool SameMapIdentity(const MapState& a, const MapState& b) {
    // selectedIndex 不参与地图身份，因为同一张地图进入楼梯/剧情 mode 会改变 selectedIndex。
    return a.sceneContainer == b.sceneContainer && a.rawSource == b.rawSource &&
           a.recordCount == b.recordCount && a.defaultIndex == b.defaultIndex &&
           a.sourceFingerprint == b.sourceFingerprint;
}

// ============================================================================
// 九、入口级 vanilla fallback 锚点
// ============================================================================
const DWORD kAnchorMinimumSamples = 8u;

struct VanillaAnchor {
    bool valid;
    ActorState actor;
    BYTE worldHeader[kWorldHeaderBytes];
};

struct AnchorTracker {
    bool identityValid;
    MapState identity;
    DWORD stableSampleCount;
    bool firstSampleValid;
    VanillaAnchor firstSample;
    VanillaAnchor frozen;
};

AnchorTracker gAnchor = {};

bool AnchorIdentityMatches(const MapState& map) {
    return gAnchor.identityValid && SameMapIdentity(gAnchor.identity, map);
}

void ResetAnchorTracker(const MapState& map) {
    gAnchor.identityValid = true;
    gAnchor.identity = map;
    gAnchor.stableSampleCount = 0u;
    gAnchor.firstSampleValid = false;
    gAnchor.frozen.valid = false;
}

bool ReadCurrentWorldHeader(VanillaAnchor* sample) {
    if (sample == nullptr) return false;
    DWORD worldAddress = 0u;
    if (!ReadExeDword(kWorldGlobalPointerRva, &worldAddress) || worldAddress == 0u) return false;
    BYTE* world = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(worldAddress));
    if (!IsReadableRange(world, kWorldHeaderBytes)) return false;

    ActorState actor;
    if (!ReadControlledActorState(&actor)) return false;
    sample->valid = true;
    sample->actor = actor;
    CopyBytes(sample->worldHeader, world, kWorldHeaderBytes);
    *reinterpret_cast<DWORD*>(sample->worldHeader + kWorldXOffset) = actor.x;
    *reinterpret_cast<DWORD*>(sample->worldHeader + kWorldYOffset) = actor.y;
    *reinterpret_cast<DWORD*>(sample->worldHeader + kWorldStateOffset) = actor.stateByte & 0xFFu;
    *reinterpret_cast<DWORD*>(sample->worldHeader + kWorldActorIndexOffset) = actor.actorIndex;
    return true;
}

void ObserveVanillaAnchor(const MapState& map) {
    if (!AnchorIdentityMatches(map)) ResetAnchorTracker(map);
    if (gAnchor.frozen.valid) return;

    // SaveEnhance 比旧版再保守一层：稳定锚点不仅要求 selected==default，还要求完整自由行动门。
    // 只要中间某一 Tick 出现 UI/对话/busy，就把当前连续窗口作废，从下一次自由 Tick 重计。
    if (!IsStrictFreeRoamCore(map)) {
        gAnchor.stableSampleCount = 0u;
        gAnchor.firstSampleValid = false;
        return;
    }

    VanillaAnchor current = {};
    if (!ReadCurrentWorldHeader(&current)) {
        gAnchor.stableSampleCount = 0u;
        gAnchor.firstSampleValid = false;
        return;
    }
    if (!gAnchor.firstSampleValid) {
        gAnchor.firstSample = current;
        gAnchor.firstSampleValid = true;
        gAnchor.stableSampleCount = 1u;
        return;
    }

    ++gAnchor.stableSampleCount;
    if (gAnchor.stableSampleCount >= kAnchorMinimumSamples) {
        gAnchor.frozen = gAnchor.firstSample;
        gAnchor.frozen.valid = true;
        ycrlog::Line("[回退] 当前地图连续 8 个严格自由行动 Tick；已冻结入口级安全锚点。");
    }
}

bool GetFrozenAnchor(const MapState& map, VanillaAnchor* out) {
    if (out == nullptr || !AnchorIdentityMatches(map) || !gAnchor.frozen.valid) return false;
    *out = gAnchor.frozen;
    return out->valid;
}

// ============================================================================
// 十、一次 Writer fallback 授权
// ============================================================================
struct ExtendedSaveArm {
    bool valid;
    MapState identity;
    VanillaAnchor fallback;
};
ExtendedSaveArm gArm = {};

void ClearArm() { gArm.valid = false; }

bool ArmExtendedSaveForMap(const MapState& map) {
    ClearArm();
    VanillaAnchor fallback = {};
    if (!GetFrozenAnchor(map, &fallback)) return false;
    gArm.valid = true;
    gArm.identity = map;
    gArm.fallback = fallback;
    return true;
}

bool ArmExtendedSaveFromCurrentMap() {
    MapState map;
    if (!ReadMapState(&map)) {
        ycrlog::Line("[存档门] 当前地图身份不可读取；不扩展原版禁存位置。");
        return false;
    }
    if (!ArmExtendedSaveForMap(map)) {
        ycrlog::Line("[存档门] 当前地图没有冻结安全锚点；不扩展原版禁存位置。");
        return false;
    }
    ycrlog::Line("[存档门] 原版拒绝，但已有当前地图安全锚点；下一次 Writer 允许使用 fallback。");
    return true;
}

bool ArmStillMatchesCurrentMap(MapState* current) {
    if (!gArm.valid || current == nullptr || !ReadMapState(current)) return false;
    return SameMapIdentity(gArm.identity, *current);
}

bool GetWriterWorldBuffer(void* runtimeManager, BYTE** world) {
    if (runtimeManager == nullptr || world == nullptr ||
        !IsReadableRange(runtimeManager, kRuntimeManagerWorldPointerOffset + sizeof(DWORD))) {
        return false;
    }
    BYTE* manager = reinterpret_cast<BYTE*>(runtimeManager);
    const DWORD length = *reinterpret_cast<const volatile DWORD*>(
        manager + kRuntimeManagerWorldLengthOffset);
    const DWORD address = *reinterpret_cast<const volatile DWORD*>(
        manager + kRuntimeManagerWorldPointerOffset);
    if (length != kExpectedWorldLength || address == 0u) return false;
    BYTE* buffer = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(address));
    if (!IsReadableRange(buffer, kWorldHeaderBytes)) return false;
    *world = buffer;
    return true;
}

// ============================================================================
// 十一、Controller API 可选接入
// ============================================================================
const CastlePadApiV1* gPadApi = nullptr;
HMODULE gPadModule = nullptr;
bool gPadPermanentFailure = false;
bool gPadMissingLogged = false;
DWORD gPadLastLookupTick = 0u;
bool gPreviousPadSaveChord = false;
bool gPreviousPadLoadChord = false;

void TryResolvePadApi() {
    if (gPadApi != nullptr || gPadPermanentFailure || !gConfig.controllerEnable) return;

    // Mod Loader 允许用户调整 ASI 加载顺序。SaveEnhance 可能先进入进程，所以“第一次没找到”
    // 不能永久判死。这里每 1000ms 最多重试一次，既兼容后加载，又不会每个 20Hz Tick 刷模块查询。
    const DWORD now = GetTickCount();
    if (gPadLastLookupTick != 0u && (now - gPadLastLookupTick) < 1000u) return;
    gPadLastLookupTick = now;

    HMODULE pad = GetModuleHandleA("Castle_PadSupport.asi");
    if (pad == nullptr) {
        if (!gPadMissingLogged) {
            ycrlog::Line("[Controller] 暂未检测到 Castle_PadSupport.asi；将低频重试，键盘 F5/F9 不受影响。");
            gPadMissingLogged = true;
        }
        return;
    }

    CastlePadGetApiFn getApi = reinterpret_cast<CastlePadGetApiFn>(
        GetProcAddress(pad, "CastlePad_GetApi"));
    if (getApi == nullptr) {
        ycrlog::Line("[Controller] 已找到 PadSupport 但没有 CastlePad_GetApi；本轮关闭手柄联动。");
        gPadPermanentFailure = true;
        return;
    }
    const CastlePadApiV1* api = getApi(CASTLE_PAD_API_VERSION_1);
    if (api == nullptr || api->magic != CASTLE_PAD_API_MAGIC ||
        api->api_version != CASTLE_PAD_API_VERSION_1 ||
        api->struct_size < sizeof(CastlePadApiV1) || api->ButtonDown == nullptr ||
        api->ActionDown == nullptr || api->ActionPressed == nullptr ||
        api->GetControlMode == nullptr || api->AllowsExternalUiInput == nullptr) {
        ycrlog::Line("[Controller] API v1 结构不完整/不兼容；本轮关闭手柄联动。");
        gPadPermanentFailure = true;
        return;
    }
    gPadModule = pad;
    gPadApi = api;
    ycrlog::Line("[Controller] 已连接 Castle_PadSupport API v1：RB+R3=快速存档，RB+Start=快速读档请求。");
}

bool PadInputAllowed() {
    return gPadApi != nullptr && gPadApi->IsReady != nullptr && gPadApi->IsConnected != nullptr &&
           gPadApi->GameForeground != nullptr && gPadApi->IsReady() != 0 &&
           gPadApi->IsConnected() != 0 && gPadApi->GameForeground() != 0 &&
           gPadApi->AllowsExternalUiInput() != 0;
}

bool PadQuickSavePressed() {
    bool chordDown = false;
    if (PadInputAllowed()) {
        chordDown = gPadApi->ButtonDown(CASTLE_PAD_BUTTON_RB) != 0 &&
                    gPadApi->ButtonDown(CASTLE_PAD_BUTTON_R3) != 0;
    }
    const bool pressed = chordDown && !gPreviousPadSaveChord;
    gPreviousPadSaveChord = chordDown;
    return pressed;
}

bool PadQuickLoadPressed() {
    bool chordDown = false;
    if (PadInputAllowed()) {
        chordDown = gPadApi->ButtonDown(CASTLE_PAD_BUTTON_RB) != 0 &&
                    gPadApi->ButtonDown(CASTLE_PAD_BUTTON_START) != 0;
    }
    const bool pressed = chordDown && !gPreviousPadLoadChord;
    gPreviousPadLoadChord = chordDown;
    return pressed;
}

// ============================================================================
// 十二、保存/读取原版链
// ============================================================================
bool GetRuntimeManager(void** manager) {
    if (manager == nullptr) return false;
    DWORD address = 0u;
    if (!ReadExeDword(kRuntimeManagerPointerRva, &address) || address == 0u) return false;
    void* result = reinterpret_cast<void*>(static_cast<SIZE_T>(address));
    if (!IsReadableRange(result, kRuntimeManagerWorldPointerOffset + sizeof(DWORD))) return false;
    *manager = result;
    return true;
}

bool PerformQuickSave() {
    MapState map;
    if (!gConfig.quickEnable || !ReadMapState(&map) || !IsStrictFreeRoamCore(map)) {
        ycrlog::Line("[快速存档] 当前不是严格自由行动状态；本次拒绝，不排队、不使用入口 fallback。");
        PlayConfiguredSound(gConfig.sound.quickSaveFailed);
        return false;
    }

    // 在调用 0x44B150 前再读一次 actor，避免恰好处于对象释放窗口。
    ActorState actor;
    if (!ReadControlledActorState(&actor) || gOriginalSavePrepare == nullptr ||
        gOriginalSaveGate == nullptr || gOriginalSaveSlot == nullptr) {
        PlayConfiguredSound(gConfig.sound.quickSaveFailed);
        return false;
    }

    gOriginalSavePrepare();
    ClearArm();
    const DWORD originalAllowed = gOriginalSaveGate(map.runtimeEntry);
    if (originalAllowed == 0u) {
        // test3 实机已经证明：即使当前通过“严格自由行动”判定，原版 save gate 仍可能返回 0。
        // 这里不能继续把 gate==0 等同于“Quick Save 一定危险”，因为严格自由行动已经排除了
        // Battle/UI/对话/地图 transient，而且 AnchorTracker 还要求连续 8 个同地图自由 Tick。
        // 因此只在当前地图确实拥有冻结锚点时授权 Writer fallback；没有锚点仍然立即失败。
        if (!ArmExtendedSaveForMap(map)) {
            ycrlog::Line("[快速存档] 原版 save gate 拒绝，且当前地图没有冻结安全锚点；本次失败。");
            PlayConfiguredSound(gConfig.sound.quickSaveFailed);
            return false;
        }
        ycrlog::Line("[快速存档] 原版 save gate 拒绝，但当前处于严格自由行动且已有安全锚点；本次使用安全 fallback 写 0 号。");
    }

    void* manager = nullptr;
    if (!GetRuntimeManager(&manager)) {
        PlayConfiguredSound(gConfig.sound.quickSaveFailed);
        return false;
    }
    const BOOL saved = gOriginalSaveSlot(manager, nullptr, kQuickSaveSlot);
    if (saved != FALSE) {
        ycrlog::Line("[快速存档] Save000.TSF 写入成功。");
        PlayConfiguredSound(gConfig.sound.quickSaveSuccess);
        return true;
    }
    ycrlog::Line("[快速存档] 原版 SaveSlot(0) 返回失败。");
    PlayConfiguredSound(gConfig.sound.quickSaveFailed);
    return false;
}

// 场景自动基准在 Quick Load 成功后重新建立，避免“刚读完快速档”被马上识别成换图并覆盖自动槽。
bool gSceneIdentityValid = false;
MapState gSceneIdentity = {};
bool gSceneAutoPending = false;
DWORD gLastAutoSaveTick = 0u;
DWORD gLastAutoAttemptTick = 0u;

void ResetAutoBaselineAfterLoad() {
    gSceneIdentityValid = false;
    gSceneAutoPending = false;
    gLastAutoSaveTick = GetTickCount();
    gLastAutoAttemptTick = 0u;
    gAnchor.identityValid = false;
    ClearArm();
}

bool PerformQuickLoad() {
    MapState map;
    if (!gConfig.quickEnable || !ReadMapState(&map) || !IsStrictFreeRoamCore(map) ||
        gOriginalLoadSlot == nullptr || gOriginalPostLoad == nullptr) {
        ycrlog::Line("[快速读档] 当前不是严格自由行动状态，或原版 Load 链不可用；本次拒绝。");
        PlayConfiguredSound(gConfig.sound.quickLoadFailed);
        return false;
    }

    void* managerVoid = nullptr;
    DWORD worldAddress = 0u;
    if (!GetRuntimeManager(&managerVoid) ||
        !ReadExeDword(kWorldGlobalPointerRva, &worldAddress) || worldAddress == 0u) {
        PlayConfiguredSound(gConfig.sound.quickLoadFailed);
        return false;
    }
    BYTE* manager = reinterpret_cast<BYTE*>(managerVoid);

    // 这里严格复刻原版隐藏 Load 命令 0x44A7D2..0x44A81A 的准备语义：
    // +0x13C = 0x27D8，+0x140 = 当前 World，然后 LoadSlot(slot)，最后 0x44B1F0 收尾。
    *reinterpret_cast<volatile DWORD*>(manager + kRuntimeManagerWorldLengthOffset) = kExpectedWorldLength;
    *reinterpret_cast<volatile DWORD*>(manager + kRuntimeManagerWorldPointerOffset) = worldAddress;
    const BOOL loaded = gOriginalLoadSlot(managerVoid, nullptr, kQuickSaveSlot);
    gOriginalPostLoad();

    if (loaded != FALSE) {
        ycrlog::Line("[快速读档] Save000.TSF 读取成功；自动存档场景基准已重置。");
        ResetAutoBaselineAfterLoad();
        PlayConfiguredSound(gConfig.sound.quickLoadSuccess);
        return true;
    }
    ycrlog::Line("[快速读档] 原版 LoadSlot(0) 返回失败；可能还没有 Save000.TSF。");
    PlayConfiguredSound(gConfig.sound.quickLoadFailed);
    return false;
}

bool PerformAutoSave(const char* reason) {
    if (!gConfig.autoEnable || gOriginalSavePrepare == nullptr || gOriginalSaveGate == nullptr ||
        gOriginalSaveSlot == nullptr) {
        return false;
    }

    MapState map;
    VanillaAnchor anchor = {};
    ActorState actor;
    if (!ReadMapState(&map) || !GetFrozenAnchor(map, &anchor) || !ReadControlledActorState(&actor)) {
        // 自动存档与 Quick 不同：这里返回 false 以后 pending 还会保留，安全后再补。
        return false;
    }

    gOriginalSavePrepare();
    ClearArm();
    const DWORD originalAllowed = gOriginalSaveGate(map.runtimeEntry);
    if (originalAllowed == 0u && !ArmExtendedSaveForMap(map)) {
        return false;
    }

    void* manager = nullptr;
    if (!GetRuntimeManager(&manager)) {
        ClearArm();
        return false;
    }

    DWORD slot = 0u;
    bool usedEmptySlot = false;
    if (!ChooseAutoSaveSlot(&slot, &usedEmptySlot)) {
        ClearArm();
        ycrlog::Line("[自动存档] 无法通过游戏文件层选择 91~99 自动槽；本次不覆盖任何自动档。");
        PlayConfiguredSound(gConfig.sound.autoSaveFailed);
        return false;
    }
    const BOOL saved = gOriginalSaveSlot(manager, nullptr, slot);
    if (saved == FALSE) {
        ClearArm();
        ycrlog::Text("[自动存档] 原版 SaveSlot 失败，原因=");
        ycrlog::Text(reason);
        ycrlog::Text("，槽=");
        ycrlog::Unsigned(slot);
        ycrlog::Line("。");
        PlayConfiguredSound(gConfig.sound.autoSaveFailed);
        return false;
    }

    ycrlog::Text("[自动存档] 成功，原因=");
    ycrlog::Text(reason);
    ycrlog::Text("，槽=");
    ycrlog::Unsigned(slot);
    ycrlog::Line("。");

    // 保存成功后立刻用“游戏自己的 File::Open(read)”回读验证。
    // 这是 test5 的关键验收：如果槽91刚保存成功，这里就必须看到91存在；下一次扫描才会走92。
    if (!GameSaveFileExists(slot)) {
        ycrlog::Text("[自动存档诊断] SaveSlot 返回成功，但游戏文件层仍无法回读刚写入的槽；槽=");
        ycrlog::Unsigned(slot);
        ycrlog::Line("。本次不推进环形游标，请保留此行。");
    } else {
        const DWORD nextSlot = NextAutoRingSlot(slot);
        SaveAutoRingState(nextSlot);
        ycrlog::Text("[自动槽] 游戏文件层已确认槽 ");
        ycrlog::Unsigned(slot);
        ycrlog::Text(" 可读取；下一个环形覆盖候选=");
        ycrlog::Unsigned(nextSlot);
        ycrlog::Line(usedEmptySlot ? "（本次填空槽）。" : "（本次环形覆盖）。");
    }

    const DWORD now = GetTickCount();
    gLastAutoSaveTick = now;
    gLastAutoAttemptTick = now;
    gSceneAutoPending = false;
    PlayConfiguredSound(gConfig.sound.autoSaveSuccess);
    return true;
}

// ============================================================================
// 十三、Quick Load 连按状态机与输入
// ============================================================================
bool gPreviousF5Down = false;
bool gPreviousF9Down = false;
DWORD gQuickLoadPressCount = 0u;
DWORD gQuickLoadDeadline = 0u;

bool KeyboardKeyPressed(int vk, bool* previousDown) {
    if (previousDown == nullptr || !ResolveUser32()) return false;
    const bool down = (gGetAsyncKeyState(vk) & static_cast<signed short>(0x8000)) != 0;
    const bool pressed = down && !*previousDown;
    *previousDown = down;
    return pressed;
}

void ResetQuickLoadConfirm() {
    gQuickLoadPressCount = 0u;
    gQuickLoadDeadline = 0u;
}

void HandleQuickLoadRequest() {
    MapState map;
    if (!ReadMapState(&map) || !IsStrictFreeRoamCore(map)) {
        ResetQuickLoadConfirm();
        ycrlog::Line("[快速读档] 请求发生时不是严格自由行动；确认状态清零。");
        PlayConfiguredSound(gConfig.sound.quickLoadFailed);
        return;
    }

    const DWORD now = GetTickCount();
    if (gQuickLoadPressCount == 0u || static_cast<LONG>(now - gQuickLoadDeadline) > 0) {
        gQuickLoadPressCount = 1u;
        gQuickLoadDeadline = now + gConfig.quickLoadWindowMs;
        ycrlog::Text("[快速读档] 第 1/");
        ycrlog::Unsigned(gConfig.quickLoadPresses);
        ycrlog::Line(" 次确认；等待下一次请求。");
        PlayConfiguredSound(gConfig.sound.quickLoadConfirm);
        return;
    }

    ++gQuickLoadPressCount;
    if (gQuickLoadPressCount < gConfig.quickLoadPresses) {
        gQuickLoadDeadline = now + gConfig.quickLoadWindowMs;
        PlayConfiguredSound(gConfig.sound.quickLoadConfirm);
        return;
    }

    ResetQuickLoadConfirm();
    PerformQuickLoad();
}

void PollQuickInputs() {
    if (!gConfig.quickEnable) {
        ResetQuickLoadConfirm();
        return;
    }

    TryResolvePadApi();
    const bool keyboardForeground = GameIsForegroundForKeyboard();
    const bool f5 = keyboardForeground && KeyboardKeyPressed(VK_F5, &gPreviousF5Down);
    const bool f9 = keyboardForeground && KeyboardKeyPressed(VK_F9, &gPreviousF9Down);

    // 即使游戏不在前台，也要更新 down 状态，避免切回游戏后把“早就按住的键”误当新按下。
    if (!keyboardForeground && ResolveUser32()) {
        KeyboardKeyPressed(VK_F5, &gPreviousF5Down);
        KeyboardKeyPressed(VK_F9, &gPreviousF9Down);
    }

    const bool padSave = gConfig.controllerEnable && PadQuickSavePressed();
    const bool padLoad = gConfig.controllerEnable && PadQuickLoadPressed();

    if (f5 || padSave) PerformQuickSave();
    if (f9 || padLoad) HandleQuickLoadRequest();

    // 确认窗口过期后主动清零。无输入时也不会让几分钟前的第一次 F9 留着。
    if (gQuickLoadPressCount != 0u) {
        const DWORD now = GetTickCount();
        if (static_cast<LONG>(now - gQuickLoadDeadline) > 0) ResetQuickLoadConfirm();
    }
}

// ============================================================================
// 十四、场景/定时自动存档调度
// ============================================================================
void UpdateSceneIdentityAndAutoPending(const MapState& map) {
    if (!gSceneIdentityValid) {
        // 插件第一次看到一张地图，或者刚 Quick Load 后第一次看到地图，只建立基准。
        gSceneIdentity = map;
        gSceneIdentityValid = true;
        gSceneAutoPending = false;
        if (gLastAutoSaveTick == 0u) gLastAutoSaveTick = GetTickCount();
        return;
    }
    if (!SameMapIdentity(gSceneIdentity, map)) {
        gSceneIdentity = map;
        if (gConfig.autoEnable && gConfig.saveOnSceneChange) {
            gSceneAutoPending = true;
            ycrlog::Line("[自动存档] 检测到真正地图身份变化；等待新地图建立安全锚点后保存。");
        }
    }
}

void MaybeRunAutoSave() {
    if (!gConfig.autoEnable) return;
    const DWORD now = GetTickCount();
    const DWORD retryMs = 5000u;
    if (gLastAutoAttemptTick != 0u && (now - gLastAutoAttemptTick) < retryMs) return;

    bool timedDue = false;
    if (gConfig.intervalMinutes != 0u) {
        const DWORD intervalMs = gConfig.intervalMinutes * 60000u;
        timedDue = (now - gLastAutoSaveTick) >= intervalMs;
    }
    if (!gSceneAutoPending && !timedDue) return;

    // 先只检查当前地图是否已有 frozen anchor。没有时不记“失败尝试”，让第 8 个稳定 Tick
    // 一形成锚点就能立刻补存，而不是又多等 5 秒。
    MapState map;
    VanillaAnchor anchor = {};
    if (!ReadMapState(&map) || !GetFrozenAnchor(map, &anchor)) return;

    gLastAutoAttemptTick = now;
    PerformAutoSave(gSceneAutoPending ? "场景切换" : "定时间隔");
}

// ============================================================================
// 十四点五、保留槽手柄焦点：只使用 PadSupport Public API v1
// ============================================================================
SaveActionHitFunction gSaveActionHitNext = nullptr;
SaveActionEventFunction gSaveActionEventNext = nullptr;
BYTE gSaveActionHitOriginalCall[5] = {};
BYTE gSaveActionEventOriginalCall[5] = {};
bool gSaveActionPadHooksInstalled = false;
bool gSaveActionPadHookFailureLogged = false;

// current 只在一次原版 SaveAction::Update 调用期间有效，wrapper 不会跨 UI 生命周期解引用它。
BYTE* gCurrentReservedAction = nullptr;
BYTE* gCurrentReservedButtons[kSaveActionButtonCount] = {};

// tracked 只用来判断“是不是同一个动作窗口”，不直接解引用。真正按钮每帧都从当前 action 重取。
BYTE* gTrackedReservedAction = nullptr;
DWORD gReservedPadFocus = kSaveActionLoadIndex;
LONG gReservedPadConfirmPending = -1;
bool gReservedPadNavActive = false;
bool gReservedPadInputArmed = false;

extern "C" BYTE __fastcall ReservedSaveActionHitHook(void* button, void* unusedEdx);
extern "C" LONG __fastcall ReservedSaveActionEventHook(void* button, void* unusedEdx);

bool AddressBelongsToModule(const void* address, HMODULE module) {
    // VirtualQuery 返回该地址所属整块映像的 AllocationBase。它等于 HMODULE 时，才能证明
    // 当前 CALL 目标确实在 PadSupport 内，而不是另一个未知插件碰巧写了同一位置。
    if (address == nullptr || module == nullptr) return false;
    MEMORY_BASIC_INFORMATION_MINI info = {};
    if (VirtualQuery(address, &info, sizeof(info)) == 0u) return false;
    return info.AllocationBase == module;
}

void* ReadRelativeCallTarget(BYTE* site) {
    if (site == nullptr || !IsReadableRange(site, 5u) || site[0] != 0xE8u) return nullptr;
    const LONG displacement = *reinterpret_cast<const volatile LONG*>(site + 1u);
    return site + 5u + displacement;
}

void BuildRelativeCall(BYTE* site, const void* target, BYTE out[5]) {
    out[0] = 0xE8u;
    const SIZE_T nextInstruction = reinterpret_cast<SIZE_T>(site + 5u);
    const SIZE_T destination = reinterpret_cast<SIZE_T>(target);
    *reinterpret_cast<DWORD*>(out + 1u) = static_cast<DWORD>(destination - nextInstruction);
}

bool PadHookTargetAllowed(void* target, DWORD originalVa) {
    const DWORD address = static_cast<DWORD>(reinterpret_cast<SIZE_T>(target));
    return address == originalVa || AddressBelongsToModule(target, gPadModule);
}

bool TryInstallReservedSaveActionPadHooks() {
    // 没有公开 API 时，原版键鼠仍会尊重 disabled；因此绝不能为了手柄视觉把 PadSupport
    // 变成强制依赖。只有 API 和模块都已确认后才尝试链式安装。
    if (gSaveActionPadHooksInstalled) return true;
    if (gPadApi == nullptr || gPadModule == nullptr || gExeBase == nullptr) return false;

    BYTE* hitSite = gExeBase + kSaveActionHitCallRva;
    BYTE* eventSite = gExeBase + kSaveActionEventCallRva;
    void* hitTarget = ReadRelativeCallTarget(hitSite);
    void* eventTarget = ReadRelativeCallTarget(eventSite);
    if (!PadHookTargetAllowed(hitTarget, kOriginalButtonHitVa) ||
        !PadHookTargetAllowed(eventTarget, kOriginalButtonEventVa)) {
        if (!gSaveActionPadHookFailureLogged) {
            ycrlog::Line("[保留槽手柄] SaveAction Hit/Event 当前目标不是原版或 PadSupport；不覆盖未知插件。");
            gSaveActionPadHookFailureLogged = true;
        }
        return false;
    }

    // 先保存完整原 CALL 字节和 next 目标。普通槽 wrapper 会继续调用它们，绝不复制 PadSupport 逻辑。
    for (SIZE_T i = 0u; i < 5u; ++i) {
        gSaveActionHitOriginalCall[i] = hitSite[i];
        gSaveActionEventOriginalCall[i] = eventSite[i];
    }
    gSaveActionHitNext = reinterpret_cast<SaveActionHitFunction>(hitTarget);
    gSaveActionEventNext = reinterpret_cast<SaveActionEventFunction>(eventTarget);

    BYTE hitPatch[5];
    BYTE eventPatch[5];
    BuildRelativeCall(hitSite, reinterpret_cast<const void*>(&ReservedSaveActionHitHook), hitPatch);
    BuildRelativeCall(eventSite, reinterpret_cast<const void*>(&ReservedSaveActionEventHook), eventPatch);
    if (!ycr::WriteBytes(hitSite, hitPatch, 5u)) {
        gSaveActionHitNext = nullptr;
        gSaveActionEventNext = nullptr;
        return false;
    }
    if (!ycr::WriteBytes(eventSite, eventPatch, 5u)) {
        ycr::WriteBytes(hitSite, gSaveActionHitOriginalCall, 5u);
        gSaveActionHitNext = nullptr;
        gSaveActionEventNext = nullptr;
        return false;
    }

    gSaveActionPadHooksInstalled = true;
    ycrlog::Line("[保留槽手柄] 已在 SaveEnhance 内接管保留槽两项焦点；普通槽继续链回原目标。");
    return true;
}

void RestoreReservedSaveActionPadHooks() {
    // 只在 CALL 仍然指向本插件时恢复。若后来又有其它插件合法接管，绝不能用旧字节盖回去。
    if (!gSaveActionPadHooksInstalled || gExeBase == nullptr) return;
    BYTE* hitSite = gExeBase + kSaveActionHitCallRva;
    BYTE* eventSite = gExeBase + kSaveActionEventCallRva;
    if (ReadRelativeCallTarget(hitSite) == reinterpret_cast<void*>(&ReservedSaveActionHitHook)) {
        ycr::WriteBytes(hitSite, gSaveActionHitOriginalCall, 5u);
    }
    if (ReadRelativeCallTarget(eventSite) == reinterpret_cast<void*>(&ReservedSaveActionEventHook)) {
        ycr::WriteBytes(eventSite, gSaveActionEventOriginalCall, 5u);
    }
    gSaveActionPadHooksInstalled = false;
}

void ResetReservedPadState() {
    gCurrentReservedAction = nullptr;
    for (DWORD i = 0u; i < kSaveActionButtonCount; ++i) gCurrentReservedButtons[i] = nullptr;
    gTrackedReservedAction = nullptr;
    gReservedPadFocus = kSaveActionLoadIndex;
    gReservedPadConfirmPending = -1;
    gReservedPadNavActive = false;
    gReservedPadInputArmed = false;
}

bool ReservedPadActionsDown() {
    if (gPadApi == nullptr || gPadApi->ActionDown == nullptr) return false;
    return gPadApi->ActionDown(CASTLE_PAD_ACTION_CONFIRM) != 0 ||
           gPadApi->ActionDown(CASTLE_PAD_ACTION_CANCEL) != 0 ||
           gPadApi->ActionDown(CASTLE_PAD_ACTION_NAV_UP) != 0 ||
           gPadApi->ActionDown(CASTLE_PAD_ACTION_NAV_DOWN) != 0;
}

void UpdateReservedPadInput(bool newAction) {
    if (gCurrentReservedAction == nullptr) return;
    if (!PadInputAllowed() || gPadApi->GetControlMode() != CASTLE_PAD_CONTROL_CONTROLLER) {
        // PadSupport 已进入鼠标/调查独占模式时立即放弃强制焦点，让 wrapper 完整链回原目标。
        gReservedPadNavActive = false;
        gReservedPadConfirmPending = -1;
        gReservedPadInputArmed = false;
        return;
    }

    // 新窗口若由 A 打开，公开快照里 CONFIRM 可能仍处于按住状态。先取得两项导航所有权，
    // 但必须等 A/方向全部释放后再武装，避免同一个 A 立即执行默认“读档”。
    if (newAction) {
        gReservedPadNavActive = ReservedPadActionsDown();
        gReservedPadInputArmed = !ReservedPadActionsDown();
        gReservedPadFocus = kSaveActionLoadIndex;
        gReservedPadConfirmPending = -1;
    }
    if (!gReservedPadInputArmed) {
        if (!ReservedPadActionsDown()) gReservedPadInputArmed = true;
        return;
    }

    const bool up = gPadApi->ActionPressed(CASTLE_PAD_ACTION_NAV_UP) != 0;
    const bool down = gPadApi->ActionPressed(CASTLE_PAD_ACTION_NAV_DOWN) != 0;
    const bool confirm = gPadApi->ActionPressed(CASTLE_PAD_ACTION_CONFIRM) != 0;
    const bool cancel = gPadApi->ActionPressed(CASTLE_PAD_ACTION_CANCEL) != 0;
    if (up || down || confirm || cancel) gReservedPadNavActive = true;
    if (up) gReservedPadFocus = kSaveActionLoadIndex;
    if (down) gReservedPadFocus = kSaveActionCancelIndex;
    if (confirm && gReservedPadNavActive) {
        gReservedPadConfirmPending = static_cast<LONG>(gReservedPadFocus);
    }
    if (cancel) {
        gReservedPadFocus = kSaveActionCancelIndex;
        gReservedPadConfirmPending = static_cast<LONG>(kSaveActionCancelIndex);
    }
}

bool CurrentReservedButtonIndex(void* button, DWORD* indexOut) {
    if (button == nullptr || indexOut == nullptr || gCurrentReservedAction == nullptr) return false;
    for (DWORD i = 0u; i < kSaveActionButtonCount; ++i) {
        if (button == gCurrentReservedButtons[i]) {
            *indexOut = i;
            return true;
        }
    }
    return false;
}

extern "C" BYTE __fastcall ReservedSaveActionHitHook(void* button, void* unusedEdx) {
    DWORD index = 0u;
    if (gReservedPadNavActive && CurrentReservedButtonIndex(button, &index)) {
        // 保留槽只有“读档/取消”两项；index2=存档无论 PadSupport 私有焦点是什么都不会命中。
        return static_cast<BYTE>(index == gReservedPadFocus && index != kSaveActionSaveIndex);
    }
    return gSaveActionHitNext != nullptr ? gSaveActionHitNext(button, unusedEdx) : 0u;
}

extern "C" LONG __fastcall ReservedSaveActionEventHook(void* button, void* unusedEdx) {
    DWORD index = 0u;
    if (gReservedPadNavActive && CurrentReservedButtonIndex(button, &index)) {
        // 保留槽直接调用原版 ButtonEvent，绕开 PadSupport 私有 pending；真实鼠标事件仍由原版优先返回。
        SaveActionEventFunction original = reinterpret_cast<SaveActionEventFunction>(
            static_cast<SIZE_T>(kOriginalButtonEventVa));
        const LONG real = original(button, unusedEdx);
        if (real != 0) {
            gReservedPadConfirmPending = -1;
            return real;
        }
        if (gReservedPadConfirmPending == static_cast<LONG>(index) &&
            index != kSaveActionSaveIndex && index == gReservedPadFocus) {
            gReservedPadConfirmPending = -1;
            return 2;
        }
        return 0;
    }
    return gSaveActionEventNext != nullptr ? gSaveActionEventNext(button, unusedEdx) : 0;
}

// ============================================================================
// 十五、保留槽 UI：同时约束原版鼠标/键盘和 Controller SaveAction Event
// ============================================================================
bool IsReservedSlot(DWORD slot) {
    return slot == kQuickSaveSlot || (slot >= kAutoSlotFirst && slot <= kAutoSlotLast);
}

// SaveAction Update 期间临时覆盖“存档”按钮 disabled 的状态。
// 这三个变量只在一次 Update 调用前后短暂有效，不跨 UI 生命周期保存对象指针。
BYTE* gOverriddenSaveButton = nullptr;
BYTE gOverriddenSaveButtonOriginal = 0u;
bool gSaveButtonOverrideActive = false;

// test2 不再保存“曾经构造过的 SaveSlot 指针”。那种缓存会引入两个问题：
// 1. constructor Hook 本身可能和其它插件冲突；
// 2. UI 销毁后缓存的是裸指针，虽然 test1 每次都会 VirtualQuery，但长期仍比现取 owner 复杂。
// 现在每一帧只从三个已经闭合的当前 owner 找 SaveSlot，完全不保留跨帧对象指针。
BYTE* SaveSlotFromOwnerGlobal(DWORD ownerGlobalRva, SIZE_T saveSlotOffset) {
    DWORD ownerAddress = 0u;
    if (!ReadExeDword(ownerGlobalRva, &ownerAddress) || ownerAddress == 0u) {
        return nullptr;
    }

    // owner+offset 保存的是一个 32 位 SaveSlot* 指针字段，不是 SaveSlot 对象直接内嵌在 owner 中。
    // 旧正式版错误地把“字段所在地址”当成 SaveSlot 起点，所以后续读取 +0x5A4 永远对不上
    // 当前 SaveAction，最终就没有机会把 0/91~99 的存档按钮设成 disabled。
    BYTE* owner = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(ownerAddress));
    if (!IsReadableRange(owner + saveSlotOffset, sizeof(DWORD))) {
        return nullptr;
    }

    DWORD saveSlotAddress = 0u;
    if (!ReadDword(owner + saveSlotOffset, &saveSlotAddress) || saveSlotAddress == 0u) {
        return nullptr;
    }

    // 解引用以后得到的才是真正 SaveSlot 对象。再验证到 +0x5A4 字段末尾，避免 owner 正在
    // 销毁或字段已经变成坏指针时继续读取游戏内存。
    BYTE* saveSlot = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(saveSlotAddress));
    if (!IsReadableRange(saveSlot, kSaveSlotActionPointerOffset + sizeof(DWORD))) {
        return nullptr;
    }
    return saveSlot;
}

BYTE* FindSaveSlotForAction(void* action) {
    if (action == nullptr) return nullptr;
    const DWORD wanted = static_cast<DWORD>(reinterpret_cast<SIZE_T>(action));

    // 三条 owner 路径按“最常见探索 UI -> 存档点 -> 标题”顺序尝试。
    // 每条都只读 action pointer；不写 owner，也不修改 Controller 的任何内部状态。
    const DWORD ownerRvas[3] = {kInterfaceUiRva, kSavePointUiRva, kTitleUiRva};
    const SIZE_T slotOffsets[3] = {
        kInterfaceSaveSlotOffset, kSavePointSaveSlotOffset, kTitleSaveSlotOffset};
    for (SIZE_T i = 0u; i < 3u; ++i) {
        BYTE* saveSlot = SaveSlotFromOwnerGlobal(ownerRvas[i], slotOffsets[i]);
        if (saveSlot == nullptr) continue;
        DWORD actionAddress = 0u;
        if (ReadDword(saveSlot + kSaveSlotActionPointerOffset, &actionAddress) &&
            actionAddress == wanted) {
            return saveSlot;
        }
    }
    return nullptr;
}

void EndSaveButtonOverride() {
    // disabled 只需要覆盖“这一帧原版 SaveActionUpdate 正在扫描按钮”的短窗口。
    // 这样鼠标/键盘以及 Controller 安装在 0x4262C0 内部的 Hit/Event Hook 都会看到 disabled=1；
    // Update 返回后立即恢复原值，不把已销毁 UI 的裸指针留到下一次窗口再写，避免 use-after-free。
    if (gSaveButtonOverrideActive && gOverriddenSaveButton != nullptr &&
        IsReadableRange(gOverriddenSaveButton + kButtonDisabledOffset, 1u)) {
        *reinterpret_cast<volatile BYTE*>(gOverriddenSaveButton + kButtonDisabledOffset) =
            gOverriddenSaveButtonOriginal;
    }
    gOverriddenSaveButton = nullptr;
    gOverriddenSaveButtonOriginal = 0u;
    gSaveButtonOverrideActive = false;
    gCurrentReservedAction = nullptr;
    for (DWORD i = 0u; i < kSaveActionButtonCount; ++i) gCurrentReservedButtons[i] = nullptr;
}

void PrepareReservedSaveButtonState(void* action) {
    EndSaveButtonOverride();

    BYTE* parent = FindSaveSlotForAction(action);
    if (parent == nullptr) {
        ResetReservedPadState();
        return;
    }

    DWORD row = 0u, page = 0u;
    if (!ReadDword(parent + kSaveSlotSelectedRowOffset, &row) ||
        !ReadDword(parent + kSaveSlotPageBaseOffset, &page) || row >= 4u || page > kLastPageBase) {
        ResetReservedPadState();
        return;
    }
    const DWORD slotNumber = page + row;
    if (!IsReservedSlot(slotNumber)) {
        ResetReservedPadState();
        return;
    }

    BYTE* actionBytes = reinterpret_cast<BYTE*>(action);
    BYTE* buttons[kSaveActionButtonCount] = {};
    for (DWORD i = 0u; i < kSaveActionButtonCount; ++i) {
        DWORD buttonAddress = 0u;
        if (!ReadDword(actionBytes + kSaveActionButtonsOffset + i * sizeof(DWORD), &buttonAddress) ||
            buttonAddress == 0u) {
            ResetReservedPadState();
            return;
        }
        buttons[i] = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(buttonAddress));
        if (!IsReadableRange(buttons[i] + kButtonDisabledOffset, 1u)) {
            ResetReservedPadState();
            return;
        }
    }

    const bool newAction = gTrackedReservedAction != actionBytes;
    gTrackedReservedAction = actionBytes;
    gCurrentReservedAction = actionBytes;
    for (DWORD i = 0u; i < kSaveActionButtonCount; ++i) gCurrentReservedButtons[i] = buttons[i];

    BYTE* saveButton = buttons[kSaveActionSaveIndex];
    gOverriddenSaveButton = saveButton;
    gOverriddenSaveButtonOriginal =
        *reinterpret_cast<const volatile BYTE*>(saveButton + kButtonDisabledOffset);
    *reinterpret_cast<volatile BYTE*>(saveButton + kButtonDisabledOffset) = 1u;
    gSaveButtonOverrideActive = true;
    UpdateReservedPadInput(newAction);
}

// ============================================================================
// 十六、兼容旧“无条件存档”补丁并验证目标
// ============================================================================
bool EnsureOriginalSaveGateFunction() {
    BYTE* start = gExeBase + kOriginalSaveGateFunctionRva;
    if (ycr::BytesEqual(start, kOriginalSaveGateFunctionBytes, 9u)) return true;
    if (ycr::BytesEqual(start, kOldUnsafeSaveGateFunctionBytes, 9u)) {
        ycrlog::Line("[启动] 检测到历史无条件随时存档机器码；先恢复原版 save gate。");
        return ycr::WriteBytes(start, kOriginalSaveGateFunctionBytes, 9u);
    }
    // save gate 本体若是第三种未知状态，也要像其它预检查点一样把字节完整打印出来。
    // 这样用户只贴一份日志就能判断是不是另一个旧随时存档插件先改了函数本体。
    ycrlog::Line("[预检查冲突] 原版 save gate 0x40A0C0 不是已确认原版/历史补丁。");
    ycrlog::Text("    原版期望=");
    ycrlog::Bytes(kOriginalSaveGateFunctionBytes, 9u);
    ycrlog::Line("");
    ycrlog::Text("    当前实际=");
    ycrlog::Bytes(start, 9u);
    ycrlog::Line("");
    return false;
}

const char* KnownModuleNameForAddress(const void* address, DWORD* moduleBaseOut) {
    if (moduleBaseOut != nullptr) *moduleBaseOut = 0u;
    if (address == nullptr) return "<null>";

    // VirtualQuery 可以告诉我们“这段内存由哪个 AllocationBase 拥有”。DLL 装载后的代码页
    // AllocationBase 就是模块基址。这样即使某个 CALL 已被其它 ASI 改掉，也能在日志里尽量指出是谁。
    MEMORY_BASIC_INFORMATION_MINI info = {};
    if (VirtualQuery(address, &info, sizeof(info)) == 0u || info.AllocationBase == nullptr) {
        return "<VirtualQuery失败>";
    }
    const DWORD ownerBase = static_cast<DWORD>(reinterpret_cast<SIZE_T>(info.AllocationBase));
    if (moduleBaseOut != nullptr) *moduleBaseOut = ownerBase;

    // 这里只列 CastleReforge 当前常见模块。找不到名字也会打印模块基址和目标地址，所以不会丢证据。
    const char* names[] = {
        "Castle_SaveEnhance.asi",
        "AnytimeSave.asi",
        "Castle_PadSupport.asi",
        "Castle_Widescreen.asi",
        "Castle_FPSUnlock.asi",
        "BugFix.asi",
        "BUGFix.asi",
        "NoCD.asi",
        "MaxGrowthAndDrop.asi",
        "RPG.exe"};
    for (SIZE_T i = 0u; i < sizeof(names) / sizeof(names[0]); ++i) {
        HMODULE module = GetModuleHandleA(names[i]);
        if (module != nullptr &&
            static_cast<DWORD>(reinterpret_cast<SIZE_T>(module)) == ownerBase) {
            return names[i];
        }
    }

    // 当前 EXE 用 nullptr 取得，文件名不一定能用 "RPG.exe" 成功查询，所以再单独比较一次。
    HMODULE exe = GetModuleHandleW(nullptr);
    if (exe != nullptr && static_cast<DWORD>(reinterpret_cast<SIZE_T>(exe)) == ownerBase) {
        return "当前RPG.exe";
    }
    return "<未知模块>";
}

void LogCallTargetIfPresent(const BYTE* site, SIZE_T size) {
    if (site == nullptr || size < 5u || site[0] != 0xE8u) return;
    // x86 E8 后面的四字节是“相对下一条指令”的有符号位移。
    // 用 DWORD 做 32 位环绕加法，在 32 位进程里能得到最终 CALL 目标地址。
    const LONG displacement = *reinterpret_cast<const volatile LONG*>(site + 1u);
    const DWORD next = static_cast<DWORD>(reinterpret_cast<SIZE_T>(site + 5u));
    const DWORD target = next + static_cast<DWORD>(displacement);
    DWORD moduleBase = 0u;
    const char* moduleName = KnownModuleNameForAddress(
        reinterpret_cast<const void*>(static_cast<SIZE_T>(target)), &moduleBase);
    ycrlog::Text("    当前CALL目标=");
    ycrlog::Hex(target);
    ycrlog::Text(" 模块=");
    ycrlog::Text(moduleName);
    ycrlog::Text(" 模块基址=");
    ycrlog::Hex(moduleBase);
    ycrlog::Line("");
}

bool CheckOriginalSite(
    const char* label,
    DWORD rva,
    const BYTE* expected,
    SIZE_T size,
    bool explainCallTarget) {
    if (gExeBase == nullptr || expected == nullptr || size == 0u) return false;
    const BYTE* actual = gExeBase + rva;
    if (ycr::BytesEqual(actual, expected, size)) return true;

    // test1 最大的问题就是把十几项检查写在一个 || 表达式里，用户只知道“失败”，不知道哪一项。
    // test2 每一项都把完整证据打到日志，下一次无需再做专门诊断版。
    ycrlog::Text("[预检查冲突] ");
    ycrlog::Text(label);
    ycrlog::Text(" 地址=");
    ycrlog::Hex(static_cast<DWORD>(reinterpret_cast<SIZE_T>(actual)));
    ycrlog::Line("");
    ycrlog::Text("    期望=");
    ycrlog::Bytes(expected, size);
    ycrlog::Line("");
    ycrlog::Text("    实际=");
    ycrlog::Bytes(actual, size);
    ycrlog::Line("");
    if (explainCallTarget) LogCallTargetIfPresent(actual, size);
    return false;
}

bool CheckVtableSite() {
    const BYTE* actual = gExeBase + kSaveActionUpdateVtableRva;
    if (ycr::BytesEqual(actual, kSaveActionUpdateVtableOriginalBytes, 4u)) return true;

    ycrlog::Text("[预检查冲突] SaveAction Update vtable 地址=");
    ycrlog::Hex(static_cast<DWORD>(reinterpret_cast<SIZE_T>(actual)));
    ycrlog::Line("");
    ycrlog::Text("    期望=");
    ycrlog::Bytes(kSaveActionUpdateVtableOriginalBytes, 4u);
    ycrlog::Line("");
    ycrlog::Text("    实际=");
    ycrlog::Bytes(actual, 4u);
    ycrlog::Line("");

    const DWORD target = *reinterpret_cast<const volatile DWORD*>(actual);
    DWORD moduleBase = 0u;
    const char* moduleName = KnownModuleNameForAddress(
        reinterpret_cast<const void*>(static_cast<SIZE_T>(target)), &moduleBase);
    ycrlog::Text("    当前vtable目标=");
    ycrlog::Hex(target);
    ycrlog::Text(" 模块=");
    ycrlog::Text(moduleName);
    ycrlog::Text(" 模块基址=");
    ycrlog::Hex(moduleBase);
    ycrlog::Line("");
    return false;
}

bool PrecheckAllHookSites() {
    bool ok = true;

    // 固定菜单补丁也是“真正要写的点”，所以仍然 fail-closed；只是现在逐项报告。
    const char* fixedNames[] = {
        "LoadSlot槽号0基",
        "SaveSlot槽号0基",
        "聚焦控件槽号0基",
        "元数据槽号0基",
        "选择标签槽号0基",
        "行标签槽号0基",
        "菜单页数25",
        "翻页箭头页数25",
        "第一页上一页箭头可用",
        "末页下一页箭头可用"};
    for (SIZE_T i = 0u; i < kFixedMenuPatchCount; ++i) {
        const ycr::Patch& patch = kFixedMenuPatches[i];
        if (!CheckOriginalSite(fixedNames[i], patch.rva, patch.original, patch.size, false)) {
            ok = false;
        }
    }

    // 这里只保留 test2 真正会覆写的 5 个 CALL。test1 的三个 SaveSlot constructor CALL
    // 已删除，因为父 SaveSlot 现在从三个已闭合 owner 路径只读定位，不再需要修改构造链。
    if (!CheckOriginalSite("正常菜单保存许可CALL", kNormalMenuSaveGateCallRva,
                           kNormalMenuSaveGateCallBytes, 5u, true)) ok = false;
    if (!CheckOriginalSite("Map Tick CALL", kMapTickCallRva,
                           kMapTickCallBytes, 5u, true)) ok = false;
    if (!CheckOriginalSite("SaveSlot Writer CALL", kSaveWriterCallRva,
                           kSaveWriterCallBytes, 5u, true)) ok = false;
    if (!CheckOriginalSite("菜单手动SaveSlot CALL", kMenuSaveCallRva,
                           kMenuSaveCallBytes, 5u, true)) ok = false;
    if (!CheckOriginalSite("隐藏命令SaveSlot CALL", kCommandSaveCallRva,
                           kCommandSaveCallBytes, 5u, true)) ok = false;

    if (!CheckOriginalSite("上一页循环page-base读取", kPrevPageBaseReadRva,
                           kPrevPageBaseReadBytes, 6u, true)) ok = false;
    if (!CheckOriginalSite("下一页循环page-base读取", kNextPageBaseReadRva,
                           kNextPageBaseReadBytes, 6u, true)) ok = false;
    if (!CheckVtableSite()) ok = false;

    if (!ok) {
        ycrlog::Line("[预检查] 至少一个真正需要写入的地址已不是目标原版状态；为避免覆盖其它插件，拒绝安装。上面的逐地址日志就是下一步证据。");
    }
    return ok;
}

// ============================================================================
// 十七、Hook wrappers 的前置声明
// ============================================================================
extern "C" DWORD __fastcall SafeSaveGateHook(void* runtimeEntry);
extern "C" void __fastcall SafeMapTickHook(void* sceneContainer, void* unusedEdx);
extern "C" BOOL __fastcall SafeSaveWriterHook(void* runtimeManager, void* unusedEdx, const char* path);
extern "C" BOOL __fastcall ProtectedManualSaveHook(void* runtimeManager, void* unusedEdx, DWORD slot);
extern "C" void __fastcall SaveActionUpdateHook(void* action, void* unusedEdx);
// MSVC 只允许把 naked 属性写在“函数定义”上，不能写在这种前置声明上。
// 这里先告诉编译器函数名称和参数即可；文件后面的真正定义仍然保留
// __declspec(naked)，所以生成的裸汇编入口不会发生任何行为变化。
extern "C" void PrevPageBaseLoopHelper();
extern "C" void NextPageBaseLoopHelper();

// 启动中途失败时用的“本轮是否已写过”标记。只有本插件刚刚改过的内容才恢复。
bool gFixedPatchesInstalled = false;
bool gPrevPageHookInstalled = false;
bool gNextPageHookInstalled = false;
bool gCoreHooksInstalled[3] = {};
bool gManualHooksInstalled[2] = {};
bool gVtableInstalled = false;

void RestoreCallIfInstalled(bool installed, DWORD rva, const BYTE* original, SIZE_T size) {
    if (installed) ycr::WriteBytes(gExeBase + rva, original, size);
}

void RollbackStartupInstall() {
    if (gVtableInstalled) {
        ycr::WriteBytes(gExeBase + kSaveActionUpdateVtableRva,
                        kSaveActionUpdateVtableOriginalBytes, 4u);
        gVtableInstalled = false;
    }
    RestoreCallIfInstalled(gManualHooksInstalled[1], kCommandSaveCallRva, kCommandSaveCallBytes, 5u);
    RestoreCallIfInstalled(gManualHooksInstalled[0], kMenuSaveCallRva, kMenuSaveCallBytes, 5u);
    RestoreCallIfInstalled(gCoreHooksInstalled[2], kSaveWriterCallRva, kSaveWriterCallBytes, 5u);
    RestoreCallIfInstalled(gCoreHooksInstalled[1], kMapTickCallRva, kMapTickCallBytes, 5u);
    RestoreCallIfInstalled(gCoreHooksInstalled[0], kNormalMenuSaveGateCallRva, kNormalMenuSaveGateCallBytes, 5u);
    RestoreCallIfInstalled(gNextPageHookInstalled, kNextPageBaseReadRva, kNextPageBaseReadBytes, 6u);
    RestoreCallIfInstalled(gPrevPageHookInstalled, kPrevPageBaseReadRva, kPrevPageBaseReadBytes, 6u);
    if (gFixedPatchesInstalled) {
        ycr::RestorePatchSetToOriginal(gExeBase, kFixedMenuPatches, kFixedMenuPatchCount);
        gFixedPatchesInstalled = false;
    }
}

bool InstallCall5FailClosed(DWORD rva, const BYTE expected[5], const void* target) {
    if (ycr::InstallRelativeCall(gExeBase, rva, expected, target)) return true;
    // InstallRelativeCall 可能“字节已写、但 FlushInstructionCache/恢复页保护失败”后返回 false。
    // 当前条目因此也要尝试恢复，不能只靠外层已成功标记。
    ycr::WriteBytes(gExeBase + rva, expected, 5u);
    return false;
}

bool InstallCall6FailClosed(DWORD rva, const BYTE expected[6], const void* target) {
    if (ycr::InstallRelativeCall6(gExeBase, rva, expected, target)) return true;
    ycr::WriteBytes(gExeBase + rva, expected, 6u);
    return false;
}

bool InstallAllHooks() {
    if (GetModuleHandleA("AnytimeSave.asi") != nullptr) {
        ycrlog::Line("[启动失败] 检测到旧 AnytimeSave.asi；SaveEnhance 已包含它，禁止同时加载两个存档插件。");
        return false;
    }
    if (!EnsureOriginalSaveGateFunction() || !PrecheckAllHookSites()) {
        ycrlog::Line("[启动失败] SaveEnhance 必需写入点预检查失败；未开始安装；请看上方逐地址冲突详情。");
        return false;
    }

    // 把所有原版函数地址一次性固定下来，后面的 wrapper 只调用这些原版入口。
    gOriginalSaveGate = reinterpret_cast<OriginalSaveGateFunction>(gExeBase + kOriginalSaveGateFunctionRva);
    gOriginalMapTick = reinterpret_cast<OriginalMapTickFunction>(gExeBase + kOriginalMapTickFunctionRva);
    gOriginalSaveWriter = reinterpret_cast<OriginalSaveWriterFunction>(gExeBase + kOriginalSaveWriterFunctionRva);
    gOriginalSaveSlot = reinterpret_cast<OriginalSaveSlotFunction>(gExeBase + kOriginalSaveSlotFunctionRva);
    gOriginalLoadSlot = reinterpret_cast<OriginalLoadSlotFunction>(gExeBase + kOriginalLoadSlotFunctionRva);
    gOriginalSavePrepare = reinterpret_cast<OriginalNoArgFunction>(gExeBase + kOriginalSavePrepareFunctionRva);
    gOriginalPostLoad = reinterpret_cast<OriginalNoArgFunction>(gExeBase + kOriginalPostLoadFunctionRva);
    gOriginalSaveActionUpdate = reinterpret_cast<OriginalSaveActionUpdateFunction>(
        static_cast<SIZE_T>(kSaveActionUpdateOriginalVa));
    gGameFileCtor = reinterpret_cast<GameFileCtorFunction>(gExeBase + kGameFileCtorFunctionRva);
    gGameFileDtor = reinterpret_cast<GameFileDtorFunction>(gExeBase + kGameFileDtorFunctionRva);
    gGameFileOpen = reinterpret_cast<GameFileOpenFunction>(gExeBase + kGameFileOpenFunctionRva);

    if (!ycr::ApplyPatchSet(gExeBase, kFixedMenuPatches, kFixedMenuPatchCount)) goto fail;
    gFixedPatchesInstalled = true;

    if (!InstallCall6FailClosed(kPrevPageBaseReadRva, kPrevPageBaseReadBytes,
                                   reinterpret_cast<const void*>(&PrevPageBaseLoopHelper))) goto fail;
    gPrevPageHookInstalled = true;
    if (!InstallCall6FailClosed(kNextPageBaseReadRva, kNextPageBaseReadBytes,
                                   reinterpret_cast<const void*>(&NextPageBaseLoopHelper))) goto fail;
    gNextPageHookInstalled = true;

    if (!InstallCall5FailClosed(kNormalMenuSaveGateCallRva, kNormalMenuSaveGateCallBytes,
                                  reinterpret_cast<const void*>(&SafeSaveGateHook))) goto fail;
    gCoreHooksInstalled[0] = true;
    if (!InstallCall5FailClosed(kMapTickCallRva, kMapTickCallBytes,
                                  reinterpret_cast<const void*>(&SafeMapTickHook))) goto fail;
    gCoreHooksInstalled[1] = true;
    if (!InstallCall5FailClosed(kSaveWriterCallRva, kSaveWriterCallBytes,
                                  reinterpret_cast<const void*>(&SafeSaveWriterHook))) goto fail;
    gCoreHooksInstalled[2] = true;

    if (!InstallCall5FailClosed(kMenuSaveCallRva, kMenuSaveCallBytes,
                                  reinterpret_cast<const void*>(&ProtectedManualSaveHook))) goto fail;
    gManualHooksInstalled[0] = true;
    if (!InstallCall5FailClosed(kCommandSaveCallRva, kCommandSaveCallBytes,
                                  reinterpret_cast<const void*>(&ProtectedManualSaveHook))) goto fail;
    gManualHooksInstalled[1] = true;

    {
        DWORD hookAddress = static_cast<DWORD>(reinterpret_cast<SIZE_T>(&SaveActionUpdateHook));
        BYTE bytes[4];
        *reinterpret_cast<DWORD*>(bytes) = hookAddress;
        if (!ycr::WriteBytes(gExeBase + kSaveActionUpdateVtableRva, bytes, 4u)) {
            ycr::WriteBytes(gExeBase + kSaveActionUpdateVtableRva, kSaveActionUpdateVtableOriginalBytes, 4u);
            goto fail;
        }
        gVtableInstalled = true;
    }

    ycrlog::Line("[启动] SaveEnhance 完整 Hook 安装成功：100槽、循环分页、安全存档、Quick、Auto、保留槽禁写。");
    return true;

fail:
    ycrlog::Line("[启动失败] 安装中途出现写入失败；正在撤回本轮已经写入的 SaveEnhance 修改。");
    RollbackStartupInstall();
    return false;
}

CastleStringView SdkView(const char* text, CastleU32 length) {
    CastleStringView view{};
    view.data = text;
    view.length = length;
    return view;
}

const CastleHookApiV1* QueryHookApi(const CastleRuntimeApiV1* runtimeApi) {
    static const char interfaceId[] = CASTLE_HOOK_INTERFACE_ID;
    CastleInterfaceQueryV1 query{};
    CastleInterfaceResultV1 result{};
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = SdkView(interfaceId,
        static_cast<CastleU32>(sizeof(interfaceId) - 1u));
    query.requested_version = CASTLE_HOOK_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_HOOK_API_V1;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (!runtimeApi || runtimeApi->QueryInterface(&query, &result) != CASTLE_OK) {
        return nullptr;
    }
    return static_cast<const CastleHookApiV1*>(result.api_pointer);
}

CastleResult AddRuntimeCall(const CastleHookApiV1* hookApi,
    CastleTransactionHandle transaction, CastleModule gameModule,
    DWORD rva, CastleAddress originalTarget, const void* hook,
    CastleStringView signature, CastleClaimHandle* outClaim) {
    static const char label[] = "SaveEnhance CALL chain";
    CastleChainHookClaimV1 claim{};
    claim.magic = CASTLE_CHAIN_HOOK_MAGIC;
    claim.struct_size = CASTLE_SIZEOF_CHAIN_HOOK_V1;
    claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    claim.hook_kind = CASTLE_HOOK_REL32_CALL;
    claim.target = {gameModule, rva, 5u};
    claim.expected_original_target = originalTarget;
    claim.replacement_hook = static_cast<CastleAddress>(
        reinterpret_cast<SIZE_T>(hook));
    claim.signature_id = signature;
    claim.phase = CASTLE_HOOK_PHASE_NORMAL;
    claim.priority = CASTLE_HOOK_PRIORITY_DEFAULT;
    claim.label = SdkView(label, static_cast<CastleU32>(sizeof(label) - 1u));
    return hookApi->AddRelativeCallHook(transaction, &claim, outClaim);
}

CastleResult AddRuntimeExclusive(const CastleHookApiV1* hookApi,
    CastleTransactionHandle transaction, CastleModule gameModule,
    DWORD rva, const BYTE* expected, const BYTE* replacement,
    CastleU32 size, const char* label, CastleU32 labelLength,
    CastleClaimHandle* outClaim) {
    CastleExclusivePatchClaimV1 claim{};
    claim.magic = CASTLE_EXCLUSIVE_PATCH_MAGIC;
    claim.struct_size = CASTLE_SIZEOF_EXCLUSIVE_PATCH_V1;
    claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    claim.flags = CASTLE_PATCH_FLAG_CODE | CASTLE_PATCH_FLAG_KEEP_ON_PROCESS_EXIT;
    claim.target = {gameModule, rva, size};
    claim.expected_bytes = expected;
    claim.expected_size = size;
    claim.replacement_bytes = replacement;
    claim.replacement_size = size;
    claim.label = SdkView(label, labelLength);
    return hookApi->AddExclusivePatch(transaction, &claim, outClaim);
}

void BuildRuntimeCall6(BYTE* site, const void* target, BYTE output[6]) {
    output[0] = 0xE8u;
    *reinterpret_cast<DWORD*>(output + 1u) = static_cast<DWORD>(
        reinterpret_cast<SIZE_T>(target) - reinterpret_cast<SIZE_T>(site + 5u));
    output[5] = 0x90u;
}

bool GetRuntimeBinding(const CastleHookApiV1* hookApi, CastleClaimHandle claim,
                       void* volatile** outSlot) {
    CastleHookBindingV1 binding{};
    binding.magic = CASTLE_HOOK_BINDING_MAGIC;
    binding.struct_size = CASTLE_SIZEOF_HOOK_BINDING_V1;
    binding.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    if (hookApi->GetHookBinding(claim, &binding) != CASTLE_OK ||
        !binding.next_slot) return false;
    *outSlot = binding.next_slot;
    return true;
}

bool InstallAllHooksIntegrated(const CastleRuntimeApiV1* runtimeApi,
                               CastlePluginHandle pluginHandle) {
    static const char transactionLabel[] = "SaveEnhance complete hook transaction";
    static const char genericSignatureText[] =
        "org.castlereforge.signature.saveenhance-call.v1";
    static const char saveActionUpdateSignatureText[] =
        "org.castlereforge.signature.save-action-update.v1";
    static const char buttonHitSignatureText[] =
        "org.castlereforge.signature.button-hit-fast.v1";
    static const char buttonEventSignatureText[] =
        "org.castlereforge.signature.button-event-this.v1";
    static const char prevLabel[] = "SaveEnhance previous-page loop";
    static const char nextLabel[] = "SaveEnhance next-page loop";
    CastleRuntimeInfoV1 info{};
    const CastleHookApiV1* hookApi = QueryHookApi(runtimeApi);
    CastleTransactionHandle transaction = 0u;
    CastleClaimHandle vtableClaim = 0u;
    CastleClaimHandle hitClaim = 0u;
    CastleClaimHandle eventClaim = 0u;
    CastleClaimHandle temporaryClaim = 0u;
    CastleResult result;
    BYTE prevReplacement[6]{};
    BYTE nextReplacement[6]{};
    void* volatile* vtableNext = nullptr;
    void* volatile* hitNext = nullptr;
    void* volatile* eventNext = nullptr;
    CastleStringView genericSignature = SdkView(genericSignatureText,
        static_cast<CastleU32>(sizeof(genericSignatureText) - 1u));

    if (GetModuleHandleA("AnytimeSave.asi") != nullptr) {
        ycrlog::Line("[启动失败] 检测到旧 AnytimeSave.asi；禁止两个存档插件并存。");
        return false;
    }
    if (!hookApi || !runtimeApi || !EnsureOriginalSaveGateFunction() ||
        !PrecheckAllHookSites()) return false;
    info.magic = CASTLE_RUNTIME_INFO_MAGIC;
    info.struct_size = CASTLE_SIZEOF_RUNTIME_INFO_V1;
    info.info_version = CASTLE_RUNTIME_INFO_VERSION_1;
    if (runtimeApi->GetRuntimeInfo(&info) != CASTLE_OK) return false;

    gOriginalSaveGate = reinterpret_cast<OriginalSaveGateFunction>(gExeBase + kOriginalSaveGateFunctionRva);
    gOriginalMapTick = reinterpret_cast<OriginalMapTickFunction>(gExeBase + kOriginalMapTickFunctionRva);
    gOriginalSaveWriter = reinterpret_cast<OriginalSaveWriterFunction>(gExeBase + kOriginalSaveWriterFunctionRva);
    gOriginalSaveSlot = reinterpret_cast<OriginalSaveSlotFunction>(gExeBase + kOriginalSaveSlotFunctionRva);
    gOriginalLoadSlot = reinterpret_cast<OriginalLoadSlotFunction>(gExeBase + kOriginalLoadSlotFunctionRva);
    gOriginalSavePrepare = reinterpret_cast<OriginalNoArgFunction>(gExeBase + kOriginalSavePrepareFunctionRva);
    gOriginalPostLoad = reinterpret_cast<OriginalNoArgFunction>(gExeBase + kOriginalPostLoadFunctionRva);
    gGameFileCtor = reinterpret_cast<GameFileCtorFunction>(gExeBase + kGameFileCtorFunctionRva);
    gGameFileDtor = reinterpret_cast<GameFileDtorFunction>(gExeBase + kGameFileDtorFunctionRva);
    gGameFileOpen = reinterpret_cast<GameFileOpenFunction>(gExeBase + kGameFileOpenFunctionRva);

    result = hookApi->BeginTransaction(pluginHandle,
        SdkView(transactionLabel,
            static_cast<CastleU32>(sizeof(transactionLabel) - 1u)),
        0u, &transaction);
    if (result < 0) return false;

    for (SIZE_T index = 0u; index < kFixedMenuPatchCount; ++index) {
        CastleStatePatchClaimV1 claim{};
        claim.magic = CASTLE_STATE_PATCH_MAGIC;
        claim.struct_size = CASTLE_SIZEOF_STATE_PATCH_V1;
        claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
        claim.flags = CASTLE_PATCH_FLAG_CODE | CASTLE_PATCH_FLAG_KEEP_ON_PROCESS_EXIT;
        claim.target = {info.game_module, kFixedMenuPatches[index].rva,
            static_cast<CastleU32>(kFixedMenuPatches[index].size)};
        claim.original_bytes = kFixedMenuPatches[index].original;
        claim.original_size = static_cast<CastleU32>(kFixedMenuPatches[index].size);
        claim.enabled_bytes = kFixedMenuPatches[index].patched;
        claim.enabled_size = static_cast<CastleU32>(kFixedMenuPatches[index].size);
        claim.desired_state = CASTLE_PATCH_STATE_ENABLED;
        claim.label = SdkView(transactionLabel,
            static_cast<CastleU32>(sizeof(transactionLabel) - 1u));
        result = hookApi->AddStatePatch(transaction, &claim, &temporaryClaim);
        if (result < 0) goto fail_runtime_install;
    }

    BuildRuntimeCall6(gExeBase + kPrevPageBaseReadRva,
        reinterpret_cast<const void*>(&PrevPageBaseLoopHelper), prevReplacement);
    BuildRuntimeCall6(gExeBase + kNextPageBaseReadRva,
        reinterpret_cast<const void*>(&NextPageBaseLoopHelper), nextReplacement);
    result = AddRuntimeExclusive(hookApi, transaction, info.game_module,
        kPrevPageBaseReadRva, kPrevPageBaseReadBytes, prevReplacement, 6u,
        prevLabel, static_cast<CastleU32>(sizeof(prevLabel) - 1u), &temporaryClaim);
    if (result < 0) goto fail_runtime_install;
    result = AddRuntimeExclusive(hookApi, transaction, info.game_module,
        kNextPageBaseReadRva, kNextPageBaseReadBytes, nextReplacement, 6u,
        nextLabel, static_cast<CastleU32>(sizeof(nextLabel) - 1u), &temporaryClaim);
    if (result < 0) goto fail_runtime_install;

    result = AddRuntimeCall(hookApi, transaction, info.game_module,
        kNormalMenuSaveGateCallRva,
        static_cast<CastleAddress>(reinterpret_cast<SIZE_T>(gExeBase + kOriginalSaveGateFunctionRva)),
        reinterpret_cast<const void*>(&SafeSaveGateHook), genericSignature, &temporaryClaim);
    if (result < 0) goto fail_runtime_install;
    result = AddRuntimeCall(hookApi, transaction, info.game_module,
        kMapTickCallRva,
        static_cast<CastleAddress>(reinterpret_cast<SIZE_T>(gExeBase + kOriginalMapTickFunctionRva)),
        reinterpret_cast<const void*>(&SafeMapTickHook), genericSignature, &temporaryClaim);
    if (result < 0) goto fail_runtime_install;
    result = AddRuntimeCall(hookApi, transaction, info.game_module,
        kSaveWriterCallRva,
        static_cast<CastleAddress>(reinterpret_cast<SIZE_T>(gExeBase + kOriginalSaveWriterFunctionRva)),
        reinterpret_cast<const void*>(&SafeSaveWriterHook), genericSignature, &temporaryClaim);
    if (result < 0) goto fail_runtime_install;
    result = AddRuntimeCall(hookApi, transaction, info.game_module,
        kMenuSaveCallRva,
        static_cast<CastleAddress>(reinterpret_cast<SIZE_T>(gExeBase + kOriginalSaveSlotFunctionRva)),
        reinterpret_cast<const void*>(&ProtectedManualSaveHook), genericSignature, &temporaryClaim);
    if (result < 0) goto fail_runtime_install;
    result = AddRuntimeCall(hookApi, transaction, info.game_module,
        kCommandSaveCallRva,
        static_cast<CastleAddress>(reinterpret_cast<SIZE_T>(gExeBase + kOriginalSaveSlotFunctionRva)),
        reinterpret_cast<const void*>(&ProtectedManualSaveHook), genericSignature, &temporaryClaim);
    if (result < 0) goto fail_runtime_install;

    {
        CastleChainHookClaimV1 claim{};
        claim.magic = CASTLE_CHAIN_HOOK_MAGIC;
        claim.struct_size = CASTLE_SIZEOF_CHAIN_HOOK_V1;
        claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
        claim.hook_kind = CASTLE_HOOK_VTABLE_POINTER;
        claim.target = {info.game_module, kSaveActionUpdateVtableRva, 4u};
        claim.expected_original_target = kSaveActionUpdateOriginalVa;
        claim.replacement_hook = static_cast<CastleAddress>(
            reinterpret_cast<SIZE_T>(&SaveActionUpdateHook));
        claim.signature_id = SdkView(saveActionUpdateSignatureText,
            static_cast<CastleU32>(sizeof(saveActionUpdateSignatureText) - 1u));
        claim.phase = CASTLE_HOOK_PHASE_NORMAL;
        claim.priority = CASTLE_HOOK_PRIORITY_DEFAULT;
        claim.label = claim.signature_id;
        result = hookApi->AddPointerHook(transaction, &claim, &vtableClaim);
        if (result < 0) goto fail_runtime_install;
    }
    result = AddRuntimeCall(hookApi, transaction, info.game_module,
        kSaveActionHitCallRva, kOriginalButtonHitVa,
        reinterpret_cast<const void*>(&ReservedSaveActionHitHook),
        SdkView(buttonHitSignatureText,
            static_cast<CastleU32>(sizeof(buttonHitSignatureText) - 1u)), &hitClaim);
    if (result < 0) goto fail_runtime_install;
    result = AddRuntimeCall(hookApi, transaction, info.game_module,
        kSaveActionEventCallRva, kOriginalButtonEventVa,
        reinterpret_cast<const void*>(&ReservedSaveActionEventHook),
        SdkView(buttonEventSignatureText,
            static_cast<CastleU32>(sizeof(buttonEventSignatureText) - 1u)), &eventClaim);
    if (result < 0) goto fail_runtime_install;

    result = hookApi->PreflightTransaction(transaction);
    if (result >= 0) result = hookApi->CommitTransaction(transaction);
    if (result < 0) return false;
    if (!GetRuntimeBinding(hookApi, vtableClaim, &vtableNext) ||
        !GetRuntimeBinding(hookApi, hitClaim, &hitNext) ||
        !GetRuntimeBinding(hookApi, eventClaim, &eventNext)) return false;
    gOriginalSaveActionUpdate = reinterpret_cast<OriginalSaveActionUpdateFunction>(*vtableNext);
    gSaveActionHitNext = reinterpret_cast<SaveActionHitFunction>(*hitNext);
    gSaveActionEventNext = reinterpret_cast<SaveActionEventFunction>(*eventNext);
    gSaveActionPadHooksInstalled = true;
    gFixedPatchesInstalled = true;
    gPrevPageHookInstalled = true;
    gNextPageHookInstalled = true;
    gCoreHooksInstalled[0] = gCoreHooksInstalled[1] = gCoreHooksInstalled[2] = true;
    gManualHooksInstalled[0] = gManualHooksInstalled[1] = true;
    gVtableInstalled = true;
    ycrlog::Line("[RuntimeSDK] SaveEnhance 完整 Hook 事务已提交；SaveAction next 指向 Controller POST 链。");
    return true;

fail_runtime_install:
    hookApi->AbortTransaction(transaction);
    return false;
}

} // namespace

// ============================================================================
// 十八、真正的 Hook wrappers
// ============================================================================
extern "C" DWORD __fastcall SafeSaveGateHook(void* runtimeEntry) {
    if (gOriginalSaveGate == nullptr || runtimeEntry == nullptr) {
        ClearArm();
        return 0u;
    }
    const DWORD result = gOriginalSaveGate(runtimeEntry);
    if (result != 0u) {
        ClearArm();
        return result;
    }
    return ArmExtendedSaveFromCurrentMap() ? 1u : 0u;
}

extern "C" void __fastcall SafeMapTickHook(void* sceneContainer, void* unusedEdx) {
    if (gOriginalMapTick != nullptr) {
        // 必须先跑原版 Tick，后面所有自由行动/锚点判断才是在“这一帧更新完成后”的状态。
        gOriginalMapTick(sceneContainer, unusedEdx);
    }

    MapState map;
    if (!ReadMapState(&map) || map.sceneContainer != sceneContainer) return;

    UpdateSceneIdentityAndAutoPending(map);
    ObserveVanillaAnchor(map);
    PollQuickInputs();
    MaybeRunAutoSave();
}

extern "C" BOOL __fastcall SafeSaveWriterHook(
    void* runtimeManager,
    void* unusedEdx,
    const char* path) {
    if (gOriginalSaveWriter == nullptr || path == nullptr) {
        ClearArm();
        return FALSE;
    }
    if (!gArm.valid) {
        return gOriginalSaveWriter(runtimeManager, unusedEdx, path);
    }

    MapState current;
    if (!ArmStillMatchesCurrentMap(&current)) {
        ycrlog::Line("[保存中止] save gate 后地图身份已变化；拒绝把旧锚点写进新地图。");
        ClearArm();
        return FALSE;
    }
    BYTE* world = nullptr;
    if (!GetWriterWorldBuffer(runtimeManager, &world)) {
        ycrlog::Line("[保存中止] 无法取得 Writer World buffer；不写未知状态 TSF。");
        ClearArm();
        return FALSE;
    }

    BYTE exactHeader[kWorldHeaderBytes];
    CopyBytes(exactHeader, world, kWorldHeaderBytes);
    CopyBytes(world, gArm.fallback.worldHeader, kWorldHeaderBytes);
    const BOOL saved = gOriginalSaveWriter(runtimeManager, unusedEdx, path);
    CopyBytes(world, exactHeader, kWorldHeaderBytes);
    ClearArm();
    return saved;
}

extern "C" BOOL __fastcall ProtectedManualSaveHook(
    void* runtimeManager,
    void* unusedEdx,
    DWORD slot) {
    if (IsReservedSlot(slot)) {
        // UI disabled 是第一层；这里是第二层。即使某个未知输入路径把 action index 强行送到 save，
        // 0 和 91~99 仍然不会被普通/隐藏手动 SaveSlot 调用覆盖。
        ClearArm();
        ycrlog::Text("[保留槽保护] 已阻止普通手动保存覆盖槽 ");
        ycrlog::Unsigned(slot);
        ycrlog::Line("。");
        return FALSE;
    }
    if (gOriginalSaveSlot == nullptr) {
        ClearArm();
        return FALSE;
    }
    return gOriginalSaveSlot(runtimeManager, unusedEdx, slot);
}

extern "C" void __fastcall SaveActionUpdateHook(void* action, void* unusedEdx) {
    if (gOriginalSaveActionUpdate == nullptr) return;

    // 标题/天书里 Map Tick 不一定运行，所以这里也低频解析公开 API。等 PadSupport 自己完成
    // 初始化和原有 Hook 后，SaveEnhance 才把自己的 wrapper 链在当前目标外层。
    TryResolvePadApi();
    if (gPadApi != nullptr) TryInstallReservedSaveActionPadHooks();

    // 一定要在原版 0x4262C0 之前写 disabled。原版键鼠直接尊重它；若公开手柄 API 可用，
    // SaveEnhance 自己的两项焦点 wrapper 会在同一次 Update 中处理读档/取消。
    PrepareReservedSaveButtonState(action);
    gOriginalSaveActionUpdate(action, unusedEdx);
    EndSaveButtonOverride();
}

// ============================================================================
// 十九、循环分页 6-byte 替换 helper
// ============================================================================
// 原版上一页：先 mov eax,[esi+598]，若 eax<=0 就不减 4。
// 我们只在 eax==0 时暂时返回 100；后面的原版 cmp 通过，再 add -4 得到真实 96。
extern "C" __declspec(naked) void PrevPageBaseLoopHelper() {
    __asm {
        mov eax, dword ptr [esi + 598h]
        test eax, eax
        jne prev_done
        mov eax, 100
    prev_done:
        ret
    }
}

// 原版下一页：后面会检查 eax+8 <= 100，再 add +4。
// 在真实最后页 96 时，我们暂时返回 -4；原版后续 -4+8<=100 成立，再 +4 得到真实 0。
extern "C" __declspec(naked) void NextPageBaseLoopHelper() {
    __asm {
        mov eax, dword ptr [esi + 598h]
        cmp eax, 96
        jne next_done
        mov eax, -4
    next_done:
        ret
    }
}

// ============================================================================
// 二十、Castle Mod Loader 正式生命周期 + DLL 入口
// ============================================================================
//
// 这里是 test3 最关键的修正。
//
// test1/test2 的错误：
// - 在 DllMain(DLL_PROCESS_ATTACH) 里面直接读取 INI、GetModuleHandle、VirtualQuery、VirtualProtect、
//   写 RPG.exe 机器码并安装 Hook；
// - 但 Windows 正在执行 DLL 装载器的 Loader Lock 生命周期，Castle Mod Loader 自己也还没来得及
//   给这个 ASI 补 Locale/Overrides IAT；
// - 用户 test2 实机日志只写到“[配置] ...”就停止，且所有保存增强完全无效，说明正式安装路径根本
//   没走到“Hook 安装成功/失败”结论。
//
// Castle Mod Loader 当前的正式时序已经在它自己的源码中明确：
// 1. LoadLibraryExW(ASI) -> 先只执行 DllMain；
// 2. LoadLibraryExW 返回；
// 3. Loader 给 ASI 执行 OverrideLoader_PatchModule / LocaleLayer_PatchModule；
// 4. GetProcAddress(module, "InitializeASI")；
// 5. 若导出存在，再在 Loader Lock 之外调用 InitializeASI()。
//
// 因此 SaveEnhance 从 test3 开始严格遵守这个接口：
// - DllMain 只保存自身 HMODULE，并关闭无用的线程 attach/detach 通知；
// - 所有文件 I/O、INI、兼容模块查询、内存检查、VirtualProtect 和 Hook 写入都放进 InitializeASI；
// - 这样也保证 SaveEnhance 看见的是 Loader 已经准备好的最终 Locale/Overrides 环境。
static bool BuildIniPathRuntime(const CastleRuntimeApiV1* runtimeApi,
                                CastlePluginHandle pluginHandle) {
    static const char interfaceId[] = CASTLE_PATH_INTERFACE_ID;
    static const wchar_t relativeName[] = L"Castle_SaveEnhance.ini";
    CastleInterfaceQueryV1 query{};
    CastleInterfaceResultV1 result{};
    CastleWideStringView relative{};
    CastleU32 outputLength = 0u;
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = SdkView(interfaceId,
        static_cast<CastleU32>(sizeof(interfaceId) - 1u));
    query.requested_version = CASTLE_PATH_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_PATH_API_V1;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (!runtimeApi || runtimeApi->QueryInterface(&query, &result) != CASTLE_OK) {
        return false;
    }
    const auto* pathApi = static_cast<const CastlePathApiV1*>(result.api_pointer);
    relative.data = reinterpret_cast<const CastleU16*>(relativeName);
    relative.length = static_cast<CastleU32>(WLen(relativeName));
    return pathApi && pathApi->BuildPluginRelativePathWide(pluginHandle,
        relative, reinterpret_cast<CastleU16*>(gIniPath), 520u,
        &outputLength) == CASTLE_OK;
}

static CastleResult InitializeSaveEnhance(const CastleRuntimeApiV1* runtimeApi,
                                          CastlePluginHandle pluginHandle,
                                          bool integrated) {
    ycrlog::Open(gSelfModule, L"Castle_SaveEnhance.log");
    ycrlog::Line("《幽城幻剑录》Castle_SaveEnhance v0.2.0 RuntimeSDK 启动。");
    ycrlog::Line("By Luminous with ChatGPT");
    ycrlog::Line(integrated
        ? "[装载] Integrated：Runtime Path + Hook 事务。"
        : "[装载] Standalone：插件本地 Path + fail-closed 补丁器。");
    ycrlog::Line("[槽位] 0=Quick，1~90=Manual，91~99=Rolling Auto；普通菜单保留槽只读。");
    ycrlog::Line("[快捷] F5=Quick Save；F9 连按确认=Quick Load；Controller API 可选联动。");
    ycrlog::Line("[声音] 只使用可选外置 WAV；空/非法/缺失文件静默，不影响存档结果。");

    // 到这个时刻 LoadLibraryExW 已经返回，所以 GetModuleHandle/VirtualQuery/VirtualProtect 等正式
    // 初始化工作不再发生在 DllMain Loader Lock 中。
    gExeBase = ycr::GetExeBase();
    const bool pathReady = integrated ? BuildIniPathRuntime(runtimeApi, pluginHandle) :
                                        BuildIniPath(gIniPath, 520u);
    if (gExeBase == nullptr || !pathReady) {
        ycrlog::Line("[启动失败] 无法取得 RPG.exe 基址或 Castle_SaveEnhance.ini 路径。");
        ycrlog::Line("[状态] SaveEnhance 未完整安装；本轮不修改任何存档逻辑。");
        return CASTLE_ERROR_RUNTIME_FAULT;
    }

    LoadConfig();
    LoadAutoRingState();
    ycrlog::Text("[配置] QuickLoadPresses=");
    ycrlog::Unsigned(gConfig.quickLoadPresses);
    ycrlog::Text(" WindowMs=");
    ycrlog::Unsigned(gConfig.quickLoadWindowMs);
    ycrlog::Text(" AutoIntervalMin=");
    ycrlog::Unsigned(gConfig.intervalMinutes);
    ycrlog::Text(" SoundVolume=");
    ycrlog::Unsigned(gConfig.soundVolume);
    ycrlog::Line("。");
    ycrlog::Text("[自动槽] 持久化环形候选=");
    ycrlog::Unsigned(gNextAutoSlot);
    ycrlog::Line("；若91~99存在空槽，仍优先填最低空槽。");

    // 增加两个阶段日志。即使以后某台机器仍在安装阶段异常停止，也能一眼知道停在“进入预检查”
    // 之前还是“已经开始机器码安装”之后，不再只剩一行配置日志。
    ycrlog::Line("[启动] 开始目标机器码预检查与兼容性检查。");
    const bool installed = integrated ?
        InstallAllHooksIntegrated(runtimeApi, pluginHandle) : InstallAllHooks();
    if (!installed) {
        ycrlog::Line("[状态] SaveEnhance 未完整安装；为保护存档，本轮不提供增强功能。");
        return CASTLE_ERROR_EXPECTED_BYTES;
    }

    ycrlog::Line("[状态] SaveEnhance 已完整安装，可以开始实机功能测试。");
    gStandaloneMode = !integrated;
    return CASTLE_OK;
}

static CastleResult CASTLE_RUNTIME_CALL SaveEnhance_Integrated(
    const CastleRuntimeApiV1* runtimeApi, CastlePluginHandle pluginHandle,
    void* userContext) {
    (void)userContext;
    return InitializeSaveEnhance(runtimeApi, pluginHandle, true);
}

static CastleResult CASTLE_RUNTIME_CALL SaveEnhance_Standalone(void* userContext) {
    (void)userContext;
    return InitializeSaveEnhance(nullptr, 0u, false);
}

static void CASTLE_RUNTIME_CALL SaveEnhance_RuntimeFault(CastleResult failure,
                                                         void* userContext) {
    (void)userContext;
    ycrlog::Open(gSelfModule, L"Castle_SaveEnhance.log");
    ycrlog::Line("[失败] Castle_Runtime.dll 存在但不可用；SaveEnhance 未回退到私有 Hook。");
    ycrlog::Text("[失败] Runtime code=");
    ycrlog::Unsigned(static_cast<DWORD>(-failure));
    ycrlog::Line("。");
}

static void CASTLE_RUNTIME_CALL SaveEnhance_ProcessExit(void* userContext) {
    (void)userContext;
    ResetReservedPadState();
    EndSaveButtonOverride();
    ycrlog::Line("[退出] Castle_SaveEnhance 随进程结束。");
    ycrlog::Close();
}

static const char gSdkPluginId[] = "org.castlereforge.saveenhance";
static const char gSdkDisplayName[] = "Castle SaveEnhance";
static const char gSdkVersion[] = "0.2.0";
static const char gSdkBuild[] = "runtimesdk-v1";
static const CastlePluginDescriptorV1 gSdkDescriptor = {
    CASTLE_PLUGIN_DESC_MAGIC, CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_DESCRIPTOR_V1,
    CASTLE_PLUGIN_FLAG_SUPPORTS_STANDALONE | CASTLE_PLUGIN_FLAG_REQUESTS_HOOKS |
        CASTLE_PLUGIN_FLAG_OFFICIAL_MODULE,
    0u,
    {gSdkPluginId, static_cast<CastleU32>(sizeof(gSdkPluginId) - 1u)},
    {gSdkDisplayName, static_cast<CastleU32>(sizeof(gSdkDisplayName) - 1u)},
    {gSdkVersion, static_cast<CastleU32>(sizeof(gSdkVersion) - 1u)},
    {gSdkBuild, static_cast<CastleU32>(sizeof(gSdkBuild) - 1u)}
};
static const CastleRuntimeClientConfigV1 gSdkClientConfig = {
    CASTLE_CLIENT_CONFIG_MAGIC, CASTLE_SIZEOF_CLIENT_CONFIG_V1,
    CASTLE_CLIENT_CONFIG_VERSION_1, 0u,
    SaveEnhance_Integrated, SaveEnhance_Standalone, SaveEnhance_RuntimeFault,
    SaveEnhance_ProcessExit, nullptr
};
static CastlePluginExportV1 gSdkExport = {
    CASTLE_PLUGIN_QUERY_MAGIC, CASTLE_SIZEOF_PLUGIN_EXPORT_V1,
    CASTLE_PLUGIN_EXPORT_VERSION_1, 0u,
    &gSdkDescriptor, &gSdkClientConfig, 0u, nullptr
};

extern "C" const CastlePluginExportV1* CASTLE_RUNTIME_CALL CastlePlugin_Query(
    CastleU32 requestedVersion) {
    return requestedVersion == CASTLE_PLUGIN_EXPORT_VERSION_1 ? &gSdkExport : nullptr;
}

extern "C" void __cdecl InitializeASI() {
    CastleRuntimeClient_RunNow();
}

extern "C" BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {

    if (reason == DLL_PROCESS_ATTACH) {
        // DllMain 必须尽可能短。这里只记住自己的模块句柄，供稍后的 InitializeASI 构造日志/INI 路径。
        gSelfModule = module;
        DisableThreadLibraryCalls(module);
        CastleRuntimeClient_OnProcessAttach(
            static_cast<CastleModule>(reinterpret_cast<SIZE_T>(module)), &gSdkExport);
        return TRUE;
    }

    if (reason == DLL_PROCESS_DETACH) {
        // 退出时只恢复本插件自己链入的两处 CALL、临时 disabled 和私有焦点，再关闭日志。
        // 不修改 PadSupport 代码，也不重装任何业务 Hook。
        if (reserved == nullptr && gStandaloneMode) RestoreReservedSaveActionPadHooks();
        CastleRuntimeClient_OnProcessDetach(reserved);
    }
    return TRUE;
}
