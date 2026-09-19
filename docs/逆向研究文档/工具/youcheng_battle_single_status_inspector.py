#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 七个单分支状态表现槽资源语义与机器行为调查器（固化48）。

这个工具只读取 RPG.exe、Public/Firttech.enc 与 Public/Ail2.ENC。
它不会修改游戏文件，也不会向游戏进程注入代码。

固化46已经证明 Battle 状态 UI 有14个逻辑表现槽；其中7个槽有两个互斥分支，
另外7个槽只有一个非零分支。固化47又把7个双分支槽闭合成7组互反抵消状态。
本工具只调查剩下的7个“单分支槽”，把四类证据接在一起：

1. UI 更新机器码：哪个 RoleDefinition 状态 DWORD 选择哪个 ITF0052 selector；
2. normal effect writer：raw ID 0..27 仍按自身编号写入 pending/runtime 协议；
3. 作者资源文本：Firttech/Ail2 哪些真实记录明确描述该 raw ID 的业务效果；
4. 运行时机器行为：已有专用解除、数值缩放、命中结果门控、特殊状态写入等证据。

非常重要的证据纪律：
- raw1/raw2/raw11/raw14/raw16 已有足以采用作者语义的多路证据；
- raw9 与 raw13 虽然能确定资源族/行为方向，但当前资源没有出现一个可证明是“状态栏正式名”的
  独立名词，因此工具只输出“冰封/凛冻类”“续命护心类”，并明确正式状态名仍未知；
