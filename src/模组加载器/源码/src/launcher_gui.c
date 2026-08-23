#include "platform.h"
#include "launcher_app.h"
#include "launcher_gui.h"
#include "launcher_mod_config.h"
#include "resource.h"
#include "about.h"

/*
 * 《幽城幻剑录》Mod Loader GUI，v0.3.0-dev9-about5（dev9封存运行时 + About 标准标题栏修订）。
 *
 * dev1 已经把“扫描、启停、拖动排序、设置、启动”这条业务链跑通，但实机截图暴露出一个很明确的问题：
 * 它虽然功能完整，却仍然保留了过多传统 Win32 控件的默认视觉——三维边框、强烈系统高亮、大片空白列表、
 * 设置窗口里的灰底复选框——看起来更像调试工具，而不是给玩家长期使用的启动器。
 *
 * dev3 在 dev2 视觉基线上继续修三个实机反馈点，不碰已经验证过的 Pre-Loader / Core 启动语义：
 *   1. ASI 与 Overrides 每行增加明确的拖动把手，并继续支持同类型列表内按住拖动排序；
 *   2. 对磁盘对象已经缺失的条目，在行尾显示“小型移除”按钮，只清理 mods.ini 陈旧引用，不删除磁盘文件；
 *   3. 设置窗口从“大卡片”改成紧凑单行设置列表，按钮同步缩小，避免 96/125/150% DPI 下内容压到底部按钮；
 *   4. 设置区按“分类标题 + 紧凑行”的结构布局，为以后继续增加功能保留更多纵向空间。
 *
 * 为什么仍然保留左右两列：ASI 与 Overrides 的“相对顺序”没有任何跨类型意义。把两类 Mod 混在一列会制造
 * 错误的优先级暗示，所以视觉可以变现代，业务分区不能为了“看起来简洁”而被合并。
 *
 * dev4 在 dev3 已实机确认“可用”的基础上完成顶部说明与 RPG.ico 接入，但实机继续暴露两个窗口尺寸问题：
 *   1. 取消最小尺寸后，窗口可以缩到布局公式本来就不支持的极端宽度，文字、列表与底部按钮会发生严重挤压；
 *   2. 只重画父窗口并不足以保证所有子窗口和“子窗口移动后露出的旧区域”在同一帧更新，连续缩放仍可能看到残留。
 *
 * dev5 因此按实机结论做针对性修正，并且仍然完全不碰 Pre-Loader / Core / Hook 业务环境：
 *   1. 主窗口恢复最小跟踪尺寸，最小客户区固定为 1024×640 DIP；Windows 标题栏与可调整边框属于非客户区，
 *      会由系统按当前 DPI 自动追加，所以用户要求的“1024×640 + 标题栏”不会把标题栏挤进内容区；
 *   2. 默认打开尺寸直接等于这个最小客户区尺寸，因此默认状态就是允许缩到的最小状态；
 *   3. WM_SIZE 中先一次性移动所有主控件但暂不逐个重画，再用 RedrawWindow 对父窗口和全部子窗口做同步擦除/重绘；
 *      这样旧卡片边框、旧 ListBox 区域和按钮移动前的位置都在当前缩放消息内被清掉，不再等鼠标松开；
 *   4. 客户区继续不重复显示大标题，顶部两行说明、分隔线、Mod 卡片、拖动/移除等 dev4 视觉与交互全部保留；
 *   5. Launcher 继续使用用户提供的 RPG.ico 作为 EXE 资源和窗口类图标。
 *
 * dev6 在上述已验收 GUI 上追加两项：
 *   1. 有“同名 .ini”的 ASI 行显示“编辑”按钮，打开内置 RichEdit 编辑器；注释、节名、变量名、等号和值分别着色；
 *   2. 保存前执行通用 INI 语法检查，错误会报告具体行号并选中问题行；保存采用同目录临时文件 + 原子替换，并尽量保持原编码。
 *
 *
 * dev7 根据 Windows 实机截图修正 INI 编辑器的分色坐标，并把 Loader 内部两个 DLL 的发布位置恢复到 mods\：
 *   1. RichEdit 分色不再用 WM_GETTEXT 缓冲区下标直接充当选择坐标，改用 EM_LINEINDEX / EM_LINELENGTH / EM_GETTEXTRANGE；
 *   2. 因此 CR/LF 表示差异不会再让颜色每换一行就累计错位，整行注释始终保持统一绿色；
 *   3. Loader 内部 DLL 的目录调整属于 launcher/build 侧，本 GUI 仍不改变 ddraw 或游戏 Hook 边界。
 *
 * dev9-about5 继续只改已经封存 dev9 的 Launcher GUI，不修改游戏运行时链：
 *   1. “关于”不再用 MessageBox，而是独立的自定义模态窗口；about.cpp 可以集中维护正文和最多 8 个可点击链接；
 *   2. 链接通过运行时动态加载 shell32!ShellExecuteW 打开，因此不会给 Launcher 增加静态 Shell32 依赖；
 *   3. INI 自动换行改为“实际布局完成后强制重新建立 RichEdit 换行宽度”，并在每次 WM_SIZE 后重算；
 *   4. 分色和错误定位不再依赖会受视觉折行影响的 EM_GETLINECOUNT/EM_LINEINDEX，而是按 RichEdit 内部 CR 字符查找逻辑行；
 *   5. 未保存与保存错误状态统一使用醒目的亮红字；编辑后立刻显示“● 未保存修改”，保存失败则保留具体错误摘要；
 *   6. CastleModCore.dll / CastleLocaleBootstrap.dll 仍保持 dev9 封存运行时边界，本轮不改 Hook、转区、ASI、Overrides 或 ddraw 兼容行为；
 *   7. about5 取消 About 错误的 WS_EX_TOOLWINDOW 扩展风格，让 Windows 使用与“设置”一致的普通标题栏/关闭按钮；
 *      About 仍由主窗口拥有、仍走同一模态消息循环，因此这只是非客户区外观修正，不改变窗口生命周期。
 *
 * 依赖策略保持不变：GUI 运行时动态加载 USER32/GDI32，INI 编辑器需要时再动态加载系统 Msftedit/Riched20。CastleModLoader.exe 的静态导入仍只需要 KERNEL32，
 * 因而本轮视觉修改不会反向改变已经验证过的 Pre-Loader 最早期依赖边界。
 */

/* ---------- GUI 专用 Win32 最小类型 ---------- */

typedef LONG LRESULT_;
typedef ULONG_PTR WPARAM_;
typedef LONG LPARAM_;
typedef LONG (CALLBACK *WNDPROC_)(HWND, UINT, WPARAM_, LPARAM_);
typedef void* HDC_;
typedef void* HBRUSH_;
typedef void* HFONT_;
typedef void* HGDIOBJ_;
typedef void* HCURSOR_;
typedef void* HMONITOR_;
typedef WORD ATOM_;

typedef struct POINT_ { LONG x; LONG y; } POINT_;
typedef struct RECT_ { LONG left, top, right, bottom; } RECT_;
typedef struct MSG_ {
    HWND hwnd;
    UINT message;
    WPARAM_ wParam;
    LPARAM_ lParam;
    DWORD time;
    POINT_ pt;
} MSG_;
typedef struct PAINTSTRUCT_ {
    HDC_ hdc;
    BOOL fErase;
    RECT_ rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    BYTE rgbReserved[32];
} PAINTSTRUCT_;
typedef struct WNDCLASSEXW_ {
    UINT cbSize;
    UINT style;
    WNDPROC_ lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    void* hIcon;
    HCURSOR_ hCursor;
    HBRUSH_ hbrBackground;
    LPCWSTR lpszMenuName;
    LPCWSTR lpszClassName;
    void* hIconSm;
} WNDCLASSEXW_;
typedef struct DRAWITEMSTRUCT_ {
    UINT CtlType;
    UINT CtlID;
    UINT itemID;
    UINT itemAction;
    UINT itemState;
    HWND hwndItem;
    HDC_ hDC;
    RECT_ rcItem;
    ULONG_PTR itemData;
} DRAWITEMSTRUCT_;
typedef struct MONITORINFO_ {
    DWORD cbSize;
    RECT_ rcMonitor;
    RECT_ rcWork;
    DWORD dwFlags;
} MONITORINFO_;

/*
 * WM_GETMINMAXINFO 会把一个 MINMAXINFO 指针放在 lParam 中。
 * 我们只修改 ptMinTrackSize，也就是“用户拖动窗口边框时允许缩到的最小外框尺寸”；其余字段保持 Windows 给出的值。
 */
typedef struct MINMAXINFO_ {
    POINT_ ptReserved;
    POINT_ ptMaxSize;
    POINT_ ptMaxPosition;
    POINT_ ptMinTrackSize;
    POINT_ ptMaxTrackSize;
} MINMAXINFO_;

/*
 * RichEdit 的字符着色只需要两个极小结构。CHARRANGE 表示“从第几个字符到第几个字符”，
 * CHARFORMATW 则告诉 RichEdit 这段文字应该使用什么颜色。我们没有复制整套 RichEdit SDK，
 * 只声明本编辑器真正会用到的字段，仍保持项目“不包含 windows.h / richedit.h”的最小依赖策略。
 */
typedef struct CHARRANGE_ {
    LONG cpMin;
    LONG cpMax;
} CHARRANGE_;

/*
 * TEXTRANGEW 是 RichEdit 的“按控件内部字符坐标读取一段文字”结构。
 * dev6 的颜色错乱根因就是把 WM_GETTEXT 得到的换行表示直接当成 RichEdit 选择坐标；
 * 不同 RichEdit 版本会把 CR/LF 以不同方式暴露给 WM_GETTEXT，导致第二行以后字符位置逐步偏移。
 * dev7 改为先用 EM_LINEINDEX/EM_LINELENGTH 取得 RichEdit 自己认可的坐标，再用 EM_GETTEXTRANGE 取行内容，
 * 因而分色范围和控件内部位置始终来自同一套坐标系。
 */
typedef struct TEXTRANGEW_ {
    CHARRANGE_ chrg;
    WCHAR* lpstrText;
} TEXTRANGEW_;

/*
 * FINDTEXTEXW 是 RichEdit 自己的“按内部字符坐标搜索文字”结构。
 * about3 用它寻找真正的 '\r' 段落分隔符，从而得到“INI 逻辑行”的精确 cp 范围。
 * 这点非常重要：开启自动换行后，EM_GETLINECOUNT/EM_LINEINDEX 会把一条很长的 INI 行
 * 拆成多条“视觉行”，如果继续拿视觉行做语法分色，颜色和错误定位都会错。
 */
typedef struct FINDTEXTEXW_ {
    CHARRANGE_ chrg;
    LPCWSTR lpstrText;
    CHARRANGE_ chrgText;
} FINDTEXTEXW_;

typedef struct CHARFORMATW_ {
    UINT cbSize;
    DWORD dwMask;
    DWORD dwEffects;
    LONG yHeight;
    LONG yOffset;
    DWORD crTextColor;
    BYTE bCharSet;
    BYTE bPitchAndFamily;
    WCHAR szFaceName[32];
} CHARFORMATW_;

/* ---------- GUI 常量：只列本文件实际使用的 Win32 值 ---------- */

#define WS_OVERLAPPED_       0x00000000u
#define WS_CAPTION_          0x00C00000u
#define WS_SYSMENU_          0x00080000u
#define WS_THICKFRAME_       0x00040000u
#define WS_MINIMIZEBOX_      0x00020000u
#define WS_CLIPCHILDREN_     0x02000000u
#define WS_CHILD_            0x40000000u
#define WS_VISIBLE_          0x10000000u
#define WS_TABSTOP_          0x00010000u
#define WS_VSCROLL_          0x00200000u
#define WS_HSCROLL_          0x00100000u
#define WS_OVERLAPPEDWINDOW_ (WS_OVERLAPPED_ | WS_CAPTION_ | WS_SYSMENU_ | WS_THICKFRAME_ | WS_MINIMIZEBOX_)
#define WS_EX_CLIENTEDGE_    0x00000200u
#define WS_EX_CONTROLPARENT_ 0x00010000u

#define BS_PUSHBUTTON_       0x00000000u
#define BS_DEFPUSHBUTTON_    0x00000001u
#define BS_AUTOCHECKBOX_     0x00000003u
#define BS_OWNERDRAW_        0x0000000Bu

/* RichEdit 文本区需要多行、滚动和回车输入；这些都是标准 EDIT/RichEdit 风格。 */
#define ES_MULTILINE_        0x0004u
#define ES_AUTOVSCROLL_      0x0040u
#define ES_AUTOHSCROLL_      0x0080u
#define ES_NOHIDESEL_        0x0100u
#define ES_WANTRETURN_       0x1000u

#define LBS_NOTIFY_          0x0001u
#define LBS_OWNERDRAWFIXED_  0x0010u
#define LBS_HASSTRINGS_      0x0040u
#define LBS_NOINTEGRALHEIGHT_ 0x0100u

#define WM_CREATE_           0x0001u
#define WM_DESTROY_          0x0002u
#define WM_SIZE_             0x0005u
#define WM_SETFOCUS_         0x0007u
#define WM_PAINT_            0x000Fu
#define WM_CLOSE_            0x0010u
#define WM_ERASEBKGND_       0x0014u
#define WM_GETMINMAXINFO_     0x0024u
#define WM_DRAWITEM_         0x002Bu
#define WM_SETFONT_          0x0030u
#define WM_SETREDRAW_        0x000Bu
#define WM_GETTEXT_          0x000Du
#define WM_GETTEXTLENGTH_    0x000Eu
#define WM_GETFONT_          0x0031u
#define WM_SETCURSOR_        0x0020u
#define WM_COMMAND_          0x0111u
#define WM_TIMER_            0x0113u
#define WM_MOUSEMOVE_        0x0200u
#define WM_LBUTTONDOWN_      0x0201u
#define WM_LBUTTONUP_        0x0202u
#define WM_CAPTURECHANGED_   0x0215u
#define WM_DPICHANGED_       0x02E0u

#define BN_CLICKED_          0u
#define LBN_SELCHANGE_       1u
#define EN_CHANGE_           0x0300u

#define LB_ADDSTRING_        0x0180u
#define LB_RESETCONTENT_     0x0184u
#define LB_SETCURSEL_        0x0186u
#define LB_GETCURSEL_        0x0188u
#define LB_GETCOUNT_         0x018Bu
#define LB_GETITEMRECT_      0x0198u
#define LB_SETITEMHEIGHT_    0x01A0u
#define LB_ITEMFROMPOINT_    0x01A9u
#define LB_ERR_              ((LONG)-1)

/*
 * RichEdit 消息：数值来自 Win32 RichEdit ABI。
 * EM_EXGETSEL / EM_EXSETSEL 使用 32 位字符位置，所以即使 INI 超过 64 KiB，也不会退化成旧 EM_GETSEL 的 16 位范围。
 */
#define WM_USER_             0x0400u
#define EM_EXGETSEL_         (WM_USER_ + 52u)
#define EM_EXLIMITTEXT_      (WM_USER_ + 53u)
#define EM_EXSETSEL_         (WM_USER_ + 55u)
#define EM_GETFIRSTVISIBLELINE_ 0x00CEu
#define EM_LINESCROLL_       0x00B6u
#define EM_GETLINECOUNT_     0x00BAu
#define EM_LINEINDEX_        0x00BBu
#define EM_LINELENGTH_       0x00C1u
#define EM_SETLIMITTEXT_     0x00C5u
#define EM_SETBKGNDCOLOR_    (WM_USER_ + 67u)
#define EM_SETCHARFORMAT_    (WM_USER_ + 68u)
#define EM_SETEVENTMASK_     (WM_USER_ + 69u)
#define EM_SETTARGETDEVICE_  (WM_USER_ + 72u)
#define EM_GETTEXTRANGE_     (WM_USER_ + 75u)
#define EM_FINDTEXTEXW_       (WM_USER_ + 124u)
#define FR_DOWN_              0x00000001u
#define SCF_SELECTION_       0x0001u
#define SCF_ALL_             0x0004u
#define CFM_COLOR_           0x40000000u
#define ENM_CHANGE_          0x00000001u

#define BM_GETCHECK_         0x00F0u
#define BM_SETCHECK_         0x00F1u
#define BST_UNCHECKED_       0u
#define BST_CHECKED_         1u

#define SW_HIDE_             0
#define SW_SHOW_             5
#define SW_SHOWNORMAL_       1
#define SWP_NOSIZE_          0x0001u
#define SWP_NOMOVE_          0x0002u
#define SWP_NOZORDER_        0x0004u
#define SWP_NOACTIVATE_      0x0010u

/* RedrawWindow 标志：一次让父窗口和所有子窗口失效、擦背景并立即完成重绘。 */
#define RDW_INVALIDATE_       0x0001u
#define RDW_ERASE_            0x0004u
#define RDW_ALLCHILDREN_      0x0080u
#define RDW_UPDATENOW_        0x0100u

#define COLOR_WINDOW_        5
#define COLOR_WINDOWTEXT_    8
#define COLOR_BTNFACE_       15
#define COLOR_BTNTEXT_       18
#define COLOR_HIGHLIGHT_     13
#define COLOR_HIGHLIGHTTEXT_ 14
#define COLOR_GRAYTEXT_      17

/*
 * Win32 GDI 使用 COLORREF：最低 8 位是红色，其次绿色，再其次蓝色。
 * 把颜色都经由 RGB_() 生成，维护者只需要按日常 R/G/B 顺序阅读，不必记住 COLORREF 的字节顺序。
 */
#define RGB_(r,g,b) ((DWORD)((((DWORD)(BYTE)(r))) | (((DWORD)(BYTE)(g)) << 8) | (((DWORD)(BYTE)(b)) << 16)))

#define TRANSPARENT_         1
#define FW_NORMAL_           400
#define FW_SEMIBOLD_         600
#define DEFAULT_CHARSET_     1u
#define OUT_DEFAULT_PRECIS_  0u
#define CLIP_DEFAULT_PRECIS_ 0u
#define CLEARTYPE_QUALITY_   5u
#define DEFAULT_PITCH_       0u

#define DT_LEFT_             0x00000000u
#define DT_CENTER_           0x00000001u
#define DT_RIGHT_            0x00000002u
#define DT_VCENTER_          0x00000004u
#define DT_SINGLELINE_       0x00000020u
#define DT_WORDBREAK_         0x00000010u
#define DT_CALCRECT_          0x00000400u
#define DT_END_ELLIPSIS_     0x00008000u
#define DT_NOPREFIX_         0x00000800u

#define ODS_SELECTED_        0x0001u
#define ODS_FOCUS_           0x0010u
#define ODS_DISABLED_        0x0004u

#define DFC_BUTTON_          4u
#define DFCS_BUTTONCHECK_    0x0000u
#define DFCS_CHECKED_        0x0400u
#define DFCS_INACTIVE_       0x0100u

#define GWL_WNDPROC_         (-4)
#define GWL_STYLE_           (-16)
#define GWL_EXSTYLE_         (-20)
#define GWLP_USERDATA_       (-21)

#define IDC_ARROW_           ((LPCWSTR)(ULONG_PTR)32512u)
#define MONITOR_DEFAULTTONEAREST_ 2u
#define SPI_GETWORKAREA_     0x0030u

#define MB_OKCANCEL_         0x00000001u
#define MB_YESNOCANCEL_      0x00000003u
#define MB_ICONWARNING_      0x00000030u
#define MB_ICONINFORMATION_  0x00000040u
#define MB_ICONQUESTION_     0x00000020u
#define IDOK_                1
#define IDCANCEL_            2
#define IDYES_                6
#define IDNO_                 7

/* ---------- 控件 ID ---------- */

#define IDC_ASI_LIST_        1001
#define IDC_OVERRIDE_LIST_   1002
#define IDC_SETTINGS_        1003
#define IDC_REFRESH_         1004
#define IDC_LAUNCH_          1005
#define IDC_ABOUT_           1006
#define IDC_SETTING_MODLOG_  1101
#define IDC_SETTING_GAMELOG_ 1102
#define IDC_SETTING_SAVE_    1103
#define IDC_SETTING_CANCEL_  1104
#define IDC_INI_EDITOR_TEXT_  1201
#define IDC_INI_EDITOR_SAVE_  1202
#define IDC_INI_EDITOR_CLOSE_ 1203
#define IDC_ABOUT_CLOSE_       1301
#define IDC_ABOUT_LINK_BASE_   1310

/* ---------- USER32 / GDI32 动态 API ---------- */

typedef ATOM_ (WINAPI *PFN_RegisterClassExW_)(const WNDCLASSEXW_*);
typedef HWND (WINAPI *PFN_CreateWindowExW_)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HANDLE, HINSTANCE, LPVOID);
typedef LRESULT_ (WINAPI *PFN_DefWindowProcW_)(HWND, UINT, WPARAM_, LPARAM_);
typedef BOOL (WINAPI *PFN_ShowWindow_)(HWND, int);
typedef BOOL (WINAPI *PFN_UpdateWindow_)(HWND);
typedef int (WINAPI *PFN_GetMessageW_)(MSG_*, HWND, UINT, UINT);
typedef BOOL (WINAPI *PFN_TranslateMessage_)(const MSG_*);
typedef LRESULT_ (WINAPI *PFN_DispatchMessageW_)(const MSG_*);
typedef void (WINAPI *PFN_PostQuitMessage_)(int);
typedef BOOL (WINAPI *PFN_DestroyWindow_)(HWND);
typedef BOOL (WINAPI *PFN_GetClientRect_)(HWND, RECT_*);
typedef BOOL (WINAPI *PFN_GetWindowRect_)(HWND, RECT_*);
typedef BOOL (WINAPI *PFN_SetWindowPos_)(HWND, HWND, int, int, int, int, UINT);
typedef BOOL (WINAPI *PFN_MoveWindow_)(HWND, int, int, int, int, BOOL);
typedef LRESULT_ (WINAPI *PFN_SendMessageW_)(HWND, UINT, WPARAM_, LPARAM_);
typedef BOOL (WINAPI *PFN_SetWindowTextW_)(HWND, LPCWSTR);
typedef BOOL (WINAPI *PFN_EnableWindow_)(HWND, BOOL);
typedef BOOL (WINAPI *PFN_InvalidateRect_)(HWND, const RECT_*, BOOL);
typedef BOOL (WINAPI *PFN_RedrawWindow_)(HWND, const RECT_*, HANDLE, UINT);
typedef BOOL (WINAPI *PFN_AdjustWindowRectEx_)(RECT_*, DWORD, BOOL, DWORD);
typedef BOOL (WINAPI *PFN_AdjustWindowRectExForDpi_)(RECT_*, DWORD, BOOL, DWORD, UINT);
typedef HDC_ (WINAPI *PFN_BeginPaint_)(HWND, PAINTSTRUCT_*);
typedef BOOL (WINAPI *PFN_EndPaint_)(HWND, const PAINTSTRUCT_*);
/* About 需要在真正绘制前测量正文高度，所以额外动态解析 GetDC/ReleaseDC；这两个 API 只服务 GUI 几何计算。 */
typedef HDC_ (WINAPI *PFN_GetDC_)(HWND);
typedef int (WINAPI *PFN_ReleaseDC_)(HWND, HDC_);
typedef int (WINAPI *PFN_DrawTextW_)(HDC_, LPCWSTR, int, RECT_*, UINT);
typedef int (WINAPI *PFN_FillRect_)(HDC_, const RECT_*, HBRUSH_);
typedef int (WINAPI *PFN_FrameRect_)(HDC_, const RECT_*, HBRUSH_);
typedef BOOL (WINAPI *PFN_DrawFrameControl_)(HDC_, RECT_*, UINT, UINT);
typedef HBRUSH_ (WINAPI *PFN_GetSysColorBrush_)(int);
typedef DWORD (WINAPI *PFN_GetSysColor_)(int);
typedef HCURSOR_ (WINAPI *PFN_LoadCursorW_)(HINSTANCE, LPCWSTR);
/* LoadIconW 从本 EXE 的资源表读取 RPG.ico；返回的是共享图标句柄，不需要手工 DestroyIcon。 */
typedef void* (WINAPI *PFN_LoadIconW_)(HINSTANCE, LPCWSTR);
typedef HWND (WINAPI *PFN_SetCapture_)(HWND);
typedef BOOL (WINAPI *PFN_ReleaseCapture_)(void);
typedef HWND (WINAPI *PFN_GetCapture_)(void);
typedef LONG (WINAPI *PFN_SetWindowLongW_)(HWND, int, LONG);
typedef LONG (WINAPI *PFN_GetWindowLongW_)(HWND, int);
typedef LRESULT_ (WINAPI *PFN_CallWindowProcW_)(WNDPROC_, HWND, UINT, WPARAM_, LPARAM_);
typedef int (WINAPI *PFN_GetDlgCtrlID_)(HWND);
typedef HWND (WINAPI *PFN_SetFocus_)(HWND);
typedef int (WINAPI *PFN_MessageBoxW_)(HWND, LPCWSTR, LPCWSTR, UINT);
typedef BOOL (WINAPI *PFN_GetCursorPos_)(POINT_*);
typedef HMONITOR_ (WINAPI *PFN_MonitorFromPoint_)(POINT_, DWORD);
typedef BOOL (WINAPI *PFN_GetMonitorInfoW_)(HMONITOR_, MONITORINFO_*);
typedef BOOL (WINAPI *PFN_SystemParametersInfoW_)(UINT, UINT, LPVOID, UINT);
typedef UINT (WINAPI *PFN_GetDpiForWindow_)(HWND);
typedef BOOL (WINAPI *PFN_SetProcessDPIAware_)(void);
typedef BOOL (WINAPI *PFN_SetProcessDpiAwarenessContext_)(HANDLE);

