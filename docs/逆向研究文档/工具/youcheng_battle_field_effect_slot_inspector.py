#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 九项持续场域表现槽静态调查器。

本工具对应 Godot 兼容引擎研究“固化58”。它的目标不是把某段汇编随便起一个名字，
而是把固化57留下的两次 ``0x43DB20`` 调用从“未知表现请求”推进成可以重放验证的
运行时协议。工具只读取输入文件，不会修改 RPG.exe、Firttech.enc 或 multimedia.zip。

给第一次接触程序的读者，可以把本工具理解成一张“自动核对清单”：

1. 先确认拿到的确实是已经冻结过哈希的原版 EXE、研究版 EXE 与 Firttech.enc；
2. 再确认 ``0x43DB20`` 在原版代码区里真的只有固化57看到的两处直接调用；
3. 从 EXE 里重新读出 9 项 FIRTTECH-ID -> slot 映射表，而不是把表抄在结论里；
4. 从 EXE 里重新读出每个 slot 固定使用的 EFF*.SF2 资源名；
5. 解密 500 条 FIRTTECH，逐项对齐 ID、作者名称、class、+0x20、+0x90 和作者说明；
6. 用关键机器码区间哈希与短字节断言确认九槽管理器的数据布局、生命周期和双实例更新；
7. 比较完全原版与历史研究版关键区间，避免把历史补丁误当成原版行为；
8. 最后导出 JSON 与 CSV，供下一次固化或兼容引擎实现直接复核。

特别注意：本工具坚持“机器能证明到哪里就写到哪里”。例如 ``+0x90`` 当前 9 条白名单
记录都为正数，管理器每次合格 update 固定减 1100；这能证明的是 raw counter 的更新规则，
不能凭空把 1100 换算成毫秒、帧或游戏秒。``+0x20==2`` 会让一条记录提交到两个管理器，
但作者说明仍写“为己方”，因此本工具只称它为“特殊双管理器路由值”，不会强命名为
“双方目标”。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
import zipfile
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Sequence, Tuple


# ---------------------------------------------------------------------------
# 一、冻结输入与数据布局常量
# ---------------------------------------------------------------------------

# 工具版本只用于报告溯源；它不会改变任何判断逻辑。
TOOL_VERSION = "0.7D-solid58.0"

# 用户已明确确认，这个哈希对应“完全没有修改过的 RPG.exe.org”。
# 因此所有原始行为判断都优先以它为静态 Oracle。
EXPECTED_ORIGINAL_RPG_SHA256 = (
    "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
)

# 历史研究版只做等价性回归，不负责定义原版行为。
EXPECTED_RESEARCH_RPG_SHA256 = (
    "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
)

# canonical Firttech.enc 的加密/解密哈希；两层都检查可以防止拿错资源。
EXPECTED_FIRTTECH_ENCODED_SHA256 = (
    "32b98193c037c8cd75a11534520e189c89c856c367bc1d6ac607e43d9cf52a5d"
)
EXPECTED_FIRTTECH_DECODED_SHA256 = (
    "5469a9dd388972b53b5661e90d85c84f2d0150d06f46ddbae159409a5ac34b1a"
)

# FIRTTECH 是 500 条定长记录，每条 0x230 字节。
FIRTTECH_RECORD_COUNT = 500
FIRTTECH_RECORD_SIZE = 0x230

# 本节点实际读取的字段偏移。
# +0x00：记录 ID；+0x04：CP950 名称；+0x18：机器 class；
# +0x20：共享尾部路由值；+0x90：传给九槽管理器的 raw counter；
# +0x17C..记录末尾：作者说明文字。
FIRT_OFF_ID = 0x00
FIRT_OFF_NAME = 0x04
FIRT_NAME_SIZE = 0x20
FIRT_OFF_CLASS = 0x18
FIRT_OFF_ROUTE_20 = 0x20
FIRT_OFF_COUNTER_90 = 0x90
FIRT_OFF_DESCRIPTION = 0x17C
FIRT_DESCRIPTION_SIZE = FIRTTECH_RECORD_SIZE - FIRT_OFF_DESCRIPTION

# 固化57已独立验证过的 class 分布；固化58再次重放，保证资源没有换掉。
EXPECTED_CLASS_COUNTS = {0: 330, 1: 94, 2: 67, 4: 7, 5: 1, 6: 1}

# 0x46BC48 开始是 9 个 8-byte 项：第一个 DWORD 是 FIRTTECH ID，第二个 DWORD 是 slot。
MAP_TABLE_VA = 0x0046BC48
MAP_ENTRY_COUNT = 9
MAP_ENTRY_SIZE = 8

# 0x46BB94 开始是 9 个固定宽度 20-byte 资源名槽。
RESOURCE_TABLE_VA = 0x0046BB94
RESOURCE_ENTRY_COUNT = 9
RESOURCE_ENTRY_SIZE = 20

# 当前原版机器表的严格期望值。
EXPECTED_ID_TO_SLOT = (
    (0x15A, 0),
    (0x17C, 1),
    (0x1C2, 2),
    (0x1C9, 3),
    (0x1CA, 4),
    (0x1CD, 5),
    (0x1CE, 6),
    (0x1CF, 7),
    (0x1D1, 8),
)
EXPECTED_RESOURCE_NAMES = (
    "EFF21761.SF2",
    "EFF21871.SF2",
    "EFF23092.SF2",
    "EFF20014.SF2",
    "EFF22082.SF2",
    "EFF23121.SF2",
    "EFF23122.SF2",
    "EFF20033.SF2",
    "EFF23131.SF2",
)

