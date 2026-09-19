#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 九项持续场域“相对阵营路由”静态调查器。

这个工具对应 Godot 兼容引擎研究“固化60”。它只读取文件，不会修改 EXE 或 ENC。
本节点研究的是固化58留下的 FIRTTECH ``+0x20`` 字段在“九项持续场域建立链”里的
机器含义。固化58已经知道这个字段会影响两个九槽 manager 的选择，但当时只能写成
“路由值 0/1/2”，还不能证明 0 和 1 究竟分别代表哪一侧。

给只学过一天编程的读者，可以把这里的目标理解成下面四个问题：

1. 施术者位于战斗角色槽 0～7 时，字段值 0/1/2 分别会把场域放到哪一个 manager？
2. 施术者位于战斗角色槽 8～15 时，同样三个值又会放到哪里？
3. 这两个 manager 能不能稳定解释成“施术者同组 / 施术者对向组”，而不是死记地址？
4. ``+0x20`` 能不能被粗暴地当成整个 FIRTTECH 数据库的全局 0/1/2 枚举？

工具会用原版机器码、500 条 canonical Firttech.enc 和九项白名单三层证据回答问题。
最终冻结的兼容实现语义是：**只在九项白名单真正进入 0x43DB20 的建立链里**，
``+0x20`` 可以安全当作“相对阵营路由选择器”：0=对向八槽组，1=同侧八槽组，
2=两个八槽组都提交。这个名字是兼容实现的中性名称，不冒充作者源码变量名。

特别重要：500 条 FIRTTECH 中 ``+0x20`` 还真实出现 3、4、16 和两个大整数值。
所以本工具明确禁止把“0/1/2 相对阵营”扩大成全表通用枚举。只有先通过 0x43DB20
九项 ID 白名单的记录，才会在这条持续场域建立路径里使用这里证明的路由语义。
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
# 一、冻结输入与结构常量
# ---------------------------------------------------------------------------

# 版本号只是输出报告里的溯源标签，不参与任何判断。
TOOL_VERSION = "0.7D-solid60.0"

# 用户已经确认 RPG.exe.org 是完全未修改原版，因此本工具始终把它当首要静态 Oracle。
EXPECTED_ORIGINAL_RPG_SHA256 = (
    "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
)

# 历史研究版只用于确认关键代码区没有被那 10 个历史补丁字节碰到。
EXPECTED_RESEARCH_RPG_SHA256 = (
    "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
)

# canonical Firttech.enc 的加密文件哈希与解密后哈希。
# 两层都检查，能防止“路径对了但附件内容换了”的情况。
EXPECTED_FIRTTECH_ENCODED_SHA256 = (
    "32b98193c037c8cd75a11534520e189c89c856c367bc1d6ac607e43d9cf52a5d"
)
EXPECTED_FIRTTECH_DECODED_SHA256 = (
    "5469a9dd388972b53b5661e90d85c84f2d0150d06f46ddbae159409a5ac34b1a"
)

# FIRTTECH 是 500 条固定 0x230 字节记录。
FIRTTECH_RECORD_COUNT = 500
FIRTTECH_RECORD_SIZE = 0x230

# 本节点实际需要读取的字段偏移。
FIRT_OFF_ID = 0x00
FIRT_OFF_NAME = 0x04
FIRT_NAME_SIZE = 0x20
FIRT_OFF_CLASS = 0x18
FIRT_OFF_ROUTE = 0x20
FIRT_OFF_DESCRIPTION = 0x17C
FIRT_DESCRIPTION_SIZE = FIRTTECH_RECORD_SIZE - FIRT_OFF_DESCRIPTION

# 固化58已经从 EXE 机器表恢复出的九项白名单。
# 本节点再次从 EXE 表中读取并和这份冻结值比较，而不是只相信手抄常量。
EXPECTED_ID_TO_SLOT: Tuple[Tuple[int, int], ...] = (
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

# 九项在 canonical FIRTTECH 里的 +0x20 值。
EXPECTED_WHITELIST_ROUTES = (0, 0, 2, 1, 0, 0, 1, 1, 0)

# 九项机器 ID -> slot 表的原版内存地址。
MAP_TABLE_VA = 0x0046BC48
MAP_ENTRY_COUNT = 9
MAP_ENTRY_SIZE = 8

# 两个九槽 manager 的固定全局地址。
# 为了避免把地址误命名成“我方/敌方”，本工具只称它们为 LOW_GROUP / HIGH_GROUP。
MANAGER_HIGH_GROUP = 0x0089FE08
MANAGER_LOW_GROUP = 0x0089FFD0

# 固化57/58已经证明，持续场域建立链里 0x43DB20 只有这两个 direct CALL。
EXPECTED_43DB20_DIRECT_CALLS = (0x004200A7, 0x004200FA)

# 本节点最核心的机器码切片。
# 第一段包含 class1/2/4 门、角色槽分组、+0x20 判断和两次 manager 提交。
# 第二段只保留真正的双 manager 路由判断，便于以后快速复核真值表。
CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (
        0x00420048,
        0x00420113,
        "class1/2/4共享尾部与双manager建立链",
        "a1eafc050c30d169bd9f6aec618355b1474d39706996eabf51d8497156649691",
    ),
    (
        0x0042006B,
        0x004200FF,
        "角色槽分组与FIRTTECH+0x20双manager路由主体",
        "783d9d8160a18786f61b43e32b4f063acfd31d4f24cfa4551a8b7d7d24cdeea6",
    ),
    (
        0x0043DB20,
        0x0043DC01,
        "九项ID白名单通过后的slot建立函数",
        "fdfb62a1b7250554ded76092f1fe817a7f8cd4253c068cd6d4dbf46ba53f5f97",
    ),
    (
        0x0046BC48,
        0x0046BC8C,
        "九项FIRTTECH-ID到slot机器表",
        "7ffd70fce4a7b96668381520c95b2a0f34555608f602756009c99e8c5519656d",
    ),
)