typedef HFONT_ (WINAPI *PFN_CreateFontW_)(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCWSTR);
typedef BOOL (WINAPI *PFN_DeleteObject_)(HGDIOBJ_);
typedef HGDIOBJ_ (WINAPI *PFN_SelectObject_)(HDC_, HGDIOBJ_);
typedef int (WINAPI *PFN_SetBkMode_)(HDC_, int);
typedef DWORD (WINAPI *PFN_SetTextColor_)(HDC_, DWORD);
typedef HBRUSH_ (WINAPI *PFN_CreateSolidBrush_)(DWORD);

typedef struct UiApi_ {
    HMODULE user32;
    HMODULE gdi32;
    PFN_RegisterClassExW_ RegisterClassExW;
    PFN_CreateWindowExW_ CreateWindowExW;
    PFN_DefWindowProcW_ DefWindowProcW;
    PFN_ShowWindow_ ShowWindow;
    PFN_UpdateWindow_ UpdateWindow;
    PFN_GetMessageW_ GetMessageW;
    PFN_TranslateMessage_ TranslateMessage;
    PFN_DispatchMessageW_ DispatchMessageW;
    PFN_PostQuitMessage_ PostQuitMessage;
    PFN_DestroyWindow_ DestroyWindow;
    PFN_GetClientRect_ GetClientRect;
    PFN_GetWindowRect_ GetWindowRect;
    PFN_SetWindowPos_ SetWindowPos;
    PFN_MoveWindow_ MoveWindow;
    PFN_SendMessageW_ SendMessageW;
    PFN_SetWindowTextW_ SetWindowTextW;
    PFN_EnableWindow_ EnableWindow;
    PFN_InvalidateRect_ InvalidateRect;
    PFN_RedrawWindow_ RedrawWindow;
    PFN_AdjustWindowRectEx_ AdjustWindowRectEx;
    PFN_AdjustWindowRectExForDpi_ AdjustWindowRectExForDpi;
    PFN_BeginPaint_ BeginPaint;
    PFN_EndPaint_ EndPaint;
    PFN_GetDC_ GetDC;
    PFN_ReleaseDC_ ReleaseDC;
    PFN_DrawTextW_ DrawTextW;
    PFN_FillRect_ FillRect;
    PFN_FrameRect_ FrameRect;
    PFN_DrawFrameControl_ DrawFrameControl;
    PFN_GetSysColorBrush_ GetSysColorBrush;
    PFN_GetSysColor_ GetSysColor;
    PFN_LoadCursorW_ LoadCursorW;
    PFN_LoadIconW_ LoadIconW;
    PFN_SetCapture_ SetCapture;
    PFN_ReleaseCapture_ ReleaseCapture;
    PFN_GetCapture_ GetCapture;
    PFN_SetWindowLongW_ SetWindowLongW;
    PFN_GetWindowLongW_ GetWindowLongW;
    PFN_CallWindowProcW_ CallWindowProcW;
    PFN_GetDlgCtrlID_ GetDlgCtrlID;
    PFN_SetFocus_ SetFocus;
    PFN_MessageBoxW_ MessageBoxW;
    PFN_GetCursorPos_ GetCursorPos;
    PFN_MonitorFromPoint_ MonitorFromPoint;
    PFN_GetMonitorInfoW_ GetMonitorInfoW;
    PFN_SystemParametersInfoW_ SystemParametersInfoW;
    PFN_GetDpiForWindow_ GetDpiForWindow;
    PFN_SetProcessDPIAware_ SetProcessDPIAware;
    PFN_SetProcessDpiAwarenessContext_ SetProcessDpiAwarenessContext;
    PFN_CreateFontW_ CreateFontW;
    PFN_DeleteObject_ DeleteObject;
    PFN_SelectObject_ SelectObject;
    PFN_SetBkMode_ SetBkMode;
    PFN_SetTextColor_ SetTextColor;
    PFN_CreateSolidBrush_ CreateSolidBrush;
} UiApi_;

static UiApi_ g_ui;

#define LOAD_USER_(field, name) do { g_ui.field = (PFN_##field##_)GetProcAddress(g_ui.user32, name); if (!g_ui.field) return 0; } while (0)
#define LOAD_GDI_(field, name)  do { g_ui.field = (PFN_##field##_)GetProcAddress(g_ui.gdi32, name); if (!g_ui.field) return 0; } while (0)

static int load_ui_api_(void) {
    /*
     * 第一步只通过 KERNEL32 动态打开 GUI DLL。这样 PE 文件自己的静态 Import Table 不需要 USER32/GDI32，
     * 也就不会改变 Pre-Loader 已经验证过的最早期装载边界。后面的每个函数指针只有在这里拿到以后才能使用。
     */
    g_ui.user32 = LoadLibraryW((const WCHAR*)L"user32.dll");
    g_ui.gdi32 = LoadLibraryW((const WCHAR*)L"gdi32.dll");
    if (!g_ui.user32 || !g_ui.gdi32) return 0;

    LOAD_USER_(RegisterClassExW, "RegisterClassExW");
    LOAD_USER_(CreateWindowExW, "CreateWindowExW");
    LOAD_USER_(DefWindowProcW, "DefWindowProcW");
    LOAD_USER_(ShowWindow, "ShowWindow");
    LOAD_USER_(UpdateWindow, "UpdateWindow");
    LOAD_USER_(GetMessageW, "GetMessageW");
    LOAD_USER_(TranslateMessage, "TranslateMessage");
    LOAD_USER_(DispatchMessageW, "DispatchMessageW");
    LOAD_USER_(PostQuitMessage, "PostQuitMessage");
    LOAD_USER_(DestroyWindow, "DestroyWindow");
    LOAD_USER_(GetClientRect, "GetClientRect");
    LOAD_USER_(GetWindowRect, "GetWindowRect");
    LOAD_USER_(SetWindowPos, "SetWindowPos");
    LOAD_USER_(MoveWindow, "MoveWindow");
    LOAD_USER_(SendMessageW, "SendMessageW");
    LOAD_USER_(SetWindowTextW, "SetWindowTextW");
    LOAD_USER_(EnableWindow, "EnableWindow");
    LOAD_USER_(InvalidateRect, "InvalidateRect");
    LOAD_USER_(RedrawWindow, "RedrawWindow");
    LOAD_USER_(AdjustWindowRectEx, "AdjustWindowRectEx");
    LOAD_USER_(BeginPaint, "BeginPaint");
    LOAD_USER_(EndPaint, "EndPaint");
    LOAD_USER_(GetDC, "GetDC");
    LOAD_USER_(ReleaseDC, "ReleaseDC");
    LOAD_USER_(DrawTextW, "DrawTextW");
    LOAD_USER_(FillRect, "FillRect");
    LOAD_USER_(FrameRect, "FrameRect");
    LOAD_USER_(DrawFrameControl, "DrawFrameControl");
    LOAD_USER_(GetSysColorBrush, "GetSysColorBrush");
    LOAD_USER_(GetSysColor, "GetSysColor");
    LOAD_USER_(LoadCursorW, "LoadCursorW");
    /* 图标只从本 EXE 资源读取，不增加静态 USER32 依赖；和其余 GUI API 一样在这里动态解析。 */
    LOAD_USER_(LoadIconW, "LoadIconW");
    LOAD_USER_(SetCapture, "SetCapture");
    LOAD_USER_(ReleaseCapture, "ReleaseCapture");
    LOAD_USER_(GetCapture, "GetCapture");
    LOAD_USER_(SetWindowLongW, "SetWindowLongW");
    LOAD_USER_(GetWindowLongW, "GetWindowLongW");
    LOAD_USER_(CallWindowProcW, "CallWindowProcW");
    LOAD_USER_(GetDlgCtrlID, "GetDlgCtrlID");
    LOAD_USER_(SetFocus, "SetFocus");
    LOAD_USER_(MessageBoxW, "MessageBoxW");
    LOAD_USER_(GetCursorPos, "GetCursorPos");
    LOAD_USER_(MonitorFromPoint, "MonitorFromPoint");
    LOAD_USER_(GetMonitorInfoW, "GetMonitorInfoW");
    LOAD_USER_(SystemParametersInfoW, "SystemParametersInfoW");

    /* DPI API 在不同 Windows 版本不一定存在，所以只“可选加载”，缺失时走 96 DPI / SetProcessDPIAware 兜底。 */
    g_ui.GetDpiForWindow = (PFN_GetDpiForWindow_)GetProcAddress(g_ui.user32, "GetDpiForWindow");
    /*
     * AdjustWindowRectExForDpi 只在较新的 Windows 提供，所以和 GetDpiForWindow 一样按“可选导出”读取。
     * 找不到时仍可退回从 XP 起就存在的 AdjustWindowRectEx，保证旧系统不会因为 GUI 最小尺寸功能而启动失败。
     */
    g_ui.AdjustWindowRectExForDpi = (PFN_AdjustWindowRectExForDpi_)GetProcAddress(g_ui.user32, "AdjustWindowRectExForDpi");
    g_ui.SetProcessDPIAware = (PFN_SetProcessDPIAware_)GetProcAddress(g_ui.user32, "SetProcessDPIAware");
    g_ui.SetProcessDpiAwarenessContext = (PFN_SetProcessDpiAwarenessContext_)GetProcAddress(g_ui.user32, "SetProcessDpiAwarenessContext");

    LOAD_GDI_(CreateFontW, "CreateFontW");
    LOAD_GDI_(DeleteObject, "DeleteObject");
    LOAD_GDI_(SelectObject, "SelectObject");
    LOAD_GDI_(SetBkMode, "SetBkMode");
    LOAD_GDI_(SetTextColor, "SetTextColor");
    LOAD_GDI_(CreateSolidBrush, "CreateSolidBrush");
    return 1;
}

static void unload_ui_api_(void) {
    /*
     * GUI 退出时把我们主动 LoadLibraryW 得到的模块引用归还给系统。先释放 GDI32、再释放 USER32 没有业务差异，
     * 但把句柄清成 NULL 可以防止未来误用已经卸载的函数表。
     */
    if (g_ui.gdi32) FreeLibrary(g_ui.gdi32);
    if (g_ui.user32) FreeLibrary(g_ui.user32);
    g_ui.gdi32 = NULL_PTR;
    g_ui.user32 = NULL_PTR;
}

/* ---------- 全局 GUI 状态 ---------- */

static const WCHAR kMainClass_[] = L"CastleModLoader.MainWindow";
static const WCHAR kSettingsClass_[] = L"CastleModLoader.SettingsWindow";
static const WCHAR kIniEditorClass_[] = L"CastleModLoader.IniEditorWindow";
static const WCHAR kAboutClass_[] = L"CastleModLoader.AboutWindow";
static const WCHAR kWindowTitle_[] = L"《幽城幻剑录》Mod Loader";

static HINSTANCE g_instance;
static HWND g_main;
static HWND g_asi_list;
static HWND g_override_list;
static HWND g_about_button;
static HWND g_settings_button;
static HWND g_refresh_button;
static HWND g_launch_button;
static HFONT_ g_font;
static HFONT_ g_title_font;
static HFONT_ g_panel_font;

/*
 * dev2 不再把系统三维控件当作视觉主体，而是自己画一套很轻的“卡片 + 扁平按钮”主题。
 * 这些画刷在 GUI 生命周期内只创建一次，避免列表每重绘一行就反复 CreateSolidBrush/DeleteObject。
 * 颜色只负责外观，不参与任何 Mod 逻辑；高对比度/系统主题适配以后若要做，也只需要替换这一层资源。
 */
static HBRUSH_ g_brush_workspace;      /* 主工作区浅灰背景。 */
static HBRUSH_ g_brush_card;           /* 卡片、普通列表行和次级按钮的白色背景。 */
static HBRUSH_ g_brush_selected;       /* 列表选中行的淡蓝背景。 */
static HBRUSH_ g_brush_accent;         /* 主蓝色：启动按钮、勾选框、选中指示条。 */
static HBRUSH_ g_brush_accent_pressed; /* 主按钮按下时使用的更深蓝色。 */
static HBRUSH_ g_brush_border;         /* 卡片、按钮和复选框的浅灰边框。 */
static HBRUSH_ g_brush_surface_pressed;/* 次级按钮/设置选项按下时的浅灰反馈。 */
static HBRUSH_ g_brush_separator;      /* 列表行与底部操作区的极浅分隔线。 */
static HBRUSH_ g_brush_muted;          /* 拖动把手等需要“看得见但不抢眼”的中性灰。 */

static WNDPROC_ g_old_list_proc;
static UINT g_exit_code = 1u;
static int g_game_started;

/* 拖动状态只允许同时存在一个列表；鼠标 Capture 保证拖出控件以后仍能收到 MouseUp。 */
static HWND g_drag_list;
static LauncherModKind_ g_drag_kind;
static int g_drag_from = -1;
static int g_drag_target = -1;
static int g_drag_started;
static int g_drag_start_y;

/* 设置窗口使用独立的简单模态循环。 */
static HWND g_settings_window;
static HWND g_setting_modlog;
static HWND g_setting_gamelog;
static int g_settings_done;

/*
 * About 现在是独立模态窗口，不再依赖 MessageBox。链接按钮数量由 about.cpp 提供，
 * 但 GUI 用 ABOUT_MAX_LINKS 固定上限保存 HWND，避免为了一个很小的页面引入动态容器。
 */
static HWND g_about_window;
static HWND g_about_close;
static HWND g_about_links[ABOUT_MAX_LINKS];
static HFONT_ g_about_link_font;
static int g_about_done;

/*
 * INI 编辑器一次只允许打开一个。它是主窗口的模态子窗口，所以状态可以用一组固定全局变量保存，
 * 不需要为每个窗口动态分配复杂对象。路径最长沿用项目 2048 WCHAR 上限，避免 MAX_PATH 对便携长路径产生额外限制。
 */
static HWND g_ini_editor_window;
static HWND g_ini_editor_text;
static HWND g_ini_editor_save;
static HWND g_ini_editor_close;
static HMODULE g_ini_richedit_module;
static HFONT_ g_ini_editor_font;
static WCHAR g_ini_editor_path[CASTLE_PATH_CAP];
static WCHAR g_ini_editor_title[CASTLE_PATH_CAP];
static WCHAR g_ini_editor_status[256];
static int g_ini_editor_done;
static int g_ini_editor_dirty;
static int g_ini_editor_coloring;
static int g_ini_editor_encoding;
static int g_ini_editor_status_error;

/* list_proc_ 位于完整编辑器实现之前，所以先声明入口，避免 C 编译器产生隐式函数声明。 */
static void show_ini_editor_(UINT asi_index);
static void show_about_dialog_(void);

/*
 * 设置窗口里的两个日志行在 dev2 中改成 BS_OWNERDRAW 的整行可点击卡片。
 * OWNERDRAW 按钮不是系统 CheckBox 类型，所以不能依赖 BM_SETCHECK 自动维护状态；
 * 这里用两个明确的 int 保存“用户当前看到的勾选状态”，点击行时切换，按保存时再写回 INI。
 */
static int g_setting_modlog_value;
static int g_setting_gamelog_value;

/* ---------- DPI、字体、几何辅助 ---------- */

static int scale_(HWND hwnd, int logical) {
    /*
     * 所有布局尺寸都先用 96 DPI 下的“逻辑像素”书写。这里统一乘以当前窗口 DPI / 96，
     * 这样 125% DPI 的 100 DIP 会得到 125 个物理像素，调用方不用在每个控件位置重复算比例。
     */
    UINT dpi = 96u;
    if (g_ui.GetDpiForWindow && hwnd) {
        UINT got = g_ui.GetDpiForWindow(hwnd);
        if (got) dpi = got;
    }
    return (int)(((LONG)logical * (LONG)dpi + 48L) / 96L);
}

static UINT current_dpi_(HWND hwnd) {
    /* Windows 10 有 GetDpiForWindow 就以当前显示器为准；旧系统没有导出时按 96 DPI 兜底。 */
    if (g_ui.GetDpiForWindow && hwnd) {
        UINT dpi = g_ui.GetDpiForWindow(hwnd);
        if (dpi) return dpi;
    }
    return 96u;
}

/*
 * 计算“1024×640 DIP 客户区”对应的整个顶层窗口外框尺寸。
 *
 * 为什么不能直接把 ptMinTrackSize 写成 1024×640：
 *   ptMinTrackSize 约束的是“整个窗口”，其中包含左右可调整边框和上方标题栏；如果直接写 1024×640，
 *   真正留给客户区的高度会少掉标题栏，用户看到的 Mod 区域就不是要求的 640 DIP。
 *
 * 新系统优先使用 AdjustWindowRectExForDpi，让非客户区厚度跟随窗口当前 DPI；旧系统退回 AdjustWindowRectEx。
 * 这个函数只做几何换算，不改变窗口风格，也不会触碰任何游戏启动、注入或 Hook 逻辑。
 */
static int get_main_minimum_outer_size_(HWND hwnd, int* out_width, int* out_height) {
    RECT_ r;
    DWORD style, ex_style;
    int client_w, client_h;

    if (!hwnd || !out_width || !out_height) return 0;

    client_w = scale_(hwnd, 1024);
    client_h = scale_(hwnd, 640);
    r.left = 0;
    r.top = 0;
    r.right = client_w;
    r.bottom = client_h;

    style = (DWORD)g_ui.GetWindowLongW(hwnd, GWL_STYLE_);
    ex_style = (DWORD)g_ui.GetWindowLongW(hwnd, GWL_EXSTYLE_);

    if (g_ui.AdjustWindowRectExForDpi) {
        if (!g_ui.AdjustWindowRectExForDpi(&r, style, FALSE_, ex_style, current_dpi_(hwnd))) return 0;
    } else {
        if (!g_ui.AdjustWindowRectEx(&r, style, FALSE_, ex_style)) return 0;
    }

    *out_width = r.right - r.left;
    *out_height = r.bottom - r.top;
    return *out_width > 0 && *out_height > 0;
}

static void recreate_fonts_(HWND hwnd) {
    /*
     * DPI 变化后旧 HFONT 的物理字号已经不合适，所以必须删掉再建。三个字体分别服务普通文本、标题和面板标题，
     * 统一使用 Segoe UI，避免每个控件自己拿系统默认字体造成字号和基线不一致。
     */
    UINT dpi = current_dpi_(hwnd);
    int body_h = -(int)((10u * dpi + 36u) / 72u);
    int title_h = -(int)((17u * dpi + 36u) / 72u);
    int panel_h = -(int)((11u * dpi + 36u) / 72u);

    if (g_font) g_ui.DeleteObject((HGDIOBJ_)g_font);
    if (g_title_font) g_ui.DeleteObject((HGDIOBJ_)g_title_font);
    if (g_panel_font) g_ui.DeleteObject((HGDIOBJ_)g_panel_font);

    g_font = g_ui.CreateFontW(body_h, 0, 0, 0, FW_NORMAL_, 0u, 0u, 0u, DEFAULT_CHARSET_,
                              OUT_DEFAULT_PRECIS_, CLIP_DEFAULT_PRECIS_, CLEARTYPE_QUALITY_, DEFAULT_PITCH_, (const WCHAR*)L"Segoe UI");
    g_title_font = g_ui.CreateFontW(title_h, 0, 0, 0, FW_SEMIBOLD_, 0u, 0u, 0u, DEFAULT_CHARSET_,
                                    OUT_DEFAULT_PRECIS_, CLIP_DEFAULT_PRECIS_, CLEARTYPE_QUALITY_, DEFAULT_PITCH_, (const WCHAR*)L"Segoe UI");
    g_panel_font = g_ui.CreateFontW(panel_h, 0, 0, 0, FW_SEMIBOLD_, 0u, 0u, 0u, DEFAULT_CHARSET_,
                                    OUT_DEFAULT_PRECIS_, CLIP_DEFAULT_PRECIS_, CLEARTYPE_QUALITY_, DEFAULT_PITCH_, (const WCHAR*)L"Segoe UI");
}

static int create_theme_resources_(void) {
    /*
     * 所有颜色都集中在这里，避免“某个按钮自己一种灰、某个列表又另一种灰”的失控状态。
     * 这套配色刻意保持低饱和：主蓝色只用于真正需要注意的动作和状态，其余区域主要靠留白、边框和字号分层。
     */
    g_brush_workspace = g_ui.CreateSolidBrush(RGB_(246, 247, 249));
    g_brush_card = g_ui.CreateSolidBrush(RGB_(255, 255, 255));
    g_brush_selected = g_ui.CreateSolidBrush(RGB_(235, 244, 255));
    g_brush_accent = g_ui.CreateSolidBrush(RGB_(10, 115, 218));
    g_brush_accent_pressed = g_ui.CreateSolidBrush(RGB_(7, 91, 174));
    g_brush_border = g_ui.CreateSolidBrush(RGB_(218, 223, 229));
    g_brush_surface_pressed = g_ui.CreateSolidBrush(RGB_(239, 242, 246));
    g_brush_separator = g_ui.CreateSolidBrush(RGB_(235, 238, 242));
    g_brush_muted = g_ui.CreateSolidBrush(RGB_(154, 162, 173));

    /*
     * 任何一支画刷创建失败都视为 GUI 初始化失败。继续运行虽然“可能还能画”，但会让部分控件随机回退成黑块/空白，
     * 这比明确失败更难诊断，所以这里宁可 fail-closed。外层会销毁窗口并返回独立错误码。
     */
    return g_brush_workspace && g_brush_card && g_brush_selected && g_brush_accent &&
           g_brush_accent_pressed && g_brush_border && g_brush_surface_pressed && g_brush_separator && g_brush_muted;
}

static void destroy_theme_resources_(void) {
    /*
     * GDI 对象由创建它们的进程负责 DeleteObject。逐个释放后清空句柄，既避免泄漏，也避免未来退出路径重复释放。
     */
    if (g_brush_workspace) g_ui.DeleteObject((HGDIOBJ_)g_brush_workspace);
    if (g_brush_card) g_ui.DeleteObject((HGDIOBJ_)g_brush_card);
    if (g_brush_selected) g_ui.DeleteObject((HGDIOBJ_)g_brush_selected);
    if (g_brush_accent) g_ui.DeleteObject((HGDIOBJ_)g_brush_accent);
    if (g_brush_accent_pressed) g_ui.DeleteObject((HGDIOBJ_)g_brush_accent_pressed);
    if (g_brush_border) g_ui.DeleteObject((HGDIOBJ_)g_brush_border);
    if (g_brush_surface_pressed) g_ui.DeleteObject((HGDIOBJ_)g_brush_surface_pressed);
    if (g_brush_separator) g_ui.DeleteObject((HGDIOBJ_)g_brush_separator);
    if (g_brush_muted) g_ui.DeleteObject((HGDIOBJ_)g_brush_muted);

    g_brush_workspace = NULL_PTR;
    g_brush_card = NULL_PTR;
    g_brush_selected = NULL_PTR;
    g_brush_accent = NULL_PTR;
    g_brush_accent_pressed = NULL_PTR;
    g_brush_border = NULL_PTR;
    g_brush_surface_pressed = NULL_PTR;
    g_brush_separator = NULL_PTR;
    g_brush_muted = NULL_PTR;
}

static void set_font_(HWND hwnd, HFONT_ font) {
    if (hwnd && font) g_ui.SendMessageW(hwnd, WM_SETFONT_, (WPARAM_)font, 1);
}

static void center_window_on_cursor_monitor_(HWND hwnd, int width, int height) {
    /*
     * 先取鼠标位置，再找离鼠标最近的显示器；用户从哪个屏幕启动 Loader，窗口就出现在那个工作区中央。
     * 使用 rcWork 而不是 rcMonitor，是为了避开任务栏占用区域。最后再 SetWindowPos，一次完成位置和尺寸。
     */
    POINT_ cursor;
    MONITORINFO_ mi;
    RECT_ work;
    HMONITOR_ monitor = NULL_PTR;

    if (g_ui.GetCursorPos && g_ui.GetCursorPos(&cursor) && g_ui.MonitorFromPoint)
        monitor = g_ui.MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST_);

    mi.cbSize = (DWORD)sizeof(mi);
    if (monitor && g_ui.GetMonitorInfoW && g_ui.GetMonitorInfoW(monitor, &mi)) {
        work = mi.rcWork;
    } else if (!g_ui.SystemParametersInfoW(SPI_GETWORKAREA_, 0u, &work, 0u)) {
        work.left = 0; work.top = 0; work.right = 1280; work.bottom = 720;
    }

    g_ui.SetWindowPos(hwnd, NULL_PTR,
                      work.left + ((work.right - work.left) - width) / 2,
                      work.top + ((work.bottom - work.top) - height) / 2,
                      width, height, SWP_NOZORDER_ | SWP_NOACTIVATE_);
}

static void center_window_over_parent_(HWND child, HWND parent, int width, int height) {
    /* 设置窗口属于主窗口的临时附属界面，因此优先按父窗口矩形居中，而不是跳到另一个显示器。 */
    RECT_ pr;
    if (parent && g_ui.GetWindowRect(parent, &pr)) {
        g_ui.SetWindowPos(child, NULL_PTR,
                          pr.left + ((pr.right - pr.left) - width) / 2,
                          pr.top + ((pr.bottom - pr.top) - height) / 2,
                          width, height, SWP_NOZORDER_ | SWP_NOACTIVATE_);
    } else {
        center_window_on_cursor_monitor_(child, width, height);
    }
}

