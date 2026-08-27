#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Castle_SaveEnhance v0.1.0-test6 静态验证工具。

这个工具只读取文件，不会修改 RPG.exe、MiscInfo.ENC 或 Castle_SaveEnhance.asi。
它的目标是让任何接手者都能重复确认：当前候选是不是针对我们锁定的台湾第三版原版，
100 槽/循环分页/安全存档/保留槽 Hook 的机器码是否仍然完全匹配，以及 ASI 本身是不是
真正可由 32 位 Windows 装载的 PE32 DLL。

为什么要做这么多“逐字节检查”：
- 老游戏补丁不是调用一个稳定 SDK，而是在固定地址改 CPU 指令；
- 同一个地址只要游戏版本不同，含义就可能完全不同；
- 所以不能只检查“文件能打开”，必须同时检查 EXE 身份和每个关键机器码；
- 静态 PASS 仍然不等于实机 PASS，UI/剧情/战斗等运行时状态只能在真实游戏里验收。
"""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Tuple


# ============================================================================
# 一、锁定的用户目标文件身份
# ============================================================================
EXPECTED_RPG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
EXPECTED_RPG_IMAGE_BASE = 0x00400000
EXPECTED_MISCINFO_SIZE = 0x41E
EXPECTED_MISCINFO_DECODED_SHA256 = (
    "cd18d110d458fe60928eb874f76453f751489cf2d0341d2c7c250c4ece13e6e9"
)
EXPECTED_MISCINFO_PAGE_COUNT_OFFSET = 0x3C2
EXPECTED_MISCINFO_PAGE_COUNT = 8


# ============================================================================
# 二、100 槽 + 循环分页固定补丁：共 10 项
# ============================================================================
# 这里记录的是“插件安装前的原版字节”。只要其中一处不同，当前候选就不应该在那个 EXE
# 上盲目安装，因为它可能是其它版本，也可能已经被另一个 MOD 改过。
FIXED_MENU_SITES: Sequence[Tuple[str, int, bytes]] = (
    ("LoadSlot 槽号 +1", 0x00424D8D, bytes.fromhex("8D 54 01 01")),
    ("SaveSlot 槽号 +1", 0x00424DE7, bytes.fromhex("8D 4C 02 01")),
    ("聚焦行控件槽号 +1", 0x00425324, bytes.fromhex("8D 54 02 01")),
    ("元数据检查槽号 +1", 0x004253C0, bytes.fromhex("8D 54 10 01")),
    ("当前选择标签槽号 +1", 0x00425486, bytes.fromhex("8D 4C 10 01")),
    ("行标签槽号 +1", 0x004255BA, bytes.fromhex("8D 54 11 01")),
    ("下一页逻辑读取 MiscInfo 页数", 0x00424ECB, bytes.fromhex("8B 8A C2 03 00 00")),
    ("下一页箭头读取 MiscInfo 页数", 0x0042565A, bytes.fromhex("8B 91 C2 03 00 00")),
    ("第一页上一页箭头边界分支", 0x00425626, bytes.fromhex("7E 11")),
    ("最后页下一页箭头边界分支", 0x00425665, bytes.fromhex("7D 11")),
)

# 两条原版 6 字节 mov eax,[esi+598] 会被 CALL+nop 替换，用来完成 0<->96 的真实循环页。
PAGE_LOOP_CALL_SITES: Sequence[Tuple[str, int, bytes]] = (
    ("上一页 page-base 读取", 0x00424E90, bytes.fromhex("8B 86 98 05 00 00")),
    ("下一页 page-base 读取", 0x00424EC2, bytes.fromhex("8B 86 98 05 00 00")),
)

# test5 直接调用游戏自己的 File 对象判断自动档是否存在。
GAME_FILE_PREFIXES: Sequence[Tuple[str, int, bytes]] = (
    ("Game File ctor 0x4416F0", 0x004416F0, bytes.fromhex("8A 54 24 04 8B C1 33 C9 C7 00 FF FF FF FF")),
    ("Game File dtor 0x441710", 0x00441710, bytes.fromhex("56 8B F1 8B 46 0C C6 46 09 00")),
    ("Game File open 0x4417C0", 0x004417C0, bytes.fromhex("81 EC 2C 01 00 00 55 56 57 8B BC 24 3C 01 00 00")),
    ("Game File close 0x441A00", 0x00441A00, bytes.fromhex("56 8B F1 8B 06 83 F8 FF 74 1F 8A 4E 09 84 C9 75")),
    ("Game File read 0x441A30", 0x00441A30, bytes.fromhex("53 8B 5C 24 10 56 8B F1 85 DB 74 66 80 7E 08 01")),
    ("Game File write 0x441AB0", 0x00441AB0, bytes.fromhex("56 8B F1 57 80 7E 08 02 74 07 5F 32 C0 5E C2 0C")),
)

# ============================================================================
# 三、5 个 5-byte CALL Hook
# ============================================================================
CALL_HOOK_SITES: Sequence[Tuple[str, int, bytes]] = (
    ("正常菜单保存许可", 0x0040CCC2, bytes.fromhex("E8 F9 D3 FF FF")),
    ("普通地图 Legacy Tick", 0x0040B078, bytes.fromhex("E8 63 FD FF FF")),
    ("SaveSlot 内部 Writer", 0x0043B34C, bytes.fromhex("E8 0F 00 00 00")),
    ("存档菜单手动 SaveSlot", 0x00424DF2, bytes.fromhex("E8 29 65 01 00")),
    ("隐藏命令手动 SaveSlot", 0x0044A82D, bytes.fromhex("E8 EE 0A FF FF")),
)

# SaveAction vtable +0x18 原本指向 0x4262C0。SaveEnhance 只替换这个 Update 虚函数，
# 不抢 Controller 已经使用的 SaveAction 内部 Button HitTest/Event CALL。
SAVE_ACTION_VTABLE_SITE = (
    "SaveAction vtable Update 指针",
    0x00460BA8,
    bytes.fromhex("C0 62 42 00"),
)

# test2 不再 Hook test1 的三个 SaveSlot constructor 调用者。
# 父 SaveSlot 改为运行时只读三个 owner：
# [0x008E241C]+0x5B4、[0x008DED0C]+0x654、[0x0089FCD0]+0x580。
# 这三个是运行时对象路径，不是需要改写的 EXE 指令，所以不会出现在机器码写入表中。

# 原版 save gate 本体。历史旧补丁会把它改成“永远返回 1”，当前 SaveEnhance 会识别后恢复；
# 静态验证器以干净原版为基准，因此这里只把原版字节列为 PASS 条件。
SAVE_GATE_FUNCTION_SITE = (
    "原版 save gate 0x40A0C0",
    0x0040A0C0,
    bytes.fromhex("8B 01 8B 80 80 03 00 00 C3"),
)

# SaveSlot / LoadSlot 开头足以证明第一个栈参数是 slot，并进入同一动态三位编号命名路径。
SAVE_LOAD_PREFIXES: Sequence[Tuple[str, int, bytes]] = (
    (
        "SaveSlot 0x43B320 关键前缀",
        0x0043B320,
        bytes.fromhex(
            "8B 44 24 04 81 EC 2C 01 00 00 56 8B F1 50 68 0C BB 46 00 "
            "8D 4C 24 0C 68 04 BB 46 00"
        ),
    ),
    (
        "LoadSlot 0x43B4D0 关键前缀",
        0x0043B4D0,
        bytes.fromhex(
            "8B 44 24 04 81 EC 2C 01 00 00 56 8B F1 50 68 0C BB 46 00 "
            "8D 4C 24 0C 68 04 BB 46 00"
        ),
    ),
)

STRING_SITES: Sequence[Tuple[str, int, bytes]] = (
    ("三位槽号格式串", 0x0046BB04, b"%s%03d\x00"),
    ("Save 文件基名", 0x0046BB0C, b"Save\x00"),
    ("TSF 扩展名", 0x0046A43C, b".TSF\x00"),
)


# ============================================================================
# 四、最小 PE32 读取器
# ============================================================================
@dataclass(frozen=True)
class Section:
    """PE 的一个节：把内存中的 RVA 映射回磁盘文件偏移。"""

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


@dataclass
class CheckResult:
    """一条人类可读检查结果。"""

    name: str
    ok: bool
    detail: str


class PEFile:
    """只实现本验证器需要的 PE32 解析，不依赖第三方 pefile。"""

    def __init__(self, path: Path) -> None:
        self.path = path
        self.data = path.read_bytes()

        # 所有 Windows PE 文件前面都保留 DOS 'MZ' 头。
        if len(self.data) < 0x40 or self.data[:2] != b"MZ":
            raise ValueError("缺少 MZ 文件头")

        # DOS 头 +0x3C 是真正 PE 头的磁盘偏移。
        self.pe_offset = self._u32(0x3C)
        if self.pe_offset + 24 > len(self.data):
            raise ValueError("PE 头偏移越界")
        if self.data[self.pe_offset : self.pe_offset + 4] != b"PE\0\0":
            raise ValueError("缺少 PE\\0\\0 签名")

        self.machine = self._u16(self.pe_offset + 4)
        self.section_count = self._u16(self.pe_offset + 6)
        self.optional_header_size = self._u16(self.pe_offset + 20)
        self.characteristics = self._u16(self.pe_offset + 22)
        self.optional_offset = self.pe_offset + 24
        self.optional_magic = self._u16(self.optional_offset)
        if self.optional_magic != 0x10B:
            raise ValueError(f"不是 PE32，OptionalHeader.Magic=0x{self.optional_magic:04X}")

        self.entry_point_rva = self._u32(self.optional_offset + 0x10)
        self.image_base = self._u32(self.optional_offset + 0x1C)

        # IMAGE_SECTION_HEADER 固定 40 字节。
        self.sections: List[Section] = []
        table = self.optional_offset + self.optional_header_size
        for index in range(self.section_count):
            off = table + index * 40
            if off + 40 > len(self.data):
                raise ValueError(f"第 {index} 个节表项越界")
            name = self.data[off : off + 8].split(b"\0", 1)[0].decode("ascii", "replace")
            virtual_size = self._u32(off + 8)
            virtual_address = self._u32(off + 12)
            raw_size = self._u32(off + 16)
            raw_offset = self._u32(off + 20)
            self.sections.append(
                Section(name, virtual_address, virtual_size, raw_offset, raw_size)
            )

    def _u16(self, offset: int) -> int:
        if offset < 0 or offset + 2 > len(self.data):
            raise ValueError(f"uint16 读取越界 0x{offset:X}")
        return struct.unpack_from("<H", self.data, offset)[0]

    def _u32(self, offset: int) -> int:
        if offset < 0 or offset + 4 > len(self.data):
            raise ValueError(f"uint32 读取越界 0x{offset:X}")
        return struct.unpack_from("<I", self.data, offset)[0]

    def rva_to_raw(self, rva: int) -> int:
        # PE 头区域通常 RVA==raw offset；先覆盖这个简单情况。
        first_section = min((section.virtual_address for section in self.sections), default=0x1000)
        if 0 <= rva < first_section and rva < len(self.data):
            return rva

        for section in self.sections:
            span = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + span:
                delta = rva - section.virtual_address
                if delta >= section.raw_size:
                    raise ValueError(
                        f"RVA 0x{rva:X} 位于 {section.name} 的内存零填充区，没有磁盘原始字节"
                    )
                raw = section.raw_offset + delta
                if raw >= len(self.data):
                    raise ValueError(f"RVA 0x{rva:X} 换算后越过文件")
                return raw
        raise ValueError(f"找不到 RVA 0x{rva:X} 所属节")

    def read_rva(self, rva: int, size: int) -> bytes:
        raw = self.rva_to_raw(rva)
        if raw + size > len(self.data):
            raise ValueError(f"RVA 0x{rva:X} 读取 {size} 字节越界")
        return self.data[raw : raw + size]

    def read_va(self, va: int, size: int) -> bytes:
        if va < self.image_base:
            raise ValueError(f"VA 0x{va:X} 小于 ImageBase")
        return self.read_rva(va - self.image_base, size)

    def read_c_string_rva(self, rva: int, max_length: int = 512) -> str:
        raw = self.rva_to_raw(rva)
        end = self.data.find(b"\0", raw, min(len(self.data), raw + max_length))
        if end < 0:
            raise ValueError(f"RVA 0x{rva:X} 的字符串没有 NUL 结束")
        return self.data[raw:end].decode("ascii", "replace")

    def import_directory(self) -> Tuple[int, int]:
        # PE32 Optional Header 的 DataDirectory 从 +0x60 开始，第 1 项是 Import。
        entry = self.optional_offset + 0x60 + 8
        if entry + 8 > self.optional_offset + self.optional_header_size:
            return (0, 0)
        return (self._u32(entry), self._u32(entry + 4))

    def export_directory(self) -> Tuple[int, int]:
        """返回 PE32 DataDirectory[0]，也就是 Export Directory 的 RVA 和大小。"""
        entry = self.optional_offset + 0x60
        if entry + 8 > self.optional_offset + self.optional_header_size:
            return (0, 0)
        return (self._u32(entry), self._u32(entry + 4))

    def list_exports(self) -> List[str]:
        """
        只解析导出函数名。test5 必须继续真正导出 InitializeASI，Castle Mod Loader 才会在
        LoadLibraryExW 返回、补完 Locale/Overrides IAT 后调用正式初始化。

        IMAGE_EXPORT_DIRECTORY 固定 40 字节，其中：
        - +0x18 = NumberOfNames；
        - +0x20 = AddressOfNames，也就是一张“字符串 RVA 数组”。
        对刚学编程的人，可以把它理解成：先知道有几个名字，再沿着目录给出的清单逐个取名字。
        """
        export_rva, _size = self.export_directory()
        if export_rva == 0:
            return []
        raw = self.rva_to_raw(export_rva)
        if raw + 40 > len(self.data):
            raise ValueError("Export Directory 越界")
        number_of_names = struct.unpack_from("<I", self.data, raw + 0x18)[0]
        address_of_names = struct.unpack_from("<I", self.data, raw + 0x20)[0]
        if number_of_names > 4096:
            raise ValueError(f"导出名字数量异常：{number_of_names}")
        if number_of_names == 0:
            return []
        names_raw = self.rva_to_raw(address_of_names)
        if names_raw + number_of_names * 4 > len(self.data):
            raise ValueError("Export Name Pointer Table 越界")
        result: List[str] = []
        for index in range(number_of_names):
            name_rva = struct.unpack_from("<I", self.data, names_raw + index * 4)[0]
            result.append(self.read_c_string_rva(name_rva))
        return result

    def list_imports(self) -> Dict[str, List[str]]:
        """解析 DLL 名和 IMAGE_IMPORT_BY_NAME；当前 ASI 不需要 ordinal 导入。"""
        result: Dict[str, List[str]] = {}
        import_rva, _size = self.import_directory()
        if import_rva == 0:
            return result

        descriptor_raw = self.rva_to_raw(import_rva)
        descriptor_index = 0
        while True:
            off = descriptor_raw + descriptor_index * 20
            if off + 20 > len(self.data):
                raise ValueError("Import Descriptor 越界")
            original_first_thunk, timestamp, forwarder, name_rva, first_thunk = struct.unpack_from(
                "<IIIII", self.data, off
            )
            if not any((original_first_thunk, timestamp, forwarder, name_rva, first_thunk)):
                break

            dll = self.read_c_string_rva(name_rva)
            thunk_rva = original_first_thunk or first_thunk
            thunk_raw = self.rva_to_raw(thunk_rva)
            names: List[str] = []
            index = 0
            while True:
                thunk_off = thunk_raw + index * 4
                if thunk_off + 4 > len(self.data):
                    raise ValueError(f"{dll} thunk 越界")
                value = struct.unpack_from("<I", self.data, thunk_off)[0]
                if value == 0:
                    break
                if value & 0x80000000:
                    names.append(f"#ORDINAL_{value & 0xFFFF}")
                else:
                    names.append(self.read_c_string_rva(value + 2))
                index += 1
            result[dll] = names
            descriptor_index += 1
        return result


# ============================================================================
# 五、通用检查辅助函数
# ============================================================================
def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def pretty_bytes(data: bytes) -> str:
    return " ".join(f"{byte:02X}" for byte in data)


def add_va_check(
    results: List[CheckResult], pe: PEFile, name: str, va: int, expected: bytes
) -> None:
    try:
        actual = pe.read_va(va, len(expected))
        results.append(
            CheckResult(
                name,
                actual == expected,
                f"VA=0x{va:08X} 预期={pretty_bytes(expected)} 实际={pretty_bytes(actual)}",
            )
        )
    except Exception as exc:  # 验证器必须继续报告其它项目，所以把异常变成 FAIL 行。
        results.append(CheckResult(name, False, f"VA=0x{va:08X} 读取失败：{exc}"))


def decode_miscinfo(encoded: bytes) -> bytes:
    """复现固化68中已经确认的 ENC 解码：每字节 XOR (index % 255)。"""
    decoded = bytearray(encoded)
    for index in range(len(decoded)):
        decoded[index] ^= index % 255
    return bytes(decoded)


# ============================================================================
# 六、验证原版 RPG.exe
# ============================================================================
def verify_rpg(path: Path) -> List[CheckResult]:
    results: List[CheckResult] = []
    try:
        pe = PEFile(path)
    except Exception as exc:
        return [CheckResult("RPG.exe PE 解析", False, str(exc))]

    digest = sha256_bytes(pe.data)
    results.append(
        CheckResult(
            "RPG.exe SHA-256",
            digest == EXPECTED_RPG_SHA256,
            f"预期={EXPECTED_RPG_SHA256} 实际={digest}",
        )
    )
    results.append(
        CheckResult(
            "RPG.exe 为 PE32/i386",
            pe.machine == 0x014C and pe.optional_magic == 0x010B,
            f"Machine=0x{pe.machine:04X} OptionalMagic=0x{pe.optional_magic:04X}",
        )
    )
    results.append(
        CheckResult(
            "RPG.exe ImageBase",
            pe.image_base == EXPECTED_RPG_IMAGE_BASE,
            f"预期=0x{EXPECTED_RPG_IMAGE_BASE:08X} 实际=0x{pe.image_base:08X}",
        )
    )

    for name, va, expected in FIXED_MENU_SITES:
        add_va_check(results, pe, f"固定菜单补丁：{name}", va, expected)
    for name, va, expected in PAGE_LOOP_CALL_SITES:
        add_va_check(results, pe, f"循环分页 Hook：{name}", va, expected)
    for name, va, expected in CALL_HOOK_SITES:
        add_va_check(results, pe, f"CALL Hook：{name}", va, expected)
    add_va_check(results, pe, *SAVE_ACTION_VTABLE_SITE)
    add_va_check(results, pe, *SAVE_GATE_FUNCTION_SITE)
    for name, va, expected in SAVE_LOAD_PREFIXES:
        add_va_check(results, pe, name, va, expected)
    for name, va, expected in GAME_FILE_PREFIXES:
        add_va_check(results, pe, name, va, expected)
    for name, va, expected in STRING_SITES:
        add_va_check(results, pe, f"存档命名证据：{name}", va, expected)
    return results


# ============================================================================
# 七、验证 MiscInfo.ENC
# ============================================================================
def verify_miscinfo(path: Path) -> List[CheckResult]:
    results: List[CheckResult] = []
    try:
        encoded = path.read_bytes()
    except OSError as exc:
        return [CheckResult("读取 MiscInfo.ENC", False, str(exc))]

    results.append(
        CheckResult(
            "MiscInfo.ENC 文件大小",
            len(encoded) == EXPECTED_MISCINFO_SIZE,
            f"预期={EXPECTED_MISCINFO_SIZE} 实际={len(encoded)}",
        )
    )
    decoded = decode_miscinfo(encoded)
    digest = sha256_bytes(decoded)
    results.append(
        CheckResult(
            "MiscInfo 解码后 SHA-256",
            digest == EXPECTED_MISCINFO_DECODED_SHA256,
            f"预期={EXPECTED_MISCINFO_DECODED_SHA256} 实际={digest}",
        )
    )
    offset = EXPECTED_MISCINFO_PAGE_COUNT_OFFSET
    if offset + 4 <= len(decoded):
        page_count = struct.unpack_from("<I", decoded, offset)[0]
        results.append(
            CheckResult(
                "MiscInfo +0x3C2 原版页数",
                page_count == EXPECTED_MISCINFO_PAGE_COUNT,
                f"预期=8 实际={page_count}；原版 8页×4行=32槽",
            )
        )
    else:
        results.append(CheckResult("MiscInfo +0x3C2 原版页数", False, "文件长度不足"))
    return results


# ============================================================================
# 八、验证候选 ASI
# ============================================================================
def verify_asi(path: Path) -> List[CheckResult]:
    results: List[CheckResult] = []
    try:
        pe = PEFile(path)
    except Exception as exc:
        return [CheckResult("Castle_SaveEnhance.asi PE 解析", False, str(exc))]

    results.append(
        CheckResult(
            "ASI 为 PE32/i386",
            pe.machine == 0x014C and pe.optional_magic == 0x010B,
            f"Machine=0x{pe.machine:04X} OptionalMagic=0x{pe.optional_magic:04X}",
        )
    )
    results.append(
        CheckResult(
            "ASI 带 DLL 标志",
            (pe.characteristics & 0x2000) != 0,
            f"Characteristics=0x{pe.characteristics:04X}",
        )
    )
    results.append(
        CheckResult(
            "ASI 入口点非零",
            pe.entry_point_rva != 0,
            f"AddressOfEntryPoint=0x{pe.entry_point_rva:X}",
        )
    )

    try:
        exports = pe.list_exports()
        results.append(
            CheckResult(
                "ASI 导出 Castle Mod Loader 正式入口 InitializeASI",
                "InitializeASI" in exports,
                "导出=" + (", ".join(exports) if exports else "<无>"),
            )
        )
    except Exception as exc:
        results.append(CheckResult("ASI Export Table 解析", False, str(exc)))

    try:
        imports = pe.list_imports()
        lower_dlls = {name.lower() for name in imports}
        results.append(
            CheckResult(
                "ASI 静态只导入 KERNEL32.dll",
                lower_dlls == {"kernel32.dll"},
                "DLL=" + (", ".join(sorted(imports)) if imports else "<无>"),
            )
        )

        kernel_names: set[str] = set()
        for dll, names in imports.items():
            if dll.lower() == "kernel32.dll":
                kernel_names.update(names)

        # 这些 API 都是当前源码直接静态使用的。user32/winmm 是运行时 LoadLibrary/GetProcAddress，
        # 所以不应该出现在 PE 静态 Import Table 中。
        required = {
            "CloseHandle",
            "CreateFileW",
            "DisableThreadLibraryCalls",
            "FlushInstructionCache",
            "GetCurrentProcess",
            "GetCurrentProcessId",
            "GetFileSize",
            "GetProcessHeap",
            "HeapAlloc",
            "HeapFree",
            "ReadFile",
            "GetModuleFileNameW",
            "GetModuleHandleA",
            "GetModuleHandleW",
            "GetPrivateProfileIntW",
            "GetPrivateProfileStringW",
            "GetProcAddress",
            "GetTickCount",
            "LoadLibraryW",
            "VirtualProtect",
            "VirtualQuery",
            "WriteFile",
        }
        missing = sorted(required - kernel_names)
        results.append(
            CheckResult(
                "ASI 必需 KERNEL32 导入齐全且未被装饰",
                not missing and all(not name.startswith("_") and "@" not in name for name in kernel_names),
                "缺少=" + (", ".join(missing) if missing else "无")
                + "; 实际=" + ", ".join(sorted(kernel_names)),
            )
        )

        # 用户已经明确要求 NextAutoSlot 只能写进 Save\.NEXTAUTOSLOT，不能再回写 INI。
        # 如果以后有人误把 WritePrivateProfileStringW 加回源码，这条导入表硬检查会立刻失败，
        # 不会只靠人工阅读文档才发现轮换状态又跟着 INI 走了。
        writes_private_profile = "WritePrivateProfileStringW" in kernel_names
        results.append(
            CheckResult(
                "NextAutoSlot 不再通过 INI 写回",
                not writes_private_profile,
                "WritePrivateProfileStringW=" + ("存在" if writes_private_profile else "不存在"),
            )
        )

        crt_markers = ("msvcr", "msvcp", "ucrt", "vcruntime", "api-ms-win-crt")
        crt = sorted(
            dll for dll in imports if any(marker in dll.lower() for marker in crt_markers)
        )
        results.append(
            CheckResult(
                "ASI 无 CRT/UCRT/VCRUNTIME 静态依赖",
                not crt,
                "发现=" + (", ".join(crt) if crt else "无"),
            )
        )
        results.append(
            CheckResult(
                "user32/winmm 保持动态可选依赖",
                "user32.dll" not in lower_dlls and "winmm.dll" not in lower_dlls,
                "Import Table 中没有 user32/winmm；运行时需要时才 LoadLibrary",
            )
        )
    except Exception as exc:
        results.append(CheckResult("ASI Import Table 解析", False, str(exc)))

    # 二进制中保留版本名是一个低成本 sanity check，能抓到误打包旧 AnytimeSave 的情况。
    marker = "Castle_SaveEnhance".encode("ascii")
    results.append(
        CheckResult(
            "ASI 含 Castle_SaveEnhance 身份字符串",
            marker in pe.data,
            "存在" if marker in pe.data else "未找到",
        )
    )

    # C++ 源码现在把 ANSI 相对路径 "Save\\.NEXTAUTOSLOT" 交给游戏自己的 File::Open。
    # 这条检查既能证明新路径进入了产物，也能防止误打包仍使用 Win32 宽字符绝对路径的旧 ASI。
    state_path_marker = "Save\\.NEXTAUTOSLOT".encode("ascii")
    results.append(
        CheckResult(
            "ASI 含自动槽状态文件路径 Save\\.NEXTAUTOSLOT",
            state_path_marker in pe.data,
            "存在" if state_path_marker in pe.data else "未找到",
        )
    )
    results.append(CheckResult("ASI SHA-256（记录）", True, sha256_bytes(pe.data)))
    return results


# ============================================================================
# 九、输出与命令行入口
# ============================================================================
def print_group(title: str, results: Sequence[CheckResult]) -> None:
    print(f"\n=== {title} ===")
    for result in results:
        print(f"[{'PASS' if result.ok else 'FAIL'}] {result.name}")
        print(f"       {result.detail}")


def parser() -> argparse.ArgumentParser:
    command = argparse.ArgumentParser(
        description="只读验证 Castle_SaveEnhance v0.1.0-test6 的目标 EXE、MiscInfo 与 ASI。"
    )
    command.add_argument("--rpg", required=True, type=Path, help="锁定原版 RPG.exe")
    command.add_argument("--miscinfo", required=True, type=Path, help="Public\\MiscInfo.ENC")
    command.add_argument("--asi", required=True, type=Path, help="Castle_SaveEnhance.asi")
    command.add_argument(
        "--json-output", type=Path, default=None, help="可选：另存一份 UTF-8 JSON 检查报告"
    )
    return command


def main(argv: Optional[Sequence[str]] = None) -> int:
    args = parser().parse_args(argv)
    missing = [path for path in (args.rpg, args.miscinfo, args.asi) if not path.is_file()]
    if missing:
        for path in missing:
            print(f"[FAIL] 找不到输入文件：{path}")
        return 2

    groups: List[Tuple[str, List[CheckResult]]] = [
        ("原版 RPG.exe", verify_rpg(args.rpg)),
        ("MiscInfo.ENC", verify_miscinfo(args.miscinfo)),
        ("Castle_SaveEnhance.asi", verify_asi(args.asi)),
    ]
    all_ok = True
    for title, results in groups:
        print_group(title, results)
        if any(not item.ok for item in results):
            all_ok = False

    if args.json_output is not None:
        payload = {
            "tool": "verify_saveenhance_candidate.py",
            "version": "v0.1.0-test6",
            "all_passed": all_ok,
            "results": [
                {
                    "group": group,
                    "name": result.name,
                    "ok": result.ok,
                    "detail": result.detail,
                }
                for group, results in groups
                for result in results
            ],
        }
        args.json_output.parent.mkdir(parents=True, exist_ok=True)
        args.json_output.write_text(
            json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
        )
        print(f"\n[INFO] JSON 报告已写入：{args.json_output}")

    print("\n=== 总结 ===")
    if all_ok:
        print("[PASS] 所有静态检查通过。注意：这仍然不是游戏实机验收。")
        return 0
    print("[FAIL] 至少一条静态检查失败；不要把该文件组合用于本版本实机测试。")
    return 1


if __name__ == "__main__":
    sys.exit(main())
