#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
幽城TSF与GameState调查器 0.7C.0

只读分析《天地劫外传：幽城幻剑录》原版 TSF 存档。
仅使用 Python 标准库，不修改输入文件。

本工具坚持“Serializer 白名单”原则：
- 只把已有原版 Save/Load 代码证据的字段解析为语义状态；
- 不把固定 57,521-byte Core 中的未初始化/指针垃圾误当成 GameState；
- Legacy 特殊/非对称数据单独保留，不合并到 Canonical GameState。
"""
# 启用较新的 Python 注解语法行为，让类型注解先按文字保存，避免旧版本在定义阶段立即求值。
from __future__ import annotations

# 导入 `argparse` 模块；这里只取得标准库功能，不会修改游戏文件。
import argparse
# 导入 `hashlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import hashlib
# 导入 `json` 模块；这里只取得标准库功能，不会修改游戏文件。
import json
# 导入 `re` 模块；这里只取得标准库功能，不会修改游戏文件。
import re
# 导入 `struct` 模块；这里只取得标准库功能，不会修改游戏文件。
import struct
# 导入 `sys` 模块；这里只取得标准库功能，不会修改游戏文件。
import sys
# 从 `collections` 模块导入 `Counter`，后面的代码会直接使用这些现成组件。
from collections import Counter
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path
# 从 `typing` 模块导入 `Any`，后面的代码会直接使用这些现成组件。
from typing import Any
# 从 `zipfile` 模块导入 `ZipFile`，后面的代码会直接使用这些现成组件。
from zipfile import ZipFile
# 从 `xml.etree` 模块导入 `ElementTree as ET`，后面的代码会直接使用这些现成组件。
from xml.etree import ElementTree as ET

# 执行一次赋值：把 `"0.7C.0"` 的结果放入左侧 `版本` 对应的变量/字段。
版本 = "0.7C.0"

# 执行一次赋值：把 `0xE0B1 # 57,521` 的结果放入左侧 `核心大小` 对应的变量/字段。
核心大小 = 0xE0B1               # 57,521
# 执行一次赋值：把 `0x27D8 # 10,200` 的结果放入左侧 `世界块固定大小` 对应的变量/字段。
世界块固定大小 = 0x27D8          # 10,200
# 执行一次赋值：把 `0x288 # 648` 的结果放入左侧 `商店记录大小` 对应的变量/字段。
商店记录大小 = 0x288             # 648
# 执行一次赋值：把 `57` 的结果放入左侧 `商店标准记录数` 对应的变量/字段。
商店标准记录数 = 57
# 执行一次赋值：把 `0x350 # 848` 的结果放入左侧 `角色记录大小` 对应的变量/字段。
角色记录大小 = 0x350             # 848
# 执行一次赋值：把 `9` 的结果放入左侧 `角色保存记录数` 对应的变量/字段。
角色保存记录数 = 9
# 执行一次赋值：把 `8` 的结果放入左侧 `角色权威加载记录数` 对应的变量/字段。
角色权威加载记录数 = 8
# 执行一次赋值：把 `0x5DE8 # 24,040` 的结果放入左侧 `库存银行大小` 对应的变量/字段。
库存银行大小 = 0x5DE8            # 24,040
# 执行一次赋值：把 `0xFA4 # 4,004` 的结果放入左侧 `库存类别大小` 对应的变量/字段。
库存类别大小 = 0xFA4             # 4,004
# 执行一次赋值：把 `6` 的结果放入左侧 `库存类别数` 对应的变量/字段。
库存类别数 = 6
# 执行一次赋值：把 `500` 的结果放入左侧 `每类最大条目` 对应的变量/字段。
每类最大条目 = 500
# 执行一次赋值：把 `5000` 的结果放入左侧 `游戏变量数量` 对应的变量/字段。
游戏变量数量 = 5000

# 执行一次赋值：把 `0x002C` 的结果放入左侧 `偏移_库存银行0` 对应的变量/字段。
偏移_库存银行0 = 0x002C
# 执行一次赋值：把 `0x5E14` 的结果放入左侧 `偏移_库存银行1` 对应的变量/字段。
偏移_库存银行1 = 0x5E14
# 执行一次赋值：把 `0xBBFC` 的结果放入左侧 `偏移_当前库存银行` 对应的变量/字段。
偏移_当前库存银行 = 0xBBFC
# 执行一次赋值：把 `0xBC00` 的结果放入左侧 `偏移_角色记录` 对应的变量/字段。
偏移_角色记录 = 0xBC00
# 执行一次赋值：把 `0xDD20` 的结果放入左侧 `偏移_队伍角色ID` 对应的变量/字段。
偏移_队伍角色ID = 0xDD20
# 执行一次赋值：把 `0xDD48` 的结果放入左侧 `偏移_队伍编成槽` 对应的变量/字段。
偏移_队伍编成槽 = 0xDD48
# 执行一次赋值：把 `0xDD70` 的结果放入左侧 `偏移_队伍数量` 对应的变量/字段。
偏移_队伍数量 = 0xDD70
# 执行一次赋值：把 `0xDD91` 的结果放入左侧 `偏移_MP3音量` 对应的变量/字段。
偏移_MP3音量 = 0xDD91
# 执行一次赋值：把 `0xDD95` 的结果放入左侧 `偏移_WAV音量` 对应的变量/字段。
偏移_WAV音量 = 0xDD95
# 执行一次赋值：把 `0xDD99` 的结果放入左侧 `偏移_旧核心编码标记` 对应的变量/字段。
偏移_旧核心编码标记 = 0xDD99
# 执行一次赋值：把 `0xE0A9` 的结果放入左侧 `偏移_商店块长度` 对应的变量/字段。
偏移_商店块长度 = 0xE0A9
# 执行一次赋值：把 `0xE0AD` 的结果放入左侧 `偏移_世界块长度` 对应的变量/字段。
偏移_世界块长度 = 0xE0AD

# 执行一次赋值：把 `每类最大条目 * 4` 的结果放入左侧 `库存数量区字节数` 对应的变量/字段。
库存数量区字节数 = 每类最大条目 * 4
# 执行一次赋值：把 `库存数量区字节数` 的结果放入左侧 `库存ID区偏移` 对应的变量/字段。
库存ID区偏移 = 库存数量区字节数
# 执行一次赋值：把 `0xFA0` 的结果放入左侧 `库存计数偏移` 对应的变量/字段。
库存计数偏移 = 0xFA0
# 执行一次赋值：把 `0x5DD8` 的结果放入左侧 `库存金钱偏移` 对应的变量/字段。
库存金钱偏移 = 0x5DD8

# 执行一次赋值：把 `0x0000` 的结果放入左侧 `世界_场景描述偏移` 对应的变量/字段。
世界_场景描述偏移 = 0x0000
# 执行一次赋值：把 `50` 的结果放入左侧 `世界_场景描述长度` 对应的变量/字段。
世界_场景描述长度 = 50
# 执行一次赋值：把 `0x0032` 的结果放入左侧 `世界_角色X偏移` 对应的变量/字段。
世界_角色X偏移 = 0x0032
# 执行一次赋值：把 `0x0036` 的结果放入左侧 `世界_角色Y偏移` 对应的变量/字段。
世界_角色Y偏移 = 0x0036
# 执行一次赋值：把 `0x003A` 的结果放入左侧 `世界_角色状态偏移` 对应的变量/字段。
世界_角色状态偏移 = 0x003A
# 执行一次赋值：把 `0x003E` 的结果放入左侧 `世界_当前受控索引偏移` 对应的变量/字段。
世界_当前受控索引偏移 = 0x003E
# 执行一次赋值：把 `0x00C8` 的结果放入左侧 `世界_变量偏移` 对应的变量/字段。
世界_变量偏移 = 0x00C8


# 定义函数 `_u32`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _u32(data: bytes | bytearray, off: int) -> int:
    # 把 `struct.unpack_from("<I", data, off)[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return struct.unpack_from("<I", data, off)[0]


# 定义函数 `_i32`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _i32(data: bytes | bytearray, off: int) -> int:
    # 把 `struct.unpack_from("<i", data, off)[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return struct.unpack_from("<i", data, off)[0]


# 定义函数 `_i16`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _i16(data: bytes | bytearray, off: int) -> int:
    # 把 `struct.unpack_from("<h", data, off)[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return struct.unpack_from("<h", data, off)[0]


# 定义函数 `_sha`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _sha(data: bytes) -> str:
    # 把 `hashlib.sha256(data).hexdigest()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return hashlib.sha256(data).hexdigest()


# 定义函数 `_hex`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _hex(data: bytes) -> str:
    # 把 `data.hex()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return data.hex()


# 定义函数 `_decode_legacy_text`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _decode_legacy_text(raw: bytes) -> str:
    # 把右侧 `raw.split(b"\x00", 1)[0]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
    raw = raw.split(b"\x00", 1)[0]
    # 开始循环 `enc in ("cp950", "big5", "ascii")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for enc in ("cp950", "big5", "ascii"):
        # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
        try:
            # 把 `raw.decode(enc)` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
            return raw.decode(enc)
        # 捕获 `UnicodeDecodeError` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
        except UnicodeDecodeError:
            # 这里故意什么也不做，只保留语法上的空分支位置。
            pass
    # 把 `raw.decode("latin1", errors="replace")` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return raw.decode("latin1", errors="replace")


# 定义函数 `_xor255_inplace`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _xor255_inplace(data: bytearray) -> None:
    # 原版统一解码函数 0x004284D0
    # 开始循环 `i in range(len(data))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(len(data)):
        # 执行一次赋值：把 `i % 255` 的结果放入左侧 `data[i] ^` 对应的变量/字段。
        data[i] ^= i % 255


# 定义函数 `_json_dump`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _json_dump(obj: Any, out: Path | None) -> None:
    # 把右侧 `json.dumps(obj, ensure_ascii=False, indent=2)` 计算得到的值保存到 `text`，后面的判断或输出会继续使用它。
    text = json.dumps(obj, ensure_ascii=False, indent=2)
    # 检查条件 `out`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if out:
        # 执行一次赋值：把 `True, exist_ok=True)` 的结果放入左侧 `out.parent.mkdir(parents` 对应的变量/字段。
        out.parent.mkdir(parents=True, exist_ok=True)
        # 执行一次赋值：把 `"utf-8")` 的结果放入左侧 `out.write_text(text + "\n", encoding` 对应的变量/字段。
        out.write_text(text + "\n", encoding="utf-8")
        # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
        print(f"已写出：{out}")
    # 前面的条件分支都没有命中时，执行这个兜底分支。
    else:
        # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
        print(text)


# 定义函数 `_extract_walkthrough_labels`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _extract_walkthrough_labels(docx: Path | None) -> dict[int, str]:
    # 检查条件 `not docx or not docx.is_file()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not docx or not docx.is_file():
        # 把 `{}` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {}
    # 创建变量 `labels`（类型提示为 `dict[int, str]`），并把 `{}` 的结果保存进去供后续步骤使用。
    labels: dict[int, str] = {}
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 进入受管理的资源作用域 `ZipFile(docx) as zf`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
        with ZipFile(docx) as zf:
            # 把右侧 `ET.fromstring(zf.read("word/document.xml"))` 计算得到的值保存到 `root`，后面的判断或输出会继续使用它。
            root = ET.fromstring(zf.read("word/document.xml"))
        # 把右侧 `{"w": "http://schemas.openxmlformats.org/wordprocessingml/2006/main"}` 计算得到的值保存到 `ns`，后面的判断或输出会继续使用它。
        ns = {"w": "http://schemas.openxmlformats.org/wordprocessingml/2006/main"}
        # 开始循环 `p in root.findall(".//w:p", ns)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for p in root.findall(".//w:p", ns):
            # 把右侧 `"".join((t.text or "") for t in p.findall(".//w:t", ns)).strip()` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
            s = "".join((t.text or "") for t in p.findall(".//w:t", ns)).strip()
            # 把右侧 `re.search(r"「[○◎●]\s*(\d{3})」(.*)", s)` 计算得到的值保存到 `m`，后面的判断或输出会继续使用它。
            m = re.search(r"「[○◎●]\s*(\d{3})」(.*)", s)
            # 检查条件 `m`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if m:
                # 执行一次赋值：把 `m.group(2).strip()` 的结果放入左侧 `labels[int(m.group(1))]` 对应的变量/字段。
                labels[int(m.group(1))] = m.group(2).strip()
    # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception as e:  # 说明文件是辅助信息，不应使 TSF 解析失败
        # 执行一次赋值：把 `sys.stderr)` 的结果放入左侧 `print(f"警告：流程说明读取失败：{e}", file` 对应的变量/字段。
        print(f"警告：流程说明读取失败：{e}", file=sys.stderr)
    # 把 `labels` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return labels


# 定义函数 `_save_index`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _save_index(path: Path) -> int:
    # 检查条件 `path.stem.lower() == "newgame"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if path.stem.lower() == "newgame":
        # 把 `0` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return 0
    # 把右侧 `re.search(r"(\d+)", path.stem)` 计算得到的值保存到 `m`，后面的判断或输出会继续使用它。
    m = re.search(r"(\d+)", path.stem)
    # 把 `int(m.group(1)) if m else 10**9` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return int(m.group(1)) if m else 10**9


# 定义函数 `_list_saves`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _list_saves(root: Path) -> list[Path]:
    # 把右侧 `[p for p in root.glob("*.TSF") if p.is_file()]` 计算得到的值保存到 `files`，后面的判断或输出会继续使用它。
    files = [p for p in root.glob("*.TSF") if p.is_file()]
    # 把 `sorted(files, key=lambda p: (_save_index(p), p.name.lower()))` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return sorted(files, key=lambda p: (_save_index(p), p.name.lower()))


# 定义函数 `_parse_inventory_bank`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _parse_inventory_bank(core: bytes, base: int, bank_index: int) -> tuple[dict[str, Any], list[str]]:
    # 创建变量 `warnings`（类型提示为 `list[str]`），并把 `[]` 的结果保存进去供后续步骤使用。
    warnings: list[str] = []
    # 把右侧 `[]` 计算得到的值保存到 `categories`，后面的判断或输出会继续使用它。
    categories = []
    # 开始循环 `cat in range(库存类别数)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for cat in range(库存类别数):
        # 把右侧 `base + cat * 库存类别大小` 计算得到的值保存到 `off`，后面的判断或输出会继续使用它。
        off = base + cat * 库存类别大小
        # 把右侧 `_u32(core, off + 库存计数偏移)` 计算得到的值保存到 `count`，后面的判断或输出会继续使用它。
        count = _u32(core, off + 库存计数偏移)
        # 检查条件 `count > 每类最大条目`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if count > 每类最大条目:
            # 执行一次赋值：把 `{count} > 500；仅按500上限展示")` 的结果放入左侧 `warnings.append(f"库存 Bank{bank_index} 类别{cat} count` 对应的变量/字段。
            warnings.append(f"库存 Bank{bank_index} 类别{cat} count={count} > 500；仅按500上限展示")
        # 把右侧 `min(count, 每类最大条目)` 计算得到的值保存到 `usable`，后面的判断或输出会继续使用它。
        usable = min(count, 每类最大条目)
        # 把右侧 `[_u32(core, off + i * 4) for i in range(usable)]` 计算得到的值保存到 `quantities`，后面的判断或输出会继续使用它。
        quantities = [_u32(core, off + i * 4) for i in range(usable)]
        # 把右侧 `[_u32(core, off + 库存ID区偏移 + i * 4) for i in range(usable)]` 计算得到的值保存到 `item_ids`，后面的判断或输出会继续使用它。
        item_ids = [_u32(core, off + 库存ID区偏移 + i * 4) for i in range(usable)]
        # 把右侧 `[` 计算得到的值保存到 `entries`，后面的判断或输出会继续使用它。
        entries = [
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"item_id": item_ids[i], "quantity": quantities[i]}
            # 开始循环 `i in range(usable)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for i in range(usable)
        ]
        # 把右侧 `core[off:off + 库存类别大小]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw = core[off:off + 库存类别大小]
        # 把这一条新结果追加到列表 `categories`；不会覆盖前面已经收集的记录。
        categories.append({
            # 给当前结果字典的“category”字段填写 `cat,`，这样导出的 JSON/表格能保留这一项证据。
            "category": cat,
            # 给当前结果字典的“count”字段填写 `count,`，这样导出的 JSON/表格能保留这一项证据。
            "count": count,
            # 给当前结果字典的“entries”字段填写 `entries,`，这样导出的 JSON/表格能保留这一项证据。
            "entries": entries,
            # 给当前结果字典的“raw_sha256”字段填写 `_sha(raw),`，这样导出的 JSON/表格能保留这一项证据。
            "raw_sha256": _sha(raw),
        })
    # 把右侧 `core[base:base + 库存银行大小]` 计算得到的值保存到 `bank_raw`，后面的判断或输出会继续使用它。
    bank_raw = core[base:base + 库存银行大小]
    # 把 `({` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return ({
        # 给当前结果字典的“bank_index”字段填写 `bank_index,`，这样导出的 JSON/表格能保留这一项证据。
        "bank_index": bank_index,
        # 给当前结果字典的“categories”字段填写 `categories,`，这样导出的 JSON/表格能保留这一项证据。
        "categories": categories,
        # 给当前结果字典的“money”字段填写 `_u32(core, base + 库存金钱偏移),`，这样导出的 JSON/表格能保留这一项证据。
        "money": _u32(core, base + 库存金钱偏移),
        # +5DDC/+5DE0/+5DE4 是运行时选择缓存/指针语义，不进入 canonical。
        # 给当前结果字典的“raw_sha256”字段填写 `_sha(bank_raw),`，这样导出的 JSON/表格能保留这一项证据。
        "raw_sha256": _sha(bank_raw),
    # 执行这一条实际代码步骤：`}, warnings)`；它与上下相邻语句共同完成当前函数的小任务。
    }, warnings)


# 定义函数 `_role_name`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _role_name(record: bytes) -> str:
    # API/Role record 的角色名位于 +0x0C 起的固定区域；取到 +0x20 前。
    # 把 `_decode_legacy_text(record[0x0C:0x20])` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return _decode_legacy_text(record[0x0C:0x20])


# 定义函数 `_parse_roles`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _parse_roles(core: bytes) -> list[dict[str, Any]]:
    # 把右侧 `[]` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out = []
    # 开始循环 `i in range(角色保存记录数)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(角色保存记录数):
        # 把右侧 `偏移_角色记录 + i * 角色记录大小` 计算得到的值保存到 `off`，后面的判断或输出会继续使用它。
        off = 偏移_角色记录 + i * 角色记录大小
        # 把右侧 `core[off:off + 角色记录大小]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec = core[off:off + 角色记录大小]
        # 把这一条新结果追加到列表 `out`；不会覆盖前面已经收集的记录。
        out.append({
            # 给当前结果字典的“slot”字段填写 `i,`，这样导出的 JSON/表格能保留这一项证据。
            "slot": i,
            # 给当前结果字典的“load_authoritative”字段填写 `i < 角色权威加载记录数,`，这样导出的 JSON/表格能保留这一项证据。
            "load_authoritative": i < 角色权威加载记录数,
            # 给当前结果字典的“legacy_auxiliary”字段填写 `i >= 角色权威加载记录数,`，这样导出的 JSON/表格能保留这一项证据。
            "legacy_auxiliary": i >= 角色权威加载记录数,
            # 给当前结果字典的“record_id”字段填写 `_u32(rec, 0x00),`，这样导出的 JSON/表格能保留这一项证据。
            "record_id": _u32(rec, 0x00),
            # 给当前结果字典的“name”字段填写 `_role_name(rec),`，这样导出的 JSON/表格能保留这一项证据。
            "name": _role_name(rec),
            # 给当前结果字典的“field_0x20”字段填写 `_u32(rec, 0x20),`，这样导出的 JSON/表格能保留这一项证据。
            "field_0x20": _u32(rec, 0x20),
            # 给当前结果字典的“raw_sha256”字段填写 `_sha(rec),`，这样导出的 JSON/表格能保留这一项证据。
            "raw_sha256": _sha(rec),
        })
    # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return out


# 定义函数 `_parse_party`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _parse_party(core: bytes) -> tuple[dict[str, Any], list[str]]:
    # 创建变量 `warnings`（类型提示为 `list[str]`），并把 `[]` 的结果保存进去供后续步骤使用。
    warnings: list[str] = []
    # 把右侧 `_u32(core, 偏移_队伍数量)` 计算得到的值保存到 `count`，后面的判断或输出会继续使用它。
    count = _u32(core, 偏移_队伍数量)
    # 检查条件 `count > 8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if count > 8:
        # 执行一次赋值：把 `{count} 超过正常可加载角色ID范围1..8")` 的结果放入左侧 `warnings.append(f"party_count` 对应的变量/字段。
        warnings.append(f"party_count={count} 超过正常可加载角色ID范围1..8")
    # 把右侧 `[]` 计算得到的值保存到 `load_slots`，后面的判断或输出会继续使用它。
    load_slots = []
    # 开始循环 `i in range(8)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(8):
        # 把右侧 `_u32(core, 偏移_队伍角色ID + i * 4)` 计算得到的值保存到 `rid`，后面的判断或输出会继续使用它。
        rid = _u32(core, 偏移_队伍角色ID + i * 4)
        # 把右侧 `_u32(core, 偏移_队伍编成槽 + i * 4)` 计算得到的值保存到 `formation`，后面的判断或输出会继续使用它。
        formation = _u32(core, 偏移_队伍编成槽 + i * 4)
        # 把这一条新结果追加到列表 `load_slots`；不会覆盖前面已经收集的记录。
        load_slots.append({"slot": i, "role_id": rid, "formation_slot": formation})
    # 把右侧 `[]` 计算得到的值保存到 `canonical`，后面的判断或输出会继续使用它。
    canonical = []
    # 开始循环 `e in load_slots[:min(count, 8)]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for e in load_slots[:min(count, 8)]:
        # 检查条件 `not (1 <= e["role_id"] <= 8)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not (1 <= e["role_id"] <= 8):
            # 执行一次赋值：把 `{e['role_id']} 不在1..8")` 的结果放入左侧 `warnings.append(f"活动队伍槽{e['slot']} role_id` 对应的变量/字段。
            warnings.append(f"活动队伍槽{e['slot']} role_id={e['role_id']} 不在1..8")
        # 把这一条新结果追加到列表 `canonical`；不会覆盖前面已经收集的记录。
        canonical.append({"role_id": e["role_id"], "formation_slot": e["formation_slot"]})

    # Save 端会写第9项，但 Load 端只恢复8项；它只能作为 legacy aux 保存。
    # 把右侧 `{` 计算得到的值保存到 `aux9`，后面的判断或输出会继续使用它。
    aux9 = {
        # 给当前结果字典的“slot”字段填写 `8,`，这样导出的 JSON/表格能保留这一项证据。
        "slot": 8,
        # 给当前结果字典的“role_id”字段填写 `_u32(core, 偏移_队伍角色ID + 8 * 4),`，这样导出的 JSON/表格能保留这一项证据。
        "role_id": _u32(core, 偏移_队伍角色ID + 8 * 4),
        # 给当前结果字典的“formation_slot”字段填写 `_u32(core, 偏移_队伍编成槽 + 8 * 4),`，这样导出的 JSON/表格能保留这一项证据。
        "formation_slot": _u32(core, 偏移_队伍编成槽 + 8 * 4),
        # 给当前结果字典的“load_authoritative”字段填写 `False,`，这样导出的 JSON/表格能保留这一项证据。
        "load_authoritative": False,
    }
    # 把 `({` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return ({
        # 给当前结果字典的“count”字段填写 `count,`，这样导出的 JSON/表格能保留这一项证据。
        "count": count,
        # 给当前结果字典的“entries”字段填写 `canonical,`，这样导出的 JSON/表格能保留这一项证据。
        "entries": canonical,
        # 给当前结果字典的“legacy_load_slots_0_to_7”字段填写 `load_slots,`，这样导出的 JSON/表格能保留这一项证据。
        "legacy_load_slots_0_to_7": load_slots,
        # 给当前结果字典的“legacy_save_only_slot_8”字段填写 `aux9,`，这样导出的 JSON/表格能保留这一项证据。
        "legacy_save_only_slot_8": aux9,
    # 执行这一条实际代码步骤：`}, warnings)`；它与上下相邻语句共同完成当前函数的小任务。
    }, warnings)


# 定义函数 `_parse_world`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _parse_world(world: bytes) -> tuple[dict[str, Any], list[str]]:
    # 创建变量 `warnings`（类型提示为 `list[str]`），并把 `[]` 的结果保存进去供后续步骤使用。
    warnings: list[str] = []
    # 检查条件 `len(world) < 世界块固定大小`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(world) < 世界块固定大小:
        # 执行一次赋值：把 `{len(world)}，小于已确认标准0x27D8")` 的结果放入左侧 `warnings.append(f"World/GameVariable block` 对应的变量/字段。
        warnings.append(f"World/GameVariable block={len(world)}，小于已确认标准0x27D8")
        # 把 `({"raw_size": len(world), "raw_sha256": _sha(world)}, warnings)` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return ({"raw_size": len(world), "raw_sha256": _sha(world)}, warnings)

    # 把右侧 `world[:世界_场景描述长度]` 计算得到的值保存到 `descriptor`，后面的判断或输出会继续使用它。
    descriptor = world[:世界_场景描述长度]
    # 把右侧 `[_i16(world, 世界_变量偏移 + i * 2) for i in range(游戏变量数量)]` 计算得到的值保存到 `game_vars`，后面的判断或输出会继续使用它。
    game_vars = [_i16(world, 世界_变量偏移 + i * 2) for i in range(游戏变量数量)]
    # 把 `({` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return ({
        # 给当前结果字典的“raw_size”字段填写 `len(world),`，这样导出的 JSON/表格能保留这一项证据。
        "raw_size": len(world),
        # 给当前结果字典的“raw_sha256”字段填写 `_sha(world),`，这样导出的 JSON/表格能保留这一项证据。
        "raw_sha256": _sha(world),
        # 给当前结果字典的“legacy_scene_load_descriptor”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "legacy_scene_load_descriptor": {
            # 给当前结果字典的“raw_hex”字段填写 `_hex(descriptor),`，这样导出的 JSON/表格能保留这一项证据。
            "raw_hex": _hex(descriptor),
            # 给当前结果字典的“decoded_first_string”字段填写 `_decode_legacy_text(descriptor),`，这样导出的 JSON/表格能保留这一项证据。
            "decoded_first_string": _decode_legacy_text(descriptor),
        },
        # 原代码 0x44B150 对这些偏移执行 DWORD 读写；偏移未按4字节对齐。
        # 给当前结果字典的“controlled_actor”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "controlled_actor": {
            # 给当前结果字典的“x”字段填写 `_i32(world, 世界_角色X偏移),`，这样导出的 JSON/表格能保留这一项证据。
            "x": _i32(world, 世界_角色X偏移),
            # 给当前结果字典的“y”字段填写 `_i32(world, 世界_角色Y偏移),`，这样导出的 JSON/表格能保留这一项证据。
            "y": _i32(world, 世界_角色Y偏移),
            # 给当前结果字典的“orientation_or_state”字段填写 `_u32(world, 世界_角色状态偏移),`，这样导出的 JSON/表格能保留这一项证据。
            "orientation_or_state": _u32(world, 世界_角色状态偏移),
            # 给当前结果字典的“controlled_actor_index”字段填写 `_u32(world, 世界_当前受控索引偏移),`，这样导出的 JSON/表格能保留这一项证据。
            "controlled_actor_index": _u32(world, 世界_当前受控索引偏移),
        },
        # 给当前结果字典的“game_variables”字段填写 `game_vars,`，这样导出的 JSON/表格能保留这一项证据。
        "game_variables": game_vars,
    # 执行这一条实际代码步骤：`}, warnings)`；它与上下相邻语句共同完成当前函数的小任务。
    }, warnings)


# 定义函数 `_parse_metadata`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _parse_metadata(core: bytes) -> dict[str, Any]:
    # +0x14 开始是明确元数据；描述串若超过20字节会被后续字段覆盖，故保留 raw20。
    # 把右侧 `core[:0x14]` 计算得到的值保存到 `raw20`，后面的判断或输出会继续使用它。
    raw20 = core[:0x14]
    # 把右侧 `_decode_legacy_text(raw20)` 计算得到的值保存到 `description`，后面的判断或输出会继续使用它。
    description = _decode_legacy_text(raw20)
    # 把右侧 `_u32(core, 0x18)` 计算得到的值保存到 `day`，后面的判断或输出会继续使用它。
    day = _u32(core, 0x18)
    # 把右侧 `_u32(core, 0x1C)` 计算得到的值保存到 `month`，后面的判断或输出会继续使用它。
    month = _u32(core, 0x1C)
    # 把右侧 `_u32(core, 0x20)` 计算得到的值保存到 `year2`，后面的判断或输出会继续使用它。
    year2 = _u32(core, 0x20)
    # 把右侧 `_u32(core, 0x24)` 计算得到的值保存到 `hour`，后面的判断或输出会继续使用它。
    hour = _u32(core, 0x24)
    # 把右侧 `_u32(core, 0x28)` 计算得到的值保存到 `minute`，后面的判断或输出会继续使用它。
    minute = _u32(core, 0x28)
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“description”字段填写 `description,`，这样导出的 JSON/表格能保留这一项证据。
        "description": description,
        # 给当前结果字典的“description_raw20_hex”字段填写 `raw20.hex(),`，这样导出的 JSON/表格能保留这一项证据。
        "description_raw20_hex": raw20.hex(),
        # serializer: manager+A4(API) +0x370 = role #1 +0x20
        # 给当前结果字典的“protagonist_role1_field_0x20_preview”字段填写 `_u32(core, 0x14),`，这样导出的 JSON/表格能保留这一项证据。
        "protagonist_role1_field_0x20_preview": _u32(core, 0x14),
        # 给当前结果字典的“legacy_timestamp”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "legacy_timestamp": {
            # 给当前结果字典的“day”字段填写 `day,`，这样导出的 JSON/表格能保留这一项证据。
            "day": day,
            # 给当前结果字典的“month”字段填写 `month,`，这样导出的 JSON/表格能保留这一项证据。
            "month": month,
            # 给当前结果字典的“year_2digit”字段填写 `year2,`，这样导出的 JSON/表格能保留这一项证据。
            "year_2digit": year2,
            # 给当前结果字典的“hour_after_legacy_plus8_wrap”字段填写 `hour,`，这样导出的 JSON/表格能保留这一项证据。
            "hour_after_legacy_plus8_wrap": hour,
            # 给当前结果字典的“minute”字段填写 `minute,`，这样导出的 JSON/表格能保留这一项证据。
            "minute": minute,
        },
    }


# 定义函数 `parse_tsf`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_tsf(path: Path) -> dict[str, Any]:
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
    raw = path.read_bytes()
    # 创建变量 `warnings`（类型提示为 `list[str]`），并把 `[]` 的结果保存进去供后续步骤使用。
    warnings: list[str] = []
    # 检查条件 `len(raw) < 核心大小`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(raw) < 核心大小:
        # 检测到不能继续的情况，主动抛出异常 `ValueError(f"文件小于固定Core大小：{len(raw)} < {核心大小}")`，让上层明确知道数据或参数不符合要求。
        raise ValueError(f"文件小于固定Core大小：{len(raw)} < {核心大小}")

    # 把右侧 `raw[:核心大小]` 计算得到的值保存到 `core_raw`，后面的判断或输出会继续使用它。
    core_raw = raw[:核心大小]
    # 完全复刻原 Loader 判定：仅当 RAW Core +DD99 DWORD 恰好为 0xFF 才解码。
    # 把右侧 `_u32(core_raw, 偏移_旧核心编码标记) == 0xFF` 计算得到的值保存到 `legacy_encoded`，后面的判断或输出会继续使用它。
    legacy_encoded = _u32(core_raw, 偏移_旧核心编码标记) == 0xFF
    # 把右侧 `bytearray(core_raw)` 计算得到的值保存到 `core`，后面的判断或输出会继续使用它。
    core = bytearray(core_raw)
    # 检查条件 `legacy_encoded`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if legacy_encoded:
        # 调用 `_xor255_inplace` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        _xor255_inplace(core)
    # 把右侧 `bytes(core)` 计算得到的值保存到 `core_b`，后面的判断或输出会继续使用它。
    core_b = bytes(core)

    # 把右侧 `_u32(core_b, 偏移_商店块长度)` 计算得到的值保存到 `shop_len`，后面的判断或输出会继续使用它。
    shop_len = _u32(core_b, 偏移_商店块长度)
    # 把右侧 `_u32(core_b, 偏移_世界块长度)` 计算得到的值保存到 `world_len`，后面的判断或输出会继续使用它。
    world_len = _u32(core_b, 偏移_世界块长度)
    # 把右侧 `核心大小 + world_len + shop_len` 计算得到的值保存到 `expected`，后面的判断或输出会继续使用它。
    expected = 核心大小 + world_len + shop_len
    # 检查条件 `expected != len(raw)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if expected != len(raw):
        # 执行一次赋值：把 `{expected}，实际={len(raw)}")` 的结果放入左侧 `warnings.append(f"容器长度不一致：Core({核心大小})+World({world_le…` 对应的变量/字段。
        warnings.append(f"容器长度不一致：Core({核心大小})+World({world_len})+Shop({shop_len})={expected}，实际={len(raw)}")

    # 把右侧 `核心大小` 计算得到的值保存到 `world_start`，后面的判断或输出会继续使用它。
    world_start = 核心大小
    # 把右侧 `min(len(raw), world_start + world_len)` 计算得到的值保存到 `world_end`，后面的判断或输出会继续使用它。
    world_end = min(len(raw), world_start + world_len)
    # 把右侧 `raw[world_start:world_end]` 计算得到的值保存到 `world`，后面的判断或输出会继续使用它。
    world = raw[world_start:world_end]
    # 把右侧 `world_start + world_len` 计算得到的值保存到 `shop_start`，后面的判断或输出会继续使用它。
    shop_start = world_start + world_len
    # 把右侧 `min(len(raw), shop_start + shop_len)` 计算得到的值保存到 `shop_end`，后面的判断或输出会继续使用它。
    shop_end = min(len(raw), shop_start + shop_len)
    # 把右侧 `raw[shop_start:shop_end]` 计算得到的值保存到 `shop`，后面的判断或输出会继续使用它。
    shop = raw[shop_start:shop_end]

    # 执行一次赋值：把 `_parse_inventory_bank(core_b, 偏移_库存银行0, 0); warnings += w` 的结果放入左侧 `bank0, w` 对应的变量/字段。
    bank0, w = _parse_inventory_bank(core_b, 偏移_库存银行0, 0); warnings += w
    # 执行一次赋值：把 `_parse_inventory_bank(core_b, 偏移_库存银行1, 1); warnings += w` 的结果放入左侧 `bank1, w` 对应的变量/字段。
    bank1, w = _parse_inventory_bank(core_b, 偏移_库存银行1, 1); warnings += w
    # 执行一次赋值：把 `_parse_party(core_b); warnings += w` 的结果放入左侧 `party, w` 对应的变量/字段。
    party, w = _parse_party(core_b); warnings += w
    # 执行一次赋值：把 `_parse_world(world); warnings += w` 的结果放入左侧 `world_state, w` 对应的变量/字段。
    world_state, w = _parse_world(world); warnings += w
    # 把右侧 `_parse_roles(core_b)` 计算得到的值保存到 `roles`，后面的判断或输出会继续使用它。
    roles = _parse_roles(core_b)

    # 把右侧 `_u32(core_b, 偏移_当前库存银行)` 计算得到的值保存到 `active_bank`，后面的判断或输出会继续使用它。
    active_bank = _u32(core_b, 偏移_当前库存银行)
    # 检查条件 `active_bank not in (0, 1)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if active_bank not in (0, 1):
        # 执行一次赋值：把 `{active_bank}，原版选择器仅确认0/1")` 的结果放入左侧 `warnings.append(f"active_inventory_bank` 对应的变量/字段。
        warnings.append(f"active_inventory_bank={active_bank}，原版选择器仅确认0/1")

    # 把右侧 `_u32(core_b, 偏移_MP3音量)` 计算得到的值保存到 `mp3_level`，后面的判断或输出会继续使用它。
    mp3_level = _u32(core_b, 偏移_MP3音量)
    # 把右侧 `_u32(core_b, 偏移_WAV音量)` 计算得到的值保存到 `wav_level`，后面的判断或输出会继续使用它。
    wav_level = _u32(core_b, 偏移_WAV音量)
    # 检查条件 `not (0 <= mp3_level <= 9 and 0 <= wav_level <= 9)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not (0 <= mp3_level <= 9 and 0 <= wav_level <= 9):
        # 执行一次赋值：把 `{mp3_level}, WAV={wav_level}")` 的结果放入左侧 `warnings.append(f"Legacy音量超出0..9：MP3` 对应的变量/字段。
        warnings.append(f"Legacy音量超出0..9：MP3={mp3_level}, WAV={wav_level}")

    # 检查条件 `shop_len and shop_len % 商店记录大小 != 0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if shop_len and shop_len % 商店记录大小 != 0:
        # 把这一条新结果追加到列表 `warnings`；不会覆盖前面已经收集的记录。
        warnings.append(f"Shop block长度{shop_len}不能被0x288整除")

    # +D9D0..DD1F 等区域已证明含未初始化/指针垃圾，只作哈希诊断，不 canonicalize。
    # 把右侧 `core_b[0xD9D0:0xDD20]` 计算得到的值保存到 `noisy_gap`，后面的判断或输出会继续使用它。
    noisy_gap = core_b[0xD9D0:0xDD20]

    # 把右侧 `_parse_metadata(core_b)` 计算得到的值保存到 `metadata`，后面的判断或输出会继续使用它。
    metadata = _parse_metadata(core_b)
    # 验证 +0x14 与 role1 +0x20；这是原 Serializer 直接复制关系。
    # 把右侧 `roles[1]["field_0x20"] if len(roles) > 1 else None` 计算得到的值保存到 `role1_f20`，后面的判断或输出会继续使用它。
    role1_f20 = roles[1]["field_0x20"] if len(roles) > 1 else None
    # 检查条件 `metadata["protagonist_role1_field_0x20_preview"] != role1_f20`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if metadata["protagonist_role1_field_0x20_preview"] != role1_f20:
        # 把这一条新结果追加到列表 `warnings`；不会覆盖前面已经收集的记录。
        warnings.append("Core+0x14 与 Role#1+0x20 不一致；当前样本应一致")

    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {
        # 给当前结果字典的“tool”字段填写 `{"name": "幽城TSF与GameState调查器", "version": 版本},`，这样导出的 JSON/表格能保留这一项证据。
        "tool": {"name": "幽城TSF与GameState调查器", "version": 版本},
        # 给当前结果字典的“source”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "source": {
            # 给当前结果字典的“path”字段填写 `str(path),`，这样导出的 JSON/表格能保留这一项证据。
            "path": str(path),
            # 给当前结果字典的“size”字段填写 `len(raw),`，这样导出的 JSON/表格能保留这一项证据。
            "size": len(raw),
            # 给当前结果字典的“sha256”字段填写 `_sha(raw),`，这样导出的 JSON/表格能保留这一项证据。
            "sha256": _sha(raw),
            # 给当前结果字典的“legacy_core_xor255_decoded”字段填写 `legacy_encoded,`，这样导出的 JSON/表格能保留这一项证据。
            "legacy_core_xor255_decoded": legacy_encoded,
        },
        # 给当前结果字典的“container”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "container": {
            # 给当前结果字典的“fixed_core_size”字段填写 `核心大小,`，这样导出的 JSON/表格能保留这一项证据。
            "fixed_core_size": 核心大小,
            # 给当前结果字典的“world_block_length”字段填写 `world_len,`，这样导出的 JSON/表格能保留这一项证据。
            "world_block_length": world_len,
            # 给当前结果字典的“shop_block_length”字段填写 `shop_len,`，这样导出的 JSON/表格能保留这一项证据。
            "shop_block_length": shop_len,
            # 给当前结果字典的“expected_total_size”字段填写 `expected,`，这样导出的 JSON/表格能保留这一项证据。
            "expected_total_size": expected,
            # 给当前结果字典的“length_matches”字段填写 `expected == len(raw),`，这样导出的 JSON/表格能保留这一项证据。
            "length_matches": expected == len(raw),
        },
        # 给当前结果字典的“legacy_save_metadata”字段填写 `metadata,`，这样导出的 JSON/表格能保留这一项证据。
        "legacy_save_metadata": metadata,
        # 给当前结果字典的“canonical_game_state”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "canonical_game_state": {
            # 给当前结果字典的“world”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            "world": {
                # 给当前结果字典的“scene_load_descriptor”字段填写 `world_state.get("legacy_scene_load_descriptor"),`，这样导出的 JSON/表格能保留这一项证据。
                "scene_load_descriptor": world_state.get("legacy_scene_load_descriptor"),
                # 给当前结果字典的“controlled_actor”字段填写 `world_state.get("controlled_actor"),`，这样导出的 JSON/表格能保留这一项证据。
                "controlled_actor": world_state.get("controlled_actor"),
                # 给当前结果字典的“game_variables”字段填写 `world_state.get("game_variables", []),`，这样导出的 JSON/表格能保留这一项证据。
                "game_variables": world_state.get("game_variables", []),
            },
            # 给当前结果字典的“party”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            "party": {
                # 给当前结果字典的“count”字段填写 `party["count"],`，这样导出的 JSON/表格能保留这一项证据。
                "count": party["count"],
                # 给当前结果字典的“entries”字段填写 `party["entries"],`，这样导出的 JSON/表格能保留这一项证据。
                "entries": party["entries"],
            },
            # 给当前结果字典的“roles”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
            "roles": [
                # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
                {k: v for k, v in r.items() if k not in ("legacy_auxiliary",)}
                # 开始循环 `r in roles[`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                for r in roles[:角色权威加载记录数]
            ],
            # 给当前结果字典的“inventory”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            "inventory": {
                # 给当前结果字典的“active_bank”字段填写 `active_bank,`，这样导出的 JSON/表格能保留这一项证据。
                "active_bank": active_bank,
                # 给当前结果字典的“banks”字段填写 `[bank0, bank1],`，这样导出的 JSON/表格能保留这一项证据。
                "banks": [bank0, bank1],
            },
            # 给当前结果字典的“shop”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            "shop": {
                # 给当前结果字典的“present”字段填写 `bool(shop_len),`，这样导出的 JSON/表格能保留这一项证据。
                "present": bool(shop_len),
                # 给当前结果字典的“length”字段填写 `shop_len,`，这样导出的 JSON/表格能保留这一项证据。
                "length": shop_len,
                # 给当前结果字典的“record_size”字段填写 `商店记录大小,`，这样导出的 JSON/表格能保留这一项证据。
                "record_size": 商店记录大小,
                # 给当前结果字典的“record_count”字段填写 `shop_len // 商店记录大小 if shop_len % 商店记录大小 == 0 else None,`，这样导出的 JSON/表格能保留这一项证据。
                "record_count": shop_len // 商店记录大小 if shop_len % 商店记录大小 == 0 else None,
                # 给当前结果字典的“raw_sha256”字段填写 `_sha(shop) if shop else None,`，这样导出的 JSON/表格能保留这一项证据。
                "raw_sha256": _sha(shop) if shop else None,
            },
        },
        # 给当前结果字典的“legacy_compatibility”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "legacy_compatibility": {
            # 给当前结果字典的“audio_preferences”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            "audio_preferences": {
                # 给当前结果字典的“mp3_music_volume_level_0_to_9”字段填写 `mp3_level,`，这样导出的 JSON/表格能保留这一项证据。
                "mp3_music_volume_level_0_to_9": mp3_level,
                # 给当前结果字典的“wav_sfx_volume_level_0_to_9”字段填写 `wav_level,`，这样导出的 JSON/表格能保留这一项证据。
                "wav_sfx_volume_level_0_to_9": wav_level,
            },
            # 给当前结果字典的“role_record_8_save_only_non_authoritative”字段填写 `roles[8],`，这样导出的 JSON/表格能保留这一项证据。
            "role_record_8_save_only_non_authoritative": roles[8],
            # 给当前结果字典的“party_mapping_save_only_slot_8”字段填写 `party["legacy_save_only_slot_8"],`，这样导出的 JSON/表格能保留这一项证据。
            "party_mapping_save_only_slot_8": party["legacy_save_only_slot_8"],
            # 给当前结果字典的“core_noisy_uninitialized_gap_D9D0_DD1F”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            "core_noisy_uninitialized_gap_D9D0_DD1F": {
                # 给当前结果字典的“canonicalize”字段填写 `False,`，这样导出的 JSON/表格能保留这一项证据。
                "canonicalize": False,
                # 给当前结果字典的“size”字段填写 `len(noisy_gap),`，这样导出的 JSON/表格能保留这一项证据。
                "size": len(noisy_gap),
                # 给当前结果字典的“sha256”字段填写 `_sha(noisy_gap),`，这样导出的 JSON/表格能保留这一项证据。
                "sha256": _sha(noisy_gap),
            },
        },
        # 给当前结果字典的“warnings”字段填写 `warnings,`，这样导出的 JSON/表格能保留这一项证据。
        "warnings": warnings,
    }
    # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return result


# 定义函数 `_semantic_digest`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _semantic_digest(parsed: dict[str, Any]) -> dict[str, Any]:
    # 把右侧 `parsed["canonical_game_state"]` 计算得到的值保存到 `gs`，后面的判断或输出会继续使用它。
    gs = parsed["canonical_game_state"]
    # 把右侧 `gs["world"]` 计算得到的值保存到 `world`，后面的判断或输出会继续使用它。
    world = gs["world"]
    # 把右侧 `gs["inventory"]` 计算得到的值保存到 `inv`，后面的判断或输出会继续使用它。
    inv = gs["inventory"]
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“size”字段填写 `parsed["source"]["size"],`，这样导出的 JSON/表格能保留这一项证据。
        "size": parsed["source"]["size"],
        # 给当前结果字典的“container”字段填写 `parsed["container"],`，这样导出的 JSON/表格能保留这一项证据。
        "container": parsed["container"],
        # 给当前结果字典的“description”字段填写 `parsed["legacy_save_metadata"]["description"],`，这样导出的 JSON/表格能保留这一项证据。
        "description": parsed["legacy_save_metadata"]["description"],
        # 给当前结果字典的“preview_level”字段填写 `parsed["legacy_save_metadata"]["protagonist_role1_field_0x20_preview"…`，这样导出的 JSON/表格能保留这一项证据。
        "preview_level": parsed["legacy_save_metadata"]["protagonist_role1_field_0x20_preview"],
        # 给当前结果字典的“scene”字段填写 `world.get("scene_load_descriptor", {}).get("decoded_first_string") if…`，这样导出的 JSON/表格能保留这一项证据。
        "scene": world.get("scene_load_descriptor", {}).get("decoded_first_string") if world.get("scene_load_descriptor") else None,
        # 给当前结果字典的“controlled_actor”字段填写 `world.get("controlled_actor"),`，这样导出的 JSON/表格能保留这一项证据。
        "controlled_actor": world.get("controlled_actor"),
        # 给当前结果字典的“party”字段填写 `gs["party"],`，这样导出的 JSON/表格能保留这一项证据。
        "party": gs["party"],
        # 给当前结果字典的“active_bank”字段填写 `inv["active_bank"],`，这样导出的 JSON/表格能保留这一项证据。
        "active_bank": inv["active_bank"],
        # 给当前结果字典的“money”字段填写 `[b["money"] for b in inv["banks"]],`，这样导出的 JSON/表格能保留这一项证据。
        "money": [b["money"] for b in inv["banks"]],
        # 给当前结果字典的“inventory_bank_hashes”字段填写 `[b["raw_sha256"] for b in inv["banks"]],`，这样导出的 JSON/表格能保留这一项证据。
        "inventory_bank_hashes": [b["raw_sha256"] for b in inv["banks"]],
        # 给当前结果字典的“role_hashes”字段填写 `[r["raw_sha256"] for r in gs["roles"]],`，这样导出的 JSON/表格能保留这一项证据。
        "role_hashes": [r["raw_sha256"] for r in gs["roles"]],
        # 给当前结果字典的“role8_aux_hash”字段填写 `parsed["legacy_compatibility"]["role_record_8_save_only_non_authorita…`，这样导出的 JSON/表格能保留这一项证据。
        "role8_aux_hash": parsed["legacy_compatibility"]["role_record_8_save_only_non_authoritative"]["raw_sha256"],
        # 给当前结果字典的“audio”字段填写 `parsed["legacy_compatibility"]["audio_preferences"],`，这样导出的 JSON/表格能保留这一项证据。
        "audio": parsed["legacy_compatibility"]["audio_preferences"],
        # 给当前结果字典的“shop”字段填写 `gs["shop"],`，这样导出的 JSON/表格能保留这一项证据。
        "shop": gs["shop"],
        # 给当前结果字典的“warnings”字段填写 `parsed["warnings"],`，这样导出的 JSON/表格能保留这一项证据。
        "warnings": parsed["warnings"],
    }


# 定义函数 `cmd_tsf`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_tsf(args: argparse.Namespace) -> None:
    # 把右侧 `parse_tsf(Path(args.file))` 计算得到的值保存到 `parsed`，后面的判断或输出会继续使用它。
    parsed = parse_tsf(Path(args.file))
    # 检查条件 `not args.包含全部变量`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not args.包含全部变量:
        # 把右侧 `{` 计算得到的值保存到 `parsed["canonical_game_state"]["world"]["game_variable…`，后面的判断或输出会继续使用它。
        parsed["canonical_game_state"]["world"]["game_variables"] = {
            # 给当前结果字典的“count”字段填写 `游戏变量数量,`，这样导出的 JSON/表格能保留这一项证据。
            "count": 游戏变量数量,
            # 给当前结果字典的“说明”字段填写 `"使用 --包含全部变量 输出5000项；批量/差分命令可直接分析。",`，这样导出的 JSON/表格能保留这一项证据。
            "说明": "使用 --包含全部变量 输出5000项；批量/差分命令可直接分析。",
        }
    # 调用 `_json_dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    _json_dump(parsed, Path(args.输出) if args.输出 else None)


# 定义函数 `cmd_diff`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_diff(args: argparse.Namespace) -> None:
    # 把右侧 `parse_tsf(Path(args.file_a)); b = parse_tsf(Path(args.file_b))` 计算得到的值保存到 `a`，后面的判断或输出会继续使用它。
    a = parse_tsf(Path(args.file_a)); b = parse_tsf(Path(args.file_b))
    # 把右侧 `a["canonical_game_state"]; gb = b["canonical_game_state"]` 计算得到的值保存到 `ga`，后面的判断或输出会继续使用它。
    ga = a["canonical_game_state"]; gb = b["canonical_game_state"]
    # 把右侧 `ga["world"]["game_variables"]; vb = gb["world"]["game_variables"]` 计算得到的值保存到 `va`，后面的判断或输出会继续使用它。
    va = ga["world"]["game_variables"]; vb = gb["world"]["game_variables"]
    # 把右侧 `[` 计算得到的值保存到 `var_diffs`，后面的判断或输出会继续使用它。
    var_diffs = [
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        {"index": i, "before": x, "after": y}
        # 开始循环 `i, (x, y) in enumerate(zip(va, vb)) if x != y`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i, (x, y) in enumerate(zip(va, vb)) if x != y
    ]
    # 把右侧 `[]` 计算得到的值保存到 `role_diffs`，后面的判断或输出会继续使用它。
    role_diffs = []
    # 开始循环 `i, (ra, rb) in enumerate(zip(ga["roles"], gb["roles"]))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i, (ra, rb) in enumerate(zip(ga["roles"], gb["roles"])):
        # 检查条件 `ra["raw_sha256"] != rb["raw_sha256"]`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ra["raw_sha256"] != rb["raw_sha256"]:
            # 把这一条新结果追加到列表 `role_diffs`；不会覆盖前面已经收集的记录。
            role_diffs.append({"role_slot": i, "before": ra["raw_sha256"], "after": rb["raw_sha256"]})
    # 把右侧 `{` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out = {
        # 给当前结果字典的“before”字段填写 `_semantic_digest(a),`，这样导出的 JSON/表格能保留这一项证据。
        "before": _semantic_digest(a),
        # 给当前结果字典的“after”字段填写 `_semantic_digest(b),`，这样导出的 JSON/表格能保留这一项证据。
        "after": _semantic_digest(b),
        # 给当前结果字典的“game_variable_changes”字段填写 `var_diffs,`，这样导出的 JSON/表格能保留这一项证据。
        "game_variable_changes": var_diffs,
        # 给当前结果字典的“game_variable_change_count”字段填写 `len(var_diffs),`，这样导出的 JSON/表格能保留这一项证据。
        "game_variable_change_count": len(var_diffs),
        # 给当前结果字典的“role_record_changes”字段填写 `role_diffs,`，这样导出的 JSON/表格能保留这一项证据。
        "role_record_changes": role_diffs,
        # 给当前结果字典的“shop_changed”字段填写 `ga["shop"]["raw_sha256"] != gb["shop"]["raw_sha256"],`，这样导出的 JSON/表格能保留这一项证据。
        "shop_changed": ga["shop"]["raw_sha256"] != gb["shop"]["raw_sha256"],
        # 给当前结果字典的“party_changed”字段填写 `ga["party"] != gb["party"],`，这样导出的 JSON/表格能保留这一项证据。
        "party_changed": ga["party"] != gb["party"],
        # 给当前结果字典的“active_inventory_bank_changed”字段填写 `ga["inventory"]["active_bank"] != gb["inventory"]["active_bank"],`，这样导出的 JSON/表格能保留这一项证据。
        "active_inventory_bank_changed": ga["inventory"]["active_bank"] != gb["inventory"]["active_bank"],
    }
    # 调用 `_json_dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    _json_dump(out, Path(args.输出) if args.输出 else None)


# 定义函数 `cmd_batch`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_batch(args: argparse.Namespace) -> None:
    # 把右侧 `Path(args.dir)` 计算得到的值保存到 `root`，后面的判断或输出会继续使用它。
    root = Path(args.dir)
    # 把右侧 `_list_saves(root)` 计算得到的值保存到 `saves`，后面的判断或输出会继续使用它。
    saves = _list_saves(root)
    # 检查条件 `not saves`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not saves:
        # 检测到不能继续的情况，主动抛出异常 `SystemExit("未找到 *.TSF")`，让上层明确知道数据或参数不符合要求。
        raise SystemExit("未找到 *.TSF")
    # 把右侧 `_extract_walkthrough_labels(Path(args.说明) if args.说明 else None)` 计算得到的值保存到 `labels`，后面的判断或输出会继续使用它。
    labels = _extract_walkthrough_labels(Path(args.说明) if args.说明 else None)
    # 把右侧 `[parse_tsf(p) for p in saves]` 计算得到的值保存到 `parsed`，后面的判断或输出会继续使用它。
    parsed = [parse_tsf(p) for p in saves]

    # 把右侧 `Counter(x["source"]["size"] for x in parsed)` 计算得到的值保存到 `size_counts`，后面的判断或输出会继续使用它。
    size_counts = Counter(x["source"]["size"] for x in parsed)
    # 把右侧 `Counter((x["container"]["world_block_length"], x["container"]["shop_block_…` 计算得到的值保存到 `block_pairs`，后面的判断或输出会继续使用它。
    block_pairs = Counter((x["container"]["world_block_length"], x["container"]["shop_block_length"]) for x in parsed)
    # 把右侧 `Counter(x["canonical_game_state"]["inventory"]["active_bank"] for x in par…` 计算得到的值保存到 `active_counts`，后面的判断或输出会继续使用它。
    active_counts = Counter(x["canonical_game_state"]["inventory"]["active_bank"] for x in parsed)
    # 把右侧 `Counter(x["canonical_game_state"]["party"]["count"] for x in parsed)` 计算得到的值保存到 `party_counts`，后面的判断或输出会继续使用它。
    party_counts = Counter(x["canonical_game_state"]["party"]["count"] for x in parsed)
    # 把右侧 `Counter((x["legacy_compatibility"]["audio_preferences"]["mp3_music_volume_…` 计算得到的值保存到 `audio_counts`，后面的判断或输出会继续使用它。
    audio_counts = Counter((x["legacy_compatibility"]["audio_preferences"]["mp3_music_volume_level_0_to_9"],
                            # 执行这一条实际代码步骤：`x["legacy_compatibility"]["audio_preferences"]["wav_sfx_volume_level_0_to_9"]) for x in parsed)`；它与上下相邻语句共同完成当前函数的小任务。
                            x["legacy_compatibility"]["audio_preferences"]["wav_sfx_volume_level_0_to_9"]) for x in parsed)

    # 把右侧 `[]` 计算得到的值保存到 `active_transitions`，后面的判断或输出会继续使用它。
    active_transitions = []
    # 开始循环 `i in range(1, len(parsed))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(1, len(parsed)):
        # 把右侧 `parsed[i-1]["canonical_game_state"]["inventory"]["active_bank"]` 计算得到的值保存到 `old`，后面的判断或输出会继续使用它。
        old = parsed[i-1]["canonical_game_state"]["inventory"]["active_bank"]
        # 把右侧 `parsed[i]["canonical_game_state"]["inventory"]["active_bank"]` 计算得到的值保存到 `new`，后面的判断或输出会继续使用它。
        new = parsed[i]["canonical_game_state"]["inventory"]["active_bank"]
        # 检查条件 `old != new`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if old != new:
            # 把右侧 `_save_index(saves[i])` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
            idx = _save_index(saves[i])
            # 把这一条新结果追加到列表 `active_transitions`；不会覆盖前面已经收集的记录。
            active_transitions.append({
                # 给当前结果字典的“save”字段填写 `saves[i].name, "before": old, "after": new,`，这样导出的 JSON/表格能保留这一项证据。
                "save": saves[i].name, "before": old, "after": new,
                # 给当前结果字典的“walkthrough”字段填写 `labels.get(idx, ""),`，这样导出的 JSON/表格能保留这一项证据。
                "walkthrough": labels.get(idx, ""),
            })

    # 统计相邻流程档中实际发生变化的 GameVar。
    # 创建变量 `variable_change_frequency`（类型提示为 `Counter[int]`），并把 `Counter()` 的结果保存进去供后续步骤使用。
    variable_change_frequency: Counter[int] = Counter()
    # 把右侧 `[]` 计算得到的值保存到 `transition_rows`，后面的判断或输出会继续使用它。
    transition_rows = []
    # 开始循环 `i in range(1, len(parsed))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(1, len(parsed)):
        # 把右侧 `parsed[i-1]["canonical_game_state"]["world"]["game_variables"]` 计算得到的值保存到 `va`，后面的判断或输出会继续使用它。
        va = parsed[i-1]["canonical_game_state"]["world"]["game_variables"]
        # 把右侧 `parsed[i]["canonical_game_state"]["world"]["game_variables"]` 计算得到的值保存到 `vb`，后面的判断或输出会继续使用它。
        vb = parsed[i]["canonical_game_state"]["world"]["game_variables"]
        # 把右侧 `[j for j, (x, y) in enumerate(zip(va, vb)) if x != y]` 计算得到的值保存到 `diffs`，后面的判断或输出会继续使用它。
        diffs = [j for j, (x, y) in enumerate(zip(va, vb)) if x != y]
        # 调用 `variable_change_frequency.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        variable_change_frequency.update(diffs)
        # 把右侧 `_save_index(saves[i])` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
        idx = _save_index(saves[i])
        # 把这一条新结果追加到列表 `transition_rows`；不会覆盖前面已经收集的记录。
        transition_rows.append({
            # 给当前结果字典的“from”字段填写 `saves[i-1].name,`，这样导出的 JSON/表格能保留这一项证据。
            "from": saves[i-1].name,
            # 给当前结果字典的“to”字段填写 `saves[i].name,`，这样导出的 JSON/表格能保留这一项证据。
            "to": saves[i].name,
            # 给当前结果字典的“walkthrough”字段填写 `labels.get(idx, ""),`，这样导出的 JSON/表格能保留这一项证据。
            "walkthrough": labels.get(idx, ""),
            # 给当前结果字典的“changed_variable_count”字段填写 `len(diffs),`，这样导出的 JSON/表格能保留这一项证据。
            "changed_variable_count": len(diffs),
            # 给当前结果字典的“changed_variables”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
            "changed_variables": [
                # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
                {"index": j, "before": va[j], "after": vb[j]} for j in diffs
            ],
        })

    # 把右侧 `Path(args.商店基线).read_bytes() if args.商店基线 else None` 计算得到的值保存到 `shop_baseline`，后面的判断或输出会继续使用它。
    shop_baseline = Path(args.商店基线).read_bytes() if args.商店基线 else None
    # 把右侧 `0` 计算得到的值保存到 `shop_equal`，后面的判断或输出会继续使用它。
    shop_equal = 0
    # 创建变量 `shop_hashes`（类型提示为 `Counter[str]`），并把 `Counter()` 的结果保存进去供后续步骤使用。
    shop_hashes: Counter[str] = Counter()
    # 开始循环 `x, p in zip(parsed, saves)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for x, p in zip(parsed, saves):
        # 把右侧 `x["container"]["shop_block_length"]` 计算得到的值保存到 `sl`，后面的判断或输出会继续使用它。
        sl = x["container"]["shop_block_length"]
        # 检查条件 `not sl`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not sl:
            # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
            continue
        # 把右侧 `p.read_bytes()` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw = p.read_bytes()
        # 把右侧 `x["container"]["world_block_length"]` 计算得到的值保存到 `world_len`，后面的判断或输出会继续使用它。
        world_len = x["container"]["world_block_length"]
        # 把右侧 `raw[核心大小 + world_len:核心大小 + world_len + sl]` 计算得到的值保存到 `shop`，后面的判断或输出会继续使用它。
        shop = raw[核心大小 + world_len:核心大小 + world_len + sl]
        # 把右侧 `_sha(shop); shop_hashes[h] += 1` 计算得到的值保存到 `h`，后面的判断或输出会继续使用它。
        h = _sha(shop); shop_hashes[h] += 1
        # 检查条件 `shop_baseline is not None and shop == shop_baseline`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if shop_baseline is not None and shop == shop_baseline:
            # 执行一次赋值：把 `1` 的结果放入左侧 `shop_equal +` 对应的变量/字段。
            shop_equal += 1

    # 把右侧 `None` 计算得到的值保存到 `api8`，后面的判断或输出会继续使用它。
    api8 = None
    # 检查条件 `args.API`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if args.API:
        # 把右侧 `Path(args.API).read_bytes()` 计算得到的值保存到 `api`，后面的判断或输出会继续使用它。
        api = Path(args.API).read_bytes()
        # 检查条件 `len(api) >= 9 * 角色记录大小`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(api) >= 9 * 角色记录大小:
            # 把右侧 `api[8*角色记录大小:9*角色记录大小]` 计算得到的值保存到 `api8`，后面的判断或输出会继续使用它。
            api8 = api[8*角色记录大小:9*角色记录大小]
    # 创建变量 `role8_variants`（类型提示为 `Counter[str]`），并把 `Counter()` 的结果保存进去供后续步骤使用。
    role8_variants: Counter[str] = Counter()
    # 把右侧 `[]` 计算得到的值保存到 `role8_diff_from_api`，后面的判断或输出会继续使用它。
    role8_diff_from_api = []
    # 开始循环 `x, p in zip(parsed, saves)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for x, p in zip(parsed, saves):
        # 把右侧 `x["legacy_compatibility"]["role_record_8_save_only_non_authoritative"]["ra…` 计算得到的值保存到 `r8h`，后面的判断或输出会继续使用它。
        r8h = x["legacy_compatibility"]["role_record_8_save_only_non_authoritative"]["raw_sha256"]
        # 执行一次赋值：把 `1` 的结果放入左侧 `role8_variants[r8h] +` 对应的变量/字段。
        role8_variants[r8h] += 1
        # 检查条件 `api8 is not None`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if api8 is not None:
            # 把右侧 `p.read_bytes()[:核心大小]` 计算得到的值保存到 `core_raw`，后面的判断或输出会继续使用它。
            core_raw = p.read_bytes()[:核心大小]
            # 把右侧 `bytearray(core_raw)` 计算得到的值保存到 `core`，后面的判断或输出会继续使用它。
            core = bytearray(core_raw)
            # 检查条件 `_u32(core, 偏移_旧核心编码标记) == 0xFF`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if _u32(core, 偏移_旧核心编码标记) == 0xFF:
                # 调用 `_xor255_inplace` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
                _xor255_inplace(core)
            # 把右侧 `bytes(core[偏移_角色记录+8*角色记录大小:偏移_角色记录+9*角色记录大小])` 计算得到的值保存到 `r8`，后面的判断或输出会继续使用它。
            r8 = bytes(core[偏移_角色记录+8*角色记录大小:偏移_角色记录+9*角色记录大小])
            # 检查条件 `r8 != api8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if r8 != api8:
                # 把右侧 `[i for i, (a, b) in enumerate(zip(r8, api8)) if a != b]` 计算得到的值保存到 `diffs`，后面的判断或输出会继续使用它。
                diffs = [i for i, (a, b) in enumerate(zip(r8, api8)) if a != b]
                # 把这一条新结果追加到列表 `role8_diff_from_api`；不会覆盖前面已经收集的记录。
                role8_diff_from_api.append({
                    # 给当前结果字典的“save”字段填写 `p.name,`，这样导出的 JSON/表格能保留这一项证据。
                    "save": p.name,
                    # 给当前结果字典的“changed_byte_count”字段填写 `len(diffs),`，这样导出的 JSON/表格能保留这一项证据。
                    "changed_byte_count": len(diffs),
                    # 给当前结果字典的“first_offset”字段填写 `min(diffs) if diffs else None,`，这样导出的 JSON/表格能保留这一项证据。
                    "first_offset": min(diffs) if diffs else None,
                    # 给当前结果字典的“last_offset”字段填写 `max(diffs) if diffs else None,`，这样导出的 JSON/表格能保留这一项证据。
                    "last_offset": max(diffs) if diffs else None,
                    # 给当前结果字典的“sha256”字段填写 `_sha(r8),`，这样导出的 JSON/表格能保留这一项证据。
                    "sha256": _sha(r8),
                })

    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows = []
    # 开始循环 `p, x in zip(saves, parsed)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for p, x in zip(saves, parsed):
        # 把右侧 `_semantic_digest(x)` 计算得到的值保存到 `d`，后面的判断或输出会继续使用它。
        d = _semantic_digest(x)
        # 把右侧 `_save_index(p)` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
        idx = _save_index(p)
        # 把右侧 `p.name` 计算得到的值保存到 `d["file"]`，后面的判断或输出会继续使用它。
        d["file"] = p.name
        # 把右侧 `labels.get(idx, "")` 计算得到的值保存到 `d["walkthrough"]`，后面的判断或输出会继续使用它。
        d["walkthrough"] = labels.get(idx, "")
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append(d)

    # 把右侧 `{` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out = {
        # 给当前结果字典的“tool”字段填写 `{"name": "幽城TSF与GameState调查器", "version": 版本},`，这样导出的 JSON/表格能保留这一项证据。
        "tool": {"name": "幽城TSF与GameState调查器", "version": 版本},
        # 给当前结果字典的“corpus”字段填写 `{"directory": str(root), "save_count": len(saves)},`，这样导出的 JSON/表格能保留这一项证据。
        "corpus": {"directory": str(root), "save_count": len(saves)},
        # 给当前结果字典的“summary”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "summary": {
            # 给当前结果字典的“file_size_counts”字段填写 `{str(k): v for k, v in sorted(size_counts.items())},`，这样导出的 JSON/表格能保留这一项证据。
            "file_size_counts": {str(k): v for k, v in sorted(size_counts.items())},
            # 给当前结果字典的“dynamic_block_length_pairs_world_shop”字段填写 `{f"{k[0]},{k[1]}": v for k, v in sorted(block_pairs.items())},`，这样导出的 JSON/表格能保留这一项证据。
            "dynamic_block_length_pairs_world_shop": {f"{k[0]},{k[1]}": v for k, v in sorted(block_pairs.items())},
            # 给当前结果字典的“active_inventory_bank_counts”字段填写 `{str(k): v for k, v in sorted(active_counts.items())},`，这样导出的 JSON/表格能保留这一项证据。
            "active_inventory_bank_counts": {str(k): v for k, v in sorted(active_counts.items())},
            # 给当前结果字典的“party_count_distribution”字段填写 `{str(k): v for k, v in sorted(party_counts.items())},`，这样导出的 JSON/表格能保留这一项证据。
            "party_count_distribution": {str(k): v for k, v in sorted(party_counts.items())},
            # 给当前结果字典的“audio_level_distribution_mp3_wav”字段填写 `{f"{k[0]},{k[1]}": v for k, v in sorted(audio_counts.items())},`，这样导出的 JSON/表格能保留这一项证据。
            "audio_level_distribution_mp3_wav": {f"{k[0]},{k[1]}": v for k, v in sorted(audio_counts.items())},
            # 给当前结果字典的“unique_shop_state_hashes”字段填写 `len(shop_hashes),`，这样导出的 JSON/表格能保留这一项证据。
            "unique_shop_state_hashes": len(shop_hashes),
            # 给当前结果字典的“shop_states_equal_to_supplied_baseline”字段填写 `shop_equal if shop_baseline is not None else None,`，这样导出的 JSON/表格能保留这一项证据。
            "shop_states_equal_to_supplied_baseline": shop_equal if shop_baseline is not None else None,
            # 给当前结果字典的“distinct_game_variables_changed_between_adja…”字段填写 `len(variable_change_frequency),`，这样导出的 JSON/表格能保留这一项证据。
            "distinct_game_variables_changed_between_adjacent_saves": len(variable_change_frequency),
            # 给当前结果字典的“total_game_variable_changes_between_adjacent…”字段填写 `sum(variable_change_frequency.values()),`，这样导出的 JSON/表格能保留这一项证据。
            "total_game_variable_changes_between_adjacent_saves": sum(variable_change_frequency.values()),
            # 给当前结果字典的“role8_unique_variants”字段填写 `len(role8_variants),`，这样导出的 JSON/表格能保留这一项证据。
            "role8_unique_variants": len(role8_variants),
        },
        # 给当前结果字典的“active_inventory_bank_transitions”字段填写 `active_transitions,`，这样导出的 JSON/表格能保留这一项证据。
        "active_inventory_bank_transitions": active_transitions,
        # 给当前结果字典的“top_game_variable_change_frequency”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "top_game_variable_change_frequency": [
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"index": idx, "transition_count": n}
            # 开始循环 `idx, n in variable_change_frequency.most_common(100)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for idx, n in variable_change_frequency.most_common(100)
        ],
        # 给当前结果字典的“shop_state_hash_frequency”字段填写 `dict(shop_hashes),`，这样导出的 JSON/表格能保留这一项证据。
        "shop_state_hash_frequency": dict(shop_hashes),
        # 给当前结果字典的“role8_variants”字段填写 `dict(role8_variants),`，这样导出的 JSON/表格能保留这一项证据。
        "role8_variants": dict(role8_variants),
        # 给当前结果字典的“role8_differences_from_supplied_API_record8”字段填写 `role8_diff_from_api,`，这样导出的 JSON/表格能保留这一项证据。
        "role8_differences_from_supplied_API_record8": role8_diff_from_api,
        # 给当前结果字典的“saves”字段填写 `rows,`，这样导出的 JSON/表格能保留这一项证据。
        "saves": rows,
        # 给当前结果字典的“adjacent_transition_game_variable_changes”字段填写 `transition_rows,`，这样导出的 JSON/表格能保留这一项证据。
        "adjacent_transition_game_variable_changes": transition_rows,
    }
    # 调用 `_json_dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    _json_dump(out, Path(args.输出) if args.输出 else None)


# 定义函数 `cmd_storyvars`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_storyvars(args: argparse.Namespace) -> None:
    # 把右侧 `Path(args.dir)` 计算得到的值保存到 `root`，后面的判断或输出会继续使用它。
    root = Path(args.dir)
    # 把右侧 `_list_saves(root)` 计算得到的值保存到 `saves`，后面的判断或输出会继续使用它。
    saves = _list_saves(root)
    # 把右侧 `_extract_walkthrough_labels(Path(args.说明) if args.说明 else None)` 计算得到的值保存到 `labels`，后面的判断或输出会继续使用它。
    labels = _extract_walkthrough_labels(Path(args.说明) if args.说明 else None)
    # 把右侧 `[parse_tsf(p) for p in saves]` 计算得到的值保存到 `parsed`，后面的判断或输出会继续使用它。
    parsed = [parse_tsf(p) for p in saves]
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows = []
    # 创建变量 `freq`（类型提示为 `Counter[int]`），并把 `Counter()` 的结果保存进去供后续步骤使用。
    freq: Counter[int] = Counter()
    # 开始循环 `i in range(1, len(parsed))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(1, len(parsed)):
        # 把右侧 `parsed[i-1]["canonical_game_state"]["world"]["game_variables"]` 计算得到的值保存到 `a`，后面的判断或输出会继续使用它。
        a = parsed[i-1]["canonical_game_state"]["world"]["game_variables"]
        # 把右侧 `parsed[i]["canonical_game_state"]["world"]["game_variables"]` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
        b = parsed[i]["canonical_game_state"]["world"]["game_variables"]
        # 把右侧 `[{"index": j, "before": x, "after": y} for j, (x, y) in enumerate(zip(a, b…` 计算得到的值保存到 `changes`，后面的判断或输出会继续使用它。
        changes = [{"index": j, "before": x, "after": y} for j, (x, y) in enumerate(zip(a, b)) if x != y]
        # 调用 `freq.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        freq.update(c["index"] for c in changes)
        # 把右侧 `_save_index(saves[i])` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
        idx = _save_index(saves[i])
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append({
            # 给当前结果字典的“from”字段填写 `saves[i-1].name,`，这样导出的 JSON/表格能保留这一项证据。
            "from": saves[i-1].name,
            # 给当前结果字典的“to”字段填写 `saves[i].name,`，这样导出的 JSON/表格能保留这一项证据。
            "to": saves[i].name,
            # 给当前结果字典的“walkthrough”字段填写 `labels.get(idx, ""),`，这样导出的 JSON/表格能保留这一项证据。
            "walkthrough": labels.get(idx, ""),
            # 给当前结果字典的“changes”字段填写 `changes,`，这样导出的 JSON/表格能保留这一项证据。
            "changes": changes,
        })
    # 把右侧 `{` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out = {
        # 给当前结果字典的“save_count”字段填写 `len(saves),`，这样导出的 JSON/表格能保留这一项证据。
        "save_count": len(saves),
        # 给当前结果字典的“distinct_changed_variables”字段填写 `len(freq),`，这样导出的 JSON/表格能保留这一项证据。
        "distinct_changed_variables": len(freq),
        # 给当前结果字典的“change_frequency”字段填写 `[{"index": i, "count": n} for i, n in freq.most_common()],`，这样导出的 JSON/表格能保留这一项证据。
        "change_frequency": [{"index": i, "count": n} for i, n in freq.most_common()],
        # 给当前结果字典的“transitions”字段填写 `rows,`，这样导出的 JSON/表格能保留这一项证据。
        "transitions": rows,
        # 给当前结果字典的“warning”字段填写 `"相邻流程档并非每一步游戏操作后的快照；changed != 单一剧情事件直接写入。变量业务名必须结合EVE/SCI/动态Oracle再确…`，这样导出的 JSON/表格能保留这一项证据。
        "warning": "相邻流程档并非每一步游戏操作后的快照；changed != 单一剧情事件直接写入。变量业务名必须结合EVE/SCI/动态Oracle再确认。",
    }
    # 调用 `_json_dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    _json_dump(out, Path(args.输出) if args.输出 else None)


# 定义函数 `build_parser`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def build_parser() -> argparse.ArgumentParser:
    # 把右侧 `argparse.ArgumentParser(description="幽城TSF与GameState调查器（只读）")` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p = argparse.ArgumentParser(description="幽城TSF与GameState调查器（只读）")
    # 执行一次赋值：把 `"version", version=版本)` 的结果放入左侧 `p.add_argument("--版本", action` 对应的变量/字段。
    p.add_argument("--版本", action="version", version=版本)
    # 把右侧 `p.add_subparsers(dest="cmd", required=True)` 计算得到的值保存到 `sub`，后面的判断或输出会继续使用它。
    sub = p.add_subparsers(dest="cmd", required=True)

    # 把右侧 `sub.add_parser("tsf", help="解析单个TSF")` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
    s = sub.add_parser("tsf", help="解析单个TSF")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("file")
    # 执行一次赋值：把 `"store_true")` 的结果放入左侧 `s.add_argument("--包含全部变量", action` 对应的变量/字段。
    s.add_argument("--包含全部变量", action="store_true")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("--输出")
    # 执行一次赋值：把 `cmd_tsf)` 的结果放入左侧 `s.set_defaults(func` 对应的变量/字段。
    s.set_defaults(func=cmd_tsf)

    # 把右侧 `sub.add_parser("差分", help="比较两个TSF的语义状态")` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
    s = sub.add_parser("差分", help="比较两个TSF的语义状态")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("file_a")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("file_b")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("--输出")
    # 执行一次赋值：把 `cmd_diff)` 的结果放入左侧 `s.set_defaults(func` 对应的变量/字段。
    s.set_defaults(func=cmd_diff)

    # 把右侧 `sub.add_parser("批量", help="分析目录内连续流程存档")` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
    s = sub.add_parser("批量", help="分析目录内连续流程存档")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("dir")
    # 执行一次赋值：把 `"可选：流程存档说明.docx，用于给Save编号附剧情标签")` 的结果放入左侧 `s.add_argument("--说明", help` 对应的变量/字段。
    s.add_argument("--说明", help="可选：流程存档说明.docx，用于给Save编号附剧情标签")
    # 执行一次赋值：把 `"可选：原 public\\ShopItem.dat")` 的结果放入左侧 `s.add_argument("--商店基线", help` 对应的变量/字段。
    s.add_argument("--商店基线", help="可选：原 public\\ShopItem.dat")
    # 执行一次赋值：把 `"可选：已解码 API.ENC / Api.bin，用于比较保存专用role record #8")` 的结果放入左侧 `s.add_argument("--API", help` 对应的变量/字段。
    s.add_argument("--API", help="可选：已解码 API.ENC / Api.bin，用于比较保存专用role record #8")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("--输出")
    # 执行一次赋值：把 `cmd_batch)` 的结果放入左侧 `s.set_defaults(func` 对应的变量/字段。
    s.set_defaults(func=cmd_batch)

    # 把右侧 `sub.add_parser("剧情变量", help="导出相邻流程档5000个GameVar的变化")` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
    s = sub.add_parser("剧情变量", help="导出相邻流程档5000个GameVar的变化")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("dir")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("--说明")
    # 调用 `s.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    s.add_argument("--输出")
    # 执行一次赋值：把 `cmd_storyvars)` 的结果放入左侧 `s.set_defaults(func` 对应的变量/字段。
    s.set_defaults(func=cmd_storyvars)
    # 把 `p` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return p


# 定义函数 `main`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def main() -> None:
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 检查条件 `hasattr(sys.stdout, "reconfigure")`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if hasattr(sys.stdout, "reconfigure"):
            # 执行一次赋值：把 `"utf-8", errors="replace")` 的结果放入左侧 `sys.stdout.reconfigure(encoding` 对应的变量/字段。
            sys.stdout.reconfigure(encoding="utf-8", errors="replace")
            # 执行一次赋值：把 `"utf-8", errors="replace")` 的结果放入左侧 `sys.stderr.reconfigure(encoding` 对应的变量/字段。
            sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    # 捕获 `Exception` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception:
        # 这里故意什么也不做，只保留语法上的空分支位置。
        pass
    # 把右侧 `build_parser().parse_args()` 计算得到的值保存到 `args`，后面的判断或输出会继续使用它。
    args = build_parser().parse_args()
    # 调用 `args.func` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    args.func(args)


# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__ == "__main__":
    # 调用 `main` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    main()
