#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》FIRTTECH +0x44 结果生成模式与双资源恢复协议静态调查器。

本工具对应 Godot 兼容引擎研究“固化61”。它只读取完全未修改的 RPG.exe.org、
历史研究版 RPG.exe 和 canonical Firttech.enc，不会修改任何输入文件。

这次要回答的问题很具体：FIRTTECH 记录 +0x44 一共有 0/1/2/3 四个 canonical 值，
此前只知道它会在 0x42C680 的结果计算后段选择不同 helper，但还不知道四个值到底
应该怎样在兼容引擎里建模。

本工具把机器码和资源数据交叉起来，冻结以下“机器业务边界”：

* mode 0：常规直接结果公式族。命中后处理门时调用 0x42BF90，固定消耗两个 RNG draw；
  该 helper 使用加法型基础量，并使用 ``raw%19+1``、``raw%9+1`` 两个随机除数。
* mode 1：目标主生存资源 max 比例型结果公式族。命中后处理门时调用 0x42C260，
  固定两个 RNG draw；其开头直接构造 ``target_main_max * FIRTTECH+0x4C / 100``，
  随机除数则是 ``floor(raw/99)+1`` 与 ``floor(raw/19)+1``。
* mode 2：双资源恢复业务族。Battle 内走 0x42C460 生成有符号资源变化；Battle 外
  的 0x43B170 也只对 ``+0x44==2`` 进入恢复循环，直接增加主/次 current 并 clamp 到 max。
* mode 3：特殊/状态/动作协议族。Battle 内和 mode2 共享 0x42C460/特殊落地协议，
  但 Battle 外恢复函数明确排除 mode3。canonical 40 条中 39 条 +0x54/+0x58 都为 0；
  因而它不是“恢复模式”，但又不能硬说“绝对没有数值结果”，因为存在一个数值例外。

另外，本节点把 +0x54/+0x58 的已证兼容语义进一步收紧为主/次结果权重百分比：
0/1 通用 Battle 路径和 mode2 Battle 外恢复路径都分别用 +0x54/+0x58 缩放主/次通道。
但原版 Battle 的 mode2/3 次资源分支存在一个必须忠实保留的旧实现细节：
它以 ``+0x58>0`` 作为“是否计算次资源”的门，却在实际乘法里再次读取 ``+0x54``。
canonical 唯一 mode2 双通道记录“天護靈燁”恰好是 100/100，因此原数据不会暴露差异。

注意：这些名称是兼容实现的中性业务名，不是作者源码变量名。作者对 +0x44 的正式字段名
仍然 UNKNOWN；本工具不会凭四个数字自己杜撰枚举名称。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple


# ---------------------------------------------------------------------------
# 一、冻结版本与输入哈希
# ---------------------------------------------------------------------------

# 输出报告里记录工具版本，方便日后单独拿到 JSON 时判断它属于哪个固化节点。
TOOL_VERSION = "0.7D-solid61.0"

# 用户确认 RPG.exe.org 是完全没有修改过的原版，因此它是本工具的首要静态 Oracle。
EXPECTED_ORIGINAL_RPG_SHA256 = (
    "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
)

# 研究版保留下来只做双版本等价复核，不能倒过来覆盖原版行为。
EXPECTED_RESEARCH_RPG_SHA256 = (
    "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
)

# canonical Firttech.enc 的“加密文件”哈希。
EXPECTED_FIRTTECH_ENCODED_SHA256 = (
    "32b98193c037c8cd75a11534520e189c89c856c367bc1d6ac607e43d9cf52a5d"
)

# 按 period-255 XOR 解密后的哈希。两层都核对可以防止附件被换掉。
EXPECTED_FIRTTECH_DECODED_SHA256 = (
    "5469a9dd388972b53b5661e90d85c84f2d0150d06f46ddbae159409a5ac34b1a"
)

# FIRTTECH 数据库是固定 500 条、每条 0x230 字节。
FIRTTECH_RECORD_COUNT = 500
FIRTTECH_RECORD_SIZE = 0x230

# 本节点实际读取的字段。字段名使用 offset，不冒充作者正式列名。
OFF_ID = 0x00
OFF_NAME = 0x04
NAME_SIZE = 0x14
OFF_CLASS = 0x18
OFF_FORMULA_SELECTOR = 0x30
OFF_PARAMETER_K = 0x40
OFF_RESULT_MODE = 0x44
OFF_FLAT_PARAMETER = 0x4C
OFF_MAIN_WEIGHT = 0x54
OFF_SECONDARY_WEIGHT = 0x58
OFF_DESCRIPTION = 0x17C
DESCRIPTION_SIZE = FIRTTECH_RECORD_SIZE - OFF_DESCRIPTION

# canonical +0x44 分布。这是资源事实，不是人工猜测。
EXPECTED_MODE_COUNTS = {0: 106, 1: 341, 2: 13, 3: 40}

# canonical Firttech.enc 中有一个真实的“物理顺序与记录内 ID 不同”小异常：
# 第305条（物理索引0x131）内部 ID 是0x132，第306条内部 ID 是0x131。
# 两条正好互换，之后又继续顺序排列。工具必须保留这个原始事实，不能为了方便
# 把物理索引冒充成 ID，也不能把 ID 强行改写成索引。
EXPECTED_ID_INDEX_MISMATCHES = (
    (0x131, 0x132),
    (0x132, 0x131),
)

# mode2 是机器级恢复族。把 13 条 ID 冻结下来，可防止将来解析偏移错一格却仍得到“像数字”的结果。
EXPECTED_MODE2_IDS = (
    0x140, 0x147, 0x17E, 0x181, 0x185, 0x186,
    0x191, 0x192, 0x193, 0x194, 0x195, 0x196, 0x197,
)

