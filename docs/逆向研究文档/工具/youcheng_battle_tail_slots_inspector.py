#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 39 槽序列化零块调查器（固化44）。

这个工具只做“读取、计算、核对、输出报告”，不会修改 RPG.exe，也不会修改 multimedia.zip。
它解决的具体问题是：
1. FightRole+0xA34..+0xACC 这 39 个 DWORD 到底从哪里来；
2. 后 11 槽（索引 28..38）是否真的有当前 canonical 正值生产者；
3. Public/API.ENC 中与这 39 槽对应的序列化初值到底是什么；
4. pending 39 槽、runtime 39 槽、writer/commit/decrement/clear 的机器码边界是否仍与固化44结论一致。

注意：这里故意不把后 11 槽命名成 reserved / unused / auxiliary。
目前能证明的是“当前 canonical 资产 + 已证 writer 链下不能生产正值”，而不是“所有版本、所有隐藏路径永远不会使用”。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
import sys
import zipfile
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


# PE32 文件从 DOS 头的 0x3C 位置保存 PE 头偏移。
# 把它写成常量，后面读代码时能直接看出“为什么要读 0x3C”。
DOS_PE_POINTER_OFFSET = 0x3C

# API.ENC 每个角色定义记录的尺寸由原版机器码直接给出：0x350 字节。
API_RECORD_SIZE = 0x350

# 当前 canonical API.ENC 有 255 条 0x350 字节记录。
# 工具会再次从实际文件长度计算并验证，不会只相信这个常量。
EXPECTED_API_RECORD_COUNT = 255

# FightRole+0x84C 是原版把一整条 RoleDefinition 复制进对象的位置。
FIGHTROLE_ROLEDEF_OFFSET = 0x84C

# 39 槽在 RoleDefinition 内从 +0x1E8 开始。
ROLEDEF_RUNTIME_BLOCK_OFFSET = 0x1E8

# FightRole 内实际 runtime 起点 = 0x84C + 0x1E8 = 0xA34。
FIGHTROLE_RUNTIME_BLOCK_OFFSET = FIGHTROLE_ROLEDEF_OFFSET + ROLEDEF_RUNTIME_BLOCK_OFFSET

# 整个状态块有 39 个 DWORD；一个 DWORD 是 4 字节。
RUNTIME_SLOT_COUNT = 39
DWORD_SIZE = 4

# normal raw effect writer 只允许 ID 0..27，因此前 28 槽有已证 canonical 正值 writer。
DIRECT_EFFECT_SLOT_COUNT = 28

# 后 11 槽就是 39 - 28。
TAIL_SLOT_COUNT = RUNTIME_SLOT_COUNT - DIRECT_EFFECT_SLOT_COUNT

# pending 数组在 FightRole+0xCC 开始，也恰好有 39 个 DWORD。
PENDING_BLOCK_OFFSET = 0xCC

# API.ENC 的 XOR 规则：第 i 个字节与 (i % 255) 异或。
# 这是项目早期已经闭合的公共数据解码规则，固化44只重放，不重新发明格式。
ENC_XOR_PERIOD = 255