# 原版 .text 对 0x43DB20 的 direct E8 CALL 应严格只有两处。
EXPECTED_43DB20_DIRECT_CALLS = (0x004200A7, 0x004200FA)

# 固化58最重要的机器码区间。这里保存原版切片哈希，工具会现场重新计算。
# 只要地址错一个字节或 EXE 版本不对，检查就会失败。
CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (0x00420048, 0x00420113, "class1/2/4共享后段与双管理器提交路由", "a1eafc050c30d169bd9f6aec618355b1474d39706996eabf51d8497156649691"),
    (0x0043DA30, 0x0043DCAB, "九槽重置、状态查询、tick、提交、owner查询与owner清理主体", "7cfafc1ee222beeff46627c4fff28103c400829e27cea2257976d3d184b511b9"),
    (0x0043DCB0, 0x0043DDC6, "EFF资源路径构造包装", "b44c4e1580f0ae62704026ae46175391ce5a603f65c8a53224db7c03a3185a17"),
    (0x0044283E, 0x00442870, "两个九槽管理器实例初始化及1/0选择值", "2a942c640538f7d1d75543cb36cc73b9bfd849f955d3132dfc60bec4a553cecf"),
    (0x00442CA2, 0x00442CC3, "Battle主更新对两个九槽实例逐轮tick", "01e89b1470ef767a2dd1b0af2ba3da6f3615ff435f78e1f4f61261626261f475"),
    (0x004211FE, 0x00421238, "角色主资源归零时双实例owner清理", "6c4d3aa26f31809ab997ba70ff9cc9f1cd2f8f3e86a77dc1256b50b6afa3776d"),
    (0x00423280, 0x004232C1, "角色失效链双实例owner清理", "025dabe5908901c576f6cdac70fc916ea80b257ae853782d7622b0305d0dc396"),
    (0x0046BB94, 0x0046BC8C, "九个固定EFF资源名与九项FIRTTECH到slot映射表", "dce3fbd949ca4e101f6b16a4e96cafbbe9ea46d4bac869a048f1422d26f8a93d"),
)

# 比较这些区间的原版与研究版，验证历史10字节补丁没有碰本节点证据。
EQUIVALENCE_RANGES: Tuple[Tuple[int, int, str], ...] = (
    (0x00420048, 0x00420113, "共享尾部双实例提交路由"),
    (0x0043DA30, 0x0043DDC6, "九槽管理器及路径构造"),
    (0x0044283E, 0x00442870, "双实例初始化"),
    (0x00442CA2, 0x00442CC3, "双实例Battle tick"),
    (0x004211FE, 0x00421238, "主资源归零owner清理"),
    (0x00423280, 0x004232C1, "角色失效owner清理"),
    (0x0046BB94, 0x0046BC8C, "EFF资源名与映射表"),
)


# ---------------------------------------------------------------------------
# 二、PE 文件读取基础结构
# ---------------------------------------------------------------------------

@dataclass(frozen=True)
class Section:
    """PE section 的最小描述。"""

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass(frozen=True)
class PeImage:
    """把 EXE 原始字节、ImageBase 与 section 表放在一起。"""

    path: Path
    data: bytes
    image_base: int
    sections: Tuple[Section, ...]

    def va_to_offset(self, va: int) -> int:
        """把反汇编里看到的 VA 换算成硬盘文件偏移。"""

        # VA 减掉 ImageBase 后才是 RVA。
        rva = va - self.image_base
        # 每个 section 在内存与文件中的起点不同，所以必须通过 section 表换算。
        for section in self.sections:
            span = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + span:
                inside = rva - section.virtual_address
                return section.raw_offset + inside
        # 找不到时立即报错，绝不能偷偷把 VA 当文件偏移继续读。
        raise ValueError(f"VA 0x{va:08X} 不属于 {self.path.name} 的任何 section")

    def read_va(self, va: int, size: int) -> bytes:
        """从指定 VA 读取固定字节数，整个过程只读。"""

        start = self.va_to_offset(va)
        end = start + size
        if end > len(self.data):
            raise ValueError(f"读取 0x{va:08X}+0x{size:X} 越过 {self.path.name} 文件末尾")
        return self.data[start:end]

    def text_section(self) -> Section:
        """返回 .text；direct CALL 扫描只在代码区做。"""

        for section in self.sections:
            if section.name == ".text":
                return section
        raise ValueError(f"{self.path.name} 没有 .text section")


@dataclass(frozen=True)
class FirttechRecord:
    """固化58真正需要的 FIRTTECH 字段视图。"""

    index: int
    record_id: int
    name: str
    action_class: int
    route_20: int
    counter_90: int
    description: str


# ---------------------------------------------------------------------------
# 三、最小二进制辅助函数
# ---------------------------------------------------------------------------

def sha256_bytes(data: bytes) -> str:
    """计算 bytes 的 SHA-256。"""

    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """读取文件并计算 SHA-256。"""

    return sha256_bytes(path.read_bytes())


def read_u16(data: bytes, offset: int) -> int:
    """读取 little-endian uint16。"""

    return struct.unpack_from("<H", data, offset)[0]


def read_u32(data: bytes, offset: int) -> int:
    """读取 little-endian uint32。"""

    return struct.unpack_from("<I", data, offset)[0]


