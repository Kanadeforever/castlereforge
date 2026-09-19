#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""固化66：审计统一角色动作状态DWORD的写入/读取面与Battle tick状态机。

固化65已经证明Battle角色锚点+0x10C8与FightRole+0xCE4是同一物理DWORD。
本工具在这个地址恒等式上继续工作：穷举两种正位移口径的完整字面命中，
把直接写、直接读、LEA别名和假阳性分开，再冻结0x442A80里的可达局部转换。

注意：这里的“统一状态”“动作请求码”等名称都是兼容层中性名称。
原作者字段名、状态枚举名及动作枚举名仍然保持UNKNOWN。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Sequence, Tuple

from youcheng_battle_role_state_alias_inspector import (
    EXPECTED_10C8_RAW_HITS,
    EXPECTED_CE4_DISP32_HITS,
    EXPECTED_CE4_FALSE_RAW_HIT,
    EXPECTED_CE4_RAW_HITS,
    EXPECTED_ORIGINAL_SHA256,
    EXPECTED_RESEARCH_SHA256,
    PeImage,
    ROLE_COUNT,
    ROLE_STRIDE,
    STATE_FROM_ANCHOR,
    STATE_FROM_FIGHT_ROLE,
    is_disp32_at,
    sha256,
)


TOOL_VERSION = "0.7D-solid66.0"


@dataclass(frozen=True)
class WriteSite:
    """一个可静态冻结的状态写入点。"""

    instruction_va: int
    view: str
    value: int
    expected_hex: str
    reachable: bool
    note: str
    # 大多数记录从写指令本身开始；BBA使用“装载EDI常量+最终写入”的复合切片。
    assertion_start_va: int | None = None


@dataclass(frozen=True)
class ReadSite:
    """一个状态读取/比较点及其在当前局部路径中显式比较的码值。"""

    instruction_va: int
    view: str
    compared_values: Tuple[int, ...]
    note: str


# 两种正位移口径下的18个直接立即数写入。
# 0x4433B6虽然机器码存在，但入口class4在0x443370已经跳走；因此单独标为不可达。
DIRECT_WRITE_SITES: Tuple[WriteSite, ...] = (
    WriteSite(0x0041F117, "FightRole+0xCE4", 0xBB9, "c786e40c0000b90b0000", True, "FightRole初始化"),
    WriteSite(0x0042012B, "FightRole+0xCE4", 0xBB9, "c786e40c0000b90b0000", True, "结果/动作链复位"),
    WriteSite(0x004203C4, "FightRole+0xCE4", 0xBB9, "c786e40c0000b90b0000", True, "结果/动作链复位"),
    WriteSite(0x00420EF6, "FightRole+0xCE4", 0xBBB, "c787e40c0000bb0b0000", True, "FIRTTECH结果应用mode0入口"),
    WriteSite(0x00420FAD, "FightRole+0xCE4", 0xBB9, "c787e40c0000b90b0000", True, "FIRTTECH结果应用mode1入口"),
    WriteSite(0x00421456, "FightRole+0xCE4", 0xBB9, "c781e40c0000b90b0000", True, "目标结果处理复位"),
    WriteSite(0x004214C9, "FightRole+0xCE4", 0xBBE, "c782e40c0000be0b0000", True, "目标结果处理进入BBE"),
    WriteSite(0x00421B84, "FightRole+0xCE4", 0xBB9, "c787e40c0000b90b0000", True, "特殊分支复位"),
    WriteSite(0x00423242, "FightRole+0xCE4", 0xBB9, "c786e40c0000b90b0000", True, "角色处理结束复位"),
    WriteSite(0x00444E9A, "FightRole+0xCE4", 0xBB9, "c781e40c0000b90b0000", True, "外层角色资源/状态清理"),
    WriteSite(0x00442F2F, "角色锚点+0x10C8", 0xBC1, "c786c8100000c10b0000", True, "Battle分支写BC1"),
    WriteSite(0x00443394, "角色锚点+0x10C8", 0xBC0, "c787c8100000c00b0000", True, "class5真实可达写入"),
    WriteSite(0x004433A5, "角色锚点+0x10C8", 0xBC6, "c787c8100000c60b0000", True, "class6真实可达写入"),
    WriteSite(0x004433B6, "角色锚点+0x10C8", 0xBC7, "c787c8100000c70b0000", False, "字面存在；入口class4已提前跳走"),
    WriteSite(0x004433C2, "角色锚点+0x10C8", 0xBBB, "c787c8100000bb0b0000", True, "普通非1/2/4/5/6默认写入"),
    WriteSite(0x00443427, "角色锚点+0x10C8", 0xBBC, "c787c8100000bc0b0000", True, "class1/2/4共享写入"),
    WriteSite(0x0044349E, "角色锚点+0x10C8", 0xBBE, "c786c8100000be0b0000", True, "独立动作入口写BBE"),
    WriteSite(0x0044354C, "角色锚点+0x10C8", 0xBC1, "c787c8100000c10b0000", True, "独立动作入口写BC1"),
)


