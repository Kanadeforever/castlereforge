#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 大效果 ID / DamageInfo 固定 8000 下游边界调查器。

这个工具只做“读取”和“验证”，不会修改 RPG.exe，也不会修改 multimedia.zip。
它的目标是重放固化42最关键的三类证据：

1. RPG.exe 中，0x42B0D0 对 signed effect-like ID > 27 的分支，确实把原 ID
   写进 DamageInfo 五槽 ID 数组，并把对应 value/residual 固定写成 8000；
2. 已经证明来源为 FightRole 内联 DamageInfo 的 canonical 后处理根中，只有
   0x422B27 这一条根继续读取五槽 ID/value；而它在真正写 pending 状态前明确
   执行 signed `ID <= 27` 门控，所以大 ID 不进入 normal 28 槽状态通道；
3. 当前 Ail2.ENC 的 19 条真实大 ID 记录全部可以归入已有独立业务路径族：
   复苏特殊物品、按 item ID 清除 runtime 状态、永久能力直接 writer、world action。

“没有 canonical DamageInfo 下游 consumer”不等于“全程序数学意义上绝对不存在任何
间接指针/虚表/未识别别名 consumer”。因此工具刻意把结论限定到当前已经建立 provenance
（指针来源证明）的 Battle DamageInfo 生命周期，不使用“死代码”“无用字段”等过强措辞。
"""

from __future__ import annotations

# argparse 用于解析命令行参数。这样工具可以被人直接运行，也可以被封包回归脚本调用。
import argparse
# csv 用来输出一份人类容易浏览的19条真实大ID记录清单。
import csv
# hashlib 用来记录输入 EXE、ZIP 与解密后 Ail2 的 SHA-256，防止把不同版本证据混用。
import hashlib
# io 让 CSV 可以先在内存里组织，然后统一以 UTF-8-SIG 写盘，Windows Excel 打开中文更稳妥。
import io
# json 用来生成机器可读的完整调查报告。
import json
# struct 用来读取 PE32 头、x86 E8 rel32，以及 Ail2 里的 little-endian int32 字段。
import struct
# sys 负责标准退出码和错误输出。
import sys
# zipfile 直接读取用户给出的 multimedia.zip，不需要把整个包先解压到临时目录。
import zipfile
# Counter 用来统计真实大 ID 的频率，避免人工数错。
from collections import Counter
# Path 统一处理文件路径。
from pathlib import Path
# Any 只用于类型注解，让报告字典的结构更容易阅读。
from typing import Any


TOOL_VERSION = "0.1.0-固化42"
CURRENT_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
EXPECTED_AIL2_SHA256 = "a85d0904e969700b996ee89134582552bfabb98af76321804af34f9896b051a8"

# Ail2.ENC 已由前代工具反复确认：700 条记录，每条 926 字节。
AIL2_RECORD_SIZE = 926
AIL2_RECORD_COUNT = 700

# Ail2 五槽中，value 在 +0xE8..+0xF8，ID 在 +0xFC..+0x10C；每槽都是4字节整数。
AIL2_VALUE_BASE = 0xE8
AIL2_ID_BASE = 0xFC
AIL2_ITEM_ID_OFFSET = 0x20
AIL2_WORLD_ACTION_OFFSET = 0x28
AIL2_NAME_OFFSET = 0x00
AIL2_NAME_SIZE = 32
AIL2_DESCRIPTION_OFFSET = 0x188
AIL2_DESCRIPTION_SIZE = 512

# DamageInfo 在 FightRole 内的内联基址已经由前代机器证据闭合为 +0x170，单目标 stride=0x50。
DAMAGE_INFO_BASE_OFFSET = 0x170
DAMAGE_INFO_STRIDE = 0x50
# 五槽 ID/value 在 DamageInfo 内的实际偏移。
DAMAGE_INFO_ID_BASE = 0x10
DAMAGE_INFO_VALUE_BASE = 0x24
BIG_EFFECT_FIXED_VALUE = 8000
NORMAL_DIRECT_EFFECT_MAX = 27

# 下面四组 item ID 覆盖当前 Ail2 中全部19条含正大ID的真实记录。
REVIVAL_ITEMS = set(range(0x10F, 0x112))
CLEAR_ITEMS = set(range(0x119, 0x11F))
PERMANENT_ITEMS = set(range(0x12D, 0x134))
WORLD_ITEMS = set(range(0x170, 0x173))

# 0x421690 的 selector 表对这些真实状态解除物品的已证结果。
EXPECTED_CLEAR_SELECTORS = {
    0x119: 1,
    0x11A: 2,
    0x11B: 0,
    0x11C: 3,
    0x11D: 4,
    0x11E: 0,
}

# selector 对应的 runtime 清零偏移。selector0 是广泛清理，0x11B 有两项例外。
CLEAR_SELECTOR_EFFECT = {
    0: "广泛清理 FightRole runtime 状态块；0x11B 对 +0xA3C/+0xA68 有条件例外",
    1: "清 FightRole+0xA34",
    2: "清 FightRole+0xA38",
    3: "清 FightRole+0xA3C",
    4: "清 FightRole+0xA60",
}

# 0x43AEC0 的 effect-ID selector 已证映射。这里只记录写入字段，不擅自赋予作者属性名。
PERSISTENT_WRITERS = {
    43: "RoleDefinition+0x9C=1；随后 +0x3C += Ail2+0xD0",
    54: "RoleDefinition+0x3C += Ail2+0xD0",
    55: "RoleDefinition+0x40 += Ail2+0xD0",
    56: "RoleDefinition+0x48 += Ail2+0xD0",
    57: "RoleDefinition+0x4C += Ail2+0xD0",
    58: "RoleDefinition+0x44 += Ail2+0xD0",
    59: "RoleDefinition+0x2C += Ail2+0xD0",
    60: "RoleDefinition+0x34 += Ail2+0xD0",
}

# world item 真正使用的是 Ail2+0x28 的 world-action code；大ID 61/63/64 不能反过来当作该 opcode。
EXPECTED_WORLD_ACTIONS = {0x170: 12, 0x171: 13, 0x172: 14}


class PEImage:
    """只实现固化42需要的最小 PE32 VA 映射与 .text 扫描功能。"""

    def __init__(self, path: Path) -> None:
        # 一次性读取 EXE。文件只有几百 KB，这样后续每个地址检查都不会反复打开文件。
        self.path = path
        self.data = path.read_bytes()
        # PE 文件必须先有 DOS MZ 头；没有就立即报错，避免把错误文件当 RPG.exe 分析。
        if self.data[:2] != b"MZ":
            raise ValueError("输入文件不是 MZ/PE 文件")
        # DOS 头 +0x3C 保存真正 PE 头的文件偏移。
        self.pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]
        # PE 头必须以固定签名 PE\0\0 开始。
        if self.data[self.pe_offset:self.pe_offset + 4] != b"PE\0\0":
            raise ValueError("PE 签名错误")
        # COFF 头 +6 处是 section 数量；+20 处是 OptionalHeader 大小。
        section_count = struct.unpack_from("<H", self.data, self.pe_offset + 6)[0]
        optional_size = struct.unpack_from("<H", self.data, self.pe_offset + 20)[0]
        self.optional_offset = self.pe_offset + 24
        # 当前研究对象是 PE32，因此 magic 必须是 0x10B。
        magic = struct.unpack_from("<H", self.data, self.optional_offset)[0]
        if magic != 0x10B:
            raise ValueError(f"只支持 PE32，当前 magic=0x{magic:04X}")
        # PE32 OptionalHeader +0x1C 是 ImageBase。
        self.image_base = struct.unpack_from("<I", self.data, self.optional_offset + 0x1C)[0]
        # section table 紧跟 OptionalHeader，每项固定40字节。
        section_table = self.optional_offset + optional_size
        self.sections: list[dict[str, int | str]] = []
        for index in range(section_count):
            offset = section_table + index * 40
            name = self.data[offset:offset + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            virtual_size = struct.unpack_from("<I", self.data, offset + 8)[0]
            virtual_address = struct.unpack_from("<I", self.data, offset + 12)[0]
            raw_size = struct.unpack_from("<I", self.data, offset + 16)[0]
            raw_offset = struct.unpack_from("<I", self.data, offset + 20)[0]
            self.sections.append({
                "name": name,
                "virtual_size": virtual_size,
                "virtual_address": virtual_address,
                "raw_size": raw_size,
                "raw_offset": raw_offset,
            })

    def rva_to_offset(self, rva: int) -> int:
        """把相对 ImageBase 的 RVA 换算成文件偏移。"""
        for section in self.sections:
            start = int(section["virtual_address"])
            span = max(int(section["virtual_size"]), int(section["raw_size"]))
            if start <= rva < start + span:
                return int(section["raw_offset"]) + (rva - start)
        raise ValueError(f"RVA 0x{rva:08X} 不在任何 section 中")

    def va_to_offset(self, va: int) -> int:
        """把运行时绝对 VA 换算成文件偏移。"""
        return self.rva_to_offset(va - self.image_base)

    def read_va(self, va: int, size: int) -> bytes:
        """按 VA 读取固定字节。"""
        offset = self.va_to_offset(va)
        return self.data[offset:offset + size]

    def text(self) -> tuple[int, bytes]:
        """返回 .text 的起始 VA 和磁盘实际代码字节。"""
        for section in self.sections:
            if section["name"] == ".text":
                start_va = self.image_base + int(section["virtual_address"])
                raw_offset = int(section["raw_offset"])
                raw_size = int(section["raw_size"])
                return start_va, self.data[raw_offset:raw_offset + raw_size]
        raise ValueError("没有找到 .text section")


def sha256_bytes(data: bytes) -> str:
    """返回字节串的 SHA-256 十六进制文本。"""
    return hashlib.sha256(data).hexdigest()


def machine_check(pe: PEImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    """检查某个 VA 是否仍然包含预期机器码，并保留实际值方便失败时定位。"""
    expected = bytes.fromhex(expected_hex)
    actual = pe.read_va(va, len(expected))
    return {
        "地址": f"0x{va:08X}",
        "含义": meaning,
        "期望机器码": expected.hex(" "),
        "实际机器码": actual.hex(" "),
        "PASS": actual == expected,
    }


def direct_e8_callers(pe: PEImage, target_va: int) -> list[int]:
    """枚举 .text 中 opcode E8 rel32 直接调用到 target_va 的地址。"""
    text_va, code = pe.text()
    callers: list[int] = []
    # E8 后面必须还有4字节位移，所以最后4个字节不能作为候选起点。
    for index in range(0, len(code) - 4):
        if code[index] != 0xE8:
            continue
        # x86 rel32 是有符号 little-endian 32位整数。
        rel = struct.unpack_from("<i", code, index + 1)[0]
        call_va = text_va + index
        destination = call_va + 5 + rel
        if destination == target_va:
            callers.append(call_va)
    return callers


def decode_legacy_xor(data: bytes) -> bytes:
    """按 public ENC 已确认规则 byte ^ (index % 255) 解密。"""
    # 用 bytearray 预分配输出，避免循环中不断拼接不可变 bytes。
    out = bytearray(len(data))
    for index, value in enumerate(data):
        # 每个字节只和当前位置模255做 XOR；这是游戏公共数据的既有解密规则。
        out[index] = value ^ (index % 255)
    return bytes(out)


def cp950_cstr(record: bytes, offset: int, size: int) -> str:
    """从固定字段中取 NUL 结束的 Big5/CP950 中文字符串。"""
    field = record[offset:offset + size]
    field = field.split(b"\0", 1)[0]
    return field.decode("cp950", errors="replace")


def load_ail2(zip_path: Path) -> tuple[str, bytes]:
    """从 multimedia.zip 定位 Ail2.ENC，解密后返回内部路径和明文。"""
    with zipfile.ZipFile(zip_path, "r") as archive:
        # 路径大小写理论上固定，但这里用 lower() 避免不同压缩工具改变目录大小写后无法重放。
        matches = [name for name in archive.namelist() if name.replace("\\", "/").lower().endswith("/public/ail2.enc")]
        if not matches:
            # 兼容 ZIP 根目录直接放 Ail2.ENC 的极端情况。
            matches = [name for name in archive.namelist() if name.replace("\\", "/").lower().endswith("ail2.enc")]
        if not matches:
            raise FileNotFoundError("multimedia.zip 中没有找到 Ail2.ENC")
        # 若出现多个副本，优先最短路径；正常当前包只会有一个。
        name = sorted(matches, key=lambda item: (len(item), item.lower()))[0]
        encrypted = archive.read(name)
    decoded = decode_legacy_xor(encrypted)
    expected_size = AIL2_RECORD_SIZE * AIL2_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(f"Ail2 解密后大小 {len(decoded)} != {expected_size}")
    return name, decoded


def classify_item(item_id: int, ids: list[int], world_action: int) -> tuple[str, str]:
    """把真实大ID记录按已经证明的独立业务入口分类；未知时宁可返回UNKNOWN。"""
    if item_id in REVIVAL_ITEMS:
        return (
            "复苏特殊物品族",
            "Battle 另有按 item ID 0x10F..0x111 的特殊恢复控制流；不能把 effect ID43 自身命名为复苏opcode。",
        )
    if item_id in CLEAR_ITEMS:
        selector = EXPECTED_CLEAR_SELECTORS.get(item_id)
        return (
            "按item-ID状态解除族",
            f"0x421690 读取 Ail2+0x20；item 0x{item_id:X} 选择 selector {selector}，{CLEAR_SELECTOR_EFFECT.get(selector, '未知清理分支')}。",
        )
    if item_id in PERMANENT_ITEMS:
        active_big_ids = [effect_id for effect_id in ids if effect_id > NORMAL_DIRECT_EFFECT_MAX]
        writers = [PERSISTENT_WRITERS.get(effect_id, "UNKNOWN") for effect_id in active_big_ids]
        return (
            "永久能力源记录writer族",
            "0x43AEC0 直接读取 Ail2 五槽ID/value，并由 Ail2+0xD0 作为实际增量写 RoleDefinition；当前大ID映射=" + "；".join(writers),
        )
    if item_id in WORLD_ITEMS:
        return (
            "world-action族",
            f"真正分派字段是 Ail2+0x28={world_action}；大ID {','.join(str(x) for x in ids if x > 27)} 不能当作 world-action opcode。",
        )
    return ("UNKNOWN", "当前没有已证独立路径；不得猜测。")


def parse_big_id_records(decoded: bytes) -> list[dict[str, Any]]:
    """扫描700条 Ail2，返回所有至少包含一个 signed ID>27 且对应value>0/ID本身存在的记录。"""
    rows: list[dict[str, Any]] = []
    for physical_index in range(AIL2_RECORD_COUNT):
        start = physical_index * AIL2_RECORD_SIZE
        record = decoded[start:start + AIL2_RECORD_SIZE]
        values = [struct.unpack_from("<i", record, AIL2_VALUE_BASE + slot * 4)[0] for slot in range(5)]
        ids = [struct.unpack_from("<i", record, AIL2_ID_BASE + slot * 4)[0] for slot in range(5)]
        big_slots = [slot for slot, effect_id in enumerate(ids) if effect_id > NORMAL_DIRECT_EFFECT_MAX]
        if not big_slots:
            continue
        item_id = struct.unpack_from("<I", record, AIL2_ITEM_ID_OFFSET)[0]
        world_action = struct.unpack_from("<i", record, AIL2_WORLD_ACTION_OFFSET)[0]
        family, route = classify_item(item_id, ids, world_action)
        rows.append({
            "物理记录索引": physical_index,
            "物理记录索引十六进制": f"0x{physical_index:X}",
            "Ail2加20物品ID": item_id,
            "Ail2加20物品ID十六进制": f"0x{item_id:X}",
            "名称": cp950_cstr(record, AIL2_NAME_OFFSET, AIL2_NAME_SIZE),
            "说明": cp950_cstr(record, AIL2_DESCRIPTION_OFFSET, AIL2_DESCRIPTION_SIZE),
            "五槽value": values,
            "五槽ID": ids,
            "大ID槽位": big_slots,
            "大ID": [ids[slot] for slot in big_slots],
            "Ail2加28": world_action,
            "独立路径族": family,
            "独立路径证据摘要": route,
        })
    return rows


def build_machine_checks(pe: PEImage) -> list[dict[str, Any]]:
    """集中列出固化42必须逐字节重放的核心机器码。"""
    checks: list[dict[str, Any]] = []
    # 大ID生成分支：ID>27后原ID写DamageInfo+0x10，value固定8000写+0x24。
    checks.append(machine_check(
        pe, 0x0042B313,
        "83 ff 1b 7e 15 8b 44 24 40 89 7c 98 10 c7 44 98 24 40 1f 00 00 e9 50 02 00 00",
        "0x42B0D0：signed ID>27写原ID并固定value=8000",
    ))
    # DamageInfo canonical基址根1：只观察每目标+0 active/success byte，stride 0x50。
    checks.append(machine_check(
        pe, 0x0041FEEA,
        "8d be 70 01 00 00 8d 96 88 00 00 00",
        "canonical DamageInfo根：FightRole+0x170，后续此路径只用record+0标志",
    ))
    # mode1从DamageInfo+0x08起步，处理主结果字段而不是五槽ID/value。
    checks.append(machine_check(
        pe, 0x00420FC7,
        "8d 9f 78 01 00 00 89 54 24 24 8b 44 24 24 8b 30",
        "mode1：工作指针=FightRole+0x178=DamageInfo+0x08",
    ))
    # mode2从DamageInfo+0x04起步，处理主结果/状态提交相关字段。
    checks.append(machine_check(
        pe, 0x00421306,
        "8d 9f 74 01 00 00 89 44 24 18 8b 4c 24 18 8b 31",
        "mode2：工作指针=FightRole+0x174=DamageInfo+0x04",
    ))
    # 唯一已证五槽下游根：真正从FightRole+0x170开始。
    checks.append(machine_check(
        pe, 0x00422B27,
        "8d 9e 70 01 00 00 8b 4c 24 18 c6 44 24 10 00",
        "五槽后处理根：EBX=FightRole+0x170",
    ))
    # 读取value，再读取对应raw ID，并在写pending前明确排除signed ID>27。
    checks.append(machine_check(
        pe, 0x00422C3A,
        "8d 6b 24 8b 7d 00 85 ff 7e 2a 8b 44 24 1c 03 c2 8b 04 86 83 f8 1b 7f 1c",
        "五槽pending writer：value>0后读取raw ID；signed ID>27直接跳过",
    ))
    checks.append(machine_check(
        pe, 0x00422C5D,
        "83 bc 81 34 0a 00 00 00 7f 07 89 bc 81 cc 00 00 00",
        "只有门控通过的ID才写FightRole+0xCC+id*4 pending槽",
    ))
    # 两个结果/UI路径同样以DamageInfo+0作为条件，不读取五槽ID/value。
    checks.append(machine_check(
        pe, 0x00423732,
        "8d 9e 70 01 00 00 8b 0f 8b 0c 8d 94 fd 89 00",
        "结果/UI路径根：DamageInfo基址；后续只检查record+0标志",
    ))
    checks.append(machine_check(
        pe, 0x0042380D,
        "8d be 70 01 00 00 80 3f 00 74 32",
        "另一结果/UI路径根：DamageInfo基址；直接比较record+0 byte",
    ))
    # Ail2状态解除dispatcher：读取selected Ail2+0x20，并用0x421990 byte selector表分派。
    checks.append(machine_check(
        pe, 0x004216C7,
        "8b 7e 20 8d b7 f8 fe ff ff 83 fe 64 0f 87 8b 02 00 00 33 c0 8a 86 90 19 42 00 ff 24 85 6c 19 42 00",
        "0x421690：按Ail2+0x20 item ID选择状态解除分支",
    ))
    # 0x43AEC0：source Ail2五槽value gate + ID 43..60 switch，独立于DamageInfo固定8000。
    checks.append(machine_check(
        pe, 0x0043AFF3,
        "8b 57 ec 8b 85 d0 00 00 00 85 d2 7e 44 8b 17 8d 72 d5 83 fe 11 77 3a 33 d2 8a 96 54 b1 43 00 ff 24 95 30 b1 43 00",
        "0x43AEC0：直接消费Ail2源记录五槽，而不是DamageInfo五槽",
    ))
    return checks


def parse_selector_tables(pe: PEImage) -> dict[str, Any]:
    """读取两个已证 selector 表，直接用当前 EXE 数据验证真实大ID物品的分派。"""
    # 0x421990 对 item ID 0x108..0x16C 共101个 byte selector。
    item_selectors = pe.read_va(0x00421990, 0x65)
    observed_clear = {item_id: item_selectors[item_id - 0x108] for item_id in EXPECTED_CLEAR_SELECTORS}
    clear_pass = observed_clear == EXPECTED_CLEAR_SELECTORS

    # 0x43B154 对 effect ID 43..60 共18个byte selector；8表示default/no-op。
    effect_selectors = list(pe.read_va(0x0043B154, 18))
    expected_effect_selectors = [0] + [8] * 10 + [1, 2, 3, 4, 5, 6, 7]
    effect_pass = effect_selectors == expected_effect_selectors
    return {
        "ArticleAil2状态解除selector": {
            "表地址": "0x00421990",
            "真实物品期望": {f"0x{k:X}": v for k, v in EXPECTED_CLEAR_SELECTORS.items()},
            "真实物品实际": {f"0x{k:X}": v for k, v in observed_clear.items()},
            "PASS": clear_pass,
        },
        "Ail2源记录大IDwriter_selector": {
            "表地址": "0x0043B154",
            "覆盖effect_ID": "43..60",
            "实际18字节": effect_selectors,
            "期望18字节": expected_effect_selectors,
            "解释": "ID43->case0；44..53->default8；54..60->case1..7",
            "PASS": effect_pass,
        },
    }


def build_report(pe: PEImage, multimedia_zip: Path) -> tuple[dict[str, Any], list[dict[str, Any]]]:
    """组合 EXE 与 Ail2 证据，生成固化42完整报告。"""
    checks = build_machine_checks(pe)
    selector_tables = parse_selector_tables(pe)
    ail2_name, decoded = load_ail2(multimedia_zip)
    rows = parse_big_id_records(decoded)

    # 统计每个大ID出现多少个槽；与前代“29槽/19记录”形成自动回归。
    all_big_ids = [effect_id for row in rows for effect_id in row["大ID"]]
    frequency = Counter(all_big_ids)
    # 只有四个已证家族才算当前真实大ID记录全部获得独立路径分类。
    family_counts = Counter(row["独立路径族"] for row in rows)
    all_rows_classified = all(row["独立路径族"] != "UNKNOWN" for row in rows)

    # world-action 三条必须再次从真实Ail2读取12/13/14，避免只复述旧文档。
    observed_world = {row["Ail2加20物品ID"]: row["Ail2加28"] for row in rows if row["Ail2加20物品ID"] in WORLD_ITEMS}
    world_pass = observed_world == EXPECTED_WORLD_ACTIONS

    # 当前 .org 整文件与历史canonical不同，这里只记录身份，不把“不等”判成失败。
    rpg_sha = sha256_bytes(pe.data)
    zip_sha = sha256_bytes(multimedia_zip.read_bytes())
    ail2_sha = sha256_bytes(decoded)

    # 0x42B0D0本身的direct caller数量也记录下来，但固化42不依赖“唯一caller”给业务命名。
    callers_42b0d0 = direct_e8_callers(pe, 0x0042B0D0)

    machine_pass = all(item["PASS"] for item in checks)
    selector_pass = all(section["PASS"] for section in selector_tables.values())
    corpus_pass = (
        len(rows) == 19
        and len(all_big_ids) == 29
        and all_rows_classified
        and ail2_sha == EXPECTED_AIL2_SHA256
        and world_pass
    )

    report: dict[str, Any] = {
        "工具": "幽城Battle大效果ID与DamageInfo固定8000下游边界调查器",
        "工具版本": TOOL_VERSION,
        "输入": {
            "RPG路径": str(pe.path),
            "RPG_SHA256": rpg_sha,
            "是否当前RPG.exe.org样本": rpg_sha == CURRENT_RPG_ORG_SHA256,
            "multimedia路径": str(multimedia_zip),
            "multimedia_SHA256": zip_sha,
            "Ail2内部路径": ail2_name,
            "Ail2解码SHA256": ail2_sha,
        },
        "DamageInfo结构": {
            "FightRole内联基址": f"+0x{DAMAGE_INFO_BASE_OFFSET:X}",
            "每目标stride": f"0x{DAMAGE_INFO_STRIDE:X}",
            "五槽ID": "+0x10..+0x20",
            "五槽value": "+0x24..+0x34",
            "大ID门": f"signed ID>{NORMAL_DIRECT_EFFECT_MAX}",
            "大ID生成value": BIG_EFFECT_FIXED_VALUE,
        },
        "机器码断言": checks,
        "机器码断言汇总": {
            "通过": sum(1 for item in checks if item["PASS"]),
            "总数": len(checks),
            "PASS": machine_pass,
        },
        "selector表重放": selector_tables,
        "0x42B0D0_direct_E8_callers": [f"0x{x:08X}" for x in callers_42b0d0],
        "canonical_DamageInfo后处理根": [
            {
                "地址": "0x0041FEEA",
                "根": "FightRole+0x170",
                "已证用途": "每目标DamageInfo+0 active/success byte；不读取五槽ID/value",
            },
            {
                "地址": "0x00420FC7",
                "根": "FightRole+0x178 = DamageInfo+0x08",
                "已证用途": "mode1主结果/资源字段；不读取五槽ID/value",
            },
            {
                "地址": "0x00421306",
                "根": "FightRole+0x174 = DamageInfo+0x04",
                "已证用途": "mode2主结果/状态提交周边字段；五槽pending读取不在此根完成",
            },
            {
                "地址": "0x00422B27",
                "根": "FightRole+0x170",
                "已证用途": "唯一已证继续遍历五槽ID/value的canonical后处理根；ID>27在0x422C50被排除",
            },
            {
                "地址": "0x00423732",
                "根": "FightRole+0x170",
                "已证用途": "结果/UI分支，只检查DamageInfo+0 byte",
            },
            {
                "地址": "0x0042380D",
                "根": "FightRole+0x170",
                "已证用途": "结果/UI分支，只检查DamageInfo+0 byte",
            },
        ],
        "真实Ail2大ID语料": {
            "记录数": len(rows),
            "大ID槽出现次数": len(all_big_ids),
            "大ID频率": {str(key): frequency[key] for key in sorted(frequency)},
            "独立路径族计数": dict(family_counts),
            "全部19条均有已证独立路径族": all_rows_classified,
            "world_action实际": {f"0x{k:X}": v for k, v in observed_world.items()},
            "world_action期望": {f"0x{k:X}": v for k, v in EXPECTED_WORLD_ACTIONS.items()},
            "world_action_PASS": world_pass,
            "记录": rows,
        },
        "固化42实现结论": [
            "0x42B0D0 对 signed ID>27 确实保留原ID，并把对应 DamageInfo value 固定写成8000；这不是文档猜测。",
            "在当前已建立指针来源证明的 canonical Battle DamageInfo 生命周期中，唯一继续遍历五槽ID/value的后处理根是0x422B27。",
            "0x422C4D/0x422C50在写pending前明确执行signed ID<=27门控，所以ID>27/value8000不进入normal 28槽pending/runtime状态通道。",
            "其余已证DamageInfo根只消费record+0标志或主结果字段，没有发现读取五槽大ID/8000的canonical下游。",
            "当前Ail2全部19条正大ID记录均归入已有独立源记录业务路径族；不能因为它们同时生成DamageInfo 8000，就把8000当作真正持续时间/属性增量/世界动作参数。",
            "兼容引擎应保留大ID与8000作为原版DamageInfo中间产物以保证结构忠实，但不得把ID>27直接映射进前28 runtime槽。",
            "真实业务效果应按已经证明的item-ID/Ail2源字段路径复刻；尚未证明的其他版本/间接consumer继续保持UNKNOWN。",
        ],
        "假阳性踩坑": [
            "全局搜索形如[reg+0x190]会命中0x42BBD2与0x42D49B，但寄存器来源分别属于其他参数结构/管理器数组，不是FightRole+0x190=DamageInfo+0x20。",
            "因此任何offset consumer结论必须同时证明base-register provenance；不能只按位移数值相同就认定为DamageInfo字段。",
        ],
        "证据边界": [
            "本工具闭合的是当前EXE中已证canonical/直接别名DamageInfo后处理边界，不声称穷尽所有可能的函数指针、虚表、手工指针算术或其他版本代码。",
            "19条真实Ail2记录的独立路径分类证明业务效果有其他入口，但不等于每一个effect-like大ID本身已经获得作者业务名称。",
            "effect ID43尤其不能直接命名为复苏opcode；复苏语义来自item ID 0x10F..0x111独立控制流。",
            "world item 0x170..0x172真正的world-action selector是Ail2+0x28=12/13/14；大ID61/63/64不是该selector。",
        ],
        "总体PASS": machine_pass and selector_pass and corpus_pass,
    }
    return report, rows


def write_csv(path: Path, rows: list[dict[str, Any]]) -> None:
    """把19条真实大ID记录输出成严格CSV，数组字段用紧凑JSON字符串保存。"""
    output = io.StringIO(newline="")
    fieldnames = [
        "物理记录索引", "物理记录索引十六进制", "Ail2加20物品ID", "Ail2加20物品ID十六进制",
        "名称", "五槽value", "五槽ID", "大ID槽位", "大ID", "Ail2加28", "独立路径族", "独立路径证据摘要",
    ]
    writer = csv.DictWriter(output, fieldnames=fieldnames, extrasaction="ignore", lineterminator="\n")
    writer.writeheader()
    for row in rows:
        flat = dict(row)
        # CSV单元格不适合直接放Python列表表示；使用合法JSON字符串，任何解析器都能可靠还原。
        for key in ("五槽value", "五槽ID", "大ID槽位", "大ID"):
            flat[key] = json.dumps(flat[key], ensure_ascii=False, separators=(",", ":"))
        writer.writerow(flat)
    # UTF-8-SIG带BOM，Windows Excel双击时更容易自动识别中文；CSV内容仍是标准UTF-8。
    path.write_text(output.getvalue(), encoding="utf-8-sig", newline="")


def parse_args() -> argparse.Namespace:
    """定义命令行参数。"""
    parser = argparse.ArgumentParser(description="重放固化42：Battle大效果ID固定8000与canonical下游边界。")
    parser.add_argument("--rpg", type=Path, required=True, help="解包后的RPG.exe路径")
    parser.add_argument("--multimedia", type=Path, required=True, help="multimedia.zip路径")
    parser.add_argument("--output", type=Path, help="可选：UTF-8 JSON报告输出路径")
    parser.add_argument("--csv", type=Path, help="可选：19条真实大ID记录CSV输出路径")
    return parser.parse_args()


def main() -> int:
    """读取输入、执行全部验证、输出报告，并用退出码告诉封包脚本是否全绿。"""
    args = parse_args()
    if not args.rpg.is_file():
        print(f"错误：找不到RPG.exe：{args.rpg}", file=sys.stderr)
        return 2
    if not args.multimedia.is_file():
        print(f"错误：找不到multimedia.zip：{args.multimedia}", file=sys.stderr)
        return 2
    try:
        pe = PEImage(args.rpg)
        report, rows = build_report(pe, args.multimedia)
    except (OSError, ValueError, struct.error, zipfile.BadZipFile) as exc:
        print(f"错误：固化42调查失败：{exc}", file=sys.stderr)
        return 2

    text = json.dumps(report, ensure_ascii=False, indent=2) + "\n"
    if args.output is not None:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(text, encoding="utf-8", newline="\n")
    if args.csv is not None:
        args.csv.parent.mkdir(parents=True, exist_ok=True)
        write_csv(args.csv, rows)
    print(text, end="")
    return 0 if report["总体PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
