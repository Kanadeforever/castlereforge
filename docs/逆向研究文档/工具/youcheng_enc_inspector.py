#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
《幽城幻剑录》ENC 调查器
版本：0.7D.62-固化62

只使用 Python 标准库。默认只读输入；“编码/解码”都要求显式指定输出文件，绝不原地覆盖。
算法已经由 RPG.exe 0x004284D0 静态恢复并由 13/13 样本全文件字节级验证：
    data[i] ^= (i % 255)
即 key 0..254 循环，周期 255。XOR 对称，因此编码与解码相同。
"""
# 启用较新的 Python 注解语法行为，让类型注解先按文字保存，避免旧版本在定义阶段立即求值。
from __future__ import annotations
# 导入 `argparse, csv, hashlib, json, struct, sys` 模块；这里只取得标准库功能，不会修改游戏文件。
import argparse, csv, hashlib, json, struct, sys
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path
# 从 `typing` 模块导入 `Any`，后面的代码会直接使用这些现成组件。
from typing import Any

# 固化62更新：ENC算法本身没有变化；这里只提升工具版本，并把LayoutGR+0x9C的已证业务语义同步进公共数据库schema。
# 这样以后单独拿这份工具导出schema时，不会继续看到固化16时代“只知道有getter”的过期字段名。
TOOL_VERSION = "0.7D.62-固化62"

# 证据级别：A=已确认；B=强证据；C=候选；U=未知。
# record_size=None 表示固定 Blob，不允许伪造固定记录数组。
# 创建变量 `TABLE_SPECS`（类型提示为 `dict[str, dict[str, Any]]`），并把 `{` 的结果保存进去供后续步骤使用。
TABLE_SPECS: dict[str, dict[str, Any]] = {
    # 给当前结果字典的“levelup”字段填写 `{"文件":"Levelup.enc","名称":"等级累计经验阈值表","身份证据":"A","record_size":4,"reco…`，这样导出的 JSON/表格能保留这一项证据。
    "levelup": {"文件":"Levelup.enc","名称":"等级累计经验阈值表","身份证据":"A","record_size":4,"record_count":101,"canonical":"LevelUpThresholdTable",
        # 给当前结果字典的“字段”字段填写 `[{"offset":0,"type":"int32","name":"cumulative_exp_threshold","eviden…`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[{"offset":0,"type":"int32","name":"cumulative_exp_threshold","evidence":"A","note":"101项数组；消费者按角色等级索引。"}]},
    # 给当前结果字典的“magictb”字段填写 `{"文件":"Magictb.enc","名称":"角色招式/术法学习条件表","身份证据":"A","record_size":36,"…`，这样导出的 JSON/表格能保留这一项证据。
    "magictb": {"文件":"Magictb.enc","名称":"角色招式/术法学习条件表","身份证据":"A","record_size":36,"record_count":102,"canonical":"TechniqueLearningConditionTable",
        # 给当前结果字典的“字段”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x00,"type":"int32","name":"role_id","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x04,"type":"int32","name":"early_unlock_level","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x08,"type":"int32","name":"automatic_unlock_level","evidence":"A"},
            # 继续填写当前数据结构或参数列表中的一项：`*[{"offset":o,"type":"int32","name":f"stat_min_{i}","evidence":"A","note":f"与Runtime Role +0x{…`。
            *[{"offset":o,"type":"int32","name":f"stat_min_{i}","evidence":"A","note":f"与Runtime Role +0x{ro:02X}比较；业务属性名待角色结构恢复。"} for i,(o,ro) in enumerate(zip((0x0C,0x10,0x14,0x18,0x1C),(0x54,0x58,0x5C,0x60,0x64)))],
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x20,"type":"int32","name":"technique_id","evidence":"A","note":"传给0x423100去重加入招式列表。"},
        ]},
    # 给当前结果字典的“magiccon”字段填写 `{"文件":"Magiccon.enc","名称":"命名招式/术法条件与AI使用条件表","身份证据":"B","record_size…`，这样导出的 JSON/表格能保留这一项证据。
    "magiccon": {"文件":"Magiccon.enc","名称":"命名招式/术法条件与AI使用条件表","身份证据":"B","record_size":64,"record_count":500,"canonical":"TechniqueConditionTable",
        # 给当前结果字典的“字段”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x00,"type":"int32","name":"id","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x04,"type":"cp950[20]","name":"name","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x18,"type":"int32","name":"condition_18","evidence":"B","note":"AI/动作筛选中的阈值/百分比类字段。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x1C,"type":"int32","name":"condition_1C","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x20,"type":"int32","name":"condition_20","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x24,"type":"int32","name":"random_divisor_or_weight","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x28,"type":"int32","name":"flag_28","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x2C,"type":"int32","name":"mode_2C","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x30,"type":"int32","name":"count_or_limit_30","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x34,"type":"int32","name":"mode_34","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x38,"type":"int32","name":"param_38","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x3C,"type":"int32","name":"reserved_3C","evidence":"A","note":"当前500条样本均为-1。"},
        ]},
    # 给当前结果字典的“enemy_ai”字段填写 `{"文件":"Enemy_ai.enc","名称":"敌人AI决策定义表","身份证据":"A","record_size":56,"re…`，这样导出的 JSON/表格能保留这一项证据。
    "enemy_ai": {"文件":"Enemy_ai.enc","名称":"敌人AI决策定义表","身份证据":"A","record_size":56,"record_count":283,"canonical":"EnemyAiDefinitionTable",
        # 给当前结果字典的“字段”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x00,"type":"int32","name":"id","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x04,"type":"cp950[20]","name":"name","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x18,"type":"int32","name":"decision_weight_0","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x1C,"type":"int32","name":"decision_weight_1","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x20,"type":"int32","name":"decision_weight_2","evidence":"A","note":"0x444240系列消费者用随机0..99按权重分区；三结果业务名仍待战斗阶段。"},
            # 继续填写当前数据结构或参数列表中的一项：`*[{"offset":o,"type":"int32","name":f"reserved_{o:02X}","evidence":"A","note":"当前283条记录全部为0。"}…`。
            *[{"offset":o,"type":"int32","name":f"reserved_{o:02X}","evidence":"A","note":"当前283条记录全部为0。"} for o in (0x24,0x28,0x2C,0x30,0x34)],
        ]},
    # 给当前结果字典的“refinet”字段填写 `{"文件":"Refinet.enc","名称":"两物品炼化配方表","身份证据":"A","record_size":12,"reco…`，这样导出的 JSON/表格能保留这一项证据。
    "refinet": {"文件":"Refinet.enc","名称":"两物品炼化配方表","身份证据":"A","record_size":12,"record_count":42,"canonical":"RefineRecipeTable",
        # 给当前结果字典的“字段”字段填写 `[{"offset":0,"type":"int32","name":"ingredient_a_item_id","evidence":…`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[{"offset":0,"type":"int32","name":"ingredient_a_item_id","evidence":"A"},{"offset":4,"type":"int32","name":"ingredient_b_item_id","evidence":"A"},{"offset":8,"type":"int32","name":"result_item_id","evidence":"A","note":"0x411960对前两项作对称匹配。"}]},
    # 给当前结果字典的“refinek”字段填写 `{"文件":"Refinek.enc","名称":"17×17炼化类别/映射矩阵","身份证据":"A","record_size":4,…`，这样导出的 JSON/表格能保留这一项证据。
    "refinek": {"文件":"Refinek.enc","名称":"17×17炼化类别/映射矩阵","身份证据":"A","record_size":4,"record_count":289,"canonical":"RefineMatrix17x17",
        # 给当前结果字典的“形状”字段填写 `[17,17],"字段":[{"offset":0,"type":"int32","name":"value","evidence":"A…`，这样导出的 JSON/表格能保留这一项证据。
        "形状":[17,17],"字段":[{"offset":0,"type":"int32","name":"value","evidence":"A","note":"按 a*17+b 直接索引；行列业务类别名待恢复。"}]},
    # 给当前结果字典的“api”字段填写 `{"文件":"Api.enc","名称":"角色基础定义表","身份证据":"A","record_size":848,"record_c…`，这样导出的 JSON/表格能保留这一项证据。
    "api": {"文件":"Api.enc","名称":"角色基础定义表","身份证据":"A","record_size":848,"record_count":255,"canonical":"RoleDefinitionTable",
        # 给当前结果字典的“字段”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x00,"type":"int32","name":"role_id_or_record_id","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x0C,"type":"cp950","size":20,"name":"name","evidence":"A"},
            # 继续填写当前数据结构或参数列表中的一项：`*[{"offset":o,"type":"ascii[10]","name":f"action_resource_{label}","evidence":"A","note":"按原前缀…`。
            *[{"offset":o,"type":"ascii[10]","name":f"action_resource_{label}","evidence":"A","note":"按原前缀保留槽名；最终动画业务语义在SCI/SF2阶段冻结。"} for o,label in ((648,"STN"),(658,"USE"),(668,"WEK"),(678,"HIT"),(688,"SLOT688"),(698,"DEF"),(708,"SLOT708"),(718,"MOV"),(728,"BAK"),(738,"RED"),(748,"SLOT748"),(758,"MAG"))],
        # 继续填写当前数据结构或参数列表中的一项：`],"说明":"Loader/初始化代码把单条API记录用 rep movs 0xD4 DWORD 原样复制成0x350-byte Runtime Role Record。"},`。
        ],"说明":"Loader/初始化代码把单条API记录用 rep movs 0xD4 DWORD 原样复制成0x350-byte Runtime Role Record。"},
    # 给当前结果字典的“ail2”字段填写 `{"文件":"Ail2.ENC","名称":"物品定义表","身份证据":"A","record_size":926,"record_co…`，这样导出的 JSON/表格能保留这一项证据。
    "ail2": {"文件":"Ail2.ENC","名称":"物品定义表","身份证据":"A","record_size":926,"record_count":700,"canonical":"ItemDefinitionTable",
        # 给当前结果字典的“字段”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x00,"type":"cp950","size":36,"name":"name","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x24,"type":"int32","name":"field_24","evidence":"B","note":"消费者已访问，业务名待Inventory/Equipment阶段。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x188,"type":"cp950","size":454,"name":"description","evidence":"A","note":"长说明文本固定起点。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x34E,"type":"int32","name":"field_34E","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x372,"type":"int32","name":"field_372","evidence":"B","note":"消费者与常量17比较。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x376,"type":"int32","name":"field_376","evidence":"B"},
        # 执行一次赋值：把 `926字节固定stride；旧文档若将Ail2解释为角色/敌人表均已废止。"},` 的结果放入左侧 `],"说明":"0x39E` 对应的变量/字段。
        ],"说明":"0x39E=926字节固定stride；旧文档若将Ail2解释为角色/敌人表均已废止。"},
    # 给当前结果字典的“firttech”字段填写 `{"文件":"Firttech.enc","名称":"招式/术法动作定义表","身份证据":"A","record_size":560,"…`，这样导出的 JSON/表格能保留这一项证据。
    "firttech": {"文件":"Firttech.enc","名称":"招式/术法动作定义表","身份证据":"A","record_size":560,"record_count":500,"canonical":"TechniqueDefinitionTable",
        # 给当前结果字典的“字段”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x18,"type":"int32","name":"field_18","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x20,"type":"int32","name":"mode_20","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x34,"type":"int32","name":"gate_34","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x38,"type":"int32","name":"resource_cost_38","evidence":"B","note":"至少一个消费路径从Runtime Role +0x38扣除此值；资源业务名待角色/战斗阶段。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x40,"type":"int32","name":"percent_40","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x44,"type":"int32","name":"mode_44","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x4C,"type":"int32","name":"base_amount_4C","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x54,"type":"int32","name":"resource_percent_a_54","evidence":"B"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x58,"type":"int32","name":"resource_percent_b_58","evidence":"B"},
            # 继续填写当前数据结构或参数列表中的一项：`*[{"offset":o,"type":"ascii[10]","name":f"action_resource_{label}","evidence":"A"} for o,label…`。
            *[{"offset":o,"type":"ascii[10]","name":f"action_resource_{label}","evidence":"A"} for o,label in ((156,"RED"),(166,"MOV"),(176,"ATT"),(186,"MIS"),(196,"BAK"),(206,"SLOT206"),(216,"SLOT216"),(230,"SHO1候选"),(240,"SHO2候选"),(250,"EFFA"),(260,"EFFM"))],
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x1EC,"type":"int32","name":"result_or_override_id_1EC","evidence":"B"},
        ]},
    # 给当前结果字典的“layoutteam”字段填写 `{"文件":"LayoutTeam.ENC","名称":"布局/编队完整载荷表","身份证据":"A","record_size":508…`，这样导出的 JSON/表格能保留这一项证据。
    "layoutteam": {"文件":"LayoutTeam.ENC","名称":"布局/编队完整载荷表","身份证据":"A","record_size":508,"record_count":334,"canonical":"LayoutTeamPayloadTable","字段":[],"说明":"每条127 DWORD；0x424430按LayoutGR选定索引后rep movs 0x7F DWORD完整复制到输出。"},
    # 给当前结果字典的“layoutgr”字段填写 `{"文件":"Layoutgr.enc","名称":"布局组/剧情场景组定义表","身份证据":"A","record_size":508…`，这样导出的 JSON/表格能保留这一项证据。
    "layoutgr": {"文件":"Layoutgr.enc","名称":"布局组/剧情场景组定义表","身份证据":"A","record_size":508,"record_count":250,"canonical":"LayoutGroupDefinitionTable",
        # 给当前结果字典的“字段”字段填写 `[`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x18,"type":"int32","name":"layout_team_choice_count","evidence":"A","note":"0x424430直接作为LayoutTeam候选数量；0时失败，非0时RNG%count。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x1C,"type":"int32[]","name":"layout_team_ids","evidence":"A","count_from_offset":0x18,"note":"0x424430按random_index从此数组读取实际LayoutTeam ID。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x44,"type":"cp950","size":40,"name":"name","evidence":"A"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x6C,"type":"int32","name":"secondary_choice_count","evidence":"A","note":"0x424430复制LayoutTeam后读取的第二候选数量；>0时再次RNG%count。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x70,"type":"int32[]","name":"secondary_choice_values","evidence":"A","count_from_offset":0x6C,"note":"0x424430按第二随机索引读取并作为helper返回值；具体业务语义仍待证明。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x88,"type":"int32","name":"field_88","evidence":"B","note":"有独立getter 0x424590。"},
            # 开始构造一个新的数据结构；后续缩进行会逐项填写其中的内容。
            {"offset":0x9C,"type":"int32","name":"escape_success_threshold","evidence":"A","note":"固化62：0x424560在FIRTTECH 0x1EA「戰場脫逃」专用路径读取；随后原版固定执行legacy rand15()%100，并以余数严格小于本值判成功。作者正式数据库列名仍UNKNOWN，因此该name是兼容层中性名。"},
        # 继续填写当前数据结构或参数列表中的一项：`],"说明":"0x424430校验GR索引；+0x18为LayoutTeam候选数，+0x1C为候选ID数组并从中随机选择后复制完整508-byte LayoutTeam payload…`。
        ],"说明":"0x424430校验GR索引；+0x18为LayoutTeam候选数，+0x1C为候选ID数组并从中随机选择后复制完整508-byte LayoutTeam payload；+0x6C/+0x70是第二组随机候选计数/值数组，返回值业务语义仍待证明。旧版把+0x18误命名为primary LayoutTeam ID，已在固化16撤销。"},
    # 给当前结果字典的“battlegb”字段填写 `{"文件":"Battlegb.enc","名称":"战斗背景/环境定义表","身份证据":"A","record_size":94,"r…`，这样导出的 JSON/表格能保留这一项证据。
    "battlegb": {"文件":"Battlegb.enc","名称":"战斗背景/环境定义表","身份证据":"A","record_size":94,"record_count":66,"canonical":"BattleBackgroundDefinitionTable",
        # 给当前结果字典的“字段”字段填写 `[{"offset":0x00,"type":"int32","name":"id","evidence":"A"},{"offset":…`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[{"offset":0x00,"type":"int32","name":"id","evidence":"A"},{"offset":0x04,"type":"ascii","size":20,"name":"sf2_resource","evidence":"A"},{"offset":0x18,"type":"cp950","size":70,"name":"description","evidence":"A"}]},
    # 给当前结果字典的“miscinfo”字段填写 `{"文件":"Miscinfo.enc","名称":"杂项固定配置Blob","身份证据":"A","record_size":None,…`，这样导出的 JSON/表格能保留这一项证据。
    "miscinfo": {"文件":"Miscinfo.enc","名称":"杂项固定配置Blob","身份证据":"A","record_size":None,"record_count":None,"blob_size":1054,"canonical":"MiscInfoBlob",
        # 给当前结果字典的“字段”字段填写 `[{"offset":0x04,"type":"ascii/cp950","name":"null_string","evidence":…`，这样导出的 JSON/表格能保留这一项证据。
        "字段":[{"offset":0x04,"type":"ascii/cp950","name":"null_string","evidence":"A"},
              # 继续填写当前数据结构或参数列表中的一项：`*[{"offset":o,"type":"ascii/cp950","name":f"stone_resource_{i}","evidence":"A"} for i,o in enu…`。
              *[{"offset":o,"type":"ascii/cp950","name":f"stone_resource_{i}","evidence":"A"} for i,o in enumerate((0x36,0x68,0x9A,0xCC,0xFE),1)],
              # 继续填写当前数据结构或参数列表中的一项：`*[{"offset":o,"type":"ascii/cp950","name":f"music_resource_{i}","evidence":"A"} for i,o in enu…`。
              *[{"offset":o,"type":"ascii/cp950","name":f"music_resource_{i}","evidence":"A"} for i,o in enumerate((0x1FC,0x22E,0x260,0x292),30)]],
        # 给当前结果字典的“说明”字段填写 `"Loader要求精确0x41E字节；不是固定记录数组。未被xref证明的剩余字节保持opaque。"},`，这样导出的 JSON/表格能保留这一项证据。
        "说明":"Loader要求精确0x41E字节；不是固定记录数组。未被xref证明的剩余字节保持opaque。"},
}

# 定义函数 `xor_crypt`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def xor_crypt(data: bytes) -> bytes:
    # 把 `bytes((b ^ (i % 255)) for i,b in enumerate(data))` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return bytes((b ^ (i % 255)) for i,b in enumerate(data))

# 定义函数 `sha256`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sha256(data: bytes) -> str:
    # 把 `hashlib.sha256(data).hexdigest()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return hashlib.sha256(data).hexdigest()

# 定义函数 `norm_key`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def norm_key(name: str) -> str:
    # 把右侧 `Path(name).stem.lower().replace('-','_')` 计算得到的值保存到 `n`，后面的判断或输出会继续使用它。
    n=Path(name).stem.lower().replace('-','_')
    # 把右侧 `{"enemy_ai":"enemy_ai","layoutteam":"layoutteam","layoutgr":"layoutgr","le…` 计算得到的值保存到 `aliases`，后面的判断或输出会继续使用它。
    aliases={"enemy_ai":"enemy_ai","layoutteam":"layoutteam","layoutgr":"layoutgr","levelup":"levelup","magictb":"magictb","magiccon":"magiccon","refinet":"refinet","refinek":"refinek","api":"api","ail2":"ail2","firttech":"firttech","battlegb":"battlegb","miscinfo":"miscinfo"}
    # 把 `aliases.get(n,n)` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return aliases.get(n,n)

# 定义函数 `spec_for`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def spec_for(path_or_name: str|Path) -> tuple[str,dict[str,Any]]:
    # 把右侧 `norm_key(str(path_or_name))` 计算得到的值保存到 `key`，后面的判断或输出会继续使用它。
    key=norm_key(str(path_or_name))
    # 检查条件 `key not in TABLE_SPECS`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if key not in TABLE_SPECS:
        # 检测到不能继续的情况，主动抛出异常 `KeyError(f"未知ENC表：{path_or_name}")`，让上层明确知道数据或参数不符合要求。
        raise KeyError(f"未知ENC表：{path_or_name}")
    # 把 `key,TABLE_SPECS[key]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return key,TABLE_SPECS[key]

# 定义函数 `maybe_decode`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def maybe_decode(path: Path, assume_decoded: bool=False) -> bytes:
    # 把右侧 `path.read_bytes()` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b=path.read_bytes()
    # 检查条件 `assume_decoded or path.suffix.lower()=='.bin'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if assume_decoded or path.suffix.lower()=='.bin': return b
    # 把 `xor_crypt(b)` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return xor_crypt(b)

# 定义函数 `read_field`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def read_field(rec: bytes, f: dict[str,Any]) -> Any:
    # 把右侧 `f['offset']; typ=f['type']; size=f.get('size')` 计算得到的值保存到 `o`，后面的判断或输出会继续使用它。
    o=f['offset']; typ=f['type']; size=f.get('size')
    # 检查条件 `typ=='int32'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if typ=='int32': return struct.unpack_from('<i',rec,o)[0]
    # 检查条件 `typ.startswith('cp950') or typ.startswith('ascii')`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if typ.startswith('cp950') or typ.startswith('ascii'):
        # 检查条件 `size is None`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if size is None:
            # bracket form e.g. cp950[20]
            # 检查条件 `'[' in typ`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if '[' in typ: size=int(typ.split('[',1)[1].split(']',1)[0])
            # 前面的条件分支都没有命中时，执行这个兜底分支。
            else: size=len(rec)-o
        # 把右侧 `rec[o:o+size].split(b'\0',1)[0]` 计算得到的值保存到 `raw`，后面的判断或输出会继续使用它。
        raw=rec[o:o+size].split(b'\0',1)[0]
        # 把右侧 `'cp950' if typ.startswith('cp950') else 'ascii'` 计算得到的值保存到 `enc`，后面的判断或输出会继续使用它。
        enc='cp950' if typ.startswith('cp950') else 'ascii'
        # 把 `raw.decode(enc,'replace')` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return raw.decode(enc,'replace')
    # 检查条件 `typ.startswith('int32[]')`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if typ.startswith('int32[]'):
        # 把右侧 `f.get('count_from_offset')` 计算得到的值保存到 `count_from`，后面的判断或输出会继续使用它。
        count_from=f.get('count_from_offset')
        # 检查条件 `count_from is not None`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if count_from is not None:
            # 把右侧 `struct.unpack_from('<i',rec,count_from)[0]` 计算得到的值保存到 `count`，后面的判断或输出会继续使用它。
            count=struct.unpack_from('<i',rec,count_from)[0]
            # 把右侧 `max(0,min(count,(len(rec)-o)//4))` 计算得到的值保存到 `count`，后面的判断或输出会继续使用它。
            count=max(0,min(count,(len(rec)-o)//4))
            # 把 `[struct.unpack_from('<i',rec,o+i*4)[0] for i in range(count)]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
            return [struct.unpack_from('<i',rec,o+i*4)[0] for i in range(count)]
        # 把 `[struct.unpack_from('<i',rec,p)[0] for p in range(o,len(rec)-3,4)]` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
        return [struct.unpack_from('<i',rec,p)[0] for p in range(o,len(rec)-3,4)]
    # 把 `rec[o:o+(size or 4)].hex()` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return rec[o:o+(size or 4)].hex()

# 定义函数 `inspect`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def inspect(path: Path, assume_decoded: bool=False) -> dict[str,Any]:
    # 执行一次赋值：把 `spec_for(path)` 的结果放入左侧 `key,sp` 对应的变量/字段。
    key,sp=spec_for(path)
    # 把右侧 `maybe_decode(path,assume_decoded)` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
    b=maybe_decode(path,assume_decoded)
    # 把右侧 `{"工具版本":TOOL_VERSION,"文件":str(path),"表":key,"名称":sp['名称'],"Canonical":sp['…` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
    out={"工具版本":TOOL_VERSION,"文件":str(path),"表":key,"名称":sp['名称'],"Canonical":sp['canonical'],"解码后大小":len(b),"SHA256":sha256(b),"身份级别":sp['身份证据']}
    # 把右侧 `sp.get('record_size'); rc=sp.get('record_count')` 计算得到的值保存到 `rs`，后面的判断或输出会继续使用它。
    rs=sp.get('record_size'); rc=sp.get('record_count')
    # 检查条件 `rs is None`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if rs is None:
        # 调用 `out.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        out.update({"类型":"blob","预期大小":sp.get('blob_size'),"大小符合":len(b)==sp.get('blob_size')})
    # 前面的条件分支都没有命中时，执行这个兜底分支。
    else:
        # 调用 `out.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        out.update({"类型":"fixed_records","记录大小":rs,"预期记录数":rc,"实际记录数":len(b)//rs if rs else None,"整除":len(b)%rs==0,"几何符合":len(b)==rs*rc})
    # 把 `out` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return out

# 定义函数 `export_records`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def export_records(path: Path, assume_decoded: bool=False) -> list[dict[str,Any]]:
    # 执行一次赋值：把 `spec_for(path); b=maybe_decode(path,assume_decoded); rs=sp.get('record_siz…` 的结果放入左侧 `key,sp` 对应的变量/字段。
    key,sp=spec_for(path); b=maybe_decode(path,assume_decoded); rs=sp.get('record_size')
    # 检查条件 `rs is None`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if rs is None: raise ValueError(f"{sp['文件']} 是固定Blob，禁止伪造记录数组")
    # 检查条件 `len(b)%rs`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
    if len(b)%rs: raise ValueError(f"文件大小 {len(b)} 不能被记录大小 {rs} 整除")
    # 把右侧 `[]` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows=[]
    # 开始循环 `i in range(len(b)//rs)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for i in range(len(b)//rs):
        # 把右侧 `b[i*rs:(i+1)*rs]` 计算得到的值保存到 `rec`，后面的判断或输出会继续使用它。
        rec=b[i*rs:(i+1)*rs]
        # 把右侧 `{"index":i,"record_sha256":sha256(rec)}` 计算得到的值保存到 `row`，后面的判断或输出会继续使用它。
        row={"index":i,"record_sha256":sha256(rec)}
        # 开始循环 `f in sp.get('字段',[])`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for f in sp.get('字段',[]):
            # 数组型字段会产生太大CSV，仅JSON允许；这里仍导出。
            # 创建变量 `try`（类型提示为 `row[f['name']]`），并把 `read_field(rec,f)` 的结果保存进去供后续步骤使用。
            try: row[f['name']]=read_field(rec,f)
            # 捕获 `Exception as e: row[f['name']]=f"<解析失败` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
            except Exception as e: row[f['name']]=f"<解析失败:{e}>"
        # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
        rows.append(row)
    # 把 `rows` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return rows

# 定义函数 `find_case_insensitive`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def find_case_insensitive(root: Path, filename: str) -> Path|None:
    # 把右侧 `filename.lower()` 计算得到的值保存到 `want`，后面的判断或输出会继续使用它。
    want=filename.lower()
    # 开始循环 `p in root.iterdir()`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for p in root.iterdir():
        # 检查条件 `p.is_file() and p.name.lower()==want`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if p.is_file() and p.name.lower()==want:return p
    # 把 `None` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return None

# 定义函数 `validate_samples`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def validate_samples(orig_dir: Path, decoded_dir: Path) -> dict[str,Any]:
    # 把右侧 `[]; ok=0` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
    rows=[]; ok=0
    # 开始循环 `key,sp in TABLE_SPECS.items()`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for key,sp in TABLE_SPECS.items():
        # 把右侧 `find_case_insensitive(orig_dir,sp['文件'])` 计算得到的值保存到 `op`，后面的判断或输出会继续使用它。
        op=find_case_insensitive(orig_dir,sp['文件'])
        # 把右侧 `find_case_insensitive(decoded_dir,Path(sp['文件']).stem+'.bin')` 计算得到的值保存到 `dp`，后面的判断或输出会继续使用它。
        dp=find_case_insensitive(decoded_dir,Path(sp['文件']).stem+'.bin')
        # 把右侧 `{"表":key,"原文件":str(op) if op else None,"参考解码":str(dp) if dp else None}` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
        r={"表":key,"原文件":str(op) if op else None,"参考解码":str(dp) if dp else None}
        # 检查条件 `not op or not dp`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if not op or not dp:
            # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
            r.update({"通过":False,"错误":"缺少原文件或参考解码文件"});rows.append(r);continue
        # 把右侧 `xor_crypt(op.read_bytes()); ref=dp.read_bytes()` 计算得到的值保存到 `dec`，后面的判断或输出会继续使用它。
        dec=xor_crypt(op.read_bytes()); ref=dp.read_bytes()
        # 调用 `r.update` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
        r.update({"原大小":op.stat().st_size,"解码大小":len(dec),"解码SHA256":sha256(dec),"参考SHA256":sha256(ref),"字节完全一致":dec==ref})
        # 把右侧 `inspect(op)` 计算得到的值保存到 `geom`，后面的判断或输出会继续使用它。
        geom=inspect(op)
        # 执行一次赋值：把 `{k:v for k,v in geom.items() if k in ("类型","记录大小","预期记录数","实际记录数","整除","几何…` 的结果放入左侧 `r["结构检查"]` 对应的变量/字段。
        r["结构检查"]={k:v for k,v in geom.items() if k in ("类型","记录大小","预期记录数","实际记录数","整除","几何符合","预期大小","大小符合")}
        # 执行这一条实际代码步骤：`r['通过']=bool(dec==ref and (geom.get('几何符合',geom.get('大小符合',False))))`；它与上下相邻语句共同完成当前函数的小任务。
        r['通过']=bool(dec==ref and (geom.get('几何符合',geom.get('大小符合',False))))
        # 执行一次赋值：把 `int(r['通过']); rows.append(r)` 的结果放入左侧 `ok+` 对应的变量/字段。
        ok+=int(r['通过']); rows.append(r)
    # 把 `{"工具版本":TOOL_VERSION,"原目录":str(orig_dir),"参考解码目录":str(decoded_dir),"总数":len(rows),"通过":ok…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"工具版本":TOOL_VERSION,"原目录":str(orig_dir),"参考解码目录":str(decoded_dir),"总数":len(rows),"通过":ok,"失败":len(rows)-ok,"表":rows}

# 定义函数 `write_json`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def write_json(path: Path,obj:Any): path.parent.mkdir(parents=True,exist_ok=True);path.write_text(json.dumps(obj,ensure_ascii=False,indent=2),encoding='utf-8')
# 定义函数 `write_csv`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def write_csv(path: Path,rows:list[dict[str,Any]]):
    # 执行一次赋值：把 `True,exist_ok=True)` 的结果放入左侧 `path.parent.mkdir(parents` 对应的变量/字段。
    path.parent.mkdir(parents=True,exist_ok=True)
    # 把右侧 `[]` 计算得到的值保存到 `keys`，后面的判断或输出会继续使用它。
    keys=[]
    # 开始循环 `r in rows`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for r in rows:
        # 开始循环 `k in r`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for k in r:
            # 检查条件 `k not in keys`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if k not in keys:keys.append(k)
    # 进入受管理的资源作用域 `path.open('w',encoding='utf-8-sig',newline='') as f`；离开缩进块时 Python 会自动执行关闭/清理动作，避免文件句柄泄漏。
    with path.open('w',encoding='utf-8-sig',newline='') as f:
        # 把右侧 `csv.DictWriter(f,fieldnames=keys);w.writeheader()` 计算得到的值保存到 `w`，后面的判断或输出会继续使用它。
        w=csv.DictWriter(f,fieldnames=keys);w.writeheader()
        # 开始循环 `r in rows`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
        for r in rows:
            # 把右侧 `{k:(json.dumps(v,ensure_ascii=False) if isinstance(v,(list,dict)) else v) …` 计算得到的值保存到 `rr`，后面的判断或输出会继续使用它。
            rr={k:(json.dumps(v,ensure_ascii=False) if isinstance(v,(list,dict)) else v) for k,v in r.items()};w.writerow(rr)

# 定义函数 `spec_export`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def spec_export()->dict[str,Any]:
    # 把 `{"工具版本":TOOL_VERSION,"ENC算法":{"函数":"RPG.exe 0x004284D0","公式":"decoded[i] = encoded[i] XOR…` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return {"工具版本":TOOL_VERSION,"ENC算法":{"函数":"RPG.exe 0x004284D0","公式":"decoded[i] = encoded[i] XOR (i mod 255)","key_range":"0..254","period":255,"对称":True,"证据":"13/13现有ENC全文件与参考解码字节完全一致"},"表":TABLE_SPECS}

# 定义函数 `main`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def main(argv=None)->int:
    # 把右侧 `argparse.ArgumentParser(description='《幽城幻剑录》ENC调查器 v0.7A（默认只读，输出必须显式指定）')` 计算得到的值保存到 `ap`，后面的判断或输出会继续使用它。
    ap=argparse.ArgumentParser(description='《幽城幻剑录》ENC调查器 v0.7A（默认只读，输出必须显式指定）')
    # 把右侧 `ap.add_subparsers(dest='cmd',required=True)` 计算得到的值保存到 `sp`，后面的判断或输出会继续使用它。
    sp=ap.add_subparsers(dest='cmd',required=True)
    # 开始循环 `cmd,helptext in [('解码','解码ENC到新文件'),('编码','用同一XOR算法编码为新文件')]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
    for cmd,helptext in [('解码','解码ENC到新文件'),('编码','用同一XOR算法编码为新文件')]:
        # 把右侧 `sp.add_parser(cmd,help=helptext);p.add_argument('input',type=Path);p.add_a…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
        p=sp.add_parser(cmd,help=helptext);p.add_argument('input',type=Path);p.add_argument('output',type=Path)
    # 把右侧 `sp.add_parser('校验样本',help='13张现有ENC与参考解码做字节级校验');p.add_argument('original_…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser('校验样本',help='13张现有ENC与参考解码做字节级校验');p.add_argument('original_dir',type=Path);p.add_argument('decoded_dir',type=Path);p.add_argument('--json',type=Path)
    # 把右侧 `sp.add_parser('规格',help='输出当前ENC算法和表结构规格');p.add_argument('--json',type=Pa…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser('规格',help='输出当前ENC算法和表结构规格');p.add_argument('--json',type=Path)
    # 把右侧 `sp.add_parser('检查',help='检查一个已知ENC/bin的几何');p.add_argument('file',type=Pat…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser('检查',help='检查一个已知ENC/bin的几何');p.add_argument('file',type=Path);p.add_argument('--已解码',action='store_true');p.add_argument('--json',type=Path)
    # 把右侧 `sp.add_parser('导出记录',help='按已知固定记录结构导出字段');p.add_argument('file',type=Path…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
    p=sp.add_parser('导出记录',help='按已知固定记录结构导出字段');p.add_argument('file',type=Path);p.add_argument('--已解码',action='store_true');p.add_argument('--json',type=Path);p.add_argument('--csv',type=Path)
    # 把右侧 `ap.parse_args(argv)` 计算得到的值保存到 `ns`，后面的判断或输出会继续使用它。
    ns=ap.parse_args(argv)
    # 开始一个可能失败的操作区；若里面抛出异常，会转到后面的 `except` 分支处理。
    try:
        # 检查条件 `ns.cmd in ('解码','编码')`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ns.cmd in ('解码','编码'):
            # 把右侧 `xor_crypt(ns.input.read_bytes());ns.output.parent.mkdir(parents=True,exist…` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
            out=xor_crypt(ns.input.read_bytes());ns.output.parent.mkdir(parents=True,exist_ok=True);ns.output.write_bytes(out)
            # 执行一次赋值：把 `{sha256(out)}");return 0` 的结果放入左侧 `print(f"完成：{ns.input} -> {ns.output}，{len(out)} 字节，SHA…` 对应的变量/字段。
            print(f"完成：{ns.input} -> {ns.output}，{len(out)} 字节，SHA256={sha256(out)}");return 0
        # 检查条件 `ns.cmd=='校验样本'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ns.cmd=='校验样本':
            # 把右侧 `validate_samples(ns.original_dir,ns.decoded_dir)` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=validate_samples(ns.original_dir,ns.decoded_dir)
            # 检查条件 `ns.json`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ns.json:write_json(ns.json,r)
            # 把当前结果打印到控制台，方便人工立即查看；这一步只输出信息，不修改输入文件。
            print(json.dumps(r,ensure_ascii=False,indent=2));return 0 if r['失败']==0 else 2
        # 检查条件 `ns.cmd=='规格'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ns.cmd=='规格':
            # 把右侧 `spec_export();` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=spec_export();
            # 检查条件 `ns.json`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ns.json:write_json(ns.json,r)
            # 执行一次赋值：把 `False,indent=2));return 0` 的结果放入左侧 `print(json.dumps(r,ensure_ascii` 对应的变量/字段。
            print(json.dumps(r,ensure_ascii=False,indent=2));return 0
        # 检查条件 `ns.cmd=='检查'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ns.cmd=='检查':
            # 把右侧 `inspect(ns.file,ns.已解码)` 计算得到的值保存到 `r`，后面的判断或输出会继续使用它。
            r=inspect(ns.file,ns.已解码)
            # 检查条件 `ns.json`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ns.json:write_json(ns.json,r)
            # 执行一次赋值：把 `False,indent=2));return 0` 的结果放入左侧 `print(json.dumps(r,ensure_ascii` 对应的变量/字段。
            print(json.dumps(r,ensure_ascii=False,indent=2));return 0
        # 检查条件 `ns.cmd=='导出记录'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
        if ns.cmd=='导出记录':
            # 把右侧 `export_records(ns.file,ns.已解码);r={"工具版本":TOOL_VERSION,"文件":str(ns.file),"记…` 计算得到的值保存到 `rows`，后面的判断或输出会继续使用它。
            rows=export_records(ns.file,ns.已解码);r={"工具版本":TOOL_VERSION,"文件":str(ns.file),"记录数":len(rows),"记录":rows}
            # 检查条件 `ns.json`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ns.json:write_json(ns.json,r)
            # 检查条件 `ns.csv`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
            if ns.csv:write_csv(ns.csv,rows)
            # 执行一次赋值：把 `False,indent=2));return 0` 的结果放入左侧 `print(json.dumps(r,ensure_ascii` 对应的变量/字段。
            print(json.dumps(r,ensure_ascii=False,indent=2));return 0
    # 捕获 `Exception as e` 对应的异常，避免程序在这里直接中断，并把失败信息转成可读结果。
    except Exception as e:
        # 执行一次赋值：把 `sys.stderr);return 1` 的结果放入左侧 `print(f"错误：{e}",file` 对应的变量/字段。
        print(f"错误：{e}",file=sys.stderr);return 1
    # 把 `0` 作为本函数结果交回调用者；执行到这里后本次函数调用结束。
    return 0
# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__=='__main__': raise SystemExit(main())
