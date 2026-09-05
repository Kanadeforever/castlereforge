#include "platform.h"
#include "game_audit.h"

/*
 * 《幽城幻剑录》原版游戏运行审计层。
 *
 * ================================ 为什么要单独做 game.log ================================
 *
 * v0.2.8 已经证明“Loader 自己的状态”和“游戏自己的运行状态”不能继续写在一份日志里：
 *   - modloader.log 需要短而明确，适合判断 Mod 顺序、Locale、Overrides、ASI 是否成功；
 *   - 排查原版游戏生命周期问题时，却需要大量 I/O、状态边界与异常现场。
 *
 * 如果把两类内容继续混在一起，用户玩十分钟以后会得到一份几万行的 modloader.log，
 * 真正重要的 Loader 错误反而会被淹没。因此从 v0.2.9 开始，本文件只写：
 *
 *     mods\game.log
 *
 * 这份日志是一台“旁路记录仪”。它不修存档、不改变 Event VM、不吞异常，也不替游戏决定路径。
 * 它只把原版已经发生的事情记录下来，方便之后把：
 *
 *     启动 -> 新游戏 -> 存档 -> 读档 -> 再新游戏 -> 崩溃
 *
 * 按时间顺序还原。
 *
 * ================================ 当前已知的原版状态边界 ================================
 *
 * 下列地址不是本轮猜出来的，而是“Godot 兼容引擎 固化40”已经静态闭合、并再次与当前 RPG.exe.org
 * 机器码核对过的 Oracle。状态 Hook 安装前还会逐字节比较函数入口；任何一处不匹配都只跳过该 Hook，
 * 绝不为了记录日志向未知版本硬写 JMP。
 *
 *   0x0044802D  新游戏分支开始；随后以字符串 "NewGame" 调 0x0043B510
 *   0x0043B360  TSF Writer
 *   0x0043B510  TSF Reader
 *   0x0043ACD0  Runtime Manager -> 固定 Core 序列化
 *   0x0043AB30  固定 Core -> Runtime Manager 反序列化
 *   0x0043AE80  读档后角色 ID -> 运行时角色指针重建
 *   0x0044B130  50 字节 Scene Load Descriptor 写入 World 块
 *   0x0044B1F0  根据 World 块重建场景/Actor/世界状态
 *   0x0040B230  Event 按 ID 激活
 *   0x0040B340  active Event VM 上下文清理；注意它不清 deferred continuation 三元组
 *   0x0040D780  带世界复位的场景装载路径
 *   0x0040D8A0  场景装载路径
 *   0x0040DAA0  存档槽/Interface 入口
 *
 * 特别值得关注的是：固化40已经证明 TSF 不保存完整 Event VM transient；并且 0x0040B340 会清 active Event
 * ID/PC，却不会清 0x0089F7F8 / FC / 800 的 deferred continuation。用户当前要追的
 * “存/读档以后再新游戏崩溃”正适合在这些边界抓快照，看看旧运行期对象或续接状态是否跨生命周期残留。
 */

#define GAME_AUDIT_PATH_CAP_ 2048u
#define GAME_AUDIT_LINE_CAP_ 4096u
#define GAME_AUDIT_MODULE_MAX_ 8u
#define GAME_AUDIT_HANDLE_MAX_ 512u
#define GAME_AUDIT_STATE_HOOK_MAX_ 16u
#define GAME_IMAGE_BASE_ORACLE_ 0x00400000u

/* AddVectoredExceptionHandler 的回调返回这个值表示“我只观察，不处理，请继续交给游戏/系统原来的异常链”。 */
#define EXCEPTION_CONTINUE_SEARCH_ 0L
#define EXCEPTION_ACCESS_VIOLATION_ 0xC0000005u
#define EXCEPTION_ILLEGAL_INSTRUCTION_ 0xC000001Du
#define EXCEPTION_STACK_OVERFLOW_ 0xC00000FDu
#define EXCEPTION_INT_DIVIDE_BY_ZERO_ 0xC0000094u
#define EXCEPTION_PRIV_INSTRUCTION_ 0xC0000096u

/*
 * 这里使用 _ReturnAddress 只为了知道“哪一个原版模块调用了被 Hook 的 Win32 API”。
 * 它不会拿这个地址做控制流跳转，因此即使编译器优化改变栈布局，也不会破坏游戏执行。
 */
void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)

typedef struct GAME_AUDIT_MODULE_ {
    BYTE* base;
    DWORD size;
    const WCHAR* name;
} GAME_AUDIT_MODULE_;

typedef struct GAME_AUDIT_HANDLE_ {
    HANDLE handle;
    WCHAR path[GAME_AUDIT_PATH_CAP_];
    DWORD access;
    DWORD creation;
    DWORD open_tick;
    DWORD read_calls;
    DWORD read_bytes;
    DWORD write_calls;
    DWORD write_bytes;
    DWORD logical_offset;
    int detailed_reads;
    /*
     * FDebug.txt 是原版自身的高频调试输出。它每次写入只有十几个字节，却会反复打开/写/关闭，
     * 如果逐条记录会让真正的地图、存档、视频 I/O 被几千行噪音淹没。quiet_fdebug=1 时仍完整统计，
     * 只是把成功操作收进 5 秒聚合窗口；失败操作仍立即逐条展开。
     */
    int quiet_fdebug;
} GAME_AUDIT_HANDLE_;

typedef struct FDEBUG_AGGREGATE_ {
    DWORD window_start_tick;
    DWORD last_tick;
    DWORD open_calls;
    DWORD write_calls;
    DWORD write_bytes;
    DWORD close_calls;
} FDEBUG_AGGREGATE_;

/*
 * loose -> DAT 不是通过一个显式“fallback API”完成，而是游戏先探测散装文件，失败后很快去对应 DAT SetFilePointer。
 * 为了让日志更容易读，我们只做一个“同线程 + 500ms 内”的时间邻接候选，不把它冒充已经静态证明的调用关系。
 */
typedef struct FALLBACK_PENDING_ {
    DWORD thread_id;
    DWORD tick;
    WCHAR loose_path[GAME_AUDIT_PATH_CAP_];
} FALLBACK_PENDING_;

typedef struct GAME_STATE_SNAPSHOT_ {
    DWORD active_controller;      /* 0x0089F804：探索侧当前控制器/上下文指针，按原始值记录。 */
    DWORD active_event_id;        /* 0x0089F808：当前 active Event ID。 */
    DWORD event_pc;               /* 0x0089F810：当前 Event 指令/PC 相关槽。 */
    BYTE  yield_flag;             /* 0x0089F81D：Event yield/continue 字节。 */
    BYTE  blocked_flag;           /* 0x0089F81E：相关 blocked/yield 字节。 */
    WORD  reserved0;
    DWORD command_ptr;            /* 0x0089F830：当前 Command 指针。 */
    DWORD wait_counter;           /* 0x0089F834：等待计数。 */
    DWORD saved_event_id;         /* 0x0089F7F8：deferred saved Event ID。 */
    DWORD resume_key_a;           /* 0x0089F7FC：deferred continuation key A。 */
    DWORD resume_key_b;           /* 0x0089F800：deferred continuation key B。 */
    DWORD world_async_result;     /* 0x0089F7D0：world-item 异步结果通道。 */
    DWORD interface_state;        /* 0x0089F818：0x40DAA0 直接读取的 Interface 状态槽。 */
    DWORD battle_container;       /* 0x008B01F0：Battle 容器指针。 */
    DWORD battle_result_ui;       /* 0x0089FCC8：战斗结果 UI 对象指针。 */
    DWORD battle_gate_raw;        /* 0x004696DC：战斗门控全局，语义未完全闭合，因此只标 raw。 */
    DWORD rng_state;              /* 0x0046E29C：已确认 RNG 状态。 */
    DWORD data_center_ptr;        /* 0x008B01EC：DataCenter/世界侧全局对象指针。 */
    DWORD manager_ptr;            /* 0x008E1C48：大型全局管理对象指针；不要过度命名具体类。 */
    DWORD manager_plus_100;       /* manager+0x100：NewGame 分支会在调用 Reader 前写 1。 */
    DWORD world_block_ptr;        /* 0x00978508：World/Scene Load Descriptor 所在块指针。 */
} GAME_STATE_SNAPSHOT_;

typedef struct GAME_STATE_HOOK_ {
    DWORD oracle_va;
    BYTE expected[8];
    BYTE expected_len;
    BYTE patch_len;
    BYTE marker_id;
    BYTE installed;
    BYTE* trampoline;
    BYTE* stub;
} GAME_STATE_HOOK_;

/*
 * x86 CONTEXT 的这部分布局来自 Win32 i386 ABI。
 * 我们只读取通用寄存器；浮点/扩展寄存器即使存在也不解释，避免把日志变成无用的几百字节 dump。
 */
typedef struct FLOATING_SAVE_AREA32_ {
    DWORD ControlWord, StatusWord, TagWord, ErrorOffset, ErrorSelector, DataOffset, DataSelector;
    BYTE RegisterArea[80];
    DWORD Cr0NpxState;
} FLOATING_SAVE_AREA32_;

typedef struct CONTEXT32_ {
    DWORD ContextFlags;
    DWORD Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
    FLOATING_SAVE_AREA32_ FloatSave;
    DWORD SegGs, SegFs, SegEs, SegDs;
    DWORD Edi, Esi, Ebx, Edx, Ecx, Eax;
    DWORD Ebp, Eip, SegCs, EFlags, Esp, SegSs;
    BYTE ExtendedRegisters[512];
} CONTEXT32_;

