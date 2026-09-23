#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <ddraw.h>
#include "SaveVisual.h"
#include "SaveVisualCore.h"

// 不链接DirectDraw库，不创建新设备：只在Runtime允许的Present前回调中，
// 借用游戏现成的back surface，Lock写入少量像素后立刻Unlock。
// 原版字库和书卷资源只读；不调用曾导致崩溃的游戏File::Read/Write。
namespace savevisual {
namespace {
const CastleOverlayApiV1* gOverlay = nullptr;
const CastleDisplayApiV1* gDisplay = nullptr;
const CastleClockApiV1* gClock = nullptr;
CastleLeaseHandle gLease = 0;
Options gOptions = {};
PathBuilder gPath = nullptr;
LogLine gLog = nullptr;
U8* gExe = nullptr;
LatestSlots gLatest = {kNoSlot, kNoSlot};
U32 gNextAuto = 91;
Book gBook = {};
bool gBookReady = false;
bool gFontReady = false;
bool gLoggedDraw = false;
U32 gDiagnosticBits = 0;
bool gLoading = false;

// 只缓存实际用到的11个原版字形，不复制近1MB整套字库，也不选择系统替代字体。
enum Glyph { Zi, Dong, Kuai, Su, Cun, Dang, Wan, Cheng, Shi, Bai, Xin, GlyphCount };
const U16 kGlyphCodes[GlyphCount] = {
    0xA6DB, 0xB0CA, 0xA7D6, 0xB374, 0xA673, 0xC0C9,
    0xA7B9, 0xA6A8, 0xA5A2, 0xB1D1, 0xB773
};
U8 gGlyphs[GlyphCount][72] = {};

struct Notice {
    bool active;
    bool automatic;
    bool finished;
    bool success;
    bool presented;
    U32 animationStart;
    U32 visibleStart;
};
Notice gNotice = {};

void Log(const char* text) { if (gLog) gLog(text); }
void LogOnce(U32 bit, const char* text) {
    // 每类诊断只记一次，避免每帧刷屏；诊断不改变绘制、保存或输入状态。
    if ((gDiagnosticBits & bit) != 0) return;
    gDiagnosticBits |= bit;
    Log(text);
}
void LogLatestState() {
    // 两个最新槽分别输出，不把“下一覆盖游标”混进诊断。999明确表示尚未观察到成功保存。
    char line[] = "[存档状态] M=999 A=999 N=091（M手动/A自动最新，N下一覆盖；999尚无记录）。";
    for (U32 i = 0; line[i] != '\0'; ++i) {
        if ((line[i] == 'M' || line[i] == 'A' || line[i] == 'N') && line[i + 1] == '=') {
            const U32 slot = line[i] == 'M' ? gLatest.manual : (line[i] == 'A' ? gLatest.automatic : gNextAuto);
            line[i + 2] = static_cast<char>('0' + slot / 100 % 10);
            line[i + 3] = static_cast<char>('0' + slot / 10 % 10);
            line[i + 4] = static_cast<char>('0' + slot % 10);
        }
    }
    Log(line);
}
bool Now(U32& value) {
    CastleU32 time = 0;
    if (!gClock || gClock->GetMonotonicMilliseconds(&time) != CASTLE_OK) return false;
    value = static_cast<U32>(time);
    return true;
}

bool ReadAt(HANDLE file, U32 offset, void* buffer, U32 bytes) {
    // SetFilePointerEx使用绝对偏移，不依赖游戏可能修改的当前工作目录或文件游标。
    LARGE_INTEGER position;
    position.QuadPart = offset;
    DWORD read = 0;
    return SetFilePointerEx(file, position, nullptr, FILE_BEGIN) &&
        ReadFile(file, buffer, bytes, &read, nullptr) && read == bytes;
}
HANDLE OpenResource(const wchar_t* relative, U32& size) {
    wchar_t path[520];
    size = 0;
    if (!gPath || !gPath(relative, path, 520)) return INVALID_HANDLE_VALUE;
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return file;
    LARGE_INTEGER length;
    // 拒绝大于256MB的意外文件；本模块只需要约65MB容器的索引和其中145KB光标资源。
    if (!GetFileSizeEx(file, &length) || length.QuadPart < 0 || length.QuadPart > 256 * 1024 * 1024) {
        CloseHandle(file);
        return INVALID_HANDLE_VALUE;
    }
    size = static_cast<U32>(length.QuadPart);
    return file;
}

bool LoadFont() {
    U32 size = 0;
    HANDLE file = OpenResource(L"..\\multimedia\\Font\\font24.fnt", size);
    if (file == INVALID_HANDLE_VALUE) return false;
    bool ok = true;
    for (U32 i = 0; ok && i < GlyphCount; ++i) {
        U32 offset = 0;
        ok = GlyphOffset(kGlyphCodes[i], size, offset) && ReadAt(file, offset, gGlyphs[i], 72);
    }
    CloseHandle(file);
    return ok;
}
bool CursorName(const U8* record) {
    const char name[] = "CURSOR.SF2";
    for (U32 i = 0; i < sizeof(name); ++i) {
        U8 c = record[10 + i];
        if (c >= 'a' && c <= 'z') c = static_cast<U8>(c - 'a' + 'A');
        if (c != static_cast<U8>(name[i])) return false;
    }
    return true;
}
bool LoadBook() {
    U32 size = 0;
    HANDLE file = OpenResource(L"..\\multimedia\\Sys\\Sys.dat", size);
    if (file == INVALID_HANDLE_VALUE) return false;
    U8 header[10];
    bool ok = ReadAt(file, 0, header, 10) && Read32(header) == 1 && Read16(header + 4) == 1;
    const U32 indexSize = ok ? Read32(header + 6) : 0;
    ok = ok && indexSize >= 39 && indexSize <= 1024 * 1024 && indexSize % 39 == 0 && Span(10, indexSize, size);
    U32 offset = 0, bytes = 0;
    // 按容器索引寻找文件，不把研究样本中59272720这个物理偏移写死。
    for (U32 i = 0; ok && i < indexSize / 39; ++i) {
        U8 record[39];
        if (!ReadAt(file, 10 + i * 39, record, 39)) { ok = false; break; }
        if (Read16(record) == 0 && CursorName(record)) {
            offset = Read32(record + 35);
            bytes = Read32(record + 2);
            break;
        }
    }
    ok = ok && bytes >= 0x42BC && bytes <= 1024 * 1024 && offset >= 10 + indexSize && Span(offset, bytes, size);
    U8* data = ok ? static_cast<U8*>(HeapAlloc(GetProcessHeap(), 0, bytes)) : nullptr;
    ok = data && ReadAt(file, offset, data, bytes) && DecodeBook(data, bytes, gBook);
    if (data) HeapFree(GetProcessHeap(), 0, data);
    CloseHandle(file);
    return ok;
}

bool ReadStatusFile(const wchar_t* relative, U8* data, U32 capacity, U32& size) {
    HANDLE file = OpenResource(relative, size);
    if (file == INVALID_HANDLE_VALUE) return false;
    const bool ok = size <= capacity && ReadAt(file, 0, data, size);
    CloseHandle(file);
    return ok;
}
bool LoadCurrentStatus() {
    U8 data[kStatusBytes];
    U32 size = 0;
    return ReadStatusFile(L"..\\multimedia\\save\\.SAVESTATUS", data, kStatusBytes, size) &&
        DecodeStatus(data, size, gLatest, gNextAuto);
}
void CleanupLegacyStatus() {
    const wchar_t* paths[] = {L"..\\multimedia\\save\\.LATESTSLOTS",
        L"..\\multimedia\\save\\.NEXTAUTOSLOT", L"Save\\.NEXTAUTOSLOT"};
    for (U32 i = 0; i < 3; ++i) {
        wchar_t path[520];
        if (gPath(paths[i], path, 520) && DeleteFileW(path))
            Log("[存档状态] .SAVESTATUS已成功写入，已清理一个旧状态文件；不删除目录或TSF。");
    }
}
bool StoreStatus() {
    wchar_t path[520], temporary[520], directory[520];
    if (!gPath || !gPath(L"..\\multimedia\\save\\.SAVESTATUS", path, 520) ||
        !gPath(L"..\\multimedia\\save\\.SAVESTATUS-TMP", temporary, 520) ||
        !gPath(L"..\\multimedia\\save\\", directory, 520)) return false;
    // 与既有轮换游标一样，只确保真实存档目录的最后一级存在，不在exe旁新建Save。
    if (!CreateDirectoryW(directory, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) return false;
    U8 data[kStatusBytes];
    EncodeStatus(gLatest, gNextAuto, data);
    // 先写临时文件再同目录替换。写盘失败时旧元数据仍在，游戏TSF完全不受影响。
    HANDLE file = CreateFileW(temporary, GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    bool ok = WriteFile(file, data, kStatusBytes, &written, nullptr) &&
        written == kStatusBytes && FlushFileBuffers(file);
    if (!CloseHandle(file)) ok = false;
    if (ok) ok = MoveFileExW(temporary, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE;
    if (!ok) DeleteFileW(temporary);
    else CleanupLegacyStatus();
    return ok;
}

bool Readable(const void* pointer, U32 size) {
    // Present回调和原版UI更新在同一游戏线程；这里只短暂读取，不保存跨帧对象指针。
    // 逐页检查，避免“首地址可读，字段却落入无效页”的误判。
    ULONG_PTR current = reinterpret_cast<ULONG_PTR>(pointer);
    const ULONG_PTR end = current + size;
    if (!pointer || end < current) return false;
    while (current < end) {
        MEMORY_BASIC_INFORMATION info;
        if (!VirtualQuery(reinterpret_cast<const void*>(current), &info, sizeof(info)) ||
            info.State != MEM_COMMIT || (info.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return false;
        const ULONG_PTR next = reinterpret_cast<ULONG_PTR>(info.BaseAddress) + info.RegionSize;
        if (next <= current) return false;
        current = next;
    }
    return true;
}
U8* Pointer(const U8* source) {
    if (!Readable(source, 4)) return nullptr;
    return reinterpret_cast<U8*>(static_cast<ULONG_PTR>(Read32(source)));
}
U8* VisibleSave() {
    // 三种入口只解包原版owner，不引用Controller，也不写它的焦点/按钮/动画字段。
    const U32 owners[3] = {0x4DED0C, 0x49FCD0, 0x4E241C};
    const U32 slots[3] = {0x654, 0x580, 0x5B4};
    for (U32 i = 0; i < 3; ++i) {
        U8* owner = Pointer(gExe + owners[i]);
        if (!Readable(owner, slots[i] + 4)) continue;
        if (i == 0 && (Read32(owner + 0x5BC) != 7 || Read32(owner + 0x5F8) != 0)) continue;
        if (i == 1 && (!Readable(owner, 0x588) || Read32(owner + 0x584) != 0)) continue;
        U8* save = Pointer(owner + slots[i]);
        if (!Readable(save, 0x5C4) || save[0x579] == 0 ||
            Pointer(save) != gExe + 0x60B50) continue;
        // 原版在LoadSlot返回后置+0x5B9，接着转入+0x580=1的关闭/过渡状态。
        // 对象此时可能仍在内存里，甚至仍标active；不能仅凭指针还存在就继续覆盖loading画面。
        if (save[0x5B9] != 0 || Read32(save + 0x580) != 0) continue;
        // +0x5BC是选中行的高亮动画倒数，每次上下移动也会设为4，不是翻页事务标记。
        // 不能因它非零停画整页标签，否则每次手柄/鼠标切换都会闪一下。
        // 本回调与原版更新在同一线程；再逐行核对显示槽号即可避免贴错行。
        if (Read32(save + 0x598) > 96 || Read32(save + 0x598) % 4) {
            LogOnce(2, "[可视诊断] 活动存档界面页基址越界，跳过标签。");
            continue;
        }
        // 打开动作/确认窗口本身不是读档开始，不再因此隐藏标签；真正进入LoadSlot才隐藏。
        return save;
    }
    return nullptr;
}

struct Row { int numberX, numberY; U32 slot; bool hasData; };
U32 ReadRows(Row rows[4]) {
    if (gLoading) return 0;
    U8* save = VisibleSave();
    if (!save) return 0;
    U32 count = 0;
    for (U32 i = 0; i < 4; ++i) {
        U8* row = Pointer(save + 0x584 + i * 4);
        // 不把父对象传播的更新开关+0x578再作为绘制门；槽号控件自身可见位才是取位依据。
        if (!Readable(row, 0x5A8) || Pointer(row) != gExe + 0x60B70 || !row[0x579]) continue;
        U8* number = Pointer(row + 0x5A4);
        if (!Readable(number, 0x54) || !number[0x30] || !number[0x31]) continue;
        const U32 slot = Read32(save + 0x598) + i;
        // +0x44正是原版这一帧拿来画槽号的值；不使用旧缓存，不干预高亮动画。
        if (Read32(number + 0x44) != slot) {
            LogOnce(8, "[可视诊断] 原槽号与页号暂不一致，仅跳过该行标签。");
            continue;
        }
        U8* parentX = Pointer(number + 0x14);
        U8* parentY = Pointer(number + 0x18);
        if (!Readable(parentX, 4) || !Readable(parentY, 4)) continue;
        // 完整复用0x431680中的坐标公式：父坐标+控件相对位置+运行时偏移。
        // 因此不把用户截图的放大像素或每行固定Y值写死。
        const U32 x = Read32(parentX) + Read32(number + 0x20) + Read32(number + 0x0C);
        const U32 y = Read32(parentY) + Read32(number + 0x24) + Read32(number + 0x10);
        if (x > 640 || y > 480) continue;
        rows[count++] = {static_cast<int>(x), static_cast<int>(y),
            slot, row[0x57C] != 0};
    }
    if (!count) LogOnce(512, "[可视诊断] 已找到活动SaveSlot，但当前没有通过坐标/槽号检查的行。");
    return count;
}

struct Canvas {
    U8* pixels; int width, height, pitch; U32 bytes;
    U32 masks[3], shifts[3], maxima[3];
};
CursorFrame gCursorFrame = {};
bool gProtectCursor = false;
int gCursorLeft = 0, gCursorTop = 0;
bool CanvasFormat(const DDSURFACEDESC& desc, Canvas& canvas) {
    const U32 bits = desc.ddpfPixelFormat.dwRGBBitCount;
    if (!desc.lpSurface || !desc.dwWidth || !desc.dwHeight || desc.dwWidth > 8192 || desc.dwHeight > 8192 ||
        (bits != 16 && bits != 24 && bits != 32)) return false;
    canvas.pixels = static_cast<U8*>(desc.lpSurface);
    canvas.width = static_cast<int>(desc.dwWidth);
    canvas.height = static_cast<int>(desc.dwHeight);
    canvas.pitch = desc.lPitch;
    canvas.bytes = bits / 8;
    if (canvas.pitch == (-2147483647 - 1) ||
        static_cast<U32>(canvas.pitch < 0 ? -canvas.pitch : canvas.pitch) < desc.dwWidth * canvas.bytes) return false;
    canvas.masks[0] = desc.ddpfPixelFormat.dwRBitMask;
    canvas.masks[1] = desc.ddpfPixelFormat.dwGBitMask;
    canvas.masks[2] = desc.ddpfPixelFormat.dwBBitMask;
    if (!canvas.masks[0] || !canvas.masks[1] || !canvas.masks[2] ||
        (canvas.masks[0] & canvas.masks[1]) || (canvas.masks[0] & canvas.masks[2]) ||
        (canvas.masks[1] & canvas.masks[2])) return false;
    for (U32 i = 0; i < 3; ++i) {
        U32 value = canvas.masks[i], shift = 0;
        while ((value & 1) == 0) { value >>= 1; ++shift; }
        // 只接受连续且最多8位的RGB通道，不猜测未知像素格式。
        if (value > 255 || (value & (value + 1)) || (bits < 32 && (canvas.masks[i] >> bits))) return false;
        canvas.shifts[i] = shift;
        canvas.maxima[i] = value;
    }
    return true;
}
void Pixel(Canvas& canvas, int x, int y, U32 rgb) {
    if (x < 0 || y < 0 || x >= canvas.width || y >= canvas.height) return;
    // 槽位文字只让出光标的不透明像素，透明孔洞仍可见文字，不是粗暴隐藏整块矩形。
    if (gProtectCursor && x >= gCursorLeft && y >= gCursorTop &&
        static_cast<U32>(x - gCursorLeft) < gCursorFrame.width &&
        static_cast<U32>(y - gCursorTop) < gCursorFrame.height &&
        (gCursorFrame.pixels[(y - gCursorTop) * 96 + x - gCursorLeft] >> 24)) return;
    U8* pixel = canvas.pixels + y * canvas.pitch + x * static_cast<int>(canvas.bytes);
    U32 old = 0, packed = 0, mask = 0;
    for (U32 i = 0; i < canvas.bytes; ++i) old |= static_cast<U32>(pixel[i]) << (i * 8);
    for (U32 i = 0; i < 3; ++i) {
        const U32 channel = (rgb >> (16 - i * 8)) & 255;
        packed |= (channel * canvas.maxima[i] / 255) << canvas.shifts[i];
        mask |= canvas.masks[i];
    }
    // 不碰RGB以外的位，避免32位wrapper的alpha字节被清空。
    packed |= old & ~mask;
    for (U32 i = 0; i < canvas.bytes; ++i) pixel[i] = static_cast<U8>(packed >> (i * 8));
}
bool CursorAlreadyVisible(const Canvas& canvas) {
    U32 visible = 0, matches = 0;
    // Controller可能主动隐藏鼠标，仅检查原版enable不够；还需确认最终画面确实已有该光标。
    // 用整幅光标的大多数颜色验证，避免隐藏光标留下一个文字空洞；容许16位转换的少量量化误差。
    for (U32 y = 0; y < gCursorFrame.height; ++y) for (U32 x = 0; x < gCursorFrame.width; ++x) {
        const U32 expected = gCursorFrame.pixels[y * 96 + x];
        const int sx = gCursorLeft + static_cast<int>(x), sy = gCursorTop + static_cast<int>(y);
        if (!(expected >> 24) || sx < 0 || sy < 0 || sx >= canvas.width || sy >= canvas.height) continue;
        const U8* pixel = canvas.pixels + sy * canvas.pitch + sx * static_cast<int>(canvas.bytes);
        U32 packed = 0;
        for (U32 b = 0; b < canvas.bytes; ++b) packed |= static_cast<U32>(pixel[b]) << (b * 8);
        bool same = true;
        for (U32 c = 0; c < 3; ++c) {
            const int actual = static_cast<int>(((packed & canvas.masks[c]) >> canvas.shifts[c]) * 255 / canvas.maxima[c]);
            const int wanted = static_cast<int>((expected >> (16 - c * 8)) & 255);
            // 5位通道经展开/再打包可累积到9级误差，不能沿用8位通道的8级上限。
            const int tolerance = canvas.maxima[c] <= 31 ? 9 : 8;
            if (actual - wanted > tolerance || wanted - actual > tolerance) same = false;
        }
        ++visible;
        if (same) ++matches;
    }
    return visible >= 8 && matches * 4 >= visible * 3;
}
void ProtectNativeCursor(const Canvas& canvas, const CastleDisplayGeometryV1& geometry) {
    gProtectCursor = false;
    U8* mouse = Pointer(gExe + 0x4E1C4C);
    if (!Readable(mouse, 0x249) || !mouse[0x248]) return;
    U8* sprite = Pointer(mouse + 0x240);
    if (!Readable(sprite, 0x360) || !sprite[8]) return;
    U8* parser = Pointer(sprite + 0x35C);
    if (!Readable(parser, 0x12E) || !parser[0x12D]) return;
    U8* raw = Pointer(parser + 0x20);
    if (!Readable(raw, 0x42BC) || Read32(raw) != 0x05324653u) return;
    U32 size = 0x42BC;
    for (U32 section = 0; section < 4; ++section) {
        const U32 offset = Read32(raw + 0x16 + section * 10);
        const U32 length = Read32(raw + 0x1A + section * 10);
        if (!Span(offset, length, 2 * 1024 * 1024)) return;
        if (offset + length > size) size = offset + length;
    }
    if (!Readable(raw, size) || !DecodeCursorFrame(raw, size, Read32(parser + 0x38), gCursorFrame)) return;
    // 原MouseDraw的319/260是图形锚点，Display的center才是宽屏偏移。
    const int mx = static_cast<int>(Read32(mouse + 0x238)), my = static_cast<int>(Read32(mouse + 0x23C));
    if (mx < -8192 || mx > 8192 || my < -8192 || my > 8192) return;
    gCursorLeft = geometry.center_x + mx - 319 + gCursorFrame.anchorX;
    gCursorTop = geometry.center_y + my - 260 + gCursorFrame.anchorY;
    gProtectCursor = CursorAlreadyVisible(canvas);
    if (gProtectCursor) LogOnce(4096, "[可视反馈] 已保护原版光标不透明像素，槽位文字位于光标下方；未重画或推进光标。");
}
void BlendPixel(Canvas& canvas, int x, int y, U32 rgb, U32 percent) {
    if (x < 0 || y < 0 || x >= canvas.width || y >= canvas.height || !percent) return;
    U8* pixel = canvas.pixels + y * canvas.pitch + x * static_cast<int>(canvas.bytes);
    U32 packed = 0, blended = 0;
    for (U32 i = 0; i < canvas.bytes; ++i) packed |= static_cast<U32>(pixel[i]) << (8 * i);
    for (U32 i = 0; i < 3; ++i) {
        const U32 before = ((packed & canvas.masks[i]) >> canvas.shifts[i]) * 255 / canvas.maxima[i];
        const U32 after = (rgb >> (16 - 8 * i)) & 255;
        blended |= ((before * (100 - percent) + after * percent + 50) / 100) << (16 - 8 * i);
    }
    Pixel(canvas, x, y, blended);
}
void DrawGlyph(Canvas& canvas, int x, int y, Glyph glyph, int size, U32 color, U32 weight) {
    for (int dy = 0; dy < size; ++dy) {
        for (int dx = 0; dx < size; ++dx) {
            if (!GlyphInk(gGlyphs[glyph], dx, dy, size)) continue;
            Pixel(canvas, x + dx, y + dy, color);
            // 由TOML决定是否补笔画；限制在本字格内，不侵占字间距。0仍保留覆盖采样，不丢细线。
            const U32 limited = weight > 200 ? 200 : weight;
            for (U32 added = 1; added <= limited / 100; ++added)
                if (dx + static_cast<int>(added) < size) Pixel(canvas, x + dx + static_cast<int>(added), y + dy, color);
            const int edge = dx + static_cast<int>(limited / 100) + 1;
            if (edge < size && limited % 100)
                BlendPixel(canvas, x + edge, y + dy, color, limited % 100);
        }
    }
}
void DrawWord(Canvas& canvas, int x, int y, const Glyph* word, U32 count, int size, U32 color, bool outline, U32 weight = 0) {
    if (!gFontReady) return;
    // 提示叠在世界画面上，单方向阴影无法隔开土黄背景和浅金笔画。
    // 先在八个相邻位置画一圈深色字形，再统一画文字本体，得到一像素完整描边。
    // 全词描边先完成，避免后一个字的描边盖住前一个字；不需要额外贴图或背景面板。
    if (outline) {
        for (U32 i = 0; i < count; ++i) {
            const int at = x + static_cast<int>(i) * size;
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx)
                    if (dx != 0 || dy != 0) DrawGlyph(canvas, at + dx, y + dy, word[i], size, 0x160D06, weight);
        }
    }
    for (U32 i = 0; i < count; ++i) {
        const int at = x + static_cast<int>(i) * size;
        DrawGlyph(canvas, at, y, word[i], size, color, weight);
    }
}
void DrawRows(Canvas& canvas, const Row* rows, U32 count, const CastleDisplayGeometryV1& geo, U32 now) {
    const Glyph automatic[] = {Zi, Dong}, quick[] = {Kuai, Su}, fresh[] = {Xin};
    for (U32 i = 0; i < count; ++i) {
        // 两字类型放在原槽号左侧，再往左才是红色“新”。20像素加粗，修复test1缩字丢笔画。
        const Row& row = rows[i];
        if (gOptions.latestMark && AutoSlot(row.slot) && gLatest.automatic == kNoSlot)
            LogOnce(1024, "[最新存档诊断] 当前页含自动槽，但A=999；尚无已记录的成功自动保存，不从轮换游标猜测。");
        if (gOptions.latestMark && row.slot == gLatest.automatic && !row.hasData)
            LogOnce(2048, "[最新存档诊断] 最新自动槽已记录，但原版当前行没有有效存档数据，因此不画新字。");
        const int right = geo.center_x + row.numberX - 12;
        const int y = geo.center_y + row.numberY + gOptions.slotTextOffsetY;
        int left = right;
        if (gOptions.slotLabels && (row.slot == 0 || AutoSlot(row.slot))) {
            left -= 40;
            DrawWord(canvas, left, y, row.slot == 0 ? quick : automatic, 2, 20, 0x682D16, false, gOptions.slotTextWeight);
        }
        if (gOptions.latestMark && IsLatest(gLatest, row.slot, row.hasData)) {
            DrawWord(canvas, left - 28, y, fresh, 1, 20, NewRed(now, gOptions.pulse), false, gOptions.slotTextWeight);
        }
    }
}
void DrawNotice(Canvas& canvas, const CastleDisplayGeometryV1& geo, U32 now) {
    if (!gNotice.active || !gOptions.notification) return;
    const U32 duration = gNotice.success ? 2600 : 4200;
    if (gNotice.finished && gNotice.presented && now - gNotice.visibleStart >= duration) {
        gNotice.active = false;
        return;
    }
    // 动画只在真正有一帧可画时起算，避免慢写盘/Alt-Tab吞掉全部成功提示。
    if (!gNotice.presented) {
        gNotice.animationStart = now;
        gNotice.visibleStart = now;
        gNotice.presented = true;
    }
    int edge = static_cast<int>(geo.output_width);
    int top = 0, bottom = static_cast<int>(geo.output_height);
    if (geo.projection_scope != CASTLE_PROJECTION_FULL_OUTPUT) {
        edge = geo.center_x + geo.center_width;
        top = geo.center_y;
        bottom = geo.center_y + geo.center_height;
    }
    const int x = edge - 192;
    const int y = gOptions.bottom ? bottom - 50 : top + 20;
    if (gBookReady) {
        const BookFrame& frame = gBook.frames[BookFrameAt(gBook, now - gNotice.animationStart)];
        for (U32 dy = 0; dy < kBookHeight; ++dy)
            for (U32 dx = 0; dx < kBookWidth; ++dx) {
                const U32 color = frame.pixels[dy * kBookWidth + dx];
                if (color >> 24) Pixel(canvas, x + static_cast<int>(dx), y + static_cast<int>(dy), color);
            }
    }
    const Glyph automatic[] = {Zi, Dong, Cun, Dang};
    const Glyph quick[] = {Kuai, Su, Cun, Dang};
    const Glyph complete[] = {Cun, Dang, Wan, Cheng};
    const Glyph failed[] = {Cun, Dang, Shi, Bai};
    const Glyph* word = gNotice.finished ? (gNotice.success ? complete : failed) :
        (gNotice.automatic ? automatic : quick);
    DrawWord(canvas, x + 46, y + 3, word, 4, 24,
        gNotice.finished && !gNotice.success ? 0xFF6050 : 0xF3DB9B, true);
}

CastleResult CASTLE_RUNTIME_CALL Draw(const CastleOverlayContextV1* context, void*) {
    if (!context || !gDisplay || !gExe || context->magic != CASTLE_OVERLAY_CONTEXT_MAGIC ||
        context->phase != CASTLE_OVERLAY_PHASE_BEFORE_PRESENT || !context->render_context) return CASTLE_OK;
    LogOnce(1, "[可视诊断] 已收到Runtime Present前绘制回调。");
    U32 now = 0;
    if (!Now(now)) return CASTLE_OK;
    Row rows[4];
    const U32 count = gFontReady && (gOptions.slotLabels || gOptions.latestMark) ? ReadRows(rows) : 0;
    if (!count && (!gOptions.notification || !gNotice.active)) return CASTLE_OK;
    CastleDisplayGeometryV1 geometry = {};
    geometry.magic = CASTLE_DISPLAY_GEOMETRY_MAGIC;
    geometry.struct_size = CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1;
    geometry.api_version = CASTLE_DISPLAY_API_VERSION_1;
    // SDK非负值均表示成功。无Widescreen时默认后端正常返回2，不能用!=0把原版画面挡掉。
    const CastleResult geometryResult = gDisplay->GetGeometry(&geometry);
    if (geometryResult < 0) {
        LogOnce(16, "[可视诊断] Display几何查询尚未成功，等待下一帧。");
        return CASTLE_OK;
    }
    if (geometryResult == CASTLE_STATUS_DEFAULT_BACKEND)
        LogOnce(32, "[可视诊断] 使用Runtime原版4:3默认显示后端，不依赖宽屏插件。");
    if (geometry.display_mode == CASTLE_DISPLAY_UNKNOWN ||
        geometry.display_mode == CASTLE_DISPLAY_TRANSITION ||
        geometry.generation != context->display_generation || geometry.center_width != 640 || geometry.center_height != 480) {
        LogOnce(64, "[可视诊断] 本帧几何过渡、代次或中央尺寸不匹配，跳过叠加。");
        return CASTLE_OK;
    }
    U8* renderer = reinterpret_cast<U8*>(static_cast<ULONG_PTR>(context->render_context));
    U8* surface = renderer ? Pointer(renderer + 8) : nullptr;
    if (!Readable(surface, 4) || !Readable(Pointer(surface), 33 * 4)) return CASTLE_OK;
    auto* back = reinterpret_cast<IDirectDrawSurface*>(surface);
    DDSURFACEDESC desc = {};
    desc.dwSize = sizeof(desc);
    if (FAILED(back->Lock(nullptr, &desc, DDLOCK_WAIT, nullptr))) {
        LogOnce(128, "[可视诊断] DirectDraw表面本次锁定失败，等待下一帧。");
        return CASTLE_OK;
    }
    Canvas canvas = {};
    const bool supported = CanvasFormat(desc, canvas) && desc.dwWidth == geometry.output_width &&
        desc.dwHeight == geometry.output_height;
    if (supported) {
        if (count) ProtectNativeCursor(canvas, geometry);
        DrawRows(canvas, rows, count, geometry, now);
        gProtectCursor = false; // 已验收的独立书卷提示保持原绘制行为。
        DrawNotice(canvas, geometry, now);
    } else LogOnce(256, "[可视诊断] 表面尺寸或RGB格式不匹配，已解锁且不影响存档。");
    // Lock成功的每条分支都配对Unlock，即使格式不支持也不能把表面锁留给游戏。
    back->Unlock(desc.lpSurface);
    if (supported && !gLoggedDraw) {
        gLoggedDraw = true;
        Log("[可视反馈] 已在Runtime Present前绘制；原版字库/书卷，无输入或按钮状态写入。");
    }
    return CASTLE_OK;
}
}

void Initialize(const CastleOverlayApiV1* overlay, const CastleDisplayApiV1* display,
    const CastleClockApiV1* clock, CastlePluginHandle plugin, unsigned char* exe,
    const Options& options, PathBuilder path, LogLine log) {
    gOptions = options; gPath = path; gLog = log; gExe = exe;
    gClock = clock; gOverlay = overlay; gDisplay = display;
    if (!options.notification && !options.slotLabels && !options.latestMark) return;
    if (!overlay || !display || !clock) {
        Log("[可视反馈] Overlay/Display/Clock接口不可用，仅停用画面提示；存档核心保持工作。");
        return;
    }
    gFontReady = LoadFont();
    gBookReady = options.notification && LoadBook();
    if (!gFontReady) Log("[可视反馈] 原版Font24字库不可用，不使用系统字体替代；槽位文字停用。");
    if (options.notification && !gBookReady) Log("[可视反馈] 原版书卷资源校验失败，提示仅显示文字；保存不受影响。");
    if (!gFontReady && !gBookReady) return;
    CastleOverlayClientV1 client = {};
    client.magic = CASTLE_OVERLAY_CLIENT_MAGIC;
    client.struct_size = CASTLE_SIZEOF_OVERLAY_CLIENT_V1;
    client.version = CASTLE_OVERLAY_STRUCTURE_VERSION_1;
    client.phase = CASTLE_OVERLAY_PHASE_BEFORE_PRESENT;
    client.priority = CASTLE_OVERLAY_PRIORITY_DEFAULT;
    client.draw = Draw;
    const char label[] = "SaveEnhance visual feedback";
    client.label = {label, sizeof(label) - 1};
    if (overlay->RegisterOverlay(plugin, &client, &gLease) != CASTLE_OK) {
        gLease = 0;
        Log("[可视反馈] 注册失败，仅停用画面提示；存档核心保持工作。");
    } else if (overlay->SetOverlayReady(gLease, 1) != CASTLE_OK) {
        overlay->UnregisterOverlay(gLease);
        gLease = 0;
        Log("[可视反馈] 启用失败，仅停用画面提示；存档核心保持工作。");
    } else Log("[可视反馈] 已启用原版书卷、自动/快速标签和独立红色新标记。");
}
void Shutdown() {
    if (gOverlay && gLease) gOverlay->UnregisterOverlay(gLease);
    gLease = 0;
    gNotice.active = false;
}
void BeginSave(bool automatic) {
    // 原版保存同步执行于游戏线程，本函数绝不强制Present或另起写盘线程。
    // 若保存同帧返回，玩家首先看到真实完成/失败结果，不伪造一段“正在保存”。
    gNotice = {};
    gNotice.active = true;
    gNotice.automatic = automatic;
}
void FinishSave(bool automatic, bool success) {
    gNotice = {};
    gNotice.active = true;
    gNotice.automatic = automatic;
    gNotice.finished = true;
    gNotice.success = success;
}
void SavedSlot(unsigned int slot, bool readable) {
    if (!MarkLatest(gLatest, slot, true, readable)) return;
    // 自动成功时同时更新最新自动槽与下一覆盖候选，三个字段一次原子写入，手动保存不动N。
    if (AutoSlot(slot)) gNextAuto = slot == 99 ? 91 : slot + 1;
    if (!StoreStatus()) Log("[存档状态] .SAVESTATUS写入失败；本次存档有效，本轮保留内存状态。");
    LogLatestState();
}
void BeginLoad() { gLoading = true; }
void EndLoad() { gLoading = false; }
unsigned int NextAutoSlot() { return gNextAuto; }
unsigned int LoadStatus(PathBuilder path, LogLine log) {
    gPath = path; gLog = log;
    gLatest = {kNoSlot, kNoSlot}; gNextAuto = 91;
    if (!LoadCurrentStatus()) {
        U8 data[kStatusBytes], normalized[kLatestBytes]; U32 size = 0;
        bool migrated = ReadStatusFile(L"..\\multimedia\\save\\.LATESTSLOTS", data, kStatusBytes, size) &&
            NormalizeStateText(data, size, normalized, kLatestBytes) && DecodeLatest(normalized, kLatestBytes, gLatest);
        const wchar_t* cursors[] = {L"..\\multimedia\\save\\.NEXTAUTOSLOT", L"Save\\.NEXTAUTOSLOT"};
        for (U32 i = 0; i < 2; ++i) {
            if (!ReadStatusFile(cursors[i], data, kStatusBytes, size) || size != 3 ||
                data[0] != '0' || data[1] != '9' || data[2] < '1' || data[2] > '9') continue;
            gNextAuto = 90 + data[2] - '0';
            migrated = true;
            break;
        }
        if (migrated) {
            Log("[存档状态] 已读取旧最新槽/环形游标，准备合并到multimedia/save/.SAVESTATUS。");
            if (!StoreStatus()) Log("[存档状态] 合并写入失败，旧文件原样保留；本轮继续使用读到的状态。");
        } else Log("[存档状态] 无有效.SAVESTATUS或旧状态；最新槽未知，下一覆盖候选91。");
    }
    LogLatestState();
    return gNextAuto;
}
}