# 原版与研究版必须逐字节一致的范围。
EQUIVALENCE_RANGES: Tuple[Tuple[int, int, str], ...] = (
    (0x00420048, 0x00420113, "共享尾部双manager建立链"),
    (0x0043DB20, 0x0043DC01, "九项slot建立函数"),
    (0x0046BC48, 0x0046BC8C, "九项ID到slot机器表"),
)

# 为了防止“整段哈希对了，但我们对具体条件分支的理解抄错”，
# 再冻结几组短字节。每一组都对应真值表里一个非常明确的判断。
EXACT_BYTES: Tuple[Tuple[int, bytes, str], ...] = (
    (
        0x0042006B,
        bytes.fromhex("8b 0e 83 f9 07 7f 07 8b 50 20 85 d2 74 10"),
        "施术者槽<=7且route==0时进入HIGH_GROUP提交块",
    ),
    (
        0x00420079,
        bytes.fromhex("83 f9 08 7c 06 83 78 20 01 74 05 39 78 20 75 23"),
        "施术者槽>=8且route==1，或任意组route==2时允许HIGH_GROUP",
    ),
    (
        0x004200AC,
        bytes.fromhex("8b 0e 83 f9 07 7f 0c 8b 86 48 08 00 00 83 78 20 01 74 1d"),
        "施术者槽<=7且route==1时进入LOW_GROUP提交块",
    ),
    (
        0x004200BF,
        bytes.fromhex("83 f9 08 7c 0d 8b 86 48 08 00 00 8b 50 20 85 d2 74 0b"),
        "施术者槽>=8且route==0时进入LOW_GROUP提交块",
    ),
    (
        0x004200D1,
        bytes.fromhex("8b 86 48 08 00 00 39 78 20 75 23"),
        "route==2无条件允许LOW_GROUP，因此route2会双提交",
    ),
)


# ---------------------------------------------------------------------------
# 二、PE 读取结构
# ---------------------------------------------------------------------------

@dataclass(frozen=True)
class Section:
    """一个 PE section 的最小信息。"""

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
        """把反汇编器显示的虚拟地址 VA 换成硬盘文件偏移。"""

        # 反汇编器显示的是 VA；PE section 表使用的是相对 ImageBase 的 RVA。
        rva = va - self.image_base

        # 逐个 section 判断目标 RVA 落在哪一段。
        for section in self.sections:
            # 有些 PE 的 VirtualSize 与 RawSize 不完全相同，因此取较大值作为覆盖范围。
            span = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + span:
                inside = rva - section.virtual_address
                return section.raw_offset + inside

        # 地址不在任何 section 时直接报错，绝不能“猜一个文件偏移”继续读。
        raise ValueError(f"VA 0x{va:08X} 不属于 {self.path.name} 的任何 section")

    def read_va(self, va: int, size: int) -> bytes:
        """从指定 VA 读取固定长度字节。"""

        start = self.va_to_offset(va)
        end = start + size
        if end > len(self.data):
            raise ValueError(f"读取 0x{va:08X}+0x{size:X} 越过文件末尾")
        return self.data[start:end]

    def text_section(self) -> Section:
        """返回代码 section；direct CALL 扫描只允许在 .text 内执行。"""

        for section in self.sections:
            if section.name == ".text":
                return section
        raise ValueError(f"{self.path.name} 找不到 .text section")


@dataclass(frozen=True)
class FirttechRecord:
    """本节点需要的 FIRTTECH 字段视图。"""

    index: int
    record_id: int
    name: str
    action_class: int
    route_value: int
    description: str


# ---------------------------------------------------------------------------
# 三、最小二进制工具函数
# ---------------------------------------------------------------------------

def sha256_bytes(data: bytes) -> str:
    """计算一段内存字节的 SHA-256。"""

    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """逐块读取文件并计算 SHA-256，避免一次把大文件全部复制一遍。"""

    digest = hashlib.sha256()
    with path.open("rb") as handle:
        while True:
            block = handle.read(1024 * 1024)
            if not block:
                break
            digest.update(block)
    return digest.hexdigest()


def u16(data: bytes, offset: int) -> int:
    """读取 little-endian uint16。"""

    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    """读取 little-endian uint32。"""

    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    """读取 little-endian int32。FIRTTECH+0x20 必须保留有符号原值。"""

    return struct.unpack_from("<i", data, offset)[0]


