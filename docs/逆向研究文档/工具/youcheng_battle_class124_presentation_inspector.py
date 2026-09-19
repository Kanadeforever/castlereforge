#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle FIRTTECH class 1 / 2 / 4 共享表现路径静态调查器。

这个工具服务于 Godot 兼容引擎研究“固化57”。它只读取文件，不会修改 RPG.exe、
Firttech.enc 或 MEN0018.SF2。工具要闭合的是一条非常具体的原版行为链：

1. canonical FIRTTECH 中哪些 class 会进入共享表现门；
2. class 1 / 2 / 4 在动作开始时怎样把战斗地板组件切到非零状态；
3. 同一组三类怎样把 FIRTTECH+0x04 的招式名称交给 0x4472C0；
4. 0x4472C0 所属的 0x447110 组件怎样加载 MEN0018.SF2、复制文字、计算水平位置；
5. 该组件怎样执行“进入 10 步 -> 保持 0x27 legacy tick -> 退出 10 步”；
6. class 1 / 2 / 4 在后段怎样把战斗地板组件恢复到 state 0；
7. class 0 / 5 / 6 为什么不进入这组门，以及 canonical 资源为什么没有 class 3。

代码注释故意写得很细。阅读者不需要懂 PE、汇编或二进制格式；每一个关键步骤都会说明
“读了什么”“为什么要读”“失败代表什么”。这样以后即使原作者不在场，也可以只拿这个
固化包继续复核，而不需要重新猜测本轮结论来自哪里。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple


# 工具版本只用于报告溯源，不参与游戏行为判断。
TOOL_VERSION = "0.7D-solid57.0"

# 用户已确认这个 SHA-256 对应完全没有修改过的 RPG.exe。
# 所以它是本项目当前最重要的原版静态 Oracle。
EXPECTED_ORIGINAL_RPG_SHA256 = (
    "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
)

# 这是历史研究版 RPG.exe 的 SHA-256。
# 它不是“原版行为”的来源，只用于确认历史补丁有没有碰到本轮关键区间。
EXPECTED_RESEARCH_RPG_SHA256 = (
    "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
)

# canonical Firttech.enc 的加密文件和解密明文哈希。
# 同时校验两层，可以防止拿错资源版本后仍然“看起来能解析”。
EXPECTED_FIRTTECH_ENCODED_SHA256 = (
    "32b98193c037c8cd75a11534520e189c89c856c367bc1d6ac607e43d9cf52a5d"
)
EXPECTED_FIRTTECH_DECODED_SHA256 = (
    "5469a9dd388972b53b5661e90d85c84f2d0150d06f46ddbae159409a5ac34b1a"
)

# 从 canonical MenusDir.DAT 提取出来的 MEN0018.SF2 哈希。
# EXE 的 0x447110 构造器直接引用字符串“Menus\\MEN0018.SF2”。
EXPECTED_MEN0018_SHA256 = (
    "2a6f885dea4c5c579bd5360e4864fb6769e13c8c4a9fd8bf2b0f192ffec2c4b6"
)
EXPECTED_MEN0018_SIZE = 21049

# FIRTTECH 数据库的固定布局。
FIRTTECH_RECORD_COUNT = 500
FIRTTECH_RECORD_SIZE = 0x230
FIRT_OFF_ID = 0x00
FIRT_OFF_NAME = 0x04
FIRT_NAME_SIZE = 0x14
FIRT_OFF_CLASS = 0x18

# canonical 500 条记录的 class 统计。
# 固化55、56已经独立得到过这个分布；固化57再次重放，防止资源输入被换掉。
EXPECTED_CLASS_COUNTS = {0: 330, 1: 94, 2: 67, 4: 7, 5: 1, 6: 1}

# 本轮机器码明确把这三个 class 放进同一组表现门。
EXPECTED_PRESENTATION_CLASSES = (1, 2, 4)

# 0x4472C0 在原版 .text 中只有一个直接 CALL 调用点。
# 这一点非常重要：它把“某个通用字符串函数”的可能性大幅排除，
# 因为原版机器码的直接调用面就是当前 Battle FIRTTECH 路径。
EXPECTED_4472C0_DIRECT_CALLS = (0x0041FCA5,)

