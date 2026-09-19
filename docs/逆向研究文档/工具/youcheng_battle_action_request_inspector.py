#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""固化68：闭合FightRole动作请求码0x67/0x68与统一状态tick同步链。

固化66在Battle tick中观察到0x67/0x68，固化67又收紧了统一状态保留值
生产边界。本工具进一步证明：

* Battle+0x10D0+i*0xDE4与FightRole+0xCEC是同一物理DWORD；
* 0x422120以0x67..0x74为闭区间跳表分派，并且只在下游成功后写+0xCEC；
* 当前18个直接CALL的请求参数都是可静态枚举的立即数；
* 0x68只有Battle tick内两个直接请求点，0x67共有七个直接请求点；
* BB9/BBA/BBE与0x67/0x68构成可重复请求、成功记账和状态切换链。

“动作请求码”“请求槽”是兼容层中性命名，不等于作者正式枚举名。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Sequence, Tuple

from youcheng_battle_role_state_alias_inspector import (
    EXPECTED_ORIGINAL_SHA256,
    EXPECTED_RESEARCH_SHA256,
    PeImage,
    is_disp32_at,
    sha256,
)
from youcheng_battle_role_state_surface_inspector import (
    scan_absolute_value,
    scan_rel32_calls,
)
from youcheng_battle_role_state_producer_inspector import (
    build_report as build_producer_report,
)


TOOL_VERSION = "0.7D-solid68.0"
DISPATCHER_VA = 0x00422120
REQUEST_FROM_FIGHT_ROLE = 0xCEC
REQUEST_FROM_BATTLE = 0x10D0
FIGHT_ROLE_FROM_BATTLE = 0x3E4
ROLE_STRIDE = 0xDE4


@dataclass(frozen=True)
class CallRoute:
    """一个0x422120直接CALL及其可到达的第一参数立即数。"""

    call_va: int
    request_codes: Tuple[int, ...]
    push_sites: Tuple[int, ...]
    note: str


CALL_ROUTES: Tuple[CallRoute, ...] = (
    CallRoute(0x0041F322, (0x67,), (0x0041F31E,), "FightRole建立后的默认请求"),
    CallRoute(0x0041FB85, (0x70,), (0x0041FB83,), "固定0x70请求"),
    CallRoute(0x0041FFC0, (0x71,), (0x0041FFBE,), "固定0x71请求"),
    CallRoute(0x0042011B, (0x67,), (0x00420117,), "角色复位链一"),
    CallRoute(0x004203B4, (0x67,), (0x004203A6,), "角色复位链二"),
    CallRoute(0x0042141D, (0x67,), (0x004213F5,), "结果应用链"),
    CallRoute(0x00423770, (0x72, 0x6C, 0x6D), (0x00423752, 0x00423766, 0x0042376E),
              "BC6/BBE/其它分支共享CALL"),
    CallRoute(0x00442AC5, (0x68,), (0x00442ABD,), "7D4且当前请求67时转请求68"),
    CallRoute(0x00442AF0, (0x68,), (0x00442AE8,), "BBA保持/重试68"),
    CallRoute(0x00442B1C, (0x67,), (0x00442B14,), "BB9保持/重试67"),
    CallRoute(0x00442B3F, (0x67,), (0x00442B37,), "BBE收尾请求67"),
    CallRoute(0x0044337B, (0x6A,), (0x00443377,), "class普通路由"),
    CallRoute(0x004433F3, (0x6A,), (0x004433EF,), "class一二四路由"),
    CallRoute(0x004434A8, (0x69,), (0x00443487,), "固定0x69请求"),
    CallRoute(0x00443535, (0x6E,), (0x00443533,), "固定0x6E请求"),
    CallRoute(0x004436F1, (0x73,), (0x004436E7,), "固定0x73请求"),
    CallRoute(0x00443894, (0x74,), (0x00443892,), "固定0x74请求"),
    CallRoute(0x00443F4D, (0x67,), (0x00443F49,), "新建/恢复角色默认请求"),
)

EXPECTED_REQUEST_DISP_HITS = (
    0x00422198, 0x00422219, 0x0042226E, 0x004222EA, 0x0042238D,
    0x004223EE, 0x00422439, 0x00422482, 0x004224CB,
)
EXPECTED_BATTLE_ALIAS_HITS = (0x00442A8F,)

