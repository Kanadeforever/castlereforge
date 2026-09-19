#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 九项持续场域“完整静态直接引用面”调查器。

本工具对应 Godot 兼容引擎逆向研究“固化63”。

前情很重要：

- 固化58已经证明 Battle 里存在两个九槽持续场域 manager；
- 固化59已经穷举 `0x43DA50 / 0x43DC10 / 0x43DC40 / 0x43DC60` 这组
  查询接口的 direct CALL，并证明 slot2/3/4/6/7/8 有高层数值 consumer；
- 当时 slot0「非天死潭」、slot1「封炎滅陣」、slot5「幽魄厲界」只能写成
  “在已穷举的有效性查询家族中没有 consumer”，不能扩大成“整个 EXE 都没有别的
  数值 consumer”。

固化63专门补这个边界。这里不再只看“谁 call 了查询函数”，而是同时做四层扫描：

1. 穷举两个九槽 manager **基址**在 `.text` 里的全部 32-bit literal；
2. 穷举两个 manager **完整对象地址范围**里出现的全部 32-bit raw candidate，确认
   是否有代码绕过 manager 方法，直接读取某个 slot 数组字段；
3. 穷举固化59确认的两组 manager **别名全局指针**，确认别名只有已知绑定和查询用途；
4. 穷举 manager 的 reset/query/tick/create/owner 方法 direct CALL，并额外扫描整个 EXE
   是否把这些函数地址作为绝对函数指针常量保存到表或全局变量。

给只上过一天编程课的读者一个直观比喻：

- 假设 manager 是两个“九格储物柜”；
- 固化59只检查了“有没有人通过前台服务员询问第几格”；
- 固化63还要检查“有没有人拿备用钥匙直接打开柜门”“有没有绕到后门读取柜子内部”
  “有没有把服务员电话号码抄到别的表里再间接打过去”。

如果这些直接静态入口都穷举完，而 slot0/1/5 仍然没有任何数值读取，那么我们才可以
把当前同版 EXE 的边界收紧成：

    “在当前已证的 canonical 静态直接引用面里，slot0/1/5 没有额外数值 consumer。”

但这仍然**不是**“slot0/1/5 没有效果”。它们明确有：

- `0x43DB20` 建立；
- `0x43DAB0` 生命周期更新；
- Effect Manager 表现句柄；
- owner 查询/清理；
- owner 持有任意九槽时影响一条 Battle 状态转换。

所以兼容实现必须保留三个 slot 和它们的表现/生命周期；只是当前无需凭空发明新的数值
公式。若未来出现间接调用、函数指针、其他版本或动态证据，本节点结论可以继续扩展。

本工具只读输入文件，不修改 EXE，不启动游戏，不做 Hook，也不产生运行时探针。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
import sys
from collections import Counter
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple


# ---------------------------------------------------------------------------
# 一、复用已经冻结过的 PE / FIRTTECH 解析器
# ---------------------------------------------------------------------------

# 固化58的工具已经稳定实现了：
# - PE section 解析；
# - VA -> 文件偏移换算；
# - FIRTTECH ENC 解密；
# - x86 `E8 rel32` direct CALL 目标计算。
#
# 新工具直接复用这些基础能力，避免为了固化63又复制一套底层解析逻辑。
try:
    from youcheng_battle_field_effect_slot_inspector import (
        EXPECTED_FIRTTECH_DECODED_SHA256,
        EXPECTED_FIRTTECH_ENCODED_SHA256,
        EXPECTED_ORIGINAL_RPG_SHA256,
        EXPECTED_RESEARCH_RPG_SHA256,
        PeImage,
        parse_firttech,
        parse_pe,
        scan_direct_calls,
        sha256_bytes,
        sha256_file,
    )
except ImportError as exc:
    # 如果完整固化包里的稳定依赖缺失，必须立即失败。
    # 绝不能悄悄少做一部分检查后仍然输出“PASS”。
    raise SystemExit(
        "缺少同目录依赖 youcheng_battle_field_effect_slot_inspector.py；"
        "请使用完整固化包。"
    ) from exc


# ---------------------------------------------------------------------------
# 二、固化63冻结常量
# ---------------------------------------------------------------------------

# 工具版本只是报告标签，方便以后知道某个 JSON 是哪一版工具生成的。
TOOL_VERSION = "0.7D-solid63.0"

# 两个已经由固化58证明的九槽 manager 对象。
# 这里继续用地址中性命名，不把低地址/高地址直接写成“敌方/己方”。
MANAGER_A = 0x0089FE08
MANAGER_B = 0x0089FFD0