typedef struct EXCEPTION_POINTERS_AUDIT_ {
    EXCEPTION_RECORD32_* ExceptionRecord;
    CONTEXT32_* ContextRecord;
} EXCEPTION_POINTERS_AUDIT_;

static HANDLE g_log = INVALID_HANDLE_VALUE_;
static DWORD g_start_tick;
static volatile LONG g_log_lock;
static GAME_AUDIT_MODULE_ g_modules[GAME_AUDIT_MODULE_MAX_];
static UINT g_module_count;
static GAME_AUDIT_HANDLE_ g_handles[GAME_AUDIT_HANDLE_MAX_];
static HMODULE g_game_module;
static DWORD g_game_image_size;
static LPVOID g_vectored_handler;
static UINT g_state_hook_count;
static int g_ready;
static FDEBUG_AGGREGATE_ g_fdebug;
static FALLBACK_PENDING_ g_fallback_pending[16];

/*
 * 状态 Hook 表中的 patch_len 必须刚好覆盖完整 x86 指令，并且至少 5 字节（E9 rel32 的长度）。
 * expected 只保存被覆盖的那几字节；安装时逐字节核对，防止版本不符。
 */
static GAME_STATE_HOOK_ g_state_hooks[] = {
    {0x0044802Du, {0x8B,0x15,0x48,0x1C,0x8E,0x00,0,0}, 6, 6,  1, 0, NULL_PTR, NULL_PTR}, /* NEW_GAME_BEGIN */
    {0x0043B360u, {0x6A,0xFF,0x64,0xA1,0x00,0x00,0x00,0x00}, 8, 8,  2, 0, NULL_PTR, NULL_PTR}, /* SAVE_WRITER */
    {0x0043B510u, {0x64,0xA1,0x00,0x00,0x00,0x00,0,0}, 6, 6,  3, 0, NULL_PTR, NULL_PTR}, /* LOAD_READER */
    {0x0043ACD0u, {0x51,0x53,0x8B,0x5C,0x24,0x0C,0,0}, 6, 6,  4, 0, NULL_PTR, NULL_PTR}, /* SERIALIZE_CORE */
    {0x0043AB30u, {0x51,0x53,0x8B,0x5C,0x24,0x0C,0,0}, 6, 6,  5, 0, NULL_PTR, NULL_PTR}, /* DESERIALIZE_CORE */
    {0x0043AE80u, {0x56,0x57,0x8D,0x71,0x14,0,0,0}, 5, 5,  6, 0, NULL_PTR, NULL_PTR}, /* REBUILD_ROLE_PTRS */
    {0x0044B130u, {0x56,0x8B,0x74,0x24,0x08,0,0,0}, 5, 5,  7, 0, NULL_PTR, NULL_PTR}, /* COPY_SCENE_DESCRIPTOR */
    {0x0044B1F0u, {0x6A,0xFF,0x68,0x7B,0xFF,0x45,0x00,0}, 7, 7,  8, 0, NULL_PTR, NULL_PTR}, /* RESTORE_WORLD */
    {0x0040B230u, {0x56,0x8B,0x74,0x24,0x08,0,0,0}, 5, 5,  9, 0, NULL_PTR, NULL_PTR}, /* EVENT_ACTIVATE */
    {0x0040B340u, {0xA0,0x77,0xF6,0x46,0x00,0,0,0}, 5, 5, 10, 0, NULL_PTR, NULL_PTR}, /* EVENT_CLEAR */
    {0x0040D780u, {0x6A,0xFF,0x68,0xCB,0xD1,0x45,0x00,0}, 7, 7, 11, 0, NULL_PTR, NULL_PTR}, /* LOAD_SCENE_RESET */
    {0x0040D8A0u, {0x6A,0xFF,0x68,0xEB,0xD1,0x45,0x00,0}, 7, 7, 12, 0, NULL_PTR, NULL_PTR}, /* LOAD_SCENE */
    {0x0040DAA0u, {0x6A,0xFF,0x68,0x0B,0xD2,0x45,0x00,0}, 7, 7, 13, 0, NULL_PTR, NULL_PTR}  /* SAVE_SLOT_UI */
};

/* ---------- 极小字符串与数字格式化：不依赖 CRT ---------- */

static UINT wlen_(const WCHAR* s) {
    UINT n = 0u;
    if (!s) return 0u;
    while (s[n]) ++n;
    return n;
}

static int wcopy_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT i = 0u;
    if (!dst || !src || !cap) return 0;
    while (src[i]) {
        if (i + 1u >= cap) return 0;
        dst[i] = src[i];
        ++i;
    }
    dst[i] = 0;
    return 1;
}

static int wappend_(WCHAR* dst, UINT cap, const WCHAR* src) {
    UINT d = wlen_(dst), i = 0u;
    if (!dst || !src || d >= cap) return 0;
    while (src[i]) {
        if (d + i + 1u >= cap) return 0;
        dst[d + i] = src[i];
        ++i;
    }
    dst[d + i] = 0;
    return 1;
}

static void append_u32_dec_(WCHAR* dst, UINT cap, DWORD value) {
    WCHAR rev[16];
    WCHAR out[16];
    UINT n = 0u, i;
    do {
        rev[n++] = (WCHAR)('0' + (value % 10u));
        value /= 10u;
    } while (value && n < 15u);
    for (i = 0u; i < n; ++i) out[i] = rev[n - 1u - i];
    out[n] = 0;
    wappend_(dst, cap, out);
}

static WCHAR hex_digit_(DWORD value) {
    value &= 0xFu;
    return (WCHAR)(value < 10u ? ('0' + value) : ('A' + value - 10u));
}

static void append_u32_hex_(WCHAR* dst, UINT cap, DWORD value) {
    WCHAR out[11];
    UINT i;
    out[0] = (WCHAR)'0';
    out[1] = (WCHAR)'x';
    for (i = 0u; i < 8u; ++i) {
        DWORD shift = (7u - i) * 4u;
        out[2u + i] = hex_digit_(value >> shift);
    }
    out[10] = 0;
    wappend_(dst, cap, out);
}

static void append_ptr_hex_(WCHAR* dst, UINT cap, LPCVOID ptr) {
    append_u32_hex_(dst, cap, (DWORD)(SIZE_T)ptr);
}

/*
 * GetLastError 只有在 API 文档定义“失败时读取”时才是错误信息。很多 Win32 API 成功后不会主动清零 LastError，
 * 因此 v0.2.9 把上一次失败残留的 2/183 也打印在成功行里，容易让人误判。
 * v0.2.11 统一规则：失败才打印真实数字；成功写 error=-，明确告诉读日志的人“这里没有失败错误码”。
 */
static void append_error_result_(WCHAR* line, UINT cap, int failed, DWORD error) {
    wappend_(line, cap, (const WCHAR*)L" error=");
    if (failed) append_u32_dec_(line, cap, error);
    else wappend_(line, cap, (const WCHAR*)L"-");
}

/*
 * CreateFileA/W 是一个少数例外：OPEN_ALWAYS / CREATE_ALWAYS 成功时，ERROR_ALREADY_EXISTS=183 本身有业务语义，
 * 可以告诉我们目标文件此前是否已经存在。这里把它写成 info=183，而不是冒充失败的 error=183。
 */
static void append_create_result_(WCHAR* line, UINT cap, HANDLE result, DWORD creation, DWORD error) {
    int failed = result == INVALID_HANDLE_VALUE_;
    append_error_result_(line, cap, failed, error);
    if (!failed && error == ERROR_ALREADY_EXISTS_ && (creation == OPEN_ALWAYS_ || creation == CREATE_ALWAYS_))
        wappend_(line, cap, (const WCHAR*)L" info=ERROR_ALREADY_EXISTS(183)");
}

/*
 * game.log 会被 BASS 等原版依赖线程同时写入，所以用一个极小自旋锁保护“整行写入”。
 * 普通路径允许 Sleep(0) 把时间片让出去；异常处理器则使用 try-lock，绝不因为日志锁造成二次死锁。
 */
static void log_lock_(void) {
    while (InterlockedExchange(&g_log_lock, 1L) != 0L) Sleep(0u);
}

static void log_unlock_(void) {
    InterlockedExchange(&g_log_lock, 0L);
}

static void write_wide_line_unlocked_(const WCHAR* line) {
    char utf8[GAME_AUDIT_LINE_CAP_ * 3u];
    int bytes;
    DWORD wrote = 0u;
    static const char crlf[2] = {'\r','\n'};

    if (g_log == INVALID_HANDLE_VALUE_ || !line) return;
    bytes = WideCharToMultiByte(CP_UTF8_, 0u, line, -1, utf8, (int)sizeof(utf8), NULL_PTR, NULL_PTR);
    if (bytes <= 0) return;
    /* bytes 包含结尾 NUL；文本文件里不写这个 NUL，只写真正内容。 */
    if (bytes > 1) WriteFile(g_log, utf8, (DWORD)(bytes - 1), &wrote, NULL_PTR);
    WriteFile(g_log, crlf, 2u, &wrote, NULL_PTR);
}

