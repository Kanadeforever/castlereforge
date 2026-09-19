#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle FIRTTECH 类四分派静态调查器。

这个工具只做“读取和验证”，不会修改 RPG.exe，也不会修改 Firttech.enc。
它的目标非常窄：把固化55需要的三组证据自动化重放出来。

第一组证据：RPG.exe 的 0x422AB0 每目标动作分派里，FIRTTECH+0x18 == 4
为什么会调用两次 0x42A7A0，以及两次调用的参数是否真的相同。

第二组证据：0x42A7A0 转发到 0x42C680 后，0x42C680 对 class 4
究竟会不会抽随机数、会不会调用其他公式 helper、会往 0x50 字节 DamageInfo
写入哪些确定值。

第三组证据：当前 canonical Public/Firttech.enc 的 500 条记录里，class 4
实际有哪些招式，它们的作者名称/说明文字呈现怎样的业务共同点。

本文件刻意写了非常细的中文注释。阅读时可以把“VA”理解为 EXE 加载到内存后
使用的地址，把“文件偏移”理解为这个地址在硬盘上的 EXE 文件里对应第几个字节。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple


# 工具版本只用于输出报告，方便以后看到 JSON 时知道它由哪一版脚本生成。
TOOL_VERSION = "0.7D-solid55.0"

# 这是用户确认的“完全未修改原版” RPG.exe 的 SHA-256。
# 固化55开始，它可以作为可信原版基线参与静态复核。
EXPECTED_ORIGINAL_RPG_SHA256 = (
    "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
)

# 这是此前连续多个固化节点一直使用的研究版 RPG.exe 的 SHA-256。
# 它和原版只有极少数字节不同。固化55不把它丢掉，而是让两份 EXE 互相复核。
EXPECTED_RESEARCH_RPG_SHA256 = (
    "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
)

# 当前 multimedia.zip 里的 Firttech.enc 固定是 500 条记录，每条 0x230 字节。
# 500 * 0x230 = 280000 字节；这个几何关系已经被前代多个工具反复验证。
FIRTTECH_RECORD_COUNT = 500
FIRTTECH_RECORD_SIZE = 0x230

# FIRTTECH 记录中本轮直接使用的字段偏移。
# +0x00：记录 ID。
# +0x04..+0x23：CP950 名称字符串区域。
# +0x18：本轮研究的动作 class/mode 枚举字段。
# +0x30/+0x3C/+0x44：用于观察 class 4 语料共同值，不在本轮强命名作者正式字段名。
# +0x1E8：0x42C680 最终可传播到 DamageInfo+0x44 的字段。
# +0x17C..+0x22F：作者说明文字区域。
FIRTTECH_OFF_ID = 0x00
FIRTTECH_OFF_NAME = 0x04
FIRTTECH_NAME_SIZE = 0x20
FIRTTECH_OFF_CLASS = 0x18
FIRTTECH_OFF_30 = 0x30
FIRTTECH_OFF_3C = 0x3C
FIRTTECH_OFF_44 = 0x44
FIRTTECH_OFF_1E8 = 0x1E8
FIRTTECH_OFF_DESCRIPTION = 0x17C
FIRTTECH_DESCRIPTION_SIZE = 0x230 - 0x17C

# 当前 canonical Firttech.enc 的 encoded / decoded SHA-256。
# 这里把两者都固化，是为了防止“文件大小碰巧一样但内容换了”的情况。
EXPECTED_FIRTTECH_ENCODED_SHA256 = (
    "32b98193c037c8cd75a11534520e189c89c856c367bc1d6ac607e43d9cf52a5d"
)
EXPECTED_FIRTTECH_DECODED_SHA256 = (
    "5469a9dd388972b53b5661e90d85c84f2d0150d06f46ddbae159409a5ac34b1a"
)

# 固化55最关键的三段代码范围。
# 第一个范围包含 class 分派与 class 4 的第二次调用。
# 第二个范围是薄包装 0x42A7A0，它把 5 个参数转发给 0x42C680。
# 第三个范围包含 0x42C680 完整主体以及 class 4 的 0x42CC70 固定输出分支。
RELEVANT_VA_RANGES: Tuple[Tuple[int, int, str], ...] = (
    (0x00422AB0, 0x00422CAF, "每目标动作分派与类四双调用"),
    (0x0042A7A0, 0x0042A7C1, "FIRTTECH五参数转发包装"),
    (0x0042C680, 0x0042CC94, "FIRTTECH主处理器与类四固定输出分支"),
)


