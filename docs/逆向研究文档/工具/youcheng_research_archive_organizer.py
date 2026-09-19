#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""《幽城幻剑录》研究资料主题化目录整理器。

本工具解决旧固化包里“数据、证据主要按固化号堆放，人类难以找到主题入口”的问题。
它只负责路径治理，不解释新的游戏语义，也不会改写任何被迁移文件的内容。

安全原则：

1. 默认仅预演；只有显式传入 ``--执行`` 才会移动文件。
2. 每个源文件先记录字节数和 SHA-256，移动后再次复核。
3. 目标路径发生碰撞时立即停止，不覆盖任何文件。
4. Markdown 说明文档不参与迁移，固定保留在“工具／数据／证据”根目录。
5. 已位于现行主题目录的文件自动跳过，因而可以在后续固化包中重复运行。
6. 旧的全文件索引、封包审计和失败记录归入“版本基线与封包审计”；它们的正文
   继续保持历史原样，不能冒充当前路径索引。

现行层级为：

    数据/<主题>/<固化节点或基础阶段>/<机器结果或表格明细>/<原文件名>
    证据/<主题>/<固化节点或基础阶段>/<证据类型>/<原文件名>

固化号仍作为第二层保留，用来恢复研究时间线；主题成为第一层，供人类浏览。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


TOOL_VERSION = "0.7D-固化66资料整理版1"


# 主题名以两位数字开头，保证 Windows 资源管理器按名称排序时阅读顺序稳定。
# “版本基线与封包审计”只收目录治理、继承、索引、失败记录等研究过程证据；
# 真正的格式、字段和机器行为结果必须进入其业务主题，不能因为文件名含“回归”就丢进审计区。
CATEGORIES: tuple[str, ...] = (
    "01_版本基线与封包审计",
    "02_公共数据加密与数据库",
    "03_事件与脚本系统",
    "04_场景实体界面与存档",
    "05_资源容器与配置",
    "06_图像动画与音频",
    "07_战斗核心对象与随机数",
    "08_战斗公式资源与物品",
    "09_战斗状态效果与场域",
    "10_战斗动作结果与表现",
    "11_运行时输入时钟与显示",
)

INDEX_CATEGORY = "00_目录索引与路径映射"
NODE_RE = re.compile(r"固化\d+(?:修正版\d+|整理版\d+)?")


# 固化节点已经在完整接档中确认了主研究方向。除少数跨主题节点外，直接使用节点主线
# 比仅看英文缩写更可靠。例如固化46的 ITF0052 是“状态表现资源”，应跟随战斗状态主题，
# 而不是因为文件名含 ITF 就被机械丢进通用图像主题。
NODE_DEFAULTS: dict[int, str] = {
    3: "07_战斗核心对象与随机数",
    4: "09_战斗状态效果与场域",
    5: "08_战斗公式资源与物品",
    6: "08_战斗公式资源与物品",
    7: "03_事件与脚本系统",
    8: "03_事件与脚本系统",
    9: "03_事件与脚本系统",
    10: "04_场景实体界面与存档",
    11: "04_场景实体界面与存档",
    12: "04_场景实体界面与存档",
    13: "04_场景实体界面与存档",
    14: "04_场景实体界面与存档",
    15: "10_战斗动作结果与表现",
    16: "10_战斗动作结果与表现",
    17: "10_战斗动作结果与表现",
    18: "07_战斗核心对象与随机数",
    19: "07_战斗核心对象与随机数",
    20: "07_战斗核心对象与随机数",
    21: "07_战斗核心对象与随机数",
    22: "04_场景实体界面与存档",
    23: "04_场景实体界面与存档",
    24: "04_场景实体界面与存档",
    25: "04_场景实体界面与存档",
    26: "05_资源容器与配置",
    27: "06_图像动画与音频",
    28: "06_图像动画与音频",
    29: "06_图像动画与音频",
    30: "06_图像动画与音频",
    31: "06_图像动画与音频",
    32: "06_图像动画与音频",
    33: "06_图像动画与音频",
    34: "06_图像动画与音频",
    35: "06_图像动画与音频",
    36: "06_图像动画与音频",
    37: "06_图像动画与音频",
    38: "11_运行时输入时钟与显示",
    39: "06_图像动画与音频",
    40: "09_战斗状态效果与场域",
    41: "11_运行时输入时钟与显示",
    42: "07_战斗核心对象与随机数",
    43: "10_战斗动作结果与表现",
    44: "09_战斗状态效果与场域",
    45: "09_战斗状态效果与场域",
    46: "09_战斗状态效果与场域",
    47: "09_战斗状态效果与场域",
    48: "09_战斗状态效果与场域",
    49: "09_战斗状态效果与场域",
    50: "09_战斗状态效果与场域",
    51: "08_战斗公式资源与物品",
    52: "08_战斗公式资源与物品",
    53: "08_战斗公式资源与物品",
    54: "08_战斗公式资源与物品",
    55: "10_战斗动作结果与表现",
    56: "10_战斗动作结果与表现",
    57: "10_战斗动作结果与表现",
    58: "09_战斗状态效果与场域",
    59: "09_战斗状态效果与场域",
    60: "09_战斗状态效果与场域",
    61: "10_战斗动作结果与表现",
    62: "07_战斗核心对象与随机数",
    63: "09_战斗状态效果与场域",
    64: "10_战斗动作结果与表现",
    65: "10_战斗动作结果与表现",
    66: "10_战斗动作结果与表现",
}