static void log_line_(const WCHAR* category, const WCHAR* body) {
    WCHAR line[GAME_AUDIT_LINE_CAP_];
    DWORD elapsed;
    line[0] = 0;
    if (!g_ready || g_log == INVALID_HANDLE_VALUE_) return;

    elapsed = GetTickCount() - g_start_tick;
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"[+");
    append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, elapsed);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"ms][T");
    append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, GetCurrentThreadId());
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"][");
    wappend_(line, GAME_AUDIT_LINE_CAP_, category ? category : (const WCHAR*)L"INFO");
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"] ");
    wappend_(line, GAME_AUDIT_LINE_CAP_, body ? body : (const WCHAR*)L"");

    log_lock_();
    write_wide_line_unlocked_(line);
    log_unlock_();
}

/* ---------- 原版模块注册与调用来源判断 ---------- */

static DWORD pe_image_size_(HMODULE module) {
    BYTE* base = (BYTE*)module;
    DWORD pe;
    if (!module || *(WORD*)base != IMAGE_DOS_SIGNATURE_) return 0u;
    pe = *(DWORD*)(base + 0x3Cu);
    if (*(DWORD*)(base + pe) != IMAGE_NT_SIGNATURE_) return 0u;
    /* PE32 OptionalHeader 的 SizeOfImage 位于 OptionalHeader+56。 */
    return *(DWORD*)(base + pe + 4u + 20u + 56u);
}

static void register_module_(HMODULE module, const WCHAR* name) {
    DWORD size;
    if (!module || g_module_count >= GAME_AUDIT_MODULE_MAX_) return;
    size = pe_image_size_(module);
    if (!size) return;
    g_modules[g_module_count].base = (BYTE*)module;
    g_modules[g_module_count].size = size;
    g_modules[g_module_count].name = name;
    ++g_module_count;
}

static const GAME_AUDIT_MODULE_* find_caller_module_(LPVOID caller) {
    BYTE* p = (BYTE*)caller;
    UINT i;
    if (!caller) return NULL_PTR;
    for (i = 0u; i < g_module_count; ++i) {
        if (p >= g_modules[i].base && p < g_modules[i].base + g_modules[i].size) return &g_modules[i];
    }
    return NULL_PTR;
}

static void append_caller_(WCHAR* line, UINT cap, LPVOID caller) {
    const GAME_AUDIT_MODULE_* module = find_caller_module_(caller);
    if (!module) {
        wappend_(line, cap, (const WCHAR*)L"caller=非原版模块");
        return;
    }
    wappend_(line, cap, (const WCHAR*)L"caller=");
    wappend_(line, cap, module->name);
    wappend_(line, cap, (const WCHAR*)L"+");
    append_u32_hex_(line, cap, (DWORD)((BYTE*)caller - module->base));
}

/* ---------- 文件句柄状态表 ---------- */

static int ends_with_i_(const WCHAR* path, const WCHAR* suffix) {
    UINT a = wlen_(path), b = wlen_(suffix), i;
    if (b > a) return 0;
    for (i = 0u; i < b; ++i) {
        WCHAR x = path[a - b + i], y = suffix[i];
        if (x >= (WCHAR)'A' && x <= (WCHAR)'Z') x = (WCHAR)(x + 32);
        if (y >= (WCHAR)'A' && y <= (WCHAR)'Z') y = (WCHAR)(y + 32);
        if (x != y) return 0;
    }
    return 1;
}

static int contains_i_(const WCHAR* path, const WCHAR* needle) {
    UINT plen = wlen_(path), nlen = wlen_(needle), i, j;
    if (!nlen || nlen > plen) return 0;
    for (i = 0u; i + nlen <= plen; ++i) {
        for (j = 0u; j < nlen; ++j) {
            WCHAR x = path[i + j], y = needle[j];
            if (x >= (WCHAR)'A' && x <= (WCHAR)'Z') x = (WCHAR)(x + 32);
            if (y >= (WCHAR)'A' && y <= (WCHAR)'Z') y = (WCHAR)(y + 32);
            if (x != y) break;
        }
        if (j == nlen) return 1;
    }
    return 0;
}

static GAME_AUDIT_HANDLE_* find_handle_(HANDLE handle) {
    UINT i;
    if (!handle || handle == INVALID_HANDLE_VALUE_) return NULL_PTR;
    for (i = 0u; i < GAME_AUDIT_HANDLE_MAX_; ++i) if (g_handles[i].handle == handle) return &g_handles[i];
    return NULL_PTR;
}

static void track_handle_(HANDLE handle, LPCWSTR path, DWORD access, DWORD creation) {
    UINT i;
    GAME_AUDIT_HANDLE_* slot = NULL_PTR;
    if (!handle || handle == INVALID_HANDLE_VALUE_) return;
    for (i = 0u; i < GAME_AUDIT_HANDLE_MAX_; ++i) {
        if (!g_handles[i].handle) { slot = &g_handles[i]; break; }
    }
    if (!slot) return;
    slot->handle = handle;
    slot->path[0] = 0;
    if (path) wcopy_(slot->path, GAME_AUDIT_PATH_CAP_, path);
    slot->access = access;
    slot->creation = creation;
    slot->open_tick = GetTickCount();
    slot->read_calls = slot->read_bytes = slot->write_calls = slot->write_bytes = 0u;
    slot->logical_offset = 0u;
    /*
     * TSF、NewGame，以及任何可写句柄都逐次记录 ReadFile。
     * 普通大资源只在 CloseHandle 汇总读次数/字节，避免音频/贴图连续小块读取把日志写成几百 MB。
     */
    slot->detailed_reads = ((access & GENERIC_WRITE_) || ends_with_i_(slot->path, (const WCHAR*)L".TSF") ||
                            contains_i_(slot->path, (const WCHAR*)L"NewGame")) ? 1 : 0;
    slot->quiet_fdebug = ends_with_i_(slot->path, (const WCHAR*)L"FDebug.txt") ? 1 : 0;
}

static void untrack_handle_(GAME_AUDIT_HANDLE_* slot) {
    if (!slot) return;
    slot->handle = NULL_PTR;
    slot->path[0] = 0;
    slot->access = slot->creation = slot->open_tick = 0u;
    slot->read_calls = slot->read_bytes = slot->write_calls = slot->write_bytes = slot->logical_offset = 0u;
    slot->detailed_reads = 0;
    slot->quiet_fdebug = 0;
}

/*
 * FDebug.txt 聚合不是“忽略日志”，而是把大量重复成功操作压成固定时间窗统计。
 * 每个窗口最多约 5 秒；即使游戏长时间运行，也会持续留下时间位置。失败不会进入这里，会由对应 Hook 立即展开。
 */
static void fdebug_flush_(void) {
    WCHAR line[512];
    if (!g_fdebug.open_calls && !g_fdebug.write_calls && !g_fdebug.close_calls) return;
    line[0] = 0;
    wappend_(line, 512u, (const WCHAR*)L"path=FDebug.txt window_ms=");
    append_u32_dec_(line, 512u, g_fdebug.last_tick - g_fdebug.window_start_tick);
    wappend_(line, 512u, (const WCHAR*)L" opens="); append_u32_dec_(line, 512u, g_fdebug.open_calls);
    wappend_(line, 512u, (const WCHAR*)L" writes="); append_u32_dec_(line, 512u, g_fdebug.write_calls);
    wappend_(line, 512u, (const WCHAR*)L" write_bytes="); append_u32_dec_(line, 512u, g_fdebug.write_bytes);
    wappend_(line, 512u, (const WCHAR*)L" closes="); append_u32_dec_(line, 512u, g_fdebug.close_calls);
    log_line_((const WCHAR*)L"IO/FDebug聚合", line);
    g_fdebug.window_start_tick = g_fdebug.last_tick = 0u;
    g_fdebug.open_calls = g_fdebug.write_calls = g_fdebug.write_bytes = g_fdebug.close_calls = 0u;
}

static void fdebug_roll_window_(DWORD now) {
    if (!g_fdebug.window_start_tick) {
        g_fdebug.window_start_tick = now;
        g_fdebug.last_tick = now;
        return;
    }
    if (now - g_fdebug.window_start_tick >= 5000u) {
        fdebug_flush_();
        g_fdebug.window_start_tick = now;
    }
    g_fdebug.last_tick = now;
}

static void fdebug_note_open_(void) {
    DWORD now = GetTickCount();
    fdebug_roll_window_(now);
    ++g_fdebug.open_calls;
}

static void fdebug_note_write_(DWORD bytes) {
    DWORD now = GetTickCount();
    fdebug_roll_window_(now);
    ++g_fdebug.write_calls;
    g_fdebug.write_bytes += bytes;
}

static void fdebug_note_close_(void) {
    DWORD now = GetTickCount();
    fdebug_roll_window_(now);
    ++g_fdebug.close_calls;
}

/* ---------- ANSI 路径转 UTF-16 与 Big5 原始字节证据 ---------- */

static int ansi_to_big5_wide_(LPCSTR path, WCHAR* out, UINT cap) {
    if (!path || !out || !cap) return 0;
    return MultiByteToWideChar(CASTLE_CP_BIG5_, 0u, path, -1, out, (int)cap) > 0;
}

static void remember_missing_loose_(LPCWSTR path) {
    DWORD tid = GetCurrentThreadId();
    DWORD now = GetTickCount();
    UINT i, slot = 0u;
    DWORD oldest = 0xFFFFFFFFu;
    if (!path || !path[0] || ends_with_i_(path, (const WCHAR*)L".DAT")) return;
    for (i = 0u; i < 16u; ++i) {
        if (g_fallback_pending[i].thread_id == tid) { slot = i; break; }
        if (!g_fallback_pending[i].thread_id) { slot = i; break; }
        if (g_fallback_pending[i].tick < oldest) { oldest = g_fallback_pending[i].tick; slot = i; }
    }
    g_fallback_pending[slot].thread_id = tid;
    g_fallback_pending[slot].tick = now;
    wcopy_(g_fallback_pending[slot].loose_path, GAME_AUDIT_PATH_CAP_, path);
}

