#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""《幽城幻剑录》SF2 Layer 32 位序列化元数据调查器（固化39）。

这个工具只做“读”和“验证”，不会修改游戏文件。
它解决的核心问题是：SF2 的 29 字节 Layer 记录里，偏移 +0x06 到 +0x09
究竟应当看成两个独立的 16 位字段，还是一个完整的 32 位字段。

固化39的证据做法分成两部分：
1. 对全部散装 SF2 与已确认 DAT 容器内 SF2 做 SHA-256 去重统计；
2. 对 RPG.exe 中两套已确认 Layer 指针构造/绘制家族重放机器码断言。

注意：
- 工具只确认“结构”和“当前直接消费边界”；
- 工具不会把这个 i32 强行命名成轨道号、层号、父节点号等作者业务术语；
- 即使当前两套 canonical renderer 都不读它，也不能推出这个字段可以删除。
"""

from __future__ import annotations

# argparse 用来解析命令行，例如“语料”“验证RPG”“综合”三个子命令。
import argparse
# csv 用来写出简单、可人工查看的取值分布表。
import csv
# hashlib 用来对每份原始 SF2 计算 SHA-256，保证重复文件只统计一次。
import hashlib
# importlib.util 用来加载同目录已经稳定的 SF2 / 资源容器工具，而不是复制一份旧解析代码。
import importlib.util
# json 用来输出机器可读的完整调查结果。
import json
# struct 用来按小端读取 PE、i16、i32 等二进制整数。
import struct
# sys 用来把动态加载的兄弟模块登记进当前 Python 进程。
import sys
# Counter / defaultdict 用于做取值频数和 SHA 分组。
from collections import Counter, defaultdict
# dataclass 让机器码断言的每个字段更清楚，不需要靠神秘的列表下标。
from dataclasses import dataclass
# Path 统一处理 Windows/Linux 路径，不手工拼接斜杠。
from pathlib import Path
# Any 只用于说明动态加载模块与 JSON 字典的类型边界。
from typing import Any

# 工具版本写进所有输出，接档时能知道结果来自哪一版逻辑。
VERSION = "0.7D.39.1"
# 当前研究对象是 32 位 x86 PE；历史项目基址一直是 0x00400000。
EXPECTED_IMAGE_BASE = 0x00400000
# Layer 固定记录长度已经由旧固化验证为 29 字节。
LAYER_SIZE = 29
# Layer 数组在每条 Section0 record 内从 +0x5E 开始。
LAYER_ARRAY_OFFSET = 0x5E
# Layer 数量在 Section0 record +0x54，是无符号 16 位。
LAYER_COUNT_OFFSET = 0x54
# 本轮要修正的字段从 Layer +0x06 开始，占 4 字节。
METADATA_OFFSET = 0x06


@dataclass(frozen=True)
class MachineAssertion:
    """一条“某虚拟地址必须具有某串机器码”的静态证据。"""

    # address 是 RPG.exe 运行时虚拟地址，例如 0x00407279。
    address: int
    # expected_hex 是不带 0x 的十六进制字节串；空格只是为了让人容易看。
    expected_hex: str
    # meaning 用简体中文说明为什么要检查这串机器码。
    meaning: str


def _load_sibling_module(module_name: str, filename: str) -> Any:
    """从本工具同目录加载另一份稳定工具模块。"""

    # __file__ 是当前脚本自己的完整路径；parent 就是“工具”目录。
    tools_dir = Path(__file__).resolve().parent
    # 拼出要复用的兄弟工具完整路径。
    module_path = tools_dir / filename
    # 如果包被拆坏、缺少兄弟工具，立即明确报错，而不是稍后出现难懂的 AttributeError。
    if not module_path.is_file():
        raise FileNotFoundError(f"缺少必须随包携带的兄弟工具：{module_path}")
    # 根据文件路径创建 Python 导入规格。
    spec = importlib.util.spec_from_file_location(module_name, module_path)
    # loader 不存在说明 Python 无法从该文件构造模块。
    if spec is None or spec.loader is None:
        raise RuntimeError(f"无法加载兄弟工具：{module_path}")
    # 创建真正的模块对象。
    module = importlib.util.module_from_spec(spec)
    # 先放入 sys.modules，dataclass 等机制才能正确识别模块归属。
    sys.modules[module_name] = module
    # 执行兄弟文件；它们的 main() 都有 __name__ 保护，所以不会误跑命令行主流程。
    spec.loader.exec_module(module)
    # 把加载后的模块交给调用者。
    return module


# 复用固化38已经稳定的 SF2 解压/Section 指针表解析器。
_sf2_tool = _load_sibling_module("youcheng_sf2_core_for_g39", "youcheng_sci_sf2_inspector.py")
# 复用固化37已经稳定的“散装 + 已确认资源容器内 SF2”语料收集器。
_audio_tool = _load_sibling_module("youcheng_sf2_audio_for_g39", "youcheng_sf2_audio_inspector.py")


def _i16(data: bytes, offset: int) -> int:
    """从 data[offset:offset+2] 读取一个 little-endian signed 16-bit。"""

    # '<h' 的 '<' 表示小端，h 表示有符号 16 位。
    return struct.unpack_from("<h", data, offset)[0]


def _u16(data: bytes, offset: int) -> int:
    """读取 little-endian unsigned 16-bit。"""

    # '<H' 的 H 表示无符号 16 位。
    return struct.unpack_from("<H", data, offset)[0]


def _i32(data: bytes, offset: int) -> int:
    """读取 little-endian signed 32-bit。"""

    # '<i' 的 i 表示有符号 32 位。
    return struct.unpack_from("<i", data, offset)[0]


def _sha256_bytes(data: bytes) -> str:
    """返回字节串的 SHA-256 十六进制文本。"""

    # hexdigest() 直接得到固定 64 个十六进制字符，方便 JSON/文档引用。
    return hashlib.sha256(data).hexdigest()


def inspect_corpus(root: Path) -> dict[str, Any]:
    """扫描完整 multimedia 类目录并验证 Layer +0x06..+0x09 的 32 位结构。"""

    # resolve() 把相对路径转为绝对路径，让报告来源可复现。
    root = root.resolve()
    # 资源根不存在时立即退出，避免生成“0个样本全部通过”的假报告。
    if not root.is_dir():
        raise FileNotFoundError(f"资源根不存在或不是目录：{root}")

    # collect_corpus_sources 会同时枚举散装 SF2 和已确认 DAT 容器里的 SF2。
    sources = _audio_tool.collect_corpus_sources(root)
    # groups 的 key 是原始文件 SHA；value 保存所有重复出现来源。
    groups: dict[str, list[Any]] = defaultdict(list)
    # 逐个出现计算 SHA，同内容只在后面解析一次。
    for source in sources:
        groups[_sha256_bytes(source.raw)].append(source)

    # i32_values 统计真正应当使用的完整 32 位有符号值。
    i32_values: Counter[int] = Counter()
    # low_words/high_words 保留“旧拆成两个 i16”时看到的低/高半字，方便证明二者并不独立。
    low_words: Counter[int] = Counter()
    high_words: Counter[int] = Counter()
    # word_pairs 统计 (低16位, 高16位) 的所有实际组合。
    word_pairs: Counter[tuple[int, int]] = Counter()
    # mode_value 观察该元数据和渲染 mode 的交叉分布，但不据此强命名字段。
    mode_value: Counter[tuple[int, int]] = Counter()
    # sequence_counter 记录同一 Section0 record 内各 Layer 的 i32 序列，辅助后续人工研究。
    sequence_counter: Counter[tuple[int, ...]] = Counter()

    # 这些计数器用于最终报告完整覆盖范围。
    standard_sf2_count = 0
    nonstandard_sf2_count = 0
    section0_record_count = 0
    layer_count = 0
    # failures 不能静默丢失任何无法解析的标准 SF2。
    failures: list[dict[str, Any]] = []
    # examples 保存少量代表性非零记录，不把 11184 条全部塞进主 JSON。
    examples: list[dict[str, Any]] = []

    # sorted(groups.items()) 保证同样输入每次输出顺序稳定，便于 diff。
    for sha256, same_content_sources in sorted(groups.items()):
        # 同 SHA 只取第一条作为代表来源；原始 bytes 本来就完全相同。
        source = same_content_sources[0]
        try:
            # 标准 SF2 工具会处理 0x6F/zlib 外包装并返回逻辑完整文件。
            logical, _wrap = _sf2_tool.sf2_decompress(source.raw)
        except ValueError as exc:
            # 已知 SAF 会被标准 SF2 解析器拒绝；它不是本工具的 29-byte Layer 研究对象。
            nonstandard_sf2_count += 1
            # 如果连原始魔数都不是 SAF，则记为真正异常，不能把未知失败冒充已知 SAF。
            if source.raw[:3] != b"SAF":
                failures.append({
                    "SHA256": sha256,
                    "代表来源": source.display_path,
                    "错误": str(exc),
                })
            # 非标准文件不进入标准 SF2 Layer 统计。
            continue

        # 能通过标准 SF2 解压并读取四 Section，才算本轮标准样本。
        sections = _sf2_tool.sf2_sections(logical)
        standard_sf2_count += 1

        # Section0 的每条 record 都包含固定 0x5E 头 + N 个 29-byte Layer。
        for record_desc in sections[0]["记录"]:
            # 用绝对起止指针切出这一条完整 Section0 record。
            record = logical[record_desc["偏移"]:record_desc["结束"]]
            section0_record_count += 1
            # 少于 0x5E 连固定头都放不下，属于结构错误。
            if len(record) < LAYER_ARRAY_OFFSET:
                raise ValueError(f"Section0 record 过短：{source.display_path}#{record_desc['索引']}")
            # +0x54 是 Layer 数量。
            count = _u16(record, LAYER_COUNT_OFFSET)
            # 固化23以后已经反复验证该精确长度公式；这里继续作为防错前置条件。
            expected_size = LAYER_ARRAY_OFFSET + LAYER_SIZE * count
            if len(record) != expected_size:
                raise ValueError(
                    f"Section0 record 大小不满足 0x5E+29*N：{source.display_path}#{record_desc['索引']} "
                    f"实际={len(record)} 期望={expected_size}"
                )

            # current_sequence 收集这一帧/动作记录内所有 Layer 的 i32 值。
            current_sequence: list[int] = []
            # 按 Layer 数量逐项切 29 字节。
            for layer_index in range(count):
                # 计算当前 Layer 的起点和终点。
                start = LAYER_ARRAY_OFFSET + layer_index * LAYER_SIZE
                end = start + LAYER_SIZE
                # 切出当前恰好 29 字节 Layer。
                layer = record[start:end]

                # 旧文档把 +0x06 和 +0x08 各读成一个 i16；这里仍读出来作为对照证据。
                low_i16 = _i16(layer, METADATA_OFFSET)
                high_i16 = _i16(layer, METADATA_OFFSET + 2)
                # 新结构把同一 4 字节一次读成一个 i32。
                value_i32 = _i32(layer, METADATA_OFFSET)
                # mode 位于 +0x0A，读取它只是为了做交叉统计。
                mode = layer[0x0A]

                # 累加各种分布。
                low_words[low_i16] += 1
                high_words[high_i16] += 1
                word_pairs[(low_i16, high_i16)] += 1
                i32_values[value_i32] += 1
                mode_value[(mode, value_i32)] += 1
                current_sequence.append(value_i32)
                layer_count += 1

                # 只保存有限数量的非默认示例，避免报告膨胀。
                if value_i32 not in (-1, 0) and len(examples) < 100:
                    examples.append({
                        "SHA256": sha256,
                        "代表来源": source.display_path,
                        "Section0索引": record_desc["索引"],
                        "Layer索引": layer_index,
                        "serialized_metadata_i32_06": value_i32,
                        "mode": mode,
                        "section1_image_index": _u16(layer, 0x00),
                        "x": _i16(layer, 0x02),
                        "y": _i16(layer, 0x04),
                    })

            # 空 record 也会留下空 tuple；这能完整反映当前 Section0 结构分布。
            sequence_counter[tuple(current_sequence)] += 1

    # “两个 i16 其实是一个有符号 i32”的关键不变量：
    # 对当前所有值，非负小值的高16位必须为0；-1的高/低16位都必须为-1。
    # 这里不依赖“只有 -1/0..48”这个具体域，而直接重算数学一致性。
    pair_i32_consistency = True
    # 遍历实际出现的每种 pair，而不是只看代表样本。
    for (low_i16, high_i16), _count in word_pairs.items():
        # 把两个 signed i16 重新拼成原始 32 位无符号 bit pattern。
        raw_u32 = (low_i16 & 0xFFFF) | ((high_i16 & 0xFFFF) << 16)
        # 按二补码转回 signed i32。
        reconstructed = raw_u32 if raw_u32 < 0x80000000 else raw_u32 - 0x100000000
        # 该完整 i32 必须真实出现在统计中；否则说明字段拆分/拼接逻辑有错误。
        if reconstructed not in i32_values:
            pair_i32_consistency = False
            break

    # 当前语料给出的更强观察：高半字仅 0 或 -1，并严格作为 i32 的符号扩展。
    sign_extension_only = all(
        (value >= 0 and high == 0) or (value < 0 and high == -1)
        for (low, high), _count in word_pairs.items()
        for value in [((low & 0xFFFF) | ((high & 0xFFFF) << 16))]
        # 上一行先拿到 raw u32；下面把 >=2^31 的值转 signed。
        for value in [value if value < 0x80000000 else value - 0x100000000]
    )

    # 把 Counter 变成稳定排序、适合 JSON 的普通列表。
    value_rows = [
        {"i32值": value, "出现次数": count}
        for value, count in sorted(i32_values.items())
    ]
    # pair 也稳定排序输出，直接展示旧低/高半字的关系。
    pair_rows = [
        {"低16位_i16": low, "高16位_i16": high, "出现次数": count}
        for (low, high), count in sorted(word_pairs.items())
    ]
    # 只保留最常见的 100 种 Section0 Layer 元数据序列，足够人工观察但不制造巨大文件。
    sequence_rows = [
        {"序列": list(sequence), "Section0记录数": count}
        for sequence, count in sequence_counter.most_common(100)
    ]
    # mode/value 交叉表用于证明值域与 render mode 并非同一个字段。
    mode_value_rows = [
        {"mode": mode, "i32值": value, "出现次数": count}
        for (mode, value), count in sorted(mode_value.items())
    ]

    # 最终 JSON 报告把“事实”和“结论边界”分开写，避免接档者误把推断当机器事实。
    return {
        "工具版本": VERSION,
        "资源根": str(root),
        "SF2出现数_散装加容器": len(sources),
        "唯一SHA256数": len(groups),
        "标准SF2唯一数": standard_sf2_count,
        "非标准SF2唯一数_当前为SAF": nonstandard_sf2_count,
        "Section0记录数": section0_record_count,
        "Layer总数": layer_count,
        "旧拆分低16位分布": [{"值": k, "次数": v} for k, v in sorted(low_words.items())],
        "旧拆分高16位分布": [{"值": k, "次数": v} for k, v in sorted(high_words.items())],
        "低高16位配对": pair_rows,
        "完整i32取值分布": value_rows,
        "完整i32最小值": min(i32_values) if i32_values else None,
        "完整i32最大值": max(i32_values) if i32_values else None,
        "低高半字重组为i32一致": pair_i32_consistency,
        "高16位严格表现为i32符号扩展": sign_extension_only,
        "mode与i32交叉分布": mode_value_rows,
        "常见Section0内Layer元数据序列_前100": sequence_rows,
        "非默认正值示例_最多100": examples,
        "解析失败": failures,
        "全部通过": (
            not failures
            and layer_count > 0
            and pair_i32_consistency
            and sign_extension_only
            and set(high_words).issubset({-1, 0})
        ),
        "结构结论": (
            "Layer +0x06..+0x09 应按一个 little-endian signed i32 序列化字段保存；"
            "旧版把 +0x06/+0x08 拆成两个独立 i16 的结构定义撤销。"
        ),
        "证据边界": (
            "当前语料只证明字段宽度、符号表现和实际值域，不证明作者业务名称。"
            "运行时是否消费由“验证RPG”子命令另行限定；即使当前 canonical renderer 不直读，也必须原样保留。"
        ),
    }


def _parse_pe_mapping(data: bytes) -> tuple[int, list[dict[str, int | str]]]:
    """读取最小 PE 信息，用虚拟地址定位文件字节。"""

    # DOS 头 0x3C 存 PE 头文件偏移；文件过短直接拒绝。
    if len(data) < 0x40:
        raise ValueError("文件过短，不是有效 PE")
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    # PE signature 必须是 'PE\0\0'。
    if pe_offset + 24 > len(data) or data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise ValueError("没有有效 PE 签名")
    # COFF Header +6 是 section 数量。
    section_count = struct.unpack_from("<H", data, pe_offset + 6)[0]
    # COFF Header +20 是 Optional Header 大小。
    optional_size = struct.unpack_from("<H", data, pe_offset + 20)[0]
    optional = pe_offset + 24
    # PE32 Optional Header +28 是 ImageBase。
    image_base = struct.unpack_from("<I", data, optional + 28)[0]
    # Section Table 紧跟 Optional Header。
    section_table = optional + optional_size
    sections: list[dict[str, int | str]] = []
    # 每个 IMAGE_SECTION_HEADER 固定 40 字节。
    for index in range(section_count):
        entry = section_table + index * 40
        if entry + 40 > len(data):
            raise ValueError("PE Section Table 越界")
        # 名称最多8字节，以NUL结束。
        name = data[entry:entry + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
        # +8/+12/+16/+20 分别是 VirtualSize/VirtualAddress/SizeOfRawData/PointerToRawData。
        virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from("<IIII", data, entry + 8)
        sections.append({
            "name": name,
            "virtual_size": virtual_size,
            "virtual_address": virtual_address,
            "raw_size": raw_size,
            "raw_pointer": raw_pointer,
        })
    return image_base, sections


def _va_to_file_offset(address: int, image_base: int, sections: list[dict[str, int | str]]) -> int:
    """把运行时 VA 转成 PE 文件内 offset。"""

    # RVA 就是 VA 减 ImageBase。
    rva = address - image_base
    # 在所有 Section 里找能覆盖该 RVA 的一段。
    for section in sections:
        start = int(section["virtual_address"])
        # 用 max(VirtualSize, RawSize) 能覆盖常见 PE 对齐差异。
        span = max(int(section["virtual_size"]), int(section["raw_size"]))
        if start <= rva < start + span:
            return int(section["raw_pointer"]) + (rva - start)
    raise ValueError(f"地址不位于任何 PE section：0x{address:08X}")


def _read_va(data: bytes, address: int, size: int, image_base: int, sections: list[dict[str, int | str]]) -> bytes:
    """按运行时 VA 读取固定长度机器码。"""

    offset = _va_to_file_offset(address, image_base, sections)
    end = offset + size
    if end > len(data):
        raise ValueError(f"读取 0x{address:08X} 时越过文件末尾")
    return data[offset:end]


def _direct_e8_callers(data: bytes, target: int, image_base: int, sections: list[dict[str, int | str]]) -> list[int]:
    """只在 .text 内枚举 opcode E8 的 direct near call，并返回命中 target 的 caller VA。"""

    # 找 .text；没有 .text 就不能做“全代码段 direct caller”结论。
    text = next((s for s in sections if s["name"] == ".text"), None)
    if text is None:
        raise ValueError("PE 中没有 .text section")
    raw_pointer = int(text["raw_pointer"])
    raw_size = int(text["raw_size"])
    virtual_address = int(text["virtual_address"])
    block = data[raw_pointer:raw_pointer + raw_size]
    callers: list[int] = []
    # E8 后面有 4 字节相对位移，所以最后4字节不可能是完整 call。
    for index in range(0, max(0, len(block) - 4)):
        if block[index] != 0xE8:
            continue
        rel = struct.unpack_from("<i", block, index + 1)[0]
        caller = image_base + virtual_address + index
        resolved = caller + 5 + rel
        if resolved == target:
            callers.append(caller)
    return callers


def verify_rpg(path: Path) -> dict[str, Any]:
    """重放两套 canonical SF2 Layer 构造/绘制链的机器码证据。"""

    path = path.resolve()
    if not path.is_file():
        raise FileNotFoundError(f"RPG.exe 不存在：{path}")
    data = path.read_bytes()
    image_base, sections = _parse_pe_mapping(data)
    if image_base != EXPECTED_IMAGE_BASE:
        raise ValueError(f"ImageBase 非预期：0x{image_base:08X}")

    # 每条断言都只覆盖本结论真正依赖的关键指令，不拿大块函数字节冒充精准证据。
    assertions = [
        MachineAssertion(0x00407279, "8B 51 48 33 F6 66 8B 72 54 3B C6 7D 24 8D 34 C5 00 00 00 00 03 D0 2B F0 89 41 54 33 C0 8D 54 B2 5E 89 51 50", "第一套对象：按 Section0+0x5E+29*index 构造 Layer 指针并写 object+0x50"),
        MachineAssertion(0x00407557, "8B 46 50 0F BF 48 0F 0F BF 50 0D 51 52 0F BF 48 0B 51 75 0F 33 D2 8B CE 8A 50 0A", "第一套绘制入口：从 object+0x50 的 Layer 读取 +0x0F/+0x0D/+0x0B/+0x0A"),
        MachineAssertion(0x00407608, "8B 47 50 89 54 24 24 0F BF 48 02 2B CD 8B 2F 03 CD 0F BF 68 04", "第一套 RGB555 类像素路径：Layer 只继续读取位置 +0x02/+0x04"),
        MachineAssertion(0x00407AE8, "8B 47 50 89 54 24 24 0F BF 48 02 2B CD 8B 2F 03 CD 0F BF 68 04", "第一套 RGB565 类像素路径：Layer 只继续读取位置 +0x02/+0x04"),
        MachineAssertion(0x00428EE1, "8B 51 34 8B 44 24 08 33 F6 66 8B 72 54 3B C6 7C 06 32 C0 5E C2 04 00 8D 34 C5 00 00 00 00 03 D0 2B F0 89 41 40 B0 01 8D 54 B2 5E 5E 89 51 3C", "第二套对象：同样按 Section0+0x5E+29*index 构造 Layer 指针并写 object+0x3C"),
        MachineAssertion(0x00429107, "8B 46 3C 33 C9 66 8B 08 51 8B CE", "第二套主 compositor：先从 Layer +0x00 取得 Section1 image index"),
        MachineAssertion(0x004291F1, "8B 7E 3C 8B 5E 44 0F BF 47 0F 0F BF 57 0D 50 52 0F BF 47 0B 33 D2 50 8A 57 0A", "第二套主 compositor：Layer 参数读取为 +0x0F/+0x0D/+0x0B/+0x0A"),
        MachineAssertion(0x004292C2, "8B 5E 3C 52 50 8B 47 08 99 2B C2 8B D0 0F BF 43 04", "第二套嵌套合成路径：Layer 指针继续读取位置 +0x04"),
        MachineAssertion(0x0042933F, "8B 46 3C 33 DB 0F BF 50 02 89 16 0F BF 50 04 89 56 04", "第二套直接绘制路径：Layer 位置读取为 +0x02/+0x04"),
        MachineAssertion(0x00429381, "0F BF 48 0F 0F BF 50 0D 51 52 0F BF 48 0B 33 D2 51 8A 50 0A", "第二套直接绘制路径：Layer 参数再次只读 +0x0F/+0x0D/+0x0B/+0x0A"),
    ]

    rows: list[dict[str, Any]] = []
    for item in assertions:
        expected = bytes.fromhex(item.expected_hex)
        actual = _read_va(data, item.address, len(expected), image_base, sections)
        rows.append({
            "地址": f"0x{item.address:08X}",
            "说明": item.meaning,
            "期望": expected.hex(" ").upper(),
            "实际": actual.hex(" ").upper(),
            "状态": "PASS" if actual == expected else "FAIL",
        })

    # 两个 Layer pointer constructor 的 direct E8 callers 也做全 .text 穷举。
    callers_407270 = _direct_e8_callers(data, 0x00407270, image_base, sections)
    callers_428ed0 = _direct_e8_callers(data, 0x00428ED0, image_base, sections)
    expected_407270 = [0x0040723E, 0x00407540]
    expected_428ed0 = [0x00428EB9, 0x00429102, 0x0042916E]

    return {
        "工具版本": VERSION,
        "文件": str(path),
        "SHA256": _sha256_bytes(data),
        "ImageBase": f"0x{image_base:08X}",
        "机器码断言": rows,
        "机器码断言数量": len(rows),
        "机器码断言全部通过": all(row["状态"] == "PASS" for row in rows),
        "Layer构造器0x00407270_direct_E8_callers": [f"0x{x:08X}" for x in callers_407270],
        "Layer构造器0x00428ED0_direct_E8_callers": [f"0x{x:08X}" for x in callers_428ed0],
        "direct_caller集合符合预期": callers_407270 == expected_407270 and callers_428ed0 == expected_428ed0,
        "运行时直读结论": (
            "同版EXE两套已确认标准SF2 Layer对象家族都按29字节构造Layer指针；"
            "已确认绘制链直接读取 +0x00/+0x02/+0x04/+0x0A/+0x0B/+0x0D/+0x0F，"
            "没有在这些 canonical direct render paths 中读取 +0x06..+0x09 i32。"
        ),
        "证据边界": (
            "这是两套已确认Layer构造/绘制家族的direct machine-code边界；"
            "不能形式化排除未来发现的别名指针、间接调用或编辑器/未执行路径，也不能据此删除该i32字段。"
        ),
        "全部通过": (
            all(row["状态"] == "PASS" for row in rows)
            and callers_407270 == expected_407270
            and callers_428ed0 == expected_428ed0
        ),
    }


def _write_json(path: Path | None, payload: dict[str, Any]) -> None:
    """有指定 --json 时写 UTF-8 JSON；没指定就什么都不做。"""

    if path is None:
        return
    path = path.resolve()
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def _write_value_csv(path: Path | None, corpus: dict[str, Any]) -> None:
    """把 i32 值域写成严格 CSV，便于 Excel/脚本复核。"""

    if path is None:
        return
    path = path.resolve()
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(["i32值", "出现次数"])
        for row in corpus["完整i32取值分布"]:
            writer.writerow([row["i32值"], row["出现次数"]])


def build_parser() -> argparse.ArgumentParser:
    """创建命令行参数结构。"""

    parser = argparse.ArgumentParser(description="《幽城幻剑录》SF2 Layer 32位序列化元数据调查器（固化39，只读）")
    parser.add_argument("--版本", action="version", version=f"SF2 Layer元数据调查器 {VERSION}")
    sub = parser.add_subparsers(dest="command", required=True)

    corpus = sub.add_parser("语料", help="扫描 multimedia 类目录：散装+容器内 SF2，按 SHA 去重统计 Layer +0x06 i32")
    corpus.add_argument("资源根", type=Path)
    corpus.add_argument("--json", type=Path)
    corpus.add_argument("--csv", type=Path, help="输出完整 i32 值域计数 CSV")

    rpg = sub.add_parser("验证RPG", help="重放两套 canonical Layer 构造/绘制家族机器码断言")
    rpg.add_argument("RPG", type=Path)
    rpg.add_argument("--json", type=Path)

    combined = sub.add_parser("综合", help="一次执行全语料结构验证 + RPG.exe 运行时直读边界验证")
    combined.add_argument("资源根", type=Path)
    combined.add_argument("RPG", type=Path)
    combined.add_argument("--json", type=Path)
    combined.add_argument("--csv", type=Path, help="输出完整 i32 值域计数 CSV")
    return parser


def main() -> int:
    """命令行入口。"""

    parser = build_parser()
    args = parser.parse_args()

    if args.command == "语料":
        result = inspect_corpus(args.资源根)
        _write_json(args.json, result)
        _write_value_csv(args.csv, result)
        print(json.dumps(result, ensure_ascii=False, indent=2))
        return 0 if result["全部通过"] else 2

    if args.command == "验证RPG":
        result = verify_rpg(args.RPG)
        _write_json(args.json, result)
        print(json.dumps(result, ensure_ascii=False, indent=2))
        return 0 if result["全部通过"] else 2

    if args.command == "综合":
        corpus_result = inspect_corpus(args.资源根)
        rpg_result = verify_rpg(args.RPG)
        result = {
            "工具版本": VERSION,
            "语料": corpus_result,
            "RPG静态验证": rpg_result,
            "全部通过": corpus_result["全部通过"] and rpg_result["全部通过"],
            "固化39结论": (
                "29-byte SF2 Layer 的 +0x06..+0x09 是一个 little-endian signed i32 序列化元数据字段；"
                "当前全部505份唯一标准SF2语料中，其高16位严格只是完整i32的符号扩展。"
                "同版EXE两套已确认canonical标准SF2绘制家族均不直接读取该i32，故兼容引擎必须保存但不得强命名业务语义。"
            ),
        }
        _write_json(args.json, result)
        _write_value_csv(args.csv, corpus_result)
        print(json.dumps(result, ensure_ascii=False, indent=2))
        return 0 if result["全部通过"] else 2

    parser.error("未知命令")
    return 2


if __name__ == "__main__":
    # 只有用户直接运行本文件时才进入 main；被其他调查器 import 时不会自动执行。
    raise SystemExit(main())
