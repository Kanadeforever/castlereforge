#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle FIRTTECH class 5 / class 6 动作状态与来源依赖静态调查器。

这个脚本服务于 Godot 兼容引擎研究固化64。它继承固化56已经验证的稀有class资源身份与基础计算分派，
并继续闭合后来发现的角色动作状态路由、动作结束门以及class6跳过的来源依赖元数据生命周期。它只读取输入文件，绝不改写 RPG.exe、
Firttech.enc 或 Magiccon.enc。它要回答的不是“招式名字看起来像什么”，而是 CPU 真正
会沿哪条机器码路径执行，以及 canonical 资源里 class 5 / class 6 到底有哪些记录。

本轮特别防止一个很容易出现的逆向误读：在 0x42C770 附近确实可以看见
“cmp class,5 / cmp class,6”，但入口 class 5 / 6 在更前面的 0x42C6F9 已经因为
“class != 0”跳到 0x42C7A9，所以那两个比较并不是“入口 class 5 / 6 的专用公式分支”。
工具会同时验证前置跳转、资源字段和下游 class 6 特判，避免只截一小段反汇编就下结论。

代码里的中文注释故意写得非常细，目标是让只学过一天编程的初中生也能逐步看懂：
每一步为什么读文件、为什么检查长度、地址怎样换算、为什么比较 SHA，以及结论怎样
从原始字节和资源记录推出来。
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


# 版本号只写入输出报告，方便以后单独看到 JSON 时判断它是哪一版工具生成的。
TOOL_VERSION = "0.7D-solid64.0"

# 用户已经确认这一份哈希对应“完全未修改的原版 RPG.exe”。
# 因此固化56把它当作原始行为的首要静态 Oracle。
EXPECTED_ORIGINAL_RPG_SHA256 = (
    "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
)

# 这一份是此前研究过程中一直使用的 RPG.exe。
# 它不是原版 Oracle，但可以用来确认本轮关键代码区有没有被历史补丁碰过。
EXPECTED_RESEARCH_RPG_SHA256 = (
    "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
)

# Firttech.enc 一共有 500 条固定大小记录，每条 0x230 字节。
FIRTTECH_RECORD_COUNT = 500
FIRTTECH_RECORD_SIZE = 0x230

# Magiccon.enc 也有 500 条记录，但每条只有 0x40 字节。
MAGICCON_RECORD_COUNT = 500
MAGICCON_RECORD_SIZE = 0x40

# canonical Firttech.enc 的加密文件与解密明文哈希。
# 两个哈希都检查，可以防止“文件大小一样但内容被替换”的误用。
EXPECTED_FIRTTECH_ENCODED_SHA256 = (
    "32b98193c037c8cd75a11534520e189c89c856c367bc1d6ac607e43d9cf52a5d"
)
EXPECTED_FIRTTECH_DECODED_SHA256 = (
    "5469a9dd388972b53b5661e90d85c84f2d0150d06f46ddbae159409a5ac34b1a"
)

# canonical Magiccon.enc 同样保存加密与解密哈希。
EXPECTED_MAGICCON_ENCODED_SHA256 = (
    "f7e20cf1ceb9f62434401bc15d57b9b561ad3622623d7b664b82c5368e9fb01e"
)
EXPECTED_MAGICCON_DECODED_SHA256 = (
    "a8fb1cb7479cd3db4fafbb68c73bef574f33bf50df65dc49ea81ee96e1118c29"
)

# FIRTTECH 本轮实际读取的字段偏移。
# +0x00 是 record ID。
FIRT_OFF_ID = 0x00
# +0x04 开始是招式名称所在的固定宽度区域。
FIRT_OFF_NAME = 0x04
# 名称最多取到 +0x17，避免碰到 +0x18 的 class 整数。
FIRT_NAME_SIZE = 0x14
# +0x18 是本轮研究核心：动作 class/mode。
FIRT_OFF_CLASS = 0x18
# +0x20..+0x60 目前只按原始字段编号保存，不擅自命名作者业务语义。
FIRT_RAW_FIELDS = tuple(range(0x20, 0x64, 4))
# +0x30 是 0x42C7B2 读取的公式/计算 selector。
FIRT_OFF_SELECTOR = 0x30
# +0x64..+0x88 是一组已经被前代研究识别为效果槽区域的 32 位值。
FIRT_EFFECT_FIELDS = tuple(range(0x64, 0x8C, 4))
# +0x8C、+0x90、+0x94、+0x98 也保留原值，便于以后独立接档时核对资源。
FIRT_EXTRA_FIELDS = (0x8C, 0x90, 0x94, 0x98, 0x1E8, 0x1EC)
# 作者说明文字从 +0x17C 一直延伸到 record 尾部。
FIRT_OFF_DESCRIPTION = 0x17C
FIRT_DESCRIPTION_SIZE = FIRTTECH_RECORD_SIZE - FIRT_OFF_DESCRIPTION

# Magiccon 本轮只做同 ID/同名称及少量原字段值交叉核对。
MAGIC_OFF_ID = 0x00
MAGIC_OFF_NAME = 0x04
MAGIC_NAME_SIZE = 0x14
MAGIC_NUMERIC_FIELDS = (0x18, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C)

# 下面五段是固化56的关键代码证据。
# 每段都使用“起始 VA + 结束 VA（不含）+ 已知原版 SHA”的形式保存。
# 这样工具不需要反汇编库，也能逐字节确认机器码没有变化。
CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (
        0x00422B86,
        0x00422C21,
        "每目标class分派与类四第二次调用门",
        "ddfdb21432852e91ec21b2ee852c502713b793e3ff48bdd1e2be5140987a1662",
    ),
    (
        0x0042C6ED,
        0x0042C7C8,
        "主计算器class0前置门与类五六表面比较",
        "f9524167d9dfe7b6d36843eb370c4eca270ace7e4ec128d24f2479e128fcdd82",
    ),
    (
        0x00420F1D,
        0x00420F78,
        "结果应用阶段类六运行时元数据抑制",
        "2c746646d32e4f27c6a7c83526d2af4f79b31273177dbcd1bb494f67e7c45cc8",
    ),
    (
        0x0042159D,
        0x0042160D,
        "结果阶段类六显示请求抑制",
        "cabd9daafc3af7d5165bf38e11df98094dbad01b9f54a3173fe0f01abc434665",
    ),
    (
        0x00446DF0,
        0x00446E57,
        "五十槽结果显示请求队列写入器",
        "4d77c50160de39222df5fc26ae3e3765d17fe2423408b7d6bc412157b4d95bf4",
    ),
)

# 这些较大的范围用来证明原版和研究版在本轮结论依赖区完全一致。
# 如果历史研究版在这些区有任何一个字节不同，本轮就不会把“双版本一致”写成 PASS。
EQUIVALENCE_RANGES: Tuple[Tuple[int, int, str], ...] = (
    (0x00420ED0, 0x0042168A, "结果应用主范围"),
    (0x00422AB0, 0x00422CAF, "每目标动作分派范围"),
    (0x0042C680, 0x0042CC94, "FIRTTECH主计算器范围"),
    (0x00446DF0, 0x00446E57, "结果显示请求队列写入范围"),
)

# 这三个地址是原版 .text 内所有“直接 CALL 0x446DF0”的调用点。
# 扫描全部 .text 而不是只查 0x421608，可以确认这个队列写入函数的静态调用面。
EXPECTED_446DF0_CALLS = (0x0042113E, 0x00421160, 0x00421608)

# canonical 资源中 class 5 / 6 各只有一条。
# 这里写死 ID 与名称，是为了让换错资源时工具明确失败，而不是悄悄研究另一版本。
EXPECTED_RARE_RECORDS = {
    5: (320, 0x140, "潛地蟄伏"),
    6: (345, 0x159, "鐵藤纏護"),
}


