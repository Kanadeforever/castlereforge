#!/usr/bin/env python3
"""
Castle PadSupport Public API v1 静态检查器。

这个工具只做“公共接口有没有被正确接进当前源码/二进制”的检查，不替代原来的
refactor_check.py。两者分工如下：

- refactor_check.py：继续验证 refactor44 的游戏功能、地址、页面适配和历史基线；
- public_api_check.py：只验证 CastlePad_GetApi v1 的源码、构建脚本和 PE 导出。

工具只使用 Python 标准库，不需要 pefile。
"""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path


def fail(message: str) -> None:
    """打印一条 FAIL，并用非零退出码结束。"""

    print(f"[FAIL] {message}")
    raise SystemExit(1)


def check_source(source: Path) -> None:
    """
    验证 Public API 的四个新增源码文件以及两个接入点。

    这里故意检查“语义锚点”而不是文件 SHA：
    后续 refactor 可以继续修改周边实现，只要稳定 ABI 和发布时序仍存在即可。
    """

    required_files = [
        "Castle_PadSupport_API.h",
        "pad_public_api.h",
        "pad_public_api.c",
        "Castle_PadSupport.def",
        "plugin.c",
    ]
    missing = [name for name in required_files if not (source / name).is_file()]
    if missing:
        fail("缺少 Public API 文件：" + ", ".join(missing))

    api = (source / "Castle_PadSupport_API.h").read_text(encoding="utf-8-sig")
    provider = (source / "pad_public_api.c").read_text(encoding="utf-8-sig")
    plugin = (source / "plugin.c").read_text(encoding="utf-8-sig")
    # 新仓库结构把 build.bat 放在 Controller 包根，源码则集中在 source/。
    # 这里从 source 的父目录读取构建脚本，绝不能为了兼容旧检查器去恢复“源码/build.bat”。
    build_path = source.parent / "build.bat"
    if not build_path.is_file():
        fail("Controller 包根缺少 build.bat。")
    build = build_path.read_text(encoding="utf-8-sig")
    definition = (source / "Castle_PadSupport.def").read_text(encoding="ascii")

    needles = [
        "CASTLE_PAD_API_MAGIC",
        "CASTLE_PAD_API_VERSION_1",
        "CastlePadApiV1",
        "CastlePadGetApiFn",
        "AllowsExternalUiInput",
    ]
    if not all(token in api for token in needles):
        fail("公共头文件缺少 v1 稳定契约字段。")

    provider_needles = [
        "CastlePad_PublicApiPublishFrame",
        "CastlePad_PublicApiReset",
        "CastlePad_GetApi",
        "PadInput_Ready",
        "ControlModes_PublicMode",
    ]
    if not all(token in provider for token in provider_needles):
        fail("provider 没有完整使用 PadInput/InputRouter/ControlModes 发布快照。")

    if '#include "pad_public_api.h"' not in plugin:
        fail("plugin.c 没有包含 Public API 发布器。")
    if "CastlePad_PublicApiPublishFrame();" not in plugin:
        fail("plugin.c 没有每 tick 发布 Public API 快照。")
    if plugin.count("CastlePad_PublicApiReset();") < 2:
        fail("plugin.c 缺少初始化/退出阶段的 Public API Reset。")

    if "pad_public_api.c pad_public_api.obj" not in build:
        fail("build.bat 没有编译 pad_public_api.c。")
    if "Castle_PadSupport.def" not in build or "/def:" not in build:
        fail("build.bat 没有使用 Castle_PadSupport.def。")

    lines = [
        line.strip()
        for line in definition.replace("\r\n", "\n").split("\n")
        if line.strip() and not line.lstrip().startswith(";")
    ]
    if "CastlePad_GetApi" not in lines:
        fail("DEF 没有裸导出 CastlePad_GetApi。")
    if any("_CastlePad_GetApi" in line for line in lines):
        fail("DEF 使用了错误的 x86 下划线别名；这会产生 __CastlePad_GetApi。")

    print("[PASS] Public API v1 源码与构建接入完整。")


def rva_to_offset(data: bytes, pe_offset: int, rva: int) -> int:
    """把 PE RVA 映射成磁盘偏移，只实现导出表检查需要的最小逻辑。"""

    coff = pe_offset + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    section_table = optional + optional_size

    for index in range(section_count):
        base = section_table + index * 40
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", data, base + 8)
        span = max(virtual_size, raw_size)
        if virtual_address <= rva < virtual_address + span:
            relative = rva - virtual_address
            if relative >= raw_size:
                raise ValueError("RVA 落在节的零填充区。")
            return raw_offset + relative
    raise ValueError(f"无法映射 RVA 0x{rva:X}")


def read_c_string(data: bytes, offset: int) -> str:
    """读取 PE 导出名的 NUL 结尾 ASCII 字符串。"""

    end = data.find(b"\0", offset)
    if end < 0:
        raise ValueError("导出名字符串没有 NUL 结尾。")
    return data[offset:end].decode("ascii", "strict")


def exported_names(path: Path) -> list[str]:
    """读取 PE32 导出目录，返回所有公开名称。"""

    data = path.read_bytes()
    if data[:2] != b"MZ":
        raise ValueError("不是 MZ 文件。")

    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise ValueError("没有 PE 签名。")

    coff = pe_offset + 4
    machine = struct.unpack_from("<H", data, coff)[0]
    if machine != 0x14C:
        raise ValueError(f"不是 x86 PE，Machine=0x{machine:04X}。")

    optional = coff + 20
    if struct.unpack_from("<H", data, optional)[0] != 0x10B:
        raise ValueError("不是 PE32。")

    # PE32 DataDirectory[0] = Export Directory，起点是 OptionalHeader + 96。
    export_rva, export_size = struct.unpack_from("<II", data, optional + 96)
    if export_rva == 0 or export_size == 0:
        return []

    export_offset = rva_to_offset(data, pe_offset, export_rva)
    number_of_names = struct.unpack_from("<I", data, export_offset + 24)[0]
    names_rva = struct.unpack_from("<I", data, export_offset + 32)[0]
    names_offset = rva_to_offset(data, pe_offset, names_rva)

    names: list[str] = []
    for index in range(number_of_names):
        name_rva = struct.unpack_from("<I", data, names_offset + index * 4)[0]
        name_offset = rva_to_offset(data, pe_offset, name_rva)
        names.append(read_c_string(data, name_offset))
    return names


def check_binary(path: Path) -> None:
    """验证最终 ASI 的 PE export 真的是未修饰 CastlePad_GetApi。"""

    names = exported_names(path)
    if "CastlePad_GetApi" not in names:
        fail("ASI 没有导出未修饰 CastlePad_GetApi；实际导出：" + ", ".join(names))
    if "_CastlePad_GetApi" in names or "__CastlePad_GetApi" in names:
        fail("ASI 同时存在错误的下划线修饰导出。")
    print("[PASS] PE32/x86 导出名正确：CastlePad_GetApi")


def main() -> int:
    parser = argparse.ArgumentParser(description="检查 Castle PadSupport Public API v1")
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1] / "source")
    parser.add_argument("--asi", type=Path, help="可选：检查已编译 Castle_PadSupport.asi 的导出表")
    args = parser.parse_args()

    check_source(args.source)
    if args.asi:
        check_binary(args.asi)
    return 0


if __name__ == "__main__":
    sys.exit(main())