def read_s32(data: bytes, offset: int) -> int:
    """读取 little-endian signed int32。"""

    return struct.unpack_from("<i", data, offset)[0]


def decode_cp950_cstring(raw: bytes) -> str:
    """读取固定宽度字段中第一个 NUL 之前的 CP950 字符串。"""

    useful = raw.split(b"\x00", 1)[0]
    return useful.decode("cp950", errors="strict")


def parse_pe(path: Path) -> PeImage:
    """解析本调查所需的 PE32 头和 section 表。"""

    data = path.read_bytes()
    # DOS 头 +0x3C 保存 PE header 文件偏移。
    if len(data) < 0x40 or data[:2] != b"MZ":
        raise ValueError(f"{path} 不是合法 MZ/PE 文件")
    pe_offset = read_u32(data, 0x3C)
    if data[pe_offset:pe_offset + 4] != b"PE\x00\x00":
        raise ValueError(f"{path} 找不到 PE\\0\\0 标记")

    coff = pe_offset + 4
    section_count = read_u16(data, coff + 2)
    optional_size = read_u16(data, coff + 16)
    optional = coff + 20
    image_base = read_u32(data, optional + 28)
    table = optional + optional_size

    sections: List[Section] = []
    # IMAGE_SECTION_HEADER 固定 40 字节；逐条把 VA 映射信息保存下来。
    for index in range(section_count):
        entry = table + index * 40
        if entry + 40 > len(data):
            raise ValueError(f"{path} section 表第 {index} 项越界")
        name = data[entry:entry + 8].split(b"\x00", 1)[0].decode("ascii", errors="replace")
        sections.append(
            Section(
                name=name,
                virtual_size=read_u32(data, entry + 8),
                virtual_address=read_u32(data, entry + 12),
                raw_size=read_u32(data, entry + 16),
                raw_offset=read_u32(data, entry + 20),
            )
        )
    return PeImage(path=path, data=data, image_base=image_base, sections=tuple(sections))


def decrypt_enc(encoded: bytes) -> bytes:
    """使用项目已经确认的公共 ENC XOR 规则解密。"""

    decoded = bytearray(len(encoded))
    # 第 i 个字节与 (i mod 255) 异或；XOR 可逆，因此同一规则也能重新编码。
    for index, value in enumerate(encoded):
        decoded[index] = value ^ (index % 255)
    return bytes(decoded)


def parse_firttech(path: Path) -> Tuple[bytes, List[FirttechRecord]]:
    """解密并遍历全部 500 条 FIRTTECH。"""

    encoded = path.read_bytes()
    expected_size = FIRTTECH_RECORD_COUNT * FIRTTECH_RECORD_SIZE
    if len(encoded) != expected_size:
        raise ValueError(f"Firttech.enc 大小={len(encoded)}，预期={expected_size}")
    decoded = decrypt_enc(encoded)
    records: List[FirttechRecord] = []

    for index in range(FIRTTECH_RECORD_COUNT):
        start = index * FIRTTECH_RECORD_SIZE
        record = decoded[start:start + FIRTTECH_RECORD_SIZE]
        records.append(
            FirttechRecord(
                index=index,
                record_id=read_u32(record, FIRT_OFF_ID),
                name=decode_cp950_cstring(record[FIRT_OFF_NAME:FIRT_OFF_NAME + FIRT_NAME_SIZE]),
                action_class=read_s32(record, FIRT_OFF_CLASS),
                route_20=read_s32(record, FIRT_OFF_ROUTE_20),
                counter_90=read_s32(record, FIRT_OFF_COUNTER_90),
                description=decode_cp950_cstring(
                    record[FIRT_OFF_DESCRIPTION:FIRT_OFF_DESCRIPTION + FIRT_DESCRIPTION_SIZE]
                ),
            )
        )
    return decoded, records


def scan_direct_calls(image: PeImage, target_va: int) -> List[int]:
    """真正解析 x86 ``E8 rel32``，返回所有直接 CALL 到目标地址的位置。"""

    section = image.text_section()
    text = image.data[section.raw_offset:section.raw_offset + section.raw_size]
    text_va = image.image_base + section.virtual_address
    hits: List[int] = []

    # 这里不是简单搜索 0xE8 字节；必须把后面 4 字节当 signed rel32 算出真实目的地址。
    for offset in range(max(0, len(text) - 4)):
        if text[offset] != 0xE8:
            continue
        relative = struct.unpack_from("<i", text, offset + 1)[0]
        caller = text_va + offset
        destination = caller + 5 + relative
        if destination == target_va:
            hits.append(caller)
    return hits


def verify_exact_bytes(image: PeImage, va: int, expected_hex: str, meaning: str) -> Dict[str, object]:
    """核对一段短机器码，帮助把整段哈希与具体语义锚点联系起来。"""

    expected = bytes.fromhex(expected_hex)
    actual = image.read_va(va, len(expected))
    return {
        "地址": f"0x{va:08X}",
        "意义": meaning,
        "实际": actual.hex(" "),
        "预期": expected.hex(" "),
        "PASS": actual == expected,
    }


# ---------------------------------------------------------------------------
# 四、机器码与静态表验证
# ---------------------------------------------------------------------------

def read_map_table(image: PeImage) -> List[Tuple[int, int]]:
    """从 EXE 原地读取九个 ``FIRTTECH ID -> slot`` 项。"""

    result: List[Tuple[int, int]] = []
    for index in range(MAP_ENTRY_COUNT):
        raw = image.read_va(MAP_TABLE_VA + index * MAP_ENTRY_SIZE, MAP_ENTRY_SIZE)
        record_id, slot = struct.unpack("<II", raw)
        result.append((record_id, slot))
    return result