# 固化64新发现：除了固化56已经闭合的结果应用差异，Battle角色动作启动器还会
# 根据 FIRTTECH class 把一个“角色动作请求状态码”写到每角色控制块 +0x10C8。
# 这里把确切机器值固定下来；它们只是兼容层中性名字，不冒充原作者枚举名。
EXPECTED_ACTION_STATE_CODES = {
    # class1 / class2 / class4 会在更早的判断处直接跳到共享分支，
    # 所以三类真正活动的状态码都是 0xBBC。
    "shared_class_1_2_4": 0x0BBC,
    5: 0x0BC0,
    6: 0x0BC6,
    "default_non_12456": 0x0BBB,
}

# 原版后面仍残留一段“再次比较 class4 并写 0xBC7”的字面代码。
# 但入口 class4 在 0x443370 已经跳走，因此当前这条入口路径下它不可达。
# 保留这个常量是为了让调查器显式证明“代码存在但不可达”，而不是把它删掉后假装没看见。
UNREACHABLE_CLASS4_LITERAL_STATE = 0x0BC7

# +0xCE8 / +0xDE0 是固化56里 class6 会跳过写入的两项 FightRole 运行时字段。
# 固化64通过“全 .text 中位移常量的完整出现面 + 关键消费者”把它们的机器协议闭合：
#   +0xCE8 默认 0xFA0；被普通动作绑定来源后变成 0xFA1；
#   +0xDE0 默认 -1；绑定后保存来源角色槽号。
# 下面这些地址是位移 DWORD 自身在指令里的起始 VA，不是指令起始 VA。
EXPECTED_CE8_DISPLACEMENT_HITS = (
    0x0041F123, 0x0041F259, 0x00420F3B, 0x00420F63, 0x00420FE8,
    0x004214BA, 0x0042253F, 0x004226FC, 0x004227A8, 0x00444EEA,
)
EXPECTED_DE0_DISPLACEMENT_HITS = (
    0x0041F150, 0x00420F4C, 0x00420F74, 0x00420FF9,
    0x0042255F, 0x00422735, 0x004227F2,
)

# +0x10C8 是固化64新闭合的角色控制块动作状态字段。
# 同样记录“位移DWORD在机器码里的位置”，再与独立反汇编证据交叉核对。
EXPECTED_10C8_DISPLACEMENT_HITS = (
    0x00442B78, 0x00442BED, 0x00442EA0, 0x00442EFF, 0x00442F31,
    0x00442F53, 0x00442F96, 0x004430E8, 0x00443396, 0x004433A7,
    0x004433B8, 0x004433C4, 0x00443429, 0x004434A0, 0x0044354E,
)

# 这些切片共同覆盖固化64新增结论的完整“建立→消费→清理/阻挡”证据链。
# 每个哈希都来自完全未修改原版 RPG.exe；研究版也必须逐字节等价。
SOLID64_CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (0x00443310, 0x00443461, "class5/6角色动作状态路由", "a94fa0e2c0ed150a0fe3dabda1aa5d9c9b90d3dd58ce74c59c223943cda418cb"),
    (0x00442CC3, 0x00442D5E, "BC0/BC6阻挡通用7D6到7D1结束门", "bcb8661c00c403c6defee0ffa39bbb44230414fb3ab7522e0b5a8a5b968c83f6"),
    (0x0041F10D, 0x0041F160, "来源依赖字段初始化", "f44696d9fc987124c04c271d68dbdb0e982443cac23fbb6251d6019ab94b2e19"),
    (0x00420F1D, 0x00420F78, "普通结果应用来源依赖写入与class6跳过门", "2c746646d32e4f27c6a7c83526d2af4f79b31273177dbcd1bb494f67e7c45cc8"),
    (0x00422530, 0x004225A7, "来源依赖首个可行动消费者", "fc702ac90c06a702f922851d208e136ee14e532ec2b75a6af49466c7b551be8b"),
    (0x004226F1, 0x0042282B, "来源依赖队伍级状态判定消费者", "6f840d8e81a5132924bb024cf8108856138afcce666125b50a037b6f4b746510"),
    (0x00445FB0, 0x0044601F, "BC0当前角色状态在候选缓存构建中被排除", "d2f3e6eee2ff96a62e4248a20715ffbfb59799cbff9bfec2a71dffffe90d1473"),
    (0x004214D3, 0x00421560, "BC6当前角色状态归零第一结果DWORD", "40040f14c3465712eb35467a9cff1cbcb2259a82d46e8c78d63814219d84fedc"),
    (0x00423738, 0x00423786, "BC6当前角色状态选择独立0x72动作请求", "e7ba3697cc60321fa9f25899ed605641d62a8689073d7bc60fc6a90264439406"),
)


@dataclass(frozen=True)
class Section:
    """PE 文件的一个 section 映射条目。"""

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass(frozen=True)
class PeImage:
    """把 EXE 原始字节、ImageBase 和 section 表放在同一个对象里。"""

    path: Path
    data: bytes
    image_base: int
    sections: Tuple[Section, ...]

    def va_to_file_offset(self, va: int) -> int:
        """把运行时 VA 换算为硬盘 EXE 内的文件偏移。"""

        # VA 是“程序装入内存之后看到的地址”。
        # 减掉 ImageBase 后得到 RVA，也就是“相对整个映像开头的位置”。
        rva = va - self.image_base

        # PE 文件不是一整块简单平铺的数据，而是分成 .text、.rdata 等 section。
        # 所以必须逐个 section 判断这个 RVA 属于谁。
        for section in self.sections:
            # VirtualSize 和 RawSize 有时并不完全相等。
            # 取较大值能覆盖 section 的合法映射区间。
            span = max(section.virtual_size, section.raw_size)

            # 如果 RVA 落在当前 section 的区间里，就可以进行换算。
            if section.virtual_address <= rva < section.virtual_address + span:
                # 先算“距离 section 开头走了多少字节”。
                inside = rva - section.virtual_address
                # 再把这个距离加到 section 在硬盘上的 RawOffset。
                return section.raw_offset + inside

        # 走到这里说明地址不属于任何 section。
        # 与其读错字节继续得出假结论，不如立刻报错停止。
        raise ValueError(f"VA 0x{va:08X} 不在 {self.path.name} 的任何 PE section 中")

    def read_va(self, va: int, size: int) -> bytes:
        """从某个运行时 VA 开始读取固定数量的机器码字节。"""

        # 第一步把内存地址换成文件偏移。
        offset = self.va_to_file_offset(va)
        # Python 切片的 end 不包含自己，因此 end=offset+size 正好得到 size 字节。
        end = offset + size

        # 防止错误参数让切片跨过文件末尾。
        if end > len(self.data):
            raise ValueError(
                f"读取 {self.path.name} 的 0x{va:08X}+0x{size:X} 会越过文件末尾"
            )

        # 这里只返回 bytes，不会对源文件做任何写入。
        return self.data[offset:end]

    def text_section(self) -> Section:
        """找到 .text section；直接 CALL 扫描只应在机器代码区进行。"""

        # 遍历全部 section 名称寻找 .text。
        for section in self.sections:
            if section.name == ".text":
                return section

        # RPG.exe 正常一定有 .text；没有就说明输入不是预期 PE。
        raise ValueError(f"{self.path.name} 找不到 .text section")


@dataclass(frozen=True)
class FirttechRecord:
    """Firttech.enc 中本轮需要的一条结构化记录。"""

    index: int
    record_id: int
    name: str
    action_class: int
    selector: int
    raw_fields: Dict[str, int]
    effect_fields: Dict[str, int]
    extra_fields: Dict[str, int]
    description: str