@dataclass(frozen=True)
class Section:
    """PE 文件中的一个 section 映射。

    为什么需要这个结构：
    objdump/Ghidra 显示的是“内存 VA”，但 Python 直接读取 EXE 时拿到的是“文件偏移”。
    PE section 表告诉我们两者怎样换算。
    """

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass(frozen=True)
class PeImage:
    """把一个 RPG.exe 的原始字节和 PE section 映射放在一起。"""

    path: Path
    data: bytes
    image_base: int
    sections: Tuple[Section, ...]

    def va_to_file_offset(self, va: int) -> int:
        """把一个内存 VA 换算成 EXE 文件偏移。

        例如 .text 的 RVA 从 0x1000 开始，文件偏移也从 0x1000 开始；
        但不能依赖这种“刚好一样”的现象，所以仍按照 PE section 表严格计算。
        """

        # 先把 VA 去掉 image base，得到 RVA。
        rva = va - self.image_base

        # 逐个 section 看这个 RVA 落在哪一段。
        for section in self.sections:
            # virtual_size 是装入内存后的逻辑大小，raw_size 是文件里实际保存的大小。
            # 这里取较大值，只是为了安全覆盖 section 尾部的合法映射范围。
            span = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + span:
                # RVA 减掉 section 起点得到“在 section 内部走了多少字节”，
                # 再加 raw_offset 就得到硬盘文件中的真正偏移。
                return section.raw_offset + (rva - section.virtual_address)

        # 找不到说明调用者给了一个不属于当前 EXE section 的地址。
        raise ValueError(f"VA 0x{va:08X} 不在 {self.path.name} 的任何已加载 section 中")

    def read_va(self, va: int, size: int) -> bytes:
        """读取从 VA 开始的固定长度字节。"""

        offset = self.va_to_file_offset(va)
        end = offset + size
        if end > len(self.data):
            raise ValueError(
                f"读取 0x{va:08X}+0x{size:X} 会越过 {self.path.name} 文件末尾"
            )
        return self.data[offset:end]


@dataclass(frozen=True)
class FirttechRecord:
    """只保存固化55需要的 FIRTTECH 字段，不假装已经知道全部字段业务名。"""

    index: int
    record_id: int
    name: str
    action_class: int
    field_30: int
    field_3c: int
    field_44: int
    field_1e8: int
    description: str


def sha256_bytes(data: bytes) -> str:
    """计算一段 bytes 的 SHA-256 十六进制字符串。"""

    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """读取整个文件并计算 SHA-256。

    RPG.exe 只有约 452 KiB，本项目输入远小于需要流式分块的体量，直接读取更直观。
    """

    return sha256_bytes(path.read_bytes())


def read_u16(data: bytes, offset: int) -> int:
    """按 little-endian 读取 16 位无符号整数。"""

    return struct.unpack_from("<H", data, offset)[0]


def read_u32(data: bytes, offset: int) -> int:
    """按 little-endian 读取 32 位无符号整数。"""

    return struct.unpack_from("<I", data, offset)[0]


def read_s32(data: bytes, offset: int) -> int:
    """按 little-endian 读取 32 位有符号整数。"""

    return struct.unpack_from("<i", data, offset)[0]


def parse_pe(path: Path) -> PeImage:
    """只解析本工具需要的最小 PE 信息：image base 与 section 表。

    这里故意不用第三方 pefile，原因不是 pefile 不好，而是固化包希望尽量只依赖
    Python 标准库，用户以后在新环境里拿到单个包也能直接重放。
    """

    data = path.read_bytes()

    # DOS 头 0x3C 保存 PE header 的文件偏移。
    if len(data) < 0x40:
        raise ValueError(f"{path} 太小，不可能是本项目的 PE EXE")
    pe_offset = read_u32(data, 0x3C)

    # PE header 必须以 ASCII 'PE\0\0' 开头。
    if data[pe_offset:pe_offset + 4] != b"PE\x00\x00":
        raise ValueError(f"{path} 的 PE 签名不正确")

    # COFF header 中 +6 是 section 数，+20 是 optional header 大小。
    number_of_sections = read_u16(data, pe_offset + 6)
    optional_header_size = read_u16(data, pe_offset + 20)

    # PE32 optional header 的 +28 是 ImageBase。幽城 RPG.exe 是 32 位 PE。
    optional_header_offset = pe_offset + 24
    magic = read_u16(data, optional_header_offset)
    if magic != 0x10B:
        raise ValueError(
            f"{path} optional header magic=0x{magic:04X}，不是预期 PE32(0x10B)"
        )
    image_base = read_u32(data, optional_header_offset + 28)

    # section table 紧接 optional header。
    section_table_offset = optional_header_offset + optional_header_size
    sections: List[Section] = []

    for index in range(number_of_sections):
        # 每条 IMAGE_SECTION_HEADER 固定 40 字节。
        entry = section_table_offset + index * 40
        raw_name = data[entry:entry + 8].split(b"\x00", 1)[0]
        name = raw_name.decode("ascii", errors="replace")

        # +8/+12/+16/+20 分别是 VirtualSize、VirtualAddress、SizeOfRawData、PointerToRawData。
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

    return PeImage(
        path=path,
        data=data,
        image_base=image_base,
        sections=tuple(sections),
    )


def decode_cp950_cstring(raw: bytes) -> str:
    """把固定宽度 CP950 字符串区域解成 Python 字符串。

    原数据是 C 风格字符串，所以第一个 0x00 后面的填充不属于文字。
    errors='replace' 只用于让损坏数据也能生成诊断；canonical 数据不应依赖替换符。
    """

    # 固定宽度字段后面通常会用 0x00 补满；真正的人类文字只到第一个 0x00 为止。
    # 如果不先截断，后面的零填充也会被送进解码器，得到一串没有意义的 NUL 字符。
    payload = raw.split(b"\x00", 1)[0]

    # 游戏原资源使用繁体中文时代常见的 CP950。
    # errors="replace" 的作用只是让坏样本也能输出诊断，而不是偷偷把坏字节当成正常数据。
    return payload.decode("cp950", errors="replace")