# 固化58证明 manager 最后一个已知字段位于 +0x1C0。
# 为了扫描“有没有直接读取对象内部字段”的字面地址，我们把对象范围定义为
# [base, base + 0x1C4)，也就是把 +0x1C0 那个 DWORD 完整包含进去。
MANAGER_OBJECT_SIZE = 0x1C4

# +0x1C0 是固化58已经证明的实例选择值：
# - MANAGER_A 初始化写 1；
# - MANAGER_B 初始化写 0；
# - `0x43DB20` 据此选择位置索引 27 / 7。
MANAGER_MODE_OFFSET = 0x1C0

# 固化59证明这两组全局变量是 manager 的别名指针。
# 0x89FD54/58 供菜单侧查询使用；0x8B01CC/D0 供公式层 wrapper 使用。
ALIASES: Dict[int, str] = {
    0x0089FD54: "菜单侧manager别名A",
    0x0089FD58: "菜单侧manager别名B",
    0x008B01CC: "公式层manager别名A",
    0x008B01D0: "公式层manager别名B",
}

# 九槽方法地址。每一个都会重新扫描 direct E8 caller。
METHODS: Dict[int, str] = {
    0x0043DA30: "九槽reset",
    0x0043DA50: "按slot查询有效性",
    0x0043DAB0: "九槽生命周期tick",
    0x0043DB20: "按FIRTTECH白名单建立场域slot",
    0x0043DC10: "按owner查询任一active slot",
    0x0043DC40: "按slot查询owner",
    0x0043DC60: "按owner清理全部slot",
}

# 这些 direct caller 集合来自固化58/59，但固化63会重新从原版 EXE 计算。
# 用完整集合断言可以防止以后只看到“数量相同”却漏掉某个地址被替换。
EXPECTED_METHOD_CALLERS: Dict[int, Tuple[int, ...]] = {
    0x0043DA30: (0x0043DA13, 0x00442849, 0x0044285D),
    0x0043DA50: (
        0x0041A761, 0x0041A772,
        0x004204DD, 0x004204F7, 0x00420515, 0x0042052F,
        0x00423087, 0x004230A3,
        0x0042BF56, 0x0042BF75,
        0x004442C9, 0x004442D9,
    ),
    0x0043DAB0: (0x00442CB4, 0x00442CBE),
    0x0043DB20: (0x004200A7, 0x004200FA),
    0x0043DC10: (0x00442D2C, 0x00442D3C),
    0x0043DC40: (0x004230DB, 0x004230ED),
    0x0043DC60: (0x00421226, 0x00421231, 0x004232A8, 0x004232B5),
}

# 两个 manager **基址**本身在 `.text` 里的 32-bit literal 起始地址。
# 注意：这里记录的是“立即数4字节从哪里开始”，不一定是整条x86指令的第一个字节。
EXPECTED_BASE_LITERAL_POSITIONS: Dict[int, Tuple[int, ...]] = {
    MANAGER_A: (
        0x0041F2C0,
        0x004200A3,
        0x004204D9,
        0x00420511,
        0x0042121C,
        0x0042309F,
        0x004230D6,
        0x004232A4,
        0x004235B1,
        0x004235E5,
        0x00442793,
        0x0044283F,
        0x00442CB0,
        0x00442D28,
        0x004442D5,
    ),
    MANAGER_B: (
        0x0041F2F7,
        0x004200F6,
        0x004204F3,
        0x0042052B,
        0x0042122D,
        0x00423082,
        0x004230E9,
        0x004232B0,
        0x00423601,
        0x00423635,
        0x004427A3,
        0x0044284F,
        0x00442CBA,
        0x00442D38,
        0x004442C5,
    ),
}

# 别名全局变量在 `.text` 中的所有 32-bit literal 位置。
# 每个都严格只有两处：一次绑定/写入，一次读取/消费。
EXPECTED_ALIAS_LITERAL_POSITIONS: Dict[int, Tuple[int, ...]] = {
    0x0089FD54: (0x0041A76C, 0x0044278F),
    0x0089FD58: (0x0041A75B, 0x0044279F),
    0x008B01CC: (0x0042A785, 0x0042BF51),
    0x008B01D0: (0x0042A779, 0x0042BF71),
}

