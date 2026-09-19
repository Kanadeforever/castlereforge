#!/usr/bin/env python3
# 这个脚本只负责“给固化包里的每个文件计算SHA-256并生成目录索引”。
# 它不会移动、删除或修改被索引文件；唯一会写入的是用户通过 --输出 指定的JSON索引文件。
"""固化包全文件SHA-256索引器：遍历包内文件并生成可复核的路径、大小和哈希清单。"""

# 启用较新的 Python 注解语法行为，让类型注解先按文字保存，避免旧版本在定义阶段立即求值。
from __future__ import annotations

# 导入 `hashlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import hashlib
# 导入 `json` 模块；这里只取得标准库功能，不会修改游戏文件。
import json
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path


# 定义函数 `sha256`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sha256(path: Path) -> str:
    # 把右侧 `hashlib.sha256()` 计算得到的值保存到 `h`，后面的判断或输出会继续使用它。
    h = hashlib.sha256()
    # 进入受管理的资源作用域 `path.open("rb") as f`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
    with path.open("rb") as f:
        # 开始循环 `block in iter(lambda: f.read(1024 * 1024), b"")`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for block in iter(lambda: f.read(1024 * 1024), b""):
            # 调用 `h.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
            h.update(block)
    # 把 `h.hexdigest()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return h.hexdigest()


# 定义函数 `inventory`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def inventory(root: Path, excluded: Path | None = None) -> list[dict[str, object]]:
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows = []
    # 开始循环 `path in sorted(p for p in root.rglob("*") if p.is_file() and p.resolve() != excluded)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for path in sorted(p for p in root.rglob("*") if p.is_file() and p.resolve() != excluded):
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append({
            # 给当前结果字典的“路径”字段填写 `path.relative_to(root).as_posix(),`，这样导出的 JSON/表格能保留这一项证据。
            "路径": path.relative_to(root).as_posix(),
            # 给当前结果字典的“大小”字段填写 `path.stat().st_size,`，这样导出的 JSON/表格能保留这一项证据。
            "大小": path.stat().st_size,
            # 给当前结果字典的“SHA256”字段填写 `sha256(path),`，这样导出的 JSON/表格能保留这一项证据。
            "SHA256": sha256(path),
        })
    # 把 `rows` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return rows


# 定义函数 `main`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def main() -> None:
    # 导入 `argparse` 模块；这里只取得标准库功能，不会修改游戏文件。
    import argparse
    # 把右侧 `argparse.ArgumentParser()` 计算得到的值保存到 `parser`，后面的判断或输出会继续使用它。
    parser = argparse.ArgumentParser()
    # 向命令行解析器登记一个参数规则，告诉工具这个参数的名字、类型以及是否必填。
    parser.add_argument("根目录", type=Path)
    # 向命令行解析器登记一个参数规则，告诉工具这个参数的名字、类型以及是否必填。
    parser.add_argument("--输出", type=Path, required=True)
    # 把右侧 `parser.parse_args()` 计算得到的值保存到 `args`，后面的判断或输出会继续使用它。
    args = parser.parse_args()
    # 把右侧 `inventory(args.根目录, args.输出.resolve())` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows = inventory(args.根目录, args.输出.resolve())
    # 把右侧 `{` 计算得到的值保存到 `result`，后面的判断或输出会继续使用它。
    result = {
        # 给当前结果字典的“说明”字段填写 `"索引覆盖包内除本索引文件自身以外的全部文件；自身哈希由外层ZIP哈希校验。",`，这样导出的 JSON/表格能保留这一项证据。
        "说明": "索引覆盖包内除本索引文件自身以外的全部文件；自身哈希由外层ZIP哈希校验。",
        # 给当前结果字典的“已索引文件数”字段填写 `len(rows),`，这样导出的 JSON/表格能保留这一项证据。
        "已索引文件数": len(rows),
        # 给当前结果字典的“文件”字段填写 `rows,`，这样导出的 JSON/表格能保留这一项证据。
        "文件": rows,
    }
    # 把刚才整理好的索引字典写成UTF-8 JSON。ensure_ascii=False让中文路径直接可读，indent=2便于人工审查。
    # 注意：这里只写“索引文件本身”，不会改动rows里列出的任何被索引文件。
    args.输出.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
    print(f"已索引 {len(rows)} 个文件：{args.输出}")


# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__ == "__main__":
    # 调用 `main` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
    main()