# mode2 中唯一同时打开主/次两个结果通道的是“天護靈燁”。
EXPECTED_MODE2_DUAL_RESOURCE_ID = 0x17E

# mode3 共40条；39条没有主/次数值权重，唯一数值例外是“邪凜夢魂”。
EXPECTED_MODE3_NUMERIC_EXCEPTION_ID = 0x15D

# 为了让 CSV 顺序稳定，固定四个模式的兼容实现中性名称。
MODE_COMPAT_NAMES = {
    0: "常规直接结果公式族",
    1: "目标主生存资源max比例型结果公式族",
    2: "双资源恢复业务族",
    3: "特殊_状态_动作协议族",
}


# ---------------------------------------------------------------------------
# 二、机器码范围与调用点冻结
# ---------------------------------------------------------------------------

# 每个区间都从完全原版重新取 SHA-256。只要机器码有一字节变化，工具就拒绝沿用旧解释。
CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (
        0x0042BF90,
        0x0042C254,
        "mode0_0x42BF90常规随机结果helper",
        "99ddd41d0e576740f7f178ce8f4e90b57e421ea30497673d125963c6055c453f",
    ),
    (
        0x0042C260,
        0x0042C454,
        "mode1_0x42C260目标max比例随机结果helper",
        "94bc28527888779cb87685e04c4e01936903813034ce2b4bd9a0079028bcf9b2",
    ),
    (
        0x0042C460,
        0x0042C4CC,
        "mode2_3_0x42C460双资源有符号结果helper",
        "e55096912460d5adf5fa11e84fcb3aa5b6733dee55bf7199e78c0836db15b7db",
    ),
    (
        0x0042C4D0,
        0x0042C60E,
        "mode0_1通用附加结果helper_2_3会跳过",
        "a3a56363dbef89a3767eaa66d0aea6a9a66543844ed9c1127e4e6b100a32ebb9",
    ),
    (
        0x0042C8D7,
        0x0042CC2B,
        "FIRTTECH加0x44四模式分派与结果落地",
        "694e5cd563b9d8ddfdd220c4024afe2c26079dbb76b591873cf6b65e13a2db9b",
    ),
    (
        0x0043B170,
        0x0043B30F,
        "Battle外FIRTTECH使用与mode2资源恢复循环",
        "3ee0a874832472dc1e9822eea6a9b886acd2db609b7d62612b6e2415eea1a452",
    ),
)

# 原版和研究版在这些关键范围必须逐字节一致。
EQUIVALENCE_RANGES = tuple((start, end, label) for start, end, label, _ in CRITICAL_SLICES)

# 这些 helper 的 direct CALL 数量非常少，可以在整个 .text 中穷举。
EXPECTED_DIRECT_CALLS = {
    0x0042BF90: (0x0042C8EB,),
    0x0042C260: (0x0042C90B,),
    0x0042C460: (0x0042C930, 0x0042C973, 0x0042C9A8),
    0x0042C4D0: (0x0042C9FE,),
    0x0043B170: (0x0043C1DE, 0x0043C333),
}

# 冻结几个最关键的短指令序列，避免“整段哈希正确但我们抄错分支位置”。
EXACT_BYTES: Tuple[Tuple[int, bytes, str], ...] = (
    (
        0x0042C8D7,
        bytes.fromhex(
            "8b 47 44 85 c0 75 1b 84 c9 74 17 8b 54 24 24 57 52 53 8b cd "
            "e8 a0 f6 ff ff 89 44 24 14 e9 da 00 00 00 83 f8 01 75 1b 84 c9 "
            "74 17 8b 44 24 24 57 50 53 8b cd e8 50 f9 ff ff"
        ),
        "mode0与mode1分别调用0x42BF90和0x42C260，且都受同一后处理门控制",
    ),
    (
        0x0042C919,
        bytes.fromhex(
            "83 f8 02 75 3e 8b 47 54 85 c0 7e 2e 8b 4c 24 24 6a 01 57 51 53 "
            "8b cd e8 2b fb ff ff"
        ),
        "mode2主资源通道由加0x54大于0开门并调用0x42C460(flag=1)",
    ),
    (
        0x0042C95C,
        bytes.fromhex(
            "83 f8 03 75 72 8b 47 54 85 c0 7e 2e 8b 4c 24 24 6a 01 57 51 53 "
            "8b cd e8 e8 fa ff ff"
        ),
        "mode3与mode2共享主资源特殊结果helper",
    ),
    (
        0x0042C996,
        bytes.fromhex(
            "8b 47 58 85 c0 7e 2e 8b 4c 24 24 6a 00 57 51 53 8b cd e8 b3 fa "
            "ff ff 8b 4f 54"
        ),
        "mode2_3次资源通道由加0x58开门但乘法重新读取加0x54",
    ),
    (
        0x0042C9E9,
        bytes.fromhex(
            "8b 47 44 83 f8 02 74 16 83 f8 03 74 11 8b 4c 24 24 51 53 8b cd "
            "e8 cd fa ff ff 01 44 24 14"
        ),
        "mode2与mode3明确跳过0x42C4D0通用附加结果helper",
    ),
    (
        0x0042C28E,
        bytes.fromhex(
            "8b 4e 2c 0f af 48 4c b8 1f 85 eb 51 f7 e9 c1 fa 05 8b c2 8b cf "
            "c1 e8 1f 03 d0"
        ),
        "mode1开头构造target主生存资源max乘FIRTTECH加0x4C再除100",
    ),
    (
        0x0042C460,
        bytes.fromhex(
            "8a 44 24 10 53 56 57 84 c0 74 11 8b 7c 24 18 8b 5c 24 14 8b 57 "
            "40 0f af 53 2c eb 0f 8b 5c 24 14 8b 7c 24 18 8b 53 34 0f af 57 "
            "40"
        ),
        "0x42C460按flag在目标主max加0x2C或次max加0x34之间选择",
    ),
    (
        0x0043B1FD,
        bytes.fromhex("83 7b 44 02 0f 85 c7 00 00 00"),
        "Battle外落地只接受FIRTTECH加0x44等于2进入恢复循环",
    ),
    (
        0x0043B23E,
        bytes.fromhex(
            "8b 71 2c 8b d6 0f af 53 40 f7 ea c1 fa 05 8b c2 c1 e8 1f 03 d0 "
            "b8 1f 85 eb 51 03 d7"
        ),
        "Battle外mode2先按主max乘加0x40除100再加加0x4C形成恢复base",
    ),
    (
        0x0043B25C,
        bytes.fromhex(
            "8b 53 58 0f af d7 f7 ea c1 fa 05 8b c2 c1 e8 1f 03 d0 b8 1f 85 "
            "eb 51 8b ea 8b 53 54 0f af d7"
        ),
        "Battle外mode2分别读取加0x58与加0x54形成次和主恢复权重",
    ),
    (
        0x0043B27D,
        bytes.fromhex(
            "8b 79 30 c1 fa 05 8b c2 c1 e8 1f 03 d0 03 fa 8b c7 89 79 30 3b "
            "c6 7e 03 89 71 30 8b 79 38 8b 41 34 03 fd"
        ),
        "Battle外mode2直接增加主current加0x30并clamp到max加0x2C，再增加次current加0x38",
    ),
)