def decrypt_enc(encoded: bytes) -> bytes:
    """按本项目已经固化的旧式 ENC 规则逐字节 XOR 解密。

    第 i 个字节使用 i % 255 作为 XOR key。
    注意 key 是从整个文件开头连续计数，而不是每条 record 重新从 0 开始。
    """

    # enumerate 会同时给出“当前位置 index”和“这个位置原来的字节 value”。
    # index % 255 让 key 按 0,1,2,...,254,0,1,... 循环。
    # value ^ key 就是 XOR 解密；XOR 的特点是再做一次相同运算就能还原。
    # 最外层 bytes(...) 把逐个算出的整数重新收成一段真正的二进制数据。
    return bytes(value ^ (index % 255) for index, value in enumerate(encoded))


def parse_firttech(path: Path) -> Tuple[bytes, List[FirttechRecord]]:
    """读取、解密并解析 500 条 Firttech 记录。"""

    # 第一步只读打开 Firttech.enc。read_bytes 不会改写原文件。
    encoded = path.read_bytes()

    # 先用“记录数 × 每条大小”计算 canonical 文件应有的精确总长度。
    # 这是一道最早的保险：长度不对时，后面所有固定偏移都会错位，所以必须立刻停止。
    expected_size = FIRTTECH_RECORD_COUNT * FIRTTECH_RECORD_SIZE
    if len(encoded) != expected_size:
        raise ValueError(
            f"Firttech.enc 应为 {expected_size} 字节，实际 {len(encoded)} 字节"
        )

    # 文件几何正确后才进行 XOR 解码，得到真正可按结构读取的明文字节。
    decoded = decrypt_enc(encoded)

    # records 是最后返回给后续统计函数的“结构化记录列表”。
    # 一开始当然是空的，下面每循环一次就追加一条。
    records: List[FirttechRecord] = []

    # range(500) 会产生 0..499，正好逐条访问全部 canonical 记录，不跳过任何一条。
    for index in range(FIRTTECH_RECORD_COUNT):
        # 先切出当前 0x230 字节 record，后面的字段偏移全部相对这个 record 计算。
        # 第0条从0开始，第1条从0x230开始，第2条从0x460开始，以此类推。
        start = index * FIRTTECH_RECORD_SIZE

        # Python 切片的结束位置不包含在结果里，所以这里恰好拿到 0x230 字节。
        record = decoded[start:start + FIRTTECH_RECORD_SIZE]

        # 本轮只读取已经由代码 consumer 或成熟表结构确认的偏移。
        # 对 +0x30/+0x3C/+0x44/+0x1E8 只记录 raw 值，不在工具里擅自命名业务语义。
        records.append(
            FirttechRecord(
                index=index,
                record_id=read_u32(record, FIRTTECH_OFF_ID),
                name=decode_cp950_cstring(
                    record[
                        FIRTTECH_OFF_NAME:
                        FIRTTECH_OFF_NAME + FIRTTECH_NAME_SIZE
                    ]
                ),
                action_class=read_s32(record, FIRTTECH_OFF_CLASS),
                field_30=read_s32(record, FIRTTECH_OFF_30),
                field_3c=read_s32(record, FIRTTECH_OFF_3C),
                field_44=read_s32(record, FIRTTECH_OFF_44),
                field_1e8=read_s32(record, FIRTTECH_OFF_1E8),
                description=decode_cp950_cstring(
                    record[
                        FIRTTECH_OFF_DESCRIPTION:
                        FIRTTECH_OFF_DESCRIPTION + FIRTTECH_DESCRIPTION_SIZE
                    ]
                ),
            )
        )

    # 同时返回“整份解码后的原始字节”和“500条结构化记录”。
    # 前者方便做 SHA/原始证据，后者方便按字段统计。
    return decoded, records


def find_all(data: bytes, needle: bytes) -> List[int]:
    """返回 needle 在 data 中所有不重叠/可重叠起点。

    这里每次只向前移动 1 字节，哪怕两个模式发生重叠也不会漏掉第二个。
    """

    # positions 专门保存每一次命中的起点。
    positions: List[int] = []

    # 第一次当然要从整段数据的第0字节开始找。
    start = 0

    # 因为不知道会命中几次，所以使用“找到就继续、找不到才退出”的循环。
    while True:
        # bytes.find 返回下一个命中位置；完全找不到时返回 -1。
        position = data.find(needle, start)

        # -1 代表后面已经没有命中，循环任务完成。
        if position < 0:
            break

        # 把当前命中记录下来，不能只保留最后一次。
        positions.append(position)

        # 下一轮从“本次命中起点+1”继续，因此即使模式彼此重叠也不会漏掉。
        start = position + 1

    # 返回全部起点；若完全没找到，就是一个空列表。
    return positions


def bytes_hex(data: bytes) -> str:
    """把机器码转成适合写进 JSON 的大写十六进制字符串。"""

    # f"{value:02X}" 会把一个字节写成固定两位大写十六进制，例如 10 -> "0A"。
    # join 再用空格把每个字节连接起来，得到人类常见的机器码写法。
    return " ".join(f"{value:02X}" for value in data)


