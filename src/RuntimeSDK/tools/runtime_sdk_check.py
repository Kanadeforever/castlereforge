#!/usr/bin/env python3
"""
RuntimeSDK 第一里程碑机械检查器。

这个工具不运行游戏，也不解析任何只读参考资料。它只检查仓库内 RuntimeSDK 自己的公开合同：
1. 必需头文件、导出表和构建入口是否存在；
2. 文档冻结的 magic、版本、结构大小和关键函数名是否真的写进头文件；
3. 公共头是否意外包含 Windows/CRT/C++ 标准库；
4. CastleRuntime.def 是否只公开允许的稳定根导出；
5. build.bat 的中文 echo/注释行是否按项目规则以两个半角空格结尾。

编译器计算 sizeof/offsetof 的职责属于 tests/abi_layout_test.c 和 build.bat。
文本检查与编译检查相互补充，不能用其中一个冒充另一个。
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


def configure_output_encoding() -> None:
    """强制管道/终端使用 UTF-8，避免简体中文 PASS 标签在非 UTF-8 系统区域下变成乱码。"""

    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="strict")
    if hasattr(sys.stderr, "reconfigure"):
        sys.stderr.reconfigure(encoding="utf-8", errors="strict")


def find_project_root(script_path: Path) -> Path:
    """从 tools/runtime_sdk_check.py 向上找到带有 src/RuntimeSDK 的仓库根目录。"""

    current = script_path.resolve().parent
    while current != current.parent:
        if (current / "src" / "RuntimeSDK").is_dir():
            return current
        current = current.parent
    raise RuntimeError("无法从检查器位置找到仓库根目录。")


class CheckContext:
    """集中记录 PASS/FAIL，确保一次运行可以报告全部问题，而不是遇到第一项就退出。"""

    def __init__(self) -> None:
        self.pass_count = 0
        self.failures: list[str] = []

    def check(self, condition: bool, label: str) -> None:
        if condition:
            self.pass_count += 1
            print(f"[PASS] {label}")
        else:
            self.failures.append(label)
            print(f"[FAIL] {label}")


def read_utf8(path: Path) -> str:
    """公共源码统一按 UTF-8 读取；解码失败应直接让工具报错，而不是替换坏字节。"""

    return path.read_text(encoding="utf-8")


def check_required_files(ctx: CheckContext, sdk_root: Path) -> dict[str, str]:
    """确认里程碑要求的文件存在，并把需要继续分析的文本加载进内存。"""

    relative_files = (
        "include/CastleRuntime_API.h",
        "include/CastlePlugin_API.h",
        "include/CastleRuntime_Client.h",
        "include/CastleHook_API.h",
        "include/CastleDisplay_API.h",
        "include/CastlePath_API.h",
        "include/CastleSchedule_API.h",
        "include/CastleWindow_API.h",
        "include/CastleRender_API.h",
        "source/CastleRuntime.def",
        "tests/abi_layout_test.c",
        "tests/runtime_host_test.c",
        "tests/entry_gate_test.c",
        "tests/client_state_test.c",
        "tests/client_bootstrap_test.c",
        "client/runtime_client.c",
        "client/runtime_entry_gate.c",
        "client/runtime_client_support.c",
        "build.bat",
        "readme.md",
        "tools/工具详细说明.md",
    )

    loaded: dict[str, str] = {}
    for relative in relative_files:
        path = sdk_root / relative
        exists = path.is_file()
        ctx.check(exists, f"必需文件存在：{relative}")
        if exists:
            loaded[relative] = read_utf8(path)
    return loaded


def check_tokens(ctx: CheckContext, loaded: dict[str, str]) -> None:
    """逐文件检查冻结常量和函数名，防止文档更新后头文件漏同步。"""

    requirements: dict[str, tuple[str, ...]] = {
        "include/CastleRuntime_API.h": (
            "CASTLE_RUNTIME_API_MAGIC        0x4E555243ul",
            "CASTLE_SIZEOF_RUNTIME_API_V1          60u",
            "BootstrapLoadedPlugins",
            "CASTLE_ERROR_ENTRY_GATE_CONFLICT",
        ),
        "include/CastlePlugin_API.h": (
            "CASTLE_PLUGIN_QUERY_MAGIC  0x52515043ul",
            "CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1 52u",
            "CASTLE_SIZEOF_CLIENT_CONFIG_V1     36u",
            "CASTLE_SIZEOF_PLUGIN_EXPORT_V1     32u",
            "client_bootstrap",
        ),
        "include/CastleRuntime_Client.h": (
            "CASTLE_RPG_ENTRY_VA_V1       0x00452C19ul",
            "CASTLE_CLIENT_LATE_LOAD_UNSUPPORTED",
            "CastleRuntimeClient_BootstrapAll",
        ),
        "include/CastleHook_API.h": (
            "CASTLE_SIZEOF_STATE_PATCH_V1          56u",
            "CASTLE_SIZEOF_HOOK_API_V1             68u",
            "AddStatePatch",
            "DescribeResource",
        ),
        "include/CastleDisplay_API.h": (
            "CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1      96u",
            "CASTLE_SIZEOF_DISPLAY_API_V1           52u",
            "WorldToScreen",
            "RegisterDisplayProvider",
        ),
        "include/CastlePath_API.h": (
            "CASTLE_PATH_INTERFACE_ID \"org.castlereforge.runtime.path\"",
            "CASTLE_SIZEOF_PATH_INFO_V1        52u",
            "CASTLE_SIZEOF_PATH_API_V1         52u",
            "GetPluginModulePathUtf8",
            "BuildPluginRelativePathWide",
            "NormalizeAbsolutePathWide",
            "PathsEqualWide",
        ),
        "include/CastleSchedule_API.h": (
            "CASTLE_SCHEDULE_INTERFACE_ID \"org.castlereforge.runtime.schedule\"",
            "CASTLE_SIZEOF_SCHEDULED_TASK_V1   48u",
            "CASTLE_SIZEOF_SCHEDULE_STATS_V1   72u",
            "CASTLE_SIZEOF_SCHEDULE_API_V1     48u",
            "UnregisterPeriodicTask",
            "PostBackgroundTask",
            "GetGamePhaseState",
        ),
        "include/CastleWindow_API.h": (
            "CASTLE_WINDOW_INTERFACE_ID \"org.castlereforge.game.window\"",
            "CASTLE_SIZEOF_WINDOW_CLIENT_V1   44u",
            "CASTLE_SIZEOF_WINDOW_API_V1      40u",
            "RegisterMessageObserver",
            "RegisterMessageFilter",
            "UnregisterWindowClient",
        ),
        "include/CastleRender_API.h": (
            "CASTLE_RENDER_INTERFACE_ID \"org.castlereforge.game.render\"",
            "CASTLE_SIZEOF_RENDER_STATE_V1    40u",
            "CASTLE_SIZEOF_RENDER_API_V1      48u",
            "BeginExtraWorldFrame",
            "RegisterRenderProvider",
            "GetRenderProviderState",
        ),
        "build.bat": (
            "runtime_crt_support.obj",
            "client\\runtime_client_support.c",
            "vswhere.exe",
            "where clang-cl.exe",
            "where python.exe",
        ),
        "client/runtime_client_support.c": (
            "__cdecl memset",
            "__cdecl memcpy",
            "volatile CastleU8*",
        ),
        "client/runtime_client.c": (
            "client_load_runtime_silently_",
            "SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX",
            "FreeLibrary(runtime_module);",
        ),
        "tests/client_bootstrap_test.c": (
            "observed_error_mode != SEM_NOGPFAULTERRORBOX",
        ),
    }

    for relative, tokens in requirements.items():
        text = loaded.get(relative, "")
        for token in tokens:
            ctx.check(token in text, f"冻结符号存在：{relative} :: {token}")

    # 只检查“脚本里出现过对象名”不够：对象可能被编译，却忘记放进 Runtime 的链接命令。
    # 这里直接定位生成 Castle_Runtime.dll 的那一行，防止 GitHub Actions 的 _memcpy 错误回归。
    build_lines = loaded.get("build.bat", "").splitlines()
    runtime_link_lines = [
        line for line in build_lines
        if "link.exe" in line.lower() and "castle_runtime.dll" in line.lower()
    ]
    ctx.check(
        len(runtime_link_lines) == 1
        and "runtime_crt_support.obj" in runtime_link_lines[0],
        "Runtime正式链接命令包含无CRT memcpy/memset对象",
    )


def remove_comments(text: str) -> str:
    """删除 C/C++ 注释，避免注释中的示例单词触发公共头依赖误报。"""

    without_blocks = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
    return re.sub(r"//.*?$", "", without_blocks, flags=re.MULTILINE)


def check_public_header_boundaries(ctx: CheckContext, loaded: dict[str, str]) -> None:
    """公共头只能依赖 SDK 自己的头，不能把实现环境泄露给第三方。"""

    for relative, text in loaded.items():
        if not relative.startswith("include/"):
            continue
        code = remove_comments(text)
        forbidden = (
            "#include <windows.h>",
            "#include <string>",
            "#include <vector>",
            "std::",
            "size_t",
            "bool ",
        )
        for token in forbidden:
            ctx.check(token not in code, f"公共头无禁止依赖：{relative} :: {token}")


def check_export_table(ctx: CheckContext, loaded: dict[str, str]) -> None:
    """稳定导出只有 CastleRuntime_GetApi；测试函数不能偷偷进入正式 ABI。"""

    text = loaded.get("source/CastleRuntime.def", "")
    export_lines: list[str] = []
    in_exports = False
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line or line.startswith(";"):
            continue
        if line.upper() == "EXPORTS":
            in_exports = True
            continue
        if in_exports:
            export_lines.append(line.split()[0])
    ctx.check(export_lines == ["CastleRuntime_GetApi"], "DEF 只导出 CastleRuntime_GetApi")


def contains_cjk(text: str) -> bool:
    """项目规则只要求中文注释/echo 行补两个空格，ASCII 行不需要额外尾随空格。"""

    return any("\u3400" <= char <= "\u9fff" for char in text)


def check_batch_trailing_spaces(ctx: CheckContext, loaded: dict[str, str]) -> None:
    """检查 build.bat 的中文 echo/rem 行，避免中文被 cmd 错误解释成下一条命令。"""

    text = loaded.get("build.bat", "")
    violations: list[int] = []
    for line_number, raw_line in enumerate(text.splitlines(), start=1):
        stripped = raw_line.lstrip().lower()
        is_comment = stripped.startswith("rem ") or stripped.startswith("::")
        is_text_echo = stripped.startswith("echo ") and stripped != "echo off"
        if contains_cjk(raw_line) and (is_comment or is_text_echo):
            if not raw_line.endswith("  "):
                violations.append(line_number)
    ctx.check(not violations, f"build.bat 中文注释/echo 行尾两个空格，违规行={violations}")


def read_u16(data: bytes, offset: int) -> int:
    """按 PE 小端格式读取 16 位整数，并在越界时抛出清晰异常。"""

    if offset < 0 or offset + 2 > len(data):
        raise ValueError(f"PE读取U16越界：0x{offset:X}")
    return int.from_bytes(data[offset : offset + 2], "little")


def read_u32(data: bytes, offset: int) -> int:
    """按 PE 小端格式读取 32 位整数。"""

    if offset < 0 or offset + 4 > len(data):
        raise ValueError(f"PE读取U32越界：0x{offset:X}")
    return int.from_bytes(data[offset : offset + 4], "little")


def read_c_string(data: bytes, offset: int) -> str:
    """读取 PE 表中的 ASCII NUL 字符串，拒绝越界或非 ASCII 名称。"""

    if offset < 0 or offset >= len(data):
        raise ValueError(f"PE字符串偏移越界：0x{offset:X}")
    end = data.find(b"\0", offset)
    if end < 0:
        raise ValueError(f"PE字符串没有结尾NUL：0x{offset:X}")
    return data[offset:end].decode("ascii")


def rva_to_file_offset(data: bytes, pe_offset: int, rva: int) -> int:
    """使用节表把内存 RVA 转换成文件偏移。"""

    section_count = read_u16(data, pe_offset + 6)
    optional_size = read_u16(data, pe_offset + 20)
    section_table = pe_offset + 24 + optional_size

    for index in range(section_count):
        section = section_table + index * 40
        virtual_size = read_u32(data, section + 8)
        virtual_address = read_u32(data, section + 12)
        raw_size = read_u32(data, section + 16)
        raw_offset = read_u32(data, section + 20)
        span = max(virtual_size, raw_size)
        if virtual_address <= rva < virtual_address + span:
            file_offset = raw_offset + (rva - virtual_address)
            if file_offset >= len(data):
                raise ValueError(f"RVA映射结果越界：0x{rva:X}")
            return file_offset
    raise ValueError(f"RVA不属于任何节：0x{rva:X}")


def parse_export_names(data: bytes, pe_offset: int, optional_offset: int) -> list[str]:
    """解析 IMAGE_EXPORT_DIRECTORY 中按名称导出的符号。"""

    export_rva = read_u32(data, optional_offset + 96)
    if export_rva == 0:
        return []
    export_offset = rva_to_file_offset(data, pe_offset, export_rva)
    name_count = read_u32(data, export_offset + 24)
    names_rva = read_u32(data, export_offset + 32)
    names_offset = rva_to_file_offset(data, pe_offset, names_rva)

    names: list[str] = []
    for index in range(name_count):
        name_rva = read_u32(data, names_offset + index * 4)
        names.append(read_c_string(data, rva_to_file_offset(data, pe_offset, name_rva)))
    return sorted(names)


def parse_import_dlls(data: bytes, pe_offset: int, optional_offset: int) -> list[str]:
    """解析 IMAGE_IMPORT_DESCRIPTOR，只关心 DLL 名，不需要解析每个函数。"""

    import_rva = read_u32(data, optional_offset + 104)
    if import_rva == 0:
        return []
    descriptor = rva_to_file_offset(data, pe_offset, import_rva)
    names: list[str] = []

    while True:
        fields = [read_u32(data, descriptor + index * 4) for index in range(5)]
        if all(value == 0 for value in fields):
            break
        name_rva = fields[3]
        names.append(read_c_string(data, rva_to_file_offset(data, pe_offset, name_rva)))
        descriptor += 20
    return sorted(names, key=str.lower)


def check_runtime_pe(ctx: CheckContext, sdk_root: Path, required: bool) -> None:
    """验证刚链接的 Runtime DLL 架构、入口、唯一导出和静态依赖。"""

    dll_path = sdk_root / "_build" / "Castle_Runtime.dll"
    if not dll_path.is_file():
        ctx.check(not required, "Runtime DLL存在（仅--require-dll模式强制）")
        return

    try:
        data = dll_path.read_bytes()
        if len(data) < 128 or data[:2] != b"MZ":
            raise ValueError("文件不是有效MZ映像")
        pe_offset = read_u32(data, 0x3C)
        if data[pe_offset : pe_offset + 4] != b"PE\0\0":
            raise ValueError("PE签名无效")
        machine = read_u16(data, pe_offset + 4)
        characteristics = read_u16(data, pe_offset + 22)
        optional_offset = pe_offset + 24
        optional_magic = read_u16(data, optional_offset)
        entry_rva = read_u32(data, optional_offset + 16)
        exports = parse_export_names(data, pe_offset, optional_offset)
        imports = parse_import_dlls(data, pe_offset, optional_offset)

        ctx.check(machine == 0x014C, "Runtime PE机器类型为i386")
        ctx.check(optional_magic == 0x010B, "Runtime OptionalHeader为PE32")
        ctx.check((characteristics & 0x2000) != 0, "Runtime具有DLL标志")
        ctx.check(entry_rva != 0, "Runtime入口RVA非零")
        ctx.check(exports == ["CastleRuntime_GetApi"], f"Runtime唯一导出正确：{exports}")
        ctx.check([name.lower() for name in imports] == ["kernel32.dll"],
                  f"Runtime静态依赖仅KERNEL32：{imports}")
    except (OSError, UnicodeError, ValueError) as error:
        ctx.check(False, f"Runtime PE解析成功：{error}")


def check_release_artifacts(ctx: CheckContext, project_root: Path, required: bool) -> None:
    """检查 build 交付树：标准导出、PE32 形状、随包说明和零编译垃圾。"""

    if not required:
        return
    release_root = project_root / "build"
    asi_root = release_root / "mods" / "asi"
    expected_exports: dict[str, list[str]] = {
        "Castle_Runtime.dll": ["CastleRuntime_GetApi"],
        "Castle_Backlog.asi": ["CastlePlugin_Query", "InitializeASI"],
        "Castle_PadSupport.asi": ["CastlePad_GetApi", "CastlePlugin_Query", "InitializeASI"],
        "Castle_SaveEnhance.asi": ["CastlePlugin_Query", "InitializeASI"],
        "Castle_Widescreen.asi": ["CastlePlugin_Query", "InitializeASI"],
        "BUGFix.asi": ["CastlePlugin_Query", "InitializeASI"],
        "NoCD.asi": ["CastlePlugin_Query", "InitializeASI"],
        "MaxGrowthAndDrop.asi": ["CastlePlugin_Query", "InitializeASI"],
    }

    for name, expected in expected_exports.items():
        path = asi_root / name
        ctx.check(path.is_file(), f"发行文件存在：build/mods/asi/{name}")
        if not path.is_file():
            continue
        try:
            data = path.read_bytes()
            pe_offset = read_u32(data, 0x3C)
            optional_offset = pe_offset + 24
            machine = read_u16(data, pe_offset + 4)
            characteristics = read_u16(data, pe_offset + 22)
            optional_magic = read_u16(data, optional_offset)
            entry_rva = read_u32(data, optional_offset + 16)
            exports = parse_export_names(data, pe_offset, optional_offset)
            ctx.check(machine == 0x014C and optional_magic == 0x010B,
                      f"发行二进制为PE32/i386：{name}")
            ctx.check((characteristics & 0x2000) != 0 and entry_rva != 0,
                      f"发行二进制具有DLL标志和非零入口：{name}")
            ctx.check(exports == sorted(expected),
                      f"发行导出表正确：{name} :: {exports}")
        except (OSError, UnicodeError, ValueError) as error:
            ctx.check(False, f"发行PE解析成功：{name} :: {error}")

    packaged_docs = (
        asi_root / "Castle_SaveEnhance" / "音效放置与INI配置说明.md",
        asi_root / "Castle_SaveEnhance" / "SaveEnhance实机测试清单.md",
    )
    for path in packaged_docs:
        ctx.check(path.is_file(), f"SaveEnhance随包说明存在：{path.name}")

    garbage_suffixes = {".obj", ".lib", ".exp", ".ilk", ".pdb"}
    garbage = sorted(
        str(path.relative_to(release_root))
        for path in release_root.rglob("*")
        if path.is_file() and path.suffix.lower() in garbage_suffixes
    ) if release_root.is_dir() else []
    ctx.check(not garbage, f"build发行目录不含编译垃圾：{garbage}")


def main() -> int:
    configure_output_encoding()

    try:
        project_root = find_project_root(Path(__file__))
    except RuntimeError as error:
        print(f"[FAIL] {error}")
        return 2

    sdk_root = project_root / "src" / "RuntimeSDK"
    ctx = CheckContext()

    print(f"[信息] RuntimeSDK目录：{sdk_root}")
    loaded = check_required_files(ctx, sdk_root)
    check_tokens(ctx, loaded)
    check_public_header_boundaries(ctx, loaded)
    check_export_table(ctx, loaded)
    check_batch_trailing_spaces(ctx, loaded)
    check_runtime_pe(ctx, sdk_root, "--require-dll" in sys.argv[1:])
    check_release_artifacts(ctx, project_root, "--require-release" in sys.argv[1:])

    print()
    print(f"[汇总] PASS={ctx.pass_count} FAIL={len(ctx.failures)}")
    if ctx.failures:
        print("[失败项]")
        for failure in ctx.failures:
            print(f"- {failure}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
