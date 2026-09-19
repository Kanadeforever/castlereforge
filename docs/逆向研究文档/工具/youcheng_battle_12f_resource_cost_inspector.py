#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle FIRTTECH ID 0x12F（绝剑凶号）主资源代价调查器（固化52）。

这个工具只做“静态、只读、可重复”的证据检查，不启动游戏，也不会改写任何输入文件。
它把三个原始输入放在同一条证据链里：

1. RPG.exe：确认普通术法 +0x34 的平坦扣除、ID 0x12F 的跳过分支、mode2 特殊分派、
   百分比扣除公式、最低1点夹取，以及 AI 候选检查仍按平坦 +0x34 比较的原版差异；
2. Firttech.enc：确认 0x12F 这一条真实资源记录就是“绝剑凶号”，并读取 +0x34/+0x38/+0x30/+0x40；
3. Api.enc：只用于检查当前 canonical RoleDefinition 的主资源 max 基线范围，证明 20% 公式在现有正式资产上
   不会因为第一步 32-bit 乘法溢出而改变数学结果。

固化52最重要的结论是：

- 对绝大多数 FIRTTECH，+0x34 在动作落地阶段作为“固定整数主资源 cost”直接相减；
- 唯独 selected FIRTTECH ID == 0x12F 时，原版显式跳过这个平坦扣除；
- 随后 mode2 特殊分派把同一个 +0x34 重新解释为“最大主资源的百分比整数参数”；
- 当前 0x12F 记录的 +0x34 = 20，因此实际主资源变化为：
      current -= trunc(max * 20 / 100)
  若结果 <= 0，则原版强制 current = 1；
- +0x38 次资源 cost 不参加这个特例。代码仍按普通路径平坦相减；只是当前 0x12F 记录恰好 +0x38 = 0；
- AI 候选预检查仍用 current_main >= raw(+0x34) 的平坦比较，没有 0x12F 百分比特判。
  这是原版静态语义差异，兼容实现不能为了“更合理”擅自统一。

