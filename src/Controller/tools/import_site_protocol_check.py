"""只读核对全部鼠标API指令、两插件登记表以及不再缓存启动期坐标API的合同。"""
import argparse
from pathlib import Path
import re
import runpy
import struct


def main():
    parser = argparse.ArgumentParser(description="验证鼠标API固定站点的完整覆盖")
    parser.add_argument("--exe", required=True, type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[3]
    # 复用已有标准库PE解析器，不增加pefile/反汇编器依赖。
    wide_tools = root / "src/Widescreen/tools/widescreen_check.py"
    pe = runpy.run_path(str(wide_tools))["PE"](args.exe.read_bytes())
    wide = (root / "src/Widescreen/source/runtime.c").read_text(encoding="utf-8-sig")
    cursor = (root / "src/Controller/source/cursor.c").read_text(encoding="utf-8-sig")
    controller = (root / "src/Controller/source/runtime.c").read_text(encoding="utf-8-sig")
    # 来源是原版.text反汇编结果；这是独立的期望表，不从待测生产数组生成答案。
    expected = {
        0x460204: ("cursor_sites", [0x4044F9, 0x404608, 0x408835, 0x430D9B, 0x43DF5B]),
        0x4601A8: ("key_sites", [0x4018F9, 0x40458E, 0x404672, 0x408953, 0x408A72,
                                 0x41B029, 0x41D936, 0x430DE2, 0x435186, 0x43E005]),
        0x4601A4: ("async_sites", [0x4041D7, 0x40447E]),
        0x46019C: ("set_sites", [0x43DF3A]),
    }
    checks = 0
    for slot, (name, addresses) in expected.items():
        found = []
        # 只扫描原版代码节；每个候选再核对绝对CALL或MOV函数装载指令编码。
        section = next(s for s in pe.sections if s[0] == 0x1000)
        code = pe.data[section[2]:section[2] + section[3]]
        for offset in range(len(code)-5):
            if struct.unpack_from("<I", code, offset+2)[0] != slot:
                continue
            if code[offset:offset+2] == b"\xff\x15" or (
                    code[offset] == 0x8B and code[offset+1] & 0xC7 == 5):
                found.append(pe.image_base + section[0] + offset)
        assert found == addresses, (hex(slot), found, addresses)
        match = re.search(rf"{name}\[\]\s*=\s*\{{([^}}]+)\}}", wide)
        assert match, name
        declared = [int(value, 16) for value in re.findall(r"0x([0-9A-Fa-f]+)u", match[1])]
        assert declared == addresses, (name, declared)
        if name == "key_sites":
            match = re.search(r"key_sites\[\]\s*=\s*\{([^}]+)\}", cursor)
            assert [int(v, 16) for v in re.findall(r"0x([0-9A-Fa-f]+)u", match[1])] == addresses
        checks += len(addresses)
    for field, function in (("get_cursor_pos", "runtime_current_get_cursor"),
                            ("set_cursor_pos", "runtime_current_set_cursor"),
                            ("get_client_rect", "runtime_current_client_rect"),
                            ("client_to_screen", "runtime_current_client_to_screen")):
        assert f"g_api.{field} = {function};" in controller, field
    assert "Runtime_PatchIatPointer(IAT_GETKEYSTATE" not in cursor
    assert "Runtime_PatchIatPointer(IAT_SETCURSORPOS" not in cursor
    print(f"PASS {checks} exact import sites; shared keys and 4 dynamic coordinate APIs")


if __name__ == "__main__":
    main()