static void emit_loose_to_dat_candidate_(LPVOID caller, LPCWSTR dat_path, DWORD offset) {
    DWORD tid = GetCurrentThreadId();
    DWORD now = GetTickCount();
    UINT i;
    if (!dat_path || !ends_with_i_(dat_path, (const WCHAR*)L".DAT")) return;
    for (i = 0u; i < 16u; ++i) {
        FALLBACK_PENDING_* pending = &g_fallback_pending[i];
        if (pending->thread_id != tid) continue;
        if (now - pending->tick <= 500u && pending->loose_path[0]) {
            WCHAR line[GAME_AUDIT_LINE_CAP_]; line[0] = 0;
            append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" loose="); wappend_(line, GAME_AUDIT_LINE_CAP_, pending->loose_path);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" dat="); wappend_(line, GAME_AUDIT_LINE_CAP_, dat_path);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" offset="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, offset);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" evidence=同线程500ms时间邻接候选");
            log_line_((const WCHAR*)L"IO/散装转DAT候选", line);
        }
        pending->thread_id = pending->tick = 0u;
        pending->loose_path[0] = 0;
        return;
    }
}

static void append_ansi_hex_(WCHAR* line, UINT cap, LPCSTR path) {
    UINT i = 0u;
    if (!path) return;
    wappend_(line, cap, (const WCHAR*)L" raw=");
    while (path[i] && i < 96u) {
        BYTE b = (BYTE)path[i];
        WCHAR pair[4];
        pair[0] = hex_digit_(b >> 4);
        pair[1] = hex_digit_(b);
        pair[2] = (WCHAR)' ';
        pair[3] = 0;
        wappend_(line, cap, pair);
        ++i;
    }
    if (path[i]) wappend_(line, cap, (const WCHAR*)L"...");
}

/* ---------- OverrideLoader 送来的 CreateFile/GetFileAttributes 观察 ---------- */

void GameAudit_RecordCreateFileA(LPVOID caller, LPCSTR path, DWORD access, DWORD share, DWORD creation,
                                 DWORD attrs, HANDLE result, DWORD error, int redirected_by_override) {
    WCHAR line[GAME_AUDIT_LINE_CAP_];
    WCHAR wide[GAME_AUDIT_PATH_CAP_];
    const GAME_AUDIT_MODULE_* source;
    DWORD final_n;

    /*
     * wide 是后面的 Big5 解码输出缓冲区。即使 path 本身无效、转换失败，
     * 后面的“最终路径反查失败时是否还能用 wide 记录句柄”也必须读到确定的 0，
     * 不能读取栈上随机残留值。先写一个结尾 0，就把这个极端失败路径也变成确定行为。
     */
    wide[0] = 0;

    if (!g_ready) return;
    source = find_caller_module_(caller);
    if (!source) return; /* ASI/Mod 的文件访问不属于“原版游戏日志”。 */

    line[0] = 0;
    append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path=");
    if (ansi_to_big5_wide_(path, wide, GAME_AUDIT_PATH_CAP_)) wappend_(line, GAME_AUDIT_LINE_CAP_, wide);
    else wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"<Big5解码失败>");
    append_ansi_hex_(line, GAME_AUDIT_LINE_CAP_, path);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" access="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, access);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" share="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, share);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" creation="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, creation);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" attrs="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, attrs);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" result="); append_ptr_hex_(line, GAME_AUDIT_LINE_CAP_, result);
    append_create_result_(line, GAME_AUDIT_LINE_CAP_, result, creation, error);
    if (redirected_by_override) wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" override=YES");

    if (result != INVALID_HANDLE_VALUE_) {
        WCHAR final_path[GAME_AUDIT_PATH_CAP_];
        final_n = GetFinalPathNameByHandleW(result, final_path, GAME_AUDIT_PATH_CAP_, 0u);
        if (final_n > 0u && final_n < GAME_AUDIT_PATH_CAP_) {
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" final=");
            wappend_(line, GAME_AUDIT_LINE_CAP_, final_path);
            track_handle_(result, final_path, access, creation);
        } else if (wide[0]) {
            track_handle_(result, wide, access, creation);
        }
        {
            GAME_AUDIT_HANDLE_* state = find_handle_(result);
            if (state && state->quiet_fdebug) {
                fdebug_note_open_();
                return;
            }
        }
    }
    log_line_((const WCHAR*)L"IO/CreateFileA", line);
}

void GameAudit_RecordCreateFileW(LPVOID caller, LPCWSTR path, DWORD access, DWORD share, DWORD creation,
                                 DWORD attrs, HANDLE result, DWORD error, int redirected_by_override) {
    WCHAR line[GAME_AUDIT_LINE_CAP_];
    const GAME_AUDIT_MODULE_* source;
    DWORD final_n;
    source = find_caller_module_(caller);
    if (!g_ready || !source) return;
    line[0] = 0;
    append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path="); wappend_(line, GAME_AUDIT_LINE_CAP_, path ? path : (const WCHAR*)L"<NULL>");
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" access="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, access);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" share="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, share);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" creation="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, creation);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" attrs="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, attrs);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" result="); append_ptr_hex_(line, GAME_AUDIT_LINE_CAP_, result);
    append_create_result_(line, GAME_AUDIT_LINE_CAP_, result, creation, error);
    if (redirected_by_override) wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" override=YES");
    if (result != INVALID_HANDLE_VALUE_) {
        WCHAR final_path[GAME_AUDIT_PATH_CAP_];
        final_n = GetFinalPathNameByHandleW(result, final_path, GAME_AUDIT_PATH_CAP_, 0u);
        if (final_n > 0u && final_n < GAME_AUDIT_PATH_CAP_) {
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" final="); wappend_(line, GAME_AUDIT_LINE_CAP_, final_path);
            track_handle_(result, final_path, access, creation);
        } else track_handle_(result, path, access, creation);
        {
            GAME_AUDIT_HANDLE_* state = find_handle_(result);
            if (state && state->quiet_fdebug) {
                fdebug_note_open_();
                return;
            }
        }
    }
    log_line_((const WCHAR*)L"IO/CreateFileW", line);
}

void GameAudit_RecordGetFileAttributesA(LPVOID caller, LPCSTR path, DWORD result, DWORD error, int redirected_by_override) {
    WCHAR line[GAME_AUDIT_LINE_CAP_], wide[GAME_AUDIT_PATH_CAP_];
    if (!g_ready || !find_caller_module_(caller)) return;
    line[0] = wide[0] = 0;
    append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path=");
    if (ansi_to_big5_wide_(path, wide, GAME_AUDIT_PATH_CAP_)) wappend_(line, GAME_AUDIT_LINE_CAP_, wide);
    else wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"<Big5解码失败>");
    append_ansi_hex_(line, GAME_AUDIT_LINE_CAP_, path);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" result="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, result);
    append_error_result_(line, GAME_AUDIT_LINE_CAP_, result == INVALID_FILE_ATTRIBUTES_, error);
    if (result == INVALID_FILE_ATTRIBUTES_ && (error == ERROR_FILE_NOT_FOUND_ || error == ERROR_PATH_NOT_FOUND_) && wide[0])
        remember_missing_loose_(wide);
    if (redirected_by_override) wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" override=YES");
    log_line_((const WCHAR*)L"IO/GetFileAttributesA", line);
}

void GameAudit_RecordGetFileAttributesW(LPVOID caller, LPCWSTR path, DWORD result, DWORD error, int redirected_by_override) {
    WCHAR line[GAME_AUDIT_LINE_CAP_];
    if (!g_ready || !find_caller_module_(caller)) return;
    line[0] = 0;
    append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path="); wappend_(line, GAME_AUDIT_LINE_CAP_, path ? path : (const WCHAR*)L"<NULL>");
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" result="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, result);
    append_error_result_(line, GAME_AUDIT_LINE_CAP_, result == INVALID_FILE_ATTRIBUTES_, error);
    if (result == INVALID_FILE_ATTRIBUTES_ && (error == ERROR_FILE_NOT_FOUND_ || error == ERROR_PATH_NOT_FOUND_) && path)
        remember_missing_loose_(path);
    if (redirected_by_override) wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" override=YES");
    log_line_((const WCHAR*)L"IO/GetFileAttributesW", line);
}

/* ---------- RPG.exe 与原版 DLL 的其余 I/O Hook ---------- */

static BOOL WINAPI Hook_ReadFile_(HANDLE file, LPVOID buffer, DWORD bytes, DWORD* read_out, LPVOID overlapped) {
    LPVOID caller = _ReturnAddress();
    GAME_AUDIT_HANDLE_* state = find_handle_(file);
    DWORD before = state ? state->logical_offset : 0u;
    DWORD actual = 0u;
    BOOL ok = ReadFile(file, buffer, bytes, read_out ? read_out : &actual, overlapped);
    DWORD error = GetLastError();
    DWORD done = read_out ? *read_out : actual;
    if (state) {
        state->read_calls++;
        state->read_bytes += done;
        if (!overlapped) state->logical_offset += done;
        if (state->detailed_reads && (!state->quiet_fdebug || !ok)) {
            WCHAR line[GAME_AUDIT_LINE_CAP_]; line[0] = 0;
            append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path="); wappend_(line, GAME_AUDIT_LINE_CAP_, state->path);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" offset="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, before);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" request="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, bytes);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" actual="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, done);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ok="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, ok ? 1u : 0u);
            append_error_result_(line, GAME_AUDIT_LINE_CAP_, !ok, error);
            log_line_((const WCHAR*)L"IO/ReadFile", line);
        }
    }
    SetLastError(error);
    return ok;
}