@dataclass(frozen=True)
class PESection:
    """保存一个 PE section 最少需要的字段，方便把虚拟地址换算成文件偏移。"""

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass(frozen=True)
class PEImage:
    """保存 PE 映像基址和 section 表。这里只实现固化44需要的只读功能。"""

    data: bytes
    image_base: int
    sections: tuple[PESection, ...]

    def va_to_offset(self, va: int) -> int:
        """把运行时虚拟地址 VA 换成 EXE 文件里的字节偏移。"""

        # RVA 就是 VA 减去 ImageBase。例如 0x41F580 - 0x400000 = 0x1F580。
        rva = va - self.image_base

        # 逐个 section 看这个 RVA 落在哪一段。
        for section in self.sections:
            # virtual_size 描述内存大小，raw_size 描述磁盘大小。
            # 取较大值能兼容“内存比磁盘大”或“磁盘按对齐补齐”的常见 PE 情况。
            covered_size = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + covered_size:
                # section 内相对偏移 = RVA - section RVA。
                inside = rva - section.virtual_address
                # 再加 section 的磁盘 raw_offset，就是文件位置。
                file_offset = section.raw_offset + inside

                # 防止损坏文件把我们带到文件末尾之外。
                if file_offset < 0 or file_offset >= len(self.data):
                    raise ValueError(f"VA 0x{va:08X} 映射到文件范围之外")
                return file_offset

        raise ValueError(f"VA 0x{va:08X} 不属于任何已解析 PE section")

    def read_va(self, va: int, size: int) -> bytes:
        """从指定 VA 读取固定数量的机器码字节。"""

        offset = self.va_to_offset(va)
        chunk = self.data[offset : offset + size]
        if len(chunk) != size:
            raise ValueError(f"VA 0x{va:08X} 需要 {size} 字节，但文件剩余不足")
        return chunk

    def read_c_string(self, va: int, limit: int = 256) -> str:
        """读取以 NUL 结尾的 ASCII 字符串，用于验证 Public\\API.ENC 路径。"""

        offset = self.va_to_offset(va)
        end = min(len(self.data), offset + limit)
        raw = self.data[offset:end]
        nul = raw.find(b"\x00")
        if nul < 0:
            raise ValueError(f"VA 0x{va:08X} 的字符串在 {limit} 字节内没有 NUL 结尾")
        return raw[:nul].decode("ascii", errors="strict")

    def text_section(self) -> tuple[int, bytes]:
        """返回 .text 的运行时起始 VA 和磁盘字节，用来扫描 direct E8 caller。"""

        for section in self.sections:
            if section.name == ".text":
                start_va = self.image_base + section.virtual_address
                raw = self.data[section.raw_offset : section.raw_offset + section.raw_size]
                return start_va, raw
        raise ValueError("PE 中找不到 .text section")


def parse_pe32(path: Path) -> PEImage:
    """读取最基本的 PE32 结构。固化44只需要 ImageBase 与 section 表，不依赖第三方库。"""

    data = path.read_bytes()
    if len(data) < 0x100:
        raise ValueError("EXE 太小，不像有效 PE 文件")
    if data[:2] != b"MZ":
        raise ValueError("EXE 缺少 MZ 头")

    # DOS 头 0x3C 存 PE header 的文件偏移。
    pe_offset = struct.unpack_from("<I", data, DOS_PE_POINTER_OFFSET)[0]
    if pe_offset + 24 > len(data) or data[pe_offset : pe_offset + 4] != b"PE\x00\x00":
        raise ValueError("EXE 缺少有效 PE\\0\\0 签名")

    # COFF header 紧跟在 4 字节 PE 签名之后。
    coff = pe_offset + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_header_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20

    # 当前游戏是 PE32，OptionalHeader.Magic 应为 0x10B。
    magic = struct.unpack_from("<H", data, optional)[0]
    if magic != 0x10B:
        raise ValueError(f"只支持本项目 PE32；当前 OptionalHeader.Magic=0x{magic:04X}")

    # PE32 的 ImageBase 位于 optional header +0x1C。
    image_base = struct.unpack_from("<I", data, optional + 0x1C)[0]

    # section table 紧跟完整 optional header。
    section_table = optional + optional_header_size
    sections: list[PESection] = []
    for index in range(section_count):
        offset = section_table + index * 40
        if offset + 40 > len(data):
            raise ValueError("section table 超出文件范围")

        # section 名最多 8 字节，以 NUL 截断。
        name = data[offset : offset + 8].split(b"\x00", 1)[0].decode("ascii", errors="replace")
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", data, offset + 8)
        sections.append(
            PESection(
                name=name,
                virtual_address=virtual_address,
                virtual_size=virtual_size,
                raw_offset=raw_offset,
                raw_size=raw_size,
            )
        )

    return PEImage(data=data, image_base=image_base, sections=tuple(sections))


