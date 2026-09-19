#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""固化68补充：独立复核原版启动、战斗条目、角色通道与显示电影路径。

本工具的输入只包含两份已知RPG.exe和三份原始ENC资源。它不读取任何第三方
项目，也不把第三方项目的功能名当作证据。工具回答五组问题：

1. 原版怎样寻找PlayDisk及 ``MultiMedia\\Mov``；
2. ``MiscInfo.ENC`` 六个动作槽与 ``0x423550`` 清理上界是否完全对应；
3. RoleDefinition ``+0x68..+0x88`` 九个字段与八个运行时重算通道怎样对齐；
4. 既有存档、掉落、成长、经验和金钱位点是否仍与当前原版一致；
5. 原版窗口/DirectDraw/Bink路径实际锁定、解锁和呈现哪一个表面。

“八通道”“清理槽”“兼容候选”是兼容层中性名称。只有Firttech资源里的动作名
属于作者文本。没有作者文字或闭合consumer的字段仍保持UNKNOWN/RAW-PRESERVE。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
import subprocess
from pathlib import Path
from typing import Any, Dict, Iterable, List, Sequence, Tuple

from youcheng_world_encounter_duration_inspector import PEImage


TOOL_VERSION = "0.7D-solid68.1"

EXPECTED_HASHES = {
    "original": "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f",
    "research": "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf",
    "misc_encoded": "79d2ec47548dae215050eb3e79f137a9c028d41b64fe6c196e9cdd21f1e9e3e3",
    "misc_decoded": "cd18d110d458fe60928eb874f76453f751489cf2d0341d2c7c250c4ece13e6e9",
    "firttech_encoded": "32b98193c037c8cd75a11534520e189c89c856c367bc1d6ac607e43d9cf52a5d",
    "firttech_decoded": "5469a9dd388972b53b5661e90d85c84f2d0150d06f46ddbae159409a5ac34b1a",
    "api_encoded": "ff9de0da24113242f352e0cb7be3cd6e3032403399d547952f8c5ab10e94d907",
    "api_decoded": "b56346ad8909000b84ac24f9ad91b44813d8267053dd7a8d30673197129e825d",
}


# 原版关键范围哈希。含历史补丁字节的范围只要求原版哈希；未受补丁影响的范围
# 还会额外要求两版逐字节相同。
CRITICAL_SLICES: Tuple[Tuple[int, int, str, str, bool], ...] = (
    (0x00402A10, 0x00402AA0, "盘符与MultiMediaMov定位", "f8d1d38ff4c63965eb67b0acb0fc257e653bee07e67c1a1d0ced890cfc903906", False),
    (0x00428400, 0x004284C0, "MiscInfo固定长度装载器", "853e0fa8c83c132a747c0498706bf77fe5ec10d29b8b6771008e51d6afaad9f5", True),
    (0x00423100, 0x004231D0, "候选动作追加与删除primitive", "660a29066bc4d3fad765f4a33685c2ebae003d2498c172f0f3311b69ca8dc292", True),
    (0x004232D0, 0x00423468, "MiscInfo选择槽临时动作加入链", "1e369e4e18840efae33ed31cd308d7880f6f1efb3d50bb7a73ae0847489fbc8f", True),
    (0x00423470, 0x00423598, "当前槽与全槽动作清理链", "6a02f4b199760c9583141edc32cf81f06e7da9f71027497c43192ddb934a7021", False),
    (0x00441F80, 0x00442034, "清理后RoleDefinition回写", "8ed75c79e94dc9c44fa244e7fc6c63f5291cd520a153e8c542be330b45793860", True),
    (0x0041F4A4, 0x0041F570, "八个运行时通道重算入口", "948bd8e8e5ac57431ab2c14bab1f190923d1e07a8bd854e1138b0bef7b9fd1bc", True),
    (0x0043BC20, 0x0043BD38, "八通道派生helper前半", "603db64acfa11ba80d817dbfa9cd4fd2080ad06eae5923a5342bf9654a5abf22", True),
    (0x00440C60, 0x00440F60, "角色界面值回写RoleDefinition", "08e6d6eb40429e2b87dafca17298d133aa5205b566ff3a07eefa8f8461f39945", False),
    (0x00401820, 0x00401880, "640乘480窗口创建", "05206dd717464f8231e4b228a0eee763fac47cf0624d0583676c47e53f823f69", True),
    (0x00405B30, 0x00405BC8, "DirectDraw表面描述符构造", "00f782c26b266587f044bb8ce7354ceba92cf254c1c1ab46343c533823b56c0e", True),
    (0x004064E0, 0x00406589, "全屏Flip与窗口Blt呈现分流", "0ac95c7399d510921b2fcb12bc6485dcc817626dc5e9959e5bbcc56207f7a271", True),
    (0x00401BA0, 0x00401C49, "Bink逐帧复制链", "a9fc6fd46f38d54bc74ba016f86561386b9b656a39b7d443cadf6be46e5d967a", True),
    (0x00406230, 0x004062D9, "DirectDraw表面锁定与解锁helper", "f6f747e2d7b853bb58ceb517fc55d9c9ff9710f34928bb7b29678efbb8923912", True),
)