def read_resource_table(image: PeImage) -> List[str]:
    """从 EXE 原地读取 9 个固定 EFF*.SF2 文件名。"""

    names: List[str] = []
    for index in range(RESOURCE_ENTRY_COUNT):
        raw = image.read_va(RESOURCE_TABLE_VA + index * RESOURCE_ENTRY_SIZE, RESOURCE_ENTRY_SIZE)
        # 这些资源名是纯 ASCII，所以严格 ASCII 解码即可。
        names.append(raw.split(b"\x00", 1)[0].decode("ascii", errors="strict"))
    return names


def verify_machine(original: PeImage, research: PeImage) -> Dict[str, object]:
    """完成固化58所有关键静态机器事实核对。"""

    slice_checks: List[Dict[str, object]] = []
    for start, end, meaning, expected_hash in CRITICAL_SLICES:
        actual_hash = sha256_bytes(original.read_va(start, end - start))
        slice_checks.append({
            "范围": f"0x{start:08X}..0x{end - 1:08X}",
            "意义": meaning,
            "实际SHA256": actual_hash,
            "预期SHA256": expected_hash,
            "PASS": actual_hash == expected_hash,
        })

    # 短断言专门钉死关键数据布局与调用关系；它们比“看起来像”更可重复。
    exact_checks = [
        verify_exact_bytes(
            original, 0x00420089,
            "8b15ecfd8900528b15f0fd8900528b10518b88900000005152b908fe8900e874da0100",
            "第一路把坐标数组、角色槽、FIRTTECH+0x90、FIRTTECH ID传给0x89FE08/0x43DB20",
        ),
        verify_exact_bytes(
            original, 0x004200DC,
            "8b15ecfd8900528b15f0fd8900528b10518b88900000005152b9d0ff8900e821da0100",
            "第二路以同样五参数提交给0x89FFD0/0x43DB20",
        ),
        verify_exact_bytes(
            original, 0x0043DA30,
            "8d4124b90900000033d28950248950dc8910c74078ffffffff83c0044975ebc3",
            "0x43DA30严格循环9槽并清active/current/step、把effect handle写-1",
        ),
        verify_exact_bytes(
            original, 0x0043DAB2,
            "8d7148bf09000000833e00744f",
            "0x43DAB0从active数组开始严格循环9槽",
        ),
        verify_exact_bytes(
            original, 0x0043DABF,
            "8b46b88b4edc03c18946b8",
            "活动槽执行current += step",
        ),
        verify_exact_bytes(
            original, 0x0043DAFB,
            "8b0ddc018b0050e8e9fcfeffc74654ffffffff",
            "槽结束时经全局Effect Manager 0x42D7F0释放句柄并写-1",
        ),
        verify_exact_bytes(
            original, 0x0043DB20,
            "8b54240456578bf983ceffb101b84cbc4600",
            "0x43DB20读取FIRTTECH ID并从0x46BC48九项表开始匹配",
        ),
        verify_exact_bytes(
            original, 0x0043DB5A,
            "8b44b74885c00f8596000000",
            "已有active槽拒绝再次创建",
        ),
        verify_exact_bytes(
            original, 0x0043DB66,
            "8b4424148944b7788b442410c744b748010000008904b7",
            "保存owner role slot、激活槽并保存传入+0x90到current数组",
        ),
        verify_exact_bytes(
            original, 0x0043DB7D,
            "85c07e0ac744b724b4fbffffeb08c744b7244c040000",
            "current>0时step=-1100，否则step=+1100",
        ),
        verify_exact_bytes(
            original, 0x0043DB93,
            "8b87c00100008b4c2418488b54241cf7d81bc06a0024ec6a0083c01b",
            "this+0x1C0选择坐标索引27或7；此断言严格使用28字节，避免历史预审长度错误",
        ),
        verify_exact_bytes(
            original, 0x0043DBC1,
            "8d0c8594bb4600518bcfe8e00000008b0ddc018b0050e8f4f8feff",
            "slot*20选择0x46BB94资源名，构造路径后交给全局Effect Manager 0x42D4D0",
        ),
        verify_exact_bytes(
            original, 0x0043DC10,
            "568b74240883c8ff33d25783c178",
            "0x43DC10从owner数组开始扫描活动槽",
        ),
        verify_exact_bytes(
            original, 0x0043DC60,
            "5356578d7178bb0900000033ff",
            "0x43DC60按owner角色槽严格循环9项执行清理",
        ),
        verify_exact_bytes(
            original, 0x0044283E,
            "b908fe89008935f8fd8900e8e2b1ffffb9d0ff8900c705c8ff890001000000e8ceb1ffff",
            "Battle初始化依次reset 0x89FE08/0x89FFD0，并把第一实例+0x1C0写1",
        ),
        verify_exact_bytes(
            original, 0x00442862,
            "8d4c241c893590018a00",
            "第二实例+0x1C0由已清零ESI写0",
        ),
        verify_exact_bytes(
            original, 0x00442CAF,
            "b908fe8900e8f7adffffb9d0ff8900e8edadffff",
            "Battle合格主更新连续tick两个九槽实例",
        ),
    ]

    direct_calls = scan_direct_calls(original, 0x0043DB20)

    equivalence_checks: List[Dict[str, object]] = []
    for start, end, meaning in EQUIVALENCE_RANGES:
        a = original.read_va(start, end - start)
        b = research.read_va(start, end - start)
        equivalence_checks.append({
            "范围": f"0x{start:08X}..0x{end - 1:08X}",
            "意义": meaning,
            "原版SHA256": sha256_bytes(a),
            "研究版SHA256": sha256_bytes(b),
            "逐字节一致": a == b,
        })

    map_table = read_map_table(original)
    resources = read_resource_table(original)

    all_pass = (
        all(item["PASS"] for item in slice_checks)
        and all(item["PASS"] for item in exact_checks)
        and tuple(direct_calls) == EXPECTED_43DB20_DIRECT_CALLS
        and tuple(map_table) == EXPECTED_ID_TO_SLOT
        and tuple(resources) == EXPECTED_RESOURCE_NAMES
        and all(item["逐字节一致"] for item in equivalence_checks)
    )

    return {
        "关键区间SHA核对": slice_checks,
        "关键短机器码断言": exact_checks,
        "0x43DB20直接CALL": [f"0x{x:08X}" for x in direct_calls],
        "0x43DB20直接CALL严格匹配": tuple(direct_calls) == EXPECTED_43DB20_DIRECT_CALLS,
        "九项ID到slot映射": [{"FIRTTECH_ID": f"0x{x:03X}", "slot": y} for x, y in map_table],
        "九项映射严格匹配": tuple(map_table) == EXPECTED_ID_TO_SLOT,
        "九项固定EFF资源名": resources,
        "九项资源名严格匹配": tuple(resources) == EXPECTED_RESOURCE_NAMES,
        "原版与研究版关键范围等价": equivalence_checks,
        "全部机器检查通过": all_pass,
    }


