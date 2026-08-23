#include "platform.h"
#include "gdi_locale.h"
#include "mod_loader.h"

/*
 * GDI 默认字符集桥。
 *
 * Locale Emulator 在目标 ACP=950 时会把 DefaultCharset 设为 CHINESEBIG5_CHARSET(136)，并在字体创建时
 * 把 ANSI_CHARSET/DEFAULT_CHARSET 替换为这个目标字符集。这里只独立重写这一个与用户设置直接相关的行为，
 * 不搬 LE 的字体枚举缓存、字体替换、Glyph 修正等无关功能。
 *
 * 《幽城》的 RPG.exe 本身没有静态导入 GDI32，所以这层很可能“0 个槽位”；仍然保留它是因为某些
 * 静态依赖/后续代码 Mod 可能用标准 CreateFont* API。日志会明确告诉我们本轮是否真的命中。
 */

#define ANSI_CHARSET_ 0u
#define DEFAULT_CHARSET_ 1u
#define LF_FACESIZE_ 32u

typedef void* HFONT_;

typedef struct LOGFONTA_ {
    LONG lfHeight, lfWidth, lfEscapement, lfOrientation, lfWeight;
    BYTE lfItalic, lfUnderline, lfStrikeOut, lfCharSet;
    BYTE lfOutPrecision, lfClipPrecision, lfQuality, lfPitchAndFamily;
    char lfFaceName[LF_FACESIZE_];
} LOGFONTA_;

typedef struct LOGFONTW_ {
    LONG lfHeight, lfWidth, lfEscapement, lfOrientation, lfWeight;
    BYTE lfItalic, lfUnderline, lfStrikeOut, lfCharSet;
    BYTE lfOutPrecision, lfClipPrecision, lfQuality, lfPitchAndFamily;
    WCHAR lfFaceName[LF_FACESIZE_];
} LOGFONTW_;

typedef HFONT_ (WINAPI *PFN_CreateFontIndirectA_)(const LOGFONTA_*);
typedef HFONT_ (WINAPI *PFN_CreateFontIndirectW_)(const LOGFONTW_*);
typedef HFONT_ (WINAPI *PFN_CreateFontA_)(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCSTR);
typedef HFONT_ (WINAPI *PFN_CreateFontW_)(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCWSTR);

static HMODULE g_gdi32;
static PFN_CreateFontIndirectA_ g_real_CreateFontIndirectA;
static PFN_CreateFontIndirectW_ g_real_CreateFontIndirectW;
static PFN_CreateFontA_ g_real_CreateFontA;
static PFN_CreateFontW_ g_real_CreateFontW;
static int g_ready;

static int aeq_i_(const char* a, const char* b) {
    UINT i = 0u;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        char ca=a[i], cb=b[i];
        if (ca>='A'&&ca<='Z') ca=(char)(ca+('a'-'A'));
        if (cb>='A'&&cb<='Z') cb=(char)(cb+('a'-'A'));
        if (ca!=cb) return 0;
        ++i;
    }
    return a[i]==0 && b[i]==0;
}

static BYTE normalize_charset_(BYTE charset) {
    /*
     * 只有“程序没有明确指定字符集”的两种情况才替换。
     * 如果游戏自己指定了 SHIFTJIS_CHARSET、GB2312_CHARSET 等其它值，就说明它主动选择了别的语义；
     * 此时必须原样返回，不能因为当前 Profile 是台湾 Big5 就强行覆盖。
     */
    if (charset == ANSI_CHARSET_ || charset == DEFAULT_CHARSET_) {
        return (BYTE)CASTLE_CHARSET_BIG5_;
    }
    return charset;
}

static HFONT_ WINAPI Hook_CreateFontIndirectA_(const LOGFONTA_* in) {
    LOGFONTA_ local;
    if (!g_real_CreateFontIndirectA) return NULL_PTR;
    if (!in) return g_real_CreateFontIndirectA(in);
    local = *in;
    local.lfCharSet = normalize_charset_(local.lfCharSet);
    return g_real_CreateFontIndirectA(&local);
}

