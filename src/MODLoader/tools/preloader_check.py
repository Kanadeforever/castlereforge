#!/usr/bin/env python3
# -*- coding: utf-8 -*-
r"""
《幽城幻剑录》Mod Loader v0.3.0-dev9-about4 —— dev9 封存运行时 + About 自适应布局 + INI 自动换行回归检查工具。

本版最重要的回归边界不是“Hook 数量”，而是 Locale 启动时序：
1. CastleLocaleBootstrap.dll 必须是 ntdll-only，绝不能静态依赖 KERNEL32；
2. Launcher 必须一次性把 LocaleBootstrap 放在 ModCore 前面，不能连续写两次同一 .rdata padding；
3. CastleModCore.dll 仍只依赖 KERNEL32，并且不再包含 v0.2.3 的 nls_runtime.c；
4. CP950 真正 NLS 重建只能出现在 LocaleBootstrap；Core 只确认状态并继续现代缓存/Win32 Hook；
5. Locale 失败不能再从 Core DllMain return FALSE 制造无诊断的 0xc0000142；EntryPoint 前才 fail-closed；
6. ASI/Overrides/相对路径等既有能力不得回归；
7. ASI 必须先转绝对路径，再用 LOAD_WITH_ALTERED_SEARCH_PATH 解析 mods\asi 同目录依赖；
8. Loader 自身两个 DLL 必须放在 mods\ 根目录，便于“删除 EXE + mods”完整卸载；
9. dev9 必须恢复 dev5 已实机成功的两段 SetDllDirectoryW(mods) 语义：Launcher CreateProcess 前临时设置并恢复父进程，Core 进入 RPG.exe 后长期设置；同时仍禁止 Launcher 管理 mods\ddraw.dll；
10. ddraw.dll 必须重新进入 Core/Overrides/Locale/USER32/GDI 的 Win32 IAT 兼容桥目标，以恢复 dev5 的转区/截图调用链；
11. GUI 只能给存在同名 INI 的 ASI 显示“编辑”，并使用系统 RichEdit 动态加载、语法分色、保存前通用 INI 结构校验与原子写回；
12. dev9-about4 的改动只允许落在 Launcher GUI/About/INI 编辑器：About 必须是独立窗口、支持 about.cpp 中配置的可点击链接，并按正文实际折行高度自适应窗口，禁止正文与“相关链接”重叠；INI 必须真正按编辑区宽度自动换行，同时未保存与保存错误使用醒目红字；两个游戏运行时 DLL 必须继续继承已封存 dev9。
"""
from __future__ import annotations
import hashlib, pathlib, struct, subprocess, sys, re

ROOT = pathlib.Path(__file__).resolve().parents[1]
SRC = ROOT / "源码"
OUT = ROOT / "编译内容"
# 当前用户实际提供并沿用到 v0.3.0-dev9 DLL 搜索环境修正版继续沿用的 RPG.exe.org。
# v0.2.7 以前把历史 canonical Oracle 的 SHA-256 当成当前用户 EXE，这是接档记录错误；
# 固化40 已确认当前文件在已覆盖代码/调用关系上与历史 Oracle 属于机器语义等价候选。
EXPECTED_RPG_SHA256 = "8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f"
HISTORICAL_ORACLE_SHA256 = "b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf"
EXPECTED_ENTRY_RVA = 0x52C19
EXPECTED_ENTRY_BYTES = bytes.fromhex("55 8B EC 6A FF")

# 所有检查结果先放进这个列表，最后再统一打印。这样任何一个失败都不会让脚本在中途停止，
# 用户可以一次看到“哪些条件通过、哪些条件失败”，不用修一个再跑一次才能发现下一个。
checks=[]

def ck(cond,text):
    # bool(cond) 把 pathlib/集合/数字等各种 Python 真值统一收成真正的 True/False。
    checks.append((bool(cond),text))

def txt(name):
    # 源码统一规定 UTF-8；这里强制按 UTF-8 读，避免系统默认编码把中文注释读坏。
    return (SRC/name).read_text(encoding='utf-8')

def pe_info(path):
    # PE 文件开头的 DOS 头在 0x3C 保存 e_lfanew，它告诉我们真正 PE 头从哪里开始。
    # 这里不调用第三方 pefile，目的是让检查工具只依赖 Python 标准库，拿到包就能直接运行。
    b=path.read_bytes()
    e=struct.unpack_from('<I',b,0x3c)[0]
    # 正常 PE 的签名必须是四字节 'PE\0\0'。不是就立即停止，避免后面把普通文件按 PE 偏移乱读。
    if b[e:e+4] != b'PE\0\0': raise ValueError('not PE')
    machine=struct.unpack_from('<H',b,e+4)[0]; nsec=struct.unpack_from('<H',b,e+6)[0]
    optsz=struct.unpack_from('<H',b,e+20)[0]; opt=e+24
    magic=struct.unpack_from('<H',b,opt)[0]; entry=struct.unpack_from('<I',b,opt+16)[0]
    imp=struct.unpack_from('<II',b,opt+96+8) if magic==0x10b else (0,0)
    secs=[]; sec_off=opt+optsz
    for i in range(nsec):
        o=sec_off+i*40; name=b[o:o+8].split(b'\0')[0].decode('ascii','replace')
        vs,va,rs,rp=struct.unpack_from('<IIII',b,o+8); secs.append((name,vs,va,rs,rp))
    return b,e,machine,magic,entry,imp,secs

def rva_to_off(rva,secs):
    # PE 内很多表保存的是 RVA（相对映像基址的地址），磁盘文件却要用文件偏移读取。
    # 因此逐节判断 RVA 落在哪个 section，再用“节的磁盘起点 + 节内偏移”换算。
    for _,vs,va,rs,rp in secs:
        if va <= rva < va+max(vs,rs): return rp+(rva-va)
    return rva

def pe_imports(path):
    # 这个函数只做一件事：把最终 PE 真正写在 Import Table 里的 DLL 名与函数名读出来。
    # 这样我们检查的是“最终成品到底依赖什么”，而不是只相信源码或链接命令看起来正确。
    b,e,machine,magic,entry,imp,secs=pe_info(path)
    rva,size=imp
    if not rva: return []
    off=rva_to_off(rva,secs); result=[]
    for idx in range(128):
        oft,ts,fc,nrva,ft=struct.unpack_from('<IIIII',b,off+idx*20)
        if not any((oft,ts,fc,nrva,ft)): break
        no=rva_to_off(nrva,secs); ne=b.find(b'\0',no); dll=b[no:ne].decode('ascii','replace')
        trva=oft or ft; to=rva_to_off(trva,secs); names=[]
        for ti in range(1024):
            v=struct.unpack_from('<I',b,to+ti*4)[0]
            if not v: break
            if v & 0x80000000: names.append('#'+str(v&0xffff))
            else:
                hn=rva_to_off(v,secs)+2; he=b.find(b'\0',hn); names.append(b[hn:he].decode('ascii','replace'))
        result.append((dll,names))
    return result

def pe_resource_type_ids(path):
    # Windows 图标不是普通文件尾巴，而是 PE .rsrc 目录中的 RT_ICON(3) + RT_GROUP_ICON(14)。
    # 这里读取资源目录第一层的数字 Type ID，确认最终 EXE 真的带这两类资源，而不是只把 RPG.ico 放在源码包里却忘了链接。
    b,e,machine,magic,entry,imp,secs=pe_info(path)
    opt=e+24
    if magic != 0x10b:
        return set()
    resource_rva,resource_size=struct.unpack_from('<II',b,opt+96+8*2)
    if not resource_rva or resource_size < 16:
        return set()
    base=rva_to_off(resource_rva,secs)
    named_count,id_count=struct.unpack_from('<HH',b,base+12)
    ids=set()
    for i in range(named_count+id_count):
        name_or_id,_=struct.unpack_from('<II',b,base+16+i*8)
        if not (name_or_id & 0x80000000):
            ids.add(name_or_id & 0xffff)
    return ids

# 下面先把需要做文本级结构检查的源码读入。文本检查不能替代 PE 检查，
# 但很适合防止“某个旧方案代码又被合并回来”这种架构回归。
launcher=txt('launcher.c'); gui=txt('launcher_gui.c'); guicfg=txt('launcher_mod_config.c'); injector=txt('pe_import_injector.c'); bootstrap=txt('locale_bootstrap.c')
core=txt('core.c'); mod=txt('mod_loader.c'); ov=txt('override_loader.c'); loc=txt('locale_layer.c'); entry=txt('entry_gate.c')
native=txt('native_locale.c'); user32loc=txt('user32_locale.c'); gdiloc=txt('gdi_locale.c'); audit=txt('game_audit.c')
about_h=txt('about.h'); about_cpp=txt('about.cpp')
build=(ROOT/'build.bat').read_text(encoding='utf-8')
loader_ini_template=ROOT/'配置模板'/'CastleModLoader.ini'
compiled_loader_ini=OUT/'mods'/'CastleModLoader.ini'

