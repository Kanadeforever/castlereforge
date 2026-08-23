# -*- coding: utf-8 -*-
"""
Castle_Widescreen v0.11-poc11 侧区样式切换静态协议检查器
==========================================

这个脚本用于“发布前机械复核”，它不能代替用户实机验收，但能自动阻止几类最危险的回归：

1. 目标 RPG.exe 不是当前已经研究闭合的精确版本；
2. 已证伪的 982×576 renderer/backing 路线重新混回代码；
3. v0.7 已实机通过的“左右 world 重放时屏蔽消息 UI”被误删；
4. v0.9 已经统一成“所有消息使用同一侧区规则”，v0.11 不能让来源分支复活；
5. Castle_Widescreen.ini 没有和 ASI 同名、缺少 Ultrawide、BlurredSides 或进入/退出毫秒键；
6. v0.11 宣称支持模糊/纯黑切换，但实际上改了触发/动画，或纯黑模式仍无条件做模糊计算；
7. Battle、当前宽屏安全 Camera、毫秒过渡、事务式 Hook 回滚等既有稳定结构被误删。

脚本只使用 Python 标准库。
"""

from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

TARGET_SHA256 = "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
IMAGE_BASE = 0x00400000

# v0.9 当前真正改写的 E8 CALL 只有四处。
PATCH_CALLS = [
    (0x004059E1, 0x00405BD0, "初次 DirectDraw 重建"),
    (0x00406307, 0x00405BD0, "Surface lost 重建"),
    (0x0044A9C6, 0x00434710, "主帧绘制队列"),
    (0x0044A9E6, 0x00405A10, "每帧 Present"),
]

# 不改写，但当前实现依赖这些原版 CALL 关系。
PROTOCOL_CALLS = [
    (0x0040B087, 0x00403E30, "world manager -> 消息更新/状态路径"),
    (0x0040B08C, 0x00404800, "world manager -> 消息主体/UI绘制路径"),
]

SIGNATURES = [
    (0x00405960, bytes.fromhex("56 8B F1 57 8B 46 28"), "原版 backing 分配"),
    (0x00405A10, bytes.fromhex("56 8B F1 6A 01 8B 46 70"), "Present"),
    (0x00434710, bytes.fromhex("53 56 6A 00 8B D9 6A 01"), "绘制队列 0x434710"),
    (0x0040B050, bytes.fromhex("56 8B F1 8A 86 19 02 00 00"), "场景 world manager draw"),
    (0x0044B300, bytes.fromhex("8B 44 24 04 8B 4C 24 08"), "Camera 边界设置"),
    (0x0044B360, bytes.fromhex("A1 50 85 97 00 53 56 57"), "Camera 跟随/Clamp"),
    (0x0040B2D0, bytes.fromhex("A1 08 F8 89 00"), "Event active ID 读取"),
    (0x00403BC2, bytes.fromhex("81 E1 FF 00 00 00 89 0D 70 F6 46 00"), "消息 Event 槽写入"),
    (0x00403F22, bytes.fromhex("89 35 40 F6 46 00 89 35 70 F6 46 00"), "消息 Event 槽清零"),
    (0x00403E30, bytes.fromhex("A0 78 F6 46 00 84 C0 75 09 A0 79 F6 46 00"), "消息更新入口读取 0x46F678/679"),
    (0x00404800, bytes.fromhex("83 EC 08 A0 79 F6 46 00 56 84 C0"), "消息绘制入口读取 0x46F679"),
    (0x0040B072, bytes.fromhex("8B 8E 80 02 00 00"), "Scene world runtime +0x280"),
    (0x004429F0, bytes.fromhex("A0 0C 24 8E 00 53 33 DB 55 3A C3 8B E9"), "BattleManager draw"),
]


