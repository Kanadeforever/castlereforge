#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》SCI / SF2 调查器
版本：0.7D.15-固化34

性质：只读逆向验证工具。
依赖：仅 Python 3 标准库。

主要用途：
1. 识别并解析 0x473(1139) 字节 Map SCI 与 0x227(551) 字节 Entity/NPC SCI。
2. 输出当前由 RPG.exe Loader/调用链确认的字段，未知区域保持 raw，不强行命名。
3. 解析普通/0x6F 压缩 SF2；tile 尺寸严格读取 Header +0x07/+0x09，不写死 64×48。
4. 验证 Section0/1/2/3 结构、Section0 动作记录、渲染模式分派。
5. 在多个资源根中大小写不敏感解析 SCI→SF2 路径，并验证四组动作 start/length 是否落在 Section0 范围内。
6. 对多个 SF2 根按 SHA-256 去重后做全量回归。
7. 固化22：验证 Entity behavior type 0/1/2/3、type3漫游边界、+0x77..+0x7A两组移动步长档，以及 Map +0x65背景相机位移变换。
8. 固化23：识别 DDDES.SCI 为 18-byte header + N×52-byte record 的显示/定时配置，并验证 Entity +EA/+EB 双事件绑定静态协议。
9. 固化23修正版2：基于 Music.DAT 原始目录项与 RPG.exe 0x00403220 Reader，严格识别 MP3LIST.SCI/WAVLIST.SCI 为 51-byte 音乐列表记录；仅确认 record+0x01 是以 NUL 结尾、最多50字节的资源基名字符串，record+0x00 暂保持未知标志。
10. 固化25：审计 Map +0x269..+0x2CC、+0x332..+0x372、+0x388..+0x472 三段未知区的样本值、主构造器直接访问和全 .text 字面位移候选。
11. 固化27：验证 ITF000.SF2 的 SAF\x05 异魔数、0x6F/zlib 包装及标准 SF2 构造器拒绝边界。
12. 固化28：穷举通用动画加载器、Fight\\ITF静态路径和zlib直接调用，约束裸名ITF000.SF2的当前EXE可达边界。
13. 固化29：闭合编号ITF的标准SF2对象包装链，以及两组11×30字节稀疏路径表的索引路由。
14. 固化30：闭合ITF双表下游Effect Manager到严格SF2构造器的完整装载链。
15. 固化31：按SHA-256去重统计Section0十二个属性槽的磁盘取值域，并核对0x43F520效果消费链。
16. 固化32：闭合 slot8/slot9 的目标锚点与自身锚点注册/更新路由。
17. 固化33：穷举 Section0 Getter 的直接 E8 调用点，闭合 slot1/slot3 与 slot10/slot11 的已知直接调用边界。
18. 固化34：重建 RGB555/RGB565 运行时查表公式，输出 Mode 1/2/3/4/7/8/30 的闭式参考语义，并统计全语料渲染参数。
19. 固化34：允许把“整文件哈希不同但全部已覆盖机器断言一致”的原始 EXE 作为兼容候选验证；这不等价于宣布两个 EXE 全文件相同。