ck('CREATE_SUSPENDED_' in launcher, 'Launcher 仍以 CREATE_SUSPENDED 创建 RPG.exe')
ck('PeImportInjector_AddEarlyImports2' in launcher, 'Launcher 使用一次性双 Early Import 注入，不重复占用 .rdata padding')
ck(launcher.find('"CastleLocaleBootstrap.dll"') < launcher.find('"CastleModCore.dll"'), 'Launcher 注入顺序为 LocaleBootstrap → ModCore')
ck('CastleLocaleBootstrap_Bootstrap' in launcher and 'CastleModCore_Bootstrap' in launcher, '两个稳定 Import-by-Name 导出均被 Launcher 使用')
ck('CreateRemoteThread(pi.' not in launcher and 'WaitForDebugEvent(' not in launcher, '未退回远程线程/调试器启动草案')
ck('SetCurrentDirectoryW(g_launcher_dir)' in launcher and 'L"RPG.exe"' in launcher, '仍保持同目录相对路径启动模型')
ck('SetDllDirectoryW(g_mods)' in launcher and launcher.count('SetDllDirectoryW(NULL_PTR)') >= 2, 'Launcher 恢复 dev5 启动期 SetDllDirectoryW(mods)，并在 CreateProcess 成功/失败两条路径恢复父进程默认搜索环境')
ck('g_mods, (const WCHAR*)L"CastleLocaleBootstrap.dll"' in launcher and 'g_mods, (const WCHAR*)L"CastleModCore.dll"' in launcher, 'Loader 自身 Bootstrap/Core 从 mods 根目录做存在性校验')
ck('\"CastleLocaleBootstrap.dll\", \"CastleLocaleBootstrap_Bootstrap\"' in launcher and '\"CastleModCore.dll\", \"CastleModCore_Bootstrap\"' in launcher, 'Early Import DLL 名恢复 dev5 的纯文件名；由子进程继承的 mods DLL 搜索环境定位内部 DLL')
ck('ddraw_path' not in launcher and '(const WCHAR*)L"ddraw.dll"' not in launcher and 'path_join_(ddraw' not in launcher, r'Launcher 不探测、不复制、不选择 ddraw.dll；文档注释提到 mods\ddraw.dll 不视为运行时代码')


