#include "platform.h"
#include "user32_locale.h"
#include "mod_loader.h"

/*
 * USER32 ANSI 桥 —— 只处理“中文台湾 / CP950”设置真正会受宿主系统 ANSI 代码页影响的文本入口。
 *
 * Locale Emulator 的 User32Hook 很大，是因为它必须兼容任意 GUI 程序、控件、MDI、列表框、剪贴板和消息类型。
 * 《幽城幻剑录》RPG.exe 的实际 USER32 import 已经静态确认，和文字编码直接相关的核心入口只有：
 *   - CreateWindowExA：主窗口标题/类名从 ANSI 进入 USER32；
 *   - MessageBoxA：错误/提示文本从 ANSI 进入 USER32；
 *   - DefWindowProcA：游戏窗口过程把 WM_SETTEXT 等默认文本消息交回 USER32。
 *
 * 本文件不把 LE 1600 行消息表照搬进来，而是按同一个原则独立重写：
 * “CP950 ANSI 字符串先由我们转换成 Unicode，再进入 W 版本 API”，从而绕开宿主 Windows
 * 可能已经缓存的 ANSI USER32 转换环境。
 *
 * 这层与真正 CP950 NLS 并不冲突：NLS 负责 `CP_ACP`/CRT/Kernel32；USER32 桥负责窗口子系统自身的 A/W thunk。
 */

#define WM_SETTEXT_ 0x000Cu
#define USER32_TEXT_CAP_ 2048u

typedef ULONG_PTR WPARAM_;
typedef LONG LPARAM_;
typedef LONG LRESULT_;
typedef void* HMENU_;

typedef HWND (WINAPI *PFN_CreateWindowExA_)(DWORD,LPCSTR,LPCSTR,DWORD,int,int,int,int,HWND,HMENU_,HINSTANCE,LPVOID);
typedef HWND (WINAPI *PFN_CreateWindowExW_)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU_,HINSTANCE,LPVOID);
typedef int (WINAPI *PFN_MessageBoxA_)(HWND,LPCSTR,LPCSTR,UINT);
typedef int (WINAPI *PFN_MessageBoxW_)(HWND,LPCWSTR,LPCWSTR,UINT);
typedef LRESULT_ (WINAPI *PFN_DefWindowProcA_)(HWND,UINT,WPARAM_,LPARAM_);
typedef LRESULT_ (WINAPI *PFN_DefWindowProcW_)(HWND,UINT,WPARAM_,LPARAM_);

static HMODULE g_user32;
static PFN_CreateWindowExA_ g_real_CreateWindowExA;
static PFN_CreateWindowExW_ g_real_CreateWindowExW;
static PFN_MessageBoxA_ g_real_MessageBoxA;
static PFN_MessageBoxW_ g_real_MessageBoxW;
static PFN_DefWindowProcA_ g_real_DefWindowProcA;
static PFN_DefWindowProcW_ g_real_DefWindowProcW;
static int g_ready;

static int aeq_i_(const char* a, const char* b) {
    UINT i = 0u;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca = (char)(ca + ('a' - 'A'));
        if (cb >= 'A' && cb <= 'Z') cb = (char)(cb + ('a' - 'A'));
        if (ca != cb) return 0;
        ++i;
    }
    return a[i] == 0 && b[i] == 0;
}

/*
 * CP950 ANSI -> UTF-16。返回 1 代表转换完成，0 代表输入为空/转换失败。
 * 缓冲区固定在调用栈上，不在 USER32/Loader Lock 附近做堆分配。
 */
static int big5_to_wide_(LPCSTR src, WCHAR* dst, int cap) {
    int n;
    if (!src || !dst || cap <= 0) return 0;
    n = MultiByteToWideChar(CASTLE_CP_BIG5_, 0u, src, -1, dst, cap);
    if (n <= 0) { dst[0] = 0; return 0; }
    return 1;
}

static HWND WINAPI Hook_CreateWindowExA_(DWORD ex_style, LPCSTR class_name, LPCSTR window_name, DWORD style,
                                          int x, int y, int width, int height, HWND parent, HMENU_ menu,
                                          HINSTANCE instance, LPVOID param) {
    WCHAR class_w[256];
    WCHAR title_w[USER32_TEXT_CAP_];
    LPCWSTR class_arg = NULL_PTR;
    LPCWSTR title_arg = NULL_PTR;

    if (!g_real_CreateWindowExW) {
        return g_real_CreateWindowExA ? g_real_CreateWindowExA(ex_style,class_name,window_name,style,x,y,width,height,parent,menu,instance,param) : NULL_PTR;
    }

    /*
     * Win32 允许 class_name 使用 MAKEINTATOM：数值小于等于 0xFFFF 时它不是字符串指针。
     * 这种情况必须原样传给 W 版本，不能把 0x1234 当地址去读字符串。
     */
    if ((ULONG_PTR)class_name <= 0xFFFFu) {
        class_arg = (LPCWSTR)class_name;
    } else if (class_name && big5_to_wide_(class_name, class_w, 256)) {
        class_arg = class_w;
    } else {
        /* 类名转换失败时宁可走原 A 版本，避免创建一个找不到类的窗口。 */
        return g_real_CreateWindowExA ? g_real_CreateWindowExA(ex_style,class_name,window_name,style,x,y,width,height,parent,menu,instance,param) : NULL_PTR;
    }

    if (!window_name) title_arg = NULL_PTR;
    else if (big5_to_wide_(window_name, title_w, (int)USER32_TEXT_CAP_)) title_arg = title_w;
    else return g_real_CreateWindowExA ? g_real_CreateWindowExA(ex_style,class_name,window_name,style,x,y,width,height,parent,menu,instance,param) : NULL_PTR;

    /*
     * 类仍然是游戏通过 RegisterClassExA 注册的 ANSI 类；Windows 会保留它的 ANSI WndProc 语义。
     * 我们只把“创建入口携带的类名/标题”改成 Unicode，避免 USER32 用宿主 ACP 重新解释 Big5 字节。
     */
    return g_real_CreateWindowExW(ex_style,class_arg,title_arg,style,x,y,width,height,parent,menu,instance,param);
}