# 固化22～24横跨场景、显示、容器和动画，需要按文件名使用已经确认的对象族锚点分流。
# 基础阶段根目录文件也使用同一组规则。匹配仅决定“资料放哪”，不等于新增字段语义。
KEYWORD_RULES: tuple[tuple[str, tuple[str, ...]], ...] = (
    ("02_公共数据加密与数据库", ("ENC格式", "ENC字段", "ENC表结构", "ENC全样本")),
    ("05_资源容器与配置", ("DAT容器", "资源容器", "系统资源", "音乐列表", "MenusDir全量提取", "ItfDir容器")),
    ("06_图像动画与音频", ("SF2", "SAF", "ITF", "WAV", "PCM", "RIFF", "图层", "动画格式", "动画结构")),
    ("03_事件与脚本系统", ("EVE", "Opcode", "opcode", "事件", "世界动作", "code11", "code12", "EventID")),
    ("04_场景实体界面与存档", ("SCI", "Map", "Entity", "实体", "地图", "场景", "存档", "TSF", "Interface", "天書", "休整", "炼化", "CanonicalGameState")),
    ("11_运行时输入时钟与显示", ("DDDES", "显示定时", "WM_TIMER", "多媒体定时", "旧式逻辑时钟", "Legacy", "鼠标", "输入", "GetKeyState", "遇敌持续")),
    ("10_战斗动作结果与表现", ("动作类", "动作状态", "统一角色状态", "来源依赖", "结果分派", "结果模式", "战斗结果", "奖励", "招式名", "战斗地板", "BC0", "BC6")),
    ("09_战斗状态效果与场域", ("三十九槽", "39槽", "前二十八", "状态表现", "状态资源", "效果槽", "互反", "对向槽", "场域", "九槽", "持续场域", "selector15", "状态容器")),
    ("08_战斗公式资源与物品", ("公式", "威力", "防禦", "物品", "能力", "代价", "资源消耗", "Ail2")),
    ("07_战斗核心对象与随机数", ("RNG", "随机", "DamageInfo", "战斗对象", "目标选择", "人工智能", "AI", "脱逃", "战斗调查器")),
)


# 这些词组合明确表示研究过程、封包、继承或失败记录。单独出现“回归”不够，因为许多
# RPG 静态回归 JSON 本身就是格式/机器行为的主要数据，应留在业务主题。
AUDIT_PATTERNS: tuple[re.Pattern[str], ...] = tuple(
    re.compile(pattern, re.IGNORECASE)
    for pattern in (
        r"全文件.*索引",
        r"本版本全文件",
        r"源包全文件",
        r"固化包全文件索引",
        r"封包.*审计",
        r"全包.*审计",
        r"文档.*审计",
        r"完整性校验",
        r"阶段里程碑",
        r"最终只读确认",
        r"第一轮.*审计",
        r"第二轮.*审计",
        r"双轮独立原始输入重放审计",
        r"继承.*哈希",
        r"预审",
        r"FAIL",
        r"失败记录",
        r"纠错.*审计",
        r"整理映射",
        r"文档合并与原路径映射",
        r"精简删除与合并映射",
        r"补充线程证据来源",
        r"自动回归检查结果",
        r"本整理版样本结构复核",
        r"代码工具.*回归审计",
        r"工具注释.*等价回归",
        r"代码等价回归",
    )
)