# 三个历史差异簇。地址是完整指令起点，不是差异字节本身。
PATCH_SITES: Tuple[Tuple[str, int, str, str, str], ...] = (
    ("PlayDisk盘型门", 0x00402A3F, "83f80575", "83f8027c", "新增独立闭合"),
    ("MultiMediaMov存在门", 0x00402A62, "742e", "eb2e", "新增独立闭合"),
    ("MiscInfo全槽清理上界", 0x00423590, "83fe28", "83fe30", "新增独立闭合"),
    ("RoleDefinition回写通道3", 0x00440E90, "89543974", "89543970", "新增独立闭合"),
    ("RoleDefinition回写通道4", 0x00440EB6, "89543978", "89543974", "新增独立闭合"),
    ("RoleDefinition回写通道5", 0x00440EDC, "8954397c", "89543978", "新增独立闭合"),
    ("RoleDefinition回写通道6", 0x00440F02, "89943980000000", "8994397c000000", "新增独立闭合"),
    ("RoleDefinition回写通道7", 0x00440F2B, "89943984000000", "89943980000000", "新增独立闭合"),
    ("RoleDefinition回写尾通道", 0x00440F54, "89843a88000000", "89843a84000000", "新增独立闭合"),
)


# 已在当前研究包闭合过的候选位点。这里再次读原版，但不会重复制造一套业务文档。
EXISTING_SITES: Tuple[Tuple[str, int, str, str], ...] = (
    ("MapSCI加380存档许可getter", 0x0040A0C2, "8b8080030000", "既有覆盖并复核"),
    ("战后掉落百分阈值RNG", 0x00443A22, "99b964000000f7f9", "既有覆盖并复核"),
    ("升级随机增长一", 0x00443BB9, "99b903000000f7f9", "既有覆盖并复核"),
    ("升级随机增长二", 0x00443BFB, "99b9030000006a00f7f9", "既有覆盖并复核"),
    ("随机闭区间helper尾部", 0x00443D7D, "f7fe8bc203c7", "既有覆盖并复核"),
    ("经验累计写回", 0x00443856, "03c3894724", "既有覆盖并复核"),
    ("经验显示参数", 0x004437F7, "8b0db0b2460053", "既有覆盖并复核"),
    ("金钱累计写回", 0x00439824, "8b91d85d000003d0", "既有覆盖并复核"),
    ("金钱显示参数", 0x004439AF, "8b088b50085155528d8424b4000000", "既有覆盖并复核"),
)


# 窗口与电影路径的原版短断言。它们不是建议补丁的验收结果，只证明原版做了什么。
DISPLAY_SITES: Tuple[Tuple[str, int, str], ...] = (
    ("窗口宽640高480与WS_POPUP", 0x0040182C, "68e001000068800200006a006a006800000080"),
    ("CreateWindowExA间接调用", 0x00401853, "ff15d8014600"),
    ("DirectDraw全屏标志分支", 0x00405C5B, "8a463084c00f843e020000"),
    ("表面helper只设置size_flags_caps", 0x00405B4D, "c74424087c00000085c0c744240c07000100c744247040000000"),
    ("呈现函数读取同一加30标志", 0x004064E6, "8a463084c0742c"),
    ("全屏路径Surface_Flip", 0x004064ED, "8b76046a016a00568b06ff502c"),
    ("窗口路径GetClientRect", 0x00406519, "a184f346008d542408575250ff15e8014600"),
    ("窗口路径Back到Primary_Blt", 0x0040654D, "8b4e088b46046a0068000000018b106a0051"),
    ("Bink默认像素格式9", 0x00401BD3, "bf09000000"),
    ("16位条件改用格式10", 0x00401BD8, "83793c107505bf0a000000"),
    ("Bink锁定helper传零", 0x00401BE3, "6a00e846460000"),
    ("Bink使用renderer加40步长", 0x00401C01, "8b4940"),
    ("Bink解锁helper传零", 0x00401C1D, "6a00e89c460000"),
    ("锁定helper零参数默认renderer加4", 0x0040623D, "85ed5775038b6904"),
    ("解锁helper零参数默认renderer加4", 0x004062C0, "8b44240485c075038b4104"),
)


EXPECTED_CALLERS: Dict[int, Tuple[int, ...]] = {
    0x00401820: (0x0040167F,),
    0x00405B30: (0x00405FDE, 0x00406093),
    0x004064E0: (0x00405A2A,),
    0x00423170: (0x0042350E, 0x0042351A, 0x0042356C, 0x00423588),
    0x00423550: (0x00441FBE,),
    0x00440C60: (0x0043503D, 0x004359F9, 0x00435FFA),
    0x00406230: (0x00401BE5, 0x0040637D, 0x004063DE, 0x00406457),
    0x004062C0: (0x00401C1F, 0x004063CD, 0x0040642E, 0x004064A9),
}