def scan_direct_e8_callers(image: PEImage, target_va: int) -> list[int]:
    """扫描 .text 中所有 E8 rel32，并返回真正跳到 target_va 的 call 指令地址。"""

    text_va, text = image.text_section()
    callers: list[int] = []

    # E8 指令总长 5 字节：1 字节 opcode + 4 字节有符号相对位移。
    for offset in range(0, len(text) - 4):
        if text[offset] != 0xE8:
            continue

        # rel32 是小端 signed int32。
        relative = struct.unpack_from("<i", text, offset + 1)[0]
        caller_va = text_va + offset
        destination = caller_va + 5 + relative
        if destination == target_va:
            callers.append(caller_va)

    return callers


def decode_api_enc(raw: bytes) -> bytes:
    """按 byte ^ (index % 255) 解密 Public/API.ENC。"""

    # bytearray 适合逐字节写入，最后再转不可变 bytes。
    decoded = bytearray(len(raw))
    for index, value in enumerate(raw):
        decoded[index] = value ^ (index % ENC_XOR_PERIOD)
    return bytes(decoded)


def find_api_member(archive: zipfile.ZipFile) -> str:
    """在 multimedia.zip 中大小写不敏感地寻找唯一 Public/Api.enc。"""

    candidates: list[str] = []
    for name in archive.namelist():
        normalized = name.replace("\\", "/").lower()
        if normalized.endswith("/public/api.enc") or normalized == "public/api.enc" or normalized == "api.enc":
            candidates.append(name)

    if len(candidates) != 1:
        raise ValueError(f"期望 multimedia.zip 中恰好一份 Public/API.ENC，实际找到 {len(candidates)} 份：{candidates}")
    return candidates[0]


def inspect_api_slots(multimedia_zip: Path) -> dict:
    """解密 API.ENC，并统计 255 条记录里的 39 DWORD 与后 11 DWORD。"""

    with zipfile.ZipFile(multimedia_zip, "r") as archive:
        member = find_api_member(archive)
        encrypted = archive.read(member)

    decoded = decode_api_enc(encrypted)

    # 整除 0x350 是最基础的结构检查；如果不整除，就不能再按固定角色记录解析。
    if len(decoded) % API_RECORD_SIZE != 0:
        raise ValueError(
            f"API.ENC 解密后长度 0x{len(decoded):X} 不能被记录尺寸 0x{API_RECORD_SIZE:X} 整除"
        )

    record_count = len(decoded) // API_RECORD_SIZE
    if record_count != EXPECTED_API_RECORD_COUNT:
        raise ValueError(f"API.ENC 记录数期望 {EXPECTED_API_RECORD_COUNT}，实际 {record_count}")

    # 这个列表保存每条角色记录的详细统计，后面可以直接输出 CSV。
    per_record: list[dict] = []

    all_values: list[int] = []
    tail_values: list[int] = []

    for record_index in range(record_count):
        record_start = record_index * API_RECORD_SIZE
        record = decoded[record_start : record_start + API_RECORD_SIZE]

        # 39 个 DWORD 从 RoleDefinition+0x1E8 开始，连续读 39 次。
        values = [
            struct.unpack_from("<I", record, ROLEDEF_RUNTIME_BLOCK_OFFSET + slot * DWORD_SIZE)[0]
            for slot in range(RUNTIME_SLOT_COUNT)
        ]

        # 后 11 槽就是索引 28..38。
        tail = values[DIRECT_EFFECT_SLOT_COUNT:]

        # 统计非零项而不是只做“是否全零”，这样未来换版本时工具仍有诊断价值。
        nonzero_slots = [slot for slot, value in enumerate(values) if value != 0]
        nonzero_tail_slots = [
            DIRECT_EFFECT_SLOT_COUNT + local_slot
            for local_slot, value in enumerate(tail)
            if value != 0
        ]

        per_record.append(
            {
                "record_index": record_index,
                "all_39_nonzero_count": len(nonzero_slots),
                "all_39_nonzero_slots": nonzero_slots,
                "tail_11_nonzero_count": len(nonzero_tail_slots),
                "tail_11_nonzero_slots": nonzero_tail_slots,
            }
        )
        all_values.extend(values)
        tail_values.extend(tail)

    return {
        "zip_member": member,
        "encrypted_size": len(encrypted),
        "decoded_size": len(decoded),
        "decoded_sha256": hashlib.sha256(decoded).hexdigest(),
        "record_size": API_RECORD_SIZE,
        "record_count": record_count,
        "runtime_block_roledef_offset": ROLEDEF_RUNTIME_BLOCK_OFFSET,
        "runtime_block_fightrole_offset": FIGHTROLE_RUNTIME_BLOCK_OFFSET,
        "runtime_slot_count": RUNTIME_SLOT_COUNT,
        "direct_effect_slot_count": DIRECT_EFFECT_SLOT_COUNT,
        "tail_slot_count": TAIL_SLOT_COUNT,
        "all_39_dword_count": len(all_values),
        "all_39_zero_count": sum(value == 0 for value in all_values),
        "all_39_nonzero_count": sum(value != 0 for value in all_values),
        "tail_11_dword_count": len(tail_values),
        "tail_11_zero_count": sum(value == 0 for value in tail_values),
        "tail_11_nonzero_count": sum(value != 0 for value in tail_values),
        "per_record": per_record,
    }


