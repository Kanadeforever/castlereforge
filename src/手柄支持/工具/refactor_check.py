#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》手柄操控模组 v0.3-refactor37 综合静态检查工具。

这个工具只使用 Python 标准库，不修改 RPG.exe，也不修改源码。
它把这次重构最容易发生的“大回归”变成可以重复执行的机械检查：

1. 如果提供 RPG.exe，就校验精确 SHA-256、refactor4/refactor7 封版的 46 个 CALL、8 个核心协议、Start=ESC 的 2 个电影协议，并校验 refactor11 已验收的 4 个对话局部协议签名、2 个历史询问框 vtable Update 槽与第二变体 4 个 CALL；
2. 检查新架构中业务模块不能直接读取 SDL/物理手柄按钮；
3. 检查 PASS/MERGE/OVERRIDE/CONSUME、COMMON/OVERLAY，以及本轮新增的每 tick 输入消费机制确实存在于代码；
4. 检查普通对话 A 只通过公共消息引擎两处局部函数指针装载生效，并明确分成“逐字补全”和“整句推进”两个包装函数，不修改全局 GetAsyncKeyState IAT；
5. 检查 refactor10/11 遗留的 ConfirmDialog 历史候选只保留在严格隔离范围内：Battle/SaveSlot 必须优先，且禁止恢复 refactor8 的 HitTest 时间窗或 refactor9 的 open-CALL 猜测；这项检查不代表通用 Yes/No 已验收，也不把它当成未来菜单架构；
6. 检查 refactor20a 主 Interface 兼容入口：Y 不发送 Space；运行时只抓 Space/busy/mode/action=1 语义特征，并从原版 Space 的 CALL 现场解析当前业务目标；同时检查动画期 Shell 输入缓冲；
7. 检查 UiBridge pending 必须带 owner；检查 Battle 禁用项 A 的确认事务具有“建立前拒绝 + 延迟确认超时 + 从原版消费时刻重新计时 + 无状态变化自动解锁 + B 逃生”五道门；同时检查 state2/state3/state4/state5 各自独立 Adapter；
8. 检查 SaveSlot、Battle 顶层/子列表视觉、Target/Cursor 等此前实机封版协议没有结构性回归；
9. 检查 dev20 的 INI 对外键名仍然完整；
10. 检查地图十字键只提供八方向步行，且松开后保留左摇杆既有全向走跑阈值；
11. 检查统一 Shop Adapter 保留 refactor36 已实机通过的连续翻页、Y 信息窗与列标记；
12. 检查 refactor37 的 Back常驻／地图RT临时鼠标优先级、LT原版resolver调查、防穿透与震动仲裁；
13. 检查 build.bat 逐个编译 29 个独立 .c（含 ControlModes 与 Investigation），并保留 x86、/W4 /WX、UTF-8、无 CRT 约束；
14. 检查编译产物确实是 PE32 / i386 DLL；
15. 检查源码文件名均为英文/ASCII，并给出注释覆盖率，帮助持续遵守“项目圣经”；
16. 检查所有现行说明文档都集中在“文档”目录，且不再携带逐版“证据”树；
16. 检查已验收业务底座保持受控：稳定旧模块继续 SHA 锁死；state3 技能页、state4 及身、state5 五内、Battle Target、法宝确认框与热插拔按用户 PASS 边界保护；并继续检查 r19 主 Interface、Back 精细鼠标会话与 r15/r16 输入底座；
17. 检查 SDL3 热插拔顺序固定为 UpdateGamepads -> 旧句柄连接检查 -> 低频重新枚举 -> 采样，并要求新连接第一帧同步 prev/current 以避免插拔误触；
18. 检查 refactor27 已实机通过的 state7 天书仍复用共享 SaveSlot，历史五个 SaveSlot Hook 与 SaveAction Hook 分离安装，并禁止页面 Adapter 自己做存档文件 I/O；
19. 检查 refactor27 已实机通过的 state8 机能仍只把 LT/RT/A 送进五个原版 ButtonEvent；refactor28 仅允许焦点坐标小幅左/下微调，且不得直接写音乐/音效数值；
20. 检查 refactor28 的 state3 治疗目标阶段只复用 page+0x768、五个角色真实 Button 与全屏取消 Button：左右只改插件私有焦点，A 注入 code=2，B 注入 code=1，禁止直接写治疗目标业务字段，并要求该状态作为 modal 阻断 InterfaceShell；
21. 检查 refactor29 客栈根层“诸态/炼化/歇息”只使用真实 Button，B 不制造不存在的退出；
22. 检查 refactor29 炼化根层 B 必须点击“用器”右侧退出图标，第二层 B 必须点击专属取消 Button，禁止鼠标右键；
23. 检查公共剧情 mode=3 两项选择使用 0x404600 真实鼠标命中/原版选择框/左键协议，不直接写剧情结果；
24. 检查地图存档点的 0x89FCD0 独立包装层只负责解包 +0x580 SaveSlot，并复用唯一一套槽位/翻页/存档业务；
25. 检查共享 SaveSlot 三项动作 Hook 不再依赖天书 owner，存档点内的二次 Yes/No 可继续读取手柄，前置剧情询问的 LEFTUP 不会被包装层阻断；
26. 检查客栈由父层记住进入诸态/炼化前的项目，子界面返回后逻辑与 HitTest 视觉共同恢复同一入口，且禁止子模块反向回写首项。

