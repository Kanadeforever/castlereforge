#!/usr/bin/env python3
# 执行这一条实际代码步骤：`"""整理固化包中的数据与证据目录，并为所有合并项生成可审计映射。"""`；它与上下相邻语句共同完成当前函数的小任务。
"""整理固化包中的数据与证据目录，并为所有合并项生成可审计映射。"""

# 启用较新的 Python 注解语法行为，让类型注解先按文字保存，避免旧版本在定义阶段立即求值。
from __future__ import annotations

# 导入 `argparse`。它会统一解析命令行参数，并自动处理 `-h/--help`。
# 旧版脚本直接读取 `sys.argv[1]`，因此把 `--help` 错当成了目录名并真的创建文件；
# 改用 argparse 后，用户请求帮助时会在任何文件操作开始前安全退出。
import argparse
# 导入 `hashlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import hashlib
# 导入 `json` 模块；这里只取得标准库功能，不会修改游戏文件。
import json
# 导入 `re` 模块；这里只取得标准库功能，不会修改游戏文件。
import re
# 导入 `shutil` 模块；这里只取得标准库功能，不会修改游戏文件。
import shutil
# 从 `collections` 模块导入 `defaultdict`，后面的代码会直接使用这些现成组件。
from collections import defaultdict
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path


# 定义函数 `digest`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def digest(path: Path) -> str:
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


# 定义函数 `rel`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def rel(path: Path, root: Path) -> str:
    # 把 `path.relative_to(root).as_posix()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return path.relative_to(root).as_posix()


# 定义函数 `score`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def score(path: Path, root: Path) -> tuple[int, int, str]:
    # 把右侧 `rel(path, root)` 计算得到的值保存到 `name`，后面的判断或输出会继续使用它。
    name = rel(path, root)
    # 把右侧 `"历史回归输出" in name or "历史固化" in name` 计算得到的值保存到 `historical`，后面的判断或输出会继续使用它。
    historical = "历史回归输出" in name or "历史固化" in name
    # 把右侧 `name.startswith("数据/") and not historical` 计算得到的值保存到 `current`，后面的判断或输出会继续使用它。
    current = name.startswith("数据/") and not historical
    # 把 `(0 if current else 1 if name.startswith("证据/") and not historical else 2,` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return (0 if current else 1 if name.startswith("证据/") and not historical else 2,
            # 调用 `len` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            len(name), name)


# 定义命令行解析器。把这一小段独立成函数，既方便 `main` 使用，也方便未来测试参数而不触碰文件。
def build_parser() -> argparse.ArgumentParser:
    # description 会显示在 `--help` 顶部，让单独拿到脚本的人也能先看懂它会做什么。
    parser = argparse.ArgumentParser(
        description="整理固化包的数据/证据目录，并只合并 SHA-256 完全相同的重复实体。"
    )
    # 根目录是唯一位置参数。`type=Path` 会把输入文字转换为 Path 对象，省去手工读取 sys.argv。
    parser.add_argument(
        "根目录",
        type=Path,
        help="待整理的固化包根目录；脚本会修改其中的数据、证据与引用文档",
    )
    # 返回配置完成的解析器。请求 `--help` 时 argparse 会在 main 继续之前自动打印并退出。
    return parser


