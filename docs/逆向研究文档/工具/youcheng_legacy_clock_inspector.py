#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Legacy Clock / 主循环单步调度静态调查器。

本工具用于重放“固化38”已经确认的时间链机器证据。它只读取 RPG.exe 与
DDDES.SCI，不会修改任何游戏文件，也不会注入进程。

固化38关心的不是“屏幕看起来多少FPS”，而是原版游戏逻辑自己的旧式时间单位：

1. DDDES.SCI +0x11 当前真实值为 20；
2. RPG.exe 内部固定时间基数为 100；
3. 原版用 floor(100 / 20) = 5 个基础多媒体定时回调作为一个逻辑触发阈值；
4. 原版意图的基础间隔来自 1000 / 100 = 10 ms；
5. 在原版预期的 1 ms WinMM 最小周期环境下，一个 Legacy Tick 名义上是
   5 * 10 ms = 50 ms，也就是 20 Hz；
6. WM_TIMER 只变成一个布尔 pending 标志，主循环每次最多消费一个逻辑步，
   不会根据“错过了多少毫秒”无限补算旧逻辑步；
7. EVE WAIT、标准场景实体 SF2 Section0 +0x22 countdown，以及 Battle 主对象
   DataCenter+0xF4 递减，都能静态挂到同一个注册对象管理器单步链。

重要边界：
- WinMM 的实际回调时间会受系统 TIMECAPS、调度抖动和线程阻塞影响，所以工具把
  50 ms / 20 Hz 称为“名义 Legacy Clock / 兼容目标”，而不是承诺现实世界每次
  回调都精确 50.000 ms。
- timeSetEvent 的机器码会把 (1000/base)/wPeriodMin 作为 uDelay，因此本工具会
  同时报告这个原版公式，不把 wPeriodMin 未知时的物理回调周期伪装成已知常数。
- SF2 +0x22 是 countdown 的单位。原播放器存在装载/切换时的状态机边界，兼容
  引擎应复现 countdown，而不是简单把整条记录粗暴改写成“持续 N*50 ms”。
