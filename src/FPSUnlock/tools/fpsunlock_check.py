#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
fpsunlock_check.py

《幽城幻剑录》Castle_FPSUnlock v1.3 静态检查工具。

这个脚本完全不会修改 RPG.exe 或 ASI。它只做“读文件 -> 检查 -> 报告”。
设计它的原因是：本插件使用的是 2002 年 32 位程序里的绝对地址，所以每次重新编译、
换 EXE、或者把别的补丁叠在一起之前，最好先用一个独立工具确认最关键的机器协议。

面向刚开始学编程的读者，可以把下面几个概念这样理解：
- 文件偏移（file offset）：某个字节在硬盘文件里从开头数第几个字节。
- RVA：程序被 Windows 装入内存后，相对于“程序起点”的位置。
- VA：真正的内存地址。原版 RPG.exe 的 ImageBase 是 0x00400000，
  所以 VA = ImageBase + RVA。
- PE section：Windows EXE/DLL 把代码、数据等分成多个区域。磁盘上的位置和内存里的
  位置不一定相同，因此不能简单用“VA - 0x400000”当成文件偏移。
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path
from typing import Dict, List, Tuple


# 用户上传并确认的台湾第三版原版 RPG.exe 基线。
EXPECTED_RPG_SIZE = 462_848
EXPECTED_RPG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
EXPECTED_IMAGE_BASE = 0x00400000

# 这里列出 v1.3 真正依赖的原版机器码锚点。
# 键是内存 VA；值是这个位置应该出现的字节。
# 0x44A9C6 / 0x44A9E6 故意不放“完整 5 字节原目标”检查，因为宽屏补丁会合法地
# 改写这两个 CALL 的目标。它们只需要仍是 E8 rel32 CALL 即可。
STRICT_ANCHORS: Dict[int, bytes] = {
    0x0040179C: bytes.fromhex("FF 15 98 01 46 00"),
    0x0040171C: bytes.fromhex("E8 4F 92 04 00"),
    0x0044A9BF: bytes.fromhex("E8 DC 76 FB FF"),
    0x0044A970: bytes.fromhex("A1 90 F3 46 00 83 EC 34"),
    0x0040B050: bytes.fromhex("56 8B F1 8A 86 19 02 00"),
    # BattleManager draw=0x4429F0 是 v1.2 起加入、v1.3 延续的明确战斗 gate。
    0x004429F0: bytes.fromhex("A0 0C 24 8E 00 53 33 DB"),
}
DYNAMIC_CALL_ANCHORS = (0x0044A9C6, 0x0044A9E6)


class PeError(RuntimeError):
    """表示文件不是我们能安全解析的标准 PE 文件。"""