# 定义函数 `main`。`argv=None` 表示正常使用命令行；测试时也可以传入一组独立参数。
def main(argv: list[str] | None = None) -> None:
    # 先解析参数。这里是任何 mkdir、move、unlink 之前的第一步，所以 `--help` 保证没有写入副作用。
    args = build_parser().parse_args(argv)
    # resolve 会得到绝对路径，后面的相对路径记录因此有唯一、稳定的计算基准。
    root = args.根目录.resolve()
    # 必须确认目标已经是一个真实目录；否则立即失败，绝不因为拼错路径而创建一棵新固化包。
    if not root.is_dir():
        raise SystemExit(f"固化包根目录不存在或不是目录：{root}")
    # 把右侧 `root / "数据" / "阶段D"` 计算得到的值保存到 `data_stage`，后面的判断或输出会继续使用它。
    data_stage = root / "数据" / "阶段D"
    # 把右侧 `root / "证据" / "阶段D" / "历史回归输出"` 计算得到的值保存到 `archive`，后面的判断或输出会继续使用它。
    archive = root / "证据" / "阶段D" / "历史回归输出"
    # 执行一次赋值：把 `True, exist_ok=True)` 的结果放入左侧 `archive.mkdir(parents` 对应的变量/字段。
    archive.mkdir(parents=True, exist_ok=True)

    # 创建变量 `moves`（类型提示为 `list[dict[str, str]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    moves: list[dict[str, str]] = []
    # 开始循环 `source in sorted(data_stage.glob("历史固化*原始输出"))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for source in sorted(data_stage.glob("历史固化*原始输出")):
        # 把右侧 `re.fullmatch(r"历史固化(.+)原始输出", source.name).group(1)` 计算得到的值保存到 `number`，后面的判断或输出会继续使用它。
        number = re.fullmatch(r"历史固化(.+)原始输出", source.name).group(1)
        # 把右侧 `archive / f"固化{number}"` 计算得到的值保存到 `target`，后面的判断或输出会继续使用它。
        target = archive / f"固化{number}"
        # 检查条件 `target.exists()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if target.exists():
            # 检测到不能继续的情况，主动抛出异常 `RuntimeError(f"目标已存在：{target}")`，让上层明确知道数据或参数不符合要求。
            raise RuntimeError(f"目标已存在：{target}")
        # 把右侧 `rel(source, root)` 计算得到的值保存到 `old`，后面的判断或输出会继续使用它。
        old = rel(source, root)
        # 调用 `shutil.move` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        shutil.move(str(source), str(target))
        # 把这一条新结果追加到列表 `moves`；不会覆盖前面已经收集的记录。
        moves.append({"原路径": old, "新路径": rel(target, root), "原因": "历史原始输出归入证据归档"})

    # 把右侧 `data_stage / "历史恢复点资料"` 计算得到的值保存到 `recovery`，后面的判断或输出会继续使用它。
    recovery = data_stage / "历史恢复点资料"
    # 检查条件 `recovery.exists()`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if recovery.exists():
        # 把右侧 `root / "证据" / "阶段D" / "历史恢复点资料"` 计算得到的值保存到 `target`，后面的判断或输出会继续使用它。
        target = root / "证据" / "阶段D" / "历史恢复点资料"
        # 把右侧 `rel(recovery, root)` 计算得到的值保存到 `old`，后面的判断或输出会继续使用它。
        old = rel(recovery, root)
        # 调用 `shutil.move` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        shutil.move(str(recovery), str(target))
        # 把这一条新结果追加到列表 `moves`；不会覆盖前面已经收集的记录。
        moves.append({"原路径": old, "新路径": rel(target, root), "原因": "恢复点材料属于历史证据"})

    # 把右侧 `[p for base in (root / "数据", root / "证据")` 计算得到的值保存到 `candidates`，后面的判断或输出会继续使用它。
    candidates = [p for base in (root / "数据", root / "证据")
                  # 开始循环 `p in base.rglob("*") if p.is_file()]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
                  for p in base.rglob("*") if p.is_file()]
    # 创建变量 `groups`（类型提示为 `dict[str, list[Path]]`），并把 `defaultdict(list)` 的结果保存进去供后续步骤使用。
    groups: dict[str, list[Path]] = defaultdict(list)
    # 开始循环 `path in candidates`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for path in candidates:
        # 执行这一条实际代码步骤：`groups[digest(path)].append(path)`；它与上下相邻语句共同完成当前函数的小任务。
        groups[digest(path)].append(path)

    # 创建变量 `aliases`（类型提示为 `list[dict[str, object]]`），并把 `[]` 的结果保存进去供后续步骤使用。
    aliases: list[dict[str, object]] = []
    # 创建变量 `replacements`（类型提示为 `dict[str, str]`），并把 `{}` 的结果保存进去供后续步骤使用。
    replacements: dict[str, str] = {}
    # 开始循环 `sha, paths in sorted(groups.items())`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for sha, paths in sorted(groups.items()):
        # 检查条件 `len(paths) < 2`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if len(paths) < 2:
            # 跳过本轮循环剩余步骤，直接开始处理下一个元素。
            continue
        # 执行一次赋值：把 `lambda p: score(p, root))` 的结果放入左侧 `paths.sort(key` 对应的变量/字段。
        paths.sort(key=lambda p: score(p, root))
        # 把右侧 `paths[0]` 计算得到的值保存到 `keep`，后面的判断或输出会继续使用它。
        keep = paths[0]
        # 把右侧 `paths[1:]` 计算得到的值保存到 `removed`，后面的判断或输出会继续使用它。
        removed = paths[1:]
        # 把这一条新结果追加到列表 `aliases`；不会覆盖前面已经收集的记录。
        aliases.append({
            # 给当前结果字典的“SHA256”字段填写 `sha,`，这样导出的 JSON/表格能保留这一项证据。
            "SHA256": sha,
            # 给当前结果字典的“保留路径”字段填写 `rel(keep, root),`，这样导出的 JSON/表格能保留这一项证据。
            "保留路径": rel(keep, root),
            # 给当前结果字典的“合并原路径”字段填写 `[rel(p, root) for p in removed],`，这样导出的 JSON/表格能保留这一项证据。
            "合并原路径": [rel(p, root) for p in removed],
            # 给当前结果字典的“确认依据”字段填写 `"逐字节SHA-256一致；内容可由保留文件完整替代",`，这样导出的 JSON/表格能保留这一项证据。
            "确认依据": "逐字节SHA-256一致；内容可由保留文件完整替代",
        })
        # 开始循环 `path in removed`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for path in removed:
            # 执行一次赋值：把 `rel(keep, root)` 的结果放入左侧 `replacements[rel(path, root)]` 对应的变量/字段。
            replacements[rel(path, root)] = rel(keep, root)
            # 调用 `path.unlink` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            path.unlink()

    # 文档中若直接写有旧路径，替换为现行路径；目录移动使用前缀替换。
    # 把右侧 `sorted(((x["原路径"], x["新路径"]) for x in moves),` 计算得到的值保存到 `prefix_moves`，后面的判断或输出会继续使用它。
    prefix_moves = sorted(((x["原路径"], x["新路径"]) for x in moves),
                          # 把右侧 `lambda x: -len(x[0]))` 计算得到的值保存到 `key`，后面的判断或输出会继续使用它。
                          key=lambda x: -len(x[0]))
    # 开始循环 `doc in root.rglob("*.md")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for doc in root.rglob("*.md"):
        # 把右侧 `doc.read_text(encoding="utf-8-sig")` 计算得到的值保存到 `text`，后面的判断或输出会继续使用它。
        text = doc.read_text(encoding="utf-8-sig")
        # 把右侧 `text` 计算得到的值保存到 `updated`，后面的判断或输出会继续使用它。
        updated = text
        # 开始循环 `old, new in prefix_moves`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for old, new in prefix_moves:
            # 把右侧 `updated.replace(old, new)` 计算得到的值保存到 `updated`，后面的判断或输出会继续使用它。
            updated = updated.replace(old, new)
        # 开始循环 `old, new in sorted(replacements.items(), key=lambda x: -len(x[0]))`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for old, new in sorted(replacements.items(), key=lambda x: -len(x[0])):
            # 把右侧 `updated.replace(old, new)` 计算得到的值保存到 `updated`，后面的判断或输出会继续使用它。
            updated = updated.replace(old, new)
        # 检查条件 `updated != text`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if updated != text:
            # 执行一次赋值：把 `"utf-8", newline="\n")` 的结果放入左侧 `doc.write_text(updated, encoding` 对应的变量/字段。
            doc.write_text(updated, encoding="utf-8", newline="\n")

    # 把右侧 `{"目录迁移": moves, "字节相同文件合并": aliases,` 计算得到的值保存到 `report`，后面的判断或输出会继续使用它。
    report = {"目录迁移": moves, "字节相同文件合并": aliases,
              # 给当前结果字典的“删除原则”字段填写 `"仅删除SHA-256完全相同且已有保留实体的副本；没有删除任何唯一字节内容"}`，这样导出的 JSON/表格能保留这一项证据。
              "删除原则": "仅删除SHA-256完全相同且已有保留实体的副本；没有删除任何唯一字节内容"}
    # 把已经整理好的文本写入输出文件；显式指定编码，保证中文证据在不同机器上可重复读取。
    (root / "证据" / "数据与证据整理映射.json").write_text(
        # 执行一次赋值：把 `False, indent=2) + "\n", encoding="utf-8")` 的结果放入左侧 `json.dumps(report, ensure_ascii` 对应的变量/字段。
        json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
    print(json.dumps({"迁移目录数": len(moves), "合并组数": len(aliases),
                      # 给当前结果字典的“移除重复实体数”字段填写 `sum(len(x["合并原路径"]) for x in aliases)}, ensure_ascii=False))`，这样导出的 JSON/表格能保留这一项证据。
                      "移除重复实体数": sum(len(x["合并原路径"]) for x in aliases)}, ensure_ascii=False))


# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__ == "__main__":
    # 调用 `main` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    main()
