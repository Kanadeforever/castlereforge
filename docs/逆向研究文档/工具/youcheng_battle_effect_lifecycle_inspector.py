#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 前28效果槽生命周期重置调查器（固化45）。

这个工具只读取 RPG.exe 和 multimedia.zip，不会修改游戏文件。
它专门重放固化45的结构级证据：

1. 0x4231D0 每次只把 FightRole+0xA34 起始的 28 个 DWORD 清零；
2. 这个函数在当前 EXE 里恰好只有四个 direct E8 caller；
3. 四个 caller 分别落在 FightRole 构造/析构复位、主生存资源归零失效、
   战斗角色装载，以及「呼叫增援 / 屍魔召喚」动态角色创建链；
4. RoleDefinition 的完整 0x350 字节会先复制进 FightRole+0x84C，之后只清前28槽，
   所以后11槽不是这个 lifecycle reset primitive 的目标；
5. multimedia/public/Firttech.enc 的真实记录再次确认 0x161 与 0x169 的中文名称。

注意：工具只证明“当前 canonical EXE / 资产中这条生命周期重置 primitive 的真实边界”。
它不擅自给作者未公开的后11槽业务含义命名，也不把它们称为 unused / reserved / dead。
"""

from __future__ import annotations

# argparse 用来解析命令行参数；这样既能人工运行，也能由封包回归脚本自动调用。
import argparse
# csv 用来输出一份直观的 caller 对照表，方便不用读 JSON 的接档者快速查看。
import csv
# hashlib 用来记录输入文件与解密资源的 SHA-256，防止以后拿错版本仍误以为结果相同。
import hashlib
# json 用来保存机器可读的完整调查结果。
import json
# struct 用来读取 PE32 的 little-endian 整数，以及解析 x86 的 E8 rel32 调用位移。
import struct
# sys 用来返回清晰的进程退出码：全部通过为0，任何关键断言失败为1。
import sys
# zipfile 直接读取 multimedia.zip，不需要把整包解压到游戏目录或临时目录。
import zipfile
# dataclass 让 PE section 描述结构清楚，避免到处传含义不明的元组下标。
from dataclasses import dataclass
# Path 统一处理 Windows / Linux 路径字符串。
from pathlib import Path
# Any 只用于 JSON 报告字典的类型注解。
from typing import Any


# 工具版本和固化节点写进常量，接档时一眼就知道报告由哪个版本生成。
TOOL_VERSION = "0.1.0-固化45"

# 当前用户提供的 RPG.exe.org 内 RPG.exe 的 SHA-256。
# 工具不会只靠哈希决定成败，因为项目已证明它与旧 Oracle 整文件哈希不同但关键代码等价；
# 这里记录它只是为了证据身份追踪。
CURRENT_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"

# Firttech.enc 解密后的已知 SHA-256；固化21以来已经反复验证。
EXPECTED_FIRTTECH_SHA256 = "5469a9dd388972b53b5661e90d85c84f2d0150d06f46ddbae159409a5ac34b1a"

# Firttech 每条记录固定 560 字节，当前文件正好 500 条。
FIRTTECH_RECORD_SIZE = 560
FIRTTECH_RECORD_COUNT = 500

# Firttech 名称字段从每条记录 +0x04 开始；这里给 64 字节窗口足够覆盖当前技能名并找到 NUL。
FIRTTECH_NAME_OFFSET = 0x04
FIRTTECH_NAME_WINDOW = 64

# 两个已经由资源正文确认的特殊召唤动作 ID。
ACTION_CALL_REINFORCEMENT = 0x161
ACTION_CORPSE_SUMMON = 0x169

# 这两个 ID 对应的真实繁体中文资源名。
EXPECTED_ACTION_NAMES = {
    ACTION_CALL_REINFORCEMENT: "呼叫增援",
    ACTION_CORPSE_SUMMON: "屍魔召喚",
}

# RoleDefinition 总长度 0x350；x86 rep movsd 复制 DWORD，所以 0x350 / 4 = 0xD4 个 DWORD。
ROLE_DEFINITION_SIZE = 0x350
ROLE_DEFINITION_DWORD_COUNT = 0xD4

# 整条 RoleDefinition 被放到 FightRole+0x84C。
FIGHTROLE_ROLEDEF_OFFSET = 0x84C

# 39个序列化/runtime状态槽位于 RoleDefinition+0x1E8，所以运行时地址是 FightRole+0xA34。
ROLEDEF_EFFECT_BLOCK_OFFSET = 0x1E8
FIGHTROLE_EFFECT_BLOCK_OFFSET = FIGHTROLE_ROLEDEF_OFFSET + ROLEDEF_EFFECT_BLOCK_OFFSET

# 0x4231D0 的专用重置只清 28 个 DWORD，也就是 raw effect ID 0..27。
RESET_SLOT_COUNT = 28
RESET_SLOT_BYTES = RESET_SLOT_COUNT * 4

# 完整序列化块有39个DWORD，后11槽从索引28开始。
SERIALIZED_SLOT_COUNT = 39
TAIL_SLOT_COUNT = SERIALIZED_SLOT_COUNT - RESET_SLOT_COUNT
TAIL_SLOT_OFFSET = FIGHTROLE_EFFECT_BLOCK_OFFSET + RESET_SLOT_BYTES

# FightRole pending数组从 +0xCC 开始，0x41F230 中会独立把39项全部清零。
FIGHTROLE_PENDING_OFFSET = 0xCC
PENDING_SLOT_COUNT = 39

# ENC 公共数据的既有解密规则是 byte XOR (absolute_index % 255)。
ENC_XOR_PERIOD = 255


@dataclass(frozen=True)
class PESection:
    """保存一个 PE section 做地址换算所需的最少字段。"""

    # name 是 .text / .rdata / .data 等 section 名称。
    name: str
    # virtual_address 是 section 相对于 ImageBase 的 RVA。
    virtual_address: int
    # virtual_size 是 section 在内存中的逻辑大小。
    virtual_size: int
    # raw_offset 是 section 在 EXE 文件中的起始偏移。
    raw_offset: int
    # raw_size 是 section 在 EXE 文件中实际占用的字节数。
    raw_size: int


@dataclass(frozen=True)
class PEImage:
    """保存 PE32 文件字节、ImageBase 与 section 表，并提供只读 VA 访问。"""

    # data 保存整个 EXE；本游戏只有几百KB，一次性读入最简单也最稳定。
    data: bytes
    # image_base 是 PE32 OptionalHeader 中声明的映像基址，当前游戏通常是 0x400000。
    image_base: int
    # sections 是解析后的只读 section 描述。
    sections: tuple[PESection, ...]

    def va_to_offset(self, va: int) -> int:
        """把运行时绝对虚拟地址 VA 换成 EXE 文件偏移。"""

        # RVA = VA - ImageBase；例如 0x4231D0 - 0x400000 = 0x231D0。
        rva = va - self.image_base

        # 逐个 section 判断这个 RVA 落在哪一段。
        for section in self.sections:
            # virtual_size 与 raw_size 有时因文件对齐不同，所以取两者较大值作为覆盖区间。
            covered = max(section.virtual_size, section.raw_size)
            # 找到包含该 RVA 的 section 后就可以计算 section 内相对位置。
            if section.virtual_address <= rva < section.virtual_address + covered:
                # inside 是 RVA 相对于这个 section 起点的距离。
                inside = rva - section.virtual_address
                # 加上磁盘 raw_offset 就得到真正的文件偏移。
                file_offset = section.raw_offset + inside
                # 坏文件可能让地址越界，所以这里明确检查而不是静默切片空字节。
                if not 0 <= file_offset < len(self.data):
                    raise ValueError(f"VA 0x{va:08X} 映射到了文件范围之外")
                return file_offset

        # 如果所有 section 都没有覆盖，说明输入地址或输入 EXE 不是本工具预期对象。
        raise ValueError(f"VA 0x{va:08X} 不属于任何已解析 PE section")

    def read_va(self, va: int, size: int) -> bytes:
        """从某个 VA 精确读取 size 个字节。"""

        # 先把 VA 换成磁盘偏移。
        offset = self.va_to_offset(va)
        # 用普通 bytes 切片拿到机器码。
        chunk = self.data[offset : offset + size]
        # 必须精确读够；否则机器断言会因为文件截断而给出误导结果。
        if len(chunk) != size:
            raise ValueError(f"VA 0x{va:08X} 需要 {size} 字节，但文件剩余不足")
        return chunk

    def text_section(self) -> tuple[int, bytes]:
        """返回 .text 的起始 VA 和磁盘字节，用于扫描 direct E8 caller。"""

        # 在 section 表中只寻找名字严格等于 .text 的代码段。
        for section in self.sections:
            if section.name == ".text":
                # 运行时起始 VA = ImageBase + section RVA。
                start_va = self.image_base + section.virtual_address
                # direct E8扫描只需要磁盘中真实存在的 raw_size 字节。
                code = self.data[section.raw_offset : section.raw_offset + section.raw_size]
                return start_va, code
        # 当前RPG.exe若没有.text就不是正确研究对象。
        raise ValueError("PE 中没有找到 .text section")


def sha256_bytes(data: bytes) -> str:
    """返回字节串的 SHA-256 十六进制文本。"""

    # hashlib.sha256 接收原始 bytes；hexdigest() 返回固定64字符的小写十六进制串。
    return hashlib.sha256(data).hexdigest()


def parse_pe32(path: Path) -> PEImage:
    """解析本工具需要的最小 PE32 结构，不依赖第三方库。"""

    # 直接把完整文件读进内存；后面所有机器断言都从同一份不可变bytes读取。
    data = path.read_bytes()
    # 太小的文件连DOS头和PE头都容不下，直接拒绝。
    if len(data) < 0x100:
        raise ValueError("输入 EXE 太小，不像有效 PE 文件")
    # Windows PE 必须先以 MZ 开头。
    if data[:2] != b"MZ":
        raise ValueError("输入 EXE 缺少 MZ 头")

    # DOS头+0x3C保存PE头的磁盘偏移。
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    # PE头必须以固定签名 PE\0\0 开始。
    if pe_offset + 24 > len(data) or data[pe_offset : pe_offset + 4] != b"PE\x00\x00":
        raise ValueError("输入 EXE 缺少有效 PE\\0\\0 签名")

    # COFF header 就在4字节PE签名之后。
    coff = pe_offset + 4
    # COFF+2 是 section 数量。
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    # COFF+16 是 OptionalHeader 大小。
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    # OptionalHeader 紧随 20字节COFF header。
    optional = coff + 20

    # PE32 的 magic 必须是0x10B；PE32+是0x20B，本项目不是后者。
    magic = struct.unpack_from("<H", data, optional)[0]
    if magic != 0x10B:
        raise ValueError(f"只支持本项目 PE32；当前 OptionalHeader.Magic=0x{magic:04X}")

    # PE32 OptionalHeader+0x1C 是 ImageBase。
    image_base = struct.unpack_from("<I", data, optional + 0x1C)[0]
    # section table 紧跟完整OptionalHeader。
    section_table = optional + optional_size

    # 用列表逐项收集，最后转tuple保证PEImage构造后不被意外修改。
    sections: list[PESection] = []
    for index in range(section_count):
        # PE section table每项固定40字节。
        offset = section_table + index * 40
        # 损坏文件的section表可能越界；这里先挡住。
        if offset + 40 > len(data):
            raise ValueError("section table 超出 EXE 文件范围")
        # section名最多8字节，以第一个NUL结尾。
        name = data[offset : offset + 8].split(b"\x00", 1)[0].decode("ascii", errors="replace")
        # section项+8起依次可取VirtualSize、VirtualAddress、SizeOfRawData、PointerToRawData。
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", data, offset + 8)
        # 每项都转成含义明确的dataclass对象。
        sections.append(
            PESection(
                name=name,
                virtual_address=virtual_address,
                virtual_size=virtual_size,
                raw_offset=raw_offset,
                raw_size=raw_size,
            )
        )

    # 返回只读PE对象；后面再也不需要重新解析文件头。
    return PEImage(data=data, image_base=image_base, sections=tuple(sections))


def scan_direct_e8_callers(image: PEImage, target_va: int) -> list[int]:
    """扫描 .text 中所有 E8 rel32，并返回真正调用到 target_va 的指令地址。"""

    # 拿到.text的运行时起始VA与原始机器码。
    text_va, code = image.text_section()
    # callers只保存最终确认目的地址等于target_va的call位置。
    callers: list[int] = []

    # E8指令至少需要1字节opcode+4字节位移，所以最后4字节不能作为候选起点。
    for offset in range(0, len(code) - 4):
        # 不是0xE8就跳过；这样不会把普通数据误当direct call。
        if code[offset] != 0xE8:
            continue
        # E8后面4字节是有符号little-endian rel32。
        relative = struct.unpack_from("<i", code, offset + 1)[0]
        # caller_va就是当前E8指令的运行时地址。
        caller_va = text_va + offset
        # x86规则：destination = 下一条指令地址(caller+5) + rel32。
        destination = caller_va + 5 + relative
        # 只有算出的目的地址精确等于目标函数才记录。
        if destination == target_va:
            callers.append(caller_va)

    # 按.text线性扫描天然已经升序，但显式sorted让函数契约更清楚。
    return sorted(callers)


def machine_check(image: PEImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    """验证某个地址的关键机器码，并把期望/实际同时写进报告。"""

    # 把例如“B9 1C 00 00 00”转换成真正bytes。
    expected = bytes.fromhex(expected_hex)
    # 只读取断言需要的最小长度，避免后续无关编译差异扩大断言范围。
    actual = image.read_va(va, len(expected))
    # 返回人类和机器都容易理解的结构。
    return {
        "地址": f"0x{va:08X}",
        "含义": meaning,
        "期望机器码": expected.hex(" "),
        "实际机器码": actual.hex(" "),
        "PASS": actual == expected,
    }


def caller_check(image: PEImage, target_va: int, expected_callers: list[int], meaning: str) -> dict[str, Any]:
    """验证一个函数在当前 .text 中的 direct E8 caller 集合是否完全符合预期。"""

    # 真正扫描当前EXE，不使用手工抄录的旧列表替代。
    actual = scan_direct_e8_callers(image, target_va)
    # 期望列表排序后再比较，避免顺序差异制造假失败。
    expected = sorted(expected_callers)
    # JSON中把地址转成统一的0xXXXXXXXX文本，接档者可直接复制到反汇编器。
    return {
        "目标函数": f"0x{target_va:08X}",
        "含义": meaning,
        "期望callers": [f"0x{value:08X}" for value in expected],
        "实际callers": [f"0x{value:08X}" for value in actual],
        "PASS": actual == expected,
    }


def decode_public_enc(raw: bytes) -> bytes:
    """按项目已确认规则 byte XOR (absolute_index % 255) 解密公共 ENC。"""

    # bytearray允许逐位置写入，比循环拼接bytes更直接也更节省临时对象。
    decoded = bytearray(len(raw))
    # enumerate同时给出绝对索引和原始加密字节。
    for index, value in enumerate(raw):
        # 每字节只与当前位置模255做XOR，这是本项目公共数据的既有算法。
        decoded[index] = value ^ (index % ENC_XOR_PERIOD)
    # 调查逻辑后面只读，所以转回不可变bytes。
    return bytes(decoded)


def find_firttech_member(archive: zipfile.ZipFile) -> str:
    """在 multimedia.zip 中大小写不敏感地寻找唯一 Firttech.enc。"""

    # candidates收集所有末尾文件名匹配者，避免硬编码压缩包顶层目录名称。
    candidates: list[str] = []
    for name in archive.namelist():
        # ZIP规范使用/，但一些工具可能保留\；先统一再比较。
        normalized = name.replace("\\", "/").lower()
        # 只接受真正名为firttech.enc的成员。
        if normalized.endswith("/firttech.enc") or normalized == "firttech.enc":
            candidates.append(name)

    # 当前canonical包必须恰好一份；多份时不能偷偷任选其一。
    if len(candidates) != 1:
        raise ValueError(f"期望 multimedia.zip 中恰好一份 Firttech.enc，实际 {len(candidates)} 份：{candidates}")
    return candidates[0]


def decode_cp950_c_string(field: bytes) -> str:
    """把固定窗口中的NUL结尾CP950/Big5字符串转换为Python文本。"""

    # 先在第一个NUL截断，后面的固定字段填充不属于文字。
    raw = field.split(b"\x00", 1)[0]
    # 原繁体资源使用CP950；errors=strict让损坏或错误偏移立即失败，而不是悄悄出现替换字符。
    return raw.decode("cp950", errors="strict")


def inspect_firttech(multimedia_zip: Path) -> dict[str, Any]:
    """读取并验证 0x161 / 0x169 两条真实 Firttech 资源记录。"""

    # 以只读模式打开压缩包；with结束后句柄自动关闭。
    with zipfile.ZipFile(multimedia_zip, "r") as archive:
        # 先找到唯一Firttech.enc成员。
        member = find_firttech_member(archive)
        # 把加密文件字节读入内存。
        encrypted = archive.read(member)

    # 用公共ENC规则解密整文件。
    decoded = decode_public_enc(encrypted)
    # 当前结构必须能被560整除，否则不能安全按记录索引。
    if len(decoded) % FIRTTECH_RECORD_SIZE != 0:
        raise ValueError(
            f"Firttech.enc 解密后大小 {len(decoded)} 不能被记录尺寸 {FIRTTECH_RECORD_SIZE} 整除"
        )
    # 实际记录数由真实文件长度计算，不只相信常量。
    record_count = len(decoded) // FIRTTECH_RECORD_SIZE
    # 当前canonical应为500条；变化时报告应明确失败提醒重新研究。
    if record_count != FIRTTECH_RECORD_COUNT:
        raise ValueError(f"Firttech记录数期望 {FIRTTECH_RECORD_COUNT}，实际 {record_count}")

    # records保存两个指定动作ID的真实名称和是否匹配。
    records: list[dict[str, Any]] = []
    for action_id, expected_name in EXPECTED_ACTION_NAMES.items():
        # 每条记录起点 = action_id × 560。
        start = action_id * FIRTTECH_RECORD_SIZE
        # 切出完整一条记录，避免跨记录读取。
        record = decoded[start : start + FIRTTECH_RECORD_SIZE]
        # 名称从+0x04开始，在64字节窗口内取NUL结尾CP950文本。
        actual_name = decode_cp950_c_string(
            record[FIRTTECH_NAME_OFFSET : FIRTTECH_NAME_OFFSET + FIRTTECH_NAME_WINDOW]
        )
        # 每条记录都独立保存PASS，未来若其中一个名称变化可以直接定位。
        records.append(
            {
                "action_id": f"0x{action_id:03X}",
                "record_index": action_id,
                "expected_name": expected_name,
                "actual_name": actual_name,
                "PASS": actual_name == expected_name,
            }
        )

    # 文件级报告同时记录加密/解密大小和SHA。
    return {
        "zip_member": member,
        "encrypted_size": len(encrypted),
        "decoded_size": len(decoded),
        "decoded_sha256": sha256_bytes(decoded),
        "expected_decoded_sha256": EXPECTED_FIRTTECH_SHA256,
        "sha256_PASS": sha256_bytes(decoded) == EXPECTED_FIRTTECH_SHA256,
        "record_size": FIRTTECH_RECORD_SIZE,
        "record_count": record_count,
        "records": records,
        "records_PASS": all(item["PASS"] for item in records),
    }


def build_machine_checks(image: PEImage) -> list[dict[str, Any]]:
    """建立固化45与结论直接相关的最小机器码断言集合。"""

    # 断言列表故意覆盖“28槽clear本体、39 pending reset、死亡失效、整记录复制、特殊召唤”五类证据。
    specs = [
        # 0x4231D0把EDI设置为FightRole+0xA34，也就是39槽块的第0槽。
        (0x004231D1, "8D B9 34 0A 00 00", "0x4231D0 清零目标起点 FightRole+0xA34"),
        # ECX=0x1C，rep stosd因此只处理28个DWORD。
        (0x004231D7, "B9 1C 00 00 00", "0x4231D0 清零计数固定为28 DWORD"),
        # 真正执行批量DWORD清零的rep stosd。
        (0x004231DE, "F3 AB", "0x4231D0 执行28项 rep stosd"),
        # 0x41F230调用前28槽clear。
        (0x0041F27D, "E8 4E 3F 00 00", "FightRole复位helper调用0x4231D0"),
        # 同一helper随后从FightRole+0xCC开始处理pending数组。
        (0x0041F284, "8D 8E CC 00 00 00", "FightRole复位helper的pending数组起点+0xCC"),
        # cmp eax,0x27代表pending循环总计39项，而不是28项。
        (0x0041F297, "83 F8 27", "FightRole复位helper独立清39项pending"),
        # 失效判断读取主生存资源FightRole+0x87C。
        (0x004231F3, "8B 86 7C 08 00 00", "失效路径读取FightRole+0x87C主生存资源"),
        # 进入失效后把FightRole+0x72C设为-1。
        (0x0042320E, "C7 86 2C 07 00 00 FF FF FF FF", "失效路径把FightRole+0x72C设为-1"),
        # 并把主生存资源FightRole+0x87C钳到0。
        (0x00423218, "C7 86 7C 08 00 00 00 00 00 00", "失效路径把FightRole+0x87C清零"),
        # 失效流程最终调用前28槽clear。
        (0x0042329B, "E8 30 FF FF FF", "主生存资源归零失效路径调用0x4231D0"),
        # FightRole构造阶段对内嵌RoleDefinition准备0xD4 DWORD清零。
        (0x0041F166, "B9 D4 00 00 00", "FightRole构造时RoleDefinition尺寸=0x350"),
        # 构造清零目标明确是FightRole+0x84C。
        (0x0041F171, "8D BE 4C 08 00 00", "FightRole构造时RoleDefinition目标+0x84C"),
        # 构造阶段真正执行整条RoleDefinition清零。
        (0x0041F184, "F3 AB", "FightRole构造时整条RoleDefinition rep stosd"),
        # API加载器正常路径同样按0xD4 DWORD复制。
        (0x0041F661, "B9 D4 00 00 00", "API加载器复制完整0x350 RoleDefinition"),
        # 真正执行rep movsd。
        (0x0041F666, "F3 A5", "API加载器执行RoleDefinition rep movsd"),
        # 战斗编队装载再次把复制计数设为0xD4 DWORD。
        (0x004426E7, "B9 D4 00 00 00", "战斗角色装载复制完整0x350 RoleDefinition"),
        # 战斗角色装载真正复制整条RoleDefinition。
        (0x004426F6, "F3 A5", "战斗角色装载执行RoleDefinition rep movsd"),
        # 随后恢复ECX为FightRole对象基址，为调用0x4231D0做准备。
        (0x004426F8, "8D 88 04 73 00 00", "战斗角色装载恢复FightRole this指针"),
        # 紧跟完整复制之后只调用前28槽clear。
        (0x00442708, "E8 C3 0A FE FF", "战斗角色装载完整复制后调用0x4231D0"),
        # 动态创建入口明确比较动作ID 0x161。
        (0x00443D9B, "3D 61 01 00 00", "动态创建链识别Firttech 0x161 呼叫增援"),
        # 同一入口明确比较动作ID 0x169。
        (0x00443DA8, "3D 69 01 00 00", "动态创建链识别Firttech 0x169 屍魔召喚"),
        # 创建新FightRole后先走0x41F2B0角色/API装载链。
        (0x00443F39, "E8 72 B3 FD FF", "特殊召唤动态角色先调用0x41F2B0"),
        # 随后立即只清前28槽。
        (0x00443F40, "E8 8B F2 FD FF", "特殊召唤动态角色紧接调用0x4231D0"),
    ]

    # 每条spec都通过统一machine_check执行，报告格式保持一致。
    return [machine_check(image, va, expected, meaning) for va, expected, meaning in specs]


def build_caller_checks(image: PEImage) -> list[dict[str, Any]]:
    """建立关键函数 direct E8 caller 集合断言。"""

    # 这些集合全部来自当前EXE整段.text扫描，不是只看局部反汇编窗口。
    specs = [
        (
            0x004231D0,
            [0x0041F27D, 0x0042329B, 0x00442708, 0x00443F40],
            "前28 runtime效果槽 lifecycle reset primitive 的全部direct callers",
        ),
        (
            0x0041F230,
            [0x0041F106, 0x0041F1DD],
            "FightRole复位helper只被构造/析构复位两处直接调用",
        ),
        (
            0x004231F0,
            [0x0042070E, 0x00421043],
            "主生存资源归零失效helper的全部direct callers",
        ),
        (
            0x0041F2B0,
            [0x0044267C, 0x00443F39],
            "角色/API装载helper分别用于战斗编队装载与特殊召唤动态创建",
        ),
        (
            0x0041F580,
            [0x0041F30D],
            "Public/API.ENC RoleDefinition加载器的唯一direct caller",
        ),
        (
            0x00443D90,
            [0x0042040A],
            "呼叫增援/屍魔召喚动态角色创建入口的唯一direct caller",
        ),
    ]

    # 使用统一caller_check逐项扫描，任何多出/缺失caller都会让PASS变false。
    return [caller_check(image, target, expected, meaning) for target, expected, meaning in specs]


def write_json(path: Path, report: dict[str, Any]) -> None:
    """以UTF-8+CRLF写机器报告。"""

    # ensure_ascii=False保留中文，不把每个汉字转成\uXXXX。
    text = json.dumps(report, ensure_ascii=False, indent=2)
    # 先统一为\n，再替换成Windows常见CRLF，避免平台混合换行。
    text = text.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n")
    # utf-8编码能同时稳定保存中文和ASCII地址。
    path.write_bytes(text.encode("utf-8"))


def write_caller_csv(path: Path, caller_checks: list[dict[str, Any]]) -> None:
    """以UTF-8-SIG+CRLF输出caller表，方便Windows Excel直接打开。"""

    # newline=""让csv模块自己负责CRLF，避免多出空行。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        # 固定列名保持不同版本报告可比较。
        writer = csv.writer(handle, lineterminator="\r\n")
        writer.writerow(["目标函数", "含义", "期望callers", "实际callers", "PASS"])
        # 一行对应一个目标函数。
        for item in caller_checks:
            # 多个caller用分号连接，避免CSV单元格里产生多行。
            expected = "; ".join(item["期望callers"])
            actual = "; ".join(item["实际callers"])
            writer.writerow([item["目标函数"], item["含义"], expected, actual, str(item["PASS"])])


def build_report(exe_path: Path, multimedia_zip: Path) -> dict[str, Any]:
    """执行固化45全部只读验证并返回完整报告。"""

    # 先解析EXE的PE结构；后续机器断言和caller扫描共用同一个PEImage。
    image = parse_pe32(exe_path)
    # 机器码断言覆盖固化45最关键地址。
    machine_checks = build_machine_checks(image)
    # caller集合断言证明0x4231D0等函数的静态直接调用面。
    caller_checks = build_caller_checks(image)
    # 资源侧读取Firttech真实名称，避免只靠旧文档给特殊召唤链命名。
    firttech = inspect_firttech(multimedia_zip)

    # 机器码全部PASS才算机器证据通过。
    machine_pass = all(item["PASS"] for item in machine_checks)
    # caller集合全部吻合才算调用面证据通过。
    callers_pass = all(item["PASS"] for item in caller_checks)
    # Firttech既要SHA吻合，也要两个名称逐条吻合。
    firttech_pass = bool(firttech["sha256_PASS"] and firttech["records_PASS"])

    # 计算关键结构偏移，报告中显式写出而不是让接档者自己心算。
    structure = {
        "RoleDefinition大小": ROLE_DEFINITION_SIZE,
        "RoleDefinition复制DWORD数": ROLE_DEFINITION_DWORD_COUNT,
        "FightRole.RoleDefinition偏移": FIGHTROLE_ROLEDEF_OFFSET,
        "RoleDefinition内39槽起点": ROLEDEF_EFFECT_BLOCK_OFFSET,
        "FightRole内39槽起点": FIGHTROLE_EFFECT_BLOCK_OFFSET,
        "完整序列化槽数": SERIALIZED_SLOT_COUNT,
        "0x4231D0清零槽数": RESET_SLOT_COUNT,
        "0x4231D0清零字节数": RESET_SLOT_BYTES,
        "后11槽起点": TAIL_SLOT_OFFSET,
        "后11槽数量": TAIL_SLOT_COUNT,
        "pending起点": FIGHTROLE_PENDING_OFFSET,
        "pending槽数": PENDING_SLOT_COUNT,
    }

    # 把研究结论写成结构化短句；这些句子严格限定在已证边界，不做作者命名推测。
    conclusions = [
        "0x4231D0 是 raw effect 0..27 的28槽生命周期重置 primitive：从 FightRole+0xA34 起仅清28个DWORD。",
        "FightRole构造/析构复位helper会调用0x4231D0，同时另行把FightRole+0xCC的39项pending全部清零。",
        "主生存资源 FightRole+0x87C 归零/失效时会调用0x4231D0，因此死亡/失效也只重置前28 runtime槽。",
        "战斗角色装载会先复制完整0x350 RoleDefinition（包含39槽），随后只调用0x4231D0清前28槽。",
        "Firttech 0x161=呼叫增援、0x169=屍魔召喚；其动态FightRole创建链在角色/API装载后立即调用0x4231D0。",
        "因此后11槽不是0x4231D0的目标；若其他版本/资产在后11槽存在非零序列化值，当前原版这条Battle生命周期初始化会保留它们。",
        "这里仍不能把后11槽命名为reserved/unused/dead；只能证明它们不属于当前已证raw effect 0..27生命周期reset primitive。",
    ]

    # overall_pass要求三个证据域同时通过。
    overall_pass = machine_pass and callers_pass and firttech_pass

    # 返回最终JSON对象；输入哈希放在最前面便于先确认研究对象身份。
    return {
        "工具版本": TOOL_VERSION,
        "固化节点": 45,
        "输入": {
            "RPG.exe": str(exe_path),
            "RPG.exe大小": len(image.data),
            "RPG.exe_SHA256": sha256_bytes(image.data),
            "当前org_SHA256参考": CURRENT_RPG_ORG_SHA256,
            "RPG.exe_SHA256_matches_current_org": sha256_bytes(image.data) == CURRENT_RPG_ORG_SHA256,
            "multimedia.zip": str(multimedia_zip),
            "multimedia.zip大小": multimedia_zip.stat().st_size,
            "multimedia.zip_SHA256": sha256_bytes(multimedia_zip.read_bytes()),
        },
        "结构边界": structure,
        "机器码断言": machine_checks,
        "机器码断言通过数": sum(1 for item in machine_checks if item["PASS"]),
        "机器码断言总数": len(machine_checks),
        "机器码断言_PASS": machine_pass,
        "direct_E8_caller断言": caller_checks,
        "direct_E8_caller断言通过数": sum(1 for item in caller_checks if item["PASS"]),
        "direct_E8_caller断言总数": len(caller_checks),
        "direct_E8_caller断言_PASS": callers_pass,
        "Firttech资源证据": firttech,
        "Firttech资源证据_PASS": firttech_pass,
        "结论": conclusions,
        "overall_pass": overall_pass,
    }


def parse_args(argv: list[str]) -> argparse.Namespace:
    """定义命令行参数。"""

    # description会在--help里直接告诉用户这个工具检查什么。
    parser = argparse.ArgumentParser(description="验证《幽城幻剑录》Battle前28效果槽生命周期重置与后11槽保留边界。")
    # --exe必须指向研究用RPG.exe。
    parser.add_argument("--exe", required=True, type=Path, help="RPG.exe 路径（只读）")
    # --multimedia必须指向包含Firttech.enc的multimedia.zip。
    parser.add_argument("--multimedia", required=True, type=Path, help="multimedia.zip 路径（只读）")
    # --json-out可选；不指定时仍会在控制台打印摘要。
    parser.add_argument("--json-out", type=Path, help="完整JSON报告输出路径")
    # --csv-out可选；用于输出caller集合表。
    parser.add_argument("--csv-out", type=Path, help="direct E8 caller对照CSV输出路径")
    # 返回解析结果，不在这里执行调查逻辑。
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    """命令行入口：运行调查、写报告并返回0/1退出码。"""

    # argv为None时使用真正命令行参数；测试时也可以传自定义列表。
    args = parse_args(sys.argv[1:] if argv is None else argv)

    # 输入不存在就立即抛出清晰错误，避免后面出现难懂的zip或PE异常。
    if not args.exe.is_file():
        raise FileNotFoundError(f"RPG.exe不存在：{args.exe}")
    if not args.multimedia.is_file():
        raise FileNotFoundError(f"multimedia.zip不存在：{args.multimedia}")

    # 执行全部只读验证。
    report = build_report(args.exe, args.multimedia)

    # 如果要求JSON输出，先创建父目录再写文件。
    if args.json_out is not None:
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        write_json(args.json_out, report)

    # 如果要求CSV输出，同样先创建父目录。
    if args.csv_out is not None:
        args.csv_out.parent.mkdir(parents=True, exist_ok=True)
        write_caller_csv(args.csv_out, report["direct_E8_caller断言"])

    # 控制台摘要只打印最关键数字，详细内容留在JSON中。
    print(f"[固化45] 机器码断言：{report['机器码断言通过数']}/{report['机器码断言总数']}")
    print(
        f"[固化45] direct E8 caller断言："
        f"{report['direct_E8_caller断言通过数']}/{report['direct_E8_caller断言总数']}"
    )
    print(f"[固化45] Firttech资源证据：{'PASS' if report['Firttech资源证据_PASS'] else 'FAIL'}")
    print(f"[固化45] overall_pass={'PASS' if report['overall_pass'] else 'FAIL'}")

    # 全部通过返回0；任何核心证据失败返回1，便于批处理/CI立即捕获。
    return 0 if report["overall_pass"] else 1


if __name__ == "__main__":
    # 把main的返回值交给操作系统作为进程退出码。
    raise SystemExit(main())