注意：静态 PASS 不等于实机 PASS。这个工具只能证明“我们没有明显破坏已经闭合的结构和地址协议”，
不能替代用户在真实 RPG.exe 中验证视觉节奏、菜单动画、鼠标所有权和手柄手感。
"""

from __future__ import annotations

import argparse
import hashlib
import re
import struct
import sys
from pathlib import Path


# refactor27 起接受两个经过原版来源确认、且逐协议检查一致的台湾第三版样本。
# 这里的 SHA 白名单只是“允许继续进入地址协议验证”的第一道门，不代表只要 SHA 命中就无条件安装 Hook。
# 后面的 CALL 目标、短机器码和结构签名仍会逐项检查；这样即使未来有人误把别的 EXE 加进白名单，也不会绕过能力级 fail-closed。
SUPPORTED_SHA256 = {
    "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf": "历史精确目标样本",
    "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f": "RPG.exe.org 原版可信样本",
}
EXPECTED_SHA256 = "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
IMAGE_BASE = 0x00400000


# 这 8 组机器码不是 Hook 本身，而是 dev20 历史已经闭合的关键“协议锚点”。
# 例如 SaveSlot 的 +0x5BC=4 动画启动链如果变了，即使 CALL 地址还一样，也不能继续假设 UI 语义相同。
BYTE_SIGNATURES = [
    (0x0043E1B0, bytes.fromhex("8A 81 48 02 00 00 84 C0"), "原版游戏内光标绘制入口"),
    (0x0043DF30, bytes.fromhex("8B 44 24 08 8B 4C 24 04 50 51 FF 15 9C 01 46 00"), "MouseManager SetCursorPos 包装器"),
    (0x004461F4, bytes.fromhex("8B 8E 28 10 00 00 83 C8 FF 2B CF 89 44 24 14"), "Target mode 分派入口"),
    (0x0044620B, bytes.fromhex("33 C0 C7 44 24 14 1C 00 00 00"), "Target mode2：允许 0..27"),
    (0x00446217, bytes.fromhex("B8 14 00 00 00 C7 44 24 14 1C 00 00 00"), "Target mode1：20..27"),
    (0x00446226, bytes.fromhex("33 C0 C7 44 24 14 08 00 00 00"), "Target mode0：0..7"),
    (0x00424CB5, bytes.fromhex("3B C5 74 10 C7 86 BC 05 00 00 04 00 00 00 89 AE C0 05 00 00"), "SaveSlot 四步选中动画启动"),
    (0x00425478, bytes.fromhex("3B 91 94 05 00 00 75 1A"), "SaveSlot refresh selected-row 比较"),
]


# Start=ESC 只依赖原窗口过程的两个只读协议。
# 这两项是可选能力，不并入 refactor4 核心 8 个签名的全局 fail-closed。
MOVIE_SIGNATURES = [
    (0x004018AB, bytes.fromhex("A1 90 F3 46 00 53 56 57 85 C0"), "原版电影活动对象判定"),
    (0x004018EF, bytes.fromhex("8B 7D 10 83 FF 1B 74 16"), "原版 ESC 关闭电影条件"),
]


# refactor11 普通对话两阶段 A 仍依赖同一组四个只读协议。
# 两个输入点现在只校验 6 字节 mov esi,[0x4601A4]，运行时会把它局部改成 mov esi,wrapper + nop。
DIALOGUE_SIGNATURES = [
    (0x00403B90, bytes.fromhex("A1 70 F6 46 00 53 33 DB 3B C3"), "公共消息活动槽入口"),
    (0x004041D7, bytes.fromhex("8B 35 A4 01 46 00"), "公共消息第一处局部 GetAsyncKeyState 装载"),
    (0x0040447E, bytes.fromhex("8B 35 A4 01 46 00"), "公共消息第二处局部 GetAsyncKeyState 装载"),
    (0x00403F28, bytes.fromhex("89 35 70 F6 46 00"), "公共消息活动槽清零"),
]

# refactor10 不再依赖 8 个业务 open CALL。
# 它直接验证两种双按钮类 vtable Update 槽，并继续验证第二种 Update 内 4 个 HitTest/Event CALL。
CONFIRM_VTABLE_SLOTS = [
    (0x00460C20, 0x004272C0, "第一种双按钮 vtable Update"),
    (0x00460C40, 0x004276F0, "第二种双按钮 vtable Update"),
]

CONFIRM_DIALOG_CALLS = [
    (0x00427800, 0x00431310, "第二种 Yes HitTest"),
    (0x00427819, 0x00431310, "第二种 No HitTest"),
    (0x0042782E, 0x00431380, "第二种 Yes Event"),
    (0x0042784F, 0x00431380, "第二种 No Event"),
]

# refactor20a 主 Interface 不再把 0x40B230 函数体当作固定指纹。
# 只验证“Space -> busy/mode 门 -> action=1”语义特征；Jcc/短跳位移用 mask 忽略。
INTERFACE_SPACE_FEATURE = (
    0x0044A4DF,
    bytes.fromhex("83 F8 20 0F 85 00 00 00 00 39 1D 08 F8 89 00 0F 85 00 00 00 00 39 1D F0 8B 46 00 0F 84 00 00 00 00 6A 01 EB 00"),
    bytes.fromhex("FF FF FF FF FF 00 00 00 00 FF FF FF FF FF FF FF FF 00 00 00 00 FF FF FF FF FF FF FF FF 00 00 00 00 FF FF FF 00"),
    "原版 Space/busy/mode/action=1 地图业务特征",
)

INTERFACE_READY_SIGNATURE = (
    0x00436330,
    bytes.fromhex("83 B9 90 05 00 00 08 0F 9F C0 C3"),
    "Interface +0x590>8 根导航动画门",
)

INTERFACE_SPACE_CALL = 0x0044A57B

INTERFACE_CALLS = [
    (0x00435BBA, 0x00431380, "大类1 Event"),
    (0x00435BD2, 0x00431380, "大类2 Event"),
    (0x00435BEA, 0x00431380, "大类3 Event"),
    (0x00435C05, 0x00431380, "大类4 Event"),
    (0x00435C20, 0x00431380, "大类5 Event"),
    (0x00435C3B, 0x00431380, "大类6 Event"),
    (0x00435C56, 0x00431380, "大类7 Event"),
    (0x00435C81, 0x00431380, "大类8 Event"),
    (0x00435C9C, 0x00431380, "根层退出 Event"),
    (0x00435E93, 0x00431380, "角色 Event"),
]


# refactor20：state2“法宝/道具”页独立验证，不并入已实机通过的 r19 Shell。
# 0x4380D0 直接证明 +0x5E8/+0x5EC 的 +0x45 由原版按子类型动态启用；
# 因此 X 只需点击“当前原版可用”的特殊按钮，不需要插件硬编码中文子类型与内部 index 的对应关系。
INTERFACE_ITEMS_SIGNATURES = [
    (0x004380D0, bytes.fromhex(
        "83 FF 05 0F 94 C2 8B 88 E8 05 00 00 83 FF 05 88 51 45 74 09 "
        "83 FF 04 74 04 B0 01 EB 02 32 C0 8B 96 90 05 00 00 8B 8A EC 05 00 00 88 41 45"
    ), "state2 分发/弃置按钮原版启用逻辑"),
]

INTERFACE_ITEMS_CALLS = [
    (0x00438093, 0x00431380, "state2 子类型 Event"),
    (0x00438136, 0x00431380, "state2 特殊按钮A Event"),
    (0x0043817A, 0x00431380, "state2 特殊按钮B Event"),
    (0x0043820E, 0x00431380, "state2 上一页 Event"),
    (0x00438274, 0x00431380, "state2 下一页 Event"),
    (0x004382DB, 0x00431380, "state2 物品行 Event"),
]


# refactor28：state3 在既有列表/确认协议上补齐治疗法术“确认后鼠标选角色”。
# 0x43C19E 短锚点只确认 page+0x768 的原版目标选择门；另外两处 CALL 分别是角色左键和右键取消。
INTERFACE_SKILLS_SIGNATURES = [
    (0x0043C19E, bytes.fromhex("8A 86 68 07 00 00 84 C0 74 7E"), "state3 治疗目标 +0x768 active 门"),
]

INTERFACE_SKILLS_CALLS = [
    (0x0043C363, 0x00431380, "state3 子类型 Event"),
    (0x0043C3B6, 0x00431380, "state3 上一页 Event"),
    (0x0043C3E8, 0x00431380, "state3 下一页 Event"),
    (0x0043C461, 0x00431380, "state3 技能行 Event"),
    (0x0043C933, 0x00431380, "state3 治疗目标角色 Event"),
    (0x0043C1F7, 0x00431380, "state3 治疗目标取消 Event"),
]


# refactor22：state4“及身/装备”页独立协议。
# 与 state2/state3 一样，插件只 Hook 原版 ButtonEvent CALL；装备事务本体 0x42E160 绝不由插件直接调用。
INTERFACE_EQUIPMENT_SIGNATURES = [
    (0x0042DEF0, bytes.fromhex("53 55 56 8B F1 57 8B 86 80 05 00 00 8A 8E 79 05 00 00"), "state4 Update 入口与列表对象读取"),
]

INTERFACE_EQUIPMENT_CALLS = [
    (0x0042DF52, 0x00431380, "state4 上一页 Event"),
    (0x0042DFA6, 0x00431380, "state4 下一页 Event"),
    (0x0042DFF1, 0x00431380, "state4 候选行 Event"),
    (0x0042E072, 0x00431380, "state4 栏位0 Event"),
    (0x0042E09F, 0x00431380, "state4 栏位1 Event"),
    (0x0042E0CB, 0x00431380, "state4 栏位2 Event"),
]


# refactor23：state5“五内”页在 r22 原版 Event 基础上增加“视觉鼠标隔离”。
# 原版 0x440610 每个 Button 先调用 0x431400 取得鼠标按键边沿，再调用 0x431380 处理 ButtonEvent。
# 两个 CALL 都必须闭合，否则合成焦点鼠标可能重新污染真实按钮的鼠标历史状态。
INTERFACE_INNER_STATS_SIGNATURES = [
    (0x00440610, bytes.fromhex("51 56 8B F1 8B 86 7C 05 00 00 8A 8E 79 05 00 00"), "state5 Update 入口与按钮面板读取"),
]

INTERFACE_INNER_STATS_CALLS = [
    (0x0044066E, 0x00431400, "state5 11按钮统一鼠标边沿"),
    (0x004406A8, 0x00431380, "state5 11按钮统一 Event"),
]

# refactor26d：实体鼠标 click 的 release 函数 0x43E6A0(0) 会把 Sprite 送入 state5 循环动画，
# 并不等于恢复普通态。Button 构造函数 0x4312C0 在初始化 Sprite 后明确调用 0x43E6F0(0)，
# 因此本轮把 0x43E6F0(0) 认定为“原版 Button idle/static 基线”。
# 运行时和离线工具都验证这个入口的机器码，防止在其它 EXE 上误套 thiscall。
INTERFACE_INNER_STATS_SPRITE_SIGNATURES = [
    (0x0043E6F0, bytes.fromhex("56 8B F1 8A 46 08 84 C0"), "Button Sprite 原版反馈/idle入口"),
]

# refactor23：state6“阵形”页独立协议。
# 只允许接入 8 阵位真实 ButtonEvent；交换来源、目标和阵位映射仍由 RPG.exe 自己维护。
INTERFACE_FORMATION_SIGNATURES = [
    (0x0043F9A0, bytes.fromhex("51 56 8B F1 8B 86 7C 05 00 00 8A 8E 79 05 00 00"), "state6 Update 入口外层同步锚点"),
    (0x0043F9E4, bytes.fromhex("BD 9C 05 00 00 33 DB 81 ED F8 BF 46 00"), "state6 panel+0x59C 八阵位循环"),
]

INTERFACE_FORMATION_CALLS = [
    (0x0043FA05, 0x00431380, "state6 八阵位统一 Event"),
]

# refactor25：state6 直接复用原版 0x43F9A0 自己读取的 8 阵位布局表作为二维拓扑。
INTERFACE_FORMATION_LAYOUT_TABLES = [
    (0x0046BFF8, (227, 171, 115, 59, 283, 227, 171, 115), "state6 原版阵位 X 表"),
    (0x0046C018, (28, 56, 84, 112, 56, 84, 112, 142), "state6 原版阵位 Y 表"),
]


# 前 41 个 CALL 与 dev20 官方静态校验范围完全相同。
# 第 42 项是 refactor3 为修复战斗顶层动画期间 marker 被原版强制写回第1项而新增的唯一动画 CALL。
# 最后 4 项是 refactor4 为修复 CMD1/CMD2 从确认框/Target 返回时的列表视觉与默认鼠标 warp 而新增：
#   - 两个列表动画 CALL；
#   - 两个列表展开完成后调用 0x43DF30 的默认鼠标定位 CALL。
# 运行时启动前预检与离线工具使用同一范围，因此不会出现“离线 PASS、运行时却 patch 错地址”。
CALLS = [
    (0x0041C3C8, 0x00431310, "顶层六命令视觉 HitTest"),
    (0x0041821B, 0x00431310, "CMD1 行视觉 HitTest A"),
    (0x00419092, 0x00431310, "CMD1 行视觉 HitTest B"),
    (0x00419C35, 0x00431310, "CMD2 行视觉 HitTest"),
    (0x004273F7, 0x00431310, "确认框确定 HitTest"),
    (0x00427410, 0x00431310, "确认框取消 HitTest"),
    (0x00417829, 0x00431380, "CMD0 主按钮 Event"),
    (0x00418175, 0x00431380, "CMD1 行 Event"),
    (0x0041829C, 0x00431380, "CMD1 上一页 Event"),
    (0x004182E1, 0x00431380, "CMD1 下一页 Event"),
    (0x00418331, 0x00431380, "CMD1 大类 Event"),
    (0x00418393, 0x00431380, "CMD1 取消 Event"),
    (0x00419BA9, 0x00431380, "CMD2 行 Event"),
    (0x00419CA6, 0x00431380, "CMD2 大类 Event"),
    (0x00419D81, 0x00431380, "CMD2 取消 Event"),
    (0x00419FB2, 0x00431380, "CMD2 上一页 Event"),
    (0x00419FDB, 0x00431380, "CMD2 下一页 Event"),
    (0x0041BAA6, 0x00431380, "顶层命令 Event"),
    (0x0041C7A2, 0x00431380, "CMD3 Event"),
    (0x00427425, 0x00431380, "确认框确定 Event"),
    (0x00427446, 0x00431380, "确认框取消 Event"),
    (0x0044604F, 0x00446490, "Target 网格索引"),
    (0x004088BD, 0x00408950, "探索八方向移动 resolver"),
    (0x00409597, 0x00408A70, "探索 0x52 走跑协议"),
    (0x00408821, 0x0040A6D0, "探索 CURSOR.SCI 绘制"),
    (0x00412399, 0x00431380, "Result 外层继续 Event"),
    (0x00412F7F, 0x00431380, "Result 明细继续 Event"),
    (0x00446673, 0x0043DF30, "Target 原版鼠标定位 A"),
    (0x004466E1, 0x0043DF30, "Target 原版鼠标定位 B"),
    (0x004467B9, 0x0043DF30, "Target 原版鼠标定位 C"),
    (0x00447C84, 0x00431310, "标题按钮0 HitTest"),
    (0x00447C9A, 0x00431310, "标题按钮1 HitTest"),
    (0x00447CB0, 0x00431310, "标题按钮2 HitTest"),
    (0x00447D35, 0x00431380, "标题按钮0 Event"),
    (0x00447D7E, 0x00431380, "标题按钮1 Event"),
    (0x00447D97, 0x00431380, "标题按钮2 Event"),
    (0x00424BE2, 0x00424F00, "SaveSlot 游戏线程动画安全点"),
    (0x00424C90, 0x00431380, "SaveSlot 行 Event"),
    (0x00424E23, 0x00431380, "SaveSlot 取消 Event"),
    (0x00424E86, 0x00431380, "SaveSlot 上一页 Event"),
    (0x00424EB2, 0x00431380, "SaveSlot 下一页 Event"),
    (0x0041C3B2, 0x0041C470, "战斗顶层展开/收起动画（refactor3 新增）"),
    (0x00418126, 0x00418830, "CMD1 列表展开/收起动画（refactor4 新增）"),
    (0x00419B4A, 0x0041A590, "CMD2 列表展开/收起动画（refactor4 新增）"),
    (0x004188CF, 0x0043DF30, "CMD1 展开完成后的默认鼠标定位（refactor4 新增）"),
    (0x0041A65B, 0x0043DF30, "CMD2 展开完成后的默认鼠标定位（refactor4 新增）"),
]


# refactor37 重新定义鼠标控制契约：旧的常驻右杆、Back 慢速与 R3 复合点击已删除，
# 对外只保留 RT 完整鼠标和 LT 调查所需参数。这里锁住当前公开键，防止实现与样例 INI 漂移。
EXPECTED_INI_KEYS = {
    "DefaultHidden",
    "TargetSelectionCursor",
    "MouseModeLeftStickSensitivityPercent",
    "MouseModeRightStickSensitivityPercent",
    "RightStickSensitivityPercent",
    "SnapRadiusPixels",
    "StrengthPercent",
    "InvestigationHoverDurationMs",
    "ControllerModeDurationMs",
    "RunThresholdPercent",
    "RememberSelection",
    "EnableShortcuts",
    "VisualMinMs",
    "VisualHitCount",
    "SettleTimeoutMs",
    "RepeatInitialMs",
    "RepeatIntervalMs",
}


# refactor4 已由用户实机确认“截至 dev20 的重构功能可以封版”。
# refactor11 从 refactor7 稳定基线继续演进，并最终把普通对话两阶段 A 做到实机 PASS。
# refactor15 明确允许修改 Cursor，并保留 refactor14 已修改的 Battle、Frontend；除此之外下面这些稳定业务模块必须继续字节级不变。
# refactor20 封包时统一了文本换行为 CRLF。下列稳定文件若 SHA 与旧版不同，均已逐文件确认归一化换行后内容完全一致。
# 因此这里记录 r20 最终 CRLF 字节级 SHA，后续继续以它们作为稳定业务护栏。
SEALED_REFACTOR16_STABLE_SHA256 = {
    "battle.h": "7b6f915c3bfd3b62132327739494f01ad215af178c72a244551efc553154dda1",
    "frontend.c": "5c5bdfa00973f08f4f3c587c4c993607efb637e07aa5577eb2a73985950d2774",
    "frontend.h": "526c4743e27f7ace66e788f0aa98fe97e96cb3d89c0ebd3c96476dc419a96c0e",
    "movie_skip.c": "316c55482c7794340833016413d3e91b4557033061a0c6977d0f88588edf689a",
    "movie_skip.h": "903fb0d11b1c789222007ead0554e73b311a833bd6d8969f240167280f468337",
    "confirm_dialog.h": "095908cd8f97450e1cba2351ce463cb7ece9edbb4cf953a450289b7bcca7a867",
    "exploration.h": "b94766c2e917f1ff83ed68cb6268470d11791c8e68a5c20a8f2927ee27be07e6",
}

# refactor11 的普通对话两阶段 A 已由用户实机验收通过。
# refactor20 统一包内文本为 CRLF 后，这两个文件只发生换行字节变化；与旧封版归一化为 LF 后内容完全一致。
# 下面记录 r20 最终 CRLF 字节级 SHA，继续防止后续出现真实业务修改。
# SaveSlot 是已经封存并被多个界面复用的稳定控制器；本轮没有理由修改它。
# Battle.c 在 refactor22 为修复用户新实机确认的“禁用项 A 后当前菜单手柄锁死”而有意改变，
# 因此不再用旧 r20 SHA 阻止合法修复，而改由后面的事务结构护栏精确检查“只修什么”。
# refactor27 经用户明确批准让 state7 直接复用 SaveSlot，并补三项动作窗口。
# 因此 save_slot.c/.h 不再做整文件旧 SHA 锁；下方改用“原五处稳定 Hook 必须保留 + 新两处 Hook 独立安装”的结构护栏。
SEALED_REFACTOR20_STABLE_SAVE_SHA256 = {}

# 用户在 2026-08-20 明确确认 state3“法术/技能页内容完整通过”。
# refactor22 继续下一页时必须把这一页字节级冻结，防止“顺手重构”造成已验收页面回归。
SEALED_REFACTOR21_SKILLS_SHA256 = {
    "interface_skills.c": "19f0552cb2f47e9baa438e70b99975fe5adb950cf1595584c2638875b50c8c58",
    "interface_skills.h": "cc814ad4fda5c83223c9f56edc1f7f6af7b62d751c78637e6a1f69de4e73133f",
}

# state4 已被用户持续增强，refactor26 又新增 D-Pad 左右翻页，因此不能再用旧整文件 SHA 阻止合法输入增强。
# state4 的“不直接写装备数据/仍走真实 ButtonEvent”由后面的结构护栏继续机械验证。

# 用户在 refactor25 实机确认 Battle Target 已经回到新排序实验之前的旧成熟方案，并决定验收通过、不再修改。
# 因此 refactor25 起直接冻结 battle.c 整文件；若以后确实要改 Battle，必须先更新验收边界，不能顺手碰。
SEALED_REFACTOR24_BATTLE_SHA256 = {
    "battle.c": "05a6f7223bcbf4923c771fee45ddd339ed6022238ed8b08a8ca42c8d62ac1a97",
}

SEALED_REFACTOR11_DIALOGUE_SHA256 = {
    "dialogue_input.c": "a0c89357393a0e2082667b1a7db7a56714ff9c8e2d091b021c460ae23bbbd1d0",
    "dialogue_input.h": "6e848df4c366e74dc6c4aee30ddc1a5290396b926a3259217c322fe1a51811c1",
}

# 天书与共享 SaveSlot 继续逐字节冻结；Cursor 是 refactor37 明确批准改造的模块，
# 因此不再错误地把它锁回 refactor33。
SEALED_REFACTOR33_TOME_SCOPE_SHA256 = {
    "interface_tome.c": "713e659127ce84469213929b5e8094a2d19524d686fc29633ef6ac620bda0257",
    "interface_tome.h": "39790bf6b288695165e3dd0327e18895bf8e6651d5fe990ff7342663fac28618",
    "save_slot.c": "7a1453b4d1e5129af51baf185a04b15888ab5bbdfc2d3fc1e669a4886e003ed9",
    "save_slot.h": "7904d2bca6eb19ed971e2bae6abf7733f5310bf620e5e30c17b1208b70d77639",
}


class CheckResult:
    """集中记录 PASS/FAIL。这样主流程不会到处维护多个失败计数，输出也更统一。"""

    def __init__(self) -> None:
        self.passed = 0
        self.failed = 0

    def ok(self, name: str, detail: str = "") -> None:
        self.passed += 1
        suffix = f"：{detail}" if detail else ""
        print(f"PASS  {name}{suffix}")

    def fail(self, name: str, detail: str) -> None:
        self.failed += 1
        print(f"FAIL  {name}：{detail}")


def sha256(path: Path) -> str:
    """分块计算 SHA，避免把整个 EXE/ZIP 一次性读进内存。"""
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def parse_pe_sections(data: bytes):
    """
    读取最少量 PE 头信息，把虚拟地址 VA 映射回磁盘文件偏移。
    这里不依赖 pefile 等第三方库，保证任何装了 Python 的机器都能运行。
    """
    if len(data) < 0x100 or data[:2] != b"MZ":
        raise ValueError("不是有效 MZ 文件")
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise ValueError("PE 签名无效")
    section_count = struct.unpack_from("<H", data, pe_offset + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe_offset + 20)[0]
    section_table = pe_offset + 24 + optional_size

    sections = []
    for index in range(section_count):
        off = section_table + index * 40
        name = data[off:off + 8].split(b"\0", 1)[0].decode("ascii", "replace")
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", data, off + 8)
        sections.append((name, virtual_address, virtual_size, raw_offset, raw_size))
    return sections


def va_to_offset(va: int, sections) -> int:
    """把 ImageBase=0x00400000 的 VA 换算为文件偏移；找不到就明确报错，不猜。"""
    rva = va - IMAGE_BASE
    for _name, section_va, virtual_size, raw_offset, raw_size in sections:
        if section_va <= rva < section_va + max(virtual_size, raw_size):
            return raw_offset + (rva - section_va)
    raise ValueError(f"VA 0x{va:08X} 不属于任何 PE 节")


def check_target_exe(exe: Path, result: CheckResult) -> None:
    """验证用户给出的 RPG.exe 是否与本项目全部地址证据一致。"""
    if not exe.is_file():
        result.fail("目标 RPG.exe", f"文件不存在：{exe}")
        return

    digest = sha256(exe)
    if digest.lower() in SUPPORTED_SHA256:
        result.ok("RPG.exe SHA-256", f"{digest}（{SUPPORTED_SHA256[digest.lower()]}，双样本白名单PASS）")
    else:
        result.fail("RPG.exe SHA-256", f"实际 {digest}，不属于两个已验证可信样本：{sorted(SUPPORTED_SHA256)}")

    data = exe.read_bytes()
    try:
        sections = parse_pe_sections(data)
    except Exception as exc:  # 出错内容需要原样告诉使用者，方便定位错误文件。
        result.fail("RPG.exe PE 解析", str(exc))
        return

    byte_failures = 0
    for va, expected, label in BYTE_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                byte_failures += 1
                print(f"      字节不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
        except Exception as exc:
            byte_failures += 1
            print(f"      无法读取 0x{va:08X} {label}：{exc}")
    if byte_failures == 0:
        result.ok("RPG.exe 8 个关键字节协议", "8/8")
    else:
        result.fail("RPG.exe 8 个关键字节协议", f"失败 {byte_failures} 项")

    movie_failures = 0
    for va, expected, label in MOVIE_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                movie_failures += 1
                print(f"      电影协议不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
        except Exception as exc:
            movie_failures += 1
            print(f"      无法读取电影协议 0x{va:08X} {label}：{exc}")
    if movie_failures == 0:
        result.ok("RPG.exe Start=ESC 电影协议", "2/2")
    else:
        result.fail("RPG.exe Start=ESC 电影协议", f"失败 {movie_failures} 项")

    dialogue_failures = 0
    for va, expected, label in DIALOGUE_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                dialogue_failures += 1
                print(f"      对话协议不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
        except Exception as exc:
            dialogue_failures += 1
            print(f"      无法读取对话协议 0x{va:08X} {label}：{exc}")
    if dialogue_failures == 0:
        result.ok("RPG.exe 公共对话推进协议", "4/4")
    else:
        result.fail("RPG.exe 公共对话推进协议", f"失败 {dialogue_failures} 项")

    confirm_failures = 0
    for slot_va, expected_target, label in CONFIRM_VTABLE_SLOTS:
        try:
            off = va_to_offset(slot_va, sections)
            actual_target = struct.unpack_from("<I", data, off)[0]
            if actual_target != expected_target:
                confirm_failures += 1
                print(f"      询问框 vtable 槽不一致 0x{slot_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            confirm_failures += 1
            print(f"      无法读取询问框 vtable 槽 0x{slot_va:08X} {label}：{exc}")

    for call_va, expected_target, label in CONFIRM_DIALOG_CALLS:
        try:
            off = va_to_offset(call_va, sections)
            if data[off] != 0xE8:
                confirm_failures += 1
                print(f"      询问框 CALL 首字节错误 0x{call_va:08X} {label}：0x{data[off]:02X}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual_target = (call_va + 5 + rel) & 0xFFFFFFFF
            if actual_target != expected_target:
                confirm_failures += 1
                print(f"      询问框 CALL 不一致 0x{call_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            confirm_failures += 1
            print(f"      无法读取询问框 CALL 0x{call_va:08X} {label}：{exc}")
    if confirm_failures == 0:
        result.ok("RPG.exe 通用询问框 vtable/第二变体协议", "2/2 vtable + 4/4 CALL")
    else:
        result.fail("RPG.exe 通用询问框 vtable/第二变体协议", f"失败 {confirm_failures} 项")

    call_failures = 0
    for call_va, expected_target, label in CALLS:
        try:
            off = va_to_offset(call_va, sections)
            if data[off] != 0xE8:
                call_failures += 1
                print(f"      不是 CALL 0x{call_va:08X} {label}：首字节=0x{data[off]:02X}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual_target = (call_va + 5 + rel) & 0xFFFFFFFF
            if actual_target != expected_target:
                call_failures += 1
                print(f"      CALL 不一致 0x{call_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            call_failures += 1
            print(f"      无法读取 CALL 0x{call_va:08X} {label}：{exc}")
    if call_failures == 0:
        result.ok("RPG.exe 关键 CALL", "dev20 41/41 + refactor3 顶层动画 1/1 + refactor4 CMD1/CMD2 阶段切换 4/4")
    else:
        result.fail("RPG.exe 关键 CALL", f"46 项中失败 {call_failures} 项")

    interface_failures = 0

    # 1) Space 业务链按 mask 验证：条件跳转位移可以变化，真正业务字节必须一致。
    try:
        va, expected, mask, label = INTERFACE_SPACE_FEATURE
        off = va_to_offset(va, sections)
        actual = data[off:off + len(expected)]
        if len(actual) != len(expected) or any(m and a != e for a, e, m in zip(actual, expected, mask)):
            interface_failures += 1
            print(f"      主Interface特征不匹配 0x{va:08X} {label}：{actual.hex(' ').upper()}")
    except Exception as exc:
        interface_failures += 1
        print(f"      无法读取主Interface Space 特征：{exc}")

    # 2) Space 的 CALL 只要求仍是有效 E8 rel32；不要求目标必须固定为 0x40B230。
    #    如果其它补丁把它改到兼容 wrapper，运行时会跟随该当前目标。
    try:
        off = va_to_offset(INTERFACE_SPACE_CALL, sections)
        if data[off] != 0xE8:
            interface_failures += 1
            print(f"      原版 Space 地图业务 CALL 不再是 E8：0x{data[off]:02X}")
        else:
            rel = struct.unpack_from("<i", data, off + 1)[0]
            dynamic_target = (INTERFACE_SPACE_CALL + 5 + rel) & 0xFFFFFFFF
            if not (0x00010000 <= dynamic_target < 0x7FFF0000):
                interface_failures += 1
                print(f"      原版 Space 地图业务 CALL 目标无效：0x{dynamic_target:08X}")
            else:
                print(f"      [信息] 原版 Space 当前地图业务 CALL 目标：0x{dynamic_target:08X}")
    except Exception as exc:
        interface_failures += 1
        print(f"      无法解析原版 Space 地图业务 CALL：{exc}")

    # 3) Interface 根动画门仍是插件真实读取的短结构，继续精确验证。
    try:
        va, expected, label = INTERFACE_READY_SIGNATURE
        off = va_to_offset(va, sections)
        actual = data[off:off + len(expected)]
        if actual != expected:
            interface_failures += 1
            print(f"      主Interface短锚点不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
    except Exception as exc:
        interface_failures += 1
        print(f"      无法读取主Interface根动画门：{exc}")

    # 4) Shell 真正改写的十个 ButtonEvent CALL 仍要求精确匹配。
    for call_va, expected_target, label in INTERFACE_CALLS:
        try:
            off = va_to_offset(call_va, sections)
            if data[off] != 0xE8:
                interface_failures += 1
                print(f"      主Interface CALL 首字节错误 0x{call_va:08X} {label}：0x{data[off]:02X}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual_target = (call_va + 5 + rel) & 0xFFFFFFFF
            if actual_target != expected_target:
                interface_failures += 1
                print(f"      主Interface CALL 不一致 0x{call_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            interface_failures += 1
            print(f"      无法读取主Interface CALL 0x{call_va:08X} {label}：{exc}")
    if interface_failures == 0:
        result.ok("RPG.exe 主Interface兼容协议", "Space业务特征 + 动态CALL目标 + 1/1短锚点 + 10/10 ButtonEvent CALL")
    else:
        result.fail("RPG.exe 主Interface兼容协议", f"失败 {interface_failures} 项")


    items_failures = 0
    for va, expected, label in INTERFACE_ITEMS_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                items_failures += 1
                print(f"      法宝页字节不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
        except Exception as exc:
            items_failures += 1
            print(f"      无法读取法宝页字节 0x{va:08X} {label}：{exc}")
    for call_va, expected_target, label in INTERFACE_ITEMS_CALLS:
        try:
            off = va_to_offset(call_va, sections)
            if data[off] != 0xE8:
                items_failures += 1
                print(f"      法宝页 CALL 首字节错误 0x{call_va:08X} {label}：0x{data[off]:02X}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual_target = (call_va + 5 + rel) & 0xFFFFFFFF
            if actual_target != expected_target:
                items_failures += 1
                print(f"      法宝页 CALL 不一致 0x{call_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            items_failures += 1
            print(f"      无法读取法宝页 CALL 0x{call_va:08X} {label}：{exc}")
    if items_failures == 0:
        result.ok("RPG.exe 法宝/道具页协议", "特殊按钮机器码 1/1 + ButtonEvent 6/6")
    else:
        result.fail("RPG.exe 法宝/道具页协议", f"失败 {items_failures} 项")


    skills_failures = 0
    for va, expected, label in INTERFACE_SKILLS_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                skills_failures += 1
                print(f"      绝学页字节锚点不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
        except Exception as exc:
            skills_failures += 1
            print(f"      无法读取绝学页锚点 0x{va:08X} {label}：{exc}")

    for call_va, expected_target, label in INTERFACE_SKILLS_CALLS:
        try:
            off = va_to_offset(call_va, sections)
            if data[off] != 0xE8:
                skills_failures += 1
                print(f"      绝学页 CALL 首字节错误 0x{call_va:08X} {label}：0x{data[off]:02X}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual_target = (call_va + 5 + rel) & 0xFFFFFFFF
            if actual_target != expected_target:
                skills_failures += 1
                print(f"      绝学页 CALL 不一致 0x{call_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            skills_failures += 1
            print(f"      无法读取绝学页 CALL 0x{call_va:08X} {label}：{exc}")
    if skills_failures == 0:
        result.ok("RPG.exe 绝学/法术页协议", "治疗目标状态锚点1/1 + ButtonEvent 6/6")
    else:
        result.fail("RPG.exe 绝学/法术页协议", f"失败 {skills_failures} 项")


    equipment_failures = 0
    for va, expected, label in INTERFACE_EQUIPMENT_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                equipment_failures += 1
                print(f"      及身页字节不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
        except Exception as exc:
            equipment_failures += 1
            print(f"      无法读取及身页字节 0x{va:08X} {label}：{exc}")
    for call_va, expected_target, label in INTERFACE_EQUIPMENT_CALLS:
        try:
            off = va_to_offset(call_va, sections)
            if data[off] != 0xE8:
                equipment_failures += 1
                print(f"      及身页 CALL 首字节错误 0x{call_va:08X} {label}：0x{data[off]:02X}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual_target = (call_va + 5 + rel) & 0xFFFFFFFF
            if actual_target != expected_target:
                equipment_failures += 1
                print(f"      及身页 CALL 不一致 0x{call_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            equipment_failures += 1
            print(f"      无法读取及身页 CALL 0x{call_va:08X} {label}：{exc}")
    if equipment_failures == 0:
        result.ok("RPG.exe 及身/装备页协议", "Update短锚点 1/1 + ButtonEvent 6/6")
    else:
        result.fail("RPG.exe 及身/装备页协议", f"失败 {equipment_failures} 项")


    inner_stats_failures = 0
    for va, expected, label in INTERFACE_INNER_STATS_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                inner_stats_failures += 1
                print(f"      五内页字节不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
        except Exception as exc:
            inner_stats_failures += 1
            print(f"      无法读取五内页字节 0x{va:08X} {label}：{exc}")
    for call_va, expected_target, label in INTERFACE_INNER_STATS_CALLS:
        try:
            off = va_to_offset(call_va, sections)
            if data[off] != 0xE8:
                inner_stats_failures += 1
                print(f"      五内页 CALL 首字节错误 0x{call_va:08X} {label}：0x{data[off]:02X}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual_target = (call_va + 5 + rel) & 0xFFFFFFFF
            if actual_target != expected_target:
                inner_stats_failures += 1
                print(f"      五内页 CALL 不一致 0x{call_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            inner_stats_failures += 1
            print(f"      无法读取五内页 CALL 0x{call_va:08X} {label}：{exc}")
    for va, expected, label in INTERFACE_INNER_STATS_SPRITE_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                inner_stats_failures += 1
                print(f"      五内页 Sprite 入口不一致 0x{va:08X} {label}：实际 {actual.hex(' ').upper()}，预期 {expected.hex(' ').upper()}")
        except Exception as exc:
            inner_stats_failures += 1
            print(f"      无法读取五内页 Sprite 入口 0x{va:08X} {label}：{exc}")

    if inner_stats_failures == 0:
        result.ok("RPG.exe 五内页协议", "Update短锚点 1/1 + 鼠标边沿/ButtonEvent 2/2 + Button Sprite idle入口 1/1")
    else:
        result.fail("RPG.exe 五内页协议", f"失败 {inner_stats_failures} 项")

    formation_failures = 0
    for va, expected, label in INTERFACE_FORMATION_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            actual = data[off:off + len(expected)]
            if actual != expected:
                formation_failures += 1
                print(f"      阵形页字节不一致 0x{va:08X} {label}：{actual.hex(' ').upper()}")
        except Exception as exc:
            formation_failures += 1
            print(f"      无法读取阵形页字节 0x{va:08X} {label}：{exc}")
    for call_va, expected_target, label in INTERFACE_FORMATION_CALLS:
        try:
            off = va_to_offset(call_va, sections)
            if data[off] != 0xE8:
                formation_failures += 1
                print(f"      阵形页 CALL 首字节错误 0x{call_va:08X} {label}：0x{data[off]:02X}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual_target = (call_va + 5 + rel) & 0xFFFFFFFF
            if actual_target != expected_target:
                formation_failures += 1
                print(f"      阵形页 CALL 不一致 0x{call_va:08X} {label}：实际=0x{actual_target:08X} 预期=0x{expected_target:08X}")
        except Exception as exc:
            formation_failures += 1
            print(f"      无法读取阵形页 CALL 0x{call_va:08X} {label}：{exc}")
    for table_va, expected_values, label in INTERFACE_FORMATION_LAYOUT_TABLES:
        try:
            off = va_to_offset(table_va, sections)
            actual_values = struct.unpack_from("<8i", data, off)
            if actual_values != expected_values:
                formation_failures += 1
                print(f"      阵形页布局表不一致 0x{table_va:08X} {label}：实际={actual_values} 预期={expected_values}")
        except Exception as exc:
            formation_failures += 1
            print(f"      无法读取阵形页布局表 0x{table_va:08X} {label}：{exc}")
    if formation_failures == 0:
        result.ok("RPG.exe 阵形页协议", "Update/八阵位循环锚点 2/2 + ButtonEvent 1/1 + 原版布局表 2/2")
    else:
        result.fail("RPG.exe 阵形页协议", f"失败 {formation_failures} 项")


    tome_failures = 0
    for va, expected, label in INTERFACE_TOME_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            if data[off:off + len(expected)] != expected:
                tome_failures += 1
                print(f"      天书页字节签名不一致 0x{va:08X} {label}")
        except Exception as exc:
            tome_failures += 1
            print(f"      无法读取天书页签名 0x{va:08X} {label}：{exc}")
    for va, target, label in INTERFACE_TOME_CALLS:
        try:
            off = va_to_offset(va, sections)
            if data[off] != 0xE8:
                tome_failures += 1
                print(f"      天书页 CALL opcode 不一致 0x{va:08X} {label}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual = va + 5 + rel
            if actual != target:
                tome_failures += 1
                print(f"      天书页 CALL 目标不一致 0x{va:08X} {label}：0x{actual:08X} != 0x{target:08X}")
        except Exception as exc:
            tome_failures += 1
            print(f"      无法读取天书页 CALL 0x{va:08X} {label}：{exc}")
    if tome_failures == 0:
        result.ok("RPG.exe 天书页协议", "state7 SaveSlot构造1/1 + SaveAction短锚点1/1 + Hit/Event 2/2")
    else:
        result.fail("RPG.exe 天书页协议", f"失败 {tome_failures} 项")

    options_failures = 0
    for va, expected, label in INTERFACE_OPTIONS_SIGNATURES:
        try:
            off = va_to_offset(va, sections)
            if data[off:off + len(expected)] != expected:
                options_failures += 1
                print(f"      机能页字节签名不一致 0x{va:08X} {label}")
        except Exception as exc:
            options_failures += 1
            print(f"      无法读取机能页签名 0x{va:08X} {label}：{exc}")
    for va, target, label in INTERFACE_OPTIONS_CALLS:
        try:
            off = va_to_offset(va, sections)
            if data[off] != 0xE8:
                options_failures += 1
                print(f"      机能页 CALL opcode 不一致 0x{va:08X} {label}")
                continue
            rel = struct.unpack_from("<i", data, off + 1)[0]
            actual = va + 5 + rel
            if actual != target:
                options_failures += 1
                print(f"      机能页 CALL 目标不一致 0x{va:08X} {label}：0x{actual:08X} != 0x{target:08X}")
        except Exception as exc:
            options_failures += 1
            print(f"      无法读取机能页 CALL 0x{va:08X} {label}：{exc}")
    if options_failures == 0:
        result.ok("RPG.exe 机能页协议", "state8构造1/1 + Update短锚点1/1 + ButtonEvent 5/5")
    else:
        result.fail("RPG.exe 机能页协议", f"失败 {options_failures} 项")

    def check_simple_group(label, signatures, calls, ok_detail):
        failures = 0
        for va, expected, item_label in signatures:
            try:
                off = va_to_offset(va, sections)
                actual = data[off:off + len(expected)]
                if actual != expected:
                    failures += 1
                    print(f"      {label}字节不一致 0x{va:08X} {item_label}：{actual.hex(' ').upper()}")
            except Exception as exc:
                failures += 1
                print(f"      无法读取{label}字节 0x{va:08X} {item_label}：{exc}")
        for va, target, item_label in calls:
            try:
                off = va_to_offset(va, sections)
                if data[off] != 0xE8:
                    failures += 1
                    print(f"      {label} CALL opcode 不一致 0x{va:08X} {item_label}")
                    continue
                rel = struct.unpack_from("<i", data, off + 1)[0]
                actual = (va + 5 + rel) & 0xFFFFFFFF
                if actual != target:
                    failures += 1
                    print(f"      {label} CALL 目标不一致 0x{va:08X} {item_label}：0x{actual:08X} != 0x{target:08X}")
            except Exception as exc:
                failures += 1
                print(f"      无法读取{label} CALL 0x{va:08X} {item_label}：{exc}")
        if failures == 0:
            result.ok(f"RPG.exe {label}协议", ok_detail)
        else:
            result.fail(f"RPG.exe {label}协议", f"失败 {failures} 项")

    check_simple_group("客栈根菜单", INN_SIGNATURES, INN_CALLS, "Update锚点1/1 + Hit/Event 2/2")
    check_simple_group("炼化两层菜单", SYNTHESIS_SIGNATURES, SYNTHESIS_CALLS, "Ctor/Update锚点2/2 + ButtonEvent 8/8")
    check_simple_group("统一商店主体/数量窗", SHOP_CORE_SIGNATURES, SHOP_CORE_CALLS, "主类/数量窗锚点4/4 + ButtonEvent 11/11")
    check_simple_group("商店Y道具信息可选能力", SHOP_INFO_SIGNATURES, SHOP_INFO_CALLS, "信息窗锚点1/1 + ButtonEvent 1/1 + transition 2/2")
    check_simple_group("剧情mode=3两项选择", SCENE_CHOICE_SIGNATURES, SCENE_CHOICE_CALLS, "分发/选择器锚点2/2 + 原版CALL 3/3")
    check_simple_group("存档点独立包装层", SAVE_POINT_SIGNATURES, SAVE_POINT_CALLS, "包装构造1/1 + SaveSlot构造1/1 + 发布/绑定/Update锚点3/3")
    check_simple_group("探索调查原版resolver", INVESTIGATION_SIGNATURES, INVESTIGATION_CALLS, "结构/类型/距离锚点6/6 + resolver CALL 1/1")



# refactor27：state7 天书与 state8 机能只新增自己的独立协议，不扩大 r19 Shell / 标题 SaveSlot 的硬门槛。
INTERFACE_TOME_SIGNATURES = [
    (0x004262C0, bytes.fromhex("53 55 56 8B F1 B9 03 00 00 00 8D AE 8C 05 00 00"), "state7 SaveAction 三项窗口 Update"),
]
INTERFACE_TOME_CALLS = [
    (0x00434E93, 0x004245B0, "state7 -> SaveSlot ctor"),
    (0x00426365, 0x00431310, "SaveAction HitTest"),
    (0x00426387, 0x00431380, "SaveAction ButtonEvent"),
]

# refactor31：地图事件 opcode 0x3E 创建的是独立包装层；包装层 +0x580 才是标准 SaveSlot。
# 这些锚点同时约束包装层发布、子对象绑定和 Update 生命周期，防止再误判成 Interface state7。
SAVE_POINT_SIGNATURES = [
    (0x0040DB32, bytes.fromhex("A3 D0 FC 89 00 C7 05 18 F8 89 00 02 00 00 00"), "地图事件发布存档点包装层"),
    (0x00413C48, bytes.fromhex("89 86 80 05 00 00 C6 80 79 05 00 00 00 8B 86 80 05 00 00"), "包装层绑定 +0x580 SaveSlot"),
    (0x00413D89, bytes.fromhex("8B 86 80 05 00 00 8A 88 B8 05 00 00 84 C9 74 33"), "包装层 Update 读取 SaveSlot 完成态"),
]
SAVE_POINT_CALLS = [
    (0x0040DB2D, 0x00413B00, "地图事件 -> 存档点包装层 ctor"),
    (0x00413C3F, 0x004245B0, "存档点包装层 -> 标准 SaveSlot ctor"),
]
INTERFACE_OPTIONS_SIGNATURES = [
    (0x00428950, bytes.fromhex("53 56 8B F1 32 DB 8B 8E A8 05 00 00 8A 81 79 05 00 00"), "state8 机能 Update"),
]
INTERFACE_OPTIONS_CALLS = [
    (0x00434EC9, 0x00428510, "state8 -> Options ctor"),
    (0x0042898C, 0x00431380, "音乐减少 Event"),
    (0x004289BF, 0x00431380, "音乐增加 Event"),
    (0x004289F3, 0x00431380, "音效减少 Event"),
    (0x00428A26, 0x00431380, "音效增加 Event"),
    (0x00428A5A, 0x00431380, "空明流转 Event"),
]

# refactor29：客栈、炼化与公共剧情 mode=3 两项选择各自独立，不扩大旧稳定模块硬门槛。
INN_SIGNATURES = [
    (0x00413850, bytes.fromhex("6A FF 68 B7 D6 45 00 64 A1 00 00 00 00 50 64 89"), "客栈三项根菜单 Update"),
]
INN_CALLS = [
    (0x00413909, 0x00431310, "客栈三项 HitTest"),
    (0x00413933, 0x00431380, "客栈三项 Event"),
]
SYNTHESIS_SIGNATURES = [
    (0x0040F6D0, bytes.fromhex("6A FF 68 C0 D2 45 00 64 A1 00 00 00 00 50 64 89"), "炼化 ctor"),
    (0x0040FBE0, bytes.fromhex("64 A1 00 00 00 00 6A FF 68 FB D2 45 00 50 64 89"), "炼化 Update"),
]
SYNTHESIS_CALLS = [
    (0x0040FE04, 0x00431380, "炼化顶部类别/退出 Event"),
    (0x0040FF2A, 0x00431380, "炼化第一层上一页"),
    (0x0040FF94, 0x00431380, "炼化第一层下一页"),
    (0x0040FFF7, 0x00431380, "炼化第二层上一页"),
    (0x00410061, 0x00431380, "炼化第二层下一页"),
    (0x004100E4, 0x00431380, "炼化第二层取消"),
    (0x0041011B, 0x00431380, "炼化第二层物品行"),
    (0x00410212, 0x00431380, "炼化第一层物品行"),
]

# refactor36：主体继续使用 refactor33 已由用户确认覆盖全部商店的协议；
# Y 信息窗复用已安装的公共 Event 桥，列表补齐上下越界连续分页。
SHOP_CORE_SIGNATURES = [
    (0x00413FA0, bytes.fromhex("6A FF 68 B1 D7 45 00 64 A1 00 00 00 00 50 64 89"), "统一商店 ctor"),
    (0x004145B0, bytes.fromhex("83 EC 1C 53 55 56 8B F1 83 CB FF 57 8B 86 9C 05 00 00"), "统一商店 Update"),
    (0x00427920, bytes.fromhex("6A FF 68 DB E6 45 00 64 A1 00 00 00 00 50 64 89"), "买卖数量窗 ctor"),
    (0x00427E80, bytes.fromhex("51 55 56 8B F1 33 ED 57 8B 86 7C 05 00 00 3B C5"), "买卖数量窗 Update"),
]
SHOP_CORE_CALLS = [
    (0x0041479F, 0x00431380, "顶部类别/退出 Event"),
    (0x0041481F, 0x00431380, "右侧卖出列上一页"),
    (0x00414887, 0x00431380, "右侧卖出列下一页"),
    (0x0041493E, 0x00431380, "左侧买入列上一页"),
    (0x004149A9, 0x00431380, "左侧买入列下一页"),
    (0x00414A65, 0x00431380, "右侧卖出列物品行"),
    (0x00414CC6, 0x00431380, "左侧买入列物品行"),
    (0x00428016, 0x00431380, "数量窗确认"),
    (0x00428037, 0x00431380, "数量窗取消"),
    (0x004280DA, 0x00431380, "数量增加"),
    (0x0042813F, 0x00431380, "数量减少"),
]
SHOP_INFO_SIGNATURES = [
    (0x00417810, bytes.fromhex("56 8B F1 8B 86 8C 05 00 00 8B 8E 6C 05 00 00 3B C1"), "道具信息窗 Update"),
]
SHOP_INFO_CALLS = [
    (0x00417829, 0x00431380, "道具信息关闭 Event"),
    (0x00414CB3, 0x004178A0, "右侧卖出列道具信息 transition"),
    (0x00415102, 0x004178A0, "左侧买入列道具信息 transition"),
]
SCENE_CHOICE_SIGNATURES = [
    (0x00403E5A, bytes.fromhex("A1 40 F6 46 00 83 F8 03 77 21 FF 24 85 8C 3E 40 00"), "公共消息 mode 分发器"),
    (0x00404600, bytes.fromhex("83 EC 08 8D 44 24 00 50 FF 15 04 02 46 00"), "mode=3 鼠标选择器"),
]
SCENE_CHOICE_CALLS = [
    (0x00403E80, 0x00404600, "mode=3 -> 0x404600"),
    (0x00404697, 0x0044B0B0, "第二项/结果2提交"),
    (0x004046AF, 0x0044B0B0, "第一项/结果1提交"),
]

# refactor37：调查模块只在这一条原版 resolver CALL 上旁路观察。
# 六个锚点分别保护入口、scene 数组、interaction type、距离分类、距离计算与 100.0 常量；
# 任何一项不一致都只关闭 LT 能力，不影响 r36 已验收底座。
INVESTIGATION_SIGNATURES = [
    (0x004098B8, bytes.fromhex("A1 08 F8 89 00 85 C0 0F 87 77 01 00 00 8B 46 20 8B CE 50"), "地图 Update 调用 resolver 前门"),
    (0x00409B50, bytes.fromhex("53 55 8B 6C 24 0C 56 8B F1 57 8D 7D FF 8B 46 1C 8B 4E 2C"), "resolver 入口及 scene +0x1C/+0x2C"),
    (0x00409B6E, bytes.fromhex("8B 46 30 8B 0C B8 8B 51 70 8A 42 73 84 C0 74 22"), "排序数组 +0x30 与 record +0x73 类型"),
    (0x00409903, bytes.fromhex("83 F8 05 55 7D 50 8B 56 1C 8B 46 2C 8B 4E 30 8B 14 90"), "type>=5 绕过距离门"),
    (0x00409918, bytes.fromhex("8B 6A 10 8B 41 10 8B 49 14 2B C5 8B 6A 14 2B CD 8B D1 0F AF D1 8B C8 0F AF C8 03 D1"), "对象/主角坐标平方距离"),
    (0x00460290, bytes.fromhex("00 00 00 00 00 00 59 40"), "原版互动距离 100.0"),
]
INVESTIGATION_CALLS = [
    (0x004098CB, 0x00409B50, "地图 Update -> 可互动目标 resolver"),
]

def read_utf8(path: Path) -> str:
    """源码可能带或不带 BOM，utf-8-sig 两种都能正常读取。"""
    return path.read_text(encoding="utf-8-sig")


def check_source_architecture(root: Path, result: CheckResult) -> None:
    """检查“新架构”不是只停留在文档里的口号。"""
    src = root / "源码"
    required = [
        "platform.h", "game_addresses.h", "runtime.h", "runtime.c",
        "pad_input.h", "pad_input.c", "input_router.h", "input_router.c",
        "movie_skip.h", "movie_skip.c",
        "confirm_dialog.h", "confirm_dialog.c", "dialogue_input.h", "dialogue_input.c",
        "cursor.h", "cursor.c", "exploration.h", "exploration.c", "investigation.h", "investigation.c",
        "inn.h", "inn.c", "synthesis.h", "synthesis.c", "shop.h", "shop.c", "scene_choice.h", "scene_choice.c",
        "ui_bridge.h", "ui_bridge.c", "interface_shell.h", "interface_shell.c", "interface_items.h", "interface_items.c", "interface_skills.h", "interface_skills.c", "interface_equipment.h", "interface_equipment.c", "interface_inner_stats.h", "interface_inner_stats.c",
        "spatial_neighbor.h", "spatial_neighbor.c", "interface_formation.h", "interface_formation.c",
        "interface_tome.h", "interface_tome.c", "interface_options.h", "interface_options.c",
        "save_slot.h", "save_slot.c", "save_point.h", "save_point.c",
        "frontend.h", "frontend.c", "battle.h", "battle.c", "plugin.c", "build.bat",
    ]
    missing = [name for name in required if not (src / name).is_file()]
    if missing:
        result.fail("重构源码文件集合", "缺少：" + ", ".join(missing))
        return
    result.ok("重构源码文件集合", f"{len(required)} 个要求文件均存在")

    # 所有代码文件名必须是英文/ASCII；这是项目长期规范，而不是本轮临时偏好。
    bad_names = []
    for path in list(src.glob("*.[ch]")) + [src / "build.bat"]:
        try:
            path.name.encode("ascii")
        except UnicodeEncodeError:
            bad_names.append(path.name)
    if bad_names:
        result.fail("代码文件名英文/ASCII", ", ".join(bad_names))
    else:
        result.ok("代码文件名英文/ASCII")

    business_files = [
        src / "battle.c", src / "frontend.c", src / "save_slot.c", src / "save_point.c", src / "ui_bridge.c",
        src / "confirm_dialog.c", src / "dialogue_input.c", src / "inn.c", src / "synthesis.c", src / "shop.c", src / "scene_choice.c", src / "interface_items.c", src / "interface_skills.c", src / "interface_equipment.c", src / "interface_inner_stats.c", src / "interface_formation.c", src / "interface_tome.c", src / "interface_options.c",
    ]
    raw_hits = []
    for path in business_files:
        text = read_utf8(path)
        if re.search(r"\bPAD_[A-Z0-9_]+\b|\bPadInput_", text):
            raw_hits.append(path.name)
    if raw_hits:
        result.fail("业务层隔离物理输入", "这些模块仍直接读取物理键：" + ", ".join(raw_hits))
    else:
        result.ok("业务层隔离物理输入", "Battle/Frontend/SaveSlot/SavePoint/UIBridge/ConfirmDialog/Dialogue/Interface state2~8 只使用语义层")

    router = read_utf8(src / "input_router.h") + read_utf8(src / "input_router.c")
    required_tokens = [
        "INPUT_PASS", "INPUT_MERGE", "INPUT_OVERRIDE", "INPUT_CONSUME",
        "INPUT_LAYER_COMMON", "INPUT_LAYER_OVERLAY", "InputRouter_ChordPressed",
        "INPUT_CTX_DIALOGUE", "INPUT_CTX_SCENE_CHOICE", "INPUT_CTX_CONFIRM_DIALOG", "INPUT_CTX_INTERFACE_SHELL", "INPUT_CTX_INTERFACE_ITEMS", "INPUT_CTX_INTERFACE_SKILLS", "INPUT_CTX_INTERFACE_EQUIPMENT", "INPUT_CTX_INTERFACE_INNER_STATS", "INPUT_CTX_INTERFACE_FORMATION", "INPUT_CTX_SAVE_ACTION", "INPUT_CTX_INTERFACE_OPTIONS", "INPUT_CTX_INN_ROOT", "INPUT_CTX_SYNTHESIS_PRIMARY", "INPUT_CTX_SYNTHESIS_SECONDARY", "INPUT_CTX_SHOP_ROOT", "INPUT_CTX_SHOP_QUANTITY",
        "INPUT_SUBTYPE_PREV", "INPUT_SUBTYPE_NEXT",
        "InputRouter_BeginFrame", "InputRouter_Consume",
    ]
    absent = [token for token in required_tokens if token not in router]
    if absent:
        result.fail("输入策略四态与双通道", "缺少：" + ", ".join(absent))
    else:
        result.ok("输入策略四态与双通道", "四态 + COMMON/OVERLAY + Dialogue/Confirm Context + tick 内输入消费")

    # refactor26a 起：机械交叉检查“策略表声明的所有权”与业务实际读取的 Layer。
    #
    # 这条检查来自真实回归：refactor26 把 state2 的 D-Pad 左右从 PASS 改成 OVERRIDE，
    # 以便法宝页直接翻页；但弃置确认弹窗仍从 COMMON 读取左右。按照 Router 规则，
    # OVERRIDE 只进入 OVERLAY，因此那两行代码在运行时永远不可达，最终表现为弹窗左右失效。
    #
    # 以后任何 Context 再发生类似改键，只要业务还读旧 Layer，这里就直接 FAIL，
    # 不允许“策略表看起来正确、实机却丢功能”再次漏到交付阶段。
    action_enum = re.search(r"typedef enum InputAction\s*\{(.*?)INPUT_ACTION_COUNT", read_utf8(src / "input_router.h"), flags=re.S)
    action_names = []
    if action_enum:
        for raw_line in action_enum.group(1).splitlines():
            line = re.sub(r"/\*.*?\*/", "", raw_line).split("//", 1)[0].strip().rstrip(",")
            if not line:
                continue
            name = line.split("=", 1)[0].strip().rstrip(",")
            if name.startswith("INPUT_"):
                action_names.append(name)

    policy_modes = {}
    for match in re.finditer(r"static const InputPolicy\s+(g_policy_\w+)\s*=\s*\{\{(.*?)\}\};", read_utf8(src / "input_router.c"), flags=re.S):
        modes = re.findall(r"INPUT_(?:PASS|MERGE|OVERRIDE|CONSUME)", match.group(2))
        policy_modes[match.group(1)] = dict(zip(action_names, modes))

    context_to_policy = {"INPUT_CTX_NONE": "g_policy_none"}
    for match in re.finditer(r"case\s+(INPUT_CTX_[A-Z0-9_]+)\s*:\s*return\s*&?(g_policy_\w+)\s*;", read_utf8(src / "input_router.c")):
        context_to_policy[match.group(1)] = match.group(2)

    layer_mismatches = []
    route_call = re.compile(
        r"InputRouter_(?:PressedOn|DownOn|ReleasedOn)\s*\(\s*"
        r"(INPUT_CTX_[A-Z0-9_]+)\s*,\s*(INPUT_[A-Z0-9_]+)\s*,\s*"
        r"(INPUT_LAYER_COMMON|INPUT_LAYER_OVERLAY)"
    )
    for source_path in sorted(src.glob("*.c")):
        source_text = read_utf8(source_path)
        for match in route_call.finditer(source_text):
            context, action, layer = match.groups()
            policy_name = context_to_policy.get(context)
            mode = policy_modes.get(policy_name, {}).get(action)
            allowed = (
                mode == "INPUT_MERGE" or
                (mode == "INPUT_PASS" and layer == "INPUT_LAYER_COMMON") or
                (mode == "INPUT_OVERRIDE" and layer == "INPUT_LAYER_OVERLAY")
            )
            if not allowed:
                line_no = source_text.count("\n", 0, match.start()) + 1
                layer_mismatches.append(f"{source_path.name}:{line_no} {context}/{action}/{layer} policy={mode}")

    if layer_mismatches:
        result.fail("Context策略与读取Layer一致性", "；".join(layer_mismatches))
    else:
        result.ok("Context策略与读取Layer一致性", "所有带Context的动作读取都落在策略允许的 COMMON/OVERLAY 通道")

    # refactor11：普通文字对话仍只修改公共消息引擎两处局部函数指针装载，并强制两个阶段使用不同包装函数。
    dialogue_text = read_utf8(src / "dialogue_input.c")
    addresses_text = read_utf8(src / "game_addresses.h")
    dialogue_required = [
        "PATCH_DIALOGUE_GETASYNC_A", "PATCH_DIALOGUE_GETASYNC_B", "Runtime_PatchMovEsiFunction",
        "DialogueInput_TypewriterGetAsyncKeyState", "DialogueInput_AdvanceGetAsyncKeyState",
        "GLOBAL_DIALOGUE_TOTAL_BYTES", "GLOBAL_DIALOGUE_VISIBLE_BYTES",
        "dialogue_text_is_still_revealing", "DIALOGUE_VK_CONTROL",
        "if (!dialogue_text_is_still_revealing()) return real_value",
        "if (dialogue_text_is_still_revealing()) return real_value",
        "g_confirm_pending = 0", "g_original_get_async_key_state",
        "ConfirmDialog_IsActive", "InputRouter_Consume(INPUT_CONFIRM)",
    ]
    dialogue_missing = [token for token in dialogue_required if token not in (dialogue_text + addresses_text + read_utf8(src / "runtime.c") + read_utf8(src / "runtime.h"))]
    dialogue_forbidden = [token for token in [
        "Runtime_PatchIatPointer(IAT_GETASYNCKEYSTATE", "_ReturnAddress", "__builtin_return_address",
        "DIALOGUE_LBUTTON_RETURN_A", "DIALOGUE_LBUTTON_RETURN_B", "SetCursorPos(", "mouse_event(", "SendInput("
    ] if token in dialogue_text]
    if not dialogue_missing and not dialogue_forbidden:
        result.ok("公共对话 A 两阶段结构", "逐字阶段只补全；整句阶段通过局部 Ctrl 门槛进入原版左键推进；不改全局 IAT")
    else:
        result.fail("公共对话 A 两阶段结构", f"缺少={dialogue_missing}，发现不允许路径={dialogue_forbidden}")

    # refactor26e：纵向对照 refactor21/refactor22/refactor24 与 refactor26d 后确认，
    # 剧情算法源码一直没变，真正回归是 refactor25 大重构把 DialogueInput_InstallHook()
    # 从 plugin 安装链漏掉。这里检查“能力是否真正接回”，而不是把 r24 的文本顺序当成永久 ABI。
    #
    # 工程原则：旧稳定版是效果/能力 Oracle。互不重叠的 installer 可以为了更清晰的模块边界重新排序；
    # 只有存在真实依赖关系的调用才要求先后顺序。这样既能防“源码在、Update 在、installer 却漏了”，
    # 也不会因为未来做无害重排而制造假 FAIL。
    plugin_text_for_dialogue = read_utf8(src / "plugin.c")
    dialogue_install_token = "if (!DialogueInput_InstallHook()) return 0;"
    dialogue_install_pos = plugin_text_for_dialogue.find(dialogue_install_token)
    confirm_install_pos_for_dialogue = plugin_text_for_dialogue.find("if (!ConfirmDialog_InstallHooks()) return 0;")
    dialogue_update_pos = plugin_text_for_dialogue.find("DialogueInput_Update();")
    dialogue_install_chain_ok = (
        dialogue_install_pos >= 0 and
        confirm_install_pos_for_dialogue >= 0 and
        dialogue_update_pos >= 0
    )
    if dialogue_install_chain_ok:
        result.ok("剧情Dialogue安装链完整性", "Dialogue installer、ConfirmDialog installer 与每帧 Dialogue Update 均可达；不机械绑定互不重叠 installer 的文本顺序")
    else:
        result.fail("剧情Dialogue安装链完整性", f"Dialogue installer位置={dialogue_install_pos}，Confirm installer位置={confirm_install_pos_for_dialogue}，Update位置={dialogue_update_pos}")

    # 总安装表护栏：防止以后模块化重构时再次发生“模块源码/Update 都在，但 installer 被总入口漏掉”。
    # MovieSkip/PadInput/Runtime 是 Initialize；其余业务模块为 InstallHook(s)。这里列出 refactor24
    # 已经验证存在、当前仍必须保留的全部业务 Hook 能力。允许未来新增模块，也允许互不依赖模块重排。
    r24_required_install_names = [
        "Cursor_InstallHooks",
        "UiBridge_InstallHooks",
        "Battle_InstallHooks",
        "Frontend_InstallHooks",
        "SaveSlot_InstallHooks",
        "SavePoint_Install",
        "InterfaceShell_InstallHooks",
        "InterfaceItems_InstallHooks",
        "InterfaceSkills_InstallHooks",
        "InterfaceEquipment_InstallHooks",
        "InterfaceInnerStats_InstallHooks",
        "InterfaceFormation_InstallHooks",
        "Exploration_InstallHooks",
        "ConfirmDialog_InstallHooks",
        "DialogueInput_InstallHook",
    ]
    install_body_match = re.search(r"static int plugin_install_all_hooks\(void\)\s*\{(.*?)\n\}", plugin_text_for_dialogue, re.S)
    current_install_sequence = []
    if install_body_match:
        body = install_body_match.group(1)
        for line in body.splitlines():
            stripped = line.strip()
            for name in r24_required_install_names:
                if stripped.startswith("if (!" + name + "()") or stripped == name + "();":
                    current_install_sequence.append(name)
                    break
    current_install_set = set(current_install_sequence)
    missing_r24_installs = [name for name in r24_required_install_names if name not in current_install_set]
    duplicate_r24_installs = sorted({name for name in current_install_sequence if current_install_sequence.count(name) > 1})
    if not missing_r24_installs and not duplicate_r24_installs:
        result.ok("refactor24业务Hook能力完整性", f"{len(r24_required_install_names)}/{len(r24_required_install_names)} 项已验证 installer 全部保留且无重复；允许无依赖模块优化顺序")
    else:
        result.fail("refactor24业务Hook能力完整性", f"缺少={missing_r24_installs}；重复={duplicate_r24_installs}；当前={current_install_sequence}")

    # installer 只约束真实依赖，而不强行复刻 r24 全排列。UiBridge 是统一 Button/Event 分发底座，
    # 依赖它的业务 Hook 必须在 UiBridge 安装之后；Exploration、Dialogue 等独立补丁则无需被虚假排序约束。
    install_index = {name: i for i, name in enumerate(current_install_sequence)}
    bridge_consumers = [
        "Battle_InstallHooks", "Frontend_InstallHooks", "SaveSlot_InstallHooks",
        "InterfaceShell_InstallHooks", "InterfaceItems_InstallHooks", "InterfaceSkills_InstallHooks",
        "InterfaceEquipment_InstallHooks", "InterfaceInnerStats_InstallHooks",
        "InterfaceFormation_InstallHooks", "ConfirmDialog_InstallHooks",
    ]
    ui_idx = install_index.get("UiBridge_InstallHooks", -1)
    bad_bridge_order = [name for name in bridge_consumers if install_index.get(name, -1) <= ui_idx]
    if ui_idx >= 0 and not bad_bridge_order:
        result.ok("业务Hook真实依赖顺序", "UiBridge 先于所有依赖其 Button/Event 分发的业务模块；独立 installer 不做无意义的全顺序绑定")
    else:
        result.fail("业务Hook真实依赖顺序", f"UiBridge索引={ui_idx}；依赖顺序异常={bad_bridge_order}；当前={current_install_sequence}")

    # worker 与 installer 不同：这里存在真实的输入消费/Context 优先级，因此仍必须约束语义顺序。
    # 但也只检查会影响效果的偏序关系，而不是把 r24 的整条列表当成不可优化的固定文本。
    known_worker_names = [
        "InputRouter_BeginFrame", "ControlModes_Update", "Cursor_Update", "Investigation_UpdateRumble",
        "MovieSkip_Update", "Exploration_Update",
        "InterfaceItems_Update", "InterfaceSkills_Update", "InterfaceEquipment_Update",
        "InterfaceInnerStats_Update", "InterfaceFormation_Update", "ConfirmDialog_Update",
        "DialogueInput_Update", "Battle_Update", "InterfaceShell_Update", "Frontend_Update",
    ]
    worker_match = re.search(r"while \(g_worker_running\) \{(.*?)Runtime_AdvanceTick\(\);", plugin_text_for_dialogue, re.S)
    current_worker_sequence = []
    if worker_match:
        body = worker_match.group(1)
        for match in re.finditer(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(", body):
            name = match.group(1)
            if name in known_worker_names:
                current_worker_sequence.append(name)
    worker_index = {name: i for i, name in enumerate(current_worker_sequence)}
    missing_worker = [name for name in known_worker_names if name not in worker_index]
    worker_constraints = [
        ("InputRouter_BeginFrame", "ControlModes_Update"),
        ("ControlModes_Update", "Cursor_Update"),
        ("Cursor_Update", "Investigation_UpdateRumble"),
        ("InputRouter_BeginFrame", "MovieSkip_Update"),
        ("InputRouter_BeginFrame", "Exploration_Update"),
        ("Cursor_Update", "Exploration_Update"),
        ("Cursor_Update", "InterfaceItems_Update"),
        ("InterfaceItems_Update", "ConfirmDialog_Update"),
        ("InterfaceSkills_Update", "ConfirmDialog_Update"),
        ("InterfaceEquipment_Update", "ConfirmDialog_Update"),
        ("InterfaceInnerStats_Update", "ConfirmDialog_Update"),
        ("InterfaceFormation_Update", "ConfirmDialog_Update"),
        ("ConfirmDialog_Update", "DialogueInput_Update"),
        ("DialogueInput_Update", "Battle_Update"),
        ("Battle_Update", "InterfaceShell_Update"),
        ("InterfaceShell_Update", "Frontend_Update"),
    ]
    broken_worker_constraints = [
        f"{a}->{b}" for a, b in worker_constraints
        if a not in worker_index or b not in worker_index or worker_index[a] >= worker_index[b]
    ]
    if not missing_worker and not broken_worker_constraints:
        result.ok("worker输入消费优先级", f"{len(known_worker_names)}/{len(known_worker_names)} 关键阶段均存在；所有真实 Context/消费偏序满足，允许互不冲突阶段优化")
    else:
        result.fail("worker输入消费优先级", f"缺少={missing_worker}；偏序异常={broken_worker_constraints}；当前={current_worker_sequence}")

    # refactor10/11 遗留的 ConfirmDialog 只是“历史失败候选代码”，不是当前推荐架构。
    # 用户已经明确否定“先做通用选择框再套各菜单”的路线；refactor12 也因此回滚。
    # 这里仍做机械检查，只是为了证明这段遗留代码没有退回 refactor8/refactor9 那种会污染
    # Title/Battle/SaveSlot 的广域猜测。后续主菜单开发应按具体页面/窗口逐一实现，完成后再判断能否抽公共部分。
    confirm_text = read_utf8(src / "confirm_dialog.c")
    bridge_text = read_utf8(src / "ui_bridge.c")
    plugin_text = read_utf8(src / "plugin.c")
    confirm_required = [
        "VTABLE_CONFIRM1_UPDATE", "VTABLE_CONFIRM2_UPDATE",
        "ConfirmDialog_HookUpdateType1", "ConfirmDialog_HookUpdateType2", "confirm_observe_owner",
        "POPUP_ACTIVE", "POPUP_RESULT", "confirm_is_battle_sealed_owner", "confirm_is_save_sealed_owner",
        "ConfirmDialog_FilterHit", "UiBridge_RequestEventOwned(UI_EVENT_OWNER_CONFIRM_DIALOG, button, 2",
        "INPUT_CTX_CONFIRM_DIALOG", "InputRouter_Consume(INPUT_CONFIRM)", "InputRouter_Consume(INPUT_CANCEL)",
        "CALL_CONFIRM2_HIT_YES", "CALL_CONFIRM2_EVENT_NO", "UiBridge_InstallConfirmDialogHooks",
        "confirm_native_to_focus", "if (native_focus == 1) return 0", "if (native_focus == 0) return 1",
    ]
    confirm_missing = [token for token in confirm_required if token not in (confirm_text + bridge_text + router + addresses_text)]
    confirm_forbidden = [token for token in [
        "confirm_seen_recently", "yes_seen_tick", "no_seen_tick", "ConfirmDialog_ObserveHit",
        "ConfirmDialog_HookOpen", "CALL_CONFIRM_OPEN_"
    ] if token in (confirm_text + bridge_text)]
    order_battle = bridge_text.find("Battle_FilterButtonHit")
    order_save = bridge_text.find("SaveSlot_FilterPopupHit", order_battle)
    order_items = bridge_text.find("InterfaceItems_FilterPopupHit", order_save)
    order_skills = bridge_text.find("InterfaceSkills_FilterPopupHit", order_items)
    order_confirm = bridge_text.find("ConfirmDialog_FilterHit", order_skills)
    priority_ok = (order_battle >= 0 and order_save > order_battle and order_items > order_save and
                   order_skills > order_items and order_confirm > order_skills)
    page_popup_isolation = "InterfaceItems_AnyPopupActive() || InterfaceSkills_AnyPopupActive()) return 0;" in confirm_text

    # refactor26c：refactor21 原本存在的 ConfirmDialog_InstallHooks() 在后续大重构时被遗漏。
    # 这不是为了重新启用“全局万能 Yes/No”，而是为了安装第二类 0x4276F0 的 4 个低层 CALL；
    # state2/state3 的页面专属 Adapter 仍然在 ConfirmDialog_Update/FilterHit 之前拥有业务和视觉。
    confirm_install_token = "if (!ConfirmDialog_InstallHooks()) return 0;"
    install_pos = plugin_text.find(confirm_install_token)
    formation_pos = plugin_text.find("if (!InterfaceFormation_InstallHooks()) return 0;")
    exploration_pos = plugin_text.find("if (!Exploration_InstallHooks()) return 0;")
    confirm_install_chain_ok = (
        install_pos >= 0 and formation_pos >= 0 and exploration_pos >= 0 and
        formation_pos < exploration_pos < install_pos
    )

    if not confirm_missing and not confirm_forbidden and priority_ok and page_popup_isolation and confirm_install_chain_ok:
        result.ok("ConfirmDialog隔离与法宝第二类Hook安装护栏", "页面专属state2/state3继续优先；同时恢复refactor21存在、后续大重构遗漏的ConfirmDialog_InstallHooks，确保0x4276F0四个CALL真正接入UiBridge")
    else:
        result.fail("ConfirmDialog隔离与法宝第二类Hook安装护栏", f"缺少={confirm_missing}，禁止项={confirm_forbidden}，优先级={priority_ok}，页面隔离={page_popup_isolation}，安装链={confirm_install_chain_ok}")

    # Start 本轮必须是最小实现：Movie Context 覆盖通用 Start，不允许再出现 refactor5/6 的状态/资源复杂化。
    movie_text = read_utf8(src / "movie_skip.c")
    movie_router_tokens = ["INPUT_SYSTEM_START", "INPUT_CTX_MOVIE", "PAD_START", "g_policy_movie"]
    missing_movie_router = [token for token in movie_router_tokens if token not in router]
    if missing_movie_router:
        result.fail("Start 通用语义与 Movie Context", "缺少：" + ", ".join(missing_movie_router))
    else:
        result.ok("Start 通用语义与 Movie Context", "Start -> INPUT_SYSTEM_START -> MOVIE/OVERRIDE")

    simple_required = [
        "GLOBAL_MOVIE_OBJECT", "MOVIE_ACTIVE_FLAG", "Runtime_MovieEscapeProtocolOk",
        "InputRouter_PressedOn(INPUT_CTX_MOVIE, INPUT_SYSTEM_START, INPUT_LAYER_OVERLAY)",
        "post_message_a(hwnd, WM_KEYDOWN_, VK_ESCAPE_, 0)",
    ]
    simple_missing = [token for token in simple_required if token not in (movie_text + read_utf8(src / "game_addresses.h") + read_utf8(src / "runtime.c"))]
    complex_forbidden = [
        "IAT_BINKOPEN", "PFN_BinkOpen", "MovieSkip_HookBinkOpen",
        "MOVIE_HOLD_SKIP_MS", "start_down_tick", "title_seen",
        "MOVIE_POLICY", "movie_policy_from_path", "\\Mov\\",
    ]
    complex_hits = [token for token in complex_forbidden if token in movie_text or token in read_utf8(src / "game_addresses.h")]
    if not simple_missing and not complex_hits:
        result.ok("Start=ESC 最小电影实现", "无资源分类/标题历史/长按计时/BinkOpen Hook")
    else:
        result.fail("Start=ESC 最小电影实现", f"缺少={simple_missing}，发现复杂旧方案={complex_hits}")

    pad_text = read_utf8(src / "pad_input.c")
    if "PAD_START" in pad_text and "mask &= ~(1u << (u32)PAD_START);" in pad_text:
        result.ok("Start 不污染 Cursor 所有权", "采样 Start，但从通用指针活动判断中排除")
    else:
        result.fail("Start 不污染 Cursor 所有权", "缺少 Start 采样或 Cursor 活动隔离")

    # refactor22：SDL3 热插拔必须主动刷新设备层。
    # 本插件没有跑 SDL 事件循环，因此不能沿用 r21 的“先 SDL_GamepadConnected，后 SDL_UpdateGamepads”顺序；
    # 没有手柄句柄时也必须继续 Update，否则重新插入的设备可能永远不进入 SDL 当前快照。
    hotplug_required = [
        "g_pad.update_gamepads();", "g_pad.gamepad && !g_pad.gamepad_connected(g_pad.gamepad)",
        "pad_close_current_gamepad", "pad_clear_sample_state", "pad_try_open_first_gamepad",
        "suppress_edges_once", "reconnect_sync_pending", "open_poll_counter++ & 0x1Fu",
        "g_pad.prev_buttons = g_pad.buttons;",
    ]
    hotplug_missing = [token for token in hotplug_required if token not in pad_text]
    poll_begin = pad_text.find("int PadInput_Poll(void)")
    poll_end = pad_text.find("int PadInput_Down", poll_begin)
    poll_text = pad_text[poll_begin:poll_end] if poll_begin >= 0 and poll_end > poll_begin else ""
    update_pos = poll_text.find("g_pad.update_gamepads();")
    connected_pos = poll_text.find("g_pad.gamepad && !g_pad.gamepad_connected(g_pad.gamepad)")
    enumerate_pos = poll_text.find("pad_try_open_first_gamepad();")
    read_pos = poll_text.find("g_pad.buttons = pad_read_buttons();")
    hotplug_order_ok = (
        update_pos >= 0 and connected_pos > update_pos and enumerate_pos > connected_pos and read_pos > enumerate_pos
    )
    read_begin = pad_text.find("static u32 pad_read_buttons(void)")
    read_end = pad_text.find("int PadInput_Poll(void)", read_begin)
    read_text = pad_text[read_begin:read_end] if read_begin >= 0 and read_end > read_begin else ""
    stale_order_absent = ("g_pad.update_gamepads()" not in read_text and "gamepad_connected" not in read_text)
    reconnect_edge_guard = (
        "else if (g_pad.suppress_edges_once)" in poll_text and
        "g_pad.prev_buttons = g_pad.buttons;" in poll_text and
        poll_text.find("g_pad.prev_buttons = g_pad.buttons;", read_pos) > read_pos
    )
    if not hotplug_missing and hotplug_order_ok and stale_order_absent and reconnect_edge_guard:
        result.ok("SDL3 手柄热插拔恢复链", "每tick先刷新设备层，再判断开/低频重枚举/采样；重连首帧同步边沿，避免插拔误触")
    else:
        result.fail("SDL3 手柄热插拔恢复链", f"缺少={hotplug_missing}，顺序={hotplug_order_ok}，旧顺序已移除={stale_order_absent}，首帧防误触={reconnect_edge_guard}")

    # refactor16：Back 首次正式进入采样，但固定只作为 Shift 修饰键。
    # 单按 Back 不能被视作普通“手柄活动”，否则只想进入精细鼠标模式时会无端改变 Cursor 所有权。
    # 所有 Context 的 Shift 策略必须保持 PASS，未来组合键才能在具体业务层自由读取，而不需要每个 Context 重复声明。
    pad_header = read_utf8(src / "pad_input.h")
    shift_required = [
        "PAD_BACK = 4", "PAD_BACK, PAD_START", "mask &= ~(1u << (u32)PAD_BACK);",
        "INPUT_MODIFIER_SHIFT", "case INPUT_MODIFIER_SHIFT:return PAD_BACK",
    ]
    shift_missing = [token for token in shift_required if token not in (pad_header + pad_text + router)]
    policy_shift_bad = []
    for match in re.finditer(r"static const InputPolicy\s+(g_policy_\w+)\s*=\s*\{\{(.*?)\}\};", router, flags=re.S):
        values = re.findall(r"INPUT_(?:PASS|MERGE|OVERRIDE|CONSUME)", match.group(2))
        if len(values) != 15 or values[-1] != "INPUT_PASS":
            policy_shift_bad.append(match.group(1))
    if not shift_missing and not policy_shift_bad:
        result.ok("Back 全局 Shift 修饰键", f"SDL Back已采样；单按不抢所有权；{len(list(re.finditer(r'static const InputPolicy\s+g_policy_', router)))}张策略均让Shift PASS")
    else:
        result.fail("Back 全局 Shift 修饰键", f"缺少={shift_missing}，Shift策略异常={policy_shift_bad}")

    # refactor37 用一套状态机裁决 Back/RT/LT；Cursor 只提供低层鼠标桥，不能再自行解释扳机。
    cursor_text_for_sensitivity = read_utf8(src / "cursor.c")
    control_text = read_utf8(src / "control_modes.c") + read_utf8(src / "control_modes.h")
    runtime_text = read_utf8(src / "runtime.c") + read_utf8(src / "runtime.h")
    mode_required = [
        "CONTROL_MODE_BACK_MOUSE", "CONTROL_MODE_RT_MOUSE", "CONTROL_MODE_INVESTIGATION",
        "if (back_pressed)", "free_map && rt_pressed", "free_map && lt_pressed",
        "rt_inhibit_until_release", "lt_inhibit_until_release", "resume_investigation_after_rt",
        "Cursor_SetMouseModeSession", "Cursor_MoveMouseSticks", "Cursor_PulseLeftClick",
        "Cursor_PulseRightClick", "InputRouter_CaptureAll", "ControlModes_OnPhysicalMouseTakeover",
        "control_any_mouse_ui_active", "Investigation_MapSnapshotReady", "GLOBAL_MAP_ACTION_BUSY",
        "MouseModeLeftStickSensitivityPercent", "MouseModeRightStickSensitivityPercent",
        'rt_cfg_int("Mouse", "MouseModeLeftStickSensitivityPercent", 100, 1, 300)',
        'rt_cfg_int("Mouse", "MouseModeRightStickSensitivityPercent", 15, 1, 300)',
    ]
    mode_joined = control_text + cursor_text_for_sensitivity + runtime_text
    mode_missing = [token for token in mode_required if token not in mode_joined]
    cursor_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", cursor_text_for_sensitivity, flags=re.S)
    removed_mouse_paths = [token for token in [
        "back_precision_mouse_session", "R3RightClickHoldMs", "right_stick_slow_sensitivity_percent",
        "InputRouter_Down(", "InputRouter_Pressed(", "PadInput_Down(PAD_RT)",
        "PadInput_Pressed(PAD_R3)", "PadInput_Released(PAD_R3)",
    ] if token in cursor_code]
    back_pos = control_text.find("if (back_pressed)")
    back_mode_pos = control_text.find("if (g_modes.mode == CONTROL_MODE_BACK_MOUSE)")
    rt_mode_pos = control_text.find("if (g_modes.mode == CONTROL_MODE_RT_MOUSE)")
    lt_mode_pos = control_text.find("if (g_modes.mode == CONTROL_MODE_INVESTIGATION)")
    priority_order_ok = 0 <= back_pos < back_mode_pos < rt_mode_pos < lt_mode_pos
    if not mode_missing and not removed_mouse_paths and priority_order_ok:
        result.ok("refactor37统一指针模式裁决", "Back常驻 > 地图RT临时 > 地图LT调查 > r36；新按沿、UI中断、释放屏障与实体鼠标无震动接管齐全")
    else:
        result.fail("refactor37统一指针模式裁决", f"缺少={mode_missing}，Cursor旧解释残留={removed_mouse_paths}，优先级顺序={priority_order_ok}")

    # 全局强度与每事件独立时长；模式回切优先级必须压住调查短震。
    rumble_required = [
        "rumble_strength_percent", "investigation_rumble_ms", "controller_mode_rumble_ms",
        'rt_cfg_int("Rumble", "StrengthPercent", 100, 0, 100)',
        'rt_cfg_int("Rumble", "InvestigationHoverDurationMs", 80, 0, 5000)',
        'rt_cfg_int("Rumble", "ControllerModeDurationMs", 1000, 0, 5000)',
        "PadInput_Rumble(u16 low_frequency, u16 high_frequency, u32 duration_ms, int priority)",
        "if (priority < g_pad.rumble_priority) return 0;", "CONTROL_RUMBLE_PRIORITY_MODE 2",
        "Runtime_Config()->investigation_rumble_ms, 1",
    ]
    rumble_joined = runtime_text + pad_text + pad_header + control_text + read_utf8(src / "investigation.c")
    rumble_missing = [token for token in rumble_required if token not in rumble_joined]
    if not rumble_missing:
        result.ok("refactor37全局震动强度与独立时长", "StrengthPercent全局；调查命中80ms、回手柄1000ms分别可调；高优先级模式反馈不被短震覆盖")
    else:
        result.fail("refactor37全局震动强度与独立时长", f"缺少={rumble_missing}")

    # refactor23 继续逐页 Adapter：r19 Shell 保持；state3/state4 已实机 PASS；state5 只做鼠标焦点隔离修复，并新增 state6 阵形独立 Adapter。
    # 状态页 state1 是只读页，r19 Shell 已覆盖其全部需求，因此不要求造一个空 state1 控制器。
    all_active_source = "\n".join(read_utf8(p) for p in src.glob("*.[ch]"))
    forbidden_patterns = {
        "SelectionUI_": "refactor12 公共 SelectionUI",
        "QuantityDialog_": "refactor12 公共 Quantity adapter",
        "InterfaceState5Adapter": "废弃的单体五内页面 Adapter 命名",
        "InterfaceState7Adapter": "废弃的单体天书页面 Adapter 命名",
        "InterfaceState8Adapter": "废弃的单体机能页面 Adapter 命名",
    }
    forbidden_hits = [label for token, label in forbidden_patterns.items() if token in all_active_source]
    extra_interface_pages = sorted(p.name for p in src.glob("interface_state*.c"))
    page_files_ok = all((src / name).is_file() for name in [
        "interface_items.c", "interface_items.h",
        "interface_skills.c", "interface_skills.h",
        "interface_equipment.c", "interface_equipment.h",
        "interface_inner_stats.c", "interface_inner_stats.h",
        "interface_formation.c", "interface_formation.h",
        "interface_tome.c", "interface_tome.h",
        "interface_options.c", "interface_options.h",
        "spatial_neighbor.c", "spatial_neighbor.h",
    ])
    if forbidden_hits or extra_interface_pages or not page_files_ok:
        detail = forbidden_hits + extra_interface_pages + ([] if page_files_ok else ["缺 state2~8 页面 Adapter / shared spatial 文件"])
        result.fail("主Interface逐页范围隔离", "发现：" + ", ".join(detail))
    else:
        result.ok("主Interface逐页范围隔离", "r19 Shell保持；state1只读封存；state2~6已PASS独立保留；state7复用SaveSlot并单独扩展动作窗口；state8独立Adapter；未恢复refactor12公共选择框")

    # refactor19：地图 Y 必须复用“原版 Space 已经触发后的业务事件”，绝不能发送假的 Space。
    # 同时 r18 的“动画门关闭时直接丢按键”必须改为短队列缓存，等 +0x590>8 后再提交。
    shell_text = read_utf8(src / "interface_shell.c")
    exploration_text = read_utf8(src / "exploration.c")
    bridge_text_shell = read_utf8(src / "ui_bridge.c")
    addresses_text = read_utf8(src / "game_addresses.h")
    runtime_shell_text = read_utf8(src / "runtime.c")
    shell_required = [
        "Runtime_InterfaceShellProtocolOk", "Runtime_MapSpaceEventTarget", "rt_mem_eq_masked",
        "UiBridge_InstallInterfaceShellHooks", "InterfaceShell_OnExplorationGameThread", "interface_shell_capture_map_y",
        "SIG_MAP_SPACE_BRANCH", "CALL_MAP_SPACE_EVENT", "g_map_space_event_target",
        "GLOBAL_MAP_ACTION_BUSY", "GLOBAL_MAP_KEY_MODE", "map_space_event(1)",
        "InterfaceShell_OnExplorationGameThread();",
        "INTERFACE_SHELL_QUEUE_CAPACITY", "interface_shell_enqueue_action",
        "interface_shell_capture_root_actions", "interface_shell_dispatch_root_action",
        "INTERFACE_READY_COUNTER",
        "CALL_INTERFACE_CATEGORY_1", "CALL_INTERFACE_CATEGORY_8",
        "CALL_INTERFACE_EXIT_EVENT", "CALL_INTERFACE_ROLE_EVENT",
        "interface_shell_button_accepts_event",
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_SHELL, INPUT_CATEGORY_PREV",
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_SHELL, INPUT_CATEGORY_NEXT",
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_SHELL, INPUT_CANCEL",
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE",
    ]
    shell_joined = shell_text + exploration_text + bridge_text_shell + addresses_text + runtime_shell_text
    shell_missing = [token for token in shell_required if token not in shell_joined]

    # 下面这些属于已经实机失败或明确禁止的实现方式。注释里可以讨论历史地址，
    # 所以只针对真正运行时代码符号/API 做检查，不能因为文档写了“不要 PostMessage”就误报。
    shell_forbidden = []
    forbidden_runtime_tokens = [
        "InterfaceShell_HookNormalMapOpenGate", "CALL_MAP_NORMAL_INTERFACE_GATE", "FN_MAP_INTERFACE_GATE",
        "g_interface_shell.map_y_latched", "FN_INTERFACE_CTOR)(", "malloc(0x660",
        "g_api.post_message_a", "SendInput(", "keybd_event(",
        "*(i32*)(i + INTERFACE_STATE) =", "*(i32*)(i + INTERFACE_SELECTED_ROLE) =",
        "*(i32*)(i + INTERFACE_CLOSE_STATE) = 1",
    ]
    for token in forbidden_runtime_tokens:
        if token in shell_text:
            shell_forbidden.append(token)

    # 0x89F818 可以保留为已确认历史字段，但 r19 地图 Y 不允许直接写它。
    if re.search(r"\*\s*\(\s*i32\s*\*\s*\)\s*GLOBAL_MAP_INTERFACE_STATE\s*=", shell_text):
        shell_forbidden.append("直接写 GLOBAL_MAP_INTERFACE_STATE")

    # 机械确认没有通过 Windows 键盘消息伪造 Space；VK_SPACE 字样只允许出现在解释原版反汇编的注释里。
    if re.search(r"post_message_a\s*\([^;]*(?:0x20|VK_SPACE)", shell_text, flags=re.I | re.S):
        shell_forbidden.append("PostMessage伪造Space")

    # r19 全局日志抑制必须是多槽缓存，才能压住“RB请求/UI消费”交替出现的重复词条。
    log_required = [
        "RUNTIME_LOG_DEDUPE_SLOTS", "RUNTIME_LOG_DEDUPE_MS", "rt_log_hash",
        "rt_log_should_suppress", "slot->last_tick = now",
        "rt_log_should_suppress(utf8_line, n)",
    ]
    log_missing = [token for token in log_required if token not in runtime_shell_text]

    if not shell_missing and not shell_forbidden and not log_missing:
        result.ok("refactor20a 地图Y特征兼容/动画缓冲/日志去重", "Y从原版Space CALL现场解析当前目标，不校验0x40B230整段函数体；Shell动画期按键进入8项语义队列；全局相同日志2秒连续抑制")
    else:
        result.fail("refactor20a 地图Y特征兼容/动画缓冲/日志去重", f"缺少={shell_missing}，禁止项={shell_forbidden}，日志缺少={log_missing}")

    # UiBridge pending 的 owner 隔离是 r17“按键迟钝”修复的核心，必须机械证明。
    battle_owner_text = read_utf8(src / "battle.c")
    save_owner_text = read_utf8(src / "save_slot.c")
    confirm_owner_text = read_utf8(src / "confirm_dialog.c")
    owner_required = [
        "typedef enum UiEventOwner", "g_pending_owner",
        "UiBridge_RequestEventOwned", "UiBridge_ClearEventOwned",
        "UI_EVENT_OWNER_BATTLE", "UI_EVENT_OWNER_SAVE_SLOT",
        "UI_EVENT_OWNER_CONFIRM_DIALOG", "UI_EVENT_OWNER_INTERFACE",
    ]
    owner_joined = read_utf8(src / "ui_bridge.h") + bridge_text_shell + battle_owner_text + save_owner_text + confirm_owner_text + shell_text
    owner_missing = [token for token in owner_required if token not in owner_joined]
    bad_battle_global_clear = "UiBridge_ClearEvent();" in battle_owner_text
    battle_owned_clear = "UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE)" in battle_owner_text
    explicit_owner_requests = all(token in owner_joined for token in [
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE",
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_SAVE_SLOT",
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_CONFIRM_DIALOG",
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE",
    ])
    if not owner_missing and battle_owned_clear and not bad_battle_global_clear and explicit_owner_requests:
        result.ok("refactor18/19 UiBridge事件owner隔离", "Battle只清BATTLE owner；Battle/Save/Confirm/Interface请求均显式标 owner，不再跨模块误删 pending")
    else:
        result.fail("refactor18/19 UiBridge事件owner隔离", f"缺少={owner_missing}，Battle有owner清理={battle_owned_clear}，Battle全局清理={bad_battle_global_clear}，四类显式owner={explicit_owner_requests}")

    # SaveSlot 的 row+0x57C 虽然作为逆向字段保留在地址表，但不能出现在导航实现中作为过滤条件。
    save_text = read_utf8(src / "save_slot.c")
    if "SAVE_ROW_HAS_DATA" in save_text:
        result.fail("SaveSlot 全显示槽可选", "save_slot.c 仍读取 SAVE_ROW_HAS_DATA")
    else:
        result.ok("SaveSlot 全显示槽可选", "导航实现不按空槽/已有存档过滤")

    # dev20 已验证的原生动画必须同时写倒计时、目标行、selected-row，再 refresh；只写 selected-row 会复发 dev18 问题。
    animation_tokens = ["SAVE_ANIM_COUNTDOWN", "SAVE_ANIM_ROW", "SAVE_SELECTED_ROW", "FN_SAVE_SLOT_REFRESH"]
    if all(token in save_text for token in animation_tokens):
        positions = [save_text.find(token) for token in animation_tokens]
        # 这里只要求“同一个实现文件完整持有协议”，具体时序还由人工/实机复核；避免脆弱地匹配格式。
        result.ok("SaveSlot 原生四步选中动画协议", "关键字段与 refresh 均存在")
    else:
        result.fail("SaveSlot 原生四步选中动画协议", "缺少关键字段/refresh")

    # 新增的 dev20 已知缺陷修复：返回事务必须比旧菜单退场 latch 更先拿到 HitTest 决策权。
    battle_text = read_utf8(src / "battle.c")
    fn_pos = battle_text.find("Battle_FilterButtonHit")
    return_pos = battle_text.find("g_return_visual", fn_pos)
    latch_pos = battle_text.find("g_visual_latch", fn_pos)
    if fn_pos >= 0 and return_pos >= 0 and latch_pos >= 0 and return_pos < latch_pos:
        result.ok("战斗取消返回视觉事务优先级", "ReturnVisual 在 teardown visual latch 之前")
    else:
        result.fail("战斗取消返回视觉事务优先级", "无法证明返回事务先于旧菜单退场锁存")

    # refactor1 实机已经证明“只过滤 HitTest”仍会让 marker 在第 1 项停留 0.5~1 秒。
    # refactor2 必须把 0x41C380 已确认的原版最终赋值协议显式写入返回事务：
    # marker+0x24 = 当前真实 Button+0x38。这里检查常量、helper 和 arm/maintenance 三层都存在。
    marker_tokens = [
        "COMMAND_MARKER_OBJECT", "BUTTON_LOCAL_Y", "MARKER_LOCAL_Y",
        "top_visual_sync_marker_to_button", "return_visual_arm_top", "return_visual_maintenance",
    ]
    missing_marker = [token for token in marker_tokens if token not in battle_text and token not in read_utf8(src / "game_addresses.h")]
    arm_pos = battle_text.find("static void return_visual_arm_top")
    maintain_pos = battle_text.find("static void return_visual_maintenance", arm_pos)
    arm_sync = battle_text.find("top_visual_sync_marker_to_button", arm_pos, maintain_pos) if arm_pos >= 0 and maintain_pos >= 0 else -1
    maintain_end = battle_text.find("static int target_mode_bounds", maintain_pos) if maintain_pos >= 0 else -1
    maintain_sync = battle_text.find("top_visual_sync_marker_to_button", maintain_pos, maintain_end) if maintain_pos >= 0 and maintain_end >= 0 else -1
    if not missing_marker and arm_sync >= 0 and maintain_sync >= 0:
        result.ok("战斗返回视觉原版 marker 同步", "建立事务时立即同步，并在原版 HitTest 汇合前持续维持")
    else:
        result.fail("战斗返回视觉原版 marker 同步", f"缺符号={missing_marker}，arm同步={arm_sync>=0}，maintenance同步={maintain_sync>=0}")

    # refactor3 的真正根因修复：0x41C470 在顶层展开/收起动画中每帧直接把 marker Y 写成第1项。
    # 因此必须 Hook 唯一的 0x41C3B2 动画 CALL，并且“先执行原版动画，再按 g_top_focus 恢复 marker Y”。
    anim_required = [
        "CALL_TOP_PANEL_ANIM", "FN_TOP_PANEL_ANIM", "Battle_HookTopPanelAnim",
        "orig(panel);", "g_top_focus", "COMMAND_MARKER_OBJECT", "COMMAND_BUTTONS",
        "MARKER_LOCAL_Y", "BUTTON_LOCAL_Y",
    ]
    addr_text = read_utf8(src / "game_addresses.h")
    missing_anim = [token for token in anim_required if token not in battle_text and token not in addr_text]
    hook_begin = battle_text.find("Battle_HookTopPanelAnim")
    hook_end = battle_text.find("int Battle_InstallHooks", hook_begin)
    hook_text = battle_text[hook_begin:hook_end] if hook_begin >= 0 and hook_end > hook_begin else ""
    orig_pos = hook_text.find("orig(panel);")
    marker_write_pos = hook_text.find("MARKER_LOCAL_Y")
    patch_present = "Runtime_PatchCall(CALL_TOP_PANEL_ANIM" in battle_text
    if not missing_anim and orig_pos >= 0 and marker_write_pos > orig_pos and patch_present:
        result.ok("战斗顶层动画期视觉一致性", "唯一 0x41C470 CALL 已 Hook；原版动画先执行，随后恢复真实逻辑焦点 marker Y")
    else:
        result.fail("战斗顶层动画期视觉一致性", f"缺符号={missing_anim}，原版先执行={orig_pos>=0 and marker_write_pos>orig_pos}，Hook安装={patch_present}")

    # refactor4：CMD1/CMD2 返回父列表时，原版动画会暂时跳过正常行 HitTest。
    # 因此必须在两个“列表动画 CALL”外层执行原版后，再按生命周期证据维持原版 Button+0x44 行视觉。
    sub_anim_required = [
        "CALL_CMD1_PANEL_ANIM", "FN_CMD1_PANEL_ANIM", "Battle_HookCmd1PanelAnim",
        "CALL_CMD2_PANEL_ANIM", "FN_CMD2_PANEL_ANIM", "Battle_HookCmd2PanelAnim",
        "submenu_animation_visual_target", "submenu_animation_sync_visual", "BUTTON_HOVER_ACTIVE",
        "g_visual_latch_active", "g_return_visual.active", "g_nav_active",
    ]
    missing_sub_anim = [token for token in sub_anim_required if token not in battle_text and token not in addr_text]
    cmd1_hook_begin = battle_text.find("Battle_HookCmd1PanelAnim")
    cmd2_hook_begin = battle_text.find("Battle_HookCmd2PanelAnim")
    warp_hook_begin = battle_text.find("Battle_HookSubmenuDefaultCursorWarp")
    cmd1_hook = battle_text[cmd1_hook_begin:cmd2_hook_begin] if cmd1_hook_begin >= 0 and cmd2_hook_begin > cmd1_hook_begin else ""
    cmd2_hook = battle_text[cmd2_hook_begin:warp_hook_begin] if cmd2_hook_begin >= 0 and warp_hook_begin > cmd2_hook_begin else ""
    cmd1_order = cmd1_hook.find("orig(sub);") >= 0 and cmd1_hook.find("submenu_animation_sync_visual") > cmd1_hook.find("orig(sub);")
    cmd2_order = cmd2_hook.find("orig(sub);") >= 0 and cmd2_hook.find("submenu_animation_sync_visual") > cmd2_hook.find("orig(sub);")
    sub_anim_patches = all(token in battle_text for token in [
        "Runtime_PatchCall(CALL_CMD1_PANEL_ANIM",
        "Runtime_PatchCall(CALL_CMD2_PANEL_ANIM",
    ])
    if not missing_sub_anim and cmd1_order and cmd2_order and sub_anim_patches:
        result.ok("CMD1/CMD2 动画期行视觉一致性", "两列表都先执行原版动画，再按 latch/return/current 三类证据恢复原版 Button+0x44")
    else:
        result.fail("CMD1/CMD2 动画期行视觉一致性", f"缺符号={missing_sub_anim}，CMD1顺序={cmd1_order}，CMD2顺序={cmd2_order}，Hook安装={sub_anim_patches}")

    # refactor4：两个列表展开完成后各有一个唯一 0x43DF30 CALL，会把真实鼠标 warp 到默认列表位置。
    # 只能在“Battle 手柄导航 + Cursor 确认控制器拥有光标”时吞掉；否则必须继续调用原版，保护键鼠。
    warp_begin = battle_text.find("Battle_HookSubmenuDefaultCursorWarp")
    warp_end = battle_text.find("Battle_HookTopPanelAnim", warp_begin)
    warp_text = battle_text[warp_begin:warp_end] if warp_begin >= 0 and warp_end > warp_begin else ""
    warp_guard = "g_nav_active && Cursor_ControllerOwnsPointer()" in warp_text
    warp_fallback = "return orig(x, y);" in warp_text
    warp_patches = all(token in battle_text for token in [
        "Runtime_PatchCall(CALL_CMD1_PANEL_CURSOR_WARP",
        "Runtime_PatchCall(CALL_CMD2_PANEL_CURSOR_WARP",
    ])
    if warp_guard and warp_fallback and warp_patches:
        result.ok("CMD1/CMD2 默认鼠标 warp Context 覆盖", "仅手柄导航拥有光标时吞掉；键鼠路径仍调用原版 0x43DF30")
    else:
        result.fail("CMD1/CMD2 默认鼠标 warp Context 覆盖", f"双条件={warp_guard}，键鼠回退={warp_fallback}，双Hook={warp_patches}")

    # 所有 Battle 语义动作最后都会经过 activate_nav()。这里统一 claim 光标后，
    # 从 Target/确认框按 B 返回的同一个 tick 就不会出现“业务已切回手柄、Cursor 仍被当成鼠标”的窗口。
    activate_begin = battle_text.find("static void activate_nav")
    activate_end = battle_text.find("static void* category_button", activate_begin)
    activate_text = battle_text[activate_begin:activate_end] if activate_begin >= 0 and activate_end > activate_begin else ""
    if "Cursor_ClaimForControllerNavigation();" in activate_text:
        result.ok("Battle 导航统一取得光标所有权", "activate_nav() 集中 claim，子 Context 不再依赖重复补调用")
    else:
        result.fail("Battle 导航统一取得光标所有权", "activate_nav() 缺少 Cursor_ClaimForControllerNavigation()")

    # Pointer takeover 只能撤销导航/视觉所有权，不能抹掉已经排给游戏线程的一次性业务事务。
    # dev20 的稳定实现没有在这里清 g_target_event_code / UiBridge pending；refactor1 多清这两项是 Target BIG 回归候选根因。
    takeover_begin = battle_text.find("void Battle_OnPointerTakeover")
    takeover_end = battle_text.find("int Battle_InstallHooks", takeover_begin)
    takeover_text = battle_text[takeover_begin:takeover_end] if takeover_begin >= 0 and takeover_end > takeover_begin else ""
    bad_target_clear = re.search(r"g_target_event_code\s*=\s*0", takeover_text) is not None
    bad_ui_clear = "UiBridge_ClearEvent();" in takeover_text
    if takeover_text and not bad_target_clear and not bad_ui_clear:
        result.ok("Pointer takeover 不销毁业务 pending", "Target A/B 与 UIBridge pending 保留到各自游戏线程/Context 生命周期消费")
    else:
        result.fail("Pointer takeover 不销毁业务 pending", f"Target清零={bad_target_clear}，UiBridge清零={bad_ui_clear}")

    # Target 的 A/B pending 由真实 selector 生命周期消费，而不是被 worker 采样 Context 卡死。
    target_hook_begin = battle_text.find("Battle_HookTargetGridIndex")
    target_hook_end = battle_text.find("static void log_native_hit_activity", target_hook_begin)
    target_hook = battle_text[target_hook_begin:target_hook_end] if target_hook_begin >= 0 and target_hook_end > target_hook_begin else ""
    target_required = ["TARGET_ACTIVE", "TARGET_EVENT_CODE", "g_target_event_code"]
    missing_target = [token for token in target_required if token not in target_hook]
    if not missing_target and "g_context == BCTX_TARGET" not in target_hook.split("if (g_nav_active", 1)[0]:
        result.ok("Target A/B 游戏线程事务门槛", "pending 由 selector+TARGET_ACTIVE 消费，不依赖 worker Context")
    else:
        result.fail("Target A/B 游戏线程事务门槛", f"缺少={missing_target}；消费段仍依赖 worker Context={'g_context == BCTX_TARGET' in target_hook.split('if (g_nav_active',1)[0]}")

    # refactor37 明确键鼠优先：真实鼠标移动必须在任何手柄活动回抢之前立即返回接管事件。
    cursor_text = read_utf8(src / "cursor.c")
    cursor_update_begin = cursor_text.find("CursorTakeoverEvent Cursor_Update")
    cursor_update_end = cursor_text.find("int Cursor_GetPointerPosition", cursor_update_begin)
    cursor_update = cursor_text[cursor_update_begin:cursor_update_end] if cursor_update_begin >= 0 and cursor_update_end > cursor_update_begin else ""
    order_physical = cursor_update.find("physical_moved = cursor_observe_physical_mouse")
    immediate_return = cursor_update.find("if (physical_moved) return CURSOR_TAKEOVER_PHYSICAL_MOUSE;")
    order_gamepad = cursor_update.find("gamepad_active = cursor_gamepad_has_navigation_activity")
    physical_wins = 0 <= order_physical < immediate_return < order_gamepad
    if physical_wins:
        result.ok("Cursor 同 tick 键鼠优先", "真实鼠标先无条件接管；ControlModes随后无震动结束手柄指针会话")
    else:
        result.fail("Cursor 同 tick 键鼠优先", f"physical={order_physical}，立即返回={immediate_return}，gamepad={order_gamepad}")

    # refactor18 仍把未涉及本轮 owner 迁移的稳定业务文件按 r16 SHA 锁死。
    # Battle/Save/Confirm 的 .c 因 owner-aware API 迁移有意改变，另用 r18 新 SHA 封住。
    sealed_bad = []
    for name, expected in SEALED_REFACTOR16_STABLE_SHA256.items():
        path = src / name
        if not path.is_file():
            sealed_bad.append(name + "(缺失)")
            continue
        actual = sha256(path)
        if actual.lower() != expected.lower():
            sealed_bad.append(name + "(SHA变化)")
    if sealed_bad:
        result.fail("refactor16 稳定业务源码字节级保护", ", ".join(sealed_bad))
    else:
        result.ok("refactor16 稳定业务源码字节级保护", f"{len(SEALED_REFACTOR16_STABLE_SHA256)}/{len(SEALED_REFACTOR16_STABLE_SHA256)} 文件 SHA 完全一致（owner迁移与Interface接入文件另行检查）")

    save_text_guard = read_utf8(src / "save_slot.c") + read_utf8(src / "save_slot.h")
    save_stable_tokens = [
        "CALL_SAVE_TICK_ANIM", "CALL_SAVE_ROW_EVENT", "CALL_SAVE_CANCEL_EVENT",
        "CALL_SAVE_PAGE_PREV", "CALL_SAVE_PAGE_NEXT", "SaveSlot_InstallHooks",
        "SaveSlot_InstallActionHooks", "CALL_SAVE_ACTION_HIT", "CALL_SAVE_ACTION_EVENT",
        "SAVE_VIEW_ACTION", "SAVE_VIEW_ACTION_POPUP",
    ]
    save_stable_missing = [token for token in save_stable_tokens if token not in save_text_guard]
    stable_installer = save_text_guard.find("int SaveSlot_InstallHooks(void)")
    action_installer = save_text_guard.find("int SaveSlot_InstallActionHooks(void)")
    separate_installers = stable_installer >= 0 and action_installer > stable_installer
    if save_stable_missing or not separate_installers:
        result.fail("SaveSlot稳定核心与共享动作扩展隔离", f"缺少={save_stable_missing}，稳定/动作Hook分离={separate_installers}")
    else:
        result.ok("SaveSlot稳定核心与共享动作扩展隔离", "标题已PASS五处Hook保留；天书/存档点共用的三项窗口两处Hook独立安装")

    # refactor26 用户明确要求给 state3 新增 D-Pad 左右翻页，因此 interface_skills.c 合法变化，
    # 不能继续拿 r21 的整文件 SHA 阻止这次用户批准的输入增强。
    # state3 的“业务安全边界”仍由下方 Adapter 结构检查、原版协议预检和“不直接写状态字段”护栏保护。
    result.ok("state3 技能页已验收核心保护", "用户批准新增左右翻页；取消整文件SHA冻结，继续用协议/无直写/确认框护栏保护原版业务")

    # Battle Target 已由用户接受旧成熟方向规则，refactor26 继续冻结，因此本轮整个 battle.c 不允许变化。
    battle_sha_bad = []
    for name, expected in SEALED_REFACTOR24_BATTLE_SHA256.items():
        path = src / name
        if not path.is_file():
            battle_sha_bad.append(name + "(缺失)")
            continue
        actual = sha256(path)
        if actual.lower() != expected.lower():
            battle_sha_bad.append(name + "(SHA变化)")
    if battle_sha_bad:
        result.fail("Battle Target 用户PASS字节级冻结", ", ".join(battle_sha_bad))
    else:
        result.ok("Battle Target 用户PASS字节级冻结", "battle.c 与 refactor26 继续冻结的旧成熟Target方案字节完全一致")

    # refactor37 为命中提醒增加 SDL_RumbleGamepad，因此 pad_input 不能继续整文件 SHA 冻结。
    # 改用结构护栏：既有热插拔时序与新连接首帧边沿抑制必须完整保留，震动导出必须可选。
    pad_joined = pad_text + pad_header
    hotplug_required = [
        "g_pad.update_gamepads();", "g_pad.gamepad_connected(g_pad.gamepad)",
        "pad_try_open_first_gamepad();", "g_pad.prev_buttons = g_pad.buttons;",
        "g_pad.suppress_edges_once", "g_pad.reconnect_sync_pending",
        "PFN_SDL_RumbleGamepad", 'PAD_RESOLVE(rumble_gamepad, PFN_SDL_RumbleGamepad, "SDL_RumbleGamepad")',
        "if (!g_pad.gamepad || !g_pad.rumble_gamepad || duration_ms == 0u) return 0;",
    ]
    hotplug_missing = [token for token in hotplug_required if token not in pad_joined]
    update_pos = pad_text.find("g_pad.update_gamepads();", pad_text.find("int PadInput_Poll"))
    connected_pos = pad_text.find("g_pad.gamepad && !g_pad.gamepad_connected", update_pos)
    enumerate_pos = pad_text.find("pad_try_open_first_gamepad();", connected_pos)
    sample_pos = pad_text.find("g_pad.prev_buttons = g_pad.buttons;", enumerate_pos)
    hotplug_order_ok = 0 <= update_pos < connected_pos < enumerate_pos < sample_pos
    required_gate = pad_text[pad_text.find("if (!g_pad.init_subsystem"):pad_text.find("g_pad.initialized = 1")]
    rumble_is_optional = "rumble_gamepad" not in required_gate
    if not hotplug_missing and hotplug_order_ok and rumble_is_optional:
        result.ok("SDL3 热插拔与可选震动边界", "Update→断连检查→低频枚举→采样顺序保留；首帧无误触；缺震动导出不影响输入")
    else:
        result.fail("SDL3 热插拔与可选震动边界", f"缺少={hotplug_missing}，时序={hotplug_order_ok}，震动可选={rumble_is_optional}")

    # refactor11 普通对话已经实机通过，本轮不能因为新增鼠标桥而改动对话包装器。
    dialogue_sha_bad = []
    for name, expected in SEALED_REFACTOR11_DIALOGUE_SHA256.items():
        path = src / name
        if not path.is_file():
            dialogue_sha_bad.append(name + "(缺失)")
            continue
        actual = sha256(path)
        if actual.lower() != expected.lower():
            dialogue_sha_bad.append(name + "(SHA变化)")
    if dialogue_sha_bad:
        result.fail("refactor11 已验收对话源码字节级保护", ", ".join(dialogue_sha_bad))
    else:
        result.ok("refactor11 已验收对话源码字节级保护", "2/2 文件 SHA 完全一致")

    tome_scope_bad = []
    for name, expected in SEALED_REFACTOR33_TOME_SCOPE_SHA256.items():
        path = src / name
        if not path.is_file():
            tome_scope_bad.append(name + "(缺失)")
        elif sha256(path).lower() != expected.lower():
            tome_scope_bad.append(name + "(SHA变化)")
    if tome_scope_bad:
        result.fail("refactor37旧天书/SaveSlot冻结范围", ", ".join(tome_scope_bad))
    else:
        result.ok("refactor37旧天书/SaveSlot冻结范围", "天书与共享SaveSlot 4/4 文件逐字节保持旧验收版本；Cursor仅按本轮授权改造")

    # R3 复合状态机被新需求明确删除，但可靠的 48ms Windows消息 + GetKeyState 双通道桥必须保留，
    # 供 RT 的 A/B 与 LT 的 A 共同复用。
    cursor_bridge_required = [
        "IAT_GETKEYSTATE", "Cursor_HookGameGetKeyState", "PFN_GetKeyState",
        "VK_LBUTTON_", "VK_RBUTTON_", "g_cursor.game_get_key_state(virtual_key)",
        "g_cursor.mouse_left_sent", "g_cursor.mouse_right_sent",
        "CURSOR_CLICK_PULSE_MS 48u", "Runtime_MsToTicks(CURSOR_CLICK_PULSE_MS)",
        "Runtime_PatchIatPointer(IAT_GETKEYSTATE", "api->mouse_event",
        "void Cursor_PulseLeftClick(void)", "void Cursor_PulseRightClick(void)",
        "cursor_update_click_pulses", "left_release_tick", "right_release_tick",
    ]
    cursor_bridge_missing = [token for token in cursor_bridge_required if token not in (cursor_text + addresses_text + read_utf8(src / "platform.h"))]
    cursor_bridge_forbidden = [token for token in [
        "Runtime_PatchIatPointer(IAT_GETASYNCKEYSTATE",
        "r3_long_locked", "r3_long_right_down", "r3_short_left_down",
        "R3_LEFT_CLICK_PULSE_MS",
    ] if token in cursor_text]
    if not cursor_bridge_missing and not cursor_bridge_forbidden:
        result.ok("refactor37共享可靠鼠标点击桥", "RT/LT共用48ms左右键脉冲；GetKeyState与mouse_event双通道保留；旧R3状态机已移除")
    else:
        result.fail("refactor37共享可靠鼠标点击桥", f"缺少={cursor_bridge_missing}，旧R3残留={cursor_bridge_forbidden}")

    # LT调查只观察原版resolver真值；模式进入、优先级和CaptureAll由ControlModes统一承担。
    investigation_text = read_utf8(src / "investigation.c") + read_utf8(src / "investigation.h")
    plugin_text_r37 = read_utf8(src / "plugin.c")
    exploration_text_r37 = read_utf8(src / "exploration.c")
    investigation_required = [
        "Runtime_InvestigationProtocolOk", "CALL_EXPLORATION_TARGET_RESOLVE", "FN_EXPLORATION_TARGET_RESOLVE",
        "i32 result = original(scene, count);", "inv_publish_snapshot(scene, count, result);", "return result;",
        "scene + 0x1Cu", "scene + 0x20u", "scene + 0x2Cu", "scene + 0x30u",
        "record + 0x73u", "type >= 5u", "distance_squared < 10000",
        "collision + 0x48u", "frame + 0x54u", "INVESTIGATION_SCREEN_WIDTH  640",
        "INVESTIGATION_SCREEN_HEIGHT 480", "INVESTIGATION_CANDIDATES    25",
        "g_snapshot_sequence", "before == after", "snapshot->serial == g_investigation.pending_snapshot_serial",
        "inv_abs(cross) > dot", "investigation_snap_radius_pixels",
        "Cursor_MoveInvestigationRightStick", "Cursor_PulseLeftClick", "Investigation_UpdateActive",
        "PadInput_Rumble", "last_rumble_object",
    ]
    investigation_joined = investigation_text + runtime_text + addresses_text + read_utf8(src / "platform.h")
    investigation_missing = [token for token in investigation_required if token not in investigation_joined]
    worker_begin = plugin_text_r37.find("while (g_worker_running)")
    worker_text = plugin_text_r37[worker_begin:] if worker_begin >= 0 else ""
    worker_order = [worker_text.find(token) for token in [
        "PadInput_Poll();", "InputRouter_BeginFrame();", "ControlModes_Update();",
        "Cursor_Update();", "Investigation_UpdateRumble();", "Exploration_Update();",
    ]]
    worker_order_ok = all(pos >= 0 for pos in worker_order) and worker_order == sorted(worker_order)
    modal_movement_ok = "ControlModes_BlocksMapMovement()" in exploration_text_r37
    protocol_gate_ok = "if (!Runtime_InvestigationProtocolOk())" in investigation_text
    no_direct_trigger_policy = all(token not in investigation_text for token in [
        "PadInput_Down(PAD_LT)", "PadInput_Down(PAD_RT)", "InputRouter_Down(INPUT_SUBTYPE_PREV)",
    ])
    if not investigation_missing and worker_order_ok and modal_movement_ok and protocol_gate_ok and no_direct_trigger_policy:
        result.ok("refactor37 LT原版resolver调查", "原函数真值不变；640x480合格快照；左杆方向硬切；右杆低速短吸附；模式裁决与地图移动抑制分层")
    else:
        result.fail("refactor37 LT原版resolver调查", f"缺少={investigation_missing}，worker顺序={worker_order_ok}，移动抑制={modal_movement_ok}，协议门={protocol_gate_ok}，无私自扳机策略={no_direct_trigger_policy}")

    # refactor14 起 CMD1/CMD2 技能/道具列表的 D-Pad 左右应复用原版分页 ButtonEvent。
    # refactor16 复核发现 r14/r15 的 Battle 业务代码虽然已经调用 INPUT_NAV_LEFT/RIGHT，
    # 但 InputRouter 的 Battle List 策略仍把左右标成 CONSUME，导致业务层根本可能收不到按键。
    # 所以现在必须同时检查“业务调用”和“路由策略”两端，防止以后再次出现代码存在但入口被吞掉的假功能。
    battle_page_required = [
        "static int request_direct_page_delta",
        "page_button(ctx, 0)", "page_button(ctx, 1)",
        "g_pending_page_context = ctx", "g_pending_page_old = page",
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_BATTLE, pb, 2",
        "battle_pressed(INPUT_NAV_LEFT)", "request_direct_page_delta(g_context,-1)",
        "battle_pressed(INPUT_NAV_RIGHT)", "request_direct_page_delta(g_context,1)",
    ]
    battle_page_missing = [token for token in battle_page_required if token not in battle_text]
    direct_page_begin = battle_text.find("static int request_direct_page_delta")
    direct_page_end = battle_text.find("static int move_list_row", direct_page_begin)
    direct_page_text = battle_text[direct_page_begin:direct_page_end] if direct_page_begin >= 0 and direct_page_end > direct_page_begin else ""
    direct_page_bad = [token for token in ["SUB1_CURRENT_PAGE =", "SUB2_CURRENT_PAGE =", "*(i32*)(sub + SUB1_CURRENT_PAGE) =", "*(i32*)(sub + SUB2_CURRENT_PAGE) ="] if token in direct_page_text]

    battle_policy_match = re.search(r"static const InputPolicy g_policy_battle_list = \{\{(.*?)\}\};", router, flags=re.S)
    battle_policy_values = re.findall(r"INPUT_(?:PASS|MERGE|OVERRIDE|CONSUME)", battle_policy_match.group(1)) if battle_policy_match else []
    # InputAction 顺序：A,B,X,Y,Up,Down,Left,Right,LB,RB,LT,RT,Start,R3,Shift。
    battle_router_ok = len(battle_policy_values) == 15 and battle_policy_values[6] == "INPUT_OVERRIDE" and battle_policy_values[7] == "INPUT_OVERRIDE"

    if not battle_page_missing and not direct_page_bad and battle_router_ok:
        result.ok("refactor16 Battle 技能/道具左右直接翻页闭环", "业务代码+InputRouter双端成立；←上一页/→下一页复用原版ButtonEvent与pending-page握手")
    else:
        result.fail("refactor16 Battle 技能/道具左右直接翻页闭环", f"缺少={battle_page_missing}，直接写页码={direct_page_bad}，路由左右OVERRIDE={battle_router_ok}")

    # refactor22：用户实机证明“只检查 +0x45/+0x04”仍不够。
    # 某些业务不可用项可能在视觉握手或 ButtonEvent 后续业务门处被原版拒绝，
    # 因此必须同时具备：建立前拒绝、延迟确认超时、事件消费后同 Context 自动解锁、B 逃生清理。
    confirm_begin = battle_text.find("static void request_confirm")
    confirm_end = battle_text.find("static void request_top_shortcut", confirm_begin)
    battle_confirm_text = battle_text[confirm_begin:confirm_end] if confirm_begin >= 0 and confirm_end > confirm_begin else ""
    watch_begin = battle_text.find("static void confirm_watch_maintenance(void) {")
    watch_end = battle_text.find("static void", watch_begin + 20)
    battle_watch_text = battle_text[watch_begin:watch_end] if watch_begin >= 0 and watch_end > watch_begin else ""
    battle_cancel_begin = battle_text.find("if(battle_pressed(INPUT_CANCEL))")
    battle_cancel_end = battle_text.find("/* 先收集纵向输入", battle_cancel_begin)
    battle_cancel_text = battle_text[battle_cancel_begin:battle_cancel_end] if battle_cancel_begin >= 0 and battle_cancel_end > battle_cancel_begin else ""
    battle_softlock_required = [
        "g_pending_confirm_start_tick", "g_confirm_watch_active", "g_confirm_watch_button",
        "confirm_watch_maintenance", "confirm_watch_clear",
        "if (!button_usable(b))",
        "UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE)",
        "g_pending_confirm_after_nav = 0", "nav_settle_clear()",
    ]
    battle_softlock_missing = [token for token in battle_softlock_required if token not in battle_text]
    pre_defer_gate = (
        "current_button = focused_button();" in battle_text and
        "if (!button_usable(current_button))" in battle_text
    )
    deferred_timeout = (
        "g_pending_confirm_start_tick" in battle_text and
        "g_nav_settle_timeout_ticks" in battle_text and
        "g_pending_confirm_after_nav = 0" in battle_text
    )
    watch_recovery = (
        "UiBridge_EventOwner() == UI_EVENT_OWNER_BATTLE" in battle_watch_text and
        "UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE);" in battle_watch_text and
        "if (g_context != g_confirm_watch_context)" in battle_watch_text and
        "age = Runtime_Tick() - g_confirm_watch_start_tick" in battle_watch_text and
        "g_confirm_watch_consumed_seen" in battle_watch_text and
        "g_confirm_watch_consumed_tick = Runtime_Tick()" in battle_watch_text and
        "Runtime_Tick() - g_confirm_watch_consumed_tick" in battle_watch_text and
        ">= g_nav_settle_min_ticks" in battle_watch_text
    )
    cancel_escape = (
        "UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE);" in battle_cancel_text and
        "confirm_watch_clear" in battle_cancel_text and
        battle_cancel_text.find("UiBridge_ClearEventOwned(UI_EVENT_OWNER_BATTLE);") < battle_cancel_text.find("request_cancel();")
    )
    if not battle_softlock_missing and pre_defer_gate and deferred_timeout and watch_recovery and cancel_escape:
        result.ok("Battle 禁用项A事务解锁闭环", "建立前拒绝 + deferred超时 + Event消费后重新计时完整握手窗口 + 原版拒绝自动解锁 + B清Battle owner")
    else:
        result.fail("Battle 禁用项A事务解锁闭环", f"缺少={battle_softlock_missing}，建立前门={pre_defer_gate}，延迟超时={deferred_timeout}，watch恢复={watch_recovery}，B逃生={cancel_escape}")

    # refactor20：法宝页只能通过原版 ButtonEvent 工作，不能直接写 subtype/page/row/inventory。
    items_text = read_utf8(src / "interface_items.c")
    items_header = read_utf8(src / "interface_items.h")
    items_required = [
        "Runtime_InterfaceItemsProtocolOk", "UiBridge_InstallInterfaceItemsHooks",
        "CALL_INTERFACE_ITEMS_SUBTYPE", "CALL_INTERFACE_ITEMS_SPECIAL_A", "CALL_INTERFACE_ITEMS_SPECIAL_B",
        "CALL_INTERFACE_ITEMS_PAGE_PREV", "CALL_INTERFACE_ITEMS_PAGE_NEXT", "CALL_INTERFACE_ITEMS_ROW",
        "INPUT_CTX_INTERFACE_ITEMS", "INPUT_SUBTYPE_PREV", "INPUT_SUBTYPE_NEXT",
        "items_change_subtype", "items_move_vertical", "items_confirm_current_row", "items_special_x",
        "INTERFACE_ITEMS_SPECIAL_BUTTON_A", "INTERFACE_ITEMS_SPECIAL_BUTTON_B",
        "items_button_usable(distribute)", "items_button_usable(discard)",
        "InterfaceItems_AnyPopupActive", "InterfaceItems_FilterPopupHit",
        "UiBridge_ClearEventOwned(UI_EVENT_OWNER_INTERFACE)",
        "InterfaceItems_ObservePopupHit", "g_items.popup_focus = 1",
        "candidate_x = *(i32*)((u8*)candidate + 0x34u)",
    ]
    items_joined = items_text + items_header + bridge_text + confirm_text + router + runtime_shell_text + addresses_text
    items_missing = [token for token in items_required if token not in items_joined]
    # 只在 interface_items.c 的运行时代码中禁止直接写这些原版状态字段。
    # 注释里会提到“绝不直接修改”，所以正则只匹配赋值形式。
    items_direct_writes = []
    for field in ["INTERFACE_ITEMS_SUBINDEX", "INTERFACE_ITEMS_CURRENT_ROW", "INTERFACE_ITEMS_CURRENT_PAGE"]:
        if re.search(rf"\([^\n]*\+\s*{field}[^\n]*\)\s*=", items_text):
            items_direct_writes.append(field)
    special_index_guess = re.search(r"INTERFACE_ITEMS_SUBINDEX[^\n]*(==|!=)\s*[45]", items_text) is not None
    if not items_missing and not items_direct_writes and not special_index_guess:
        result.ok("state2法宝/道具页面专属Adapter", "LT/RT六子类、上下8行/跨页、A、X分发/弃置、本页弹窗全部复用原版Event；无直接写页码/子类型")
    else:
        result.fail("state2法宝/道具页面专属Adapter", f"缺少={items_missing}，直接写字段={items_direct_writes}，X硬猜子类型index={special_index_guess}")


    # refactor21：state3 必须是独立 Adapter，并继承“安全默认取消 + 原版 HitTest 观察”原则。
    skills_text = read_utf8(src / "interface_skills.c")
    skills_header = read_utf8(src / "interface_skills.h")
    skills_required = [
        "Runtime_InterfaceSkillsProtocolOk", "UiBridge_InstallInterfaceSkillsHooks",
        "CALL_INTERFACE_SKILLS_SUBTYPE", "CALL_INTERFACE_SKILLS_PAGE_PREV",
        "CALL_INTERFACE_SKILLS_PAGE_NEXT", "CALL_INTERFACE_SKILLS_ROW",
        "CALL_INTERFACE_SKILLS_TARGET_ROW", "CALL_INTERFACE_SKILLS_TARGET_CANCEL",
        "SIG_INTERFACE_SKILLS_TARGET_UPDATE", "INTERFACE_SKILLS_TARGET_ACTIVE",
        "INTERFACE_SKILLS_TARGET_BUTTON0", "INTERFACE_SKILLS_TARGET_CANCEL",
        "INPUT_CTX_INTERFACE_SKILLS", "InterfaceSkills_ObservePopupHit", "InterfaceSkills_FilterPopupHit",
        "g_skills.popup_focus = 1", "skills_change_subtype", "skills_move_vertical",
        "skills_confirm_current", "INTERFACE_SKILLS_POPUP", "POPUP_BUTTON_NO",
        "skills_update_target_selector", "skills_target_show_focus", "UiBridge_GetButtonScreenCenter",
        "skills_request_event(button, 2", "skills_request_event(button, 1",
        "InterfaceSkills_TargetSelectionActive",
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE",
    ]
    skills_joined = skills_text + skills_header + bridge_text + confirm_text + router + runtime_shell_text + addresses_text
    skills_missing = [token for token in skills_required if token not in skills_joined]
    skills_direct_writes = []
    for field in ["INTERFACE_SKILLS_SUBINDEX", "INTERFACE_SKILLS_CURRENT_ROW", "INTERFACE_SKILLS_CURRENT_PAGE",
                  "INTERFACE_SKILLS_TARGET_ACTIVE"]:
        if re.search(rf"\([^\n]*\+\s*{field}[^\n]*\)\s*=", skills_text):
            skills_direct_writes.append(field)
    # A 的按钮必须由 popup_focus 选择；禁止写死 POPUP_BUTTON_YES。B 则必须显式走 NO。
    popup_begin = skills_text.find("static void skills_update_popup")
    popup_end = skills_text.find("static void skills_change_subtype", popup_begin)
    skills_popup_text = skills_text[popup_begin:popup_end] if popup_begin >= 0 and popup_end > popup_begin else ""
    skills_popup_safe = (
        "g_skills.popup_focus == 0 ? POPUP_BUTTON_YES : POPUP_BUTTON_NO" in skills_popup_text and
        "button = *(void**)(popup + POPUP_BUTTON_NO);" in skills_popup_text and
        "InterfaceSkills_ObservePopupHit" in skills_text
    )
    target_modal_safe = all(token in skills_joined + shell_text for token in [
        "INTERFACE_SKILLS_TARGET_ACTIVE", "INTERFACE_SKILLS_TARGET_BUTTON0", "INTERFACE_SKILLS_TARGET_CANCEL",
        "UiBridge_GetButtonScreenCenter", "InputRouter_Consume(INPUT_CANCEL)",
        "InputRouter_Consume(INPUT_CATEGORY_PREV)", "InputRouter_Consume(INPUT_CATEGORY_NEXT)",
        "if (InterfaceSkills_TargetSelectionActive()) return 1;",
    ])
    # 治疗目标真正的角色ID/目标数位于 +0x58C/+0x588/+0x788/+0x78C；refactor28 只能让原版 Event 自己写。
    target_business_direct_write = re.search(r"\+\s*0x(?:58C|588|788|78C)u?[^\n]*\)\s*=", skills_text, flags=re.IGNORECASE) is not None
    if not skills_missing and not skills_direct_writes and skills_popup_safe and target_modal_safe and not target_business_direct_write:
        result.ok("state3绝学/法术页面专属Adapter", "既有列表/分页/安全确认保持；治疗法术目标复用原版5角色Button与右键取消，←/→选人、A使用、B取消；不直接写目标ID")
    else:
        result.fail("state3绝学/法术页面专属Adapter", f"缺少={skills_missing}，直接写字段={skills_direct_writes}，确认安全={skills_popup_safe}，目标modal={target_modal_safe}，目标业务直写={target_business_direct_write}")

    # refactor26：角色切换必须由左摇杆水平越过50%的单次方向沿产生，D-Pad 不再进入 Shell 的角色切换读取。
    analog_role_ok = all(token in router + shell_text for token in [
        "INPUT_LEFT_STICK_50_THRESHOLD 16384",
        "InputRouter_LeftStickHorizontalStep50",
        "g_left_stick_horizontal_latched",
        "axis > -PAD_STICK_DEADZONE && axis < PAD_STICK_DEADZONE",
        "INTERFACE_SHELL_ACTION_ROLE_PREV", "INTERFACE_SHELL_ACTION_ROLE_NEXT",
        "state >= 1 && state <= 5",
    ]) and (
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_SHELL, INPUT_NAV_LEFT" not in shell_text and
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_SHELL, INPUT_NAV_RIGHT" not in shell_text
    )
    if analog_role_ok:
        result.ok("主Interface左摇杆50%角色切换", "水平绝对值达到50%才产生一次方向沿；持续推住不重复，回中后重新武装；D-Pad已从Shell换人移除")
    else:
        result.fail("主Interface左摇杆50%角色切换", "左摇杆阈值/单次锁存/Shell去D-Pad角色读取未完整闭合")

    # state4“及身/装备”必须是独立 Adapter。
    # refactor26 产品规格：X循环三个装备栏位、↑↓候选、A装备；D-Pad左右与LT/RT都能翻页，四个入口必须复用同一个原版分页事务。
    equipment_text = read_utf8(src / "interface_equipment.c")
    equipment_header = read_utf8(src / "interface_equipment.h")
    equipment_required = [
        "Runtime_InterfaceEquipmentProtocolOk", "UiBridge_InstallInterfaceEquipmentHooks",
        "CALL_INTERFACE_EQUIPMENT_PAGE_PREV", "CALL_INTERFACE_EQUIPMENT_PAGE_NEXT",
        "CALL_INTERFACE_EQUIPMENT_ROW", "CALL_INTERFACE_EQUIPMENT_SLOT_0",
        "CALL_INTERFACE_EQUIPMENT_SLOT_1", "CALL_INTERFACE_EQUIPMENT_SLOT_2",
        "INPUT_CTX_INTERFACE_EQUIPMENT", "equipment_cycle_slot", "equipment_move_vertical",
        "equipment_confirm_current", "equipment_finish_page_landing", "equipment_request_page",
        "[及身页] ←：请求原版上一页。", "[及身页] →：请求原版下一页。",
        "[及身页] LT：请求原版上一页。", "[及身页] RT：请求原版下一页。",
        "page_wait_start_tick", "Runtime_MsToTicks(1000u)",
        "INTERFACE_EQUIPMENT_ROW_COUNT", "INTERFACE_EQUIPMENT_SLOT_COUNT",
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE",
    ]
    equipment_joined = equipment_text + equipment_header + bridge_text + router + runtime_shell_text + addresses_text
    equipment_missing = [token for token in equipment_required if token not in equipment_joined]
    equipment_direct_writes = []
    for field in ["INTERFACE_EQUIPMENT_CURRENT_SLOT", "INTERFACE_EQUIPMENT_CURRENT_PAGE", "INTERFACE_EQUIPMENT_CURRENT_ROW"]:
        if re.search(rf"\([^\n]*\+\s*{field}[^\n]*\)\s*=", equipment_text):
            equipment_direct_writes.append(field)
    direct_transaction = ("0x42E160" in re.sub(r"/\*.*?\*/|//[^\n]*", "", equipment_text, flags=re.S))
    # 策略顺序与 InputAction 枚举一一对应：A,B,X,Y,↑,↓,←,→,LB,RB,LT,RT,Start,R3,Shift。
    equipment_policy = re.search(r"static const InputPolicy\s+g_policy_interface_equipment\s*=\s*\{\{(.*?)\}\};", router, flags=re.S)
    equipment_policy_values = re.findall(r"INPUT_(?:PASS|MERGE|OVERRIDE|CONSUME)", equipment_policy.group(1)) if equipment_policy else []
    equipment_policy_ok = (
        len(equipment_policy_values) == 15 and
        equipment_policy_values[0] == "INPUT_OVERRIDE" and  # A
        equipment_policy_values[1] == "INPUT_PASS" and      # B -> Shell
        equipment_policy_values[2] == "INPUT_OVERRIDE" and  # X
        equipment_policy_values[4] == "INPUT_OVERRIDE" and equipment_policy_values[5] == "INPUT_OVERRIDE" and
        equipment_policy_values[6] == "INPUT_OVERRIDE" and equipment_policy_values[7] == "INPUT_OVERRIDE" and
        equipment_policy_values[8] == "INPUT_PASS" and equipment_policy_values[9] == "INPUT_PASS" and
        equipment_policy_values[10] == "INPUT_OVERRIDE" and equipment_policy_values[11] == "INPUT_OVERRIDE"
    )
    page_lr_routing_ok = (
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)" in items_text and
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_ITEMS, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)" in items_text and
        "items_request_page(page, -1)" in items_text and "items_request_page(page, 1)" in items_text and
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)" in skills_text and
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_SKILLS, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)" in skills_text and
        "skills_request_page(page, -1)" in skills_text and "skills_request_page(page, 1)" in skills_text and
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY)" in equipment_text and
        "InputRouter_PressedOn(INPUT_CTX_INTERFACE_EQUIPMENT, INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY)" in equipment_text and
        "[及身页] ←：请求原版上一页。" in equipment_text and "[及身页] →：请求原版下一页。" in equipment_text
    )

    if not equipment_missing and not equipment_direct_writes and not direct_transaction and equipment_policy_ok and page_lr_routing_ok:
        result.ok("state4及身/装备页面专属Adapter", "X循环3栏位、↑↓候选；D-Pad左右与LT/RT全部复用同一原版分页事务；A走原版装备事务；角色切换改左摇杆；不直接写原版装备状态")
    else:
        result.fail("state4及身/装备页面专属Adapter", f"缺少={equipment_missing}，直接写字段={equipment_direct_writes}，直调装备事务={direct_transaction}，策略={equipment_policy_ok}，三页左右翻页={page_lr_routing_ok}")

    # refactor22：state5“五内”也必须是独立 Adapter。
    # X 只循环插件私有焦点；LT/RT/Y 必须点击原版真实 Button；左右必须 PASS 给 Shell 换人，绝不能直接写五内临时值或角色属性。
    inner_text = read_utf8(src / "interface_inner_stats.c")
    inner_header = read_utf8(src / "interface_inner_stats.h")
    cursor_text = read_utf8(src / "cursor.c") + read_utf8(src / "cursor.h")
    inner_required = [
        "Runtime_InterfaceInnerStatsProtocolOk", "UiBridge_InstallInterfaceInnerStatsHooks",
        "CALL_INTERFACE_INNER_STATS_EVENT", "SIG_INTERFACE_INNER_STATS_UPDATE",
        "INPUT_CTX_INTERFACE_INNER_STATS", "g_inner_cycle_order",
        "INTERFACE_INNER_NODE_FIERCE", "INTERFACE_INNER_NODE_DIVINE",
        "INTERFACE_INNER_NODE_DEMON", "INTERFACE_INNER_NODE_SOUL", "INTERFACE_INNER_NODE_SWIFT",
        "inner_cycle_focus", "inner_adjust_current", "inner_commit",
        "INTERFACE_INNER_STATS_COMMIT_INDEX", "Cursor_ShowMenuFocusAt",
        "Cursor_HideMenuFocusImmediately", "InterfaceInnerStats_OnPointerTakeover",
        "inner_begin_button_pulse", "InterfaceInnerStats_TrySyntheticButtonEvent",
        "InterfaceInnerStats_SyntheticEventPending", "pulse_phase",
        "idle_reset_button", "inner_restore_button_idle_if_owned", "FN_BUTTON_SPRITE_FEEDBACK",
    ]
    inner_joined = inner_text + inner_header + bridge_text + router + runtime_shell_text + addresses_text + cursor_text
    inner_missing = [token for token in inner_required if token not in inner_joined]

    # 用户指定 X 顺序：烈 -> 神 -> 魔 -> 魂 -> 迅。这里机械核对源码中的顺序表，避免内部 0..4 顺序被误当成视觉顺序。
    cycle_match = re.search(r"g_inner_cycle_order\s*\[[^]]+\]\s*=\s*\{(.*?)\};", inner_text, flags=re.S)
    cycle_values = re.findall(r"INTERFACE_INNER_NODE_[A-Z]+", cycle_match.group(1)) if cycle_match else []
    expected_cycle = [
        "INTERFACE_INNER_NODE_FIERCE", "INTERFACE_INNER_NODE_DIVINE",
        "INTERFACE_INNER_NODE_DEMON", "INTERFACE_INNER_NODE_SOUL", "INTERFACE_INNER_NODE_SWIFT",
    ]
    cycle_ok = cycle_values == expected_cycle

    # LT/RT 调用的加减函数必须严格按“偶数减、奇数加”选择真实 Button；Y 必须点击 index10。
    event_map_ok = (
        "g_inner.node_index * 2 + (direction > 0 ? 1 : 0)" in inner_text and
        "inner_button(page, INTERFACE_INNER_STATS_COMMIT_INDEX)" in inner_text
    )

    # 策略顺序：A,B,X,Y,↑,↓,←,→,LB,RB,LT,RT,Start,R3,Shift。
    inner_policy = re.search(r"static const InputPolicy\s+g_policy_interface_inner_stats\s*=\s*\{\{(.*?)\}\};", router, flags=re.S)
    inner_policy_values = re.findall(r"INPUT_(?:PASS|MERGE|OVERRIDE|CONSUME)", inner_policy.group(1)) if inner_policy else []
    inner_policy_ok = (
        len(inner_policy_values) == 15 and
        inner_policy_values[0] == "INPUT_CONSUME" and
        inner_policy_values[1] == "INPUT_PASS" and
        inner_policy_values[2] == "INPUT_OVERRIDE" and inner_policy_values[3] == "INPUT_OVERRIDE" and
        inner_policy_values[4] == "INPUT_CONSUME" and inner_policy_values[5] == "INPUT_CONSUME" and
        inner_policy_values[6] == "INPUT_CONSUME" and inner_policy_values[7] == "INPUT_CONSUME" and
        inner_policy_values[8] == "INPUT_PASS" and inner_policy_values[9] == "INPUT_PASS" and
        inner_policy_values[10] == "INPUT_OVERRIDE" and inner_policy_values[11] == "INPUT_OVERRIDE"
    )

    # 五内业务修改必须全部留给原版 Event。源码中不允许出现对五个值/角色字段的直接赋值形式。
    # 本模块只允许给插件自己的 g_inner 字段赋值。
    suspicious_direct_writes = []
    code_without_comments = re.sub(r"/\*.*?\*/|//[^\n]*", "", inner_text, flags=re.S)
    for token in ["ROLE_", "INNER_VALUE", "TEMP_VALUE", "STAT_VALUE"]:
        if re.search(rf"\*?\([^\n]*{token}[^\n]*\)\s*=", code_without_comments):
            suspicious_direct_writes.append(token)

    # refactor23 必须把“合成焦点鼠标”与 state5 两条真实鼠标业务入口都隔离：
    # 0x431400 防鼠标按键边沿，UiBridge 过滤防 0x431380 内部 HitTest/历史状态继续累积。
    visual_isolation_ok = all(token in inner_joined for token in [
        "CALL_INTERFACE_INNER_STATS_MOUSE", "FN_BUTTON_MOUSE_EDGE",
        "InterfaceInnerStats_HookMouseEdge", "InterfaceInnerStats_FilterVisualOnlyButtonEvent",
        "if (InterfaceInnerStats_FilterVisualOnlyButtonEvent(button)) return 0",
        "Cursor_ShowMenuFocusAt", "controller_focus_visible",
    ])
    # refactor26b：五内焦点不再压在数值中心。逻辑锚点仍是减/加按钮中心之间，
    # 但显示用手形鼠标固定向右上约60°偏移（+21,-36，约42px）。
    focus_offset_ok = all(token in inner_text for token in [
        "minus_center_x", "plus_center_x",
        "#define INNER_FOCUS_OFFSET_X  21",
        "#define INNER_FOCUS_OFFSET_Y -36",
        "*out_x = (minus_center_x + plus_center_x) / 2 + INNER_FOCUS_OFFSET_X",
        "*out_y = (minus_center_y + plus_center_y) / 2 + INNER_FOCUS_OFFSET_Y",
        "UiBridge_GetButtonScreenRect",
    ])
    exact_button_geometry_ok = all(token in bridge_text + addresses_text for token in [
        "UiBridge_GetButtonScreenRect", "BUTTON_SPRITE_FRAME_OWNER",
        "BUTTON_SPRITE_FRAME_GEOMETRY", "BUTTON_SPRITE_FRAME_WIDTH", "BUTTON_SPRITE_FRAME_HEIGHT",
        "if (width == 0 || height == 0)",
    ])

    # refactor26 产品键位：五内只从 LT/RT 调 inner_adjust_current，D-Pad 不承担角色切换；
    # Shell 必须改用左摇杆水平50%单次方向沿，且只在 state1..5 允许角色切换。
    inner_trigger_binding_ok = (
        "INPUT_SUBTYPE_PREV, INPUT_LAYER_OVERLAY" in inner_text and
        "INPUT_SUBTYPE_NEXT, INPUT_LAYER_OVERLAY" in inner_text and
        "INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY" not in inner_text and
        "INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY" not in inner_text
    )
    shell_state5_role_ok = (
        "if (state < 1 || state > 5) return 0;" in shell_text and
        "InputRouter_LeftStickHorizontalStep50()" in shell_text and
        "state >= 1 && state <= 5" in shell_text and
        "INPUT_NAV_LEFT, INPUT_LAYER_OVERLAY" not in shell_text and
        "INPUT_NAV_RIGHT, INPUT_LAYER_OVERLAY" not in shell_text
    )

    pulse_ok = all(token in inner_text + bridge_text for token in [
        "if (g_inner.pulse_phase == 1)", "g_inner.pulse_phase = 2",
        "if (g_inner.pulse_phase == 2)", "g_inner.pulse_phase = 3",
        "InterfaceInnerStats_TrySyntheticButtonEvent(button)",
        "return g_pending_code != 0 || InterfaceInnerStats_SyntheticEventPending()",
        "Runtime_MsToTicks(500u)",
    ])

    # refactor26b：不能只伪造 0x431400/0x431380 的返回值。
    # 必须像真实鼠标一样维护 Button+0x2C（press history）与 +0x28（release history）：
    #   按下帧  +0x2C=2，随后 +0x28=2；
    #   松开帧  +0x2C=0，随后 +0x28=0 并返回 code=2。
    # 非脉冲视觉帧也必须把两份历史归零，防止按钮动画停留在旧状态。
    mouse_history_sync_ok = all(token in inner_text for token in [
        "#define INNER_BUTTON_RELEASE_HISTORY 0x28u",
        "#define INNER_BUTTON_PRESS_HISTORY   0x2Cu",
        "*(i32*)((u8*)button + INNER_BUTTON_PRESS_HISTORY) = 2",
        "*(i32*)((u8*)button + INNER_BUTTON_PRESS_HISTORY) = 0",
        "*(i32*)((u8*)button + INNER_BUTTON_RELEASE_HISTORY) = 2",
        "*(i32*)((u8*)button + INNER_BUTTON_RELEASE_HISTORY) = 0",
        "inner_clear_button_mouse_history_if_owned",
    ])

    # refactor26c：点击事务与“手形鼠标焦点是否仍显示”必须彻底解耦。
    # 一旦 pulse 开始，HookMouseEdge 必须先处理 pulse，再判断 controller_focus_visible/Cursor owner；
    # 指针接管只能隐藏已验收的焦点鼠标，不能把已经按下的业务 pulse 直接取消；
    # 500ms watchdog 在按下已发生时必须推进 release，而不能再次丢弃半个点击。
    pulse_priority_pos = inner_text.find("if (owned && g_inner.pulse_phase != 0)")
    visual_gate_pos = inner_text.find("if (owned && g_inner.controller_focus_visible && Cursor_ControllerOwnsPointer())")
    takeover_begin = inner_text.find("void InterfaceInnerStats_OnPointerTakeover")
    update_begin = inner_text.find("void InterfaceInnerStats_Update", takeover_begin)
    takeover_block = inner_text[takeover_begin:update_begin] if takeover_begin >= 0 and update_begin > takeover_begin else ""
    pulse_transaction_completion_ok = (
        pulse_priority_pos >= 0 and visual_gate_pos > pulse_priority_pos and
        "g_inner.pulse_button = NULL;" not in takeover_block and
        "g_inner.pulse_phase = 0;" not in takeover_block and
        "g_inner.pulse_phase = 3;" in inner_text and
        "等待原版0x431380执行恢复动画" in inner_text
    )

    # refactor26d：release 不是 idle。原版 0x43E6A0(0) 会进入 Sprite state5 循环，
    # 所以 synthetic release 完成后必须记住同一个真实 Button，并在“下一次 state5 游戏线程扫描到它”时
    # 调用原版 Button 初始化时使用的 0x43E6F0(0) 恢复普通态。
    # 这里同时禁止在五内模块里直接写 Sprite+0x1C 等动画状态字段；业务只能调用原版函数。
    idle_restore_ok = all(token in inner_text + addresses_text for token in [
        "idle_reset_button",
        "inner_restore_button_idle_if_owned",
        "FN_BUTTON_SPRITE_FEEDBACK",
        "g_inner.idle_reset_button = button",
        "if (owned && g_inner.idle_reset_button == button)",
        "set_feedback(sprite, 0)",
    ])
    direct_sprite_state_write_absent = not re.search(
        r"(?:BUTTON_SPRITE_PTR|sprite).*?\+\s*0x1C[^\n]*=|\+\s*0x1Cu[^\n]*=",
        code_without_comments,
        flags=re.I | re.S,
    )

    # 用户已经实机确认 refactor26b 的五内手形鼠标位置/右上偏移可以封版。
    # 这里对 inner_focus_anchor + inner_show_node_focus 的完整源码片段做固定 SHA，
    # 防止未来修按钮动画时又“顺手”改掉已验收的鼠标视觉。
    focus_start = inner_text.find("static int inner_focus_anchor")
    focus_end = inner_text.find("\n/*\n * 这是 refactor23 为五内新增的“视觉鼠标隔离器”。", focus_start)
    focus_chunk = inner_text[focus_start:focus_end] if focus_start >= 0 and focus_end > focus_start else ""
    focus_visual_sha = hashlib.sha256(focus_chunk.encode("utf-8")).hexdigest() if focus_chunk else ""
    focus_visual_frozen_ok = focus_visual_sha == "e886d71a2cd2aade5287a88e900a31eb25279aff21dcc542664622f306dd755a"

    if not inner_missing and cycle_ok and event_map_ok and inner_policy_ok and not suspicious_direct_writes and visual_isolation_ok and focus_offset_ok and exact_button_geometry_ok and inner_trigger_binding_ok and shell_state5_role_ok and pulse_ok and mouse_history_sync_ok and pulse_transaction_completion_ok and idle_restore_ok and direct_sprite_state_write_absent and focus_visual_frozen_ok:
        result.ok("state5五内页面专属Adapter", "X循环五节点；LT/RT减加、Y蕴魂；press->release事务不再依赖焦点鼠标所有权；D-Pad不换人；角色用左摇杆50%；已验收手形鼠标视觉SHA冻结")
    else:
        result.fail("state5五内页面专属Adapter", f"缺少={inner_missing}，X顺序={cycle_ok}，Event映射={event_map_ok}，策略={inner_policy_ok}，LT/RT绑定={inner_trigger_binding_ok}，左摇杆换人={shell_state5_role_ok}，两阶段脉冲={pulse_ok}，鼠标历史同步={mouse_history_sync_ok}，事务必完成={pulse_transaction_completion_ok}，release后idle恢复={idle_restore_ok}，无直接Sprite状态写={direct_sprite_state_write_absent}，视觉隔离={visual_isolation_ok}，焦点偏移={focus_offset_ok}，焦点视觉冻结={focus_visual_frozen_ok}，原版Button完整几何={exact_button_geometry_ok}，疑似直接写={suspicious_direct_writes}")

    # 鼠标焦点视觉已经由用户实机确认封版；按钮动画仍待本轮实机。
    # 因此这里将“鼠标视觉冻结”和“点击事务完整性”分成两个独立机械结果，避免以后再次混写验收状态。
    if focus_visual_frozen_ok and focus_offset_ok:
        result.ok("state5五内鼠标焦点视觉用户PASS冻结", "inner_focus_anchor + inner_show_node_focus 与refactor26b字节片段SHA一致；固定+21/-36右上偏移")
    else:
        result.fail("state5五内鼠标焦点视觉用户PASS冻结", f"视觉片段SHA={focus_visual_sha}，焦点偏移={focus_offset_ok}")

    if mouse_history_sync_ok and pulse_transaction_completion_ok and idle_restore_ok and direct_sprite_state_write_absent:
        result.ok("state5五内按钮动画事务护栏", "press/release历史逐帧同步；release后下一state5游戏帧调用原版0x43E6F0(0)恢复idle；不直接写Sprite动画状态")
    else:
        result.fail("state5五内按钮动画事务护栏", f"鼠标历史同步={mouse_history_sync_ok}，事务必完成={pulse_transaction_completion_ok}，release后idle恢复={idle_restore_ok}，无直接Sprite状态写={direct_sprite_state_write_absent}")

    if idle_restore_ok and direct_sprite_state_write_absent:
        result.ok("state5五内release后原版idle根修", "synthetic release只负责让RPG.exe播放释放反馈；下一次同Button扫描调用0x43E6F0(0)回到构造时idle基线")
    else:
        result.fail("state5五内release后原版idle根修", f"idle恢复链={idle_restore_ok}，无直接Sprite状态写={direct_sprite_state_write_absent}")

    # refactor23：Battle Target 和阵形只共用二维“找邻居”，不能共用业务状态。
    spatial_text = read_utf8(src / "spatial_neighbor.c") + read_utf8(src / "spatial_neighbor.h")
    battle_text_spatial = read_utf8(src / "battle.c")
    spatial_required = [
        "SpatialNeighbor_Find", "SPATIAL_LEFT", "SPATIAL_RIGHT", "SPATIAL_UP", "SPATIAL_DOWN",
        "case SPATIAL_LEFT:  return dx < 0", "case SPATIAL_RIGHT: return dx > 0",
        "case SPATIAL_UP:    return dy < 0", "case SPATIAL_DOWN:  return dy > 0",
        "secondary = spatial_abs(dy)", "secondary = spatial_abs(dx)",
        "target_spatial_direction", "missing_positions", "point_count",
        "if (best < 0 && !missing_positions) return",
    ]
    spatial_missing = [token for token in spatial_required if token not in (spatial_text + battle_text_spatial)]
    strict_cone_absent = not any(token in spatial_text for token in ["ax >= ay", "ay > ax", "45° 互斥扇区"])
    old_crossed_grid_absent = not any(token in battle_text_spatial for token in [
        "dx < 0 && dr < 0", "dx > 0 && dr > 0", "dy < 0 && dc < 0", "dy > 0 && dc > 0",
    ])
    legality_preserved = all(token in battle_text_spatial for token in [
        "target_build_cells", "target_cell_is_legal", "target_mode_bounds",
    ])
    if not spatial_missing and strict_cone_absent and old_crossed_grid_absent and legality_preserved:
        result.ok("Battle Target用户PASS成熟屏幕方向", "恢复r22成熟屏幕规则：方向半平面 + 副轴对齐优先；缺坐标只降级缺口，不再整组切网格；旧交叉轴fallback已移除；dev15合法集保持")
    else:
        result.fail("Battle Target用户PASS成熟屏幕方向", f"缺少={spatial_missing}，严格45度扇区已移除={strict_cone_absent}，旧交叉轴已移除={old_crossed_grid_absent}，合法集保留={legality_preserved}")

    # state6 阵形必须只把 D-Pad/A/B 映射到原版8阵位 Event；不得直接写交换字段/角色映射。
    formation_text = read_utf8(src / "interface_formation.c")
    formation_header = read_utf8(src / "interface_formation.h")
    formation_joined = formation_text + formation_header + router + bridge_text + runtime_shell_text + addresses_text + spatial_text
    formation_required = [
        "Runtime_InterfaceFormationProtocolOk", "UiBridge_InstallInterfaceFormationHooks",
        "CALL_INTERFACE_FORMATION_EVENT", "SIG_INTERFACE_FORMATION_UPDATE",
        "INPUT_CTX_INTERFACE_FORMATION", "formation_build_points", "SpatialNeighbor_Find",
        "formation_confirm", "formation_cancel_swap", "UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE",
        "INTERFACE_FORMATION_SLOT_COUNT", "Cursor_ShowMenuFocusAt", "UiBridge_GetButtonScreenCenter",
        "INTERFACE_FORMATION_LAYOUT_X_TABLE", "INTERFACE_FORMATION_LAYOUT_Y_TABLE",
        "formation_owns_directional_input", "formation_consume_owned_directions",
        "InterfaceShell_DiscardQueuedRoleActions",
    ]
    formation_missing = [token for token in formation_required if token not in formation_joined]
    formation_policy = re.search(r"static const InputPolicy\s+g_policy_interface_formation\s*=\s*\{\{(.*?)\}\};", router, flags=re.S)
    formation_policy_values = re.findall(r"INPUT_(?:PASS|MERGE|OVERRIDE|CONSUME)", formation_policy.group(1)) if formation_policy else []
    formation_policy_ok = (
        len(formation_policy_values) == 15 and
        formation_policy_values[0] == "INPUT_OVERRIDE" and
        formation_policy_values[1] == "INPUT_MERGE" and
        formation_policy_values[2] == "INPUT_CONSUME" and formation_policy_values[3] == "INPUT_CONSUME" and
        all(formation_policy_values[i] == "INPUT_OVERRIDE" for i in (4,5,6,7)) and
        formation_policy_values[8] == "INPUT_PASS" and formation_policy_values[9] == "INPUT_PASS" and
        formation_policy_values[10] == "INPUT_CONSUME" and formation_policy_values[11] == "INPUT_CONSUME"
    )
    formation_code_no_comments = re.sub(r"/\*.*?\*/|//[^\n]*", "", formation_text, flags=re.S)
    forbidden_formation_assignments = []
    for macro in ["INTERFACE_FORMATION_SWAP_PENDING", "INTERFACE_FORMATION_SOURCE_SLOT", "INTERFACE_FORMATION_TARGET_SLOT", "INTERFACE_FORMATION_ROLE_MAP"]:
        if re.search(rf"\+\s*{macro}[^\n]*\)\s*=", formation_code_no_comments):
            forbidden_formation_assignments.append(macro)
    # 用 RPG.exe 原版 8 阵位表做一个固定拓扑回归。这个测试不是猜玩法，而是确认 shared 算法
    # 在错列布局上至少能产生稳定的四向邻居，不再出现 r23 “↑/↓大量返回-1”的45度扇区回归。
    formation_points = [
        (0, 227, 28), (1, 171, 56), (2, 115, 84), (3, 59, 112),
        (4, 283, 56), (5, 227, 84), (6, 171, 112), (7, 115, 142),
    ]
    def expected_neighbor(current_id, direction):
        cur = next((p for p in formation_points if p[0] == current_id), None)
        if cur is None:
            return None
        best = None
        for point in formation_points:
            if point[0] == current_id:
                continue
            dx, dy = point[1] - cur[1], point[2] - cur[2]
            if direction == "L" and not dx < 0: continue
            if direction == "R" and not dx > 0: continue
            if direction == "U" and not dy < 0: continue
            if direction == "D" and not dy > 0: continue
            if direction in ("L", "R"):
                key = (abs(dy), abs(dx), point[0])
            else:
                key = (abs(dx), abs(dy), point[0])
            if best is None or key < best[0]:
                best = (key, point[0])
        return None if best is None else best[1]
    topology = {i: {d: expected_neighbor(i, d) for d in "LRUD"} for i in range(8)}
    formation_topology_ok = (
        topology[0] == {"L": 1, "R": 4, "U": None, "D": 5} and
        topology[1]["U"] == 0 and topology[1]["D"] == 6 and
        topology[2]["U"] == 1 and topology[2]["D"] == 7 and
        topology[7]["U"] == 2 and topology[7]["D"] is None
    )
    if not formation_missing and formation_policy_ok and not forbidden_formation_assignments and formation_topology_ok:
        result.ok("state6阵形页面Adapter", "原版8项布局表作为拓扑 + 完整Button屏幕中心显示鼠标；四方向硬所有权并清旧换人队列；A/B仍走原版交换事务；错列拓扑固定回归PASS")
    else:
        result.fail("state6阵形页面Adapter", f"缺少={formation_missing}，策略={formation_policy_ok}，拓扑={formation_topology_ok}，疑似直接写={forbidden_formation_assignments}")

    # 全局序列方向硬规则：上一项一律是 LB/LT/←，下一项一律是 RB/RT/→。
    # refactor29 的炼化实机暴露出一个典型坑：原版 category native index 增长方向与画面左右相反。
    # 因此方向语义本身绝不能改，必须由炼化 Adapter 把视觉方向换算成相反的原版 index。
    synthesis_direction_text = read_utf8(src / "synthesis.c")
    synthesis_direction_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", synthesis_direction_text, flags=re.S)
    direction_required = [
        "INPUT_CATEGORY_PREV", "INPUT_CATEGORY_NEXT", "INPUT_SUBTYPE_PREV", "INPUT_SUBTYPE_NEXT",
        "items_change_subtype(page, -1)", "items_change_subtype(page, 1)",
        "skills_change_subtype(page, -1)", "skills_change_subtype(page, 1)",
        "direction<0 永远代表视觉左侧/上一项",
        "target = current - delta",
        "原版类别字段编号方向与画面左右相反",
    ]
    direction_text = router + items_text + skills_text + shell_text + synthesis_direction_text
    direction_missing = [token for token in direction_required if token not in direction_text]
    synthesis_reversed_again = "target = current + delta" in synthesis_direction_code
    if not direction_missing and not synthesis_reversed_again:
        result.ok("全局序列方向硬规则", "LB/LT/←=上一项/左/减；RB/RT/→=下一项/右/加；炼化反向native index由Adapter换算")
    else:
        result.fail("全局序列方向硬规则", "缺少=" + ", ".join(direction_missing) + ("；炼化又出现current+delta反向映射" if synthesis_reversed_again else ""))

    # Battle/Frontend 的接管日志只允许在“接管前确实还有手柄强制焦点”时写。
    # 这样持续移动实体鼠标或右摇杆不会每个 worker tick 重复堆同一句日志。
    frontend_text = read_utf8(src / "frontend.c")
    log_dedup_required = [
        "had_forced_focus = g_nav_active || g_visual_latch_active || g_return_visual.active",
        "if (had_forced_focus)",
        "had_forced_focus = g_front.title_nav_active || SaveSlot_IsControllerActive()",
    ]
    combined_takeover = battle_text + "\n" + frontend_text
    log_dedup_missing = [token for token in log_dedup_required if token not in combined_takeover]
    if not log_dedup_missing:
        result.ok("Battle/Frontend 实体鼠标接管日志状态去重", "只在手柄强制焦点→鼠标接管的状态变化时各记录一次")
    else:
        result.fail("Battle/Frontend 实体鼠标接管日志状态去重", f"缺少={log_dedup_missing}")


    # refactor27：state7 必须是真正复用 SaveSlot，而不是复制一套槽位；三项窗口与二次 Yes/No 有明确分层。
    tome_source = read_utf8(src / "interface_tome.c")
    tome_text = tome_source + read_utf8(src / "interface_tome.h") + read_utf8(src / "save_slot.c") + bridge_text + runtime_shell_text + plugin_text
    tome_required = [
        "Runtime_InterfaceTomeProtocolOk", "Runtime_SaveSlotActionProtocolOk", "SaveSlot_InstallActionHooks", "SaveSlot_Begin", "SaveSlot_Update", "SaveSlot_End",
        "SAVE_VIEW_ACTION", "SAVE_VIEW_ACTION_POPUP", "INPUT_CTX_SAVE_ACTION", "CALL_SAVE_ACTION_HIT", "CALL_SAVE_ACTION_EVENT",
        "InterfaceTome_ModalActive", "SaveSlot_DetectView",
    ]
    tome_missing = [token for token in tome_required if token not in tome_text]
    tome_forbidden = [token for token in ["fopen(", "CreateFile", "WriteFile", "SAVE_SELECTED_ROW) =", "SAVE_PAGE_BASE) ="] if token in tome_source]
    action_owner_coupled = "SaveSlot_InstallActionHooks()" in tome_source
    if not tome_missing and not tome_forbidden and not action_owner_coupled:
        result.ok("state7天书页面Adapter", "只管理state7 owner；共享三项动作Hook不再依赖天书；不直接读写存档文件/槽位字段")
    else:
        result.fail("state7天书页面Adapter", f"缺少={tome_missing}，禁止直接业务={tome_forbidden}，动作Hook仍耦合天书={action_owner_coupled}")

    # refactor31：原版 Oracle 证明地图存档点不是 Interface state7。
    # 事件 opcode 0x3E 会发布 0x89FCD0 独立包装层，而包装层 +0x580 才是标准 SaveSlot。
    # refactor32：三项动作窗口属于共享 SaveSlot；其二次 Yes/No 必须在包装层总模态内继续分发，
    # 前置剧情 mode=3 的 LEFTDOWN 也必须按时释放，但不得在存档页背后继续读取新选择输入。
    save_point_source = read_utf8(src / "save_point.c")
    save_point_header = read_utf8(src / "save_point.h")
    save_point_plugin = read_utf8(src / "plugin.c")
    save_point_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", save_point_source, flags=re.S)
    save_point_combined = (
        save_point_source + save_point_header + read_utf8(src / "save_slot.c") +
        read_utf8(src / "save_slot.h") + read_utf8(src / "confirm_dialog.c") +
        save_point_plugin + addresses_text + read_utf8(src / "runtime.c") + read_utf8(src / "runtime.h")
    )
    save_point_required = [
        "GLOBAL_SAVE_POINT_UI", "SAVE_POINT_SLOT", "SAVE_POINT_PHASE", "Runtime_SavePointProtocolOk",
        "SavePoint_Install", "SavePoint_Update", "SavePoint_Active", "SavePoint_OnPointerTakeover",
        "save = *(u8**)(wrapper + SAVE_POINT_SLOT)", "phase != SAVE_POINT_PHASE_STABLE",
        "SaveSlot_Begin(save", "SaveSlot_Update(save)", "SaveSlot_End()", "SaveSlot_OwnsDirectPopup",
        "Runtime_SaveSlotActionProtocolOk", "action_hooks_enabled", "SAVE_VIEW_ACTION", "SAVE_VIEW_ACTION_POPUP",
        "SavePoint_Update();", "save_point_active = SavePoint_Active()", "if (save_point_active)",
        "ConfirmDialog_Update();", "SceneChoice_MaintainPulse();",
        "CALL_MAP_SAVE_POINT_WRAPPER_CTOR", "CALL_SAVE_POINT_SLOT_CTOR",
        "SIG_SAVE_POINT_WRAPPER_PUBLISH", "SIG_SAVE_POINT_WRAPPER_BIND", "SIG_SAVE_POINT_WRAPPER_UPDATE",
    ]
    save_point_missing = [token for token in save_point_required if token not in save_point_combined]
    save_point_forbidden = [
        token for token in [
            "fopen(", "CreateFile", "WriteFile", "SAVE_SELECTED_ROW", "SAVE_PAGE_BASE",
            "SAVE_PAGE_PREV_BUTTON", "SAVE_PAGE_NEXT_BUTTON", "UiBridge_RequestEventOwned",
            "FN_SAVE_SLOT_REFRESH",
        ] if token in save_point_code
    ]
    save_slot_update_defs = 0
    for c_path in src.glob("*.c"):
        c_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", read_utf8(c_path), flags=re.S)
        save_slot_update_defs += len(re.findall(r"\bvoid\s+SaveSlot_Update\s*\(", c_code))
    tome_code_only = re.sub(r"/\*.*?\*/|//[^\n]*", "", read_utf8(src / "interface_tome.c"), flags=re.S)
    owner_boundary_ok = "GLOBAL_SAVE_POINT_UI" not in tome_code_only and "INTERFACE_PAGE_7" not in save_point_code
    slot_install_pos = save_point_plugin.find("if (!SaveSlot_InstallHooks()) return 0;")
    action_install_token = "if (!SaveSlot_InstallActionHooks()) return 0;"
    action_install_pos = save_point_plugin.find(action_install_token)
    point_install_pos = save_point_plugin.find("if (!SavePoint_Install()) return 0;")
    tome_install_pos = save_point_plugin.find("if (!InterfaceTome_InstallHooks()) return 0;")
    update_pos = save_point_plugin.find("SavePoint_Update();")
    active_pos = save_point_plugin.find("save_point_active = SavePoint_Active()")
    gate_pos = save_point_plugin.find("if (save_point_active)")
    deep_confirm_pos = save_point_plugin.find("ConfirmDialog_Update();", gate_pos)
    pulse_maintain_pos = save_point_plugin.find("SceneChoice_MaintainPulse();", gate_pos)
    branch_else_pos = save_point_plugin.find("} else {", gate_pos)
    first_lower_context_pos = save_point_plugin.find("InterfaceItems_Update();")
    save_point_branch = save_point_plugin[gate_pos:branch_else_pos] if 0 <= gate_pos < branch_else_pos else ""
    deep_modal_strict_ok = (
        "ConfirmDialog_Update();" in save_point_branch and
        "SceneChoice_MaintainPulse();" in save_point_branch and
        "SceneChoice_Update();" not in save_point_branch and
        "DialogueInput_Update();" not in save_point_branch
    )
    action_install_once = save_point_plugin.count(action_install_token) == 1
    dispatch_order_ok = (
        0 <= slot_install_pos < action_install_pos < point_install_pos < tome_install_pos and
        action_install_once and
        0 <= update_pos < active_pos < gate_pos < deep_confirm_pos < pulse_maintain_pos < branch_else_pos < first_lower_context_pos and
        deep_modal_strict_ok
    )
    update_block_start = save_point_source.find("void SavePoint_Update(void)")
    update_block_end = save_point_source.find("void SavePoint_OnPointerTakeover", update_block_start)
    update_block = save_point_source[update_block_start:update_block_end]
    protocol_gate_pos = update_block.find("if (!g_save_point.enabled)")
    child_read_pos = update_block.find("save = save_point_inner_slot(wrapper)")
    active_block_start = save_point_source.find("int SavePoint_Active(void)")
    active_block_end = save_point_source.find("static void save_point_end_session", active_block_start)
    active_block = save_point_source[active_block_start:active_block_end]
    fail_closed_layout_ok = (
        0 <= protocol_gate_pos < child_read_pos and
        "save_point_inner_slot" not in active_block and
        "if (!wrapper) return 0;" in active_block
    )
    if (
        not save_point_missing and not save_point_forbidden and save_slot_update_defs == 1 and
        owner_boundary_ok and dispatch_order_ok and fail_closed_layout_ok
    ):
        result.ok("存档点完整SaveSlot/深层询问边界", "wrapper只拆包；共享槽位+三项动作独立安装；直接确认由SaveSlot处理，二次Yes/No在总模态内交ConfirmDialog；剧情LEFTUP只维护不穿透")
    else:
        result.fail(
            "存档点完整SaveSlot/深层询问边界",
            f"缺少={save_point_missing}，禁止业务={save_point_forbidden}，SaveSlot_Update定义数={save_slot_update_defs}，owner边界={owner_boundary_ok}，调度/安装顺序={dispatch_order_ok}，深层严格分支={deep_modal_strict_ok}，动作安装一次={action_install_once}，fail-closed布局读取={fail_closed_layout_ok}",
        )

    # refactor27：state8 只允许通过五个真实 ButtonEvent 改设置；插件不得直接改音乐/音效数值或空明流转状态。
    options_text = read_utf8(src / "interface_options.c") + read_utf8(src / "interface_options.h") + bridge_text + runtime_shell_text + addresses_text
    options_required = [
        "Runtime_InterfaceOptionsProtocolOk", "UiBridge_InstallInterfaceOptionsHooks", "INPUT_CTX_INTERFACE_OPTIONS",
        "INTERFACE_OPTIONS_MUSIC_DEC", "INTERFACE_OPTIONS_MUSIC_INC", "INTERFACE_OPTIONS_SOUND_DEC", "INTERFACE_OPTIONS_SOUND_INC",
        "INTERFACE_OPTIONS_KARMA_BUTTON", "UiBridge_RequestEventOwned(UI_EVENT_OWNER_INTERFACE", "UiBridge_GetButtonScreenRect",
        "right + OPTIONS_FOCUS_GAP_X", "OPTIONS_FOCUS_OFFSET_Y", "options_popup_active",
    ]
    options_missing = [token for token in options_required if token not in options_text]
    options_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", read_utf8(src / "interface_options.c"), flags=re.S)
    options_direct_writes = [token for token in ["INTERFACE_OPTIONS_MUSIC_VALUE) =", "INTERFACE_OPTIONS_SOUND_VALUE) =", "INTERFACE_OPTIONS_POPUP) ="] if token in options_code]
    if not options_missing and not options_direct_writes:
        result.ok("state8机能页面Adapter", "refactor27业务PASS保持；焦点X仍在按钮矩形外并微调左移，Y小幅下移；LT/RT/A仍只走原版Event，无直接数值写")
    else:
        result.fail("state8机能页面Adapter", f"缺少={options_missing}，疑似直接写={options_direct_writes}")

    # refactor29：新增客栈/炼化/剧情选择必须保持各自原版业务边界。
    inn_text = read_utf8(src / "inn.c")
    synthesis_text = read_utf8(src / "synthesis.c")
    scene_choice_text = read_utf8(src / "scene_choice.c")
    ui_bridge_r29 = read_utf8(src / "ui_bridge.c")
    plugin_r29 = read_utf8(src / "plugin.c")

    inn_required = [
        "UI_EVENT_OWNER_INN", "Inn_FilterButtonHit", "INPUT_CTX_INN_ROOT",
        "INPUT_NAV_UP", "INPUT_NAV_DOWN", "INPUT_CONFIRM",
    ]
    inn_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", inn_text, flags=re.S)
    inn_forbidden = ["VK_RBUTTON", "MOUSEEVENTF_RIGHT", "mouse_event("]
    inn_ok = all(t in inn_text for t in inn_required) and not any(t in inn_code for t in inn_forbidden)
    # 客栈根层允许读取/消费 B 防止穿透，但绝不能拿 B 请求任何原版 ButtonEvent。
    inn_cancel_business = re.search(
        r"INPUT_CANCEL.*?UiBridge_RequestEventOwned\s*\(\s*UI_EVENT_OWNER_INN",
        inn_code,
        flags=re.S,
    )
    if inn_ok and not inn_cancel_business:
        result.ok("refactor29客栈根菜单边界", "↑/↓ + A只操作三只真实Button；B仅防穿透，没有凭空制造退出/RMB")
    else:
        result.fail("refactor29客栈根菜单边界", "客栈Adapter缺真实Button语义，或B/RMB越过原版能力边界")

    synthesis_required = [
        "UI_EVENT_OWNER_SYNTHESIS", "SYNTHESIS_SECONDARY_CANCEL_BUTTON",
        "synthesis_top_button(owner, 0)", "INPUT_CTX_SYNTHESIS_PRIMARY",
        "INPUT_CTX_SYNTHESIS_SECONDARY", "INPUT_CATEGORY_PREV", "INPUT_CATEGORY_NEXT",
        "UiBridge_InstallSynthesisHooks", "不可炼化时由原版自行拒绝",
    ]
    synthesis_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", synthesis_text, flags=re.S)
    synthesis_forbidden = ["VK_RBUTTON", "MOUSEEVENTF_RIGHT", "mouse_event(", "SYNTHESIS_ELIG"]
    synth_ok = all(t in (synthesis_text + ui_bridge_r29) for t in synthesis_required)
    synth_ok = synth_ok and not any(t in synthesis_code for t in synthesis_forbidden)
    if synth_ok:
        result.ok("refactor29炼化两层边界", "根层B=用器右侧退出图标；第二层B=专属取消Button；不可炼化A由RPG.exe原版自行拒绝；无RMB")
    else:
        result.fail("refactor29炼化两层边界", "缺真实退出/取消链，或出现RMB/自行复制可炼化资格逻辑")

    # refactor33：地图十字键只增加“八方向步行”这一种来源，不能篡改左摇杆既有全向走跑阈值。
    exploration_text = read_utf8(src / "exploration.c")
    exploration_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", exploration_text, flags=re.S)
    map_walk_required = [
        "exploration_dpad_to_direction", "InputRouter_DownOn(INPUT_CTX_EXPLORATION",
        "INPUT_NAV_LEFT", "INPUT_NAV_RIGHT", "INPUT_NAV_UP", "INPUT_NAV_DOWN",
        "g_move_force_walk", "desired_run = g_move_force_walk ? 0 : exploration_left_stick_wants_run()",
        "g_move_force_walk = 1", "g_move_force_walk = 0",
        "exploration_stick_to_direction(PadInput_Axis(PAD_AXIS_LEFT_X), PadInput_Axis(PAD_AXIS_LEFT_Y))",
    ]
    map_walk_missing = [token for token in map_walk_required if token not in exploration_text]
    direction_codes_ok = all(f"0x{code:02x}" in exploration_text.lower() for code in range(0x21, 0x29))
    # 物理 D-Pad 编号不得泄漏到业务模块；十字键只能从 InputRouter 语义层进入。
    map_walk_forbidden = [token for token in ["PAD_DPAD_UP", "PAD_DPAD_DOWN", "PAD_DPAD_LEFT", "PAD_DPAD_RIGHT"] if token in exploration_code]
    dpad_assign = exploration_text.find("dpad_code = exploration_dpad_to_direction()")
    stick_assign = exploration_text.rfind("g_move_code = exploration_stick_to_direction")
    dpad_priority_ok = 0 <= dpad_assign < stick_assign and "if (dpad_code != 0)" in exploration_text[dpad_assign:stick_assign]
    if not map_walk_missing and direction_codes_ok and not map_walk_forbidden and dpad_priority_ok:
        result.ok("refactor33地图十字键仅步行", "四方向语义可组合为原版0x21..0x28八方向；十字键优先且force_walk=1，松开恢复左摇杆径向走跑")
    else:
        result.fail("refactor33地图十字键仅步行", f"缺少={map_walk_missing}，八方向码={direction_codes_ok}，物理键泄漏={map_walk_forbidden}，优先级={dpad_priority_ok}")

    # refactor36：保留 refactor33 的全商店主体入口，修正 r35 信息 Hook 重复所有权，
    # 并让上下越界和主 Interface/Battle 一样继续走原版分页事务。
    shop_text = read_utf8(src / "shop.c")
    shop_header = read_utf8(src / "shop.h")
    shop_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", shop_text, flags=re.S)
    shop_required = [
        "Shop_InstallHooks", "Runtime_ShopProtocolOk", "UiBridge_InstallShopHooks", "UI_EVENT_OWNER_SHOP",
        "SHOP_SIDE_BUY", "SHOP_SIDE_SELL", "shop_switch_side", "INPUT_SPECIAL_X",
        "shop_show_column_marker", "UiBridge_GetButtonScreenRect",
        "x = right - width / 6 - 10", "y = top - height + 5",
        "shop_active_owner", "shop_hide_own_marker", "marker_visible", "item_info_enabled",
        "INPUT_NAV_UP", "INPUT_NAV_DOWN", "INPUT_NAV_LEFT", "INPUT_NAV_RIGHT",
        "INPUT_CATEGORY_PREV", "INPUT_CATEGORY_NEXT", "target = current - visual_delta",
        "shop_top_button(owner, 0)", "SHOP_QUANTITY_DEC_BUTTON", "SHOP_QUANTITY_INC_BUTTON",
        "SHOP_QUANTITY_CONFIRM_BUTTON", "SHOP_QUANTITY_CANCEL_BUTTON",
        "shop_sync_popup_selection", "Cursor_MoveHiddenSelectionAt",
        "SHOP_ITEM_INFO", "SHOP_ITEM_INFO_MODE", "SHOP_ITEM_INFO_CURRENT_POSITION",
        "SHOP_ITEM_INFO_CLOSED_POSITION", "SHOP_ITEM_INFO_OPEN_POSITION",
        "SHOP_ITEM_INFO_CLOSE_BUTTON", "INPUT_SPECIAL_Y",
        "SHOP_ROW_INTENT_NAVIGATION", "SHOP_ROW_INTENT_ITEM_INFO", "Shop_HookItemInfoTransition",
        "CALL_SHOP_SELL_ITEM_INFO_OPEN", "CALL_SHOP_BUY_ITEM_INFO_OPEN", "FN_SHOP_ITEM_INFO_TRANSITION",
        "CALL_SHOP_ITEM_INFO_CLOSE_EVENT", "shop_handle_item_info", "shop_item_info_state",
        "Runtime_ShopItemInfoProtocolOk", "shop_finish_page_landing", "shop_clear_page_wait",
        "SHOP_PAGE_LAND_FIRST", "SHOP_PAGE_LAND_LAST", "SHOP_PAGE_LAND_PRESERVE",
        "page_before_request", "page_wait_start_tick", "page_change_observed",
        "INPUT_SUBTYPE_PREV", "INPUT_SUBTYPE_NEXT", "INPUT_CTX_SHOP_ROOT", "INPUT_CTX_SHOP_QUANTITY",
        "shop_consume_modal_actions", "Shop_OnPointerTakeover", "Cursor_HideMenuFocusImmediately",
    ]
    shop_all = shop_text + shop_header + ui_bridge_r29 + runtime_shell_text + addresses_text + plugin_r29 + router
    shop_missing = [token for token in shop_required if token not in shop_all]
    shop_forbidden = [token for token in ["VK_RBUTTON", "MOUSEEVENTF_RIGHT", "mouse_event("] if token in shop_code]
    shop_direct_writes = re.findall(
        r"\*\s*\(\s*(?:u8|u32|i32)\s*\*\s*\)\s*\(\s*(?:owner|popup|list)\s*\+\s*(SHOP_[A-Z0-9_]+)\s*\)\s*=(?!=)",
        shop_code,
    )
    bridge_shop_calls = len(re.findall(r"Runtime_PatchCall\s*\(\s*CALL_SHOP_", ui_bridge_r29))
    bridge_shop_ok = bridge_shop_calls == 11
    shared_info_event_ok = (
        len(re.findall(r"Runtime_PatchCall\s*\(\s*CALL_CMD0_EVENT", ui_bridge_r29)) == 1 and
        "Runtime_PatchCall(CALL_SHOP_ITEM_INFO_CLOSE_EVENT" not in ui_bridge_r29 and
        "UiBridge_InstallShopItemInfoHooks" not in (ui_bridge_r29 + shop_text + shop_header) and
        "#define CALL_SHOP_ITEM_INFO_CLOSE_EVENT  CALL_CMD0_EVENT" in addresses_text
    )
    transition_hook_count = len(re.findall(
        r"Runtime_PatchCall\s*\(\s*CALL_SHOP_(?:SELL|BUY)_ITEM_INFO_OPEN",
        shop_text,
    ))
    transition_hooks_ok = transition_hook_count == 2
    row_codes_ok = (
        "target, 1, SHOP_ROW_INTENT_NAVIGATION" in shop_text and
        "row, 2, SHOP_ROW_INTENT_NONE" in shop_text
    )
    popup_focus_ok = (
        "g_shop.popup_focus = SHOP_POPUP_CONFIRM" in shop_text and
        "g_shop.popup_focus = SHOP_POPUP_CANCEL" in shop_text and
        "shop_sync_popup_selection(popup)" in shop_text and
        "Cursor_MoveHiddenSelectionAt(x, y)" in shop_text and
        "shop_show_popup_marker" not in shop_text
    )
    shop_policy_match = re.search(r"static const InputPolicy g_policy_shop_root = \{\{(.*?)\}\};", router, flags=re.S)
    shop_policy_values = re.findall(r"INPUT_(?:PASS|MERGE|OVERRIDE|CONSUME)", shop_policy_match.group(1)) if shop_policy_match else []
    y_policy_ok = len(shop_policy_values) == 15 and shop_policy_values[3] == "INPUT_OVERRIDE"
    item_info_ok = all(token in shop_text for token in [
        "if (!g_shop.item_info_enabled) return SHOP_ITEM_INFO_UNAVAILABLE",
        "intent == SHOP_ROW_INTENT_NAVIGATION", "intent == SHOP_ROW_INTENT_ITEM_INFO",
        "current_position == open_position", "current_position == closed_position",
        "info_state == SHOP_ITEM_INFO_OPEN", "info_state == SHOP_ITEM_INFO_CLOSING",
        "shop_handle_item_info(owner)",
        "UiBridge_RequestEventOwned(UI_EVENT_OWNER_SHOP, close_button, 2",
        "shop_request_row_event(owner, g_shop.focus_side, row, 1, SHOP_ROW_INTENT_ITEM_INFO",
    ])
    page_continuity_ok = all(token in shop_text for token in [
        "delta < 0 ? SHOP_PAGE_LAND_LAST : SHOP_PAGE_LAND_FIRST",
        "target_row = shop_first_present_row(list)", "target_row = shop_last_present_row(list)",
        "landing == SHOP_PAGE_LAND_PRESERVE", "current_page == g_shop.page_before_request",
        "Runtime_MsToTicks(1000u)", "shop_clear_page_wait();",
        "shop_request_row_event(owner, side, target_row, 1, SHOP_ROW_INTENT_NAVIGATION",
        "SHOP_PAGE_LAND_PRESERVE, row",
    ])
    active_owner_match = re.search(r"static u8\* shop_active_owner\(void\)\s*\{(.*?)\n\}", shop_text, flags=re.S)
    active_owner_body = active_owner_match.group(1) if active_owner_match else ""
    core_entry_ok = (
        "owner = shop_owner();" in active_owner_body and
        "*(u8*)(owner + SHOP_ACTIVE) == 0" in active_owner_body and
        not any(token in active_owner_body for token in [
            "SHOP_VTABLE", "shop_topbar(", "shop_list(", "shop_quantity_popup(", "shop_item_info("
        ]) and
        "SHOP_VTABLE" not in shop_text and "SIG_SHOP_CTOR_VTABLE" not in addresses_text
    )
    optional_start = shop_text.find("g_shop.item_info_enabled = 0;")
    optional_end = shop_text.find("g_shop.enabled = 1;", optional_start)
    optional_block = shop_text[optional_start:optional_end] if 0 <= optional_start < optional_end else ""
    optional_isolation_ok = all(token in optional_block for token in [
        "Runtime_ShopItemInfoProtocolOk()",
        "g_shop.item_info_enabled = 1", "r33 商店主体继续启用"
    ]) and "g_shop.enabled = 0" not in optional_block and "UiBridge_InstallShopItemInfoHooks" not in optional_block
    boundary_ok = all(token in shop_text for token in [
        "shop_topbar(owner)", "shop_list(owner, g_shop.focus_side)",
        "shop_quantity_popup(owner)", "shop_item_info(owner)",
        "shop_hide_own_marker();", "shop_clear_row_intent();", "shop_clear_page_wait();",
    ])
    plugin_shop_order_ok = (
        "if (!Shop_InstallHooks()) return 0;" in plugin_r29 and
        "Shop_OnPointerTakeover(takeover);" in plugin_r29 and
        0 <= plugin_r29.find("Shop_Update();") < plugin_r29.find("Inn_Update();")
    )
    if (
        not shop_missing and not shop_forbidden and not shop_direct_writes and bridge_shop_ok and shared_info_event_ok and
        transition_hooks_ok and row_codes_ok and popup_focus_ok and y_policy_ok and item_info_ok and
        page_continuity_ok and core_entry_ok and optional_isolation_ok and boundary_ok and plugin_shop_order_ok
    ):
        result.ok("refactor36商店连续跨页与Y说明修正", "r33统一商店入口与11 Event保持；上下边界连续翻页；信息窗复用公共Event且按原版位置识别稳态；列标记左10/下5")
    else:
        result.fail(
            "refactor36商店连续跨页与Y说明修正",
            f"缺少={shop_missing}，禁止输入={shop_forbidden}，疑似字段直写={shop_direct_writes}，主体Bridge11={bridge_shop_ok}，共享信息Event={shared_info_event_ok}，transition2={transition_hooks_ok}，连续分页={page_continuity_ok}，r33入口={core_entry_ok}，可选隔离={optional_isolation_ok}，行code1/2={row_codes_ok}，数量焦点={popup_focus_ok}，Y策略={y_policy_ok}，信息窗={item_info_ok}，边界={boundary_ok}，安装/调度={plugin_shop_order_ok}",
        )

    # refactor32：焦点来源由客栈父层在进入诸态/炼化前记录，不能由炼化子模块反向要求固定回首项。
    # 同一 owner 必须先真实 inactive，再在根层重新 active 时把逻辑和既有 HitTest 视觉恢复到入口项目。
    inn_header = read_utf8(src / "inn.h")
    origin_required = [
        "session_owner", "return_focus", "return_armed", "child_seen_inactive",
        "inn_arm_return_to_origin", "g_inn.return_focus = g_inn.focus",
        "g_inn.return_armed = 1", "if (g_inn.return_armed) g_inn.child_seen_inactive = 1",
        "if (g_inn.return_armed && g_inn.child_seen_inactive)",
        "g_inn.focus = g_inn.return_focus", "inn_claim_navigation()",
        "g_inn.focus != 0 && g_inn.focus != 1", "g_inn.session_owner != owner",
        "子界面原位返回",
    ]
    origin_text = synthesis_text + inn_text + inn_header
    origin_missing = [token for token in origin_required if token not in origin_text]
    origin_forbidden = [
        token for token in [
            "Cursor_ShowMenuFocusAt", "Cursor_MoveHiddenSelectionAt", "SetCursorPos(",
            "FN_BUTTON_SPRITE_FEEDBACK", "Inn_RequestRootFocusRestoreFromChild",
            "root_exit_requested_by_pad", "restore_focus_pending",
        ] if token in (synthesis_code + inn_code)
    ]
    arm_call_count = len(re.findall(r"inn_arm_return_to_origin\s*\(\s*\)\s*;", inn_code))
    synthesis_parent_coupling = '#include "inn.h"' in synthesis_text or bool(re.search(r"\bInn_[A-Za-z0-9_]+", synthesis_code))
    generic_pointer_probe = "Cursor_ControllerOwnsPointer()" in inn_code
    restore_start = inn_text.find("if (g_inn.return_armed && g_inn.child_seen_inactive)")
    restore_end = inn_text.find("} else {", restore_start)
    restore_block = inn_text[restore_start:restore_end] if 0 <= restore_start < restore_end else ""
    restore_direction_ok = (
        "g_inn.focus = g_inn.return_focus" in restore_block and
        "inn_claim_navigation();" in restore_block and
        "g_inn.focus = 0" not in restore_block
    )
    pointer_cancel_ok = (
        "void Inn_OnPointerTakeover" in inn_text and
        "g_inn.return_armed = 0;" in inn_text[inn_text.find("void Inn_OnPointerTakeover"):]
    )
    if (
        not origin_missing and not origin_forbidden and arm_call_count == 1 and
        not synthesis_parent_coupling and not generic_pointer_probe and
        restore_direction_ok and pointer_cancel_ok
    ):
        result.ok("客栈子界面原位返回焦点", "父层记录诸态/炼化入口；同owner真实往返后逻辑+HitTest视觉共同恢复入口；子模块零耦合，鼠标可取消，无瞬移/Sprite私写")
    else:
        result.fail(
            "客栈子界面原位返回焦点",
            f"缺少={origin_missing}，禁止路径={origin_forbidden}，入口锁存调用数={arm_call_count}，子模块耦合={synthesis_parent_coupling}，恢复方向={restore_direction_ok}，鼠标取消={pointer_cancel_ok}，客栈通用指针探测={generic_pointer_probe}",
        )

    scene_required = [
        "GLOBAL_DIALOGUE_MODE", "SCENE_CHOICE_MODE_YES_NO", "Cursor_MoveHiddenSelectionAt",
        "GLOBAL_DIALOGUE_CHOICE_VISUAL_STATE", "GLOBAL_DIALOGUE_CHOICE_HOVER_STATE",
        "MOUSEEVENTF_LEFTDOWN_", "MOUSEEVENTF_LEFTUP_", "INPUT_CTX_SCENE_CHOICE",
        "Runtime_SceneChoiceProtocolOk", "SceneChoice_Update",
    ]
    scene_code = re.sub(r"/\*.*?\*/|//[^\n]*", "", scene_choice_text, flags=re.S)
    scene_ok = all(t in (scene_choice_text + plugin_r29 + read_utf8(src / "runtime.c")) for t in scene_required)
    scene_forbidden = [
        "0x0044B0B0", "0x44B0B0", "GLOBAL_DIALOGUE_CHOICE_HOVER_STATE =", "GLOBAL_DIALOGUE_MODE =",
        "Cursor_ShowMenuFocusAt",
    ]
    scene_ok = scene_ok and not any(t in scene_code for t in scene_forbidden)
    if scene_ok:
        result.ok("refactor29剧情mode=3两项选择边界", "隐藏鼠标只驱动原版选择框命中并发左键脉冲；A当前项、B选择否；不直接写剧情结果/不显示第二套手形焦点")
    else:
        result.fail("refactor29剧情mode=3两项选择边界", "缺mode=3精确协议/输入消费，或出现直接剧情结果写入")

    # 代码里出现 TODO/FIXME 往往意味着“交付前已经知道没做完却没有写进接档”。本包禁止这种隐性状态。
    todo_hits = []
    for path in src.glob("*.[ch]"):
        text = read_utf8(path)
        if re.search(r"\bTODO\b|\bFIXME\b", text, flags=re.IGNORECASE):
            todo_hits.append(path.name)
    if todo_hits:
        result.fail("源码无隐性 TODO/FIXME", ", ".join(todo_hits))
    else:
        result.ok("源码无隐性 TODO/FIXME")

    # 注释比例只是“最低机械护栏”，不能代替人工判断注释是否真的让初学者看懂。
    low_comment_files = []
    ratios = []
    for path in sorted(src.glob("*.[ch]")):
        lines = [line for line in read_utf8(path).splitlines() if line.strip()]
        comment_lines = sum(1 for line in lines if line.lstrip().startswith(("/*", "*", "//")))
        ratio = comment_lines / max(1, len(lines))
        ratios.append(f"{path.name}={ratio:.1%}")
        minimum = 0.10 if path.suffix == ".c" else 0.10
        if ratio < minimum:
            low_comment_files.append(f"{path.name}({ratio:.1%})")
    if low_comment_files:
        result.fail("源码注释最低覆盖率", ", ".join(low_comment_files))
    else:
        result.ok("源码注释最低覆盖率", "；".join(ratios))


def check_ini_and_build(root: Path, result: CheckResult) -> None:
    """检查用户侧配置兼容性和正式 Windows 构建规则。"""
    ini = root / "编译内容" / "Castle_PadSupport.ini"
    if not ini.is_file():
        result.fail("INI 配置", "Castle_PadSupport.ini 不存在")
    else:
        text = read_utf8(ini)
        found = set(re.findall(r"^([A-Za-z0-9_]+)\s*=", text, flags=re.MULTILINE))
        missing = sorted(EXPECTED_INI_KEYS - found)
        unexpected = sorted(found - EXPECTED_INI_KEYS)
        if missing or unexpected:
            result.fail("INI 公开键集合", f"缺少={missing}，新增/意外={unexpected}")
        else:
            result.ok("INI 公开键集合", f"{len(found)} 个公开键完整（Back/RT双杆速度 + LT精调/吸附 + 全局强度/独立时长）")

    build = root / "源码" / "build.bat"
    if not build.is_file():
        result.fail("Windows 构建脚本", "build.bat 不存在")
        return
    text = read_utf8(build)
    required_flags = ["/W4", "/WX", "/utf-8", "/machine:x86", "/nodefaultlib"]
    missing_flags = [flag for flag in required_flags if flag.lower() not in text.lower()]
    source_units = [
        "runtime.c", "pad_input.c", "input_router.c", "movie_skip.c", "confirm_dialog.c", "dialogue_input.c",
        "cursor.c", "exploration.c", "investigation.c", "control_modes.c", "ui_bridge.c", "interface_shell.c", "interface_items.c", "interface_skills.c", "interface_equipment.c", "interface_inner_stats.c",
        "spatial_neighbor.c", "interface_formation.c", "interface_tome.c", "interface_options.c",
        "inn.c", "synthesis.c", "shop.c", "scene_choice.c", "save_slot.c", "save_point.c", "frontend.c", "battle.c", "plugin.c",
    ]
    missing_units = [name for name in source_units if name not in text]
    first_lines = text.splitlines()[:5]
    chcp_ok = any("chcp 65001" in line.lower() for line in first_lines)
    raw_build = build.read_bytes()
    utf8_bom = raw_build.startswith(b"\xef\xbb\xbf")
    crlf_only = b"\n" not in raw_build.replace(b"\r\n", b"")
    if missing_flags or missing_units or not chcp_ok or not utf8_bom or not crlf_only:
        result.fail("Windows 独立编译单元构建规则", f"缺标志={missing_flags}，缺源码={missing_units}，chcp65001={chcp_ok}，UTF8_BOM={utf8_bom}，CRLF={crlf_only}")
    else:
        result.ok("Windows 独立编译单元构建规则", "29 个 .c + x86 /W4 /WX /utf-8 /nodefaultlib + UTF-8 BOM/CRLF")


def check_artifact(root: Path, result: CheckResult) -> None:
    """读取最少量 PE 头，确认打包的 ASI 确实是 32 位 x86 DLL，而不是误打包其它文件。"""
    asi = root / "编译内容" / "Castle_PadSupport.asi"
    if not asi.is_file():
        result.fail("ASI 编译产物", "文件不存在")
        return
    data = asi.read_bytes()
    try:
        pe = struct.unpack_from("<I", data, 0x3C)[0]
        machine = struct.unpack_from("<H", data, pe + 4)[0]
        characteristics = struct.unpack_from("<H", data, pe + 22)[0]
        magic = struct.unpack_from("<H", data, pe + 24)[0]
        is_i386 = machine == 0x014C
        is_pe32 = magic == 0x010B
        is_dll = bool(characteristics & 0x2000)
        compiled_markers = [
            b"refactor37",
            "r36路由底座已启用".encode("utf-8"),
            "RT临时鼠标".encode("utf-8"),
            "LT调查".encode("utf-8"),
            "全局震动强度".encode("utf-8"),
        ]
        missing_markers = [marker.decode("utf-8") for marker in compiled_markers if marker not in data]
        if is_i386 and is_pe32 and is_dll and not missing_markers:
            result.ok("ASI PE 结构/本轮编译标记", f"PE32/i386 DLL + refactor37 Back/RT/LT模式与全局震动配置标记，SHA-256={sha256(asi)}")
        else:
            result.fail("ASI PE 结构/本轮编译标记", f"machine=0x{machine:04X}, magic=0x{magic:04X}, DLL={is_dll}，缺编译标记={missing_markers}")
    except Exception as exc:
        result.fail("ASI PE 结构", str(exc))


def check_documents(root: Path, result: CheckResult) -> None:
    """
    检查交付包的文档与精简验证边界。

    项目现在明确规定：
    1. 所有“现行说明文档”统一放在根目录的“文档”文件夹；
    2. 从 refactor36 起不再内置逐版 TXT 证据树；源码、现行文档与检查工具就是交接依据；
    3. 将来若确有额外证据，只允许合并为“文档/验证汇总.md”这一份持续更新的文档。

    这样做的目的不是追求目录整齐本身，而是让任何人打开包时可以立即区分：
    “当前应阅读的说明”与“会持续更新的唯一验证摘要”。
    """
    doc_dir = root / "文档"
    required_docs = [
        "截至本版本的完整接档.md",
        "架构设计与模块边界.md",
        "历史功能与修复因果表.md",
        "地址与原版协议记录.md",
        "测试与回归清单.md",
        "已知问题与实机验收说明.md",
        "构建与部署说明.md",
        "第三方依赖说明.md",
        "工具详细说明.md",
        "本版本更新记录.md",
        "项目圣经执行检查.md",
        "文件校验清单.md",
        "放置说明.txt",
        "主菜单手柄交互设计.md",
    ]

    missing = [name for name in required_docs if not (doc_dir / name).is_file()]
    if missing:
        result.fail("独立接档文档集合", "文档目录缺少：" + ", ".join(missing))
    else:
        result.ok("独立接档文档集合", f"文档目录内 {len(required_docs)} 份现行说明均存在")

    # 所有 Markdown 都属于“现行说明文档”，因此不允许再散落到根目录、源码、编译内容或证据目录。
    misplaced_markdown = []
    for path in root.rglob("*.md"):
        try:
            path.relative_to(doc_dir)
        except ValueError:
            misplaced_markdown.append(str(path.relative_to(root)))
    if misplaced_markdown:
        result.fail("文档集中到文档目录", "发现散落 Markdown：" + ", ".join(sorted(misplaced_markdown)))
    else:
        result.ok("文档集中到文档目录", "所有 Markdown 均位于 文档/")

    # 最终“编译内容”只保留用户真正需要部署/配置的 ASI 与 INI。
    # 链接器临时生成的 .lib/.exp、旧版说明 TXT 等都不应该混进最终交付。
    compiled_dir = root / "编译内容"
    allowed_compiled = {"Castle_PadSupport.asi", "Castle_PadSupport.ini"}
    actual_compiled = {path.name for path in compiled_dir.iterdir() if path.is_file()} if compiled_dir.is_dir() else set()
    unexpected_compiled = sorted(actual_compiled - allowed_compiled)
    missing_compiled = sorted(allowed_compiled - actual_compiled)
    if unexpected_compiled or missing_compiled:
        result.fail("编译内容目录白名单", f"缺少={missing_compiled}，多余={unexpected_compiled}")
    else:
        result.ok("编译内容目录白名单", "仅 ASI + INI")

    # 交付包不能夹带 Python 缓存、OBJ、EXP 等构建中间件。
    # 这些文件既不是源码也不是证据，只会让接档者误以为它们属于正式内容。
    junk = []
    for path in root.rglob("*"):
        if path.is_dir() and path.name == "__pycache__":
            junk.append(str(path.relative_to(root)) + "/")
        elif path.is_file() and path.suffix.lower() in {".pyc", ".obj", ".exp"}:
            junk.append(str(path.relative_to(root)))
    if junk:
        result.fail("交付包无缓存/中间文件", ", ".join(sorted(junk)))
    else:
        result.ok("交付包无缓存/中间文件")

    # 用户明确要求从本版起不再内置逐版证据；旧证据树即使内容正确也不能继续塞进交付包。
    evidence_dir = root / "证据"
    if evidence_dir.exists():
        result.fail("refactor37精简证据策略", "交付包仍包含 证据/；请删除逐版证据树，必要结论只合并进一份持续维护文档")
    else:
        result.ok("refactor37精简证据策略", "未内置逐版证据树；源码、现行文档与本检查器构成交接依据")


def main() -> int:
    parser = argparse.ArgumentParser(description="检查幽城手柄操控模组 refactor37：校验Back常驻/地图RT临时鼠标、LT原版resolver调查、震动仲裁、29个独立C构建、文档与目标RPG.exe")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parent.parent, help="包根目录；默认自动取工具目录的上一层")
    parser.add_argument("--exe", type=Path, help="可选：待验证的 RPG.exe。提供后先检查双样本 SHA 白名单，再执行既有冻结协议以及主 Interface state2～state8 页面协议；state3 治疗目标的 +0x768 短锚点与两处新 Event CALL、以及既有 state7/state8 协议也必须通过")
    args = parser.parse_args()

    root = args.root.resolve()
    result = CheckResult()
    print(f"检查包：{root}")
    print("=" * 78)

    check_source_architecture(root, result)
    check_ini_and_build(root, result)
    check_artifact(root, result)
    check_documents(root, result)
    if args.exe:
        check_target_exe(args.exe.resolve(), result)
    else:
        print("提示：本次没有传 --exe，因此只检查源码、构建、文档与交付结构；RPG.exe 的双 SHA 白名单、既有冻结协议、state2～state8、存档点与调查 resolver 协议均未执行。")

    print("=" * 78)
    print(f"总结果：PASS={result.passed}，FAIL={result.failed}")
    if result.failed:
        print("结论：静态检查失败。不要把这个包标成可测试候选，应先修复上面的 FAIL。")
        return 1
    print("结论：静态检查全部通过。注意：仍必须进行真实游戏实机验收。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