@dataclass(frozen=True)
class MagicconRecord:
    """Magiccon.enc 中本轮用于交叉核对的一条结构化记录。"""

    index: int
    record_id: int
    name: str
    numeric_fields: Dict[str, int]


def sha256_bytes(data: bytes) -> str:
    """计算一段内存字节的 SHA-256。"""

    # hashlib.sha256 会建立哈希对象；hexdigest 把结果写成常见的 64 位十六进制字符串。
    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """读取完整文件并计算 SHA-256。"""

    # 本项目输入都很小，直接 read_bytes 更容易让初学者理解，也不会有内存压力。
    return sha256_bytes(path.read_bytes())


def read_u16(data: bytes, offset: int) -> int:
    """从 bytes 指定位置读取 little-endian 16 位无符号整数。"""

    # '<H' 中 '<' 表示小端序，H 表示 16 位无符号整数。
    return struct.unpack_from("<H", data, offset)[0]


def read_u32(data: bytes, offset: int) -> int:
    """读取 little-endian 32 位无符号整数。"""

    # '<I' 中 I 表示 32 位无符号整数。
    return struct.unpack_from("<I", data, offset)[0]


def read_s32(data: bytes, offset: int) -> int:
    """读取 little-endian 32 位有符号整数。"""

    # '<i' 的小写 i 表示有符号 32 位整数，因此 0xFFFFFFFF 会得到 -1。
    return struct.unpack_from("<i", data, offset)[0]


def parse_pe(path: Path) -> PeImage:
    """只用 Python 标准库解析本工具需要的最小 PE 信息。"""

    # 先把整个 EXE 读进内存；下面所有解析都只针对这个 bytes 对象。
    data = path.read_bytes()

    # DOS 头至少要能容纳 0x3C 位置的 PE header 指针。
    if len(data) < 0x40:
        raise ValueError(f"{path} 太小，不可能是本项目的 RPG.exe")

    # DOS header +0x3C 保存真正 PE header 的文件偏移。
    pe_offset = read_u32(data, 0x3C)

    # 标准 PE header 必须以四字节 'PE\0\0' 开头。
    if data[pe_offset:pe_offset + 4] != b"PE\x00\x00":
        raise ValueError(f"{path} 的 PE 签名不正确")

    # COFF header +6 保存 section 数量。
    number_of_sections = read_u16(data, pe_offset + 6)
    # COFF header +20 保存 Optional Header 大小。
    optional_header_size = read_u16(data, pe_offset + 20)
    # PE signature 是4字节，COFF header 是20字节，所以 Optional Header 从 +24 开始。
    optional_offset = pe_offset + 24

    # 本游戏是 32 位 PE32，Magic 正常应为 0x10B。
    magic = read_u16(data, optional_offset)
    if magic != 0x10B:
        raise ValueError(f"{path} 不是预期的 32 位 PE32，OptionalHeader.Magic=0x{magic:04X}")

    # PE32 Optional Header +0x1C 是 ImageBase。
    image_base = read_u32(data, optional_offset + 0x1C)

    # section table 紧跟在 Optional Header 后面。
    section_table_offset = optional_offset + optional_header_size
    sections: List[Section] = []

    # 每个 IMAGE_SECTION_HEADER 固定 40 字节。
    for index in range(number_of_sections):
        # 先算当前 section header 的文件位置。
        entry = section_table_offset + index * 40
        # 前8字节是 ASCII 名称，尾部零填充要去掉。
        raw_name = data[entry:entry + 8].split(b"\x00", 1)[0]
        # section 名只需要 ASCII；无法识别的字节用替换符显示诊断即可。
        name = raw_name.decode("ascii", errors="replace")
        # +8 VirtualSize。
        virtual_size = read_u32(data, entry + 8)
        # +12 VirtualAddress，也就是相对 ImageBase 的 RVA。
        virtual_address = read_u32(data, entry + 12)
        # +16 SizeOfRawData。
        raw_size = read_u32(data, entry + 16)
        # +20 PointerToRawData。
        raw_offset = read_u32(data, entry + 20)

        # 把散落的数值收进 Section 对象，后面换算地址时更清晰。
        sections.append(
            Section(
                name=name,
                virtual_address=virtual_address,
                virtual_size=virtual_size,
                raw_offset=raw_offset,
                raw_size=raw_size,
            )
        )

    # 最后返回一个不可变的 PeImage。
    return PeImage(path=path, data=data, image_base=image_base, sections=tuple(sections))


def decrypt_enc(encoded: bytes) -> bytes:
    """按已固化的 ENC 规则做逐字节 XOR 解码。"""

    # 第 i 个字节使用 i % 255 当作 key。
    # XOR 的同一运算再做一次就能恢复，所以这里既是“解密规则”，也是可重放的结构规则。
    return bytes(value ^ (index % 255) for index, value in enumerate(encoded))


def decode_cp950_cstring(raw: bytes) -> str:
    """把固定宽度 CP950 C 字符串变成人类可读文字。"""

    # C 字符串以第一个 0x00 结束，后面的零只是填充，不应变成文字的一部分。
    payload = raw.split(b"\x00", 1)[0]
    # 台湾版资源使用 CP950；replace 只让坏样本仍可报告，canonical 不应出现替换符。
    return payload.decode("cp950", errors="replace")


def parse_firttech(path: Path) -> Tuple[bytes, List[FirttechRecord]]:
    """解码并遍历全部 500 条 FIRTTECH，不只挑 class 5/6。"""

    # 读取加密文件。
    encoded = path.read_bytes()
    # 固定几何应该等于 500 × 0x230。
    expected_size = FIRTTECH_RECORD_COUNT * FIRTTECH_RECORD_SIZE
    # 长度不符时任何字段偏移都会失去意义，所以立刻失败。
    if len(encoded) != expected_size:
        raise ValueError(f"Firttech.enc 应为 {expected_size} 字节，实际 {len(encoded)} 字节")

    # 对整个文件连续 XOR，不能每条 record 重置 key。
    decoded = decrypt_enc(encoded)
    # 建立空列表准备保存500条结构化记录。
    records: List[FirttechRecord] = []

    # 0..499 每个 index 都要解析，才能可靠统计 class 分布。
    for index in range(FIRTTECH_RECORD_COUNT):
        # 计算当前 record 的起点。
        start = index * FIRTTECH_RECORD_SIZE
        # 切出刚好0x230字节。
        record = decoded[start:start + FIRTTECH_RECORD_SIZE]

        # raw_fields 用“+0xNN”作 key，明确表示这里只记录磁盘原值。
        raw_fields = {
            f"+0x{offset:03X}": read_s32(record, offset)
            for offset in FIRT_RAW_FIELDS
        }
        # effect_fields 同样不为尚未完全定名的槽位编造名称。
        effect_fields = {
            f"+0x{offset:03X}": read_s32(record, offset)
            for offset in FIRT_EFFECT_FIELDS
        }
        # extra_fields 保存后部本轮需要核对的几个数值。
        extra_fields = {
            f"+0x{offset:03X}": read_s32(record, offset)
            for offset in FIRT_EXTRA_FIELDS
        }

        # 把当前记录收进列表。
        records.append(
            FirttechRecord(
                index=index,
                record_id=read_u32(record, FIRT_OFF_ID),
                name=decode_cp950_cstring(record[FIRT_OFF_NAME:FIRT_OFF_NAME + FIRT_NAME_SIZE]),
                action_class=read_s32(record, FIRT_OFF_CLASS),
                selector=read_s32(record, FIRT_OFF_SELECTOR),
                raw_fields=raw_fields,
                effect_fields=effect_fields,
                extra_fields=extra_fields,
                description=decode_cp950_cstring(
                    record[FIRT_OFF_DESCRIPTION:FIRT_OFF_DESCRIPTION + FIRT_DESCRIPTION_SIZE]
                ),
            )
        )

    # 返回明文字节供哈希验证，同时返回结构化记录供统计。
    return decoded, records