def verify_machine_patterns(image: PeImage) -> Dict[str, object]:
    """验证固化55依赖的关键机器码模式。

    不使用“只搜索一个短字节串然后猜地址”的方式，而是在已知 VA 读取完整局部字节。
    这样哪怕别处碰巧有同样三四个字节，也不会造成误判。
    """

    checks: List[Dict[str, object]] = []

    def add_exact_check(name: str, va: int, expected: bytes, meaning: str) -> None:
        """内部小函数：读取指定 VA，并和预期机器码逐字节比较。"""

        # expected 有多少字节，就从同一个 VA 读取多少字节；长度必须一模一样才能逐字节比较。
        actual = image.read_va(va, len(expected))

        # 不只保存 True/False，还把预期、实际和业务解释都写入报告。
        # 这样未来失败时能直接看到哪个字节变了，不必先修改工具再打印。
        checks.append(
            {
                "名称": name,
                "VA": f"0x{va:08X}",
                "预期机器码": bytes_hex(expected),
                "实际机器码": bytes_hex(actual),
                "含义": meaning,
                "PASS": actual == expected,
            }
        )

    # 0x422ADB：从当前 FIRTTECH record +0x18 读取动作 class。
    add_exact_check(
        "FIRTTECH类字段读取",
        0x00422ADB,
        bytes.fromhex("8B 40 18 89 44 24 0C"),
        "读取 [FIRTTECH+0x18] 并保存为本轮每目标分派 class。",
    )

    # 0x422B98：class==4 会进入与 1/2/5/6 共用的第一次 FIRTTECH processor 调用。
    add_exact_check(
        "类四进入第一次普通FIRTTECH调用",
        0x00422B98,
        bytes.fromhex("83 F9 04 74 33"),
        "class 4 与其他已列 class 一样先跳到 0x422BD0。",
    )

    # 第一次调用完整参数准备。这里把从读取 FIRTTECH 到 CALL 的全部局部序列锁住。
    add_exact_check(
        "类四第一次调用参数序列",
        0x00422BD0,
        bytes.fromhex(
            "8B 8E 48 08 00 00 "
            "57 51 "
            "05 4C 08 00 00 "
            "53 "
            "8D 96 4C 08 00 00 "
            "50 52 "
            "B9 94 01 8A 00 "
            "E8 B0 7B 00 00"
        ),
        "第一次把 flag、FIRTTECH、同一DamageInfo、target RoleDefinition、performer RoleDefinition 传给0x42A7A0。",
    )

    # 第一次返回后只重新读取 class 并判断是否为4；中间没有应用第一份 DamageInfo 的代码。
    add_exact_check(
        "类四第二次调用门",
        0x00422BF0,
        bytes.fromhex("8B 4C 24 14 83 F9 04 75 28"),
        "第一次返回后立刻检查 class==4；非4跳过第二次调用。",
    )

    # 第二次调用用不同寄存器重新取得同一 target 指针，但最终压栈的五个逻辑参数与第一次相同。
    add_exact_check(
        "类四第二次调用参数序列",
        0x00422BF9,
        bytes.fromhex(
            "8B 86 48 08 00 00 "
            "8B 0C AD 94 FD 89 00 "
            "57 50 "
            "81 C1 4C 08 00 00 "
            "53 "
            "8D 96 4C 08 00 00 "
            "51 52 "
            "B9 94 01 8A 00 "
            "E8 7F 7B 00 00"
        ),
        "第二次仍传同一flag、FIRTTECH、DamageInfo、target RoleDefinition、performer RoleDefinition；没有交换角色方向。",
    )

    # 0x42A7A0 是一个非常薄的包装，它只按相同顺序把五个栈参数重新 push 后调用 0x42C680。
    add_exact_check(
        "五参数原样转发包装",
        0x0042A7A0,
        bytes.fromhex(
            "8B 44 24 14 8B 54 24 10 50 "
            "8B 44 24 10 52 "
            "8B 54 24 10 50 "
            "8B 44 24 10 52 50 "
            "E8 C2 1E 00 00 C2 14 00"
        ),
        "0x42A7A0不改业务数据，只把五参数传给0x42C680，然后ret 0x14。",
    )

    # 0x42C680 读取 record+0x18，若等于4直接跳到 0x42CC70。
    add_exact_check(
        "主处理器类四直接分支",
        0x0042C7A9,
        bytes.fromhex("83 F8 04 0F 84 BE 04 00 00"),
        "FIRTTECH+0x18==4 时直接跳过常规公式/RNG/五槽处理，进入0x42CC70。",
    )

    # 这是 class 4 的整个专用尾分支直到 ret。
    # 注意这些写入全部落在 esi 指向的同一个 0x50-byte DamageInfo 中。
    class4_branch = bytes.fromhex(
        "C6 46 0C 00 "
        "C6 06 01 "
        "89 4E 38 "
        "89 4E 04 "
        "89 4E 3C "
        "89 4E 08 "
        "8B 97 E8 01 00 00 "
        "89 56 44 "
        "5F 5E 5D 5B "
        "83 C4 0C "
        "C2 14 00"
    )
    add_exact_check(
        "类四固定DamageInfo输出",
        0x0042CC70,
        class4_branch,
        "class4固定写success=1、damage/value相关DWORD=0、DamageInfo+0x44=FIRTTECH+0x1E8，然后直接返回。",
    )

    # class4 专用分支里确实存在一个数值字节 0xE8：
    # 它来自指令 `mov edx,[edi+0x1E8]` 的位移常量，而不是 CALL opcode。
    # 因此绝对不能用“原始字节中是否出现 0xE8”来判断有没有 CALL。
    # 固化55首轮工具就踩到了这个坑，所以这里改成更严格也更正确的办法：
    # 只有当整个分支逐字节等于上面已经人工按指令边界解码的完整序列时，
    # 才承认“这组已知指令中没有 CALL”。任何一个字节变化都会令本项失败并要求重新反汇编。
    class4_actual = image.read_va(0x0042CC70, len(class4_branch))
    class4_matches_known_no_call_sequence = class4_actual == class4_branch
    checks.append(
        {
            "名称": "类四专用分支无CALL",
            "VA范围": "0x0042CC70..0x0042CC93",
            "检查": "完整分支必须逐字节匹配已人工解码、确认无CALL的固定指令序列",
            "原始字节中0xE8的说明": "+0x1E8位移常量的一部分，不是CALL opcode",
            "含义": "该已知class4专用分支不调用0x451565 RNG，也不调用公式/五槽/helper。",
            "PASS": class4_matches_known_no_call_sequence,
        }
    )

    # 把所有子检查汇总成一个总 PASS。只要一个关键字节不同，就不能自动宣称固化55结论仍适用。
    return {
        "检查数": len(checks),
        "通过数": sum(1 for item in checks if item["PASS"]),
        "PASS": all(item["PASS"] for item in checks),
        "明细": checks,
    }


