#ifndef CASTLE_SAVE_VISUAL_CORE_H
#define CASTLE_SAVE_VISUAL_CORE_H

// 这里只放与游戏进程无关的小算法。宿主测试和正式插件使用同一份实现，
// 因而可以在不启动游戏、不改原资源的情况下检查损坏文件、动画和最新槽状态。
namespace savevisual {
using U8 = unsigned char;
using U16 = unsigned short;
using U32 = unsigned int;
static_assert(sizeof(U32) == 4, "需要32位整数");

inline U16 Read16(const U8* p) { return static_cast<U16>(p[0] | (p[1] << 8)); }
inline U32 Read32(const U8* p) {
    return static_cast<U32>(p[0]) | (static_cast<U32>(p[1]) << 8) |
        (static_cast<U32>(p[2]) << 16) | (static_cast<U32>(p[3]) << 24);
}
inline bool Span(U32 start, U32 count, U32 size) {
    // 先减后比，避免“很大的偏移 + 长度”发生整数回绕后误通过检查。
    return start <= size && count <= size - start;
}

struct LatestSlots { U32 manual; U32 automatic; };
constexpr U32 kNoSlot = 999;
constexpr U32 kLatestBytes = 16;
inline bool ManualSlot(U32 slot) { return slot >= 1 && slot <= 90; }
inline bool AutoSlot(U32 slot) { return slot >= 91 && slot <= 99; }

inline bool MarkLatest(LatestSlots& state, U32 slot, bool saved, bool readable) {
    // 失败或回读失败不能移动“新”；快速槽0也不能抢走任何一个分类的标记。
    if (!saved || !readable) return false;
    if (ManualSlot(slot)) state.manual = slot;
    else if (AutoSlot(slot)) state.automatic = slot;
    else return false;
    return true;
}
inline bool IsLatest(const LatestSlots& state, U32 slot, bool hasData) {
    return hasData && ((ManualSlot(slot) && slot == state.manual) ||
        (AutoSlot(slot) && slot == state.automatic));
}
inline void EncodeLatest(const LatestSlots& state, U8 out[kLatestBytes]) {
    // 固定16字节：SE1换行、M=三位数字换行、A=三位数字换行。
    // 999表示本分类尚无已知最新档，不从槽号或下一覆盖游标猜测旧档的新旧。
    const char pattern[] = "SE1\nM=999\nA=999\n";
    for (U32 i = 0; i < kLatestBytes; ++i) out[i] = static_cast<U8>(pattern[i]);
    const U32 slots[2] = {state.manual, state.automatic};
    for (U32 i = 0; i < 2; ++i) {
        const U32 offset = 6 + i * 6;
        out[offset] = static_cast<U8>('0' + slots[i] / 100 % 10);
        out[offset + 1] = static_cast<U8>('0' + slots[i] / 10 % 10);
        out[offset + 2] = static_cast<U8>('0' + slots[i] % 10);
    }
}
inline bool DecodeLatest(const U8* data, U32 size, LatestSlots& state) {
    if (!data || size != kLatestBytes || data[0] != 'S' || data[1] != 'E' ||
        data[2] != '1' || data[3] != '\n' || data[4] != 'M' || data[5] != '=' ||
        data[9] != '\n' || data[10] != 'A' || data[11] != '=' || data[15] != '\n') return false;
    LatestSlots candidate = {kNoSlot, kNoSlot};
    U32* fields[2] = {&candidate.manual, &candidate.automatic};
    for (U32 i = 0; i < 2; ++i) {
        U32 value = 0;
        for (U32 j = 0; j < 3; ++j) {
            const U8 digit = data[6 + i * 6 + j];
            if (digit < '0' || digit > '9') return false;
            value = value * 10 + digit - '0';
        }
        if (value != kNoSlot && !(i == 0 ? ManualSlot(value) : AutoSlot(value))) return false;
        *fields[i] = value;
    }
    // 全部字段合法才整体接收，不让损坏文件覆盖已知的内存状态。
    state = candidate;
    return true;
}

constexpr U32 kStatusBytes = 26;
inline bool NormalizeStateText(const U8* data, U32 size, U8* out, U32 expected) {
    // 允许旧文件LF或用户编辑器保存的CRLF；其它多余字节、孤立CR、BOM都拒绝。
    U32 used = 0;
    for (U32 i = 0; i < size; ++i) {
        if (data[i] == '\r') {
            if (i + 1 >= size || data[i + 1] != '\n') return false;
            continue;
        }
        if (used == expected) return false;
        out[used++] = data[i];
    }
    return used == expected;
}
inline void EncodeStatus(const LatestSlots& state, U32 next, U8 out[kStatusBytes]) {
    const char pattern[] = "SE2\r\nM=999\r\nA=999\r\nN=091\r\n";
    for (U32 i = 0; i < kStatusBytes; ++i) out[i] = static_cast<U8>(pattern[i]);
    const U32 values[3] = {state.manual, state.automatic, next};
    for (U32 i = 0; i < 3; ++i) {
        const U32 offset = 7 + i * 7;
        out[offset] = static_cast<U8>('0' + values[i] / 100 % 10);
        out[offset + 1] = static_cast<U8>('0' + values[i] / 10 % 10);
        out[offset + 2] = static_cast<U8>('0' + values[i] % 10);
    }
}
inline bool DecodeStatus(const U8* data, U32 size, LatestSlots& state, U32& next) {
    U8 normalized[22];
    if (!data || !NormalizeStateText(data, size, normalized, 22) || normalized[2] != '2' ||
        normalized[16] != 'N' || normalized[17] != '=' || normalized[21] != '\n') return false;
    // 复用已测试的两类最新槽校验，所有字段通过后才一起替换内存，避免部分接收坏文件。
    normalized[2] = '1';
    LatestSlots candidate = {kNoSlot, kNoSlot};
    if (!DecodeLatest(normalized, 16, candidate)) return false;
    U32 parsed = 0;
    for (U32 i = 18; i < 21; ++i) {
        if (normalized[i] < '0' || normalized[i] > '9') return false;
        parsed = parsed * 10 + normalized[i] - '0';
    }
    if (!AutoSlot(parsed)) return false;
    state = candidate;
    next = parsed;
    return true;
}

inline U32 NewRed(U32 now, bool pulse) {
    // 一个整数三角波就能产生慢速明暗变化；始终为红色且不消失。
    // 没有定时器、线程、对象队列，也不随游戏帧率改变速度。
    const U32 phase = now % 1600;
    const U32 triangle = phase < 800 ? phase : 1600 - phase;
    const U32 red = pulse ? 176 + triangle * 64 / 800 : 240;
    return (red << 16) | 0x1818u;
}

// 字库是原版Font24.Fnt，每字24行、每行3字节，一位表示一个笔画像素。
inline bool GlyphOffset(U16 big5, U32 size, U32& offset) {
    const U32 lead = big5 >> 8;
    const U32 trail = big5 & 255;
    if (lead < 0xA4 || lead > 0xC8 ||
        !((trail >= 0x40 && trail <= 0x7E) || (trail >= 0xA1 && trail <= 0xFE))) return false;
    const U32 index = 256 + (lead - 0xA4) * 157 +
        (trail >= 0xA1 ? trail - 0x62 : trail - 0x40);
    offset = index * 72;
    return Span(offset, 72, size);
}

inline bool GlyphInk(const U8* glyph, int x, int y, int size) {
    if (!glyph || size < 1 || size > 24 || x < 0 || y < 0 || x >= size || y >= size) return false;
    // 缩小点阵不能只挑一个源像素，否则24->18/20时，恰落在未采样列的竖线会整条消失。
    // 目标像素覆盖的源矩形中只要有笔画就保留，原大小24时自然退化为一对一读取。
    const int beginX = x * 24 / size, endX = ((x + 1) * 24 + size - 1) / size;
    const int beginY = y * 24 / size, endY = ((y + 1) * 24 + size - 1) / size;
    for (int sy = beginY; sy < endY; ++sy)
        for (int sx = beginX; sx < endX; ++sx)
            if (glyph[sy * 3 + sx / 8] & (0x80 >> (sx % 8))) return true;
    return false;
}

constexpr U32 kBookFrames = 25;
constexpr U32 kBookWidth = 38;
constexpr U32 kBookHeight = 30;
struct BookFrame {
    U32 pixels[kBookWidth * kBookHeight]; // 0表示透明，其余为不透明ARGB。
    U32 durationMs;
};
struct Book { BookFrame frames[kBookFrames]; U32 durationMs; };

inline bool Record(const U8* data, U32 size, U32 section, U32 index,
                   const U8*& record, U32& length) {
    if (!data || size < 0x42BC || section > 2) return false;
    const U8* header = data + 0x14 + section * 10;
    const U32 count = Read16(header);
    const U32 table = Read32(header + 2);
    const U32 bytes = Read32(header + 6);
    if (index >= count || table < 0x42BC || !Span(table, bytes, size) || count * 4 > bytes) return false;
    const U32 begin = Read32(data + table + index * 4);
    const U32 end = index + 1 < count ? Read32(data + table + (index + 1) * 4) : table + bytes;
    if (begin < table + count * 4 || end < begin || end > table + bytes) return false;
    record = data + begin;
    length = end - begin;
    return true;
}
inline U32 Color555(U16 value) {
    return 0xFF000000u | (((value >> 10) & 31) * 255 / 31 << 16) |
        (((value >> 5) & 31) * 255 / 31 << 8) | ((value & 31) * 255 / 31);
}
inline bool DecodeTile(const U8* data, U32 size, U32* pixels, U32 count) {
    // SF2压缩块只有三种指令：原样颜色、透明、重复颜色。
    // 每读一次都检查输入长度和输出容量，未知/截断资源仅让图标不可用。
    U32 input = 0, output = 0;
    while (output < count) {
        if (input >= size) return false;
        const U8 code = data[input++];
        const U32 run = (code & 63) + 1;
        if (run > count - output) return false;
        if ((code & 64) == 0) {
            if (!Span(input, run * 2, size)) return false;
            for (U32 i = 0; i < run; ++i) {
                pixels[output++] = Color555(Read16(data + input));
                input += 2;
            }
        } else {
            U32 color = 0;
            if ((code & 128) != 0) {
                if (!Span(input, 2, size)) return false;
                color = Color555(Read16(data + input));
                input += 2;
            }
            for (U32 i = 0; i < run; ++i) pixels[output++] = color;
        }
    }
    return input == size;
}
inline bool DecodeBook(const U8* data, U32 size, Book& book) {
    if (!data || size < 0x42BC || Read32(data) != 0x05324653u ||
        data[11] != 0 || Read16(data + 7) != 64 || Read16(data + 9) != 48) return false;
    U32 tile[64 * 48];
    book.durationMs = 0;
    for (U32 i = 0; i < kBookFrames; ++i) {
        const U8* frame = nullptr; U32 length = 0;
        // 锁定用户选中的原版书卷：组合159～183引用图像33～57。
        // 不尝试猜测另一版本的索引，更不会误取60开始的对话气泡。
        if (!Record(data, size, 0, 159 + i, frame, length) || length != 123 ||
            Read16(frame + 84) != 1 || Read16(frame + 94) != 33 + i ||
            Read16(frame + 100) != 44 || Read16(frame + 34) != 2) return false;
        const int dx = static_cast<short>(Read16(frame + 96)) - 291;
        const int dy = static_cast<short>(Read16(frame + 98)) - 245;
        const U8* image = nullptr;
        if (!Record(data, size, 1, 33 + i, image, length) || length != 26 ||
            Read16(image) != 1 || Read16(image + 2) != 1) return false;
        const U32 width = Read32(image + 4), height = Read32(image + 8);
        if (dx < 0 || dy < 0 || width == 0 || height == 0 ||
            static_cast<U32>(dx) >= kBookWidth || static_cast<U32>(dy) >= kBookHeight ||
            width > kBookWidth - static_cast<U32>(dx) || height > kBookHeight - static_cast<U32>(dy)) return false;
        const U8* encoded = nullptr;
        if (!Record(data, size, 2, Read16(image + 24), encoded, length) ||
            !DecodeTile(encoded, length, tile, 64 * 48)) return false;
        BookFrame& out = book.frames[i];
        for (U32 p = 0; p < kBookWidth * kBookHeight; ++p) out.pixels[p] = 0;
        for (U32 y = 0; y < height; ++y)
            for (U32 x = 0; x < width; ++x)
                out.pixels[(y + static_cast<U32>(dy)) * kBookWidth + x + static_cast<U32>(dx)] = tile[y * 64 + x];
        // 本版采用100ms/帧的视觉节拍，不伪称已接管原版鼠标动画时钟。
        out.durationMs = 100;
        book.durationMs += out.durationMs;
    }
    return true;
}
inline U32 BookFrameAt(const Book& book, U32 elapsed) {
    if (book.durationMs == 0) return 0;
    U32 time = elapsed % book.durationMs;
    for (U32 i = 0; i < kBookFrames; ++i) {
        if (time < book.frames[i].durationMs) return i;
        time -= book.frames[i].durationMs;
    }
    return 0;
}

// 菜单手形来自原版MouseManager当前SF2，不把鼠标重新画一次（那会推进动画）。
// 这里只解码一个小型单层图像用于遮挡，支持至多2×2个tile，未知格式直接拒绝。
struct CursorFrame {
    U32 pixels[96 * 96];
    int anchorX, anchorY;
    U32 width, height;
};
inline bool DecodeCursorFrame(const U8* data, U32 size, U32 frameIndex, CursorFrame& out) {
    if (!data || size < 0x42BC || Read32(data) != 0x05324653u || data[11] != 0 ||
        Read16(data + 7) != 64 || Read16(data + 9) != 48) return false;
    const U8* frame = nullptr; U32 length = 0;
    if (!Record(data, size, 0, frameIndex, frame, length) || length != 123 || Read16(frame + 84) != 1) return false;
    out.anchorX = static_cast<short>(Read16(frame + 96));
    out.anchorY = static_cast<short>(Read16(frame + 98));
    const U8* image = nullptr;
    if (!Record(data, size, 1, Read16(frame + 94), image, length) || length < 26) return false;
    const U32 columns = Read16(image), rows = Read16(image + 2);
    out.width = Read32(image + 4); out.height = Read32(image + 8);
    if (!columns || columns > 2 || !rows || rows > 2 || length != 24 + columns * rows * 2 ||
        !out.width || out.width > 96 || out.width > columns * 64 ||
        !out.height || out.height > 96 || out.height > rows * 48) return false;
    U32 tile[64 * 48];
    for (U32 i = 0; i < 96 * 96; ++i) out.pixels[i] = 0;
    for (U32 ty = 0; ty < rows; ++ty) for (U32 tx = 0; tx < columns; ++tx) {
        const U8* encoded = nullptr;
        if (!Record(data, size, 2, Read16(image + 24 + (ty * columns + tx) * 2), encoded, length) ||
            !DecodeTile(encoded, length, tile, 64 * 48)) return false;
        for (U32 y = 0; y < 48 && ty * 48 + y < out.height; ++y)
            for (U32 x = 0; x < 64 && tx * 64 + x < out.width; ++x)
                out.pixels[(ty * 48 + y) * 96 + tx * 64 + x] = tile[y * 64 + x];
    }
    return true;
}
}
#endif
