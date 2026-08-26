#!/usr/bin/env python3
"""
Castle Backlog 静态检查器。

这个工具不启动游戏、不修改 EXE，也不依赖 pefile 等第三方包。它只读取：
1. 参考 RPG.exe 的 PE 头、SHA-256、关键机器码和 vtable；
2. 构建出的 Castle_Backlog.asi 的位数、导入表和版本字符串；
3. 源码/INI 的基础封包规则。

初学者可以把它理解成“交卷前逐项对答案”：只要某个关键地址、依赖或包结构不符合
本版本已经确认的事实，工具就返回非零退出码，让构建者不能把问题误写成 PASS。
"""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


SUPPORTED_GAME_SHA256 = {
    "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf",
    "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f",
}


@dataclass(frozen=True)
class Section:
    """PE 的一个节：RVA 是装载后的相对地址，raw_offset 是磁盘文件位置。"""

    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int


class PEImage:
    """只实现本检查器需要的最小 PE32 读取功能，不尝试成为通用 PE 库。"""

    def __init__(self, path: Path):
        self.path = path
        self.data = path.read_bytes()
        if len(self.data) < 0x100 or self.data[:2] != b"MZ":
            raise ValueError(f"{path} 不是合法 MZ 文件")

        self.pe_offset = self.u32(0x3C)
        if self.data[self.pe_offset : self.pe_offset + 4] != b"PE\0\0":
            raise ValueError(f"{path} 没有合法 PE 签名")

        coff = self.pe_offset + 4
        self.machine = self.u16(coff)
        self.section_count = self.u16(coff + 2)
        optional_size = self.u16(coff + 16)
        self.optional_offset = coff + 20
        if self.u16(self.optional_offset) != 0x10B:
            raise ValueError(f"{path} 不是 PE32")

        self.entry_rva = self.u32(self.optional_offset + 16)
        self.image_base = self.u32(self.optional_offset + 28)
        self.import_rva = self.u32(self.optional_offset + 96 + 8)
        self.import_size = self.u32(self.optional_offset + 96 + 12)

        section_offset = self.optional_offset + optional_size
        sections: list[Section] = []
        for index in range(self.section_count):
            base = section_offset + index * 40
            name = self.data[base : base + 8].split(b"\0", 1)[0].decode("ascii", "replace")
            sections.append(
                Section(
                    name=name,
                    virtual_size=self.u32(base + 8),
                    virtual_address=self.u32(base + 12),
                    raw_size=self.u32(base + 16),
                    raw_offset=self.u32(base + 20),
                )
            )
        self.sections = sections

    def u16(self, offset: int) -> int:
        """从文件偏移读取 little-endian uint16；越界由 struct 明确抛错。"""

        return struct.unpack_from("<H", self.data, offset)[0]

    def u32(self, offset: int) -> int:
        """从文件偏移读取 little-endian uint32。"""

        return struct.unpack_from("<I", self.data, offset)[0]

    def rva_to_offset(self, rva: int) -> int:
        """把装载时 RVA 换成磁盘偏移；只接受确实落在某个节 raw 数据中的地址。"""

        for section in self.sections:
            span = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + span:
                relative = rva - section.virtual_address
                if relative >= section.raw_size:
                    raise ValueError(f"RVA 0x{rva:X} 落在零填充区，没有磁盘原始字节")
                return section.raw_offset + relative
        if rva < min((section.raw_offset for section in self.sections), default=len(self.data)):
            return rva
        raise ValueError(f"无法映射 RVA 0x{rva:X}")

    def va_bytes(self, va: int, size: int) -> bytes:
        """按虚拟地址读取机器码；RPG.exe 的 VA 会先减去固定 image base。"""

        rva = va - self.image_base
        offset = self.rva_to_offset(rva)
        return self.data[offset : offset + size]

    def c_string_at_rva(self, rva: int, limit: int = 260) -> str:
        """读取导入表 DLL 名这类 ASCII NUL 字符串。"""

        offset = self.rva_to_offset(rva)
        end = offset
        while end < len(self.data) and end - offset < limit and self.data[end] != 0:
            end += 1
        return self.data[offset:end].decode("ascii", "replace")

    def imported_dlls(self) -> list[str]:
        """遍历 20 字节 IMAGE_IMPORT_DESCRIPTOR，只返回 DLL 名，不解析每个 API。"""

        if self.import_rva == 0:
            return []
        offset = self.rva_to_offset(self.import_rva)
        names: list[str] = []
        while offset + 20 <= len(self.data):
            descriptor = struct.unpack_from("<IIIII", self.data, offset)
            if descriptor == (0, 0, 0, 0, 0):
                break
            name_rva = descriptor[3]
            names.append(self.c_string_at_rva(name_rva).lower())
            offset += 20
        return names