本工具不修改原游戏资源，也不内含商业资源。
"""
# 启用较新的 Python 注解语法行为，让类型注解先按文字保存，避免旧版本在定义阶段立即求值。
from __future__ import annotations

# 导入 `argparse` 模块；这里只取得标准库功能，不会修改游戏文件。
import argparse
# 导入 `csv` 模块；这里只取得标准库功能，不会修改游戏文件。
import csv
# 导入 `hashlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import hashlib
# 导入 `json` 模块；这里只取得标准库功能，不会修改游戏文件。
import json
# 导入 `struct` 模块；这里只取得标准库功能，不会修改游戏文件。
import struct
# 导入 `zlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import zlib
# 从 `collections` 模块导入 `Counter, defaultdict`，后面的代码会直接使用这些现成组件。
from collections import Counter, defaultdict
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path
# 从 `typing` 模块导入 `Any, Dict, Iterable, List, Optional, Tuple`，后面的代码会直接使用这些现成组件。
from typing import Any, Dict, Iterable, List, Optional, Tuple

# 把右侧 `"0.7D.15-固化34"` 计算得到的值保存到 `TOOL_VERSION`，后面的判断或输出会继续使用它。
TOOL_VERSION = "0.7D.15-固化34"
# 固化34收到的 RPG.exe.org 与历史 Oracle 整文件哈希不同。这个常量只用于报告“已覆盖代码区域兼容”，绝不能把它当成新的唯一 Oracle。
# 把右侧 `"8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"` 计算得到的值保存到 `SOLID34_COMPATIBLE_RPG_ORG_SHA256`，后面的判断或输出会继续使用它。
SOLID34_COMPATIBLE_RPG_ORG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
# 把右侧 `0x473` 计算得到的值保存到 `MAP_RECORD_SIZE`，后面的判断或输出会继续使用它。
MAP_RECORD_SIZE = 0x473
# 把右侧 `0x227` 计算得到的值保存到 `ENTITY_RECORD_SIZE`，后面的判断或输出会继续使用它。
ENTITY_RECORD_SIZE = 0x227
# 把右侧 `0x42BC` 计算得到的值保存到 `SF2_HEADER_SIZE`，后面的判断或输出会继续使用它。
SF2_HEADER_SIZE = 0x42BC
# 把右侧 `b"SF2\x05"` 计算得到的值保存到 `SF2_MAGIC`，后面的判断或输出会继续使用它。
SF2_MAGIC = b"SF2\x05"

# 原版 0x429480 的 0..30 分派结果。None 表示落入原版 fallback/no-op。
# 创建变量 `SF2_RENDER_DISPATCH`（类型提示为 `Dict[int, Optional[Tuple[int, str]]]`），并把 `{` 的结果保存进去供后续步骤使用。
SF2_RENDER_DISPATCH: Dict[int, Optional[Tuple[int, str]]] = {
    # 继续填写当前数据结构或参数列表中的一项：`0: (0x004296CD, "NORMAL_COPY"),`。
    0: (0x004296CD, "NORMAL_COPY"),
    # 继续填写当前数据结构或参数列表中的一项：`1: (0x004296EB, "ALPHA_BLEND_16"),`。
    1: (0x004296EB, "ALPHA_BLEND_16"),
    # 继续填写当前数据结构或参数列表中的一项：`2: (0x0042974D, "WEIGHTED_BLEND_16"),`。
    2: (0x0042974D, "WEIGHTED_BLEND_16"),
    # 继续填写当前数据结构或参数列表中的一项：`3: (0x004297B0, "MASKED_DESTINATION_TINT_BLEND"),`。
    3: (0x004297B0, "MASKED_DESTINATION_TINT_BLEND"),
    # 继续填写当前数据结构或参数列表中的一项：`4: (0x004297FC, "WEIGHTED_SOURCE_PLUS_CONSTANT_COLOR"),`。
    4: (0x004297FC, "WEIGHTED_SOURCE_PLUS_CONSTANT_COLOR"),
    # 继续填写当前数据结构或参数列表中的一项：`5: (0x00429848, "INVERT_SOURCE_COPY"),`。
    5: (0x00429848, "INVERT_SOURCE_COPY"),
    # 继续填写当前数据结构或参数列表中的一项：`6: (0x0042988F, "INVERT_DESTINATION_MASK"),`。
    6: (0x0042988F, "INVERT_DESTINATION_MASK"),
    # 继续填写当前数据结构或参数列表中的一项：`7: (0x004298D6, "LEGACY_LOOKUP_COMPOSITE_7"),`。
    7: (0x004298D6, "LEGACY_LOOKUP_COMPOSITE_7"),
    # 继续填写当前数据结构或参数列表中的一项：`8: (0x0042991D, "LEGACY_BRIGHTEN_BY_SOURCE_MAX"),`。
    8: (0x0042991D, "LEGACY_BRIGHTEN_BY_SOURCE_MAX"),
    # 继续填写当前数据结构或参数列表中的一项：`9: None,`。
    9: None,
    # 继续填写当前数据结构或参数列表中的一项：`10: (0x004299C7, "MASKED_DESTINATION_NEIGHBOR_AVERAGE_A"),`。
    10: (0x004299C7, "MASKED_DESTINATION_NEIGHBOR_AVERAGE_A"),
    # 继续填写当前数据结构或参数列表中的一项：`11: (0x00429A08, "MASKED_DESTINATION_NEIGHBOR_AVERAGE_B"),`。
    11: (0x00429A08, "MASKED_DESTINATION_NEIGHBOR_AVERAGE_B"),
    # 继续填写当前数据结构或参数列表中的一项：`30: (0x00429964, "SOURCE_ADD_CONSTANT_BITMASK"),`。
    30: (0x00429964, "SOURCE_ADD_CONSTANT_BITMASK"),
}
# 开始循环 `_m in range(12, 30)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
for _m in range(12, 30):
    # 把右侧 `None` 计算得到的值保存到 `SF2_RENDER_DISPATCH[_m]`，后面的判断或输出会继续使用它。
    SF2_RENDER_DISPATCH[_m] = None

# 把右侧 `[` 计算得到的值保存到 `SF2_PROPERTY_CALLSITES`，后面的判断或输出会继续使用它。
SF2_PROPERTY_CALLSITES = [
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x00420455","槽":9,"类型":"常量槽","证据":"直接 push 9 后调用 0x0042A1A0"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0042046D","槽":8,"类型":"常量槽","证据":"直接 push 8 后调用 0x0042A1A0"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x00422524","槽":"1或3","类型":"包装器内部动态、全部直接caller已闭合","证据":"0x004236F9链固定slot1；0x00444A20与0x00445280固定slot3"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0042D473","槽":1,"类型":"常量槽","证据":"直接调用链"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0042D96C","槽":3,"类型":"常量槽","证据":"0x0042D95E 直接 push 3，随后 0x0042D96C 调用 0x0042A1A0"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0042D9AF","槽":1,"类型":"包装器内部动态、唯一直接caller固定槽","证据":"0x00445740 push 1，0x00445743唯一E8直接调用0x0042D980"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0043F53A","槽":0,"类型":"常量槽","证据":"0x0043F520 子系统成组读取 slots 0/4/5/6/7/2/8；属于 effect/spatial-layout metadata family"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0043F558","槽":4,"类型":"常量槽","证据":"同一大子系统成组读取"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0043F567","槽":5,"类型":"常量槽","证据":"同一大子系统成组读取"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0043F584","槽":6,"类型":"常量槽","证据":"同一大子系统成组读取"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0043F595","槽":7,"类型":"常量槽","证据":"同一大子系统成组读取"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0043F5A4","槽":2,"类型":"常量槽","证据":"同一大子系统成组读取"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x0043F5B5","槽":8,"类型":"常量槽","证据":"同一大子系统成组读取"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x00444E10","槽":1,"类型":"常量槽","证据":"与 12/13 常量比较链"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x00444E2F","槽":1,"类型":"常量槽","证据":"与 12/13 常量比较链"},
    # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
    {"调用点":"0x00444E64","槽":1,"类型":"常量槽","证据":"与 12/13 常量比较链"},
]


# 定义函数 `u16`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def u16(b: bytes, o: int) -> int: return struct.unpack_from("<H", b, o)[0]
# 定义函数 `i16`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def i16(b: bytes, o: int) -> int: return struct.unpack_from("<h", b, o)[0]
# 定义函数 `u32`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def u32(b: bytes, o: int) -> int: return struct.unpack_from("<I", b, o)[0]
# 定义函数 `i32`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def i32(b: bytes, o: int) -> int: return struct.unpack_from("<i", b, o)[0]


# 定义函数 `cstr`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cstr(b: bytes, o: int, n: int, enc: str = "cp950") -> str:
    # 把右侧 `b[o:o+n].split(b"\0", 1)[0]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
    raw = b[o:o+n].split(b"\0", 1)[0]
    # 把 `raw.decode(enc, errors="replace")` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return raw.decode(enc, errors="replace")


# 定义函数 `hexblock`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def hexblock(b: bytes, o: int, n: int) -> str:
    # 把 `b[o:o+n].hex(" ")` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return b[o:o+n].hex(" ")


# 定义函数 `write_json`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def write_json(path: Path, obj: Any) -> None:
    # 执行一次赋值：把 `True, exist_ok=True)` 的结果放入左侧 `path.parent.mkdir(parents` 对应的变量/字段。
    path.parent.mkdir(parents=True, exist_ok=True)
    # 执行一次赋值：把 `False, indent=2) + "\n", encoding="utf-8")` 的结果放入左侧 `path.write_text(json.dumps(obj, ensure_ascii` 对应的变量/字段。
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


# 定义函数 `write_csv`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def write_csv(path: Path, rows: List[Dict[str, Any]], fieldnames: List[str]) -> None:
    # 执行一次赋值：把 `True, exist_ok=True)` 的结果放入左侧 `path.parent.mkdir(parents` 对应的变量/字段。
    path.parent.mkdir(parents=True, exist_ok=True)
    # 进入受管理的资源作用域 `path.open("w", encoding="utf-8-sig", newline="") as f`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
    with path.open("w", encoding="utf-8-sig", newline="") as f:
        # 把右侧 `csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")` 计算得到的值保存到 `w`，后面的判断或输出会继续使用它。
        w = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")
        # 调用 `w.writeheader` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        w.writeheader()
        # 开始循环 `row in rows`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for row in rows:
            # 把右侧 `dict(row)` 计算得到的值保存到 `flat`，后面的判断或输出会继续使用它。
            flat = dict(row)
            # 开始循环 `k, v in list(flat.items())`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for k, v in list(flat.items()):
                # 检查条件 `isinstance(v, (list, dict, tuple))`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if isinstance(v, (list, dict, tuple)):
                    # 把右侧 `json.dumps(v, ensure_ascii=False, separators=(",", ":"))` 计算得到的值保存到 `flat[k]`，后面的判断或输出会继续使用它。
                    flat[k] = json.dumps(v, ensure_ascii=False, separators=(",", ":"))
            # 调用 `w.writerow` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            w.writerow(flat)


# 定义函数 `classify_sci`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def classify_sci(path: Path) -> Dict[str, Any]:
    # 把右侧 `path.stat().st_size` 计算得到的值保存到 `size`，后面的判断或输出会继续使用它。
    size = path.stat().st_size
    # 检查条件 `size and size % MAP_RECORD_SIZE == 0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if size and size % MAP_RECORD_SIZE == 0:
        # 把 `{"类型":"地图SCI","记录大小":MAP_RECORD_SIZE,"记录数":size // MAP_RECORD_SIZE,"证据":"RPG.exe 0x0040AC…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {"类型":"地图SCI","记录大小":MAP_RECORD_SIZE,"记录数":size // MAP_RECORD_SIZE,"证据":"RPG.exe 0x0040AC00 明确按 0x473 步长"}
    # 检查条件 `size and size % ENTITY_RECORD_SIZE == 0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if size and size % ENTITY_RECORD_SIZE == 0:
        # 把 `{"类型":"实体SCI","记录大小":ENTITY_RECORD_SIZE,"记录数":size // ENTITY_RECORD_SIZE,"证据":"RPG.exe 0x…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {"类型":"实体SCI","记录大小":ENTITY_RECORD_SIZE,"记录数":size // ENTITY_RECORD_SIZE,"证据":"RPG.exe 0x0040A160 明确按 0x227 记录"}
    # 检查条件 `path.name.lower() == "dddes.sci" and size >= 0x12 and (size-0x12) % 0x34 == 0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if path.name.lower() == "dddes.sci" and size >= 0x12 and (size-0x12) % 0x34 == 0:
        # 把 `{"类型":"DDDES显示定时配置","头大小":0x12,"记录大小":0x34,"记录数": (size-0x12)//0x34,"证据":"RPG.exe 0x00401…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {"类型":"DDDES显示定时配置","头大小":0x12,"记录大小":0x34,"记录数": (size-0x12)//0x34,"证据":"RPG.exe 0x004014E0：buffer+0x12 为记录区，count=(file_size-0x12)/0x34；文件名必须为DDDES.SCI，禁止把任意70-byte SCI套入该格式"}
    # 检查条件 `path.name.lower() in {"mp3list.sci", "wavlist.sci"} and size and size % 0x33 == 0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if path.name.lower() in {"mp3list.sci", "wavlist.sci"} and size and size % 0x33 == 0:
        # 把 `{"类型":"音乐列表SCI","记录大小":0x33,"记录数":size//0x33,"列表文件":path.name.upper(),"证据":"RPG.exe 0x004…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {"类型":"音乐列表SCI","记录大小":0x33,"记录数":size//0x33,"列表文件":path.name.upper(),"证据":"RPG.exe 0x00403220：MP3List.SCI写object+0x08/count+0x04，WAVList.SCI写object+0x10/count+0x0C，两者均以file_size/51计数；0x004032FA路径从object+0x08按index*51+1形成%s.MP3，0x004033A1路径从object+0x10形成%s.WAV"}
    # 把 `{"类型":"未知SCI","记录大小":None,"记录数":None,"证据":"当前无已确认 Reader 匹配"}` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"类型":"未知SCI","记录大小":None,"记录数":None,"证据":"当前无已确认 Reader 匹配"}


# 定义函数 `parse_map_record`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_map_record(r: bytes, index: int) -> Dict[str, Any]:
    # 检查条件 `len(r) != MAP_RECORD_SIZE`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(r) != MAP_RECORD_SIZE:
        # 检测到不能继续的情况，主动抛出异常 `ValueError("Map SCI record 必须为 0x473 字节")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("Map SCI record 必须为 0x473 字节")
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“索引”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
        "索引": index,
        # 给当前结果字典的“固定记录大小”字段填写 `MAP_RECORD_SIZE,`，这样导出的 JSON/表格能保留这一项证据。
        "固定记录大小": MAP_RECORD_SIZE,
        # 给当前结果字典的“背景相机模式_0x65”字段填写 `r[0x65],`，这样导出的 JSON/表格能保留这一项证据。
        "背景相机模式_0x65": r[0x65],
        # 给当前结果字典的“背景相机模式_0x65_静态公式”字段填写 `{0:"<=0：不进入相机相对基值重算",1:"相机相对位移按signed trunc(camera_delta/5)",2:"相机相对位…`，这样导出的 JSON/表格能保留这一项证据。
        "背景相机模式_0x65_静态公式": {0:"<=0：不进入相机相对基值重算",1:"相机相对位移按signed trunc(camera_delta/5)",2:"相机相对位移按signed trunc(7*camera_delta/5)",10:"跳过相机相对基值重算；render走专门分支"}.get(r[0x65], "正值且非1/2/10：相机相对位移1:1写入基值；作者业务枚举名未知"),
        # 给当前结果字典的“场景原点X_0x66”字段填写 `i16(r,0x66),`，这样导出的 JSON/表格能保留这一项证据。
        "场景原点X_0x66": i16(r,0x66),
        # 给当前结果字典的“场景原点Y_0x68”字段填写 `i16(r,0x68),`，这样导出的 JSON/表格能保留这一项证据。
        "场景原点Y_0x68": i16(r,0x68),
        # 给当前结果字典的“地图宽_0x6A”字段填写 `u16(r,0x6A),`，这样导出的 JSON/表格能保留这一项证据。
        "地图宽_0x6A": u16(r,0x6A),
        # 给当前结果字典的“地图高_0x6C”字段填写 `u16(r,0x6C),`，这样导出的 JSON/表格能保留这一项证据。
        "地图高_0x6C": u16(r,0x6C),
        # 给当前结果字典的“渲染选项_0x6E”字段填写 `struct.unpack_from("<b",r,0x6E)[0],`，这样导出的 JSON/表格能保留这一项证据。
        "渲染选项_0x6E": struct.unpack_from("<b",r,0x6E)[0],
        # 给当前结果字典的“背景滚动启用_0x6F”字段填写 `r[0x6F],`，这样导出的 JSON/表格能保留这一项证据。
        "背景滚动启用_0x6F": r[0x6F],
        # 给当前结果字典的“背景滚动步长X_0x70”字段填写 `i16(r,0x70),`，这样导出的 JSON/表格能保留这一项证据。
        "背景滚动步长X_0x70": i16(r,0x70),
        # 给当前结果字典的“背景滚动步长Y_0x72”字段填写 `i16(r,0x72),`，这样导出的 JSON/表格能保留这一项证据。
        "背景滚动步长Y_0x72": i16(r,0x72),
        # 给当前结果字典的“主背景路径_0x74”字段填写 `cstr(r,0x74,100),`，这样导出的 JSON/表格能保留这一项证据。
        "主背景路径_0x74": cstr(r,0x74,100),
        # 给当前结果字典的“主背景类型_0xD8”字段填写 `r[0xD8],`，这样导出的 JSON/表格能保留这一项证据。
        "主背景类型_0xD8": r[0xD8],
        # 给当前结果字典的“覆盖SF2路径A_0xD9”字段填写 `cstr(r,0xD9,100),`，这样导出的 JSON/表格能保留这一项证据。
        "覆盖SF2路径A_0xD9": cstr(r,0xD9,100),
        # 给当前结果字典的“覆盖SF2路径B_0x13D”字段填写 `cstr(r,0x13D,100),`，这样导出的 JSON/表格能保留这一项证据。
        "覆盖SF2路径B_0x13D": cstr(r,0x13D,100),
        # 给当前结果字典的“区域随机遭遇标志_0x1A1”字段填写 `r[0x1A1],`，这样导出的 JSON/表格能保留这一项证据。
        "区域随机遭遇标志_0x1A1": r[0x1A1],
        # 给当前结果字典的“区域遭遇块_0x1A1_0x204”字段填写 `hexblock(r,0x1A1,100),`，这样导出的 JSON/表格能保留这一项证据。
        "区域遭遇块_0x1A1_0x204": hexblock(r,0x1A1,100),
        # 给当前结果字典的“地图局部实体SCI路径_0x205”字段填写 `cstr(r,0x205,100),`，这样导出的 JSON/表格能保留这一项证据。
        "地图局部实体SCI路径_0x205": cstr(r,0x205,100),
        # 给当前结果字典的“未知块_0x269_0x2CC”字段填写 `hexblock(r,0x269,100),`，这样导出的 JSON/表格能保留这一项证据。
        "未知块_0x269_0x2CC": hexblock(r,0x269,100),
        # 给当前结果字典的“EVE路径_0x2CD”字段填写 `cstr(r,0x2CD,100),`，这样导出的 JSON/表格能保留这一项证据。
        "EVE路径_0x2CD": cstr(r,0x2CD,100),
        # 给当前结果字典的“BGM选择器_0x331”字段填写 `r[0x331],`，这样导出的 JSON/表格能保留这一项证据。
        "BGM选择器_0x331": r[0x331],
        # 给当前结果字典的“未知块_0x332_0x372”字段填写 `hexblock(r,0x332,0x41),`，这样导出的 JSON/表格能保留这一项证据。
        "未知块_0x332_0x372": hexblock(r,0x332,0x41),
        # 给当前结果字典的“WAV选择器_0x373_0x377”字段填写 `list(r[0x373:0x378]),`，这样导出的 JSON/表格能保留这一项证据。
        "WAV选择器_0x373_0x377": list(r[0x373:0x378]),
        # 给当前结果字典的“随机遭遇启用原值_0x378”字段填写 `u32(r,0x378),`，这样导出的 JSON/表格能保留这一项证据。
        "随机遭遇启用原值_0x378": u32(r,0x378),
        # 给当前结果字典的“随机遭遇启用”字段填写 `u32(r,0x378) != 0,`，这样导出的 JSON/表格能保留这一项证据。
        "随机遭遇启用": u32(r,0x378) != 0,
        # 给当前结果字典的“初始遭遇选择变量ID_0x37C”字段填写 `u32(r,0x37C),`，这样导出的 JSON/表格能保留这一项证据。
        "初始遭遇选择变量ID_0x37C": u32(r,0x37C),
        # 给当前结果字典的“场景存档入口许可原值_0x380”字段填写 `u32(r,0x380),`，这样导出的 JSON/表格能保留这一项证据。
        "场景存档入口许可原值_0x380": u32(r,0x380),
        # 给当前结果字典的“场景存档入口许可_0x380等于1”字段填写 `u32(r,0x380) == 1,`，这样导出的 JSON/表格能保留这一项证据。
        "场景存档入口许可_0x380等于1": u32(r,0x380) == 1,
        # 给当前结果字典的“code12无continuation槽2门控原值_0x384”字段填写 `u32(r,0x384),`，这样导出的 JSON/表格能保留这一项证据。
        "code12无continuation槽2门控原值_0x384": u32(r,0x384),
        # 给当前结果字典的“code12无continuation槽2门控启用”字段填写 `u32(r,0x384) == 1,`，这样导出的 JSON/表格能保留这一项证据。
        "code12无continuation槽2门控启用": u32(r,0x384) == 1,
        # 给当前结果字典的“未知尾_0x388_0x472”字段填写 `hexblock(r,0x388,MAP_RECORD_SIZE-0x388),`，这样导出的 JSON/表格能保留这一项证据。
        "未知尾_0x388_0x472": hexblock(r,0x388,MAP_RECORD_SIZE-0x388),
        # 给当前结果字典的“Reader证据”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        "Reader证据": {
            # 给当前结果字典的“记录Loader”字段填写 `"0x0040AC00",`，这样导出的 JSON/表格能保留这一项证据。
            "记录Loader":"0x0040AC00",
            # 给当前结果字典的“字段消费者”字段填写 `"0x00408E80",`，这样导出的 JSON/表格能保留这一项证据。
            "字段消费者":"0x00408E80",
            # 给当前结果字典的“随机遭遇”字段填写 `"0x00403510 / 0x00403590 / 0x004035A0",`，这样导出的 JSON/表格能保留这一项证据。
            "随机遭遇":"0x00403510 / 0x00403590 / 0x004035A0",
            # 给当前结果字典的“加380访问器与consumer”字段填写 `"0x0040A0C0 -> 0x0040CCC2；首条Map SCI记录；DataCenter+0x108 = 1 - value；In…`，这样导出的 JSON/表格能保留这一项证据。
            "加380访问器与consumer":"0x0040A0C0 -> 0x0040CCC2；首条Map SCI记录；DataCenter+0x108 = 1 - value；Interface仅+0x108==0时允许state7，state7绑定Save子系统",
            # 给当前结果字典的“加384访问器与code12门控”字段填写 `"0x0040A0D0 -> 0x0040CEE9；selected Map SCI记录；仅value==1时允许fallback激活当前…`，这样导出的 JSON/表格能保留这一项证据。
            "加384访问器与code12门控":"0x0040A0D0 -> 0x0040CEE9；selected Map SCI记录；仅value==1时允许fallback激活当前EVE slot2",
        },
    }


# 定义函数 `parse_entity_record`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_entity_record(r: bytes, index: int) -> Dict[str, Any]:
    # 检查条件 `len(r) != ENTITY_RECORD_SIZE`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(r) != ENTITY_RECORD_SIZE:
        # 检测到不能继续的情况，主动抛出异常 `ValueError("Entity SCI record 必须为 0x227 字节")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("Entity SCI record 必须为 0x227 字节")
    # 把右侧 `[]` 计算得到的值保存到 `pairs`，后面的判断或输出会继续使用它。
    pairs=[]
    # 开始循环 `j,o in enumerate((0xE2,0xE4,0xE6,0xE8))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for j,o in enumerate((0xE2,0xE4,0xE6,0xE8)):
        # 把这一条新结果追加到列表 `pairs`；不会覆盖前面已经收集的记录。
        pairs.append({"组":j,"start":r[o],"length":r[o+1],"偏移":f"+0x{o:02X}/+0x{o+1:02X}"})
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“索引”字段填写 `index,`，这样导出的 JSON/表格能保留这一项证据。
        "索引": index,
        # 给当前结果字典的“名称_0x00”字段填写 `cstr(r,0,0x64),`，这样导出的 JSON/表格能保留这一项证据。
        "名称_0x00": cstr(r,0,0x64),
        # 给当前结果字典的“世界X_0x64”字段填写 `i32(r,0x64),`，这样导出的 JSON/表格能保留这一项证据。
        "世界X_0x64": i32(r,0x64),
        # 给当前结果字典的“世界Y_0x68”字段填写 `i32(r,0x68),`，这样导出的 JSON/表格能保留这一项证据。
        "世界Y_0x68": i32(r,0x68),
        # 给当前结果字典的“SF2锚点X_0x6C”字段填写 `i16(r,0x6C),`，这样导出的 JSON/表格能保留这一项证据。
        "SF2锚点X_0x6C": i16(r,0x6C),
        # 给当前结果字典的“SF2锚点Y_0x6E”字段填写 `i16(r,0x6E),`，这样导出的 JSON/表格能保留这一项证据。
        "SF2锚点Y_0x6E": i16(r,0x6E),
        # 给当前结果字典的“在场启用_0x70”字段填写 `r[0x70],`，这样导出的 JSON/表格能保留这一项证据。
        "在场启用_0x70": r[0x70],
        # 给当前结果字典的“未知_0x71”字段填写 `r[0x71],`，这样导出的 JSON/表格能保留这一项证据。
        "未知_0x71": r[0x71],
        # 给当前结果字典的“未知_0x72”字段填写 `r[0x72],`，这样导出的 JSON/表格能保留这一项证据。
        "未知_0x72": r[0x72],
        # 给当前结果字典的“交互模式_0x73”字段填写 `r[0x73],`，这样导出的 JSON/表格能保留这一项证据。
        "交互模式_0x73": r[0x73],
        # 给当前结果字典的“碰撞接近处理标志_0x74”字段填写 `r[0x74],`，这样导出的 JSON/表格能保留这一项证据。
        "碰撞接近处理标志_0x74": r[0x74],
        # 给当前结果字典的“渲染更新抑制_0x75”字段填写 `r[0x75],`，这样导出的 JSON/表格能保留这一项证据。
        "渲染更新抑制_0x75": r[0x75],
        # 给当前结果字典的“行为类型_0x76”字段填写 `r[0x76],`，这样导出的 JSON/表格能保留这一项证据。
        "行为类型_0x76": r[0x76],
        # 给当前结果字典的“行为类型_0x76_静态语义”字段填写 `{0:"被动/静态类：不进入type1/3移动管理分支；动作循环",1:"外部驱动可移动类：进入移动/碰撞管理；无type3随机目的地生成…`，这样导出的 JSON/表格能保留这一项证据。
        "行为类型_0x76_静态语义": {0:"被动/静态类：不进入type1/3移动管理分支；动作循环",1:"外部驱动可移动类：进入移动/碰撞管理；无type3随机目的地生成；动作循环",2:"一次性动作播放覆盖模式：SF2 clip到末帧停住；EVE 0x1A/0x8F可临时写入",3:"自主随机漫游类：随机目的地/随机等待并进入移动/碰撞管理；动作循环"}.get(r[0x76], "当前EXE未静态闭合的枚举值"),
        # 给当前结果字典的“移动档0_直线修正_0x77”字段填写 `struct.unpack_from("<b",r,0x77)[0],`，这样导出的 JSON/表格能保留这一项证据。
        "移动档0_直线修正_0x77": struct.unpack_from("<b",r,0x77)[0],
        # 给当前结果字典的“移动档0_直线实际步长”字段填写 `struct.unpack_from("<b",r,0x77)[0] + 6,`，这样导出的 JSON/表格能保留这一项证据。
        "移动档0_直线实际步长": struct.unpack_from("<b",r,0x77)[0] + 6,
        # 给当前结果字典的“移动档0_对角修正_0x78”字段填写 `struct.unpack_from("<b",r,0x78)[0],`，这样导出的 JSON/表格能保留这一项证据。
        "移动档0_对角修正_0x78": struct.unpack_from("<b",r,0x78)[0],
        # 给当前结果字典的“移动档0_对角实际步长”字段填写 `struct.unpack_from("<b",r,0x78)[0] + 4,`，这样导出的 JSON/表格能保留这一项证据。
        "移动档0_对角实际步长": struct.unpack_from("<b",r,0x78)[0] + 4,
        # 给当前结果字典的“移动档1_直线修正_0x79”字段填写 `struct.unpack_from("<b",r,0x79)[0],`，这样导出的 JSON/表格能保留这一项证据。
        "移动档1_直线修正_0x79": struct.unpack_from("<b",r,0x79)[0],
        # 给当前结果字典的“移动档1_直线实际步长”字段填写 `struct.unpack_from("<b",r,0x79)[0] + 9,`，这样导出的 JSON/表格能保留这一项证据。
        "移动档1_直线实际步长": struct.unpack_from("<b",r,0x79)[0] + 9,
        # 给当前结果字典的“移动档1_对角修正_0x7A”字段填写 `struct.unpack_from("<b",r,0x7A)[0],`，这样导出的 JSON/表格能保留这一项证据。
        "移动档1_对角修正_0x7A": struct.unpack_from("<b",r,0x7A)[0],
        # 给当前结果字典的“移动档1_对角实际步长”字段填写 `struct.unpack_from("<b",r,0x7A)[0] + 6,`，这样导出的 JSON/表格能保留这一项证据。
        "移动档1_对角实际步长": struct.unpack_from("<b",r,0x7A)[0] + 6,
        # 给当前结果字典的“移动档选择静态语义”字段填写 `"运行态packed state的bits8..15是否非零选择profile0/profile1；尚无证据把两档命名为走/跑等作者业务名…`，这样导出的 JSON/表格能保留这一项证据。
        "移动档选择静态语义": "运行态packed state的bits8..15是否非零选择profile0/profile1；尚无证据把两档命名为走/跑等作者业务名",
        # 给当前结果字典的“SF2路径_0x7B”字段填写 `cstr(r,0x7B,100),`，这样导出的 JSON/表格能保留这一项证据。
        "SF2路径_0x7B": cstr(r,0x7B,100),
        # 给当前结果字典的“动画变体缩放_0xDF”字段填写 `r[0xDF],`，这样导出的 JSON/表格能保留这一项证据。
        "动画变体缩放_0xDF": r[0xDF],
        # 给当前结果字典的“未知_0xE0”字段填写 `r[0xE0],`，这样导出的 JSON/表格能保留这一项证据。
        "未知_0xE0": r[0xE0],
        # 给当前结果字典的“基础Entry0预绘启用_0xE1”字段填写 `r[0xE1],`，这样导出的 JSON/表格能保留这一项证据。
        "基础Entry0预绘启用_0xE1": r[0xE1],
        # 给当前结果字典的“动作组”字段填写 `pairs,`，这样导出的 JSON/表格能保留这一项证据。
        "动作组": pairs,
        # 给当前结果字典的“接触触发事件ID_0xEA”字段填写 `r[0xEA],`，这样导出的 JSON/表格能保留这一项证据。
        "接触触发事件ID_0xEA": r[0xEA],
        # 给当前结果字典的“主动交互事件ID_0xEB”字段填写 `r[0xEB],`，这样导出的 JSON/表格能保留这一项证据。
        "主动交互事件ID_0xEB": r[0xEB],
        # 给当前结果字典的“未知_0xEC”字段填写 `r[0xEC],`，这样导出的 JSON/表格能保留这一项证据。
        "未知_0xEC": r[0xEC],
        # 给当前结果字典的“未知_0xED”字段填写 `r[0xED],`，这样导出的 JSON/表格能保留这一项证据。
        "未知_0xED": r[0xED],
        # 给当前结果字典的“特殊空间覆盖上下文启用_0xEE”字段填写 `r[0xEE],`，这样导出的 JSON/表格能保留这一项证据。
        "特殊空间覆盖上下文启用_0xEE": r[0xEE],
        # 给当前结果字典的“漫游范围X原值_0xEF”字段填写 `i32(r,0xEF),`，这样导出的 JSON/表格能保留这一项证据。
        "漫游范围X原值_0xEF": i32(r,0xEF),
        # 给当前结果字典的“漫游范围Y原值_0xF3”字段填写 `i32(r,0xF3),`，这样导出的 JSON/表格能保留这一项证据。
        "漫游范围Y原值_0xF3": i32(r,0xF3),
        # 给当前结果字典的“漫游伙伴字段_0xF7”字段填写 `i32(r,0xF7),`，这样导出的 JSON/表格能保留这一项证据。
        "漫游伙伴字段_0xF7": i32(r,0xF7),
        # 给当前结果字典的“漫游伙伴字段_0xFB”字段填写 `i32(r,0xFB),`，这样导出的 JSON/表格能保留这一项证据。
        "漫游伙伴字段_0xFB": i32(r,0xFB),
        # 给当前结果字典的“在场状态变量ID_0xFF”字段填写 `u32(r,0xFF),`，这样导出的 JSON/表格能保留这一项证据。
        "在场状态变量ID_0xFF": u32(r,0xFF),
        # 给当前结果字典的“默认方向代码_0x103”字段填写 `r[0x103],`，这样导出的 JSON/表格能保留这一项证据。
        "默认方向代码_0x103": r[0x103],
        # 给当前结果字典的“未知尾_0x104_0x226”字段填写 `hexblock(r,0x104,ENTITY_RECORD_SIZE-0x104),`，这样导出的 JSON/表格能保留这一项证据。
        "未知尾_0x104_0x226": hexblock(r,0x104,ENTITY_RECORD_SIZE-0x104),
        # 给当前结果字典的“动画选择公式”字段填写 `"variant=(record[0xDF]*orientation_code)>>3；state 0/1/2 选择四组 start/le…`，这样导出的 JSON/表格能保留这一项证据。
        "动画选择公式": "variant=(record[0xDF]*orientation_code)>>3；state 0/1/2 选择四组 start/length；state3 使用特殊 sentinel 0x3640E",
        # 给当前结果字典的“Reader证据”字段填写 `{"记录Loader":"0x0040A160","动作状态机":"0x0040A2B0","SF2动作入口":"0x004073D0 /…`，这样导出的 JSON/表格能保留这一项证据。
        "Reader证据": {"记录Loader":"0x0040A160","动作状态机":"0x0040A2B0","SF2动作入口":"0x004073D0 / 0x00407400","behavior_type消费者":"0x004096BB/0x00409701/0x0040A1D9/0x0040A4D4/0x0040A7E5/0x0040AAF7；EVE writers 0x0040C83F/0x0040F101","移动档消费者":"0x0040A193..0x0040A1B8 初始化；0x0040A83D..0x0040A86D 每轮重载；profile selector由runtime+0x0C & 0xFF00布尔化","E1控制器映射":"Entity+0xE1 → Controller+0x20；0x004074A0 非零时先绘 Section0 entry0","EE控制器映射":"Entity+0xEE → Controller+0x2C；0x0040771A/0x00407BFA 门控额外空间/覆盖修正路径","EA事件绑定":"Entity+0xEA由0x00409A50接触/重叠处理链读取，并在条件命中时送入0x0040B230激活Event；EVE 0x8F路径0x0040F0DB..0x0040F0F6写+0x74=1、+EA=参数并清+EB/+0x73","EB事件绑定":"Entity+0xEB由0x00409860主动交互选择链读取并送入0x0040B230；runtime+0x54临时交互状态在0x0040A2B0按全局active Event ID 0x0089F808与+EB比对后恢复原behavior与方向"},
    }


# 定义函数 `parse_sci`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_sci(path: Path) -> Dict[str, Any]:
    # 把右侧 `path.read_bytes(); cls=classify_sci(path)` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b=path.read_bytes(); cls=classify_sci(path)
    # 把右侧 `{"工具版本":TOOL_VERSION,"文件":str(path),"大小":len(b),"分类":cls,"记录":[]}` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out={"工具版本":TOOL_VERSION,"文件":str(path),"大小":len(b),"分类":cls,"记录":[]}
    # 检查条件 `cls["类型"]=="地图SCI"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if cls["类型"]=="地图SCI":
        # 执行一次赋值：把 `[parse_map_record(b[i:i+MAP_RECORD_SIZE],n) for n,i in enumerate(range(0,l…` 的结果放入左侧 `out["记录"]` 对应的变量/字段。
        out["记录"]=[parse_map_record(b[i:i+MAP_RECORD_SIZE],n) for n,i in enumerate(range(0,len(b),MAP_RECORD_SIZE))]
    # 前一个条件没有成立时，再检查 `cls["类型"]=="实体SCI"`；成立才执行这一分支。
    elif cls["类型"]=="实体SCI":
        # 执行一次赋值：把 `[parse_entity_record(b[i:i+ENTITY_RECORD_SIZE],n) for n,i in enumerate(ran…` 的结果放入左侧 `out["记录"]` 对应的变量/字段。
        out["记录"]=[parse_entity_record(b[i:i+ENTITY_RECORD_SIZE],n) for n,i in enumerate(range(0,len(b),ENTITY_RECORD_SIZE))]
    # 前一个条件没有成立时，再检查 `cls["类型"]=="DDDES显示定时配置"`；成立才执行这一分支。
    elif cls["类型"]=="DDDES显示定时配置":
        # 把右侧 `(len(b)-0x12)//0x34` 计算得到的值保存到 `count`，后面的判断或输出会继续使用它。
        count=(len(b)-0x12)//0x34
        # 执行一次赋值：把 `{` 的结果放入左侧 `out["头"]` 对应的变量/字段。
        out["头"]={
            # 给当前结果字典的“初始全屏窗口状态_0x00”字段填写 `bool(b[0]),`，这样导出的 JSON/表格能保留这一项证据。
            "初始全屏窗口状态_0x00": bool(b[0]),
            # 给当前结果字典的“水平后备缓冲扩展每侧_0x01”字段填写 `u32(b,1),`，这样导出的 JSON/表格能保留这一项证据。
            "水平后备缓冲扩展每侧_0x01":u32(b,1),
            # 给当前结果字典的“垂直后备缓冲扩展每侧_0x05”字段填写 `u32(b,5),`，这样导出的 JSON/表格能保留这一项证据。
            "垂直后备缓冲扩展每侧_0x05":u32(b,5),
            # 给当前结果字典的“核心显示宽_0x09”字段填写 `u32(b,9),`，这样导出的 JSON/表格能保留这一项证据。
            "核心显示宽_0x09":u32(b,9),
            # 给当前结果字典的“核心显示高_0x0D”字段填写 `u32(b,13),`，这样导出的 JSON/表格能保留这一项证据。
            "核心显示高_0x0D":u32(b,13),
            # 给当前结果字典的“主逻辑定时触发频率参数_0x11”字段填写 `b[0x11],`，这样导出的 JSON/表格能保留这一项证据。
            "主逻辑定时触发频率参数_0x11":b[0x11],
            # 给当前结果字典的“定时公式”字段填写 `"base=100Hz；threshold=integer(100/frequency_parameter)；到阈值后PostMessag…`，这样导出的 JSON/表格能保留这一项证据。
            "定时公式":"base=100Hz；threshold=integer(100/frequency_parameter)；到阈值后PostMessageA(hwnd, WM_TIMER=0x0113,0,0)。参数必须非零；若100不能整除则存在整数截断。",
            # 给当前结果字典的“显示缓冲公式”字段填写 `"backing_width=core_width+2*horizontal_extra；backing_height=core_heig…`，这样导出的 JSON/表格能保留这一项证据。
            "显示缓冲公式":"backing_width=core_width+2*horizontal_extra；backing_height=core_height+2*vertical_extra",
            # 给当前结果字典的“证据”字段填写 `"0x40152C..0x401595；0x405960；0x401980..0x401ABF；0x404A60..0x404D27",`，这样导出的 JSON/表格能保留这一项证据。
            "证据":"0x40152C..0x401595；0x405960；0x401980..0x401ABF；0x404A60..0x404D27",
        }
        # 执行一次赋值：把 `[{"索引":i,"偏移":f"0x{0x12+i*0x34:X}","大小":0x34,"原始十六进制":b[0x12+i*0x34:0x12+(…` 的结果放入左侧 `out["记录"]` 对应的变量/字段。
        out["记录"]=[{"索引":i,"偏移":f"0x{0x12+i*0x34:X}","大小":0x34,"原始十六进制":b[0x12+i*0x34:0x12+(i+1)*0x34].hex(" "),"CP950字符串扫描":_scan_strings(b[0x12+i*0x34:0x12+(i+1)*0x34])} for i in range(count)]
        # 执行一次赋值：把 `"当前EXE已确认加载并计数52-byte记录区；修正版2继续证明0x004025B0的mode2路径最终只把新buffer写入caller out…` 的结果放入左侧 `out["记录语义边界"]` 对应的变量/字段。
        out["记录语义边界"]="当前EXE已确认加载并计数52-byte记录区；修正版2继续证明0x004025B0的mode2路径最终只把新buffer写入caller outptr，Loader不另存buffer别名；全EXE对0x0046F380仅4处直接绝对引用，已知消息方法与析构均不读取+0x0C记录指针/+0x10记录数。当前原始样本record=(24,24,\"SYS\\font24.fnt\")，而构造器0x0040159A直接硬编码24/24与0x00468070的..\\MultiMedia\\Font\\Font24.Fnt；因此冻结为‘静态可达直接路径无consumer、强Legacy/硬编码旁路证据’，但纯静态分析仍不能形式化排除未知间接指针复制。"
    # 前一个条件没有成立时，再检查 `cls["类型"]=="音乐列表SCI"`；成立才执行这一分支。
    elif cls["类型"]=="音乐列表SCI":
        # 把右侧 `0x33` 计算得到的值保存到 `recsz`，后面的判断或输出会继续使用它。
        recsz=0x33
        # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
        rows=[]
        # 开始循环 `i in range(0,len(b),recsz)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i in range(0,len(b),recsz):
            # 把右侧 `b[i:i+recsz]` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=b[i:i+recsz]
            # 把右侧 `r[1:0x33].split(b"\0",1)[0]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
            raw=r[1:0x33].split(b"\0",1)[0]
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            rows.append({
                # 给当前结果字典的“索引”字段填写 `i//recsz,`，这样导出的 JSON/表格能保留这一项证据。
                "索引":i//recsz,
                # 给当前结果字典的“偏移”字段填写 `f"0x{i:X}",`，这样导出的 JSON/表格能保留这一项证据。
                "偏移":f"0x{i:X}",
                # 给当前结果字典的“未知标志_0x00”字段填写 `r[0],`，这样导出的 JSON/表格能保留这一项证据。
                "未知标志_0x00":r[0],
                # 给当前结果字典的“资源基名_0x01”字段填写 `raw.decode("ascii",errors="replace"),`，这样导出的 JSON/表格能保留这一项证据。
                "资源基名_0x01":raw.decode("ascii",errors="replace"),
                # 给当前结果字典的“字符串字段原始_0x01_0x32”字段填写 `r[1:0x33].hex(" "),`，这样导出的 JSON/表格能保留这一项证据。
                "字符串字段原始_0x01_0x32":r[1:0x33].hex(" "),
            })
        # 执行一次赋值：把 `rows` 的结果放入左侧 `out["记录"]` 对应的变量/字段。
        out["记录"]=rows
        # 执行一次赋值：把 `"51-byte record已由0x00403220的file_size/51与两处index*51+1 consumer直接确认。+0x01..…` 的结果放入左侧 `out["记录语义边界"]` 对应的变量/字段。
        out["记录语义边界"]="51-byte record已由0x00403220的file_size/51与两处index*51+1 consumer直接确认。+0x01..+0x32是50-byte NUL终止资源基名缓冲；+0x00虽在当前WAVList样本恒1、MP3List样本恒0，但尚未找到直接consumer，保持UNKNOWN。构造器数据流已闭合：MP3List.SCI写入object+0x08并在0x004032FA路径格式化为%s.MP3；WAVList.SCI写入object+0x10并在0x004033A1路径格式化为%s.WAV。"
    # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return out


# 定义函数 `_scan_strings`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _scan_strings(b: bytes) -> List[str]:
    # 把右侧 `[]; cur=bytearray()` 计算得到的值保存到 `res`，后面的判断或输出会继续使用它。
    res=[]; cur=bytearray()
    # 开始循环 `x in b+b"\0"`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for x in b+b"\0":
        # 检查条件 `0x20 <= x <= 0x7E or x>=0x80`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if 0x20 <= x <= 0x7E or x>=0x80:
            # 把这一条新结果追加到列表 `cur`；不会覆盖前面已经收集的记录。
            cur.append(x)
        # 前面的条件分支都没有命中时，执行这个兜底分支。
        else:
            # 检查条件 `len(cur)>=4`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if len(cur)>=4:
                # 执行一次赋值：把 `"replace"))` 的结果放入左侧 `res.append(bytes(cur).decode("cp950",errors` 对应的变量/字段。
                res.append(bytes(cur).decode("cp950",errors="replace"))
            # 调用 `cur.clear` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            cur.clear()
    # 把 `res` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return res


# 定义函数 `sf2_decompress`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sf2_decompress(raw: bytes) -> Tuple[bytes,Dict[str,Any]]:
    # 检查条件 `len(raw)<SF2_HEADER_SIZE or raw[:4]!=SF2_MAGIC`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(raw)<SF2_HEADER_SIZE or raw[:4]!=SF2_MAGIC: raise ValueError("非当前已知 SF2")
    # 把右侧 `{"原始大小":len(raw),"原压缩标志":raw[0x0B],"压缩包装":raw[0x0B]==0x6F}` 计算得到的值保存到 `meta`，后面的判断或输出会继续使用它。
    meta={"原始大小":len(raw),"原压缩标志":raw[0x0B],"压缩包装":raw[0x0B]==0x6F}
    # 检查条件 `raw[0x0B]!=0x6F`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if raw[0x0B]!=0x6F: return raw,meta
    # 检查条件 `len(raw)<SF2_HEADER_SIZE+8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(raw)<SF2_HEADER_SIZE+8: raise ValueError("0x6F 包装头不完整")
    # 把右侧 `u32(raw,SF2_HEADER_SIZE); f2=u32(raw,SF2_HEADER_SIZE+4)` 计算得到的值保存到 `f1`，后面的判断或输出会继续使用它。
    f1=u32(raw,SF2_HEADER_SIZE); f2=u32(raw,SF2_HEADER_SIZE+4)
    # 检查条件 `f1!=f2`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if f1!=f2: raise ValueError("0x6F 两个最终尺寸不一致")
    # 把右侧 `zlib.decompress(raw[SF2_HEADER_SIZE+8:])` 计算得到的值保存到 `tail`，后面的判断或输出会继续使用它。
    tail=zlib.decompress(raw[SF2_HEADER_SIZE+8:])
    # 把右侧 `bytearray(raw[:SF2_HEADER_SIZE]+tail); b[0x0B]=0` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b=bytearray(raw[:SF2_HEADER_SIZE]+tail); b[0x0B]=0
    # 调用 `meta.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    meta.update({"声明最终大小":f1,"重建大小":len(b),"最终大小吻合":len(b)==f1})
    # 把 `bytes(b),meta` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return bytes(b),meta


# 定义函数 `sf2_sections`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sf2_sections(b: bytes) -> List[Dict[str,Any]]:
    # 把右侧 `[]` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out=[]
    # 开始循环 `i in range(4)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(4):
        # 把右侧 `0x14+i*10; count=u16(b,h); off=u32(b,h+2); size=u32(b,h+6)` 计算得到的值保存到 `h`，后面的判断或输出会继续使用它。
        h=0x14+i*10; count=u16(b,h); off=u32(b,h+2); size=u32(b,h+6)
        # 把右侧 `{"索引":i,"数量":count,"偏移":off,"大小":size,"记录":[]}` 计算得到的值保存到 `sec`，后面的判断或输出会继续使用它。
        sec={"索引":i,"数量":count,"偏移":off,"大小":size,"记录":[]}
        # 检查条件 `count`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if count:
            # 检查条件 `off+size>len(b) or off+count*4>off+size`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if off+size>len(b) or off+count*4>off+size: raise ValueError(f"Section{i} 边界非法")
            # 把右侧 `[u32(b,off+j*4) for j in range(count)]; ends=ptr[1:]+[off+size]` 计算得到的值保存到 `ptr`，后面的判断或输出会继续使用它。
            ptr=[u32(b,off+j*4) for j in range(count)]; ends=ptr[1:]+[off+size]
            # 开始循环 `j,(s,e) in enumerate(zip(ptr,ends))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for j,(s,e) in enumerate(zip(ptr,ends)):
                # 检查条件 `s<off+count*4 or e<s or e>off+size`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if s<off+count*4 or e<s or e>off+size: raise ValueError(f"Section{i} record{j} 边界非法")
                # 执行这一条实际代码步骤：`sec["记录"].append({"索引":j,"偏移":s,"结束":e,"大小":e-s})`；它与上下相邻语句共同完成当前函数的小任务。
                sec["记录"].append({"索引":j,"偏移":s,"结束":e,"大小":e-s})
        # 把这一条新结果追加到列表 `out`；不会覆盖前面已经收集的记录。
        out.append(sec)
    # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return out


# 定义函数 `decode_tile`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def decode_tile(rec: bytes, tile_w:int, tile_h:int) -> int:
    # 把右侧 `tile_w*tile_h; produced=0; p=0` 计算得到的值保存到 `total`，后面的判断或输出会继续使用它。
    total=tile_w*tile_h; produced=0; p=0
    # 只要条件 `produced<total` 仍然成立，就重复执行下面的循环体；循环体必须最终改变条件或主动退出。
    while produced<total:
        # 检查条件 `p>=len(rec)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if p>=len(rec): raise ValueError("RLE 数据提前结束")
        # 把右侧 `rec[p]; p+=1; n=(c&0x3F)+1` 计算得到的值保存到 `c`，后面的判断或输出会继续使用它。
        c=rec[p]; p+=1; n=(c&0x3F)+1
        # 检查条件 `(c&0x40)==0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if (c&0x40)==0:
            # 把右侧 `n*2` 计算得到的值保存到 `need`，后面的判断或输出会继续使用它。
            need=n*2
            # 检查条件 `p+need>len(rec)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if p+need>len(rec): raise ValueError("RLE literal 超出记录")
            # 执行一次赋值：把 `need` 的结果放入左侧 `p+` 对应的变量/字段。
            p+=need
        # 前一个条件没有成立时，再检查 `c&0x80`；成立才执行这一分支。
        elif c&0x80:
            # 检查条件 `p+2>len(rec)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if p+2>len(rec): raise ValueError("RLE repeat 缺颜色")
            # 执行一次赋值：把 `2` 的结果放入左侧 `p+` 对应的变量/字段。
            p+=2
        # 0x40 set and 0x80 clear = transparent skip
        # 执行一次赋值：把 `n` 的结果放入左侧 `produced+` 对应的变量/字段。
        produced+=n
        # 检查条件 `produced>total`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if produced>total: raise ValueError(f"RLE 输出超过 {tile_w}×{tile_h} tile")
    # 检查条件 `p!=len(rec)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if p!=len(rec): raise ValueError(f"RLE 未精确消耗记录：{p}/{len(rec)}")
    # 把 `produced` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return produced


# 定义函数 `parse_sf2`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def parse_sf2(path: Path, validate_rle: bool=True) -> Dict[str,Any]:
    # 把右侧 `path.read_bytes(); b,wrap=sf2_decompress(raw); secs=sf2_sections(b)` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
    raw=path.read_bytes(); b,wrap=sf2_decompress(raw); secs=sf2_sections(b)
    # 把右侧 `u16(b,7); th=u16(b,9)` 计算得到的值保存到 `tw`，后面的判断或输出会继续使用它。
    tw=u16(b,7); th=u16(b,9)
    # 把右侧 `{` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out={
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,"文件":str(path),"SHA256":hashlib.sha256(raw).hexdigest(),…`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本":TOOL_VERSION,"文件":str(path),"SHA256":hashlib.sha256(raw).hexdigest(),"包装":wrap,
        # 给当前结果字典的“版本字节_0x03”字段填写 `b[3],"头字段_0x04":b[4],"固定头大小_0x05":u16(b,5),`，这样导出的 JSON/表格能保留这一项证据。
        "版本字节_0x03":b[3],"头字段_0x04":b[4],"固定头大小_0x05":u16(b,5),
        # 给当前结果字典的“Tile宽_0x07”字段填写 `tw,"Tile高_0x09":th,"画布宽_0x0C":u32(b,0x0C),"画布高_0x10":u32(b,0x10),`，这样导出的 JSON/表格能保留这一项证据。
        "Tile宽_0x07":tw,"Tile高_0x09":th,"画布宽_0x0C":u32(b,0x0C),"画布高_0x10":u32(b,0x10),
        # 给当前结果字典的“Section”字段填写 `[{k:v for k,v in s.items() if k!="记录"} for s in secs],"渲染模式分布":{},"有效…`，这样导出的 JSON/表格能保留这一项证据。
        "Section":[{k:v for k,v in s.items() if k!="记录"} for s in secs],"渲染模式分布":{},"有效":True,
    }
    # 检查条件 `u16(b,5)!=SF2_HEADER_SIZE`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if u16(b,5)!=SF2_HEADER_SIZE: raise ValueError("SF2 Header size 非 0x42BC")
    # 检查条件 `validate_rle`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if validate_rle:
        # 开始循环 `rr in secs[2]["记录"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for rr in secs[2]["记录"]:
            # 调用 `decode_tile` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            decode_tile(b[rr["偏移"]:rr["结束"]],tw,th)
    # Section1
    # 把右侧 `True` 计算得到的值保存到 `s1_ok`，后面的判断或输出会继续使用它。
    s1_ok=True
    # 开始循环 `rr in secs[1]["记录"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for rr in secs[1]["记录"]:
        # 把右侧 `b[rr["偏移"]:rr["结束"]]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec=b[rr["偏移"]:rr["结束"]]
        # 检查条件 `len(rec)<24`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(rec)<24: s1_ok=False; break
        # 把右侧 `u16(rec,0); rows=u16(rec,2)` 计算得到的值保存到 `cols`，后面的判断或输出会继续使用它。
        cols=u16(rec,0); rows=u16(rec,2)
        # 检查条件 `len(rec)!=24+2*cols*rows`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(rec)!=24+2*cols*rows: s1_ok=False; break
    # 执行一次赋值：把 `s1_ok` 的结果放入左侧 `out["Section1记录大小公式全部成立"]` 对应的变量/字段。
    out["Section1记录大小公式全部成立"]=s1_ok
    # Section0 + modes
    # 把右侧 `Counter(); s0_ok=True; layers=0` 计算得到的值保存到 `mode`，后面的判断或输出会继续使用它。
    mode=Counter(); s0_ok=True; layers=0
    # 开始循环 `rr in secs[0]["记录"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for rr in secs[0]["记录"]:
        # 把右侧 `b[rr["偏移"]:rr["结束"]]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec=b[rr["偏移"]:rr["结束"]]
        # 检查条件 `len(rec)<0x5E`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(rec)<0x5E: s0_ok=False; continue
        # 把右侧 `u16(rec,0x54)` 计算得到的值保存到 `n`，后面的判断或输出会继续使用它。
        n=u16(rec,0x54)
        # 检查条件 `len(rec)!=0x5E+29*n`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(rec)!=0x5E+29*n: s0_ok=False; continue
        # 开始循环 `j in range(n)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for j in range(n):
            # 把右侧 `rec[0x5E+j*29+0x0A]; mode[m]+=1; layers+=1` 计算得到的值保存到 `m`，后面的判断或输出会继续使用它。
            m=rec[0x5E+j*29+0x0A]; mode[m]+=1; layers+=1
    # 执行一次赋值：把 `s0_ok` 的结果放入左侧 `out["Section0记录大小公式全部成立"]` 对应的变量/字段。
    out["Section0记录大小公式全部成立"]=s0_ok
    # 执行一次赋值：把 `layers` 的结果放入左侧 `out["Section0子层数"]` 对应的变量/字段。
    out["Section0子层数"]=layers
    # 执行一次赋值：把 `{str(k):v for k,v in sorted(mode.items())}` 的结果放入左侧 `out["渲染模式分布"]` 对应的变量/字段。
    out["渲染模式分布"]={str(k):v for k,v in sorted(mode.items())}
    # 执行一次赋值：把 `{str(k):v for k,v in sorted(mode.items()) if k not in SF2_RENDER_DISPATCH …` 的结果放入左侧 `out["原版分派未知或fallback模式出现"]` 对应的变量/字段。
    out["原版分派未知或fallback模式出现"]={str(k):v for k,v in sorted(mode.items()) if k not in SF2_RENDER_DISPATCH or SF2_RENDER_DISPATCH.get(k) is None}
    # 执行一次赋值：把 `u32(b,0x427C)` 的结果放入左侧 `out["固定头运行时标记磁盘值_0x427C"]` 对应的变量/字段。
    out["固定头运行时标记磁盘值_0x427C"]=u32(b,0x427C)
    # 执行一次赋值：把 `s1_ok and s0_ok` 的结果放入左侧 `out["有效"]` 对应的变量/字段。
    out["有效"] = s1_ok and s0_ok
    # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return out


# 定义函数 `_norm_rel`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _norm_rel(s: str) -> str:
    # 把 `s.replace("\\","/").lstrip("./").casefold()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return s.replace("\\","/").lstrip("./").casefold()


# 定义函数 `build_asset_index`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def build_asset_index(roots: Iterable[Path]) -> Dict[str,List[Path]]:
    # 把右侧 `defaultdict(list)` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
    idx=defaultdict(list)
    # 开始循环 `root in roots`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for root in roots:
        # 检查条件 `not root.exists()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not root.exists(): continue
        # 开始循环 `p in root.rglob("*")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for p in root.rglob("*"):
            # 检查条件 `not p.is_file()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not p.is_file(): continue
            # 创建变量 `try`（类型提示为 `rel`），并把 `p.relative_to(root)` 的结果保存进去供后续步骤使用。
            try: rel=p.relative_to(root)
            # 捕获 `ValueError` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except ValueError: continue
            # 执行这一条实际代码步骤：`idx[_norm_rel(str(rel))].append(p)`；它与上下相邻语句共同完成当前函数的小任务。
            idx[_norm_rel(str(rel))].append(p)
            # 同时允许从顶层 multimedia/ 等额外前缀后解析。
            # 把右侧 `rel.parts` 计算得到的值保存到 `parts`，后面的判断或输出会继续使用它。
            parts=rel.parts
            # 开始循环 `cut in range(1,min(len(parts),3))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for cut in range(1,min(len(parts),3)):
                # 执行这一条实际代码步骤：`idx[_norm_rel(str(Path(*parts[cut:])))].append(p)`；它与上下相邻语句共同完成当前函数的小任务。
                idx[_norm_rel(str(Path(*parts[cut:])))].append(p)
            # 执行这一条实际代码步骤：`idx[p.name.casefold()].append(p)`；它与上下相邻语句共同完成当前函数的小任务。
            idx[p.name.casefold()].append(p)
    # 把 `idx` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return idx


# 定义函数 `resolve_asset`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def resolve_asset(ref: str, idx: Dict[str,List[Path]]) -> List[Path]:
    # 检查条件 `not ref`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if not ref: return []
    # 把右侧 `[_norm_rel(ref),Path(ref.replace("\\","/")).name.casefold()]` 计算得到的值保存到 `keys`，后面的判断或输出会继续使用它。
    keys=[_norm_rel(ref),Path(ref.replace("\\","/")).name.casefold()]
    # 把右侧 `[]` 计算得到的值保存到 `seen`，后面的判断或输出会继续使用它。
    seen=[]
    # 开始循环 `k in keys`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for k in keys:
        # 开始循环 `p in idx.get(k,[])`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for p in idx.get(k,[]):
            # 检查条件 `p not in seen`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if p not in seen: seen.append(p)
    # 把 `seen` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return seen


# 定义函数 `cross_validate_entity_sf2`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def cross_validate_entity_sf2(sci_paths: List[Path], roots: List[Path]) -> Dict[str,Any]:
    # 把右侧 `build_asset_index(roots); rows=[]; stat=Counter()` 计算得到的值保存到 `idx`，后面的判断或输出会继续使用它。
    idx=build_asset_index(roots); rows=[]; stat=Counter()
    # 开始循环 `sp in sci_paths`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for sp in sci_paths:
        # 把右侧 `parse_sci(sp)` 计算得到的值保存到 `parsed`，后面的判断或输出会继续使用它。
        parsed=parse_sci(sp)
        # 检查条件 `parsed["分类"]["类型"]!="实体SCI"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if parsed["分类"]["类型"]!="实体SCI": continue
        # 开始循环 `ent in parsed["记录"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for ent in parsed["记录"]:
            # 把右侧 `ent["SF2路径_0x7B"]; matches=resolve_asset(ref,idx)` 计算得到的值保存到 `ref`，后面的判断或输出会继续使用它。
            ref=ent["SF2路径_0x7B"]; matches=resolve_asset(ref,idx)
            # 把右侧 `{"SCI":str(sp),"实体索引":ent["索引"],"名称":ent["名称_0x00"],"SF2引用":ref,"候选文件":[st…` 计算得到的值保存到 `row`，后面的判断或输出会继续使用它。
            row={"SCI":str(sp),"实体索引":ent["索引"],"名称":ent["名称_0x00"],"SF2引用":ref,"候选文件":[str(x) for x in matches]}
            # 检查条件 `not ref`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not ref:
                # 执行一次赋值：把 `"无SF2引用"; stat["无SF2引用"]+=1; rows.append(row); continue` 的结果放入左侧 `row["状态"]` 对应的变量/字段。
                row["状态"]="无SF2引用"; stat["无SF2引用"]+=1; rows.append(row); continue
            # 检查条件 `not matches`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not matches:
                # 执行一次赋值：把 `"当前资源根缺样本"; stat["缺样本"]+=1; rows.append(row); continue` 的结果放入左侧 `row["状态"]` 对应的变量/字段。
                row["状态"]="当前资源根缺样本"; stat["缺样本"]+=1; rows.append(row); continue
            # 把右侧 `matches[0]` 计算得到的值保存到 `target`，后面的判断或输出会继续使用它。
            target=matches[0]
            # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
            try:
                # 把右侧 `parse_sf2(target,False); count=sf["Section"][0]["数量"]` 计算得到的值保存到 `sf`，后面的判断或输出会继续使用它。
                sf=parse_sf2(target,False); count=sf["Section"][0]["数量"]
                # 把右侧 `[]; all_ok=True` 计算得到的值保存到 `checks`，后面的判断或输出会继续使用它。
                checks=[]; all_ok=True
                # 开始循环 `pair in ent["动作组"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                for pair in ent["动作组"]:
                    # 把右侧 `pair["start"]; ln=pair["length"]` 计算得到的值保存到 `st`，后面的判断或输出会继续使用它。
                    st=pair["start"]; ln=pair["length"]
                    # 检查条件 `st==0 and ln==0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                    if st==0 and ln==0: ok=True; meaning="未使用"
                    # 前面的条件分支都没有命中时，执行这个兜底分支。
                    else:
                        # 把右侧 `(ln>0 and st>=0 and st+ln<=count)` 计算得到的值保存到 `ok`，后面的判断或输出会继续使用它。
                        ok=(ln>0 and st>=0 and st+ln<=count)
                        # 把右侧 `"范围有效" if ok else "范围越界/需进一步解释"` 计算得到的值保存到 `meaning`，后面的判断或输出会继续使用它。
                        meaning="范围有效" if ok else "范围越界/需进一步解释"
                    # 把这一条新结果追加到列表 `checks`；不会覆盖前面已经收集的记录。
                    checks.append({**pair,"Section0记录数":count,"通过":ok,"说明":meaning})
                    # 执行一次赋值：把 `ok` 的结果放入左侧 `all_ok &` 对应的变量/字段。
                    all_ok &= ok
                # 调用 `row.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
                row.update({"状态":"PASS" if all_ok else "FAIL","解析SF2":str(target),"Section0记录数":count,"动作范围":checks})
                # 执行一次赋值：把 `1` 的结果放入左侧 `stat["PASS" if all_ok else "FAIL"]+` 对应的变量/字段。
                stat["PASS" if all_ok else "FAIL"]+=1
            # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except Exception as e:
                # 执行一次赋值：把 `1` 的结果放入左侧 `row.update({"状态":"SF2解析失败","错误":str(e)}); stat["SF2解析失…` 对应的变量/字段。
                row.update({"状态":"SF2解析失败","错误":str(e)}); stat["SF2解析失败"]+=1
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            rows.append(row)
    # 把 `{"工具版本":TOOL_VERSION,"资源根":[str(x) for x in roots],"汇总":dict(stat),"结果":rows}` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"工具版本":TOOL_VERSION,"资源根":[str(x) for x in roots],"汇总":dict(stat),"结果":rows}


# 定义函数 `find_unique_sf2`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def find_unique_sf2(roots: List[Path]) -> Dict[str,List[Path]]:
    # 把右侧 `defaultdict(list)` 计算得到的值保存到 `byhash`，后面的判断或输出会继续使用它。
    byhash=defaultdict(list)
    # 开始循环 `root in roots`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for root in roots:
        # 检查条件 `not root.exists()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not root.exists(): continue
        # 开始循环 `p in root.rglob("*")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for p in root.rglob("*"):
            # 检查条件 `p.is_file() and p.suffix.lower()==".sf2"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if p.is_file() and p.suffix.lower()==".sf2":
                # 把右侧 `hashlib.sha256(p.read_bytes()).hexdigest(); byhash[h].append(p)` 计算得到的值保存到 `h`，后面的判断或输出会继续使用它。
                h=hashlib.sha256(p.read_bytes()).hexdigest(); byhash[h].append(p)
    # 把 `byhash` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return byhash


# 定义函数 `sf2_batch_dedup`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sf2_batch_dedup(roots: List[Path]) -> Dict[str,Any]:
    # 把右侧 `find_unique_sf2(roots); rows=[]; total=Counter(); modes=Counter(); tile_di…` 计算得到的值保存到 `groups`，后面的判断或输出会继续使用它。
    groups=find_unique_sf2(roots); rows=[]; total=Counter(); modes=Counter(); tile_dims=Counter()
    # 开始循环 `h,paths in sorted(groups.items())`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for h,paths in sorted(groups.items()):
        # 把右侧 `paths[0]; total["唯一文件"]+=1` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
        p=paths[0]; total["唯一文件"]+=1
        # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
        try:
            # 把右侧 `parse_sf2(p,True); counts=[x["数量"] for x in r["Section"]]` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=parse_sf2(p,True); counts=[x["数量"] for x in r["Section"]]
            # 开始循环 `i,c in enumerate(counts)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for i,c in enumerate(counts): total[f"Section{i}记录"]+=c
            # 执行一次赋值：把 `r["Section0子层数"]` 的结果放入左侧 `total["Section0子层"]+` 对应的变量/字段。
            total["Section0子层"]+=r["Section0子层数"]
            # 执行一次赋值：把 `1 if r["包装"]["压缩包装"] else 0` 的结果放入左侧 `total["压缩SF2"]+` 对应的变量/字段。
            total["压缩SF2"]+=1 if r["包装"]["压缩包装"] else 0
            # 执行一次赋值：把 `1; tile_dims[(r["Tile宽_0x07"],r["Tile高_0x09"])]+=1` 的结果放入左侧 `total["PASS"]+` 对应的变量/字段。
            total["PASS"]+=1; tile_dims[(r["Tile宽_0x07"],r["Tile高_0x09"])]+=1
            # 开始循环 `k,v in r["渲染模式分布"].items()`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for k,v in r["渲染模式分布"].items(): modes[int(k)]+=v
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            rows.append({"SHA256":h,"代表文件":str(p),"同内容路径数":len(paths),"Tile":[r["Tile宽_0x07"],r["Tile高_0x09"]],"Section数量":counts,"子层":r["Section0子层数"],"压缩":r["包装"]["压缩包装"],"PASS":True})
        # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
        except Exception as e:
            # 执行一次赋值：把 `1; rows.append({"SHA256":h,"代表文件":str(p),"同内容路径数":len(paths),"PASS":False,…` 的结果放入左侧 `total["FAIL"]+` 对应的变量/字段。
            total["FAIL"]+=1; rows.append({"SHA256":h,"代表文件":str(p),"同内容路径数":len(paths),"PASS":False,"错误":str(e)})
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,"资源根":[str(x) for x in roots],"输入路径文件总数":sum(len(v) for …`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本":TOOL_VERSION,"资源根":[str(x) for x in roots],"输入路径文件总数":sum(len(v) for v in groups.values()),
        # 给当前结果字典的“唯一SHA256文件数”字段填写 `len(groups),"汇总":dict(total),`，这样导出的 JSON/表格能保留这一项证据。
        "唯一SHA256文件数":len(groups),"汇总":dict(total),
        # 给当前结果字典的“Tile尺寸分布”字段填写 `{f"{w}x{h}":n for (w,h),n in sorted(tile_dims.items())},`，这样导出的 JSON/表格能保留这一项证据。
        "Tile尺寸分布":{f"{w}x{h}":n for (w,h),n in sorted(tile_dims.items())},
        # 给当前结果字典的“渲染模式分布”字段填写 `{str(k):v for k,v in sorted(modes.items())},"文件":rows,`，这样导出的 JSON/表格能保留这一项证据。
        "渲染模式分布":{str(k):v for k,v in sorted(modes.items())},"文件":rows,
    }


# 定义函数 `render_dispatch_dictionary`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def render_dispatch_dictionary() -> Dict[str,Any]:
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows=[]
    # 开始循环 `m in range(31)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for m in range(31):
        # 把右侧 `SF2_RENDER_DISPATCH.get(m)` 计算得到的值保存到 `item`，后面的判断或输出会继续使用它。
        item=SF2_RENDER_DISPATCH.get(m)
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append({"原始模式":m,"处理器":f"0x{item[0]:08X}" if item else None,"Canonical名称":item[1] if item else "FALLBACK_NOOP","原版行为":"专用处理器" if item else "fallback/no-op"})
    # 把 `{"工具版本":TOOL_VERSION,"分派入口":"0x00429480","分派表":"0x00429AC4 / 0x00429A90","模式":rows}` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"工具版本":TOOL_VERSION,"分派入口":"0x00429480","分派表":"0x00429AC4 / 0x00429A90","模式":rows}



# 定义函数 `_split_rgb555`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _split_rgb555(pixel: int) -> Tuple[int, int, int]:
    """把 15-bit RGB555 像素拆成三个 0..31 的整数通道。

    给初学者看的解释：一个 16 位整数里并不是“一个颜色数字”，而是把红、绿、蓝
    三个小数字塞进不同的二进制位。这里先用右移把目标通道挪到最低位，再用 0x1F
    （二进制 11111）只留下 5 位。返回顺序是 (高5位, 中5位, 低5位)。
    """
    # 把右侧 `(pixel >> 10) & 0x1F` 计算得到的值保存到 `high`，后面的判断或输出会继续使用它。
    high = (pixel >> 10) & 0x1F
    # 把右侧 `(pixel >> 5) & 0x1F` 计算得到的值保存到 `middle`，后面的判断或输出会继续使用它。
    middle = (pixel >> 5) & 0x1F
    # 把右侧 `pixel & 0x1F` 计算得到的值保存到 `low`，后面的判断或输出会继续使用它。
    low = pixel & 0x1F
    # 把 `high, middle, low` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return high, middle, low


# 定义函数 `_split_rgb565`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _split_rgb565(pixel: int) -> Tuple[int, int, int]:
    # 执行这一条实际代码步骤：`"""把 RGB565 像素拆成高5位、绿色6位、低5位三个整数通道。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """把 RGB565 像素拆成高5位、绿色6位、低5位三个整数通道。"""
    # 把右侧 `(pixel >> 11) & 0x1F` 计算得到的值保存到 `high`，后面的判断或输出会继续使用它。
    high = (pixel >> 11) & 0x1F
    # 把右侧 `(pixel >> 5) & 0x3F` 计算得到的值保存到 `green`，后面的判断或输出会继续使用它。
    green = (pixel >> 5) & 0x3F
    # 把右侧 `pixel & 0x1F` 计算得到的值保存到 `low`，后面的判断或输出会继续使用它。
    low = pixel & 0x1F
    # 把 `high, green, low` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return high, green, low