# 这些是固化57最重要的原版代码片段。
# 每个条目保存起始 VA、结束 VA（不包含 end）、人类说明、原版 SHA-256。
# 工具会重新从 EXE 取字节并计算 SHA；任何一个字节变化都会让 PASS 失败。
CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (
        0x0041FBE0,
        0x0041FCAA,
        "class1/2/4动作开始门、战斗地板state3与招式名提交",
        "60f45663badf968cb848e1e6b0ae08f44e5a69e20dc2c881ce31ec5687934800",
    ),
    (
        0x00420048,
        0x00420113,
        "class1/2/4后段表现门、可选效果请求与战斗地板state0恢复",
        "a1eafc050c30d169bd9f6aec618355b1474d39706996eabf51d8497156649691",
    ),
    (
        0x00432518,
        0x00432582,
        "0x8C40C8战斗地板组件创建、初始化与作者错误文字",
        "8fcfd8f3b929dd83ae669023e95c03330f7e6cb26c989432ea7278e1e0fcb762",
    ),
    (
        0x00434200,
        0x004342A2,
        "战斗地板四子对象state0/非零切换器",
        "8a74376d66b53677b4c1d9767fb12a295f1fb75ebf5838fb9ec2a2cfffdb903f",
    ),
    (
        0x00441D72,
        0x00441DA6,
        "Battle对象内0x447110组件分配并写入全局0x8E2418",
        "5d007c22ff289be2f2335091477bf4b02384bc55a2b131e8310b73900e78a3c2",
    ),
    (
        0x00447110,
        0x00447244,
        "招式名临时显示组件构造、MEN0018与文字对象创建",
        "17aa2884ffe7f626c824884087055c218a8716d0ca6c88fd81a2645e75234358",
    ),
    (
        0x004472C0,
        0x004473E5,
        "招式名复制、水平定位与进入保持退出状态机",
        "dfc516d609895b78839dfc8e19c4bf5d5420fabdca7668e03af2528d6bcde558",
    ),
)

# 下面这些较大范围用来比较“完全原版”和“历史研究版”。
# 只有所有区间逐字节一样，才能写“历史补丁不影响固化57结论”。
EQUIVALENCE_RANGES: Tuple[Tuple[int, int, str], ...] = (
    (0x0041FBE0, 0x00420163, "Battle动作开始/后段表现范围"),
    (0x00432518, 0x004342A2, "战斗地板组件创建与状态切换范围"),
    (0x00441D72, 0x00441DA6, "Battle招式名组件创建范围"),
    (0x00447110, 0x004473E5, "招式名临时显示组件完整范围"),
)

# 这些原版作者/资源字符串直接帮助我们给对象定业务边界。
EXPECTED_EXE_STRINGS = {
    0x0046ABCC: "戰鬥地板啟動失敗!",
    0x0046AB68: "Flr0020.sf2",
    0x0046CA60: r"Menus\MEN0018.SF2",
}

# 0x447110 对象的虚表地址。
# 第 6 个零基槽（偏移 +0x18）必须是 0x447300，说明主更新会推进本组件状态机。
BANNER_VTABLE_VA = 0x00461144
BANNER_UPDATE_SLOT_INDEX = 6
EXPECTED_BANNER_UPDATE_VA = 0x00447300


@dataclass(frozen=True)
class Section:
    """PE 文件中的一个 section 映射条目。"""

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass(frozen=True)
class PeImage:
    """把 EXE 字节、ImageBase 与 section 表放在一起，方便安全按 VA 读取。"""

    path: Path
    data: bytes
    image_base: int
    sections: Tuple[Section, ...]

    def va_to_file_offset(self, va: int) -> int:
        """把游戏运行时 VA 换算成 EXE 文件里的实际偏移。"""

        # Windows 把 PE 装入内存时会加上 ImageBase。
        # 所以先减掉 ImageBase，得到相对映像开头的 RVA。
        rva = va - self.image_base

        # RVA 不能直接当硬盘文件偏移，因为 PE 的各个 section 在文件里有独立布局。
        # 这里逐个查找 RVA 落在哪一个 section。
        for section in self.sections:
            # VirtualSize 与 RawSize 有时不同，取较大值可以覆盖合法映射区间。
            span = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + span:
                # inside 是“从这个 section 开头走了多少字节”。
                inside = rva - section.virtual_address
                # 再把 inside 加到 section 在硬盘上的起始位置，就得到文件偏移。
                return section.raw_offset + inside

        # 地址不在任何 section 中时绝不能继续猜，否则后面所有机器码结论都会错位。
        raise ValueError(f"VA 0x{va:08X} 不属于 {self.path.name} 的任何 PE section")

    def read_va(self, va: int, size: int) -> bytes:
        """从指定 VA 读取固定数量字节，只读不写。"""

        # 先完成 VA -> 文件偏移转换。
        start = self.va_to_file_offset(va)
        # Python 切片右端不包含，所以 end=start+size 正好得到 size 字节。
        end = start + size
        # 越过文件末尾通常意味着地址、section 表或 size 写错，应立即停止。
        if end > len(self.data):
            raise ValueError(f"读取 0x{va:08X}+0x{size:X} 会越过 {self.path.name} 末尾")
        return self.data[start:end]

    def read_u32_va(self, va: int) -> int:
        """从 VA 读取一个小端 32 位无符号整数。"""

        # x86 PE 使用 little-endian，所以格式字符使用 <I。
        return struct.unpack("<I", self.read_va(va, 4))[0]

    def read_cstring_va(self, va: int, encoding: str = "cp950") -> str:
        """从 VA 开始读取一个以 0 结尾的 C 字符串。"""

        # 先定位第一个字节在文件中的位置。
        start = self.va_to_file_offset(va)
        # C 字符串以 0x00 结尾，因此寻找第一个 NUL。
        end = self.data.find(b"\x00", start)
        if end < 0:
            raise ValueError(f"0x{va:08X} 后找不到字符串终止 0")
        # 游戏台湾版资源/作者文字以 Big5/CP950 为主要编码。
        return self.data[start:end].decode(encoding)

    def text_section(self) -> Section:
        """返回 .text section，直接 CALL 扫描只能在代码区进行。"""

        for section in self.sections:
            if section.name == ".text":
                return section
        raise ValueError(f"{self.path.name} 没有 .text section")