# ---------------------------------------------------------------------------
# 五、资源语料与 multimedia 子集检查
# ---------------------------------------------------------------------------

def inspect_canonical_records(records: Sequence[FirttechRecord]) -> Dict[str, object]:
    """把九项白名单与 canonical 500条资源逐项对齐。"""

    by_id = {record.record_id: record for record in records}
    rows: List[Dict[str, object]] = []

    for slot, (record_id, mapped_slot) in enumerate(EXPECTED_ID_TO_SLOT):
        if slot != mapped_slot:
            raise AssertionError("内部冻结表 slot 顺序异常")
        record = by_id.get(record_id)
        if record is None:
            rows.append({"slot": slot, "FIRTTECH_ID": f"0x{record_id:03X}", "存在": False})
            continue
        rows.append({
            "slot": slot,
            "FIRTTECH_ID": f"0x{record_id:03X}",
            "名称": record.name,
            "class_加18": record.action_class,
            "路由值_加20": record.route_20,
            "raw_counter_加90": record.counter_90,
            "作者说明": record.description,
            "固定EFF资源": EXPECTED_RESOURCE_NAMES[slot],
            "存在": True,
        })

    class_counts = dict(sorted(Counter(record.action_class for record in records).items()))
    expected_names = ("非天死潭", "封炎滅陣", "反咒禁制", "閾迦封界", "鬼唳天幕", "幽魄厲界", "六甲神儀", "時輪返陣", "鬼縛之陣")
    actual_names = tuple(row.get("名称", "") for row in rows)
    classes = tuple(row.get("class_加18") for row in rows)
    route_values = tuple(row.get("路由值_加20") for row in rows)
    counters = tuple(row.get("raw_counter_加90") for row in rows)

    # 9条应严格是 class1一条、class2一条、class4七条。
    expected_classes = (1, 2, 4, 4, 4, 4, 4, 4, 4)
    # +0x20 的当前 canonical 路由值也一起固化。
    expected_routes = (0, 0, 2, 1, 0, 0, 1, 1, 0)
    # +0x90 是 raw counter，不做时间单位命名。
    expected_counters = (900000, 420000, 350000, 1000000, 800000, 500000, 800000, 900000, 750000)

    all_pass = (
        class_counts == EXPECTED_CLASS_COUNTS
        and all(row.get("存在") for row in rows)
        and actual_names == expected_names
        and classes == expected_classes
        and route_values == expected_routes
        and counters == expected_counters
        and all(value > 0 for value in counters)
    )

    return {
        "canonical_class分布": class_counts,
        "canonical_class分布严格匹配": class_counts == EXPECTED_CLASS_COUNTS,
        "九项白名单": rows,
        "九项名称严格匹配": actual_names == expected_names,
        "九项class严格匹配": classes == expected_classes,
        "九项加20严格匹配": route_values == expected_routes,
        "九项加90严格匹配": counters == expected_counters,
        "九项加90全部为正": all(value > 0 for value in counters),
        "特殊双管理器路由提示": "当前+0x20==2仅反咒禁制；机器双实例提交，但作者说明写为己方，因此仅冻结为特殊双管理器路由值，不强命名双方目标。",
        "全部资源检查通过": all_pass,
    }