static int WINAPI Hook_MessageBoxA_(HWND owner, LPCSTR text, LPCSTR caption, UINT type) {
    WCHAR text_w[USER32_TEXT_CAP_];
    WCHAR caption_w[512];
    LPCWSTR tw = NULL_PTR, cw = NULL_PTR;
    if (!g_real_MessageBoxW) return g_real_MessageBoxA ? g_real_MessageBoxA(owner,text,caption,type) : 0;

    if (text) {
        if (!big5_to_wide_(text,text_w,(int)USER32_TEXT_CAP_)) return g_real_MessageBoxA ? g_real_MessageBoxA(owner,text,caption,type) : 0;
        tw = text_w;
    }
    if (caption) {
        if (!big5_to_wide_(caption,caption_w,512)) return g_real_MessageBoxA ? g_real_MessageBoxA(owner,text,caption,type) : 0;
        cw = caption_w;
    }
    return g_real_MessageBoxW(owner,tw,cw,type);
}

static LRESULT_ WINAPI Hook_DefWindowProcA_(HWND hwnd, UINT message, WPARAM_ wparam, LPARAM_ lparam) {
    /*
     * WM_SETTEXT 的 lParam 是 ANSI 字符串。这个消息经常用于运行中更新窗口标题。
     * 对它单独转成 Unicode 再走 DefWindowProcW，避免系统 A->W thunk 使用宿主 ANSI 环境。
     * 其它消息没有明确的字符串负载，本版继续交给真实 DefWindowProcA，避免无意义改写消息语义。
     */
    if (message == WM_SETTEXT_ && lparam != 0 && g_real_DefWindowProcW) {
        WCHAR text_w[USER32_TEXT_CAP_];
        if (big5_to_wide_((LPCSTR)(ULONG_PTR)lparam,text_w,(int)USER32_TEXT_CAP_))
            return g_real_DefWindowProcW(hwnd,message,wparam,(LPARAM_)(ULONG_PTR)text_w);
    }
    return g_real_DefWindowProcA ? g_real_DefWindowProcA(hwnd,message,wparam,lparam) : 0;
}

static DWORD hook_address_for_(const char* dll, const char* fn) {
    if (!aeq_i_(dll,"USER32.dll")) return 0u;
    if (aeq_i_(fn,"CreateWindowExA")) return (DWORD)(SIZE_T)&Hook_CreateWindowExA_;
    if (aeq_i_(fn,"MessageBoxA")) return (DWORD)(SIZE_T)&Hook_MessageBoxA_;
    if (aeq_i_(fn,"DefWindowProcA")) return (DWORD)(SIZE_T)&Hook_DefWindowProcA_;
    return 0u;
}

UINT User32Locale_PatchModule(HMODULE module) {
    /*
     * 和 Overrides/GDI 一样，这里只修改“调用方模块自己的 IAT”，不全局改写 USER32 导出。
     * 这样如果某个无关系统模块依赖 USER32，它不会被我们的 Big5 文本桥影响。
     */
    BYTE* base = (BYTE*)module;
    DWORD pe_off, optional_off, import_rva, desc_rva;
    WORD magic;
    UINT patched = 0u;

    if (!module || *(WORD*)base != IMAGE_DOS_SIGNATURE_) return 0u;
    pe_off = *(DWORD*)(base + 0x3Cu);
    if (*(DWORD*)(base + pe_off) != IMAGE_NT_SIGNATURE_) return 0u;
    optional_off = pe_off + 24u;
    magic = *(WORD*)(base + optional_off);
    if (magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC_) return 0u;
    import_rva = *(DWORD*)(base + optional_off + 96u + IMAGE_DIRECTORY_ENTRY_IMPORT_ * 8u);
    if (!import_rva) return 0u;

    desc_rva = import_rva;
    for (;;) {
        DWORD* desc = (DWORD*)(base + desc_rva);
        DWORD oft = desc[0], name_rva = desc[3], ft = desc[4], idx = 0u;
        const char* dll;
        if (!desc[0] && !desc[1] && !desc[2] && !desc[3] && !desc[4]) break;
        desc_rva += 20u;
        if (!oft || !name_rva || !ft) continue;
        dll = (const char*)(base + name_rva);
        for (;;) {
            DWORD name_thunk = *(DWORD*)(base + oft + idx * 4u);
            DWORD* slot = (DWORD*)(base + ft + idx * 4u);
            const char* fn;
            DWORD hook, oldp = 0u, ignored = 0u;
            if (!name_thunk) break;
            ++idx;
            if (name_thunk & IMAGE_ORDINAL_FLAG32_) continue;
            fn = (const char*)(base + name_thunk + 2u);
            /* 只有 USER32.dll 的三个文本入口会得到非 0 Hook 地址。 */
            hook = hook_address_for_(dll,fn);
            if (!hook || *slot == hook) continue;

            /* IAT 常驻只读页，必须临时解保护；失败时保持原函数，不冒险写坏进程。 */
            if (!VirtualProtect(slot,4u,PAGE_READWRITE_,&oldp)) continue;
            *slot = hook;
            VirtualProtect(slot,4u,oldp,&ignored);
            ++patched;
        }
    }
    if (patched) FlushInstructionCache(GetCurrentProcess(),module,1u);
    return patched;
}

