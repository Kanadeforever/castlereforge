#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》Battle 九项场域“有效性查询 + 高层效果消费”静态调查器。

本工具对应 Godot 兼容引擎逆向研究“固化59”。固化58已经把九项持续场域的
创建、双实例、九槽布局、计数更新、Effect Manager 句柄以及 owner 清理闭合。
固化59不再重复证明那些内容，而是专门回答一个更高层的问题：

    “九槽被建立以后，原版到底在什么地方查询它仍然有效，并据此改变什么游戏行为？”

为了避免只看一两个调用点就下结论，本工具采用“穷举 direct CALL + 上层 wrapper”方式：

1. 重新扫描原版 `.text`，准确列出 `0x43DA50` 的全部直接调用；
2. 重新扫描 `0x43DC10 / 0x43DC40 / 0x43DC60` 的全部直接调用；
3. 验证 `0x42BF40 / 0x423070 / 0x4230B0` 三个高层 wrapper 的调用面；
4. 验证 `0x8B01CC / 0x8B01D0 / 0x89FD54 / 0x89FD58` 最终都只别名到
   固化58已经确认的 `0x89FFD0 / 0x89FE08` 两个九槽管理器；
5. 对 slot2/3/4/6/7/8 的每条已证高层消费链做短机器码、整段 SHA-256 和
   caller 集合断言；
6. 解密 canonical `Firttech.enc`，重新把 9 个 slot 对齐作者名称、class 与说明；
7. 输出机器可重放 JSON/CSV，明确哪些是“机器事实”、哪些只是“实现级研究标签”；
8. 对完全原版 `RPG.exe.org` 与历史研究版 `RPG.exe` 的所有固化59关键范围做
   逐字节比较，防止把历史补丁行为错误写成原版行为。

给第一次学习编程的读者一个最直观的比喻：

- 固化58像是确认“战场上有两块九格插线板，每格能插一个持续效果”；
- 固化59则是在找“游戏哪些代码会去看某一格是否还亮着，然后决定要不要改伤害、
  改行动速度、过滤 AI 招式，或者延迟某个角色状态转换”。

本工具不会修改任何输入文件，也不会启动游戏。它只读取二进制和资源，然后把
已经人工逆向确认的地址关系重新计算成可重复验证的机器报告。

固化63同步说明：本工具仍然只负责“固化59的有效性查询家族”本身，不伪装成
全PE直接引用面调查器。固化63新增同目录
`youcheng_battle_field_effect_direct_surface_inspector.py`，进一步穷举manager基址、
整个manager对象地址范围、四个已知alias、七个manager方法direct CALL与绝对函数VA
常量。两者合并后，slot0/1/5可以收紧为“当前同版canonical静态direct面未发现额外
数值consumer”；但建立、EFF表现、生命周期、owner清理与任意active slot的状态转换门
仍然必须保留，未知计算指针/动态间接访问/其它版本也仍不在静态direct证明范围内。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple


# ---------------------------------------------------------------------------
# 一、复用固化58的稳定基础解析器
# ---------------------------------------------------------------------------

# 这个工具和固化58工具放在同一个“工具”目录里。
# Python 启动脚本时，会自动把脚本所在目录放进 sys.path，
# 因此可以安全复用已经冻结过的 PE / FIRTTECH 解析代码。
# 这样做的好处是：同一种 PE 地址换算、同一种 ENC 解密只保留一个经过回归的实现，
# 不会因为固化59重新抄一份而制造第二套算法。
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
    # 如果有人只拿走本文件，没有带上固化58稳定工具，就直接给出清楚错误。
    # 不能悄悄退化成“少做一些检查”，因为那会破坏固化包的可重放性。
    raise SystemExit(
        "缺少同目录依赖 youcheng_battle_field_effect_slot_inspector.py；"
        "请使用完整固化包，不要单独抽走本文件。"
    ) from exc


# ---------------------------------------------------------------------------
# 二、固化59冻结常量
# ---------------------------------------------------------------------------

# 工具版本只负责报告追踪，不参与任何业务判断。
TOOL_VERSION = "0.7D-solid59.1-solid63-sync"