def compare_relevant_ranges(original: PeImage, research: PeImage) -> Dict[str, object]:
    """比较原版与研究版在固化55关键代码范围是否逐字节一致。"""

    # 每个 rows 元素对应一段关键地址范围，而不是对应单个字节。
    rows: List[Dict[str, object]] = []

    # 三元组里 start_va 是起点，end_va 是“不包含的末端”，meaning 是人类说明。
    for start_va, end_va, meaning in RELEVANT_VA_RANGES:
        # 半开区间 [start,end) 的长度就是 end-start。
        size = end_va - start_va

        # 分别从原版和研究版读取“完全相同 VA、完全相同长度”的代码。
        original_bytes = original.read_va(start_va, size)
        research_bytes = research.read_va(start_va, size)

        # 同时保存两边 SHA 和直接逐字节比较结果。
        # SHA 方便文档引用，布尔比较则避免把“哈希字符串写错”当成证据。
        rows.append(
            {
                "起始VA": f"0x{start_va:08X}",
                "结束VA_不含": f"0x{end_va:08X}",
                "长度": size,
                "含义": meaning,
                "原版SHA256": sha256_bytes(original_bytes),
                "研究版SHA256": sha256_bytes(research_bytes),
                "逐字节一致": original_bytes == research_bytes,
            }
        )

    # all(...) 要求三段都一致；任何一段不同，总PASS就必须失败。
    return {
        "范围数": len(rows),
        "PASS": all(row["逐字节一致"] for row in rows),
        "范围": rows,
    }


def map_file_offset_to_va(image: PeImage, file_offset: int) -> Tuple[Optional[int], Optional[str]]:
    """把一个文件偏移反向映射成 VA，用于生成两份 EXE 的完整差异清单。"""

    # 反向映射时要逐个 section 看“这个文件偏移是否落在它真正保存于磁盘的 raw 范围”。
    for section in image.sections:
        if section.raw_offset <= file_offset < section.raw_offset + section.raw_size:
            # delta 是“从这个 section 的文件起点往后走了多少字节”。
            delta = file_offset - section.raw_offset

            # 内存 VA = ImageBase + section的RVA起点 + section内部位移。
            va = image.image_base + section.virtual_address + delta
            return va, section.name

    # PE header、overlay 等位置不属于已加载 section；这时没有可靠 VA，就明确返回 None。
    return None, None