"""

from __future__ import annotations

# argparse 用来定义清楚的命令行参数。用户只要执行 --help 就能知道怎么用工具。
import argparse
# hashlib 用于计算文件 SHA-256，确保研究时拿到的确实是同一份资源/EXE。
import hashlib
# json 用来输出机器可读回归结果，后续固化节点可以直接比较而不必人工抄数字。
import json
# struct 用来读取 PE 头和 little-endian 整数；原游戏是 32 位 little-endian PE。
import struct
# sys 只负责返回标准退出码；0 表示所有断言通过，1 表示至少一项失败。
import sys
# pathlib.Path 让 Windows/Linux 路径处理更直观，也避免手工拼接斜杠。
from pathlib import Path
# Any 只用于类型提示，让下面较大的 JSON 字典更容易阅读。
from typing import Any


# 工具版本必须和固化节点绑定；后续如果修改判断逻辑，应同步提升版本号。
TOOL_VERSION = "0.1.0-固化38"

# 当前从 multimedia/Sys/Sys.dat 直接提取出的 DDDES.SCI 已经有稳定 SHA-256。
# 把它写在这里不是为了“只允许这一份文件”，而是为了给当前正式样本一个身份锚点。
CURRENT_DDDES_SHA256 = "1c8e3a6e0dfc314bcdc34aeae57388539da95d6443ee0441032452a7d76b2972"

# DDDES 固定头长度是 18 字节；后面每条旧显示记录是 52 字节。
DDDES_HEADER_SIZE = 0x12
DDDES_RECORD_SIZE = 0x34
# 固化23已经证明 +0x11 是传给多媒体计时初始化的 u8 配置值。
DDDES_LOGIC_RATE_OFFSET = 0x11

# 原版 .data 里的基础定时常量地址。当前值为 DWORD 100。
LEGACY_BASE_VA = 0x0046850C
# 原版先算 1000/base 得到基础间隔整数毫秒。
MILLISECONDS_PER_SECOND = 1000


class PEImage:
    """实现本工具需要的最小 PE32 虚拟地址到文件偏移映射。"""

    def __init__(self, path: Path) -> None:
        # 一次性读取整个 RPG.exe。文件只有几百 KB，这样代码最简单，也没有性能问题。
        self.path = path
        self.data = path.read_bytes()

        # DOS 头必须以 ASCII “MZ” 开始；否则传入的就不是本工具要检查的 Windows PE。
        if self.data[:2] != b"MZ":
            raise ValueError("输入文件没有 MZ 头，不能按 RPG.exe PE32 解析")

        # DOS 头 +0x3C 的 e_lfanew 指向真正的 PE 头。
        pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]
        # PE 头固定签名是四字节 PE\0\0。这里先检查，避免后面在错误偏移乱读。
        if self.data[pe_offset : pe_offset + 4] != b"PE\0\0":
            raise ValueError("PE 签名错误")

        # COFF FileHeader +2 是 section 数量。
        section_count = struct.unpack_from("<H", self.data, pe_offset + 6)[0]
        # COFF FileHeader +16 是 OptionalHeader 大小。
        optional_size = struct.unpack_from("<H", self.data, pe_offset + 20)[0]
        # OptionalHeader 在 4 字节 PE 签名 + 20 字节 FileHeader 后开始。
        optional_offset = pe_offset + 24
        # 当前 RPG.exe 是 PE32；OptionalHeader +0x1C 是 32 位 ImageBase。
        self.image_base = struct.unpack_from("<I", self.data, optional_offset + 0x1C)[0]
        # section table 紧跟在 OptionalHeader 后面，每项固定 40 字节。
        section_table = optional_offset + optional_size

        # 用列表保存每个 section 的虚拟范围和磁盘 raw 范围，后面 read_va 会用它们换算。
        self.sections: list[dict[str, int | str]] = []
        for index in range(section_count):
            # 找到第 index 个 section header。
            offset = section_table + index * 40
            # 前 8 字节是 section 名称，遇到 NUL 就结束。
            name = self.data[offset : offset + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            # +8 VirtualSize，表示装入内存后的有效大小。
            virtual_size = struct.unpack_from("<I", self.data, offset + 8)[0]
            # +12 VirtualAddress 是相对 ImageBase 的 RVA。
            virtual_address = struct.unpack_from("<I", self.data, offset + 12)[0]
            # +16 SizeOfRawData 是磁盘文件中 section 占用的字节数。
            raw_size = struct.unpack_from("<I", self.data, offset + 16)[0]
            # +20 PointerToRawData 是磁盘文件偏移。
            raw_offset = struct.unpack_from("<I", self.data, offset + 20)[0]
            # 保存所有映射信息。取 max(VirtualSize, RawSize) 可以覆盖常见的尾部对齐差异。
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
        """把绝对虚拟地址 VA 转成 RPG.exe 文件偏移。"""

        # PE section 表保存的是 RVA，所以先减 ImageBase。
        rva = va - self.image_base
        for section in self.sections:
            # section 的虚拟起点。
            start = int(section["virtual_address"])
            # 取虚拟大小与磁盘大小较大者，避免对齐尾部地址被误判成不在 section 中。
            span = max(int(section["virtual_size"]), int(section["raw_size"]))
            # 当前 RVA 落在这个 section 中时，就能进行线性换算。
            if start <= rva < start + span:
                # “磁盘 section 起点 + section 内相对偏移”就是最终文件偏移。
                return int(section["raw_offset"]) + (rva - start)
        # 没有任何 section 覆盖时必须拒绝，不能返回一个猜出来的文件偏移。
        raise ValueError(f"VA 0x{va:08X} 不在任何 PE section 中")

    def read_va(self, va: int, size: int) -> bytes:
        """从绝对虚拟地址读取固定长度机器字节。"""

        # 先把 VA 映射到文件偏移。
        offset = self.va_to_offset(va)
        # 计算读取结束位置。
        end = offset + size
        # 如果超出文件末尾，说明地址/长度或 PE 映射有问题，必须明确报错。
        if end > len(self.data):
            raise ValueError(f"VA 0x{va:08X} 读取 {size} 字节会超出文件末尾")
        # 返回不可变 bytes，供机器码断言精确比较。
        return self.data[offset:end]

    def text_section_bytes(self) -> tuple[int, bytes]:
        """返回 .text 的起始 VA 和磁盘字节，用于扫描 direct E8 caller。"""

        for section in self.sections:
            # 只寻找名字正好等于 .text 的代码 section。
            if section["name"] == ".text":
                # .text 起始绝对 VA = ImageBase + section RVA。
                start_va = self.image_base + int(section["virtual_address"])
                # 扫描 direct E8 只需要磁盘实际存在的 raw 字节。
                raw_offset = int(section["raw_offset"])
                raw_size = int(section["raw_size"])
                return start_va, self.data[raw_offset : raw_offset + raw_size]
        # 当前 RPG.exe 必须有 .text；找不到说明输入并非研究对象或 PE 异常。
        raise ValueError("PE 中没有找到 .text section")


def sha256_bytes(data: bytes) -> str:
    """计算一段 bytes 的 SHA-256 十六进制字符串。"""

    # hexdigest() 直接返回常见的 64 字符小写十六进制摘要。
    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """计算一个文件的 SHA-256。"""

    # 文件尺寸很小，直接一次读完再复用上面的函数最清楚。
    return sha256_bytes(path.read_bytes())


def parse_dddes(path: Path) -> dict[str, Any]:
    """读取 DDDES.SCI，并只解析固化38需要的结构字段。"""

    # 读取原始文件；调查器不对它做任何写操作。
    raw = path.read_bytes()
    # 至少要有 18 字节固定头，否则连 +0x11 计时字段都不存在。
    if len(raw) < DDDES_HEADER_SIZE:
        raise ValueError(f"DDDES.SCI 只有 {len(raw)} 字节，短于 18 字节固定头")
    # 固化23已经证明后半部分按 52 字节记录分组；余数非0说明不是当前已知格式。
    tail_size = len(raw) - DDDES_HEADER_SIZE
    if tail_size % DDDES_RECORD_SIZE != 0:
        raise ValueError(
            f"DDDES.SCI 在18字节头后剩 {tail_size} 字节，不能整除52字节记录大小"
        )

    # +0x11 是原版以 BYTE 读取并传给 MMTimer 初始化器的逻辑频率参数。
    logic_rate = raw[DDDES_LOGIC_RATE_OFFSET]
    # 0 会让原版 0x404A85 的 div esi 发生除零，所以这是结构上必须明确报告的危险值。
    if logic_rate == 0:
        raise ValueError("DDDES.SCI +0x11 为0；原版会在 floor(100/value) 处除零")

    # 返回当前格式身份、SHA和关键值；工具不猜 52 字节 record 的作者业务名。
    return {
        "路径": str(path),
        "文件大小": len(raw),
        "SHA256": sha256_bytes(raw),
        "是否当前已知正式样本SHA": sha256_bytes(raw) == CURRENT_DDDES_SHA256,
        "固定头字节数": DDDES_HEADER_SIZE,
        "52字节记录数": tail_size // DDDES_RECORD_SIZE,
        "逻辑频率参数_u8_加0x11": logic_rate,
    }


def machine_check(pe: PEImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    """对某段关键机器码做逐字节断言。"""

    # fromhex() 把便于文档复制的“aa bb cc”十六进制文本变成真正 bytes。
    expected = bytes.fromhex(expected_hex)
    # 从 PE 的指定虚拟地址读出同样长度的实际机器字节。
    actual = pe.read_va(va, len(expected))
    # 机器证据必须逐字节相同才算 PASS，不能只看“附近像是这个函数”。
    passed = actual == expected
    # 把 expected/actual 都保存在 JSON 中，后续不同 EXE 版本出现差异时能立即定位。
    return {
        "地址": f"0x{va:08X}",
        "含义": meaning,
        "期望机器码": expected.hex(" "),
        "实际机器码": actual.hex(" "),
        "PASS": passed,
    }


def direct_e8_callers(pe: PEImage, target_va: int) -> list[int]:
    """枚举 .text 中所有直接 E8 rel32 调到 target_va 的 caller 地址。"""

    # 取得 .text 的起始 VA 和原始代码字节。
    text_va, code = pe.text_section_bytes()
    # 用列表保存每个 call 指令本身的绝对 VA。
    callers: list[int] = []
    # E8 rel32 一共5字节，所以最后4字节不能再作为完整候选起点。
    for index in range(0, len(code) - 4):
        # direct near call 的 opcode 第一个字节必须是 0xE8。
        if code[index] != 0xE8:
            continue
        # 后四字节是有符号 little-endian 相对位移。
        rel = struct.unpack_from("<i", code, index + 1)[0]
        # CPU 的目标 = “下一条指令地址” + rel32。
        caller_va = text_va + index
        calculated_target = caller_va + 5 + rel
        # 只记录恰好落到目标函数入口的 direct call。
        if calculated_target == target_va:
            callers.append(caller_va)
    return callers


def build_machine_checks(pe: PEImage) -> list[dict[str, Any]]:
    """建立固化38必须通过的机器码断言集合。"""

    # 每条断言都尽量只覆盖语义所需的最小连续字节，减少无关编译差异造成误伤。
    specs = [
        (
            0x00401584,
            "8b 4e 08 8b 06 33 d2 8a 11 8b 0d b4 f6 46 00 52 50 e8 a6 37 00 00",
            "DDDES对象从保存的header+0x11指针读取u8配置并调用0x404D40计时包装器",
        ),
        (
            0x00404A75,
            "8b 3d 0c 85 46 00 33 d2 8b c7 89 35 a0 f6 46 00 f7 f6 33 d2 a3 90 f6 46 00 b8 e8 03 00 00 f7 f7 a3 b8 f6 46 00",
            "0x404A60读取base=100，计算floor(base/config)阈值和floor(1000/base)基础毫秒",
        ),
        (
            0x00404ABD,
            "8d 44 24 08 6a 08 50 88 1d 94 f6 46 00 89 1d a8 f6 46 00 89 1d 9c f6 46 00 89 1d a4 f6 46 00 89 1d 98 f6 46 00 ff 15 18 02 46 00",
            "0x404AB0清计时状态并通过IAT调用timeGetDevCaps",
        ),
        (
            0x00404AE8,
            "8b 4c 24 08 51 ff 15 14 02 46 00 8b 4c 24 08 a1 b8 f6 46 00 33 d2 6a 01 f7 f1 53 68 90 4c 40 00 51 50 ff 15 10 02 46 00",
            "取wPeriodMin、timeBeginPeriod，并以TIME_PERIODIC创建回调0x404C90",
        ),
        (
            0x00404C00,
            "8b 44 24 08 85 c0 0f 95 c0 a2 94 f6 46 00",
            "timer control selector0把第二参数压成布尔值写入0x46F694暂停/待重臂标志",
        ),
        (
            0x00404CDD,
            "8b 0d b0 f6 46 00 a1 90 f6 46 00 3b c8 72 3b a0 94 f6 46 00 84 c0 75 32",
            "多媒体回调达到divider阈值后还要检查0x46F694，已挂起时不重复投递",
        ),
        (
            0x00404CF5,
            "8b 15 ac f6 46 00 6a 00 6a 00 68 13 01 00 00 52 ff 15 90 01 46 00",
            "阈值满足时向主窗口PostMessageA(WM_TIMER=0x0113)",
        ),
        (
            0x00404D10,
            "c6 05 94 f6 46 00 01 40 c7 05 b0 f6 46 00 00 00 00",
            "投递后把0x46F694置1并清divider计数，形成单个待处理令牌",
        ),
        (
            0x00401B31,
            "8b c8 81 e9 11 01 00 00 74 4b 83 e9 02 75 19 c6 05 8d f3 46 00 01",
            "窗口过程识别WM_TIMER=0x113后只把0x46F38D这个BYTE pending标志置1",
        ),
        (
            0x00401707,
            "38 1d 8d f3 46 00 0f 84 89 00 00 00 8b 0d f4 24 8e 00 89 5d fc e8 4f 92 04 00",
            "主循环看到pending BYTE后只调用一次0x44A970游戏逻辑总更新",
        ),
        (
            0x00401721,
            "8b 0d b4 f6 46 00 53 53 e8 c2 34 00 00 88 1d 8d f3 46 00",
            "一次逻辑更新后调用timer control(0,0)重臂，并把pending BYTE清0",
        ),
        (
            0x0044A9B1,
            "8b ce e8 78 9e fe ff 8b ce e8 31 9c fe ff e8 dc 76 fb ff 8b ce e8 45 9d fe ff",
            "0x44A970的单步主链每次只调用一次注册对象更新0x4345F0",
        ),
        (
            0x004345F0,
            "8b 15 d4 40 8c 00 56 33 f6 85 d2 7e 29 57 bf c4 e6 8d 00 8b 0f 85 c9 74 14 8a 81 14 02 00 00 84 c0 74 0a 8b 01 ff 10",
            "注册对象管理器第一遍对active对象至多执行一次vtable[0]",
        ),
        (
            0x0043462C,
            "b9 c4 e6 8d 00 8b 01 85 c0 74 0d c6 80 14 02 00 00 01",
            "管理器第二遍把已注册对象+0x214 active BYTE置1，下一逻辑步可执行",
        ),
        (
            0x0040AE8A,
            "c7 06 98 02 46 00",
            "EVE主对象构造时vtable设为0x460298",
        ),
        (
            0x00460298,
            "50 b1 40 00",
            "EVE vtable[0]直接指向0x40B150",
        ),
        (
            0x0040AF5F,
            "6a 00 68 b8 0b 00 00 56 e8 c4 94 02 00",
            "EVE对象通过0x434430注册进同一个全局对象管理器",
        ),
        (
            0x0040B150,
            "56 8b f1 e8 78 01 00 00",
            "EVE vtable[0]=0x40B150每次先调用解释器0x40B2D0",
        ),
        (
            0x0040B178,
            "8a 86 19 02 00 00 84 c0 75 0b 8b 8e 80 02 00 00 e8 13 fc ff ff",
            "EVE正常活动分支同一逻辑步调用场景实体更新0x40ADA0",
        ),
        (
            0x0040C000,
            "8b c1 33 d2 8b 0d 34 f8 89 00 56 3b ca 75 25",
            "WAIT handler读取全局计数器并区分首次进入/后续逻辑步",
        ),
        (
            0x0040C034,
            "3b 0d 2c f8 89 00 74 10 41 c6 05 1d f8 89 00 01 89 0d 34 f8 89 00",
            "WAIT未达到目标时每次只加1并设置yield标志",
        ),
        (
            0x0040A4E6,
            "8b 5e 04 50 0f bf 47 6c 03 c3 50 52 55 e8 d8 ce ff ff",
            "标准场景实体动作更新在同一场景逻辑步调用0x4073D0播放器包装器",
        ),
        (
            0x00407400,
            "8b 41 18 8b 54 24 04 85 c0 56 74 11 39 51 10 75 0c 48 c6 41 2d 00 89 41 18 5e c2 0c 00",
            "SF2播放器已有countdown且动作未变时每次调用只减1然后返回",
        ),
        (
            0x00407478,
            "8b 51 14 8b 41 38 33 f6 8b 14 90 8b 41 34 66 8b 74 02 22 c6 41 2d 01 89 71 18",
            "SF2推进新记录时从Section0 record+0x22读取u16并写入countdown",
        ),
        (
            0x004610D4,
            "20 29 44 00",
            "Battle最终vtable[0]直接指向0x442920",
        ),
        (
            0x0043D6AE,
            "8b 96 1c 02 00 00 6a 00 52 56 8b ce e8 71 6d ff ff",
            "Battle对象初始化链通过0x434430注册进同一个全局对象管理器",
        ),
        (
            0x00442920,
            "a0 0c 24 8e 00 57 84 c0 8b f9 0f 84 af 00 00 00 8b 0d 48 1c 8e 00",
            "Battle vtable[0]先检查battle active gate并取得DataCenter",
        ),
        (
            0x0044293D,
            "8b 81 f4 00 00 00 3b c3 7e 11 8a 15 dc 96 46 00 84 d2 74 07 48 89 81 f4 00 00 00",
            "Battle同一管理器逻辑步在progression gate允许且+0xF4>1时只减1",
        ),
        (
            0x0046850C,
            "64 00 00 00",
            "Legacy timer固定基础常量为DWORD 100",
        ),
    ]

    # 逐条执行 machine_check，最终结果列表可以直接统计 PASS 数。
    return [machine_check(pe, va, expected, meaning) for va, expected, meaning in specs]


def analyze(exe_path: Path, dddes_path: Path) -> dict[str, Any]:
    """综合 DDDES、PE机器码、caller和时间公式生成固化38报告。"""

    # 先解析当前真实 DDDES.SCI，拿到 +0x11 配置值。
    dddes = parse_dddes(dddes_path)
    # 再建立 PE 映射，后续所有地址都按当前输入 EXE 实际字节验证。
    pe = PEImage(exe_path)

    # RPG.exe 的基础时间常量必须直接从当前输入文件读取，不能只相信源码中的常量备注。
    base = struct.unpack("<I", pe.read_va(LEGACY_BASE_VA, 4))[0]
    # +0x11 是正整数；原版使用无符号 div，计算 floor(base / rate)。
    rate = int(dddes["逻辑频率参数_u8_加0x11"])
    divider = base // rate
    # base 如果大于1000会导致整数除法为0；当前样本不会，但工具仍明确检查。
    if base == 0:
        raise ValueError("RPG.exe Legacy base 为0，无法计算时间公式")
    base_interval_ms = MILLISECONDS_PER_SECOND // base
    # 如果 divider 为0，原版逻辑阈值会退化；当前20不会发生。
    if divider == 0:
        raise ValueError(f"floor({base}/{rate})=0，超出当前固化38可解释范围")

    # 固化38的名义 Legacy Tick 按“原版预期 wPeriodMin=1ms”计算。
    nominal_tick_ms = divider * base_interval_ms
    # 转成 Hz。这里使用浮点只为人类阅读，真正兼容实现可保持整数50ms步长。
    nominal_tick_hz = MILLISECONDS_PER_SECOND / nominal_tick_ms

    # 重放全部关键机器码断言。
    checks = build_machine_checks(pe)
    # 统计通过数量，任何一项失败都不应把该EXE直接当成固化38同一机器语义基线。
    passed_count = sum(1 for item in checks if item["PASS"])

    # direct E8 caller 扫描用来证明0x44A970不是在多个地方被额外驱动。
    update_callers = direct_e8_callers(pe, 0x0044A970)
    # 同样记录场景更新0x40ADA0的direct caller，方便复核EVE到场景实体更新的接线。
    scene_update_callers = direct_e8_callers(pe, 0x0040ADA0)

    # Battle特殊计数器在旧节点已证明初值范围是RNG%400+800，即800..1199。
    battle_min = 800
    battle_max = 1199

    # 返回一个自描述JSON对象。这里把“机器事实”和“兼容解释”分开，防止后人混淆。
    return {
        "工具版本": TOOL_VERSION,
        "输入": {
            "RPG.exe": str(exe_path),
            "RPG.exe_SHA256": sha256_file(exe_path),
            "DDDES.SCI": dddes,
        },
        "LegacyClock机器公式": {
            "固定基础常量_B": base,
            "DDDES逻辑频率参数_R": rate,
            "divider_floor_B_div_R": divider,
            "基础间隔_floor_1000_div_B_ms": base_interval_ms,
            "WinMM_wPeriodMin记号": "p毫秒；运行时由timeGetDevCaps取得",
            "原版timeSetEvent_uDelay公式_ms": f"floor({base_interval_ms}/p)",
            "原版timeSetEvent_uResolution": "p",
            "逻辑消息阈值": f"每{divider}个基础回调尝试投递一次WM_TIMER；pending期间不重复投递",
            "在p等于1ms时名义LegacyTick_ms": nominal_tick_ms,
            "在p等于1ms时名义LegacyTick_Hz": nominal_tick_hz,
            "当前兼容目标": "50ms固定Legacy逻辑步 / 20Hz名义逻辑时钟；渲染时钟独立",
        },
        "单步调度语义": {
            "WM_TIMER": "0x0113",
            "窗口pending标志": "BYTE [0x46F38D]，只表示有/无，不保存积压次数",
            "多媒体timer抑制标志": "BYTE [0x46F694]，一次PostMessage后置1，主循环完成一次update后清0",
            "主逻辑总更新": "0x44A970",
            "注册对象单步管理器": "0x4345F0",
            "补帧结论": "不存在按错过的墙钟时间循环执行N次0x44A970的已知路径；语义是单pending令牌/合并，而不是无限追赶",
        },
        "同一LegacyTick消费链": {
            "EVE": "0x4345F0 -> EVE vtable[0] 0x40B150 -> 0x40B2D0；WAIT未完成时每逻辑步只把计数器+1",
            "SF2": "0x40B150 -> 0x40ADA0 -> entity updater -> 0x40A2B0 -> 0x4073D0 -> 0x407400；Section0 +0x22 countdown每次只减1",
            "Battle": "Battle注册到0x4345F0；vtable[0]=0x442920；gate允许且DataCenter+0xF4>1时每逻辑步只减1",
        },
        "现实时间换算边界": {
            "一个LegacyTick单位名义毫秒": nominal_tick_ms,
            "WAIT参数单位": "1单位 = 1个Legacy Tick；当前名义50ms",
            "SF2_加0x22": "1个countdown单位 = 1个Legacy Tick；当前名义50ms。实际记录切换仍须复现0x407400状态机，不建议只乘毫秒后丢掉离散状态。",
            "Battle_加0xF4初值范围": [battle_min, battle_max],
            "Battle_存储计数单位对应名义秒范围": [battle_min * nominal_tick_ms / 1000.0, battle_max * nominal_tick_ms / 1000.0],
            "Battle范围注意": "这里只换算计数单位尺度；实际终止时刻还受active/progression gate、>1停止递减和detector检查时机影响，不能把该范围直接当绝对墙钟倒计时。",
            "墙钟精度注意": "WinMM实际回调受TIMECAPS与调度影响；50ms/20Hz是当前配置的名义逻辑协议与兼容实现目标，不是实时系统精度保证。",
        },
        "direct_E8_callers": {
            "0x44A970": [f"0x{x:08X}" for x in update_callers],
            "0x40ADA0": [f"0x{x:08X}" for x in scene_update_callers],
        },
        "机器断言": checks,
        "机器断言通过": passed_count,
        "机器断言总数": len(checks),
        "全部机器断言PASS": passed_count == len(checks),
        "固化38关键结论PASS": (
            passed_count == len(checks)
            and base == 100
            and rate == 20
            and divider == 5
            and base_interval_ms == 10
            and nominal_tick_ms == 50
            and update_callers == [0x0040171C]
            and 0x0040B188 in scene_update_callers
        ),
    }


def write_json(path: Path, report: dict[str, Any]) -> None:
    """把报告以UTF-8、缩进JSON写到指定位置。"""

    # ensure_ascii=False 让中文直接可读；indent=2 方便人工diff和长期接档。
    text = json.dumps(report, ensure_ascii=False, indent=2) + "\n"
    # 明确使用UTF-8，避免Windows默认代码页把中文报告写坏。
    path.write_text(text, encoding="utf-8")


def main() -> int:
    """命令行入口。"""

    # 创建主解析器，并给 --help 一个能直接看懂的中文说明。
    parser = argparse.ArgumentParser(description="《幽城幻剑录》Legacy Clock / 主循环单步调度静态调查器")
    # --version/--版本都指向同一个工具版本，便于中文和英文终端习惯。
    parser.add_argument("--version", "--版本", action="version", version=TOOL_VERSION)
    # RPG.exe 是第一个必需参数；工具只读它的PE结构和机器码。
    parser.add_argument("RPG.exe", type=Path, help="要验证的32位RPG.exe")
    # DDDES.SCI 是第二个必需参数；建议直接使用Sys.dat提取出的真实文件。
    parser.add_argument("DDDES.SCI", type=Path, help="从Sys.dat提取的DDDES.SCI")
    # --json 可选；不提供时只在终端打印摘要，不制造额外文件。
    parser.add_argument("--json", type=Path, default=None, help="可选：把完整机器可读报告写到这个JSON文件")
    # 真正解析用户命令行。
    args = parser.parse_args()

    try:
        # 执行全部只读分析。
        report = analyze(getattr(args, "RPG.exe"), getattr(args, "DDDES.SCI"))
    except Exception as exc:
        # 调查工具遇到格式/文件错误时输出简洁中文原因，并返回1方便BAT/CI识别失败。
        print(f"[失败] {exc}", file=sys.stderr)
        return 1

    # 如果用户要求保存JSON，就先确保父目录存在，再写UTF-8报告。
    if args.json is not None:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        write_json(args.json, report)

    # 下面只打印最重要的摘要；详细逐地址机器码保留在 --json 报告里。
    formula = report["LegacyClock机器公式"]
    print(f"工具版本：{TOOL_VERSION}")
    print(f"RPG.exe SHA-256：{report['输入']['RPG.exe_SHA256']}")
    print(f"DDDES.SCI SHA-256：{report['输入']['DDDES.SCI']['SHA256']}")
    print(f"DDDES +0x11：{formula['DDDES逻辑频率参数_R']}")
    print(f"固定基础常量：{formula['固定基础常量_B']}")
    print(f"逻辑divider：{formula['divider_floor_B_div_R']}")
    print(f"名义Legacy Tick：{formula['在p等于1ms时名义LegacyTick_ms']} ms / {formula['在p等于1ms时名义LegacyTick_Hz']:.6g} Hz")
    print(f"机器断言：{report['机器断言通过']}/{report['机器断言总数']} PASS")
    print(f"0x44A970 direct E8 callers：{', '.join(report['direct_E8_callers']['0x44A970']) or '无'}")
    print("单步语义：WM_TIMER只形成布尔pending；主循环一次pending只消费一次0x44A970，不累计N个补算步。")

    # 只有所有固化38关键条件同时成立才返回0。
    if report["固化38关键结论PASS"]:
        print("固化38关键结论：PASS")
        return 0

    # 机器断言或关键公式有任何偏差就返回1，禁止静默把不同EXE当成同一基线。
    print("固化38关键结论：FAIL", file=sys.stderr)
    return 1


# 只有用户直接执行这个.py时才进入命令行；被别的工具import时不会自动跑分析。
if __name__ == "__main__":
    raise SystemExit(main())