# ---------------------------------------------------------------------------
# 三、PE 读取器
# ---------------------------------------------------------------------------

@dataclass(frozen=True)
class Section:
    """PE section 的最小结构，只保存地址换算需要的五个值。"""

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass(frozen=True)
class PeImage:
    """已经读入内存的 PE 文件。"""

    path: Path
    data: bytes
    image_base: int
    sections: Tuple[Section, ...]

    def va_to_offset(self, va: int) -> int:
        """把反汇编器使用的 VA 转换成硬盘文件里的 offset。"""

        # PE section 表保存的是 RVA，所以先减 ImageBase。
        rva = va - self.image_base

        # 一个地址只能属于一个正常 section；逐段寻找即可。
        for section in self.sections:
            # VirtualSize 和 RawSize 偶尔不同，取较大值可覆盖磁盘与内存有效范围。
            span = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + span:
                # 先算出目标在 section 内部走了多少字节。
                inside = rva - section.virtual_address
                # 再加 section 在文件中的 RawOffset，得到真实硬盘偏移。
                return section.raw_offset + inside

        # 静态取证工具不允许猜地址；找不到就立即失败。
        raise ValueError(f"VA 0x{va:08X} 不属于 {self.path.name} 的任何 section")

    def read_va(self, va: int, size: int) -> bytes:
        """从指定 VA 精确读取 size 个字节。"""

        offset = self.va_to_offset(va)
        chunk = self.data[offset: offset + size]

        # 如果文件被截断，切片会悄悄变短，所以必须主动检查长度。
        if len(chunk) != size:
            raise ValueError(
                f"{self.path.name}: 0x{va:08X} 需要 {size} 字节，实际只有 {len(chunk)}"
            )
        return chunk

    def text_section(self) -> Tuple[int, bytes]:
        """返回 .text 的 VA 起点和原始字节，供 direct CALL 穷举。"""

        for section in self.sections:
            if section.name == ".text":
                base = self.image_base + section.virtual_address
                blob = self.data[section.raw_offset: section.raw_offset + section.raw_size]
                return base, blob
        raise ValueError(f"{self.path.name}: 没有找到 .text section")


def load_pe(path: Path) -> PeImage:
    """解析最小 PE 头，不依赖第三方库。"""

    # 一次读完整文件，后续所有 SHA 和切片都来自同一份不可变 bytes。
    # 初学者可以把 bytes 想成“整本书一次摊开在桌上”，后面只是在不同页码取片段。
    # 这样不会因为多次打开文件而读到不同版本，也方便所有检查共享同一份原始证据。
    data = path.read_bytes()

    # DOS 头 0x3C 保存 PE Header 的文件偏移。
    if len(data) < 0x40:
        raise ValueError(f"{path}: 文件太短，不可能是完整 PE")
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]

    # 检查 PE\0\0 魔数，避免误把别的文件当 EXE。
    if data[pe_offset: pe_offset + 4] != b"PE\0\0":
        raise ValueError(f"{path}: PE 签名不正确")

    # COFF Header 中读取 section 数与 Optional Header 长度。
    # section_count 告诉我们后面有多少个分区；optional_size 告诉我们分区表从哪里开始。
    # 这两个数如果读错，后续 VA 映射会整体错位，所以不能写死成经验值。
    section_count = struct.unpack_from("<H", data, pe_offset + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe_offset + 20)[0]

    # PE32 Optional Header 的 ImageBase 位于 +0x1C。
    # 注意：这里是“相对于 Optional Header 起点”的偏移，不是相对于 PE Header 起点。
    # 早期预审版误写成 +0x34；+0x34 实际已经落到别的 Optional Header 字段，
    # 结果会让 VA→RVA 换算整体错位。这里严格按 IMAGE_OPTIONAL_HEADER32 布局读取。
    optional_offset = pe_offset + 24
    image_base = struct.unpack_from("<I", data, optional_offset + 0x1C)[0]

    # section table 紧跟在 Optional Header 后面。
    section_table = optional_offset + optional_size
    sections: List[Section] = []

    # 每个 IMAGE_SECTION_HEADER 固定 40 字节。
    # 循环每次前进40字节，相当于逐行读取 PE 的“目录表”。
    # 我们只保存地址换算需要的字段，其它链接器信息不参与本节点判断。
    for index in range(section_count):
        entry = section_table + index * 40

        # section 名最多8字节，以 NUL 结束。
        raw_name = data[entry: entry + 8].split(b"\0", 1)[0]
        name = raw_name.decode("ascii", errors="replace")

        # +8 起依次是 VirtualSize / VirtualAddress / SizeOfRawData / PointerToRawData。
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
            "<IIII", data, entry + 8
        )
        sections.append(
            Section(name, virtual_address, virtual_size, raw_offset, raw_size)
        )

    return PeImage(path, data, image_base, tuple(sections))