# “扫描对象完整地址范围”会把每个连续4字节窗口都当成一个小端DWORD候选。
# 这种扫描故意比“只找基址”更宽，因为我们想捕获 `base+offset` 的直接字段引用。
#
# 但 x86 是变长指令，所以一个4字节窗口也可能从某条 CALL 的 opcode 中间起步，
# 碰巧拼出一个落在 manager 地址范围内的整数。下面冻结了当前原版全部候选：
# - 真正的基址 literal；
# - 真正的 `+0x1C0` 字段 literal；
# - 极少数跨指令字节形成的假候选。
#
# 这比简单说“没找到”更强：以后工具会要求**候选全集完全相同**。
EXPECTED_OBJECT_RANGE_CANDIDATES_A: Tuple[Tuple[int, int, str], ...] = (
    (0x0041F2C0, 0x0089FE08, "base"),
    (0x004200A3, 0x0089FE08, "base"),
    (0x004204D9, 0x0089FE08, "base"),
    (0x00420511, 0x0089FE08, "base"),
    (0x0042121C, 0x0089FE08, "base"),
    (0x0042309F, 0x0089FE08, "base"),
    (0x004230D6, 0x0089FE08, "base"),
    (0x004232A4, 0x0089FE08, "base"),
    (0x004235B1, 0x0089FE08, "base"),
    (0x004235E5, 0x0089FE08, "base"),
    (0x00442793, 0x0089FE08, "base"),
    (0x0044283F, 0x0089FE08, "base"),
    (0x00442855, 0x0089FFC8, "direct_field_+0x1C0"),
    (0x00442CB0, 0x0089FE08, "base"),
    (0x00442D28, 0x0089FE08, "base"),
    (0x004442D5, 0x0089FE08, "base"),
    (0x00448B4D, 0x0089FEE8, "false_positive_cross_instruction_bytes"),
)

EXPECTED_OBJECT_RANGE_CANDIDATES_B: Tuple[Tuple[int, int, str], ...] = (
    (0x0041F2F7, 0x0089FFD0, "base"),
    (0x004200F6, 0x0089FFD0, "base"),
    (0x004204F3, 0x0089FFD0, "base"),
    (0x0042052B, 0x0089FFD0, "base"),
    (0x0042122D, 0x0089FFD0, "base"),
    (0x00423082, 0x0089FFD0, "base"),
    (0x004230E9, 0x0089FFD0, "base"),
    (0x004232B0, 0x0089FFD0, "base"),
    (0x00423601, 0x0089FFD0, "base"),
    (0x00423635, 0x0089FFD0, "base"),
    (0x004427A3, 0x0089FFD0, "base"),
    (0x0044284F, 0x0089FFD0, "base"),
    (0x00442868, 0x008A0190, "direct_field_+0x1C0"),
    (0x00442CBA, 0x0089FFD0, "base"),
    (0x00442D38, 0x0089FFD0, "base"),
    (0x004442C5, 0x0089FFD0, "base"),
    (0x0044EC1E, 0x008A0000, "false_positive_cross_instruction_bytes"),
    (0x00457729, 0x008A0046, "false_positive_cross_instruction_bytes"),
)

# 固化59已经证明通过有效性查询家族实际消费的slot。
QUERY_CONSUMED_SLOTS = (2, 3, 4, 6, 7, 8)

# 本节点专门收口的三个slot。
SLOTS_WITHOUT_NUMERIC_DIRECT_CONSUMER = (0, 1, 5)

# 九项 FIRTTECH ID -> slot 映射仍用固化58机器表。
SLOT_TO_FIRTTECH_ID: Tuple[int, ...] = (
    0x15A,  # slot0 非天死潭
    0x17C,  # slot1 封炎滅陣
    0x1C2,  # slot2 反咒禁制
    0x1C9,  # slot3 閾迦封界
    0x1CA,  # slot4 鬼唳天幕
    0x1CD,  # slot5 幽魄厲界
    0x1CE,  # slot6 六甲神儀
    0x1CF,  # slot7 時輪返陣
    0x1D1,  # slot8 鬼縛之陣
)

