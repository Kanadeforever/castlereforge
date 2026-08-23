#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
WiseTreeExtractor

用于 1990 年代末 / 2000 年代初 Wise Installation System 安装程序的
“目录树还原”解包器。针对本项目中的台湾 2001 年 Wise 安装程序：

- 不执行安装程序本身；
- 直接解析 PE overlay 中连续的 raw-DEFLATE 数据流；
- 从 Wise 安装信息流中读取 Install File 记录；
- 按 Windows 台湾 Big5 代码页 CP950 解码目标路径；
- 用文件大小 + CRC32 将安装目标路径与 payload 精确配对；
- 重建 %MAINDIR% 下的完整目录树，因此不会发生 /X 模式的“全部落在 root”问题；
- 同名文件位于不同目录时可以全部保留。

用法（编译成 EXE 后同样适用）：
    把一个或多个 Wise 安装程序 .exe 拖到本工具上。

命令行：
    WiseTreeExtractor.exe setup.exe [setup2.exe ...]

输出：
    <安装程序名>_Extracted\
    <安装程序名>_extract_report.txt
"""

from __future__ import annotations

import binascii
import os
import shutil
import struct
import sys
import zlib
from collections import defaultdict
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Tuple


VERSION = "1.0"
PATH_ENCODING = "cp950"  # Windows 台湾 Big5（代码页 950）
OUTPUT_SUFFIX = "_Extracted"
REPORT_SUFFIX = "_extract_report.txt"
CHUNK_SIZE = 1024 * 1024

WISE_TEXT_MARKER = b"Initializing Wise Installation Wizard..."
MAINDIR_MARKER = b"%MAINDIR%\\"

# 本批 2001 年 Wise Install File 记录的稳定结构：
# 目标路径前 44 字节处可见 00 00 80 00；
# 路径前 28 字节是未压缩文件大小；
# 路径前 4 字节是文件 CRC32。
INSTALL_FILE_HEADER_DISTANCE = 44
INSTALL_FILE_HEADER_MAGIC = b"\x00\x00\x80\x00"
INSTALL_FILE_SIZE_DISTANCE = 28
INSTALL_FILE_CRC_DISTANCE = 4


@dataclass(frozen=True)
class FileRecord:
    order: int
    script_offset: int
    raw_path: bytes
    decoded_path: str
    relative_path: Path
    size: int
    crc32: int

    @property
    def key(self) -> Tuple[int, int]:
        return self.size, self.crc32


@dataclass
class StreamResult:
    index: int
    start: int
    end: int
    crc_position: int
    size: int
    crc32: int
    data: bytes

    @property
    def key(self) -> Tuple[int, int]:
        return self.size, self.crc32


class ExtractError(RuntimeError):
    pass


def configure_console() -> None:
    """让 Windows 控制台尽量正确显示中文；不影响实际文件名编码。"""
    if sys.platform == "win32":
        try:
            import ctypes

            ctypes.windll.kernel32.SetConsoleOutputCP(65001)
            ctypes.windll.kernel32.SetConsoleCP(65001)
        except Exception:
            pass

    for stream_name in ("stdout", "stderr"):
        stream = getattr(sys, stream_name, None)
        if stream is not None and hasattr(stream, "reconfigure"):
            try:
                stream.reconfigure(encoding="utf-8", errors="replace")
            except Exception:
                pass


def wait_before_exit() -> None:
    if sys.stdin is not None and sys.stdin.isatty():
        try:
            input("\n按回车退出...")
        except (EOFError, KeyboardInterrupt):
            pass


def log(fp, text: str = "") -> None:
    print(text)
    fp.write(text + "\n")
    fp.flush()


def read_u16(data: bytes, offset: int) -> int:
    if offset < 0 or offset + 2 > len(data):
        raise ExtractError(f"读取 U16 越界：0x{offset:X}")
    return struct.unpack_from("<H", data, offset)[0]


def read_u32(data: bytes, offset: int) -> int:
    if offset < 0 or offset + 4 > len(data):
        raise ExtractError(f"读取 U32 越界：0x{offset:X}")
    return struct.unpack_from("<I", data, offset)[0]


def find_pe_overlay(data: bytes) -> int:
    if len(data) < 0x40 or data[:2] != b"MZ":
        raise ExtractError("输入文件不是有效的 PE/MZ 可执行文件。")

    pe_offset = read_u32(data, 0x3C)
    if pe_offset + 24 > len(data) or data[pe_offset:pe_offset + 4] != b"PE\x00\x00":
        raise ExtractError("找不到有效 PE 头。")

    section_count = read_u16(data, pe_offset + 6)
    optional_header_size = read_u16(data, pe_offset + 20)
    section_table = pe_offset + 24 + optional_header_size

    if section_count <= 0 or section_table + section_count * 40 > len(data):
        raise ExtractError("PE 节表无效。")

    overlay_start = 0
    for i in range(section_count):
        off = section_table + i * 40
        raw_size = read_u32(data, off + 16)
        raw_offset = read_u32(data, off + 20)
        overlay_start = max(overlay_start, raw_offset + raw_size)

    if overlay_start <= 0 or overlay_start >= len(data):
        raise ExtractError("文件没有可识别的 PE overlay。")

    return overlay_start


def inflate_one(data: bytes, start: int) -> Tuple[bytes, int]:
    """
    从 start 解一个 raw-DEFLATE 流。
    返回 (未压缩数据, DEFLATE 流结束位置)。

    分块喂给 zlib，避免每个流都复制“当前位置到文件末尾”的巨大切片。
    """
    if start < 0 or start >= len(data):
        raise ExtractError(f"压缩流起点越界：0x{start:X}")

    dec = zlib.decompressobj(-15)
    out_parts: List[bytes] = []
    cursor = start

    while cursor < len(data):
        chunk_end = min(cursor + CHUNK_SIZE, len(data))
        chunk = data[cursor:chunk_end]

        try:
            out = dec.decompress(chunk)
        except zlib.error as exc:
            raise ExtractError(
                f"raw-DEFLATE 解压失败 @ 0x{start:X}: {exc}"
            ) from exc

        if out:
            out_parts.append(out)

        if dec.eof:
            consumed_in_chunk = len(chunk) - len(dec.unused_data)
            end = cursor + consumed_in_chunk
            return b"".join(out_parts), end

        cursor = chunk_end

    raise ExtractError(f"raw-DEFLATE 流没有正常结束 @ 0x{start:X}")


def locate_crc(data: bytes, nominal_end: int, crc32_value: int) -> Optional[int]:
    """
    Wise 老版本在个别流边界上存在 1~3 字节偏移现象。
    先检查正常位置，再按旧 Wise 解包器常见兼容范围检查附近位置。
    """
    for delta in (0, -3, -2, -1, 1, 2, 3):
        pos = nominal_end + delta
        if 0 <= pos <= len(data) - 4 and read_u32(data, pos) == crc32_value:
            return pos
    return None


def validate_stream_candidate(data: bytes, start: int) -> bool:
    try:
        payload, end = inflate_one(data, start)
    except ExtractError:
        return False

    if len(payload) < 64:
        return False

    crc = binascii.crc32(payload) & 0xFFFFFFFF
    return locate_crc(data, end, crc) is not None


def find_first_wise_stream(data: bytes, overlay_start: int) -> int:
    """
    本批 Wise PE 的文本头后紧跟第一个 raw-DEFLATE 流。
    优先从 "Initializing Wise Installation Wizard..." 后开始找，
    失败时只在 overlay 开头的小范围内兜底扫描。
    """
    search_end = min(len(data), overlay_start + 8192)
    marker_pos = data.find(WISE_TEXT_MARKER, overlay_start, search_end)

    candidates: List[int] = []

    if marker_pos >= 0:
        nul = data.find(b"\x00", marker_pos, search_end)
        if nul >= 0:
            # 正常样本就是 nul+1；少量格式可能还有数十字节附加字段。
            candidates.extend(range(nul + 1, min(nul + 257, search_end)))

    # 兜底：仅扫描 overlay 前 2 KiB，避免对整包做昂贵逐字节试解。
    candidates.extend(
        range(overlay_start, min(overlay_start + 2048, search_end))
    )

    seen = set()
    for pos in candidates:
        if pos in seen:
            continue
        seen.add(pos)
        if validate_stream_candidate(data, pos):
            return pos

    raise ExtractError(
        "找不到 Wise raw-DEFLATE 数据流。"
        "该安装程序可能不是本批 Wise 格式，或使用了不同压缩模式。"
    )


def iter_wise_streams(data: bytes, first_stream: int) -> Iterable[StreamResult]:
    pos = first_stream
    index = 0

    while pos < len(data):
        payload, nominal_end = inflate_one(data, pos)
        crc = binascii.crc32(payload) & 0xFFFFFFFF
        crc_pos = locate_crc(data, nominal_end, crc)

        if crc_pos is None:
            raise ExtractError(
                f"第 {index} 个 Wise 数据流 CRC 边界无法确认："
                f"stream=0x{pos:X}, nominal_end=0x{nominal_end:X}, CRC={crc:08X}"
            )

        yield StreamResult(
            index=index,
            start=pos,
            end=nominal_end,
            crc_position=crc_pos,
            size=len(payload),
            crc32=crc,
            data=payload,
        )

        next_pos = crc_pos + 4
        if next_pos <= pos:
            raise ExtractError("Wise 数据流位置没有前进，停止以避免死循环。")

        pos = next_pos
        index += 1


def decode_big5_path(raw_path: bytes, script_offset: int) -> str:
    """
    2001 台湾 Windows 安装包使用 CP950（Windows Big5）。

    必须先把整条路径按 CP950 解成 Unicode，再处理反斜杠。
    Big5 的第二字节本身可能是 0x5C，因此不能先对原始 bytes 做 split(b'\\')。
    """
    try:
        return raw_path.decode(PATH_ENCODING, errors="strict")
    except UnicodeDecodeError as exc:
        sample = raw_path.hex(" ")
        raise ExtractError(
            f"Big5/CP950 路径解码失败 @ script+0x{script_offset:X}。\n"
            f"原始字节：{sample}\n"
            "为避免生成乱码文件名，本工具已停止，而不是用替换字符继续写盘。"
        ) from exc


def make_safe_relative_path(decoded_path: str) -> Path:
    prefix = "%MAINDIR%\\"
    if not decoded_path.upper().startswith(prefix):
        raise ExtractError(f"不是 %MAINDIR% 路径：{decoded_path}")

    relative = decoded_path[len(prefix):].replace("/", "\\")
    raw_parts = relative.split("\\")
    parts: List[str] = []

    invalid_chars = set('<>:"|?*')

    for part in raw_parts:
        if part in ("", "."):
            continue
        if part == "..":
            raise ExtractError(f"拒绝越出输出目录的目标路径：{decoded_path}")
        if any(ch in invalid_chars for ch in part):
            raise ExtractError(f"目标文件名含 Windows 非法字符：{decoded_path}")
        if "\x00" in part:
            raise ExtractError(f"目标文件名含 NUL：{decoded_path}")
        parts.append(part)

    if not parts:
        raise ExtractError(f"空目标路径：{decoded_path}")

    return Path(*parts)


def parse_install_file_records(script: bytes) -> List[FileRecord]:
    """
    解析本批 Wise 的 Install File 记录。

    这是结构识别，不是“见到 %MAINDIR% 字符串就猜”：
    只有路径前 44 字节处存在 00 00 80 00 的记录才视为文件安装项，
    并直接读取该记录中的未压缩大小与 CRC32。
    """
    records: List[FileRecord] = []
    search_pos = 0

    while True:
        off = script.find(MAINDIR_MARKER, search_pos)
        if off < 0:
            break

        end = script.find(b"\x00", off)
        if end < 0:
            break

        search_pos = end + 1

        if off < INSTALL_FILE_HEADER_DISTANCE:
            continue

        magic_start = off - INSTALL_FILE_HEADER_DISTANCE
        if script[magic_start:magic_start + 4] != INSTALL_FILE_HEADER_MAGIC:
            continue

        size_off = off - INSTALL_FILE_SIZE_DISTANCE
        crc_off = off - INSTALL_FILE_CRC_DISTANCE
        if size_off < 0 or crc_off < 0:
            continue

        raw_path = script[off:end]
        decoded = decode_big5_path(raw_path, off)
        relative = make_safe_relative_path(decoded)
        size = read_u32(script, size_off)
        crc = read_u32(script, crc_off)

        records.append(
            FileRecord(
                order=len(records),
                script_offset=off,
                raw_path=raw_path,
                decoded_path=decoded,
                relative_path=relative,
                size=size,
                crc32=crc,
            )
        )

    return records


def looks_like_archive_info(payload: bytes) -> bool:
    # 至少要有一个“结构正确的 Install File 记录”才认作安装信息流。
    marker_pos = payload.find(MAINDIR_MARKER)
    while marker_pos >= 0:
        if (
            marker_pos >= INSTALL_FILE_HEADER_DISTANCE
            and payload[
                marker_pos - INSTALL_FILE_HEADER_DISTANCE:
                marker_pos - INSTALL_FILE_HEADER_DISTANCE + 4
            ] == INSTALL_FILE_HEADER_MAGIC
        ):
            return True
        marker_pos = payload.find(MAINDIR_MARKER, marker_pos + 1)
    return False


def extract_installer(installer: Path) -> bool:
    installer = installer.resolve()
    output_dir = installer.parent / f"{installer.stem}{OUTPUT_SUFFIX}"
    report_path = installer.parent / f"{installer.stem}{REPORT_SUFFIX}"

    with open(report_path, "w", encoding="utf-8", newline="\r\n") as report:
        log(report, f"WiseTreeExtractor v{VERSION}")
        log(report, "Wise 安装程序目录树还原报告")
        log(report, f"时间：{datetime.now():%Y-%m-%d %H:%M:%S}")
        log(report, f"安装程序：{installer}")
        log(report, f"路径编码：{PATH_ENCODING}（Windows 台湾 Big5）")
        log(report, f"输出目录：{output_dir}")
        log(report, "方式：直接解析 Wise 数据，不执行安装程序，不使用 /X。")
        log(report)

        try:
            if not installer.is_file():
                raise ExtractError("文件不存在。")
            if installer.suffix.casefold() != ".exe":
                raise ExtractError("只接受 .exe 安装程序。")

            data = installer.read_bytes()
            log(report, f"EXE 大小：{len(data):,} bytes")

            overlay_start = find_pe_overlay(data)
            log(report, f"PE overlay：0x{overlay_start:X}")

            if data.find(b"WiseMain", 0, min(len(data), overlay_start)) < 0 and \
               data.find(WISE_TEXT_MARKER, overlay_start, min(len(data), overlay_start + 8192)) < 0:
                raise ExtractError("没有检测到预期的 Wise Installation System 特征。")

            first_stream = find_first_wise_stream(data, overlay_start)
            log(report, f"首个 Wise 压缩流：0x{first_stream:X}")

            records: Optional[List[FileRecord]] = None
            record_keys: set[Tuple[int, int]] = set()
            payload_by_key: Dict[Tuple[int, int], bytes] = {}
            pre_info_streams: List[StreamResult] = []
            stream_count = 0
            info_stream_index: Optional[int] = None

            for stream in iter_wise_streams(data, first_stream):
                stream_count += 1

                if records is None:
                    if looks_like_archive_info(stream.data):
                        parsed = parse_install_file_records(stream.data)
                        if not parsed:
                            raise ExtractError(
                                "找到了疑似安装信息流，但没有解析出 Install File 记录。"
                            )

                        records = parsed
                        info_stream_index = stream.index
                        record_keys = {rec.key for rec in records}

                        log(report, f"安装信息流：stream #{stream.index}")
                        log(report, f"Install File 记录：{len(records)}")
                        log(report)

                        # 理论上 payload 位于信息流之后；仍检查此前流，避免特殊包漏项。
                        for old in pre_info_streams:
                            if old.key in record_keys and old.key not in payload_by_key:
                                payload_by_key[old.key] = old.data
                        pre_info_streams.clear()

                        if stream.key in record_keys and stream.key not in payload_by_key:
                            payload_by_key[stream.key] = stream.data
                    else:
                        # 安装信息通常很早出现，因此这里只会暂存少量小型支持流。
                        pre_info_streams.append(stream)
                else:
                    if stream.key in record_keys and stream.key not in payload_by_key:
                        payload_by_key[stream.key] = stream.data

            if records is None:
                raise ExtractError("没有找到可解析的 Wise Install File 记录表。")

            missing_records = [rec for rec in records if rec.key not in payload_by_key]
            if missing_records:
                log(report, "[失败] 以下文件记录找不到对应 payload：")
                for rec in missing_records[:50]:
                    log(
                        report,
                        f"  {rec.decoded_path}  size={rec.size} CRC={rec.crc32:08X}"
                    )
                if len(missing_records) > 50:
                    log(report, f"  ……另有 {len(missing_records) - 50} 项")
                raise ExtractError(
                    f"有 {len(missing_records)} 个文件记录无法与压缩数据匹配。"
                )

            # 所有映射都验证完成后才动旧输出目录，避免解析失败时破坏上次结果。
            if output_dir.exists():
                shutil.rmtree(output_dir)
            output_dir.mkdir(parents=True, exist_ok=True)

            written_paths: Dict[str, Tuple[int, int]] = {}
            written_count = 0
            overwrite_count = 0

            log(report, "========== 文件 ==========")

            for rec in records:
                payload = payload_by_key[rec.key]

                # 双重验证，防止后续代码改动造成静默错误。
                actual_size = len(payload)
                actual_crc = binascii.crc32(payload) & 0xFFFFFFFF
                if actual_size != rec.size or actual_crc != rec.crc32:
                    raise ExtractError(
                        f"payload 校验异常：{rec.decoded_path}"
                    )

                dest = output_dir / rec.relative_path
                dest.parent.mkdir(parents=True, exist_ok=True)

                path_key = str(rec.relative_path).casefold()
                if path_key in written_paths:
                    overwrite_count += 1
                    action = "覆盖"
                else:
                    action = "写入"

                with open(dest, "wb") as fp:
                    fp.write(payload)

                written_paths[path_key] = rec.key
                written_count += 1

                log(
                    report,
                    f"[{action}] {rec.relative_path}  "
                    f"size={rec.size} CRC={rec.crc32:08X}"
                )

            log(report)
            log(report, "========== 汇总 ==========")
            log(report, f"Wise 数据流总数：{stream_count}")
            log(report, f"安装信息流编号：{info_stream_index}")
            log(report, f"Install File 记录：{len(records)}")
            log(report, f"实际写入次数：{written_count}")
            log(report, f"同一路径后项覆盖：{overwrite_count}")
            log(report, f"唯一输出路径：{len(written_paths)}")
            log(report, f"Big5/CP950 解码错误：0")
            log(report, "CRC/大小未匹配：0")
            log(report, f"输出：{output_dir}")
            log(report)
            log(report, "[成功] 已按 Wise 原安装目标重建 %MAINDIR% 目录树。")

            return True

        except Exception as exc:
            log(report)
            log(report, f"[失败] {type(exc).__name__}: {exc}")
            return False


def main() -> int:
    configure_console()

    args = [Path(arg) for arg in sys.argv[1:] if arg.strip()]

    if not args:
        print(f"WiseTreeExtractor v{VERSION}")
        print("Wise 安装程序目录树还原器")
        print()
        print("把一个或多个旧 Wise 安装程序 EXE 拖到本工具上即可。")
        print("本工具直接解析安装包，不会执行目标 EXE。")
        print(f"台湾繁体路径按 {PATH_ENCODING} / Windows Big5 解码。")
        print()
        wait_before_exit()
        return 0

    success = 0
    failed = 0

    print(f"WiseTreeExtractor v{VERSION}")
    print("=" * 68)

    for index, installer in enumerate(args, 1):
        print()
        print(f"[{index}/{len(args)}] {installer}")
        print("-" * 68)

        if extract_installer(installer):
            success += 1
            print("[OK] 完成")
        else:
            failed += 1
            print("[FAIL] 请查看同目录报告")

    print()
    print("=" * 68)
    print(f"全部完成：成功 {success}，失败 {failed}")

    wait_before_exit()
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