def inspect_multimedia_zip(zip_path: Path) -> Dict[str, object]:
    """核对当前用户提供的 multimedia.zip 子集中是否含九个固定 EFF 文件。"""

    if not zip_path.exists():
        return {"提供": False, "PASS": True, "说明": "未提供 multimedia.zip；本项不参与主结论。"}

    with zipfile.ZipFile(zip_path, "r") as archive:
        names = archive.namelist()
        basenames = {Path(name).name.lower() for name in names}
        matches = {
            resource: [name for name in names if Path(name).name.lower() == resource.lower()]
            for resource in EXPECTED_RESOURCE_NAMES
        }

    present = [resource for resource in EXPECTED_RESOURCE_NAMES if resource.lower() in basenames]
    return {
        "提供": True,
        "ZIP条目数": len(names),
        "九项固定EFF实体命中": matches,
        "命中的九项资源": present,
        "结论": "当前提供的multimedia研究子集未包含这九个固定EFF实体；这只描述本次输入子集，不能扩大成原游戏缺失资源。",
        "PASS": len(present) == 0,
    }


# ---------------------------------------------------------------------------
# 六、输出 JSON / CSV
# ---------------------------------------------------------------------------

def write_csv(path: Path, rows: Sequence[Dict[str, object]]) -> None:
    """把九项白名单写成严格 UTF-8-SIG CSV，方便 Windows/Excel 直接打开。"""

    path.parent.mkdir(parents=True, exist_ok=True)
    fields = [
        "slot", "FIRTTECH_ID", "名称", "class_加18", "路由值_加20",
        "raw_counter_加90", "固定EFF资源", "作者说明",
    ]
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def build_report(
    original_path: Path,
    research_path: Path,
    firttech_path: Path,
    multimedia_zip: Path,
) -> Dict[str, object]:
    """运行全部检查并构造最终机器可读报告。"""

    original = parse_pe(original_path)
    research = parse_pe(research_path)
    decoded, records = parse_firttech(firttech_path)

    input_checks = {
        "原版RPG.exe": {
            "路径": str(original_path),
            "SHA256": sha256_file(original_path),
            "预期": EXPECTED_ORIGINAL_RPG_SHA256,
        },
        "研究版RPG.exe": {
            "路径": str(research_path),
            "SHA256": sha256_file(research_path),
            "预期": EXPECTED_RESEARCH_RPG_SHA256,
        },
        "Firttech.enc": {
            "路径": str(firttech_path),
            "加密SHA256": sha256_file(firttech_path),
            "加密预期": EXPECTED_FIRTTECH_ENCODED_SHA256,
            "解密SHA256": sha256_bytes(decoded),
            "解密预期": EXPECTED_FIRTTECH_DECODED_SHA256,
        },
    }
    input_pass = (
        input_checks["原版RPG.exe"]["SHA256"] == EXPECTED_ORIGINAL_RPG_SHA256
        and input_checks["研究版RPG.exe"]["SHA256"] == EXPECTED_RESEARCH_RPG_SHA256
        and input_checks["Firttech.enc"]["加密SHA256"] == EXPECTED_FIRTTECH_ENCODED_SHA256
        and input_checks["Firttech.enc"]["解密SHA256"] == EXPECTED_FIRTTECH_DECODED_SHA256
    )

    machine = verify_machine(original, research)
    resources = inspect_canonical_records(records)
    multimedia = inspect_multimedia_zip(multimedia_zip)

    all_pass = input_pass and machine["全部机器检查通过"] and resources["全部资源检查通过"] and multimedia["PASS"]

    return {
        "工具": "youcheng_battle_field_effect_slot_inspector.py",
        "工具版本": TOOL_VERSION,
        "节点": "v0.7D阶段中-固化58",
        "输入哈希核对": input_checks,
        "输入哈希全部通过": input_pass,
        "机器证据": machine,
        "canonical资源证据": resources,
        "multimedia输入子集证据": multimedia,
        "Godot兼容实现冻结规则": {
            "白名单": "只有九项静态ID表能占用这套持续场域表现槽；其他FIRTTECH即使class为1/2/4也不能因此自动创建该表现。",
            "双实例": "共享尾部按角色槽0..7/8..及FIRTTECH+0x20条件选择0x89FE08与0x89FFD0；+0x20==2为机器特殊双管理器路由，不强命名业务双方。",
            "槽布局": "每实例9槽；已证数组为current(this+0x00)、step(+0x24)、active(+0x48)、owner_role_slot(+0x78)、effect_handle(+0x9C)，每项4字节。",
            "创建": "匹配ID且slot未active时保存owner和+0x90；current>0用step=-1100，否则+1100；按实例+0x1C0选择坐标索引27或7；使用slot固定EFF资源经0x42D4D0创建Effect Manager句柄。",
            "更新释放": "Battle合格更新中两个实例各调用一次0x43DAB0；active槽执行current+=step，到0边界后清active并经0x42D7F0释放句柄。",
            "owner清理": "0x43DC10可按角色槽反查active slot；0x43DC60在角色资源归零/角色失效链上按owner清除并释放两个实例中的表现。",
            "时间单位": "+0x90与1100只保留raw counter/raw step语义；未证明与毫秒、帧或游戏秒的换算。",
        },
        "仍保持UNKNOWN": [
            "class1/class2作者正式枚举名",
            "FIRTTECH+0x20作者正式字段名",
            "FIRTTECH+0x90作者正式字段名与真实时间单位",
            "0x43DA50更高层所有业务consumer的完整语义",
            "九个EFF实体在当前multimedia研究子集之外的原始容器来源",
            "D8 draw order",
        ],
        "全部通过": all_pass,
    }