def decode_cp950_c_string(raw: bytes) -> str:
    """读取固定宽度 CP950/Big5 C 字符串。"""

    # C 字符串遇到第一个 NUL 就结束；后面的填充字节不是正文。
    raw = raw.split(b"\x00", 1)[0]
    if not raw:
        return ""
    return raw.decode("cp950", errors="replace")


def parse_pe(path: Path) -> PeImage:
    """只解析本工具需要的 PE32 头和 section 表。"""

    data = path.read_bytes()

    # DOS 头必须以 MZ 开始。
    if data[:2] != b"MZ":
        raise ValueError(f"{path} 不是 MZ/PE 文件")

    # DOS+0x3C 保存 PE Header 的文件偏移。
    pe_offset = u32(data, 0x3C)
    if data[pe_offset:pe_offset + 4] != b"PE\x00\x00":
        raise ValueError(f"{path} 找不到 PE\\0\\0 签名")

    # COFF Header 紧跟 PE signature；+2 是 section 数。
    coff = pe_offset + 4
    section_count = u16(data, coff + 2)

    # +16 是 Optional Header 大小。
    optional_size = u16(data, coff + 16)
    optional = coff + 20

    # PE32 magic 必须是 0x10B。
    if u16(data, optional) != 0x10B:
        raise ValueError(f"{path} 不是预期的 PE32")

    # PE32 Optional Header +0x1C 是 ImageBase。
    image_base = u32(data, optional + 0x1C)

    # section table 从 Optional Header 末尾开始，每项40字节。
    section_table = optional + optional_size
    sections: List[Section] = []

    for index in range(section_count):
        entry = section_table + index * 40
        name = data[entry:entry + 8].split(b"\x00", 1)[0].decode("ascii", errors="replace")
        virtual_size = u32(data, entry + 8)
        virtual_address = u32(data, entry + 12)
        raw_size = u32(data, entry + 16)
        raw_offset = u32(data, entry + 20)
        sections.append(
            Section(
                name=name,
                virtual_address=virtual_address,
                virtual_size=virtual_size,
                raw_offset=raw_offset,
                raw_size=raw_size,
            )
        )

    return PeImage(path=path, data=data, image_base=image_base, sections=tuple(sections))


def decode_enc(encoded: bytes) -> bytes:
    """执行项目阶段A已经冻结的 ENC XOR 解码。"""

    # 第 i 个字节与 i mod 255 做 XOR。
    return bytes(value ^ (index % 255) for index, value in enumerate(encoded))


def parse_firttech(path: Path) -> Tuple[bytes, List[FirttechRecord]]:
    """解密并解析 500 条 FIRTTECH 记录。"""

    encoded = path.read_bytes()
    decoded = decode_enc(encoded)

    # 长度不是 500×0x230 就说明拿错表或文件损坏。
    expected_size = FIRTTECH_RECORD_COUNT * FIRTTECH_RECORD_SIZE
    if len(decoded) != expected_size:
        raise ValueError(f"Firttech 解密长度 {len(decoded)} != {expected_size}")

    records: List[FirttechRecord] = []
    for index in range(FIRTTECH_RECORD_COUNT):
        start = index * FIRTTECH_RECORD_SIZE
        raw = decoded[start:start + FIRTTECH_RECORD_SIZE]

        # 每个字段都按固定偏移读取，不通过名称猜位置。
        record_id = u32(raw, FIRT_OFF_ID)
        name = decode_cp950_c_string(raw[FIRT_OFF_NAME:FIRT_OFF_NAME + FIRT_NAME_SIZE])
        action_class = s32(raw, FIRT_OFF_CLASS)
        route_value = s32(raw, FIRT_OFF_ROUTE)
        description = decode_cp950_c_string(
            raw[FIRT_OFF_DESCRIPTION:FIRT_OFF_DESCRIPTION + FIRT_DESCRIPTION_SIZE]
        )

        records.append(
            FirttechRecord(
                index=index,
                record_id=record_id,
                name=name,
                action_class=action_class,
                route_value=route_value,
                description=description,
            )
        )

    return decoded, records


def scan_direct_rel32_calls(image: PeImage, target_va: int) -> List[int]:
    """扫描 .text 中真正解析后目标等于 target_va 的 E8 rel32 CALL。"""

    text = image.text_section()
    raw = image.data[text.raw_offset:text.raw_offset + text.raw_size]
    text_va = image.image_base + text.virtual_address
    calls: List[int] = []

    # E8 后面必须还有4个字节的 signed rel32，所以最后4个位置不能作为起点。
    for offset in range(0, len(raw) - 4):
        if raw[offset] != 0xE8:
            continue

        # rel32 是有符号32位位移，必须做 sign extension。
        displacement = struct.unpack_from("<i", raw, offset + 1)[0]
        call_va = text_va + offset
        resolved = call_va + 5 + displacement

        if resolved == target_va:
            calls.append(call_va)

    return calls


