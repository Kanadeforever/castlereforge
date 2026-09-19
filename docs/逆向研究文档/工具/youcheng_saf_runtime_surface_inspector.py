#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》SAF 运行时请求面与资源语料静态调查器
版本：0.1.0-固化36

本工具只读取文件，不会修改 RPG.exe、DAT 容器、SF2/SAF 资源或任何游戏文件。

它专门重放固化36新增的三类证据：
1. RPG.exe 里与 ITF000/SAF 有关的“直接调用面”负证据；
2. ItfDir.DAT 解出的 101 份 ITF 中，SAF 与 100 份标准 SF2 的结构/几何差异；
3. SAF 的 125 个 25×20 tile 是否能在 4978 个标准 64×48 tile 内找到完全相同的 25×20 子矩形。

为什么这里强调“直接调用面”？
因为静态逆向不能把“没有找到直接 E8 caller”错误升级成“运行时绝对不可能调用”。
函数指针、虚表、脚本数字编码、运行时拼出来的名字，仍可能绕开这里能静态枚举的路径。
所以工具只证明：截至固化36，在当前这份 EXE 与当前资源语料中，已枚举的直接路径没有定位到 SAF Reader/Requester。

依赖：仅 Python 3 标准库。
"""

# argparse 负责读取命令行参数。用户可以只做 EXE 检查，也可以同时给 ITF 目录和资源提取目录。
import argparse
# hashlib 用 SHA-256 固定输入文件身份，避免以后拿错 EXE/资源却误以为回归通过。
import hashlib
# json 用于写出机器可复核的结构化报告。
import json
# struct 用小端序读取 PE、SF2、SAF 里的 16/32 位整数。
import struct
# zlib 用于还原标准 SF2 和 SAF 共用的 0x6F 压缩尾包装。
import zlib
# defaultdict 让相同 64 位窗口哈希可以映射到多个 SAF tile 编号。
from collections import defaultdict
# Path 统一处理 Windows/Linux 路径，也让工具不依赖当前工作目录。
from pathlib import Path
# 类型提示不会改变程序行为，但能让接档者更容易知道每个函数需要什么数据。
from typing import Any, Dict, Iterable, List, Optional, Sequence, Tuple


# 每份 JSON 都写入版本号；以后只拿到报告也能知道是哪一版工具产生的。
TOOL_VERSION = "0.1.0-固化36"
# 当前用户提供的 RPG.exe.org 哈希。它与历史 canonical Oracle 整文件哈希不同，但已覆盖代码区持续通过机器码回归。
KNOWN_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
# 标准 SF2 的固定 Header 大小；同时也是 0x6F 包装的固定前缀切分点。
SF2_HEADER_SIZE = 0x42BC
# SAF 的逻辑 Header 大小。注意：SAF 的逻辑 Header 是 0x74，但压缩尾仍从 0x42BC 之后开始。
SAF_HEADER_SIZE = 0x74
# SAF 的四段描述表从 +0x0C 开始；标准 SF2 则从 +0x14 开始，这是固化35/36的重要结构分流点。
SAF_SECTION_TABLE = 0x0C
# 64 位滚动哈希用这个掩码模拟无符号 64 位自然溢出。
MASK64 = (1 << 64) - 1
# 横向、纵向分别使用不同的奇数基数，减少规则像素图产生系统性碰撞的概率。
HASH_BASE_X = 1_000_003
HASH_BASE_Y = 1_000_033
# RLE 的透明像素不能和真实颜色 0x0000 混淆，因此用 16 位颜色范围以外的 0x10000 表示透明。
TRANSPARENT_SENTINEL = 0x10000


# ------------------------------
# 通用小工具
# ------------------------------

def sha256_bytes(data: bytes) -> str:
    """计算一段字节的 SHA-256。"""
    # hashlib.sha256 只读取输入；hexdigest() 返回方便人工复制核对的十六进制字符串。
    return hashlib.sha256(data).hexdigest()


def write_json(path: Path, value: Any) -> None:
    """把 Python 对象以 UTF-8、缩进格式写成 JSON。"""
    # 输出目录可能还不存在，先创建它，避免因为目录问题中断研究回归。
    path.parent.mkdir(parents=True, exist_ok=True)
    # ensure_ascii=False 保留简体中文；indent=2 让人也能直接审查；最后补换行符合文本文件惯例。
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def u16(data: bytes, offset: int) -> int:
    """从 offset 读取小端 u16，并先做边界检查。"""
    # 如果不足 2 字节，主动报出易懂的结构错误，而不是让 struct 抛出难懂异常。
    if offset < 0 or offset + 2 > len(data):
        raise ValueError(f"u16 越界：offset=0x{offset:X}, size={len(data)}")
    # <H = little-endian unsigned short。
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    """从 offset 读取小端 u32，并先做边界检查。"""
    # 读取 4 字节前同样检查边界，保证任何失败都能追到具体字段。
    if offset < 0 or offset + 4 > len(data):
        raise ValueError(f"u32 越界：offset=0x{offset:X}, size={len(data)}")
    # <I = little-endian unsigned int。
    return struct.unpack_from("<I", data, offset)[0]


# ------------------------------
# PE / RPG.exe 机器码检查
# ------------------------------

class PEImage:
    """只实现本调查需要的最小 PE32 读取器：VA→文件偏移、.text、C 字符串。"""

    def __init__(self, path: Path) -> None:
        # 一次性只读载入整个 EXE，后续所有机器码断言都针对同一份 bytes，避免文件中途变化。
        self.path = path
        self.data = path.read_bytes()
        # DOS Header +0x3C 保存 PE Header 文件偏移。
        if len(self.data) < 0x40:
            raise ValueError("EXE 太短，无法读取 DOS/PE Header")
        pe_offset = u32(self.data, 0x3C)
        # PE Signature 应为 ASCII 'PE\0\0'；不是则说明输入根本不是普通 PE。
        if pe_offset + 24 > len(self.data) or self.data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("找不到合法 PE Signature")
        # COFF Header +2 是 section 数量；+20 是 Optional Header 长度。
        section_count = u16(self.data, pe_offset + 6)
        optional_size = u16(self.data, pe_offset + 20)
        optional = pe_offset + 24
        # PE32 Optional Header Magic 必须是 0x10B；本游戏是 32 位程序。
        if u16(self.data, optional) != 0x10B:
            raise ValueError("当前工具只支持本项目已确认的 PE32 RPG.exe")
        # PE32 ImageBase 位于 Optional Header +0x1C。
        self.image_base = u32(self.data, optional + 0x1C)
        # Section Table 紧跟 Optional Header。
        section_table = optional + optional_size
        self.sections: List[Dict[str, Any]] = []
        # 逐项记录 VA/虚拟大小/原始大小/原始偏移，供 va_to_offset 使用。
        for index in range(section_count):
            entry = section_table + index * 40
            if entry + 40 > len(self.data):
                raise ValueError("PE Section Table 被截断")
            name = self.data[entry:entry + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            virtual_size = u32(self.data, entry + 8)
            virtual_address = u32(self.data, entry + 12)
            raw_size = u32(self.data, entry + 16)
            raw_pointer = u32(self.data, entry + 20)
            self.sections.append({
                "名称": name,
                "RVA": virtual_address,
                "虚拟大小": virtual_size,
                "原始大小": raw_size,
                "原始偏移": raw_pointer,
            })

    def va_to_offset(self, va: int) -> int:
        """把进程里的 VA 映射回 EXE 文件偏移。"""
        # RVA = VA - ImageBase；如果 VA 比 ImageBase 小，显然不是本映像地址。
        rva = va - self.image_base
        if rva < 0:
            raise ValueError(f"VA 低于 ImageBase：0x{va:X}")
        # PE Section 的有效覆盖取 virtual/raw size 的较大值，兼容磁盘与内存尺寸差异。
        for section in self.sections:
            start = section["RVA"]
            span = max(section["虚拟大小"], section["原始大小"])
            if start <= rva < start + span:
                offset = section["原始偏移"] + (rva - start)
                if offset >= len(self.data):
                    raise ValueError(f"VA 映射超出文件：0x{va:X}")
                return offset
        raise ValueError(f"VA 不属于任何 Section：0x{va:X}")

    def read_va(self, va: int, size: int) -> bytes:
        """按 VA 读取固定长度机器码/数据。"""
        # 先映射起点，再检查末尾没有超出文件。
        offset = self.va_to_offset(va)
        if offset + size > len(self.data):
            raise ValueError(f"VA 读取越界：0x{va:X}+0x{size:X}")
        return self.data[offset:offset + size]

    def c_string(self, va: int, encoding: str = "cp950", max_length: int = 512) -> str:
        """读取以 NUL 结束的游戏字符串。"""
        # 从地址映射出的文件偏移开始，最多看 max_length 字节，避免损坏数据无限向后扫。
        offset = self.va_to_offset(va)
        end_limit = min(len(self.data), offset + max_length)
        end = self.data.find(b"\0", offset, end_limit)
        if end < 0:
            raise ValueError(f"0x{va:X} 在 {max_length} 字节内没有 NUL 终止")
        # 游戏本体字符串按台湾版 CP950 解码；errors=replace 只影响异常字节的显示，不影响机器码判断。
        return self.data[offset:end].decode(encoding, errors="replace")

    def text_bytes(self) -> Tuple[int, bytes]:
        """返回 .text 的起始 VA 与原始字节。"""
        # 直接按 section 名找 .text，避免把资源区里偶然出现的 E8 字节误当作 CALL。
        for section in self.sections:
            if section["名称"] == ".text":
                offset = section["原始偏移"]
                size = section["原始大小"]
                return self.image_base + section["RVA"], self.data[offset:offset + size]
        raise ValueError("PE 中没有 .text Section")

    def direct_e8_callers(self, target_va: int) -> List[int]:
        """枚举 .text 中机器码 E8 rel32 直接调用 target_va 的 callsite VA。"""
        text_va, text = self.text_bytes()
        callers: List[int] = []
        # E8 后必须还有 4 字节 signed rel32，因此最后 4 字节不能作为起点。
        for index in range(0, len(text) - 4):
            # 只有首字节 E8 才按 near relative CALL 解码。
            if text[index] != 0xE8:
                continue
            # rel32 是有符号 32 位；目标 = 下一条指令地址 + rel32。
            rel = struct.unpack_from("<i", text, index + 1)[0]
            callsite = text_va + index
            target = callsite + 5 + rel
            if target == target_va:
                callers.append(callsite)
        return callers


def call_surface_assert(pe: PEImage, target: int, expected: Sequence[int], meaning: str) -> Dict[str, Any]:
    """重放“某函数所有直接 E8 caller 必须恰好是这组地址”的断言。"""
    # 真实枚举结果排序后与硬编码证据排序比较，防止仅检查数量而漏掉“旧 caller 消失/新 caller 出现”。
    actual = pe.direct_e8_callers(target)
    expected_sorted = sorted(expected)
    passed = actual == expected_sorted
    return {
        "目标VA": f"0x{target:08X}",
        "语义": meaning,
        "预期直接E8调用数": len(expected_sorted),
        "实际直接E8调用数": len(actual),
        "预期调用点": [f"0x{x:08X}" for x in expected_sorted],
        "实际调用点": [f"0x{x:08X}" for x in actual],
        "通过": passed,
    }


def string_assert(pe: PEImage, va: int, expected: str, meaning: str) -> Dict[str, Any]:
    """重放一个固定字符串地址，证明路径分类不是凭人工记忆。"""
    # 直接从 EXE 当前字节重新读字符串；如果地址或文本变化，断言会失败。
    actual = pe.c_string(va)
    return {
        "VA": f"0x{va:08X}",
        "语义": meaning,
        "预期": expected,
        "实际": actual,
        "通过": actual == expected,
    }


def verify_exe_runtime_surface(exe_path: Path) -> Dict[str, Any]:
    """对固化36所依赖的直接文件/容器/SF2 调用面做机器级回归。"""
    pe = PEImage(exe_path)
    exe_sha = sha256_bytes(pe.data)

    # 这些列表来自对当前 RPG.exe.org .text 的完整 E8 rel32 枚举，而不是手抄“几个代表点”。
    call_checks = [
        call_surface_assert(
            pe, 0x4417C0,
            [
                0x40F946, 0x40F9A1, 0x415859, 0x41F5BE, 0x4242E2, 0x424377, 0x4257E7,
                0x428456, 0x42D155, 0x431DC9, 0x431FAC, 0x433E11, 0x439E65, 0x439EE0,
                0x439F61, 0x43B3E9, 0x43B5A1, 0x43BB4A, 0x44175C, 0x4420F4, 0x442153,
                0x4422E0, 0x44234A, 0x444110, 0x444191, 0x44592C,
            ],
            "通用文件对象 Open：固化36枚举全部直接E8调用面",
        ),
        call_surface_assert(
            pe, 0x441740,
            [0x432CB9, 0x432D99, 0x43388A],
            "整文件读取到新分配缓冲区：全部直接E8调用点",
        ),
        call_surface_assert(
            pe, 0x432B20,
            [0x432EBC],
            "资源缓存内部主读取函数：直接入口只来自0x432EB0包装",
        ),
        call_surface_assert(
            pe, 0x432EB0,
            [0x428BDF],
            "资源缓存对外包装：当前直接caller仅标准SF2构造器0x428B60内部",
        ),
        call_surface_assert(
            pe, 0x433820,
            [0x4015B9],
            "另一整文件读取使用者：当前直接caller仅字体初始化",
        ),
        call_surface_assert(
            pe, 0x449F50,
            [0x449FE9, 0x44A02D, 0x44A07D],
            "挂载资源路径/别名解析：只由三个文件API包装调用",
        ),
        call_surface_assert(
            pe, 0x449B50,
            [0x449FFB, 0x44A041, 0x44A090],
            "容器按文件名查目录项：三个外层包装路径",
        ),
        call_surface_assert(
            pe, 0x449B70,
            [0x44A04E],
            "容器按已解析索引取目录记录：没有独立外部直接E8入口",
        ),
        call_surface_assert(
            pe, 0x449BA0,
            [0x44A0AC],
            "容器记录数据读取：只从外层读取包装进入",
        ),
        call_surface_assert(
            pe, 0x449FE0,
            [0x44182A, 0x44192A],
            "挂载文件存在/查询包装：直接外部使用位于通用文件对象",
        ),
        call_surface_assert(
            pe, 0x44A020,
            [0x4419B6],
            "挂载文件大小/记录定位包装：直接外部使用位于通用文件对象",
        ),
        call_surface_assert(
            pe, 0x44A070,
            [0x441A75],
            "挂载文件读取包装：直接外部使用位于通用文件对象",
        ),
        call_surface_assert(
            pe, 0x44CD7B,
            [0x4071D9, 0x428E48],
            "canonical RGB16 RLE/像素转换辅助函数：全部直接E8 caller仍只有两条标准SF2链",
        ),
    ]

    # 固化36把两个动态 SF2 名称来源和几个容易误判的 %03d 格式串也固定下来。
    string_checks = [
        string_assert(pe, 0x469F6C, "%s%s\\%s.SF2", "BaseRole动态SF2路径格式"),
        string_assert(pe, 0x469F78, "FIGHT\\", "BaseRole动态SF2路径固定目录"),
        string_assert(pe, 0x46A424, "%s%03d%s", "保存路径三位编号格式；不是ITF编号生成器"),
        string_assert(pe, 0x46A430, "Save\\Save", "上述%03d格式的固定保存前缀"),
        string_assert(pe, 0x46A43C, ".TSF", "上述%03d格式的保存扩展名"),
        string_assert(pe, 0x46BB04, "%s%03d", "另一三位编号格式；位于ENC/Save相邻路径族"),
        string_assert(pe, 0x46BB14, "%s\\%s%s", "同区域通用路径拼接格式"),
        string_assert(pe, 0x468070, "..\\MultiMedia\\Font\\Font24.Fnt", "0x433820唯一caller传入的固定字体文件"),
        string_assert(pe, 0x46C6F0, "Fight\\ITF\\ITF001.SF2", "战斗界面初始化已证的标准ITF资源"),
    ]

    # SAF 描述表第0段偏移字段位于 base+0x0E，是非常不寻常的未对齐 dword。
    # 这里直接扫描 .text 字节对应的常见 x86 编码形态：MOV r32,[r32+0x0E]。
    # 该检查不是“证明所有可能编译写法都不存在”，而是记录当前 EXE 没有最直接的结构体字段读取模式。
    text_va, text = pe.text_bytes()
    unaligned_dword_hits: List[Dict[str, Any]] = []
    # ModRM 形式 01 reg r/m + disp8，opcode 8B；r/m=100 还会进入 SIB，故这里只枚举不使用ESP基址的简单寄存器形式。
    for index in range(0, len(text) - 3):
        if text[index] != 0x8B:
            continue
        modrm = text[index + 1]
        # mod bits=01 表示 [base + signed disp8]。
        # r/m=4 是 ESP/SIB，需要额外解析；r/m=5 是 EBP，也就是典型栈局部变量。
        # SAF Reader 特征关注“资源基址寄存器 + 偏移”，所以这两种栈/复杂寻址都不算直接结构字段证据。
        base_rm = modrm & 0x07
        if (modrm & 0xC0) != 0x40 or base_rm in (0x04, 0x05):
            continue
        # x86 的 0x66 是操作数宽度前缀：`66 8B ...` 读取的是16位 WORD，不是 SAF 描述表需要的32位 DWORD。
        # 固化36第一次工具回归正是靠这个检查消除了 0x43E510/0x43E983 两个“word读被误报成dword读”的假阳性。
        if index > 0 and text[index - 1] == 0x66:
            continue
        disp = text[index + 2]
        if disp in (0x0E, 0x22):
            unaligned_dword_hits.append({
                "VA": f"0x{text_va + index:08X}",
                "字段偏移": f"0x{disp:02X}",
                "机器码": text[index:index + 3].hex(" "),
            })

    # 所有机器断言都通过才给总状态 PASS；只要一个 caller 列表变化，就要求人工重新审查。
    all_pass = all(item["通过"] for item in call_checks) and all(item["通过"] for item in string_checks)
    return {
        "工具版本": TOOL_VERSION,
        "EXE": str(exe_path),
        "EXE_SHA256": exe_sha,
        "当前RPG_org整文件身份匹配": exe_sha == KNOWN_RPG_ORG_SHA256,
        "直接E8调用面断言": call_checks,
        "关键路径字符串断言": string_checks,
        "SAF未对齐描述偏移常见dword读取特征": {
            "扫描说明": "扫描.text中opcode 8B、非ESP/EBP简单寄存器基址、disp8=0x0E或0x22的MOV r32,[base+disp8]，并排除紧邻0x66的16位操作数前缀；不覆盖SIB/复制内存/间接解释器等其它写法",
            "命中数": len(unaligned_dword_hits),
            "命中": unaligned_dword_hits,
            "固化36解释": "当前常见直接结构体读取形态没有出现SAF +0x0E/+0x22 的dword consumer；这是负证据，不是运行时绝对不可达证明。",
        },
        "全部机器断言通过": all_pass,
    }


# ------------------------------
# SF2 / SAF 最小解析与像素解码
# ------------------------------

def rebuild_legacy_tail(raw: bytes, logical_magic: bytes, logical_header_size: int) -> bytes:
    """还原标准 SF2/SAF 共用的 0x6F 外层压缩尾。"""
    # 最少要能读取到 +0x0B 的包装标志。
    if len(raw) < 0x0C:
        raise ValueError("动画文件太短")
    # magic 参数由调用者明确给出，避免把 SAF 当 SF2 或反过来。
    if raw[:len(logical_magic)] != logical_magic:
        raise ValueError(f"动画魔数不匹配：期望 {logical_magic!r}，实际 {raw[:len(logical_magic)]!r}")
    # 未压缩文件直接返回；逻辑 Header 大小只用于后面的结构检查，不等于压缩切分点。
    if raw[0x0B] != 0x6F:
        return raw
    # 两种格式当前样本都证明：压缩尾切分位置仍固定在 0x42BC，而不是 SAF 的 0x74。
    if len(raw) < SF2_HEADER_SIZE + 8:
        raise ValueError("0x6F 包装缺少双最终尺寸")
    final1 = u32(raw, SF2_HEADER_SIZE)
    final2 = u32(raw, SF2_HEADER_SIZE + 4)
    if final1 != final2:
        raise ValueError("0x6F 双最终尺寸不一致")
    # 真正 zlib 流从两个尺寸字段之后开始。
    tail = zlib.decompress(raw[SF2_HEADER_SIZE + 8:])
    rebuilt = bytearray(raw[:SF2_HEADER_SIZE] + tail)
    # 逻辑内容里包装标志恢复为 0，与原版标准 SF2 解包后的内存形态一致。
    rebuilt[0x0B] = 0
    if len(rebuilt) != final1:
        raise ValueError(f"0x6F 重建大小不一致：{len(rebuilt)} != {final1}")
    # 参数 logical_header_size 在这里仅用于帮助调用者表达“逻辑格式”，避免误把0x42BC叫成SAF Header。
    _ = logical_header_size
    return bytes(rebuilt)


def parse_sections(data: bytes, table_offset: int) -> List[List[Tuple[int, int]]]:
    """读取四个 10-byte section descriptor，并返回每段各 record 的 [start,end)。"""
    result: List[List[Tuple[int, int]]] = []
    # SF2 与 SAF 都有四段；差别在描述表起点。
    for section_index in range(4):
        descriptor = table_offset + section_index * 10
        count = u16(data, descriptor)
        section_offset = u32(data, descriptor + 2)
        section_size = u32(data, descriptor + 6)
        section_end = section_offset + section_size
        if section_end > len(data):
            raise ValueError(f"Section{section_index} 超出文件")
        if section_offset + count * 4 > section_end:
            raise ValueError(f"Section{section_index} 指针表超出本段")
        pointers = [u32(data, section_offset + i * 4) for i in range(count)]
        ends = pointers[1:] + [section_end]
        records: List[Tuple[int, int]] = []
        for start, end in zip(pointers, ends):
            if start < section_offset + count * 4 or end < start or end > section_end:
                raise ValueError(f"Section{section_index} record 边界非法")
            records.append((start, end))
        result.append(records)
    return result


def decode_rgb16_rle(record: bytes, width: int, height: int) -> List[int]:
    """把一个 RGB16 RLE tile 解成 width*height 个整数；透明写成 0x10000。"""
    expected = width * height
    pixels: List[int] = []
    cursor = 0
    # 每轮读取一个 control byte，直到恰好填满 tile 面积。
    while len(pixels) < expected:
        if cursor >= len(record):
            raise ValueError("RLE 在 tile 填满前提前结束")
        control = record[cursor]
        cursor += 1
        run = (control & 0x3F) + 1
        # bit6=0：literal，后面跟 run 个 16-bit 像素。
        if (control & 0x40) == 0:
            need = run * 2
            if cursor + need > len(record):
                raise ValueError("RLE literal 越界")
            for i in range(run):
                pixels.append(u16(record, cursor + i * 2))
            cursor += need
        # bit6=1且bit7=1：repeat，后面一个颜色重复 run 次。
        elif control & 0x80:
            if cursor + 2 > len(record):
                raise ValueError("RLE repeat 缺少颜色")
            color = u16(record, cursor)
            cursor += 2
            pixels.extend([color] * run)
        # bit6=1且bit7=0：transparent skip，没有 payload，只向输出推进 run 个透明像素。
        else:
            pixels.extend([TRANSPARENT_SENTINEL] * run)
        # 任何 run 把输出推进超过 tile 面积都说明算法/结构不匹配。
        if len(pixels) > expected:
            raise ValueError("RLE 输出超过 tile 面积")
    # 固化35要求输入字节也精确消费；有尾字节就不能算严格解码通过。
    if cursor != len(record):
        raise ValueError(f"RLE 未精确消费：{cursor}/{len(record)}")
    return pixels


def load_saf_tiles(path: Path) -> Tuple[int, int, List[List[int]]]:
    """读取 ITF000.SF2/SAF 的 125 个 Section2 tile。"""
    raw = path.read_bytes()
    data = rebuild_legacy_tail(raw, b"SAF\x05", SAF_HEADER_SIZE)
    if u16(data, 0x05) != SAF_HEADER_SIZE:
        raise ValueError("SAF Header size 不是 0x74")
    width = u16(data, 0x07)
    height = u16(data, 0x09)
    sections = parse_sections(data, SAF_SECTION_TABLE)
    tiles = [decode_rgb16_rle(data[start:end], width, height) for start, end in sections[2]]
    return width, height, tiles


def load_standard_sf2_tiles(path: Path) -> Tuple[int, int, List[List[int]]]:
    """读取一份标准 SF2 的全部 Section2 tile。"""
    raw = path.read_bytes()
    data = rebuild_legacy_tail(raw, b"SF2", SF2_HEADER_SIZE)
    if u16(data, 0x05) != SF2_HEADER_SIZE:
        raise ValueError("标准 SF2 Header size 不是 0x42BC")
    width = u16(data, 0x07)
    height = u16(data, 0x09)
    sections = parse_sections(data, 0x14)
    tiles = [decode_rgb16_rle(data[start:end], width, height) for start, end in sections[2]]
    return width, height, tiles


# ------------------------------
# 25×20 子矩形全量精确匹配
# ------------------------------

def hash_sequence(values: Sequence[int], base: int) -> int:
    """计算一个整数序列的 64 位多项式哈希。"""
    value = 0
    for item in values:
        # +1 让数值0与“没加入任何东西”更不容易形成简单前导碰撞；64位自然溢出保持速度。
        value = ((value * base) + item + 1) & MASK64
    return value


def rolling_row_hashes(row: Sequence[int], window_width: int) -> List[int]:
    """一行内用 O(width) 得到所有固定宽度窗口哈希。"""
    if window_width > len(row):
        return []
    # 第一窗口直接计算；后续窗口用“减旧首项、乘基数、加新尾项”滚动更新。
    current = hash_sequence(row[:window_width], HASH_BASE_X)
    hashes = [current]
    # old_factor = base^(window_width-1)，因为首元素位于最高次幂位置。
    old_factor = pow(HASH_BASE_X, window_width - 1, 1 << 64)
    for start in range(1, len(row) - window_width + 1):
        old_value = row[start - 1] + 1
        new_value = row[start + window_width - 1] + 1
        current = (current - (old_value * old_factor)) & MASK64
        current = ((current * HASH_BASE_X) + new_value) & MASK64
        hashes.append(current)
    return hashes


def window_hashes_2d(tile: Sequence[int], tile_width: int, tile_height: int, window_width: int, window_height: int) -> Iterable[Tuple[int, int, int]]:
    """枚举 tile 内每个 window_width×window_height 子矩形的 (x,y,hash)。"""
    if window_width > tile_width or window_height > tile_height:
        return
    # 先把扁平像素拆成每行，再对每行做横向滚动哈希。
    rows = [tile[y * tile_width:(y + 1) * tile_width] for y in range(tile_height)]
    row_hashes = [rolling_row_hashes(row, window_width) for row in rows]
    x_count = tile_width - window_width + 1
    y_count = tile_height - window_height + 1
    # 对每个 x 独立把 window_height 个“行窗口哈希”再做纵向多项式哈希。
    vertical_factor = pow(HASH_BASE_Y, window_height - 1, 1 << 64)
    for x in range(x_count):
        column_values = [row_hashes[y][x] for y in range(tile_height)]
        current = hash_sequence(column_values[:window_height], HASH_BASE_Y)
        yield x, 0, current
        for y in range(1, y_count):
            old_value = column_values[y - 1] + 1
            new_value = column_values[y + window_height - 1] + 1
            current = (current - (old_value * vertical_factor)) & MASK64
            current = ((current * HASH_BASE_Y) + new_value) & MASK64
            yield x, y, current


def exact_window_equal(big_tile: Sequence[int], big_width: int, x: int, y: int, small_tile: Sequence[int], small_width: int, small_height: int) -> bool:
    """哈希相同后再逐行逐像素比较，保证最终“0匹配”不是概率哈希结论。"""
    for row in range(small_height):
        big_start = (y + row) * big_width + x
        small_start = row * small_width
        if big_tile[big_start:big_start + small_width] != small_tile[small_start:small_start + small_width]:
            return False
    return True


def compare_saf_against_standard_itf(itf_dir: Path) -> Dict[str, Any]:
    """全量解析 101 份 ITF，并把 SAF tile 与所有标准 tile 子矩形做精确匹配。"""
    files = sorted([p for p in itf_dir.rglob("*.SF2") if p.is_file()], key=lambda p: p.name.lower())
    if not files:
        raise ValueError(f"ITF目录里没有 .SF2：{itf_dir}")

    # 按魔数找唯一 SAF；不能把文件名本身当格式身份的唯一依据。
    saf_files = [p for p in files if p.read_bytes()[:4] == b"SAF\x05"]
    if len(saf_files) != 1:
        raise ValueError(f"期望恰好1份 SAF，实际 {len(saf_files)}")
    saf_path = saf_files[0]
    saf_width, saf_height, saf_tiles = load_saf_tiles(saf_path)

    # 每个 SAF tile 的完整 2D 哈希作为第一层快速筛选；同哈希可能对应多个tile，所以值是列表。
    saf_hash_to_indices: Dict[int, List[int]] = defaultdict(list)
    for saf_index, saf_tile in enumerate(saf_tiles):
        # SAF tile 自身恰好就是窗口大小，所以先对每行哈希，再把所有行哈希合成一个2D哈希。
        row_hashes = [hash_sequence(saf_tile[y * saf_width:(y + 1) * saf_width], HASH_BASE_X) for y in range(saf_height)]
        tile_hash = hash_sequence(row_hashes, HASH_BASE_Y)
        saf_hash_to_indices[tile_hash].append(saf_index)

    standard_file_count = 0
    standard_tile_count = 0
    geometry_histogram: Dict[str, int] = {}
    candidate_hash_hits = 0
    exact_matches: List[Dict[str, Any]] = []
    possible_windows = 0

    # 一份一份处理标准 SF2，不把 4978×3072 个 Python int 同时留在内存里。
    for path in files:
        if path == saf_path:
            continue
        raw = path.read_bytes()
        if raw[:3] != b"SF2":
            raise ValueError(f"除唯一SAF外出现非标准SF2：{path}")
        width, height, tiles = load_standard_sf2_tiles(path)
        standard_file_count += 1
        standard_tile_count += len(tiles)
        key = f"{width}x{height}"
        geometry_histogram[key] = geometry_histogram.get(key, 0) + len(tiles)

        # 如果标准 tile 比 SAF 小，当然不存在子矩形；当前语料实际全部 64×48。
        windows_per_tile = max(0, width - saf_width + 1) * max(0, height - saf_height + 1)
        possible_windows += windows_per_tile * len(tiles)

        for tile_index, tile in enumerate(tiles):
            for x, y, window_hash in window_hashes_2d(tile, width, height, saf_width, saf_height):
                candidate_indices = saf_hash_to_indices.get(window_hash)
                if not candidate_indices:
                    continue
                # 哈希命中只代表“值得做精确比较”；最终结论必须来自逐像素比较。
                candidate_hash_hits += len(candidate_indices)
                for saf_index in candidate_indices:
                    if exact_window_equal(tile, width, x, y, saf_tiles[saf_index], saf_width, saf_height):
                        exact_matches.append({
                            "SAF_Tile": saf_index,
                            "标准文件": str(path),
                            "标准Tile": tile_index,
                            "子矩形X": x,
                            "子矩形Y": y,
                        })

    # 固化36正语料预期：100份标准ITF全部64×48，共4978 tile；唯一SAF 25×20，共125 tile。
    return {
        "工具版本": TOOL_VERSION,
        "ITF目录": str(itf_dir),
        "总SF2扩展名文件数": len(files),
        "唯一SAF文件": str(saf_path),
        "SAF几何": f"{saf_width}x{saf_height}",
        "SAF_Section2_Tile数": len(saf_tiles),
        "标准SF2文件数": standard_file_count,
        "标准SF2_Section2_Tile总数": standard_tile_count,
        "标准Tile几何分布_按Tile计": geometry_histogram,
        "每个64x48内25x20候选位置": (64 - saf_width + 1) * (48 - saf_height + 1) if saf_width <= 64 and saf_height <= 48 else None,
        "全量候选子矩形数": possible_windows,
        "滚动哈希候选命中次数": candidate_hash_hits,
        "逐像素精确匹配数": len(exact_matches),
        "逐像素精确匹配": exact_matches,
        "结论边界": "若精确匹配为0，只能排除“某个SAF tile原样等于某个标准64x48 tile内部25x20子矩形”；不排除缩放、调色、跨tile拼接、变换或运行时生成。",
    }


# ------------------------------
# 已提取资源正文文本引用穷举
# ------------------------------

def search_resource_bodies(root: Path) -> Dict[str, Any]:
    """扫描已提取资源文件正文中是否直接出现 ITF000 名称字节。"""
    patterns = [
        b"ITF000",
        b"ITF000.SF2",
        b"itf000",
        b"itf000.sf2",
        b"Fight\\ITF\\ITF000",
        b"FIGHT\\ITF\\ITF000",
    ]
    files = sorted([p for p in root.rglob("*") if p.is_file()])
    hits: List[Dict[str, Any]] = []
    # 对每个文件只读一次，再测试所有短字节串；646份资源规模很小，不需要建立复杂索引。
    for path in files:
        data = path.read_bytes()
        for pattern in patterns:
            offset = data.find(pattern)
            if offset >= 0:
                hits.append({
                    "文件": str(path),
                    "模式": pattern.decode("ascii"),
                    "首次偏移": offset,
                })
    return {
        "工具版本": TOOL_VERSION,
        "扫描根目录": str(root),
        "扫描文件数": len(files),
        "搜索模式": [p.decode("ascii") for p in patterns],
        "命中数": len(hits),
        "命中": hits,
        "结论边界": "0命中只表示可直接搜索的ASCII名称不存在；不排除数字ID、压缩/加密正文、二进制脚本字段或运行时拼接。",
    }


# ------------------------------
# 命令行入口
# ------------------------------

def build_parser() -> argparse.ArgumentParser:
    """建立命令行参数。"""
    parser = argparse.ArgumentParser(description="幽城幻剑录 SAF 运行时请求面与资源语料静态调查器")
    # EXE 是固化36机器码回归的核心输入，因此设为必填。
    parser.add_argument("--exe", type=Path, required=True, help="RPG.exe / RPG.exe.org 路径")
    # ITF目录可选；提供后才做101份ITF与577万窗口的像素关系回归。
    parser.add_argument("--itf-dir", type=Path, help="ItfDir.DAT解出的ITF目录")
    # 资源提取目录可选；提供后扫描所有已提取正文中的ITF000 ASCII引用。
    parser.add_argument("--resource-root", type=Path, help="九个资源容器已提取正文的根目录")
    # JSON 输出必填，避免长结果只停留在终端而无法进入固化包。
    parser.add_argument("--json", type=Path, required=True, help="综合JSON报告输出路径")
    return parser


def main(argv: Optional[Sequence[str]] = None) -> int:
    """执行所选检查，并把所有结果合并成一份综合报告。"""
    args = build_parser().parse_args(argv)
    # 先做EXE检查；它不依赖资源目录，所以即使后面的语料检查失败也能明确失败位置。
    report: Dict[str, Any] = {
        "工具版本": TOOL_VERSION,
        "固化节点": 36,
        "研究主题": "SAF运行时请求面与资源语料负证据边界",
        "EXE直接调用面": verify_exe_runtime_surface(args.exe),
    }
    # 用户提供ITF目录时追加完整几何/像素关系检查。
    if args.itf_dir is not None:
        report["ITF几何与像素关系"] = compare_saf_against_standard_itf(args.itf_dir)
    # 用户提供资源正文根目录时追加ASCII requester穷举。
    if args.resource_root is not None:
        report["资源正文ITF000引用"] = search_resource_bodies(args.resource_root)

    # 总PASS只约束“工具明确设定的硬断言”：EXE机器断言必须全通过；语料结果本身是统计，不强行把0命中写死为程序成功条件。
    report["硬断言全部通过"] = bool(report["EXE直接调用面"]["全部机器断言通过"])
    # 把结果先落盘，再打印最简摘要；这样终端输出不会因为完整JSON过长而难以阅读。
    write_json(args.json, report)
    print(f"[PASS] EXE机器断言={report['硬断言全部通过']}")
    if "ITF几何与像素关系" in report:
        pixel = report["ITF几何与像素关系"]
        print(
            "[ITF] 标准文件=%d 标准tile=%d SAFtile=%d 候选窗口=%d 精确匹配=%d"
            % (
                pixel["标准SF2文件数"], pixel["标准SF2_Section2_Tile总数"],
                pixel["SAF_Section2_Tile数"], pixel["全量候选子矩形数"], pixel["逐像素精确匹配数"],
            )
        )
    if "资源正文ITF000引用" in report:
        resource = report["资源正文ITF000引用"]
        print(f"[RESOURCE] 文件={resource['扫描文件数']} ITF000文本命中={resource['命中数']}")
    print(f"[JSON] {args.json}")
    return 0 if report["硬断言全部通过"] else 2


# 只有直接执行本文件时才进入main；被其它回归脚本import时不会自动扫描大批资源。
if __name__ == "__main__":
    raise SystemExit(main())
