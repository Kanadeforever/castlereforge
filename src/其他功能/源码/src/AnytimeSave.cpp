#include "PatchUtil.h"
#include "PluginLog.h"

// ============================================================================
// 极小的编译器复制 / 清零兜底
// ----------------------------------------------------------------------------
// 正式 ASI 使用 /NODEFAULTLIB，不链接 C/C++ 运行库。即使源码自己没有主动调用
// memcpy/memset，优化器也可能把“复制一个结构”“把一个结构清零”自动改写成这两个函数。
// 因此这里提供最小实现，保证最终插件仍然只依赖 Windows 自带的 kernel32.dll。
//
// volatile 的作用可以理解成“每个字节都必须真的读、真的写，编译器不要自作主张重新
// 合并成 memcpy/memset”。否则这个函数自己又可能被优化成对自己的调用，形成递归。
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
// AnytimeSave.asi  v0.3.1a
// ----------------------------------------------------------------------------
// 当前正式目标已经从“精确恢复到随时存档瞬间”收缩成“安全保住进度”。
//
// 之前 dev1~dev3 尝试使用 .YCS / .TSA 旁挂文件补充 selected Scene Mode 等原版 TSF
// 没有保存的运行时状态。用户实机已经证明：
//   1. 磐沙堡楼梯危险点，旧无条件随时存档会形成坏档；
//   2. dev1 起的 vanilla fallback 能把同一危险点保存成可读档，读档回到场景入口；
//   3. dev3 的 TSA 仍无法生成，日志 Win32 错误码 2（ERROR_FILE_NOT_FOUND），说明插件
//      构造的 TSF 绝对路径和游戏自己的文件系统实际写盘位置并不一致。
//
// 与其继续增加路径追踪、旁挂 I/O、Load Hook 和精确恢复复杂度，v0.3.1a 正式把 TSA
// 整条实验链从正式插件删除，只保留已经有实机正向结果的“原版安全回退”闭环。
//
// 最终规则只有三条：
//   A. 原版自己允许存档 -> 完全交给 RPG.exe，不修改任何 World 数据；
//   B. 原版拒绝存档，但当前地图已经建立入口级 vanilla fallback -> 允许保存；
//   C. 原版拒绝存档，而且还没有 fallback -> 暂时不开放，宁可不能存，也不写未知坏档。
//
// 对 B 类扩展存档，插件只在调用原版 Writer 的那一小段时间内，把“准备写入 TSF 的
// 200-byte World 场景恢复头”替换成这张地图早期 default Scene Mode 下真实跑过的稳定
// 快照。原版 Writer 返回后立刻恢复内存，所以屏幕上的角色不会被移动。
//
// 重要兼容性：
//   - 不新增 .TSA / .YCS / 其它旁挂文件；
//   - 不改变 TSF 文件格式、长度、字段布局；
//   - 没有插件的原版 RPG.exe 仍然只看到一个普通 TSF；
//   - 扩展存档读回时不保证回到“按保存键那一格”，而是回到场景入口/早期安全点；
//   - 角色、队伍、道具、GameVar 等原版其它存档块仍由原版 Writer 保存当前进度。
//
// 这不是“任意一帧精确快照”，而是“安全扩展存档 / 进度保险”。
// ============================================================================