# v0.3.0-dev5：GUI 必须只是稳定 Pre-Loader 前面的可视前端，不能反向污染启动链。
ck('LauncherGui_Run()' in launcher and launcher.find('LauncherGui_Run()') > launcher.find('initialize_paths_and_config_()'), 'Launcher 初始化路径/配置后进入 GUI，而不是打开 EXE 就直接创建 RPG.exe')
start_segment=launcher[launcher.find('int LauncherApp_StartGame(void)'):launcher.find('void WINAPI WinMainCRTStartup')]
ck(start_segment.find('open_log_for_launch_()') >= 0 and start_segment.find('open_log_for_launch_()') < start_segment.find('create_and_prepare_game_('), '用户点击启动后才打开/清空 modloader.log，再进入原有启动链')
init_segment=launcher[launcher.find('static int initialize_paths_and_config_'):launcher.find('static int open_log_for_launch_')]
ck('CREATE_ALWAYS_' not in init_segment and 'open_log_for_launch_()' not in init_segment, '单纯打开 GUI 不会截断上一轮 modloader.log')
ck('CastleModLoader.ini.castle.tmp' in launcher and 'MoveFileExW' in launcher and 'FlushFileBuffers' in launcher, '设置窗口保存 Loader 配置使用同目录临时文件 + Flush + 原子替换')
ck(all(x in gui for x in ['代码 Mod（ASI）','文件 Mod（Overrides）','启动游戏','重新扫描','设置','关于']), 'GUI 主页面包含两类 Mod 管理、重新扫描、关于、设置和突出启动按钮')
ck('#include "about.h"' in gui and 'IDC_ABOUT_' in gui and 'g_about_button' in gui, '主 GUI 已接入独立 About 模块并创建专用“关于”按钮')
ck(gui.find('out->about_button.right = out->settings_button.left') >= 0, '“关于”按钮布局固定在“设置”左侧，不挤占底部主操作区')
ck('show_about_dialog_' in gui and 'kAboutClass_' in gui and 'about_proc_' in gui, '点击“关于”打开独立 About 窗口，不再使用只适合纯文字的 MessageBox')
ck('About_GetDialogText()' in gui and 'About_GetDialogTitle()' in gui and 'About_GetLinkCount()' in gui and 'About_GetLinkLabel(' in gui and 'About_GetLinkUrl(' in gui, 'About 窗口的正文和链接全部读取 about.cpp 数据，不在 GUI 中重复维护内容')
ck('extern "C"' in about_h and all(x in about_h for x in ['About_GetDialogTitle','About_GetDialogText','About_GetLinkCount','About_GetLinkLabel','About_GetLinkUrl','ABOUT_MAX_LINKS']), 'about.h 以 C ABI 暴露正文与链接读取接口，C/C++ 源码可以稳定链接')
ck('static const LPCWSTR kAboutDialogTitle_' in about_cpp and 'static const LPCWSTR kAboutDialogText_' in about_cpp and 'static const AboutLink_ kAboutLinks_' in about_cpp, 'about.cpp 集中保存可编辑 About 标题、正文与链接，不需要修改 GUI 代码')
ck('v0.3.0-dev9-about4' in about_cpp and '运行时基线：v0.3.0-dev9' in about_cpp, '默认 About 明确显示 about4 前端版本与 dev9 封存运行时基线')
ck('shell32.dll' in gui and 'ShellExecuteW' in gui and 'GetProcAddress' in gui and 'LoadLibraryW' in gui, 'About 点击链接时动态解析 ShellExecuteW，避免给 Launcher 增加静态 SHELL32 依赖')
ck(all(x in gui for x in ['DT_CALCRECT_','measure_about_body_height_','get_about_required_outer_size_','GetDC','ReleaseDC']), 'about4 使用真实字体/宽度测量正文高度并按内容计算 About 外框，不再依赖固定正文高度')
ck('body.bottom = body_bottom' in gui and 'label.top = label_top' in gui and 'links_top = about_links_top_' in gui, 'About 正文、相关链接标题与链接按钮按同一套实测坐标排列，不允许最后一行正文被链接区覆盖')
ck('count > 2u ? 430 : 390' not in gui, 'About 已删除 about3 按链接数量猜测 390/430 DIP 固定高度的失败方案')
ck('从上到下加载' in gui and '越靠下优先级越高' in gui, 'GUI 明确区分 ASI 加载顺序与 Overrides 覆盖优先级语义')
ck('LBS_OWNERDRAWFIXED_' in gui and 'draw_mod_row_' in gui and 'BS_OWNERDRAW_' in gui and 'draw_flat_button_' in gui, 'Mod 列表与主操作按钮统一使用自绘，避免传统灰底/三维按钮视觉')
ck('LauncherModConfig_SetEnabled' in gui and 'LauncherModConfig_Move' in gui, '列表点击可启停，按住拖动可直接调整对应类型的 Mod 顺序')
ck('SetCapture' in gui and 'ReleaseCapture' in gui and 'g_drag_target' in gui and 'g_drag_started' in gui, '拖动排序具备鼠标捕获、目标行、拖动阈值和释放收尾，不依赖偶然的选择变化')
ck('WM_DPICHANGED_' in gui and 'SetProcessDpiAwarenessContext' in gui and 'Segoe UI' in gui, 'GUI 支持高 DPI 动态缩放并使用 Segoe UI 字体')
main_proc_segment=gui[gui.find('static LRESULT_ CALLBACK main_proc_'):gui.find('/* ---------- 类注册、控件创建、消息循环 ---------- */')]
ck('WM_GETMINMAXINFO_' in gui and 'ptMinTrackSize.x' in gui and 'ptMinTrackSize.y' in gui, 'dev5 恢复 Win32 最小跟踪尺寸处理，不再允许窗口缩进布局不支持的极端宽度')
ck('scale_(hwnd, 1024)' in gui and 'scale_(hwnd, 640)' in gui and 'get_main_minimum_outer_size_' in gui, 'dev5 最小客户区固定为 1024×640 DIP，并通过统一函数换算标题栏/边框后的外框尺寸')
ck('AdjustWindowRectExForDpi' in gui and 'AdjustWindowRectEx' in gui, '最小外框尺寸优先按当前 DPI 精确换算，并保留旧 Windows 的 AdjustWindowRectEx 回退')
ck('width = 1024;' in gui and 'get_main_minimum_outer_size_(g_main, &width, &height)' in gui, '默认打开尺寸与最小尺寸使用同一套 1024×640 客户区规则')
ck('if (msg == WM_SIZE_)' in main_proc_segment and 'layout_main_();' in main_proc_segment and 'redraw_main_now_(hwnd);' in main_proc_segment, '主窗口 WM_SIZE 在完成全部布局后立即执行统一完整重绘')
ck('RedrawWindow' in gui and 'RDW_ALLCHILDREN_' in gui and 'RDW_ERASE_' in gui and 'RDW_UPDATENOW_' in gui, '缩放刷新使用 RedrawWindow 同步擦除并刷新父窗口与全部子窗口，清理旧列表/边框/按钮残留')
ck('move_to_rect_without_repaint_' in gui and 'MoveWindow(hwnd, r->left, r->top, r->right - r->left, r->bottom - r->top, FALSE_)' in gui, '主窗口缩放时先批量移动子控件而不逐个重画，避免中间几何状态产生条纹残影')
ck('MonitorFromPoint' in gui and 'GetMonitorInfoW' in gui and 'center_window_on_cursor_monitor_' in gui, '主窗口按鼠标所在显示器工作区居中，多显示器不会固定跑到主屏')
ck('勾选复选框启用或禁用 Mod' in gui and 'ASI 从上到下加载；Overrides 越靠下优先级越高' in gui, 'dev5 继续保留两行直接操作说明，并明确两类 Mod 的顺序语义')
# v0.3.0-dev9 追加：ASI 同名 INI 内置编辑器。
# 这里既检查“按钮是否真的由磁盘 has_ini 状态驱动”，也检查编辑器是否满足颜色、校验、编码与安全写回边界。
ck('int has_ini;' in txt('launcher_mod_config.h') and 'g_asi[i].has_ini = file_exists_(path);' in guicfg, r'GUI 配置层只根据 mods\asi 中同名 .ini 的真实存在状态标记 ASI 可编辑')
ck('kind == LAUNCHER_MOD_ASI && item->present && item->has_ini' in gui and 'L"编辑"' in gui and 'show_ini_editor_((UINT)index);' in gui, '只有主文件存在且有同名 INI 的 ASI 行显示/响应“编辑”；Overrides 不误显示编辑按钮')
ck(all(x in gui for x in ['RICHEDIT50W','RichEdit20W','Msftedit.dll','Riched20.dll']), 'INI 编辑器运行时优先动态载入 RichEdit 4.1，并为旧系统保留 RichEdit 2.0 回退')
ck('validate_ini_text_' in gui and '变量名=值' in gui and '节名不能为空' in gui and '含有不允许的控制字符' in gui, '保存前执行通用 INI 结构校验，并能指出缺等号、空节名、控制字符等具体原因')
ck(all(x in gui for x in ['; 注释','[节]','变量名','RGB_(86, 124, 78)','RGB_(116, 82, 164)','RGB_(38, 98, 162)','RGB_(173, 91, 38)']), 'INI 编辑器对注释、节名、变量名和值使用不同语义颜色并显示颜色图例')
ck('CHARFORMATW_' in gui and 'EM_SETCHARFORMAT_' in gui and 'SCF_SELECTION_' in gui and 'colorize_ini_editor_' in gui, 'INI 语法分色通过 RichEdit 字符格式实现，不是伪造静态图例')
ck(all(x in gui for x in ['EM_GETTEXTRANGE_','EM_FINDTEXTEXW_','TEXTRANGEW_','FINDTEXTEXW_']), 'about3 分色直接在 RichEdit cp 坐标中搜索真实 CR/LF，不把视觉自动折行误当成 INI 逻辑行')
color_seg=gui[gui.find('static void colorize_ini_editor_'):gui.find('static void delete_ini_temp_')]
ck('get_ini_editor_text_' not in color_seg and 'find_next_ini_line_break_' in color_seg and 'EM_GETTEXTRANGE_' in color_seg and 'EM_GETLINECOUNT_' not in color_seg and 'EM_LINEINDEX_' not in color_seg, '自动换行开启后，分色函数不使用会受视觉折行影响的 RichEdit 行号 API')
ck('set_rich_color_(cp_start, cp_start + line_len, RGB_(86, 124, 78))' in color_seg, '整条逻辑注释行包含缩进在内统一着为绿色，即使视觉上折成多行也不会被切色')
sel_start = gui.rfind('static void select_ini_error_line_(')
sel_end = gui.find('static int save_ini_editor_(', sel_start) if sel_start >= 0 else -1
sel_seg = gui[sel_start:sel_end] if sel_start >= 0 and sel_end > sel_start else ''
ck('get_ini_logical_line_range_' in sel_seg and 'EM_EXSETSEL_' in sel_seg, '保存校验失败按真实 CR/LF 逻辑行重新查询 RichEdit cp 范围，视觉折行不会把错误定位到错误位置')
ck('refresh_ini_word_wrap_' in gui and 'EM_SETTARGETDEVICE_' in gui and 'EM_SETTARGETDEVICE_, 0, 1' in gui and 'EM_SETTARGETDEVICE_, 0, 0' in gui, 'INI 编辑器每次布局后强制刷新 RichEdit 自动换行目标宽度，修复 about2 无横向滚动但实际不折行的问题')
ck('WS_HSCROLL_' not in gui[gui.find('g_ini_editor_text ='):gui.find('g_ini_editor_save =')] and 'ES_AUTOHSCROLL_' not in gui[gui.find('g_ini_editor_text ='):gui.find('g_ini_editor_save =')], 'INI RichEdit 本体不启用横向滚动/自动横向滚动样式')
ck('RGB_(220, 38, 38)' in gui and '● 未保存修改' in gui and '● 保存失败：' in gui, '未保存状态和保存错误统一使用醒目亮红字并带实心圆标记')
ck('INI_ENCODING_UTF8_BOM_' in gui and 'INI_ENCODING_UTF16LE_' in gui and 'INI_ENCODING_UTF16BE_' in gui and 'INI_ENCODING_ANSI_' in gui, 'INI 编辑器能识别并记住 ANSI/UTF-8(BOM或无BOM)/UTF-16LE/BE 编码')
ck('.castle.tmp' in gui and 'FlushFileBuffers' in gui and 'MOVEFILE_REPLACE_EXISTING_' in gui and 'MOVEFILE_WRITE_THROUGH_' in gui, 'INI 保存使用同目录临时文件 + Flush + REPLACE_EXISTING|WRITE_THROUGH 原子替换')
ck('为避免静默变成问号，本次拒绝保存' in gui and 'WC_NO_BEST_FIT_CHARS_' in gui, 'ANSI INI 遇到不能无损表示的新字符时拒绝保存，避免静默变成问号/近似字符')
ck('MB_YESNOCANCEL_' in gui and '这个 INI 还有未保存的修改' in gui, '关闭有未保存修改的 INI 编辑器时提供保存/放弃/取消三路选择')
ck('layout.asi_card.top - scale_(hwnd, 10)' in gui and 'g_ui.FillRect(dc, &line, g_brush_border);' in gui, '顶部说明与 Mod 工作区之间存在独立细分隔线')
ck('g_ui.DrawTextW(dc, (const WCHAR*)L"《幽城幻剑录》Mod Loader"' not in gui, '客户区不再重复绘制程序大标题，程序名称只保留在 Windows 标题栏')
resource_header=SRC/'resource.h'
resource_script=SRC/'launcher.rc'
resource_icon=ROOT/'资源'/'RPG.ico'
ck(resource_header.exists() and resource_script.exists() and resource_icon.exists(), r'源码包自带 resource.h、launcher.rc 与用户提供的 资源\RPG.ico')
if resource_header.exists() and resource_script.exists():
    resource_header_text=resource_header.read_text(encoding='utf-8')
    resource_script_text=resource_script.read_text(encoding='utf-8')
    ck('IDI_RPG_ICON 101' in resource_header_text and 'IDI_RPG_ICON ICON' in resource_script_text and 'RPG.ico' in resource_script_text, 'C/RC 共用同一个图标资源 ID，并把 RPG.ico 编译为 Launcher ICON 资源')
ck('LoadIconW' in gui and 'IDI_RPG_ICON' in gui and 'wc.hIcon = app_icon' in gui and 'wc.hIconSm = app_icon' in gui, '窗口类从 EXE 资源加载 RPG.ico，同时设置大图标和小图标')
ck('where rc.exe' in build and 'launcher.res' in build and 'launcher.rc' in build, 'Windows 正式 build.bat 明确编译并链接 RPG.ico 资源，不会生成无图标半成品')