# 八个早期文件名只有地址或非常宽的名称，无法靠字符串规则可靠分类。它们已经在历史
# 接档中闭合了对象归属，因此这里显式列出，不允许用“其他”目录掩盖人工判断。
EXPLICIT_PATH_CATEGORIES: dict[str, str] = {
    "阶段D/直接调用引用_0x4231F0_主资源零值状态转移.json": "09_战斗状态效果与场域",
    "阶段D/直接调用引用_0x42B0D0.json": "09_战斗状态效果与场域",
    "阶段D/直接调用引用_0x42C680.json": "08_战斗公式资源与物品",
    "阶段D/直接调用引用_FIRTTECH特殊动作_0x421A00.json": "10_战斗动作结果与表现",
    "阶段D/直接调用引用_Runtime前28槽全清_0x4231D0.json": "09_战斗状态效果与场域",
    "阶段D/战斗单位构造与角色定义复制_0041F140-0041F6CF.asm": "07_战斗核心对象与随机数",
    "阶段D/战斗模块关键字符串与源码路径.txt": "07_战斗核心对象与随机数",
    "阶段D/证据来源.json": "01_版本基线与封包审计",
}


@dataclass(frozen=True)
class MoveRecord:
    """一项只读源文件到主题目录的迁移计划。"""

    area: str
    source: Path
    source_rel: str
    target: Path
    target_rel: str
    category: str
    node: str
    evidence_type: str
    rule: str
    size: int
    sha256: str


def sha256_file(path: Path) -> str:
    """分块计算 SHA-256，避免一次把较大的二进制证据全部读入内存。"""

    digest = hashlib.sha256()
    with path.open("rb") as handle:
        while True:
            block = handle.read(1024 * 1024)
            if not block:
                break
            digest.update(block)
    return digest.hexdigest()


def extract_node(relative_path: Path) -> tuple[str, int | None]:
    """优先从目录层取得固化节点；根目录历史文件再从文件名提取。"""

    for part in relative_path.parts[:-1]:
        if NODE_RE.fullmatch(part):
            number = int(re.search(r"\d+", part).group(0))
            return part, number
    match = NODE_RE.search(relative_path.name)
    if match:
        label = match.group(0)
        number = int(re.search(r"\d+", label).group(0))
        return label, number
    return "基础阶段", None


def is_audit_path(relative_path: Path) -> bool:
    """识别历史封包、继承、失败与目录治理记录。"""

    text = relative_path.as_posix()
    if "历史回归输出/" in text or "历史恢复点资料/" in text:
        return True
    return any(pattern.search(text) for pattern in AUDIT_PATTERNS)


def category_for(relative_path: Path, node_number: int | None) -> tuple[str, str]:
    """按“审计优先、稳定节点主线、跨主题关键词、节点默认”的顺序分类。"""

    text = relative_path.as_posix()
    if text in EXPLICIT_PATH_CATEGORIES:
        return EXPLICIT_PATH_CATEGORIES[text], "历史接档已确认的显式对象归属"
    if is_audit_path(relative_path):
        return "01_版本基线与封包审计", "封包／继承／失败／目录治理规则"

    # 固化22～24和没有节点号的基础文件确实跨多个主题，允许关键词分流。
    # 其它节点优先服从已经固化的主研究方向，避免资源名掩盖上层业务用途。
    if node_number in {22, 23, 24} or node_number is None:
        for category, keywords in KEYWORD_RULES:
            matched = next((keyword for keyword in keywords if keyword in text), None)
            if matched is not None:
                return category, f"跨主题文件名锚点：{matched}"

    if node_number is not None and node_number in NODE_DEFAULTS:
        return NODE_DEFAULTS[node_number], f"固化{node_number}已确认主研究方向"

    # 少数早期全局文件没有固化号但仍可能被较宽的关键词捕获；这里再扫一次。
    for category, keywords in KEYWORD_RULES:
        matched = next((keyword for keyword in keywords if keyword in text), None)
        if matched is not None:
            return category, f"文件名对象族锚点：{matched}"

    # 只有真正无法从现有节点和名称判断的研究治理文件才进入审计区。
    # 工具会在目录报告中把这类“兜底”单独计数，便于人工发现并修正规则。
    return "01_版本基线与封包审计", "无业务主题锚点，保守归入治理区"