static BOOL WINAPI Hook_WriteFile_(HANDLE file, LPCVOID buffer, DWORD bytes, DWORD* written_out, LPVOID overlapped) {
    LPVOID caller = _ReturnAddress();
    GAME_AUDIT_HANDLE_* state = find_handle_(file);
    DWORD before = state ? state->logical_offset : 0u;
    DWORD actual = 0u;
    BOOL ok = WriteFile(file, buffer, bytes, written_out ? written_out : &actual, overlapped);
    DWORD error = GetLastError();
    DWORD done = written_out ? *written_out : actual;
    if (state) {
        state->write_calls++;
        state->write_bytes += done;
        if (!overlapped) state->logical_offset += done;
        if (state->quiet_fdebug && ok) {
            fdebug_note_write_(done);
        } else {
            WCHAR line[GAME_AUDIT_LINE_CAP_]; line[0] = 0;
            append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path="); wappend_(line, GAME_AUDIT_LINE_CAP_, state->path);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" offset="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, before);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" request="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, bytes);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" actual="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, done);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ok="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, ok ? 1u : 0u);
            append_error_result_(line, GAME_AUDIT_LINE_CAP_, !ok, error);
            log_line_((const WCHAR*)L"IO/WriteFile", line);
        }
    }
    SetLastError(error);
    return ok;
}

static DWORD WINAPI Hook_SetFilePointer_(HANDLE file, LONG distance, LONG* high, DWORD method) {
    LPVOID caller = _ReturnAddress();
    DWORD result = SetFilePointer(file, distance, high, method);
    DWORD error = GetLastError();
    GAME_AUDIT_HANDLE_* state = find_handle_(file);
    if (state) {
        WCHAR line[GAME_AUDIT_LINE_CAP_]; line[0] = 0;
        if (result != INVALID_SET_FILE_POINTER_ || error == 0u) {
            state->logical_offset = result;
            emit_loose_to_dat_candidate_(caller, state->path, result);
        }
        append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path="); wappend_(line, GAME_AUDIT_LINE_CAP_, state->path);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" distance="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, (DWORD)distance);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" method="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, method);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" result="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, result);
        append_error_result_(line, GAME_AUDIT_LINE_CAP_, result == INVALID_SET_FILE_POINTER_ && error != ERROR_SUCCESS_, error);
        if (!state->quiet_fdebug || (result == INVALID_SET_FILE_POINTER_ && error != ERROR_SUCCESS_))
            log_line_((const WCHAR*)L"IO/SetFilePointer", line);
    }
    SetLastError(error);
    return result;
}

static BOOL WINAPI Hook_SetEndOfFile_(HANDLE file) {
    LPVOID caller = _ReturnAddress();
    BOOL ok = SetEndOfFile(file);
    DWORD error = GetLastError();
    GAME_AUDIT_HANDLE_* state = find_handle_(file);
    if (state) {
        WCHAR line[GAME_AUDIT_LINE_CAP_]; line[0] = 0;
        append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path="); wappend_(line, GAME_AUDIT_LINE_CAP_, state->path);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" offset="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, state->logical_offset);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ok="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, ok ? 1u : 0u);
        append_error_result_(line, GAME_AUDIT_LINE_CAP_, !ok, error);
        if (!state->quiet_fdebug || !ok) log_line_((const WCHAR*)L"IO/SetEndOfFile", line);
    }
    SetLastError(error);
    return ok;
}

static BOOL WINAPI Hook_FlushFileBuffers_(HANDLE file) {
    LPVOID caller = _ReturnAddress();
    BOOL ok = FlushFileBuffers(file);
    DWORD error = GetLastError();
    GAME_AUDIT_HANDLE_* state = find_handle_(file);
    if (state) {
        WCHAR line[GAME_AUDIT_LINE_CAP_]; line[0] = 0;
        append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path="); wappend_(line, GAME_AUDIT_LINE_CAP_, state->path);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ok="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, ok ? 1u : 0u);
        append_error_result_(line, GAME_AUDIT_LINE_CAP_, !ok, error);
        if (!state->quiet_fdebug || !ok) log_line_((const WCHAR*)L"IO/FlushFileBuffers", line);
    }
    SetLastError(error);
    return ok;
}

static BOOL WINAPI Hook_CloseHandle_(HANDLE file) {
    LPVOID caller = _ReturnAddress();
    GAME_AUDIT_HANDLE_* state = find_handle_(file);
    WCHAR path_copy[GAME_AUDIT_PATH_CAP_];
    DWORD reads = 0u, read_bytes = 0u, writes = 0u, write_bytes = 0u, open_tick = 0u;
    if (state) {
        wcopy_(path_copy, GAME_AUDIT_PATH_CAP_, state->path);
        reads = state->read_calls; read_bytes = state->read_bytes;
        writes = state->write_calls; write_bytes = state->write_bytes; open_tick = state->open_tick;
    } else path_copy[0] = 0;
    {
        BOOL ok = CloseHandle(file);
        DWORD error = GetLastError();
        if (state) {
            int quiet_fdebug = state->quiet_fdebug;
            WCHAR line[GAME_AUDIT_LINE_CAP_]; line[0] = 0;
            append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path="); wappend_(line, GAME_AUDIT_LINE_CAP_, path_copy);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" lifetime_ms="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, GetTickCount() - open_tick);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" reads="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, reads);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" read_bytes="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, read_bytes);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" writes="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, writes);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" write_bytes="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, write_bytes);
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ok="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, ok ? 1u : 0u);
            append_error_result_(line, GAME_AUDIT_LINE_CAP_, !ok, error);
            if (quiet_fdebug && ok) fdebug_note_close_();
            else log_line_((const WCHAR*)L"IO/CloseHandle", line);
            untrack_handle_(state);
        }
        SetLastError(error);
        return ok;
    }
}

static DWORD WINAPI Hook_GetCurrentDirectoryA_(DWORD cap, LPSTR out) {
    LPVOID caller = _ReturnAddress();
    DWORD result = GetCurrentDirectoryA(cap, out);
    DWORD error = GetLastError();
    if (find_caller_module_(caller)) {
        WCHAR line[GAME_AUDIT_LINE_CAP_], wide[GAME_AUDIT_PATH_CAP_]; line[0] = 0; wide[0] = 0;
        append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" result_chars="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, result);
        if (result && out && ansi_to_big5_wide_(out, wide, GAME_AUDIT_PATH_CAP_)) {
            wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" cwd="); wappend_(line, GAME_AUDIT_LINE_CAP_, wide);
        }
        append_error_result_(line, GAME_AUDIT_LINE_CAP_, result == 0u, error);
        log_line_((const WCHAR*)L"IO/GetCurrentDirectoryA", line);
    }
    SetLastError(error);
    return result;
}

static BOOL WINAPI Hook_DeleteFileA_(LPCSTR path) {
    LPVOID caller = _ReturnAddress();
    BOOL ok = DeleteFileA(path);
    DWORD error = GetLastError();
    if (find_caller_module_(caller)) {
        WCHAR line[GAME_AUDIT_LINE_CAP_], wide[GAME_AUDIT_PATH_CAP_]; line[0] = 0; wide[0] = 0;
        append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path=");
        if (ansi_to_big5_wide_(path, wide, GAME_AUDIT_PATH_CAP_)) wappend_(line, GAME_AUDIT_LINE_CAP_, wide);
        else wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"<Big5解码失败>");
        append_ansi_hex_(line, GAME_AUDIT_LINE_CAP_, path);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ok="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, ok ? 1u : 0u);
        append_error_result_(line, GAME_AUDIT_LINE_CAP_, !ok, error);
        log_line_((const WCHAR*)L"IO/DeleteFileA", line);
    }
    SetLastError(error);
    return ok;
}

static BOOL WINAPI Hook_SetFileAttributesA_(LPCSTR path, DWORD attrs) {
    LPVOID caller = _ReturnAddress();
    BOOL ok = SetFileAttributesA(path, attrs);
    DWORD error = GetLastError();
    if (find_caller_module_(caller)) {
        WCHAR line[GAME_AUDIT_LINE_CAP_], wide[GAME_AUDIT_PATH_CAP_]; line[0] = 0; wide[0] = 0;
        append_caller_(line, GAME_AUDIT_LINE_CAP_, caller);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" path=");
        if (ansi_to_big5_wide_(path, wide, GAME_AUDIT_PATH_CAP_)) wappend_(line, GAME_AUDIT_LINE_CAP_, wide);
        else wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"<Big5解码失败>");
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" attrs="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, attrs);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ok="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, ok ? 1u : 0u);
        append_error_result_(line, GAME_AUDIT_LINE_CAP_, !ok, error);
        log_line_((const WCHAR*)L"IO/SetFileAttributesA", line);
    }
    SetLastError(error);
    return ok;
}

/*
 * IAT 修改器只接管 RPG.exe/原版依赖模块自己的 IAT。
 * Core 自己的 IAT 从不修改，所以 Hook 内部直接调用 ReadFile/WriteFile 等真正 KERNEL32 导入不会递归。
 */