def parse_magiccon(path: Path) -> Tuple[bytes, List[MagicconRecord]]:
    """解码 Magiccon.enc，并完整解析500条记录用于同索引交叉核对。"""

    # 读取加密 Magiccon.enc。
    encoded = path.read_bytes()
    # 这里的固定几何是 500 × 0x40。
    expected_size = MAGICCON_RECORD_COUNT * MAGICCON_RECORD_SIZE
    # 大小不对就停止，避免把错文件当 Magiccon。
    if len(encoded) != expected_size:
        raise ValueError(f"Magiccon.enc 应为 {expected_size} 字节，实际 {len(encoded)} 字节")

    # 使用和其他 ENC 相同的连续 XOR 规则解码。
    decoded = decrypt_enc(encoded)
    # 准备保存500条记录。
    records: List[MagicconRecord] = []

    # 逐条遍历全部记录。
    for index in range(MAGICCON_RECORD_COUNT):
        # 算当前0x40字节 record 的起点。
        start = index * MAGICCON_RECORD_SIZE
        # 切出当前记录。
        record = decoded[start:start + MAGICCON_RECORD_SIZE]
        # 将十个32位数字按原始偏移保存。
        numeric_fields = {
            f"+0x{offset:02X}": read_s32(record, offset)
            for offset in MAGIC_NUMERIC_FIELDS
        }
        # 保存 record ID、名称与数值。
        records.append(
            MagicconRecord(
                index=index,
                record_id=read_u32(record, MAGIC_OFF_ID),
                name=decode_cp950_cstring(record[MAGIC_OFF_NAME:MAGIC_OFF_NAME + MAGIC_NAME_SIZE]),
                numeric_fields=numeric_fields,
            )
        )

    # 同样返回明文与结构化列表。
    return decoded, records


def verify_hash(name: str, actual: str, expected: str) -> Dict[str, object]:
    """把一次 SHA 比较包装成统一 JSON 结构。"""

    # 报告同时保存实际值和预期值，失败时不用重新跑工具才知道差在哪里。
    return {
        "名称": name,
        "实际SHA256": actual,
        "预期SHA256": expected,
        "PASS": actual == expected,
    }


def verify_exact_bytes(image: PeImage, va: int, expected_hex: str, meaning: str) -> Dict[str, object]:
    """在固定 VA 验证一段很短但语义关键的机器码。"""

    # bytes.fromhex 把“83 F9 05”这种人类写法变成真正 bytes。
    expected = bytes.fromhex(expected_hex)
    # 从 EXE 的同一个运行时 VA 读取同样长度。
    actual = image.read_va(va, len(expected))
    # 返回可供人类复核的十六进制和 PASS。
    return {
        "VA": f"0x{va:08X}",
        "含义": meaning,
        "预期机器码": actual.hex(" ").upper() if actual == expected else expected.hex(" ").upper(),
        "实际机器码": actual.hex(" ").upper(),
        "PASS": actual == expected,
    }


def verify_critical_machine_code(original: PeImage, research: PeImage) -> Dict[str, object]:
    """验证固化56全部关键机器码与双 EXE 等价范围。"""

    # slice_checks 用于保存五个关键局部片段的原版哈希检查。
    slice_checks: List[Dict[str, object]] = []

    # 逐个读取关键片段。
    for start, end, label, expected_sha in CRITICAL_SLICES:
        # end 不含自身，所以长度直接是 end-start。
        blob = original.read_va(start, end - start)
        # 计算原版该局部片段 SHA。
        actual_sha = sha256_bytes(blob)
        # 记录结果。
        slice_checks.append(
            {
                "名称": label,
                "范围": f"0x{start:08X}..0x{end - 1:08X}",
                "长度": len(blob),
                "实际SHA256": actual_sha,
                "预期SHA256": expected_sha,
                "PASS": actual_sha == expected_sha,
            }
        )

    # equivalence_checks 验证原版与研究版在四个大范围逐字节相同。
    equivalence_checks: List[Dict[str, object]] = []

    # 每一个大范围都分别从两份 EXE 读取。
    for start, end, label in EQUIVALENCE_RANGES:
        # 原版字节。
        original_blob = original.read_va(start, end - start)
        # 研究版字节。
        research_blob = research.read_va(start, end - start)
        # 直接 bytes 比较比只比较哈希更严格直观；哈希另外写进报告方便复核。
        same = original_blob == research_blob
        # 保存双版本 SHA 与逐字节是否一致。
        equivalence_checks.append(
            {
                "名称": label,
                "范围": f"0x{start:08X}..0x{end - 1:08X}",
                "长度": len(original_blob),
                "原版SHA256": sha256_bytes(original_blob),
                "研究版SHA256": sha256_bytes(research_blob),
                "逐字节完全一致": same,
                "PASS": same,
            }
        )

    # 下面这些短模式不是为了代替反汇编，而是把本轮最重要的控制流门固定下来。
    exact_checks = [
        verify_exact_bytes(
            original,
            0x00422B9D,
            "83 F9 05 74 2E 83 F9 06 74 29",
            "每目标分派明确把 class5 与 class6 都送到 0x422BD0 的普通 FIRTTECH 调用入口。",
        ),
        verify_exact_bytes(
            original,
            0x00422BF4,
            "83 F9 04 75 28",
            "第一次 0x42A7A0 返回后只有 class4 才进入第二次调用；class5/6直接离开该双调用门。",
        ),
        verify_exact_bytes(
            original,
            0x0042C6ED,
            "8B 47 18 3B C1 89 44 24 28 89 46 4C 0F 85 AA 00 00 00",
            "主计算器先读取 class；只要 class!=0 就在 0x42C6F9 跳到 0x42C7A9。",
        ),
        verify_exact_bytes(
            original,
            0x0042C770,
            "8B 47 18 83 F8 05 74 05 83 F8 06 75 04 C6 46 0C 00",
            "字面存在 class5/6 比较，但它位于前述 class==0 才能自然落入的代码块内部。",
        ),
        verify_exact_bytes(
            original,
            0x0042C7A9,
            "83 F8 04 0F 84 BE 04 00 00 8B 47 30",
            "非零 class 到达 0x42C7A9 后只先特判 class4，然后读取 FIRTTECH+0x30 selector。",
        ),
        verify_exact_bytes(
            original,
            0x0042C82C,
            "8B 54 24 24 8B 5C 24 20 57 52 53 8B CD E8 B2 ED FF FF",
            "selector 不在2..6跳表时走默认 helper 0x42B5F0；canonical class5/6 的 selector 都是0。",
        ),
        verify_exact_bytes(
            original,
            0x00420F52,
            "8B 97 48 08 00 00 85 D2 74 1C 39 6A 18 74 17",
            "普通 FIRTTECH 结果应用路径读取 class，并在 class6 时跳过后面的两项目标运行时元数据写入。",
        ),
        verify_exact_bytes(
            original,
            0x00420F61,
            "89 99 E8 0C 00 00 8B 08 8B 14 8D 94 FD 89 00 8B 0F 89 8A E0 0D 00 00",
            "未被 class6 门跳过时写 target+0xCE8 与 target+0xDE0；这里只保留中性字段描述。",
        ),
        verify_exact_bytes(
            original,
            0x0042159D,
            "8B 81 48 08 00 00 85 C0 74 06 83 78 18 06 74 60",
            "结果阶段再次检查当前 FIRTTECH class；class6 直接跳到 0x42160D。",
        ),
        verify_exact_bytes(
            original,
            0x00421602,
            "8B 0D 14 24 8E 00 E8 E3 57 02 00",
            "未被 class6 门跳过且其他条件满足时调用 0x446DF0 提交一条结果显示请求。",
        ),
    ]

    # 只有五段哈希、四个双版本范围和十个控制流短模式全部通过，机器码证据才算通过。
    overall = (
        all(item["PASS"] for item in slice_checks)
        and all(item["PASS"] for item in equivalence_checks)
        and all(item["PASS"] for item in exact_checks)
    )

    # 返回分组后的机器码证据。
    return {
        "关键片段哈希": slice_checks,
        "原版与研究版关键范围等价": equivalence_checks,
        "控制流关键机器码": exact_checks,
        "PASS": overall,
    }