namespace {

// ---------------------------------------------------------------------------
// 一、当前参考 RPG.exe 的 Hook 地址和原始机器码
// ---------------------------------------------------------------------------
// 所有 RVA 都以 0x00400000 为 EXE image base。
// 每个 Hook 写入前必须先验证 5 字节原始 CALL；不认识的 EXE 一律 fail-closed。

const DWORD kNormalMenuSaveGateCallRva = 0x0000CCC2u;
const BYTE kNormalMenuSaveGateCallBytes[5] = {0xE8, 0xF9, 0xD3, 0xFF, 0xFF};

const DWORD kMapTickCallRva = 0x0000B078u;
const BYTE kMapTickCallBytes[5] = {0xE8, 0x63, 0xFD, 0xFF, 0xFF};

const DWORD kSaveWriterCallRva = 0x0003B34Cu;
const BYTE kSaveWriterCallBytes[5] = {0xE8, 0x0F, 0x00, 0x00, 0x00};

const DWORD kOriginalSaveGateFunctionRva = 0x0000A0C0u;
const BYTE kOriginalSaveGateFunctionBytes[9] = {
    0x8B, 0x01, 0x8B, 0x80, 0x80, 0x03, 0x00, 0x00, 0xC3
};

// 历史 3in1/5in1 的旧“随时存档”会把 0x40A0C0 改成固定返回 1。
// 新插件如果遇到这组已知字节，会先在内存恢复原版访问器，再接管正常菜单 CALL。
// 这样旧补丁不会在我们的安全回退逻辑之前就把所有场景无条件放行。
const BYTE kOldUnsafeSaveGateFunctionBytes[9] = {
    0x8B, 0x01, 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90, 0xC3
};

const DWORD kOriginalMapTickFunctionRva = 0x0000ADE0u;
const DWORD kOriginalSaveWriterFunctionRva = 0x0003B360u;

// ---------------------------------------------------------------------------
// 二、原版运行时结构中已经闭合的字段
// ---------------------------------------------------------------------------

// DataCenter* 全局。DataCenter + 0x280 -> 当前 Map scene container。
const DWORD kDataCenterPointerRva = 0x0049F804u;       // absolute 0x0089F804

// 角色 runtime 数组和当前受控角色索引。
const DWORD kActorRuntimeArrayPointerRva = 0x0049F7F0u; // absolute 0x0089F7F0
const DWORD kControlledActorIndexRva = 0x00068BF0u;     // absolute 0x00468BF0

// 0x44B060 创建的 World/GameVariable manager 指针。
// 这份指针既被 GameVar API 使用，也被原版 Save/Load World 逻辑直接使用。
const DWORD kWorldGlobalPointerRva = 0x00578508u;       // absolute 0x00978508

const SIZE_T kDataCenterSceneContainerOffset = 0x280u;
const SIZE_T kMapRecordCountOffset = 0x00u;
const SIZE_T kMapSelectedIndexOffset = 0x04u;
const SIZE_T kMapRawSourceOffset = 0x0Cu;
const SIZE_T kMapSciRecordSize = 0x473u;
const DWORD kMaxReasonableMapRecords = 128u;

const SIZE_T kActorStride = 0x74u;
const SIZE_T kActorXOffset = 0x10u;
const SIZE_T kActorYOffset = 0x14u;
const SIZE_T kActorStateByteOffset = 0x1Au;

// 传给 0x43B360 的 runtime manager：+0x13C 是 World 长度，+0x140 是 World 指针。
const SIZE_T kRuntimeManagerWorldLengthOffset = 0x13Cu;
const SIZE_T kRuntimeManagerWorldPointerOffset = 0x140u;
const DWORD kExpectedWorldLength = 0x27D8u;

// World 0x0000..0x00C7 共 200 bytes，是原版场景恢复头；0x00C8 后才开始 5000 个
// int16 GameVar。因此安全回退只覆盖前 200 bytes，不把剧情变量倒退到入口时刻。
const SIZE_T kWorldHeaderBytes = 0x00C8u;
const SIZE_T kWorldXOffset = 0x32u;
const SIZE_T kWorldYOffset = 0x36u;
const SIZE_T kWorldStateOffset = 0x3Au;
const SIZE_T kWorldActorIndexOffset = 0x3Eu;

// ---------------------------------------------------------------------------
// 三、原版函数调用约定
// ---------------------------------------------------------------------------

typedef DWORD (__fastcall *OriginalSaveGateFunction)(void* runtimeEntry);
typedef void (__fastcall *OriginalMapTickFunction)(void* sceneContainer, void* unusedEdx);
typedef BOOL (__fastcall *OriginalSaveWriterFunction)(
    void* runtimeManager,
    void* unusedEdx,
    const char* path);

BYTE* gExeBase = nullptr;
OriginalSaveGateFunction gOriginalSaveGate = nullptr;
OriginalMapTickFunction gOriginalMapTick = nullptr;
OriginalSaveWriterFunction gOriginalSaveWriter = nullptr;

// ---------------------------------------------------------------------------
// 四、最小内存安全读取工具
// ---------------------------------------------------------------------------

bool IsReadableRange(const void* address, SIZE_T size) {
    if (address == nullptr || size == 0u) {
        return false;
    }

    const SIZE_T begin = reinterpret_cast<SIZE_T>(address);
    const SIZE_T end = begin + size;
    if (end < begin) {
        // 指针加法溢出，说明这个范围本身不可信。
        return false;
    }

    SIZE_T cursor = begin;
    while (cursor < end) {
        MEMORY_BASIC_INFORMATION_MINI info;
        const SIZE_T queried = VirtualQuery(
            reinterpret_cast<LPCVOID>(cursor), &info, sizeof(info));
        if (queried == 0u ||
            info.State != MEM_COMMIT ||
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

bool ReadDword(const BYTE* address, DWORD* value) {
    if (value == nullptr || !IsReadableRange(address, sizeof(DWORD))) {
        return false;
    }
    *value = *reinterpret_cast<const volatile DWORD*>(address);
    return true;
}

bool ReadExeDword(DWORD rva, DWORD* value) {
    return gExeBase != nullptr && ReadDword(gExeBase + rva, value);
}

void CopyBytes(BYTE* destination, const BYTE* source, SIZE_T size) {
    // 单独保留这个函数是为了让下面“备份 200 bytes / 临时覆盖 / 恢复”读起来更直观。
    // volatile 保证编译器不会把循环重新合并成 CRT memcpy。
    volatile BYTE* out = reinterpret_cast<volatile BYTE*>(destination);
    const volatile BYTE* in = reinterpret_cast<const volatile BYTE*>(source);
    for (SIZE_T i = 0u; i < size; ++i) {
        out[i] = in[i];
    }
}

// ---------------------------------------------------------------------------
// 五、读取当前 Map 身份和 default / selected Scene Mode
// ---------------------------------------------------------------------------

struct MapState {
    BYTE* sceneContainer;
    BYTE* rawSource;
    DWORD recordCount;
    DWORD selectedIndex;
    DWORD defaultIndex;
    DWORD sourceFingerprint;
};

// 地图对象的内存地址有可能在切图后被分配器复用，所以不能只比较 sceneContainer /
// rawSource 指针。这里把当前 Map SCI 的全部 record 字节都做一次轻量 FNV-1a 指纹。
//
// 为什么愿意扫描全部 record：典型地图只有很少几条记录，即使按保护上限 128 条计算，
// 总量也只有约 145KB；相比错误复用上一张地图的安全锚点，这点 CPU 成本非常值得。
// 这个指纹只在内存里做“地图身份绑定”，不会写入存档，也不是加密用途。
DWORD BuildSourceFingerprint(const BYTE* rawSource, DWORD recordCount) {
    DWORD value = 2166136261u;
    const SIZE_T total = static_cast<SIZE_T>(recordCount) * kMapSciRecordSize;
    if (!IsReadableRange(rawSource, total)) {
        return 0u;
    }

    for (SIZE_T i = 0u; i < total; ++i) {
        value ^= static_cast<DWORD>(rawSource[i]);
        value *= 16777619u;
    }
    return value;
}

bool ReadMapState(MapState* state) {
    if (state == nullptr) {
        return false;
    }

    DWORD dataCenterAddress = 0u;
    if (!ReadExeDword(kDataCenterPointerRva, &dataCenterAddress) || dataCenterAddress == 0u) {
        return false;
    }

    BYTE* dataCenter = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(dataCenterAddress));
    DWORD sceneContainerAddress = 0u;
    if (!ReadDword(dataCenter + kDataCenterSceneContainerOffset, &sceneContainerAddress) ||
        sceneContainerAddress == 0u) {
        return false;
    }

    BYTE* sceneContainer = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(sceneContainerAddress));
    if (!IsReadableRange(sceneContainer, 0x10u)) {
        return false;
    }

    DWORD recordCount = 0u;
    DWORD selectedIndex = 0u;
    DWORD rawSourceAddress = 0u;
    if (!ReadDword(sceneContainer + kMapRecordCountOffset, &recordCount) ||
        !ReadDword(sceneContainer + kMapSelectedIndexOffset, &selectedIndex) ||
        !ReadDword(sceneContainer + kMapRawSourceOffset, &rawSourceAddress)) {
        return false;
    }

    if (recordCount == 0u || recordCount > kMaxReasonableMapRecords ||
        selectedIndex >= recordCount || rawSourceAddress == 0u) {
        return false;
    }

    BYTE* rawSource = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(rawSourceAddress));
    const SIZE_T sourceBytes = static_cast<SIZE_T>(recordCount) * kMapSciRecordSize;
    if (!IsReadableRange(rawSource, sourceBytes)) {
        return false;
    }

    // 原版 0x40AC00 在初始化 scene_container+4 时先清零，然后逐条扫描 Map SCI。
    // 每遇到 record byte0 != 0 就把当前索引写入 +4，因此最后一个非零 record 是默认值。
    DWORD defaultIndex = 0u;
    for (DWORD i = 0u; i < recordCount; ++i) {
        if (rawSource[static_cast<SIZE_T>(i) * kMapSciRecordSize] != 0u) {
            defaultIndex = i;
        }
    }

    state->sceneContainer = sceneContainer;
    state->rawSource = rawSource;
    state->recordCount = recordCount;
    state->selectedIndex = selectedIndex;
    state->defaultIndex = defaultIndex;
    state->sourceFingerprint = BuildSourceFingerprint(rawSource, recordCount);
    return state->sourceFingerprint != 0u;
}

// ---------------------------------------------------------------------------
// 六、读取当前受控角色的“原版 TSF 已确认字段”
// ---------------------------------------------------------------------------

struct ActorState {
    DWORD actorIndex;
    DWORD x;
    DWORD y;
    DWORD stateByte;
};

bool ReadControlledActorState(ActorState* state) {
    if (state == nullptr) {
        return false;
    }

    DWORD actorIndex = 0u;
    DWORD actorArrayAddress = 0u;
    if (!ReadExeDword(kControlledActorIndexRva, &actorIndex) ||
        !ReadExeDword(kActorRuntimeArrayPointerRva, &actorArrayAddress) ||
        actorArrayAddress == 0u || actorIndex >= 256u) {
        return false;
    }

    BYTE* actorArray = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(actorArrayAddress));
    BYTE* actor = actorArray + static_cast<SIZE_T>(actorIndex) * kActorStride;
    if (!IsReadableRange(actor, kActorStateByteOffset + 1u)) {
        return false;
    }