EXPECTED_IMPORTS: Dict[int, Tuple[str, str]] = {
    0x004600CC: ("KERNEL32.dll", "GetFileAttributesA"),
    0x004600D4: ("KERNEL32.dll", "GetLastError"),
    0x00460124: ("KERNEL32.dll", "GetDriveTypeA"),
    0x00460194: ("USER32.dll", "wsprintfA"),
    0x004601D8: ("USER32.dll", "CreateWindowExA"),
    0x004601E8: ("USER32.dll", "GetClientRect"),
    0x004601F0: ("USER32.dll", "ClientToScreen"),
    0x004601F8: ("USER32.dll", "MoveWindow"),
    0x00460200: ("USER32.dll", "AdjustWindowRectEx"),
    0x0046024C: ("binkw32.dll", "_BinkCopyToBuffer@28"),
}


EXPECTED_ACTION_NAMES = {
    0x17C: "封炎滅陣",
    0x17F: "冰魄晶壁",
    0x17D: "紫電絕號",
    0x17E: "天護靈燁",
    0x180: "冥殺獄陣",
}

EXPECTED_MISC_PAIRS = ((0, 0), (0, 0x17C), (0, 0x17F), (0, 0x17D), (0, 0x17E), (0, 0x180))


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def decode_enc(data: bytes) -> bytes:
    """重放游戏当前已闭合的逐字节XOR层。"""

    return bytes(value ^ (index % 255) for index, value in enumerate(data))


def c_string(data: bytes, encoding: str = "ascii") -> str:
    return data.split(b"\0", 1)[0].decode(encoding, errors="strict")


def read_c_string_va(pe: PEImage, va: int, encoding: str = "ascii") -> str:
    raw = bytearray()
    while True:
        value = pe.read_va(va + len(raw), 1)[0]
        if value == 0:
            return bytes(raw).decode(encoding, errors="strict")
        raw.append(value)
        if len(raw) > 4096:
            raise ValueError(f"VA 0x{va:08X}字符串超过安全上限")


def direct_callers(pe: PEImage, target: int) -> Tuple[int, ...]:
    """枚举.text中E8 rel32直接调用；不声称覆盖函数指针调用。"""

    base, blob = pe.text_section_bytes()
    result: List[int] = []
    for index in range(len(blob) - 4):
        if blob[index] != 0xE8:
            continue
        displacement = struct.unpack_from("<i", blob, index + 1)[0]
        caller = base + index
        if caller + 5 + displacement == target:
            result.append(caller)
    return tuple(result)


def verify_inputs(
    original: PEImage,
    research: PEImage,
    misc_path: Path,
    firttech_path: Path,
    api_path: Path,
) -> Tuple[Dict[str, Any], bytes, bytes, bytes]:
    misc_encoded = misc_path.read_bytes()
    firttech_encoded = firttech_path.read_bytes()
    api_encoded = api_path.read_bytes()
    misc = decode_enc(misc_encoded)
    firttech = decode_enc(firttech_encoded)
    api = decode_enc(api_encoded)

    rows = {
        "完全未修改原版RPG.exe": {
            "路径": str(original.path), "字节数": len(original.data),
            "SHA-256": sha256(original.data), "预期": EXPECTED_HASHES["original"],
        },
        "历史研究版RPG.exe": {
            "路径": str(research.path), "字节数": len(research.data),
            "SHA-256": sha256(research.data), "预期": EXPECTED_HASHES["research"],
        },
        "MiscInfo.ENC": {
            "路径": str(misc_path), "字节数": len(misc_encoded),
            "编码SHA-256": sha256(misc_encoded), "编码预期": EXPECTED_HASHES["misc_encoded"],
            "解码SHA-256": sha256(misc), "解码预期": EXPECTED_HASHES["misc_decoded"],
            "预期字节数": 0x41E,
        },
        "Firttech.enc": {
            "路径": str(firttech_path), "字节数": len(firttech_encoded),
            "编码SHA-256": sha256(firttech_encoded), "编码预期": EXPECTED_HASHES["firttech_encoded"],
            "解码SHA-256": sha256(firttech), "解码预期": EXPECTED_HASHES["firttech_decoded"],
            "预期字节数": 500 * 0x230,
        },
        "Api.enc": {
            "路径": str(api_path), "字节数": len(api_encoded),
            "编码SHA-256": sha256(api_encoded), "编码预期": EXPECTED_HASHES["api_encoded"],
            "解码SHA-256": sha256(api), "解码预期": EXPECTED_HASHES["api_decoded"],
            "预期字节数": 255 * 0x350,
        },
    }
    for name, row in rows.items():
        if name.endswith("RPG.exe"):
            row["PASS"] = row["SHA-256"] == row["预期"] and row["字节数"] == 462848
        else:
            row["PASS"] = (
                row["编码SHA-256"] == row["编码预期"]
                and row["解码SHA-256"] == row["解码预期"]
                and row["字节数"] == row["预期字节数"]
            )
    return rows, misc, firttech, api


