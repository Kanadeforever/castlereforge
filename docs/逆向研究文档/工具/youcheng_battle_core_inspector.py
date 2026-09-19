#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""幽城战斗核心调查器 0.7D.12-固化22

只读工具：
1. 对 Public ENC 做 period-255 解码、Battle 表几何校验与记录导出；
2. 对当前 Oracle RPG.exe 做 SHA256、PE 映射、关键调用与关键指令字节验证；
3. 扫描 x86 E8 rel32 直接调用引用，辅助重新建立 caller/callee 证据。

本工具不修改游戏文件，也不把业务语义猜测写成验证事实。
"""
# 启用较新的 Python 注解语法行为，让类型注解先按文字保存，避免旧版本在定义阶段立即求值。
from __future__ import annotations

# 导入 `argparse` 模块；这里只取得标准库功能，不会修改游戏文件。
import argparse
# 导入 `hashlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import hashlib
# 导入 `json` 模块；这里只取得标准库功能，不会修改游戏文件。
import json
# 导入 `struct` 模块；这里只取得标准库功能，不会修改游戏文件。
import struct
# 导入 `zipfile` 模块；这里只取得标准库功能，不会修改游戏文件。
import zipfile
# 从 `collections` 模块导入 `Counter`，后面的代码会直接使用这些现成组件。
from collections import Counter
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path
# 从 `typing` 模块导入 `Any`，后面的代码会直接使用这些现成组件。
from typing import Any

# 把右侧 `'0.7D.12-固化22'` 计算得到的值保存到 `VER`，后面的判断或输出会继续使用它。
VER = '0.7D.62-固化62'
# 把右侧 `'b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf'` 计算得到的值保存到 `ORACLE_SHA256`，后面的判断或输出会继续使用它。
ORACLE_SHA256 = 'b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf'

# 把右侧 `{` 计算得到的值保存到 `TABLES`，后面的判断或输出会继续使用它。
TABLES = {
    # 给当前结果字典的“api”字段填写 `('Api.enc', 848, 255),`，这样导出的 JSON/表格能保留这一项证据。
    'api': ('Api.enc', 848, 255),
    # 给当前结果字典的“article”字段填写 `('Ail2.ENC', 926, 700),`，这样导出的 JSON/表格能保留这一项证据。
    'article': ('Ail2.ENC', 926, 700),
    # 给当前结果字典的“enemy_ai”字段填写 `('Enemy_ai.enc', 56, 283),`，这样导出的 JSON/表格能保留这一项证据。
    'enemy_ai': ('Enemy_ai.enc', 56, 283),
    # 给当前结果字典的“magiccon”字段填写 `('Magiccon.enc', 64, 500),`，这样导出的 JSON/表格能保留这一项证据。
    'magiccon': ('Magiccon.enc', 64, 500),
    # 给当前结果字典的“firttech”字段填写 `('Firttech.enc', 560, 500),`，这样导出的 JSON/表格能保留这一项证据。
    'firttech': ('Firttech.enc', 560, 500),
    # 给当前结果字典的“battlegb”字段填写 `('Battlegb.enc', 94, 66),`，这样导出的 JSON/表格能保留这一项证据。
    'battlegb': ('Battlegb.enc', 94, 66),
    # 给当前结果字典的“layoutgr”字段填写 `('Layoutgr.enc', 508, 250),`，这样导出的 JSON/表格能保留这一项证据。
    'layoutgr': ('Layoutgr.enc', 508, 250),
    # 给当前结果字典的“layoutteam”字段填写 `('LayoutTeam.ENC', 508, 334),`，这样导出的 JSON/表格能保留这一项证据。
    'layoutteam': ('LayoutTeam.ENC', 508, 334),
}


# 定义函数 `dec`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def dec(data: bytes) -> bytes:
    # 把 `bytes(value ^ (index % 255) for index, value in enumerate(data))` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return bytes(value ^ (index % 255) for index, value in enumerate(data))


# 定义函数 `find_file`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def find_file(root: str | Path, name: str) -> Path:
    # 把右侧 `Path(root)` 计算得到的值保存到 `root_path`，后面的判断或输出会继续使用它。
    root_path = Path(root)
    # 开始循环 `path in root_path.iterdir()`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for path in root_path.iterdir():
        # 检查条件 `path.name.lower() == name.lower()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if path.name.lower() == name.lower():
            # 把 `path` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
            return path
    # 检测到不能继续的情况，主动抛出异常 `FileNotFoundError(name)`，让上层明确知道数据或参数不符合要求。
    raise FileNotFoundError(name)


# 定义函数 `dump_json`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def dump_json(value: Any, output: str | None = None) -> None:
    # 把右侧 `json.dumps(value, ensure_ascii=False, indent=2)` 计算得到的值保存到 `text`，后面的判断或输出会继续使用它。
    text = json.dumps(value, ensure_ascii=False, indent=2)
    # 检查条件 `output`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if output:
        # 执行一次赋值：把 `'utf-8')` 的结果放入左侧 `Path(output).write_text(text + '\n', encoding` 对应的变量/字段。
        Path(output).write_text(text + '\n', encoding='utf-8')
    # 前面的条件分支都没有命中时，执行这个兜底分支。
    else:
        # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
        print(text)


# 定义函数 `load_table`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def load_table(root: str | Path, key: str):
    # 执行一次赋值：把 `TABLES[key]` 的结果放入左侧 `name, record_size, count` 对应的变量/字段。
    name, record_size, count = TABLES[key]
    # 把右侧 `find_file(root, name)` 计算得到的值保存到 `path`，后面的判断或输出会继续使用它。
    path = find_file(root, name)
    # 把右侧 `dec(path.read_bytes())` 计算得到的值保存到 `data`，后面的判断或输出会继续使用它。
    data = dec(path.read_bytes())
    # 把右侧 `record_size * count` 计算得到的值保存到 `expected`，后面的判断或输出会继续使用它。
    expected = record_size * count
    # 检查条件 `len(data) != expected`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(data) != expected:
        # 检测到不能继续的情况，主动抛出异常 `ValueError(f'{name} 几何不匹配：{len(data)} != {record_size}*{count}={expected}')`，让上层明确知道数据或参数不符合要求。
        raise ValueError(f'{name} 几何不匹配：{len(data)} != {record_size}*{count}={expected}')
    # 把 `path, data, record_size, count` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return path, data, record_size, count


# 定义函数 `cmd_verify_enc`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_verify_enc(args):
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows = []
    # 开始循环 `key, (name, record_size, count) in TABLES.items()`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for key, (name, record_size, count) in TABLES.items():
        # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
        try:
            # 执行一次赋值：把 `load_table(args.dir, key)` 的结果放入左侧 `path, data, record_size, count` 对应的变量/字段。
            path, data, record_size, count = load_table(args.dir, key)
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            rows.append({
                # 给当前结果字典的“key”字段填写 `key,`，这样导出的 JSON/表格能保留这一项证据。
                'key': key,
                # 给当前结果字典的“file”字段填写 `path.name,`，这样导出的 JSON/表格能保留这一项证据。
                'file': path.name,
                # 给当前结果字典的“record_size”字段填写 `record_size,`，这样导出的 JSON/表格能保留这一项证据。
                'record_size': record_size,
                # 给当前结果字典的“count”字段填写 `count,`，这样导出的 JSON/表格能保留这一项证据。
                'count': count,
                # 给当前结果字典的“decoded_sha256”字段填写 `hashlib.sha256(data).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
                'decoded_sha256': hashlib.sha256(data).hexdigest(),
                # 给当前结果字典的“status”字段填写 `'PASS',`，这样导出的 JSON/表格能保留这一项证据。
                'status': 'PASS',
            })
        # 捕获 `Exception as exc` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
        except Exception as exc:
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            rows.append({'key': key, 'file': name, 'status': 'FAIL', 'error': str(exc)})
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json({'工具版本': VER, '结果': rows}, args.json)


# 定义函数 `cmd_record`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_record(args):
    # 执行一次赋值：把 `load_table(args.dir, args.table)` 的结果放入左侧 `_, data, record_size, count` 对应的变量/字段。
    _, data, record_size, count = load_table(args.dir, args.table)
    # 把右侧 `args.index` 计算得到的值保存到 `index`，后面的判断或输出会继续使用它。
    index = args.index
    # 检查条件 `not 0 <= index < count`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not 0 <= index < count:
        # 检测到不能继续的情况，主动抛出异常 `SystemExit(f'索引越界：{index}，合法范围 0..{count - 1}')`，让上层明确知道数据或参数不符合要求。
        raise SystemExit(f'索引越界：{index}，合法范围 0..{count - 1}')
    # 把右侧 `data[index * record_size:(index + 1) * record_size]` 计算得到的值保存到 `record`，后面的判断或输出会继续使用它。
    record = data[index * record_size:(index + 1) * record_size]
    # 把右侧 `[struct.unpack_from('<I', record, offset)[0]` 计算得到的值保存到 `values`，后面的判断或输出会继续使用它。
    values = [struct.unpack_from('<I', record, offset)[0]
              # 开始循环 `offset in range(0, min(record_size, 256) - 3, 4)]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
              for offset in range(0, min(record_size, 256) - 3, 4)]
    # 把右侧 `{` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out = {
        # 给当前结果字典的“table”字段填写 `args.table,`，这样导出的 JSON/表格能保留这一项证据。
        'table': args.table,
        # 给当前结果字典的“index”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
        'index': index,
        # 给当前结果字典的“record_size”字段填写 `record_size,`，这样导出的 JSON/表格能保留这一项证据。
        'record_size': record_size,
        # 给当前结果字典的“sha256”字段填写 `hashlib.sha256(record).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
        'sha256': hashlib.sha256(record).hexdigest(),
        # 给当前结果字典的“hex”字段填写 `record.hex(),`，这样导出的 JSON/表格能保留这一项证据。
        'hex': record.hex(),
        # 给当前结果字典的“u32_first_256”字段填写 `values,`，这样导出的 JSON/表格能保留这一项证据。
        'u32_first_256': values,
    }
    # 检查条件 `args.table == 'api'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if args.table == 'api':
        # 把右侧 `record[0x0C:0x20].split(b'\0', 1)[0]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw = record[0x0C:0x20].split(b'\0', 1)[0]
        # 把右侧 `raw.decode('cp950', 'replace')` 计算得到的值保存到 `out['role_name']`，后面的判断或输出会继续使用它。
        out['role_name'] = raw.decode('cp950', 'replace')
    # 检查条件 `args.table == 'enemy_ai'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if args.table == 'enemy_ai':
        # 把右侧 `list(struct.unpack('<14i', record))` 计算得到的值保存到 `out['int32_14']`，后面的判断或输出会继续使用它。
        out['int32_14'] = list(struct.unpack('<14i', record))
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json(out, args.json)


# 定义函数 `load_ail2_source`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def load_ail2_source(source: str | Path) -> tuple[str, bytes]:
    # 执行这一条实际代码步骤：`"""从 public 目录或 multimedia.zip 读取并解码 Ail2.ENC。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """从 public 目录或 multimedia.zip 读取并解码 Ail2.ENC。"""
    # 把右侧 `Path(source)` 计算得到的值保存到 `src`，后面的判断或输出会继续使用它。
    src = Path(source)
    # 检查条件 `src.is_file() and src.suffix.lower() == '.zip'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if src.is_file() and src.suffix.lower() == '.zip':
        # 进入受管理的资源作用域 `zipfile.ZipFile(src) as zf`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
        with zipfile.ZipFile(src) as zf:
            # 把右侧 `[n for n in zf.namelist() if n.lower().endswith('/ail2.enc') or n.lower() …` 计算得到的值保存到 `names`，后面的判断或输出会继续使用它。
            names = [n for n in zf.namelist() if n.lower().endswith('/ail2.enc') or n.lower() == 'ail2.enc']
            # 检查条件 `not names`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not names:
                # 检测到不能继续的情况，主动抛出异常 `FileNotFoundError('ZIP 中未找到 Ail2.ENC')`，让上层明确知道数据或参数不符合要求。
                raise FileNotFoundError('ZIP 中未找到 Ail2.ENC')
            # 检查条件 `len(names) > 1`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if len(names) > 1:
                # Prefer public/Ail2.ENC when present.
                # 执行一次赋值：把 `lambda n: (not n.lower().endswith('/public/ail2.enc'), len(n)))` 的结果放入左侧 `names.sort(key` 对应的变量/字段。
                names.sort(key=lambda n: (not n.lower().endswith('/public/ail2.enc'), len(n)))
            # 把右侧 `names[0]` 计算得到的值保存到 `name`，后面的判断或输出会继续使用它。
            name = names[0]
            # 把右侧 `zf.read(name)` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
            raw = zf.read(name)
            # 把右侧 `f'{src}::{name}'` 计算得到的值保存到 `origin`，后面的判断或输出会继续使用它。
            origin = f'{src}::{name}'
    # 前一个条件没有成立时，再检查 `src.is_dir()`；成立才执行这一分支。
    elif src.is_dir():
        # 把右侧 `find_file(src, 'Ail2.ENC')` 计算得到的值保存到 `path`，后面的判断或输出会继续使用它。
        path = find_file(src, 'Ail2.ENC')
        # 把右侧 `path.read_bytes()` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw = path.read_bytes()
        # 把右侧 `str(path)` 计算得到的值保存到 `origin`，后面的判断或输出会继续使用它。
        origin = str(path)
    # 前面的条件分支都没有命中时，执行这个兜底分支。
    else:
        # 检测到不能继续的情况，主动抛出异常 `FileNotFoundError(f'既不是 multimedia.zip，也不是 public 目录：{source}')`，让上层明确知道数据或参数不符合要求。
        raise FileNotFoundError(f'既不是 multimedia.zip，也不是 public 目录：{source}')
    # 把右侧 `dec(raw)` 计算得到的值保存到 `data`，后面的判断或输出会继续使用它。
    data = dec(raw)
    # 把右侧 `926 * 700` 计算得到的值保存到 `expected`，后面的判断或输出会继续使用它。
    expected = 926 * 700
    # 检查条件 `len(data) != expected`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(data) != expected:
        # 检测到不能继续的情况，主动抛出异常 `ValueError(f'Ail2.ENC 几何不匹配：{len(data)} != 926*700={expected}')`，让上层明确知道数据或参数不符合要求。
        raise ValueError(f'Ail2.ENC 几何不匹配：{len(data)} != 926*700={expected}')
    # 把 `origin, data` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return origin, data


# 定义函数 `cp950_cstr`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cp950_cstr(record: bytes, offset: int, max_len: int) -> str:
    # 把 `record[offset:offset + max_len].split(b'\0', 1)[0].decode('cp950', 'replace')` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return record[offset:offset + max_len].split(b'\0', 1)[0].decode('cp950', 'replace')


# 定义函数 `cmd_analyze_ail2_big_ids`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_analyze_ail2_big_ids(args):
    # 执行一次赋值：把 `load_ail2_source(args.source)` 的结果放入左侧 `origin, data` 对应的变量/字段。
    origin, data = load_ail2_source(args.source)
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows = []
    # 把右侧 `[]` 计算得到的值保存到 `plus20`，后面的判断或输出会继续使用它。
    plus20 = []
    # 开始循环 `index in range(700)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for index in range(700):
        # 把右侧 `data[index * 926:(index + 1) * 926]` 计算得到的值保存到 `record`，后面的判断或输出会继续使用它。
        record = data[index * 926:(index + 1) * 926]
        # 把右侧 `struct.unpack_from('<I', record, 0x20)[0]` 计算得到的值保存到 `item_id`，后面的判断或输出会继续使用它。
        item_id = struct.unpack_from('<I', record, 0x20)[0]
        # 把这一条新结果追加到列表 `plus20`；不会覆盖前面已经收集的记录。
        plus20.append(item_id)
        # 把右侧 `[struct.unpack_from('<i', record, 0xE8 + slot * 4)[0] for slot in range(5)]` 计算得到的值保存到 `values`，后面的判断或输出会继续使用它。
        values = [struct.unpack_from('<i', record, 0xE8 + slot * 4)[0] for slot in range(5)]
        # 把右侧 `[struct.unpack_from('<i', record, 0xFC + slot * 4)[0] for slot in range(5)]` 计算得到的值保存到 `ids`，后面的判断或输出会继续使用它。
        ids = [struct.unpack_from('<i', record, 0xFC + slot * 4)[0] for slot in range(5)]
        # 把右侧 `[{'slot': slot, 'id': eid, 'value': values[slot]}` 计算得到的值保存到 `big_slots`，后面的判断或输出会继续使用它。
        big_slots = [{'slot': slot, 'id': eid, 'value': values[slot]}
                     # 开始循环 `slot, eid in enumerate(ids) if eid > 27]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                     for slot, eid in enumerate(ids) if eid > 27]
        # 检查条件 `not big_slots`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not big_slots:
            # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
            continue
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append({
            # 给当前结果字典的“物理记录索引”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
            '物理记录索引': index,
            # 给当前结果字典的“物理记录索引十六进制”字段填写 `f'0x{index:X}',`，这样导出的 JSON/表格能保留这一项证据。
            '物理记录索引十六进制': f'0x{index:X}',
            # 给当前结果字典的“Ail2加20记录物品ID”字段填写 `item_id,`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加20记录物品ID': item_id,
            # 给当前结果字典的“Ail2加20十六进制”字段填写 `f'0x{item_id:X}',`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加20十六进制': f'0x{item_id:X}',
            # 给当前结果字典的“名称”字段填写 `cp950_cstr(record, 0, 32),`，这样导出的 JSON/表格能保留这一项证据。
            '名称': cp950_cstr(record, 0, 32),
            # 给当前结果字典的“说明”字段填写 `cp950_cstr(record, 0x188, 512),`，这样导出的 JSON/表格能保留这一项证据。
            '说明': cp950_cstr(record, 0x188, 512),
            # 给当前结果字典的“五槽数值_加E8”字段填写 `values,`，这样导出的 JSON/表格能保留这一项证据。
            '五槽数值_加E8': values,
            # 给当前结果字典的“五槽ID_加FC”字段填写 `ids,`，这样导出的 JSON/表格能保留这一项证据。
            '五槽ID_加FC': ids,
            # 给当前结果字典的“大ID槽”字段填写 `big_slots,`，这样导出的 JSON/表格能保留这一项证据。
            '大ID槽': big_slots,
            # 给当前结果字典的“主次资源扣除_加118_加11C”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
            '主次资源扣除_加118_加11C': [
                # 调用 `struct.unpack_from` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
                struct.unpack_from('<i', record, 0x118)[0],
                # 调用 `struct.unpack_from` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
                struct.unpack_from('<i', record, 0x11C)[0],
            ],
        })
    # 把右侧 `[{'物理记录索引': i, 'Ail2加20': v}` 计算得到的值保存到 `mismatches`，后面的判断或输出会继续使用它。
    mismatches = [{'物理记录索引': i, 'Ail2加20': v}
                  # 开始循环 `i, v in enumerate(plus20) if i != v]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                  for i, v in enumerate(plus20) if i != v]
    # 把右侧 `[slot['id'] for row in rows for slot in row['大ID槽']]` 计算得到的值保存到 `big_ids`，后面的判断或输出会继续使用它。
    big_ids = [slot['id'] for row in rows for slot in row['大ID槽']]
    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {
        # 给当前结果字典的“工具版本”字段填写 `VER,`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本': VER,
        # 给当前结果字典的“来源”字段填写 `origin,`，这样导出的 JSON/表格能保留这一项证据。
        '来源': origin,
        # 给当前结果字典的“Ail2解码SHA256”字段填写 `hashlib.sha256(data).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
        'Ail2解码SHA256': hashlib.sha256(data).hexdigest(),
        # 给当前结果字典的“几何”字段填写 `{'record_size': 926, 'count': 700},`，这样导出的 JSON/表格能保留这一项证据。
        '几何': {'record_size': 926, 'count': 700},
        # 给当前结果字典的“Ail2加20字段”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'Ail2加20字段': {
            # 给当前结果字典的“唯一值数量”字段填写 `len(set(plus20)),`，这样导出的 JSON/表格能保留这一项证据。
            '唯一值数量': len(set(plus20)),
            # 给当前结果字典的“范围”字段填写 `[min(plus20), max(plus20)],`，这样导出的 JSON/表格能保留这一项证据。
            '范围': [min(plus20), max(plus20)],
            # 给当前结果字典的“与物理索引相等”字段填写 `700 - len(mismatches),`，这样导出的 JSON/表格能保留这一项证据。
            '与物理索引相等': 700 - len(mismatches),
            # 给当前结果字典的“不相等”字段填写 `mismatches,`，这样导出的 JSON/表格能保留这一项证据。
            '不相等': mismatches,
            # 给当前结果字典的“边界”字段填写 `'证明 +0x20 是唯一记录/物品 ID 字段；不等同于物理记录索引，也不声称作者原始变量名。',`，这样导出的 JSON/表格能保留这一项证据。
            '边界': '证明 +0x20 是唯一记录/物品 ID 字段；不等同于物理记录索引，也不声称作者原始变量名。',
        },
        # 给当前结果字典的“大ID”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '大ID': {
            # 给当前结果字典的“定义”字段填写 `'五槽 ID(+0xFC..+0x10C) signed > 27',`，这样导出的 JSON/表格能保留这一项证据。
            '定义': '五槽 ID(+0xFC..+0x10C) signed > 27',
            # 给当前结果字典的“涉及记录数”字段填写 `len(rows),`，这样导出的 JSON/表格能保留这一项证据。
            '涉及记录数': len(rows),
            # 给当前结果字典的“槽出现次数”字段填写 `len(big_ids),`，这样导出的 JSON/表格能保留这一项证据。
            '槽出现次数': len(big_ids),
            # 给当前结果字典的“频率”字段填写 `dict(sorted(Counter(big_ids).items())),`，这样导出的 JSON/表格能保留这一项证据。
            '频率': dict(sorted(Counter(big_ids).items())),
            # 给当前结果字典的“记录”字段填写 `rows,`，这样导出的 JSON/表格能保留这一项证据。
            '记录': rows,
        },
        # 给当前结果字典的“边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '边界': [
            # 继续填写当前数据结构或参数列表中的一项：`'该命令只从 Ail2 数据证明记录、文本和五槽字段分布，不单独证明 EXE consumer。',`。
            '该命令只从 Ail2 数据证明记录、文本和五槽字段分布，不单独证明 EXE consumer。',
            # 继续填写当前数据结构或参数列表中的一项：`'结合当前 Oracle 的 0x42B0D0/0x422C3D，可证明 positive ID>27 被 normal 28槽 raw-ID pending/runtime 写入通道排除…`。
            '结合当前 Oracle 的 0x42B0D0/0x422C3D，可证明 positive ID>27 被 normal 28槽 raw-ID pending/runtime 写入通道排除。',
            # 继续填写当前数据结构或参数列表中的一项：`'大ID的具体业务效果必须继续用 EXE item-ID 分支、writer 或其他 consumer 证明。',`。
            '大ID的具体业务效果必须继续用 EXE item-ID 分支、writer 或其他 consumer 证明。',
        ],
    }
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json(result, args.json)



# 定义函数 `load_named_enc_source`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def load_named_enc_source(source: str | Path, name: str, record_size: int, count: int) -> tuple[str, bytes]:
    # 执行这一条实际代码步骤：`"""从 public 目录或 multimedia.zip 读取指定 ENC 并按 period-255 解码。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """从 public 目录或 multimedia.zip 读取指定 ENC 并按 period-255 解码。"""
    # 把右侧 `Path(source)` 计算得到的值保存到 `src`，后面的判断或输出会继续使用它。
    src = Path(source)
    # 检查条件 `src.is_file() and src.suffix.lower() == '.zip'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if src.is_file() and src.suffix.lower() == '.zip':
        # 进入受管理的资源作用域 `zipfile.ZipFile(src) as zf`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
        with zipfile.ZipFile(src) as zf:
            # 把右侧 `[n for n in zf.namelist() if n.lower().endswith('/' + name.lower()) or n.l…` 计算得到的值保存到 `names`，后面的判断或输出会继续使用它。
            names = [n for n in zf.namelist() if n.lower().endswith('/' + name.lower()) or n.lower() == name.lower()]
            # 检查条件 `not names`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not names:
                # 检测到不能继续的情况，主动抛出异常 `FileNotFoundError(f'ZIP 中未找到 {name}')`，让上层明确知道数据或参数不符合要求。
                raise FileNotFoundError(f'ZIP 中未找到 {name}')
            # 执行一次赋值：把 `lambda n: (not n.lower().endswith('/public/' + name.lower()), len(n)))` 的结果放入左侧 `names.sort(key` 对应的变量/字段。
            names.sort(key=lambda n: (not n.lower().endswith('/public/' + name.lower()), len(n)))
            # 把右侧 `names[0]` 计算得到的值保存到 `member`，后面的判断或输出会继续使用它。
            member = names[0]
            # 把右侧 `zf.read(member)` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
            raw = zf.read(member)
            # 把右侧 `f'{src}::{member}'` 计算得到的值保存到 `origin`，后面的判断或输出会继续使用它。
            origin = f'{src}::{member}'
    # 前一个条件没有成立时，再检查 `src.is_dir()`；成立才执行这一分支。
    elif src.is_dir():
        # 把右侧 `find_file(src, name)` 计算得到的值保存到 `path`，后面的判断或输出会继续使用它。
        path = find_file(src, name)
        # 把右侧 `path.read_bytes()` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw = path.read_bytes()
        # 把右侧 `str(path)` 计算得到的值保存到 `origin`，后面的判断或输出会继续使用它。
        origin = str(path)
    # 前面的条件分支都没有命中时，执行这个兜底分支。
    else:
        # 检测到不能继续的情况，主动抛出异常 `FileNotFoundError(f'既不是 multimedia.zip，也不是 public 目录：{source}')`，让上层明确知道数据或参数不符合要求。
        raise FileNotFoundError(f'既不是 multimedia.zip，也不是 public 目录：{source}')
    # 把右侧 `dec(raw)` 计算得到的值保存到 `data`，后面的判断或输出会继续使用它。
    data = dec(raw)
    # 把右侧 `record_size * count` 计算得到的值保存到 `expected`，后面的判断或输出会继续使用它。
    expected = record_size * count
    # 检查条件 `len(data) != expected`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(data) != expected:
        # 检测到不能继续的情况，主动抛出异常 `ValueError(f'{name} 几何不匹配：{len(data)} != {record_size}*{count}={expected}')`，让上层明确知道数据或参数不符合要求。
        raise ValueError(f'{name} 几何不匹配：{len(data)} != {record_size}*{count}={expected}')
    # 把 `origin, data` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return origin, data


