#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""《幽城幻剑录》Battle「戰場脫逃」成功阈值与 LayoutGR +0x9C 调查器。

这个脚本只做“只读静态核对”，不会修改 RPG.exe、ENC 资源或任何游戏文件。

为什么需要一个独立工具：
1. 历史固化18～22已经把 Battle RNG 的 direct caller 全部分类过，不能在固化62重复研究同一件事；
2. 历史接档仍明确留下一个小而关键的未知：FIRTTECH 0x1EA「戰場脫逃」调用
   0x424560 得到的阈值到底来自哪里、是什么业务意义；
3. 这个脚本把 EXE 机器码、LayoutGR 250条 canonical 数据和 FIRTTECH 作者名称三条证据链合并，
   让“逃跑成功率”不再只靠名称猜测；
4. 输出 JSON/CSV 可以在以后重新拿完全原版、研究版或其它版本做自动回归。

面向初学者的阅读提示：
- VA（Virtual Address）可以理解成“程序运行后看到的地址”；
- PE 文件里的数据并不是直接按 VA 排列，因此代码要先把 VA 换算成文件偏移；
- x86 的 `CALL rel32` 保存的是“从下一条指令再走多少字节”，所以扫描调用点时必须计算目标地址；
- Public ENC 不是压缩，而是每个字节与 `index % 255` 做 XOR；解密后才可以按固定记录大小解析；
- LayoutGR 每条记录 508 bytes，+0x44 是已确认的 CP950 名称，+0x9C 是本节点要闭合的字段；
- FIRTTECH 每条记录 560 bytes，0x1EA 的作者名称是「戰場脫逃」。

兼容实现最重要的结论由工具验证，而不是写死在说明里：
`success = (legacy_rand15() % 100) < layoutgr_record.field_9c`