# 关键区间做原版冻结SHA和原版↔研究版逐字节等价检查。
# 这些区间覆盖：
# - 两个manager绑定进公式层；
# - 菜单别名查询；
# - 九槽完整方法主体；
# - Battle初始化/别名绑定/reset；
# - Battle tick与owner状态门；
# - Enemy AI slot2查询。
CRITICAL_RANGES: Tuple[Tuple[int, int, str], ...] = (
    (0x0041A730, 0x0041A78F, "菜单侧manager别名slot2查询"),
    (0x0041F2B0, 0x0041F30B, "Battle构造把双manager绑定到公式层"),
    (0x00420048, 0x00420113, "九项场域建立双manager提交"),
    (0x004204C6, 0x00420553, "slot7和slot8行动计时消费"),
    (0x004211FE, 0x00421238, "主资源归零owner清理"),
    (0x00423070, 0x004230F3, "manager查询包装器"),
    (0x00423280, 0x004232C1, "角色失效owner清理"),
    (0x004235A0, 0x00423640, "双manager CRT构造析构包装"),
    (0x0042A760, 0x0042A793, "公式层manager别名绑定"),
    (0x0042BF40, 0x0042BF84, "公式层slot有效性wrapper"),
    (0x0043DA10, 0x0043DCAB, "九槽reset查询tick建立owner方法主体"),
    (0x00442783, 0x00442863, "Battle别名绑定与双manager reset"),
    (0x00442CA2, 0x00442D5D, "双manager tick与owner转换门"),
    (0x004442A9, 0x004442E7, "Enemy AI slot2查询"),
)


# ---------------------------------------------------------------------------
# 三、小型通用辅助函数
# ---------------------------------------------------------------------------

def read_u32_le(data: bytes, offset: int) -> int:
    """从 byte 串中按小端读取一个无符号32位整数。"""

    # `struct.unpack_from` 会直接检查越界，比手工拼4个字节更安全。
    return struct.unpack_from("<I", data, offset)[0]


def format_va(value: int) -> str:
    """把地址统一格式化成8位十六进制，便于JSON人工比对。"""

    return f"0x{value:08X}"


def text_bytes_and_va(image: PeImage) -> Tuple[bytes, int]:
    """取出 `.text` 原始字节以及该section的首VA。"""

    # 先找到 `.text` section 描述。
    section = image.text_section()
    # 文件里的代码字节从 raw_offset 开始，长度取 raw_size。
    data = image.data[section.raw_offset:section.raw_offset + section.raw_size]
    # section 在内存中的地址 = ImageBase + section RVA。
    base_va = image.image_base + section.virtual_address
    return data, base_va


def scan_exact_u32_literal_in_text(image: PeImage, value: int) -> List[int]:
    """扫描 `.text` 中所有完全等于指定DWORD的4字节窗口。"""

    # 把目标整数编码成x86/PE使用的小端4字节形式。
    needle = struct.pack("<I", value)
    text, text_va = text_bytes_and_va(image)
    positions: List[int] = []

    # `bytes.find` 每次找到一个匹配位置。
    # 下一轮从当前命中+1开始，这样连重叠候选也不会漏掉。
    cursor = 0
    while True:
        found = text.find(needle, cursor)
        if found < 0:
            break
        positions.append(text_va + found)
        cursor = found + 1
    return positions


def scan_u32_windows_in_range(image: PeImage, low: int, high: int) -> List[Tuple[int, int]]:
    """枚举 `.text` 所有落在 [low, high) 的4字节小端整数候选。"""

    # 这个函数故意不尝试“猜x86指令边界”。
    # 它的职责是先形成一个**不会漏掉任何连续4字节窗口**的超集。
    text, text_va = text_bytes_and_va(image)
    hits: List[Tuple[int, int]] = []

    # 最后3个字节不足以组成DWORD，所以循环到 len(text)-4 为止。
    for offset in range(0, max(0, len(text) - 3)):
        value = read_u32_le(text, offset)
        if low <= value < high:
            hits.append((text_va + offset, value))
    return hits


def scan_absolute_function_pointer_literals(image: PeImage, target_va: int) -> List[Tuple[int, str]]:
    """扫描整个PE文件，查找把函数VA作为绝对DWORD常量保存的位置。"""

    # direct E8 CALL 使用的是 rel32，不会直接出现函数VA四字节。
    # 如果整个EXE里真的出现目标VA本身，往往意味着：
    # - 函数指针表；
    # - 全局函数指针变量；
    # - 某段代码显式 `push function_address` / `mov reg, function_address`。
    needle = struct.pack("<I", target_va)
    hits: List[Tuple[int, str]] = []
    cursor = 0

    while True:
        found = image.data.find(needle, cursor)
        if found < 0:
            break

        # 默认先把位置标成PE headers；如果落进某个section，再换成section名和VA。
        label = f"file+0x{found:X}"
        for section in image.sections:
            if section.raw_offset <= found < section.raw_offset + section.raw_size:
                inside = found - section.raw_offset
                va = image.image_base + section.virtual_address + inside
                label = f"{section.name}:{format_va(va)}"
                break

        hits.append((found, label))
        cursor = found + 1
    return hits


