#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""验证Battle角色锚点+0x10C8与FightRole+0xCE4是否是同一物理DWORD。

本工具只做可重复的静态机器证据核对，不为字段或状态码冒充原作者正式命名。
它同时读取完全未修改原版与历史研究版RPG.exe；所有业务结论以原版为准，
研究版只用于证明本节点依赖代码区间是否仍逐字节一致。
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


TOOL_VERSION = "0.7D-solid65.0"
EXPECTED_ORIGINAL_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
EXPECTED_RESEARCH_SHA256 = "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"

# 同一角色在Battle对象中的索引步长。0x44331B..0x443327的LEA链可按整数式恢复为
# 4 * (i + 8 * (3 * (i + 4 * (i + 8*i)))) = 0xDE4 * i。
ROLE_STRIDE = 0x0DE4

# 角色锚点到FightRole对象首地址的固定偏移。
FIGHT_ROLE_FROM_ANCHOR = 0x03E4

# 统一状态DWORD的两种观察口径：从角色锚点看是+0x10C8，从FightRole看是+0xCE4。
STATE_FROM_ANCHOR = 0x10C8
STATE_FROM_FIGHT_ROLE = 0x0CE4
ROLE_COUNT = 0x10


# 这些VA是“四字节小端常量本身”的起始地址，而不是指令起始地址。
# +0xCE4原始字节搜索有24项；最后一项位于C7 45 E4 0C 00 00 00中，
# E4是disp8、0C 00 00 00是立即数，所以不是+0xCE4 disp32内存访问。
EXPECTED_CE4_RAW_HITS = (
    0x0041F119, 0x0042012D, 0x004203C6, 0x00420EF8, 0x00420FAF,
    0x00421458, 0x004214CB, 0x004214FF, 0x00421B86, 0x004222F9,
    0x0042231E, 0x00422344, 0x00422582, 0x0042267A, 0x00422708,
    0x00422746, 0x004227B0, 0x00422803, 0x00422B46, 0x00423244,
    0x00423743, 0x00444E9C, 0x00445FEC, 0x0045AB8A,
)
EXPECTED_CE4_DISP32_HITS = EXPECTED_CE4_RAW_HITS[:-1]
EXPECTED_CE4_FALSE_RAW_HIT = 0x0045AB8A

EXPECTED_10C8_RAW_HITS = (
    0x00442B78, 0x00442BED, 0x00442EA0, 0x00442EFF, 0x00442F31,
    0x00442F53, 0x00442F96, 0x004430E8, 0x00443396, 0x004433A7,
    0x004433B8, 0x004433C4, 0x00443429, 0x004434A0, 0x0044354E,
)


# 区间哈希既防止地址错位，也要求历史研究版在本节点依赖范围与原版一致。
CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (0x00443310, 0x00443340, "按角色索引计算0xDE4步长并形成FightRole+0x3E4基址", "3f97f0a7d73a9e6e07a5f0d9ad4d8295d5fd3866ea7c617ffe505daf3be26daa"),
    (0x00442BEB, 0x00442C23, "Battle+0x10C8首项指针与16槽0xDE4步进", "ea85a50127cb19ecd5cb7d20168965a91e4ef8a22b00dc946b2ec6c46f34c70a"),
    (0x00442CC3, 0x00442CE0, "状态指针减0xCE4反推FightRole基址", "5819a739e828650c1a40c0a1c13ef97ac35aadcf8eed90e7823f44a9751da3f7"),
    (0x00442F89, 0x00442FA0, "第二条Battle+0x10C8首项指针", "e87b6ff534e84695af28001e521f1f868535615bdccb376465f406c123319697"),
    (0x00443470, 0x004434AD, "同一角色锚点配对+0x3E4与+0x10C8写入", "257872472d626ecede9a349ced10562730708bfbd80d7b1d2449aa4256180a6a"),
    (0x004434F0, 0x00443518, "角色索引步长与FightRole+0x3E4基址复核", "01cd99531278ff04698d9c44a700a8b89ed36a9a72302b8c691e39cf60a34da7"),
    (0x00445FB0, 0x0044600E, "FightRole数组0xDE4步长与+0xCE4读取", "5d0ce2467c19d2fc668ce2860e936ea8ad635b56c3f6dab353bded25a531ef7a"),
)