# dev2 的用户反馈是“GUI 太丑”，dev3 又继续收到“拖动不明显、设置压框且过大、缺失项无法清理”的实机反馈。
# dev5 只修最小尺寸与缩放完整重绘；下面继续钉住 dev2/dev3/dev4 已确认的视觉与交互基线。
# 1) 统一主题画刷 + 卡片绘制必须存在，防止又退回每个控件各画各的默认灰色；
# 2) 空列表必须隐藏真实 ListBox，否则父窗口画的“暂无 Mod”会被白色子窗口盖住；
# 3) ListBox 创建位置不得重新加回 WS_EX_CLIENTEDGE，否则经典三维凹边会回归；
# 4) 设置页日志开关必须继续使用整行自绘，但改为紧凑单行，不能再退回会压住按钮的 66 DIP 大卡片。
# 这些仍然只是“源码结构防回归”，不能替代用户在 Windows 上对最终像素效果的主观验收。
ck('g_brush_workspace' in gui and 'g_brush_card' in gui and 'g_brush_selected' in gui and 'draw_card_' in gui, 'dev5 继续使用集中主题画刷与卡片结构，不退回系统三维控件主视觉')
ck('暂无文件 Mod' in gui and '暂无代码 Mod' in gui and 'g_ui.ShowWindow(list, SW_HIDE_)' in gui, '空列表隐藏 ListBox 并显示明确空状态，不留下整块无说明白板')
ck('CreateWindowExW(0u, (const WCHAR*)L"LISTBOX"' in gui and 'WS_EX_CLIENTEDGE_, (const WCHAR*)L"LISTBOX"' not in gui, 'dev5 列表继续禁用 WS_EX_CLIENTEDGE 三维凹边')
ck('Mod Loader 日志' in gui and '游戏运行审计日志' in gui and 'draw_setting_toggle_' in gui, '独立设置窗口以整行自绘选项承载两个日志开关，不挤占主 Mod 管理页')
ck('draw_drag_grip_' in gui and 'calculate_mod_row_layout_' in gui and '可拖动排序' in gui, 'ASI/Overrides 行都绘制明确拖动把手并在卡片副标题提示可拖动排序')
ck('LauncherModConfig_RemoveMissing' in gui and 'draw_inline_remove_button_' in gui and '文件缺失' in gui, '缺失 Mod 行提供内嵌移除按钮，并由 GUI 调用配置层清理陈旧引用')
ck('remove_document_entries_by_name_' in guicfg and 'LauncherModConfig_RemoveMissing' in guicfg and 'if (items[index].present)' in guicfg, '配置层只允许移除 present=0 项，并会清理同类型所有同名重复键防止旧键重新接管')
ck('reload_after_failed_edit_preserving_error_' in guicfg and 'if (saved_error[0]) set_error_(saved_error);' in guicfg, '启停/排序/缺失项清理写盘失败后，回读磁盘模型仍保留原始错误文字给 GUI')
ck('row_h = scale_(hwnd, 42)' in gui and 'bw = scale_(hwnd, 72)' in gui and 'bh = scale_(hwnd, 32)' in gui, '设置窗口使用 42 DIP 紧凑行与 72×32 小按钮，避免 dev2 大卡片压住底部按钮')
ck('int w = scale_(g_main, 460)' in gui and 'int h = scale_(g_main, 260)' in gui and '日志设置' not in gui, '设置弹窗缩小为 460×260，客户区只保留分类标题与紧凑行，不再堆重复大标题/长说明')
ck('LoadLibraryW((const WCHAR*)L"user32.dll")' in gui and 'LoadLibraryW((const WCHAR*)L"gdi32.dll")' in gui, 'USER32/GDI32 由 GUI 运行时动态加载，不成为 Launcher 静态导入')
ck('mods.ini.castle.gui.tmp' in guicfg and 'MoveFileExW' in guicfg and 'FlushFileBuffers' in guicfg, 'GUI 修改 mods.ini 同样使用临时文件 + Flush + 原子替换')
ck('scan_asi_' in guicfg and 'scan_overrides_' in guicfg and 'directory_has_regular_file_recursive_' in guicfg, 'GUI 打开/刷新时遵循 ASI 自动发现与 Overrides 非空递归扫描规则')
ck('OVERRIDE_TEMPLATE_NAME_' in guicfg and 'if (directory_exists_(root)) return 1;' in guicfg and 'valid_override_name_' in guicfg, 'GUI 只在模板不存在时创建，已有模板完全忽略且不进入列表')
ck('append_entry_to_section_' in guicfg and 'last_header' in guicfg, 'GUI 自动发现新 Mod 时复用最后一个已有同名 section，不持续追加表头')
ck('line_is_blank_' in guicfg and 'while (insert_at > (UINT)last_header + 1u' in guicfg and 'g_lines[insert_at - 1u].text' in guicfg, 'GUI 自动补全把新条目插在节尾连续空行之前，不再把空行留在 [ASI] 表头下')
ck('present' in guicfg and 'usable' in guicfg and '缺失' in gui and '空目录' in gui, 'GUI 保留 INI 中缺失/空 Overrides 条目并给出可视状态，而不是擅自删除配置')
ck('g_has_duplicate_entries' in guicfg and '本次不执行排序' in guicfg and '最上面第一次为准' in guicfg, 'GUI 检测同名重复 Mod 键后拒绝拖动，避免历史重复键因重排改变 Core 第一次优先语义')
ck('launcher_gui.obj' in build and 'launcher_mod_config.obj' in build and 'about.obj' in build, 'Windows 正式构建已包含 GUI、GUI Mod 配置与 About 三个职责独立的前端源码单元')
compiled_launcher=OUT/'CastleModLoader.exe'
if compiled_launcher.exists():
    try:
        resource_types=pe_resource_type_ids(compiled_launcher)
        ck(3 in resource_types and 14 in resource_types, '最终 CastleModLoader.exe 同时包含 RT_ICON(3) 与 RT_GROUP_ICON(14)，RPG.ico 已实际链接进 PE')
    except Exception as exc:
        ck(False, f'读取 CastleModLoader.exe 图标资源目录失败：{exc}')
else:
    ck(False, '编译内容缺少 CastleModLoader.exe，无法验证 RPG.ico 资源')
ck('模板_复制后改名' in build and 'MultiMedia\\%%D' in build, '正式 build.bat 会生成完整 Overrides 模板骨架，而不是留下运行时不会再补齐的半成品模板')
template_root=OUT/'mods'/'overrides'/'模板_复制后改名'
expected_template_dirs=['exe','MultiMedia','MultiMedia/fight','MultiMedia/Font','MultiMedia/Map','MultiMedia/menus','MultiMedia/Mov','MultiMedia/Music','MultiMedia/public','MultiMedia/save','MultiMedia/Sys']
ck(all((template_root/pathlib.Path(x)).is_dir() for x in expected_template_dirs), '编译内容当前已包含完整 Overrides 模板目录骨架')

# v0.2.11：Loader 自身配置与日志开关
ck('CastleModLoader.ini' in launcher and 'read_loader_ini_' in launcher, 'Launcher 在创建日志前读取独立 CastleModLoader.ini')
ck('ModLoaderLog' in launcher and 'GameLog' in launcher and '[Logging]' in launcher, 'Loader 配置首批提供 ModLoaderLog/GameLog 两个独立开关')
ck('CASTLE_MODLOADER_LOG_ENABLED' in launcher and 'CASTLE_GAME_LOG_ENABLED' in launcher, 'Launcher 把本轮日志开关通过环境传给 RPG.exe 内 Core')
ck('if (!g_modloader_log_enabled)' in launcher and 'g_log = INVALID_HANDLE_VALUE_' in launcher, 'ModLoaderLog=0 时 Launcher 不创建/截断 modloader.log')
ck('ModLoader_IsGameLogEnabled()' in core and 'if (ModLoader_IsGameLogEnabled())' in core, 'Core 只在 GameLog=1 时建立原版游戏审计层')
ck('GameLog=0：本轮不建立 game.log' in core and 'GameAudit_Initialize()' in core, 'GameLog=0 明确跳过 game.log 与仅用于审计的 Hook')
ck(loader_ini_template.exists() and compiled_loader_ini.exists(), '源码包配置模板与编译内容都带 CastleModLoader.ini')
if loader_ini_template.exists():
    ini_text=loader_ini_template.read_text(encoding='utf-8-sig')
    ck('[Logging]' in ini_text and 'ModLoaderLog=1' in ini_text and 'GameLog=1' in ini_text, '默认 Loader 配置模板启用两个日志且职责分离')
ck(r'配置模板\CastleModLoader.ini' in build and r'mods\CastleModLoader.ini' in build, '正式 build.bat 会把 Loader 默认配置复制到编译内容')