    state->actorIndex = actorIndex;
    state->x = *reinterpret_cast<const volatile DWORD*>(actor + kActorXOffset);
    state->y = *reinterpret_cast<const volatile DWORD*>(actor + kActorYOffset);
    state->stateByte = static_cast<DWORD>(
        *reinterpret_cast<const volatile BYTE*>(actor + kActorStateByteOffset));
    return true;
}

// ---------------------------------------------------------------------------
// 七、建立“入口级 vanilla fallback”
// ---------------------------------------------------------------------------
// 目的不是尽量接近玩家当前坐标，而是给没有插件的原版 RPG.exe 留一个尽可能稳定的
// 读档落点。因此：
//   1. 只在 selected == default Scene Mode 的完整 Map Tick 后采样；
//   2. 连续稳定 8 Tick 才确认地图初始化已经基本结束；
//   3. 固化最早那个样本；
//   4. 同一地图生命周期内不再跟着玩家滚动。
//
// 用户已经在磐沙堡楼梯危险点实机确认：采用这种 fallback 写出的 TSF，读档会回到
// 场景入口而不是楼梯保存坐标，并且不再形成原来的坏档。

const DWORD kAnchorMinimumSamples = 8u;

struct VanillaAnchor {
    bool valid;
    ActorState actor;
    BYTE worldHeader[kWorldHeaderBytes];
};