def compare_full_exe(original: PeImage, research: PeImage) -> Dict[str, object]:
    """逐字节列出原版与研究版的所有差异。

    本函数不是为了证明“整份 EXE 等价”。恰恰相反，它明确保存差异边界，避免后续研究
    忘记两份文件并不完全相同。固化55只对自己使用的关键代码范围要求逐字节一致。
    """

    # 逐字节 zip 比较要求两边长度相同。若长度先不同，不能假装只比较共同前缀就算完成。
    if len(original.data) != len(research.data):
        # 这里直接失败，并把两边大小写进报告，方便未来判断是不是换了版本。
        return {
            "同大小": False,
            "原版大小": len(original.data),
            "研究版大小": len(research.data),
            "差异": [],
            "PASS": False,
        }

    # diffs 只收真正不同的字节；相同的四十多万个字节没必要逐条写进 JSON。
    diffs: List[Dict[str, object]] = []

    # zip 把两份文件同位置字节配成一对；enumerate 再给出这个位置的文件偏移。
    for offset, (old_byte, new_byte) in enumerate(zip(original.data, research.data)):
        # 两边一样就继续看下一个位置。
        if old_byte == new_byte:
            continue

        # 对于真正不同的位置，再把文件偏移翻译成 VA/section，方便逆向人员在反汇编器里定位。
        va, section_name = map_file_offset_to_va(original, offset)

        # 每个差异都同时保存原字节与研究版字节，避免只有“总共10处”却不知道改了什么。
        diffs.append(
            {
                "文件偏移": f"0x{offset:06X}",
                "VA": None if va is None else f"0x{va:08X}",
                "section": section_name,
                "原版字节": f"0x{old_byte:02X}",
                "研究版字节": f"0x{new_byte:02X}",
            }
        )

    return {
        "同大小": True,
        "原版大小": len(original.data),
        "研究版大小": len(research.data),
        "差异字节数": len(diffs),
        "差异": diffs,
        # 这里的 PASS 只表示“成功完成完整差异枚举且差异数量符合当前已知10字节基线”。
        # 它绝不表示两份 EXE 完全相同。
        "PASS": len(diffs) == 10,
    }


def class_distribution(records: Sequence[FirttechRecord]) -> Dict[str, int]:
    """统计 500 条 FIRTTECH 中每个 +0x18 class 出现多少次。"""

    # counts 的 key 是 class 数字，value 是这个 class 已经见过多少条。
    counts: Dict[int, int] = {}

    # 必须遍历全部500条，而不是只扫描我们预期的7条 class4。
    for record in records:
        # dict.get(...,0) 表示第一次看见这个 class 时从0开始计数。
        counts[record.action_class] = counts.get(record.action_class, 0) + 1

    # JSON object 的 key 最终是字符串，主动转成十进制字符串能让输出稳定清楚。
    return {str(key): counts[key] for key in sorted(counts)}


def collect_class4(records: Sequence[FirttechRecord]) -> List[FirttechRecord]:
    """筛出本轮研究的 +0x18 == 4 记录。"""

    # 列表推导式逐条检查 action_class；只有等于4的记录才进入结果。
    # 不按ID硬编码筛选，是为了让“class字段本身变了”的情况能被后面的断言发现。
    return [record for record in records if record.action_class == 4]


def verify_class4_corpus(records: Sequence[FirttechRecord]) -> Dict[str, object]:
    """验证 canonical class4 语料的数量、ID、名称和共同字段值。"""

    # 先从500条结构化记录里按真正的 +0x18 值筛出 class4。
    class4 = collect_class4(records)

    # 固化55现场解析得到的七条记录。把 ID 和名称一起锁住，避免只有“数量7”却换成了别的记录。
    expected_id_name = [
        (450, "反咒禁制"),
        (457, "閾迦封界"),
        (458, "鬼唳天幕"),
        (461, "幽魄厲界"),
        (462, "六甲神儀"),
        (463, "時輪返陣"),
        (465, "鬼縛之陣"),
    ]
    # actual_id_name 来自当前输入文件现场解析，不是复制 expected 表。
    # 后面直接比较两个列表，顺序、ID、名称任一变化都会失败。
    actual_id_name = [(record.record_id, record.name) for record in class4]

    # 七条 class4 在当前 canonical 表中的几个 raw 字段恰好完全一致。
    # 本工具只把这个事实作为语料边界保存，不擅自把 +0x30/+0x3C/+0x44 命成作者变量名。
    fields_same = all(
        record.field_30 == 0
        and record.field_3c == 11
        and record.field_44 == 3
        and record.field_1e8 == 0
        for record in class4
    )

    # 七条作者描述都非空。这一点很重要，因为业务族判断不是从空 record 名称硬猜出来的。
    descriptions_nonempty = all(bool(record.description.strip()) for record in class4)

    return {
        "class分布": class_distribution(records),
        "class4数量": len(class4),
        "预期ID与名称": [
            {"ID": record_id, "名称": name}
            for record_id, name in expected_id_name
        ],
        "实际ID与名称": [
            {"ID": record_id, "名称": name}
            for record_id, name in actual_id_name
        ],
        "七条共同raw字段": {
            "+0x30": 0,
            "+0x3C": 11,
            "+0x44": 3,
            "+0x1E8": 0,
            "全部匹配": fields_same,
        },
        "七条作者说明均非空": descriptions_nonempty,
        "PASS": (
            actual_id_name == expected_id_name
            and fields_same
            and descriptions_nonempty
        ),
    }