@dataclass(frozen=True)
class FirttechRecord:
    """固化57实际需要的最小 FIRTTECH 记录视图。"""

    index: int
    record_id: int
    name: str
    action_class: int


def sha256_bytes(data: bytes) -> str:
    """计算内存字节的 SHA-256 十六进制字符串。"""

    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """计算文件 SHA-256；文件只以二进制读取模式打开。"""

    # 当前样本最大也不大，直接 read_bytes 可保持实现简单且不改变文件。
    return sha256_bytes(path.read_bytes())


def read_u16(data: bytes, offset: int) -> int:
    """从 bytes 的指定偏移读取 little-endian uint16。"""

    return struct.unpack_from("<H", data, offset)[0]


def read_u32(data: bytes, offset: int) -> int:
    """从 bytes 的指定偏移读取 little-endian uint32。"""

    return struct.unpack_from("<I", data, offset)[0]


def parse_pe(path: Path) -> PeImage:
    """只解析本轮需要的 PE32 头与 section 表。"""

    # 先把文件完整读入内存。后续所有读取都在这个不可变 bytes 上进行。
    data = path.read_bytes()

    # DOS 头至少要能容纳 e_lfanew；太短直接拒绝。
    if len(data) < 0x40:
        raise ValueError(f"{path} 太短，不可能是合法 PE")

    # PE 文件开头必须是 MZ。
    if data[:2] != b"MZ":
        raise ValueError(f"{path} 缺少 MZ 标记")

    # DOS+0x3C 的 DWORD 指向 PE 签名。
    pe_offset = read_u32(data, 0x3C)
    if pe_offset + 24 > len(data):
        raise ValueError(f"{path} 的 PE 头偏移越界")

    # PE 签名必须是“PE\0\0”。
    if data[pe_offset : pe_offset + 4] != b"PE\x00\x00":
        raise ValueError(f"{path} 缺少 PE\\0\\0 签名")

    # COFF 头 +2 是 section 数量。
    section_count = read_u16(data, pe_offset + 6)
    # COFF 头 +16 是 OptionalHeader 大小。
    optional_size = read_u16(data, pe_offset + 20)
    # PE32 OptionalHeader +28 是 ImageBase。
    optional_offset = pe_offset + 24
    image_base = read_u32(data, optional_offset + 28)

    # section 表紧跟在 OptionalHeader 后。
    section_table = optional_offset + optional_size
    sections: List[Section] = []

    # 每个 IMAGE_SECTION_HEADER 固定 40 字节。
    for index in range(section_count):
        entry = section_table + index * 40
        if entry + 40 > len(data):
            raise ValueError(f"{path} 的 section 表在第 {index} 项越界")

        # 前 8 字节是 ASCII section 名称，右侧可能补 0。
        name = data[entry : entry + 8].split(b"\x00", 1)[0].decode("ascii", errors="replace")
        # +8 VirtualSize，+12 VirtualAddress，+16 RawSize，+20 RawOffset。
        virtual_size = read_u32(data, entry + 8)
        virtual_address = read_u32(data, entry + 12)
        raw_size = read_u32(data, entry + 16)
        raw_offset = read_u32(data, entry + 20)
        sections.append(
            Section(
                name=name,
                virtual_address=virtual_address,
                virtual_size=virtual_size,
                raw_offset=raw_offset,
                raw_size=raw_size,
            )
        )

    # 返回一个不可变 PeImage，后面所有地址检查都复用它。
    return PeImage(path=path, data=data, image_base=image_base, sections=tuple(sections))


def decrypt_enc(encoded: bytes) -> bytes:
    """按已确认的公共 ENC 规则解密：byte[i] XOR (i % 255)。"""

    # bytearray 允许我们逐字节写解密结果。
    decoded = bytearray(len(encoded))
    # enumerate 同时给出索引和原字节。
    for index, value in enumerate(encoded):
        # 固化前序已确认，密钥就是全文件绝对索引对 255 取模。
        decoded[index] = value ^ (index % 255)
    # 转回不可变 bytes，防止后面不小心继续修改。
    return bytes(decoded)


def decode_cp950_cstring(raw: bytes) -> str:
    """从固定宽度字段中读取第一个 NUL 前的 CP950 文本。"""

    # split(...,1)[0] 只保留第一个 0 之前的实际字符串数据。
    useful = raw.split(b"\x00", 1)[0]
    # strict 解码能让坏数据立即暴露，不用替换字符掩盖输入错误。
    return useful.decode("cp950", errors="strict")