def verify_imports(pe: PEImage) -> List[Dict[str, Any]]:
    rows: List[Dict[str, Any]] = []
    for va, expected in EXPECTED_IMPORTS.items():
        item = pe.import_at_iat_va(va)
        actual = None if item is None else (item.get("DLL"), item.get("函数"))
        rows.append({
            "IAT地址": f"0x{va:08X}",
            "实际DLL": None if actual is None else actual[0],
            "实际函数": None if actual is None else actual[1],
            "预期DLL": expected[0],
            "预期函数": expected[1],
            "PASS": actual == expected,
        })
    return rows


def verify_slices(original: PEImage, research: PEImage) -> List[Dict[str, Any]]:
    rows: List[Dict[str, Any]] = []
    for start, end, label, expected_hash, require_equal in CRITICAL_SLICES:
        original_bytes = original.read_va(start, end - start)
        research_bytes = research.read_va(start, end - start)
        actual_hash = sha256(original_bytes)
        rows.append({
            "名称": label,
            "范围": f"0x{start:08X}..0x{end:08X}",
            "长度": end - start,
            "原版SHA-256": actual_hash,
            "预期SHA-256": expected_hash,
            "要求双版本一致": require_equal,
            "双版本逐字节一致": original_bytes == research_bytes,
            "PASS": actual_hash == expected_hash and (not require_equal or original_bytes == research_bytes),
        })
    return rows


def verify_patch_sites(original: PEImage, research: PEImage) -> List[Dict[str, Any]]:
    rows: List[Dict[str, Any]] = []
    for label, va, original_hex, research_hex, category in PATCH_SITES:
        size = len(bytes.fromhex(original_hex))
        ob = original.read_va(va, size).hex()
        rb = research.read_va(va, size).hex()
        rows.append({
            "主题": label, "VA": f"0x{va:08X}",
            "原版HEX": ob, "原版预期HEX": original_hex,
            "研究版HEX": rb, "研究版预期HEX": research_hex,
            "分类": category,
            "PASS": ob == original_hex and rb == research_hex,
        })
    return rows


def verify_existing_sites(original: PEImage, research: PEImage) -> List[Dict[str, Any]]:
    rows: List[Dict[str, Any]] = []
    for label, va, expected_hex, category in EXISTING_SITES:
        expected = bytes.fromhex(expected_hex)
        ob = original.read_va(va, len(expected)).hex()
        rb = research.read_va(va, len(expected)).hex()
        rows.append({
            "主题": label, "VA": f"0x{va:08X}", "原版HEX": ob,
            "研究版HEX": rb, "预期HEX": expected_hex, "分类": category,
            "PASS": ob == expected_hex and rb == expected_hex,
        })
    return rows


def verify_no_cd(original: PEImage, research: PEImage) -> Dict[str, Any]:
    strings = {
        "失败提示": (0x00468370, "Please insert PlayDisk(4th) into CD_ROM "),
        "电影目录格式": (0x004683B0, "%c:\\MultiMedia\\Mov"),
        "盘根格式": (0x004683C4, "%c:\\"),
    }
    string_rows = []
    for label, (va, expected) in strings.items():
        actual = read_c_string_va(original, va)
        string_rows.append({"名称": label, "VA": f"0x{va:08X}", "实际": actual, "预期": expected, "PASS": actual == expected})

    assertions = (
        (0x00402A23, "b341", "从盘符A开始"),
        (0x00402A3F, "83f8057520", "原版只接受GetDriveTypeA返回5"),
        (0x00402A58, "e881a20500", "调用0x45CCDE存在性检查"),
        (0x00402A60, "85c0742e", "检查返回0才采用当前盘符"),
        (0x00402A64, "fec380fb5b7cba", "逐盘符递增直到Z"),
        (0x00402A94, "881dc8f54600", "保存最终盘符到0x46F5C8"),
        (0x0045CCDE, "ff742404ff15cc00460083f8ff7511", "存在性helper调用GetFileAttributesA"),
    )
    byte_rows = []
    for va, expected, label in assertions:
        actual = original.read_va(va, len(bytes.fromhex(expected))).hex()
        byte_rows.append({"名称": label, "VA": f"0x{va:08X}", "实际HEX": actual, "预期HEX": expected, "PASS": actual == expected})

    return {
        "作者字符串": string_rows,
        "关键机器码": byte_rows,
        "原版行为": (
            "从A到Z枚举盘符；仅GetDriveTypeA返回5的盘继续检查；"
            "格式化%c:\\MultiMedia\\Mov后经GetFileAttributesA包装helper确认存在，"
            "成功盘符写入0x0046F5C8，否则显示PlayDisk提示。"
        ),
        "研究版差异": (
            "盘型门由等于5改成返回值不小于2，并把目录存在检查成功分支改成无条件采用；"
            "这是历史研究版的实际字节行为，不是原版行为。"
        ),
        "PASS": all(row["PASS"] for row in string_rows + byte_rows),
    }