# 定义函数 `cmd_analyze_d7_rewards`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_analyze_d7_rewards(args):
    # 执行这一条实际代码步骤：`"""重放 D7 胜利奖励所用 LayoutTeam/LevelUp/FIRTTECH 数据证据。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """重放 D7 胜利奖励所用 LayoutTeam/LevelUp/FIRTTECH 数据证据。"""
    # 执行一次赋值：把 `load_named_enc_source(args.source, 'LayoutTeam.ENC', 508, 334)` 的结果放入左侧 `lt_origin, lt` 对应的变量/字段。
    lt_origin, lt = load_named_enc_source(args.source, 'LayoutTeam.ENC', 508, 334)
    # 执行一次赋值：把 `load_named_enc_source(args.source, 'Firttech.enc', 560, 500)` 的结果放入左侧 `ft_origin, ft` 对应的变量/字段。
    ft_origin, ft = load_named_enc_source(args.source, 'Firttech.enc', 560, 500)
    # 执行一次赋值：把 `load_named_enc_source(args.source, 'Levelup.enc', 4, 101)` 的结果放入左侧 `lv_origin, lv` 对应的变量/字段。
    lv_origin, lv = load_named_enc_source(args.source, 'Levelup.enc', 4, 101)

    # 把右侧 `[]` 计算得到的值保存到 `layouts`，后面的判断或输出会继续使用它。
    layouts = []
    # 把右侧 `Counter()` 计算得到的值保存到 `source_count_dist`，后面的判断或输出会继续使用它。
    source_count_dist = Counter()
    # 把右侧 `Counter()` 计算得到的值保存到 `drop_count_dist`，后面的判断或输出会继续使用它。
    drop_count_dist = Counter()
    # 把右侧 `Counter()` 计算得到的值保存到 `active_drop_thresholds`，后面的判断或输出会继续使用它。
    active_drop_thresholds = Counter()
    # 把右侧 `True` 计算得到的值保存到 `all_source_ids_valid`，后面的判断或输出会继续使用它。
    all_source_ids_valid = True
    # 把右侧 `True` 计算得到的值保存到 `all_active_drop_ids_valid`，后面的判断或输出会继续使用它。
    all_active_drop_ids_valid = True
    # 开始循环 `index in range(334)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for index in range(334):
        # 把右侧 `lt[index * 508:(index + 1) * 508]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec = lt[index * 508:(index + 1) * 508]
        # 把右侧 `struct.unpack_from('<i', rec, 0x00)[0]` 计算得到的值保存到 `source_count`，后面的判断或输出会继续使用它。
        source_count = struct.unpack_from('<i', rec, 0x00)[0]
        # 把右侧 `[struct.unpack_from('<i', rec, 0x04 + slot * 4)[0]` 计算得到的值保存到 `source_ids`，后面的判断或输出会继续使用它。
        source_ids = [struct.unpack_from('<i', rec, 0x04 + slot * 4)[0]
                      # 开始循环 `slot in range(max(0, min(source_count, 8)))]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                      for slot in range(max(0, min(source_count, 8)))]
        # 把右侧 `struct.unpack_from('<i', rec, 0x74)[0]` 计算得到的值保存到 `money_base`，后面的判断或输出会继续使用它。
        money_base = struct.unpack_from('<i', rec, 0x74)[0]
        # 把右侧 `struct.unpack_from('<i', rec, 0x78)[0]` 计算得到的值保存到 `money_range`，后面的判断或输出会继续使用它。
        money_range = struct.unpack_from('<i', rec, 0x78)[0]
        # 把右侧 `struct.unpack_from('<i', rec, 0x7C)[0]` 计算得到的值保存到 `drop_count`，后面的判断或输出会继续使用它。
        drop_count = struct.unpack_from('<i', rec, 0x7C)[0]
        # 把右侧 `[struct.unpack_from('<i', rec, 0x80 + slot * 4)[0] for slot in range(4)]` 计算得到的值保存到 `drop_ids`，后面的判断或输出会继续使用它。
        drop_ids = [struct.unpack_from('<i', rec, 0x80 + slot * 4)[0] for slot in range(4)]
        # 把右侧 `[struct.unpack_from('<i', rec, 0x90 + slot * 4)[0] for slot in range(4)]` 计算得到的值保存到 `drop_thresholds`，后面的判断或输出会继续使用它。
        drop_thresholds = [struct.unpack_from('<i', rec, 0x90 + slot * 4)[0] for slot in range(4)]
        # 执行一次赋值：把 `1` 的结果放入左侧 `source_count_dist[source_count] +` 对应的变量/字段。
        source_count_dist[source_count] += 1
        # 执行一次赋值：把 `1` 的结果放入左侧 `drop_count_dist[drop_count] +` 对应的变量/字段。
        drop_count_dist[drop_count] += 1
        # 执行这一条实际代码步骤：`all_source_ids_valid &= 0 <= source_count <= 8 and all(0 <= value < 255 for value in source_ids)`；它与上下相邻语句共同完成当前函数的小任务。
        all_source_ids_valid &= 0 <= source_count <= 8 and all(0 <= value < 255 for value in source_ids)
        # 把右侧 `max(0, min(drop_count, 4))` 计算得到的值保存到 `active`，后面的判断或输出会继续使用它。
        active = max(0, min(drop_count, 4))
        # 执行这一条实际代码步骤：`all_active_drop_ids_valid &= 0 <= drop_count <= 4 and all(0 <= drop_ids[i] < 700 for i in range(act…`；它与上下相邻语句共同完成当前函数的小任务。
        all_active_drop_ids_valid &= 0 <= drop_count <= 4 and all(0 <= drop_ids[i] < 700 for i in range(active))
        # 开始循环 `i in range(active)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i in range(active):
            # 执行一次赋值：把 `1` 的结果放入左侧 `active_drop_thresholds[drop_thresholds[i]] +` 对应的变量/字段。
            active_drop_thresholds[drop_thresholds[i]] += 1
        # 把这一条新结果追加到列表 `layouts`；不会覆盖前面已经收集的记录。
        layouts.append({
            # 给当前结果字典的“LayoutTeam索引”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
            'LayoutTeam索引': index,
            # 给当前结果字典的“奖励源数量_加00”字段填写 `source_count,`，这样导出的 JSON/表格能保留这一项证据。
            '奖励源数量_加00': source_count,
            # 给当前结果字典的“奖励源API_ID_加04”字段填写 `source_ids,`，这样导出的 JSON/表格能保留这一项证据。
            '奖励源API_ID_加04': source_ids,
            # 给当前结果字典的“銀兩基础值_加74”字段填写 `money_base,`，这样导出的 JSON/表格能保留这一项证据。
            '銀兩基础值_加74': money_base,
            # 给当前结果字典的“銀兩随机范围_加78”字段填写 `money_range,`，这样导出的 JSON/表格能保留这一项证据。
            '銀兩随机范围_加78': money_range,
            # 给当前结果字典的“掉落项数量_加7C”字段填写 `drop_count,`，这样导出的 JSON/表格能保留这一项证据。
            '掉落项数量_加7C': drop_count,
            # 给当前结果字典的“掉落Ail2_ID_加80到8C”字段填写 `drop_ids,`，这样导出的 JSON/表格能保留这一项证据。
            '掉落Ail2_ID_加80到8C': drop_ids,
            # 给当前结果字典的“掉落比较阈值_加90到9C”字段填写 `drop_thresholds,`，这样导出的 JSON/表格能保留这一项证据。
            '掉落比较阈值_加90到9C': drop_thresholds,
        })

    # 把右侧 `list(struct.unpack('<101I', lv))` 计算得到的值保存到 `thresholds`，后面的判断或输出会继续使用它。
    thresholds = list(struct.unpack('<101I', lv))
    # 把右侧 `ft[0x1D0 * 560:(0x1D0 + 1) * 560]` 计算得到的值保存到 `ft_rec`，后面的判断或输出会继续使用它。
    ft_rec = ft[0x1D0 * 560:(0x1D0 + 1) * 560]
    # 把右侧 `{` 计算得到的值保存到 `firttech_1d0`，后面的判断或输出会继续使用它。
    firttech_1d0 = {
        # 给当前结果字典的“ID”字段填写 `'0x1D0',`，这样导出的 JSON/表格能保留这一项证据。
        'ID': '0x1D0',
        # 给当前结果字典的“记录ID_加00”字段填写 `struct.unpack_from('<I', ft_rec, 0)[0],`，这样导出的 JSON/表格能保留这一项证据。
        '记录ID_加00': struct.unpack_from('<I', ft_rec, 0)[0],
        # 给当前结果字典的“名称_加04”字段填写 `cp950_cstr(ft_rec, 0x04, 32),`，这样导出的 JSON/表格能保留这一项证据。
        '名称_加04': cp950_cstr(ft_rec, 0x04, 32),
        # 给当前结果字典的“说明_加17C”字段填写 `cp950_cstr(ft_rec, 0x17C, 560 - 0x17C),`，这样导出的 JSON/表格能保留这一项证据。
        '说明_加17C': cp950_cstr(ft_rec, 0x17C, 560 - 0x17C),
        # 给当前结果字典的“五槽首ID_加78”字段填写 `struct.unpack_from('<i', ft_rec, 0x78)[0],`，这样导出的 JSON/表格能保留这一项证据。
        '五槽首ID_加78': struct.unpack_from('<i', ft_rec, 0x78)[0],
        # 给当前结果字典的“证据用途”字段填写 `'与 Oracle 0x421FED 特殊分支合并，证明 FightRole+0xA74=1000000 属于“幽冥返召/操偶”特殊状态路…`，这样导出的 JSON/表格能保留这一项证据。
        '证据用途': '与 Oracle 0x421FED 特殊分支合并，证明 FightRole+0xA74=1000000 属于“幽冥返召/操偶”特殊状态路径；数据本身不单独证明该 runtime writer。',
    }

    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {
        # 给当前结果字典的“工具版本”字段填写 `VER,`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本': VER,
        # 给当前结果字典的“来源”字段填写 `{'LayoutTeam': lt_origin, 'LevelUp': lv_origin, 'FIRTTECH': ft_origin…`，这样导出的 JSON/表格能保留这一项证据。
        '来源': {'LayoutTeam': lt_origin, 'LevelUp': lv_origin, 'FIRTTECH': ft_origin},
        # 给当前结果字典的“解码SHA256”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '解码SHA256': {
            # 给当前结果字典的“LayoutTeam”字段填写 `hashlib.sha256(lt).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
            'LayoutTeam': hashlib.sha256(lt).hexdigest(),
            # 给当前结果字典的“LevelUp”字段填写 `hashlib.sha256(lv).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
            'LevelUp': hashlib.sha256(lv).hexdigest(),
            # 给当前结果字典的“FIRTTECH”字段填写 `hashlib.sha256(ft).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
            'FIRTTECH': hashlib.sha256(ft).hexdigest(),
        },
        # 给当前结果字典的“LayoutTeam奖励字段”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'LayoutTeam奖励字段': {
            # 给当前结果字典的“记录大小”字段填写 `508,`，这样导出的 JSON/表格能保留这一项证据。
            '记录大小': 508,
            # 给当前结果字典的“记录数”字段填写 `334,`，这样导出的 JSON/表格能保留这一项证据。
            '记录数': 334,
            # 给当前结果字典的“静态代码来源边界”字段填写 `'0x424430 复制整条 508-byte LayoutTeam record 到 BattleManager+0xE331；胜利 0…`，这样导出的 JSON/表格能保留这一项证据。
            '静态代码来源边界': '0x424430 复制整条 508-byte LayoutTeam record 到 BattleManager+0xE331；胜利 0x443660 直接读取对应偏移。',
            # 给当前结果字典的“字段”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '字段': {
                # 给当前结果字典的“+0x00”字段填写 `'奖励源 API/RoleDefinition ID 数量（当前数据 1..8）',`，这样导出的 JSON/表格能保留这一项证据。
                '+0x00': '奖励源 API/RoleDefinition ID 数量（当前数据 1..8）',
                # 给当前结果字典的“+0x04...”字段填写 `'按 +0x00 数量读取的奖励源 API ID 列表',`，这样导出的 JSON/表格能保留这一项证据。
                '+0x04...': '按 +0x00 数量读取的奖励源 API ID 列表',
                # 给当前结果字典的“+0x74”字段填写 `'勝利銀兩基础值',`，这样导出的 JSON/表格能保留这一项证据。
                '+0x74': '勝利銀兩基础值',
                # 给当前结果字典的“+0x78”字段填写 `'勝利銀兩随机范围；0则无随机，否则 reward=base+(RNG % range)',`，这样导出的 JSON/表格能保留这一项证据。
                '+0x78': '勝利銀兩随机范围；0则无随机，否则 reward=base+(RNG % range)',
                # 给当前结果字典的“+0x7C”字段填写 `'掉落项数量（0..4）',`，这样导出的 JSON/表格能保留这一项证据。
                '+0x7C': '掉落项数量（0..4）',
                # 给当前结果字典的“+0x80..+0x8C”字段填写 `'4个 Ail2 物品 ID 槽',`，这样导出的 JSON/表格能保留这一项证据。
                '+0x80..+0x8C': '4个 Ail2 物品 ID 槽',
                # 给当前结果字典的“+0x90..+0x9C”字段填写 `'4个掉落比较阈值；代码执行 RNG%100 <= threshold，不能擅自改写成传统百分比公式',`，这样导出的 JSON/表格能保留这一项证据。
                '+0x90..+0x9C': '4个掉落比较阈值；代码执行 RNG%100 <= threshold，不能擅自改写成传统百分比公式',
            },
            # 给当前结果字典的“奖励源数量分布”字段填写 `dict(sorted(source_count_dist.items())),`，这样导出的 JSON/表格能保留这一项证据。
            '奖励源数量分布': dict(sorted(source_count_dist.items())),
            # 给当前结果字典的“掉落项数量分布”字段填写 `dict(sorted(drop_count_dist.items())),`，这样导出的 JSON/表格能保留这一项证据。
            '掉落项数量分布': dict(sorted(drop_count_dist.items())),
            # 给当前结果字典的“活动掉落阈值分布”字段填写 `dict(sorted(active_drop_thresholds.items())),`，这样导出的 JSON/表格能保留这一项证据。
            '活动掉落阈值分布': dict(sorted(active_drop_thresholds.items())),
            # 给当前结果字典的“奖励源ID均在API范围0..254”字段填写 `bool(all_source_ids_valid),`，这样导出的 JSON/表格能保留这一项证据。
            '奖励源ID均在API范围0..254': bool(all_source_ids_valid),
            # 给当前结果字典的“活动掉落ID均在Ail2范围0..699”字段填写 `bool(all_active_drop_ids_valid),`，这样导出的 JSON/表格能保留这一项证据。
            '活动掉落ID均在Ail2范围0..699': bool(all_active_drop_ids_valid),
            # 给当前结果字典的“记录”字段填写 `layouts,`，这样导出的 JSON/表格能保留这一项证据。
            '记录': layouts,
        },
        # 给当前结果字典的“LevelUp”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'LevelUp': {
            # 给当前结果字典的“记录大小”字段填写 `4,`，这样导出的 JSON/表格能保留这一项证据。
            '记录大小': 4,
            # 给当前结果字典的“记录数”字段填写 `101,`，这样导出的 JSON/表格能保留这一项证据。
            '记录数': 101,
            # 给当前结果字典的“阈值”字段填写 `thresholds,`，这样导出的 JSON/表格能保留这一项证据。
            '阈值': thresholds,
            # 给当前结果字典的“有效累计阈值区间”字段填写 `'index 0..98；index 99/100 为尾部0，不能纳入全表单调判断',`，这样导出的 JSON/表格能保留这一项证据。
            '有效累计阈值区间': 'index 0..98；index 99/100 为尾部0，不能纳入全表单调判断',
            # 给当前结果字典的“0到98单调不下降”字段填写 `all(a <= b for a, b in zip(thresholds[:99], thresholds[1:99])),`，这样导出的 JSON/表格能保留这一项证据。
            '0到98单调不下降': all(a <= b for a, b in zip(thresholds[:99], thresholds[1:99])),
            # 给当前结果字典的“尾部值_99_100”字段填写 `thresholds[99:101],`，这样导出的 JSON/表格能保留这一项证据。
            '尾部值_99_100': thresholds[99:101],
            # 给当前结果字典的“边界”字段填写 `'与 Oracle 0x443B60 结合后，RoleDefinition+0x20=等级索引、+0x24=累计经验、+0x28=当前/下…`，这样导出的 JSON/表格能保留这一项证据。
            '边界': '与 Oracle 0x443B60 结合后，RoleDefinition+0x20=等级索引、+0x24=累计经验、+0x28=当前/下一累计升级阈值缓存。最大可用等级边界仍应服从运行时代码的独立上限检查，不能仅由本表尾部0猜测。',
        },
        # 给当前结果字典的“FIRTTECH_0x1D0”字段填写 `firttech_1d0,`，这样导出的 JSON/表格能保留这一项证据。
        'FIRTTECH_0x1D0': firttech_1d0,
        # 给当前结果字典的“证据边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '证据边界': [
            # 继续填写当前数据结构或参数列表中的一项：`'LayoutTeam 字段业务语义来自“当前真实数据分布 + 0x424430整记录复制 + 0x443660胜利结算直接 consumer”交叉；不是仅凭数值猜测。',`。
            'LayoutTeam 字段业务语义来自“当前真实数据分布 + 0x424430整记录复制 + 0x443660胜利结算直接 consumer”交叉；不是仅凭数值猜测。',
            # 继续填写当前数据结构或参数列表中的一项：`'掉落条件严格保留原指令语义 RNG%100 <= threshold；若 threshold=10，数学上命中0..10共11个余数。兼容引擎不得擅自改成 RNG%100 < thres…`。
            '掉落条件严格保留原指令语义 RNG%100 <= threshold；若 threshold=10，数学上命中0..10共11个余数。兼容引擎不得擅自改成 RNG%100 < threshold。',
            # 继续填写当前数据结构或参数列表中的一项：`'FIRTTECH 0x1D0 的名称/说明来自原ENC；FightRole+0xA74 runtime 语义仍需与 Oracle特殊分支/胜负计数合并判断。',`。
            'FIRTTECH 0x1D0 的名称/说明来自原ENC；FightRole+0xA74 runtime 语义仍需与 Oracle特殊分支/胜负计数合并判断。',
        ],
    }
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json(result, args.json)


# 定义函数 `cmd_analyze_d7_special_end`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_analyze_d7_special_end(args):
    # 执行这一条实际代码步骤：`"""重放伏龍A特殊结束、LayoutGR候选布局与伏龍A/B数据交叉证据。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """重放伏龍A特殊结束、LayoutGR候选布局与伏龍A/B数据交叉证据。"""
    # 执行一次赋值：把 `load_named_enc_source(args.source, 'Layoutgr.enc', 508, 250)` 的结果放入左侧 `gr_origin, gr` 对应的变量/字段。
    gr_origin, gr = load_named_enc_source(args.source, 'Layoutgr.enc', 508, 250)
    # 执行一次赋值：把 `load_named_enc_source(args.source, 'LayoutTeam.ENC', 508, 334)` 的结果放入左侧 `lt_origin, lt` 对应的变量/字段。
    lt_origin, lt = load_named_enc_source(args.source, 'LayoutTeam.ENC', 508, 334)
    # 执行一次赋值：把 `load_named_enc_source(args.source, 'Api.enc', 848, 255)` 的结果放入左侧 `api_origin, api` 对应的变量/字段。
    api_origin, api = load_named_enc_source(args.source, 'Api.enc', 848, 255)

    # 定义函数 `i32`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def i32(rec, off): return struct.unpack_from('<i', rec, off)[0]
    # 定义函数 `layout_gr_row`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def layout_gr_row(index):
        # 把右侧 `gr[index*508:(index+1)*508]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec=gr[index*508:(index+1)*508]
        # 把右侧 `i32(rec,0x18)` 计算得到的值保存到 `count`，后面的判断或输出会继续使用它。
        count=i32(rec,0x18)
        # 把右侧 `max(0,min(count,(508-0x1C)//4))` 计算得到的值保存到 `safe`，后面的判断或输出会继续使用它。
        safe=max(0,min(count,(508-0x1C)//4))
        # 把右侧 `[i32(rec,0x1C+i*4) for i in range(safe)]` 计算得到的值保存到 `ids`，后面的判断或输出会继续使用它。
        ids=[i32(rec,0x1C+i*4) for i in range(safe)]
        # 把右侧 `i32(rec,0x6C)` 计算得到的值保存到 `sc`，后面的判断或输出会继续使用它。
        sc=i32(rec,0x6C)
        # 把右侧 `max(0,min(sc,(508-0x70)//4))` 计算得到的值保存到 `ssafe`，后面的判断或输出会继续使用它。
        ssafe=max(0,min(sc,(508-0x70)//4))
        # 把右侧 `[i32(rec,0x70+i*4) for i in range(ssafe)]` 计算得到的值保存到 `secondary`，后面的判断或输出会继续使用它。
        secondary=[i32(rec,0x70+i*4) for i in range(ssafe)]
        # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {
            # 给当前结果字典的“索引”字段填写 `index,'名称_加44':cp950_cstr(rec,0x44,40),`，这样导出的 JSON/表格能保留这一项证据。
            '索引':index,'名称_加44':cp950_cstr(rec,0x44,40),
            # 给当前结果字典的“LayoutTeam候选数量_加18”字段填写 `count,'LayoutTeam候选ID_加1C':ids,`，这样导出的 JSON/表格能保留这一项证据。
            'LayoutTeam候选数量_加18':count,'LayoutTeam候选ID_加1C':ids,
            # 给当前结果字典的“第二候选数量_加6C”字段填写 `sc,'第二候选值_加70':secondary,`，这样导出的 JSON/表格能保留这一项证据。
            '第二候选数量_加6C':sc,'第二候选值_加70':secondary,
            # 给当前结果字典的“字段_加88”字段填写 `i32(rec,0x88),'字段_加9C':i32(rec,0x9C),`，这样导出的 JSON/表格能保留这一项证据。
            '字段_加88':i32(rec,0x88),'字段_加9C':i32(rec,0x9C),
            # 给当前结果字典的“记录SHA256”字段填写 `hashlib.sha256(rec).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
            '记录SHA256':hashlib.sha256(rec).hexdigest(),
        }
    # 定义函数 `layout_team_row`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def layout_team_row(index):
        # 把右侧 `lt[index*508:(index+1)*508]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec=lt[index*508:(index+1)*508]
        # 把右侧 `i32(rec,0x00); ids=[i32(rec,0x04+i*4) for i in range(max(0,min(cnt,8)))]` 计算得到的值保存到 `cnt`，后面的判断或输出会继续使用它。
        cnt=i32(rec,0x00); ids=[i32(rec,0x04+i*4) for i in range(max(0,min(cnt,8)))]
        # 把 `{'索引':index,'奖励源数量_加00':cnt,'奖励源API_ID_加04':ids,` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {'索引':index,'奖励源数量_加00':cnt,'奖励源API_ID_加04':ids,
                # 给当前结果字典的“銀兩基础_加74”字段填写 `i32(rec,0x74),'銀兩随机范围_加78':i32(rec,0x78),`，这样导出的 JSON/表格能保留这一项证据。
                '銀兩基础_加74':i32(rec,0x74),'銀兩随机范围_加78':i32(rec,0x78),
                # 给当前结果字典的“掉落数量_加7C”字段填写 `i32(rec,0x7C),`，这样导出的 JSON/表格能保留这一项证据。
                '掉落数量_加7C':i32(rec,0x7C),
                # 给当前结果字典的“掉落ID_加80到8C”字段填写 `[i32(rec,0x80+i*4) for i in range(4)],`，这样导出的 JSON/表格能保留这一项证据。
                '掉落ID_加80到8C':[i32(rec,0x80+i*4) for i in range(4)],
                # 给当前结果字典的“掉落阈值_加90到9C”字段填写 `[i32(rec,0x90+i*4) for i in range(4)],`，这样导出的 JSON/表格能保留这一项证据。
                '掉落阈值_加90到9C':[i32(rec,0x90+i*4) for i in range(4)],
                # 给当前结果字典的“活动掉落ID”字段填写 `[i32(rec,0x80+i*4) for i in range(max(0,min(i32(rec,0x7C),4)))],`，这样导出的 JSON/表格能保留这一项证据。
                '活动掉落ID':[i32(rec,0x80+i*4) for i in range(max(0,min(i32(rec,0x7C),4)))],
                # 给当前结果字典的“活动掉落阈值”字段填写 `[i32(rec,0x90+i*4) for i in range(max(0,min(i32(rec,0x7C),4)))],`，这样导出的 JSON/表格能保留这一项证据。
                '活动掉落阈值':[i32(rec,0x90+i*4) for i in range(max(0,min(i32(rec,0x7C),4)))],
                # 给当前结果字典的“记录SHA256”字段填写 `hashlib.sha256(rec).hexdigest()}`，这样导出的 JSON/表格能保留这一项证据。
                '记录SHA256':hashlib.sha256(rec).hexdigest()}
    # 定义函数 `api_row`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def api_row(index):
        # 把右侧 `api[index*848:(index+1)*848]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec=api[index*848:(index+1)*848]
        # 把 `{'索引':index,'名称_加0C':cp950_cstr(rec,0x0C,20),'等级_加20':i32(rec,0x20),'EXP_加24':i32(rec,0x2…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {'索引':index,'名称_加0C':cp950_cstr(rec,0x0C,20),'等级_加20':i32(rec,0x20),'EXP_加24':i32(rec,0x24),
                # 给当前结果字典的“主资源max_加2C”字段填写 `i32(rec,0x2C),'主资源current_加30':i32(rec,0x30),`，这样导出的 JSON/表格能保留这一项证据。
                '主资源max_加2C':i32(rec,0x2C),'主资源current_加30':i32(rec,0x30),
                # 给当前结果字典的“次资源max_加34”字段填写 `i32(rec,0x34),'次资源current_加38':i32(rec,0x38),`，这样导出的 JSON/表格能保留这一项证据。
                '次资源max_加34':i32(rec,0x34),'次资源current_加38':i32(rec,0x38),
                # 给当前结果字典的“字段_加3C”字段填写 `i32(rec,0x3C),'字段_加40':i32(rec,0x40),'字段_加44':i32(rec,0x44),`，这样导出的 JSON/表格能保留这一项证据。
                '字段_加3C':i32(rec,0x3C),'字段_加40':i32(rec,0x40),'字段_加44':i32(rec,0x44),
                # 给当前结果字典的“字段_加48”字段填写 `i32(rec,0x48),'字段_加4C':i32(rec,0x4C),'记录SHA256':hashlib.sha256(rec).h…`，这样导出的 JSON/表格能保留这一项证据。
                '字段_加48':i32(rec,0x48),'字段_加4C':i32(rec,0x4C),'记录SHA256':hashlib.sha256(rec).hexdigest()}

    # 把右侧 `Counter(); secondary_dist=Counter(); invalid=[]` 计算得到的值保存到 `count_dist`，后面的判断或输出会继续使用它。
    count_dist=Counter(); secondary_dist=Counter(); invalid=[]
    # 开始循环 `index in range(250)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for index in range(250):
        # 把右侧 `gr[index*508:(index+1)*508]; c=i32(rec,0x18); sc=i32(rec,0x6C)` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec=gr[index*508:(index+1)*508]; c=i32(rec,0x18); sc=i32(rec,0x6C)
        # 执行一次赋值：把 `1; secondary_dist[sc]+=1` 的结果放入左侧 `count_dist[c]+` 对应的变量/字段。
        count_dist[c]+=1; secondary_dist[sc]+=1
        # 检查条件 `c<0 or c>(508-0x1C)//4`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if c<0 or c>(508-0x1C)//4:
            # 把这一条新结果追加到列表 `invalid`；不会覆盖前面已经收集的记录。
            invalid.append({'索引':index,'原因':'候选数量越界','count':c}); continue
        # 开始循环 `slot in range(c)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for slot in range(c):
            # 把右侧 `i32(rec,0x1C+slot*4)` 计算得到的值保存到 `v`，后面的判断或输出会继续使用它。
            v=i32(rec,0x1C+slot*4)
            # 检查条件 `not 0<=v<334: invalid.append({'索引':index,'slot':slot,'LayoutTeamID'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not 0<=v<334: invalid.append({'索引':index,'slot':slot,'LayoutTeamID':v})
    # 把右侧 `layout_gr_row(130); b=layout_gr_row(131)` 计算得到的值保存到 `a`，后面的判断或输出会继续使用它。
    a=layout_gr_row(130); b=layout_gr_row(131)
    # 把右侧 `layout_team_row(130); ltb=layout_team_row(131)` 计算得到的值保存到 `lta`，后面的判断或输出会继续使用它。
    lta=layout_team_row(130); ltb=layout_team_row(131)
    # 把右侧 `api_row(236); api211=api_row(211)` 计算得到的值保存到 `api236`，后面的判断或输出会继续使用它。
    api236=api_row(236); api211=api_row(211)
    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result={
        # 给当前结果字典的“工具版本”字段填写 `VER,'来源':{'LayoutGR':gr_origin,'LayoutTeam':lt_origin,'API':api_origi…`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本':VER,'来源':{'LayoutGR':gr_origin,'LayoutTeam':lt_origin,'API':api_origin},
        # 给当前结果字典的“解码SHA256”字段填写 `{'LayoutGR':hashlib.sha256(gr).hexdigest(),'LayoutTeam':hashlib.sha25…`，这样导出的 JSON/表格能保留这一项证据。
        '解码SHA256':{'LayoutGR':hashlib.sha256(gr).hexdigest(),'LayoutTeam':hashlib.sha256(lt).hexdigest(),'API':hashlib.sha256(api).hexdigest()},
        # 给当前结果字典的“LayoutGR字段当前结论”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'LayoutGR字段当前结论':{
            # 给当前结果字典的“+0x18”字段填写 `'LayoutTeam候选数量；0x424430直接作为RNG除数/数量使用',`，这样导出的 JSON/表格能保留这一项证据。
            '+0x18':'LayoutTeam候选数量；0x424430直接作为RNG除数/数量使用',
            # 给当前结果字典的“+0x1C”字段填写 `'LayoutTeam候选ID数组；按随机索引读取实际ID',`，这样导出的 JSON/表格能保留这一项证据。
            '+0x1C':'LayoutTeam候选ID数组；按随机索引读取实际ID',
            # 给当前结果字典的“+0x6C”字段填写 `'第二候选数量；复制LayoutTeam后由0x424430再次作为RNG除数',`，这样导出的 JSON/表格能保留这一项证据。
            '+0x6C':'第二候选数量；复制LayoutTeam后由0x424430再次作为RNG除数',
            # 给当前结果字典的“+0x70”字段填写 `'第二候选值数组；按第二随机索引返回，业务语义仍待证明',`，这样导出的 JSON/表格能保留这一项证据。
            '+0x70':'第二候选值数组；按第二随机索引返回，业务语义仍待证明',
            # 给当前结果字典的“旧结论撤销”字段填写 `'旧v0.7A把+0x18称为layout_team_index_primary、+0x1C称alternate candidate，已被…`，这样导出的 JSON/表格能保留这一项证据。
            '旧结论撤销':'旧v0.7A把+0x18称为layout_team_index_primary、+0x1C称alternate candidate，已被0x424430直接consumer否定。',
        },
        # 给当前结果字典的“LayoutGR候选数量分布_加18”字段填写 `dict(sorted(count_dist.items())),`，这样导出的 JSON/表格能保留这一项证据。
        'LayoutGR候选数量分布_加18':dict(sorted(count_dist.items())),
        # 给当前结果字典的“第二候选数量分布_加6C”字段填写 `dict(sorted(secondary_dist.items())),`，这样导出的 JSON/表格能保留这一项证据。
        '第二候选数量分布_加6C':dict(sorted(secondary_dist.items())),
        # 给当前结果字典的“全部活动LayoutTeam候选ID有效”字段填写 `len(invalid)==0,'无效项':invalid,`，这样导出的 JSON/表格能保留这一项证据。
        '全部活动LayoutTeam候选ID有效':len(invalid)==0,'无效项':invalid,
        # 给当前结果字典的“伏龍A_LayoutGR130”字段填写 `a,'伏龍B_LayoutGR131':b,`，这样导出的 JSON/表格能保留这一项证据。
        '伏龍A_LayoutGR130':a,'伏龍B_LayoutGR131':b,
        # 给当前结果字典的“伏龍A_LayoutTeam130”字段填写 `lta,'伏龍B_LayoutTeam131':ltb,`，这样导出的 JSON/表格能保留这一项证据。
        '伏龍A_LayoutTeam130':lta,'伏龍B_LayoutTeam131':ltb,
        # 给当前结果字典的“伏龍A奖励源_API236”字段填写 `api236,'伏龍B奖励源_API211':api211,`，这样导出的 JSON/表格能保留这一项证据。
        '伏龍A奖励源_API236':api236,'伏龍B奖励源_API211':api211,
        # 给当前结果字典的“伏龍A特殊结束静态规则”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '伏龍A特殊结束静态规则':{
            # 给当前结果字典的“触发配置”字段填写 `'EVE opcode0x37 record+0x04 == 0x82；0x82十进制130即LayoutGR[130]“伏龍A”',`，这样导出的 JSON/表格能保留这一项证据。
            '触发配置':'EVE opcode0x37 record+0x04 == 0x82；0x82十进制130即LayoutGR[130]“伏龍A”',
            # 给当前结果字典的“计数器初始化”字段填写 `'DataCenter+0xF4 = RNG % 400 + 800；其他LayoutGR清0',`，这样导出的 JSON/表格能保留这一项证据。
            '计数器初始化':'DataCenter+0xF4 = RNG % 400 + 800；其他LayoutGR清0',
            # 给当前结果字典的“递减”字段填写 `'Battle主虚函数0x442920在battle-active gate有效且0x4696DC非0时，若+0xF4>1则每次合格调用减…`，这样导出的 JSON/表格能保留这一项证据。
            '递减':'Battle主虚函数0x442920在battle-active gate有效且0x4696DC非0时，若+0xF4>1则每次合格调用减1；单位不得擅自称帧/毫秒',
            # 给当前结果字典的“结束”字段填写 `'0x443590见+0xF4==1时清零、把正常敌方存活计数置0并返回特殊detector code4；0x4328A0把detecto…`，这样导出的 JSON/表格能保留这一项证据。
            '结束':'0x443590见+0xF4==1时清零、把正常敌方存活计数置0并返回特殊detector code4；0x4328A0把detector 1或4统一以dispatcher参数1进入完整胜利路径。',
            # 给当前结果字典的“与因事停戰区分”字段填写 `'detector return4不是0x443660 dispatcher参数4；后者文本“因事停戰”当前无已知E8 caller传4。…`，这样导出的 JSON/表格能保留这一项证据。
            '与因事停戰区分':'detector return4不是0x443660 dispatcher参数4；后者文本“因事停戰”当前无已知E8 caller传4。',
        },
        # 给当前结果字典的“数据交叉”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '数据交叉':{
            # 给当前结果字典的“伏龍A”字段填写 `'LayoutTeam130→API236“黑龍解臾”，EXP=0，銀兩=0，掉落=0，主资源max=100000。',`，这样导出的 JSON/表格能保留这一项证据。
            '伏龍A':'LayoutTeam130→API236“黑龍解臾”，EXP=0，銀兩=0，掉落=0，主资源max=100000。',
            # 给当前结果字典的“伏龍B”字段填写 `'LayoutTeam131→API211“黑龍解臾”，EXP=18000，銀兩base=1600/range=800，存在正常掉落，主资…`，这样导出的 JSON/表格能保留这一项证据。
            '伏龍B':'LayoutTeam131→API211“黑龍解臾”，EXP=18000，銀兩base=1600/range=800，存在正常掉落，主资源max=10000。',
            # 给当前结果字典的“推断边界”字段填写 `'A/B同名且A无标准奖励、B有正常奖励，强支持阶段战解释；但当前真实EVE样本不含opcode0x37，不能冻结“伏龍A后必然紧接伏龍B…`，这样导出的 JSON/表格能保留这一项证据。
            '推断边界':'A/B同名且A无标准奖励、B有正常奖励，强支持阶段战解释；但当前真实EVE样本不含opcode0x37，不能冻结“伏龍A后必然紧接伏龍B”的脚本顺序。',
        },
    }
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json(result,args.json)