# 固化58已经从原版 EXE 静态表读出的九个 slot 对应 ID。
# 固化59再次核对资源记录，是为了把高层 consumer 的“slot编号”转换回作者名称，
# 让兼容引擎开发者不必每次手工查表。
SLOT_TO_ID: Tuple[int, ...] = (
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

# 这是本节点最先纠正的一项事实：0x43DA50 direct CALL 是 12 处，不是 14 处。
# 这里把全部地址冻结成机器断言，今后若地址数变化，工具必须直接 FAIL。
EXPECTED_43DA50_CALLS: Tuple[int, ...] = (
    0x0041A761,
    0x0041A772,
    0x004204DD,
    0x004204F7,
    0x00420515,
    0x0042052F,
    0x00423087,
    0x004230A3,
    0x0042BF56,
    0x0042BF75,
    0x004442C9,
    0x004442D9,
)

# owner -> active slot 查询只有 Battle 主循环里的两次调用。
EXPECTED_43DC10_CALLS: Tuple[int, ...] = (
    0x00442D2C,
    0x00442D3C,
)

# slot -> owner 查询只被 0x4230B0 两个分支调用。
EXPECTED_43DC40_CALLS: Tuple[int, ...] = (
    0x004230DB,
    0x004230ED,
)

# owner 清理已经在固化58闭合；固化59仍重扫一次，确保调用面没有被误改。
EXPECTED_43DC60_CALLS: Tuple[int, ...] = (
    0x00421226,
    0x00421231,
    0x004232A8,
    0x004232B5,
)

# 0x42BF40 是公式侧最重要的“manager selector + slot有效性”包装器。
# 原版只有 9 个 direct caller；这些 caller 恰好覆盖 slot 2/3/4/6。
EXPECTED_42BF40_CALLS: Tuple[int, ...] = (
    0x0042A919,
    0x0042A9FB,
    0x0042AE50,
    0x0042AF31,
    0x0042C0D0,
    0x0042C4A9,
    0x0042C88A,
    0x0042CA13,
    0x0042CA3E,
)

# 0x423070 只在 Battle result apply 的 slot3 拦截链里被调用一次。
EXPECTED_423070_CALLS: Tuple[int, ...] = (0x0042133D,)

# 0x4230B0 只在 slot3 owner 次资源扣除链里被调用一次。
EXPECTED_4230B0_CALLS: Tuple[int, ...] = (0x004211D2,)

# 0x42A760 是把三条 Battle 全局指针绑定进公式层的唯一 direct caller。
EXPECTED_42A760_CALLS: Tuple[int, ...] = (0x0041F306,)

# 原版九槽管理器两个实际对象。
MANAGER_LOW_GROUP = 0x0089FFD0
MANAGER_HIGH_GROUP = 0x0089FE08

# 固化59明确使用的 slot 集合。
# 这里的“被查询”只表示通过本节点穷举到的 0x43DA50 / wrapper 查询家族消费，
# 固化63补充后，这句话可以进一步收紧为：在当前同版canonical的静态direct引用面中，
# slot0/1/5没有发现固化59家族之外的额外数值consumer。
# 但这仍然不能扩大成“其它 slot 没有任何游戏效果”，因为建立、EFF表现、生命周期、
# owner清理以及任意active slot阻挡特定Battle状态转换都是真实存在的共享行为。
QUERY_CONSUMED_SLOTS = (2, 3, 4, 6, 7, 8)
NO_VALIDITY_QUERY_SLOTS = (0, 1, 5)


# ---------------------------------------------------------------------------
# 三、关键机器码切片
# ---------------------------------------------------------------------------

# 每个元组包含：起始 VA、结束 VA（不含）、中文意义、原版冻结 SHA-256。
# 使用整段哈希的原因是：只断言一条 CALL 很容易漏掉 CALL 后面的条件分支；
# 整段哈希可以保证本节点解释所依赖的控制流没有悄悄变化。
CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (
        0x0043DA50,
        0x0043DAA9,
        "0x43DA50九槽有效性谓词完整主体",
        "4cfa9bad3273b1c5238b317f9b683b1ba7526076b4158321e507926d0b1a041e",
    ),
    (
        0x0041A6D0,
        0x0041A78F,
        "slot2对Battle菜单class2状态表现flag的消费",
        "a8bf5bfc111d04b6022e18ac78512b01662b25cdf16b33aa5203083ef4b90bff",
    ),
    (
        0x0041A3A8,
        0x0041A3CC,
        "class2条目读取0x78C flag并改变条目属性",
        "7fa31d2bb6148777daac4c489c29ae6ff8d50f3380fc9d5676138ef8faa862e5",
    ),
    (
        0x004204C6,
        0x00420619,
        "slot7/8对角色计时结构两个百分比步进参数的消费",
        "e5452bd151b362a6c276304577793a02f599f78a950292800485850b4732491f",
    ),
    (
        0x00423070,
        0x004230F3,
        "按角色槽组选manager的slot有效性与slot-owner包装器",
        "df4c16b6208503b85ef09fbb0102883024a8c505b58f777593271baa9caf11cc",
    ),
    (
        0x0042BF40,
        0x0042BF84,
        "公式层manager-selector到0x43DA50的包装器",
        "dd4884cecb81cc8401b3f94fb32a4a9b3d27687de788f76ca0931ae8effe82b5",
    ),
    (
        0x004442A9,
        0x004443D7,
        "slot2影响Enemy AI class2候选过滤的控制流",
        "148f206f2d4fd7ec010fd68318c2eb59239e19b29afbd661ae710bd1bab5fd3e",
    ),
    (
        0x0042A760,
        0x0042A791,
        "公式层三全局指针绑定函数",
        "8c98b8aa9f1cc69ea7802f689c2f4a8f35c8666fcc3723fad6631b57251b3957",
    ),
    (
        0x0041F2B0,
        0x0041F30B,
        "Battle构造调用0x42A760并传入两个既有九槽实例",
        "d122f23598b4478ef4ed557f5f950a58be4237b26f6d0f4f2bbe544c3a2a69d2",
    ),
    (
        0x00442783,
        0x004427A7,
        "Battle初始化把0x89FD54/58别名到两个九槽实例",
        "e386fc893e502f585c693815e24c0e800b2327f74697cbfb334f1b7cd03528f6",
    ),
    (
        0x004211C5,
        0x00421236,
        "slot3 owner次资源扣除与归零后双实例owner清理",
        "0993cbb03de8c308de530f7cb462800f1486dc0ccb3d035b2bd77a084fe7fd3b",
    ),
    (
        0x004212CE,
        0x00421374,
        "slot3在result阶段累计两个结果通道并清零",
        "421e3308f470629167c8ce8472dc0614d652593206784111a2106f58f2cdf585",
    ),
    (
        0x0042CA07,
        0x0042CA65,
        "slot2对class2、slot6对class0/1的结果清零门",
        "1769b325762d4348bc8f1ae67906c84e833480cbdffa008b5b369a809205cd8c",
    ),
    (
        0x00442D01,
        0x00442D5D,
        "owner仍持有任一九槽效果时阻止0x7D6到0x7D1状态转换",
        "a201d733555ee3f5a787948f622deec29b7e1c8b018f10d14082b9398e62a107",
    ),
    (
        0x00423DA0,
        0x00423F3E,
        "角色三通道计时结构初始化与按百分比推进",
        "cdc90a205189ba64d9a562df3fb04562635372753e9497b2e8fab3dbc5d3ba50",
    ),
    (
        0x00424090,
        0x004240B9,
        "0x424090把mode1/2/3百分比写入+0x2C/+0x30/+0x34",
        "b21f950089378094cf47c663f1f636cd7dd1ed9bd60591dad05edb5c9b1a2502",
    ),
    (
        0x0042A8FF,
        0x0042AA11,
        "slot3/4在第一组公式路径中的布尔/零值消费",
        "132c68fa5fe2d9bf436b32c0e3100fc3aa31ab7041bd2c95cd11607f5e54bdd7",
    ),
    (
        0x0042AE43,
        0x0042AF56,
        "slot4在两条公式路径把两个结果DWORD清零",
        "44d592d8a3322f9587bc916c214ab534e9122ac26c25c5067496c7b52230c90e",
    ),
    (
        0x0042C0C4,
        0x0042C0E6,
        "slot4在另一公式分支的条件清零",
        "2ec6b7e53506a3fa3e6a8dcdc877dc8c929a1a4b0aecdc1b697df4936f6693f2",
    ),
    (
        0x0042C48B,
        0x0042C4BC,
        "slot4把该helper计算结果直接压成0",
        "43eccca3b648a93f1ffd70d6751721d5a069ebcaee9143c08162965b63a29d6f",
    ),
    (
        0x0042C884,
        0x0042C8CB,
        "slot3在主DamageInfo计算路径强制局部布尔门为1",
        "7bb52d310b82bd78bdfb7f73b40ff9ceb75e39b4cfc2d269a66875461d7740ad",
    ),
)