EXPECTED_JUMP_TABLE: Tuple[int, ...] = (
    0x00422160,  # 0x67
    0x004221A5,  # 0x68
    0x00422226,  # 0x69
    0x0042227B,  # 0x6A
    0x004222EE,  # 0x6B
    0x004222F7,  # 0x6C
    0x0042231C,  # 0x6D
    0x0042239A,  # 0x6E
    0x004222EE,  # 0x6F
    0x004223B0,  # 0x70
    0x004223FB,  # 0x71
    0x00422342,  # 0x72
    0x00422446,  # 0x73
    0x0042248F,  # 0x74
)

CRITICAL_SLICES: Tuple[Tuple[int, int, str, str], ...] = (
    (0x00422120, 0x00422529, "动作请求分派器和跳表",
     "891549c9444b8a764af726f35a8ff6817af21448b1fa95fb2d66008a1369ba51"),
    (0x00422147, 0x004221A5, "范围门和0x67路由",
     "ad2836504c37eed248e25e918d6ac52a0e8334557a284ce7d0e2961bb648bb1a"),
    (0x004221A5, 0x00422226, "0x68路由",
     "7d1326414764529f1be699ecf9d23694ee40c9cbe2d6fcdfd16b7c1be7daaac9"),
    (0x004224D8, 0x00422510, "0x67至0x74跳表",
     "ab07fb2ab06ed2a3291841e2efff43a1c46f622f879312f86d3f98a3492ce7ed"),
    (0x00442A80, 0x00442B58, "Battle tick请求同步链",
     "3f90fee5899bd2deeb923fbfb76ff131d2f021fdb30aa4003d231571ccdeebbc"),
    (0x0041F2F6, 0x0041F32D, "建立链0x67请求",
     "406b01f1ea3ca672d260cba98474914249833353e730b686a199e2916ed071d0"),
    (0x004200FF, 0x00420129, "复位链一0x67请求",
     "30d7987b43989bc970c805285ac32ac149d9b7241ca89872a75033239dbb4585"),
    (0x0042039A, 0x004203C2, "复位链二0x67请求",
     "40a17d9a7de36ca0c9f436aac4293e5392acb6028637121d45bca3305704046e"),
    (0x004213E1, 0x00421422, "结果应用链0x67请求",
     "e8310dd8a21c143a8965134ead07da12014a21257f6f63a32f5d12d18f6099fc"),
    (0x00443F30, 0x00443F52, "新建恢复链0x67请求",
     "55a0d783074b587e2afb5c2589910e0c82e430a712b3ac0f966b80b084864304"),
)


