// 宿主测试直接编译正式可视层，实现与ASI一致，但测试程序允许使用CRT打印断言。
// 所有模拟存档元数据只写到运行器创建的临时目录；游戏目录仅用于读原始图标和字库。
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "../source/SaveVisual.cpp"

using namespace savevisual;
static_assert(sizeof(void*) == 4, "宿主测试必须使用x86，以覆盖正式ASI的对象ABI");
static int gChecks = 0, gFailures = 0;
static const wchar_t* gGameDirectory = nullptr;
static const wchar_t* gTemporaryDirectory = nullptr;

static void Check(bool condition, const char* name) {
    ++gChecks;
    if (!condition) { ++gFailures; printf("[失败] %s\n", name); }
}
static bool TestPath(const wchar_t* relative, wchar_t* output, unsigned int capacity) {
    // 状态文件必须被重定向，绝不能把测试用091/092写进参考资料的真实save目录。
    const bool state = wcsstr(relative, L"\\save\\") || wcsstr(relative, L".NEXTAUTOSLOT") ||
        wcsstr(relative, L".LATESTSLOTS") || wcsstr(relative, L".SAVESTATUS");
    const wchar_t* base = state ? gTemporaryDirectory : gGameDirectory;
    const wchar_t* name = relative;
    if (base == gTemporaryDirectory) {
        name = wcsrchr(relative, L'\\') + 1;
        if (wcsncmp(relative, L"Save\\", 5) == 0) name = L"legacy-NEXTAUTOSLOT";
    }
    else if (wcsncmp(relative, L"..\\", 3) == 0) name = relative + 3;
    return swprintf_s(output, capacity, L"%s\\%s", base, name) > 0;
}
static void TestLog(const char*) {}