注意：字段 +0x9C 的作者源码列名没有找到，因此工具使用中性的兼容层名称
`escape_success_threshold`，不能把这个名称冒充成原作者正式数据库列名。
"""

from __future__ import annotations

# argparse 用来处理命令行参数。例如 `--original-exe xxx` 会被解析成一个 Path。
import argparse
# csv 用来把 250 条 LayoutGR 记录输出成可以直接用表格软件查看的 CSV。
import csv
# hashlib 用来计算 SHA-256，保证“本轮输入”和“以后重放输入”可以精确比对。
import hashlib
# json 用来保存完整机器结论和所有断言结果。
import json
# struct 用来读取 PE 头里的 little-endian 整数。
import struct
# Counter 用来统计 +0x9C 的 0/30/50/100 分布。
from collections import Counter
# dataclass 让“PE section”和“LayoutGR record”有清楚的字段，而不是到处用魔法下标。
from dataclasses import dataclass
# Path 让路径拼接、读写文件更直观，也能避免手工处理 Windows/Linux 斜杠差异。
from pathlib import Path
# Any 只用于 JSON 数据结构的类型提示，不改变运行逻辑。
from typing import Any


# 工具版本只代表这个脚本自己的冻结版本，不代表游戏版本。
TOOL_VERSION = "0.7D.62"
# 完全原版 RPG.exe.org 的可信 SHA-256；用户已经确认它是完全未修改原版。
EXPECTED_ORIGINAL_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
# 历史研究版 RPG.exe 的 SHA-256。它只用于双版本等价比较，不作为原版行为来源。
EXPECTED_RESEARCH_SHA256 = "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"

# LayoutGR 几何已经在早期节点闭合：250 条，每条 508 bytes。
LAYOUTGR_RECORD_SIZE = 508
LAYOUTGR_RECORD_COUNT = 250
# FIRTTECH 几何已经闭合：500 条，每条 560 bytes。
FIRTTECH_RECORD_SIZE = 560
FIRTTECH_RECORD_COUNT = 500
# 「戰場脫逃」的 raw FIRTTECH ID。
ESCAPE_FIRTTECH_ID = 0x1EA

# LayoutGR +0x44 是 CP950 名称字段。
LAYOUTGR_NAME_OFFSET = 0x44
# 这个长度只用于从记录中安全找 NUL；名称区到后续数值区有足够空间。
LAYOUTGR_NAME_MAX = 0x28
# 本节点要闭合的字段偏移。
LAYOUTGR_ESCAPE_THRESHOLD_OFFSET = 0x9C
# +0x18 是已经确认的 LayoutTeam 候选数量；输出 CSV 时一起保留，方便区分场景组类型。
LAYOUTGR_TEAM_CHOICE_COUNT_OFFSET = 0x18

# FIRTTECH +0x04 是作者名称。
FIRTTECH_NAME_OFFSET = 0x04
FIRTTECH_NAME_MAX = 0x10
# 作者说明位于 +0x17C 到记录末尾。
FIRTTECH_DESCRIPTION_OFFSET = 0x17C
FIRTTECH_DESCRIPTION_SIZE = FIRTTECH_RECORD_SIZE - FIRTTECH_DESCRIPTION_OFFSET

# PE 运行地址基址并不硬编码成 0x400000；后面会从 Optional Header 实际读取。
# 下面这些 VA 是完全原版里已经人工反汇编过、由本工具继续机器验证的固定入口。
VA_ESCAPE_BRANCH_START = 0x0042227B
VA_ESCAPE_THRESHOLD_CALL = 0x0042229B
VA_ESCAPE_RNG_CALL = 0x004222A2
VA_ESCAPE_HELPER = 0x00424560
VA_RNG_FUNCTION = 0x00451565


@dataclass(frozen=True)
class Section:
    """保存一个 PE section 的最小映射信息。

    初学者可以把 section 想成“文件里的一个区域”。例如 `.text` 放机器码，`.rdata` 放只读数据。
    `virtual_address` 是它加载到内存后的 RVA，`raw_offset` 是它在硬盘文件中的起点。
    """

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass(frozen=True)
class LayoutRecord:
    """保存一条 LayoutGR 记录中本节点真正需要的字段。"""

    index: int
    name: str
    team_choice_count: int
    escape_success_threshold: int
    record_sha256: str


class PeImage:
    """非常小的 32-bit PE 只读映射器。

    我们没有依赖 pefile/capstone，这样工具可以在干净 Python 环境直接运行。
    它只实现本节点需要的三件事：
    1. 读取 ImageBase 和 section 表；
    2. 把 VA 换算成文件偏移；
    3. 读取指定 VA 的原始机器码。
    """

    def __init__(self, path: Path) -> None:
        # 先把整个 EXE 读进内存。后续操作都针对 bytes，不会写回磁盘。
        self.path = path
        self.data = path.read_bytes()

        # DOS 头 0x3C 位置保存 PE header 的文件偏移（e_lfanew）。
        if len(self.data) < 0x40:
            raise ValueError(f"{path} 太小，不可能是有效 PE")
        self.pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]

        # PE header 必须以 ASCII `PE\0\0` 开头。若不匹配，后面的地址换算都没有意义。
        if self.data[self.pe_offset:self.pe_offset + 4] != b"PE\0\0":
            raise ValueError(f"{path} 缺少 PE 签名")

        # COFF header 紧跟 PE 签名。+2 是 NumberOfSections，+16 是 OptionalHeader 大小。
        coff = self.pe_offset + 4
        self.number_of_sections = struct.unpack_from("<H", self.data, coff + 2)[0]
        self.optional_header_size = struct.unpack_from("<H", self.data, coff + 16)[0]

        # Optional Header 紧跟 20-byte COFF header。
        optional = coff + 20
        magic = struct.unpack_from("<H", self.data, optional)[0]
        # 本游戏是 PE32，magic 应为 0x10B。PE32+ 的字段位置不同，因此不能混用。
        if magic != 0x10B:
            raise ValueError(f"{path} 不是预期的 PE32：magic=0x{magic:X}")

        # PE32 Optional Header +0x1C 保存 ImageBase。幽城原版应为 0x00400000。
        self.image_base = struct.unpack_from("<I", self.data, optional + 0x1C)[0]

        # section table 从 Optional Header 末尾开始，每项固定 40 bytes。
        section_table = optional + self.optional_header_size
        sections: list[Section] = []
        for section_index in range(self.number_of_sections):
            entry = section_table + section_index * 40
            raw_name = self.data[entry:entry + 8].split(b"\0", 1)[0]
            name = raw_name.decode("ascii", errors="replace")
            virtual_size = struct.unpack_from("<I", self.data, entry + 8)[0]
            virtual_address = struct.unpack_from("<I", self.data, entry + 12)[0]
            raw_size = struct.unpack_from("<I", self.data, entry + 16)[0]
            raw_offset = struct.unpack_from("<I", self.data, entry + 20)[0]
            sections.append(
                Section(
                    name=name,
                    virtual_address=virtual_address,
                    virtual_size=virtual_size,
                    raw_offset=raw_offset,
                    raw_size=raw_size,
                )
            )
        self.sections = tuple(sections)

    def va_to_offset(self, va: int) -> int:
        """把运行时 VA 换算成 EXE 文件中的偏移。"""

        # RVA = VA - ImageBase。比如 0x424560 在 ImageBase 0x400000 下 RVA 是 0x24560。
        rva = va - self.image_base
        if rva < 0:
            raise ValueError(f"VA 0x{va:X} 小于 ImageBase 0x{self.image_base:X}")

        # 遍历 section，找到这个 RVA 落在哪一个区间。
        for section in self.sections:
            # 映射时取 virtual/raw 两种尺寸的较大值，可以兼容末尾存在对齐填充的 section。
            mapped_size = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + mapped_size:
                inside = rva - section.virtual_address
                # 不能让“内存虚拟区”越过硬盘 raw bytes；本工具只读文件实际存在的数据。
                if inside >= section.raw_size:
                    raise ValueError(
                        f"VA 0x{va:X} 位于 {section.name} 的虚拟尾部，文件中没有对应 raw bytes"
                    )
                return section.raw_offset + inside

        # 如果任何 section 都没包含这个地址，就说明调用者传入了错误 VA。
        raise ValueError(f"VA 0x{va:X} 不属于任何 PE section")

    def read_va(self, va: int, size: int) -> bytes:
        """只读取得某段机器码。"""

        offset = self.va_to_offset(va)
        end = offset + size
        if end > len(self.data):
            raise ValueError(f"VA 0x{va:X} 读取 {size} bytes 会越过文件末尾")
        return self.data[offset:end]

    def text_section(self) -> tuple[int, bytes]:
        """返回 `.text` 的起始 VA 和 raw bytes，供 direct CALL 穷举使用。"""

        for section in self.sections:
            if section.name == ".text":
                start_va = self.image_base + section.virtual_address
                data = self.data[section.raw_offset:section.raw_offset + section.raw_size]
                return start_va, data
        raise ValueError(f"{self.path} 没有 .text section")


# 计算 bytes 的 SHA-256；独立写成函数可以避免每个地方重复三行代码。
def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


# 计算文件 SHA-256。这里直接复用上面的 bytes 版本，逻辑只有一个来源。
def sha256_file(path: Path) -> str:
    return sha256_bytes(path.read_bytes())


# Public ENC 的已确认解密算法：每个 byte 与当前位置 index%255 做 XOR。
def decode_public_enc(raw: bytes) -> bytes:
    return bytes(value ^ (index % 255) for index, value in enumerate(raw))


# 从一个固定长度字段中读取 NUL 结尾 CP950 字符串。
def decode_cp950_cstring(field: bytes) -> str:
    # `split(..., 1)[0]` 只取第一个 NUL 前的内容，后面的固定宽度填充不属于正文。
    body = field.split(b"\0", 1)[0]
    return body.decode("cp950", errors="replace")


# little-endian signed 32-bit reader。LayoutGR 的数值字段由原版 `mov eax,[...]` 读取。
def i32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<i", data, offset)[0]


# x86 `E8 rel32` 的目标 = call地址 + 5 + signed rel32。
def decode_rel32_call_target(pe: PeImage, call_va: int) -> int:
    instruction = pe.read_va(call_va, 5)
    if instruction[0] != 0xE8:
        raise ValueError(f"0x{call_va:X} 不是 E8 direct CALL：{instruction.hex(' ')}")
    relative = int.from_bytes(instruction[1:5], "little", signed=True)
    return call_va + 5 + relative


# 穷举 .text 中所有 E8 rel32，并留下目标等于指定函数的调用点。
def scan_direct_callers(pe: PeImage, target_va: int) -> list[int]:
    text_va, text = pe.text_section()
    callers: list[int] = []
    # 最后4个字节不可能再容纳完整 E8+rel32，因此循环到 len(text)-4 即可。
    for index in range(0, len(text) - 4):
        if text[index] != 0xE8:
            continue
        relative = int.from_bytes(text[index + 1:index + 5], "little", signed=True)
        call_va = text_va + index
        if call_va + 5 + relative == target_va:
            callers.append(call_va)
    return callers


# 一个统一的机器码断言函数：既返回 JSON 记录，也不会在失败时悄悄继续。
def check_bytes(pe: PeImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    expected = bytes.fromhex(expected_hex)
    actual = pe.read_va(va, len(expected))
    passed = actual == expected
    return {
        "va": f"0x{va:08X}",
        "meaning": meaning,
        "expected_hex": expected.hex(" "),
        "actual_hex": actual.hex(" "),
        "pass": passed,
    }


# direct CALL 断言除了检查 opcode，还检查算出来的 callee 地址。
def check_call(pe: PeImage, call_va: int, expected_target: int, meaning: str) -> dict[str, Any]:
    actual_target = decode_rel32_call_target(pe, call_va)
    return {
        "call_va": f"0x{call_va:08X}",
        "meaning": meaning,
        "expected_target": f"0x{expected_target:08X}",
        "actual_target": f"0x{actual_target:08X}",
        "pass": actual_target == expected_target,
    }


# 读取并解密 LayoutGR；几何不对就立即失败，避免用错文件还继续得出“结论”。
def load_layoutgr(path: Path) -> tuple[bytes, list[LayoutRecord]]:
    raw = path.read_bytes()
    decoded = decode_public_enc(raw)
    expected_size = LAYOUTGR_RECORD_SIZE * LAYOUTGR_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(
            f"LayoutGR 几何错误：{len(decoded)} != {LAYOUTGR_RECORD_COUNT}*{LAYOUTGR_RECORD_SIZE}={expected_size}"
        )

    records: list[LayoutRecord] = []
    for index in range(LAYOUTGR_RECORD_COUNT):
        # 每条 record 的起点就是 index*508。
        start = index * LAYOUTGR_RECORD_SIZE
        record = decoded[start:start + LAYOUTGR_RECORD_SIZE]
        name = decode_cp950_cstring(
            record[LAYOUTGR_NAME_OFFSET:LAYOUTGR_NAME_OFFSET + LAYOUTGR_NAME_MAX]
        )
        team_choice_count = i32(record, LAYOUTGR_TEAM_CHOICE_COUNT_OFFSET)
        threshold = i32(record, LAYOUTGR_ESCAPE_THRESHOLD_OFFSET)
        records.append(
            LayoutRecord(
                index=index,
                name=name,
                team_choice_count=team_choice_count,
                escape_success_threshold=threshold,
                record_sha256=sha256_bytes(record),
            )
        )
    return decoded, records


# 读取 FIRTTECH 0x1EA，确认“戰場脫逃”作者名称确实来自资源，不来自我们自己命名。
def load_escape_firttech(path: Path) -> dict[str, Any]:
    raw = path.read_bytes()
    decoded = decode_public_enc(raw)
    expected_size = FIRTTECH_RECORD_SIZE * FIRTTECH_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(
            f"FIRTTECH 几何错误：{len(decoded)} != {FIRTTECH_RECORD_COUNT}*{FIRTTECH_RECORD_SIZE}={expected_size}"
        )

    start = ESCAPE_FIRTTECH_ID * FIRTTECH_RECORD_SIZE
    record = decoded[start:start + FIRTTECH_RECORD_SIZE]
    raw_id = i32(record, 0x00)
    name = decode_cp950_cstring(record[FIRTTECH_NAME_OFFSET:FIRTTECH_NAME_OFFSET + FIRTTECH_NAME_MAX])
    description = decode_cp950_cstring(
        record[FIRTTECH_DESCRIPTION_OFFSET:FIRTTECH_DESCRIPTION_OFFSET + FIRTTECH_DESCRIPTION_SIZE]
    )
    return {
        "physical_index": ESCAPE_FIRTTECH_ID,
        "raw_id": raw_id,
        "name": name,
        "description": description,
        "record_sha256": sha256_bytes(record),
        # 额外保留几个现有已知字段，方便未来判断“这条记录有没有被资源修改”。
        "class_0x18": i32(record, 0x18),
        "raw_field_0x20": i32(record, 0x20),
        "result_mode_0x44": i32(record, 0x44),
    }


# 根据名称和物理记录区间给出“证据观察分类”。它不是作者枚举，只方便 CSV 人工审查。
def observed_layout_bucket(record: LayoutRecord) -> str:
    index = record.index
    if index == 0 and record.name == "Test":
        return "测试记录"
    if 1 <= index <= 48 and record.name:
        return "有名普通区域/迷宫组"
    if 101 <= index <= 185 and record.name:
        return "有名剧情/脚本战斗组"
    if 201 <= index <= 243 and record.name:
        return "有名大地图/跨区域遭遇组"
    if not record.name:
        return "空名称预留记录"
    return "其它有名记录"


# 统计 canonical 表的字段分布，并验证非常整齐的四个有名区段。
def analyze_layout_distribution(records: list[LayoutRecord]) -> dict[str, Any]:
    all_counter = Counter(record.escape_success_threshold for record in records)
    named_records = [record for record in records if record.name]
    named_counter = Counter(record.escape_success_threshold for record in named_records)

    # 这四个 expected 区段并不是凭“想当然”写的，而是本轮先扫描完整250条后得到的 canonical 事实。
    # 工具再次运行时会逐条验证；以后资源只要变了一条，就会明确 FAIL。
    expected_named_bands = [
        (0, 0, 100, "Test"),
        (1, 48, 30, None),
        (101, 185, 0, None),
        (201, 243, 50, None),
    ]
    band_checks: list[dict[str, Any]] = []
    for first, last, expected_value, required_name in expected_named_bands:
        part = records[first:last + 1]
        names_nonempty = all(bool(record.name) for record in part)
        thresholds_ok = all(record.escape_success_threshold == expected_value for record in part)
        exact_name_ok = True
        if required_name is not None:
            exact_name_ok = len(part) == 1 and part[0].name == required_name
        band_checks.append(
            {
                "range": f"{first}..{last}",
                "expected_threshold": expected_value,
                "all_names_nonempty": names_nonempty,
                "required_exact_name": required_name,
                "exact_name_ok": exact_name_ok,
                "thresholds_ok": thresholds_ok,
                "pass": names_nonempty and thresholds_ok and exact_name_ok,
            }
        )

    return {
        "all_value_counts": {str(key): all_counter[key] for key in sorted(all_counter)},
        "named_value_counts": {str(key): named_counter[key] for key in sorted(named_counter)},
        "unique_values": sorted(all_counter),
        "named_record_count": len(named_records),
        "band_checks": band_checks,
        "canonical_distribution_pass": (
            dict(all_counter) == {0: 100, 30: 74, 50: 43, 100: 33}
            and dict(named_counter) == {0: 85, 30: 48, 50: 43, 100: 1}
            and all(item["pass"] for item in band_checks)
        ),
    }


# 计算15-bit RNG 输出空间中，执行 `%100 < threshold` 后有多少个值会成功。
def analyze_modulo_bias(thresholds: list[int]) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    total = 1 << 15
    for threshold in thresholds:
        success_count = sum(1 for value in range(total) if value % 100 < threshold)
        rows.append(
            {
                "threshold": threshold,
                "success_outputs_in_0_32767": success_count,
                "total_outputs": total,
                "ratio": success_count / total,
                "ratio_percent": success_count * 100.0 / total,
                "note": (
                    "这是对15-bit输出值域做枚举后的算术比例；原机仍必须按 rand15()%100 比较，"
                    "不要改成浮点uniform百分比。"
                ),
            }
        )
    return rows


# 比较原版与研究版关键区间，证明本节点没有踩到历史研究补丁的10-byte差异。
def compare_key_ranges(original: PeImage, research: PeImage) -> list[dict[str, Any]]:
    ranges = [
        (0x0042227B, 0x004222C7, "戰場脫逃专用判定链"),
        (0x00424560, 0x00424589, "LayoutGR +0x9C getter"),
        (0x0045155B, 0x00451583, "RNG播种入口与rand15本体前段"),
    ]
    result: list[dict[str, Any]] = []
    for start, end, meaning in ranges:
        size = end - start
        original_bytes = original.read_va(start, size)
        research_bytes = research.read_va(start, size)
        result.append(
            {
                "start_va": f"0x{start:08X}",
                "end_va_exclusive": f"0x{end:08X}",
                "meaning": meaning,
                "size": size,
                "original_sha256": sha256_bytes(original_bytes),
                "research_sha256": sha256_bytes(research_bytes),
                "byte_identical": original_bytes == research_bytes,
            }
        )
    return result


# 把 LayoutGR 全表输出成 CSV，让文档审查者可以直接看到名字和阈值，而不是只相信汇总数字。
def write_layout_csv(path: Path, records: list[LayoutRecord]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.writer(handle, lineterminator="\r\n")
        writer.writerow(
            [
                "物理索引",
                "名称",
                "LayoutTeam候选数_加0x18",
                "脱逃成功阈值_加0x9C",
                "观察分类_非作者枚举",
                "记录SHA256",
            ]
        )
        for record in records:
            writer.writerow(
                [
                    record.index,
                    record.name,
                    record.team_choice_count,
                    record.escape_success_threshold,
                    observed_layout_bucket(record),
                    record.record_sha256,
                ]
            )


# 主分析函数把所有独立证据合并，并生成“是否PASS”的严格总门。
def analyze(args: argparse.Namespace) -> dict[str, Any]:
    original_path = Path(args.original_exe)
    research_path = Path(args.research_exe)
    layoutgr_path = Path(args.layoutgr)
    firttech_path = Path(args.firttech)

    # 输入SHA先保存下来。以后重放时，不需要靠文件名猜是不是同一份文件。
    original_sha = sha256_file(original_path)
    research_sha = sha256_file(research_path)
    layoutgr_raw_sha = sha256_file(layoutgr_path)
    firttech_raw_sha = sha256_file(firttech_path)

    # 创建PE映射器。这里只读，不会在EXE中打补丁。
    original_pe = PeImage(original_path)
    research_pe = PeImage(research_path)

    # 重新解密并解析 canonical LayoutGR，绝不依赖历史JSON直接抄结论。
    layoutgr_decoded, layout_records = load_layoutgr(layoutgr_path)
    # 重新解密 FIRTTECH，确认0x1EA的作者名称。
    escape_firttech = load_escape_firttech(firttech_path)

    # 机器码断言覆盖“识别技能ID → 取LayoutGR索引/表对象 → 调getter → RNG%100 → 严格小于阈值”。
    machine_checks = [
        check_bytes(
            original_pe,
            0x00422281,
            "81 39 EA 01 00 00",
            "当前FIRTTECH raw ID必须等于0x1EA才进入戰場脫逃专用判定",
        ),
        check_bytes(
            original_pe,
            0x00422289,
            "A1 48 1C 8E 00 8B 88 D8 00 00 00 51 8B 88 E8 00 00 00",
            "从0x8E1C48全局对象读取+0xD8当前LayoutGR索引并以+0xE8对象作为this",
        ),
        check_call(
            original_pe,
            VA_ESCAPE_THRESHOLD_CALL,
            VA_ESCAPE_HELPER,
            "调用0x424560取得当前LayoutGR+0x9C阈值",
        ),
        check_call(
            original_pe,
            VA_ESCAPE_RNG_CALL,
            VA_RNG_FUNCTION,
            "戰場脫逃成功判定固定消费一次legacy rand15",
        ),
        check_bytes(
            original_pe,
            0x004222A7,
            "99 B9 64 00 00 00 F7 F9 3B D7 7D 14",
            "RNG结果做有符号idiv 100取EDX余数，并以 remainder>=threshold 跳到失败路径",
        ),
        check_bytes(
            original_pe,
            0x004222B3,
            "8B 15 74 FD 89 00 C6 82 FC 05 00 00 01 C6 05 DC 96 46 00 00",
            "只有 remainder<threshold 才进入成功侧，写入两项Battle运行时标志",
        ),
        check_bytes(
            original_pe,
            VA_ESCAPE_HELPER,
            "8B 54 24 04 B8 64 00 00 00 85 D2 7F 08 56 8B 31 3B D6 5E 7D 11 8B 49 04 8B C2 C1 E0 07 2B C2 8B 84 81 9C 00 00 00 C2 04 00",
            "0x424560用508-byte记录步长直接读取LayoutGR record+0x9C；默认EAX先置100",
        ),
        check_bytes(
            original_pe,
            VA_RNG_FUNCTION,
            "A1 9C E2 46 00 69 C0 FD 43 03 00 05 C3 9E 26 00 A3 9C E2 46 00 C1 F8 10 25 FF 7F 00 00 C3",
            "legacy rand15严格为state=state*0x343FD+0x269EC3，返回高15位",
        ),
    ]

    # 0x424560 必须只有这一个 direct caller；这样 +0x9C 的当前EXE直接业务消费面才真正有限闭合。
    escape_helper_callers = scan_direct_callers(original_pe, VA_ESCAPE_HELPER)
    caller_check = {
        "target": f"0x{VA_ESCAPE_HELPER:08X}",
        "direct_callers": [f"0x{va:08X}" for va in escape_helper_callers],
        "expected": [f"0x{VA_ESCAPE_THRESHOLD_CALL:08X}"],
        "pass": escape_helper_callers == [VA_ESCAPE_THRESHOLD_CALL],
    }

    distribution = analyze_layout_distribution(layout_records)
    modulo_bias = analyze_modulo_bias(distribution["unique_values"])
    key_range_comparison = compare_key_ranges(original_pe, research_pe)

    # FIRTTECH作者名称必须精确匹配；这样“戰場脫逃”不是后人按效果自造名称。
    firttech_check = {
        "raw_id_matches_index": escape_firttech["raw_id"] == ESCAPE_FIRTTECH_ID,
        "author_name_is_escape": escape_firttech["name"] == "戰場脫逃",
        "description_is_empty": escape_firttech["description"] == "",
        "pass": (
            escape_firttech["raw_id"] == ESCAPE_FIRTTECH_ID
            and escape_firttech["name"] == "戰場脫逃"
            and escape_firttech["description"] == ""
        ),
    }

    # 这是本节点最重要的兼容语义，不使用“随机百分比函数”模糊表达，而保存精确整数运算顺序。
    compatibility_rule = {
        "raw_formula": "success = (legacy_rand15() % 100) < LayoutGR[current_layoutgr_index].field_0x9C",
        "canonical_field_values": distribution["unique_values"],
        "recommended_compatibility_name": "escape_success_threshold",
        "author_column_name": "UNKNOWN",
        "comparison_is_strict_less_than": True,
        "one_rng_draw_per_escape_attempt": True,
        "invalid_index_boundary": (
            "0x424560自身的legacy guard并不是可靠的现代数组边界检查；"
            "本节点只冻结已经由Battle建立链保证的canonical有效LayoutGR index输入。"
        ),
    }

    # 输入身份也是总PASS的一部分。研究版不用于决定原版行为，但必须确认是我们一直追踪的同一历史版本。
    input_checks = {
        "original_sha256": original_sha,
        "original_expected_sha256": EXPECTED_ORIGINAL_SHA256,
        "original_matches_expected": original_sha == EXPECTED_ORIGINAL_SHA256,
        "research_sha256": research_sha,
        "research_expected_sha256": EXPECTED_RESEARCH_SHA256,
        "research_matches_expected": research_sha == EXPECTED_RESEARCH_SHA256,
        "layoutgr_raw_sha256": layoutgr_raw_sha,
        "layoutgr_decoded_sha256": sha256_bytes(layoutgr_decoded),
        "firttech_raw_sha256": firttech_raw_sha,
        "original_image_base": f"0x{original_pe.image_base:08X}",
        "research_image_base": f"0x{research_pe.image_base:08X}",
    }

    total_pass = all(
        [
            input_checks["original_matches_expected"],
            input_checks["research_matches_expected"],
            all(check["pass"] for check in machine_checks),
            caller_check["pass"],
            distribution["canonical_distribution_pass"],
            firttech_check["pass"],
            all(item["byte_identical"] for item in key_range_comparison),
        ]
    )

    return {
        "tool_version": TOOL_VERSION,
        "status": "PASS" if total_pass else "FAIL",
        "scope": "固化62：Battle戰場脫逃成功阈值与LayoutGR+0x9C canonical边界",
        "inputs": {
            "original_exe": str(original_path.resolve()),
            "research_exe": str(research_path.resolve()),
            "layoutgr_enc": str(layoutgr_path.resolve()),
            "firttech_enc": str(firttech_path.resolve()),
        },
        "input_checks": input_checks,
        "escape_firttech": escape_firttech,
        "escape_firttech_check": firttech_check,
        "machine_checks": machine_checks,
        "escape_threshold_getter_direct_callers": caller_check,
        "layoutgr_distribution": distribution,
        "layoutgr_records": [
            {
                "index": record.index,
                "name": record.name,
                "team_choice_count_0x18": record.team_choice_count,
                "field_0x9C": record.escape_success_threshold,
                "observed_bucket_not_author_enum": observed_layout_bucket(record),
                "record_sha256": record.record_sha256,
            }
            for record in layout_records
        ],
        "rand15_mod_100_arithmetic": modulo_bias,
        "original_vs_research_key_ranges": key_range_comparison,
        "compatibility_rule": compatibility_rule,
        "boundaries": [
            "作者没有在当前资源或EXE中暴露LayoutGR+0x9C的正式列名；兼容层名称不是作者术语。",
            "历史固化18～22已经闭合56个direct RNG caller的静态分类，本节点没有重复把D8重新做一遍。",
            "本节点只证明0x424560的direct caller面；没有宣称所有可能的间接/数据驱动读取都不存在。",
            "canonical阈值0/30/50/100对应机器比较阈值；由于原版先rand15()%100，30和50并非用浮点uniform实现。",
            "0x424560的invalid-index legacy行为存在不安全边界，本节点不把异常输入行为提升为兼容API规范。",
        ],
        "implementation_contract": [
            "Battle建立时保留当前LayoutGR物理索引。",
            "执行FIRTTECH raw ID 0x1EA「戰場脫逃」时读取当前LayoutGR record+0x9C。",
            "严格消费一次legacy rand15，随后做整数余数 `%100`。",
            "余数严格小于阈值才成功；等于阈值必须失败。",
            "canonical 0/30/50/100必须原样保存，不要按场景名称重新推导。",
            "数据模型建议字段名escape_success_threshold，但序列化/证据层仍保留raw offset 0x9C和UNKNOWN作者列名。",
        ],
    }


# -----------------------------------------------------------------------------
# 给初学者的“从输入到结论”逐步阅读地图
# -----------------------------------------------------------------------------
# 下面这一大段只有注释，不会执行，也不会改变脚本任何运算结果。
# 它的目的，是让只学过一天编程的人也能把前面的函数按正确顺序串起来。
# 第01步：main() 先读取命令行参数，知道四个输入文件和两个输出文件放在哪里。
# 第02步：analyze() 用 sha256_file() 给每个关键输入做“数字指纹”。
# 第03步：完全原版 EXE 的指纹必须匹配固定 SHA，否则不能把它当成可信 Oracle。
# 第04步：历史研究版 EXE 也要匹配固定 SHA，但它只负责“差异边界比较”，不决定原版规则。
# 第05步：PeImage 会读取 PE 文件头，而不是假设所有程序都固定以同样方式排布。
# 第06步：PeImage 先从 DOS 头找到 PE 头，再读取 section 数量、ImageBase 和 section 表。
# 第07步：以后看到一个运行地址 VA 时，va_to_offset() 才能找到它在硬盘 EXE 里的真实位置。
# 第08步：read_va() 始终只读 bytes；脚本没有任何把 bytes 写回 EXE 的代码。
# 第09步：load_layoutgr() 读取 Layoutgr.enc 后，先执行 period-255 XOR 解码。
# 第10步：解码后长度必须正好是 250*508；不一致就说明拿错文件或资源已变化。
# 第11步：每条 LayoutGR 的物理起点是 index*508，因此字段 +0x9C 的物理地址可稳定计算。
# 第12步：名称从 +0x44 读取，并使用原作繁体中文版对应的 CP950 编码解码。
# 第13步：名称只用于资源语料交叉验证，机器结论仍由 EXE 的读取与比较指令决定。
# 第14步：load_escape_firttech() 独立解码 Firttech.enc，找到物理索引 0x1EA。
# 第15步：记录内 raw ID 也必须是 0x1EA，避免把“物理索引”和“raw ID”误当永远相同。
# 第16步：作者名称必须精确为「戰場脫逃」，所以这个业务名不是调查者自己发明的。
# 第17步：0x422281 的 cmp 再从机器侧证明：只有 raw ID 0x1EA 才进入这段专用逻辑。
# 第18步：0x422289 开始从全局对象读取当前 LayoutGR 索引和 LayoutGR 管理对象。
# 第19步：0x42229B 直接调用 0x424560；scan_direct_callers() 还要确认它没有别的 direct caller。
# 第20步：0x424560 不是随机函数，它只是按 508-byte 记录步长读取 record+0x9C。
# 第21步：因此“阈值来自 LayoutGR +0x9C”是地址级事实，不是根据字段数值猜出来的。
# 第22步：0x4222A2 才调用真正的 legacy rand15；一次脱逃尝试固定消费这一抽。
# 第23步：rand15 自身执行线性同余更新，再返回 state 的高15位，输出范围是 0..32767。
# 第24步：接着原版不是除以32768做浮点概率，而是用 idiv 100 直接取得整数余数。
# 第25步：余数必然落在 0..99；这是理解 +0x9C 为“0到100阈值”的关键机器依据。
# 第26步：cmp edx,edi 比较“余数”和“阈值”；jge 表示余数大于等于阈值时走失败侧。
# 第27步：所以成功条件必须写成 remainder < threshold，而不是 <=。
# 第28步：threshold=0 时，没有任何 0..99 的余数能小于0，因此 canonical 行为为必失败。
# 第29步：threshold=100 时，所有 0..99 的余数都小于100，因此 canonical 行为为必成功。
# 第30步：threshold=30 时，成功余数是0..29；threshold=50时成功余数是0..49。
# 第31步：analyze_modulo_bias() 额外枚举0..32767，是为了说明 `%100` 和理想浮点均匀概率不同。
# 第32步：这个枚举不会取代原版 RNG；兼容实现仍必须复刻 rand15()%100 的整数顺序。
# 第33步：LayoutGR 全表扫描不是只看几个“看起来像迷宫”的样本，而是250条全部解析。
# 第34步：全表只出现0、30、50、100四个阈值，这是 canonical 数据事实。
# 第35步：1..48 的有名区域记录全部是30，形成一个非常整齐的资源区段。
# 第36步：101..185 的有名剧情/脚本战斗记录全部是0，说明这类战斗禁止通过该技能成功脱逃。
# 第37步：201..243 的有名大地图/跨区域遭遇记录全部是50，形成另一整齐区段。
# 第38步：index0「Test」为100，表明同一个字段也能表达“必成功”阈值。
# 第39步：空名称预留记录也会输出到CSV，不能为了叙述方便从数据库里偷偷删掉。
# 第40步：CSV 的“观察分类”明确写着非作者枚举，避免后人把我们的分组名称误当原作字段。
# 第41步：字段的兼容层中性名可以叫 escape_success_threshold，便于代码阅读。
# 第42步：但序列化层仍要记住它来自 LayoutGR +0x9C，作者正式列名继续写 UNKNOWN。
# 第43步：如果以后出现改版 LayoutGR，工具不会用名称重新算阈值，而会原样读取 +0x9C。
# 第44步：这样既能支持原版，也能支持“资源改过但仍沿用同一结构”的兼容场景。
# 第45步：compare_key_ranges() 会把三个关键原版区间和研究版逐字节比较。
# 第46步：若关键区间不同，本节点就不能再声称研究版能作为这些地址的等价回归样本。
# 第47步：当前三段全部相同，说明历史10-byte研究补丁没有碰到本节点机器逻辑。
# 第48步：工具没有扫描和重新分类全部56个 RNG caller，因为那件事在历史固化18～22已经做完。
# 第49步：固化62只解决历史接档明确留下的“小洞”：0x424560到底返回什么、怎样参与逃跑。
# 第50步：这样可以避免“看起来做了很多，其实重复旧工作”的研究污染。
# 第51步：0x424560 自己的 legacy index guard 并不适合作为现代安全数组检查。
# 第52步：本节点只冻结 canonical Battle 已建立完成、当前 LayoutGR index 有效时的真实行为。
# 第53步：Godot兼容层可以在读资源时做更安全的边界校验，但正常有效输入的结果必须和原机一致。
# 第54步：若要追异常越界输入，那应另开专门兼容性节点，不能混进本节点的正常行为结论。
# 第55步：machine_checks 中每一项都保存 expected_hex 和 actual_hex，失败时可以直接看是哪一字节变化。
# 第56步：check_call() 不是只看“开头是E8”，还会计算 rel32 确认真正 callee 地址。
# 第57步：caller_check 要求 0x424560 direct caller 集合精确等于 [0x42229B]，多一个少一个都FAIL。
# 第58步：distribution 的计数也必须精确匹配，不接受“差不多还是这些值”的模糊通过。
# 第59步：FIRTTECH 名称、raw ID、说明字段也单独形成 pass 门，避免资源身份错位。
# 第60步：最终 status 只有所有独立门都通过才是 PASS；任一关键证据失败都会返回退出码2。
# 第61步：退出码2让自动回归脚本能在没有人工看屏幕时也知道这轮证据已经不成立。
# 第62步：JSON 保存所有细节，适合机器复读；CSV保存250条表格，适合人工审阅。
# 第63步：JSON和CSV都来自同一轮输入，不会用历史JSON拼接出“伪重放”。
# 第64步：以后做双轮独立重放时，应重新从用户原始ZIP提取EXE和ENC，再运行这个脚本。
# 第65步：双轮输出比较时只能屏蔽输入绝对路径，任何机器字段、计数、SHA和记录值都必须一致。
# 第66步：如果未来原版Oracle换成另一个合法发行版，应先显式建立新的SHA和版本边界，不能静默接受。
# 第67步：作者正式术语与兼容实现术语必须分开；“能实现”不等于“已经找到作者源码变量名”。
# 第68步：本工具证明的是静态规则；它不需要注入DLL，也不需要恢复已经放弃的BattleTrace Hook路线。
# 第69步：真正实现兼容核心时，最重要的是保持“先抽一次rand15、再%100、再严格小于比较”的顺序。
# 第70步：若直接调用宿主引擎的 randomf()<0.3，会改变随机流消费和模偏差，不能视为严格兼容。
# 第71步：若把30写成“30%概率”但内部仍使用 legacy rand15()%100，则可以作为UI说明；核心仍保存raw阈值。
# 第72步：若字段未来出现负数或大于100，本工具会原样记录；不要在数据解析层先强行clamp。
# 第73步：canonical当前没有这种异常值，因此异常值的业务策略继续保持未定义。
# 第74步：这就是本工具的证据边界：正常canonical输入闭合，作者列名和异常输入政策不伪装成已知。
# -----------------------------------------------------------------------------

# 程序入口只负责参数、输出和退出码，不把核心研究逻辑藏在CLI里。
def main() -> int:
    parser = argparse.ArgumentParser(
        description="固化62：核对Battle「戰場脫逃」成功阈值、LayoutGR+0x9C分布与精确RNG比较规则"
    )
    parser.add_argument("--original-exe", required=True, help="完全未修改的RPG.exe.org解压后EXE")
    parser.add_argument("--research-exe", required=True, help="历史研究版RPG.exe，仅用于关键区间等价比较")
    parser.add_argument("--layoutgr", required=True, help="canonical multimedia/public/Layoutgr.enc")
    parser.add_argument("--firttech", required=True, help="canonical multimedia/public/Firttech.enc")
    parser.add_argument("--json", required=True, help="完整JSON机器结果输出路径")
    parser.add_argument("--csv", required=True, help="250条LayoutGR字段分布CSV输出路径")
    args = parser.parse_args()

    result = analyze(args)

    json_path = Path(args.json)
    csv_path = Path(args.csv)
    json_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    # CSV 和 JSON 来自同一轮内存中的 records，避免“两个文件实际跑了不同输入”。
    _, records = load_layoutgr(Path(args.layoutgr))
    write_layout_csv(csv_path, records)

    print(f"[{result['status']}] {result['scope']}")
    print(f"JSON: {json_path}")
    print(f"CSV : {csv_path}")
    return 0 if result["status"] == "PASS" else 2


if __name__ == "__main__":
    raise SystemExit(main())