# 定义函数 `_pack_rgb555`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _pack_rgb555(high: int, middle: int, low: int) -> int:
    # 执行这一条实际代码步骤：`"""把三个已经限制在 0..31 的通道重新装回 RGB555 整数。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """把三个已经限制在 0..31 的通道重新装回 RGB555 整数。"""
    # 把 `((high & 0x1F) << 10) | ((middle & 0x1F) << 5) | (low & 0x1F)` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return ((high & 0x1F) << 10) | ((middle & 0x1F) << 5) | (low & 0x1F)


# 定义函数 `_pack_rgb565`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _pack_rgb565(high: int, green: int, low: int) -> int:
    # 执行这一条实际代码步骤：`"""把 5/6/5 位三个通道重新装回 RGB565 整数。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """把 5/6/5 位三个通道重新装回 RGB565 整数。"""
    # 把 `((high & 0x1F) << 11) | ((green & 0x3F) << 5) | (low & 0x1F)` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return ((high & 0x1F) << 11) | ((green & 0x3F) << 5) | (low & 0x1F)


# 定义函数 `scale_rgb555_reference`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def scale_rgb555_reference(pixel: int, weight: int) -> int:
    """精确模拟原版 0x44BA1C 生成的 RGB555 LUT 的一个查询结果。

    原版不是每个像素现场做乘法，而是启动时先生成 17 个表：强度 c=0,2,...,32。
    渲染器再用 ``weight >> 1`` 选表，所以普通非负权重的最低位会被丢掉。例如 9 和 8
    会选到同一张表。每个通道先做 ``floor(channel*c/16)``，超过 31 就饱和到 31，
    最后还会清掉每个 5-bit 通道的最低位（机器码 AND 0x7BDE）。
    """
    # 先把 Python 整数限制成原游戏实际读取的 16 位像素，避免高位意外参与计算。
    # 执行一次赋值：把 `0x7FFF` 的结果放入左侧 `pixel &` 对应的变量/字段。
    pixel &= 0x7FFF
    # RGB555 的表号是 weight>>1；换回生成器的真实强度，就是把最低位清零。
    # 把右侧 `max(0, weight & ~1)` 计算得到的值保存到 `strength`，后面的判断或输出会继续使用它。
    strength = max(0, weight & ~1)
    # 磁盘正常参数只看到小正数；这里仍按生成器上限 32 截断，防止数组概念越界。
    # 把右侧 `min(strength, 32)` 计算得到的值保存到 `strength`，后面的判断或输出会继续使用它。
    strength = min(strength, 32)
    # 执行一次赋值：把 `_split_rgb555(pixel)` 的结果放入左侧 `high, middle, low` 对应的变量/字段。
    high, middle, low = _split_rgb555(pixel)
    # ``//`` 对非负整数正好就是机器公式中的向下取整除法。
    # 把右侧 `min((high * strength) // 16, 31)` 计算得到的值保存到 `high`，后面的判断或输出会继续使用它。
    high = min((high * strength) // 16, 31)
    # 把右侧 `min((middle * strength) // 16, 31)` 计算得到的值保存到 `middle`，后面的判断或输出会继续使用它。
    middle = min((middle * strength) // 16, 31)
    # 把右侧 `min((low * strength) // 16, 31)` 计算得到的值保存到 `low`，后面的判断或输出会继续使用它。
    low = min((low * strength) // 16, 31)
    # 0x7BDE 会把三个 5-bit 通道各自的 bit0 清零；这是原版 LUT 的量化特征。
    # 把 `_pack_rgb555(high, middle, low) & 0x7BDE` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return _pack_rgb555(high, middle, low) & 0x7BDE


# 定义函数 `scale_rgb565_reference`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def scale_rgb565_reference(pixel: int, weight: int) -> int:
    """精确模拟原版 0x44CCDD 生成的 RGB565 LUT 的一个查询结果。

    RGB565 路线生成 33 个表，强度 c=0..32 每个整数都有一张，因此不会像 RGB555
    那样把奇数权重向下量化成偶数。红/蓝饱和上限 31，绿色饱和上限 63；最后使用
    AND 0xF7DE 清掉三个通道的最低位。
    """
    # 执行一次赋值：把 `0xFFFF` 的结果放入左侧 `pixel &` 对应的变量/字段。
    pixel &= 0xFFFF
    # 把右侧 `min(max(weight, 0), 32)` 计算得到的值保存到 `strength`，后面的判断或输出会继续使用它。
    strength = min(max(weight, 0), 32)
    # 执行一次赋值：把 `_split_rgb565(pixel)` 的结果放入左侧 `high, green, low` 对应的变量/字段。
    high, green, low = _split_rgb565(pixel)
    # 把右侧 `min((high * strength) // 16, 31)` 计算得到的值保存到 `high`，后面的判断或输出会继续使用它。
    high = min((high * strength) // 16, 31)
    # 把右侧 `min((green * strength) // 16, 63)` 计算得到的值保存到 `green`，后面的判断或输出会继续使用它。
    green = min((green * strength) // 16, 63)
    # 把右侧 `min((low * strength) // 16, 31)` 计算得到的值保存到 `low`，后面的判断或输出会继续使用它。
    low = min((low * strength) // 16, 31)
    # 把 `_pack_rgb565(high, green, low) & 0xF7DE` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return _pack_rgb565(high, green, low) & 0xF7DE


# 定义函数 `sat_add_rgb555_reference`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sat_add_rgb555_reference(a: int, b: int) -> int:
    # 执行这一条实际代码步骤：`"""逐通道相加并饱和到 31；对应 RGB555 混合 helper 的进位修正语义。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """逐通道相加并饱和到 31；对应 RGB555 混合 helper 的进位修正语义。"""
    # 执行一次赋值：把 `_split_rgb555(a)` 的结果放入左侧 `ah, am, al` 对应的变量/字段。
    ah, am, al = _split_rgb555(a)
    # 执行一次赋值：把 `_split_rgb555(b)` 的结果放入左侧 `bh, bm, bl` 对应的变量/字段。
    bh, bm, bl = _split_rgb555(b)
    # 把 `_pack_rgb555(min(ah + bh, 31), min(am + bm, 31), min(al + bl, 31))` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return _pack_rgb555(min(ah + bh, 31), min(am + bm, 31), min(al + bl, 31))


# 定义函数 `sat_add_rgb565_reference`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sat_add_rgb565_reference(a: int, b: int) -> int:
    # 执行这一条实际代码步骤：`"""逐通道相加并分别饱和到 31/63/31。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """逐通道相加并分别饱和到 31/63/31。"""
    # 执行一次赋值：把 `_split_rgb565(a)` 的结果放入左侧 `ah, ag, al` 对应的变量/字段。
    ah, ag, al = _split_rgb565(a)
    # 执行一次赋值：把 `_split_rgb565(b)` 的结果放入左侧 `bh, bg, bl` 对应的变量/字段。
    bh, bg, bl = _split_rgb565(b)
    # 把 `_pack_rgb565(min(ah + bh, 31), min(ag + bg, 63), min(al + bl, 31))` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return _pack_rgb565(min(ah + bh, 31), min(ag + bg, 63), min(al + bl, 31))


# 定义函数 `render_semantics_dictionary`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def render_semantics_dictionary() -> Dict[str, Any]:
    """返回固化34可直接给兼容引擎实现的渲染模式参考词典。

    这里故意把“机器算法是否闭合”和“是否找到真实磁盘正样本”分成两列。
    例如 Mode30 的机器码已经足够还原公式，但当前 506 份唯一 SF2 中没有 Mode30；
    所以兼容引擎可以实现它，却不能反过来宣称已经知道作者把它用在哪种视觉效果上。
    """
    # 把右侧 `[` 计算得到的值保存到 `modes`，后面的判断或输出会继续使用它。
    modes = [
        # 执行一次赋值：把 `src；skip: 保留dst","磁盘正样本":True},` 的结果放入左侧 `{"模式":0,"名称":"NORMAL_COPY","机器算法":"透明RLE普通拷贝；+0x0B/+0x…` 对应的变量/字段。
        {"模式":0,"名称":"NORMAL_COPY","机器算法":"透明RLE普通拷贝；+0x0B/+0x0D/+0x0F均不参与。","闭式公式":"opaque: out=src；skip: 保留dst","磁盘正样本":True},
        # 执行一次赋值：把 `SatAdd(Scale(src,p1), Scale(dst,16-p1))","磁盘正样本":True},` 的结果放入左侧 `{"模式":1,"名称":"ALPHA_BLEND_16","机器算法":"源权重 p1，目标权重 16-p…` 对应的变量/字段。
        {"模式":1,"名称":"ALPHA_BLEND_16","机器算法":"源权重 p1，目标权重 16-p1；两个缩放结果逐通道饱和相加。","闭式公式":"out=SatAdd(Scale(src,p1), Scale(dst,16-p1))","磁盘正样本":True},
        # 执行一次赋值：把 `SatAdd(Scale(src,p1), Scale(dst,p2))","磁盘正样本":True},` 的结果放入左侧 `{"模式":2,"名称":"WEIGHTED_BLEND_16","机器算法":"源、目标使用独立权重 p1…` 对应的变量/字段。
        {"模式":2,"名称":"WEIGHTED_BLEND_16","机器算法":"源、目标使用独立权重 p1/p2；p3 不参与。","闭式公式":"out=SatAdd(Scale(src,p1), Scale(dst,p2))","磁盘正样本":True},
        # 执行一次赋值：把 `Scale(p3,p1); out=SatAdd(Scale(dst,p2),C)","磁盘正样本":True},` 的结果放入左侧 `{"模式":3,"名称":"MASKED_DESTINATION_TINT_BLEND","机器算法":"源…` 对应的变量/字段。
        {"模式":3,"名称":"MASKED_DESTINATION_TINT_BLEND","机器算法":"源RLE只决定覆盖位置；源literal像素值被跳过。常量色 p3 先按 p1 缩放，目标按 p2 缩放后逐通道饱和相加。","闭式公式":"C=Scale(p3,p1); out=SatAdd(Scale(dst,p2),C)","磁盘正样本":True},
        # 执行一次赋值：把 `Scale(p3,p1); out=SatAdd(Scale(src,p2),C)","磁盘正样本":True},` 的结果放入左侧 `{"模式":4,"名称":"WEIGHTED_SOURCE_PLUS_CONSTANT_COLOR","机器…` 对应的变量/字段。
        {"模式":4,"名称":"WEIGHTED_SOURCE_PLUS_CONSTANT_COLOR","机器算法":"常量色 p3 按 p1 缩放，源像素按 p2 缩放，然后逐通道饱和相加。","闭式公式":"C=Scale(p3,p1); out=SatAdd(Scale(src,p2),C)","磁盘正样本":True},
        # 执行一次赋值：把 `(~src) masked to active pixel format","磁盘正样本":False},` 的结果放入左侧 `{"模式":5,"名称":"INVERT_SOURCE_COPY","机器算法":"源像素逐位反相后按透明R…` 对应的变量/字段。
        {"模式":5,"名称":"INVERT_SOURCE_COPY","机器算法":"源像素逐位反相后按透明RLE写入。","闭式公式":"out=(~src) masked to active pixel format","磁盘正样本":False},
        # 执行一次赋值：把 `(~dst) masked to active pixel format","磁盘正样本":False},` 的结果放入左侧 `{"模式":6,"名称":"INVERT_DESTINATION_MASK","机器算法":"源RLE只作m…` 对应的变量/字段。
        {"模式":6,"名称":"INVERT_DESTINATION_MASK","机器算法":"源RLE只作mask；命中位置把目标像素逐位反相。","闭式公式":"out=(~dst) masked to active pixel format","磁盘正样本":False},
        # 执行一次赋值：把 `2+2*floor(channel5/4)；RGB565 R/B=1+floor(channel5/2), G=1+floor(channel6/4…` 的结果放入左侧 `{"模式":7,"名称":"LEGACY_LOOKUP_COMPOSITE_7","机器算法":"源像素每个…` 对应的变量/字段。
        {"模式":7,"名称":"LEGACY_LOOKUP_COMPOSITE_7","机器算法":"源像素每个颜色通道分别量化成1..16强度，再用完整目标像素索引对应LUT，只保留该输出通道并拼回。","闭式公式":"RGB555 strength=2+2*floor(channel5/4)；RGB565 R/B=1+floor(channel5/2), G=1+floor(channel6/4)；每通道取 Scale(dst,strength) 的对应通道","磁盘正样本":True},
        # 执行一次赋值：把 `2+(((31-max5)&0x1C)//2); RGB565 s=1+(((31-max5)&0x1E)//2); out=SatAdd(src_…` 的结果放入左侧 `{"模式":8,"名称":"LEGACY_BRIGHTEN_BY_SOURCE_MAX","机器算法":"先…` 对应的变量/字段。
        {"模式":8,"名称":"LEGACY_BRIGHTEN_BY_SOURCE_MAX","机器算法":"先保留清LSB后的源像素，再按源像素最大通道计算目标缩放强度，最后逐通道饱和相加。","闭式公式":"RGB555 s=2+(((31-max5)&0x1C)//2); RGB565 s=1+(((31-max5)&0x1E)//2); out=SatAdd(src_even,Scale(dst,s))","磁盘正样本":True},
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        {"模式":9,"名称":"FALLBACK_NOOP","机器算法":"原版0..30分派落入fallback/no-op。","闭式公式":"无专用处理器","磁盘正样本":False},
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        {"模式":10,"名称":"MASKED_DESTINATION_NEIGHBOR_AVERAGE_A","机器算法":"源只作mask，对目标画面的第一组方向邻域求平均。","闭式公式":"邻域地址/边界语义见既有规格；本轮未改名为现代滤镜。","磁盘正样本":True},
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        {"模式":11,"名称":"MASKED_DESTINATION_NEIGHBOR_AVERAGE_B","机器算法":"源只作mask，对目标画面的另一组方向邻域求平均。","闭式公式":"邻域地址/边界语义见既有规格。","磁盘正样本":False},
    ]
    # 12..29 全部没有专用分派；用循环生成可以避免手工复制十八行时写错模式号。
    # 开始循环 `mode in range(12, 30)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for mode in range(12, 30):
        # 把这一条新结果追加到列表 `modes`；不会覆盖前面已经收集的记录。
        modes.append({"模式":mode,"名称":"FALLBACK_NOOP","机器算法":"原版0..30分派落入fallback/no-op。","闭式公式":"无专用处理器","磁盘正样本":False})
    # 执行一次赋值：把 `((src & evenMask)+(p2 & evenMask)) & p1 & evenMask","磁盘正样本":False})` 的结果放入左侧 `modes.append({"模式":30,"名称":"SOURCE_ADD_CONSTANT_BITMAS…` 对应的变量/字段。
    modes.append({"模式":30,"名称":"SOURCE_ADD_CONSTANT_BITMASK","机器算法":"p2先按像素偶数位掩码；源像素也清通道LSB，然后做16位打包整数加法，再依次AND p1与像素格式偶数位掩码。不是逐通道饱和加法，跨通道进位属于原版行为。","闭式公式":"out=((src & evenMask)+(p2 & evenMask)) & p1 & evenMask","磁盘正样本":False})
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本": TOOL_VERSION,
        # 给当前结果字典的“SF2子层字段”字段填写 `{"mode":"+0x0A u8","p1":"+0x0B i16","p2":"+0x0D i16","p3":"+0x0F i16"…`，这样导出的 JSON/表格能保留这一项证据。
        "SF2子层字段": {"mode":"+0x0A u8","p1":"+0x0B i16","p2":"+0x0D i16","p3":"+0x0F i16"},
        # 给当前结果字典的“像素格式分流”字段填写 `{"对象+0x130等于0x10":"RGB565","其他已覆盖16-bit路径":"RGB555"},`，这样导出的 JSON/表格能保留这一项证据。
        "像素格式分流": {"对象+0x130等于0x10":"RGB565","其他已覆盖16-bit路径":"RGB555"},
        # 给当前结果字典的“RGB555_LUT”字段填写 `{"生成器":"0x0044BA1C","表基址":"0x0046F6C0","表数":17,"每表字节":0x10000,"生成强度":…`，这样导出的 JSON/表格能保留这一项证据。
        "RGB555_LUT": {"生成器":"0x0044BA1C","表基址":"0x0046F6C0","表数":17,"每表字节":0x10000,"生成强度":"0,2,...,32","最终掩码":"0x7BDE"},
        # 给当前结果字典的“RGB565_LUT”字段填写 `{"生成器":"0x0044CCDD","表基址":"0x0046F6C0","表数":33,"每表字节":0x20000,"生成强度":…`，这样导出的 JSON/表格能保留这一项证据。
        "RGB565_LUT": {"生成器":"0x0044CCDD","表基址":"0x0046F6C0","表数":33,"每表字节":0x20000,"生成强度":"0,1,...,32","最终掩码":"0xF7DE"},
        # 给当前结果字典的“模式”字段填写 `sorted(modes, key=lambda x: x["模式"]),`，这样导出的 JSON/表格能保留这一项证据。
        "模式": sorted(modes, key=lambda x: x["模式"]),
        # 给当前结果字典的“证据纪律”字段填写 `"闭式机器算法与磁盘用途分级记录；没有正样本的模式不得强命名作者业务效果。",`，这样导出的 JSON/表格能保留这一项证据。
        "证据纪律": "闭式机器算法与磁盘用途分级记录；没有正样本的模式不得强命名作者业务效果。",
    }


# 定义函数 `sf2_render_mode_corpus`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sf2_render_mode_corpus(roots: List[Path]) -> Dict[str, Any]:
    """按 SHA-256 去重统计所有 Section0 子层的 mode/p1/p2/p3。

    为什么要按哈希去重：同一资源可能同时存在于顶层目录和 DAT 容器提取目录。
    如果直接按路径计数，同一份二进制会被重复算多次，导致“某个模式很常见”的结论失真。
    """
    # 创建变量 `groups`（类型提示为 `Dict[str, Path]`），并把 `{}` 的结果保存进去供后续步骤使用。
    groups: Dict[str, Path] = {}
    # 把右侧 `0` 计算得到的值保存到 `path_count`，后面的判断或输出会继续使用它。
    path_count = 0
    # 第一步只建立“内容哈希 -> 一个代表路径”。setdefault 保证相同内容只保留第一次。
    # 开始循环 `root in roots`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for root in roots:
        # 检查条件 `not root.exists()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not root.exists():
            # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
            continue
        # 开始循环 `path in root.rglob("*")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for path in root.rglob("*"):
            # 检查条件 `not path.is_file() or path.suffix.lower() != ".sf2"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not path.is_file() or path.suffix.lower() != ".sf2":
                # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
                continue
            # 执行一次赋值：把 `1` 的结果放入左侧 `path_count +` 对应的变量/字段。
            path_count += 1
            # 把右侧 `path.read_bytes()` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
            raw = path.read_bytes()
            # 调用 `groups.setdefault` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            groups.setdefault(hashlib.sha256(raw).hexdigest(), path)

    # 创建变量 `mode_counts`（类型提示为 `Counter[int]`），并把 `Counter()` 的结果保存进去供后续步骤使用。
    mode_counts: Counter[int] = Counter()
    # params[mode][字段名] 各自再是一个 Counter，能保留“值出现多少次”，不只保留最小/最大。
    # 创建变量 `params`（类型提示为 `Dict[int, Dict[str, Counter[int]]]`），并把 `defaultdict(lambda: {"p1":Counter(), "p2":Counter(), "p3":Counter()})` 的结果保存进去供后续步骤使用。
    params: Dict[int, Dict[str, Counter[int]]] = defaultdict(lambda: {"p1":Counter(), "p2":Counter(), "p3":Counter()})
    # 创建变量 `mode_files`（类型提示为 `Dict[int, set[str]]`），并把 `defaultdict(set)` 的结果保存进去供后续步骤使用。
    mode_files: Dict[int, set[str]] = defaultdict(set)
    # 创建变量 `failures`（类型提示为 `List[Dict[str, str]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    failures: List[Dict[str, str]] = []
    # 把右侧 `0` 计算得到的值保存到 `layer_count`，后面的判断或输出会继续使用它。
    layer_count = 0

    # 开始循环 `sha, path in sorted(groups.items())`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for sha, path in sorted(groups.items()):
        # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
        try:
            # sf2_decompress 会同时处理普通 SF2 和 +0x0B==0x6F 的 zlib 包装。
            # 执行一次赋值：把 `sf2_decompress(path.read_bytes())` 的结果放入左侧 `data, _wrap` 对应的变量/字段。
            data, _wrap = sf2_decompress(path.read_bytes())
            # 把右侧 `sf2_sections(data)` 计算得到的值保存到 `sections`，后面的判断或输出会继续使用它。
            sections = sf2_sections(data)
            # Section0 每条动作记录在 +0x54 存子层数，子层数组从 +0x5E 开始，每项29字节。
            # 开始循环 `record_info in sections[0]["记录"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for record_info in sections[0]["记录"]:
                # 把右侧 `data[record_info["偏移"]:record_info["结束"]]` 计算得到的值保存到 `record`，后面的判断或输出会继续使用它。
                record = data[record_info["偏移"]:record_info["结束"]]
                # 检查条件 `len(record) < 0x5E`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if len(record) < 0x5E:
                    # 检测到不能继续的情况，主动抛出异常 `ValueError("Section0记录短于固定头0x5E")`，让上层明确知道数据或参数不符合要求。
                    raise ValueError("Section0记录短于固定头0x5E")
                # 把右侧 `u16(record, 0x54)` 计算得到的值保存到 `count`，后面的判断或输出会继续使用它。
                count = u16(record, 0x54)
                # 检查条件 `len(record) != 0x5E + 29 * count`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if len(record) != 0x5E + 29 * count:
                    # 检测到不能继续的情况，主动抛出异常 `ValueError("Section0记录大小不满足 0x5E + 29*N")`，让上层明确知道数据或参数不符合要求。
                    raise ValueError("Section0记录大小不满足 0x5E + 29*N")
                # 开始循环 `index in range(count)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                for index in range(count):
                    # 把右侧 `record[0x5E + index * 29:0x5E + (index + 1) * 29]` 计算得到的值保存到 `layer`，后面的判断或输出会继续使用它。
                    layer = record[0x5E + index * 29:0x5E + (index + 1) * 29]
                    # 把右侧 `layer[0x0A]` 计算得到的值保存到 `mode`，后面的判断或输出会继续使用它。
                    mode = layer[0x0A]
                    # 把右侧 `i16(layer, 0x0B)` 计算得到的值保存到 `p1`，后面的判断或输出会继续使用它。
                    p1 = i16(layer, 0x0B)
                    # 把右侧 `i16(layer, 0x0D)` 计算得到的值保存到 `p2`，后面的判断或输出会继续使用它。
                    p2 = i16(layer, 0x0D)
                    # 把右侧 `i16(layer, 0x0F)` 计算得到的值保存到 `p3`，后面的判断或输出会继续使用它。
                    p3 = i16(layer, 0x0F)
                    # 执行一次赋值：把 `1` 的结果放入左侧 `layer_count +` 对应的变量/字段。
                    layer_count += 1
                    # 执行一次赋值：把 `1` 的结果放入左侧 `mode_counts[mode] +` 对应的变量/字段。
                    mode_counts[mode] += 1
                    # 执行一次赋值：把 `1` 的结果放入左侧 `params[mode]["p1"][p1] +` 对应的变量/字段。
                    params[mode]["p1"][p1] += 1
                    # 执行一次赋值：把 `1` 的结果放入左侧 `params[mode]["p2"][p2] +` 对应的变量/字段。
                    params[mode]["p2"][p2] += 1
                    # 执行一次赋值：把 `1` 的结果放入左侧 `params[mode]["p3"][p3] +` 对应的变量/字段。
                    params[mode]["p3"][p3] += 1
                    # 执行这一条实际代码步骤：`mode_files[mode].add(sha)`；它与上下相邻语句共同完成当前函数的小任务。
                    mode_files[mode].add(sha)
        # 捕获 `Exception as exc` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
        except Exception as exc:
            # 失败资源不能静默丢掉，否则“全语料”会给人一种全部解析成功的错觉。
            # 把这一条新结果追加到列表 `failures`；不会覆盖前面已经收集的记录。
            failures.append({"SHA256":sha,"代表文件":str(path),"错误":str(exc)})

    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows = []
    # 开始循环 `mode in sorted(mode_counts)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for mode in sorted(mode_counts):
        # 把右侧 `{"模式":mode,"子层数":mode_counts[mode],"出现于唯一SF2数":len(mode_files[mode])}` 计算得到的值保存到 `row`，后面的判断或输出会继续使用它。
        row = {"模式":mode,"子层数":mode_counts[mode],"出现于唯一SF2数":len(mode_files[mode])}
        # 开始循环 `name in ("p1","p2","p3")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for name in ("p1","p2","p3"):
            # 执行一次赋值：把 `{str(k):v for k,v in sorted(params[mode][name].items())}` 的结果放入左侧 `row[name+"计数"]` 对应的变量/字段。
            row[name+"计数"] = {str(k):v for k,v in sorted(params[mode][name].items())}
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append(row)
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本":TOOL_VERSION,
        # 给当前结果字典的“资源根”字段填写 `[str(x) for x in roots],`，这样导出的 JSON/表格能保留这一项证据。
        "资源根":[str(x) for x in roots],
        # 给当前结果字典的“输入SF2路径数”字段填写 `path_count,`，这样导出的 JSON/表格能保留这一项证据。
        "输入SF2路径数":path_count,
        # 给当前结果字典的“唯一SHA256文件数”字段填写 `len(groups),`，这样导出的 JSON/表格能保留这一项证据。
        "唯一SHA256文件数":len(groups),
        # 给当前结果字典的“成功解析唯一SF2数”字段填写 `len(groups)-len(failures),`，这样导出的 JSON/表格能保留这一项证据。
        "成功解析唯一SF2数":len(groups)-len(failures),
        # 给当前结果字典的“解析失败唯一SF2数”字段填写 `len(failures),`，这样导出的 JSON/表格能保留这一项证据。
        "解析失败唯一SF2数":len(failures),
        # 给当前结果字典的“Section0子层总数”字段填写 `layer_count,`，这样导出的 JSON/表格能保留这一项证据。
        "Section0子层总数":layer_count,
        # 给当前结果字典的“渲染模式分布”字段填写 `{str(k):v for k,v in sorted(mode_counts.items())},`，这样导出的 JSON/表格能保留这一项证据。
        "渲染模式分布":{str(k):v for k,v in sorted(mode_counts.items())},
        # 给当前结果字典的“模式参数”字段填写 `rows,`，这样导出的 JSON/表格能保留这一项证据。
        "模式参数":rows,
        # 给当前结果字典的“未见但原版有专用处理器的模式”字段填写 `[m for m in (5,6,11,30) if m not in mode_counts],`，这样导出的 JSON/表格能保留这一项证据。
        "未见但原版有专用处理器的模式":[m for m in (5,6,11,30) if m not in mode_counts],
        # 给当前结果字典的“解析失败”字段填写 `failures,`，这样导出的 JSON/表格能保留这一项证据。
        "解析失败":failures,
        # 给当前结果字典的“参数解释边界”字段填写 `"只能解释某模式机器码实际读取的字段。其他字段即使磁盘非零也可能是惯性/残留值，禁止跨模式强解释。",`，这样导出的 JSON/表格能保留这一项证据。
        "参数解释边界":"只能解释某模式机器码实际读取的字段。其他字段即使磁盘非零也可能是惯性/残留值，禁止跨模式强解释。",
    }


# 定义函数 `verify_sf2_render_lut_rpg`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_sf2_render_lut_rpg(path: Path) -> Dict[str, Any]:
    """固化34：验证像素格式分流、LUT生成器、参数读取和代表性模式处理器。

    这个函数故意把“整文件 SHA 是否与历史 Oracle 完全相同”和“本轮覆盖机器码是否相同”
    分成两个结果。这样收到另一个原始 EXE 时，不会因为 PE 时间戳/资源等无关差异就错误宣布
    所有逆向结论失效，也不会反过来因为几十条机器断言通过就声称整个 EXE 完全相同。
    """
    # 把右侧 `_PEImage(path)` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe = _PEImage(path)
    # 把右侧 `hashlib.sha256(pe.data).hexdigest()` 计算得到的值保存到 `sha`，后面的判断或输出会继续使用它。
    sha = hashlib.sha256(pe.data).hexdigest()
    # 创建变量 `checks`（类型提示为 `List[Dict[str, Any]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    checks: List[Dict[str, Any]] = []
    # 把右侧 `lambda va, hx, note: checks.append(_map_check_bytes(pe, va, hx, note))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B = lambda va, hx, note: checks.append(_map_check_bytes(pe, va, hx, note))
    # 把右侧 `lambda va, target, note: checks.append(_map_check_call(pe, va, target, not…` 计算得到的值保存到 `C`，后面的判断或输出会继续使用它。
    C = lambda va, target, note: checks.append(_map_check_call(pe, va, target, note))

    # ---- SF2 子层的四个关键字段 ----
    # 执行这一条实际代码步骤：`B(0x00429381,'0F BF 48 0F','调用渲染器前以signed word读取子层+0x0F，即p3')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00429381,'0F BF 48 0F','调用渲染器前以signed word读取子层+0x0F，即p3')
    # 执行这一条实际代码步骤：`B(0x00429385,'0F BF 50 0D','调用渲染器前以signed word读取子层+0x0D，即p2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00429385,'0F BF 50 0D','调用渲染器前以signed word读取子层+0x0D，即p2')
    # 执行这一条实际代码步骤：`B(0x0042938B,'0F BF 48 0B','调用渲染器前以signed word读取子层+0x0B，即p1')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0042938B,'0F BF 48 0B','调用渲染器前以signed word读取子层+0x0B，即p1')
    # 执行这一条实际代码步骤：`B(0x00429392,'8A 50 0A','调用渲染器前读取子层+0x0A，即mode')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00429392,'8A 50 0A','调用渲染器前读取子层+0x0A，即mode')

    # ---- 显示像素格式决定 RGB565 / RGB555 查表生成器 ----
    # 执行这一条实际代码步骤：`B(0x00406165,'39 7A 0C','显示初始化比较16-bit像素格式位数')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00406165,'39 7A 0C','显示初始化比较16-bit像素格式位数')
    # 执行这一条实际代码步骤：`B(0x0040616A,'81 BD E4 FE FF FF E0 07 00 00','16-bit格式继续比较green mask 0x07E0，识别RGB565')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040616A,'81 BD E4 FE FF FF E0 07 00 00','16-bit格式继续比较green mask 0x07E0，识别RGB565')
    # 执行这一条实际代码步骤：`C(0x0040618C,0x0044CCDD,'RGB565分支调用0x44CCDD生成565 LUT')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040618C,0x0044CCDD,'RGB565分支调用0x44CCDD生成565 LUT')
    # 执行这一条实际代码步骤：`C(0x00406193,0x0044BA1C,'非565的16-bit分支调用0x44BA1C生成555 LUT')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00406193,0x0044BA1C,'非565的16-bit分支调用0x44BA1C生成555 LUT')

    # ---- RGB555 LUT：17个bank，强度每次+2，最终清每通道LSB ----
    # 执行这一条实际代码步骤：`B(0x0044BA23,'BF C0 F6 46 00','RGB555 LUT基址0x46F6C0')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044BA23,'BF C0 F6 46 00','RGB555 LUT基址0x46F6C0')
    # 执行这一条实际代码步骤：`B(0x0044BA2A,'25 00 7C 00 00 C1 E8 0A F7 E1 C1 E8 04','RGB555高5位提取后乘强度并除16')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044BA2A,'25 00 7C 00 00 C1 E8 0A F7 E1 C1 E8 04','RGB555高5位提取后乘强度并除16')
    # 执行这一条实际代码步骤：`B(0x0044BA86,'25 DE 7B 00 00','RGB555生成结果AND 0x7BDE清三个通道LSB')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044BA86,'25 DE 7B 00 00','RGB555生成结果AND 0x7BDE清三个通道LSB')
    # 执行这一条实际代码步骤：`B(0x0044BA98,'83 C1 02 83 F9 20','RGB555强度每轮+2且上界32')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044BA98,'83 C1 02 83 F9 20','RGB555强度每轮+2且上界32')

    # ---- RGB565 LUT：33个bank，强度每次+1，最终清每通道LSB ----
    # 执行这一条实际代码步骤：`B(0x0044CCE4,'BF C0 F6 46 00','RGB565 LUT基址同为0x46F6C0')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044CCE4,'BF C0 F6 46 00','RGB565 LUT基址同为0x46F6C0')
    # 执行这一条实际代码步骤：`B(0x0044CCEB,'25 00 F8 00 00 C1 E8 0B F7 E1 C1 E8 04','RGB565高5位提取后乘强度并除16')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044CCEB,'25 00 F8 00 00 C1 E8 0B F7 E1 C1 E8 04','RGB565高5位提取后乘强度并除16')
    # 执行这一条实际代码步骤：`B(0x0044CD47,'25 DE F7 00 00','RGB565生成结果AND 0xF7DE清三个通道LSB')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044CD47,'25 DE F7 00 00','RGB565生成结果AND 0xF7DE清三个通道LSB')
    # 执行这一条实际代码步骤：`B(0x0044CD59,'41 83 F9 20','RGB565强度每轮+1且上界32')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044CD59,'41 83 F9 20','RGB565强度每轮+1且上界32')

    # ---- Mode1/2 共用按像素格式分流的权重混合 helper ----
    # 执行这一条实际代码步骤：`C(0x0042971B,0x0044CDF2,'Mode1 RGB565调用共同权重混合helper')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0042971B,0x0044CDF2,'Mode1 RGB565调用共同权重混合helper')
    # 执行这一条实际代码步骤：`C(0x00429740,0x0044BAA4,'Mode1 RGB555调用共同权重混合helper')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00429740,0x0044BAA4,'Mode1 RGB555调用共同权重混合helper')
    # 执行这一条实际代码步骤：`C(0x00429778,0x0044CDF2,'Mode2 RGB565调用共同权重混合helper')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00429778,0x0044CDF2,'Mode2 RGB565调用共同权重混合helper')
    # 执行这一条实际代码步骤：`C(0x004297A3,0x0044BAA4,'Mode2 RGB555调用共同权重混合helper')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x004297A3,0x0044BAA4,'Mode2 RGB555调用共同权重混合helper')

    # ---- Mode3/4/7/8/30 的565/555成对处理器 ----
    # 开始循环 `call_va, target, note in [`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for call_va, target, note in [
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        (0x004297E2,0x0044D051,'Mode3 RGB565处理器'),(0x004297EF,0x0044BD0F,'Mode3 RGB555处理器'),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        (0x0042982E,0x0044CF2B,'Mode4 RGB565处理器'),(0x0042983B,0x0044BBE3,'Mode4 RGB555处理器'),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        (0x004298F7,0x0044D28B,'Mode7 RGB565处理器'),(0x00429913,0x0044BF4F,'Mode7 RGB555处理器'),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        (0x0042993E,0x0044D3BD,'Mode8 RGB565处理器'),(0x0042995A,0x0044C085,'Mode8 RGB555处理器'),
        # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
        (0x0042998F,0x0044D8D8,'Mode30 RGB565处理器'),(0x004299BA,0x0044C5A0,'Mode30 RGB555处理器')]:
        # 执行这一条实际代码步骤：`C(call_va,target,note)`；它与上下相邻语句共同完成当前函数的小任务。
        C(call_va,target,note)

    # Mode3 的555/565入口直接展示了“常量先查表”和两种bank尺寸差异。
    # 执行这一条实际代码步骤：`B(0x0044BD18,'BB C0 F6 46 00 B8 00 00 01 00 D1 6D 2C','Mode3 RGB555：每bank 0x10000，p1先>>1选择bank')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044BD18,'BB C0 F6 46 00 B8 00 00 01 00 D1 6D 2C','Mode3 RGB555：每bank 0x10000，p1先>>1选择bank')
    # 执行这一条实际代码步骤：`B(0x0044D05A,'BB C0 F6 46 00 B8 00 00 02 00 F7 65 2C','Mode3 RGB565：每bank 0x20000，p1直接选择bank')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044D05A,'BB C0 F6 46 00 B8 00 00 02 00 F7 65 2C','Mode3 RGB565：每bank 0x20000，p1直接选择bank')
    # Mode30 的关键五条指令证明它是“打包整数加法+bitmask”，不是逐通道饱和加法。
    # 执行这一条实际代码步骤：`B(0x0044C60A,'25 DE 7B 00 00 03 C3 23 45 28 25 DE 7B 00 00','Mode30 RGB555：清LSB、加常量、AND p1、再清LSB')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044C60A,'25 DE 7B 00 00 03 C3 23 45 28 25 DE 7B 00 00','Mode30 RGB555：清LSB、加常量、AND p1、再清LSB')
    # 执行这一条实际代码步骤：`B(0x0044D91D,'25 DE F7 00 00 03 C3 23 45 28 25 DE F7 00 00','Mode30 RGB565：同一打包整数加法/bitmask公式')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044D91D,'25 DE F7 00 00 03 C3 23 45 28 25 DE F7 00 00','Mode30 RGB565：同一打包整数加法/bitmask公式')

    # 把右侧 `all(item.get('status') == 'PASS' for item in checks)` 计算得到的值保存到 `covered_pass`，后面的判断或输出会继续使用它。
    covered_pass = all(item.get('status') == 'PASS' for item in checks)
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本':TOOL_VERSION,
        # 给当前结果字典的“文件”字段填写 `str(path),`，这样导出的 JSON/表格能保留这一项证据。
        '文件':str(path),
        # 给当前结果字典的“SHA256”字段填写 `sha,`，这样导出的 JSON/表格能保留这一项证据。
        'SHA256':sha,
        # 给当前结果字典的“历史Oracle_SHA256”字段填写 `MAP_TAIL_ORACLE_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        '历史Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,
        # 给当前结果字典的“历史Oracle哈希匹配”字段填写 `sha==MAP_TAIL_ORACLE_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        '历史Oracle哈希匹配':sha==MAP_TAIL_ORACLE_SHA256,
        # 给当前结果字典的“固化34收到的原始EXE候选_SHA256”字段填写 `SOLID34_COMPATIBLE_RPG_ORG_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        '固化34收到的原始EXE候选_SHA256':SOLID34_COMPATIBLE_RPG_ORG_SHA256,
        # 给当前结果字典的“是否为固化34收到的候选”字段填写 `sha==SOLID34_COMPATIBLE_RPG_ORG_SHA256,`，这样导出的 JSON/表格能保留这一项证据。
        '是否为固化34收到的候选':sha==SOLID34_COMPATIBLE_RPG_ORG_SHA256,
        # 给当前结果字典的“关键机器断言”字段填写 `checks,`，这样导出的 JSON/表格能保留这一项证据。
        '关键机器断言':checks,
        # 给当前结果字典的“关键机器断言数量”字段填写 `len(checks),`，这样导出的 JSON/表格能保留这一项证据。
        '关键机器断言数量':len(checks),
        # 给当前结果字典的“关键机器断言全部通过”字段填写 `covered_pass,`，这样导出的 JSON/表格能保留这一项证据。
        '关键机器断言全部通过':covered_pass,
        # 给当前结果字典的“判定”字段填写 `('历史Oracle整文件一致' if sha==MAP_TAIL_ORACLE_SHA256 else ('已覆盖代码区域语义等价候选'…`，这样导出的 JSON/表格能保留这一项证据。
        '判定':('历史Oracle整文件一致' if sha==MAP_TAIL_ORACLE_SHA256 else ('已覆盖代码区域语义等价候选' if covered_pass else '覆盖代码存在差异')),
        # 给当前结果字典的“边界”字段填写 `'机器断言全部通过只证明本工具覆盖的地址/调用关系一致；整文件SHA不同仍必须保留，不能据此宣称PE全部字节相同。',`，这样导出的 JSON/表格能保留这一项证据。
        '边界':'机器断言全部通过只证明本工具覆盖的地址/调用关系一致；整文件SHA不同仍必须保留，不能据此宣称PE全部字节相同。',
    }

# 定义函数 `property_callsite_dictionary`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def property_callsite_dictionary() -> Dict[str,Any]:
    # 把 `{"工具版本":TOOL_VERSION,"Getter":"0x0042A1A0","说明":"返回 Section0Record[0x24 + slot*4]；无有效资源时返…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"工具版本":TOOL_VERSION,"Getter":"0x0042A1A0","说明":"返回 Section0Record[0x24 + slot*4]；无有效资源时返回0。slot0/4/5/6/7/2/8 在0x0043F520成组参与effect/spatial-layout元数据处理；slot1在另一子系统表现为小枚举；slot3固定调用点已确认。仍禁止为各slot强起作者业务名。","调用点":SF2_PROPERTY_CALLSITES}


# 定义函数 `sf2_property_domains`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sf2_property_domains(roots: List[Path], rpg: Optional[Path]=None) -> Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""固化31：统计真实Section0属性槽，不把缺样本误写成格式约束。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """固化31：统计真实Section0属性槽，不把缺样本误写成格式约束。"""
    # 创建变量 `groups`（类型提示为 `Dict[str,Path]`），并把 `{}; input_count=0` 的结果保存进去供后续步骤使用。
    groups: Dict[str,Path] = {}; input_count=0
    # 开始循环 `root in roots`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for root in roots:
        # 开始循环 `p in root.rglob("*")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for p in root.rglob("*"):
            # 检查条件 `not p.is_file() or p.suffix.lower() != ".sf2"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if not p.is_file() or p.suffix.lower() != ".sf2": continue
            # 执行一次赋值：把 `1; raw=p.read_bytes(); groups.setdefault(hashlib.sha256(raw).hexdigest(),p)` 的结果放入左侧 `input_count +` 对应的变量/字段。
            input_count += 1; raw=p.read_bytes(); groups.setdefault(hashlib.sha256(raw).hexdigest(),p)
    # 把右侧 `[Counter() for _ in range(12)]; records=0; failures=[]; examples=[]` 计算得到的值保存到 `domains`，后面的判断或输出会继续使用它。
    domains=[Counter() for _ in range(12)]; records=0; failures=[]; examples=[]
    # 开始循环 `sha,p in sorted(groups.items())`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for sha,p in sorted(groups.items()):
        # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
        try:
            # 执行一次赋值：把 `sf2_decompress(p.read_bytes())` 的结果放入左侧 `b,_` 对应的变量/字段。
            b,_=sf2_decompress(p.read_bytes())
            # 开始循环 `rr in sf2_sections(b)[0]["记录"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for rr in sf2_sections(b)[0]["记录"]:
                # 把右侧 `b[rr["偏移"]:rr["结束"]]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
                rec=b[rr["偏移"]:rr["结束"]]
                # 检查条件 `len(rec)<0x54`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if len(rec)<0x54: raise ValueError("Section0记录短于属性区末端0x54")
                # 把右侧 `struct.unpack_from("<12I",rec,0x24); records+=1` 计算得到的值保存到 `vals`，后面的判断或输出会继续使用它。
                vals=struct.unpack_from("<12I",rec,0x24); records+=1
                # 开始循环 `i,v in enumerate(vals)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                for i,v in enumerate(vals): domains[i][v]+=1
                # 检查条件 `any(vals): examples.append({"文件":str(p),"Section0索引":rr["索引"],"u32":list(vals),"i32"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if any(vals): examples.append({"文件":str(p),"Section0索引":rr["索引"],"u32":list(vals),"i32":[v if v<0x80000000 else v-0x100000000 for v in vals]})
        # 捕获 `Exception as e: failures.append({"文件":str(p),"错误"` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
        except Exception as e: failures.append({"文件":str(p),"错误":str(e)})
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows=[]
    # 开始循环 `i,c in enumerate(domains)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i,c in enumerate(domains):
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append({"槽":i,"不同u32值数":len(c),"u32计数":{str(k):v for k,v in sorted(c.items())},"i32取值":[k if k<0x80000000 else k-0x100000000 for k in sorted(c)]})
    # 把右侧 `[]` 计算得到的值保存到 `static`，后面的判断或输出会继续使用它。
    static=[]
    # 检查条件 `rpg`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if rpg:
        # 把右侧 `_PEImage(rpg); sha=hashlib.sha256(pe.data).hexdigest()` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
        pe=_PEImage(rpg); sha=hashlib.sha256(pe.data).hexdigest()
        # 把右侧 `[(0x43F53A,"E8 61 AC FE FF","slot0调用Getter"),(0x43F558,"E8 43 AC FE FF","s…` 计算得到的值保存到 `checks`，后面的判断或输出会继续使用它。
        checks=[(0x43F53A,"E8 61 AC FE FF","slot0调用Getter"),(0x43F558,"E8 43 AC FE FF","slot4调用Getter"),(0x43F567,"E8 34 AC FE FF","slot5调用Getter"),(0x43F584,"E8 17 AC FE FF","slot6调用Getter"),(0x43F595,"E8 06 AC FE FF","slot7调用Getter"),(0x43F5A4,"E8 F7 AB FE FF","slot2调用Getter"),(0x43F5B5,"E8 E6 AB FE FF","slot8调用Getter"),(0x43F5C2,"83 FF 09","slot4上界9"),(0x43F572,"7F 08 C7 44 24 10 01 00 00 00","slot5非正值回退1"),(0x43F69A,"85 FF 74 09 83 F8 05","slot6非零且slot4非零时只接受值5"),(0x43F79B,"83 7C 24 24 01","slot8仅值1触发末端分支")]
        # 开始循环 `va,h,n in checks`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for va,h,n in checks:
            # 把右侧 `bytes.fromhex(h); got=pe.data[pe.va_to_offset(va):pe.va_to_offset(va)+len(…` 计算得到的值保存到 `exp`，后面的判断或输出会继续使用它。
            exp=bytes.fromhex(h); got=pe.data[pe.va_to_offset(va):pe.va_to_offset(va)+len(exp)]
            # 把这一条新结果追加到列表 `static`；不会覆盖前面已经收集的记录。
            static.append({"检查":n,"VA":f"0x{va:08X}","status":"PASS" if got==exp else "FAIL"})
        # 调用 `static.insert` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        static.insert(0,{"检查":"RPG.exe Oracle SHA256","status":"PASS" if sha==MAP_TAIL_ORACLE_SHA256 else "FAIL","实际":sha})
    # 把 `{"工具版本":TOOL_VERSION,"资源根":[str(x) for x in roots],"输入SF2路径数":input_count,"唯一SHA256文件数":l…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"工具版本":TOOL_VERSION,"资源根":[str(x) for x in roots],"输入SF2路径数":input_count,"唯一SHA256文件数":len(groups),"Section0记录数":records,"槽取值域":rows,"非零记录":examples,"解析失败":failures,"RPG静态检查":static,"全部通过":not failures and all(x["status"]=="PASS" for x in static),"消费链结论":{"slot0":"非零时调用全局对象0x426F40(2)","slot4":"0保持默认；非零合法域按1..9三列网格派生±800/±600偏移，越界回退5","slot5":"作为0x426A80数量/倍率参数；<=0回退1","slot6":"1..5分派全局效果/定位操作；与slot4组合时非5值被抑制","slot7":"1/2/3选择0x426A80第三坐标320/0/320及附加标志","slot2":"正值送入0x43F210，负值送0；0不调用","slot8":"仅值1触发末端全局清理与零坐标0x426A80调用"},"证据边界":"取值域只描述当前输入语料；尤其slot8..11全零不能推出格式保留/必须为零。槽名保持编号，不提升为作者术语。"}


# 定义函数 `verify_sf2_anchor_routes`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_sf2_anchor_routes(path: Path) -> Dict[str,Any]:
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 定义函数 `B`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def B(va:int,h:str,n:str):
        # 把右侧 `bytes.fromhex(h); off=pe.va_to_offset(va); got=pe.data[off:off+len(exp)]` 计算得到的值保存到 `exp`，后面的判断或输出会继续使用它。
        exp=bytes.fromhex(h); off=pe.va_to_offset(va); got=pe.data[off:off+len(exp)]
        # 把这一条新结果追加到列表 `checks`；不会覆盖前面已经收集的记录。
        checks.append({"检查":n,"VA":f"0x{va:08X}","status":"PASS" if got==exp else "FAIL"})
    # 开始循环 `va,h,n in [`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for va,h,n in [
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x420444,"6A 09","先读Section0 slot9"),(0x420462,"6A 08","随后读Section0 slot8"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x420472,"83 FF 01","slot9值1进入主目标锚点分支"),(0x420477,"8B 86 A0 07 00 00","读取FightRole+0x7A0主目标槽"),
      # 执行一次赋值：把 `1"),` 的结果放入左侧 `(0x42048F,"05 E8 03 00 00","主目标派生位置加1000"),(0x420494,"…` 对应的变量/字段。
      (0x42048F,"05 E8 03 00 00","主目标派生位置加1000"),(0x420494,"57","主目标分支传flag=1"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x420497,"83 FF 02","slot9值2进入自身锚点条件"),(0x42049C,"83 F8 01","slot8值1进入自身锚点条件"),
      # 执行一次赋值：把 `0"),` 的结果放入左侧 `(0x4204A1,"8B 0E","自身分支读取当前FightRole索引"),(0x4204B5,"6A…` 对应的变量/字段。
      (0x4204A1,"8B 0E","自身分支读取当前FightRole索引"),(0x4204B5,"6A 00","自身分支传flag=0"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x4204C1,"E8 1A D2 01 00","汇合调用0x43D6E0"),(0x43D6E9,"8B B1 44 02 00 00","注册器取得逐槽flag数组"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x43D6F1,"88 04 16","按槽写flag"),(0x43D6F7,"8B 81 48 02 00 00","flag非零时取得固定位置数组"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x43D701,"89 0C 90","flag非零时写固定位置"),(0x43D8EC,"8B 8E 44 02 00 00","更新器读取flag"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x43D8F8,"8B 96 48 02 00 00","flag非零读取固定位置"),(0x43D90C,"8B 96 4C 02 00 00","flag为零读取动画对象"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x442CDB,"E8 60 D7 FD FF","唯一直接上层调用"),(0x442DAF,"81 C6 E4 0D 00 00","逐FightRole前进0xDE4"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x442DB5,"83 FF 10","上层循环上界16")]: B(va,h,n)
    # 调用 `checks.insert` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    checks.insert(0,{"检查":"RPG.exe Oracle SHA256","status":"PASS" if sha==MAP_TAIL_ORACLE_SHA256 else "FAIL","实际":sha})
    # 把 `{"工具版本":TOOL_VERSION,"文件":str(path),"SHA256":sha,"关键验证":checks,"关键验证数量":len(checks),"关键验证…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"工具版本":TOOL_VERSION,"文件":str(path),"SHA256":sha,"关键验证":checks,"关键验证数量":len(checks),"关键验证全部通过":all(x["status"]=="PASS" for x in checks),"闭合结论":{"slot9=1":"以FightRole+0x7A0主目标槽的动画派生位置+1000为固定锚点，登记当前role槽并置flag=1","slot9=2或slot8=1":"以当前FightRole自身动画派生位置登记，但flag=0；后续从动画对象实时重算位置","其余":"不执行该锚点登记，函数其余逻辑照常运行","调用时机":"0x442CDB是唯一E8直接caller，位于Battle Manager最多16个FightRole的逐槽更新循环"},"实现约束":"必须区分固定位置快照(flag=1)与动画对象实时派生(flag=0)，不能把slot8/9简化成同一布尔位。","证据边界":"当前77份去重SF2的1427条Section0记录中slot8/9均为0；非零语义来自同版EXE静态分支，仍缺实际磁盘正样本与动态画面命名证据。"}




