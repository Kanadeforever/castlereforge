#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 七组互反抵消效果对调查器（固化47）。

这个脚本只读取三个输入：RPG.exe、Public/Firttech.enc、Public/Ail2.ENC。
它不会修改游戏文件，也不会注入进程。它的用途是把固化47最关键的三种证据放在
同一次可重复检查中：

1. RPG.exe 的 0x46A668 表到底保存什么；
2. 0x42B53A 一带机器码到底怎样使用这张表；
3. Firttech/Ail2 的作者文本能否给七组互反编号提供可靠业务语义；
4. 固化46的七个“双分支 Battle 状态表现槽”是否恰好对应这七组互反编号。

特别重要：固化46曾把 0x46A668 简写成“raw ID -> runtime slot 映射”。
固化47重新展开 0x42B53A 后确认，这个说法不够准确：normal effect 的 raw ID 本身仍按
同编号进入正常 0..27 runtime/pending 协议；0x46A668 查到的是“需要参与抵消/残量运算
的对向 runtime 槽索引”。因此本工具统一使用 counterpart_slot（对向槽）这个名字。
"""

import argparse
import csv
import hashlib
import json
import struct
from pathlib import Path
from typing import Dict, List, Tuple


TOOL_VERSION = "0.7D-solid47.0"

# -----------------------------
# 一、固定格式与机器地址
# -----------------------------

# 两张 ENC 表的记录尺寸已经在更早固化中由全表长度和访问代码闭合。
FIRTTECH_RECORD_SIZE = 0x230       # 560 字节；500 条记录。
FIRTTECH_RECORD_COUNT = 500
AIL2_RECORD_SIZE = 0x39E           # 926 字节；700 条记录。
AIL2_RECORD_COUNT = 700

# Firttech 的五组 effect value / raw ID 字段。
FIRTTECH_VALUE_BASE = 0x64
FIRTTECH_ID_BASE = 0x78
# Ail2 的五组 effect value / raw ID 字段。
AIL2_VALUE_BASE = 0xE8
AIL2_ID_BASE = 0xFC

# 0x46A668 每项 8 字节。0x42B53A 明确用 raw_id*8 访问第一 DWORD。
VA_COUNTERPART_TABLE = 0x0046A668
COUNTERPART_ENTRY_SIZE = 8
COUNTERPART_ENTRY_COUNT = 28

# 0x42B53A..0x42B569 是 normal effect 处理器里真正消费 0x46A668 的片段。
VA_COUNTERPART_USE = 0x0042B536

# Ail2 特殊分派里两条非常有价值的对称清除路径：
# 0x138 斑蜥煉脂（raw21）会先清 runtime slot0；
# 0x139 九子菩提（raw0）会先清 runtime slot21。
VA_AIL2_CLEAR_RAW21_COUNTERPART = 0x00421805
VA_AIL2_CLEAR_RAW0_COUNTERPART = 0x0042183E

# 固化46已经找到七个“双分支表现槽”。这里重新保存对应机器入口，
# 不是为了重新证明整个14槽构造器，而是为了独立证明七组互反ID与七个双分支UI槽完全重合。
# tuple = (逻辑表现槽, 第一raw ID, 第一Role字段, 第一selector, 第一入口VA,
#                        第二raw ID, 第二Role字段, 第二selector, 第二入口VA)
UI_PAIR_ROUTES = [
    (0,  0,  0x1E8,  1, 0x41CD79, 21, 0x23C, 12, 0x41CD96),
    (1,  5,  0x1FC,  2, 0x41CDC9, 24, 0x248,  3, 0x41CDE6),
    (2,  3,  0x1F4,  7, 0x41CE19, 22, 0x240,  6, 0x41CE36),
    (3,  4,  0x1F8, 17, 0x41CE69, 23, 0x244,  4, 0x41CE86),
    (4,  6,  0x200, 14, 0x41CEB9, 25, 0x24C, 13, 0x41CED6),
    (5,  7,  0x204, 10, 0x41CF09, 26, 0x250, 11, 0x41CF26),
    # 这个槽的UI优先级与“按小ID书写”的顺序相反：先看raw15，再看raw10。
    (8, 15,  0x224,  8, 0x41CFBF, 10, 0x210, 19, 0x41CFDC),
]

# 当前同版 EXE 的完整 28 项“raw ID -> 对向槽”表。
# -1 表示该 raw ID 在这里没有对向槽。
# raw9 -> slot8 是单向关系；它不属于七个互反二元环。
EXPECTED_COUNTERPART = [
    21, -1, -1, 22, 23, 24, 25, 26,
    -1, 8, 15, -1, -1, -1, -1, 10,
    -1, -1, -1, -1, -1, 0, 3, 4,
    5, 6, 7, -1,
]

# 七组严格 2-cycle。每一对都满足 table[a]==b 且 table[b]==a。
RECIPROCAL_PAIRS = [(0, 21), (3, 22), (4, 23), (5, 24), (6, 25), (7, 26), (10, 15)]

# 业务语义只使用作者资源文本能直接支持的叫法。
# 某些状态没有在文本里出现一个单独、规范的名词，所以故意保留“侧”或斜杠说明，
# 避免把研究者自己的简称冒充成官方术语。
PAIR_SEMANTICS = {
    (0, 21):  ("中毒", "命蘊式持续自愈"),
    (3, 22):  ("疲弱", "神力"),
    (4, 23):  ("迟缓/如受绳缚", "奮馳"),
    (5, 24):  ("愚鈍", "聖睿"),
    (6, 25):  ("蝕禦", "披甲"),
    (7, 26):  ("障眼", "迅目"),
    (10, 15): ("昏沉睡眠", "狂魔/狂暴"),
}

# 每个 raw ID 选择一条最清楚的 Firttech 作者文本作为语义锚点。
# record_index 也是记录内 +0x00 的实值；当前表恰好 index==ID。
FIRTTECH_ANCHORS = {
    0:  (433, "戾血萬毒", "中毒之效"),
    21: (466, "生聚靈陣", "傷勢不斷生發自癒"),
    3:  (440, "絕魄之陣", "疲弱之效"),
    22: (444, "血刃之咒", "神力之效"),
    4:  (439, "戾魄封陣", "動作遲緩、如受繩縛"),
    23: (443, "攬風神行", "奮馳之效"),
    5:  (442, "冥幻之陣", "愚鈍之效"),
    24: (446, "靈智真法", "聖睿之效"),
    6:  (441, "凶祟之陣", "蝕禦之效"),
    25: (445, "地禦之陣", "披甲之效"),
    7:  (437, "黑暗冥獄", "障眼之效"),
    26: (438, "妙心神念", "迅目之效"),
    10: (434, "幽夢幻界", "昏沈睡去"),
    15: (436, "狂魔大法", "狂性殺心"),
}

# Ail2 额外提供第一组最关键的“命蘊”直接用词和对向清除证据。
AIL2_ANCHORS = {
    0:  (313, "九子菩提", "著名的毒物"),
    21: (288, "迦葉神藥", "命蘊之效"),
}


def u16(data: bytes, offset: int) -> int:
    """按 little-endian 读取无符号16位整数。"""
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    """按 little-endian 读取无符号32位整数。"""
    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    """按 little-endian 读取有符号32位整数。"""
    return struct.unpack_from("<i", data, offset)[0]


def sha256_bytes(data: bytes) -> str:
    """返回一段字节的 SHA-256，用于报告输入身份。"""
    return hashlib.sha256(data).hexdigest()


def decode_cp950_cstring(raw: bytes) -> str:
    """
    读取 CP950 的零结尾字符串。

    游戏台湾版资源使用繁体中文 Big5/CP950。这里使用 replace 兜底，原因不是要吞掉错误，
    而是让调查器即使遇到单个异常字节也能继续输出整条证据；锚点文本随后还会逐项断言。
    """
    raw = raw.split(b"\0", 1)[0]
    return raw.decode("cp950", "replace")


def decrypt_enc(data: bytes) -> bytes:
    """
    解开 Public/*.ENC 的逐字节 XOR 包装。

    第 i 个字节与 (i % 255) 异或。这个规则来自此前 ENC 全表结构固化，
    这里重新直接执行，不依赖任何外部解密后的临时文件。
    """
    return bytes(value ^ (index % 255) for index, value in enumerate(data))


class PeImage:
    """只读 PE32 VA->文件偏移映射器。"""

    def __init__(self, data: bytes):
        # 保存原始字节。后面的断言永远只读这个对象。
        self.data = data
        if data[:2] != b"MZ":
            raise ValueError("输入 EXE 缺少 MZ 签名")

        # DOS +0x3C 指向 PE Header。
        pe_offset = u32(data, 0x3C)
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("输入 EXE 缺少 PE\\0\\0 签名")

        # 解析 COFF 与 PE32 Optional Header。
        coff = pe_offset + 4
        section_count = u16(data, coff + 2)
        optional_size = u16(data, coff + 16)
        optional = coff + 20
        if u16(data, optional) != 0x10B:
            raise ValueError("当前调查器只接受 PE32")
        self.image_base = u32(data, optional + 0x1C)

        # 收集 section 的 RVA、磁盘偏移与大小，供 VA 换算使用。
        self.sections = []
        section_table = optional + optional_size
        for index in range(section_count):
            base = section_table + index * 40
            self.sections.append({
                "name": data[base:base + 8].split(b"\0", 1)[0].decode("ascii", "replace"),
                "virtual_size": u32(data, base + 8),
                "virtual_address": u32(data, base + 12),
                "raw_size": u32(data, base + 16),
                "raw_offset": u32(data, base + 20),
            })

    def va_to_offset(self, va: int) -> int:
        # 绝对 VA 先减 ImageBase 得到 RVA。
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
        # 只返回固定长度切片；不足长度直接报错，避免静默生成残缺断言。
        offset = self.va_to_offset(va)
        result = self.data[offset:offset + size]
        if len(result) != size:
            raise ValueError(f"读取 VA 0x{va:08X} 超出文件")
        return result


def verify_machine_code(pe: PeImage) -> List[Dict[str, object]]:
    """核对固化47依赖的核心机器码，不依赖反汇编器文本输出。"""
    checks: List[Dict[str, object]] = []

    # 这是 0x42B536 起始的完整“raw ID -> 对向槽 -> 残量减法”片段。
    # 其中 `8B 14 CD 68 A6 46 00` 就是 `[ecx*8 + 0x46A668]`。
    expected_counterpart_use = bytes.fromhex(
        "8b 4c 9f 10 "
        "8b 14 cd 68 a6 46 00 "
        "85 d2 7c 24 "
        "8b 74 24 34 "
        "8b 4c 9f 24 "
        "8b 84 96 e8 01 00 00 "
        "3b c1 7e 0b "
        "2b c1 "
        "89 84 96 e8 01 00 00 "
        "eb 06 "
        "2b c8 "
        "89 4c 9f 24"
    )
    actual = pe.read(VA_COUNTERPART_USE, len(expected_counterpart_use))
    checks.append({
        "名称": "0x46A668对向槽消费与残量减法",
        "VA": f"0x{VA_COUNTERPART_USE:08X}",
        "期望": expected_counterpart_use.hex(" "),
        "实际": actual.hex(" "),
        "PASS": actual == expected_counterpart_use,
    })

    # 两条Ail2专用路径分别清 slot0 与 slot21。
    # 这里不把整个循环硬编码，只核最能证明目标槽位的 mov [edi+disp],edx。
    clear_checks = [
        (0x421828, 0xA34, "斑蜥煉脂路径清raw0/runtime slot0"),
        (0x421861, 0xA88, "九子菩提路径清raw21/runtime slot21"),
    ]
    for va, disp, name in clear_checks:
        expected = b"\x89\x97" + struct.pack("<I", disp)
        actual = pe.read(va, 6)
        checks.append({
            "名称": name,
            "VA": f"0x{va:08X}",
            "期望": expected.hex(" "),
            "实际": actual.hex(" "),
            "PASS": actual == expected,
        })

    # 固化46的七个双分支表现槽在这里独立再核一次。
    # 每个分支开头都是 `mov eax,[esi+RoleDefinition字段]`，之后固定出现 `push selector`。
    for channel, raw_a, field_a, selector_a, va_a, raw_b, field_b, selector_b, va_b in UI_PAIR_ROUTES:
        for raw_id, field, selector, entry_va in [
            (raw_a, field_a, selector_a, va_a),
            (raw_b, field_b, selector_b, va_b),
        ]:
            read_expected = b"\x8B\x86" + struct.pack("<I", field)
            read_actual = pe.read(entry_va, 6)
            checks.append({
                "名称": f"UI逻辑槽{channel} raw{raw_id}字段读取",
                "VA": f"0x{entry_va:08X}",
                "期望": read_expected.hex(" "),
                "实际": read_actual.hex(" "),
                "PASS": read_actual == read_expected,
            })

            # 固化46机器模板里 selector push 位于字段读取入口 +0x10。
            push_va = entry_va + 0x10
            push_expected = bytes((0x6A, selector & 0xFF))
            push_actual = pe.read(push_va, 2)
            checks.append({
                "名称": f"UI逻辑槽{channel} raw{raw_id} selector={selector}",
                "VA": f"0x{push_va:08X}",
                "期望": push_expected.hex(" "),
                "实际": push_actual.hex(" "),
                "PASS": push_actual == push_expected,
            })

    return checks


def read_counterpart_table(pe: PeImage) -> List[Dict[str, object]]:
    """读取28项对向槽表，并标出互反、单向或无映射三种关系。"""
    rows = []
    for raw_id in range(COUNTERPART_ENTRY_COUNT):
        base = VA_COUNTERPART_TABLE + raw_id * COUNTERPART_ENTRY_SIZE
        counterpart = s32(pe.read(base, 4), 0)
        second_dword = s32(pe.read(base + 4, 4), 0)

        # 判定关系类型时只基于当前28项表本身，不使用任何业务名。
        if counterpart < 0:
            relation = "无对向槽"
        elif 0 <= counterpart < COUNTERPART_ENTRY_COUNT and EXPECTED_COUNTERPART[counterpart] == raw_id:
            relation = "严格互反2-cycle"
        else:
            relation = "单向映射"

        rows.append({
            "raw_id": raw_id,
            "counterpart_slot": counterpart,
            "second_dword": second_dword,
            "关系": relation,
        })
    return rows


def parse_firttech(decoded: bytes) -> List[Dict[str, object]]:
    """把500条 Firttech 记录解析成调查所需的最小字段集。"""
    expected_size = FIRTTECH_RECORD_SIZE * FIRTTECH_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(f"Firttech解密后大小应为{expected_size}，实际{len(decoded)}")

    records = []
    for index in range(FIRTTECH_RECORD_COUNT):
        record = decoded[index * FIRTTECH_RECORD_SIZE:(index + 1) * FIRTTECH_RECORD_SIZE]
        values = [s32(record, FIRTTECH_VALUE_BASE + i * 4) for i in range(5)]
        raw_ids = [s32(record, FIRTTECH_ID_BASE + i * 4) for i in range(5)]
        records.append({
            "index": index,
            "record_id": u32(record, 0x00),
            "name": decode_cp950_cstring(record[0x04:0x24]),
            "values": values,
            "raw_ids": raw_ids,
            "description": decode_cp950_cstring(record[0x17C:0x230]),
        })
    return records


def parse_ail2(decoded: bytes) -> List[Dict[str, object]]:
    """把700条 Ail2 记录解析成调查所需的最小字段集。"""
    expected_size = AIL2_RECORD_SIZE * AIL2_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(f"Ail2解密后大小应为{expected_size}，实际{len(decoded)}")

    records = []
    for index in range(AIL2_RECORD_COUNT):
        record = decoded[index * AIL2_RECORD_SIZE:(index + 1) * AIL2_RECORD_SIZE]
        values = [s32(record, AIL2_VALUE_BASE + i * 4) for i in range(5)]
        raw_ids = [s32(record, AIL2_ID_BASE + i * 4) for i in range(5)]
        records.append({
            "index": index,
            "record_id": u32(record, 0x20),
            "name": decode_cp950_cstring(record[0x00:0x20]),
            "values": values,
            "raw_ids": raw_ids,
            "description": decode_cp950_cstring(record[0x188:0x388]),
        })
    return records


def verify_anchor_record(record: Dict[str, object], raw_id: int, expected_name: str,
                         expected_phrase: str) -> Dict[str, object]:
    """验证一条作者语义锚点确实包含目标raw ID、名称和关键描述。"""
    active_pairs = list(zip(record["raw_ids"], record["values"]))
    has_effect = any(effect_id == raw_id and value > 0 for effect_id, value in active_pairs)
    name_ok = record["name"] == expected_name
    phrase_ok = expected_phrase in record["description"]
    id_ok = record["record_id"] == record["index"]
    return {
        "记录索引": record["index"],
        "记录ID": record["record_id"],
        "名称": record["name"],
        "目标raw_id": raw_id,
        "五组raw_id": record["raw_ids"],
        "五组value": record["values"],
        "描述": record["description"],
        "名称匹配": name_ok,
        "关键文本匹配": phrase_ok,
        "目标effect存在且value>0": has_effect,
        "记录ID等于索引": id_ok,
        "PASS": name_ok and phrase_ok and has_effect and id_ok,
    }


def build_pair_report(counterpart_rows: List[Dict[str, object]], firttech_records: List[Dict[str, object]],
                      ail2_records: List[Dict[str, object]]) -> Tuple[List[Dict[str, object]], List[Dict[str, object]]]:
    """把机器互反表、UI路由、Firttech/Ail2文本整合成七组最终证据。"""
    counterpart = {row["raw_id"]: row["counterpart_slot"] for row in counterpart_rows}
    pair_rows: List[Dict[str, object]] = []
    anchor_checks: List[Dict[str, object]] = []

    # 先建立 raw ID -> UI route 的查询表，之后每个pair可以直接列出selector与优先级。
    ui_by_raw: Dict[int, Dict[str, int]] = {}
    for channel, raw_a, field_a, selector_a, _va_a, raw_b, field_b, selector_b, _va_b in UI_PAIR_ROUTES:
        ui_by_raw[raw_a] = {"channel": channel, "field": field_a, "selector": selector_a, "priority": 1}
        ui_by_raw[raw_b] = {"channel": channel, "field": field_b, "selector": selector_b, "priority": 2}

    # 固化46逻辑槽8是特殊情况：runtime15在UI里优先，runtime10其次。
    # 上面的tuple已经按真实UI顺序放置，因此不需要额外改写。

    for raw_a, raw_b in RECIPROCAL_PAIRS:
        semantic_a, semantic_b = PAIR_SEMANTICS[(raw_a, raw_b)]

        # 两端各找一条作者文本锚点，并立即做严格断言。
        f_index_a, f_name_a, f_phrase_a = FIRTTECH_ANCHORS[raw_a]
        f_index_b, f_name_b, f_phrase_b = FIRTTECH_ANCHORS[raw_b]
        check_a = verify_anchor_record(firttech_records[f_index_a], raw_a, f_name_a, f_phrase_a)
        check_b = verify_anchor_record(firttech_records[f_index_b], raw_b, f_name_b, f_phrase_b)
        check_a["来源"] = "Firttech"
        check_b["来源"] = "Firttech"
        anchor_checks.extend([check_a, check_b])

        pair_rows.append({
            "raw_a": raw_a,
            "raw_b": raw_b,
            "table_a_to_b": counterpart[raw_a],
            "table_b_to_a": counterpart[raw_b],
            "互反成立": counterpart[raw_a] == raw_b and counterpart[raw_b] == raw_a,
            "语义A": semantic_a,
            "语义B": semantic_b,
            "Firttech锚点A": f"{f_index_a}:{f_name_a}",
            "Firttech锚点B": f"{f_index_b}:{f_name_b}",
            "UI逻辑槽": ui_by_raw[raw_a]["channel"],
            "A字段": f"+0x{ui_by_raw[raw_a]['field']:03X}",
            "Aselector": ui_by_raw[raw_a]["selector"],
            "A_UI优先级": ui_by_raw[raw_a]["priority"],
            "B字段": f"+0x{ui_by_raw[raw_b]['field']:03X}",
            "Bselector": ui_by_raw[raw_b]["selector"],
            "B_UI优先级": ui_by_raw[raw_b]["priority"],
        })

    # 第一组额外用Ail2验证“中毒”和“命蘊”作者用词。
    for raw_id, (index, name, phrase) in AIL2_ANCHORS.items():
        check = verify_anchor_record(ail2_records[index], raw_id, name, phrase)
        check["来源"] = "Ail2"
        anchor_checks.append(check)

    return pair_rows, anchor_checks


def write_csv(path: Path, pair_rows: List[Dict[str, object]]) -> None:
    """输出给人直接查看的七组互反关系CSV。"""
    fieldnames = [
        "raw_a", "语义A", "raw_b", "语义B", "互反成立", "UI逻辑槽",
        "A字段", "Aselector", "A_UI优先级", "B字段", "Bselector", "B_UI优先级",
        "Firttech锚点A", "Firttech锚点B",
    ]
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8-sig", newline="") as fp:
        writer = csv.DictWriter(fp, fieldnames=fieldnames, extrasaction="ignore")
        writer.writeheader()
        writer.writerows(pair_rows)


def main() -> int:
    # argparse 让工具既能由人工运行，也能在封包回归脚本中无交互重放。
    parser = argparse.ArgumentParser(description="调查Battle七组互反抵消effect与作者状态语义")
    parser.add_argument("--exe", required=True, type=Path, help="RPG.exe 路径")
    parser.add_argument("--firttech", required=True, type=Path, help="Public/Firttech.enc 路径")
    parser.add_argument("--ail2", required=True, type=Path, help="Public/Ail2.ENC 路径")
    parser.add_argument("--json", dest="json_path", required=True, type=Path, help="输出JSON路径")
    parser.add_argument("--csv", dest="csv_path", required=True, type=Path, help="输出CSV路径")
    args = parser.parse_args()

    # 一次性读入输入文件。只读bytes可保证后续所有分析都不会改变原文件。
    exe_data = args.exe.read_bytes()
    firttech_encrypted = args.firttech.read_bytes()
    ail2_encrypted = args.ail2.read_bytes()

    # 解开两个ENC表，并解析最小字段集。
    firttech_decoded = decrypt_enc(firttech_encrypted)
    ail2_decoded = decrypt_enc(ail2_encrypted)
    firttech_records = parse_firttech(firttech_decoded)
    ail2_records = parse_ail2(ail2_decoded)

    # PE层机器码与0x46A668表都直接从本次输入RPG.exe读取。
    pe = PeImage(exe_data)
    machine_checks = verify_machine_code(pe)
    counterpart_rows = read_counterpart_table(pe)

    # 检查28项表是否与本节点冻结值完全一致，并确认第二DWORD当前全0。
    table_values = [row["counterpart_slot"] for row in counterpart_rows]
    table_exact = table_values == EXPECTED_COUNTERPART
    table_second_all_zero = all(row["second_dword"] == 0 for row in counterpart_rows)

    # 自动从表里找所有严格2-cycle，避免只相信手工写的RECIPROCAL_PAIRS常量。
    discovered_pairs = []
    for raw_id, other in enumerate(table_values):
        if other < 0 or other >= len(table_values):
            continue
        if table_values[other] == raw_id and raw_id < other:
            discovered_pairs.append((raw_id, other))
    reciprocal_exact = discovered_pairs == RECIPROCAL_PAIRS

    # 组装作者文本 + UI route + counterpart table 的七组交叉证据。
    pair_rows, anchor_checks = build_pair_report(counterpart_rows, firttech_records, ail2_records)

    # 额外确认Ail2第一组专用对向清除的两条机器断言在总machine_checks里都通过。
    machine_all_pass = all(item["PASS"] for item in machine_checks)
    anchors_all_pass = all(item["PASS"] for item in anchor_checks)
    pairs_all_reciprocal = all(item["互反成立"] for item in pair_rows)

    report = {
        "tool_version": TOOL_VERSION,
        "status": "PASS" if all([
            machine_all_pass,
            table_exact,
            table_second_all_zero,
            reciprocal_exact,
            anchors_all_pass,
            pairs_all_reciprocal,
        ]) else "FAIL",
        "inputs": {
            "RPG.exe": {"size": len(exe_data), "sha256": sha256_bytes(exe_data)},
            "Firttech.enc": {"size": len(firttech_encrypted), "sha256": sha256_bytes(firttech_encrypted)},
            "Ail2.ENC": {"size": len(ail2_encrypted), "sha256": sha256_bytes(ail2_encrypted)},
        },
        "core_correction": {
            "solid46_historical_label": "raw ID -> runtime slot mapping",
            "solid47_current_label": "raw ID -> counterpart runtime slot used by cancellation/residual arithmetic",
            "normal_effect_self_index_note": "normal raw ID 0..27仍按原raw ID进入其自身pending/runtime协议；0x46A668不是自身目的槽表",
        },
        "counterpart_table": counterpart_rows,
        "counterpart_table_exact": table_exact,
        "counterpart_second_dword_all_zero": table_second_all_zero,
        "discovered_reciprocal_pairs": [list(pair) for pair in discovered_pairs],
        "expected_reciprocal_pairs": [list(pair) for pair in RECIPROCAL_PAIRS],
        "reciprocal_pairs_exact": reciprocal_exact,
        "one_way_relation": {"raw_id": 9, "counterpart_slot": 8, "reverse": table_values[8]},
        "pair_semantics_and_ui": pair_rows,
        "author_text_anchor_checks": anchor_checks,
        "machine_checks": machine_checks,
        "machine_check_pass_count": sum(1 for item in machine_checks if item["PASS"]),
        "machine_check_total": len(machine_checks),
        "conclusions": [
            "0x46A668在0x42B53A处按raw_id*8读取第一DWORD；该值是对向runtime槽索引，用于直接减法/残量运算，不是normal effect自身目的槽索引。",
            "当前28项表恰有七组严格互反2-cycle：0<->21、3<->22、4<->23、5<->24、6<->25、7<->26、10<->15。",
            "七组2-cycle与固化46七个双分支Battle状态表现槽逐组完全一致；UI仍保留各槽原有第一/第二分支优先级。",
            "Firttech作者文本可为七组两端提供语义锚点；Ail2进一步把raw0锚定到毒物/中毒、raw21锚定到命蘊式持续复癒。",
            "raw9->slot8是单向关系，不属于七组互反2-cycle；不得凭数字相邻或图标把它并入七组。",
            "兼容引擎应把counterpart cancellation与normal effect自身pending/runtime写入分开建模，不能继续把0x46A668实现成raw ID目的槽重映射。",
        ],
    }

    # JSON使用UTF-8且保留中文，不转成\uXXXX，便于人工接档审阅。
    args.json_path.parent.mkdir(parents=True, exist_ok=True)
    args.json_path.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    write_csv(args.csv_path, pair_rows)

    print(f"[固化47] status={report['status']}")
    print(f"[固化47] machine={report['machine_check_pass_count']}/{report['machine_check_total']}")
    print(f"[固化47] reciprocal_pairs={discovered_pairs}")
    print(f"[固化47] author_anchors={sum(1 for x in anchor_checks if x['PASS'])}/{len(anchor_checks)}")
    print(f"[固化47] json={args.json_path}")
    print(f"[固化47] csv={args.csv_path}")
    return 0 if report["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