# ---------------------------------------------------------------------------
# 四、通用小工具
# ---------------------------------------------------------------------------

def sha256_bytes(data: bytes) -> str:
    """返回 bytes 的小写 SHA-256 十六进制字符串。"""

    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """读取文件并计算 SHA-256。"""

    return sha256_bytes(path.read_bytes())


def decode_period255_xor(encoded: bytes) -> bytes:
    """按幽城公共数据库已经闭合的 period-255 XOR 规则解密。"""

    # 第 i 个字节与 i%255 异或；255 个字节后 key 从0重新开始。
    return bytes(value ^ (index % 255) for index, value in enumerate(encoded))


def read_i32(record: bytes, offset: int) -> int:
    """从固定记录中读取 little-endian signed int32。"""

    return struct.unpack_from("<i", record, offset)[0]


def read_u32(record: bytes, offset: int) -> int:
    """从固定记录中读取 little-endian unsigned int32。"""

    return struct.unpack_from("<I", record, offset)[0]


def decode_cp950_field(record: bytes, offset: int, size: int) -> str:
    """读取固定长度 CP950 C 字符串。"""

    # 先截出字段，再在第一个 NUL 处结束，模拟原版 C 字符串。
    raw = record[offset: offset + size].split(b"\0", 1)[0]

    # errors=replace 只用于报告可读性；canonical 文件哈希已经先严格验证。
    return raw.decode("cp950", errors="replace")


def find_direct_rel32_calls(image: PeImage, target_va: int) -> Tuple[int, ...]:
    """穷举 .text 中 opcode E8 的 rel32 direct CALL，并筛选目标地址。"""

    text_va, text = image.text_section()
    hits: List[int] = []

    # 最后4字节不足以组成 E8+rel32，所以循环到 len-5。
    # x86 的直接 CALL 是1字节 E8 加4字节有符号相对位移，一共正好5字节。
    # 这里只承认这种 direct CALL，不把间接 call eax / call [mem] 混进统计。
    for index in range(0, len(text) - 4):
        if text[index] != 0xE8:
            continue

        # rel32 是有符号 little-endian 32位偏移。
        relative = struct.unpack_from("<i", text, index + 1)[0]
        call_va = text_va + index
        destination = call_va + 5 + relative

        # 只有精确落到目标函数入口才计入。
        if destination == target_va:
            hits.append(call_va)

    return tuple(hits)


def counter_to_plain_dict(counter: Counter) -> Dict[str, int]:
    """把 Counter 转成按数字顺序稳定输出的 JSON 字典。"""

    return {str(key): counter[key] for key in sorted(counter)}


# ---------------------------------------------------------------------------
# 五、FIRTTECH 结构读取
# ---------------------------------------------------------------------------

@dataclass(frozen=True)
class FirttechRecord:
    """本节点真正关心的一条 FIRTTECH 记录。"""

    index: int
    record_id: int
    name: str
    action_class: int
    formula_selector: int
    parameter_k: int
    result_mode: int
    flat_parameter: int
    main_weight: int
    secondary_weight: int
    description: str


def parse_firttech(path: Path) -> Tuple[List[FirttechRecord], Dict[str, str]]:
    """验证哈希、解密并解析500条 canonical FIRTTECH。"""

    encoded = path.read_bytes()
    encoded_sha = sha256_bytes(encoded)

    # 先检查加密附件本身，错误就立刻停止，不能拿别版数据库做结论。
    if encoded_sha != EXPECTED_FIRTTECH_ENCODED_SHA256:
        raise ValueError(
            "Firttech.enc 加密文件 SHA-256 不匹配："
            f"expected={EXPECTED_FIRTTECH_ENCODED_SHA256}, actual={encoded_sha}"
        )

    decoded = decode_period255_xor(encoded)
    decoded_sha = sha256_bytes(decoded)

    # 再检查解密结果，能同时验证 XOR 算法没有写错。
    if decoded_sha != EXPECTED_FIRTTECH_DECODED_SHA256:
        raise ValueError(
            "Firttech.enc 解密后 SHA-256 不匹配："
            f"expected={EXPECTED_FIRTTECH_DECODED_SHA256}, actual={decoded_sha}"
        )

    expected_size = FIRTTECH_RECORD_COUNT * FIRTTECH_RECORD_SIZE
    if len(decoded) != expected_size:
        raise ValueError(
            f"FIRTTECH 解密长度应为 {expected_size}，实际 {len(decoded)}"
        )

    records: List[FirttechRecord] = []

    # 逐条按固定0x230步长切记录。
    for index in range(FIRTTECH_RECORD_COUNT):
        start = index * FIRTTECH_RECORD_SIZE
        record = decoded[start: start + FIRTTECH_RECORD_SIZE]

        # 当前 canonical 记录 ID 与数组 index 一致；把它也记录下来做完整性检查。
        record_id = read_u32(record, OFF_ID)

        records.append(
            FirttechRecord(
                index=index,
                record_id=record_id,
                name=decode_cp950_field(record, OFF_NAME, NAME_SIZE),
                action_class=read_i32(record, OFF_CLASS),
                formula_selector=read_i32(record, OFF_FORMULA_SELECTOR),
                parameter_k=read_i32(record, OFF_PARAMETER_K),
                result_mode=read_i32(record, OFF_RESULT_MODE),
                flat_parameter=read_i32(record, OFF_FLAT_PARAMETER),
                main_weight=read_i32(record, OFF_MAIN_WEIGHT),
                secondary_weight=read_i32(record, OFF_SECONDARY_WEIGHT),
                description=decode_cp950_field(
                    record, OFF_DESCRIPTION, DESCRIPTION_SIZE
                ),
            )
        )

    return records, {
        "encoded_sha256": encoded_sha,
        "decoded_sha256": decoded_sha,
    }