def scan_direct_calls(image: PeImage, target_va: int) -> List[int]:
    """扫描 .text 内所有 x86 E8 rel32 形式的直接 CALL，并筛出目标地址。"""

    # 先定位 .text section，避免把数据区里碰巧出现的 0xE8 当成指令。
    section = image.text_section()
    # .text 的运行时起点 = ImageBase + section RVA。
    text_va = image.image_base + section.virtual_address
    # 真正存进文件的机器代码长度使用 raw_size。
    text = image.data[section.raw_offset:section.raw_offset + section.raw_size]
    # 保存命中的 CALL 指令地址。
    hits: List[int] = []

    # E8 rel32 一共5字节，所以最后4字节不能再当合法起点。
    for offset in range(0, max(0, len(text) - 4)):
        # 不是 opcode E8 就直接继续，减少无意义的位移解码。
        if text[offset] != 0xE8:
            continue

        # E8 后四字节是有符号相对位移。
        displacement = struct.unpack_from("<i", text, offset + 1)[0]
        # 当前 CALL 指令的运行时 VA。
        call_va = text_va + offset
        # x86 rel32 的目标 = 下一条指令地址(call_va+5) + displacement。
        resolved_target = call_va + 5 + displacement

        # 只保存真正解析到目标函数的 CALL。
        if resolved_target == target_va:
            hits.append(call_va)

    # 按扫描顺序自然就是地址升序。
    return hits


def analyze_resources(
    firttech_path: Path,
    magiccon_path: Path,
    firttech_decoded: bytes,
    firttech_records: Sequence[FirttechRecord],
    magiccon_decoded: bytes,
    magiccon_records: Sequence[MagicconRecord],
) -> Dict[str, object]:
    """统计资源 class 分布并严格验证两条稀有 class 记录。"""

    # 先统计全部500条的 class 分布，而不是只找5和6。
    class_counts: Dict[int, int] = {}
    # 每读一条就把对应 class 计数加1。
    for record in firttech_records:
        class_counts[record.action_class] = class_counts.get(record.action_class, 0) + 1

    # 用字符串 key 写 JSON，避免不同 JSON 阅读器对整数 key 的显示差异。
    class_counts_json = {str(key): class_counts[key] for key in sorted(class_counts)}

    # rare_rows 保存最终要写 CSV 的两条完整交叉核对行。
    rare_rows: List[Dict[str, object]] = []
    # record_checks 保存每个 class 的严格断言。
    record_checks: List[Dict[str, object]] = []

    # 按 class5、class6 的顺序检查。
    for action_class in (5, 6):
        # 从500条里筛出当前 class。
        matches = [record for record in firttech_records if record.action_class == action_class]
        # 读取 canonical 预期 index/id/name。
        expected_index, expected_id, expected_name = EXPECTED_RARE_RECORDS[action_class]

        # 先检查是否真的只有一条。
        one_only = len(matches) == 1
        # 如果不是一条，就不能安全访问 matches[0]；先使用 None。
        record = matches[0] if one_only else None

        # 只有唯一记录且 index 合法时，才拿同 index 的 Magiccon 交叉核对。
        magic = magiccon_records[expected_index] if 0 <= expected_index < len(magiccon_records) else None

        # 分项检查，失败时报告能直接指出是哪条假设坏了。
        checks = {
            "仅一条该class": one_only,
            "FIRTTECH索引正确": bool(record and record.index == expected_index),
            "FIRTTECH_ID正确": bool(record and record.record_id == expected_id),
            "FIRTTECH名称正确": bool(record and record.name == expected_name),
            "FIRTTECH作者说明为空": bool(record and record.description == ""),
            "FIRTTECH_selector为0": bool(record and record.selector == 0),
            "Magiccon同索引ID正确": bool(magic and magic.record_id == expected_id),
            "Magiccon同索引名称正确": bool(magic and magic.name == expected_name),
        }

        # 当前 class 的所有分项都 True 才算 PASS。
        class_pass = all(checks.values())

        # 保存人类可读检查结果。
        record_checks.append(
            {
                "class": action_class,
                "预期索引": expected_index,
                "预期ID": f"0x{expected_id:X}",
                "预期名称": expected_name,
                "命中数量": len(matches),
                "分项": checks,
                "PASS": class_pass,
            }
        )

        # 如果记录存在，就把所有本轮关心字段写入 CSV 数据行。
        if record is not None and magic is not None:
            # 先放最容易阅读的身份字段。
            row: Dict[str, object] = {
                "class": action_class,
                "FIRTTECH索引": record.index,
                "FIRTTECH_ID": f"0x{record.record_id:X}",
                "FIRTTECH名称": record.name,
                "FIRTTECH作者说明": record.description,
                "FIRTTECH_+0x30_selector": record.selector,
                "Magiccon索引": magic.index,
                "Magiccon_ID": f"0x{magic.record_id:X}",
                "Magiccon名称": magic.name,
            }
            # 把 +0x20..+0x60 原字段追加到同一行。
            for key, value in record.raw_fields.items():
                row[f"FIRTTECH_{key}"] = value
            # 再追加效果槽原值。
            for key, value in record.effect_fields.items():
                row[f"FIRTTECH_{key}"] = value
            # 再追加后部字段。
            for key, value in record.extra_fields.items():
                row[f"FIRTTECH_{key}"] = value
            # 最后追加 Magiccon 数值。
            for key, value in magic.numeric_fields.items():
                row[f"Magiccon_{key}"] = value
            # 当前行加入输出列表。
            rare_rows.append(row)

    # 两类各一条的结构约束。
    count_pass = class_counts.get(5) == 1 and class_counts.get(6) == 1
    # 所有具体 record 检查也必须通过。
    records_pass = all(item["PASS"] for item in record_checks)

    # 返回资源分析，同时把 CSV 行暂存在内部字段，主函数写完 CSV 后不会删掉 JSON 中的可追溯数据。
    return {
        "输入": {
            "Firttech.enc": str(firttech_path),
            "Magiccon.enc": str(magiccon_path),
        },
        "哈希检查": [
            verify_hash(
                "Firttech.enc加密文件",
                sha256_file(firttech_path),
                EXPECTED_FIRTTECH_ENCODED_SHA256,
            ),
            verify_hash(
                "Firttech.enc解密明文",
                sha256_bytes(firttech_decoded),
                EXPECTED_FIRTTECH_DECODED_SHA256,
            ),
            verify_hash(
                "Magiccon.enc加密文件",
                sha256_file(magiccon_path),
                EXPECTED_MAGICCON_ENCODED_SHA256,
            ),
            verify_hash(
                "Magiccon.enc解密明文",
                sha256_bytes(magiccon_decoded),
                EXPECTED_MAGICCON_DECODED_SHA256,
            ),
        ],
        "FIRTTECH_class分布": class_counts_json,
        "class5与class6记录检查": record_checks,
        "稀有记录字段": rare_rows,
        "PASS": count_pass and records_pass,
    }