def read_machine_map(image: PeImage) -> Tuple[Tuple[int, int], ...]:
    """从 EXE 的 9×8-byte 静态表重新读取 FIRTTECH ID -> slot。"""

    raw = image.read_va(MAP_TABLE_VA, MAP_ENTRY_COUNT * MAP_ENTRY_SIZE)
    rows: List[Tuple[int, int]] = []

    for index in range(MAP_ENTRY_COUNT):
        offset = index * MAP_ENTRY_SIZE
        record_id = u32(raw, offset)
        slot = s32(raw, offset + 4)
        rows.append((record_id, slot))

    return tuple(rows)


# ---------------------------------------------------------------------------
# 四、机器真值表：把汇编条件改写成可测试的参考函数
# ---------------------------------------------------------------------------

def machine_route(actor_slot: int, route_value: int) -> Tuple[str, ...]:
    """复刻 0x42006B..0x4200FA 对 0..15 角色槽的 manager 选择。

    返回值是一个字符串元组。例如 ``("HIGH_GROUP",)`` 表示只向高8槽 manager 提交；
    ``("HIGH_GROUP", "LOW_GROUP")`` 表示两个 manager 都会尝试建立同一个场域 slot。

    这个函数不是“重新设计规则”，而是把汇编中的两段 if 条件翻译成容易检查的 Python。
    """

    if not 0 <= actor_slot <= 15:
        raise ValueError("本节点只讨论真实 Battle FightRole 槽 0..15")

    selected: List[str] = []

    # 第一段对应 0x42006B..0x4200A7，决定是否向 HIGH_GROUP(0x89FE08) 提交。
    # actor 0..7：route 0 或 2 命中。
    # actor 8..15：route 1 或 2 命中。
    if actor_slot <= 7:
        high = route_value == 0 or route_value == 2
    else:
        high = route_value == 1 or route_value == 2

    if high:
        selected.append("HIGH_GROUP")

    # 第二段对应 0x4200AC..0x4200FA，决定是否向 LOW_GROUP(0x89FFD0) 提交。
    # actor 0..7：route 1 或 2 命中。
    # actor 8..15：route 0 或 2 命中。
    if actor_slot <= 7:
        low = route_value == 1 or route_value == 2
    else:
        low = route_value == 0 or route_value == 2

    if low:
        selected.append("LOW_GROUP")

    return tuple(selected)


def relative_route_meaning(actor_slot: int, managers: Sequence[str]) -> Tuple[str, ...]:
    """把绝对 manager 地址语义转换成“施术者同组/对向组”。"""

    # 角色槽0..7属于 LOW_GROUP；8..15属于 HIGH_GROUP。
    actor_group = "LOW_GROUP" if actor_slot <= 7 else "HIGH_GROUP"
    opposite_group = "HIGH_GROUP" if actor_group == "LOW_GROUP" else "LOW_GROUP"

    result: List[str] = []
    for manager in managers:
        if manager == actor_group:
            result.append("SAME_GROUP")
        elif manager == opposite_group:
            result.append("OPPOSITE_GROUP")
        else:
            raise AssertionError(f"未知 manager 名称: {manager}")
    return tuple(result)


def build_truth_table() -> List[Dict[str, object]]:
    """生成两个代表性施术者组 × route 0/1/2 的完整真值表。"""

    rows: List[Dict[str, object]] = []

    # 用槽0代表低8组，用槽8代表高8组即可；同组内其余槽走完全相同的比较结果。
    for actor_slot in (0, 8):
        actor_group = "LOW_GROUP(0..7)" if actor_slot == 0 else "HIGH_GROUP(8..15)"
        for route_value in (0, 1, 2):
            managers = machine_route(actor_slot, route_value)
            relative = relative_route_meaning(actor_slot, managers)
            rows.append(
                {
                    "代表施术者槽": actor_slot,
                    "施术者八槽组": actor_group,
                    "FIRTTECH+0x20": route_value,
                    "机器提交manager": list(managers),
                    "相对施术者语义": list(relative),
                }
            )

    return rows


# ---------------------------------------------------------------------------
# 五、资源语料：为什么只能在九项白名单里使用这个枚举
# ---------------------------------------------------------------------------

def analyze_route_distribution(records: Sequence[FirttechRecord]) -> Dict[str, object]:
    """统计整个500条表与各machine class的+0x20原始取值。"""

    total = Counter(record.route_value for record in records)
    by_class: Dict[int, Counter[int]] = defaultdict(Counter)

    for record in records:
        by_class[record.action_class][record.route_value] += 1

    # 列出所有不是0/1/2的真实记录，证明这个字段不能被当成全表通用三值枚举。
    outside: List[Dict[str, object]] = []
    for record in records:
        if record.route_value not in (0, 1, 2):
            outside.append(
                {
                    "index": record.index,
                    "ID": f"0x{record.record_id:X}",
                    "名称": record.name,
                    "class": record.action_class,
                    "加20_signed": record.route_value,
                    "加20_u32_hex": f"0x{record.route_value & 0xFFFFFFFF:08X}",
                }
            )

    return {
        "全500条加20取值分布": {str(key): value for key, value in sorted(total.items())},
        "按class分布": {
            str(action_class): {str(key): value for key, value in sorted(counter.items())}
            for action_class, counter in sorted(by_class.items())
        },
        "非0_1_2真实记录": outside,
        "非0_1_2记录数": len(outside),
        "结论": (
            "+0x20不是整个FIRTTECH表的全局三值枚举；本节点的0/1/2相对阵营语义只冻结在"
            "通过0x43DB20九项ID白名单并进入持续场域建立链的上下文。"
        ),
    }


