#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 五项核心能力作者名称与公式 A/B 派生量调查器（固化53）。

这个工具只做“读取与核对”，不会启动游戏，也不会修改 RPG.exe / Ail2.ENC。
它要解决的是前代 D5 公式里最后一个长期保留的命名问题：

1. RoleDefinition+0x3C / +0x40 / +0x44 / +0x48 / +0x4C 到底叫什么；
2. 这些名字是不是作者自己写进游戏的，而不是我们看公式后猜的；
3. 公式临时量 A / B 分别从哪一个作者属性派生；
4. 永久能力物品能否提供第二条独立资源文字证据；
5. 哪些名字可以正式用于兼容引擎，哪些仍只能使用“派生量”这种中性名称。

固化53采用四路交叉证据：

A. RPG.exe 的升级/成长 UI 硬编码 CP950 格式串：
   「膂力 %d > %d」「體魄 %d > %d」「靈力 %d > %d」「迅捷 %d > %d」「機運 %d > %d」。

B. 0x443B60 的成长写回：
   五项 stat index 0..4 依次修改 RoleDefinition+0x3C/+0x40/+0x44/+0x48/+0x4C，
   并把每项修改前/后的数值写进 0x70-byte UI result record。

C. 0x413170 一带的 UI 显示：
   从同一个 result record 取出对应 old/new，再使用上述五个作者格式串显示。
   这一步把“字段偏移”和“作者名称”直接接在了一起。

D. Ail2.ENC 的永久能力物品：
   熊王金膽、鬼神天釀、靈泉玉露、迅神五書、玄機道卷分别通过已证 writer
   增加 +0x3C/+0x40/+0x44/+0x48/+0x4C，物品说明又给出膂力、健体、灵力/灵智、迅捷、机运等作者文字。

最后，本工具还直接检查 D5 的 A/B builder：

- A = 0x42BA80(performer)：基础从 RoleDefinition+0x3C（膂力）开始，
  再叠加三件装备各自 Ail2+0x38，并受 raw3「疲弱」与 raw22「神力」倍率影响；
- B = 0x42BB50(target)：基础从 trunc(RoleDefinition+0x40（體魄）/2) 开始，
  再叠加三件装备各自 Ail2+0x3C，并受 raw15/raw25/raw6 等已证状态倍率影响。

【非常重要的证据边界】

作者明确给出了五个基础能力的名字，但没有在当前 EXE 中发现把内部临时量 0x42BA80 / 0x42BB50
直接命名成“攻击力 / 防御力”的作者字符串。因此固化53只把它们称为：

- “膂力派生攻击侧量 A”
- “體魄派生防御侧量 B”