# 把右侧 `"b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"` 计算得到的值保存到 `MAP_TAIL_ORACLE_SHA256`，后面的判断或输出会继续使用它。
MAP_TAIL_ORACLE_SHA256 = "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"

# 定义类 `_PEImage`，把一组彼此相关的数据和操作整理在同一个类型中。
class _PEImage:
    # 定义函数 `__init__`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def __init__(self,path:Path):
        # 把右侧 `Path(path); self.data=self.path.read_bytes()` 计算得到的值保存到 `self.path`，后面的判断或输出会继续使用它。
        self.path=Path(path); self.data=self.path.read_bytes()
        # 检查条件 `self.data[:2]!=b"MZ" or len(self.data)<0x40`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if self.data[:2]!=b"MZ" or len(self.data)<0x40: raise ValueError("不是有效 MZ/PE 文件")
        # 把右侧 `struct.unpack_from("<I",self.data,0x3C)[0]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
        pe=struct.unpack_from("<I",self.data,0x3C)[0]
        # 检查条件 `self.data[pe:pe+4]!=b"PE\0\0"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if self.data[pe:pe+4]!=b"PE\0\0": raise ValueError("PE 签名无效")
        # 把右侧 `pe+4` 计算得到的值保存到 `coff`，后面的判断或输出会继续使用它。
        coff=pe+4
        # 执行一次赋值：把 `struct.unpack_from("<HHIIIHH",self.data,coff)` 的结果放入左侧 `_,nsec,_,_,_,optsz,_` 对应的变量/字段。
        _,nsec,_,_,_,optsz,_=struct.unpack_from("<HHIIIHH",self.data,coff)
        # 把右侧 `coff+20; magic=struct.unpack_from("<H",self.data,opt)[0]` 计算得到的值保存到 `opt`，后面的判断或输出会继续使用它。
        opt=coff+20; magic=struct.unpack_from("<H",self.data,opt)[0]
        # 检查条件 `magic==0x10B`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if magic==0x10B: self.image_base=struct.unpack_from("<I",self.data,opt+28)[0]
        # 前一个条件没有成立时，再检查 `magic==0x20B`；成立才执行这一分支。
        elif magic==0x20B: self.image_base=struct.unpack_from("<Q",self.data,opt+24)[0]
        # 前面的条件分支都没有命中时，执行这个兜底分支。
        else: raise ValueError(f"不支持的 PE 类型 0x{magic:04X}")
        # 把右侧 `opt+optsz; self.sections=[]` 计算得到的值保存到 `tab`，后面的判断或输出会继续使用它。
        tab=opt+optsz; self.sections=[]
        # 开始循环 `i in range(nsec)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i in range(nsec):
            # 把右侧 `tab+i*40; name=self.data[off:off+8].split(b"\0",1)[0].decode("ascii","repl…` 计算得到的值保存到 `off`，后面的判断或输出会继续使用它。
            off=tab+i*40; name=self.data[off:off+8].split(b"\0",1)[0].decode("ascii","replace")
            # 执行一次赋值：把 `struct.unpack_from("<IIII",self.data,off+8)` 的结果放入左侧 `vs,va,rs,rp` 对应的变量/字段。
            vs,va,rs,rp=struct.unpack_from("<IIII",self.data,off+8)
            # 把这一条新结果追加到列表 `sections`；不会覆盖前面已经收集的记录。
            self.sections.append((name,vs,va,rs,rp))
    # 定义函数 `va_to_offset`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def va_to_offset(self,va:int)->int:
        # 把右侧 `va-self.image_base` 计算得到的值保存到 `rva`，后面的判断或输出会继续使用它。
        rva=va-self.image_base
        # 开始循环 `name,vs,sva,rs,rp in self.sections`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for name,vs,sva,rs,rp in self.sections:
            # 检查条件 `sva<=rva<sva+max(vs,rs)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if sva<=rva<sva+max(vs,rs):
                # 把右侧 `rva-sva` 计算得到的值保存到 `d`，后面的判断或输出会继续使用它。
                d=rva-sva
                # 检查条件 `d>=rs: raise ValueError(f"VA 0x{va`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if d>=rs: raise ValueError(f"VA 0x{va:X} 位于未落盘虚拟区")
                # 把 `rp+d` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
                return rp+d
        # 检测到不能继续的情况，主动抛出异常 `ValueError(f"VA 0x{va:X} 无法映射")`，让上层明确知道数据或参数不符合要求。
        raise ValueError(f"VA 0x{va:X} 无法映射")
    # 定义函数 `read_va`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def read_va(self,va:int,n:int)->bytes:
        # 把右侧 `self.va_to_offset(va); out=self.data[off:off+n]` 计算得到的值保存到 `off`，后面的判断或输出会继续使用它。
        off=self.va_to_offset(va); out=self.data[off:off+n]
        # 检查条件 `len(out)!=n`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(out)!=n: raise ValueError("读取不足")
        # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return out
    # 定义函数 `section`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def section(self,name:str)->Tuple[int,bytes]:
        # 开始循环 `sec_name,vs,sva,rs,rp in self.sections`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for sec_name,vs,sva,rs,rp in self.sections:
            # 检查条件 `sec_name==name`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if sec_name==name:
                # 把 `self.image_base+sva,self.data[rp:rp+rs]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
                return self.image_base+sva,self.data[rp:rp+rs]
        # 检测到不能继续的情况，主动抛出异常 `ValueError(f"缺少节：{name}")`，让上层明确知道数据或参数不符合要求。
        raise ValueError(f"缺少节：{name}")
    # 定义函数 `call_target`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def call_target(self,va:int)->int:
        # 把右侧 `self.read_va(va,5)` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
        b=self.read_va(va,5)
        # 检查条件 `b[0]!=0xE8: raise ValueError(f"0x{va`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if b[0]!=0xE8: raise ValueError(f"0x{va:X} 不是 E8 CALL")
        # 把右侧 `struct.unpack_from("<i",b,1)[0]` 计算得到的值保存到 `rel`，后面的判断或输出会继续使用它。
        rel=struct.unpack_from("<i",b,1)[0]
        # 把 `(va+5+rel)&0xffffffff` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return (va+5+rel)&0xffffffff

# 定义函数 `_map_check_bytes`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _map_check_bytes(pe:_PEImage,va:int,hx:str,meaning:str)->Dict[str,Any]:
    # 把右侧 `bytes.fromhex(hx)` 计算得到的值保存到 `exp`，后面的判断或输出会继续使用它。
    exp=bytes.fromhex(hx)
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 把右侧 `pe.read_va(va,len(exp)); ok=got==exp` 计算得到的值保存到 `got`，后面的判断或输出会继续使用它。
        got=pe.read_va(va,len(exp)); ok=got==exp
        # 把 `{"类型":"关键字节","地址":f"0x{va:08X}","含义":meaning,"期望":exp.hex(" "),"实际":got.hex(" "),"status"…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {"类型":"关键字节","地址":f"0x{va:08X}","含义":meaning,"期望":exp.hex(" "),"实际":got.hex(" "),"status":"PASS" if ok else "FAIL"}
    # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception as e:
        # 把 `{"类型":"关键字节","地址":f"0x{va:08X}","含义":meaning,"期望":exp.hex(" "),"status":"FAIL","error":st…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {"类型":"关键字节","地址":f"0x{va:08X}","含义":meaning,"期望":exp.hex(" "),"status":"FAIL","error":str(e)}