# ---------------------------------------------------------------------------
# 六、机器码验证
# ---------------------------------------------------------------------------

def verify_executables(original: PeImage, research: PeImage) -> Dict[str, object]:
    """验证整文件身份、关键区间哈希、短字节和 direct CALL 集合。"""

    checks: List[Dict[str, object]] = []

    # 先锁死整文件身份。
    original_sha = sha256_bytes(original.data)
    research_sha = sha256_bytes(research.data)
    if original_sha != EXPECTED_ORIGINAL_RPG_SHA256:
        raise ValueError(f"原版 RPG SHA 不匹配：{original_sha}")
    if research_sha != EXPECTED_RESEARCH_RPG_SHA256:
        raise ValueError(f"研究版 RPG SHA 不匹配：{research_sha}")

    # 每段关键机器码既检查原版哈希，也检查研究版是否逐字节相同。
    # 第一层回答“原版这段还是不是我们研究的那段”；第二层回答“研究版有没有改到这里”。
    # 两层同时通过，才允许把完全原版 Oracle 的结论用于当前研究版回归。
    for start, end, label, expected_sha in CRITICAL_SLICES:
        original_bytes = original.read_va(start, end - start)
        research_bytes = research.read_va(start, end - start)
        actual_sha = sha256_bytes(original_bytes)

        if actual_sha != expected_sha:
            raise ValueError(
                f"关键区间 {label} 哈希变化：expected={expected_sha}, actual={actual_sha}"
            )
        if original_bytes != research_bytes:
            raise ValueError(f"关键区间 {label} 在原版与研究版之间不一致")

        checks.append(
            {
                "label": label,
                "start_va": f"0x{start:08X}",
                "end_va_exclusive": f"0x{end:08X}",
                "size": end - start,
                "sha256": actual_sha,
                "original_equals_research": True,
            }
        )

    # 短字节逐条精确匹配。
    # 大区间哈希负责防整体变化，短锚点负责把关键 cmp/jump/call 与语义结论一一对上。
    # 这样以后即使有人只看 JSON，也能知道某条结论对应的是哪几条具体机器指令。
    exact_results = []
    for va, expected, meaning in EXACT_BYTES:
        actual = original.read_va(va, len(expected))
        if actual != expected:
            raise ValueError(
                f"短锚点失败：0x{va:08X} {meaning}\n"
                f"expected={expected.hex(' ')}\nactual  ={actual.hex(' ')}"
            )
        exact_results.append(
            {
                "va": f"0x{va:08X}",
                "size": len(expected),
                "meaning": meaning,
                "bytes": expected.hex(" "),
            }
        )

    # direct CALL 在完整 .text 上重新扫描，避免只看局部反汇编漏掉第二个 caller。
    call_results = {}
    for target, expected_calls in EXPECTED_DIRECT_CALLS.items():
        actual_calls = find_direct_rel32_calls(original, target)
        if actual_calls != expected_calls:
            raise ValueError(
                f"0x{target:08X} direct CALL 集合变化："
                f"expected={[hex(x) for x in expected_calls]}, "
                f"actual={[hex(x) for x in actual_calls]}"
            )
        call_results[f"0x{target:08X}"] = [f"0x{x:08X}" for x in actual_calls]

    return {
        "original_path": str(original.path.resolve()),
        "original_sha256": original_sha,
        "research_path": str(research.path.resolve()),
        "research_sha256": research_sha,
        "critical_slices": checks,
        "exact_byte_anchors": exact_results,
        "direct_calls": call_results,
    }


# ---------------------------------------------------------------------------
# 七、资源统计与业务边界验证
# ---------------------------------------------------------------------------