@dataclass
class CheckResult:
    """一条可同时输出给人类和 JSON 的检查结论。"""

    name: str
    passed: bool
    detail: str


def check(condition: bool, name: str, detail: str) -> CheckResult:
    """把布尔条件包装成统一结果，避免主流程到处重复构造字典。"""

    return CheckResult(name=name, passed=bool(condition), detail=detail)


def check_game(path: Path) -> list[CheckResult]:
    """验证目标 RPG.exe 的身份和 Backlog 真正依赖的机器协议。"""

    pe = PEImage(path)
    sha256 = hashlib.sha256(pe.data).hexdigest()
    results = [
        check(sha256 in SUPPORTED_GAME_SHA256, "RPG.exe SHA-256 白名单", sha256),
        check(pe.machine == 0x14C, "RPG.exe 为 x86", f"machine=0x{pe.machine:04X}"),
        check(pe.image_base == 0x00400000, "RPG.exe 固定基址", f"image_base=0x{pe.image_base:08X}"),
    ]

    signatures = [
        (0x00403C60, bytes.fromhex("64 A1 00 00 00 00 6A FF 68 B6 CF 45 00"), "说话人切换入口"),
        (0x00403E30, bytes.fromhex("A0 78 F6 46 00 84 C0 75 09 A0 79 F6 46 00"), "消息更新入口"),
        (0x00404800, bytes.fromhex("83 EC 08 A0 79 F6 46 00 56 84 C0"), "消息绘制入口"),
        (0x0040B150, bytes.fromhex("56 8B F1 E8 78 01 00 00 E8 43 84 FF FF"), "场景更新入口"),
        (0x00404859, bytes.fromhex("E8 B2 2C 00 00"), "当前剧情人物图绘制 CALL"),
        (0x0040486E, bytes.fromhex("E8 9D 2C 00 00"), "F-Talk 绘制 CALL"),
        (0x00404899, bytes.fromhex("E8 72 2C 00 00"), "当前剧情 F-Name 绘制 CALL"),
        (0x004048E6, bytes.fromhex("E8 F5 E5 FF FF"), "当前剧情姓名文字绘制 CALL"),
        (0x004049FF, bytes.fromhex("E8 DC E4 FF FF"), "正文绘制 CALL"),
    ]
    for address, expected, label in signatures:
        actual = pe.va_bytes(address, len(expected))
        results.append(
            check(actual == expected, label, f"0x{address:08X}: {actual.hex(' ')}")
        )

    vtable = pe.va_bytes(0x00460298, 8)
    update, draw = struct.unpack("<II", vtable)
    results.extend(
        [
            check(update == 0x0040B150, "场景 vtable[0]", f"0x{update:08X}"),
            check(draw == 0x0040B050, "场景 vtable[1]", f"0x{draw:08X}"),
        ]
    )
    return results


def check_asi(path: Path) -> list[CheckResult]:
    """验证 ASI 位数、入口、无 SDL/CRT 硬依赖，以及 Public API 协作标记。"""

    pe = PEImage(path)
    imports = pe.imported_dlls()
    forbidden = {
        name
        for name in imports
        if "sdl" in name or "msvcr" in name or "ucrt" in name or "vcruntime" in name
    }
    # O2 可能把固定 INI 文件名拆成若干立即数写入，不保证磁盘里仍有连续 ASCII；
    # INI 的存在与内容由源码包检查负责。ASI 本体这里只要求版本标识和动态 SDL 名仍可诊断。
    required_strings = [b"Castle Backlog v0.3.3-test4", b"CastlePad_GetApi"]

    return [
        check(pe.machine == 0x14C, "ASI 为 x86", f"machine=0x{pe.machine:04X}"),
        check(pe.entry_rva != 0, "ASI 存在 DllMain 入口", f"entry_rva=0x{pe.entry_rva:X}"),
        check(not forbidden, "ASI 不强制导入 SDL/CRT", ", ".join(imports) or "无导入"),
        check(set(imports).issubset({"kernel32.dll", "user32.dll"}), "ASI 仅导入稳定 Win32 DLL", ", ".join(imports)),
        check(all(value in pe.data for value in required_strings), "ASI 携带版本/Public API 诊断字符串", "2/2"),
    ]