# ---------------------------------------------------------------------------
# 七、给初学者看的逐步阅读提示（这些注释不参与程序运行）
# ---------------------------------------------------------------------------
# 01. EXE 在硬盘里的字节位置，和反汇编器显示的内存地址不是同一个数字。
# 02. 所以本工具永远先读 PE section 表，再把 VA 换算成文件偏移。
# 03. 如果跳过这一步，哪怕只错一个 section 起点，后面读取的机器码都会全部错位。
# 04. SHA-256 在这里相当于“这一整段字节的指纹”，不是游戏自己的校验算法。
# 05. 原版切片哈希正确，说明我们重新读取的范围与固化时看到的原版范围逐字节相同。
# 06. 研究版只和原版比较关键范围；研究版整文件哈希不同并不等于本节点机器码不同。
# 07. ``E8 rel32`` 是 32 位 x86 常见的直接 CALL 指令编码。
# 08. E8 后四字节不是绝对地址，而是“从下一条指令走多远”的有符号位移。
# 09. 因此扫描 CALL 时必须把这个位移真正算出来，不能只搜索一个 E8 字节。
# 10. 固化55曾经证明过这种区别非常重要：普通数据里也可能恰好出现字节 E8。
# 11. ``0x43DB20`` 的第一个参数是 FIRTTECH ID；它先在九项表里查找对应 slot。
# 12. 表中没有的 ID 会得到 -1，因此直接失败，不会偷偷借用别的槽。
# 13. slot 只有 0..8 九种合法值；代码还额外做了下界和上界检查。
# 14. ``active`` 数组表示这一格现在是否已被占用。
# 15. 如果同一个 slot 已经 active，新提交会失败，而不是覆盖旧句柄。
# 16. ``owner_role_slot`` 保存“哪个战斗角色槽启动了这项表现”。
# 17. 它不是角色名字，也不是 API 数据库 ID，所以兼容层不要混成同一个概念。
# 18. ``current`` 保存从 FIRTTECH+0x90 传入的原始计数量。
# 19. ``step`` 保存每次管理器更新对 current 加上的固定有符号变化量。
# 20. 当前九条 canonical 记录的 +0x90 都大于 0，所以实际 step 全部是 -1100。
# 21. 机器码也保留了 current<=0 时 step=+1100 的镜像路径。
# 22. 但当前资源没有九槽白名单的负值/零实例，因此不能虚构它的作者用途。
# 23. ``effect_handle`` 是全局 Effect Manager 返回的句柄。
# 24. 句柄小于 0 表示创建失败或当前没有有效句柄。
# 25. 句柄有效时，代码会调用 0x42D6E0(handle,1) 做后续启动设置。
# 26. slot 结束时则调用 0x42D7F0(handle) 释放，并把保存位置重新写成 -1。
# 27. 这证明九槽对象不只是“记录状态”，它确实拥有并管理 Effect Manager 资源。
# 28. 九个 EFF 文件名不是从 FIRTTECH 文本猜出来的，而是 EXE 自带固定字符串表。
# 29. slot 乘以 20 后正好索引到一个 20-byte 固定资源名格。
# 30. 资源名交给 0x43DCB0 后再交给 Effect Manager 创建句柄。
# 31. 本工具把这个事实称为“持续场域表现槽”，而不把 EFF 文件内容进一步强命名。
# 32. 当前 multimedia.zip 是研究资料子集；里面找不到九个 EFF 不表示原游戏也找不到。
# 33. 资源缺失判断必须区分“当前附件没有”和“游戏安装资源不存在”，二者完全不同。
# 34. 两个管理器实例地址分别是 0x89FE08 和 0x89FFD0。
# 35. Battle 初始化会先对两个实例都调用 0x43DA30，把九个槽恢复为空状态。
# 36. 第一实例 +0x1C0 被写 1；第二实例同一位置被写 0。
# 37. 0x43DB20 根据这个值选择坐标数组中的索引 27 或 7。
# 38. 因此两个实例不只是“同一个对象复制两份”，它们还选取不同定位锚点。
# 39. 固化58暂时只冻结“索引27/7”的机器事实，不擅自给这两个索引起作者业务名。
# 40. 共享尾部先读取 FightRole+0x00 的角色槽号，再按 <=7 / >=8 分两组判断。
# 41. 这种低8/高8分组是机器事实；是否等价于作者命名的“我方/敌方”要由独立证据支持。
# 42. FIRTTECH+0x20 值 0 和 1 会让低8/高8选择相反实例。
# 43. +0x20 值 2 会让同一个动作同时满足两个实例的提交条件。
# 44. 反咒禁制是当前九槽白名单中唯一的 +0x20==2 记录。
# 45. 但它作者说明写“为己方”，所以不能把数字 2 偷换成“双方目标”这个作者枚举名。
# 46. 兼容引擎应复刻条件表达式本身，而不是依赖我们对数字含义的猜测。
# 47. ``0x43DAB0`` 每次被调用时只推进九槽一次。
# 48. Battle 主更新在门条件成立后连续调用两个实例各一次。
# 49. 当前没有独立证据把一次这种调用换算成固定毫秒，因此不写“每1100毫秒”。
# 50. 如果以后结合旧式20Hz时钟证明同一门控关系，才可以再升级真实时间换算。
# 51. current 与 step 相加后，负 step 在 current<=0 时结束。
# 52. 正 step 则在 current>=0 时结束，这是机器码保留的镜像条件。
# 53. 结束不是只把 current 夹到0；机器首先把 active 清零，再负责释放句柄。
# 54. ``0x43DC10`` 可以按 owner_role_slot 扫描九个 active 槽并返回 slot。
# 55. 这说明 owner 字段是真正会被后续逻辑查询的运行时元数据，不是写了不用。
# 56. ``0x43DC40`` 可以给定 slot 查询 active 槽的 owner；未激活则返回 -1。
# 57. ``0x43DC60`` 则做反向操作：给定 owner，清掉属于这个角色的所有活动槽。
# 58. 角色主资源归零路径会对两个实例都调用 0x43DC60。
# 59. 角色失效/重置路径也会对两个实例都调用 0x43DC60。
# 60. 所以 Godot 兼容层不能只做“倒计时结束删除”，还必须实现 owner 驱动的提前清理。
# 61. class 分布再次统计，是为了证明这轮 FIRTTECH 输入仍是同一份 canonical 资源。
# 62. 九槽白名单由 1条class1、1条class2、7条class4 组成。
# 63. 这并不意味着“所有class1/2/4都有九槽表现”；恰恰相反，白名单只有九个 ID。
# 64. 其他159条class1/2/4动作即使进入固化57共享表现门，也会被九项ID表拒绝。
# 65. 兼容实现若直接写成 ``if class in {1,2,4}: create_field_effect`` 就会明显扩大原版行为。
# 66. CSV 专门把 slot、ID、名称、class、+0x20、+0x90、EFF 和说明放在同一行。
# 67. 这样人工检查时不必在九份二进制窗口之间来回切换，也更容易发现错位。
# 68. JSON 则保留所有机器断言和哈希，适合以后自动重放。
# 69. JSON 的 ``全部通过`` 只有输入、机器、资源、附件子集四大类都通过才会为 true。
# 70. 任意断言失败时工具返回退出码1，封包流程必须把它当成阻塞，而不是忽略。
# 71. 工具不会写 EXE；它唯一会创建/覆盖的是用户明确指定的 JSON 与 CSV 输出文件。
# 72. 输出目录不存在时会自动创建，这是为了让全新解压的固化包也能独立运行。
# 73. CP950 解码使用 strict；坏字节会直接抛异常，不用替代字符掩盖输入错误。
# 74. 固定资源名用 strict ASCII；如果表内容不是预期英文文件名，同样立即暴露。
# 75. 所有 32 位整数读取都明确区分 signed/unsigned，避免 -1 被误读成 4294967295。
# 76. 映射表的 ID/slot 本身按 uint32 读取，因为它们不需要负值语义。
# 77. FIRTTECH class/+0x20/+0x90 按 signed int32 保存，保留未来遇到负值的可能。
# 78. ``EXPECTED_*`` 常量不是为了“让工具总是得到预期结论”，而是用于检测输入或代码变化。
# 79. 实际值始终先从文件现场读取，再和冻结值比较；报告同时保存“实际”和“预期”。
# 80. 因此未来若换 EXE 版本，工具会明确 FAIL，而不是静默输出旧结论。
# 81. 关键区间 SHA 与短指令断言是两层保护：前者防整体变化，后者解释具体为什么重要。
# 82. 只做短指令断言可能漏掉同一函数其他位置变化；只做整段哈希则不方便人工理解语义。
# 83. 两层一起使用，既适合机器回归，也适合下一位研究者人工接档。
# 84. 原版与研究版“关键范围逐字节一致”也只限列出的范围，不等于两份 EXE 全文件相同。
# 85. 本项目已知两份 EXE 整体有10字节历史差异，所以这种边界说明尤其重要。
# 86. 本工具不检查 D8 draw order，因为那是另一个尚未固化的问题。
# 87. 不把未研究问题塞进同一个节点，可以让每个固化包的结论更容易审计和回滚。
# 88. 本工具也不尝试给 class1/class2 起作者正式名字；名称必须来自更直接的作者证据。
# 89. 同理，+0x20/+0x90 在代码中仍以偏移称呼，避免把观察到的用途冒充字段原名。
# 90. 只要后续出现新的作者字符串、调试符号或更强 consumer，就可以在新节点升级这些 UNKNOWN。



