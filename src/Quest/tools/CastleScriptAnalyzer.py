#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》全地图脚本 / 任务事实导出器
版本：0.1.5

这个工具的目标不是“猜任务”，而是把游戏 Map 目录里能够静态证明的事实展开成严格 CSV：
- 哪个场景有哪些 SCI 对象；
- 对象的接触 Event（EA）和主动交互 Event（EB）是什么；
- 每个 EVE Event 有哪些指令；
- 哪些 Event 读取 / 写入哪些 GameVar；
- 哪些 Event 会跳场景、开战斗、增减物品；
- MSG 中有哪些消息文本，以及 EVE 的 SHOW_MESSAGE 指向哪条消息；
- Map 根目录或子目录里的 DAT/SYS 资源容器中还藏着哪些 EVE/SCI/MSG/SF2；
- public/Ail2.ENC、RefineT.ENC、RefineK.ENC、ShopItem.dat 中与任务拼图直接相关的全局物品/炼化/商店事实；
- v0.1.5 额外把原版 SF2 Section0 接触判定几何与 EA/EB/Event 关联成“原版事件空间锚点”表。

重要原则：
1. 只读游戏资源，不会修改任何原始文件。
2. 默认在脚本所在 multimedia 目录中新建“任务分析_0.1.5_原版事件空间数据”子目录，避免和旧版 CSV 混在一起。
3. 一个文件解析失败不会让全局扫描中断；错误会单独写入“任务分析_扫描错误.csv”。
4. 对尚未完全闭合的格式绝不装作已确认：CSV 会把证据等级和未知边界写出来。
5. 只使用 Python 标准库，不需要安装第三方包。
"""

from __future__ import annotations

# argparse 用来提供 --root / --output 等可选命令行参数；正常双击或直接运行时完全不需要参数。
import argparse
# csv 是 Python 自带的 CSV 写入器；使用它可以正确处理逗号、双引号和特殊字符，而不是自己手拼字符串。
import csv
# hashlib 用于给文件和容器内资源计算 SHA-256，方便之后确认两份资料是否真的是同一份字节数据。
import hashlib
# os 这里只用于少量路径和文件系统兼容处理；绝不会拿它去修改游戏环境变量。
import os
# re 用于识别 mp0101、mp3001 这一类场景编号，以及筛选可能的资源名。
import re
# struct 用于按 little-endian 方式从二进制 EVE/SCI/DAT 中读取 u16/u32/i16/i32。
import struct
# sys 用于返回退出码和打印最外层错误。
import sys
# unicodedata 用来把路径统一到稳定的 Unicode 形式，避免同一个繁体/组合字符资源因为编码细节被误判成两份。
import unicodedata
# zlib 用于解开原版 SF2 的 0x6F 压缩包装；它属于 Python 标准库，不需要额外安装。
import zlib
# dataclass 用来把“一个待分析资源”组织成清楚的数据结构，避免用位置含义不明的 tuple。
from dataclasses import dataclass
# Path 是标准库的现代路径对象，能在 Windows 上正确处理中文目录和反斜杠路径。
from pathlib import Path, PurePosixPath
# typing 只提供类型提示，让代码更容易检查和维护，不参与游戏数据逻辑。
from typing import Any, Iterable, Iterator


# ------------------------------
# 版本与已确认的固定结构
# ------------------------------

# 每次修改工具逻辑都应更新这个版本号；CSV 的“工具版本”列会写入同一个值。
TOOL_VERSION = "0.1.5"

# v0.1.1 关键修正：
# - EVE 文件槽号是 0 基；游戏运行时 Event、SCI EA/EB 是 1 基。
# - 所有 EVE 派生 CSV 的 EventID 统一输出“运行时 1 基 EventID”。
# - MSG 自身仍保留原始 0 基 Event槽，消息关联时内部自动做 -1 映射。
#
# v0.1.2 新增“资源覆盖 / 有效资源视图”：
# - 游戏目录里可能同时存在 Map\场景\散装 EVE/SCI/MSG 和 Map 根 DAT 中的同名原始资源。
# - 原始事实不能丢，所以详细 CSV 仍保留两边；但 GameVar 索引、场景摘要、事件对象关联必须只使用一份“当前有效资源”，否则同一 Event 会被重复或被旧版本污染。
# - 默认策略是 loose-first：同一逻辑资源出现散装物理文件和 DAT 成员时，散装物理文件优先。这个策略与当前补丁/安装布局和实机证据一致；工具同时提供 container-first 诊断模式，避免把仍待进一步静态闭合的加载优先级写死成不可检查的假设。
# - 新增“任务分析_资源覆盖关系.csv”，每一份候选资源都明确记录逻辑资源ID、优先级、是否有效、覆盖者、内容是否相同。

# v0.1.5 新增“原版事件空间数据”导出：
# - 扫描 Map 下以及 DAT/SYS 容器里的 SF2；
# - 解析原版 SF2 Section0 记录的局部碰撞矩形；
# - 把 EntitySCI 的对象 World、SF2 anchor、EA/EB、SF2 Section0 几何与 EVE 事件摘要拼成一张表；
# - 目的不是直接宣布某个矩形中心就是最终 Marker，而是一次性导出原版碰撞/触发空间证据，供插件建立可靠适配层。

# 原版 Map SCI 的固定记录大小已经由 RPG.exe 直接 Reader 0x0040AC00 闭合。
MAP_SCI_RECORD_SIZE = 0x473
# 原版 Entity/NPC SCI 的固定记录大小已经由 RPG.exe 直接 Reader 0x0040A160 闭合。
ENTITY_SCI_RECORD_SIZE = 0x227
# 原版 SF2 固定头大小来自已经固化的 SF2 解析器；普通和 0x6F 压缩包装都保留这段头。
SF2_HEADER_SIZE = 0x42BC
SF2_MAGIC = b"SF2\x05"

# EVE/MSG 顶层都使用 8 字节头；已知地图样本随后通常有 130 个 16 字节槽描述记录。
TOP_HEADER_SIZE = 8
# EVE/MSG 的槽描述记录是 4 个 little-endian u32，共 16 字节。
TOP_SLOT_RECORD_SIZE = 16
# 当前已确认地图 EVE/MSG 常见槽数是 130；如果未来遇到别的值，工具会记录警告但不会直接退出。
KNOWN_EVENT_SLOT_COUNT = 130

# DAT/SYS 资源容器已经确认使用 10 字节头。
CONTAINER_HEADER_SIZE = 10
# 容器目录项固定为 39 字节。
CONTAINER_RECORD_SIZE = 39

# 为了防止损坏容器或恶意构造的目录产生无限递归，嵌套容器最多自动深入 6 层。
MAX_CONTAINER_DEPTH = 6

# 场景名通常类似 mp0101、mp2412a、mp3001；这里不限制后缀只能是数字，保留 a/b/c 等字母。
SCENE_RE = re.compile(r"(?i)^mp[0-9][0-9a-z]*(?:-[0-9a-z]+)*$")

# CSV 全部使用 UTF-8 with BOM。Windows Excel 直接双击时能可靠识别中文，不会误判成 ANSI。
CSV_ENCODING = "utf-8-sig"

# EVE/SCI/MSG/SF2 都会直接生成任务或事件空间事实，因此资源覆盖判定统一对这四类做“谁有效”的选择。
# DAT/SYS 本身仍会完整列在文件清单和容器目录中，但不会和容器内部成员争夺同一个逻辑资源ID。
TASK_FACT_SUFFIXES = {".eve", ".sci", ".msg", ".sf2"}

# 默认优先散装文件。用户若要做逆向对照，可以在命令行传 --resource-policy container-first，
# 让 DAT 成员暂时成为有效视图；两种模式都不会删除另一层原始事实。
RESOURCE_POLICY_CHOICES = ("loose-first", "container-first")

# 这些列会附加到所有能追溯到 EVE/SCI/MSG/SF2 的详细 CSV 中。
# “有效资源=1”代表该行属于当前资源策略选中的运行时视图；0 代表它只是被覆盖的历史/原始候选。
RESOURCE_META_FIELDS = [
    "逻辑资源ID",
    "资源来源层级",
    "覆盖有效",
    "有效资源",
    "覆盖状态",
    "运行时引用逻辑资源ID",
    "有效资源路径",
    "资源策略",
]


# ------------------------------
# EVE Opcode 名称表
# ------------------------------
# 这里仅收录当前固化研究中已经有静态/机器码证据的 Canonical 名称。
# 即使某个 Opcode 的参数还没完全闭合，也仍然能在“事件指令.csv”中显示名称和完整原始 bytes。
# 后续如果语义继续闭合，只需要扩展解码函数，不需要改 CSV 的基础架构。
OPCODE_NAMES: dict[int, str] = {
    0x00: "END_EVENT",
    0x01: "WAIT_LEGACY_TICKS",
    0x02: "SET_VAR",
    0x03: "ADD_VAR",
    0x04: "SUB_VAR",
    0x05: "SHOW_MESSAGE",
    0x0B: "LOAD_NPC",
    0x0C: "UNLOAD_NPC",
    0x0D: "ENABLE_SCENE_RENDER_OVERRIDE",
    0x11: "SET_NPC_SPEED",
    0x12: "SET_NPC_STATE",
    0x13: "SET_NPC_POSITION_FACING",
    0x14: "SET_ENTITY_POSITION",
    0x15: "SET_ENTITY_STATE_BYTE",
    0x16: "CONFIGURE_ENTITY_ACTION_STATE",
    0x19: "CONFIGURE_ENTITY_ACTION_RELATIVE_TO_PLAYER",
    0x1A: "SET_ENTITY_ACTION_RANGE_E2_E3",
    0x1B: "SET_CAMERA_TRACK_ENTITY",
    0x1C: "RESET_CAMERA_TRACK_TO_PLAYER",
    0x1D: "WAIT_SCENE_ENTITY_ACTIONS_COMPLETE",
    0x1E: "PAN_CAMERA_PROXY_TO",
    0x1F: "POSITION_ENTITY_RELATIVE_TO_PLAYER",
    0x33: "SHOW_MESSAGE_ALIAS_33",
    0x34: "REQUEST_APPLICATION_EXIT",
    0x35: "RUN_INTERFACE_MENU_DISPATCHER",
    0x36: "OPEN_INTERFACE_SCREEN_WITH_MODE",
    0x37: "RUN_BATTLE_AND_WAIT_RESULT",
    0x39: "RUN_REST_REFINE_STATUS_HUB",
    0x3A: "LOAD_SCENE_WITH_WORLD_RESET",
    0x3B: "LOAD_SCENE",
    0x3C: "CONFIGURE_SCENE_VISUAL_EFFECT",
    0x3D: "SET_ACTIVE_SCENE_SUBSYSTEM_MODE_BYTE",
    0x3E: "RUN_SAVE_SLOT_INTERFACE",
    0x47: "BRANCH_VAR_EQ",
    0x48: "BRANCH_VAR_NE",
    0x49: "BRANCH_VAR_GT",
    0x4A: "BRANCH_VAR_GE",
    0x4B: "BRANCH_VAR_LT",
    0x4C: "BRANCH_VAR_LE",
    0x4D: "GOTO",
    0x51: "YIELD_EVENT_VM_A",
    0x52: "YIELD_EVENT_VM_B",
    0x53: "VISUAL_TRANSITION_RAMP_A",
    0x54: "VISUAL_TRANSITION_RAMP_B",
    0x55: "VISUAL_TRANSITION_HOLD",
    0x56: "VISUAL_TRANSITION_CUSTOM",
    0x57: "RESET_VISUAL_TRANSITION",
    0x58: "CONFIGURE_CAMERA_JITTER",
    0x59: "SET_SCENE_RENDER_OVERRIDE",
    0x5A: "VISUAL_TRANSITION_RAMP_A_EX",
    0x5B: "VISUAL_TRANSITION_RAMP_B_EX",
    0x5C: "HARD_HALT_SPIN",
    0x65: "SET_PLAYER_POSITION_AND_STATE_BYTE",
    0x66: "SET_PLAYER_POSITION",
    0x67: "SET_PLAYER_STATE_BYTE",
    0x68: "SWITCH_CONTROLLED_ACTOR_PRESERVE_POSITION",
    0x69: "APPEND_FIXED_ROLE_LIST_ENTRY_BY_SELECTOR",
    0x6A: "SAVE_CAMERA_ORIGIN",
    0x6B: "POSITION_PLAYER_RELATIVE_TO_ENTITY",
    0x79: "REMOVE_PARTY_MEMBER",
    0x7A: "ADD_PARTY_MEMBER",
    0x7C: "ADJUST_PARTY_CURRENT_RESOURCES",
    0x7D: "SET_PARTY_RESOURCE_PERCENT",
    0x7E: "ADD_ITEM",
    0x7F: "CONSUME_ITEM_IF_AVAILABLE",
    0x80: "ADD_MONEY",
    0x81: "TRY_SPEND_MONEY",
    0x82: "NOP",
    0x83: "CHECK_ROLE_SLOT_FIELD_EQ",
    0x85: "SELECT_PARTY_PRESET_AND_ROLE_BANK",
    0x86: "CHECK_ROLE_STAT_GT",
    0x87: "SET_ROLE_SLOT_RETURN_OLD_ITEM_CANDIDATE",
    0x88: "ADD_ROLE_FIELD_24",
    0x8C: "REBUILD_EXPLORATION_CONTEXT_AND_YIELD",
    0x8D: "RUN_START_MENU_DISPATCHER",
    0x8E: "AUDIO_CONTROL",
    0x8F: "CONFIGURE_PROXIMITY_TRIGGER",
    0x90: "SHIFT_WORLD_POS_X_PLUS_640",
    0x91: "SHIFT_WORLD_POS_Y_PLUS_480",
    0x92: "SHIFT_WORLD_POS_X_MINUS_640",
    0x93: "SHIFT_WORLD_POS_Y_MINUS_480",
    0x94: "BRANCH_ON_WORLD_ITEM_RESULT_EQ",
    0x95: "BRANCH_IF_WORLD_ITEM_RESULT_NONZERO",
    0x96: "STORE_DEFERRED_EVENT_RESUME_KEYS_AND_EXIT",
    0x97: "REQUIRE_MOVEMENT_MODE_1_OR_ABORT",
    0x98: "PLAY_MOVIE",
    0x99: "RANDOM_TO_VAR999",
    0x9A: "BRANCH_IF_MOVEMENT_NOT_SUCCESS",
}

# 变量比较 Opcode 与数学符号的映射。这样 CSV 不需要读者再自己记 0x47 是“==”。
VAR_BRANCH_OPERATORS: dict[int, str] = {
    0x47: "==",
    0x48: "!=",
    0x49: ">",
    0x4A: ">=",
    0x4B: "<",
    0x4C: "<=",
}

# 这些 Opcode 已确认会把结果写到特殊脚本变量 VAR999。
# 它们不是“record 内直接携带 var_index”，因此需要在 GameVar CSV 中额外生成隐式写入行。
IMPLICIT_VAR999_WRITERS: dict[int, str] = {
    0x7F: "库存数量检查/消耗结果写入 VAR999",
    0x81: "金钱是否足够及扣款结果写入 VAR999",
    0x83: "角色槽字段比较结果写入 VAR999",
    0x86: "角色数值阈值比较结果写入 VAR999",
    0x99: "随机整数直接写入 VAR999",
}


# ------------------------------
# 小型数据结构
# ------------------------------

@dataclass(frozen=True)
class ResourceBlob:
    """表示一个可分析资源；既可以是真实磁盘文件，也可以是 DAT 容器里的虚拟文件。"""

    # source_kind 用 PHYSICAL / CONTAINER_MEMBER 区分资源来源。
    source_kind: str
    # source_path 是实际磁盘上的文件路径；容器成员会指向外层 DAT/SYS 文件。
    source_path: str
    # resource_path 是对人最有意义的逻辑路径；容器成员使用容器内路径。
    resource_path: str
    # data 保存当前资源的原始字节；只有真正需要解析的文件才会创建这一对象。
    data: bytes
    # scene_id 是从 Map 子目录或资源路径推断出的场景名；全局资源可以为空字符串。
    scene_id: str
    # container_depth 用于限制嵌套容器递归层数。
    container_depth: int = 0


class AnalyzerState:
    """集中保存所有准备写入 CSV 的行，避免函数之间通过全局变量偷偷交换状态。"""

    def __init__(self) -> None:
        # 每个列表都对应一个最终 CSV；所有成员都是只包含标量值的 dict。
        self.file_rows: list[dict[str, Any]] = []
        self.container_rows: list[dict[str, Any]] = []
        self.scene_map_rows: list[dict[str, Any]] = []
        self.object_rows: list[dict[str, Any]] = []
        self.trigger_rows: list[dict[str, Any]] = []
        # v0.1.5：每个 SF2 的 Section0 记录几何。后面会与 EntitySCI 的 EA/EB 行自动关联。
        self.sf2_geometry_rows: list[dict[str, Any]] = []
        self.event_rows: list[dict[str, Any]] = []
        self.instruction_rows: list[dict[str, Any]] = []
        self.var_rows: list[dict[str, Any]] = []
        self.control_flow_rows: list[dict[str, Any]] = []
        self.scene_transition_rows: list[dict[str, Any]] = []
        self.battle_rows: list[dict[str, Any]] = []
        self.item_rows: list[dict[str, Any]] = []
        self.entity_operation_rows: list[dict[str, Any]] = []
        self.message_rows: list[dict[str, Any]] = []
        self.message_ref_rows: list[dict[str, Any]] = []
        self.error_rows: list[dict[str, Any]] = []

        # v0.1.3 开始把 public 目录里已经确认结构的“全局数据库”也一起摊平成 CSV。
        # 之所以把它们放进同一个 AnalyzerState，是为了让 Map/EVE 任务事实与物品/炼化/商店事实
        # 使用同一次运行、同一个工具版本输出；这样后续拼任务时不会出现“地图表是新版本、物品表是旧版本”的混乱。
        self.public_item_rows: list[dict[str, Any]] = []
        self.refine_recipe_rows: list[dict[str, Any]] = []
        # RefineK.ENC 是 17×17 的炼化类别矩阵。它不是具体物品配方，而是另一层炼化规则，
        # 但后续判断“某材料类别组合是否允许/如何处理”时会用到，所以 v0.1.3 一并导出。
        self.refine_category_rows: list[dict[str, Any]] = []
        self.shop_item_rows: list[dict[str, Any]] = []

        # 用集合防止同一个“容器内容 SHA + 深度”被无限递归扫描。
        self.seen_container_hashes: set[tuple[str, int]] = set()


# ------------------------------
# 安全的二进制读取辅助函数
# ------------------------------

# 下面这些函数都先检查边界；如果数据不够长就返回 None，而不是让 struct.unpack_from 直接把整个扫描打断。
def read_u8(data: bytes, offset: int) -> int | None:
    if 0 <= offset < len(data):
        return data[offset]
    return None


def read_u16(data: bytes, offset: int) -> int | None:
    if offset < 0 or offset + 2 > len(data):
        return None
    return struct.unpack_from("<H", data, offset)[0]


def read_i16(data: bytes, offset: int) -> int | None:
    if offset < 0 or offset + 2 > len(data):
        return None
    return struct.unpack_from("<h", data, offset)[0]


def read_u32(data: bytes, offset: int) -> int | None:
    if offset < 0 or offset + 4 > len(data):
        return None
    return struct.unpack_from("<I", data, offset)[0]


def read_i32(data: bytes, offset: int) -> int | None:
    if offset < 0 or offset + 4 > len(data):
        return None
    return struct.unpack_from("<i", data, offset)[0]


def low_i16_from_u32(value: int | None) -> int | None:
    """EVE 的 SET/ADD/SUB_VAR 参数序列化为 u32，但原脚本变量实际是 signed WORD；这里只取低 16 位再转有符号。"""
    if value is None:
        return None
    value &= 0xFFFF
    return value - 0x10000 if value >= 0x8000 else value


def decode_cstr(data: bytes, offset: int, max_len: int, encoding: str = "cp950") -> str:
    """读取固定区域中的 NUL 结尾字符串；台湾原版资源优先按 CP950/Big5 解码。"""
    if offset < 0 or offset >= len(data) or max_len <= 0:
        return ""
    raw = data[offset : min(len(data), offset + max_len)]
    raw = raw.split(b"\x00", 1)[0]
    if not raw:
        return ""
    try:
        return raw.decode(encoding)
    except UnicodeDecodeError:
        # 如果遇到少量损坏字节，保留替换字符，至少不丢掉整个对象名或路径。
        return raw.decode(encoding, errors="replace")


def decode_asciiish_cstr(data: bytes, offset: int, max_len: int) -> str:
    """场景名和资源路径多数是 ASCII；这里先 ASCII，失败再退回 CP950。"""
    if offset < 0 or offset >= len(data) or max_len <= 0:
        return ""
    raw = data[offset : min(len(data), offset + max_len)].split(b"\x00", 1)[0]
    if not raw:
        return ""
    try:
        return raw.decode("ascii")
    except UnicodeDecodeError:
        return raw.decode("cp950", errors="replace")


def sha256_bytes(data: bytes) -> str:
    """给内存中的资源算 SHA-256。"""
    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    """按 1 MiB 分块计算磁盘文件 SHA-256，避免大型 DAT 一次复制很多内存。"""
    h = hashlib.sha256()
    with path.open("rb") as f:
        while True:
            chunk = f.read(1024 * 1024)
            if not chunk:
                break
            h.update(chunk)
    return h.hexdigest()


def normalize_suffix(path_text: str) -> str:
    """只取最后一个扩展名并统一成小写，例如 '.EVE' -> '.eve'。"""
    return PurePosixPath(path_text.replace("\\", "/")).suffix.lower()


def safe_join(values: Iterable[Any]) -> str:
    """把列表压成稳定的 | 分隔文本；CSV 自己负责引号转义。"""
    return "|".join(str(v) for v in values if str(v) != "")


# ------------------------------
# 路径 / 场景识别
# ------------------------------

def find_map_directory(base_dir: Path) -> Path | None:
    """在 multimedia 根目录下按大小写不敏感方式寻找 Map 目录。"""
    direct = base_dir / "Map"
    if direct.is_dir():
        return direct
    for child in base_dir.iterdir() if base_dir.is_dir() else []:
        if child.is_dir() and child.name.casefold() == "map":
            return child
    return None


def extract_scene_id(path_text: str, map_dir: Path | None = None) -> str:
    """从物理路径或容器内路径中寻找 mpXXXX 形式的场景编号。"""
    # 统一使用正斜杠，再逐段检查；这样 Windows 与容器虚拟路径使用同一套逻辑。
    normalized = path_text.replace("\\", "/")
    parts = [part for part in normalized.split("/") if part]

    # 优先使用真正的目录段，例如 Map/mp0101/Mp0101.eve 中的 mp0101。
    for part in parts:
        if SCENE_RE.fullmatch(part):
            return part.lower()

    # 有些资源名本身会写成 MPmp2412a.SCI；如果目录段没命中，再从 stem 中寻找 mp 子串。
    stem = PurePosixPath(normalized).stem
    match = re.search(r"(?i)(mp[0-9][0-9a-z]*(?:-[0-9a-z]+)*)", stem)
    if match:
        return match.group(1).lower()

    # 实在无法识别时返回空字符串；空值比凭空猜一个场景更安全。
    return ""


def relative_display_path(path: Path, base_dir: Path) -> str:
    """尽量把文件路径写成相对 multimedia 的路径，让用户在另一台电脑上也容易对照。"""
    try:
        return path.resolve().relative_to(base_dir.resolve()).as_posix()
    except Exception:
        return path.as_posix()


def normalize_resource_path_text(path_text: str) -> str:
    """
    把“磁盘相对路径”和“DAT!/容器内路径”都整理成稳定的正斜杠 Unicode 文本。

    这里不做业务判断，只做三件非常基础的清洁：
    1. Windows 反斜杠换成 `/`；
    2. 删除路径首尾多余 `/`；
    3. 使用 NFC 规范化 Unicode，避免视觉上相同的组合字符被字典当成不同 key。
    """
    normalized = path_text.replace("\\", "/").strip("/")
    return unicodedata.normalize("NFC", normalized)


def logical_resource_id(resource_path: str, scene_id: str) -> str:
    """
    把散装文件和 DAT 内成员转换成同一个“逻辑资源ID”。

    例子：
        Map/Mp0101/Mp0101.eve
        Map/mp0101.DAT!/mp0101/MP0101.EVE
    两者最终都会得到：
        mp0101/mp0101.eve

    这一步非常重要，因为只有先证明“两份物理来源在逻辑上代表同一个游戏资源”，
    后面才有资格讨论谁覆盖谁。工具不会只按文件名粗暴比较，而是同时保留场景 ID 和场景内相对路径。
    """
    resource_path = normalize_resource_path_text(resource_path)
    scene = unicodedata.normalize("NFC", scene_id or "").casefold()

    # 容器资源的真正游戏内路径在 `!/` 右边；左边只是它来自哪个 DAT 的证据路径。
    if "!/" in resource_path:
        logical_part = resource_path.split("!/", 1)[1]
    else:
        logical_part = resource_path
        # 物理文件通常写成 Map/场景/文件名。逻辑 ID 不需要最外层 Map 目录。
        parts = [part for part in logical_part.split("/") if part]
        if parts and parts[0].casefold() == "map":
            parts = parts[1:]
        logical_part = "/".join(parts)

    parts = [unicodedata.normalize("NFC", part) for part in logical_part.split("/") if part]

    # 容器内和散装目录通常都会再带一次场景目录名；既然 scene 已经放在逻辑ID开头，
    # 就去掉这个重复段，避免 mp0101/mp0101/mp0101.eve 这种难读 key。
    if scene and parts and parts[0].casefold() == scene:
        parts = parts[1:]

    tail = "/".join(part.casefold() for part in parts)
    if scene:
        return f"{scene}/{tail}" if tail else scene
    return tail


def container_depth_from_file_row(row: dict[str, Any]) -> int:
    """从文件清单备注读取容器深度；读取不到时用资源路径中的 `!/` 数量兜底。"""
    note = str(row.get("备注", ""))
    match = re.search(r"容器深度=(\d+)", note)
    if match:
        return int(match.group(1))
    return normalize_resource_path_text(str(row.get("资源路径", ""))).count("!/")


def resource_priority(row: dict[str, Any], policy: str) -> int:
    """
    给一个任务资源候选分配“只用于覆盖选择”的优先级。

    loose-first：散装物理文件 200，DAT 成员约 100；
    container-first：DAT 成员约 200，散装物理文件 100。

    容器深度越深优先级略低，作用只是让同一容器树出现重复逻辑路径时有稳定、可解释的顺序；
    它绝不代表已经证明“嵌套越深游戏越不会加载”。如果将来静态闭合出更精确 Loader 规则，只改这一处即可。
    """
    source_kind = str(row.get("来源类型", ""))
    depth = container_depth_from_file_row(row)
    if policy == "container-first":
        return 200 - depth if source_kind == "CONTAINER_MEMBER" else 100
    return 200 if source_kind == "PHYSICAL" else 100 - depth


def build_resource_resolution(
    state: AnalyzerState,
    policy: str,
) -> tuple[list[dict[str, Any]], dict[str, dict[str, Any]], dict[str, int]]:
    """
    根据文件清单建立“原始候选 -> 当前有效资源”的完整映射。

    返回三份东西：
    - 覆盖关系 CSV 行；
    - resource_path -> 元数据 的快速查找表；
    - 汇总统计，供终端输出和测试使用。

    安全原则：如果最高优先级同时出现两份不同 SHA 的资源，工具不会偷偷按文件名挑一个，
    而是把整个逻辑资源标记为 AMBIGUOUS_TOP_CONFLICT，并让聚合索引暂时排除它。
    """
    groups: dict[str, list[dict[str, Any]]] = {}

    for row in state.file_rows:
        suffix = str(row.get("扩展名", "")).casefold()
        if suffix not in TASK_FACT_SUFFIXES:
            continue
        resource_path = str(row.get("资源路径", ""))
        # 不完全信任旧 CSV/旧版本留下的“场景”列：直接从当前资源路径重新识别一次。
        # 这同时修复 v0.1.1 把 mp0601-1 / mp0601-2 都压成 mp0601、把 MPMP0601-1 错识别成 mpmp0601 的问题。
        scene = extract_scene_id(resource_path) or str(row.get("场景", ""))
        logical_id = logical_resource_id(resource_path, scene)
        candidate = dict(row)
        candidate["逻辑资源ID"] = logical_id
        candidate["优先级"] = resource_priority(row, policy)
        groups.setdefault(logical_id, []).append(candidate)

    relation_rows: list[dict[str, Any]] = []
    meta_by_path: dict[str, dict[str, Any]] = {}
    stats = {
        "逻辑资源数": 0,
        "多候选逻辑资源数": 0,
        "内容相同覆盖组": 0,
        "内容不同覆盖组": 0,
        "同优先级冲突组": 0,
        "被覆盖候选数": 0,
    }

    for logical_id in sorted(groups, key=str.casefold):
        candidates = groups[logical_id]
        stats["逻辑资源数"] += 1
        if len(candidates) > 1:
            stats["多候选逻辑资源数"] += 1
            if len({str(c.get("SHA256", "")) for c in candidates}) == 1:
                stats["内容相同覆盖组"] += 1
            else:
                stats["内容不同覆盖组"] += 1

        top_priority = max(int(c["优先级"]) for c in candidates)
        top_candidates = [c for c in candidates if int(c["优先级"]) == top_priority]
        top_hashes = {str(c.get("SHA256", "")) for c in top_candidates}

        winner: dict[str, Any] | None
        ambiguous = len(top_candidates) > 1 and len(top_hashes) > 1
        if ambiguous:
            winner = None
            stats["同优先级冲突组"] += 1
        else:
            # 同优先级且字节完全相同的重复，只选路径排序最稳定的一份作为“有效代表”；
            # 另一份仍保留在原始事实中，并标明 EQUIVALENT_TOP_DUPLICATE。
            winner = sorted(top_candidates, key=lambda c: normalize_resource_path_text(str(c.get("资源路径", ""))).casefold())[0]

        winner_path = str(winner.get("资源路径", "")) if winner else ""
        winner_kind = str(winner.get("来源类型", "")) if winner else ""
        winner_hash = str(winner.get("SHA256", "")) if winner else ""

        for candidate in sorted(candidates, key=lambda c: (-int(c["优先级"]), normalize_resource_path_text(str(c.get("资源路径", ""))).casefold())):
            path = str(candidate.get("资源路径", ""))
            digest = str(candidate.get("SHA256", ""))

            if ambiguous:
                effective = 0
                status = "AMBIGUOUS_TOP_CONFLICT" if int(candidate["优先级"]) == top_priority else "SHADOWED_BY_AMBIGUOUS_TOP"
                reason = "最高优先级存在不同SHA候选；为避免伪造运行时事实，本组不选有效资源。"
            elif path == winner_path:
                effective = 1
                status = "EFFECTIVE"
                reason = "当前资源策略选中的有效代表。"
            elif int(candidate["优先级"]) == top_priority and digest == winner_hash:
                effective = 0
                status = "EQUIVALENT_TOP_DUPLICATE"
                reason = "与有效代表同优先级且字节完全相同；为防止聚合重复，只保留一份代表。"
                stats["被覆盖候选数"] += 1
            elif digest == winner_hash:
                effective = 0
                status = "SHADOWED_SAME_BYTES"
                reason = "优先级较低，但内容与有效资源完全相同。"
                stats["被覆盖候选数"] += 1
            else:
                effective = 0
                status = "SHADOWED_DIFFERENT_BYTES"
                reason = "优先级较低，且内容与有效资源不同；原始事实保留，但聚合任务视图不使用。"
                stats["被覆盖候选数"] += 1

            meta = {
                "逻辑资源ID": logical_id,
                "资源来源层级": candidate.get("来源类型", ""),
                # 覆盖有效：只回答“同一个逻辑文件的散装/DAT候选谁赢”。
                # 有效资源：还会在下面叠加 MapSCI EVE引用筛选，最终供任务聚合表使用。
                "覆盖有效": effective,
                "有效资源": effective,
                "覆盖状态": status,
                "运行时选择状态": "COVERAGE_EFFECTIVE" if effective else status,
                "运行时引用逻辑资源ID": "",
                "有效资源路径": winner_path,
                "资源策略": policy,
                "来源文件": candidate.get("来源文件", ""),
                "优先级": candidate.get("优先级", ""),
                "SHA256": digest,
                "有效来源类型": winner_kind,
                "有效SHA256": winner_hash,
            }
            meta_by_path[path] = meta

            relation_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "资源策略": policy,
                    "逻辑资源ID": logical_id,
                    "场景": candidate.get("场景", ""),
                    "扩展名": candidate.get("扩展名", ""),
                    "候选数": len(candidates),
                    "来源类型": candidate.get("来源类型", ""),
                    "来源文件": candidate.get("来源文件", ""),
                    "资源路径": path,
                    "优先级": candidate.get("优先级", ""),
                    "SHA256": digest,
                    "覆盖有效": effective,
                    "是否有效": effective,
                    "覆盖状态": status,
                    "运行时选择状态": "COVERAGE_EFFECTIVE" if effective else status,
                    "运行时引用逻辑资源ID": "",
                    "有效资源路径": winner_path,
                    "有效来源类型": winner_kind,
                    "有效SHA256": winner_hash,
                    "与有效资源内容相同": 1 if winner and digest == winner_hash else ("" if not winner else 0),
                    "判定原因": reason,
                    "证据": "默认 loose-first 依据当前安装/补丁布局与实机任务链；container-first 可用于诊断反证。原始候选永不删除。",
                }
            )

    # ------------------------------------------------------------------
    # 第二层：使用“有效 MapSCI 里的 EVE引用”筛掉同场景里没有被当前地图定义引用的备用/旧脚本。
    # ------------------------------------------------------------------
    # 这一步解决的不是“散装覆盖 DAT”，而是另一种真实存在的情况：
    # 一个 DAT 里可能同时塞着 MP0104.EVE 和 MP0204.EVE，但 MapSCI 明确写 EVE引用=mp0104\mp0104.eve。
    # 两个文件的逻辑资源ID不同，所以单靠第一层覆盖规则都会被视为有效；如果不看 MapSCI，Event 图仍会混入闲置脚本。
    scene_eve_refs: dict[str, set[str]] = {}
    for map_row in state.scene_map_rows:
        map_path = str(map_row.get("资源路径", ""))
        map_meta = meta_by_path.get(map_path)
        if not map_meta or map_meta.get("覆盖有效") != 1:
            continue
        scene = extract_scene_id(map_path) or str(map_row.get("场景", ""))
        eve_ref = str(map_row.get("EVE引用", "")).strip()
        if scene and eve_ref:
            scene_eve_refs.setdefault(scene, set()).add(logical_resource_id(eve_ref, scene))

    # 用 EVE引用 同名推导 MSG；当前 MapSCI 没有独立 MSG 字段，而实际资源长期采用同 basename 的 .MSG。
    # 如果未来发现不遵循这一规则的正例，只需取消对应场景的 MSG 筛选，不影响 EVE 本身。
    scene_msg_refs: dict[str, set[str]] = {}
    for scene, eve_ids in scene_eve_refs.items():
        for eve_id in eve_ids:
            if eve_id.endswith(".eve"):
                scene_msg_refs.setdefault(scene, set()).add(eve_id[:-4] + ".msg")

    # 先记住每个逻辑资源第一层覆盖选中的代表路径，方便把“未引用 EVE”指向真正被 MapSCI 选中的脚本。
    winner_path_by_logical: dict[str, str] = {}
    for path, meta in meta_by_path.items():
        if meta.get("覆盖有效") == 1:
            winner_path_by_logical[str(meta.get("逻辑资源ID", ""))] = path

    stats["未被MapSCI引用EVE数"] = 0
    stats["未被MapSCI配对MSG数"] = 0

    for path, meta in meta_by_path.items():
        # 被同逻辑资源更高层候选覆盖的行本来就不会进入聚合视图，不需要再叠加“未引用”标签。
        if meta.get("覆盖有效") != 1:
            continue
        scene = extract_scene_id(path)
        suffix = normalize_suffix(path)
        logical_id = str(meta.get("逻辑资源ID", ""))

        if suffix == ".eve" and scene in scene_eve_refs and logical_id not in scene_eve_refs[scene]:
            expected_ids = sorted(scene_eve_refs[scene])
            meta["有效资源"] = 0
            meta["覆盖状态"] = "UNREFERENCED_BY_EFFECTIVE_MAPSCI"
            meta["运行时选择状态"] = "UNREFERENCED_BY_EFFECTIVE_MAPSCI"
            meta["运行时引用逻辑资源ID"] = safe_join(expected_ids)
            selected_paths = [winner_path_by_logical[eid] for eid in expected_ids if eid in winner_path_by_logical]
            if selected_paths:
                meta["有效资源路径"] = safe_join(selected_paths)
                selected_meta = meta_by_path.get(selected_paths[0], {})
                meta["有效来源类型"] = selected_meta.get("资源来源层级", "")
                meta["有效SHA256"] = selected_meta.get("SHA256", "")
            stats["未被MapSCI引用EVE数"] += 1

        elif suffix == ".msg" and scene in scene_msg_refs and logical_id not in scene_msg_refs[scene]:
            expected_ids = sorted(scene_msg_refs[scene])
            meta["有效资源"] = 0
            meta["覆盖状态"] = "UNREFERENCED_MSG_FOR_EFFECTIVE_EVE"
            meta["运行时选择状态"] = "UNREFERENCED_MSG_FOR_EFFECTIVE_EVE"
            meta["运行时引用逻辑资源ID"] = safe_join(expected_ids)
            selected_paths = [winner_path_by_logical[mid] for mid in expected_ids if mid in winner_path_by_logical]
            if selected_paths:
                meta["有效资源路径"] = safe_join(selected_paths)
                selected_meta = meta_by_path.get(selected_paths[0], {})
                meta["有效来源类型"] = selected_meta.get("资源来源层级", "")
                meta["有效SHA256"] = selected_meta.get("SHA256", "")
            stats["未被MapSCI配对MSG数"] += 1

    # relation_rows 是给人看的完整覆盖表；把第二层运行时筛选结果同步回去。
    for row in relation_rows:
        meta = meta_by_path.get(str(row.get("资源路径", "")), {})
        row["覆盖有效"] = meta.get("覆盖有效", row.get("覆盖有效", ""))
        row["是否有效"] = meta.get("有效资源", row.get("是否有效", ""))
        row["覆盖状态"] = meta.get("覆盖状态", row.get("覆盖状态", ""))
        row["运行时选择状态"] = meta.get("运行时选择状态", row.get("运行时选择状态", ""))
        row["运行时引用逻辑资源ID"] = meta.get("运行时引用逻辑资源ID", "")
        row["有效资源路径"] = meta.get("有效资源路径", row.get("有效资源路径", ""))
        row["有效来源类型"] = meta.get("有效来源类型", row.get("有效来源类型", ""))
        row["有效SHA256"] = meta.get("有效SHA256", row.get("有效SHA256", ""))
        if meta.get("有效SHA256"):
            row["与有效资源内容相同"] = 1 if str(row.get("SHA256", "")) == str(meta.get("有效SHA256", "")) else 0
        if row.get("运行时选择状态") == "UNREFERENCED_BY_EFFECTIVE_MAPSCI":
            row["判定原因"] = "该逻辑 EVE 本身有覆盖代表，但有效 MapSCI 的 EVE引用指向同场景另一份脚本，因此不进入运行时任务图。"
        elif row.get("运行时选择状态") == "UNREFERENCED_MSG_FOR_EFFECTIVE_EVE":
            row["判定原因"] = "该逻辑 MSG 本身有覆盖代表，但与有效 MapSCI 选中的 EVE basename 不匹配，因此不进入默认消息视图。"

    return relation_rows, meta_by_path, stats


def resource_meta_fields(resource_path: str, meta_by_path: dict[str, dict[str, Any]], policy: str) -> dict[str, Any]:
    """把一条事实所属资源的覆盖元数据整理成可直接塞进 CSV 行的 6 个固定字段。"""
    meta = meta_by_path.get(str(resource_path), {})
    return {
        "逻辑资源ID": meta.get("逻辑资源ID", ""),
        "资源来源层级": meta.get("资源来源层级", ""),
        "覆盖有效": meta.get("覆盖有效", ""),
        "有效资源": meta.get("有效资源", ""),
        "覆盖状态": meta.get("覆盖状态", ""),
        "运行时引用逻辑资源ID": meta.get("运行时引用逻辑资源ID", ""),
        "有效资源路径": meta.get("有效资源路径", ""),
        "资源策略": meta.get("资源策略", policy),
    }


def is_effective_resource(resource_path: str, meta_by_path: dict[str, dict[str, Any]]) -> bool:
    """
    判断某一 EVE/SCI/MSG/SF2 是否属于当前有效视图。

    正常扫描出来的任务资源一定会出现在 meta_by_path；如果未来新增解析器漏做文件清单登记，
    这里宁可返回 False，让聚合表少一条，也不要让“来源不明的数据”悄悄污染正式任务图。
    """
    meta = meta_by_path.get(str(resource_path))
    return bool(meta and meta.get("有效资源") == 1)


def annotate_resource_rows(state: AnalyzerState, meta_by_path: dict[str, dict[str, Any]], policy: str) -> None:
    """
    给所有详细事实行追加资源覆盖字段。

    解析器内部原始结构不需要为了 v0.1.2 全部重写；我们在扫描结束、写 CSV 之前统一根据来源资源路径补标签。
    这样可以最大限度减少对已经验证过的 EVE/SCI/MSG 解码逻辑的干扰。
    """
    collections_and_keys = [
        (state.file_rows, "资源路径"),
        (state.scene_map_rows, "资源路径"),
        (state.object_rows, "资源路径"),
        (state.trigger_rows, "对象资源"),
        (state.sf2_geometry_rows, "SF2资源"),
        (state.event_rows, "EVE资源"),
        (state.instruction_rows, "EVE资源"),
        (state.var_rows, "来源资源"),
        (state.control_flow_rows, "来源资源"),
        (state.scene_transition_rows, "来源资源"),
        (state.battle_rows, "来源资源"),
        (state.item_rows, "来源资源"),
        (state.entity_operation_rows, "来源资源"),
        (state.message_rows, "MSG资源"),
        (state.message_ref_rows, "EVE资源"),
    ]
    for rows, key in collections_and_keys:
        for row in rows:
            path = str(row.get(key, ""))
            row.update(resource_meta_fields(path, meta_by_path, policy))


# ------------------------------
# CSV 写入
# ------------------------------

def write_csv_strict(path: Path, rows: list[dict[str, Any]], fieldnames: list[str]) -> None:
    """使用 RFC4180 风格写严格 CSV；字段顺序固定，所有输出使用 UTF-8 BOM。"""
    # newline="" 是 csv 模块官方建议的写法，否则 Windows 上可能出现多出来的空行。
    with path.open("w", encoding=CSV_ENCODING, newline="") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=fieldnames,
            extrasaction="ignore",
            dialect="excel",
            quoting=csv.QUOTE_MINIMAL,
            lineterminator="\r\n",
        )
        writer.writeheader()
        for row in rows:
            # 所有 None 转成空字段；bool/int/str 则保持正常文本形式。
            clean = {name: ("" if row.get(name) is None else row.get(name, "")) for name in fieldnames}
            writer.writerow(clean)


def add_error(state: AnalyzerState, resource: ResourceBlob | None, stage: str, message: str) -> None:
    """统一收集错误；任何单文件失败都只记一行，不会停止扫描其他地图。"""
    state.error_rows.append(
        {
            "工具版本": TOOL_VERSION,
            "来源类型": resource.source_kind if resource else "GLOBAL",
            "来源文件": resource.source_path if resource else "",
            "资源路径": resource.resource_path if resource else "",
            "场景": resource.scene_id if resource else "",
            "阶段": stage,
            "错误": message,
        }
    )


# ------------------------------
# DAT/SYS 资源容器
# ------------------------------

def decode_container_name(raw: bytes) -> tuple[str, bool]:
    """容器目录项的名称区域固定 25 字节；优先 CP950，返回名称与“是否有 NUL 终止”。"""
    terminated = b"\x00" in raw
    content = raw.split(b"\x00", 1)[0]
    try:
        name = content.decode("cp950")
    except UnicodeDecodeError:
        name = content.decode("cp950", errors="replace")
    return name, terminated


def parse_container_members(data: bytes) -> tuple[bool, list[dict[str, Any]], str]:
    """
    解析已确认的 10-byte header + N×39-byte directory container。

    返回：
    - 是否识别为该容器格式；
    - 所有可从根目录记录 0 到达的文件/目录行；
    - 如果不识别或结构有问题，给一段原因文本。
    """
    # 最短必须能容纳 10 字节头和至少一个 39 字节目录记录。
    if len(data) < CONTAINER_HEADER_SIZE + CONTAINER_RECORD_SIZE:
        return False, [], "文件太短，不能满足10字节头+39字节记录"

    header_u32 = read_u32(data, 0)
    header_u16 = read_u16(data, 4)
    index_size = read_u32(data, 6)

    # 当前九个已确认原版容器都使用 1 / 1 签名；不匹配就不要误把普通 DAT 当资源目录。
    if header_u32 != 1 or header_u16 != 1:
        return False, [], "头部1/1签名不匹配"
    if index_size is None or index_size < CONTAINER_RECORD_SIZE or index_size % CONTAINER_RECORD_SIZE != 0:
        return False, [], "index_size 不是正整数个39字节目录记录"

    record_count = index_size // CONTAINER_RECORD_SIZE
    data_base = CONTAINER_HEADER_SIZE + index_size
    if data_base > len(data):
        return False, [], "目录索引区超过文件大小"

    # 先把每个固定记录解出来，后面目录递归只操作这些小字典。
    records: list[dict[str, Any]] = []
    for index in range(record_count):
        offset = CONTAINER_HEADER_SIZE + index * CONTAINER_RECORD_SIZE
        kind = read_u16(data, offset)
        value = read_u32(data, offset + 2)
        unknown = read_u32(data, offset + 6)
        name, terminated = decode_container_name(data[offset + 10 : offset + 35])
        target = read_u32(data, offset + 35)
        records.append(
            {
                "record_index": index,
                "record_offset": offset,
                "kind": kind,
                "value": value,
                "unknown": unknown,
                "name": name,
                "terminated": terminated,
                "target": target,
            }
        )

    # 目录记录的 target 指向另一组39字节记录；文件记录的 target 指向数据区。
    # 从记录0递归走，可以保留原目录层级，不会把同名文件混在一起。
    output: list[dict[str, Any]] = []
    active: set[int] = set()
    visited: set[int] = set()
    errors: list[str] = []

    def walk(record_index: int, parent: tuple[str, ...]) -> None:
        if record_index in active:
            errors.append(f"目录图出现环：record {record_index}")
            return
        if not 0 <= record_index < len(records):
            errors.append(f"目录图引用越界：record {record_index}")
            return

        rec = records[record_index]
        visited.add(record_index)
        name = str(rec["name"])
        path_parts = parent + ((name or f"__record_{record_index}"),)
        logical_path = "/".join(path_parts)

        kind = rec["kind"]
        value = rec["value"]
        target = rec["target"]

        if kind == 0:
            valid_range = (
                isinstance(target, int)
                and isinstance(value, int)
                and target >= data_base
                and target + value <= len(data)
            )
            output.append(
                {
                    "记录索引": record_index,
                    "类型": "文件",
                    "容器内路径": logical_path,
                    "名称": name,
                    "字节数": value if isinstance(value, int) else "",
                    "数据偏移": target if isinstance(target, int) else "",
                    "范围有效": 1 if valid_range else 0,
                    "名称NUL终止": 1 if rec["terminated"] else 0,
                    "未知加6": rec["unknown"],
                }
            )
            return

        if kind == 1:
            # 目录的 value 是直接子项数，target 是子表在索引区中的字节偏移。
            aligned = isinstance(target, int) and target >= CONTAINER_HEADER_SIZE and (target - CONTAINER_HEADER_SIZE) % CONTAINER_RECORD_SIZE == 0
            child_index = (target - CONTAINER_HEADER_SIZE) // CONTAINER_RECORD_SIZE if aligned else None
            valid_child_table = (
                child_index is not None
                and isinstance(value, int)
                and 0 <= child_index <= len(records)
                and child_index + value <= len(records)
            )
            output.append(
                {
                    "记录索引": record_index,
                    "类型": "目录",
                    "容器内路径": logical_path,
                    "名称": name,
                    "字节数": "",
                    "数据偏移": target if isinstance(target, int) else "",
                    "范围有效": 1 if valid_child_table else 0,
                    "名称NUL终止": 1 if rec["terminated"] else 0,
                    "未知加6": rec["unknown"],
                }
            )
            if not valid_child_table:
                errors.append(f"目录record {record_index}的子表越界或未按39字节对齐")
                return
            active.add(record_index)
            assert child_index is not None
            for child in range(child_index, child_index + int(value)):
                walk(child, path_parts)
            active.remove(record_index)
            return

        # 遇到未知 kind 时仍然输出，避免信息被静默丢掉。
        output.append(
            {
                "记录索引": record_index,
                "类型": f"未知({kind})",
                "容器内路径": logical_path,
                "名称": name,
                "字节数": value if isinstance(value, int) else "",
                "数据偏移": target if isinstance(target, int) else "",
                "范围有效": 0,
                "名称NUL终止": 1 if rec["terminated"] else 0,
                "未知加6": rec["unknown"],
            }
        )
        errors.append(f"record {record_index} 出现未知类型 {kind}")

    if records:
        walk(0, ())

    # 不能从根目录到达的记录不一定是错误，但对完整任务资料来说值得记下来。
    unreachable = len(records) - len(visited)
    if unreachable:
        errors.append(f"有 {unreachable} 条目录记录不能从 record0 到达")

    return True, output, "；".join(errors)


def scan_container_resource(state: AnalyzerState, resource: ResourceBlob) -> list[ResourceBlob]:
    """解析一个 DAT/SYS；把容器目录写入 CSV，并把内部 EVE/SCI/MSG 变成新的 ResourceBlob 返回。"""
    if resource.container_depth > MAX_CONTAINER_DEPTH:
        add_error(state, resource, "容器", f"超过最大容器递归深度 {MAX_CONTAINER_DEPTH}")
        return []

    digest = sha256_bytes(resource.data)
    dedup_key = (digest, resource.container_depth)
    if dedup_key in state.seen_container_hashes:
        return []
    state.seen_container_hashes.add(dedup_key)

    recognized, entries, note = parse_container_members(resource.data)
    if not recognized:
        # DAT 不一定都是资源目录，所以“不识别”不是全局错误；文件清单仍然保留它。
        return []

    nested_resources: list[ResourceBlob] = []

    for entry in entries:
        row = {
            "工具版本": TOOL_VERSION,
            "容器来源": resource.source_path,
            "容器资源路径": resource.resource_path,
            "容器深度": resource.container_depth,
            **entry,
        }

        # 文件项只有范围有效时才能安全切出真实数据；目录项没有数据块。
        if entry["类型"] == "文件" and entry["范围有效"]:
            offset = int(entry["数据偏移"])
            size = int(entry["字节数"])
            member_data = resource.data[offset : offset + size]
            row["SHA256"] = sha256_bytes(member_data)

            # 容器内路径保留原树结构，前面再加外层 resource_path，避免多个 DAT 的同名成员冲突。
            virtual_path = f"{resource.resource_path}!/{entry['容器内路径']}"
            scene_id = extract_scene_id(entry["容器内路径"])
            suffix = normalize_suffix(entry["容器内路径"])

            # EVE/SCI/MSG/嵌套容器继续进入队列。
            # SF2 往往远大于脚本文件，如果也把所有 SF2 bytes 暂存在 nested_resources，完整游戏扫描会平白占用大量内存。
            # 因此 SF2 在当前目录项这里就“读一份、解析一份、马上释放一份”，只把几何结果留在 AnalyzerState。
            if suffix == ".sf2":
                sf2_resource = ResourceBlob(
                    source_kind="CONTAINER_MEMBER",
                    source_path=resource.source_path,
                    resource_path=virtual_path,
                    data=member_data,
                    scene_id=scene_id,
                    container_depth=resource.container_depth + 1,
                )
                state.file_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "来源类型": sf2_resource.source_kind,
                        "来源文件": sf2_resource.source_path,
                        "资源路径": sf2_resource.resource_path,
                        "场景": sf2_resource.scene_id,
                        "扩展名": ".sf2",
                        "字节数": len(member_data),
                        "SHA256": row["SHA256"],
                        "备注": f"容器深度={sf2_resource.container_depth}；SF2即时解析",
                    }
                )
                try:
                    parse_sf2(state, sf2_resource)
                except Exception as exc:
                    add_error(state, sf2_resource, "SF2解析", f"{type(exc).__name__}: {exc}")
            elif suffix in {".eve", ".sci", ".msg", ".dat", ".sys"}:
                nested_resources.append(
                    ResourceBlob(
                        source_kind="CONTAINER_MEMBER",
                        source_path=resource.source_path,
                        resource_path=virtual_path,
                        data=member_data,
                        scene_id=scene_id,
                        container_depth=resource.container_depth + 1,
                    )
                )
        else:
            row["SHA256"] = ""

        state.container_rows.append(row)

    if note:
        add_error(state, resource, "容器结构警告", note)

    return nested_resources



# ------------------------------
# SF2 原版空间判定几何
# ------------------------------

def decompress_sf2_for_geometry(raw: bytes) -> tuple[bytes, dict[str, Any]]:
    """
    把一个原版 SF2 变成可以直接按 Section 表读取的字节流。

    原版存在两种情况：
    1. 普通 SF2：整个文件已经是直接可读结构；
    2. Header +0x0B == 0x6F：0x42BC 字节固定头后面还有 8 字节尺寸，真正的数据尾由 zlib 压缩。

    这里完全只读。我们不会尝试重新编码 SF2，也不会改变游戏文件。
    """
    if len(raw) < SF2_HEADER_SIZE or raw[:4] != SF2_MAGIC:
        raise ValueError("不是当前已确认的标准 SF2")

    meta: dict[str, Any] = {
        "原始大小": len(raw),
        "压缩标志": raw[0x0B],
        "0x6F压缩包装": 1 if raw[0x0B] == 0x6F else 0,
        "声明最终大小": "",
        "重建大小": len(raw),
        "最终大小吻合": "",
    }

    # 非 0x6F 文件无需任何处理，直接把原字节交给 Section 解析器。
    if raw[0x0B] != 0x6F:
        return raw, meta

    if len(raw) < SF2_HEADER_SIZE + 8:
        raise ValueError("0x6F SF2 的尺寸头不完整")

    final_size_a = read_u32(raw, SF2_HEADER_SIZE)
    final_size_b = read_u32(raw, SF2_HEADER_SIZE + 4)
    if final_size_a is None or final_size_b is None or final_size_a != final_size_b:
        raise ValueError("0x6F SF2 的两个最终尺寸字段不一致")

    tail = zlib.decompress(raw[SF2_HEADER_SIZE + 8 :])
    rebuilt = bytearray(raw[:SF2_HEADER_SIZE] + tail)
    # 原版解包以后等价于普通 SF2；把包装标志清零，避免后面的代码再次误判。
    rebuilt[0x0B] = 0

    meta["声明最终大小"] = final_size_a
    meta["重建大小"] = len(rebuilt)
    meta["最终大小吻合"] = 1 if len(rebuilt) == final_size_a else 0
    return bytes(rebuilt), meta


def parse_sf2(state: AnalyzerState, resource: ResourceBlob) -> None:
    """
    导出 SF2 Section0 record0 的原版空间几何。

    为什么当前最关心 Section0：
    - RPG.exe 0x00409A50 的 EA 接触事件链会进入 0x004080D0；
    - 0x004080D0 在判定前会调用 0x00407200(controller, 0)；
    - 因而 EA 的第一层原版空间判定确实来自 Section0 record0 的矩形，随后才继续做更细的像素/遮罩碰撞。

    为了让完整游戏一次扫描尽量快、CSV 尽量小，v0.1.5 只导出 record0：
    EA 原版触发链本来就明确调用 0x00407200(controller, 0)，没有必要把几百个动画动作记录全部倾倒出来。
    """
    data, wrap_meta = decompress_sf2_for_geometry(resource.data)

    # SF2 头部从 +0x14 开始依次保存 4 个 Section 描述，每个描述 10 字节：
    # u16 count + u32 offset + u32 size。
    section_desc: list[tuple[int, int, int]] = []
    for section_index in range(4):
        header_offset = 0x14 + section_index * 10
        count = read_u16(data, header_offset)
        offset = read_u32(data, header_offset + 2)
        size = read_u32(data, header_offset + 6)
        if count is None or offset is None or size is None:
            raise ValueError(f"SF2 Section{section_index} 描述读取失败")
        section_desc.append((count, offset, size))

    section0_count, section0_offset, section0_size = section_desc[0]
    section_counts = [desc[0] for desc in section_desc]

    # 即便 Section0 没有记录，也输出一行。这样完整游戏扫描后能区分“SF2 找到了但没有 Section0”与“SF2 根本没找到”。
    if section0_count == 0:
        state.sf2_geometry_rows.append(
            {
                "工具版本": TOOL_VERSION,
                "来源类型": resource.source_kind,
                "来源文件": resource.source_path,
                "SF2资源": resource.resource_path,
                "场景": resource.scene_id,
                "0x6F压缩包装": wrap_meta["0x6F压缩包装"],
                "原始大小": wrap_meta["原始大小"],
                "重建大小": wrap_meta["重建大小"],
                "Section0记录数": 0,
                "Section1记录数": section_counts[1],
                "Section2记录数": section_counts[2],
                "Section3记录数": section_counts[3],
                "Section0记录索引": "",
                "局部Left": "",
                "局部Top": "",
                "Width": "",
                "Height": "",
                "EntryCount": "",
                "记录大小": "",
                "结构大小吻合": "",
                "记录SHA256": "",
                "解析状态": "NO_SECTION0_RECORD",
                "证据": "标准SF2头与Section描述已解析；Section0 count=0",
            }
        )
        return

    section0_end = section0_offset + section0_size
    pointer_table_end = section0_offset + section0_count * 4
    if section0_offset < 0 or section0_end > len(data) or pointer_table_end > section0_end:
        raise ValueError("SF2 Section0 边界非法")

    pointers: list[int] = []
    for index in range(section0_count):
        pointer = read_u32(data, section0_offset + index * 4)
        if pointer is None:
            raise ValueError(f"SF2 Section0 record{index} 指针读取失败")
        pointers.append(pointer)

    ends = pointers[1:] + [section0_end]
    for index, (record_start, record_end) in enumerate(zip(pointers, ends)):
        # EA 触发链明确只选择 index=0。完整游戏可能有成百上千个 SF2，
        # 如果把每个动画动作都导出，会让一次任务数据扫描无意义地膨胀，所以 record0 处理完立即结束。
        if index > 0:
            break
        if record_start < pointer_table_end or record_end < record_start or record_end > section0_end:
            raise ValueError(f"SF2 Section0 record{index} 边界非法")

        record = data[record_start:record_end]
        if len(record) < 0x5E:
            # 结构异常仍然留一行，便于完整游戏扫描后精确定位资源，而不是静默漏掉。
            state.sf2_geometry_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "来源类型": resource.source_kind,
                    "来源文件": resource.source_path,
                    "SF2资源": resource.resource_path,
                    "场景": resource.scene_id,
                    "0x6F压缩包装": wrap_meta["0x6F压缩包装"],
                    "原始大小": wrap_meta["原始大小"],
                    "重建大小": wrap_meta["重建大小"],
                    "Section0记录数": section0_count,
                    "Section1记录数": section_counts[1],
                    "Section2记录数": section_counts[2],
                    "Section3记录数": section_counts[3],
                    "Section0记录索引": index,
                    "记录大小": len(record),
                    "结构大小吻合": 0,
                    "记录SHA256": sha256_bytes(record),
                    "解析状态": "SECTION0_RECORD_TOO_SHORT",
                    "证据": "记录短于已确认固定头0x5E",
                }
            )
            continue

        local_left = read_i32(record, 0x00)
        local_top = read_i32(record, 0x04)
        width = read_i32(record, 0x08)
        height = read_i32(record, 0x0C)
        entry_count = read_u16(record, 0x54)
        expected_size = 0x5E + 29 * int(entry_count or 0)
        size_matches = 1 if entry_count is not None and len(record) == expected_size else 0

        state.sf2_geometry_rows.append(
            {
                "工具版本": TOOL_VERSION,
                "来源类型": resource.source_kind,
                "来源文件": resource.source_path,
                "SF2资源": resource.resource_path,
                "场景": resource.scene_id,
                "0x6F压缩包装": wrap_meta["0x6F压缩包装"],
                "原始大小": wrap_meta["原始大小"],
                "重建大小": wrap_meta["重建大小"],
                "Section0记录数": section0_count,
                "Section1记录数": section_counts[1],
                "Section2记录数": section_counts[2],
                "Section3记录数": section_counts[3],
                "Section0记录索引": index,
                "局部Left": local_left,
                "局部Top": local_top,
                "Width": width,
                "Height": height,
                "EntryCount": entry_count,
                "记录大小": len(record),
                "结构大小吻合": size_matches,
                "记录SHA256": sha256_bytes(record),
                "解析状态": "OK" if size_matches else "SIZE_MISMATCH",
                "证据": "SF2 Section0：+0x00/+0x04局部位置，+0x08/+0x0C宽高，+0x54子层数；EA碰撞链会选择record0",
            }
        )

# ------------------------------
# SCI 识别与解析
# ------------------------------

def sci_classification_score(data: bytes, resource_path: str) -> tuple[str, int, int]:
    """
    返回 (类型, map_score, entity_score)。

    不能只靠“文件大小能整除某个 stride”就下结论，所以同时检查文件名和已知字符串字段是否像真的 Map/Entity。
    """
    size = len(data)
    name = PurePosixPath(resource_path.replace("\\", "/")).name.casefold()
    map_score = 0
    entity_score = 0

    if size > 0 and size % MAP_SCI_RECORD_SIZE == 0:
        map_score += 3
        if name.startswith("mp"):
            map_score += 2
        # Map SCI 的 0x205 和 0x2CD 是资源路径区；出现 .SCI/.EVE 会进一步提高可信度。
        if size >= MAP_SCI_RECORD_SIZE:
            entity_ref = decode_asciiish_cstr(data, 0x205, 100).casefold()
            eve_ref = decode_asciiish_cstr(data, 0x2CD, 100).casefold()
            if ".sci" in entity_ref or entity_ref:
                map_score += 2
            if ".eve" in eve_ref or eve_ref:
                map_score += 2
            width = read_u16(data, 0x6A)
            height = read_u16(data, 0x6C)
            if width is not None and height is not None and 0 < width < 20000 and 0 < height < 20000:
                map_score += 1

    if size > 0 and size % ENTITY_SCI_RECORD_SIZE == 0:
        entity_score += 3
        if name.startswith("np") or "npc" in name or "entity" in name:
            entity_score += 2
        if size >= ENTITY_SCI_RECORD_SIZE:
            object_name = decode_cstr(data, 0, 0x64)
            sf2_path = decode_asciiish_cstr(data, 0x7B, 100)
            if object_name:
                entity_score += 1
            if sf2_path.casefold().endswith(".sf2") or sf2_path:
                entity_score += 2

    if map_score == 0 and entity_score == 0:
        return "UNKNOWN_SCI", map_score, entity_score
    if map_score >= entity_score + 2:
        return "MAP_SCI", map_score, entity_score
    if entity_score >= map_score + 2:
        return "ENTITY_SCI", map_score, entity_score

    # 分数接近时宁可标记不确定，避免把错误记录大量写入场景对象表。
    return "AMBIGUOUS_SCI", map_score, entity_score


def parse_map_sci(state: AnalyzerState, resource: ResourceBlob) -> None:
    """按 0x473 stride 展开 Map SCI 的任务相关字段。"""
    if len(resource.data) % MAP_SCI_RECORD_SIZE != 0:
        add_error(state, resource, "Map SCI", "文件大小不是0x473的整数倍")
        return

    record_count = len(resource.data) // MAP_SCI_RECORD_SIZE
    for index in range(record_count):
        start = index * MAP_SCI_RECORD_SIZE
        rec = resource.data[start : start + MAP_SCI_RECORD_SIZE]
        row = {
            "工具版本": TOOL_VERSION,
            "来源类型": resource.source_kind,
            "来源文件": resource.source_path,
            "资源路径": resource.resource_path,
            "场景": resource.scene_id,
            "记录索引": index,
            "场景原点X": read_i16(rec, 0x66),
            "场景原点Y": read_i16(rec, 0x68),
            "地图宽": read_u16(rec, 0x6A),
            "地图高": read_u16(rec, 0x6C),
            "本地实体SCI引用": decode_asciiish_cstr(rec, 0x205, 100),
            "EVE引用": decode_asciiish_cstr(rec, 0x2CD, 100),
            "BGM选择器": read_u8(rec, 0x331),
            "随机遭遇启用原值": read_u32(rec, 0x378),
            "初始遭遇GameVarID": read_u32(rec, 0x37C),
            "存档入口许可": read_u32(rec, 0x380),
            "Event槽2回退门控": read_u32(rec, 0x384),
            "证据": "MapSCI stride=0x473；字段来自当前固化直接Reader/consumer结论",
        }
        state.scene_map_rows.append(row)

        # Map 的 +0x37C 本身是一个 GameVar ID；它不是读写指令，但对任务/遭遇图非常有用，所以也写入变量事实表。
        encounter_var = row["初始遭遇GameVarID"]
        if isinstance(encounter_var, int) and encounter_var != 0:
            state.var_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "场景": resource.scene_id,
                    "EventID": "",
                    "指令索引": "",
                    "Opcode": "MAP+0x37C",
                    "指令名": "MAP_INITIAL_ENCOUNTER_SELECTOR_VAR",
                    "GameVarID": encounter_var,
                    "访问类型": "REFERENCE",
                    "操作": "地图随机遭遇选择变量引用",
                    "值或比较值": "",
                    "跳转目标": "",
                    "来源资源": resource.resource_path,
                    "证据": "强结构证据",
                }
            )


def parse_entity_sci(state: AnalyzerState, resource: ResourceBlob) -> None:
    """按 0x227 stride 展开 NPC/物件，并直接生成 EA/EB 触发点。"""
    if len(resource.data) % ENTITY_SCI_RECORD_SIZE != 0:
        add_error(state, resource, "Entity SCI", "文件大小不是0x227的整数倍")
        return

    record_count = len(resource.data) // ENTITY_SCI_RECORD_SIZE
    for index in range(record_count):
        start = index * ENTITY_SCI_RECORD_SIZE
        rec = resource.data[start : start + ENTITY_SCI_RECORD_SIZE]

        name = decode_cstr(rec, 0x00, 0x64)
        world_x = read_i32(rec, 0x64)
        world_y = read_i32(rec, 0x68)
        anchor_x = read_i16(rec, 0x6C)
        anchor_y = read_i16(rec, 0x6E)
        present_enabled = read_u8(rec, 0x70)
        interaction_mode = read_u8(rec, 0x73)
        collision_proximity_flag = read_u8(rec, 0x74)
        render_update_suppression = read_u8(rec, 0x75)
        behavior_type = read_u8(rec, 0x76)
        sf2_path = decode_asciiish_cstr(rec, 0x7B, 100)
        touch_event = read_u8(rec, 0xEA)
        manual_event = read_u8(rec, 0xEB)
        presence_var = read_u32(rec, 0xFF)

        object_row = {
            "工具版本": TOOL_VERSION,
            "来源类型": resource.source_kind,
            "来源文件": resource.source_path,
            "资源路径": resource.resource_path,
            "场景": resource.scene_id,
            "记录索引": index,
            "对象名": name,
            "世界X": world_x,
            "世界Y": world_y,
            "SF2锚点X": anchor_x,
            "SF2锚点Y": anchor_y,
            "在场启用": present_enabled,
            "交互模式": interaction_mode,
            "碰撞接近标志": collision_proximity_flag,
            "渲染更新抑制": render_update_suppression,
            "行为类型": behavior_type,
            "SF2路径": sf2_path,
            "接触Event_EA": touch_event,
            "主动Event_EB": manual_event,
            "在场状态GameVarID": presence_var,
            "记录SHA256": sha256_bytes(rec),
            "证据": "EntitySCI stride=0x227；EA/EB和+0xFF来自当前固化直接consumer结论",
        }
        state.object_rows.append(object_row)

        # EA 非零表示接触/重叠触发 Event；一个对象可以同时有 EA 和 EB，所以分别写两行。
        if isinstance(touch_event, int) and touch_event != 0:
            state.trigger_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "场景": resource.scene_id,
                    "对象资源": resource.resource_path,
                    "对象记录索引": index,
                    "对象名": name,
                    "触发类型": "TOUCH_EA",
                    "EventID": touch_event,
                    "世界X": world_x,
                    "世界Y": world_y,
                    "SF2锚点X": anchor_x,
                    "SF2锚点Y": anchor_y,
                    "SF2路径": sf2_path,
                    "交互模式": interaction_mode,
                    "碰撞接近标志": collision_proximity_flag,
                    "在场状态GameVarID": presence_var,
                    "证据": "EA +0xEA；0x409A50接触链",
                }
            )

        # EB 非零表示玩家主动/手动交互 Event。
        if isinstance(manual_event, int) and manual_event != 0:
            state.trigger_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "场景": resource.scene_id,
                    "对象资源": resource.resource_path,
                    "对象记录索引": index,
                    "对象名": name,
                    "触发类型": "MANUAL_EB",
                    "EventID": manual_event,
                    "世界X": world_x,
                    "世界Y": world_y,
                    "SF2锚点X": anchor_x,
                    "SF2锚点Y": anchor_y,
                    "SF2路径": sf2_path,
                    "交互模式": interaction_mode,
                    "碰撞接近标志": collision_proximity_flag,
                    "在场状态GameVarID": presence_var,
                    "证据": "EB +0xEB；0x409860主动交互链",
                }
            )

        # +0xFF 是对象在场状态变量 ID；把它作为“引用”放入统一变量事实表，后面能和 EVE 的写入点做全局拼图。
        if isinstance(presence_var, int) and presence_var not in {0, 0xFFFFFFFF}:
            state.var_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "场景": resource.scene_id,
                    "EventID": "",
                    "指令索引": "",
                    "Opcode": "SCI+0xFF",
                    "指令名": "ENTITY_PRESENCE_VAR_REFERENCE",
                    "GameVarID": presence_var,
                    "访问类型": "REFERENCE",
                    "操作": f"对象在场状态引用：{name}",
                    "值或比较值": "",
                    "跳转目标": "",
                    "来源资源": resource.resource_path,
                    "证据": "已确认字段；具体启用值语义仍由脚本条件决定",
                }
            )


def parse_sci(state: AnalyzerState, resource: ResourceBlob) -> None:
    """先保守分类，再交给 Map SCI 或 Entity SCI 解析器。"""
    kind, map_score, entity_score = sci_classification_score(resource.data, resource.resource_path)
    if kind == "MAP_SCI":
        parse_map_sci(state, resource)
    elif kind == "ENTITY_SCI":
        parse_entity_sci(state, resource)
    else:
        add_error(
            state,
            resource,
            "SCI分类",
            f"未可靠分类：{kind}；map_score={map_score} entity_score={entity_score} size={len(resource.data)}",
        )


# ------------------------------
# MSG 解析
# ------------------------------

def decode_msg_payload(payload: bytes) -> tuple[str, str]:
    """
    MSG 文本主体是 Big5/CP950，中间夹有 00 01、00 02 等控制序列。
    当前控制码业务语义没有完全闭合，所以这里做两件事：
    - “纯文本”只抽可解码的中文/ASCII；
    - “控制码”保留所有遇到的 00 xx，绝不伪造其含义。
    """
    chunks: list[bytes] = []
    controls: list[str] = []
    current = bytearray()
    i = 0

    while i < len(payload):
        b = payload[i]
        if b == 0x00:
            if current:
                chunks.append(bytes(current))
                current.clear()
            if i + 1 < len(payload):
                controls.append(f"00 {payload[i + 1]:02X}")
                i += 2
                continue
            controls.append("00")
            i += 1
            continue
        current.append(b)
        i += 1

    if current:
        chunks.append(bytes(current))

    decoded_parts: list[str] = []
    for chunk in chunks:
        try:
            decoded_parts.append(chunk.decode("cp950"))
        except UnicodeDecodeError:
            decoded_parts.append(chunk.decode("cp950", errors="replace"))

    return "".join(decoded_parts), safe_join(controls)


def parse_msg(state: AnalyzerState, resource: ResourceBlob) -> None:
    """解析已确认的 MSG 顶层槽表，并保守抽取每个消息 payload 的 Big5 文本。"""
    data = resource.data
    if len(data) < TOP_HEADER_SIZE:
        add_error(state, resource, "MSG", "文件不足8字节")
        return

    slot_count = read_u32(data, 0)
    data_base = read_u32(data, 4)
    if slot_count is None or data_base is None:
        add_error(state, resource, "MSG", "头部读取失败")
        return

    expected_base = TOP_HEADER_SIZE + slot_count * TOP_SLOT_RECORD_SIZE
    if expected_base > len(data):
        add_error(state, resource, "MSG", "槽描述表超过文件")
        return
    if data_base != expected_base:
        add_error(state, resource, "MSG警告", f"data_base={data_base:#x} 与 8+slot_count*16={expected_base:#x} 不一致")

    for slot_id in range(slot_count):
        desc = TOP_HEADER_SIZE + slot_id * TOP_SLOT_RECORD_SIZE
        off = read_u32(data, desc)
        count = read_u32(data, desc + 4)
        unknown0 = read_u32(data, desc + 8)
        size = read_u32(data, desc + 12)
        if None in {off, count, unknown0, size}:
            add_error(state, resource, "MSG", f"slot {slot_id} 描述读取失败")
            continue
        assert off is not None and count is not None and size is not None

        if off == 0 and count == 0 and size == 0:
            continue
        if off < expected_base or off + size > len(data):
            add_error(state, resource, "MSG", f"slot {slot_id} 范围非法 off={off:#x} size={size}")
            continue

        # 当前样本已经确认槽数据最前面有一个 DWORD 0，然后是 count 条绝对文件偏移。
        pointer_table_bytes = 4 + count * 4
        if pointer_table_bytes > size:
            add_error(state, resource, "MSG", f"slot {slot_id} 的 4+count*4 指针表超过槽大小")
            continue

        slot_header_unknown = read_u32(data, off)
        pointers: list[int] = []
        pointer_ok = True
        for i in range(count):
            p = read_u32(data, off + 4 + i * 4)
            if p is None:
                pointer_ok = False
                break
            pointers.append(p)
        if not pointer_ok:
            add_error(state, resource, "MSG", f"slot {slot_id} 指针表读取失败")
            continue

        slot_end = off + size
        for message_index, start in enumerate(pointers):
            end = pointers[message_index + 1] if message_index + 1 < len(pointers) else slot_end
            if start < off + pointer_table_bytes or start > end or end > slot_end:
                add_error(state, resource, "MSG", f"slot {slot_id} message {message_index} 指针越界")
                continue
            payload = data[start:end]
            text, controls = decode_msg_payload(payload)
            state.message_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "场景": resource.scene_id,
                    "MSG资源": resource.resource_path,
                    "Event槽": slot_id,
                    "消息索引_0基": message_index,
                    "槽内消息数": count,
                    "槽头未知DWORD": slot_header_unknown,
                    "文本": text,
                    "控制码": controls,
                    "原始十六进制": payload.hex(" "),
                    "证据": "MSG顶层130-slot已确认；内层count/绝对指针结构由当前样本支持；控制码业务语义未冻结",
                }
            )


# ------------------------------
# EVE 指令解码
# ------------------------------

def instruction_params(opcode: int, record: bytes) -> dict[str, Any]:
    """
    只解码当前已经确认或有强结构证据的参数。
    返回字典中的值都会进入“事件指令.csv”的固定参数列或派生事实 CSV。
    """
    p: dict[str, Any] = {}

    if opcode == 0x01:
        p["arg1"] = read_u32(record, 4)
        p["arg_summary"] = f"tick_count={p['arg1']}"

    elif opcode in {0x02, 0x03, 0x04}:
        p["var_index"] = read_u32(record, 4)
        raw_value = read_u32(record, 8)
        p["value"] = low_i16_from_u32(raw_value)
        label = {0x02: "set", 0x03: "add", 0x04: "sub"}[opcode]
        p["arg_summary"] = f"var={p['var_index']} {label} {p['value']}"

    elif opcode in {0x05, 0x33}:
        # SHOW_MESSAGE 的 message_index_minus_one 位于未对齐的 +0x07，这一点来自 Execute 静态链。
        p["ui_arg_a"] = read_u8(record, 4)
        p["ui_arg_b"] = read_u8(record, 5)
        p["ui_arg_c"] = read_u8(record, 6)
        p["message_index_minus_one"] = read_u32(record, 7)
        p["message_option"] = read_u8(record, 0x0B)
        p["arg_summary"] = f"msg_index_minus_one={p['message_index_minus_one']} option={p['message_option']}"

    elif opcode in {0x0B, 0x0C}:
        p["entity_name"] = decode_cstr(record, 4, min(20, max(0, len(record) - 4)))
        if opcode == 0x0C:
            p["arg1"] = read_u32(record, 0x18)
        p["arg_summary"] = f"name={p['entity_name']}"

    elif opcode == 0x14:
        p["entity_name"] = decode_cstr(record, 4, min(20, max(0, len(record) - 4)))
        p["x"] = read_i32(record, 0x18)
        p["y"] = read_i32(record, 0x1C)
        p["arg_summary"] = f"name={p['entity_name']} x={p['x']} y={p['y']}"

    elif opcode == 0x1E:
        p["x"] = read_i32(record, 4)
        p["y"] = read_i32(record, 8)
        p["arg1"] = read_u32(record, 0x0C)
        p["arg_summary"] = f"x={p['x']} y={p['y']} ticks={p['arg1']}"

    elif opcode == 0x36:
        p["arg1"] = read_u32(record, 4)
        p["arg_summary"] = f"interface_mode={p['arg1']}"

    elif opcode == 0x37:
        p["battle_group"] = read_u32(record, 4)
        p["arg1"] = read_u32(record, 8)
        p["target_instruction"] = read_u32(record, 0x0C)
        p["arg_summary"] = f"layout_group={p['battle_group']} defeat_flag={p['arg1']} defeat_target={p['target_instruction']}"

    elif opcode == 0x39:
        p["arg1"] = read_u32(record, 4)
        p["arg_summary"] = f"men9001_index={p['arg1']}"

    elif opcode == 0x3A:
        p["scene_name"] = decode_asciiish_cstr(record, 4, min(20, max(0, len(record) - 4)))
        p["arg_summary"] = f"scene={p['scene_name']}"

    elif opcode == 0x3B:
        # 0x3B 已确认是轻量 LOAD_SCENE，但参数 exact layout 尚未完全冻结。
        # 为了不丢线索，只扫描 +4 起的候选字符串，并明确标成“候选”。
        candidate = decode_asciiish_cstr(record, 4, min(32, max(0, len(record) - 4)))
        if candidate and re.fullmatch(r"(?i)[a-z0-9_\\/.-]+", candidate):
            p["scene_name_candidate"] = candidate
            p["arg_summary"] = f"scene_candidate={candidate}"
        else:
            p["arg_summary"] = "参数布局未完全冻结"

    elif opcode in VAR_BRANCH_OPERATORS:
        p["var_index"] = read_u32(record, 4)
        p["compare_value"] = read_i32(record, 8)
        p["target_instruction"] = read_u32(record, 0x0C)
        p["arg_summary"] = f"var={p['var_index']} {VAR_BRANCH_OPERATORS[opcode]} {p['compare_value']} -> {p['target_instruction']}"

    elif opcode == 0x4D:
        p["target_instruction"] = read_u32(record, 4)
        p["arg_summary"] = f"goto={p['target_instruction']}"

    elif opcode == 0x56:
        vals = [read_u16(record, o) for o in (4, 6, 8, 10, 12)]
        p["arg_summary"] = "transition=" + safe_join(vals)

    elif opcode == 0x58:
        vals = [read_i32(record, o) for o in (4, 8, 12, 16, 20)]
        p["arg_summary"] = "camera_jitter=" + safe_join(vals)

    elif opcode == 0x68:
        p["arg1"] = read_u32(record, 4)
        p["arg_summary"] = f"actor_index={p['arg1']}"

    elif opcode in {0x79, 0x7A}:
        p["arg1"] = read_u8(record, 4)
        p["arg_summary"] = f"role_id={p['arg1']}"

    elif opcode in {0x7C, 0x7D}:
        p["arg1"] = read_i32(record, 4)
        p["arg2"] = read_i32(record, 8)
        p["arg_summary"] = f"a={p['arg1']} b={p['arg2']}"

    elif opcode == 0x7E:
        p["item_id"] = read_i32(record, 4)
        p["quantity"] = read_i32(record, 8)
        p["item_category"] = read_u32(record, 0x0C)
        p["arg_summary"] = f"item={p['item_id']} qty={p['quantity']} category={p['item_category']}"

    elif opcode == 0x7F:
        p["item_id"] = read_u32(record, 4)
        p["quantity"] = read_i32(record, 8)
        p["arg_summary"] = f"item={p['item_id']} required={p['quantity']} -> VAR999"

    elif opcode in {0x80, 0x81}:
        p["arg1"] = read_i32(record, 4)
        p["arg_summary"] = f"money={p['arg1']}" + (" -> VAR999" if opcode == 0x81 else "")

    elif opcode == 0x83:
        p["arg1"] = read_u32(record, 4)
        p["arg2"] = read_u32(record, 8)
        p["arg3"] = read_i32(record, 0x0C)
        p["arg_summary"] = f"role={p['arg1']} slot={p['arg2']} expected={p['arg3']} -> VAR999"

    elif opcode == 0x85:
        p["arg1"] = read_u32(record, 4)
        p["arg_summary"] = f"selector={p['arg1']}"

    elif opcode == 0x86:
        p["arg1"] = read_u32(record, 4)
        p["arg2"] = read_u32(record, 8)
        p["arg3"] = read_i32(record, 0x0C)
        p["arg_summary"] = f"role={p['arg1']} stat={p['arg2']} threshold={p['arg3']} -> VAR999"

    elif opcode == 0x87:
        p["arg1"] = read_u32(record, 4)
        p["arg2"] = read_u32(record, 8)
        p["arg3"] = read_u32(record, 0x0C)
        p["arg_summary"] = f"role={p['arg1']} new={p['arg2']} return_old={p['arg3']}"

    elif opcode == 0x88:
        p["arg1"] = read_u32(record, 4)
        p["arg2"] = read_i32(record, 8)
        p["arg_summary"] = f"role={p['arg1']} delta={p['arg2']}"

    elif opcode == 0x8E:
        p["arg1"] = read_u32(record, 4)
        p["arg2"] = read_u32(record, 8)
        p["arg3"] = read_u32(record, 0x0C)
        p["arg_summary"] = f"channel={p['arg1']} audio_id={p['arg2']} option={p['arg3']}"

    elif opcode == 0x94:
        p["item_id"] = read_u32(record, 4)
        p["target_instruction"] = read_u32(record, 8)
        p["arg1"] = read_u32(record, 0x0C)
        p["arg_summary"] = f"world_item={p['item_id']} target={p['target_instruction']} failure_policy={p['arg1']}"

    elif opcode in {0x95, 0x9A}:
        p["target_instruction"] = read_u32(record, 4)
        p["arg_summary"] = f"target={p['target_instruction']}"

    elif opcode == 0x96:
        p["arg1"] = read_u32(record, 4)
        p["arg2"] = read_u32(record, 8)
        p["arg_summary"] = f"resume_key_a={p['arg1']} resume_key_b={p['arg2']}"

    elif opcode == 0x98:
        p["arg1"] = read_u32(record, 4)
        p["arg2"] = read_u32(record, 8)
        p["arg_summary"] = f"movie_id={p['arg1']} option={p['arg2']}"

    elif opcode == 0x99:
        p["arg1"] = read_i32(record, 4)
        p["arg2"] = read_i32(record, 8)
        p["arg_summary"] = f"random=[{p['arg2']},{p['arg1']}] -> VAR999"

    else:
        p["arg_summary"] = ""

    return p


def add_var_fact(
    state: AnalyzerState,
    resource: ResourceBlob,
    event_id: int,
    instruction_index: int,
    opcode: int,
    var_id: int,
    access_type: str,
    operation: str,
    value: Any = "",
    target: Any = "",
    evidence: str = "已确认",
) -> None:
    """统一追加一条 GameVar 事实，确保所有来源列格式一致。"""
    state.var_rows.append(
        {
            "工具版本": TOOL_VERSION,
            "场景": resource.scene_id,
            "EventID": event_id,
            "指令索引": instruction_index,
            "Opcode": f"0x{opcode:02X}",
            "指令名": OPCODE_NAMES.get(opcode, "UNKNOWN"),
            "GameVarID": var_id,
            "访问类型": access_type,
            "操作": operation,
            "值或比较值": value,
            "跳转目标": target,
            "来源资源": resource.resource_path,
            "证据": evidence,
        }
    )


def parse_eve(state: AnalyzerState, resource: ResourceBlob) -> None:
    """解析 EVE 顶层 slot、每条指令、直接 GameVar 数据流以及任务常用关联。"""
    data = resource.data
    if len(data) < TOP_HEADER_SIZE:
        add_error(state, resource, "EVE", "文件不足8字节")
        return

    slot_count = read_u32(data, 0)
    data_base = read_u32(data, 4)
    if slot_count is None or data_base is None:
        add_error(state, resource, "EVE", "头部读取失败")
        return

    expected_base = TOP_HEADER_SIZE + slot_count * TOP_SLOT_RECORD_SIZE
    if expected_base > len(data):
        add_error(state, resource, "EVE", "槽描述表超过文件")
        return
    if slot_count != KNOWN_EVENT_SLOT_COUNT:
        add_error(state, resource, "EVE警告", f"slot_count={slot_count}，当前已知地图常见值为130")
    if data_base != expected_base:
        add_error(state, resource, "EVE警告", f"data_base={data_base:#x} 与 8+slot_count*16={expected_base:#x} 不一致")

    # 重要：EVE 文件里的槽数组是 0 基索引，但游戏运行时、SCI 的 EA/EB 字段、
    # 以及 Castle_Quest 日志使用的是 1 基 Event ID。
    #
    # 例如：
    #   EVE 槽 22（0 基） == 运行时 Event 23 == SCI 的 EA/EB 值 0x17。
    #
    # v0.1.0 错把“槽索引”直接输出成 EventID，导致所有 EVE→SCI 对象关联整体错一位。
    # v0.1.1 从这里统一换算：event_slot_0 只负责定位文件槽；event_id 才是对外输出的运行时 ID。
    for event_slot_0 in range(slot_count):
        event_id = event_slot_0 + 1
        desc = TOP_HEADER_SIZE + event_slot_0 * TOP_SLOT_RECORD_SIZE
        off = read_u32(data, desc)
        count = read_u32(data, desc + 4)
        unknown0 = read_u32(data, desc + 8)
        size = read_u32(data, desc + 12)
        if None in {off, count, unknown0, size}:
            add_error(state, resource, "EVE", f"Event {event_id} 槽描述读取失败")
            continue
        assert off is not None and count is not None and size is not None

        # EVE 的空槽判断不能要求 off 也为 0。
        # 实机/样本已经出现“count=0、size=0，但 off 仍保留某个非零占位值”的槽描述。
        # 这种槽没有任何指令，也没有任何有效数据；如果因为 off 非零就把它当成 Event，
        # 就会把一个实际上只有 1 个 Event 的 EVE 错报成 130 个 Event。
        # 因此这里严格按旧研究工具已经验证过的规则：count 和 size 同时为 0 就视为空槽。
        if count == 0 and size == 0:
            continue
        if off < expected_base or off + size > len(data):
            add_error(state, resource, "EVE", f"Event {event_id} 槽范围非法 off={off:#x} size={size}")
            continue
        pointer_bytes = count * 4
        if pointer_bytes > size:
            add_error(state, resource, "EVE", f"Event {event_id} 指令偏移表大于槽大小")
            continue

        pointers: list[int] = []
        pointer_ok = True
        for i in range(count):
            p = read_u32(data, off + i * 4)
            if p is None:
                pointer_ok = False
                break
            pointers.append(p)
        if not pointer_ok:
            add_error(state, resource, "EVE", f"Event {event_id} 指令偏移表读取失败")
            continue

        slot_end = off + size
        event_opcode_names: list[str] = []
        vars_read: set[int] = set()
        vars_written: set[int] = set()
        battle_groups: set[int] = set()
        item_ids: set[int] = set()
        scene_targets: set[str] = set()
        message_refs: set[int] = set()

        for instruction_index, start in enumerate(pointers):
            end = pointers[instruction_index + 1] if instruction_index + 1 < len(pointers) else slot_end
            if start < off + pointer_bytes or start + 4 > slot_end or end < start or end > slot_end:
                add_error(state, resource, "EVE", f"Event {event_id} instruction {instruction_index} 指针/边界非法")
                continue

            record = data[start:end]
            opcode = read_u32(record, 0)
            if opcode is None:
                add_error(state, resource, "EVE", f"Event {event_id} instruction {instruction_index} 不足4字节")
                continue

            name = OPCODE_NAMES.get(opcode, f"UNKNOWN_0x{opcode:08X}")
            params = instruction_params(opcode, record)
            event_opcode_names.append(name)

            state.instruction_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "场景": resource.scene_id,
                    "EVE资源": resource.resource_path,
                    "EventID": event_id,
                    "指令索引": instruction_index,
                    "文件偏移": f"0x{start:X}",
                    "记录大小": len(record),
                    "Opcode": f"0x{opcode:02X}",
                    "指令名": name,
                    "参数摘要": params.get("arg_summary", ""),
                    "GameVarID": params.get("var_index", ""),
                    "值": params.get("value", params.get("compare_value", "")),
                    "跳转目标": params.get("target_instruction", ""),
                    "场景参数": params.get("scene_name", params.get("scene_name_candidate", "")),
                    "BattleGroup": params.get("battle_group", ""),
                    "ItemID": params.get("item_id", ""),
                    "数量": params.get("quantity", ""),
                    "实体名": params.get("entity_name", ""),
                    "原始记录十六进制": record.hex(" "),
                    "识别状态": "KNOWN_OPCODE" if opcode in OPCODE_NAMES else "UNKNOWN_OPCODE",
                }
            )

            # -------- GameVar 直接读写 --------
            if opcode == 0x02 and isinstance(params.get("var_index"), int):
                var_id = int(params["var_index"])
                vars_written.add(var_id)
                add_var_fact(state, resource, event_id, instruction_index, opcode, var_id, "WRITE", "SET", params.get("value", ""))

            elif opcode == 0x03 and isinstance(params.get("var_index"), int):
                var_id = int(params["var_index"])
                vars_read.add(var_id)
                vars_written.add(var_id)
                add_var_fact(state, resource, event_id, instruction_index, opcode, var_id, "READ_WRITE", "ADD", params.get("value", ""))

            elif opcode == 0x04 and isinstance(params.get("var_index"), int):
                var_id = int(params["var_index"])
                vars_read.add(var_id)
                vars_written.add(var_id)
                add_var_fact(state, resource, event_id, instruction_index, opcode, var_id, "READ_WRITE", "SUB", params.get("value", ""))

            elif opcode in VAR_BRANCH_OPERATORS and isinstance(params.get("var_index"), int):
                var_id = int(params["var_index"])
                vars_read.add(var_id)
                add_var_fact(
                    state,
                    resource,
                    event_id,
                    instruction_index,
                    opcode,
                    var_id,
                    "READ",
                    f"BRANCH_{VAR_BRANCH_OPERATORS[opcode]}",
                    params.get("compare_value", ""),
                    params.get("target_instruction", ""),
                )

            # -------- 隐式 VAR999 writer --------
            if opcode in IMPLICIT_VAR999_WRITERS:
                vars_written.add(999)
                add_var_fact(
                    state,
                    resource,
                    event_id,
                    instruction_index,
                    opcode,
                    999,
                    "WRITE_IMPLICIT",
                    IMPLICIT_VAR999_WRITERS[opcode],
                    "",
                    "",
                    "当前固化语义已确认/强结构证据",
                )

            # -------- 控制流边 --------
            next_index = instruction_index + 1 if instruction_index + 1 < count else None
            if opcode in VAR_BRANCH_OPERATORS:
                target = params.get("target_instruction")
                if isinstance(target, int):
                    state.control_flow_rows.append(
                        {
                            "工具版本": TOOL_VERSION,
                            "场景": resource.scene_id,
                            "EventID": event_id,
                            "起点指令": instruction_index,
                            "边类型": f"VAR_{VAR_BRANCH_OPERATORS[opcode]}_TRUE",
                            "条件GameVarID": params.get("var_index", ""),
                            "比较值": params.get("compare_value", ""),
                            "目标指令": target,
                            "来源资源": resource.resource_path,
                        }
                    )
                if next_index is not None:
                    state.control_flow_rows.append(
                        {
                            "工具版本": TOOL_VERSION,
                            "场景": resource.scene_id,
                            "EventID": event_id,
                            "起点指令": instruction_index,
                            "边类型": "VAR_BRANCH_FALSE_FALLTHROUGH",
                            "条件GameVarID": params.get("var_index", ""),
                            "比较值": params.get("compare_value", ""),
                            "目标指令": next_index,
                            "来源资源": resource.resource_path,
                        }
                    )

            elif opcode == 0x4D:
                target = params.get("target_instruction")
                if isinstance(target, int):
                    state.control_flow_rows.append(
                        {
                            "工具版本": TOOL_VERSION,
                            "场景": resource.scene_id,
                            "EventID": event_id,
                            "起点指令": instruction_index,
                            "边类型": "GOTO",
                            "条件GameVarID": "",
                            "比较值": "",
                            "目标指令": target,
                            "来源资源": resource.resource_path,
                        }
                    )

            elif opcode in {0x94, 0x95, 0x9A}:
                target = params.get("target_instruction")
                if isinstance(target, int):
                    state.control_flow_rows.append(
                        {
                            "工具版本": TOOL_VERSION,
                            "场景": resource.scene_id,
                            "EventID": event_id,
                            "起点指令": instruction_index,
                            "边类型": name,
                            "条件GameVarID": "",
                            "比较值": params.get("item_id", ""),
                            "目标指令": target,
                            "来源资源": resource.resource_path,
                        }
                    )
                if next_index is not None:
                    state.control_flow_rows.append(
                        {
                            "工具版本": TOOL_VERSION,
                            "场景": resource.scene_id,
                            "EventID": event_id,
                            "起点指令": instruction_index,
                            "边类型": "CONDITIONAL_FALLTHROUGH",
                            "条件GameVarID": "",
                            "比较值": "",
                            "目标指令": next_index,
                            "来源资源": resource.resource_path,
                        }
                    )

            elif opcode == 0x37:
                # 战斗失败可以跳到 record+0x0C 指定的指令；正常胜利通常继续下一条。
                defeat_target = params.get("target_instruction")
                if isinstance(defeat_target, int):
                    state.control_flow_rows.append(
                        {
                            "工具版本": TOOL_VERSION,
                            "场景": resource.scene_id,
                            "EventID": event_id,
                            "起点指令": instruction_index,
                            "边类型": "BATTLE_DEFEAT_TARGET",
                            "条件GameVarID": "",
                            "比较值": params.get("battle_group", ""),
                            "目标指令": defeat_target,
                            "来源资源": resource.resource_path,
                        }
                    )
                if next_index is not None:
                    state.control_flow_rows.append(
                        {
                            "工具版本": TOOL_VERSION,
                            "场景": resource.scene_id,
                            "EventID": event_id,
                            "起点指令": instruction_index,
                            "边类型": "BATTLE_SUCCESS_FALLTHROUGH",
                            "条件GameVarID": "",
                            "比较值": params.get("battle_group", ""),
                            "目标指令": next_index,
                            "来源资源": resource.resource_path,
                        }
                    )

            elif opcode not in {0x00, 0x34, 0x5C} and next_index is not None:
                # 普通指令按顺序进入下一条；YIELD 只是本轮暂停，下一次仍从后续指令继续，因此也保留顺序边。
                state.control_flow_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "场景": resource.scene_id,
                        "EventID": event_id,
                        "起点指令": instruction_index,
                        "边类型": "SEQUENTIAL",
                        "条件GameVarID": "",
                        "比较值": "",
                        "目标指令": next_index,
                        "来源资源": resource.resource_path,
                    }
                )

            # -------- 场景跳转 --------
            if opcode == 0x3A and params.get("scene_name"):
                target_scene = str(params["scene_name"])
                scene_targets.add(target_scene)
                state.scene_transition_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "来源场景": resource.scene_id,
                        "EventID": event_id,
                        "指令索引": instruction_index,
                        "Opcode": f"0x{opcode:02X}",
                        "类型": "LOAD_SCENE_WITH_WORLD_RESET",
                        "目标场景": target_scene,
                        "证据等级": "已确认",
                        "来源资源": resource.resource_path,
                    }
                )
            elif opcode == 0x3B and params.get("scene_name_candidate"):
                target_scene = str(params["scene_name_candidate"])
                scene_targets.add(target_scene)
                state.scene_transition_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "来源场景": resource.scene_id,
                        "EventID": event_id,
                        "指令索引": instruction_index,
                        "Opcode": f"0x{opcode:02X}",
                        "类型": "LOAD_SCENE",
                        "目标场景": target_scene,
                        "证据等级": "场景加载语义强证据；参数字符串位置仍按候选导出",
                        "来源资源": resource.resource_path,
                    }
                )

            # -------- Battle --------
            if opcode == 0x37 and isinstance(params.get("battle_group"), int):
                group = int(params["battle_group"])
                battle_groups.add(group)
                state.battle_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "场景": resource.scene_id,
                        "EventID": event_id,
                        "指令索引": instruction_index,
                        "LayoutGroup": group,
                        "败北转MP0000_Event11标志": params.get("arg1", ""),
                        "败北目标指令": params.get("target_instruction", ""),
                        "来源资源": resource.resource_path,
                        "证据": "EVE 0x37 RUN_BATTLE_AND_WAIT_RESULT",
                    }
                )

            # -------- Item --------
            if opcode in {0x7E, 0x7F, 0x94} and isinstance(params.get("item_id"), int):
                item_id = int(params["item_id"])
                item_ids.add(item_id)
                state.item_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "场景": resource.scene_id,
                        "EventID": event_id,
                        "指令索引": instruction_index,
                        "Opcode": f"0x{opcode:02X}",
                        "指令名": name,
                        "ItemID": item_id,
                        "数量": params.get("quantity", ""),
                        "类别": params.get("item_category", ""),
                        "跳转目标": params.get("target_instruction", ""),
                        "来源资源": resource.resource_path,
                    }
                )

            # -------- NPC / Entity 操作 --------
            if opcode in {0x0B, 0x0C, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x19, 0x1A, 0x1B, 0x1C, 0x1F, 0x65, 0x66, 0x67, 0x68, 0x6B, 0x8F}:
                state.entity_operation_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "场景": resource.scene_id,
                        "EventID": event_id,
                        "指令索引": instruction_index,
                        "Opcode": f"0x{opcode:02X}",
                        "指令名": name,
                        "实体名": params.get("entity_name", ""),
                        "X": params.get("x", ""),
                        "Y": params.get("y", ""),
                        "参数摘要": params.get("arg_summary", ""),
                        "来源资源": resource.resource_path,
                    }
                )

            # -------- SHOW_MESSAGE 引用 --------
            if opcode in {0x05, 0x33} and isinstance(params.get("message_index_minus_one"), int):
                msg_zero_based = int(params["message_index_minus_one"])
                message_refs.add(msg_zero_based)
                state.message_ref_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "场景": resource.scene_id,
                        "EventID": event_id,
                        "指令索引": instruction_index,
                        "Opcode": f"0x{opcode:02X}",
                        "消息选择器原值_message_index_minus_one": msg_zero_based,
                        "对应MSG消息索引_0基": msg_zero_based,
                        "UI参数A": params.get("ui_arg_a", ""),
                        "UI参数B": params.get("ui_arg_b", ""),
                        "UI参数C": params.get("ui_arg_c", ""),
                        "消息选项": params.get("message_option", ""),
                        "EVE资源": resource.resource_path,
                        "证据": "SHOW_MESSAGE Execute 已确认按当前运行时 Event ID 对应的 EVE 0基槽 + message selector 查询 MSG；v0.1.1 已修正 EventID 1基换算",
                    }
                )

        # 每个非空 Event 再输出一行摘要；这是之后人工“拼任务”的主要入口表。
        state.event_rows.append(
            {
                "工具版本": TOOL_VERSION,
                "场景": resource.scene_id,
                "EVE资源": resource.resource_path,
                "EventID": event_id,
                "指令数": count,
                "槽未知0": unknown0,
                "槽大小": size,
                "Opcode序列": safe_join(event_opcode_names),
                "读取GameVar": safe_join(sorted(vars_read)),
                "写入GameVar": safe_join(sorted(vars_written)),
                "场景目标": safe_join(sorted(scene_targets)),
                "BattleGroup": safe_join(sorted(battle_groups)),
                "ItemID": safe_join(sorted(item_ids)),
                "消息索引_0基": safe_join(sorted(message_refs)),
            }
        )


# ------------------------------
# 资源扫描调度
# ------------------------------

def add_file_inventory_row(
    state: AnalyzerState,
    base_dir: Path,
    path: Path,
    scene_id: str,
) -> None:
    """物理文件清单用流式 SHA，保证即使是很大的 DAT 也不会为算哈希额外复制整份内存。"""
    try:
        size = path.stat().st_size
        digest = sha256_file(path)
        error = ""
    except Exception as exc:
        size = ""
        digest = ""
        error = f"读取元数据或SHA失败：{exc}"

    state.file_rows.append(
        {
            "工具版本": TOOL_VERSION,
            "来源类型": "PHYSICAL",
            "来源文件": relative_display_path(path, base_dir),
            "资源路径": relative_display_path(path, base_dir),
            "场景": scene_id,
            "扩展名": path.suffix.lower(),
            "字节数": size,
            "SHA256": digest,
            "备注": error,
        }
    )


def resource_from_physical(path: Path, base_dir: Path, scene_id: str) -> ResourceBlob | None:
    """读取需要深入解析的脚本/容器资源；SF2 在 scan_map_tree 中有单次读取的专用快速路径。"""
    try:
        data = path.read_bytes()
    except Exception:
        return None
    return ResourceBlob(
        source_kind="PHYSICAL",
        source_path=relative_display_path(path, base_dir),
        resource_path=relative_display_path(path, base_dir),
        data=data,
        scene_id=scene_id,
        container_depth=0,
    )


def analyze_resource(state: AnalyzerState, resource: ResourceBlob) -> list[ResourceBlob]:
    """根据扩展名把资源交给对应解析器；返回值是容器里继续发现的新资源。"""
    suffix = normalize_suffix(resource.resource_path)
    try:
        if suffix == ".eve":
            parse_eve(state, resource)
        elif suffix == ".sci":
            parse_sci(state, resource)
        elif suffix == ".msg":
            parse_msg(state, resource)
        elif suffix == ".sf2":
            parse_sf2(state, resource)
        elif suffix in {".dat", ".sys"}:
            return scan_container_resource(state, resource)
    except Exception as exc:
        # 这里是最后一道保险：单个解析器即使出现未预料异常，也只记录，不让其他几百张地图丢失。
        add_error(state, resource, "未捕获解析异常", f"{type(exc).__name__}: {exc}")
    return []


def scan_map_tree(base_dir: Path, map_dir: Path, state: AnalyzerState) -> None:
    """扫描 Map 目录所有物理文件，并把任务相关资源排队解析。"""
    queue: list[ResourceBlob] = []

    # rglob('*') 会递归遍历所有地图子目录；sorted 保证不同电脑上的 CSV 行顺序尽量稳定。
    for path in sorted((p for p in map_dir.rglob("*") if p.is_file()), key=lambda p: p.as_posix().casefold()):
        scene_id = extract_scene_id(relative_display_path(path, map_dir))
        suffix = path.suffix.lower()

        if suffix == ".sf2":
            # v0.1.5 的完整游戏扫描会接触大量 SF2。旧流程如果先流式算 SHA、随后又 read_bytes 解析，
            # 同一个大文件会从磁盘完整读两遍。这里改成一次 read_bytes：同一份 bytes 同时用于 SHA 与几何解析。
            try:
                data = path.read_bytes()
                display_path = relative_display_path(path, base_dir)
                state.file_rows.append(
                    {
                        "工具版本": TOOL_VERSION,
                        "来源类型": "PHYSICAL",
                        "来源文件": display_path,
                        "资源路径": display_path,
                        "场景": scene_id,
                        "扩展名": ".sf2",
                        "字节数": len(data),
                        "SHA256": sha256_bytes(data),
                        "备注": "SF2单次读取：同一份bytes用于SHA与Section0解析",
                    }
                )
                resource = ResourceBlob(
                    source_kind="PHYSICAL",
                    source_path=display_path,
                    resource_path=display_path,
                    data=data,
                    scene_id=scene_id,
                    container_depth=0,
                )
                try:
                    parse_sf2(state, resource)
                except Exception as exc:
                    add_error(state, resource, "SF2解析", f"{type(exc).__name__}: {exc}")
            except Exception as exc:
                add_error(state, None, "读取物理SF2", f"无法读取 {path}：{exc}")
            continue

        # 非 SF2 文件沿用稳定的文件清单逻辑；这里不会把无关的图片/音频完整读进内存。
        add_file_inventory_row(state, base_dir, path, scene_id)
        if suffix in {".eve", ".sci", ".msg", ".dat", ".sys"}:
            resource = resource_from_physical(path, base_dir, scene_id)
            if resource is None:
                add_error(state, None, "读取物理资源", f"无法读取 {path}")
            else:
                queue.append(resource)

    # 使用显式队列而不是递归函数，可以让“容器里还有容器”也有统一的错误处理和深度限制。
    while queue:
        resource = queue.pop(0)
        nested = analyze_resource(state, resource)

        # 容器成员也加入文件清单，让用户知道某个 Event 到底来自磁盘文件还是 DAT 内部资源。
        # SF2 已经在 scan_container_resource 内即时登记+解析，因此不会重复走到这里。
        for child in nested:
            state.file_rows.append(
                {
                    "工具版本": TOOL_VERSION,
                    "来源类型": child.source_kind,
                    "来源文件": child.source_path,
                    "资源路径": child.resource_path,
                    "场景": child.scene_id,
                    "扩展名": normalize_suffix(child.resource_path),
                    "字节数": len(child.data),
                    "SHA256": sha256_bytes(child.data),
                    "备注": f"容器深度={child.container_depth}",
                }
            )
            queue.append(child)


# ------------------------------
# 扫描结束后的跨表关联
# ------------------------------

def build_cross_indexes(
    state: AnalyzerState,
    meta_by_path: dict[str, dict[str, Any]],
) -> tuple[list[dict[str, Any]], list[dict[str, Any]], list[dict[str, Any]]]:
    """
    把详细事实拼成三个高价值聚合表。

    v0.1.2 起，这三个聚合表只使用“有效资源=1”的事实：
    - 事件对象关联：避免 SCI 散装版和 DAT 旧版同时给同一 Event 绑定两个对象；
    - GameVar 索引：避免同一脚本的旧/新两份读写被重复统计；
    - 场景摘要：计数反映当前有效视图，而不是“磁盘上所有历史候选加总”。

    被覆盖资源并没有丢，仍完整保留在详细 CSV，并可通过“任务分析_资源覆盖关系.csv”追溯。
    """
    effective_event_rows = [
        row for row in state.event_rows
        if is_effective_resource(str(row.get("EVE资源", "")), meta_by_path)
    ]
    effective_trigger_rows = [
        row for row in state.trigger_rows
        if is_effective_resource(str(row.get("对象资源", "")), meta_by_path)
    ]
    effective_var_rows = [
        row for row in state.var_rows
        if is_effective_resource(str(row.get("来源资源", "")), meta_by_path)
    ]

    # 一般一个场景只有一份有效 EVE；若未来出现不同逻辑 EVE 文件都定义了同一 Event，
    # 我们不让 dict 后写覆盖前写，而是保留第一份并在对象关联中只做事实级匹配。
    event_index: dict[tuple[str, int], dict[str, Any]] = {}
    for row in sorted(effective_event_rows, key=lambda r: (str(r.get("场景", "")), int(r.get("EventID", 0)), str(r.get("EVE资源", "")).casefold())):
        if isinstance(row.get("EventID"), int):
            event_index.setdefault((str(row.get("场景", "")), int(row["EventID"])), row)

    event_object_rows: list[dict[str, Any]] = []
    for trigger in effective_trigger_rows:
        scene = str(trigger.get("场景", ""))
        event_id = trigger.get("EventID")
        summary = event_index.get((scene, int(event_id))) if isinstance(event_id, int) else None
        event_object_rows.append(
            {
                "工具版本": TOOL_VERSION,
                "场景": scene,
                "EventID": event_id,
                "触发类型": trigger.get("触发类型", ""),
                "对象名": trigger.get("对象名", ""),
                "对象记录索引": trigger.get("对象记录索引", ""),
                "世界X": trigger.get("世界X", ""),
                "世界Y": trigger.get("世界Y", ""),
                "在场状态GameVarID": trigger.get("在场状态GameVarID", ""),
                "Event存在": 1 if summary else 0,
                "指令数": summary.get("指令数", "") if summary else "",
                "读取GameVar": summary.get("读取GameVar", "") if summary else "",
                "写入GameVar": summary.get("写入GameVar", "") if summary else "",
                "场景目标": summary.get("场景目标", "") if summary else "",
                "BattleGroup": summary.get("BattleGroup", "") if summary else "",
                "ItemID": summary.get("ItemID", "") if summary else "",
                "对象资源": trigger.get("对象资源", ""),
                "EVE资源": summary.get("EVE资源", "") if summary else "",
                "对象逻辑资源ID": trigger.get("逻辑资源ID", ""),
                "EVE逻辑资源ID": summary.get("逻辑资源ID", "") if summary else "",
                "资源策略": trigger.get("资源策略", ""),
                "证据": trigger.get("证据", ""),
            }
        )

    # GameVar 索引只统计有效 EVE。这样“Var503 被谁写”直接等于当前运行时视图，而不是补丁前后两套脚本的并集。
    var_group: dict[int, dict[str, Any]] = {}
    for row in effective_var_rows:
        var_id = row.get("GameVarID")
        if not isinstance(var_id, int):
            continue
        group = var_group.setdefault(
            var_id,
            {
                "读位置": set(),
                "写位置": set(),
                "读写位置": set(),
                "引用位置": set(),
                "操作": set(),
                "场景": set(),
            },
        )
        scene = str(row.get("场景", ""))
        event_id = row.get("EventID", "")
        instr = row.get("指令索引", "")
        location = f"{scene}:E{event_id}:I{instr}" if event_id != "" else f"{scene}:{row.get('Opcode','')}"
        access = str(row.get("访问类型", ""))
        if access == "READ":
            group["读位置"].add(location)
        elif access in {"WRITE", "WRITE_IMPLICIT"}:
            group["写位置"].add(location)
        elif access == "READ_WRITE":
            group["读写位置"].add(location)
        else:
            group["引用位置"].add(location)
        if scene:
            group["场景"].add(scene)
        if row.get("操作"):
            group["操作"].add(str(row["操作"]))

    var_index_rows: list[dict[str, Any]] = []
    for var_id in sorted(var_group):
        group = var_group[var_id]
        var_index_rows.append(
            {
                "工具版本": TOOL_VERSION,
                "资源视图": "EFFECTIVE_ONLY",
                "GameVarID": var_id,
                "场景": safe_join(sorted(group["场景"])),
                "读取次数": len(group["读位置"]),
                "写入次数": len(group["写位置"]),
                "读写次数": len(group["读写位置"]),
                "引用次数": len(group["引用位置"]),
                "读取位置": safe_join(sorted(group["读位置"])),
                "写入位置": safe_join(sorted(group["写位置"])),
                "读写位置": safe_join(sorted(group["读写位置"])),
                "引用位置": safe_join(sorted(group["引用位置"])),
                "操作摘要": safe_join(sorted(group["操作"])),
            }
        )

    # 场景摘要同样只看有效资源。MapSCI/EntitySCI/EVE/MSG 的被覆盖版本仍在详细表中，不进入这里的计数。
    effective_scene_map_rows = [r for r in state.scene_map_rows if is_effective_resource(str(r.get("资源路径", "")), meta_by_path)]
    effective_object_rows = [r for r in state.object_rows if is_effective_resource(str(r.get("资源路径", "")), meta_by_path)]
    effective_message_rows = [r for r in state.message_rows if is_effective_resource(str(r.get("MSG资源", "")), meta_by_path)]

    scene_ids: set[str] = set()
    for collection in (effective_scene_map_rows, effective_object_rows, effective_event_rows, effective_message_rows):
        for row in collection:
            scene = str(row.get("场景", ""))
            if scene:
                scene_ids.add(scene)

    scene_summary_rows: list[dict[str, Any]] = []
    for scene in sorted(scene_ids):
        map_resources = sorted({str(r["资源路径"]) for r in effective_scene_map_rows if r.get("场景") == scene})
        entity_resources = sorted({str(r["资源路径"]) for r in effective_object_rows if r.get("场景") == scene})
        eve_resources = sorted({str(r["EVE资源"]) for r in effective_event_rows if r.get("场景") == scene})
        msg_resources = sorted({str(r["MSG资源"]) for r in effective_message_rows if r.get("场景") == scene})
        scene_summary_rows.append(
            {
                "工具版本": TOOL_VERSION,
                "资源视图": "EFFECTIVE_ONLY",
                "场景": scene,
                "MapSCI资源": safe_join(map_resources),
                "EntitySCI资源": safe_join(entity_resources),
                "EVE资源": safe_join(eve_resources),
                "MSG资源": safe_join(msg_resources),
                "对象数": sum(1 for r in effective_object_rows if r.get("场景") == scene),
                "触发点数": sum(1 for r in effective_trigger_rows if r.get("场景") == scene),
                "非空Event数": sum(1 for r in effective_event_rows if r.get("场景") == scene),
                "消息数": sum(1 for r in effective_message_rows if r.get("场景") == scene),
            }
        )

    return event_object_rows, var_index_rows, scene_summary_rows

def resolve_message_texts(state: AnalyzerState, meta_by_path: dict[str, dict[str, Any]]) -> None:
    """
    把 EVE SHOW_MESSAGE 与 MSG 做资源层级一致的配对。

    v0.1.1 以前同一场景同时有散装 MSG 和 DAT 内 MSG 时，简单 dict 会让“后扫描到哪份就用哪份”，
    这会把散装 EVE 的消息引用误配到 DAT 旧文本。v0.1.2 改成：
    1. Event槽 / 消息索引必须匹配；
    2. 优先和 EVE 使用相同来源层级；
    3. 如果都是容器成员，再优先同一个外层 DAT；
    4. 当前 EVE 是有效资源时，再优先有效 MSG。
    """
    msg_index: dict[tuple[str, int, int], list[dict[str, Any]]] = {}
    for row in state.message_rows:
        scene = str(row.get("场景", ""))
        slot = row.get("Event槽")
        idx = row.get("消息索引_0基")
        if isinstance(slot, int) and isinstance(idx, int):
            msg_index.setdefault((scene, slot, idx), []).append(row)

    for ref in state.message_ref_rows:
        scene = str(ref.get("场景", ""))
        event_id = ref.get("EventID")
        msg_idx = ref.get("对应MSG消息索引_0基")
        if not isinstance(event_id, int) or not isinstance(msg_idx, int):
            continue

        candidates = msg_index.get((scene, event_id - 1, msg_idx), [])
        eve_path = str(ref.get("EVE资源", ""))
        eve_meta = meta_by_path.get(eve_path, {})

        def score(msg: dict[str, Any]) -> tuple[int, str]:
            msg_path = str(msg.get("MSG资源", ""))
            msg_meta = meta_by_path.get(msg_path, {})
            value = 0
            if msg_meta.get("资源来源层级") == eve_meta.get("资源来源层级"):
                value += 100
            if msg_meta.get("来源文件") and msg_meta.get("来源文件") == eve_meta.get("来源文件"):
                value += 50
            if eve_meta.get("有效资源") == 1 and msg_meta.get("有效资源") == 1:
                value += 20
            return value, msg_path.casefold()

        msg = sorted(candidates, key=lambda m: (-score(m)[0], score(m)[1]))[0] if candidates else None
        ref["MSG已解析"] = 1 if msg else 0
        ref["文本"] = msg.get("文本", "") if msg else ""
        ref["控制码"] = msg.get("控制码", "") if msg else ""
        ref["MSG资源"] = msg.get("MSG资源", "") if msg else ""
        ref["MSG逻辑资源ID"] = msg.get("逻辑资源ID", "") if msg else ""
        ref["MSG有效资源"] = msg.get("有效资源", "") if msg else ""
        ref["MSG配对规则"] = "同Event槽/消息索引；优先同来源层级、同外层容器、有效视图" if msg else "未找到候选"


# ------------------------------
# Public 全局数据库解析（v0.1.3）
# ------------------------------

def decode_public_enc(encoded: bytes) -> bytes:
    """
    解开 public/*.ENC 使用的旧式逐字节 XOR 包装。

    原版 `RPG.exe` 的 0x004284D0 已在主逆向项目中闭合：第 i 个字节与 `(i % 255)`
    做 XOR。XOR 是对称运算，所以同一个公式既能解密也能重新编码。

    这里专门保留一个很小、很直白的函数，而不把逻辑藏进复杂类里：
    初学者只要记住“逐个字节走过去，位置编号除以 255 取余数，再异或”即可。
    """
    decoded = bytearray(len(encoded))
    for index, value in enumerate(encoded):
        decoded[index] = value ^ (index % 255)
    return bytes(decoded)


def find_public_directory(base_dir: Path) -> Path | None:
    """
    在 multimedia 根目录下寻找 public/Public 目录。

    Windows 文件系统通常不区分大小写，但测试环境可能区分，所以不能只写死 `base_dir / "public"`。
    我们遍历一层子目录并比较小写名称，找到后立即返回。
    """
    try:
        for child in base_dir.iterdir():
            if child.is_dir() and child.name.lower() == "public":
                return child
    except OSError:
        return None
    return None


def find_case_insensitive_file(directory: Path, filename: str) -> Path | None:
    """在一个目录内按不区分大小写的文件名查找文件。"""
    wanted = filename.lower()
    try:
        for child in directory.iterdir():
            if child.is_file() and child.name.lower() == wanted:
                return child
    except OSError:
        return None
    return None


def add_public_file_inventory_row(base_dir: Path, state: AnalyzerState, path: Path, remark: str) -> None:
    """
    把 public 目录里的真实磁盘文件也加入统一“任务分析_文件清单.csv”。

    v0.1.2 的文件清单只来自 Map 扫描。v0.1.3 既然已经把 public 数据作为任务拼图的一部分，
    就必须让使用者能同时看到这些全局数据库的大小和 SHA-256，避免以后拿错版本却完全不知情。
    这里不把 .ENC/.DAT 当成 Map 任务资源参与覆盖竞争，只是登记“这次分析到底读了哪一个文件”。
    """
    relative = str(path.relative_to(base_dir)).replace("\\", "/")
    state.file_rows.append({
        "工具版本": TOOL_VERSION,
        "来源类型": "PUBLIC_PHYSICAL",
        "来源文件": relative,
        "资源路径": relative,
        "场景": "",
        "扩展名": path.suffix.lower(),
        "字节数": path.stat().st_size,
        "SHA256": sha256_file(path),
        "备注": remark,
    })


def enrich_item_names_in_map_facts(state: AnalyzerState, item_names: dict[int, str]) -> None:
    """
    用 Ail2 的“真正 ItemID”给 Map/EVE 里已经抽出的物品操作补上中文名称。

    这一步只做展示层关联，不改变任何原始 ItemID。即使名称表缺失，原来的数值事实仍然完整保留。
    事件摘要里可能一次涉及多个物品，因此它的 ItemID 是 `|` 分隔文本；这里逐个拆开再按相同顺序
    生成 Item名称，方便之后直接看 CSV 拼任务，而不用每次手工去物品定义表查编号。
    """
    for row in state.item_rows:
        item_id = row.get("ItemID")
        if isinstance(item_id, int):
            row["物品名称"] = item_names.get(item_id, "")

    for row in state.instruction_rows:
        item_id = row.get("ItemID")
        if isinstance(item_id, int):
            row["物品名称"] = item_names.get(item_id, "")

    for row in state.event_rows:
        raw = str(row.get("ItemID", "")).strip()
        if not raw:
            row["Item名称"] = ""
            continue
        names: list[str] = []
        for token in raw.split("|"):
            try:
                item_id = int(token)
            except ValueError:
                names.append("")
                continue
            names.append(item_names.get(item_id, ""))
        row["Item名称"] = safe_join(names)


def scan_public_tables(base_dir: Path, state: AnalyzerState) -> None:
    """
    解析任务系统已经能直接利用的三个 public 全局数据库。

    目前只碰“结构已经有足够证据”的部分：
    1. Ail2.ENC：700 × 926-byte 物品定义；名称 +0x00，唯一 ItemID +0x20，世界物品动作码 +0x28，长说明 +0x188。
       特别注意：ItemID 是 0..699 的唯一全排列，但有 9 条记录与物理记录索引不同，所以绝不能把
       “第几条记录”偷换成 ItemID。这个细节已经由固化5全表证明。
       v0.1.4 新增导出 +0x28：原版 0x43AEC0 已证明当该值 > 0 时，它会被写入 DataCenter+0xC4，
       再由世界主流程 0x40CE23 按动作码 3..17 分派。这个字段尤其用于发现“使用任务物品后推进 Flag”
       这种不经过 EVE SET_VAR 的隐藏任务迁移。
    2. RefineT.enc：42 × 12-byte 两物品炼化配方，三个 int32 分别是材料A、材料B、结果。
    3. RefineK.enc：17 × 17 × int32 的炼化类别矩阵；类别业务名还未恢复，所以只导出行/列索引和值。
    4. ShopItem.dat：57 × 648-byte 商店记录。57×648 的记录几何已确认；记录内 50 个物品槽/价格/
       初始状态三段布局由全文件重复结构强烈支持，但字段 consumer 尚未完全闭合，所以 CSV 明确标 B 级
       “结构候选”，绝不把商店记录号直接猜成某个城镇或 NPC，也不把候选字段伪装成作者正式命名。

    单个文件缺失或尺寸不符不会中断 Map 扫描；问题会进入“任务分析_扫描错误.csv”。
    """
    public_dir = find_public_directory(base_dir)
    if public_dir is None:
        state.error_rows.append({
            "工具版本": TOOL_VERSION,
            "来源类型": "PUBLIC",
            "来源文件": "public",
            "资源路径": "public",
            "场景": "",
            "阶段": "PUBLIC_SCAN",
            "错误": "找不到 public/Public 目录；Map 任务数据仍可正常输出。",
        })
        return

    # 先登记 public 下所有真实文件。即使某个文件暂时还没有专用解析器，也会有大小和 SHA，
    # 这样以后新增解析器时可以确认“输入版本是否与这次任务资料一致”。
    try:
        for public_file in sorted((x for x in public_dir.iterdir() if x.is_file()), key=lambda x: x.name.casefold()):
            parsed_now = public_file.name.casefold() in {"ail2.enc", "refinet.enc", "refinek.enc", "shopitem.dat"}
            add_public_file_inventory_row(
                base_dir,
                state,
                public_file,
                "v0.1.3已专用解析" if parsed_now else "已登记SHA/大小；当前版本未解析业务字段",
            )
    except OSError as exc:
        state.error_rows.append({
            "工具版本": TOOL_VERSION,
            "来源类型": "PUBLIC",
            "来源文件": "public",
            "资源路径": str(public_dir),
            "场景": "",
            "阶段": "PUBLIC_MANIFEST",
            "错误": f"枚举 public 文件失败：{exc}",
        })

    # --------------------------
    # Ail2.ENC：物品定义
    # --------------------------
    item_file = find_case_insensitive_file(public_dir, "Ail2.ENC")
    item_names: dict[int, str] = {}
    if item_file is not None:
        try:
            encoded = item_file.read_bytes()
            decoded = decode_public_enc(encoded)
            record_size = 926
            expected_count = 700
            if len(decoded) != record_size * expected_count:
                raise ValueError(f"Ail2.ENC 尺寸异常：{len(decoded)}，预期 {record_size * expected_count}")

            seen_item_ids: set[int] = set()
            for physical_index in range(expected_count):
                start = physical_index * record_size
                record = decoded[start : start + record_size]
                name = decode_cstr(record, 0x00, 36, "cp950")
                description = decode_cstr(record, 0x188, 454, "cp950")
                record_id = read_i32(record, 0x20)
                base_price_candidate = read_i32(record, 0x14)
                item_type_candidate = read_i32(record, 0x18)
                field_24 = read_i32(record, 0x24)
                # Ail2 +0x28 不是“猜出来的功能字段”。原版物品应用函数 0x43AEC0 会先读取这里：
                # 如果数值大于 0，就把它写到 DataCenter+0xC4，并把当前 ItemID 写到 DataCenter+0xEC。
                # 随后的世界主循环 0x40CE23 会按 3..17 的动作码跳转。
                # 这意味着某些关键任务物品的剧情推进根本不会出现在 EVE 的 SET_VAR 指令里。
                world_action_code = read_i32(record, 0x28)

                # 固化5已经证明 +0x20 才是原版使用的唯一 ItemID：700 条恰好形成 0..699 全排列。
                # 其中有 9 条与物理记录索引不同（例如物理509的 ItemID 是510），所以这里必须用 record_id
                # 建立名称索引。若错误地用 physical_index，恰好那 9 个物品在任务/商店/炼化表里都会被错名。
                if record_id is None or not (0 <= record_id < expected_count):
                    raise ValueError(f"Ail2.ENC 第{physical_index}条的 +0x20 ItemID 异常：{record_id}")
                if record_id in seen_item_ids:
                    raise ValueError(f"Ail2.ENC ItemID 重复：{record_id}")
                seen_item_ids.add(record_id)
                item_names[record_id] = name

                state.public_item_rows.append({
                    "工具版本": TOOL_VERSION,
                    "物理记录索引": physical_index,
                    "ItemID": record_id,
                    "ItemID与物理索引一致": 1 if record_id == physical_index else 0,
                    "名称": name,
                    "长说明": description,
                    "基础价格候选": "" if base_price_candidate is None else base_price_candidate,
                    "物品类型候选": "" if item_type_candidate is None else item_type_candidate,
                    "字段0x24": "" if field_24 is None else field_24,
                    "世界动作码0x28": "" if world_action_code is None else world_action_code,
                    "世界动作有效": 1 if world_action_code is not None and world_action_code > 0 else 0,
                    "来源资源": str(item_file.relative_to(base_dir)).replace("\\", "/"),
                    "证据": "A: 700×926；+0x20 是唯一 ItemID（0..699全排列，9条与物理索引不同）；名称+0x00、说明+0x188已确认；+0x28由0x43AEC0→0x40CE23闭合为世界物品动作码；价格/类型字段仍按候选名保留",
                })

            if seen_item_ids != set(range(expected_count)):
                missing = sorted(set(range(expected_count)) - seen_item_ids)
                raise ValueError(f"Ail2.ENC +0x20 ItemID 未形成完整0..699全排列；缺失={missing[:20]}")
        except Exception as exc:
            state.error_rows.append({
                "工具版本": TOOL_VERSION,
                "来源类型": "PUBLIC",
                "来源文件": item_file.name,
                "资源路径": str(item_file),
                "场景": "",
                "阶段": "AIL2_PARSE",
                "错误": str(exc),
            })
    else:
        state.error_rows.append({
            "工具版本": TOOL_VERSION,
            "来源类型": "PUBLIC",
            "来源文件": "Ail2.ENC",
            "资源路径": str(public_dir),
            "场景": "",
            "阶段": "AIL2_PARSE",
            "错误": "文件不存在；无法输出物品名称索引。",
        })

    # --------------------------
    # RefineT.enc：确定的炼化配方
    # --------------------------
    refine_file = find_case_insensitive_file(public_dir, "Refinet.enc")
    if refine_file is not None:
        try:
            decoded = decode_public_enc(refine_file.read_bytes())
            record_size = 12
            expected_count = 42
            if len(decoded) != record_size * expected_count:
                raise ValueError(f"Refinet.enc 尺寸异常：{len(decoded)}，预期 {record_size * expected_count}")
            for recipe_index in range(expected_count):
                start = recipe_index * record_size
                ingredient_a, ingredient_b, result_item = struct.unpack_from("<iii", decoded, start)
                state.refine_recipe_rows.append({
                    "工具版本": TOOL_VERSION,
                    "配方索引": recipe_index,
                    "材料A_ItemID": ingredient_a,
                    "材料A_名称": item_names.get(ingredient_a, ""),
                    "材料B_ItemID": ingredient_b,
                    "材料B_名称": item_names.get(ingredient_b, ""),
                    "结果_ItemID": result_item,
                    "结果_名称": item_names.get(result_item, ""),
                    "来源资源": str(refine_file.relative_to(base_dir)).replace("\\", "/"),
                    "证据": "A: 42×12；0x411960 对前两项作对称匹配，第三项为结果",
                })
        except Exception as exc:
            state.error_rows.append({
                "工具版本": TOOL_VERSION,
                "来源类型": "PUBLIC",
                "来源文件": refine_file.name,
                "资源路径": str(refine_file),
                "场景": "",
                "阶段": "REFINET_PARSE",
                "错误": str(exc),
            })

    # --------------------------
    # RefineK.enc：17×17 炼化类别矩阵
    # --------------------------
    refine_k_file = find_case_insensitive_file(public_dir, "Refinek.enc")
    if refine_k_file is not None:
        try:
            decoded = decode_public_enc(refine_k_file.read_bytes())
            side = 17
            expected_size = side * side * 4
            if len(decoded) != expected_size:
                raise ValueError(f"Refinek.enc 尺寸异常：{len(decoded)}，预期 {expected_size}")

            # 原版 consumer 直接按 matrix[a * 17 + b] 读取，因此我们忠实输出 289 个单元格。
            # 17 种类别的作者正式名称目前仍未知，故这里只使用“行类别索引/列类别索引”，不擅自命名。
            values = struct.unpack("<289i", decoded)
            for row_index in range(side):
                for column_index in range(side):
                    value = values[row_index * side + column_index]
                    state.refine_category_rows.append({
                        "工具版本": TOOL_VERSION,
                        "行类别索引": row_index,
                        "列类别索引": column_index,
                        "矩阵值": value,
                        "来源资源": str(refine_k_file.relative_to(base_dir)).replace("\\", "/"),
                        "证据": "A: RefineK.ENC=17×17×int32；原版 consumer 直接 matrix[a*17+b]；17类业务名仍UNKNOWN",
                    })
        except Exception as exc:
            state.error_rows.append({
                "工具版本": TOOL_VERSION,
                "来源类型": "PUBLIC",
                "来源文件": refine_k_file.name,
                "资源路径": str(refine_k_file),
                "场景": "",
                "阶段": "REFINEK_PARSE",
                "错误": str(exc),
            })

    # --------------------------
    # ShopItem.dat：商店初始库存的结构化候选
    # --------------------------
    shop_file = find_case_insensitive_file(public_dir, "Shopitem.dat")
    if shop_file is not None:
        try:
            raw = shop_file.read_bytes()
            record_size = 648
            record_count = 57
            if len(raw) != record_size * record_count:
                raise ValueError(f"Shopitem.dat 尺寸异常：{len(raw)}，预期 {record_size * record_count}")

            for shop_index in range(record_count):
                record = raw[shop_index * record_size : (shop_index + 1) * record_size]
                # 每条 648-byte 记录恰好是 162 个 DWORD。
                # 全 57 条记录都表现为：DWORD0=类别候选；DWORD1=有效物品数；DWORD7..56=50个ItemID；
                # DWORD57..106=对应价格；DWORD107..156=对应初始库存。因为字段消费者尚未完全静态闭合，
                # 这里把“类别/价格/库存”保留为结构候选，并把原始记录号作为稳定主键，不猜店名。
                values = struct.unpack("<162i", record)
                category_candidate = values[0]
                item_count = values[1]
                safe_count = max(0, min(50, item_count))
                for slot in range(safe_count):
                    item_id = values[7 + slot]
                    price = values[57 + slot]
                    stock = values[107 + slot]
                    state.shop_item_rows.append({
                        "工具版本": TOOL_VERSION,
                        "商店记录索引": shop_index,
                        "商店类别候选": category_candidate,
                        "记录声明物品数": item_count,
                        "槽位": slot,
                        "ItemID": item_id,
                        "物品名称": item_names.get(item_id, ""),
                        "价格候选": price,
                        "初始库存候选": stock,
                        "来源资源": str(shop_file.relative_to(base_dir)).replace("\\", "/"),
                        "证据": "B: 57×648已确认；50槽ItemID/价格/库存三段由全文件重复结构支持，店铺地点映射待闭合",
                    })
        except Exception as exc:
            state.error_rows.append({
                "工具版本": TOOL_VERSION,
                "来源类型": "PUBLIC",
                "来源文件": shop_file.name,
                "资源路径": str(shop_file),
                "场景": "",
                "阶段": "SHOPITEM_PARSE",
                "错误": str(exc),
            })

    # Ail2 名称表准备好以后，把中文物品名补回 Map/EVE 的物品操作事实。
    # 这不会改变任何事件逻辑，只是让后续“拼任务”时一眼能看出 Item451=彩石弹珠之类的关系。
    if item_names:
        enrich_item_names_in_map_facts(state, item_names)



# ------------------------------
# v0.1.5：把“事件身份”与“原版空间几何”拼成一张直接可用的表
# ------------------------------

def build_original_event_spatial_rows(
    state: AnalyzerState,
    meta_by_path: dict[str, dict[str, Any]],
) -> list[dict[str, Any]]:
    """
    生成“任务分析_原版事件空间锚点.csv”。

    这张表是本次升级最重要的产物。以前需要人工把“场景对象、触发点、场景跳转、SF2”几张表拼在一起；
    现在工具直接为每个有效 EA/EB 对象给出：
    - 对象静态 World；
    - SF2 anchor；
    - 按原版构造公式得到的 ControllerBase；
    - 对应 SF2 Section0 record0 的局部 AABB；
    - 把局部 AABB 还原到世界坐标后的 Left/Top/Right/Bottom/Center；
    - EVE 事件会不会切图、改物品、开战斗、读写哪些 GameVar。

    重要边界：
    EA 的 AABB 有直接机器码证据，但 0x004080D0 在 AABB 后还会继续像素/遮罩判定，
    因而这里称为“原版接触AABB候选”，绝不伪装成已经闭合的最终 Marker 点。
    """
    # 只使用当前资源策略下真正有效的 EVE，避免散装补丁和 DAT 原版脚本重复污染事件摘要。
    event_index: dict[tuple[str, int], dict[str, Any]] = {}
    for row in state.event_rows:
        eve_path = str(row.get("EVE资源", ""))
        event_id = row.get("EventID")
        if not is_effective_resource(eve_path, meta_by_path) or not isinstance(event_id, int):
            continue
        key = (str(row.get("场景", "")), event_id)
        event_index.setdefault(key, row)

    # SF2 geometry 已在 annotate_resource_rows 中得到“逻辑资源ID/有效资源”等字段。
    # 对一个 SF2，EA 原版路径明确选择 Section0 record0，所以这里只建立 record0 快速索引。
    geometry_index: dict[str, dict[str, Any]] = {}
    for row in state.sf2_geometry_rows:
        sf2_path = str(row.get("SF2资源", ""))
        if not is_effective_resource(sf2_path, meta_by_path):
            continue
        if row.get("Section0记录索引") != 0 or row.get("解析状态") not in {"OK", "SIZE_MISMATCH"}:
            continue
        logical_id = str(row.get("逻辑资源ID", ""))
        if logical_id:
            geometry_index.setdefault(logical_id, row)

    result: list[dict[str, Any]] = []
    for trigger in state.trigger_rows:
        object_resource = str(trigger.get("对象资源", ""))
        if not is_effective_resource(object_resource, meta_by_path):
            continue

        scene = str(trigger.get("场景", ""))
        event_id = trigger.get("EventID")
        trigger_type = str(trigger.get("触发类型", ""))
        event_summary = event_index.get((scene, int(event_id))) if isinstance(event_id, int) else None

        sf2_ref = str(trigger.get("SF2路径", "")).strip()
        sf2_logical_id = logical_resource_id(sf2_ref, scene) if sf2_ref else ""
        geometry = geometry_index.get(sf2_logical_id)

        object_x = trigger.get("世界X")
        object_y = trigger.get("世界Y")
        anchor_x = trigger.get("SF2锚点X")
        anchor_y = trigger.get("SF2锚点Y")

        controller_x: int | str = ""
        controller_y: int | str = ""
        if all(isinstance(v, int) for v in (object_x, object_y, anchor_x, anchor_y)):
            # 0x40A160 已确认：Controller 根坐标 = runtime/entity World + source SF2 anchor。
            # 对静态导出而言，EntitySCI 初始 World 是可用的静态基准；移动 NPC 的正式插件仍应优先读 runtime Entity。
            controller_x = int(object_x) + int(anchor_x)
            controller_y = int(object_y) + int(anchor_y)

        local_left = geometry.get("局部Left", "") if geometry else ""
        local_top = geometry.get("局部Top", "") if geometry else ""
        width = geometry.get("Width", "") if geometry else ""
        height = geometry.get("Height", "") if geometry else ""

        aabb_left: int | str = ""
        aabb_top: int | str = ""
        aabb_right: int | str = ""
        aabb_bottom: int | str = ""
        aabb_center_x: int | str = ""
        aabb_center_y: int | str = ""
        geometry_usable = all(isinstance(v, int) for v in (controller_x, controller_y, local_left, local_top, width, height))
        if geometry_usable:
            aabb_left = int(controller_x) + int(local_left)
            aabb_top = int(controller_y) + int(local_top)
            aabb_right = aabb_left + int(width)
            aabb_bottom = aabb_top + int(height)
            aabb_center_x = aabb_left + int(width) // 2
            aabb_center_y = aabb_top + int(height) // 2

        if trigger_type == "TOUCH_EA":
            geometry_level = "EA_STRONG_AABB"
            recommended_usage = "地点/切图/接触事件优先研究原版Section0判定几何；最终Marker仍应服从原版实际触发语义"
            geometry_evidence = "0x409A50 -> 0x4080D0 -> 0x407200(controller,0)；AABB后仍有像素/遮罩碰撞"
        elif trigger_type == "MANUAL_EB":
            geometry_level = "EB_ENTITY_ASSOCIATED"
            recommended_usage = "NPC/主动交互目标优先绑定运行时实体；这里的Section0只作为该实体原生空间几何证据"
            geometry_evidence = "EB由0x409860主动交互链读取；未把EA的Section0碰撞算法机械套成EB最终判定"
        else:
            geometry_level = "UNKNOWN"
            recommended_usage = "仅保留原始数据，等待对应原版触发链闭合"
            geometry_evidence = "未知触发类型"

        result.append(
            {
                "工具版本": TOOL_VERSION,
                "场景": scene,
                "对象记录索引": trigger.get("对象记录索引", ""),
                "对象名": trigger.get("对象名", ""),
                "触发类型": trigger_type,
                "EventID": event_id,
                "对象WorldX": object_x,
                "对象WorldY": object_y,
                "SF2锚点X": anchor_x,
                "SF2锚点Y": anchor_y,
                "ControllerBaseX候选": controller_x,
                "ControllerBaseY候选": controller_y,
                "SF2引用": sf2_ref,
                "SF2逻辑资源ID": sf2_logical_id,
                "SF2解析命中": 1 if geometry else 0,
                "SF2有效资源": geometry.get("SF2资源", "") if geometry else "",
                "Section0记录数": geometry.get("Section0记录数", "") if geometry else "",
                "Section0局部Left": local_left,
                "Section0局部Top": local_top,
                "Section0Width": width,
                "Section0Height": height,
                "Section0EntryCount": geometry.get("EntryCount", "") if geometry else "",
                "原版AABB_Left": aabb_left,
                "原版AABB_Top": aabb_top,
                "原版AABB_Right": aabb_right,
                "原版AABB_Bottom": aabb_bottom,
                "原版AABB_CenterX": aabb_center_x,
                "原版AABB_CenterY": aabb_center_y,
                "空间证据等级": geometry_level,
                "推荐用途": recommended_usage,
                "交互模式": trigger.get("交互模式", ""),
                "碰撞接近标志": trigger.get("碰撞接近标志", ""),
                "在场状态GameVarID": trigger.get("在场状态GameVarID", ""),
                "Event存在": 1 if event_summary else 0,
                "场景目标": event_summary.get("场景目标", "") if event_summary else "",
                "BattleGroup": event_summary.get("BattleGroup", "") if event_summary else "",
                "ItemID": event_summary.get("ItemID", "") if event_summary else "",
                "Item名称": event_summary.get("Item名称", "") if event_summary else "",
                "读取GameVar": event_summary.get("读取GameVar", "") if event_summary else "",
                "写入GameVar": event_summary.get("写入GameVar", "") if event_summary else "",
                "对象资源": object_resource,
                "EVE资源": event_summary.get("EVE资源", "") if event_summary else "",
                "对象逻辑资源ID": trigger.get("逻辑资源ID", ""),
                "资源策略": trigger.get("资源策略", ""),
                "空间证据": geometry_evidence,
            }
        )

    return result

# ------------------------------
# 输出所有 CSV
# ------------------------------

def write_all_outputs(base_output: Path, state: AnalyzerState, resource_policy: str) -> tuple[list[Path], dict[str, int]]:
    """
    集中定义每张 CSV 的固定字段，并在这里完成 v0.1.2 的资源有效性决策。

    详细表：保留所有原始候选，并带“有效资源/覆盖状态”列。
    聚合表：事件对象关联、GameVar索引、场景摘要只消费有效资源。
    """
    relation_rows, meta_by_path, resolution_stats = build_resource_resolution(state, resource_policy)
    annotate_resource_rows(state, meta_by_path, resource_policy)
    resolve_message_texts(state, meta_by_path)
    event_object_rows, var_index_rows, scene_summary_rows = build_cross_indexes(state, meta_by_path)
    original_event_spatial_rows = build_original_event_spatial_rows(state, meta_by_path)

    outputs: list[tuple[str, list[dict[str, Any]], list[str]]] = [
        (
            "任务分析_资源覆盖关系.csv",
            relation_rows,
            ["工具版本", "资源策略", "逻辑资源ID", "场景", "扩展名", "候选数", "来源类型", "来源文件", "资源路径", "优先级", "SHA256", "覆盖有效", "是否有效", "覆盖状态", "运行时选择状态", "运行时引用逻辑资源ID", "有效资源路径", "有效来源类型", "有效SHA256", "与有效资源内容相同", "判定原因", "证据"],
        ),
        (
            "任务分析_文件清单.csv",
            state.file_rows,
            ["工具版本", "来源类型", "来源文件", "资源路径", "场景", "扩展名", "字节数", "SHA256", "备注", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_容器目录.csv",
            state.container_rows,
            ["工具版本", "容器来源", "容器资源路径", "容器深度", "记录索引", "类型", "容器内路径", "名称", "字节数", "数据偏移", "范围有效", "名称NUL终止", "未知加6", "SHA256"],
        ),
        (
            "任务分析_场景摘要.csv",
            scene_summary_rows,
            ["工具版本", "资源视图", "场景", "MapSCI资源", "EntitySCI资源", "EVE资源", "MSG资源", "对象数", "触发点数", "非空Event数", "消息数"],
        ),
        (
            "任务分析_场景Map定义.csv",
            state.scene_map_rows,
            ["工具版本", "来源类型", "来源文件", "资源路径", "场景", "记录索引", "场景原点X", "场景原点Y", "地图宽", "地图高", "本地实体SCI引用", "EVE引用", "BGM选择器", "随机遭遇启用原值", "初始遭遇GameVarID", "存档入口许可", "Event槽2回退门控", "证据", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_场景对象.csv",
            state.object_rows,
            ["工具版本", "来源类型", "来源文件", "资源路径", "场景", "记录索引", "对象名", "世界X", "世界Y", "SF2锚点X", "SF2锚点Y", "在场启用", "交互模式", "碰撞接近标志", "渲染更新抑制", "行为类型", "SF2路径", "接触Event_EA", "主动Event_EB", "在场状态GameVarID", "记录SHA256", "证据", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_触发点.csv",
            state.trigger_rows,
            ["工具版本", "场景", "对象资源", "对象记录索引", "对象名", "触发类型", "EventID", "世界X", "世界Y", "SF2锚点X", "SF2锚点Y", "SF2路径", "交互模式", "碰撞接近标志", "在场状态GameVarID", "证据", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_SF2触发几何.csv",
            state.sf2_geometry_rows,
            ["工具版本", "来源类型", "来源文件", "SF2资源", "场景", "0x6F压缩包装", "原始大小", "重建大小", "Section0记录数", "Section1记录数", "Section2记录数", "Section3记录数", "Section0记录索引", "局部Left", "局部Top", "Width", "Height", "EntryCount", "记录大小", "结构大小吻合", "记录SHA256", "解析状态", "证据", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_原版事件空间锚点.csv",
            original_event_spatial_rows,
            ["工具版本", "场景", "对象记录索引", "对象名", "触发类型", "EventID", "对象WorldX", "对象WorldY", "SF2锚点X", "SF2锚点Y", "ControllerBaseX候选", "ControllerBaseY候选", "SF2引用", "SF2逻辑资源ID", "SF2解析命中", "SF2有效资源", "Section0记录数", "Section0局部Left", "Section0局部Top", "Section0Width", "Section0Height", "Section0EntryCount", "原版AABB_Left", "原版AABB_Top", "原版AABB_Right", "原版AABB_Bottom", "原版AABB_CenterX", "原版AABB_CenterY", "空间证据等级", "推荐用途", "交互模式", "碰撞接近标志", "在场状态GameVarID", "Event存在", "场景目标", "BattleGroup", "ItemID", "Item名称", "读取GameVar", "写入GameVar", "对象资源", "EVE资源", "对象逻辑资源ID", "资源策略", "空间证据"],
        ),
        (
            "任务分析_事件对象关联.csv",
            event_object_rows,
            ["工具版本", "场景", "EventID", "触发类型", "对象名", "对象记录索引", "世界X", "世界Y", "在场状态GameVarID", "Event存在", "指令数", "读取GameVar", "写入GameVar", "场景目标", "BattleGroup", "ItemID", "Item名称", "对象资源", "EVE资源", "对象逻辑资源ID", "EVE逻辑资源ID", "资源策略", "证据"],
        ),
        (
            "任务分析_事件摘要.csv",
            state.event_rows,
            ["工具版本", "场景", "EVE资源", "EventID", "指令数", "槽未知0", "槽大小", "Opcode序列", "读取GameVar", "写入GameVar", "场景目标", "BattleGroup", "ItemID", "Item名称", "消息索引_0基", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_事件指令.csv",
            state.instruction_rows,
            ["工具版本", "场景", "EVE资源", "EventID", "指令索引", "文件偏移", "记录大小", "Opcode", "指令名", "参数摘要", "GameVarID", "值", "跳转目标", "场景参数", "BattleGroup", "ItemID", "物品名称", "数量", "实体名", "原始记录十六进制", "识别状态", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_GameVar读写.csv",
            state.var_rows,
            ["工具版本", "场景", "EventID", "指令索引", "Opcode", "指令名", "GameVarID", "访问类型", "操作", "值或比较值", "跳转目标", "来源资源", "证据", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_GameVar索引.csv",
            var_index_rows,
            ["工具版本", "资源视图", "GameVarID", "场景", "读取次数", "写入次数", "读写次数", "引用次数", "读取位置", "写入位置", "读写位置", "引用位置", "操作摘要"],
        ),
        (
            "任务分析_事件控制流.csv",
            state.control_flow_rows,
            ["工具版本", "场景", "EventID", "起点指令", "边类型", "条件GameVarID", "比较值", "目标指令", "来源资源", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_场景跳转.csv",
            state.scene_transition_rows,
            ["工具版本", "来源场景", "EventID", "指令索引", "Opcode", "类型", "目标场景", "证据等级", "来源资源", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_战斗关联.csv",
            state.battle_rows,
            ["工具版本", "场景", "EventID", "指令索引", "LayoutGroup", "败北转MP0000_Event11标志", "败北目标指令", "来源资源", "证据", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_物品关联.csv",
            state.item_rows,
            ["工具版本", "场景", "EventID", "指令索引", "Opcode", "指令名", "ItemID", "物品名称", "数量", "类别", "跳转目标", "来源资源", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_实体操作.csv",
            state.entity_operation_rows,
            ["工具版本", "场景", "EventID", "指令索引", "Opcode", "指令名", "实体名", "X", "Y", "参数摘要", "来源资源", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_MSG消息.csv",
            state.message_rows,
            ["工具版本", "场景", "MSG资源", "Event槽", "消息索引_0基", "槽内消息数", "槽头未知DWORD", "文本", "控制码", "原始十六进制", "证据", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_事件消息引用.csv",
            state.message_ref_rows,
            ["工具版本", "场景", "EventID", "指令索引", "Opcode", "消息选择器原值_message_index_minus_one", "对应MSG消息索引_0基", "UI参数A", "UI参数B", "UI参数C", "消息选项", "MSG已解析", "文本", "控制码", "EVE资源", "MSG资源", "MSG逻辑资源ID", "MSG有效资源", "MSG配对规则", "证据", *RESOURCE_META_FIELDS],
        ),
        (
            "任务分析_物品定义.csv",
            state.public_item_rows,
            ["工具版本", "物理记录索引", "ItemID", "ItemID与物理索引一致", "名称", "长说明", "基础价格候选", "物品类型候选", "字段0x24", "世界动作码0x28", "世界动作有效", "来源资源", "证据"],
        ),
        (
            "任务分析_炼化配方.csv",
            state.refine_recipe_rows,
            ["工具版本", "配方索引", "材料A_ItemID", "材料A_名称", "材料B_ItemID", "材料B_名称", "结果_ItemID", "结果_名称", "来源资源", "证据"],
        ),
        (
            "任务分析_炼化类别矩阵.csv",
            state.refine_category_rows,
            ["工具版本", "行类别索引", "列类别索引", "矩阵值", "来源资源", "证据"],
        ),
        (
            "任务分析_商店物品.csv",
            state.shop_item_rows,
            ["工具版本", "商店记录索引", "商店类别候选", "记录声明物品数", "槽位", "ItemID", "物品名称", "价格候选", "初始库存候选", "来源资源", "证据"],
        ),
        (
            "任务分析_扫描错误.csv",
            state.error_rows,
            ["工具版本", "来源类型", "来源文件", "资源路径", "场景", "阶段", "错误"],
        ),
    ]

    written: list[Path] = []
    for filename, rows, fields in outputs:
        path = base_output / filename
        write_csv_strict(path, rows, fields)
        written.append(path)
    return written, resolution_stats


# ------------------------------
# 内建自检
# ------------------------------

def run_self_tests() -> None:
    """
    运行不依赖游戏文件的最小回归测试。

    这些测试专门守住 v0.1.2 新增的两个容易再次出错的地方：
    - `mp0601-1` 这种带连字符场景不能再被压成 `mp0601`；
    - 散装/DAT 同名资源先按覆盖优先级选代表，再由 MapSCI 的 EVE引用筛掉同场景备用脚本。

    任何 assert 失败都会让 --self-test 返回非 0，从而在以后改代码时第一时间暴露回归。
    """
    assert extract_scene_id("Map/mp0601-1/Mp0601-1.eve") == "mp0601-1"
    assert extract_scene_id("Map/mp0601-1.DAT!/mp0601-1/MPMP0601-1.SCI") == "mp0601-1"
    assert logical_resource_id("Map/Mp0101/Mp0101.eve", "mp0101") == "mp0101/mp0101.eve"
    assert logical_resource_id("Map/mp0101.DAT!/mp0101/MP0101.EVE", "mp0101") == "mp0101/mp0101.eve"

    state = AnalyzerState()
    state.file_rows = [
        {
            "工具版本": TOOL_VERSION,
            "来源类型": "PHYSICAL",
            "来源文件": "Map/mp0101/Mp0101.eve",
            "资源路径": "Map/mp0101/Mp0101.eve",
            "场景": "mp0101",
            "扩展名": ".eve",
            "字节数": 10,
            "SHA256": "A" * 64,
            "备注": "",
        },
        {
            "工具版本": TOOL_VERSION,
            "来源类型": "CONTAINER_MEMBER",
            "来源文件": "Map/mp0101.DAT",
            "资源路径": "Map/mp0101.DAT!/mp0101/MP0101.EVE",
            "场景": "mp0101",
            "扩展名": ".eve",
            "字节数": 9,
            "SHA256": "B" * 64,
            "备注": "容器深度=1",
        },
        {
            "工具版本": TOOL_VERSION,
            "来源类型": "CONTAINER_MEMBER",
            "来源文件": "Map/mp0101.DAT",
            "资源路径": "Map/mp0101.DAT!/mp0101/OLD0101.EVE",
            "场景": "mp0101",
            "扩展名": ".eve",
            "字节数": 8,
            "SHA256": "C" * 64,
            "备注": "容器深度=1",
        },
    ]
    state.scene_map_rows = [
        {
            "工具版本": TOOL_VERSION,
            "来源类型": "CONTAINER_MEMBER",
            "来源文件": "Map/mp0101.DAT",
            "资源路径": "Map/mp0101.DAT!/mp0101/MPMP0101.SCI",
            "场景": "mp0101",
            "记录索引": 0,
            "EVE引用": r"mp0101\mp0101.eve",
        }
    ]
    # MapSCI 本身也必须出现在文件清单里，覆盖解析器才知道它属于哪一层。
    state.file_rows.append(
        {
            "工具版本": TOOL_VERSION,
            "来源类型": "CONTAINER_MEMBER",
            "来源文件": "Map/mp0101.DAT",
            "资源路径": "Map/mp0101.DAT!/mp0101/MPMP0101.SCI",
            "场景": "mp0101",
            "扩展名": ".sci",
            "字节数": 0x473,
            "SHA256": "D" * 64,
            "备注": "容器深度=1",
        }
    )

    _, meta, stats = build_resource_resolution(state, "loose-first")
    assert meta["Map/mp0101/Mp0101.eve"]["有效资源"] == 1
    assert meta["Map/mp0101.DAT!/mp0101/MP0101.EVE"]["有效资源"] == 0
    assert meta["Map/mp0101.DAT!/mp0101/OLD0101.EVE"]["有效资源"] == 0
    assert meta["Map/mp0101.DAT!/mp0101/OLD0101.EVE"]["覆盖状态"] == "UNREFERENCED_BY_EFFECTIVE_MAPSCI"
    assert stats["同优先级冲突组"] == 0

    # v0.1.3 的关键防回归原则：Ail2 的 ItemID 必须来自记录 +0x20，而不能来自物理索引。
    # 这里不用真实游戏文件也能守住“名称映射字典必须以 ItemID 为 key”这一编程规则。
    synthetic_rows = [(509, 510, "骨卒"), (510, 511, "死靨鬼卒"), (511, 509, "死刀兵")]
    synthetic_item_names = {record_id: name for _physical, record_id, name in synthetic_rows}
    assert synthetic_item_names[510] == "骨卒"
    assert synthetic_item_names[509] == "死刀兵"

    # v0.1.5：用一份完全合成、没有任何商业游戏字节的最小 SF2 守住 Section0 record0 几何解析。
    synthetic_sf2 = bytearray(SF2_HEADER_SIZE + 4 + 0x5E + 29)
    synthetic_sf2[:4] = SF2_MAGIC
    # Section0: count=1, offset=0x42BC, size=4字节指针表 + 0x7B字节记录。
    struct.pack_into("<HII", synthetic_sf2, 0x14, 1, SF2_HEADER_SIZE, 4 + 0x5E + 29)
    # 其余三个 Section 保持 count=0；描述字段本来就是全零。
    struct.pack_into("<I", synthetic_sf2, SF2_HEADER_SIZE, SF2_HEADER_SIZE + 4)
    record0 = SF2_HEADER_SIZE + 4
    struct.pack_into("<iiii", synthetic_sf2, record0, -20, 10, 80, 40)
    struct.pack_into("<H", synthetic_sf2, record0 + 0x54, 1)
    synthetic_sf2_state = AnalyzerState()
    parse_sf2(
        synthetic_sf2_state,
        ResourceBlob("PHYSICAL", "Map/mp9999/test.sf2", "Map/mp9999/test.sf2", bytes(synthetic_sf2), "mp9999"),
    )
    assert len(synthetic_sf2_state.sf2_geometry_rows) == 1
    synthetic_geometry = synthetic_sf2_state.sf2_geometry_rows[0]
    assert synthetic_geometry["Section0记录索引"] == 0
    assert synthetic_geometry["局部Left"] == -20
    assert synthetic_geometry["局部Top"] == 10
    assert synthetic_geometry["Width"] == 80
    assert synthetic_geometry["Height"] == 40
    assert synthetic_geometry["EntryCount"] == 1
    assert synthetic_geometry["结构大小吻合"] == 1

    # 再守住“Entity World + SF2 anchor + Section0局部矩形 -> 原版AABB世界坐标”的拼接。
    joined_state = AnalyzerState()
    joined_state.trigger_rows = [
        {
            "场景": "mp9999",
            "对象资源": "Map/mp9999/entity.sci",
            "对象记录索引": 7,
            "对象名": "测试出口",
            "触发类型": "TOUCH_EA",
            "EventID": 12,
            "世界X": 1000,
            "世界Y": 2000,
            "SF2锚点X": -320,
            "SF2锚点Y": -260,
            "SF2路径": r"MP9999\test.sf2",
            "交互模式": 0,
            "碰撞接近标志": 0,
            "在场状态GameVarID": 0,
        }
    ]
    joined_state.sf2_geometry_rows = [
        {
            "SF2资源": "Map/mp9999/test.sf2",
            "Section0记录索引": 0,
            "解析状态": "OK",
            "Section0记录数": 1,
            "局部Left": -20,
            "局部Top": 10,
            "Width": 80,
            "Height": 40,
            "EntryCount": 1,
        }
    ]
    joined_state.event_rows = [
        {
            "场景": "mp9999",
            "EVE资源": "Map/mp9999/test.eve",
            "EventID": 12,
            "场景目标": "mp0001",
            "BattleGroup": "",
            "ItemID": "",
            "Item名称": "",
            "读取GameVar": "",
            "写入GameVar": "",
        }
    ]
    joined_meta = {
        "Map/mp9999/entity.sci": {"有效资源": 1, "逻辑资源ID": "mp9999/entity.sci", "资源策略": "loose-first"},
        "Map/mp9999/test.sf2": {"有效资源": 1, "逻辑资源ID": "mp9999/test.sf2", "资源策略": "loose-first"},
        "Map/mp9999/test.eve": {"有效资源": 1, "逻辑资源ID": "mp9999/test.eve", "资源策略": "loose-first"},
    }
    annotate_resource_rows(joined_state, joined_meta, "loose-first")
    joined_rows = build_original_event_spatial_rows(joined_state, joined_meta)
    assert len(joined_rows) == 1
    joined = joined_rows[0]
    # ControllerBase=(680,1740)，再叠加 local(-20,+10) 得到左上=(660,1750)，中心=(700,1770)。
    assert joined["ControllerBaseX候选"] == 680
    assert joined["ControllerBaseY候选"] == 1740
    assert joined["原版AABB_Left"] == 660
    assert joined["原版AABB_Top"] == 1750
    assert joined["原版AABB_CenterX"] == 700
    assert joined["原版AABB_CenterY"] == 1770
    assert joined["场景目标"] == "mp0001"

    synthetic_state = AnalyzerState()
    synthetic_state.item_rows = [{"ItemID": 510}]
    synthetic_state.instruction_rows = [{"ItemID": 509}]
    synthetic_state.event_rows = [{"ItemID": "509|510"}]
    enrich_item_names_in_map_facts(synthetic_state, synthetic_item_names)
    assert synthetic_state.item_rows[0]["物品名称"] == "骨卒"
    assert synthetic_state.instruction_rows[0]["物品名称"] == "死刀兵"
    assert synthetic_state.event_rows[0]["Item名称"] == "死刀兵|骨卒"


# ------------------------------
# 主程序
# ------------------------------

def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="扫描《幽城幻剑录》multimedia/Map 与 public 已确认数据库，导出任务相关严格 CSV。",
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=None,
        help="multimedia 根目录。默认就是本脚本所在目录。",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=None,
        help="CSV 输出目录。默认在脚本所在 multimedia 下新建“任务分析_0.1.5_原版事件空间数据”；用户正常使用无需指定。",
    )
    parser.add_argument(
        "--resource-policy",
        choices=RESOURCE_POLICY_CHOICES,
        default="loose-first",
        help="同一逻辑 EVE/SCI/MSG/SF2 同时有散装和 DAT 成员时的有效资源策略。默认 loose-first；container-first 仅用于逆向诊断。",
    )
    parser.add_argument(
        "--self-test",
        action="store_true",
        help="只运行内建资源覆盖/场景识别回归测试，不扫描游戏，也不写 CSV。",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)

    if args.self_test:
        run_self_tests()
        print(f"CastleScriptAnalyzer v{TOOL_VERSION} [SELFTEST] PASS")
        return 0

    # 用户要求工具就放在 multimedia 目录里，所以默认 root 与 output 都取脚本自身目录。
    script_dir = Path(__file__).resolve().parent
    base_dir = (args.root.resolve() if args.root else script_dir)
    output_dir = (args.output.resolve() if args.output else script_dir / "任务分析_0.1.5_原版事件空间数据")
    output_dir.mkdir(parents=True, exist_ok=True)

    print(f"CastleScriptAnalyzer v{TOOL_VERSION}")
    print(f"[ROOT] {base_dir}")
    print(f"[OUT ] {output_dir}")
    print(f"[POL ] {args.resource_policy}")

    map_dir = find_map_directory(base_dir)
    if map_dir is None:
        print("[ERROR] 在工具运行目录下找不到 Map/map 目录。")
        print("        请把 CastleScriptAnalyzer.py 放到 multimedia 目录后再运行。")
        return 2

    print(f"[MAP ] {map_dir}")
    state = AnalyzerState()

    # 整个扫描过程只读 Map 资源；CSV 写入发生在最后，任何单文件错误都由 add_error 收集。
    scan_map_tree(base_dir, map_dir, state)
    # v0.1.3：地图扫描结束后再解析 public 全局表。它们不参与 Map 资源覆盖决策，但会补齐物品名、炼化配方和商店库存事实。
    scan_public_tables(base_dir, state)
    written, resolution_stats = write_all_outputs(output_dir, state, args.resource_policy)

    print("[DONE] 扫描完成。")
    print(f"       物理/容器资源行：{len(state.file_rows)}")
    print(f"       场景对象：{len(state.object_rows)}")
    print(f"       SF2 Section0几何行：{len(state.sf2_geometry_rows)}")
    print(f"       非空 Event：{len(state.event_rows)}")
    print(f"       EVE 指令：{len(state.instruction_rows)}")
    print(f"       GameVar 事实：{len(state.var_rows)}")
    print(f"       物品定义：{len(state.public_item_rows)}")
    print(f"       炼化配方：{len(state.refine_recipe_rows)}")
    print(f"       炼化类别矩阵：{len(state.refine_category_rows)}")
    print(f"       商店物品行：{len(state.shop_item_rows)}")
    print(f"       扫描警告/错误：{len(state.error_rows)}")
    print(f"       逻辑任务资源：{resolution_stats['逻辑资源数']}")
    print(f"       多来源资源组：{resolution_stats['多候选逻辑资源数']}")
    print(f"       内容不同覆盖组：{resolution_stats['内容不同覆盖组']}")
    print(f"       同优先级冲突组：{resolution_stats['同优先级冲突组']}")
    print(f"       MapSCI未引用EVE：{resolution_stats['未被MapSCI引用EVE数']}")
    print(f"       非配对MSG：{resolution_stats['未被MapSCI配对MSG数']}")
    print(f"       CSV 数量：{len(written)}")
    print(f"\n请把整个输出目录压缩后交回：{output_dir}")
    print("不要只挑一张 CSV；尤其必须保留“任务分析_原版事件空间锚点.csv”和“任务分析_SF2触发几何.csv”。")
    return 0


if __name__ == "__main__":
    # sys.exit 会把 main 的退出码交给命令行：0=成功，2=运行目录不对，其他未捕获错误由 Python 自己报告。
    sys.exit(main())
