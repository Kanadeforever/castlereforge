#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 单向对向槽与对向表第二字段调查器（固化49）。

这个工具只读三个输入：

1. RPG.exe；
2. Public/Firttech.enc；
3. Public/Ail2.ENC。

它不会修改 EXE、不会写回 ENC、不会注入游戏进程。它做的事情可以概括成四步：

- 从 RPG.exe 直接读取 0x46A668 的 28 个 8-byte 表项；
- 证明 raw9 -> slot8 是单向关系，而 raw8 -> -1；
- 证明每个 8-byte 表项的第二个 DWORD 在当前 EXE 中都是 0，并且当前静态绝对地址引用面没有任何代码直接读取这些第二 DWORD；
- 解密 Firttech/Ail2 全表，穷举 raw8/raw9 的真实资源记录，核对当前 canonical 资源的直接 value 分布。

为什么要把这些事情放在一个工具里？

因为固化47已经证明 0x46A668 是“raw effect ID -> 对向/抵消槽”的表，而不是 normal effect 自身目标槽。
七组互反状态对都满足 a->b 且 b->a；raw9->8 却不是这样。若兼容引擎为了“看起来整齐”把它自动补成 8->9，
就会创造原版不存在的行为。与此同时，表项每项占 8 字节，但当前消费机器码只读第一个 DWORD；第二 DWORD 必须保留，
却不能因为它当前全 0 就擅自命名成 padding/reserved/dead。

本工具坚持两个边界：

- “没有找到当前静态绝对地址直读”不等于“宇宙中绝对没有任何间接用途”；
- “当前 canonical Firttech/Ail2 中 raw8 的直接 value 都为 0”不等于“运行时 raw8 永远不可能为正”。
  generic normal-effect writer 仍然能够按 raw ID 同编号写入 slot8，只要上游真的提供了正值 raw8。