def write_class4_csv(path: Path, records: Sequence[FirttechRecord]) -> None:
    """把七条 class4 原始业务语料写成严格 CSV，供人工复核。"""

    # 输出目录可能还不存在；parents=True 会连上层目录一起建立，exist_ok=True 允许目录已经存在。
    path.parent.mkdir(parents=True, exist_ok=True)

    # utf-8-sig 会写 UTF-8 BOM，让 Windows Excel 直接打开中文 CSV 时更不容易乱码。
    # newline="" 是 Python csv 模块推荐写法，可避免 Windows 下多出空白行。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        # csv.writer 负责正确处理作者说明里可能出现的逗号、引号等特殊字符。
        writer = csv.writer(handle)

        # 第一行是严格固定的列名；人工打开 CSV 时可以立刻知道每一列是什么。
        writer.writerow(
            [
                "索引",
                "记录ID",
                "记录ID十六进制",
                "名称",
                "FIRTTECH+0x18",
                "+0x30",
                "+0x3C",
                "+0x44",
                "+0x1E8",
                "作者说明",
            ]
        )

        # 这里只导出 class4 七条，不把另外493条无关记录塞进这个专题 CSV。
        for record in collect_class4(records):
            # 每条结构化记录严格按上面表头的同一顺序写一行。
            writer.writerow(
                [
                    record.index,
                    record.record_id,
                    f"0x{record.record_id:X}",
                    record.name,
                    record.action_class,
                    record.field_30,
                    record.field_3c,
                    record.field_44,
                    record.field_1e8,
                    record.description,
                ]
            )


def build_report(
    original_path: Path,
    research_path: Path,
    firttech_path: Path,
) -> Tuple[Dict[str, object], List[FirttechRecord]]:
    """执行所有固化55静态检查并返回一份可序列化报告。"""

    # 先分别解析两份EXE的PE结构。这里只读，不会把任何补丁写回文件。
    original = parse_pe(original_path)
    research = parse_pe(research_path)

    # 对两份完整EXE计算SHA，先确认“正在研究的到底是哪两个文件”。
    original_sha = sha256_bytes(original.data)
    research_sha = sha256_bytes(research.data)

    # Firttech 同时需要解码后的结构化记录和编码前原始字节，二者各有一套SHA基线。
    decoded_firttech, records = parse_firttech(firttech_path)
    encoded_firttech = firttech_path.read_bytes()

    # input_checks 是“输入身份保险丝”：任何一个 SHA 不符合已确认 canonical 基线，
    # 后面就算某些地址碰巧还能读到相同字节，总报告也不能宣称 PASS。
    input_checks = {
        "原版RPG": {
            "路径": str(original_path),
            "大小": len(original.data),
            "SHA256": original_sha,
            "预期SHA256": EXPECTED_ORIGINAL_RPG_SHA256,
            "PASS": original_sha == EXPECTED_ORIGINAL_RPG_SHA256,
        },
        "研究版RPG": {
            "路径": str(research_path),
            "大小": len(research.data),
            "SHA256": research_sha,
            "预期SHA256": EXPECTED_RESEARCH_RPG_SHA256,
            "PASS": research_sha == EXPECTED_RESEARCH_RPG_SHA256,
        },
        "Firttech.enc": {
            "路径": str(firttech_path),
            "大小": len(encoded_firttech),
            "encoded_SHA256": sha256_bytes(encoded_firttech),
            "预期_encoded_SHA256": EXPECTED_FIRTTECH_ENCODED_SHA256,
            "decoded_SHA256": sha256_bytes(decoded_firttech),
            "预期_decoded_SHA256": EXPECTED_FIRTTECH_DECODED_SHA256,
            "PASS": (
                sha256_bytes(encoded_firttech) == EXPECTED_FIRTTECH_ENCODED_SHA256
                and sha256_bytes(decoded_firttech) == EXPECTED_FIRTTECH_DECODED_SHA256
            ),
        },
    }

    # 第一份EXE单独做9项关键机器码断言，证明原版自身支持这套解释。
    original_machine = verify_machine_patterns(original)

    # 研究版也做完全相同的9项断言，不能只因为“历史上用过”就默认相同。
    research_machine = verify_machine_patterns(research)

    # 再比较固化55真正依赖的三个完整范围，防止9个局部锚点之间还藏有差异。
    relevant_equivalence = compare_relevant_ranges(original, research)

    # 与“关键范围相同”同时保存“整文件确实不同”的10字节完整边界。
    full_diff = compare_full_exe(original, research)

    # 最后独立验证资源表class分布和七条作者语料。机器证据与资源证据不能互相替代。
    corpus = verify_class4_corpus(records)

    # “同参”不是因为两段机器码字节完全相同，而是因为两段都从同一个 FightRole/target
    # 重新取出同一逻辑对象，并以相同压栈顺序进入同一个 wrapper。
    # 这条语义由上面的两组精确机器码断言共同支持。
    # 只有“原版局部机器码通过 + 研究版局部机器码通过 + 三段完整范围一致”三者同时成立，
    # 才允许把“两个调用逻辑实参相同”提升成报告里的 True。
    same_arguments_conclusion = (
        original_machine["PASS"]
        and research_machine["PASS"]
        and relevant_equivalence["PASS"]
    )

    # class4 分支从 0x42C7A9 直接跳到 0x42CC70；固定分支内部无 CALL，
    # 所以两次执行不会因为 RNG/global helper 造成“第二次和第一次不同”。
    # 在本工具已经精确锁住的 class4 固定分支里没有 CALL；因此当输入实参相同，
    # 第二次执行不会因为随机数或另一个 helper 的隐式全局状态而得到不同结果。
    deterministic_duplicate_conclusion = same_arguments_conclusion

    # report 把“输入身份、机器码、双EXE差异、资源语料、最终结论”全部收在一个JSON对象里。
    report: Dict[str, object] = {
        "工具版本": TOOL_VERSION,
        "研究节点": "v0.7D阶段中-固化55",
        "主题": "Battle FIRTTECH动作类四同参双调用与战场结界语义边界",
        "输入校验": input_checks,
        "原版RPG机器检查": original_machine,
        "研究版RPG机器检查": research_machine,
        "固化55关键代码双EXE等价": relevant_equivalence,
        "原版与研究版整文件差异基线": full_diff,
        "Firttech类四语料": corpus,
        "静态结论": {
            "类四两次0x42A7A0逻辑实参完全相同": same_arguments_conclusion,
            "旧结论_第二次交换施术者与目标": False,
            "0x42C680类四路径消耗RNG": False,
            "0x42C680类四路径调用公式或五槽helper": False,
            "类四两次调用对同一DamageInfo产生相同确定输出": deterministic_duplicate_conclusion,
            "Godot实现可将第二次纯计算调用折叠为一次而保持当前已证DamageInfo结果": deterministic_duplicate_conclusion,
            "canonical类四业务族": "持续性的战场结界/阵法/场域类动作",
            "作者源码枚举正式名": "UNKNOWN",
        },
    }

    # 最终 PASS 使用 all(...)，意思是下面每一层保险都必须通过。
    # 不允许出现“资源对了但EXE错了”或“EXE对了但class4语料变了”仍然返回成功。
    report["PASS"] = all(
        [
            all(item["PASS"] for item in input_checks.values()),
            original_machine["PASS"],
            research_machine["PASS"],
            relevant_equivalence["PASS"],
            full_diff["PASS"],
            corpus["PASS"],
            same_arguments_conclusion,
            deterministic_duplicate_conclusion,
        ]
    )

    # JSON报告和结构化记录都返回：main 用前者写JSON，用后者写七条CSV。
    return report, records