# 短切片用于让人工审查能直接看到别名方程，不必只相信大区间哈希。
SHORT_ASSERTIONS: Tuple[Tuple[int, int, str, str], ...] = (
    (0x0044331B, 0x00443331, "角色索引LEA链与FightRole+0x3E4", "8d04f68d04868d0c408d14ce8d7c95008d9fe4030000"),
    (0x00443498, 0x004434A8, "同一ESI下+0x3E4对象指针与+0x10C8状态写", "8d8ee4030000c786c8100000be0b0000"),
    (0x00442CD3, 0x00442CE0, "状态指针减0xCE4并作为FightRole this调用", "8dae1cf3ffff8bcde860d7fdff"),
    (0x00442BEB, 0x00442C20, "+0x10C8首项指针按0xDE4遍历16角色", "8db5c8100000b9100000008bc68138bc0b000075188b9098fbffff85d2740839b848faffff7506c700b90b000005e40d00004975d8"),
    (0x00445FD3, 0x0044600E, "FightRole数组按0xDE4遍历并读取+0xCE4", "8b831802000085c074228b8c302c07000003c685c97e1581b8e40c0000c00b0000740957508bcbe8a1feffff81c6e40d00004781fe40de00007cc5"),
)


@dataclass(frozen=True)
class Section:
    """PE section到文件原始字节的最小映射信息。"""

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass(frozen=True)
class PeImage:
    """本工具所需的最小PE视图；不依赖第三方反汇编库。"""

    path: Path
    data: bytes
    image_base: int
    sections: Tuple[Section, ...]

    @classmethod
    def load(cls, path: Path) -> "PeImage":
        data = path.read_bytes()
        if data[:2] != b"MZ":
            raise ValueError(f"不是MZ可执行文件：{path}")
        pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError(f"不是PE文件：{path}")
        coff = pe_offset + 4
        section_count = struct.unpack_from("<H", data, coff + 2)[0]
        optional_size = struct.unpack_from("<H", data, coff + 16)[0]
        optional = coff + 20
        magic = struct.unpack_from("<H", data, optional)[0]
        if magic != 0x10B:
            raise ValueError("只支持本游戏使用的32位PE32映像")
        image_base = struct.unpack_from("<I", data, optional + 28)[0]
        table = optional + optional_size
        sections: List[Section] = []
        for index in range(section_count):
            off = table + index * 40
            name = data[off:off + 8].split(b"\0", 1)[0].decode("ascii", "replace")
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", data, off + 8)
            sections.append(Section(name, virtual_address, virtual_size, raw_offset, raw_size))
        return cls(path, data, image_base, tuple(sections))

    def section(self, name: str) -> Section:
        for item in self.sections:
            if item.name == name:
                return item
        raise KeyError(f"PE中没有section：{name}")

    def va_to_offset(self, va: int) -> int:
        rva = va - self.image_base
        for item in self.sections:
            span = max(item.virtual_size, item.raw_size)
            if item.virtual_address <= rva < item.virtual_address + span:
                return item.raw_offset + (rva - item.virtual_address)
        # PE headers也按ImageBase映射；保留这条兼容路径，但本节点代码切片都在.text。
        if 0 <= rva < len(self.data):
            return rva
        raise ValueError(f"VA不在文件映射内：0x{va:08X}")

    def read_va(self, start_va: int, size: int) -> bytes:
        off = self.va_to_offset(start_va)
        out = self.data[off:off + size]
        if len(out) != size:
            raise ValueError(f"读取越界：0x{start_va:08X}+0x{size:X}")
        return out

    def scan_text(self, literal: bytes) -> List[Tuple[int, int]]:
        """返回.text中每个原始字节命中的(VA, 文件偏移)。"""

        text = self.section(".text")
        blob = self.data[text.raw_offset:text.raw_offset + text.raw_size]
        text_va = self.image_base + text.virtual_address
        found: List[Tuple[int, int]] = []
        pos = 0
        while True:
            index = blob.find(literal, pos)
            if index < 0:
                break
            found.append((text_va + index, text.raw_offset + index))
            pos = index + 1
        return found


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def is_disp32_at(image: PeImage, file_offset: int) -> bool:
    """用窄化ModRM规则判断命中四字节是否位于真实disp32位置。

    本函数不是通用x86反汇编器；它只识别本节点出现的一字节opcode+ModRM，
    或一字节opcode+ModRM+SIB两种形式。这样足以排除0x45AB88的disp8+imm32假阳性，
    同时避免把“搜到四个字节”错误等同于“程序访问了这个结构位移”。
    """

    data = image.data
    # 普通形式：opcode, ModRM, disp32。mod==2表示32位位移。
    if file_offset >= 2:
        modrm = data[file_offset - 1]
        if (modrm >> 6) == 2 and (modrm & 7) != 4:
            return True
    # SIB形式：opcode, ModRM(r/m=4), SIB, disp32。
    if file_offset >= 3:
        modrm = data[file_offset - 2]
        if (modrm >> 6) == 2 and (modrm & 7) == 4:
            return True
    return False