def range_sha(image: PeImage, start_va: int, end_va: int) -> str:
    """计算一个VA半开区间的SHA-256。"""

    # end-start 就是要读的字节数。
    data = image.read_va(start_va, end_va - start_va)
    return hashlib.sha256(data).hexdigest()


def compare_range(original: PeImage, research: PeImage, start_va: int, end_va: int) -> Dict[str, object]:
    """比较原版与研究版某个关键区间是否逐字节一致。"""

    original_bytes = original.read_va(start_va, end_va - start_va)
    research_bytes = research.read_va(start_va, end_va - start_va)
    same = original_bytes == research_bytes
    return {
        "起始": format_va(start_va),
        "结束_不含": format_va(end_va),
        "长度": end_va - start_va,
        "原版SHA256": sha256_bytes(original_bytes),
        "研究版SHA256": sha256_bytes(research_bytes),
        "逐字节一致": same,
    }


def normalize_candidate_list(items: Iterable[Tuple[int, int]]) -> List[Tuple[int, int]]:
    """把候选按地址排序，避免集合顺序影响机器报告。"""

    return sorted(items, key=lambda pair: (pair[0], pair[1]))


# ---------------------------------------------------------------------------
# 四、把 raw object-range 候选与冻结分类表逐项对齐
# ---------------------------------------------------------------------------

def inspect_object_range(
    image: PeImage,
    base: int,
    expected: Sequence[Tuple[int, int, str]],
    manager_name: str,
) -> Dict[str, object]:
    """检查一个manager完整对象地址范围里的所有32位候选。"""

    # 先从 `.text` 每个连续4字节窗口里找出落在对象范围的数值。
    actual_pairs = scan_u32_windows_in_range(image, base, base + MANAGER_OBJECT_SIZE)
    actual_pairs = normalize_candidate_list(actual_pairs)

    # 冻结表除了地址和值，还包含我们已经人工验证过的分类。
    expected_pairs = sorted((va, value) for va, value, _kind in expected)

    # 地址和值必须一项不多、一项不少地完全相等。
    complete_match = actual_pairs == expected_pairs

    # 为了让报告更容易读，把每个候选加上 `value-base` 偏移。
    rows: List[Dict[str, object]] = []
    kind_by_pair = {(va, value): kind for va, value, kind in expected}
    for va, value in actual_pairs:
        rows.append(
            {
                "literal起始VA": format_va(va),
                "候选值": format_va(value),
                "相对manager偏移": f"+0x{value - base:X}",
                "冻结分类": kind_by_pair.get((va, value), "UNEXPECTED_NEW_CANDIDATE"),
            }
        )

    # 真正直接指向对象内部字段的候选，只应该是 +0x1C0 两处初始化写入。
    direct_fields = [
        row for row in rows
        if str(row["冻结分类"]).startswith("direct_field_")
    ]

    # 跨指令假阳性必须明确保留在报告里。
    # 这样以后维护者不会重新看到这些DWORD后误以为发现了新consumer。
    false_candidates = [
        row for row in rows
        if row["冻结分类"] == "false_positive_cross_instruction_bytes"
    ]

    return {
        "manager": manager_name,
        "base": format_va(base),
        "扫描范围": f"[{format_va(base)}, {format_va(base + MANAGER_OBJECT_SIZE)})",
        "候选数量": len(rows),
        "候选全集": rows,
        "候选全集严格匹配冻结表": complete_match,
        "真正直接字段literal": direct_fields,
        "跨指令假阳性": false_candidates,
        "除base与+0x1C0外出现其它直接字段literal": any(
            row["冻结分类"] == "UNEXPECTED_NEW_CANDIDATE" for row in rows
        ),
    }


# ---------------------------------------------------------------------------
# 五、检查九槽方法的调用面与函数指针面
# ---------------------------------------------------------------------------

def inspect_methods(image: PeImage) -> List[Dict[str, object]]:
    """对所有九槽方法做 direct CALL + 绝对函数指针双重检查。"""

    reports: List[Dict[str, object]] = []

    for target_va, meaning in METHODS.items():
        # 第一层：严格解析 `E8 rel32`，得到direct caller。
        callers = tuple(scan_direct_calls(image, target_va))
        expected = EXPECTED_METHOD_CALLERS[target_va]

        # 第二层：整个PE搜索函数VA四字节常量。
        # 当前所有目标都应该是0命中。
        pointer_literals = scan_absolute_function_pointer_literals(image, target_va)

        reports.append(
            {
                "目标": format_va(target_va),
                "意义": meaning,
                "direct_CALL数量": len(callers),
                "direct_CALL实际": [format_va(x) for x in callers],
                "direct_CALL预期": [format_va(x) for x in expected],
                "direct_CALL严格匹配": callers == expected,
                "整个PE绝对函数VA常量数量": len(pointer_literals),
                "整个PE绝对函数VA常量": [label for _off, label in pointer_literals],
                "无绝对函数指针常量": len(pointer_literals) == 0,
            }
        )

    return reports