def parse_firttech_actions(decoded: bytes) -> Dict[int, Dict[str, Any]]:
    result: Dict[int, Dict[str, Any]] = {}
    for physical_index in range(500):
        record = decoded[physical_index * 0x230:(physical_index + 1) * 0x230]
        record_id = struct.unpack_from("<I", record, 0)[0]
        if record_id not in EXPECTED_ACTION_NAMES:
            continue
        result[record_id] = {
            "物理索引": physical_index,
            "记录ID": record_id,
            "作者名": c_string(record[4:4 + 0x14], "big5"),
            "作者说明": c_string(record[0x17C:0x230], "big5"),
        }
    return result


def verify_misc_cleanup(original: PEImage, research: PEImage, misc: bytes, firttech: bytes) -> Dict[str, Any]:
    pairs = tuple(
        (struct.unpack_from("<i", misc, 0x130 + index * 8)[0], struct.unpack_from("<i", misc, 0x134 + index * 8)[0])
        for index in range(6)
    )
    actions = parse_firttech_actions(firttech)
    pair_rows: List[Dict[str, Any]] = []
    for slot, (first, second) in enumerate(pairs):
        action = actions.get(second)
        pair_rows.append({
            "槽": slot,
            "MiscInfo偏移": f"+0x{0x130 + slot * 8:X}/+0x{0x134 + slot * 8:X}",
            "第一DWORD": first,
            "第二DWORD": second,
            "第二DWORD十六进制": f"0x{second:X}",
            "Firttech作者名": None if action is None else action["作者名"],
            "选择值": slot,
        })

    action_rows = []
    for action_id, expected_name in EXPECTED_ACTION_NAMES.items():
        row = actions.get(action_id)
        action_rows.append({
            "记录ID": action_id, "记录ID十六进制": f"0x{action_id:X}",
            "实际作者名": None if row is None else row["作者名"],
            "预期作者名": expected_name,
            "作者说明": None if row is None else row["作者说明"],
            "PASS": row is not None and row["作者名"] == expected_name,
        })

    original_bound = original.read_va(0x00423590, 3).hex()
    research_bound = research.read_va(0x00423590, 3).hex()
    return {
        "EXE资源绑定": {
            "路径字符串": read_c_string_va(original, 0x0046A578),
            "装载长度比较": original.read_va(0x00428466, 5).hex(),
            "全局对象绑定": original.read_va(0x0043A07D, 5).hex(),
            "PASS": (
                read_c_string_va(original, 0x0046A578) == "Public\\MiscInfo.ENC"
                and original.read_va(0x00428466, 5).hex() == "3d1e040000"
                and original.read_va(0x0043A07D, 5).hex() == "a3b4018a00"
            ),
        },
        "六槽原始值": pair_rows,
        "六槽预期": [list(pair) for pair in EXPECTED_MISC_PAIRS],
        "六槽匹配": pairs == EXPECTED_MISC_PAIRS,
        "Firttech作者记录": action_rows,
        "原版循环上界HEX": original_bound,
        "研究版循环上界HEX": research_bound,
        "原版实际槽集合": [0, 1, 2, 3, 4],
        "研究版实际槽集合": [0, 1, 2, 3, 4, 5],
        "被原版遗漏的唯一正值": {"槽": 5, "动作ID": "0x180", "作者名": "冥殺獄陣"},
        "结构结论": (
            "0x4232D0用选择值1..5索引MiscInfo+0x130/+0x134并把正值加入FightRole候选动作池；"
            "0x423470清当前选择槽；0x423550在RoleDefinition回写前清全部槽。"
            "原版ESI=0,8,16,24,32，仅覆盖槽0..4，确实遗漏槽5的动作0x180“冥殺獄陣”；"
            "研究版上界0x30才覆盖槽5。"
        ),
        "PASS": (
            pairs == EXPECTED_MISC_PAIRS
            and all(row["PASS"] for row in action_rows)
            and original_bound == "83fe28"
            and research_bound == "83fe30"
        ),
    }