static int ascii_eq_i_(const char* a, const char* b) {
    UINT i = 0u;
    if (!a || !b) return 0;
    while (a[i] && b[i]) {
        char x = a[i], y = b[i];
        if (x >= 'A' && x <= 'Z') x = (char)(x + 32);
        if (y >= 'A' && y <= 'Z') y = (char)(y + 32);
        if (x != y) return 0;
        ++i;
    }
    return a[i] == 0 && b[i] == 0;
}

static UINT patch_io_iat_(HMODULE module) {
    BYTE* base = (BYTE*)module;
    DWORD pe, opt, import_rva, desc_rva;
    UINT patched = 0u;
    if (!module || *(WORD*)base != IMAGE_DOS_SIGNATURE_) return 0u;
    pe = *(DWORD*)(base + 0x3Cu);
    if (*(DWORD*)(base + pe) != IMAGE_NT_SIGNATURE_) return 0u;
    opt = pe + 4u + 20u;
    if (*(WORD*)(base + opt) != IMAGE_NT_OPTIONAL_HDR32_MAGIC_) return 0u;
    import_rva = *(DWORD*)(base + opt + 96u + IMAGE_DIRECTORY_ENTRY_IMPORT_ * 8u);
    if (!import_rva) return 0u;

    desc_rva = import_rva;
    for (;;) {
        DWORD* desc = (DWORD*)(base + desc_rva);
        DWORD oft = desc[0], name_rva = desc[3], ft = desc[4], index = 0u;
        const char* dll;
        if (!desc[0] && !desc[1] && !desc[2] && !desc[3] && !desc[4]) break;
        desc_rva += 20u;
        if (!oft || !name_rva || !ft) continue;
        dll = (const char*)(base + name_rva);
        if (!ascii_eq_i_(dll, "KERNEL32.dll") && !ascii_eq_i_(dll, "KERNELBASE.dll")) continue;
        for (;;) {
            DWORD thunk = *(DWORD*)(base + oft + index * 4u);
            DWORD* slot = (DWORD*)(base + ft + index * 4u);
            const char* name;
            DWORD hook = 0u, oldp = 0u, ignored = 0u;
            if (!thunk) break;
            ++index;
            if (thunk & IMAGE_ORDINAL_FLAG32_) continue;
            name = (const char*)(base + thunk + 2u);
            if (ascii_eq_i_(name, "ReadFile")) hook = (DWORD)(SIZE_T)&Hook_ReadFile_;
            else if (ascii_eq_i_(name, "WriteFile")) hook = (DWORD)(SIZE_T)&Hook_WriteFile_;
            else if (ascii_eq_i_(name, "CloseHandle")) hook = (DWORD)(SIZE_T)&Hook_CloseHandle_;
            else if (ascii_eq_i_(name, "SetFilePointer")) hook = (DWORD)(SIZE_T)&Hook_SetFilePointer_;
            else if (ascii_eq_i_(name, "SetEndOfFile")) hook = (DWORD)(SIZE_T)&Hook_SetEndOfFile_;
            else if (ascii_eq_i_(name, "FlushFileBuffers")) hook = (DWORD)(SIZE_T)&Hook_FlushFileBuffers_;
            else if (ascii_eq_i_(name, "GetCurrentDirectoryA")) hook = (DWORD)(SIZE_T)&Hook_GetCurrentDirectoryA_;
            else if (ascii_eq_i_(name, "DeleteFileA")) hook = (DWORD)(SIZE_T)&Hook_DeleteFileA_;
            else if (ascii_eq_i_(name, "SetFileAttributesA")) hook = (DWORD)(SIZE_T)&Hook_SetFileAttributesA_;
            else continue;
            if (!VirtualProtect(slot, 4u, PAGE_READWRITE_, &oldp)) continue;
            *slot = hook;
            VirtualProtect(slot, 4u, oldp, &ignored);
            ++patched;
        }
    }
    if (patched) FlushInstructionCache(GetCurrentProcess(), module, 1u);
    return patched;
}

UINT GameAudit_PatchOriginalIoModules(void) {
    UINT i, total = 0u;

    /*
     * 每个原版模块单独统计 IAT Hook 数量并写进 game.log。
     * 这样用户不需要只相信 modloader.log 里的一个“总数”：如果某台机器 BASS.dll 没加载、
     * 或某个模块没有我们要找的 I/O 导入，game.log 会明确显示该模块 slots=0。
     */
    for (i = 0u; i < g_module_count; ++i) {
        UINT patched = patch_io_iat_((HMODULE)g_modules[i].base);
        WCHAR line[512];
        line[0] = 0;
        wappend_(line, 512u, (const WCHAR*)L"module=");
        wappend_(line, 512u, g_modules[i].name);
        wappend_(line, 512u, (const WCHAR*)L" base=");
        append_ptr_hex_(line, 512u, g_modules[i].base);
        wappend_(line, 512u, (const WCHAR*)L" slots=");
        append_u32_dec_(line, 512u, patched);
        log_line_((const WCHAR*)L"AUDIT/IOHook", line);
        total += patched;
    }
    return total;
}

/* ---------- 已确认生命周期边界的状态快照 ---------- */

static int read_game_memory_(DWORD oracle_va, LPVOID out, SIZE_T size) {
    SIZE_T got = 0u;
    BYTE* address;
    if (!g_game_module || oracle_va < GAME_IMAGE_BASE_ORACLE_) return 0;
    address = (BYTE*)g_game_module + (oracle_va - GAME_IMAGE_BASE_ORACLE_);
    return ReadProcessMemory(GetCurrentProcess(), address, out, size, &got) && got == size;
}

static int read_pointer_offset_(DWORD pointer_value, DWORD offset, DWORD* out) {
    SIZE_T got = 0u;
    if (!pointer_value || !out) return 0;
    return ReadProcessMemory(GetCurrentProcess(), (LPCVOID)(SIZE_T)(pointer_value + offset), out, sizeof(DWORD), &got) && got == sizeof(DWORD);
}

static void capture_state_(GAME_STATE_SNAPSHOT_* s) {
    BYTE flags[2] = {0,0};
    if (!s) return;
    /* 先全部清零；某个地址若不可读，日志里的 0 至少不会来自未初始化栈垃圾。 */
    { BYTE* z = (BYTE*)s; UINT zi; for (zi = 0u; zi < (UINT)sizeof(*s); ++zi) z[zi] = 0u; }
    read_game_memory_(0x0089F804u, &s->active_controller, sizeof(DWORD));
    read_game_memory_(0x0089F808u, &s->active_event_id, sizeof(DWORD));
    read_game_memory_(0x0089F810u, &s->event_pc, sizeof(DWORD));
    if (read_game_memory_(0x0089F81Du, flags, 2u)) { s->yield_flag = flags[0]; s->blocked_flag = flags[1]; }
    read_game_memory_(0x0089F830u, &s->command_ptr, sizeof(DWORD));
    read_game_memory_(0x0089F834u, &s->wait_counter, sizeof(DWORD));
    read_game_memory_(0x0089F7F8u, &s->saved_event_id, sizeof(DWORD));
    read_game_memory_(0x0089F7FCu, &s->resume_key_a, sizeof(DWORD));
    read_game_memory_(0x0089F800u, &s->resume_key_b, sizeof(DWORD));
    read_game_memory_(0x0089F7D0u, &s->world_async_result, sizeof(DWORD));
    read_game_memory_(0x0089F818u, &s->interface_state, sizeof(DWORD));
    read_game_memory_(0x008B01F0u, &s->battle_container, sizeof(DWORD));
    read_game_memory_(0x0089FCC8u, &s->battle_result_ui, sizeof(DWORD));
    read_game_memory_(0x004696DCu, &s->battle_gate_raw, sizeof(DWORD));
    read_game_memory_(0x0046E29Cu, &s->rng_state, sizeof(DWORD));
    read_game_memory_(0x008B01ECu, &s->data_center_ptr, sizeof(DWORD));
    read_game_memory_(0x008E1C48u, &s->manager_ptr, sizeof(DWORD));
    read_game_memory_(0x00978508u, &s->world_block_ptr, sizeof(DWORD));
    if (s->manager_ptr) read_pointer_offset_(s->manager_ptr, 0x100u, &s->manager_plus_100);
}

static const WCHAR* marker_name_(UINT id) {
    switch (id) {
        /* 0 不是代码 Hook，而是 GameAudit 初始化完成后、RPG.exe 第一条业务代码以前主动拍的一张基线照片。 */
        case 0: return (const WCHAR*)L"BASELINE_BEFORE_RPG_ENTRY";
        case 1: return (const WCHAR*)L"NEW_GAME_BEGIN";
        case 2: return (const WCHAR*)L"SAVE_WRITER";
        case 3: return (const WCHAR*)L"LOAD_READER";
        case 4: return (const WCHAR*)L"SERIALIZE_CORE";
        case 5: return (const WCHAR*)L"DESERIALIZE_CORE";
        case 6: return (const WCHAR*)L"REBUILD_ROLE_PTRS";
        case 7: return (const WCHAR*)L"COPY_SCENE_DESCRIPTOR";
        case 8: return (const WCHAR*)L"RESTORE_WORLD";
        case 9: return (const WCHAR*)L"EVENT_ACTIVATE";
        case 10: return (const WCHAR*)L"EVENT_CLEAR";
        case 11: return (const WCHAR*)L"LOAD_SCENE_RESET";
        case 12: return (const WCHAR*)L"LOAD_SCENE";
        case 13: return (const WCHAR*)L"SAVE_SLOT_UI";
        /* 14 同样不是代码 Hook；它只由 VEH 在严重异常寄存器行写完后补拍当前游戏全局状态。 */
        case 14: return (const WCHAR*)L"EXCEPTION_SNAPSHOT";
        default: return (const WCHAR*)L"UNKNOWN_MARKER";
    }
}