def build_static_conclusions(machine: Dict[str, object], resources: Dict[str, object], call_hits: Sequence[int]) -> Dict[str, object]:
    """把已验证的机器事实整理成兼容实现可直接使用的边界结论。"""

    # 这些文字不是靠招式名猜的，而是把前面已经验证的控制流事实翻成实现规则。
    conclusions = {
        "入口分派": (
            "0x422AB0 每目标分派对 class5 与 class6 都只执行一次 0x42A7A0；"
            "第二次 0x42A7A0 的专用门只属于 class4。"
        ),
        "0x42C770假专用分支边界": (
            "对入口 class5/class6 而言，0x42C773/0x42C778 的 class5/class6 比较不可达："
            "0x42C6F9 已在 class!=0 时跳到 0x42C7A9。不能把这两条比较解释成入口类五/六专用计算。"
        ),
        "实际计算路径": (
            "canonical class5『潛地蟄伏』与 class6『鐵藤纏護』的 FIRTTECH+0x30 都为0；"
            "它们绕过 class4 固定返回后进入 selector 默认 helper 0x42B5F0，随后继续通用后处理。"
        ),
        "class6下游特判一": (
            "在 0x420F52 所在普通 FIRTTECH 结果应用分支中，class6 于 0x420F5C 被排除，"
            "从而跳过 target+0xCE8 与 target+0xDE0 两项运行时元数据写入。"
            "注意 action+0xDD0>0 的另一条上游分支不受这个 class6 门控制，因此不能泛化为『任何场景永不写』。"
        ),
        "class6下游特判二": (
            "结果阶段 0x4215A7 对 class6 直接跳到 0x42160D，因此跳过该段原本可能调用的 0x446DF0。"
            "0x446DF0 可静态证明是固定50槽请求队列写入器；这里中性称为『战斗结果显示请求队列』，"
            "不擅自扩大成『class6没有任何视觉/动画』。"
        ),
        "class5下游": (
            "在本轮已闭合的两个可达特判点中没有 class5 排除；class5 沿通用下游条件执行。"
            "这不等于已经证明作者把 class5 定义成某个正式业务枚举。"
        ),
        "作者术语边界": (
            "两条 canonical 稀有记录的作者说明字段均为空；因此 class5/class6 的作者正式枚举名继续记 UNKNOWN，"
            "不得仅依据『潛地蟄伏』『鐵藤纏護』名称自行命名。"
        ),
        "Godot兼容规则": (
            "canonical class5/class6 计算应复现非零、非class4、selector0的通用计算路径；"
            "class6 还必须按原版门控抑制上述两项特定下游副作用。只抑制已证明的元数据写入与该结果显示请求，"
            "不要未经证据关闭其他动画、音频或状态效果。"
        ),
    }

    # 静态结论成立的基础是机器证据、资源证据和446DF0调用面都通过。
    calls_pass = tuple(call_hits) == EXPECTED_446DF0_CALLS
    overall = bool(machine.get("PASS")) and bool(resources.get("PASS")) and calls_pass

    # 返回结论和适用边界。
    return {
        "结论": conclusions,
        "0x446DF0直接CALL地址": [f"0x{va:08X}" for va in call_hits],
        "0x446DF0预期直接CALL地址": [f"0x{va:08X}" for va in EXPECTED_446DF0_CALLS],
        "0x446DF0调用面检查PASS": calls_pass,
        "范围限制": [
            "本节点闭合的是 canonical class5/class6 的可达计算分派与两个已定位的 class6 下游特判。",
            "没有闭合 class4 场域建立/容器/消失完整链，也没有闭合 class5/class6 作者正式枚举名。",
            "没有证明 class6 不产生任何视觉、动画或音频；只证明它跳过 0x4215AD..0x421608 这一结果显示请求块。",
            "0x420F5C 的元数据抑制结论限定于到达 0x420F52 的普通 FIRTTECH 分支；action+0xDD0>0 另有写入路径。",
        ],
        "PASS": overall,
    }



def scan_text_literal_positions(image: PeImage, literal: bytes) -> List[int]:
    """在 .text 原始机器码里逐字节寻找一个固定字节串，并返回每次命中的 VA。"""

    # 先取得 .text 的 section 元数据。我们只在代码区搜索，避免 .rdata 里同样的数字干扰结果。
    text = image.text_section()
    # section 的硬盘起点加 RawSize 就得到本次扫描终点。
    start = text.raw_offset
    end = start + text.raw_size
    # 把代码区切成一个独立 bytes，后面的 find 不会越到别的 section。
    blob = image.data[start:end]
    # .text 的运行时起始 VA = ImageBase + section RVA。
    text_va = image.image_base + text.virtual_address
    # 用列表保存所有命中；完整集合比单纯“至少有一次”更适合负证据审计。
    hits: List[int] = []
    # pos 是下一次搜索从哪里开始。每次命中后只前进1字节，允许理论上的重叠匹配。
    pos = 0
    while True:
        # bytes.find 返回相对 blob 的字节索引；找不到时返回 -1。
        found = blob.find(literal, pos)
        if found < 0:
            break
        # 把相对索引换成运行时 VA，便于直接对照反汇编地址。
        hits.append(text_va + found)
        # 从命中字节的下一位继续找，保证不会漏掉相邻候选。
        pos = found + 1
    # 返回稳定的地址列表。
    return hits