def parse_firttech(path: Path) -> Tuple[bytes, List[FirttechRecord]]:
    """解密并解析 canonical Firttech.enc 的 ID、名称和 class。"""

    # 先读取加密文件。
    encoded = path.read_bytes()
    # 500 条 * 0x230 是严格尺寸；不符合就不是当前 canonical 表。
    expected_size = FIRTTECH_RECORD_COUNT * FIRTTECH_RECORD_SIZE
    if len(encoded) != expected_size:
        raise ValueError(
            f"Firttech.enc 大小 {len(encoded)}，预期 {expected_size}；拒绝继续"
        )

    # 使用公共 ENC 规则解密。
    decoded = decrypt_enc(encoded)
    records: List[FirttechRecord] = []

    # 按固定记录大小逐条读取。
    for index in range(FIRTTECH_RECORD_COUNT):
        base = index * FIRTTECH_RECORD_SIZE
        record_id = read_u32(decoded, base + FIRT_OFF_ID)
        name = decode_cp950_cstring(
            decoded[base + FIRT_OFF_NAME : base + FIRT_OFF_NAME + FIRT_NAME_SIZE]
        )
        action_class = read_u32(decoded, base + FIRT_OFF_CLASS)
        records.append(
            FirttechRecord(
                index=index,
                record_id=record_id,
                name=name,
                action_class=action_class,
            )
        )

    return decoded, records


def verify_hash(label: str, actual: str, expected: str) -> Dict[str, object]:
    """把一个 SHA-256 检查整理成结构化报告。"""

    return {
        "项目": label,
        "实际SHA256": actual,
        "预期SHA256": expected,
        "PASS": actual == expected,
    }


def verify_exact_bytes(image: PeImage, va: int, expected_hex: str, meaning: str) -> Dict[str, object]:
    """验证某个地址上的短机器码序列是否逐字节等于预期。"""

    # fromhex 把便于阅读的十六进制文字变成真正 bytes。
    expected = bytes.fromhex(expected_hex)
    # 从原版 EXE 的指定 VA 读相同长度。
    actual = image.read_va(va, len(expected))
    return {
        "地址": f"0x{va:08X}",
        "意义": meaning,
        "实际": actual.hex(" "),
        "预期": expected.hex(" "),
        "PASS": actual == expected,
    }


def scan_direct_calls(image: PeImage, target_va: int) -> List[int]:
    """扫描 .text 中所有 x86 E8 rel32，返回直接 CALL 到 target_va 的调用点。"""

    # 只扫描 .text，避免数据区恰好出现 0xE8 被误认成 CALL。
    section = image.text_section()
    text_va = image.image_base + section.virtual_address
    text_size = min(section.raw_size, len(image.data) - section.raw_offset)
    text = image.data[section.raw_offset : section.raw_offset + text_size]
    hits: List[int] = []

    # E8 后面还需要 4 字节相对位移，因此最后 4 字节不可能形成完整 CALL。
    for offset in range(0, max(0, len(text) - 4)):
        # 不是 E8 就跳过。这比固化55早期“范围中只要出现 E8 字节”可靠得多。
        if text[offset] != 0xE8:
            continue

        # rel32 是有符号 32 位数，可以向前也可以向后跳。
        relative = struct.unpack_from("<i", text, offset + 1)[0]
        caller_va = text_va + offset
        # x86 CALL 的目标 = 下一条指令地址 + rel32；E8 rel32 总长度 5 字节。
        destination = caller_va + 5 + relative
        if destination == target_va:
            hits.append(caller_va)

    return hits