struct AnchorTracker {
    bool identityValid;
    BYTE* sceneContainer;
    BYTE* rawSource;
    DWORD recordCount;
    DWORD defaultIndex;
    DWORD sourceFingerprint;
    DWORD stableSampleCount;
    bool firstSampleValid;
    VanillaAnchor firstSample;
    VanillaAnchor frozen;
};

AnchorTracker gAnchor = {};

bool MapIdentityMatchesTracker(const MapState& map) {
    return gAnchor.identityValid &&
           gAnchor.sceneContainer == map.sceneContainer &&
           gAnchor.rawSource == map.rawSource &&
           gAnchor.recordCount == map.recordCount &&
           gAnchor.defaultIndex == map.defaultIndex &&
           gAnchor.sourceFingerprint == map.sourceFingerprint;
}

void ResetAnchorTracker(const MapState& map) {
    gAnchor.identityValid = true;
    gAnchor.sceneContainer = map.sceneContainer;
    gAnchor.rawSource = map.rawSource;
    gAnchor.recordCount = map.recordCount;
    gAnchor.defaultIndex = map.defaultIndex;
    gAnchor.sourceFingerprint = map.sourceFingerprint;
    gAnchor.stableSampleCount = 0u;
    gAnchor.firstSampleValid = false;
    gAnchor.frozen.valid = false;
}

