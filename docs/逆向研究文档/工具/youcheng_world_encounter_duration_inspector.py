#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》世界遇敌持续状态事件计数静态调查器。

这个工具只读取 RPG.exe，用来重放“固化41”的核心机器证据。
它不会修改 EXE，也不会向游戏目录写任何文件；只有用户显式提供 --output 时，
才会把调查结果写成一个 JSON 文件。

固化41回答的是一个很容易被“50 ms / 20 Hz Legacy Tick”误导的问题：

    world action code13 / code14 写入的常量 1500，究竟是不是 1500 个 Legacy Tick？

答案是否定的。

机器码给出的真实链路是：

1. code13 / code14 都把全局 0x0046F61C 写成 1500；
2. 只有 0x004034C0 会把配套计数器 0x0046F620 加 1；
3. 0x004034C0 在整个 .text 中只有两个 direct E8 caller：0x00408CB1 与 0x00408CBC；
4. 这两个 caller 都位于 0x00408C40，并且只有第二参数非 0 时才会到达；
5. 0x00409580 的“输入启用”分支会把 0x00408A70 的返回值保留为
   0x00408C40 的第二参数；禁用分支明确传 0；
6. 0x00408A70 通过 USER32.GetKeyState 读取 VK_RBUTTON(2) 与 VK_LBUTTON(1)，
   把当前鼠标键状态编码成 0 / 1 / 2 / 0x52；
7. 它把新状态与 0x0089F7E4 中的上一次状态比较：
   - 状态没变 -> 返回 0；
   - 状态变化 -> 保存新状态，并返回“新状态本身”；
   因而“松开到 0”虽然是状态变化，但返回值仍为 0，不会进入 0x004034C0；
8. 所以 1500 的计数域不是墙钟时间，而是“遇敌系统启用时，鼠标按键状态
   变化到非 0 状态，并且当前 movement profile 可以映射为遇敌权重 1 或 2”的
   合格输入事件次数。

还有一个必须精确保留的 off-by-one：
0x004034C0 先把 0x0046F620 加 1，再比较 counter <= limit。
limit=1500 时，前 1500 次合格调用都不会清除；第 1501 次调用把
0x0046F61C 与 0x0046F620 一起清 0。因此兼容引擎应按原版比较顺序复刻，
不能把它简化成“计到 1500 就立即在第1500次前清掉”。

