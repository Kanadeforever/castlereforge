#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle Runtime 39 槽计时量子静态调查器。

本工具只读取 RPG.exe 与可选的 DDDES.SCI，用来重放“固化40”的机器证据。
它不会修改 EXE，不会写入游戏目录，也不会注入运行中的游戏进程。

固化40要回答的核心问题是：
“FightRole+0xA34..+0xACC 这 39 个 runtime 状态槽，每次减 10 到底多久发生一次？”

前代已经分别知道：
1. 固化4：0x420440 会扫描 39 个 DWORD runtime 槽，对正值执行减 10 并夹到 0；
2. 固化38：Battle 主对象 vtable[0] = 0x442920，而全局对象管理器每个 Legacy Tick
   最多调用一次这个 vtable[0]；当前 DDDES.SCI +0x11 = 20，所以名义 Legacy Tick
   是 50 ms / 20 Hz。

固化40把两条链真正接起来：
0x442920 -> 0x442A80 -> （结构门控） -> 每个有效 FightRole 调 0x420440。

因此在门控开放时：
- 每个有效 FightRole 每个 Legacy Tick 最多执行一次 39 槽递减；
- 每次递减的量子是 10 raw units；
- 当前 20 Hz 配置下，名义速度是 10 * 20 = 200 raw units / second；
- 等价地，raw magnitude 200 对应 1 秒“合格 Battle 逻辑推进时间”。