class PeFile:
    """只实现本检查器真正需要的最小 PE 读取功能，不依赖第三方库。"""

    def __init__(self, path: Path) -> None:
        self.path = path
        self.data = path.read_bytes()

        # DOS 头前两个字节必须是 ASCII 的 MZ。
        if self.data[:2] != b"MZ":
            raise PeError("文件开头不是 MZ，不是标准 Windows PE 文件")

        # DOS 头 +0x3C 保存 PE 头在文件中的位置。
        self.pe_offset = self._u32(0x3C)
        if self.data[self.pe_offset:self.pe_offset + 4] != b"PE\0\0":
            raise PeError("找不到 PE\\0\\0 签名")

        coff = self.pe_offset + 4
        self.machine = self._u16(coff)
        self.number_of_sections = self._u16(coff + 2)
        self.size_of_optional_header = self._u16(coff + 16)
        self.characteristics = self._u16(coff + 18)

        optional = coff + 20
        self.optional_magic = self._u16(optional)
        if self.optional_magic != 0x10B:
            raise PeError("不是 PE32（32 位）Optional Header")

        self.image_base = self._u32(optional + 28)
        self.number_of_rva_and_sizes = self._u32(optional + 92)
        self.data_directory_offset = optional + 96

        # Section Table 紧跟在 Optional Header 后面。
        section_table = optional + self.size_of_optional_header
        self.sections: List[Tuple[str, int, int, int, int]] = []
        for index in range(self.number_of_sections):
            off = section_table + index * 40
            raw_name = self.data[off:off + 8].split(b"\0", 1)[0]
            name = raw_name.decode("ascii", errors="replace")
            virtual_size = self._u32(off + 8)
            virtual_address = self._u32(off + 12)
            raw_size = self._u32(off + 16)
            raw_offset = self._u32(off + 20)
            self.sections.append(
                (name, virtual_address, virtual_size, raw_size, raw_offset)
            )

    def _u16(self, offset: int) -> int:
        if offset < 0 or offset + 2 > len(self.data):
            raise PeError("读取 WORD 时越过文件末尾")
        return struct.unpack_from("<H", self.data, offset)[0]

    def _u32(self, offset: int) -> int:
        if offset < 0 or offset + 4 > len(self.data):
            raise PeError("读取 DWORD 时越过文件末尾")
        return struct.unpack_from("<I", self.data, offset)[0]

    def data_directory(self, index: int) -> Tuple[int, int]:
        """返回某个 PE Data Directory 的 (RVA, Size)。"""
        if index >= self.number_of_rva_and_sizes:
            return 0, 0
        off = self.data_directory_offset + index * 8
        return self._u32(off), self._u32(off + 4)

    def rva_to_file_offset(self, rva: int) -> int:
        """把 Windows 装载地址体系里的 RVA 安全转换为磁盘文件偏移。"""
        for _name, section_rva, virtual_size, raw_size, raw_offset in self.sections:
            span = max(virtual_size, raw_size)
            if section_rva <= rva < section_rva + span:
                delta = rva - section_rva
                if delta >= raw_size:
                    raise PeError("目标 RVA 落在 section 的内存补零区，磁盘没有对应字节")
                return raw_offset + delta

        # PE 头本身通常按 RVA=文件偏移映射。这里只允许仍在第一个 section 之前的情况。
        if self.sections:
            first_rva = min(section[1] for section in self.sections)
            if 0 <= rva < first_rva and rva < len(self.data):
                return rva
        raise PeError(f"RVA 0x{rva:08X} 不属于任何可读取 section")

    def read_va(self, va: int, size: int) -> bytes:
        """按 VA 读取原始机器码。"""
        if va < self.image_base:
            raise PeError("VA 小于 ImageBase")
        rva = va - self.image_base
        off = self.rva_to_file_offset(rva)
        end = off + size
        if end > len(self.data):
            raise PeError("目标机器码越过文件末尾")
        return self.data[off:end]


def sha256(path: Path) -> str:
    """分块计算 SHA-256；即使以后拿来检查更大的文件也不会一次占很多内存。"""
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        while True:
            chunk = handle.read(1024 * 1024)
            if not chunk:
                break
            digest.update(chunk)
    return digest.hexdigest()


def decode_e8_target(pe: PeFile, call_va: int) -> int | None:
    """解析 x86 的 E8 rel32 CALL，返回它当前实际跳到的 VA。"""
    code = pe.read_va(call_va, 5)
    if code[0] != 0xE8:
        return None
    relative = struct.unpack_from("<i", code, 1)[0]
    return (call_va + 5 + relative) & 0xFFFFFFFF


def check_rpg(path: Path) -> bool:
    """检查 RPG.exe 基线和 v1.1 所需协议。"""
    print(f"[RPG] 文件: {path}")
    ok = True

    actual_size = path.stat().st_size
    actual_hash = sha256(path)
    print(f"[RPG] 大小: {actual_size} bytes")
    print(f"[RPG] SHA-256: {actual_hash}")

    if actual_size != EXPECTED_RPG_SIZE:
        print(f"[失败] 大小应为 {EXPECTED_RPG_SIZE} bytes")
        ok = False
    if actual_hash != EXPECTED_RPG_SHA256:
        print("[提示] SHA-256 不是完全未修改原版。若只叠加了运行时 ASI，这里仍应是原版哈希。")
        # 哈希不同不立刻终止，因为工具还要报告具体哪些机器锚点是否仍兼容。

    try:
        pe = PeFile(path)
    except PeError as exc:
        print(f"[失败] PE 解析失败: {exc}")
        return False

    print(f"[RPG] Machine=0x{pe.machine:04X}, ImageBase=0x{pe.image_base:08X}")
    if pe.machine != 0x014C or pe.image_base != EXPECTED_IMAGE_BASE:
        print("[失败] 不是已确认的 x86 / 0x00400000 映像布局。")
        ok = False

    for va, expected in STRICT_ANCHORS.items():
        try:
            actual = pe.read_va(va, len(expected))
        except PeError as exc:
            print(f"[失败] 0x{va:08X}: {exc}")
            ok = False
            continue
        if actual == expected:
            print(f"[通过] 0x{va:08X}: {actual.hex(' ').upper()}")
        else:
            print(f"[失败] 0x{va:08X}: 当前={actual.hex(' ').upper()}")
            print(f"       期望={expected.hex(' ').upper()}")
            ok = False

    for call_va in DYNAMIC_CALL_ANCHORS:
        try:
            target = decode_e8_target(pe, call_va)
        except PeError as exc:
            print(f"[失败] 0x{call_va:08X}: {exc}")
            ok = False
            continue
        if target is None:
            print(f"[失败] 0x{call_va:08X}: 当前不是 E8 rel32 CALL")
            ok = False
        else:
            print(f"[通过] 0x{call_va:08X}: 仍是 E8 CALL，当前目标=0x{target:08X}")

    return ok