bool ReadCurrentWorldHeader(VanillaAnchor* sample) {
    if (sample == nullptr) {
        return false;
    }

    DWORD worldAddress = 0u;
    if (!ReadExeDword(kWorldGlobalPointerRva, &worldAddress) || worldAddress == 0u) {
        return false;
    }

    BYTE* world = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(worldAddress));
    if (!IsReadableRange(world, kWorldHeaderBytes)) {
        return false;
    }

    ActorState actor;
    if (!ReadControlledActorState(&actor)) {
        return false;
    }

    sample->valid = true;
    sample->actor = actor;
    CopyBytes(sample->worldHeader, world, kWorldHeaderBytes);

    // World 里的这四项通常在正式 Save 准备阶段才由 0x44B150 刷新。
    // Map Tick 采样时为了让锚点真正代表“这一帧的受控角色”，用 live actor 覆盖这四项。
    *reinterpret_cast<DWORD*>(sample->worldHeader + kWorldXOffset) = actor.x;
    *reinterpret_cast<DWORD*>(sample->worldHeader + kWorldYOffset) = actor.y;
    *reinterpret_cast<DWORD*>(sample->worldHeader + kWorldStateOffset) = actor.stateByte & 0xFFu;
    *reinterpret_cast<DWORD*>(sample->worldHeader + kWorldActorIndexOffset) = actor.actorIndex;
    return true;
}

void ObserveVanillaAnchor(const MapState& map) {
    if (!MapIdentityMatchesTracker(map)) {
        ResetAnchorTracker(map);
    }

    if (gAnchor.frozen.valid) {
        // 已有安全入口锚点后不再移动它。
        return;
    }

    if (map.selectedIndex != map.defaultIndex) {
        // “连续稳定 8 Tick”必须真的是连续的。只要中途进入其它 Scene Mode，前面的候选
        // 样本就作废；以后回到 default mode 时从第 1 Tick 重新计数。否则可能把“进楼梯前
        // 3 Tick + 出楼梯后 5 Tick”误拼成一段连续稳定窗口。
        gAnchor.stableSampleCount = 0u;
        gAnchor.firstSampleValid = false;
        return;
    }

    VanillaAnchor current = {};
    if (!ReadCurrentWorldHeader(&current)) {
        // 某一 Tick 连基础 World/actor 都读不到，也不能把前后两段样本当成连续稳定。
        gAnchor.stableSampleCount = 0u;
        gAnchor.firstSampleValid = false;
        return;
    }

    if (!gAnchor.firstSampleValid) {
        // 只保存这段连续稳定窗口的第一个样本。后面 7 Tick 只负责证明它没有发生场景切换。
        CopyBytes(
            reinterpret_cast<BYTE*>(&gAnchor.firstSample),
            reinterpret_cast<const BYTE*>(&current),
            sizeof(VanillaAnchor));
        gAnchor.firstSampleValid = true;
        gAnchor.stableSampleCount = 1u;
        return;
    }

    ++gAnchor.stableSampleCount;
    if (gAnchor.stableSampleCount >= kAnchorMinimumSamples) {
        CopyBytes(
            reinterpret_cast<BYTE*>(&gAnchor.frozen),
            reinterpret_cast<const BYTE*>(&gAnchor.firstSample),
            sizeof(VanillaAnchor));
        gAnchor.frozen.valid = true;
        ycrlog::Line("[回退] 已在默认 Scene Mode 连续稳定 8 Tick 后冻结入口级原版安全锚点。");
    }
}

