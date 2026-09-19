#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""幽城幻剑录样本结构验证器

用途：对《幽城幻剑录》原版/样本资源执行只读结构检查，验证兼容引擎研究手册中的
若干已确认事实。仅使用 Python 标准库，不修改任何输入文件。
"""
# 启用较新的 Python 注解语法行为，让类型注解先按文字保存，避免旧版本在定义阶段立即求值。
from __future__ import annotations
# 导入 `argparse` 模块；这里只取得标准库功能，不会修改游戏文件。
import argparse
# 导入 `hashlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import hashlib
# 导入 `json` 模块；这里只取得标准库功能，不会修改游戏文件。
import json
# 导入 `os` 模块；这里只取得标准库功能，不会修改游戏文件。
import os
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path
# 导入 `struct` 模块；这里只取得标准库功能，不会修改游戏文件。
import struct
# 导入 `sys` 模块；这里只取得标准库功能，不会修改游戏文件。
import sys
# 导入 `zipfile` 模块；这里只取得标准库功能，不会修改游戏文件。
import zipfile
# 从 `typing` 模块导入 `Iterable, Optional`，后面的代码会直接使用这些现成组件。
from typing import Iterable, Optional

# 把右侧 `"0.1.2"` 计算得到的值保存到 `VERSION`，后面的判断或输出会继续使用它。
VERSION = "0.1.2"


# 定义函数 `sha256`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sha256(path: Path) -> str:
    # 把右侧 `hashlib.sha256()` 计算得到的值保存到 `h`，后面的判断或输出会继续使用它。
    h = hashlib.sha256()
    # 进入受管理的资源作用域 `path.open("rb") as f`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
    with path.open("rb") as f:
        # 开始循环 `chunk in iter(lambda: f.read(1024 * 1024), b"")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            # 调用 `h.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            h.update(chunk)
    # 把 `h.hexdigest()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return h.hexdigest()


# 定义函数 `u16`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def u16(b: bytes, o: int) -> int:
    # 把 `struct.unpack_from("<H", b, o)[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return struct.unpack_from("<H", b, o)[0]


# 定义函数 `u32`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def u32(b: bytes, o: int) -> int:
    # 把 `struct.unpack_from("<I", b, o)[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return struct.unpack_from("<I", b, o)[0]


# 定义函数 `index_files`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def index_files(root: Path) -> dict[str, list[Path]]:
    # 创建变量 `out`（类型提示为 `dict[str, list[Path]]`），并把 `{}` 的结果保存进去供后续步骤使用。
    out: dict[str, list[Path]] = {}
    # 检查条件 `not root.exists()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not root.exists():
        # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return out
    # 开始循环 `p in root.rglob("*")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for p in root.rglob("*"):
        # 检查条件 `p.is_file()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if p.is_file():
            # 调用 `out.setdefault` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            out.setdefault(p.name.lower(), []).append(p)
    # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return out


# 定义函数 `first`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def first(index: dict[str, list[Path]], *names: str) -> Optional[Path]:
    # 开始循环 `n in names`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for n in names:
        # 把右侧 `index.get(n.lower())` 计算得到的值保存到 `xs`，后面的判断或输出会继续使用它。
        xs = index.get(n.lower())
        # 检查条件 `xs`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if xs:
            # 把 `xs[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
            return xs[0]
    # 把 `None` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return None


# 定义函数 `pe_info`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def pe_info(path: Path) -> dict:
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b = path.read_bytes()
    # 检查条件 `len(b) < 0x100 or b[:2] != b"MZ"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(b) < 0x100 or b[:2] != b"MZ":
        # 检测到不能继续的情况，主动抛出异常 `ValueError("不是有效的 MZ/PE 文件")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("不是有效的 MZ/PE 文件")
    # 把右侧 `u32(b, 0x3C)` 计算得到的值保存到 `peoff`，后面的判断或输出会继续使用它。
    peoff = u32(b, 0x3C)
    # 检查条件 `b[peoff:peoff+4] != b"PE\0\0"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if b[peoff:peoff+4] != b"PE\0\0":
        # 检测到不能继续的情况，主动抛出异常 `ValueError("找不到 PE 签名")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("找不到 PE 签名")
    # 把右侧 `peoff + 4` 计算得到的值保存到 `coff`，后面的判断或输出会继续使用它。
    coff = peoff + 4
    # 把右侧 `u16(b, coff)` 计算得到的值保存到 `machine`，后面的判断或输出会继续使用它。
    machine = u16(b, coff)
    # 把右侧 `u16(b, coff + 2)` 计算得到的值保存到 `nsec`，后面的判断或输出会继续使用它。
    nsec = u16(b, coff + 2)
    # 把右侧 `u32(b, coff + 4)` 计算得到的值保存到 `timestamp`，后面的判断或输出会继续使用它。
    timestamp = u32(b, coff + 4)
    # 把右侧 `u16(b, coff + 16)` 计算得到的值保存到 `optsz`，后面的判断或输出会继续使用它。
    optsz = u16(b, coff + 16)
    # 把右侧 `u16(b, coff + 18)` 计算得到的值保存到 `chars`，后面的判断或输出会继续使用它。
    chars = u16(b, coff + 18)
    # 把右侧 `coff + 20` 计算得到的值保存到 `opt`，后面的判断或输出会继续使用它。
    opt = coff + 20
    # 把右侧 `u16(b, opt)` 计算得到的值保存到 `magic`，后面的判断或输出会继续使用它。
    magic = u16(b, opt)
    # 把右侧 `u32(b, opt + 16)` 计算得到的值保存到 `entry`，后面的判断或输出会继续使用它。
    entry = u32(b, opt + 16)
    # 把右侧 `u32(b, opt + 28) if magic == 0x10B else None` 计算得到的值保存到 `image_base`，后面的判断或输出会继续使用它。
    image_base = u32(b, opt + 28) if magic == 0x10B else None
    # 把右侧 `u32(b, opt + 56)` 计算得到的值保存到 `size_image`，后面的判断或输出会继续使用它。
    size_image = u32(b, opt + 56)
    # 把右侧 `u16(b, opt + 68)` 计算得到的值保存到 `subsystem`，后面的判断或输出会继续使用它。
    subsystem = u16(b, opt + 68)
    # 把右侧 `u16(b, opt + 70)` 计算得到的值保存到 `dllchars`，后面的判断或输出会继续使用它。
    dllchars = u16(b, opt + 70)
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“path”字段填写 `str(path),`，这样导出的 JSON/表格能保留这一项证据。
        "path": str(path),
        # 给当前结果字典的“size”字段填写 `len(b),`，这样导出的 JSON/表格能保留这一项证据。
        "size": len(b),
        # 给当前结果字典的“sha256”字段填写 `sha256(path),`，这样导出的 JSON/表格能保留这一项证据。
        "sha256": sha256(path),
        # 给当前结果字典的“machine”字段填写 `f"0x{machine:04X}",`，这样导出的 JSON/表格能保留这一项证据。
        "machine": f"0x{machine:04X}",
        # 给当前结果字典的“pe_magic”字段填写 `f"0x{magic:04X}",`，这样导出的 JSON/表格能保留这一项证据。
        "pe_magic": f"0x{magic:04X}",
        # 给当前结果字典的“sections”字段填写 `nsec,`，这样导出的 JSON/表格能保留这一项证据。
        "sections": nsec,
        # 给当前结果字典的“timestamp”字段填写 `timestamp,`，这样导出的 JSON/表格能保留这一项证据。
        "timestamp": timestamp,
        # 给当前结果字典的“entry_rva”字段填写 `f"0x{entry:08X}",`，这样导出的 JSON/表格能保留这一项证据。
        "entry_rva": f"0x{entry:08X}",
        # 给当前结果字典的“image_base”字段填写 `f"0x{image_base:08X}" if image_base is not None else None,`，这样导出的 JSON/表格能保留这一项证据。
        "image_base": f"0x{image_base:08X}" if image_base is not None else None,
        # 给当前结果字典的“size_of_image”字段填写 `size_image,`，这样导出的 JSON/表格能保留这一项证据。
        "size_of_image": size_image,
        # 给当前结果字典的“subsystem”字段填写 `subsystem,`，这样导出的 JSON/表格能保留这一项证据。
        "subsystem": subsystem,
        # 给当前结果字典的“characteristics”字段填写 `f"0x{chars:04X}",`，这样导出的 JSON/表格能保留这一项证据。
        "characteristics": f"0x{chars:04X}",
        # 给当前结果字典的“dll_characteristics”字段填写 `f"0x{dllchars:04X}",`，这样导出的 JSON/表格能保留这一项证据。
        "dll_characteristics": f"0x{dllchars:04X}",
        # 给当前结果字典的“relocations_stripped”字段填写 `bool(chars & 0x0001),`，这样导出的 JSON/表格能保留这一项证据。
        "relocations_stripped": bool(chars & 0x0001),
        # 给当前结果字典的“large_address_aware”字段填写 `bool(chars & 0x0020),`，这样导出的 JSON/表格能保留这一项证据。
        "large_address_aware": bool(chars & 0x0020),
        # 给当前结果字典的“dynamic_base”字段填写 `bool(dllchars & 0x0040),`，这样导出的 JSON/表格能保留这一项证据。
        "dynamic_base": bool(dllchars & 0x0040),
    }


# 定义函数 `parse_archive_entries`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_archive_entries(path: Path, file_table_offset: int, count: int) -> list[dict]:
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b = path.read_bytes()
    # 把右侧 `[]` 计算得到的值保存到 `entries`，后面的判断或输出会继续使用它。
    entries = []
    # 开始循环 `i in range(count)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(count):
        # 把右侧 `file_table_offset + i * 39` 计算得到的值保存到 `o`，后面的判断或输出会继续使用它。
        o = file_table_offset + i * 39
        # 检查条件 `o + 39 > len(b)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if o + 39 > len(b):
            # 立即结束当前循环，不再处理后续元素。
            break
        # 把右侧 `u16(b, o)` 计算得到的值保存到 `flags`，后面的判断或输出会继续使用它。
        flags = u16(b, o)
        # 把右侧 `u32(b, o + 2)` 计算得到的值保存到 `size`，后面的判断或输出会继续使用它。
        size = u32(b, o + 2)
        # 把右侧 `u32(b, o + 6)` 计算得到的值保存到 `unknown`，后面的判断或输出会继续使用它。
        unknown = u32(b, o + 6)
        # 把右侧 `b[o+10:o+35].split(b"\0", 1)[0]` 计算得到的值保存到 `raw_name`，后面的判断或输出会继续使用它。
        raw_name = b[o+10:o+35].split(b"\0", 1)[0]
        # 把右侧 `raw_name.decode("latin1", errors="replace")` 计算得到的值保存到 `name`，后面的判断或输出会继续使用它。
        name = raw_name.decode("latin1", errors="replace")
        # 把右侧 `u32(b, o + 35)` 计算得到的值保存到 `data_offset`，后面的判断或输出会继续使用它。
        data_offset = u32(b, o + 35)
        # 把这一条新结果追加到列表 `entries`；不会覆盖前面已经收集的记录。
        entries.append({
            # 给当前结果字典的“index”字段填写 `i, "flags": flags, "size": size,`，这样导出的 JSON/表格能保留这一项证据。
            "index": i, "flags": flags, "size": size,
            # 给当前结果字典的“unknown”字段填写 `unknown, "name": name, "offset": data_offset,`，这样导出的 JSON/表格能保留这一项证据。
            "unknown": unknown, "name": name, "offset": data_offset,
            # 给当前结果字典的“in_bounds”字段填写 `data_offset + size <= len(b)`，这样导出的 JSON/表格能保留这一项证据。
            "in_bounds": data_offset + size <= len(b)
        })
    # 把 `entries` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return entries


# 定义函数 `detect_known_archive`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def detect_known_archive(path: Path) -> dict:
    """针对当前已确认样本的 DAT/SYS/MUSIC 容器结构做保守解析。

    已确认：目录项和文件项均可使用 39-byte 结构描述；这里通过扫描已知目录名后的
    39-byte 子项定位文件表。若不能可靠定位则只返回头信息，不猜测。
    """
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b = path.read_bytes()
    # 把右侧 `{"path": str(path), "size": len(b), "sha256": sha256(path)}` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {"path": str(path), "size": len(b), "sha256": sha256(path)}
    # 在前 0x4000 字节中扫描看起来像 39-byte 目录记录的结构：flags==1、count合理、unknown==0、ASCII名、offset合理。
    # 把右侧 `[]` 计算得到的值保存到 `candidates`，后面的判断或输出会继续使用它。
    candidates = []
    # 把右侧 `min(len(b) - 39, 0x4000)` 计算得到的值保存到 `limit`，后面的判断或输出会继续使用它。
    limit = min(len(b) - 39, 0x4000)
    # 开始循环 `o in range(limit)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for o in range(limit):
        # 检查条件 `u16(b, o) != 1`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if u16(b, o) != 1:
            # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
            continue
        # 把右侧 `u32(b, o + 2)` 计算得到的值保存到 `cnt`，后面的判断或输出会继续使用它。
        cnt = u32(b, o + 2)
        # 把右侧 `u32(b, o + 6)` 计算得到的值保存到 `unk`，后面的判断或输出会继续使用它。
        unk = u32(b, o + 6)
        # 检查条件 `not (1 <= cnt <= 10000 and unk == 0)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not (1 <= cnt <= 10000 and unk == 0):
            # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
            continue
        # 把右侧 `b[o+10:o+35].split(b"\0",1)[0]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw = b[o+10:o+35].split(b"\0",1)[0]
        # 检查条件 `not raw or any(x < 0x20 or x >= 0x7F for x in raw)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not raw or any(x < 0x20 or x >= 0x7F for x in raw):
            # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
            continue
        # 把右侧 `u32(b, o+35)` 计算得到的值保存到 `off`，后面的判断或输出会继续使用它。
        off = u32(b, o+35)
        # 检查条件 `off + cnt * 39 <= len(b)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if off + cnt * 39 <= len(b):
            # 把这一条新结果追加到列表 `candidates`；不会覆盖前面已经收集的记录。
            candidates.append((o,cnt,raw.decode('ascii','replace'),off))
    # 优先名字像目录的候选。
    # 把右侧 `None` 计算得到的值保存到 `chosen`，后面的判断或输出会继续使用它。
    chosen = None
    # 开始循环 `c in candidates`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for c in candidates:
        # 检查条件 `c[2].upper() in {"MENUS","SYS","MUSIC","HIT","WEK","END","LUP","ITF","USE"}`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if c[2].upper() in {"MENUS","SYS","MUSIC","HIT","WEK","END","LUP","ITF","USE"}:
            # 把右侧 `c; break` 计算得到的值保存到 `chosen`，后面的判断或输出会继续使用它。
            chosen = c; break
    # 检查条件 `chosen is None and candidates`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if chosen is None and candidates:
        # 把右侧 `candidates[0]` 计算得到的值保存到 `chosen`，后面的判断或输出会继续使用它。
        chosen = candidates[0]
    # 检查条件 `chosen`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if chosen:
        # 执行一次赋值：把 `chosen` 的结果放入左侧 `o,cnt,name,off` 对应的变量/字段。
        o,cnt,name,off = chosen
        # 把右侧 `parse_archive_entries(path, off, cnt)` 计算得到的值保存到 `entries`，后面的判断或输出会继续使用它。
        entries = parse_archive_entries(path, off, cnt)
        # 调用 `result.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        result.update({
            # 给当前结果字典的“directory_record_offset”字段填写 `o,`，这样导出的 JSON/表格能保留这一项证据。
            "directory_record_offset": o,
            # 给当前结果字典的“directory_name”字段填写 `name,`，这样导出的 JSON/表格能保留这一项证据。
            "directory_name": name,
            # 给当前结果字典的“entry_count”字段填写 `cnt,`，这样导出的 JSON/表格能保留这一项证据。
            "entry_count": cnt,
            # 给当前结果字典的“file_table_offset”字段填写 `off,`，这样导出的 JSON/表格能保留这一项证据。
            "file_table_offset": off,
            # 给当前结果字典的“entry_size”字段填写 `39,`，这样导出的 JSON/表格能保留这一项证据。
            "entry_size": 39,
            # 给当前结果字典的“valid_entries”字段填写 `sum(e["in_bounds"] for e in entries),`，这样导出的 JSON/表格能保留这一项证据。
            "valid_entries": sum(e["in_bounds"] for e in entries),
            # 给当前结果字典的“first_entries”字段填写 `entries[:8],`，这样导出的 JSON/表格能保留这一项证据。
            "first_entries": entries[:8],
            # 给当前结果字典的“entries”字段填写 `entries,`，这样导出的 JSON/表格能保留这一项证据。
            "entries": entries,
        })
    # 前面的条件分支都没有命中时，执行这个兜底分支。
    else:
        # 把右侧 `"未能可靠定位 39-byte 文件表；未进行猜测。"` 计算得到的值保存到 `result["warning"]`，后面的判断或输出会继续使用它。
        result["warning"] = "未能可靠定位 39-byte 文件表；未进行猜测。"
    # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return result


# 定义函数 `parse_slot_file`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_slot_file(path: Path) -> dict:
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b = path.read_bytes()
    # 检查条件 `len(b) < 8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(b) < 8:
        # 检测到不能继续的情况，主动抛出异常 `ValueError("文件过短")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("文件过短")
    # 把右侧 `u32(b,0)` 计算得到的值保存到 `slot_count`，后面的判断或输出会继续使用它。
    slot_count = u32(b,0)
    # 把右侧 `u32(b,4)` 计算得到的值保存到 `data_base`，后面的判断或输出会继续使用它。
    data_base = u32(b,4)
    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {
        # 给当前结果字典的“path”字段填写 `str(path), "size": len(b), "sha256": sha256(path),`，这样导出的 JSON/表格能保留这一项证据。
        "path": str(path), "size": len(b), "sha256": sha256(path),
        # 给当前结果字典的“slot_count”字段填写 `slot_count, "data_base": data_base,`，这样导出的 JSON/表格能保留这一项证据。
        "slot_count": slot_count, "data_base": data_base,
        # 给当前结果字典的“expected_table_end”字段填写 `8 + slot_count * 16,`，这样导出的 JSON/表格能保留这一项证据。
        "expected_table_end": 8 + slot_count * 16,
        # 给当前结果字典的“table_matches_data_base”字段填写 `(8 + slot_count * 16 == data_base),`，这样导出的 JSON/表格能保留这一项证据。
        "table_matches_data_base": (8 + slot_count * 16 == data_base),
        # 给当前结果字典的“nonempty_slots”字段填写 `[]`，这样导出的 JSON/表格能保留这一项证据。
        "nonempty_slots": []
    }
    # 检查条件 `slot_count > 100000 or 8 + slot_count*16 > len(b)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if slot_count > 100000 or 8 + slot_count*16 > len(b):
        # 把右侧 `"槽表结构不成立"` 计算得到的值保存到 `result["warning"]`，后面的判断或输出会继续使用它。
        result["warning"] = "槽表结构不成立"
        # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return result
    # 开始循环 `i in range(slot_count)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(slot_count):
        # 执行一次赋值：把 `struct.unpack_from("<4I",b,8+i*16)` 的结果放入左侧 `off,count,unk,size` 对应的变量/字段。
        off,count,unk,size = struct.unpack_from("<4I",b,8+i*16)
        # 检查条件 `count or size`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if count or size:
            # 执行这一条实际代码步骤：`result["nonempty_slots"].append({"slot":i,"offset":off,"count":count,"unknown":unk,"size":size})`；它与上下相邻语句共同完成当前函数的小任务。
            result["nonempty_slots"].append({"slot":i,"offset":off,"count":count,"unknown":unk,"size":size})
    # 检查条件 `path.suffix.lower() == ".eve" and result["nonempty_slots"]`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if path.suffix.lower() == ".eve" and result["nonempty_slots"]:
        # 把右侧 `result["nonempty_slots"][0]` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
        s = result["nonempty_slots"][0]
        # 执行一次赋值：把 `s["offset"],s["count"],s["size"]` 的结果放入左侧 `off,count,size` 对应的变量/字段。
        off,count,size = s["offset"],s["count"],s["size"]
        # 把右侧 `[]` 计算得到的值保存到 `offsets`，后面的判断或输出会继续使用它。
        offsets=[]
        # 检查条件 `off+count*4 <= len(b)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if off+count*4 <= len(b):
            # 把右侧 `[u32(b,off+i*4) for i in range(count)]` 计算得到的值保存到 `offsets`，后面的判断或输出会继续使用它。
            offsets=[u32(b,off+i*4) for i in range(count)]
        # 把右侧 `[]` 计算得到的值保存到 `cmds`，后面的判断或输出会继续使用它。
        cmds=[]
        # 开始循环 `po in offsets`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for po in offsets:
            # 检查条件 `po+4 <= len(b): cmds.append({"offset":po,"opcode"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if po+4 <= len(b): cmds.append({"offset":po,"opcode":u32(b,po)})
        # 把右侧 `offsets` 计算得到的值保存到 `result["first_nonempty_eve_offsets"]`，后面的判断或输出会继续使用它。
        result["first_nonempty_eve_offsets"] = offsets
        # 把右侧 `cmds` 计算得到的值保存到 `result["first_nonempty_eve_opcodes"]`，后面的判断或输出会继续使用它。
        result["first_nonempty_eve_opcodes"] = cmds
    # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return result


# 定义函数 `parse_map_sci`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_map_sci(path: Path) -> dict:
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b=path.read_bytes()
    # 把右侧 `{"path":str(path),"size":len(b),"sha256":sha256(path)}` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
    r={"path":str(path),"size":len(b),"sha256":sha256(path)}
    # 检查条件 `len(b) >= 0x6E`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(b) >= 0x6E:
        # 把右侧 `u16(b,0x6A)` 计算得到的值保存到 `r["confirmed_map_width_u16_at_0x6A"]`，后面的判断或输出会继续使用它。
        r["confirmed_map_width_u16_at_0x6A"] = u16(b,0x6A)
        # 把右侧 `u16(b,0x6C)` 计算得到的值保存到 `r["confirmed_map_height_u16_at_0x6C"]`，后面的判断或输出会继续使用它。
        r["confirmed_map_height_u16_at_0x6C"] = u16(b,0x6C)
    # 提取路径样字符串作辅助证据；正式 Map schema 见 v0.7B SCI 专项规格。
    # 把右侧 `[]` 计算得到的值保存到 `strings`，后面的判断或输出会继续使用它。
    strings=[]
    # 把右侧 `0` 计算得到的值保存到 `start`，后面的判断或输出会继续使用它。
    start=0
    # 只要条件 `start < len(b)` 仍然成立，就重复执行下面的循环体；循环体必须最终改变条件或主动退出。
    while start < len(b):
        # 把右侧 `b.find(b"\0",start)` 计算得到的值保存到 `end`，后面的判断或输出会继续使用它。
        end=b.find(b"\0",start)
        # 检查条件 `end<0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if end<0: break
        # 把右侧 `b[start:end]` 计算得到的值保存到 `seg`，后面的判断或输出会继续使用它。
        seg=b[start:end]
        # 检查条件 `len(seg)>=4 and all((x>=0x20 and x<0x7f) or x>=0x80 for x in seg)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(seg)>=4 and all((x>=0x20 and x<0x7f) or x>=0x80 for x in seg):
            # 创建变量 `try`（类型提示为 `s`），并把 `seg.decode('big5')` 的结果保存进去供后续步骤使用。
            try: s=seg.decode('big5')
            # 创建变量 `except`（类型提示为 `s`），并把 `seg.decode('latin1','replace')` 的结果保存进去供后续步骤使用。
            except: s=seg.decode('latin1','replace')
            # 检查条件 `'\\' in s or '/' in s: strings.append({"offset":start,"text"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if '\\' in s or '/' in s: strings.append({"offset":start,"text":s})
        # 把右侧 `end+1` 计算得到的值保存到 `start`，后面的判断或输出会继续使用它。
        start=end+1
    # 把右侧 `strings[:20]` 计算得到的值保存到 `r["path_like_strings"]`，后面的判断或输出会继续使用它。
    r["path_like_strings"] = strings[:20]
    # 把 `r` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return r


# 定义函数 `parse_npc_sci`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_npc_sci(path: Path) -> dict:
    # 把右侧 `path.read_bytes(); r={"path":str(path),"size":len(b),"sha256":sha256(path)}` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b=path.read_bytes(); r={"path":str(path),"size":len(b),"sha256":sha256(path)}
    # 检查条件 `len(b)%551==0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(b)%551==0:
        # 把右侧 `551` 计算得到的值保存到 `r["record_size"]`，后面的判断或输出会继续使用它。
        r["record_size"] = 551
        # 把右侧 `len(b)//551` 计算得到的值保存到 `r["record_count"]`，后面的判断或输出会继续使用它。
        r["record_count"] = len(b)//551
        # 把右侧 `[]` 计算得到的值保存到 `samples`，后面的判断或输出会继续使用它。
        samples=[]
        # 开始循环 `i in range(min(r["record_count"],16))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i in range(min(r["record_count"],16)):
            # 把右侧 `b[i*551:(i+1)*551]` 计算得到的值保存到 `c`，后面的判断或输出会继续使用它。
            c=b[i*551:(i+1)*551]
            # 把右侧 `c[:64].split(b"\0",1)[0]` 计算得到的值保存到 `name_raw`，后面的判断或输出会继续使用它。
            name_raw=c[:64].split(b"\0",1)[0]
            # 创建变量 `try`（类型提示为 `name`），并把 `name_raw.decode('big5')` 的结果保存进去供后续步骤使用。
            try: name=name_raw.decode('big5')
            # 创建变量 `except`（类型提示为 `name`），并把 `name_raw.decode('latin1','replace')` 的结果保存进去供后续步骤使用。
            except: name=name_raw.decode('latin1','replace')
            # 把右侧 `struct.unpack_from("<i",c,100)[0] if len(c)>=108 else None` 计算得到的值保存到 `x`，后面的判断或输出会继续使用它。
            x=struct.unpack_from("<i",c,100)[0] if len(c)>=108 else None
            # 把右侧 `struct.unpack_from("<i",c,104)[0] if len(c)>=108 else None` 计算得到的值保存到 `y`，后面的判断或输出会继续使用它。
            y=struct.unpack_from("<i",c,104)[0] if len(c)>=108 else None
            # 把右侧 `c[0x7B:0x7B+128].split(b"\0",1)[0]` 计算得到的值保存到 `path_raw`，后面的判断或输出会继续使用它。
            path_raw=c[0x7B:0x7B+128].split(b"\0",1)[0]
            # 创建变量 `try`（类型提示为 `spr`），并把 `path_raw.decode('big5')` 的结果保存进去供后续步骤使用。
            try: spr=path_raw.decode('big5')
            # 创建变量 `except`（类型提示为 `spr`），并把 `path_raw.decode('latin1','replace')` 的结果保存进去供后续步骤使用。
            except: spr=path_raw.decode('latin1','replace')
            # 把这一条新结果追加到列表 `samples`；不会覆盖前面已经收集的记录。
            samples.append({"record":i,"name":name,"confirmed_world_x_i32_at_0x64":x,"confirmed_world_y_i32_at_0x68":y,"sprite_path_at_0x7B":spr})
        # 把右侧 `samples` 计算得到的值保存到 `r["sample_records"]`，后面的判断或输出会继续使用它。
        r["sample_records"] = samples
    # 前面的条件分支都没有命中时，执行这个兜底分支。
    else:
        # 把右侧 `"长度不是 551 的整数倍；不要套用已确认 NPC SCI 记录格式。"` 计算得到的值保存到 `r["warning"]`，后面的判断或输出会继续使用它。
        r["warning"] = "长度不是 551 的整数倍；不要套用已确认 NPC SCI 记录格式。"
    # 把 `r` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return r


# 定义函数 `parse_sf2`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_sf2(path: Path) -> dict:
    # 把右侧 `path.read_bytes(); r={"path":str(path),"size":len(b),"sha256":sha256(path)}` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b=path.read_bytes(); r={"path":str(path),"size":len(b),"sha256":sha256(path)}
    # 把右侧 `b[:6].hex(" ") if len(b)>=6 else b.hex(" ")` 计算得到的值保存到 `r["magic"]`，后面的判断或输出会继续使用它。
    r["magic"] = b[:6].hex(" ") if len(b)>=6 else b.hex(" ")
    # 检查条件 `len(b)>=0x14 and b[:4]==b"SF2\x05"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(b)>=0x14 and b[:4]==b"SF2\x05":
        # 把右侧 `u16(b,0x07)` 计算得到的值保存到 `r["tile_width_u16_at_0x07"]`，后面的判断或输出会继续使用它。
        r["tile_width_u16_at_0x07"] = u16(b,0x07)
        # 把右侧 `u16(b,0x09)` 计算得到的值保存到 `r["tile_height_u16_at_0x09"]`，后面的判断或输出会继续使用它。
        r["tile_height_u16_at_0x09"] = u16(b,0x09)
        # 把右侧 `u32(b,0x0C)` 计算得到的值保存到 `r["canvas_width_u32_at_0x0C"]`，后面的判断或输出会继续使用它。
        r["canvas_width_u32_at_0x0C"] = u32(b,0x0C)
        # 把右侧 `u32(b,0x10)` 计算得到的值保存到 `r["canvas_height_u32_at_0x10"]`，后面的判断或输出会继续使用它。
        r["canvas_height_u32_at_0x10"] = u32(b,0x10)
        # 检查条件 `len(b)>=0x38`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(b)>=0x38:
            # 把右侧 `u32(b,0x34)` 计算得到的值保存到 `r["u32_at_0x34"]`，后面的判断或输出会继续使用它。
            r["u32_at_0x34"] = u32(b,0x34)
    # 把 `r` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return r


# 定义函数 `parse_shop`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_shop(path: Path) -> dict:
    # 把右侧 `path.stat().st_size` 计算得到的值保存到 `size`，后面的判断或输出会继续使用它。
    size=path.stat().st_size
    # 把右侧 `{"path":str(path),"size":size,"sha256":sha256(path),"confirmed_record_size…` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
    r={"path":str(path),"size":size,"sha256":sha256(path),"confirmed_record_size":648}
    # 把右侧 `size//648 if size%648==0 else None` 计算得到的值保存到 `r["record_count_if_648"]`，后面的判断或输出会继续使用它。
    r["record_count_if_648"] = size//648 if size%648==0 else None
    # 把 `r` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return r


# 定义函数 `parse_font24`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_font24(path: Path) -> dict:
    # 把右侧 `path.stat().st_size` 计算得到的值保存到 `size`，后面的判断或输出会继续使用它。
    size=path.stat().st_size
    # 把 `{"path":str(path),"size":size,"sha256":sha256(path),"legacy_record_size_observed":216,` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"path":str(path),"size":size,"sha256":sha256(path),"legacy_record_size_observed":216,
            # 给当前结果字典的“record_count_if_216”字段填写 `size//216 if size%216==0 else None,`，这样导出的 JSON/表格能保留这一项证据。
            "record_count_if_216": size//216 if size%216==0 else None,
            # 给当前结果字典的“runtime_policy”字段填写 `"仅作原版视觉/格式参考；Remastered 字体运行时不依赖原 FNT。"}`，这样导出的 JSON/表格能保留这一项证据。
            "runtime_policy":"仅作原版视觉/格式参考；Remastered 字体运行时不依赖原 FNT。"}


# 定义函数 `scan_tsf_append`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def scan_tsf_append(save_dir: Path, shop_path: Optional[Path]) -> dict:
    # 把右侧 `{"save_dir":str(save_dir),"files":[]}` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
    r={"save_dir":str(save_dir),"files":[]}
    # 把右侧 `shop_path.read_bytes() if shop_path and shop_path.exists() else None` 计算得到的值保存到 `shop`，后面的判断或输出会继续使用它。
    shop=shop_path.read_bytes() if shop_path and shop_path.exists() else None
    # 开始循环 `p in sorted(save_dir.glob("*.TSF"), key=lambda q:q.name.lower())`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for p in sorted(save_dir.glob("*.TSF"), key=lambda q:q.name.lower()):
        # 把右侧 `p.stat().st_size` 计算得到的值保存到 `size`，后面的判断或输出会继续使用它。
        size=p.stat().st_size
        # 把右侧 `{"name":p.name,"size":size}` 计算得到的值保存到 `item`，后面的判断或输出会继续使用它。
        item={"name":p.name,"size":size}
        # 检查条件 `shop and size>=len(shop)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if shop and size>=len(shop):
            # 把右侧 `p.read_bytes().find(shop)` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
            idx=p.read_bytes().find(shop)
            # 检查条件 `idx>=0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if idx>=0: item["contains_shopitem_exact_at"] = idx
        # 执行这一条实际代码步骤：`r["files"].append(item)`；它与上下相邻语句共同完成当前函数的小任务。
        r["files"].append(item)
    # 把 `r` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return r


# 定义函数 `inspect_nested_save_zip`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def inspect_nested_save_zip(zip_path: Path, shop_path: Optional[Path]) -> dict:
    # 把右侧 `{"path":str(zip_path),"size":zip_path.stat().st_size,"sha256":sha256(zip_p…` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
    r={"path":str(zip_path),"size":zip_path.stat().st_size,"sha256":sha256(zip_path),"entries":0,"tsf_sizes":{},"shopitem_exact_matches":[]}
    # 把右侧 `shop_path.read_bytes() if shop_path and shop_path.exists() else None` 计算得到的值保存到 `shop`，后面的判断或输出会继续使用它。
    shop=shop_path.read_bytes() if shop_path and shop_path.exists() else None
    # 进入受管理的资源作用域 `zipfile.ZipFile(zip_path,'r') as z`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
    with zipfile.ZipFile(zip_path,'r') as z:
        # 把右侧 `[i for i in z.infolist() if not i.is_dir()]` 计算得到的值保存到 `infos`，后面的判断或输出会继续使用它。
        infos=[i for i in z.infolist() if not i.is_dir()]
        # 把右侧 `len(infos)` 计算得到的值保存到 `r["entries"]`，后面的判断或输出会继续使用它。
        r["entries"]=len(infos)
        # 开始循环 `info in infos`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for info in infos:
            # 检查条件 `info.filename.lower().endswith('.tsf')`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if info.filename.lower().endswith('.tsf'):
                # 把右侧 `z.read(info)` 计算得到的值保存到 `data`，后面的判断或输出会继续使用它。
                data=z.read(info)
                # 执行一次赋值：把 `1` 的结果放入左侧 `r["tsf_sizes"].setdefault(str(len(data)),0); r["tsf_si…` 对应的变量/字段。
                r["tsf_sizes"].setdefault(str(len(data)),0); r["tsf_sizes"][str(len(data))]+=1
                # 检查条件 `shop`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if shop:
                    # 把右侧 `data.find(shop)` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
                    idx=data.find(shop)
                    # 检查条件 `idx>=0: r["shopitem_exact_matches"].append({"name":info.filename,"offset"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                    if idx>=0: r["shopitem_exact_matches"].append({"name":info.filename,"offset":idx})
    # 把 `r` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return r


# 定义函数 `main`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def main() -> int:
    # 把右侧 `argparse.ArgumentParser(description="《幽城幻剑录》兼容引擎研究用只读样本结构验证器")` 计算得到的值保存到 `ap`，后面的判断或输出会继续使用它。
    ap=argparse.ArgumentParser(description="《幽城幻剑录》兼容引擎研究用只读样本结构验证器")
    # 执行一次赋值：把 `"?", help="原版/样本资源根目录；例如包含 Map、public、fight 等目录的位置")` 的结果放入左侧 `ap.add_argument("资源目录", nargs` 对应的变量/字段。
    ap.add_argument("资源目录", nargs="?", help="原版/样本资源根目录；例如包含 Map、public、fight 等目录的位置")
    # 执行一次赋值：把 `"可选：RPG.exe 路径，仅读取 PE 基线")` 的结果放入左侧 `ap.add_argument("--exe", help` 对应的变量/字段。
    ap.add_argument("--exe", help="可选：RPG.exe 路径，仅读取 PE 基线")
    # 执行一次赋值：把 `"json_out", help="可选：将完整结果写入 JSON 文件")` 的结果放入左侧 `ap.add_argument("--json", dest` 对应的变量/字段。
    ap.add_argument("--json", dest="json_out", help="可选：将完整结果写入 JSON 文件")
    # 执行一次赋值：把 `"version", version=f"幽城样本结构验证器 {VERSION}")` 的结果放入左侧 `ap.add_argument("--版本", action` 对应的变量/字段。
    ap.add_argument("--版本", action="version", version=f"幽城样本结构验证器 {VERSION}")
    # 把右侧 `ap.parse_args()` 计算得到的值保存到 `args`，后面的判断或输出会继续使用它。
    args=ap.parse_args()

    # 检查条件 `not args.资源目录 and not args.exe`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not args.资源目录 and not args.exe:
        # 调用 `ap.error` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        ap.error("至少提供资源目录或 --exe")
    # 把右侧 `{"tool_version":VERSION,"warnings":["本工具只验证已经建立证据的结构，不会把候选字段自动升级为已确认字段。"]}` 计算得到的值保存到 `report`，后面的判断或输出会继续使用它。
    report={"tool_version":VERSION,"warnings":["本工具只验证已经建立证据的结构，不会把候选字段自动升级为已确认字段。"]}
    # 把右侧 `Path(args.资源目录).resolve() if args.资源目录 else None` 计算得到的值保存到 `root`，后面的判断或输出会继续使用它。
    root=Path(args.资源目录).resolve() if args.资源目录 else None
    # 把右侧 `index_files(root) if root else {}` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
    idx=index_files(root) if root else {}

    # 检查条件 `args.exe`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if args.exe:
        # 创建变量 `try`（类型提示为 `report["pe"]`），并把 `pe_info(Path(args.exe).resolve())` 的结果保存进去供后续步骤使用。
        try: report["pe"] = pe_info(Path(args.exe).resolve())
        # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
        except Exception as e: report["pe_error"] = str(e)

    # 检查条件 `root`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if root:
        # 把右侧 `str(root)` 计算得到的值保存到 `report["resource_root"]`，后面的判断或输出会继续使用它。
        report["resource_root"] = str(root)
        # 开始循环 `key,names in {`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for key,names in {
            # 给当前结果字典的“menus_archive”字段填写 `("menusdir.dat",),`，这样导出的 JSON/表格能保留这一项证据。
            "menus_archive":("menusdir.dat",),
            # 给当前结果字典的“sys_archive”字段填写 `("sys.dat",),`，这样导出的 JSON/表格能保留这一项证据。
            "sys_archive":("sys.dat",),
            # 给当前结果字典的“music_archive”字段填写 `("music.dat",),`，这样导出的 JSON/表格能保留这一项证据。
            "music_archive":("music.dat",),
        # 继续填写当前数据结构或参数列表中的一项：`}.items():`。
        }.items():
            # 把右侧 `first(idx,*names)` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
            p=first(idx,*names)
            # 检查条件 `p`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if p:
                # 创建变量 `try`（类型提示为 `report[key]`），并把 `detect_known_archive(p)` 的结果保存进去供后续步骤使用。
                try: report[key]=detect_known_archive(p)
                # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
                except Exception as e: report[key+"_error"]=str(e)

        # 把右侧 `first(idx,"mp2412a.eve") or next((p for ps in idx.values() for p in ps if …` 计算得到的值保存到 `eve`，后面的判断或输出会继续使用它。
        eve=first(idx,"mp2412a.eve") or next((p for ps in idx.values() for p in ps if p.suffix.lower()=='.eve'),None)
        # 把右侧 `first(idx,"mp2412a.msg") or next((p for ps in idx.values() for p in ps if …` 计算得到的值保存到 `msg`，后面的判断或输出会继续使用它。
        msg=first(idx,"mp2412a.msg") or next((p for ps in idx.values() for p in ps if p.suffix.lower()=='.msg'),None)
        # 检查条件 `eve`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if eve:
            # 创建变量 `try`（类型提示为 `report["eve_sample"]`），并把 `parse_slot_file(eve)` 的结果保存进去供后续步骤使用。
            try: report["eve_sample"]=parse_slot_file(eve)
            # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except Exception as e: report["eve_sample_error"]=str(e)
        # 检查条件 `msg`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if msg:
            # 创建变量 `try`（类型提示为 `report["msg_sample"]`），并把 `parse_slot_file(msg)` 的结果保存进去供后续步骤使用。
            try: report["msg_sample"]=parse_slot_file(msg)
            # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except Exception as e: report["msg_sample_error"]=str(e)

        # 把右侧 `first(idx,"mpmp2412a.sci","mpmp2506.sci")` 计算得到的值保存到 `mapsci`，后面的判断或输出会继续使用它。
        mapsci=first(idx,"mpmp2412a.sci","mpmp2506.sci")
        # 检查条件 `mapsci`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if mapsci:
            # 创建变量 `try`（类型提示为 `report["map_sci_sample"]`），并把 `parse_map_sci(mapsci)` 的结果保存进去供后续步骤使用。
            try: report["map_sci_sample"]=parse_map_sci(mapsci)
            # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except Exception as e: report["map_sci_sample_error"]=str(e)

        # 把右侧 `None` 计算得到的值保存到 `npc`，后面的判断或输出会继续使用它。
        npc=None
        # 开始循环 `ps in idx.values()`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for ps in idx.values():
            # 开始循环 `p in ps`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for p in ps:
                # 检查条件 `p.suffix.lower()=='.sci' and p.stat().st_size%551==0 and p.stat().st_size>=551`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if p.suffix.lower()=='.sci' and p.stat().st_size%551==0 and p.stat().st_size>=551:
                    # 把右侧 `p; break` 计算得到的值保存到 `npc`，后面的判断或输出会继续使用它。
                    npc=p; break
            # 检查条件 `npc`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if npc: break
        # 检查条件 `npc`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if npc:
            # 创建变量 `try`（类型提示为 `report["npc_sci_sample"]`），并把 `parse_npc_sci(npc)` 的结果保存进去供后续步骤使用。
            try: report["npc_sci_sample"]=parse_npc_sci(npc)
            # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except Exception as e: report["npc_sci_sample_error"]=str(e)

        # 把右侧 `first(idx,"mousewait.sf2","fanout.sf2","men0000.sf2")` 计算得到的值保存到 `sf2`，后面的判断或输出会继续使用它。
        sf2=first(idx,"mousewait.sf2","fanout.sf2","men0000.sf2")
        # 检查条件 `sf2`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if sf2:
            # 创建变量 `try`（类型提示为 `report["sf2_sample"]`），并把 `parse_sf2(sf2)` 的结果保存进去供后续步骤使用。
            try: report["sf2_sample"]=parse_sf2(sf2)
            # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except Exception as e: report["sf2_sample_error"]=str(e)

        # 把右侧 `first(idx,"shopitem.dat")` 计算得到的值保存到 `shop`，后面的判断或输出会继续使用它。
        shop=first(idx,"shopitem.dat")
        # 检查条件 `shop`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if shop:
            # 把右侧 `parse_shop(shop)` 计算得到的值保存到 `report["shopitem"]`，后面的判断或输出会继续使用它。
            report["shopitem"]=parse_shop(shop)
        # 把右侧 `first(idx,"font24.fnt")` 计算得到的值保存到 `font`，后面的判断或输出会继续使用它。
        font=first(idx,"font24.fnt")
        # 检查条件 `font`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if font:
            # 把右侧 `parse_font24(font)` 计算得到的值保存到 `report["font24"]`，后面的判断或输出会继续使用它。
            report["font24"]=parse_font24(font)

        # 把右侧 `None` 计算得到的值保存到 `nested`，后面的判断或输出会继续使用它。
        nested=None
        # 开始循环 `ps in idx.values()`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for ps in idx.values():
            # 开始循环 `p in ps`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for p in ps:
                # 检查条件 `p.suffix.lower()=='.zip'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if p.suffix.lower()=='.zip':
                    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
                    try:
                        # 进入受管理的资源作用域 `zipfile.ZipFile(p) as z`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
                        with zipfile.ZipFile(p) as z:
                            # 检查条件 `sum(1 for i in z.infolist() if i.filename.lower().endswith('.tsf'))>=10`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                            if sum(1 for i in z.infolist() if i.filename.lower().endswith('.tsf'))>=10:
                                # 把右侧 `p; break` 计算得到的值保存到 `nested`，后面的判断或输出会继续使用它。
                                nested=p; break
                    # 执行这一条实际代码步骤：`except: pass`；它与上下相邻语句共同完成当前函数的小任务。
                    except: pass
            # 检查条件 `nested`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if nested: break
        # 检查条件 `nested`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if nested:
            # 创建变量 `try`（类型提示为 `report["flow_save_zip"]`），并把 `inspect_nested_save_zip(nested,shop)` 的结果保存进去供后续步骤使用。
            try: report["flow_save_zip"]=inspect_nested_save_zip(nested,shop)
            # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except Exception as e: report["flow_save_zip_error"]=str(e)

        # 把右侧 `[]` 计算得到的值保存到 `save_dirs`，后面的判断或输出会继续使用它。
        save_dirs=[]
        # 开始循环 `d,dirs,files in os.walk(root)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for d,dirs,files in os.walk(root):
            # 检查条件 `sum(1 for f in files if f.lower().endswith('.tsf'))>=3`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if sum(1 for f in files if f.lower().endswith('.tsf'))>=3:
                # 把这一条新结果追加到列表 `save_dirs`；不会覆盖前面已经收集的记录。
                save_dirs.append(Path(d))
        # 检查条件 `save_dirs`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if save_dirs:
            # 把右侧 `[scan_tsf_append(d,shop) for d in save_dirs[:5]]` 计算得到的值保存到 `report["save_dirs"]`，后面的判断或输出会继续使用它。
            report["save_dirs"]=[scan_tsf_append(d,shop) for d in save_dirs[:5]]

    # 把右侧 `json.dumps(report,ensure_ascii=False,indent=2)` 计算得到的值保存到 `text`，后面的判断或输出会继续使用它。
    text=json.dumps(report,ensure_ascii=False,indent=2)
    # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
    print(text)
    # 检查条件 `args.json_out`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if args.json_out:
        # 把右侧 `Path(args.json_out)` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
        out=Path(args.json_out)
        # 执行一次赋值：把 `"utf-8")` 的结果放入左侧 `out.write_text(text+"\n",encoding` 对应的变量/字段。
        out.write_text(text+"\n",encoding="utf-8")
        # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
        print(f"\n已写入：{out}")
    # 把 `0` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return 0

# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__ == "__main__":
    # 检测到不能继续的情况，主动抛出异常 `SystemExit(main())`，让上层明确知道数据或参数不符合要求。
    raise SystemExit(main())