这里的“攻击侧 / 防御侧”来自它们在伤害/公式 helper 中的加减角色；
它不是声称原版源码变量名就叫 Attack / Defense。
"""

# argparse：处理命令行参数。这样接档者可以把工具用于另一份 EXE / Ail2，而不需要改源码。
import argparse
# csv：把五项能力映射输出成简单表格，便于人工检查和导入兼容引擎资料表。
import csv
# hashlib：记录输入文件和解码后资源的 SHA-256，避免以后误把不同版本当成同一证据。
import hashlib
# json：输出完整结构化报告，供后续固化节点自动回归。
import json
# struct：按照 x86/PE 使用的小端格式读取 16/32 位整数。
import struct
# Path：统一处理 Windows/Linux 路径，不手工拼接斜杠。
from pathlib import Path
# 类型标注不是运行必需，但能让初学者更清楚“函数接收什么、返回什么”。
from typing import Any, Dict, List, Tuple


# 初学者逐步说明：把右侧结果保存到 `TOOL_VERSION`；这里保存的是 `'0.7D-solid53.0'`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
TOOL_VERSION = "0.7D-solid53.0"
# 初学者逐步说明：把右侧结果保存到 `EXPECTED_RPG_SHA256`；这里保存的是 `'8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f'`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
EXPECTED_RPG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
# 初学者逐步说明：把右侧结果保存到 `EXPECTED_AIL2_ENCODED_SHA256`；这里保存的是 `'a24784316c2bd23fc2683637ab860c0e54f1b4175f6db990398843bd0a13c5d8'`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
EXPECTED_AIL2_ENCODED_SHA256 = "a24784316c2bd23fc2683637ab860c0e54f1b4175f6db990398843bd0a13c5d8"
# 初学者逐步说明：把右侧结果保存到 `EXPECTED_AIL2_DECODED_SHA256`；这里保存的是 `'a85d0904e969700b996ee89134582552bfabb98af76321804af34f9896b051a8'`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
EXPECTED_AIL2_DECODED_SHA256 = "a85d0904e969700b996ee89134582552bfabb98af76321804af34f9896b051a8"

# Ail2 的格式参数已经由前代多次回归：700 条记录，每条 926 字节。
AIL2_RECORD_SIZE = 926
# 初学者逐步说明：把右侧结果保存到 `AIL2_RECORD_COUNT`；这里保存的是 `700`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_RECORD_COUNT = 700
# 初学者逐步说明：把右侧结果保存到 `AIL2_NAME_OFFSET`；这里保存的是 `0`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_NAME_OFFSET = 0x00
# 初学者逐步说明：把右侧结果保存到 `AIL2_NAME_SIZE`；这里保存的是 `32`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_NAME_SIZE = 32
# 初学者逐步说明：把右侧结果保存到 `AIL2_ITEM_ID_OFFSET`；这里保存的是 `32`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_ITEM_ID_OFFSET = 0x20
# 初学者逐步说明：把右侧结果保存到 `AIL2_INCREMENT_OFFSET`；这里保存的是 `208`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_INCREMENT_OFFSET = 0xD0
# 初学者逐步说明：把右侧结果保存到 `AIL2_VALUE_BASE`；这里保存的是 `232`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_VALUE_BASE = 0xE8
# 初学者逐步说明：把右侧结果保存到 `AIL2_ID_BASE`；这里保存的是 `252`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_ID_BASE = 0xFC
# 初学者逐步说明：把右侧结果保存到 `AIL2_DESCRIPTION_OFFSET`；这里保存的是 `392`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_DESCRIPTION_OFFSET = 0x188
# 初学者逐步说明：把右侧结果保存到 `AIL2_DESCRIPTION_SIZE`；这里保存的是 `512`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
AIL2_DESCRIPTION_SIZE = 512

# 五个永久能力物品的 item ID。它们恰好给五项作者能力各提供一条独立文字证据。
PERMANENT_ITEM_IDS = [0x12F, 0x132, 0x130, 0x131, 0x133]

# 这是固化53最终要冻结的五项映射。
# result_old/result_new 是 0x443B60 写入 0x70-byte result record 的偏移；
# UI 再从这个 result record 取 old/new，配合作者格式串显示。
STAT_DEFINITIONS: List[Dict[str, Any]] = [
    {
        "stat_index": 0,
        "role_offset": 0x3C,
        "author_name": "膂力",
        "label_va": 0x00469458,
        "label_xref_va": 0x004131AE,
        "result_old": 0x30,
        "result_new": 0x1C,
        "permanent_item_id": 0x12F,
        "permanent_effect_id": 54,
        "writer_va": 0x0043B023,
        "formula_relation": "A 的基础作者属性；A 还叠加装备 Ail2+0x38，并受疲弱/神力状态倍率修正",
    },
    {
        "stat_index": 1,
        "role_offset": 0x40,
        "author_name": "體魄",
        "label_va": 0x00469438,
        "label_xref_va": 0x00413231,
        "result_old": 0x34,
        "result_new": 0x20,
        "permanent_item_id": 0x132,
        "permanent_effect_id": 55,
        "writer_va": 0x0043B028,
        "formula_relation": "B 的基础作者属性；B 先取 trunc(體魄/2)，再叠加装备 Ail2+0x3C，并受状态倍率修正",
    },
    {
        "stat_index": 2,
        "role_offset": 0x44,
        "author_name": "靈力",
        "label_va": 0x00469448,
        "label_xref_va": 0x004131EF,
        "result_old": 0x38,
        "result_new": 0x24,
        "permanent_item_id": 0x130,
        "permanent_effect_id": 58,
        "writer_va": 0x0043B037,
        "formula_relation": "五项成长能力之一；不是 D5 的 A/B 基础字段",
    },
    {
        "stat_index": 3,
        "role_offset": 0x48,
        "author_name": "迅捷",
        "label_va": 0x00469428,
        "label_xref_va": 0x00413273,
        "result_old": 0x3C,
        "result_new": 0x28,
        "permanent_item_id": 0x131,
        "permanent_effect_id": 56,
        "writer_va": 0x0043B02D,
        "formula_relation": "五项成长能力之一；另有其他命中/行动相关 consumer，但本节点不扩大业务命名",
    },
    {
        "stat_index": 4,
        "role_offset": 0x4C,
        "author_name": "機運",
        "label_va": 0x00469418,
        "label_xref_va": 0x004132BE,
        "result_old": 0x40,
        "result_new": 0x2C,
        "permanent_item_id": 0x133,
        "permanent_effect_id": 57,
        "writer_va": 0x0043B032,
        "formula_relation": "五项成长能力之一；本节点只冻结作者名称，不把其他派生 helper 强命名",
    },
]


# -----------------------------
# 一、最基础的字节读取帮助函数
# -----------------------------
def sha256_bytes(data: bytes) -> str:
    """返回一段 bytes 的 SHA-256 十六进制字符串。"""
    # SHA-256 只用于确认“输入是不是同一份文件”，不会参与任何游戏公式。
    return hashlib.sha256(data).hexdigest()


def u16(data: bytes, offset: int) -> int:
    """读取 unsigned 16-bit little-endian。"""
    # “<H”里的 < 表示 little-endian，H 表示无符号 16 位整数。
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    """读取 unsigned 32-bit little-endian。"""
    # 初学者逐步说明：把 `struct.unpack_from('<I', data, offset)[0]` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    """读取 signed 32-bit little-endian。"""
    # 初学者逐步说明：把 `struct.unpack_from('<i', data, offset)[0]` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return struct.unpack_from("<i", data, offset)[0]


def decode_period255(raw: bytes) -> bytes:
    """
    解码 Public/*.ENC 使用的 period-255 XOR。

    第 i 个字节执行：decoded[i] = raw[i] XOR (i % 255)。
    XOR 是可逆的；本工具只在内存里生成 decoded，不覆盖原文件。
    """
    # 初学者逐步说明：把 `bytes((value ^ index % 255 for index, value in enumerate(raw)))` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return bytes(value ^ (index % 255) for index, value in enumerate(raw))


def cp950_cstr(field: bytes) -> str:
    """把固定宽度字段按 NUL 结束、CP950/Big5 解码为 Python 字符串。"""
    # 数据库字段后面通常有 0 填充，所以先截到第一个 0x00。
    trimmed = field.split(b"\0", 1)[0]
    # replace 只防止单个坏字节让整个工具退出；原始输入 SHA 仍会被报告保存。
    return trimmed.decode("cp950", errors="replace")


# -----------------------------
# 二、最小 PE32 映射器
# -----------------------------
class PeImage:
    """把逆向文档里的 VA（虚拟地址）映射成 EXE 文件中的 raw offset。"""

    def __init__(self, data: bytes) -> None:
        # 保存原始 EXE 字节，后面的所有机器断言都从这里读取。
        self.data = data
        # 初学者逐步说明：检查条件 `data[:2] != b'MZ'`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
        if data[:2] != b"MZ":
            # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
            raise ValueError("输入不是 MZ 可执行文件")

        # DOS header +0x3C 保存 PE header 的文件偏移。
        pe_offset = u32(data, 0x3C)
        # 初学者逐步说明：检查条件 `data[pe_offset:pe_offset + 4] != b'PE\x00\x00'`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
            raise ValueError("输入没有有效 PE\\0\\0 签名")

        # COFF header 给出 section 数量和 optional header 长度。
        section_count = u16(data, pe_offset + 6)
        # 初学者逐步说明：把右侧结果保存到 `optional_size`；这里保存的是 `u16(data, pe_offset + 20)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        optional_size = u16(data, pe_offset + 20)
        # 初学者逐步说明：把右侧结果保存到 `optional_offset`；这里保存的是 `pe_offset + 24`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        optional_offset = pe_offset + 24

        # PE32 optional header +0x1C 是 ImageBase；这版游戏通常为 0x00400000。
        self.image_base = u32(data, optional_offset + 0x1C)
        # 初学者逐步说明：把右侧结果保存到 `section_table`；这里保存的是 `optional_offset + optional_size`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        section_table = optional_offset + optional_size
        # 初学者逐步说明：把右侧结果保存到 `self.sections`；这里保存的是 `[]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        self.sections: List[Tuple[str, int, int, int, int]] = []

        # 每个 IMAGE_SECTION_HEADER 固定 40 字节。
        for index in range(section_count):
            # 初学者逐步说明：把右侧结果保存到 `row`；这里保存的是 `section_table + index * 40`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
            row = section_table + index * 40
            # 初学者逐步说明：把右侧结果保存到 `name`；这里保存的是 `data[row:row + 8].split(b'\x00', 1)[0].decode('ascii', errors='repl...`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
            name = data[row:row + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            # 初学者逐步说明：把右侧结果保存到 `virtual_size`；这里保存的是 `u32(data, row + 8)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
            virtual_size = u32(data, row + 8)
            # 初学者逐步说明：把右侧结果保存到 `virtual_address`；这里保存的是 `u32(data, row + 12)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
            virtual_address = u32(data, row + 12)
            # 初学者逐步说明：把右侧结果保存到 `raw_size`；这里保存的是 `u32(data, row + 16)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
            raw_size = u32(data, row + 16)
            # 初学者逐步说明：把右侧结果保存到 `raw_pointer`；这里保存的是 `u32(data, row + 20)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
            raw_pointer = u32(data, row + 20)
            # 初学者逐步说明：调用 `self.sections.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
            self.sections.append((name, virtual_address, virtual_size, raw_pointer, raw_size))

    def va_to_offset(self, va: int) -> int:
        """把一个运行时 VA 转换成磁盘文件偏移。"""
        # 初学者逐步说明：把右侧结果保存到 `rva`；这里保存的是 `va - self.image_base`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        rva = va - self.image_base
        # 初学者逐步说明：依次遍历 `self.sections` 中的每一项，并把当前项放进 `(_name, section_rva, virtual_siz...`；循环体会对每一项执行同样的只读核对。
        for _name, section_rva, virtual_size, raw_pointer, raw_size in self.sections:
            # 用 max 是为了兼容 virtual size 与 raw size 其中一边稍大的常见 PE 情况。
            span = max(virtual_size, raw_size)
            # 初学者逐步说明：检查条件 `section_rva <= rva < section_rva + span`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
            if section_rva <= rva < section_rva + span:
                # 初学者逐步说明：把右侧结果保存到 `delta`；这里保存的是 `rva - section_rva`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
                delta = rva - section_rva
                # 初学者逐步说明：检查条件 `delta >= raw_size`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
                if delta >= raw_size:
                    # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
                    raise ValueError(f"VA 0x{va:08X} 落在 section 虚拟尾部，磁盘无对应字节")
                # 初学者逐步说明：把 `raw_pointer + delta` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
                return raw_pointer + delta
        # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
        raise ValueError(f"VA 0x{va:08X} 不属于任何已加载 section")

    def read(self, va: int, size: int) -> bytes:
        """读取指定 VA 开始的 size 个磁盘字节。"""
        # 初学者逐步说明：把右侧结果保存到 `offset`；这里保存的是 `self.va_to_offset(va)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        offset = self.va_to_offset(va)
        # 初学者逐步说明：把 `self.data[offset:offset + size]` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
        return self.data[offset:offset + size]

    def read_cstr(self, va: int, limit: int = 128) -> bytes:
        """从 VA 开始读取 0 结尾的原始字符串字节。"""
        # 初学者逐步说明：把右侧结果保存到 `offset`；这里保存的是 `self.va_to_offset(va)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        offset = self.va_to_offset(va)
        # 初学者逐步说明：把右侧结果保存到 `end`；这里保存的是 `self.data.find(b'\x00', offset, min(len(self.data), offset + limit))`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        end = self.data.find(b"\0", offset, min(len(self.data), offset + limit))
        # 初学者逐步说明：检查条件 `end < 0`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
        if end < 0:
            # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
            raise ValueError(f"VA 0x{va:08X} 在 {limit} 字节内没有 NUL 结尾")
        # 初学者逐步说明：把 `self.data[offset:end]` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
        return self.data[offset:end]

    def text_bytes(self) -> Tuple[int, bytes]:
        """返回 .text 的起始 VA 与 raw bytes，用于扫描 direct E8 caller。"""
        # 初学者逐步说明：依次遍历 `self.sections` 中的每一项，并把当前项放进 `(name, rva, _virtual_size, raw_p...`；循环体会对每一项执行同样的只读核对。
        for name, rva, _virtual_size, raw_pointer, raw_size in self.sections:
            # 初学者逐步说明：检查条件 `name == '.text'`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
            if name == ".text":
                # 初学者逐步说明：把 `(self.image_base + rva, self.data[raw_pointer:raw_pointer + raw_size])` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
                return self.image_base + rva, self.data[raw_pointer:raw_pointer + raw_size]
        # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
        raise ValueError("PE 中没有 .text section")


# -----------------------------
# 三、机器断言与 direct caller 扫描
# -----------------------------
def assert_bytes(pe: PeImage, va: int, expected_hex: str, label: str, checks: List[Dict[str, Any]]) -> None:
    """核对一段固定机器码，并把结果加入 checks。"""
    # 初学者逐步说明：把右侧结果保存到 `expected`；这里保存的是 `bytes.fromhex(expected_hex)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    expected = bytes.fromhex(expected_hex)
    # 初学者逐步说明：把右侧结果保存到 `actual`；这里保存的是 `pe.read(va, len(expected))`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    actual = pe.read(va, len(expected))
    # 初学者逐步说明：把右侧结果保存到 `passed`；这里保存的是 `actual == expected`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    passed = actual == expected
    # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    checks.append({
        "名称": label,
        "VA": f"0x{va:08X}",
        "期望": expected.hex(" "),
        "实际": actual.hex(" "),
        "通过": passed,
    })


def direct_e8_callers(pe: PeImage, target_va: int) -> List[int]:
    """扫描 .text 中 opcode E8 rel32，返回直接调用 target_va 的 call 指令地址。"""
    # 初学者逐步说明：把右侧结果保存到 `(text_va, text)`；这里保存的是 `pe.text_bytes()`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    text_va, text = pe.text_bytes()
    # 初学者逐步说明：把右侧结果保存到 `callers`；这里保存的是 `[]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    callers: List[int] = []
    # E8 后面固定跟 4-byte signed relative displacement，所以最后 4 字节不能作为新 call 起点。
    for index in range(0, len(text) - 4):
        # 初学者逐步说明：检查条件 `text[index] != 232`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
        if text[index] != 0xE8:
            continue
        # 初学者逐步说明：把右侧结果保存到 `relative`；这里保存的是 `struct.unpack_from('<i', text, index + 1)[0]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        relative = struct.unpack_from("<i", text, index + 1)[0]
        # 初学者逐步说明：把右侧结果保存到 `source_va`；这里保存的是 `text_va + index`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        source_va = text_va + index
        # 初学者逐步说明：把右侧结果保存到 `resolved`；这里保存的是 `source_va + 5 + relative`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        resolved = source_va + 5 + relative
        # 初学者逐步说明：检查条件 `resolved == target_va`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
        if resolved == target_va:
            # 初学者逐步说明：调用 `callers.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
            callers.append(source_va)
    # 初学者逐步说明：把 `callers` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return callers


# -----------------------------
# 四、Ail2 永久能力物品解析
# -----------------------------
def decode_ail2(encoded: bytes) -> bytes:
    """检查 Ail2 尺寸后解码。"""
    # 初学者逐步说明：把右侧结果保存到 `expected_size`；这里保存的是 `AIL2_RECORD_SIZE * AIL2_RECORD_COUNT`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    expected_size = AIL2_RECORD_SIZE * AIL2_RECORD_COUNT
    # 初学者逐步说明：检查条件 `len(encoded) != expected_size`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
    if len(encoded) != expected_size:
        # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
        raise ValueError(f"Ail2.ENC 大小异常：{len(encoded)}，期望 {expected_size}")
    # 初学者逐步说明：把 `decode_period255(encoded)` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return decode_period255(encoded)


def ail2_record(decoded: bytes, physical_index: int) -> bytes:
    """按物理记录索引切出一条 926-byte Ail2 记录。"""
    # 初学者逐步说明：把右侧结果保存到 `start`；这里保存的是 `physical_index * AIL2_RECORD_SIZE`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    start = physical_index * AIL2_RECORD_SIZE
    # 初学者逐步说明：把 `decoded[start:start + AIL2_RECORD_SIZE]` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return decoded[start:start + AIL2_RECORD_SIZE]


def find_item_record(decoded: bytes, item_id: int) -> Tuple[int, bytes]:
    """
    用 Ail2+0x20 的真实 item ID 找记录，而不是偷懒认为“物理索引永远等于 item ID”。

    前代已经发现 9 条记录存在物理索引置换；这个写法即使未来目标恰好落在置换区也不会取错。
    """
    # 初学者逐步说明：把右侧结果保存到 `matches`；这里保存的是 `[]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    matches: List[Tuple[int, bytes]] = []
    # 初学者逐步说明：依次遍历 `range(AIL2_RECORD_COUNT)` 中的每一项，并把当前项放进 `physical_index`；循环体会对每一项执行同样的只读核对。
    for physical_index in range(AIL2_RECORD_COUNT):
        # 初学者逐步说明：把右侧结果保存到 `record`；这里保存的是 `ail2_record(decoded, physical_index)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        record = ail2_record(decoded, physical_index)
        # 初学者逐步说明：把右侧结果保存到 `record_item_id`；这里保存的是 `u32(record, AIL2_ITEM_ID_OFFSET)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        record_item_id = u32(record, AIL2_ITEM_ID_OFFSET)
        # 初学者逐步说明：检查条件 `record_item_id == item_id`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
        if record_item_id == item_id:
            # 初学者逐步说明：调用 `matches.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
            matches.append((physical_index, record))
    # 初学者逐步说明：检查条件 `len(matches) != 1`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
    if len(matches) != 1:
        # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
        raise ValueError(f"item 0x{item_id:X} 匹配 {len(matches)} 条记录，期望恰好1条")
    # 初学者逐步说明：把 `matches[0]` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return matches[0]


def parse_permanent_item(decoded: bytes, item_id: int) -> Dict[str, Any]:
    """读取一条永久能力物品的作者名称、说明、五槽 effect ID/value 和实际增量。"""
    # 初学者逐步说明：把右侧结果保存到 `(physical_index, record)`；这里保存的是 `find_item_record(decoded, item_id)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    physical_index, record = find_item_record(decoded, item_id)
    # 初学者逐步说明：把右侧结果保存到 `ids`；这里保存的是 `[s32(record, AIL2_ID_BASE + slot * 4) for slot in range(5)]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    ids = [s32(record, AIL2_ID_BASE + slot * 4) for slot in range(5)]
    # 初学者逐步说明：把右侧结果保存到 `values`；这里保存的是 `[s32(record, AIL2_VALUE_BASE + slot * 4) for slot in range(5)]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    values = [s32(record, AIL2_VALUE_BASE + slot * 4) for slot in range(5)]
    # 初学者逐步说明：把 `{'物品ID': item_id, '物品ID十六进制': f'0x{item_id:X}', '物理记录索引': physical_index, '名称': cp...` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return {
        "物品ID": item_id,
        "物品ID十六进制": f"0x{item_id:X}",
        "物理记录索引": physical_index,
        "名称": cp950_cstr(record[AIL2_NAME_OFFSET:AIL2_NAME_OFFSET + AIL2_NAME_SIZE]),
        "说明": cp950_cstr(record[AIL2_DESCRIPTION_OFFSET:AIL2_DESCRIPTION_OFFSET + AIL2_DESCRIPTION_SIZE]),
        "五槽ID": ids,
        "五槽值": values,
        "永久增量_Ail2+0xD0": s32(record, AIL2_INCREMENT_OFFSET),
    }


# -----------------------------
# 五、五项作者名称与 A/B 证据重建
# -----------------------------
def build_stat_rows(pe: PeImage, decoded_ail2: bytes, checks: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    """把机器 UI 标签、成长写回、永久物品三类证据合并成五项能力表。"""
    # 初学者逐步说明：把右侧结果保存到 `items`；这里保存的是 `{item_id: parse_permanent_item(decoded_ail2, item_id) for item_id i...`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    items = {item_id: parse_permanent_item(decoded_ail2, item_id) for item_id in PERMANENT_ITEM_IDS}
    # 初学者逐步说明：把右侧结果保存到 `rows`；这里保存的是 `[]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    rows: List[Dict[str, Any]] = []

    # 0x443B60 写入五项 old/new 的关键指令地址。
    growth_instruction_map = {
        0x3C: [(0x443C5A, "8b 56 3c"), (0x443C5F, "89 57 30"), (0x443C69, "89 4e 3c"), (0x443C6C, "89 47 1c")],
        0x40: [(0x443C79, "8b 4e 40"), (0x443C7C, "89 4f 34"), (0x443C84, "89 4e 40"), (0x443C8B, "89 47 20")],
        0x44: [(0x443C98, "8b 4e 44"), (0x443C9D, "89 4f 38"), (0x443CA7, "89 4e 44"), (0x443CAA, "89 47 24")],
        0x48: [(0x443CB7, "8b 4e 48"), (0x443CBC, "89 4f 3c"), (0x443CC8, "89 56 48"), (0x443CCB, "89 47 28")],
        0x4C: [(0x443CD6, "8b 4e 4c"), (0x443CD9, "89 4f 40"), (0x443CE3, "89 5e 4c"), (0x443CE6, "89 47 2c")],
    }

    # 初学者逐步说明：依次遍历 `STAT_DEFINITIONS` 中的每一项，并把当前项放进 `definition`；循环体会对每一项执行同样的只读核对。
    for definition in STAT_DEFINITIONS:
        # 初学者逐步说明：把右侧结果保存到 `role_offset`；这里保存的是 `definition['role_offset']`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        role_offset = definition["role_offset"]
        # 初学者逐步说明：把右侧结果保存到 `author_name`；这里保存的是 `definition['author_name']`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        author_name = definition["author_name"]

        # 先直接读取作者格式串。这里不是“字符串附近看起来像”，而是固定 VA 的 CP950 原文。
        label_raw = pe.read_cstr(definition["label_va"])
        # 初学者逐步说明：把右侧结果保存到 `label_text`；这里保存的是 `label_raw.decode('cp950', errors='replace')`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        label_text = label_raw.decode("cp950", errors="replace")
        # 初学者逐步说明：把右侧结果保存到 `expected_label`；这里保存的是 `f'{author_name} %d > %d'`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        expected_label = f"{author_name} %d > %d"
        # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        checks.append({
            "名称": f"作者UI格式串_{author_name}",
            "VA": f"0x{definition['label_va']:08X}",
            "期望": expected_label,
            "实际": label_text,
            "通过": label_text == expected_label,
        })

        # UI xref 必须是 push imm32 label_va。68 是 x86 push immediate 的 opcode。
        push_expected = b"\x68" + struct.pack("<I", definition["label_va"])
        # 初学者逐步说明：把右侧结果保存到 `push_actual`；这里保存的是 `pe.read(definition['label_xref_va'], 5)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        push_actual = pe.read(definition["label_xref_va"], 5)
        # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        checks.append({
            "名称": f"UI格式串直接引用_{author_name}",
            "VA": f"0x{definition['label_xref_va']:08X}",
            "期望": push_expected.hex(" "),
            "实际": push_actual.hex(" "),
            "通过": push_actual == push_expected,
        })

        # 再核对成长状态机真正读/写这个 RoleDefinition 字段，并保存 old/new 到 result record。
        for va, expected_hex in growth_instruction_map[role_offset]:
            # 初学者逐步说明：调用 `assert_bytes` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
            assert_bytes(pe, va, expected_hex, f"成长写回_{author_name}_{va:08X}", checks)

        # 最后核对永久物品数据库中的作者文字和 writer effect ID。
        item = items[definition["permanent_item_id"]]
        # 初学者逐步说明：把右侧结果保存到 `nonzero_effect_ids`；这里保存的是 `[effect_id for effect_id, value in zip(item['五槽ID'], item['五槽值']) i...`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        nonzero_effect_ids = [effect_id for effect_id, value in zip(item["五槽ID"], item["五槽值"]) if effect_id != 0 or value != 0]
        # 这5件物品都应由第一个大 effect ID 驱动永久能力 writer；这里只检查定义中的 effect ID 确实存在。
        effect_ok = definition["permanent_effect_id"] in item["五槽ID"]
        # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        checks.append({
            "名称": f"永久物品effectID_{author_name}",
            "物品": item["名称"],
            "期望effectID": definition["permanent_effect_id"],
            "实际五槽ID": item["五槽ID"],
            "通过": effect_ok,
        })

        # writer 指令固定为 add [ecx+offset], eax，所以机器码可以直接核偏移。
        writer_expected = bytes([0x01, 0x41, role_offset])
        # 初学者逐步说明：把右侧结果保存到 `writer_actual`；这里保存的是 `pe.read(definition['writer_va'], 3)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        writer_actual = pe.read(definition["writer_va"], 3)
        # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        checks.append({
            "名称": f"永久能力writer_{author_name}",
            "VA": f"0x{definition['writer_va']:08X}",
            "期望": writer_expected.hex(" "),
            "实际": writer_actual.hex(" "),
            "通过": writer_actual == writer_expected,
        })

        # 初学者逐步说明：调用 `rows.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        rows.append({
            "stat_index": definition["stat_index"],
            "RoleDefinition偏移": f"0x{role_offset:02X}",
            "作者正式名称": author_name,
            "作者UI格式串": label_text,
            "UI格式串VA": f"0x{definition['label_va']:08X}",
            "UI引用VA": f"0x{definition['label_xref_va']:08X}",
            "成长result旧值偏移": f"0x{definition['result_old']:02X}",
            "成长result新值偏移": f"0x{definition['result_new']:02X}",
            "永久能力物品ID": item["物品ID十六进制"],
            "永久能力物品名称": item["名称"],
            "永久能力物品说明": item["说明"],
            "永久能力effectID": definition["permanent_effect_id"],
            "永久增量": item["永久增量_Ail2+0xD0"],
            "公式关系": definition["formula_relation"],
            "证据等级": "作者UI硬编码 + 成长写回机器码 + 永久物品writer/作者文本",
        })

    # 初学者逐步说明：把 `rows` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return rows


def add_ab_machine_checks(pe: PeImage, checks: List[Dict[str, Any]]) -> Dict[str, Any]:
    """核对 A/B builder 的字段来源、装备修正字段和状态倍率字段。"""
    # A base：先读取 RoleDefinition+0x3C；随后三件装备都从 Ail2 record +0x38 加到 eax。
    assert_bytes(pe, 0x42BA0C, "8b 8a 90 00 00 00 8b 42 3c", "A基线读取膂力+0x3C", checks)
    # 初学者逐步说明：依次遍历 `(4373034, 4373065, 4373096)` 中的每一项，并把当前项放进 `va`；循环体会对每一项执行同样的只读核对。
    for va in (0x42BA2A, 0x42BA49, 0x42BA68):
        # 三条 add 的 ModRM/SIB 前缀不同，但最后 displacement 都是 0x38；逐条保存完整已知字节。
        expected = {
            0x42BA2A: "03 44 7e 38",
            0x42BA49: "03 44 7e 38",
            0x42BA68: "03 44 56 38",
        }[va]
        # 初学者逐步说明：调用 `assert_bytes` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        assert_bytes(pe, va, expected, f"A装备修正Ail2+0x38_{va:08X}", checks)

    # A 状态 wrapper：slot3=+0x1F4（疲弱）与 slot22=+0x240（神力）。
    assert_bytes(pe, 0x42BA8B, "8b 8e f4 01 00 00", "A状态读取raw3疲弱槽", checks)
    # 初学者逐步说明：调用 `assert_bytes` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    assert_bytes(pe, 0x42BAA0, "8b 8e 40 02 00 00", "A状态读取raw22神力槽", checks)

    # B base：读取 +0x40 后使用 cdq/sub/sar 实现 signed trunc(value/2)。
    assert_bytes(pe, 0x42BAD6, "8b 46 40 8b 8e 90 00 00 00 99 2b c2", "B基线读取體魄并准备除2", checks)
    # 初学者逐步说明：调用 `assert_bytes` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    assert_bytes(pe, 0x42BAE8, "d1 f8", "B基线signed除2", checks)
    # 初学者逐步说明：依次遍历 `((4373247, '03 44 7a 3c'), (4373278, '03 44 7a 3c'), (4373309, '03 ...` 中的每一项，并把当前项放进 `(va, expected)`；循环体会对每一项执行同样的只读核对。
    for va, expected in (
        (0x42BAFF, "03 44 7a 3c"),
        (0x42BB1E, "03 44 7a 3c"),
        (0x42BB3D, "03 44 72 3c"),
    ):
        # 初学者逐步说明：调用 `assert_bytes` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        assert_bytes(pe, va, expected, f"B装备修正Ail2+0x3C_{va:08X}", checks)

    # B wrapper 的三个已证 runtime 状态字段。
    assert_bytes(pe, 0x42BB5B, "8b 8e 24 02 00 00", "B状态读取raw15槽", checks)
    # 初学者逐步说明：调用 `assert_bytes` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    assert_bytes(pe, 0x42BB6E, "8b 8e 4c 02 00 00", "B状态读取raw25披甲槽", checks)
    # 初学者逐步说明：调用 `assert_bytes` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    assert_bytes(pe, 0x42BB95, "8b 8e 00 02 00 00", "B状态读取raw6蚀御槽", checks)

    # A/B wrappers 的 direct caller 集合也要保存，证明它们确实被 D5 formula helper 反复共同使用。
    a_callers = direct_e8_callers(pe, 0x42BA80)
    # 初学者逐步说明：把右侧结果保存到 `b_callers`；这里保存的是 `direct_e8_callers(pe, 4373328)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    b_callers = direct_e8_callers(pe, 0x42BB50)
    # 初学者逐步说明：把右侧结果保存到 `expected_a`；这里保存的是 `[4371882, 4371962, 4372026, 4372203, 4372314, 4372442]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    expected_a = [0x42B5AA, 0x42B5FA, 0x42B63A, 0x42B6EB, 0x42B75A, 0x42B7DA]
    # 初学者逐步说明：把右侧结果保存到 `expected_b`；这里保存的是 `[4371906, 4371976, 4372123, 4372267, 4372403, 4372504]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    expected_b = [0x42B5C2, 0x42B608, 0x42B69B, 0x42B72B, 0x42B7B3, 0x42B818]
    # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    checks.append({
        "名称": "A_wrapper_direct_callers",
        "期望": [f"0x{x:08X}" for x in expected_a],
        "实际": [f"0x{x:08X}" for x in a_callers],
        "通过": a_callers == expected_a,
    })
    # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    checks.append({
        "名称": "B_wrapper_direct_callers",
        "期望": [f"0x{x:08X}" for x in expected_b],
        "实际": [f"0x{x:08X}" for x in b_callers],
        "通过": b_callers == expected_b,
    })

    # 初学者逐步说明：把 `{'A': {'中性实现名': '膂力派生攻击侧量A', 'builder': '0x42BA80', 'base_builder': '0x42BA00', '作...` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return {
        "A": {
            "中性实现名": "膂力派生攻击侧量A",
            "builder": "0x42BA80",
            "base_builder": "0x42BA00",
            "作者基础属性": "膂力",
            "RoleDefinition基础偏移": "0x3C",
            "装备修正": "三件装备各自 Ail2+0x38",
            "状态修正": {
                "raw3_疲弱": "正值时乘0.7（整数截断）",
                "raw22_神力": "疲弱不生效且神力正值时乘1.3（整数截断）",
            },
            "证据边界": "作者只正式命名基础属性『膂力』；没有证据证明原源码把A叫『攻击力』。",
        },
        "B": {
            "中性实现名": "體魄派生防御侧量B",
            "builder": "0x42BB50",
            "base_builder": "0x42BAD0",
            "作者基础属性": "體魄",
            "RoleDefinition基础偏移": "0x40",
            "基础公式": "trunc_signed(體魄/2)",
            "装备修正": "三件装备各自 Ail2+0x3C",
            "状态修正": {
                "raw15": "正值时乘0.5（整数截断；固化47已有作者语义链）",
                "raw25_披甲": "raw15不生效且披甲正值时乘1.3（整数截断）",
                "raw6_蝕禦": "前两项不生效且蝕禦正值时乘0.7（整数截断）",
            },
            "证据边界": "作者只正式命名基础属性『體魄』；没有证据证明原源码把B叫『防御力』。",
        },
    }


def add_growth_and_ui_call_checks(pe: PeImage, checks: List[Dict[str, Any]]) -> Dict[str, Any]:
    """补充成长函数和 UI result 复制链的 caller 级断言。"""
    # 0x443B60 只由 0x44387B 直接调用；它生成升级/成长 result record。
    growth_callers = direct_e8_callers(pe, 0x443B60)
    # 初学者逐步说明：把右侧结果保存到 `expected_growth_callers`；这里保存的是 `[4470907]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    expected_growth_callers = [0x44387B]
    # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    checks.append({
        "名称": "成长结果构造函数_direct_callers",
        "期望": [f"0x{x:08X}" for x in expected_growth_callers],
        "实际": [f"0x{x:08X}" for x in growth_callers],
        "通过": growth_callers == expected_growth_callers,
    })

    # 0x443D40 是五项成长随机增量 helper；0x443B60 对它恰好调用5次。
    growth_roll_callers = direct_e8_callers(pe, 0x443D40)
    # 初学者逐步说明：把右侧结果保存到 `expected_roll_callers`；这里保存的是 `[4471893, 4471924, 4471955, 4471986, 4472017]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    expected_roll_callers = [0x443C55, 0x443C74, 0x443C93, 0x443CB2, 0x443CD1]
    # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    checks.append({
        "名称": "五项成长随机helper_direct_callers",
        "期望": [f"0x{x:08X}" for x in expected_roll_callers],
        "实际": [f"0x{x:08X}" for x in growth_roll_callers],
        "通过": growth_roll_callers == expected_roll_callers,
    })

    # 0x412C60 负责把 0x70-byte result record 复制进 UI 队列；唯一 caller 是 0x44398C。
    result_enqueue_callers = direct_e8_callers(pe, 0x412C60)
    # 初学者逐步说明：把右侧结果保存到 `expected_enqueue`；这里保存的是 `[4471180]`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    expected_enqueue = [0x44398C]
    # 初学者逐步说明：调用 `checks.append` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    checks.append({
        "名称": "成长result入UI_direct_callers",
        "期望": [f"0x{x:08X}" for x in expected_enqueue],
        "实际": [f"0x{x:08X}" for x in result_enqueue_callers],
        "通过": result_enqueue_callers == expected_enqueue,
    })

    # 初学者逐步说明：把 `{'成长结果构造': '0x443B60', '五项成长随机helper': '0x443D40', 'UI result入队': '0x412C60', 'UI显...` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return {
        "成长结果构造": "0x443B60",
        "五项成长随机helper": "0x443D40",
        "UI result入队": "0x412C60",
        "UI显示函数范围": "0x413170..0x4132D3",
    }


# -----------------------------
# 六、输出报告
# -----------------------------
def write_csv(path: Path, rows: List[Dict[str, Any]]) -> None:
    """把五项能力映射写成 UTF-8-SIG CSV，方便 Windows Excel 直接打开中文。"""
    # 初学者逐步说明：调用 `path.parent.mkdir` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    path.parent.mkdir(parents=True, exist_ok=True)
    # 初学者逐步说明：把右侧结果保存到 `fieldnames`；这里保存的是 `list(rows[0].keys())`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    fieldnames = list(rows[0].keys())
    # 初学者逐步说明：进入受管理的文件/资源操作区；离开缩进块时Python会自动完成关闭或清理，避免文件句柄残留。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        # 初学者逐步说明：把右侧结果保存到 `writer`；这里保存的是 `csv.DictWriter(handle, fieldnames=fieldnames)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        # 初学者逐步说明：调用 `writer.writeheader` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        writer.writeheader()
        # 初学者逐步说明：调用 `writer.writerows` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
        writer.writerows(rows)


def main() -> int:
    """命令行入口：读输入、做断言、写 JSON/CSV，并用退出码表示 PASS/FAIL。"""
    # 初学者逐步说明：把右侧结果保存到 `parser`；这里保存的是 `argparse.ArgumentParser(description='固化53：Battle五项核心能力作者名称与公式A/B派生量...`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    parser = argparse.ArgumentParser(description="固化53：Battle五项核心能力作者名称与公式A/B派生量调查器")
    # 初学者逐步说明：调用 `parser.add_argument` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    parser.add_argument("--rpg", required=True, type=Path, help="RPG.exe 或本轮 RPG.exe.org 解压后的 RPG.exe")
    # 初学者逐步说明：调用 `parser.add_argument` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    parser.add_argument("--ail2", required=True, type=Path, help="multimedia/public/Ail2.ENC")
    # 初学者逐步说明：调用 `parser.add_argument` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    parser.add_argument("--json-out", required=True, type=Path, help="结构化 JSON 输出路径")
    # 初学者逐步说明：调用 `parser.add_argument` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    parser.add_argument("--csv-out", required=True, type=Path, help="五项能力映射 CSV 输出路径")
    # 初学者逐步说明：把右侧结果保存到 `args`；这里保存的是 `parser.parse_args()`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    args = parser.parse_args()

    # 一次性把两个输入读进内存。文件都很小，这样后面不会反复打开磁盘文件。
    rpg_data = args.rpg.read_bytes()
    # 初学者逐步说明：把右侧结果保存到 `ail2_encoded`；这里保存的是 `args.ail2.read_bytes()`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    ail2_encoded = args.ail2.read_bytes()
    # 初学者逐步说明：把右侧结果保存到 `ail2_decoded`；这里保存的是 `decode_ail2(ail2_encoded)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    ail2_decoded = decode_ail2(ail2_encoded)
    # 初学者逐步说明：把右侧结果保存到 `pe`；这里保存的是 `PeImage(rpg_data)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    pe = PeImage(rpg_data)

    # checks 里的每一项都有“期望/实际/通过”，最后统一统计，便于接档者定位具体失败点。
    checks: List[Dict[str, Any]] = []

    # 输入身份本身也是断言。若另一版资源不同，工具仍会继续解析，但最终状态会FAIL，避免误固化。
    rpg_sha = sha256_bytes(rpg_data)
    # 初学者逐步说明：把右侧结果保存到 `ail2_encoded_sha`；这里保存的是 `sha256_bytes(ail2_encoded)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    ail2_encoded_sha = sha256_bytes(ail2_encoded)
    # 初学者逐步说明：把右侧结果保存到 `ail2_decoded_sha`；这里保存的是 `sha256_bytes(ail2_decoded)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    ail2_decoded_sha = sha256_bytes(ail2_decoded)
    # 初学者逐步说明：调用 `checks.extend` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    checks.extend([
        {"名称": "RPG.exe SHA-256", "期望": EXPECTED_RPG_SHA256, "实际": rpg_sha, "通过": rpg_sha == EXPECTED_RPG_SHA256},
        {"名称": "Ail2.ENC encoded SHA-256", "期望": EXPECTED_AIL2_ENCODED_SHA256, "实际": ail2_encoded_sha, "通过": ail2_encoded_sha == EXPECTED_AIL2_ENCODED_SHA256},
        {"名称": "Ail2.ENC decoded SHA-256", "期望": EXPECTED_AIL2_DECODED_SHA256, "实际": ail2_decoded_sha, "通过": ail2_decoded_sha == EXPECTED_AIL2_DECODED_SHA256},
    ])

    # 初学者逐步说明：把右侧结果保存到 `stat_rows`；这里保存的是 `build_stat_rows(pe, ail2_decoded, checks)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    stat_rows = build_stat_rows(pe, ail2_decoded, checks)
    # 初学者逐步说明：把右侧结果保存到 `ab`；这里保存的是 `add_ab_machine_checks(pe, checks)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    ab = add_ab_machine_checks(pe, checks)
    # 初学者逐步说明：把右侧结果保存到 `growth_ui_chain`；这里保存的是 `add_growth_and_ui_call_checks(pe, checks)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    growth_ui_chain = add_growth_and_ui_call_checks(pe, checks)

    # 初学者逐步说明：把右侧结果保存到 `passed`；这里保存的是 `sum((1 for check in checks if check['通过']))`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    passed = sum(1 for check in checks if check["通过"])
    # 初学者逐步说明：把右侧结果保存到 `total`；这里保存的是 `len(checks)`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    total = len(checks)
    # 初学者逐步说明：把右侧结果保存到 `status`；这里保存的是 `'PASS' if passed == total else 'FAIL'`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    status = "PASS" if passed == total else "FAIL"

    # 初学者逐步说明：把右侧结果保存到 `report`；这里保存的是 `{'工具版本': TOOL_VERSION, '固化节点': 53, '状态': status, '机器与资源断言': {'通过': ...`，后续步骤直接复用这个中间结果，避免重复计算或弄混数据来源。
    report: Dict[str, Any] = {
        "工具版本": TOOL_VERSION,
        "固化节点": 53,
        "状态": status,
        "机器与资源断言": {"通过": passed, "总数": total, "明细": checks},
        "输入": {
            "RPG.exe": {"路径": str(args.rpg), "大小": len(rpg_data), "SHA256": rpg_sha},
            "Ail2.ENC": {
                "路径": str(args.ail2),
                "encoded大小": len(ail2_encoded),
                "encoded_SHA256": ail2_encoded_sha,
                "decoded_SHA256": ail2_decoded_sha,
                "记录数": AIL2_RECORD_COUNT,
                "记录大小": AIL2_RECORD_SIZE,
            },
        },
        "五项作者能力": stat_rows,
        "成长与UI链": growth_ui_chain,
        "D5公式派生量": ab,
        "固化53结论": [
            "RoleDefinition+0x3C/+0x40/+0x44/+0x48/+0x4C 的作者正式名称分别为膂力/體魄/靈力/迅捷/機運。",
            "五项名称由作者UI硬编码字符串、0x443B60成长old/new写回、0x413xxx显示消费三者直接闭合，并有永久能力物品writer/作者说明作为独立资源交叉证据。",
            "D5内部量A以膂力为基础，叠加装备Ail2+0x38并受疲弱/神力倍率影响；可安全称为膂力派生攻击侧量A。",
            "D5内部量B以trunc(體魄/2)为基础，叠加装备Ail2+0x3C并受状态倍率影响；可安全称为體魄派生防御侧量B。",
            "当前没有作者字符串证明内部A/B本身正式叫攻击力/防御力；兼容引擎文档不得把中性派生量名伪装成作者源码变量名。",
        ],
        "仍保持UNKNOWN": [
            "Ail2+0x38 / +0x3C 两个装备修正字段的作者正式字段名；本节点只证明它们分别进入A/B。",
            "A/B在原作者源码中的变量名。",
            "非canonical损坏/MOD数据下五项能力的范围约束策略。",
        ],
    }

    # 初学者逐步说明：调用 `args.json_out.parent.mkdir` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    # 初学者逐步说明：调用 `args.json_out.write_text` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    # 初学者逐步说明：调用 `write_csv` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    write_csv(args.csv_out, stat_rows)

    # 初学者逐步说明：调用 `print` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    print(f"固化53 Battle五项核心能力调查：{status} ({passed}/{total})")
    # 初学者逐步说明：调用 `print` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    print("五项：" + " / ".join(f"+0x{row['RoleDefinition偏移'][2:]}={row['作者正式名称']}" for row in stat_rows))
    # 初学者逐步说明：调用 `print` 执行当前步骤；这里主要产生核对、写出或记录副作用，不把返回值当成新的业务数值。
    print("A：膂力派生攻击侧量；B：體魄派生防御侧量（不冒充作者变量名）")
    # 初学者逐步说明：把 `0 if status == 'PASS' else 1` 作为本函数结果交还调用者；执行到这里后，本函数这一条路径立即结束。
    return 0 if status == "PASS" else 1


# 初学者逐步说明：检查条件 `__name__ == '__main__'`；只有条件成立才进入下面缩进块，因此异常输入和正常输入不会混在同一路径处理。
if __name__ == "__main__":
    # 初学者逐步说明：当前输入已经违反本工具的证据前提，因此立即抛出异常停止这一条路径，防止继续用错误偏移或错误资源生成伪结论。
    raise SystemExit(main())