bool GetFrozenAnchor(const MapState& map, VanillaAnchor* anchor) {
    if (anchor == nullptr || !MapIdentityMatchesTracker(map) || !gAnchor.frozen.valid) {
        return false;
    }

    CopyBytes(
        reinterpret_cast<BYTE*>(anchor),
        reinterpret_cast<const BYTE*>(&gAnchor.frozen),
        sizeof(VanillaAnchor));
    return anchor->valid;
}

// ---------------------------------------------------------------------------
// 八、一次扩展存档授权
// ---------------------------------------------------------------------------
// 原版允许时永远不会建立这个结构；只有原版明确返回 0，且已经存在 vanilla fallback，
// 才把“下一次 Save Writer 应该使用哪份安全头”记下来。

struct ExtendedSaveArm {
    bool valid;
    BYTE* sceneContainer;
    BYTE* rawSource;
    DWORD recordCount;
    DWORD sourceFingerprint;
    DWORD selectedIndex;
    DWORD defaultIndex;
    VanillaAnchor fallback;
};

ExtendedSaveArm gArm = {};

void ClearArm() {
    gArm.valid = false;
}

bool ArmExtendedSave() {
    ClearArm();

    MapState map;
    if (!ReadMapState(&map)) {
        ycrlog::Line("[存档门] 当前地图身份不可读取；为了避免写出未知坏档，本次不扩展开放。");
        return false;
    }

    VanillaAnchor fallback = {};
    if (!GetFrozenAnchor(map, &fallback)) {
        ycrlog::Line("[存档门] 当前地图尚未建立入口级安全锚点；本次暂不扩展开放。");
        return false;
    }

    gArm.valid = true;
    gArm.sceneContainer = map.sceneContainer;
    gArm.rawSource = map.rawSource;
    gArm.recordCount = map.recordCount;
    gArm.sourceFingerprint = map.sourceFingerprint;
    gArm.selectedIndex = map.selectedIndex;
    gArm.defaultIndex = map.defaultIndex;
    CopyBytes(
        reinterpret_cast<BYTE*>(&gArm.fallback),
        reinterpret_cast<const BYTE*>(&fallback),
        sizeof(VanillaAnchor));

    ycrlog::Text("[存档门] 原版拒绝，但已有安全回退；已扩展允许。当前 Scene Mode=");
    ycrlog::Unsigned(map.selectedIndex);
    ycrlog::Text("，默认 Scene Mode=");
    ycrlog::Unsigned(map.defaultIndex);
    ycrlog::Line("。本档将保存当前进度，但读档位置回到入口级安全锚点。");
    return true;
}

bool ArmStillMatchesCurrentMap(MapState* current) {
    if (!gArm.valid || current == nullptr || !ReadMapState(current)) {
        return false;
    }

    return current->sceneContainer == gArm.sceneContainer &&
           current->rawSource == gArm.rawSource &&
           current->recordCount == gArm.recordCount &&
           current->sourceFingerprint == gArm.sourceFingerprint;
}

// ---------------------------------------------------------------------------
// 九、取得原版 Save Writer 正在使用的 World buffer
// ---------------------------------------------------------------------------