static HFONT_ WINAPI Hook_CreateFontIndirectW_(const LOGFONTW_* in) {
    LOGFONTW_ local;
    if (!g_real_CreateFontIndirectW) return NULL_PTR;
    if (!in) return g_real_CreateFontIndirectW(in);
    local = *in;
    local.lfCharSet = normalize_charset_(local.lfCharSet);
    return g_real_CreateFontIndirectW(&local);
}

static HFONT_ WINAPI Hook_CreateFontA_(int h,int w,int e,int o,int weight,DWORD italic,DWORD underline,DWORD strike,
                                       DWORD charset,DWORD outp,DWORD clipp,DWORD quality,DWORD pitch,LPCSTR face) {
    if (!g_real_CreateFontA) return NULL_PTR;
    charset = (DWORD)normalize_charset_((BYTE)charset);
    return g_real_CreateFontA(h,w,e,o,weight,italic,underline,strike,charset,outp,clipp,quality,pitch,face);
}

static HFONT_ WINAPI Hook_CreateFontW_(int h,int w,int e,int o,int weight,DWORD italic,DWORD underline,DWORD strike,
                                       DWORD charset,DWORD outp,DWORD clipp,DWORD quality,DWORD pitch,LPCWSTR face) {
    if (!g_real_CreateFontW) return NULL_PTR;
    charset = (DWORD)normalize_charset_((BYTE)charset);
    return g_real_CreateFontW(h,w,e,o,weight,italic,underline,strike,charset,outp,clipp,quality,pitch,face);
}

static DWORD hook_address_for_(const char* dll, const char* fn) {
    if (!aeq_i_(dll,"GDI32.dll")) return 0u;
    if (aeq_i_(fn,"CreateFontIndirectA")) return (DWORD)(SIZE_T)&Hook_CreateFontIndirectA_;
    if (aeq_i_(fn,"CreateFontIndirectW")) return (DWORD)(SIZE_T)&Hook_CreateFontIndirectW_;
    if (aeq_i_(fn,"CreateFontA")) return (DWORD)(SIZE_T)&Hook_CreateFontA_;
    if (aeq_i_(fn,"CreateFontW")) return (DWORD)(SIZE_T)&Hook_CreateFontW_;
    return 0u;
}

UINT GdiLocale_PatchModule(HMODULE module) {
    /*
     * 这里直接遍历目标模块自己的 PE Import Table。
     * 原因是我们不希望全局 Detour GDI32：只让《幽城》及它的已知依赖进入 Big5 默认字符集桥，
     * 其它系统 DLL / 第三方软件继续按 Windows 原样运行，问题边界更清楚。
     */
    BYTE* base=(BYTE*)module;
    DWORD pe_off,optional_off,import_rva,desc_rva;
    WORD magic;
    UINT patched=0u;
    if (!module || *(WORD*)base!=IMAGE_DOS_SIGNATURE_) return 0u;
    pe_off=*(DWORD*)(base+0x3Cu);
    if (*(DWORD*)(base+pe_off)!=IMAGE_NT_SIGNATURE_) return 0u;
    optional_off=pe_off+24u;
    magic=*(WORD*)(base+optional_off);
    if (magic!=IMAGE_NT_OPTIONAL_HDR32_MAGIC_) return 0u;
    import_rva=*(DWORD*)(base+optional_off+96u+IMAGE_DIRECTORY_ENTRY_IMPORT_*8u);
    if (!import_rva) return 0u;
    desc_rva=import_rva;
    for (;;) {
        DWORD* desc=(DWORD*)(base+desc_rva);
        DWORD oft=desc[0],name_rva=desc[3],ft=desc[4],idx=0u;
        const char* dll;
        if (!desc[0]&&!desc[1]&&!desc[2]&&!desc[3]&&!desc[4]) break;
        desc_rva+=20u;
        if (!oft||!name_rva||!ft) continue;
        /* 当前 Import Descriptor 对应的 DLL 名，例如 GDI32.dll。 */
        dll=(const char*)(base+name_rva);
        for (;;) {
            DWORD name_thunk=*(DWORD*)(base+oft+idx*4u);
            DWORD* slot=(DWORD*)(base+ft+idx*4u);
            const char* fn;
            DWORD hook,oldp=0u,ignored=0u;
            if (!name_thunk) break;
            ++idx;
            if (name_thunk&IMAGE_ORDINAL_FLAG32_) continue;
            fn=(const char*)(base+name_thunk+2u);
            /*
             * hook_address_for_ 只会为 GDI32 的四个标准 CreateFont* 返回地址。
             * 其它导入得到 0 后直接跳过，所以这段代码不会误改别的 API。
             */
            hook=hook_address_for_(dll,fn);
            if (!hook||*slot==hook) continue;

            /*
             * IAT 在正常 PE 中通常是只读页；写入前临时改成 PAGE_READWRITE，写完立即恢复。
             * 如果 VirtualProtect 失败，宁可保留原函数，也不能直接向只读页写造成崩溃。
             */
            if (!VirtualProtect(slot,4u,PAGE_READWRITE_,&oldp)) continue;
            *slot=hook;
            VirtualProtect(slot,4u,oldp,&ignored);
            ++patched;
        }
    }
    if (patched) FlushInstructionCache(GetCurrentProcess(),module,1u);
    return patched;
}

