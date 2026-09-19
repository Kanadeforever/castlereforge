#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""固化67：审计Battle统一角色动作状态四个保留值的静态生产边界。

固化66确认0xBBD、0xBBF、0xBC4、0xBC5出现在统一状态消费者中，但没有
可达的已知写入。本工具不把“搜不到写入”夸大成死状态，而是固定以下证据：

1. 四个值在全部文件映射section中的16/32位字面命中；
2. 真实CMP立即数与call rel32操作数假阳性的严格分离；
3. 0x422530布尔谓词的完整直接CALL入口；
4. FightRole构造初始化与已知邻接批量复制均不生产/覆盖+0xCE4；
5. 原版Oracle与历史研究版在所有关键切片逐字节一致。

“统一状态”“保留值”“布尔谓词”均为兼容层中性名称；原作者枚举名仍为
UNKNOWN，字段必须RAW-PRESERVE。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
from pathlib import Path
from typing import Dict, List, Sequence, Tuple

from youcheng_battle_role_state_alias_inspector import (
    EXPECTED_ORIGINAL_SHA256,
    EXPECTED_RESEARCH_SHA256,
    PeImage,
    sha256,
)
from youcheng_battle_role_state_surface_inspector import (
    build_report as build_surface_report,
    scan_rel32_calls,
)


TOOL_VERSION = "0.7D-solid67.0"
RESERVED_VALUES: Tuple[int, ...] = (0xBBD, 0xBBF, 0xBC4, 0xBC5)

# 地址是立即数字节的起点，不是指令起点。
EXPECTED_LITERAL32: Dict[int, Tuple[int, ...]] = {
    0xBBD: (0x00437ACF, 0x00442B7D),
    0xBBF: (0x0042269B, 0x00422729, 0x004227CA),
    0xBC4: (0x004226B0, 0x004227D8),
    0xBC5: (0x004226B7, 0x004227E6),
}

TRUE_COMPARE_IMMEDIATES: Dict[int, Tuple[int, ...]] = {
    0xBBD: (0x00442B7D,),
    0xBBF: (0x0042269B, 0x00422729, 0x004227CA),
    0xBC4: (0x004226B0, 0x004227D8),
    0xBC5: (0x004226B7, 0x004227E6),
}

CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (0x00422530, 0x00422850, "四保留值状态白名单消费者",
     "c35d3b461665775251cc67bb36232db669848d96174bb1588f1b2ba07d5bf2d5"),
    (0x00442B58, 0x00442B91, "BBD当前活动角色结束保留门",
     "16d8d8e3da731bedf40deb7dd489a62dbdd3bad633e6747bc4e5ef317b2c9934"),
    (0x00437ACC, 0x00437AD3, "BBD字节窗口假阳性rel32",
     "86be1873aebf7fd7ff7d4cc67b226ab4d7f41853c066d094b407e3934b5d3e58"),
    (0x00441CB0, 0x00441CF8, "FightRole十六元素数组构造",
     "31fcbaf450933679a03edbeef6e720ee187d74e46e5a6f1b7388ca6d3151f5a6"),
    (0x0041F104, 0x0041F186, "FightRole元素统一状态初始化",
     "6c5be55bce5576b18304b15448b48c654b5dd6534f6607453fc37b3d95e35a4a"),
    (0x0041F63D, 0x0041F6C3, "RoleDefinition导入FightRole",
     "8ff487fc719768e9c726a518acfcf077ab63a5c803df75f52b88ad5dc7ca4786"),
    (0x00423A00, 0x00423A5E, "FightRole尾部0xE4字节导入",
     "e00a3af408e60a8de0aae25fe36803346f32287fb36f8dabcbe1aecaa0a87f90"),
    (0x00441FB8, 0x00441FF1, "FightRole导出RoleDefinition",
     "7287be42e41dff5ece14c9ead178c3616d62a59cee9ba6aea49683afcbc47c98"),
    (0x004426D3, 0x00442708, "RoleDefinition战斗内导入",
     "9c93e24e005bb6dff3ee7952ef71453aff010f8bedf1a6236b23a18b612611a6"),
)

# 这些是当前确认会复制/清零的FightRole相对区间；采用半开区间。
COPY_RANGES: Tuple[Tuple[str, int, int, str], ...] = (
    ("构造器清零RoleDefinition区", 0x84C, 0xB9C, "0x41F166..0x41F184"),
    ("RoleDefinition导入区", 0x84C, 0xB9C, "0x41F63D..0x41F6BB"),
    ("RoleDefinition导出源区", 0x84C, 0xB9C, "0x441FB8..0x441FEF"),
    ("战斗内RoleDefinition导入区", 0x84C, 0xB9C, "0x4426D3..0x4426F6"),
    ("动作工作尾块导入区", 0xCFC, 0xDE0, "0x423A00..0x423A19"),
)