bool GetWriterWorldBuffer(void* runtimeManager, BYTE** world) {
    if (runtimeManager == nullptr || world == nullptr ||
        !IsReadableRange(runtimeManager, kRuntimeManagerWorldPointerOffset + sizeof(DWORD))) {
        return false;
    }

    BYTE* manager = reinterpret_cast<BYTE*>(runtimeManager);
    const DWORD worldLength =
        *reinterpret_cast<const volatile DWORD*>(manager + kRuntimeManagerWorldLengthOffset);
    const DWORD worldAddress =
        *reinterpret_cast<const volatile DWORD*>(manager + kRuntimeManagerWorldPointerOffset);

    if (worldLength != kExpectedWorldLength || worldAddress == 0u) {
        return false;
    }

    BYTE* buffer = reinterpret_cast<BYTE*>(static_cast<SIZE_T>(worldAddress));
    if (!IsReadableRange(buffer, kWorldHeaderBytes)) {
        return false;
    }

    *world = buffer;
    return true;
}

// ---------------------------------------------------------------------------
// 十、兼容历史“无条件随时存档”机器码
// ---------------------------------------------------------------------------

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
        ycrlog::Line("[启动] 检测到历史无条件随时存档机器码；已先在内存恢复原版存档许可访问器。");
        return ycr::WriteBytes(
            functionStart,
            kOriginalSaveGateFunctionBytes,
            sizeof(kOriginalSaveGateFunctionBytes));
    }

    ycrlog::Line("[启动失败] 0x40A0C0 不是已确认的原版/历史补丁机器码；拒绝安装安全存档 Hook。");
    return false;
}

// ---------------------------------------------------------------------------
// 十一、一次性预检查并安装三个必要 Hook
// ---------------------------------------------------------------------------

bool InstallHooks() {
    if (!EnsureOriginalSaveGateFunction()) {
        return false;
    }

    gOriginalSaveGate = reinterpret_cast<OriginalSaveGateFunction>(
        gExeBase + kOriginalSaveGateFunctionRva);
    gOriginalMapTick = reinterpret_cast<OriginalMapTickFunction>(
        gExeBase + kOriginalMapTickFunctionRva);
    gOriginalSaveWriter = reinterpret_cast<OriginalSaveWriterFunction>(
        gExeBase + kOriginalSaveWriterFunctionRva);

    // 先全检查再写，避免“前两个 Hook 已经装了，第三个才发现版本不对”的半安装状态。
    if (!ycr::BytesEqual(gExeBase + kNormalMenuSaveGateCallRva, kNormalMenuSaveGateCallBytes, 5u) ||
        !ycr::BytesEqual(gExeBase + kMapTickCallRva, kMapTickCallBytes, 5u) ||
        !ycr::BytesEqual(gExeBase + kSaveWriterCallRva, kSaveWriterCallBytes, 5u)) {
        ycrlog::Line("[启动失败] Hook 机器码预检查失败；未安装任何安全存档 Hook。");
        return false;
    }

    // wrapper 在 namespace 外定义，下面先声明它们。
    return true;
}

} // namespace

// ============================================================================
// 十二、Hook wrappers
// ============================================================================

extern "C" DWORD __fastcall SafeSaveGateHook(void* runtimeEntry) {
    if (gOriginalSaveGate == nullptr || runtimeEntry == nullptr) {
        ClearArm();
        return 0u;
    }

    // 第一原则：先真的执行 RPG.exe 自己的判断。
    const DWORD originalResult = gOriginalSaveGate(runtimeEntry);
    if (originalResult != 0u) {
        // 原版允许的情况完全不进入我们的 fallback 规则。
        ClearArm();
        return originalResult;
    }

    return ArmExtendedSave() ? 1u : 0u;
}

extern "C" void __fastcall SafeMapTickHook(void* sceneContainer, void* unusedEdx) {
    if (gOriginalMapTick != nullptr) {
        // 先让原版完成这一 Tick，再观察“更新之后”的稳定状态。
        gOriginalMapTick(sceneContainer, unusedEdx);
    }

    MapState map;
    if (ReadMapState(&map) && map.sceneContainer == sceneContainer) {
        ObserveVanillaAnchor(map);
    }
}