# 三条先建立本地指针、再用[reg]或[reg-8]写同一字段的写入。
COMPUTED_WRITE_SITES: Tuple[WriteSite, ...] = (
    WriteSite(
        0x00442ACA, "Battle+0x10D0局部指针-8", 0xBBA,
        "bfba0b00008b8640faffff85c00f8ca0000000817ef4d40700007519833e6775146a006a006a688d8e14f3ffffe856f6fdff897ef8",
        True, "EDI在0x442A98装入0xBBA；0x442ACA写[ESI-8]", 0x00442A98,
    ),
    WriteSite(0x00442B44, "Battle+0x10D0局部指针-8", 0xBB9, "c746f8b90b0000", True, "BBE且相邻状态为0x7D1时回写BB9"),
    WriteSite(0x00442C12, "Battle+0x10C8数组指针+0", 0xBB9, "c700b90b0000", True, "BBC满足回收条件时回写BB9"),
)


# +0xCE4真实disp32的13个读取点。compared_values只记录当前局部控制流中显式出现的码值。
CE4_READ_SITES: Tuple[ReadSite, ...] = (
    ReadSite(0x004214FD, "FightRole+0xCE4", (0xBC6,), "class6第一结果DWORD归零门"),
    ReadSite(0x004222F7, "FightRole+0xCE4", (0xBBB, 0xBC1), "三条相邻资源/表现路径之一"),
    ReadSite(0x0042231C, "FightRole+0xCE4", (0xBBB, 0xBC1), "三条相邻资源/表现路径之二"),
    ReadSite(0x00422342, "FightRole+0xCE4", (0xBBB, 0xBC1), "三条相邻资源/表现路径之三"),
    ReadSite(0x00422580, "FightRole+0xCE4", (0xBBB,), "角色可行动谓词"),
    ReadSite(0x00422678, "FightRole+0xCE4", (0xBB9, 0xBBA, 0xBBB, 0xBBE, 0xBBF, 0xBC0, 0xBC4, 0xBC5, 0xBC6), "队伍级状态白名单"),
    ReadSite(0x00422706, "FightRole+0xCE4", (0xBB9, 0xBBA, 0xBBE, 0xBBF, 0xBC6), "无来源依赖时的目标状态白名单"),
    ReadSite(0x00422744, "FightRole+0xCE4", (0xBBB,), "有来源依赖时比较来源角色状态；比较值由EBP=0xBBB提供"),
    ReadSite(0x004227AE, "FightRole+0xCE4", (0xBB9, 0xBBA, 0xBBE, 0xBBF, 0xBC0, 0xBC4, 0xBC5, 0xBC6), "全角色状态白名单"),
    ReadSite(0x00422801, "FightRole+0xCE4", (0xBBB, 0xBC0), "来源角色状态检查"),
    ReadSite(0x00422B44, "FightRole+0xCE4", (0xBBE,), "每目标动作分派局部布尔"),
    ReadSite(0x00423741, "FightRole+0xCE4", (0xBC6, 0xBBE), "0x72/0x6C/0x6D动作请求选择"),
    ReadSite(0x00445FEA, "FightRole+0xCE4", (0xBC0,), "角色候选/缓存构建排除"),
)