def verify_solid64_extension(original: PeImage, research: PeImage) -> Dict[str, object]:
    """验证固化64新增的动作状态路由、来源依赖字段和BC0/BC6消费者。"""

    # 第一层：逐段核对关键机器码哈希，同时要求研究版与完全原版逐字节一致。
    slice_checks: List[Dict[str, object]] = []
    for start_va, end_va, label, expected_hash in SOLID64_CRITICAL_SLICES:
        # 计算区间长度。结束地址按“不包含”处理，所以直接相减即可。
        size = end_va - start_va
        # 从完全原版读取这一整段字节。
        original_bytes = original.read_va(start_va, size)
        # 从研究版读取同地址、同长度的字节。
        research_bytes = research.read_va(start_va, size)
        # 原版哈希用于防止地址错一字节还不自知。
        actual_hash = sha256_bytes(original_bytes)
        # 分项PASS要求：原版哈希命中 + 双版本字节完全相同。
        item_pass = actual_hash == expected_hash and original_bytes == research_bytes
        slice_checks.append({
            "名称": label,
            "范围": f"0x{start_va:08X}..0x{end_va:08X}",
            "原版SHA256": actual_hash,
            "预期SHA256": expected_hash,
            "原版与研究版逐字节一致": original_bytes == research_bytes,
            "PASS": item_pass,
        })

    # 第二层：对三个重要结构位移做整个 .text 的完整字节面穷举。
    # x86 little-endian 下 +0xCE8 写成 E8 0C 00 00。
    ce8_hits = scan_text_literal_positions(original, struct.pack("<I", 0x0CE8))
    # +0xDE0 写成 E0 0D 00 00。
    de0_hits = scan_text_literal_positions(original, struct.pack("<I", 0x0DE0))
    # +0x10C8 写成 C8 10 00 00。
    state_hits = scan_text_literal_positions(original, struct.pack("<I", 0x10C8))

    # 精确比较完整集合；数量或任何一个地址变化都会让工具失败。
    ref_checks = {
        "+0xCE8完整位移字面命中": {
            "实际": [f"0x{x:08X}" for x in ce8_hits],
            "预期": [f"0x{x:08X}" for x in EXPECTED_CE8_DISPLACEMENT_HITS],
            "PASS": tuple(ce8_hits) == EXPECTED_CE8_DISPLACEMENT_HITS,
        },
        "+0xDE0完整位移字面命中": {
            "实际": [f"0x{x:08X}" for x in de0_hits],
            "预期": [f"0x{x:08X}" for x in EXPECTED_DE0_DISPLACEMENT_HITS],
            "PASS": tuple(de0_hits) == EXPECTED_DE0_DISPLACEMENT_HITS,
        },
        "+0x10C8完整位移字面命中": {
            "实际": [f"0x{x:08X}" for x in state_hits],
            "预期": [f"0x{x:08X}" for x in EXPECTED_10C8_DISPLACEMENT_HITS],
            "PASS": tuple(state_hits) == EXPECTED_10C8_DISPLACEMENT_HITS,
        },
    }

    # 第三层：冻结几条非常短但关键的控制流字节，避免只靠大切片哈希让人工审查不直观。
    # 先冻结 0x443362 开始的“class2 / class1 / class4 提前跳共享分支”判断。
    # 这一步非常重要：它证明后面虽然还有一段“再次 cmp class4 -> 写0xBC7”的字面代码，
    # 但入口 class4 根本到不了那里。初版调查器曾漏掉这个可达性关系，已作为FAIL历史保留。
    shared_class_gate = original.read_va(0x00443362, 0x14)

    # 0x44341F 起是 class1/2/4 最终汇合的共享分支；0x443427 明确写 +0x10C8=0xBBC。
    shared_class_write = original.read_va(0x0044341F, 0x1B)

    # 0x4433B1 的第二个 class4 判断及 0xBC7 写入确实存在于机器码中，
    # 但因为更早的 class4 已经跳到 0x4433ED，所以这里只作为“不可达冗余字面分支”冻结。
    unreachable_class4_literal = original.read_va(0x004433B1, 0x1B)

    # 0x44338F: cmp eax,5；随后 class5 把 +0x10C8 写成0xBC0。
    class5_gate = original.read_va(0x0044338F, 0x11)
    # 0x4433A0: cmp eax,6；随后 class6 把 +0x10C8 写成0xBC6。
    class6_gate = original.read_va(0x004433A0, 0x11)
    # 0x442D10 / 0x442D1B: BC6、BC0 都直接跳过通用 7D6->7D1 收尾。
    finish_gate = original.read_va(0x00442D10, 0x16)
    # 这些 hex 是直接从完全原版机器码冻结下来的精确序列。
    short_checks = {
        "class1_2_4提前跳共享分支": {
            "实际HEX": shared_class_gate.hex(),
            "预期HEX": "83f8020f848200000083f801747d83f804747851",
            "PASS": shared_class_gate.hex() == "83f8020f848200000083f801747d83f804747851",
        },
        "class1_2_4共享分支写BBC": {
            "实际HEX": shared_class_write.hex(),
            "预期HEX": "6a038d8f8c100000c787c8100000bc0b0000e84a0bfeffc787c410",
            "PASS": shared_class_write.hex() == "6a038d8f8c100000c787c8100000bc0b0000e84a0bfeffc787c410",
        },
        "BC7字面分支存在但入口class4不可达": {
            "实际HEX": unreachable_class4_literal.hex(),
            "预期HEX": "83f804750cc787c8100000c70b0000eb0ac787c8100000bb0b0000",
            "PASS": (
                unreachable_class4_literal.hex()
                == "83f804750cc787c8100000c70b0000eb0ac787c8100000bb0b0000"
                and UNREACHABLE_CLASS4_LITERAL_STATE == 0x0BC7
            ),
        },
        "class5写BC0": {
            "实际HEX": class5_gate.hex(),
            "预期HEX": "83f805750cc787c8100000c00b0000eb2c",
            "PASS": class5_gate.hex() == "83f805750cc787c8100000c00b0000eb2c",
        },
        "class6写BC6": {
            "实际HEX": class6_gate.hex(),
            "预期HEX": "83f806750cc787c8100000c60b0000eb1b",
            "PASS": class6_gate.hex() == "83f806750cc787c8100000c60b0000eb1b",
        },
        "BC6与BC0阻挡通用结束门": {
            "实际HEX": finish_gate.hex(),
            "预期HEX": "3dc60b00000f848f0000003dc00b00000f8484000000",
            "PASS": finish_gate.hex() == "3dc60b00000f848f0000003dc00b00000f8484000000",
        },
    }

    # 第四层：把机器事实翻译成Godot兼容实现的中性协议。
    # 注意这里明确区分“机器语义已证”与“作者正式字段/枚举名未知”。
    conclusions = {
        "第三处class5_6专属分派": (
            "0x443310角色动作启动链先把class1/2/4提前分流到共享分支，并在0x443427把+0x10C8写为0xBBC；"
            "class5写0xBC0，class6写0xBC6，其它普通非1/2/4/5/6路径写0xBBB。"
            "后段虽字面存在class4->0xBC7分支，但入口class4已在0x443370提前跳走，因此该分支在当前入口路径不可达。"
        ),
        "BC0_BC6结束门": (
            "Battle主循环到达通用动作收尾条件时，+0x10C8为0xBC6或0xBC0都会在0x442D10/0x442D1B"
            "直接跳过通常的0x7D6->0x7D1转换。兼容实现必须保留这两个状态的特殊驻留/收尾语义。"
        ),
        "来源依赖字段协议": (
            "FightRole+0xCE8默认0xFA0，+0xDE0默认-1。普通FIRTTECH结果应用会把目标+0xCE8置0xFA1，"
            "+0xDE0记录来源角色槽；后续0x422530与0x4226F1..0x42282B用二者反查来源角色的状态与资源，"
            "形成目标对来源角色的运行时依赖/联动判定。class6在固化56已知门0x420F5C跳过这一绑定。"
        ),
        "class6当前状态消费者": (
            "独立的FightRole当前状态+0xCE4若为0xBC6，0x421503会把当前目标第一结果DWORD归零；"
            "0x423747还会选择独立的0x72动作请求，而不是该分支通常的0x6C/0x6D请求。"
            "本节点没有证明+0x10C8到+0xCE4的具体转移函数，因此只冻结两层状态码共同存在的事实。"
        ),
        "class5当前状态消费者": (
            "独立的FightRole当前状态+0xCE4若为0xBC0，0x445FB0的一条角色候选/缓存构建链会跳过该角色。"
            "由于0x445FB0的最高层业务名称尚未闭合，不把它扩大命名为绝对不可选中/不可攻击。"
        ),
        "作者术语边界": (
            "0xBC0/0xBC6、+0x10C8、+0xCE8、+0xDE0的作者正式枚举/字段名仍UNKNOWN。"
            "兼容层可中性命名为action_state_code、source_dependency_marker、source_role_slot，"
            "但文档必须保留这是兼容命名。"
        ),
    }

    # 汇总所有机器硬断言。
    slices_pass = all(item["PASS"] for item in slice_checks)
    refs_pass = all(item["PASS"] for item in ref_checks.values())
    shorts_pass = all(item["PASS"] for item in short_checks.values())
    return {
        "关键切片": slice_checks,
        "完整直接位移引用面": ref_checks,
        "短控制流断言": short_checks,
        "兼容实现结论": conclusions,
        "范围限制": [
            "完整位移引用面是对当前同版原版.exe .text内4字节位移字面量的穷举，并由独立objdump指令边界证据交叉核对。",
            "没有证明运行时计算地址、间接函数指针或其它游戏版本不存在额外访问。",
            "+0x10C8与FightRole+0xCE4共享0xBC0/0xBC6码值，但本节点尚未闭合二者之间的具体转移函数。",
            "0x445FB0只中性称为角色候选/缓存构建链，不越界命名为正式目标选择器。",
        ],
        "PASS": slices_pass and refs_pass and shorts_pass,
    }