def parse_args() -> argparse.Namespace:
    """定义命令行参数。所有输入都显式给路径，避免工具偷偷依赖当前工作目录。"""

    # ArgumentParser 负责生成 --help、检查必填参数，并把文本路径转成 Path。
    parser = argparse.ArgumentParser(
        description="幽城Battle FIRTTECH类四同参双调用与资源语料静态调查器"
    )
    # 原版和研究版分成两个参数，故意不让调用者用一个“rpg”参数模糊两者身份。
    parser.add_argument(
        "--original-rpg",
        required=True,
        type=Path,
        help="完全未修改原版 RPG.exe 路径",
    )
    # 第二份输入是历史研究版；它的用途是做差异边界与关键区间交叉复核。
    parser.add_argument(
        "--research-rpg",
        required=True,
        type=Path,
        help="此前研究一直使用的修改版 RPG.exe 路径",
    )
    # Firttech 是业务语料来源，必须显式给文件，工具不会偷偷去当前目录猜路径。
    parser.add_argument(
        "--firttech",
        required=True,
        type=Path,
        help="multimedia/public/Firttech.enc 路径",
    )
    # JSON 是完整机器可读报告，供后续自动审计和接档脚本读取。
    parser.add_argument(
        "--json-out",
        required=True,
        type=Path,
        help="调查 JSON 输出路径",
    )
    # CSV 是人类最容易直接查看的七条 class4 作者语料表。
    parser.add_argument(
        "--csv-out",
        required=True,
        type=Path,
        help="class4 七条资源语料 CSV 输出路径",
    )
    # 真正解析 sys.argv；缺少任何 required 参数时 argparse 会自动说明错误并退出。
    return parser.parse_args()


def main() -> int:
    """程序入口：重放调查、落盘 JSON/CSV，再用退出码表达 PASS/FAIL。"""

    # 第一步把命令行文字变成结构化参数对象；此时还没有开始研究文件。
    args = parse_args()

    # 先执行全部只读分析。任何输入几何/PE格式错误都会直接抛异常，避免生成半真半假的报告。
    report, records = build_report(
        original_path=args.original_rpg,
        research_path=args.research_rpg,
        firttech_path=args.firttech,
    )

    # 输出目录可能是新建的“数据/固化55”，所以这里主动创建父目录。
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.csv_out.parent.mkdir(parents=True, exist_ok=True)

    # JSON 使用 UTF-8、中文原样输出、2 空格缩进，方便人工 diff 和接档阅读。
    args.json_out.write_text(
        json.dumps(report, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )

    # CSV 单独保留作者名称/说明全文，便于不看 JSON 也能快速审核七条语料。
    write_class4_csv(args.csv_out, records)

    # 控制台只打印一个很短的机器可读摘要；详细内容都已经进入 JSON。
    print(
        json.dumps(
            {
                "工具版本": TOOL_VERSION,
                "PASS": report["PASS"],
                "class4数量": report["Firttech类四语料"]["class4数量"],
                "原版研究版差异字节数": report["原版与研究版整文件差异基线"].get(
                    "差异字节数"
                ),
            },
            ensure_ascii=False,
        )
    )

    # 0 表示所有固化断言通过；1 表示至少一项失败。
    # 这样以后批处理/CI 可以直接通过进程退出码判断是否还能信任这套结论。
    return 0 if report["PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