def main() -> int:
    """命令行入口；返回 0 表示所有冻结断言通过。"""

    parser = argparse.ArgumentParser(
        description="固化58：Battle九项持续场域表现槽、双实例路由、生命周期与owner清理静态调查器"
    )
    parser.add_argument("--original-exe", required=True, type=Path, help="完全未修改原版 RPG.exe")
    parser.add_argument("--research-exe", required=True, type=Path, help="历史研究版 RPG.exe，仅做关键范围等价核对")
    parser.add_argument("--firttech", required=True, type=Path, help="canonical Firttech.enc")
    parser.add_argument("--multimedia-zip", required=True, type=Path, help="用户提供的 multimedia.zip 研究子集")
    parser.add_argument("--output-json", required=True, type=Path, help="机器调查 JSON 输出路径")
    parser.add_argument("--output-csv", required=True, type=Path, help="九项白名单 CSV 输出路径")
    args = parser.parse_args()

    report = build_report(args.original_exe, args.research_exe, args.firttech, args.multimedia_zip)
    args.output_json.parent.mkdir(parents=True, exist_ok=True)
    args.output_json.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    rows = report["canonical资源证据"]["九项白名单"]
    write_csv(args.output_csv, rows)

    # 控制台只打印最重要状态，详细证据全部在 JSON/CSV 中。
    print(f"固化58静态调查：{'PASS' if report['全部通过'] else 'FAIL'}")
    print(f"JSON: {args.output_json}")
    print(f"CSV : {args.output_csv}")
    return 0 if report["全部通过"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