# +0x10C8口径的5个直接读取；另有LEA建立数组首指针的2处在别名表中处理。
ANCHOR_READ_SITES: Tuple[ReadSite, ...] = (
    ReadSite(0x00442B75, "角色锚点+索引*0xDE4+0x10C8", (0xBBD, 0xBBC, 0xBC1), "当前活动角色结束/保留门"),
    ReadSite(0x00442E9E, "角色锚点+0x10C8", (0xBBB,), "Battle分派分支0"),
    ReadSite(0x00442EFD, "角色锚点+0x10C8", (0xBBB,), "Battle分派分支1"),
    ReadSite(0x00442F51, "角色锚点+0x10C8", (0xBBB,), "Battle分派默认分支"),
    ReadSite(0x004430E5, "角色锚点+索引*0xDE4+0x10C8", (0xBBB,), "索引角色状态消费者"),
)


# 通过本地零偏移/负八偏移别名读取的状态点；它们不能靠搜索+0xCE4/+0x10C8再次发现。
LOCAL_ALIAS_READ_SITES: Tuple[ReadSite, ...] = (
    ReadSite(0x00442ACD, "Battle+0x10D0局部指针-8", (0xBBA,), "BBA分支保持0x68请求"),
    ReadSite(0x00442AF5, "Battle+0x10D0局部指针-8", (0xBB9,), "BB9分支保持0x67请求"),
    ReadSite(0x00442B21, "Battle+0x10D0局部指针-8", (0xBBE,), "BBE且相邻0x7D1时请求0x67并回BB9"),
    ReadSite(0x00442BF8, "Battle+0x10C8数组指针+0", (0xBBC,), "BBC条件回收扫描"),
    ReadSite(0x00442FAC, "Battle+0x10C8数组指针+0", (0xBBB,), "来源标记为FA0时的局部扫描"),
    ReadSite(0x00442D0E, "Battle+0x10C8数组指针+0", (0xBC0, 0xBC6), "通用7D6到7D1收尾保留门"),
)


CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (0x00442920, 0x004429E1, "Battle顶层合格tick与单一0x442A80调用", "6105e650761863fc805859a9538164a6a78053b817d42660484906fcff0a788a"),
    (0x00442A80, 0x00442B58, "16角色统一状态局部状态机", "3f90fee5899bd2deeb923fbfb76ff131d2f021fdb30aa4003d231571ccdeebbc"),
    (0x00442B58, 0x00442C23, "当前角色门与BBC到BB9条件回收", "c4e58ec5cc058d1e8e9b8e8290312b2e37c8fbb1a7b4f38c38564d41f327c3a6"),
    (0x00442CC3, 0x00442DBE, "16角色状态/相邻状态收尾与BC0_BC6保留门", "0e5ac96ed4b3686a9e3c0b4c9811f697a9cbbb11219e35d74bda6ba684dca2f0"),
    (0x00442E90, 0x00442F78, "统一状态BBB分支与BC1写入", "6ac2839965f506e87df7ad65be027c6a5141bdebb3328db011be789a5723ed49"),
    (0x00442F89, 0x00442FE8, "统一状态数组第二局部扫描", "2dcd533a15080782ce325b5bdea0be5c969f0a8cee56819e9a302ed902f99e9a"),
    (0x004430D8, 0x0044313A, "索引角色状态BBB消费者", "3b90b785dc546ebc19f33f7bdf88805bb2f4dd925adf323ab5aca4dfee633570"),
    (0x00443310, 0x00443461, "class分派状态写入", "a94fa0e2c0ed150a0fe3dabda1aa5d9c9b90d3dd58ce74c59c223943cda418cb"),
    (0x00443470, 0x004434DD, "BBE写入路径", "760bd011ddfbf853bed1d9a236e2bc94bb8296c85da72b825b8826bc546503cd"),
    (0x004434F0, 0x00443586, "BC1写入路径", "b7df80f683c94fbd29a860e16a1b7d33c52f056769cc125188316281b1fb4a73"),
)