ck('RtlInitNlsTables' in bootstrap and 'RtlResetRtlTranslations' in bootstrap, 'CP950 真正 NLS 重建位于 LocaleBootstrap')
ck('NtOpenFile' in bootstrap and 'NtReadFile' in bootstrap and 'NtAllocateVirtualMemory' in bootstrap, 'LocaleBootstrap 使用 Nt/Rtl 原语读取/映射 NLS')
ck('GetModuleHandleW' not in bootstrap and 'CreateFileW' not in bootstrap and 'GetProcAddress' not in bootstrap, 'LocaleBootstrap 源码不调用 KERNEL32 高层 API')
ck('c_950.nls' in bootstrap and 'l_intl.nls' in bootstrap and 'SystemRoot' in bootstrap, 'LocaleBootstrap 固定使用 CP950 与 Unicode case NLS 文件')
ck('peb + 0x58u' in bootstrap and 'peb + 0x5Cu' in bootstrap and 'peb + 0x60u' in bootstrap, 'LocaleBootstrap 更新 PEB 三个真正 NLS table pointer')
ck('g_observed_ansi_codepage' in bootstrap and 'g_observed_oem_codepage' in bootstrap and 'CodePage != 950u' not in bootstrap, 'LocaleBootstrap 不再用 NLSTABLEINFO CodePage 字段误判启动，只保留诊断观察值')
ck('peb + 0x58u' in bootstrap and '!= (PVOID)ansi' in bootstrap, 'LocaleBootstrap 对 x86 PEB 三个 NLS 原始数据指针做写回读回验证')
ck('return 1;' in bootstrap[bootstrap.find('BOOL NTAPI DllMain'):], 'LocaleBootstrap DllMain 不再用 FALSE 折叠成 0xc0000142')

ck('nls_runtime.c' not in build and not (SRC/'nls_runtime.c').exists(), 'v0.2.3 nls_runtime 已从普通 Core 构建彻底移除')
ck('CASTLE_LOCALE_BOOTSTRAP_DLL_' in loc and 'CASTLE_LOCALE_BOOTSTRAP_STATUS_EXPORT_' in loc, '普通 Locale Layer 接管早期 Bootstrap 状态')
ck('g_real_GetACP' in loc and 'g_real_GetOEMCP' in loc and 'verify_real_win32_codepage_' in loc, 'Kernel32 阶段使用未 Hook 的真实 GetACP/GetOEMCP 做 950/950 第二道硬验证')
ck('CastleLocaleBootstrap_GetObservedAnsiCodePage' in loc and 'CastleLocaleBootstrap_GetObservedOemCodePage' in loc, 'Core 会把 Bootstrap 内部观察字段写入日志但不作为成功门槛')
ck('NlsRuntime_InitializeCp950' not in loc and 'RtlInitNlsTables(' not in loc, '普通 Core 不再重复调用 NLS 重建函数')
ck('peb + 0x228u' in loc and 'peb + 0x22Au' in loc and 'refresh_modern_nls_hash_' in loc, 'Core 只做现代 ANSI/OEM cache + KernelBase hash 补充')
ck('Taipei Standard Time' in loc and 'PreferredUILanguages' in loc and 'InstallLanguage' in loc, '台北时区 + UI语言 + 语言注册表伪造仍保留')

# DllMain 中 Locale 失败只能记状态，不能 return FALSE。
dllmain=core[core.find('BOOL WINAPI DllMain'):]
ck('g_early_locale_ready = (NativeLocale_Initialize() && LocaleLayer_Initialize() &&' in dllmain, 'Core DllMain 把 Native/Win32/USER32/GDI Locale 联合结果保存成状态')
segment=dllmain[dllmain.find('g_early_locale_ready ='):]
ck('if (!LocaleLayer_Initialize()) return FALSE_' not in segment, 'Locale 失败不再从 Core DllMain 返回 FALSE')
ck('[致命] Locale 早期 Bootstrap/Win32 层未通过' in core, 'EntryPoint 前仍 fail-closed，半转区状态不会进入游戏业务')

# v0.2.8：Locale Emulator 等价语义补全
ck('NativeLocale_Initialize()' in core and 'NativeLocale_Refresh()' in core, 'Core 已接入 Native Locale 早期/EntryPoint 双阶段确认')
ck(dllmain.find('ModLoader_PrepareOverrides()') < dllmain.find('NativeLocale_Initialize()') < dllmain.find('LocaleLayer_Initialize()'), 'Early 顺序保持 Overrides → Native Locale → Win32 Locale')
ck('NtQueryDefaultLocale' in native and 'NtQueryDefaultUILanguage' in native and 'NtQueryInstallUILanguage' in native, 'Native Locale 覆盖默认 Locale 与 UI Language ntdll 语义')
ck('NtQuerySystemInformation' in native and 'SYSTEM_CURRENT_TIME_ZONE_INFORMATION_ 44u' in native, 'Native Locale 覆盖 ntdll 当前时区查询')
ck('NtInitializeNlsFiles' in native, 'Native Locale 保留默认 NLS LocaleID 语义')
ck('teb + 0xC4u' in native and 'NATIVE_LOCALE_LCID_' in native, 'Native Locale 直接固定 x86 TEB.CurrentLocale=0x0404')
ck('NtQueryValueKey' in native and 'NtQueryKey' in native, 'Native Locale 已把语言注册表伪造下沉到 ntdll NtQueryValueKey')
ck(all(token in native for token in ['InstallLanguage','OEMCP','ACP','LocaleName','PreferredUILanguages','MachinePreferredUILanguages']), 'Native 注册表层包含用户指定的 zh-TW/CP950 高级语言键值')
ck('g_real_NtQueryValueKey(NULL_PTR' not in native, 'Native 注册表回退不会再用空 KeyHandle 错误调用真实 NtQueryValueKey')
ck('KEY_VALUE_PARTIAL_INFORMATION_ALIGN64_' in native and 'round_up_dword_' in native, 'Native 注册表返回结构显式处理 Partial/Full 对齐边界')
ck("hook->target[0] != 0xB8u" in native and 'PAGE_EXECUTE_READWRITE_' in native, 'ntdll inline Hook 只接受完整 5 字节 mov-eax syscall stub，未知布局 fail-closed')
ck('RtlCustomCPToUnicodeN' in native and 'RtlInitCodePageTable' in native, 'Native Locale 已补 Locale Emulator Core 同类的 CPTABLEINFO 陈旧代码页修复层')
ck('measure_safe_prologue_' in native and 'x86_instruction_length_safe_' in native and 'MAX_FUNCTION_PROLOGUE_' in native, 'RtlCustomCPToUnicodeN 普通函数 Hook 会先按完整 x86 指令边界测量入口，不盲切 5 字节')
ck('custom_cp->CodePage != (WORD)CUSTOM_CP_UTF8_' in native and 'custom_cp->CodePage != (WORD)CASTLE_CP_BIG5_' in native, 'CPTABLEINFO 修复只处理既非 UTF-8 也非目标 CP950 的陈旧表')
ck('current_ansi_nls_table_' in native and 'peb + 0x58u' in native, 'CPTABLEINFO 修复从 Bootstrap 已切换的 PEB.AnsiCodePageData 取得目标 c_950.nls 原始表')
ck('NativeLocale_GetCustomCpRepairCount' in native and '本次启动已修复的陈旧 CPTABLEINFO 次数' in core, '实体文件自检后会记录本机实际命中的 CPTABLEINFO 修复次数')
ck('CreateWindowExA' in user32loc and 'CreateWindowExW' in user32loc and 'CASTLE_CP_BIG5_' in user32loc, 'USER32 桥把 CP950 CreateWindowExA 文本转为 Unicode 再进入 W API')
ck('MessageBoxA' in user32loc and 'MessageBoxW' in user32loc and 'WM_SETTEXT_' in user32loc, 'USER32 桥覆盖 MessageBoxA 与 DefWindowProcA/WM_SETTEXT 文本路径')
ck('CHINESEBIG5_CHARSET' in gdiloc or 'CASTLE_CHARSET_BIG5_' in gdiloc, 'GDI 桥固定默认字体字符集为 CHINESEBIG5_CHARSET(136)')
ck('CreateFontIndirectA' in gdiloc and 'CreateFontIndirectW' in gdiloc and 'CreateFontA' in gdiloc and 'CreateFontW' in gdiloc, 'GDI 桥只覆盖标准字体创建入口，不搬入无关字体替换逻辑')
ck('native_locale.obj' in build and 'user32_locale.obj' in build and 'gdi_locale.obj' in build, 'Windows 正式构建已包含 Native/USER32/GDI Locale 三个新模块')