def analyze_records(records: Sequence[FirttechRecord]) -> Dict[str, object]:
    """统计四模式、权重、class与关键canonical实例，并执行严格断言。"""

    # 绝大多数记录的 raw ID 与物理索引一致，但 canonical 文件有一处真实的两条互换。
    # 因此这里不能使用“所有 ID 必须等于 index”这种过强断言。正确做法是同时保存两者，
    # 并把已知异常精确冻结；以后若异常集合发生变化，工具才拒绝继续沿用当前解释。
    id_mismatches = tuple(
        (r.index, r.record_id) for r in records if r.record_id != r.index
    )
    if id_mismatches != EXPECTED_ID_INDEX_MISMATCHES:
        raise ValueError(
            "FIRTTECH 物理索引/raw ID 异常集合变化："
            f"expected={EXPECTED_ID_INDEX_MISMATCHES}, actual={id_mismatches}"
        )

    mode_counts = Counter(r.result_mode for r in records)
    if dict(mode_counts) != EXPECTED_MODE_COUNTS:
        raise ValueError(
            f"+0x44 分布变化：expected={EXPECTED_MODE_COUNTS}, actual={dict(mode_counts)}"
        )

    # canonical 当前只出现0..3；若以后 MOD/异版出现其它值，必须单独研究，不能落进 default。
    # 这里故意不用“else 就当 mode3”的宽松写法，因为新增值可能代表完全不同的协议。
    # 兼容引擎的数据层应保留 raw int32，解释层只对已经证明的0、1、2、3建立规则。
    if set(mode_counts) != {0, 1, 2, 3}:
        raise ValueError(f"+0x44 出现未研究的新值：{sorted(mode_counts)}")

    # 按模式收集记录，后面做每族独立统计。
    # defaultdict(list) 的作用只是把相同 mode 的记录放到同一个篮子里，不改变原记录顺序。
    # 保留顺序很重要，因为 mode2 的13条ID集合也被当作 canonical 断言使用。
    by_mode: Dict[int, List[FirttechRecord]] = defaultdict(list)
    for record in records:
        by_mode[record.result_mode].append(record)

    # mode2 的 13 条 ID 必须完全一致。
    mode2_ids = tuple(r.record_id for r in by_mode[2])
    if mode2_ids != EXPECTED_MODE2_IDS:
        raise ValueError(
            f"mode2 ID 集变化：expected={EXPECTED_MODE2_IDS}, actual={mode2_ids}"
        )

    # mode2 的双通道实例必须仍只有天護靈燁。
    # “双通道”这里严格指 +0x54 与 +0x58 都大于0，不是按技能说明文字人工判断。
    # 这条唯一实例让我们能暴露 Battle 内“+0x58开门但乘+0x54”的历史代码细节。
    mode2_dual = [
        r for r in by_mode[2] if r.main_weight > 0 and r.secondary_weight > 0
    ]
    if [r.record_id for r in mode2_dual] != [EXPECTED_MODE2_DUAL_RESOURCE_ID]:
        raise ValueError(
            "mode2 双资源权重实例变化："
            f"{[(hex(r.record_id), r.name) for r in mode2_dual]}"
        )

    # mode3 里数值权重非零的例外目前只有邪凜夢魂。
    mode3_numeric = [
        r for r in by_mode[3] if r.main_weight != 0 or r.secondary_weight != 0
    ]
    if [r.record_id for r in mode3_numeric] != [EXPECTED_MODE3_NUMERIC_EXCEPTION_ID]:
        raise ValueError(
            "mode3 数值权重例外变化："
            f"{[(hex(r.record_id), r.name) for r in mode3_numeric]}"
        )

    # 汇总每种 mode 的 class 与(+54,+58)分布。
    # class 与 result mode 是两个独立字段，所以这里做交叉统计而不是把它们当同一个枚举。
    # 这样能防止“class2看起来像法术，所以mode2就是法术”这类业务直觉误命名。
    modes = {}
    for mode in range(4):
        group = by_mode[mode]
        class_counts = Counter(r.action_class for r in group)
        weight_counts = Counter((r.main_weight, r.secondary_weight) for r in group)

        # JSON key 不能是 tuple，所以把“主/次”拼成稳定字符串。
        weight_distribution = {
            f"{main}/{secondary}": count
            for (main, secondary), count in sorted(weight_counts.items())
        }

        named = [
            {
                "id": f"0x{r.record_id:03X}",
                "name": r.name,
                "class": r.action_class,
                "formula_selector": r.formula_selector,
                "parameter_k": r.parameter_k,
                "flat_parameter_0x4C": r.flat_parameter,
                "main_weight_0x54": r.main_weight,
                "secondary_weight_0x58": r.secondary_weight,
                "description": r.description,
            }
            for r in group
            if r.name.strip()
        ]

        modes[str(mode)] = {
            "compatibility_name": MODE_COMPAT_NAMES[mode],
            "count": len(group),
            "class_distribution": counter_to_plain_dict(class_counts),
            "weight_pair_distribution": weight_distribution,
            "named_records": named,
        }

    # 用作者文本只做“业务族支持”，不把它升级成作者枚举名。
    # 作者描述可以帮助说明 mode2 为什么属于恢复业务，但没有直接给出 +0x44 的正式列名。
    # 因此 JSON 里始终把 author_field_name / author enum 保持 UNKNOWN。
    mode2_named = [r for r in by_mode[2] if r.name.strip()]
    mode3_named = [r for r in by_mode[3] if r.name.strip()]

    return {
        "record_id_index_mismatches": [
            {
                "physical_index": index,
                "physical_index_hex": f"0x{index:03X}",
                "raw_record_id": record_id,
                "raw_record_id_hex": f"0x{record_id:03X}",
            }
            for index, record_id in id_mismatches
        ],
        "mode_counts": counter_to_plain_dict(mode_counts),
        "modes": modes,
        "mode2_recovery_family": {
            "ids": [f"0x{x:03X}" for x in mode2_ids],
            "named_count": len(mode2_named),
            "dual_resource_record": {
                "id": f"0x{mode2_dual[0].record_id:03X}",
                "name": mode2_dual[0].name,
                "weights": [mode2_dual[0].main_weight, mode2_dual[0].secondary_weight],
            },
            "machine_boundary": (
                "0x43B170 only enters its direct current+=restore and max-clamp loop when "
                "FIRTTECH+0x44==2; battle 0x42C680 also routes mode2 through 0x42C460."
            ),
            "author_name_status": "UNKNOWN",
        },
        "mode3_special_family": {
            "count": len(by_mode[3]),
            "zero_weight_count": len(by_mode[3]) - len(mode3_numeric),
            "numeric_exception": {
                "id": f"0x{mode3_numeric[0].record_id:03X}",
                "name": mode3_numeric[0].name,
                "weights": [mode3_numeric[0].main_weight, mode3_numeric[0].secondary_weight],
                "flat_parameter_0x4C": mode3_numeric[0].flat_parameter,
            },
            "named_count": len(mode3_named),
            "author_name_status": "UNKNOWN",
        },
    }