int GdiLocale_Initialize(void) {
    HMODULE modules[6];
    UINT i,patched=0u;
    /*
     * Early Core 进入时 GDI32 不一定已经映射，所以先只查询，不主动 LoadLibrary。
     * 主动加载会改变 Windows 原始静态依赖时序；这里选择等待 EntryPoint 前第二次初始化。
     */
    g_gdi32=GetModuleHandleW((const WCHAR*)L"gdi32.dll");
    if (!g_gdi32) {
        ModLoader_Log((const WCHAR*)L"[Locale/GDI] GDI32 尚未装入；当前没有默认字体字符集入口需要处理。");
        return 1;
    }
    if (!g_ready) {
        g_real_CreateFontIndirectA=(PFN_CreateFontIndirectA_)GetProcAddress(g_gdi32,"CreateFontIndirectA");
        g_real_CreateFontIndirectW=(PFN_CreateFontIndirectW_)GetProcAddress(g_gdi32,"CreateFontIndirectW");
        g_real_CreateFontA=(PFN_CreateFontA_)GetProcAddress(g_gdi32,"CreateFontA");
        g_real_CreateFontW=(PFN_CreateFontW_)GetProcAddress(g_gdi32,"CreateFontW");
        if (!g_real_CreateFontIndirectA||!g_real_CreateFontIndirectW||!g_real_CreateFontA||!g_real_CreateFontW) {
            ModLoader_Log((const WCHAR*)L"[Locale/GDI错误] 无法解析标准字体创建 API。");
            return 0;
        }
        g_ready=1;
    }
    /*
     * 只扫描游戏主模块和与窗口/字体链同一生命周期的已知模块。
     * dev9 恢复 dev5 对 ddraw.dll 的 GDI 字符集桥，因为用户已实测 dev6 排除 ddraw 后区域环境表象回退。
     * 这里仍然只把 ANSI/DEFAULT_CHARSET 规范到 CHINESEBIG5_CHARSET，不触碰 DirectDraw 渲染接口。
     * 即使某个模块没有 GDI32 导入，PatchModule 返回 0 也完全安全。
     */
    modules[0]=GetModuleHandleW(NULL_PTR);
    modules[1]=GetModuleHandleW((const WCHAR*)L"BASS.dll");
    modules[2]=GetModuleHandleW((const WCHAR*)L"binkw32.dll");
    modules[3]=GetModuleHandleW((const WCHAR*)L"ijl10.dll");
    modules[4]=GetModuleHandleW((const WCHAR*)L"ddraw.dll");
    modules[5]=GetModuleHandleW((const WCHAR*)L"user32.dll");
    for (i=0u;i<6u;++i) if (modules[i]) patched+=GdiLocale_PatchModule(modules[i]);
    if (patched)
        ModLoader_Log((const WCHAR*)L"[Locale/GDI] 已把标准字体的 ANSI/DEFAULT_CHARSET 映射为 CHINESEBIG5_CHARSET(136)。");
    else
        ModLoader_Log((const WCHAR*)L"[Locale/GDI] 当前模块没有 CreateFont* IAT 槽；CP950 默认字符集规则保持待用。");
    return 1;
}