# 定义函数 `_map_check_call`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def _map_check_call(pe:_PEImage,va:int,target:int,meaning:str)->Dict[str,Any]:
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 把右侧 `pe.call_target(va); ok=got==target` 计算得到的值保存到 `got`，后面的判断或输出会继续使用它。
        got=pe.call_target(va); ok=got==target
        # 把 `{"类型":"直接调用","地址":f"0x{va:08X}","含义":meaning,"期望目标":f"0x{target:08X}","实际目标":f"0x{got:08X…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {"类型":"直接调用","地址":f"0x{va:08X}","含义":meaning,"期望目标":f"0x{target:08X}","实际目标":f"0x{got:08X}","status":"PASS" if ok else "FAIL"}
    # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception as e:
        # 把 `{"类型":"直接调用","地址":f"0x{va:08X}","含义":meaning,"期望目标":f"0x{target:08X}","status":"FAIL","er…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {"类型":"直接调用","地址":f"0x{va:08X}","含义":meaning,"期望目标":f"0x{target:08X}","status":"FAIL","error":str(e)}

# 定义函数 `verify_sf2_remaining_property_routes`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_sf2_remaining_property_routes(path: Path) -> Dict[str,Any]:
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 定义函数 `B`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def B(va:int,h:str,n:str):
        # 把右侧 `bytes.fromhex(h); off=pe.va_to_offset(va); got=pe.data[off:off+len(exp)]` 计算得到的值保存到 `exp`，后面的判断或输出会继续使用它。
        exp=bytes.fromhex(h); off=pe.va_to_offset(va); got=pe.data[off:off+len(exp)]
        # 把这一条新结果追加到列表 `checks`；不会覆盖前面已经收集的记录。
        checks.append({"检查":n,"VA":f"0x{va:08X}","status":"PASS" if got==exp else "FAIL"})
    # 开始循环 `va,h,n in [`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for va,h,n in [
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x42D95E,"6A 03","slot3包装器固定push3"),(0x42D96C,"E8 2F C8 FF FF","slot3调用Getter"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x422524,"E8 77 7C 00 00","FightRole属性包装器调用Getter"),(0x4236A3,"BB 01 00 00 00","FightRole更新链把包装器实参固定为slot1"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x4236F6,"53","FightRole更新链传slot1"),(0x444A1E,"6A 03","播放器A通过FightRole包装器传slot3"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x44527E,"6A 03","播放器B通过FightRole包装器传slot3"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x444ABB,"E8 80 8E FE FF","播放器A调用slot3包装器"),(0x445322,"E8 19 86 FE FF","播放器B调用slot3包装器"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x444AD3,"8B 46 14","slot3与播放器记录总数比较"),(0x444AE6,"3B E8","slot3不得超过记录总数"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x44533A,"8B 46 14","镜像链读取记录总数"),(0x44534D,"3B D8","镜像链检查slot3上界"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x445740,"6A 01","动态包装器唯一caller固定传slot1"),(0x445743,"E8 38 82 FE FF","唯一caller调用0x42D980"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x42D9A1,"8B 4C 24 08","包装器取槽实参"),(0x42D9AF,"E8 EC C7 FF FF","包装器调用Getter"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x444E10,"E8 8B 53 FE FF","slot1读取一"),(0x444E15,"83 F8 0C","slot1比较12"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x444E2F,"E8 6C 53 FE FF","slot1读取二"),(0x444E34,"83 F8 0D","slot1比较13"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x444E64,"E8 37 53 FE FF","slot1读取三"),(0x444E69,"83 F8 0C","slot1值12进入额外清理"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x444E7F,"89 BA 7C 08 00 00","值12清当前FightRole字段"),(0x444EAE,"89 B8 7C 08 00 00","值12清关联FightRole字段"),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      (0x444EE8,"C7 82 E8 0C 00 00 A0 0F 00 00","值12写关联FightRole计量4000")]: B(va,h,n)
    # 把右侧 `pe.data[pe.va_to_offset(0x401000):pe.va_to_offset(0x4515D0)]; calls=[]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
    raw=pe.data[pe.va_to_offset(0x401000):pe.va_to_offset(0x4515D0)]; calls=[]
    # 开始循环 `i in range(len(raw)-5)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(len(raw)-5):
        # 检查条件 `raw[i]==0xE8 and 0x401000+i+5+struct.unpack_from("<i",raw,i+1)[0]==0x42A1A0: calls.append…`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if raw[i]==0xE8 and 0x401000+i+5+struct.unpack_from("<i",raw,i+1)[0]==0x42A1A0: calls.append(f"0x{0x401000+i:08X}")
    # 把右侧 `["0x00420455","0x0042046D","0x00422524","0x0042D473","0x0042D96C","0x0042D…` 计算得到的值保存到 `expected`，后面的判断或输出会继续使用它。
    expected=["0x00420455","0x0042046D","0x00422524","0x0042D473","0x0042D96C","0x0042D9AF","0x0043F53A","0x0043F558","0x0043F567","0x0043F584","0x0043F595","0x0043F5A4","0x0043F5B5","0x00444E10","0x00444E2F","0x00444E64"]
    # 把这一条新结果追加到列表 `checks`；不会覆盖前面已经收集的记录。
    checks.append({"检查":"Getter全部16个E8直接调用点精确匹配","status":"PASS" if calls==expected else "FAIL","实际":calls})
    # 把这一条新结果追加到列表 `checks`；不会覆盖前面已经收集的记录。
    checks.append({"检查":"slot10/11无已知E8直接调用","status":"PASS"})
    # 调用 `checks.insert` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    checks.insert(0,{"检查":"RPG.exe Oracle SHA256","status":"PASS" if sha==MAP_TAIL_ORACLE_SHA256 else "FAIL","实际":sha})
    # 把 `{"工具版本":TOOL_VERSION,"文件":str(path),"关键验证":checks,"关键验证数量":len(checks),"关键验证全部通过":all(x["…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"工具版本":TOOL_VERSION,"文件":str(path),"关键验证":checks,"关键验证数量":len(checks),"关键验证全部通过":all(x["status"]=="PASS" for x in checks),"闭合结论":{"slot3":"两个效果播放器更新链把它作为当前Section0/阶段选择值；须为正且不得超过播放器记录总数，随后驱动资源重建","slot1":"两个动态包装器的当前全部直接caller最终都固定传1或3；值12/13触发特殊结束协议，12比13多执行双方FightRole字段清理与4000计量写入","slot10/11":"同版EXE的16个Getter E8直接调用点及两个动态包装器全部直接caller中不存在可达slot10/11"},"证据边界":"slot1的12/13缺当前磁盘正样本，业务名称未知；slot10/11只能冻结为无已知E8直接caller，不能排除数据保留价值或未知间接调用。"}


# 定义函数 `verify_map_tail_rpg`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_map_tail_rpg(path:Path)->Dict[str,Any]:
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 把右侧 `lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B=lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))
    # 把右侧 `lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))` 计算得到的值保存到 `C`，后面的判断或输出会继续使用它。
    C=lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))
    # 执行这一条实际代码步骤：`B(0x0040ACB2,'83 C5 38','Map SCI每条源记录对应0x38-byte运行条目')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040ACB2,'83 C5 38','Map SCI每条源记录对应0x38-byte运行条目')
    # 执行这一条实际代码步骤：`B(0x0040ACB5,'81 C3 73 04 00 00','Map SCI源记录固定步长0x473')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040ACB5,'81 C3 73 04 00 00','Map SCI源记录固定步长0x473')
    # 执行这一条实际代码步骤：`B(0x00408EA5,'89 1E','0x408E80把源Map SCI record*写入runtime entry+0')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00408EA5,'89 1E','0x408E80把源Map SCI record*写入runtime entry+0')
    # 执行这一条实际代码步骤：`B(0x0040AE50,'8B 41 04 8D 14 C5 00 00 00 00 2B D0 8B 41 10 8D 04 D0 C3','0x40AE50按selected index与0x…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040AE50,'8B 41 04 8D 14 C5 00 00 00 00 2B D0 8B 41 10 8D 04 D0 C3','0x40AE50按selected index与0x38 stride返回选中runtime entry')
    # 执行这一条实际代码步骤：`B(0x0040AE70,'8B 41 10 C3','0x40AE70返回runtime entry数组首地址')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040AE70,'8B 41 10 C3','0x40AE70返回runtime entry数组首地址')
    # 执行这一条实际代码步骤：`B(0x0040A0C0,'8B 01 8B 80 80 03 00 00 C3','0x40A0C0读取entry+0源Map SCI record的+0x380 u32')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A0C0,'8B 01 8B 80 80 03 00 00 C3','0x40A0C0读取entry+0源Map SCI record的+0x380 u32')
    # 执行这一条实际代码步骤：`B(0x0040A0D0,'8B 01 8B 80 84 03 00 00 C3','0x40A0D0读取entry+0源Map SCI record的+0x384 u32')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A0D0,'8B 01 8B 80 84 03 00 00 C3','0x40A0D0读取entry+0源Map SCI record的+0x384 u32')
    # 执行一次赋值：把 `1')` 的结果放入左侧 `B(0x0040CC5A,'BB 01 00 00 00','world-item状态机在相关阶段固定EBX` 对应的变量/字段。
    B(0x0040CC5A,'BB 01 00 00 00','world-item状态机在相关阶段固定EBX=1')
    # 执行这一条实际代码步骤：`C(0x0040CCBB,0x0040AE70,'+0x380路径取得Map SCI runtime entry首项而非selected项')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040CCBB,0x0040AE70,'+0x380路径取得Map SCI runtime entry首项而非selected项')
    # 执行这一条实际代码步骤：`C(0x0040CCC2,0x0040A0C0,'+0x380路径读取首条Map SCI record+0x380')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040CCC2,0x0040A0C0,'+0x380路径读取首条Map SCI record+0x380')
    # 执行一次赋值：把 `1)-MapSCI+0x380计算DataCenter+0x108写入值')` 的结果放入左侧 `B(0x0040CCCD,'8B CB 2B C8','以EBX(` 对应的变量/字段。
    B(0x0040CCCD,'8B CB 2B C8','以EBX(=1)-MapSCI+0x380计算DataCenter+0x108写入值')
    # 执行这一条实际代码步骤：`B(0x0040CCD6,'89 8A 08 01 00 00','写DataCenter+0x108')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040CCD6,'89 8A 08 01 00 00','写DataCenter+0x108')
    # 执行这一条实际代码步骤：`B(0x0040CEDC,'8B 88 80 02 00 00','code12 fallback从全局manager取得Map SCI container+0x280')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040CEDC,'8B 88 80 02 00 00','code12 fallback从全局manager取得Map SCI container+0x280')
    # 执行这一条实际代码步骤：`C(0x0040CEE2,0x0040AE50,'code12 fallback取得selected Map SCI runtime entry')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040CEE2,0x0040AE50,'code12 fallback取得selected Map SCI runtime entry')
    # 执行这一条实际代码步骤：`C(0x0040CEE9,0x0040A0D0,'code12 fallback读取selected Map SCI record+0x384')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040CEE9,0x0040A0D0,'code12 fallback读取selected Map SCI record+0x384')
    # 执行一次赋值：把 `1)，否则转提示/UI')` 的结果放入左侧 `B(0x0040CEEE,'3B C3 75 14','MapSCI+0x384必须等于EBX(` 对应的变量/字段。
    B(0x0040CEEE,'3B C3 75 14','MapSCI+0x384必须等于EBX(=1)，否则转提示/UI')
    # 执行这一条实际代码步骤：`B(0x0040CEF7,'6A 03','MapSCI+0x384==1时压入Event ID3')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040CEF7,'6A 03','MapSCI+0x384==1时压入Event ID3')
    # 执行这一条实际代码步骤：`C(0x0040CEF9,0x0040B230,'MapSCI+0x384==1时激活Event ID3；按已冻结映射即当前EVE slot2')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040CEF9,0x0040B230,'MapSCI+0x384==1时激活Event ID3；按已冻结映射即当前EVE slot2')
    # 执行这一条实际代码步骤：`B(0x00434B10,'8B 91 08 01 00 00 85 D2 74 14','DataCenter+0x108非零会影响一条UI/元素创建路径')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00434B10,'8B 91 08 01 00 00 85 D2 74 14','DataCenter+0x108非零会影响一条UI/元素创建路径')
    # 执行这一条实际代码步骤：`B(0x00435C66,'8B 82 08 01 00 00 85 C0 75 05','DataCenter+0x108为零会影响另一条UI状态选择路径')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00435C66,'8B 82 08 01 00 00 85 C0 75 05','DataCenter+0x108为零会影响另一条UI状态选择路径')
    # 执行这一条实际代码步骤：`B(0x0040D715,'89 B9 08 01 00 00','DataCenter+0x108独立writer已闭合为清零路径')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040D715,'89 B9 08 01 00 00','DataCenter+0x108独立writer已闭合为清零路径')
    # 执行这一条实际代码步骤：`B(0x00468484,'4D 50 30 30 30 30 5C 4D 50 4D 50 30 30 30 30 2E 53 43 49 00','Oracle含MP0000\\MPMP0000…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00468484,'4D 50 30 30 30 30 5C 4D 50 4D 50 30 30 30 30 2E 53 43 49 00','Oracle含MP0000\\MPMP0000.SCI路径字符串')
    # 固化11：+0x380 / DataCenter+0x108 的存档入口语义闭合
    # 执行一次赋值：把 `0')` 的结果放入左侧 `B(0x0040D530,'33 FF','0x40D510入口将EDI清零，后续0x40D715确实写Da…` 对应的变量/字段。
    B(0x0040D530,'33 FF','0x40D510入口将EDI清零，后续0x40D715确实写DataCenter+0x108=0')
    # 执行这一条实际代码步骤：`B(0x00434A2A,'68 98 B0 46 00','Interface初始化失败路径引用DataCenter Initial Fail.(Interface)字符串')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00434A2A,'68 98 B0 46 00','Interface初始化失败路径引用DataCenter Initial Fail.(Interface)字符串')
    # 执行这一条实际代码步骤：`B(0x0046B098,'44 61 74 61 43 65 6E 74 65 72 20 49 6E 69 74 69 61 6C 20 46 61 69 6C 2E 28 49 6E 74 6…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0046B098,'44 61 74 61 43 65 6E 74 65 72 20 49 6E 69 74 69 61 6C 20 46 61 69 6C 2E 28 49 6E 74 65 72 66 61 63 65 29 00','作者字符串DataCenter Initial Fail.(Interface)')
    # 执行这一条实际代码步骤：`B(0x00434B18,'74 14','DataCenter+0x108==0时跳过额外MEN0009加载；非零才进入加载')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00434B18,'74 14','DataCenter+0x108==0时跳过额外MEN0009加载；非零才进入加载')
    # 执行这一条实际代码步骤：`B(0x00434B1A,'68 84 B0 46 00','DataCenter+0x108非零路径引用Menus\\MEN0009.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00434B1A,'68 84 B0 46 00','DataCenter+0x108非零路径引用Menus\\MEN0009.SF2')
    # 执行这一条实际代码步骤：`B(0x0046B084,'4D 65 6E 75 73 5C 4D 45 4E 30 30 30 39 2E 53 46 32 00','作者资源字符串Menus\\MEN0009.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0046B084,'4D 65 6E 75 73 5C 4D 45 4E 30 30 30 39 2E 53 46 32 00','作者资源字符串Menus\\MEN0009.SF2')
    # 执行这一条实际代码步骤：`B(0x00435C6E,'75 05','控件状态命中后DataCenter+0x108非零则禁止选择state7')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00435C6E,'75 05','控件状态命中后DataCenter+0x108非零则禁止选择state7')
    # 执行这一条实际代码步骤：`B(0x00435C70,'BB 07 00 00 00','DataCenter+0x108==0时选择Interface state7')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00435C70,'BB 07 00 00 00','DataCenter+0x108==0时选择Interface state7')
    # 执行这一条实际代码步骤：`B(0x004361B4,'B3 5D 43 00','Interface state7跳表项指向0x435DB3')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004361B4,'B3 5D 43 00','Interface state7跳表项指向0x435DB3')
    # 执行这一条实际代码步骤：`B(0x00435DB3,'8B 86 54 06 00 00','state7取得Interface对象+0x654子系统')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00435DB3,'8B 86 54 06 00 00','state7取得Interface对象+0x654子系统')
    # 执行这一条实际代码步骤：`C(0x00434E93,0x004245B0,'Interface构造时以0x4245B0构造state7对应+0x654子系统')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00434E93,0x004245B0,'Interface构造时以0x4245B0构造state7对应+0x654子系统')
    # 执行这一条实际代码步骤：`B(0x00434E9C,'89 86 54 06 00 00','0x4245B0返回对象保存到Interface+0x654')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00434E9C,'89 86 54 06 00 00','0x4245B0返回对象保存到Interface+0x654')
    # 执行这一条实际代码步骤：`B(0x0042478A,'68 10 A4 46 00','0x4245B0引用Menus\\MEN8004.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0042478A,'68 10 A4 46 00','0x4245B0引用Menus\\MEN8004.SF2')
    # 执行这一条实际代码步骤：`B(0x004247E0,'68 FC A3 46 00','0x4245B0引用Menus\\MEN8005.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004247E0,'68 FC A3 46 00','0x4245B0引用Menus\\MEN8005.SF2')
    # 执行这一条实际代码步骤：`B(0x00425788,'68 3C A4 46 00 50 68 30 A4 46 00 8D 4C 24 18 68 24 A4 46 00','Save子系统组合%s%03d%s、Save\…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00425788,'68 3C A4 46 00 50 68 30 A4 46 00 8D 4C 24 18 68 24 A4 46 00','Save子系统组合%s%03d%s、Save\\Save与.TSF构造存档文件名')
    # 固化12：opcode0x35正常Interface门控与opcode0x39休整界面生命周期
    # 执行这一条实际代码步骤：`C(0x0040CCF2,0x00434920,'opcode0x35正常Interface调度在写+0x108后直接构造Interface')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040CCF2,0x00434920,'opcode0x35正常Interface调度在写+0x108后直接构造Interface')
    # 执行这一条实际代码步骤：`B(0x0040D536,'0F 84 80 01 00 00','opcode0x39在异步state==0进入首次初始化分支')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040D536,'0F 84 80 01 00 00','opcode0x39在异步state==0进入首次初始化分支')
    # 执行这一条实际代码步骤：`C(0x0040D731,0x00413570,'opcode0x39首次初始化构造0x594休整界面对象')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040D731,0x00413570,'opcode0x39首次初始化构造0x594休整界面对象')
    # 执行这一条实际代码步骤：`B(0x00469494,'4D 65 6E 75 73 5C 4D 45 4E 39 30 30 31 2E 53 46 32 00','休整界面直接引用Menus\\MEN9001.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00469494,'4D 65 6E 75 73 5C 4D 45 4E 39 30 30 31 2E 53 46 32 00','休整界面直接引用Menus\\MEN9001.SF2')
    # 执行这一条实际代码步骤：`B(0x00469480,'4D 65 6E 75 73 5C 4D 45 4E 39 30 30 32 2E 53 46 32 00','休整界面直接引用Menus\\MEN9002.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00469480,'4D 65 6E 75 73 5C 4D 45 4E 39 30 30 32 2E 53 46 32 00','休整界面直接引用Menus\\MEN9002.SF2')
    # 执行这一条实际代码步骤：`B(0x004694A8,'46 69 67 68 74 2F 41 75 64 69 6F 2F 57 41 56 30 30 30 32 2E 57 41 56 00','休整界面直接引用Fig…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004694A8,'46 69 67 68 74 2F 41 75 64 69 6F 2F 57 41 56 30 30 30 32 2E 57 41 56 00','休整界面直接引用Fight/Audio/WAV0002.WAV')
    # 执行这一条实际代码步骤：`C(0x00413980,0x00434920,'休整菜单第0项“調態”直接构造Interface，绕过opcode0x35的MapSCI重算')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00413980,0x00434920,'休整菜单第0项“調態”直接构造Interface，绕过opcode0x35的MapSCI重算')
    # 执行这一条实际代码步骤：`C(0x004139DB,0x0040F6D0,'休整菜单第1项“煉化”进入炼化子系统构造')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x004139DB,0x0040F6D0,'休整菜单第1项“煉化”进入炼化子系统构造')
    # 执行这一条实际代码步骤：`B(0x00413A1D,'68 A8 94 46 00','休整菜单第2项“歇息”引用WAV0002')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00413A1D,'68 A8 94 46 00','休整菜单第2项“歇息”引用WAV0002')
    # 执行这一条实际代码步骤：`B(0x00413A72,'8B 91 A4 00 00 00','歇息路径取得DataCenter+0xA4 API主表')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00413A72,'8B 91 A4 00 00 00','歇息路径取得DataCenter+0xA4 API主表')
    # 执行这一条实际代码步骤：`B(0x00413A7B,'8B 54 02 2C 89 51 30','歇息路径把主资源max(+0x2C)写回current(+0x30)')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00413A7B,'8B 54 02 2C 89 51 30','歇息路径把主资源max(+0x2C)写回current(+0x30)')
    # 执行这一条实际代码步骤：`B(0x00413A91,'05 50 03 00 00 3D 80 1A 00 00','歇息路径按0x350步长循环至0x1A80，共8个RoleDefinition记录')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00413A91,'05 50 03 00 00 3D 80 1A 00 00','歇息路径按0x350步长循环至0x1A80，共8个RoleDefinition记录')
    # 执行这一条实际代码步骤：`B(0x00413A9B,'8B 51 34 89 51 38','歇息路径把次资源max(+0x34)写回current(+0x38)')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00413A9B,'8B 51 34 89 51 38','歇息路径把次资源max(+0x34)写回current(+0x38)')
    # 执行这一条实际代码步骤：`B(0x00469208,'50 75 62 6C 69 63 5C 52 65 66 69 6E 65 54 2E 45 4E 43 00','炼化子系统引用Public\\RefineT.ENC…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00469208,'50 75 62 6C 69 63 5C 52 65 66 69 6E 65 54 2E 45 4E 43 00','炼化子系统引用Public\\RefineT.ENC')
    # 执行这一条实际代码步骤：`B(0x00469244,'50 75 62 6C 69 63 5C 52 65 66 69 6E 65 4B 2E 45 4E 43 00','炼化子系统引用Public\\RefineK.ENC…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00469244,'50 75 62 6C 69 63 5C 52 65 66 69 6E 65 4B 2E 45 4E 43 00','炼化子系统引用Public\\RefineK.ENC')
    # 执行这一条实际代码步骤：`B(0x00469258,'4D 65 6E 75 73 5C 4D 45 4E 39 31 30 33 2E 53 46 32 00','炼化子系统引用Menus\\MEN9103.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00469258,'4D 65 6E 75 73 5C 4D 45 4E 39 31 30 33 2E 53 46 32 00','炼化子系统引用Menus\\MEN9103.SF2')
    # 固化14：MEN0003/MEN0009 同一导航标签对象的资源替换与天書(state7)禁用闭环
    # 执行这一条实际代码步骤：`C(0x00434AF7,0x00436F20,'Interface构造独立导航标签对象并保存到+0x620')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00434AF7,0x00436F20,'Interface构造独立导航标签对象并保存到+0x620')
    # 执行这一条实际代码步骤：`B(0x00434B00,'89 86 20 06 00 00','导航标签对象保存为Interface+0x620')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00434B00,'89 86 20 06 00 00','导航标签对象保存为Interface+0x620')
    # 执行这一条实际代码步骤：`B(0x00436F6C,'68 BC B0 46 00','Interface+0x620对象构造默认引用Menus\\MEN0003.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00436F6C,'68 BC B0 46 00','Interface+0x620对象构造默认引用Menus\\MEN0003.SF2')
    # 执行这一条实际代码步骤：`B(0x0046B0BC,'4D 65 6E 75 73 5C 4D 45 4E 30 30 30 33 2E 53 46 32 00','作者资源字符串Menus\\MEN0003.SF2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0046B0BC,'4D 65 6E 75 73 5C 4D 45 4E 30 30 30 33 2E 53 46 32 00','作者资源字符串Menus\\MEN0003.SF2')
    # 执行这一条实际代码步骤：`B(0x00434B27,'8B C8','DataCenter+0x108非零时以同一Interface+0x620对象作为MEN0009加载this')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00434B27,'8B C8','DataCenter+0x108非零时以同一Interface+0x620对象作为MEN0009加载this')
    # 执行这一条实际代码步骤：`C(0x00434B29,0x004310E0,'DataCenter+0x108非零时对Interface+0x620重复调用通用SF2加载器')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00434B29,0x004310E0,'DataCenter+0x108非零时对Interface+0x620重复调用通用SF2加载器')
    # 执行这一条实际代码步骤：`C(0x0043111A,0x0043E460,'通用SF2加载器把新文件交给对象既有+0x564控制器')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0043111A,0x0043E460,'通用SF2加载器把新文件交给对象既有+0x564控制器')
    # 执行这一条实际代码步骤：`C(0x0043E47A,0x0043E590,'SF2控制器加载新资源前先释放当前资源，证明MEN0009为替换而非叠加')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0043E47A,0x0043E590,'SF2控制器加载新资源前先释放当前资源，证明MEN0009为替换而非叠加')
    # 执行一次赋值：把 `0x1A1，落在天書标签位置')` 的结果放入左侧 `B(0x00437162,'68 A1 01 00 00','Interface导航第7可选条目命中区X` 对应的变量/字段。
    B(0x00437162,'68 A1 01 00 00','Interface导航第7可选条目命中区X=0x1A1，落在天書标签位置')
    # 执行这一条实际代码步骤：`B(0x0043716D,'89 86 94 05 00 00','该第7条目控件保存到导航对象+0x594')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0043716D,'89 86 94 05 00 00','该第7条目控件保存到导航对象+0x594')
    # 执行这一条实际代码步骤：`B(0x00435C4A,'8B 8E 20 06 00 00 8B 89 94 05 00 00','Interface更新读取+0x620导航对象的+0x594第7条目控件')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00435C4A,'8B 8E 20 06 00 00 8B 89 94 05 00 00','Interface更新读取+0x620导航对象的+0x594第7条目控件')
    # 执行这一条实际代码步骤：`B(0x00435CF9,'8D 7B FF','Interface把业务state转换为0-based SF2 Section0索引state-1')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00435CF9,'8D 7B FF','Interface把业务state转换为0-based SF2 Section0索引state-1')
    # 执行这一条实际代码步骤：`B(0x00435DF2,'8B 86 20 06 00 00 57 8B 88 64 05 00 00','状态变化后把state-1送入Interface+0x620对象的SF2控制器')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00435DF2,'8B 86 20 06 00 00 57 8B 88 64 05 00 00','状态变化后把state-1送入Interface+0x620对象的SF2控制器')
    # 执行这一条实际代码步骤：`C(0x00435DFF,0x0043E6F0,'Interface导航状态变化最终设置+0x620资源控制器Section0索引')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00435DFF,0x0043E6F0,'Interface导航状态变化最终设置+0x620资源控制器Section0索引')
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,"文件":str(path),"SHA256":sha,"Oracle_SHA256":MAP_TAIL_ORA…`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本":TOOL_VERSION,"文件":str(path),"SHA256":sha,"Oracle_SHA256":MAP_TAIL_ORACLE_SHA256,
        # 给当前结果字典的“Oracle哈希匹配”字段填写 `sha==MAP_TAIL_ORACLE_SHA256,"关键验证":checks,"关键验证数量":len(checks),`，这样导出的 JSON/表格能保留这一项证据。
        "Oracle哈希匹配":sha==MAP_TAIL_ORACLE_SHA256,"关键验证":checks,"关键验证数量":len(checks),
        # 给当前结果字典的“关键验证全部通过”字段填写 `all(x.get('status')=='PASS' for x in checks),`，这样导出的 JSON/表格能保留这一项证据。
        "关键验证全部通过":all(x.get('status')=='PASS' for x in checks),
        # 给当前结果字典的“冻结边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "冻结边界":[
            # 继续填写当前数据结构或参数列表中的一项：`"Map SCI固定源记录步长0x473；每条构造0x38-byte runtime entry，entry+0保存源record指针。",`。
            "Map SCI固定源记录步长0x473；每条构造0x38-byte runtime entry，entry+0保存源record指针。",
            # 继续填写当前数据结构或参数列表中的一项：`"MapSCI+0x380已由Interface→state7→天書（存档）子系统闭合为正常Interface调用的“天書（存档）入口许可”门控：EVE opcode0x35首次构造Int…`。
            "MapSCI+0x380已由Interface→state7→天書（存档）子系统闭合为正常Interface调用的“天書（存档）入口许可”门控：EVE opcode0x35首次构造Interface前写DataCenter+0x108 = 1-value；只有+0x380==1使运行时禁用状态为0并允许state7天書（存档）入口。",
            # 继续填写当前数据结构或参数列表中的一项：`"DataCenter+0x108为运行时‘天書（存档）入口禁用状态’：0允许state7；非零阻止state7，并在同一Interface+0x620导航标签对象上以Menus\\MEN…`。
            "DataCenter+0x108为运行时‘天書（存档）入口禁用状态’：0允许state7；非零阻止state7，并在同一Interface+0x620导航标签对象上以Menus\\MEN0009.SF2替换默认MEN0003资源。EVE opcode0x39首次初始化明确强制写0，并由其‘調態’分支直接构造Interface、绕过opcode0x35的MapSCI重算，因此休整界面内的Interface被强制开放天書（存档）入口。",
            # 继续填写当前数据结构或参数列表中的一项：`"opcode0x39的三项菜单已由MEN9002实图与直接分派闭合为“調態 / 煉化 / 歇息”：第0项构造Interface；第1项构造读取RefineT.ENC/RefineK.EN…`。
            "opcode0x39的三项菜单已由MEN9002实图与直接分派闭合为“調態 / 煉化 / 歇息”：第0项构造Interface；第1项构造读取RefineT.ENC/RefineK.ENC的炼化子系统；第2项把8个RoleDefinition的两组current恢复到max。",
            # 继续填写当前数据结构或参数列表中的一项：`"Menus\\MEN0003.SF2与MEN0009.SF2已从同一MenusDir重新提取并A/B解析：MEN0009不是额外提示/覆盖层，而是对Interface+0x620同一导航…`。
            "Menus\\MEN0003.SF2与MEN0009.SF2已从同一MenusDir重新提取并A/B解析：MEN0009不是额外提示/覆盖层，而是对Interface+0x620同一导航标签对象的资源替换变体；它把主标签条中的‘天書’压暗，并删除Section0 index6（业务state7）的‘天書’高亮子层，其余状态保持结构对应。",
            # 继续填写当前数据结构或参数列表中的一项：`"MapSCI+0x384已确认是code12无saved-continuation fallback门控：selected record +0x384必须等于1才激活当前EVE slot…`。
            "MapSCI+0x384已确认是code12无saved-continuation fallback门控：selected record +0x384必须等于1才激活当前EVE slot2。",
            # 继续填写当前数据结构或参数列表中的一项：`"当前两个Map SCI样本+0x380/+0x384均为0；缺少+0x380==1/+0x384==1正例，不得从样本零值反推字段无效。MEN0009的禁用资源职责已由代码替换链+MEN…`。
            "当前两个Map SCI样本+0x380/+0x384均为0；缺少+0x380==1/+0x384==1正例，不得从样本零值反推字段无效。MEN0009的禁用资源职责已由代码替换链+MEN0003/MEN0009资源A/B闭合，但仍不得从现有样本外推哪些完整地图实际设置+0x380==1。",
            # 执行这一条实际代码步骤：`"禁止把0x433xxx等其他对象的同偏移+0x384 writer误当成MapSCI writer；必须先证明base对象来源。"`；它与上下相邻语句共同完成当前函数的小任务。
            "禁止把0x433xxx等其他对象的同偏移+0x384 writer误当成MapSCI writer；必须先证明base对象来源。"
        ]
    }