# ---------------------------------------------------------------------------
# 八、冻结兼容实现协议
# ---------------------------------------------------------------------------

def build_compatibility_protocol() -> Dict[str, object]:
    """把已经由机器断言支持的实现规则集中写成结构化协议。"""

    # 下面这份结构不是再次“计算”结果，而是把已经通过断言的机器事实整理成实现契约。
    # Godot 侧可以据此建立 switch，但仍要保存原始字段，方便未来异版或 MOD 出现新值。
    return {
        "FIRTTECH_plus_0x44": {
            "storage_type": "signed int32 raw",
            "author_field_name": "UNKNOWN",
            "canonical_values": [0, 1, 2, 3],
            "modes": {
                "0": {
                    "compatibility_name": MODE_COMPAT_NAMES[0],
                    "battle_helper": "0x42BF90",
                    "direct_rng_draws_when_gate_passes": 2,
                    "rng_transform": ["raw%19+1", "raw%9+1"],
                    "generic_aux_0x42C4D0": "used",
                    "resource_note": "canonical named entries are dominated by ordinary techniques and standard elemental/direct attacks",
                },
                "1": {
                    "compatibility_name": MODE_COMPAT_NAMES[1],
                    "battle_helper": "0x42C260",
                    "direct_rng_draws_when_gate_passes": 2,
                    "rng_transform": ["floor(raw/99)+1", "floor(raw/19)+1"],
                    "formula_anchor": "target main-resource max * FIRTTECH+0x4C / 100 appears at helper start",
                    "generic_aux_0x42C4D0": "used",
                    "resource_note": "canonical named entries are concentrated in soul/dark/death/drain-like special attacks; this is resource semantics, not an author enum name",
                },
                "2": {
                    "compatibility_name": MODE_COMPAT_NAMES[2],
                    "battle_helper": "0x42C460",
                    "direct_rng_draws_in_helper": 0,
                    "out_of_battle_0x43B170": "only this mode enters direct main/secondary current restoration loop",
                    "generic_aux_0x42C4D0": "skipped",
                    "machine_business_boundary": "dual-resource recovery family",
                },
                "3": {
                    "compatibility_name": MODE_COMPAT_NAMES[3],
                    "battle_helper": "0x42C460",
                    "direct_rng_draws_in_helper": 0,
                    "out_of_battle_0x43B170": "explicitly excluded because the gate is ==2",
                    "generic_aux_0x42C4D0": "skipped",
                    "machine_business_boundary": "special/state/action protocol family; not guaranteed to be numerically zero because canonical has one weighted exception",
                },
            },
        },
        "FIRTTECH_plus_0x54_plus_0x58": {
            "author_field_names": ["UNKNOWN", "UNKNOWN"],
            "compatibility_names": [
                "main_result_weight_percent",
                "secondary_result_weight_percent",
            ],
            "proven_paths": [
                "mode0/1 generic battle output scales DamageInfo+0x04 by +0x54 and DamageInfo+0x08 by +0x58",
                "0x43B170 mode2 out-of-battle recovery scales main current restore by +0x54 and secondary restore by +0x58",
            ],
            "legacy_battle_mode2_3_quirk": (
                "secondary channel is gated by FIRTTECH+0x58>0, but the actual multiplication at 0x42C9AD reloads FIRTTECH+0x54; preserve this machine behavior."
            ),
            "canonical_masking_case": (
                "the only mode2 record with both channels enabled is 0x17E 天護靈燁 and it has +0x54/+0x58=100/100, so the quirk is numerically hidden in canonical data."
            ),
        },
        "mode2_0x42C460_signed_restore": {
            "flag_1_base": "trunc(target_main_max * FIRTTECH+0x40 / 100) + FIRTTECH+0x4C",
            "flag_0_base": "trunc(target_secondary_max * FIRTTECH+0x40 / 100) + FIRTTECH+0x4C",
            "status_gate_note": "helper contains an inherited status/effect check and a target runtime sign inversion; this node freezes the resource-direction behavior, not the author's formal status-field names",
            "DamageInfo_application_context": "DamageInfo+0x04/+0x08 are later subtracted from target main/secondary current; therefore negative values restore resources",
        },
    }


# ---------------------------------------------------------------------------
# 九、CSV 与 JSON 输出
# ---------------------------------------------------------------------------

