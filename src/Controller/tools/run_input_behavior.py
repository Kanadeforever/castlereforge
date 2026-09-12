"""编译并运行真实C源码行为宿主；产物只留在临时目录，源码和游戏资源只读。"""
import argparse
import os
from pathlib import Path
import subprocess
import shutil
import tempfile


def main():
    parser = argparse.ArgumentParser(description="运行x86输入行为宿主")
    parser.add_argument("--source", type=Path, default=Path(__file__).with_name("input_behavior_test.c"))
    args = parser.parse_args()
    # 自动发现VS，与正式构建采用同一规则，不依赖开发者编译器地址记录。
    locator = Path(os.environ["ProgramFiles(x86)"]) / "Microsoft Visual Studio/Installer/vswhere.exe"
    install = subprocess.check_output([str(locator), "-latest", "-products", "*", "-requires",
        "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"], text=True).strip()
    sdk = Path(__file__).resolve().parents[2] / "RuntimeSDK/include"
    compiler = shutil.which("clang-cl")
    if not compiler:
        raise RuntimeError("未找到clang-cl，请安装LLVM并加入PATH。")
    environment = os.environ.copy()
    environment["PATH"] = str(Path(os.environ["SystemRoot"])/"System32") + ";" + environment.get("PATH", "")
    for key in ("INCLUDE", "LIB", "LIBPATH", "VSCMD_VER", "VSCMD_ARG_TGT_ARCH", "VSCMD_ARG_HOST_ARCH", "__VSCMD_PREINIT_PATH", "VSINSTALLDIR", "VCINSTALLDIR", "VISUALSTUDIOVERSION"):
        environment.pop(key, None)
    with tempfile.TemporaryDirectory(prefix="castle_input_test_") as folder:
        # cmd只初始化工具环境并编译；不执行删除或移动。临时目录由标准库负责清理。
        command = (f'set "PATH=%SystemRoot%\\System32;%PATH%" && call "{install}\\Common7\\Tools\\VsDevCmd.bat" -no_logo -arch=x86 -host_arch=x64 >nul'
                   f' && "{compiler}" --target=i686-pc-windows-msvc /nologo /TC /W4 /WX /Od /utf-8 /MT /I"{sdk}" "{args.source.resolve()}" /Fe:test.exe /link /BASE:0x10000000')
        subprocess.run('cmd /d /s /c "' + command + '"', cwd=folder, env=environment, check=True, timeout=60)
        subprocess.run([str(Path(folder)/"test.exe")], cwd=folder, check=True, timeout=30)


if __name__ == "__main__":
    main()
