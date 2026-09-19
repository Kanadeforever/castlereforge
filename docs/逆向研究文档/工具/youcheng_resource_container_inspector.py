#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""《幽城幻剑录》DAT/SYS 资源容器调查器。

默认只读。只有“提取”子命令会写出内嵌文件，而且必须显式给出输出目录。
本工具只把当前样本和同 SHA-256 RPG.exe 已经支持的结构写成已确认项；
头部两个常量字段仍以未命名原值输出，不猜测作者名称。
"""

# 启用较新的 Python 注解语法行为，让类型注解先按文字保存，避免旧版本在定义阶段立即求值。
from __future__ import annotations

# 导入 `argparse` 模块；这里只取得标准库功能，不会修改游戏文件。
import argparse
# 导入 `hashlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import hashlib
# 导入 `json` 模块；这里只取得标准库功能，不会修改游戏文件。
import json
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path
# 导入 `struct` 模块；这里只取得标准库功能，不会修改游戏文件。
import struct
# 从 `typing` 模块导入 `Any, Iterable`，后面的代码会直接使用这些现成组件。
from typing import Any, Iterable


# 把右侧 `"0.1.0"` 计算得到的值保存到 `VERSION`，后面的判断或输出会继续使用它。
VERSION = "0.1.0"
# 把右侧 `"b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"` 计算得到的值保存到 `ORACLE_SHA256`，后面的判断或输出会继续使用它。
ORACLE_SHA256 = "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
# 把右侧 `10` 计算得到的值保存到 `HEADER_SIZE`，后面的判断或输出会继续使用它。
HEADER_SIZE = 10
# 把右侧 `39` 计算得到的值保存到 `RECORD_SIZE`，后面的判断或输出会继续使用它。
RECORD_SIZE = 39


# 定义函数 `sha256_bytes`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sha256_bytes(data: bytes) -> str:
    # 把 `hashlib.sha256(data).hexdigest()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return hashlib.sha256(data).hexdigest()


# 定义函数 `sha256_file`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sha256_file(path: Path) -> str:
    # 把右侧 `hashlib.sha256()` 计算得到的值保存到 `h`，后面的判断或输出会继续使用它。
    h = hashlib.sha256()
    # 进入受管理的资源作用域 `path.open("rb") as stream`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
    with path.open("rb") as stream:
        # 开始循环 `block in iter(lambda: stream.read(1024 * 1024), b"")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            # 调用 `h.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            h.update(block)
    # 把 `h.hexdigest()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return h.hexdigest()


# 定义函数 `u16`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def u16(data: bytes, offset: int) -> int:
    # 把 `struct.unpack_from("<H", data, offset)[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return struct.unpack_from("<H", data, offset)[0]


# 定义函数 `u32`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def u32(data: bytes, offset: int) -> int:
    # 把 `struct.unpack_from("<I", data, offset)[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return struct.unpack_from("<I", data, offset)[0]


# 定义函数 `decode_name`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def decode_name(raw: bytes) -> tuple[str, bool, bool]:
    # 把右侧 `raw.find(b"\0")` 计算得到的值保存到 `nul`，后面的判断或输出会继续使用它。
    nul = raw.find(b"\0")
    # 把右侧 `nul >= 0` 计算得到的值保存到 `terminated`，后面的判断或输出会继续使用它。
    terminated = nul >= 0
    # 把右侧 `raw if nul < 0 else raw[:nul]` 计算得到的值保存到 `payload`，后面的判断或输出会继续使用它。
    payload = raw if nul < 0 else raw[:nul]
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 把 `payload.decode("ascii"), terminated, True` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return payload.decode("ascii"), terminated, True
    # 捕获 `UnicodeDecodeError` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except UnicodeDecodeError:
        # 把 `payload.decode("latin1", errors="replace"), terminated, False` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return payload.decode("latin1", errors="replace"), terminated, False


# 定义函数 `safe_component`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def safe_component(name: str) -> bool:
    # 把 `bool(name) and name not in {".", ".."} and "/" not in name and "\\" not in name and "\0" …` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return bool(name) and name not in {".", ".."} and "/" not in name and "\\" not in name and "\0" not in name


# 定义函数 `record_view`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def record_view(data: bytes, index: int) -> dict[str, Any]:
    # 把右侧 `HEADER_SIZE + index * RECORD_SIZE` 计算得到的值保存到 `offset`，后面的判断或输出会继续使用它。
    offset = HEADER_SIZE + index * RECORD_SIZE
    # 把右侧 `data[offset + 10 : offset + 35]` 计算得到的值保存到 `raw_name`，后面的判断或输出会继续使用它。
    raw_name = data[offset + 10 : offset + 35]
    # 执行一次赋值：把 `decode_name(raw_name)` 的结果放入左侧 `name, terminated, ascii_only` 对应的变量/字段。
    name, terminated, ascii_only = decode_name(raw_name)
    # 把右侧 `u16(data, offset)` 计算得到的值保存到 `kind`，后面的判断或输出会继续使用它。
    kind = u16(data, offset)
    # 把右侧 `u32(data, offset + 2)` 计算得到的值保存到 `value`，后面的判断或输出会继续使用它。
    value = u32(data, offset + 2)
    # 把右侧 `u32(data, offset + 6)` 计算得到的值保存到 `unknown`，后面的判断或输出会继续使用它。
    unknown = u32(data, offset + 6)
    # 把右侧 `u32(data, offset + 35)` 计算得到的值保存到 `target`，后面的判断或输出会继续使用它。
    target = u32(data, offset + 35)
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“索引”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
        "索引": index,
        # 给当前结果字典的“记录偏移”字段填写 `offset,`，这样导出的 JSON/表格能保留这一项证据。
        "记录偏移": offset,
        # 给当前结果字典的“类型原值_加0”字段填写 `kind,`，这样导出的 JSON/表格能保留这一项证据。
        "类型原值_加0": kind,
        # 给当前结果字典的“类型”字段填写 `"目录" if kind == 1 else "文件" if kind == 0 else "未知",`，这样导出的 JSON/表格能保留这一项证据。
        "类型": "目录" if kind == 1 else "文件" if kind == 0 else "未知",
        # 给当前结果字典的“大小或子项数_加2”字段填写 `value,`，这样导出的 JSON/表格能保留这一项证据。
        "大小或子项数_加2": value,
        # 给当前结果字典的“未命名_加6”字段填写 `unknown,`，这样导出的 JSON/表格能保留这一项证据。
        "未命名_加6": unknown,
        # 给当前结果字典的“名称_加10”字段填写 `name,`，这样导出的 JSON/表格能保留这一项证据。
        "名称_加10": name,
        # 给当前结果字典的“名称25字节内有终止零”字段填写 `terminated,`，这样导出的 JSON/表格能保留这一项证据。
        "名称25字节内有终止零": terminated,
        # 给当前结果字典的“名称为纯ASCII”字段填写 `ascii_only,`，这样导出的 JSON/表格能保留这一项证据。
        "名称为纯ASCII": ascii_only,
        # 给当前结果字典的“数据或子表偏移_加35”字段填写 `target,`，这样导出的 JSON/表格能保留这一项证据。
        "数据或子表偏移_加35": target,
    }


# 定义函数 `parse_container`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_container(path: Path, *, include_records: bool = True) -> dict[str, Any]:
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `data`，后面的判断或输出会继续使用它。
    data = path.read_bytes()
    # 创建变量 `result`（类型提示为 `dict[str, Any]`），并把 `{` 的结果保存进去供后续步骤使用。
    result: dict[str, Any] = {
        # 给当前结果字典的“文件”字段填写 `str(path),`，这样导出的 JSON/表格能保留这一项证据。
        "文件": str(path),
        # 给当前结果字典的“字节数”字段填写 `len(data),`，这样导出的 JSON/表格能保留这一项证据。
        "字节数": len(data),
        # 给当前结果字典的“SHA256”字段填写 `sha256_bytes(data),`，这样导出的 JSON/表格能保留这一项证据。
        "SHA256": sha256_bytes(data),
        # 给当前结果字典的“识别为资源容器”字段填写 `False,`，这样导出的 JSON/表格能保留这一项证据。
        "识别为资源容器": False,
    }
    # 检查条件 `len(data) < HEADER_SIZE + RECORD_SIZE`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(data) < HEADER_SIZE + RECORD_SIZE:
        # 执行一次赋值：把 `"文件短于10字节头加一条39字节记录"` 的结果放入左侧 `result["不识别原因"]` 对应的变量/字段。
        result["不识别原因"] = "文件短于10字节头加一条39字节记录"
        # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return result

    # 把右侧 `u32(data, 0)` 计算得到的值保存到 `header_u32`，后面的判断或输出会继续使用它。
    header_u32 = u32(data, 0)
    # 把右侧 `u16(data, 4)` 计算得到的值保存到 `header_u16`，后面的判断或输出会继续使用它。
    header_u16 = u16(data, 4)
    # 把右侧 `u32(data, 6)` 计算得到的值保存到 `index_size`，后面的判断或输出会继续使用它。
    index_size = u32(data, 6)
    # 执行一次赋值：把 `{` 的结果放入左侧 `result["头部"]` 对应的变量/字段。
    result["头部"] = {
        # 给当前结果字典的“未命名_u32_加0”字段填写 `header_u32,`，这样导出的 JSON/表格能保留这一项证据。
        "未命名_u32_加0": header_u32,
        # 给当前结果字典的“未命名_u16_加4”字段填写 `header_u16,`，这样导出的 JSON/表格能保留这一项证据。
        "未命名_u16_加4": header_u16,
        # 给当前结果字典的“索引区字节数_u32_加6”字段填写 `index_size,`，这样导出的 JSON/表格能保留这一项证据。
        "索引区字节数_u32_加6": index_size,
    }
    # 检查条件 `header_u32 != 1 or header_u16 != 1`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if header_u32 != 1 or header_u16 != 1:
        # 执行一次赋值：把 `"当前九个已确认资源容器共有的头部1/1签名不匹配"` 的结果放入左侧 `result["不识别原因"]` 对应的变量/字段。
        result["不识别原因"] = "当前九个已确认资源容器共有的头部1/1签名不匹配"
        # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return result
    # 检查条件 `index_size < RECORD_SIZE or index_size % RECORD_SIZE`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if index_size < RECORD_SIZE or index_size % RECORD_SIZE:
        # 执行一次赋值：把 `"头部+6不是正整数条39字节记录"` 的结果放入左侧 `result["不识别原因"]` 对应的变量/字段。
        result["不识别原因"] = "头部+6不是正整数条39字节记录"
        # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return result

    # 把右侧 `index_size // RECORD_SIZE` 计算得到的值保存到 `count`，后面的判断或输出会继续使用它。
    count = index_size // RECORD_SIZE
    # 把右侧 `HEADER_SIZE + index_size` 计算得到的值保存到 `data_base`，后面的判断或输出会继续使用它。
    data_base = HEADER_SIZE + index_size
    # 检查条件 `data_base > len(data)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if data_base > len(data):
        # 执行一次赋值：把 `"索引区越过文件末尾"` 的结果放入左侧 `result["不识别原因"]` 对应的变量/字段。
        result["不识别原因"] = "索引区越过文件末尾"
        # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return result

    # 把右侧 `[record_view(data, index) for index in range(count)]` 计算得到的值保存到 `records`，后面的判断或输出会继续使用它。
    records = [record_view(data, index) for index in range(count)]
    # 创建变量 `errors`（类型提示为 `list[str]`），并把 `[]` 的结果保存进去供后续步骤使用。
    errors: list[str] = []
    # 创建变量 `warnings`（类型提示为 `list[str]`），并把 `[]` 的结果保存进去供后续步骤使用。
    warnings: list[str] = []
    # 开始循环 `record in records`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for record in records:
        # 把右侧 `record["索引"]` 计算得到的值保存到 `index`，后面的判断或输出会继续使用它。
        index = record["索引"]
        # 把右侧 `record["类型原值_加0"]` 计算得到的值保存到 `kind`，后面的判断或输出会继续使用它。
        kind = record["类型原值_加0"]
        # 把右侧 `record["大小或子项数_加2"]` 计算得到的值保存到 `value`，后面的判断或输出会继续使用它。
        value = record["大小或子项数_加2"]
        # 把右侧 `record["数据或子表偏移_加35"]` 计算得到的值保存到 `target`，后面的判断或输出会继续使用它。
        target = record["数据或子表偏移_加35"]
        # 把右侧 `record["名称_加10"]` 计算得到的值保存到 `name`，后面的判断或输出会继续使用它。
        name = record["名称_加10"]
        # 检查条件 `kind not in {0, 1}`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if kind not in {0, 1}:
            # 把这一条新结果追加到列表 `errors`；不会覆盖前面已经收集的记录。
            errors.append(f"记录{index}类型原值不是0/1：{kind}")
        # 检查条件 `not record["名称25字节内有终止零"]`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not record["名称25字节内有终止零"]:
            # 把这一条新结果追加到列表 `errors`；不会覆盖前面已经收集的记录。
            errors.append(f"记录{index}名称25字节内没有NUL终止")
        # 检查条件 `not safe_component(name)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not safe_component(name):
            # 把这一条新结果追加到列表 `errors`；不会覆盖前面已经收集的记录。
            errors.append(f"记录{index}名称不能作为安全单级路径：{name!r}")
        # 检查条件 `kind == 0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if kind == 0:
            # 执行一次赋值：把 `target + value` 的结果放入左侧 `record["数据结束偏移"]` 对应的变量/字段。
            record["数据结束偏移"] = target + value
            # 执行这一条实际代码步骤：`record["数据在界内"] = target >= data_base and target + value <= len(data)`；它与上下相邻语句共同完成当前函数的小任务。
            record["数据在界内"] = target >= data_base and target + value <= len(data)
            # 检查条件 `not record["数据在界内"]`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not record["数据在界内"]:
                # 执行一次赋值：把 `{target}, size={value}")` 的结果放入左侧 `errors.append(f"文件记录{index}数据范围越界：offset` 对应的变量/字段。
                errors.append(f"文件记录{index}数据范围越界：offset={target}, size={value}")
        # 前一个条件没有成立时，再检查 `kind == 1`；成立才执行这一分支。
        elif kind == 1:
            # 把右侧 `target >= HEADER_SIZE and (target - HEADER_SIZE) % RECORD_SIZE == 0` 计算得到的值保存到 `aligned`，后面的判断或输出会继续使用它。
            aligned = target >= HEADER_SIZE and (target - HEADER_SIZE) % RECORD_SIZE == 0
            # 把右侧 `(target - HEADER_SIZE) // RECORD_SIZE if aligned else None` 计算得到的值保存到 `child_index`，后面的判断或输出会继续使用它。
            child_index = (target - HEADER_SIZE) // RECORD_SIZE if aligned else None
            # 执行一次赋值：把 `child_index` 的结果放入左侧 `record["首子项索引"]` 对应的变量/字段。
            record["首子项索引"] = child_index
            # 执行一次赋值：把 `bool(` 的结果放入左侧 `record["子表对齐且在索引区"]` 对应的变量/字段。
            record["子表对齐且在索引区"] = bool(
                # 执行这一条实际代码步骤：`aligned and child_index is not None and child_index + value <= count`；它与上下相邻语句共同完成当前函数的小任务。
                aligned and child_index is not None and child_index + value <= count
            )
            # 检查条件 `not record["子表对齐且在索引区"]`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not record["子表对齐且在索引区"]:
                # 把这一条新结果追加到列表 `errors`；不会覆盖前面已经收集的记录。
                errors.append(f"目录记录{index}子表越界或未按39字节对齐")

    # 创建变量 `reachable`（类型提示为 `set[int]`），并把 `set()` 的结果保存进去供后续步骤使用。
    reachable: set[int] = set()
    # 创建变量 `active`（类型提示为 `set[int]`），并把 `set()` 的结果保存进去供后续步骤使用。
    active: set[int] = set()
    # 创建变量 `tree_files`（类型提示为 `list[dict[str, Any]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    tree_files: list[dict[str, Any]] = []
    # 创建变量 `tree_dirs`（类型提示为 `list[dict[str, Any]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    tree_dirs: list[dict[str, Any]] = []

    # 定义函数 `walk`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def walk(index: int, parent: tuple[str, ...]) -> None:
        # 检查条件 `index in active`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if index in active:
            # 把这一条新结果追加到列表 `errors`；不会覆盖前面已经收集的记录。
            errors.append(f"目录图存在环：记录{index}")
            # 结束当前函数并返回；这里没有额外结果值。
            return
        # 检查条件 `not (0 <= index < count)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not (0 <= index < count):
            # 把这一条新结果追加到列表 `errors`；不会覆盖前面已经收集的记录。
            errors.append(f"目录图引用越界记录：{index}")
            # 结束当前函数并返回；这里没有额外结果值。
            return
        # 把右侧 `records[index]` 计算得到的值保存到 `record`，后面的判断或输出会继续使用它。
        record = records[index]
        # 把当前值加入集合 `reachable`；集合会自动避免完全相同的重复项。
        reachable.add(index)
        # 把右侧 `record["名称_加10"]` 计算得到的值保存到 `name`，后面的判断或输出会继续使用它。
        name = record["名称_加10"]
        # 把右侧 `parent + (name,)` 计算得到的值保存到 `path_parts`，后面的判断或输出会继续使用它。
        path_parts = parent + (name,)
        # 检查条件 `record["类型原值_加0"] == 0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if record["类型原值_加0"] == 0:
            # 把这一条新结果追加到列表 `tree_files`；不会覆盖前面已经收集的记录。
            tree_files.append(
                # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
                {
                    # 给当前结果字典的“记录索引”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
                    "记录索引": index,
                    # 给当前结果字典的“容器内路径”字段填写 `"/".join(path_parts),`，这样导出的 JSON/表格能保留这一项证据。
                    "容器内路径": "/".join(path_parts),
                    # 给当前结果字典的“名称”字段填写 `name,`，这样导出的 JSON/表格能保留这一项证据。
                    "名称": name,
                    # 给当前结果字典的“字节数”字段填写 `record["大小或子项数_加2"],`，这样导出的 JSON/表格能保留这一项证据。
                    "字节数": record["大小或子项数_加2"],
                    # 给当前结果字典的“数据偏移”字段填写 `record["数据或子表偏移_加35"],`，这样导出的 JSON/表格能保留这一项证据。
                    "数据偏移": record["数据或子表偏移_加35"],
                    # 给当前结果字典的“SHA256”字段填写 `sha256_bytes(`，这样导出的 JSON/表格能保留这一项证据。
                    "SHA256": sha256_bytes(
                        # 执行这一条实际代码步骤：`data[`；它与上下相邻语句共同完成当前函数的小任务。
                        data[
                            # 继续填写当前数据结构或参数列表中的一项：`record["数据或子表偏移_加35"] :`。
                            record["数据或子表偏移_加35"] :
                            # 执行这一条实际代码步骤：`record["数据或子表偏移_加35"] + record["大小或子项数_加2"]`；它与上下相邻语句共同完成当前函数的小任务。
                            record["数据或子表偏移_加35"] + record["大小或子项数_加2"]
                        ]
                    ),
                }
            )
            # 结束当前函数并返回；这里没有额外结果值。
            return
        # 检查条件 `record["类型原值_加0"] != 1`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if record["类型原值_加0"] != 1:
            # 结束当前函数并返回；这里没有额外结果值。
            return
        # 把这一条新结果追加到列表 `tree_dirs`；不会覆盖前面已经收集的记录。
        tree_dirs.append(
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {
                # 给当前结果字典的“记录索引”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
                "记录索引": index,
                # 给当前结果字典的“容器内路径”字段填写 `"/".join(path_parts),`，这样导出的 JSON/表格能保留这一项证据。
                "容器内路径": "/".join(path_parts),
                # 给当前结果字典的“直接子项数”字段填写 `record["大小或子项数_加2"],`，这样导出的 JSON/表格能保留这一项证据。
                "直接子项数": record["大小或子项数_加2"],
                # 给当前结果字典的“首子项索引”字段填写 `record.get("首子项索引"),`，这样导出的 JSON/表格能保留这一项证据。
                "首子项索引": record.get("首子项索引"),
            }
        )
        # 把右侧 `record.get("首子项索引")` 计算得到的值保存到 `child`，后面的判断或输出会继续使用它。
        child = record.get("首子项索引")
        # 检查条件 `child is None`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if child is None:
            # 结束当前函数并返回；这里没有额外结果值。
            return
        # 把当前值加入集合 `active`；集合会自动避免完全相同的重复项。
        active.add(index)
        # 开始循环 `child_index in range(child, child + record["大小或子项数_加2"])`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for child_index in range(child, child + record["大小或子项数_加2"]):
            # 调用 `walk` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            walk(child_index, path_parts)
        # 调用 `active.remove` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        active.remove(index)

    # 检查条件 `records`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if records:
        # 调用 `walk` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        walk(0, ())

    # 把右侧 `sorted(set(range(count)) - reachable)` 计算得到的值保存到 `unreachable`，后面的判断或输出会继续使用它。
    unreachable = sorted(set(range(count)) - reachable)
    # 检查条件 `unreachable`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if unreachable:
        # 把这一条新结果追加到列表 `warnings`；不会覆盖前面已经收集的记录。
        warnings.append(f"有{len(unreachable)}条索引记录不能从记录0沿目录子表到达")

    # 把右侧 `sorted(tree_files, key=lambda item: (item["数据偏移"], item["记录索引"]))` 计算得到的值保存到 `files_by_offset`，后面的判断或输出会继续使用它。
    files_by_offset = sorted(tree_files, key=lambda item: (item["数据偏移"], item["记录索引"]))
    # 创建变量 `data_gaps`（类型提示为 `list[dict[str, int]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    data_gaps: list[dict[str, int]] = []
    # 创建变量 `data_overlaps`（类型提示为 `list[dict[str, int]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    data_overlaps: list[dict[str, int]] = []
    # 把右侧 `data_base` 计算得到的值保存到 `cursor`，后面的判断或输出会继续使用它。
    cursor = data_base
    # 开始循环 `item in files_by_offset`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for item in files_by_offset:
        # 把右侧 `item["数据偏移"]` 计算得到的值保存到 `start`，后面的判断或输出会继续使用它。
        start = item["数据偏移"]
        # 把右侧 `start + item["字节数"]` 计算得到的值保存到 `end`，后面的判断或输出会继续使用它。
        end = start + item["字节数"]
        # 检查条件 `start > cursor`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if start > cursor:
            # 把这一条新结果追加到列表 `data_gaps`；不会覆盖前面已经收集的记录。
            data_gaps.append({"起始": cursor, "结束": start, "字节数": start - cursor})
        # 前一个条件没有成立时，再检查 `start < cursor`；成立才执行这一分支。
        elif start < cursor:
            # 把这一条新结果追加到列表 `data_overlaps`；不会覆盖前面已经收集的记录。
            data_overlaps.append({"起始": start, "前一结束": cursor, "重叠字节数": cursor - start})
        # 把右侧 `max(cursor, end)` 计算得到的值保存到 `cursor`，后面的判断或输出会继续使用它。
        cursor = max(cursor, end)
    # 检查条件 `cursor < len(data)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if cursor < len(data):
        # 把这一条新结果追加到列表 `data_gaps`；不会覆盖前面已经收集的记录。
        data_gaps.append({"起始": cursor, "结束": len(data), "字节数": len(data) - cursor})
    # 检查条件 `data_overlaps`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if data_overlaps:
        # 把这一条新结果追加到列表 `errors`；不会覆盖前面已经收集的记录。
        errors.append(f"内嵌文件数据区存在{len(data_overlaps)}处重叠")

    # 创建变量 `folded_names`（类型提示为 `dict[str, list[str]]`），并把 `{}` 的结果保存进去供后续步骤使用。
    folded_names: dict[str, list[str]] = {}
    # 开始循环 `item in tree_files`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for item in tree_files:
        # 调用 `folded_names.setdefault` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        folded_names.setdefault(item["名称"].casefold(), []).append(item["容器内路径"])
    # 把右侧 `{key: value for key, value in folded_names.items() if len(value) > 1}` 计算得到的值保存到 `duplicate_names`，后面的判断或输出会继续使用它。
    duplicate_names = {key: value for key, value in folded_names.items() if len(value) > 1}

    # 调用 `result.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    result.update(
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        {
            # 给当前结果字典的“识别为资源容器”字段填写 `True,`，这样导出的 JSON/表格能保留这一项证据。
            "识别为资源容器": True,
            # 给当前结果字典的“记录大小”字段填写 `RECORD_SIZE,`，这样导出的 JSON/表格能保留这一项证据。
            "记录大小": RECORD_SIZE,
            # 给当前结果字典的“索引记录数”字段填写 `count,`，这样导出的 JSON/表格能保留这一项证据。
            "索引记录数": count,
            # 给当前结果字典的“数据区起点”字段填写 `data_base,`，这样导出的 JSON/表格能保留这一项证据。
            "数据区起点": data_base,
            # 给当前结果字典的“目录数”字段填写 `len(tree_dirs),`，这样导出的 JSON/表格能保留这一项证据。
            "目录数": len(tree_dirs),
            # 给当前结果字典的“文件数”字段填写 `len(tree_files),`，这样导出的 JSON/表格能保留这一项证据。
            "文件数": len(tree_files),
            # 给当前结果字典的“从记录0可达记录数”字段填写 `len(reachable),`，这样导出的 JSON/表格能保留这一项证据。
            "从记录0可达记录数": len(reachable),
            # 给当前结果字典的“不可达记录索引”字段填写 `unreachable,`，这样导出的 JSON/表格能保留这一项证据。
            "不可达记录索引": unreachable,
            # 给当前结果字典的“目录树”字段填写 `tree_dirs,`，这样导出的 JSON/表格能保留这一项证据。
            "目录树": tree_dirs,
            # 给当前结果字典的“内嵌文件”字段填写 `tree_files,`，这样导出的 JSON/表格能保留这一项证据。
            "内嵌文件": tree_files,
            # 给当前结果字典的“大小写折叠后重复文件名”字段填写 `duplicate_names,`，这样导出的 JSON/表格能保留这一项证据。
            "大小写折叠后重复文件名": duplicate_names,
            # 给当前结果字典的“数据区间隙”字段填写 `data_gaps,`，这样导出的 JSON/表格能保留这一项证据。
            "数据区间隙": data_gaps,
            # 给当前结果字典的“数据区重叠”字段填写 `data_overlaps,`，这样导出的 JSON/表格能保留这一项证据。
            "数据区重叠": data_overlaps,
            # 给当前结果字典的“类型原值分布”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            "类型原值分布": {
                # 调用 `str` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
                str(kind): sum(1 for record in records if record["类型原值_加0"] == kind)
                # 开始循环 `kind in sorted({record["类型原值_加0"] for record in records})`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                for kind in sorted({record["类型原值_加0"] for record in records})
            },
            # 给当前结果字典的“加6未命名字段分布”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            "加6未命名字段分布": {
                # 调用 `str` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
                str(value): sum(1 for record in records if record["未命名_加6"] == value)
                # 开始循环 `value in sorted({record["未命名_加6"] for record in records})`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                for value in sorted({record["未命名_加6"] for record in records})
            },
            # 给当前结果字典的“错误”字段填写 `errors,`，这样导出的 JSON/表格能保留这一项证据。
            "错误": errors,
            # 给当前结果字典的“警告”字段填写 `warnings,`，这样导出的 JSON/表格能保留这一项证据。
            "警告": warnings,
            # 给当前结果字典的“结构全部通过”字段填写 `not errors,`，这样导出的 JSON/表格能保留这一项证据。
            "结构全部通过": not errors,
        }
    )
    # 检查条件 `include_records`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if include_records:
        # 执行一次赋值：把 `records` 的结果放入左侧 `result["全部39字节记录"]` 对应的变量/字段。
        result["全部39字节记录"] = records
    # 把 `result` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return result


# 定义函数 `iter_dat_files`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def iter_dat_files(root: Path) -> Iterable[Path]:
    # 把 `sorted(` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return sorted(
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        (path for path in root.rglob("*") if path.is_file() and path.suffix.casefold() == ".dat"),
        # 把右侧 `lambda path: path.relative_to(root).as_posix().casefold(),` 计算得到的值保存到 `key`，后面的判断或输出会继续使用它。
        key=lambda path: path.relative_to(root).as_posix().casefold(),
    )


# 定义函数 `batch_report`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def batch_report(root: Path) -> dict[str, Any]:
    # 把右侧 `[]` 计算得到的值保存到 `reports`，后面的判断或输出会继续使用它。
    reports = []
    # 开始循环 `path in iter_dat_files(root)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for path in iter_dat_files(root):
        # 把右侧 `parse_container(path, include_records=False)` 计算得到的值保存到 `item`，后面的判断或输出会继续使用它。
        item = parse_container(path, include_records=False)
        # 执行一次赋值：把 `path.relative_to(root).as_posix()` 的结果放入左侧 `item["相对路径"]` 对应的变量/字段。
        item["相对路径"] = path.relative_to(root).as_posix()
        # 把这一条新结果追加到列表 `reports`；不会覆盖前面已经收集的记录。
        reports.append(item)
    # 把右侧 `[item for item in reports if item["识别为资源容器"]]` 计算得到的值保存到 `recognized`，后面的判断或输出会继续使用它。
    recognized = [item for item in reports if item["识别为资源容器"]]
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `VERSION,`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本": VERSION,
        # 给当前结果字典的“扫描根”字段填写 `str(root),`，这样导出的 JSON/表格能保留这一项证据。
        "扫描根": str(root),
        # 给当前结果字典的“DAT文件数”字段填写 `len(reports),`，这样导出的 JSON/表格能保留这一项证据。
        "DAT文件数": len(reports),
        # 给当前结果字典的“识别资源容器数”字段填写 `len(recognized),`，这样导出的 JSON/表格能保留这一项证据。
        "识别资源容器数": len(recognized),
        # 给当前结果字典的“非本格式DAT数”字段填写 `len(reports) - len(recognized),`，这样导出的 JSON/表格能保留这一项证据。
        "非本格式DAT数": len(reports) - len(recognized),
        # 给当前结果字典的“资源容器结构全部通过”字段填写 `all(item.get("结构全部通过", False) for item in recognized),`，这样导出的 JSON/表格能保留这一项证据。
        "资源容器结构全部通过": all(item.get("结构全部通过", False) for item in recognized),
        # 给当前结果字典的“资源容器总目录数”字段填写 `sum(item["目录数"] for item in recognized),`，这样导出的 JSON/表格能保留这一项证据。
        "资源容器总目录数": sum(item["目录数"] for item in recognized),
        # 给当前结果字典的“资源容器总内嵌文件数”字段填写 `sum(item["文件数"] for item in recognized),`，这样导出的 JSON/表格能保留这一项证据。
        "资源容器总内嵌文件数": sum(item["文件数"] for item in recognized),
        # 给当前结果字典的“逐文件”字段填写 `reports,`，这样导出的 JSON/表格能保留这一项证据。
        "逐文件": reports,
    }


# 定义函数 `parse_pe_sections`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_pe_sections(data: bytes) -> tuple[int, list[dict[str, int | str]]]:
    # 检查条件 `len(data) < 0x100 or data[:2] != b"MZ"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(data) < 0x100 or data[:2] != b"MZ":
        # 检测到不能继续的情况，主动抛出异常 `ValueError("不是MZ可执行文件")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("不是MZ可执行文件")
    # 把右侧 `u32(data, 0x3C)` 计算得到的值保存到 `pe_offset`，后面的判断或输出会继续使用它。
    pe_offset = u32(data, 0x3C)
    # 检查条件 `data[pe_offset : pe_offset + 4] != b"PE\0\0"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if data[pe_offset : pe_offset + 4] != b"PE\0\0":
        # 检测到不能继续的情况，主动抛出异常 `ValueError("找不到PE签名")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("找不到PE签名")
    # 把右侧 `pe_offset + 4` 计算得到的值保存到 `coff`，后面的判断或输出会继续使用它。
    coff = pe_offset + 4
    # 把右侧 `u16(data, coff + 2)` 计算得到的值保存到 `section_count`，后面的判断或输出会继续使用它。
    section_count = u16(data, coff + 2)
    # 把右侧 `u16(data, coff + 16)` 计算得到的值保存到 `optional_size`，后面的判断或输出会继续使用它。
    optional_size = u16(data, coff + 16)
    # 把右侧 `coff + 20` 计算得到的值保存到 `optional`，后面的判断或输出会继续使用它。
    optional = coff + 20
    # 检查条件 `u16(data, optional) != 0x10B`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if u16(data, optional) != 0x10B:
        # 检测到不能继续的情况，主动抛出异常 `ValueError("当前验证器只支持PE32")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("当前验证器只支持PE32")
    # 把右侧 `u32(data, optional + 28)` 计算得到的值保存到 `image_base`，后面的判断或输出会继续使用它。
    image_base = u32(data, optional + 28)
    # 把右侧 `optional + optional_size` 计算得到的值保存到 `table`，后面的判断或输出会继续使用它。
    table = optional + optional_size
    # 创建变量 `sections`（类型提示为 `list[dict[str, int | str]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    sections: list[dict[str, int | str]] = []
    # 开始循环 `index in range(section_count)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for index in range(section_count):
        # 把右侧 `table + index * 40` 计算得到的值保存到 `offset`，后面的判断或输出会继续使用它。
        offset = table + index * 40
        # 把右侧 `data[offset : offset + 8].split(b"\0", 1)[0].decode("ascii", errors="repla…` 计算得到的值保存到 `name`，后面的判断或输出会继续使用它。
        name = data[offset : offset + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
        # 把这一条新结果追加到列表 `sections`；不会覆盖前面已经收集的记录。
        sections.append(
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {
                # 给当前结果字典的“name”字段填写 `name,`，这样导出的 JSON/表格能保留这一项证据。
                "name": name,
                # 给当前结果字典的“virtual_size”字段填写 `u32(data, offset + 8),`，这样导出的 JSON/表格能保留这一项证据。
                "virtual_size": u32(data, offset + 8),
                # 给当前结果字典的“rva”字段填写 `u32(data, offset + 12),`，这样导出的 JSON/表格能保留这一项证据。
                "rva": u32(data, offset + 12),
                # 给当前结果字典的“raw_size”字段填写 `u32(data, offset + 16),`，这样导出的 JSON/表格能保留这一项证据。
                "raw_size": u32(data, offset + 16),
                # 给当前结果字典的“raw_offset”字段填写 `u32(data, offset + 20),`，这样导出的 JSON/表格能保留这一项证据。
                "raw_offset": u32(data, offset + 20),
            }
        )
    # 把 `image_base, sections` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return image_base, sections


# 定义函数 `va_to_offset`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def va_to_offset(va: int, image_base: int, sections: list[dict[str, int | str]]) -> int:
    # 把右侧 `va - image_base` 计算得到的值保存到 `rva`，后面的判断或输出会继续使用它。
    rva = va - image_base
    # 开始循环 `section in sections`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for section in sections:
        # 把右侧 `int(section["rva"])` 计算得到的值保存到 `start`，后面的判断或输出会继续使用它。
        start = int(section["rva"])
        # 把右侧 `max(int(section["virtual_size"]), int(section["raw_size"]))` 计算得到的值保存到 `span`，后面的判断或输出会继续使用它。
        span = max(int(section["virtual_size"]), int(section["raw_size"]))
        # 检查条件 `start <= rva < start + span`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if start <= rva < start + span:
            # 把 `int(section["raw_offset"]) + (rva - start)` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
            return int(section["raw_offset"]) + (rva - start)
    # 检测到不能继续的情况，主动抛出异常 `ValueError(f"VA 0x{va:08X}不在任何节内")`，让上层明确知道数据或参数不符合要求。
    raise ValueError(f"VA 0x{va:08X}不在任何节内")


# 定义函数 `verify_rpg_loader`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_rpg_loader(path: Path) -> dict[str, Any]:
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `data`，后面的判断或输出会继续使用它。
    data = path.read_bytes()
    # 执行一次赋值：把 `parse_pe_sections(data)` 的结果放入左侧 `image_base, sections` 对应的变量/字段。
    image_base, sections = parse_pe_sections(data)
    # 创建变量 `checks`（类型提示为 `list[dict[str, Any]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    checks: list[dict[str, Any]] = []

    # 定义函数 `add_bytes`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def add_bytes(va: int, expected_hex: str, meaning: str) -> None:
        # 把右侧 `bytes.fromhex(expected_hex)` 计算得到的值保存到 `expected`，后面的判断或输出会继续使用它。
        expected = bytes.fromhex(expected_hex)
        # 把右侧 `va_to_offset(va, image_base, sections)` 计算得到的值保存到 `offset`，后面的判断或输出会继续使用它。
        offset = va_to_offset(va, image_base, sections)
        # 把右侧 `data[offset : offset + len(expected)]` 计算得到的值保存到 `actual`，后面的判断或输出会继续使用它。
        actual = data[offset : offset + len(expected)]
        # 把这一条新结果追加到列表 `checks`；不会覆盖前面已经收集的记录。
        checks.append(
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {
                # 给当前结果字典的“类型”字段填写 `"关键字节",`，这样导出的 JSON/表格能保留这一项证据。
                "类型": "关键字节",
                # 给当前结果字典的“地址”字段填写 `f"0x{va:08X}",`，这样导出的 JSON/表格能保留这一项证据。
                "地址": f"0x{va:08X}",
                # 给当前结果字典的“含义”字段填写 `meaning,`，这样导出的 JSON/表格能保留这一项证据。
                "含义": meaning,
                # 给当前结果字典的“期望”字段填写 `expected.hex(" "),`，这样导出的 JSON/表格能保留这一项证据。
                "期望": expected.hex(" "),
                # 给当前结果字典的“实际”字段填写 `actual.hex(" "),`，这样导出的 JSON/表格能保留这一项证据。
                "实际": actual.hex(" "),
                # 给当前结果字典的“status”字段填写 `"PASS" if actual == expected else "FAIL",`，这样导出的 JSON/表格能保留这一项证据。
                "status": "PASS" if actual == expected else "FAIL",
            }
        )

    # 定义函数 `add_call`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def add_call(va: int, target: int, meaning: str) -> None:
        # 把右侧 `va_to_offset(va, image_base, sections)` 计算得到的值保存到 `offset`，后面的判断或输出会继续使用它。
        offset = va_to_offset(va, image_base, sections)
        # 把右侧 `None` 计算得到的值保存到 `actual_target`，后面的判断或输出会继续使用它。
        actual_target = None
        # 检查条件 `data[offset] == 0xE8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if data[offset] == 0xE8:
            # 把右侧 `struct.unpack_from("<i", data, offset + 1)[0]` 计算得到的值保存到 `displacement`，后面的判断或输出会继续使用它。
            displacement = struct.unpack_from("<i", data, offset + 1)[0]
            # 把右侧 `va + 5 + displacement` 计算得到的值保存到 `actual_target`，后面的判断或输出会继续使用它。
            actual_target = va + 5 + displacement
        # 把这一条新结果追加到列表 `checks`；不会覆盖前面已经收集的记录。
        checks.append(
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {
                # 给当前结果字典的“类型”字段填写 `"直接调用",`，这样导出的 JSON/表格能保留这一项证据。
                "类型": "直接调用",
                # 给当前结果字典的“地址”字段填写 `f"0x{va:08X}",`，这样导出的 JSON/表格能保留这一项证据。
                "地址": f"0x{va:08X}",
                # 给当前结果字典的“含义”字段填写 `meaning,`，这样导出的 JSON/表格能保留这一项证据。
                "含义": meaning,
                # 给当前结果字典的“期望目标”字段填写 `f"0x{target:08X}",`，这样导出的 JSON/表格能保留这一项证据。
                "期望目标": f"0x{target:08X}",
                # 给当前结果字典的“实际目标”字段填写 `f"0x{actual_target:08X}" if actual_target is not None else None,`，这样导出的 JSON/表格能保留这一项证据。
                "实际目标": f"0x{actual_target:08X}" if actual_target is not None else None,
                # 给当前结果字典的“status”字段填写 `"PASS" if actual_target == target else "FAIL",`，这样导出的 JSON/表格能保留这一项证据。
                "status": "PASS" if actual_target == target else "FAIL",
            }
        )

    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004027D0, "cc 25 40 00 27 26 40 00 cf 26 40 00 4b 27 40 00", "0x4025B0按mode 0/1/2/3分派的四项跳表")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004025BA, "83 f8 03 56 57 0f 87 fc 00 00 00 ff 24 85 d0 27 40 00", "Loader只接受0..3并按跳表分派")
    # 调用 `add_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_call(0x004025DC, 0x004027E0, "mode0先走直接文件读取")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004026B1, "6a 01 56 68 c8 f3 46 00 e8 92 01 00 00", "mode1失败回退调用内部组1（Map容器）")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x00402700, "6a 02 57 56 e8 47 01 00 00", "mode2调用内部组2（SYS容器）")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x00402780, "6a 03 57 56 e8 c7 00 00 00", "mode3调用内部组3（Music容器）")
    # 调用 `add_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_call(0x004026B9, 0x00402850, "mode1进入容器读取器")
    # 调用 `add_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_call(0x00402704, 0x00402850, "mode2进入容器读取器")
    # 调用 `add_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_call(0x00402784, 0x00402850, "mode3进入容器读取器")

    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x00402382, "68 ac 82 46 00 e8 64 92 04 00", "SYS容器使用固定路径字符串并打开")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004023AD, "8d 44 24 0c 6a 0a 50 51 e8 16 9c 05 00", "SYS初始化先读取10字节头")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004023BA, "8b 4c 24 1e b8 a5 41 1a a4 f7 e1 2b ca d1 e9 03 ca c1 e9 05 49", "SYS以header+6除39再减1，得到跳过根记录后的扫描项数")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004023D5, "8d 14 89 c1 e2 03 2b d1", "SYS按count乘39计算索引缓存字节数")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004023E8, "a1 d0 f5 46 00 6a 00 6a 31 50 e8 d5 64 05 00", "SYS从绝对文件偏移49开始读取根记录后的索引")

    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x00402442, "68 78 82 46 00 e8 a4 91 04 00", "Music容器使用固定路径字符串并打开")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x00402467, "8b 15 d4 f5 46 00 8d 4c 24 0c 6a 0a 51 52", "Music初始化同样读取10字节头")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x0040247A, "8b 4c 24 1e b8 a5 41 1a a4 f7 e1 2b ca d1 e9 03 ca c1 e9 05 49", "Music使用同一header+6/39-1公式")

    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004021D6, "56 68 4c 82 46 00 68 c8 f3 46 00", "Map容器路径由场景名拼接到固定Map目录")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x0040226B, "8d 54 24 20 6a 0a 52 50 e8 58 9d 05 00", "Map容器初始化读取10字节头")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x00402278, "8b 4c 24 32 b8 a5 41 1a a4 f7 e1 2b ca d1 e9 03 ca c1 e9 05 49", "Map容器使用同一header+6/39-1公式")

    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x00402866, "a1 e4 f5 46 00 8b 0d f0 f5 46 00", "内部组3绑定Music的记录数与39字节索引缓存")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004028D7, "8b 15 e0 f5 46 00 a1 ec f5 46 00", "内部组2绑定SYS的记录数与39字节索引缓存")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x00402942, "8b 0d e8 f5 46 00 8b 15 f4 f5 46 00", "内部组1绑定当前Map的记录数与39字节索引缓存")

    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004029D6, "8d 47 0a 50 55 e8 50 a3 05 00", "记录查找把record+10名称与请求名比较")
    # 调用 `add_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_call(0x004029DB, 0x0045CD30, "记录名称进入ASCII不区分大小写比较函数")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004029E7, "46 83 c7 27 3b f3 7c e7", "未命中时record指针每次增加39字节")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x004029F6, "8b 54 24 18 8b 4f 02 89 0a 8b 47 23 8b 4c 24 1c", "命中后分别取record+2大小与record+35数据偏移")
    # 调用 `add_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    add_bytes(0x0045CD5A, "2c 41 3c 1a 1a c9 80 e1 20 02 c1 04 41", "比较函数对ASCII A..Z折叠大小写")

    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `VERSION,`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本": VERSION,
        # 给当前结果字典的“文件”字段填写 `str(path),`，这样导出的 JSON/表格能保留这一项证据。
        "文件": str(path),
        # 给当前结果字典的“SHA256”字段填写 `sha256_bytes(data),`，这样导出的 JSON/表格能保留这一项证据。
        "SHA256": sha256_bytes(data),
        # 给当前结果字典的“Oracle_SHA256”字段填写 `ORACLE_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        "Oracle_SHA256": ORACLE_SHA256,
        # 给当前结果字典的“Oracle哈希匹配”字段填写 `sha256_bytes(data) == ORACLE_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        "Oracle哈希匹配": sha256_bytes(data) == ORACLE_SHA256,
        # 给当前结果字典的“关键验证”字段填写 `checks,`，这样导出的 JSON/表格能保留这一项证据。
        "关键验证": checks,
        # 给当前结果字典的“关键验证数量”字段填写 `len(checks),`，这样导出的 JSON/表格能保留这一项证据。
        "关键验证数量": len(checks),
        # 给当前结果字典的“关键验证全部通过”字段填写 `all(item["status"] == "PASS" for item in checks),`，这样导出的 JSON/表格能保留这一项证据。
        "关键验证全部通过": all(item["status"] == "PASS" for item in checks),
        # 给当前结果字典的“静态闭合”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "静态闭合": {
            # 给当前结果字典的“模式0”字段填写 `"直接文件路径读取与路径回退；不使用已载入DAT索引。",`，这样导出的 JSON/表格能保留这一项证据。
            "模式0": "直接文件路径读取与路径回退；不使用已载入DAT索引。",
            # 给当前结果字典的“模式1”字段填写 `"当前Map DAT容器。",`，这样导出的 JSON/表格能保留这一项证据。
            "模式1": "当前Map DAT容器。",
            # 给当前结果字典的“模式2”字段填写 `"SYS.DAT容器。",`，这样导出的 JSON/表格能保留这一项证据。
            "模式2": "SYS.DAT容器。",
            # 给当前结果字典的“模式3”字段填写 `"Music.DAT容器。",`，这样导出的 JSON/表格能保留这一项证据。
            "模式3": "Music.DAT容器。",
            # 给当前结果字典的“索引扫描”字段填写 `"初始化读取10字节头，以header+6/39-1分配并从文件偏移49缓存根记录后的39字节项；查找时名称位于+10、stride=39…`，这样导出的 JSON/表格能保留这一项证据。
            "索引扫描": "初始化读取10字节头，以header+6/39-1分配并从文件偏移49缓存根记录后的39字节项；查找时名称位于+10、stride=39、命中后返回+2大小和+35绝对数据偏移。",
            # 给当前结果字典的“名称比较”字段填写 `"ASCII不区分大小写。",`，这样导出的 JSON/表格能保留这一项证据。
            "名称比较": "ASCII不区分大小写。",
        },
        # 给当前结果字典的“证据边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "证据边界": [
            # 继续填写当前数据结构或参数列表中的一项：`"RPG.exe直接Loader没有消费头部+0的u32和+4的u16；当前只能记录九个真实容器均为1/1，不能擅自命名。",`。
            "RPG.exe直接Loader没有消费头部+0的u32和+4的u16；当前只能记录九个真实容器均为1/1，不能擅自命名。",
            # 继续填写当前数据结构或参数列表中的一项：`"Loader的快速文件查找从偏移49开始做扁平名称扫描；目录树递归关系由真实39字节记录的type/count/child-offset结构和九容器交叉不变量支持，但这条快速路径本身不会…`。
            "Loader的快速文件查找从偏移49开始做扁平名称扫描；目录树递归关系由真实39字节记录的type/count/child-offset结构和九容器交叉不变量支持，但这条快速路径本身不会递归遍历。",
            # 继续填写当前数据结构或参数列表中的一项：`"当前样本名称均为ASCII；不能据此断言格式永远禁止其他编码。",`。
            "当前样本名称均为ASCII；不能据此断言格式永远禁止其他编码。",
        ],
    }


# 定义函数 `write_json`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def write_json(path: Path | None, report: dict[str, Any]) -> None:
    # 把右侧 `json.dumps(report, ensure_ascii=False, indent=2)` 计算得到的值保存到 `text`，后面的判断或输出会继续使用它。
    text = json.dumps(report, ensure_ascii=False, indent=2)
    # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
    print(text)
    # 检查条件 `path is not None`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if path is not None:
        # 执行一次赋值：把 `True, exist_ok=True)` 的结果放入左侧 `path.parent.mkdir(parents` 对应的变量/字段。
        path.parent.mkdir(parents=True, exist_ok=True)
        # 执行一次赋值：把 `"utf-8")` 的结果放入左侧 `path.write_text(text + "\n", encoding` 对应的变量/字段。
        path.write_text(text + "\n", encoding="utf-8")
        # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
        print(f"\n已写入：{path}")


# 定义函数 `extract_container`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def extract_container(path: Path, output: Path, allow_overwrite: bool) -> dict[str, Any]:
    # 把右侧 `parse_container(path, include_records=False)` 计算得到的值保存到 `report`，后面的判断或输出会继续使用它。
    report = parse_container(path, include_records=False)
    # 检查条件 `not report.get("识别为资源容器") or not report.get("结构全部通过")`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not report.get("识别为资源容器") or not report.get("结构全部通过"):
        # 检测到不能继续的情况，主动抛出异常 `ValueError("输入未通过资源容器结构检查，拒绝提取")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("输入未通过资源容器结构检查，拒绝提取")
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `data`，后面的判断或输出会继续使用它。
    data = path.read_bytes()
    # 创建变量 `written`（类型提示为 `list[dict[str, Any]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    written: list[dict[str, Any]] = []
    # 开始循环 `item in report["内嵌文件"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for item in report["内嵌文件"]:
        # 把右侧 `item["容器内路径"].split("/")` 计算得到的值保存到 `parts`，后面的判断或输出会继续使用它。
        parts = item["容器内路径"].split("/")
        # 检查条件 `not parts or not all(safe_component(part) for part in parts)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not parts or not all(safe_component(part) for part in parts):
            # 检测到不能继续的情况，主动抛出异常 `ValueError(f"不安全的容器内路径：{item['容器内路径']!r}")`，让上层明确知道数据或参数不符合要求。
            raise ValueError(f"不安全的容器内路径：{item['容器内路径']!r}")
        # 把右侧 `output.joinpath(*parts)` 计算得到的值保存到 `target`，后面的判断或输出会继续使用它。
        target = output.joinpath(*parts)
        # 检查条件 `target.exists() and not allow_overwrite`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if target.exists() and not allow_overwrite:
            # 检测到不能继续的情况，主动抛出异常 `FileExistsError(f"目标已存在；未指定--允许覆盖：{target}")`，让上层明确知道数据或参数不符合要求。
            raise FileExistsError(f"目标已存在；未指定--允许覆盖：{target}")
        # 执行一次赋值：把 `True, exist_ok=True)` 的结果放入左侧 `target.parent.mkdir(parents` 对应的变量/字段。
        target.parent.mkdir(parents=True, exist_ok=True)
        # 把右侧 `item["数据偏移"]` 计算得到的值保存到 `start`，后面的判断或输出会继续使用它。
        start = item["数据偏移"]
        # 把右侧 `data[start : start + item["字节数"]]` 计算得到的值保存到 `payload`，后面的判断或输出会继续使用它。
        payload = data[start : start + item["字节数"]]
        # 把二进制结果原样写入文件；这里不会把字节转换成字符串。
        target.write_bytes(payload)
        # 把这一条新结果追加到列表 `written`；不会覆盖前面已经收集的记录。
        written.append(
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {
                # 给当前结果字典的“容器内路径”字段填写 `item["容器内路径"],`，这样导出的 JSON/表格能保留这一项证据。
                "容器内路径": item["容器内路径"],
                # 给当前结果字典的“输出”字段填写 `str(target),`，这样导出的 JSON/表格能保留这一项证据。
                "输出": str(target),
                # 给当前结果字典的“字节数”字段填写 `len(payload),`，这样导出的 JSON/表格能保留这一项证据。
                "字节数": len(payload),
                # 给当前结果字典的“SHA256”字段填写 `sha256_bytes(payload),`，这样导出的 JSON/表格能保留这一项证据。
                "SHA256": sha256_bytes(payload),
                # 给当前结果字典的“与目录记录一致”字段填写 `len(payload) == item["字节数"] and sha256_bytes(payload) == item["SHA256…`，这样导出的 JSON/表格能保留这一项证据。
                "与目录记录一致": len(payload) == item["字节数"] and sha256_bytes(payload) == item["SHA256"],
            }
        )
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `VERSION,`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本": VERSION,
        # 给当前结果字典的“输入”字段填写 `str(path),`，这样导出的 JSON/表格能保留这一项证据。
        "输入": str(path),
        # 给当前结果字典的“输出目录”字段填写 `str(output),`，这样导出的 JSON/表格能保留这一项证据。
        "输出目录": str(output),
        # 给当前结果字典的“允许覆盖”字段填写 `allow_overwrite,`，这样导出的 JSON/表格能保留这一项证据。
        "允许覆盖": allow_overwrite,
        # 给当前结果字典的“提取文件数”字段填写 `len(written),`，这样导出的 JSON/表格能保留这一项证据。
        "提取文件数": len(written),
        # 给当前结果字典的“全部字节与目录记录一致”字段填写 `all(item["与目录记录一致"] for item in written),`，这样导出的 JSON/表格能保留这一项证据。
        "全部字节与目录记录一致": all(item["与目录记录一致"] for item in written),
        # 给当前结果字典的“逐文件”字段填写 `written,`，这样导出的 JSON/表格能保留这一项证据。
        "逐文件": written,
    }


# 定义函数 `main`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def main() -> int:
    # 把右侧 `argparse.ArgumentParser(description="《幽城幻剑录》DAT/SYS资源容器调查器")` 计算得到的值保存到 `parser`，后面的判断或输出会继续使用它。
    parser = argparse.ArgumentParser(description="《幽城幻剑录》DAT/SYS资源容器调查器")
    # 向命令行解析器登记一个参数规则，告诉工具这个参数的名字、类型以及是否必填。
    parser.add_argument("--版本", action="version", version=f"幽城资源容器调查器 {VERSION}")
    # 把右侧 `parser.add_subparsers(dest="command", required=True)` 计算得到的值保存到 `sub`，后面的判断或输出会继续使用它。
    sub = parser.add_subparsers(dest="command", required=True)

    # 把右侧 `sub.add_parser("检查", help="检查一个DAT/SYS资源容器")` 计算得到的值保存到 `one`，后面的判断或输出会继续使用它。
    one = sub.add_parser("检查", help="检查一个DAT/SYS资源容器")
    # 调用 `one.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    one.add_argument("文件")
    # 调用 `one.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    one.add_argument("--json")

    # 把右侧 `sub.add_parser("批量", help="递归扫描目录下全部.dat并区分资源容器与其他DAT")` 计算得到的值保存到 `many`，后面的判断或输出会继续使用它。
    many = sub.add_parser("批量", help="递归扫描目录下全部.dat并区分资源容器与其他DAT")
    # 调用 `many.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    many.add_argument("根目录")
    # 调用 `many.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    many.add_argument("--json")

    # 把右侧 `sub.add_parser("验证RPG加载协议", help="对同版RPG.exe重放资源Loader静态断言")` 计算得到的值保存到 `replay`，后面的判断或输出会继续使用它。
    replay = sub.add_parser("验证RPG加载协议", help="对同版RPG.exe重放资源Loader静态断言")
    # 调用 `replay.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    replay.add_argument("exe")
    # 调用 `replay.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    replay.add_argument("--json")

    # 把右侧 `sub.add_parser("提取", help="显式提取一个已通过检查的容器；默认拒绝覆盖")` 计算得到的值保存到 `extract`，后面的判断或输出会继续使用它。
    extract = sub.add_parser("提取", help="显式提取一个已通过检查的容器；默认拒绝覆盖")
    # 调用 `extract.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    extract.add_argument("文件")
    # 调用 `extract.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    extract.add_argument("输出目录")
    # 执行一次赋值：把 `"store_true")` 的结果放入左侧 `extract.add_argument("--允许覆盖", action` 对应的变量/字段。
    extract.add_argument("--允许覆盖", action="store_true")
    # 调用 `extract.add_argument` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    extract.add_argument("--json")

    # 把右侧 `parser.parse_args()` 计算得到的值保存到 `args`，后面的判断或输出会继续使用它。
    args = parser.parse_args()
    # 检查条件 `args.command == "检查"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if args.command == "检查":
        # 把右侧 `parse_container(Path(args.文件).resolve())` 计算得到的值保存到 `report`，后面的判断或输出会继续使用它。
        report = parse_container(Path(args.文件).resolve())
    # 前一个条件没有成立时，再检查 `args.command == "批量"`；成立才执行这一分支。
    elif args.command == "批量":
        # 把右侧 `batch_report(Path(args.根目录).resolve())` 计算得到的值保存到 `report`，后面的判断或输出会继续使用它。
        report = batch_report(Path(args.根目录).resolve())
    # 前一个条件没有成立时，再检查 `args.command == "验证RPG加载协议"`；成立才执行这一分支。
    elif args.command == "验证RPG加载协议":
        # 把右侧 `verify_rpg_loader(Path(args.exe).resolve())` 计算得到的值保存到 `report`，后面的判断或输出会继续使用它。
        report = verify_rpg_loader(Path(args.exe).resolve())
    # 前面的条件分支都没有命中时，执行这个兜底分支。
    else:
        # 把右侧 `extract_container(` 计算得到的值保存到 `report`，后面的判断或输出会继续使用它。
        report = extract_container(
            # 调用 `Path` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            Path(args.文件).resolve(),
            # 调用 `Path` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            Path(args.输出目录).resolve(),
            # 继续填写当前数据结构或参数列表中的一项：`args.允许覆盖,`。
            args.允许覆盖,
        )
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    write_json(Path(args.json).resolve() if args.json else None, report)
    # 把 `0` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return 0


# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__ == "__main__":
    # 检测到不能继续的情况，主动抛出异常 `SystemExit(main())`，让上层明确知道数据或参数不符合要求。
    raise SystemExit(main())