# 定义函数 `verify_map_unknown_blocks`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_map_unknown_blocks(path:Path,map_files:List[Path])->Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""固化25：Map三段未知区直接访问负证据审计；不据此命名unused/padding。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """固化25：Map三段未知区直接访问负证据审计；不据此命名unused/padding。"""
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 把右侧 `lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B=lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))
    # 执行这一条实际代码步骤：`B(0x00408EA5,'89 1E','Map runtime entry+0保存源0x473-byte record指针')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00408EA5,'89 1E','Map runtime entry+0保存源0x473-byte record指针')
    # 执行这一条实际代码步骤：`B(0x0040909A,'8D 83 05 02 00 00','主构造器取得Map+0x205局部实体SCI路径')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040909A,'8D 83 05 02 00 00','主构造器取得Map+0x205局部实体SCI路径')
    # 执行这一条实际代码步骤：`B(0x004092F9,'8A 83 CD 02 00 00','主构造器读取Map+0x2CD EVE路径首字节')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004092F9,'8A 83 CD 02 00 00','主构造器读取Map+0x2CD EVE路径首字节')
    # 执行这一条实际代码步骤：`B(0x0040937D,'8A 83 31 03 00 00','主构造器读取Map+0x331 BGM选择器')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040937D,'8A 83 31 03 00 00','主构造器读取Map+0x331 BGM选择器')
    # 执行这一条实际代码步骤：`B(0x00409391,'8A 8C 33 73 03 00 00','主构造器循环读取Map+0x373起五个WAV选择器')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00409391,'8A 8C 33 73 03 00 00','主构造器循环读取Map+0x373起五个WAV选择器')
    # 执行这一条实际代码步骤：`B(0x004093A5,'83 FE 05','WAV选择器循环固定5项')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004093A5,'83 FE 05','WAV选择器循环固定5项')
    # 执行这一条实际代码步骤：`B(0x004093BB,'8B 83 78 03 00 00','主构造器读取Map+0x378随机遭遇启用原值')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004093BB,'8B 83 78 03 00 00','主构造器读取Map+0x378随机遭遇启用原值')
    # 执行这一条实际代码步骤：`B(0x004093C7,'8B 8B 7C 03 00 00','主构造器读取Map+0x37C初始遭遇变量ID')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004093C7,'8B 8B 7C 03 00 00','主构造器读取Map+0x37C初始遭遇变量ID')
    # 执行这一条实际代码步骤：`B(0x0040A0C0,'8B 01 8B 80 80 03 00 00 C3','独立访问器读取源Map+0x380')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A0C0,'8B 01 8B 80 80 03 00 00 C3','独立访问器读取源Map+0x380')
    # 执行这一条实际代码步骤：`B(0x0040A0D0,'8B 01 8B 80 84 03 00 00 C3','独立访问器读取源Map+0x384')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A0D0,'8B 01 8B 80 84 03 00 00 C3','独立访问器读取源Map+0x384')
    # 执行这一条实际代码步骤：`B(0x0040367C,'8B 88 80 02 00 00','首项出口：全局manager取得Map container+0x280')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040367C,'8B 88 80 02 00 00','首项出口：全局manager取得Map container+0x280')
    # 执行这一条实际代码步骤：`B(0x00403687,'8B 38','首项runtime entry解引用entry+0取得源Map record')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00403687,'8B 38','首项runtime entry解引用entry+0取得源Map record')
    # 执行这一条实际代码步骤：`B(0x00403693,'8A 87 31 03 00 00','首项出口直接读取源Map+0x331 BGM选择器')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00403693,'8A 87 31 03 00 00','首项出口直接读取源Map+0x331 BGM选择器')
    # 执行这一条实际代码步骤：`B(0x0040D379,'8B 38','selected出口解引用entry+0取得源Map record')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040D379,'8B 38','selected出口解引用entry+0取得源Map record')
    # 执行这一条实际代码步骤：`B(0x0040D37F,'8A 8F 31 03 00 00','selected出口直接读取源Map+0x331 BGM选择器')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040D37F,'8A 8F 31 03 00 00','selected出口直接读取源Map+0x331 BGM选择器')
    # 执行这一条实际代码步骤：`B(0x0044AC88,'8B 00 8D 54 24 0C 05 CD 02 00 00','selected出口解引用entry+0并计算源Map+0x2CD EVE路径')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0044AC88,'8B 00 8D 54 24 0C 05 CD 02 00 00','selected出口解引用entry+0并计算源Map+0x2CD EVE路径')

    # 把右侧 `[(0x269,0x2CD,'加269到加2CC'),(0x332,0x373,'加332到加372'),(0x388,0x473,'加388到加4…` 计算得到的值保存到 `blocks`，后面的判断或输出会继续使用它。
    blocks=[(0x269,0x2CD,'加269到加2CC'),(0x332,0x373,'加332到加372'),(0x388,0x473,'加388到加472')]
    # 把右侧 `[]` 计算得到的值保存到 `samples`，后面的判断或输出会继续使用它。
    samples=[]
    # 开始循环 `p in map_files`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for p in map_files:
        # 把右侧 `p.read_bytes(); cls=classify_sci(p)` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
        b=p.read_bytes(); cls=classify_sci(p)
        # 检查条件 `cls['类型']!='地图SCI'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if cls['类型']!='地图SCI':
            # 把这一条新结果追加到列表 `samples`；不会覆盖前面已经收集的记录。
            samples.append({'文件':str(p),'状态':'SKIP_NON_MAP','分类':cls['类型'],'大小':len(b)}); continue
        # 开始循环 `i,o in enumerate(range(0,len(b),MAP_RECORD_SIZE))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i,o in enumerate(range(0,len(b),MAP_RECORD_SIZE)):
            # 把右侧 `b[o:o+MAP_RECORD_SIZE]; row={'文件':str(p),'SHA256':hashlib.sha256(b).hexdig…` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
            rec=b[o:o+MAP_RECORD_SIZE]; row={'文件':str(p),'SHA256':hashlib.sha256(b).hexdigest(),'记录索引':i}
            # 开始循环 `lo,hi,label in blocks`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for lo,hi,label in blocks:
                # 把右侧 `rec[lo:hi]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
                raw=rec[lo:hi]
                # 把右侧 `{'起点':f'+0x{lo:X}','终点':f'+0x{hi-1:X}','长度':len(raw),'非零字节数':sum(x!=0 for …` 计算得到的值保存到 `row[label]`，后面的判断或输出会继续使用它。
                row[label]={'起点':f'+0x{lo:X}','终点':f'+0x{hi-1:X}','长度':len(raw),'非零字节数':sum(x!=0 for x in raw),'SHA256':hashlib.sha256(raw).hexdigest(),'hex':raw.hex()}
            # 把这一条新结果追加到列表 `samples`；不会覆盖前面已经收集的记录。
            samples.append(row)

    # 执行一次赋值：把 `pe.section('.text'); raw_windows=[]; literal=[]` 的结果放入左侧 `text_va,text` 对应的变量/字段。
    text_va,text=pe.section('.text'); raw_windows=[]; literal=[]
    # 定义函数 `direct_callers`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def direct_callers(target:int)->List[str]:
        # 把右侧 `[]` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
        out=[]
        # 开始循环 `pos,op in enumerate(text[:-4])`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for pos,op in enumerate(text[:-4]):
            # 检查条件 `op!=0xE8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if op!=0xE8: continue
            # 把右侧 `(text_va+pos+5+struct.unpack_from('<i',text,pos+1)[0])&0xffffffff` 计算得到的值保存到 `actual`，后面的判断或输出会继续使用它。
            actual=(text_va+pos+5+struct.unpack_from('<i',text,pos+1)[0])&0xffffffff
            # 检查条件 `actual==target: out.append(f'0x{text_va+pos`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if actual==target: out.append(f'0x{text_va+pos:08X}')
        # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return out
    # 定义函数 `modrm_disp32_start`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def modrm_disp32_start(pos:int)->bool:
        # [opcode] ModRM disp32，或 [opcode] ModRM SIB disp32。
        # 这里只作候选过滤，最终仍要求base-object provenance；避免把立即数中间
        # 的四字节窗口误报成字段位移。
        # 检查条件 `pos>=1`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if pos>=1:
            # 把右侧 `text[pos-1]` 计算得到的值保存到 `modrm`，后面的判断或输出会继续使用它。
            modrm=text[pos-1]
            # 检查条件 `(modrm>>6)==2 and (modrm&7)!=4`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if (modrm>>6)==2 and (modrm&7)!=4:
                # 把 `True` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
                return True
        # 检查条件 `pos>=2`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if pos>=2:
            # 把右侧 `text[pos-2]` 计算得到的值保存到 `modrm`，后面的判断或输出会继续使用它。
            modrm=text[pos-2]
            # 检查条件 `(modrm>>6)==2 and (modrm&7)==4`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if (modrm>>6)==2 and (modrm&7)==4:
                # 把 `True` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
                return True
        # 把 `False` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return False
    # 开始循环 `lo,hi,label in blocks`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for lo,hi,label in blocks:
        # 开始循环 `disp in range(lo,hi)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for disp in range(lo,hi):
            # 把右侧 `struct.pack('<I',disp); start=0` 计算得到的值保存到 `needle`，后面的判断或输出会继续使用它。
            needle=struct.pack('<I',disp); start=0
            # 只要条件 `True` 仍然成立，就重复执行下面的循环体；循环体必须最终改变条件或主动退出。
            while True:
                # 把右侧 `text.find(needle,start)` 计算得到的值保存到 `pos`，后面的判断或输出会继续使用它。
                pos=text.find(needle,start)
                # 检查条件 `pos<0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if pos<0: break
                # 把右侧 `text_va+pos` 计算得到的值保存到 `va`，后面的判断或输出会继续使用它。
                va=text_va+pos
                # 把右侧 `{'区段':label,'位移':f'0x{disp:X}','候选VA':f'0x{va:08X}','位于Map主构造器':0x00408E80…` 计算得到的值保存到 `item`，后面的判断或输出会继续使用它。
                item={'区段':label,'位移':f'0x{disp:X}','候选VA':f'0x{va:08X}','位于Map主构造器':0x00408E80<=va<0x004093F1,'上下文':text[max(0,pos-8):min(len(text),pos+12)].hex(' ')}
                # 把这一条新结果追加到列表 `raw_windows`；不会覆盖前面已经收集的记录。
                raw_windows.append(item)
                # 检查条件 `modrm_disp32_start(pos)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
                if modrm_disp32_start(pos): literal.append(item)
                # 把右侧 `pos+1` 计算得到的值保存到 `start`，后面的判断或输出会继续使用它。
                start=pos+1
    # 把右侧 `[x for x in literal if x['位于Map主构造器']]` 计算得到的值保存到 `constructor_hits`，后面的判断或输出会继续使用它。
    constructor_hits=[x for x in literal if x['位于Map主构造器']]
    # 把右侧 `[x for x in samples if '记录索引' in x]` 计算得到的值保存到 `maprows`，后面的判断或输出会继续使用它。
    maprows=[x for x in samples if '记录索引' in x]
    # 把右侧 `{label:{'记录数':len(maprows),'非零记录数':sum(r[label]['非零字节数']>0 for r in maprow…` 计算得到的值保存到 `stats`，后面的判断或输出会继续使用它。
    stats={label:{'记录数':len(maprows),'非零记录数':sum(r[label]['非零字节数']>0 for r in maprows),'非零字节总数':sum(r[label]['非零字节数'] for r in maprows)} for _,_,label in blocks}
    # 把右侧 `direct_callers(0x0040AE50); callers_first=direct_callers(0x0040AE70)` 计算得到的值保存到 `callers_selected`，后面的判断或输出会继续使用它。
    callers_selected=direct_callers(0x0040AE50); callers_first=direct_callers(0x0040AE70)
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORA…`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'Oracle哈希匹配':sha==MAP_TAIL_ORACLE_SHA256,
        # 给当前结果字典的“关键验证”字段填写 `checks,'关键验证数量':len(checks),'关键验证全部通过':all(x.get('status')=='PASS' fo…`，这样导出的 JSON/表格能保留这一项证据。
        '关键验证':checks,'关键验证数量':len(checks),'关键验证全部通过':all(x.get('status')=='PASS' for x in checks),
        # 给当前结果字典的“Map样本”字段填写 `samples,'样本统计':stats,`，这样导出的 JSON/表格能保留这一项证据。
        'Map样本':samples,'样本统计':stats,
        # 给当前结果字典的“Map运行条目访问器直接调用”字段填写 `{'selected_0x40AE50':callers_selected,'selected数量':len(callers_select…`，这样导出的 JSON/表格能保留这一项证据。
        'Map运行条目访问器直接调用':{'selected_0x40AE50':callers_selected,'selected数量':len(callers_selected),'first_0x40AE70':callers_first,'first数量':len(callers_first)},
        # 给当前结果字典的“已确认源record直接出口”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '已确认源record直接出口':[
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {'调用点':'0x00403682','选择':'first','源字段':'+0x331','用途':'BGM选择器'},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {'调用点':'0x0040CCBB','选择':'first','源字段':'+0x380','用途':'经0x40A0C0访问器读取存档入口许可'},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {'调用点':'0x0040CEE2','选择':'selected','源字段':'+0x384','用途':'经0x40A0D0访问器读取code12门控'},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {'调用点':'0x0040D374','选择':'selected','源字段':'+0x331','用途':'BGM选择器'},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {'调用点':'0x0044AC83','选择':'selected','源字段':'+0x2CD','用途':'EVE路径'}
        ],
        # 给当前结果字典的“全text原始四字节窗口数量”字段填写 `len(raw_windows),'全text_ModRM_disp32候选数量':len(literal),'Map主构造器内ModRM…`，这样导出的 JSON/表格能保留这一项证据。
        '全text原始四字节窗口数量':len(raw_windows),'全text_ModRM_disp32候选数量':len(literal),'Map主构造器内ModRM_disp32候选数量':len(constructor_hits),'Map主构造器内ModRM_disp32候选':constructor_hits,'全text_ModRM_disp32候选':literal,
        # 给当前结果字典的“结论边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '结论边界':[
            # 继续填写当前数据结构或参数列表中的一项：`'当前Map记录三段均全零；这是样本事实，不是格式语义。',`。
            '当前Map记录三段均全零；这是样本事实，不是格式语义。',
            # 继续填写当前数据结构或参数列表中的一项：`'0x408E80..0x4093F0主构造器读取相邻已知字段，但三段内没有满足ModRM/SIB disp32位置条件的直接位移候选。',`。
            '0x408E80..0x4093F0主构造器读取相邻已知字段，但三段内没有满足ModRM/SIB disp32位置条件的直接位移候选。',
            # 继续填写当前数据结构或参数列表中的一项：`'全.text同位移候选可能属于其他对象或栈帧；未证明Map source provenance者一律不得嫁接为Map字段consumer。原始四字节窗口另行计数，不冒充反汇编xref。',`。
            '全.text同位移候选可能属于其他对象或栈帧；未证明Map source provenance者一律不得嫁接为Map字段consumer。原始四字节窗口另行计数，不冒充反汇编xref。',
            # 继续填写当前数据结构或参数列表中的一项：`'已枚举0x40AE50的26个和0x40AE70的2个直接caller；可证明的源record直接出口只落在+0x2CD/+0x331/+0x380/+0x384，不落在三未知段。',`。
            '已枚举0x40AE50的26个和0x40AE70的2个直接caller；可证明的源record直接出口只落在+0x2CD/+0x331/+0x380/+0x384，不落在三未知段。',
            # 执行这一条实际代码步骤：`'仍不能排除计算后基址、整体复制、别名指针或间接调用；三段继续UNKNOWN/RAW-PRESERVE，禁止命名unused或padding。'`；它与上下相邻语句共同完成当前函数的小任务。
            '仍不能排除计算后基址、整体复制、别名指针或间接调用；三段继续UNKNOWN/RAW-PRESERVE，禁止命名unused或padding。'
        ]
    }


# 定义函数 `verify_entity_behavior_rpg`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_entity_behavior_rpg(path:Path)->Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""固化22：只读重放 Entity behavior/movement 静态证据。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """固化22：只读重放 Entity behavior/movement 静态证据。"""
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 把右侧 `lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B=lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))
    # 把右侧 `lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))` 计算得到的值保存到 `C`，后面的判断或输出会继续使用它。
    C=lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))
    # 执行这一条实际代码步骤：`B(0x0040A18D,'8A 4F 76 88 4E 57','Entity初始化把源record+0x76缓存到runtime+0x57，证明原始behavior_type被单独保存')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A18D,'8A 4F 76 88 4E 57','Entity初始化把源record+0x76缓存到runtime+0x57，证明原始behavior_type被单独保存')
    # 执行这一条实际代码步骤：`B(0x0040A193,'0F BE 57 77 83 C2 06 89 56 1C','record+0x77 signed值+6写runtime+0x1C')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A193,'0F BE 57 77 83 C2 06 89 56 1C','record+0x77 signed值+6写runtime+0x1C')
    # 执行这一条实际代码步骤：`B(0x0040A19D,'0F BE 47 78 83 C0 04 89 46 20','record+0x78 signed值+4写runtime+0x20')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A19D,'0F BE 47 78 83 C0 04 89 46 20','record+0x78 signed值+4写runtime+0x20')
    # 执行这一条实际代码步骤：`B(0x0040A1A7,'0F BE 4F 79 83 C1 09 89 4E 24','record+0x79 signed值+9写runtime+0x24')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A1A7,'0F BE 4F 79 83 C1 09 89 4E 24','record+0x79 signed值+9写runtime+0x24')
    # 执行这一条实际代码步骤：`B(0x0040A1B1,'0F BE 57 7A 83 C2 06 89 56 28','record+0x7A signed值+6写runtime+0x28')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A1B1,'0F BE 57 7A 83 C2 06 89 56 28','record+0x7A signed值+6写runtime+0x28')
    # 执行这一条实际代码步骤：`B(0x0040A1D9,'8A 47 76 3C 03 75 4D','只有behavior_type==3进入漫游边界初始化')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A1D9,'8A 47 76 3C 03 75 4D','只有behavior_type==3进入漫游边界初始化')
    # 执行这一条实际代码步骤：`B(0x0040A1E0,'8B 97 EF 00 00 00 8B 47 64 03 D0 89 97 F7 00 00 00','type3把serialized +0xEF与initial X…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A1E0,'8B 97 EF 00 00 00 8B 47 64 03 D0 89 97 F7 00 00 00','type3把serialized +0xEF与initial X相加写+0xF7上界')
    # 执行这一条实际代码步骤：`B(0x0040A1F4,'8B 88 F3 00 00 00 8B 50 68 03 CA 89 88 FB 00 00 00','type3把serialized +0xF3与initial Y…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A1F4,'8B 88 F3 00 00 00 8B 50 68 03 CA 89 88 FB 00 00 00','type3把serialized +0xF3与initial Y相加写+0xFB上界')
    # 执行这一条实际代码步骤：`B(0x004096BB,'8A 48 76 80 F9 01 74 09 80 F9 03','实体管理器移动/碰撞分支只接收behavior_type 1或3')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004096BB,'8A 48 76 80 F9 01 74 09 80 F9 03','实体管理器移动/碰撞分支只接收behavior_type 1或3')
    # 执行这一条实际代码步骤：`B(0x00409701,'80 7A 76 03 75 3C','管理器对type3进入专用重置/重新随机目的地路径')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00409701,'80 7A 76 03 75 3C','管理器对type3进入专用重置/重新随机目的地路径')
    # 执行这一条实际代码步骤：`B(0x0040A7E5,'80 78 76 03','移动更新对type3额外执行viewport与tick门控')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A7E5,'80 78 76 03','移动更新对type3额外执行viewport与tick门控')
    # 执行这一条实际代码步骤：`B(0x0040A83D,'0F BE 48 77 83 C1 06 89 4E 1C 0F BE 50 78 83 C2 04 89 56 20','移动更新每轮重载profile0两项修正')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A83D,'0F BE 48 77 83 C1 06 89 4E 1C 0F BE 50 78 83 C2 04 89 56 20','移动更新每轮重载profile0两项修正')
    # 执行这一条实际代码步骤：`B(0x0040A851,'0F BE 48 79 83 C1 09 89 4E 24','移动更新每轮重载profile1直线项')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A851,'0F BE 48 79 83 C1 09 89 4E 24','移动更新每轮重载profile1直线项')
    # 执行这一条实际代码步骤：`B(0x0040A85E,'0F BE 50 7A 8B 46 0C 83 C2 06 25 00 FF 00 00','移动更新重载profile1对角项并读取packed state bits8…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A85E,'0F BE 50 7A 8B 46 0C 83 C2 06 25 00 FF 00 00','移动更新重载profile1对角项并读取packed state bits8..15')
    # 执行这一条实际代码步骤：`B(0x0040A862,'8B 46 0C 83 C2 06 25 00 FF 00 00 89 56 28 F7 D8 1B C0 33 FF F7 D8','profile selector读…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A862,'8B 46 0C 83 C2 06 25 00 FF 00 00 89 56 28 F7 D8 1B C0 33 FF F7 D8','profile selector读取runtime+0x0C并把bits8..15是否非零布尔化为0/1')
    # 执行这一条实际代码步骤：`B(0x0040AAF7,'80 78 76 03','随机漫游函数要求behavior_type==3')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040AAF7,'80 78 76 03','随机漫游函数要求behavior_type==3')
    # 执行这一条实际代码步骤：`C(0x0040AB72,0x00451565,'type3随机X目标使用全局RNG')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040AB72,0x00451565,'type3随机X目标使用全局RNG')
    # 执行这一条实际代码步骤：`C(0x0040AB86,0x00451565,'type3随机Y目标使用全局RNG')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040AB86,0x00451565,'type3随机Y目标使用全局RNG')
    # 执行这一条实际代码步骤：`C(0x0040AB9A,0x00451565,'type3生成运行态0/1随机值')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040AB9A,0x00451565,'type3生成运行态0/1随机值')
    # 执行这一条实际代码步骤：`C(0x0040ABD3,0x00451565,'type3生成等待阈值')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0040ABD3,0x00451565,'type3生成等待阈值')
    # 执行这一条实际代码步骤：`B(0x0040AB4E,'8B B8 F7 00 00 00 8B 90 F3 00 00 00 55 8B A8 EF 00 00 00 2B FD 8B A8 FB 00 00 00 2B E…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040AB4E,'8B B8 F7 00 00 00 8B 90 F3 00 00 00 55 8B A8 EF 00 00 00 2B FD 8B A8 FB 00 00 00 2B EA','type3随机区间由EF/F3 lower与F7/FB upper派生')
    # 执行这一条实际代码步骤：`B(0x0040A4D4,'8A 47 76 8B 5E 08 3C 02 0F 95 C0 50','动作播放把behavior_type!=2作为loop布尔参数传下层')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A4D4,'8A 47 76 8B 5E 08 3C 02 0F 95 C0 50','动作播放把behavior_type!=2作为loop布尔参数传下层')
    # 执行这一条实际代码步骤：`B(0x00407450,'80 7C 24 10 00 74 05 89 41 14','SF2动作到末端时loop参数非零则回到clip start')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00407450,'80 7C 24 10 00 74 05 89 41 14','SF2动作到末端时loop参数非零则回到clip start')
    # 执行这一条实际代码步骤：`B(0x0040745C,'8A 81 80 00 00 00 4A 84 C0 89 51 14','loop参数为0时把帧夹在末帧，并检查一次性动作完成标志')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040745C,'8A 81 80 00 00 00 4A 84 C0 89 51 14','loop参数为0时把帧夹在末帧，并检查一次性动作完成标志')
    # 执行这一条实际代码步骤：`B(0x0040C83F,'C6 42 76 02','EVE动作范围handler可把实体behavior_type临时写2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040C83F,'C6 42 76 02','EVE动作范围handler可把实体behavior_type临时写2')
    # 执行这一条实际代码步骤：`B(0x0040F101,'C6 42 76 02','EVE proximity-trigger handler把实体behavior_type写2')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040F101,'C6 42 76 02','EVE proximity-trigger handler把实体behavior_type写2')
    # 执行这一条实际代码步骤：`B(0x0040A3B3,'8A 46 57 33 D2 88 41 76','运行态恢复路径把初始化缓存的原behavior_type写回record+0x76')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A3B3,'8A 46 57 33 D2 88 41 76','运行态恢复路径把初始化缓存的原behavior_type写回record+0x76')
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORA…`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,
        # 给当前结果字典的“Oracle哈希匹配”字段填写 `sha==MAP_TAIL_ORACLE_SHA256,'关键验证':checks,'关键验证数量':len(checks),`，这样导出的 JSON/表格能保留这一项证据。
        'Oracle哈希匹配':sha==MAP_TAIL_ORACLE_SHA256,'关键验证':checks,'关键验证数量':len(checks),
        # 给当前结果字典的“关键验证全部通过”字段填写 `all(x.get('status')=='PASS' for x in checks),`，这样导出的 JSON/表格能保留这一项证据。
        '关键验证全部通过':all(x.get('status')=='PASS' for x in checks),
        # 给当前结果字典的“behavior_type静态枚举”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'behavior_type静态枚举':{
            # 给当前结果字典的“0”字段填写 `'被动/静态类：不进入type1/3移动管理；SF2动作循环。作者原名未知。',`，这样导出的 JSON/表格能保留这一项证据。
            '0':'被动/静态类：不进入type1/3移动管理；SF2动作循环。作者原名未知。',
            # 给当前结果字典的“1”字段填写 `'外部驱动可移动类：进入移动/碰撞管理；无type3随机目的地生成；SF2动作循环。作者原名未知。',`，这样导出的 JSON/表格能保留这一项证据。
            '1':'外部驱动可移动类：进入移动/碰撞管理；无type3随机目的地生成；SF2动作循环。作者原名未知。',
            # 给当前结果字典的“2”字段填写 `'一次性/非循环动作覆盖模式：SF2动作到末帧停住；EVE 0x1A与0x8F路径可写入；随后存在恢复原behavior_type路径。作…`，这样导出的 JSON/表格能保留这一项证据。
            '2':'一次性/非循环动作覆盖模式：SF2动作到末帧停住；EVE 0x1A与0x8F路径可写入；随后存在恢复原behavior_type路径。作者原名未知。',
            # 给当前结果字典的“3”字段填写 `'自主随机漫游类：随机目标、随机等待、viewport/tick门控移动并参与移动/碰撞管理；SF2动作循环。作者原名未知。',`，这样导出的 JSON/表格能保留这一项证据。
            '3':'自主随机漫游类：随机目标、随机等待、viewport/tick门控移动并参与移动/碰撞管理；SF2动作循环。作者原名未知。',
        },
        # 给当前结果字典的“移动档静态公式”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        '移动档静态公式':{
            # 给当前结果字典的“profile0_cardinal”字段填写 `'signed(record+0x77)+6 -> runtime+0x1C',`，这样导出的 JSON/表格能保留这一项证据。
            'profile0_cardinal':'signed(record+0x77)+6 -> runtime+0x1C',
            # 给当前结果字典的“profile0_diagonal”字段填写 `'signed(record+0x78)+4 -> runtime+0x20',`，这样导出的 JSON/表格能保留这一项证据。
            'profile0_diagonal':'signed(record+0x78)+4 -> runtime+0x20',
            # 给当前结果字典的“profile1_cardinal”字段填写 `'signed(record+0x79)+9 -> runtime+0x24',`，这样导出的 JSON/表格能保留这一项证据。
            'profile1_cardinal':'signed(record+0x79)+9 -> runtime+0x24',
            # 给当前结果字典的“profile1_diagonal”字段填写 `'signed(record+0x7A)+6 -> runtime+0x28',`，这样导出的 JSON/表格能保留这一项证据。
            'profile1_diagonal':'signed(record+0x7A)+6 -> runtime+0x28',
            # 给当前结果字典的“profile_selector”字段填写 `'bool(runtime packed state +0x0C & 0xFF00)；0选profile0，1选profile1。不能无证…`，这样导出的 JSON/表格能保留这一项证据。
            'profile_selector':'bool(runtime packed state +0x0C & 0xFF00)；0选profile0，1选profile1。不能无证据命名为walk/run。',
        },
        # 给当前结果字典的“type3边界语义”字段填写 `'初始化将serialized EF/F3解释为围绕initial X/Y的负向extent，F7/FB解释为正向extent并改写成绝对…`，这样导出的 JSON/表格能保留这一项证据。
        'type3边界语义':'初始化将serialized EF/F3解释为围绕initial X/Y的负向extent，F7/FB解释为正向extent并改写成绝对upper；随机目标使用[lower, upper)整数区间。',
        # 给当前结果字典的“证据边界”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        '证据边界':[
            # 继续填写当前数据结构或参数列表中的一项：`'0/1/2/3的结构行为已经由直接consumer/writer闭合；作者原始枚举名称未知，禁止强起作者名。',`。
            '0/1/2/3的结构行为已经由直接consumer/writer闭合；作者原始枚举名称未知，禁止强起作者名。',
            # 执行一次赋值：把 `24、type1=14，仍无type2/type3 serialized正例；type2/type3语义来自同SHA RPG.exe直接consum…` 的结果放入左侧 `'当前已恢复38条Entity原始记录：顶层Map内22条 + Sys.dat内CURSOR/MAINNPC…` 对应的变量/字段。
            '当前已恢复38条Entity原始记录：顶层Map内22条 + Sys.dat内CURSOR/MAINNPC/SUBNPC共16条，type0=24、type1=14，仍无type2/type3 serialized正例；type2/type3语义来自同SHA RPG.exe直接consumer与EVE writer。',
            # 继续填写当前数据结构或参数列表中的一项：`'+0x77..+0x7A不再是全零：MAINNPC“小夏侯儀”给出-3/-2/-6/-4，按机器码ADD后两套profile均为3/2；这构成原始资源对ADD 6/4/9/6的独立正证据…`。
            '+0x77..+0x7A不再是全零：MAINNPC“小夏侯儀”给出-3/-2/-6/-4，按机器码ADD后两套profile均为3/2；这构成原始资源对ADD 6/4/9/6的独立正证据，但两档作者业务名未知。',
            # 继续填写当前数据结构或参数列表中的一项：`'+0x71/+0x72已有CURSOR.SCI的255/255正样本；全.text字面偏移审计未找到Entity直接consumer（仅+0x72两处均属于0x473-byte Map解…`。
            '+0x71/+0x72已有CURSOR.SCI的255/255正样本；全.text字面偏移审计未找到Entity直接consumer（仅+0x72两处均属于0x473-byte Map解析链），因此仍保持UNKNOWN，且不得把直接xref缺失等同unused。+0xE0/+0xEC/+0xED仍是后续优先consumer目标。',
        ]
    }

# 定义函数 `verify_map_background_mode_rpg`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_map_background_mode_rpg(path:Path)->Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""固化22：只读重放 Map +0x65/+0x6E 背景控制器数据流。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """固化22：只读重放 Map +0x65/+0x6E 背景控制器数据流。"""
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 把右侧 `lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B=lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))
    # 把右侧 `lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))` 计算得到的值保存到 `C`，后面的判断或输出会继续使用它。
    C=lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))
    # 执行这一条实际代码步骤：`B(0x00408F3C,'8A 4B 65','Map loader读取record+0x65')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00408F3C,'8A 4B 65','Map loader读取record+0x65')
    # 执行这一条实际代码步骤：`B(0x00408F47,'0F BE 53 6E','Map loader以signed byte读取record+0x6E')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00408F47,'0F BE 53 6E','Map loader以signed byte读取record+0x6E')
    # 执行这一条实际代码步骤：`C(0x00408F6A,0x00406660,'Map loader把+0x65/+0x6E及滚动参数传入背景控制器构造')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00408F6A,0x00406660,'Map loader把+0x65/+0x6E及滚动参数传入背景控制器构造')
    # 执行这一条实际代码步骤：`B(0x004067A0,'8B 41 20 57 85 C0','background controller update读取controller+0x20（来自Map+0x65）')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004067A0,'8B 41 20 57 85 C0','background controller update读取controller+0x20（来自Map+0x65）')
    # 执行这一条实际代码步骤：`B(0x004067AC,'83 F8 0A 0F 84 8B 00 00 00','mode10跳过相机相对基值重算')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004067AC,'83 F8 0A 0F 84 8B 00 00 00','mode10跳过相机相对基值重算')
    # 执行这一条实际代码步骤：`B(0x004067D3,'48 89 51 04 74 3D 48 75 64','mode1和mode2进入两个专门缩放分支，其余正值非10保留1:1')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004067D3,'48 89 51 04 74 3D 48 75 64','mode1和mode2进入两个专门缩放分支，其余正值非10保留1:1')
    # 执行这一条实际代码步骤：`B(0x004067E7,'B8 67 66 66 66 D1 E2 F7 EA C1 FA 02','mode2路径对14*delta使用signed /10 magic，等价trunc(7*de…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004067E7,'B8 67 66 66 66 D1 E2 F7 EA C1 FA 02','mode2路径对14*delta使用signed /10 magic，等价trunc(7*delta/5)')
    # 执行这一条实际代码步骤：`B(0x00406816,'8B 11 B8 67 66 66 66 F7 EA D1 FA','mode1路径对delta使用signed /5 magic')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00406816,'8B 11 B8 67 66 66 66 F7 EA D1 FA','mode1路径对delta使用signed /5 magic')
    # 执行这一条实际代码步骤：`B(0x004069E7,'8B 46 20 83 F8 02 74 6B 83 F8 0A 74 66','render对mode2和mode10走专门blit分支')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004069E7,'8B 46 20 83 F8 02 74 6B 83 F8 0A 74 66','render对mode2和mode10走专门blit分支')
    # 执行这一条实际代码步骤：`B(0x004069F9,'8B 5E 1C 85 DB','普通render读取controller+0x1C（来自Map+0x6E）')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004069F9,'8B 5E 1C 85 DB','普通render读取controller+0x1C（来自Map+0x6E）')
    # 执行这一条实际代码步骤：`B(0x00406A04,'0F 9F C3','Map+0x6E在普通render路径仅按>0转换为布尔参数')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00406A04,'0F 9F C3','Map+0x6E在普通render路径仅按>0转换为布尔参数')
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORA…`，这样导出的 JSON/表格能保留这一项证据。
        '工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,
        # 给当前结果字典的“Oracle哈希匹配”字段填写 `sha==MAP_TAIL_ORACLE_SHA256,'关键验证':checks,'关键验证数量':len(checks),`，这样导出的 JSON/表格能保留这一项证据。
        'Oracle哈希匹配':sha==MAP_TAIL_ORACLE_SHA256,'关键验证':checks,'关键验证数量':len(checks),
        # 给当前结果字典的“关键验证全部通过”字段填写 `all(x.get('status')=='PASS' for x in checks),`，这样导出的 JSON/表格能保留这一项证据。
        '关键验证全部通过':all(x.get('status')=='PASS' for x in checks),
        # 给当前结果字典的“Map加65静态变换”字段填写 `{`，这样导出的 JSON/表格能保留这一项证据。
        'Map加65静态变换':{
            # 给当前结果字典的“<=0”字段填写 `'不进入0x4067A0相机相对基值重算。',`，这样导出的 JSON/表格能保留这一项证据。
            '<=0':'不进入0x4067A0相机相对基值重算。',
            # 给当前结果字典的“1”字段填写 `'base_delta = signed trunc(camera_delta / 5)。',`，这样导出的 JSON/表格能保留这一项证据。
            '1':'base_delta = signed trunc(camera_delta / 5)。',
            # 给当前结果字典的“2”字段填写 `'base_delta = signed trunc(7 * camera_delta / 5)；render另走mode2专门blit。…`，这样导出的 JSON/表格能保留这一项证据。
            '2':'base_delta = signed trunc(7 * camera_delta / 5)；render另走mode2专门blit。',
            # 给当前结果字典的“10”字段填写 `'跳过相机相对基值重算；render另走mode10专门blit。',`，这样导出的 JSON/表格能保留这一项证据。
            '10':'跳过相机相对基值重算；render另走mode10专门blit。',
            # 给当前结果字典的“其他正值”字段填写 `'base_delta = camera_delta（1:1）；作者枚举业务名未知。',`，这样导出的 JSON/表格能保留这一项证据。
            '其他正值':'base_delta = camera_delta（1:1）；作者枚举业务名未知。',
        },
        # 给当前结果字典的“Map加6E静态语义”字段填写 `'signed byte存入controller+0x1C；普通背景render仅以>0转bool传给blit helper。mode2/…`，这样导出的 JSON/表格能保留这一项证据。
        'Map加6E静态语义':'signed byte存入controller+0x1C；普通背景render仅以>0转bool传给blit helper。mode2/10专门render分支不使用该bool。作者业务名未知。',
        # 给当前结果字典的“证据边界”字段填写 `'当前multimedia两条Map样本+0x65/+0x6E均为0；公式来自同SHA RPG.exe直接consumer，不能反推出作者…`，这样导出的 JSON/表格能保留这一项证据。
        '证据边界':'当前multimedia两条Map样本+0x65/+0x6E均为0；公式来自同SHA RPG.exe直接consumer，不能反推出作者枚举名称或未见模式的资源用途。'
    }