# ---------------------------------------------------------------------------
# 六、检查manager基址和别名全局
# ---------------------------------------------------------------------------

def inspect_exact_literals(image: PeImage) -> Dict[str, object]:
    """检查两个manager基址以及四个别名变量的精确literal集合。"""

    managers: List[Dict[str, object]] = []
    for base in (MANAGER_A, MANAGER_B):
        actual = tuple(scan_exact_u32_literal_in_text(image, base))
        expected = EXPECTED_BASE_LITERAL_POSITIONS[base]
        managers.append(
            {
                "manager_base": format_va(base),
                "实际数量": len(actual),
                "实际literal起始": [format_va(x) for x in actual],
                "预期literal起始": [format_va(x) for x in expected],
                "严格匹配": actual == expected,
            }
        )

    aliases: List[Dict[str, object]] = []
    for address, meaning in ALIASES.items():
        actual = tuple(scan_exact_u32_literal_in_text(image, address))
        expected = EXPECTED_ALIAS_LITERAL_POSITIONS[address]
        aliases.append(
            {
                "alias全局": format_va(address),
                "意义": meaning,
                "实际数量": len(actual),
                "实际literal起始": [format_va(x) for x in actual],
                "预期literal起始": [format_va(x) for x in expected],
                "严格匹配": actual == expected,
            }
        )

    return {
        "manager基址literal": managers,
        "manager别名literal": aliases,
    }


# ---------------------------------------------------------------------------
# 七、把九个slot重新对齐canonical FIRTTECH资源
# ---------------------------------------------------------------------------

def inspect_firttech(firttech_path: Path) -> Tuple[Dict[str, object], List[Dict[str, object]]]:
    """重新解密FIRTTECH并输出九项slot对应的作者名称/说明。"""

    decoded, records = parse_firttech(firttech_path)
    encoded_sha = sha256_file(firttech_path)
    decoded_sha = sha256_bytes(decoded)

    # 用 raw record ID 建索引。
    # 固化61已经提醒过：物理index与raw ID不能全局假定完全相等。
    by_id = {record.record_id: record for record in records}

    rows: List[Dict[str, object]] = []
    for slot, record_id in enumerate(SLOT_TO_FIRTTECH_ID):
        record = by_id[record_id]
        rows.append(
            {
                "slot": slot,
                "FIRTTECH_ID": f"0x{record_id:03X}",
                "物理record_index": record.index,
                "作者名称": record.name,
                "machine_class": record.action_class,
                "+0x20_raw": record.route_20,
                "+0x90_raw": record.counter_90,
                "作者说明": record.description,
                "当前高层数值查询状态": (
                    "已有固化59数值consumer"
                    if slot in QUERY_CONSUMED_SLOTS
                    else "固化63关闭当前canonical静态直接额外数值consumer边界"
                ),
            }
        )

    summary = {
        "加密SHA256": encoded_sha,
        "加密SHA严格匹配": encoded_sha == EXPECTED_FIRTTECH_ENCODED_SHA256,
        "解密SHA256": decoded_sha,
        "解密SHA严格匹配": decoded_sha == EXPECTED_FIRTTECH_DECODED_SHA256,
        "记录数": len(records),
        "九项slot数": len(rows),
        "无额外数值direct_consumer的slot": list(SLOTS_WITHOUT_NUMERIC_DIRECT_CONSUMER),
    }
    return summary, rows


# ---------------------------------------------------------------------------
# 八、关键区间双版本等价检查
# ---------------------------------------------------------------------------

