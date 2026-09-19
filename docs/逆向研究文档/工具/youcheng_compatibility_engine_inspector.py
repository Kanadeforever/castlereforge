#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""幽城兼容引擎综合调查器 0.7D-固化23
只读工具；Python 标准库。
提供 ENC 解码/几何检查、SF2 Header、SCI 分类、TSF 顶层、EVE Factory 基线。
"""
# 导入 `argparse, json, struct, hashlib` 模块；这里只取得标准库功能，不会修改游戏文件。
import argparse, json, struct, hashlib
# 从 `pathlib` 模块导入 `Path`，后面的代码会直接使用这些现成组件。
from pathlib import Path

# 把右侧 `'0.7D-固化23'` 计算得到的值保存到 `VER`，后面的判断或输出会继续使用它。
VER='0.7D-固化23'
# 把右侧 `'00 01 02 03 04 05 0B 0C 0D 11 12 13 14 15 16 19 1A 1B 1C 1D 1E 1F 33 34 3…` 计算得到的值保存到 `LEGAL`，后面的判断或输出会继续使用它。
LEGAL='00 01 02 03 04 05 0B 0C 0D 11 12 13 14 15 16 19 1A 1B 1C 1D 1E 1F 33 34 35 36 37 39 3A 3B 3C 3D 3E 47 48 49 4A 4B 4C 4D 51 52 53 54 55 56 57 58 59 5A 5B 5C 65 66 67 68 69 6A 6B 79 7A 7C 7D 7E 7F 80 81 82 83 85 86 87 88 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A'.split()
# 把右侧 `{` 计算得到的值保存到 `ENC`，后面的判断或输出会继续使用它。
ENC={
# 给当前结果字典的“Ail2.ENC”字段填写 `(648200,926,700),'Api.enc':(216240,848,255),'Battlegb.enc':(6204,94,6…`，这样导出的 JSON/表格能保留这一项证据。
'Ail2.ENC':(648200,926,700),'Api.enc':(216240,848,255),'Battlegb.enc':(6204,94,66),'Enemy_ai.enc':(15848,56,283),'Firttech.enc':(280000,560,500),'Layoutgr.enc':(127000,508,250),'LayoutTeam.ENC':(169672,508,334),'Levelup.enc':(404,4,101),'Magiccon.enc':(32000,64,500),'Magictb.enc':(3672,36,102),'Miscinfo.enc':(1054,None,None),'Refinek.enc':(1156,4,289),'Refinet.enc':(504,12,42)}

# 定义函数 `decode`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def decode(b): return bytes(x ^ (i%255) for i,x in enumerate(b))
# 定义函数 `dump`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def dump(x,p=None):
 # 把右侧 `json.dumps(x,ensure_ascii=False,indent=2)` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
 s=json.dumps(x,ensure_ascii=False,indent=2)
 # 检查条件 `p`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if p: Path(p).write_text(s+'\n',encoding='utf-8')
 # 前面的条件分支都没有命中时，执行这个兜底分支。
 else: print(s)

# 定义函数 `enc_file`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def enc_file(a):
 # 把右侧 `Path(a.file); b=p.read_bytes(); d=decode(b)` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
 p=Path(a.file); b=p.read_bytes(); d=decode(b)
 # 把右侧 `{'工具版本':VER,'文件':str(p),'大小':len(b),'SHA256':hashlib.sha256(b).hexdigest()…` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
 out={'工具版本':VER,'文件':str(p),'大小':len(b),'SHA256':hashlib.sha256(b).hexdigest(),'解码SHA256':hashlib.sha256(d).hexdigest()}
 # 检查条件 `a.decoded`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if a.decoded: Path(a.decoded).write_bytes(d); out['解码输出']=a.decoded
 # 调用 `dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
 dump(out,a.json)
# 定义函数 `enc_dir`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def enc_dir(a):
 # 把右侧 `Path(a.dir); rows=[]` 计算得到的值保存到 `root`，后面的判断或输出会继续使用它。
 root=Path(a.dir); rows=[]
 # 开始循环 `name,(sz,rs,cnt) in ENC.items()`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
 for name,(sz,rs,cnt) in ENC.items():
  # case-insensitive find
  # 把右侧 `[p for p in root.iterdir() if p.is_file() and p.name.lower()==name.lower()]` 计算得到的值保存到 `ps`，后面的判断或输出会继续使用它。
  ps=[p for p in root.iterdir() if p.is_file() and p.name.lower()==name.lower()]
  # 检查条件 `not ps: rows.append({'文件':name,'状态'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
  if not ps: rows.append({'文件':name,'状态':'MISSING'}); continue
  # 把右侧 `ps[0]; n=p.stat().st_size` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
  p=ps[0]; n=p.stat().st_size
  # 把右侧 `n==sz and (rs is None or n//rs==cnt and n%rs==0)` 计算得到的值保存到 `ok`，后面的判断或输出会继续使用它。
  ok=n==sz and (rs is None or n//rs==cnt and n%rs==0)
  # 把这一条新结果追加到列表 `rows`；不会覆盖前面已经收集的记录。
  rows.append({'文件':p.name,'大小':n,'预期大小':sz,'record_size':rs,'count':cnt,'状态':'PASS' if ok else 'FAIL'})
 # 调用 `dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
 dump({'工具版本':VER,'目录':str(root),'结果':rows,'通过':sum(r['状态']=='PASS' for r in rows)},a.json)
# 定义函数 `sf2`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sf2(a):
 # 把右侧 `Path(a.file).read_bytes();` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
 b=Path(a.file).read_bytes();
 # 检查条件 `len(b)<0x3c or b[:3]!=b'SF2'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if len(b)<0x3c or b[:3]!=b'SF2': raise SystemExit('不是可识别SF2')
 # 把右侧 `[]` 计算得到的值保存到 `desc`，后面的判断或输出会继续使用它。
 desc=[]
 # 开始循环 `off in [0x14,0x1e,0x28,0x32]`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
 for off in [0x14,0x1e,0x28,0x32]:
  # 把右侧 `struct.unpack_from('<H',b,off)[0]; pos=struct.unpack_from('<I',b,off+2)[0]…` 计算得到的值保存到 `cnt`，后面的判断或输出会继续使用它。
  cnt=struct.unpack_from('<H',b,off)[0]; pos=struct.unpack_from('<I',b,off+2)[0]; size=struct.unpack_from('<I',b,off+6)[0]
  # 把这一条新结果追加到列表 `desc`；不会覆盖前面已经收集的记录。
  desc.append({'count':cnt,'offset':pos,'size':size})
 # 调用 `dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
 dump({'文件':a.file,'tile_width':struct.unpack_from('<H',b,7)[0],'tile_height':struct.unpack_from('<H',b,9)[0],'compression':b[0xb],'canvas':[struct.unpack_from('<I',b,0xc)[0],struct.unpack_from('<I',b,0x10)[0]],'sections':desc},a.json)
# 定义函数 `sci`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def sci(a):
 # 把右侧 `Path(a.file).stat().st_size` 计算得到的值保存到 `n`，后面的判断或输出会继续使用它。
 n=Path(a.file).stat().st_size
 # 把右侧 `[]` 计算得到的值保存到 `kinds`，后面的判断或输出会继续使用它。
 kinds=[]
 # 检查条件 `n%1139==0:kinds.append({'类型':'Map SCI','记录数':n//1139,'stride'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if n%1139==0:kinds.append({'类型':'Map SCI','记录数':n//1139,'stride':1139})
 # 检查条件 `n%551==0:kinds.append({'类型':'Entity SCI','记录数':n//551,'stride'`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if n%551==0:kinds.append({'类型':'Entity SCI','记录数':n//551,'stride':551})
 # 检查条件 `Path(a.file).name.lower()=='dddes.sci' and n>=0x12 and (n-0x12)%0x34==0:kinds.append({'类型…`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if Path(a.file).name.lower()=='dddes.sci' and n>=0x12 and (n-0x12)%0x34==0:kinds.append({'类型':'DDDES显示定时配置','记录数':(n-0x12)//0x34,'header':0x12,'stride':0x34})
 # 调用 `dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
 dump({'文件':a.file,'大小':n,'候选分类':kinds or ['UNKNOWN']},a.json)
# 定义函数 `tsf`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def tsf(a):
 # 把右侧 `bytearray(Path(a.file).read_bytes()); core=0xe0b1` 计算得到的值保存到 `b`，后面的判断或输出会继续使用它。
 b=bytearray(Path(a.file).read_bytes()); core=0xe0b1
 # 检查条件 `len(b)<core`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if len(b)<core:raise SystemExit('TSF小于固定Core')
 # 把右侧 `struct.unpack_from('<I',b,0xdd99)[0]==0xff` 计算得到的值保存到 `encoded`，后面的判断或输出会继续使用它。
 encoded=struct.unpack_from('<I',b,0xdd99)[0]==0xff
 # 检查条件 `encoded`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if encoded:
  # 开始循环 `i in range(core)`；每轮取一个元素放进循环变量，再执行下面的缩进代码。
  for i in range(core): b[i]^=i%255
 # 把右侧 `struct.unpack_from('<I',b,0xe0a9)[0]; world=struct.unpack_from('<I',b,0xe0…` 计算得到的值保存到 `shop`，后面的判断或输出会继续使用它。
 shop=struct.unpack_from('<I',b,0xe0a9)[0]; world=struct.unpack_from('<I',b,0xe0ad)[0]
 # 把右侧 `{'文件':a.file,'大小':len(b),'固定Core':core,'legacy_xor':encoded,'World长度':worl…` 计算得到的值保存到 `out`，后面的判断或输出会继续使用它。
 out={'文件':a.file,'大小':len(b),'固定Core':core,'legacy_xor':encoded,'World长度':world,'Shop长度':shop,'容器长度匹配':core+world+shop==len(b),'active_inventory_bank':struct.unpack_from('<I',b,0xbbfc)[0],'party_count':struct.unpack_from('<I',b,0xdd70)[0],'MP3音量':struct.unpack_from('<I',b,0xdd91)[0],'WAV音量':struct.unpack_from('<I',b,0xdd95)[0]}
 # 检查条件 `world>=10200 and len(b)>=core+world`；条件成立才执行下一层缩进代码，不成立就跳过这一分支。
 if world>=10200 and len(b)>=core+world:
  # 把右侧 `b[core:core+world]; out['受控角色']={'x':struct.unpack_from('<i',w,0x32)[0],'y…` 计算得到的值保存到 `w`，后面的判断或输出会继续使用它。
  w=b[core:core+world]; out['受控角色']={'x':struct.unpack_from('<i',w,0x32)[0],'y':struct.unpack_from('<i',w,0x36)[0],'state':struct.unpack_from('<I',w,0x3a)[0],'index':struct.unpack_from('<I',w,0x3e)[0],'GameVar数量':5000}
 # 调用 `dump` 执行这一小步；传入的具体参数写在括号里，执行结果按该函数定义处理。
 dump(out,a.json)
# 定义函数 `eve`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def eve(a): dump({'合法Opcode数':88,'非法Opcode数':67,'不同Execute':87,'合法Opcode':['0x'+x for x in LEGAL],'共享Execute':{'0x0040CB70':['0x05','0x33']}},a.json)

# 定义函数 `main`。调用它时会从这里进入，参数在括号中传入，函数体按下面缩进顺序执行。
def main():
 # 把右侧 `argparse.ArgumentParser(description='幽城兼容引擎综合调查器（只读）'); p.add_argument('--…` 计算得到的值保存到 `p`，后面的判断或输出会继续使用它。
 p=argparse.ArgumentParser(description='幽城兼容引擎综合调查器（只读）'); p.add_argument('--version',action='version',version=VER); sp=p.add_subparsers(dest='cmd',required=True)
 # 把右侧 `sp.add_parser('enc解码'); s.add_argument('file'); s.add_argument('--decoded'…` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
 s=sp.add_parser('enc解码'); s.add_argument('file'); s.add_argument('--decoded'); s.add_argument('--json'); s.set_defaults(f=enc_file)
 # 把右侧 `sp.add_parser('enc目录'); s.add_argument('dir'); s.add_argument('--json'); s…` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
 s=sp.add_parser('enc目录'); s.add_argument('dir'); s.add_argument('--json'); s.set_defaults(f=enc_dir)
 # 把右侧 `sp.add_parser('sf2头'); s.add_argument('file'); s.add_argument('--json'); s…` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
 s=sp.add_parser('sf2头'); s.add_argument('file'); s.add_argument('--json'); s.set_defaults(f=sf2)
 # 把右侧 `sp.add_parser('sci分类'); s.add_argument('file'); s.add_argument('--json'); …` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
 s=sp.add_parser('sci分类'); s.add_argument('file'); s.add_argument('--json'); s.set_defaults(f=sci)
 # 把右侧 `sp.add_parser('tsf头'); s.add_argument('file'); s.add_argument('--json'); s…` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
 s=sp.add_parser('tsf头'); s.add_argument('file'); s.add_argument('--json'); s.set_defaults(f=tsf)
 # 把右侧 `sp.add_parser('eve基线'); s.add_argument('--json'); s.set_defaults(f=eve)` 计算得到的值保存到 `s`，后面的判断或输出会继续使用它。
 s=sp.add_parser('eve基线'); s.add_argument('--json'); s.set_defaults(f=eve)
 # 把右侧 `p.parse_args(); a.f(a)` 计算得到的值保存到 `a`，后面的判断或输出会继续使用它。
 a=p.parse_args(); a.f(a)
# 判断这个文件是不是被用户直接运行；只有直接运行时才进入命令行主流程，被其他脚本导入时不会自动执行。
if __name__=='__main__': main()