def write_csv(path: Path, rows: Sequence[Dict[str, object]]) -> None:
    """把 class5/class6 两条资源交叉核对写成严格 CSV。"""

    # 输出目录可能还不存在，所以先递归创建。
    path.parent.mkdir(parents=True, exist_ok=True)

    # 没有行时无法决定表头，这代表前面的资源解析已经严重异常。
    if not rows:
        raise ValueError("没有 class5/class6 资源行，无法生成 CSV")

    # 第一行字典的 key 顺序就是我们希望的 CSV 列顺序。
    fieldnames = list(rows[0].keys())

    # newline='' 是 Python csv 模块官方推荐写法，防止 Windows 出现多余空行。
    # utf-8-sig 带 BOM，用户在简体中文 Windows/Excel 直接打开时更稳妥。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        # DictWriter 按 fieldnames 把每个字典值放到固定列。
        writer = csv.DictWriter(handle, fieldnames=fieldnames, extrasaction="ignore")
        # 写表头。
        writer.writeheader()
        # 逐行写数据。
        for row in rows:
            writer.writerow(row)


def write_json(path: Path, payload: Dict[str, object]) -> None:
    """用稳定、可读的 UTF-8 格式写 JSON 报告。"""

    # 先创建父目录，避免 open 因目录不存在失败。
    path.parent.mkdir(parents=True, exist_ok=True)
    # ensure_ascii=False 让中文直接可读；indent=2 方便人工 diff。
    text = json.dumps(payload, ensure_ascii=False, indent=2, sort_keys=False)
    # 固定 LF 写入即可；Markdown/数据文件不受 BAT/CMD 的 CRLF 特殊约束。
    path.write_text(text + "\n", encoding="utf-8")


def build_parser() -> argparse.ArgumentParser:
    """定义命令行参数。"""

    # description 会出现在 --help，确保单独拿到脚本也知道用途。
    parser = argparse.ArgumentParser(
        description="验证《幽城幻剑录》Battle class5/class6动作状态路由、BC0/BC6结束门与class6来源依赖字段生命周期。"
    )

    # 原版 EXE 必须显式传入，避免工具偷偷使用当前目录里不确定的文件。
    parser.add_argument("--original-rpg", required=True, type=Path, help="完全未修改原版 RPG.exe")
    # 研究版 EXE 用于关键范围双版本比对。
    parser.add_argument("--research-rpg", required=True, type=Path, help="历史研究版 RPG.exe")
    # canonical Firttech.enc。
    parser.add_argument("--firttech", required=True, type=Path, help="canonical Public/Firttech.enc")
    # canonical Magiccon.enc。
    parser.add_argument("--magiccon", required=True, type=Path, help="canonical Public/Magiccon.enc")
    # JSON 是机器可重放的主报告。
    parser.add_argument("--json-out", required=True, type=Path, help="输出 JSON 报告路径")
    # CSV 是给人直接核对两条稀有记录的表格。
    parser.add_argument("--csv-out", required=True, type=Path, help="输出 class5/class6 资源字段 CSV 路径")

    # 返回配置好的 parser。
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    """执行完整固化64验证流程；成功返回0，任一硬断言失败返回2。"""

    # 解析命令行参数。
    args = build_parser().parse_args(argv)

    # 为了报错更清楚，先逐个检查输入文件存在。
    for label, path in (
        ("原版RPG.exe", args.original_rpg),
        ("研究版RPG.exe", args.research_rpg),
        ("Firttech.enc", args.firttech),
        ("Magiccon.enc", args.magiccon),
    ):
        # is_file 同时排除“路径存在但其实是目录”的情况。
        if not path.is_file():
            raise FileNotFoundError(f"{label} 不存在或不是普通文件：{path}")

    # 第一组输入校验：完整 EXE 哈希。
    original_hash = sha256_file(args.original_rpg)
    research_hash = sha256_file(args.research_rpg)
    exe_hash_checks = [
        verify_hash("完全未修改原版RPG.exe", original_hash, EXPECTED_ORIGINAL_RPG_SHA256),
        verify_hash("历史研究版RPG.exe", research_hash, EXPECTED_RESEARCH_RPG_SHA256),
    ]

    # 解析两份 PE，以后所有 VA 都通过 section 表换算，不直接把 VA 当文件偏移。
    original_image = parse_pe(args.original_rpg)
    research_image = parse_pe(args.research_rpg)

    # 解码两份资源并解析所有记录。
    firttech_decoded, firttech_records = parse_firttech(args.firttech)
    magiccon_decoded, magiccon_records = parse_magiccon(args.magiccon)

    # 验证资源分布与两条稀有记录。
    resource_report = analyze_resources(
        args.firttech,
        args.magiccon,
        firttech_decoded,
        firttech_records,
        magiccon_decoded,
        magiccon_records,
    )

    # 资源哈希必须单独并入 PASS；analyze_resources 的结构 PASS 只代表记录约束。
    resource_hash_pass = all(item["PASS"] for item in resource_report["哈希检查"])
    # 把最终资源 PASS 改成“结构 + 哈希”都通过。
    resource_report["PASS"] = bool(resource_report["PASS"]) and resource_hash_pass

    # 验证关键机器码与双版本等价范围。
    machine_report = verify_critical_machine_code(original_image, research_image)

    # 固化64新增：验证角色动作状态路由、来源依赖字段和BC0/BC6消费者。
    solid64_extension = verify_solid64_extension(original_image, research_image)

    # 扫描原版 .text 中所有直接 CALL 0x446DF0。
    call_hits = scan_direct_calls(original_image, 0x00446DF0)

    # 把机器与资源事实整理为实现边界。
    conclusion_report = build_static_conclusions(machine_report, resource_report, call_hits)

    # EXE 哈希也必须全部正确。
    exe_pass = all(item["PASS"] for item in exe_hash_checks)
    # 最终 PASS 需要输入、资源、机器码、结论四层同时成立。
    overall_pass = (
        exe_pass
        and bool(resource_report["PASS"])
        and bool(machine_report["PASS"])
        and bool(conclusion_report["PASS"])
        and bool(solid64_extension["PASS"])
    )

    # 组装主 JSON。路径保留是为了以后知道当时用的哪份输入；复读比较时可做路径归一化。
    report: Dict[str, object] = {
        "工具": "youcheng_battle_class56_action_state_inspector.py",
        "工具版本": TOOL_VERSION,
        "固化节点": 64,
        "目标": "在固化56基础上闭合class5/class6第三处角色动作状态路由、BC0/BC6结束门，以及class6跳过的来源依赖元数据生命周期。",
        "输入": {
            "原版RPG.exe": str(args.original_rpg),
            "研究版RPG.exe": str(args.research_rpg),
            "Firttech.enc": str(args.firttech),
            "Magiccon.enc": str(args.magiccon),
        },
        "EXE哈希检查": exe_hash_checks,
        "机器码证据": machine_report,
        "固化64新增机器与状态证据": solid64_extension,
        "资源证据": resource_report,
        "静态闭合结论": conclusion_report,
        "总体PASS": overall_pass,
    }

    # 先写 CSV，便于即使 JSON 后续人工查看也有独立表格证据。
    write_csv(args.csv_out, resource_report["稀有记录字段"])
    # 再写 JSON 主报告。
    write_json(args.json_out, report)

    # 控制台输出保持简洁，只报告总体结果与产物位置。
    print(f"[固化64] 总体结果：{'PASS' if overall_pass else 'FAIL'}")
    print(f"[固化64] JSON：{args.json_out}")
    print(f"[固化64] CSV ：{args.csv_out}")

    # Unix/Windows 命令行约定：0 表示成功，非0表示失败。
    return 0 if overall_pass else 2


# 只有直接运行脚本时才执行 main；如果未来被别的测试脚本 import，不会自动开始扫描文件。
if __name__ == "__main__":
    raise SystemExit(main())