def inspect_equivalence(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    """确认固化63依赖区间没有落进历史研究版10字节补丁簇。"""

    reports: List[Dict[str, object]] = []
    for start_va, end_va, meaning in CRITICAL_RANGES:
        row = compare_range(original, research, start_va, end_va)
        row["意义"] = meaning
        reports.append(row)
    return reports


# ---------------------------------------------------------------------------
# 九、汇总“slot0/1/5没有额外静态直接数值consumer”的证据链
# ---------------------------------------------------------------------------

def build_boundary_conclusion(
    method_reports: Sequence[Dict[str, object]],
    exact_literals: Dict[str, object],
    object_a: Dict[str, object],
    object_b: Dict[str, object],
) -> Dict[str, object]:
    """把多个独立扫描结果组合成一个不越界的实现级结论。"""

    # 所有method direct caller集合必须严格匹配固化58/59冻结集合。
    methods_ok = all(
        bool(row["direct_CALL严格匹配"]) and bool(row["无绝对函数指针常量"])
        for row in method_reports
    )

    # manager基址与四个alias的literal集合也必须完全匹配。
    manager_literals_ok = all(
        bool(row["严格匹配"])
        for row in exact_literals["manager基址literal"]
    )
    alias_literals_ok = all(
        bool(row["严格匹配"])
        for row in exact_literals["manager别名literal"]
    )

    # 完整对象范围候选必须和冻结表一项不差。
    object_ranges_ok = (
        bool(object_a["候选全集严格匹配冻结表"])
        and bool(object_b["候选全集严格匹配冻结表"])
        and not bool(object_a["除base与+0x1C0外出现其它直接字段literal"])
        and not bool(object_b["除base与+0x1C0外出现其它直接字段literal"])
    )

    closed = methods_ok and manager_literals_ok and alias_literals_ok and object_ranges_ok

    return {
        "direct方法调用面严格闭合": methods_ok,
        "manager基址literal面严格闭合": manager_literals_ok,
        "manager别名literal面严格闭合": alias_literals_ok,
        "manager对象内部直接字段literal面严格闭合": object_ranges_ok,
        "slot0_1_5当前canonical静态直接额外数值consumer边界关闭": closed,
        "关闭范围": (
            "当前同版RPG.exe.org的静态direct E8调用、绝对函数VA常量、manager基址literal、"
            "完整manager对象地址范围DWORD literal、固化59四组manager别名literal"
        ),
        "不能扩大成": [
            "slot0/1/5没有任何效果",
            "slot0/1/5在其他版本没有额外consumer",
            "不存在计算型指针、未识别间接调用、JIT/自修改或运行时动态访问",
            "可以删除slot0/1/5或不实现它们的表现/生命周期/owner逻辑",
        ],
        "兼容实现规则": [
            "九槽结构仍完整保留0..8全部slot。",
            "slot0/1/5仍按固化58规则建立、计时、创建EFF句柄、到界释放并支持owner清理。",
            "owner拥有任意九槽时，仍参与固化59已证的Battle状态转换阻挡门。",
            "当前canonical实现不要为slot0/1/5凭空新增额外伤害、恢复、AI或计时数值公式。",
            "未来若新证据证明间接/异版本consumer，只新增consumer，不推翻九槽结构。",
        ],
    }


# ---------------------------------------------------------------------------
# 十、写CSV：给人工浏览一个更紧凑的九项表
# ---------------------------------------------------------------------------

def write_csv(path: Path, rows: Sequence[Dict[str, object]]) -> None:
    """输出九项场域slot的资源与consumer状态表。"""

    # 输出目录如果不存在就创建。
    path.parent.mkdir(parents=True, exist_ok=True)

    # CSV使用UTF-8 BOM，Windows Excel直接打开中文更稳妥。
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        fieldnames = [
            "slot",
            "FIRTTECH_ID",
            "物理record_index",
            "作者名称",
            "machine_class",
            "+0x20_raw",
            "+0x90_raw",
            "当前高层数值查询状态",
            "作者说明",
        ]
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            writer.writerow({key: row.get(key, "") for key in fieldnames})


# ---------------------------------------------------------------------------
# 十一、命令行主流程
# ---------------------------------------------------------------------------

def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    """定义命令行参数。"""

    parser = argparse.ArgumentParser(
        description=(
            "静态核对《幽城幻剑录》Battle九项持续场域manager的完整direct引用面，"
            "关闭slot0/1/5当前canonical额外数值consumer边界。"
        )
    )
    parser.add_argument("--original-rpg", required=True, type=Path, help="完全未修改原版 RPG.exe.org 解压后的RPG.exe")
    parser.add_argument("--research-rpg", required=True, type=Path, help="历史研究版 RPG.exe")
    parser.add_argument("--firttech", required=True, type=Path, help="canonical Public/Firttech.enc")
    parser.add_argument("--json", required=True, type=Path, help="输出机器审计JSON")
    parser.add_argument("--csv", required=True, type=Path, help="输出九项场域CSV")
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    """执行所有检查并在任一硬断言失败时返回非0。"""

    args = parse_args(sys.argv[1:] if argv is None else argv)

    # 第一步：检查输入文件哈希。
    original_sha = sha256_file(args.original_rpg)
    research_sha = sha256_file(args.research_rpg)
    input_hash_ok = (
        original_sha == EXPECTED_ORIGINAL_RPG_SHA256
        and research_sha == EXPECTED_RESEARCH_RPG_SHA256
    )

    # 第二步：解析两份PE。
    # 后续所有VA读取都依赖 section 表，不能拿VA直接当文件偏移。
    original = parse_pe(args.original_rpg)
    research = parse_pe(args.research_rpg)

    # 第三步：重新扫描所有九槽方法的direct caller和函数指针常量。
    method_reports = inspect_methods(original)

    # 第四步：检查两个manager基址与四个alias的所有literal位置。
    exact_literals = inspect_exact_literals(original)

    # 第五步：扫描两个manager完整地址范围。
    # 这是固化63比固化59多出来的关键检查。
    object_a = inspect_object_range(
        original,
        MANAGER_A,
        EXPECTED_OBJECT_RANGE_CANDIDATES_A,
        "manager_A_0x89FE08",
    )
    object_b = inspect_object_range(
        original,
        MANAGER_B,
        EXPECTED_OBJECT_RANGE_CANDIDATES_B,
        "manager_B_0x89FFD0",
    )

    # 第六步：重新解密FIRTTECH，把九个slot对齐作者资源文字。
    firt_summary, firt_rows = inspect_firttech(args.firttech)

    # 第七步：原版与研究版关键区间逐字节比较。
    equivalence = inspect_equivalence(original, research)
    equivalence_ok = all(bool(row["逐字节一致"]) for row in equivalence)

    # 第八步：组合多个独立扫描，形成不越界的边界结论。
    boundary = build_boundary_conclusion(
        method_reports,
        exact_literals,
        object_a,
        object_b,
    )

    # 所有硬检查都必须通过，才允许最终“全部通过=true”。
    all_pass = all(
        [
            input_hash_ok,
            bool(firt_summary["加密SHA严格匹配"]),
            bool(firt_summary["解密SHA严格匹配"]),
            equivalence_ok,
            bool(boundary["slot0_1_5当前canonical静态直接额外数值consumer边界关闭"]),
        ]
    )

    # 构造最终机器报告。
    report: Dict[str, object] = {
        "工具": Path(__file__).name,
        "工具版本": TOOL_VERSION,
        "节点": "固化63",
        "研究主题": "Battle九槽manager完整静态直接引用面与slot0/1/5额外数值consumer边界",
        "输入": {
            "原版RPG": str(args.original_rpg.resolve()),
            "研究版RPG": str(args.research_rpg.resolve()),
            "Firttech": str(args.firttech.resolve()),
        },
        "输入哈希": {
            "原版RPG_SHA256": original_sha,
            "原版严格匹配": original_sha == EXPECTED_ORIGINAL_RPG_SHA256,
            "研究版RPG_SHA256": research_sha,
            "研究版严格匹配": research_sha == EXPECTED_RESEARCH_RPG_SHA256,
        },
        "Firttech": firt_summary,
        "九项canonical记录": firt_rows,
        "九槽方法direct调用与函数指针面": method_reports,
        "manager基址与别名literal面": exact_literals,
        "manager_A完整对象地址范围扫描": object_a,
        "manager_B完整对象地址范围扫描": object_b,
        "关键区间原版与研究版等价": equivalence,
        "结论边界": boundary,
        "全部通过": all_pass,
    }

    # 写JSON前创建父目录。
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(
        json.dumps(report, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )

    # CSV单独输出，方便人工快速检查九个slot。
    write_csv(args.csv, firt_rows)

    # 控制台输出只保留关键摘要。
    print(f"[固化63] 原版RPG SHA: {original_sha}")
    print(f"[固化63] 研究版RPG SHA: {research_sha}")
    print(f"[固化63] manager A base literal: {len(EXPECTED_BASE_LITERAL_POSITIONS[MANAGER_A])}处")
    print(f"[固化63] manager B base literal: {len(EXPECTED_BASE_LITERAL_POSITIONS[MANAGER_B])}处")
    print(f"[固化63] slot0/1/5 direct numeric consumer boundary closed: {boundary['slot0_1_5当前canonical静态直接额外数值consumer边界关闭']}")
    print(f"[固化63] 全部通过: {all_pass}")

    return 0 if all_pass else 1


if __name__ == "__main__":
    raise SystemExit(main())