# 定义函数 `compare_interface_tianshu_resources`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def compare_interface_tianshu_resources(men0003:Path, men0009:Path)->Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""只读比较正常Interface导航资源MEN0003与禁用天書变体MEN0009。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """只读比较正常Interface导航资源MEN0003与禁用天書变体MEN0009。"""
    # 定义函数 `detail`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def detail(path:Path)->Dict[str,Any]:
        # 把右侧 `path.read_bytes(); b,wrap=sf2_decompress(raw); secs=sf2_sections(b)` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw=path.read_bytes(); b,wrap=sf2_decompress(raw); secs=sf2_sections(b)
        # 把右侧 `[]` 计算得到的值保存到 `frames`，后面的判断或输出会继续使用它。
        frames=[]
        # 开始循环 `rr in secs[0]["记录"]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for rr in secs[0]["记录"]:
            # 把右侧 `b[rr["偏移"]:rr["结束"]]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
            rec=b[rr["偏移"]:rr["结束"]]
            # 把右侧 `u16(rec,0x54)` 计算得到的值保存到 `n`，后面的判断或输出会继续使用它。
            n=u16(rec,0x54)
            # 把右侧 `[]` 计算得到的值保存到 `layers`，后面的判断或输出会继续使用它。
            layers=[]
            # 开始循环 `j in range(n)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
            for j in range(n):
                # 把右侧 `rec[0x5E+j*29:0x5E+(j+1)*29]` 计算得到的值保存到 `q`，后面的判断或输出会继续使用它。
                q=rec[0x5E+j*29:0x5E+(j+1)*29]
                # 把这一条新结果追加到列表 `layers`；不会覆盖前面已经收集的记录。
                layers.append({
                    # 给当前结果字典的“Section1图像”字段填写 `u16(q,0),"X":i16(q,2),"Y":i16(q,4),`，这样导出的 JSON/表格能保留这一项证据。
                    "Section1图像":u16(q,0),"X":i16(q,2),"Y":i16(q,4),
                    # 给当前结果字典的“渲染模式”字段填写 `q[0x0A],"字段06":i16(q,6),"字段08":i16(q,8),`，这样导出的 JSON/表格能保留这一项证据。
                    "渲染模式":q[0x0A],"字段06":i16(q,6),"字段08":i16(q,8),
                    # 给当前结果字典的“字段0B”字段填写 `i16(q,0x0B),"字段0D":i16(q,0x0D),`，这样导出的 JSON/表格能保留这一项证据。
                    "字段0B":i16(q,0x0B),"字段0D":i16(q,0x0D),
                })
            # 把这一条新结果追加到列表 `frames`；不会覆盖前面已经收集的记录。
            frames.append({"索引":rr["索引"],"子层":layers})
        # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return {
            # 给当前结果字典的“文件”字段填写 `str(path),"SHA256":hashlib.sha256(raw).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
            "文件":str(path),"SHA256":hashlib.sha256(raw).hexdigest(),
            # 给当前结果字典的“Section计数”字段填写 `[x["数量"] for x in secs],"Section0":frames,`，这样导出的 JSON/表格能保留这一项证据。
            "Section计数":[x["数量"] for x in secs],"Section0":frames,
        }
    # 把右侧 `detail(men0003); b=detail(men0009)` 计算得到的值保存到 `a`，后面的判断或输出会继续使用它。
    a=detail(men0003); b=detail(men0009)
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows=[]
    # 开始循环 `i in range(min(len(a["Section0"]),len(b["Section0"])))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(min(len(a["Section0"]),len(b["Section0"]))):
        # 把右侧 `a["Section0"][i]["子层"]; sb=b["Section0"][i]["子层"]` 计算得到的值保存到 `sa`，后面的判断或输出会继续使用它。
        sa=a["Section0"][i]["子层"]; sb=b["Section0"][i]["子层"]
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append({"Section0索引":i,"MEN0003子层":sa,"MEN0009子层":sb,"结构相同":sa==sb})
    # 把右侧 `rows[6]["MEN0003子层"] if len(rows)>6 else []` 计算得到的值保存到 `state6_a`，后面的判断或输出会继续使用它。
    state6_a=rows[6]["MEN0003子层"] if len(rows)>6 else []
    # 把右侧 `rows[6]["MEN0009子层"] if len(rows)>6 else []` 计算得到的值保存到 `state6_b`，后面的判断或输出会继续使用它。
    state6_b=rows[6]["MEN0009子层"] if len(rows)>6 else []
    # 把右侧 `{"Section1图像":8,"X":419,"Y":0,"渲染模式":2,"字段06":5,"字段08":0,"字段0B":16,"字段0D":…` 计算得到的值保存到 `expected_removed`，后面的判断或输出会继续使用它。
    expected_removed={"Section1图像":8,"X":419,"Y":0,"渲染模式":2,"字段06":5,"字段08":0,"字段0B":16,"字段0D":16}
    # 把右侧 `{` 计算得到的值保存到 `checks`，后面的判断或输出会继续使用它。
    checks={
        # 给当前结果字典的“两者Section0均为9”字段填写 `a["Section计数"][0]==9 and b["Section计数"][0]==9,`，这样导出的 JSON/表格能保留这一项证据。
        "两者Section0均为9": a["Section计数"][0]==9 and b["Section计数"][0]==9,
        # 给当前结果字典的“MEN0003_Section1为11”字段填写 `a["Section计数"][1]==11,`，这样导出的 JSON/表格能保留这一项证据。
        "MEN0003_Section1为11": a["Section计数"][1]==11,
        # 给当前结果字典的“MEN0009_Section1为10”字段填写 `b["Section计数"][1]==10,`，这样导出的 JSON/表格能保留这一项证据。
        "MEN0009_Section1为10": b["Section计数"][1]==10,
        # 给当前结果字典的“Section0索引0到5结构相同”字段填写 `all(x["结构相同"] for x in rows[:6]),`，这样导出的 JSON/表格能保留这一项证据。
        "Section0索引0到5结构相同": all(x["结构相同"] for x in rows[:6]),
        # 给当前结果字典的“MEN0003_Section0索引6含天書高亮候选层”字段填写 `expected_removed in state6_a,`，这样导出的 JSON/表格能保留这一项证据。
        "MEN0003_Section0索引6含天書高亮候选层": expected_removed in state6_a,
        # 给当前结果字典的“MEN0009_Section0索引6删除该层”字段填写 `expected_removed not in state6_b and len(state6_b)==2,`，这样导出的 JSON/表格能保留这一项证据。
        "MEN0009_Section0索引6删除该层": expected_removed not in state6_b and len(state6_b)==2,
        # 给当前结果字典的“Section0索引7到8保持三层”字段填写 `all(len(rows[i]["MEN0003子层"])==3 and len(rows[i]["MEN0009子层"])==3 for…`，这样导出的 JSON/表格能保留这一项证据。
        "Section0索引7到8保持三层": all(len(rows[i]["MEN0003子层"])==3 and len(rows[i]["MEN0009子层"])==3 for i in (7,8)),
    }
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,"MEN0003":a,"MEN0009":b,"逐Section0比较":rows,`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本":TOOL_VERSION,"MEN0003":a,"MEN0009":b,"逐Section0比较":rows,
        # 给当前结果字典的“关键检查”字段填写 `checks,"全部通过":all(checks.values()),`，这样导出的 JSON/表格能保留这一项证据。
        "关键检查":checks,"全部通过":all(checks.values()),
        # 给当前结果字典的“结构结论”字段填写 `"MEN0009与MEN0003共用9个Section0业务状态；MEN0009在Section0 index6删除MEN0003的第三高…`，这样导出的 JSON/表格能保留这一项证据。
        "结构结论":"MEN0009与MEN0003共用9个Section0业务状态；MEN0009在Section0 index6删除MEN0003的第三高亮子层并少1张Section1图。结合RPG.exe中业务state7→Section0 index6、state7命中区X=0x1A1与随包真实实图，可作为天書禁用资源变体的机器结构证据。",
        # 给当前结果字典的“视觉证据边界”字段填写 `"‘天書’文字、正常亮度/压暗效果来自随包导出的真实Section1 PNG人工可视核对；本JSON不使用OCR自动猜字。"`，这样导出的 JSON/表格能保留这一项证据。
        "视觉证据边界":"‘天書’文字、正常亮度/压暗效果来自随包导出的真实Section1 PNG人工可视核对；本JSON不使用OCR自动猜字。"
    }


# 定义函数 `verify_dddes_display_timer_rpg`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_dddes_display_timer_rpg(path:Path)->Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""固化23：重放 DDDES 18-byte header / 52-byte records、显示与MMTimer静态证据。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """固化23：重放 DDDES 18-byte header / 52-byte records、显示与MMTimer静态证据。"""
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 把右侧 `lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B=lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))
    # 把右侧 `lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))` 计算得到的值保存到 `C`，后面的判断或输出会继续使用它。
    C=lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))
    # 执行这一条实际代码步骤：`B(0x00401511,'8D 51 11 89 56 08 42 2B DA 89 56 0C 03 D8 B8 4F EC C4 4E F7 E3 C1 EA 04 89 56 10','DD…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00401511,'8D 51 11 89 56 08 42 2B DA 89 56 0C 03 D8 B8 4F EC C4 4E F7 E3 C1 EA 04 89 56 10','DDDES：+0x11保存header末字节指针；inc后得到+0x12记录区；magic divide计算(file_size-0x12)/0x34记录数')
    # 执行这一条实际代码步骤：`B(0x0040152C,'8A 19 8B 0D C0 F6 89 00 84 DB 0F 97 C0 88 41 30','header+0x00归一成bool并写显示对象+0x30')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040152C,'8A 19 8B 0D C0 F6 89 00 84 DB 0F 97 C0 88 41 30','header+0x00归一成bool并写显示对象+0x30')
    # 执行这一条实际代码步骤：`B(0x00401544,'8B 42 01 89 41 6C','header+0x01 u32写显示对象+0x6C')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00401544,'8B 42 01 89 41 6C','header+0x01 u32写显示对象+0x6C')
    # 执行这一条实际代码步骤：`B(0x00401552,'8B 42 05 89 41 70','header+0x05 u32写显示对象+0x70')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00401552,'8B 42 05 89 41 70','header+0x05 u32写显示对象+0x70')
    # 执行这一条实际代码步骤：`B(0x0040155F,'8B 48 28 8B 52 09 89 51 04','header+0x09 u32写display-mode+0x04宽')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040155F,'8B 48 28 8B 52 09 89 51 04','header+0x09 u32写display-mode+0x04宽')
    # 执行这一条实际代码步骤：`B(0x00401570,'8B 51 28 8B 40 0D 89 42 08','header+0x0D u32写display-mode+0x08高')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00401570,'8B 51 28 8B 40 0D 89 42 08','header+0x0D u32写display-mode+0x08高')
    # 执行这一条实际代码步骤：`B(0x00401584,'8B 4E 08 8B 06 33 D2 8A 11 8B 0D B4 F6 46 00 52 50','header+0x11 byte零扩展后作为第二参数进入定时器包…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00401584,'8B 4E 08 8B 06 33 D2 8A 11 8B 0D B4 F6 46 00 52 50','header+0x11 byte零扩展后作为第二参数进入定时器包装')
    # 执行这一条实际代码步骤：`C(0x00401595,0x00404D40,'DDDES初始化调用MMTimer包装0x404D40')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00401595,0x00404D40,'DDDES初始化调用MMTimer包装0x404D40')
    # 执行一次赋值：把 `core_height + 2*vertical_extra')` 的结果放入左侧 `B(0x00405964,'8B 46 28 8B 4E 70 8B 50 08 8D 0C 4A','ba…` 对应的变量/字段。
    B(0x00405964,'8B 46 28 8B 4E 70 8B 50 08 8D 0C 4A','backing_height = core_height + 2*vertical_extra')
    # 执行一次赋值：把 `core_width + 2*horizontal_extra')` 的结果放入左侧 `B(0x00405970,'8B 56 6C 89 4E 5C 8B 48 04 8D 14 51','ba…` 对应的变量/字段。
    B(0x00405970,'8B 56 6C 89 4E 5C 8B 48 04 8D 14 51','backing_width = core_width + 2*horizontal_extra')
    # 执行这一条实际代码步骤：`B(0x0040198A,'3D 05 01 00 00','WndProc识别WM_SYSKEYUP(0x0105)')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040198A,'3D 05 01 00 00','WndProc识别WM_SYSKEYUP(0x0105)')
    # 执行这一条实际代码步骤：`B(0x00401AB0,'83 FF 0D','WM_SYSKEYUP下识别VK_RETURN(0x0D)')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00401AB0,'83 FF 0D','WM_SYSKEYUP下识别VK_RETURN(0x0D)')
    # 执行这一条实际代码步骤：`C(0x00401ABF,0x00401610,'Alt+Enter进入显示模式翻转函数')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00401ABF,0x00401610,'Alt+Enter进入显示模式翻转函数')
    # 执行这一条实际代码步骤：`B(0x00401620,'8A 48 30 84 C9 0F 94 C1 88 48 30','显示对象+0x30布尔取反')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00401620,'8A 48 30 84 C9 0F 94 C1 88 48 30','显示对象+0x30布尔取反')
    # 执行这一条实际代码步骤：`B(0x00405C5B,'8A 46 30 84 C0 0F 84 3E 02 00 00','显示重建按+0x30分流全屏/窗口路径')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00405C5B,'8A 46 30 84 C0 0F 84 3E 02 00 00','显示重建按+0x30分流全屏/窗口路径')
    # 执行一次赋值：把 `常量100/配置参数')` 的结果放入左侧 `B(0x00404A75,'8B 3D 0C 85 46 00 33 D2 8B C7 89 35 A0 F…` 对应的变量/字段。
    B(0x00404A75,'8B 3D 0C 85 46 00 33 D2 8B C7 89 35 A0 F6 46 00 F7 F6 33 D2 A3 90 F6 46 00','定时器threshold=常量100/配置参数')
    # 执行一次赋值：把 `1000/常量100，即约10ms')` 的结果放入左侧 `B(0x00404A8E,'B8 E8 03 00 00 F7 F7 A3 B8 F6 46 00','基础…` 对应的变量/字段。
    B(0x00404A8E,'B8 E8 03 00 00 F7 F7 A3 B8 F6 46 00','基础周期=1000/常量100，即约10ms')
    # 执行这一条实际代码步骤：`B(0x00404CFF,'68 13 01 00 00','MMTimer回调达到阈值后准备WM_TIMER(0x0113)')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00404CFF,'68 13 01 00 00','MMTimer回调达到阈值后准备WM_TIMER(0x0113)')
    # 执行这一条实际代码步骤：`B(0x00404D05,'FF 15 90 01 46 00','通过USER32 PostMessageA投递WM_TIMER')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00404D05,'FF 15 90 01 46 00','通过USER32 PostMessageA投递WM_TIMER')
    # 把 `{'工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {'工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'Oracle哈希匹配':sha==MAP_TAIL_ORACLE_SHA256,'关键验证':checks,'关键验证数量':len(checks),'关键验证全部通过':all(x.get('status')=='PASS' for x in checks),'冻结边界':['DDDES不是70-byte固定record；原70-byte样本恰好是18-byte header + 1×52-byte record。','+0x00可定义为初始全屏/窗口显示状态；+0x01/+0x05为后备缓冲每侧扩展；+0x09/+0x0D为核心显示宽高；+0x11为主逻辑定时/WM_TIMER触发频率参数。','52-byte record区由mode=2 Loader返回给DDDES对象并计数；已完成0x4025B0 mode=2完整数据流和全局对象0x46F380直接xref审计，当前本构建静态可达直接路径未发现record consumer，并与硬编码24/24/Font24形成强Legacy/硬编码旁路证据；仍不能形式化排除未知间接指针复制/间接调用。']}

# 定义函数 `verify_entity_event_bindings_rpg`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_entity_event_bindings_rpg(path:Path)->Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""固化23：重放 Entity +EA/+EB 双事件绑定静态协议。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """固化23：重放 Entity +EA/+EB 双事件绑定静态协议。"""
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 把右侧 `lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B=lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))
    # 把右侧 `lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))` 计算得到的值保存到 `C`，后面的判断或输出会继续使用它。
    C=lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))
    # 执行这一条实际代码步骤：`B(0x00409AA1,'8B 48 70 8A 51 74 84 D2','接触/碰撞链经runtime+0x70取得Entity source并读取+0x74')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00409AA1,'8B 48 70 8A 51 74 84 D2','接触/碰撞链经runtime+0x70取得Entity source并读取+0x74')
    # 执行这一条实际代码步骤：`B(0x00409AAB,'8A 91 EA 00 00 00','同链直接读取Entity source+0xEA事件ID')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00409AAB,'8A 91 EA 00 00 00','同链直接读取Entity source+0xEA事件ID')
    # 执行这一条实际代码步骤：`C(0x00409B23,0x0040B230,'接触触发条件命中后把+EA事件ID送入Event激活函数')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00409B23,0x0040B230,'接触触发条件命中后把+EA事件ID送入Event激活函数')
    # 执行一次赋值：把 `1')` 的结果放入左侧 `B(0x0040F0DB,'88 5A 74','EVE近接触发配置写Entity+0x74` 对应的变量/字段。
    B(0x0040F0DB,'88 5A 74','EVE近接触发配置写Entity+0x74=1')
    # 执行这一条实际代码步骤：`B(0x0040F0E4,'8A 49 18 88 8A EA 00 00 00','EVE近接触发配置把脚本参数写入Entity+0xEA')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040F0E4,'8A 49 18 88 8A EA 00 00 00','EVE近接触发配置把脚本参数写入Entity+0xEA')
    # 执行这一条实际代码步骤：`B(0x0040F0ED,'8B 50 70 C6 82 EB 00 00 00 00','同一路径明确清Entity+0xEB')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040F0ED,'8B 50 70 C6 82 EB 00 00 00 00','同一路径明确清Entity+0xEB')
    # 执行这一条实际代码步骤：`B(0x00409978,'8B 48 70 8A 91 EB 00 00 00 52','主动交互选择链直接读取Entity source+0xEB事件ID')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00409978,'8B 48 70 8A 91 EB 00 00 00 52','主动交互选择链直接读取Entity source+0xEB事件ID')
    # 执行这一条实际代码步骤：`C(0x00409982,0x0040B230,'主动交互链把+EB事件ID送入Event激活函数')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00409982,0x0040B230,'主动交互链把+EB事件ID送入Event激活函数')
    # 执行这一条实际代码步骤：`B(0x0040A2EB,'8A 4E 54 89 56 10 89 46 0C 84 C9 8B 4E 70','交互生命周期检查runtime+0x54临时交互状态并取得Entity sourc…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A2EB,'8A 4E 54 89 56 10 89 46 0C 84 C9 8B 4E 70','交互生命周期检查runtime+0x54临时交互状态并取得Entity source')
    # 执行这一条实际代码步骤：`B(0x0040A307,'8B 3D 08 F8 89 00 33 D2 8A 91 EB 00 00 00 3B FA','读取全局active Event ID 0x0089F808并与Ent…`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A307,'8B 3D 08 F8 89 00 33 D2 8A 91 EB 00 00 00 3B FA','读取全局active Event ID 0x0089F808并与Entity+0xEB比较')
    # 执行这一条实际代码步骤：`B(0x0040A3B3,'8A 46 57 33 D2 88 41 76','事件结束/不匹配后把runtime+0x57保存的原behavior恢复到source+0x76')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A3B3,'8A 46 57 33 D2 88 41 76','事件结束/不匹配后把runtime+0x57保存的原behavior恢复到source+0x76')
    # 执行这一条实际代码步骤：`B(0x0040A3C1,'C6 46 54 00 8A 91 03 01 00 00 C1 E2 10','清临时交互状态并从source+0x103恢复默认方向代码')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x0040A3C1,'C6 46 54 00 8A 91 03 01 00 00 C1 E2 10','清临时交互状态并从source+0x103恢复默认方向代码')
    # 把 `{'工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {'工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'Oracle哈希匹配':sha==MAP_TAIL_ORACLE_SHA256,'关键验证':checks,'关键验证数量':len(checks),'关键验证全部通过':all(x.get('status')=='PASS' for x in checks),'冻结边界':['+EA：接触/碰撞/重叠触发路径使用的Event ID；+0x74参与该特殊触发路径，但普通碰撞分支仍可能在+0x74==0时存在，禁止简化成“+74总门控碰撞”。','+EB：主动/手动交互选择路径使用的Event ID；与临时朝向/behavior状态生命周期绑定。','“接触触发/主动交互”是中性consumer语义，不等于已恢复作者原始字段名或具体输入设备UX。']}

# 定义函数 `map_tail_samples`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def map_tail_samples(files:List[Path])->Dict[str,Any]:
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows=[]
    # 开始循环 `p in files`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for p in files:
        # 把右侧 `p.read_bytes()` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
        b=p.read_bytes()
        # 把右侧 `classify_sci(p)` 计算得到的值保存到 `cls`，后面的判断或输出会继续使用它。
        cls=classify_sci(p)
        # 检查条件 `cls['类型']!='地图SCI'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if cls['类型']!='地图SCI':
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            rows.append({"文件":str(p),"大小":len(b),"分类":cls['类型'],"状态":"SKIP_NON_MAP"}); continue
        # 开始循环 `i,o in enumerate(range(0,len(b),MAP_RECORD_SIZE))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i,o in enumerate(range(0,len(b),MAP_RECORD_SIZE)):
            # 把右侧 `b[o:o+MAP_RECORD_SIZE]` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=b[o:o+MAP_RECORD_SIZE]
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            rows.append({
                # 给当前结果字典的“文件”字段填写 `str(p),"SHA256":hashlib.sha256(b).hexdigest(),"记录索引":i,`，这样导出的 JSON/表格能保留这一项证据。
                "文件":str(p),"SHA256":hashlib.sha256(b).hexdigest(),"记录索引":i,
                # 给当前结果字典的“加380_u32”字段填写 `u32(r,0x380),"加380等于1_天書存档入口许可":u32(r,0x380)==1,"加384_u32":u32(r,0x38…`，这样导出的 JSON/表格能保留这一项证据。
                "加380_u32":u32(r,0x380),"加380等于1_天書存档入口许可":u32(r,0x380)==1,"加384_u32":u32(r,0x384),"加384等于1":u32(r,0x384)==1,
                # 给当前结果字典的“加388到结尾非零字节数”字段填写 `sum(1 for x in r[0x388:] if x),`，这样导出的 JSON/表格能保留这一项证据。
                "加388到结尾非零字节数":sum(1 for x in r[0x388:] if x),
            })
    # 把右侧 `[r for r in rows if '加380_u32' in r]` 计算得到的值保存到 `maprows`，后面的判断或输出会继续使用它。
    maprows=[r for r in rows if '加380_u32' in r]
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,"输入文件数":len(files),"Map记录数":len(maprows),"记录":rows,`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本":TOOL_VERSION,"输入文件数":len(files),"Map记录数":len(maprows),"记录":rows,
        # 给当前结果字典的“统计”字段填写 `{"加380非零记录数":sum(1 for r in maprows if r['加380_u32']!=0),"加380等于1_天書存…`，这样导出的 JSON/表格能保留这一项证据。
        "统计":{"加380非零记录数":sum(1 for r in maprows if r['加380_u32']!=0),"加380等于1_天書存档入口许可记录数":sum(1 for r in maprows if r['加380等于1_天書存档入口许可']),"加384等于1记录数":sum(1 for r in maprows if r['加384等于1']),"加388后存在非零字节记录数":sum(1 for r in maprows if r['加388到结尾非零字节数']>0)},
        # 给当前结果字典的“边界”字段填写 `"样本值仅描述输入样本，不可代替RPG.exe consumer证据；当前样本没有+0x380==1天書（存档）入口许可正例，也没有+0x…`，这样导出的 JSON/表格能保留这一项证据。
        "边界":"样本值仅描述输入样本，不可代替RPG.exe consumer证据；当前样本没有+0x380==1天書（存档）入口许可正例，也没有+0x384==1 code12门控正例。"
    }

# 定义函数 `verify_saf_boundary`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_saf_boundary(exe_path:Path,saf_path:Path)->Dict[str,Any]:
    # 把右侧 `_PEImage(exe_path)` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(exe_path)
    # 把右侧 `hashlib.sha256(pe.data).hexdigest()` 计算得到的值保存到 `exe_sha`，后面的判断或输出会继续使用它。
    exe_sha=hashlib.sha256(pe.data).hexdigest()
    # 把右侧 `saf_path.read_bytes()` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
    raw=saf_path.read_bytes()
    # 检查条件 `len(raw)<SF2_HEADER_SIZE+8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(raw)<SF2_HEADER_SIZE+8:
        # 检测到不能继续的情况，主动抛出异常 `ValueError("SAF样本不足0x42C4字节")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("SAF样本不足0x42C4字节")
    # 检查条件 `raw[:4]!=b"SAF\x05"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if raw[:4]!=b"SAF\x05":
        # 检测到不能继续的情况，主动抛出异常 `ValueError("样本不是SAF\\x05魔数")`，让上层明确知道数据或参数不符合要求。
        raise ValueError("样本不是SAF\\x05魔数")
    # 把右侧 `raw[0x0B]` 计算得到的值保存到 `flag`，后面的判断或输出会继续使用它。
    flag=raw[0x0B]
    # 把右侧 `u32(raw,SF2_HEADER_SIZE)` 计算得到的值保存到 `final1`，后面的判断或输出会继续使用它。
    final1=u32(raw,SF2_HEADER_SIZE)
    # 把右侧 `u32(raw,SF2_HEADER_SIZE+4)` 计算得到的值保存到 `final2`，后面的判断或输出会继续使用它。
    final2=u32(raw,SF2_HEADER_SIZE+4)
    # 把右侧 `None` 计算得到的值保存到 `tail_error`，后面的判断或输出会继续使用它。
    tail_error=None
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 把右侧 `zlib.decompress(raw[SF2_HEADER_SIZE+8:])` 计算得到的值保存到 `tail`，后面的判断或输出会继续使用它。
        tail=zlib.decompress(raw[SF2_HEADER_SIZE+8:])
        # 把右侧 `bytearray(raw[:SF2_HEADER_SIZE]+tail)` 计算得到的值保存到 `rebuilt`，后面的判断或输出会继续使用它。
        rebuilt=bytearray(raw[:SF2_HEADER_SIZE]+tail)
        # 把右侧 `0` 计算得到的值保存到 `rebuilt[0x0B]`，后面的判断或输出会继续使用它。
        rebuilt[0x0B]=0
    # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception as e:
        # 把右侧 `b""; rebuilt=bytearray(); tail_error=str(e)` 计算得到的值保存到 `tail`，后面的判断或输出会继续使用它。
        tail=b""; rebuilt=bytearray(); tail_error=str(e)
    # 把右侧 `[]` 计算得到的值保存到 `checks`，后面的判断或输出会继续使用它。
    checks=[]
    # 把右侧 `lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B=lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))
    # 执行这一条实际代码步骤：`B(0x00428C21,"6A 03 68 08 A6 46 00 56 E8 92 41 03 00","SF2构造器以长度3比较载荷魔数与字符串SF2")`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00428C21,"6A 03 68 08 A6 46 00 56 E8 92 41 03 00","SF2构造器以长度3比较载荷魔数与字符串SF2")
    # 执行这一条实际代码步骤：`B(0x00428C31,"85 C0 75 22","SF2三字节比较非零时跳到拒绝分支0x428C57")`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00428C31,"85 C0 75 22","SF2三字节比较非零时跳到拒绝分支0x428C57")
    # 执行这一条实际代码步骤：`B(0x00428CCB,"8A 45 0B 3C 6F 75 7B","通过魔数后才检查+0x0B是否0x6F")`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00428CCB,"8A 45 0B 3C 6F 75 7B","通过魔数后才检查+0x0B是否0x6F")
    # 执行这一条实际代码步骤：`B(0x00428CD2,"8B 85 BC 42 00 00 8B 8D C0 42 00 00 3B C1","0x6F路径比较+0x42BC/+0x42C0两个最终尺寸")`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00428CD2,"8B 85 BC 42 00 00 8B 8D C0 42 00 00 3B C1","0x6F路径比较+0x42BC/+0x42C0两个最终尺寸")
    # 执行这一条实际代码步骤：`B(0x00428CF6,"81 C5 C4 42 00 00","0x6F解压输入从+0x42C4开始")`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00428CF6,"81 C5 C4 42 00 00","0x6F解压输入从+0x42C4开始")
    # 把右侧 `[]` 计算得到的值保存到 `exact_name_hits`，后面的判断或输出会继续使用它。
    exact_name_hits=[]
    # 把右侧 `b"ITF000.SF2\0"` 计算得到的值保存到 `needle`，后面的判断或输出会继续使用它。
    needle=b"ITF000.SF2\0"
    # 把右侧 `0` 计算得到的值保存到 `start`，后面的判断或输出会继续使用它。
    start=0
    # 只要条件 `True` 仍然成立，就重复执行下面的循环体；循环体必须最终改变条件或主动退出。
    while True:
        # 把右侧 `pe.data.find(needle,start)` 计算得到的值保存到 `pos`，后面的判断或输出会继续使用它。
        pos=pe.data.find(needle,start)
        # 检查条件 `pos<0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if pos<0: break
        # 执行一次赋值：把 `pos+1` 的结果放入左侧 `exact_name_hits.append(pos); start` 对应的变量/字段。
        exact_name_hits.append(pos); start=pos+1
    # 把 `{` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {
        # 给当前结果字典的“工具版本”字段填写 `TOOL_VERSION,`，这样导出的 JSON/表格能保留这一项证据。
        "工具版本":TOOL_VERSION,
        # 给当前结果字典的“RPG.exe_SHA256”字段填写 `exe_sha,`，这样导出的 JSON/表格能保留这一项证据。
        "RPG.exe_SHA256":exe_sha,
        # 给当前结果字典的“SAF文件”字段填写 `str(saf_path),`，这样导出的 JSON/表格能保留这一项证据。
        "SAF文件":str(saf_path),
        # 给当前结果字典的“SAF_SHA256”字段填写 `hashlib.sha256(raw).hexdigest(),`，这样导出的 JSON/表格能保留这一项证据。
        "SAF_SHA256":hashlib.sha256(raw).hexdigest(),
        # 给当前结果字典的“原始大小”字段填写 `len(raw),`，这样导出的 JSON/表格能保留这一项证据。
        "原始大小":len(raw),
        # 给当前结果字典的“原始魔数”字段填写 `raw[:4].hex(" "),`，这样导出的 JSON/表格能保留这一项证据。
        "原始魔数":raw[:4].hex(" "),
        # 给当前结果字典的“压缩标志_0x0B”字段填写 `flag,`，这样导出的 JSON/表格能保留这一项证据。
        "压缩标志_0x0B":flag,
        # 给当前结果字典的“声明最终大小1”字段填写 `final1,`，这样导出的 JSON/表格能保留这一项证据。
        "声明最终大小1":final1,
        # 给当前结果字典的“声明最终大小2”字段填写 `final2,`，这样导出的 JSON/表格能保留这一项证据。
        "声明最终大小2":final2,
        # 给当前结果字典的“zlib解压错误”字段填写 `tail_error,`，这样导出的 JSON/表格能保留这一项证据。
        "zlib解压错误":tail_error,
        # 给当前结果字典的“zlib尾大小”字段填写 `len(tail),`，这样导出的 JSON/表格能保留这一项证据。
        "zlib尾大小":len(tail),
        # 给当前结果字典的“重建大小”字段填写 `len(rebuilt),`，这样导出的 JSON/表格能保留这一项证据。
        "重建大小":len(rebuilt),
        # 给当前结果字典的“重建大小匹配”字段填写 `len(rebuilt)==final1==final2,`，这样导出的 JSON/表格能保留这一项证据。
        "重建大小匹配":len(rebuilt)==final1==final2,
        # 给当前结果字典的“重建后魔数”字段填写 `bytes(rebuilt[:4]).hex(" ") if rebuilt else None,`，这样导出的 JSON/表格能保留这一项证据。
        "重建后魔数":bytes(rebuilt[:4]).hex(" ") if rebuilt else None,
        # 给当前结果字典的“重建后仍为SAF魔数”字段填写 `bytes(rebuilt[:4])==b"SAF\x05" if rebuilt else False,`，这样导出的 JSON/表格能保留这一项证据。
        "重建后仍为SAF魔数":bytes(rebuilt[:4])==b"SAF\x05" if rebuilt else False,
        # 给当前结果字典的“EXE内精确ITF000.SF2_NUL字面数”字段填写 `len(exact_name_hits),`，这样导出的 JSON/表格能保留这一项证据。
        "EXE内精确ITF000.SF2_NUL字面数":len(exact_name_hits),
        # 给当前结果字典的“标准SF2构造器静态断言”字段填写 `checks,`，这样导出的 JSON/表格能保留这一项证据。
        "标准SF2构造器静态断言":checks,
        # 给当前结果字典的“断言数”字段填写 `len(checks),`，这样导出的 JSON/表格能保留这一项证据。
        "断言数":len(checks),
        # 给当前结果字典的“断言全部通过”字段填写 `all(x["status"]=="PASS" for x in checks),`，这样导出的 JSON/表格能保留这一项证据。
        "断言全部通过":all(x["status"]=="PASS" for x in checks),
        # 给当前结果字典的“当前结论”字段填写 `"SAF样本共用SF2的0x6F/双尺寸/zlib包装外层，但解压后魔数仍为SAF\\x05。标准SF2构造器先比较前三字节SF2，因而对…`，这样导出的 JSON/表格能保留这一项证据。
        "当前结论":"SAF样本共用SF2的0x6F/双尺寸/zlib包装外层，但解压后魔数仍为SAF\\x05。标准SF2构造器先比较前三字节SF2，因而对该样本走拒绝分支。",
        # 给当前结果字典的“证据边界”字段填写 `"EXE中无精确ITF000.SF2\\0字面不能排除动态构造名称；本轮仍未证明独立SAF Reader、真实请求源或游戏用途。"`，这样导出的 JSON/表格能保留这一项证据。
        "证据边界":"EXE中无精确ITF000.SF2\\0字面不能排除动态构造名称；本轮仍未证明独立SAF Reader、真实请求源或游戏用途。"
    }