def analyze_whitelist(records: Sequence[FirttechRecord], machine_map: Sequence[Tuple[int, int]]) -> Dict[str, object]:
    """把九项机器白名单与 canonical 作者名称/说明逐条对齐。"""

    by_id = {record.record_id: record for record in records}
    rows: List[Dict[str, object]] = []

    for record_id, slot in machine_map:
        record = by_id.get(record_id)
        if record is None:
            raise AssertionError(f"canonical FIRTTECH 缺少机器白名单 ID 0x{record_id:X}")

        route = record.route_value

        # 这里只给0/1/2生成中性兼容语义；其它值如果出现在白名单必须立即暴露异常。
        if route == 0:
            neutral = "对向八槽组"
        elif route == 1:
            neutral = "同侧八槽组"
        elif route == 2:
            neutral = "双八槽组"
        else:
            neutral = "UNEXPECTED"

        rows.append(
            {
                "slot": slot,
                "FIRTTECH_ID": f"0x{record_id:X}",
                "名称": record.name,
                "class_加18": record.action_class,
                "route_加20": route,
                "兼容实现中性路由名": neutral,
                "作者说明": record.description,
            }
        )

    actual_routes = tuple(row["route_加20"] for row in rows)
    return {
        "九项": rows,
        "九项route严格匹配": actual_routes == EXPECTED_WHITELIST_ROUTES,
        "route0作者文本交叉": (
            "slot1封炎滅陣、slot4鬼唳天幕、slot5幽魄厲界、slot8鬼縛之陣的作者说明均明确指向敌方/敌阵；"
            "slot0非天死潭说明为空。机器真值表把route0闭合为施术者对向八槽组。"
        ),
        "route1作者文本交叉": (
            "slot3閾迦封界、slot6六甲神儀、slot7時輪返陣的作者说明均明确写伙伴/己方；"
            "机器真值表把route1闭合为施术者同侧八槽组。"
        ),
        "route2边界": (
            "当前九项中只有slot2反咒禁制route2；机器严格双manager提交，但作者说明写‘为己方’。"
            "因此兼容实现可称‘双八槽组’，不能把它冒充成作者字段枚举‘双方’。"
        ),
    }


# ---------------------------------------------------------------------------
# 六、机器证据核对
# ---------------------------------------------------------------------------

def verify_machine(original: PeImage, research: PeImage) -> Dict[str, object]:
    """核对关键切片、短字节、direct CALL、机器表与双版本等价。"""

    slice_checks: List[Dict[str, object]] = []
    for start, end, meaning, expected_hash in CRITICAL_SLICES:
        data = original.read_va(start, end - start)
        actual_hash = sha256_bytes(data)
        slice_checks.append(
            {
                "起始": f"0x{start:08X}",
                "结束_不含": f"0x{end:08X}",
                "意义": meaning,
                "长度": len(data),
                "实际SHA256": actual_hash,
                "预期SHA256": expected_hash,
                "PASS": actual_hash == expected_hash,
            }
        )

    exact_checks: List[Dict[str, object]] = []
    for va, expected, meaning in EXACT_BYTES:
        actual = original.read_va(va, len(expected))
        exact_checks.append(
            {
                "地址": f"0x{va:08X}",
                "意义": meaning,
                "实际": actual.hex(" "),
                "预期": expected.hex(" "),
                "PASS": actual == expected,
            }
        )

    direct_calls = scan_direct_rel32_calls(original, 0x0043DB20)
    machine_map = read_machine_map(original)

    equivalence: List[Dict[str, object]] = []
    for start, end, meaning in EQUIVALENCE_RANGES:
        original_bytes = original.read_va(start, end - start)
        research_bytes = research.read_va(start, end - start)
        equivalence.append(
            {
                "起始": f"0x{start:08X}",
                "结束_不含": f"0x{end:08X}",
                "意义": meaning,
                "长度": end - start,
                "原版SHA256": sha256_bytes(original_bytes),
                "研究版SHA256": sha256_bytes(research_bytes),
                "逐字节一致": original_bytes == research_bytes,
            }
        )

    all_pass = (
        all(item["PASS"] for item in slice_checks)
        and all(item["PASS"] for item in exact_checks)
        and tuple(direct_calls) == EXPECTED_43DB20_DIRECT_CALLS
        and tuple(machine_map) == EXPECTED_ID_TO_SLOT
        and all(item["逐字节一致"] for item in equivalence)
    )

    return {
        "关键区间": slice_checks,
        "短机器码": exact_checks,
        "0x43DB20_direct_CALL": [f"0x{value:08X}" for value in direct_calls],
        "0x43DB20_direct_CALL严格匹配": tuple(direct_calls) == EXPECTED_43DB20_DIRECT_CALLS,
        "九项机器映射表": [
            {"FIRTTECH_ID": f"0x{record_id:X}", "slot": slot}
            for record_id, slot in machine_map
        ],
        "九项机器映射严格匹配": tuple(machine_map) == EXPECTED_ID_TO_SLOT,
        "原版研究版等价": equivalence,
        "全部机器检查通过": all_pass,
    }