def verify_machine_code(original: PeImage, research: PeImage) -> Dict[str, object]:
    """验证固化57依赖的机器码、短断言、字符串、虚表和双版本等价性。"""

    slice_checks: List[Dict[str, object]] = []

    # 逐段重新计算 trusted original 的 SHA。
    for start, end, meaning, expected_hash in CRITICAL_SLICES:
        data = original.read_va(start, end - start)
        actual_hash = sha256_bytes(data)
        slice_checks.append(
            {
                "范围": f"0x{start:08X}..0x{end - 1:08X}",
                "意义": meaning,
                "实际SHA256": actual_hash,
                "预期SHA256": expected_hash,
                "PASS": actual_hash == expected_hash,
            }
        )

    # 短机器码断言不只是“整段哈希没变”，还直接指向本轮结论的关键指令。
    exact_checks = [
        verify_exact_bytes(
            original,
            0x0041FBE0,
            "8b8e480800008b411883f802740a83f801740583f804750d",
            "读取FIRTTECH+0x18并仅把class2/1/4送入共享开始门",
        ),
        verify_exact_bytes(
            original,
            0x0041FBF8,
            "8b0dc8408c006a03e8fb450100",
            "对全局0x8C40C8调用0x434200(state=3)",
        ),
        verify_exact_bytes(
            original,
            0x0041FC83,
            "8b8e480800008b411883f801740a83f802740583f804750f83c104518b0d18248e00e816760200",
            "再次仅允许class1/2/4，把FIRTTECH+4名称传给0x4472C0",
        ),
        verify_exact_bytes(
            original,
            0x00420048,
            "8b8648080000c7442410ed0300008b48183bcf740e83f901740983f9040f85a8000000",
            "后段再次只让class2/1/4进入共享表现块，其余跳到0x420113",
        ),
        verify_exact_bytes(
            original,
            0x004200FF,
            "8b0dc8408c006a00e8f4400100",
            "共享表现块尾部对0x8C40C8调用0x434200(state=0)",
        ),
        verify_exact_bytes(
            original,
            0x0043251F,
            "68a4030000e836f1010083c4048944240885c0c74424140400000074098bc8e8fd160000eb0233c06afe6a008bc88974241ca3c8408c00",
            "创建0x3A4字节对象、调用0x433C40并保存到全局0x8C40C8",
        ),
        verify_exact_bytes(
            original,
            0x0043255F,
            "8b0dec018b0068ccab46006a00e8fff8ffff",
            "战斗地板初始化失败时提交作者错误字符串0x46ABCC",
        ),
        verify_exact_bytes(
            original,
            0x00434243,
            "33f639b7940300007528",
            "0x434200以state==0与state!=0为四子对象切换总门",
        ),
        verify_exact_bytes(
            original,
            0x0043425B,
            "c6825803000001",
            "state0路径把每个子对象+0x358标志写1",
        ),
        verify_exact_bytes(
            original,
            0x00434281,
            "c6815803000000",
            "非零state路径把每个子对象+0x358标志写0",
        ),
        verify_exact_bytes(
            original,
            0x00441D72,
            "6894050000e8e3f8000083c4048944240c3bc3c64424180574098bc8e87d530000eb0233c0a318248e00",
            "Battle对象分配0x594字节、调用0x447110并保存全局0x8E2418",
        ),
        verify_exact_bytes(
            original,
            0x0044715A,
            "6860ca46008d8e6c05000053",
            "0x447110构造器直接引用Menus\\MEN0018.SF2字符串",
        ),
        verify_exact_bytes(
            original,
            0x004471D3,
            "89867c050000e812a8feff",
            "构造器把文字对象保存到this+0x57C",
        ),
        verify_exact_bytes(
            original,
            0x004472C8,
            "578b8e7c050000e84ca7feff",
            "0x4472C0把输入C字符串复制进this+0x57C文字对象",
        ),
        verify_exact_bytes(
            original,
            0x004472E3,
            "498d0449b94c000000d1e02bc8894a24",
            "水平位置公式严格为x=76-6*strlen_bytes",
        ),
        verify_exact_bytes(
            original,
            0x004473A0,
            "b801000000c781880500000000000089818c050000888179050000c7819005000027000000c3",
            "进入状态：step=0,state=1,active=1,hold=0x27",
        ),
        verify_exact_bytes(
            original,
            0x004473D0,
            "c7818c05000002000000c7818805000000000000c3",
            "保持计时结束后切到退出state=2并把step重置0",
        ),
    ]

    # 读取原版作者/资源字符串，避免人工抄错。
    string_checks: List[Dict[str, object]] = []
    for va, expected in EXPECTED_EXE_STRINGS.items():
        actual = original.read_cstring_va(va, "cp950")
        string_checks.append(
            {
                "地址": f"0x{va:08X}",
                "实际": actual,
                "预期": expected,
                "PASS": actual == expected,
            }
        )

    # 虚表的第6个零基槽应直接指向 0x447300 更新函数。
    update_slot_va = BANNER_VTABLE_VA + BANNER_UPDATE_SLOT_INDEX * 4
    update_target = original.read_u32_va(update_slot_va)
    vtable_check = {
        "虚表": f"0x{BANNER_VTABLE_VA:08X}",
        "槽索引": BANNER_UPDATE_SLOT_INDEX,
        "槽VA": f"0x{update_slot_va:08X}",
        "实际目标": f"0x{update_target:08X}",
        "预期目标": f"0x{EXPECTED_BANNER_UPDATE_VA:08X}",
        "PASS": update_target == EXPECTED_BANNER_UPDATE_VA,
    }

    # 逐个大范围比较 original 和 research EXE。
    equivalence_checks: List[Dict[str, object]] = []
    for start, end, meaning in EQUIVALENCE_RANGES:
        original_bytes = original.read_va(start, end - start)
        research_bytes = research.read_va(start, end - start)
        equivalence_checks.append(
            {
                "范围": f"0x{start:08X}..0x{end - 1:08X}",
                "意义": meaning,
                "原版SHA256": sha256_bytes(original_bytes),
                "研究版SHA256": sha256_bytes(research_bytes),
                "逐字节一致": original_bytes == research_bytes,
                "PASS": original_bytes == research_bytes,
            }
        )

    # 扫描全部 .text，而不是 grep 文本反汇编，从机器字节直接确认调用面。
    call_hits = scan_direct_calls(original, 0x004472C0)
    direct_call_check = {
        "目标": "0x004472C0",
        "实际直接CALL": [f"0x{x:08X}" for x in call_hits],
        "预期直接CALL": [f"0x{x:08X}" for x in EXPECTED_4472C0_DIRECT_CALLS],
        "PASS": tuple(call_hits) == EXPECTED_4472C0_DIRECT_CALLS,
    }

    # 把所有子检查合并为机器层总 PASS。
    all_pass = (
        all(item["PASS"] for item in slice_checks)
        and all(item["PASS"] for item in exact_checks)
        and all(item["PASS"] for item in string_checks)
        and bool(vtable_check["PASS"])
        and all(item["PASS"] for item in equivalence_checks)
        and bool(direct_call_check["PASS"])
    )

    return {
        "关键切片": slice_checks,
        "关键指令断言": exact_checks,
        "作者与资源字符串": string_checks,
        "招式名组件虚表更新槽": vtable_check,
        "原版与研究版关键范围等价": equivalence_checks,
        "0x4472C0直接调用面": direct_call_check,
        "PASS": all_pass,
    }