EXPECTED_STRIDE_LITERAL_COUNT = 30
EXPECTED_STRIDE_PUSH_SITES = (0x00441CE9, 0x00441F25, 0x0045F8BF, 0x0045F92F)


def scan_section(image: PeImage, section_name: str, literal: bytes) -> List[int]:
    """返回指定文件映射section内的全部字节命中VA。"""

    section = image.section(section_name)
    blob = image.data[section.raw_offset:section.raw_offset + section.raw_size]
    base = image.image_base + section.virtual_address
    found: List[int] = []
    pos = 0
    while True:
        index = blob.find(literal, pos)
        if index < 0:
            break
        found.append(base + index)
        pos = index + 1
    return found


def scan_all_sections(image: PeImage, literal: bytes) -> Dict[str, List[int]]:
    """逐一扫描全部具有文件字节的PE section。"""

    result: Dict[str, List[int]] = {}
    for section in image.sections:
        if section.raw_size:
            result[section.name] = scan_section(image, section.name, literal)
    return result


def verify_literal_surface(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []
    for value in RESERVED_VALUES:
        literal32 = struct.pack("<I", value)
        literal16 = struct.pack("<H", value)
        o32 = scan_all_sections(original, literal32)
        r32 = scan_all_sections(research, literal32)
        o16 = scan_all_sections(original, literal16)
        r16 = scan_all_sections(research, literal16)
        flat32 = tuple(va for section in original.sections for va in o32.get(section.name, ()))
        flat16 = tuple(va for section in original.sections for va in o16.get(section.name, ()))
        expected = EXPECTED_LITERAL32[value]
        only_text = all(not hits for name, hits in o32.items() if name != ".text")
        rows.append({
            "码值": f"0x{value:X}",
            "32位命中": [f"0x{x:08X}" for x in flat32],
            "16位命中": [f"0x{x:08X}" for x in flat16],
            "预期命中": [f"0x{x:08X}" for x in expected],
            "非text命中": {
                name: [f"0x{x:08X}" for x in hits]
                for name, hits in o32.items() if name != ".text" and hits
            },
            "两版32位命中一致": o32 == r32,
            "两版16位命中一致": o16 == r16,
            "16与32位命中起点一致": flat16 == flat32,
            "仅text存在": only_text,
            "PASS": (
                flat32 == expected and flat16 == expected and only_text
                and o32 == r32 and o16 == r16
            ),
        })
    return rows


def verify_occurrence_classification(
    original: PeImage, research: PeImage
) -> List[Dict[str, object]]:
    """验证每个命中究竟是CMP立即数还是rel32操作数。"""

    rows: List[Dict[str, object]] = []
    for value in RESERVED_VALUES:
        for immediate_va in EXPECTED_LITERAL32[value]:
            if immediate_va == 0x00437ACF:
                start = 0x00437ACE
                expected = "e8bd0b0000"
                kind = "假阳性：CALL rel32操作数"
                target = "0x00438690"
            else:
                start = immediate_va - 1
                expected = "3d" + struct.pack("<I", value).hex()
                kind = "真实消费者：CMP EAX, imm32"
                target = "-"
            ob = original.read_va(start, len(expected) // 2)
            rb = research.read_va(start, len(expected) // 2)
            rows.append({
                "码值": f"0x{value:X}",
                "命中字节VA": f"0x{immediate_va:08X}",
                "指令起点VA": f"0x{start:08X}",
                "分类": kind,
                "CALL目标": target,
                "实际HEX": ob.hex(),
                "预期HEX": expected,
                "双版本逐字节一致": ob == rb,
                "PASS": ob.hex() == expected and ob == rb,
            })
    return rows


def verify_slices(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []
    for start, end, name, expected in CRITICAL_SLICES:
        ob = original.read_va(start, end - start)
        rb = research.read_va(start, end - start)
        actual = hashlib.sha256(ob).hexdigest()
        rows.append({
            "名称": name,
            "范围": f"0x{start:08X}..0x{end:08X}",
            "长度": end - start,
            "原版SHA-256": actual,
            "预期SHA-256": expected,
            "双版本逐字节一致": ob == rb,
            "PASS": actual == expected and ob == rb,
        })
    return rows


def verify_copy_boundaries() -> List[Dict[str, object]]:
    state_offset = 0xCE4
    rows: List[Dict[str, object]] = []
    for name, start, end, site in COPY_RANGES:
        overlaps = start <= state_offset < end
        rows.append({
            "名称": name,
            "FightRole半开区间": f"+0x{start:X}..+0x{end:X}",
            "长度": end - start,
            "机器码位置": site,
            "覆盖统一状态+0xCE4": overlaps,
            "与状态距离": (
                f"之前0x{state_offset - end:X}字节"
                if end <= state_offset else f"之后0x{start - state_offset:X}字节"
            ),
            "PASS": not overlaps,
        })
    return rows


def verify_stride_surface(original: PeImage, research: PeImage) -> Dict[str, object]:
    literal = struct.pack("<I", 0xDE4)
    ohits = [va for va, _ in original.scan_text(literal)]
    rhits = [va for va, _ in research.scan_text(literal)]
    push_sites: List[int] = []
    for immediate_va in ohits:
        if original.read_va(immediate_va - 1, 1) == b"\x68":
            push_sites.append(immediate_va - 1)
    expected_push_bytes = {
        0x00441CE9: "68e40d0000",
        0x00441F25: "68e40d0000",
        0x0045F8BF: "68e40d0000",
        0x0045F92F: "68e40d0000",
    }
    bytes_pass = all(
        original.read_va(site, 5).hex() == expected
        and original.read_va(site, 5) == research.read_va(site, 5)
        for site, expected in expected_push_bytes.items()
    )
    return {
        "0xDE4四字节命中数": len(ohits),
        "预期命中数": EXPECTED_STRIDE_LITERAL_COUNT,
        "两版命中集合一致": ohits == rhits,
        "PUSH_0xDE4指令": [f"0x{x:08X}" for x in push_sites],
        "预期PUSH指令": [f"0x{x:08X}" for x in EXPECTED_STRIDE_PUSH_SITES],
        "四处PUSH逐字节通过": bytes_pass,
        "说明": (
            "0x441CE9是16元素构造，0x441F25及0x45F8BF/0x45F92F是析构/异常清理；"
            "这些生命周期helper参数不能当作整对象数据复制证据。"
        ),
        "PASS": (
            len(ohits) == EXPECTED_STRIDE_LITERAL_COUNT
            and ohits == rhits
            and tuple(push_sites) == EXPECTED_STRIDE_PUSH_SITES
            and bytes_pass
        ),
    }


def build_report(original: PeImage, research: PeImage) -> Dict[str, object]:
    original_hash = sha256(original.data)
    research_hash = sha256(research.data)
    hash_pass = (
        original_hash == EXPECTED_ORIGINAL_SHA256
        and research_hash == EXPECTED_RESEARCH_SHA256
    )
    inherited = build_surface_report(original, research)
    literals = verify_literal_surface(original, research)
    classification = verify_occurrence_classification(original, research)
    slices = verify_slices(original, research)
    copy_ranges = verify_copy_boundaries()
    stride = verify_stride_surface(original, research)
    predicate_callers_o = scan_rel32_calls(original, 0x00422530)
    predicate_callers_r = scan_rel32_calls(research, 0x00422530)
    callers_pass = (
        predicate_callers_o == [0x00443333, 0x00443511]
        and predicate_callers_o == predicate_callers_r
    )
    constructor_rule = (
        original.read_va(0x0041F10D, 30).hex()
        == "c786e00c0000d1070000c786e40c0000b90b0000c786e80c0000a00f0000"
        and original.read_va(0x0041F10D, 30)
        == research.read_va(0x0041F10D, 30)
    )
    overall = (
        hash_pass
        and bool(inherited["总体PASS"])
        and all(row["PASS"] for row in literals)
        and all(row["PASS"] for row in classification)
        and all(row["PASS"] for row in slices)
        and all(row["PASS"] for row in copy_ranges)
        and stride["PASS"]
        and callers_pass
        and constructor_rule
    )
    return {
        "工具": Path(__file__).name,
        "工具版本": TOOL_VERSION,
        "固化节点": "v0.7D阶段中-固化67",
        "目标": (
            "闭合统一角色状态0xBBD/0xBBF/0xBC4/0xBC5的完整字面消费者、"
            "假阳性、构造初始化和已知批量复制静态边界。"
        ),
        "输入": {
            "完全未修改原版RPG.exe": {
                "路径": str(original.path), "SHA-256": original_hash,
                "PASS": original_hash == EXPECTED_ORIGINAL_SHA256,
            },
            "历史研究版RPG.exe": {
                "路径": str(research.path), "SHA-256": research_hash,
                "PASS": research_hash == EXPECTED_RESEARCH_SHA256,
            },
        },
        "固化66继承回归PASS": bool(inherited["总体PASS"]),
        "四保留值完整字面面": literals,
        "逐命中指令分类": classification,
        "0x422530直接调用面": {
            "原版CALL指令": [f"0x{x:08X}" for x in predicate_callers_o],
            "研究版CALL指令": [f"0x{x:08X}" for x in predicate_callers_r],
            "预期": ["0x00443333", "0x00443511"],
            "PASS": callers_pass,
        },
        "构造初始化": {
            "FightRole数组": (
                "Battle构造器0x441CB0以元素构造器0x41F070、析构器0x41F1A0、"
                "计数0x10和步长0xDE4构造16个元素。"
            ),
            "元素状态": (
                "0x41F10D依次初始化+0xCE0=0x7D1、+0xCE4=0xBB9、"
                "+0xCE8=0xFA0。"
            ),
            "初始化规则逐字节PASS": constructor_rule,
        },
        "邻接批量复制边界": copy_ranges,
        "0xDE4字面与生命周期PUSH": stride,
        "关键区间": slices,
        "闭合结论": {
            "BBD": (
                "完整32位字面面只有两处：0x437ACF是CALL 0x438690的rel32操作数假阳性；"
                "唯一真实立即数是0x442B7C的CMP，和BBC/BC1共同进入当前活动角色结束保留门。"
            ),
            "BBF_BC4_BC5": (
                "全部真实立即数都位于0x422530布尔谓词的状态白名单比较中；"
                "该函数当前只有0x443333与0x443511两个直接CALL。"
            ),
            "生产边界": (
                "构造器把统一状态初始化为0xBB9；已确认的0x350字节RoleDefinition"
                "复制区为+0x84C..+0xB9B，0xE4字节尾块为+0xCFC..+0xDDF，"
                "两者都不覆盖+0xCE4。当前没有0xDE4字节整FightRole数据复制证据。"
            ),
            "结论等级": (
                "当前原版EXE静态面未找到四值的可确认生产者；它们仍是消费者已知、"
                "生产者UNKNOWN的RAW-PRESERVE码值。"
            ),
        },
        "范围限制": [
            "静态负证据不能证明四值在所有运行路径、其它模块或其它发行版本绝对不可达。",
            "计算生成的数值、深层指针别名、函数指针/虚调用、脚本或外部数据驱动仍未被一般性排除。",
            "没有把0x422530强行命名为具体业务函数；这里只记录其可验证的布尔谓词/白名单行为。",
            "不允许把保留值删除、重编号或合并；兼容层必须原值保存并允许未来动态轨迹补证。",
        ],
        "总体PASS": overall,
    }


def write_csv(path: Path, report: Dict[str, object]) -> None:
    """输出人类可筛选的逐命中分类表。"""

    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow([
            "码值", "命中字节VA", "指令起点VA", "分类", "CALL目标",
            "实际HEX", "双版本一致", "PASS",
        ])
        for row in report["逐命中指令分类"]:
            writer.writerow([
                row["码值"], row["命中字节VA"], row["指令起点VA"],
                row["分类"], row["CALL目标"], row["实际HEX"],
                "是" if row["双版本逐字节一致"] else "否",
                "PASS" if row["PASS"] else "FAIL",
            ])


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="审计Battle统一角色状态四个保留值的消费者与静态生产边界。"
    )
    parser.add_argument("--original-rpg", required=True, type=Path, help="完全未修改原版RPG.exe")
    parser.add_argument("--research-rpg", required=True, type=Path, help="历史研究版RPG.exe")
    parser.add_argument("--json-out", required=True, type=Path, help="输出机器审计JSON")
    parser.add_argument("--csv-out", required=True, type=Path, help="输出逐命中分类CSV")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    original = PeImage.load(args.original_rpg)
    research = PeImage.load(args.research_rpg)
    report = build_report(original, research)
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.json_out.write_text(
        json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    write_csv(args.csv_out, report)
    print(f"[固化67] 总体结果：{'PASS' if report['总体PASS'] else 'FAIL'}")
    print(f"[固化67] JSON：{args.json_out}")
    print(f"[固化67] CSV ：{args.csv_out}")
    return 0 if report["总体PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