# 把右侧 `[0x10F, 0x110, 0x111]` 计算得到的值保存到 `REVIVAL_ITEM_IDS`，后面的判断或输出会继续使用它。
REVIVAL_ITEM_IDS = [0x10F, 0x110, 0x111]
# 把右侧 `list(range(0x12D, 0x134))` 计算得到的值保存到 `PERMANENT_ITEM_IDS`，后面的判断或输出会继续使用它。
PERMANENT_ITEM_IDS = list(range(0x12D, 0x134))
# 把右侧 `[0x170, 0x171, 0x172]` 计算得到的值保存到 `SPECIAL_WORLD_ITEM_IDS`，后面的判断或输出会继续使用它。
SPECIAL_WORLD_ITEM_IDS = [0x170, 0x171, 0x172]

# 0x43AEC0 / 0x43B130..0x43B154 directly proven mapping.
# Business labels are intentionally phrased conservatively.
# 把右侧 `{` 计算得到的值保存到 `PERSISTENT_EFFECT_WRITER`，后面的判断或输出会继续使用它。
PERSISTENT_EFFECT_WRITER = {
    # 执行一次赋值：把 `1；随后 +0x3C += Ail2+0xD0', '地址': ['0x43B019', '0x43B023'],` 的结果放入左侧 `43: {'RoleDefinition偏移': '0x9C` 对应的变量/字段。
    43: {'RoleDefinition偏移': '0x9C=1；随后 +0x3C += Ail2+0xD0', '地址': ['0x43B019', '0x43B023'],
         # 给当前结果字典的“边界”字段填写 `'特殊复苏相关物品会进入；不得把 effect ID 43 本身称为复苏 opcode。'},`，这样导出的 JSON/表格能保留这一项证据。
         '边界': '特殊复苏相关物品会进入；不得把 effect ID 43 本身称为复苏 opcode。'},
    # 继续填写当前数据结构或参数列表中的一项：`54: {'RoleDefinition偏移': '0x3C', '地址': ['0x43B023']},`。
    54: {'RoleDefinition偏移': '0x3C', '地址': ['0x43B023']},
    # 继续填写当前数据结构或参数列表中的一项：`55: {'RoleDefinition偏移': '0x40', '地址': ['0x43B028']},`。
    55: {'RoleDefinition偏移': '0x40', '地址': ['0x43B028']},
    # 继续填写当前数据结构或参数列表中的一项：`56: {'RoleDefinition偏移': '0x48', '地址': ['0x43B02D']},`。
    56: {'RoleDefinition偏移': '0x48', '地址': ['0x43B02D']},
    # 继续填写当前数据结构或参数列表中的一项：`57: {'RoleDefinition偏移': '0x4C', '地址': ['0x43B032']},`。
    57: {'RoleDefinition偏移': '0x4C', '地址': ['0x43B032']},
    # 继续填写当前数据结构或参数列表中的一项：`58: {'RoleDefinition偏移': '0x44', '地址': ['0x43B037']},`。
    58: {'RoleDefinition偏移': '0x44', '地址': ['0x43B037']},
    # 继续填写当前数据结构或参数列表中的一项：`59: {'RoleDefinition偏移': '0x2C', '地址': ['0x43B03C']},`。
    59: {'RoleDefinition偏移': '0x2C', '地址': ['0x43B03C']},
    # 继续填写当前数据结构或参数列表中的一项：`60: {'RoleDefinition偏移': '0x34', '地址': ['0x43B041']},`。
    60: {'RoleDefinition偏移': '0x34', '地址': ['0x43B041']},
}


# 定义函数 `cmd_analyze_special_items`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_analyze_special_items(args):
    # 执行一次赋值：把 `load_ail2_source(args.source)` 的结果放入左侧 `origin, data` 对应的变量/字段。
    origin, data = load_ail2_source(args.source)
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows = []
    # 开始循环 `index in REVIVAL_ITEM_IDS + PERMANENT_ITEM_IDS + SPECIAL_WORLD_ITEM_IDS`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for index in REVIVAL_ITEM_IDS + PERMANENT_ITEM_IDS + SPECIAL_WORLD_ITEM_IDS:
        # 把右侧 `data[index * 926:(index + 1) * 926]` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
        r = data[index * 926:(index + 1) * 926]
        # 把右侧 `[struct.unpack_from('<i', r, 0xE8 + s * 4)[0] for s in range(5)]` 计算得到的值保存到 `values`，后面的判断或输出会继续使用它。
        values = [struct.unpack_from('<i', r, 0xE8 + s * 4)[0] for s in range(5)]
        # 把右侧 `[struct.unpack_from('<i', r, 0xFC + s * 4)[0] for s in range(5)]` 计算得到的值保存到 `ids`，后面的判断或输出会继续使用它。
        ids = [struct.unpack_from('<i', r, 0xFC + s * 4)[0] for s in range(5)]
        # 把右侧 `{` 计算得到的值保存到 `row`，后面的判断或输出会继续使用它。
        row = {
            # 给当前结果字典的“物理记录索引”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
            '物理记录索引': index,
            # 给当前结果字典的“十六进制”字段填写 `f'0x{index:X}',`，这样导出的 JSON/表格能保留这一项证据。
            '十六进制': f'0x{index:X}',
            # 给当前结果字典的“Ail2加20记录物品ID”字段填写 `struct.unpack_from('<I', r, 0x20)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加20记录物品ID': struct.unpack_from('<I', r, 0x20)[0],
            # 给当前结果字典的“名称”字段填写 `cp950_cstr(r, 0, 32),`，这样导出的 JSON/表格能保留这一项证据。
            '名称': cp950_cstr(r, 0, 32),
            # 给当前结果字典的“说明”字段填写 `cp950_cstr(r, 0x188, 512),`，这样导出的 JSON/表格能保留这一项证据。
            '说明': cp950_cstr(r, 0x188, 512),
            # 给当前结果字典的“Ail2加24”字段填写 `struct.unpack_from('<i', r, 0x24)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加24': struct.unpack_from('<i', r, 0x24)[0],
            # 给当前结果字典的“Ail2加28特殊动作码”字段填写 `struct.unpack_from('<i', r, 0x28)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加28特殊动作码': struct.unpack_from('<i', r, 0x28)[0],
            # 给当前结果字典的“Ail2加2C”字段填写 `struct.unpack_from('<i', r, 0x2C)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加2C': struct.unpack_from('<i', r, 0x2C)[0],
            # 给当前结果字典的“Ail2加30”字段填写 `struct.unpack_from('<i', r, 0x30)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加30': struct.unpack_from('<i', r, 0x30)[0],
            # 给当前结果字典的“Ail2加C4”字段填写 `struct.unpack_from('<i', r, 0xC4)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加C4': struct.unpack_from('<i', r, 0xC4)[0],
            # 给当前结果字典的“Ail2加C8”字段填写 `struct.unpack_from('<i', r, 0xC8)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加C8': struct.unpack_from('<i', r, 0xC8)[0],
            # 给当前结果字典的“Ail2加D0写入增量”字段填写 `struct.unpack_from('<i', r, 0xD0)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加D0写入增量': struct.unpack_from('<i', r, 0xD0)[0],
            # 给当前结果字典的“Ail2加D8”字段填写 `struct.unpack_from('<i', r, 0xD8)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加D8': struct.unpack_from('<i', r, 0xD8)[0],
            # 给当前结果字典的“Ail2加DC”字段填写 `struct.unpack_from('<i', r, 0xDC)[0],`，这样导出的 JSON/表格能保留这一项证据。
            'Ail2加DC': struct.unpack_from('<i', r, 0xDC)[0],
            # 给当前结果字典的“五槽数值”字段填写 `values,`，这样导出的 JSON/表格能保留这一项证据。
            '五槽数值': values,
            # 给当前结果字典的“五槽ID”字段填写 `ids,`，这样导出的 JSON/表格能保留这一项证据。
            '五槽ID': ids,
        }
        # 把右侧 `[]` 计算得到的值保存到 `mapped`，后面的判断或输出会继续使用它。
        mapped = []
        # 开始循环 `slot, eid in enumerate(ids)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for slot, eid in enumerate(ids):
            # 检查条件 `values[slot] > 0 and eid in PERSISTENT_EFFECT_WRITER`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if values[slot] > 0 and eid in PERSISTENT_EFFECT_WRITER:
                # 把右侧 `dict(PERSISTENT_EFFECT_WRITER[eid])` 计算得到的值保存到 `entry`，后面的判断或输出会继续使用它。
                entry = dict(PERSISTENT_EFFECT_WRITER[eid])
                # 调用 `entry.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
                entry.update({'slot': slot, 'effect_ID': eid, 'slot_value_gate': values[slot],
                              # 给当前结果字典的“实际加数_Ail2加D0”字段填写 `row['Ail2加D0写入增量']})`，这样导出的 JSON/表格能保留这一项证据。
                              '实际加数_Ail2加D0': row['Ail2加D0写入增量']})
                # 把这一条新结果追加到列表 `mapped`；不会覆盖前面已经收集的记录。
                mapped.append(entry)
        # 执行一次赋值：把 `mapped` 的结果放入左侧 `row['0x43AEC0可直接证明的writer']` 对应的变量/字段。
        row['0x43AEC0可直接证明的writer'] = mapped
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append(row)
    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {
        # 给当前结果字典的“工具版本”字段填写 `VER,`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本': VER,
        # 给当前结果字典的“来源”字段填写 `origin,`，这样导出的 JSON/表格能保留这一项证据。
        '来源': origin,
        # 给当前结果字典的“Ail2解码SHA256”字段填写 `hashlib.sha256(data).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
        'Ail2解码SHA256': hashlib.sha256(data).hexdigest(),
        # 给当前结果字典的“复苏相关物品”字段填写 `['0x10F', '0x110', '0x111'],`，这样导出的 JSON/表格能保留这一项证据。
        '复苏相关物品': ['0x10F', '0x110', '0x111'],
        # 给当前结果字典的“永久能力物品范围”字段填写 `['0x12D', '0x133'],`，这样导出的 JSON/表格能保留这一项证据。
        '永久能力物品范围': ['0x12D', '0x133'],
        # 给当前结果字典的“特殊世界物品”字段填写 `['0x170', '0x171', '0x172'],`，这样导出的 JSON/表格能保留这一项证据。
        '特殊世界物品': ['0x170', '0x171', '0x172'],
        # 给当前结果字典的“记录”字段填写 `rows,`，这样导出的 JSON/表格能保留这一项证据。
        '记录': rows,
        # 给当前结果字典的“静态writer规则”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '静态writer规则': {
            # 给当前结果字典的“函数”字段填写 `'0x43AEC0',`，这样导出的 JSON/表格能保留这一项证据。
            '函数': '0x43AEC0',
            # 给当前结果字典的“目标记录”字段填写 `'DataCenter+0xA4 + role_id*0x350（DataCenter+0xA4 的 API.ENC 身份需与“验证RPG…`，这样导出的 JSON/表格能保留这一项证据。
            '目标记录': 'DataCenter+0xA4 + role_id*0x350（DataCenter+0xA4 的 API.ENC 身份需与“验证RPG”证据合并判断）',
            # 给当前结果字典的“五槽”字段填写 `'value=Ail2+0xE8+slot*4；ID=Ail2+0xFC+slot*4；value>0 且 ID∈43..60 才进入该 …`，这样导出的 JSON/表格能保留这一项证据。
            '五槽': 'value=Ail2+0xE8+slot*4；ID=Ail2+0xFC+slot*4；value>0 且 ID∈43..60 才进入该 switch',
            # 给当前结果字典的“实际加数”字段填写 `'Ail2+0xD0；五槽 value 在该 writer 中是 gate，不是最终 add 数值',`，这样导出的 JSON/表格能保留这一项证据。
            '实际加数': 'Ail2+0xD0；五槽 value 在该 writer 中是 gate，不是最终 add 数值',
            # 给当前结果字典的“映射”字段填写 `PERSISTENT_EFFECT_WRITER,`，这样导出的 JSON/表格能保留这一项证据。
            '映射': PERSISTENT_EFFECT_WRITER,
        },
        # 给当前结果字典的“边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '边界': [
            # 继续填写当前数据结构或参数列表中的一项：`'本命令从 Ail2 数据重放物品字段；writer 地址与 DataCenter/API 身份必须同时由当前 Oracle RPG.exe 静态检查证明。',`。
            '本命令从 Ail2 数据重放物品字段；writer 地址与 DataCenter/API 身份必须同时由当前 Oracle RPG.exe 静态检查证明。',
            # 继续填写当前数据结构或参数列表中的一项：`'ID44..53 在 0x43AEC0 这一个 writer 中落到 no-op/default；不能据此证明它们在全游戏无其他 consumer。',`。
            'ID44..53 在 0x43AEC0 这一个 writer 中落到 no-op/default；不能据此证明它们在全游戏无其他 consumer。',
            # 执行一次赋值：把 `12/13/14 特殊动作码提前返回；其大ID 61/63/64 不是这条执行分派的 selector。',` 的结果放入左侧 `'0x170..0x172 的实际世界效果使用 Ail2+0x28` 对应的变量/字段。
            '0x170..0x172 的实际世界效果使用 Ail2+0x28=12/13/14 特殊动作码提前返回；其大ID 61/63/64 不是这条执行分派的 selector。',
            # 继续填写当前数据结构或参数列表中的一项：`'属性作者原名仅在有独立 UI/文本/consumer 证据时命名；本输出以 RoleDefinition offset 和官方物品描述为主。',`。
            '属性作者原名仅在有独立 UI/文本/consumer 证据时命名；本输出以 RoleDefinition offset 和官方物品描述为主。',
        ],
    }
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json(result, args.json)


# 定义类 `PEImage`，把一组彼此相关的数据和操作整理在同一个类型中。
class PEImage:
    # 定义函数 `__init__`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def __init__(self, path: str | Path):
        # 把右侧 `Path(path)` 计算得到的值保存到 `self.path`，后面的判断或输出会继续使用它。
        self.path = Path(path)
        # 把右侧 `self.path.read_bytes()` 计算得到的值保存到 `self.data`，后面的判断或输出会继续使用它。
        self.data = self.path.read_bytes()
        # 检查条件 `self.data[:2] != b'MZ'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if self.data[:2] != b'MZ':
            # 检测到不能继续的情况，主动抛出异常 `ValueError('不是 MZ/PE 文件')`，让上层明确知道数据或参数不符合要求。
            raise ValueError('不是 MZ/PE 文件')
        # 检查条件 `len(self.data) < 0x40`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(self.data) < 0x40:
            # 检测到不能继续的情况，主动抛出异常 `ValueError('文件过短')`，让上层明确知道数据或参数不符合要求。
            raise ValueError('文件过短')
        # 把右侧 `struct.unpack_from('<I', self.data, 0x3C)[0]` 计算得到的值保存到 `pe_offset`，后面的判断或输出会继续使用它。
        pe_offset = struct.unpack_from('<I', self.data, 0x3C)[0]
        # 检查条件 `self.data[pe_offset:pe_offset + 4] != b'PE\0\0'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if self.data[pe_offset:pe_offset + 4] != b'PE\0\0':
            # 检测到不能继续的情况，主动抛出异常 `ValueError('PE 签名无效')`，让上层明确知道数据或参数不符合要求。
            raise ValueError('PE 签名无效')
        # 把右侧 `pe_offset + 4` 计算得到的值保存到 `coff`，后面的判断或输出会继续使用它。
        coff = pe_offset + 4
        # 执行一次赋值：把 `struct.unpack_from('<HHIIIHH', self.data, coff)` 的结果放入左侧 `self.machine, self.section_count, _, _, _, optional_si…` 对应的变量/字段。
        self.machine, self.section_count, _, _, _, optional_size, _ = struct.unpack_from('<HHIIIHH', self.data, coff)
        # 把右侧 `coff + 20` 计算得到的值保存到 `optional`，后面的判断或输出会继续使用它。
        optional = coff + 20
        # 把右侧 `struct.unpack_from('<H', self.data, optional)[0]` 计算得到的值保存到 `magic`，后面的判断或输出会继续使用它。
        magic = struct.unpack_from('<H', self.data, optional)[0]
        # 检查条件 `magic == 0x10B`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if magic == 0x10B:  # PE32
            # 把右侧 `'PE32'` 计算得到的值保存到 `self.pe_kind`，后面的判断或输出会继续使用它。
            self.pe_kind = 'PE32'
            # 把右侧 `struct.unpack_from('<I', self.data, optional + 28)[0]` 计算得到的值保存到 `self.image_base`，后面的判断或输出会继续使用它。
            self.image_base = struct.unpack_from('<I', self.data, optional + 28)[0]
            # 把右侧 `struct.unpack_from('<I', self.data, optional + 16)[0]` 计算得到的值保存到 `self.entry_rva`，后面的判断或输出会继续使用它。
            self.entry_rva = struct.unpack_from('<I', self.data, optional + 16)[0]
        # 前一个条件没有成立时，再检查 `magic == 0x20B`；成立才执行这一分支。
        elif magic == 0x20B:  # PE32+
            # 把右侧 `'PE32+'` 计算得到的值保存到 `self.pe_kind`，后面的判断或输出会继续使用它。
            self.pe_kind = 'PE32+'
            # 把右侧 `struct.unpack_from('<Q', self.data, optional + 24)[0]` 计算得到的值保存到 `self.image_base`，后面的判断或输出会继续使用它。
            self.image_base = struct.unpack_from('<Q', self.data, optional + 24)[0]
            # 把右侧 `struct.unpack_from('<I', self.data, optional + 16)[0]` 计算得到的值保存到 `self.entry_rva`，后面的判断或输出会继续使用它。
            self.entry_rva = struct.unpack_from('<I', self.data, optional + 16)[0]
        # 前面的条件分支都没有命中时，执行这个兜底分支。
        else:
            # 检测到不能继续的情况，主动抛出异常 `ValueError(f'不支持的 PE OptionalHeader Magic: 0x{magic:04X}')`，让上层明确知道数据或参数不符合要求。
            raise ValueError(f'不支持的 PE OptionalHeader Magic: 0x{magic:04X}')
        # 把右侧 `optional + optional_size` 计算得到的值保存到 `section_table`，后面的判断或输出会继续使用它。
        section_table = optional + optional_size
        # 把右侧 `[]` 计算得到的值保存到 `self.sections`，后面的判断或输出会继续使用它。
        self.sections = []
        # 开始循环 `index in range(self.section_count)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for index in range(self.section_count):
            # 把右侧 `section_table + index * 40` 计算得到的值保存到 `off`，后面的判断或输出会继续使用它。
            off = section_table + index * 40
            # 把右侧 `self.data[off:off + 8].split(b'\0', 1)[0]` 计算得到的值保存到 `raw_name`，后面的判断或输出会继续使用它。
            raw_name = self.data[off:off + 8].split(b'\0', 1)[0]
            # 把右侧 `raw_name.decode('ascii', 'replace')` 计算得到的值保存到 `name`，后面的判断或输出会继续使用它。
            name = raw_name.decode('ascii', 'replace')
            # 执行一次赋值：把 `struct.unpack_from('<IIII', self.data, off + 8)` 的结果放入左侧 `virtual_size, virtual_address, raw_size, raw_ptr` 对应的变量/字段。
            virtual_size, virtual_address, raw_size, raw_ptr = struct.unpack_from('<IIII', self.data, off + 8)
            # 把右侧 `struct.unpack_from('<I', self.data, off + 36)[0]` 计算得到的值保存到 `characteristics`，后面的判断或输出会继续使用它。
            characteristics = struct.unpack_from('<I', self.data, off + 36)[0]
            # 把这一条新结果追加到列表 `sections`；不会覆盖前面已经收集的记录。
            self.sections.append({
                # 给当前结果字典的“name”字段填写 `name,`，这样导出的 JSON/表格能保留这一项证据。
                'name': name,
                # 给当前结果字典的“virtual_size”字段填写 `virtual_size,`，这样导出的 JSON/表格能保留这一项证据。
                'virtual_size': virtual_size,
                # 给当前结果字典的“virtual_address”字段填写 `virtual_address,`，这样导出的 JSON/表格能保留这一项证据。
                'virtual_address': virtual_address,
                # 给当前结果字典的“raw_size”字段填写 `raw_size,`，这样导出的 JSON/表格能保留这一项证据。
                'raw_size': raw_size,
                # 给当前结果字典的“raw_ptr”字段填写 `raw_ptr,`，这样导出的 JSON/表格能保留这一项证据。
                'raw_ptr': raw_ptr,
                # 给当前结果字典的“characteristics”字段填写 `characteristics,`，这样导出的 JSON/表格能保留这一项证据。
                'characteristics': characteristics,
            })

    # 定义函数 `va_to_offset`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def va_to_offset(self, va: int) -> int:
        # 把右侧 `va - self.image_base` 计算得到的值保存到 `rva`，后面的判断或输出会继续使用它。
        rva = va - self.image_base
        # 检查条件 `rva < 0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if rva < 0:
            # 检测到不能继续的情况，主动抛出异常 `ValueError(f'VA 低于 ImageBase: 0x{va:X}')`，让上层明确知道数据或参数不符合要求。
            raise ValueError(f'VA 低于 ImageBase: 0x{va:X}')
        # 开始循环 `section in self.sections`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for section in self.sections:
            # 把右侧 `section['virtual_address']` 计算得到的值保存到 `start`，后面的判断或输出会继续使用它。
            start = section['virtual_address']
            # 把右侧 `max(section['virtual_size'], section['raw_size'])` 计算得到的值保存到 `span`，后面的判断或输出会继续使用它。
            span = max(section['virtual_size'], section['raw_size'])
            # 检查条件 `start <= rva < start + span`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if start <= rva < start + span:
                # 把右侧 `rva - start` 计算得到的值保存到 `delta`，后面的判断或输出会继续使用它。
                delta = rva - start
                # 检查条件 `delta >= section['raw_size']`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if delta >= section['raw_size']:
                    # 检测到不能继续的情况，主动抛出异常 `ValueError(f'VA 0x{va:X} 位于未落盘的虚拟区')`，让上层明确知道数据或参数不符合要求。
                    raise ValueError(f'VA 0x{va:X} 位于未落盘的虚拟区')
                # 把 `section['raw_ptr'] + delta` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
                return section['raw_ptr'] + delta
        # PE headers can also be addressed by RVA.
        # 检查条件 `rva < min((s['raw_ptr'] for s in self.sections), default=len(self.data))`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if rva < min((s['raw_ptr'] for s in self.sections), default=len(self.data)):
            # 把 `rva` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
            return rva
        # 检测到不能继续的情况，主动抛出异常 `ValueError(f'VA 无法映射到文件偏移: 0x{va:X}')`，让上层明确知道数据或参数不符合要求。
        raise ValueError(f'VA 无法映射到文件偏移: 0x{va:X}')

    # 定义函数 `read_va`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def read_va(self, va: int, size: int) -> bytes:
        # 把右侧 `self.va_to_offset(va)` 计算得到的值保存到 `off`，后面的判断或输出会继续使用它。
        off = self.va_to_offset(va)
        # 把右侧 `self.data[off:off + size]` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
        out = self.data[off:off + size]
        # 检查条件 `len(out) != size`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(out) != size:
            # 检测到不能继续的情况，主动抛出异常 `ValueError(f'VA 0x{va:X} 读取不足 {size} 字节')`，让上层明确知道数据或参数不符合要求。
            raise ValueError(f'VA 0x{va:X} 读取不足 {size} 字节')
        # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return out

    # 定义函数 `read_u32`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def read_u32(self, va: int) -> int:
        # 把 `struct.unpack('<I', self.read_va(va, 4))[0]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return struct.unpack('<I', self.read_va(va, 4))[0]

    # 定义函数 `call_target`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def call_target(self, call_va: int) -> int:
        # 把右侧 `self.read_va(call_va, 5)` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw = self.read_va(call_va, 5)
        # 检查条件 `raw[0] != 0xE8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if raw[0] != 0xE8:
            # 检测到不能继续的情况，主动抛出异常 `ValueError(f'0x{call_va:X} 不是 E8 rel32 CALL，实际首字节 0x{raw[0]:02X}')`，让上层明确知道数据或参数不符合要求。
            raise ValueError(f'0x{call_va:X} 不是 E8 rel32 CALL，实际首字节 0x{raw[0]:02X}')
        # 把右侧 `struct.unpack_from('<i', raw, 1)[0]` 计算得到的值保存到 `rel`，后面的判断或输出会继续使用它。
        rel = struct.unpack_from('<i', raw, 1)[0]
        # 把 `(call_va + 5 + rel) & 0xFFFFFFFF` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return (call_va + 5 + rel) & 0xFFFFFFFF

    # 定义函数 `direct_call_refs`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def direct_call_refs(self, target_va: int):
        # 把右侧 `[]` 计算得到的值保存到 `refs`，后面的判断或输出会继续使用它。
        refs = []
        # 把右侧 `b'\xE8'` 计算得到的值保存到 `marker`，后面的判断或输出会继续使用它。
        marker = b'\xE8'
        # 开始循环 `section in self.sections`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for section in self.sections:
            # IMAGE_SCN_MEM_EXECUTE = 0x20000000
            # 检查条件 `not (section['characteristics'] & 0x20000000)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not (section['characteristics'] & 0x20000000):
                # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
                continue
            # 把右侧 `self.data[section['raw_ptr']:section['raw_ptr'] + section['raw_size']]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
            raw = self.data[section['raw_ptr']:section['raw_ptr'] + section['raw_size']]
            # 把右侧 `self.image_base + section['virtual_address']` 计算得到的值保存到 `base_va`，后面的判断或输出会继续使用它。
            base_va = self.image_base + section['virtual_address']
            # 把右侧 `raw.find(marker)` 计算得到的值保存到 `pos`，后面的判断或输出会继续使用它。
            pos = raw.find(marker)
            # 只要条件 `0 <= pos <= len(raw) - 5` 仍然成立，就重复执行下面的循环体；循环体必须最终改变条件或主动退出。
            while 0 <= pos <= len(raw) - 5:
                # 把右侧 `struct.unpack_from('<i', raw, pos + 1)[0]` 计算得到的值保存到 `rel`，后面的判断或输出会继续使用它。
                rel = struct.unpack_from('<i', raw, pos + 1)[0]
                # 把右侧 `base_va + pos` 计算得到的值保存到 `call_va`，后面的判断或输出会继续使用它。
                call_va = base_va + pos
                # 把右侧 `(call_va + 5 + rel) & 0xFFFFFFFF` 计算得到的值保存到 `dest`，后面的判断或输出会继续使用它。
                dest = (call_va + 5 + rel) & 0xFFFFFFFF
                # 检查条件 `dest == target_va`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if dest == target_va:
                    # 把这一条新结果追加到列表 `refs`；不会覆盖前面已经收集的记录。
                    refs.append(call_va)
                # 把右侧 `raw.find(marker, pos + 1)` 计算得到的值保存到 `pos`，后面的判断或输出会继续使用它。
                pos = raw.find(marker, pos + 1)
        # 把 `refs` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return refs

    # 定义函数 `direct_call_refs_many`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def direct_call_refs_many(self, target_vas):
        # 把右侧 `set(target_vas)` 计算得到的值保存到 `targets`，后面的判断或输出会继续使用它。
        targets = set(target_vas)
        # 把右侧 `{target: [] for target in targets}` 计算得到的值保存到 `refs`，后面的判断或输出会继续使用它。
        refs = {target: [] for target in targets}
        # 把右侧 `b'\xE8'` 计算得到的值保存到 `marker`，后面的判断或输出会继续使用它。
        marker = b'\xE8'
        # 开始循环 `section in self.sections`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for section in self.sections:
            # 检查条件 `not (section['characteristics'] & 0x20000000)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not (section['characteristics'] & 0x20000000):
                # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
                continue
            # 把右侧 `self.data[section['raw_ptr']:section['raw_ptr'] + section['raw_size']]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
            raw = self.data[section['raw_ptr']:section['raw_ptr'] + section['raw_size']]
            # 把右侧 `self.image_base + section['virtual_address']` 计算得到的值保存到 `base_va`，后面的判断或输出会继续使用它。
            base_va = self.image_base + section['virtual_address']
            # 把右侧 `raw.find(marker)` 计算得到的值保存到 `pos`，后面的判断或输出会继续使用它。
            pos = raw.find(marker)
            # 只要条件 `0 <= pos <= len(raw) - 5` 仍然成立，就重复执行下面的循环体；循环体必须最终改变条件或主动退出。
            while 0 <= pos <= len(raw) - 5:
                # 把右侧 `struct.unpack_from('<i', raw, pos + 1)[0]` 计算得到的值保存到 `rel`，后面的判断或输出会继续使用它。
                rel = struct.unpack_from('<i', raw, pos + 1)[0]
                # 把右侧 `base_va + pos` 计算得到的值保存到 `call_va`，后面的判断或输出会继续使用它。
                call_va = base_va + pos
                # 把右侧 `(call_va + 5 + rel) & 0xFFFFFFFF` 计算得到的值保存到 `dest`，后面的判断或输出会继续使用它。
                dest = (call_va + 5 + rel) & 0xFFFFFFFF
                # 检查条件 `dest in targets`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if dest in targets:
                    # 执行这一条实际代码步骤：`refs[dest].append(call_va)`；它与上下相邻语句共同完成当前函数的小任务。
                    refs[dest].append(call_va)
                # 把右侧 `raw.find(marker, pos + 1)` 计算得到的值保存到 `pos`，后面的判断或输出会继续使用它。
                pos = raw.find(marker, pos + 1)
        # 把 `refs` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return refs


# 定义函数 `hexva`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def hexva(value: int) -> str:
    # 把 `f'0x{value:08X}'` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return f'0x{value:08X}'


# 定义函数 `check_call`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def check_call(pe: PEImage, call_va: int, target_va: int, meaning: str):
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 把右侧 `pe.call_target(call_va)` 计算得到的值保存到 `actual`，后面的判断或输出会继续使用它。
        actual = pe.call_target(call_va)
        # 把右侧 `actual == target_va` 计算得到的值保存到 `ok`，后面的判断或输出会继续使用它。
        ok = actual == target_va
        # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {
            # 给当前结果字典的“类型”字段填写 `'直接调用', '地址': hexva(call_va), '期望目标': hexva(target_va),`，这样导出的 JSON/表格能保留这一项证据。
            '类型': '直接调用', '地址': hexva(call_va), '期望目标': hexva(target_va),
            # 给当前结果字典的“实际目标”字段填写 `hexva(actual), '含义': meaning, 'status': 'PASS' if ok else 'FAIL'`，这样导出的 JSON/表格能保留这一项证据。
            '实际目标': hexva(actual), '含义': meaning, 'status': 'PASS' if ok else 'FAIL'
        }
    # 捕获 `Exception as exc` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception as exc:
        # 把 `{'类型': '直接调用', '地址': hexva(call_va), '期望目标': hexva(target_va),` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {'类型': '直接调用', '地址': hexva(call_va), '期望目标': hexva(target_va),
                # 给当前结果字典的“含义”字段填写 `meaning, 'status': 'FAIL', 'error': str(exc)}`，这样导出的 JSON/表格能保留这一项证据。
                '含义': meaning, 'status': 'FAIL', 'error': str(exc)}


# 定义函数 `check_bytes`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def check_bytes(pe: PEImage, va: int, expected_hex: str, meaning: str):
    # 把右侧 `bytes.fromhex(expected_hex)` 计算得到的值保存到 `expected`，后面的判断或输出会继续使用它。
    expected = bytes.fromhex(expected_hex)
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 把右侧 `pe.read_va(va, len(expected))` 计算得到的值保存到 `actual`，后面的判断或输出会继续使用它。
        actual = pe.read_va(va, len(expected))
        # 把右侧 `actual == expected` 计算得到的值保存到 `ok`，后面的判断或输出会继续使用它。
        ok = actual == expected
        # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {
            # 给当前结果字典的“类型”字段填写 `'关键字节', '地址': hexva(va), '期望': expected.hex(' '),`，这样导出的 JSON/表格能保留这一项证据。
            '类型': '关键字节', '地址': hexva(va), '期望': expected.hex(' '),
            # 给当前结果字典的“实际”字段填写 `actual.hex(' '), '含义': meaning, 'status': 'PASS' if ok else 'FAIL'`，这样导出的 JSON/表格能保留这一项证据。
            '实际': actual.hex(' '), '含义': meaning, 'status': 'PASS' if ok else 'FAIL'
        }
    # 捕获 `Exception as exc` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception as exc:
        # 把 `{'类型': '关键字节', '地址': hexva(va), '期望': expected.hex(' '),` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {'类型': '关键字节', '地址': hexva(va), '期望': expected.hex(' '),
                # 给当前结果字典的“含义”字段填写 `meaning, 'status': 'FAIL', 'error': str(exc)}`，这样导出的 JSON/表格能保留这一项证据。
                '含义': meaning, 'status': 'FAIL', 'error': str(exc)}