def analyze_resources(
    firttech_path: Path,
    men0018_path: Path,
    decoded: bytes,
    records: Sequence[FirttechRecord],
) -> Dict[str, object]:
    """验证 FIRTTECH class 分布、共享显示集合与 MEN0018 身份。"""

    # 先统计每个 class 有多少条记录。
    class_counts: Dict[int, int] = {}
    for record in records:
        class_counts[record.action_class] = class_counts.get(record.action_class, 0) + 1

    # Python 字典的键可能不是固定顺序；输出前按数值排序，便于人工 diff。
    sorted_counts = {str(key): class_counts[key] for key in sorted(class_counts)}
    expected_counts = {str(key): value for key, value in EXPECTED_CLASS_COUNTS.items()}

    # canonical 资源里没有 class3。这里单独给出布尔值，防止只看统计表漏掉这个边界。
    class3_absent = class_counts.get(3, 0) == 0

    # 统计会被本轮共享表现门覆盖的记录数量。
    displayed_records = [
        record for record in records if record.action_class in EXPECTED_PRESENTATION_CLASSES
    ]

    # 统计明确不进入该门的当前 canonical class。
    excluded_records = [
        record for record in records if record.action_class not in EXPECTED_PRESENTATION_CLASSES
    ]

    # 资源哈希是所有统计的前提。
    firt_encoded_hash = sha256_file(firttech_path)
    firt_decoded_hash = sha256_bytes(decoded)
    men_hash = sha256_file(men0018_path)
    men_size = men0018_path.stat().st_size

    hash_checks = [
        verify_hash(
            "canonical Firttech.enc 加密文件",
            firt_encoded_hash,
            EXPECTED_FIRTTECH_ENCODED_SHA256,
        ),
        verify_hash(
            "canonical Firttech.enc 解密明文",
            firt_decoded_hash,
            EXPECTED_FIRTTECH_DECODED_SHA256,
        ),
        verify_hash(
            "MenusDir.DAT提取MEN0018.SF2",
            men_hash,
            EXPECTED_MEN0018_SHA256,
        ),
    ]

    # 给 CSV 输出准备一行一个 class 的清楚摘要。
    csv_rows: List[Dict[str, object]] = []
    all_classes = sorted(set(class_counts) | {3})
    for action_class in all_classes:
        sample_names = [
            record.name
            for record in records
            if record.action_class == action_class and record.name
        ][:6]
        csv_rows.append(
            {
                "class": action_class,
                "canonical记录数": class_counts.get(action_class, 0),
                "进入共享招式名显示门": action_class in EXPECTED_PRESENTATION_CLASSES,
                "代表名称": " | ".join(sample_names),
                "备注": (
                    "canonical资源无此class"
                    if class_counts.get(action_class, 0) == 0
                    else ""
                ),
            }
        )

    # 资源层总 PASS 要同时满足：哈希、分布、class3缺席、显示记录总数、MEN0018大小。
    expected_display_count = sum(EXPECTED_CLASS_COUNTS[x] for x in EXPECTED_PRESENTATION_CLASSES)
    structure_pass = (
        sorted_counts == expected_counts
        and class3_absent
        and len(displayed_records) == expected_display_count
        and len(excluded_records) == FIRTTECH_RECORD_COUNT - expected_display_count
        and men_size == EXPECTED_MEN0018_SIZE
    )
    overall = all(item["PASS"] for item in hash_checks) and structure_pass

    return {
        "哈希检查": hash_checks,
        "FIRTTECH_class分布": sorted_counts,
        "预期class分布": expected_counts,
        "canonical_class3缺席": class3_absent,
        "共享表现class": list(EXPECTED_PRESENTATION_CLASSES),
        "共享表现记录数": len(displayed_records),
        "预期共享表现记录数": expected_display_count,
        "非共享表现记录数": len(excluded_records),
        "MEN0018": {
            "实际大小": men_size,
            "预期大小": EXPECTED_MEN0018_SIZE,
            "SHA256": men_hash,
            "备注": "画布153x40、Section[1,1,3,0]由同包稳定SF2调查器另行重放并保存证据JSON。",
        },
        "class摘要表": csv_rows,
        "PASS": overall,
    }


