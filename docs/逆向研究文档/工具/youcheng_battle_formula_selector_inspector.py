#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle FIRTTECH 公式 selector 2..6 与 +0x40 参数调查器（固化51）。

本工具只做“静态、只读、可重复”的调查，不启动游戏，也不会修改任何输入文件。
它同时读取四份原始证据：

1. RPG.exe：确认公式分派、五个特殊公式 helper、selector5 的 signed idiv、以及 +0x40 的全局零门；
2. Firttech.enc：统计 500 条术法记录真实的 selector / +0x40 取值，并保存作者名称与说明锚点；
3. Api.enc：确认当前 canonical 角色/敌人 RoleDefinition 的 +0x3C 基线取值；
4. Ail2.ENC：确认 0x42BA00 会叠加的装备/Article +0x38 修正量当前没有负值。

固化51最重要的实现结论不是给 +0x40 强行起一个“伤害百分比”名字，而是证明：

- +0x30 是公式算法 selector；
- +0x40 是“selector 相关的原始公式参数”，不同 selector 的除数尺度并不相同；
- selector 0/1 走默认公式时，+0x40 的数值本身不参与默认 helper 的乘法，但在后级仍作为“0=主结果归零”的门；
- selector5 使用 x86 signed idiv，函数内部没有 base==0 的分支保护；
- 当前 canonical 有名称 RoleDefinition 的 +0x3C 全部为正，且 Ail2 +0x38 修正量全部非负，
  因而在当前已证正常构造链下 selector5 的 base 有严格正下界，不会触发除零；
- 这个“正常输入不为0”结论绝不能扩大成“任意损坏数据/MOD数据也安全”。