# 定义函数 `cmd_verify_rpg`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_verify_rpg(args):
    # 把右侧 `Path(args.exe)` 计算得到的值保存到 `path`，后面的判断或输出会继续使用它。
    path = Path(args.exe)
    # 把右侧 `hashlib.sha256(path.read_bytes()).hexdigest()` 计算得到的值保存到 `digest`，后面的判断或输出会继续使用它。
    digest = hashlib.sha256(path.read_bytes()).hexdigest()
    # 把右侧 `PEImage(path)` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe = PEImage(path)
    # 把右侧 `[` 计算得到的值保存到 `checks`，后面的判断或输出会继续使用它。
    checks = [
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0044267C, 0x0041F2B0, 'BattleManager 首槽 FightRole 初始化调用'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443F39, 0x0041F2B0, '另一条 manager+slot*0xDE4+0x3E4 FightRole 初始化调用'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0041F47A, '89 34 85 94 FD 89 00', '将真实 FightRole* 写入 0x89FD94[slot]'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442E58, 0x00444240, 'AI 动作准备调用 Enemy_AI 三分区/候选入口'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442ECF, 0x00422F00, 'AI mode 1 路径准备 FIRTTECH'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442F4A, 0x00423BA0, 'AI mode 7 路径准备 Article/Ail2'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422F41, '89 86 A8 07 00 00', 'FightRole+0x7A8 写入选中 FIRTTECH ID'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422F4F, '89 8E 48 08 00 00', 'FightRole+0x848 写入选中 FIRTTECH record*'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00422F81, 0x00422CB0, 'FIRTTECH 准备后生成目标种子'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00422FF6, 0x00422850, 'FIRTTECH 准备后构造目标集合'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004321E5, '8B 88 B4 00 00 00', 'DataCenter+0xB4 读取 FIRTTECH base'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004321F1, '8B 90 AC 00 00 00', 'DataCenter+0xAC 读取 Ail2 base'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00423BAE, '89 86 F0 0C 00 00', 'Article 准备将 FightRole+0xCF0 写为 mode 7'),
        # 执行一次赋值：把 `Ail2 record*'),` 的结果放入左侧 `check_bytes(pe, 0x00423BD7, '89 8E F4 0C 00 00', 'Arti…` 对应的变量/字段。
        check_bytes(pe, 0x00423BD7, '89 8E F4 0C 00 00', 'Article 准备写 FightRole+0xCF4 = Ail2 record*'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00422B7C, 0x0042A7A0, 'FIRTTECH 类动作进入 FIRTTECH 处理包装器'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00422BC9, 0x0042A7D0, 'class 0xA 进入 Article/Ail2 处理器'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042A7B9, 0x0042C680, 'FIRTTECH 包装器直接转发到主处理器 0x42C680'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422C8D, '83 C3 50', '每目标 DamageInfo 指针步进 0x50'),
        # 执行一次赋值：把 `2 进入公式 helper 0x42B630'),` 的结果放入左侧 `check_call(pe, 0x0042C7D5, 0x0042B630, 'FIRTTECH+0x30 …` 对应的变量/字段。
        check_call(pe, 0x0042C7D5, 0x0042B630, 'FIRTTECH+0x30 selector=2 进入公式 helper 0x42B630'),
        # 执行一次赋值：把 `3 进入公式 helper 0x42B6C0'),` 的结果放入左侧 `check_call(pe, 0x0042C7E9, 0x0042B6C0, 'FIRTTECH+0x30 …` 对应的变量/字段。
        check_call(pe, 0x0042C7E9, 0x0042B6C0, 'FIRTTECH+0x30 selector=3 进入公式 helper 0x42B6C0'),
        # 执行一次赋值：把 `4 进入公式 helper 0x42B750'),` 的结果放入左侧 `check_call(pe, 0x0042C7FD, 0x0042B750, 'FIRTTECH+0x30 …` 对应的变量/字段。
        check_call(pe, 0x0042C7FD, 0x0042B750, 'FIRTTECH+0x30 selector=4 进入公式 helper 0x42B750'),
        # 执行一次赋值：把 `5 进入公式 helper 0x42B7D0'),` 的结果放入左侧 `check_call(pe, 0x0042C811, 0x0042B7D0, 'FIRTTECH+0x30 …` 对应的变量/字段。
        check_call(pe, 0x0042C811, 0x0042B7D0, 'FIRTTECH+0x30 selector=5 进入公式 helper 0x42B7D0'),
        # 执行一次赋值：把 `6 进入公式 helper 0x42B840'),` 的结果放入左侧 `check_call(pe, 0x0042C825, 0x0042B840, 'FIRTTECH+0x30 …` 对应的变量/字段。
        check_call(pe, 0x0042C825, 0x0042B840, 'FIRTTECH+0x30 selector=6 进入公式 helper 0x42B840'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C839, 0x0042B5F0, 'FIRTTECH+0x30 其他值进入默认公式 helper 0x42B5F0'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B5FA, 0x0042BA80, '默认公式读取 performer 派生标量 A'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B608, 0x0042BB50, '默认公式读取 target 派生标量 B'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B617, 0x0042BCD0, '默认公式进入随机差值组合 helper'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042BCFC, 0x0042CCB0, '随机差值 helper 调用通用随机区间函数'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042CCD9, 0x00451565, '通用随机区间函数使用全局 RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0045155B, '8B 44 24 04 A3 9C E2 46 00 C3', 'RNG 播种入口：直接写全局 state 0x46E29C'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00451565, 'A1 9C E2 46 00 69 C0 FD 43 03 00 05 C3 9E 26 00 A3 9C E2 46 00 C1 F8 10 25 FF 7F 00 00 C3', 'RNG LCG：*0x343FD +0x269EC3，返回 (state>>16)&0x7FFF'),
        # 固化22：固化21剩余11个非Battle-core RNG直接点的静态语义细分证据。
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004010BA, 0x00451583, '窗口创建初始化0x401000先取得时间类值'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004010C0, 0x0045155B, '窗口创建初始化0x401000使用该值重播种全局RNG'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00401109, 0x00451565, '窗口创建初始化首个随机字段使用全局RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040110E, '25 03 00 00 80 79 05 48 83 C8 FC 40', '0x401109结果按signed remainder语义压到0..3后写DataCenter-like对象+0xBC'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040113D, 0x00451565, '窗口创建初始化循环随机值使用全局RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00401142, '99 B9 55 00 00 00 F7 F9 42', '0x40113D结果除以0x55取余并+1'),
        # 执行一次赋值：把 `1持续到ESI<0x1F，共30次'),` 的结果放入左侧 `check_bytes(pe, 0x0040115F, '83 FE 1F 7C D7', '窗口创建初始化…` 对应的变量/字段。
        check_bytes(pe, 0x0040115F, '83 FE 1F 7C D7', '窗口创建初始化随机循环从ESI=1持续到ESI<0x1F，共30次'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00401A23, 0x00401000, '窗口消息分派的message==1分支调用初始化0x401000'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004019AA, '8B C8 49 74 38 49 0F 85 A3 01 00 00', '窗口消息分派对message减1；message==1进入0x4019E7创建分支'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040352E, 0x00451565, '随机遭遇阈值初始化使用全局RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00403524, 'C7 05 10 F6 46 00 96 00 00 00', '随机遭遇基础阈值写为150'),
        # 执行一次赋值：把 `150时形成RNG%300'),` 的结果放入左侧 `check_bytes(pe, 0x00403533, '8B 0D 10 F6 46 00 99 8D 3…` 对应的变量/字段。
        check_bytes(pe, 0x00403533, '8B 0D 10 F6 46 00 99 8D 34 09 F7 FE', '随机遭遇阈值用2*base作为除数；base=150时形成RNG%300'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00403718, 0x00451565, '随机遭遇阈值续订使用全局RNG'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004093C2, 0x00403510, 'Map/SCI装载链把对应尾字段送入随机遭遇阈值初始化'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040B158, 0x004035A0, 'world update调用随机遭遇阈值运行更新函数'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040AAF7, '80 78 76 03', 'Entity/NPC运行函数仅在序列化实体record+0x76==3时进入该随机行为'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040AB72, 0x00451565, 'Entity/NPC behavior type 3 的X范围随机'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040AB86, 0x00451565, 'Entity/NPC behavior type 3 的Y范围随机'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040AB9A, 0x00451565, 'Entity/NPC behavior type 3 的运行态0/1随机'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040ABD3, 0x00451565, 'Entity/NPC behavior type 3 的等待阈值随机'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040AB4E, '8B B8 F7 00 00 00 8B 90 F3 00 00 00 55 8B A8 EF 00 00 00 2B FD 8B A8 FB 00 00 00 2B EA', '漫游随机区间由record+0xEF/+0xF3/+0xF7/+0xFB四个边界字段派生'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040F633, 0x00451565, 'EVE opcode 0x99 RANDOM_TO_VAR999使用全局RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040F624, '8B 47 04 89 47 08 8B 70 04 8B 48 08 2B F1 46', 'EVE 0x99从命令+4/+8形成inclusive区间宽度(end-start+1)'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040F641, '52 68 E7 03 00 00 E8 64 BA 03 00', 'EVE 0x99将随机结果与变量索引999(0x3E7)送入变量写入helper'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0044B52C, 0x00451565, 'EVE相机jitter X偏移使用全局RNG'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0044B54D, 0x00451565, 'EVE相机jitter Y偏移使用全局RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044B531, '8B 0D 40 85 97 00 99 8D 34 09 F7 FE 2B D1', '相机jitter X按RNG%(2*radius)-radius生成偏移'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044B552, '8B 0D 40 85 97 00 99 8D 34 09 F7 FE', '相机jitter Y同样按2*radius做随机除数'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040B05D, 0x0044B500, '主更新/渲染链调用相机jitter apply函数0x44B500'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040E13D, 0x0044B4B0, 'EVE opcode 0x58处理器调用相机jitter配置setter'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442076, 0x00451583, 'Battle 初始化取得系统时间转换值作为 RNG seed 来源'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0044207C, 0x0045155B, 'Battle 初始化重新播种 RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004445D4, '8B 77 38', 'AI候选检查读取 FIRTTECH+0x38 次资源消耗量'),
        # 执行一次赋值：把 `FightRole+0x884 次资源当前值'),` 的结果放入左侧 `check_bytes(pe, 0x004445DA, '8B 8C 93 68 0C 00 00', 'A…` 对应的变量/字段。
        check_bytes(pe, 0x004445DA, '8B 8C 93 68 0C 00 00', 'AI候选检查读取 raw-slot+0xC68 = FightRole+0x884 次资源当前值'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004445F2, '8B 4F 34', 'AI候选检查读取 FIRTTECH+0x34 主生存资源消耗量'),
        # 执行一次赋值：把 `FightRole+0x87C 主生存资源当前值'),` 的结果放入左侧 `check_bytes(pe, 0x004445EC, '8B 92 60 0C 00 00', 'AI候选…` 对应的变量/字段。
        check_bytes(pe, 0x004445EC, '8B 92 60 0C 00 00', 'AI候选检查读取 raw-slot+0xC60 = FightRole+0x87C 主生存资源当前值'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421012, '2B 43 FC', 'DamageInfo+0x04 从目标主生存资源当前值扣除'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421024, '29 88 84 08 00 00', 'DamageInfo+0x08 从目标次资源当前值扣除'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00421043, 0x004231F0, '主生存资源 current<=0 时进入零值失效类状态转移'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421054, '89 91 7C 08 00 00', '主生存资源 current 上限 clamp 到 FightRole+0x878'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004210B8, '89 86 84 08 00 00', '次资源 current 上限 clamp 到 FightRole+0x880'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421249, '8B 48 38', '动作落地读取 FIRTTECH+0x38 并扣次资源'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421264, '8B 50 34', '动作落地读取 FIRTTECH+0x34 并扣主生存资源（ID 0x12F 特例除外）'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042129B, '8B 88 18 01 00 00', 'Article/Ail2+0x118 作为主生存资源扣除量'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004212AF, '8B 90 1C 01 00 00', 'Article/Ail2+0x11C 作为次资源扣除量'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00423218, 'C7 86 7C 08 00 00 00 00 00 00', '主资源零值状态转移将 FightRole+0x87C 清零'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040E6F2, 0x0043B8F0, 'EVE opcode 0x7C 进入队伍双资源 current/max 调整函数'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043B967, '8B 50 2C 8B 78 30', 'EVE资源调整读取主资源 max +0x2C / current +0x30'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043B971, '89 50 30', 'EVE资源调整将主 current clamp 到主 max'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043B9B8, '8B 50 34 8B 78 38', 'EVE资源调整读取次资源 max +0x34 / current +0x38'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043B9C2, '89 50 38', 'EVE资源调整将次 current clamp 到次 max'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B126, '8B 44 99 64 8B 54 99 78', '五槽处理按 slot 读取 FIRTTECH+0x64 数值与 +0x78 ID 两组平行字段'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B514, '89 54 9F 10', '五槽处理写 DamageInfo+0x10+slot*4 effect-like ID'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B528, '89 54 9F 24', '五槽处理写 DamageInfo+0x24+slot*4 计算值'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B53A, '8B 14 CD 68 A6 46 00', 'effect-like ID 通过 0x46A668 的 8-byte 映射表取得 Role runtime 槽索引'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B54D, '8B 84 96 E8 01 00 00', '映射后读取 target RoleDefinition+0x1E8+index*4'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B57E, '83 FB 05', '五槽循环固定边界 slot<5'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B050, 0x0042B0D0, 'Article/Ail2 processor 路径调用五槽处理器'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042CBE9, 0x0042B0D0, 'FIRTTECH processor 路径调用五槽处理器'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042A89F, '8B 5C 24 30', 'Article/Ail2 processor 将第4参数保存到 EBX，后续作为五槽处理 arg4'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B318, '8B 44 24 40 89 7C 98 10', '五槽 ID>27 分支直接把原 ID 写入 DamageInfo ID 槽'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B320, 'C7 44 98 24 40 1F 00 00', '五槽 ID>27 分支把 DamageInfo 值固定写为8000'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0041CD79, '8B 86 E8 01 00 00', '表现层独立 consumer 读取 RoleDefinition+0x1E8'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0041CD89, '6A 01', 'RoleDefinition+0x1E8>0 分支向表现层对象传 selector code 1'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042CC5D, '8B 8F E8 01 00 00', 'FIRTTECH processor 尾部读取 RoleDefinition+0x1E8'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042CC64, '89 4E 44', '将 RoleDefinition+0x1E8 传播写入 DamageInfo+0x44'),
        # 执行一次赋值：把 `DamageInfo+0x04，因此这里实际读取 DamageInfo+0x48 低字节'),` 的结果放入左侧 `check_bytes(pe, 0x00421518, '8A 4B 44', 'mode2 工作指针` 对应的变量/字段。
        check_bytes(pe, 0x00421518, '8A 4B 44', 'mode2 工作指针=DamageInfo+0x04，因此这里实际读取 DamageInfo+0x48 低字节'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00421521, 0x00420E20, 'DamageInfo+0x48 条件参数进入关联单位/状态后处理判定 0x420E20'),
        # 执行一次赋值：把 `FightRole+0x170'),` 的结果放入左侧 `check_bytes(pe, 0x00422B27, '8D 9E 70 01 00 00', '每目标 …` 对应的变量/字段。
        check_bytes(pe, 0x00422B27, '8D 9E 70 01 00 00', '每目标 DamageInfo 真实起点 = FightRole+0x170'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422C3A, '8D 6B 24', '五槽第一阶段从 DamageInfo+0x24 residual/value 数组开始'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422C4D, '83 F8 1B', '五槽 pending application 对 raw ID 做 signed <=27 边界检查'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422C67, '89 BC 81 CC 00 00 00', '五槽 residual 写入目标 pending 数组 FightRole+0xCC+raw_id*4'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0041F284, '8D 8E CC 00 00 00', 'FightRole reset 从 pending 数组 +0xCC 开始初始化'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0041F297, '83 F8 27', 'FightRole reset 固定清零 pending 39 DWORD'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042160D, 'B8 CC 00 00 00', 'mode2 从 pending +0xCC 开始第二阶段提交扫描'),
        # 执行一次赋值：把 `0xA34'),` 的结果放入左侧 `check_bytes(pe, 0x0042162A, '89 94 01 68 09 00 00', 'p…` 对应的变量/字段。
        check_bytes(pe, 0x0042162A, '89 94 01 68 09 00 00', 'pending offset+0x968 一对一写入 runtime；0xCC+0x968=0xA34'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421638, 'C7 04 02 00 00 00 00', 'pending 成功提交后清零源槽'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00420662, '8D BE 34 0A 00 00', 'Battle per-role update 从 runtime +0xA34 开始扫描'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00420681, '83 C1 F6', 'runtime 正值递减固定 10'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00420694, '83 FA 27', 'runtime update 固定扫描 39 DWORD'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442CDB, 0x00420440, 'Battle manager per-role update 调用 runtime 递减所在 0x420440'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004231D1, '8D B9 34 0A 00 00', 'runtime 前28 raw effect-ID 槽清除起点 +0xA34'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004231D7, 'B9 1C 00 00 00', '0x4231D0 固定清除 28 DWORD'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004216C7, '8B 7E 20', 'Article/Ail2 状态解除分派读取 selected Ail2+0x20'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004216DB, '8A 86 90 19 42 00', 'Article/Ail2+0x20 通过 0x421990 byte selector table 分派'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421BA9, '8D B1 D1 FE FF FF', 'FIRTTECH 特殊分派以 selected ID-0x12F 为表索引'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421BBD, '8A 96 74 20 42 00', 'FIRTTECH 特殊分派读取 0x422074 byte selector table'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004201F7, '8B 91 2C 07 00 00 85 D2 7D 61', 'Article目标为inactive/defeated类状态时进入复苏物品例外检查'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00420201, '8B 40 20 3D 0F 01 00 00 74 57 3D 10 01 00 00 74 50 3D 11 01 00 00 74 49', 'Ail2+0x20 的0x10F/0x110/0x111允许inactive目标继续'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B055, '81 7B 20 08 01 00 00', 'Article processor 按 Ail2+0x20 特判0x108'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B071, '3D 0F 01 00 00 74 0E 3D 10 01 00 00 74 07 3D 11 01 00 00 75 0B', 'Article processor 按 Ail2+0x20 特判复苏物品0x10F/110/111'),
        # 执行一次赋值：把 `1 并令 target RoleDefinition+0x30=1'),` 的结果放入左侧 `check_bytes(pe, 0x0042B086, 'C6 47 40 01 C7 46 30 01 0…` 对应的变量/字段。
        check_bytes(pe, 0x0042B086, 'C6 47 40 01 C7 46 30 01 00 00 00', '复苏物品路径置 DamageInfo+0x40=1 并令 target RoleDefinition+0x30=1'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422630, '8B 41 20 3D 0F 01 00 00', '动作流再次读取 Ail2+0x20 并从0x10F开始复苏族判定'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043798D, '3D 2D 01 00 00 7C 07 3D 33 01 00 00 7E 2C', '物品菜单逻辑明确识别 Ail2 ID 0x12D..0x133 连续特殊范围'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00469F2C, '50 75 62 6C 69 63 5C 41 50 49 2E 45 4E 43 00', '初始化器引用的文件名字符串 Public\\API.ENC'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00439E52, '68 2C 9F 46 00', 'DataCenter 初始化器打开 Public\\API.ENC'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00439E9A, '89 96 A0 00 00 00', 'API.ENC 几何验证后写 DataCenter+0xA0 record count'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00439EAA, '89 86 A4 00 00 00', 'API.ENC 分配缓冲区写 DataCenter+0xA4'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00439EC0, 0x004284D0, 'API.ENC 读入后调用公共 ENC 解码/处理函数'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0046BAF4, '50 75 62 6C 69 63 5C 41 69 6C 32 2E 45 4E 43 00', '初始化器引用的文件名字符串 Public\\Ail2.ENC'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00439F2B, '89 86 AC 00 00 00', 'Ail2.ENC 分配缓冲区写 DataCenter+0xAC'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00441FDD, '8B 88 A4 00 00 00', 'Battle 同步路径读取 DataCenter+0xA4 API 主表'),
        # 执行一次赋值：把 `0x350 bytes 回 API 主表'),` 的结果放入左侧 `check_bytes(pe, 0x00441FEA, 'B9 D4 00 00 00 F3 A5', 'B…` 对应的变量/字段。
        check_bytes(pe, 0x00441FEA, 'B9 D4 00 00 00 F3 A5', 'Battle 将 FightRole 内嵌 RoleDefinition 复制 0xD4 DWORD=0x350 bytes 回 API 主表'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043AEFC, '8B 83 AC 00 00 00', '物品应用函数从 DataCenter+0xAC 取得 Ail2 主表'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043AF05, '8B 44 78 28', '物品应用函数读取 Ail2+0x28 特殊动作码'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043AF10, '89 83 C4 00 00 00', 'Ail2+0x28>0 时写 DataCenter+0xC4 特殊动作码'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043AF1A, '89 8B EC 00 00 00', '特殊动作路径将 item ID 写 DataCenter+0xEC 并提前返回'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043AFE3, '8B 83 A4 00 00 00', '物品应用 writer 从 DataCenter+0xA4 取得 API 主表'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043B000, '8B 17 8D 72 D5 83 FE 11', '物品五槽 effect ID 转为 43..60 switch 索引'),
        # 执行一次赋值：把 `1 并落入 +0x3C += Ail2+0xD0'),` 的结果放入左侧 `check_bytes(pe, 0x0043B019, 'C7 81 9C 00 00 00 01 00 0…` 对应的变量/字段。
        check_bytes(pe, 0x0043B019, 'C7 81 9C 00 00 00 01 00 00 00 01 41 3C', 'effect ID43 特殊写 RoleDefinition+0x9C=1 并落入 +0x3C += Ail2+0xD0'),
        # 执行一次赋值：把 `Ail2+0xD0'),` 的结果放入左侧 `check_bytes(pe, 0x0043B028, '01 41 40', 'effect ID55 w…` 对应的变量/字段。
        check_bytes(pe, 0x0043B028, '01 41 40', 'effect ID55 writer: RoleDefinition+0x40 += Ail2+0xD0'),
        # 执行一次赋值：把 `Ail2+0xD0'),` 的结果放入左侧 `check_bytes(pe, 0x0043B02D, '01 41 48', 'effect ID56 w…` 对应的变量/字段。
        check_bytes(pe, 0x0043B02D, '01 41 48', 'effect ID56 writer: RoleDefinition+0x48 += Ail2+0xD0'),
        # 执行一次赋值：把 `Ail2+0xD0'),` 的结果放入左侧 `check_bytes(pe, 0x0043B032, '01 41 4C', 'effect ID57 w…` 对应的变量/字段。
        check_bytes(pe, 0x0043B032, '01 41 4C', 'effect ID57 writer: RoleDefinition+0x4C += Ail2+0xD0'),
        # 执行一次赋值：把 `Ail2+0xD0'),` 的结果放入左侧 `check_bytes(pe, 0x0043B037, '01 41 44', 'effect ID58 w…` 对应的变量/字段。
        check_bytes(pe, 0x0043B037, '01 41 44', 'effect ID58 writer: RoleDefinition+0x44 += Ail2+0xD0'),
        # 执行一次赋值：把 `Ail2+0xD0'),` 的结果放入左侧 `check_bytes(pe, 0x0043B03C, '01 41 2C', 'effect ID59 w…` 对应的变量/字段。
        check_bytes(pe, 0x0043B03C, '01 41 2C', 'effect ID59 writer: RoleDefinition+0x2C += Ail2+0xD0'),
        # 执行一次赋值：把 `Ail2+0xD0'),` 的结果放入左侧 `check_bytes(pe, 0x0043B041, '01 41 34', 'effect ID60 w…` 对应的变量/字段。
        check_bytes(pe, 0x0043B041, '01 41 34', 'effect ID60 writer: RoleDefinition+0x34 += Ail2+0xD0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040CE23, 'A1 48 1C 8E 00 8B 88 EC 00 00 00', '主流程以 DataCenter+0xEC>0 作为特殊物品 action pending gate'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040CE36, '8B 80 C4 00 00 00 83 C0 FD 83 F8 0E', '特殊物品 action 读取 DataCenter+0xC4，按 code-3 分派 3..17'),
        # 执行一次赋值：把 `1500，mode flag=BL(本函数BL=1)'),` 的结果放入左侧 `check_bytes(pe, 0x0040CF60, 'C7 05 1C F6 46 00 DC 05 0…` 对应的变量/字段。
        check_bytes(pe, 0x0040CF60, 'C7 05 1C F6 46 00 DC 05 00 00 88 1D 6D 84 46 00', 'action code13 handler：duration=1500，mode flag=BL(本函数BL=1)'),
        # 执行一次赋值：把 `1500，mode flag=0'),` 的结果放入左侧 `check_bytes(pe, 0x0040CF2D, 'C7 05 1C F6 46 00 DC 05 0…` 对应的变量/字段。
        check_bytes(pe, 0x0040CF2D, 'C7 05 1C F6 46 00 DC 05 00 00 C6 05 6D 84 46 00 00', 'action code14 handler：duration=1500，mode flag=0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004034DD, '8B 0D 1C F6 46 00', '持续效果 updater 读取 0x46F61C duration limit'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004034EC, '40 3B C1 A3 20 F6 46 00', '持续效果 elapsed counter 递增并与 duration 比较'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00403552, 'A0 6D 84 46 00 84 C0', '遇敌阈值生成读取 mode flag 0x46846D'),
        # 执行一次赋值：把 `1 时将随机阈值放大为 2×'),` 的结果放入左侧 `check_bytes(pe, 0x0040355B, '8D 04 12', 'mode flag` 对应的变量/字段。
        check_bytes(pe, 0x0040355B, '8D 04 12', 'mode flag=1 时将随机阈值放大为 2×'),
        # 执行一次赋值：把 `0 时进入整数 /3 阈值缩放路径'),` 的结果放入左侧 `check_bytes(pe, 0x0040356E, 'B8 56 55 55 55 F7 EA', 'm…` 对应的变量/字段。
        check_bytes(pe, 0x0040356E, 'B8 56 55 55 55 F7 EA', 'mode flag=0 时进入整数 /3 阈值缩放路径'),
        # D7: Battle outcome / victory reward / persistent writeback.
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004328B2, 0x00443590, '高层 Battle mode update 调用胜负/结束判定器 0x443590'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004328BE, '8A 88 FC 05 00 00 84 C9 74 05 BF 03 00 00 00', '外部 +0x5FC flag 可把高层 outcome 强制为 3'),
        # 执行一次赋值：把 `4 在此被翻译为 dispatcher=1'),` 的结果放入左侧 `check_call(pe, 0x00432908, 0x00443660, '高层 outcome 1/4…` 对应的变量/字段。
        check_call(pe, 0x00432908, 0x00443660, '高层 outcome 1/4 分支以参数1调用结果分派器；注意 detector=4 在此被翻译为 dispatcher=1'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00432929, 0x00443660, '高层 outcome 2 以参数2调用结果分派器'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00432948, 0x00443660, '高层 outcome 3 以参数3调用结果分派器'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004329F7, 0x00441F60, '结果UI/倒计时结束后调用 Battle→API 0x350整记录回写'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044359C, '8D 87 34 0C 00 00 BA 10 00 00 00', '胜负判定从 raw-slot+0xC34 起扫描16槽'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004435A7, '83 B8 DC FE FF FF FF', '胜负计数先要求 raw-slot+0xB10 = FightRole+0x72C != -1'),
        # 执行一次赋值：把 `FightRole+0xA74 为0；操偶状态被排除'),` 的结果放入左侧 `check_bytes(pe, 0x004435B0, '8B 88 24 02 00 00 85 C9 7…` 对应的变量/字段。
        check_bytes(pe, 0x004435B0, '8B 88 24 02 00 00 85 C9 75 0F', '胜负计数要求 raw-slot+0xE58 = FightRole+0xA74 为0；操偶状态被排除'),
        # 执行一次赋值：把 `0/1 分别累计两侧有效单位数量'),` 的结果放入左侧 `check_bytes(pe, 0x004435BA, '8B 08 85 C9 75 03 46 EB 0…` 对应的变量/字段。
        check_bytes(pe, 0x004435BA, '8B 08 85 C9 75 03 46 EB 06 83 F9 01 75 01 45', 'FightRole+0x850=0/1 分别累计两侧有效单位数量'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044362D, '5F 5E 5D C6 05 DC 96 46 00 00 B8 02 00 00 00', '侧1计数为0时 detector 返回2'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443642, '5F 5E 5D C6 05 DC 96 46 00 00 B8 01 00 00 00', '侧0计数为0时 detector 返回1'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004435D1, 'A1 48 1C 8E 00 83 B8 F4 00 00 00 01', 'DataCenter+0xF4==1 进入 detector 的特殊结束请求分支'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443622, 'B8 04 00 00 00', '特殊结束请求分支 detector 返回4'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443ACB, '83 F8 02', '结果分派器支持参数2分支'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443AE2, '83 F8 03', '结果分派器支持参数3分支'),
        # 执行一次赋值：把 `4原样传入'),` 的结果放入左侧 `check_bytes(pe, 0x00443AF9, '83 F8 04', '结果分派器代码中还存在参数…` 对应的变量/字段。
        check_bytes(pe, 0x00443AF9, '83 F8 04', '结果分派器代码中还存在参数4分支；当前已知高层E8调用不会把detector=4原样传入'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0046C56C, '20 20 20 20 20 20 A5 E6 20 20 BE D4 20 20 A5 FE 20 20 B3 D3 20 20 20 20 20 20 20', '原版CP950结果文本：交戰全勝'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0046C514, '20 20 20 20 20 20 B1 D1 20 20 B0 7D 20 20 A4 A7 20 20 BE D4 20 20 20 20 20 20 00', '原版CP950结果文本：敗陣之戰'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0046C4F8, '20 20 20 20 20 20 A5 FE 20 20 B6 A4 20 20 B2 E6 20 20 B0 6B 20 20 20 20 20 20 00', '原版CP950结果文本：全隊脫逃'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0046C4DC, '20 20 20 20 20 20 A6 5D 20 20 A8 C6 20 20 B0 B1 20 20 BE D4 20 20 20 20 20 20 00', '原版CP950结果文本：因事停戰（dispatcher参数4分支文本）'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004423B2, '8D 85 31 E3 00 00 50', 'Battle 初始化把 manager+0xE331 作为 LayoutTeam 508-byte payload 输出缓冲'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004423CB, 0x00424430, 'Battle 初始化调用 LayoutGR→LayoutTeam 选择/整记录复制 helper'),
        # 执行一次赋值：把 `508 bytes完整 LayoutTeam record'),` 的结果放入左侧 `check_bytes(pe, 0x00424519, 'B9 7F 00 00 00 F3 A5', '0…` 对应的变量/字段。
        check_bytes(pe, 0x00424519, 'B9 7F 00 00 00 F3 A5', '0x424430 复制0x7F DWORD=508 bytes完整 LayoutTeam record'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443703, '8B B2 31 E3 00 00', '胜利结算读取 LayoutTeam+0x00 奖励源ID数量'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443718, '81 C2 35 E3 00 00', '胜利结算把 LayoutTeam+0x04 作为奖励源API ID列表起点'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443736, '8B 6C 39 24 03 DD', '按奖励源API ID读取 RoleDefinition+0x24 并累计胜利EXP'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443853, '8B 47 24 03 C3 89 47 24', '每个合格我方 RoleDefinition+0x24 累加胜利EXP总量'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443B6B, '8B 46 20 8B 0D F8 23 8E 00', '升级helper用 RoleDefinition+0x20 索引 LevelUp.ENC 阈值表'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443B9B, '8B 4E 24 57 3B 0C 82', '升级helper比较 RoleDefinition+0x24 累计EXP 与 LevelUp[level]'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443BAC, '40 89 46 20', '达到阈值后 RoleDefinition+0x20 等级索引+1'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443747, '8B 82 A9 E3 00 00', '胜利銀兩读取 LayoutTeam+0x78 随机范围'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443759, 0x00451565, '勝利銀兩随机范围非0时调用全局RNG'),
        # 执行一次赋值：把 `LayoutTeam+0x74基础值 + RNG余数'),` 的结果放入左侧 `check_bytes(pe, 0x00443769, '8B AE A5 E3 00 00 6A 01 0…` 对应的变量/字段。
        check_bytes(pe, 0x00443769, '8B AE A5 E3 00 00 6A 01 03 EA', '勝利銀兩=LayoutTeam+0x74基础值 + RNG余数'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0046C540, 'C0 F2 B1 6F AA F7 BF FA 23 63 25 78 20 25 64 23 63 25 78 20 A8 E2 00', '原版CP950文本直接标示“獲得金錢 ... 兩”'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004439E6, 0x00439820, '胜利銀兩通过既有 money-add helper 写入 Bank+0x5DD8'),
        # 执行一次赋值：把 `amount'),` 的结果放入左侧 `check_bytes(pe, 0x00439820, '8B 44 24 04 8B 91 D8 5D 0…` 对应的变量/字段。
        check_bytes(pe, 0x00439820, '8B 44 24 04 8B 91 D8 5D 00 00 03 D0 89 91 D8 5D 00 00 C2 04 00', '0x439820 精确执行 Bank+0x5DD8 += amount'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004439EB, '8B 86 AD E3 00 00', '胜利掉落读取 LayoutTeam+0x7C 掉落项数量'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443A03, '81 C6 B1 E3 00 00', '掉落ID数组起点 LayoutTeam+0x80（manager+0xE3B1）'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443A12, '8B 46 10', '每个掉落槽读取平行阈值数组 +0x10，即 LayoutTeam+0x90..+0x9C'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443A1D, 0x00451565, '掉落阈值>0时调用全局RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443A23, 'B9 64 00 00 00 F7 F9 3B 56 10 7F', '掉落严格执行 RNG%100 <= threshold；等于阈值仍命中'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443A35, '8B 06 3B 81 A8 00 00 00', '掉落物品ID必须小于 DataCenter+0xA8 Ail2 record count'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443A50, '8B 81 AC 00 00 00', '掉落物品名/记录从 DataCenter+0xAC Ail2 主表取得'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443A99, 0x00439120, '命中掉落以数量1调用库存添加 helper 0x439120'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421FED, '39 9F C8 00 00 00 7E 42', 'FIRTTECH特殊动作0x1D0分支要求至少一个目标'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422002, 'C7 81 7C 08 00 00 01 00 00 00', '幽冥返召特殊分支把首目标主生存资源 current 设为1'),
        # 执行一次赋值：把 `1000000 操偶状态'),` 的结果放入左侧 `check_bytes(pe, 0x00422016, 'C7 82 74 0A 00 00 40 42 0…` 对应的变量/字段。
        check_bytes(pe, 0x00422016, 'C7 82 74 0A 00 00 40 42 0F 00', '幽冥返召特殊分支写 FightRole+0xA74=1000000 操偶状态'),
        # 固化16：EVE opcode0x37 / 伏龍A特殊结束 / LayoutGR候选结构
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D412, '81 78 04 82 00 00 00', 'opcode0x37比较raw record+0x04 LayoutGR索引是否为0x82'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040D41B, 0x00451565, '伏龍A路径调用全局RNG初始化特殊结束计数器'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D421, 'B9 90 01 00 00 F7 F9', '伏龍A路径以400为除数取RNG余数'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D433, '89 90 F4 00 00 00', '伏龍A把800..1199写DataCenter+0xF4'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D441, '89 99 F4 00 00 00', '非伏龍A Battle命令把DataCenter+0xF4清0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D450, '8B 42 04 89 81 D8 00 00 00', 'opcode0x37把record+0x04写DataCenter+0xD8 LayoutGR索引'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00432596, 0x00424430, 'Fight对象建立读取DataCenter+0xD8并调用LayoutGR选择helper'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00424497, '8B 7C 28 18', '0x424430读取LayoutGR+0x18作为LayoutTeam候选数量'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004244EC, 0x00451565, 'LayoutTeam候选数量非零时调用RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00424508, '8B 44 8A 1C', '0x424430按随机索引从LayoutGR+0x1C数组读取实际LayoutTeam ID'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00441D41, 'C7 06 D4 10 46 00', 'Battle对象构造写虚表0x4610D4'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004610D4, '20 29 44 00', 'Battle虚表首项直接指向0x442920主更新入口'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00441DB2, 'C6 05 DC 96 46 00 01', 'Battle构造将0x4696DC progression/action gate初始化为1'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00441DFA, 'C6 05 0C 24 8E 00 01', 'Battle构造将0x8E240C active gate初始化为1'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00442920, 'A0 0C 24 8E 00', 'Battle主更新入口先读取active gate 0x8E240C'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044293D, '8B 81 F4 00 00 00 3B C3 7E 11 8A 15 DC 96 46 00 84 D2 74 07 48 89 81 F4 00 00 00', 'Battle主更新：+0xF4>1且0x4696DC非0时计数减1'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004435D6, '83 B8 F4 00 00 00 01 75 0A 33 F6 B3 01 89 B0 F4 00 00 00', '特殊计数到1时清敌方正常存活计数、置特殊flag并清计数器'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443622, 'B8 04 00 00 00', '特殊flag优先令detector返回4'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004328DB, '83 FF 01 74 0F 83 FF 02 74 0A 83 FF 04 74 05 83 FF 03', '高层Battle结果状态机显式接受detector 1/2/4/3'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00432908, 0x00443660, 'detector 1或4最终均以dispatcher参数1进入胜利结果分派'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00432913, 'C7 81 18 01 00 00 01 00 00 00', 'detector 1/4胜利路径把DataCenter+0x118写1'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D262, '8B 45 08 39 58 08', 'opcode0x37败北路径读取record+0x08固定MP0000/Event11开关'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D315, '8B 40 0C 55 A3 10 F8 89 00', 'opcode0x37另一败北路径读取record+0x0C作为Event目标指令'),
        # 固化17：D7结果界面双层活动门 / FANIN转场 / 延迟提交生命周期
        # 执行一次赋值：把 `15'),` 的结果放入左侧 `check_bytes(pe, 0x00432110, 'C7 86 44 02 00 00 0F 00 0…` 对应的变量/字段。
        check_bytes(pe, 0x00432110, 'C7 86 44 02 00 00 0F 00 00 00', 'Battle结果容器构造初始化提交延迟计数+0x244=15'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043211A, '88 9E 48 02 00 00', 'Battle结果容器构造将FANIN转场已启动标志+0x248清0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004322DA, '68 1C 06 00 00', 'Battle初始化为全局结果UI分配0x61C字节对象'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004322F9, 0x00411FB0, 'Battle初始化调用结果UI外层对象构造0x411FB0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043230F, 'A3 C8 FC 89 00', '将外层战斗结果UI对象写入全局0x89FCC8'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00411FE6, 'C7 45 00 90 07 46 00', '结果UI外层对象写虚表0x460790'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004607A8, '90 22 41 00', '结果UI外层虚表+0x18直接指向更新函数0x412290'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00412022, '88 9D 79 05 00 00', '结果UI外层构造将活动/过渡标志+0x579清0'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00412073, 0x00412570, '结果UI外层构造创建结果明细子界面0x412570'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00412082, '89 85 00 06 00 00', '结果UI外层把明细子界面指针保存到+0x600'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0041246A, 'C6 81 79 05 00 00 01', '0x412440启动外层结果UI时将+0x579置1'),
        # 执行一次赋值：把 `2完成后清+0x579'),` 的结果放入左侧 `check_bytes(pe, 0x00412368, '83 BE 18 06 00 00 02 75 0…` 对应的变量/字段。
        check_bytes(pe, 0x00412368, '83 BE 18 06 00 00 02 75 06 88 9E 79 05 00 00', '外层结果UI内部state=2完成后清+0x579'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004125A6, 'C7 06 B0 07 46 00', '结果明细子界面写虚表0x4607B0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004607C8, 'A0 2C 41 00', '结果明细子界面虚表+0x18直接指向更新函数0x412CA0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004125E1, '88 9E 79 05 00 00', '结果明细子界面构造将自身+0x579活动标志清0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004133EA, 'C6 81 79 05 00 00 01', '0x4133C0启动结果明细子界面时将+0x579置1'),
        # 执行一次赋值：把 `2完成后清+0x579'),` 的结果放入左侧 `check_bytes(pe, 0x00412F60, '83 BE 44 06 00 00 02 75 0…` 对应的变量/字段。
        check_bytes(pe, 0x00412F60, '83 BE 44 06 00 00 02 75 06 88 9E 79 05 00 00', '结果明细子界面内部state=2完成后清+0x579'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043295A, '83 FF 04 74 0B', 'detector4特殊胜利明确跳过正常结果UI启动调用'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00432965, 0x00412440, '普通detector1/2/3结果路径启动外层结果UI'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00432978, 'A1 C8 FC 89 00 8A 88 79 05 00 00 84 C9', 'Battle提交前先等待外层结果UI+0x579归零'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043298B, '8B 88 00 06 00 00 8A 81 79 05 00 00 84 C0', 'Battle提交前再等待+0x600明细子界面+0x579归零'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0046A498, '46 69 67 68 74 5C 4F 74 68 65 72 5C 46 41 4E 49 4E 2E 53 46 32 00', '结果界面结束后使用作者资源Fight\\Other\\FANIN.SF2'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004329CD, 0x0042D4D0, '双结果UI结束后通过Effect Manager创建FANIN转场效果'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004329DB, 0x0042D6E0, 'FANIN effect slot创建后立即交给0x42D6E0启动/配置'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004329E0, 'C6 86 48 02 00 00 01', 'FANIN创建后把Battle container+0x248置1，保证一次性启动'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004329E7, '8B 86 44 02 00 00 85 C0 7F 22', 'FANIN之后检查+0x244延迟计数，>0时走递减而不提交'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00432A13, '48 89 86 44 02 00 00', '+0x244每次合格Battle container update递减1'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004329F7, 0x00441F60, '结果UI/FANIN/延迟全部完成后整记录回写API主表'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00432A02, 0x00432130, 'API回写后销毁Battle container'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D4A5, '68 4C 02 00 00', 'EVE opcode0x37首次启动时分配0x24C字节Battle container'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040D4C0, 0x00432090, 'EVE opcode0x37调用Battle container构造0x432090'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D4D3, 'A3 F0 01 8B 00', 'EVE opcode0x37把Battle container写入全局0x8B01F0'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040D4D8, 0x004322C0, 'Battle container建立后调用0x4322C0初始化结果UI等Battle子系统'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0040D24F, '8B 0D 48 1C 8E 00 83 B9 18 01 00 00 02', 'EVE opcode0x37等待DataCenter+0x118异步Battle结果并识别败北值2'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00432181, 'C7 05 F0 01 8B 00 00 00 00 00', 'Battle container析构清全局0x8B01F0'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00469364, '46 69 67 68 74 5C 49 54 46 5C 49 54 46 30 34 30 30 2E 53 46 32 00', '结果UI外层与明细子界面共同使用Fight\\ITF\\ITF0400.SF2'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00412028, 0x004310E0, '结果UI外层构造加载ITF0400.SF2'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004125E7, 0x004310E0, '结果明细子界面构造加载ITF0400.SF2'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00432875, 0x00412190, 'Battle清理阶段调用结果UI外层析构0x412190'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00432883, '89 1D C8 FC 89 00', '结果UI析构释放后清全局0x89FCC8'),
        # 固化18：D8 胜利结算 RNG draw order / 升级随机增长
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443759, 0x00451565, '胜利结算：銀兩随机范围非0时首先调用全局RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443763, 'F7 BE A9 E3 00 00', '銀兩RNG结果按LayoutTeam+0x78随机范围取模'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0044387B, 0x00443B60, '胜利结算按角色槽调用升级helper 0x443B60'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044382A, '8D BE 50 7B 00 00 C7 44 24 18 08 00 00 00', '胜利升级遍历从角色区起点开始并固定循环8槽'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443999, '81 C7 E4 0D 00 00 48 89 44 24 18', '胜利升级角色槽按0xDE4步进并递减8槽计数'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443BB4, 0x00451565, '成功升级：主生存资源max增长前第1次直接RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443BBA, 'B9 03 00 00 00 F7 F9', '升级第1次直接RNG严格按3取模'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443BF6, 0x00451565, '成功升级：次资源max增长前第2次直接RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443BFC, 'B9 03 00 00 00 6A 00 F7 F9', '升级第2次直接RNG严格按3取模，随后压入五项索引0'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443C55, 0x00443D40, '升级五项随机增长索引0进入0x443D40'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443C74, 0x00443D40, '升级五项随机增长索引1进入0x443D40'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443C93, 0x00443D40, '升级五项随机增长索引2进入0x443D40'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443CB2, 0x00443D40, '升级五项随机增长索引3进入0x443D40'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443CD1, 0x00443D40, '升级五项随机增长索引4进入0x443D40'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443D44, '85 C0 7E 40 83 F8 06 7F 3B', '0x443D40仅接受角色ID 1..6'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443D51, '85 C9 7C 33 83 F9 04 7F 2E', '0x443D40仅接受五项索引0..4'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443D77, 0x00451565, '有效角色ID/五项索引时0x443D40每次恰调用一次RNG'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443A1D, 0x00451565, '胜利结算掉落槽threshold>0时调用全局RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443A22, '99 B9 64 00 00 00 F7 F9 3B 56 10 7F', '掉落RNG按100取模，并以remainder<=threshold为成功条件'),
        # 固化18：D8 Battle启动前后 reseed 与初始化 LayoutGR draw 顺序
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040D41B, 0x00451565, '伏龍A Battle命令在创建Battle container之前先draw特殊结束计数器'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0040D4D8, 0x004322C0, 'opcode0x37创建Battle container后进入0x4322C0初始化链'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004324EC, 0x00442040, '0x4322C0在同一初始化链中调用Battle核心初始化0x442040'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442076, 0x00451583, 'Battle核心初始化先取得系统时间派生seed'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0044207C, 0x0045155B, 'Battle核心初始化随后重新播种全局RNG state'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044229E, '83 FB 02 0F 85 5C 01 00 00', 'Battle初始化内部mode==2进入第一条LayoutGR选择分支'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004423CB, 0x00424430, '初始化mode2路径调用LayoutGR→LayoutTeam随机选择helper'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00442403, '83 FB 01 0F 85 C2 00 00 00', 'Battle初始化内部mode==1进入第二条LayoutGR选择分支'),
        # 执行一次赋值：把 `1'),` 的结果放入左侧 `check_bytes(pe, 0x00442490, 'C7 80 D8 00 00 00 01 00 0…` 对应的变量/字段。
        check_bytes(pe, 0x00442490, 'C7 80 D8 00 00 00 01 00 00 00', '初始化mode1路径强制DataCenter+0xD8=1'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004424AC, 0x00424430, '初始化mode1路径调用LayoutGR→LayoutTeam随机选择helper'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004424F9, '8B 88 D8 00 00 00 85 C9 75 19', '默认初始化路径仅在DataCenter+0xD8仍为0时进入随机fallback'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442503, 0x00451565, '默认初始化路径fallback直接draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00442508, '99 B9 06 00 00 00 F7 F9', '默认初始化fallback严格执行RNG%6'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00442515, '42 89 90 D8 00 00 00', '默认初始化fallback把余数+1写DataCenter+0xD8，范围1..6'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442536, 0x00424430, '默认初始化路径随后调用LayoutGR→LayoutTeam随机选择helper'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00424497, '8B 7C 28 18 85 FF 7C 43 3B FA 7D 3F 85 FF 75 45', 'LayoutGR helper读取+0x18第一候选数量；合法且非0时才进入随机选择'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004244EC, 0x00451565, 'LayoutGR helper第一候选数量合法非0时draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00424508, '8B 44 8A 1C', '第一draw按余数从LayoutGR+0x1C选择LayoutTeam ID'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00424524, '8B 46 04 8B 6C 28 6C 85 ED 7E 0A', 'LayoutGR helper随后读取同记录+0x6C第二候选数量'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042452F, 0x00451565, 'LayoutGR+0x6C第二候选数量>0时额外draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00424539, '8B 56 04 8B CB C1 E1 07 2B CB 5F 03 CD 8B 44 8A 70', '第二draw余数用于从LayoutGR+0x70第二候选值数组取返回值'),
        # 固化19：D8 AI / target / action 条件性 draw 协议
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00444267, 0x00451565, 'AI 0x444240 每次决策先固定draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044426D, 'B9 64 00 00 00 C6 44 24 20 01 F7 F9', 'AI首draw严格按100取模'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044428C, '8B 6C D0 20 8B 4C D0 1C', 'AI当前56-byte Enemy_AI记录读取+0x20与+0x1C两段阈值'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00444304, 0x00444330, 'AI首draw落入中间阈值区时进入逐候选评分0x444330'),
        # 执行一次赋值：把 `FightRole+0x8F4候选action数量'),` 的结果放入左侧 `check_bytes(pe, 0x00444349, '8B AC 81 D8 0C 00 00', 'A…` 对应的变量/字段。
        check_bytes(pe, 0x00444349, '8B AC 81 D8 0C 00 00', 'AI评分读取raw-slot+0xCD8=FightRole+0x8F4候选action数量'),
        # 执行一次赋值：把 `FightRole+0x8F8作为候选action ID列表起点'),` 的结果放入左侧 `check_bytes(pe, 0x00444371, '8D 82 DC 0C 00 00', 'AI评分…` 对应的变量/字段。
        check_bytes(pe, 0x00444371, '8D 82 DC 0C 00 00', 'AI评分以raw-slot+0xCDC=FightRole+0x8F8作为候选action ID列表起点'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004444D4, 0x00444590, '每候选先做资源/状态有效性检查0x444590'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004444E1, 0x00451565, '0x444330每个候选无条件各draw一次RNG；有效性不影响draw是否已发生'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004444FB, '99 F7 7C 31 24', '有效AI候选才将已取得RNG对MagicCon[candidate]+0x24取模生成评分'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422DE0, '85 FF 75 0E 5F 5E 5D 83 C8 FF 5B 81 C4 A8 00 00 00 C3', 'target随机列表A为空时直接-1返回，不draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00422DF2, 0x00451565, 'target selector随机列表A非空时恰draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422E5A, '85 FF 75 0E 5F 5E 5D 83 C8 FF 5B 81 C4 A8 00 00 00 C3', 'target随机列表B为空时直接-1返回，不draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00422E6C, 0x00451565, 'target selector随机列表B非空时恰draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422EDC, '56 2E 42 00 DC 2D 42 00 56 2E 42 00 DC 2D 42 00', 'FightRole+0x850==1时target selector 0..3在两随机列表路径间交替映射'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422EEC, 'DC 2D 42 00 56 2E 42 00 DC 2D 42 00 56 2E 42 00', 'FightRole+0x850!=1时target selector 0..3交换两随机列表路径映射'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422F0D, '8B BE F4 08 00 00 85 FF 7E 40 8B 44 24 10 85 C0 7F 13', 'action mode1仅在FightRole+0x8F4>0且未传入正显式action ID时需要随机候选'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00422F1F, 0x00451565, 'action mode1无显式ID时从候选池随机draw一次'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422F27, '8B 84 96 F8 08 00 00', 'action随机余数索引FightRole+0x8F8[]取得action ID'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00422281, '81 39 EA 01 00 00', '特定FIRTTECH ID 0x1EA进入戰場脫逃专用概率路径'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x004222A2, 0x00451565, 'FIRTTECH 0x1EA 戰場脫逃专用RNG%100判定'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004222A7, '99 B9 64 00 00 00 F7 F9 3B D7 7D 14', '戰場脫逃RNG严格按100取模并与0x424560返回阈值比较'),
        # 固化20：D8 公式 / 五槽效果 / FIRTTECH 主处理随机协议
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042CCB0, '8B 44 24 04 8B 4C 24 08 56 3B C1 57', '0x42CCB0通用半开区间随机helper读取两个整数实参并排序'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042CCCC, '85 F6 75 09 33 C0 8B C7 5F 5E C2 08 00', '半开区间helper上下界相等时直接返回且不draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042CCD9, 0x00451565, '半开区间helper仅在上下界不同时draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042CCDE, '99 F7 FE 8D 04 3A', '半开区间helper执行RNG%(max-min)+min，严格不含上界'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B68D, 0x0042CCB0, 'FIRTTECH selector2公式helper调用半开区间随机'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B71D, 0x0042CCB0, 'FIRTTECH selector3公式helper调用半开区间随机'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B7A5, 0x0042CCB0, 'FIRTTECH selector4公式helper调用半开区间随机'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B80A, 0x0042CCB0, 'FIRTTECH selector5公式helper调用半开区间随机'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042BCFC, 0x0042CCB0, 'FIRTTECH默认/selector6间接helper 0x42BCD0最终调用半开区间随机'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042A942, 0x00451565, 'Article/Ail2前置概率门在非强制条件下draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042A947, '99 8B CD F7 F9 3B 54 24 30', 'Article/Ail2前置概率门按100取模并与Ail2+0xCC镜像阈值比较'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042A957, '8B 83 C8 00 00 00', 'Article/Ail2读取+0xC8作为后续公式分支选择字段'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042AAD3, 0x00451565, 'Article/Ail2 +0xC8==0公式第1draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042AAD8, '99 B9 09 00 00 00 8B 6C 24 2C F7 F9', 'Article/Ail2 +0xC8==0公式第1draw按9取模'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042AAEE, 0x00451565, 'Article/Ail2 +0xC8==0公式第2draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042AAF3, '99 B9 13 00 00 00 F7 F9', 'Article/Ail2 +0xC8==0公式第2draw按19取模'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042AB14, 0x00451565, 'Article/Ail2 +0xC8==0公式第3draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042AB33, 0x00451565, 'Article/Ail2 +0xC8==0公式第4draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042AC95, 0x00451565, 'Article/Ail2 +0xC8==1公式第1draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042ACB0, 0x00451565, 'Article/Ail2 +0xC8==1公式第2draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042ACCC, 0x00451565, 'Article/Ail2 +0xC8==1公式第3draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042ACD1, '99 B9 63 00 00 00 F7 F9', 'Article/Ail2 +0xC8==1公式第3draw按99取模'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042ACEB, 0x00451565, 'Article/Ail2 +0xC8==1公式第4draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B1BF, '8B 44 24 14 85 C0 0F 8E B2 03 00 00', '五槽当前slot value<=0时直接跳过，不进入RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B313, '83 FF 1B 7E 15', '五槽effect ID>27直接进入8000特殊分支，不draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B32D, 0x00451565, '五槽effect ID0..27固定先draw一次RNG%100'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B332, '99 B9 64 00 00 00 F7 F9 3B D6', '五槽第一draw严格RNG%100并与计算阈值比较'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B30E, 'BE 64 00 00 00', '五槽ID21..27把概率阈值强制为100但仍继续执行第一draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042B4D3, '2B FA 83 FF 01 7D 04', '五槽第二随机先计算high-low且仅范围>=1才draw'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042B4DE, 0x00451565, '五槽通过概率门后、派生区间足够时执行第二draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042C6ED, '8B 47 18 3B C1', 'FIRTTECH主处理读取record+0x18并区分特殊入口'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C728, 0x00451565, 'FIRTTECH+0x18==0特殊入口固定执行第一RNG%100门'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042C72D, '99 B9 64 00 00 00 F7 F9 3B D3', 'FIRTTECH特殊入口第一门严格RNG%100'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C757, 0x00451565, 'FIRTTECH特殊入口第一门通过后固定执行第二RNG%100门'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C79B, 0x0042CD50, 'FIRTTECH特殊入口成功后调用记录派生条件随机helper'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042C7B2, '8B 47 30', 'FIRTTECH常规入口读取record+0x30作为公式selector'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C845, 0x0042CD50, 'FIRTTECH常规公式计算后同样调用记录派生条件随机helper'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C8B7, 0x00451565, 'FIRTTECH后处理概率门只有前置状态无法决定时才draw RNG%100'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042C8BC, '99 B9 64 00 00 00 F7 F9 3B 54 24 2C', 'FIRTTECH后处理概率门严格RNG%100比较派生阈值'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C17F, 0x00451565, '0x42BF90后处理公式固定第1draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042C184, '99 B9 13 00 00 00 F7 F9', '0x42BF90第1draw按19取模后+1作除数'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C197, 0x00451565, '0x42BF90后处理公式固定第2draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042C19C, '99 B9 09 00 00 00 F7 F9', '0x42BF90第2draw按9取模后+1作除数'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C380, 0x00451565, '0x42C260后处理公式固定第1draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042C385, '8B C8 B8 03 B5 7E A5 F7 E9 03 D1 C1 FA 06', '0x42C260第1draw通过魔数除法构造floor(RNG/99)+1除数'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042C3AA, 0x00451565, '0x42C260后处理公式固定第2draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042C3AF, '8B C8 B8 F3 1A CA 6B F7 E9 C1 FA 03', '0x42C260第2draw通过魔数除法构造floor(RNG/19)+1除数'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042CD58, '8B 80 90 00 00 00 85 C0 7E 40', '0x42CD50仅在输入record+0x90>0时尝试派生随机上限'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042CD7C, 'B8 67 66 66 66 F7 E9 C1 FA 03', '0x42CD50把全局派生字段按20整除得到RNG模数候选'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042CD93, 0x00451565, '0x42CD50仅在派生模数>0时draw一次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042CD98, '99 F7 FE', '0x42CD50返回RNG%派生模数'),
        # 固化21：D8 剩余 Battle-core / Battle-associated 直接 RNG 点分类
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0041F33E, 0x00451565, 'FightRole成功初始化路径第1个直接RNG draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0041F343, '25 01 00 00 80', 'FightRole初始化第1draw压到0/1'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0041F454, 0x00451565, 'FightRole成功初始化路径第2个直接RNG draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0041F459, '99 B9 40 9C 00 00 F7 F9 8B CF 81 C2 80 38 01 00', 'FightRole初始化第2draw为RNG%40000+80000'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421A00, '83 EC 08 53 55 56 57 8B F9 33 DB', 'Battle动作内部状态更新函数0x421A00入口'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00421AB6, 0x00451565, '0x421A00第一条件随机draw'),
        # 执行一次赋值：把 `floor((n-1)/2)'),` 的结果放入左侧 `check_bytes(pe, 0x00421AAC, '8D 42 FF 99 2B C2 8B E8 D…` 对应的变量/字段。
        check_bytes(pe, 0x00421AAC, '8D 42 FF 99 2B C2 8B E8 D1 FD', '0x421AB6前构造low=floor((n-1)/2)'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421AC1, '99 2B C2 8B D1 8B C8 D1 F9 8B C2 2B CD 99 41 F7 F9', '0x421AB6以floor(n/2)-floor((n-1)/2)+1为模数'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00421B20, 0x00451565, '0x421A00第二条件概率draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421B25, '99 B9 64 00 00 00 F7 F9', '0x421B20严格RNG%100'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00421B33, '8D 45 01 3B C1 7E 02 8B C1 89 84 B7 84 09 00 00', '第二概率门成功后+0x984[index]递增并cap100'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00442D8E, 0x004232D0, 'BattleManager更新链调用候选注入/概率函数0x4232D0'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00423395, 0x00451565, '0x4232D0在前置条件通过后固定draw一次RNG%100'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00423379, '8B 8E 80 08 00 00 85 C9 7E 12 8B 84 86 84 09 00 00', '0x4232D0阈值由FightRole字段与+0x984[index]派生'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0042339A, '99 B9 64 00 00 00 F7 F9 3B FA', '0x423395严格RNG%100并与派生阈值比较'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00422384, 0x0043D300, 'Battle动作/表现链调用0x43D300资源表现对象'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043D303, '8A 46 14 84 C0 74 12', '0x43D300已有初始化标志时不再进入首次随机初始化'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0043D352, 0x00451565, '0x43D300首次资源初始化消耗1次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0043D357, '25 01 00 00 80', '0x43D352只取0/1并写对象+0x10'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0042040A, 0x00443D90, 'Battle FIRTTECH执行链调用特殊召唤helper 0x443D90'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443D9B, '3D 61 01 00 00', '特殊召唤helper识别FIRTTECH 0x161'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443DA8, '3D 69 01 00 00', '特殊召唤helper识别FIRTTECH 0x169'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443DB3, 0x00451565, 'FIRTTECH 0x169「屍魔召喚」四选一draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443DB8, '25 03 00 00 80', '0x169首draw压到0..3'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443DDA, 0x00451565, 'FIRTTECH 0x161「呼叫增援」二选一draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443DDF, '25 01 00 00 80', '0x161首draw压到0..1'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443E24, 0x00451565, '两种特殊召唤共同的召唤数量draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00443E29, '25 01 00 00 80', '召唤数量draw压到0/1后+1，即请求1或2个'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x00443F39, 0x0041F2B0, '特殊召唤实际创建FightRole时进入构造函数，成功构造会间接再消费2 draw'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x004466FD, 'BE 64 00 00 00', '随机FightRole槽重试上限初始化为100'),
        # 调用 `check_call` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_call(pe, 0x0044674B, 0x00451565, '随机FightRole槽选择每次尝试消费1次RNG'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00446750, '25 07 00 00 80', 'FightRole槽随机索引压到0..7'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x00446760, '4E 83 F8 04 7D 10', '每次draw后先递减剩余尝试次数并按槽半区规则校验'),
        # 调用 `check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        check_bytes(pe, 0x0044679B, '85 C0 7C A8', '选中槽映射无效时回到随机重试循环'),
    ]
    # 把右侧 `{}` 计算得到的值保存到 `jump_tables`，后面的判断或输出会继续使用它。
    jump_tables = {}
    # 开始循环 `name, va, count in [`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for name, va, count in [
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        ('目标集合selector表A', 0x00422A40, 4),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        ('目标集合pattern表B', 0x00422A50, 6),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        ('目标关系表_侧1', 0x00422EDC, 4),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        ('目标关系表_非侧1', 0x00422EEC, 4),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        ('FIRTTECH公式分派表', 0x0042CC98, 5),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        ('ArticleAil2解除跳转表', 0x0042196C, 9),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        ('FIRTTECH特殊动作跳转表', 0x00422044, 12),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        ('特殊物品动作跳转表', 0x0040D0EC, 9),
    # 继续填写当前数据结构或参数列表中的一项：`]:`。
    ]:
        # 把右侧 `[hexva(pe.read_u32(va + i * 4)) for i in range(count)]` 计算得到的值保存到 `jump_tables[name]`，后面的判断或输出会继续使用它。
        jump_tables[name] = [hexva(pe.read_u32(va + i * 4)) for i in range(count)]

    # 执行一次赋值：把 `list(pe.read_va(0x0040D110, 15))` 的结果放入左侧 `jump_tables['特殊物品动作selector_动作码3到17']` 对应的变量/字段。
    jump_tables['特殊物品动作selector_动作码3到17'] = list(pe.read_va(0x0040D110, 15))
    # 执行一次赋值：把 `list(pe.read_va(0x0043B154, 18))` 的结果放入左侧 `jump_tables['永久物品effect_selector_ID43到60']` 对应的变量/字段。
    jump_tables['永久物品effect_selector_ID43到60'] = list(pe.read_va(0x0043B154, 18))

    # 把右侧 `[0x0041F2B0, 0x00444240, 0x00444330, 0x00422F00, 0x00423BA0,` 计算得到的值保存到 `xref_targets`，后面的判断或输出会继续使用它。
    xref_targets = [0x0041F2B0, 0x00444240, 0x00444330, 0x00422F00, 0x00423BA0,
                    # 继续填写当前数据结构或参数列表中的一项：`0x00422CB0, 0x00422850, 0x00422AB0, 0x0042A7A0, 0x0042A7D0,`。
                    0x00422CB0, 0x00422850, 0x00422AB0, 0x0042A7A0, 0x0042A7D0,
                    # 继续填写当前数据结构或参数列表中的一项：`0x0042C680, 0x0042B0D0, 0x0042B5F0, 0x0042B630, 0x0042B6C0,`。
                    0x0042C680, 0x0042B0D0, 0x0042B5F0, 0x0042B630, 0x0042B6C0,
                    # 继续填写当前数据结构或参数列表中的一项：`0x0042B750, 0x0042B7D0, 0x0042B840, 0x0042BA80, 0x0042BB50,`。
                    0x0042B750, 0x0042B7D0, 0x0042B840, 0x0042BA80, 0x0042BB50,
                    # 执行这一条实际代码步骤：`0x0042BCD0, 0x0042CCB0, 0x0045155B, 0x00451565, 0x00451583, 0x00420ED0, 0x00420E20, 0x00420440, 0x0…`；它与上下相邻语句共同完成当前函数的小任务。
                    0x0042BCD0, 0x0042CCB0, 0x0045155B, 0x00451565, 0x00451583, 0x00420ED0, 0x00420E20, 0x00420440, 0x00421690, 0x00421A00, 0x004231D0, 0x004231F0, 0x0043B8F0, 0x00437960, 0x00439D70, 0x0043AEC0, 0x00443590, 0x00443660, 0x00441F60, 0x00424430, 0x00443B60, 0x00439820, 0x00439120, 0x00411FB0, 0x00412440, 0x00412570, 0x00412CA0, 0x004133C0, 0x0042D4D0, 0x0042D6E0, 0x00432130, 0x00443D40]
    # 把右侧 `pe.direct_call_refs_many(xref_targets)` 计算得到的值保存到 `scanned`，后面的判断或输出会继续使用它。
    scanned = pe.direct_call_refs_many(xref_targets)
    # 把右侧 `{hexva(target): [hexva(x) for x in scanned[target]] for target in xref_tar…` 计算得到的值保存到 `known_xrefs`，后面的判断或输出会继续使用它。
    known_xrefs = {hexva(target): [hexva(x) for x in scanned[target]] for target in xref_targets}

    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {
        # 给当前结果字典的“工具版本”字段填写 `VER,`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本': VER,
        # 给当前结果字典的“文件”字段填写 `str(path),`，这样导出的 JSON/表格能保留这一项证据。
        '文件': str(path),
        # 给当前结果字典的“SHA256”字段填写 `digest,`，这样导出的 JSON/表格能保留这一项证据。
        'SHA256': digest,
        # 给当前结果字典的“Oracle_SHA256”字段填写 `ORACLE_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        'Oracle_SHA256': ORACLE_SHA256,
        # 给当前结果字典的“Oracle哈希匹配”字段填写 `digest == ORACLE_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        'Oracle哈希匹配': digest == ORACLE_SHA256,
        # 给当前结果字典的“PE”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'PE': {
            # 给当前结果字典的“类型”字段填写 `pe.pe_kind,`，这样导出的 JSON/表格能保留这一项证据。
            '类型': pe.pe_kind,
            # 给当前结果字典的“Machine”字段填写 `f'0x{pe.machine:04X}',`，这样导出的 JSON/表格能保留这一项证据。
            'Machine': f'0x{pe.machine:04X}',
            # 给当前结果字典的“ImageBase”字段填写 `hexva(pe.image_base),`，这样导出的 JSON/表格能保留这一项证据。
            'ImageBase': hexva(pe.image_base),
            # 给当前结果字典的“EntryVA”字段填写 `hexva(pe.image_base + pe.entry_rva),`，这样导出的 JSON/表格能保留这一项证据。
            'EntryVA': hexva(pe.image_base + pe.entry_rva),
            # 给当前结果字典的“Sections”字段填写 `pe.sections,`，这样导出的 JSON/表格能保留这一项证据。
            'Sections': pe.sections,
        },
        # 给当前结果字典的“关键验证”字段填写 `checks,`，这样导出的 JSON/表格能保留这一项证据。
        '关键验证': checks,
        # 给当前结果字典的“关键验证数量”字段填写 `len(checks),`，这样导出的 JSON/表格能保留这一项证据。
        '关键验证数量': len(checks),
        # 给当前结果字典的“关键验证全部通过”字段填写 `all(row['status'] == 'PASS' for row in checks),`，这样导出的 JSON/表格能保留这一项证据。
        '关键验证全部通过': all(row['status'] == 'PASS' for row in checks),
        # 给当前结果字典的“跳转表原值”字段填写 `jump_tables,`，这样导出的 JSON/表格能保留这一项证据。
        '跳转表原值': jump_tables,
        # 给当前结果字典的“已知目标的E8直接调用引用”字段填写 `known_xrefs,`，这样导出的 JSON/表格能保留这一项证据。
        '已知目标的E8直接调用引用': known_xrefs,
        # 给当前结果字典的“证据边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '证据边界': [
            # 继续填写当前数据结构或参数列表中的一项：`'PASS 只证明当前 Oracle 中这些指令、调用和表值存在；不自动证明作者原始变量名。',`。
            'PASS 只证明当前 Oracle 中这些指令、调用和表值存在；不自动证明作者原始变量名。',
            # 执行一次赋值：把 `manager+0x3E4+slot*0xDE4。',` 的结果放入左侧 `'BattleManager raw-slot 坐标与 FightRole 相对坐标必须分开：FightRo…` 对应的变量/字段。
            'BattleManager raw-slot 坐标与 FightRole 相对坐标必须分开：FightRole(slot)=manager+0x3E4+slot*0xDE4。',
            # 继续填写当前数据结构或参数列表中的一项：`'RNG 核心算法与全局 state/播种入口已由指令直接闭合；Battle 内条件分支造成的变长draw数量只能冻结静态可达协议，当前不把未执行路径假定为实际发生。',`。
            'RNG 核心算法与全局 state/播种入口已由指令直接闭合；Battle 内条件分支造成的变长draw数量只能冻结静态可达协议，当前不把未执行路径假定为实际发生。',
            # 继续填写当前数据结构或参数列表中的一项：`'RoleDefinition+0x2C/+0x30 与 +0x34/+0x38 已由 Battle+EVE 双 consumer 闭合为两对 max/current 资源；作者原始资源名…`。
            'RoleDefinition+0x2C/+0x30 与 +0x34/+0x38 已由 Battle+EVE 双 consumer 闭合为两对 max/current 资源；作者原始资源名称仍未证明。',
            # 继续填写当前数据结构或参数列表中的一项：`'D6 已静态闭合 residual→pending(+0xCC)→runtime(+0xA34)、39槽递减和多条清除分派；不得把递减10直接命名为毫秒/帧。',`。
            'D6 已静态闭合 residual→pending(+0xCC)→runtime(+0xA34)、39槽递减和多条清除分派；不得把递减10直接命名为毫秒/帧。',
            # 继续填写当前数据结构或参数列表中的一项：`'固化3曾错误把 0x421518 解释为读取 DamageInfo+0x44；真实工作指针为 DamageInfo+0x04，故实际读取 +0x48。RoleDef+0x1E8→Dama…`。
            '固化3曾错误把 0x421518 解释为读取 DamageInfo+0x44；真实工作指针为 DamageInfo+0x04，故实际读取 +0x48。RoleDef+0x1E8→DamageInfo+0x44 复制本身仍成立。',
            # 继续填写当前数据结构或参数列表中的一项：`'Ail2+0x20 已由数据证明为0..699唯一记录/物品ID字段（存在9条与物理索引不一致的置换）；0x10F/110/111 的复苏专用路径与0x12D..133特殊范围识别均使用…`。
            'Ail2+0x20 已由数据证明为0..699唯一记录/物品ID字段（存在9条与物理索引不一致的置换）；0x10F/110/111 的复苏专用路径与0x12D..133特殊范围识别均使用这个字段/ID空间。',
            # 继续填写当前数据结构或参数列表中的一项：`'positive 五槽 ID>27 会被 normal raw-ID pending writer 的 signed<=27 gate 排除；这不等于“大ID无效果”。非战斗 item-…`。
            'positive 五槽 ID>27 会被 normal raw-ID pending writer 的 signed<=27 gate 排除；这不等于“大ID无效果”。非战斗 item-use 的 0x43AEC0 已证明会按 ID43..60 直接修改 API 主表角色记录，而 0x170..172 又以 Ail2+0x28 特殊动作码执行；因此大ID的业务解释必须按具体 consumer 分层。',
            # 继续填写当前数据结构或参数列表中的一项：`'DataCenter+0xA4 已由 Public\\API.ENC loader + 0x350 几何 + Battle整记录回写三重证据闭合；0x43AEC0 对该表的 writer…`。
            'DataCenter+0xA4 已由 Public\\API.ENC loader + 0x350 几何 + Battle整记录回写三重证据闭合；0x43AEC0 对该表的 writer 属于实际角色运行/持久数据修改，不是菜单预览。',
            # 执行一次赋值：把 `13/14 的 encounter 语义由官方物品文本与 0x403510/0x4036DE 阈值 consumer 交叉闭合；duration=1…` 的结果放入左侧 `'Ail2+0x28` 对应的变量/字段。
            'Ail2+0x28=13/14 的 encounter 语义由官方物品文本与 0x403510/0x4036DE 阈值 consumer 交叉闭合；固化41进一步证明duration=1500是合格鼠标输入状态变化事件计数上限，第1501次合格调用才清零，不能换算成固定Legacy Tick或墙钟时间。',
            # 执行一次赋值：把 `4 传给 0x443660 的却是参数1，并把 DataCenter+0x118 写1；dispatcher 参数4虽有“因事停戰”文本分支，但当前…` 的结果放入左侧 `'D7 outcome detector 的 return4 与 result dispatcher 参数4…` 对应的变量/字段。
            'D7 outcome detector 的 return4 与 result dispatcher 参数4不是同一语义链：固化43全量扫描确认0x443660仅有0x432908/0x432929/0x432948三个direct E8 caller，全部位于0x4328A0；detector=4明确走dispatcher参数1并把DataCenter+0x118写1。参数4虽保留“因事停戰”文本分支，但当前canonical状态机无生产者。不得把两者合并命名，也不得把参数4绝对称为死代码。',
            # 继续填写当前数据结构或参数列表中的一项：`'胜利掉落判断严格为 RNG%100 <= LayoutTeam阈值；兼容实现必须保留等号，不能擅自按常见百分比写成 <。',`。
            '胜利掉落判断严格为 RNG%100 <= LayoutTeam阈值；兼容实现必须保留等号，不能擅自按常见百分比写成 <。',
            # 继续填写当前数据结构或参数列表中的一项：`'opcode0x37已直接闭合为启动Battle并等待结果：record+0x04为LayoutGR索引；参数值0x82指LayoutGR[130]“伏龍A”，不是EVE opcode0…`。
            'opcode0x37已直接闭合为启动Battle并等待结果：record+0x04为LayoutGR索引；参数值0x82指LayoutGR[130]“伏龍A”，不是EVE opcode0x82。',
            # 继续填写当前数据结构或参数列表中的一项：`'LayoutGR+0x18旧名layout_team_index_primary已撤销；0x424430直接证明它是候选数量，+0x1C才是LayoutTeam ID数组。',`。
            'LayoutGR+0x18旧名layout_team_index_primary已撤销；0x424430直接证明它是候选数量，+0x1C才是LayoutTeam ID数组。',
            # 继续填写当前数据结构或参数列表中的一项：`'固化38已把伏龍A DataCenter+0xF4接入统一Legacy Clock：Battle对象在合格更新门控下每个Legacy Tick最多递减1；当前DDDES=20时名义20Hz/50ms，但主线程阻塞、pending合并和Battle gate仍会拉长实际墙钟。',`。
            '固化38已把伏龍A DataCenter+0xF4接入统一Legacy Clock：Battle对象在合格更新门控下每个Legacy Tick最多递减1；当前DDDES=20时名义20Hz/50ms，但主线程阻塞、pending合并和Battle gate仍会拉长实际墙钟。',
            # 继续填写当前数据结构或参数列表中的一项：`'伏龍A/伏龍B的两阶段顺序目前只有数据结构强推断，当前真实EVE样本不含opcode0x37，不能升级为脚本事实。',`。
            '伏龍A/伏龍B的两阶段顺序目前只有数据结构强推断，当前真实EVE样本不含opcode0x37，不能升级为脚本事实。',
            # 继续填写当前数据结构或参数列表中的一项：`'固化17结果UI生命周期：0x89FCC8外层对象与其+0x600明细子界面各自拥有独立+0x579活动/过渡标志；高层必须等待两者均归零，不能合并成一个完成位。',`。
            '固化17结果UI生命周期：0x89FCC8外层对象与其+0x600明细子界面各自拥有独立+0x579活动/过渡标志；高层必须等待两者均归零，不能合并成一个完成位。',
            # 继续填写当前数据结构或参数列表中的一项：`'FANIN.SF2 的“进入全黑”视觉语义由真实SF2 Section0权重序列与近黑色全屏Section1共同证明；FANIN/FANOUT作者文件名本身不能单独作为视觉方向证据。',`。
            'FANIN.SF2 的“进入全黑”视觉语义由真实SF2 Section0权重序列与近黑色全屏Section1共同证明；FANIN/FANOUT作者文件名本身不能单独作为视觉方向证据。',
            # 执行一次赋值：把 `15只能称15次符合条件的容器update递减；不能称15帧、15毫秒或15 Legacy Tick。计数归零后的下一次合格update才执行0x4…` 的结果放入左侧 `'Battle container+0x244` 对应的变量/字段。
            'Battle container+0x244=15只能称15次符合条件的容器update递减；不能称15帧、15毫秒或15 Legacy Tick。计数归零后的下一次合格update才执行0x441F60提交。',
            # 继续填写当前数据结构或参数列表中的一项：`'0x42D4D0在此链是Effect Manager创建/取得FANIN效果句柄，不是“清理函数”；固化17已把此前口头“收尾cleanup”泛称撤销。',`。
            '0x42D4D0在此链是Effect Manager创建/取得FANIN效果句柄，不是“清理函数”；固化17已把此前口头“收尾cleanup”泛称撤销。',
            # 继续填写当前数据结构或参数列表中的一项：`'固化18已静态闭合胜利结果RNG主顺序：銀兩range>0先draw；随后按8个角色槽执行条件升级，成功升级先直接draw两次，再调用五个0x443D40增长helper；最后才按最多4…`。
            '固化18已静态闭合胜利结果RNG主顺序：銀兩range>0先draw；随后按8个角色槽执行条件升级，成功升级先直接draw两次，再调用五个0x443D40增长helper；最后才按最多4个掉落槽做threshold>0的drop draw。条件不同会改变后续全局RNG流位置。',
            # 执行一次赋值：把 `7次RNG，其他角色即使进入升级helper也不会在五个0x443D40子调用中消耗RNG。',` 的结果放入左侧 `'0x443D40只有角色ID 1..6、增长索引0..4时才draw；因此主角色一次成功升级消耗2+5` 对应的变量/字段。
            '0x443D40只有角色ID 1..6、增长索引0..4时才draw；因此主角色一次成功升级消耗2+5=7次RNG，其他角色即使进入升级helper也不会在五个0x443D40子调用中消耗RNG。',

            # 继续填写当前数据结构或参数列表中的一项：`'静态协议必须逐项标明每个draw的发生条件；不能把条件draw误写成固定每阶段draw数。',`。
            '静态协议必须逐项标明每个draw的发生条件；不能把条件draw误写成固定每阶段draw数。',
        ],
    }
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json(result, args.json)
    # 检查条件 `not result['Oracle哈希匹配'] or not result['关键验证全部通过']`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not result['Oracle哈希匹配'] or not result['关键验证全部通过']:
        # 检测到不能继续的情况，主动抛出异常 `SystemExit(2)`，让上层明确知道数据或参数不符合要求。
        raise SystemExit(2)