/*
 * 这个函数由动态生成的 x86 stub 调用。
 * stub 在调用前 pushfd+pushad，返回后完整恢复寄存器和 EFLAGS，因此记录行为不会把 C 函数使用的寄存器污染回游戏。
 */
void __cdecl GameAudit_RecordStateMarker(UINT marker_id) {
    GAME_STATE_SNAPSHOT_ s;
    WCHAR line[GAME_AUDIT_LINE_CAP_];
    capture_state_(&s);
    line[0] = 0;
    wappend_(line, GAME_AUDIT_LINE_CAP_, marker_name_(marker_id));
    /* activeController 是探索侧当前控制器/上下文指针。它如果跨“读档 → 新游戏”错误残留，后续对象访问可能直接变成悬空指针。 */
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" activeController="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.active_controller);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" activeEvent="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.active_event_id);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" eventPC="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.event_pc);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" command="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.command_ptr);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" yield="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, s.yield_flag);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" blocked="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, s.blocked_flag);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" wait="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, s.wait_counter);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" savedEvent="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.saved_event_id);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" keyA="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.resume_key_a);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" keyB="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.resume_key_b);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" async="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.world_async_result);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" interface="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.interface_state);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" manager="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.manager_ptr);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" manager+100="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.manager_plus_100);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" dataCenter="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.data_center_ptr);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" worldBlock="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.world_block_ptr);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" battle="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.battle_container);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" battleUI="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.battle_result_ui);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" battleGateRaw="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.battle_gate_raw);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" rng="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, s.rng_state);
    log_line_((const WCHAR*)L"STATE", line);
    /*
     * 生命周期状态点数量很少，但它们恰恰是崩溃分析最不能丢的行。
     * 普通文件 I/O 日志依赖 Windows 文件缓存即可；状态点则在写完后主动刷新一次，
     * 这样即使下一条游戏指令马上访问违规并终止进程，前一个状态边界也尽量已经落到磁盘。
     */
    if (g_log != INVALID_HANDLE_VALUE_) FlushFileBuffers(g_log);
}

/* ---------- 状态函数入口的 fail-closed x86 detour ---------- */

static int bytes_equal_(const BYTE* a, const BYTE* b, UINT n) {
    UINT i;
    for (i = 0u; i < n; ++i) if (a[i] != b[i]) return 0;
    return 1;
}

static void write_rel32_(BYTE* at, BYTE opcode, BYTE* destination) {
    LONG rel = (LONG)(destination - (at + 5));
    at[0] = opcode;
    *(LONG*)(at + 1) = rel;
}

static int install_state_hook_(GAME_STATE_HOOK_* hook) {
    BYTE* target;
    BYTE* trampoline;
    BYTE* stub;
    DWORD oldp = 0u, ignored = 0u;
    UINT i;
    if (!hook || !g_game_module || hook->patch_len < 5u || hook->patch_len > 8u || hook->expected_len != hook->patch_len) return 0;
    target = (BYTE*)g_game_module + (hook->oracle_va - GAME_IMAGE_BASE_ORACLE_);
    if (!bytes_equal_(target, hook->expected, hook->expected_len)) return 0;

    /*
     * trampoline = 原始被覆盖字节 + E9 跳回 target+patch_len。
     * 表内所有 patch_len 都是事先人工反汇编确认的完整指令边界，并且没有搬运相对 CALL/JMP。
     */
    trampoline = (BYTE*)VirtualAlloc(NULL_PTR, 32u, MEM_COMMIT_ | MEM_RESERVE_, PAGE_EXECUTE_READWRITE_);
    stub = (BYTE*)VirtualAlloc(NULL_PTR, 64u, MEM_COMMIT_ | MEM_RESERVE_, PAGE_EXECUTE_READWRITE_);
    if (!trampoline || !stub) return 0;
    for (i = 0u; i < hook->patch_len; ++i) trampoline[i] = target[i];
    write_rel32_(trampoline + hook->patch_len, 0xE9u, target + hook->patch_len);

    /*
     * 动态 stub 的机器码顺序：
     *   9C             pushfd            保存 EFLAGS
     *   60             pushad            保存 8 个通用寄存器
     *   68 id          push marker_id    给 C 记录器一个普通 cdecl 参数
     *   E8 rel32       call recorder
     *   83 C4 04       add esp,4         cdecl 由调用者清参数
     *   61             popad             恢复通用寄存器
     *   9D             popfd             恢复 EFLAGS
     *   E9 rel32       jmp trampoline    继续原游戏被覆盖的第一条指令
     */
    i = 0u;
    stub[i++] = 0x9Cu;
    stub[i++] = 0x60u;
    stub[i++] = 0x68u; *(DWORD*)(stub + i) = (DWORD)hook->marker_id; i += 4u;
    write_rel32_(stub + i, 0xE8u, (BYTE*)(SIZE_T)&GameAudit_RecordStateMarker); i += 5u;
    stub[i++] = 0x83u; stub[i++] = 0xC4u; stub[i++] = 0x04u;
    stub[i++] = 0x61u;
    stub[i++] = 0x9Du;
    /* 最后一条是无条件跳转，后面不会再继续往 stub 里写字节，所以这里不需要再增加 i。 */
    write_rel32_(stub + i, 0xE9u, trampoline);

    if (!VirtualProtect(target, hook->patch_len, PAGE_EXECUTE_READWRITE_, &oldp)) return 0;
    write_rel32_(target, 0xE9u, stub);
    for (i = 5u; i < hook->patch_len; ++i) target[i] = 0x90u;
    VirtualProtect(target, hook->patch_len, oldp, &ignored);
    FlushInstructionCache(GetCurrentProcess(), target, hook->patch_len);
    hook->trampoline = trampoline;
    hook->stub = stub;
    hook->installed = 1u;
    return 1;
}

static UINT install_state_hooks_(void) {
    UINT i, count = 0u;
    for (i = 0u; i < (UINT)(sizeof(g_state_hooks) / sizeof(g_state_hooks[0])); ++i) {
        WCHAR line[512]; line[0] = 0;
        if (install_state_hook_(&g_state_hooks[i])) {
            ++count;
            wappend_(line, 512u, (const WCHAR*)L"已安装 "); wappend_(line, 512u, marker_name_(g_state_hooks[i].marker_id));
            wappend_(line, 512u, (const WCHAR*)L" @ "); append_u32_hex_(line, 512u, g_state_hooks[i].oracle_va);
            log_line_((const WCHAR*)L"AUDIT/StateHook", line);
        } else {
            wappend_(line, 512u, (const WCHAR*)L"跳过 "); wappend_(line, 512u, marker_name_(g_state_hooks[i].marker_id));
            wappend_(line, 512u, (const WCHAR*)L" @ "); append_u32_hex_(line, 512u, g_state_hooks[i].oracle_va);
            wappend_(line, 512u, (const WCHAR*)L"：入口字节或安装条件不匹配；fail-closed，不修改该地址。");
            log_line_((const WCHAR*)L"AUDIT/StateHook", line);
        }
    }
    return count;
}

UINT GameAudit_StateHookCount(void) { return g_state_hook_count; }

UINT GameAudit_InstallDeferredStateHooks(void) {
    if (!g_ready || g_state_hook_count != 0u) return g_state_hook_count;
    g_state_hook_count = install_state_hooks_();
    return g_state_hook_count;
}

/* ---------- 崩溃/异常旁路观察 ---------- */

static LONG CALLBACK vectored_exception_(EXCEPTION_POINTERS_AUDIT_* info) {
    EXCEPTION_RECORD32_* er;
    CONTEXT32_* c;
    WCHAR line[GAME_AUDIT_LINE_CAP_];
    DWORD code;
    if (!g_ready || !info || !info->ExceptionRecord) return EXCEPTION_CONTINUE_SEARCH_;
    er = info->ExceptionRecord;
    c = info->ContextRecord;
    code = er->ExceptionCode;

    /*
     * 断点/单步等调试类异常可能被游戏或其它工具大量使用，不记；
     * 这里只记录真正有崩溃诊断价值的严重异常。
     */
    if (code != EXCEPTION_ACCESS_VIOLATION_ && code != EXCEPTION_ILLEGAL_INSTRUCTION_ &&
        code != EXCEPTION_STACK_OVERFLOW_ && code != EXCEPTION_INT_DIVIDE_BY_ZERO_ &&
        code != EXCEPTION_PRIV_INSTRUCTION_) return EXCEPTION_CONTINUE_SEARCH_;

    /* 异常可能发生在另一个线程正写日志时。拿不到锁就宁可少一行，也绝不在异常路径死锁。 */
    if (InterlockedExchange(&g_log_lock, 1L) != 0L) return EXCEPTION_CONTINUE_SEARCH_;
    line[0] = 0;
    /*
     * VEH 不能调用普通 log_line_：普通路径会一直等待日志锁，而异常现场最怕二次死锁。
     * 因此这里仍然使用 try-lock，但手工补齐与普通日志完全相同的时间、线程和分类前缀。
     * 这样用户只看 game.log 最后一段，也能精确知道“哪一个线程、启动后多少毫秒、发生了什么异常”。
     */
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"[+");
    append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, GetTickCount() - g_start_tick);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"ms][T");
    append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, GetCurrentThreadId());
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"][EXCEPTION] 严重异常 code=");
    append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, code);
    wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" address="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, er->ExceptionAddress);
    if (code == EXCEPTION_ACCESS_VIOLATION_ && er->NumberParameters >= 2u) {
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" access="); append_u32_dec_(line, GAME_AUDIT_LINE_CAP_, er->ExceptionInformation[0]);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" target="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, er->ExceptionInformation[1]);
    }
    if (c) {
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" EIP="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Eip);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ESP="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Esp);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" EBP="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Ebp);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" EAX="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Eax);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" EBX="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Ebx);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ECX="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Ecx);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" EDX="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Edx);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" ESI="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Esi);
        wappend_(line, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" EDI="); append_u32_hex_(line, GAME_AUDIT_LINE_CAP_, c->Edi);
    }
    write_wide_line_unlocked_(line);
    /*
     * 异常行比普通 I/O 行更重要，而且进程可能在几毫秒后就被系统终止。
     * 这里在仍持有 try-lock 时直接刷新日志，避免最后一条寄存器现场只停留在系统文件缓存里。
     */
    FlushFileBuffers(g_log);
    InterlockedExchange(&g_log_lock, 0L);

    /*
     * 普通 AV/非法指令等异常后再补一份游戏状态快照，可以把 EIP 现场与 Event/World 状态关联起来。
     * 但 STATUS_STACK_OVERFLOW 说明当前线程栈已经极度不足，再调用一个较大的 C 状态记录函数本身就可能二次溢出；
     * 此时只保留上面的异常寄存器行，绝不为了多一份状态快照改变原异常链。
     */
    if (code != EXCEPTION_STACK_OVERFLOW_) GameAudit_RecordStateMarker(14u);
    return EXCEPTION_CONTINUE_SEARCH_;
}