static void StateTests() {
    LatestSlots state = {kNoSlot, kNoSlot};
    Check(!MarkLatest(state, 0, true, true), "快速槽不能改变新标记");
    Check(!MarkLatest(state, 1, false, true), "保存失败不改标记");
    Check(!MarkLatest(state, 91, true, false), "回读失败不改标记");
    Check(MarkLatest(state, 90, true, true) && state.manual == 90, "手动上界");
    Check(MarkLatest(state, 91, true, true) && state.manual == 90 && state.automatic == 91, "分类独立");
    Check(MarkLatest(state, 99, true, true) && IsLatest(state, 99, true) && !IsLatest(state, 91, true), "自动新标记移动");
    Check(!IsLatest(state, 99, false), "空槽无新标记");
    Check(!MarkLatest(state, 100, true, true), "越界槽拒绝");
    U8 data[kLatestBytes];
    EncodeLatest(state, data);
    LatestSlots loaded = {kNoSlot, kNoSlot};
    Check(DecodeLatest(data, sizeof(data), loaded) && loaded.manual == 90 && loaded.automatic == 99, "状态往返");
    for (U32 i = 0; i < sizeof(data); ++i) {
        U8 broken[kLatestBytes];
        memcpy(broken, data, sizeof(data));
        broken[i] = 0xFF;
        Check(!DecodeLatest(broken, sizeof(broken), loaded) && loaded.manual == 90, "损坏状态不接收");
    }
    Check(!DecodeLatest(data, 15, loaded) && !DecodeLatest(data, 17, loaded), "长度必须精确");
    data[6] = '0'; data[7] = '9'; data[8] = '1';
    Check(!DecodeLatest(data, 16, loaded), "自动槽不能出现在手动字段");
    Check((NewRed(0, true) >> 16) == 176 && (NewRed(800, true) >> 16) == 240 &&
        NewRed(1600, true) == NewRed(0, true), "红色呼吸周期");
    Check(NewRed(321, false) == NewRed(999, false) && (NewRed(0, false) >> 16) == 240, "关闭呼吸常亮");
    gPath = TestPath; gLog = TestLog;
    gLatest = {1, 91};
    Check(StoreStatus(), "临时目录原子持久化");
    gLatest = {kNoSlot, kNoSlot};
    LoadCurrentStatus();
    Check(gLatest.manual == 1 && gLatest.automatic == 91, "模拟重启读回两个分类");
    wchar_t temporary[520];
    TestPath(L"..\\multimedia\\save\\.SAVESTATUS-TMP", temporary, 520);
    Check(GetFileAttributesW(temporary) == INVALID_FILE_ATTRIBUTES, "成功后无临时文件残留");
    wchar_t target[520];
    TestPath(L"..\\multimedia\\save\\.SAVESTATUS", target, 520);
    HANDLE locked = CreateFileW(target, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    gLatest = {2, 92};
    Check(!StoreStatus(), "替换被拒绝时报告失败");
    CloseHandle(locked);
    LoadCurrentStatus();
    Check(gLatest.manual == 1 && gLatest.automatic == 91, "失败保留旧元数据");
    Check(GetFileAttributesW(temporary) == INVALID_FILE_ATTRIBUTES, "失败后清理临时文件");
    SavedSlot(95, true);
    SavedSlot(2, true);
    Check(gLatest.automatic == 95 && gLatest.manual == 2, "真实SavedSlot入口手动保存不覆盖最新自动槽");
    gLatest = {kNoSlot, kNoSlot}; LoadCurrentStatus();
    Check(gLatest.automatic == 95 && gLatest.manual == 2, "两类真实写入后重启仍独立");
    Check(gNextAuto == 96, "自动成功统一推进游标，手动保存不改变游标");
}

static void WriteFixture(const wchar_t* relative, const U8* data, U32 size) {
    wchar_t path[520]; TestPath(relative, path, 520);
    // 所有测试写路径必须在调用者创建的专用临时目录内，参考游戏永远只读。
    Check(wcsncmp(path, gTemporaryDirectory, wcslen(gTemporaryDirectory)) == 0, "状态样本隔离在临时目录");
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    DWORD written = 0;
    Check(file != INVALID_HANDLE_VALUE && WriteFile(file, data, size, &written, nullptr) && written == size, "创建状态样本");
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
}
static void MigrationTests() {
    U8 unified[kStatusBytes];
    LatestSlots source = {14, kNoSlot}, target = {kNoSlot, kNoSlot};
    U32 cursor = 91;
    EncodeStatus(source, 95, unified);
    Check(DecodeStatus(unified, sizeof(unified), target, cursor) && target.manual == 14 &&
        target.automatic == kNoSlot && cursor == 95, "合并协议三个字段独立往返");
    for (U32 i = 0; i < sizeof(unified); ++i) {
        U8 bad[kStatusBytes]; memcpy(bad, unified, sizeof(bad)); bad[i] = 0xFF;
        Check(!DecodeStatus(bad, sizeof(bad), target, cursor) && cursor == 95, "合并状态坏字节整体拒绝");
    }
    U8 compact[22];
    Check(NormalizeStateText(unified, kStatusBytes, compact, 22) &&
        DecodeStatus(compact, 22, target, cursor), "LF与CRLF都可读取");
    wchar_t newPath[520], oldLatest[520], oldNext[520];
    TestPath(L"..\\multimedia\\save\\.SAVESTATUS", newPath, 520);
    TestPath(L"..\\multimedia\\save\\.LATESTSLOTS", oldLatest, 520);
    TestPath(L"..\\multimedia\\save\\.NEXTAUTOSLOT", oldNext, 520);
    DeleteFileW(newPath);
    U8 legacy[kLatestBytes]; EncodeLatest(source, legacy);
    const U8 oldCursor[3] = {'0', '9', '5'};
    WriteFixture(L"..\\multimedia\\save\\.LATESTSLOTS", legacy, kLatestBytes);
    WriteFixture(L"..\\multimedia\\save\\.NEXTAUTOSLOT", oldCursor, 3);
    Check(LoadStatus(TestPath, TestLog) == 95 && gLatest.manual == 14 && gLatest.automatic == kNoSlot,
        "旧文件合并不从游标猜最新自动槽");
    Check(GetFileAttributesW(oldLatest) == INVALID_FILE_ATTRIBUTES && GetFileAttributesW(oldNext) == INVALID_FILE_ATTRIBUTES &&
        GetFileAttributesW(newPath) != INVALID_FILE_ATTRIBUTES, "新状态成功落盘后才删除旧文件");
    SavedSlot(96, true);
    Check(LoadStatus(TestPath, TestLog) == 97 && gLatest.automatic == 96 && gLatest.manual == 14, "一次自动保存同步持久化A和N");
    const U8 conflicting[3] = {'0', '9', '9'};
    WriteFixture(L"..\\multimedia\\save\\.NEXTAUTOSLOT", conflicting, 3);
    Check(LoadStatus(TestPath, TestLog) == 97, "有效新状态优先于旧游标");

    // 故意让新状态损坏且被其它句柄占用，迁移失败必须保留旧文件以便下次重试。
    const U8 invalid = 0;
    WriteFixture(L"..\\multimedia\\save\\.SAVESTATUS", &invalid, 1);
    WriteFixture(L"..\\multimedia\\save\\.LATESTSLOTS", legacy, kLatestBytes);
    HANDLE locked = CreateFileW(newPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    Check(LoadStatus(TestPath, TestLog) == 99 && gLatest.manual == 14, "迁移失败仍保留可用内存状态");
    Check(GetFileAttributesW(oldLatest) != INVALID_FILE_ATTRIBUTES && GetFileAttributesW(oldNext) != INVALID_FILE_ATTRIBUTES,
        "迁移失败不删除旧文件");
    CloseHandle(locked);
    Check(StoreStatus(), "解除占用后可安全重试合并");
}

static void ResourceTests() {
    Check(LoadFont(), "原版字库11字全部读取");
    for (U32 i = 0; i < GlyphCount; ++i) {
        U32 ink = 0;
        for (U32 j = 0; j < 72; ++j) ink |= gGlyphs[i][j];
        Check(ink != 0, "原版字形非空");
    }
    U32 offset = 0;
    Check(!GlyphOffset(0xA680, 1000000, offset) && !GlyphOffset(0xA6DB, 72, offset), "非法Big5或短字库拒绝");
    // 每一个原字库像素单独点亮，缩为20像素后都必须还留下笔画，不能再出现丢整条细线。
    for (int sy = 0; sy < 24; ++sy) for (int sx = 0; sx < 24; ++sx) {
        U8 single[72] = {};
        single[sy * 3 + sx / 8] = static_cast<U8>(0x80 >> (sx % 8));
        bool found = false;
        for (int y = 0; y < 20; ++y) for (int x = 0; x < 20; ++x) found |= GlyphInk(single, x, y, 20);
        Check(found, "缩字保留每个源笔画像素");
    }
    Check(LoadBook() && gBook.durationMs == 2500, "原版25帧书卷完整解码");
    // 以下逐帧散列由仓库既有Python SF2解码器独立计算，覆盖原像素、透明区和逐帧坐标。
    const U32 hashes[kBookFrames] = {
        0x1e09aecc, 0xab9878f8, 0xce3aa4ca, 0xc3ec3d62, 0x84de826e,
        0xe0fdd66e, 0x827307e8, 0x57055b56, 0x88e0e486, 0xc6266256,
        0xc778628f, 0x51063fb4, 0xe025cb29, 0x14236334, 0x74b7bc41,
        0xfef2696d, 0x685de0c2, 0x2e6cd8c5, 0xedb61d1a, 0x4a31a024,
        0x6152cd8d, 0xa25869f2, 0x2806d98f, 0xdce77cff, 0xa76ca7e5
    };
    for (U32 i = 0; i < kBookFrames; ++i) {
        U32 hash = 2166136261u;
        const U8* pixels = reinterpret_cast<const U8*>(gBook.frames[i].pixels);
        for (U32 j = 0; j < sizeof(gBook.frames[i].pixels); ++j) hash = (hash ^ pixels[j]) * 16777619u;
        Check(hash == hashes[i], "书卷逐帧像素与独立解码器一致");
    }
    Check(BookFrameAt(gBook, 0) == 0 && BookFrameAt(gBook, 99) == 0 &&
        BookFrameAt(gBook, 100) == 1 && BookFrameAt(gBook, 2499) == 24 &&
        BookFrameAt(gBook, 2500) == 0, "动画边界与循环");
    U32 tile[4] = {};
    const U8 literal[] = {0, 0x00, 0x7C};
    const U8 transparent[] = {0x40};
    const U8 repeated[] = {0xC3, 0xE0, 0x03};
    Check(DecodeTile(literal, 3, tile, 1) && tile[0] == 0xFFFF0000, "RGB555原样红色");
    Check(DecodeTile(transparent, 1, tile, 1) && tile[0] == 0, "透明像素");
    Check(DecodeTile(repeated, 3, tile, 4) && tile[3] == 0xFF00FF00, "重复绿色");
    Check(!DecodeTile(repeated, 2, tile, 4) && !DecodeTile(repeated, 3, tile, 3), "截断或输出越界拒绝");
    Check(!DecodeTile(literal, 3, tile, 2), "输出不足拒绝");
    const U8 extra[] = {0x40, 0};
    Check(!DecodeTile(extra, 2, tile, 1), "多余尾字节拒绝");
    Check(!Span(0xFFFFFFF0u, 32, 64), "溢出偏移拒绝");

    // 拿真实SF2做破坏测试，比只测人工RLE片段更容易发现索引层的边界错误。
    U32 size = 0;
    HANDLE file = OpenResource(L"..\\multimedia\\Sys\\Sys.dat", size);
    if (file != INVALID_HANDLE_VALUE) {
        U8* data = static_cast<U8*>(HeapAlloc(GetProcessHeap(), 0, 145753));
        const bool read = data && ReadAt(file, 59272720, data, 145753);
        Check(read, "读取锁定研究样本");
        if (read) {
            Check(!DecodeBook(data, 100, gBook), "短SF2拒绝");
            data[11] = 0x6F;
            Check(!DecodeBook(data, 145753, gBook), "未知压缩包装拒绝");
            data[11] = 0;
            const U32 table = Read32(data + 0x16);
            const U32 frame = Read32(data + table + 159 * 4);
            data[frame + 94] = 60;
            Check(!DecodeBook(data, 145753, gBook), "气泡不能冒充书卷");
        }
        if (data) HeapFree(GetProcessHeap(), 0, data);
        CloseHandle(file);
    }
    Check(LoadBook(), "破坏测试后重新加载正确书卷");
}

static U32 gTime = 100;
static CastleDisplayGeometryV1 gGeometry = {};
static CastleResult gGeometryResult = CASTLE_OK;
static U8 gPixels[1120 * 480 * 4];
static U32 gWidth = 640, gBpp = 32;
static bool gLockFail = false;
static U32 gLocks = 0, gUnlocks = 0;
static CastleResult CASTLE_RUNTIME_CALL FakeNow(CastleU32* value) { *value = gTime; return CASTLE_OK; }
static CastleResult CASTLE_RUNTIME_CALL FakeGeometry(CastleDisplayGeometryV1* value) {
    *value = gGeometry;
    return gGeometryResult;
}
static HRESULT WINAPI FakeLock(IDirectDrawSurface*, RECT*, DDSURFACEDESC* desc, DWORD, HANDLE) {
    ++gLocks;
    if (gLockFail) return E_FAIL;
    desc->lpSurface = gPixels;
    desc->dwWidth = gWidth; desc->dwHeight = 480; desc->lPitch = static_cast<LONG>(gWidth * 4);
    desc->ddpfPixelFormat.dwRGBBitCount = gBpp;
    desc->ddpfPixelFormat.dwRBitMask = 0xFF0000;
    desc->ddpfPixelFormat.dwGBitMask = 0xFF00;
    desc->ddpfPixelFormat.dwBBitMask = 0xFF;
    return S_OK;
}
static HRESULT WINAPI FakeUnlock(IDirectDrawSurface*, void*) { ++gUnlocks; return S_OK; }

static void Put32(U8* destination, U32 value) { memcpy(destination, &value, 4); }
static void PutPointer(U8* destination, const void* value) { memcpy(destination, &value, 4); }
static void RowTests() {
    // 分配模拟EXE全局区和三个owner，所有字段地址均在测试自己的内存里。
    U8* exe = static_cast<U8*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x4E2420));
    Check(exe != nullptr, "模拟对象内存分配");
    if (!exe) return;
    U8 owner[0x658] = {}, save[0x5C4] = {}, row[0x5A8] = {}, number[0x54] = {}, popup[0x57A] = {};
    U32 parentX = 20, parentY = 45;
    gExe = exe;
    PutPointer(save, exe + 0x60B50);
    save[0x578] = save[0x579] = 1;
    Put32(save + 0x598, 88);
    PutPointer(save + 0x584, row);
    PutPointer(row, exe + 0x60B70);
    row[0x578] = row[0x579] = row[0x57C] = 1;
    PutPointer(row + 0x5A4, number);
    number[0x30] = number[0x31] = 1;
    PutPointer(number + 0x14, &parentX); PutPointer(number + 0x18, &parentY);
    Put32(number + 0x20, 540); Put32(number + 0x24, 10);
    Put32(number + 0x0C, 2); Put32(number + 0x10, 3);
    Put32(number + 0x44, 88);
    const U32 globals[3] = {0x4DED0C, 0x49FCD0, 0x4E241C};
    const U32 offsets[3] = {0x654, 0x580, 0x5B4};
    for (U32 i = 0; i < 3; ++i) {
        memset(owner, 0, sizeof(owner));
        PutPointer(exe + globals[i], owner);
        PutPointer(owner + offsets[i], save);
        if (i == 0) Put32(owner + 0x5BC, 7);
        Row rows[4] = {};
        U8 before[sizeof(save)]; memcpy(before, save, sizeof(save));
        Check(ReadRows(rows) == 1 && rows[0].slot == 88 && rows[0].hasData &&
            rows[0].numberX == 562 && rows[0].numberY == 58, "三种入口使用原控件动态坐标");
        Check(memcmp(save, before, sizeof(save)) == 0, "读取不改焦点和输入对象");
        save[0x578] = row[0x578] = 0;
        Check(ReadRows(rows) == 1, "更新开关不作为可见槽号的额外门槛");
        save[0x578] = row[0x578] = 1;
        for (U32 remaining = 4; remaining != 0; --remaining) {
            Put32(save + 0x5BC, remaining);
            Check(ReadRows(rows) == 1, "高亮动画期间标签不闪断");
        }
        Put32(save + 0x5BC, 0);
        Put32(number + 0x44, 92);
        Check(ReadRows(rows) == 0, "显示槽号尚未与当前页一致时跳过该行");
        Put32(number + 0x44, 88);
        PutPointer(save + 0x5AC, popup); popup[0x579] = 1;
        Check(ReadRows(rows) == 1, "仅打开确认框仍保留槽位标识");
        BeginLoad();
        Check(ReadRows(rows) == 0, "实际进入读档立即隐藏标识");
        EndLoad();
        save[0x5B9] = 1;
        Check(ReadRows(rows) == 0, "读档返回后的原版交接标记继续隐藏");
        save[0x5B9] = 0; Put32(save + 0x580, 1);
        Check(ReadRows(rows) == 0, "loading关闭阶段active未清也不显示");
        Put32(save + 0x580, 0);
        Check(ReadRows(rows) == 1, "重新进入正常列表恢复显示而不清除最新状态");
        popup[0x579] = 0;
        row[0x57C] = 0;
        Check(ReadRows(rows) == 1 && !rows[0].hasData, "空槽仍保留类型但不标新");
        row[0x57C] = 1;
        Put32(save + 0x598, 100);
        Check(ReadRows(rows) == 0, "越界页拒绝");
        Put32(save + 0x598, 88);
        PutPointer(exe + globals[i], nullptr);
    }
    HeapFree(GetProcessHeap(), 0, exe);
    gExe = nullptr;
}

