#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》SAF 异格式静态调查器
版本：0.1.0-固化35

这个工具只做“读取、验证、导出研究结果”，不会修改原游戏 EXE，也不会改写原始 SAF/SF2 资源。

固化35已经确认的用途：
1. 读取 ItfDir.DAT 中唯一已知的 `ITF000.SF2` / `SAF\x05` 样本。
2. 还原它共用的 `0x6F + 双最终尺寸 + zlib` 外层包装。
3. 按 SAF 自己的逻辑头 `0x74` 和从 `0x0C` 开始的四段描述表解析记录。
4. 验证第1段的 `4 + 2*列*行` tilemap 公式。
5. 用标准 SF2 同型的 RGB16 RLE 算法逐 tile 解码第2段，并要求每条记录“像素数刚好、输入字节也刚好”双重精确结束。
6. 可把第1段 tilemap 与第2段 tile 合成为透明 PNG，作为“结构确实能重建成图像”的可视化证据。
7. 对 RPG.exe 重放固化35静态边界：ItfDir.DAT 挂载、标准 SF2 魔数门、标准 SF2 四段描述表字段、canonical RLE decoder 的全部直接 E8 caller，以及动态 Fight 路径格式化证据。

依赖：仅 Python 3 标准库。
"""

# `argparse` 用来读取命令行参数，例如“检查SAF 文件名 --json 输出.json”。
import argparse
# `hashlib` 只用于计算 SHA-256，方便确认研究时使用的文件是否还是同一份。
import hashlib
# `json` 用来把结构化研究结果写成人可以查看、程序也可以复查的 JSON。
import json
# `struct` 用来按“小端序”读取 EXE/资源里的 16 位和 32 位整数。
import struct
# `zlib` 一方面用于解开 SAF 的压缩尾部，另一方面也用于写 PNG 的 IDAT 数据。
import zlib
# `Counter` 用来统计 RLE 三类 run 的数量，让报告能看出样本的编码组成。
from collections import Counter
# `Path` 让 Windows/Linux 路径都能用统一方法读取，不需要手工拼接斜杠。
from pathlib import Path
# 类型提示不会改变运行逻辑，但能让后续维护者更容易看懂“这里应该传什么、返回什么”。
from typing import Any, Dict, List, Optional, Sequence, Tuple

# 工具版本写入每份 JSON，未来拿到单独一份报告时仍能知道它是由哪版工具生成的。
TOOL_VERSION = "0.1.0-固化35"
# 当前唯一已知 SAF 样本的魔数。前三字节是 ASCII “SAF”，第4字节是版本/格式字节 0x05。
SAF_MAGIC = b"SAF\x05"
# SAF 自己声明的逻辑头大小。注意它不是外层压缩切分位置，两者在这个样本里是不同概念。
SAF_LOGICAL_HEADER_SIZE = 0x74
# SAF 的四段描述表从 0x0C 开始，每个描述项固定 10 字节：u16 count + u32 offset + u32 size。
SAF_SECTION_TABLE_OFFSET = 0x0C
# 当前样本的 0x6F 包装仍在 0x42BC 处分割前缀与压缩尾；这个位置与标准 SF2 固定头大小相同。
# 固化35只把它登记为“外层包装切分位置”，绝不能误叫成 SAF 逻辑 Header 大小。
LEGACY_COMPRESSED_TAIL_SPLIT = 0x42BC
# 当前用户给出的 RPG.exe.org SHA-256。整文件不是历史 canonical Oracle，但已覆盖代码区通过既有回归。
SOLID34_COMPATIBLE_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"


def read_u16(data: bytes, offset: int) -> int:
    """从 `offset` 开始按小端序读取 2 字节无符号整数。"""
    # 先检查边界，避免损坏/截断文件让 `struct` 抛出难懂的底层错误。
    if offset < 0 or offset + 2 > len(data):
        raise ValueError(f"读取 u16 越界：offset=0x{offset:X}, size={len(data)}")
    # `<H` 的 `<` 表示小端序，`H` 表示 16 位无符号整数。
    return struct.unpack_from("<H", data, offset)[0]


def read_u32(data: bytes, offset: int) -> int:
    """从 `offset` 开始按小端序读取 4 字节无符号整数。"""
    # 和 read_u16 一样先做显式边界判断，让失败原因直接对应到资源结构。
    if offset < 0 or offset + 4 > len(data):
        raise ValueError(f"读取 u32 越界：offset=0x{offset:X}, size={len(data)}")
    # `<I` 表示小端序 32 位无符号整数。
    return struct.unpack_from("<I", data, offset)[0]


def sha256_bytes(data: bytes) -> str:
    """返回一段字节的 SHA-256 十六进制字符串。"""
    # 哈希只读取数据，不会修改输入；hexdigest() 把 32 字节哈希转成人能复制核对的 64 位十六进制文本。
    return hashlib.sha256(data).hexdigest()


def write_json(path: Path, value: Any) -> None:
    """以 UTF-8 写出易读 JSON。"""
    # 如果用户指定了尚不存在的子目录，先创建目录，避免因为目录缺失而让研究命令失败。
    path.parent.mkdir(parents=True, exist_ok=True)
    # ensure_ascii=False 保留中文；indent=2 让层级清楚；末尾加换行方便 Git/文本工具处理。
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def rebuild_saf(raw: bytes) -> Tuple[bytes, Dict[str, Any]]:
    """还原 SAF 的 0x6F 外层包装，并返回“逻辑文件字节 + 包装元数据”。"""
    # 最前面必须至少容纳 SAF 基本字段，否则连魔数和 0x0B 包装标志都无法安全读取。
    if len(raw) < 0x0C:
        raise ValueError("文件太短，无法容纳 SAF 基本头字段")
    # 固化35只解析已有正样本证明的 SAF\x05；未知版本不能假装兼容。
    if raw[:4] != SAF_MAGIC:
        raise ValueError(f"不是当前已确认的 SAF\\x05：实际魔数={raw[:4].hex(' ')}")

    # 0x0B 在压缩文件里是 0x6F；重建后的逻辑数据会把它恢复为 0，和标准 SF2 包装行为一致。
    compression_flag = raw[0x0B]
    # 先建立公共元数据；无论是否压缩都能报告原始文件状态。
    meta: Dict[str, Any] = {
        "原始大小": len(raw),
        "原始压缩标志_0x0B": compression_flag,
        "使用0x6F外层包装": compression_flag == 0x6F,
        "外层包装切分位置": LEGACY_COMPRESSED_TAIL_SPLIT,
    }

    # 如果不是 0x6F，就把输入直接视为逻辑 SAF；这样未来拿到未压缩 SAF 时也能复用同一解析器。
    if compression_flag != 0x6F:
        meta.update({
            "声明最终大小1": None,
            "声明最终大小2": None,
            "zlib尾解压大小": None,
            "重建大小": len(raw),
            "重建大小匹配": True,
        })
        return raw, meta

    # 0x6F 包装要求在 0x42BC 后至少还有两个 u32 尺寸；不足说明文件被截断。
    if len(raw) < LEGACY_COMPRESSED_TAIL_SPLIT + 8:
        raise ValueError("0x6F SAF 包装不足以容纳 +0x42BC/+0x42C0 两个最终尺寸")
    # 第一个最终尺寸位于 0x42BC。
    final_size_1 = read_u32(raw, LEGACY_COMPRESSED_TAIL_SPLIT)
    # 第二个最终尺寸紧跟其后，位于 0x42C0。
    final_size_2 = read_u32(raw, LEGACY_COMPRESSED_TAIL_SPLIT + 4)
    # 原版标准 SF2 链也要求两个尺寸相等；当前 SAF 正样本同样满足，因此这里严格验证。
    if final_size_1 != final_size_2:
        raise ValueError(f"SAF 0x6F 双最终尺寸不一致：{final_size_1} != {final_size_2}")

    # 真正 zlib 流从 0x42C4 开始，即“固定前缀 + 两个尺寸”之后。
    compressed_tail = raw[LEGACY_COMPRESSED_TAIL_SPLIT + 8:]
    # zlib.decompress 会校验压缩流本身；损坏数据会在这里明确失败，而不是产生半截图像。
    decompressed_tail = zlib.decompress(compressed_tail)
    # 外层包装只压缩 0x42BC 以后的尾部，所以逻辑文件 = 原前缀 + 解压后的尾部。
    rebuilt = bytearray(raw[:LEGACY_COMPRESSED_TAIL_SPLIT] + decompressed_tail)
    # 逻辑文件里不再保留 0x6F 包装状态；将 0x0B 还原为 0，便于后续按实际结构读取描述表。
    rebuilt[0x0B] = 0

    # 声明大小必须和真正重建后的字节数一致，否则不能把结构解析结果当作可靠证据。
    if len(rebuilt) != final_size_1:
        raise ValueError(f"SAF 重建大小不匹配：重建={len(rebuilt)}，声明={final_size_1}")

    # 把压缩过程的所有关键数字写进元数据，后续文档不必靠人工抄写。
    meta.update({
        "声明最终大小1": final_size_1,
        "声明最终大小2": final_size_2,
        "zlib尾压缩大小": len(compressed_tail),
        "zlib尾解压大小": len(decompressed_tail),
        "重建大小": len(rebuilt),
        "重建大小匹配": True,
    })
    # bytes() 把可修改 bytearray 冻结成不可修改字节串，避免解析函数意外改动研究数据。
    return bytes(rebuilt), meta


def parse_section_table(data: bytes) -> List[Dict[str, Any]]:
    """解析 SAF 从 0x0C 开始的四个 10-byte section descriptor。"""
    # 四个描述项共 40 字节，所以文件至少要覆盖 0x0C..0x33；逻辑头 0x74 当然应更长，但这里先做最直接边界检查。
    if len(data) < SAF_SECTION_TABLE_OFFSET + 4 * 10:
        raise ValueError("SAF 文件不足以容纳四段描述表")

    sections: List[Dict[str, Any]] = []
    # SAF 与标准 SF2 一样有四个逻辑段；这里只确认“段编号”，不猜作者原始类名。
    for section_index in range(4):
        # 每项固定 10 字节，所以第 n 项起点 = 0x0C + n*10。
        descriptor_offset = SAF_SECTION_TABLE_OFFSET + section_index * 10
        # 前 2 字节是记录数量。
        count = read_u16(data, descriptor_offset)
        # 接下来 4 字节是该段在整个逻辑文件中的绝对偏移。
        section_offset = read_u32(data, descriptor_offset + 2)
        # 最后 4 字节是该段总字节数。
        section_size = read_u32(data, descriptor_offset + 6)
        # 段结束位置用“偏移 + 大小”得到，后面所有 record pointer 都必须落在这个范围内。
        section_end = section_offset + section_size

        # 即使记录数是 0，offset/size 仍属于格式证据；只要结束位置越过文件，就必须判失败。
        if section_end > len(data):
            raise ValueError(f"SAF Section{section_index} 超出文件：end=0x{section_end:X}, file=0x{len(data):X}")
        # 非空段开头保存 count 个 u32 绝对 record pointer，因此 pointer table 至少需要 count*4 字节。
        pointer_table_end = section_offset + count * 4
        if pointer_table_end > section_end:
            raise ValueError(f"SAF Section{section_index} pointer table 超出本段")

        # 逐个读出 absolute record pointer；当前正样本证明它们与标准 SF2 一样是“文件绝对偏移”，不是段内相对偏移。
        pointers = [read_u32(data, section_offset + record_index * 4) for record_index in range(count)]
        # 某条记录的结束位置就是下一条记录的起点；最后一条则以 section_end 为结束。
        record_ends = pointers[1:] + [section_end]
        records: List[Dict[str, Any]] = []
        # 把 pointer 和推导出的 end 配成一对，逐条做严格边界验证。
        for record_index, (record_start, record_end) in enumerate(zip(pointers, record_ends)):
            # 数据记录不能覆盖 pointer table 自身，也不能倒序或越出 section。
            if record_start < pointer_table_end or record_end < record_start or record_end > section_end:
                raise ValueError(
                    f"SAF Section{section_index} record{record_index} 边界非法："
                    f"0x{record_start:X}..0x{record_end:X}"
                )
            # 保存后续解析真正需要的绝对范围；不在这里猜具体语义。
            records.append({
                "索引": record_index,
                "偏移": record_start,
                "结束": record_end,
                "大小": record_end - record_start,
            })

        # 一个 section 的描述表字段和 record 范围合并成结构化对象，方便 JSON 直接审计。
        sections.append({
            "索引": section_index,
            "描述项偏移": descriptor_offset,
            "数量": count,
            "偏移": section_offset,
            "大小": section_size,
            "结束": section_end,
            "指针表结束": pointer_table_end,
            "记录": records,
        })

    # 返回四段，调用者再分别应用目前有正证据的 Section1/2 公式。
    return sections


def decode_rgb16_rle_record(record: bytes, tile_width: int, tile_height: int) -> Tuple[List[Optional[int]], Dict[str, int]]:
    """按标准 SF2 同型 RLE 解码一个 SAF tile；透明 run 用 None 表示。"""
    # 一个 tile 最终应产生 width*height 个像素位置；这也是防止 RLE 越界的硬上限。
    expected_pixels = tile_width * tile_height
    # pixels 按扫描顺序保存；None 与颜色值 0 必须区分，因为“透明”不能等同“黑色”。
    pixels: List[Optional[int]] = []
    # cursor 指向 record 中下一字节控制码。
    cursor = 0
    # 统计 literal/repeat/transparent 三类 run，既是诊断信息，也能发现意外的新编码类型。
    run_types: Counter[str] = Counter()

    # 一直解码到像素位置数量达到 tile 应有面积。
    while len(pixels) < expected_pixels:
        # 如果还没凑够像素却已经没有输入字节，说明记录被截断或算法不匹配。
        if cursor >= len(record):
            raise ValueError("RGB16 RLE 在 tile 填满前提前结束")
        # 取 1 字节控制码，然后把 cursor 移到控制码后的第一个 payload 字节。
        control = record[cursor]
        cursor += 1
        # 低 6 位保存 run_length-1，因此真实长度范围是 1..64。
        run_length = (control & 0x3F) + 1

        # bit6=0 表示 literal：后面直接跟 run_length 个 16-bit 像素。
        if (control & 0x40) == 0:
            run_types["literal"] += 1
            # 每个 RGB16 像素占 2 字节，所以 literal payload 共 run_length*2 字节。
            payload_size = run_length * 2
            if cursor + payload_size > len(record):
                raise ValueError("RGB16 RLE literal payload 超出 record")
            # 逐像素读取，保留原始 16-bit 值；颜色解释放到 PNG 导出阶段，解析本身不丢信息。
            for pixel_index in range(run_length):
                pixels.append(read_u16(record, cursor + pixel_index * 2))
            # 一次跨过整个 literal payload。
            cursor += payload_size

        # bit6=1 且 bit7=1 表示 repeat：后面只有 1 个颜色，重复 run_length 次。
        elif (control & 0x80) != 0:
            run_types["repeat"] += 1
            if cursor + 2 > len(record):
                raise ValueError("RGB16 RLE repeat 缺少 2 字节颜色")
            # 读取一次颜色。
            color = read_u16(record, cursor)
            cursor += 2
            # 把同一个 16-bit 颜色重复填入指定长度。
            pixels.extend([color] * run_length)

        # bit6=1 且 bit7=0 表示 transparent skip：没有 payload，只把这些像素位置标成透明。
        else:
            run_types["transparent"] += 1
            pixels.extend([None] * run_length)

        # run 绝不能把 tile 填过头；出现这种情况说明尺寸或 RLE 语义至少有一个不对。
        if len(pixels) > expected_pixels:
            raise ValueError(
                f"RGB16 RLE 输出超过 tile 面积：{len(pixels)} > {expected_pixels}"
            )

    # 固化35要求“精确消费 record”：即最后一个像素完成时，也必须刚好用完这条记录全部字节。
    if cursor != len(record):
        raise ValueError(f"RGB16 RLE 未精确消费 record：已用 {cursor} / 总长 {len(record)}")

    # 报告里同时返回像素列表和三类 run 统计，调用者可选择只保存统计或继续做 atlas 合成。
    return pixels, dict(run_types)


def rgb555_to_rgba(pixel: Optional[int]) -> Tuple[int, int, int, int]:
    """把 SAF/SF2 原始 RGB555 像素扩展成 8-bit RGBA；透明 run 输出 alpha=0。"""
    # None 是 RLE transparent run，直接输出全透明像素；RGB 值设为 0 只是透明状态下的占位，不影响显示。
    if pixel is None:
        return 0, 0, 0, 0
    # 传统 RGB555：bit10..14 为 R，bit5..9 为 G，bit0..4 为 B；每通道范围 0..31。
    red5 = (pixel >> 10) & 0x1F
    green5 = (pixel >> 5) & 0x1F
    blue5 = pixel & 0x1F
    # `(v << 3) | (v >> 2)` 是常用的 5-bit→8-bit 位复制扩展，可让 31 精确映射到 255。
    red8 = (red5 << 3) | (red5 >> 2)
    green8 = (green5 << 3) | (green5 >> 2)
    blue8 = (blue5 << 3) | (blue5 >> 2)
    # 非透明颜色的 alpha 固定 255。
    return red8, green8, blue8, 255


def write_rgba_png(path: Path, width: int, height: int, rgba: bytes) -> None:
    """只用标准库写一个 8-bit RGBA PNG；避免给研究工具增加 Pillow 等外部依赖。"""
    # RGBA 每像素 4 字节，长度不匹配说明上游合成有 bug，不能静默写出损坏图片。
    if len(rgba) != width * height * 4:
        raise ValueError(f"PNG RGBA 长度不匹配：{len(rgba)} != {width}*{height}*4")
    # PNG 文件固定以 8 字节签名开始。
    signature = b"\x89PNG\r\n\x1a\n"

    # 一个 PNG chunk = 长度 + 类型 + 数据 + CRC。定义小函数避免三种 chunk 重复拼接代码。
    def make_chunk(chunk_type: bytes, payload: bytes) -> bytes:
        # 长度是 big-endian u32，PNG 标准和游戏文件的小端序正好相反。
        length = struct.pack(">I", len(payload))
        # CRC 覆盖“4字节类型 + payload”，不包含长度字段本身。
        crc = struct.pack(">I", zlib.crc32(chunk_type + payload) & 0xFFFFFFFF)
        return length + chunk_type + payload + crc

    # IHDR：宽、高、8-bit、颜色类型6(RGBA)、压缩0、过滤0、非隔行0。
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    # PNG 每一行前需要 1 字节 filter type；这里用 0，表示不做任何预测过滤，最容易复查。
    scanlines = bytearray()
    row_size = width * 4
    for row_index in range(height):
        scanlines.append(0)
        row_start = row_index * row_size
        scanlines.extend(rgba[row_start:row_start + row_size])
    # IDAT 内容本身按 PNG 标准再用 zlib 压缩；这和游戏 SAF 的 zlib 包装是两回事。
    idat = zlib.compress(bytes(scanlines), level=9)
    # IEND 没有 payload，只标记 PNG 完成。
    png = signature + make_chunk(b"IHDR", ihdr) + make_chunk(b"IDAT", idat) + make_chunk(b"IEND", b"")
    # 创建输出目录并写文件；不会触碰原始 SAF。
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(png)


def inspect_saf(path: Path, atlas_png: Optional[Path] = None, tilemap_index: int = 0) -> Dict[str, Any]:
    """完整检查一个 SAF 样本，并可选择导出指定 tilemap 的静态 atlas。"""
    # 只读打开用户指定的样本。
    raw = path.read_bytes()
    # 先还原外层包装，后面的所有 offset 都针对“重建后的逻辑文件”。
    data, wrapper = rebuild_saf(raw)

    # SAF 正样本的逻辑头字段必须能覆盖到 0x0B；rebuild_saf 已保证这一点，这里读取具体结构值。
    logical_header_size = read_u16(data, 0x05)
    tile_width = read_u16(data, 0x07)
    tile_height = read_u16(data, 0x09)
    # 固化35把 0x74 作为“该正样本确认的 SAF 逻辑头大小”；不同值不能无证据地套用当前布局。
    if logical_header_size != SAF_LOGICAL_HEADER_SIZE:
        raise ValueError(
            f"SAF 逻辑 Header size 不是当前已确认的 0x74：实际 0x{logical_header_size:X}"
        )
    # tile 尺寸为 0 时，RLE 面积没有意义，因此直接拒绝。
    if tile_width <= 0 or tile_height <= 0:
        raise ValueError(f"SAF tile 尺寸非法：{tile_width}×{tile_height}")

    # 解析从 0x0C 开始的四段描述表，并对每条 record 做绝对边界验证。
    sections = parse_section_table(data)

    # Section0 当前只有一个 23-byte 正样本，尚无 EXE consumer 可以安全命名字段，所以原样保留十六进制。
    section0_records: List[Dict[str, Any]] = []
    for record_info in sections[0]["记录"]:
        record = data[record_info["偏移"]:record_info["结束"]]
        section0_records.append({
            **record_info,
            "原始十六进制": record.hex(" "),
            "语义状态": "UNKNOWN_RAW：固化35未找到可证明字段含义的SAF Reader/consumer",
        })

    # Section1 每条正记录都按“u16 cols + u16 rows + cols*rows 个 u16 tile id”验证。
    section1_records: List[Dict[str, Any]] = []
    # 后面合成 atlas 时需要原始 tile ID 列表，所以另存一份只在内存使用，不塞进结构元数据对象以外的地方。
    tilemaps: List[Tuple[int, int, List[int]]] = []
    for record_info in sections[1]["记录"]:
        record = data[record_info["偏移"]:record_info["结束"]]
        # 最少 4 字节才可能容纳 cols/rows。
        if len(record) < 4:
            raise ValueError("SAF Section1 record 小于 4 字节，无法读取列/行")
        columns = read_u16(record, 0)
        rows = read_u16(record, 2)
        expected_size = 4 + columns * rows * 2
        # 固化35正样本 264 字节正好满足 4+2*26*5；这里用通式，不把 26×5 写死。
        if len(record) != expected_size:
            raise ValueError(
                f"SAF Section1 record 大小公式失败：实际 {len(record)}，预期 {expected_size}"
            )
        # 按行优先顺序读取所有 tile ID。
        tile_ids = [read_u16(record, 4 + index * 2) for index in range(columns * rows)]
        # tile ID 必须索引 Section2 的 tile 记录；越界说明我们对 Section1 的理解不成立。
        invalid_ids = [value for value in tile_ids if value >= sections[2]["数量"]]
        if invalid_ids:
            raise ValueError(f"SAF Section1 出现越界 tile ID：{invalid_ids[:8]}")
        tilemaps.append((columns, rows, tile_ids))
        section1_records.append({
            **record_info,
            "列": columns,
            "行": rows,
            "记录大小公式": f"4 + 2*{columns}*{rows} = {expected_size}",
            "公式成立": True,
            "TileID数量": len(tile_ids),
            "TileID最小值": min(tile_ids) if tile_ids else None,
            "TileID最大值": max(tile_ids) if tile_ids else None,
            "唯一TileID数": len(set(tile_ids)),
            "TileID列表": tile_ids,
            "静态图集宽": columns * tile_width,
            "静态图集高": rows * tile_height,
        })

    # Section2 是 tile 记录。每条都必须按已有 SF2 RGB16 RLE 完整、精确解码。
    decoded_tiles: List[List[Optional[int]]] = []
    aggregate_runs: Counter[str] = Counter()
    section2_records: List[Dict[str, Any]] = []
    for record_info in sections[2]["记录"]:
        record = data[record_info["偏移"]:record_info["结束"]]
        pixels, run_count = decode_rgb16_rle_record(record, tile_width, tile_height)
        decoded_tiles.append(pixels)
        aggregate_runs.update(run_count)
        section2_records.append({
            **record_info,
            "解码像素数": len(pixels),
            "预期像素数": tile_width * tile_height,
            "精确解码": len(pixels) == tile_width * tile_height,
            "RLE运行统计": run_count,
        })

    # 当前正样本 Section3 为空；若未来出现非空记录，先只保留边界，不在没有 consumer 的情况下猜格式。
    section3_records = [dict(record_info) for record_info in sections[3]["记录"]]

    # 可选 PNG 只用于“从已解析结构静态重建”，不会反向影响格式判定。
    atlas_result: Optional[Dict[str, Any]] = None
    if atlas_png is not None:
        # 用户指定的 tilemap 索引必须存在；当前样本只有索引 0，但工具不把这个数量写死。
        if tilemap_index < 0 or tilemap_index >= len(tilemaps):
            raise ValueError(f"tilemap 索引越界：{tilemap_index}，实际数量={len(tilemaps)}")
        columns, rows, tile_ids = tilemaps[tilemap_index]
        atlas_width = columns * tile_width
        atlas_height = rows * tile_height
        # 先分配完全透明的 RGBA 画布，随后逐 tile 写入。
        rgba = bytearray(atlas_width * atlas_height * 4)
        for map_index, tile_id in enumerate(tile_ids):
            # tilemap 是行优先：列号 = index % columns，行号 = index // columns。
            tile_column = map_index % columns
            tile_row = map_index // columns
            tile_pixels = decoded_tiles[tile_id]
            # 每个 tile 内部也按行优先扫描。
            for local_index, pixel in enumerate(tile_pixels):
                local_x = local_index % tile_width
                local_y = local_index // tile_width
                atlas_x = tile_column * tile_width + local_x
                atlas_y = tile_row * tile_height + local_y
                rgba_offset = (atlas_y * atlas_width + atlas_x) * 4
                red, green, blue, alpha = rgb555_to_rgba(pixel)
                rgba[rgba_offset:rgba_offset + 4] = bytes((red, green, blue, alpha))
        # 标准库 PNG writer 真正落盘。
        write_rgba_png(atlas_png, atlas_width, atlas_height, bytes(rgba))
        # 报告输出文件哈希，确保 PNG 证据本身也能独立核对。
        atlas_bytes = atlas_png.read_bytes()
        atlas_result = {
            "Tilemap索引": tilemap_index,
            "输出文件": str(atlas_png),
            "宽": atlas_width,
            "高": atlas_height,
            "SHA256": sha256_bytes(atlas_bytes),
            "像素解释": "RGB555；RLE transparent run -> alpha 0",
        }

    # 输出以“已证明”和“仍未知”分开，避免未来接档者把静态重建误读为已找到运行时 Reader。
    return {
        "工具版本": TOOL_VERSION,
        "文件": str(path),
        "原始SHA256": sha256_bytes(raw),
        "原始魔数": raw[:4].hex(" "),
        "包装": wrapper,
        "重建后魔数": data[:4].hex(" "),
        "逻辑Header大小_0x05": logical_header_size,
        "Tile宽_0x07": tile_width,
        "Tile高_0x09": tile_height,
        "Section描述表起点": SAF_SECTION_TABLE_OFFSET,
        "Section": [
            {key: value for key, value in section.items() if key != "记录"}
            for section in sections
        ],
        "Section0未知记录": section0_records,
        "Section1Tilemap": section1_records,
        "Section2Tile": section2_records,
        "Section2全部精确RLE解码": len(decoded_tiles) == sections[2]["数量"],
        "Section2精确解码Tile数": len(decoded_tiles),
        "Section2总解码像素": sum(len(tile) for tile in decoded_tiles),
        "Section2RLE总运行统计": dict(aggregate_runs),
        "Section3记录": section3_records,
        "导出图集": atlas_result,
        "固化35已确认": [
            "SAF\\x05 在当前正样本中拥有 0x74 逻辑头，而不是标准 SF2 的 0x42BC 逻辑头。",
            "SAF 四段描述表从 0x0C 开始；每项为 u16 count + u32 absolute offset + u32 size。",
            "非空段开头使用 count 个 u32 绝对 record pointer。",
            "Section1 正样本符合 4 + 2*columns*rows，tile id 全部落入 Section2。",
            "Section2 全部记录复用已知 SF2 RGB16 RLE 控制码语义，并要求像素和输入字节双重精确结束。",
        ],
        "固化35仍未知": [
            "Section0 的 23-byte record 字段语义尚无 SAF consumer 证据，不命名。",
            "当前样本 Section3 为空，不能推断非空 Section3 的结构。",
            "静态 atlas 可重建不等于已经找到当前 RPG.exe 的 SAF Reader 或实际运行时请求源。",
        ],
    }


class PEImage:
    """只实现本项目静态断言需要的最小 PE32 映射，不承担通用 PE 编辑功能。"""

    def __init__(self, path: Path) -> None:
        # 读取完整 EXE；本类所有操作都基于这份只读 bytes。
        self.path = path
        self.data = path.read_bytes()
        # DOS 头前两字节必须是 MZ，否则不能按 PE 结构解释。
        if len(self.data) < 0x40 or self.data[:2] != b"MZ":
            raise ValueError("输入不是有效 MZ/PE 文件")
        # DOS 头 +0x3C 保存 PE Header 的文件偏移。
        pe_offset = read_u32(self.data, 0x3C)
        if pe_offset + 0x18 > len(self.data) or self.data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("找不到有效 PE\\0\\0 签名")
        # COFF Header：+4 Machine，+6 NumberOfSections，+20 SizeOfOptionalHeader。
        self.number_of_sections = read_u16(self.data, pe_offset + 6)
        optional_size = read_u16(self.data, pe_offset + 20)
        optional_offset = pe_offset + 24
        if optional_offset + optional_size > len(self.data):
            raise ValueError("PE Optional Header 越界")
        # PE32 Optional Header +0x1C 是 ImageBase；当前 RPG.exe 是 32 位程序。
        magic = read_u16(self.data, optional_offset)
        if magic != 0x10B:
            raise ValueError(f"当前工具只验证 PE32，实际 OptionalHeader magic=0x{magic:X}")
        self.image_base = read_u32(self.data, optional_offset + 0x1C)
        # Section Table 紧跟 Optional Header。
        section_table = optional_offset + optional_size
        self.sections: List[Dict[str, int | str]] = []
        for index in range(self.number_of_sections):
            entry = section_table + index * 40
            if entry + 40 > len(self.data):
                raise ValueError("PE Section Table 越界")
            # 8 字节 section name 以 NUL 终止，例如 .text、.rdata。
            name = self.data[entry:entry + 8].split(b"\0", 1)[0].decode("ascii", "replace")
            virtual_size = read_u32(self.data, entry + 8)
            virtual_address = read_u32(self.data, entry + 12)
            raw_size = read_u32(self.data, entry + 16)
            raw_offset = read_u32(self.data, entry + 20)
            self.sections.append({
                "name": name,
                "virtual_size": virtual_size,
                "virtual_address": virtual_address,
                "raw_size": raw_size,
                "raw_offset": raw_offset,
            })

    def va_to_file_offset(self, va: int) -> int:
        """把运行时 VA 转成磁盘文件偏移。"""
        # RVA = VA - ImageBase；负数显然不在该映像内。
        rva = va - self.image_base
        if rva < 0:
            raise ValueError(f"VA 低于 ImageBase：0x{va:X}")
        # 找包含该 RVA 的 section。用 max(virtual_size, raw_size) 兼容磁盘/内存尺寸略有差异的老 PE。
        for section in self.sections:
            start = int(section["virtual_address"])
            span = max(int(section["virtual_size"]), int(section["raw_size"]))
            if start <= rva < start + span:
                offset = int(section["raw_offset"]) + (rva - start)
                if offset < 0 or offset > len(self.data):
                    break
                return offset
        raise ValueError(f"VA 不在已映射 PE section：0x{va:X}")

    def read_va(self, va: int, size: int) -> bytes:
        """按 VA 读取磁盘映像中的机器码/静态数据。"""
        offset = self.va_to_file_offset(va)
        if offset + size > len(self.data):
            raise ValueError(f"VA 读取越界：0x{va:X}+0x{size:X}")
        return self.data[offset:offset + size]

    def section_bytes(self, name: str) -> Tuple[int, bytes]:
        """返回某个 section 的“运行时起始 VA + 磁盘 raw bytes”。"""
        for section in self.sections:
            if section["name"] == name:
                start = int(section["raw_offset"])
                size = int(section["raw_size"])
                va = self.image_base + int(section["virtual_address"])
                return va, self.data[start:start + size]
        raise ValueError(f"PE 不含 section：{name}")

    def find_ascii_va(self, text: bytes) -> List[int]:
        """在各 PE section 的 raw data 里找 ASCII 字节串，并返回准确 VA。"""
        hits: List[int] = []
        for section in self.sections:
            raw_offset = int(section["raw_offset"])
            raw_size = int(section["raw_size"])
            virtual_address = int(section["virtual_address"])
            section_data = self.data[raw_offset:raw_offset + raw_size]
            cursor = 0
            while True:
                found = section_data.find(text, cursor)
                if found < 0:
                    break
                hits.append(self.image_base + virtual_address + found)
                cursor = found + 1
        return sorted(hits)


def byte_assert(pe: PEImage, va: int, expected_hex: str, meaning: str) -> Dict[str, Any]:
    """验证指定 VA 的机器码是否和固化35记录完全一致。"""
    # 文档里用空格分隔十六进制最便于人工核对；bytes.fromhex() 将它还原成真正字节。
    expected = bytes.fromhex(expected_hex)
    # 从当前 EXE 映像按 VA 读取同样长度。
    actual = pe.read_va(va, len(expected))
    # 不做“近似匹配”；逆向地址证据必须逐字节一致才算 PASS。
    passed = actual == expected
    return {
        "地址": f"0x{va:08X}",
        "含义": meaning,
        "status": "PASS" if passed else "FAIL",
        "期望": expected.hex(" ").upper(),
        "实际": actual.hex(" ").upper(),
    }


def direct_e8_callers(pe: PEImage, target_va: int) -> List[int]:
    """穷举 .text 中 opcode E8 rel32 的直接 caller；不把它扩大解释成所有可能的间接调用。"""
    # 只扫描可执行主代码段 .text，避免把资源数据里的偶然 0xE8 当作机器指令。
    text_va, text = pe.section_bytes(".text")
    callers: List[int] = []
    # E8 rel32 共 5 字节，所以最后 4 字节不能作为完整 call 起点。
    for index in range(max(0, len(text) - 4)):
        # 首字节不是 E8 就一定不是这种“near relative direct call”。
        if text[index] != 0xE8:
            continue
        # rel32 是有符号 32 位位移，以“下一条指令地址”作为基准。
        relative = struct.unpack_from("<i", text, index + 1)[0]
        source_va = text_va + index
        destination_va = (source_va + 5 + relative) & 0xFFFFFFFF
        if destination_va == target_va:
            callers.append(source_va)
    return callers


def verify_rpg_saf_boundary(exe_path: Path, saf_path: Path) -> Dict[str, Any]:
    """重放固化35“已知标准链 vs 未定位 SAF Reader”的静态机器边界。"""
    # PEImage 负责 VA 映射和 .text caller 扫描；不会写入 EXE。
    pe = PEImage(exe_path)
    # SAF 本体也重新解析一次，确保 EXE 报告引用的样本结构仍然是已经验证的那份。
    saf_report = inspect_saf(saf_path)
    # 机器码断言都放进同一个数组，最终可以统一统计 PASS/FAIL。
    checks: List[Dict[str, Any]] = []

    # 标准 SF2 构造器先比较前三字节 “SF2”；SAF 因此不会进入它后面的正常初始化分支。
    checks.append(byte_assert(
        pe, 0x00428C21,
        "6A 03 68 08 A6 46 00 56 E8 92 41 03 00",
        "标准SF2构造器用长度3比较载荷前三字节与字符串SF2",
    ))
    checks.append(byte_assert(
        pe, 0x00428C31,
        "85 C0 75 22",
        "SF2三字节比较非零时跳到拒绝分支",
    ))
    checks.append(byte_assert(
        pe, 0x00428CCB,
        "8A 45 0B 3C 6F 75 7B",
        "只有通过SF2魔数门后才检查+0x0B的0x6F包装标志",
    ))

    # 0x4070D0 过去被泛称“通用动画加载器”；固化35证明它实际上绑定标准 SF2 的 0x14 起四段表。
    checks.append(byte_assert(
        pe, 0x004070FF,
        "66 8B 48 14",
        "0x4070D0对象初始化读取标准SF2 Section0 count @ +0x14",
    ))
    checks.append(byte_assert(
        pe, 0x00407128,
        "8B 50 16",
        "0x4070D0对象初始化读取标准SF2 Section0 offset @ +0x16",
    ))
    checks.append(byte_assert(
        pe, 0x00407133,
        "8B 48 20",
        "0x4070D0对象初始化读取标准SF2 Section1 offset @ +0x20",
    ))
    checks.append(byte_assert(
        pe, 0x0040713B,
        "8B 50 2A",
        "0x4070D0对象初始化读取标准SF2 Section2 offset @ +0x2A",
    ))
    checks.append(byte_assert(
        pe, 0x00407143,
        "8B 48 34",
        "0x4070D0对象初始化读取标准SF2 Section3 offset @ +0x34",
    ))
    checks.append(byte_assert(
        pe, 0x00407165,
        "66 39 7A 1E",
        "0x4070D0对象初始化读取标准SF2 Section1 count @ +0x1E",
    ))
    checks.append(byte_assert(
        pe, 0x0040717F,
        "66 39 78 32",
        "0x4070D0对象初始化读取标准SF2 Section3 count @ +0x32",
    ))

    # ItfDir.DAT 在启动时被注册为别名 itf；这证明 SAF 所在容器确实进入资源系统，而不是未挂载孤立文件。
    checks.append(byte_assert(
        pe, 0x0043A1F0,
        "68 74 B9 46 00 68 54 B9 46 00 E8 31 FB 00 00",
        "启动链把别名itf与..\\MultiMedia\\Fight\\ItfDir.DAT送入0x449D30注册/挂载",
    ))

    # 动态 Fight 路径生成器说明“EXE 没有精确 ITF000.SF2 字面量”不能单独证明运行时绝对不可达。
    checks.append(byte_assert(
        pe, 0x0041F7F9,
        "68 78 9F 46 00 68 6C 9F 46 00 56 E8 6A 22 03 00",
        "动态路径链使用FIGHT\\与%s%s\\%s.SF2格式串生成SF2路径",
    ))
    checks.append(byte_assert(
        pe, 0x0043DD15,
        "68 78 9F 46 00 68 6C 9F 46 00 57 E8 4E 3D 01 00",
        "另一对象路径链同样使用FIGHT\\与%s%s\\%s.SF2动态格式",
    ))

    # canonical RGB16 RLE decoder 在当前 .text 中只有两个 E8 direct caller；固化35逐一检查其来源。
    rle_callers = direct_e8_callers(pe, 0x0044CD7B)
    expected_rle_callers = [0x004071D9, 0x00428E48]
    checks.append({
        "地址": "0x0044CD7B",
        "含义": "canonical RGB16 RLE decoder 的 .text E8 直接 caller 恰为两个标准SF2对象链",
        "status": "PASS" if rle_callers == expected_rle_callers else "FAIL",
        "期望": [f"0x{value:08X}" for value in expected_rle_callers],
        "实际": [f"0x{value:08X}" for value in rle_callers],
    })
    # 两个 caller 都在调用前读取标准对象保存的 tile 宽高并把 record 指针交给 decoder。
    checks.append(byte_assert(
        pe, 0x004071CC,
        "66 8B 48 09 52 33 D2 51 66 8B 50 07 52 E8 9D 5B 04 00",
        "0x4071D9调用canonical RLE decoder前从标准对象Header读取+0x09/+0x07 tile尺寸",
    ))
    checks.append(byte_assert(
        pe, 0x00428E1F,
        "66 8B 58 07 33 ED 66 8B 68 09",
        "0x428E48调用链同样从严格SF2对象Header读取+0x07/+0x09 tile尺寸",
    ))

    # 精确 ASCII 搜索使用 PE section→VA 映射，不用“文件偏移+ImageBase”的近似方法。
    saf_literal_hits = pe.find_ascii_va(b"SAF")
    bare_name_hits = pe.find_ascii_va(b"ITF000.SF2\0")
    itfdir_hits = pe.find_ascii_va(b"..\\MultiMedia\\Fight\\ItfDir.DAT\0")
    alias_hits = pe.find_ascii_va(b"itf\0")
    dynamic_format_hits = pe.find_ascii_va(b"%s%s\\%s.SF2\0")
    fight_prefix_hits = pe.find_ascii_va(b"FIGHT\\\0")

    # 当前 EXE 整文件哈希可能不是历史 canonical Oracle，所以单独报告“是否等于固化34兼容候选”。
    exe_sha = sha256_bytes(pe.data)
    # 只要任一机器码断言失败，报告总状态就必须失败，不能用文字结论掩盖版本漂移。
    all_checks_pass = all(item.get("status") == "PASS" for item in checks)

    return {
        "工具版本": TOOL_VERSION,
        "RPG.exe": str(exe_path),
        "RPG.exe_SHA256": exe_sha,
        "是否固化34兼容候选哈希": exe_sha == SOLID34_COMPATIBLE_RPG_ORG_SHA256,
        "SAF文件": str(saf_path),
        "SAF_SHA256": saf_report["原始SHA256"],
        "SAF逻辑结构摘要": {
            "逻辑Header大小": saf_report["逻辑Header大小_0x05"],
            "Tile宽": saf_report["Tile宽_0x07"],
            "Tile高": saf_report["Tile高_0x09"],
            "Section": saf_report["Section"],
            "Section2全部精确RLE解码": saf_report["Section2全部精确RLE解码"],
        },
        "字符串边界": {
            "SAF三字节字面VA": [f"0x{value:08X}" for value in saf_literal_hits],
            "精确ITF000.SF2_NUL字面VA": [f"0x{value:08X}" for value in bare_name_hits],
            "ItfDir.DAT路径VA": [f"0x{value:08X}" for value in itfdir_hits],
            "itf别名VA": [f"0x{value:08X}" for value in alias_hits],
            "动态SF2路径格式串VA": [f"0x{value:08X}" for value in dynamic_format_hits],
            "FIGHT前缀VA": [f"0x{value:08X}" for value in fight_prefix_hits],
        },
        "canonical_RLE_decoder": {
            "地址": "0x0044CD7B",
            "E8直接caller": [f"0x{value:08X}" for value in rle_callers],
            "直接caller数量": len(rle_callers),
            "地址作为32位立即数字面出现次数": pe.data.count(struct.pack("<I", 0x0044CD7B)),
        },
        "机器断言": checks,
        "机器断言数量": len(checks),
        "机器断言全部通过": all_checks_pass,
        "固化35静态结论": [
            "ItfDir.DAT 在当前 EXE 启动链中确实注册为别名 itf，因此 SAF 所在容器属于已挂载资源域。",
            "0x4070D0 明确读取标准 SF2 从 +0x14 开始的四段描述表，不能直接解释 SAF 从 +0x0C 开始的布局。",
            "0x428B60 标准对象在进入 0x6F 包装逻辑前先做 SF2 三字节魔数门，SAF 会被该对象拒绝。",
            "0x44CD7B canonical RGB16 RLE decoder 的全部 E8 直接 caller 只有 0x4071D9 与 0x428E48，二者均属于标准 SF2 对象链。",
            "当前 EXE 无 SAF 三字节字面量，也无精确 ITF000.SF2\\0；但存在动态 %s%s\\%s.SF2 路径生成，因此不能仅凭字符串缺失宣称运行时绝对不可达。",
        ],
        "固化35边界": [
            "本工具穷举的是 .text 中 E8 rel32 直接 caller，不形式化排除函数指针、内联副本、自修改代码或外部模块。",
            "SAF 第2段与标准 SF2 RLE 数据格式相同，只能证明“磁盘 codec 同型”，不能证明当前 EXE 运行时一定复用 0x44CD7B 这个具体函数。",
            "截至固化35仍未静态定位能按 SAF +0x0C 描述表消费 ITF000 的专用 Reader/consumer；因此不编造调用链。",
        ],
    }


def build_argument_parser() -> argparse.ArgumentParser:
    """集中定义命令行界面，避免 main() 里混进大量参数声明。"""
    # 程序总说明会显示在 `-h/--help` 中。
    parser = argparse.ArgumentParser(description="《幽城幻剑录》SAF异格式静态调查器（固化35）")
    # 子命令让“资源结构检查”和“EXE静态边界验证”互不混淆。
    subparsers = parser.add_subparsers(dest="command", required=True)

    # `检查SAF` 只需要一个 SAF 文件；可选导出 JSON 与 PNG。
    inspect_parser = subparsers.add_parser("检查SAF", help="解析SAF四段结构、验证RLE，并可导出静态图集PNG")
    inspect_parser.add_argument("file", type=Path, help="要检查的 SAF/ITF000.SF2 文件")
    inspect_parser.add_argument("--json", type=Path, help="可选：把完整结构报告写到 JSON")
    inspect_parser.add_argument("--atlas-png", type=Path, help="可选：把指定Section1 tilemap静态重建成透明PNG")
    inspect_parser.add_argument("--tilemap-index", type=int, default=0, help="导出哪条Section1 tilemap；默认0")

    # `验证RPG_SAF静态边界` 同时需要 EXE 和 SAF 样本，用来重放地址/机器码结论。
    verify_parser = subparsers.add_parser("验证RPG_SAF静态边界", help="验证ItfDir挂载、标准SF2排斥边界与RLE直接caller")
    verify_parser.add_argument("RPG", type=Path, help="要验证的 RPG.exe")
    verify_parser.add_argument("SAF", type=Path, help="对应的 ITF000.SF2 / SAF 正样本")
    verify_parser.add_argument("--json", type=Path, help="可选：把完整静态断言报告写到 JSON")

    return parser


def main(argv: Optional[Sequence[str]] = None) -> int:
    """程序入口：执行用户选中的子命令，并用退出码明确告诉自动化脚本成功或失败。"""
    # 先建立参数规则，再解析真实命令行（或测试传入的 argv）。
    parser = build_argument_parser()
    args = parser.parse_args(argv)

    try:
        # 资源结构命令只解析 SAF，并根据参数决定是否额外写 PNG。
        if args.command == "检查SAF":
            result = inspect_saf(args.file, args.atlas_png, args.tilemap_index)
        # EXE 边界命令会内部再次解析 SAF，确保静态地址与样本结构来自同一次可重复验证。
        elif args.command == "验证RPG_SAF静态边界":
            result = verify_rpg_saf_boundary(args.RPG, args.SAF)
        # argparse 已限制了子命令，正常情况下不会走到这里；保留防御分支让未来修改时失败更清楚。
        else:
            raise ValueError(f"未知命令：{args.command}")

        # 如果用户指定 --json，就把与屏幕输出相同的结构化结果写入文件。
        if getattr(args, "json", None) is not None:
            write_json(args.json, result)
        # 屏幕也完整打印，便于不写 JSON 时临时检查。
        print(json.dumps(result, ensure_ascii=False, indent=2))
        # 能执行到这里表示所有严格结构/机器断言函数都没有抛异常；返回 0 表示成功。
        return 0
    except Exception as exc:
        # 错误写到 stderr，便于 BAT/CI 把正常 JSON 与失败原因分开捕获。
        import sys
        print(f"错误：{exc}", file=sys.stderr)
        # 非零退出码 2 明确表示验证失败，不让脚本误以为“打印过一些结果”就是成功。
        return 2


# 只有用户直接运行这个 .py 时才进入 main；如果以后被别的工具 import，不会自动执行命令行逻辑。
if __name__ == "__main__":
    raise SystemExit(main())
