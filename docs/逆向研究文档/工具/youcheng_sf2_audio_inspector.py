#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》SF2 Section3 嵌入 PCM / RIFF 调查器。

这个工具只做“读取、验证、导出证据”，不会修改游戏文件。
它专门固化固化37已经由 RPG.exe 机器码证明的音频语义：

1. Section3 +0x00 是声道数（u8）；
2. Section3 +0x01 是每样本位数（u8）；
3. Section3 +0x02 是采样率（u16 little-endian）；
4. Section3 +0x04 是裸 PCM 数据字节数（u32 little-endian）；
5. Section3 +0x08 / +0x0C 是当前全语料均为 0 的保留字段；
6. Section3 +0x10 开始是裸 PCM 数据；
7. 原版 0x4057D0 会构造标准 44 字节 RIFF/WAVE PCM 头，并把裸 PCM 原样复制到 data 块；
8. 原版通过 BASS_SampleLoad 的内存文件分支加载这份临时 WAV。

重要：
- 本工具的源码文件名保持英文；
- 代码内使用非常详细的简体中文注释，尽量让只学过一天编程的人也能逐步看懂；
- 本工具依赖同目录中的两个已经随固化包交付的稳定调查器，避免复制两套 SF2/DAT 解析逻辑后产生分叉。
"""

from __future__ import annotations

# argparse 用来制作命令行子命令，例如“检查”“批量”“验证RPG”“导出”。
import argparse
# hashlib 用来计算 SHA-256；这样同一份资源即使在不同目录出现，也可以可靠去重。
import hashlib
# importlib.util 允许我们按“同目录文件路径”加载已有工具，而不是要求用户额外安装 Python 包。
import importlib.util
# json 用来生成机器可读的调查报告，方便后续固化节点做自动回归。
import json
# struct 用来按小端格式读取/写入 u16、u32，也用于精确构造 RIFF/WAVE 头。
import struct
# sys 用来返回标准退出码，并处理同目录模块加载失败时的错误输出。
import sys
# collections.Counter 用来统计格式、采样率、引用次数等分布。
from collections import Counter
# dataclass 让“一份 SF2 的来源”有清晰字段，避免到处传递含义不明的元组。
from dataclasses import dataclass
# pathlib.Path 让 Windows/Linux 路径拼接都更清楚，也能直接读写文件。
from pathlib import Path
# Any / Iterable 只用于类型提示，帮助读代码的人知道变量大概是什么形态。
from typing import Any, Iterable


# 工具版本与固化节点绑定。后续若修改工具逻辑，应同步提升版本号并更新中文工具说明。
TOOL_VERSION = "0.1.0-固化37"

# 标准 PCM RIFF/WAVE 文件头固定是 44 字节：
# RIFF 12 字节 + fmt 块 24 字节 + data 块头 8 字节 = 44 字节。
WAVE_HEADER_SIZE = 44

# 这里保存当前项目已经遇到并由全语料验证过的声道数。
# 工具不会因为未来出现别的声道数就擅自“修正”，而是把它报告成未知/不支持。
KNOWN_CHANNELS = {1, 2}

# 这里保存当前项目已经遇到并由原版 RIFF 构造链证明可直接送入 PCM WAVE 的位深。
KNOWN_BITS = {8, 16}


@dataclass(frozen=True)
class Sf2Source:
    """表示一份 SF2 的原始字节和它来自哪里。"""

    # display_path 是给人看的来源，例如“fight/Audio/sound02.SF2”或“某DAT::容器内路径”。
    display_path: str
    # raw 是该 SF2 在磁盘/容器中的原始字节；压缩 SF2 仍然保留压缩前的原始形态。
    raw: bytes
    # source_kind 只区分“散装文件”和“资源容器内嵌文件”，便于统计语料覆盖范围。
    source_kind: str


# 先取得“当前脚本所在目录”。因为三个工具都放在包内“工具”目录，所以这是最稳定的相对位置。
_THIS_DIR = Path(__file__).resolve().parent


def _load_sibling_module(module_name: str, filename: str) -> Any:
    """按文件路径加载同目录工具模块，并给出明确错误。"""

    # 拼出目标 Python 文件的绝对路径。
    module_path = _THIS_DIR / filename
    # 如果文件不存在，说明用户只拷了单个工具而没有保留完整固化包；这里要直接说明，而不是报神秘 ImportError。
    if not module_path.is_file():
        raise RuntimeError(f"缺少同目录依赖工具：{module_path}")
    # 根据文件路径创建“模块加载规格”；它告诉 Python 这个文件要以什么模块名临时载入。
    spec = importlib.util.spec_from_file_location(module_name, module_path)
    # 极少数情况下 Python 无法为这个文件创建 loader；显式检查能让错误更容易理解。
    if spec is None or spec.loader is None:
        raise RuntimeError(f"无法为依赖工具创建模块加载器：{module_path}")
    # 根据规格创建空模块对象；下一步 exec_module 才会真正执行文件并填入函数/常量。
    module = importlib.util.module_from_spec(spec)
    # 执行模块。这里只执行工具定义；两个依赖工具都有 if __name__ == '__main__' 保护，不会因此跑命令行主程序。
    spec.loader.exec_module(module)
    # 返回已经加载好的模块，后面就能调用其中经过前几轮固化验证的 SF2/DAT 解析函数。
    return module


# 加载标准 SF2 调查器。固化37刻意复用它已经验证过的 0x6F 解压和四 Section 指针表解析。
_sf2_tool = _load_sibling_module("youcheng_eve_sf2_inspector_g37", "youcheng_eve_sf2_inspector.py")
# 加载 DAT/SYS 资源容器调查器。这样“批量”命令可以同时覆盖散装 SF2 和九个已确认资源容器里的 SF2。
_container_tool = _load_sibling_module("youcheng_resource_container_inspector_g37", "youcheng_resource_container_inspector.py")


def sha256_bytes(data: bytes) -> str:
    """计算一段字节的 SHA-256 十六进制字符串。"""

    # hashlib.sha256(data) 先计算 256 位摘要，hexdigest() 再把它变成人可复制的 64 个十六进制字符。
    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """计算一个文件的 SHA-256。"""

    # 研究对象 RPG.exe 只有几百 KB，直接一次读入内存最简单，也不会造成实际内存压力。
    return sha256_bytes(path.read_bytes())


def _u16(data: bytes, offset: int) -> int:
    """从 data 的指定偏移读取 little-endian 无符号 16 位整数。"""

    # '<H' 的 '<' 表示 little-endian，'H' 表示 unsigned short（2 字节）。
    return struct.unpack_from("<H", data, offset)[0]


def _u32(data: bytes, offset: int) -> int:
    """从 data 的指定偏移读取 little-endian 无符号 32 位整数。"""

    # '<I' 的 'I' 表示 unsigned int（4 字节）。
    return struct.unpack_from("<I", data, offset)[0]


def _i16(data: bytes, offset: int) -> int:
    """从 data 的指定偏移读取 little-endian 有符号 16 位整数。"""

    # Section0 +0x20 的音频索引必须按有符号数解释，因为原版使用 movsx，并以 -1 表示“没有音频”。
    return struct.unpack_from("<h", data, offset)[0]


def parse_section3_record(record: bytes, index: int) -> dict[str, Any]:
    """按固化37已证明的真实字段语义解析一条 Section3 记录。"""

    # 每条 Section3 至少要有 16 字节自定义头；再短就连 data_size 字段都不完整，不能继续。
    if len(record) < 16:
        raise ValueError(f"Section3 记录 {index} 只有 {len(record)} 字节，短于 16 字节固定头")

    # 原版两个播放器 caller 都用 BYTE PTR [record+0] 取这里，所以 +0x00 是独立的 u8 声道数。
    channels = record[0]
    # 原版两个播放器 caller 都用 BYTE PTR [record+1] 取这里，所以 +0x01 是独立的 u8 位深。
    bits_per_sample = record[1]
    # 原版用 WORD PTR [record+2] 取采样率，所以这里是 little-endian u16。
    sample_rate = _u16(record, 2)
    # 原版用 DWORD PTR [record+4] 取 PCM 数据大小，所以这里是 little-endian u32。
    data_size = _u32(record, 4)
    # +0x08 / +0x0C 当前全语料都是 0；仍原样读取并报告，未来若出现非零值不会被工具吃掉。
    reserved0 = _u32(record, 8)
    reserved1 = _u32(record, 12)
    # 固定头之后的所有字节就是原版 memcpy 到 RIFF data 块的 PCM payload。
    payload = record[16:]

    # 一个采样帧包含“所有声道各一个 sample”。例如 16-bit stereo 是 2 声道 × 2 字节 = 4 字节一帧。
    # 只有位深能整除 8 时，这个简单公式才有意义；异常数据时保持 0，避免误报。
    bytes_per_sample = bits_per_sample // 8 if bits_per_sample % 8 == 0 else 0
    block_align = channels * bytes_per_sample if channels > 0 and bytes_per_sample > 0 else 0

    # 原版 RIFF 构造器会写 nAvgBytesPerSec = sample_rate * block_align。
    avg_bytes_per_sec = sample_rate * block_align if block_align else 0
    # data_size / nAvgBytesPerSec 就是这条 PCM 的理论时长；除数为 0 时不能计算，所以返回 None。
    duration_seconds = (data_size / avg_bytes_per_sec) if avg_bytes_per_sec else None

    # 旧工具为了兼容历史文档，把前两个字节合成过 u16 “格式码”。
    # 固化37已经证明这不是一个独立枚举，因此这里只保留“兼容打包值”，并在字段名里明确它不是新语义。
    packed_legacy_view = channels | (bits_per_sample << 8)

    # 返回机器可读报告。每个验证项单独列出，后续回归能清楚知道究竟是哪条条件失败。
    return {
        "索引": index,
        "声道数_u8_加0": channels,
        "每样本位数_u8_加1": bits_per_sample,
        "采样率_u16_加2": sample_rate,
        "PCM数据字节数_u32_加4": data_size,
        "保留0_u32_加8": reserved0,
        "保留1_u32_加12": reserved1,
        "PCM数据实际字节数": len(payload),
        "兼容旧文档小端打包值_非独立格式枚举": packed_legacy_view,
        "每采样帧字节数_BlockAlign": block_align,
        "平均每秒字节数_AvgBytesPerSec": avg_bytes_per_sec,
        "理论时长秒": duration_seconds,
        "记录大小等于16加data_size": len(record) == 16 + data_size,
        "保留字段全零": reserved0 == 0 and reserved1 == 0,
        "声道数属于当前已知集合": channels in KNOWN_CHANNELS,
        "位深属于当前已知集合": bits_per_sample in KNOWN_BITS,
        "PCM大小按BlockAlign整除": bool(block_align) and data_size % block_align == 0,
        "PCM_SHA256": sha256_bytes(payload),
    }


def build_original_pcm_wave_header(channels: int, bits_per_sample: int, sample_rate: int, data_size: int) -> bytes:
    """逐字段重建原版 0x4057D0 生成的 44 字节 PCM RIFF/WAVE 头。"""

    # 位深必须按整字节组成 sample，否则 PCM block-align 公式无法成立。
    if bits_per_sample <= 0 or bits_per_sample % 8 != 0:
        raise ValueError(f"位深必须是正数且能被 8 整除，当前为 {bits_per_sample}")
    # 声道数、采样率、data_size 都不能是负数；RIFF 头对应字段也是无符号值。
    if channels <= 0 or sample_rate <= 0 or data_size < 0:
        raise ValueError("声道数和采样率必须大于 0，data_size 不能为负数")

    # 原版 0x40584C 先计算 channels * bits_per_sample。
    bits_all_channels = channels * bits_per_sample
    # 原版 0x40586D～0x40587A 再除以 8，得到 nBlockAlign。
    block_align = bits_all_channels // 8
    # 原版 0x405853～0x40586A 计算 channels * bits * sample_rate / 8，得到 nAvgBytesPerSec。
    avg_bytes_per_sec = sample_rate * block_align
    # RIFF 的 size 字段按规范等于“整个文件大小 - 8”；整个文件是 44 + data_size。
    riff_size_minus_8 = WAVE_HEADER_SIZE + data_size - 8

    # 用 bytearray 先申请正好 44 个零字节；后面按原版机器码写入每个字段。
    header = bytearray(WAVE_HEADER_SIZE)
    # 0x00 写 ASCII “RIFF”。原版从 0x4686E4 取这个 4 字节常量。
    header[0:4] = b"RIFF"
    # 0x04 写 RIFF chunk size = 文件总大小 - 8。
    struct.pack_into("<I", header, 4, riff_size_minus_8)
    # 0x08 写 ASCII “WAVE”。原版从 0x4686DC 取这个常量。
    header[8:12] = b"WAVE"
    # 0x0C 写 ASCII “fmt ”。末尾是一个空格，不是 NUL。
    header[12:16] = b"fmt "
    # 0x10 写 16，表示 PCM 的 fmt 子块正文固定 16 字节。
    struct.pack_into("<I", header, 16, 16)
    # 0x14 写 wFormatTag=1。原版 0x40585B 明确写 WORD 1，也就是 WAVE_FORMAT_PCM。
    struct.pack_into("<H", header, 20, 1)
    # 0x16 写声道数。
    struct.pack_into("<H", header, 22, channels)
    # 0x18 写采样率。
    struct.pack_into("<I", header, 24, sample_rate)
    # 0x1C 写平均每秒字节数。
    struct.pack_into("<I", header, 28, avg_bytes_per_sec)
    # 0x20 写 block-align。
    struct.pack_into("<H", header, 32, block_align)
    # 0x22 写每样本位数。
    struct.pack_into("<H", header, 34, bits_per_sample)
    # 0x24 写 ASCII “data”。
    header[36:40] = b"data"
    # 0x28 写 data_size；PCM 数据本体紧接在 44 字节头后。
    struct.pack_into("<I", header, 40, data_size)
    # 转成不可变 bytes 返回，防止调用者意外继续修改头内容。
    return bytes(header)


def build_original_pcm_wave(record_bytes: bytes, index: int) -> tuple[bytes, dict[str, Any]]:
    """把一条 Section3 记录转换成原版 0x4057D0 等价的完整 WAV 字节。"""

    # 先用唯一的真实字段解析函数取得声道、位深、采样率和 data_size。
    info = parse_section3_record(record_bytes, index)
    # 按 data_size 截取 payload；如果记录大小公式不成立，下面会拒绝而不是悄悄截断/补零。
    declared_size = info["PCM数据字节数_u32_加4"]
    payload = record_bytes[16:]
    # 原版依赖合法记录；工具为了研究安全，遇到大小不一致时明确停止，防止导出伪造 WAV。
    if len(payload) != declared_size:
        raise ValueError(
            f"Section3 记录 {index} 的 data_size={declared_size}，但实际 payload={len(payload)}，拒绝导出"
        )
    # 按原版公式构造 44 字节头。
    header = build_original_pcm_wave_header(
        info["声道数_u8_加0"],
        info["每样本位数_u8_加1"],
        info["采样率_u16_加2"],
        declared_size,
    )
    # 原版 0x4057F3～0x405801 就是把 payload 逐字节复制到 buffer+0x2C，因此这里直接 header + payload。
    wav = header + payload
    # 额外把完整 WAV 的 SHA-256 放进报告；同一固化包未来可以用它做 bit-exact 回归。
    info = dict(info)
    info["原版等价RIFF_WAVE_SHA256"] = sha256_bytes(wav)
    info["原版等价RIFF_WAVE总字节数"] = len(wav)
    return wav, info


def _parse_standard_sf2_bytes(raw: bytes, display_path: str) -> dict[str, Any]:
    """只解析标准 SF2，并提取 Section0→Section3 引用与 Section3 音频。"""

    # 复用旧工具已经验证过的“标准 SF2 + 0x6F zlib-tail”处理；SAF 会在这里被明确拒绝。
    full, compression_meta = _sf2_tool.sf2_decompress_if_needed(raw)
    # 复用旧工具已经验证过的四段 absolute record pointer table 解析。
    sections = _sf2_tool.sf2_sections(full)

    # Section3 解析结果按记录顺序放在列表中，索引与原文件中的 Section3 index 一致。
    audio_records: list[dict[str, Any]] = []
    for record_desc in sections[3]["记录"]:
        # 每条 descriptor 已经给出绝对起止偏移，直接切出该记录原始字节。
        record = full[record_desc["偏移"] : record_desc["结束"]]
        # 解析真实字段，并把结果加入列表。
        audio_records.append(parse_section3_record(record, record_desc["索引"]))

    # Section0 +0x20 是有符号 16 位 audio index。这里把所有非 -1 引用列出来并检查边界。
    audio_refs: list[dict[str, Any]] = []
    for record_desc in sections[0]["记录"]:
        # 先切出这条 Section0 动作记录。
        record = full[record_desc["偏移"] : record_desc["结束"]]
        # +0x20 至少要能读两个字节；标准记录正常都会满足，保护判断是为了不让坏样本导致 Python 越界异常。
        if len(record) < 0x22:
            continue
        # 按原版 movsx 的语义读取 signed int16。
        audio_index = _i16(record, 0x20)
        # -1 是原版明确处理的“没有音频”哨兵值，不算一条实际引用。
        if audio_index == -1:
            continue
        # 只要不是 -1，就记录它是否落在当前文件 Section3 的有效下标范围。
        audio_refs.append(
            {
                "Section0记录索引": record_desc["索引"],
                "Section3音频索引_来自Section0加0x20": audio_index,
                "Section3记录数": len(audio_records),
                "引用有效": 0 <= audio_index < len(audio_records),
            }
        )

    # 返回单文件报告。这里不把完整 PCM 字节写进 JSON，避免报告体积无意义膨胀。
    return {
        "路径": display_path,
        "原始SHA256": sha256_bytes(raw),
        "压缩信息": compression_meta,
        "Section数量": [len(section["记录"]) for section in sections],
        "Section3音频": audio_records,
        "Section0到Section3音频引用": audio_refs,
        "验证": {
            "Section3记录大小全部成立": all(item["记录大小等于16加data_size"] for item in audio_records),
            "Section3保留字段全部为零": all(item["保留字段全零"] for item in audio_records),
            "Section3声道全部属于当前已知集合": all(item["声道数属于当前已知集合"] for item in audio_records),
            "Section3位深全部属于当前已知集合": all(item["位深属于当前已知集合"] for item in audio_records),
            "Section3数据大小全部按BlockAlign整除": all(item["PCM大小按BlockAlign整除"] for item in audio_records),
            "Section0非负音频引用全部有效": all(item["引用有效"] for item in audio_refs),
        },
    }


def inspect_sf2_file(path: Path) -> dict[str, Any]:
    """检查一个磁盘上的标准 SF2 文件。"""

    # 先确认输入存在且是普通文件，避免把目录当文件读导致难懂异常。
    if not path.is_file():
        raise FileNotFoundError(f"找不到 SF2 文件：{path}")
    # 读取原始字节；后续统一走 bytes 解析，所以压缩/非压缩都不会修改源文件。
    raw = path.read_bytes()
    # 把人类可读路径直接放进报告。
    result = _parse_standard_sf2_bytes(raw, str(path))
    # 加上工具版本，方便以后看到单独 JSON 时也知道它由哪个版本生成。
    result["工具版本"] = TOOL_VERSION
    return result


def _iter_loose_sf2(root: Path) -> Iterable[Sf2Source]:
    """递归枚举目录里直接存在的 .SF2 文件。"""

    # rglob('*') 会递归遍历所有层级；再用 suffix.lower() 实现大小写不敏感匹配。
    for path in sorted(root.rglob("*"), key=lambda item: str(item).lower()):
        # 只处理普通文件，并且扩展名必须是 .sf2。
        if path.is_file() and path.suffix.lower() == ".sf2":
            # relative_to(root) 让报告不绑死本机绝对路径，固化包换机器后仍容易阅读。
            relative = str(path.relative_to(root)).replace("\\", "/")
            # yield 一次返回一条来源，而不是先把所有文件全部堆进列表。
            yield Sf2Source(relative, path.read_bytes(), "散装SF2")


def _iter_container_sf2(root: Path) -> Iterable[Sf2Source]:
    """递归扫描 DAT，并从已确认的资源容器中取出内嵌 SF2。"""

    # 先枚举所有 .DAT；排序保证每次报告顺序稳定，便于 Git/哈希对比。
    dat_files = sorted(
        (path for path in root.rglob("*") if path.is_file() and path.suffix.lower() == ".dat"),
        key=lambda item: str(item).lower(),
    )
    for dat_path in dat_files:
        # 调用已有资源容器调查器。普通地图/数据库 DAT 会返回“不是资源容器”，不会被误解包。
        container_report = _container_tool.parse_container(dat_path, include_records=False)
        # 只有已经通过该工具格式识别的资源容器才继续。
        if not container_report.get("识别为资源容器"):
            continue
        # 结构检查失败时绝不能继续切片，否则错误 offset 可能生成伪 SF2。
        if not container_report.get("结构全部通过"):
            raise ValueError(f"资源容器结构检查失败，拒绝继续：{dat_path}")
        # 一次读入容器原始字节，后面按每个内嵌记录的 data offset + size 切片。
        container_bytes = dat_path.read_bytes()
        # 报告中的“内嵌文件”已经由容器解析器验证过边界。
        for embedded in container_report.get("内嵌文件", []):
            # 只取名称以 .SF2 结尾的内嵌资源；其他 EVE/SCI 等文件与本工具无关。
            if not str(embedded["名称"]).lower().endswith(".sf2"):
                continue
            # “数据偏移”是相对整个 DAT 文件起点的绝对 file offset。
            start = int(embedded["数据偏移"])
            # “字节数”来自容器目录记录；end 就是切片结束位置。
            end = start + int(embedded["字节数"])
            # 切出完整内嵌文件字节。
            raw = container_bytes[start:end]
            # 再用 SHA-256 与容器工具报告交叉验证，防止本工具切错一个字节还继续统计。
            actual_sha = sha256_bytes(raw)
            if actual_sha != embedded["SHA256"]:
                raise ValueError(
                    f"容器内文件切片 SHA 不一致：{dat_path}::{embedded['容器内路径']}，"
                    f"期望 {embedded['SHA256']}，实际 {actual_sha}"
                )
            # 用“DAT相对路径::容器内路径”表示来源，既不污染游戏目录，也能精确回到原位置。
            dat_relative = str(dat_path.relative_to(root)).replace("\\", "/")
            display = f"{dat_relative}::{embedded['容器内路径']}"
            yield Sf2Source(display, raw, "资源容器内SF2")


def collect_corpus_sources(root: Path) -> list[Sf2Source]:
    """收集散装 + 资源容器内嵌 SF2 的全部出现。"""

    # 输入必须是目录；这是批量模式最基本的前置条件。
    if not root.is_dir():
        raise NotADirectoryError(f"找不到语料根目录：{root}")
    # 先收散装，再收容器内嵌；最终还会按路径稳定排序，所以顺序不影响结果。
    sources = list(_iter_loose_sf2(root))
    sources.extend(_iter_container_sf2(root))
    # 按显示路径排序，让 JSON 每次生成都稳定。
    sources.sort(key=lambda item: item.display_path.lower())
    return sources


def batch_corpus(root: Path) -> dict[str, Any]:
    """对完整 multimedia 类目录做 SHA 去重后的 Section3 全语料统计。"""

    # 先收集所有“出现”；同一 SHA 可能在多个文件名/容器记录里重复出现。
    sources = collect_corpus_sources(root)
    # 用 SHA 作为键建立去重表。value 保存代表字节和所有出现位置。
    unique: dict[str, dict[str, Any]] = {}
    for source in sources:
        # 每份出现计算 SHA。
        digest = sha256_bytes(source.raw)
        # 第一次见到这个 SHA 时，保存原始字节；后续相同 SHA 只追加来源，不重复解析。
        slot = unique.setdefault(digest, {"raw": source.raw, "occurrences": []})
        # 同 SHA 理论上应该字节完全相同；用断言式显式检查，防止 SHA 键使用逻辑写错。
        if slot["raw"] != source.raw:
            raise RuntimeError("出现理论上不可能的 SHA-256 碰撞/内部逻辑错误")
        # 记录这一份相同内容出现在哪。
        slot["occurrences"].append(
            {"来源类型": source.source_kind, "路径": source.display_path}
        )

    # 保存每个标准 SF2 的简化报告；不把没有 Section3 的文件展开太多细节，控制 JSON 体积。
    standard_files: list[dict[str, Any]] = []
    # SAF/其他拒绝样本单独列出，避免把“解析失败”与“已知异格式”混成一类。
    nonstandard_files: list[dict[str, Any]] = []
    # 真正解析错误（例如标准魔数但 Section 越界）单独列出，正常固化语料应为 0。
    parse_errors: list[dict[str, Any]] = []
    # 全部 Section3 记录平铺到一个列表，方便后面做格式分布统计。
    all_audio_records: list[dict[str, Any]] = []
    # 全部 Section0→Section3 实际引用平铺到这里。
    all_audio_refs: list[dict[str, Any]] = []

    for digest, slot in sorted(unique.items(), key=lambda pair: pair[0]):
        # 取这个 SHA 的第一条出现位置作为“代表路径”；所有路径仍会完整保留在文件条目里。
        representative = slot["occurrences"][0]["路径"]
        raw = slot["raw"]
        try:
            # 这里会自动处理标准 SF2 的 0x6F zlib-tail；已知 SAF 会被“不是当前已知 SF2”拒绝。
            file_report = _parse_standard_sf2_bytes(raw, representative)
        except ValueError as exc:
            # 固化35已证明 ITF000.SF2 是 SAF 异格式。它不应算“标准 SF2 解析失败”，而是已知非标准成员。
            if b"SAF" in raw[:16] or "不是当前已知 SF2 文件" in str(exc):
                nonstandard_files.append(
                    {
                        "SHA256": digest,
                        "代表路径": representative,
                        "全部出现": slot["occurrences"],
                        "原因": str(exc),
                        "头16字节HEX": raw[:16].hex(),
                    }
                )
                continue
            # 其他 ValueError 记录为真正错误，后续封包回归应要求它为空。
            parse_errors.append(
                {
                    "SHA256": digest,
                    "代表路径": representative,
                    "全部出现": slot["occurrences"],
                    "错误": str(exc),
                }
            )
            continue

        # 给单文件报告补充去重信息。
        file_report["全部出现"] = slot["occurrences"]
        file_report["出现次数"] = len(slot["occurrences"])
        # 只保留 Section3/引用与必要元信息，避免重复带入更大的图像解析数据。
        standard_files.append(file_report)

        # 把记录复制一份并附上文件 SHA/路径，这样平铺统计后仍能回溯来源。
        for audio in file_report["Section3音频"]:
            expanded = dict(audio)
            expanded["SF2_SHA256"] = digest
            expanded["代表路径"] = representative
            all_audio_records.append(expanded)
        # 同理展开 Section0 音频引用。
        for ref in file_report["Section0到Section3音频引用"]:
            expanded = dict(ref)
            expanded["SF2_SHA256"] = digest
            expanded["代表路径"] = representative
            all_audio_refs.append(expanded)

    # 统计“声道/位深/采样率”三元组；这是最精确的实际格式分布。
    format_counter = Counter(
        (
            item["声道数_u8_加0"],
            item["每样本位数_u8_加1"],
            item["采样率_u16_加2"],
        )
        for item in all_audio_records
    )
    # 统计只按“声道/位深”分组，方便快速看到 mono/stereo、8/16-bit 的大类数量。
    channel_bits_counter = Counter(
        (item["声道数_u8_加0"], item["每样本位数_u8_加1"])
        for item in all_audio_records
    )
    # 统计各采样率出现次数。
    sample_rate_counter = Counter(item["采样率_u16_加2"] for item in all_audio_records)

    # 建立“某 SF2 的某 Section3 index 被 Section0 引用了几次”的计数器。
    reference_counter = Counter(
        (item["SF2_SHA256"], item["Section3音频索引_来自Section0加0x20"])
        for item in all_audio_refs
        if item["引用有效"]
    )
    # 给每条音频记录补一列“实际被多少条 Section0 动作引用”。
    for audio in all_audio_records:
        audio["被Section0动作引用次数"] = reference_counter[(audio["SF2_SHA256"], audio["索引"])]

    # 把 Counter 的 tuple key 转成普通 JSON 结构；JSON 本身不允许 tuple 当对象 key。
    format_distribution = [
        {"声道数": key[0], "位深": key[1], "采样率": key[2], "记录数": count}
        for key, count in sorted(format_counter.items())
    ]
    channel_bits_distribution = [
        {"声道数": key[0], "位深": key[1], "记录数": count}
        for key, count in sorted(channel_bits_counter.items())
    ]
    sample_rate_distribution = [
        {"采样率": rate, "记录数": count}
        for rate, count in sorted(sample_rate_counter.items())
    ]

    # 顶层汇总直接对应固化37要冻结的关键数字。
    summary = {
        "SF2出现总数": len(sources),
        "散装SF2出现数": sum(1 for item in sources if item.source_kind == "散装SF2"),
        "容器内SF2出现数": sum(1 for item in sources if item.source_kind == "资源容器内SF2"),
        "唯一SHA256总数": len(unique),
        "标准SF2唯一文件数": len(standard_files),
        "已知非标准或异格式唯一文件数": len(nonstandard_files),
        "标准SF2解析错误数": len(parse_errors),
        "含Section3的标准SF2唯一文件数": sum(1 for item in standard_files if item["Section3音频"]),
        "Section3音频记录总数": len(all_audio_records),
        "Section0非负音频引用总数": len(all_audio_refs),
        "Section0音频引用有效数": sum(1 for item in all_audio_refs if item["引用有效"]),
        "Section0音频引用无效数": sum(1 for item in all_audio_refs if not item["引用有效"]),
        "被至少一条Section0动作引用的Section3记录数": sum(
            1 for item in all_audio_records if item["被Section0动作引用次数"] > 0
        ),
        "未被Section0动作直接引用的Section3记录数": sum(
            1 for item in all_audio_records if item["被Section0动作引用次数"] == 0
        ),
        "8位PCM记录数": sum(1 for item in all_audio_records if item["每样本位数_u8_加1"] == 8),
        "16位PCM记录数": sum(1 for item in all_audio_records if item["每样本位数_u8_加1"] == 16),
        "所有Section3记录大小公式成立": all(item["记录大小等于16加data_size"] for item in all_audio_records),
        "所有Section3保留字段全零": all(item["保留字段全零"] for item in all_audio_records),
        "所有Section3数据按BlockAlign整除": all(item["PCM大小按BlockAlign整除"] for item in all_audio_records),
        "所有Section0非负音频引用有效": all(item["引用有效"] for item in all_audio_refs),
    }

    return {
        "工具版本": TOOL_VERSION,
        "语料根": str(root),
        "汇总": summary,
        "声道位深分布": channel_bits_distribution,
        "采样率分布": sample_rate_distribution,
        "声道位深采样率三元组分布": format_distribution,
        "Section3记录": all_audio_records,
        "Section0到Section3音频引用": all_audio_refs,
        "标准SF2文件": standard_files,
        "已知非标准或异格式文件": nonstandard_files,
        "解析错误": parse_errors,
    }


class PEImage:
    """只实现固化37机器码断言需要的最小 PE32 VA→文件偏移映射。"""

    def __init__(self, path: Path) -> None:
        # 保存路径，错误报告里可以直接显示当前检查的是哪份 EXE。
        self.path = path
        # 一次读入整个 RPG.exe，后面所有断言都在只读 bytes 上完成。
        self.data = path.read_bytes()
        # DOS 头前两个字节必须是 MZ；不是就说明输入根本不是 PE 文件。
        if self.data[:2] != b"MZ":
            raise ValueError("输入文件没有 MZ DOS 头，不是当前工具可检查的 PE")
        # DOS 头 +0x3C 存放 PE 头文件偏移 e_lfanew。
        pe_offset = _u32(self.data, 0x3C)
        # PE 头必须以 'PE\\0\\0' 开始。
        if self.data[pe_offset : pe_offset + 4] != b"PE\0\0":
            raise ValueError("PE 签名不正确")
        # COFF FileHeader 中 +2 是 section 数量。
        section_count = _u16(self.data, pe_offset + 6)
        # COFF FileHeader 中 +16 是 OptionalHeader 大小。
        optional_size = _u16(self.data, pe_offset + 20)
        # OptionalHeader 紧跟在 24 字节 PE Signature + FileHeader 之后。
        optional_offset = pe_offset + 24
        # PE32 OptionalHeader +0x1C 是 ImageBase。当前 RPG.exe 是 32 位程序。
        self.image_base = _u32(self.data, optional_offset + 0x1C)
        # Section table 紧跟 OptionalHeader。
        section_table = optional_offset + optional_size
        self.sections: list[dict[str, int | str]] = []
        for index in range(section_count):
            # 每个 IMAGE_SECTION_HEADER 固定 40 字节。
            offset = section_table + index * 40
            # 前 8 字节是 section 名，NUL 后内容忽略。
            name = self.data[offset : offset + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
            # +8 VirtualSize，表示内存中的逻辑大小。
            virtual_size = _u32(self.data, offset + 8)
            # +12 VirtualAddress 是相对 ImageBase 的 RVA。
            virtual_address = _u32(self.data, offset + 12)
            # +16 SizeOfRawData 是磁盘中实际占用字节数。
            raw_size = _u32(self.data, offset + 16)
            # +20 PointerToRawData 是该 section 在文件中的起点。
            raw_offset = _u32(self.data, offset + 20)
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
        # 绝对虚拟地址减去 ImageBase 得到 RVA。
        rva = va - self.image_base
        for section in self.sections:
            # 映射范围取 VirtualSize / RawSize 的较大者，兼容文件大小与内存大小略有不同的 section。
            start = int(section["virtual_address"])
            span = max(int(section["virtual_size"]), int(section["raw_size"]))
            if start <= rva < start + span:
                # 同一 section 内的相对偏移，加上磁盘 raw 起点，就是文件偏移。
                return int(section["raw_offset"]) + (rva - start)
        raise ValueError(f"VA 0x{va:08X} 不在任何 PE section 中")

    def read_va(self, va: int, size: int) -> bytes:
        # 先把 VA 映射成文件 offset。
        offset = self.va_to_offset(va)
        # 再从不可变 bytes 中取出指定长度，不会修改 EXE。
        return self.data[offset : offset + size]


def _machine_check(pe: PEImage, va: int, expected_hex: str, meaning: str) -> dict[str, Any]:
    """检查某个 VA 的机器码是否与固化37断言完全一致。"""

    # fromhex 把“0f bf 48 20”这种便于人工核对的文本变回 bytes。
    expected = bytes.fromhex(expected_hex)
    # 读取同长度实际机器码。
    actual = pe.read_va(va, len(expected))
    # 返回完整证据，而不是只返回 True/False；以后 EXE 变体不通过时能看到实际差异。
    return {
        "VA": f"0x{va:08X}",
        "含义": meaning,
        "期望HEX": expected.hex(" "),
        "实际HEX": actual.hex(" "),
        "通过": actual == expected,
    }


def _machine_check_ascii(pe: PEImage, va: int, expected: bytes, meaning: str) -> dict[str, Any]:
    """检查 RPG.exe 数据区中的 ASCII 常量。"""

    # 读取与期望字符串相同长度的数据。
    actual = pe.read_va(va, len(expected))
    return {
        "VA": f"0x{va:08X}",
        "含义": meaning,
        "期望ASCII": expected.decode("ascii", errors="replace"),
        "实际HEX": actual.hex(" "),
        "通过": actual == expected,
    }


def verify_rpg_audio_protocol(path: Path) -> dict[str, Any]:
    """重放固化37 SF2 Section3→RIFF→BASS 的关键机器码断言。"""

    # PEImage 构造时会先验证 MZ/PE 结构并建立 VA 映射。
    pe = PEImage(path)
    checks: list[dict[str, Any]] = []

    # 第一套标准 SF2 播放对象：Section0 +0x20 取 signed audio index。
    checks.append(_machine_check(pe, 0x00407386, "0f bf 48 20", "第一播放器：Section0 +0x20 以有符号16位读取音频索引"))
    # 第一播放器从 Section3 +0x02 读取采样率。
    checks.append(_machine_check(pe, 0x004073A9, "66 8b 50 02", "第一播放器：Section3 +0x02 读取u16采样率"))
    # 第一播放器从 Section3 +0x01 读取位深。
    checks.append(_machine_check(pe, 0x004073AD, "8a 48 01", "第一播放器：Section3 +0x01 读取u8位深"))
    # 第一播放器从 Section3 +0x00 读取声道数。
    checks.append(_machine_check(pe, 0x004073B3, "8a 10", "第一播放器：Section3 +0x00 读取u8声道数"))
    # 第一播放器从 Section3 +0x04 读取 PCM 数据字节数。
    checks.append(_machine_check(pe, 0x004073B5, "8b 40 04", "第一播放器：Section3 +0x04 读取u32 PCM数据大小"))
    # 第一播放器最终调用共同的 RIFF/WAVE 构造器 0x4057D0。
    checks.append(_machine_check(pe, 0x004073C9, "e8 02 e4 ff ff", "第一播放器：调用0x4057D0构造并播放内存WAV"))

    # 第二套标准 SF2 播放对象重复同一字段协议，因此它是独立交叉证明，不是同一函数的重复观察。
    checks.append(_machine_check(pe, 0x00429030, "0f bf 40 20", "第二播放器：Section0 +0x20 以有符号16位读取音频索引"))
    checks.append(_machine_check(pe, 0x00429051, "66 8b 48 02", "第二播放器：Section3 +0x02 读取u16采样率"))
    checks.append(_machine_check(pe, 0x00429055, "8a 50 01", "第二播放器：Section3 +0x01 读取u8位深"))
    checks.append(_machine_check(pe, 0x0042905B, "8a 08", "第二播放器：Section3 +0x00 读取u8声道数"))
    checks.append(_machine_check(pe, 0x0042905E, "8b 50 04", "第二播放器：Section3 +0x04 读取u32 PCM数据大小"))
    checks.append(_machine_check(pe, 0x00429071, "e8 5a c7 fd ff", "第二播放器：调用0x4057D0构造并播放内存WAV"))

    # 0x4057D0 的关键语义：总缓冲区 = data_size + 0x2C，也就是预留 44 字节 WAV 头。
    checks.append(_machine_check(pe, 0x004057DA, "8d 46 2c", "RIFF构造器：申请/布局时把PCM大小加0x2C=44字节头"))
    # payload 目标地址固定是临时缓冲区 +0x2C。
    checks.append(_machine_check(pe, 0x004057F3, "8d 7b 2c", "RIFF构造器：PCM复制目标从buffer+0x2C开始"))
    # fmt chunk size 固定写 16。
    checks.append(_machine_check(pe, 0x00405805, "c7 43 10 10 00 00 00", "RIFF构造器：fmt chunk正文大小固定16"))
    # data_size 原样写到 WAV +0x28。
    checks.append(_machine_check(pe, 0x0040580C, "89 43 28", "RIFF构造器：WAV +0x28写PCM data_size"))
    # wFormatTag 固定写 1，也就是 PCM。
    checks.append(_machine_check(pe, 0x0040585B, "66 c7 43 14 01 00", "RIFF构造器：WAVE_FORMAT_PCM=1"))
    # sample_rate 写到 +0x18。
    checks.append(_machine_check(pe, 0x00405858, "89 53 18", "RIFF构造器：WAV +0x18写sample_rate"))
    # nAvgBytesPerSec 写到 +0x1C。
    checks.append(_machine_check(pe, 0x0040586A, "89 43 1c", "RIFF构造器：WAV +0x1C写channels*bits*rate/8"))
    # nBlockAlign 写到 +0x20。
    checks.append(_machine_check(pe, 0x0040587A, "66 89 43 20", "RIFF构造器：WAV +0x20写channels*bits/8"))
    # 构造后交给内存播放包装 0x4056C0。
    checks.append(_machine_check(pe, 0x0040587E, "e8 3d fe ff ff", "RIFF构造器：调用0x4056C0进入内存SampleLoad/播放链"))

    # 0x4053B0 内部给 BASS_SampleLoad 压入 mem=1；说明传入的是“完整内存文件”，不是裸PCM指针。
    checks.append(_machine_check(pe, 0x0040547F, "68 00 00 02 00 6a 20 6a 00 6a 00", "BASS内存加载：flags/max/offset/length参数组"))
    checks.append(_machine_check(pe, 0x0040548A, "52 6a 01", "BASS内存加载：压入内存文件指针后再压入mem=1"))
    # BASS_SampleLoad 的 thunk 明确跳到 IAT 0x460018。
    checks.append(_machine_check(pe, 0x0044B742, "ff 25 18 00 46 00", "BASS_SampleLoad导入跳板"))

    # 四个 RIFF 常量同样直接来自 RPG.exe 数据区。
    checks.append(_machine_check_ascii(pe, 0x004686E4, b"RIFF", "RIFF四字节标识"))
    checks.append(_machine_check_ascii(pe, 0x004686DC, b"WAVE", "WAVE四字节标识"))
    checks.append(_machine_check_ascii(pe, 0x004686D4, b"fmt ", "fmt空格四字节标识"))
    checks.append(_machine_check_ascii(pe, 0x004686CC, b"data", "data四字节标识"))
    # BASS 版本字符串用于冻结“原版确实针对 BASS 0.6”的历史运行库边界。
    checks.append(_machine_check_ascii(pe, 0x00468548, b"BASS version 0.6 was not loaded.", "原版BASS版本错误字符串"))

    # 汇总通过数；不要求整个 EXE SHA 与旧 Oracle 一样，因为固化34已经证明当前 .org 是“已覆盖代码区等价”而非整文件同SHA。
    passed = sum(1 for item in checks if item["通过"])
    return {
        "工具版本": TOOL_VERSION,
        "RPG文件": str(path),
        "RPG_SHA256": sha256_file(path),
        "断言数": len(checks),
        "通过数": passed,
        "失败数": len(checks) - passed,
        "全部通过": passed == len(checks),
        "断言": checks,
        "说明": (
            "固化34已确认当前RPG.exe.org与旧Oracle整文件SHA不同，但大量已覆盖机器码等价；"
            "因此本工具以本专题精确VA机器码断言作为通过条件，不把整文件SHA当作唯一Oracle。"
        ),
    }


def export_audio_from_sf2(path: Path, outdir: Path, only_index: int | None) -> dict[str, Any]:
    """把一个标准 SF2 的 Section3 逐条导出成原版等价 RIFF/WAVE。"""

    # 读取并按旧工具稳定逻辑解压/解析标准 SF2。
    raw = path.read_bytes()
    full, _compression_meta = _sf2_tool.sf2_decompress_if_needed(raw)
    sections = _sf2_tool.sf2_sections(full)
    # 输出目录不存在就创建；exist_ok=True 表示已经存在时不报错。
    outdir.mkdir(parents=True, exist_ok=True)
    exports: list[dict[str, Any]] = []

    for record_desc in sections[3]["记录"]:
        # 用户如果指定 --索引，就只处理那一条，其他记录直接跳过。
        if only_index is not None and record_desc["索引"] != only_index:
            continue
        # 切出完整 Section3 记录。
        record = full[record_desc["偏移"] : record_desc["结束"]]
        # 构造 bit-exact RIFF/WAVE，并取得解析信息。
        wav, info = build_original_pcm_wave(record, record_desc["索引"])
        # 文件名把采样率/位深/声道写出来，肉眼就能分辨导出的格式。
        filename = (
            f"{path.stem}_Section3_{record_desc['索引']:04d}_"
            f"{info['采样率_u16_加2']}Hz_{info['每样本位数_u8_加1']}bit_"
            f"{info['声道数_u8_加0']}ch.wav"
        )
        destination = outdir / filename
        # 写出完整 WAV；源 SF2 不会被修改。
        destination.write_bytes(wav)
        # 报告导出文件路径与 SHA，方便用户核对。
        exports.append(
            {
                "Section3索引": record_desc["索引"],
                "输出文件": str(destination),
                "输出SHA256": sha256_bytes(wav),
                "输出字节数": len(wav),
            }
        )

    # 指定了一个不存在的索引时，明确报错，避免用户以为成功但目录为空。
    if only_index is not None and not exports:
        raise IndexError(f"Section3 中不存在索引 {only_index}")
    return {"工具版本": TOOL_VERSION, "输入文件": str(path), "导出": exports}


def write_json(path: Path | None, report: dict[str, Any]) -> None:
    """把报告写成 UTF-8 JSON；path=None 时打印到标准输出。"""

    # ensure_ascii=False 保留中文，不把每个汉字变成 \uXXXX。
    text = json.dumps(report, ensure_ascii=False, indent=2)
    if path is None:
        # 没指定输出文件时直接打印，适合临时人工检查。
        print(text)
        return
    # 创建父目录，避免用户指定“reports/xxx.json”时因为目录不存在失败。
    path.parent.mkdir(parents=True, exist_ok=True)
    # newline='\n' 让 JSON 在不同平台生成稳定换行；内容编码明确为 UTF-8。
    path.write_text(text + "\n", encoding="utf-8", newline="\n")


def _build_parser() -> argparse.ArgumentParser:
    """创建命令行参数解析器。"""

    # description 会显示在 --help 顶部。
    parser = argparse.ArgumentParser(description="《幽城幻剑录》SF2 Section3 嵌入PCM/RIFF 调查器")
    # --版本 方便封包审计不用真正解析资源就确认工具版本。
    parser.add_argument("--版本", action="version", version=TOOL_VERSION)
    # subparsers 表示必须先选择一个动作。
    sub = parser.add_subparsers(dest="command", required=True)

    # “检查”只看一个标准 SF2，适合验证单个异常样本。
    one = sub.add_parser("检查", help="检查一个标准SF2的Section3与Section0音频引用")
    one.add_argument("文件", type=Path, help="要检查的.SF2文件")
    one.add_argument("--输出JSON", type=Path, help="可选：把报告写入JSON文件")

    # “批量”会同时扫描散装 SF2 和已识别资源容器内的 SF2，并按 SHA 去重。
    batch = sub.add_parser("批量", help="扫描multimedia类目录的散装+资源容器内SF2并做全语料统计")
    batch.add_argument("根目录", type=Path, help="例如解压后的multimedia目录")
    batch.add_argument("--输出JSON", type=Path, help="可选：把完整报告写入JSON文件")

    # “验证RPG”只读检查机器码，不执行/注入游戏。
    verify = sub.add_parser("验证RPG", help="重放Section3→RIFF→BASS原版机器码断言")
    verify.add_argument("RPG", type=Path, help="同版RPG.exe或RPG.exe.org")
    verify.add_argument("--输出JSON", type=Path, help="可选：把断言报告写入JSON文件")

    # “导出”把 Section3 按原版 0x4057D0 的字段写法构造成标准 WAV。
    export = sub.add_parser("导出", help="按原版0x4057D0等价公式导出Section3为RIFF/WAVE")
    export.add_argument("文件", type=Path, help="要导出的标准SF2")
    export.add_argument("输出目录", type=Path, help="WAV输出目录")
    export.add_argument("--索引", type=int, help="可选：只导出一个Section3索引")
    export.add_argument("--输出JSON", type=Path, help="可选：把导出清单写入JSON文件")

    return parser


def main(argv: list[str] | None = None) -> int:
    """命令行入口。返回0表示成功，1表示研究验证没有通过或输入异常。"""

    # 解析用户参数；argv=None 时 argparse 自动读取 sys.argv[1:]。
    parser = _build_parser()
    args = parser.parse_args(argv)
    try:
        # 根据子命令进入对应逻辑。
        if args.command == "检查":
            report = inspect_sf2_file(args.文件)
            write_json(args.输出JSON, report)
            # 单文件验证中只要任何核心验证失败，就用退出码1提醒自动回归。
            return 0 if all(report["验证"].values()) else 1
        if args.command == "批量":
            report = batch_corpus(args.根目录)
            write_json(args.输出JSON, report)
            summary = report["汇总"]
            # 固化37的批量“通过”要求：标准解析零错误、引用零越界、结构公式全部成立。
            ok = (
                summary["标准SF2解析错误数"] == 0
                and summary["Section0音频引用无效数"] == 0
                and summary["所有Section3记录大小公式成立"]
                and summary["所有Section3保留字段全零"]
                and summary["所有Section3数据按BlockAlign整除"]
            )
            return 0 if ok else 1
        if args.command == "验证RPG":
            report = verify_rpg_audio_protocol(args.RPG)
            write_json(args.输出JSON, report)
            return 0 if report["全部通过"] else 1
        if args.command == "导出":
            report = export_audio_from_sf2(args.文件, args.输出目录, args.索引)
            write_json(args.输出JSON, report)
            return 0
        # 理论上 argparse 的 required=True 已经保证不会走到这里；保留保护分支便于未来改子命令时发现遗漏。
        parser.error("未知命令")
        return 1
    except Exception as exc:
        # 研究工具遇到错误时只输出清楚的中文一行，不打印巨大 traceback 干扰普通使用。
        print(f"错误：{exc}", file=sys.stderr)
        return 1


# 只有直接运行本文件时才执行 main；被固化回归脚本 import 时只加载函数，不会自动扫描资源。
if __name__ == "__main__":
    raise SystemExit(main())