/* ---------- 字符串/状态工具 ---------- */

static void u32_to_w_(UINT value, WCHAR* out, UINT cap) {
    WCHAR temp[16];
    UINT n = 0u, i;
    if (!out || cap == 0u) return;
    do {
        temp[n++] = (WCHAR)('0' + (value % 10u));
        value /= 10u;
    } while (value && n < 15u);
    if (n + 1u > cap) { out[0] = 0; return; }
    for (i = 0u; i < n; ++i) out[i] = temp[n - 1u - i];
    out[n] = 0;
}

static int wcopy_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT i = 0u;
    if (!dst || !src || cap == 0u) return 0;
    while (src[i]) {
        if (i + 1u >= cap) return 0;
        dst[i] = src[i];
        ++i;
    }
    dst[i] = 0;
    return 1;
}

static int wappend_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT d = 0u, i = 0u;
    if (!dst || !src || cap == 0u) return 0;
    while (dst[d]) ++d;
    while (src[i]) {
        if (d + i + 1u >= cap) return 0;
        dst[d + i] = src[i];
        ++i;
    }
    dst[d + i] = 0;
    return 1;
}

static int path_join_(WCHAR* out, UINT cap, const WCHAR* left, const WCHAR* right) {
    /*
     * GUI 只在构造“mods\asi\插件.ini”和临时保存路径时需要拼路径。
     * 这里和 Core/配置层使用同一条规则：左边末尾没有斜杠才补一个反斜杠，然后再追加右边。
     * 每一步都检查容量，任何溢出风险都直接返回 0，而不是截断成一个看似存在、实际指错位置的路径。
     */
    UINT n;
    if (!out || !left || !right || cap == 0u) return 0;
    if (!wcopy_(out, cap, left)) return 0;
    n = 0u;
    while (out[n]) ++n;
    if (n && out[n - 1u] != (WCHAR)'\\' && out[n - 1u] != (WCHAR)'/') {
        if (!wappend_(out, cap, (const WCHAR*)L"\\")) return 0;
    }
    return wappend_(out, cap, right);
}

static void show_config_error_(HWND owner) {
    const WCHAR* detail = LauncherModConfig_GetLastErrorText();
    WCHAR msg[768];
    msg[0] = 0;
    wcopy_(msg, 768u, (const WCHAR*)L"无法保存或读取 Mod 配置。\n\n");
    wappend_(msg, 768u, detail && detail[0] ? detail : (const WCHAR*)L"没有更多错误信息。");
    g_ui.MessageBoxW(owner, msg, kWindowTitle_, MB_ICONERROR_ | MB_OK_);
}

/* ---------- ListBox 数据刷新 ---------- */

static HWND list_for_kind_(LauncherModKind_ kind) {
    return kind == LAUNCHER_MOD_ASI ? g_asi_list : g_override_list;
}

static LauncherModKind_ kind_for_list_(HWND hwnd) {
    return hwnd == g_asi_list ? LAUNCHER_MOD_ASI : LAUNCHER_MOD_OVERRIDE;
}

static void refresh_list_(LauncherModKind_ kind, int select) {
    /*
     * ListBox 本身只负责画“有多少行”，真正的名称/启用/缺失状态始终从 LauncherModConfig 模型读取。
     * 这里先清空旧行，再按模型数量塞入占位字符串，Owner Draw 时再用 itemID 回查真实数据。
     */
    HWND list = list_for_kind_(kind);
    UINT count = LauncherModConfig_GetCount(kind);
    UINT i;
    if (!list) return;
    g_ui.SendMessageW(list, LB_RESETCONTENT_, 0, 0);
    for (i = 0u; i < count; ++i) {
        /*
         * LBS_HASSTRINGS 要求 LB_ADDSTRING 收到一个字符串。真正绘制不使用这份文本，而是按 itemID
         * 直接读取 LauncherModConfig 模型；传空字符串可以避免 ListBox 再复制一份 Mod 名称。
         */
        g_ui.SendMessageW(list, LB_ADDSTRING_, 0, (LPARAM_)(const WCHAR*)L"");
    }
    if (count) {
        if (select < 0) select = 0;
        if ((UINT)select >= count) select = (int)count - 1;
        g_ui.SendMessageW(list, LB_SETCURSEL_, (WPARAM_)select, 0);
        /* 有条目时显示真正 ListBox；行内容继续由 Owner Draw 绘制。 */
        g_ui.ShowWindow(list, SW_SHOW_);
    } else {
        /*
         * 空列表直接隐藏子窗口，让父窗口能在同一张卡片里画“暂无 Mod”的空状态。
         * 如果 ListBox 仍显示，它自己的白色客户区会盖住父窗口文字，于是右侧就会重新变成 dev1 截图里的整块空白。
         */
        g_ui.ShowWindow(list, SW_HIDE_);
    }
    g_ui.InvalidateRect(list, NULL_PTR, TRUE_);
}

static void refresh_all_lists_(void) {
    int asi_sel = (int)g_ui.SendMessageW(g_asi_list, LB_GETCURSEL_, 0, 0);
    int ov_sel = (int)g_ui.SendMessageW(g_override_list, LB_GETCURSEL_, 0, 0);
    refresh_list_(LAUNCHER_MOD_ASI, asi_sel == LB_ERR_ ? 0 : asi_sel);
    refresh_list_(LAUNCHER_MOD_OVERRIDE, ov_sel == LB_ERR_ ? 0 : ov_sel);
    g_ui.InvalidateRect(g_main, NULL_PTR, TRUE_);
}

/* ---------- Owner Draw：列表行 ---------- */