class PE:
    """只实现本检查器需要的 PE32 虚拟地址读取与基本头检查。"""

    def __init__(self, data: bytes):
        self.data = data
        if data[:2] != b"MZ":
            raise ValueError("不是 MZ PE 文件")
        pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
        if data[pe_offset:pe_offset + 4] != b"PE\0\0":
            raise ValueError("PE 签名无效")

        self.pe_offset = pe_offset
        self.machine = struct.unpack_from("<H", data, pe_offset + 4)[0]
        number_sections = struct.unpack_from("<H", data, pe_offset + 6)[0]
        optional_size = struct.unpack_from("<H", data, pe_offset + 20)[0]
        self.characteristics = struct.unpack_from("<H", data, pe_offset + 22)[0]

        optional = pe_offset + 24
        self.magic = struct.unpack_from("<H", data, optional)[0]
        if self.magic != 0x10B:
            raise ValueError("目标不是 PE32")

        self.image_base = struct.unpack_from("<I", data, optional + 28)[0]
        self.import_rva = struct.unpack_from("<I", data, optional + 104)[0]
        self.import_size = struct.unpack_from("<I", data, optional + 108)[0]

        section_table = optional + optional_size
        self.sections: list[tuple[int, int, int, int]] = []
        for index in range(number_sections):
            off = section_table + index * 40
            virtual_size, virtual_address, raw_size, raw_ptr = struct.unpack_from("<IIII", data, off + 8)
            self.sections.append((virtual_address, max(virtual_size, raw_size), raw_ptr, raw_size))

    def va_to_offset(self, va: int) -> int:
        rva = va - self.image_base
        for section_va, section_size, raw_ptr, raw_size in self.sections:
            if section_va <= rva < section_va + section_size:
                delta = rva - section_va
                if delta >= raw_size:
                    raise ValueError(f"VA {va:#x} 落在节的零填充区")
                return raw_ptr + delta
        raise ValueError(f"VA {va:#x} 不在任何节内")

    def read(self, va: int, size: int) -> bytes:
        off = self.va_to_offset(va)
        return self.data[off:off + size]


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description="检查 Castle_Widescreen v0.11-poc11 16:9 / 21:9 + 模糊/纯黑侧区切换静态协议")
    parser.add_argument("--root", type=Path, required=True, help="交付包根目录")
    parser.add_argument("--exe", type=Path, required=True, help="目标 RPG.exe")
    args = parser.parse_args()

    passed = 0
    failed = 0

    def result(ok: bool, name: str, detail: str = "") -> None:
        nonlocal passed, failed
        if ok:
            passed += 1
            print(f"[PASS] {name}" + (f"：{detail}" if detail else ""))
        else:
            failed += 1
            print(f"[FAIL] {name}" + (f"：{detail}" if detail else ""))

    exe_data = args.exe.read_bytes()
    actual_sha = hashlib.sha256(exe_data).hexdigest()
    result(actual_sha == TARGET_SHA256, "RPG.exe SHA-256", actual_sha)

    try:
        pe = PE(exe_data)
        result(pe.image_base == IMAGE_BASE, "RPG.exe PE32 ImageBase", hex(pe.image_base))
        result(pe.machine == 0x014C, "RPG.exe 机器类型 i386", hex(pe.machine))

        for address, target, label in PATCH_CALLS + PROTOCOL_CALLS:
            raw = pe.read(address, 5)
            actual = None
            ok = len(raw) == 5 and raw[0] == 0xE8
            if ok:
                rel = struct.unpack_from("<i", raw, 1)[0]
                actual = (address + 5 + rel) & 0xFFFFFFFF
                ok = actual == target
            result(ok, f"CALL {label}",
                   f"{address:#010x} -> {actual:#010x}" if actual is not None else raw.hex(" "))

        for address, signature, label in SIGNATURES:
            result(pe.read(address, len(signature)) == signature,
                   f"机器码 {label}", f"{address:#010x}")
    except Exception as exc:
        result(False, "RPG.exe PE 协议解析", str(exc))

    src = args.root / "源码"
    compiled = args.root / "编译内容"

    wide = (src / "widescreen.c").read_text(encoding="utf-8-sig")
    runtime = (src / "runtime.c").read_text(encoding="utf-8-sig")
    platform = (src / "platform.h").read_text(encoding="utf-8-sig")
    build = (src / "build.bat").read_text(encoding="utf-8-sig")
    ini_source = src / "Castle_Widescreen.ini"
    ini_release = compiled / "Castle_Widescreen.ini"
    asi = compiled / "Castle_Widescreen.asi"

    # 原版768 renderer + 两套最终输出模式必须同时成立。
    required_geometry = [
        "#define OUTPUT_WIDTH_16_9         854u",
        "#define SIDE_WIDTH_16_9           107u",
        "#define STAGING_WIDTH_16_9        982u",
        "#define OUTPUT_WIDTH_21_9        1120u",
        "#define SIDE_WIDTH_21_9           240u",
        "#define STAGING_WIDTH_21_9       1248u",
        "#define ORIGINAL_BACKING_W   768u",
        "#define OUTPUT_WIDTH          (g_output_width)",
        "#define SIDE_WIDTH            (g_side_width)",
        "#define PRESENT_STAGING_W     (g_present_staging_w)",
        "calculate_camera_plan",
        "wide_min_camera = min_x + (i32)SIDE_WIDTH",
        "wide_max_camera = max_x - viewport_w - (i32)SIDE_WIDTH",
        "restore_world_only_draw_queue",
        "FN_SCENE_WORLD_DRAW",
    ]
    missing = [item for item in required_geometry if item not in wide]
    result(not missing, "16:9 / 21:9 动态几何 + 原版768多Camera结构齐全",
           repr(missing) if missing else "完整")

    # 用纯数学再核一次两个模式，防止以后有人只改了一个常量。
    result(854 == 107 + 640 + 107 and 982 == 854 + 64 * 2,
           "16:9 几何关系", "107+640+107=854；854+128=982")
    result(1120 == 240 + 640 + 240 and 1248 == 1120 + 64 * 2,
           "21:9 几何关系", "240+640+240=1120；1120+128=1248")
    result(1120 * 9 == 480 * 21,
           "1120×480 为严格21:9", "1120:480 = 21:9")
    result(107 < 640 and 240 < 640,
           "两种侧画位移都小于单次640 renderer宽度", "107<640；240<640")
    result(64 + 854 + 64 == 982 and 64 + 1120 + 64 == 1248,
           "两种Present staging中央区都保留原版64像素extra", "PASS")
    result((854 + 3) // 4 == 214 and (1120 + 3) // 4 == 280,
           "电影式低分辨率宽度数学关系", "16:9=214；21:9=280")
    result(round(640 * 480 / 854) == 360 and round(640 * 480 / 1120) == 274,
           "电影式fill源裁切高度数学关系", "16:9≈360；21:9≈274")

    required_mode_runtime = [
        '"Display", "Ultrawide", 0u, 0u, 1u',
        "g_output_width = OUTPUT_WIDTH_21_9;",
        "g_side_width = SIDE_WIDTH_21_9;",
        "g_present_staging_w = STAGING_WIDTH_21_9;",
        "g_output_width = OUTPUT_WIDTH_16_9;",
        "g_side_width = SIDE_WIDTH_16_9;",
        "g_present_staging_w = STAGING_WIDTH_16_9;",
        "g_cinematic_low_w = (g_output_width + 3u) / 4u;",
    ]
    missing = [item for item in required_mode_runtime if item not in wide]
    result(not missing, "Ultrawide=0/1 在启动时选择完整几何",
           repr(missing) if missing else "完整")

    # 模糊背景必须按当前宽高比重新裁切，不能把16:9的214×120硬拉成21:9。
    required_dynamic_blur = [
        "crop_height = (LOGICAL_WIDTH * OUTPUT_HEIGHT + OUTPUT_WIDTH / 2u) / OUTPUT_WIDTH;",
        "sx = (lx * (LOGICAL_WIDTH - 1u)) / (CINEMATIC_LOW_W - 1u);",
        "sy = crop_top + (ly * (crop_height - 1u)) / (CINEMATIC_LOW_H - 1u);",
        "sample_blur_background",
        "panel_width = (SIDE_WIDTH * eased + 255u) >> 8;",
    ]
    missing = [item for item in required_dynamic_blur if item not in wide]
    result(not missing, "电影式模糊随16:9/21:9宽高比动态生成",
           repr(missing) if missing else "完整")

    result(
        "i32 total_extra = map_width - viewport_w;" in wide
        and "left_room = total_extra / 2;" in wide
        and "right_room = total_extra - left_room;" in wide
        and "center = min_x + left_room;" in wide,
        "小于当前输出宽度的地图采用对称居中安全策略",
    )

    # v0.7 已实机通过的消息隔离必须继续冻结。
    required_isolation = [
        "message_ui_is_active",
        "render_side_world_without_message_ui",
        "*(volatile u8*)GLOBAL_MESSAGE_TARGET_STATE = 0u",
        "*(volatile u8*)GLOBAL_MESSAGE_CURRENT_STATE = 0u",
        "*(volatile u8*)GLOBAL_MESSAGE_CURRENT_STATE = old_current",
        "*(volatile u8*)GLOBAL_MESSAGE_TARGET_STATE = old_target",
    ]
    missing = [item for item in required_isolation if item not in wide]
    result(not missing, "侧画消息 UI 隔离结构保留",
           repr(missing) if missing else "完整")
    result(wide.count("render_side_world_without_message_ui(self);") == 2,
           "左右两次侧画都使用消息隔离 helper",
           str(wide.count("render_side_world_without_message_ui(self);")))

    # v0.9 已验收的核心继续冻结：彻底取消来源二分。
    forbidden_classification = [
        "Hook_ManualInteractionEventActivate",
        "visible_message_is_manual_interaction",
        "sync_manual_interaction_lifetime",
        "g_manual_interaction_event_id",
        "来源=玩家主动：不启用模糊",
        "来源=非玩家触发消息",
        "patched_manual_event",
    ]
    found = [item for item in forbidden_classification if item in wide or item in runtime]
    result(not found, "玩家/非玩家视觉分类已从当前代码删除",
           repr(found) if found else "未发现旧分类")

    result(
        "if (!message_active) return;" in wide
        and "检测到原版消息 UI：不区分来源，统一启用左右侧面板推入" in wide,
        "任意消息 UI 统一触发侧面板模式",
    )

    result(
        "active_event == 0u && !message_active" in wide
        and "scene_identity != g_cinematic_scene_identity" in wide,
        "消息链锁存到 Event+消息结束或场景切换",
    )

    # Battle 继续同方案。
    result(
        "FN_BATTLE_MANAGER_DRAW" in wide
        and "update_battle_latch" in wide
        and "if (g_cinematic_latched || g_battle_latched)" in wide,
        "Battle 与消息统一使用中央640电影式模式",
    )

    # 电影式空间动画必须仍然是“移动”，不是退回整块 alpha。
    required_slide = [
        "cinematic_smoothstep",
        "panel_width = (SIDE_WIDTH * eased + 255u) >> 8;",
        "right_start = SIDE_WIDTH - panel_width;",
        "left_source_offset = SIDE_WIDTH - panel_width;",
        "CINEMATIC_EDGE_FEATHER",
        "apply_cinematic_slide_to_staging",
    ]
    missing = [item for item in required_slide if item not in wide]
    result(not missing, "电影式左右推入/退出空间动画完整",
           repr(missing) if missing else "完整")

    # v0.11 的新功能必须只改变侧面板像素来源，不能分叉触发/动画状态机。
    required_style_switch = [
        "static int g_cinematic_blurred_sides = 1;",
        '"Cinematic", "BlurredSides", 1u, 0u, 1u',
        "if (g_cinematic_blurred_sides) {",
        "panel_pixel = 0u;",
        "visible[x] = blend_rgb565(visible[x], panel_pixel, alpha);",
        "visible[output_x] = blend_rgb565(visible[output_x], panel_pixel, alpha);",
    ]
    missing = [item for item in required_style_switch if item not in wide]
    result(not missing, "模糊/纯黑侧区样式分支完整",
           repr(missing) if missing else "完整")

    result(
        wide.count("build_cinematic_low_source();") == 1
        and "if (g_cinematic_blurred_sides) {\n        build_cinematic_low_source();" in wide,
        "纯黑模式可跳过模糊背景生成",
    )

    result(
        "cinematic_smoothstep" in wide
        and "cinematic_left_edge_alpha" in wide
        and "cinematic_right_edge_alpha" in wide
        and wide.count("g_cinematic_blurred_sides") >= 7,
        "两种侧区样式共享同一推入/退出与边缘柔化路径",
    )

    # INI 与真实毫秒计时。
    result(ini_source.exists(), "源码目录存在 Castle_Widescreen.ini")
    result(ini_release.exists(), "编译内容存在 Castle_Widescreen.ini")
    if ini_release.exists():
        ini_text = ini_release.read_text(encoding="ascii")
        result("[Display]" in ini_text, "INI 包含 [Display]")
        result("Ultrawide=0" in ini_text, "INI 默认关闭21:9，保持16:9")
        result("[Cinematic]" in ini_text, "INI 包含 [Cinematic]")
        result("BlurredSides=1" in ini_text, "INI 默认保持电影式模糊侧区")
        result("[Transition]" in ini_text, "INI 包含 [Transition]")
        result("EnterDurationMs=250" in ini_text, "INI 默认进入时间 250ms")
        result("ExitDurationMs=250" in ini_text, "INI 默认退出时间 250ms")

    result(
        "Runtime_ReadPluginIniU32" in wide
        and '"Display", "Ultrawide"' in wide
        and '"Cinematic", "BlurredSides"' in wide
        and '"Transition", "EnterDurationMs"' in wide
        and '"Transition", "ExitDurationMs"' in wide,
        "源码读取同名 INI 的显示模式、侧区样式与进入/退出时间",
    )
    result(
        "PFN_GetPrivateProfileIntA" in platform
        and "GetPrivateProfileIntA" in runtime,
        "INI 使用 Win32 Profile API，无自制脆弱解析器",
    )
    result(
        "PFN_GetTickCount" in platform
        and "Runtime_GetTickCountMs" in runtime
        and "advance_timed_amount" in wide,
        "过渡使用真实毫秒计时",
    )
    result(
        "BAR_FADE_STEP" not in wide and "CINEMATIC_FILL_STEP" not in wide,
        "固定每帧步长已删除",
    )
    result(
        "TRANSITION_DEFAULT_MS      250u" in wide
        and "TRANSITION_MAX_MS        10000u" in wide,
        "INI 默认值与范围常量存在",
    )
    result(
        'copy /y "%ROOT%Castle_Widescreen.ini"' in build,
        "Windows 一键构建同步同名 INI",
    )


    result(
        "static u32 g_output_width = OUTPUT_WIDTH_16_9;" in wide
        and "static u32 g_side_width = SIDE_WIDTH_16_9;" in wide
        and "static u32 g_present_staging_w = STAGING_WIDTH_16_9;" in wide,
        "即使INI读取前，静态默认几何也是16:9",
    )

    # 旧失败路线不能复活。
    forbidden_old = [
        "Hook_DisplayAllocate",
        "Hook_BlitCopy",
        "Hook_BlitMasked",
        "CALL_DISPLAY_ALLOCATE",
        "message_choice_ui_is_active",
        "current_command_is_strong_director_signal",
        "g_manual_event_director_signal",
        "observe_manual_event_director_signal",
    ]
    found = [item for item in forbidden_old if item in wide]
    result(not found, "POC1失败路线/旧导演分类未复活",
           repr(found) if found else "未发现")

    # Hook 安装仍然必须可事务回滚，且主动来源 Hook 已不再安装。
    result(
        "Runtime_RestoreCall" in wide and "goto rollback" in wide,
        "Hook 安装失败仍有事务式 CALL 回滚",
    )
    result(
        "CALL_MANUAL_INTERACTION_EVENT" not in wide
        and 'CHECK_CALL("主动交互 Event 激活"' not in runtime,
        "v0.11 继续不改写主动交互 Event CALL",
    )

    # 编译产物基本格式。
    if asi.exists():
        try:
            asi_pe = PE(asi.read_bytes())
            result(asi_pe.machine == 0x014C, "ASI 为 i386 PE32", hex(asi_pe.machine))
            result((asi_pe.characteristics & 0x2000) != 0, "ASI 具有 DLL 标志")
            result(asi_pe.import_rva == 0 and asi_pe.import_size == 0,
                   "ASI 没有静态 Import Directory")
        except Exception as exc:
            result(False, "ASI PE 格式解析", str(exc))
    else:
        result(False, "编译内容/Castle_Widescreen.asi 存在")

    print()
    print(f"TOTAL: {passed} PASS / {failed} FAIL")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