static void RenderingTests() {
    U8 guarded[20]; memset(guarded, 0x77, sizeof(guarded));
    DDSURFACEDESC desc = {}; desc.lpSurface = guarded + 4;
    desc.dwWidth = 2; desc.dwHeight = 2; desc.lPitch = 4;
    desc.ddpfPixelFormat.dwRGBBitCount = 16;
    desc.ddpfPixelFormat.dwRBitMask = 0xF800;
    desc.ddpfPixelFormat.dwGBitMask = 0x7E0;
    desc.ddpfPixelFormat.dwBBitMask = 31;
    Canvas canvas = {};
    Check(CanvasFormat(desc, canvas), "RGB565格式");
    Pixel(canvas, 0, 0, 0xFF0000);
    Check(Read16(guarded + 4) == 0xF800, "RGB565正确打包");
    Pixel(canvas, -1, 0, 0); Pixel(canvas, 2, 0, 0); Pixel(canvas, 0, 2, 0);
    Check(guarded[3] == 0x77 && guarded[12] == 0x77, "裁剪保护边界");
    desc.ddpfPixelFormat.dwRGBBitCount = 32; desc.lPitch = 8;
    desc.ddpfPixelFormat.dwRBitMask = 0xFF0000;
    desc.ddpfPixelFormat.dwGBitMask = 0xFF00;
    desc.ddpfPixelFormat.dwBBitMask = 0xFF;
    Check(CanvasFormat(desc, canvas), "RGB32格式");
    Pixel(canvas, 0, 0, 0xF01818);
    Check(Read32(guarded + 4) == 0x77F01818, "颜色与alpha保留");
    desc.ddpfPixelFormat.dwRGBBitCount = 24; desc.lPitch = 6;
    Check(CanvasFormat(desc, canvas), "RGB24格式");
    desc.lPitch = -6; desc.lpSurface = guarded + 10;
    Check(CanvasFormat(desc, canvas), "负行距支持");
    Pixel(canvas, 0, 1, 0x010203);
    Check(guarded[4] == 3 && guarded[5] == 2 && guarded[6] == 1, "负行距寻址");
    desc.lPitch = 1;
    Check(!CanvasFormat(desc, canvas), "过短行距拒绝");
    desc.lPitch = 6; desc.ddpfPixelFormat.dwRBitMask = 0;
    Check(!CanvasFormat(desc, canvas), "未知颜色掩码拒绝");

    // 把一个测试字临时替换为中心单像素，验证描边确实覆盖八个方向而不覆盖字心。
    U8 original[72]; memcpy(original, gGlyphs[Xin], 72);
    memset(gGlyphs[Xin], 0, 72); gGlyphs[Xin][12 * 3 + 1] = 0x08;
    U8 outlined[32 * 32 * 4]; memset(outlined, 0, sizeof(outlined));
    desc.lpSurface = outlined; desc.dwWidth = desc.dwHeight = 32; desc.lPitch = 128;
    desc.ddpfPixelFormat.dwRGBBitCount = 32; desc.ddpfPixelFormat.dwRBitMask = 0xFF0000;
    Check(CanvasFormat(desc, canvas), "描边测试画布");
    gFontReady = true;
    const Glyph word[] = {Xin};
    DrawWord(canvas, 2, 2, word, 1, 24, 0xF3DB9B, true);
    Check((Read32(outlined + (14 * 32 + 14) * 4) & 0xFFFFFF) == 0xF3DB9B, "描边保留浅金字心");
    for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) {
        if (!dx && !dy) continue;
        Check((Read32(outlined + ((14 + dy) * 32 + 14 + dx) * 4) & 0xFFFFFF) == 0x160D06, "八方向完整描边");
    }
    memcpy(gGlyphs[Xin], original, 72);

    // 使用真正的原版“新”字点阵，分别检查自动/手动行都有红色像素，而不是只测元数据。
    desc.lpSurface = gPixels; desc.dwWidth = 640; desc.dwHeight = 480; desc.lPitch = 640 * 4;
    Check(CanvasFormat(desc, canvas), "最新标记测试画布");
    CastleDisplayGeometryV1 rowGeometry = {};
    gOptions = {false, true, true, false, true, 0, 0, false};
    gLatest = {2, 95};
    const Row latestRows[2] = {{580, 40, 2, true}, {580, 100, 95, true}};
    memset(gPixels, 0, sizeof(gPixels));
    DrawRows(canvas, latestRows, 2, rowGeometry, 0);
    for (U32 i = 0; i < 2; ++i) {
        U32 red = 0;
        for (int y = latestRows[i].numberY; y < latestRows[i].numberY + 20; ++y)
            for (int x = 480; x < 568; ++x)
                if ((Read32(gPixels + (y * 640 + x) * 4) & 0xFFFFFF) == 0xF01818) ++red;
        Check(red != 0, "手动与自动新字均实际写入画布");
    }
    U32 coverage[3] = {};
    for (U32 weight = 0; weight < 3; ++weight) {
        memset(gPixels, 0, sizeof(gPixels));
        DrawGlyph(canvas, 2, 2, Xin, 20, 0xF01818, weight * 100);
        for (int y = 0; y < 24; ++y) for (int x = 0; x < 24; ++x)
            if (Read32(gPixels + (y * 640 + x) * 4) != 0) ++coverage[weight];
    }
    Check(coverage[0] < coverage[1] && coverage[1] < coverage[2], "三个粗细档位实际覆盖面积递增");
    memcpy(original, gGlyphs[Xin], 72);
    memset(gGlyphs[Xin], 0, 72); gGlyphs[Xin][12 * 3 + 1] = 0x08;
    for (U32 weight = 0; weight <= 200; ++weight) {
        memset(gPixels, 0, 32 * 640 * 4);
        DrawGlyph(canvas, 2, 2, Xin, 24, 0xC86432, weight);
        const U32 first = (Read32(gPixels + (14 * 640 + 15) * 4) >> 16) & 255;
        const U32 second = (Read32(gPixels + (14 * 640 + 16) * 4) >> 16) & 255;
        Check(first == (weight >= 100 ? 200 : weight * 2) &&
            second == (weight <= 100 ? 0 : (weight - 100) * 2), "201种粗细值均按百分比混合边缘");
    }
    memcpy(gGlyphs[Xin], original, 72);
    int tops[2] = {-1, -1};
    for (int pass = 0; pass < 2; ++pass) {
        memset(gPixels, 0, sizeof(gPixels));
        gOptions.slotTextOffsetY = pass == 0 ? 0 : -2;
        DrawRows(canvas, latestRows, 2, rowGeometry, 0);
        for (int y = 20; y < 70 && tops[pass] < 0; ++y)
            for (int x = 480; x < 568; ++x)
                if ((Read32(gPixels + (y * 640 + x) * 4) & 0xFFFFFF) == 0xF01818) { tops[pass] = y; break; }
    }
    Check(tops[0] >= 0 && tops[1] == tops[0] - 2, "文字垂直偏移精确生效");

    // 通过真正Draw回调驱动一张模拟DirectDraw表面，检查失败路径也能正确解锁。
    void* methods[33] = {}; methods[25] = reinterpret_cast<void*>(FakeLock); methods[32] = reinterpret_cast<void*>(FakeUnlock);
    void** table = methods;
    U8 renderer[16] = {}; *reinterpret_cast<void**>(renderer + 8) = &table;
    static CastleClockApiV1 clock = {}; clock.GetMonotonicMilliseconds = FakeNow;
    static CastleDisplayApiV1 display = {}; display.GetGeometry = FakeGeometry;
    gClock = &clock; gDisplay = &display;
    gExe = renderer; // 本段关闭槽位读取，因此不会把模拟renderer当作真正EXE读。
    gOptions = {true, false, false, true, true, 0, 0, false};
    gBookReady = gFontReady = true;
    gGeometry.output_width = 640; gGeometry.output_height = 480;
    gGeometry.center_width = 640; gGeometry.center_height = 480;
    gGeometry.generation = 1; gGeometry.display_mode = CASTLE_DISPLAY_ORIGINAL_4_3;
    gGeometry.projection_scope = CASTLE_PROJECTION_CENTER_640;
    gGeometryResult = CASTLE_STATUS_DEFAULT_BACKEND;
    CastleOverlayContextV1 context = {};
    context.magic = CASTLE_OVERLAY_CONTEXT_MAGIC;
    context.phase = CASTLE_OVERLAY_PHASE_BEFORE_PRESENT;
    context.display_generation = 1;
    context.render_context = reinterpret_cast<CastleAddress>(renderer);
    FinishSave(true, true);
    Check(Draw(&context, nullptr) == CASTLE_OK && gLocks == 1 && gUnlocks == 1 && gNotice.presented, "原版默认后端返回2也绘制并解锁");
    gTime += 2600;
    Draw(&context, nullptr);
    Check(!gNotice.active, "完成提示按毫秒消失");
    FinishSave(true, false);
    gLockFail = true;
    Draw(&context, nullptr);
    Check(gUnlocks == 2 && !gNotice.presented, "锁失败不解锁且不消耗提示");
    gLockFail = false; gBpp = 8;
    Draw(&context, nullptr);
    Check(gUnlocks == 3 && !gNotice.presented, "不支持格式仍解锁");
    gBpp = 32; gWidth = 854; gGeometry.output_width = 854; gGeometry.center_x = 107;
    gGeometryResult = CASTLE_OK;
    Draw(&context, nullptr);
    Check(gNotice.presented, "16比9最终表面绘制");
    gWidth = 1120; gGeometry.output_width = 1120; gGeometry.center_x = 240;
    FinishSave(false, true); Draw(&context, nullptr);
    Check(gNotice.presented, "21比9最终表面绘制");
    gGeometry.generation = 2; const U32 before = gLocks;
    Draw(&context, nullptr);
    Check(gLocks == before, "显示代次不匹配时不绘制");
    gGeometry.generation = 1; gGeometry.display_mode = CASTLE_DISPLAY_TRANSITION;
    Draw(&context, nullptr);
    Check(gLocks == before, "过渡帧不绘制");
    gGeometry.display_mode = CASTLE_DISPLAY_ORIGINAL_4_3;
    gGeometryResult = CASTLE_ERROR_NOT_READY;
    Draw(&context, nullptr);
    Check(gLocks == before, "负值才是几何查询失败");
}