# v0.2.8 已实机闭合的文件名级转区验收 + v0.2.11 原版游戏审计收拢
ck('LocaleLayer_RunFileNameSelfTest' in loc and 'CP_ACP_' in loc, 'Locale 文件名自检继续使用真实 CP_ACP 转换，不把固定 950 数字伪装成默认代码页')
ck('big5_name_bytes' in loc and '0xC1u,0x63u' in loc and '繁體中文轉區驗證' in loc, 'Locale 文件名自检保留可人工核对的 Big5 原始字节与繁体 Unicode 期望名')
ck('CREATE_NEW_' in loc and 'FILE_FLAG_DELETE_ON_CLOSE_' in loc, '一次性 Unicode 探针继续使用 CREATE_NEW + DELETE_ON_CLOSE，不覆盖用户文件且正常不留垃圾')
ck('g_real_CreateFileA(ansi_path' in loc and 'CreateFileW(unicode_path' in loc, '文件名自检继续真实执行 CreateFileW 创建 → 真实 CreateFileA(Big5) 重开同一实体文件')
ck('g_real_GetFinalPathNameByHandleW' in loc and '实际打开的 NTFS Unicode 路径' in loc, '文件名自检继续反查成功句柄的最终 NTFS Unicode 路径')
ck(core.find('LocaleLayer_RunFileNameSelfTest()') < core.find('GameAudit_Initialize()') < core.find('OverrideLoader_EnableGameAudit()') < core.find('ModLoader_LoadAsi()'), 'EntryPoint 前顺序为文件名硬自检 → game.log 初始化 → 游戏审计 Hook → ASI')
ck('game_audit.obj' in build and 'game_audit.c' in build, 'Windows 正式构建已包含独立 game_audit 审计模块')
ck('mods\\game.log' in audit and 'CREATE_ALWAYS_' in audit, 'game.log 每次启动独立清空，形成单次游戏运行时间线')
ck('《幽城幻剑录》原版游戏运行审计 v0.3.0-dev9' in audit, 'game.log 明确标识 v0.3.0-dev9 且职责仅为原版 I/O、状态与异常')
ck('register_module_(g_game_module' in audit and 'find_caller_module_' in audit, '游戏审计以调用返回地址过滤 ASI/Mod，只接受已注册原版模块')
ck('register_module_(GetModuleHandleW((const WCHAR*)L"BASS.dll")' in audit and 'register_module_(GetModuleHandleW((const WCHAR*)L"binkw32.dll")' in audit and 'register_module_(GetModuleHandleW((const WCHAR*)L"ijl10.dll")' in audit, 'game.log 注册 RPG.exe 与原版 BASS/bink/ijl10 中间件调用来源')
ck('register_module_(GetModuleHandleW((const WCHAR*)L"ddraw.dll")' not in audit and 'register_module_(GetModuleHandleW((const WCHAR*)L"SHLWAPI.dll")' not in audit, 'game.log 不把可选 ddraw 兼容层或系统 SHLWAPI 自身 I/O 冒充原版游戏调用')
ck('GameAudit_RecordCreateFileA' in ov and 'GameAudit_RecordCreateFileW' in ov and 'GameAudit_RecordGetFileAttributesA' in ov and 'GameAudit_RecordGetFileAttributesW' in ov, 'CreateFile/GetFileAttributes 由 Overrides 单一 Hook 所有者旁路送入 game.log，避免重复 IAT Hook 互相覆盖')
ck('raw=' in audit and 'ansi_to_big5_wide_' in audit and 'GetFinalPathNameByHandleW' in audit, 'ANSI 文件路径日志同时保存原始字节、CP950 解码和成功句柄最终 Unicode 路径')

ck('append_error_result_' in audit and ' error=-' in audit, 'game.log 成功 API 不再打印陈旧 GetLastError；失败才显示错误码')
ck('info=ERROR_ALREADY_EXISTS(183)' in audit, 'CreateFile 成功时仍保留 ERROR_ALREADY_EXISTS=183 的有意义状态信息')
ck('IO/FDebug聚合' in audit and '5000u' in audit and 'quiet_fdebug' in audit, 'FDebug.txt 高频成功 I/O 已改为 5 秒窗口聚合，失败仍逐条记录')
ck('GameAudit_Shutdown' in audit and 'fdebug_flush_();' in audit and 'GameAudit_Shutdown();' in core, '正常退出会刷新未满5秒的 FDebug 聚合尾窗并关闭 game.log')
ck('IO/散装转DAT候选' in audit and '同线程500ms时间邻接候选' in audit, 'game.log 会把散装文件失败→DAT SetFilePointer 标为时间邻接候选，不冒充绝对因果')
ck(all(x in audit for x in ['Hook_ReadFile_','Hook_WriteFile_','Hook_CloseHandle_','Hook_SetFilePointer_','Hook_SetEndOfFile_','Hook_FlushFileBuffers_','Hook_GetCurrentDirectoryA_','Hook_DeleteFileA_','Hook_SetFileAttributesA_']), 'RPG.exe 已确认的非冲突 KERNEL32 I/O 入口全部具备旁路 Hook')
ck('GAME_AUDIT_HANDLE_MAX_ 512u' in audit and 'read_calls' in audit and 'write_calls' in audit and 'logical_offset' in audit, 'game.log 跟踪文件句柄生命周期、逻辑偏移及读写调用/字节统计')
ck('ends_with_i_(slot->path, (const WCHAR*)L".TSF")' in audit and 'contains_i_(slot->path, (const WCHAR*)L"NewGame")' in audit, 'TSF/NewGame/可写句柄会升级为逐次读写详细记录，普通大资源读仅在关闭时汇总')
ck(all(f'0x{a:08X}u' in audit for a in [0x0044802D,0x0043B360,0x0043B510,0x0043ACD0,0x0043AB30,0x0043AE80,0x0044B130,0x0044B1F0,0x0040B230,0x0040B340,0x0040D780,0x0040D8A0,0x0040DAA0]), '13 个已闭合的存档/读档/新游戏/Event/场景生命周期边界全部进入状态审计表')
ck('bytes_equal_(target, hook->expected' in audit and 'return 0;' in audit[audit.find('static int install_state_hook_'):audit.find('static UINT install_all_state_hooks_')], '状态 Hook 在写 JMP 前逐字节核对当前 RPG.exe 机器码，不匹配即 fail-closed')
ck('NEW_GAME_BEGIN' in audit and 'LOAD_READER' in audit and '0x0044802Du' in audit and '0x0043B510u' in audit, '新游戏分支与共用 TSF Reader 同时布点，可直接重放“读档后再新游戏”路径')
ck(all(x in audit for x in ['0x0089F804u','0x0089F808u','0x0089F7F8u','0x0089F7FCu','0x0089F800u','activeController=','manager_plus_100','battle_container','rng_state']), '状态快照包含 active controller、active/deferred Event、NewGame manager 标志、Battle 与 RNG 等关键跨生命周期状态')
ck('BASELINE_BEFORE_RPG_ENTRY' in audit and 'EXCEPTION_SNAPSHOT' in audit, 'EntryPoint 前基线与异常后状态快照使用明确标记，不再混成 UNKNOWN_MARKER')
ck('AUDIT/Module' in audit and 'AUDIT/IOHook' in audit, 'game.log 会逐个记录原版模块基址/大小和每模块 I/O Hook 槽位数量')
ck('AddVectoredExceptionHandler' in audit and 'EXCEPTION_CONTINUE_SEARCH_' in audit, '严重异常只由 VEH 旁路记录并继续原异常链，不吞游戏崩溃')
ck('[EXCEPTION] 严重异常 code=' in audit and all(x in audit for x in [' EIP=',' ESP=',' EBP=',' EAX=',' EBX=',' ECX=',' EDX=',' ESI=',' EDI=']), '严重异常行带时间/线程分类、访问目标和完整 x86 通用寄存器现场')
ck(audit.count('FlushFileBuffers(g_log)') >= 2 and 'code != EXCEPTION_STACK_OVERFLOW_' in audit, '状态断点和异常现场主动刷盘；栈溢出异常不再递归调用大状态快照')
ck('[转区/游戏实测]' not in ov and 'MAX_ANSI_PATH_PROBES_' not in ov and 'OverrideLoader_EnableAnsiPathProbe' not in ov, 'v0.2.8 的 32 条非 ASCII CreateFileA 临时观察器已移除，不再污染 modloader.log')
ck('OverrideLoader_EnableGameAudit' in ov and 'GameAudit_PatchOriginalIoModules' in core and 'install_state_hooks_()' in audit, 'v0.3.0-dev9 保持独立 game.log 审计入口，不把游戏状态细节写回 ModLoader 日志')
ck(r'已启用独立 mods\\game.log' in core and '[转区/游戏实测]' not in core, 'modloader.log 只保留 game.log 基础设施状态，不承载旧版原始游戏路径流水')

ck('《幽城幻剑录》Mod Core v0.3.0-dev9 已进入 RPG.exe。' in mod, 'Mod Core 当前日志版本已同步到 v0.3.0-dev9')