# 定义函数 `parse_int`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_int(text: str) -> int:
    # 把 `int(text, 0)` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return int(text, 0)


# 定义函数 `cmd_xrefs`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_xrefs(args):
    # 把右侧 `PEImage(args.exe)` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe = PEImage(args.exe)
    # 把右侧 `args.target` 计算得到的值保存到 `target`，后面的判断或输出会继续使用它。
    target = args.target
    # 把右侧 `pe.direct_call_refs(target)` 计算得到的值保存到 `refs`，后面的判断或输出会继续使用它。
    refs = pe.direct_call_refs(target)
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json({
        # 给当前结果字典的“工具版本”字段填写 `VER,`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本': VER,
        # 给当前结果字典的“文件”字段填写 `str(args.exe),`，这样导出的 JSON/表格能保留这一项证据。
        '文件': str(args.exe),
        # 给当前结果字典的“目标VA”字段填写 `hexva(target),`，这样导出的 JSON/表格能保留这一项证据。
        '目标VA': hexva(target),
        # 给当前结果字典的“E8直接调用引用数量”字段填写 `len(refs),`，这样导出的 JSON/表格能保留这一项证据。
        'E8直接调用引用数量': len(refs),
        # 给当前结果字典的“引用”字段填写 `[hexva(x) for x in refs],`，这样导出的 JSON/表格能保留这一项证据。
        '引用': [hexva(x) for x in refs],
        # 给当前结果字典的“边界”字段填写 `'仅扫描可执行节中的 x86 E8 rel32 直接 CALL；函数指针、jmp、SEH/表驱动间接调用不在此结果中。',`，这样导出的 JSON/表格能保留这一项证据。
        '边界': '仅扫描可执行节中的 x86 E8 rel32 直接 CALL；函数指针、jmp、SEH/表驱动间接调用不在此结果中。',
    # 执行这一条实际代码步骤：`}, args.json)`；它与上下相邻语句共同完成当前函数的小任务。
    }, args.json)