def verify_hashes(original: PeImage, research: PeImage) -> Dict[str, object]:
    original_hash = sha256(original.data)
    research_hash = sha256(research.data)
    return {
        "完全未修改原版": {
            "路径": str(original.path), "字节数": len(original.data),
            "SHA-256": original_hash, "预期": EXPECTED_ORIGINAL_SHA256,
            "PASS": original_hash == EXPECTED_ORIGINAL_SHA256,
        },
        "历史研究版": {
            "路径": str(research.path), "字节数": len(research.data),
            "SHA-256": research_hash, "预期": EXPECTED_RESEARCH_SHA256,
            "PASS": research_hash == EXPECTED_RESEARCH_SHA256,
        },
    }


def verify_slices(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    out: List[Dict[str, object]] = []
    for start, end, label, expected in CRITICAL_SLICES:
        ob = original.read_va(start, end - start)
        rb = research.read_va(start, end - start)
        actual = sha256(ob)
        out.append({
            "名称": label,
            "范围": f"0x{start:08X}..0x{end:08X}",
            "长度": end - start,
            "原版SHA-256": actual,
            "预期SHA-256": expected,
            "双版本逐字节一致": ob == rb,
            "PASS": actual == expected and ob == rb,
        })
    return out


def verify_short_assertions(original: PeImage) -> List[Dict[str, object]]:
    out: List[Dict[str, object]] = []
    for start, end, label, expected_hex in SHORT_ASSERTIONS:
        actual_hex = original.read_va(start, end - start).hex()
        out.append({
            "名称": label,
            "范围": f"0x{start:08X}..0x{end:08X}",
            "实际HEX": actual_hex,
            "预期HEX": expected_hex,
            "PASS": actual_hex == expected_hex,
        })
    return out


def verify_literal_surfaces(original: PeImage) -> Dict[str, object]:
    ce4_raw = original.scan_text(struct.pack("<I", STATE_FROM_FIGHT_ROLE))
    ce4_valid = [va for va, off in ce4_raw if is_disp32_at(original, off)]
    ten_raw = original.scan_text(struct.pack("<I", STATE_FROM_ANCHOR))
    ten_valid = [va for va, off in ten_raw if is_disp32_at(original, off)]
    false_hits = [va for va, off in ce4_raw if not is_disp32_at(original, off)]
    return {
        "+0xCE4原始四字节命中": {
            "实际": [f"0x{x:08X}" for x, _ in ce4_raw],
            "预期": [f"0x{x:08X}" for x in EXPECTED_CE4_RAW_HITS],
            "PASS": tuple(x for x, _ in ce4_raw) == EXPECTED_CE4_RAW_HITS,
        },
        "+0xCE4真实disp32命中": {
            "实际": [f"0x{x:08X}" for x in ce4_valid],
            "预期": [f"0x{x:08X}" for x in EXPECTED_CE4_DISP32_HITS],
            "PASS": tuple(ce4_valid) == EXPECTED_CE4_DISP32_HITS,
        },
        "+0xCE4假阳性": {
            "实际": [f"0x{x:08X}" for x in false_hits],
            "预期": [f"0x{EXPECTED_CE4_FALSE_RAW_HIT:08X}"],
            "说明": "0x45AB88为C7 45 E4 0C 00 00 00；E4是disp8，0C是立即数，不是[base+0xCE4]。",
            "PASS": false_hits == [EXPECTED_CE4_FALSE_RAW_HIT],
        },
        "+0x10C8真实disp32命中": {
            "实际": [f"0x{x:08X}" for x in ten_valid],
            "预期": [f"0x{x:08X}" for x in EXPECTED_10C8_RAW_HITS],
            "PASS": tuple(ten_valid) == EXPECTED_10C8_RAW_HITS,
        },
    }


def build_report(original: PeImage, research: PeImage) -> Dict[str, object]:
    hashes = verify_hashes(original, research)
    slices = verify_slices(original, research)
    shorts = verify_short_assertions(original)
    surfaces = verify_literal_surfaces(original)

    # 这三个算式是固化65的核心。它们不是“数值相似”，而是同一个角色索引、
    # 同一个0xDE4步长下可双向换算的地址恒等式。
    equations = {
        "正向别名": {
            "表达式": "role_anchor + 0x3E4 + 0xCE4 == role_anchor + 0x10C8",
            "左侧数值": FIGHT_ROLE_FROM_ANCHOR + STATE_FROM_FIGHT_ROLE,
            "右侧数值": STATE_FROM_ANCHOR,
            "PASS": FIGHT_ROLE_FROM_ANCHOR + STATE_FROM_FIGHT_ROLE == STATE_FROM_ANCHOR,
        },
        "反向别名": {
            "表达式": "state_pointer - 0xCE4 == FightRole_pointer",
            "机器锚点": "0x442CD3 lea ebp,[esi-0xCE4]；esi来自Battle+0x10C8并按0xDE4步进",
            "PASS": original.read_va(0x00442CD3, 6).hex() == "8dae1cf3ffff",
        },
        "角色索引步长": {
            "表达式": "4 * (i + 8 * (3 * (i + 4 * (i + 8*i)))) == 0xDE4*i",
            "样本0到15": [4 * (i + 8 * (3 * (i + 4 * (i + 8 * i)))) for i in range(ROLE_COUNT)],
            "预期0到15": [ROLE_STRIDE * i for i in range(ROLE_COUNT)],
            "PASS": all(
                4 * (i + 8 * (3 * (i + 4 * (i + 8 * i)))) == ROLE_STRIDE * i
                for i in range(ROLE_COUNT)
            ),
        },
    }

    all_pass = (
        all(item["PASS"] for item in hashes.values())
        and all(item["PASS"] for item in slices)
        and all(item["PASS"] for item in shorts)
        and all(item["PASS"] for item in surfaces.values())
        and all(item["PASS"] for item in equations.values())
    )
    return {
        "工具": Path(__file__).name,
        "工具版本": TOOL_VERSION,
        "固化节点": "v0.7D阶段中-固化65",
        "目标": "闭合Battle角色锚点+0x10C8与FightRole+0xCE4的物理地址关系。",
        "输入哈希": hashes,
        "结构常量": {
            "角色数": ROLE_COUNT,
            "角色步长": f"0x{ROLE_STRIDE:X}",
            "角色锚点到FightRole": f"0x{FIGHT_ROLE_FROM_ANCHOR:X}",
            "角色锚点视图状态偏移": f"0x{STATE_FROM_ANCHOR:X}",
            "FightRole视图状态偏移": f"0x{STATE_FROM_FIGHT_ROLE:X}",
        },
        "地址恒等式": equations,
        "关键区间": slices,
        "短机器码断言": shorts,
        "完整字面位移面": surfaces,
        "闭合结论": {
            "物理关系": "+0x10C8与FightRole+0xCE4不是两层状态，也不存在等待寻找的复制/转移函数；二者是同一DWORD的两种基址口径。",
            "正向构造": "角色锚点+0x3E4得到FightRole；再加0xCE4恰好落到角色锚点+0x10C8。0x443498/0x44349E在同一ESI上并列展示两种口径。",
            "反向构造": "Battle tick先令状态指针指向Battle+0x10C8，再在0x442CD3减0xCE4恢复FightRole this并调用0x420440。",
            "数组关系": "16个角色槽的角色锚点、FightRole对象和统一状态DWORD都按0xDE4同步步进。",
            "固化64边界修正": "固化64把二者保守记作“共享码值、转移函数UNKNOWN”没有制造错误语义，但现在可升级为已确认精确物理别名。",
            "作者术语边界": "原作者字段名和0xBB9..0xBC7状态枚举名仍UNKNOWN；兼容层可中性称为role_action_state_code。",
        },
        "范围限制": [
            "本节点证明同一物理DWORD，不等于已经闭合该字段所有状态码的业务名称。",
            "完整动态Battle时序、间接函数指针和其它EXE版本不由本节点静态别名证明覆盖。",
            "0x45AB8A仅为disp8与立即数字节拼接的假阳性，不能计入+0xCE4结构访问。",
        ],
        "总体PASS": all_pass,
    }


def write_csv(path: Path, report: Dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    rows = [
        ("结构", "角色槽数量", "0x10", "16个角色槽"),
        ("结构", "角色槽步长", "0xDE4", "角色锚点、FightRole和状态DWORD同步步进"),
        ("别名", "FightRole基址", "role_anchor+0x3E4", "0x44332B、0x443498、0x443509"),
        ("别名", "锚点视图状态", "role_anchor+0x10C8", "Battle侧直接读写口径"),
        ("别名", "FightRole视图状态", "FightRole+0xCE4", "FightRole方法与全局指针表口径"),
        ("恒等式", "正向", "0x3E4+0xCE4=0x10C8", "同一物理DWORD"),
        ("恒等式", "反向", "state_pointer-0xCE4=FightRole_pointer", "0x442CD3机器指令直接实现"),
        ("边界", "作者正式字段名", "UNKNOWN", "role_action_state_code仅为兼容层中性名称"),
    ]
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(["类别", "项目", "值或表达式", "证据结论"])
        writer.writerows(rows)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="验证Battle角色锚点+0x10C8与FightRole+0xCE4为同一物理DWORD。"
    )
    parser.add_argument("--original-rpg", required=True, type=Path, help="完全未修改原版RPG.exe")
    parser.add_argument("--research-rpg", required=True, type=Path, help="历史研究版RPG.exe")
    parser.add_argument("--json-out", required=True, type=Path, help="输出机器审计JSON")
    parser.add_argument("--csv-out", required=True, type=Path, help="输出别名关系CSV")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    original = PeImage.load(args.original_rpg)
    research = PeImage.load(args.research_rpg)
    report = build_report(original, research)
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    write_csv(args.csv_out, report)
    print(f"[固化65] 总体结果：{'PASS' if report['总体PASS'] else 'FAIL'}")
    print(f"[固化65] JSON：{args.json_out}")
    print(f"[固化65] CSV ：{args.csv_out}")
    return 0 if report["总体PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