static void draw_checkbox_(HDC_ dc, HWND owner, RECT_ box, int checked, int enabled) {
    /*
     * 系统 DrawFrameControl 的复选框会带出传统 Win32 立体边缘，是 dev1 截图里最明显的“老式控件感”来源之一。
     * dev2 只画一个 18 DIP 的平面方框：未选中是白底细边框，选中是主蓝底白色勾；不可用时改为中性灰。
     * 这里不改变“能不能点击”的业务规则，只改变最终像素长什么样。
     */
    HGDIOBJ_ old_font;
    HBRUSH_ fill = checked ? (enabled ? g_brush_accent : g_brush_border) : g_brush_card;

    g_ui.FillRect(dc, &box, fill);
    if (!checked) g_ui.FrameRect(dc, &box, g_brush_border);

    if (checked) {
        g_ui.SetBkMode(dc, TRANSPARENT_);
        g_ui.SetTextColor(dc, RGB_(255, 255, 255));
        old_font = g_ui.SelectObject(dc, (HGDIOBJ_)g_panel_font);
        g_ui.DrawTextW(dc, (const WCHAR*)L"✓", -1, &box,
                       DT_CENTER_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
        g_ui.SelectObject(dc, old_font);
    }

    /* owner 目前只用于 DPI 语义说明，显式引用避免 /W4 /WX 把它当作未使用参数。 */
    (void)owner;
}

typedef struct ModRowLayout_ {
    /*
     * 同一份行内矩形既给 Owner Draw 使用，也给鼠标命中测试使用。
     * dev6 新增“编辑”后尤其不能把按钮绘制位置和点击位置拆成两套数字，否则 DPI 缩放后最容易出现错位。
     */
    RECT_ checkbox;
    RECT_ drag_grip;
    RECT_ name;
    RECT_ status;
    RECT_ edit_button;
    RECT_ remove_button;
    int has_status;
    int has_edit;
    int has_remove;
} ModRowLayout_;

static int calculate_mod_row_layout_(HWND hwnd, const RECT_* row, LauncherModKind_ kind,
                                     const LauncherModItem_* item, ModRowLayout_* out) {
    int center_y;
    int check_size;
    int grip_w;
    int right_edge;

    if (!hwnd || !row || !item || !out) return 0;
    center_y = row->top + (row->bottom - row->top) / 2;
    check_size = scale_(hwnd, 18);
    grip_w = scale_(hwnd, 22);

    out->checkbox.left = row->left + scale_(hwnd, 14);
    out->checkbox.top = center_y - check_size / 2;
    out->checkbox.right = out->checkbox.left + check_size;
    out->checkbox.bottom = out->checkbox.top + check_size;

    /* 复选框之后固定留一个 22 DIP 的三横线把手区；名称区仍然也能直接拖动。 */
    out->drag_grip.left = out->checkbox.right + scale_(hwnd, 6);
    out->drag_grip.right = out->drag_grip.left + grip_w;
    out->drag_grip.top = row->top;
    out->drag_grip.bottom = row->bottom;

    out->name = *row;
    out->name.left = out->drag_grip.right + scale_(hwnd, 6);
    out->name.right = row->right - scale_(hwnd, 14);

    out->status = *row;
    out->status.left = out->status.right = 0;
    out->edit_button = *row;
    out->edit_button.left = out->edit_button.right = 0;
    out->remove_button = *row;
    out->remove_button.left = out->remove_button.right = 0;
    out->has_status = 0;
    out->has_edit = 0;
    out->has_remove = 0;

    /*
     * 右侧动作从最右边往左排。危险动作“移除”永远最靠右；正常 ASI 如果存在同名 .ini，
     * “编辑”放在它左边（没有移除时自然就是最右按钮）。这样用户的视线和点击习惯稳定，不会因为状态变化乱跳。
     */
    right_edge = row->right - scale_(hwnd, 12);
    if (!item->present) {
        out->has_remove = 1;
        out->remove_button.right = right_edge;
        out->remove_button.left = out->remove_button.right - scale_(hwnd, 48);
        out->remove_button.top = row->top + scale_(hwnd, 7);
        out->remove_button.bottom = row->bottom - scale_(hwnd, 7);
        right_edge = out->remove_button.left - scale_(hwnd, 8);
    }

    /* 只给“当前主 ASI 文件存在 + 磁盘上确有同名 INI”的代码 Mod 显示编辑按钮。Overrides 不猜配置文件。 */
    if (kind == LAUNCHER_MOD_ASI && item->present && item->has_ini) {
        out->has_edit = 1;
        out->edit_button.right = right_edge;
        out->edit_button.left = out->edit_button.right - scale_(hwnd, 48);
        out->edit_button.top = row->top + scale_(hwnd, 7);
        out->edit_button.bottom = row->bottom - scale_(hwnd, 7);
        right_edge = out->edit_button.left - scale_(hwnd, 8);
    }

    if (!item->present) {
        out->has_status = 1;
        out->status.right = right_edge;
        out->status.left = out->status.right - scale_(hwnd, 68);
        out->name.right = out->status.left - scale_(hwnd, 8);
    } else if (!item->usable) {
        out->has_status = 1;
        out->status.right = right_edge;
        out->status.left = out->status.right - scale_(hwnd, 58);
        out->name.right = out->status.left - scale_(hwnd, 8);
    } else {
        out->name.right = right_edge;
    }
    return 1;
}

static int point_in_rect_(const RECT_* r, int x, int y) {
    /* Win32 RECT 的 right/bottom 按惯例是不包含的边界，命中测试也保持同一语义。 */
    return r && x >= r->left && x < r->right && y >= r->top && y < r->bottom;
}

static void draw_drag_grip_(HDC_ dc, HWND hwnd, const RECT_* grip, int active) {
    RECT_ line;
    int cx, cy, half_w, thickness, gap;

    if (!dc || !grip) return;
    cx = grip->left + (grip->right - grip->left) / 2;
    cy = grip->top + (grip->bottom - grip->top) / 2;
    half_w = scale_(hwnd, 5);
    thickness = scale_(hwnd, 1);
    if (thickness < 1) thickness = 1;
    gap = scale_(hwnd, 4);

    /* 三条短横线比依赖某个 Unicode“拖动”字形更稳定，Segoe UI 缺字时也不会变成方框。 */
    line.left = cx - half_w;
    line.right = cx + half_w;
    line.top = cy - gap - thickness / 2;
    line.bottom = line.top + thickness;
    g_ui.FillRect(dc, &line, active ? g_brush_accent : g_brush_muted);
    line.top = cy - thickness / 2;
    line.bottom = line.top + thickness;
    g_ui.FillRect(dc, &line, active ? g_brush_accent : g_brush_muted);
    line.top = cy + gap - thickness / 2;
    line.bottom = line.top + thickness;
    g_ui.FillRect(dc, &line, active ? g_brush_accent : g_brush_muted);
}

static void draw_inline_remove_button_(HDC_ dc, const RECT_* button) {
    HGDIOBJ_ old_font;
    if (!dc || !button) return;

    /*
     * “移除”只清理已经缺失的配置引用，因此使用轻量白底小按钮，不和蓝色“启动游戏”抢主动作层级。
     * 文字用暖色提醒这是删除类操作，但不使用高饱和红底，避免列表里出现过强警告块。
     */
    g_ui.FillRect(dc, button, g_brush_card);
    g_ui.FrameRect(dc, button, g_brush_border);
    g_ui.SetBkMode(dc, TRANSPARENT_);
    g_ui.SetTextColor(dc, RGB_(168, 72, 38));
    old_font = g_ui.SelectObject(dc, (HGDIOBJ_)g_font);
    g_ui.DrawTextW(dc, (const WCHAR*)L"移除", -1, (RECT_*)button,
                   DT_CENTER_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    g_ui.SelectObject(dc, old_font);
}

static void draw_inline_edit_button_(HDC_ dc, const RECT_* button) {
    HGDIOBJ_ old_font;
    if (!dc || !button) return;

    /*
     * “编辑”是普通配置动作，所以沿用白底细边框，但文字使用主蓝色，与暖色“移除”形成明确语义区分。
     * 按钮故意保持 48 DIP 宽，不再额外放齿轮图标，避免窄行里出现难以识别的小图形。
     */
    g_ui.FillRect(dc, button, g_brush_card);
    g_ui.FrameRect(dc, button, g_brush_border);
    g_ui.SetBkMode(dc, TRANSPARENT_);
    g_ui.SetTextColor(dc, RGB_(42, 102, 170));
    old_font = g_ui.SelectObject(dc, (HGDIOBJ_)g_font);
    g_ui.DrawTextW(dc, (const WCHAR*)L"编辑", -1, (RECT_*)button,
                   DT_CENTER_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    g_ui.SelectObject(dc, old_font);
}

static void draw_mod_row_(const DRAWITEMSTRUCT_* dis) {
    /*
     * dev3 一行的视觉顺序固定为“选中指示 → 复选框 → 拖动把手 → 名称 → 状态 → 可选编辑/移除按钮 → 分隔线”。
     * 把拖动把手直接画出来，是为了不再依赖用户猜“整行其实可以拖”；ASI 与 Overrides 两列使用完全相同的规则。
     */
    LauncherModKind_ kind;
    const LauncherModItem_* item;
    ModRowLayout_ layout;
    RECT_ row, line, accent_bar;
    HGDIOBJ_ old_font;
    int selected;

    if (!dis || dis->itemID == 0xFFFFFFFFu) return;
    kind = dis->hwndItem == g_asi_list ? LAUNCHER_MOD_ASI : LAUNCHER_MOD_OVERRIDE;
    item = LauncherModConfig_GetItem(kind, dis->itemID);
    if (!item) return;

    row = dis->rcItem;
    selected = (dis->itemState & ODS_SELECTED_) != 0u;
    g_ui.FillRect(dis->hDC, &row, selected ? g_brush_selected : g_brush_card);

    if (selected) {
        accent_bar = row;
        accent_bar.right = accent_bar.left + scale_(dis->hwndItem, 3);
        g_ui.FillRect(dis->hDC, &accent_bar, g_brush_accent);
    }

    if (!calculate_mod_row_layout_(dis->hwndItem, &row, kind, item, &layout)) return;
    draw_checkbox_(dis->hDC, dis->hwndItem, layout.checkbox, item->enabled, item->usable);
    draw_drag_grip_(dis->hDC, dis->hwndItem, &layout.drag_grip,
                    g_drag_started && dis->hwndItem == g_drag_list && (int)dis->itemID == g_drag_from);

    old_font = g_ui.SelectObject(dis->hDC, (HGDIOBJ_)g_font);
    g_ui.SetBkMode(dis->hDC, TRANSPARENT_);
    g_ui.SetTextColor(dis->hDC, item->usable ? RGB_(35, 39, 44) : RGB_(145, 151, 160));
    g_ui.DrawTextW(dis->hDC, item->name, -1, &layout.name,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    if (!item->present && layout.has_status) {
        g_ui.SetTextColor(dis->hDC, RGB_(181, 71, 8));
        g_ui.DrawTextW(dis->hDC, (const WCHAR*)L"文件缺失", -1, &layout.status,
                       DT_RIGHT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    } else if (!item->usable && layout.has_status) {
        g_ui.SetTextColor(dis->hDC, RGB_(122, 132, 145));
        g_ui.DrawTextW(dis->hDC, (const WCHAR*)L"空目录", -1, &layout.status,
                       DT_RIGHT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    }
    g_ui.SelectObject(dis->hDC, old_font);

    if (layout.has_edit) draw_inline_edit_button_(dis->hDC, &layout.edit_button);
    if (layout.has_remove) draw_inline_remove_button_(dis->hDC, &layout.remove_button);

    line = row;
    line.top = line.bottom - scale_(dis->hwndItem, 1);
    g_ui.FillRect(dis->hDC, &line, g_brush_separator);

    /* 拖动插入线仍按“向上=目标顶部、向下=目标底部”绘制，与配置层最终 Move 落点严格一致。 */
    if (g_drag_started && dis->hwndItem == g_drag_list && (int)dis->itemID == g_drag_target) {
        line = row;
        if (g_drag_target > g_drag_from) line.top = line.bottom - scale_(dis->hwndItem, 3);
        else line.bottom = line.top + scale_(dis->hwndItem, 3);
        g_ui.FillRect(dis->hDC, &line, g_brush_accent);
    }
}

static void draw_flat_button_(const DRAWITEMSTRUCT_* dis, const WCHAR* text, int primary) {
    /*
     * 全部可点击动作统一由这一处画：主动作使用蓝底白字，次级动作使用白底深字 + 细灰边框。
     * ODS_SELECTED 代表鼠标/键盘正在按下；ODS_FOCUS 只加强边框，不额外画系统虚线焦点框，避免风格重新混回经典控件。
     */
    RECT_ r;
    HBRUSH_ fill;
    HBRUSH_ border;
    DWORD text_color;
    HGDIOBJ_ old_font;
    int disabled;

    if (!dis || !text) return;
    r = dis->rcItem;
    disabled = (dis->itemState & ODS_DISABLED_) != 0u;

    if (primary) {
        if (disabled) fill = g_brush_border;
        else if (dis->itemState & ODS_SELECTED_) fill = g_brush_accent_pressed;
        else fill = g_brush_accent;
        border = fill;
        text_color = disabled ? RGB_(245, 246, 248) : RGB_(255, 255, 255);
    } else {
        fill = (dis->itemState & ODS_SELECTED_) ? g_brush_surface_pressed : g_brush_card;
        border = (dis->itemState & ODS_FOCUS_) ? g_brush_accent : g_brush_border;
        text_color = disabled ? RGB_(151, 157, 166) : RGB_(43, 48, 55);
    }

    g_ui.FillRect(dis->hDC, &r, fill);
    g_ui.FrameRect(dis->hDC, &r, border);
    g_ui.SetBkMode(dis->hDC, TRANSPARENT_);
    g_ui.SetTextColor(dis->hDC, text_color);
    old_font = g_ui.SelectObject(dis->hDC, (HGDIOBJ_)(primary ? g_panel_font : g_font));
    g_ui.DrawTextW(dis->hDC, text, -1, &r, DT_CENTER_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    g_ui.SelectObject(dis->hDC, old_font);
}

static void draw_setting_toggle_(const DRAWITEMSTRUCT_* dis, const WCHAR* title, const WCHAR* file_name, int checked) {
    /*
     * dev3 把设置项压成单行：左边复选框 + 设置名，右边只保留对应日志文件名。
     * 这比 dev2 的两行 66 DIP 大卡片节省约三分之一高度，未来继续增加设置项时不需要不断放大弹窗。
     */
    RECT_ r, box, title_rc, file_rc;
    HGDIOBJ_ old_font;

    if (!dis || !title || !file_name) return;
    r = dis->rcItem;
    g_ui.FillRect(dis->hDC, &r, (dis->itemState & ODS_SELECTED_) ? g_brush_surface_pressed : g_brush_card);
    g_ui.FrameRect(dis->hDC, &r, (dis->itemState & ODS_FOCUS_) ? g_brush_accent : g_brush_border);

    box.left = r.left + scale_(dis->hwndItem, 12);
    box.top = r.top + (r.bottom - r.top - scale_(dis->hwndItem, 18)) / 2;
    box.right = box.left + scale_(dis->hwndItem, 18);
    box.bottom = box.top + scale_(dis->hwndItem, 18);
    draw_checkbox_(dis->hDC, dis->hwndItem, box, checked, 1);

    title_rc = r;
    title_rc.left = box.right + scale_(dis->hwndItem, 10);
    title_rc.right = r.right - scale_(dis->hwndItem, 112);

    file_rc = r;
    file_rc.left = title_rc.right + scale_(dis->hwndItem, 8);
    file_rc.right = r.right - scale_(dis->hwndItem, 12);

    g_ui.SetBkMode(dis->hDC, TRANSPARENT_);
    old_font = g_ui.SelectObject(dis->hDC, (HGDIOBJ_)g_font);
    g_ui.SetTextColor(dis->hDC, RGB_(35, 39, 44));
    g_ui.DrawTextW(dis->hDC, title, -1, &title_rc,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);
    g_ui.SetTextColor(dis->hDC, RGB_(124, 132, 143));
    g_ui.DrawTextW(dis->hDC, file_name, -1, &file_rc,
                   DT_RIGHT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);
    g_ui.SelectObject(dis->hDC, old_font);
}

/* ---------- ListBox 子类：点击复选框 + 按住拖动排序 ---------- */

static int get_y_from_lparam_(LPARAM_ l) {
    return (int)(short)((((DWORD)l) >> 16) & 0xFFFFu);
}

static int get_x_from_lparam_(LPARAM_ l) {
    return (int)(short)(((DWORD)l) & 0xFFFFu);
}

static int list_item_from_point_(HWND list, int x, int y) {
    /* LB_ITEMFROMPOINT 把“命中的行号”和“是否在客户区外”打包进一个返回值，这里统一拆开并转成 -1/索引。 */
    DWORD packed = ((DWORD)(y & 0xFFFF) << 16) | (DWORD)(x & 0xFFFF);
    LRESULT_ r = g_ui.SendMessageW(list, LB_ITEMFROMPOINT_, 0, (LPARAM_)packed);
    int outside = (int)(((DWORD)r >> 16) & 0xFFFFu);
    int index = (int)((DWORD)r & 0xFFFFu);
    if (outside) return -1;
    return index;
}

static LRESULT_ CALLBACK list_proc_(HWND hwnd, UINT msg, WPARAM_ w, LPARAM_ l) {
    /*
     * 两个列表共用同一套鼠标规则：
     *   - 点复选框：切换启用状态并立即保存；
     *   - 点“编辑”：只有 ASI 存在同名 .ini 时打开带语法高亮和保存校验的内置编辑器；
     *   - 点“移除”：只清理磁盘已经缺失的 mods.ini 条目；
     *   - 按住把手或名称区域再纵向移动：进入拖动排序；
     *   - 单击其它区域但不移动：只改变当前选中行。
     */
    if (msg == WM_LBUTTONDOWN_) {
        int x = get_x_from_lparam_(l);
        int y = get_y_from_lparam_(l);
        int index = list_item_from_point_(hwnd, x, y);
        if (index >= 0) {
            RECT_ rc;
            ModRowLayout_ layout;
            LauncherModKind_ kind = kind_for_list_(hwnd);
            const LauncherModItem_* item = LauncherModConfig_GetItem(kind, (UINT)index);
            g_ui.SendMessageW(hwnd, LB_SETCURSEL_, (WPARAM_)index, 0);
            g_ui.SetFocus(hwnd);

            if (!item || g_ui.SendMessageW(hwnd, LB_GETITEMRECT_, (WPARAM_)index, (LPARAM_)&rc) == LB_ERR_) return 0;
            if (!calculate_mod_row_layout_(hwnd, &rc, kind, item, &layout)) return 0;

            /* 复选框命中区与真正画出来的方框完全一致，避免新增把手后仍沿用旧“左侧 38 DIP 全算复选框”的模糊范围。 */
            if (point_in_rect_(&layout.checkbox, x, y)) {
                if (!LauncherModConfig_SetEnabled(LauncherApp_GetModsRoot(), kind, (UINT)index, !item->enabled))
                    show_config_error_(g_main);
                refresh_list_(kind, index);
                return 0;
            }

            if (layout.has_edit && point_in_rect_(&layout.edit_button, x, y)) {
                /*
                 * 编辑动作只读取/保存该 ASI 身边的同名 INI，不改 mods.ini，也不影响插件启停和排序。
                 * show_ini_editor_ 自己还会重新构造并验证路径，UI 的 has_edit 只是“是否显示按钮”，不是安全边界。
                 */
                show_ini_editor_((UINT)index);
                return 0;
            }

            if (layout.has_remove && point_in_rect_(&layout.remove_button, x, y)) {
                /*
                 * 行内按钮只会出现在 present=0 的项目上。配置层仍会再次验证 present，且只修改 mods.ini，
                 * 因此这里不弹“是否删除文件”的确认框——根本没有任何磁盘文件会被删除。
                 */
                if (!LauncherModConfig_RemoveMissing(LauncherApp_GetModsRoot(), kind, (UINT)index)) {
                    show_config_error_(g_main);
                }
                refresh_list_(kind, index);
                g_ui.InvalidateRect(g_main, NULL_PTR, TRUE_);
                return 0;
            }

            /*
             * dev3 把拖动把手画出来作为明确提示，但不强迫用户只能精准抓把手：名称和状态空白区也可以按住拖。
             * 这样既容易发现，也保留 dev1/dev2 已存在的“整行拖动”快捷操作。
             */
            g_drag_list = hwnd;
            g_drag_kind = kind;
            g_drag_from = index;
            g_drag_target = index;
            g_drag_started = 0;
            g_drag_start_y = y;
            g_ui.SetCapture(hwnd);
            return 0;
        }
    } else if (msg == WM_MOUSEMOVE_ && g_ui.GetCapture() == hwnd && g_drag_list == hwnd && g_drag_from >= 0) {
        int x = get_x_from_lparam_(l);
        int y = get_y_from_lparam_(l);
        int index;
        if (!g_drag_started) {
            int delta = y - g_drag_start_y;
            if (delta < 0) delta = -delta;
            if (delta >= scale_(hwnd, 4)) {
                g_drag_started = 1;
                /* 第一次跨过阈值就立刻重画，把手会变蓝且目标行出现插入线，用户能马上知道拖动已生效。 */
                g_ui.InvalidateRect(hwnd, NULL_PTR, TRUE_);
            }
        }
        if (g_drag_started) {
            index = list_item_from_point_(hwnd, x, y);
            if (index < 0) {
                LONG count = (LONG)g_ui.SendMessageW(hwnd, LB_GETCOUNT_, 0, 0);
                if (count > 0) index = y < 0 ? 0 : (int)count - 1;
            }
            if (index >= 0 && index != g_drag_target) {
                g_drag_target = index;
                g_ui.InvalidateRect(hwnd, NULL_PTR, TRUE_);
            }
        }
        return 0;
    } else if (msg == WM_LBUTTONUP_ && g_ui.GetCapture() == hwnd && g_drag_list == hwnd) {
        int from = g_drag_from;
        int to = g_drag_target;
        int was_dragging = g_drag_started;
        LauncherModKind_ kind = g_drag_kind;

        g_ui.ReleaseCapture();
        g_drag_list = NULL_PTR;
        g_drag_from = -1;
        g_drag_target = -1;
        g_drag_started = 0;

        if (was_dragging && from >= 0 && to >= 0 && from != to) {
            if (!LauncherModConfig_Move(LauncherApp_GetModsRoot(), kind, (UINT)from, (UINT)to))
                show_config_error_(g_main);
            refresh_list_(kind, to);
        } else {
            g_ui.InvalidateRect(hwnd, NULL_PTR, TRUE_);
        }
        return 0;
    } else if (msg == WM_CAPTURECHANGED_) {
        g_drag_list = NULL_PTR;
        g_drag_from = -1;
        g_drag_target = -1;
        g_drag_started = 0;
        g_ui.InvalidateRect(hwnd, NULL_PTR, TRUE_);
    }
    return g_ui.CallWindowProcW(g_old_list_proc, hwnd, msg, w, l);
}

/* ---------- 主窗口绘制与布局 ---------- */

typedef struct MainLayout_ {
    /*
     * 同一组几何值同时被 MoveWindow 和 WM_PAINT 使用。
     * 如果布局和绘制各自“再算一遍但公式稍有不同”，边框、标题和真正的 ListBox 很容易错开 1～2 个 DIP。
     * 因此 dev2 把所有主窗口矩形统一收敛到这一份结构里。
     */
    RECT_ asi_card;
    RECT_ override_card;
    RECT_ asi_list;
    RECT_ override_list;
    RECT_ footer;
    RECT_ about_button;
    RECT_ settings_button;
    RECT_ refresh_button;
    RECT_ launch_button;
} MainLayout_;

static int calculate_main_layout_(HWND hwnd, MainLayout_* out) {
    RECT_ rc;
    int m, top, header_h, gap, footer_h, card_top, card_bottom, half, card_header_h;
    int about_w, about_h, settings_w, settings_h, header_button_gap;
    int refresh_w, refresh_h, launch_w, launch_h;

    if (!out || !g_ui.GetClientRect(hwnd, &rc)) return 0;

    /*
     * 下面所有数字都是 96 DPI 下的逻辑 DIP。scale_() 会在 125%/150% DPI 自动转成物理像素，
     * 所以这里表达的是“视觉间距关系”，不是某个显示器上的硬编码像素坐标。
     */
    m = scale_(hwnd, 24);
    top = scale_(hwnd, 18);
    /*
     * dev4 起顶部不再放重复的大标题，而是保留两行使用说明。
     * 72 DIP 足够放下两行正文、设置按钮和底部细分隔线，同时比 dev3 的品牌标题区更紧凑。
     */
    header_h = scale_(hwnd, 72);
    gap = scale_(hwnd, 16);
    footer_h = scale_(hwnd, 88);
    card_header_h = scale_(hwnd, 68);

    /*
     * “关于”和“设置”属于同一级的次要操作，所以使用完全相同的尺寸。
     * 两个按钮并排放在顶部右侧，关于在左、设置在右；这样不会改变用户已经熟悉的设置按钮靠右位置，
     * 也不会把低频的 About 放进底部“重新扫描/启动游戏”主操作区。
     */
    about_w = scale_(hwnd, 78);
    about_h = scale_(hwnd, 38);
    settings_w = scale_(hwnd, 78);
    settings_h = scale_(hwnd, 38);
    header_button_gap = scale_(hwnd, 10);
    refresh_w = scale_(hwnd, 108);
    refresh_h = scale_(hwnd, 40);
    launch_w = scale_(hwnd, 176);
    launch_h = scale_(hwnd, 52);

    card_top = top + header_h;
    card_bottom = rc.bottom - footer_h - scale_(hwnd, 16);
    half = (rc.right - m * 2 - gap) / 2;

    out->asi_card.left = m;
    out->asi_card.top = card_top;
    out->asi_card.right = m + half;
    out->asi_card.bottom = card_bottom;

    out->override_card.left = out->asi_card.right + gap;
    out->override_card.top = card_top;
    out->override_card.right = rc.right - m;
    out->override_card.bottom = card_bottom;

    /* ListBox 只占卡片标题区下面的正文区域，并向内缩 1 DIP，让父窗口画出的细边框始终可见。 */
    out->asi_list.left = out->asi_card.left + scale_(hwnd, 1);
    out->asi_list.top = out->asi_card.top + card_header_h;
    out->asi_list.right = out->asi_card.right - scale_(hwnd, 1);
    out->asi_list.bottom = out->asi_card.bottom - scale_(hwnd, 1);

    out->override_list.left = out->override_card.left + scale_(hwnd, 1);
    out->override_list.top = out->override_card.top + card_header_h;
    out->override_list.right = out->override_card.right - scale_(hwnd, 1);
    out->override_list.bottom = out->override_card.bottom - scale_(hwnd, 1);

    out->footer.left = 0;
    out->footer.top = rc.bottom - footer_h;
    out->footer.right = rc.right;
    out->footer.bottom = rc.bottom;

    out->settings_button.left = rc.right - m - settings_w;
    out->settings_button.top = top;
    out->settings_button.right = rc.right - m;
    out->settings_button.bottom = top + settings_h;

    out->about_button.right = out->settings_button.left - header_button_gap;
    out->about_button.left = out->about_button.right - about_w;
    out->about_button.top = top;
    out->about_button.bottom = top + about_h;

    out->refresh_button.left = m;
    out->refresh_button.top = out->footer.top + (footer_h - refresh_h) / 2;
    out->refresh_button.right = m + refresh_w;
    out->refresh_button.bottom = out->refresh_button.top + refresh_h;

    out->launch_button.right = rc.right - m;
    out->launch_button.left = out->launch_button.right - launch_w;
    out->launch_button.top = out->footer.top + (footer_h - launch_h) / 2;
    out->launch_button.bottom = out->launch_button.top + launch_h;
    return 1;
}

static void move_to_rect_(HWND hwnd, const RECT_* r) {
    /* 把 RECT 的四条边转换成 MoveWindow 需要的 x/y/width/height，避免每个控件重复写减法。 */
    if (!hwnd || !r) return;
    g_ui.MoveWindow(hwnd, r->left, r->top, r->right - r->left, r->bottom - r->top, TRUE_);
}

static void move_to_rect_without_repaint_(HWND hwnd, const RECT_* r) {
    /*
     * 主窗口连续缩放时，不让每个子控件 MoveWindow 后立刻单独重画。
     * 如果五个控件边移动边各画一次，父窗口与子窗口会短暂处在不同几何状态，旧 ListBox 区域就可能留下“条纹/重影”。
     * 因此主布局先只完成全部位置修改，最后由 redraw_main_now_() 一次同步重画整个父子窗口树。
     */
    if (!hwnd || !r) return;
    g_ui.MoveWindow(hwnd, r->left, r->top, r->right - r->left, r->bottom - r->top, FALSE_);
}

static void layout_main_(void) {
    MainLayout_ layout;
    if (!g_main) return;
    if (!calculate_main_layout_(g_main, &layout)) return;
    move_to_rect_without_repaint_(g_about_button, &layout.about_button);
    move_to_rect_without_repaint_(g_settings_button, &layout.settings_button);
    move_to_rect_without_repaint_(g_asi_list, &layout.asi_list);
    move_to_rect_without_repaint_(g_override_list, &layout.override_list);
    move_to_rect_without_repaint_(g_refresh_button, &layout.refresh_button);
    move_to_rect_without_repaint_(g_launch_button, &layout.launch_button);
}

static void redraw_main_now_(HWND hwnd) {
    /*
     * RedrawWindow 的关键点是 RDW_ALLCHILDREN：不仅父窗口的卡片/背景要重画，两个 ListBox 和三个按钮也必须在同一轮处理。
     * RDW_ERASE 会先清除移动后暴露出来的旧位置，RDW_UPDATENOW 则要求 Windows 立即完成 WM_ERASEBKGND/WM_PAINT，
     * 不把这次刷新拖到消息队列空闲或用户松开鼠标之后。
     */
    if (!hwnd) return;
    g_ui.RedrawWindow(hwnd, NULL_PTR, NULL_PTR,
                      RDW_INVALIDATE_ | RDW_ERASE_ | RDW_ALLCHILDREN_ | RDW_UPDATENOW_);
}

static void draw_card_(HDC_ dc, const RECT_* card) {
    /* 卡片只用纯白背景 + 1 DIP 浅灰边框，不做阴影，保证 GDI 实现简单、清晰，并避免模糊边缘。 */
    if (!dc || !card) return;
    g_ui.FillRect(dc, card, g_brush_card);
    g_ui.FrameRect(dc, card, g_brush_border);
}

static void draw_empty_state_(HDC_ dc, HWND hwnd, const RECT_* list_rect, const WCHAR* title, const WCHAR* detail) {
    /*
     * 空状态放在列表正文区域中央附近：第一行说明“这里现在没有内容”，第二行告诉用户下一步该把文件放在哪里。
     * 这比单纯留白更重要，因为 Overrides=0 是正常状态，不应该让用户误以为界面没有加载完整。
     */
    RECT_ title_rc, detail_rc;
    HGDIOBJ_ old_font;
    int center_y;

    if (!dc || !list_rect || !title || !detail) return;
    center_y = list_rect->top + (list_rect->bottom - list_rect->top) / 2;

    title_rc.left = list_rect->left + scale_(hwnd, 22);
    title_rc.right = list_rect->right - scale_(hwnd, 22);
    title_rc.top = center_y - scale_(hwnd, 27);
    title_rc.bottom = center_y - scale_(hwnd, 1);
    detail_rc = title_rc;
    detail_rc.top = center_y + scale_(hwnd, 2);
    detail_rc.bottom = center_y + scale_(hwnd, 30);

    g_ui.SetBkMode(dc, TRANSPARENT_);
    g_ui.SetTextColor(dc, RGB_(96, 105, 116));
    old_font = g_ui.SelectObject(dc, (HGDIOBJ_)g_panel_font);
    g_ui.DrawTextW(dc, title, -1, &title_rc,
                   DT_CENTER_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    g_ui.SetTextColor(dc, RGB_(139, 147, 157));
    g_ui.SelectObject(dc, (HGDIOBJ_)g_font);
    g_ui.DrawTextW(dc, detail, -1, &detail_rc,
                   DT_CENTER_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);
    g_ui.SelectObject(dc, old_font);
}

static void paint_main_(HWND hwnd) {
    /*
     * dev2 主窗口采用三层视觉结构：浅灰工作区 → 两张白色 Mod 卡片 → 底部白色操作栏。
     * 所有说明文字由父窗口直接画，不额外创建 Static，因此不会出现传统 Static 的灰底，也减少 DPI 布局分叉。
     */
    PAINTSTRUCT_ ps;
    RECT_ rc, text, line, count_rc;
    MainLayout_ layout;
    HDC_ dc = g_ui.BeginPaint(hwnd, &ps);
    HGDIOBJ_ old;
    WCHAR num_a[16], num_o[16], status[256];

    if (!dc) return;
    if (!g_ui.GetClientRect(hwnd, &rc) || !calculate_main_layout_(hwnd, &layout)) {
        g_ui.EndPaint(hwnd, &ps);
        return;
    }

    g_ui.FillRect(dc, &rc, g_brush_workspace);
    g_ui.SetBkMode(dc, TRANSPARENT_);

    /*
     * dev4 起顶部只承担“怎么用”的说明职责。程序名已经在 Windows 标题栏里显示，客户区再画一次只会浪费纵向空间。
     * 两行说明分别讲“操作方式”和“两类 Mod 的顺序语义”，让第一次打开的人不用猜拖动方向代表什么。
     */
    old = g_ui.SelectObject(dc, (HGDIOBJ_)g_font);
    g_ui.SetTextColor(dc, RGB_(91, 101, 113));
    text.left = scale_(hwnd, 24);
    /*
     * 顶部现在有“关于 + 设置”两个按钮，所以说明文字的右边界必须停在最左侧的“关于”按钮之前。
     * 如果仍以设置按钮为边界，较长说明会被新按钮盖住。
     */
    text.right = layout.about_button.left - scale_(hwnd, 18);
    text.top = scale_(hwnd, 14);
    text.bottom = scale_(hwnd, 40);
    g_ui.DrawTextW(dc, (const WCHAR*)L"勾选复选框启用或禁用 Mod；按住行内 ≡ 把手上下拖动即可排序，所有修改会立即保存。", -1, &text,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    g_ui.SetTextColor(dc, RGB_(116, 125, 136));
    text.top = scale_(hwnd, 38);
    text.bottom = scale_(hwnd, 64);
    g_ui.DrawTextW(dc, (const WCHAR*)L"ASI 从上到下加载；Overrides 越靠下优先级越高；有同名 .ini 的 ASI 可直接编辑，缺失旧条目可移除。", -1, &text,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    /*
     * 一条很浅的横线把“使用说明”与“Mod 工作区”分开。这里不使用粗边框或额外标题，避免重新堆出一个新的大头部。
     */
    line.left = scale_(hwnd, 24);
    line.right = rc.right - scale_(hwnd, 24);
    line.top = layout.asi_card.top - scale_(hwnd, 10);
    line.bottom = line.top + scale_(hwnd, 1);
    g_ui.FillRect(dc, &line, g_brush_border);

    /* 两张 Mod 卡片及标题区。 */
    draw_card_(dc, &layout.asi_card);
    draw_card_(dc, &layout.override_card);

    g_ui.SetTextColor(dc, RGB_(35, 39, 44));
    g_ui.SelectObject(dc, (HGDIOBJ_)g_panel_font);
    text.left = layout.asi_card.left + scale_(hwnd, 18);
    text.top = layout.asi_card.top + scale_(hwnd, 10);
    text.right = layout.asi_card.right - scale_(hwnd, 18);
    text.bottom = layout.asi_card.top + scale_(hwnd, 36);
    g_ui.DrawTextW(dc, (const WCHAR*)L"代码 Mod（ASI）", -1, &text,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    text.left = layout.override_card.left + scale_(hwnd, 18);
    text.right = layout.override_card.right - scale_(hwnd, 18);
    g_ui.DrawTextW(dc, (const WCHAR*)L"文件 Mod（Overrides）", -1, &text,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    g_ui.SelectObject(dc, (HGDIOBJ_)g_font);
    g_ui.SetTextColor(dc, RGB_(116, 125, 136));
    text.left = layout.asi_card.left + scale_(hwnd, 18);
    text.top = layout.asi_card.top + scale_(hwnd, 35);
    text.right = layout.asi_card.right - scale_(hwnd, 18);
    text.bottom = layout.asi_card.top + scale_(hwnd, 60);
    g_ui.DrawTextW(dc, (const WCHAR*)L"从上到下加载  ·  可拖动排序", -1, &text,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    text.left = layout.override_card.left + scale_(hwnd, 18);
    text.right = layout.override_card.right - scale_(hwnd, 18);
    g_ui.DrawTextW(dc, (const WCHAR*)L"越靠下优先级越高  ·  可拖动排序", -1, &text,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    /* 卡片标题区和真正列表正文之间留一条极浅分隔线。 */
    line.left = layout.asi_card.left + scale_(hwnd, 1);
    line.right = layout.asi_card.right - scale_(hwnd, 1);
    line.top = layout.asi_list.top - scale_(hwnd, 1);
    line.bottom = layout.asi_list.top;
    g_ui.FillRect(dc, &line, g_brush_separator);
    line.left = layout.override_card.left + scale_(hwnd, 1);
    line.right = layout.override_card.right - scale_(hwnd, 1);
    g_ui.FillRect(dc, &line, g_brush_separator);

    if (LauncherModConfig_GetCount(LAUNCHER_MOD_ASI) == 0u) {
        draw_empty_state_(dc, hwnd, &layout.asi_list,
                          (const WCHAR*)L"暂无代码 Mod",
                          (const WCHAR*)L"把 .asi 文件放入 mods\\asi 后重新扫描");
    }
    if (LauncherModConfig_GetCount(LAUNCHER_MOD_OVERRIDE) == 0u) {
        draw_empty_state_(dc, hwnd, &layout.override_list,
                          (const WCHAR*)L"暂无文件 Mod",
                          (const WCHAR*)L"把 Mod 文件夹放入 mods\\overrides 后重新扫描");
    }

    /* 底部操作栏覆盖整个宽度，用一条顶部细线与内容区分开。 */
    g_ui.FillRect(dc, &layout.footer, g_brush_card);
    line = layout.footer;
    line.bottom = line.top + scale_(hwnd, 1);
    g_ui.FillRect(dc, &line, g_brush_border);

    u32_to_w_(LauncherModConfig_GetCount(LAUNCHER_MOD_ASI), num_a, 16u);
    u32_to_w_(LauncherModConfig_GetCount(LAUNCHER_MOD_OVERRIDE), num_o, 16u);
    status[0] = 0;
    wcopy_(status, 256u, (const WCHAR*)L"ASI  ");
    wappend_(status, 256u, num_a);
    wappend_(status, 256u, (const WCHAR*)L" 个     ·     Overrides  ");
    wappend_(status, 256u, num_o);
    wappend_(status, 256u, (const WCHAR*)L" 个");

    g_ui.SetTextColor(dc, RGB_(112, 121, 132));
    g_ui.SelectObject(dc, (HGDIOBJ_)g_font);
    count_rc.left = layout.refresh_button.right + scale_(hwnd, 18);
    count_rc.top = layout.footer.top;
    count_rc.right = layout.launch_button.left - scale_(hwnd, 18);
    count_rc.bottom = layout.footer.bottom;
    g_ui.DrawTextW(dc, status, -1, &count_rc,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    g_ui.SelectObject(dc, old);
    g_ui.EndPaint(hwnd, &ps);
}

/* ---------- 设置窗口 ---------- */

static HWND g_setting_save;
static HWND g_setting_cancel;

static void layout_settings_full_(HWND hwnd) {
    RECT_ rc;
    RECT_ row;
    int m, row_h, row_gap, bw, bh, button_gap, footer_h;

    if (!g_ui.GetClientRect(hwnd, &rc)) return;
    m = scale_(hwnd, 20);
    row_h = scale_(hwnd, 42);
    row_gap = scale_(hwnd, 7);
    bw = scale_(hwnd, 72);
    bh = scale_(hwnd, 32);
    button_gap = scale_(hwnd, 8);
    footer_h = scale_(hwnd, 58);

    /* 分类标题由父窗口绘制，真正的可点击行从 50 DIP 开始；两个紧凑行合计只占 91 DIP。 */
    row.left = m;
    row.top = scale_(hwnd, 50);
    row.right = rc.right - m;
    row.bottom = row.top + row_h;
    move_to_rect_(g_setting_modlog, &row);

    row.top = row.bottom + row_gap;
    row.bottom = row.top + row_h;
    move_to_rect_(g_setting_gamelog, &row);

    /* 小按钮固定放在底部右侧，不再使用 dev2 的 96×40 大按钮，避免短窗口里反客为主。 */
    row.right = rc.right - m;
    row.left = row.right - bw;
    row.top = rc.bottom - (footer_h + bh) / 2;
    row.bottom = row.top + bh;
    move_to_rect_(g_setting_save, &row);

    row.right = row.left - button_gap;
    row.left = row.right - bw;
    move_to_rect_(g_setting_cancel, &row);
}

static void paint_settings_(HWND hwnd) {
    PAINTSTRUCT_ ps;
    RECT_ rc, text, line;
    HDC_ dc = g_ui.BeginPaint(hwnd, &ps);
    HGDIOBJ_ old;

    if (!dc) return;
    g_ui.GetClientRect(hwnd, &rc);
    g_ui.FillRect(dc, &rc, g_brush_workspace);
    g_ui.SetBkMode(dc, TRANSPARENT_);

    /* 标题栏已经写了“设置”，客户区只保留分类名，减少重复大标题和说明文字。 */
    g_ui.SetTextColor(dc, RGB_(45, 50, 57));
    old = g_ui.SelectObject(dc, (HGDIOBJ_)g_panel_font);
    text.left = scale_(hwnd, 20);
    text.top = scale_(hwnd, 14);
    text.right = rc.right - scale_(hwnd, 20);
    text.bottom = scale_(hwnd, 42);
    g_ui.DrawTextW(dc, (const WCHAR*)L"日志", -1, &text,
                   DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    line.left = 0;
    line.right = rc.right;
    line.top = rc.bottom - scale_(hwnd, 58);
    line.bottom = line.top + scale_(hwnd, 1);
    g_ui.FillRect(dc, &line, g_brush_border);

    g_ui.SelectObject(dc, old);
    g_ui.EndPaint(hwnd, &ps);
}

static LRESULT_ CALLBACK settings_proc_(HWND hwnd, UINT msg, WPARAM_ w, LPARAM_ l) {
    /* 设置窗口只管理两个日志开关；任何保存失败都保持窗口打开，绝不制造“看起来保存成功”的假状态。 */
    if (msg == WM_PAINT_) {
        paint_settings_(hwnd);
        return 0;
    }
    if (msg == WM_SIZE_) {
        layout_settings_full_(hwnd);
        return 0;
    }
    if (msg == WM_DRAWITEM_) {
        const DRAWITEMSTRUCT_* dis = (const DRAWITEMSTRUCT_*)l;
        if (!dis) return 0;
        if (dis->CtlID == IDC_SETTING_MODLOG_) {
            draw_setting_toggle_(dis,
                                 (const WCHAR*)L"Mod Loader 日志",
                                 (const WCHAR*)L"modloader.log",
                                 g_setting_modlog_value);
            return 1;
        }
        if (dis->CtlID == IDC_SETTING_GAMELOG_) {
            draw_setting_toggle_(dis,
                                 (const WCHAR*)L"游戏运行审计日志",
                                 (const WCHAR*)L"game.log",
                                 g_setting_gamelog_value);
            return 1;
        }
        if (dis->CtlID == IDC_SETTING_SAVE_) {
            draw_flat_button_(dis, (const WCHAR*)L"保存", 1);
            return 1;
        }
        if (dis->CtlID == IDC_SETTING_CANCEL_) {
            draw_flat_button_(dis, (const WCHAR*)L"取消", 0);
            return 1;
        }
    }
    if (msg == WM_COMMAND_) {
        UINT id = (UINT)(w & 0xFFFFu);
        UINT code = (UINT)((w >> 16) & 0xFFFFu);
        if (code == BN_CLICKED_ && id == IDC_SETTING_MODLOG_) {
            g_setting_modlog_value = !g_setting_modlog_value;
            g_ui.InvalidateRect(g_setting_modlog, NULL_PTR, TRUE_);
            return 0;
        }
        if (code == BN_CLICKED_ && id == IDC_SETTING_GAMELOG_) {
            g_setting_gamelog_value = !g_setting_gamelog_value;
            g_ui.InvalidateRect(g_setting_gamelog, NULL_PTR, TRUE_);
            return 0;
        }
        if (code == BN_CLICKED_ && id == IDC_SETTING_SAVE_) {
            if (!LauncherApp_SaveLoggingSettings(g_setting_modlog_value, g_setting_gamelog_value)) {
                g_ui.MessageBoxW(hwnd, (const WCHAR*)L"无法保存 mods\\CastleModLoader.ini。请检查文件是否只读或被占用。",
                                 kWindowTitle_, MB_ICONERROR_ | MB_OK_);
                return 0;
            }
            g_settings_done = 1;
            g_ui.DestroyWindow(hwnd);
            return 0;
        }
        if (code == BN_CLICKED_ && id == IDC_SETTING_CANCEL_) {
            g_settings_done = 1;
            g_ui.DestroyWindow(hwnd);
            return 0;
        }
    }
    if (msg == WM_CLOSE_) {
        g_settings_done = 1;
        g_ui.DestroyWindow(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY_) {
        g_settings_window = NULL_PTR;
        return 0;
    }
    if (msg == WM_DPICHANGED_) {
        RECT_* suggested = (RECT_*)l;
        if (suggested) {
            g_ui.SetWindowPos(hwnd, NULL_PTR, suggested->left, suggested->top,
                              suggested->right - suggested->left, suggested->bottom - suggested->top,
                              SWP_NOZORDER_ | SWP_NOACTIVATE_);
        }
        layout_settings_full_(hwnd);
        g_ui.InvalidateRect(hwnd, NULL_PTR, TRUE_);
        return 0;
    }
    return g_ui.DefWindowProcW(hwnd, msg, w, l);
}

static void show_settings_dialog_(void) {
    /*
     * 继续沿用 dev1 的轻量模态循环：设置窗口打开时禁用主窗口，关闭后恢复。
     * dev2 只换掉子控件视觉，并把 WS_EX_TOOLWINDOW 去掉，使用和主窗口一致的正常标题栏比例，避免截图中那种“小工具窗”质感。
     */
    MSG_ msg;
    int w = scale_(g_main, 460);
    int h = scale_(g_main, 260);

    if (g_settings_window) return;
    g_settings_done = 0;
    g_setting_modlog_value = LauncherApp_GetModLoaderLogEnabled() ? 1 : 0;
    g_setting_gamelog_value = LauncherApp_GetGameLogEnabled() ? 1 : 0;

    g_settings_window = g_ui.CreateWindowExW(WS_EX_CONTROLPARENT_, kSettingsClass_,
                                              (const WCHAR*)L"设置 - 《幽城幻剑录》Mod Loader",
                                              WS_CAPTION_ | WS_SYSMENU_,
                                              0, 0, w, h, g_main, NULL_PTR, g_instance, NULL_PTR);
    if (!g_settings_window) return;

    /*
     * 两个日志开关仍是整行 OWNERDRAW 按钮，但 dev3 改成 42 DIP 单行紧凑布局。
     * 用户点方框、标题或文件名任意位置都能切换；将来增加新设置时只需继续追加同类行，不必再堆大卡片。
     */
    g_setting_modlog = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"",
                                             WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                             0,0,0,0,g_settings_window,(HANDLE)(ULONG_PTR)IDC_SETTING_MODLOG_,g_instance,NULL_PTR);
    g_setting_gamelog = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"",
                                              WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                              0,0,0,0,g_settings_window,(HANDLE)(ULONG_PTR)IDC_SETTING_GAMELOG_,g_instance,NULL_PTR);
    g_setting_save = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"保存",
                                           WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                           0,0,0,0,g_settings_window,(HANDLE)(ULONG_PTR)IDC_SETTING_SAVE_,g_instance,NULL_PTR);
    g_setting_cancel = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"取消",
                                             WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                             0,0,0,0,g_settings_window,(HANDLE)(ULONG_PTR)IDC_SETTING_CANCEL_,g_instance,NULL_PTR);

    if (!g_setting_modlog || !g_setting_gamelog || !g_setting_save || !g_setting_cancel) {
        g_settings_done = 1;
        g_ui.DestroyWindow(g_settings_window);
        return;
    }

    set_font_(g_setting_modlog, g_font);
    set_font_(g_setting_gamelog, g_font);
    set_font_(g_setting_save, g_font);
    set_font_(g_setting_cancel, g_font);

    center_window_over_parent_(g_settings_window, g_main, w, h);
    layout_settings_full_(g_settings_window);
    g_ui.EnableWindow(g_main, FALSE_);
    g_ui.ShowWindow(g_settings_window, SW_SHOW_);
    g_ui.UpdateWindow(g_settings_window);

    while (!g_settings_done && g_ui.GetMessageW(&msg, NULL_PTR, 0u, 0u) > 0) {
        g_ui.TranslateMessage(&msg);
        g_ui.DispatchMessageW(&msg);
    }
    g_ui.EnableWindow(g_main, TRUE_);
    g_ui.SetFocus(g_main);
}

/* ---------- 自定义 About 窗口 ---------- */

static void recreate_about_link_font_(HWND hwnd) {
    UINT i;
    int height = -scale_(hwnd, 15);

    /*
     * 链接使用“蓝色 + 下划线”这个 Windows 用户最熟悉的网页链接视觉。
     * CreateFontW 的第 7 个布尔参数就是 underline；这里传 TRUE_，其余字号/字体继续沿用 Segoe UI。
     */
    if (g_about_link_font) g_ui.DeleteObject((HGDIOBJ_)g_about_link_font);
    g_about_link_font = g_ui.CreateFontW(height, 0, 0, 0, FW_NORMAL_, FALSE_, TRUE_, FALSE_, DEFAULT_CHARSET_,
                                         OUT_DEFAULT_PRECIS_, CLIP_DEFAULT_PRECIS_, CLEARTYPE_QUALITY_, DEFAULT_PITCH_,
                                         (const WCHAR*)L"Segoe UI");
    if (!g_about_link_font) return;
    for (i = 0u; i < ABOUT_MAX_LINKS; ++i) {
        if (g_about_links[i]) set_font_(g_about_links[i], g_about_link_font);
    }
}

/*
 * 用 About 当前实际字体和正文宽度计算正文需要的高度。
 *
 * about3 的实机问题就出在这里：窗口高度和“相关链接”的位置都是固定值，正文只要比预想多折一行，
 * 最后一行就会直接被链接标题盖住。about4 不再猜“正文大概有几行”，而是先让 DrawTextW 用
 * DT_CALCRECT 按真正的窗口宽度做一次不显示的排版测量，再根据测量结果摆放后续内容。
 *
 * 这样以后用户只改 about.cpp 里的文字，只要仍是正常的 About 长度，就不需要再回来手调 GUI 坐标。
 */
static int measure_about_body_height_(HWND hwnd, int body_width) {
    RECT_ calc;
    HDC_ dc;
    HGDIOBJ_ old;
    int height;

    if (!hwnd || body_width <= 0 || !g_ui.GetDC || !g_ui.ReleaseDC) return scale_(hwnd, 120);

    dc = g_ui.GetDC(hwnd);
    if (!dc) return scale_(hwnd, 120);

    calc.left = 0;
    calc.top = 0;
    calc.right = body_width;
    calc.bottom = 0;

    old = g_ui.SelectObject(dc, (HGDIOBJ_)g_font);
    g_ui.DrawTextW(dc, About_GetDialogText(), -1, &calc,
                   DT_LEFT_ | DT_WORDBREAK_ | DT_NOPREFIX_ | DT_CALCRECT_);
    g_ui.SelectObject(dc, old);
    g_ui.ReleaseDC(hwnd, dc);

    height = calc.bottom - calc.top;
    if (height < scale_(hwnd, 72)) height = scale_(hwnd, 72);
    return height;
}

/*
 * 计算正文、链接和底部按钮共用的垂直位置。
 *
 * 这里故意让“相关链接”跟在正文真正结束位置之后，而不是从窗口底部反推一个固定 links_top。
 * 这条规则是 about4 修复正文重叠的核心：正文变高，链接自然一起往下走；正文变短，界面也不会留下夸张空洞。
 */
static int about_links_top_(HWND hwnd, int client_width, int* out_body_bottom, int* out_label_top) {
    int m = scale_(hwnd, 24);
    int body_width = client_width - m * 2;
    int body_height = measure_about_body_height_(hwnd, body_width);
    int body_bottom = m + body_height;
    int label_top = body_bottom + scale_(hwnd, 18);
    int links_top = label_top + scale_(hwnd, 26);

    if (out_body_bottom) *out_body_bottom = body_bottom;
    if (out_label_top) *out_label_top = label_top;
    return links_top;
}

/*
 * 根据 about.cpp 当前正文和链接数量，求出 About 顶层窗口真正需要的外框高度。
 *
 * 先按客户区计算“正文 + 链接 + 底部按钮”总高度，再用 AdjustWindowRectExForDpi/AdjustWindowRectEx
 * 把标题栏和边框加回去。这样用户以后加几行文字或多放几个链接，窗口会自动长高，不再截断正文。
 *
 * 宽度仍保持创建时的 600 DIP，不让 About 因文字长度变成一条横向长条；长句继续由 DT_WORDBREAK 自动折行。
 */
static int get_about_required_outer_size_(HWND hwnd, UINT count, int* out_width, int* out_height) {
    RECT_ client;
    RECT_ outer;
    RECT_ current_outer;
    DWORD style, ex_style;
    int body_bottom, label_top, links_top;
    int link_h, link_gap, footer_h, desired_client_h;

    if (!hwnd || !out_width || !out_height) return 0;
    if (!g_ui.GetClientRect(hwnd, &client) || !g_ui.GetWindowRect(hwnd, &current_outer)) return 0;
    if (count > ABOUT_MAX_LINKS) count = ABOUT_MAX_LINKS;

    link_h = scale_(hwnd, 26);
    link_gap = scale_(hwnd, 4);
    footer_h = scale_(hwnd, 58);
    links_top = about_links_top_(hwnd, client.right - client.left, &body_bottom, &label_top);
    (void)body_bottom;
    (void)label_top;

    desired_client_h = links_top;
    if (count) {
        desired_client_h += (int)count * link_h;
        if (count > 1u) desired_client_h += (int)(count - 1u) * link_gap;
    } else {
        /* 没有链接时不需要空出“相关链接”区域，只在正文后留正常呼吸空间。 */
        desired_client_h = body_bottom + scale_(hwnd, 18);
    }
    desired_client_h += scale_(hwnd, 18) + footer_h;

    /* 即使 About 内容很短，也保留一个不拥挤的最低客户区高度。 */
    if (desired_client_h < scale_(hwnd, 330)) desired_client_h = scale_(hwnd, 330);

    outer.left = 0;
    outer.top = 0;
    outer.right = client.right - client.left;
    outer.bottom = desired_client_h;
    style = (DWORD)g_ui.GetWindowLongW(hwnd, GWL_STYLE_);
    ex_style = (DWORD)g_ui.GetWindowLongW(hwnd, GWL_EXSTYLE_);

    if (g_ui.AdjustWindowRectExForDpi) {
        if (!g_ui.AdjustWindowRectExForDpi(&outer, style, FALSE_, ex_style, current_dpi_(hwnd))) return 0;
    } else {
        if (!g_ui.AdjustWindowRectEx(&outer, style, FALSE_, ex_style)) return 0;
    }

    /* 外框宽度沿用当前创建宽度；高度才是本函数根据内容重新求出的值。 */
    *out_width = current_outer.right - current_outer.left;
    *out_height = outer.bottom - outer.top;
    return *out_width > 0 && *out_height > 0;
}

static void layout_about_(HWND hwnd) {
    RECT_ rc;
    RECT_ r;
    UINT i;
    int m, close_w, close_h, link_h, link_gap, links_top;

    if (!g_ui.GetClientRect(hwnd, &rc)) return;
    m = scale_(hwnd, 24);
    close_w = scale_(hwnd, 82);
    close_h = scale_(hwnd, 34);
    link_h = scale_(hwnd, 26);
    link_gap = scale_(hwnd, 4);
    /* 关闭按钮固定在右下角；正文和链接变高时，顶层窗口自身会先被扩高，因此这里不会压到内容。 */
    r.right = rc.right - m;
    r.left = r.right - close_w;
    r.bottom = rc.bottom - scale_(hwnd, 12);
    r.top = r.bottom - close_h;
    move_to_rect_without_repaint_(g_about_close, &r);

    links_top = about_links_top_(hwnd, rc.right - rc.left, NULL_PTR, NULL_PTR);
    for (i = 0u; i < ABOUT_MAX_LINKS; ++i) {
        if (!g_about_links[i]) continue;
        r.left = m;
        r.right = rc.right - m;
        r.top = links_top + (int)i * (link_h + link_gap);
        r.bottom = r.top + link_h;
        move_to_rect_without_repaint_(g_about_links[i], &r);
    }

}

static void paint_about_(HWND hwnd) {
    PAINTSTRUCT_ ps;
    RECT_ rc, body, label, line;
    HDC_ dc = g_ui.BeginPaint(hwnd, &ps);
    HGDIOBJ_ old;
    UINT count = About_GetLinkCount();
    int body_bottom, label_top, links_top, footer_h;

    if (!dc) return;
    if (!g_ui.GetClientRect(hwnd, &rc)) { g_ui.EndPaint(hwnd, &ps); return; }
    if (count > ABOUT_MAX_LINKS) count = ABOUT_MAX_LINKS;
    footer_h = scale_(hwnd, 58);
    links_top = about_links_top_(hwnd, rc.right - rc.left, &body_bottom, &label_top);

    g_ui.FillRect(dc, &rc, g_brush_workspace);
    g_ui.SetBkMode(dc, TRANSPARENT_);
    old = g_ui.SelectObject(dc, (HGDIOBJ_)g_font);

    /*
     * body.bottom 现在来自实际测量结果，而不是“链接区上方随便减 34 DIP”。
     * 这保证最后一行正文永远有自己完整的绘制矩形，不会再被“相关链接”覆盖。
     */
    body.left = scale_(hwnd, 24);
    body.right = rc.right - scale_(hwnd, 24);
    body.top = scale_(hwnd, 24);
    body.bottom = body_bottom;
    g_ui.SetTextColor(dc, RGB_(47, 54, 64));
    g_ui.DrawTextW(dc, About_GetDialogText(), -1, &body, DT_LEFT_ | DT_WORDBREAK_ | DT_NOPREFIX_);

    if (count) {
        label.left = scale_(hwnd, 24);
        label.right = rc.right - scale_(hwnd, 24);
        label.top = label_top;
        label.bottom = links_top - scale_(hwnd, 4);
        g_ui.SetTextColor(dc, RGB_(112, 121, 132));
        g_ui.DrawTextW(dc, (const WCHAR*)L"相关链接", -1, &label,
                       DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    }

    line.left = 0;
    line.right = rc.right;
    line.top = rc.bottom - footer_h;
    line.bottom = line.top + scale_(hwnd, 1);
    g_ui.FillRect(dc, &line, g_brush_border);

    g_ui.SelectObject(dc, old);
    g_ui.EndPaint(hwnd, &ps);
}

static void draw_about_link_(const DRAWITEMSTRUCT_* dis, UINT index) {
    RECT_ r;
    HGDIOBJ_ old;
    LPCWSTR label;
    DWORD color;

    if (!dis || index >= About_GetLinkCount()) return;
    label = About_GetLinkLabel(index);
    r = dis->rcItem;
    g_ui.FillRect(dis->hDC, &r, g_brush_workspace);
    g_ui.SetBkMode(dis->hDC, TRANSPARENT_);
    old = g_ui.SelectObject(dis->hDC, (HGDIOBJ_)(g_about_link_font ? g_about_link_font : g_font));

    /* 按住时用稍深蓝色反馈；平时保持常见的网页链接蓝。 */
    color = (dis->itemState & ODS_SELECTED_) ? RGB_(0, 78, 152) : RGB_(0, 102, 204);
    g_ui.SetTextColor(dis->hDC, color);
    g_ui.DrawTextW(dis->hDC, label, -1, &r, DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);
    g_ui.SelectObject(dis->hDC, old);
}

static int open_about_url_(HWND owner, LPCWSTR url) {
    typedef HINSTANCE (WINAPI *PFN_ShellExecuteW_)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, int);
    HMODULE shell32;
    PFN_ShellExecuteW_ shell_execute;
    HINSTANCE result;

    if (!url || !url[0]) return 0;

    /*
     * ShellExecuteW 只在用户实际点链接时才需要，所以运行时动态加载 shell32.dll。
     * 这样 CastleModLoader.exe 的静态导入表仍然保持 dev9 的 KERNEL32-only 边界。
     */
    shell32 = LoadLibraryW((const WCHAR*)L"shell32.dll");
    if (!shell32) return 0;
    shell_execute = (PFN_ShellExecuteW_)GetProcAddress(shell32, "ShellExecuteW");
    if (!shell_execute) {
        FreeLibrary(shell32);
        return 0;
    }
    result = shell_execute(owner, (const WCHAR*)L"open", url, NULL_PTR, NULL_PTR, SW_SHOWNORMAL_);
    FreeLibrary(shell32);

    /* ShellExecute 约定大于 32 才代表成功；0～32 是各种“找不到关联程序/访问失败”等错误。 */
    return (ULONG_PTR)result > 32u;
}

static LRESULT_ CALLBACK about_proc_(HWND hwnd, UINT msg, WPARAM_ w, LPARAM_ l) {
    if (msg == WM_PAINT_) {
        paint_about_(hwnd);
        return 0;
    }
    if (msg == WM_SIZE_) {
        layout_about_(hwnd);
        g_ui.RedrawWindow(hwnd, NULL_PTR, NULL_PTR, RDW_INVALIDATE_ | RDW_ERASE_ | RDW_ALLCHILDREN_ | RDW_UPDATENOW_);
        return 0;
    }
    if (msg == WM_DRAWITEM_) {
        const DRAWITEMSTRUCT_* dis = (const DRAWITEMSTRUCT_*)l;
        if (!dis) return 0;
        if (dis->CtlID == IDC_ABOUT_CLOSE_) {
            draw_flat_button_(dis, (const WCHAR*)L"关闭", 0);
            return 1;
        }
        if (dis->CtlID >= IDC_ABOUT_LINK_BASE_ && dis->CtlID < IDC_ABOUT_LINK_BASE_ + ABOUT_MAX_LINKS) {
            draw_about_link_(dis, dis->CtlID - IDC_ABOUT_LINK_BASE_);
            return 1;
        }
    }
    if (msg == WM_COMMAND_) {
        UINT id = (UINT)(w & 0xFFFFu);
        UINT code = (UINT)((w >> 16) & 0xFFFFu);
        if (code == BN_CLICKED_ && id == IDC_ABOUT_CLOSE_) {
            g_about_done = 1;
            g_ui.DestroyWindow(hwnd);
            return 0;
        }
        if (code == BN_CLICKED_ && id >= IDC_ABOUT_LINK_BASE_ && id < IDC_ABOUT_LINK_BASE_ + ABOUT_MAX_LINKS) {
            UINT index = id - IDC_ABOUT_LINK_BASE_;
            if (index < About_GetLinkCount() && !open_about_url_(hwnd, About_GetLinkUrl(index))) {
                g_ui.MessageBoxW(hwnd,
                                 (const WCHAR*)L"无法打开这个链接。请确认 Windows 已配置默认浏览器，或稍后直接复制 About 源码中的网址访问。",
                                 (const WCHAR*)L"打开链接失败", MB_ICONERROR_ | MB_OK_);
            }
            return 0;
        }
    }
    if (msg == WM_CLOSE_) {
        g_about_done = 1;
        g_ui.DestroyWindow(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY_) {
        g_about_window = NULL_PTR;
        return 0;
    }
    if (msg == WM_DPICHANGED_) {
        RECT_* suggested = (RECT_*)l;
        if (suggested) g_ui.SetWindowPos(hwnd, NULL_PTR, suggested->left, suggested->top,
                                         suggested->right - suggested->left, suggested->bottom - suggested->top,
                                         SWP_NOZORDER_ | SWP_NOACTIVATE_);
        recreate_about_link_font_(hwnd);
        set_font_(g_about_close, g_font);
        {
            int required_w = 0;
            int required_h = 0;
            if (get_about_required_outer_size_(hwnd, About_GetLinkCount(), &required_w, &required_h)) {
                g_ui.SetWindowPos(hwnd, NULL_PTR, 0, 0, required_w, required_h,
                                  SWP_NOMOVE_ | SWP_NOZORDER_ | SWP_NOACTIVATE_);
            }
        }
        layout_about_(hwnd);
        g_ui.InvalidateRect(hwnd, NULL_PTR, TRUE_);
        return 0;
    }
    return g_ui.DefWindowProcW(hwnd, msg, w, l);
}

static void show_about_dialog_(void) {
    MSG_ msg;
    UINT count = About_GetLinkCount();
    UINT i;
    int w, h;

    if (g_about_window) return;
    if (count > ABOUT_MAX_LINKS) count = ABOUT_MAX_LINKS;
    for (i = 0u; i < ABOUT_MAX_LINKS; ++i) g_about_links[i] = NULL_PTR;
    g_about_close = NULL_PTR;
    g_about_done = 0;

    /*
     * 先用 600×390 DIP 创建一个尚未显示的窗口，只为了拿到该窗口真正的 DPI、字体和客户区宽度。
     * 子控件创建完后会立刻调用 get_about_required_outer_size_ 按 about.cpp 当前内容重新求高度，
     * 所以 390 只是“测量壳”的初始值，不再是 About 正文必须硬塞进去的固定高度。
     */
    w = scale_(g_main, 600);
    h = scale_(g_main, 390);
    /*
     * About 必须使用和“设置”窗口一致的普通顶层窗口扩展风格。
     *
     * about3/about4 曾额外使用 WS_EX_TOOLWINDOW。这个风格的本意是做浮动工具条：
     * Windows 会给它使用较小的工具窗口标题栏和较小的右上角关闭按钮。
     * 实机截图里那个突兀的红色小方块 X 正是这种非客户端样式带来的视觉差异，
     * 不是我们自己画的按钮。About 明明是正常的模态信息窗口，不应该伪装成工具条。
     *
     * 这里改成只保留 WS_EX_CONTROLPARENT，与“设置”窗口同类：
     *   - Windows 自己绘制标准标题栏和标准关闭按钮；
     *   - 窗口类已经带 RPG.ico，因此标题栏恢复正常程序图标；
     *   - About 仍以 g_main 为 owner，仍然不会因为取消 TOOLWINDOW 就单独占一个任务栏按钮；
     *   - 底部“关闭”按钮和 WM_CLOSE 逻辑都不变。
     *
     * 这只是 Launcher 非客户端外观修正，不触碰 dev9 游戏运行时、Locale、ddraw、ASI 或 Overrides。
     */
    g_about_window = g_ui.CreateWindowExW(WS_EX_CONTROLPARENT_, kAboutClass_, About_GetDialogTitle(),
                                           WS_OVERLAPPED_ | WS_CAPTION_ | WS_SYSMENU_ | WS_CLIPCHILDREN_,
                                           0, 0, w, h, g_main, NULL_PTR, g_instance, NULL_PTR);
    if (!g_about_window) return;

    g_about_close = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"关闭",
                                          WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                          0, 0, 0, 0, g_about_window, (HANDLE)(ULONG_PTR)IDC_ABOUT_CLOSE_, g_instance, NULL_PTR);
    if (!g_about_close) {
        g_about_done = 1;
        g_ui.DestroyWindow(g_about_window);
        return;
    }
    set_font_(g_about_close, g_font);

    for (i = 0u; i < count; ++i) {
        g_about_links[i] = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", About_GetLinkLabel(i),
                                                WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                                0, 0, 0, 0, g_about_window,
                                                (HANDLE)(ULONG_PTR)(IDC_ABOUT_LINK_BASE_ + i), g_instance, NULL_PTR);
        if (!g_about_links[i]) {
            g_about_done = 1;
            g_ui.DestroyWindow(g_about_window);
            return;
        }
    }

    recreate_about_link_font_(g_about_window);

    /*
     * 真正显示前按正文实际折行高度和链接数量扩展窗口。about.cpp 以后多写几行说明时，
     * 这里会自动得到新的 h，不需要用户再去 launcher_gui.c 里找“390/430”之类的魔法数字。
     */
    {
        int required_w = w;
        int required_h = h;
        if (get_about_required_outer_size_(g_about_window, count, &required_w, &required_h)) {
            w = required_w;
            h = required_h;
        }
    }
    center_window_over_parent_(g_about_window, g_main, w, h);
    layout_about_(g_about_window);
    g_ui.EnableWindow(g_main, FALSE_);
    g_ui.ShowWindow(g_about_window, SW_SHOW_);
    g_ui.UpdateWindow(g_about_window);

    while (!g_about_done && g_ui.GetMessageW(&msg, NULL_PTR, 0u, 0u) > 0) {
        g_ui.TranslateMessage(&msg);
        g_ui.DispatchMessageW(&msg);
    }

    if (g_about_link_font) g_ui.DeleteObject((HGDIOBJ_)g_about_link_font);
    g_about_link_font = NULL_PTR;
    g_about_close = NULL_PTR;
    for (i = 0u; i < ABOUT_MAX_LINKS; ++i) g_about_links[i] = NULL_PTR;
    g_ui.EnableWindow(g_main, TRUE_);
    g_ui.SetFocus(g_main);
}

/* ---------- ASI 同名 INI 编辑器 ---------- */

#define INI_ENCODING_ANSI_       1
#define INI_ENCODING_UTF8_       2
#define INI_ENCODING_UTF8_BOM_   3
#define INI_ENCODING_UTF16LE_    4
#define INI_ENCODING_UTF16BE_    5
#define INI_EDITOR_MAX_BYTES_    (8u * 1024u * 1024u)
#define INI_EDITOR_MAX_CHARS_    (4u * 1024u * 1024u)
/* Windows SDK 中这三个常量的固定数值。项目不包含 windows.h，所以在此用带下划线的本地名字声明。 */
#define WC_NO_BEST_FIT_CHARS_        0x00000400u
#define MOVEFILE_REPLACE_EXISTING_   0x00000001u
#define MOVEFILE_WRITE_THROUGH_      0x00000008u

static int is_space_tab_(WCHAR c) {
    /* INI 的“左右空白”只处理空格和 Tab；换行在逐行扫描时已经被分离。 */
    return c == (WCHAR)' ' || c == (WCHAR)'\t';
}

static int is_full_line_comment_(const WCHAR* text, UINT start, UINT end) {
    /*
     * 标准 INI 最常见的是 ';' 和 '#' 注释；不少 ASI 插件也把 '//' 当整行说明。
     * 这里只在“去掉行首空白以后”判断，因此值里的 http://、分号等不会被误当成注释。
     */
    UINT i = start;
    while (i < end && is_space_tab_(text[i])) ++i;
    if (i >= end) return 0;
    if (text[i] == (WCHAR)';' || text[i] == (WCHAR)'#') return 1;
    return text[i] == (WCHAR)'/' && i + 1u < end && text[i + 1u] == (WCHAR)'/';
}

static void set_ini_error_(WCHAR* out, UINT cap, UINT line_no, const WCHAR* reason, const WCHAR* text, UINT start, UINT end) {
    WCHAR num[24];
    UINT i;
    UINT preview_count = 0u;

    if (!out || cap == 0u) return;
    out[0] = 0;
    u32_to_w_(line_no, num, 24u);
    wcopy_(out, cap, (const WCHAR*)L"第 ");
    wappend_(out, cap, num);
    wappend_(out, cap, (const WCHAR*)L" 行：");
    wappend_(out, cap, reason ? reason : (const WCHAR*)L"INI 语法不合法。");

    /*
     * 再附上最多 100 个字符的原行预览。这样弹窗不仅告诉用户“第几行”，还直接展示出问题内容；
     * 预览过长会遮住真正原因，所以故意截断并追加省略号。
     */
    if (!text || start >= end) return;
    wappend_(out, cap, (const WCHAR*)L"\n\n内容：");
    for (i = start; i < end && preview_count < 100u; ++i, ++preview_count) {
        WCHAR one[2];
        one[0] = text[i];
        one[1] = 0;
        if (!wappend_(out, cap, one)) break;
    }
    if (i < end) wappend_(out, cap, (const WCHAR*)L"…");
}

static int validate_ini_text_(const WCHAR* text, UINT chars, WCHAR* error, UINT error_cap,
                              LONG* error_start, LONG* error_end, UINT* error_line) {
    /*
     * 这是“通用 INI 结构检查”，不是某个插件的业务配置检查。编辑器不知道每个 ASI 允许哪些键和值，
     * 因此只验证所有正常 INI 都应该满足的语法边界：
     *   - 空行和整行注释允许；
     *   - section 必须是 [名称]，名称不能为空，']' 后只能是空白或注释；
     *   - 普通设置必须包含 '='，并且等号左边的键名去掉空白后不能为空；
     *   - 文本里不能混入除 Tab 之外的不可见控制字符。
     * 值内容保持开放：数字、布尔、路径、中文、再次出现 '=' 都由具体插件自己解释。
     */
    UINT line_start = 0u;
    UINT line_no = 1u;

    if (error && error_cap) error[0] = 0;
    if (error_start) *error_start = -1;
    if (error_end) *error_end = -1;
    if (error_line) *error_line = 0u;
    if (!text) return 0;

    while (line_start <= chars) {
        UINT line_end = line_start;
        UINT left, right, i;

        while (line_end < chars && text[line_end] != (WCHAR)'\r' && text[line_end] != (WCHAR)'\n') ++line_end;
        left = line_start;
        right = line_end;
        while (left < right && is_space_tab_(text[left])) ++left;
        while (right > left && is_space_tab_(text[right - 1u])) --right;

        for (i = left; i < right; ++i) {
            if (text[i] < (WCHAR)0x20u && text[i] != (WCHAR)'\t') {
                set_ini_error_(error, error_cap, line_no, (const WCHAR*)L"含有不允许的控制字符。", text, line_start, line_end);
                if (error_start) *error_start = (LONG)line_start;
                if (error_end) *error_end = (LONG)line_end;
                if (error_line) *error_line = line_no;
                return 0;
            }
        }

        if (left < right && !is_full_line_comment_(text, line_start, line_end)) {
            if (text[left] == (WCHAR)'[') {
                UINT close = left + 1u;
                UINT name_left = left + 1u;
                UINT name_right;
                while (close < right && text[close] != (WCHAR)']') ++close;
                if (close >= right) {
                    set_ini_error_(error, error_cap, line_no, (const WCHAR*)L"节名以 '[' 开始，但这一行缺少右方括号 ']'.", text, line_start, line_end);
                    if (error_start) *error_start = (LONG)line_start;
                    if (error_end) *error_end = (LONG)line_end;
                    if (error_line) *error_line = line_no;
                    return 0;
                }
                name_right = close;
                while (name_left < name_right && is_space_tab_(text[name_left])) ++name_left;
                while (name_right > name_left && is_space_tab_(text[name_right - 1u])) --name_right;
                if (name_left >= name_right) {
                    set_ini_error_(error, error_cap, line_no, (const WCHAR*)L"节名不能为空；请在 [ 和 ] 之间填写名称。", text, line_start, line_end);
                    if (error_start) *error_start = (LONG)line_start;
                    if (error_end) *error_end = (LONG)line_end;
                    if (error_line) *error_line = line_no;
                    return 0;
                }

                /* ']' 之后只允许空白，或者再跟一个整行尾注释标记。 */
                i = close + 1u;
                while (i < right && is_space_tab_(text[i])) ++i;
                if (i < right && !(text[i] == (WCHAR)';' || text[i] == (WCHAR)'#' ||
                                   (text[i] == (WCHAR)'/' && i + 1u < right && text[i + 1u] == (WCHAR)'/'))) {
                    set_ini_error_(error, error_cap, line_no, (const WCHAR*)L"节名右方括号 ']' 后出现了无法识别的内容。", text, line_start, line_end);
                    if (error_start) *error_start = (LONG)line_start;
                    if (error_end) *error_end = (LONG)line_end;
                    if (error_line) *error_line = line_no;
                    return 0;
                }
            } else {
                UINT eq = left;
                UINT key_right;
                while (eq < right && text[eq] != (WCHAR)'=') ++eq;
                if (eq >= right) {
                    set_ini_error_(error, error_cap, line_no, (const WCHAR*)L"普通设置必须写成“变量名=值”；这一行缺少等号 '='。", text, line_start, line_end);
                    if (error_start) *error_start = (LONG)line_start;
                    if (error_end) *error_end = (LONG)line_end;
                    if (error_line) *error_line = line_no;
                    return 0;
                }
                key_right = eq;
                while (key_right > left && is_space_tab_(text[key_right - 1u])) --key_right;
                if (key_right <= left) {
                    set_ini_error_(error, error_cap, line_no, (const WCHAR*)L"等号左边的变量名不能为空。", text, line_start, line_end);
                    if (error_start) *error_start = (LONG)line_start;
                    if (error_end) *error_end = (LONG)line_end;
                    if (error_line) *error_line = line_no;
                    return 0;
                }
            }
        }

        if (line_end >= chars) break;
        if (text[line_end] == (WCHAR)'\r' && line_end + 1u < chars && text[line_end + 1u] == (WCHAR)'\n') line_start = line_end + 2u;
        else line_start = line_end + 1u;
        ++line_no;
    }
    return 1;
}

typedef LPVOID (WINAPI *PFN_VirtualAlloc_)(LPVOID, SIZE_T, DWORD, DWORD);
typedef BOOL (WINAPI *PFN_VirtualFree_)(LPVOID, SIZE_T, DWORD);

static PFN_VirtualAlloc_ get_virtual_alloc_(void) {
    /*
     * Launcher 一直坚持“最终 PE 静态只导入 KERNEL32 中既有的最小函数集合”。INI 编辑器需要较大的临时缓冲区，
     * 但没有必要为了两个内存函数重新扩大我们手工维护的 x86 导入表。因此这里和 USER32/GDI32 一样，
     * 从已经加载的 KERNEL32 运行时取 VirtualAlloc 地址。GetModuleHandleW / GetProcAddress 本来就是 Launcher 的稳定依赖。
     *
     * 对初学者来说可以把它理解成：我们先向 Windows 问“VirtualAlloc 这个按钮在哪里”，拿到地址后再按按钮，
     * 而不是在 EXE 的固定进口清单里再增加一项。功能完全相同，只是让发布包的静态依赖边界保持不变。
     */
    HMODULE kernel = GetModuleHandleW((const WCHAR*)L"KERNEL32.dll");
    return kernel ? (PFN_VirtualAlloc_)GetProcAddress(kernel, "VirtualAlloc") : NULL_PTR;
}

static PFN_VirtualFree_ get_virtual_free_(void) {
    /* 与上面的 VirtualAlloc 配对；只有成功取得函数地址时才会真正释放。 */
    HMODULE kernel = GetModuleHandleW((const WCHAR*)L"KERNEL32.dll");
    return kernel ? (PFN_VirtualFree_)GetProcAddress(kernel, "VirtualFree") : NULL_PTR;
}

static WCHAR* alloc_wchars_(UINT chars) {
    PFN_VirtualAlloc_ fn = get_virtual_alloc_();
    SIZE_T bytes = ((SIZE_T)chars + 1u) * (SIZE_T)sizeof(WCHAR);
    /* 多留一个 WCHAR 给结尾的 NUL；函数地址拿不到时返回 NULL，让上层显示明确的“内存分配失败”，绝不继续写空指针。 */
    return fn ? (WCHAR*)fn(NULL_PTR, bytes, MEM_RESERVE_ | MEM_COMMIT_, PAGE_READWRITE_) : NULL_PTR;
}

static BYTE* alloc_bytes_(UINT bytes) {
    PFN_VirtualAlloc_ fn = get_virtual_alloc_();
    /* 多留 4 字节只是为了后续编码转换时有安全尾部，不把它当成文件数据写回。 */
    return fn ? (BYTE*)fn(NULL_PTR, (SIZE_T)bytes + 4u, MEM_RESERVE_ | MEM_COMMIT_, PAGE_READWRITE_) : NULL_PTR;
}

static void free_alloc_(void* memory) {
    PFN_VirtualFree_ fn;
    if (!memory) return;
    fn = get_virtual_free_();
    /* MEM_RELEASE 要求 size=0；只有函数地址存在才调用，避免错误函数指针造成崩溃。 */
    if (fn) fn(memory, 0u, MEM_RELEASE_);
}

static int load_ini_text_(const WCHAR* path, WCHAR** out_text, UINT* out_chars, int* out_encoding,
                          WCHAR* error, UINT error_cap) {
    HANDLE file;
    DWORD high = 0u;
    DWORD size;
    DWORD read = 0u;
    BYTE* bytes = NULL_PTR;
    WCHAR* text = NULL_PTR;
    UINT offset = 0u;
    UINT payload;
    int chars = 0;
    UINT i;
    int has_high_byte = 0;

    if (out_text) *out_text = NULL_PTR;
    if (out_chars) *out_chars = 0u;
    if (out_encoding) *out_encoding = INI_ENCODING_ANSI_;
    if (error && error_cap) error[0] = 0;
    if (!path || !out_text || !out_chars || !out_encoding) return 0;

    file = CreateFileW(path, GENERIC_READ_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_, NULL_PTR,
                       OPEN_EXISTING_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) {
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"无法打开这个 INI；文件可能已被移动、删除或没有读取权限。");
        return 0;
    }
    size = GetFileSize(file, &high);
    if (high != 0u || size > INI_EDITOR_MAX_BYTES_) {
        CloseHandle(file);
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"INI 文件过大。内置编辑器为避免一次占用过多内存，单文件上限为 8 MiB。");
        return 0;
    }

    bytes = alloc_bytes_(size + 1u);
    if (!bytes) {
        CloseHandle(file);
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"没有足够内存读取 INI。");
        return 0;
    }
    if (size && (!ReadFile(file, bytes, size, &read, NULL_PTR) || read != size)) {
        CloseHandle(file);
        free_alloc_(bytes);
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"读取 INI 时发生错误；文件内容没有进入编辑器。");
        return 0;
    }
    CloseHandle(file);

    /* 先看 BOM，因为 BOM 是最可靠的编码声明。 */
    if (size >= 2u && bytes[0] == 0xFFu && bytes[1] == 0xFEu) {
        payload = size - 2u;
        if (payload & 1u) {
            free_alloc_(bytes);
            if (error) wcopy_(error, error_cap, (const WCHAR*)L"UTF-16LE INI 的字节数不是偶数，文件可能已经损坏。");
            return 0;
        }
        text = alloc_wchars_(payload / 2u);
        if (!text) { free_alloc_(bytes); if (error) wcopy_(error, error_cap, (const WCHAR*)L"没有足够内存解码 INI。"); return 0; }
        for (i = 0u; i < payload / 2u; ++i) text[i] = (WCHAR)((UINT)bytes[2u + i * 2u] | ((UINT)bytes[3u + i * 2u] << 8));
        chars = (int)(payload / 2u);
        *out_encoding = INI_ENCODING_UTF16LE_;
    } else if (size >= 2u && bytes[0] == 0xFEu && bytes[1] == 0xFFu) {
        payload = size - 2u;
        if (payload & 1u) {
            free_alloc_(bytes);
            if (error) wcopy_(error, error_cap, (const WCHAR*)L"UTF-16BE INI 的字节数不是偶数，文件可能已经损坏。");
            return 0;
        }
        text = alloc_wchars_(payload / 2u);
        if (!text) { free_alloc_(bytes); if (error) wcopy_(error, error_cap, (const WCHAR*)L"没有足够内存解码 INI。"); return 0; }
        for (i = 0u; i < payload / 2u; ++i) text[i] = (WCHAR)(((UINT)bytes[2u + i * 2u] << 8) | (UINT)bytes[3u + i * 2u]);
        chars = (int)(payload / 2u);
        *out_encoding = INI_ENCODING_UTF16BE_;
    } else {
        if (size >= 3u && bytes[0] == 0xEFu && bytes[1] == 0xBBu && bytes[2] == 0xBFu) {
            offset = 3u;
            *out_encoding = INI_ENCODING_UTF8_BOM_;
        } else {
            for (i = 0u; i < size; ++i) if (bytes[i] >= 0x80u) { has_high_byte = 1; break; }
            /*
             * 无 BOM 且全 ASCII 的老插件 INI 更可能是 ANSI；ASCII 在 ANSI/UTF-8 中字节本来相同，
             * 选择 ANSI 可以在用户随后输入本地文字时尽量保持旧插件的传统编码习惯。
             */
            *out_encoding = has_high_byte ? INI_ENCODING_UTF8_ : INI_ENCODING_ANSI_;
        }
        payload = size - offset;

        if (*out_encoding == INI_ENCODING_UTF8_ || *out_encoding == INI_ENCODING_UTF8_BOM_) {
            if (payload) chars = MultiByteToWideChar(CP_UTF8_, MB_ERR_INVALID_CHARS_, (LPCSTR)(bytes + offset), (int)payload, NULL_PTR, 0);
            if (payload && chars <= 0 && *out_encoding == INI_ENCODING_UTF8_) {
                /* 无 BOM 的高位字节不是合法 UTF-8，就按当前 Windows ANSI 代码页解释并按同编码保存。 */
                *out_encoding = INI_ENCODING_ANSI_;
                chars = MultiByteToWideChar(CP_ACP_, 0u, (LPCSTR)bytes, (int)size, NULL_PTR, 0);
                offset = 0u;
                payload = size;
            }
        } else if (payload) {
            chars = MultiByteToWideChar(CP_ACP_, 0u, (LPCSTR)bytes, (int)payload, NULL_PTR, 0);
        }

        if (payload && chars <= 0) {
            free_alloc_(bytes);
            if (error) wcopy_(error, error_cap, (const WCHAR*)L"无法识别 INI 的文字编码；原文件没有被修改。");
            return 0;
        }
        text = alloc_wchars_((UINT)chars);
        if (!text) { free_alloc_(bytes); if (error) wcopy_(error, error_cap, (const WCHAR*)L"没有足够内存解码 INI。"); return 0; }
        if (payload) {
            UINT cp = (*out_encoding == INI_ENCODING_ANSI_) ? CP_ACP_ : CP_UTF8_;
            DWORD flags = cp == CP_UTF8_ ? MB_ERR_INVALID_CHARS_ : 0u;
            if (MultiByteToWideChar(cp, flags, (LPCSTR)(bytes + offset), (int)payload, text, chars) != chars) {
                free_alloc_(text);
                free_alloc_(bytes);
                if (error) wcopy_(error, error_cap, (const WCHAR*)L"解码 INI 时发生错误；原文件没有被修改。");
                return 0;
            }
        }
    }

    text[(UINT)chars] = 0;
    free_alloc_(bytes);
    *out_text = text;
    *out_chars = (UINT)chars;
    return 1;
}

static WCHAR* get_ini_editor_text_(UINT* out_chars) {
    LRESULT_ len;
    WCHAR* text;
    LRESULT_ got;

    if (out_chars) *out_chars = 0u;
    if (!g_ini_editor_text) return NULL_PTR;
    len = g_ui.SendMessageW(g_ini_editor_text, WM_GETTEXTLENGTH_, 0, 0);
    if (len < 0 || (UINT)len > INI_EDITOR_MAX_CHARS_) return NULL_PTR;
    text = alloc_wchars_((UINT)len);
    if (!text) return NULL_PTR;
    got = g_ui.SendMessageW(g_ini_editor_text, WM_GETTEXT_, (WPARAM_)((UINT)len + 1u), (LPARAM_)text);
    if (got < 0) {
        free_alloc_(text);
        return NULL_PTR;
    }
    if (out_chars) *out_chars = (UINT)got;
    return text;
}

static void set_rich_color_(LONG start, LONG end, DWORD color) {
    CHARRANGE_ range;
    CHARFORMATW_ cf;
    UINT i;

    if (!g_ini_editor_text || start < 0 || end <= start) return;
    range.cpMin = start;
    range.cpMax = end;
    g_ui.SendMessageW(g_ini_editor_text, EM_EXSETSEL_, 0, (LPARAM_)&range);

    /* CHARFORMATW 没有 CRT memset；每个字段显式清零，让结构内容完全可预测。 */
    cf.cbSize = (UINT)sizeof(cf);
    cf.dwMask = CFM_COLOR_;
    cf.dwEffects = 0u;
    cf.yHeight = 0;
    cf.yOffset = 0;
    cf.crTextColor = color;
    cf.bCharSet = 0u;
    cf.bPitchAndFamily = 0u;
    for (i = 0u; i < 32u; ++i) cf.szFaceName[i] = 0;
    g_ui.SendMessageW(g_ini_editor_text, EM_SETCHARFORMAT_, SCF_SELECTION_, (LPARAM_)&cf);
}

static int find_next_ini_line_break_(LONG start, LONG doc_end, LONG* break_start, LONG* break_end) {
    FINDTEXTEXW_ cr;
    FINDTEXTEXW_ lf;
    LRESULT_ cr_result;
    LRESULT_ lf_result;
    LONG best_start = -1;
    LONG best_end = -1;

    if (break_start) *break_start = -1;
    if (break_end) *break_end = -1;
    if (!g_ini_editor_text || start < 0 || doc_end < start) return 0;

    /*
     * RichEdit 通常把段落分隔符规范化成 '\r'，但旧版本/特殊粘贴来源也可能暴露 '\n'。
     * 两个都搜索并取更靠前的那个，保证“逻辑 INI 行”不依赖某一个 RichEdit 版本的换行表示。
     */
    cr.chrg.cpMin = start;
    cr.chrg.cpMax = doc_end;
    cr.lpstrText = (const WCHAR*)L"\r";
    cr.chrgText.cpMin = cr.chrgText.cpMax = -1;
    cr_result = g_ui.SendMessageW(g_ini_editor_text, EM_FINDTEXTEXW_, FR_DOWN_, (LPARAM_)&cr);
    if (cr_result >= 0) {
        best_start = cr.chrgText.cpMin;
        best_end = cr.chrgText.cpMax;
    }

    lf.chrg.cpMin = start;
    lf.chrg.cpMax = doc_end;
    lf.lpstrText = (const WCHAR*)L"\n";
    lf.chrgText.cpMin = lf.chrgText.cpMax = -1;
    lf_result = g_ui.SendMessageW(g_ini_editor_text, EM_FINDTEXTEXW_, FR_DOWN_, (LPARAM_)&lf);
    if (lf_result >= 0 && (best_start < 0 || lf.chrgText.cpMin < best_start)) {
        best_start = lf.chrgText.cpMin;
        best_end = lf.chrgText.cpMax;
    }

    if (best_start < 0) return 0;
    if (break_start) *break_start = best_start;
    if (break_end) *break_end = best_end;
    return 1;
}

static int get_ini_logical_line_range_(UINT line_no, LONG* out_start, LONG* out_end) {
    LONG doc_end;
    LONG cp_start = 0;
    UINT current = 1u;

    if (out_start) *out_start = -1;
    if (out_end) *out_end = -1;
    if (!g_ini_editor_text || line_no == 0u) return 0;

    doc_end = (LONG)g_ui.SendMessageW(g_ini_editor_text, WM_GETTEXTLENGTH_, 0, 0);
    if (doc_end < 0) return 0;

    for (;;) {
        LONG break_start = -1;
        LONG break_end = -1;
        int has_break = find_next_ini_line_break_(cp_start, doc_end, &break_start, &break_end);
        LONG cp_end = has_break ? break_start : doc_end;

        if (current == line_no) {
            if (out_start) *out_start = cp_start;
            if (out_end) *out_end = cp_end;
            return 1;
        }
        if (!has_break) return 0;

        /*
         * 极端情况下 RichEdit 若把 CR/LF 暴露成两个独立字符，下一次循环会立刻遇到第二个换行。
         * 这里检测相邻的另一种换行并一起跨过去，避免把 CRLF 错算成中间多一个空逻辑行。
         */
        cp_start = break_end;
        if (cp_start < doc_end) {
            WCHAR pair[2];
            TEXTRANGEW_ tr;
            tr.chrg.cpMin = cp_start;
            tr.chrg.cpMax = cp_start + 1;
            tr.lpstrText = pair;
            pair[0] = pair[1] = 0;
            if (g_ui.SendMessageW(g_ini_editor_text, EM_GETTEXTRANGE_, 0, (LPARAM_)&tr) == 1 &&
                (pair[0] == (WCHAR)'\r' || pair[0] == (WCHAR)'\n')) {
                cp_start += 1;
            }
        }
        ++current;
        if (cp_start > doc_end) return 0;
    }
}

static void colorize_ini_editor_(void) {
    CHARRANGE_ saved;
    LONG first_line_before;
    LONG first_line_after;
    LONG doc_end;
    LONG cp_start = 0;

    if (!g_ini_editor_text || g_ini_editor_coloring) return;

    /*
     * about3 开启自动换行以后，绝对不能继续用 EM_GETLINECOUNT / EM_LINEINDEX 做语法行：
     * Microsoft 的 RichEdit 会把“视觉折行”也算成 line，窗口宽度一变，line 数量就会变化。
     *
     * 这里改为真正的“逻辑行”流程：
     *   1. 用 EM_FINDTEXTEXW 在 RichEdit 自己的 cp 坐标里寻找 '\r'/'\n'；
     *   2. 两个换行符之间才是一条真实 INI 行；
     *   3. 再用 EM_GETTEXTRANGE 读取同一 cp 区间；
     *   4. 所有颜色仍用同一 cp 区间写回。
     *
     * 因此一条超长注释即使视觉上自动折成三行，语法上仍只是一整条注释，颜色也会完整保持绿色。
     */
    g_ini_editor_coloring = 1;
    saved.cpMin = saved.cpMax = 0;
    g_ui.SendMessageW(g_ini_editor_text, EM_EXGETSEL_, 0, (LPARAM_)&saved);
    first_line_before = (LONG)g_ui.SendMessageW(g_ini_editor_text, EM_GETFIRSTVISIBLELINE_, 0, 0);
    g_ui.SendMessageW(g_ini_editor_text, WM_SETREDRAW_, FALSE_, 0);
    doc_end = (LONG)g_ui.SendMessageW(g_ini_editor_text, WM_GETTEXTLENGTH_, 0, 0);
    if (doc_end < 0) doc_end = 0;

    while (cp_start <= doc_end) {
        LONG break_start = -1;
        LONG break_end = -1;
        int has_break = find_next_ini_line_break_(cp_start, doc_end, &break_start, &break_end);
        LONG cp_end = has_break ? break_start : doc_end;
        LONG line_len = cp_end - cp_start;

        if (line_len < 0 || (UINT)line_len > INI_EDITOR_MAX_CHARS_) break;

        if (line_len > 0) {
            WCHAR* line = alloc_wchars_((UINT)line_len);
            if (line) {
                TEXTRANGEW_ tr;
                LRESULT_ got;
                UINT left;
                UINT right;

                tr.chrg.cpMin = cp_start;
                tr.chrg.cpMax = cp_end;
                tr.lpstrText = line;
                got = g_ui.SendMessageW(g_ini_editor_text, EM_GETTEXTRANGE_, 0, (LPARAM_)&tr);
                if (got < 0) got = 0;
                if (got > line_len) got = line_len;
                line[(UINT)got] = 0;
                line_len = (LONG)got;

                /* 先恢复整条逻辑行的正文色，再覆盖语义色，避免编辑后残留上一种颜色。 */
                if (line_len > 0) set_rich_color_(cp_start, cp_start + line_len, RGB_(45, 50, 57));

                left = 0u;
                right = (UINT)line_len;
                while (left < right && is_space_tab_(line[left])) ++left;
                while (right > left && is_space_tab_(line[right - 1u])) --right;

                if (left < right) {
                    if (is_full_line_comment_(line, 0u, (UINT)line_len)) {
                        set_rich_color_(cp_start, cp_start + line_len, RGB_(86, 124, 78));
                    } else if (line[left] == (WCHAR)'[') {
                        UINT close = left + 1u;
                        while (close < right && line[close] != (WCHAR)']') ++close;
                        if (close < right) {
                            UINT tail = close + 1u;
                            set_rich_color_(cp_start + (LONG)left, cp_start + (LONG)(close + 1u), RGB_(116, 82, 164));
                            while (tail < right && is_space_tab_(line[tail])) ++tail;
                            if (tail < right && (line[tail] == (WCHAR)';' || line[tail] == (WCHAR)'#' ||
                                (line[tail] == (WCHAR)'/' && tail + 1u < right && line[tail + 1u] == (WCHAR)'/'))) {
                                set_rich_color_(cp_start + (LONG)tail, cp_start + line_len, RGB_(86, 124, 78));
                            } else if (tail < right) {
                                set_rich_color_(cp_start + (LONG)tail, cp_start + (LONG)right, RGB_(220, 38, 38));
                            }
                        } else {
                            set_rich_color_(cp_start + (LONG)left, cp_start + (LONG)right, RGB_(220, 38, 38));
                        }
                    } else {
                        UINT eq = left;
                        UINT key_right;
                        while (eq < right && line[eq] != (WCHAR)'=') ++eq;
                        if (eq < right) {
                            UINT value_left = eq + 1u;
                            key_right = eq;
                            while (key_right > left && is_space_tab_(line[key_right - 1u])) --key_right;
                            while (value_left < right && is_space_tab_(line[value_left])) ++value_left;

                            if (key_right > left)
                                set_rich_color_(cp_start + (LONG)left, cp_start + (LONG)key_right, RGB_(38, 98, 162));
                            else
                                set_rich_color_(cp_start + (LONG)left, cp_start + (LONG)(eq + 1u), RGB_(220, 38, 38));
                            set_rich_color_(cp_start + (LONG)eq, cp_start + (LONG)(eq + 1u), RGB_(122, 130, 140));
                            if (value_left < right)
                                set_rich_color_(cp_start + (LONG)value_left, cp_start + (LONG)right, RGB_(173, 91, 38));
                        } else {
                            set_rich_color_(cp_start + (LONG)left, cp_start + (LONG)right, RGB_(220, 38, 38));
                        }
                    }
                }
                free_alloc_(line);
            }
        }

        if (!has_break) break;

        /*
         * 跳过刚找到的换行；如果 RichEdit 把 CRLF 暴露成两个相邻字符，再顺手跨过第二个，
         * 这样下一轮一定从下一条真正的 INI 逻辑行开始。
         */
        cp_start = break_end;
        if (cp_start < doc_end) {
            WCHAR pair[2];
            TEXTRANGEW_ tr;
            tr.chrg.cpMin = cp_start;
            tr.chrg.cpMax = cp_start + 1;
            tr.lpstrText = pair;
            pair[0] = pair[1] = 0;
            if (g_ui.SendMessageW(g_ini_editor_text, EM_GETTEXTRANGE_, 0, (LPARAM_)&tr) == 1 &&
                (pair[0] == (WCHAR)'\r' || pair[0] == (WCHAR)'\n')) {
                cp_start += 1;
            }
        }
    }

    g_ui.SendMessageW(g_ini_editor_text, EM_EXSETSEL_, 0, (LPARAM_)&saved);
    first_line_after = (LONG)g_ui.SendMessageW(g_ini_editor_text, EM_GETFIRSTVISIBLELINE_, 0, 0);
    if (first_line_after != first_line_before)
        g_ui.SendMessageW(g_ini_editor_text, EM_LINESCROLL_, 0, (LPARAM_)(first_line_before - first_line_after));
    g_ui.SendMessageW(g_ini_editor_text, WM_SETREDRAW_, TRUE_, 0);
    g_ui.InvalidateRect(g_ini_editor_text, NULL_PTR, TRUE_);
    g_ini_editor_coloring = 0;
}

static void delete_ini_temp_(const WCHAR* path) {
    typedef BOOL (WINAPI *PFN_DeleteFileW_)(LPCWSTR);
    HMODULE kernel = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    PFN_DeleteFileW_ fn = kernel ? (PFN_DeleteFileW_)GetProcAddress(kernel, "DeleteFileW") : NULL_PTR;
    if (fn && path) fn(path);
}

static int replace_ini_temp_(const WCHAR* temp_path, const WCHAR* final_path) {
    typedef BOOL (WINAPI *PFN_MoveFileExW_)(LPCWSTR, LPCWSTR, DWORD);
    HMODULE kernel = GetModuleHandleW((const WCHAR*)L"kernel32.dll");
    PFN_MoveFileExW_ fn = kernel ? (PFN_MoveFileExW_)GetProcAddress(kernel, "MoveFileExW") : NULL_PTR;
    /* MOVEFILE_REPLACE_EXISTING(1) + MOVEFILE_WRITE_THROUGH(8)：替换主文件并尽量在返回前把写入落盘。 */
    return fn && fn(temp_path, final_path, MOVEFILE_REPLACE_EXISTING_ | MOVEFILE_WRITE_THROUGH_);
}

static int save_ini_editor_file_(WCHAR* error, UINT error_cap, LONG* bad_start, LONG* bad_end, UINT* bad_line) {
    WCHAR* text;
    UINT chars;
    BYTE* encoded = NULL_PTR;
    UINT byte_count = 0u;
    UINT prefix = 0u;
    int needed = 0;
    BOOL used_default = FALSE_;
    WCHAR temp_path[CASTLE_PATH_CAP];
    HANDLE file;
    DWORD wrote = 0u;
    UINT i;

    if (error && error_cap) error[0] = 0;
    if (bad_start) *bad_start = -1;
    if (bad_end) *bad_end = -1;
    if (bad_line) *bad_line = 0u;
    text = get_ini_editor_text_(&chars);
    if (!text) {
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"无法从编辑框读取完整文本；本次没有保存。");
        return 0;
    }

    /* 第一道门：先验证结构。任何非法行都在写临时文件之前停止，因此原 INI 绝不会被半写坏。 */
    if (!validate_ini_text_(text, chars, error, error_cap, bad_start, bad_end, bad_line)) {
        free_alloc_(text);
        return 0;
    }

    if (g_ini_editor_encoding == INI_ENCODING_UTF16LE_ || g_ini_editor_encoding == INI_ENCODING_UTF16BE_) {
        prefix = 2u;
        byte_count = prefix + chars * 2u;
        encoded = alloc_bytes_(byte_count);
        if (!encoded) { free_alloc_(text); if (error) wcopy_(error, error_cap, (const WCHAR*)L"没有足够内存编码 INI；本次没有保存。"); return 0; }
        encoded[0] = g_ini_editor_encoding == INI_ENCODING_UTF16LE_ ? 0xFFu : 0xFEu;
        encoded[1] = g_ini_editor_encoding == INI_ENCODING_UTF16LE_ ? 0xFEu : 0xFFu;
        for (i = 0u; i < chars; ++i) {
            WCHAR c = text[i];
            if (g_ini_editor_encoding == INI_ENCODING_UTF16LE_) {
                encoded[prefix + i * 2u] = (BYTE)(c & 0xFFu);
                encoded[prefix + i * 2u + 1u] = (BYTE)((c >> 8) & 0xFFu);
            } else {
                encoded[prefix + i * 2u] = (BYTE)((c >> 8) & 0xFFu);
                encoded[prefix + i * 2u + 1u] = (BYTE)(c & 0xFFu);
            }
        }
    } else {
        UINT cp = g_ini_editor_encoding == INI_ENCODING_ANSI_ ? CP_ACP_ : CP_UTF8_;
        DWORD convert_flags = cp == CP_ACP_ ? WC_NO_BEST_FIT_CHARS_ : 0u;
        prefix = g_ini_editor_encoding == INI_ENCODING_UTF8_BOM_ ? 3u : 0u;
        /*
         * ANSI 保存时明确禁止 Windows 的“近似字符替换”(best fit)。例如某个 Unicode 字符如果不能真正写回当前 ANSI 代码页，
         * 我们宁可拒绝保存并告诉用户，也不能偷偷换成一个看起来相似但字节已经不同的字符。UTF-8 不允许传这个标志，所以仍用 0。
         */
        if (chars) needed = WideCharToMultiByte(cp, convert_flags, text, (int)chars, NULL_PTR, 0, NULL_PTR,
                                                cp == CP_ACP_ ? &used_default : NULL_PTR);
        if (chars && needed <= 0) {
            free_alloc_(text);
            if (error) wcopy_(error, error_cap, (const WCHAR*)L"无法按原文件编码重新编码文字；本次没有保存。");
            return 0;
        }
        if (cp == CP_ACP_ && used_default) {
            free_alloc_(text);
            if (error) wcopy_(error, error_cap, (const WCHAR*)L"原 INI 是 ANSI 编码，但当前输入包含本机 ANSI 代码页无法表示的字符。为避免静默变成问号，本次拒绝保存。请改用该插件支持的字符，或先在外部工具明确转换编码。");
            return 0;
        }
        byte_count = prefix + (UINT)needed;
        encoded = alloc_bytes_(byte_count);
        if (!encoded) { free_alloc_(text); if (error) wcopy_(error, error_cap, (const WCHAR*)L"没有足够内存编码 INI；本次没有保存。"); return 0; }
        if (prefix == 3u) { encoded[0] = 0xEFu; encoded[1] = 0xBBu; encoded[2] = 0xBFu; }
        used_default = FALSE_;
        if (chars && WideCharToMultiByte(cp, convert_flags, text, (int)chars, (LPSTR)(encoded + prefix), needed, NULL_PTR,
                                         cp == CP_ACP_ ? &used_default : NULL_PTR) != needed) {
            free_alloc_(encoded);
            free_alloc_(text);
            if (error) wcopy_(error, error_cap, (const WCHAR*)L"编码 INI 时发生错误；本次没有保存。");
            return 0;
        }
        if (cp == CP_ACP_ && used_default) {
            free_alloc_(encoded);
            free_alloc_(text);
            if (error) wcopy_(error, error_cap, (const WCHAR*)L"ANSI 编码转换会丢失字符；为避免损坏，本次没有保存。");
            return 0;
        }
    }
    free_alloc_(text);

    if (!wcopy_(temp_path, CASTLE_PATH_CAP, g_ini_editor_path) ||
        !wappend_(temp_path, CASTLE_PATH_CAP, (const WCHAR*)L".castle.tmp")) {
        free_alloc_(encoded);
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"INI 路径过长，无法构造安全临时文件名；本次没有保存。");
        return 0;
    }

    file = CreateFileW(temp_path, GENERIC_WRITE_, FILE_SHARE_READ_, NULL_PTR,
                       CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (file == INVALID_HANDLE_VALUE_) {
        free_alloc_(encoded);
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"无法创建同目录临时文件；请检查 INI 所在目录是否只读或被安全软件阻止。");
        return 0;
    }
    if (byte_count && (!WriteFile(file, encoded, byte_count, &wrote, NULL_PTR) || wrote != byte_count)) {
        CloseHandle(file);
        delete_ini_temp_(temp_path);
        free_alloc_(encoded);
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"写入临时 INI 失败；原 INI 保持不变。");
        return 0;
    }
    if (!FlushFileBuffers(file)) {
        CloseHandle(file);
        delete_ini_temp_(temp_path);
        free_alloc_(encoded);
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"临时 INI 无法完整刷新到磁盘；原 INI 保持不变。");
        return 0;
    }
    CloseHandle(file);
    free_alloc_(encoded);

    if (!replace_ini_temp_(temp_path, g_ini_editor_path)) {
        delete_ini_temp_(temp_path);
        if (error) wcopy_(error, error_cap, (const WCHAR*)L"临时文件已写完，但无法原子替换原 INI。请检查原文件是否只读、被占用或没有写入权限；原 INI 保持不变。");
        return 0;
    }
    return 1;
}

static void select_ini_error_line_(UINT line_no) {
    LONG cp_start = -1;
    LONG cp_end = -1;
    CHARRANGE_ range;

    if (!g_ini_editor_text || line_no == 0u) return;

    /*
     * 自动换行后，EM_LINEINDEX(line_no-1) 指向的是“视觉行”，不再等于 INI 的真实第 N 行。
     * about3 与分色共用同一个逻辑行定位器：按 RichEdit 内部的真实 CR/LF 分隔符找到第 N 条配置行。
     */
    if (!get_ini_logical_line_range_(line_no, &cp_start, &cp_end)) return;
    range.cpMin = cp_start;
    range.cpMax = cp_end;
    g_ui.SendMessageW(g_ini_editor_text, EM_EXSETSEL_, 0, (LPARAM_)&range);
    g_ui.SetFocus(g_ini_editor_text);
}

static int save_ini_editor_(HWND owner) {
    WCHAR error[1024];
    LONG bad_start = -1;
    LONG bad_end = -1;
    UINT bad_line = 0u;

    if (!save_ini_editor_file_(error, 1024u, &bad_start, &bad_end, &bad_line)) {
        if (bad_line != 0u) select_ini_error_line_(bad_line);

        /*
         * MessageBox 仍然负责把完整原因一次说清楚；同时把摘要写进编辑器底部状态栏，并标成亮红色。
         * 这样用户关掉弹窗后不会马上失去“刚才为什么不能保存”的视觉提示。
         */
        g_ini_editor_status_error = 1;
        g_ini_editor_status[0] = 0;
        wcopy_(g_ini_editor_status, 256u, (const WCHAR*)L"● 保存失败：");
        if (bad_line != 0u) {
            WCHAR line_text[24];
            wappend_(g_ini_editor_status, 256u, (const WCHAR*)L"第 ");
            u32_to_w_(bad_line, line_text, 24u);
            wappend_(g_ini_editor_status, 256u, line_text);
            wappend_(g_ini_editor_status, 256u, (const WCHAR*)L" 行；");
        }
        if (error[0]) wappend_(g_ini_editor_status, 256u, error);
        else wappend_(g_ini_editor_status, 256u, (const WCHAR*)L"INI 未通过检查，原文件没有被修改。");
        g_ui.InvalidateRect(owner, NULL_PTR, TRUE_);

        g_ui.MessageBoxW(owner,
                         error[0] ? error : (const WCHAR*)L"INI 保存失败；原文件没有被修改。",
                         (const WCHAR*)L"INI 检查未通过", MB_ICONERROR_ | MB_OK_);
        return 0;
    }
    g_ini_editor_dirty = 0;
    g_ini_editor_status_error = 0;
    wcopy_(g_ini_editor_status, 256u, (const WCHAR*)L"已检查语法并安全保存。原文件编码保持不变。");
    g_ui.InvalidateRect(owner, NULL_PTR, TRUE_);
    return 1;
}

static int request_close_ini_editor_(HWND hwnd) {
    int answer;
    if (!g_ini_editor_dirty) {
        g_ini_editor_done = 1;
        g_ui.DestroyWindow(hwnd);
        return 1;
    }

    answer = g_ui.MessageBoxW(hwnd,
                              (const WCHAR*)L"这个 INI 还有未保存的修改。\n\n是：先检查语法并保存，再关闭\n否：放弃修改并关闭\n取消：继续编辑",
                              (const WCHAR*)L"关闭 INI 编辑器", MB_YESNOCANCEL_ | MB_ICONQUESTION_);
    if (answer == IDYES_) {
        if (!save_ini_editor_(hwnd)) return 0;
        g_ini_editor_done = 1;
        g_ui.DestroyWindow(hwnd);
        return 1;
    }
    if (answer == IDNO_) {
        g_ini_editor_done = 1;
        g_ui.DestroyWindow(hwnd);
        return 1;
    }
    return 0;
}

static void refresh_ini_word_wrap_(void) {
    if (!g_ini_editor_text) return;

    /*
     * RichEdit 的 EM_SETTARGETDEVICE(NULL, 1) 表示“不按窗口宽度折行”，而 NULL, 0 表示恢复按控件宽度折行。
     * about2 的调用发生在 RichEdit 仍然是 0×0 的创建阶段，实机证明没有形成有效的最终折行宽度。
     *
     * about3 把这一步放到“MoveWindow 已经给文本框真实尺寸之后”，并先切到 1、再切回 0：
     *   1. 先明确清掉可能缓存的旧目标宽度；
     *   2. 再让 RichEdit 用当前客户区宽度重新排版；
     *   3. 每次 WM_SIZE 都会重新执行，因此拖动编辑器大小时换行也会立即跟着变化。
     *
     * 这只是显示排版，不会向正文插入 CR/LF，所以保存出来的 INI 文件仍保持原始逻辑行。
     */
    g_ui.SendMessageW(g_ini_editor_text, EM_SETTARGETDEVICE_, 0, 1);
    g_ui.SendMessageW(g_ini_editor_text, EM_SETTARGETDEVICE_, 0, 0);
}

static void recreate_ini_font_(HWND hwnd) {
    int height = -scale_(hwnd, 16);
    if (g_ini_editor_font) g_ui.DeleteObject((HGDIOBJ_)g_ini_editor_font);
    /* Consolas 是 Windows 自带等宽字体；INI 的 '='、缩进和路径在等宽字体下更容易逐列核对。 */
    g_ini_editor_font = g_ui.CreateFontW(height,0,0,0,FW_NORMAL_,FALSE_,FALSE_,FALSE_,DEFAULT_CHARSET_,
                                         OUT_DEFAULT_PRECIS_,CLIP_DEFAULT_PRECIS_,CLEARTYPE_QUALITY_,DEFAULT_PITCH_,
                                         (const WCHAR*)L"Consolas");
    if (g_ini_editor_text && g_ini_editor_font) set_font_(g_ini_editor_text, g_ini_editor_font);
}

static void layout_ini_editor_(HWND hwnd) {
    RECT_ rc;
    RECT_ r;
    int m, top_h, footer_h, bw, bh, gap;
    if (!g_ui.GetClientRect(hwnd, &rc)) return;
    m = scale_(hwnd, 16);
    top_h = scale_(hwnd, 50);
    footer_h = scale_(hwnd, 62);
    bw = scale_(hwnd, 82);
    bh = scale_(hwnd, 34);
    gap = scale_(hwnd, 8);

    /* 文本区四周留 16 DIP；顶部给语法颜色图例，底部给状态文字和保存/关闭按钮。 */
    r.left = m + scale_(hwnd, 1);
    r.top = top_h + scale_(hwnd, 1);
    r.right = rc.right - m - scale_(hwnd, 1);
    r.bottom = rc.bottom - footer_h - scale_(hwnd, 10) - scale_(hwnd, 1);
    move_to_rect_without_repaint_(g_ini_editor_text, &r);
    /* 文本框已经有真实宽度以后再强制 RichEdit 重新建立自动换行布局。 */
    refresh_ini_word_wrap_();

    r.right = rc.right - m;
    r.left = r.right - bw;
    r.top = rc.bottom - footer_h + (footer_h - bh) / 2;
    r.bottom = r.top + bh;
    move_to_rect_without_repaint_(g_ini_editor_close, &r);
    r.right = r.left - gap;
    r.left = r.right - bw;
    move_to_rect_without_repaint_(g_ini_editor_save, &r);
}

static void paint_ini_editor_(HWND hwnd) {
    PAINTSTRUCT_ ps;
    RECT_ rc, r, line;
    HDC_ dc = g_ui.BeginPaint(hwnd, &ps);
    HGDIOBJ_ old;
    if (!dc) return;
    if (!g_ui.GetClientRect(hwnd, &rc)) { g_ui.EndPaint(hwnd, &ps); return; }

    g_ui.FillRect(dc, &rc, g_brush_workspace);
    g_ui.SetBkMode(dc, TRANSPARENT_);
    old = g_ui.SelectObject(dc, (HGDIOBJ_)g_font);

    /* 顶部直接给出颜色图例：用户不需要猜紫色/蓝色/橙色分别代表什么。 */
    r.top = scale_(hwnd, 13);
    r.bottom = scale_(hwnd, 40);
    r.left = scale_(hwnd, 18);
    r.right = r.left + scale_(hwnd, 84);
    g_ui.SetTextColor(dc, RGB_(86, 124, 78));
    g_ui.DrawTextW(dc, (const WCHAR*)L"; 注释", -1, &r, DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    r.left = r.right;
    r.right = r.left + scale_(hwnd, 72);
    g_ui.SetTextColor(dc, RGB_(116, 82, 164));
    g_ui.DrawTextW(dc, (const WCHAR*)L"[节]", -1, &r, DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    r.left = r.right;
    r.right = r.left + scale_(hwnd, 88);
    g_ui.SetTextColor(dc, RGB_(38, 98, 162));
    g_ui.DrawTextW(dc, (const WCHAR*)L"变量名", -1, &r, DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    r.left = r.right;
    r.right = r.left + scale_(hwnd, 30);
    g_ui.SetTextColor(dc, RGB_(122, 130, 140));
    g_ui.DrawTextW(dc, (const WCHAR*)L"=", -1, &r, DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);
    r.left = r.right;
    r.right = r.left + scale_(hwnd, 110);
    g_ui.SetTextColor(dc, RGB_(173, 91, 38));
    g_ui.DrawTextW(dc, (const WCHAR*)L"值", -1, &r, DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_NOPREFIX_);

    /* RichEdit 自己负责白色正文；父窗口只画一圈非常轻的卡片边框，不使用经典 WS_EX_CLIENTEDGE 凹框。 */
    r.left = scale_(hwnd, 16);
    r.top = scale_(hwnd, 50);
    r.right = rc.right - scale_(hwnd, 16);
    r.bottom = rc.bottom - scale_(hwnd, 72);
    g_ui.FrameRect(dc, &r, g_brush_border);

    line.left = 0;
    line.right = rc.right;
    line.top = rc.bottom - scale_(hwnd, 62);
    line.bottom = line.top + scale_(hwnd, 1);
    g_ui.FillRect(dc, &line, g_brush_border);

    r.left = scale_(hwnd, 18);
    r.top = line.top;
    r.right = g_ini_editor_save ? (rc.right - scale_(hwnd, 196)) : rc.right - scale_(hwnd, 18);
    r.bottom = rc.bottom;
    /* 未保存和保存错误都必须一眼能看到：统一使用高对比度亮红，而不是旧版不明显的棕色。 */
    g_ui.SetTextColor(dc, (g_ini_editor_dirty || g_ini_editor_status_error) ? RGB_(220, 38, 38) : RGB_(112, 121, 132));
    g_ui.DrawTextW(dc,
                   g_ini_editor_status[0] ? g_ini_editor_status : (g_ini_editor_dirty ? (const WCHAR*)L"● 未保存修改；保存时会先检查 INI 结构。" : (const WCHAR*)L"保存前会检查 INI 结构，并保持原文件编码。"),
                   -1, &r, DT_LEFT_ | DT_VCENTER_ | DT_SINGLELINE_ | DT_END_ELLIPSIS_ | DT_NOPREFIX_);

    g_ui.SelectObject(dc, old);
    g_ui.EndPaint(hwnd, &ps);
}

static LRESULT_ CALLBACK ini_editor_proc_(HWND hwnd, UINT msg, WPARAM_ w, LPARAM_ l) {
    if (msg == WM_PAINT_) {
        paint_ini_editor_(hwnd);
        return 0;
    }
    if (msg == WM_GETMINMAXINFO_) {
        MINMAXINFO_* info = (MINMAXINFO_*)l;
        if (info) {
            /* 编辑器允许缩放，但不能小到按钮和正文互相覆盖。数值是外框近似下限，主窗口仍保持自己的精确 1024×640 客户区规则。 */
            info->ptMinTrackSize.x = (LONG)scale_(hwnd, 680);
            info->ptMinTrackSize.y = (LONG)scale_(hwnd, 460);
        }
        return 0;
    }
    if (msg == WM_SIZE_) {
        layout_ini_editor_(hwnd);
        g_ui.RedrawWindow(hwnd, NULL_PTR, NULL_PTR, RDW_INVALIDATE_ | RDW_ERASE_ | RDW_ALLCHILDREN_ | RDW_UPDATENOW_);
        return 0;
    }
    if (msg == WM_DRAWITEM_) {
        const DRAWITEMSTRUCT_* dis = (const DRAWITEMSTRUCT_*)l;
        if (!dis) return 0;
        if (dis->CtlID == IDC_INI_EDITOR_SAVE_) { draw_flat_button_(dis, (const WCHAR*)L"保存", 1); return 1; }
        if (dis->CtlID == IDC_INI_EDITOR_CLOSE_) { draw_flat_button_(dis, (const WCHAR*)L"关闭", 0); return 1; }
    }
    if (msg == WM_COMMAND_) {
        UINT id = (UINT)(w & 0xFFFFu);
        UINT code = (UINT)((w >> 16) & 0xFFFFu);
        if (id == IDC_INI_EDITOR_TEXT_ && code == EN_CHANGE_) {
            if (!g_ini_editor_coloring) {
                g_ini_editor_dirty = 1;
                g_ini_editor_status_error = 0;
                wcopy_(g_ini_editor_status, 256u, (const WCHAR*)L"● 未保存修改；保存时会先检查 INI 结构。");
                colorize_ini_editor_();
                g_ui.InvalidateRect(hwnd, NULL_PTR, TRUE_);
            }
            return 0;
        }
        if (id == IDC_INI_EDITOR_SAVE_ && code == BN_CLICKED_) {
            save_ini_editor_(hwnd);
            return 0;
        }
        if (id == IDC_INI_EDITOR_CLOSE_ && code == BN_CLICKED_) {
            request_close_ini_editor_(hwnd);
            return 0;
        }
    }
    if (msg == WM_CLOSE_) {
        request_close_ini_editor_(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY_) {
        g_ini_editor_window = NULL_PTR;
        return 0;
    }
    if (msg == WM_DPICHANGED_) {
        RECT_* suggested = (RECT_*)l;
        if (suggested) g_ui.SetWindowPos(hwnd, NULL_PTR, suggested->left, suggested->top,
                                         suggested->right - suggested->left, suggested->bottom - suggested->top,
                                         SWP_NOZORDER_ | SWP_NOACTIVATE_);
        recreate_ini_font_(hwnd);
        layout_ini_editor_(hwnd);
        g_ui.InvalidateRect(hwnd, NULL_PTR, TRUE_);
        return 0;
    }
    return g_ui.DefWindowProcW(hwnd, msg, w, l);
}

static int build_asi_ini_path_(UINT index, WCHAR* out, UINT cap) {
    const LauncherModItem_* item = LauncherModConfig_GetItem(LAUNCHER_MOD_ASI, index);
    WCHAR asi_root[CASTLE_PATH_CAP];
    WCHAR file_name[MAX_PATH_];

    if (!item || !item->present || !item->has_ini || !out || cap == 0u) return 0;
    if (!path_join_(asi_root, CASTLE_PATH_CAP, LauncherApp_GetModsRoot(), (const WCHAR*)L"asi")) return 0;
    if (!wcopy_(file_name, MAX_PATH_, item->name) || !wappend_(file_name, MAX_PATH_, (const WCHAR*)L".ini")) return 0;
    return path_join_(out, cap, asi_root, file_name);
}

static void show_ini_editor_(UINT asi_index) {
    const LauncherModItem_* item;
    WCHAR* initial_text = NULL_PTR;
    UINT initial_chars = 0u;
    WCHAR error[1024];
    const WCHAR* rich_class;
    MSG_ msg;
    int w, h;

    if (g_ini_editor_window) return;
    item = LauncherModConfig_GetItem(LAUNCHER_MOD_ASI, asi_index);
    if (!item || !build_asi_ini_path_(asi_index, g_ini_editor_path, CASTLE_PATH_CAP)) {
        g_ui.MessageBoxW(g_main, (const WCHAR*)L"没有找到这个插件对应的同名 INI。请重新扫描后再试。",
                         (const WCHAR*)L"INI 编辑器", MB_ICONERROR_ | MB_OK_);
        return;
    }

    if (!load_ini_text_(g_ini_editor_path, &initial_text, &initial_chars, &g_ini_editor_encoding, error, 1024u)) {
        g_ui.MessageBoxW(g_main, error[0] ? error : (const WCHAR*)L"无法读取这个 INI。",
                         (const WCHAR*)L"INI 编辑器", MB_ICONERROR_ | MB_OK_);
        return;
    }

    /* RichEdit 作为系统组件动态载入，因此 Launcher 的静态 PE 依赖仍然只有 KERNEL32。优先 RichEdit 4.1，旧系统再退回 RichEdit 2.0。 */
    g_ini_richedit_module = LoadLibraryW((const WCHAR*)L"Msftedit.dll");
    rich_class = (const WCHAR*)L"RICHEDIT50W";
    if (!g_ini_richedit_module) {
        g_ini_richedit_module = LoadLibraryW((const WCHAR*)L"Riched20.dll");
        rich_class = (const WCHAR*)L"RichEdit20W";
    }
    if (!g_ini_richedit_module) {
        free_alloc_(initial_text);
        g_ui.MessageBoxW(g_main, (const WCHAR*)L"系统 RichEdit 组件无法载入，因此不能提供带语法颜色的内置编辑器。",
                         (const WCHAR*)L"INI 编辑器", MB_ICONERROR_ | MB_OK_);
        return;
    }

    g_ini_editor_title[0] = 0;
    wcopy_(g_ini_editor_title, CASTLE_PATH_CAP, item->name);
    wappend_(g_ini_editor_title, CASTLE_PATH_CAP, (const WCHAR*)L".ini - INI 编辑器");
    g_ini_editor_status[0] = 0;
    g_ini_editor_status_error = 0;
    g_ini_editor_dirty = 0;
    g_ini_editor_coloring = 1;
    g_ini_editor_done = 0;

    w = scale_(g_main, 860);
    h = scale_(g_main, 620);
    g_ini_editor_window = g_ui.CreateWindowExW(WS_EX_CONTROLPARENT_, kIniEditorClass_, g_ini_editor_title,
                                                WS_OVERLAPPEDWINDOW_ | WS_CLIPCHILDREN_,
                                                0,0,w,h,g_main,NULL_PTR,g_instance,NULL_PTR);
    if (!g_ini_editor_window) {
        g_ini_editor_coloring = 0;
        free_alloc_(initial_text);
        FreeLibrary(g_ini_richedit_module);
        g_ini_richedit_module = NULL_PTR;
        return;
    }

    g_ini_editor_text = g_ui.CreateWindowExW(0u, rich_class, (const WCHAR*)L"",
                                              WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | WS_VSCROLL_ |
                                              ES_MULTILINE_ | ES_AUTOVSCROLL_ | ES_NOHIDESEL_ | ES_WANTRETURN_,
                                              0,0,0,0,g_ini_editor_window,(HANDLE)(ULONG_PTR)IDC_INI_EDITOR_TEXT_,g_instance,NULL_PTR);
    g_ini_editor_save = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"保存",
                                              WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                              0,0,0,0,g_ini_editor_window,(HANDLE)(ULONG_PTR)IDC_INI_EDITOR_SAVE_,g_instance,NULL_PTR);
    g_ini_editor_close = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"关闭",
                                               WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                               0,0,0,0,g_ini_editor_window,(HANDLE)(ULONG_PTR)IDC_INI_EDITOR_CLOSE_,g_instance,NULL_PTR);
    if (!g_ini_editor_text || !g_ini_editor_save || !g_ini_editor_close) {
        g_ini_editor_done = 1;
        g_ui.DestroyWindow(g_ini_editor_window);
        free_alloc_(initial_text);
        if (g_ini_richedit_module) FreeLibrary(g_ini_richedit_module);
        g_ini_richedit_module = NULL_PTR;
        g_ini_editor_coloring = 0;
        return;
    }

    recreate_ini_font_(g_ini_editor_window);
    set_font_(g_ini_editor_save, g_font);
    set_font_(g_ini_editor_close, g_font);
    g_ui.SendMessageW(g_ini_editor_text, EM_EXLIMITTEXT_, 0, (LPARAM_)INI_EDITOR_MAX_CHARS_);
    g_ui.SendMessageW(g_ini_editor_text, EM_SETBKGNDCOLOR_, 0, (LPARAM_)RGB_(255, 255, 255));
    g_ui.SendMessageW(g_ini_editor_text, EM_SETEVENTMASK_, 0, (LPARAM_)ENM_CHANGE_);
    g_ui.SetWindowTextW(g_ini_editor_text, initial_text);
    free_alloc_(initial_text);
    g_ini_editor_coloring = 0;
    colorize_ini_editor_();
    g_ini_editor_dirty = 0;

    center_window_over_parent_(g_ini_editor_window, g_main, w, h);
    layout_ini_editor_(g_ini_editor_window);
    g_ui.EnableWindow(g_main, FALSE_);
    g_ui.ShowWindow(g_ini_editor_window, SW_SHOW_);
    g_ui.UpdateWindow(g_ini_editor_window);
    g_ui.SetFocus(g_ini_editor_text);

    while (!g_ini_editor_done && g_ui.GetMessageW(&msg, NULL_PTR, 0u, 0u) > 0) {
        g_ui.TranslateMessage(&msg);
        g_ui.DispatchMessageW(&msg);
    }

    g_ini_editor_text = NULL_PTR;
    g_ini_editor_save = NULL_PTR;
    g_ini_editor_close = NULL_PTR;
    if (g_ini_editor_font) g_ui.DeleteObject((HGDIOBJ_)g_ini_editor_font);
    g_ini_editor_font = NULL_PTR;
    if (g_ini_richedit_module) FreeLibrary(g_ini_richedit_module);
    g_ini_richedit_module = NULL_PTR;
    g_ui.EnableWindow(g_main, TRUE_);
    g_ui.SetFocus(g_main);
}


/* ---------- 主窗口过程 ---------- */

static LRESULT_ CALLBACK main_proc_(HWND hwnd, UINT msg, WPARAM_ w, LPARAM_ l) {
    /*
     * 主窗口过程只做 UI 路由：绘制、尺寸、按钮命令、DPI 和退出。真正的 Mod 规则不写在这里，
     * 所以以后即使改视觉，也不应该改变 launcher_mod_config.c/Core 已固定的扫描和持久化语义。
     */
    if (msg == WM_PAINT_) {
        paint_main_(hwnd);
        return 0;
    }
    if (msg == WM_GETMINMAXINFO_) {
        MINMAXINFO_* info = (MINMAXINFO_*)l;
        int min_w, min_h;
        /*
         * 用户已经明确裁决：默认窗口就是最小窗口，客户区不得小于 1024×640 DIP。
         * 这里约束的是 Windows 的“拖动最小外框尺寸”，因此先把客户区尺寸换算为含标题栏/边框的真实外框尺寸。
         */
        if (info && get_main_minimum_outer_size_(hwnd, &min_w, &min_h)) {
            info->ptMinTrackSize.x = (LONG)min_w;
            info->ptMinTrackSize.y = (LONG)min_h;
        }
        return 0;
    }
    if (msg == WM_SIZE_) {
        /*
         * 先把全部主控件一次性移动到新的几何位置，但不让五个子控件各自立即重画；
         * 等位置全部稳定以后，再统一刷新父窗口和所有子窗口，这样连续缩放时不会留下旧 ListBox/边框/按钮残影。
         */
        layout_main_();
        redraw_main_now_(hwnd);
        return 0;
    }
    if (msg == WM_DRAWITEM_) {
        const DRAWITEMSTRUCT_* dis = (const DRAWITEMSTRUCT_*)l;
        if (!dis) return 0;
        if (dis->CtlID == IDC_ASI_LIST_ || dis->CtlID == IDC_OVERRIDE_LIST_) {
            draw_mod_row_(dis);
            return 1;
        }
        if (dis->CtlID == IDC_ABOUT_) {
            draw_flat_button_(dis, (const WCHAR*)L"关于", 0);
            return 1;
        }
        if (dis->CtlID == IDC_SETTINGS_) {
            draw_flat_button_(dis, (const WCHAR*)L"设置", 0);
            return 1;
        }
        if (dis->CtlID == IDC_REFRESH_) {
            draw_flat_button_(dis, (const WCHAR*)L"重新扫描", 0);
            return 1;
        }
        if (dis->CtlID == IDC_LAUNCH_) {
            draw_flat_button_(dis, (const WCHAR*)L"启动游戏", 1);
            return 1;
        }
    }
    if (msg == WM_COMMAND_) {
        UINT id = (UINT)(w & 0xFFFFu);
        UINT code = (UINT)((w >> 16) & 0xFFFFu);
        if (code == BN_CLICKED_ && id == IDC_ABOUT_) {
            /*
             * About 已经是独立自定义窗口。正文和链接仍全部来自 about.cpp，主窗口只负责打开它；
             * 因此以后改 About 内容或 URL，不需要碰主窗口布局、Mod 配置或游戏启动流程。
             */
            show_about_dialog_();
            return 0;
        }
        if (code == BN_CLICKED_ && id == IDC_SETTINGS_) {
            show_settings_dialog_();
            return 0;
        }
        if (code == BN_CLICKED_ && id == IDC_REFRESH_) {
            /* 重新扫描会重新读取磁盘和 INI；新 Mod 会按固定规则自动登记，缺失/空目录状态也会刷新。 */
            if (!LauncherModConfig_Refresh(LauncherApp_GetModsRoot())) show_config_error_(hwnd);
            else refresh_all_lists_();
            return 0;
        }
        if (code == BN_CLICKED_ && id == IDC_LAUNCH_) {
            /*
             * 启动期间先禁用按钮防止双击创建两个 RPG.exe。成功后销毁 GUI；失败则恢复按钮并留在界面上，
             * 用户可以检查日志/路径后再次尝试。
             */
            g_ui.EnableWindow(g_launch_button, FALSE_);
            if (LauncherApp_StartGame()) {
                g_game_started = 1;
                g_exit_code = 0u;
                g_ui.DestroyWindow(hwnd);
            } else {
                g_ui.EnableWindow(g_launch_button, TRUE_);
                g_ui.MessageBoxW(hwnd, (const WCHAR*)L"游戏没有成功启动。请检查 modloader.log 或确认 RPG.exe / Loader 核心文件是否完整。",
                                 kWindowTitle_, MB_ICONERROR_ | MB_OK_);
            }
            return 0;
        }
    }
    if (msg == WM_DPICHANGED_) {
        RECT_* suggested = (RECT_*)l;
        if (suggested) g_ui.SetWindowPos(hwnd, NULL_PTR, suggested->left, suggested->top,
                                         suggested->right - suggested->left, suggested->bottom - suggested->top,
                                         SWP_NOZORDER_ | SWP_NOACTIVATE_);
        recreate_fonts_(hwnd);
        set_font_(g_asi_list, g_font);
        set_font_(g_override_list, g_font);
        set_font_(g_about_button, g_font);
        set_font_(g_settings_button, g_font);
        set_font_(g_refresh_button, g_font);
        set_font_(g_launch_button, g_font);
        g_ui.SendMessageW(g_asi_list, LB_SETITEMHEIGHT_, 0, (LPARAM_)scale_(g_asi_list, 42));
        g_ui.SendMessageW(g_override_list, LB_SETITEMHEIGHT_, 0, (LPARAM_)scale_(g_override_list, 42));
        layout_main_();
        redraw_main_now_(hwnd);
        return 0;
    }
    if (msg == WM_CLOSE_) {
        g_exit_code = g_game_started ? 0u : 1u;
        g_ui.DestroyWindow(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY_) {
        g_main = NULL_PTR;
        g_ui.PostQuitMessage(0);
        return 0;
    }
    return g_ui.DefWindowProcW(hwnd, msg, w, l);
}

/* ---------- 类注册、控件创建、消息循环 ---------- */

static int register_classes_(void) {
    /*
     * Win32 创建自定义顶层窗口前必须先注册窗口类。主窗口、设置窗口、About、INI 编辑器分别注册，
     * 因为它们的 WndProc、缩放能力和生命周期不同；背景刷都使用同一工作区画刷，系统擦背景时不会先闪出经典白底。
     */
    WNDCLASSEXW_ wc;
    HCURSOR_ cursor = g_ui.LoadCursorW(NULL_PTR, IDC_ARROW_);
    /*
     * RPG.ico 被编译进资源表，resource.h 与 launcher.rc 共用同一个 ID。
     * LoadIconW 读取的是模块共享资源，生命周期由 Windows 管理，所以这里只保存句柄给窗口类使用，不需要释放。
     */
    void* app_icon = g_ui.LoadIconW(g_instance, (LPCWSTR)(ULONG_PTR)IDI_RPG_ICON);

    wc.cbSize = (UINT)sizeof(wc);
    wc.style = 0u;
    wc.lpfnWndProc = main_proc_;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = g_instance;
    wc.hIcon = app_icon;
    wc.hCursor = cursor;
    wc.hbrBackground = g_brush_workspace;
    wc.lpszMenuName = NULL_PTR;
    wc.lpszClassName = kMainClass_;
    wc.hIconSm = app_icon;
    if (!g_ui.RegisterClassExW(&wc)) return 0;

    wc.lpfnWndProc = settings_proc_;
    wc.lpszClassName = kSettingsClass_;
    if (!g_ui.RegisterClassExW(&wc)) return 0;

    /* About 使用自己的窗口类，才能提供可点击链接，而不是退回无法承载链接控件的 MessageBox。 */
    wc.lpfnWndProc = about_proc_;
    wc.lpszClassName = kAboutClass_;
    if (!g_ui.RegisterClassExW(&wc)) return 0;

    /* INI 编辑器是第四个独立顶层窗口类：它可缩放、带 RichEdit 文本区，生命周期与紧凑设置窗不同。 */
    wc.lpfnWndProc = ini_editor_proc_;
    wc.lpszClassName = kIniEditorClass_;
    if (!g_ui.RegisterClassExW(&wc)) return 0;
    return 1;
}

static int create_main_controls_(void) {
    /*
     * 这里只创建真正需要接收输入/滚动的控件；标题和提示文字由父窗口自己画。两个 ListBox 使用同一种风格，
     * 然后统一子类化到 list_proc_，因此启停和拖动逻辑不会因为左右列表不同而分叉。
     */
    DWORD list_style = WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | WS_VSCROLL_ |
                       LBS_NOTIFY_ | LBS_OWNERDRAWFIXED_ | LBS_HASSTRINGS_ | LBS_NOINTEGRALHEIGHT_;

    /*
     * dev2 故意去掉 WS_EX_CLIENTEDGE：卡片外框已经由父窗口统一绘制，再叠一个系统凹边只会出现“双层边框”。
     * 三个主按钮都改成 OWNERDRAW，让设置、刷新和启动共享同一套扁平视觉。
     */
    g_asi_list = g_ui.CreateWindowExW(0u, (const WCHAR*)L"LISTBOX", (const WCHAR*)L"", list_style,
                                      0,0,0,0,g_main,(HANDLE)(ULONG_PTR)IDC_ASI_LIST_,g_instance,NULL_PTR);
    g_override_list = g_ui.CreateWindowExW(0u, (const WCHAR*)L"LISTBOX", (const WCHAR*)L"", list_style,
                                           0,0,0,0,g_main,(HANDLE)(ULONG_PTR)IDC_OVERRIDE_LIST_,g_instance,NULL_PTR);
    g_about_button = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"关于",
                                           WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                           0,0,0,0,g_main,(HANDLE)(ULONG_PTR)IDC_ABOUT_,g_instance,NULL_PTR);
    g_settings_button = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"设置",
                                              WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                              0,0,0,0,g_main,(HANDLE)(ULONG_PTR)IDC_SETTINGS_,g_instance,NULL_PTR);
    g_refresh_button = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"重新扫描",
                                             WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                             0,0,0,0,g_main,(HANDLE)(ULONG_PTR)IDC_REFRESH_,g_instance,NULL_PTR);
    g_launch_button = g_ui.CreateWindowExW(0u, (const WCHAR*)L"BUTTON", (const WCHAR*)L"启动游戏",
                                            WS_CHILD_ | WS_VISIBLE_ | WS_TABSTOP_ | BS_OWNERDRAW_,
                                            0,0,0,0,g_main,(HANDLE)(ULONG_PTR)IDC_LAUNCH_,g_instance,NULL_PTR);
    if (!g_asi_list || !g_override_list || !g_about_button || !g_settings_button || !g_refresh_button || !g_launch_button) return 0;

    set_font_(g_asi_list, g_font);
    set_font_(g_override_list, g_font);
    set_font_(g_about_button, g_font);
    set_font_(g_settings_button, g_font);
    set_font_(g_refresh_button, g_font);
    set_font_(g_launch_button, g_font);
    g_ui.SendMessageW(g_asi_list, LB_SETITEMHEIGHT_, 0, (LPARAM_)scale_(g_asi_list, 42));
    g_ui.SendMessageW(g_override_list, LB_SETITEMHEIGHT_, 0, (LPARAM_)scale_(g_override_list, 42));

    /*
     * 两个 ListBox 都来自系统同一窗口类，原 WndProc 理论上一样。第一次保存下来，第二个只替换成同一个子类过程。
     * x86 下 WNDPROC 和 LONG 都是 32 位，所以 SetWindowLongW 足够；工程本身也只构建 PE32/i386。
     */
    g_old_list_proc = (WNDPROC_)(ULONG_PTR)(DWORD)g_ui.SetWindowLongW(g_asi_list, GWL_WNDPROC_, (LONG)(ULONG_PTR)list_proc_);
    g_ui.SetWindowLongW(g_override_list, GWL_WNDPROC_, (LONG)(ULONG_PTR)list_proc_);
    return g_old_list_proc != NULL_PTR;
}

UINT LauncherGui_Run(void) {
    /*
     * GUI 顶层入口的顺序刻意固定：加载 GUI API → 设置 DPI → 读取/扫描 Mod → 注册窗口类 → 建主窗口 → 消息循环。
     * 任何一步失败都返回不同代码，让 Launcher 能干净退出；在用户点击“启动游戏”之前不会创建 RPG.exe。
     */
    MSG_ msg;
    int width, height;

    if (!load_ui_api_()) return 20u;
    g_instance = (HINSTANCE)GetModuleHandleW(NULL_PTR);

    /* Windows 10+ 优先 Per-Monitor V2；旧系统没有这个导出时退回传统 DPI Aware。 */
    if (g_ui.SetProcessDpiAwarenessContext) {
        g_ui.SetProcessDpiAwarenessContext((HANDLE)(LONG)-4);
    } else if (g_ui.SetProcessDPIAware) {
        g_ui.SetProcessDPIAware();
    }

    /*
     * 先把磁盘和 mods.ini 收敛成一个可显示模型。这样主窗口第一次出现时，新放进去但尚未登记的 Mod 已经能看见，
     * 不需要用户先点一次“重新扫描”。如果配置有危险错误则直接停止，不展示一个可能与 Core 不一致的列表。
     */
    if (!LauncherModConfig_LoadAndScan(LauncherApp_GetModsRoot())) {
        const WCHAR* detail = LauncherModConfig_GetLastErrorText();
        g_ui.MessageBoxW(NULL_PTR, detail && detail[0] ? detail : (const WCHAR*)L"无法读取 Mod 配置。",
                         kWindowTitle_, MB_ICONERROR_ | MB_OK_);
        unload_ui_api_();
        return 21u;
    }

    /*
     * 自绘窗口类要把工作区画刷挂到 hbrBackground 上，所以主题 GDI 资源必须先于 RegisterClassExW 创建。
     * 失败时不进入半成品 GUI，直接释放已经加载的 USER32/GDI32 并返回单独错误码。
     */
    if (!create_theme_resources_()) {
        destroy_theme_resources_();
        unload_ui_api_();
        return 22u;
    }

    if (!register_classes_()) {
        destroy_theme_resources_();
        unload_ui_api_();
        return 23u;
    }

    /*
     * CreateWindowExW 的第一次尺寸只是让 Windows 建立窗口对象；真正的默认尺寸会在窗口创建后，
     * 通过 get_main_minimum_outer_size_() 精确换算成“1024×640 DIP 客户区 + 当前 DPI 的标题栏/边框”。
     * 这样默认状态和 WM_GETMINMAXINFO 的最小状态使用同一套公式，不会出现默认尺寸反而小于最小尺寸的矛盾。
     */
    width = 1024;
    height = 640;
    g_main = g_ui.CreateWindowExW(WS_EX_CONTROLPARENT_, kMainClass_, kWindowTitle_,
                                   WS_OVERLAPPEDWINDOW_ | WS_CLIPCHILDREN_,
                                   0,0,width,height,NULL_PTR,NULL_PTR,g_instance,NULL_PTR);
    if (!g_main) {
        destroy_theme_resources_();
        unload_ui_api_();
        return 24u;
    }

    recreate_fonts_(g_main);
    if (!create_main_controls_()) {
        g_ui.DestroyWindow(g_main);
        destroy_theme_resources_();
        unload_ui_api_();
        return 25u;
    }
    refresh_all_lists_();

    if (!get_main_minimum_outer_size_(g_main, &width, &height)) {
        /*
         * 正常 Windows 上不应进入这里；如果非客户区换算 API 意外失败，则仍保证至少按 1024×640 DIP 打开，
         * 而不是回退到 dev4 的 960 宽度。后续 WM_GETMINMAXINFO 仍会继续尝试精确换算。
         */
        width = scale_(g_main, 1024);
        height = scale_(g_main, 640);
    }
    center_window_on_cursor_monitor_(g_main, width, height);
    layout_main_();
    redraw_main_now_(g_main);
    g_ui.ShowWindow(g_main, SW_SHOWNORMAL_);
    g_ui.UpdateWindow(g_main);

    /* 标准消息循环：GetMessage 取消息，TranslateMessage 生成键盘字符消息，DispatchMessage 交给对应 WndProc。 */
    while (g_ui.GetMessageW(&msg, NULL_PTR, 0u, 0u) > 0) {
        g_ui.TranslateMessage(&msg);
        g_ui.DispatchMessageW(&msg);
    }

    if (g_font) g_ui.DeleteObject((HGDIOBJ_)g_font);
    if (g_title_font) g_ui.DeleteObject((HGDIOBJ_)g_title_font);
    if (g_panel_font) g_ui.DeleteObject((HGDIOBJ_)g_panel_font);
    g_font = NULL_PTR;
    g_title_font = NULL_PTR;
    g_panel_font = NULL_PTR;
    destroy_theme_resources_();
    unload_ui_api_();
    return g_exit_code;
}