def type_for(area: str, path: Path) -> str:
    """为数据和证据提供一致、面向人的第三层类型名。"""

    suffix = path.suffix.lower()
    if area == "数据":
        return "表格明细" if suffix == ".csv" else "机器结果"
    return {
        ".asm": "反汇编",
        ".txt": "文本摘录",
        ".png": "图像证据",
        ".sf2": "原始资源样本",
        ".bin": "二进制片段",
        ".json": "机器结果",
        ".csv": "表格明细",
    }.get(suffix, "其他证据")


def is_already_organized(area_root: Path, path: Path) -> bool:
    """判断文件是否已经位于现行主题目录或目录索引区。"""

    relative = path.relative_to(area_root)
    if len(relative.parts) < 2:
        return False
    return relative.parts[0] in CATEGORIES or relative.parts[0] == INDEX_CATEGORY


def iter_legacy_files(root: Path) -> Iterable[tuple[str, Path]]:
    """枚举数据、证据中的旧布局文件；根目录 Markdown 说明固定跳过。"""

    for area in ("数据", "证据"):
        area_root = root / area
        if not area_root.is_dir():
            raise SystemExit(f"缺少目录：{area_root}")
        for path in sorted(area_root.rglob("*")):
            if not path.is_file() or path.suffix.lower() == ".md":
                continue
            if is_already_organized(area_root, path):
                continue
            yield area, path


def build_plan(root: Path) -> list[MoveRecord]:
    """建立完整迁移计划，并在任何写入前检查目标路径碰撞。"""

    records: list[MoveRecord] = []
    targets: dict[Path, str] = {}
    for area, source in iter_legacy_files(root):
        area_root = root / area
        relative = source.relative_to(area_root)
        node, node_number = extract_node(relative)
        category, rule = category_for(relative, node_number)
        evidence_type = type_for(area, source)
        target = area_root / category / node / evidence_type / source.name
        target_rel = target.relative_to(root).as_posix()
        source_rel = source.relative_to(root).as_posix()

        if target in targets:
            raise SystemExit(
                "目标路径碰撞，已停止且未写入：\n"
                f"  {targets[target]}\n  {source_rel}\n  -> {target_rel}"
            )
        if target.exists() and target != source:
            raise SystemExit(f"目标已存在，已停止且未覆盖：{target_rel}")
        targets[target] = source_rel
        records.append(
            MoveRecord(
                area=area,
                source=source,
                source_rel=source_rel,
                target=target,
                target_rel=target_rel,
                category=category,
                node=node,
                evidence_type=evidence_type,
                rule=rule,
                size=source.stat().st_size,
                sha256=sha256_file(source),
            )
        )
    return records


def remove_empty_legacy_directories(root: Path) -> None:
    """只用 rmdir 删除数据／证据中的空旧目录；非空目录绝不会被递归删除。"""

    for area in ("数据", "证据"):
        area_root = root / area
        directories = sorted(
            (path for path in area_root.rglob("*") if path.is_dir()),
            key=lambda path: len(path.parts),
            reverse=True,
        )
        for directory in directories:
            try:
                directory.rmdir()
            except OSError:
                # 非空目录或仍被使用的现行主题目录必须保留。
                pass


def record_to_dict(record: MoveRecord) -> dict[str, object]:
    """把内部迁移记录转换为可长期保存的简体中文 JSON 字段。"""

    return {
        "原路径": record.source_rel,
        "新路径": record.target_rel,
        "主题": record.category,
        "固化节点": record.node,
        "资料类型": record.evidence_type,
        "分类依据": record.rule,
        "字节数": record.size,
        "SHA-256": record.sha256,
    }