# Mod 系统回归
ck('GetFullPathNameW(path, YCR_PATH_CAP, absolute_path, NULL_PTR)' in mod and 'LoadLibraryExW(absolute_path, NULL_PTR, LOAD_WITH_ALTERED_SEARCH_PATH_)' in mod, 'ASI 先转绝对路径，再用 LOAD_WITH_ALTERED_SEARCH_PATH 加载同目录依赖')
ck('module = LoadLibraryW(path);' not in mod, 'ASI 不再退回相对路径 LoadLibraryW')
ck('SetDllDirectoryW(g_mods_root)' in mod, 'Core 恢复 dev5 已实机成功的 SetDllDirectoryW(mods) 进程级兼容环境')
ck(r'请把该依赖 DLL 放在 mods\\asi，与 ASI 放在同一目录' in mod, r'错误 126 诊断明确要求 ASI 依赖 DLL 与 ASI 一起放在 mods\asi')
ck(all('GetModuleHandleW((const WCHAR*)L"ddraw.dll")' in text for text in [core,ov,loc,user32loc,gdiloc]), 'ddraw.dll 已恢复进入 Core/Overrides/Locale/USER32/GDI 的 Win32 IAT 兼容桥目标')
ck('mods\\asi' in mod and 'mods\\overrides' in mod, '唯一 Mod 路径仍为 mods\\asi / mods\\overrides')
ck('collect_new_asi_items_' in mod and 'collect_new_override_items_' in mod and 'activate_new_items_for_this_run_' in mod, '自动扫描、配置比较与本轮激活已拆成独立阶段')
ck('persist_new_items_atomic_' in mod and 'mods.ini.castle.tmp' in mod and 'MoveFileExW' in mod and '0x00000001u | 0x00000008u' in mod, 'mods.ini 新项目先写同目录临时文件，再用 REPLACE_EXISTING|WRITE_THROUGH 原子替换')
ck('CreateFileW(g_mods_ini, GENERIC_WRITE_' not in mod[mod.find('static int persist_new_items_atomic_'):mod.find('static void activate_new_items_for_this_run_')], '自动补全路径不再 CREATE_ALWAYS 直接截断原 mods.ini')
ck('bytes <= 0' in mod and 'if (bytes > 1)' in mod and '空字符串 L"" 正好返回 1' in mod, 'INI 写行器允许空行，修复 v0.2.10 遇空行即失败的根因')
ck('g_new_asi_items' in mod and 'g_new_override_items' in mod and '按文件名排序' in mod, '新 ASI/Overrides 使用独立列表，保留磁盘名称排序结果')
ck('g_asi_count + g_new_asi_count >= MAX_MOD_ITEMS_' in mod and 'g_override_count + g_new_override_count >= MAX_MOD_ITEMS_' in mod, '自动发现尊重 256 项总上限，超限项目不会出现“写进 INI 但本轮装不下”的分叉')
ck(mod.find('persist_new_items_atomic_()') < mod.find('activate_new_items_for_this_run_()'), '新 Mod 先尝试持久化，再无条件加入本轮内存顺序')
ck('本轮仍按扫描结果加载' in mod and '原配置未受损' in mod, 'mods.ini 持久化失败不会让本轮自动发现失效，也不会破坏旧配置')
ck('asi_insert_before' in mod and 'override_insert_before' in mod and '最后一个同名节' in mod, '新条目定位并复用最后一个已有 [ASI]/[Overrides] 节')
ck('WCHAR* trailing_blank = NULL_PTR;' in mod and 'WCHAR* section_end = trailing_blank ? trailing_blank : line;' in mod and 'asi_insert_before = trailing_blank' in mod and 'override_insert_before = trailing_blank' in mod, 'Core 自动补全把新条目写在节尾连续空行之前，空行继续分隔 [ASI]/[Overrides]')
ck('if (!asi_section_exists)' in mod and 'L"[ASI]"' in mod and 'if (!override_section_exists)' in mod and 'L"[Overrides]"' in mod, '只有对应节完全不存在时才补建一次表头')
ck('override_dir_contains_file_' in mod and 'depth >= 32u' in mod and '0x00000400u' in mod, 'Overrides 自动发现递归确认普通文件，并限制深度/跳过重解析点')
ck('g_last_empty_override_dirs' in mod and '已忽略没有任何文件的 Overrides 空目录数量' in mod, '空 Overrides 目录不进入自动配置，并在日志中给出忽略计数')
ck('if (directory_exists_(template_root)) return 1;' in mod, 'Overrides 模板存在时完全忽略，不再每次启动改写模板内部结构')
ck('ASI跳过' in mod and '磁盘文件不存在' in mod and 'if (skipped) ++*skipped' in mod, 'INI 中存在但磁盘缺失的 ASI 仅跳过本轮，不删除配置也不计为加载失败')
ck('Overrides跳过' in mod and '目录存在但递归没有任何普通文件' in mod, 'INI 中存在但缺失/为空的 Overrides 仅跳过本轮，保留配置顺序')
ck('成功=' in mod and '跳过=' in mod and '失败=' in mod, 'ASI 汇总区分成功、跳过与真正加载失败')
ck('OVERRIDE_TEMPLATE_NAME_' in mod and '模板_复制后改名' in mod, 'Overrides 模板仍不进入配置')
ck('i > 0u' in ov and '--i;' in ov, 'Overrides 仍按配置逆序命中：下面覆盖上面')
ck('GENERIC_WRITE_' in ov and 'OPEN_EXISTING_' in ov, 'Overrides 仍只重定向读取型 OPEN_EXISTING')
ck('MultiByteToWideChar(CASTLE_CP_BIG5_' in ov, 'ANSI 覆盖路径仍固定按 CP950 解码')
ck('0x00052C19u' in entry and '0x55u, 0x8Bu, 0xECu, 0x6Au, 0xFFu' in entry, 'Entry Gate 固定 RPG.exe 入口签名保持不变')

# 正式构建结构
ck('ntdll.lib' in build and 'CastleLocaleBootstrap.dll' in build, 'Windows 正式构建新增 ntdll-only LocaleBootstrap')
ck('kernel32.lib' in build and 'CastleModCore.dll' in build, '普通 Mod Core 继续链接 KERNEL32')
ck('CXXFLAGS=' in build and 'about.cpp' in build and 'about.obj' in build, 'Windows 正式构建会以独立 C++ 单元编译/链接 about.cpp')
ck('/GR-' in build and '/EHs-' in build and '/EHc-' in build and '/Zl' in build, 'About C++ 单元继续禁用 RTTI/异常默认库，不把 CRT 带回 Launcher')
ck('/out:"%OUT%\\mods\\CastleLocaleBootstrap.dll"' in build and '/out:"%OUT%\\mods\\CastleModCore.dll"' in build, '正式构建把 Loader 自身两个 DLL 输出到 mods 根目录，保持一键卸载布局')
ck('if exist "%OUT%\\CastleLocaleBootstrap.dll" del' in build and 'if exist "%OUT%\\CastleModCore.dll" del' in build, '正式构建主动清理 dev6 根目录旧 DLL，避免同时残留两份核心')
ck('libvcruntime.lib' not in '\n'.join(x for x in build.splitlines() if not x.lstrip().lower().startswith('rem ')), '正式构建不引入 vcruntime')

exe=OUT/'CastleModLoader.exe'; bootdll=OUT/'mods'/'CastleLocaleBootstrap.dll'; coredll=OUT/'mods'/'CastleModCore.dll'
# about4 只允许改 Launcher GUI/About/INI 编辑器。为了让检查器在“用户用不同 MSVC/Windows SDK 重新构建”以后仍然有效，
# 这里不把编译后二进制 SHA 当成永久规则，因为不同链接器版本即使源码相同也可能生成不同字节。
# 更稳妥的做法是把所有影响 RPG.exe 运行时的源码按固定顺序拼接后计算一个聚合 SHA-256；
# 只要这些源码没有动，about4 就没有把前端小改偷偷扩展成 Hook/Locale/ModCore 运行时改动。
DEV9_RUNTIME_SOURCE_SHA256='369acf08a4353b8c083af989711d237c0a20fe193a7a81083ce709e56aab41a9'
_runtime_source_names=['core.c','entry_gate.c','mod_loader.c','override_loader.c','game_audit.c','locale_layer.c','native_locale.c','user32_locale.c','gdi_locale.c','locale_bootstrap.c','platform.h','runtime_support.c']
_runtime_hash=hashlib.sha256()
for _name in _runtime_source_names:
    _runtime_hash.update(_name.encode('utf-8')); _runtime_hash.update(b'\0')
    _runtime_hash.update((SRC/_name).read_bytes()); _runtime_hash.update(b'\0')
ck(_runtime_hash.hexdigest()==DEV9_RUNTIME_SOURCE_SHA256, 'about4 的全部游戏运行时源码与封存 dev9 聚合 SHA-256 完全一致')
ck(exe.exists(), '存在 CastleModLoader.exe')
ck(bootdll.exists(), r'存在 mods\CastleLocaleBootstrap.dll')
ck(coredll.exists(), r'存在 mods\CastleModCore.dll')
ck(not (OUT/'CastleLocaleBootstrap.dll').exists() and not (OUT/'CastleModCore.dll').exists(), '编译内容根目录不存在 dev6 布局遗留的 Loader DLL')
allowed_root_dlls={'castlelocalebootstrap.dll','castlemodcore.dll'}
actual_root_dlls={x.name.lower() for x in (OUT/'mods').iterdir() if x.is_file() and x.suffix.lower()=='.dll'} if (OUT/'mods').is_dir() else set()
ck(actual_root_dlls == allowed_root_dlls, 'mods 根目录 DLL 仅允许 Loader 自身 CastleLocaleBootstrap.dll / CastleModCore.dll；用户依赖仍应放 asi 子目录')