static void SlotOutlineTests() {
    DDSURFACEDESC desc = {};
    desc.lpSurface = gPixels; desc.dwWidth = 640; desc.dwHeight = 480; desc.lPitch = 640 * 4;
    desc.ddpfPixelFormat.dwRGBBitCount = 32;
    desc.ddpfPixelFormat.dwRBitMask = 0xFF0000; desc.ddpfPixelFormat.dwGBitMask = 0xFF00;
    desc.ddpfPixelFormat.dwBBitMask = 0xFF;
    Canvas canvas = {}; Check(CanvasFormat(desc, canvas), "槽位描边独立测试画布");
    CastleDisplayGeometryV1 geometry = {};
    const U32 slots[] = {2, 95, 0}; // 分别单独测“新”“自动”“快速”，避免只测到组合里的某一项。
    const U32 weights[] = {0, 50, 100};
    gLatest = {2, 95}; gFontReady = true; gProtectCursor = false;
    U32 before[32][128];
    for (U32 item = 0; item < 3; ++item) for (U32 weight : weights) {
        const Row row = {580, 40, slots[item], true};
        gOptions = {false, item != 0, item == 0, false, true, weight, 0, false};
        memset(gPixels, 0, sizeof(gPixels));
        DrawRows(canvas, &row, 1, geometry, 0);
        U32 withoutOutline = 0;
        for (int y = 0; y < 32; ++y) for (int x = 0; x < 128; ++x) {
            before[y][x] = Read32(gPixels + ((y + 30) * 640 + x + 480) * 4) & 0xFFFFFF;
            if (before[y][x] == 0x160D06) ++withoutOutline;
        }
        Check(withoutOutline == 0, "关闭开关不增加描边像素");
        gOptions.slotTextOutline = true;
        memset(gPixels, 0, sizeof(gPixels));
        DrawRows(canvas, &row, 1, geometry, 0);
        U32 dark = 0; bool nearStroke = true, inkKept = true;
        const U32 ink = item == 0 ? 0xF01818u : 0x682D16u;
        for (int y = 1; y < 31; ++y) for (int x = 1; x < 127; ++x) {
            const U32 color = Read32(gPixels + ((y + 30) * 640 + x + 480) * 4) & 0xFFFFFF;
            if (before[y][x] == ink && color != ink) inkKept = false;
            if (color != 0x160D06) continue;
            ++dark;
            bool adjacent = false;
            for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx)
                adjacent |= before[y + dy][x + dx] != 0;
            nearStroke &= adjacent;
        }
        Check(dark != 0, "三种槽位文字分别开启深色描边");
        Check(nearStroke, "描边始终在现有笔画一像素邻域内");
        Check(inkKept, "开启描边保留原实心笔画颜色");
    }
}