【证据边界】
“主资源/次资源”沿用前代已经闭合的结构性称呼，作者正式属性名仍保持 UNKNOWN。
本工具不把 0x12F 的 UI 描述、AI 可用性或角色专属关系扩大成未经机器证据支持的规则。
"""

import argparse
import csv
import hashlib
import json
import struct
from pathlib import Path
from typing import Dict, List, Tuple


# 这一步把右侧计算/读取出来的结果保存到“TOOL_VERSION”，后面的检查会继续使用这个值。
TOOL_VERSION = "0.7D-solid52.0"

# FIRTTECH 与 API 的结构尺寸已经由前代固化确认。
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_RECORD_SIZE”，后面的检查会继续使用这个值。
FIRTTECH_RECORD_SIZE = 0x230
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_RECORD_COUNT”，后面的检查会继续使用这个值。
FIRTTECH_RECORD_COUNT = 500
# 这一步把右侧计算/读取出来的结果保存到“API_RECORD_SIZE”，后面的检查会继续使用这个值。
API_RECORD_SIZE = 0x350
# 这一步把右侧计算/读取出来的结果保存到“API_RECORD_COUNT”，后面的检查会继续使用这个值。
API_RECORD_COUNT = 255

# 本轮只围绕这个唯一特殊 ID 展开。
# 这一步把右侧计算/读取出来的结果保存到“SPECIAL_ID”，后面的检查会继续使用这个值。
SPECIAL_ID = 0x12F

# FIRTTECH 中本轮实际要读取的字段。
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_ID_OFF”，后面的检查会继续使用这个值。
FIRTTECH_ID_OFF = 0x00
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_NAME_OFF”，后面的检查会继续使用这个值。
FIRTTECH_NAME_OFF = 0x04
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_NAME_SIZE”，后面的检查会继续使用这个值。
FIRTTECH_NAME_SIZE = 0x14
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_MODE_OFF”，后面的检查会继续使用这个值。
FIRTTECH_MODE_OFF = 0x18
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_SELECTOR_OFF”，后面的检查会继续使用这个值。
FIRTTECH_SELECTOR_OFF = 0x30
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_MAIN_COST_OFF”，后面的检查会继续使用这个值。
FIRTTECH_MAIN_COST_OFF = 0x34
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_SECOND_COST_OFF”，后面的检查会继续使用这个值。
FIRTTECH_SECOND_COST_OFF = 0x38
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_K_OFF”，后面的检查会继续使用这个值。
FIRTTECH_K_OFF = 0x40
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_DESC_OFF”，后面的检查会继续使用这个值。
FIRTTECH_DESC_OFF = 0x17C
# 这一步把右侧计算/读取出来的结果保存到“FIRTTECH_DESC_SIZE”，后面的检查会继续使用这个值。
FIRTTECH_DESC_SIZE = 0xB4

# API/RoleDefinition 的名字与主资源 max/current 字段位置。
# 这一步把右侧计算/读取出来的结果保存到“API_NAME_OFF”，后面的检查会继续使用这个值。
API_NAME_OFF = 0x0C
# 这一步把右侧计算/读取出来的结果保存到“API_NAME_SIZE”，后面的检查会继续使用这个值。
API_NAME_SIZE = 0x14
# 这一步把右侧计算/读取出来的结果保存到“API_MAIN_MAX_OFF”，后面的检查会继续使用这个值。
API_MAIN_MAX_OFF = 0x2C
# 这一步把右侧计算/读取出来的结果保存到“API_MAIN_CURRENT_OFF”，后面的检查会继续使用这个值。
API_MAIN_CURRENT_OFF = 0x30

# 关键函数/表地址。地址只是当前 EXE 的已证机器位置，不等于作者符号名。
# 这一步把右侧计算/读取出来的结果保存到“VA_SPECIAL_DISPATCH”，后面的检查会继续使用这个值。
VA_SPECIAL_DISPATCH = 0x00421A00
# 这一步把右侧计算/读取出来的结果保存到“VA_SPECIAL_SELECTOR_TABLE”，后面的检查会继续使用这个值。
VA_SPECIAL_SELECTOR_TABLE = 0x00422074
# 这一步把右侧计算/读取出来的结果保存到“VA_SPECIAL_JUMP_TABLE”，后面的检查会继续使用这个值。
VA_SPECIAL_JUMP_TABLE = 0x00422044
# 这一步把右侧计算/读取出来的结果保存到“VA_SPECIAL_ID12F_TARGET”，后面的检查会继续使用这个值。
VA_SPECIAL_ID12F_TARGET = 0x00421BCA


# -----------------------------------------------------------------------------
# 一、最基础的整数、文本、哈希函数
# -----------------------------------------------------------------------------
# 下面定义函数“u16”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def u16(data: bytes, offset: int) -> int:
    """从 little-endian 字节流读取一个无符号16位整数。"""
    # '<H' 的 '<' 表示小端，H 表示 unsigned short。
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return struct.unpack_from("<H", data, offset)[0]


# 下面定义函数“u32”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def u32(data: bytes, offset: int) -> int:
    """从 little-endian 字节流读取一个无符号32位整数。"""
    # PE 地址、记录 ID 都按这个方式读取。
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return struct.unpack_from("<I", data, offset)[0]


# 下面定义函数“s32”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def s32(data: bytes, offset: int) -> int:
    """从 little-endian 字节流读取一个有符号32位整数。"""
    # 资源 cost、selector 等运行时会按 signed int 参与比较/算术，因此这里保留符号。
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return struct.unpack_from("<i", data, offset)[0]


# 下面定义函数“sha256_bytes”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def sha256_bytes(data: bytes) -> str:
    """计算输入 bytes 的 SHA-256，方便以后确认是不是同一份证据。"""
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return hashlib.sha256(data).hexdigest()


# 下面定义函数“decode_period255”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def decode_period255(raw: bytes) -> bytes:
    """
    解码 Public/*.ENC 当前使用的 period-255 XOR。

    第 i 个字节只做：decoded[i] = raw[i] XOR (i % 255)。
    XOR 是可逆运算，所以不会需要额外密钥表。
    """
    # enumerate 同时给出当前位置 i 与原字节 value；每个位置独立 XOR。
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return bytes(value ^ (index % 255) for index, value in enumerate(raw))


# 下面定义函数“decode_cp950_field”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def decode_cp950_field(record: bytes, offset: int, size: int) -> str:
    """读取固定宽度、NUL 结尾的 Big5/CP950 文本字段。"""
    # 先切固定范围，再在第一个 0x00 停止，避免把后面的二进制字段当文字。
    # 这一步把右侧计算/读取出来的结果保存到“raw”，后面的检查会继续使用这个值。
    raw = record[offset:offset + size].split(b"\0", 1)[0]
    # errors='replace' 只保证坏字节不会让调查器崩溃；原始文件哈希仍完整保留。
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return raw.decode("cp950", errors="replace")


# 下面定义函数“to_s32”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def to_s32(value: int) -> int:
    """把 Python 任意整数按 x86 32-bit signed 规则折回 -2^31..2^31-1。"""
    # 先只保留低32位。
    # 这里在“value”原有数值基础上继续累加或扣减，用于推进当前计数/偏移。
    value &= 0xFFFFFFFF
    # 如果最高位为1，就把它解释成负数。
    # 这里检查条件“value & 2147483648”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
    if value & 0x80000000:
        # 这里在“value”原有数值基础上继续累加或扣减，用于推进当前计数/偏移。
        value -= 0x100000000
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return value


# 下面定义函数“trunc_div_toward_zero”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def trunc_div_toward_zero(numerator: int, denominator: int) -> int:
    """模拟 x86 IDIV 的“向0截断”有符号整数除法。"""
    # 这里检查条件“denominator == 0”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
    if denominator == 0:
        # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
        raise ZeroDivisionError("IDIV denominator is zero")
    # Python // 对负数会向负无穷取整，所以不能直接使用。
    # 这一步把右侧计算/读取出来的结果保存到“magnitude”，后面的检查会继续使用这个值。
    magnitude = abs(numerator) // abs(denominator)
    # 被除数与除数异号时结果为负；同号时为正。
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return -magnitude if (numerator < 0) ^ (denominator < 0) else magnitude


# -----------------------------------------------------------------------------
# 二、最小 PE32 映射器
# -----------------------------------------------------------------------------
# 下面定义“PeImage”这个小工具类，用来集中保存同一类数据和操作，避免地址换算逻辑散落在各处。
class PeImage:
    """
    只读 PE32 地址映射器。

    逆向证据里写的是运行时 VA（例如 0x00421BCA），而磁盘 EXE 按 section raw offset 存放。
    这个类只负责把 VA 换成文件偏移并读字节，不做反汇编、不改写文件。
    """

    # 下面定义函数“__init__”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
    def __init__(self, data: bytes):
        # 保存原始 EXE bytes，后面的所有读取都从这里发生。
        # 这一步把右侧计算/读取出来的结果保存到“self.data”，后面的检查会继续使用这个值。
        self.data = data
        # Windows PE 必须先有 DOS 'MZ' 头。
        # 这里检查条件“data[:2] != b'MZ'”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
        if data[:2] != b"MZ":
            # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
            raise ValueError("输入不是 MZ/PE 可执行文件")

        # DOS 头 0x3C 保存真正 PE 头的文件偏移。
        # 这一步把右侧计算/读取出来的结果保存到“pe_offset”，后面的检查会继续使用这个值。
        pe_offset = u32(data, 0x3C)
        # 这里检查条件“data[pe_offset:pe_offset + 4] != b'PE\x00\x00'”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
            raise ValueError("找不到 PE\\0\\0 签名")

        # COFF 头紧跟在 PE 签名后面。
        # 这一步把右侧计算/读取出来的结果保存到“coff”，后面的检查会继续使用这个值。
        coff = pe_offset + 4
        # 这一步把右侧计算/读取出来的结果保存到“section_count”，后面的检查会继续使用这个值。
        section_count = u16(data, coff + 2)
        # 这一步把右侧计算/读取出来的结果保存到“optional_size”，后面的检查会继续使用这个值。
        optional_size = u16(data, coff + 16)
        # 这一步把右侧计算/读取出来的结果保存到“optional”，后面的检查会继续使用这个值。
        optional = coff + 20

        # 0x10B 是 PE32；本游戏是32位程序。
        # 这里检查条件“u16(data, optional) != 267”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
        if u16(data, optional) != 0x10B:
            # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
            raise ValueError("本调查器只支持当前游戏使用的 PE32")

        # ImageBase 用于把绝对 VA 换成 section RVA。
        # 这一步把右侧计算/读取出来的结果保存到“self.image_base”，后面的检查会继续使用这个值。
        self.image_base = u32(data, optional + 0x1C)
        # 这一步把右侧计算/读取出来的结果保存到“section_table”，后面的检查会继续使用这个值。
        section_table = optional + optional_size
        # 这一步把右侧计算/读取出来的结果保存到“self.sections”，后面的检查会继续使用这个值。
        self.sections: List[Dict[str, int]] = []

        # 每个 section header 固定40字节；逐个保存映射需要的字段。
        # 这里逐个遍历“range(section_count)”中的项目，让同一套读取或验证规则覆盖每一项，而不是只检查一个样本。
        for index in range(section_count):
            # 这一步把右侧计算/读取出来的结果保存到“base”，后面的检查会继续使用这个值。
            base = section_table + index * 40
            # 这一步把右侧计算/读取出来的结果保存到“name”，后面的检查会继续使用这个值。
            name = data[base:base + 8].split(b"\0", 1)[0].decode("ascii", "replace")
            # 这里执行“self.sections.append({'name': name, 'virtual_size': u32(data, base + 8), 'v...”；它用于输出、写入结果或触发一个明确的检查步骤。
            self.sections.append({
                "name": name,
                "virtual_size": u32(data, base + 8),
                "virtual_address": u32(data, base + 12),
                "raw_size": u32(data, base + 16),
                "raw_offset": u32(data, base + 20),
            })

    # 下面定义函数“va_to_offset”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
    def va_to_offset(self, va: int) -> int:
        """把绝对虚拟地址 VA 转换成磁盘文件偏移。"""
        # 先减 ImageBase 得到 RVA。
        # 这一步把右侧计算/读取出来的结果保存到“rva”，后面的检查会继续使用这个值。
        rva = va - self.image_base
        # 这里逐个遍历“self.sections”中的项目，让同一套读取或验证规则覆盖每一项，而不是只检查一个样本。
        for section in self.sections:
            # 这一步把右侧计算/读取出来的结果保存到“start”，后面的检查会继续使用这个值。
            start = section["virtual_address"]
            # virtual_size 与 raw_size 取较大者，避免尾部映射被漏掉。
            # 这一步把右侧计算/读取出来的结果保存到“span”，后面的检查会继续使用这个值。
            span = max(section["virtual_size"], section["raw_size"])
            # 这里检查条件“start <= rva < start + span”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
            if start <= rva < start + span:
                # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
                return section["raw_offset"] + (rva - start)
        # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
        raise ValueError(f"VA 0x{va:08X} 不属于任何 PE section")

    # 下面定义函数“read_va”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
    def read_va(self, va: int, size: int) -> bytes:
        """读取某个 VA 起始的 size 个原始机器字节。"""
        # 这一步把右侧计算/读取出来的结果保存到“offset”，后面的检查会继续使用这个值。
        offset = self.va_to_offset(va)
        # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
        return self.data[offset:offset + size]

    # 下面定义函数“text_range”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
    def text_range(self) -> Tuple[int, bytes]:
        """返回 .text 的起始 VA 与原始 bytes，供 direct E8 caller 扫描。"""
        # 这里逐个遍历“self.sections”中的项目，让同一套读取或验证规则覆盖每一项，而不是只检查一个样本。
        for section in self.sections:
            # 这里检查条件“section['name'] == '.text'”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
            if section["name"] == ".text":
                # 这一步把右侧计算/读取出来的结果保存到“va”，后面的检查会继续使用这个值。
                va = self.image_base + section["virtual_address"]
                # 这一步把右侧计算/读取出来的结果保存到“raw”，后面的检查会继续使用这个值。
                raw = self.data[section["raw_offset"]:section["raw_offset"] + section["raw_size"]]
                # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
                return va, raw
        # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
        raise ValueError("找不到 .text section")


# -----------------------------------------------------------------------------
# 三、机器断言和 direct E8 caller 扫描
# -----------------------------------------------------------------------------
# 下面定义函数“make_byte_assertion”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def make_byte_assertion(pe: PeImage, name: str, va: int, expected_hex: str) -> Dict[str, object]:
    """检查某段关键机器码是否仍与本轮逆向使用的字节完全一致。"""
    # 这一步把右侧计算/读取出来的结果保存到“expected”，后面的检查会继续使用这个值。
    expected = bytes.fromhex(expected_hex)
    # 这一步把右侧计算/读取出来的结果保存到“actual”，后面的检查会继续使用这个值。
    actual = pe.read_va(va, len(expected))
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return {
        "名称": name,
        "VA": f"0x{va:08X}",
        "期望": expected.hex(" "),
        "实际": actual.hex(" "),
        "PASS": actual == expected,
    }


# 下面定义函数“find_direct_e8_callers”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def find_direct_e8_callers(pe: PeImage, target_va: int) -> List[int]:
    """
    扫描 .text 中所有 opcode E8 rel32，并返回直接调用 target_va 的 call 指令地址。

    这只能证明“direct E8 调用面”，不能证明函数指针、虚表或动态间接调用绝对不存在。
    """
    # 这一步把右侧计算/读取出来的结果保存到“(text_va, text)”，后面的检查会继续使用这个值。
    text_va, text = pe.text_range()
    # 这一步把右侧计算/读取出来的结果保存到“callers”，后面的检查会继续使用这个值。
    callers: List[int] = []

    # E8 后面固定跟4字节有符号相对位移，所以最后4字节不能作为起点。
    # 这里逐个遍历“range(0, len(text) - 4)”中的项目，让同一套读取或验证规则覆盖每一项，而不是只检查一个样本。
    for offset in range(0, len(text) - 4):
        # 这里检查条件“text[offset] != 232”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
        if text[offset] != 0xE8:
            # 当前这一项已经不满足继续处理的条件；这里直接跳到循环下一项，避免把无效记录混进统计。
            continue
        # 这一步把右侧计算/读取出来的结果保存到“rel”，后面的检查会继续使用这个值。
        rel = struct.unpack_from("<i", text, offset + 1)[0]
        # 这一步把右侧计算/读取出来的结果保存到“call_va”，后面的检查会继续使用这个值。
        call_va = text_va + offset
        # 这一步把右侧计算/读取出来的结果保存到“resolved”，后面的检查会继续使用这个值。
        resolved = call_va + 5 + rel
        # 这里检查条件“resolved == target_va”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
        if resolved == target_va:
            # 这里执行“callers.append(call_va)”；它用于输出、写入结果或触发一个明确的检查步骤。
            callers.append(call_va)

    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return callers


# -----------------------------------------------------------------------------
# 四、真实资源解析
# -----------------------------------------------------------------------------
# 下面定义函数“parse_special_firttech”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def parse_special_firttech(decoded: bytes) -> Dict[str, object]:
    """在500条 FIRTTECH 记录中找到唯一 ID 0x12F，并输出本轮相关字段。"""
    # 这一步把右侧计算/读取出来的结果保存到“expected_size”，后面的检查会继续使用这个值。
    expected_size = FIRTTECH_RECORD_SIZE * FIRTTECH_RECORD_COUNT
    # 这里检查条件“len(decoded) != expected_size”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
    if len(decoded) != expected_size:
        # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
        raise ValueError(f"Firttech解码后应为 {expected_size} 字节，实际 {len(decoded)}")

    # 这一步把右侧计算/读取出来的结果保存到“matches”，后面的检查会继续使用这个值。
    matches: List[Dict[str, object]] = []
    # 这里逐个遍历“range(FIRTTECH_RECORD_COUNT)”中的项目，让同一套读取或验证规则覆盖每一项，而不是只检查一个样本。
    for index in range(FIRTTECH_RECORD_COUNT):
        # 这一步把右侧计算/读取出来的结果保存到“start”，后面的检查会继续使用这个值。
        start = index * FIRTTECH_RECORD_SIZE
        # 这一步把右侧计算/读取出来的结果保存到“record”，后面的检查会继续使用这个值。
        record = decoded[start:start + FIRTTECH_RECORD_SIZE]
        # 这一步把右侧计算/读取出来的结果保存到“record_id”，后面的检查会继续使用这个值。
        record_id = u32(record, FIRTTECH_ID_OFF)
        # 这里检查条件“record_id != SPECIAL_ID”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
        if record_id != SPECIAL_ID:
            # 当前这一项已经不满足继续处理的条件；这里直接跳到循环下一项，避免把无效记录混进统计。
            continue

        # 只对目标记录做完整文字/字段读取，避免报告被其余499条淹没。
        # 这里执行“matches.append({'record_index': index, 'id': record_id, 'id_hex': f'0x{reco...”；它用于输出、写入结果或触发一个明确的检查步骤。
        matches.append({
            "record_index": index,
            "id": record_id,
            "id_hex": f"0x{record_id:X}",
            "name": decode_cp950_field(record, FIRTTECH_NAME_OFF, FIRTTECH_NAME_SIZE),
            "mode_0x18": s32(record, FIRTTECH_MODE_OFF),
            "selector_0x30": s32(record, FIRTTECH_SELECTOR_OFF),
            "main_parameter_0x34": s32(record, FIRTTECH_MAIN_COST_OFF),
            "second_cost_0x38": s32(record, FIRTTECH_SECOND_COST_OFF),
            "formula_parameter_k_0x40": s32(record, FIRTTECH_K_OFF),
            "description": decode_cp950_field(record, FIRTTECH_DESC_OFF, FIRTTECH_DESC_SIZE),
        })

    # 这里检查条件“len(matches) != 1”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
    if len(matches) != 1:
        # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
        raise ValueError(f"FIRTTECH ID 0x12F 预期唯一，实际找到 {len(matches)} 条")
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return matches[0]


# 下面定义函数“parse_api_main_resource”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def parse_api_main_resource(decoded: bytes, percent_parameter: int) -> Dict[str, object]:
    """
    统计当前 API.ENC 里有名字 RoleDefinition 的主资源 max，并计算 0x12F 对应百分比 cost。

    这里不是说运行时 max 永远等于数据库初值；只用于证明当前 canonical 资产本身不会触发乘法溢出。
    """
    # 这一步把右侧计算/读取出来的结果保存到“expected_size”，后面的检查会继续使用这个值。
    expected_size = API_RECORD_SIZE * API_RECORD_COUNT
    # 这里检查条件“len(decoded) != expected_size”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
    if len(decoded) != expected_size:
        # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
        raise ValueError(f"API解码后应为 {expected_size} 字节，实际 {len(decoded)}")

    # 这一步把右侧计算/读取出来的结果保存到“rows”，后面的检查会继续使用这个值。
    rows: List[Dict[str, object]] = []
    # 这里逐个遍历“range(API_RECORD_COUNT)”中的项目，让同一套读取或验证规则覆盖每一项，而不是只检查一个样本。
    for index in range(API_RECORD_COUNT):
        # 这一步把右侧计算/读取出来的结果保存到“start”，后面的检查会继续使用这个值。
        start = index * API_RECORD_SIZE
        # 这一步把右侧计算/读取出来的结果保存到“record”，后面的检查会继续使用这个值。
        record = decoded[start:start + API_RECORD_SIZE]
        # 这一步把右侧计算/读取出来的结果保存到“name”，后面的检查会继续使用这个值。
        name = decode_cp950_field(record, API_NAME_OFF, API_NAME_SIZE)
        # 这里检查条件“not name”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
        if not name:
            # 当前这一项已经不满足继续处理的条件；这里直接跳到循环下一项，避免把无效记录混进统计。
            continue

        # 这一步把右侧计算/读取出来的结果保存到“max_main”，后面的检查会继续使用这个值。
        max_main = s32(record, API_MAIN_MAX_OFF)
        # 这一步把右侧计算/读取出来的结果保存到“current_main”，后面的检查会继续使用这个值。
        current_main = s32(record, API_MAIN_CURRENT_OFF)
        # 这一步把右侧计算/读取出来的结果保存到“product”，后面的检查会继续使用这个值。
        product = max_main * percent_parameter
        # 当前正常数据产品远小于 2^31；这里仍显式计算，防止以后资源变更时悄悄越界。
        # 这一步把右侧计算/读取出来的结果保存到“cost”，后面的检查会继续使用这个值。
        cost = trunc_div_toward_zero(product, 100)
        # 这里执行“rows.append({'record_index': index, 'name': name, 'main_max_0x2C': max_main...”；它用于输出、写入结果或触发一个明确的检查步骤。
        rows.append({
            "record_index": index,
            "name": name,
            "main_max_0x2C": max_main,
            "main_current_0x30": current_main,
            "product_max_times_parameter": product,
            "special_cost_percent_trunc": cost,
            "product_fits_signed32": -0x80000000 <= product <= 0x7FFFFFFF,
        })

    # 这里检查条件“not rows”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
    if not rows:
        # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
        raise ValueError("API中没有找到任何有名字记录")

    # 这一步把右侧计算/读取出来的结果保存到“max_values”，后面的检查会继续使用这个值。
    max_values = [int(row["main_max_0x2C"]) for row in rows]
    # 这一步把右侧计算/读取出来的结果保存到“products”，后面的检查会继续使用这个值。
    products = [int(row["product_max_times_parameter"]) for row in rows]
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return {
        "named_record_count": len(rows),
        "main_max_min": min(max_values),
        "main_max_max": max(max_values),
        "product_min": min(products),
        "product_max": max(products),
        "all_products_fit_signed32": all(bool(row["product_fits_signed32"]) for row in rows),
        "rows": rows,
    }


# -----------------------------------------------------------------------------
# 五、还原 0x421BCA 的百分比整数公式
# -----------------------------------------------------------------------------
# 下面定义函数“emulate_id12f_delta”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def emulate_id12f_delta(max_main: int, parameter: int) -> int:
    """
    用“结果语义”表达 0x421BCA 的 canonical 正常公式：delta = -trunc(max*parameter/100)。

    原机器先做32-bit IMUL得到低32位，再用 0xAE147AE1 / SAR 5 的魔数除法得到负的 /100 商。
    对当前 canonical 输入，max*parameter 已证明不会溢出，所以结果与这个清晰公式完全一致。
    """
    # 这一步把右侧计算/读取出来的结果保存到“product”，后面的检查会继续使用这个值。
    product = max_main * parameter
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return -trunc_div_toward_zero(product, 100)


# 下面定义函数“apply_id12f_cost”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def apply_id12f_cost(current_main: int, max_main: int, parameter: int) -> Dict[str, int]:
    """计算 0x12F 特例的 current 更新，并模拟“结果<=0则固定1”的夹取。"""
    # 这一步把右侧计算/读取出来的结果保存到“delta”，后面的检查会继续使用这个值。
    delta = emulate_id12f_delta(max_main, parameter)
    # 这一步把右侧计算/读取出来的结果保存到“raw_after”，后面的检查会继续使用这个值。
    raw_after = current_main + delta
    # 这一步把右侧计算/读取出来的结果保存到“clamped_after”，后面的检查会继续使用这个值。
    clamped_after = raw_after if raw_after > 0 else 1
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return {
        "current_before": current_main,
        "max_main": max_main,
        "parameter_0x34": parameter,
        "delta": delta,
        "raw_after": raw_after,
        "final_after_min1_clamp": clamped_after,
    }


# 下面定义函数“build_formula_examples”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def build_formula_examples(parameter: int) -> List[Dict[str, int]]:
    """给接档者准备几组容易人工心算的例子，用来避免把“最大值百分比”误写成“当前值百分比”。"""
    # 这一步把右侧计算/读取出来的结果保存到“examples”，后面的检查会继续使用这个值。
    examples = [
        (100, 100),
        (50, 100),
        (10, 100),
        (1, 100),
        (1000, 1000),
        (50, 1000),
    ]
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return [apply_id12f_cost(current, maximum, parameter) for current, maximum in examples]


# -----------------------------------------------------------------------------
# 六、组合本轮全部机器断言
# -----------------------------------------------------------------------------
# 下面定义函数“build_machine_assertions”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def build_machine_assertions(pe: PeImage) -> Dict[str, object]:
    """把固化52所有关键机器边界放在一处，封包回归时可以一键重放。"""
    # 这一步把右侧计算/读取出来的结果保存到“items”，后面的检查会继续使用这个值。
    items = [
        make_byte_assertion(
            pe,
            "动作落地先无条件读取FIRTTECH+0x38并从次资源current平坦相减",
            0x00421249,
            "8B 48 38 8B 97 84 08 00 00 2B D1 89 97 84 08 00 00",
        ),
        make_byte_assertion(
            pe,
            "动作落地读取selected FIRTTECH ID并显式比较0x12F",
            0x0042125A,
            "8B 08 81 F9 2F 01 00 00",
        ),
        make_byte_assertion(
            pe,
            "ID等于0x12F时直接跳过普通+0x34平坦主资源扣除",
            0x00421262,
            "74 57",
        ),
        make_byte_assertion(
            pe,
            "非0x12F普通路径读取+0x34并从主资源current直接相减",
            0x00421264,
            "8B 50 34 8B 87 7C 08 00 00 2B C2 6A 03 8B CF 89 87 7C 08 00 00",
        ),
        make_byte_assertion(
            pe,
            "普通动作落地路径以mode3调用特殊分派",
            0x00421279,
            "E8 82 07 00 00",
        ),
        make_byte_assertion(
            pe,
            "0x12F跳过平坦扣除后同样以mode3调用特殊分派",
            0x004212BB,
            "6A 03 8B CF E8 3C 07 00 00",
        ),
        make_byte_assertion(
            pe,
            "动作状态值2时把2作为mode参数调用特殊分派",
            0x004212CE,
            "83 F8 02 0F 85 AB 03 00 00 50 8B CF 89 2D 84 FD 89 00 E8 1B 07 00 00",
        ),
        make_byte_assertion(
            pe,
            "特殊分派只有mode2才进入selected FIRTTECH ID表",
            0x00421B98,
            "83 F8 02 0F 85 96 04 00 00",
        ),
        make_byte_assertion(
            pe,
            "mode2以selected ID-0x12F作为165项selector表索引",
            0x00421BA1,
            "8B 87 48 08 00 00 8B 08 8D B1 D1 FE FF FF 81 FE A4 00 00 00",
        ),
        make_byte_assertion(
            pe,
            "0x12F对应selector表第0项为0",
            VA_SPECIAL_SELECTOR_TABLE,
            "00",
        ),
        make_byte_assertion(
            pe,
            "selector0 jump target精确指向0x421BCA",
            VA_SPECIAL_JUMP_TABLE,
            "CA 1B 42 00",
        ),
        make_byte_assertion(
            pe,
            "0x12F特殊分支读取同一FIRTTECH+0x34并乘主资源max",
            0x00421BCA,
            "8B 48 34 B8 E1 7A 14 AE 0F AF 8F 78 08 00 00 F7 E9",
        ),
        make_byte_assertion(
            pe,
            "0xAE147AE1配合SAR5形成负的signed除100商",
            0x00421BD9,
            "F7 E9 8B 8F 7C 08 00 00 C1 FA 05 8B C2 C1 E8 1F 03 D0",
        ),
        make_byte_assertion(
            pe,
            "百分比负delta加到主资源current并写回",
            0x00421BDB,
            "8B 8F 7C 08 00 00 C1 FA 05 8B C2 C1 E8 1F 03 D0 03 CA 8B C1 89 8F 7C 08 00 00",
        ),
        make_byte_assertion(
            pe,
            "扣除后current<=0时强制夹到1",
            0x00421BF5,
            "3B C3 0F 8F 3A 04 00 00 C7 87 7C 08 00 00 01 00 00 00",
        ),
        make_byte_assertion(
            pe,
            "AI候选检查先按+0x38与次资源current做平坦可支付比较",
            0x004445D4,
            "8B 77 38 8D 14 CA 8B 8C 93 68 0C 00 00 3B CE",
        ),
        make_byte_assertion(
            pe,
            "AI候选检查随后仍把+0x34当平坦主资源门槛，无0x12F特判",
            0x004445EC,
            "8B 92 60 0C 00 00 8B 4F 34 3B D1 0F 8C 24 02 00 00",
        ),
    ]

    # 直接 caller 集合用扫描结果证明，不依赖人工抄写反汇编列表。
    # 这一步把右侧计算/读取出来的结果保存到“callers”，后面的检查会继续使用这个值。
    callers = find_direct_e8_callers(pe, VA_SPECIAL_DISPATCH)
    # 这一步把右侧计算/读取出来的结果保存到“expected_callers”，后面的检查会继续使用这个值。
    expected_callers = [0x00421279, 0x004212BF, 0x004212E0]
    # 这一步把右侧计算/读取出来的结果保存到“caller_check”，后面的检查会继续使用这个值。
    caller_check = {
        "名称": "0x421A00 direct E8 caller集合严格只有动作状态机三处",
        "target": f"0x{VA_SPECIAL_DISPATCH:08X}",
        "expected_callers": [f"0x{x:08X}" for x in expected_callers],
        "actual_callers": [f"0x{x:08X}" for x in callers],
        "PASS": callers == expected_callers,
    }

    # 这一步把右侧计算/读取出来的结果保存到“pass_count”，后面的检查会继续使用这个值。
    pass_count = sum(bool(item["PASS"]) for item in items) + int(bool(caller_check["PASS"]))
    # 这一步把右侧计算/读取出来的结果保存到“total_count”，后面的检查会继续使用这个值。
    total_count = len(items) + 1
    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return {
        "pass_count": pass_count,
        "total_count": total_count,
        "all_pass": pass_count == total_count,
        "byte_assertions": items,
        "direct_caller_assertion": caller_check,
    }


# -----------------------------------------------------------------------------
# 七、CSV与JSON输出
# -----------------------------------------------------------------------------
# 下面定义函数“write_api_csv”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def write_api_csv(path: Path, rows: List[Dict[str, object]]) -> None:
    """把151条有名字API记录的最大主资源与20%代价写成严格CSV。"""
    # 这里执行“path.parent.mkdir(parents=True, exist_ok=True)”；它用于输出、写入结果或触发一个明确的检查步骤。
    path.parent.mkdir(parents=True, exist_ok=True)
    # 这一步把右侧计算/读取出来的结果保存到“columns”，后面的检查会继续使用这个值。
    columns = [
        "record_index",
        "name",
        "main_max_0x2C",
        "main_current_0x30",
        "product_max_times_parameter",
        "special_cost_percent_trunc",
        "product_fits_signed32",
    ]
    # utf-8-sig 让简体中文 Windows/Excel 直接打开时更不容易出现乱码。
    # 这里用上下文管理方式打开/使用资源；离开缩进块时Python会自动收尾，避免文件句柄遗漏。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        # 这一步把右侧计算/读取出来的结果保存到“writer”，后面的检查会继续使用这个值。
        writer = csv.DictWriter(handle, fieldnames=columns)
        # 这里执行“writer.writeheader()”；它用于输出、写入结果或触发一个明确的检查步骤。
        writer.writeheader()
        # 这里逐个遍历“rows”中的项目，让同一套读取或验证规则覆盖每一项，而不是只检查一个样本。
        for row in rows:
            # 这里执行“writer.writerow({key: row.get(key, '') for key in columns})”；它用于输出、写入结果或触发一个明确的检查步骤。
            writer.writerow({key: row.get(key, "") for key in columns})


# 下面定义函数“main”。把这一小段工作单独封装，主流程就能按步骤调用，也方便逐项验证。
def main() -> int:
    # argparse 把命令行参数变成带名字的 Path，避免在代码里写死机器路径。
    # 这一步把右侧计算/读取出来的结果保存到“parser”，后面的检查会继续使用这个值。
    parser = argparse.ArgumentParser(
        description="静态核对Battle FIRTTECH ID 0x12F的最大主资源百分比代价、最低1点夹取与AI预检查差异"
    )
    # 这里执行“parser.add_argument('--rpg', type=Path, required=True, help='RPG.exe 路径')”；它用于输出、写入结果或触发一个明确的检查步骤。
    parser.add_argument("--rpg", type=Path, required=True, help="RPG.exe 路径")
    # 这里执行“parser.add_argument('--firttech', type=Path, required=True, help='Public/Fi...”；它用于输出、写入结果或触发一个明确的检查步骤。
    parser.add_argument("--firttech", type=Path, required=True, help="Public/Firttech.enc 路径")
    # 这里执行“parser.add_argument('--api', type=Path, required=True, help='Public/Api.enc...”；它用于输出、写入结果或触发一个明确的检查步骤。
    parser.add_argument("--api", type=Path, required=True, help="Public/Api.enc 路径")
    # 这里执行“parser.add_argument('--json-out', type=Path, help='可选：写出完整JSON报告')”；它用于输出、写入结果或触发一个明确的检查步骤。
    parser.add_argument("--json-out", type=Path, help="可选：写出完整JSON报告")
    # 这里执行“parser.add_argument('--csv-out', type=Path, help='可选：写出API主资源基线与特例cost CSV')”；它用于输出、写入结果或触发一个明确的检查步骤。
    parser.add_argument("--csv-out", type=Path, help="可选：写出API主资源基线与特例cost CSV")
    # 这一步把右侧计算/读取出来的结果保存到“args”，后面的检查会继续使用这个值。
    args = parser.parse_args()

    # 第一步：只读加载三份原始证据，并马上记录原始哈希。
    # 这一步把右侧计算/读取出来的结果保存到“rpg_raw”，后面的检查会继续使用这个值。
    rpg_raw = args.rpg.read_bytes()
    # 这一步把右侧计算/读取出来的结果保存到“firttech_raw”，后面的检查会继续使用这个值。
    firttech_raw = args.firttech.read_bytes()
    # 这一步把右侧计算/读取出来的结果保存到“api_raw”，后面的检查会继续使用这个值。
    api_raw = args.api.read_bytes()

    # 第二步：ENC只在内存解码，不覆盖用户原文件。
    # 这一步把右侧计算/读取出来的结果保存到“firttech_decoded”，后面的检查会继续使用这个值。
    firttech_decoded = decode_period255(firttech_raw)
    # 这一步把右侧计算/读取出来的结果保存到“api_decoded”，后面的检查会继续使用这个值。
    api_decoded = decode_period255(api_raw)

    # 第三步：解析 0x12F 唯一术法记录；+0x34 的真实值会作为后续百分比参数。
    # 这一步把右侧计算/读取出来的结果保存到“special”，后面的检查会继续使用这个值。
    special = parse_special_firttech(firttech_decoded)
    # 这一步把右侧计算/读取出来的结果保存到“parameter”，后面的检查会继续使用这个值。
    parameter = int(special["main_parameter_0x34"])

    # 第四步：从API资产检查当前有名字角色/敌人的max范围和32位乘法安全范围。
    # 这一步把右侧计算/读取出来的结果保存到“api_summary”，后面的检查会继续使用这个值。
    api_summary = parse_api_main_resource(api_decoded, parameter)

    # 第五步：直接对 EXE 关键字节与 direct caller 集合做机器回归。
    # 这一步把右侧计算/读取出来的结果保存到“pe”，后面的检查会继续使用这个值。
    pe = PeImage(rpg_raw)
    # 这一步把右侧计算/读取出来的结果保存到“machine”，后面的检查会继续使用这个值。
    machine = build_machine_assertions(pe)

    # 第六步：把最容易人工理解的几个公式例子也放进报告。
    # 这一步把右侧计算/读取出来的结果保存到“examples”，后面的检查会继续使用这个值。
    examples = build_formula_examples(parameter)

    # 第七步：组合“这轮是否可以封存”的明确布尔检查。
    # 这一步把右侧计算/读取出来的结果保存到“checks”，后面的检查会继续使用这个值。
    checks = {
        "special_id_unique_is_0x12F": int(special["id"]) == SPECIAL_ID,
        "special_name_is_expected": special["name"] == "絕劍凶號",
        "special_main_parameter_is_20": parameter == 20,
        "special_second_cost_is_0": int(special["second_cost_0x38"]) == 0,
        "special_formula_selector_is_3": int(special["selector_0x30"]) == 3,
        "machine_assertions_all_pass": bool(machine["all_pass"]),
        "api_named_record_count_is_151": int(api_summary["named_record_count"]) == 151,
        "api_main_max_min_is_150": int(api_summary["main_max_min"]) == 150,
        "api_main_max_max_is_100000": int(api_summary["main_max_max"]) == 100000,
        "api_all_special_products_fit_signed32": bool(api_summary["all_products_fit_signed32"]),
        "example_100_of_100_becomes_80": examples[0]["final_after_min1_clamp"] == 80,
        "example_10_of_100_clamps_to_1": examples[2]["final_after_min1_clamp"] == 1,
    }

    # 这一步把右侧计算/读取出来的结果保存到“status”，后面的检查会继续使用这个值。
    status = "PASS" if all(checks.values()) else "FAIL"
    # 这一步把右侧计算/读取出来的结果保存到“report”，后面的检查会继续使用这个值。
    report = {
        "tool": "youcheng_battle_12f_resource_cost_inspector.py",
        "tool_version": TOOL_VERSION,
        "status": status,
        "inputs": {
            "rpg": {
                "path": str(args.rpg),
                "size": len(rpg_raw),
                "sha256": sha256_bytes(rpg_raw),
            },
            "firttech_enc": {
                "path": str(args.firttech),
                "size": len(firttech_raw),
                "sha256": sha256_bytes(firttech_raw),
                "decoded_sha256": sha256_bytes(firttech_decoded),
            },
            "api_enc": {
                "path": str(args.api),
                "size": len(api_raw),
                "sha256": sha256_bytes(api_raw),
                "decoded_sha256": sha256_bytes(api_decoded),
            },
        },
        "special_firttech_record": special,
        "machine_assertions": machine,
        "formula_semantics": {
            "normal_firttech_main_cost": "current_main -= FIRTTECH+0x34 (flat integer)",
            "id_0x12f_main_cost": "current_main -= trunc(max_main * FIRTTECH+0x34 / 100); if result<=0 then current_main=1",
            "id_0x12f_current_parameter": parameter,
            "id_0x12f_canonical_formula": f"current_main -= trunc(max_main * {parameter} / 100); min result = 1",
            "second_resource_rule": "仍走普通 flat +0x38 扣除；当前0x12F记录+0x38=0",
            "machine_magic": {
                "constant_hex": "0xAE147AE1",
                "signed_value": to_s32(0xAE147AE1),
                "positive_div100_reciprocal_hex": "0x51EB851F",
                "relationship": "0xAE147AE1 == -0x51EB851F (mod 2^32); 配合IMUL/high32/SAR5/sign correction形成 -trunc(x/100)",
            },
            "ai_precheck_quirk": "AI candidate gate仍比较 current_main >= raw(FIRTTECH+0x34)，没有0x12F百分比特判",
        },
        "formula_examples": examples,
        "api_canonical_main_resource": {
            key: value for key, value in api_summary.items() if key != "rows"
        },
        "checks": checks,
        "evidence_boundary": [
            "主资源/次资源是当前兼容引擎结构称呼；作者正式属性名仍UNKNOWN。",
            "0x12F=绝剑凶号、+0x34=20、+0x38=0、selector3来自当前canonical Firttech.enc唯一真实记录。",
            "20%解释来自机器整数公式，不来自UI猜测或说明文字。",
            "最低1点夹取只证明0x12F特殊分支；不能推广成所有主资源扣除都保底1。",
            "AI预检查与实际执行的差异是当前EXE静态行为；不推断作者是否故意，也不主动修正。",
            "API max范围只证明当前canonical资产乘积安全，不代表损坏/MOD/其他版本输入永远不会32位溢出。",
        ],
    }

    # 第八步：按用户参数写文件。父目录不存在时自动创建。
    # 这里检查条件“args.json_out”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
    if args.json_out:
        # 这里执行“args.json_out.parent.mkdir(parents=True, exist_ok=True)”；它用于输出、写入结果或触发一个明确的检查步骤。
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        # 这里执行“args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) +...”；它用于输出、写入结果或触发一个明确的检查步骤。
        args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    # 这里检查条件“args.csv_out”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
    if args.csv_out:
        # 这里执行“write_api_csv(args.csv_out, api_summary['rows'])”；它用于输出、写入结果或触发一个明确的检查步骤。
        write_api_csv(args.csv_out, api_summary["rows"])

    # 第九步：终端输出只保留最重要结果，详细证据都在JSON/CSV里。
    # 这里执行“print(f'[固化52] status={status}')”；它用于输出、写入结果或触发一个明确的检查步骤。
    print(f"[固化52] status={status}")
    # 这里执行“print(f"[固化52] FIRTTECH 0x12F: {special['name']} / +0x34={parameter} / +0x3...”；它用于输出、写入结果或触发一个明确的检查步骤。
    print(f"[固化52] FIRTTECH 0x12F: {special['name']} / +0x34={parameter} / +0x38={special['second_cost_0x38']}")
    # 这里执行“print(f"[固化52] machine assertions: {machine['pass_count']}/{machine['total_...”；它用于输出、写入结果或触发一个明确的检查步骤。
    print(f"[固化52] machine assertions: {machine['pass_count']}/{machine['total_count']}")
    # 这里执行“print(f"[固化52] API named max range: {api_summary['main_max_min']}..{api_sum...”；它用于输出、写入结果或触发一个明确的检查步骤。
    print(f"[固化52] API named max range: {api_summary['main_max_min']}..{api_summary['main_max_max']}")
    # 这里执行“print('[固化52] canonical rule: current -= trunc(max*20/100), result<=0 => 1')”；它用于输出、写入结果或触发一个明确的检查步骤。
    print("[固化52] canonical rule: current -= trunc(max*20/100), result<=0 => 1")

    # 当前函数到这里已经得到需要的结果；下面把结果交回调用者，让上层流程继续下一步。
    return 0 if status == "PASS" else 1


# 这里检查条件“__name__ == '__main__'”；只有条件成立才进入下面的缩进块，从而把异常情况或特殊分支分开处理。
if __name__ == "__main__":
    # 前面的结构或数值若不符合已知格式，就在这里立即报错停止，避免拿错误输入继续计算并制造假结论。
    raise SystemExit(main())