# ---------------------------------------------------------------------------
# 四、短机器码断言
# ---------------------------------------------------------------------------

@dataclass(frozen=True)
class ByteAssertion:
    """描述一条必须逐字节相等的短机器码证据。"""

    va: int
    expected_hex: str
    meaning: str


# 短断言的作用是帮助读报告的人快速看懂“整段哈希为什么有意义”。
# 例如，看到 `push 7 -> manager -> call 43DA50` 就能直接理解 slot7 查询。
BYTE_ASSERTIONS: Tuple[ByteAssertion, ...] = (
    ByteAssertion(
        0x004204D6,
        "6a 07 b9 08 fe 89 00 e8 6e d5 01 00",
        "角色group=1时查询0x89FE08的slot7",
    ),
    ByteAssertion(
        0x004204F0,
        "6a 07 b9 d0 ff 89 00 e8 54 d5 01 00",
        "角色group=0时查询0x89FFD0的slot7",
    ),
    ByteAssertion(
        0x0042050E,
        "6a 08 b9 08 fe 89 00 e8 36 d5 01 00",
        "角色group=1时查询0x89FE08的slot8",
    ),
    ByteAssertion(
        0x00420528,
        "6a 08 b9 d0 ff 89 00 e8 1c d5 01 00",
        "角色group=0时查询0x89FFD0的slot8",
    ),
    ByteAssertion(
        0x00420500,
        "bf c8 00 00 00",
        "slot7有效时百分比参数从100改成200",
    ),
    ByteAssertion(
        0x00420538,
        "8d 04 bf 8d 0c 80 b8 1f 85 eb 51 d1 e1 f7 e9 c1 fa 05 8b ca c1 e9 1f 03 d1 8b fa",
        "slot8有效时执行当前百分比参数/2的整数公式",
    ),
    ByteAssertion(
        0x00424090,
        "8b 44 24 08 48 74 1a 48 74 0d 48 75 1b",
        "0x424090按mode 1/2/3分派百分比字段",
    ),
    ByteAssertion(
        0x00423F1A,
        "8b 51 2c b8 1f 85 eb 51 0f af 51 08 f7 ea c1 fa 05",
        "计时mode1使用+0x2C百分比乘+0x08后/100",
    ),
    ByteAssertion(
        0x00423EEC,
        "8b 51 30 b8 1f 85 eb 51 0f af 51 14 f7 ea c1 fa 05",
        "计时mode2使用+0x30百分比乘+0x14后/100",
    ),
    ByteAssertion(
        0x0042CA0B,
        "6a 02 8b cd 8b 42 04 50 e8 28 f5 ff ff",
        "主DamageInfo路径查询slot2",
    ),
    ByteAssertion(
        0x0042CA1C,
        "83 7f 18 02 75 10 33 c0 89 44 24 20 89 44 24 14 89 46 04 89 46 08",
        "slot2有效且machine class2时清局部值和DamageInfo两个DWORD",
    ),
    ByteAssertion(
        0x0042CA36,
        "6a 06 8b 51 04 8b cd 52 e8 fd f4 ff ff",
        "主DamageInfo路径查询slot6",
    ),
    ByteAssertion(
        0x0042CA47,
        "8b 4f 18 33 c0 3b c8 74 05 83 f9 01 75 0c 89 44 24 14 89 46 04 89 46 08",
        "slot6有效且machine class0/1时清主要结果和DamageInfo两个DWORD",
    ),
    ByteAssertion(
        0x004442C2,
        "6a 02 b9 d0 ff 89 00 e8 82 97 ff ff",
        "Enemy AI先查询0x89FFD0 slot2",
    ),
    ByteAssertion(
        0x004442D2,
        "6a 02 b9 08 fe 89 00 e8 72 97 ff ff",
        "Enemy AI再查询0x89FE08 slot2",
    ),
    ByteAssertion(
        0x004443AD,
        "84 c0 75 26",
        "AI传入布尔为0时进入class过滤分支",
    ),
    ByteAssertion(
        0x004443C6,
        "83 7c 38 18 02 75 0a c6 44 24 13 00",
        "AI布尔为0且候选FIRTTECH class2时把候选可用flag清0",
    ),
    ByteAssertion(
        0x00421338,
        "56 6a 03 8b cf e8 2e 1d 00 00",
        "result阶段按目标role slot查询slot3有效性",
    ),
    ByteAssertion(
        0x00421346,
        "a1 84 fd 89 00 8b 13 03 c2 a3 84 fd 89 00 8b 4b 04 03 c1 a3 84 fd 89 00",
        "slot3有效时把两个结果DWORD累加进0x89FD84",
    ),
    ByteAssertion(
        0x0042136F,
        "89 2b 89 6b 04",
        "slot3有效后把刚累计的两个结果DWORD清零",
    ),
    ByteAssertion(
        0x004211CD,
        "55 6a 03 8b cf e8 d9 1e 00 00",
        "消费阶段按当前角色反向选择manager并查询slot3 owner",
    ),
    ByteAssertion(
        0x004211E3,
        "b8 77 77 77 77 f7 eb 8b 0c b5 94 fd 89 00 2b d3 c1 fa 03",
        "slot3 owner代价使用0x77777777 magic除法序列",
    ),
    ByteAssertion(
        0x00442D26,
        "57 b9 08 fe 89 00 e8 df ae ff ff 83 f8 ff 75 74",
        "角色状态转换前先查0x89FE08是否仍有该owner的active slot",
    ),
    ByteAssertion(
        0x00442D36,
        "57 b9 d0 ff 89 00 e8 cf ae ff ff 83 f8 ff 75 64",
        "再查0x89FFD0；任一找到owner都跳过转换",
    ),
)