def has_cjk(text: str) -> bool:
    """源码必须包含面向初学者的中文注释；这里只做最低存在性护栏。"""

    return any("\u4e00" <= character <= "\u9fff" for character in text)


def check_source(source_root: Path) -> list[CheckResult]:
    """检查当前 v0.3.3-test4 源码、Public API 协作、INI 和构建文件。"""

    required = {
        "platform.h",
        "game_addresses.h",
        "runtime.h",
        "runtime.c",
        "mouse_input.h",
        "mouse_input.c",
        "pad_bridge.h",
        "pad_bridge.c",
        "Castle_PadSupport_API.h",
        "name_panel_pool.h",
        "name_panel_pool.c",
        "backlog.h",
        "backlog.c",
        "plugin.c",
        "Castle_Backlog.ini",
        "build.bat",
    }
    existing = {path.name for path in source_root.iterdir() if path.is_file()}
    code_files = sorted(source_root.glob("*.c")) + sorted(source_root.glob("*.h"))
    english_names = all(path.name.isascii() for path in code_files)
    comment_ok = all(
        has_cjk(path.read_text(encoding="utf-8-sig")) and "/*" in path.read_text(encoding="utf-8-sig")
        for path in code_files
    )

    ini_path = source_root / "Castle_Backlog.ini"
    ini_text = ini_path.read_text(encoding="utf-8-sig") if ini_path.exists() else ""
    backlog_text = (source_root / "backlog.c").read_text(encoding="utf-8-sig") if (source_root / "backlog.c").exists() else ""
    bridge_text = (source_root / "pad_bridge.c").read_text(encoding="utf-8-sig") if (source_root / "pad_bridge.c").exists() else ""
    build_text = (source_root / "build.bat").read_text(encoding="utf-8-sig") if (source_root / "build.bat").exists() else ""

    ini_needles = [
        "[Backlog]",
        "[Keyboard]",
        "Open=B",
        "Exit=B",
        "PanelStrideY=118",
        "PageSize=4",
    ]
    modern_needles = [
        # test4 的关键调度不变量：真实剧情旁路冻结，但 synthetic 不能被同一 return 截断。
        "if (g_active && g_opened_over_live_dialogue) return;",
        "Backlog_HookCurrentSpeakerPortraitDraw",
        "Backlog_HookCurrentNamePanelDraw",
        "Backlog_HookCurrentNameTextDraw",
        "Backlog_HookPanelDraw",
        "Backlog_HookTextDraw",
        "g_opened_over_live_dialogue",
        "CALL_DIALOGUE_SPEAKER_PORTRAIT_DRAW",
        "CALL_DIALOGUE_NAME_TEXT_DRAW",
        "NamePanelPool_Create",
    ]
    api_needles = [
        'GetProcAddress(g_pad_module, "CastlePad_GetApi")',
        "CASTLE_PAD_API_VERSION_1",
        "AllowsExternalUiInput",
    ]
    # 历史注释允许提到 PadInputState/ControlModeState；真正禁止的是仍存在运行时代码扫描。
    no_internal_pad_scan = all(
        token not in bridge_text
        for token in ["VirtualQuery(", "scan_executable", "IMAGE_SECTION_HEADER", "module_base +"]
    )
    no_sdl_source = not (source_root / "sdl_input.c").exists() and not (source_root / "sdl_input.h").exists()
    no_broad_active_freeze = (
        "if (g_active) return;\n\n    if (g_previous_scene_update)" not in backlog_text and
        "if (g_active) return;\r\n\r\n    if (g_previous_scene_update)" not in backlog_text
    )
    release_docs = '..\\release\\docs' in build_text and 'backlog_check.py' in build_text

    return [
        check(required.issubset(existing), "源码包必需文件", f"{len(required & existing)}/{len(required)}"),
        check(english_names, "代码文件名为英文/ASCII", ", ".join(path.name for path in code_files)),
        check(comment_ok, "每个 C/H 文件含中文块注释", f"{len(code_files)} 个文件"),
        check(all(needle in ini_text for needle in ini_needles), "INI Virtual-Key/间距默认值", f"{len(ini_needles)} 项"),
        check(all(needle in backlog_text for needle in modern_needles), "剧情旁路/四框绘制不变量", f"{len(modern_needles)} 项"),
        check(all(needle in bridge_text for needle in api_needles), "PadSupport Public API v1 协作", f"{len(api_needles)} 项"),
        check(no_internal_pad_scan, "不再扫描 PadSupport 内部布局", "无内部状态/VirtualQuery 扫描"),
        check(no_sdl_source, "Backlog 不自带 SDL 输入后端", "sdl_input.c/h 不存在"),
        check(no_broad_active_freeze, "synthetic 不被全局 g_active 冻结", "只冻结 live dialogue 旁路"),
        check(release_docs, "构建包携带中文文档和最新检查器", "release/docs + backlog_check.py"),
    ]