def expected_machine_assertions() -> list[tuple[int, bytes, str]]:
    """列出固化44最关键的机器码断言。每条都只断言与本节点结论直接相关的最小字节。"""

    return [
        # 0x41F580 的路径参数：push 0x469F2C，后面会再验证该地址字符串正是 Public\\API.ENC。
        (0x0041F5B1, bytes.fromhex("68 2C 9F 46 00"), "API.ENC 路径常量入栈"),
        # role_id * 0x350 + 0x350 的尺寸/边界算式；这里保留完整算式字节，避免只看常量猜结构。
        (0x0041F5EC, bytes.fromhex("8D 04 49 8D 04 C0 D1 E0 2B C1 C1 E0 04 05 50 03 00 00"), "role_id×0x350+0x350 边界算式"),
        # 正常路径 rep movsd count=0xD4；0xD4 DWORD = 0x350 bytes。
        (0x0041F661, bytes.fromhex("B9 D4 00 00 00 F3 A5"), "正常路径整条0x350 RoleDefinition复制"),
        # fallback 路径同样 count=0xD4，再执行 rep movsd。
        (0x0041F6B2, bytes.fromhex("B9 D4 00 00 00 8B F3 8B FA F3 A5"), "fallback路径整条0x350 RoleDefinition复制"),
        # constructor 先准备 0xD4 DWORD 的清零数量。
        (0x0041F166, bytes.fromhex("B9 D4 00 00 00"), "FightRole内嵌RoleDefinition清零计数"),
        # 清零目标就是 FightRole+0x84C。
        (0x0041F171, bytes.fromhex("8D BE 4C 08 00 00"), "FightRole+0x84C RoleDefinition目标"),
        # rep stosd 真正执行整块清零。
        (0x0041F184, bytes.fromhex("F3 AB"), "FightRole内嵌RoleDefinition rep stosd清零"),
        # pending 清零循环起点是 FightRole+0xCC。
        (0x0041F284, bytes.fromhex("8D 8E CC 00 00 00"), "pending 39槽起点 FightRole+0xCC"),
        # 循环上界比较 0x27，即十进制39个元素。
        (0x0041F297, bytes.fromhex("83 F8 27"), "pending初始化循环39项上界"),
        # commit 从 pending 偏移 0xCC 起步。
        (0x0042160D, bytes.fromhex("B8 CC 00 00 00"), "commit循环pending起始偏移"),
        # runtime目标 = pending偏移 + 0x968；0xCC+0x968=0xA34。
        (0x0042162A, bytes.fromhex("89 94 01 68 09 00 00"), "pending→runtime变量索引commit"),
        # 0x168 是 pending末端后一项，因此实际处理 0xCC..0x164 共39项。
        (0x00421642, bytes.fromhex("3D 68 01 00 00"), "commit循环39项终点"),
        # canonical effect writer 明确把 raw ID 限制为 <=27。
        (0x00422C4D, bytes.fromhex("83 F8 1B"), "normal effect writer的ID<=27硬边界"),
        # 通过 raw ID 变量索引写 FightRole+0xCC pending。
        (0x00422C67, bytes.fromhex("89 BC 81 CC 00 00 00"), "normal effect writer写pending数组"),
        # runtime通用递减循环从 FightRole+0xA34 开始。
        (0x00420662, bytes.fromhex("8D BE 34 0A 00 00"), "runtime 39槽递减起点"),
        # 递减循环比较索引0x27，即扫满39项。
        (0x00420694, bytes.fromhex("83 FA 27"), "runtime递减循环39项上界"),
        # 直接clear函数同样从 runtime+0xA34 开始。
        (0x004231D1, bytes.fromhex("8D B9 34 0A 00 00"), "runtime直接clear起点"),
        # 但这个直接clear只清0x1C=28个DWORD，体现前28 raw-ID槽的专用边界。
        (0x004231D7, bytes.fromhex("B9 1C 00 00 00"), "runtime直接clear只清前28槽"),
        (0x004231DE, bytes.fromhex("F3 AB"), "runtime前28槽 rep stosd clear"),
        # 0x41F2B0 setup 中唯一 direct call 到 0x41F580。
        (0x0041F30D, bytes.fromhex("E8 6E 02 00 00"), "setup调用API RoleDefinition loader"),
    ]