# ---------------------------------------------------------------------------
# 五、报告数据结构
# ---------------------------------------------------------------------------

@dataclass(frozen=True)
class SlotRecordView:
    """把 canonical FIRTTECH 中九条白名单压成固化59需要的字段。"""

    slot: int
    record_id: int
    name: str
    action_class: int
    route_20: int
    counter_90: int
    description: str
    queried_by_validity_family: bool


@dataclass(frozen=True)
class ConsumerRow:
    """CSV 中每一条高层消费规则。"""

    slot: int
    record_id_hex: str
    name: str
    consumer: str
    machine_effect: str
    evidence_addresses: str
    boundary: str


# ---------------------------------------------------------------------------
# 六、通用小工具
# ---------------------------------------------------------------------------

def format_va_list(values: Sequence[int]) -> List[str]:
    """把整数地址统一转成 0xXXXXXXXX 字符串，便于 JSON 阅读。"""

    # 地址统一八位十六进制，避免 0x41 和 0x0041 混在一份报告里。
    return [f"0x{value:08X}" for value in values]


def verify_calls(image: PeImage, target: int, expected: Sequence[int], meaning: str) -> Dict[str, object]:
    """扫描一个目标函数的全部 direct E8 caller，并与冻结集合严格比较。"""

    # scan_direct_calls 会真正解 rel32，不是搜索字节 0xE8。
    actual = tuple(scan_direct_calls(image, target))
    # caller 集合必须数量、顺序、地址全部一致。
    passed = actual == tuple(expected)
    return {
        "目标": f"0x{target:08X}",
        "意义": meaning,
        "实际数量": len(actual),
        "预期数量": len(expected),
        "实际caller": format_va_list(actual),
        "预期caller": format_va_list(expected),
        "PASS": passed,
    }


def verify_assertion(image: PeImage, item: ByteAssertion) -> Dict[str, object]:
    """读取一段短机器码并逐字节比较。"""

    # bytes.fromhex 把人能读的十六进制文字变成真实字节。
    expected = bytes.fromhex(item.expected_hex)
    # read_va 会通过 PE section 表把 VA 正确换算成文件偏移。
    actual = image.read_va(item.va, len(expected))
    return {
        "地址": f"0x{item.va:08X}",
        "意义": item.meaning,
        "长度": len(expected),
        "实际": actual.hex(" "),
        "预期": expected.hex(" "),
        "PASS": actual == expected,
    }