def verify_role_channels(original: PEImage, research: PEImage, api: bytes) -> Dict[str, Any]:
    fields: List[Dict[str, Any]] = []
    for offset in range(0x68, 0x8C, 4):
        values = [struct.unpack_from("<i", api, index * 0x350 + offset)[0] for index in range(255)]
        fields.append({
            "RoleDefinition偏移": f"+0x{offset:X}",
            "非零记录数": sum(value != 0 for value in values),
            "最小值": min(values), "最大值": max(values),
            "不同值数量": len(set(values)), "等于100记录数": sum(value == 100 for value in values),
        })

    # 0x440C60九次写入的来源控件偏移，以及两版目标偏移。
    source_offsets = (0x580, 0x588, 0x58C, 0x590, 0x598, 0x5A0, 0x5A8, 0x5B0, 0x5B8)
    original_targets = (0x68, 0x6C, 0x70, 0x74, 0x78, 0x7C, 0x80, 0x84, 0x88)
    research_targets = (0x68, 0x6C, 0x70, 0x70, 0x74, 0x78, 0x7C, 0x80, 0x84)
    writes = []
    for source, ot, rt in zip(source_offsets, original_targets, research_targets):
        writes.append({
            "来源对象偏移": f"+0x{source:X}",
            "原版目标": f"RoleDefinition+0x{ot:X}",
            "研究版目标": f"RoleDefinition+0x{rt:X}",
            "研究版效果": "后写覆盖同一+0x70" if source == 0x590 else "直接回写",
        })

    recompute = []
    for selector in range(8):
        destination = 0x8B4 + selector * 4
        role_offset = destination - 0x84C
        recompute.append({
            "selector": selector,
            "FightRole目标": f"+0x{destination:X}",
            "RoleDefinition别名": f"+0x{role_offset:X}",
        })

    expected_stats = {
        0x68: (249, -100, 200, 24, 167),
        0x6C: (251, -100, 200, 23, 173),
        0x70: (249, -100, 150, 14, 188),
        0x74: (251, -100, 200, 18, 182),
        0x78: (235, -100, 180, 18, 175),
        0x7C: (219, -100, 100, 13, 150),
        0x80: (206, 0, 100, 9, 155),
        0x84: (232, 0, 100, 8, 205),
        0x88: (255, 100, 100, 1, 255),
    }
    distribution_pass = all(
        (
            row["非零记录数"], row["最小值"], row["最大值"],
            row["不同值数量"], row["等于100记录数"],
        ) == expected_stats[int(row["RoleDefinition偏移"][3:], 16)]
        for row in fields
    )

    return {
        "Api字段分布": fields,
        "界面回写映射": writes,
        "运行时八通道重算映射": recompute,
        "关键边界": {
            "八个重算目标": "RoleDefinition+0x68..+0x84",
            "+0x88资源事实": "255/255均为100，且0x41F4A4八次helper不写+0x88",
            "研究版最终有效映射": (
                "+0x580/+0x588/+0x590/+0x598/+0x5A0/+0x5A8/+0x5B0/+0x5B8"
                "依次写+0x68/+0x6C/+0x70/+0x74/+0x78/+0x7C/+0x80/+0x84；"
                "+0x58C先写+0x70但立即被+0x590覆盖。"
            ),
            "禁止越界": (
                "上述证据强力支持八通道对齐并保留+0x88常量，但当前仍未取得作者正式字段名；"
                "不能只凭取值像百分比就把八项逐一命名。"
            ),
        },
        "直接调用者": [f"0x{x:08X}" for x in direct_callers(original, 0x00440C60)],
        "双版本差异符合九位点基线": all(
            original.read_va(va, len(bytes.fromhex(oh))).hex() == oh
            and research.read_va(va, len(bytes.fromhex(rh))).hex() == rh
            for _, va, oh, rh, _ in PATCH_SITES[3:]
        ),
        "PASS": (
            distribution_pass
            and direct_callers(original, 0x00440C60) == EXPECTED_CALLERS[0x00440C60]
            and fields[-1]["最小值"] == 100 and fields[-1]["最大值"] == 100
        ),
    }


def verify_display_and_bink(original: PEImage, research: PEImage) -> Dict[str, Any]:
    byte_rows = []
    for label, va, expected in DISPLAY_SITES:
        size = len(bytes.fromhex(expected))
        ob = original.read_va(va, size).hex()
        rb = research.read_va(va, size).hex()
        byte_rows.append({
            "名称": label, "VA": f"0x{va:08X}", "实际HEX": ob,
            "预期HEX": expected, "双版本一致": ob == rb,
            "PASS": ob == expected and ob == rb,
        })

    call_rows = []
    for target in (0x00401820, 0x00405B30, 0x004064E0, 0x00406230, 0x004062C0):
        actual = direct_callers(original, target)
        expected = EXPECTED_CALLERS[target]
        call_rows.append({
            "目标": f"0x{target:08X}",
            "实际直接调用者": [f"0x{x:08X}" for x in actual],
            "预期直接调用者": [f"0x{x:08X}" for x in expected],
            "PASS": actual == expected,
        })

    return {
        "关键机器码": byte_rows,
        "直接调用面": call_rows,
        "原版窗口与呈现路径": (
            "0x401820以WS_POPUP创建640x480窗口；0x405BD0读取renderer+0x30，"
            "真值进入全屏初始化，假值进入窗口初始化；0x4064E0再次读取同一标志，"
            "真值对renderer+4调用Flip，假值以renderer+8为源、renderer+4为目标调用Blt。"
        ),
        "原版Bink路径": (
            "0x401BA0给锁定/解锁helper都传0；两个helper在参数为0时选择renderer+4。"
            "BinkCopyToBuffer格式默认为9，renderer+0x3C==16时改10，步长取renderer+0x40。"
            "该逐帧函数没有调用0x4064E0；0x4064E0唯一直接调用者是0x405A2A。"
        ),
        "兼容候选边界": (
            "把电影目标切到renderer+8、改用另一pitch/格式并在解锁后调用窗口呈现，"
            "均不属于两份RPG.exe的原版行为；在动态DirectDraw/Bink验收前只能列为待验证兼容方案。"
        ),
        "PASS": all(row["PASS"] for row in byte_rows + call_rows),
    }