def run_machine_assertions(image: PEImage) -> list[dict]:
    """逐条比较固化44机器码期望值与当前 EXE 实际字节。"""

    results: list[dict] = []
    for va, expected, meaning in expected_machine_assertions():
        actual = image.read_va(va, len(expected))
        passed = actual == expected
        results.append(
            {
                "va": f"0x{va:08X}",
                "meaning": meaning,
                "expected_hex": expected.hex(" "),
                "actual_hex": actual.hex(" "),
                "pass": passed,
            }
        )
    return results


def build_report(exe_path: Path, multimedia_zip: Path) -> dict:
    """组合 EXE 机器证据和 API.ENC 全资产统计，形成一份自描述 JSON 报告。"""

    image = parse_pe32(exe_path)
    api = inspect_api_slots(multimedia_zip)
    assertions = run_machine_assertions(image)

    # 读字符串本体，避免只看到 push 0x469F2C 就把地址含义写死。
    api_path_string = image.read_c_string(0x00469F2C)

    # direct E8 caller 集合给出“当前静态直接调用面”的严格边界。
    callers = {
        "0x0041F580_api_role_loader": [f"0x{x:08X}" for x in scan_direct_e8_callers(image, 0x0041F580)],
        "0x0041F2B0_role_setup": [f"0x{x:08X}" for x in scan_direct_e8_callers(image, 0x0041F2B0)],
        "0x004231D0_runtime_first28_clear": [f"0x{x:08X}" for x in scan_direct_e8_callers(image, 0x004231D0)],
        "0x00420440_runtime_39_decrement": [f"0x{x:08X}" for x in scan_direct_e8_callers(image, 0x00420440)],
    }

    expected_callers = {
        "0x0041F580_api_role_loader": ["0x0041F30D"],
        "0x0041F2B0_role_setup": ["0x0044267C", "0x00443F39"],
        "0x004231D0_runtime_first28_clear": ["0x0041F27D", "0x0042329B", "0x00442708", "0x00443F40"],
        "0x00420440_runtime_39_decrement": ["0x00442CDB"],
    }

    caller_pass = callers == expected_callers
    assertion_pass_count = sum(item["pass"] for item in assertions)

    # 固化44的几个“必须同时成立”的条件集中到一处，避免只看某一个PASS就误判。
    checks = {
        "machine_assertions_all_pass": assertion_pass_count == len(assertions),
        "api_path_string_is_Public_API_ENC": api_path_string == r"Public\API.ENC",
        "direct_callers_match_expected": caller_pass,
        "api_record_count_is_255": api["record_count"] == EXPECTED_API_RECORD_COUNT,
        "api_all_39_slots_are_zero": api["all_39_nonzero_count"] == 0,
        "api_tail_11_slots_are_zero": api["tail_11_nonzero_count"] == 0,
        "all_39_value_count_is_9945": api["all_39_dword_count"] == EXPECTED_API_RECORD_COUNT * RUNTIME_SLOT_COUNT,
        "tail_11_value_count_is_2805": api["tail_11_dword_count"] == EXPECTED_API_RECORD_COUNT * TAIL_SLOT_COUNT,
    }

    # 只有所有独立检查同时为真，overall_pass 才能为真。
    overall_pass = all(checks.values())

    return {
        "tool": "youcheng_battle_tail_slots_inspector.py",
        "solidification": 44,
        "mode": "read-only-static-verification",
        "inputs": {
            "exe": str(exe_path),
            "exe_size": len(image.data),
            "exe_sha256": hashlib.sha256(image.data).hexdigest(),
            "multimedia_zip": str(multimedia_zip),
            "multimedia_zip_sha256": hashlib.sha256(multimedia_zip.read_bytes()).hexdigest(),
        },
        "layout": {
            "fightrole_role_definition_offset": f"0x{FIGHTROLE_ROLEDEF_OFFSET:X}",
            "role_definition_runtime_block_offset": f"0x{ROLEDEF_RUNTIME_BLOCK_OFFSET:X}",
            "fightrole_runtime_block_offset": f"0x{FIGHTROLE_RUNTIME_BLOCK_OFFSET:X}",
            "fightrole_pending_block_offset": f"0x{PENDING_BLOCK_OFFSET:X}",
            "runtime_slot_count": RUNTIME_SLOT_COUNT,
            "direct_effect_slot_indices": "0..27",
            "tail_slot_indices": "28..38",
            "tail_fightrole_range": "0xAA4..0xACC",
        },
        "api_path_string": api_path_string,
        "api": api,
        "machine_assertions": assertions,
        "machine_assertion_pass_count": assertion_pass_count,
        "machine_assertion_total": len(assertions),
        "direct_e8_callers": callers,
        "expected_direct_e8_callers": expected_callers,
        "checks": checks,
        "overall_pass": overall_pass,
        "conclusion": {
            "confirmed": [
                "FightRole+0x84C 接收 Public/API.ENC 的完整 0x350-byte RoleDefinition 记录。",
                "FightRole+0xA34..+0xACC 对应 RoleDefinition/API.ENC +0x1E8..+0x280 的 39 DWORD 原位副本。",
                "当前 canonical API.ENC 的 255 条记录中，该 39-DWORD 块 9945/9945 个 DWORD 全为 0。",
                "pending 数组初始化覆盖 39 槽；commit 与 runtime decrement 结构也覆盖完整 39 槽。",
                "当前已证 normal effect writer 在写 pending 前硬限制 raw ID<=27，因此只能生产前 28 槽的 canonical 正值。",
                "后 11 槽在当前 canonical 资产 + 已证 writer 链下没有正值生产者。",
            ],
            "must_not_overclaim": [
                "不能把后11槽命名为 reserved、unused、dead 或 auxiliary。",
                "不能把当前 canonical 静态边界扩大成所有版本、所有间接路径绝对不可写。",
                "兼容引擎仍应保留完整39槽布局与通用commit/decrement覆盖范围。",
            ],
        },
    }