# ---------------------------------------------------------------------------
# 七、输出表
# ---------------------------------------------------------------------------

def write_csv(path: Path, rows: Sequence[Dict[str, object]]) -> None:
    """把九项白名单与路由语义写成 UTF-8-SIG CSV。"""

    path.parent.mkdir(parents=True, exist_ok=True)
    fields = [
        "slot",
        "FIRTTECH_ID",
        "名称",
        "class_加18",
        "route_加20",
        "兼容实现中性路由名",
        "作者说明",
    ]

    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def build_report(original_path: Path, research_path: Path, firttech_path: Path) -> Dict[str, object]:
    """执行固化60所有只读核对并生成最终 JSON 对象。"""

    original = parse_pe(original_path)
    research = parse_pe(research_path)
    decoded, records = parse_firttech(firttech_path)

    # 第一步先做输入哈希。输入不对时，即使后面的字节碰巧匹配，也不能签发节点结论。
    input_checks = {
        "原版RPG.exe": {
            "路径": str(original_path),
            "实际SHA256": sha256_file(original_path),
            "预期SHA256": EXPECTED_ORIGINAL_RPG_SHA256,
        },
        "研究版RPG.exe": {
            "路径": str(research_path),
            "实际SHA256": sha256_file(research_path),
            "预期SHA256": EXPECTED_RESEARCH_RPG_SHA256,
        },
        "Firttech.enc": {
            "路径": str(firttech_path),
            "加密实际SHA256": sha256_file(firttech_path),
            "加密预期SHA256": EXPECTED_FIRTTECH_ENCODED_SHA256,
            "解密实际SHA256": sha256_bytes(decoded),
            "解密预期SHA256": EXPECTED_FIRTTECH_DECODED_SHA256,
        },
    }

    input_pass = (
        input_checks["原版RPG.exe"]["实际SHA256"] == EXPECTED_ORIGINAL_RPG_SHA256
        and input_checks["研究版RPG.exe"]["实际SHA256"] == EXPECTED_RESEARCH_RPG_SHA256
        and input_checks["Firttech.enc"]["加密实际SHA256"] == EXPECTED_FIRTTECH_ENCODED_SHA256
        and input_checks["Firttech.enc"]["解密实际SHA256"] == EXPECTED_FIRTTECH_DECODED_SHA256
    )

    # 第二步核对机器代码与机器白名单。
    machine = verify_machine(original, research)
    machine_map = read_machine_map(original)

    # 第三步把500条数据的真实取值域与九项白名单分开统计。
    distribution = analyze_route_distribution(records)
    whitelist = analyze_whitelist(records, machine_map)

    # 第四步构造路由真值表，并用“相对施术者组”检查0/1/2是否稳定。
    truth_table = build_truth_table()
    expected_relative = {
        0: ("OPPOSITE_GROUP",),
        1: ("SAME_GROUP",),
        2: ("OPPOSITE_GROUP", "SAME_GROUP"),
    }

    # route2 在低组时返回 HIGH,LOW；在高组时返回 HIGH,LOW，转换成相对语义后顺序不同。
    # 所以集合比较而不是顺序比较。
    truth_pass = True
    for row in truth_table:
        route = int(row["FIRTTECH+0x20"])
        actual = set(row["相对施术者语义"])
        expected = set(expected_relative[route])
        if actual != expected:
            truth_pass = False

    all_pass = (
        input_pass
        and machine["全部机器检查通过"]
        and whitelist["九项route严格匹配"]
        and truth_pass
        and distribution["非0_1_2记录数"] > 0
    )

    return {
        "工具": "youcheng_battle_field_route_inspector.py",
        "工具版本": TOOL_VERSION,
        "节点": "v0.7D阶段中-固化60",
        "主题": "Battle九项持续场域FIRTTECH+0x20相对阵营路由与双八槽组真值表边界",
        "输入哈希": input_checks,
        "输入哈希全部通过": input_pass,
        "机器证据": machine,
        "路由真值表": truth_table,
        "路由真值表PASS": truth_pass,
        "canonical全表取值边界": distribution,
        "九项白名单作者语料": whitelist,
        "兼容实现冻结规则": {
            "适用上下文": (
                "仅当动作已通过0x43DB20九项FIRTTECH-ID白名单、准备建立持续场域slot时，"
                "把FIRTTECH+0x20解释为相对阵营路由。"
            ),
            "route0": "提交到施术者对向的八槽角色组manager。",
            "route1": "提交到施术者所在的同侧八槽角色组manager。",
            "route2": "两个八槽角色组manager都尝试提交。",
            "角色组": "FightRole slot 0..7 与 8..15 是机器分组；这里使用同侧/对向中性名称，不冒充作者‘我方/敌方’枚举。",
            "必须保留的边界": (
                "整张FIRTTECH表的+0x20存在3/4/16及大整数值，因此禁止把0/1/2路由枚举全局套到所有记录。"
            ),
            "反咒禁制例外": (
                "route2机器为双manager，但作者说明写‘为己方’；实现按机器双提交，文档不得把route2强命名成作者术语‘双方’。"
            ),
        },
        "仍保持UNKNOWN": [
            "FIRTTECH+0x20作者源码正式字段名",
            "非九项白名单记录中+0x20各其它值的跨class业务含义",
            "为何反咒禁制作者文字‘为己方’而机器建立链采用双manager",
            "两个八槽角色组的作者源码正式阵营枚举名",
            "FIRTTECH+0x90作者字段名与真实时间单位",
            "九项EFF原始容器来源",
            "D8 draw order",
        ],
        "全部通过": all_pass,
    }