def verify_all_callers(original: PEImage) -> List[Dict[str, Any]]:
    rows = []
    for target, expected in EXPECTED_CALLERS.items():
        actual = direct_callers(original, target)
        rows.append({
            "目标": f"0x{target:08X}",
            "实际": [f"0x{x:08X}" for x in actual],
            "预期": [f"0x{x:08X}" for x in expected],
            "边界": "仅E8 rel32直接调用，不形式化排除函数指针",
            "PASS": actual == expected,
        })
    return rows


def build_comparison_summary() -> List[Dict[str, str]]:
    """当前包对候选主题的去重分类；不保存候选来源身份。"""

    return [
        {"主题": "PlayDisk与MultiMediaMov盘符定位", "分类": "新增独立闭合", "处理": "记录原版启动协议与研究版差异"},
        {"主题": "MiscInfo第六槽动作清理", "分类": "新增独立闭合", "处理": "记录0x180冥殺獄陣遗漏边界"},
        {"主题": "RoleDefinition八通道回写对齐", "分类": "新增独立闭合", "处理": "记录两版映射与作者命名边界"},
        {"主题": "MapSCI+0x380存档许可", "分类": "既有覆盖并复核", "处理": "沿用固化10至14结论，不重复建规格"},
        {"主题": "掉落与成长随机位点", "分类": "既有覆盖并复核", "处理": "沿用固化15至21结论"},
        {"主题": "经验与金钱战后结算位点", "分类": "既有覆盖并复核", "处理": "沿用D7奖励链，不复制候选实现"},
        {"主题": "原版窗口与DirectDraw呈现", "分类": "新增独立闭合", "处理": "记录全屏Flip/窗口Blt分流"},
        {"主题": "原版Bink默认表面路径", "分类": "新增独立闭合", "处理": "记录renderer+4锁解锁与无逐帧present"},
        {"主题": "窗口化表面格式与电影重定向", "分类": "待动态验收的兼容候选", "处理": "不写成游戏原始行为或已成功方案"},
    ]


def build_report(args: argparse.Namespace) -> Tuple[Dict[str, Any], List[Dict[str, Any]]]:
    original = PEImage(args.original_rpg)
    research = PEImage(args.research_rpg)
    input_rows, misc, firttech, api = verify_inputs(
        original, research, args.miscinfo, args.firttech, args.api
    )
    patch_rows = verify_patch_sites(original, research)
    existing_rows = verify_existing_sites(original, research)
    imports = verify_imports(original)
    slices = verify_slices(original, research)
    caller_rows = verify_all_callers(original)
    no_cd = verify_no_cd(original, research)
    misc_cleanup = verify_misc_cleanup(original, research, misc, firttech)
    role_channels = verify_role_channels(original, research, api)
    display = verify_display_and_bink(original, research)

    section_passes = {
        "输入身份": all(row["PASS"] for row in input_rows.values()),
        "导入表": all(row["PASS"] for row in imports),
        "关键区间": all(row["PASS"] for row in slices),
        "历史差异簇": all(row["PASS"] for row in patch_rows),
        "既有位点复核": all(row["PASS"] for row in existing_rows),
        "直接调用面": all(row["PASS"] for row in caller_rows),
        "盘符定位": no_cd["PASS"],
        "第六槽清理": misc_cleanup["PASS"],
        "八通道对齐": role_channels["PASS"],
        "显示与电影": display["PASS"],
    }

    report = {
        "研究节点": "v0.7D阶段中-固化68补充",
        "工具版本": TOOL_VERSION,
        "研究原则": (
            "只把原版EXE、历史研究版EXE和原始ENC资源可复现的事实写入当前包；"
            "候选来源身份不进入报告；既有主题只做去重复核。"
        ),
        "输入校验": input_rows,
        "导入表身份": imports,
        "关键范围哈希": slices,
        "三差异簇精确字节": patch_rows,
        "既有研究位点复核": existing_rows,
        "全部关键直接调用者": caller_rows,
        "PlayDisk与电影目录定位": no_cd,
        "MiscInfo第六槽动作清理": misc_cleanup,
        "RoleDefinition八通道回写对齐": role_channels,
        "窗口DirectDraw与Bink原版路径": display,
        "候选主题去重分类": build_comparison_summary(),
        "分项PASS": section_passes,
        "总体PASS": all(section_passes.values()),
        "后续边界": [
            "不把八通道强命名为作者正式字段；保留原始偏移。",
            "原版第五选择槽清理遗漏已静态闭合；兼容实现应覆盖槽0..5。",
            "窗口表面格式与Bink目标重定向仍需真实Windows/DirectDraw/Bink动态验收。",
            "本工具只穷举E8直接调用；函数指针与运行时改写不在直接调用负证据覆盖内。",
        ],
    }
    return report, patch_rows + existing_rows