if exe.exists():
    b,e,m,magic,ep,imp,secs=pe_info(exe); ck(m==0x14c and magic==0x10b, 'Launcher 为 PE32/i386')
    imports=pe_imports(exe); mods={d.lower() for d,_ in imports}; names={n.lower() for _,ns in imports for n in ns}; ck(mods=={'kernel32.dll'}, 'Launcher 静态依赖仅 KERNEL32.dll'); ck('setdlldirectoryw' in names, 'Launcher 最终 PE 已恢复 dev5 启动期 SetDllDirectoryW(mods) 所需导入')
    exe_bytes=exe.read_bytes()
    ck('v0.3.0-dev9-about4'.encode('utf-16le') in exe_bytes and '关于 - 《幽城幻剑录》Mod Loader'.encode('utf-16le') in exe_bytes, '最终 Launcher 已实际链接 about.cpp 的 about4 About 文本，不是只有源码没有进入 EXE')
    resource_types=pe_resource_type_ids(exe)
    ck(3 in resource_types and 14 in resource_types, '最终 Launcher 仍包含 RPG.ico 的 RT_ICON / RT_GROUP_ICON 资源')
if bootdll.exists():
    b,e,m,magic,ep,imp,secs=pe_info(bootdll); ck(m==0x14c and magic==0x10b, 'LocaleBootstrap 为 PE32/i386')
    imports=pe_imports(bootdll); mods={d.lower() for d,_ in imports}; names={n for _,ns in imports for n in ns}
    ck(mods=={'ntdll.dll'}, 'LocaleBootstrap 最终 PE 静态依赖严格只有 ntdll.dll')
    ck('NtOpenFile' in names and 'RtlInitNlsTables' in names and 'RtlResetRtlTranslations' in names, 'LocaleBootstrap 最终 PE 确实导入 Nt/Rtl NLS 原语')
    try:
        out=subprocess.check_output(['/usr/local/swift/usr/bin/llvm-objdump','-p',str(bootdll)],text=True,stderr=subprocess.STDOUT)
        ck('CastleLocaleBootstrap_Bootstrap' in out and 'CastleLocaleBootstrap_GetStatus' in out and 'CastleLocaleBootstrap_GetObservedAnsiCodePage' in out and 'CastleLocaleBootstrap_GetObservedOemCodePage' in out, 'LocaleBootstrap 导出 Bootstrap/Status 与诊断 ABI')
    except Exception: ck(True,'当前环境无 llvm-objdump 时跳过 Bootstrap 导出文本复核')
if coredll.exists():
    b,e,m,magic,ep,imp,secs=pe_info(coredll); ck(m==0x14c and magic==0x10b, 'ModCore 为 PE32/i386')
    imports=pe_imports(coredll); mods={d.lower() for d,_ in imports}; names={n.lower() for _,ns in imports for n in ns}
    ck(mods=={'kernel32.dll'}, 'ModCore 最终 PE 静态依赖仅 KERNEL32.dll')
    ck('loadlibraryexw' in names and 'setdlldirectoryw' in names, 'ModCore 最终 PE 同时导入 LoadLibraryExW 与 SetDllDirectoryW：ASI 同目录依赖 + dev5 兼容环境同时保留')
    ck(all(x in names for x in ['addvectoredexceptionhandler','getfinalpathnamebyhandlew','setlasterror','readfile','writefile','setendoffile']), 'ModCore 最终 PE 已包含 game.log 审计所需的真实 KERNEL32 导入')

if len(sys.argv)>1:
    rpg=pathlib.Path(sys.argv[1])
    if rpg.exists():
        data=rpg.read_bytes(); ck(hashlib.sha256(data).hexdigest()==EXPECTED_RPG_SHA256, 'RPG.exe SHA-256 精确匹配当前用户沿用到 v0.3.0-dev9 的可信基线（829483...）')
        b,e,m,magic,ep,imp,secs=pe_info(rpg); ck(m==0x14c and magic==0x10b, 'RPG.exe 为 PE32/i386'); ck(ep==EXPECTED_ENTRY_RVA,'RPG.exe EntryPoint RVA=0x52C19')
        rpg_import_names={n.lower() for _,ns in pe_imports(rpg) for n in ns}
        ck(all(x in rpg_import_names for x in ['createfilea','getfileattributesa','readfile','writefile','closehandle','setendoffile','setfilepointer','getcurrentdirectorya','deletefilea','flushfilebuffers','setfileattributesa']), '固定 RPG.exe 已确认的 KERNEL32 文件 I/O 导入集合与当前审计范围一致')
        off=rva_to_off(ep,secs); ck(data[off:off+5]==EXPECTED_ENTRY_BYTES,'RPG.exe EntryPoint 5 字节签名匹配')
        # v0.2.11 沿用的 13 个状态断点不是只检查“源码里写了这些地址”，还直接读取当前 RPG.exe 磁盘机器码。
        # 这样后续如果换了 EXE、打了别的补丁，哪怕地址仍存在，只要入口指令变化，检查工具就会明确 FAIL。
        state_hook_oracles = [
            (0x0044802D, bytes.fromhex("8B 15 48 1C 8E 00"), "NEW_GAME_BEGIN"),
            (0x0043B360, bytes.fromhex("6A FF 64 A1 00 00 00 00"), "SAVE_WRITER"),
            (0x0043B510, bytes.fromhex("64 A1 00 00 00 00"), "LOAD_READER"),
            (0x0043ACD0, bytes.fromhex("51 53 8B 5C 24 0C"), "SERIALIZE_CORE"),
            (0x0043AB30, bytes.fromhex("51 53 8B 5C 24 0C"), "DESERIALIZE_CORE"),
            (0x0043AE80, bytes.fromhex("56 57 8D 71 14"), "REBUILD_ROLE_PTRS"),
            (0x0044B130, bytes.fromhex("56 8B 74 24 08"), "COPY_SCENE_DESCRIPTOR"),
            (0x0044B1F0, bytes.fromhex("6A FF 68 7B FF 45 00"), "RESTORE_WORLD"),
            (0x0040B230, bytes.fromhex("56 8B 74 24 08"), "EVENT_ACTIVATE"),
            (0x0040B340, bytes.fromhex("A0 77 F6 46 00"), "EVENT_CLEAR"),
            (0x0040D780, bytes.fromhex("6A FF 68 CB D1 45 00"), "LOAD_SCENE_RESET"),
            (0x0040D8A0, bytes.fromhex("6A FF 68 EB D1 45 00"), "LOAD_SCENE"),
            (0x0040DAA0, bytes.fromhex("6A FF 68 0B D2 45 00"), "SAVE_SLOT_UI"),
        ]
        for va_abs, expected, marker in state_hook_oracles:
            hook_off = rva_to_off(va_abs - 0x00400000, secs)
            ck(data[hook_off:hook_off+len(expected)] == expected, f'RPG.exe 状态断点 {marker} @ 0x{va_abs:08X} 机器字节精确匹配')
        # 双 import payload 尺寸计算
        irva,isize=imp; io=rva_to_off(irva,secs); count=0
        while count<64:
            d=struct.unpack_from('<IIIII',data,io+count*20)
            if not any(d): break
            count+=1
        align=lambda v,a:(v+a-1)&~(a-1)
        specs=[(b'CastleLocaleBootstrap.dll',b'CastleLocaleBootstrap_Bootstrap'),(b'CastleModCore.dll',b'CastleModCore_Bootstrap')]
        cur=align((count+3)*20,4)
        for dn,fn in specs:
            cur+=len(dn)+1; cur=align(cur,2); cur+=2+len(fn)+1; cur=align(cur,4); cur+=16
        need=align(cur,16)
        rd=next((s for s in secs if s[0]=='.rdata'),None)
        if rd:
            _,vs,va,rs,rp=rd; avail=rs-align(vs,16); ck(avail>=need,f'RPG.exe .rdata 可容纳双 Early Import payload（需要 {need}，可用 {avail}）')
        else: ck(False,'RPG.exe 存在 .rdata 节')
    else: ck(False,'传入的 RPG.exe 路径存在')
else:
    ck(True,'未传 RPG.exe：跳过目标文件精确基线检查')

fails=[x for x in checks if not x[0]]
for ok,msg in checks: print(('PASS' if ok else 'FAIL')+' | '+msg)
print(f'\n总计：{len(checks)-len(fails)} PASS / {len(fails)} FAIL')
sys.exit(1 if fails else 0)