# 定义函数 `cmd_analyze_d8_rng`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_analyze_d8_rng(args):
    # 执行这一条实际代码步骤：`"""重放 D8 胜利结算 RNG draw 顺序与角色随机成长静态参数。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """重放 D8 胜利结算 RNG draw 顺序与角色随机成长静态参数。"""
    # 把右侧 `Path(args.exe)` 计算得到的值保存到 `path`，后面的判断或输出会继续使用它。
    path = Path(args.exe)
    # 把右侧 `hashlib.sha256(path.read_bytes()).hexdigest()` 计算得到的值保存到 `digest`，后面的判断或输出会继续使用它。
    digest = hashlib.sha256(path.read_bytes()).hexdigest()
    # 把右侧 `PEImage(path)` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe = PEImage(path)
    # 把右侧 `pe.direct_call_refs(0x00451565)` 计算得到的值保存到 `refs`，后面的判断或输出会继续使用它。
    refs = pe.direct_call_refs(0x00451565)

    # 0x443D40 使用的静态随机成长参数：先由 role×5+index 映射到 category，
    # 再由 category 取得 base 与 range_minus_one，返回 base + RNG%(range_minus_one+1)。
    # 把右侧 `[pe.read_u32(0x0046C1B0 + i * 4) for i in range(5)]` 计算得到的值保存到 `bases`，后面的判断或输出会继续使用它。
    bases = [pe.read_u32(0x0046C1B0 + i * 4) for i in range(5)]
    # 把右侧 `[pe.read_u32(0x0046C1C4 + i * 4) for i in range(5)]` 计算得到的值保存到 `range_minus_one`，后面的判断或输出会继续使用它。
    range_minus_one = [pe.read_u32(0x0046C1C4 + i * 4) for i in range(5)]
    # 把右侧 `[pe.read_u32(0x0046C1D8 + i * 4) for i in range(30)]` 计算得到的值保存到 `categories`，后面的判断或输出会继续使用它。
    categories = [pe.read_u32(0x0046C1D8 + i * 4) for i in range(30)]
    # 把右侧 `[]` 计算得到的值保存到 `role_rows`，后面的判断或输出会继续使用它。
    role_rows = []
    # 开始循环 `role_id in range(1, 7)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for role_id in range(1, 7):
        # 把右侧 `[]` 计算得到的值保存到 `row`，后面的判断或输出会继续使用它。
        row = []
        # 开始循环 `stat_index in range(5)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for stat_index in range(5):
            # 把右侧 `categories[(role_id - 1) * 5 + stat_index]` 计算得到的值保存到 `category`，后面的判断或输出会继续使用它。
            category = categories[(role_id - 1) * 5 + stat_index]
            # 把右侧 `bases[category]` 计算得到的值保存到 `base`，后面的判断或输出会继续使用它。
            base = bases[category]
            # 把右侧 `range_minus_one[category] + 1` 计算得到的值保存到 `divisor`，后面的判断或输出会继续使用它。
            divisor = range_minus_one[category] + 1
            # 把这一条新结果追加到列表 `row`；不会覆盖前面已经收集的记录。
            row.append({
                # 给当前结果字典的“增长索引”字段填写 `stat_index,`，这样导出的 JSON/表格能保留这一项证据。
                '增长索引': stat_index,
                # 给当前结果字典的“类别”字段填写 `category,`，这样导出的 JSON/表格能保留这一项证据。
                '类别': category,
                # 给当前结果字典的“基础增量”字段填写 `base,`，这样导出的 JSON/表格能保留这一项证据。
                '基础增量': base,
                # 给当前结果字典的“RNG模数”字段填写 `divisor,`，这样导出的 JSON/表格能保留这一项证据。
                'RNG模数': divisor,
                # 给当前结果字典的“可能增量范围”字段填写 `[base, base + divisor - 1],`，这样导出的 JSON/表格能保留这一项证据。
                '可能增量范围': [base, base + divisor - 1],
            })
        # 把这一条新结果追加到列表 `role_rows`；不会覆盖前面已经收集的记录。
        role_rows.append({'角色ID': role_id, '五项增长': row})

    # 把右侧 `[0x00443759, 0x00443BB4, 0x00443BF6, 0x00443D77, 0x00443A1D]` 计算得到的值保存到 `victory_sites`，后面的判断或输出会继续使用它。
    victory_sites = [0x00443759, 0x00443BB4, 0x00443BF6, 0x00443D77, 0x00443A1D]
    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {
        # 给当前结果字典的“工具版本”字段填写 `VER,`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本': VER,
        # 给当前结果字典的“文件”字段填写 `str(path),`，这样导出的 JSON/表格能保留这一项证据。
        '文件': str(path),
        # 给当前结果字典的“SHA256”字段填写 `digest,`，这样导出的 JSON/表格能保留这一项证据。
        'SHA256': digest,
        # 给当前结果字典的“Oracle_SHA256”字段填写 `ORACLE_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        'Oracle_SHA256': ORACLE_SHA256,
        # 给当前结果字典的“Oracle哈希匹配”字段填写 `digest == ORACLE_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        'Oracle哈希匹配': digest == ORACLE_SHA256,
        # 给当前结果字典的“全EXE_RNG直接E8调用数量”字段填写 `len(refs),`，这样导出的 JSON/表格能保留这一项证据。
        '全EXE_RNG直接E8调用数量': len(refs),
        # 给当前结果字典的“全EXE_RNG直接E8调用引用”字段填写 `[hexva(x) for x in refs],`，这样导出的 JSON/表格能保留这一项证据。
        '全EXE_RNG直接E8调用引用': [hexva(x) for x in refs],
        # 给当前结果字典的“Battle启动与初始化静态draw顺序”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'Battle启动与初始化静态draw顺序': {
            # 给当前结果字典的“伏龍A_Battle前draw”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '伏龍A_Battle前draw': {
                # 给当前结果字典的“调用点”字段填写 `hexva(0x0040D41B),`，这样导出的 JSON/表格能保留这一项证据。
                '调用点': hexva(0x0040D41B),
                # 给当前结果字典的“条件”字段填写 `'EVE opcode0x37 record+0x04 == 0x82（LayoutGR[130]「伏龍A」）',`，这样导出的 JSON/表格能保留这一项证据。
                '条件': 'EVE opcode0x37 record+0x04 == 0x82（LayoutGR[130]「伏龍A」）',
                # 给当前结果字典的“变换”字段填写 `'RNG % 400 + 800 -> DataCenter+0xF4',`，这样导出的 JSON/表格能保留这一项证据。
                '变换': 'RNG % 400 + 800 -> DataCenter+0xF4',
                # 给当前结果字典的“调用链到重新播种”字段填写 `[hexva(x) for x in [0x0040D41B,0x0040D4D8,0x004324EC,0x00442076,0x004…`，这样导出的 JSON/表格能保留这一项证据。
                '调用链到重新播种': [hexva(x) for x in [0x0040D41B,0x0040D4D8,0x004324EC,0x00442076,0x0044207C]],
                # 给当前结果字典的“结论”字段填写 `'该draw发生在0x44207C重新播种之前，因此不会推进重新播种后的Battle RNG流。',`，这样导出的 JSON/表格能保留这一项证据。
                '结论': '该draw发生在0x44207C重新播种之前，因此不会推进重新播种后的Battle RNG流。',
            },
            # 给当前结果字典的“Battle重新播种”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'Battle重新播种': {
                # 给当前结果字典的“seed_source_call”字段填写 `hexva(0x00442076),`，这样导出的 JSON/表格能保留这一项证据。
                'seed_source_call': hexva(0x00442076),
                # 给当前结果字典的“seed_write_call”字段填写 `hexva(0x0044207C),`，这样导出的 JSON/表格能保留这一项证据。
                'seed_write_call': hexva(0x0044207C),
                # 给当前结果字典的“state”字段填写 `hexva(0x0046E29C),`，这样导出的 JSON/表格能保留这一项证据。
                'state': hexva(0x0046E29C),
            },
            # 给当前结果字典的“重新播种后的初始化随机分支”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
            '重新播种后的初始化随机分支': [
                # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
                {
                    # 给当前结果字典的“内部mode”字段填写 `2,`，这样导出的 JSON/表格能保留这一项证据。
                    '内部mode': 2,
                    # 给当前结果字典的“LayoutGR_helper_call”字段填写 `hexva(0x004423CB),`，这样导出的 JSON/表格能保留这一项证据。
                    'LayoutGR_helper_call': hexva(0x004423CB),
                    # 给当前结果字典的“前置直接RNG”字段填写 `'该分支在0x442040本函数内无0x451565直接call；随后0x424430按LayoutGR候选结构条件draw。',`，这样导出的 JSON/表格能保留这一项证据。
                    '前置直接RNG': '该分支在0x442040本函数内无0x451565直接call；随后0x424430按LayoutGR候选结构条件draw。',
                },
                # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
                {
                    # 给当前结果字典的“内部mode”字段填写 `1,`，这样导出的 JSON/表格能保留这一项证据。
                    '内部mode': 1,
                    # 给当前结果字典的“动作”字段填写 `'先强制DataCenter+0xD8=1',`，这样导出的 JSON/表格能保留这一项证据。
                    '动作': '先强制DataCenter+0xD8=1',
                    # 给当前结果字典的“LayoutGR_helper_call”字段填写 `hexva(0x004424AC),`，这样导出的 JSON/表格能保留这一项证据。
                    'LayoutGR_helper_call': hexva(0x004424AC),
                    # 给当前结果字典的“前置直接RNG”字段填写 `'该分支在0x442040本函数内无0x451565直接call；随后0x424430按LayoutGR候选结构条件draw。',`，这样导出的 JSON/表格能保留这一项证据。
                    '前置直接RNG': '该分支在0x442040本函数内无0x451565直接call；随后0x424430按LayoutGR候选结构条件draw。',
                },
                # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
                {
                    # 给当前结果字典的“内部mode”字段填写 `'其他/默认',`，这样导出的 JSON/表格能保留这一项证据。
                    '内部mode': '其他/默认',
                    # 给当前结果字典的“fallback_direct_draw”字段填写 `hexva(0x00442503),`，这样导出的 JSON/表格能保留这一项证据。
                    'fallback_direct_draw': hexva(0x00442503),
                    # 给当前结果字典的“fallback条件”字段填写 `'外部helper未提供值且DataCenter+0xD8==0',`，这样导出的 JSON/表格能保留这一项证据。
                    'fallback条件': '外部helper未提供值且DataCenter+0xD8==0',
                    # 给当前结果字典的“fallback变换”字段填写 `'RNG%6 + 1 -> DataCenter+0xD8',`，这样导出的 JSON/表格能保留这一项证据。
                    'fallback变换': 'RNG%6 + 1 -> DataCenter+0xD8',
                    # 给当前结果字典的“LayoutGR_helper_call”字段填写 `hexva(0x00442536),`，这样导出的 JSON/表格能保留这一项证据。
                    'LayoutGR_helper_call': hexva(0x00442536),
                },
            ],
            # 给当前结果字典的“LayoutGR_helper_0x424430_draw协议”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'LayoutGR_helper_0x424430_draw协议': {
                # 给当前结果字典的“第一draw”字段填写 `'合法LayoutGR记录的+0x18第一候选数量必须>0；0x4244EC draw，RNG%count，从+0x1C数组选Layout…`，这样导出的 JSON/表格能保留这一项证据。
                '第一draw': '合法LayoutGR记录的+0x18第一候选数量必须>0；0x4244EC draw，RNG%count，从+0x1C数组选LayoutTeam ID。',
                # 给当前结果字典的“第二draw”字段填写 `'随后读取+0x6C；仅当second_count>0时0x42452F再draw，RNG%second_count，从+0x70数组取第…`，这样导出的 JSON/表格能保留这一项证据。
                '第二draw': '随后读取+0x6C；仅当second_count>0时0x42452F再draw，RNG%second_count，从+0x70数组取第二候选值。',
                # 给当前结果字典的“每次成功helper调用draw数”字段填写 `'1 + (LayoutGR+0x6C>0 ? 1 : 0)',`，这样导出的 JSON/表格能保留这一项证据。
                '每次成功helper调用draw数': '1 + (LayoutGR+0x6C>0 ? 1 : 0)',
            },
            # 给当前结果字典的“边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
            '边界': [
                # 继续填写当前数据结构或参数列表中的一项：`'这里闭合的是opcode0x37→Battle核心reseed的直接调用顺序，以及0x442040/0x424430中显式RNG call的条件顺序。',`。
                '这里闭合的是opcode0x37→Battle核心reseed的直接调用顺序，以及0x442040/0x424430中显式RNG call的条件顺序。',
                # 继续填写当前数据结构或参数列表中的一项：`'“重新播种后的第一批draw”不得外推成整个Battle所有间接callee均已穷尽；后续继续按56个直接site、callee与状态机做静态闭合。',`。
                '“重新播种后的第一批draw”不得外推成整个Battle所有间接callee均已穷尽；后续继续按56个直接site、callee与状态机做静态闭合。',
            ],
        },
        # 给当前结果字典的“AI目标与动作准备静态draw顺序”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'AI目标与动作准备静态draw顺序': {
            # 给当前结果字典的“AI_0x444240”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'AI_0x444240': {
                # 给当前结果字典的“固定首draw”字段填写 `'每次调用0x444240先在0x444267 draw一次，RNG%100。',`，这样导出的 JSON/表格能保留这一项证据。
                '固定首draw': '每次调用0x444240先在0x444267 draw一次，RNG%100。',
                # 给当前结果字典的“阈值”字段填写 `'读取当前56-byte Enemy_AI record +0x1C与+0x20；只有roll落入中间区间才调用0x444330。',`，这样导出的 JSON/表格能保留这一项证据。
                '阈值': '读取当前56-byte Enemy_AI record +0x1C与+0x20；只有roll落入中间区间才调用0x444330。',
                # 给当前结果字典的“逐候选评分”字段填写 `'0x444330循环FightRole+0x8F4个候选action ID（列表+0x8F8），每个候选无条件在0x4444E1各dra…`，这样导出的 JSON/表格能保留这一项证据。
                '逐候选评分': '0x444330循环FightRole+0x8F4个候选action ID（列表+0x8F8），每个候选无条件在0x4444E1各draw一次；即使候选经0x444590判无效，draw仍已发生。有效候选才对MagicCon[candidate]+0x24取模生成评分。',
                # 给当前结果字典的“显式draw数”字段填写 `'1 或 1+FightRole+0x8F4；取决于首draw是否进入0x444330中间阈值分支。',`，这样导出的 JSON/表格能保留这一项证据。
                '显式draw数': '1 或 1+FightRole+0x8F4；取决于首draw是否进入0x444330中间阈值分支。',
            },
            # 给当前结果字典的“目标选择_0x422CB0”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '目标选择_0x422CB0': {
                # 给当前结果字典的“随机点”字段填写 `[hexva(0x00422DF2), hexva(0x00422E6C)],`，这样导出的 JSON/表格能保留这一项证据。
                '随机点': [hexva(0x00422DF2), hexva(0x00422E6C)],
                # 给当前结果字典的“规则”字段填写 `'两条路径互斥；由target selector与FightRole+0x850决定选择哪一类候选列表。所选列表count>0时恰1 dr…`，这样导出的 JSON/表格能保留这一项证据。
                '规则': '两条路径互斥；由target selector与FightRole+0x850决定选择哪一类候选列表。所选列表count>0时恰1 draw%count；count==0直接-1且不draw。selector 2/4存在直接目标返回路径，不经这两个随机点。',
            },
            # 给当前结果字典的“动作准备_0x422F00”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '动作准备_0x422F00': {
                # 给当前结果字典的“随机点”字段填写 `hexva(0x00422F1F),`，这样导出的 JSON/表格能保留这一项证据。
                '随机点': hexva(0x00422F1F),
                # 给当前结果字典的“条件”字段填写 `'action mode=1、FightRole+0x8F4>0、调用者未传入正的显式action ID。',`，这样导出的 JSON/表格能保留这一项证据。
                '条件': 'action mode=1、FightRole+0x8F4>0、调用者未传入正的显式action ID。',
                # 给当前结果字典的“变换”字段填写 `'RNG%FightRole+0x8F4，索引FightRole+0x8F8[]取得action ID。',`，这样导出的 JSON/表格能保留这一项证据。
                '变换': 'RNG%FightRole+0x8F4，索引FightRole+0x8F8[]取得action ID。',
                # 给当前结果字典的“交叉”字段填写 `'与AI 0x444330使用同一候选action池。',`，这样导出的 JSON/表格能保留这一项证据。
                '交叉': '与AI 0x444330使用同一候选action池。',
            },
            # 给当前结果字典的“FIRTTECH_0x1EA”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'FIRTTECH_0x1EA': {
                # 给当前结果字典的“作者名称”字段填写 `'戰場脫逃（名称由Firttech.enc原始CP950记录独立证明；本EXE命令只验证ID与控制流）',`，这样导出的 JSON/表格能保留这一项证据。
                '作者名称': '戰場脫逃（名称由Firttech.enc原始CP950记录独立证明；本EXE命令只验证ID与控制流）',
                # 给当前结果字典的“随机点”字段填写 `hexva(0x004222A2),`，这样导出的 JSON/表格能保留这一项证据。
                '随机点': hexva(0x004222A2),
                # 给当前结果字典的“规则”字段填写 `'仅当前FIRTTECH ID==0x1EA时执行1次RNG%100，与0x424560返回阈值比较。',`，这样导出的 JSON/表格能保留这一项证据。
                '规则': '仅当前FIRTTECH ID==0x1EA时执行1次RNG%100，与0x424560返回阈值比较。',
                # 固化62同步：这一字段不再是待拆黑箱；下面明确写入已经由专用调查器闭合的LayoutGR+0x9C脱逃阈值规则，同时继续保留作者正式列名UNKNOWN边界。
                '边界': '固化62现行：0x424560在canonical有效LayoutGR索引下直接返回record+0x9C；FIRTTECH 0x1EA「戰場脫逃」随后严格执行legacy rand15()%100 < 该值。兼容层可称escape_success_threshold，但作者正式数据库列名仍UNKNOWN。',
            },
        },
        # 给当前结果字典的“公式与效果区静态draw顺序”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '公式与效果区静态draw顺序': {
            # 给当前结果字典的“通用半开区间随机_0x42CCB0”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '通用半开区间随机_0x42CCB0': {
                # 给当前结果字典的“规则”字段填写 `'先排序两个整数边界；相等时直接返回且0 draw；不等时1 draw，返回 min + RNG%(max-min)，严格区间[min,m…`，这样导出的 JSON/表格能保留这一项证据。
                '规则': '先排序两个整数边界；相等时直接返回且0 draw；不等时1 draw，返回 min + RNG%(max-min)，严格区间[min,max)。',
                # 给当前结果字典的“直接caller”字段填写 `[hexva(x) for x in [0x0042B68D,0x0042B71D,0x0042B7A5,0x0042B80A,0x004…`，这样导出的 JSON/表格能保留这一项证据。
                '直接caller': [hexva(x) for x in [0x0042B68D,0x0042B71D,0x0042B7A5,0x0042B80A,0x0042BCAC,0x0042BCFC]],
                # 给当前结果字典的“同步陷阱”字段填写 `'上下界相等时不得仍推进RNG；上界严格不包含。',`，这样导出的 JSON/表格能保留这一项证据。
                '同步陷阱': '上下界相等时不得仍推进RNG；上界严格不包含。',
            },
            # 给当前结果字典的“Article_Ail2_0x42A7D0”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'Article_Ail2_0x42A7D0': {
                # 给当前结果字典的“前置概率门”字段填写 `'只有0x42BF40与RoleDefinition+0x220/+0x224等前置状态均不能直接决定时，0x42A942才1 draw%…`，这样导出的 JSON/表格能保留这一项证据。
                '前置概率门': '只有0x42BF40与RoleDefinition+0x220/+0x224等前置状态均不能直接决定时，0x42A942才1 draw%100，与Ail2+0xCC阈值比较。',
                # 给当前结果字典的“Ail2+0xC8==0”字段填写 `{'通过门后固定draw数':4,'模数顺序':[9,19,9,19],'调用点':[hexva(x) for x in [0x0042A…`，这样导出的 JSON/表格能保留这一项证据。
                'Ail2+0xC8==0': {'通过门后固定draw数':4,'模数顺序':[9,19,9,19],'调用点':[hexva(x) for x in [0x0042AAD3,0x0042AAEE,0x0042AB14,0x0042AB33]]},
                # 给当前结果字典的“Ail2+0xC8==1”字段填写 `{'通过门后固定draw数':4,'模数顺序':[9,19,99,19],'调用点':[hexva(x) for x in [0x0042…`，这样导出的 JSON/表格能保留这一项证据。
                'Ail2+0xC8==1': {'通过门后固定draw数':4,'模数顺序':[9,19,99,19],'调用点':[hexva(x) for x in [0x0042AC95,0x0042ACB0,0x0042ACCC,0x0042ACEB]]},
                # 给当前结果字典的“边界”字段填写 `'这里只冻结draw条件/模数与record字段偏移；+0xC8的作者业务枚举名仍未证明。',`，这样导出的 JSON/表格能保留这一项证据。
                '边界': '这里只冻结draw条件/模数与record字段偏移；+0xC8的作者业务枚举名仍未证明。',
            },
            # 给当前结果字典的“五槽effect_0x42B0D0”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '五槽effect_0x42B0D0': {
                # 给当前结果字典的“slot数”字段填写 `5,`，这样导出的 JSON/表格能保留这一项证据。
                'slot数':5,
                # 给当前结果字典的“slot_value<=0”字段填写 `'0 draw，直接跳过该槽',`，这样导出的 JSON/表格能保留这一项证据。
                'slot_value<=0':'0 draw，直接跳过该槽',
                # 给当前结果字典的“effect_ID>27”字段填写 `'0 draw，走ID原样+value=8000特殊输出分支',`，这样导出的 JSON/表格能保留这一项证据。
                'effect_ID>27':'0 draw，走ID原样+value=8000特殊输出分支',
                # 给当前结果字典的“effect_ID_0到27_第一draw”字段填写 `'固定1 draw%100；ID21..27虽阈值强制100、概率门必过，但这次draw仍发生。',`，这样导出的 JSON/表格能保留这一项证据。
                'effect_ID_0到27_第一draw':'固定1 draw%100；ID21..27虽阈值强制100、概率门必过，但这次draw仍发生。',
                # 给当前结果字典的“第二draw”字段填写 `'仅概率门通过且派生range=trunc(RoleDefinition+0x4C/5)-trunc(RoleDefinition+0x4…`，这样导出的 JSON/表格能保留这一项证据。
                '第二draw':'仅概率门通过且派生range=trunc(RoleDefinition+0x4C/5)-trunc(RoleDefinition+0x4C/10) >=1时发生；RNG%range后加low=trunc(+0x4C/10)，形成[low,high)随机量。range<1时直接0且不draw。',
                # 给当前结果字典的“每槽显式draw数”字段填写 `'0 / 1 / 2，取决于value、ID分支、概率门和派生range。',`，这样导出的 JSON/表格能保留这一项证据。
                '每槽显式draw数':'0 / 1 / 2，取决于value、ID分支、概率门和派生range。',
            },
            # 给当前结果字典的“FIRTTECH主处理_0x42C680”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'FIRTTECH主处理_0x42C680': {
                # 给当前结果字典的“record+0x18==0”字段填写 `'第一门固定1 draw%100；失败立即结束。通过后第二门再固定1 draw%100；随后0x42CD50按派生模数决定是否额外1 dr…`，这样导出的 JSON/表格能保留这一项证据。
                'record+0x18==0': '第一门固定1 draw%100；失败立即结束。通过后第二门再固定1 draw%100；随后0x42CD50按派生模数决定是否额外1 draw。',
                # 给当前结果字典的“record+0x18非0且非4”字段填写 `'先由record+0x30选择公式helper；当前selector路径最终均可触及0x42CCB0半开区间helper，但边界相等时该…`，这样导出的 JSON/表格能保留这一项证据。
                'record+0x18非0且非4': '先由record+0x30选择公式helper；当前selector路径最终均可触及0x42CCB0半开区间helper，但边界相等时该helper不draw；随后0x42CD50仍为条件0/1 draw。',
                # 给当前结果字典的“后处理概率门_0x42C8B7”字段填写 `'前置helper/状态能直接决定时0 draw；否则1 draw%100比较派生阈值。',`，这样导出的 JSON/表格能保留这一项证据。
                '后处理概率门_0x42C8B7': '前置helper/状态能直接决定时0 draw；否则1 draw%100比较派生阈值。',
                # 给当前结果字典的“record+0x44==0且后处理门成立”字段填写 `'调用0x42BF90，固定2 draw：先RNG%19+1作除数，再RNG%9+1作除数。',`，这样导出的 JSON/表格能保留这一项证据。
                'record+0x44==0且后处理门成立': '调用0x42BF90，固定2 draw：先RNG%19+1作除数，再RNG%9+1作除数。',
                # 给当前结果字典的“record+0x44==1且后处理门成立”字段填写 `'调用0x42C260，固定2 draw；第1除数=floor(raw_rng/99)+1，第2除数=floor(raw_rng/19)+…`，这样导出的 JSON/表格能保留这一项证据。
                'record+0x44==1且后处理门成立': '调用0x42C260，固定2 draw；第1除数=floor(raw_rng/99)+1，第2除数=floor(raw_rng/19)+1，注意不是取模。',
                # 给当前结果字典的“record+0x44==2或3”字段填写 `'0x42C460本体无0x451565直接call；当前只冻结本体显式draw为0，不外推未知间接路径。',`，这样导出的 JSON/表格能保留这一项证据。
                'record+0x44==2或3': '0x42C460本体无0x451565直接call；当前只冻结本体显式draw为0，不外推未知间接路径。',
            },
            # 给当前结果字典的“记录派生条件随机_0x42CD50”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '记录派生条件随机_0x42CD50': {
                # 给当前结果字典的“条件”字段填写 `'输入record+0x90>0，并由全局记录字段/20得到的派生模数>0。',`，这样导出的 JSON/表格能保留这一项证据。
                '条件': '输入record+0x90>0，并由全局记录字段/20得到的派生模数>0。',
                # 给当前结果字典的“draw”字段填写 `'条件成立1 draw，返回RNG%derived_modulus；否则返回0且不draw。',`，这样导出的 JSON/表格能保留这一项证据。
                'draw': '条件成立1 draw，返回RNG%derived_modulus；否则返回0且不draw。',
                # 给当前结果字典的“直接caller”字段填写 `[hexva(0x0042C79B),hexva(0x0042C845)],`，这样导出的 JSON/表格能保留这一项证据。
                '直接caller': [hexva(0x0042C79B),hexva(0x0042C845)],
                # 给当前结果字典的“边界”字段填写 `'全局记录类型/作者字段名仍待独立闭合，因此保持结构命名。',`，这样导出的 JSON/表格能保留这一项证据。
                '边界': '全局记录类型/作者字段名仍待独立闭合，因此保持结构命名。',
            },
        },
        # 给当前结果字典的“固化22_非Battle_core_11点静态语义细分”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '固化22_非Battle_core_11点静态语义细分': {
            # 给当前结果字典的“总点数”字段填写 `11,`，这样导出的 JSON/表格能保留这一项证据。
            '总点数': 11,
            # 给当前结果字典的“窗口创建初始化_2点”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '窗口创建初始化_2点': {
                # 给当前结果字典的“调用点”字段填写 `[hexva(0x00401109), hexva(0x0040113D)],`，这样导出的 JSON/表格能保留这一项证据。
                '调用点': [hexva(0x00401109), hexva(0x0040113D)],
                # 给当前结果字典的“静态闭合”字段填写 `'0x401000先用0x451583结果调用seed setter 0x45155B；随后0x401109生成0..3随机字段，0x40…`，这样导出的 JSON/表格能保留这一项证据。
                '静态闭合': '0x401000先用0x451583结果调用seed setter 0x45155B；随后0x401109生成0..3随机字段，0x40113D在ESI=1..30循环中生成RNG%0x55+1。0x401000的直接调用位于窗口消息分派message==1分支0x401A23。',
                # 给当前结果字典的“边界”字段填写 `'这里只把它归入窗口创建/启动初始化，不擅自命名DataCenter-like对象+0xBC或+0x90子结构的作者业务字段名。',`，这样导出的 JSON/表格能保留这一项证据。
                '边界': '这里只把它归入窗口创建/启动初始化，不擅自命名DataCenter-like对象+0xBC或+0x90子结构的作者业务字段名。',
            },
            # 给当前结果字典的“随机遭遇阈值_2点”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '随机遭遇阈值_2点': {
                # 给当前结果字典的“调用点”字段填写 `[hexva(0x0040352E), hexva(0x00403718)],`，这样导出的 JSON/表格能保留这一项证据。
                '调用点': [hexva(0x0040352E), hexva(0x00403718)],
                # 给当前结果字典的“静态闭合”字段填写 `'0x403510在启用时先写base=150，再以2*base为除数并+base，形成基础RNG%300+150；0x4035A0运行更…`，这样导出的 JSON/表格能保留这一项证据。
                '静态闭合': '0x403510在启用时先写base=150，再以2*base为除数并+base，形成基础RNG%300+150；0x4035A0运行更新满足门条件后在0x403718续订同型阈值。0x4093C2与0x40B158分别提供Map/SCI装载链和world update caller。',
                # 给当前结果字典的“交叉证据”字段填写 `'既有SCI字段规格与世界特殊物品动作证据已把该状态闭合为encounter trigger threshold；持续态13/14可进一步…`，这样导出的 JSON/表格能保留这一项证据。
                '交叉证据': '既有SCI字段规格与世界特殊物品动作证据已把该状态闭合为encounter trigger threshold；持续态13/14可进一步缩放该阈值。',
            },
            # 给当前结果字典的“Entity_NPC漫游行为_4点”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'Entity_NPC漫游行为_4点': {
                # 给当前结果字典的“调用点”字段填写 `[hexva(x) for x in [0x0040AB72,0x0040AB86,0x0040AB9A,0x0040ABD3]],`，这样导出的 JSON/表格能保留这一项证据。
                '调用点': [hexva(x) for x in [0x0040AB72,0x0040AB86,0x0040AB9A,0x0040ABD3]],
                # 给当前结果字典的“静态闭合”字段填写 `'0x40AAF0要求serialized entity record+0x76==3。0x40AB72/86分别以record+0xEF…`，这样导出的 JSON/表格能保留这一项证据。
                '静态闭合': '0x40AAF0要求serialized entity record+0x76==3。0x40AB72/86分别以record+0xEF..+0xF7和+0xF3..+0xFB形成两个随机坐标区间并写运行态+0x2C/+0x30；0x40AB9A生成0/1写+0x34；0x40ABD3生成RNG%30+30写等待阈值+0x64。',
                # 给当前结果字典的“边界”字段填写 `'+0x34只冻结为运行态0/1随机，不擅自命名方向/朝向；坐标X/Y命名来自既有SCI实体范围字段交叉证据。',`，这样导出的 JSON/表格能保留这一项证据。
                '边界': '+0x34只冻结为运行态0/1随机，不擅自命名方向/朝向；坐标X/Y命名来自既有SCI实体范围字段交叉证据。',
            },
            # 给当前结果字典的“EVE_RANDOM_TO_VAR999_1点”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'EVE_RANDOM_TO_VAR999_1点': {
                # 给当前结果字典的“调用点”字段填写 `[hexva(0x0040F633)],`，这样导出的 JSON/表格能保留这一项证据。
                '调用点': [hexva(0x0040F633)],
                # 给当前结果字典的“静态闭合”字段填写 `'EVE opcode 0x99处理器0x40F620读取命令+4/+8，形成(end-start+1)区间宽度，RNG取余后+start…`，这样导出的 JSON/表格能保留这一项证据。
                '静态闭合': 'EVE opcode 0x99处理器0x40F620读取命令+4/+8，形成(end-start+1)区间宽度，RNG取余后+start，并以变量索引999(0x3E7)写入。',
            },
            # 给当前结果字典的“EVE相机jitter_2点”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'EVE相机jitter_2点': {
                # 给当前结果字典的“调用点”字段填写 `[hexva(0x0044B52C),hexva(0x0044B54D)],`，这样导出的 JSON/表格能保留这一项证据。
                '调用点': [hexva(0x0044B52C),hexva(0x0044B54D)],
                # 给当前结果字典的“静态闭合”字段填写 `'0x44B500在jitter启用且计数门满足时先后生成X/Y随机偏移；两轴核心均为RNG%(2*radius)-radius，再叠加各…`，这样导出的 JSON/表格能保留这一项证据。
                '静态闭合': '0x44B500在jitter启用且计数门满足时先后生成X/Y随机偏移；两轴核心均为RNG%(2*radius)-radius，再叠加各自bias/origin。0x40B05D为apply caller；EVE opcode 0x58处理器0x40E120经0x40E13D调用配置setter 0x44B4B0。',
            },
            # 给当前结果字典的“动态边界”字段填写 `'以上完成的是11个直接caller的静态业务所属细分；RNG state@0x46E29C全局共享。当前阶段只冻结静态调用所属与算法语义…`，这样导出的 JSON/表格能保留这一项证据。
            '动态边界': '以上完成的是11个直接caller的静态业务所属细分；RNG state@0x46E29C全局共享。当前阶段只冻结静态调用所属与算法语义，不把“是否在某一运行窗口执行”升级为已证事实；在游戏数据与逻辑彻底静态拆解完成前，不开展运行时注入/探针工作。',
        },
        # 给当前结果字典的“固化21_剩余直接RNG分类”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '固化21_剩余直接RNG分类': {
            # 给当前结果字典的“总直接调用点”字段填写 `len(refs),`，这样导出的 JSON/表格能保留这一项证据。
            '总直接调用点': len(refs),
            # 给当前结果字典的“固化20后机器差集_21点”字段填写 `[hexva(x) for x in [`，这样导出的 JSON/表格能保留这一项证据。
            '固化20后机器差集_21点': [hexva(x) for x in [
                # 继续填写当前数据结构或参数列表中的一项：`0x00401109,0x0040113D,0x0040352E,0x00403718,0x0040AB72,0x0040AB86,0x0040AB9A,0x0040ABD3,0x0040…`。
                0x00401109,0x0040113D,0x0040352E,0x00403718,0x0040AB72,0x0040AB86,0x0040AB9A,0x0040ABD3,0x0040F633,
                # 继续填写当前数据结构或参数列表中的一项：`0x0041F33E,0x0041F454,0x00421AB6,0x00421B20,0x00423395,0x0043D352,0x00443DB3,0x00443DDA,0x0044…`。
                0x0041F33E,0x0041F454,0x00421AB6,0x00421B20,0x00423395,0x0043D352,0x00443DB3,0x00443DDA,0x00443E24,0x0044674B,
                # 继续填写当前数据结构或参数列表中的一项：`0x0044B52C,0x0044B54D]],`。
                0x0044B52C,0x0044B54D]],
            # 给当前结果字典的“Battle_core或Battle直接关联_10点”字段填写 `[hexva(x) for x in [0x0041F33E,0x0041F454,0x00421AB6,0x00421B20,0x004…`，这样导出的 JSON/表格能保留这一项证据。
            'Battle_core或Battle直接关联_10点': [hexva(x) for x in [0x0041F33E,0x0041F454,0x00421AB6,0x00421B20,0x00423395,0x0043D352,0x00443DB3,0x00443DDA,0x00443E24,0x0044674B]],
            # 给当前结果字典的“非Battle_core直接点_11点”字段填写 `[hexva(x) for x in [0x00401109,0x0040113D,0x0040352E,0x00403718,0x004…`，这样导出的 JSON/表格能保留这一项证据。
            '非Battle_core直接点_11点': [hexva(x) for x in [0x00401109,0x0040113D,0x0040352E,0x00403718,0x0040AB72,0x0040AB86,0x0040AB9A,0x0040ABD3,0x0040F633,0x0044B52C,0x0044B54D]],
            # 给当前结果字典的“FightRole构造_0x41F2B0”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'FightRole构造_0x41F2B0': {
                # 给当前结果字典的“条件”字段填写 `'仅0x41F580成功路径进入这两次直接draw。',`，这样导出的 JSON/表格能保留这一项证据。
                '条件': '仅0x41F580成功路径进入这两次直接draw。',
                # 给当前结果字典的“draw1”字段填写 `'0x41F33E：raw RNG压到0/1，传给表现/控制子对象。',`，这样导出的 JSON/表格能保留这一项证据。
                'draw1': '0x41F33E：raw RNG压到0/1，传给表现/控制子对象。',
                # 给当前结果字典的“draw2”字段填写 `'0x41F454：RNG%40000+80000，传入FightRole内部控制对象。',`，这样导出的 JSON/表格能保留这一项证据。
                'draw2': '0x41F454：RNG%40000+80000，传入FightRole内部控制对象。',
                # 给当前结果字典的“显式draw数”字段填写 `'成功初始化路径固定2；失败路径0。',`，这样导出的 JSON/表格能保留这一项证据。
                '显式draw数': '成功初始化路径固定2；失败路径0。',
            },
            # 给当前结果字典的“动作内部状态_0x421A00”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '动作内部状态_0x421A00': {
                # 给当前结果字典的“draw1_0x421AB6”字段填写 `'满足前置索引/计数条件时1 draw；low=floor((n-1)/2)，mod=floor(n/2)-floor((n-1)/2)+…`，这样导出的 JSON/表格能保留这一项证据。
                'draw1_0x421AB6': '满足前置索引/计数条件时1 draw；low=floor((n-1)/2)，mod=floor(n/2)-floor((n-1)/2)+1，结果=low+RNG%mod。n为正值时mod只会是1或2；即便mod=1仍发生draw。',
                # 给当前结果字典的“draw2_0x421B20”字段填写 `'随后仅派生阈值>0时再1 draw%100；成功后+0x984[index]递增并cap100，同时+0x998[index]清0。',`，这样导出的 JSON/表格能保留这一项证据。
                'draw2_0x421B20': '随后仅派生阈值>0时再1 draw%100；成功后+0x984[index]递增并cap100，同时+0x998[index]清0。',
                # 给当前结果字典的“边界”字段填写 `'字段业务名尚未独立证明，保持结构名。',`，这样导出的 JSON/表格能保留这一项证据。
                '边界': '字段业务名尚未独立证明，保持结构名。',
            },
            # 给当前结果字典的“候选注入_0x4232D0”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '候选注入_0x4232D0': {
                # 给当前结果字典的“caller”字段填写 `'0x442D8E BattleManager更新链',`，这样导出的 JSON/表格能保留这一项证据。
                'caller': '0x442D8E BattleManager更新链',
                # 给当前结果字典的“规则”字段填写 `'通过函数前置条件与index 1..5筛选后，0x423395固定1 draw%100；阈值由FightRole+0x984[index…`，这样导出的 JSON/表格能保留这一项证据。
                '规则': '通过函数前置条件与index 1..5筛选后，0x423395固定1 draw%100；阈值由FightRole+0x984[index]及相关运行字段派生。成功后可把表内action ID送入0x423100加入候选。',
            },
            # 给当前结果字典的“Battle表现对象_0x43D300”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            'Battle表现对象_0x43D300': {
                # 给当前结果字典的“caller”字段填写 `'0x422384 Battle动作/表现链',`，这样导出的 JSON/表格能保留这一项证据。
                'caller': '0x422384 Battle动作/表现链',
                # 给当前结果字典的“规则”字段填写 `'对象+0x14==0的首次资源初始化才在0x43D352消耗1 draw并取0/1写+0x10；之后+0x14=1，重复调用不draw。…`，这样导出的 JSON/表格能保留这一项证据。
                '规则': '对象+0x14==0的首次资源初始化才在0x43D352消耗1 draw并取0/1写+0x10；之后+0x14=1，重复调用不draw。',
                # 给当前结果字典的“分类”字段填写 `'Battle-associated表现层；不是伤害/AI core，但会推进共享全局RNG。',`，这样导出的 JSON/表格能保留这一项证据。
                '分类': 'Battle-associated表现层；不是伤害/AI core，但会推进共享全局RNG。',
            },
            # 给当前结果字典的“特殊召唤_0x443D90”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '特殊召唤_0x443D90': {
                # 给当前结果字典的“FIRTTECH_0x161”字段填写 `'原ENC名称「呼叫增援」；0x443DDA先1 draw二选一，选择0x7D/0x7E。',`，这样导出的 JSON/表格能保留这一项证据。
                'FIRTTECH_0x161': '原ENC名称「呼叫增援」；0x443DDA先1 draw二选一，选择0x7D/0x7E。',
                # 给当前结果字典的“FIRTTECH_0x169”字段填写 `'原ENC名称「屍魔召喚」；0x443DB3先1 draw四选一，选择0x91/0x92/0x93/0x94。',`，这样导出的 JSON/表格能保留这一项证据。
                'FIRTTECH_0x169': '原ENC名称「屍魔召喚」；0x443DB3先1 draw四选一，选择0x91/0x92/0x93/0x94。',
                # 给当前结果字典的“共同数量draw”字段填写 `'0x443E24再1 draw，结果+1，要求创建1或2个。',`，这样导出的 JSON/表格能保留这一项证据。
                '共同数量draw': '0x443E24再1 draw，结果+1，要求创建1或2个。',
                # 给当前结果字典的“间接draw”字段填写 `'每个实际进入0x443F39→0x41F2B0且成功的FightRole构造会额外消费2 draw。',`，这样导出的 JSON/表格能保留这一项证据。
                '间接draw': '每个实际进入0x443F39→0x41F2B0且成功的FightRole构造会额外消费2 draw。',
                # 给当前结果字典的“静态边界”字段填写 `'实际创建数量受分支和构造成功次数影响，不能把FIRTTECH固定写成2次总draw；静态规格只冻结直接draw与每次成功FightRol…`，这样导出的 JSON/表格能保留这一项证据。
                '静态边界': '实际创建数量受分支和构造成功次数影响，不能把FIRTTECH固定写成2次总draw；静态规格只冻结直接draw与每次成功FightRole构造额外2 draw。',
            },
            # 给当前结果字典的“随机FightRole槽_0x44674B”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
            '随机FightRole槽_0x44674B': {
                # 给当前结果字典的“前置”字段填写 `'存在候选且重试预算初值100。',`，这样导出的 JSON/表格能保留这一项证据。
                '前置': '存在候选且重试预算初值100。',
                # 给当前结果字典的“每次尝试”字段填写 `'1 draw，索引压到0..7；随后按两组布尔条件和映射有效性校验。',`，这样导出的 JSON/表格能保留这一项证据。
                '每次尝试': '1 draw，索引压到0..7；随后按两组布尔条件和映射有效性校验。',
                # 给当前结果字典的“重试”字段填写 `'失败则回到0x446747继续，预算每draw减1；因此显式draw数为0或1..100，取决于前置与找到合法槽所需尝试次数。',`，这样导出的 JSON/表格能保留这一项证据。
                '重试': '失败则回到0x446747继续，预算每draw减1；因此显式draw数为0或1..100，取决于前置与找到合法槽所需尝试次数。',
            },
            # 给当前结果字典的“非Battle_core边界”字段填写 `'11个剩余直接点已有模块/调用链证据不属于Battle core；但RNG state 0x46E29C为全局共享。当前只冻结静态所属，…`，这样导出的 JSON/表格能保留这一项证据。
            '非Battle_core边界': '11个剩余直接点已有模块/调用链证据不属于Battle core；但RNG state 0x46E29C为全局共享。当前只冻结静态所属，不把任何跨状态运行关系升级为已证事实；继续从调用者/状态机静态闭合。',
        },
        # 给当前结果字典的“胜利结算静态draw顺序”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '胜利结算静态draw顺序': [
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {
                # 给当前结果字典的“顺序”字段填写 `1,`，这样导出的 JSON/表格能保留这一项证据。
                '顺序': 1,
                # 给当前结果字典的“类别”字段填写 `'銀兩随机范围',`，这样导出的 JSON/表格能保留这一项证据。
                '类别': '銀兩随机范围',
                # 给当前结果字典的“调用点”字段填写 `hexva(0x00443759),`，这样导出的 JSON/表格能保留这一项证据。
                '调用点': hexva(0x00443759),
                # 给当前结果字典的“发生条件”字段填写 `'LayoutTeam奖励随机范围(+0x78 / manager镜像+0xE3A9) != 0',`，这样导出的 JSON/表格能保留这一项证据。
                '发生条件': 'LayoutTeam奖励随机范围(+0x78 / manager镜像+0xE3A9) != 0',
                # 给当前结果字典的“变换”字段填写 `'RNG % range；随后加base。注意随机值先生成，銀兩实际写入Bank发生在角色升级处理之后。',`，这样导出的 JSON/表格能保留这一项证据。
                '变换': 'RNG % range；随后加base。注意随机值先生成，銀兩实际写入Bank发生在角色升级处理之后。',
            },
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {
                # 给当前结果字典的“顺序”字段填写 `2,`，这样导出的 JSON/表格能保留这一项证据。
                '顺序': 2,
                # 给当前结果字典的“类别”字段填写 `'8个角色槽的条件升级',`，这样导出的 JSON/表格能保留这一项证据。
                '类别': '8个角色槽的条件升级',
                # 给当前结果字典的“调用入口”字段填写 `hexva(0x0044387B),`，这样导出的 JSON/表格能保留这一项证据。
                '调用入口': hexva(0x0044387B),
                # 给当前结果字典的“helper”字段填写 `hexva(0x00443B60),`，这样导出的 JSON/表格能保留这一项证据。
                'helper': hexva(0x00443B60),
                # 给当前结果字典的“槽顺序”字段填写 `'起点+slot*0xDE4，slot 0..7',`，这样导出的 JSON/表格能保留这一项证据。
                '槽顺序': '起点+slot*0xDE4，slot 0..7',
                # 给当前结果字典的“成功升级时直接draw”字段填写 `[hexva(0x00443BB4), hexva(0x00443BF6)],`，这样导出的 JSON/表格能保留这一项证据。
                '成功升级时直接draw': [hexva(0x00443BB4), hexva(0x00443BF6)],
                # 给当前结果字典的“直接draw变换”字段填写 `['RNG % 3', 'RNG % 3'],`，这样导出的 JSON/表格能保留这一项证据。
                '直接draw变换': ['RNG % 3', 'RNG % 3'],
                # 给当前结果字典的“随后五个增长helper调用点”字段填写 `[hexva(x) for x in [0x00443C55,0x00443C74,0x00443C93,0x00443CB2,0x004…`，这样导出的 JSON/表格能保留这一项证据。
                '随后五个增长helper调用点': [hexva(x) for x in [0x00443C55,0x00443C74,0x00443C93,0x00443CB2,0x00443CD1]],
                # 给当前结果字典的“增长helper_RNG调用点”字段填写 `hexva(0x00443D77),`，这样导出的 JSON/表格能保留这一项证据。
                '增长helper_RNG调用点': hexva(0x00443D77),
                # 给当前结果字典的“增长helper_draw条件”字段填写 `'role_id in 1..6 且 增长索引 in 0..4',`，这样导出的 JSON/表格能保留这一项证据。
                '增长helper_draw条件': 'role_id in 1..6 且 增长索引 in 0..4',
                # 给当前结果字典的“主角色成功升级总draw数”字段填写 `7,`，这样导出的 JSON/表格能保留这一项证据。
                '主角色成功升级总draw数': 7,
                # 给当前结果字典的“边界”字段填写 `'若升级条件不成立，0x443B60在RNG前返回；若role_id不在1..6，五个0x443D40调用均返回0且不draw。',`，这样导出的 JSON/表格能保留这一项证据。
                '边界': '若升级条件不成立，0x443B60在RNG前返回；若role_id不在1..6，五个0x443D40调用均返回0且不draw。',
            },
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {
                # 给当前结果字典的“顺序”字段填写 `3,`，这样导出的 JSON/表格能保留这一项证据。
                '顺序': 3,
                # 给当前结果字典的“类别”字段填写 `'掉落槽判定',`，这样导出的 JSON/表格能保留这一项证据。
                '类别': '掉落槽判定',
                # 给当前结果字典的“调用点”字段填写 `hexva(0x00443A1D),`，这样导出的 JSON/表格能保留这一项证据。
                '调用点': hexva(0x00443A1D),
                # 给当前结果字典的“循环”字段填写 `'最多4槽，受drop_count与每槽threshold约束',`，这样导出的 JSON/表格能保留这一项证据。
                '循环': '最多4槽，受drop_count与每槽threshold约束',
                # 给当前结果字典的“发生条件”字段填写 `'该槽threshold > 0',`，这样导出的 JSON/表格能保留这一项证据。
                '发生条件': '该槽threshold > 0',
                # 给当前结果字典的“变换”字段填写 `'RNG % 100；成功条件 remainder <= threshold（保留等号）',`，这样导出的 JSON/表格能保留这一项证据。
                '变换': 'RNG % 100；成功条件 remainder <= threshold（保留等号）',
            },
        ],
        # 给当前结果字典的“升级五项随机增长参数”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '升级五项随机增长参数': {
            # 给当前结果字典的“类别基础值”字段填写 `bases,`，这样导出的 JSON/表格能保留这一项证据。
            '类别基础值': bases,
            # 给当前结果字典的“类别range_minus_one”字段填写 `range_minus_one,`，这样导出的 JSON/表格能保留这一项证据。
            '类别range_minus_one': range_minus_one,
            # 给当前结果字典的“类别实际RNG模数”字段填写 `[x + 1 for x in range_minus_one],`，这样导出的 JSON/表格能保留这一项证据。
            '类别实际RNG模数': [x + 1 for x in range_minus_one],
            # 给当前结果字典的“角色到类别与范围”字段填写 `role_rows,`，这样导出的 JSON/表格能保留这一项证据。
            '角色到类别与范围': role_rows,
            # 给当前结果字典的“业务字段边界”字段填写 `'五项索引0..4分别写RoleDefinition+0x3C/+0x40/+0x44/+0x48/+0x4C；作者属性名未全部证明，保持…`，这样导出的 JSON/表格能保留这一项证据。
            '业务字段边界': '五项索引0..4分别写RoleDefinition+0x3C/+0x40/+0x44/+0x48/+0x4C；作者属性名未全部证明，保持索引/偏移结构名。',
        },
        # 给当前结果字典的“未来确定性重放需要保留的静态信息”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '未来确定性重放需要保留的静态信息': [
            # 继续填写当前数据结构或参数列表中的一项：`'seed/state_before/state_after',`。
            'seed/state_before/state_after',
            # 继续填写当前数据结构或参数列表中的一项：`'global_draw_index',`。
            'global_draw_index',
            # 继续填写当前数据结构或参数列表中的一项：`'caller_va / static_category',`。
            'caller_va / static_category',
            # 继续填写当前数据结构或参数列表中的一项：`'raw_rng_0_32767',`。
            'raw_rng_0_32767',
            # 继续填写当前数据结构或参数列表中的一项：`'modulus与派生remainder（若有）',`。
            'modulus与派生remainder（若有）',
            # 继续填写当前数据结构或参数列表中的一项：`'battle phase / role slot / action / target（能证明时记录）',`。
            'battle phase / role slot / action / target（能证明时记录）',
            # 继续填写当前数据结构或参数列表中的一项：`'最关键：保留条件draw的触发谓词，不能用固定每阶段draw数代替。',`。
            '最关键：保留条件draw的触发谓词，不能用固定每阶段draw数代替。',
        ],
        # 给当前结果字典的“证据边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '证据边界': [
            # 继续填写当前数据结构或参数列表中的一项：`'本命令已闭合Battle启动/初始化、AI候选、target随机选择、action候选抽取、戰場脫逃、Article/Ail2公式、五槽effect、FIRTTECH主处理部分公式/概率…`。
            '本命令已闭合Battle启动/初始化、AI候选、target随机选择、action候选抽取、戰場脫逃、Article/Ail2公式、五槽effect、FIRTTECH主处理部分公式/概率门以及胜利结果路径的静态draw协议；仍不代表56个RNG call-site已全部完成业务分类。',
            # 继续填写当前数据结构或参数列表中的一项：`'銀兩draw发生在角色升级loop之前，但Bank实际加钱调用0x439820发生在升级loop之后；deterministic replay按draw发生顺序而不是副作用提交顺序记录。…`。
            '銀兩draw发生在角色升级loop之前，但Bank实际加钱调用0x439820发生在升级loop之后；deterministic replay按draw发生顺序而不是副作用提交顺序记录。',
            # 继续填写当前数据结构或参数列表中的一项：`'升级helper每个角色槽只调用一次；当前静态路径一次调用最多提升一级。是否存在其他系统补做多级升级需独立证明。',`。
            '升级helper每个角色槽只调用一次；当前静态路径一次调用最多提升一级。是否存在其他系统补做多级升级需独立证明。',
            # 继续填写当前数据结构或参数列表中的一项：`'伏龍A倒计数draw在opcode0x37中先发生，随后Battle创建链0x40D4D8→0x4322C0→0x4324EC→0x442040于0x44207C重写全局RNG stat…`。
            '伏龍A倒计数draw在opcode0x37中先发生，随后Battle创建链0x40D4D8→0x4322C0→0x4324EC→0x442040于0x44207C重写全局RNG state；因此该pre-reseed draw不推进post-reseed Battle RNG序列。',
            # 继续填写当前数据结构或参数列表中的一项：`'Battle初始化0x442040中的显式随机顺序受内部mode与DataCenter+0xD8条件影响；0x424430每次成功调用固定先按LayoutGR+0x18 draw一次，再…`。
            'Battle初始化0x442040中的显式随机顺序受内部mode与DataCenter+0xD8条件影响；0x424430每次成功调用固定先按LayoutGR+0x18 draw一次，再按+0x6C>0决定是否额外draw一次。',
            # 继续填写当前数据结构或参数列表中的一项：`'当前固化只陈述机器码可证明的条件顺序与可达draw协议；对未能由静态状态机闭合的实际执行次数保持未知，不用运行时假设填补。',`。
            '当前固化只陈述机器码可证明的条件顺序与可达draw协议；对未能由静态状态机闭合的实际执行次数保持未知，不用运行时假设填补。',
        ],
    }
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json(result, args.json)
    # 检查条件 `digest != ORACLE_SHA256`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if digest != ORACLE_SHA256:
        # 检测到不能继续的情况，主动抛出异常 `SystemExit(2)`，让上层明确知道数据或参数不符合要求。
        raise SystemExit(2)