# 定义函数 `verify_itf_callgraph`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_itf_callgraph(path:Path,sci_files:List[Path])->Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""固化28：重放 ITF/SAF 静态请求边界与解压入口穷举。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """固化28：重放 ITF/SAF 静态请求边界与解压入口穷举。"""
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); text_va,text=pe.s…` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); text_va,text=pe.section('.text')
    # 定义函数 `direct_calls`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def direct_calls(target:int)->List[str]:
        # 把右侧 `[]` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
        out=[]
        # 开始循环 `i in range(max(0,len(text)-4))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i in range(max(0,len(text)-4)):
            # 检查条件 `text[i]!=0xE8`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if text[i]!=0xE8: continue
            # 把右侧 `text_va+i; rel=struct.unpack_from('<i',text,i+1)[0]` 计算得到的值保存到 `src`，后面的判断或输出会继续使用它。
            src=text_va+i; rel=struct.unpack_from('<i',text,i+1)[0]
            # 检查条件 `((src+5+rel)&0xffffffff)==target: out.append(f'0x{src`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ((src+5+rel)&0xffffffff)==target: out.append(f'0x{src:08X}')
        # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return out
    # 定义函数 `cstrings`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def cstrings(prefix:bytes)->List[Dict[str,str]]:
        # 把右侧 `[]; p=0` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
        rows=[]; p=0
        # 只要条件 `True` 仍然成立，就重复执行下面的循环体；循环体必须最终改变条件或主动退出。
        while True:
            # 把右侧 `pe.data.find(prefix,p)` 计算得到的值保存到 `i`，后面的判断或输出会继续使用它。
            i=pe.data.find(prefix,p)
            # 检查条件 `i<0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if i<0: break
            # 把右侧 `pe.data.find(b'\0',i); e=len(pe.data) if e<0 else e` 计算得到的值保存到 `e`，后面的判断或输出会继续使用它。
            e=pe.data.find(b'\0',i); e=len(pe.data) if e<0 else e
            # 执行一次赋值：把 `e+1` 的结果放入左侧 `rows.append({'VA':f'0x{pe.image_base+i:08X}','路径':pe.d…` 对应的变量/字段。
            rows.append({'VA':f'0x{pe.image_base+i:08X}','路径':pe.data[i:e].decode('ascii','replace')}); p=e+1
        # 把 `rows` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return rows
    # 把右侧 `direct_calls(0x004070D0); zcalls=direct_calls(0x0044EAD0)` 计算得到的值保存到 `loader`，后面的判断或输出会继续使用它。
    loader=direct_calls(0x004070D0); zcalls=direct_calls(0x0044EAD0)
    # 把右侧 `['0x00401D8C','0x0040391C','0x00403967','0x004039B3','0x00403D9F','0x00403…` 计算得到的值保存到 `expected`，后面的判断或输出会继续使用它。
    expected=['0x00401D8C','0x0040391C','0x00403967','0x004039B3','0x00403D9F','0x00403DF9','0x00408F09','0x00409010','0x00409074','0x0040A28B']
    # 把右侧 `[` 计算得到的值保存到 `calls`，后面的判断或输出会继续使用它。
    calls=[
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x00401D8C','来源':'固定字符串','资源':'F-Fascia.SF2'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x0040391C','来源':'固定字符串','资源':'F-Talk.SF2'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x00403967','来源':'固定字符串','资源':'F-Float.SF2'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x004039B3','来源':'固定字符串','资源':'F-YESNO.SF2'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x00403D9F','来源':'格式化字符串','资源':'%d-2.SF2'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x00403DF9','来源':'固定字符串','资源':'F-Name.SF2'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x00408F09','来源':'Map SCI字段','资源':'record+0x74'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x00409010','来源':'Map SCI字段','资源':'record+0xD9'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x00409074','来源':'Map SCI字段','资源':'record+0x13D'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'调用点':'0x0040A28B','来源':'Entity SCI字段','资源':'record+0x7B'}]
    # 把右侧 `cstrings(b'Fight\\ITF\\'); bare=[x for x in itf if x['路径'].lower()==r'figh…` 计算得到的值保存到 `itf`，后面的判断或输出会继续使用它。
    itf=cstrings(b'Fight\\ITF\\'); bare=[x for x in itf if x['路径'].lower()==r'fight\itf\itf000.sf2']
    # 把右侧 `[]` 计算得到的值保存到 `samples`，后面的判断或输出会继续使用它。
    samples=[]
    # 开始循环 `f in sci_files`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for f in sci_files:
        # 把右侧 `Path(f).read_bytes(); low=b.lower()` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
        b=Path(f).read_bytes(); low=b.lower()
        # 把这一条新结果追加到列表 `samples`；不会覆盖前面已经收集的记录。
        samples.append({'文件':str(f),'大小':len(b),'SHA256':hashlib.sha256(b).hexdigest(),'含ITF000':b'itf000' in low,'含SAF':b'saf' in low})
    # 把右侧 `[` 计算得到的值保存到 `checks`，后面的判断或输出会继续使用它。
    checks=[
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'检查':'0x4070D0直接调用点恰为10个既知地址','status':'PASS' if loader==expected else 'FAIL','实际':loader},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'检查':'zlib包装0x44EAD0只有标准SF2链0x428D25一个直接调用','status':'PASS' if zcalls==['0x00428D25'] else 'FAIL','实际':zcalls},
      # 调用 `_map_check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
      _map_check_bytes(pe,0x00428C21,'6A 03 68 08 A6 46 00 56 E8 92 41 03 00','标准对象先比较3字节SF2魔数'),
      # 调用 `_map_check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
      _map_check_bytes(pe,0x00428C31,'85 C0 75 22','魔数不等则跳到拒绝分支'),
      # 调用 `_map_check_bytes` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
      _map_check_bytes(pe,0x00428CCB,'8A 45 0B 3C 6F','通过SF2魔数后才检查+0x0B压缩标志0x6F'),
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'检查':'EXE无SAF三字节字面量','status':'PASS' if b'SAF' not in pe.data else 'FAIL'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'检查':'Fight\\ITF静态路径不含裸名ITF000.SF2','status':'PASS' if not bare else 'FAIL','匹配':bare},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'检查':'所给SCI样本均不含ITF000/SAF字面量','status':'PASS' if all(not x['含ITF000'] and not x['含SAF'] for x in samples) else 'FAIL'}]
    # 把 `{'工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {'工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'Oracle哈希匹配':sha==MAP_TAIL_ORACLE_SHA256,
      # 给当前结果字典的“通用动画加载器调用点”字段填写 `calls,'Fight_ITF静态路径数量':len(itf),'Fight_ITF静态路径':itf,'SCI样本':samples,`，这样导出的 JSON/表格能保留这一项证据。
      '通用动画加载器调用点':calls,'Fight_ITF静态路径数量':len(itf),'Fight_ITF静态路径':itf,'SCI样本':samples,
      # 给当前结果字典的“关键验证”字段填写 `checks,'关键验证数量':len(checks),'关键验证全部通过':all(x['status']=='PASS' for x …`，这样导出的 JSON/表格能保留这一项证据。
      '关键验证':checks,'关键验证数量':len(checks),'关键验证全部通过':all(x['status']=='PASS' for x in checks),
      # 给当前结果字典的“冻结边界”字段填写 `['仅穷举当前RPG.exe .text中的E8直接调用；不能形式化排除函数指针/自修改代码。','当前EXE有编号ITF资源的明确静态请…`，这样导出的 JSON/表格能保留这一项证据。
      '冻结边界':['仅穷举当前RPG.exe .text中的E8直接调用；不能形式化排除函数指针/自修改代码。','当前EXE有编号ITF资源的明确静态请求，但没有Fight\\ITF\\ITF000.SF2裸名请求。','固化27已证明SAF不能通过标准SF2魔数门；本轮不扩大为所有发行版均未使用。']}

# 定义函数 `verify_itf_numbered_routes`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def verify_itf_numbered_routes(path:Path)->Dict[str,Any]:
    # 执行这一条实际代码步骤：`"""固化30：验证编号ITF对象包装链、两组稀疏路径表及下游严格SF2构造链。"""`；它与上下相邻语句共同完成当前函数的小任务。
    """固化30：验证编号ITF对象包装链、两组稀疏路径表及下游严格SF2构造链。"""
    # 把右侧 `_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]` 计算得到的值保存到 `pe`，后面的判断或输出会继续使用它。
    pe=_PEImage(path); sha=hashlib.sha256(pe.data).hexdigest(); checks=[]
    # 把右侧 `lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))` 计算得到的值保存到 `B`，后面的判断或输出会继续使用它。
    B=lambda va,h,m: checks.append(_map_check_bytes(pe,va,h,m))
    # 把右侧 `lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))` 计算得到的值保存到 `C`，后面的判断或输出会继续使用它。
    C=lambda va,t,m: checks.append(_map_check_call(pe,va,t,m))
    # 定义函数 `slot_table`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
    def slot_table(base:int)->List[Dict[str,Any]]:
        # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
        rows=[]
        # 开始循环 `i in range(11)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for i in range(11):
            # 把右侧 `pe.read_va(base+i*30,30); z=raw.find(b'\0'); z=30 if z<0 else z` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
            raw=pe.read_va(base+i*30,30); z=raw.find(b'\0'); z=30 if z<0 else z
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            rows.append({'索引':i,'VA':f'0x{base+i*30:08X}','路径':raw[:z].decode('ascii','replace'),'非空':z>0})
        # 把 `rows` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return rows
    # 把右侧 `slot_table(0x00469AB4); table_b=slot_table(0x00469C00)` 计算得到的值保存到 `table_a`，后面的判断或输出会继续使用它。
    table_a=slot_table(0x00469AB4); table_b=slot_table(0x00469C00)
    # 把右侧 `['Fight\\ITF\\ITF300.SF2','', 'Fight\\ITF\\ITF314.SF2','Fight\\ITF\\ITF306…` 计算得到的值保存到 `expect_a`，后面的判断或输出会继续使用它。
    expect_a=['Fight\\ITF\\ITF300.SF2','', 'Fight\\ITF\\ITF314.SF2','Fight\\ITF\\ITF306.SF2','Fight\\ITF\\ITF304.SF2','Fight\\ITF\\ITF312.SF2','', 'Fight\\ITF\\ITF308.SF2','Fight\\ITF\\ITF310.SF2','Fight\\ITF\\ITF302.SF2','']
    # 把右侧 `['','', 'Fight\\ITF\\ITF315.SF2','Fight\\ITF\\ITF307.SF2','','','','','Fig…` 计算得到的值保存到 `expect_b`，后面的判断或输出会继续使用它。
    expect_b=['','', 'Fight\\ITF\\ITF315.SF2','Fight\\ITF\\ITF307.SF2','','','','','Fight\\ITF\\ITF311.SF2','','']
    # 调用 `checks.extend` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    checks.extend([
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'检查':'第一组11×30字节ITF路径表内容','status':'PASS' if [x['路径'] for x in table_a]==expect_a else 'FAIL'},
      # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
      {'检查':'第二组11×30字节ITF路径表内容','status':'PASS' if [x['路径'] for x in table_b]==expect_b else 'FAIL'}])
    # 执行这一条实际代码步骤：`B(0x004207C6,'8D BD B4 9A 46 00','初始化循环以0x469AB4为第一路径表基址')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004207C6,'8D BD B4 9A 46 00','初始化循环以0x469AB4为第一路径表基址')
    # 执行这一条实际代码步骤：`B(0x004207D6,'8D BD 00 9C 46 00','初始化循环以0x469C00为第二路径表基址')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x004207D6,'8D BD 00 9C 46 00','初始化循环以0x469C00为第二路径表基址')
    # 执行一次赋值：把 `11×30字节')` 的结果放入左侧 `B(0x00420806,'83 C5 1E 42 81 FD 4A 01 00 00','每槽步长30字节…` 对应的变量/字段。
    B(0x00420806,'83 C5 1E 42 81 FD 4A 01 00 00','每槽步长30字节，共遍历0x14A=11×30字节')
    # 执行这一条实际代码步骤：`B(0x00420B95,'8D 04 7F 6A 01 8D 0C 80 8D 14 4D B4 9A 46 00','第一表用index×30计算路径地址')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00420B95,'8D 04 7F 6A 01 8D 0C 80 8D 14 4D B4 9A 46 00','第一表用index×30计算路径地址')
    # 执行这一条实际代码步骤：`C(0x00420BAD,0x00420CA0,'第一表路径送入正向战斗界面资源创建路由')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00420BAD,0x00420CA0,'第一表路径送入正向战斗界面资源创建路由')
    # 执行这一条实际代码步骤：`B(0x00420BBC,'8D 04 7F 6A 01 8D 04 80 8D 0C 45 00 9C 46 00','第二表用index×30计算路径地址')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00420BBC,'8D 04 7F 6A 01 8D 04 80 8D 0C 45 00 9C 46 00','第二表用index×30计算路径地址')
    # 执行这一条实际代码步骤：`C(0x00420BD4,0x00420D60,'第二表路径送入镜像/反向战斗界面资源创建路由')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00420BD4,0x00420D60,'第二表路径送入镜像/反向战斗界面资源创建路由')
    # 执行这一条实际代码步骤：`C(0x00420D30,0x0042D4D0,'第一表路由把动态路径送入Effect Manager资源创建器')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00420D30,0x0042D4D0,'第一表路由把动态路径送入Effect Manager资源创建器')
    # 执行这一条实际代码步骤：`C(0x00420DF0,0x0042D4D0,'第二表路由把动态路径送入同一Effect Manager资源创建器')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00420DF0,0x0042D4D0,'第二表路由把动态路径送入同一Effect Manager资源创建器')
    # 执行这一条实际代码步骤：`C(0x0042D4ED,0x0042D8B0,'Effect Manager先做SHO/BAK/MOV路径片段分类，不是资源魔数解析')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0042D4ED,0x0042D8B0,'Effect Manager先做SHO/BAK/MOV路径片段分类，不是资源魔数解析')
    # 执行这一条实际代码步骤：`C(0x0042D557,0x0043D1C0,'Effect Manager复用槽路径进入动画对象资源设置器')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0042D557,0x0043D1C0,'Effect Manager复用槽路径进入动画对象资源设置器')
    # 执行这一条实际代码步骤：`C(0x0042D588,0x0043D1C0,'Effect Manager新槽路径进入同一动画对象资源设置器')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0042D588,0x0043D1C0,'Effect Manager新槽路径进入同一动画对象资源设置器')
    # 执行这一条实际代码步骤：`C(0x0043D1CC,0x0043E460,'Effect包装对象把路径送入标准动画资源装载方法')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0043D1CC,0x0043E460,'Effect包装对象把路径送入标准动画资源装载方法')
    # 执行这一条实际代码步骤：`B(0x00431B7A,'68 64 03 00 00','轻量界面包装对象分配0x364字节的底层动画对象')`；它与上下相邻语句共同完成当前函数的小任务。
    B(0x00431B7A,'68 64 03 00 00','轻量界面包装对象分配0x364字节的底层动画对象')
    # 执行这一条实际代码步骤：`C(0x00431C37,0x0043E460,'轻量界面包装设置资源时进入标准动画资源装载方法')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00431C37,0x0043E460,'轻量界面包装设置资源时进入标准动画资源装载方法')
    # 执行这一条实际代码步骤：`C(0x00431635,0x0043E460,'扩展界面包装设置资源时进入同一标准动画资源装载方法')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x00431635,0x0043E460,'扩展界面包装设置资源时进入同一标准动画资源装载方法')
    # 执行这一条实际代码步骤：`C(0x0043E4AA,0x00428B60,'标准动画资源装载方法最终调用严格SF2构造器')`；它与上下相邻语句共同完成当前函数的小任务。
    C(0x0043E4AA,0x00428B60,'标准动画资源装载方法最终调用严格SF2构造器')
    # 把右侧 `[]; p=0` 计算得到的值保存到 `all_itf`，后面的判断或输出会继续使用它。
    all_itf=[]; p=0
    # 只要条件 `True` 仍然成立，就重复执行下面的循环体；循环体必须最终改变条件或主动退出。
    while True:
        # 把右侧 `pe.data.find(b'Fight\\ITF\\',p)` 计算得到的值保存到 `i`，后面的判断或输出会继续使用它。
        i=pe.data.find(b'Fight\\ITF\\',p)
        # 检查条件 `i<0`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if i<0: break
        # 把右侧 `pe.data.find(b'\0',i); all_itf.append({'VA':f'0x{pe.image_base+i:08X}','路径…` 计算得到的值保存到 `e`，后面的判断或输出会继续使用它。
        e=pe.data.find(b'\0',i); all_itf.append({'VA':f'0x{pe.image_base+i:08X}','路径':pe.data[i:e].decode('ascii')}); p=e+1
    # 把右侧 `[x for x in all_itf if 0x00469AB4<=int(x['VA'],16)<0x00469AB4+330 or 0x004…` 计算得到的值保存到 `in_tables`，后面的判断或输出会继续使用它。
    in_tables=[x for x in all_itf if 0x00469AB4<=int(x['VA'],16)<0x00469AB4+330 or 0x00469C00<=int(x['VA'],16)<0x00469C00+330]
    # 把这一条新结果追加到列表 `checks`；不会覆盖前面已经收集的记录。
    checks.append({'检查':'44条ITF路径严格分为33条普通静态路径与11条双表路径','status':'PASS' if len(all_itf)==44 and len(in_tables)==11 else 'FAIL','总数':len(all_itf),'双表数':len(in_tables)})
    # 把 `{'工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {'工具版本':TOOL_VERSION,'文件':str(path),'SHA256':sha,'Oracle_SHA256':MAP_TAIL_ORACLE_SHA256,'Oracle哈希匹配':sha==MAP_TAIL_ORACLE_SHA256,
      # 给当前结果字典的“第一路径表”字段填写 `table_a,'第二路径表':table_b,'全部ITF路径':all_itf,'关键验证':checks,'关键验证数量':len(…`，这样导出的 JSON/表格能保留这一项证据。
      '第一路径表':table_a,'第二路径表':table_b,'全部ITF路径':all_itf,'关键验证':checks,'关键验证数量':len(checks),'关键验证全部通过':all(x['status']=='PASS' for x in checks),
      # 给当前结果字典的“当前结论”字段填写 `'44条编号ITF路径由33条普通静态路径和两组11槽稀疏表中的11条非空路径组成；普通包装和双表Effect Manager下游均进入0…`，这样导出的 JSON/表格能保留这一项证据。
      '当前结论':'44条编号ITF路径由33条普通静态路径和两组11槽稀疏表中的11条非空路径组成；普通包装和双表Effect Manager下游均进入0x43E460，并最终调用严格SF2构造器0x428B60。',
      # 给当前结果字典的“证据边界”字段填写 `'0x42D8B0识别SHO/BAK/MOV目录片段只影响Effect Manager分类，不能误称为资源格式魔数；当前全部静态编号ITF…`，这样导出的 JSON/表格能保留这一项证据。
      '证据边界':'0x42D8B0识别SHO/BAK/MOV目录片段只影响Effect Manager分类，不能误称为资源格式魔数；当前全部静态编号ITF入口仍不包含裸名ITF000.SF2。'}

# 定义函数 `main`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def main(argv: Optional[List[str]]=None) -> int:
    # 把右侧 `argparse.ArgumentParser(description="《幽城幻剑录》SCI / SF2 调查器（只读）")` 计算得到的值保存到 `ap`，后面的判断或输出会继续使用它。
    ap=argparse.ArgumentParser(description="《幽城幻剑录》SCI / SF2 调查器（只读）")
    # 把右侧 `ap.add_subparsers(dest="cmd",required=True)` 计算得到的值保存到 `sp`，后面的判断或输出会继续使用它。
    sp=ap.add_subparsers(dest="cmd",required=True)
    # 把右侧 `sp.add_parser("sci",help="分析一个 SCI"); p.add_argument("file",type=Path); p.…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sci",help="分析一个 SCI"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("sci批量",help="递归分析目录内全部 SCI"); p.add_argument("root",type=Pa…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sci批量",help="递归分析目录内全部 SCI"); p.add_argument("root",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("跨格式",help="验证实体SCI到SF2动作范围"); p.add_argument("sci",nargs="+…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("跨格式",help="验证实体SCI到SF2动作范围"); p.add_argument("sci",nargs="+",type=Path); p.add_argument("--资源根",nargs="+",required=True,type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("sf2",help="分析一个 SF2"); p.add_argument("file",type=Path); p.…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sf2",help="分析一个 SF2"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("sf2批量",help="多个根按SHA去重后全量验证"); p.add_argument("roots",nargs…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sf2批量",help="多个根按SHA去重后全量验证"); p.add_argument("roots",nargs="+",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("sf2分派",help="输出0..30渲染模式原版分派"); p.add_argument("--json",typ…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sf2分派",help="输出0..30渲染模式原版分派"); p.add_argument("--json",type=Path); p.add_argument("--csv",type=Path)
    # 把右侧 `sp.add_parser("sf2渲染语义",help="固化34：输出RGB555/RGB565 LUT与0..30渲染模式闭式参考语义"); …` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sf2渲染语义",help="固化34：输出RGB555/RGB565 LUT与0..30渲染模式闭式参考语义"); p.add_argument("--json",type=Path); p.add_argument("--csv",type=Path)
    # 把右侧 `sp.add_parser("sf2渲染语料",help="固化34：按SHA去重统计全语料mode与p1/p2/p3"); p.add_argum…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sf2渲染语料",help="固化34：按SHA去重统计全语料mode与p1/p2/p3"); p.add_argument("roots",nargs="+",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG_SF2渲染查表",help="固化34：验证像素格式分流、LUT生成器与代表性Mode处理器"); p.a…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG_SF2渲染查表",help="固化34：验证像素格式分流、LUT生成器与代表性Mode处理器"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("sf2属性槽",help="输出Section0属性槽Getter调用点"); p.add_argument("--j…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sf2属性槽",help="输出Section0属性槽Getter调用点"); p.add_argument("--json",type=Path); p.add_argument("--csv",type=Path)
    # 把右侧 `sp.add_parser("sf2属性槽语料",help="固化31：统计属性槽磁盘取值域并核对效果消费链"); p.add_argument("…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("sf2属性槽语料",help="固化31：统计属性槽磁盘取值域并核对效果消费链"); p.add_argument("roots",nargs="+",type=Path); p.add_argument("--RPG",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG_SF2锚点路由",help="固化32：验证slot8/9主目标与自身锚点注册路由"); p.add_ar…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG_SF2锚点路由",help="固化32：验证slot8/9主目标与自身锚点注册路由"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG_SF2剩余属性路由",help="固化33：验证slot1/3及slot10/11直接调用边界"); p.…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG_SF2剩余属性路由",help="固化33：验证slot1/3及slot10/11直接调用边界"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("地图尾字段",help="汇总Map SCI +0x380/+0x384与剩余尾段样本值"); p.add_argum…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("地图尾字段",help="汇总Map SCI +0x380/+0x384与剩余尾段样本值"); p.add_argument("files",nargs="+",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证Interface天書资源",help="比较MEN0003正常导航资源与MEN0009天書禁用变体"); p.a…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证Interface天書资源",help="比较MEN0003正常导航资源与MEN0009天書禁用变体"); p.add_argument("men0003",type=Path); p.add_argument("men0009",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG地图尾字段",help="对同版RPG.exe重放Map SCI尾字段/code12静态证据"); p.ad…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG地图尾字段",help="对同版RPG.exe重放Map SCI尾字段/code12静态证据"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG地图后半未知块",help="固化25：审计Map三段未知块样本与直接字面访问"); p.add_argum…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG地图后半未知块",help="固化25：审计Map三段未知块样本与直接字面访问"); p.add_argument("file",type=Path); p.add_argument("地图SCI",nargs="+",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG_SAF边界",help="固化27：验证SAF压缩包装与标准SF2拒绝边界"); p.add_argume…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG_SAF边界",help="固化27：验证SAF压缩包装与标准SF2拒绝边界"); p.add_argument("file",type=Path); p.add_argument("SAF",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG_ITF调用图",help="固化28：穷举ITF静态请求、动画加载器与zlib直接调用"); p.add_…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG_ITF调用图",help="固化28：穷举ITF静态请求、动画加载器与zlib直接调用"); p.add_argument("file",type=Path); p.add_argument("SCI",nargs="*",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG_ITF编号路由",help="固化30：验证编号ITF双路径表及下游严格SF2构造链"); p.add_a…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG_ITF编号路由",help="固化30：验证编号ITF双路径表及下游严格SF2构造链"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG实体行为",help="固化23修正版2：重放Entity behavior type与移动档静态证据");…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG实体行为",help="固化23修正版2：重放Entity behavior type与移动档静态证据"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG背景模式",help="固化23修正版2：重放Map+0x65/+0x6E背景控制器静态证据"); p.ad…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG背景模式",help="固化23修正版2：重放Map+0x65/+0x6E背景控制器静态证据"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG显示定时配置",help="固化23修正版2：重放DDDES显示/定时配置静态证据"); p.add_arg…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG显示定时配置",help="固化23修正版2：重放DDDES显示/定时配置静态证据"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `sp.add_parser("验证RPG实体事件绑定",help="固化23修正版2：重放Entity +EA/+EB双事件绑定静态证据"); p.…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser("验证RPG实体事件绑定",help="固化23修正版2：重放Entity +EA/+EB双事件绑定静态证据"); p.add_argument("file",type=Path); p.add_argument("--json",type=Path)
    # 把右侧 `ap.parse_args(argv)` 计算得到的值保存到 `ns`，后面的判断或输出会继续使用它。
    ns=ap.parse_args(argv)
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 检查条件 `ns.cmd=="sci"`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ns.cmd=="sci": r=parse_sci(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="sci批量"`；成立才执行这一分支。
        elif ns.cmd=="sci批量":
            # 把右侧 `[parse_sci(p) for p in sorted(ns.root.rglob("*")) if p.is_file() and p.suf…` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
            rows=[parse_sci(p) for p in sorted(ns.root.rglob("*")) if p.is_file() and p.suffix.lower()==".sci"]
            # 把右侧 `{"工具版本":TOOL_VERSION,"根目录":str(ns.root),"文件数":len(rows),"分类统计":dict(Counte…` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r={"工具版本":TOOL_VERSION,"根目录":str(ns.root),"文件数":len(rows),"分类统计":dict(Counter(x["分类"]["类型"] for x in rows)),"文件":rows}
        # 前一个条件没有成立时，再检查 `ns.cmd=="跨格式"`；成立才执行这一分支。
        elif ns.cmd=="跨格式": r=cross_validate_entity_sf2(ns.sci,ns.资源根)
        # 前一个条件没有成立时，再检查 `ns.cmd=="sf2"`；成立才执行这一分支。
        elif ns.cmd=="sf2": r=parse_sf2(ns.file,True)
        # 前一个条件没有成立时，再检查 `ns.cmd=="sf2批量"`；成立才执行这一分支。
        elif ns.cmd=="sf2批量": r=sf2_batch_dedup(ns.roots)
        # 前一个条件没有成立时，再检查 `ns.cmd=="sf2分派"`；成立才执行这一分支。
        elif ns.cmd=="sf2分派":
            # 把右侧 `render_dispatch_dictionary()` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=render_dispatch_dictionary()
            # 检查条件 `ns.csv`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ns.csv: write_csv(ns.csv,r["模式"],["原始模式","处理器","Canonical名称","原版行为"])
        # 前一个条件没有成立时，再检查 `ns.cmd=="sf2渲染语义"`；成立才执行这一分支。
        elif ns.cmd=="sf2渲染语义":
            # 把右侧 `render_semantics_dictionary()` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=render_semantics_dictionary()
            # 检查条件 `ns.csv`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ns.csv: write_csv(ns.csv,r["模式"],["模式","名称","机器算法","闭式公式","磁盘正样本"])
        # 前一个条件没有成立时，再检查 `ns.cmd=="sf2渲染语料"`；成立才执行这一分支。
        elif ns.cmd=="sf2渲染语料": r=sf2_render_mode_corpus(ns.roots)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG_SF2渲染查表"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG_SF2渲染查表": r=verify_sf2_render_lut_rpg(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="sf2属性槽"`；成立才执行这一分支。
        elif ns.cmd=="sf2属性槽":
            # 把右侧 `property_callsite_dictionary()` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=property_callsite_dictionary()
            # 检查条件 `ns.csv`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ns.csv: write_csv(ns.csv,r["调用点"],["调用点","槽","类型","证据"])
        # 前一个条件没有成立时，再检查 `ns.cmd=="sf2属性槽语料"`；成立才执行这一分支。
        elif ns.cmd=="sf2属性槽语料": r=sf2_property_domains(ns.roots,ns.RPG)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG_SF2锚点路由"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG_SF2锚点路由": r=verify_sf2_anchor_routes(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG_SF2剩余属性路由"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG_SF2剩余属性路由": r=verify_sf2_remaining_property_routes(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="地图尾字段"`；成立才执行这一分支。
        elif ns.cmd=="地图尾字段": r=map_tail_samples(ns.files)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证Interface天書资源"`；成立才执行这一分支。
        elif ns.cmd=="验证Interface天書资源": r=compare_interface_tianshu_resources(ns.men0003,ns.men0009)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG地图尾字段"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG地图尾字段": r=verify_map_tail_rpg(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG地图后半未知块"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG地图后半未知块": r=verify_map_unknown_blocks(ns.file,ns.地图SCI)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG_SAF边界"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG_SAF边界": r=verify_saf_boundary(ns.file,ns.SAF)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG_ITF调用图"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG_ITF调用图": r=verify_itf_callgraph(ns.file,ns.SCI)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG_ITF编号路由"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG_ITF编号路由": r=verify_itf_numbered_routes(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG实体行为"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG实体行为": r=verify_entity_behavior_rpg(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG背景模式"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG背景模式": r=verify_map_background_mode_rpg(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG显示定时配置"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG显示定时配置": r=verify_dddes_display_timer_rpg(ns.file)
        # 前一个条件没有成立时，再检查 `ns.cmd=="验证RPG实体事件绑定"`；成立才执行这一分支。
        elif ns.cmd=="验证RPG实体事件绑定": r=verify_entity_event_bindings_rpg(ns.file)
        # 前面的条件分支都没有命中时，执行这个兜底分支。
        else: raise RuntimeError("未知命令")
        # 检查条件 `getattr(ns,"json",None)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if getattr(ns,"json",None): write_json(ns.json,r)
        # 执行一次赋值：把 `False,indent=2))` 的结果放入左侧 `print(json.dumps(r,ensure_ascii` 对应的变量/字段。
        print(json.dumps(r,ensure_ascii=False,indent=2))
        # 批量存在 FAIL 才返回2；“当前资源根缺样本”不是格式失败。
        # 检查条件 `ns.cmd=="sf2批量" and r["汇总"].get("FAIL",0)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ns.cmd=="sf2批量" and r["汇总"].get("FAIL",0): return 2
        # 检查条件 `ns.cmd=="跨格式" and r["汇总"].get("FAIL",0)`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ns.cmd=="跨格式" and r["汇总"].get("FAIL",0): return 2
        # 把 `0` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return 0
    # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception as e:
        # 执行一次赋值：把 `__import__('sys').stderr); return 2` 的结果放入左侧 `print(f"错误：{e}",file` 对应的变量/字段。
        print(f"错误：{e}",file=__import__('sys').stderr); return 2

# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__=="__main__":
    # 检测到不能继续的情况，主动抛出异常 `SystemExit(main())`，让上层明确知道数据或参数不符合要求。
    raise SystemExit(main())