def write_csv(path: Path, records: Sequence[FirttechRecord]) -> None:
    """写出500条逐记录表，方便人工校对或后续工具直接读取。"""

    path.parent.mkdir(parents=True, exist_ok=True)

    # newline="" 是 Python csv 在 Windows/跨平台下避免空行的标准写法。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(
            [
                "ID",
                "名称",
                "机器class_0x18",
                "公式selector_0x30",
                "参数K_0x40",
                "结果模式_0x44",
                "兼容业务族",
                "平坦参数_0x4C",
                "主结果权重_0x54",
                "次结果权重_0x58",
                "作者说明",
            ]
        )

        for record in records:
            writer.writerow(
                [
                    f"0x{record.record_id:03X}",
                    record.name,
                    record.action_class,
                    record.formula_selector,
                    record.parameter_k,
                    record.result_mode,
                    MODE_COMPAT_NAMES[record.result_mode],
                    record.flat_parameter,
                    record.main_weight,
                    record.secondary_weight,
                    record.description,
                ]
            )


def write_json(path: Path, payload: Dict[str, object]) -> None:
    """用稳定UTF-8格式输出机器结论。"""

    path.parent.mkdir(parents=True, exist_ok=True)

    # ensure_ascii=False 让中文直接可读；indent=2 方便手工diff。
    text = json.dumps(payload, ensure_ascii=False, indent=2, sort_keys=True)
    path.write_text(text + "\n", encoding="utf-8", newline="\n")


# ---------------------------------------------------------------------------
# 十、命令行入口
# ---------------------------------------------------------------------------

def build_parser() -> argparse.ArgumentParser:
    """创建命令行参数解析器。"""

    # argparse 只负责把命令行字符串转换成明确的 Path 参数，不参与任何逆向判断。
    # 每个输入都要求显式给出，避免工具偷偷去当前目录寻找“同名文件”而误用旧版本。
    parser = argparse.ArgumentParser(
        description=(
            "静态验证《幽城幻剑录》FIRTTECH+0x44四种结果模式、mode2双资源恢复协议"
            "以及+0x54/+0x58结果权重边界。"
        )
    )
    parser.add_argument("--original-rpg", required=True, type=Path, help="完全未修改 RPG.exe.org 解压后的 EXE")
    parser.add_argument("--research-rpg", required=True, type=Path, help="历史研究版 RPG.exe")
    parser.add_argument("--firttech", required=True, type=Path, help="canonical 加密 Firttech.enc")
    parser.add_argument("--output-json", required=True, type=Path, help="机器结论 JSON 输出路径")
    parser.add_argument("--output-csv", required=True, type=Path, help="500条逐记录 CSV 输出路径")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    """执行一次完整、只读、可重放的固化61静态调查。"""

    # 1. 解析命令行。所有输入输出都由调用者显式指定，工具不猜游戏目录。
    args = build_parser().parse_args(argv)

    # 2. 读取两份 EXE。此时还没有相信任何结论，下面会先锁整文件 SHA。
    original = load_pe(args.original_rpg)
    research = load_pe(args.research_rpg)

    # 3. 验证关键机器码、双版本等价区间、短字节锚点和全部 direct CALL。
    executable_evidence = verify_executables(original, research)

    # 4. 验证 canonical Firttech.enc 哈希并解密成500条结构记录。
    records, firttech_hashes = parse_firttech(args.firttech)

    # 5. 对 +0x44 模式分布、mode2/mode3关键集合和权重进行严格资源断言。
    resource_evidence = analyze_records(records)

    # 6. 把机器码已经支持的兼容实现规则单独组织出来，避免后续从散乱ASM重新猜。
    compatibility_protocol = build_compatibility_protocol()

    # 7. CSV 是人工校对层，先写它。即便 JSON 很详细，逐500条看字段组合仍以CSV更直观。
    write_csv(args.output_csv, records)

    # 8. 组装最终 JSON。总体状态只有全部断言走到这里才可能写 PASS。
    payload: Dict[str, object] = {
        "tool": {
            "name": "youcheng_battle_firttech_result_mode_inspector.py",
            "version": TOOL_VERSION,
            "solidification": 61,
            "status": "PASS",
        },
        "inputs": {
            "original_rpg": str(args.original_rpg.resolve()),
            "research_rpg": str(args.research_rpg.resolve()),
            "firttech": str(args.firttech.resolve()),
            "firttech_hashes": firttech_hashes,
        },
        "executable_evidence": executable_evidence,
        "resource_evidence": resource_evidence,
        "compatibility_protocol": compatibility_protocol,
        "strict_boundaries": [
            "FIRTTECH+0x44 author/source enum name remains UNKNOWN.",
            "mode0/mode1 compatibility names describe proven machine formula families, not author terminology.",
            "mode2 is a proven recovery business family because 0x43B170 exclusively checks ==2 before directly increasing and clamping both resource currents.",
            "mode3 is a special/state/action protocol family in canonical resource semantics, but not an absolute zero-damage promise; 0x15D is a weighted numeric exception.",
            "FIRTTECH+0x54/+0x58 compatibility names are proven result-weight semantics on the listed paths; author database column names remain UNKNOWN.",
            "Do not repair the battle mode2/3 secondary-channel +0x58 gate / +0x54 multiply quirk unless intentionally implementing a non-original behavior option.",
        ],
    }

    # 9. 最后写 JSON。这样如果前面任何一步失败，就不会留下一个伪装成PASS的半成品。
    write_json(args.output_json, payload)

    # 10. 控制台只输出短摘要，详细证据全部在JSON/CSV。
    print("PASS: solid61 FIRTTECH result-mode protocol verified")
    print("mode counts:", dict(Counter(r.result_mode for r in records)))
    print("mode2 ids:", ", ".join(f"0x{x:03X}" for x in EXPECTED_MODE2_IDS))
    print("JSON:", args.output_json)
    print("CSV :", args.output_csv)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