def check_document_packages(repository: Path) -> list[CheckResult]:
    """遍历全部 BACKLOG 文档；源码镜像必须一致，release 若存在也必须一致。"""

    canonical_root = repository / "docs" / "BACKLOG"
    source_mirror = repository / "src" / "backlog" / "文档"
    release_root = repository / "src" / "backlog" / "release"
    release_mirror = release_root / "docs"

    canonical = sorted(canonical_root.glob("*.md"), key=lambda path: path.name)
    canonical_names = {path.name for path in canonical}
    source_names = {path.name for path in source_mirror.glob("*.md")}

    chinese_names = all(
        not any(character.isascii() and character.isalpha() for character in path.stem)
        for path in canonical
    )

    source_matches = canonical_names == source_names
    if source_matches:
        for document in canonical:
            source_copy = source_mirror / document.name
            if hashlib.sha256(source_copy.read_bytes()).digest() != hashlib.sha256(document.read_bytes()).digest():
                source_matches = False
                break

    release_exists = release_mirror.exists()
    release_matches = True
    if release_exists:
        release_names = {path.name for path in release_mirror.glob("*.md")}
        release_matches = release_names == canonical_names
        if release_matches:
            for document in canonical:
                release_copy = release_mirror / document.name
                if hashlib.sha256(release_copy.read_bytes()).digest() != hashlib.sha256(document.read_bytes()).digest():
                    release_matches = False
                    break

    handoff_ok = (canonical_root / "完整接档说明.md").exists()
    tool_doc_ok = (canonical_root / "工具详细说明.md").exists()

    return [
        check(len(canonical) >= 10, "权威中文 Markdown 数量", f"{len(canonical)} 份"),
        check(chinese_names, "文档主文件名不含英文字母", ", ".join(path.name for path in canonical)),
        check(source_matches, "docs/BACKLOG 与源码包文档镜像一致", f"{len(canonical_names)} 份"),
        check(not release_exists or release_matches, "release 文档镜像（若已构建）一致", "未构建或逐文件一致"),
        check(handoff_ok and tool_doc_ok, "独立接档核心文档存在", "完整接档说明 + 工具详细说明"),
    ]

def result_dict(result: CheckResult) -> dict[str, object]:
    """把 dataclass 转成稳定中文 JSON 字段。"""

    return {"检查项": result.name, "通过": result.passed, "详情": result.detail}


def print_results(results: Iterable[CheckResult]) -> bool:
    """逐条打印，最终返回是否全 PASS。"""

    all_passed = True
    for result in results:
        status = "PASS" if result.passed else "FAIL"
        print(f"[{status}] {result.name}: {result.detail}")
        all_passed = all_passed and result.passed
    return all_passed


def main() -> int:
    """解析命令行、运行三组检查，并用退出码 0/1 告诉构建系统结果。"""

    script = Path(__file__).resolve()
    repository = script.parents[3]
    parser = argparse.ArgumentParser(description="Castle Backlog 静态兼容与封包检查")
    parser.add_argument("--game", type=Path, default=repository / "参考资料" / "Castle" / "exe" / "RPG.exe")
    parser.add_argument("--asi", type=Path, default=repository / "src" / "backlog" / "编译内容" / "Castle_Backlog.asi")
    parser.add_argument("--source-root", type=Path, default=repository / "src" / "backlog" / "源码")
    parser.add_argument("--json", type=Path, help="可选：把本次结果另存为 UTF-8 JSON")
    args = parser.parse_args()

    results: list[CheckResult] = []
    try:
        results.extend(check_game(args.game))
        results.extend(check_asi(args.asi))
        results.extend(check_source(args.source_root))
        results.extend(check_document_packages(repository))
    except (OSError, ValueError, struct.error) as error:
        results.append(CheckResult("检查器运行", False, str(error)))

    passed = print_results(results)
    if args.json:
        payload = {
            "工具版本": "1.1.0",
            "总体通过": passed,
            "结果": [result_dict(result) for result in results],
        }
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    print("总体结果：PASS" if passed else "总体结果：FAIL")
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())