def build_conclusions(machine: Dict[str, object], resources: Dict[str, object]) -> Dict[str, object]:
    """把已经验证过的机器事实翻译成兼容引擎可以直接实现的规则。"""

    # 这里不再做新的猜测，只把前面通过的断言组织成实现语句。
    conclusions = {
        "共享class集合": (
            "原版在0x41FBE0、0x41FC83与0x420048三处都把FIRTTECH class 1/2/4放入同一表现组；"
            "canonical class0/5/6不进入，class3在500条canonical资源中不存在。"
        ),
        "动作开始_战斗地板": (
            "class1/2/4在0x41FBF8对全局0x8C40C8调用0x434200(3)。"
            "0x8C40C8由0x433C40对象创建，初始化失败使用作者文字『戰鬥地板啟動失敗!』，"
            "并加载Flr0020.sf2；因此可把它限定称为战斗地板组件。"
        ),
        "战斗地板state语义": (
            "0x434200保存state到this+0x394，然后循环4个子对象。state==0时把每个子对象+0x358写1并调用0x43E6A0；"
            "任意非零state（包括3）时写0并调用0x43E6D0。固化57中性描述为四子对象启用/抑制，"
            "不把未知底层调用擅自命名成具体渲染API。"
        ),
        "招式名提交": (
            "class1/2/4在0x444860动作表现构造之后，把FIRTTECH指针加4，即FIRTTECH+0x04名称字段，"
            "传给全局0x8E2418对象的0x4472C0。原版.text对0x4472C0只有0x41FCA5这一处直接CALL。"
        ),
        "招式名组件身份": (
            "0x8E2418由Battle对象构造时分配0x594字节并调用0x447110创建。0x447110直接加载Menus\\MEN0018.SF2，"
            "建立文字对象到this+0x57C；MEN0018 canonical哈希为2a6f...c4b6。"
        ),
        "文字复制与居中": (
            "0x4472C0先用0x431A20深拷贝输入C字符串；0x431A20会释放旧缓冲、按strlen+1重新分配并逐字节复制。"
            "随后0x4472C0再次计算strlen，并严格写x = 76 - 6 * strlen_bytes到文字对象+0x24。"
            "MEN0018画布宽153；对Big5中文每字2字节时相当于每汉字12像素的中心定位规则。"
        ),
        "进入保持退出状态机": (
            "0x4473A0启动时step=0、state=1、active=1、hold=0x27。虚表0x461144的更新槽直接指向0x447300。"
            "0x447330让step从0到9共推进10步；进入阶段完成后保持计数由0x447300每legacy tick减1，"
            "归零时0x4473D0切state=2并重置step，再执行0到9共10步退出，最终active清0。"
        ),
        "动作后段_战斗地板恢复": (
            "0x420048再次只让class1/2/4进入共享后段；两路0x43DB20请求是否触发还受角色索引与FIRTTECH+0x20条件控制，"
            "但该共享块尾部0x4200FF固定对0x8C40C8调用0x434200(0)，恢复四子对象启用状态。"
        ),
        "Godot兼容规则": (
            "对于canonical class1/2/4，兼容引擎应复现：进入动作时抑制战斗地板四子对象 -> 完成动作表现构造后提交"
            "FIRTTECH名称到MEN0018等价的临时招式名组件 -> 组件按10步进入、0x27 tick保持、10步退出推进 -> "
            "后段恢复战斗地板四子对象。class0/5/6不要套用这组门；class3当前资源无实例。"
        ),
        "术语边界": (
            "本节点只闭合机器class集合与共享表现行为，不把class1/class2自行改名成作者正式『绝学/法术』枚举。"
            "资源语义可用于理解，但作者源码枚举名仍应与机器class值分开记录。"
        ),
    }

    overall = bool(machine.get("PASS")) and bool(resources.get("PASS"))
    return {
        "结论": conclusions,
        "范围限制": [
            "0x43DB20两路请求的最终业务名称未在固化57闭合；只保留为共享后段中的条件表现请求。",
            "0x434200底层0x43E6A0/0x43E6D0没有在本节点重新命名；这里只按+0x358标志和作者『战斗地板』边界描述启用/抑制。",
            "0x27是旧式逻辑tick计数；历史节点已确认主逻辑20Hz，但本节点不把资源动画帧率与此计数混为一谈。",
            "class1/class2的作者正式枚举名称仍未直接取到；不得只根据菜单或招式文本宣称源码枚举名已闭合。",
        ],
        "PASS": overall,
    }


def write_json(path: Path, payload: Dict[str, object]) -> None:
    """用 UTF-8、两空格缩进写机器报告。"""

    # 输出目录可能不存在，所以先创建。
    path.parent.mkdir(parents=True, exist_ok=True)
    # ensure_ascii=False 让中文直接可读；末尾补换行方便版本控制和文本工具。
    path.write_text(
        json.dumps(payload, ensure_ascii=False, indent=2, sort_keys=False) + "\n",
        encoding="utf-8",
    )