def verify_slice_hashes(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    """核对原版冻结哈希，并比较研究版是否在同一区间逐字节一致。"""

    result: List[Dict[str, object]] = []
    # 每一个关键区间都同时做两类检查：
    # A. 原版是否仍等于本节点冻结哈希；
    # B. 研究版是否与原版逐字节相同。
    for start, end, meaning, expected_hash in CRITICAL_SLICES:
        original_bytes = original.read_va(start, end - start)
        research_bytes = research.read_va(start, end - start)
        actual_hash = sha256_bytes(original_bytes)
        result.append(
            {
                "起始": f"0x{start:08X}",
                "结束_不含": f"0x{end:08X}",
                "意义": meaning,
                "长度": end - start,
                "原版SHA256": actual_hash,
                "冻结SHA256": expected_hash,
                "原版哈希PASS": actual_hash == expected_hash,
                "研究版逐字节等同原版": research_bytes == original_bytes,
                "PASS": actual_hash == expected_hash and research_bytes == original_bytes,
            }
        )
    return result


def signed_divide_by_15_cost(accumulated: int) -> int:
    """复刻 slot3 owner 链在 accumulated>0 时的最终次资源扣除量。

    原版机器码不是直接写 ``idiv 15``，而是用常数 0x77777777 做有符号乘法高位
    近似，再经过 ``sub / sar / sign correction`` 得到负商。对当前调用路径已经先证明
    ``0x89FD84 > 0``，因此最终效果就是：

        secondary_current -= floor(accumulated / 15)

    这个 Python 函数只用于把机器数学写得清楚；真正可信证据仍是 EXE 字节断言。
    """

    # 本函数只接受当前真实 consumer 会进入的正数域。
    if accumulated <= 0:
        raise ValueError("slot3 owner cost 只在 accumulated > 0 时执行")
    # 对正整数，C/C++ trunc 与数学 floor 相同。
    return accumulated // 15


def slot7_slot8_percent(slot7_active: bool, slot8_active: bool) -> int:
    """按 0x4204C6..0x420553 复刻两个场域对计时百分比参数的组合。

    原版先从100开始；slot7有效则改为200；之后slot8有效就把“当前值”除2。
    所以四种组合严格是 100 / 200 / 50 / 100。
    """

    # 默认百分比是100，等价于不缩放。
    value = 100
    # slot7 的机器分支不是“+100”，而是直接写200。
    if slot7_active:
        value = 200
    # slot8 对当前值做有符号整数 /2；当前域都是正数，所以等价于 //2。
    if slot8_active:
        value = int(value / 2)
    return value


def build_slot_views(firttech_path: Path) -> Tuple[List[SlotRecordView], Dict[str, object]]:
    """解密 canonical FIRTTECH 并提取九条 slot 记录。"""

    # parse_firttech 返回解密后的原始 bytes 和 500 条结构化记录。
    decoded, records = parse_firttech(firttech_path)
    # 先做两层哈希核对，防止用户误给了另一个版本的 Firttech.enc。
    encoded_hash = sha256_file(firttech_path)
    decoded_hash = sha256_bytes(decoded)
    # 用 ID 建字典，后面每个 slot 能 O(1) 找到记录。
    by_id = {record.record_id: record for record in records}

    views: List[SlotRecordView] = []
    for slot, record_id in enumerate(SLOT_TO_ID):
        # 如果九项 ID 任意一个不存在，说明资源基线不再是 canonical。
        if record_id not in by_id:
            raise ValueError(f"canonical FIRTTECH 缺少 slot{slot} 对应 ID 0x{record_id:X}")
        record = by_id[record_id]
        views.append(
            SlotRecordView(
                slot=slot,
                record_id=record.record_id,
                name=record.name,
                action_class=record.action_class,
                route_20=record.route_20,
                counter_90=record.counter_90,
                description=record.description,
                queried_by_validity_family=slot in QUERY_CONSUMED_SLOTS,
            )
        )

    hashes = {
        "encoded_sha256": encoded_hash,
        "encoded_expected": EXPECTED_FIRTTECH_ENCODED_SHA256,
        "encoded_PASS": encoded_hash == EXPECTED_FIRTTECH_ENCODED_SHA256,
        "decoded_sha256": decoded_hash,
        "decoded_expected": EXPECTED_FIRTTECH_DECODED_SHA256,
        "decoded_PASS": decoded_hash == EXPECTED_FIRTTECH_DECODED_SHA256,
    }
    hashes["PASS"] = bool(hashes["encoded_PASS"] and hashes["decoded_PASS"])
    return views, hashes


# ---------------------------------------------------------------------------
# 七、把机器 consumer 整理成兼容实现可读表
# ---------------------------------------------------------------------------

def build_consumer_rows(views: Sequence[SlotRecordView]) -> List[ConsumerRow]:
    """生成“slot -> 高层消费行为”CSV。

    注意：这里写的是已经由机器分支证明的效果，不给未知字段强行起作者正式名。
    同一个 slot 可以有多行，因为一个场域会同时被 UI、AI、公式或结果阶段消费。
    """

    # 先把 slot 记录放进字典，后面写行时能直接带出作者名称和 ID。
    by_slot = {view.slot: view for view in views}
    rows: List[ConsumerRow] = []

    def add(slot: int, consumer: str, effect: str, addresses: str, boundary: str) -> None:
        """内部小函数：减少重复填写 record ID/name 的机械代码。"""

        view = by_slot[slot]
        rows.append(
            ConsumerRow(
                slot=slot,
                record_id_hex=f"0x{view.record_id:X}",
                name=view.name,
                consumer=consumer,
                machine_effect=effect,
                evidence_addresses=addresses,
                boundary=boundary,
            )
        )

    # slot2：反咒禁制。Battle 菜单建立时只要任一管理器 slot2 有效，就把 +0x78C 清0；
    # 之后遍历 class2 条目时，+0x78C==0 会改写对应条目的 +0x38 属性。
    add(
        2,
        "Battle菜单class2条目状态表现",
        "任一九槽实例slot2有效 -> 对象+0x78C=0；遍历FIRTTECH class2条目时据此改写条目+0x38属性。",
        "0x41A6D0..0x41A781; 0x41A3A8..0x41A3CC",
        "已证明是class2条目状态表现门；+0x38的作者UI属性名仍UNKNOWN，不把它擅自命名成‘灰色/禁用颜色’。",
    )

    # slot2：Enemy AI。AI先把一个候选过滤布尔设为1；若角色状态或任一slot2有效则设0。
    # 传到 0x444330 后，布尔为0且候选FIRTTECH class==2，候选flag直接清0。
    add(
        2,
        "Enemy AI候选过滤",
        "任一九槽实例slot2有效 -> 传给0x444330的布尔=0；候选FIRTTECH machine class2被拒绝。",
        "0x4442C2..0x4442E7; 0x4443AD..0x4443D2",
        "只冻结machine class2过滤；class2作者源码枚举名仍UNKNOWN。",
    )

    # slot2：主 DamageInfo 计算。在公式wrapper选择到的manager slot2有效且当前FIRTTECH class2时，
    # 两个 DamageInfo 数值DWORD和两个局部结果量被写0。
    add(
        2,
        "DamageInfo结果保护门",
        "选定manager的slot2有效且当前FIRTTECH machine class2 -> local+0x20/local+0x14以及DamageInfo+0x04/+0x08清0。",
        "0x42CA07..0x42CA32 via 0x42BF40",
        "字段作者名仍UNKNOWN；可冻结为class2结果归零，不扩大为所有class2副作用都被取消。",
    )

    # slot3：phase2 中按目标role slot选对应manager查询。有效时把两条待应用结果累计到全局，
    # 然后把原结果两项清0，因此目标本轮不会按普通路径吃到这两项。
    add(
        3,
        "Battle result phase2拦截",
        "目标所属manager的slot3有效 -> 把结果结构两个DWORD累加进0x89FD84，然后把这两个DWORD清0。",
        "0x4212CE..0x421374 via 0x423070",
        "这两DWORD对应result结构的两个数值通道；作者字段正式名仍UNKNOWN。",
    )

    # slot3：后续 phase1 消费累计值。0x4230B0 用当前角色组反向选择另一manager，找到slot3 owner。
    # 对正累计值，magic division 等价于 floor(sum/15)，从owner的次资源current扣除。
    add(
        3,
        "被拦截结果转为owner次资源代价",
        "若0x89FD84>0且能找到对向manager的slot3 owner -> owner FightRole+0x884 减 floor(0x89FD84/15)；若降到<=0则夹0并清该owner在两个manager的全部九槽效果。",
        "0x4211C5..0x421236 via 0x4230B0/0x43DC40",
        "FightRole+0x884已冻结为次资源current；次资源作者正式属性名仍UNKNOWN。",
    )

    # slot3：两个公式链里还会强制局部布尔门为真。
    add(
        3,
        "公式局部布尔门",
        "slot3有效可直接把两个不同公式路径里的局部判定门设为1/true，绕过各自后续随机/状态判定。",
        "0x42A902..0x42A957; 0x42C884..0x42C8CB",
        "只冻结局部布尔结果；未给这两个局部变量猜作者业务名。",
    )

    # slot4：多个公式 helper 都有明确零值路径，与作者“敌阵神光和治愈咒术失效”文字一致。
    # 但函数内部比较的 subtype/selector 字段正式名并未闭合，因此只写机器条件。
    add(
        4,
        "多条公式结果抑制",
        "slot4有效会在多条公式路径把局部计算量或两个结果DWORD清0；部分路径还要求局部mode/subtype==4。",
        "0x42A9F1..0x42AA11; 0x42AE46..0x42AE63; 0x42AF27..0x42AF44; 0x42C0C4..0x42C0E6; 0x42C48B..0x42C4B4",
        "作者说明支持‘神光/治愈失效’业务族，但内部mode/subtype正式字段名仍UNKNOWN。",
    )

    # slot6：主 DamageInfo 计算对 machine class0/1 做硬清零。
    add(
        6,
        "DamageInfo结果保护门",
        "选定manager的slot6有效且当前FIRTTECH machine class0或class1 -> local+0x14以及DamageInfo+0x04/+0x08清0。",
        "0x42CA32..0x42CA61 via 0x42BF40",
        "只冻结machine class0/1结果归零；class1作者源码枚举名仍UNKNOWN。",
    )

    # slot7：角色所属manager有效时百分比参数直接变200。
    add(
        7,
        "角色行动计时百分比",
        "默认100；角色所属manager的slot7有效时直接改为200，并写入计时结构mode1/mode2的+0x2C/+0x30百分比。",
        "0x4204C6..0x42050A; 0x4205FE..0x420614; 0x424090; 0x423EEC/0x423F1A",
        "机器只证明这两个计时通道按200%推进；不要仅凭作者‘三倍’文字把底层参数改写成300。",
    )

    # slot8：在slot7处理之后，把“当前百分比”做整数除2。
    add(
        8,
        "角色行动计时百分比",
        "角色所属manager的slot8有效时把当前百分比整数除2；故仅slot8=50，仅slot7=200，两者同时=100。",
        "0x420505..0x420553; 0x4205FE..0x420614; 0x424090; 0x423EEC/0x423F1A",
        "冻结的是计时结构两个百分比通道的机器参数，不把它直接等同为墙钟速度。",
    )

    # 所有slot共有的 owner 状态门。只要角色还是任一active slot的owner，Battle状态机就不执行该转换。
    # 这不是某个单独slot的效果，因此用slot=-1表示“九槽共有”。
    rows.append(
        ConsumerRow(
            slot=-1,
            record_id_hex="ALL",
            name="九槽共有owner门",
            consumer="Battle角色状态转换",
            machine_effect="当角色状态满足该分支前置条件时，若任一manager的0x43DC10(owner)返回非-1，则跳过0x7D6 -> 0x7D1转换；只有两个manager都无该owner active slot才转换。",
            evidence_addresses="0x442D01..0x442D5D",
            boundary="0x7D6/0x7D1作者状态枚举名未闭合，因此只冻结数值状态转换门。",
        )
    )

    return rows


# ---------------------------------------------------------------------------
# 八、生成结构化机器结论
# ---------------------------------------------------------------------------

def build_machine_conclusions() -> Dict[str, object]:
    """把固化59已闭合的控制流整理成 JSON 中的核心结论。"""

    # 先把四种 slot7/slot8 组合全部算出来，避免文档里手抄错。
    speed_matrix = {
        "slot7=0,slot8=0": slot7_slot8_percent(False, False),
        "slot7=1,slot8=0": slot7_slot8_percent(True, False),
        "slot7=0,slot8=1": slot7_slot8_percent(False, True),
        "slot7=1,slot8=1": slot7_slot8_percent(True, True),
    }

    # slot3 cost 选几个边界样本，直观看出 1..14 不扣、15 扣1、30扣2。
    slot3_cost_examples = {
        str(value): signed_divide_by_15_cost(value)
        for value in (1, 14, 15, 16, 29, 30, 31, 150, 151, 1500)
    }

    return {
        "0x43DA50通用机器谓词": {
            "非法slot": "false",
            "active==0": "false",
            "step<0": "current>=0 -> true；current<0 -> active清0并false",
            "step==0": "false",
            "step>0": "current<0 -> false；current>=0 -> 返回true并立即active清0",
            "canonical简化": "九条canonical均positive start + step=-1100；在正常0x43DAB0 tick管理下可作为‘该slot仍在有效窗口’谓词使用。",
            "重要边界": "generic非canonical输入不能把0x43DA50简化成单纯active!=0。",
        },
        "manager别名": {
            "0x8B01CC": "0x89FFD0",
            "0x8B01D0": "0x89FE08",
            "0x89FD58": "0x89FFD0",
            "0x89FD54": "0x89FE08",
            "结论": "固化59全部高层consumer仍只访问固化58的两个九槽实例，不存在第三套同布局manager。",
        },
        "按role槽组选manager": {
            "0x423070": "role 0..7 -> 0x89FFD0；role 8..15 -> 0x89FE08；其它 -> false",
            "0x4230B0 flag0": "当前role 0..7 -> 查0x89FE08 owner；role 8..15 -> 查0x89FFD0 owner，即对向manager",
            "0x4230B0 flag1": "当前role 0..7 -> 查0x89FFD0 owner；role 8..15 -> 查0x89FE08 owner，即同组manager",
        },
        "被有效性查询家族消费的slot": list(QUERY_CONSUMED_SLOTS),
        "未发现0x43DA50_43DC40有效性消费的slot": list(NO_VALIDITY_QUERY_SLOTS),
        "slot7_slot8计时百分比组合": speed_matrix,
        "slot3_owner次资源代价样本_floor_sum_div15": slot3_cost_examples,
        "Godot实现要点": [
            "必须保留两个九槽manager，而不是把查询折叠成一张全局9-bit表。",
            "slot2同时影响UI状态表现、Enemy AI class2候选和class2结果归零。",
            "slot3 result拦截要跨phase保存0x89FD84累计值，并向对向slot3 owner收取floor(sum/15)次资源。",
            "slot4有多条公式零值consumer；不要只播放持续EFF而忽略数值抑制。",
            "slot6对machine class0/1结果做硬清零。",
            "slot7/8修改的是计时结构mode1/mode2百分比：200/50，二者同时回到100。",
            "角色拥有任一active九槽效果时，特定0x7D6->0x7D1状态转换必须暂缓。",
        ],
        "明确不能外推": [
            "固化59本工具本身只能证明slot0/1/5没有本查询家族consumer；固化63 companion工具已进一步关闭当前同版canonical静态direct额外数值consumer面，但共享建立/表现/生命周期/owner清理/状态转换门仍存在。",
            "slot7底层写200，不得仅凭作者‘三倍’说明把此字段强写300。",
            "0x7D6/0x7D1没有作者枚举名，不得擅自命名。",
            "class1/class2作者源码枚举名仍UNKNOWN。",
            "slot4内部mode/subtype==4字段正式名仍UNKNOWN。",
        ],
    }


# ---------------------------------------------------------------------------
# 九、输出文件
# ---------------------------------------------------------------------------

def write_csv(path: Path, rows: Sequence[ConsumerRow]) -> None:
    """把高层 consumer 表写成严格 UTF-8-SIG CSV。"""

    # Excel / Windows 中文环境打开 UTF-8-SIG 更稳，因此沿用项目既有严格CSV习惯。
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(
            [
                "slot",
                "record_id",
                "名称",
                "consumer",
                "机器效果",
                "证据地址",
                "边界",
            ]
        )
        for row in rows:
            writer.writerow(
                [
                    row.slot,
                    row.record_id_hex,
                    row.name,
                    row.consumer,
                    row.machine_effect,
                    row.evidence_addresses,
                    row.boundary,
                ]
            )


def write_json(path: Path, payload: Dict[str, object]) -> None:
    """把报告以稳定缩进写成 UTF-8 JSON。"""

    path.parent.mkdir(parents=True, exist_ok=True)
    # ensure_ascii=False 保留中文，方便人工审查；sort_keys=False 保留阅读顺序。
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


# ---------------------------------------------------------------------------
# 十、主调查流程
# ---------------------------------------------------------------------------

def inspect(original_exe: Path, research_exe: Path, firttech: Path) -> Tuple[Dict[str, object], List[ConsumerRow]]:
    """执行固化59全部静态核对并返回 JSON payload + CSV rows。"""

    # 第一步：载入两份 EXE。原版负责定义行为，研究版只做“关键区间是否未被历史补丁碰到”的回归。
    original = parse_pe(original_exe)
    research = parse_pe(research_exe)

    # 第二步：整文件哈希必须命中冻结基线。
    original_hash = sha256_file(original_exe)
    research_hash = sha256_file(research_exe)
    input_hashes = {
        "原版RPG.exe": {
            "实际": original_hash,
            "预期": EXPECTED_ORIGINAL_RPG_SHA256,
            "PASS": original_hash == EXPECTED_ORIGINAL_RPG_SHA256,
        },
        "研究版RPG.exe": {
            "实际": research_hash,
            "预期": EXPECTED_RESEARCH_RPG_SHA256,
            "PASS": research_hash == EXPECTED_RESEARCH_RPG_SHA256,
        },
    }

    # 第三步：把所有 direct CALL 集合重新扫描出来。
    # 这里不是“找到几个看起来像调用的位置”就算结束，而是扫描整个原版 .text。
    # 每一个目标函数都同时保存“预期调用地址集合”和“实际扫描地址集合”。
    # 两个集合只要少一个、多一个、或某个地址不同，该项目就会直接判定 FAIL。
    # 这样以后即使重做反汇编，也不会因为人工漏抄调用点而悄悄改变结论。
    call_checks = [
        verify_calls(original, 0x0043DA50, EXPECTED_43DA50_CALLS, "九槽有效性谓词"),
        verify_calls(original, 0x0043DC10, EXPECTED_43DC10_CALLS, "按owner查active slot"),
        verify_calls(original, 0x0043DC40, EXPECTED_43DC40_CALLS, "按slot查owner"),
        verify_calls(original, 0x0043DC60, EXPECTED_43DC60_CALLS, "按owner清理全部slot"),
        verify_calls(original, 0x0042BF40, EXPECTED_42BF40_CALLS, "公式侧manager/slot查询wrapper"),
        verify_calls(original, 0x00423070, EXPECTED_423070_CALLS, "按目标role组查slot有效性wrapper"),
        verify_calls(original, 0x004230B0, EXPECTED_4230B0_CALLS, "按当前role和方向flag查slot owner wrapper"),
        verify_calls(original, 0x0042A760, EXPECTED_42A760_CALLS, "Battle到公式层全局manager指针绑定"),
    ]

    # 第四步：逐段验证本节点人工解释依赖的机器码。
    # “短机器码断言”用来确认某个关键比较、跳转、常量或 CALL 仍然原样存在。
    # “整段哈希断言”则负责保护更大的控制流范围，防止只看短锚点时漏掉旁边的改动。
    # 同一批整段哈希还会拿研究版 EXE 与完全原版逐段比较。
    # 因而本节点可以明确区分“原版行为”与“历史研究补丁可能造成的行为”。
    byte_checks = [verify_assertion(original, item) for item in BYTE_ASSERTIONS]
    slice_checks = verify_slice_hashes(original, research)

    # 第五步：重新解密 FIRTTECH，确认 slot 名称/ID/class 没换。
    # 这里重新读资源，而不是把“反咒禁制=slot2”之类的名字直接写死进最终报告。
    # 这样如果有人误拿另一版本 Firttech.enc，输入哈希和九项 ID 对齐都会同时报警。
    # 资源记录中的作者说明也由本次输入重新取出，方便把机器行为和作者文字并排审查。
    slot_views, firttech_hashes = build_slot_views(firttech)
    slot_id_pass = [view.record_id for view in slot_views] == list(SLOT_TO_ID)

    # 第六步：生成面向兼容实现的 consumer 表。
    # 表格只写已经证明的“某槽在哪条高层链被怎样消费”。
    # 没有走这套 validity-query 家族的 slot0/1/5 会明确标成“本家族未发现 consumer”。
    # 固化63 companion工具进一步穷举全PE静态direct面后，已经关闭“额外数值consumer”边界；
    # 但共享的建立、Effect Manager表现、生命周期、owner清理和状态转换门仍然必须实现。
    rows = build_consumer_rows(slot_views)

    # 第七步：汇总所有 PASS 条件。这里故意用 all()，任何一个断言失败都会让整份报告 FAIL。
    all_pass = (
        all(item["PASS"] for item in input_hashes.values())
        and all(item["PASS"] for item in call_checks)
        and all(item["PASS"] for item in byte_checks)
        and all(item["PASS"] for item in slice_checks)
        and bool(firttech_hashes["PASS"])
        and slot_id_pass
        and slot7_slot8_percent(False, False) == 100
        and slot7_slot8_percent(True, False) == 200
        and slot7_slot8_percent(False, True) == 50
        and slot7_slot8_percent(True, True) == 100
        and signed_divide_by_15_cost(14) == 0
        and signed_divide_by_15_cost(15) == 1
        and signed_divide_by_15_cost(30) == 2
    )

    # 第八步：构造完整 JSON。报告把“证据”和“解释”分栏，避免以后只看到一句总结却找不到依据。
    payload: Dict[str, object] = {
        "工具": Path(__file__).name,
        "工具版本": TOOL_VERSION,
        "节点": "固化59",
        "研究主题": "Battle九项场域有效性查询与高层效果消费边界",
        "输入": {
            "原版EXE": str(original_exe.resolve()),
            "研究版EXE": str(research_exe.resolve()),
            "Firttech": str(firttech.resolve()),
        },
        "输入哈希": input_hashes,
        "Firttech哈希": firttech_hashes,
        "direct_CALL穷举": call_checks,
        "短机器码断言": byte_checks,
        "关键区间原版冻结与研究版等价": slice_checks,
        "九项canonical记录": [asdict(view) for view in slot_views],
        "高层consumer": [asdict(row) for row in rows],
        "机器结论": build_machine_conclusions(),
        "纠错记录": {
            "此前进度口头误报": "曾把0x43DA50 direct CALL口头写成14处。",
            "正式静态结果": "严格12处；已用rel32扫描和冻结地址集合双重断言。",
            "是否影响先前固化节点": False,
        },
        "全部通过": all_pass,
    }
    return payload, rows


# ---------------------------------------------------------------------------
# 十一、命令行
# ---------------------------------------------------------------------------

def build_parser() -> argparse.ArgumentParser:
    """建立命令行参数解析器。"""

    parser = argparse.ArgumentParser(
        description="核对《幽城幻剑录》Battle九项场域有效性查询与高层consumer。"
    )
    # 完全原版是主 Oracle。
    parser.add_argument("--original-exe", type=Path, required=True, help="完全未修改的原版 RPG.exe")
    # 研究版只用于关键区间等价回归。
    parser.add_argument("--research-exe", type=Path, required=True, help="历史研究版 RPG.exe")
    # canonical Firttech.enc 用来恢复九项作者名称与class。
    parser.add_argument("--firttech", type=Path, required=True, help="canonical Firttech.enc")
    # JSON 是本节点最完整的机器结果。
    parser.add_argument("--json-out", type=Path, required=True, help="输出 JSON 路径")
    # CSV 是兼容引擎实现时最方便逐条查阅的consumer表。
    parser.add_argument("--csv-out", type=Path, required=True, help="输出 CSV 路径")
    return parser


def main() -> int:
    """命令行入口。"""

    parser = build_parser()
    args = parser.parse_args()

    # 所有输入必须先存在；错误路径不允许生成半份报告。
    for label, path in (
        ("original-exe", args.original_exe),
        ("research-exe", args.research_exe),
        ("firttech", args.firttech),
    ):
        if not path.is_file():
            parser.error(f"{label} 不存在或不是文件: {path}")

    # 执行全部核对。
    payload, rows = inspect(args.original_exe, args.research_exe, args.firttech)

    # 即使最终 FAIL，也把 JSON/CSV 写出来；这样失败证据可以被封包保留和复盘。
    write_json(args.json_out, payload)
    write_csv(args.csv_out, rows)

    # 控制台只输出很短的摘要，详细内容全部在 JSON。
    print(f"solid59 all_pass={payload['全部通过']}")
    print(f"0x43DA50 direct_calls={len(payload['direct_CALL穷举'][0]['实际caller'])}")
    print(f"consumer_rows={len(rows)}")
    print(f"json={args.json_out}")
    print(f"csv={args.csv_out}")

    # CI/封包审计可以根据退出码判断：0=全部通过，1=有机器断言失败。
    return 0 if payload["全部通过"] else 1


if __name__ == "__main__":
    sys.exit(main())