- 工具不会根据 ITF0052 图标长相给状态命名。
"""

from __future__ import annotations

# argparse 负责命令行参数。这样同一个工具既能手工运行，也能被封包回归脚本无交互调用。
import argparse
# csv 用于输出一份便于人工浏览的七状态摘要表。
import csv
# hashlib 用于把输入文件身份写进报告，避免以后把不同版本的 EXE/资源混在一起比较。
import hashlib
# json 用于输出完整、机器可读的调查报告。
import json
# struct 用于读取 PE32 头、x86 little-endian 位移，以及资源记录里的32位整数。
import struct
# Path 让 Windows/Linux 环境下的路径处理保持一致。
from pathlib import Path
# 类型注解不是运行必需，但能让接档者快速看懂每个函数输入输出是什么。
from typing import Any


TOOL_VERSION = "0.7D-solid48.0"

# 当前 `.org` 输入的整文件 SHA。它与早期 canonical Oracle 的整文件 SHA 不同，
# 但固化34以后已经对大量覆盖代码做过机器码等价复核。这里记录它只是为了身份追踪，
# 不把“整文件 SHA 不同”误写成“代码语义不同”。
CURRENT_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"

# Firttech/Ail2 是旧式 XOR 编码资源。下面的尺寸与字段偏移均由前代工具反复验证。
FIRTTECH_RECORD_SIZE = 0x230
FIRTTECH_RECORD_COUNT = 500
FIRTTECH_VALUE_BASE = 0x64
FIRTTECH_ID_BASE = 0x78

AIL2_RECORD_SIZE = 0x39E
AIL2_RECORD_COUNT = 700
AIL2_VALUE_BASE = 0xE8
AIL2_ID_BASE = 0xFC

# FightRole+0xA34 对应 raw effect 0，因此 raw N 的 runtime DWORD 就是 +0xA34+N*4。
RUNTIME_EFFECT_BASE = 0xA34
# RoleDefinition 的同一39槽序列化块从 +0x1E8 开始，所以 raw N 对应 +0x1E8+N*4。
ROLEDEF_EFFECT_BASE = 0x1E8
# normal effect 的 pending 块从 FightRole+0xCC 开始；固化44/45已经证明它有39项维护，
# 但 canonical 正值 writer 在写入前限制 raw ID <= 27。
PENDING_EFFECT_BASE = 0x0CC

# 七个单分支 UI 槽。entry_va 是 `mov eax,[esi+RoleDefinition字段]`；
# push_va 是原版压入 selector 常量的 `push imm8`。
# channel 编号沿用固化46的14路逻辑槽编号；selector 对应 ITF0052 的 Section0 入口编号。
SINGLE_CHANNELS = [
    {"raw_id": 1,  "channel": 6,  "selector": 5,  "entry_va": 0x41CF59, "push_va": 0x41CF69},
    {"raw_id": 2,  "channel": 7,  "selector": 18, "entry_va": 0x41CF8C, "push_va": 0x41CF9C},
    {"raw_id": 11, "channel": 9,  "selector": 20, "entry_va": 0x41D00F, "push_va": 0x41D01F},
    {"raw_id": 13, "channel": 10, "selector": 21, "entry_va": 0x41D042, "push_va": 0x41D052},
    {"raw_id": 14, "channel": 11, "selector": 9,  "entry_va": 0x41D075, "push_va": 0x41D085},
    {"raw_id": 16, "channel": 12, "selector": 16, "entry_va": 0x41D0A8, "push_va": 0x41D0B8},
    {"raw_id": 9,  "channel": 13, "selector": 22, "entry_va": 0x41D0DB, "push_va": 0x41D0EB},
]

# 为了让输出稳定、便于实现层直接消费，这里按 raw ID 排序保存最终保守标签。
# “正式状态名未见”不是缺陷，而是证据边界：我们宁愿少命名，也不把研究者简称伪装成作者术语。
SEMANTIC_LABELS = {
    1:  "麻痹／软瘫",
    2:  "封咒／不能施咒",
    9:  "冰封／凛冻类（作者正式状态栏名未见）",
    11: "逆阙",
    13: "续命护心类（作者正式状态栏名未见）",
    14: "化幻／无形",
    16: "幽冥返召／操偶",
}

# 作者资源文本锚点。每一条都同时指定：资源表、物理记录索引、名称、必须出现的关键短语、
# 以及该记录必须真的包含哪个 positive raw effect。
# raw13 当前 Firttech 没有正样本，所以使用 Ail2 的“血灵神珠”；raw16 则反过来只有 Firttech 强锚点。
AUTHOR_ANCHORS = [
    {"raw_id": 1,  "table": "Firttech", "index": 432, "name": "奪魄鬼咒", "phrase": "麻痺癱軟而無法動彈", "kind": "施加"},
    {"raw_id": 1,  "table": "Ail2",     "index": 314, "name": "蔓陀花苞", "phrase": "通體麻痺", "kind": "施加"},
    {"raw_id": 2,  "table": "Firttech", "index": 435, "name": "封魔神道", "phrase": "不能施咒", "kind": "施加"},
    {"raw_id": 2,  "table": "Ail2",     "index": 362, "name": "封魔神水", "phrase": "無法施咒", "kind": "施加"},
    {"raw_id": 9,  "table": "Firttech", "index": 383, "name": "冰魄晶壁", "phrase": "永凍冰壁", "kind": "施加/状态族"},
    {"raw_id": 9,  "table": "Ail2",     "index": 360, "name": "封凜冰露", "phrase": "凍慄冰嵐", "kind": "施加/状态族"},
    {"raw_id": 11, "table": "Firttech", "index": 460, "name": "血魂之咒", "phrase": "逆闕之效", "kind": "施加"},
    {"raw_id": 11, "table": "Ail2",     "index": 289, "name": "鬼軀化膏", "phrase": "逆闕的後果", "kind": "施加"},
    {"raw_id": 13, "table": "Ail2",     "index": 426, "name": "血靈神珠", "phrase": "續命護心之能", "kind": "施加"},
    {"raw_id": 14, "table": "Firttech", "index": 449, "name": "無方變幻", "phrase": "無形之身", "kind": "施加"},
    {"raw_id": 14, "table": "Ail2",     "index": 364, "name": "時幻沙", "phrase": "刀劍咒法皆不能及", "kind": "施加"},
    {"raw_id": 16, "table": "Firttech", "index": 464, "name": "幽冥返召", "phrase": "戰鬥的「操偶」", "kind": "特殊施加"},
]

# 三个专用解除物品本身没有 normal raw ID；它们走 item-ID selector 分派。
# 因此这里单独保存 item ID 与作者说明，后面再用 RPG.exe 的 selector 表和清零机器码把它们接到具体 runtime 槽。
CURE_ANCHORS = [
    {"raw_id": 1,  "ail2_index": 282, "item_id": 0x11A, "name": "通脈丸", "phrase": "治療軟癱麻病", "selector": 2, "clear_va": 0x421744, "runtime_offset": 0xA38},
    {"raw_id": 2,  "ail2_index": 284, "item_id": 0x11C, "name": "綺羅天香", "phrase": "破除封咒", "selector": 3, "clear_va": 0x42177D, "runtime_offset": 0xA3C},
    {"raw_id": 11, "ail2_index": 285, "item_id": 0x11D, "name": "血仙邪釀", "phrase": "解救逆闕", "selector": 4, "clear_va": 0x4217B6, "runtime_offset": 0xA60},
]


def u16(data: bytes, offset: int) -> int:
    """从 bytes 的指定位置读取 little-endian 无符号16位整数。"""
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    """从 bytes 的指定位置读取 little-endian 无符号32位整数。"""
    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    """从 bytes 的指定位置读取 little-endian 有符号32位整数。"""
    return struct.unpack_from("<i", data, offset)[0]


def sha256_bytes(data: bytes) -> str:
    """计算一段字节的 SHA-256，返回小写十六进制字符串。"""
    return hashlib.sha256(data).hexdigest()


def decode_cp950_cstring(raw: bytes) -> str:
    """读取定长字段中的 C 风格字符串，并按台湾版资源使用的 CP950 解码。"""
    # 先在第一个 NUL 处截断，避免把字段后面的填充字节误当成文字。
    raw = raw.split(b"\0", 1)[0]
    # errors=replace 只在遇到损坏字节时替换；当前已知资源正常情况下不会触发替换。
    return raw.decode("cp950", errors="replace")


def decrypt_enc(data: bytes) -> bytes:
    """重放游戏旧式 ENC XOR：第 i 字节与 (i % 255) 异或。"""
    # 使用 bytearray 是因为它可以逐字节写入，比反复拼 bytes 更直观也更高效。
    out = bytearray(len(data))
    for index, value in enumerate(data):
        # 游戏算法的 key 每255字节从0重新开始。
        out[index] = value ^ (index % 255)
    return bytes(out)


class PeImage:
    """最小只读 PE32 映射器：只实现本专题需要的 VA -> 文件偏移和定长读取。"""

    def __init__(self, data: bytes) -> None:
        # 保存完整 EXE 字节；后面所有断言都只读取这个内存对象，不会修改磁盘文件。
        self.data = data
        # Windows PE 首先必须有 DOS MZ 头。
        if data[:2] != b"MZ":
            raise ValueError("输入 RPG.exe 不是 MZ/PE 文件")
        # DOS 头 +0x3C 指向真正的 PE 头。
        pe_offset = u32(data, 0x3C)
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("找不到 PE\\0\\0 签名")
        # PE Signature 后是20字节 COFF header。
        coff = pe_offset + 4
        section_count = u16(data, coff + 2)
        optional_size = u16(data, coff + 16)
        optional = coff + 20
        # 当前游戏是32位 PE，OptionalHeader.Magic 必须为0x10B。
        if u16(data, optional) != 0x10B:
            raise ValueError("本工具只支持当前32位 PE（Magic=0x10B）")
        # PE32 OptionalHeader +0x1C 是 ImageBase；当前通常为0x00400000，但不写死更稳妥。
        self.image_base = u32(data, optional + 0x1C)
        # section table 紧跟 OptionalHeader，每个 section header 固定40字节。
        section_table = optional + optional_size
        self.sections: list[dict[str, int | str]] = []
        for index in range(section_count):
            base = section_table + index * 40
            name = data[base:base + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            self.sections.append({
                "name": name,
                "virtual_size": u32(data, base + 8),
                "virtual_address": u32(data, base + 12),
                "raw_size": u32(data, base + 16),
                "raw_offset": u32(data, base + 20),
            })

    def va_to_offset(self, va: int) -> int:
        """把运行时绝对虚拟地址 VA 换算成磁盘文件偏移。"""
        # RVA 就是 VA 减 ImageBase。
        rva = va - self.image_base
        for section in self.sections:
            start = int(section["virtual_address"])
            # 内存 virtual_size 与磁盘 raw_size 可能不同；判断所属节时取较大者。
            span = max(int(section["virtual_size"]), int(section["raw_size"]))
            if start <= rva < start + span:
                delta = rva - start
                # 如果落到内存零填充区，磁盘没有真实字节，不能做机器码断言。
                if delta >= int(section["raw_size"]):
                    raise ValueError(f"VA 0x{va:08X} 落在 section 零填充区")
                return int(section["raw_offset"]) + delta
        raise ValueError(f"VA 0x{va:08X} 不属于任何 PE section")

    def read(self, va: int, size: int) -> bytes:
        """按 VA 读取固定数量字节；长度不足时直接报错。"""
        offset = self.va_to_offset(va)
        chunk = self.data[offset:offset + size]
        if len(chunk) != size:
            raise ValueError(f"读取 VA 0x{va:08X} 时超出文件")
        return chunk


# 统一的机器码断言函数。这样报告中每条机器证据都用完全相同的格式输出，便于自动审计。
def machine_check(pe: PeImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    expected = bytes.fromhex(expected_hex)
    actual = pe.read(va, len(expected))
    return {
        "VA": f"0x{va:08X}",
        "含义": meaning,
        "期望": expected.hex(" "),
        "实际": actual.hex(" "),
        "PASS": actual == expected,
    }


def build_machine_checks(pe: PeImage) -> list[dict[str, Any]]:
    """重放固化48所依赖的 UI、normal writer、解除与运行时行为机器证据。"""
    checks: list[dict[str, Any]] = []

    # 第一组：7个单分支 UI 路由。每一路都要同时证明：
    # A. 读取的 RoleDefinition 字段正好是 +0x1E8 + raw_id*4；
    # B. 随后压入的 selector 正好是固化46记录的值。
    for row in SINGLE_CHANNELS:
        raw_id = int(row["raw_id"])
        role_offset = ROLEDEF_EFFECT_BASE + raw_id * 4
        field_expected = b"\x8B\x86" + struct.pack("<I", role_offset)
        field_actual = pe.read(int(row["entry_va"]), len(field_expected))
        checks.append({
            "VA": f"0x{int(row['entry_va']):08X}",
            "含义": f"raw{raw_id}单分支UI读取 RoleDefinition+0x{role_offset:X}",
            "期望": field_expected.hex(" "),
            "实际": field_actual.hex(" "),
            "PASS": field_actual == field_expected,
        })
        push_expected = bytes((0x6A, int(row["selector"]) & 0xFF))
        push_actual = pe.read(int(row["push_va"]), 2)
        checks.append({
            "VA": f"0x{int(row['push_va']):08X}",
            "含义": f"raw{raw_id}单分支UI压入 ITF0052 selector {row['selector']}",
            "期望": push_expected.hex(" "),
            "实际": push_actual.hex(" "),
            "PASS": push_actual == push_expected,
        })

    # 第二组：normal effect 的“自身编号写入”协议。
    # 这里先读取 raw ID，然后 signed compare 0x1B；通过后检查对应 runtime 槽是否已有正值，
    # 最后把 value 写到 pending + raw_id*4。这个断言专门防止把固化47的 counterpart 表
    # 再次误解成“normal effect 自身重映射表”。
    checks.append(machine_check(
        pe, 0x00422C44,
        "8b 44 24 1c 03 c2 8b 04 86 83 f8 1b 7f 1c 8b 4c 24 24 8b 0c 8d 94 fd 89 00",
        "normal effect读取raw ID并要求signed raw ID<=27",
    ))
    checks.append(machine_check(
        pe, 0x00422C5D,
        "83 bc 81 34 0a 00 00 00 7f 07 89 bc 81 cc 00 00 00",
        "normal effect按同一raw ID检查runtime并写同编号pending槽",
    ))

    # 第三组：raw1“麻痹/软瘫”的运行时效果。
    # +0xA38>0 时，原版用 signed /10 的常见乘法魔数缩放 EDI；后面又把一个默认100的量改成10。
    checks.append(machine_check(
        pe, 0x00420553,
        "8b 86 38 0a 00 00 85 c0 7e 13 b8 67 66 66 66 f7 ef c1 fa 02 8b c2 c1 e8 1f 03 d0 8b fa",
        "raw1运行时：+0xA38>0时把派生EDI按有符号/10缩放",
    ))
    checks.append(machine_check(
        pe, 0x00420619,
        "8b 86 38 0a 00 00 ba 64 00 00 00 85 c0 7e 05 ba 0a 00 00 00",
        "raw1运行时：+0xA38>0时把另一派生量从100降到10",
    ))

    # 第四组：raw9。+0xA58>0 会把传给0x424090两个模式的 EDI 直接清0；
    # 另一路命中/结果处理在特定符号条件满足时把 +0xA58 自身清掉。
    checks.append(machine_check(
        pe, 0x004205E3,
        "8b 86 58 0a 00 00 85 c0 7e 04 33 ff eb 0d",
        "raw9运行时：+0xA58>0时把派生EDI直接清0",
    ))
    checks.append(machine_check(
        pe, 0x0042139E,
        "39 a8 58 0a 00 00 7e 0f 39 2b 7d 05 39 6b 04 7c 06 89 a8 58 0a 00 00",
        "raw9结果路径：特定结果条件满足时清除+0xA58",
    ))

    # raw2、raw11、raw13 在当前已证 0x420440 片段中主要被打包为布尔状态。
    # 这里保存三条直接读取，证明它们确实属于活动状态布尔集合，而不是只存在于资源文件。
    checks.append(machine_check(
        pe, 0x0042085C,
        "8b be 3c 0a 00 00 0f 9f c0",
        "raw2运行时：读取+0xA3C并转成>0布尔状态",
    ))
    checks.append(machine_check(
        pe, 0x00420883,
        "8b 96 60 0a 00 00 0f 9f c0",
        "raw11运行时：读取+0xA60并转成>0布尔状态",
    ))
    checks.append(machine_check(
        pe, 0x00420899,
        "8b 8e 68 0a 00 00 3b d3 0f 9f c2 3b cb 88 54 24 24 0f 9f c0",
        "raw13运行时：读取+0xA68并转成>0布尔状态",
    ))

    # 第五组：raw14“化幻/无形”。一处会在状态活动时把两个结果字段清0；
    # 另一处会阻止普通 pending effect commit 进入 runtime，体现其特殊隔离/无形门控。
    checks.append(machine_check(
        pe, 0x00421367,
        "39 a8 6c 0a 00 00 7e 05 89 2b 89 6b 04",
        "raw14结果路径：+0xA6C>0时把两个结果字段清0",
    ))
    checks.append(machine_check(
        pe, 0x00421620,
        "8b b9 6c 0a 00 00 85 ff 7f 15 89 94 01 68 09 00 00",
        "raw14提交门控：+0xA6C>0时跳过normal pending→runtime commit",
    ))

    # 第六组：三个专用解除分支。写入值来自前文已经清零的 EDX，因此这三条就是直接清 runtime 槽。
    checks.append(machine_check(pe, 0x00421744, "89 97 38 0a 00 00", "通脉丸分支清raw1 runtime +0xA38"))
    checks.append(machine_check(pe, 0x0042177D, "89 97 3c 0a 00 00", "绮罗天香分支清raw2 runtime +0xA3C"))
    checks.append(machine_check(pe, 0x004217B6, "89 97 60 0a 00 00", "血仙邪酿分支清raw11 runtime +0xA60"))

    # Ail2 item-ID selector 表位于 0x421990。直接读取三项，可以证明 0x11A/0x11C/0x11D
    # 分别确实选择上面三个清零分支，而不是仅仅“文字看起来像能解除”。
    item_selector_table = pe.read(0x00421990, 0x65)
    expected_cure_selectors = {0x11A: 2, 0x11C: 3, 0x11D: 4}
    for item_id, expected_selector in expected_cure_selectors.items():
        actual_selector = item_selector_table[item_id - 0x108]
        checks.append({
            "VA": f"0x{0x00421990 + item_id - 0x108:08X}",
            "含义": f"Ail2 item 0x{item_id:X} 的状态解除selector",
            "期望": f"{expected_selector:02x}",
            "实际": f"{actual_selector:02x}",
            "PASS": actual_selector == expected_selector,
        })

    # 第七组：raw16“幽冥返召/操偶”的特殊路径。
    # 0x422016 直接把 +0xA74 写成1,000,000；0x443590的结束判定循环又从
    # FightRole+0x850 起步，并读取 +0x224，也就是同一 FightRole+0xA74，非0时跳过人数统计。
    checks.append(machine_check(
        pe, 0x00422016,
        "c7 82 74 0a 00 00 40 42 0f 00",
        "raw16特殊施加：FightRole+0xA74写1,000,000",
    ))
    checks.append(machine_check(
        pe, 0x0044359C,
        "8d 87 34 0c 00 00 ba 10 00 00 00 83 b8 dc fe ff ff ff 74 19 8b 88 24 02 00 00 85 c9 75 0f",
        "Battle结束判定：以FightRole+0x850为游标并检查同角色+0xA74，非0跳过人数统计",
    ))

    return checks


def parse_firttech(decoded: bytes) -> list[dict[str, Any]]:
    """把500条 Firttech 解成仅本专题需要的字段。"""
    expected_size = FIRTTECH_RECORD_SIZE * FIRTTECH_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(f"Firttech解密大小错误：{len(decoded)} != {expected_size}")
    records: list[dict[str, Any]] = []
    for index in range(FIRTTECH_RECORD_COUNT):
        # 每次先切出一条固定大小记录，后面的字段偏移就都相对 record 起点计算。
        start = index * FIRTTECH_RECORD_SIZE
        record = decoded[start:start + FIRTTECH_RECORD_SIZE]
        records.append({
            "index": index,
            "record_id": u32(record, 0x00),
            "name": decode_cp950_cstring(record[0x04:0x24]),
            "values": [s32(record, FIRTTECH_VALUE_BASE + slot * 4) for slot in range(5)],
            "raw_ids": [s32(record, FIRTTECH_ID_BASE + slot * 4) for slot in range(5)],
            "description": decode_cp950_cstring(record[0x17C:0x230]),
        })
    return records


def parse_ail2(decoded: bytes) -> list[dict[str, Any]]:
    """把700条 Ail2 解成仅本专题需要的字段。"""
    expected_size = AIL2_RECORD_SIZE * AIL2_RECORD_COUNT
    if len(decoded) != expected_size:
        raise ValueError(f"Ail2解密大小错误：{len(decoded)} != {expected_size}")
    records: list[dict[str, Any]] = []
    for index in range(AIL2_RECORD_COUNT):
        start = index * AIL2_RECORD_SIZE
        record = decoded[start:start + AIL2_RECORD_SIZE]
        records.append({
            "index": index,
            "record_id": u32(record, 0x20),
            "name": decode_cp950_cstring(record[0x00:0x20]),
            "values": [s32(record, AIL2_VALUE_BASE + slot * 4) for slot in range(5)],
            "raw_ids": [s32(record, AIL2_ID_BASE + slot * 4) for slot in range(5)],
            "description": decode_cp950_cstring(record[0x188:0x388]),
        })
    return records


def verify_author_anchor(anchor: dict[str, Any], firttech: list[dict[str, Any]], ail2: list[dict[str, Any]]) -> dict[str, Any]:
    """验证一条“施加类”作者文本锚点：名称、关键短语和positive raw effect必须同时匹配。"""
    table_name = str(anchor["table"])
    records = firttech if table_name == "Firttech" else ail2
    record = records[int(anchor["index"])]
    raw_id = int(anchor["raw_id"])
    # raw_ids 与 values 是五个并行槽；只有同一槽 raw_id相等 且 value>0 才算真实施加样本。
    has_positive_effect = any(
        effect_id == raw_id and value > 0
        for effect_id, value in zip(record["raw_ids"], record["values"])
    )
    return {
        "raw_id": raw_id,
        "资源表": table_name,
        "记录索引": record["index"],
        "记录ID": record["record_id"],
        "名称": record["name"],
        "说明": record["description"],
        "五槽raw_id": record["raw_ids"],
        "五槽value": record["values"],
        "关键短语": anchor["phrase"],
        "用途": anchor["kind"],
        "名称匹配": record["name"] == anchor["name"],
        "关键短语匹配": str(anchor["phrase"]) in record["description"],
        "positive raw effect匹配": has_positive_effect,
        "PASS": (
            record["name"] == anchor["name"]
            and str(anchor["phrase"]) in record["description"]
            and has_positive_effect
        ),
    }


def verify_cure_anchor(anchor: dict[str, Any], ail2: list[dict[str, Any]], pe: PeImage) -> dict[str, Any]:
    """验证专用解除物品：作者说明、item ID、selector表与目标runtime清零必须四者一致。"""
    record = ail2[int(anchor["ail2_index"])]
    item_id = int(anchor["item_id"])
    # 0x421990 的表从 item ID 0x108 开始，因此 item_id-0x108 就是字节索引。
    actual_selector = pe.read(0x00421990 + item_id - 0x108, 1)[0]
    expected_clear = b"\x89\x97" + struct.pack("<I", int(anchor["runtime_offset"]))
    actual_clear = pe.read(int(anchor["clear_va"]), len(expected_clear))
    return {
        "raw_id": anchor["raw_id"],
        "Ail2记录索引": record["index"],
        "物品ID": f"0x{record['record_id']:X}",
        "名称": record["name"],
        "说明": record["description"],
        "关键短语": anchor["phrase"],
        "期望selector": anchor["selector"],
        "实际selector": actual_selector,
        "目标runtime偏移": f"+0x{int(anchor['runtime_offset']):X}",
        "清零机器VA": f"0x{int(anchor['clear_va']):08X}",
        "记录ID匹配": record["record_id"] == item_id,
        "名称匹配": record["name"] == anchor["name"],
        "关键短语匹配": str(anchor["phrase"]) in record["description"],
        "selector匹配": actual_selector == int(anchor["selector"]),
        "清零机器码匹配": actual_clear == expected_clear,
        "PASS": (
            record["record_id"] == item_id
            and record["name"] == anchor["name"]
            and str(anchor["phrase"]) in record["description"]
            and actual_selector == int(anchor["selector"])
            and actual_clear == expected_clear
        ),
    }


def build_status_rows() -> list[dict[str, Any]]:
    """生成兼容引擎可以直接参考的七状态实现表；标签严格区分“已命名”与“保守类名”。"""
    by_raw = {int(row["raw_id"]): row for row in SINGLE_CHANNELS}
    rows: list[dict[str, Any]] = []
    for raw_id in sorted(by_raw):
        route = by_raw[raw_id]
        # role/runtime offset 都按同一个 raw_id 计算，能直接看出 UI 字段与 runtime slot 的同源关系。
        role_offset = ROLEDEF_EFFECT_BASE + raw_id * 4
        runtime_offset = RUNTIME_EFFECT_BASE + raw_id * 4
        pending_offset = PENDING_EFFECT_BASE + raw_id * 4

        # 为每个状态写一条“为什么可以/不可以采用正式业务名”的说明。
        if raw_id == 1:
            evidence_level = "A级：作者施加文本+专用解除文本/机器码+运行时强减速+UI路由"
            implementation = "活动时影响战斗派生量（其中一项按/10缩放，另一项100→10）；专用解除物品通脉丸清此槽。"
        elif raw_id == 2:
            evidence_level = "A级：作者施加文本+专用解除文本/机器码+UI路由"
            implementation = "作者文本直接说明不能施咒；绮罗天香的作者文本写破除封咒，机器码专门清此槽。"
        elif raw_id == 9:
            evidence_level = "A级结构/行为，业务正式名未见：冰系作者文本+运行时EDI清0/条件自清+UI路由"
            implementation = "活动时把一个送入0x424090的派生EDI清0；特定结果条件会清除本槽。只命名为冰封/凛冻类。"
        elif raw_id == 11:
            evidence_level = "A级：作者施加文本+专用解除文本/机器码+UI路由"
            implementation = "作者文本直接使用‘逆阙’；血仙邪酿说明‘解救逆阙’，机器码专门清此槽。"
        elif raw_id == 13:
            evidence_level = "B+：唯一当前positive资源锚点+运行时布尔状态+UI路由；缺独立行为consumer与正式状态名"
            implementation = "当前Ail2唯一positive样本血灵神珠说明‘续命护心之能’；运行时至少被打包为>0状态布尔。"
        elif raw_id == 14:
            evidence_level = "A级：作者施加文本+结果归零门控+pending提交阻断+UI路由"
            implementation = "活动时可把两个结果字段清0，并阻断normal pending→runtime提交；作者文本为化幻、无形、刀剑咒法皆不能及。"
        else:  # raw16
            evidence_level = "A级：作者文本+特殊1,000,000写入+胜负人数排除+UI路由"
            implementation = "幽冥返召特殊分支把本槽写1,000,000；Battle结束判定排除此槽非0的操偶单位。"

        rows.append({
            "raw_id": raw_id,
            "语义标签": SEMANTIC_LABELS[raw_id],
            "RoleDefinition字段": f"+0x{role_offset:X}",
            "FightRole runtime槽": f"+0x{runtime_offset:X}",
            "FightRole pending槽": f"+0x{pending_offset:X}",
            "Battle UI逻辑槽": route["channel"],
            "ITF0052 selector": route["selector"],
            "证据等级": evidence_level,
            "兼容实现摘要": implementation,
        })
    return rows


def write_csv(path: Path, rows: list[dict[str, Any]]) -> None:
    """以 UTF-8-SIG 写CSV，方便Windows记事本/Excel直接显示中文。"""
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "raw_id", "语义标签", "RoleDefinition字段", "FightRole runtime槽", "FightRole pending槽",
        "Battle UI逻辑槽", "ITF0052 selector", "证据等级", "兼容实现摘要",
    ]
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def main() -> int:
    """命令行入口：读取输入、重放断言、输出JSON/CSV，并用退出码表示总体是否PASS。"""
    parser = argparse.ArgumentParser(description="调查Battle七个单分支状态表现槽的资源语义与机器行为")
    parser.add_argument("--rpg", required=True, type=Path, help="RPG.exe 或 RPG.exe.org 解压后的文件")
    parser.add_argument("--firttech", required=True, type=Path, help="Public/Firttech.enc 路径")
    parser.add_argument("--ail2", required=True, type=Path, help="Public/Ail2.ENC 路径")
    parser.add_argument("--json", required=True, type=Path, help="输出完整JSON报告")
    parser.add_argument("--csv", required=True, type=Path, help="输出七状态CSV摘要")
    args = parser.parse_args()

    # 一次性读取三个输入。整个工具之后都只对内存副本做计算。
    rpg_data = args.rpg.read_bytes()
    firttech_encrypted = args.firttech.read_bytes()
    ail2_encrypted = args.ail2.read_bytes()

    # 先解密资源，再按固定记录尺寸解析。任何尺寸不匹配都会抛异常并立即中止，避免带错资源继续得出假结论。
    firttech_decoded = decrypt_enc(firttech_encrypted)
    ail2_decoded = decrypt_enc(ail2_encrypted)
    firttech_records = parse_firttech(firttech_decoded)
    ail2_records = parse_ail2(ail2_decoded)

    # PE映射器只负责把VA定位到文件字节；所有机器码证据在build_machine_checks里集中重放。
    pe = PeImage(rpg_data)
    machine_checks = build_machine_checks(pe)

    # 逐条验证作者“施加”文本锚点。
    author_checks = [
        verify_author_anchor(anchor, firttech_records, ail2_records)
        for anchor in AUTHOR_ANCHORS
    ]
    # 三条专用解除还要额外连上item selector表与清零机器码。
    cure_checks = [
        verify_cure_anchor(anchor, ail2_records, pe)
        for anchor in CURE_ANCHORS
    ]

    # status_rows 是最终给兼容引擎看的结构摘要；它不依赖图标主观识别。
    status_rows = build_status_rows()

    # 单独计算几个PASS维度，报告里保留它们，出错时接档者能立刻知道是机器码、资源文本还是解除链失败。
    machine_pass = all(bool(item["PASS"]) for item in machine_checks)
    author_pass = all(bool(item["PASS"]) for item in author_checks)
    cure_pass = all(bool(item["PASS"]) for item in cure_checks)
    sha_pass = sha256_bytes(rpg_data) == CURRENT_RPG_ORG_SHA256

    report = {
        "工具版本": TOOL_VERSION,
        "输入": {
            "RPG.exe": {"路径": str(args.rpg), "大小": len(rpg_data), "sha256": sha256_bytes(rpg_data), "当前org整文件SHA匹配": sha_pass},
            "Firttech.enc": {"路径": str(args.firttech), "大小": len(firttech_encrypted), "sha256": sha256_bytes(firttech_encrypted)},
            "Ail2.ENC": {"路径": str(args.ail2), "大小": len(ail2_encrypted), "sha256": sha256_bytes(ail2_encrypted)},
        },
        "结构规则": {
            "normal_effect自身槽": "raw ID 0..27 -> pending/runtime同编号槽；0x46A668只负责counterpart抵消，不能拿来重映射自身槽",
            "RoleDefinition状态块": "raw N -> +0x1E8 + N*4",
            "FightRole_runtime状态块": "raw N -> +0xA34 + N*4",
            "FightRole_pending状态块": "raw N -> +0xCC + N*4",
            "本节点调查对象": [1, 2, 9, 11, 13, 14, 16],
        },
        "七个单分支状态实现表": status_rows,
        "机器码断言": machine_checks,
        "作者施加文本锚点": author_checks,
        "专用解除链锚点": cure_checks,
        "证据边界": [
            "raw1可采用麻痹/软瘫：施加文本、解除文本、解除机器码与运行时减速方向一致。",
            "raw2可采用封咒/不能施咒：封魔神道/封魔神水作者文本与绮罗天香‘破除封咒’专用清槽一致。",
            "raw11可采用逆阙：血魂之咒/鬼躯化膏作者文本与血仙邪酿‘解救逆阙’专用清槽一致。",
            "raw14可采用化幻/无形：无方变幻/时幻沙作者文本与结果归零、pending提交阻断一致。",
            "raw16可采用幽冥返召/操偶：作者文本、特殊1,000,000写入和胜负人数排除三者一致。",
            "raw9只冻结为冰封/凛冻类：资源文本和机器行为都指向冰系控制/屏障状态，但当前没有作者正式状态栏名。",
            "raw13只冻结为续命护心类：当前唯一positive样本血灵神珠直接写‘续命护心之能’，但没有独立专用consumer或作者正式状态栏名。",
            "任何状态名均没有使用ITF0052图标外观作为唯一证据。",
        ],
        "PASS汇总": {
            "RPG_org_SHA身份匹配": sha_pass,
            "机器码断言": machine_pass,
            "作者施加文本锚点": author_pass,
            "专用解除链锚点": cure_pass,
        },
        "总体PASS": sha_pass and machine_pass and author_pass and cure_pass,
    }

    # 输出目录可能尚不存在，所以先创建父目录。
    args.json.parent.mkdir(parents=True, exist_ok=True)
    # ensure_ascii=False 保留中文，indent=2 让人工审查也容易阅读。
    args.json.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    write_csv(args.csv, status_rows)

    # 控制台只输出最关键的总览，详细内容以JSON/CSV为准。
    print(f"[固化48] 机器码断言：{sum(bool(x['PASS']) for x in machine_checks)}/{len(machine_checks)}")
    print(f"[固化48] 作者施加文本锚点：{sum(bool(x['PASS']) for x in author_checks)}/{len(author_checks)}")
    print(f"[固化48] 专用解除链锚点：{sum(bool(x['PASS']) for x in cure_checks)}/{len(cure_checks)}")
    print(f"[固化48] 总体PASS：{report['总体PASS']}")

    # 回归脚本以0表示成功、1表示失败。这样CI/批处理无需解析中文输出也能判断结果。
    return 0 if report["总体PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