static void CursorTests() {
    U32 size = 0;
    HANDLE file = OpenResource(L"..\\multimedia\\fight\\mouse\\mousedefault.sf2", size);
    U8* raw = static_cast<U8*>(HeapAlloc(GetProcessHeap(), 0, size));
    const bool loaded = file != INVALID_HANDLE_VALUE && raw && ReadAt(file, 0, raw, size);
    Check(loaded, "只读加载原版手形光标资源");
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (!loaded) { if (raw) HeapFree(GetProcessHeap(), 0, raw); return; }
    Check(DecodeCursorFrame(raw, size, 0, gCursorFrame), "原版当前光标帧透明像素解码");
    Check(!DecodeCursorFrame(raw, size, 99999, gCursorFrame), "越界光标帧拒绝");
    U8* exe = static_cast<U8*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x4E1C50));
    Check(exe != nullptr, "模拟光标对象分配");
    if (!exe) { HeapFree(GetProcessHeap(), 0, raw); return; }
    U8 mouse[0x249] = {}, sprite[0x360] = {}, parser[0x12E] = {};
    gExe = exe;
    PutPointer(exe + 0x4E1C4C, mouse); PutPointer(mouse + 0x240, sprite);
    PutPointer(sprite + 0x35C, parser); PutPointer(parser + 0x20, raw);
    mouse[0x248] = sprite[8] = parser[0x12D] = 1;
    Put32(mouse + 0x238, 200); Put32(mouse + 0x23C, 150);
    U8 savedMouse[sizeof(mouse)]; memcpy(savedMouse, mouse, sizeof(mouse));
    const int sides[] = {0, 107, 240};
    const U32 depths[] = {16, 32};
    for (U32 depth : depths) for (int side : sides) {
        DDSURFACEDESC desc = {};
        desc.lpSurface = gPixels; desc.dwWidth = static_cast<DWORD>(640 + 2 * side);
        desc.dwHeight = 480; desc.lPitch = static_cast<LONG>(desc.dwWidth * (depth / 8));
        desc.ddpfPixelFormat.dwRGBBitCount = depth;
        desc.ddpfPixelFormat.dwRBitMask = depth == 16 ? 0xF800 : 0xFF0000;
        desc.ddpfPixelFormat.dwGBitMask = depth == 16 ? 0x07E0 : 0xFF00;
        desc.ddpfPixelFormat.dwBBitMask = depth == 16 ? 0x001F : 0xFF;
        Canvas canvas = {}; Check(CanvasFormat(desc, canvas), "光标保护画布");
        CastleDisplayGeometryV1 geometry = {}; geometry.center_x = side;
        memset(gPixels, 0, sizeof(gPixels));
        ProtectNativeCursor(canvas, geometry);
        Check(!gProtectCursor, "Controller未画光标时不在文字上挖洞");
        const int left = side + 200 - 319 + gCursorFrame.anchorX;
        const int top = 150 - 260 + gCursorFrame.anchorY;
        for (U32 y = 0; y < gCursorFrame.height; ++y) for (U32 x = 0; x < gCursorFrame.width; ++x) {
            const U32 color = gCursorFrame.pixels[y * 96 + x];
            if (color >> 24) Pixel(canvas, left + static_cast<int>(x), top + static_cast<int>(y), color);
        }
        ProtectNativeCursor(canvas, geometry);
        Check(gProtectCursor && gCursorLeft == left && gCursorTop == top, "已绘制光标按中央偏移正确定位");
        bool opaqueKept = true, transparentWritten = true;
        U32 opaque = 0, transparent = 0;
        for (U32 y = 0; y < gCursorFrame.height; ++y) for (U32 x = 0; x < gCursorFrame.width; ++x) {
            const int px = left + static_cast<int>(x), py = top + static_cast<int>(y);
            const U8* pixel = gPixels + py * canvas.pitch + px * canvas.bytes;
            const U32 originalPixel = depth == 16 ? Read16(pixel) : Read32(pixel);
            Pixel(canvas, px, py, 0xFF1818);
            const U32 after = depth == 16 ? Read16(pixel) : Read32(pixel);
            if (gCursorFrame.pixels[y * 96 + x] >> 24) { ++opaque; opaqueKept &= after == originalPixel; }
            else { ++transparent; transparentWritten &= (after & 0xFFFFFF) == (depth == 16 ? 0xF8A2u : 0xFF1818u); }
        }
        Check(opaque != 0 && opaqueKept, "文字不覆盖光标不透明像素");
        Check(transparent != 0 && transparentWritten, "光标透明区域仍正常显示文字");
        mouse[0x248] = 0; ProtectNativeCursor(canvas, geometry);
        Check(!gProtectCursor, "原版不画光标时不强制遮挡");
        mouse[0x248] = 1;
    }
    Check(memcmp(savedMouse, mouse, sizeof(mouse)) == 0 && Read32(parser + 0x38) == 0,
        "光标保护不写位置显隐或动画状态");
    gProtectCursor = false; gExe = nullptr;
    HeapFree(GetProcessHeap(), 0, exe); HeapFree(GetProcessHeap(), 0, raw);
}

int wmain(int argc, wchar_t** argv) {
    if (argc != 3) return 2;
    SetConsoleOutputCP(CP_UTF8);
    gGameDirectory = argv[1]; gTemporaryDirectory = argv[2];
    StateTests(); MigrationTests(); ResourceTests(); RowTests(); RenderingTests(); SlotOutlineTests(); CursorTests();
    printf("[汇总] 检查=%d 失败=%d\n", gChecks, gFailures);
    return gFailures ? 1 : 0;
}