SHORT_RULES: Tuple[Tuple[int, int, str, str], ...] = (
    (0x004429A3, 0x004429AA, "Battle顶层到0x442A80的单次直接调用", "8bcfe8d6000000"),
    (0x00442A8D, 0x00442A9D, "Battle+0x10D0局部指针、16槽计数与BBA常量", "8db5d0100000bb10000000bfba0b0000"),
    (0x00442AAB, 0x00442ACD, "相邻0x7D4且请求0x67时请求0x68并写BBA", "817ef4d40700007519833e6775146a006a006a688d8e14f3ffffe856f6fdff897ef8"),
    (0x00442ACD, 0x00442AF5, "BBA路径保持0x68请求", "397ef87523833e68741e8b86b8feffff8a481484c975116a006a006a688d8e14f3ffffe82bf6fdff"),
    (0x00442AF5, 0x00442B21, "BB9路径保持0x67请求", "817ef8b90b00007523833e67741e8b8eb8feffff8a411484c075116a006a006a678d8e14f3ffffe8fff5fdff"),
    (0x00442B21, 0x00442B4B, "BBE且相邻0x7D1时请求0x67并回写BB9", "817ef8be0b00007521817ef4d107000075186a006a006a678d8e14f3ffffe8dcf5fdffc746f8b90b0000"),
    (0x00442BEB, 0x00442C20, "BBC在+0x87C为0或+0x72C为-1时回写BB9", "8db5c8100000b9100000008bc68138bc0b000075188b9098fbffff85d2740839b848faffff7506c700b90b000005e40d00004975d8"),
    (0x00442CD3, 0x00442CE0, "统一状态指针减0xCE4还原FightRole并调用0x420440", "8dae1cf3ffff8bcde860d7fdff"),
)


def scan_rel32_calls(image: PeImage, target_va: int) -> List[int]:
    """穷举.text内E8 rel32直接调用者；不把随机四字节窗口当作调用。"""

    text = image.section(".text")
    blob = image.data[text.raw_offset:text.raw_offset + text.raw_size]
    text_va = image.image_base + text.virtual_address
    hits: List[int] = []
    for index in range(0, max(0, len(blob) - 4)):
        if blob[index] != 0xE8:
            continue
        relative = int.from_bytes(blob[index + 1:index + 5], "little", signed=True)
        caller = text_va + index
        if caller + 5 + relative == target_va:
            hits.append(caller)
    return hits


def scan_absolute_value(image: PeImage, value: int) -> List[int]:
    """搜索完整PE中一个绝对32位值，用于冻结vtable函数指针锚点。"""

    literal = struct.pack("<I", value)
    hits: List[int] = []
    pos = 0
    while True:
        found = image.data.find(literal, pos)
        if found < 0:
            break
        # 本游戏PE各section的RVA与文件偏移不是靠猜测换算；逐section反查VA。
        va = None
        for section in image.sections:
            if section.raw_offset <= found < section.raw_offset + section.raw_size:
                va = image.image_base + section.virtual_address + (found - section.raw_offset)
                break
        if va is None and found < len(image.data):
            va = image.image_base + found
        hits.append(int(va))
        pos = found + 1
    return hits