def write_record_csv(path: Path, report: dict) -> None:
    """把 255 条 API 记录的零块统计写成严格 CSV，方便人工抽查和后续版本对比。"""

    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle, lineterminator="\n")
        writer.writerow(
            [
                "record_index",
                "all_39_nonzero_count",
                "all_39_nonzero_slots",
                "tail_11_nonzero_count",
                "tail_11_nonzero_slots",
            ]
        )
        for row in report["api"]["per_record"]:
            writer.writerow(
                [
                    row["record_index"],
                    row["all_39_nonzero_count"],
                    ";".join(str(value) for value in row["all_39_nonzero_slots"]),
                    row["tail_11_nonzero_count"],
                    ";".join(str(value) for value in row["tail_11_nonzero_slots"]),
                ]
            )


def parse_args(argv: Iterable[str] | None = None) -> argparse.Namespace:
    """建立命令行参数。所有输出文件都是可选的，所以单纯运行也能在终端看到JSON。"""

    parser = argparse.ArgumentParser(
        description="只读核验《幽城幻剑录》Battle 39槽序列化零块与后11槽canonical正值生产边界。"
    )
    parser.add_argument("--exe", type=Path, required=True, help="待核验的 RPG.exe / RPG.exe.org 解包后路径")
    parser.add_argument("--multimedia", type=Path, required=True, help="包含 multimedia/public/Api.enc 的 multimedia.zip")
    parser.add_argument("--json", type=Path, help="可选：把完整调查结果写入此JSON")
    parser.add_argument("--csv", type=Path, help="可选：把255条API记录的39槽/后11槽统计写入此CSV")
    return parser.parse_args(argv)