int User32Locale_Initialize(void) {
    HMODULE modules[6];
    UINT i, patched = 0u;

    /*
     * 不主动 LoadLibrary(USER32)。Early 阶段如果 USER32 还没轮到 Windows Loader 映射，
     * 我们只记录“等待”，让原始依赖图继续；EntryPoint 前第二次调用自然会补挂。
     */
    g_user32 = GetModuleHandleW((const WCHAR*)L"user32.dll");
    if (!g_user32) {
        /* Core 的 Early 阶段可能早于 USER32 完成映射；这不是错误，EntryPoint 前会再次调用。 */
        ModLoader_Log((const WCHAR*)L"[Locale/USER32] USER32 尚未可用，本轮暂不安装 ANSI 窗口桥；EntryPoint 前会补挂。");
        return 1;
    }

    if (!g_ready) {
        g_real_CreateWindowExA = (PFN_CreateWindowExA_)GetProcAddress(g_user32,"CreateWindowExA");
        g_real_CreateWindowExW = (PFN_CreateWindowExW_)GetProcAddress(g_user32,"CreateWindowExW");
        g_real_MessageBoxA = (PFN_MessageBoxA_)GetProcAddress(g_user32,"MessageBoxA");
        g_real_MessageBoxW = (PFN_MessageBoxW_)GetProcAddress(g_user32,"MessageBoxW");
        g_real_DefWindowProcA = (PFN_DefWindowProcA_)GetProcAddress(g_user32,"DefWindowProcA");
        g_real_DefWindowProcW = (PFN_DefWindowProcW_)GetProcAddress(g_user32,"DefWindowProcW");
        if (!g_real_CreateWindowExA || !g_real_CreateWindowExW || !g_real_MessageBoxA || !g_real_MessageBoxW || !g_real_DefWindowProcA || !g_real_DefWindowProcW) {
            ModLoader_Log((const WCHAR*)L"[Locale/USER32错误] 无法解析 ANSI/Unicode 窗口文本桥所需的 USER32 API。");
            return 0;
        }
        g_ready = 1;
    }

    /*
     * dev9 根据 dev5 的实机成功结果，把 ddraw.dll 重新纳入 USER32 Locale 桥。
     * cnc-ddraw 会参与游戏窗口创建与消息链；如果只修 RPG.exe 而放任 wrapper 内部继续按系统 ACP(例如 936)
     * 调用 CreateWindowExA/DefWindowProcA，窗口标题等 ANSI 文本仍可能在 wrapper 一侧被错误解释。
     *
     * 注意：这不是接管 cnc-ddraw 的键盘 Hook，也不是 Hook SetWindowsHookExA。这里只处理本文件原本就负责的
     * CreateWindowExA / MessageBoxA / DefWindowProcA 三个文本兼容入口，恢复 dev5 已验证的行为。
     */
    modules[0] = GetModuleHandleW(NULL_PTR);
    modules[1] = GetModuleHandleW((const WCHAR*)L"BASS.dll");
    modules[2] = GetModuleHandleW((const WCHAR*)L"binkw32.dll");
    modules[3] = GetModuleHandleW((const WCHAR*)L"ijl10.dll");
    modules[4] = GetModuleHandleW((const WCHAR*)L"ddraw.dll");
    modules[5] = GetModuleHandleW((const WCHAR*)L"SHLWAPI.dll");
    for (i = 0u; i < 6u; ++i) if (modules[i]) patched += User32Locale_PatchModule(modules[i]);

    if (patched)
        ModLoader_Log((const WCHAR*)L"[Locale/USER32] 已安装 CP950 ANSI 窗口标题/MessageBox/WM_SETTEXT Unicode 桥。");
    else
        ModLoader_Log((const WCHAR*)L"[Locale/USER32] 当前模块没有新的 USER32 文本 IAT 槽需要改写，已有 Hook 保持有效。");
    return 1;
}