# 定义函数 `cmd_snapshot`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cmd_snapshot(args):
    # 把右侧 `{` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out = {
        # 给当前结果字典的“工具版本”字段填写 `VER,`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本': VER,
        # 给当前结果字典的“快照性质”字段填写 `'固化22阶段中导航索引；若要验证当前 EXE，请使用“验证RPG”。',`，这样导出的 JSON/表格能保留这一项证据。
        '快照性质': '固化22阶段中导航索引；若要验证当前 EXE，请使用“验证RPG”。',
        # 给当前结果字典的“固定结构证据”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '固定结构证据': {
            # 给当前结果字典的“BattleManager槽步长”字段填写 `'0xDE4',`，这样导出的 JSON/表格能保留这一项证据。
            'BattleManager槽步长': '0xDE4',
            # 给当前结果字典的“FightRole真实对象”字段填写 `'FightRole(slot) = BattleManager + 0x3E4 + slot*0xDE4',`，这样导出的 JSON/表格能保留这一项证据。
            'FightRole真实对象': 'FightRole(slot) = BattleManager + 0x3E4 + slot*0xDE4',
            # 给当前结果字典的“API内嵌块”字段填写 `'FightRole+0x84C，大小0x350；同一数值字段在已观测路径还作为 Enemy_AI record index 与 fall…`，这样导出的 JSON/表格能保留这一项证据。
            'API内嵌块': 'FightRole+0x84C，大小0x350；同一数值字段在已观测路径还作为 Enemy_AI record index 与 fallback FIRTTECH ID 使用，作者业务名未定',
            # 给当前结果字典的“AI候选区”字段填写 `'raw-slot +0xCD8/+0xCDC = FightRole+0x8F4/+0x8F8：candidate count / DW…`，这样导出的 JSON/表格能保留这一项证据。
            'AI候选区': 'raw-slot +0xCD8/+0xCDC = FightRole+0x8F4/+0x8F8：candidate count / DWORD IDs',
            # 给当前结果字典的“选中动作”字段填写 `'FightRole+0x7A8 = selected FIRTTECH ID；+0x848 = selected FIRTTECH re…`，这样导出的 JSON/表格能保留这一项证据。
            '选中动作': 'FightRole+0x7A8 = selected FIRTTECH ID；+0x848 = selected FIRTTECH record*',
            # 给当前结果字典的“目标”字段填写 `'FightRole+0x7A0 = target seed/primary index；+0x88 target list；+0xC8 …`，这样导出的 JSON/表格能保留这一项证据。
            '目标': 'FightRole+0x7A0 = target seed/primary index；+0x88 target list；+0xC8 target count',
            # 给当前结果字典的“动作模式”字段填写 `'FightRole+0xCF0；mode 7 已直接闭合为 Article/Ail2；mode 1/4 仍只保留结构名',`，这样导出的 JSON/表格能保留这一项证据。
            '动作模式': 'FightRole+0xCF0；mode 7 已直接闭合为 Article/Ail2；mode 1/4 仍只保留结构名',
            # 给当前结果字典的“Article”字段填写 `'FightRole+0xCF4 = Ail2 record*；0x42A7D0 为 Article/Ail2 processor',`，这样导出的 JSON/表格能保留这一项证据。
            'Article': 'FightRole+0xCF4 = Ail2 record*；0x42A7D0 为 Article/Ail2 processor',
            # 给当前结果字典的“FIRTTECH处理”字段填写 `'0x42A7A0 5参数包装器 → 0x42C680 FIRTTECH Action/Damage Processor',`，这样导出的 JSON/表格能保留这一项证据。
            'FIRTTECH处理': '0x42A7A0 5参数包装器 → 0x42C680 FIRTTECH Action/Damage Processor',
            # 给当前结果字典的“DamageInfo”字段填写 `'每目标 base=FightRole+0x170，stride=0x50；0x42B0D0 固定处理5槽：ID +0x10、residu…`，这样导出的 JSON/表格能保留这一项证据。
            'DamageInfo': '每目标 base=FightRole+0x170，stride=0x50；0x42B0D0 固定处理5槽：ID +0x10、residual/value +0x24；RoleDefinition+0x1E8 会复制到 +0x44，但 0x421518 实际读取的是 +0x48，固化3旧直接串联已撤销',
            # 给当前结果字典的“D6状态管线”字段填写 `'五槽 residual→目标 pending +0xCC+rawID*4；mode2 将39槽 pending 一对一提交到 runti…`，这样导出的 JSON/表格能保留这一项证据。
            'D6状态管线': '五槽 residual→目标 pending +0xCC+rawID*4；mode2 将39槽 pending 一对一提交到 runtime +0xA34；0x420440 每次调用对正 runtime 值一般减10并夹0；normal producer 对 signed raw ID<=27，因此前28槽对应 raw effect-like ID 0..27。后11槽属于generic 39槽 envelope；固化44确认其源自API.ENC序列化39-DWORD零块，当前255条API记录尾11槽全0，且normal正值writer只允许ID0..27，因此当前canonical资产+已证writer链下无正值producer；作者业务名仍未知，不能擅称辅助/保留/unused；0x421690/0x421A00/0x4231D0 提供直接清除路径',
            # 给当前结果字典的“API主表”字段填写 `'DataCenter+0xA4 由 Public\\API.ENC 初始化器直接装载，几何 255×0x350；Battle 0x441…`，这样导出的 JSON/表格能保留这一项证据。
            'API主表': 'DataCenter+0xA4 由 Public\\API.ENC 初始化器直接装载，几何 255×0x350；Battle 0x441FD0 路径把 FightRole+0x84C 的 0x350-byte RoleDefinition 整记录同步回对应 API record。',
            # 给当前结果字典的“非战斗物品writer”字段填写 `'0x43AEC0 在 DataCenter+0xA4 + role_id*0x350 上直接写持久角色字段。五槽 value>0 且 e…`，这样导出的 JSON/表格能保留这一项证据。
            '非战斗物品writer': '0x43AEC0 在 DataCenter+0xA4 + role_id*0x350 上直接写持久角色字段。五槽 value>0 且 effect ID43..60 进入 switch；ID54..60 映射到 +0x3C/+0x40/+0x48/+0x4C/+0x44/+0x2C/+0x34，实际 add=Ail2+0xD0；ID44..53 在此 switch no-op，不能外推全局无用途。',
            # 给当前结果字典的“世界物品动作”字段填写 `'Ail2+0x28>0 时 0x43AEC0 写 DataCenter+0xC4=action code、+0xEC=item ID 并…`，这样导出的 JSON/表格能保留这一项证据。
            '世界物品动作': 'Ail2+0x28>0 时 0x43AEC0 写 DataCenter+0xC4=action code、+0xEC=item ID 并提前返回；主流程 0x40CE23 按 code3..17 分派。0x170/171/172 对应 code12/13/14；13/14 分别把 1500-duration encounter threshold mode 设为放大约2× / 压缩约1/3，对应降低/提高遇敌。',

            # 给当前结果字典的“RoleDefinition双资源”字段填写 `'RoleDefinition+0x2C/+0x30 = FightRole+0x878/+0x87C 主生存资源 max/current…`，这样导出的 JSON/表格能保留这一项证据。
            'RoleDefinition双资源': 'RoleDefinition+0x2C/+0x30 = FightRole+0x878/+0x87C 主生存资源 max/current；+0x34/+0x38 = FightRole+0x880/+0x884 次资源 max/current；Battle与EVE双 consumer 交叉证明，作者资源名未知',
            # 给当前结果字典的“动作资源消耗”字段填写 `'FIRTTECH+0x34/+0x38 分别为主/次动作资源消耗量，AI前检与动作后扣除闭环；Article/Ail2+0x118/+0…`，这样导出的 JSON/表格能保留这一项证据。
            '动作资源消耗': 'FIRTTECH+0x34/+0x38 分别为主/次动作资源消耗量，AI前检与动作后扣除闭环；Article/Ail2+0x118/+0x11C分别扣主/次资源',
            # 给当前结果字典的“公式分派”字段填写 `'FIRTTECH+0x30：2/3/4/5/6 → 0x42B630/6C0/750/7D0/840，其他 → 0x42B5F0；精确整…`，这样导出的 JSON/表格能保留这一项证据。
            '公式分派': 'FIRTTECH+0x30：2/3/4/5/6 → 0x42B630/6C0/750/7D0/840，其他 → 0x42B5F0；精确整数公式已固化到阶段D数据',
            # 给当前结果字典的“RNG”字段填写 `'state@0x46E29C；0x45155B seed；0x451565: state=state*0x343FD+0x269EC3(…`，这样导出的 JSON/表格能保留这一项证据。
            'RNG': 'state@0x46E29C；0x45155B seed；0x451565: state=state*0x343FD+0x269EC3(mod 2^32), return (state>>16)&0x7FFF；Battle init 0x442076/0x44207C 用系统时间 helper 0x451583 重新播种',
            # 给当前结果字典的“D8固化21随机调用闭合”字段填写 `'同 SHA Oracle 对 0x451565 有56个直接E8 caller；扣除固化18～20已闭合协议后机器差集21点，固化21静…`，这样导出的 JSON/表格能保留这一项证据。
            'D8固化21随机调用闭合': '同 SHA Oracle 对 0x451565 有56个直接E8 caller；扣除固化18～20已闭合协议后机器差集21点，固化21静态分类为10个Battle-core/Battle直接关联点与11个非Battle-core直接点；固化22已进一步静态闭合这11点的业务所属。RNG state全局共享，当前不从模块所属外推跨状态实际执行关系。',
            # 给当前结果字典的“D7胜负与奖励”字段填写 `'0x443590 扫16个 FightRole，+0x850=0/1 对应敌方侧/我方侧计数，+0xA74!=0 的幽冥返召操偶单位不计…`，这样导出的 JSON/表格能保留这一项证据。
            'D7胜负与奖励': '0x443590 扫16个 FightRole，+0x850=0/1 对应敌方侧/我方侧计数，+0xA74!=0 的幽冥返召操偶单位不计入正常存活数；return1=敌方侧清零、return2=我方侧清零。0x443660 参数1进入完整胜利奖励：LayoutTeam+0x00/+0x04 奖励源API ID累加 RoleDefinition+0x24 EXP，+0x74/+0x78 生成銀兩，+0x7C/+0x80/+0x90 为最多4项 Ail2 掉落及阈值。固化17已闭合结果UI生命周期：普通1/2/3启动0x89FCC8外层结果UI并等待外层+0x579与+0x600明细子界面+0x579分别归零；随后只创建一次FANIN黑幕转场，+0x244从15按合格update递减，下一次合格update再0x441F60回写RoleDefinition并销毁Battle容器。detector4虽走dispatcher参数1胜利奖励，却明确跳过正常结果UI启动。',
        },
        # 给当前结果字典的“坐标换算示例”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '坐标换算示例': {
            # 给当前结果字典的“raw+0xC30”字段填写 `'FightRole+0x84C',`，这样导出的 JSON/表格能保留这一项证据。
            'raw+0xC30': 'FightRole+0x84C',
            # 给当前结果字典的“raw+0xCD8”字段填写 `'FightRole+0x8F4',`，这样导出的 JSON/表格能保留这一项证据。
            'raw+0xCD8': 'FightRole+0x8F4',
            # 给当前结果字典的“raw+0xCDC”字段填写 `'FightRole+0x8F8',`，这样导出的 JSON/表格能保留这一项证据。
            'raw+0xCDC': 'FightRole+0x8F8',
            # 给当前结果字典的“raw+0x10D4”字段填写 `'FightRole+0xCF0',`，这样导出的 JSON/表格能保留这一项证据。
            'raw+0x10D4': 'FightRole+0xCF0',
        },
        # 给当前结果字典的“当前继续入口”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '当前继续入口': [
            # 继续填写当前数据结构或参数列表中的一项：`'D5：精确公式与 RoleDefinition 双资源 runtime 结构、FIRTTECH+0x34/+0x38 动作消耗已闭合；继续证明作者资源名、selector/FIRTTEC…`。
            'D5：精确公式与 RoleDefinition 双资源 runtime 结构、FIRTTECH+0x34/+0x38 动作消耗已闭合；继续证明作者资源名、selector/FIRTTECH+0x40 业务语义与其他后处理',
            # 执行一次赋值：把 `Ail2+0xD0；ID43 为特殊路径。0x170..172 使用 Ail2+0x28=12/13/14 的世界动作 dispatch，13/14…` 的结果放入左侧 `'D6：normal residual→pending→runtime 已闭合；非战斗物品应用 0x43AE…` 对应的变量/字段。
            'D6：normal residual→pending→runtime 已闭合；非战斗物品应用 0x43AEC0 已直接闭合 API 主表 writer：effect ID54..60 写 RoleDefinition +0x3C/+0x40/+0x48/+0x4C/+0x44/+0x2C/+0x34，实际增量=Ail2+0xD0；ID43 为特殊路径。0x170..172 使用 Ail2+0x28=12/13/14 的世界动作 dispatch，13/14 已闭合为 encounter-threshold 降低/提高算法。继续追属性作者术语、code12 场景迁移更深边界；后11槽当前canonical初值/normal正值producer已由固化44关闭，只保留作者命名、异版本/非canonical新证据与特殊clear业务解释。',
            # 继续填写当前数据结构或参数列表中的一项：`'D7：主奖励、伏龍A特殊结束、结果UI双层活动门、FANIN转场与API持久回写时序已静态闭合；detector4走胜利奖励但跳过正常结果UI。D7下一侧线只剩未分类的dispatche…`。
            'D7：主奖励、伏龍A特殊结束、结果UI双层活动门、FANIN转场与API持久回写时序已静态闭合；固化43进一步确认detector4走dispatcher1胜利奖励并跳过普通结果UI，而dispatcher参数4“因事停戰”在当前canonical静态调用面无生产者。后续只保留其他版本/非canonical间接调用等新证据边界。',
            # 继续填写当前数据结构或参数列表中的一项：`'D8：固化22在固化21的56点全量枚举基础上，把11个非Battle-core直接caller静态细分为窗口创建初始化2点、随机遭遇阈值2点、Entity/NPC漫游4点、EVE RA…`。
            'D8：固化22在固化21的56点全量枚举基础上，把11个非Battle-core直接caller静态细分为窗口创建初始化2点、随机遭遇阈值2点、Entity/NPC漫游4点、EVE RANDOM_TO_VAR999 1点、EVE相机jitter 2点。当前主线继续静态拆解RPG.exe与multimedia的数据结构、消费者和状态机；不开展运行时注入/探针。',
        ],
    }
    # 把已经计算好的结构化结果交给统一 JSON 输出函数，便于保存为可复核证据。
    dump_json(out, args.json)