# ---------------------------------------------------------------------------
# 八、给初学者看的逐步代码阅读提示
# ---------------------------------------------------------------------------
# 01. 这个脚本不会运行游戏，它只是把 EXE 和数据文件当成普通字节文件读取。
# 02. “静态调查”就是不让 CPU 真正执行游戏逻辑，而是从已经存在的机器码恢复规则。
# 03. 原版 EXE 的 SHA-256 相当于身份证；不一致就说明输入不是我们已经确认的原版。
# 04. 研究版 EXE 有10字节历史差异，所以整文件哈希不同是正常的。
# 05. 但本节点依赖的三段代码必须和原版逐字节一致，否则研究版只能退出本节点证据链。
# 06. Firttech.enc 先按阶段A已经证明的 XOR 周期255算法解密。
# 07. 每条记录固定0x230字节，所以第N条起点就是 N*0x230。
# 08. +0x18 是当前已经使用多轮的machine class，不等于作者源码枚举名。
# 09. +0x20 是本节点目标字段，但只有在特定上下文里才能给它路由语义。
# 10. 为什么强调“特定上下文”？因为500条全表里这个位置不止出现0、1、2。
# 11. 例如潜地蛰伏、钢皮铁罩等真实记录这里是4。
# 12. 还有两个class2记录这里是3，甚至个别class0记录会出现更大的32位原值。
# 13. 所以把整个字段全局写成 enum { enemy, ally, both } 会直接丢失真实数据。
# 14. 正确做法是先看当前代码路径是否真的把记录送进0x43DB20。
# 15. 0x43DB20 自己先用 FIRTTECH ID 查九项静态表。
# 16. 表里只有9个ID；不在表里的记录不会建立这套持续场域slot。
# 17. 因此本节点的路由枚举只属于这9项建立协议。
# 18. Battle 一共有16个 FightRole 槽，机器代码把0..7和8..15分成两组。
# 19. 这里把0..7叫 LOW_GROUP，是因为它们编号较低，不表示“玩家”或“我方”。
# 20. 把8..15叫 HIGH_GROUP，也只是中性地址/编号名称。
# 21. 施术者槽号从当前 FightRole+0x00 读取。
# 22. 第一段条件决定是否向 HIGH_GROUP manager 0x89FE08 提交。
# 23. 当施术者在0..7时，route0会向HIGH_GROUP提交。
# 24. 对0..7施术者来说，HIGH_GROUP正好是“对向组”。
# 25. 当施术者在8..15时，route1才会向HIGH_GROUP提交。
# 26. 对8..15施术者来说，HIGH_GROUP正好是“同组”。
# 27. 两种情况合起来，route0不能简单叫“高组”，route1也不能简单叫“低组”。
# 28. 它们真正稳定的相对语义是：route0=对向，route1=同侧。
# 29. 第二段条件决定是否向 LOW_GROUP manager 0x89FFD0 提交。
# 30. 施术者在0..7时，route1向LOW_GROUP提交，也就是同组。
# 31. 施术者在8..15时，route0向LOW_GROUP提交，也就是对向组。
# 32. 这和第一段形成完全对称的真值表，是本节点最强的机器结构证据。
# 33. route2在第一段不会被0/1条件挡住，会进入HIGH_GROUP提交。
# 34. route2在第二段也不会被0/1条件挡住，会进入LOW_GROUP提交。
# 35. 因此route2的机器事实不是“任选一边”，而是“两边都尝试建立”。
# 36. “尝试”二字也很重要：0x43DB20仍会检查白名单和slot是否已经active。
# 37. 所以双manager路由不等于两个请求都保证成功，只等于两边都会进入提交函数。
# 38. 九项资源中route0共有5条，其中非天死潭没有作者说明。
# 39. 另外4条route0作者文字明确出现敌方、敌阵等词，和机器“对向组”一致。
# 40. route1共有3条，作者说明明确写伙伴或己方，和机器“同侧组”一致。
# 41. 这种作者文本只能用来支持业务理解，不能反过来覆盖机器分支。
# 42. 反咒禁制route2就是最典型的例子：作者说明写为己方，机器却确实双manager提交。
# 43. 兼容引擎首先要复刻机器真值表，而不是为了让文字好看而改成单侧。
# 44. 如果以后找到作者源码字段名，再把中性名称升级成正式名称；现在不能提前编造。
# 45. scan_direct_rel32_calls 只把真正解析后目标等于0x43DB20的E8当作CALL。
# 46. 单纯在文件里搜索E8字节是不可靠的，因为普通数据和立即数也能包含E8。
# 47. 固化55已经踩过这个坑，因此后续工具统一解析 signed rel32。
# 48. 整段SHA断言保护的是控制流整体；短字节断言保护的是我们实际解释的几个分支。
# 49. 两种断言同时使用，比只检查一条CALL更难“检查通过但解释错行”。
# 50. build_truth_table 不直接手写最终表，而是调用 machine_route 现场计算。
# 51. 这样以后如果维护者改错一个分支，真值表和冻结预期会一起暴露FAIL。
# 52. relative_route_meaning 再把绝对manager名称换成 SAME_GROUP / OPPOSITE_GROUP。
# 53. 对route0，不论施术者属于哪一组，结果集合都必须只有OPPOSITE_GROUP。
# 54. 对route1，结果集合必须只有SAME_GROUP。
# 55. 对route2，结果集合必须同时含SAME_GROUP和OPPOSITE_GROUP。
# 56. route2的列表顺序会随施术者组不同，所以校验使用集合，不错误要求固定顺序。
# 57. “同侧/对向”是兼容实现中性语义，比“我方/敌方”更安全。
# 58. 因为敌方角色自己施放时，它的同侧组对玩家来说恰恰是敌方。
# 59. 如果把route1硬写成“player_side”，敌方施法就会被实现错。
# 60. 所以实现时要先根据caster slot得到caster_group，再计算相对目标组。
# 61. 具体代码可以把0..7和8..15封装成两个BattleSideGroup对象。
# 62. 但不要把“8个槽”误写成“最多8个活人”；这是容器槽分组，不是人数结论。
# 63. 九项白名单仍然必须先于route判断。
# 64. 也就是说一个普通FIRTTECH即使+0x20==0，也不能因此自动创建对向场域。
# 65. 只有ID匹配九项表，且slot未被占用时，0x43DB20才真正建立场域。
# 66. +0x20的其它值3/4等当前不在这条九项建立链中产生新的第三/第四manager。
# 67. 它们属于其它machine class或其它行为协议，后续需要独立consumer才能命名。
# 68. 本节点因此关闭的是“九项场域路由”，不是“FIRTTECH+0x20整个数据库字段”。
# 69. CSV只导出九项白名单，因为它是兼容引擎实际实现这条协议最直接的表。
# 70. JSON还保留500条取值分布和非0/1/2记录，作为防止未来过度简化的机器护栏。
# 71. 如果未来资源版本新增白名单项，机器表哈希或映射断言会FAIL，提醒重新研究。
# 72. 如果只是普通FIRTTECH新增route4记录，全局分布会变化，但不能自动改九项路由语义。
# 73. 原版与研究版关键区相同，说明历史10字节补丁没有污染本节点。
# 74. 最终“全部通过”只有输入、机器、白名单、真值表和非三值边界一起通过才为true。
# 75. 任意一层失败都应该阻止固化节点签发，而不是删掉失败项继续打包。


