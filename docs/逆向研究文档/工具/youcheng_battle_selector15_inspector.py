#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 状态资源 selector 15 静态生产面调查器（固化50）。

这个工具只读两个已经放在固化包里的证据源：

1. 原始 RPG.exe；
2. 固化46从原始 ITF0052.SF2 独立解析得到的 JSON 结果。

它要回答的不是“selector 15 看起来像什么状态”，而是更基础、也更适合兼容引擎实现的问题：

- ITF0052 的 selector 15 是不是真实存在的资源入口？
- 原版 Battle 状态容器在构造时会不会主动选到 15？
- 正常 14 路状态刷新会不会主动选到 15？
- 状态容器自己的专用虚方法里，有没有另一条 selector setter 调用把某个 child 切到 15？
- 析构/释放阶段有没有隐藏的 selector 写入？

【证据边界】
工具只能证明“当前同版 RPG.exe 的 canonical Battle 状态容器静态直接生产面”。
即使结果为 PASS，也绝对不能把 selector 15 命名成 unused/dead/reserved，
因为 ITF0052 资源中 selector 15 本身是真实存在的合法条目；其他版本、间接别名写入、
外部修改或当前静态扫描没有覆盖到的非 canonical 路径，都必须继续保留可能性。
"""

# argparse 用来读取命令行参数，例如 --rpg 和 --json-out。
import argparse
# hashlib 用来计算 SHA-256，确保接档者以后重跑时能确认自己拿的是哪一份输入。
import hashlib
# json 用来读取固化46证据，也用来输出固化50的结构化调查结果。
import json
# struct 用来按 x86/PE 使用的小端字节序读取 16/32 位整数，以及解析 E8 rel32 调用。
import struct
# pathlib.Path 让 Windows/Linux 路径处理更清楚，也避免手工拼接斜杠。
from pathlib import Path
# 类型标注不是运行所必需，但能让初学者一眼知道函数“输入什么、返回什么”。
from typing import Dict, List, Optional, Tuple


# 工具版本只代表本调查器自身，不代表游戏版本。
TOOL_VERSION = "0.7D-solid50.0"

# 下面这些地址全部来自当前同版 RPG.exe 的静态反汇编，并在本工具里再次做机器码断言。
# VA 的意思是 Virtual Address（虚拟地址）：游戏运行时看到的是这种地址，不是磁盘文件偏移。
VA_ITF0052_PATH = 0x004699C4
VA_SELECTOR_SETTER = 0x0043E6F0
VA_STATUS_CONTAINER_CTOR = 0x0041D160
VA_STATUS_CONTAINER_DTOR = 0x0041D720
VA_STATUS_CUSTOM_VMETHOD = 0x0041D920
VA_STATUS_VTABLE = 0x00460AD0

# 外层 Battle UI 构造器会分配 0x608 字节，再调用状态容器构造器，最后把指针存到 +0x5C0。
VA_PARENT_ALLOC = 0x0041C57A
VA_CTOR_DIRECT_CALL = 0x0041C5A4
VA_PARENT_STORE = 0x0041C5B6

# 构造器中两排 ITF0052 child 的关键机器码锚点。
VA_FIRST_ROW_SETUP = 0x0041D2F3
VA_FIRST_ROW_CHILD_BASE = 0x0041D2FD
VA_FIRST_ROW_PATH_PUSH = 0x0041D32F
VA_FIRST_ROW_SELECTOR_STEP = 0x0041D34B
VA_FIRST_ROW_SETTER_CALL = 0x0041D350
VA_FIRST_ROW_LOOP_UPDATE = 0x0041D355
VA_SECOND_ROW_SELECTOR_SEED = 0x0041D36A
VA_SECOND_ROW_CHILD_BASE = 0x0041D376
VA_SECOND_ROW_PATH_PUSH = 0x0041D3A5
VA_SECOND_ROW_SELECTOR_PUSH = 0x0041D3BD
VA_SECOND_ROW_SETTER_CALL = 0x0041D3C7
VA_SECOND_ROW_LOOP_UPDATE = 0x0041D3CC

# 析构阶段从 self+0x598 开始，固定循环 0x0E=14 个 child。
VA_DTOR_VTABLE_RESET = 0x0041D742
VA_DTOR_CHILD_BASE = 0x0041D88E
VA_DTOR_CHILD_COUNT = 0x0041D894
VA_DTOR_LOOP_BODY = 0x0041D89C
VA_DTOR_LOOP_TAIL = 0x0041D8B8

# 固化46已经闭合了 14 路正常状态 update。这里把范围与预期 call 地址继续作为固化50前置证据重放。
VA_UPDATE_BEGIN = 0x0041CD79
VA_UPDATE_END = 0x0041D119
EXPECTED_UPDATE_SETTER_CALLS = [
    0x0041CDC4, 0x0041CE14, 0x0041CE64, 0x0041CEB4, 0x0041CF04,
    0x0041CF54, 0x0041CF87, 0x0041CFBA, 0x0041D00A, 0x0041D03D,
    0x0041D070, 0x0041D0A3, 0x0041D0D6, 0x0041D0F6, 0x0041D114,
]

# 14 路 update 能主动产生的非零 selector 集合。
# selector 0 是“字段都不满足时的空/回退”，因此另行记录。
EXPECTED_UPDATE_NONZERO_SELECTORS = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    16, 17, 18, 19, 20, 21, 22,
}

# 状态容器 vtable 的前 8 项。只有 0x41D920 是这个容器特有、并且本轮需要继续检查的虚方法。
EXPECTED_VTABLE_FIRST8 = [
    0x00431140,
    0x00431170,
    0x00413000,
    0x00413000,
    0x00413000,
    0x0040FBD0,
    0x0041D920,
    0x00413000,
]

# ITF0052 的路径字符串必须包含末尾 NUL；这样能防止只匹配到一个短前缀。
EXPECTED_ITF0052_PATH = b"Fight\\ITF\\ITF0052.SF2\x00"


# 下面四个小函数只是“按小端读取整数”。
# 单独写出来后，后面 PE 解析代码会比反复写 struct.unpack_from 更容易读。
def u16(data: bytes, offset: int) -> int:
    """从 data[offset] 开始读取一个无符号 16 位小端整数。"""
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    """从 data[offset] 开始读取一个无符号 32 位小端整数。"""
    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    """从 data[offset] 开始读取一个有符号 32 位小端整数。"""
    return struct.unpack_from("<i", data, offset)[0]


def sha256_bytes(data: bytes) -> str:
    """计算一段字节的 SHA-256 十六进制字符串。"""
    return hashlib.sha256(data).hexdigest()


class PeImage:
    """最小只读 PE32 映射器：只实现本调查需要的地址换算和 section 读取。"""

    def __init__(self, data: bytes):
        # 保存原始 EXE 字节。整个类从不修改这个 bytes，所以调查是只读的。
        self.data = data

        # Windows PE 文件最前面应该是 DOS MZ 签名。
        if data[:2] != b"MZ":
            raise ValueError("输入文件不是 MZ/PE 可执行文件")

        # DOS 头 +0x3C 保存真正 PE 头在磁盘文件里的偏移。
        pe_offset = u32(data, 0x3C)
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("找不到 PE\\0\\0 签名")

        # PE 签名后面紧跟 COFF Header。
        coff = pe_offset + 4
        section_count = u16(data, coff + 2)
        optional_size = u16(data, coff + 16)

        # COFF Header 固定 20 字节，所以 Optional Header 从 coff+20 开始。
        optional = coff + 20
        # 0x10B 表示 PE32；当前 RPG.exe 是 32 位程序。
        if u16(data, optional) != 0x10B:
            raise ValueError("当前调查器只支持 PE32（OptionalHeader.Magic=0x10B）")

        # PE32 Optional Header +0x1C 是 ImageBase。当前游戏通常是 0x00400000。
        self.image_base = u32(data, optional + 0x1C)

        # Section Table 紧跟 Optional Header；每个 section header 固定 40 字节。
        section_table = optional + optional_size
        self.sections: List[Dict[str, int]] = []
        for index in range(section_count):
            base = section_table + index * 40
            name_bytes = data[base:base + 8].split(b"\0", 1)[0]
            name = name_bytes.decode("ascii", "replace")
            virtual_size = u32(data, base + 8)
            virtual_address = u32(data, base + 12)
            raw_size = u32(data, base + 16)
            raw_offset = u32(data, base + 20)
            self.sections.append({
                "name": name,
                "virtual_size": virtual_size,
                "virtual_address": virtual_address,
                "raw_size": raw_size,
                "raw_offset": raw_offset,
            })

    def va_to_offset(self, va: int) -> int:
        """把运行时 VA 换算成 EXE 文件偏移。"""
        # VA - ImageBase = RVA。
        rva = va - self.image_base
        for section in self.sections:
            start = section["virtual_address"]
            # 内存虚拟大小与磁盘 raw 大小可能不同；判断“属于哪个 section”时取较大值。
            span = max(section["virtual_size"], section["raw_size"])
            if start <= rva < start + span:
                delta = rva - start
                # 如果落在只在内存补零、磁盘并不存在的尾部，就不能继续读原文件字节。
                if delta >= section["raw_size"]:
                    raise ValueError(f"VA 0x{va:08X} 落在 section 的磁盘零填充区")
                return section["raw_offset"] + delta
        raise ValueError(f"VA 0x{va:08X} 不属于任何 PE section")

    def offset_to_va(self, offset: int) -> Optional[int]:
        """把磁盘文件偏移反算成 VA；不属于任何 section 时返回 None。"""
        for section in self.sections:
            start = section["raw_offset"]
            end = start + section["raw_size"]
            if start <= offset < end:
                return self.image_base + section["virtual_address"] + (offset - start)
        return None

    def read(self, va: int, size: int) -> bytes:
        """从指定 VA 读取固定长度字节；不足长度就报错，避免静默截断。"""
        offset = self.va_to_offset(va)
        chunk = self.data[offset:offset + size]
        if len(chunk) != size:
            raise ValueError(f"读取 VA 0x{va:08X} 时超出文件")
        return chunk

    def text_range(self) -> Tuple[int, int]:
        """返回 .text 的 [起始VA, 结束VA)。"""
        for section in self.sections:
            if section["name"] == ".text":
                start = self.image_base + section["virtual_address"]
                end = start + section["raw_size"]
                return start, end
        raise ValueError("PE 中找不到 .text section")


# 这个函数把“某个地址应该是什么机器码”做成统一的结构化断言。
def assert_bytes(pe: PeImage, va: int, expected: bytes, label: str) -> Dict[str, object]:
    actual = pe.read(va, len(expected))
    return {
        "名称": label,
        "VA": f"0x{va:08X}",
        "期望": expected.hex(" "),
        "实际": actual.hex(" "),
        "PASS": actual == expected,
    }


# x86 的 E8 rel32 是最常见的“直接 call”。
# 这里逐字节扫描限定区间，并计算每个 E8 的实际目标地址。
def scan_direct_e8_calls(pe: PeImage, start_va: int, end_va: int, target_va: int) -> List[int]:
    if end_va <= start_va:
        return []
    blob = pe.read(start_va, end_va - start_va)
    found: List[int] = []
    for index in range(0, len(blob) - 4):
        if blob[index] != 0xE8:
            continue
        rel = struct.unpack_from("<i", blob, index + 1)[0]
        call_va = start_va + index
        decoded_target = call_va + 5 + rel
        if decoded_target == target_va:
            found.append(call_va)
    return found


# 在整个文件里找一个32位小端常量，例如 0x004699C4。
# 返回的是“这个四字节常量起始位置对应的 VA”，用于确认资源路径地址到底有几处静态引用。
def scan_dword_literal_vas(pe: PeImage, value: int) -> List[int]:
    needle = struct.pack("<I", value)
    result: List[int] = []
    start = 0
    while True:
        offset = pe.data.find(needle, start)
        if offset < 0:
            break
        va = pe.offset_to_va(offset)
        if va is not None:
            result.append(va)
        start = offset + 1
    return result


# 构造器的两排 child 使用同样的横坐标序列：16,46,76,...,196。
# 原版每次 +30，当下一次达到226(0xE2)时退出，因此每排恰好7个。
def derive_row_positions() -> List[int]:
    positions: List[int] = []
    x = 0x10
    while x < 0xE2:
        positions.append(x)
        x += 0x1E
    return positions


# 第一排 selector 从 edi=0 开始，每次创建后 inc edi 再 push，所以得到 1..7。
# 第二排读取 edi+1 作为起点，第一排结束 edi=7，因此第二排得到8..14。
def derive_constructor_selectors() -> Dict[str, List[int]]:
    positions = derive_row_positions()
    first_row = list(range(1, 1 + len(positions)))
    second_row_start = first_row[-1] + 1
    second_row = list(range(second_row_start, second_row_start + len(positions)))
    return {
        "横坐标": positions,
        "第一排": first_row,
        "第二排": second_row,
        "合计": first_row + second_row,
    }


# 固化46 JSON 是由原始 ITF0052.SF2 解析得到的前代证据。
# 本函数只抽取固化50所需的几个事实，并检查 selector15 确实存在于 Section0/1/2。
def inspect_solid46_asset_json(path: Path) -> Dict[str, object]:
    data = json.loads(path.read_text(encoding="utf-8"))
    itf = data.get("itf0052", {})
    counts = itf.get("section_counts")

    section0_selectors = [int(item["selector"]) for item in itf.get("section0", [])]
    section1_selectors = [int(item["selector"]) for item in itf.get("section1", [])]

    # 固化46 JSON 没有逐项展开 Section2，但已经记录 Section0/1/2 一一对应和23个Section2。
    # 所以 selector15 对 Section2 的存在由“23/23/23 + 一一对应 + Section0/1含15”共同证明。
    selector15_section0 = 15 in section0_selectors
    selector15_section1 = 15 in section1_selectors
    one_to_one = bool(itf.get("section0_section1_section2_one_to_one"))
    selector15_section2 = counts == [23, 23, 23, 0] and one_to_one and selector15_section0

    passed = (
        data.get("result") == "PASS"
        and counts == [23, 23, 23, 0]
        and itf.get("selector_valid_range_from_asset") == [0, 22]
        and selector15_section0
        and selector15_section1
        and selector15_section2
    )
    return {
        "来源": str(path),
        "固化46结果": data.get("result"),
        "ITF0052原始SHA256": itf.get("raw_sha256"),
        "Section计数": counts,
        "合法selector范围": itf.get("selector_valid_range_from_asset"),
        "Section0_1_2一一对应": one_to_one,
        "selector15存在于Section0": selector15_section0,
        "selector15存在于Section1": selector15_section1,
        "selector15存在于Section2": selector15_section2,
        "PASS": passed,
    }


# 把一组断言的 PASS 汇总起来。只要有一条失败，整个组就失败。
def all_pass(checks: List[Dict[str, object]]) -> bool:
    return all(bool(item.get("PASS")) for item in checks)


def main() -> int:
    # 建立命令行参数说明。
    parser = argparse.ArgumentParser(
        description="只读调查 RPG.exe 中 ITF0052 selector15 的 canonical Battle 状态容器静态直接生产面。"
    )
    parser.add_argument("--rpg", required=True, type=Path, help="原始 RPG.exe 路径")
    parser.add_argument(
        "--solid46-json",
        required=True,
        type=Path,
        help="固化46的 Battle十四路状态表现与ITF0052路由调查.json，用于复核 selector15 资源条目真实存在",
    )
    parser.add_argument("--json-out", required=True, type=Path, help="固化50结构化JSON输出路径")
    args = parser.parse_args()

    # 一次性把 EXE 读进内存。之后的所有调查都对这个 bytes 做只读访问。
    exe_data = args.rpg.read_bytes()
    pe = PeImage(exe_data)

    # 先检查路径字符串本身。这个断言能防止“地址没变但内容已经不是ITF0052”的情况。
    path_check = assert_bytes(
        pe,
        VA_ITF0052_PATH,
        EXPECTED_ITF0052_PATH,
        "ITF0052路径字符串",
    )

    # 下面这些机器码断言覆盖父构造器、两排 child 初始化和析构循环。
    # 每条 expected 都尽量短到只包含与结论直接相关的指令，避免无关编译差异造成假失败。
    machine_checks: List[Dict[str, object]] = [
        path_check,
        assert_bytes(pe, VA_PARENT_ALLOC, b"\x68\x08\x06\x00\x00", "父Battle UI分配0x608字节状态容器"),
        assert_bytes(pe, VA_CTOR_DIRECT_CALL, b"\xE8\xB7\x0B\x00\x00", "父Battle UI直接调用0x41D160"),
        assert_bytes(pe, VA_PARENT_STORE, b"\x89\x86\xC0\x05\x00\x00", "父Battle UI把状态容器保存到+0x5C0"),
        assert_bytes(pe, VA_FIRST_ROW_SETUP, b"\x33\xFF\xC7\x44\x24\x10\x10\x00\x00\x00", "第一排selector计数从EDI=0且X从16开始"),
        assert_bytes(pe, VA_FIRST_ROW_CHILD_BASE, b"\x8D\xAE\x98\x05\x00\x00", "第一排child指针从self+0x598开始"),
        assert_bytes(pe, VA_FIRST_ROW_PATH_PUSH, b"\x68\xC4\x99\x46\x00", "第一排加载ITF0052路径"),
        assert_bytes(pe, VA_FIRST_ROW_SELECTOR_STEP, b"\x47\x57\x8B\x4A\x1C", "第一排每次先INC EDI再PUSH selector"),
        assert_bytes(pe, VA_FIRST_ROW_SETTER_CALL, b"\xE8\x9B\x13\x02\x00", "第一排调用selector setter"),
        assert_bytes(pe, VA_FIRST_ROW_LOOP_UPDATE, b"\x8B\x44\x24\x10\x83\xC5\x04\x83\xC0\x1E\x3D\xE2\x00\x00\x00", "第一排child+4且X+30直到226"),
        assert_bytes(pe, VA_SECOND_ROW_SELECTOR_SEED, b"\x8D\x47\x01\xBD\x10\x00\x00\x00\x89\x44\x24\x10", "第二排selector从第一排末值+1且X重置为16"),
        assert_bytes(pe, VA_SECOND_ROW_CHILD_BASE, b"\x8D\xBC\xBE\x98\x05\x00\x00", "第二排child紧接第一排后的self+0x598数组"),
        assert_bytes(pe, VA_SECOND_ROW_PATH_PUSH, b"\x68\xC4\x99\x46\x00", "第二排加载同一ITF0052路径"),
        assert_bytes(pe, VA_SECOND_ROW_SELECTOR_PUSH, b"\x8B\x07\x8B\x54\x24\x10\x52\x8B\x48\x1C", "第二排读取运行selector并PUSH"),
        assert_bytes(pe, VA_SECOND_ROW_SETTER_CALL, b"\xE8\x24\x13\x02\x00", "第二排调用selector setter"),
        assert_bytes(pe, VA_SECOND_ROW_LOOP_UPDATE, b"\x8B\x54\x24\x10\x83\xC7\x04\x83\xC5\x1E\x42\x81\xFD\xE2\x00\x00\x00", "第二排child+4、X+30、selector+1"),
        assert_bytes(pe, VA_DTOR_VTABLE_RESET, b"\xC7\x06\xD0\x0A\x46\x00", "析构阶段恢复状态容器vtable"),
        assert_bytes(pe, VA_DTOR_CHILD_BASE, b"\x8D\x9E\x98\x05\x00\x00", "析构从14个状态child数组起点开始"),
        assert_bytes(pe, VA_DTOR_CHILD_COUNT, b"\xC7\x44\x24\x10\x0E\x00\x00\x00", "析构child计数固定为14"),
        assert_bytes(pe, VA_DTOR_LOOP_BODY, b"\x8B\x3B\x3B\xFD", "析构循环读取当前child并判空"),
        assert_bytes(pe, VA_DTOR_LOOP_TAIL, b"\x83\xC3\x04\x48\x89\x44\x24\x10\x75\xDA", "析构每次child指针+4并循环14次"),
    ]

    # 在整个 .text 中找状态容器构造器的 direct E8 caller。
    text_start, text_end = pe.text_range()
    ctor_callers = scan_direct_e8_calls(pe, text_start, text_end, VA_STATUS_CONTAINER_CTOR)
    ctor_callers_pass = ctor_callers == [VA_CTOR_DIRECT_CALL]

    # 资源路径的32位地址在当前EXE中只作为两条 push imm32 的立即数出现。
    # 注意：scan_dword_literal_vas 返回的是“四字节立即数开始位置”，所以比 push 指令地址多1。
    path_literal_vas = scan_dword_literal_vas(pe, VA_ITF0052_PATH)
    expected_path_literal_vas = [VA_FIRST_ROW_PATH_PUSH + 1, VA_SECOND_ROW_PATH_PUSH + 1]
    path_literal_pass = path_literal_vas == expected_path_literal_vas

    # 根据构造器机器协议数学重建两排selector，而不是凭手工抄写1..14。
    constructor_derived = derive_constructor_selectors()
    constructor_selectors = constructor_derived["合计"]
    constructor_pass = (
        constructor_derived["横坐标"] == [0x10, 0x2E, 0x4C, 0x6A, 0x88, 0xA6, 0xC4]
        and constructor_selectors == list(range(1, 15))
        and 15 not in constructor_selectors
    )

    # 重放正常14路状态update里的直接setter call集合。
    update_calls = scan_direct_e8_calls(pe, VA_UPDATE_BEGIN, VA_UPDATE_END, VA_SELECTOR_SETTER)
    update_calls_pass = update_calls == EXPECTED_UPDATE_SETTER_CALLS

    # selector集合直接读取固化46已经逐条机器断言通过的 route_machine_checks。
    # 这样固化50不会再造一份“可能抄错的新路由表”，而是利用前代可重复证据做交叉复核。
    solid46_data = json.loads(args.solid46_json.read_text(encoding="utf-8"))
    route_checks = solid46_data.get("exe", {}).get("route_machine_checks", [])
    update_nonzero_selectors = sorted({int(item["selector"]) for item in route_checks if item.get("PASS")})
    update_selector_pass = (
        len(route_checks) == 21
        and all(bool(item.get("PASS")) for item in route_checks)
        and set(update_nonzero_selectors) == EXPECTED_UPDATE_NONZERO_SELECTORS
        and 15 not in update_nonzero_selectors
    )

    # 构造+update附近的setter调用总共应有18处：
    # 15个正常update call + 2个ITF0052循环call + 1个构造器里“其他资源child”的selector0 call。
    canonical_region_calls = scan_direct_e8_calls(pe, 0x0041CD00, 0x0041D920, VA_SELECTOR_SETTER)
    expected_region_calls = sorted(EXPECTED_UPDATE_SETTER_CALLS + [0x0041D1E0, VA_FIRST_ROW_SETTER_CALL, VA_SECOND_ROW_SETTER_CALL])
    canonical_region_pass = canonical_region_calls == expected_region_calls

    # vtable本身是数据，不是代码。直接读取前8个DWORD并与预期比较。
    vtable_actual = list(struct.unpack("<8I", pe.read(VA_STATUS_VTABLE, 8 * 4)))
    vtable_pass = vtable_actual == EXPECTED_VTABLE_FIRST8

    # 状态容器唯一专用虚方法 0x41D920..0x41DB80 内不应出现selector setter的direct E8调用。
    custom_vmethod_setter_calls = scan_direct_e8_calls(pe, VA_STATUS_CUSTOM_VMETHOD, 0x0041DB80, VA_SELECTOR_SETTER)
    custom_vmethod_pass = custom_vmethod_setter_calls == []

    # 析构函数本身也不应调用selector setter；它只释放14个child。
    destructor_setter_calls = scan_direct_e8_calls(pe, VA_STATUS_CONTAINER_DTOR, VA_STATUS_CUSTOM_VMETHOD, VA_SELECTOR_SETTER)
    destructor_setter_pass = destructor_setter_calls == []

    # 前代资源证据：selector15是合法资源入口，而不是“越界不存在”。
    asset_check = inspect_solid46_asset_json(args.solid46_json)

    # 汇总所有必须同时成立的条件。
    pass_items = [
        all_pass(machine_checks),
        ctor_callers_pass,
        path_literal_pass,
        constructor_pass,
        update_calls_pass,
        update_selector_pass,
        canonical_region_pass,
        vtable_pass,
        custom_vmethod_pass,
        destructor_setter_pass,
        bool(asset_check["PASS"]),
    ]
    overall_pass = all(pass_items)

    # 输出报告尽量同时给“原始事实”和“实现边界”，方便未来兼容引擎直接消费。
    report: Dict[str, object] = {
        "tool_version": TOOL_VERSION,
        "topic": "Battle状态资源selector15静态直接生产面",
        "result": "PASS" if overall_pass else "FAIL",
        "exe": {
            "path": str(args.rpg),
            "size": len(exe_data),
            "sha256": sha256_bytes(exe_data),
            "image_base": f"0x{pe.image_base:08X}",
        },
        "machine_checks": machine_checks,
        "machine_check_count": len(machine_checks),
        "machine_check_pass_count": sum(1 for item in machine_checks if item["PASS"]),
        "itf0052_path_static_references": {
            "dword_literal_vas": [f"0x{x:08X}" for x in path_literal_vas],
            "expected": [f"0x{x:08X}" for x in expected_path_literal_vas],
            "meaning": "两处都位于0x41D160状态容器构造器的push imm32；未发现第三处同地址静态字面引用",
            "PASS": path_literal_pass,
        },
        "status_container_constructor": {
            "va": f"0x{VA_STATUS_CONTAINER_CTOR:08X}",
            "direct_e8_callers_in_text": [f"0x{x:08X}" for x in ctor_callers],
            "expected_unique_caller": f"0x{VA_CTOR_DIRECT_CALL:08X}",
            "parent_alloc_size": "0x608",
            "parent_store_offset": "+0x5C0",
            "derived_x_positions_each_row": constructor_derived["横坐标"],
            "derived_first_row_selectors": constructor_derived["第一排"],
            "derived_second_row_selectors": constructor_derived["第二排"],
            "derived_all_itf0052_initial_selectors": constructor_selectors,
            "selector15_possible": 15 in constructor_selectors,
            "PASS": ctor_callers_pass and constructor_pass,
        },
        "normal_status_update": {
            "range": [f"0x{VA_UPDATE_BEGIN:08X}", f"0x{VA_UPDATE_END:08X}"],
            "direct_setter_calls": [f"0x{x:08X}" for x in update_calls],
            "direct_setter_call_count": len(update_calls),
            "logical_channel_count": 14,
            "nonzero_selectors": update_nonzero_selectors,
            "zero_fallback_selector": 0,
            "selector15_possible": 15 in update_nonzero_selectors,
            "PASS": update_calls_pass and update_selector_pass,
        },
        "canonical_constructor_update_region": {
            "range": ["0x0041CD00", "0x0041D920"],
            "direct_setter_calls": [f"0x{x:08X}" for x in canonical_region_calls],
            "direct_setter_call_count": len(canonical_region_calls),
            "classification": {
                "normal_update_calls": len(EXPECTED_UPDATE_SETTER_CALLS),
                "itf0052_constructor_loop_call_sites": 2,
                "other_resource_child_constructor_call": 1,
            },
            "PASS": canonical_region_pass,
        },
        "status_container_vtable": {
            "va": f"0x{VA_STATUS_VTABLE:08X}",
            "first8": [f"0x{x:08X}" for x in vtable_actual],
            "expected_first8": [f"0x{x:08X}" for x in EXPECTED_VTABLE_FIRST8],
            "custom_method": f"0x{VA_STATUS_CUSTOM_VMETHOD:08X}",
            "custom_method_direct_setter_calls": [f"0x{x:08X}" for x in custom_vmethod_setter_calls],
            "PASS": vtable_pass and custom_vmethod_pass,
        },
        "destructor": {
            "va": f"0x{VA_STATUS_CONTAINER_DTOR:08X}",
            "child_array_offset": "+0x598",
            "child_count": 14,
            "direct_setter_calls": [f"0x{x:08X}" for x in destructor_setter_calls],
            "PASS": destructor_setter_pass,
        },
        "selector15_asset_existence": asset_check,
        "frozen_boundary": {
            "selector15_is_valid_itf0052_asset_entry": True,
            "canonical_battle_status_container_direct_producer_count_for_selector15": 0,
            "constructor_produces": "1..14",
            "normal_update_produces": "0或1..14/16..22",
            "custom_status_vmethod_direct_selector_setter_calls": 0,
            "destructor_direct_selector_setter_calls": 0,
            "implementation_rule": "当前同版canonical Battle状态容器不得主动把14个状态child切到selector15；但资源解析器必须保留selector15这个合法入口。",
        },
        "forbidden_overreach": [
            "不得把selector15命名为unused/dead/reserved/padding。",
            "不得因为canonical状态容器无直接producer就删除ITF0052的selector15资源条目。",
            "不得把本结论扩大成全程序、所有版本、所有间接/别名路径绝对不可达。",
            "不得根据selector15图标外观猜作者业务状态名。",
        ],
    }

    # 输出目录可能还不存在，所以先递归创建。
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    # indent=2 让JSON能直接人工阅读；ensure_ascii=False保留中文，不转成\uXXXX。
    args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    # 命令行也打印最关键摘要，方便运行者不用先打开JSON就知道是否通过。
    print(f"[{TOOL_VERSION}] result={report['result']}")
    print(f"RPG.exe sha256={report['exe']['sha256']}")
    print(f"machine_checks={report['machine_check_pass_count']}/{report['machine_check_count']}")
    print(f"ITF0052 path refs={report['itf0052_path_static_references']['dword_literal_vas']}")
    print(f"constructor selectors={constructor_selectors}")
    print(f"update nonzero selectors={update_nonzero_selectors}")
    print(f"selector15 canonical direct producer count=0")

    # shell里 0 表示成功，1 表示失败；这样批处理/CI可以直接判断。
    return 0 if overall_pass else 1


# 只有直接运行这个文件时才执行main；被别的Python脚本import时不会自动开始调查。
if __name__ == "__main__":
    raise SystemExit(main())
