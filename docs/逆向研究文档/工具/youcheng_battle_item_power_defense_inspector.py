#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle Ail2 +0x38 / +0x3C 物品修正字段调查器（固化54）。

这个工具只读取文件，不修改 RPG.exe，也不修改 Ail2.ENC。
它要解决的是固化53留下的一个非常具体的问题：

1. Ail2 记录 +0x38 为什么会被加进“膂力派生攻击侧量 A”？
2. Ail2 记录 +0x3C 为什么会被加进“體魄派生防御侧量 B”？
3. 作者自己的物品说明是否给这两个字段提供了稳定的业务语义？
4. 这两个字段能不能安全叫“武器攻击力 / 防具防御力”？
5. 当前 EXE 是否真的存在“攻击力 / 防御力”这样的正式 UI 字段标签？

固化54采用三种互相独立的证据：

A. 机器码证据：
   - 0x42BA00 从 RoleDefinition+0x3C（膂力）开始，三次把装备 Ail2+0x38 加进 A；
   - 0x42BAD0 从 trunc(RoleDefinition+0x40（體魄）/2) 开始，三次把装备 Ail2+0x3C 加进 B。

B. Ail2 全语料证据：
   - 当前 1..99 区间的 85 条有名武器记录，85/85 的 +0x38 都是正值；
   - 当前 101..152 区间的 51 条有名防具记录，51/51 的 +0x3C 都是正值；
   - 作者说明中“威力”与 +0x38 的高值武器反复对应；
   - 作者说明中“防禦力 / 護禦”与 +0x3C 的防具反复对应；
   - 饰品和敌方 Article 也会使用这两个字段，因此它们不是“武器专属 / 防具专属”字段。

C. EXE 文字边界：
   当前 RPG.exe 里能找到五项基础能力的作者 UI 字符串，
   但找不到“攻擊力 / 防禦力 / 威力 / 護禦 / 武器 / 裝備”这些 CP950 字符串。
   所以固化54只把 +0x38 称为“威力/攻侧修正”，把 +0x3C 称为“防禦/护侧修正”；
   这两个名称是兼容实现的中性业务名，不冒充作者数据库字段名。

【实现时最重要的边界】