def build_catalog(records: list[MoveRecord]) -> dict[str, object]:
    """汇总每个主题、节点和资料类型，供说明文档与后续审计直接读取。"""

    areas: dict[str, dict[str, object]] = {}
    for area in ("数据", "证据"):
        subset = [record for record in records if record.area == area]
        category_counts = Counter(record.category for record in subset)
        type_counts = Counter(record.evidence_type for record in subset)
        node_counts = Counter(record.node for record in subset)
        fallback = sum(1 for record in subset if record.rule.startswith("无业务主题锚点"))
        fallback_paths = [
            record.source_rel
            for record in subset
            if record.rule.startswith("无业务主题锚点")
        ]
        areas[area] = {
            "迁移文件数": len(subset),
            "主题计数": dict(sorted(category_counts.items())),
            "资料类型计数": dict(sorted(type_counts.items())),
            "固化节点计数": dict(sorted(node_counts.items())),
            "保守兜底分类数": fallback,
            "保守兜底路径": fallback_paths,
        }
    return {
        "工具": Path(__file__).name,
        "工具版本": TOOL_VERSION,
        "目录规则": "主题优先／固化节点第二层／资料类型第三层／原文件名不变",
        "主题顺序": list(CATEGORIES),
        "迁移总数": len(records),
        "区域": areas,
        "PASS": all(areas[area]["保守兜底分类数"] == 0 for area in areas),
    }


def execute_plan(root: Path, records: list[MoveRecord]) -> None:
    """逐项移动并立即复核；任何一项失败都抛出异常，不静默继续。"""

    for record in records:
        record.target.parent.mkdir(parents=True, exist_ok=True)
        record.source.rename(record.target)
        if record.target.stat().st_size != record.size:
            raise SystemExit(f"移动后字节数不符：{record.target_rel}")
        if sha256_file(record.target) != record.sha256:
            raise SystemExit(f"移动后SHA-256不符：{record.target_rel}")
    remove_empty_legacy_directories(root)


def write_reports(root: Path, records: list[MoveRecord]) -> tuple[Path, Path, Path]:
    """写出旧路径映射、机器目录摘要和便于筛选的 CSV 总索引。"""

    report_dir = root / "数据" / INDEX_CATEGORY
    report_dir.mkdir(parents=True, exist_ok=True)
    mapping_path = report_dir / "固化66资料整理版1_旧路径到新路径映射.json"
    catalog_path = report_dir / "固化66资料整理版1_主题目录机器摘要.json"
    csv_path = report_dir / "固化66资料整理版1_资料分类总索引.csv"

    mapping_payload = {
        "整理节点": "固化66资料整理版1",
        "工具版本": TOOL_VERSION,
        "原则": "只移动路径，不改写文件内容；每项迁移前后字节数与SHA-256一致。",
        "迁移文件数": len(records),
        "映射": [record_to_dict(record) for record in records],
        "PASS": True,
    }
    mapping_path.write_text(
        json.dumps(mapping_payload, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )
    catalog_path.write_text(
        json.dumps(build_catalog(records), ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )
    with csv_path.open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(("区域", "主题", "固化节点", "资料类型", "原路径", "新路径", "字节数", "SHA-256", "分类依据"))
        for record in records:
            writer.writerow(
                (
                    record.area,
                    record.category,
                    record.node,
                    record.evidence_type,
                    record.source_rel,
                    record.target_rel,
                    record.size,
                    record.sha256,
                    record.rule,
                )
            )
    return mapping_path, catalog_path, csv_path


def parse_args() -> argparse.Namespace:
    """建立命令行；默认预演，避免误操作研究包。"""

    parser = argparse.ArgumentParser(
        description="把幽城研究包的数据／证据从固化号优先布局迁移为主题优先布局。"
    )
    parser.add_argument("根目录", type=Path, help="包含工具、数据、证据、文档的研究包根目录")
    parser.add_argument(
        "--执行",
        action="store_true",
        help="实际移动文件并生成映射；不传时只输出预演统计",
    )
    return parser.parse_args()


def main() -> int:
    """程序入口：先完整规划，再根据显式开关决定是否执行。"""

    args = parse_args()
    root = args.根目录.resolve()
    if not root.is_dir():
        raise SystemExit(f"根目录不存在：{root}")
    records = build_plan(root)
    catalog = build_catalog(records)
    print(json.dumps(catalog, ensure_ascii=False, indent=2))
    if not args.执行:
        print("预演完成：未移动任何文件。确认后加 --执行。")
        return 0
    if not catalog["PASS"]:
        raise SystemExit("存在保守兜底分类；请先补充规则，不能直接执行。")
    execute_plan(root, records)
    reports = write_reports(root, records)
    print(f"已移动并复核 {len(records)} 个文件。")
    for report in reports:
        print(report.relative_to(root).as_posix())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
