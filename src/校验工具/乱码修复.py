# -*- coding: utf-8 -*-

import os
import sys
from pathlib import Path
import io
import csv


# ==================================================
# 内嵌重命名规则
#
# 左边 = 原文件名（不含扩展名）
# 右边 = 新文件名（不含扩展名）
#
# 例如:
# oldname,newname
#
# ==================================================

RENAME_CSV = r"""
NP穝瓜001,NP新圖001
NP穝瓜002,NP新圖002
NP穝瓜003,NP新圖003
NP穝瓜004,NP新圖004
NP穝瓜005,NP新圖005
NP穝瓜006,NP新圖006
NP穝瓜010,NP新圖010
NP穝瓜012,NP新圖012
NP穝瓜013,NP新圖013
NP穝瓜014,NP新圖014
NP穝瓜030,NP新圖030
NP穝瓜033,NP新圖033
いァ祇,中央發光
筿,電
霉筹┏畒,羅喉底座
霉筹ホ近,羅喉石輪
╦ビ,皇甫申
"""

def get_base_dir():
    """
    获取程序所在目录
    支持 py / exe
    """
    if getattr(sys, "frozen", False):
        return Path(sys.executable).parent

    return Path(__file__).parent



def load_rules():

    rules = {}

    reader = csv.reader(
        io.StringIO(RENAME_CSV.strip())
    )

    for row in reader:

        if len(row) < 2:
            continue

        old = row[0].strip()
        new = row[1].strip()

        if not old or not new:
            continue

        rules[old.lower()] = new


    return rules



def make_target(path, new_stem):

    """
    保留扩展名
    """

    return path.with_name(
        new_stem + path.suffix
    )



def unique_target(path):

    """
    防止覆盖已有文件
    """

    if not path.exists():
        return path


    index = 1

    while True:

        new_path = path.with_name(
            f"{path.stem}_{index}{path.suffix}"
        )

        if not new_path.exists():
            return new_path

        index += 1



def main():

    base = get_base_dir()

    rules = load_rules()


    report_file = base / "rename_report.txt"


    logs = []

    total_files = 0
    matched = 0
    success = 0
    failed = 0



    logs.append(
        "=============================="
    )

    logs.append(
        "批量重命名报告"
    )

    logs.append(
        "=============================="
    )

    logs.append(
        f"规则数量: {len(rules)}"
    )

    logs.append("")



    for root, dirs, files in os.walk(base):

        root = Path(root)


        for filename in files:

            total_files += 1


            # 跳过报告
            if filename == "rename_report.txt":
                continue


            file_path = root / filename


            # 去掉扩展名
            stem = file_path.stem


            key = stem.lower()


            if key not in rules:
                continue


            matched += 1


            new_stem = rules[key]


            target = make_target(
                file_path,
                new_stem
            )


            target = unique_target(target)


            try:

                file_path.rename(target)

                success += 1


                logs.append(
                    "[成功]"
                )

                logs.append(
                    str(file_path.relative_to(base))
                )

                logs.append(
                    "  ↓"
                )

                logs.append(
                    str(target.relative_to(base))
                )

                logs.append("")


            except Exception as e:


                failed += 1


                logs.append(
                    "[失败]"
                )

                logs.append(
                    str(file_path)
                )

                logs.append(
                    f"原因: {e}"
                )

                logs.append("")



    logs.append(
        "=============================="
    )

    logs.append(
        "统计"
    )

    logs.append(
        "=============================="
    )

    logs.append(
        f"扫描文件: {total_files}"
    )

    logs.append(
        f"匹配文件: {matched}"
    )

    logs.append(
        f"成功: {success}"
    )

    logs.append(
        f"失败: {failed}"
    )



    with open(
        report_file,
        "w",
        encoding="utf-8"
    ) as f:

        f.write(
            "\n".join(logs)
        )



    print(
        f"完成: 成功 {success} 个"
    )

    print(
        f"报告: {report_file}"
    )

    input(
        "按回车退出..."
    )



if __name__ == "__main__":
    main()