extern "C" BOOL __fastcall SafeSaveWriterHook(
    void* runtimeManager,
    void* unusedEdx,
    const char* tsfPath) {

    if (gOriginalSaveWriter == nullptr || tsfPath == nullptr) {
        ClearArm();
        return FALSE;
    }

    // gArm=false 代表这是原版自己允许的正常存档，必须原样走原 Writer。
    if (!gArm.valid) {
        return gOriginalSaveWriter(runtimeManager, unusedEdx, tsfPath);
    }

    // 如果从菜单判断到真正 Writer 之间已经换了地图/Map runtime，不能把旧地图锚点套给新地图。
    MapState currentMap;
    if (!ArmStillMatchesCurrentMap(&currentMap)) {
        ycrlog::Line("[保存中止] 存档门判断后地图身份已经变化；为避免把旧锚点写进新场景，本次不写 TSF。");
        ClearArm();
        return FALSE;
    }

    BYTE* world = nullptr;
    if (!GetWriterWorldBuffer(runtimeManager, &world)) {
        ycrlog::Line("[保存中止] 无法取得原版 Writer 的 World 缓冲；为避免写出未规范化存档，本次不写 TSF。");
        ClearArm();
        return FALSE;
    }

    // exactHeader 只是“当前内存临时备份”，绝不会写额外文件。
    // 保存前先备份当前 200 bytes，然后把安全锚点覆盖给原版 Writer。
    BYTE exactHeader[kWorldHeaderBytes];
    CopyBytes(exactHeader, world, kWorldHeaderBytes);
    CopyBytes(world, gArm.fallback.worldHeader, kWorldHeaderBytes);

    // 真正的 TSF 文件仍然完全由原版 0x43B360 写。
    const BOOL saved = gOriginalSaveWriter(runtimeManager, unusedEdx, tsfPath);

    // 无论原版写盘成功还是失败，都必须立刻恢复游戏内存，保证画面上的玩家没有被移动。
    CopyBytes(world, exactHeader, kWorldHeaderBytes);

    if (saved != FALSE) {
        ycrlog::Line("[保存] 原版 TSF 写入成功；场景恢复头已使用入口级安全锚点规范化。未创建任何旁挂文件。");
    } else {
        ycrlog::Line("[保存] 原版 TSF Writer 返回失败；游戏内存已经恢复，本次没有留下增强存档。");
    }

    ClearArm();
    return saved;
}

namespace {

bool InstallFinalHooks() {
    if (!InstallHooks()) {
        return false;
    }

    if (!ycr::InstallRelativeCall(
            kNormalMenuSaveGateCallRva,
            kNormalMenuSaveGateCallBytes,
            reinterpret_cast<const void*>(&SafeSaveGateHook)) ||
        !ycr::InstallRelativeCall(
            kMapTickCallRva,
            kMapTickCallBytes,
            reinterpret_cast<const void*>(&SafeMapTickHook)) ||
        !ycr::InstallRelativeCall(
            kSaveWriterCallRva,
            kSaveWriterCallBytes,
            reinterpret_cast<const void*>(&SafeSaveWriterHook))) {
        ycrlog::Line("[启动失败] Hook 安装失败；不会继续以半安装状态运行。");
        return false;
    }

    ycrlog::Line("[启动] 安全扩展存档 Hook 已安装：原版门控 + Map Tick 锚点 + 原版 Writer 快照规范化。");
    return true;
}

} // namespace

// ============================================================================
// 十三、DLL 入口
// ============================================================================

extern "C" BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        ycrlog::Open(module, L"AnytimeSave.log");
        ycrlog::Line("《幽城幻剑录》安全扩展存档插件 v0.3.1a 启动。");
        ycrlog::Line("By Luminous with ChatGPT");
        ycrlog::Line("[模式] 只保留原版安全回退；TSA/YCS 旁挂与精确恢复链已从正式版删除。");
        ycrlog::Line("[格式] TSF 结构、长度、字段布局保持原版；不会创建任何额外存档文件。");

        gExeBase = ycr::GetExeBase();
        if (gExeBase == nullptr) {
            ycrlog::Line("[启动失败] 无法取得 RPG.exe 模块基址。");
            return TRUE;
        }
        InstallFinalHooks();
    } else if (reason == DLL_PROCESS_DETACH) {
        ycrlog::Line("[退出] 安全扩展存档插件卸载。");
        ycrlog::Close();
    }

    return TRUE;
}