def write_csv(path: Path, rows: Sequence[Dict[str, Any]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    columns = ["主题", "VA", "原版HEX", "研究版HEX", "原版预期HEX", "研究版预期HEX", "预期HEX", "分类", "PASS"]
    with path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=columns, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def objdump_range(objdump: str, exe: Path, start: int, end: int) -> str:
    command = [
        objdump, "-d", "-Mintel",
        f"--start-address=0x{start:X}", f"--stop-address=0x{end:X}", str(exe),
    ]
    completed = subprocess.run(command, check=True, capture_output=True, text=True)
    return completed.stdout


def write_evidence(args: argparse.Namespace) -> List[str]:
    if args.evidence_root is None:
        return []
    groups: Tuple[Tuple[str, Tuple[Tuple[str, Path, int, int], ...]], ...] = (
        (
            "01_版本基线与封包审计/固化68/反汇编/原版_盘符与MultiMediaMov定位_多区间.asm",
            (
                ("原版盘符定位", args.original_rpg, 0x00402A10, 0x00402AA0),
                ("原版GetFileAttributesA包装helper", args.original_rpg, 0x0045CCDE, 0x0045CD22),
                ("研究版盘符定位差异", args.research_rpg, 0x00402A10, 0x00402AA0),
            ),
        ),
        (
            "07_战斗核心对象与随机数/固化68/反汇编/原版与研究版_MiscInfo六槽动作加入清理与回写_多区间.asm",
            (
                ("原版MiscInfo装载", args.original_rpg, 0x00428400, 0x004284C0),
                ("原版动作池加入删除与全槽清理", args.original_rpg, 0x00423100, 0x00423598),
                ("研究版全槽清理上界", args.research_rpg, 0x00423550, 0x00423598),
                ("清理后角色数据回写", args.original_rpg, 0x00441F80, 0x00442034),
            ),
        ),
        (
            "07_战斗核心对象与随机数/固化68/反汇编/原版与研究版_RoleDefinition八通道回写差异_多区间.asm",
            (
                ("原版八通道运行时重算", args.original_rpg, 0x0041F4A4, 0x0041F570),
                ("原版界面到RoleDefinition九次回写", args.original_rpg, 0x00440C60, 0x00440F60),
                ("研究版界面到RoleDefinition回写", args.research_rpg, 0x00440C60, 0x00440F60),
            ),
        ),
        (
            "11_运行时输入时钟与显示/固化68/反汇编/原版_窗口创建DirectDraw全窗口分流与呈现_多区间.asm",
            (
                ("窗口创建", args.original_rpg, 0x00401820, 0x00401880),
                ("表面描述符", args.original_rpg, 0x00405B30, 0x00405BC8),
                ("全屏与窗口初始化分流", args.original_rpg, 0x00405BD0, 0x00405CA0),
                ("窗口初始化", args.original_rpg, 0x00405EA4, 0x004060A0),
                ("Flip与Blt呈现", args.original_rpg, 0x004064E0, 0x00406589),
            ),
        ),
        (
            "11_运行时输入时钟与显示/固化68/反汇编/原版_Bink默认表面锁定复制解锁边界_多区间.asm",
            (
                ("Bink逐帧复制", args.original_rpg, 0x00401BA0, 0x00401C49),
                ("表面锁定解锁helper", args.original_rpg, 0x00406230, 0x004062D9),
            ),
        ),
    )
    written: List[str] = []
    for relative, ranges in groups:
        path = args.evidence_root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        parts = [
            "; 固化68补充直接反汇编证据。由当前工具从已校验输入只读生成。",
            "; 业务结论仍以机器JSON和完整接档说明中的证据边界为准。",
            "",
        ]
        for label, exe, start, end in ranges:
            parts.extend((f"; ===== {label} 0x{start:08X}..0x{end:08X} =====", objdump_range(args.objdump, exe, start, end), ""))
        path.write_text("\n".join(parts), encoding="utf-8")
        written.append(str(path))
    return written


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="独立复核原版启动、战斗条目、角色通道与显示电影路径")
    parser.add_argument("--original-rpg", type=Path, required=True, help="完全未修改的RPG.exe")
    parser.add_argument("--research-rpg", type=Path, required=True, help="历史研究版RPG.exe")
    parser.add_argument("--miscinfo", type=Path, required=True, help="原始Public/MiscInfo.ENC")
    parser.add_argument("--firttech", type=Path, required=True, help="原始Public/Firttech.enc")
    parser.add_argument("--api", type=Path, required=True, help="原始Public/Api.enc")
    parser.add_argument("--json-out", type=Path, help="机器结果JSON输出")
    parser.add_argument("--csv-out", type=Path, help="位点明细CSV输出")
    parser.add_argument("--evidence-root", type=Path, help="可选：证据目录根；提供后生成五份ASM")
    parser.add_argument("--objdump", default="objdump", help="生成ASM时使用的objdump命令")
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    report, rows = build_report(args)
    evidence_files = write_evidence(args)
    if evidence_files:
        report["生成反汇编证据"] = evidence_files
    if args.json_out:
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    if args.csv_out:
        write_csv(args.csv_out, rows)
    print(json.dumps({
        "工具版本": TOOL_VERSION,
        "总体PASS": report["总体PASS"],
        "分项PASS": report["分项PASS"],
        "反汇编证据数": len(evidence_files),
    }, ensure_ascii=False, indent=2))
    return 0 if report["总体PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