非常重要：
“1 raw unit = 5 ms”只能作为比例换算，不能理解成游戏真的有 5 ms 的调度 tick。
真实调度仍然以 50 ms Legacy Tick 为离散步长，每次一口气减 10。
而且 0x420440 还有 BattleObject+0xE224、全局 0x4696DC、FightRole 有效性等门控；
门控关闭时不会递减，所以真实墙钟持续时间可能比 raw/200 秒更长。
"""

from __future__ import annotations

# argparse 负责命令行参数。把输入/输出都显式写成参数，方便未来任何一个固化包独立重放。
import argparse
# hashlib 用来记录输入文件身份，避免把不同 EXE 的结果混到同一个证据结论里。
import hashlib
# json 用来写机器可读结果。固化节点之间可以直接比较 JSON，而不是依赖人工抄写。
import json
# struct 用来按 little-endian 读取 PE32 头与相对调用位移。
import struct
# sys 用于返回标准退出码：全部断言通过返回 0，有断言失败返回 1。
import sys
# pathlib.Path 让 Windows / Linux 路径都能用同一种写法处理。
from pathlib import Path
# Any 只用于类型注解，使输出字典结构更清楚。
from typing import Any


# 工具版本与固化节点绑定。以后如果判断逻辑变化，必须同步升级这里的版本号。
TOOL_VERSION = "0.1.0-固化40"

# 当前用户提供的 RPG.exe.org 解包后 SHA-256。它与更早登记的 Oracle 整文件 SHA 不同，
# 但固化34以后已经用大量机器断言证明当前覆盖代码区语义等价，所以这里只做身份记录，
# 不把“整文件 SHA 不同”误判成“当前机器证据失败”。
CURRENT_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"

# 当前 Sys.dat 中 DDDES.SCI 的稳定 SHA。提供 --dddes 时可以同时确认逻辑频率值来自正式样本。
CURRENT_DDDES_SHA256 = "1c8e3a6e0dfc314bcdc34aeae57388539da95d6443ee0441032452a7d76b2972"
# DDDES.SCI +0x11 是固化23/38已经证明的逻辑频率字节。
DDDES_LOGIC_RATE_OFFSET = 0x11

# Battle 主对象的 Legacy Tick 更新函数。
BATTLE_TICK_VA = 0x00442920
# 0x442920 每个合格 Battle Tick 调用的内部更新函数。
BATTLE_INNER_UPDATE_VA = 0x00442A80
# 39 槽 runtime 状态扫描与减 10 所在函数。
RUNTIME_UPDATE_VA = 0x00420440

# FightRole stride 在前代 Battle 研究中已闭合为 0xDE4。
FIGHT_ROLE_STRIDE = 0xDE4
# Battle 一次最多扫描 16 个 FightRole 槽。
FIGHT_ROLE_COUNT = 16
# runtime 状态数组从 FightRole+0xA34 开始，共 39 个 DWORD。
RUNTIME_ARRAY_OFFSET = 0xA34
RUNTIME_SLOT_COUNT = 39
# 每个合格调用对正值状态的固定递减量。
RUNTIME_DECREMENT_QUANTUM = 10


class PEImage:
    """只实现本工具需要的 PE32 VA→文件偏移映射与 .text 读取。"""

    def __init__(self, path: Path) -> None:
        # 一次性读完整文件。RPG.exe 只有几百 KB，这比维护文件句柄更简单、更不易出错。
        self.path = path
        self.data = path.read_bytes()

        # Windows PE 必须先有 DOS MZ 头。如果这里失败，后续所有虚拟地址都会失去意义。
        if self.data[:2] != b"MZ":
            raise ValueError("输入文件没有 MZ 头，不能按 RPG.exe PE32 解析")

        # DOS 头 +0x3C 保存 PE 头文件偏移 e_lfanew。
        pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]
        # 真正 PE 头开头必须是 ASCII 'PE\\0\\0'。
        if self.data[pe_offset : pe_offset + 4] != b"PE\0\0":
            raise ValueError("PE 签名错误")

        # COFF FileHeader +2 是 section 数量。
        section_count = struct.unpack_from("<H", self.data, pe_offset + 6)[0]
        # COFF FileHeader +16 是 OptionalHeader 长度。
        optional_size = struct.unpack_from("<H", self.data, pe_offset + 20)[0]
        # OptionalHeader 从 PE 签名 4 字节 + COFF 头 20 字节之后开始。
        optional_offset = pe_offset + 24
        # 当前研究对象是 PE32；OptionalHeader +0x1C 是 ImageBase。
        self.image_base = struct.unpack_from("<I", self.data, optional_offset + 0x1C)[0]
        # section table 紧跟 OptionalHeader，每个 section header 固定 40 字节。
        section_table = optional_offset + optional_size

        # 保存每个 section 的 RVA、磁盘偏移和长度，供后面把 VA 换算成文件偏移。
        self.sections: list[dict[str, int | str]] = []
        for index in range(section_count):
            # 找到第 index 个 section header 的起点。
            offset = section_table + index * 40
            # 前 8 字节是名称，NUL 后面的填充不属于名称。
            name = self.data[offset : offset + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            # +8 VirtualSize 是内存中的逻辑大小。
            virtual_size = struct.unpack_from("<I", self.data, offset + 8)[0]
            # +12 VirtualAddress 是相对 ImageBase 的 RVA。
            virtual_address = struct.unpack_from("<I", self.data, offset + 12)[0]
            # +16 SizeOfRawData 是磁盘中实际存放的长度。
            raw_size = struct.unpack_from("<I", self.data, offset + 16)[0]
            # +20 PointerToRawData 是磁盘中的起始偏移。
            raw_offset = struct.unpack_from("<I", self.data, offset + 20)[0]
            self.sections.append(
                {
                    "name": name,
                    "virtual_size": virtual_size,
                    "virtual_address": virtual_address,
                    "raw_size": raw_size,
                    "raw_offset": raw_offset,
                }
            )

    def va_to_offset(self, va: int) -> int:
        """把运行时绝对虚拟地址转换成 EXE 文件偏移。"""

        # section 表使用 RVA，所以先减掉 ImageBase。
        rva = va - self.image_base
        for section in self.sections:
            # 这个 section 在内存中的起始 RVA。
            start = int(section["virtual_address"])
            # VirtualSize 与 RawSize 可能因 PE 对齐不同；取较大值可以覆盖边缘地址。
            span = max(int(section["virtual_size"]), int(section["raw_size"]))
            if start <= rva < start + span:
                # 磁盘偏移 = section 磁盘起点 + 当前地址在 section 内的相对距离。
                return int(section["raw_offset"]) + (rva - start)
        raise ValueError(f"VA 0x{va:08X} 不在任何 PE section 中")

    def read_va(self, va: int, size: int) -> bytes:
        """按绝对 VA 读取固定长度字节。"""

        offset = self.va_to_offset(va)
        end = offset + size
        if end > len(self.data):
            raise ValueError(f"VA 0x{va:08X} 读取 {size} 字节会超出文件末尾")
        return self.data[offset:end]

    def text_section_bytes(self) -> tuple[int, bytes]:
        """返回 .text 起始 VA 和实际代码字节，用于扫描 direct E8 caller。"""

        for section in self.sections:
            if section["name"] == ".text":
                start_va = self.image_base + int(section["virtual_address"])
                raw_offset = int(section["raw_offset"])
                raw_size = int(section["raw_size"])
                return start_va, self.data[raw_offset : raw_offset + raw_size]
        raise ValueError("PE 中没有找到 .text section")


def sha256_bytes(data: bytes) -> str:
    """返回 bytes 的 SHA-256 小写十六进制摘要。"""

    return hashlib.sha256(data).hexdigest()


def machine_check(pe: PEImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    """逐字节检查某段关键机器码。"""

    # expected_hex 用空格分隔，便于和 objdump/x64dbg 证据互相核对。
    expected = bytes.fromhex(expected_hex)
    # 实际读取完全相同长度；任何一个字节不同都视为断言失败。
    actual = pe.read_va(va, len(expected))
    return {
        "地址": f"0x{va:08X}",
        "含义": meaning,
        "期望机器码": expected.hex(" "),
        "实际机器码": actual.hex(" "),
        "PASS": actual == expected,
    }


def direct_e8_callers(pe: PEImage, target_va: int) -> list[int]:
    """枚举 .text 中所有真正以 E8 rel32 直接调用 target_va 的 call 指令地址。"""

    text_va, code = pe.text_section_bytes()
    callers: list[int] = []

    # E8 指令总长度固定 5 字节：1 字节 opcode + 4 字节 signed rel32。
    for index in range(0, len(code) - 4):
        if code[index] != 0xE8:
            continue
        # rel32 必须按 signed 32-bit 解释。
        rel = struct.unpack_from("<i", code, index + 1)[0]
        call_va = text_va + index
        # x86 E8 的目标 = 下一条指令地址 + rel32。
        computed_target = call_va + 5 + rel
        if computed_target == target_va:
            callers.append(call_va)
    return callers


def parse_dddes_logic_rate(path: Path) -> dict[str, Any]:
    """只读取 DDDES.SCI 中与固化40时间换算有关的 +0x11 字节。"""

    raw = path.read_bytes()
    # 至少要有 +0x11 这个字节，所以长度必须 >= 0x12。
    if len(raw) <= DDDES_LOGIC_RATE_OFFSET:
        raise ValueError("DDDES.SCI 太短，无法读取 +0x11 逻辑频率参数")
    logic_rate = raw[DDDES_LOGIC_RATE_OFFSET]
    # 0 Hz 没有意义，而且前代已经证明原版会用这个值做除法，因此明确拒绝。
    if logic_rate == 0:
        raise ValueError("DDDES.SCI +0x11 为0，不能计算 Legacy Clock")
    digest = sha256_bytes(raw)
    return {
        "路径": str(path),
        "文件大小": len(raw),
        "SHA256": digest,
        "是否当前正式DDDES样本": digest == CURRENT_DDDES_SHA256,
        "逻辑频率_Hz_加0x11": logic_rate,
    }


def build_checks(pe: PEImage) -> list[dict[str, Any]]:
    """建立固化40需要的全部关键机器码断言。"""

    checks: list[dict[str, Any]] = []

    # Battle vtable[0] 入口先检查 battle-active 全局字节。只有非0才进入本次主更新。
    checks.append(
        machine_check(
            pe,
            0x00442920,
            "a0 0c 24 8e 00 57 84 c0 8b f9 0f 84 af 00 00 00",
            "Battle vtable[0] 入口读取0x8E240C并以0作为整次更新返回门控",
        )
    )

    # 同一主更新在通过入口门后只 direct-call 一次 0x442A80。
    checks.append(
        machine_check(
            pe,
            0x004429A3,
            "8b cf e8 d6 00 00 00",
            "0x442920 把 this 放入ECX并每次只直接调用一次0x442A80",
        )
    )

    # 0x442A80 后段首先要求 BattleObject+0xE224 == -1；否则跳过整段39槽角色更新。
    checks.append(
        machine_check(
            pe,
            0x00442C95,
            "83 bd 24 e2 00 00 ff 0f 85 1c 01 00 00",
            "BattleObject+0xE224 必须等于-1，否则直接跳到0x442DBE并绕过39槽递减循环",
        )
    )

    # 第二个结构门控读取全局 0x4696DC；为0同样跳过角色递减循环。
    checks.append(
        machine_check(
            pe,
            0x00442CA2,
            "a0 dc 96 46 00 84 c0 0f 84 0f 01 00 00",
            "全局0x4696DC必须非0，否则绕过39槽递减循环",
        )
    )

    # 循环从角色索引0开始；每个角色先看 FightRole 相关有效性字段是否为负。
    checks.append(
        machine_check(
            pe,
            0x00442CC3,
            "33 ff 8b 86 48 fa ff ff 85 c0 0f 8c db 00 00 00",
            "per-role循环从索引0开始；当前角色有效性字段<0时跳过0x420440",
        )
    )

    # 对有效角色，把真正 FightRole 基址放到 ECX，然后 direct-call 0x420440。
    checks.append(
        machine_check(
            pe,
            0x00442CD3,
            "8d ae 1c f3 ff ff 8b cd e8 60 d7 fd ff",
            "由当前角色游标反推出FightRole基址后调用0x420440",
        )
    )

    # 每处理一个槽，角色游标 +0xDE4，索引+1，直到16个角色。
    checks.append(
        machine_check(
            pe,
            0x00442DAA,
            "8b 6c 24 10 47 81 c6 e4 0d 00 00 83 ff 10 0f 8c 07 ff ff ff",
            "per-role循环每次+0xDE4并扫描固定16个FightRole槽",
        )
    )

    # 0x420440 的状态计时循环明确从 FightRole+0xA34 开始，索引EDX从0开始。
    checks.append(
        machine_check(
            pe,
            0x00420662,
            "8d be 34 0a 00 00 33 d2 8b c7 5b",
            "runtime状态计时循环起点=FightRole+0xA34，slot index从0开始",
        )
    )

    # 对正值执行 add ecx,-10；写回后如果<=0则清零。
    checks.append(
        machine_check(
            pe,
            0x0042066D,
            "8b 08 85 c9 7e 1d 83 fa 10 75 09 85 c9 7e 14 83 fa 13 74 0f 83 c1 f6 85 c9 89 08 7f 06 c7 00 00 00 00 00",
            "39槽正值路径的实际递减量为10，递减后<=0时夹到0",
        )
    )

    # 循环每次 index+1、指针+4，到 index==39 停止。
    checks.append(
        machine_check(
            pe,
            0x00420690,
            "42 83 c0 04 83 fa 27 7c d4",
            "runtime数组按DWORD步进并固定扫描39槽",
        )
    )

    return checks


def build_report(exe: Path, dddes: Path | None) -> dict[str, Any]:
    """组合机器断言、直接caller、时间比例和严格边界，生成最终机器报告。"""

    pe = PEImage(exe)
    exe_sha = sha256_bytes(pe.data)
    checks = build_checks(pe)

    # 扫 direct E8 caller 是为了证明当前 EXE 中没有第二个直接入口以不同频率调用0x420440。
    runtime_callers = direct_e8_callers(pe, RUNTIME_UPDATE_VA)
    inner_callers = direct_e8_callers(pe, BATTLE_INNER_UPDATE_VA)

    # 如果用户提供 DDDES，就使用真实文件 +0x11；否则仍允许只做 EXE 机器码回归。
    dddes_info: dict[str, Any] | None = None
    logic_rate: int | None = None
    if dddes is not None:
        dddes_info = parse_dddes_logic_rate(dddes)
        logic_rate = int(dddes_info["逻辑频率_Hz_加0x11"])

    # 只有拿到真实 logic_rate 才计算现实比例；否则把比例字段保留为 null，避免偷偷假设20。
    timing: dict[str, Any]
    if logic_rate is not None:
        timing = {
            "Legacy逻辑频率_Hz": logic_rate,
            "LegacyTick名义毫秒": 1000.0 / logic_rate,
            "每个合格LegacyTick递减raw单位": RUNTIME_DECREMENT_QUANTUM,
            "合格Battle逻辑时间每秒递减raw单位": RUNTIME_DECREMENT_QUANTUM * logic_rate,
            "raw单位比例换算_毫秒每raw单位": 1000.0 / (RUNTIME_DECREMENT_QUANTUM * logic_rate),
            "严格解释": (
                "调度步长仍是Legacy Tick；当前配置每50ms最多减10。"
                "5ms/raw-unit只是比例，不代表存在5ms调度器。"
            ),
        }
    else:
        timing = {
            "Legacy逻辑频率_Hz": None,
            "LegacyTick名义毫秒": None,
            "每个合格LegacyTick递减raw单位": RUNTIME_DECREMENT_QUANTUM,
            "合格Battle逻辑时间每秒递减raw单位": None,
            "raw单位比例换算_毫秒每raw单位": None,
            "严格解释": "未提供DDDES.SCI，因此只冻结每个合格Legacy Tick减10，不自行假设Hz。",
        }

    # caller 集合本身也是断言：当前 EXE 中 0x420440 只有 0x442CDB 一个 direct E8 caller；
    # 0x442A80 只有 0x4429A5 一个 direct E8 caller。这能排除第二条已知直接频率路径。
    caller_checks = {
        "0x420440_direct_E8_callers": [f"0x{x:08X}" for x in runtime_callers],
        "0x420440_期望": ["0x00442CDB"],
        "0x420440_PASS": runtime_callers == [0x00442CDB],
        "0x442A80_direct_E8_callers": [f"0x{x:08X}" for x in inner_callers],
        "0x442A80_期望": ["0x004429A5"],
        "0x442A80_PASS": inner_callers == [0x004429A5],
    }

    # 最终 PASS 只有机器字节和 caller 集合都通过才为真。
    all_checks_pass = all(bool(item["PASS"]) for item in checks)
    all_pass = all_checks_pass and bool(caller_checks["0x420440_PASS"]) and bool(caller_checks["0x442A80_PASS"])

    return {
        "工具": "幽城幻剑录Battle Runtime 39槽计时量子静态调查器",
        "工具版本": TOOL_VERSION,
        "输入RPG": {
            "路径": str(exe),
            "文件大小": len(pe.data),
            "SHA256": exe_sha,
            "是否当前RPG.exe.org身份SHA": exe_sha == CURRENT_RPG_ORG_SHA256,
        },
        "输入DDDES": dddes_info,
        "结构常量": {
            "FightRole_stride": FIGHT_ROLE_STRIDE,
            "FightRole_count": FIGHT_ROLE_COUNT,
            "runtime数组偏移": f"0x{RUNTIME_ARRAY_OFFSET:X}",
            "runtime槽数": RUNTIME_SLOT_COUNT,
            "每次递减量": RUNTIME_DECREMENT_QUANTUM,
        },
        "机器码断言": checks,
        "机器码断言通过数": sum(1 for item in checks if item["PASS"]),
        "机器码断言总数": len(checks),
        "直接调用集合": caller_checks,
        "时间比例": timing,
        "固化40边界": [
            "0x442920由统一对象管理器每个Legacy逻辑步最多调用一次；这条事实继承固化38。",
            "0x442920每次通过入口gate后只直接调用一次0x442A80。",
            "0x442A80只有在BattleObject+0xE224==-1且0x4696DC!=0时才进入39槽per-role循环。",
            "per-role循环固定扫描16个0xDE4 stride角色槽；角色有效性字段<0时跳过该角色。",
            "有效FightRole每次调用0x420440后，FightRole+0xA34起39个DWORD正值各减10并夹零。",
            "因此递减速率是10 raw units / eligible Legacy Tick，而不是无条件墙钟计时。",
            "当前DDDES=20时为名义200 raw units/s；raw 200对应1秒合格Battle逻辑推进时间。",
            "动作/流程gate、主线程阻塞以及Legacy pending单bit合并都可能让真实墙钟持续时间更长。",
            "不能把1 raw unit直接实现成5ms timer；兼容引擎应保持50ms离散tick、每tick减10的原始量子。",
        ],
        "全部PASS": all_pass,
    }


def main() -> int:
    """解析参数、生成报告、可选写JSON，并用退出码表示回归结果。"""

    parser = argparse.ArgumentParser(description="《幽城幻剑录》Battle Runtime 39槽计时量子静态调查器")
    parser.add_argument("RPG", type=Path, help="要验证的RPG.exe路径")
    parser.add_argument(
        "--dddes",
        type=Path,
        default=None,
        help="可选：从Sys.dat提取的DDDES.SCI；提供后会计算20Hz/50ms现实比例",
    )
    parser.add_argument("--json", type=Path, default=None, help="可选：把完整机器结果写入UTF-8 JSON")
    parser.add_argument("--版本", action="version", version=TOOL_VERSION)
    args = parser.parse_args()

    try:
        report = build_report(args.RPG, args.dddes)
    except Exception as exc:  # noqa: BLE001 - CLI工具需要把所有输入错误转成清楚的中文错误信息。
        print(f"错误：{exc}", file=sys.stderr)
        return 2

    # ensure_ascii=False 保留中文，indent=2 让证据文件可以人工审阅。
    text = json.dumps(report, ensure_ascii=False, indent=2)
    print(text)

    if args.json is not None:
        # 如果父目录不存在，先创建，避免用户手工建目录。
        args.json.parent.mkdir(parents=True, exist_ok=True)
        # 固定 UTF-8 + LF；这是Python工具输出JSON，不是Windows BAT/CMD。
        args.json.write_text(text + "\n", encoding="utf-8")
        print(f"\n已写入：{args.json}")

    # 全部PASS才返回0；CI/后续封包脚本可直接依赖退出码。
    return 0 if report["全部PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