- 不要把 +0x38 窄化成 weapon_attack：饰品和敌方 Article 也能有正值；
- 不要把 +0x3C 窄化成 armor_defense：饰品和敌方 Article 也能有正值；
- 不要把 A 直接改名成“攻击力”，也不要把 B 直接改名成“防御力”；
- 兼容层应原样保存两个 signed int32，并在 A/B builder 中按原版加法使用。
"""

import argparse
import csv
import hashlib
import json
import struct
from pathlib import Path
from typing import Any, Dict, List, Tuple


# -----------------------------
# 一、版本与文件格式常量
# -----------------------------

# 工具版本只用于报告，不参与任何游戏公式。
TOOL_VERSION = "0.7D-solid54.0"
# 这三个 SHA-256 用来确认“本次核对的输入和固化53是不是同一套基线”。
EXPECTED_RPG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
EXPECTED_AIL2_ENCODED_SHA256 = "a24784316c2bd23fc2683637ab860c0e54f1b4175f6db990398843bd0a13c5d8"
EXPECTED_AIL2_DECODED_SHA256 = "a85d0904e969700b996ee89134582552bfabb98af76321804af34f9896b051a8"

# Ail2 每条记录固定 926 字节，共 700 条。
AIL2_RECORD_SIZE = 926
AIL2_RECORD_COUNT = 700
# 名称、item ID、说明和本轮研究的两个字段偏移。
AIL2_NAME_OFFSET = 0x00
AIL2_NAME_SIZE = 32
AIL2_ITEM_ID_OFFSET = 0x20
AIL2_POWER_OFFSET = 0x38
AIL2_DEFENSE_OFFSET = 0x3C
AIL2_DESCRIPTION_OFFSET = 0x188
AIL2_DESCRIPTION_SIZE = 512

# 这些区间不是“凭空定义的游戏类别”。
# 它们来自当前 Ail2 数据的实际排列：1..99 主要是武器，101..152 是成套防具，
# 161..208 是饰品，500..699 是敌方/怪物 Article 记录。
WEAPON_RANGE = (1, 99)
ARMOR_RANGE = (101, 152)
ACCESSORY_RANGE = (161, 208)
ENEMY_RANGE = (500, 699)

# 这些作者原文锚点只选少量代表记录，避免“统计很好看但没有可人工复核的原文”。
POWER_TEXT_ANCHOR_IDS = [2, 6, 28, 48, 50, 65, 74, 82, 90]
DEFENSE_TEXT_ANCHOR_IDS = [102, 103, 105, 106, 117, 120, 126, 132, 133, 140, 141, 150]
# 饰品跨类别正值样本，用来证明字段不能窄化成武器/防具专属字段。
CROSS_CATEGORY_ANCHOR_IDS = [176, 180, 181, 182, 185, 187, 192, 193, 201]

# 当前 EXE 中这些词没有 CP950 命中；这条负证据用来限制“正式字段名”的强命名。
ABSENT_UI_TERMS = ["攻擊", "攻擊力", "防禦", "防禦力", "威力", "護禦", "武器", "裝備"]
# 五项核心能力是正对照：这些词在同一 EXE 里确实存在，因此“搜不到”不是编码方法错了。
PRESENT_UI_TERMS = ["膂力", "體魄", "靈力", "迅捷", "機運"]


# -----------------------------
# 二、最基础的二进制读取函数
# -----------------------------

def sha256_bytes(data: bytes) -> str:
    """计算一段字节的 SHA-256。"""
    # hashlib.sha256 只做身份核对，不会修改输入。
    return hashlib.sha256(data).hexdigest()


def u16(data: bytes, offset: int) -> int:
    """按 little-endian 读取 unsigned 16-bit。"""
    # < 表示小端，H 表示无符号 16 位整数。
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    """按 little-endian 读取 unsigned 32-bit。"""
    # PE header 里的地址和尺寸大多是无符号 DWORD，因此这里用 I。
    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    """按 little-endian 读取 signed 32-bit。"""
    # Ail2+0x38/+0x3C 会出现负值，所以必须用有符号 i32，而不是 unsigned DWORD。
    return struct.unpack_from("<i", data, offset)[0]


def decode_period255(raw: bytes) -> bytes:
    """解码 Public/*.ENC 的 period-255 XOR。"""
    # 第 i 个字节的密钥就是 i%255；XOR 再做一次即可还原。
    # 这里生成新的 bytes，不会覆盖用户的原始 ENC 文件。
    return bytes(value ^ (index % 255) for index, value in enumerate(raw))


def cp950_cstr(field: bytes) -> str:
    """把固定宽度 NUL 结尾字段按 CP950 解码。"""
    # 数据库字符串后面有很多 0x00 填充，先截到第一个 NUL。
    trimmed = field.split(b"\0", 1)[0]
    # replace 只防止单个异常字节让整个调查器崩掉；输入 SHA 仍会完整保存。
    return trimmed.decode("cp950", errors="replace").strip()


def count_bytes(haystack: bytes, needle: bytes) -> int:
    """统计 needle 在 haystack 中不重叠出现多少次。"""
    # bytes.count 正好满足本轮“固定 CP950 字符串是否存在”的需求。
    return haystack.count(needle)


# -----------------------------
# 三、最小 PE32 地址映射器
# -----------------------------

class PeImage:
    """把逆向文档中的 VA 转换成 RPG.exe 磁盘文件偏移。"""

    def __init__(self, data: bytes) -> None:
        # 保存原始 EXE 字节，所有机器断言都只读这份数据。
        self.data = data
        # 正常 Windows EXE 必须以 MZ 开头；不满足就不能继续按 PE 映射。
        if data[:2] != b"MZ":
            raise ValueError("输入不是 MZ 可执行文件")

        # DOS header +0x3C 保存 PE header 在文件中的位置。
        pe_offset = u32(data, 0x3C)
        # PE header 必须以 PE\0\0 开头。
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("输入没有有效 PE 签名")

        # COFF header 给出 section 数量和 optional header 长度。
        section_count = u16(data, pe_offset + 6)
        optional_size = u16(data, pe_offset + 20)
        optional_offset = pe_offset + 24
        # PE32 optional header +0x1C 是 ImageBase；本游戏通常是 0x00400000。
        self.image_base = u32(data, optional_offset + 0x1C)
        # section table 紧跟在 optional header 后面。
        section_table = optional_offset + optional_size
        self.sections: List[Tuple[str, int, int, int, int]] = []

        # 每个 IMAGE_SECTION_HEADER 固定 40 字节。
        for index in range(section_count):
            row = section_table + index * 40
            name = data[row:row + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            virtual_size = u32(data, row + 8)
            virtual_address = u32(data, row + 12)
            raw_size = u32(data, row + 16)
            raw_pointer = u32(data, row + 20)
            self.sections.append((name, virtual_address, virtual_size, raw_pointer, raw_size))

    def va_to_offset(self, va: int) -> int:
        """把运行时虚拟地址 VA 转换成文件 raw offset。"""
        # 先从 VA 减掉 ImageBase 得到 RVA。
        rva = va - self.image_base
        # 再判断这个 RVA 落在哪个 section 内。
        for _name, section_rva, virtual_size, raw_pointer, raw_size in self.sections:
            span = max(virtual_size, raw_size)
            if section_rva <= rva < section_rva + span:
                delta = rva - section_rva
                # 如果只存在于虚拟尾部而不在磁盘 raw data 中，就不能硬读。
                if delta >= raw_size:
                    raise ValueError(f"VA 0x{va:08X} 落在 section 虚拟尾部")
                return raw_pointer + delta
        raise ValueError(f"VA 0x{va:08X} 不属于任何 section")

    def read(self, va: int, size: int) -> bytes:
        """读取指定 VA 开始的固定长度字节。"""
        offset = self.va_to_offset(va)
        return self.data[offset:offset + size]

    def text_bytes(self) -> Tuple[int, bytes]:
        """返回 .text 的起始 VA 和原始代码字节。"""
        for name, rva, _virtual_size, raw_pointer, raw_size in self.sections:
            if name == ".text":
                return self.image_base + rva, self.data[raw_pointer:raw_pointer + raw_size]
        raise ValueError("PE 中没有 .text section")


# -----------------------------
# 四、机器码断言帮助函数
# -----------------------------

def assert_bytes(pe: PeImage, va: int, expected_hex: str, label: str, checks: List[Dict[str, Any]]) -> None:
    """比较固定 VA 的机器码是否与固化54期望一致。"""
    # expected_hex 用人类易核对的十六进制字符串保存。
    expected = bytes.fromhex(expected_hex)
    # 从 PE 中读取完全相同的长度。
    actual = pe.read(va, len(expected))
    # 只有逐字节相同才算 PASS。
    passed = actual == expected
    checks.append({
        "名称": label,
        "VA": f"0x{va:08X}",
        "期望": expected.hex(" "),
        "实际": actual.hex(" "),
        "通过": passed,
    })


def direct_e8_callers(pe: PeImage, target_va: int) -> List[int]:
    """扫描 .text 的 E8 rel32，列出 target_va 的所有直接 call 地址。"""
    text_va, text = pe.text_bytes()
    callers: List[int] = []
    # E8 后固定还有 4 字节 rel32，所以最后四字节不可能是完整 call 起点。
    for index in range(len(text) - 4):
        if text[index] != 0xE8:
            continue
        # E8 指令里的四字节不是“绝对地址”，而是“从下一条指令往前/往后跳多少字节”。
        # 所以第一步必须按有符号 32 位整数读取；如果误用 unsigned，向后跳的负偏移会变成巨大正数。
        relative = struct.unpack_from("<i", text, index + 1)[0]
        # source_va 是这条 call 指令自己的运行时地址。
        source_va = text_va + index
        # x86 的 E8 rel32 以“call 后面那条指令”为基准，因此要先加完整指令长度 5，再加 relative。
        resolved = source_va + 5 + relative
        # 只有实际解析出来的目标地址和我们要找的函数完全相等，才把这一处登记成 direct caller。
        if resolved == target_va:
            callers.append(source_va)
    return callers


def add_machine_checks(pe: PeImage, checks: List[Dict[str, Any]]) -> Dict[str, Any]:
    """核对 A/B builder 怎样消费 Ail2+0x38/+0x3C。"""
    # A 的基础 helper 先从 RoleDefinition+0x3C 读取作者已命名的“膂力”。
    assert_bytes(pe, 0x42BA0C, "8b 8a 90 00 00 00 8b 42 3c", "A基线读取膂力", checks)
    # 三件装备依次按 record+0x38 加进同一个 eax。
    assert_bytes(pe, 0x42BA2A, "03 44 7e 38", "A装备1读取Ail2+0x38", checks)
    assert_bytes(pe, 0x42BA49, "03 44 7e 38", "A装备2读取Ail2+0x38", checks)
    assert_bytes(pe, 0x42BA68, "03 44 56 38", "A装备3读取Ail2+0x38", checks)

    # B 的基础 helper 从 RoleDefinition+0x40（體魄）读取基础值。
    assert_bytes(pe, 0x42BAD6, "8b 46 40 8b 8e 90 00 00 00 99 2b c2", "B基线读取體魄并准备除2", checks)
    # sar eax,1 完成有符号除2的最后一步。
    assert_bytes(pe, 0x42BAE8, "d1 f8", "B基线signed除2", checks)
    # 三件装备依次从 record+0x3C 加进 B。
    assert_bytes(pe, 0x42BAFF, "03 44 7a 3c", "B装备1读取Ail2+0x3C", checks)
    assert_bytes(pe, 0x42BB1E, "03 44 7a 3c", "B装备2读取Ail2+0x3C", checks)
    assert_bytes(pe, 0x42BB3D, "03 44 72 3c", "B装备3读取Ail2+0x3C", checks)

    # wrapper caller 集合用来证明 A/B 不是孤立 helper，而是 D5 六条公式路径共同使用的派生量。
    # 先重新扫描当前 EXE，而不是直接相信旧文档里的 caller 列表。
    # 这样如果用户以后误把别的 RPG.exe 丢进来，即使某几条局部机器码碰巧相同，caller 集合也会暴露版本差异。
    a_callers = direct_e8_callers(pe, 0x42BA80)
    b_callers = direct_e8_callers(pe, 0x42BB50)
    # 这两组地址是固化53已经闭合的六条 D5 公式路径。
    # A 与 B 都各自被六条公式直接调用，说明它们是公共派生量，不是某一个技能的临时变量。
    expected_a = [0x42B5AA, 0x42B5FA, 0x42B63A, 0x42B6EB, 0x42B75A, 0x42B7DA]
    expected_b = [0x42B5C2, 0x42B608, 0x42B69B, 0x42B72B, 0x42B7B3, 0x42B818]
    checks.append({
        "名称": "A_wrapper_direct_callers",
        "期望": [f"0x{x:08X}" for x in expected_a],
        "实际": [f"0x{x:08X}" for x in a_callers],
        "通过": a_callers == expected_a,
    })
    checks.append({
        "名称": "B_wrapper_direct_callers",
        "期望": [f"0x{x:08X}" for x in expected_b],
        "实际": [f"0x{x:08X}" for x in b_callers],
        "通过": b_callers == expected_b,
    })

    # 返回的字典不是在“重新计算游戏伤害”，而是把刚才机器码证明出来的来源关系写成结构化摘要。
    # 后面的 JSON 报告会把这份摘要和资源语料放在一起，方便接档者一眼区分“机器数据流”和“作者文字”。
    return {
        "A": {
            "中性名称": "膂力派生攻击侧量A",
            "基础字段": "RoleDefinition+0x3C=膂力",
            "装备加成字段": "Ail2+0x38",
            "装备次数": 3,
        },
        "B": {
            "中性名称": "體魄派生防御侧量B",
            "基础字段": "trunc_signed(RoleDefinition+0x40=體魄 / 2)",
            "装备加成字段": "Ail2+0x3C",
            "装备次数": 3,
        },
    }


# -----------------------------
# 五、Ail2 全语料解析
# -----------------------------

def decode_ail2(encoded: bytes) -> bytes:
    """检查大小并解码 Ail2.ENC。"""
    # 700 条 × 每条 926 字节，就是当前台湾第三版 Ail2.ENC 应有的精确字节数。
    # 在解密前先查大小，可以避免“拿错文件但 XOR 后仍得到一堆看似可解析垃圾”的假阳性。
    expected_size = AIL2_RECORD_SIZE * AIL2_RECORD_COUNT
    if len(encoded) != expected_size:
        raise ValueError(f"Ail2.ENC 大小异常：{len(encoded)}，期望 {expected_size}")
    # 大小正确后才执行 period-255 XOR；返回的是新 bytes，原始 encoded 完全不动。
    return decode_period255(encoded)


def parse_ail2_rows(decoded: bytes) -> List[Dict[str, Any]]:
    """把700条 Ail2 记录解析成只包含本轮关心字段的列表。"""
    # rows 是最终要写进 CSV 的 700 条精简记录。
    # 我们故意不把 926 字节里所有未知字段都“顺手命名”，只抽取本轮真正有证据的字段。
    rows: List[Dict[str, Any]] = []
    # physical_index 是文件里的第几条记录；真实 item ID 另外从 +0x20 读取，两者不能想当然地视为永远相同。
    for physical_index in range(AIL2_RECORD_COUNT):
        # 固定长数据库没有每条记录的分隔符，所以第 N 条记录起点就是 N × 926。
        start = physical_index * AIL2_RECORD_SIZE
        # Python 切片生成这一条 926 字节的只读视图副本，后面所有偏移都相对于 record 起点。
        record = decoded[start:start + AIL2_RECORD_SIZE]
        # 名称和说明是作者文字证据；item ID 用来按真实ID而不是物理索引分类。
        name = cp950_cstr(record[AIL2_NAME_OFFSET:AIL2_NAME_OFFSET + AIL2_NAME_SIZE])
        item_id = u32(record, AIL2_ITEM_ID_OFFSET)
        description = cp950_cstr(record[AIL2_DESCRIPTION_OFFSET:AIL2_DESCRIPTION_OFFSET + AIL2_DESCRIPTION_SIZE])
        # 两个字段都按 signed int32 读取，因为当前武器样本里 +0x3C 确实存在负值。
        power_value = s32(record, AIL2_POWER_OFFSET)
        defense_value = s32(record, AIL2_DEFENSE_OFFSET)
        # 把人类可读文本、真实 ID 和两个 signed i32 放在同一行。
        # 这样人工打开 CSV 时可以直接比较“作者怎么描述这件物品”和“两个字段实际是多少”。
        rows.append({
            "物理记录索引": physical_index,
            "物品ID": item_id,
            "物品ID十六进制": f"0x{item_id:X}",
            "名称": name,
            "Ail2+0x38_威力攻侧修正": power_value,
            "Ail2+0x3C_防禦护侧修正": defense_value,
            "说明": description,
        })
    return rows


def named_in_range(rows: List[Dict[str, Any]], lo: int, hi: int) -> List[Dict[str, Any]]:
    """取真实 item ID 落在区间内且名称非空的记录。"""
    # 先按“真实物品ID”筛选，再要求名称非空。
    # 名称为空的保留槽不参与“作者文字”统计，否则会把空记录误当成真实装备。
    return [row for row in rows if lo <= row["物品ID"] <= hi and row["名称"]]


def find_by_id(rows: List[Dict[str, Any]], item_id: int) -> Dict[str, Any]:
    """按真实 item ID 找唯一记录。"""
    # 真实 item ID 理论上在当前表中唯一；先把所有匹配项找出来而不是直接取第一条。
    matches = [row for row in rows if row["物品ID"] == item_id]
    # 如果未来其他版本出现重复 ID，这里必须明确报错，不能悄悄选第一条制造错误证据。
    if len(matches) != 1:
        raise ValueError(f"item {item_id} 匹配 {len(matches)} 条，期望1条")
    return matches[0]


def summarize_group(rows: List[Dict[str, Any]], label: str) -> Dict[str, Any]:
    """统计一个资源组里两个字段的正值/非零分布。"""
    # 这里同时保存“正值数”和“非零数”。
    # 两者不能合并：例如武器族 +0x3C 的三个负值样本对判断 signedness 与“字段不是 padding”非常重要。
    return {
        "组": label,
        "有名记录数": len(rows),
        "+0x38正值数": sum(row["Ail2+0x38_威力攻侧修正"] > 0 for row in rows),
        "+0x38非零数": sum(row["Ail2+0x38_威力攻侧修正"] != 0 for row in rows),
        "+0x3C正值数": sum(row["Ail2+0x3C_防禦护侧修正"] > 0 for row in rows),
        "+0x3C非零数": sum(row["Ail2+0x3C_防禦护侧修正"] != 0 for row in rows),
        "+0x38最小值": min((row["Ail2+0x38_威力攻侧修正"] for row in rows), default=None),
        "+0x38最大值": max((row["Ail2+0x38_威力攻侧修正"] for row in rows), default=None),
        "+0x3C最小值": min((row["Ail2+0x3C_防禦护侧修正"] for row in rows), default=None),
        "+0x3C最大值": max((row["Ail2+0x3C_防禦护侧修正"] for row in rows), default=None),
    }


def add_corpus_checks(rows: List[Dict[str, Any]], checks: List[Dict[str, Any]]) -> Dict[str, Any]:
    """把武器、防具、饰品、敌方Article四类语料交叉成可重复断言。"""
    # 四个列表只代表“当前 canonical Ail2 数据中这些 ID 区间的实际资源族”。
    # 它们用于统计，不会被写回游戏，也不意味着兼容引擎应该把 record 类型硬编码成四种互斥类。
    weapons = named_in_range(rows, *WEAPON_RANGE)
    armors = named_in_range(rows, *ARMOR_RANGE)
    accessories = named_in_range(rows, *ACCESSORY_RANGE)
    enemies = named_in_range(rows, *ENEMY_RANGE)

    # 当前武器族：85条有名记录，+0x38 全为正值；这是“威力/攻侧”语义的第一条大样本证据。
    # 这一条检查同时验证两个条件：
    # ① 武器区间里到底有多少条“有名称的真实记录”；
    # ② 这些真实武器的 +0x38 是否每条都大于0。
    # 两个条件必须一起成立，才能把“85/85”作为稳定全语料事实写进固化文档。
    checks.append({
        "名称": "武器族_1到99_数量与0x38正值",
        "期望": {"有名记录": 85, "+0x38正值": 85},
        "实际": {"有名记录": len(weapons), "+0x38正值": sum(r["Ail2+0x38_威力攻侧修正"] > 0 for r in weapons)},
        "通过": len(weapons) == 85 and all(r["Ail2+0x38_威力攻侧修正"] > 0 for r in weapons),
    })
    # 武器族 +0x3C 只有三个负值例外，说明这个字段不是简单“武器必须为0”的padding。
    weapon_def_nonzero = [r for r in weapons if r["Ail2+0x3C_防禦护侧修正"] != 0]
    # 这条检查专门防止把 +0x3C 当成“防具才有、武器永远为0”的字段。
    # 只要武器族真实存在负值，兼容层就必须用 signed int32 保存，而且不能按类别删除该字段。
    checks.append({
        "名称": "武器族_0x3C仅3条负修正",
        "期望": {"非零数": 3, "全部负值": True},
        "实际": {"非零数": len(weapon_def_nonzero), "值": [r["Ail2+0x3C_防禦护侧修正"] for r in weapon_def_nonzero]},
        "通过": len(weapon_def_nonzero) == 3 and all(r["Ail2+0x3C_防禦护侧修正"] < 0 for r in weapon_def_nonzero),
    })

    # 当前防具族：51条有名记录，+0x3C 全为正值；这是“防禦/护侧”语义的第二条大样本证据。
    # 防具区间做与武器对称的完整样本检查。
    # 如果以后换版本后数量不再是51，工具会直接FAIL，而不是继续输出“51/51”的旧结论。
    checks.append({
        "名称": "防具族_101到152_数量与0x3C正值",
        "期望": {"有名记录": 51, "+0x3C正值": 51},
        "实际": {"有名记录": len(armors), "+0x3C正值": sum(r["Ail2+0x3C_防禦护侧修正"] > 0 for r in armors)},
        "通过": len(armors) == 51 and all(r["Ail2+0x3C_防禦护侧修正"] > 0 for r in armors),
    })
    # 同一批防具的 +0x38 全为0，进一步说明两个字段在常规装备设计中承担不同方向。
    # 当前常规防具的 +0x38 全0，是“两个字段在常规装备设计中分工明显”的证据。
    # 但这不是格式限制；饰品和敌方Article的交叉样本马上会证明 +0x38 仍是通用字段。
    checks.append({
        "名称": "防具族_0x38全部为0",
        "期望": 0,
        "实际": sum(r["Ail2+0x38_威力攻侧修正"] != 0 for r in armors),
        "通过": all(r["Ail2+0x38_威力攻侧修正"] == 0 for r in armors),
    })

    # 饰品中两个字段都存在正值，证明它们不是“只有武器/防具才能用”的类别字段。
    # 饰品是本轮特别重要的“反例族”：它们告诉我们 +0x38/+0x3C 不是武器/防具专用字段。
    # 分别数正值而不是只看有没有非零，可以直接和当前已知 4 / 8 条正样本做机器回归。
    accessory_power = sum(r["Ail2+0x38_威力攻侧修正"] > 0 for r in accessories)
    accessory_def = sum(r["Ail2+0x3C_防禦护侧修正"] > 0 for r in accessories)
    # 饰品同时打破“+0x38只能属于武器”和“+0x3C只能属于防具”两个过窄模型。
    # 因此这里要求两个正值计数都精确等于固化54语料统计，而不仅仅是“至少有一条”。
    checks.append({
        "名称": "饰品族_两个字段均存在正值",
        "期望": {"+0x38正值": 4, "+0x3C正值": 8},
        "实际": {"+0x38正值": accessory_power, "+0x3C正值": accessory_def},
        "通过": len(accessories) == 48 and accessory_power == 4 and accessory_def == 8,
    })

    # 敌方/怪物 Article 记录同时大量使用两个字段，进一步排除“武器/防具专属字段”解释。
    # 敌方Article是更强的跨类别证据：它们不是玩家装备，但战斗公式仍需要同一对字段。
    # 139/139同时为正说明兼容数据模型应该把这两个值放在通用Article record，而不是玩家装备子结构里。
    checks.append({
        "名称": "敌方Article_500到699_两个字段全正",
        "期望": {"有名记录": 139, "+0x38正值": 139, "+0x3C正值": 139},
        "实际": {
            "有名记录": len(enemies),
            "+0x38正值": sum(r["Ail2+0x38_威力攻侧修正"] > 0 for r in enemies),
            "+0x3C正值": sum(r["Ail2+0x3C_防禦护侧修正"] > 0 for r in enemies),
        },
        "通过": len(enemies) == 139 and all(
            r["Ail2+0x38_威力攻侧修正"] > 0 and r["Ail2+0x3C_防禦护侧修正"] > 0
            for r in enemies
        ),
    })

    # 作者原文“威力”锚点必须确实来自预定的武器记录，并且每条 +0x38 都是正值。
    # 统计只说明“这个字段在某一族里很常见”，还不能单独给业务含义命名。
    # 因此再逐条取作者原文锚点，要求说明中真的出现“威力”，形成文字证据。
    power_anchors = [find_by_id(rows, item_id) for item_id in POWER_TEXT_ANCHOR_IDS]
    # 每条锚点都要同时满足“说明里出现作者词『威力』”与“+0x38>0”。
    # 如果只满足文字、不满足字段，或只满足字段、不满足文字，都不能用来支持本轮业务命名。
    checks.append({
        "名称": "作者文字_威力锚点",
        "期望": f"{len(POWER_TEXT_ANCHOR_IDS)}/{len(POWER_TEXT_ANCHOR_IDS)}说明含威力且+0x38>0",
        "实际": [
            {"ID": r["物品ID"], "名称": r["名称"], "+0x38": r["Ail2+0x38_威力攻侧修正"], "含威力": "威力" in r["说明"]}
            for r in power_anchors
        ],
        "通过": all("威力" in r["说明"] and r["Ail2+0x38_威力攻侧修正"] > 0 for r in power_anchors),
    })

    # 防具作者原文同时允许“防禦”和“護禦”两种措辞；两者都是游戏资源里的作者词。
    # 防守侧同理：这里只接受作者资源中真正出现“防禦”或“護禦”的记录。
    # 不根据装备图标、价格、数值大小去猜“这大概是防御力”。
    defense_anchors = [find_by_id(rows, item_id) for item_id in DEFENSE_TEXT_ANCHOR_IDS]
    # 防守侧作者用词存在两种写法：『防禦』和『護禦』。
    # 这里接受任意一种，但仍要求 +0x3C>0，确保文字和机器字段指向同一业务方向。
    checks.append({
        "名称": "作者文字_防禦護禦锚点",
        "期望": f"{len(DEFENSE_TEXT_ANCHOR_IDS)}/{len(DEFENSE_TEXT_ANCHOR_IDS)}说明含防禦/護禦且+0x3C>0",
        "实际": [
            {"ID": r["物品ID"], "名称": r["名称"], "+0x3C": r["Ail2+0x3C_防禦护侧修正"], "含防护词": any(t in r["说明"] for t in ("防禦", "護禦"))}
            for r in defense_anchors
        ],
        "通过": all(any(t in r["说明"] for t in ("防禦", "護禦")) and r["Ail2+0x3C_防禦护侧修正"] > 0 for r in defense_anchors),
    })

    # 完成所有硬断言后，再把四个族群的完整最小值/最大值/正值数写进报告。
    # 这部分是“可调查数据”，不是新的硬编码规则；未来版本变化时可以直接比较差异。
    return {
        "武器族": summarize_group(weapons, "ID 1..99 有名记录"),
        "防具族": summarize_group(armors, "ID 101..152 有名记录"),
        "饰品族": summarize_group(accessories, "ID 161..208 有名记录"),
        "敌方Article": summarize_group(enemies, "ID 500..699 有名记录"),
        "作者威力锚点": [find_by_id(rows, i) for i in POWER_TEXT_ANCHOR_IDS],
        "作者防禦護禦锚点": [find_by_id(rows, i) for i in DEFENSE_TEXT_ANCHOR_IDS],
        "跨类别锚点": [find_by_id(rows, i) for i in CROSS_CATEGORY_ANCHOR_IDS],
    }


def add_exe_term_checks(rpg_data: bytes, checks: List[Dict[str, Any]]) -> Dict[str, Any]:
    """检查作者是否在当前 EXE 中留下这两个字段的明确 UI 名称。"""
    # 负证据部分先查“我们很想拿来当正式字段名”的候选词。
    # 每个词都按游戏使用的 CP950 编码转成原始字节，再直接扫描 EXE，而不是扫描反编译文本。
    absent_counts: Dict[str, int] = {}
    for term in ABSENT_UI_TERMS:
        # 如果命中为0，只能说明“当前 EXE 没有这个 CP950 字面字符串”，不能证明作者源码里绝对没有这个概念。
        count = count_bytes(rpg_data, term.encode("cp950"))
        absent_counts[term] = count
        checks.append({
            "名称": f"EXE作者字段名负证据_{term}",
            "期望命中数": 0,
            "实际命中数": count,
            "通过": count == 0,
        })

    # 正对照很重要：如果五项作者能力同样搜不到，就可能是我们的编码或搜索方法错了。
    # 接着做正对照：五项能力名称在同一 EXE 中应该各命中一次。
    # 只有正对照成功，前面的“0命中”才值得作为强命名限制；否则可能只是编码选错了。
    present_counts: Dict[str, int] = {}
    for term in PRESENT_UI_TERMS:
        count = count_bytes(rpg_data, term.encode("cp950"))
        present_counts[term] = count
        checks.append({
            "名称": f"EXE作者UI正对照_{term}",
            "期望命中数": 1,
            "实际命中数": count,
            "通过": count == 1,
        })

    # 返回时把负证据和正对照分开保存。
    # 这样接档者不会把“某词0命中”误读成“字符串扫描整体失效”。
    return {
        "未发现的候选正式字段词": absent_counts,
        "同EXE可找到的五项能力正对照": present_counts,
        "解释": "负证据只限制强命名：当前EXE没有这些CP950作者字段标签；不代表作者内部源码绝对没有类似概念。",
    }


# -----------------------------
# 六、CSV与作者文字证据输出
# -----------------------------

def write_csv(path: Path, rows: List[Dict[str, Any]]) -> None:
    """把700条记录写成 UTF-8-SIG CSV，便于 Excel 直接查看。"""
    # 用户可能把输出路径指向一个尚未存在的目录，因此先递归建立目录。
    path.parent.mkdir(parents=True, exist_ok=True)
    # 本工具输出的每一行字段完全相同，所以取第一行 keys 当表头即可。
    fieldnames = list(rows[0].keys())
    # UTF-8-SIG 会在文件开头写 BOM，让 Windows/Excel 更容易自动识别中文编码。
    # newline="" 是 csv 模块官方推荐方式，避免 Windows 上出现多余空行。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        # 第一行先写中文列名，随后一次性写入700条记录。
        writer.writeheader()
        writer.writerows(rows)


def write_anchor_text(path: Path, corpus: Dict[str, Any]) -> None:
    """生成一份纯文本作者文字锚点，方便不跑Python也能人工复核。"""
    # 这份 TXT 是“无需安装 Python 也能人工复核”的证据副本。
    # 所以它故意保留物品 ID、名称、两个字段数值和完整作者说明，而不是只写一句总结。
    lines: List[str] = []
    lines.append("固化54：Ail2 +0x38 / +0x3C 作者文字锚点")
    lines.append("")
    lines.append("【+0x38：威力/攻侧修正作者文字】")
    # 每个锚点先写机器字段数值，再下一行写作者说明。
    # 这样人工检查时不会因为横向一行太长而把字段和值看错。
    for row in corpus["作者威力锚点"]:
        lines.append(f"ID {row['物品ID']:>3} {row['名称']}  +0x38={row['Ail2+0x38_威力攻侧修正']}  +0x3C={row['Ail2+0x3C_防禦护侧修正']}")
        lines.append(f"  {row['说明']}")
    lines.append("")
    lines.append("【+0x3C：防禦/護禦侧修正作者文字】")
    # 防守侧也使用相同格式，确保两组证据可以直接对照。
    for row in corpus["作者防禦護禦锚点"]:
        lines.append(f"ID {row['物品ID']:>3} {row['名称']}  +0x38={row['Ail2+0x38_威力攻侧修正']}  +0x3C={row['Ail2+0x3C_防禦护侧修正']}")
        lines.append(f"  {row['说明']}")
    lines.append("")
    lines.append("【跨类别样本：证明字段不是武器/防具专属】")
    # 跨类别锚点专门证明“字段不是武器/防具专属”。
    for row in corpus["跨类别锚点"]:
        lines.append(f"ID {row['物品ID']:>3} {row['名称']}  +0x38={row['Ail2+0x38_威力攻侧修正']}  +0x3C={row['Ail2+0x3C_防禦护侧修正']}")
        lines.append(f"  {row['说明']}")
    # 最后才建立输出目录并一次性写文件，避免中途异常留下半份证据。
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


# -----------------------------
# 七、主程序
# -----------------------------

def main() -> int:
    """读取原始输入、执行全部断言、输出JSON/CSV/文字证据。"""
    # argparse 负责命令行参数检查；required=True 能避免因为漏传文件而误用默认路径。
    parser = argparse.ArgumentParser(description="固化54：Battle Ail2物品威力与防禦侧修正字段调查器")
    parser.add_argument("--rpg", required=True, type=Path, help="RPG.exe")
    parser.add_argument("--ail2", required=True, type=Path, help="multimedia/public/Ail2.ENC")
    parser.add_argument("--json-out", required=True, type=Path, help="结构化JSON输出路径")
    parser.add_argument("--csv-out", required=True, type=Path, help="700条Ail2字段CSV输出路径")
    parser.add_argument("--anchors-out", required=True, type=Path, help="作者文字锚点TXT输出路径")
    # 到这里 argparse 会自动处理 --help；真正执行调查时，五个必填路径都必须存在于命令行。
    args = parser.parse_args()

    # 一次性读入文件，后面所有步骤都只操作内存中的副本。
    # 先读 EXE；后面既要做机器码断言，也要做 CP950 作者UI字符串扫描。
    rpg_data = args.rpg.read_bytes()
    # Ail2 先保留 encoded 原始字节用于 SHA 身份核对，再另外生成 decoded 副本。
    ail2_encoded = args.ail2.read_bytes()
    ail2_decoded = decode_ail2(ail2_encoded)
    # PeImage 只建立 VA→文件偏移映射，不执行任何游戏代码。
    pe = PeImage(rpg_data)

    # checks 是整个工具的“逐条验收表”。
    # 每个检查项都保存期望、实际与通过状态；最后只有全部通过，工具才返回0。
    checks: List[Dict[str, Any]] = []
    # 先做输入身份核对，避免误把不同版本资源的统计当成当前固化证据。
    rpg_sha = sha256_bytes(rpg_data)
    ail2_encoded_sha = sha256_bytes(ail2_encoded)
    ail2_decoded_sha = sha256_bytes(ail2_decoded)
    # 三个 SHA 检查先放在最前面；这能让报告明确记录“后面的结论针对哪一份二进制和数据库”。
    checks.extend([
        {"名称": "RPG.exe SHA-256", "期望": EXPECTED_RPG_SHA256, "实际": rpg_sha, "通过": rpg_sha == EXPECTED_RPG_SHA256},
        {"名称": "Ail2.ENC encoded SHA-256", "期望": EXPECTED_AIL2_ENCODED_SHA256, "实际": ail2_encoded_sha, "通过": ail2_encoded_sha == EXPECTED_AIL2_ENCODED_SHA256},
        {"名称": "Ail2.ENC decoded SHA-256", "期望": EXPECTED_AIL2_DECODED_SHA256, "实际": ail2_decoded_sha, "通过": ail2_decoded_sha == EXPECTED_AIL2_DECODED_SHA256},
    ])

    # 机器码先证明“这两个字段确实进入 A/B”；资源统计再解释它们的作者业务语义。
    # 第一步：只看 RPG.exe，证明两个偏移确实被 A/B builder 消费。
    formula_chain = add_machine_checks(pe, checks)
    # 第二步：解析全部700条数据库记录，不能只挑“看起来支持结论”的几个样本。
    rows = parse_ail2_rows(ail2_decoded)
    # 第三步：在完整语料上做族群统计，并把作者文字锚点加入验收。
    corpus = add_corpus_checks(rows, checks)
    # 第四步：用 EXE 字符串正/负对照限制我们能不能把中性业务名冒充作者正式字段名。
    exe_terms = add_exe_term_checks(rpg_data, checks)

    # Python 的 bool 可以当 0/1 使用，所以把所有“通过=True”的项目相加即可得到通过数。
    passed = sum(1 for check in checks if check["通过"])
    total = len(checks)
    # 这里故意要求“全通过才PASS”，不使用百分比阈值；任何一个机器/资源不变量变化都应该让封包审计停下来。
    status = "PASS" if passed == total else "FAIL"

    # report 是完整结构化证据：既保存原始输入身份，也保存每条断言和最后的“能说什么/不能说什么”。
    # 这样后续接档时不需要只相信终端的一行 PASS。
    report: Dict[str, Any] = {
        "工具版本": TOOL_VERSION,
        "固化节点": 54,
        "状态": status,
        "断言": {"通过": passed, "总数": total, "明细": checks},
        "输入": {
            "RPG.exe": {"路径": str(args.rpg), "大小": len(rpg_data), "SHA256": rpg_sha},
            "Ail2.ENC": {
                "路径": str(args.ail2),
                "encoded大小": len(ail2_encoded),
                "encoded_SHA256": ail2_encoded_sha,
                "decoded_SHA256": ail2_decoded_sha,
                "记录大小": AIL2_RECORD_SIZE,
                "记录数": AIL2_RECORD_COUNT,
            },
        },
        "机器公式链": formula_chain,
        "全语料统计与作者文字": corpus,
        "EXE正式字段名边界": exe_terms,
        # 这一段是给兼容引擎实现者看的“可以安全采用的名字”。
        # 它和上面的原始机器/语料证据分开，明确表示这是研究层中性命名，不是作者源码符号。
        "固化54兼容语义": {
            "Ail2+0x38": {
                "中性实现名": "威力/攻侧修正值",
                "类型": "signed int32",
                "运行时作用": "三件装备各自值直接加到膂力派生攻击侧量A",
                "作者文字依据": "武器说明反复使用『威力』，且当前85条有名武器85/85为正值",
                "禁止过度命名": "未证实数据库/UI正式字段名；不能窄化为武器攻击力",
            },
            "Ail2+0x3C": {
                "中性实现名": "防禦/护侧修正值",
                "类型": "signed int32",
                "运行时作用": "三件装备各自值直接加到體魄派生防御侧量B",
                "作者文字依据": "防具说明反复使用『防禦力/護禦』，且当前51条有名防具51/51为正值",
                "禁止过度命名": "未证实数据库/UI正式字段名；不能窄化为防具防御力",
            },
        },
        # “结论”只写已经被机器码 + 全语料 + 作者文字共同支持的内容。
        # 不把下一轮可能研究的 +0x40/+0x44/+0x4C 顺手塞进来，避免跨固化节点污染。
        "固化54结论": [
            "Ail2+0x38 是进入膂力派生攻击侧量A的通用signed i32装备/Article修正字段；作者物品文字支持『威力/攻侧』业务语义。",
            "Ail2+0x3C 是进入體魄派生防御侧量B的通用signed i32装备/Article修正字段；作者物品文字支持『防禦/護禦侧』业务语义。",
            "两个字段都能出现在饰品和敌方Article记录中，因此不能实现成仅武器/仅防具类别字段。",
            "当前RPG.exe没有发现攻擊力/防禦力/威力/護禦等正式UI字段标签，所以兼容引擎使用中性名，不冒充作者数据库字段名。",
        ],
        # 最后显式列出 UNKNOWN，是为了防止后来的人看到“威力/防禦”就误以为正式数据库列名也已经证明。
        # 保留未知项比强行补一个好听的变量名更重要，因为兼容引擎需要可追溯证据。
        "仍保持UNKNOWN": [
            "Ail2+0x38 与 +0x3C 在原作者数据库/源码中的正式字段变量名。",
            "Ail2其他战斗修正字段（如+0x40/+0x44/+0x4C）的作者正式语义，本节点不扩大研究范围。",
            "其他版本或MOD是否会给这两个字段施加超出当前canonical语料范围的数值。",
        ],
    }

    # 输出目录不存在时自动建立；这不会碰用户的输入目录。
    # JSON 是机器可重放的正式报告；ensure_ascii=False 保留中文，便于人工审阅。
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    # CSV 保存完整700条精简语料；TXT 保存少量可人工快速阅读的作者文字锚点。
    write_csv(args.csv_out, rows)
    write_anchor_text(args.anchors_out, corpus)

    # 终端只打印最关键的摘要，详细证据全部在 JSON/CSV/TXT 中。
    print(f"固化54 Ail2威力/防禦侧字段调查：{status} ({passed}/{total})")
    print("+0x38：威力/攻侧修正；+0x3C：防禦/护侧修正（均不冒充正式数据库字段名）")
    print("武器 85/85 +0x38>0；防具 51/51 +0x3C>0；敌方Article 139/139 两字段均>0")
    # 命令行返回码遵循常见约定：0表示成功，1表示至少一条断言失败。
    # 封包审计脚本可以直接检查这个返回码来决定是否继续。
    return 0 if status == "PASS" else 1


if __name__ == "__main__":
    # 只有直接运行这个文件时才进入 main；被别的Python文件导入时不会自动执行调查。
    raise SystemExit(main())