# ---------------------------------------------------------------------------
# 九、命令行入口
# ---------------------------------------------------------------------------

def main() -> int:
    """解析参数、运行调查、写 JSON/CSV，并用退出码表示成败。"""

    parser = argparse.ArgumentParser(
        description="核对《幽城幻剑录》Battle九项持续场域FIRTTECH+0x20相对阵营路由。"
    )
    parser.add_argument("--original-exe", required=True, type=Path, help="完全未修改原版 RPG.exe.org 解出的 RPG.exe")
    parser.add_argument("--research-exe", required=True, type=Path, help="历史研究版 RPG.exe")
    parser.add_argument("--firttech", required=True, type=Path, help="canonical Firttech.enc")
    parser.add_argument("--json", required=True, type=Path, help="输出机器核对 JSON")
    parser.add_argument("--csv", required=True, type=Path, help="输出九项白名单路由 CSV")
    args = parser.parse_args()

    # 先生成完整报告；任何输入/解析异常都会直接抛错并停止，不写伪PASS。
    report = build_report(args.original_exe, args.research_exe, args.firttech)

    # JSON 使用UTF-8、中文不转义，并固定缩进，便于人工diff。
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    # CSV只写九项白名单，方便兼容引擎实现时直接查阅。
    write_csv(args.csv, report["九项白名单作者语料"]["九项"])

    # 控制台只输出最关键的摘要，不用几百行JSON淹没人眼。
    print(f"tool={report['工具']} version={report['工具版本']}")
    print(f"route_truth_table_pass={report['路由真值表PASS']}")
    print(f"non_012_records={report['canonical全表取值边界']['非0_1_2记录数']}")
    print(f"all_pass={report['全部通过']}")

    # 0表示成功，1表示静态断言没有全部通过；CI/批处理可以直接看退出码。
    return 0 if report["全部通过"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