"""

import argparse
import csv
import hashlib
import json
import struct
from pathlib import Path
from typing import Dict, List, Tuple


TOOL_VERSION = "0.7D-solid49.0"

# -----------------------------------------------------------------------------
# 一、资源表的已经固化的结构尺寸
# -----------------------------------------------------------------------------

# Firttech.enc 解密后固定 500 条，每条 0x230 字节。
FIRTTECH_RECORD_SIZE = 0x230
FIRTTECH_RECORD_COUNT = 500

# Ail2.ENC 解密后固定 700 条，每条 0x39E 字节。
AIL2_RECORD_SIZE = 0x39E
AIL2_RECORD_COUNT = 700

# 两种资源都各自保存五组 effect value / raw ID。
# 这里只读取已经在前代固化中确认的偏移，不重新猜格式。
FIRTTECH_VALUE_BASE = 0x64
FIRTTECH_ID_BASE = 0x78
AIL2_VALUE_BASE = 0xE8
AIL2_ID_BASE = 0xFC

# -----------------------------------------------------------------------------
# 二、RPG.exe 中与固化49直接相关的地址
# -----------------------------------------------------------------------------

# 28 项对向/抵消表。每项 8 字节：
#   +0x00 DWORD：当前已证 consumer 使用的 counterpart slot；
#   +0x04 DWORD：当前表中全 0，本固化继续调查其是否存在静态直读。
VA_COUNTERPART_TABLE = 0x0046A668
COUNTERPART_ENTRY_SIZE = 8
COUNTERPART_ENTRY_COUNT = 28

# normal effect 处理器中消费第一 DWORD 的机器码起点。
VA_COUNTERPART_USE = 0x0042B536

# 这里先检查 effect value > 0，<=0 会直接跳到函数尾。
# 这条断言非常重要：它说明当前资源里 raw8 value=0 不能被我们直接解释成“正值 raw8 状态来源”。
VA_POSITIVE_VALUE_GATE = 0x0042B1BF

# 两处已知 broad clear 会显式把 FightRole+0xA54（raw8 runtime slot）写 0。
# 它们证明 raw8 处在状态维护结构中，但不能证明 raw8 当前有正值 producer。
VA_RAW8_CLEAR_A = 0x00421915
VA_RAW8_CLEAR_B = 0x00421F9B

# generic pending writer 使用 raw ID 同编号写入 pending 数组；前代已经证明 ID<=27 才进入这条路径。
# 固化49保留这条断言，是为了防止把“当前资源 direct value=0”误写成“代码绝对写不出 raw8”。
VA_NORMAL_PENDING_WRITE = 0x00422C44

# 当前同版 EXE 的 28 项第一 DWORD 精确值。
EXPECTED_COUNTERPART = [
    21, -1, -1, 22, 23, 24, 25, 26,
    -1, 8, 15, -1, -1, -1, -1, 10,
    -1, -1, -1, -1, -1, 0, 3, 4,
    5, 6, 7, -1,
]

# raw8/raw9 当前资源正样本。这里保存的是“应当出现的记录索引、名称、直接value”。
# 工具随后会完整扫描 500+700 条记录，而不是只抽查这些行。
EXPECTED_RAW8_FIRTTECH = {
    312: ("炎殺戟", 0),
    339: ("炎殺彈", 0),
    408: ("離火神訣", 0),
    409: ("焚炎之陣", 0),
    410: ("七燁真火", 0),
    411: ("天火之術", 0),
    412: ("九俱焚滅", 0),
}

EXPECTED_RAW9_FIRTTECH = {
    336: ("魔獄冰嵐", 50),
    340: ("冰破飛錐", 60),
    368: ("青色魔霧", 90),
    383: ("冰魄晶壁", 90),
    413: ("天霜雪舞", 10),
    414: ("冰華之陣", 20),
    415: ("玄凜冰煞", 30),
    416: ("冰獄寒嵐", 40),
    417: ("魂凍冰魄", 50),
}

EXPECTED_RAW9_AIL2 = {
    360: ("封凜冰露", 30),
}


def u16(data: bytes, offset: int) -> int:
    """从 data[offset:] 按 little-endian 读取无符号 16 位整数。"""
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    """从 data[offset:] 按 little-endian 读取无符号 32 位整数。"""
    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    """从 data[offset:] 按 little-endian 读取有符号 32 位整数。"""
    return struct.unpack_from("<i", data, offset)[0]


def sha256_bytes(data: bytes) -> str:
    """返回输入字节的 SHA-256；报告里用它固定本次调查的输入身份。"""
    return hashlib.sha256(data).hexdigest()


def decode_cp950_cstring(raw: bytes) -> str:
    """
    把固定宽度 CP950 字节区读成零结尾字符串。

    台湾版资源正文使用 Big5/CP950。先在第一个 0x00 截断，再按 cp950 解码。
    errors='replace' 只用于避免一个坏字节让整张表无法审计；预期记录名称随后还有精确断言。
    """
    return raw.split(b"\0", 1)[0].decode("cp950", "replace")


def decrypt_enc(data: bytes) -> bytes:
    """
    解开 Public/*.ENC 的逐字节 XOR 包装。

    第 i 个字节与 (i % 255) 异或。工具每次都从用户给的原始 ENC 现场解密，
    不依赖包外的“已经解密文件”，这样单独拿到本包也能完整重放。
    """
    return bytes(value ^ (index % 255) for index, value in enumerate(data))


class PeImage:
    """
    最小只读 PE32 解析器。

    我们只需要把绝对 VA（例如 0x0042B53A）换算成磁盘文件偏移。
    不使用第三方 PE 库，可以减少环境依赖，也让调查器在纯 Python 环境中可运行。
    """

    def __init__(self, data: bytes):
        self.data = data

        # Windows PE 文件必须先有 DOS 'MZ' 头。
        if data[:2] != b"MZ":
            raise ValueError("输入 EXE 缺少 MZ 签名")

        # DOS Header +0x3C 保存 PE Header 的文件偏移。
        pe_offset = u32(data, 0x3C)
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("输入 EXE 缺少 PE\\0\\0 签名")

        coff = pe_offset + 4
        section_count = u16(data, coff + 2)
        optional_size = u16(data, coff + 16)
        optional = coff + 20

        # 0x10B 是 PE32；本游戏是 32 位程序。
        if u16(data, optional) != 0x10B:
            raise ValueError("当前调查器只接受 PE32")

        self.image_base = u32(data, optional + 0x1C)
        self.sections: List[Dict[str, int]] = []

        # Section Table 紧跟 Optional Header。记录 RVA 与文件偏移即可。
        section_table = optional + optional_size
        for index in range(section_count):
            base = section_table + index * 40
            self.sections.append({
                "virtual_size": u32(data, base + 8),
                "virtual_address": u32(data, base + 12),
                "raw_size": u32(data, base + 16),
                "raw_offset": u32(data, base + 20),
            })

    def va_to_offset(self, va: int) -> int:
        """把绝对 VA 换成磁盘文件偏移；不允许读取 section 的零填充尾部。"""
        rva = va - self.image_base
        for section in self.sections:
            start = section["virtual_address"]
            span = max(section["virtual_size"], section["raw_size"])
            if start <= rva < start + span:
                delta = rva - start
                if delta >= section["raw_size"]:
                    raise ValueError(f"VA 0x{va:08X} 位于 section 零填充区")
                return section["raw_offset"] + delta
        raise ValueError(f"VA 0x{va:08X} 不属于任何 section")

    def read(self, va: int, size: int) -> bytes:
        """从绝对 VA 读取固定长度字节；不足长度时直接报错。"""
        offset = self.va_to_offset(va)
        result = self.data[offset:offset + size]
        if len(result) != size:
            raise ValueError(f"读取 VA 0x{va:08X} 超出文件")
        return result


def assert_machine_bytes(pe: PeImage) -> List[Dict[str, object]]:
    """
    验证固化49依赖的机器码。

    这里故意检查原始机器字节，而不是检查 objdump 文本。原因是反汇编文本格式会因工具版本变化，
    原始机器字节才是稳定证据。
    """
    checks: List[Tuple[str, int, bytes]] = [
        # 0x42B536：取 raw ID，随后 `[ecx*8 + 0x46A668]` 只读取每项第一个 DWORD。
        (
            "counterpart_first_dword_only",
            VA_COUNTERPART_USE,
            bytes.fromhex(
                "8b4c9f10"          # mov ecx,[edi+ebx*4+0x10] : raw id
                "8b14cd68a64600"    # mov edx,[ecx*8+0x46A668] : 第一个 DWORD
                "85d27c24"          # test edx,edx / jl no_counterpart
                "8b742434"          # mov esi,[esp+0x34]
                "8b4c9f24"          # mov ecx,[edi+ebx*4+0x24] : 新 effect value
                "8b8496e8010000"    # mov eax,[esi+edx*4+0x1e8] : 对向 runtime slot
                "3bc17e0b"          # cmp eax,ecx / jle residual branch
                "2bc1"              # sub eax,ecx
                "898496e8010000"    # 写回减少后的对向槽
                "eb06"              # 跳过 residual 计算
                "2bc8"              # residual = new - old_counterpart
                "894c9f24"          # 写回 residual new effect value
            ),
        ),
        # 0x42B1BF：真正进入 normal effect 参数与抵消阶段前，value 必须 > 0。
        (
            "positive_value_gate",
            VA_POSITIVE_VALUE_GATE,
            bytes.fromhex("8b44241485c00f8eb2030000"),
        ),
        # 两个 explicit raw8 broad-clear：目标偏移都是 FightRole+0xA54。
        (
            "raw8_broad_clear_a",
            VA_RAW8_CLEAR_A,
            bytes.fromhex("8995540a0000"),
        ),
        (
            "raw8_broad_clear_b",
            VA_RAW8_CLEAR_B,
            bytes.fromhex("899e540a0000"),
        ),
        # generic pending writer 的关键片段：ID<=27 后按同编号索引写 pending。
        # 这条证据用于提醒实现者：raw8并非在代码结构上“禁止写入”。
        (
            "normal_pending_same_index_writer",
            VA_NORMAL_PENDING_WRITE,
            bytes.fromhex("8b44241c03c28b048683f81b7f1c"),
        ),
    ]

    results: List[Dict[str, object]] = []
    for name, va, expected in checks:
        actual = pe.read(va, len(expected))
        results.append({
            "名称": name,
            "VA": f"0x{va:08X}",
            "expected_hex": expected.hex(),
            "actual_hex": actual.hex(),
            "PASS": actual == expected,
        })
    return results


def read_counterpart_table(pe: PeImage) -> List[Dict[str, object]]:
    """
    读取完整 28×8-byte 对向表。

    每一行都保留两个 signed DWORD。第二 DWORD 当前虽然全 0，但不能丢弃；
    兼容引擎的数据结构至少应该保留原值，以防其他版本或 MOD 使用。
    """
    raw = pe.read(VA_COUNTERPART_TABLE, COUNTERPART_ENTRY_COUNT * COUNTERPART_ENTRY_SIZE)
    rows: List[Dict[str, object]] = []
    for raw_id in range(COUNTERPART_ENTRY_COUNT):
        offset = raw_id * COUNTERPART_ENTRY_SIZE
        first = s32(raw, offset)
        second = s32(raw, offset + 4)
        rows.append({
            "raw_id": raw_id,
            "counterpart_slot": first,
            "second_dword": second,
        })
    return rows


def find_all(haystack: bytes, needle: bytes) -> List[int]:
    """返回 needle 在 haystack 中所有（允许重叠）文件偏移。"""
    positions: List[int] = []
    start = 0
    while True:
        position = haystack.find(needle, start)
        if position < 0:
            return positions
        positions.append(position)
        start = position + 1


def scan_table_absolute_address_literals(exe_bytes: bytes) -> Dict[str, object]:
    """
    扫描 28 个第一 DWORD 地址和 28 个第二 DWORD 地址是否作为 32 位绝对常量出现。

    这是“静态绝对地址引用面”检查，不是假装实现一个完整 x86 数据流分析器。
    对本程序这种 PE32 固定地址访问，机器指令通常会把 0x46xxxx 绝对地址直接编码在指令中。
    当前第一 DWORD 动态索引 consumer 会出现表基址 0x46A668；如果某段代码直接访问某一项的
    第二 DWORD，也通常会出现 0x46A66C、0x46A674……这样的绝对地址。

    返回所有命中，而不是只返回布尔值。这样以后换 EXE 时可以直接看新增引用落在哪里。
    """
    first_hits: List[Dict[str, object]] = []
    second_hits: List[Dict[str, object]] = []

    for raw_id in range(COUNTERPART_ENTRY_COUNT):
        first_address = VA_COUNTERPART_TABLE + raw_id * COUNTERPART_ENTRY_SIZE
        second_address = first_address + 4

        first_positions = find_all(exe_bytes, struct.pack("<I", first_address))
        second_positions = find_all(exe_bytes, struct.pack("<I", second_address))

        if first_positions:
            first_hits.append({
                "raw_id": raw_id,
                "address": f"0x{first_address:08X}",
                "file_offsets": [f"0x{position:X}" for position in first_positions],
            })
        if second_positions:
            second_hits.append({
                "raw_id": raw_id,
                "address": f"0x{second_address:08X}",
                "file_offsets": [f"0x{position:X}" for position in second_positions],
            })

    return {
        "第一DWORD绝对地址命中": first_hits,
        "第二DWORD绝对地址命中": second_hits,
        "第一DWORD命中项数": len(first_hits),
        "第二DWORD命中项数": len(second_hits),
        # 当前预期：只有 raw0 的 first address（也就是动态索引基址）出现一次；second 全部 0 次。
        "PASS": (
            len(first_hits) == 1
            and first_hits[0]["raw_id"] == 0
            and first_hits[0]["address"] == "0x0046A668"
            and len(first_hits[0]["file_offsets"]) == 1
            and len(second_hits) == 0
        ),
    }


def parse_firttech(decoded: bytes) -> List[Dict[str, object]]:
    """解析 Firttech 全表中本调查需要的名称、五组ID/value与描述。"""
    expected_size = FIRTTECH_RECORD_SIZE * FIRTTECH_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(f"Firttech解密后大小应为{expected_size}，实际{len(decoded)}")

    records: List[Dict[str, object]] = []
    for index in range(FIRTTECH_RECORD_COUNT):
        start = index * FIRTTECH_RECORD_SIZE
        record = decoded[start:start + FIRTTECH_RECORD_SIZE]
        records.append({
            "source": "Firttech",
            "index": index,
            "record_id": u32(record, 0x00),
            "name": decode_cp950_cstring(record[0x04:0x24]),
            "raw_ids": [s32(record, FIRTTECH_ID_BASE + slot * 4) for slot in range(5)],
            "values": [s32(record, FIRTTECH_VALUE_BASE + slot * 4) for slot in range(5)],
            "description": decode_cp950_cstring(record[0x17C:0x230]),
        })
    return records


def parse_ail2(decoded: bytes) -> List[Dict[str, object]]:
    """解析 Ail2 全表中本调查需要的名称、五组ID/value与描述。"""
    expected_size = AIL2_RECORD_SIZE * AIL2_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(f"Ail2解密后大小应为{expected_size}，实际{len(decoded)}")

    records: List[Dict[str, object]] = []
    for index in range(AIL2_RECORD_COUNT):
        start = index * AIL2_RECORD_SIZE
        record = decoded[start:start + AIL2_RECORD_SIZE]
        records.append({
            "source": "Ail2",
            "index": index,
            "record_id": u32(record, 0x20),
            "name": decode_cp950_cstring(record[0x00:0x20]),
            "raw_ids": [s32(record, AIL2_ID_BASE + slot * 4) for slot in range(5)],
            "values": [s32(record, AIL2_VALUE_BASE + slot * 4) for slot in range(5)],
            "description": decode_cp950_cstring(record[0x188:0x388]),
        })
    return records


def collect_raw_occurrences(records: List[Dict[str, object]], raw_id: int) -> List[Dict[str, object]]:
    """
    穷举一张资源表里 raw_id 出现的所有槽。

    同一条记录理论上可能在五个槽里出现多次，所以这里按“槽出现”输出，不能只按记录去重。
    当前 raw8/raw9 语料碰巧都是每记录一次，但工具不把这个偶然现象写死成格式约束。
    """
    rows: List[Dict[str, object]] = []
    for record in records:
        for effect_slot, (effect_id, value) in enumerate(zip(record["raw_ids"], record["values"])):
            if effect_id != raw_id:
                continue
            rows.append({
                "source": record["source"],
                "record_index": record["index"],
                "record_id": record["record_id"],
                "name": record["name"],
                "effect_slot": effect_slot,
                "raw_id": raw_id,
                "value": value,
                "description": record["description"],
            })
    return rows


def verify_expected_occurrences(rows: List[Dict[str, object]], expected: Dict[int, Tuple[str, int]], source: str) -> Dict[str, object]:
    """验证某个 source 的全量命中集合与当前 canonical 预期完全相等。"""
    actual: Dict[int, Tuple[str, int]] = {}
    duplicate_indexes: List[int] = []
    for row in rows:
        if row["source"] != source:
            continue
        index = int(row["record_index"])
        if index in actual:
            duplicate_indexes.append(index)
        actual[index] = (str(row["name"]), int(row["value"]))

    return {
        "source": source,
        "expected": {str(index): [name, value] for index, (name, value) in expected.items()},
        "actual": {str(index): [name, value] for index, (name, value) in actual.items()},
        "duplicate_indexes": duplicate_indexes,
        "PASS": actual == expected and not duplicate_indexes,
    }


def write_csv(path: Path, rows: List[Dict[str, object]]) -> None:
    """把 raw8/raw9 全量资源命中写成 UTF-8-SIG CSV，便于 Windows Excel 直接查看中文。"""
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "source", "record_index", "record_id", "name", "effect_slot",
        "raw_id", "value", "description",
    ]
    with path.open("w", encoding="utf-8-sig", newline="") as fp:
        writer = csv.DictWriter(fp, fieldnames=fieldnames, extrasaction="ignore")
        writer.writeheader()
        writer.writerows(rows)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="调查Battle raw9->slot8单向对向关系与0x46A668第二DWORD静态直读边界"
    )
    parser.add_argument("--exe", required=True, type=Path, help="RPG.exe路径")
    parser.add_argument("--firttech", required=True, type=Path, help="Public/Firttech.enc路径")
    parser.add_argument("--ail2", required=True, type=Path, help="Public/Ail2.ENC路径")
    parser.add_argument("--json-out", type=Path, help="可选：JSON报告输出路径")
    parser.add_argument("--csv-out", type=Path, help="可选：raw8/raw9资源正样本CSV输出路径")
    args = parser.parse_args()

    exe_bytes = args.exe.read_bytes()
    firttech_encoded = args.firttech.read_bytes()
    ail2_encoded = args.ail2.read_bytes()

    pe = PeImage(exe_bytes)
    machine_checks = assert_machine_bytes(pe)
    counterpart_rows = read_counterpart_table(pe)
    address_scan = scan_table_absolute_address_literals(exe_bytes)

    # 先验证整张第一 DWORD 表，防止只盯 raw8/raw9 而漏掉 EXE 已经换版本。
    first_values = [int(row["counterpart_slot"]) for row in counterpart_rows]
    second_values = [int(row["second_dword"]) for row in counterpart_rows]
    table_checks = {
        "first_dword_28项精确匹配": first_values == EXPECTED_COUNTERPART,
        "second_dword_28项全零": second_values == [0] * COUNTERPART_ENTRY_COUNT,
        "raw8_counterpart": first_values[8],
        "raw9_counterpart": first_values[9],
        "raw9_to_8_is_one_way": first_values[9] == 8 and first_values[8] == -1,
    }

    # ENC 每次从原文件现场解密，随后扫描完整 500+700 条表。
    firttech_decoded = decrypt_enc(firttech_encoded)
    ail2_decoded = decrypt_enc(ail2_encoded)
    firttech_records = parse_firttech(firttech_decoded)
    ail2_records = parse_ail2(ail2_decoded)

    raw8_rows = collect_raw_occurrences(firttech_records, 8) + collect_raw_occurrences(ail2_records, 8)
    raw9_rows = collect_raw_occurrences(firttech_records, 9) + collect_raw_occurrences(ail2_records, 9)

    raw8_firttech_check = verify_expected_occurrences(raw8_rows, EXPECTED_RAW8_FIRTTECH, "Firttech")
    raw8_ail2_check = verify_expected_occurrences(raw8_rows, {}, "Ail2")
    raw9_firttech_check = verify_expected_occurrences(raw9_rows, EXPECTED_RAW9_FIRTTECH, "Firttech")
    raw9_ail2_check = verify_expected_occurrences(raw9_rows, EXPECTED_RAW9_AIL2, "Ail2")

    raw8_values = [int(row["value"]) for row in raw8_rows]
    raw9_values = [int(row["value"]) for row in raw9_rows]

    resource_summary = {
        "raw8_occurrence_count": len(raw8_rows),
        "raw8_firttech_count": sum(1 for row in raw8_rows if row["source"] == "Firttech"),
        "raw8_ail2_count": sum(1 for row in raw8_rows if row["source"] == "Ail2"),
        "raw8_values": raw8_values,
        "raw8_all_direct_values_zero": bool(raw8_rows) and all(value == 0 for value in raw8_values),
        "raw9_occurrence_count": len(raw9_rows),
        "raw9_firttech_count": sum(1 for row in raw9_rows if row["source"] == "Firttech"),
        "raw9_ail2_count": sum(1 for row in raw9_rows if row["source"] == "Ail2"),
        "raw9_values": raw9_values,
        "raw9_all_direct_values_positive": bool(raw9_rows) and all(value > 0 for value in raw9_values),
        "raw8_firttech_exact": raw8_firttech_check,
        "raw8_ail2_exact": raw8_ail2_check,
        "raw9_firttech_exact": raw9_firttech_check,
        "raw9_ail2_exact": raw9_ail2_check,
    }

    all_machine_pass = all(bool(check["PASS"]) for check in machine_checks)
    all_resource_pass = all([
        raw8_firttech_check["PASS"],
        raw8_ail2_check["PASS"],
        raw9_firttech_check["PASS"],
        raw9_ail2_check["PASS"],
        resource_summary["raw8_all_direct_values_zero"],
        resource_summary["raw9_all_direct_values_positive"],
    ])
    all_table_pass = all([
        table_checks["first_dword_28项精确匹配"],
        table_checks["second_dword_28项全零"],
        table_checks["raw9_to_8_is_one_way"],
        address_scan["PASS"],
    ])

    report = {
        "工具版本": TOOL_VERSION,
        "输入": {
            "RPG.exe": {
                "路径": str(args.exe),
                "大小": len(exe_bytes),
                "SHA256": sha256_bytes(exe_bytes),
            },
            "Firttech.enc": {
                "路径": str(args.firttech),
                "大小": len(firttech_encoded),
                "SHA256": sha256_bytes(firttech_encoded),
                "解密后SHA256": sha256_bytes(firttech_decoded),
            },
            "Ail2.ENC": {
                "路径": str(args.ail2),
                "大小": len(ail2_encoded),
                "SHA256": sha256_bytes(ail2_encoded),
                "解密后SHA256": sha256_bytes(ail2_decoded),
            },
        },
        "机器码断言": machine_checks,
        "机器码断言通过数": sum(1 for check in machine_checks if check["PASS"]),
        "机器码断言总数": len(machine_checks),
        "对向表": counterpart_rows,
        "对向表检查": table_checks,
        "绝对地址静态引用扫描": address_scan,
        "资源正样本摘要": resource_summary,
        "raw8资源命中": raw8_rows,
        "raw9资源命中": raw9_rows,
        "结论边界": [
            "raw9->slot8 是当前表中的真实单向对向/抵消边；raw8->-1，兼容实现不得自动补成8->9。",
            "0x46A668每项第二DWORD当前28/28为0；当前EXE静态绝对地址引用面未发现任何第二DWORD地址命中。",
            "第二DWORD因此应保留原值但继续保持UNKNOWN；不能命名为padding/reserved/dead，也不能在兼容格式中删除。",
            "当前canonical Firttech/Ail2全表中raw8共7次，全部来自Firttech且direct value=0；raw9共10次，9条Firttech+1条Ail2且direct value全部>0。",
            "raw8资源名称与火系术法高度关联、raw9资源名称与冰/凛冻高度关联，但这只能作为资源关联观察；raw8作者正式状态名和正值运行时业务仍未证明。",
            "generic normal-effect同编号writer仍有能力写slot8；因此不得把当前资源direct value=0扩大成raw8运行时绝对不可为正。",
        ],
        "PASS": all_machine_pass and all_table_pass and all_resource_pass,
    }

    if args.json_out:
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    if args.csv_out:
        # CSV 把 raw8 与 raw9 合在一起，raw_id 列足以区分，便于直接筛选比较。
        write_csv(args.csv_out, raw8_rows + raw9_rows)

    # 终端输出保持简洁，只打印重放时最需要看的摘要。
    print(f"工具版本: {TOOL_VERSION}")
    print(f"机器码断言: {report['机器码断言通过数']}/{report['机器码断言总数']}")
    print(f"raw9->slot8 单向: {table_checks['raw9_to_8_is_one_way']}")
    print(f"第二DWORD全零: {table_checks['second_dword_28项全零']}")
    print(f"第二DWORD绝对地址命中项数: {address_scan['第二DWORD命中项数']}")
    print(f"raw8资源命中: {len(raw8_rows)}，direct value全零: {resource_summary['raw8_all_direct_values_zero']}")
    print(f"raw9资源命中: {len(raw9_rows)}，direct value全正: {resource_summary['raw9_all_direct_values_positive']}")
    print(f"PASS: {report['PASS']}")

    return 0 if report["PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