固化41仍然不强命名作者业务术语。这里使用“遇敌持续状态 / 合格输入事件”
是根据已经闭合的 consumer 链给出的实现级中性名称，而不是声称原作者源码中
一定使用了相同变量名。
"""

from __future__ import annotations

# argparse 解析命令行参数。把 EXE 和输出路径都做成参数，便于每个固化包独立重放。
import argparse
# hashlib 计算输入文件 SHA-256，防止把不同 RPG.exe 的结果混为一谈。
import hashlib
# json 用来输出机器可读证据，方便以后版本自动回归。
import json
# struct 用来解析 PE32 头、导入表和 x86 E8 rel32 位移。
import struct
# sys 用来返回标准进程退出码：所有断言通过为0，存在失败为1。
import sys
# Path 统一处理 Windows/Linux 路径；研究包在不同环境解压后无需改代码。
from pathlib import Path
# Any 只用于输出字典的类型注解，让代码阅读者更容易理解数据形状。
from typing import Any


# 工具版本必须和固化节点绑定；以后若修改证据判断逻辑，应同步升级版本。
TOOL_VERSION = "0.1.0-固化41"

# 当前用户提供的 RPG.exe.org 解包后 SHA-256。
# 它与早期 canonical Oracle 的整文件 SHA 不同，但固化34之后已在大量覆盖地址上
# 证明机器语义等价，因此这里只记录身份，不把 SHA 差异直接判成研究失败。
CURRENT_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"

# 下面这些地址都是固化41需要重放的核心节点。
WORLD_ENCOUNTER_UPDATE_VA = 0x004034C0
MOUSE_STATE_ENCODER_VA = 0x00408A70
INPUT_TO_ENCOUNTER_BRIDGE_VA = 0x00408C40
SELECTED_MAP_RUNTIME_UPDATE_VA = 0x00409580
MAP_UPDATE_DISPATCH_VA = 0x0040ADA0
WORLD_ACTION_DISPATCH_VA = 0x0040CC00

# world action code13 / code14 写入的“事件次数上限”。十进制1500 = 十六进制0x5DC。
ENCOUNTER_MODIFIER_EVENT_LIMIT = 1500
# 运行时保存该上限与已消费事件次数的两个全局地址。
ENCOUNTER_MODIFIER_LIMIT_GLOBAL = 0x0046F61C
ENCOUNTER_MODIFIER_COUNTER_GLOBAL = 0x0046F620
# 模式位：固化6已经证明0/1会在后续随机阈值生成时分别走约 /3 与 ×2 分支。
ENCOUNTER_MODIFIER_MODE_GLOBAL = 0x0046846D
# 当前鼠标键状态的历史缓存；0x408A70只在新状态不同于这里时返回“新状态”。
LAST_MOUSE_STATE_GLOBAL = 0x0089F7E4
# 0x409580 的输入门控。这里不强行使用作者业务名称，只称“输入到遇敌桥接门控”。
INPUT_ENCOUNTER_GATE_GLOBAL = 0x00468BB9

# PE加载后的 IAT 地址。固化41要机器确认这里对应 USER32.GetKeyState。
GET_KEY_STATE_IAT_VA = 0x004601A8


class PEImage:
    """只实现本工具需要的 PE32 映射、导入表读取与 .text 扫描。"""

    def __init__(self, path: Path) -> None:
        # RPG.exe 只有几百KB，一次性读入内存最简单，也避免文件句柄生命周期干扰理解。
        self.path = path
        self.data = path.read_bytes()

        # Windows PE 必须以 DOS 'MZ' 头开头；不是则立即停止，防止后面把随机字节当地址表。
        if self.data[:2] != b"MZ":
            raise ValueError("输入文件没有 MZ 头，不能按 RPG.exe PE32 解析")

        # DOS头+0x3C保存 PE 头在文件中的位置 e_lfanew。
        self.pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]
        # PE头开头固定是四字节 'PE\\0\\0'。
        if self.data[self.pe_offset : self.pe_offset + 4] != b"PE\0\0":
            raise ValueError("PE 签名错误")

        # COFF FileHeader +6之前的+2字段是 section 数量。
        section_count = struct.unpack_from("<H", self.data, self.pe_offset + 6)[0]
        # COFF FileHeader +20位置的 WORD 是 OptionalHeader 字节长度。
        optional_size = struct.unpack_from("<H", self.data, self.pe_offset + 20)[0]
        # PE签名4字节 + COFF头20字节之后就是 OptionalHeader。
        self.optional_offset = self.pe_offset + 24
        # 当前研究对象是32位PE；PE32 OptionalHeader魔数应为0x10B。
        optional_magic = struct.unpack_from("<H", self.data, self.optional_offset)[0]
        if optional_magic != 0x10B:
            raise ValueError(f"只支持 PE32(0x10B)，当前 OptionalHeader magic=0x{optional_magic:04X}")

        # PE32 OptionalHeader +0x1C 是 ImageBase；RPG.exe正常加载基址为0x00400000。
        self.image_base = struct.unpack_from("<I", self.data, self.optional_offset + 0x1C)[0]
        # DataDirectory[IMPORT] 是第二项；PE32数据目录从 OptionalHeader+0x60 开始，
        # 所以 Import Directory RVA/Size 位于 +0x68/+0x6C。
        self.import_rva = struct.unpack_from("<I", self.data, self.optional_offset + 0x68)[0]
        self.import_size = struct.unpack_from("<I", self.data, self.optional_offset + 0x6C)[0]

        # section table 紧跟 OptionalHeader；每个 IMAGE_SECTION_HEADER 固定40字节。
        section_table = self.optional_offset + optional_size
        self.sections: list[dict[str, int | str]] = []
        for index in range(section_count):
            # 找到第index个section header。
            offset = section_table + index * 40
            # section名称最多8字节，遇NUL结束。
            name = self.data[offset : offset + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            # +8 / +12 / +16 / +20 分别是 VirtualSize、VirtualAddress、RawSize、RawOffset。
            virtual_size = struct.unpack_from("<I", self.data, offset + 8)[0]
            virtual_address = struct.unpack_from("<I", self.data, offset + 12)[0]
            raw_size = struct.unpack_from("<I", self.data, offset + 16)[0]
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

    def rva_to_offset(self, rva: int) -> int:
        """把相对ImageBase的RVA转换成磁盘文件偏移。"""

        for section in self.sections:
            # 当前section在内存中的起始RVA。
            start = int(section["virtual_address"])
            # VirtualSize和RawSize可能因PE对齐略有差异；取较大者覆盖有效边界。
            span = max(int(section["virtual_size"]), int(section["raw_size"]))
            if start <= rva < start + span:
                # 文件偏移 = section磁盘起点 + 当前RVA在section内的距离。
                return int(section["raw_offset"]) + (rva - start)
        raise ValueError(f"RVA 0x{rva:08X} 不在任何 PE section 中")

    def va_to_offset(self, va: int) -> int:
        """把运行时绝对VA转换成磁盘文件偏移。"""

        return self.rva_to_offset(va - self.image_base)

    def read_va(self, va: int, size: int) -> bytes:
        """从绝对VA读取固定长度机器码/数据。"""

        offset = self.va_to_offset(va)
        end = offset + size
        if end > len(self.data):
            raise ValueError(f"VA 0x{va:08X} 读取 {size} 字节会超出文件末尾")
        return self.data[offset:end]

    def read_c_string_rva(self, rva: int) -> str:
        """读取以NUL结束的ASCII导入DLL名/函数名。"""

        offset = self.rva_to_offset(rva)
        end = self.data.find(b"\0", offset)
        if end < 0:
            raise ValueError(f"RVA 0x{rva:08X} 指向的字符串没有NUL终止符")
        return self.data[offset:end].decode("ascii", errors="replace")

    def text_section_bytes(self) -> tuple[int, bytes]:
        """返回.text起始VA与磁盘中的实际代码字节。"""

        for section in self.sections:
            if section["name"] == ".text":
                start_va = self.image_base + int(section["virtual_address"])
                raw_offset = int(section["raw_offset"])
                raw_size = int(section["raw_size"])
                return start_va, self.data[raw_offset : raw_offset + raw_size]
        raise ValueError("PE中没有找到 .text section")

    def import_at_iat_va(self, iat_va: int) -> dict[str, Any] | None:
        """在PE导入表里查找某个IAT绝对VA对应的DLL与函数名。"""

        # 没有Import Directory则不可能解析IAT。
        if self.import_rva == 0:
            return None

        # IMAGE_IMPORT_DESCRIPTOR固定20字节，以全零descriptor结束。
        descriptor_offset = self.rva_to_offset(self.import_rva)
        while True:
            original_first_thunk, timestamp, forwarder_chain, name_rva, first_thunk = struct.unpack_from(
                "<IIIII", self.data, descriptor_offset
            )
            # 五个DWORD全0表示导入描述符数组结束。
            if (original_first_thunk | timestamp | forwarder_chain | name_rva | first_thunk) == 0:
                break

            # 当前DLL名称，例如 USER32.dll。
            dll_name = self.read_c_string_rva(name_rva)
            # OriginalFirstThunk通常指向名称表；若为0，规范允许直接使用FirstThunk。
            lookup_thunk = original_first_thunk or first_thunk
            thunk_index = 0
            while True:
                # PE32每个thunk是4字节。
                lookup_offset = self.rva_to_offset(lookup_thunk + thunk_index * 4)
                lookup_value = struct.unpack_from("<I", self.data, lookup_offset)[0]
                if lookup_value == 0:
                    break

                # 运行时IAT槽地址 = ImageBase + FirstThunk + index*4。
                current_iat_va = self.image_base + first_thunk + thunk_index * 4
                if current_iat_va == iat_va:
                    # 最高位1表示ordinal import；本目标GetKeyState是name import。
                    if lookup_value & 0x80000000:
                        return {
                            "IAT地址": f"0x{iat_va:08X}",
                            "DLL": dll_name,
                            "按序号导入": True,
                            "序号": lookup_value & 0xFFFF,
                            "函数": None,
                        }
                    # IMAGE_IMPORT_BY_NAME前2字节是Hint，后面才是NUL结尾函数名。
                    name_offset = self.rva_to_offset(lookup_value)
                    hint = struct.unpack_from("<H", self.data, name_offset)[0]
                    end = self.data.find(b"\0", name_offset + 2)
                    if end < 0:
                        raise ValueError("导入函数名没有NUL终止符")
                    function_name = self.data[name_offset + 2 : end].decode("ascii", errors="replace")
                    return {
                        "IAT地址": f"0x{iat_va:08X}",
                        "DLL": dll_name,
                        "按序号导入": False,
                        "Hint": hint,
                        "函数": function_name,
                    }
                thunk_index += 1

            # 移到下一个20字节descriptor。
            descriptor_offset += 20
        return None


def sha256_bytes(data: bytes) -> str:
    """计算bytes的SHA-256小写十六进制摘要。"""

    return hashlib.sha256(data).hexdigest()


def machine_check(pe: PEImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    """逐字节检查一段关键机器码是否与固化41证据一致。"""

    # expected_hex用空格分隔，便于人工和objdump/x64dbg对照。
    expected = bytes.fromhex(expected_hex)
    # 从EXE读取完全相同长度。
    actual = pe.read_va(va, len(expected))
    return {
        "地址": f"0x{va:08X}",
        "含义": meaning,
        "期望机器码": expected.hex(" "),
        "实际机器码": actual.hex(" "),
        "PASS": actual == expected,
    }


def direct_e8_callers(pe: PEImage, target_va: int) -> list[int]:
    """枚举.text里所有E8 rel32直接调用target_va的指令地址。"""

    text_va, code = pe.text_section_bytes()
    callers: list[int] = []
    # E8 call rel32固定5字节；最后4字节不足时不能再解析。
    for index in range(0, len(code) - 4):
        if code[index] != 0xE8:
            continue
        # rel32是有符号32位相对位移，必须按signed解释。
        rel = struct.unpack_from("<i", code, index + 1)[0]
        call_va = text_va + index
        # x86 E8目标 = 下一条指令地址(call_va+5) + rel32。
        if call_va + 5 + rel == target_va:
            callers.append(call_va)
    return callers


def build_checks(pe: PEImage) -> list[dict[str, Any]]:
    """建立固化41的关键机器码断言。"""

    checks: list[dict[str, Any]] = []

    # code13把持续上限写成1500，并把模式字节明确写0。
    checks.append(
        machine_check(
            pe,
            0x0040CF2D,
            "c7 05 1c f6 46 00 dc 05 00 00 c6 05 6d 84 46 00 00",
            "world action code13写0x46F61C=1500，并写0x46846D=0",
        )
    )
    # code14同样写1500；这里BL在0x40CC5A被固定为1，所以写入模式字节的是1。
    checks.append(
        machine_check(
            pe,
            0x0040CF60,
            "c7 05 1c f6 46 00 dc 05 00 00 88 1d 6d 84 46 00",
            "world action code14写0x46F61C=1500，并把BL(该函数中固定为1)写入0x46846D",
        )
    )
    # 用函数前段证明BL确实在该dispatch中初始化为1，避免只看mov [global],bl时猜寄存器值。
    checks.append(
        machine_check(
            pe,
            0x0040CC55,
            "a1 18 f8 89 00 bb 01 00 00 00",
            "world action dispatch在进入后续分派前把EBX/BL初始化为1",
        )
    )

    # 0x4034C0先在阈值激活时把参数加到遇敌meter；后半段才处理1500事件计数。
    checks.append(
        machine_check(
            pe,
            0x004034C0,
            "a1 14 f6 46 00 33 d2 3b c2 74 12 8b 44 24 04 8b 0d 18 f6 46 00 03 c8 89 0d 18 f6 46 00",
            "0x4034C0在阈值非0时把本次权重参数累加到0x46F618",
        )
    )
    # 这段是固化41最关键的“事件次数”证据：每次函数调用counter只+1，不读取时钟。
    checks.append(
        machine_check(
            pe,
            0x004034DD,
            "8b 0d 1c f6 46 00 3b ca 74 1b a1 20 f6 46 00 40 3b c1 a3 20 f6 46 00 7e 0c 89 15 1c f6 46 00 89 15 20 f6 46 00",
            "持续上限非0时counter每次0x4034C0调用只加1；counter>limit才同时清limit/counter",
        )
    )

    # 0x408C40只有第二参数非0才进入输入事件分支；第二参数0时直接走另一条路径。
    checks.append(
        machine_check(
            pe,
            0x00408C40,
            "56 8b 74 24 08 85 f6 74 0a c7 05 f8 f7 89 00 00 00 00 00",
            "0x408C40读取第二参数；为0则跳过非零输入事件分支",
        )
    )
    # profile高字节为1时给权重2；为0时给权重1；其它值不调用0x4034C0。
    checks.append(
        machine_check(
            pe,
            0x00408C99,
            "e8 72 ff ff ff 8b 40 18 33 c9 8a cc 8b c1 83 e8 00 74 0e 48 75 66 6a 02 e8 0a a8 ff ff 83 c4 04 c3 6a 01 e8 ff a7 ff ff",
            "0x408C40按当前profile高字节0/1分别向0x4034C0提交权重1/2",
        )
    )

    # 0x408A70先调用IAT 0x4601A8，实参2；这就是GetKeyState(VK_RBUTTON)。
    checks.append(
        machine_check(
            pe,
            0x00408A70,
            "56 57 8b 3d a8 01 46 00 6a 02 33 f6 ff d7 0f bf c0 a9 00 80 ff ff 6a 01",
            "鼠标状态编码器先查询键值2，再准备查询键值1",
        )
    )
    # 右键按下时，若左键也按下生成0x52；否则生成2。右键未按下而左键按下则生成1。
    checks.append(
        machine_check(
            pe,
            0x00408A86,
            "6a 01 74 17 ff d7 0f bf f0 81 e6 00 80 ff ff f7 de 1b f6 83 e6 50 83 c6 02 eb 12 ff d7 0f bf c8 f7 c1 00 80 ff ff 74 05 be 01 00 00 00",
            "GetKeyState高位被编码为鼠标状态0/1/2/0x52",
        )
    )
    # 与历史状态相同就返回0；不同则保存新状态并返回新状态本身。
    checks.append(
        machine_check(
            pe,
            0x00408AB3,
            "3b 35 e4 f7 89 00 74 0b 89 35 e4 f7 89 00 8b c6 5f 5e c3 5f 33 c0 5e c3",
            "鼠标状态变化时保存并返回新状态；未变化返回0，因此变化到0也向下游表现为0",
        )
    )

    # 输入启用分支先把edi(0x408A70结果)压栈，再经0x408830计算另一个参数，最后调用0x408C40。
    checks.append(
        machine_check(
            pe,
            0x004095AF,
            "a0 b9 8b 46 00 84 c0 74 1e 8b 4c 24 0c 8b 54 24 08 57 51 52 e8 68 f2 ff ff 83 c4 08 50 e8 6f f6 ff ff",
            "0x409580在0x468BB9非0时把0x408A70结果保留为0x408C40第二参数",
        )
    )
    # 门控关闭分支明确push 0 / push 0调用0x408C40，所以绝不会触发0x4034C0事件计数。
    checks.append(
        machine_check(
            pe,
            0x004095D6,
            "8b 44 24 0c 8b 4c 24 08 50 51 c7 05 f4 f7 89 00 00 00 00 00 e8 41 f2 ff ff 6a 00 6a 00 e8 48 f6 ff ff",
            "0x409580在输入门控关闭时向0x408C40明确传入两个0",
        )
    )

    # 0x40ADA0只对选中的Map runtime entry调用0x409580，其它entry走0x409510。
    checks.append(
        machine_check(
            pe,
            0x0040ADB4,
            "3b f8 75 09 03 cb e8 c1 e7 ff ff eb 07 03 cb e8 48 e7 ff ff",
            "Map runtime dispatch仅对selected entry调用0x409580",
        )
    )

    return checks


def build_report(pe: PEImage) -> dict[str, Any]:
    """把机器码、direct caller、导入表与推导结论汇总成一个JSON对象。"""

    checks = build_checks(pe)
    import_entry = pe.import_at_iat_va(GET_KEY_STATE_IAT_VA)

    # 对关键函数做全.text direct E8 caller枚举，防止漏掉另一条会改变计数单位的直接链。
    callers_4034c0 = direct_e8_callers(pe, WORLD_ENCOUNTER_UPDATE_VA)
    callers_408c40 = direct_e8_callers(pe, INPUT_TO_ENCOUNTER_BRIDGE_VA)
    callers_409580 = direct_e8_callers(pe, SELECTED_MAP_RUNTIME_UPDATE_VA)
    callers_40ada0 = direct_e8_callers(pe, MAP_UPDATE_DISPATCH_VA)

    # 固化41预期的唯一direct caller集合。若未来EXE出现额外caller，工具应报警而不是静默接受。
    expected_4034c0 = [0x00408CB1, 0x00408CBC]
    expected_408c40 = [0x004095CC, 0x004095F3]
    expected_409580 = [0x0040ADBA]
    expected_40ada0 = [0x0040B188]

    # Import表必须把0x4601A8解析成USER32.dll!GetKeyState；这比单看调用地址更硬。
    import_pass = bool(
        import_entry
        and str(import_entry.get("DLL", "")).lower() == "user32.dll"
        and import_entry.get("函数") == "GetKeyState"
    )

    # 所有静态检查共同决定总体PASS。
    all_machine_checks_pass = all(item["PASS"] for item in checks)
    caller_checks = {
        "0x4034C0_direct_E8_callers": [f"0x{x:08X}" for x in callers_4034c0],
        "0x4034C0_期望": [f"0x{x:08X}" for x in expected_4034c0],
        "0x4034C0_PASS": callers_4034c0 == expected_4034c0,
        "0x408C40_direct_E8_callers": [f"0x{x:08X}" for x in callers_408c40],
        "0x408C40_期望": [f"0x{x:08X}" for x in expected_408c40],
        "0x408C40_PASS": callers_408c40 == expected_408c40,
        "0x409580_direct_E8_callers": [f"0x{x:08X}" for x in callers_409580],
        "0x409580_期望": [f"0x{x:08X}" for x in expected_409580],
        "0x409580_PASS": callers_409580 == expected_409580,
        "0x40ADA0_direct_E8_callers": [f"0x{x:08X}" for x in callers_40ada0],
        "0x40ADA0_期望": [f"0x{x:08X}" for x in expected_40ada0],
        "0x40ADA0_PASS": callers_40ada0 == expected_40ada0,
    }
    all_caller_checks_pass = all(value for key, value in caller_checks.items() if key.endswith("_PASS"))

    # 用公式明确记录原版off-by-one，避免兼容引擎把“1500”做成错误的倒计时器。
    first_clear_invocation = ENCOUNTER_MODIFIER_EVENT_LIMIT + 1

    return {
        "工具": "幽城世界遇敌持续状态事件计数静态调查器",
        "工具版本": TOOL_VERSION,
        "输入RPG": {
            "路径": str(pe.path),
            "文件大小": len(pe.data),
            "SHA256": sha256_bytes(pe.data),
            "是否当前RPG.exe.org样本": sha256_bytes(pe.data) == CURRENT_RPG_ORG_SHA256,
            "ImageBase": f"0x{pe.image_base:08X}",
        },
        "固化41核心常量": {
            "持续上限全局": f"0x{ENCOUNTER_MODIFIER_LIMIT_GLOBAL:08X}",
            "事件计数器全局": f"0x{ENCOUNTER_MODIFIER_COUNTER_GLOBAL:08X}",
            "模式字节全局": f"0x{ENCOUNTER_MODIFIER_MODE_GLOBAL:08X}",
            "鼠标历史状态全局": f"0x{LAST_MOUSE_STATE_GLOBAL:08X}",
            "输入桥接门控全局": f"0x{INPUT_ENCOUNTER_GATE_GLOBAL:08X}",
            "写入上限": ENCOUNTER_MODIFIER_EVENT_LIMIT,
            "首次清除发生在第几次合格0x4034C0调用": first_clear_invocation,
        },
        "IAT导入证明": {
            "查询IAT地址": f"0x{GET_KEY_STATE_IAT_VA:08X}",
            "解析结果": import_entry,
            "期望": "USER32.dll!GetKeyState",
            "PASS": import_pass,
            "键值解释": {
                "1": "VK_LBUTTON",
                "2": "VK_RBUTTON",
            },
        },
        "机器码断言": checks,
        "机器码断言汇总": {
            "通过": sum(1 for item in checks if item["PASS"]),
            "总数": len(checks),
            "PASS": all_machine_checks_pass,
        },
        "direct_E8_caller枚举": caller_checks,
        "固化41实现结论": [
            "world action code13与code14都把0x46F61C写成1500；该值不是Legacy Tick倒计时。",
            "0x46F620只在0x4034C0每次实际调用时加1；函数内部没有读取系统时钟、Legacy Tick计数或毫秒值。",
            "0x4034C0在当前EXE只有0x408CB1与0x408CBC两个direct E8 caller，分别提交遇敌权重2与1。",
            "0x408A70通过USER32.GetKeyState读取VK_RBUTTON/VK_LBUTTON，编码当前鼠标键状态为0/1/2/0x52。",
            "0x408A70只在状态变化时保存并返回新状态；状态不变返回0；释放到0虽然发生变化，但返回值仍是0。",
            "因此只有变化到非0鼠标键状态，并通过0x468BB9门控且当前profile映射为权重1/2时，才会消费一次持续事件计数。",
            "limit=1500时，第1..1500次合格调用不会清除；第1501次合格调用把limit与counter同时清0。",
            "兼容引擎不得把1500实现为固定75秒、1500个50ms tick或1500毫秒；真实墙钟持续时间取决于玩家输入事件与门控。",
        ],
        "证据边界": [
            "本固化闭合的是计数域与调用门控，不强命名原作者变量名。",
            "0x468BB9的完整作者业务名称仍不强命名；这里只证明它控制0x409580是否把真实鼠标状态送入0x408C40。",
            "固化38的20Hz Legacy Clock仍成立，但这里只作为0x409580被周期调度的背景；1500本身并不按每个Legacy Tick自动消费。",
        ],
        "总体PASS": all_machine_checks_pass and all_caller_checks_pass and import_pass,
    }


def parse_args() -> argparse.Namespace:
    """定义并解析命令行参数。"""

    parser = argparse.ArgumentParser(
        description="重放固化41：world encounter 1500持续值的鼠标输入事件计数边界。"
    )
    # --rpg是唯一必填输入，既可以传RPG.exe，也可以传解包后的RPG.exe.org副本。
    parser.add_argument("--rpg", required=True, type=Path, help="RPG.exe路径")
    # --output可选；不传时只打印JSON到标准输出，工具不会落盘。
    parser.add_argument("--output", type=Path, help="可选：把结果写入UTF-8 JSON文件")
    return parser.parse_args()


def main() -> int:
    """程序入口：读取EXE、生成报告、可选写文件，并按PASS状态返回退出码。"""

    args = parse_args()
    # 先检查文件是否存在，给初学者一个明确错误，而不是让read_bytes抛出难懂堆栈。
    if not args.rpg.is_file():
        print(f"错误：找不到RPG.exe：{args.rpg}", file=sys.stderr)
        return 2

    try:
        # PEImage构造阶段同时验证MZ/PE32结构。
        pe = PEImage(args.rpg)
        # build_report执行全部机器码、caller和导入表检查。
        report = build_report(pe)
    except (OSError, ValueError, struct.error) as exc:
        print(f"错误：调查失败：{exc}", file=sys.stderr)
        return 2

    # ensure_ascii=False保证简体中文直接写入JSON，不变成\\uXXXX；indent=2便于人工审阅。
    text = json.dumps(report, ensure_ascii=False, indent=2) + "\n"
    if args.output is not None:
        # 若父目录不存在则创建，方便独立固化包直接输出到“数据/固化41”。
        args.output.parent.mkdir(parents=True, exist_ok=True)
        # 统一UTF-8与LF；这是数据文件，不是Windows批处理文件。
        args.output.write_text(text, encoding="utf-8", newline="\n")

    # 标准输出始终打印一份完整结果，用户即使不写文件也能立即检查。
    print(text, end="")
    # 证据全通过返回0；任意机器码/caller/import断言失败返回1，便于批处理自动阻止封包。
    return 0 if report["总体PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