def main(argv: Iterable[str] | None = None) -> int:
    """程序入口：读取输入、生成报告、可选写文件，并用退出码告诉自动审计是否PASS。"""

    args = parse_args(argv)

    # 先检查输入存在，错误信息比 Python 的深层 FileNotFoundError 更容易看懂。
    if not args.exe.is_file():
        print(f"错误：EXE不存在：{args.exe}", file=sys.stderr)
        return 2
    if not args.multimedia.is_file():
        print(f"错误：multimedia.zip不存在：{args.multimedia}", file=sys.stderr)
        return 2

    try:
        report = build_report(args.exe, args.multimedia)
    except Exception as exc:  # noqa: BLE001 - 命令行调查器需要把所有格式错误统一打印给用户。
        print(f"调查失败：{exc}", file=sys.stderr)
        return 2

    # ensure_ascii=False 保留中文，indent=2 方便人读；末尾补换行，便于Git/文本工具处理。
    json_text = json.dumps(report, ensure_ascii=False, indent=2) + "\n"

    if args.json is not None:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json_text, encoding="utf-8", newline="\n")

    if args.csv is not None:
        write_record_csv(args.csv, report)

    # 即使写了JSON，也在终端打印一个短摘要，方便批处理立即看到PASS/FAIL。
    print(
        "固化44 Battle 39槽调查："
        f"机器断言 {report['machine_assertion_pass_count']}/{report['machine_assertion_total']}；"
        f"API 39槽非零 {report['api']['all_39_nonzero_count']}/{report['api']['all_39_dword_count']}；"
        f"后11槽非零 {report['api']['tail_11_nonzero_count']}/{report['api']['tail_11_dword_count']}；"
        f"overall={'PASS' if report['overall_pass'] else 'FAIL'}"
    )

    # 自动审计约定：0=全部通过，1=程序正常执行但至少一项验证失败。
    return 0 if report["overall_pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