【证据边界】
工具输出的“结构类名称”只是为了让兼容引擎程序员分清数学路径，例如“缺口十分位加成型”。
除非作者资源文字已经直接支持，否则这些标签不是原版官方术语，也不能擅自改写成
“物理攻击 / 法术攻击 / 治疗 / 暴击 / 防御穿透百分比”等未经证明的业务字段名。
"""

# argparse：读取 --rpg / --firttech / --api / --ail2 / --json-out / --csv-out。
import argparse
# csv：把 selector 2..6 的真实 FIRTTECH 记录输出成接档者可以直接查看的表格。
import csv
# hashlib：计算输入 SHA-256，防止以后把另一版资源误当成本轮证据。
import hashlib
# json：输出结构化调查报告，方便后续版本自动回归。
import json
# struct：按 x86/PE 的 little-endian（小端）规则读取整数和 PE 头。
import struct
# pathlib.Path：跨平台、清晰地处理文件路径。
from pathlib import Path
# Counter：统计 selector / K 的真实取值分布。
from collections import Counter
# 类型标注帮助初学者看清每个函数“收什么、回什么”。
from typing import Dict, List, Optional, Tuple


TOOL_VERSION = "0.7D-solid51.0"

# -----------------------------
# 一、当前 Oracle RPG.exe 的固定地址
# -----------------------------
# 这些地址不是“猜出来的变量名”，只是已经由反汇编确认的函数/指令位置。
VA_FORMULA_DEFAULT = 0x0042B5F0
VA_FORMULA_2 = 0x0042B630
VA_FORMULA_3 = 0x0042B6C0
VA_FORMULA_4 = 0x0042B750
VA_FORMULA_5 = 0x0042B7D0
VA_FORMULA_6 = 0x0042B840
VA_BUILD_A_BASE = 0x0042BA00
VA_BUILD_A_WITH_STATUS = 0x0042BA80
VA_BUILD_B_BASE = 0x0042BAD0
VA_BUILD_B_WITH_STATUS = 0x0042BB50
VA_RANDOM_RANGE = 0x0042CCB0
VA_DAMAGE_INFO_BUILDER = 0x0042C680
VA_SELECTOR_DISPATCH = 0x0042C7B2
VA_SELECTOR_JUMP_TABLE = 0x0042CC98
VA_GLOBAL_K_ZERO_GATE = 0x0042C850

# FIRTTECH 的当前 canonical 格式已经在前代闭合为 500 × 0x230。
FIRTTECH_RECORD_SIZE = 0x230
FIRTTECH_RECORD_COUNT = 500
# API/RoleDefinition 当前格式为 255 × 0x350。
API_RECORD_SIZE = 0x350
API_RECORD_COUNT = 255
# Ail2 当前格式为 700 × 0x39E。
AIL2_RECORD_SIZE = 0x39E
AIL2_RECORD_COUNT = 700

# 这几个字段是本轮真正消费的字段。
FIRTTECH_SELECTOR_OFF = 0x30
FIRTTECH_MAIN_COST_OFF = 0x34
FIRTTECH_SECOND_COST_OFF = 0x38
FIRTTECH_K_OFF = 0x40
FIRTTECH_NAME_OFF = 0x04
FIRTTECH_NAME_SIZE = 0x14
FIRTTECH_DESC_OFF = 0x17C
FIRTTECH_DESC_SIZE = 0xB4
API_NAME_OFF = 0x0C
API_NAME_SIZE = 0x14
API_S_OFF = 0x3C
AIL2_A_BONUS_OFF = 0x38


# -----------------------------
# 二、最基础的整数/哈希小工具
# -----------------------------
def u16(data: bytes, offset: int) -> int:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """从 bytes 的指定位置读取一个 unsigned 16-bit little-endian 整数。"""
    # 初学者逐步说明：把 `struct.unpack_from("<H", data, offset)[0]` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """从 bytes 的指定位置读取一个 unsigned 32-bit little-endian 整数。"""
    # 初学者逐步说明：把 `struct.unpack_from("<I", data, offset)[0]` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """从 bytes 的指定位置读取一个 signed 32-bit little-endian 整数。"""
    # 初学者逐步说明：把 `struct.unpack_from("<i", data, offset)[0]` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return struct.unpack_from("<i", data, offset)[0]


def sha256_bytes(data: bytes) -> str:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """返回 bytes 的 SHA-256 十六进制字符串。"""
    # 初学者逐步说明：把 `hashlib.sha256(data).hexdigest()` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return hashlib.sha256(data).hexdigest()


def decode_period255(raw: bytes) -> bytes:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """
    解码《幽城幻剑录》这批 Public/*.ENC 使用的 period-255 XOR。

    第 i 个字节只做一件事：
        decoded[i] = raw[i] XOR (i % 255)

    XOR 是可逆的，所以同一个函数既能描述“解码”，也能描述“再次编码”。
    本工具永远只在内存里生成 decoded bytes，不覆盖磁盘原文件。
    """
    # 初学者逐步说明：把 `bytes(value ^ (index % 255) for index, value in enumerate(raw))` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return bytes(value ^ (index % 255) for index, value in enumerate(raw))


def decode_cp950_field(record: bytes, offset: int, size: int) -> str:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """
    读取固定宽度的 Big5/CP950 文本字段。

    原始数据库使用 NUL（0x00）结束字符串；因此先截到第一个 NUL，再按 CP950 解码。
    errors='replace' 的目的不是“掩盖错误”，而是保证单个坏字节不会让整个调查器崩溃；
    输入 SHA 和原始字节仍然会被保留，接档者可以继续人工核对。
    """
    # 初学者逐步说明：计算右侧表达式并保存到 `raw`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    raw = record[offset:offset + size].split(b"\0", 1)[0]
    # 初学者逐步说明：把 `raw.decode("cp950", errors="replace")` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return raw.decode("cp950", errors="replace")


# -----------------------------
# 三、最小 PE32 地址映射器
# -----------------------------
class PeImage:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """
    只读 PE32 映射器。

    逆向文档里写的是运行时 VA，例如 0x0042B7D0；
    但磁盘上的 EXE 是按 section raw offset 排列的。
    这个类只负责把 VA 换算成文件偏移，然后读取字节。
    """

    def __init__(self, data: bytes):
        # 初学者逐步说明：计算右侧表达式并保存到 `self.data`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        self.data = data
        # 初学者逐步说明：检查条件 `data[:2] != b"MZ"`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
        if data[:2] != b"MZ":
            # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
            raise ValueError("输入不是 MZ/PE 可执行文件")

        # 初学者逐步说明：计算右侧表达式并保存到 `pe_offset`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        pe_offset = u32(data, 0x3C)
        # 初学者逐步说明：检查条件 `data[pe_offset:pe_offset + 4] != b"PE\0\0"`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
            raise ValueError("找不到 PE\\0\\0 签名")

        # 初学者逐步说明：计算右侧表达式并保存到 `coff`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        coff = pe_offset + 4
        # 初学者逐步说明：计算右侧表达式并保存到 `section_count`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        section_count = u16(data, coff + 2)
        # 初学者逐步说明：计算右侧表达式并保存到 `optional_size`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        optional_size = u16(data, coff + 16)
        # 初学者逐步说明：计算右侧表达式并保存到 `optional`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        optional = coff + 20
        # 初学者逐步说明：检查条件 `u16(data, optional) != 0x10B`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
        if u16(data, optional) != 0x10B:
            # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
            raise ValueError("本调查器只支持当前游戏使用的 PE32")

        # 初学者逐步说明：计算右侧表达式并保存到 `self.image_base`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        self.image_base = u32(data, optional + 0x1C)
        # 初学者逐步说明：计算右侧表达式并保存到 `section_table`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        section_table = optional + optional_size
        # 初学者逐步说明：创建带类型提示的 `self.sections` 并保存当前结果；类型提示只帮助阅读/检查，不会改变Python运行语义。
        self.sections: List[Dict[str, int]] = []
        # 初学者逐步说明：逐项遍历 `range(section_count)`，每次把当前元素放进 `index`；这样可以对全部记录/断言逐个处理而不漏项。
        for index in range(section_count):
            # 初学者逐步说明：计算右侧表达式并保存到 `base`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
            base = section_table + index * 40
            # 初学者逐步说明：计算右侧表达式并保存到 `name`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
            name = data[base:base + 8].split(b"\0", 1)[0].decode("ascii", "replace")
            # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
            self.sections.append({
                "name": name,
                "virtual_size": u32(data, base + 8),
                "virtual_address": u32(data, base + 12),
                "raw_size": u32(data, base + 16),
                "raw_offset": u32(data, base + 20),
            })

    def va_to_offset(self, va: int) -> int:
        # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
        """把运行时 VA 转换为磁盘文件偏移。"""
        # 初学者逐步说明：计算右侧表达式并保存到 `rva`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        rva = va - self.image_base
        # 初学者逐步说明：逐项遍历 `self.sections`，每次把当前元素放进 `section`；这样可以对全部记录/断言逐个处理而不漏项。
        for section in self.sections:
            # 初学者逐步说明：计算右侧表达式并保存到 `start`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
            start = section["virtual_address"]
            # 初学者逐步说明：计算右侧表达式并保存到 `span`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
            span = max(section["virtual_size"], section["raw_size"])
            # 初学者逐步说明：检查条件 `start <= rva < start + span`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
            if start <= rva < start + span:
                # 初学者逐步说明：计算右侧表达式并保存到 `delta`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
                delta = rva - start
                # 初学者逐步说明：检查条件 `delta >= section["raw_size"]`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
                if delta >= section["raw_size"]:
                    # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
                    raise ValueError(f"VA 0x{va:08X} 位于 section 的磁盘零填充区")
                # 初学者逐步说明：把 `section["raw_offset"] + delta` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
                return section["raw_offset"] + delta
        # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
        raise ValueError(f"VA 0x{va:08X} 不属于任何 PE section")

    def read(self, va: int, size: int) -> bytes:
        # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
        """从 VA 读取固定长度 bytes；长度不足直接报错。"""
        # 初学者逐步说明：计算右侧表达式并保存到 `offset`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        offset = self.va_to_offset(va)
        # 初学者逐步说明：计算右侧表达式并保存到 `chunk`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        chunk = self.data[offset:offset + size]
        # 初学者逐步说明：检查条件 `len(chunk) != size`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
        if len(chunk) != size:
            # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
            raise ValueError(f"读取 VA 0x{va:08X} 时超出文件")
        # 初学者逐步说明：把 `chunk` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
        return chunk


# -----------------------------
# 四、机器码断言
# -----------------------------
def assert_bytes(pe: PeImage, va: int, expected: bytes, label: str) -> Dict[str, object]:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """比较某个 VA 的真实机器码与固化51预期机器码。"""
    # 初学者逐步说明：计算右侧表达式并保存到 `actual`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    actual = pe.read(va, len(expected))
    # 初学者逐步说明：把 `{ "名称": label, "VA": f"0x{va:08X}", "期望": expected.hex(" "), "实际": actual.hex(" "), "PASS": actual =` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return {
        "名称": label,
        "VA": f"0x{va:08X}",
        "期望": expected.hex(" "),
        "实际": actual.hex(" "),
        "PASS": actual == expected,
    }


def build_machine_assertions(pe: PeImage) -> List[Dict[str, object]]:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """
    构造本节点最关键的机器断言。

    每条断言都尽量选择“能直接解释数学语义”的短片段，
    而不是把整个函数几百字节全部硬编码进工具。
    这样既容易人工审阅，也能在地址漂移时快速看出究竟哪一层变化了。
    """
    # 初学者逐步说明：创建带类型提示的 `checks` 并保存当前结果；类型提示只帮助阅读/检查，不会改变Python运行语义。
    checks: List[Dict[str, object]] = []

    # 0x42C7B2：从 FIRTTECH+0x30 读 selector，减2后只接受 0..4，
    # 因此恰好把 selector 2..6 分派到五个特殊 helper；其他值走 default。
    checks.append(assert_bytes(
        pe, 0x0042C7B2,
        bytes.fromhex("8b 47 30 89 44 24 18 83 c0 fe 83 f8 04 77 6b ff 24 85 98 cc 42 00"),
        "FIRTTECH+0x30 只把 selector2..6送入五路jump table",
    ))

    # jump table 五项分别落到调用 42B630/6C0/750/7D0/840 的五段 stub。
    checks.append(assert_bytes(
        pe, VA_SELECTOR_JUMP_TABLE,
        struct.pack("<IIIII", 0x0042C7C8, 0x0042C7DC, 0x0042C7F0, 0x0042C804, 0x0042C818),
        "selector2..6 jump table五项目标",
    ))

    # selector2：明确读取 Role+2C 与 Role+30 做 max-current 差值，随后乘 K。
    checks.append(assert_bytes(
        pe, 0x0042B63F,
        bytes.fromhex("8b 4b 2c 8b 53 30 8b f0 8b 44 24 18 2b ca 8b 5b 3c 0f af 48 40"),
        "selector2读取主资源max-current缺口并乘FIRTTECH+0x40",
    ))
    # 0x66666667 + sar2 是 MSVC 对 signed /10 的常见常量除法实现。
    checks.append(assert_bytes(
        pe, 0x0042B654,
        bytes.fromhex("b8 67 66 66 66 f7 e9 c1 fa 02 8b ca b8 67 66 66 66 c1 e9 1f 03 d1 03 f2"),
        "selector2把缺口乘K后的结果按signed /10加入A",
    ))

    # selector3：Role+2C（主资源max）* K，然后用0x51EB851F路径做 signed /100。
    checks.append(assert_bytes(
        pe, 0x0042B6CF,
        bytes.fromhex("8b 4b 2c 0f af 48 40 b8 1f 85 eb 51 f7 e9 c1 fa 05 8b ca c1 e9 1f 03 d1"),
        "selector3把主资源max乘K后按signed /100",
    ))

    # selector4：A * K /10，然后才进入随机项与B扣减。
    checks.append(assert_bytes(
        pe, 0x0042B75A,
        bytes.fromhex("e8 21 03 00 00 8b 4c 24 18 8b 49 40 0f af c8 b8 67 66 66 66 f7 e9"),
        "selector4先取得A并乘K",
    ))
    # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
    checks.append(assert_bytes(
        pe, 0x0042B770,
        bytes.fromhex("8b 4b 3c c1 fa 02 8b c2 c1 e8 1f 03 d0"),
        "selector4的A*K结果按signed /10后继续",
    ))

    # selector5：先形成 base=A+随机，再调用B；随后直接 cdq/idiv esi。
    # 这里没有 test esi / je / jne 等本地除零保护。
    checks.append(assert_bytes(
        pe, 0x0042B80F,
        bytes.fromhex("8b 54 24 14 8b cf 52 03 f0 e8 33 03 00 00 99 f7 fe"),
        "selector5形成base后直接signed idiv base，无本地除零guard",
    ))
    # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
    checks.append(assert_bytes(
        pe, 0x0042B820,
        bytes.fromhex("8b 4c 24 18 5f 0f af 41 40 2b f0"),
        "selector5把整数商乘K后从base中扣除",
    ))

    # selector6：先完整调用 default formula，再把 default结果乘K/10。
    checks.append(assert_bytes(
        pe, 0x0042B840,
        bytes.fromhex("8b 44 24 08 8b 54 24 04 56 8b 74 24 10 56 50 52 e8 9b fd ff ff"),
        "selector6先调用default formula",
    ))
    # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
    checks.append(assert_bytes(
        pe, 0x0042B855,
        bytes.fromhex("8b 4e 40 5e 0f af c8 b8 67 66 66 66 f7 e9 c1 fa 02"),
        "selector6把default结果乘K并按signed /10",
    ))

    # default helper 自身从 42B5F0 到返回之间不读取 FIRTTECH+0x40；
    # 这里用函数开头与尾部调用形态做锚点。真正的“无+0x40读取”还会在结构报告中
    # 通过对函数字节扫描 8B ?? 40 等模式人工辅助，但不能只靠单一字节grep赋予语义。
    checks.append(assert_bytes(
        pe, VA_FORMULA_DEFAULT,
        bytes.fromhex("53 56 57 8b 7c 24 10 8b f1 57 e8 81 04 00 00"),
        "default formula入口取得performer并调用A builder",
    ))

    # formula helper 返回后，0x42C850 再读取 FIRTTECH+0x40；若K==0就把主结果清0。
    # 这证明K除了selector2..6的数学角色之外，还有一层全局zero gate。
    checks.append(assert_bytes(
        pe, VA_GLOBAL_K_ZERO_GATE,
        bytes.fromhex("8b 47 40 85 c0 89 4c 24 20 75 08 c7 44 24 20 00 00 00 00"),
        "FIRTTECH+0x40的全局zero gate：K==0则主结果归零",
    ))

    # A builder 的最初基线来自 performer+0x3C。
    checks.append(assert_bytes(
        pe, 0x0042BA0C,
        bytes.fromhex("8b 8a 90 00 00 00 8b 42 3c 85 c9"),
        "A builder从RoleDefinition+0x3C取基线",
    ))
    # 装备/Article ID有效时，A builder 加载 Ail2派生记录 +0x38 修正量。
    checks.append(assert_bytes(
        pe, 0x0042BA19,
        bytes.fromhex("8d 3c cd 00 00 00 00 2b f9 8d 3c b9 c1 e7 04 2b f9 03 44 7e 38"),
        "A builder按装备ID叠加Ail2派生记录+0x38",
    ))

    # 42BA80 的 +1F4 状态分支把A变为7/10；+240状态分支把A变为13/10。
    checks.append(assert_bytes(
        pe, 0x0042BA8B,
        bytes.fromhex("8b 8e f4 01 00 00 85 c9 7e 0b 8d 0c c5 00 00 00 00 2b c8"),
        "A builder状态+0x1F4分支构造7*A",
    ))
    # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
    checks.append(assert_bytes(
        pe, 0x0042BAA0,
        bytes.fromhex("8b 8e 40 02 00 00 85 c9 7e 19 8d 0c 40 8d 0c 88"),
        "A builder状态+0x240分支构造13*A",
    ))
    # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
    checks.append(assert_bytes(
        pe, 0x0042BAB0,
        bytes.fromhex("b8 67 66 66 66 f7 e9 c1 fa 02 8b c2 c1 e8 1f 03 d0 8b c2"),
        "A builder状态倍率最终按signed /10",
    ))

    # selector5随机边界直接来自原始 S=performer+0x3C：S/10 与 S/50。
    checks.append(assert_bytes(
        pe, 0x0042B7DF,
        bytes.fromhex("8b 4b 3c 8b f0 b8 67 66 66 66 f7 e9"),
        "selector5随机边界源S来自RoleDefinition+0x3C",
    ))

    # 初学者逐步说明：把 `checks` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return checks


# -----------------------------
# 五、数据库解析与统计
# -----------------------------
def parse_firttech(decoded: bytes) -> List[Dict[str, object]]:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """把500条FIRTTECH记录解析成本轮需要的最小字段集合。"""
    # 初学者逐步说明：计算右侧表达式并保存到 `expected_size`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    expected_size = FIRTTECH_RECORD_SIZE * FIRTTECH_RECORD_COUNT
    # 初学者逐步说明：检查条件 `len(decoded) != expected_size`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
    if len(decoded) != expected_size:
        # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
        raise ValueError(f"Firttech解码后尺寸应为 {expected_size}，实际 {len(decoded)}")

    # 初学者逐步说明：创建带类型提示的 `rows` 并保存当前结果；类型提示只帮助阅读/检查，不会改变Python运行语义。
    rows: List[Dict[str, object]] = []
    # 初学者逐步说明：逐项遍历 `range(FIRTTECH_RECORD_COUNT)`，每次把当前元素放进 `index`；这样可以对全部记录/断言逐个处理而不漏项。
    for index in range(FIRTTECH_RECORD_COUNT):
        # 初学者逐步说明：计算右侧表达式并保存到 `record`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        record = decoded[index * FIRTTECH_RECORD_SIZE:(index + 1) * FIRTTECH_RECORD_SIZE]
        # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
        rows.append({
            "record_index": index,
            "id": u32(record, 0x00),
            "name": decode_cp950_field(record, FIRTTECH_NAME_OFF, FIRTTECH_NAME_SIZE),
            "mode_0x18": s32(record, 0x18),
            "selector_0x30": s32(record, FIRTTECH_SELECTOR_OFF),
            "main_cost_0x34": s32(record, FIRTTECH_MAIN_COST_OFF),
            "second_cost_0x38": s32(record, FIRTTECH_SECOND_COST_OFF),
            "k_0x40": s32(record, FIRTTECH_K_OFF),
            "description": decode_cp950_field(record, FIRTTECH_DESC_OFF, FIRTTECH_DESC_SIZE),
        })
    # 初学者逐步说明：把 `rows` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return rows


def summarize_firttech(rows: List[Dict[str, object]]) -> Dict[str, object]:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """统计selector与K分布，并抽出selector2..6全部真实记录。"""
    # 初学者逐步说明：计算右侧表达式并保存到 `selector_counts`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    selector_counts = Counter(int(row["selector_0x30"]) for row in rows)
    # 初学者逐步说明：创建带类型提示的 `by_selector` 并保存当前结果；类型提示只帮助阅读/检查，不会改变Python运行语义。
    by_selector: Dict[str, object] = {}

    # 初学者逐步说明：逐项遍历 `range(0, 7)`，每次把当前元素放进 `selector`；这样可以对全部记录/断言逐个处理而不漏项。
    for selector in range(0, 7):
        # 初学者逐步说明：计算右侧表达式并保存到 `selected`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        selected = [row for row in rows if row["selector_0x30"] == selector]
        # 初学者逐步说明：计算右侧表达式并保存到 `k_values`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        k_values = [int(row["k_0x40"]) for row in selected]
        # 初学者逐步说明：计算右侧表达式并保存到 `by_selector[str(selector)]`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        by_selector[str(selector)] = {
            "record_count": len(selected),
            "k_zero_count": sum(value == 0 for value in k_values),
            "k_nonzero_count": sum(value != 0 for value in k_values),
            "k_min": min(k_values) if k_values else None,
            "k_max": max(k_values) if k_values else None,
            "k_distribution": dict(sorted(Counter(k_values).items())),
        }

    # selector2..6没有必要抽样：数量总共只有68条，所以全部保存在JSON/CSV中。
    special_rows = [row for row in rows if 2 <= int(row["selector_0x30"]) <= 6]

    # 这几个作者文本锚点不是用来“替代机器公式”，而是作为业务方向的第二证据源。
    anchor_ids = [0x130, 0x12F, 0x136, 0x139, 0x132]
    # 初学者逐步说明：计算右侧表达式并保存到 `anchors`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    anchors = []
    # 初学者逐步说明：逐项遍历 `anchor_ids`，每次把当前元素放进 `wanted_id`；这样可以对全部记录/断言逐个处理而不漏项。
    for wanted_id in anchor_ids:
        # 初学者逐步说明：计算右侧表达式并保存到 `matches`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        matches = [row for row in rows if row["id"] == wanted_id]
        # 初学者逐步说明：检查条件 `len(matches) != 1`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
        if len(matches) != 1:
            # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
            raise ValueError(f"FIRTTECH ID 0x{wanted_id:X} 预期唯一，实际 {len(matches)} 条")
        # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
        anchors.append(matches[0])

    # 初学者逐步说明：把 `{ "selector_distribution": dict(sorted(selector_counts.items())), "by_selector": by_selector, "selec` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return {
        "selector_distribution": dict(sorted(selector_counts.items())),
        "by_selector": by_selector,
        "selector_2_to_6_records": special_rows,
        "author_text_anchors": anchors,
        "selector4_k_20000_count": sum(
            row["selector_0x30"] == 4 and row["k_0x40"] == 20000 for row in rows
        ),
    }


def parse_api_selector5_invariant(decoded: bytes) -> Dict[str, object]:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """
    统计 API RoleDefinition+0x3C。

    当前数据里有名称的记录正好与 +0x3C>0 完全对应：
    151条有名称/正值，104条空记录/零值。
    这给 selector5 的 S 与 A 基线提供了一个非常直接的 canonical 数据不变量。
    """
    # 初学者逐步说明：计算右侧表达式并保存到 `expected_size`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    expected_size = API_RECORD_SIZE * API_RECORD_COUNT
    # 初学者逐步说明：检查条件 `len(decoded) != expected_size`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
    if len(decoded) != expected_size:
        # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
        raise ValueError(f"API解码后尺寸应为 {expected_size}，实际 {len(decoded)}")

    # 初学者逐步说明：计算右侧表达式并保存到 `rows`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    rows = []
    # 初学者逐步说明：逐项遍历 `range(API_RECORD_COUNT)`，每次把当前元素放进 `index`；这样可以对全部记录/断言逐个处理而不漏项。
    for index in range(API_RECORD_COUNT):
        # 初学者逐步说明：计算右侧表达式并保存到 `record`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        record = decoded[index * API_RECORD_SIZE:(index + 1) * API_RECORD_SIZE]
        # 初学者逐步说明：计算右侧表达式并保存到 `name`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        name = decode_cp950_field(record, API_NAME_OFF, API_NAME_SIZE)
        # 初学者逐步说明：计算右侧表达式并保存到 `s_value`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        s_value = s32(record, API_S_OFF)
        # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
        rows.append({"role_id": index, "name": name, "s_0x3c": s_value})

    # 初学者逐步说明：计算右侧表达式并保存到 `named`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    named = [row for row in rows if row["name"]]
    # 初学者逐步说明：计算右侧表达式并保存到 `blank`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    blank = [row for row in rows if not row["name"]]
    # 初学者逐步说明：计算右侧表达式并保存到 `mismatches`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    mismatches = [row for row in rows if bool(row["name"]) != (int(row["s_0x3c"]) > 0)]

    # 初学者逐步说明：把 `{ "record_count": len(rows), "named_record_count": len(named), "blank_record_count": len(blank), "po` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return {
        "record_count": len(rows),
        "named_record_count": len(named),
        "blank_record_count": len(blank),
        "positive_s_count": sum(int(row["s_0x3c"]) > 0 for row in rows),
        "zero_s_count": sum(int(row["s_0x3c"]) == 0 for row in rows),
        "negative_s_count": sum(int(row["s_0x3c"]) < 0 for row in rows),
        "named_s_min": min(int(row["s_0x3c"]) for row in named),
        "named_s_max": max(int(row["s_0x3c"]) for row in named),
        "name_nonempty_iff_s_positive_mismatch_count": len(mismatches),
        "smallest_named_records": sorted(named, key=lambda row: int(row["s_0x3c"]))[:12],
    }


def parse_ail2_a_bonus_invariant(decoded: bytes) -> Dict[str, object]:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """统计 A builder 实际加到A上的 Ail2+0x38 修正量。"""
    # 初学者逐步说明：计算右侧表达式并保存到 `expected_size`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    expected_size = AIL2_RECORD_SIZE * AIL2_RECORD_COUNT
    # 初学者逐步说明：检查条件 `len(decoded) != expected_size`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
    if len(decoded) != expected_size:
        # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
        raise ValueError(f"Ail2解码后尺寸应为 {expected_size}，实际 {len(decoded)}")

    # 初学者逐步说明：计算右侧表达式并保存到 `values`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    values = []
    # 初学者逐步说明：逐项遍历 `range(AIL2_RECORD_COUNT)`，每次把当前元素放进 `index`；这样可以对全部记录/断言逐个处理而不漏项。
    for index in range(AIL2_RECORD_COUNT):
        # 初学者逐步说明：计算右侧表达式并保存到 `record`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        record = decoded[index * AIL2_RECORD_SIZE:(index + 1) * AIL2_RECORD_SIZE]
        # 初学者逐步说明：把当前新结果追加到列表尾部；这样最终报告能保留每一条机器断言或每一条资源记录，而不是只保留最后一条。
        values.append(s32(record, AIL2_A_BONUS_OFF))

    # 初学者逐步说明：把 `{ "record_count": len(values), "min": min(values), "max": max(values), "negative_count": sum(value <` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return {
        "record_count": len(values),
        "min": min(values),
        "max": max(values),
        "negative_count": sum(value < 0 for value in values),
        "zero_count": sum(value == 0 for value in values),
        "positive_count": sum(value > 0 for value in values),
    }


def build_selector5_boundary(api_summary: Dict[str, object], ail2_summary: Dict[str, object]) -> Dict[str, object]:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """
    用已经由机器码证明的运算规则，计算当前 canonical 数据的安全下界。

    注意：这里只证明“当前正常构造链”的输入边界。
    如果未来MOD把 RoleDefinition+0x3C改成0/负数，原版函数本身没有本地guard，
    这就属于超出本轮 canonical 输入域的异常数据问题。
    """
    # 初学者逐步说明：计算右侧表达式并保存到 `s_min`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    s_min = int(api_summary["named_s_min"])
    # 初学者逐步说明：计算右侧表达式并保存到 `ail2_bonus_min`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    ail2_bonus_min = int(ail2_summary["min"])

    # A0 = S + 三个可能的Ail2+0x38修正。当前修正最小0，因此最坏仍是S本身。
    a0_min = s_min + 3 * min(0, ail2_bonus_min)

    # 42BA80 的最小倍率分支是 7/10；对当前正整数输入按向0截断等价于 floor。
    a_after_weak_min = int(a0_min * 7 / 10)

    # selector5 的随机下界来自 trunc(S/50)。S_min=20时为0，且S为正时不会得到负随机量。
    random_lower_min = int(s_min / 50)
    # 初学者逐步说明：计算右侧表达式并保存到 `base_min`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    base_min = a_after_weak_min + random_lower_min

    # 初学者逐步说明：把 `{ "canonical_named_role_s_min": s_min, "ail2_plus_0x38_min": ail2_bonus_min, "a_before_status_lower_` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return {
        "canonical_named_role_s_min": s_min,
        "ail2_plus_0x38_min": ail2_bonus_min,
        "a_before_status_lower_bound": a0_min,
        "a_after_0_7_status_lower_bound": a_after_weak_min,
        "random_term_lower_bound_at_s_min": random_lower_min,
        "selector5_base_lower_bound": base_min,
        "base_zero_possible_under_current_proven_inputs": base_min <= 0,
        "original_function_has_local_zero_guard": False,
        "compatibility_rule": (
            "canonical正常输入必须按原式计算；不要在正常路径插入改变结果的额外分支。"
            "若宿主引擎希望对损坏/MOD数据防崩溃，应把base<=0当作非canonical数据保护策略，"
            "并与原版语义明确分层记录。"
        ),
    }


def build_formula_semantics() -> Dict[str, object]:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """保存已经由机器码闭合的五个特殊公式及K的量纲边界。"""
    # 初学者逐步说明：把 `{ "definitions": { "A": "0x42BA80(performer) 的派生量；基线来自 RoleDefinition+0x3C，并叠加Ail2+0x38修正与临时倍率", "B"` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return {
        "definitions": {
            "A": "0x42BA80(performer) 的派生量；基线来自 RoleDefinition+0x3C，并叠加Ail2+0x38修正与临时倍率",
            "B": "0x42BB50(target) 的派生量；作者正式属性名仍不强命名",
            "S": "performer RoleDefinition+0x3C 原始值",
            "K": "FIRTTECH+0x40 原始参数；不是全局统一百分比",
            "Rand": "0x42CCB0 的 sorted half-open integer random range",
        },
        "default": "Clamp1(A + Rand[S/25,S/10) - B)（前代已闭合；本轮不重新命名业务属性）",
        "selector2": "Clamp0(A + trunc((max_main-current_main)*K/10) + Rand[S/50,S/10) - B)",
        "selector3": "Clamp0(A + trunc(max_main*K/100) + Rand[S/50,S/10) - B)",
        "selector4": "Clamp0(trunc(A*K/10) + Rand[S/50,S/10) - B)",
        "selector5": "base=A+Rand[S/50,S/10); Clamp0(base - trunc(B/base)*K)",
        "selector6": "Clamp0(trunc(default_formula*K/10))",
        "k_semantic_boundary": {
            "global": "helper返回后仍有K==0则主结果归零的zero gate",
            "selector0_or_1": "走default helper；K数值不参与default helper倍率，只参与后级zero gate",
            "selector2": "缺口项的十分位系数",
            "selector3": "max主资源项的百分位系数",
            "selector4": "A的十分位前置倍率",
            "selector5": "整数商 trunc(B/base) 的原始乘数/扣减量子；不是百分比",
            "selector6": "default结果的十分位后置倍率",
            "forbidden_simplification": "不得把FIRTTECH+0x40统一转换成一个damage_percent字段",
        },
    }


def write_csv(path: Path, rows: List[Dict[str, object]]) -> None:
    # 初学者逐步说明：执行这一条独立表达式语句；它用于触发当前步骤的明确操作，而不是计算一个被丢弃的业务数值。
    """把 selector2..6 全部真实记录写成 UTF-8-SIG CSV，方便 Windows 直接打开。"""
    # 初学者逐步说明：调用 `path.parent.mkdir` 执行这一小步操作；返回值若无需继续使用，就只保留它产生的明确副作用（例如追加、写出或打印）。
    path.parent.mkdir(parents=True, exist_ok=True)
    # 初学者逐步说明：计算右侧表达式并保存到 `fieldnames`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    fieldnames = [
        "record_index", "id_hex", "name", "selector", "k_0x40",
        "main_cost_0x34", "second_cost_0x38", "mode_0x18", "description",
    ]
    # 初学者逐步说明：进入受控资源上下文；代码块结束时Python会自动关闭文件句柄，即使中途报错也不会把句柄遗留在系统里。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        # 初学者逐步说明：计算右侧表达式并保存到 `writer`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        # 初学者逐步说明：把已经计算好的结果写入用户指定的输出文件；写出的只是调查报告，不会覆盖RPG.exe或数据库原始输入。
        writer.writeheader()
        # 初学者逐步说明：逐项遍历 `rows`，每次把当前元素放进 `row`；这样可以对全部记录/断言逐个处理而不漏项。
        for row in rows:
            # 初学者逐步说明：把已经计算好的结果写入用户指定的输出文件；写出的只是调查报告，不会覆盖RPG.exe或数据库原始输入。
            writer.writerow({
                "record_index": row["record_index"],
                "id_hex": f"0x{int(row['id']):03X}",
                "name": row["name"],
                "selector": row["selector_0x30"],
                "k_0x40": row["k_0x40"],
                "main_cost_0x34": row["main_cost_0x34"],
                "second_cost_0x38": row["second_cost_0x38"],
                "mode_0x18": row["mode_0x18"],
                "description": row["description"],
            })


def main() -> int:
    # 初学者逐步说明：计算右侧表达式并保存到 `parser`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    parser = argparse.ArgumentParser(
        description="静态核对Battle FIRTTECH selector2..6、+0x40量纲与selector5除零输入边界"
    )
    # 初学者逐步说明：向命令行解析器登记这个参数，明确告诉使用者应提供什么输入/输出路径；参数定义本身不会访问磁盘。
    parser.add_argument("--rpg", required=True, type=Path, help="RPG.exe 路径")
    # 初学者逐步说明：向命令行解析器登记这个参数，明确告诉使用者应提供什么输入/输出路径；参数定义本身不会访问磁盘。
    parser.add_argument("--firttech", required=True, type=Path, help="原始 Firttech.enc 路径")
    # 初学者逐步说明：向命令行解析器登记这个参数，明确告诉使用者应提供什么输入/输出路径；参数定义本身不会访问磁盘。
    parser.add_argument("--api", required=True, type=Path, help="原始 Api.enc 路径")
    # 初学者逐步说明：向命令行解析器登记这个参数，明确告诉使用者应提供什么输入/输出路径；参数定义本身不会访问磁盘。
    parser.add_argument("--ail2", required=True, type=Path, help="原始 Ail2.ENC 路径")
    # 初学者逐步说明：向命令行解析器登记这个参数，明确告诉使用者应提供什么输入/输出路径；参数定义本身不会访问磁盘。
    parser.add_argument("--json-out", type=Path, help="可选：写出JSON调查报告")
    # 初学者逐步说明：向命令行解析器登记这个参数，明确告诉使用者应提供什么输入/输出路径；参数定义本身不会访问磁盘。
    parser.add_argument("--csv-out", type=Path, help="可选：写出selector2..6真实记录CSV")
    # 初学者逐步说明：计算右侧表达式并保存到 `args`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    args = parser.parse_args()

    # 第一步：读原始输入。这里全部是 bytes，后续所有“解码”也只发生在内存。
    rpg_raw = args.rpg.read_bytes()
    # 初学者逐步说明：计算右侧表达式并保存到 `firttech_raw`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    firttech_raw = args.firttech.read_bytes()
    # 初学者逐步说明：计算右侧表达式并保存到 `api_raw`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    api_raw = args.api.read_bytes()
    # 初学者逐步说明：计算右侧表达式并保存到 `ail2_raw`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    ail2_raw = args.ail2.read_bytes()

    # 初学者逐步说明：计算右侧表达式并保存到 `pe`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    pe = PeImage(rpg_raw)
    # 初学者逐步说明：计算右侧表达式并保存到 `firttech_decoded`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    firttech_decoded = decode_period255(firttech_raw)
    # 初学者逐步说明：计算右侧表达式并保存到 `api_decoded`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    api_decoded = decode_period255(api_raw)
    # 初学者逐步说明：计算右侧表达式并保存到 `ail2_decoded`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    ail2_decoded = decode_period255(ail2_raw)

    # 第二步：先做机器码断言。只要RPG关键指令变化，就不能继续拿旧公式硬套。
    machine_assertions = build_machine_assertions(pe)
    # 初学者逐步说明：计算右侧表达式并保存到 `machine_pass`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    machine_pass = all(item["PASS"] for item in machine_assertions)

    # 第三步：解析真实数据库，并从资源侧约束公式参数与selector5输入域。
    firttech_rows = parse_firttech(firttech_decoded)
    # 初学者逐步说明：计算右侧表达式并保存到 `firttech_summary`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    firttech_summary = summarize_firttech(firttech_rows)
    # 初学者逐步说明：计算右侧表达式并保存到 `api_summary`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    api_summary = parse_api_selector5_invariant(api_decoded)
    # 初学者逐步说明：计算右侧表达式并保存到 `ail2_summary`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    ail2_summary = parse_ail2_a_bonus_invariant(ail2_decoded)
    # 初学者逐步说明：计算右侧表达式并保存到 `selector5_boundary`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    selector5_boundary = build_selector5_boundary(api_summary, ail2_summary)

    # 第四步：把“数据应该满足什么”写成显式布尔条件。
    # 如果后续换版本后任何一项不成立，工具会返回非0，而不是默默继续输出旧结论。
    corpus_checks = {
        "firttech_selector_distribution_is_0_to_6_only": set(
            int(key) for key in firttech_summary["selector_distribution"].keys()
        ) == set(range(7)),
        "selector2_count_is_2": firttech_summary["by_selector"]["2"]["record_count"] == 2,
        "selector3_count_is_2": firttech_summary["by_selector"]["3"]["record_count"] == 2,
        "selector4_count_is_35": firttech_summary["by_selector"]["4"]["record_count"] == 35,
        "selector5_count_is_8": firttech_summary["by_selector"]["5"]["record_count"] == 8,
        "selector6_count_is_21": firttech_summary["by_selector"]["6"]["record_count"] == 21,
        "selector2_to_6_k_all_nonzero": all(
            firttech_summary["by_selector"][str(selector)]["k_zero_count"] == 0
            for selector in range(2, 7)
        ),
        "selector4_has_five_k_20000_outliers": firttech_summary["selector4_k_20000_count"] == 5,
        "api_255_records": api_summary["record_count"] == 255,
        "api_named_151": api_summary["named_record_count"] == 151,
        "api_named_iff_s_positive": api_summary["name_nonempty_iff_s_positive_mismatch_count"] == 0,
        "api_named_s_min_is_20": api_summary["named_s_min"] == 20,
        "ail2_700_records": ail2_summary["record_count"] == 700,
        "ail2_plus_0x38_has_no_negative": ail2_summary["negative_count"] == 0,
        "selector5_canonical_base_lower_bound_positive": selector5_boundary["selector5_base_lower_bound"] > 0,
    }
    # 初学者逐步说明：计算右侧表达式并保存到 `corpus_pass`；后续步骤都会从这个已命名中间值继续处理，避免重复计算或混淆来源。
    corpus_pass = all(corpus_checks.values())

    # 第五步：组织最终JSON。这里同时保存原始输入哈希，确保任何接档者都能定位证据身份。
    report = {
        "tool": "youcheng_battle_formula_selector_inspector.py",
        "tool_version": TOOL_VERSION,
        "status": "PASS" if machine_pass and corpus_pass else "FAIL",
        "inputs": {
            "rpg": {"path": str(args.rpg), "size": len(rpg_raw), "sha256": sha256_bytes(rpg_raw)},
            "firttech_enc": {"path": str(args.firttech), "size": len(firttech_raw), "sha256": sha256_bytes(firttech_raw)},
            "firttech_decoded_sha256": sha256_bytes(firttech_decoded),
            "api_enc": {"path": str(args.api), "size": len(api_raw), "sha256": sha256_bytes(api_raw)},
            "api_decoded_sha256": sha256_bytes(api_decoded),
            "ail2_enc": {"path": str(args.ail2), "size": len(ail2_raw), "sha256": sha256_bytes(ail2_raw)},
            "ail2_decoded_sha256": sha256_bytes(ail2_decoded),
        },
        "machine_assertions": {
            "pass_count": sum(bool(item["PASS"]) for item in machine_assertions),
            "total_count": len(machine_assertions),
            "all_pass": machine_pass,
            "items": machine_assertions,
        },
        "formula_semantics": build_formula_semantics(),
        "firttech_corpus": firttech_summary,
        "api_roledefinition_invariant": api_summary,
        "ail2_a_bonus_invariant": ail2_summary,
        "selector5_division_boundary": selector5_boundary,
        "corpus_checks": corpus_checks,
        "evidence_boundary": [
            "+0x40不是全局统一百分比；不同selector必须按各自公式原样解释。",
            "selector0/1的default helper本身不按K缩放，但后级K==0会把主结果归零。",
            "selector5函数内部没有base==0保护；当前canonical已证正常构造输入给出base>=14。",
            "base>=14只覆盖当前canonical有名称RoleDefinition、当前Ail2+0x38非负修正以及已证0.7/1.3状态倍率。",
            "不得把损坏数据、恶意MOD或其他版本强行归入canonical安全输入域。",
            "结构类名称不是作者官方变量名；A/B等内部派生量继续保持业务名UNKNOWN。",
        ],
    }

    # 可选写JSON/CSV。目录不存在时主动创建，避免用户必须手工建目录。
    if args.json_out:
        # 初学者逐步说明：调用 `args.json_out.parent.mkdir` 执行这一小步操作；返回值若无需继续使用，就只保留它产生的明确副作用（例如追加、写出或打印）。
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        # 初学者逐步说明：把已经计算好的结果写入用户指定的输出文件；写出的只是调查报告，不会覆盖RPG.exe或数据库原始输入。
        args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    # 初学者逐步说明：检查条件 `args.csv_out`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
    if args.csv_out:
        # 初学者逐步说明：调用 `write_csv` 执行这一小步操作；返回值若无需继续使用，就只保留它产生的明确副作用（例如追加、写出或打印）。
        write_csv(args.csv_out, firttech_summary["selector_2_to_6_records"])

    # 终端只打印最关键结论，详细数据全部在JSON/CSV中。
    print(f"[固化51] machine assertions: {report['machine_assertions']['pass_count']}/{report['machine_assertions']['total_count']}")
    # 初学者逐步说明：把这一项关键摘要打印到终端，方便人工快速判断是否PASS；完整细节仍保存在JSON/CSV中。
    print(f"[固化51] selector distribution: {firttech_summary['selector_distribution']}")
    # 初学者逐步说明：把这一项关键摘要打印到终端，方便人工快速判断是否PASS；完整细节仍保存在JSON/CSV中。
    print(f"[固化51] named API RoleDefinition: {api_summary['named_record_count']}, S_min={api_summary['named_s_min']}")
    # 初学者逐步说明：把这一项关键摘要打印到终端，方便人工快速判断是否PASS；完整细节仍保存在JSON/CSV中。
    print(f"[固化51] Ail2+0x38 negative count: {ail2_summary['negative_count']}")
    # 初学者逐步说明：把这一项关键摘要打印到终端，方便人工快速判断是否PASS；完整细节仍保存在JSON/CSV中。
    print(f"[固化51] selector5 canonical base lower bound: {selector5_boundary['selector5_base_lower_bound']}")
    # 初学者逐步说明：把这一项关键摘要打印到终端，方便人工快速判断是否PASS；完整细节仍保存在JSON/CSV中。
    print(f"[固化51] status: {report['status']}")

    # 初学者逐步说明：把 `0 if report["status"] == "PASS" else 1` 作为本函数最终结果交还给调用者；执行到这里后本函数立即结束。
    return 0 if report["status"] == "PASS" else 1


# 初学者逐步说明：检查条件 `__name__ == "__main__"`；只有条件成立时才进入下面缩进块，从而把异常输入或不同数据分支明确分开。
if __name__ == "__main__":
    # 初学者逐步说明：当前输入已经违反本工具的前提条件，因此立即抛出异常并停止这一条路径，防止继续用错误偏移产生伪结论。
    raise SystemExit(main())