def check_asi(path: Path) -> bool:
    """检查编译成品是不是我们要求的轻量 32 位无 CRT Import ASI。"""
    print(f"\n[ASI] 文件: {path}")
    if not path.exists():
        print("[失败] ASI 文件不存在。")
        return False

    print(f"[ASI] 大小: {path.stat().st_size} bytes")
    print(f"[ASI] SHA-256: {sha256(path)}")

    try:
        pe = PeFile(path)
    except PeError as exc:
        print(f"[失败] PE 解析失败: {exc}")
        return False

    ok = True
    # IMAGE_FILE_MACHINE_I386 = 0x014C；IMAGE_FILE_DLL = 0x2000。
    if pe.machine != 0x014C:
        print(f"[失败] Machine=0x{pe.machine:04X}，不是 i386/x86。")
        ok = False
    else:
        print("[通过] Machine=i386/x86")

    if (pe.characteristics & 0x2000) == 0:
        print("[失败] PE Characteristics 没有 DLL 标志。")
        ok = False
    else:
        print("[通过] DLL 标志存在")

    # Data Directory 1 = Import Table；12 = IAT；5 = Base Relocation Table。
    import_rva, import_size = pe.data_directory(1)
    reloc_rva, reloc_size = pe.data_directory(5)
    iat_rva, iat_size = pe.data_directory(12)

    if import_rva == 0 and import_size == 0:
        print("[通过] Import Directory = 0（不依赖外部 CRT/DLL Import）")
    else:
        print(f"[失败] Import Directory 非零: RVA=0x{import_rva:X}, Size={import_size}")
        ok = False

    if iat_rva == 0 and iat_size == 0:
        print("[通过] IAT Directory = 0")
    else:
        print(f"[失败] IAT Directory 非零: RVA=0x{iat_rva:X}, Size={iat_size}")
        ok = False

    if reloc_rva != 0 and reloc_size != 0:
        print(f"[通过] Base Relocation 存在: RVA=0x{reloc_rva:X}, Size={reloc_size}")
    else:
        print("[失败] Base Relocation 缺失，ASLR 下不安全。")
        ok = False

    return ok


def main(argv: List[str]) -> int:
    """命令行入口：第一个参数 RPG.exe，第二个参数可选 ASI。"""
    if len(argv) < 2:
        print("用法: python fpsunlock_check.py <RPG.exe> [Castle_FPSUnlock.asi]")
        print("示例: python fpsunlock_check.py D:\\Game\\RPG.exe release\\Castle_FPSUnlock.asi")
        return 2

    rpg_path = Path(argv[1]).expanduser().resolve()
    if not rpg_path.is_file():
        print(f"[失败] 找不到 RPG.exe: {rpg_path}")
        return 2

    rpg_ok = check_rpg(rpg_path)
    asi_ok = True

    if len(argv) >= 3:
        asi_path = Path(argv[2]).expanduser().resolve()
        asi_ok = check_asi(asi_path)

    print("\n========== 最终结果 ==========")
    if rpg_ok and asi_ok:
        print("[通过] 当前文件满足 Castle_FPSUnlock v1.3 的静态协议要求。")
        return 0

    print("[失败] 至少有一项静态检查没有通过；不要把失败项当成已确认兼容。")
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