/* ---------- 初始化 ---------- */

int GameAudit_Initialize(void) {
    static const BYTE bom[3] = {0xEFu,0xBBu,0xBFu};
    DWORD wrote = 0u;
    UINT i;
    WCHAR header[GAME_AUDIT_LINE_CAP_];

    if (g_ready) return 1;
    g_game_module = GetModuleHandleW(NULL_PTR);
    if (!g_game_module) return 0;
    g_game_image_size = pe_image_size_(g_game_module);
    if (!g_game_image_size) return 0;

    /* 每次启动都清空 game.log。它与 modloader.log 一样代表“本轮唯一时间线”。 */
    g_log = CreateFileW((const WCHAR*)L"mods\\logs\\game.log", GENERIC_WRITE_, FILE_SHARE_READ_ | FILE_SHARE_WRITE_,
                        NULL_PTR, CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_, NULL_PTR);
    if (g_log == INVALID_HANDLE_VALUE_) return 0;
    WriteFile(g_log, bom, 3u, &wrote, NULL_PTR);
    g_start_tick = GetTickCount();
    g_log_lock = 0L;
    g_module_count = 0u;
    g_state_hook_count = 0u;
    g_fdebug.window_start_tick = g_fdebug.last_tick = 0u;
    g_fdebug.open_calls = g_fdebug.write_calls = g_fdebug.write_bytes = g_fdebug.close_calls = 0u;
    for (i = 0u; i < 16u; ++i) {
        g_fallback_pending[i].thread_id = g_fallback_pending[i].tick = 0u;
        g_fallback_pending[i].loose_path[0] = 0;
    }
    for (i = 0u; i < GAME_AUDIT_HANDLE_MAX_; ++i) {
        g_handles[i].handle = NULL_PTR;
        g_handles[i].quiet_fdebug = 0;
    }

    /*
     * 只注册游戏本体和原版静态依赖。ASI 以后即使也经过同一个 CreateFile Hook，caller 不在这些范围就会被过滤。
     * 这条边界正是“game.log 只记原版，modloader.log 只记 Mod”的核心。
     */
    register_module_(g_game_module, (const WCHAR*)L"RPG.exe");
    register_module_(GetModuleHandleW((const WCHAR*)L"BASS.dll"), (const WCHAR*)L"BASS.dll");
    register_module_(GetModuleHandleW((const WCHAR*)L"binkw32.dll"), (const WCHAR*)L"binkw32.dll");
    register_module_(GetModuleHandleW((const WCHAR*)L"ijl10.dll"), (const WCHAR*)L"ijl10.dll");
    /*
     * 不把 ddraw.dll / SHLWAPI.dll 注册成“原版游戏调用者”。
     * ddraw.dll 可能是用户放在 RPG.exe 同目录的第三方兼容层，SHLWAPI.dll 又是系统组件；它们自己的内部 I/O
     * 不应该混进“原版 RPG.exe 时间线”。
     *
     * dev9 虽然根据 dev5 实机 Oracle 恢复了对 ddraw.dll 的 Win32 IAT 兼容桥，但审计 caller 白名单仍然不包含 ddraw。
     * 因此 cnc-ddraw 自己写截图文件等行为不会被冒充成“RPG.exe 原版资源访问”写进 game.log。
     */

    g_ready = 1;
    log_line_((const WCHAR*)L"AUDIT", (const WCHAR*)L"《幽城幻剑录》原版游戏运行审计 v0.3.0-dev9 启动；本日志只记录原版游戏 I/O、生命周期状态与严重异常，不记录 ASI/Mod 自身行为。");
    header[0] = 0;
    wappend_(header, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L"原版模块数量="); append_u32_dec_(header, GAME_AUDIT_LINE_CAP_, g_module_count);
    wappend_(header, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" RPG基址="); append_ptr_hex_(header, GAME_AUDIT_LINE_CAP_, g_game_module);
    wappend_(header, GAME_AUDIT_LINE_CAP_, (const WCHAR*)L" SizeOfImage="); append_u32_hex_(header, GAME_AUDIT_LINE_CAP_, g_game_image_size);
    log_line_((const WCHAR*)L"AUDIT", header);

    /*
     * 把每个已经注册的原版模块基址和 SizeOfImage 都写出来。后面的 caller=模块+RVA 就能和这里直接对照；
     * 同时也能判断某个可选原版 DLL 在本轮启动时是否真的已经映射，而不是只看代码里有一张固定名单。
     */
    {
        UINT mi;
        for (mi = 0u; mi < g_module_count; ++mi) {
            WCHAR module_line[512];
            module_line[0] = 0;
            wappend_(module_line, 512u, (const WCHAR*)L"name=");
            wappend_(module_line, 512u, g_modules[mi].name);
            wappend_(module_line, 512u, (const WCHAR*)L" base=");
            append_ptr_hex_(module_line, 512u, g_modules[mi].base);
            wappend_(module_line, 512u, (const WCHAR*)L" size=");
            append_u32_hex_(module_line, 512u, g_modules[mi].size);
            log_line_((const WCHAR*)L"AUDIT/Module", module_line);
        }
    }

    log_line_((const WCHAR*)L"AUDIT", (const WCHAR*)L"读档/新游戏诊断重点：NEW_GAME_BEGIN、LOAD_READER、DESERIALIZE_CORE、REBUILD_ROLE_PTRS、RESTORE_WORLD，以及 active Event 与 deferred continuation 是否在生命周期边界正确清理。");

    /*
     * 状态入口 Hook 延后到全部 ASI 初始化之后。Runtime 若已接管某个入口，下面的严格
     * expected bytes 会让审计层跳过该点；其余入口仍保留原版审计，不影响早期 I/O 观察。
     */
    log_line_((const WCHAR*)L"AUDIT/StateHook",
              (const WCHAR*)L"生命周期入口 Hook 已延后，等待全部 ASI/Runtime 初始化完成。");
    g_vectored_handler = AddVectoredExceptionHandler(1u, (LPVOID)&vectored_exception_);
    if (g_vectored_handler)
        log_line_((const WCHAR*)L"AUDIT/Exception", (const WCHAR*)L"VEH 严重异常旁路观察已安装；只记录后继续 EXCEPTION_CONTINUE_SEARCH，不吞掉游戏异常。");
    else
        log_line_((const WCHAR*)L"AUDIT/Exception", (const WCHAR*)L"警告：VEH 安装失败；I/O 与状态断点仍可用，但崩溃寄存器现场可能缺失。");

    /* 在 RPG.exe 第一条业务代码以前记录一份基线状态，方便之后和读档/新游戏边界做差分。 */
    GameAudit_RecordStateMarker(0u);
    return 1;
}

void GameAudit_Shutdown(void) {
    /*
     * 正常退出时，最后一个 FDebug 聚合窗口可能尚未满 5 秒。
     * 如果直接让进程卸载，这几次成功写入虽然不影响游戏，但统计会少一截。
     * 所以先把当前窗口压成一条摘要，再写结束标记，最后才关闭句柄。
     *
     * 注意：严重崩溃/TerminateProcess 不保证调用 DLL_PROCESS_DETACH，因此不能把关键证据拖到这里才写。
     * 本模块的状态 Hook 和 VEH 一直都是“发生即写、关键点立即 FlushFileBuffers”，Shutdown 只负责正常收尾。
     */
    if (!g_ready || g_log == INVALID_HANDLE_VALUE_) return;
    fdebug_flush_();
    log_line_((const WCHAR*)L"AUDIT", (const WCHAR*)L"原版游戏运行审计正常收尾；未满 5 秒的 FDebug 聚合尾窗已刷新。" );
    FlushFileBuffers(g_log);
    CloseHandle(g_log);
    g_log = INVALID_HANDLE_VALUE_;
    g_ready = 0;
}
