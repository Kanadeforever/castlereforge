#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle D7 结果分派参数4 / detector4 静态边界调查器。

这个工具只读取 RPG.exe，不会修改 EXE，也不会向游戏目录写任何内容。
它专门重放固化43的几个关键事实：

1. Battle 结果容器的 vtable[0] 确实是 0x4328A0，因此这里是已证 canonical 结果更新状态机；
2. 0x4328A0 调用结果 detector 0x443590 后，只会向 result dispatcher 0x443660
   传入参数1、2、3；detector 返回4时会被明确改写成 dispatcher 参数1；
3. detector4 路径还把 DataCenter+0x118 写成1，并跳过普通结果 UI 的启动调用；
4. 0x443660 自身确实保留参数4分支，并引用 Big5/CP950 文本“因  事  停  戰”；
5. 当前 EXE 的 .text 中，0x443660 只有三个 direct E8 caller，而且全部位于 0x4328A0；
6. 当前整个 EXE 中没有把 0x443660 地址以 little-endian 绝对函数指针常量保存到别处。

第5、6条只是“当前静态直接调用/静态地址常量面”的证据。它们不能数学证明不存在
运行时计算地址、自修改代码、外部补丁或其他版本新增的间接调用。因此固化43只把参数4
冻结为“当前已证 canonical D7 状态机没有生产者 / 静态孤立结果标题分支”，不会写成
“全程序绝对死代码”。
"""

from __future__ import annotations

# argparse 用来读取命令行里的 RPG.exe 路径与可选 JSON 输出路径。
import argparse
# hashlib 用来记录输入 EXE 的 SHA-256，防止把不同版本的机器证据混在一起。
import hashlib
# json 用来输出机器可读调查结果，后续固化包可以自动重放。
import json
# struct 用来读取 PE32 头、DWORD、以及 x86 E8 rel32 的有符号位移。
import struct
# sys 用来返回明确的成功/失败退出码。
import sys
# pathlib.Path 统一处理 Windows / Linux 路径，不手工拼接斜杠。
from pathlib import Path
# Any 只用于类型标注，让大型结果字典仍然容易读懂。
from typing import Any


TOOL_VERSION = "0.1.0-固化43"
CURRENT_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"

# 下面这些地址全部来自固化43正在复核的同版 RPG.exe。
RESULT_CONTAINER_VTABLE = 0x00460DC4
RESULT_UPDATE = 0x004328A0
OUTCOME_DETECTOR = 0x00443590
RESULT_DISPATCHER = 0x00443660
RESULT_TEXT_STOP_REASON = 0x0046C4DC
DATA_CENTER_GLOBAL = 0x008E1C48


class PEImage:
    """只实现本调查器需要的 PE32 VA 映射、.text 扫描与固定地址读取。"""

    def __init__(self, path: Path) -> None:
        # 一次性把 EXE 读入内存。RPG.exe 很小，后面每个机器断言就不必重复打开文件。
        self.path = path
        self.data = path.read_bytes()

        # Windows PE 必须先有 DOS 时代留下的 MZ 头；没有就说明用户选错了文件。
        if self.data[:2] != b"MZ":
            raise ValueError("输入文件没有 MZ 头，不是有效的 Windows PE")

        # DOS 头 +0x3C 保存真正 PE header 的文件偏移。
        self.pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]
        # 真正 PE header 必须以四字节固定签名 PE\0\0 开始。
        if self.data[self.pe_offset:self.pe_offset + 4] != b"PE\0\0":
            raise ValueError("PE 签名错误")

        # COFF header 中记录 section 数量和 OptionalHeader 大小。
        section_count = struct.unpack_from("<H", self.data, self.pe_offset + 6)[0]
        optional_size = struct.unpack_from("<H", self.data, self.pe_offset + 20)[0]
        self.optional_offset = self.pe_offset + 24

        # 原游戏是32位程序，所以 OptionalHeader magic 必须是 PE32 的 0x10B。
        magic = struct.unpack_from("<H", self.data, self.optional_offset)[0]
        if magic != 0x10B:
            raise ValueError(f"只支持 PE32；当前 OptionalHeader magic=0x{magic:04X}")

        # PE32 OptionalHeader +0x1C 是 ImageBase；当前正常值是0x00400000，但不硬编码。
        self.image_base = struct.unpack_from("<I", self.data, self.optional_offset + 0x1C)[0]

        # section table 紧跟 OptionalHeader，每项固定40字节。我们把后续VA换算需要的字段保存下来。
        section_table = self.optional_offset + optional_size
        self.sections: list[dict[str, int | str]] = []
        for index in range(section_count):
            entry = section_table + index * 40
            # section name 最多8字节，以 NUL 截断，例如 .text / .rdata / .data。
            name = self.data[entry:entry + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            virtual_size = struct.unpack_from("<I", self.data, entry + 8)[0]
            virtual_address = struct.unpack_from("<I", self.data, entry + 12)[0]
            raw_size = struct.unpack_from("<I", self.data, entry + 16)[0]
            raw_offset = struct.unpack_from("<I", self.data, entry + 20)[0]
            self.sections.append({
                "name": name,
                "virtual_size": virtual_size,
                "virtual_address": virtual_address,
                "raw_size": raw_size,
                "raw_offset": raw_offset,
            })

    def va_to_offset(self, va: int) -> int:
        """把反汇编里使用的绝对虚拟地址 VA 换算成 EXE 文件偏移。"""
        # 先减 ImageBase 得到 RVA；section table 里的 VirtualAddress 使用的就是 RVA。
        rva = va - self.image_base
        for section in self.sections:
            start = int(section["virtual_address"])
            # 某些PE的 VirtualSize 与 RawSize 不完全相等，所以取二者较大值做安全覆盖。
            span = max(int(section["virtual_size"]), int(section["raw_size"]))
            if start <= rva < start + span:
                return int(section["raw_offset"]) + (rva - start)
        raise ValueError(f"VA 0x{va:08X} 不属于任何已映射 section")

    def read_va(self, va: int, size: int) -> bytes:
        """从某个 VA 精确读取 size 个字节。"""
        offset = self.va_to_offset(va)
        end = offset + size
        if end > len(self.data):
            raise ValueError(f"读取 0x{va:08X}+{size} 超出文件末尾")
        return self.data[offset:end]

    def read_u32_va(self, va: int) -> int:
        """读取一个 little-endian DWORD；用于验证 vtable 项。"""
        return struct.unpack("<I", self.read_va(va, 4))[0]

    def text(self) -> tuple[int, bytes]:
        """返回 .text 的起始 VA 和磁盘代码字节，供 E8 caller 全量扫描。"""
        for section in self.sections:
            if section["name"] == ".text":
                start_va = self.image_base + int(section["virtual_address"])
                raw_offset = int(section["raw_offset"])
                raw_size = int(section["raw_size"])
                return start_va, self.data[raw_offset:raw_offset + raw_size]
        raise ValueError("PE 中没有找到 .text section")

    def file_offset_to_va(self, offset: int) -> int | None:
        """把文件偏移反算成VA；用于报告绝对地址常量出现在什么位置。"""
        for section in self.sections:
            raw_start = int(section["raw_offset"])
            raw_size = int(section["raw_size"])
            if raw_start <= offset < raw_start + raw_size:
                return self.image_base + int(section["virtual_address"]) + (offset - raw_start)
        # PE header 等不映射到普通 section 的位置返回 None，不擅自伪造 VA。
        return None


def sha256_bytes(blob: bytes) -> str:
    """计算一段字节的 SHA-256 十六进制文本。"""
    return hashlib.sha256(blob).hexdigest()


def machine_check(pe: PEImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    """比较固定 VA 的真实机器码与固化43期望字节。"""
    expected = bytes.fromhex(expected_hex)
    actual = pe.read_va(va, len(expected))
    return {
        "地址": f"0x{va:08X}",
        "含义": meaning,
        "期望机器码": expected.hex(" "),
        "实际机器码": actual.hex(" "),
        "PASS": actual == expected,
    }


def direct_e8_callers(pe: PEImage, target_va: int) -> list[int]:
    """枚举 .text 中所有 opcode E8 rel32 直接调用 target_va 的指令地址。"""
    text_va, code = pe.text()
    callers: list[int] = []
    # E8 后必须有4字节相对位移，因此最后4字节不可能是完整call指令起点。
    for index in range(len(code) - 4):
        if code[index] != 0xE8:
            continue
        # rel32 按有符号32位整数解释；负数表示向低地址调用。
        relative = struct.unpack_from("<i", code, index + 1)[0]
        call_va = text_va + index
        destination = call_va + 5 + relative
        if destination == target_va:
            callers.append(call_va)
    return callers


def absolute_pointer_hits(pe: PEImage, target_va: int) -> list[dict[str, Any]]:
    """搜索整个 EXE 是否直接保存 target_va 的4字节 little-endian 常量。"""
    needle = struct.pack("<I", target_va)
    hits: list[dict[str, Any]] = []
    start = 0
    while True:
        offset = pe.data.find(needle, start)
        if offset < 0:
            break
        va = pe.file_offset_to_va(offset)
        hits.append({
            "文件偏移": f"0x{offset:X}",
            "映射VA": None if va is None else f"0x{va:08X}",
        })
        # 从命中位置后一字节继续，连着出现或重叠的常量也不会漏掉。
        start = offset + 1
    return hits


def read_cp950_cstr(pe: PEImage, va: int, limit: int = 128) -> str:
    """读取 NUL 结束的 CP950/Big5 字符串；结果分派文本就是这种编码。"""
    raw = pe.read_va(va, limit)
    text_bytes = raw.split(b"\0", 1)[0]
    return text_bytes.decode("cp950", errors="replace")


def build_report(exe_path: Path) -> dict[str, Any]:
    """执行全部固化43断言并返回结构化报告。"""
    pe = PEImage(exe_path)
    exe_sha = sha256_bytes(pe.data)

    # 这些断言故意覆盖“容器身份→detector→参数改写→dispatcher 1/2/3/4分支”整条链，
    # 而不是只验证一处 push 1。这样将来二进制有任何局部变化时能精确看到断在哪里。
    checks = [
        machine_check(pe, 0x004320C6, "c7 06 c4 0d 46 00", "Battle结果容器构造器把vtable写为0x460DC4"),
        machine_check(pe, 0x004328B2, "e8 d9 0c 01 00", "canonical结果update调用outcome detector 0x443590"),
        machine_check(pe, 0x004328DB, "83 ff 01 74 0f 83 ff 02 74 0a 83 ff 04 74 05 83 ff 03 75 7b", "结果update只把detector 1/2/4/3视为结束分支"),
        machine_check(pe, 0x004328FB, "83 ff 04 75 1d", "detector为1或4都会汇合到同一dispatcher参数1路径"),
        machine_check(pe, 0x00432906, "6a 01 e8 53 0d 01 00", "detector1/4路径固定push 1后调用result dispatcher"),
        machine_check(pe, 0x00432913, "c7 81 18 01 00 00 01 00 00 00", "detector1/4路径把DataCenter+0x118固定写1"),
        machine_check(pe, 0x00432922, "8b 8e 18 02 00 00 57 e8 32 0d 01 00", "detector2路径push edi=2后调用result dispatcher"),
        machine_check(pe, 0x0043293C, "83 ff 03 75 19", "detector3拥有独立dispatcher分支"),
        machine_check(pe, 0x00432941, "8b 8e 18 02 00 00 57 e8 13 0d 01 00", "detector3路径push edi=3后调用result dispatcher"),
        machine_check(pe, 0x0043295A, "83 ff 04 74 0b", "detector4在dispatcher1处理后跳过普通结果UI启动调用0x412440"),
        machine_check(pe, 0x0044369B, "83 f8 01", "result dispatcher先识别参数1并进入胜利奖励主链"),
        machine_check(pe, 0x00443ACB, "83 f8 02 75 12", "result dispatcher保留参数2标题分支"),
        machine_check(pe, 0x00443AE2, "83 f8 03 75 12", "result dispatcher保留参数3标题分支"),
        machine_check(pe, 0x00443AF9, "83 f8 04 75 3b", "result dispatcher确实保留参数4标题分支"),
        machine_check(pe, 0x00443B09, "68 dc c4 46 00", "dispatcher参数4把0x46C4DC结果标题送入UI"),
    ]

    # vtable 第一项直接读DWORD，比只看构造器写vtable更直接证明0x4328A0是该对象的vtable[0]。
    vtable_first = pe.read_u32_va(RESULT_CONTAINER_VTABLE)
    vtable_ok = vtable_first == RESULT_UPDATE

    # 分别枚举 detector 和 dispatcher 的 direct E8 caller；预期都是非常窄的集合。
    detector_callers = direct_e8_callers(pe, OUTCOME_DETECTOR)
    dispatcher_callers = direct_e8_callers(pe, RESULT_DISPATCHER)
    expected_detector_callers = [0x004328B2]
    expected_dispatcher_callers = [0x00432908, 0x00432929, 0x00432948]

    # 如果0x443660被写进vtable/回调表，通常会以 little-endian DWORD 出现在文件中。
    # 当前结果应为0；但报告仍完整保存命中列表，未来版本一旦变化就能直接看到地址。
    pointer_hits = absolute_pointer_hits(pe, RESULT_DISPATCHER)

    # 直接从 EXE 解码参数4标题，避免只依赖历史人工转录。
    param4_text = read_cp950_cstr(pe, RESULT_TEXT_STOP_REASON)
    param4_text_ok = "因" in param4_text and "事" in param4_text and "停" in param4_text and "戰" in param4_text

    machine_pass = sum(1 for item in checks if item["PASS"])
    all_pass = (
        machine_pass == len(checks)
        and vtable_ok
        and detector_callers == expected_detector_callers
        and dispatcher_callers == expected_dispatcher_callers
        and len(pointer_hits) == 0
        and param4_text_ok
    )

    # 报告同时区分“事实”和“证据边界”。兼容引擎可以据此实现canonical状态机，
    # 但不能把没有静态生产者的参数4直接删掉，因为格式/旧版本/外部脚本仍可能利用它。
    return {
        "工具版本": TOOL_VERSION,
        "输入": {
            "RPG.exe": str(exe_path),
            "SHA256": exe_sha,
            "当前RPG.org期望SHA256": CURRENT_RPG_ORG_SHA256,
            "整文件SHA匹配当前org": exe_sha == CURRENT_RPG_ORG_SHA256,
            "ImageBase": f"0x{pe.image_base:08X}",
        },
        "对象与调用面": {
            "Battle结果容器vtable": f"0x{RESULT_CONTAINER_VTABLE:08X}",
            "vtable[0]实际值": f"0x{vtable_first:08X}",
            "vtable[0]是0x4328A0": vtable_ok,
            "outcome detector": f"0x{OUTCOME_DETECTOR:08X}",
            "detector direct E8 callers": [f"0x{x:08X}" for x in detector_callers],
            "detector caller集合符合预期": detector_callers == expected_detector_callers,
            "result dispatcher": f"0x{RESULT_DISPATCHER:08X}",
            "dispatcher direct E8 callers": [f"0x{x:08X}" for x in dispatcher_callers],
            "dispatcher caller集合符合预期": dispatcher_callers == expected_dispatcher_callers,
            "dispatcher地址静态绝对指针命中": pointer_hits,
            "dispatcher地址静态绝对指针命中数": len(pointer_hits),
        },
        "参数4文本": {
            "地址": f"0x{RESULT_TEXT_STOP_REASON:08X}",
            "CP950解码": param4_text,
            "包含因事停戰四字": param4_text_ok,
        },
        "detector到dispatcher精确语义": {
            "detector=1": "dispatcher(1)，DataCenter+0x118=1，随后可进入普通结果UI启动路径",
            "detector=2": "dispatcher(2)，DataCenter+0x118=2，随后进入普通结果UI启动路径",
            "detector=3": "dispatcher(3)，DataCenter+0x118=3，随后进入普通结果UI启动路径",
            "detector=4": "dispatcher(1)，DataCenter+0x118=1；之后cmp edi,4命中，跳过普通结果UI启动调用0x412440",
            "dispatcher参数4": "函数内部存在并显示“因事停戰”标题，但当前已证canonical结果update没有任何路径传入4",
        },
        "机器码断言": checks,
        "机器码PASS": machine_pass,
        "机器码总数": len(checks),
        "结论": {
            "canonical参数4生产者": "未发现；当前0x443660三个direct E8 caller只形成参数1/2/3，detector4明确改写成参数1",
            "实现要求": "兼容引擎的canonical Battle结果状态机按原版detector4→dispatcher1处理并跳过普通结果UI；参数4分支作为可保留的legacy/latent结果标题分支，不得擅自绑定detector4",
            "证据边界": "不形式化排除运行时计算函数地址、自修改/外部补丁、其他版本或尚未证明的非canonical间接调用",
        },
        "PASS": all_pass,
    }


def main() -> int:
    """命令行入口：执行调查、打印JSON，并按PASS返回0/1。"""
    parser = argparse.ArgumentParser(
        description="《幽城幻剑录》固化43：D7结果分派参数4与detector4改写静态调查器"
    )
    parser.add_argument("rpg_exe", type=Path, help="要验证的32位 RPG.exe / RPG.exe.org 解压后文件")
    parser.add_argument("--json", type=Path, help="可选：把完整调查结果写入指定UTF-8 JSON文件")
    args = parser.parse_args()

    try:
        report = build_report(args.rpg_exe)
    except Exception as exc:  # noqa: BLE001 - 命令行工具要把输入错误清楚打印给初学者，而不是只给traceback。
        print(f"[错误] {exc}", file=sys.stderr)
        return 2

    # ensure_ascii=False 让中文直接出现在JSON里；indent=2便于人类逐层阅读。
    rendered = json.dumps(report, ensure_ascii=False, indent=2)
    print(rendered)

    if args.json is not None:
        # 先建立父目录，用户指定“数据/固化43/xxx.json”时无需手工创建目录。
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(rendered + "\n", encoding="utf-8")

    # PASS返回0，任何机器证据不一致返回1；自动回归脚本可以直接依赖退出码。
    return 0 if report["PASS"] else 1


if __name__ == "__main__":
    sys.exit(main())