def check_slice(image: PeImage, start: int, expected_hex: str) -> bool:
    return image.read_va(start, len(expected_hex) // 2).hex() == expected_hex


def verify_write_sites(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []
    for site in DIRECT_WRITE_SITES + COMPUTED_WRITE_SITES:
        start = site.assertion_start_va or site.instruction_va
        size = len(site.expected_hex) // 2
        ob = original.read_va(start, size)
        rb = research.read_va(start, size)
        rows.append({
            "写指令VA": f"0x{site.instruction_va:08X}",
            "断言起始VA": f"0x{start:08X}",
            "观察口径": site.view,
            "写入码值": f"0x{site.value:X}",
            "当前入口可达": site.reachable,
            "说明": site.note,
            "实际HEX": ob.hex(),
            "预期HEX": site.expected_hex,
            "双版本逐字节一致": ob == rb,
            "PASS": ob.hex() == site.expected_hex and ob == rb,
        })
    return rows


def verify_slices(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []
    for start, end, label, expected in CRITICAL_SLICES:
        ob = original.read_va(start, end - start)
        rb = research.read_va(start, end - start)
        actual = sha256(ob)
        rows.append({
            "名称": label, "范围": f"0x{start:08X}..0x{end:08X}", "长度": end - start,
            "原版SHA-256": actual, "预期SHA-256": expected,
            "双版本逐字节一致": ob == rb,
            "PASS": actual == expected and ob == rb,
        })
    return rows


def verify_short_rules(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []
    for start, end, label, expected_hex in SHORT_RULES:
        ob = original.read_va(start, end - start)
        rb = research.read_va(start, end - start)
        rows.append({
            "名称": label, "范围": f"0x{start:08X}..0x{end:08X}",
            "实际HEX": ob.hex(), "预期HEX": expected_hex,
            "双版本逐字节一致": ob == rb,
            "PASS": ob.hex() == expected_hex and ob == rb,
        })
    return rows


def verify_displacement_partition(original: PeImage) -> Dict[str, object]:
    ce4_raw_pairs = original.scan_text(struct.pack("<I", STATE_FROM_FIGHT_ROLE))
    ce4_raw = tuple(va for va, _ in ce4_raw_pairs)
    ce4_valid = tuple(va for va, off in ce4_raw_pairs if is_disp32_at(original, off))
    ce4_false = tuple(va for va, off in ce4_raw_pairs if not is_disp32_at(original, off))
    ten_pairs = original.scan_text(struct.pack("<I", STATE_FROM_ANCHOR))
    ten_raw = tuple(va for va, _ in ten_pairs)
    ten_valid = tuple(va for va, off in ten_pairs if is_disp32_at(original, off))

    # 写入点和读取点都换算成“disp32四字节自身VA”，再验证它们恰好覆盖完整集合。
    ce4_write_disp = {site.instruction_va + 2 for site in DIRECT_WRITE_SITES if site.view == "FightRole+0xCE4"}
    ce4_read_disp = {site.instruction_va + 2 for site in CE4_READ_SITES}
    ten_write_disp = {site.instruction_va + 2 for site in DIRECT_WRITE_SITES if site.view == "角色锚点+0x10C8"}
    # 两个SIB读取的disp32在指令+3；其余普通ModRM读取在+2。
    ten_read_disp = {
        site.instruction_va + (3 if site.instruction_va in (0x00442B75, 0x004430E5) else 2)
        for site in ANCHOR_READ_SITES
    }
    ten_lea_disp = {0x00442BED, 0x00442F96}

    return {
        "+0xCE4原始命中": {
            "实际数": len(ce4_raw), "预期数": len(EXPECTED_CE4_RAW_HITS),
            "完整集合一致": ce4_raw == EXPECTED_CE4_RAW_HITS,
            "PASS": ce4_raw == EXPECTED_CE4_RAW_HITS,
        },
        "+0xCE4真实访问分区": {
            "真实disp32数": len(ce4_valid), "直接写": len(ce4_write_disp), "直接读": len(ce4_read_disp),
            "假阳性": [f"0x{x:08X}" for x in ce4_false],
            "分区无遗漏": ce4_write_disp | ce4_read_disp == set(EXPECTED_CE4_DISP32_HITS),
            "分区无重叠": not (ce4_write_disp & ce4_read_disp),
            "PASS": (
                ce4_valid == EXPECTED_CE4_DISP32_HITS
                and ce4_false == (EXPECTED_CE4_FALSE_RAW_HIT,)
                and ce4_write_disp | ce4_read_disp == set(EXPECTED_CE4_DISP32_HITS)
                and not (ce4_write_disp & ce4_read_disp)
            ),
        },
        "+0x10C8真实访问分区": {
            "真实disp32数": len(ten_valid), "直接写": len(ten_write_disp),
            "直接读": len(ten_read_disp), "LEA别名": len(ten_lea_disp),
            "分区无遗漏": ten_write_disp | ten_read_disp | ten_lea_disp == set(EXPECTED_10C8_RAW_HITS),
            "两两无重叠": not (
                (ten_write_disp & ten_read_disp) or (ten_write_disp & ten_lea_disp) or (ten_read_disp & ten_lea_disp)
            ),
            "PASS": (
                ten_raw == EXPECTED_10C8_RAW_HITS
                and ten_valid == EXPECTED_10C8_RAW_HITS
                and ten_write_disp | ten_read_disp | ten_lea_disp == set(EXPECTED_10C8_RAW_HITS)
                and not ((ten_write_disp & ten_read_disp) or (ten_write_disp & ten_lea_disp) or (ten_read_disp & ten_lea_disp))
            ),
        },
    }


def build_report(original: PeImage, research: PeImage) -> Dict[str, object]:
    original_hash = sha256(original.data)
    research_hash = sha256(research.data)
    hash_pass = original_hash == EXPECTED_ORIGINAL_SHA256 and research_hash == EXPECTED_RESEARCH_SHA256
    write_rows = verify_write_sites(original, research)
    slices = verify_slices(original, research)
    short_rules = verify_short_rules(original, research)
    partition = verify_displacement_partition(original)
    calls_442a80 = scan_rel32_calls(original, 0x00442A80)
    vtable_442920 = scan_absolute_value(original, 0x00442920)

    reachable_write_values = sorted({site.value for site in DIRECT_WRITE_SITES + COMPUTED_WRITE_SITES if site.reachable})
    all_write_values = sorted({site.value for site in DIRECT_WRITE_SITES + COMPUTED_WRITE_SITES})
    all_reads = CE4_READ_SITES + ANCHOR_READ_SITES + LOCAL_ALIAS_READ_SITES
    compared_values = sorted({value for site in all_reads for value in site.compared_values})
    compared_without_direct_writer = sorted(set(compared_values) - set(reachable_write_values))

    access_rows = []
    for site in all_reads:
        access_rows.append({
            "读取指令VA": f"0x{site.instruction_va:08X}",
            "观察口径": site.view,
            "显式比较码值": [f"0x{x:X}" for x in site.compared_values],
            "说明": site.note,
        })

    caller_pass = calls_442a80 == [0x004429A5]
    vtable_pass = vtable_442920 == [0x004610D4]
    overall = (
        hash_pass
        and all(item["PASS"] for item in write_rows)
        and all(item["PASS"] for item in slices)
        and all(item["PASS"] for item in short_rules)
        and all(item["PASS"] for item in partition.values())
        and caller_pass and vtable_pass
    )

    return {
        "工具": Path(__file__).name,
        "工具版本": TOOL_VERSION,
        "固化节点": "v0.7D阶段中-固化66",
        "目标": "闭合统一角色动作状态DWORD的两种正位移直接访问分区、局部指针别名写入及Battle tick可达状态转换。",
        "输入": {
            "完全未修改原版RPG.exe": {"路径": str(original.path), "SHA-256": original_hash, "PASS": original_hash == EXPECTED_ORIGINAL_SHA256},
            "历史研究版RPG.exe": {"路径": str(research.path), "SHA-256": research_hash, "PASS": research_hash == EXPECTED_RESEARCH_SHA256},
        },
        "入口与调用面": {
            "0x442A80直接CALL": [f"0x{x:08X}" for x in calls_442a80],
            "预期0x442A80直接CALL": ["0x004429A5"],
            "Battle_vtable_0x442920绝对指针": [f"0x{x:08X}" for x in vtable_442920],
            "预期vtable指针位置": ["0x004610D4"],
            "PASS": caller_pass and vtable_pass,
        },
        "完整正位移访问分区": partition,
        "状态写入点": write_rows,
        "状态读取点": access_rows,
        "关键区间": slices,
        "Battle_tick短规则": short_rules,
        "码值集合": {
            "当前可达直接或局部别名写入": [f"0x{x:X}" for x in reachable_write_values],
            "包含不可达BC7字面写入": [f"0x{x:X}" for x in all_write_values],
            "当前直接或局部别名比较": [f"0x{x:X}" for x in compared_values],
            "有比较但本节点未找到可达静态写入": [f"0x{x:X}" for x in compared_without_direct_writer],
            "边界": "比较存在不等于当前路径必能生产；未找到静态写入也不等于动态绝对不可达。",
        },
        "闭合结论": {
            "16槽tick": "Battle vtable入口0x442920只在0x4429A5直接调用0x442A80；该函数以Battle+0x10D0为局部指针、字段位于-8，并按0xDE4循环16个角色。",
            "BBA规则": "相邻状态+0x10C4为0x7D4且动作请求槽+0x10D0为0x67时，请求0x68并把统一状态写为0xBBA；处于BBA且当前请求不是0x68、相关子对象字节为0时，会再次请求0x68。",
            "BB9规则": "处于0xBB9且当前请求不是0x67、相关子对象字节为0时，请求0x67。",
            "BBE回收": "处于0xBBE且相邻状态为0x7D1时，请求0x67并把统一状态回写0xBB9。",
            "BBC回收": "第二个16槽扫描中，统一状态为0xBBC且FightRole+0x87C为0，或FightRole+0x72C为-1时，回写0xBB9。",
            "BC0_BC6保留": "统一状态为0xBC0或0xBC6时，0x442D10/0x442D1B继续阻挡通常的相邻0x7D6到0x7D1收尾；固化65已证明这里读取的就是FightRole+0xCE4同一DWORD。",
            "访问面": "+0xCE4原始四字节24命中中23项是真实disp32（10写、13读），1项是假阳性；+0x10C8共15项（8写、5读、2个LEA别名），三类分区无遗漏无重叠。",
        },
        "范围限制": [
            "本节点闭合的是当前同版原版EXE的正位移直接访问和三条已知局部指针写入/读取链；未知整体复制、函数指针、运行时自修改或其它版本不在证明范围。",
            "0xBBD/0xBBF/0xBC4/0xBC5在已确认读取面出现，但本节点没有找到可达静态写入；必须继续RAW-PRESERVE，不能称为死状态。",
            "0xBC7写指令真实存在，但在0x443310当前入口下不可达；不可作为可达状态生产者。",
            "0x67/0x68、0x7D1/0x7D4及0xBB9..0xBC7的作者正式枚举名仍UNKNOWN。",
            "本节点不冒充完整动态Battle Trace；只冻结机器可达局部规则与静态调用边界。",
        ],
        "总体PASS": overall,
    }


def write_csv(path: Path) -> None:
    """把访问点展开为便于排序、筛选和实现对照的CSV。"""

    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(["访问类型", "指令VA", "观察口径", "码值", "当前入口可达", "说明"])
        for site in DIRECT_WRITE_SITES + COMPUTED_WRITE_SITES:
            writer.writerow([
                "写入", f"0x{site.instruction_va:08X}", site.view, f"0x{site.value:X}",
                "是" if site.reachable else "否", site.note,
            ])
        for site in CE4_READ_SITES + ANCHOR_READ_SITES + LOCAL_ALIAS_READ_SITES:
            writer.writerow([
                "读取/比较", f"0x{site.instruction_va:08X}", site.view,
                "/".join(f"0x{x:X}" for x in site.compared_values), "是", site.note,
            ])
        writer.writerow(["别名建立", "0x00442BEB", "Battle+0x10C8数组首指针", "-", "是", "按0xDE4步进16槽"])
        writer.writerow(["别名建立", "0x00442F94", "Battle+0x10C8数组首指针", "-", "是", "按0xDE4步进16槽"])
        writer.writerow(["反向别名", "0x00442CD3", "state_pointer-0xCE4=FightRole", "-", "是", "作为this调用0x420440"])
        writer.writerow(["假阳性", "0x0045AB88", "disp8+imm32", "0x0C", "否", "不是[base+0xCE4]"])


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="审计统一角色动作状态DWORD的完整正位移面与Battle tick局部状态机。"
    )
    parser.add_argument("--original-rpg", required=True, type=Path, help="完全未修改原版RPG.exe")
    parser.add_argument("--research-rpg", required=True, type=Path, help="历史研究版RPG.exe")
    parser.add_argument("--json-out", required=True, type=Path, help="输出机器审计JSON")
    parser.add_argument("--csv-out", required=True, type=Path, help="输出状态访问面CSV")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    original = PeImage.load(args.original_rpg)
    research = PeImage.load(args.research_rpg)
    report = build_report(original, research)
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    write_csv(args.csv_out)
    print(f"[固化66] 总体结果：{'PASS' if report['总体PASS'] else 'FAIL'}")
    print(f"[固化66] JSON：{args.json_out}")
    print(f"[固化66] CSV ：{args.csv_out}")
    return 0 if report["总体PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
