#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 状态表现路由调查器（固化46）。

这个工具只读取用户提供的 RPG.exe 与已经从 ItfDir.DAT 提取出的 ITF0052.SF2，
不会修改 EXE、不会修改资源，也不会向游戏进程注入任何代码。

工具要回答的是一个非常具体的结构问题：

1. Battle UI 是否真的创建了 14 个独立的“状态表现槽位”；
2. 这 14 个槽位是否全部加载同一份 Fight\\ITF\\ITF0052.SF2；
3. RoleDefinition 的哪些运行时 DWORD 会把哪一个 selector code 送进哪一个表现槽；
4. ITF0052.SF2 是否真的提供 selector 0..22 所需的 23 个 Section0 条目；
5. 这些 23 个条目是否一一映射到 23 个 Section1 / Section2 图像记录。

【固化47补注】本工具原固化46版本把 0x46A668 简写成 raw ID -> runtime slot。
当前版本已纠正为 raw ID -> counterpart runtime slot（对向/抵消槽）；数值读取算法未变。

注意：工具只证明“字段 -> 表现槽 -> selector -> ITF0052 条目”的机器结构。
它不会凭图标长相把 selector 强行命名成“中毒、睡眠、麻痹”等作者业务名。
"""

import argparse
import hashlib
import json
import struct
import zlib
from pathlib import Path
from typing import Dict, List, Tuple


TOOL_VERSION = "0.7D-solid47.1"

# RPG.exe 是 32 位 PE。下面这些 VA 都来自固化46重新核对过的当前同版代码。
# 这里保存的是“虚拟地址”，真正读取文件时还会通过 PE section 表换算成文件偏移。
VA_ITF0052_PATH = 0x004699C4
VA_SELECTOR_SETTER = 0x0043E6F0
VA_SELECTOR_CLAMP = 0x0043E720
VA_STATUS_UI_CONSTRUCTOR = 0x0041D160
VA_STATUS_UI_UPDATE_BEGIN = 0x0041CD79
VA_STATUS_UI_UPDATE_END = 0x0041D119

# 【固化47语义纠正】0x46A668 不是 normal effect 的“自身目的runtime槽”映射表。
# 0x42B53A 明确用 raw effect ID 访问这里的第一DWORD，然后读取该编号的
# RoleDefinition runtime槽并执行直接减法/残量运算。因此这里保存的是“对向/抵消槽索引”。
# normal effect 自身仍按 raw ID 0..27 进入同编号 pending/runtime 协议。
# 每项 8 字节：第一DWORD是 counterpart_index；第二DWORD当前为0但业务含义仍未命名。
VA_RAW_ID_TO_COUNTERPART_SLOT_TABLE = 0x0046A668
RAW_ID_COUNT = 28
RAW_ID_MAP_STRIDE = 8

# ITF0052 的标准 SF2 固定头大小。这个值不是“猜出来的常量”，而是标准 SF2 头 +0x05 的实值。
SF2_FIXED_HEADER_SIZE = 0x42BC
SF2_MAGIC = b"SF2\x05"

# 14个 Battle 状态表现槽的完整路由表。
# 每一项都来自 0x41CD79..0x41D114 的直接机器码，而不是从图标外观反推。
# field_offset 是 RoleDefinition 内偏移；child_offset 是状态 UI 子对象内的指针槽偏移。
# routes 里一个表现槽可以有一个或两个优先级分支；若所有字段都 <=0，原版送 selector 0。
PRESENTATION_CHANNELS = [
    {"channel": 0,  "child_offset": 0x598, "routes": [(0x1E8, 1),  (0x23C, 12)]},
    {"channel": 1,  "child_offset": 0x59C, "routes": [(0x1FC, 2),  (0x248, 3)]},
    {"channel": 2,  "child_offset": 0x5A0, "routes": [(0x1F4, 7),  (0x240, 6)]},
    {"channel": 3,  "child_offset": 0x5A4, "routes": [(0x1F8, 17), (0x244, 4)]},
    {"channel": 4,  "child_offset": 0x5A8, "routes": [(0x200, 14), (0x24C, 13)]},
    {"channel": 5,  "child_offset": 0x5AC, "routes": [(0x204, 10), (0x250, 11)]},
    {"channel": 6,  "child_offset": 0x5B0, "routes": [(0x1EC, 5)]},
    {"channel": 7,  "child_offset": 0x5B4, "routes": [(0x1F0, 18)]},
    {"channel": 8,  "child_offset": 0x5B8, "routes": [(0x224, 8),  (0x210, 19)]},
    {"channel": 9,  "child_offset": 0x5BC, "routes": [(0x214, 20)]},
    {"channel": 10, "child_offset": 0x5C0, "routes": [(0x21C, 21)]},
    {"channel": 11, "child_offset": 0x5C4, "routes": [(0x220, 9)]},
    {"channel": 12, "child_offset": 0x5C8, "routes": [(0x228, 16)]},
    {"channel": 13, "child_offset": 0x5CC, "routes": [(0x20C, 22)]},
]

# 每一个非零分支的机器码入口地址。
# 这些地址让工具能够检查“文档里的 field/selector 表”有没有被人工抄错。
# entry_va 指向 mov eax,[esi+field]；push_va 指向真正压入 selector 常量的 push 指令。
ROUTE_MACHINE_POINTS = [
    (0x41CD79, 0x1E8, 0x41CD89, 1),   (0x41CD96, 0x23C, 0x41CDA6, 12),
    (0x41CDC9, 0x1FC, 0x41CDD9, 2),   (0x41CDE6, 0x248, 0x41CDF6, 3),
    (0x41CE19, 0x1F4, 0x41CE29, 7),   (0x41CE36, 0x240, 0x41CE46, 6),
    (0x41CE69, 0x1F8, 0x41CE79, 17),  (0x41CE86, 0x244, 0x41CE96, 4),
    (0x41CEB9, 0x200, 0x41CEC9, 14),  (0x41CED6, 0x24C, 0x41CEE6, 13),
    (0x41CF09, 0x204, 0x41CF19, 10),  (0x41CF26, 0x250, 0x41CF36, 11),
    (0x41CF59, 0x1EC, 0x41CF69, 5),   (0x41CF8C, 0x1F0, 0x41CF9C, 18),
    (0x41CFBF, 0x224, 0x41CFCF, 8),   (0x41CFDC, 0x210, 0x41CFEC, 19),
    (0x41D00F, 0x214, 0x41D01F, 20),  (0x41D042, 0x21C, 0x41D052, 21),
    (0x41D075, 0x220, 0x41D085, 9),   (0x41D0A8, 0x228, 0x41D0B8, 16),
    (0x41D0DB, 0x20C, 0x41D0EB, 22),
]


# 下面几个小函数只做“按小端读整数”。
# 把它们单独写出来，是为了避免后面每次 struct.unpack_from 都重复一长串格式字符串。
def u16(data: bytes, offset: int) -> int:
    return struct.unpack_from("<H", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<i", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def sha256_bytes(data: bytes) -> str:
    """计算一段字节的 SHA-256；报告里用它确认输入是不是同一份内容。"""
    return hashlib.sha256(data).hexdigest()


class PeImage:
    """非常小的只读 PE 映射器，只实现本调查需要的 VA -> 文件偏移换算。"""

    def __init__(self, data: bytes):
        # 先保存整个 EXE 字节。后面所有机器码断言都只从这个 bytes 对象读取，不会写回磁盘。
        self.data = data

        # DOS 头最开始必须是 MZ；如果连这个都不对，就不能把文件当作当前 Windows EXE。
        if data[:2] != b"MZ":
            raise ValueError("输入文件不是 MZ/PE 可执行文件")

        # DOS 头 +0x3C 保存 PE Header 的文件偏移。
        pe_offset = u32(data, 0x3C)
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("找不到 PE\\0\\0 签名")

        # COFF File Header 紧跟 PE 签名。+2 是 section 数，+16 是 Optional Header 大小。
        coff = pe_offset + 4
        section_count = u16(data, coff + 2)
        optional_size = u16(data, coff + 16)

        # 当前 RPG.exe 是 PE32。Optional Header 的 magic 应为 0x10B。
        optional = coff + 20
        if u16(data, optional) != 0x10B:
            raise ValueError("当前工具只接受 PE32（OptionalHeader.Magic=0x10B）")

        # PE32 Optional Header +0x1C 是 ImageBase。VA 换算时必须先减掉它得到 RVA。
        self.image_base = u32(data, optional + 0x1C)

        # Section table 位于 Optional Header 之后。每个 section header 固定 40 字节。
        section_table = optional + optional_size
        self.sections = []
        for index in range(section_count):
            base = section_table + index * 40
            name = data[base:base + 8].split(b"\0", 1)[0].decode("ascii", "replace")
            virtual_size = u32(data, base + 8)
            virtual_address = u32(data, base + 12)
            raw_size = u32(data, base + 16)
            raw_offset = u32(data, base + 20)
            self.sections.append({
                "name": name,
                "virtual_size": virtual_size,
                "virtual_address": virtual_address,
                "raw_size": raw_size,
                "raw_offset": raw_offset,
            })

    def va_to_offset(self, va: int) -> int:
        # VA = ImageBase + RVA，所以先把绝对虚拟地址换成 RVA。
        rva = va - self.image_base
        for section in self.sections:
            start = section["virtual_address"]
            # 映像在内存里的有效范围可能比磁盘 raw_size 大，也可能相反；取二者较大值用于判断所属 section。
            span = max(section["virtual_size"], section["raw_size"])
            if start <= rva < start + span:
                delta = rva - start
                if delta >= section["raw_size"]:
                    raise ValueError(f"VA 0x{va:08X} 落在 section 的零填充区，磁盘上没有对应字节")
                return section["raw_offset"] + delta
        raise ValueError(f"VA 0x{va:08X} 不属于任何 PE section")

    def read(self, va: int, size: int) -> bytes:
        # 通过上面的地址换算找到真实文件偏移，然后返回只读切片。
        offset = self.va_to_offset(va)
        chunk = self.data[offset:offset + size]
        if len(chunk) != size:
            raise ValueError(f"读取 VA 0x{va:08X} 时超出文件")
        return chunk


# x86 的 `push imm8` 有两种常见编码：6A xx；较大的常量也可以是 68 dword。
# 本专题 selector 全在0..22，原版这里全部使用 6A imm8，所以可以严格检查两个字节。
def assert_push_imm8(pe: PeImage, va: int, value: int) -> Dict[str, object]:
    actual = pe.read(va, 2)
    expected = bytes((0x6A, value & 0xFF))
    return {
        "VA": f"0x{va:08X}",
        "期望": expected.hex(" "),
        "实际": actual.hex(" "),
        "PASS": actual == expected,
    }


def assert_role_field_read(pe: PeImage, va: int, field_offset: int) -> Dict[str, object]:
    # 当前21个条件入口都使用同一个模板：8B 86 <disp32>，也就是 mov eax,[esi+field_offset]。
    expected = b"\x8B\x86" + struct.pack("<I", field_offset)
    actual = pe.read(va, len(expected))
    return {
        "VA": f"0x{va:08X}",
        "字段": f"+0x{field_offset:03X}",
        "期望": expected.hex(" "),
        "实际": actual.hex(" "),
        "PASS": actual == expected,
    }


def scan_direct_e8_calls(pe: PeImage, start_va: int, end_va: int, target_va: int) -> List[int]:
    # 这里只扫描一个已经人工限定好的函数区间，寻找 E8 rel32 直接调用。
    # x86 E8 的目标 = 下一条指令地址 + signed rel32。
    data = pe.read(start_va, end_va - start_va)
    calls = []
    for i in range(0, len(data) - 4):
        if data[i] != 0xE8:
            continue
        rel = struct.unpack_from("<i", data, i + 1)[0]
        call_va = start_va + i
        resolved = call_va + 5 + rel
        if resolved == target_va:
            calls.append(call_va)
    return calls


def verify_constructor_machine_code(pe: PeImage) -> List[Dict[str, object]]:
    """验证14个ITF0052子对象的两轮7项构造循环与selector夹取器。"""
    checks = []

    # 这些片段刻意选在最能说明结构的地方，不要求把整个函数几百字节都硬编码进工具。
    machine_assertions = [
        # 第一排：child base = +0x598；x从0x10开始；资源路径固定0x4699C4；y=0xF9；x每次+0x1E；x==0xE2停止。
        (0x41D2F3, "33 ff c7 44 24 10 10 00 00 00 8d ae 98 05 00 00"),
        (0x41D32A, "8b 4c 24 10 53 68 c4 99 46 00 68 f9 00 00 00 51"),
        (0x41D348, "8b 55 00 47 57 8b 4a 1c e8 9b 13 02 00"),
        (0x41D355, "8b 44 24 10 83 c5 04 83 c0 1e 3d e2 00 00 00"),
        # 第二排：从上轮edi=7继续，child从+0x5B4开始；selector从8开始；资源仍ITF0052；y=0x116。
        (0x41D36A, "8d 47 01 bd 10 00 00 00 89 44 24 10 8d bc be 98 05 00 00"),
        (0x41D3A4, "53 68 c4 99 46 00 68 16 01 00 00 55"),
        (0x41D3BD, "8b 07 8b 54 24 10 52 8b 48 1c e8 24 13 02 00"),
        (0x41D3CC, "8b 54 24 10 83 c7 04 83 c5 1e 42 81 fd e2 00 00 00"),
        # selector setter 先写 object+0x10，再调用0x43E720夹取到合法范围。
        (0x43E6FA, "8b 4c 24 08 85 c9 7c 0d 8d 46 10 50 89 08 8b ce e8 11 00 00 00"),
        # clamp上界是 object+0x30 的“开区间末端”，超出时写成 end-1；下界是 object+0x2C。
        (0x43E720, "8b 44 24 04 8b 51 30 56 8b 30 3b f2 5e 7c 03 4a 89 10"),
        (0x43E732, "8b 49 2c 8b 10 3b d1 7d 02 89 08"),
    ]

    for va, expected_hex in machine_assertions:
        expected = bytes.fromhex(expected_hex)
        actual = pe.read(va, len(expected))
        checks.append({
            "VA": f"0x{va:08X}",
            "期望": expected.hex(" "),
            "实际": actual.hex(" "),
            "PASS": actual == expected,
        })

    return checks


def read_raw_id_counterpart_mapping(pe: PeImage) -> List[Dict[str, int]]:
    # 直接从当前 EXE 的 0x46A668 读取28条“对向槽”关系。
    # 这里故意不再使用 mapped_index 这种模糊名字，因为固化47已经证明它不是自身目的槽。
    rows = []
    for raw_id in range(RAW_ID_COUNT):
        base = VA_RAW_ID_TO_COUNTERPART_SLOT_TABLE + raw_id * RAW_ID_MAP_STRIDE
        counterpart_index = s32(pe.read(base, 4), 0)
        second_dword = s32(pe.read(base + 4, 4), 0)
        rows.append({
            "raw_id": raw_id,
            "counterpart_index": counterpart_index,
            "second_dword": second_dword,
        })
    return rows


def add_counterpart_backlinks(channels: List[Dict[str, object]], counterpart_map: List[Dict[str, int]]) -> None:
    # 构造“某runtime槽被哪些raw ID当作对向槽”的反向表。
    # 这与“某raw ID最终写入哪个自身runtime槽”是两个不同概念，不能再混为一谈。
    reverse: Dict[int, List[int]] = {}
    for row in counterpart_map:
        if row["counterpart_index"] >= 0:
            reverse.setdefault(row["counterpart_index"], []).append(row["raw_id"])

    for channel in channels:
        for route in channel["routes"]:
            runtime_index = (route["field_offset"] - 0x1E8) // 4
            route["runtime_index"] = runtime_index
            route["raw_ids_whose_counterpart_is_this_slot"] = reverse.get(runtime_index, [])


def decompress_sf2(raw: bytes) -> Tuple[bytes, Dict[str, object]]:
    # 标准SF2在0x0B可以是0，或0x6F表示固定头后面是zlib包装。
    if len(raw) < SF2_FIXED_HEADER_SIZE or raw[:4] != SF2_MAGIC:
        raise ValueError("ITF0052 输入不是当前已知标准 SF2")

    meta = {
        "raw_size": len(raw),
        "compression_flag_0x0B": raw[0x0B],
        "compressed_wrapper": raw[0x0B] == 0x6F,
    }

    if raw[0x0B] != 0x6F:
        return raw, meta

    # 0x6F包装在固定头之后有两个相同的最终大小DWORD，然后才是zlib流。
    declared_a = u32(raw, SF2_FIXED_HEADER_SIZE)
    declared_b = u32(raw, SF2_FIXED_HEADER_SIZE + 4)
    if declared_a != declared_b:
        raise ValueError("SF2 0x6F包装的两个最终大小字段不一致")

    tail = zlib.decompress(raw[SF2_FIXED_HEADER_SIZE + 8:])
    rebuilt = bytearray(raw[:SF2_FIXED_HEADER_SIZE] + tail)
    rebuilt[0x0B] = 0
    meta.update({
        "declared_final_size": declared_a,
        "rebuilt_size": len(rebuilt),
        "size_match": len(rebuilt) == declared_a,
    })
    return bytes(rebuilt), meta


def parse_sections(sf2: bytes) -> List[Dict[str, object]]:
    # SF2固定头+0x14开始有4个10字节SectionDescriptor。
    # 每个descriptor = u16 count + u32 section_offset + u32 section_size。
    sections = []
    for index in range(4):
        h = 0x14 + index * 10
        count = u16(sf2, h)
        section_offset = u32(sf2, h + 2)
        section_size = u32(sf2, h + 6)
        end = section_offset + section_size
        if end > len(sf2):
            raise ValueError(f"Section{index} 超出文件边界")

        records = []
        if count:
            # Section开头不是记录本体，而是 count 个绝对offset DWORD。
            if section_offset + count * 4 > end:
                raise ValueError(f"Section{index} offset表超出Section边界")
            starts = [u32(sf2, section_offset + i * 4) for i in range(count)]
            ends = starts[1:] + [end]
            for rec_index, (start, rec_end) in enumerate(zip(starts, ends)):
                if start < section_offset + count * 4 or rec_end < start or rec_end > end:
                    raise ValueError(f"Section{index} record{rec_index} 边界非法")
                records.append({"index": rec_index, "start": start, "end": rec_end})

        sections.append({
            "index": index,
            "count": count,
            "offset": section_offset,
            "size": section_size,
            "records": records,
        })
    return sections


def decode_rgb555(value: int) -> Tuple[int, int, int, int]:
    # 当前ITF0052走标准16位RGB555类像素。每个5-bit通道扩展到0..255只用于证据PNG，不改变原始数据。
    r = (value >> 10) & 0x1F
    g = (value >> 5) & 0x1F
    b = value & 0x1F
    return (r * 255 // 31, g * 255 // 31, b * 255 // 31, 255)


def decode_tile_rle(record: bytes, tile_width: int, tile_height: int) -> List[Tuple[int, int, int, int]]:
    # 这是固化B阶段已经闭合的Section2 RLE：
    # bit6=0 -> literal；bit6=1且bit7=1 -> repeat；bit6=1且bit7=0 -> transparent skip。
    total = tile_width * tile_height
    pixels: List[Tuple[int, int, int, int]] = []
    cursor = 0

    while len(pixels) < total:
        if cursor >= len(record):
            raise ValueError("Section2 RLE提前结束")
        control = record[cursor]
        cursor += 1
        run = (control & 0x3F) + 1

        if (control & 0x40) == 0:
            for _ in range(run):
                if cursor + 2 > len(record):
                    raise ValueError("Section2 literal像素越过记录末尾")
                value = u16(record, cursor)
                cursor += 2
                pixels.append(decode_rgb555(value))
        elif control & 0x80:
            if cursor + 2 > len(record):
                raise ValueError("Section2 repeat像素越过记录末尾")
            value = u16(record, cursor)
            cursor += 2
            pixels.extend([decode_rgb555(value)] * run)
        else:
            pixels.extend([(0, 0, 0, 0)] * run)

        if len(pixels) > total:
            raise ValueError("Section2 RLE解码像素数超过tile尺寸")

    if cursor != len(record):
        raise ValueError("Section2 RLE已经填满tile但记录尾部仍有未消费字节")
    return pixels


def png_chunk(kind: bytes, payload: bytes) -> bytes:
    # PNG chunk = length + fourcc + payload + CRC32(fourcc+payload)。
    import binascii
    body = kind + payload
    return struct.pack(">I", len(payload)) + body + struct.pack(">I", binascii.crc32(body) & 0xFFFFFFFF)


def write_rgba_png(path: Path, width: int, height: int, pixels: List[Tuple[int, int, int, int]]) -> None:
    # 为了让工具保持纯标准库，不依赖Pillow，这里手工写最基础的RGBA PNG。
    if len(pixels) != width * height:
        raise ValueError("PNG像素数量与宽高不匹配")

    raw = bytearray()
    for y in range(height):
        # 每一行最前面的0表示PNG filter type 0：本行字节原样保存。
        raw.append(0)
        row = pixels[y * width:(y + 1) * width]
        for r, g, b, a in row:
            raw.extend((r, g, b, a))

    png = bytearray(b"\x89PNG\r\n\x1a\n")
    png += png_chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
    png += png_chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    png += png_chunk(b"IEND", b"")
    path.write_bytes(bytes(png))


# 3×5像素的小数字，用来给证据联系表标上0..22索引。
# 我们自己画数字，可以继续保持“零第三方依赖”。
DIGITS = {
    "0": ["111", "101", "101", "101", "111"],
    "1": ["010", "110", "010", "010", "111"],
    "2": ["111", "001", "111", "100", "111"],
    "3": ["111", "001", "111", "001", "111"],
    "4": ["101", "101", "111", "001", "001"],
    "5": ["111", "100", "111", "001", "111"],
    "6": ["111", "100", "111", "101", "111"],
    "7": ["111", "001", "001", "001", "001"],
    "8": ["111", "101", "111", "101", "111"],
    "9": ["111", "101", "111", "001", "111"],
}


def draw_index(pixels: List[Tuple[int, int, int, int]], width: int, x: int, y: int, text: str, scale: int = 2) -> None:
    # 直接在RGBA列表上画黑色数字；这里只用于证据图索引，不参与任何格式判断。
    cursor_x = x
    for ch in text:
        pattern = DIGITS[ch]
        for py, line in enumerate(pattern):
            for px, bit in enumerate(line):
                if bit != "1":
                    continue
                for sy in range(scale):
                    for sx in range(scale):
                        xx = cursor_x + px * scale + sx
                        yy = y + py * scale + sy
                        if 0 <= xx < width and 0 <= yy < len(pixels) // width:
                            pixels[yy * width + xx] = (0, 0, 0, 255)
        cursor_x += 4 * scale


def inspect_itf0052(sf2_path: Path, contact_png: Path = None) -> Dict[str, object]:
    raw = sf2_path.read_bytes()
    sf2, compression = decompress_sf2(raw)
    sections = parse_sections(sf2)

    tile_width = u16(sf2, 0x07)
    tile_height = u16(sf2, 0x09)
    counts = [section["count"] for section in sections]

    # 固化46真正需要的核心是23/23/23/0。
    # 这使 selector 0..22 拥有完整的23个合法Section0条目。
    count_shape_ok = counts == [23, 23, 23, 0]

    section0_rows = []
    section1_rows = []
    tile_pixels = []
    one_to_one_ok = True

    for index in range(23):
        s0_meta = sections[0]["records"][index]
        s0 = sf2[s0_meta["start"]:s0_meta["end"]]
        if len(s0) < 0x5E:
            raise ValueError(f"Section0[{index}]短于0x5E")
        layer_count = u16(s0, 0x54)
        if layer_count != 1 or len(s0) != 0x5E + 29:
            one_to_one_ok = False
        layer = s0[0x5E:0x5E + 29]
        section1_index = u16(layer, 0)
        layer_x = struct.unpack_from("<h", layer, 2)[0]
        layer_y = struct.unpack_from("<h", layer, 4)[0]
        metadata_i32 = s32(layer, 6)
        render_mode = layer[0x0A]
        if section1_index != index:
            one_to_one_ok = False
        section0_rows.append({
            "selector": index,
            "record_size": len(s0),
            "layer_count": layer_count,
            "section1_index": section1_index,
            "layer_x": layer_x,
            "layer_y": layer_y,
            "serialized_metadata_i32_06": metadata_i32,
            "render_mode": render_mode,
        })

        s1_meta = sections[1]["records"][index]
        s1 = sf2[s1_meta["start"]:s1_meta["end"]]
        cols = u16(s1, 0)
        rows = u16(s1, 2)
        field_04 = u32(s1, 4)
        field_08 = u32(s1, 8)
        if cols != 1 or rows != 1 or len(s1) != 26:
            one_to_one_ok = False
        tile_index = u16(s1, 24)
        if tile_index != index:
            one_to_one_ok = False
        section1_rows.append({
            "selector": index,
            "record_size": len(s1),
            "cols": cols,
            "rows": rows,
            "field_u32_04": field_04,
            "field_u32_08": field_08,
            "section2_tile_index": tile_index,
        })

        s2_meta = sections[2]["records"][index]
        s2 = sf2[s2_meta["start"]:s2_meta["end"]]
        pixels = decode_tile_rle(s2, tile_width, tile_height)
        tile_pixels.append(pixels)

    if contact_png is not None:
        # 证据联系表排成5列×5行。每格上方留14像素写selector索引。
        cols = 5
        rows = 5
        cell_w = tile_width + 8
        cell_h = tile_height + 18
        sheet_w = cols * cell_w
        sheet_h = rows * cell_h
        sheet = [(236, 236, 236, 255)] * (sheet_w * sheet_h)

        for index, source in enumerate(tile_pixels):
            cell_x = (index % cols) * cell_w
            cell_y = (index // cols) * cell_h
            draw_index(sheet, sheet_w, cell_x + 2, cell_y + 2, str(index), 2)
            dst_x = cell_x + 4
            dst_y = cell_y + 16

            # 透明像素在证据图里用深灰底显示，非透明像素保持解码后的RGB555颜色。
            for y in range(tile_height):
                for x in range(tile_width):
                    r, g, b, a = source[y * tile_width + x]
                    if a == 0:
                        color = (80, 80, 80, 255)
                    else:
                        color = (r, g, b, 255)
                    sheet[(dst_y + y) * sheet_w + dst_x + x] = color

        write_rgba_png(contact_png, sheet_w, sheet_h, sheet)

    return {
        "path": str(sf2_path),
        "raw_sha256": sha256_bytes(raw),
        "raw_size": len(raw),
        "compression": compression,
        "tile_width": tile_width,
        "tile_height": tile_height,
        "section_counts": counts,
        "count_shape_23_23_23_0": count_shape_ok,
        "selector_valid_range_from_asset": [0, 22] if count_shape_ok else None,
        "section0_section1_section2_one_to_one": one_to_one_ok,
        "section0": section0_rows,
        "section1": section1_rows,
    }


def inspect_exe(exe_path: Path) -> Dict[str, object]:
    data = exe_path.read_bytes()
    pe = PeImage(data)

    # 第一步先确认路径常量本身。若这里失败，后面的“同一ITF0052资源”结论就不能继续成立。
    path_expected = b"Fight\\ITF\\ITF0052.SF2\0"
    path_actual = pe.read(VA_ITF0052_PATH, len(path_expected))
    path_check = {
        "VA": f"0x{VA_ITF0052_PATH:08X}",
        "expected_ascii": path_expected[:-1].decode("ascii"),
        "actual_hex": path_actual.hex(" "),
        "PASS": path_actual == path_expected,
    }

    route_checks = []
    for entry_va, field_offset, push_va, selector in ROUTE_MACHINE_POINTS:
        field_check = assert_role_field_read(pe, entry_va, field_offset)
        push_check = assert_push_imm8(pe, push_va, selector)
        route_checks.append({
            "field_offset": f"+0x{field_offset:03X}",
            "selector": selector,
            "field_read": field_check,
            "selector_push": push_check,
            "PASS": field_check["PASS"] and push_check["PASS"],
        })

    constructor_checks = verify_constructor_machine_code(pe)

    # 状态更新逻辑有14个表现槽，但机器码里会出现15条direct call指令。
    # 原因是前13个槽把“非零selector/零fallback”两条分支汇聚到同一个call；
    # 最后一个 +0x5CC 槽为了直接return，把真分支的call放在0x41D0F6、假分支的call放在0x41D114，
    # 所以“逻辑槽数14”与“E8指令数15”并不矛盾。这里把15条地址也冻结，防止以后误数。
    update_calls = scan_direct_e8_calls(pe, VA_STATUS_UI_UPDATE_BEGIN, VA_STATUS_UI_UPDATE_END, VA_SELECTOR_SETTER)
    expected_update_calls = [
        0x41CDC4, 0x41CE14, 0x41CE64, 0x41CEB4, 0x41CF04, 0x41CF54, 0x41CF87,
        0x41CFBA, 0x41D00A, 0x41D03D, 0x41D070, 0x41D0A3, 0x41D0D6, 0x41D0F6, 0x41D114,
    ]

    # 构造阶段有14个child，每个都会初始化一次selector；为了不靠复杂反汇编器，
    # 这里再直接验证两轮循环关键机器码，而不把“225个全EXE caller”混进本专题。
    counterpart_map = read_raw_id_counterpart_mapping(pe)

    channels = []
    for channel in PRESENTATION_CHANNELS:
        row = {
            "channel": channel["channel"],
            "child_offset": f"+0x{channel['child_offset']:03X}",
            "zero_fallback_selector": 0,
            "routes": [],
        }
        for field_offset, selector in channel["routes"]:
            row["routes"].append({
                "field_offset": field_offset,
                "field_offset_hex": f"+0x{field_offset:03X}",
                "selector": selector,
            })
        channels.append(row)
    add_counterpart_backlinks(channels, counterpart_map)

    all_machine_checks = [path_check]
    all_machine_checks.extend({"PASS": row["PASS"]} for row in route_checks)
    all_machine_checks.extend(constructor_checks)
    machine_pass = all(bool(row["PASS"]) for row in all_machine_checks)

    return {
        "path": str(exe_path),
        "sha256": sha256_bytes(data),
        "size": len(data),
        "image_base": f"0x{pe.image_base:08X}",
        "itf0052_path_check": path_check,
        "route_machine_checks": route_checks,
        "constructor_and_clamp_checks": constructor_checks,
        "status_update_direct_calls_to_0x43E6F0": [f"0x{x:08X}" for x in update_calls],
        "status_update_direct_call_count": len(update_calls),
        "status_update_expected_direct_calls": [f"0x{x:08X}" for x in expected_update_calls],
        "status_update_direct_call_set_pass": update_calls == expected_update_calls,
        "status_update_logical_channel_count": len(PRESENTATION_CHANNELS),
        "status_update_has_exactly_14_logical_channels": len(PRESENTATION_CHANNELS) == 14,
        "raw_id_to_counterpart_slot_0x46A668": counterpart_map,
        "presentation_channels": channels,
        "machine_checks_pass": machine_pass and update_calls == expected_update_calls and len(PRESENTATION_CHANNELS) == 14,
    }


def main() -> None:
    parser = argparse.ArgumentParser(
        description="《幽城幻剑录》固化46：Battle十四路状态表现槽与ITF0052二十三序列路由只读调查器"
    )
    parser.add_argument("--exe", required=True, help="RPG.exe 或 RPG.exe.org 解出的32位EXE路径")
    parser.add_argument("--sf2", required=True, help="从 ItfDir.DAT 提取出的 ITF0052.SF2 路径")
    parser.add_argument("--json", help="可选：把完整机器/资源证据写成UTF-8 JSON")
    parser.add_argument("--png", help="可选：输出23个selector图像的联系表PNG（纯标准库生成）")
    args = parser.parse_args()

    exe_path = Path(args.exe)
    sf2_path = Path(args.sf2)
    if not exe_path.is_file():
        raise SystemExit(f"EXE不存在：{exe_path}")
    if not sf2_path.is_file():
        raise SystemExit(f"SF2不存在：{sf2_path}")

    png_path = Path(args.png) if args.png else None
    if png_path is not None:
        png_path.parent.mkdir(parents=True, exist_ok=True)

    exe_result = inspect_exe(exe_path)
    sf2_result = inspect_itf0052(sf2_path, png_path)

    # 最终PASS条件故意只包含本节点已经有直接证据的结构事实。
    # “作者业务名”没有证据，所以不会被偷偷塞进PASS条件。
    passed = (
        exe_result["machine_checks_pass"]
        and sf2_result["count_shape_23_23_23_0"]
        and sf2_result["section0_section1_section2_one_to_one"]
        and sf2_result["tile_width"] == 64
        and sf2_result["tile_height"] == 48
    )

    result = {
        "tool_version": TOOL_VERSION,
        "topic": "Battle十四路状态表现槽与ITF0052二十三序列路由",
        "result": "PASS" if passed else "FAIL",
        "exe": exe_result,
        "itf0052": sf2_result,
        "frozen_boundary": [
            "Battle状态UI构造器创建14个独立表现child：两排各7个。",
            "14个child全部加载Fight\\ITF\\ITF0052.SF2；第一排坐标x=16+30*n,y=249，第二排x=16+30*n,y=278。",
            "状态更新函数有14个逻辑child槽；机器码共有15条0x43E6F0 direct call，因为最后一个+0x5CC槽的真/假分支各自直接call后return，其余13槽在分支汇聚点call；每路无条件命中时回退selector 0。",
            "ITF0052.SF2严格包含23个Section0/Section1/Section2记录、0个Section3；selector合法资产索引为0..22。",
            "ITF0052的23个Section0均只有1个Layer，并严格一一引用同编号Section1；23个Section1均为1x1并严格一一引用同编号Section2。",
            "21个非零selector分支的RoleDefinition字段与selector常量均由机器码逐项复核；selector 15不在0x41CD79..0x41D114这14路update mux中出现，但不得据此宣称全程序未使用。",
            "【固化47纠正】0x46A668反链表示哪些raw ID把该runtime槽当作对向/抵消槽，不表示raw ID自身最终写入该槽；作者业务名仍需独立证据。",
        ],
        "forbidden_overreach": [
            "不得只凭联系表图标外观把selector或runtime slot命名为中毒、睡眠、麻痹、石化等作者状态名。",
            "不得把selector code直接叫frame；这里证明的是ITF0052 Section0 action/state entry索引。",
            "不得把selector 15写成unused/dead；本节点只证明它不由这一段14路update mux选择。",
        ],
    }

    text = json.dumps(result, ensure_ascii=False, indent=2)
    if args.json:
        out = Path(args.json)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(text + "\n", encoding="utf-8", newline="\n")
    else:
        print(text)

    if not passed:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