def verify_slices(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []
    for start, end, name, expected in CRITICAL_SLICES:
        ob = original.read_va(start, end - start)
        rb = research.read_va(start, end - start)
        actual = hashlib.sha256(ob).hexdigest()
        rows.append({
            "名称": name,
            "范围": f"0x{start:08X}..0x{end:08X}",
            "长度": end - start,
            "原版SHA-256": actual,
            "预期SHA-256": expected,
            "双版本逐字节一致": ob == rb,
            "PASS": actual == expected and ob == rb,
        })
    return rows


def verify_call_routes(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    """确认18个直接CALL及其第一参数push imm8。"""

    rows: List[Dict[str, object]] = []
    for route in CALL_ROUTES:
        push_rows = []
        for code, site in zip(route.request_codes, route.push_sites):
            expected = bytes((0x6A, code))
            ob = original.read_va(site, 2)
            rb = research.read_va(site, 2)
            push_rows.append({
                "请求码": f"0x{code:02X}",
                "PUSH指令VA": f"0x{site:08X}",
                "实际HEX": ob.hex(),
                "预期HEX": expected.hex(),
                "PASS": ob == expected and ob == rb,
            })
        # 每个CALL本身也必须真实解析到0x422120。
        call = original.read_va(route.call_va, 5)
        relative = int.from_bytes(call[1:5], "little", signed=True) if call[:1] == b"\xE8" else 0
        target = route.call_va + 5 + relative
        call_pass = (
            call[:1] == b"\xE8"
            and target == DISPATCHER_VA
            and call == research.read_va(route.call_va, 5)
        )
        rows.append({
            "CALL指令VA": f"0x{route.call_va:08X}",
            "可达请求码": [f"0x{x:02X}" for x in route.request_codes],
            "请求PUSH": push_rows,
            "说明": route.note,
            "CALL目标": f"0x{target:08X}",
            "PASS": call_pass and all(row["PASS"] for row in push_rows),
        })
    return rows


def verify_request_field_surface(
    original: PeImage, research: PeImage
) -> Dict[str, object]:
    """分离FightRole+CEC真实写与Battle+10D0数组首指针别名。"""

    cec_o = original.scan_text(struct.pack("<I", REQUEST_FROM_FIGHT_ROLE))
    cec_r = research.scan_text(struct.pack("<I", REQUEST_FROM_FIGHT_ROLE))
    cec_hits = tuple(va for va, _ in cec_o)
    cec_valid = tuple(va for va, off in cec_o if is_disp32_at(original, off))
    cec_writes = []
    write_rows = []
    for disp_va, _ in cec_o:
        start = disp_va - 2
        ob = original.read_va(start, 6)
        rb = research.read_va(start, 6)
        is_write = ob == bytes.fromhex("89aeec0c0000")
        if is_write:
            cec_writes.append(disp_va)
        write_rows.append({
            "位移字节VA": f"0x{disp_va:08X}",
            "指令VA": f"0x{start:08X}",
            "实际HEX": ob.hex(),
            "分类": "MOV [ESI+0xCEC], EBP" if is_write else "非预期",
            "双版本逐字节一致": ob == rb,
            "PASS": is_write and ob == rb,
        })

    ten_o = original.scan_text(struct.pack("<I", REQUEST_FROM_BATTLE))
    ten_r = research.scan_text(struct.pack("<I", REQUEST_FROM_BATTLE))
    ten_hits = tuple(va for va, _ in ten_o)
    alias_bytes_o = original.read_va(0x00442A8D, 6)
    alias_bytes_r = research.read_va(0x00442A8D, 6)
    alias_pass = (
        ten_hits == EXPECTED_BATTLE_ALIAS_HITS
        and ten_o == ten_r
        and alias_bytes_o.hex() == "8db5d0100000"
        and alias_bytes_o == alias_bytes_r
    )
    return {
        "FightRole+0xCEC": {
            "原始四字节命中": [f"0x{x:08X}" for x in cec_hits],
            "预期命中": [f"0x{x:08X}" for x in EXPECTED_REQUEST_DISP_HITS],
            "真实disp32命中": [f"0x{x:08X}" for x in cec_valid],
            "写入分区": write_rows,
            "全部是成功路径写EBP请求码": tuple(cec_writes) == EXPECTED_REQUEST_DISP_HITS,
            "两版命中集合一致": cec_o == cec_r,
            "PASS": (
                cec_hits == EXPECTED_REQUEST_DISP_HITS
                and cec_valid == EXPECTED_REQUEST_DISP_HITS
                and tuple(cec_writes) == EXPECTED_REQUEST_DISP_HITS
                and cec_o == cec_r
                and all(row["PASS"] for row in write_rows)
            ),
        },
        "Battle+0x10D0": {
            "四字节命中": [f"0x{x:08X}" for x in ten_hits],
            "预期": [f"0x{x:08X}" for x in EXPECTED_BATTLE_ALIAS_HITS],
            "唯一指令": "0x442A8D LEA ESI,[EBP+0x10D0]",
            "局部读取": ["0x442AB4", "0x442AD2", "0x442AFE"],
            "PASS": alias_pass,
        },
        "地址恒等式": {
            "Battle角色首项": f"+0x{FIGHT_ROLE_FROM_BATTLE:X}",
            "请求槽Battle口径": f"+0x{REQUEST_FROM_BATTLE:X}",
            "请求槽FightRole口径": f"+0x{REQUEST_FROM_FIGHT_ROLE:X}",
            "算术": (
                f"0x{REQUEST_FROM_BATTLE:X}-0x{REQUEST_FROM_FIGHT_ROLE:X}"
                f"=0x{REQUEST_FROM_BATTLE - REQUEST_FROM_FIGHT_ROLE:X}"
            ),
            "十六角色步长": f"0x{ROLE_STRIDE:X}",
            "PASS": (
                REQUEST_FROM_BATTLE - REQUEST_FROM_FIGHT_ROLE
                == FIGHT_ROLE_FROM_BATTLE
            ),
        },
    }


def verify_jump_table(original: PeImage, research: PeImage) -> Dict[str, object]:
    gate_o = original.read_va(0x0042214D, 19)
    gate_r = research.read_va(0x0042214D, 19)
    table_o = struct.unpack("<14I", original.read_va(0x004224D8, 14 * 4))
    table_r = struct.unpack("<14I", research.read_va(0x004224D8, 14 * 4))
    gate_expected = bytes.fromhex("8d4d9983f90d0f8795010000ff248dd8244200")
    rows = [
        {"请求码": f"0x{0x67 + index:02X}", "目标VA": f"0x{target:08X}"}
        for index, target in enumerate(table_o)
    ]
    return {
        "范围": "0x67..0x74（EBP-0x67后与0x0D比较）",
        "范围门实际HEX": gate_o.hex(),
        "范围门预期HEX": gate_expected.hex(),
        "跳表": rows,
        "预期跳表": [f"0x{x:08X}" for x in EXPECTED_JUMP_TABLE],
        "两版逐字节一致": gate_o == gate_r and table_o == table_r,
        "PASS": (
            gate_o == gate_expected
            and gate_o == gate_r
            and table_o == EXPECTED_JUMP_TABLE
            and table_o == table_r
        ),
    }


def verify_tick_rules(original: PeImage, research: PeImage) -> List[Dict[str, object]]:
    rules = (
        (0x00442AAB, 0x00442ACD,
         "相邻0x7D4且请求槽为0x67时调用0x68，随后统一状态写0xBBA",
         "817ef4d40700007519833e6775146a006a006a688d8e14f3ffffe856f6fdff897ef8"),
        (0x00442ACD, 0x00442AF5,
         "BBA且请求槽不是0x68、子对象空闲时再次请求0x68",
         "397ef87523833e68741e8b86b8feffff8a481484c975116a006a006a688d8e14f3ffffe82bf6fdff"),
        (0x00442AF5, 0x00442B21,
         "BB9且请求槽不是0x67、子对象空闲时再次请求0x67",
         "817ef8b90b00007523833e67741e8b8eb8feffff8a411484c075116a006a006a678d8e14f3ffffe8fff5fdff"),
        (0x00442B21, 0x00442B4B,
         "BBE且相邻0x7D1时请求0x67，随后统一状态写回BB9",
         "817ef8be0b00007521817ef4d107000075186a006a006a678d8e14f3ffffe8dcf5fdffc746f8b90b0000"),
    )
    rows: List[Dict[str, object]] = []
    for start, end, name, expected in rules:
        ob = original.read_va(start, end - start)
        rb = research.read_va(start, end - start)
        rows.append({
            "规则": name,
            "范围": f"0x{start:08X}..0x{end:08X}",
            "实际HEX": ob.hex(),
            "预期HEX": expected,
            "双版本逐字节一致": ob == rb,
            "PASS": ob.hex() == expected and ob == rb,
        })
    return rows


def build_report(original: PeImage, research: PeImage) -> Dict[str, object]:
    original_hash = sha256(original.data)
    research_hash = sha256(research.data)
    inherited = build_producer_report(original, research)
    slices = verify_slices(original, research)
    call_rows = verify_call_routes(original, research)
    field = verify_request_field_surface(original, research)
    jump_table = verify_jump_table(original, research)
    tick = verify_tick_rules(original, research)
    callers_o = scan_rel32_calls(original, DISPATCHER_VA)
    callers_r = scan_rel32_calls(research, DISPATCHER_VA)
    expected_callers = [route.call_va for route in CALL_ROUTES]
    direct_call_pass = callers_o == expected_callers and callers_o == callers_r
    absolute_o = scan_absolute_value(original, DISPATCHER_VA)
    absolute_r = scan_absolute_value(research, DISPATCHER_VA)
    absolute_pass = absolute_o == [] and absolute_r == []
    code67_calls = [
        route.call_va for route in CALL_ROUTES if 0x67 in route.request_codes
    ]
    code68_calls = [
        route.call_va for route in CALL_ROUTES if 0x68 in route.request_codes
    ]
    overall = (
        original_hash == EXPECTED_ORIGINAL_SHA256
        and research_hash == EXPECTED_RESEARCH_SHA256
        and bool(inherited["总体PASS"])
        and all(row["PASS"] for row in slices)
        and all(row["PASS"] for row in call_rows)
        and all(section["PASS"] for section in field.values())
        and jump_table["PASS"]
        and all(row["PASS"] for row in tick)
        and direct_call_pass
        and absolute_pass
    )
    return {
        "工具": Path(__file__).name,
        "工具版本": TOOL_VERSION,
        "固化节点": "v0.7D阶段中-固化68",
        "目标": (
            "闭合FightRole+0xCEC/Battle+0x10D0物理别名、0x422120请求分派器"
            "完整直接调用面，以及0x67/0x68与统一状态tick同步链。"
        ),
        "输入": {
            "完全未修改原版RPG.exe": {
                "路径": str(original.path), "SHA-256": original_hash,
                "PASS": original_hash == EXPECTED_ORIGINAL_SHA256,
            },
            "历史研究版RPG.exe": {
                "路径": str(research.path), "SHA-256": research_hash,
                "PASS": research_hash == EXPECTED_RESEARCH_SHA256,
            },
        },
        "固化67继承回归PASS": bool(inherited["总体PASS"]),
        "请求槽访问面与物理别名": field,
        "0x422120请求范围与跳表": jump_table,
        "0x422120直接调用面": {
            "实际CALL": [f"0x{x:08X}" for x in callers_o],
            "预期CALL": [f"0x{x:08X}" for x in expected_callers],
            "数量": len(callers_o),
            "两版一致": callers_o == callers_r,
            "完整文件中绝对函数指针命中": [f"0x{x:08X}" for x in absolute_o],
            "PASS": direct_call_pass and absolute_pass,
        },
        "逐直接CALL请求码": call_rows,
        "0x67_0x68直接请求集合": {
            "0x67直接CALL": [f"0x{x:08X}" for x in code67_calls],
            "0x67直接CALL数": len(code67_calls),
            "0x68直接CALL": [f"0x{x:08X}" for x in code68_calls],
            "0x68直接CALL数": len(code68_calls),
            "结论": "0x68的两个直接请求均位于0x442A80 tick；0x67另有建立、复位、结果应用及新建/恢复入口。",
            "PASS": len(code67_calls) == 7 and len(code68_calls) == 2,
        },
        "Battle_tick同步规则": tick,
        "关键区间": slices,
        "闭合结论": {
            "字段": (
                "Battle+0x10D0+i*0xDE4 == FightRole+0xCEC；tick用本地[ESI]"
                "读取，0x422120在九条成功出口统一写[ESI+0xCEC]=请求参数。"
            ),
            "分派": (
                "0x422120只接受0x67..0x74并经14项跳表分派；超范围直接失败。"
                "0x6B/0x6F在当前跳表直接落到失败出口。"
            ),
            "0x67": (
                "路由使用FightRole+0xAD4来源，完成0x41F700及下游对象调用后才把"
                "+0xCEC写为0x67。"
            ),
            "0x68": (
                "路由优先检查FightRole+0x848对象的+0x9C来源，必要时回退到"
                "FightRole+0xB2E；成功后才把+0xCEC写为0x68。"
            ),
            "同步状态机": (
                "7D4+请求67触发请求68并写BBA；BBA重试68，BB9重试67；"
                "BBE+7D1请求67并写回BB9。首次切状态的两处写入不检查分派返回值，"
                "后续tick通过请求槽和子对象空闲条件重试。"
            ),
        },
        "范围限制": [
            "0x67/0x68的作者正式枚举名仍UNKNOWN；不得直接命名为idle/run/attack等。",
            "当前直接CALL与文件内绝对函数指针表已穷举，但一般性计算函数指针或其它模块调用仍未绝对排除。",
            "静态成功路径不等于每个运行时请求都成功；0x422120失败时不会更新+0xCEC。",
            "下游+0xAD4、+0xB2E及[+0x848]+0x9C的资源/动作作者正式字段名仍需动态或符号证据。",
        ],
        "总体PASS": overall,
    }


def write_csv(path: Path, report: Dict[str, object]) -> None:
    """输出18个直接CALL与请求码明细。"""

    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(["CALL指令VA", "可达请求码", "请求PUSH指令", "说明", "PASS"])
        for row in report["逐直接CALL请求码"]:
            writer.writerow([
                row["CALL指令VA"],
                "/".join(row["可达请求码"]),
                "/".join(push["PUSH指令VA"] for push in row["请求PUSH"]),
                row["说明"],
                "PASS" if row["PASS"] else "FAIL",
            ])


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="审计FightRole动作请求码0x67/0x68与统一状态tick同步链。"
    )
    parser.add_argument("--original-rpg", required=True, type=Path, help="完全未修改原版RPG.exe")
    parser.add_argument("--research-rpg", required=True, type=Path, help="历史研究版RPG.exe")
    parser.add_argument("--json-out", required=True, type=Path, help="输出机器审计JSON")
    parser.add_argument("--csv-out", required=True, type=Path, help="输出直接调用明细CSV")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    original = PeImage.load(args.original_rpg)
    research = PeImage.load(args.research_rpg)
    report = build_report(original, research)
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.json_out.write_text(
        json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    write_csv(args.csv_out, report)
    print(f"[固化68] 总体结果：{'PASS' if report['总体PASS'] else 'FAIL'}")
    print(f"[固化68] JSON：{args.json_out}")
    print(f"[固化68] CSV ：{args.csv_out}")
    return 0 if report["总体PASS"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