# 定义函数 `main`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def main():
    # 把右侧 `argparse.ArgumentParser(description='幽城战斗核心调查器（只读、证据优先）')` 计算得到的值保存到 `parser`，后面的判断或输出会继续使用它。
    parser = argparse.ArgumentParser(description='幽城战斗核心调查器（只读、证据优先）')
    # 把右侧 `parser.add_subparsers(dest='cmd', required=True)` 计算得到的值保存到 `sub`，后面的判断或输出会继续使用它。
    sub = parser.add_subparsers(dest='cmd', required=True)

    # 把右侧 `sub.add_parser('验证', help='验证 Battle 相关 ENC 表几何并输出解码 SHA256')` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('验证', help='验证 Battle 相关 ENC 表几何并输出解码 SHA256')
    # 执行一次赋值：把 `'public ENC 文件所在目录')` 的结果放入左侧 `cmd.add_argument('dir', help` 对应的变量/字段。
    cmd.add_argument('dir', help='public ENC 文件所在目录')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_verify_enc)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_verify_enc)

    # 把右侧 `sub.add_parser('记录', help='导出指定 Battle ENC record 的原始结构视图')` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('记录', help='导出指定 Battle ENC record 的原始结构视图')
    # 执行一次赋值：把 `'public ENC 文件所在目录')` 的结果放入左侧 `cmd.add_argument('dir', help` 对应的变量/字段。
    cmd.add_argument('dir', help='public ENC 文件所在目录')
    # 执行一次赋值：把 `TABLES)` 的结果放入左侧 `cmd.add_argument('table', choices` 对应的变量/字段。
    cmd.add_argument('table', choices=TABLES)
    # 执行一次赋值：把 `int)` 的结果放入左侧 `cmd.add_argument('index', type` 对应的变量/字段。
    cmd.add_argument('index', type=int)
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_record)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_record)

    # 把右侧 `sub.add_parser('验证RPG', help='对当前 Oracle RPG.exe 重放关键静态证据检查')` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('验证RPG', help='对当前 Oracle RPG.exe 重放关键静态证据检查')
    # 执行一次赋值：把 `'RPG.exe 路径')` 的结果放入左侧 `cmd.add_argument('exe', help` 对应的变量/字段。
    cmd.add_argument('exe', help='RPG.exe 路径')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_verify_rpg)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_verify_rpg)

    # 把右侧 `sub.add_parser('调用引用', help='扫描对目标 VA 的 x86 E8 rel32 直接 CALL 引用')` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('调用引用', help='扫描对目标 VA 的 x86 E8 rel32 直接 CALL 引用')
    # 执行一次赋值：把 `'RPG.exe 路径')` 的结果放入左侧 `cmd.add_argument('exe', help` 对应的变量/字段。
    cmd.add_argument('exe', help='RPG.exe 路径')
    # 执行一次赋值：把 `parse_int, help='目标 VA，例如 0x42C680')` 的结果放入左侧 `cmd.add_argument('target', type` 对应的变量/字段。
    cmd.add_argument('target', type=parse_int, help='目标 VA，例如 0x42C680')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_xrefs)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_xrefs)

    # 把右侧 `sub.add_parser('分析Ail2大ID', help='从 public 目录或 multimedia.zip 分析 Ail2+0x20…` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('分析Ail2大ID', help='从 public 目录或 multimedia.zip 分析 Ail2+0x20 与五槽 ID>27 记录')
    # 执行一次赋值：把 `'public 目录或 multimedia.zip 路径')` 的结果放入左侧 `cmd.add_argument('source', help` 对应的变量/字段。
    cmd.add_argument('source', help='public 目录或 multimedia.zip 路径')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_analyze_ail2_big_ids)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_analyze_ail2_big_ids)

    # 把右侧 `sub.add_parser('分析特殊物品', help='分析0x10F..0x111复苏、0x12D..0x133永久能力与0x170..0x…` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('分析特殊物品', help='分析0x10F..0x111复苏、0x12D..0x133永久能力与0x170..0x172世界特殊物品字段')
    # 执行一次赋值：把 `'public 目录或 multimedia.zip 路径')` 的结果放入左侧 `cmd.add_argument('source', help` 对应的变量/字段。
    cmd.add_argument('source', help='public 目录或 multimedia.zip 路径')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_analyze_special_items)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_analyze_special_items)

    # 把右侧 `sub.add_parser('分析D7奖励', help='从 public 目录或 multimedia.zip 重放 LayoutTeam/L…` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('分析D7奖励', help='从 public 目录或 multimedia.zip 重放 LayoutTeam/LevelUp/FIRTTECH 的 D7 奖励数据证据')
    # 执行一次赋值：把 `'public 目录或 multimedia.zip 路径')` 的结果放入左侧 `cmd.add_argument('source', help` 对应的变量/字段。
    cmd.add_argument('source', help='public 目录或 multimedia.zip 路径')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_analyze_d7_rewards)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_analyze_d7_rewards)

    # 把右侧 `sub.add_parser('分析D7特殊结束', help='重放伏龍A/伏龍B、LayoutGR候选字段与特殊结束数据证据')` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('分析D7特殊结束', help='重放伏龍A/伏龍B、LayoutGR候选字段与特殊结束数据证据')
    # 执行一次赋值：把 `'public 目录或 multimedia.zip 路径')` 的结果放入左侧 `cmd.add_argument('source', help` 对应的变量/字段。
    cmd.add_argument('source', help='public 目录或 multimedia.zip 路径')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_analyze_d7_special_end)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_analyze_d7_special_end)

    # 把右侧 `sub.add_parser('分析D8随机顺序', help='重放D8已闭合随机协议、固化22非Battle 11点静态语义细分与全EXE RN…` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('分析D8随机顺序', help='重放D8已闭合随机协议、固化22非Battle 11点静态语义细分与全EXE RNG直接调用清单')
    # 执行一次赋值：把 `'RPG.exe 路径')` 的结果放入左侧 `cmd.add_argument('exe', help` 对应的变量/字段。
    cmd.add_argument('exe', help='RPG.exe 路径')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_analyze_d8_rng)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_analyze_d8_rng)

    # 把右侧 `sub.add_parser('快照', help='输出固化22阶段中当前结构导航索引（不是独立证据生成器）')` 计算得到的值保存到 `cmd`，后面的判断或输出会继续使用它。
    cmd = sub.add_parser('快照', help='输出固化22阶段中当前结构导航索引（不是独立证据生成器）')
    # 执行一次赋值：把 `'输出 JSON 路径')` 的结果放入左侧 `cmd.add_argument('--json', help` 对应的变量/字段。
    cmd.add_argument('--json', help='输出 JSON 路径')
    # 执行一次赋值：把 `cmd_snapshot)` 的结果放入左侧 `cmd.set_defaults(func` 对应的变量/字段。
    cmd.set_defaults(func=cmd_snapshot)

    # 把右侧 `parser.parse_args()` 计算得到的值保存到 `args`，后面的判断或输出会继续使用它。
    args = parser.parse_args()
    # 调用 `args.func` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    args.func(args)


# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__ == '__main__':
    # 调用 `main` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    main()
