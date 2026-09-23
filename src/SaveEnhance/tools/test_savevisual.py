"""编译并运行可视反馈宿主测试；参考游戏只读，测试状态隔离在自动清理的临时目录。"""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import uuid


def development_environment() -> dict[str, str]:
    # 只整理当前子进程的PATH，不修改注册表。过度重复的PATH可能超过cmd的8191字符限制。
    env = os.environ.copy()
    entries = env.get("PATH", "").split(";")
    env["PATH"] = ";".join(dict.fromkeys(item for item in entries if item and item != "%PATH%"))
    locator = Path(env.get("ProgramFiles(x86)", r"C:\Program Files (x86)")) / "Microsoft Visual Studio/Installer/vswhere.exe"
    if locator.is_file():
        installation = subprocess.check_output([
            str(locator), "-latest", "-products", "*", "-requires",
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath",
        ], env=env, text=True).strip()
        if installation:
            script = Path(installation) / "Common7/Tools/VsDevCmd.bat"
            # set输出仅在内存中解析，不能把包含私人路径/环境设置的整张表输出到日志。
            command = f'call "{script}" -arch=x86 -host_arch=x64 -no_logo >nul && set'
            # cmd不使用C运行库的反斜线转义规则，因此不能把含双引号的命令交给list2cmdline。
            shell = env.get("COMSPEC", "cmd.exe")
            raw = subprocess.check_output(f'"{shell}" /d /s /c "{command}"', env=env)
            for line in raw.decode("mbcs").splitlines():
                if "=" in line and not line.startswith("="):
                    key, value = line.split("=", 1)
                    env[key.upper()] = value
    if not shutil.which("cl.exe", path=env.get("PATH")):
        raise RuntimeError("需要Visual Studio C++工具链或已经初始化的x86开发者命令行")
    return env


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--game-directory", type=Path, required=True, help="含exe和multimedia的只读游戏根目录")
    args = parser.parse_args()
    game = args.game_directory.resolve()
    if not (game / "multimedia/Sys/Sys.dat").is_file():
        parser.error("游戏目录缺少multimedia/Sys/Sys.dat")
    module = Path(__file__).resolve().parents[1]
    env = development_environment()
    # 编译物和.LATESTSLOTS样本全部进入此专用临时目录，退出时无论成功/失败都删除。
    build_directory = module / "_build"
    existed = build_directory.exists()
    build_directory.mkdir(exist_ok=True)
    try:
        return run_tests(module, game, env, build_directory)
    finally:
        # 只移除本工具新建且已经为空的_build，不能清掉其它构建任务留下的内容。
        if not existed:
            build_directory.rmdir()


def run_tests(module: Path, game: Path, env: dict[str, str], build_directory: Path) -> int:
    # Python3.13起的TemporaryDirectory在Windows使用0700 ACL；受限构建账户可能无法访问。
    # 普通mkdir继承项目权限，随机名称仍保证不同测试不会共用状态文件。
    output = build_directory / ("visual-tests-" + uuid.uuid4().hex)
    output.mkdir()
    try:
        executable = output / "visual_host_tests.exe"
        # Windows的CreateProcess不会按传入env的PATH寻找主程序，必须显式解析刚取得的x86 cl。
        compiler = shutil.which("cl.exe", path=env["PATH"])
        command = [compiler, "/nologo", "/std:c++17", "/utf-8", "/W4", "/WX", "/EHsc",
                   "/I" + str(module.parent / "RuntimeSDK/include"),
                   "/Fo" + str(output / "visual_host_tests.obj"), "/Fe" + str(executable),
                   str(module / "tools/visual_host_tests.cpp")]
        result = subprocess.run(command, cwd=output, env=env, check=False)
        if result.returncode:
            return result.returncode
        return subprocess.run([str(executable), str(game), str(output)], env=env, check=False).returncode
    finally:
        # 删除前确认它确实是本次新建的直接子目录，绝不能递归删除模块或_build根。
        if output.resolve().parent != build_directory.resolve() or not output.name.startswith("visual-tests-"):
            raise RuntimeError("拒绝清理越界临时路径")
        shutil.rmtree(output)


if __name__ == "__main__":
    raise SystemExit(main())
