#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""编译并执行编辑器实际使用的 Unicode 校验函数；不启动游戏，不写用户配置。"""

from __future__ import annotations

import pathlib
import shutil
import subprocess
import sys
import tempfile


def main() -> int:
    # 测试直接截取生产函数，避免测试里另写一份相似算法、两边同时出错却互相证明。
    source = pathlib.Path(__file__).resolve().parents[1] / "source" / "launcher_gui.c"
    text = source.read_text(encoding="utf-8-sig")
    begin = text.index("static int toml_utf16_is_valid_(")
    end = text.index("static int save_ini_editor_file_(", begin)
    implementation = text[begin:end]
    compiler = shutil.which("cl.exe")
    if not compiler:
        print("[失败] 请在 Visual Studio 开发者命令行中运行本检查器。")
        return 2

    # WCHAR 固定成 16 位，和 Win32 编辑器一致。用例包含合法扩展字符、断裂代理对及三种换行。
    harness = r'''
#include <stdio.h>
typedef unsigned short WCHAR;
typedef unsigned int UINT;
'''+implementation+r'''
struct Sample {
    WCHAR text[12];
    UINT size;
    int expected;
    UINT index;
    UINT line;
};
int main(void) {
    static const struct Sample samples[] = {
        {{0}, 0, 1, 0, 0},
        {{0x4E2D, 0x6587}, 2, 1, 0, 0},
        {{0xD840, 0xDC00}, 2, 1, 0, 0},
        {{0xDBFF, 0xDFFF}, 2, 1, 0, 0},
        {{0xD800}, 1, 0, 0, 1},
        {{0xDC00}, 1, 0, 0, 1},
        {{0xD800, 'A'}, 2, 0, 0, 1},
        {{0xD800, 0xD800}, 2, 0, 0, 1},
        {{0xD840, 0xDC00, 0xDC00}, 3, 0, 2, 1},
        {{'a', '\r', 0xD800}, 3, 0, 2, 2},
        {{'a', '\n', 0xD800}, 3, 0, 2, 2},
        {{'a', '\r', '\n', 0xD800}, 4, 0, 3, 2},
        {{'\r', '\r', '\n', '\n', 0xDC00}, 5, 0, 4, 4}
    };
    UINT test;
    for (test = 0; test < sizeof(samples)/sizeof(samples[0]); ++test) {
        UINT index = 999, line = 999;
        const struct Sample* sample = &samples[test];
        int result = toml_utf16_is_valid_(sample->text, sample->size, &index, &line);
        if (result != sample->expected || index != sample->index || line != sample->line) {
            printf("FAIL case=%u result=%d index=%u line=%u\n", test, result, index, line);
            return 1;
        }
    }
    if (toml_utf16_is_valid_(0, 0, 0, 0)) return 2;
    if (!toml_utf16_is_valid_(samples[2].text, samples[2].size, 0, 0)) return 3;
    if (toml_utf16_is_valid_(samples[4].text, samples[4].size, 0, 0)) return 4;
    puts("PASS: 16 Unicode cases");
    return 0;
}
'''
    # 编译产物仅存在于系统临时目录，退出时自动回收，绝不进入发行 build 目录。
    with tempfile.TemporaryDirectory(prefix="castle_toml_editor_") as directory:
        root = pathlib.Path(directory)
        test_source = root / "unicode_test.c"
        test_source.write_text(harness, encoding="utf-8")
        build = subprocess.run(
            [compiler, "/nologo", "/TC", "/W4", "/WX", "/Od", "/utf-8", "/MT",
             str(test_source), "/Founicode_test.obj", "/Feunicode_test.exe"],
            cwd=root, capture_output=True, timeout=60,
        )
        if build.returncode:
            sys.stdout.buffer.write(build.stdout + build.stderr)
            return build.returncode
        run = subprocess.run([str(root / "unicode_test.exe")], capture_output=True, timeout=15)
        sys.stdout.buffer.write(run.stdout + run.stderr)
        return run.returncode


if __name__ == "__main__":
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    raise SystemExit(main())