def write_csv(path: Path, rows: Sequence[Dict[str, object]]) -> None:
    """写严格 CSV；使用 UTF-8 BOM 便于简体中文 Windows/Excel 直接打开。"""

    path.parent.mkdir(parents=True, exist_ok=True)
    if not rows:
        raise ValueError("class摘要表为空，不能写CSV")

    # 第一行字典的 key 顺序就是固定列顺序。
    fieldnames = list(rows[0].keys())
    # newline='' 交给 csv 模块自己处理换行，避免 Windows 双空行。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def build_parser() -> argparse.ArgumentParser:
    """定义命令行接口。"""

    parser = argparse.ArgumentParser(
        description=(
            "验证《幽城幻剑录》Battle FIRTTECH class1/2/4共享招式名临时显示、"
            "战斗地板state3/state0表现边界与MEN0018身份。"
        )
    )

    # 原版和研究版都要求显式传入，工具不会偷偷从当前目录猜文件。
    parser.add_argument("--original-rpg", required=True, type=Path, help="完全未修改原版 RPG.exe")
    parser.add_argument("--research-rpg", required=True, type=Path, help="历史研究版 RPG.exe")
    # Firttech.enc 用来验证 class 分布和显示集合规模。
    parser.add_argument("--firttech", required=True, type=Path, help="canonical Public/Firttech.enc")
    # MEN0018 应先由同包资源容器工具从 canonical MenusDir.DAT 提取。
    parser.add_argument("--men0018", required=True, type=Path, help="从 canonical MenusDir.DAT 提取的 MEN0018.SF2")
    # JSON 是机器可重放主报告。
    parser.add_argument("--json-out", required=True, type=Path, help="输出 JSON 报告")
    # CSV 让人一眼看到各 class 数量和是否进入共享显示门。
    parser.add_argument("--csv-out", required=True, type=Path, help="输出 class 分布 CSV")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    """执行固化57完整验证；全部通过返回0，否则返回2。"""

    # argparse 会处理 --help，并检查参数是否齐全。
    args = build_parser().parse_args(argv)

    # 在真正解析前逐个确认输入是普通文件，报错会更直观。
    for label, path in (
        ("完全原版RPG.exe", args.original_rpg),
        ("历史研究版RPG.exe", args.research_rpg),
        ("Firttech.enc", args.firttech),
        ("MEN0018.SF2", args.men0018),
    ):
        if not path.is_file():
            raise FileNotFoundError(f"{label} 不存在或不是普通文件：{path}")

    # 第一层：先锁死两份 EXE 身份。
    exe_hash_checks = [
        verify_hash(
            "完全未修改原版RPG.exe",
            sha256_file(args.original_rpg),
            EXPECTED_ORIGINAL_RPG_SHA256,
        ),
        verify_hash(
            "历史研究版RPG.exe",
            sha256_file(args.research_rpg),
            EXPECTED_RESEARCH_RPG_SHA256,
        ),
    ]

    # 第二层：解析 PE section 表，所有 VA 都通过正式映射读取。
    original = parse_pe(args.original_rpg)
    research = parse_pe(args.research_rpg)

    # 第三层：解密 FIRTTECH 并读取500条 class。
    decoded, records = parse_firttech(args.firttech)

    # 第四层：机器码、作者字符串、虚表、direct CALL和双版本等价性。
    machine_report = verify_machine_code(original, research)

    # 第五层：canonical class 分布和 MEN0018 身份。
    resource_report = analyze_resources(args.firttech, args.men0018, decoded, records)

    # 第六层：只有前面的事实都 PASS，才允许把它们翻译成兼容规则。
    conclusion_report = build_conclusions(machine_report, resource_report)

    # 最终 PASS 还必须包含 EXE 身份检查。
    overall_pass = (
        all(item["PASS"] for item in exe_hash_checks)
        and bool(machine_report["PASS"])
        and bool(resource_report["PASS"])
        and bool(conclusion_report["PASS"])
    )

    report: Dict[str, object] = {
        "工具": "youcheng_battle_class124_presentation_inspector.py",
        "工具版本": TOOL_VERSION,
        "固化节点": 57,
        "目标": (
            "闭合FIRTTECH class1/2/4共享招式名临时显示、MEN0018组件状态机、"
            "战斗地板state3抑制/state0恢复与canonical class边界。"
        ),
        "输入": {
            "原版RPG.exe": str(args.original_rpg),
            "研究版RPG.exe": str(args.research_rpg),
            "Firttech.enc": str(args.firttech),
            "MEN0018.SF2": str(args.men0018),
        },
        "EXE哈希检查": exe_hash_checks,
        "机器码证据": machine_report,
        "资源证据": resource_report,
        "静态闭合结论": conclusion_report,
        "总体PASS": overall_pass,
    }

    # 先写 CSV，再写 JSON；即使后续人只想看统计，也不用打开大报告。
    write_csv(args.csv_out, resource_report["class摘要表"])
    write_json(args.json_out, report)

    print(f"[固化57] 总体结果：{'PASS' if overall_pass else 'FAIL'}")
    print(f"[固化57] JSON：{args.json_out}")
    print(f"[固化57] CSV ：{args.csv_out}")

    # shell/Windows约定：0成功，非0失败。
    return 0 if overall_pass else 2


# 只有直接运行脚本时才执行；被其他测试脚本 import 时不会自动扫描文件。
if __name__ == "__main__":
    raise SystemExit(main())
