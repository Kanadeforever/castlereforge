#include "backlog.h"
#include "runtime.h"
#include "game_addresses.h"
#include "mouse_input.h"
#include "pad_bridge.h"
#include "CastleRuntime_API.h"
#include "CastleHook_API.h"

/*
 * backlog.c
 *
 * 这是插件的核心文件。整体数据流如下：
 *
 * 原版场景更新（游戏线程）
 *   -> 未打开 Backlog 时读取当前消息页并复制到插件环形历史
 *   -> 消费 worker 提交的打开/移动/关闭请求
 *   -> 若正在查看，切换通用 F-Talk 多框与原字体姓名/正文列表
 *   -> Backlog 活动期间暂停 SceneWorld update 链，只保留绘制链
 *
 * v0.3.4-test1 保留真实剧情 / 自由探索两种载体，但统一采用“只登记绘制对象”的冻结方式：
 *
 * 1. 剧情旁路模式：
 *    原版当前已经有稳定展开的对话框时，Backlog 不伪造正文缓冲、current/target、
 *    speaker、dialogue_id 等剧情状态；只临时把 mode 置为只读并冻结 SceneWorld 逻辑；
 *    原版当前人物、姓名牌、姓名文字则分别在自己的绘制 CALL 上被临时跳过。
 *    历史内容完全由 Backlog 的绘制 Hook 画出。
 *
 * 2. 自由探索载体模式：
 *    原版没有对话框，因此仍使用 synthetic dialogue 建立一个“只供绘制”的载体。
 *
 * 两种模式活动时都不调用完整 0x40B150，而只调用 0x434500 登记 scene_world 绘制对象。
 * 这样不会推进 Event/地图逻辑，同时 0x40B050/0x404800 每帧仍有真正入口。
 *
 * 这样剧情中打开历史时，原版正在运行的消息状态机既不会看到 BACKLOG_READ_ONLY_MODE，
 * 也不会看到 speaker=0 / dialogue_id=0 / current=0 之类临时假状态；从根上避免“冻结了逻辑，
 * 却又在绘制或后台插件仍可观察的全局里制造半套状态”的崩溃。
 *
 * worker（每 8ms）
 *   -> 采样键盘，并消费 WndProc 已记录的鼠标事件
 *   -> 若 PadSupport 可用，只读取它已经采样好的按钮/模式状态
 *   -> 只增加动作计数，不解引用消息正文，不调用 RPG.exe 内部资源函数
 *
 * 这样安排有两个重要好处：
 * 1. 文本捕获和原版绘制 Hook 全部发生在 RPG.exe 原本的游戏线程；
 * 2. 输入线程不会一边复制文本，另一边正好卸载地图资源，避免跨线程悬空指针。
 */

#define BACKLOG_STATIC_CAPACITY 256u
#define BACKLOG_MAX_COMMANDS_PER_TICK 64u
#define BACKLOG_VISIBLE_ENTRIES 4u
/*
 * 原版有姓名对话不是“姓名占掉正文左边一列”。
 *
 * RPG.exe 0x40489E..0x4049FF 的机器码给出了姓名文字相对正文的实际坐标：
 *
 *   左侧姓名：
 *     姓名 X = base_x + 0x20
 *     正文 X = base_x + 0x3C
 *     所以姓名相对正文左移 28 像素。
 *
 *   右侧姓名：
 *     姓名 X = base_x + 0x1E8
 *     正文 X = base_x + 0x3C
 *     所以姓名相对正文右移 428 像素。
 *
 *   纵向：
 *     姓名 Y = base_y + 0x159
 *     正文 Y = base_y + 0x17C
 *     所以姓名在正文上方 35 像素。
 *
 * F-Name.SF2 的右侧位置与左侧位置相差 456 像素。
 *
 * 这些常量只恢复原版布局，不重新启用人物 %d-2.SF2 动画。
 */
#define BACKLOG_NAME_PANEL_RIGHT_OFFSET_X 456
#define BACKLOG_NAME_TEXT_LEFT_OFFSET_X   (-28)
#define BACKLOG_NAME_TEXT_RIGHT_OFFSET_X  428
#define BACKLOG_NAME_TEXT_OFFSET_Y        (-35)
#define BACKLOG_SPEAKER_NAME_BYTES 64u
/* 默认/最大 stride=110、最上 slot 相对底部为 -330；再小于此值就会挤出姓名上边界。 */
#define BACKLOG_MIN_SAFE_SHIFT_Y           (-330)

typedef void (BACKLOG_THISCALL *PFN_SceneWorldUpdate)(void* scene_world);
/* 0x434500 自己 ret 8，所以这里必须用 stdcall；ECX 在原函数中没有被读取。 */
typedef void (BACKLOG_STDCALL *PFN_DrawQueueRegister)(void* object, u32 key);
typedef void (BACKLOG_THISCALL *PFN_DialoguePanelDraw)(void* panel,
                                                       i32 arg1, i32 arg2, i32 arg3,
                                                       i32 arg4, i32 arg5);
typedef void (BACKLOG_THISCALL *PFN_DialogueTextDraw)(void* font,
                                                      i32 origin_x, i32 origin_y,
                                                      void* surface, i32 x, i32 y,
                                                      i32 draw_mode, const u8* text,
                                                      i32 draw_flags);

/*
 * 一条历史就是“原版一个对话框页面”的完整快照。
 * text 保留 CP950 正文和原版 /t 等控制对；播放时不转码、不重新排版。
 * speaker/style/variant 原样来自 0x403C60 的三个全局结果，用来判断是否保存姓名；
 * 现代浏览保存这些字段来恢复姓名文字左右布局；不会用它们重建人物资源。
 */
typedef struct BacklogEntry {
    u16 length;
    u8 speaker;
    u8 speaker_style;
    u8 speaker_variant;
    u8 reserved;
    /*
     * 姓名按原版 00 02 结束协议复制成独立小缓冲。
     * 现代列表把姓名画在原版姓名位置；不加载/显示人物 %d-2.SF2。
     */
    u8 speaker_name[BACKLOG_SPEAKER_NAME_BYTES];
    u8 text[DIALOGUE_BUFFER_BYTES];
} BacklogEntry;

/*
 * 打开 Backlog 前保存所有会被临时改动的原版标量和缓冲指针。
 * 关闭时按相反顺序恢复，保证剧情消息 ID、逐字进度和事件阻塞状态与打开前一致。
 */
typedef struct SavedGameState {
    u32 dialogue_id;
    u32 dialogue_mode;
    u32 total_bytes;
    u32 visible_bytes;
    u8* display_buffer;
    u8* speaker_name;

    u8 target_state;
    u8 current_state;
    u8 speaker;
    u8 speaker_style;
    u8 speaker_variant;
    u8 event_yield;
    u8 event_block;
    u8 map_input_gate;
} SavedGameState;

/* 用于判断“这一页是否已经记录过”，避免同一页每个逻辑步都重复入队。 */
typedef struct CaptureKey {
    u32 dialogue_id;
    u32 source_base;
    u32 record_offset;
    u32 page_offset;
    u32 total_bytes;
    int valid;
} CaptureKey;

/*
 * repeat 状态把“刚按一次”和“按住自动滚动”统一起来。
 * held_ticks 从 1 开始计数；到达 RepeatDelay 后，每隔 RepeatInterval 再产生一次动作。
 */
typedef struct RepeatKeyState {
    int was_down;
    u32 held_ticks;
} RepeatKeyState;

/*
 * 0x407510 每画一层都会经 0x407270/0x4072D0 改写这四个对象游标。
 * 同一 F-Talk/F-Name 在一帧画多次时，必须让每次从同一游标快照开始，并在结束后恢复。
 */
typedef struct Sf2DrawCursor {
    void* layer;
    i32 layer_index;
    void* subresource;
    i32 subresource_index;
} Sf2DrawCursor;

static BacklogEntry g_history[BACKLOG_STATIC_CAPACITY];
static u8 g_overlay_buffer[DIALOGUE_BUFFER_BYTES];
static SavedGameState g_saved;
static CaptureKey g_last_capture;

static volatile u32 g_history_count;
static u32 g_history_head;
static u32 g_selected_from_newest;
static volatile LONG g_active;
static volatile LONG g_accept_input;

/*
 * 记录本次打开时是否已经存在一条真实剧情对话。
 *
 * 0 = 自由探索载体模式：
 *     原版没有消息绘制载体，需要 synthetic dialogue 才能让 0x404800 进入绘制。
 *
 * 1 = 剧情旁路模式：
 *     原版已有真实对话，绝不能把它改造成 synthetic/只读消息。
 *     Backlog 冻结 SceneWorld 逻辑并临时使用只读 mode，保留其它真实剧情字段。
 */
static int g_opened_over_live_dialogue;

/* 每种动作使用独立计数器；InterlockedExchange 可以在消费时不丢掉并发新请求。 */
static volatile LONG g_request_open;
static volatile LONG g_request_close;
static volatile LONG g_request_up;
static volatile LONG g_request_down;
static volatile LONG g_request_left;
static volatile LONG g_request_right;

static RepeatKeyState g_key_open;
static RepeatKeyState g_key_exit;
static RepeatKeyState g_key_up;
static RepeatKeyState g_key_down;
static RepeatKeyState g_key_left;
static RepeatKeyState g_key_right;
static int g_worker_saw_active;

/*
 * PadSupport 模式切换的“必须先松键”屏障。
 *
 * 例如玩家按住 LT 调查时又一直按着 LB：Backlog 在调查模式中完全不接受手柄输入。
 * 当 LT 松开、PadSupport 回到普通 Controller 模式后，也不能把“仍然按住的 LB”误判成一次
 * 新的打开动作。因此只要 PadSupport 刚出现，或刚从被屏蔽模式回来，就要求相关按钮全部松开一次。
 */
static int g_pad_was_available;
static int g_pad_release_barrier;

static PFN_SceneWorldUpdate g_previous_scene_update;

/*
 * 原版 0x404800 的五个关键绘制 CALL 必须分别保存自己的“前一目标”。
 *
 * 为什么不能只 Hook F-Talk 和正文：
 * 剧情中打开 Backlog 时，真实对话的 speaker_active 不能再被改成 0。于是原版仍会走到：
 *   0x404859 当前人物图
 *   0x404899 当前姓名牌
 *   0x4048E6 当前姓名文字
 *
 * v0.3.3-test3 不改这些全局状态，而是在上述三个 CALL 点看到 g_active 后只跳过“当前剧情视觉”。
 * F-Talk 0x40486E 和正文 0x4049FF 则继续被展开成历史列表。
 *
 * 每个 CALL 都保存自己的 previous target，是为了兼容加载在我们前面的其它 ASI wrapper。
 * 绝不能拿 F-Talk 的 previous 去调用 F-Name；v0.3.1 已证明这种对象/调用链混用很危险。
 */
static PFN_DialoguePanelDraw g_previous_speaker_portrait_draw;
static PFN_DialoguePanelDraw g_previous_panel_draw;
static PFN_DialoguePanelDraw g_previous_name_panel_draw;
static PFN_DialogueTextDraw g_previous_name_text_draw;
static PFN_DialogueTextDraw g_previous_text_draw;
/* Runtime 模式缓存的是地址稳定的 next 槽；后续链重排只改槽内容，不改槽地址。 */
static void* volatile* g_scene_next_slot;
static void* volatile* g_speaker_portrait_next_slot;
static void* volatile* g_panel_next_slot;
static void* volatile* g_name_panel_next_slot;
static void* volatile* g_name_text_next_slot;
static void* volatile* g_text_next_slot;

static PFN_DialoguePanelDraw backlog_panel_next(void* volatile* slot,
                                                PFN_DialoguePanelDraw fallback) {
    void* current = slot ? *slot : (void*)fallback;
    return (PFN_DialoguePanelDraw)current;
}

static PFN_DialogueTextDraw backlog_text_next(void* volatile* slot,
                                              PFN_DialogueTextDraw fallback) {
    void* current = slot ? *slot : (void*)fallback;
    return (PFN_DialogueTextDraw)current;
}

static PFN_SceneWorldUpdate backlog_scene_next(void) {
    void* current = g_scene_next_slot ? *g_scene_next_slot :
                                        (void*)g_previous_scene_update;
    return (PFN_SceneWorldUpdate)current;
}

static int g_hook_installed;
static int g_speaker_portrait_hook_installed;
static int g_panel_hook_installed;
static int g_name_panel_hook_installed;
static int g_name_text_hook_installed;
static int g_text_hook_installed;
static int g_oversized_page_logged;
/*
 * 如果历史条目有姓名，但当前场景已经没有可读的 F-Name.SF2 对象，
 * 只记录一次日志并安全跳过姓名牌。
 *
 * 绝不能为了补姓名牌重新调用 0x403C60，因为它会一起重建人物 %d-2.SF2；
 * 旧版已经实机证明跨人物历史可能因此触发 SAF FrameID 越界。
 */
static int g_missing_name_panel_logged;
/*
 * 关闭分成两个 Backlog tick，防止关闭键本身穿透。
 * 剧情旁路只使用插件内部屏障，绝不再改真实 dialogue_id；自由探索 synthetic 才允许清 synthetic ID。
 */
static int g_close_barrier_pending;

/* 把一个固定字节区清零；volatile 防止编译器把循环替换成 CRT memset。 */
static void backlog_zero_bytes(u8* destination, u32 size) {
    volatile u8* output = (volatile u8*)destination;
    u32 index;
    if (!destination) return;
    for (index = 0u; index < size; ++index) output[index] = 0u;
}

/* 同样用显式循环复制，保持 /nodefaultlib 构建没有 memcpy 外部符号。 */
static void backlog_copy_bytes(u8* destination, const u8* source, u32 size) {
    volatile u8* output = (volatile u8*)destination;
    const volatile u8* input = (const volatile u8*)source;
    u32 index;
    if (!destination || !source) return;
    for (index = 0u; index < size; ++index) output[index] = input[index];
}

/* 读取全局指针槽时先取出数值，再做保守用户地址检查。 */
static int backlog_global_pointer_present(u32 address) {
    void* pointer = *(void* volatile*)address;

    /*
     * 这里只需要确认 owner 对象至少有 1 字节落在真正已提交的可读页面中。
     * 比“数值像一个指针”更严格，可以挡住已经 VirtualFree 的旧 UI 对象。
     */
    return Runtime_MemoryRangeReadable(pointer, 1u);
}

/*
 * 已知 UI 只要任意一个全局 owner 还存在，就不能把当前状态叫作“自由探索”。
 * 这里宁可多挡住一小段开关动画，也不允许 Backlog 在战斗、标题或菜单上误开。
 */
static int backlog_known_ui_present(void) {
    if (backlog_global_pointer_present(GLOBAL_BATTLE_UI)) return 1;
    if (backlog_global_pointer_present(GLOBAL_RESULT_UI)) return 1;
    if (backlog_global_pointer_present(GLOBAL_TARGET_SELECTOR)) return 1;
    if (backlog_global_pointer_present(GLOBAL_TITLE_UI)) return 1;
    if (backlog_global_pointer_present(GLOBAL_INTERFACE_UI)) return 1;
    if (backlog_global_pointer_present(GLOBAL_SAVE_POINT_UI)) return 1;
    if (backlog_global_pointer_present(GLOBAL_INN_UI)) return 1;
    if (backlog_global_pointer_present(GLOBAL_SYNTHESIS_UI)) return 1;
    if (backlog_global_pointer_present(GLOBAL_SHOP_UI)) return 1;

    /* 电影对象正常播放期间也不是自由探索；对象为空时不做任何偏移读取。 */
    {
        u8* movie = *(u8* volatile*)GLOBAL_MOVIE_OBJECT;
        if (Runtime_MemoryRangeReadable(movie, MOVIE_ACTIVE_FLAG_OFFSET + 1u) &&
            *(volatile u8*)(movie + MOVIE_ACTIVE_FLAG_OFFSET) != 0u) {
            return 1;
        }
    }
    return 0;
}

/*
 * “有对话框”是最高优先级允许条件。
 * 没有对话框时必须同时满足地图表存在、动作/事件空闲、输入门开放、角色模式有效和无已知 UI。
 * worker 会做一次快速判断，真正打开前游戏线程还会再做一次，防止检查与执行之间状态改变。
 */
static int backlog_can_open_now(void) {
    u32 dialogue_id = *(volatile u32*)GLOBAL_DIALOGUE_ID;

    /*
     * 剧情中打开必须等原版对话框进入稳定展开态。
     *
     * 旧候选曾只要 dialogue_id!=0 就允许打开，然后立即把 mode/current/target
     * 改成 Backlog 自己的值。若原版消息正在展开、逐字或切页，这相当于在状态机半途中
     * 把地板抽走，实机会在“剧情中打开”后崩溃。
     *
     * v0.3.3-test3 不再这样做：只有 current=4 且 target=4 才允许进入剧情旁路。
     * 如果玩家在过渡帧按下打开键，本次请求被安全忽略；等文字框稳定后再按一次即可。
     */
    if (dialogue_id != 0u && dialogue_id != BACKLOG_SYNTHETIC_DIALOGUE_ID) {
        u32 dialogue_mode = *(volatile u32*)GLOBAL_DIALOGUE_MODE;

        /*
         * 原版 0x404800 里 mode=2 还有一条额外的特殊绘制路径，mode=3 则已经由
         * PadSupport 的 SceneChoice 模块确认属于剧情“是/否”选择。
         *
         * v0.3.3-test3 只承诺覆盖普通稳定剧情，不应该在一个仍等待玩家选择的模态窗口
         * 上面再叠一层 Backlog。否则即使我们不写 dialogue 全局，未拦截的特殊选择 UI 仍可能
         * 和历史绘制链同时运行。
         *
         * 因此当前先允许 mode 0/1；mode 2/3 以及未来未知 mode 一律 fail-closed。
         * 这不是永久断言“0/1 的业务名是什么”，只是当前已经证明安全边界之外不抢输入。
         */
        if (dialogue_mode > 1u) return 0;

        return *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE == 4u &&
               *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE == 4u;
    }

    /*
     * synthetic ID 只可能由 Backlog 自己在活动期间写入。
     * 非活动状态看到它意味着上一次异常退出后状态没有收干净，不能再次套一层。
     */
    if (dialogue_id == BACKLOG_SYNTHETIC_DIALOGUE_ID) return 0;

    if (backlog_known_ui_present()) return 0;
    if (!backlog_global_pointer_present(GLOBAL_EVENT_TABLE)) return 0;
    if (*(volatile u32*)GLOBAL_MAP_ACTION_BUSY != 0u) return 0;
    if (*(volatile u8*)GLOBAL_EVENT_YIELD_FLAG != 0u) return 0;
    if (*(volatile u8*)GLOBAL_EVENT_BLOCK_FLAG != 0u) return 0;
    if (*(volatile u8*)GLOBAL_MAP_INPUT_GATE == 0u) return 0;
    if (*(volatile i32*)GLOBAL_MAP_KEY_MODE == 0) return 0;
    return 1;
}

/* 比较两次捕获键；字段完全相同表示仍是同一个原版页面。 */
static int backlog_capture_key_equal(const CaptureKey* left, const CaptureKey* right) {
    if (!left || !right || !left->valid || !right->valid) return 0;
    return left->dialogue_id == right->dialogue_id &&
           left->source_base == right->source_base &&
           left->record_offset == right->record_offset &&
           left->page_offset == right->page_offset &&
           left->total_bytes == right->total_bytes;
}

/*
 * 把当前 NameList 姓名复制进历史条目。
 * 原版字体不是普通 C 字符串：每个显示单元是两个字节，00 02 才表示结束。
 * 姓名前可能有奇数个 ASCII 空格，所以不能假定结束对一定落在偶数下标；
 * 这里逐字节前进并观察“当前字节=00、下一字节=02”，超过 62 字节则主动补结束对。
 */
static void backlog_capture_speaker_name(BacklogEntry* entry) {
    const volatile u8* source;
    u32 index;

    if (!entry) return;
    backlog_zero_bytes(entry->speaker_name, BACKLOG_SPEAKER_NAME_BYTES);
    entry->speaker_name[1] = 0x02u;
    if (entry->speaker == 0u) return;

    source = *(u8* volatile*)GLOBAL_DIALOGUE_SPEAKER_NAME;
    /*
     * 旧版只检查 source 这个“起点数值”，如果名字跨到未提交页面仍可能崩。
     * 这里先确认最多会读取的 64 字节整段都可读。
     */
    if (!Runtime_MemoryRangeReadable((const void*)source, BACKLOG_SPEAKER_NAME_BYTES)) return;

    for (index = 0u; index + 3u < BACKLOG_SPEAKER_NAME_BYTES; ++index) {
        u8 first = source[index];
        u8 second = source[index + 1u];
        entry->speaker_name[index] = first;
        if (first == 0u && second == 0x02u) {
            entry->speaker_name[index + 1u] = second;
            return;
        }
    }

    entry->speaker_name[BACKLOG_SPEAKER_NAME_BYTES - 2u] = 0u;
    entry->speaker_name[BACKLOG_SPEAKER_NAME_BYTES - 1u] = 0x02u;
}

/*
 * 游戏线程捕获当前页。
 *
 * 原版页面正文地址 = source_base + record_offset + page_offset。
 * 这三个值都由 0x403B90/0x4046F0 建立；total_bytes 是本页而不是整条 MSG 的长度。
 * 只有消息 ID 非零且所有加法没有 32 位溢出时才复制，绝不猜未知内存。
 */
static void backlog_capture_current_page(void) {
    CaptureKey key;
    BacklogEntry* entry;
    const RuntimeConfig* config = Runtime_Config();
    u32 source_address;
    u32 after_record;
    u32 index;

    if (g_active) return;
    key.dialogue_id = *(volatile u32*)GLOBAL_DIALOGUE_ID;
    if (key.dialogue_id == 0u || key.dialogue_id == BACKLOG_SYNTHETIC_DIALOGUE_ID) {
        g_last_capture.valid = 0;
        return;
    }

    key.source_base = *(volatile u32*)GLOBAL_DIALOGUE_SOURCE_BASE;
    key.record_offset = *(volatile u32*)GLOBAL_DIALOGUE_RECORD_OFFSET;
    key.page_offset = *(volatile u32*)GLOBAL_DIALOGUE_PAGE_OFFSET;
    key.total_bytes = *(volatile u32*)GLOBAL_DIALOGUE_TOTAL_BYTES;
    key.valid = 1;

    if (backlog_capture_key_equal(&key, &g_last_capture)) return;
    if (key.total_bytes == 0u) return;
    if (key.total_bytes > DIALOGUE_PAYLOAD_MAX_BYTES) {
        if (!g_oversized_page_logged) {
            g_oversized_page_logged = 1;
            Runtime_Log("[记录] 遇到超过原版 511 字节显示缓冲的页面；为安全起见本页未入历史。");
        }
        return;
    }
    if (!Runtime_PointerLooksReadable((const void*)key.source_base)) return;

    after_record = key.source_base + key.record_offset;
    if (after_record < key.source_base) return;
    source_address = after_record + key.page_offset;
    if (source_address < after_record) return;
    /*
     * 一次验证整页正文，而不是只验证首尾两个地址数值。
     * 这样中间跨过 PAGE_NOACCESS / 未提交区时也会被拒绝。
     */
    if (!Runtime_MemoryRangeReadable((const void*)source_address, key.total_bytes)) return;

    /* head 永远指向下一格可写槽；达到 INI 容量后从 0 开始覆盖最旧记录。 */
    entry = &g_history[g_history_head];
    entry->length = (u16)key.total_bytes;
    entry->speaker = *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_ACTIVE;
    entry->speaker_style = *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_STYLE;
    entry->speaker_variant = *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_VARIANT;
    entry->reserved = 0u;
    backlog_capture_speaker_name(entry);

    backlog_zero_bytes(entry->text, DIALOGUE_BUFFER_BYTES);
    backlog_copy_bytes(entry->text, (const u8*)source_address, key.total_bytes);
    entry->text[DIALOGUE_BUFFER_BYTES - 1u] = 0x02u;

    g_history_head = (g_history_head + 1u) % config->max_entries;
    if (g_history_count < config->max_entries) ++g_history_count;

    /* 逐字段复制捕获键，避免结构赋值被编译器优化成 CRT memcpy。 */
    g_last_capture.dialogue_id = key.dialogue_id;
    g_last_capture.source_base = key.source_base;
    g_last_capture.record_offset = key.record_offset;
    g_last_capture.page_offset = key.page_offset;
    g_last_capture.total_bytes = key.total_bytes;
    g_last_capture.valid = 1;

    /* index 只用于让初学者清楚：正文之外的剩余字节已经在上面的 zero 循环清完。 */
    index = key.total_bytes;
    (void)index;
}

/* 由“距最新多少条”换算环形数组真实下标。offset=0 永远是刚记录的最新一条。 */
static BacklogEntry* backlog_entry_from_newest(u32 offset) {
    const RuntimeConfig* config = Runtime_Config();
    u32 count = g_history_count;
    u32 newest;
    u32 index;

    if (count == 0u || offset >= count) return NULL;
    newest = (g_history_head + config->max_entries - 1u) % config->max_entries;
    index = (newest + config->max_entries - offset) % config->max_entries;
    return &g_history[index];
}

/* 当前滚动锚点及其更旧记录中，最多取四条放进一个 640×480 画面。 */
static u32 backlog_visible_slot_capacity(void) {
    /*
     * 完整原版对话框很高。间距超过 110 后继续塞四条不仅难看，最上姓名还会越过屏幕。
     * 改成三条后，最大 stride=160 的最上姓名仍有 25 像素上边距。
     */
    return Runtime_Config()->panel_stride_y > 110u ? 3u : BACKLOG_VISIBLE_ENTRIES;
}

/* 当前滚动锚点及其更旧记录中，按当前间距最多取三条或四条。 */
static u32 backlog_visible_entry_count(void) {
    u32 count = g_history_count;
    u32 available;
    u32 capacity = backlog_visible_slot_capacity();
    if (count == 0u || g_selected_from_newest >= count) return 0u;
    available = count - g_selected_from_newest;
    return available < capacity ? available : capacity;
}

/*
 * order=0 表示当前画面最上方的记录；最后一个 order 是最下方、也是当前滚动锚点。
 * 记录不足四条时向屏幕底部对齐，保留原游戏“消息从下方出现”的视觉方向。
 */
static BacklogEntry* backlog_visible_entry(u32 order, u32* out_panel_slot) {
    u32 visible = backlog_visible_entry_count();
    u32 capacity = backlog_visible_slot_capacity();
    u32 offset;
    if (visible == 0u || order >= visible) return NULL;

    offset = g_selected_from_newest + (visible - 1u - order);
    if (out_panel_slot) *out_panel_slot = capacity - visible + order;
    return backlog_entry_from_newest(offset);
}

/*
 * 用 INI 计算槽位纵向位移后再做第二道硬夹紧。
 * 即使配置结构被旧 INI 或其它内存错误写成大数，最上方也不会超过 -330。
 */
static i32 backlog_panel_shift_y(u32 panel_slot) {
    u32 capacity = backlog_visible_slot_capacity();
    i32 shift;
    if (panel_slot >= capacity) panel_slot = capacity - 1u;
    shift =
        ((i32)panel_slot - ((i32)capacity - 1)) *
        (i32)Runtime_Config()->panel_stride_y;
    if (shift < BACKLOG_MIN_SAFE_SHIFT_Y) shift = BACKLOG_MIN_SAFE_SHIFT_Y;
    return shift;
}

/* 保存 0x407510 会改写的 +0x50/+0x54/+0x58/+0x5C。 */
static int backlog_save_sf2_cursor(void* object, Sf2DrawCursor* saved) {
    u8* bytes = (u8*)object;
    if (!saved || !Runtime_MemoryRangeReadable(object, 0x60u)) return 0;
    saved->layer = *(void**)(bytes + 0x50u);
    saved->layer_index = *(i32*)(bytes + 0x54u);
    saved->subresource = *(void**)(bytes + 0x58u);
    saved->subresource_index = *(i32*)(bytes + 0x5Cu);
    return 1;
}

/* 恢复同一组游标；只对已经通过 0x60 字节范围检查的对象调用。 */
static void backlog_restore_sf2_cursor(void* object, const Sf2DrawCursor* saved) {
    u8* bytes = (u8*)object;
    if (!object || !saved) return;
    *(void**)(bytes + 0x50u) = saved->layer;
    *(i32*)(bytes + 0x54u) = saved->layer_index;
    *(void**)(bytes + 0x58u) = saved->subresource;
    *(i32*)(bytes + 0x5Cu) = saved->subresource_index;
}

/*
 * 取得原版当前仍存活的 F-Name.SF2 对象。
 *
 * 这个对象由原版 0x403C60 创建，并不会在一句剧情结束时立即释放；切换下一个说话人时才会
 * 先析构旧对象、再把新对象写回同一个 0x46F658 槽。因此“剧情结束后查看刚才的历史”仍可
 * 使用最后一只原版 F-Name，而不需要 Backlog 私自构造资源。
 *
 * 但只检查地址非空还不够。0x407510 入口马上读取 object+0x48，再读取 frame+0x54，
 * 所以下面逐层证明对象、资源和当前 frame 都在可读内存中。任一项失败就返回 NULL，
 * 本帧只画姓名文字和正文，绝不把可疑对象交给原版绘制函数。
 */
static void* backlog_original_name_panel(void) {
    u8* object = *(u8* volatile*)GLOBAL_DIALOGUE_NAME_PANEL_OBJECT;
    void* resource_a;
    void* resource_b;
    void* frame;

    if (!Runtime_MemoryRangeReadable(object, 0x84u)) return NULL;

    resource_a = *(void**)(object + 0x30u);
    resource_b = *(void**)(object + 0x34u);
    if (!resource_a || resource_a != resource_b) return NULL;
    if (!Runtime_MemoryRangeReadable(resource_a, 0x38u)) return NULL;

    frame = *(void**)(object + 0x48u);
    if (!Runtime_MemoryRangeReadable(frame, 0x56u)) return NULL;
    return object;
}

/*
 * 0x404859：原版“当前说话人物图”绘制 CALL。
 *
 * Backlog 未打开时完全链回前一目标；打开后只跳过这一笔“当前剧情人物图”。
 * 这里不改 GLOBAL_DIALOGUE_SPEAKER_ACTIVE，所以剧情状态机和其它后台插件始终看到
 * 打开前真实值。可以把它理解成：不把电灯开关拆掉，只在历史界面盖住这盏灯。
 */
static void BACKLOG_THISCALL Backlog_HookCurrentSpeakerPortraitDraw(void* panel,
                                                                    i32 arg1, i32 arg2, i32 arg3,
                                                                    i32 arg4, i32 arg5) {
    PFN_DialoguePanelDraw previous = backlog_panel_next(
        g_speaker_portrait_next_slot, g_previous_speaker_portrait_draw);
    if (!previous) return;
    if (g_active) return;
    previous(panel, arg1, arg2, arg3, arg4, arg5);
}

/*
 * 0x404899：原版“当前剧情 F-Name.SF2”绘制 CALL。
 *
 * - 原版来到 0x404899 时，本函数在 Backlog 活动期间直接 return，隐藏当前剧情那一块姓名牌；
 * - Backlog_HookPanelDraw 画历史组合框时，使用同一只已验证的原版 F-Name 对象，直接调用
 *   g_previous_name_panel_draw，因此不会再次绕回这个 Hook。
 */
static void BACKLOG_THISCALL Backlog_HookCurrentNamePanelDraw(void* panel,
                                                               i32 arg1, i32 arg2, i32 arg3,
                                                               i32 arg4, i32 arg5) {
    PFN_DialoguePanelDraw previous = backlog_panel_next(
        g_name_panel_next_slot, g_previous_name_panel_draw);
    if (!previous) return;
    if (g_active) return;
    previous(panel, arg1, arg2, arg3, arg4, arg5);
}

/*
 * 0x4048E6：原版“当前剧情姓名文字”绘制 CALL。
 *
 * 历史姓名文字不是从这里画：Backlog_HookTextDraw 会直接用历史条目的 speaker_name
 * 调用正文绘制链。因此 Backlog 活动时安全跳过这一笔即可。
 */
static void BACKLOG_THISCALL Backlog_HookCurrentNameTextDraw(void* font,
                                                              i32 origin_x, i32 origin_y,
                                                              void* surface, i32 x, i32 y,
                                                              i32 draw_mode, const u8* text,
                                                              i32 draw_flags) {
    PFN_DialogueTextDraw previous = backlog_text_next(
        g_name_text_next_slot, g_previous_name_text_draw);
    if (!previous) return;
    if (g_active) return;
    previous(font, origin_x, origin_y, surface,
             x, y, draw_mode, text, draw_flags);
}

/*
 * 0x40486E 原本负责绘制 F-Talk.SF2。
 *
 * v0.3.3-test4 的私有 NamePanelPool 走不通：它把裸 C 字符串交给要求游戏字符串对象的
 * 0x4070D0，剧情结束后打开会稳定破坏资源加载链。现行方案不再创建任何 SF2 对象。
 *
 * 有姓名历史严格复用原版“F-Talk + F-Name + 姓名文字 + 正文”组合：
 * - F-Talk 使用本 CALL 已经传入的原版对象；
 * - F-Name 使用 0x46F658 最后一只原版已加载对象，并经过完整对象/资源/frame 可读检查；
 * - 姓名与正文分别走原版 0x4048E6、0x4049FF 的当前链目标；
 * - 人物图仍在 0x404859 单独屏蔽。
 */
static void BACKLOG_THISCALL Backlog_HookPanelDraw(void* panel,
                                                    i32 arg1, i32 arg2, i32 arg3,
                                                    i32 arg4, i32 arg5) {
    PFN_DialoguePanelDraw previous_panel = backlog_panel_next(
        g_panel_next_slot, g_previous_panel_draw);
    PFN_DialoguePanelDraw previous_name_panel = backlog_panel_next(
        g_name_panel_next_slot, g_previous_name_panel_draw);
    u8* original_name_panel;
    Sf2DrawCursor talk_cursor;
    Sf2DrawCursor name_cursor;
    int talk_cursor_saved;
    int name_cursor_saved = 0;
    i32 original_x;
    i32 original_y;
    i32 original_name_x = 0;
    i32 original_name_y = 0;
    u32 visible;
    u32 order;

    if (!previous_panel) return;

    /*
     * Backlog 没打开时，这个 Hook 必须表现得像不存在一样。
     * 正常剧情仍只执行原来 0x40486E 那一次 F-Talk 绘制。
     */
    if (!g_active) {
        previous_panel(panel, arg1, arg2, arg3, arg4, arg5);
        return;
    }

    /*
     * 除 X/Y 外还要保存 0x407510 会改写到 +0x5C 的游标，所以必须证明前 0x60 字节可读。
     * 若条件不满足，就不做多框展开，直接把原调用交回前一层。
     */
    if (!Runtime_MemoryRangeReadable(panel, 0x60u)) {
        previous_panel(panel, arg1, arg2, arg3, arg4, arg5);
        return;
    }

    talk_cursor_saved = backlog_save_sf2_cursor(panel, &talk_cursor);
    if (!talk_cursor_saved) {
        previous_panel(panel, arg1, arg2, arg3, arg4, arg5);
        return;
    }

    original_x = *(i32*)((u8*)panel + 0u);
    original_y = *(i32*)((u8*)panel + 4u);
    original_name_panel = (u8*)backlog_original_name_panel();
    if (original_name_panel) {
        original_name_x = *(i32*)(original_name_panel + 0u);
        original_name_y = *(i32*)(original_name_panel + 4u);
        name_cursor_saved = backlog_save_sf2_cursor(original_name_panel, &name_cursor);
        if (!name_cursor_saved) original_name_panel = NULL;
    }
    visible = backlog_visible_entry_count();

    for (order = 0u; order < visible; ++order) {
        BacklogEntry* entry;
        u32 panel_slot = 0u;
        i32 shift_y;
        int has_name;

        entry = backlog_visible_entry(order, &panel_slot);
        if (!entry) continue;

        /*
         * panel_slot 固定是 0..3。
         * 最下面一条使用 slot=3，所以 shift_y=0；越旧的记录依次向上移动 PanelStrideY。
         */
        shift_y = backlog_panel_shift_y(panel_slot);

        /* 先画这条记录对应的 F-Talk 主框。 */
        backlog_restore_sf2_cursor(panel, &talk_cursor);
        *(i32*)((u8*)panel + 0u) = original_x;
        *(i32*)((u8*)panel + 4u) = original_y + shift_y;
        previous_panel(panel, arg1, arg2, arg3, arg4, arg5);

        /*
         * speaker!=0 且姓名缓冲不只是原版结束标记 00 02，才需要姓名牌。
         * 这里直接读历史快照，不读取任何“当前 NPC”全局对象。
         */
        has_name =
            entry->speaker != 0u &&
            !(entry->speaker_name[0] == 0u &&
              entry->speaker_name[1] == 0x02u);

        if (has_name && previous_name_panel) {
            if (original_name_panel) {
                i32 name_target_x = original_x;

                /*
                 * F-Name 是原版通用姓名框资源。每条绘制只临时改坐标，循环结束后恢复；
                 * 不加载资源、不切换 speaker，也不制造第二套对象生命周期。
                 */
                if (entry->speaker_variant != 0u) {
                    name_target_x += BACKLOG_NAME_PANEL_RIGHT_OFFSET_X;
                }

                backlog_restore_sf2_cursor(original_name_panel, &name_cursor);
                *(i32*)(original_name_panel + 0u) = name_target_x;
                *(i32*)(original_name_panel + 4u) = original_y + shift_y;

                /*
                 * 姓名框仍走真正的 0x404899 当前 CALL 目标。
                 * 如果宽屏等插件已经链在该调用点，我们继续尊重它的 wrapper。
                 */
                previous_name_panel(original_name_panel, arg1, arg2, arg3, arg4, arg5);
            } else if (!g_missing_name_panel_logged) {
                g_missing_name_panel_logged = 1;
                Runtime_Log(
                    "[Backlog] 原版 F-Name 姓名框当前不可读；"
                    "本帧安全退化为姓名文字+正文，不创建私有 SF2。"
                );
            }
        }
    }

    /* F-Talk 属于游戏自己，必须恢复它进入 Hook 前的原坐标。 */
    *(i32*)((u8*)panel + 0u) = original_x;
    *(i32*)((u8*)panel + 4u) = original_y;
    backlog_restore_sf2_cursor(panel, &talk_cursor);
    if (original_name_panel) {
        *(i32*)(original_name_panel + 0u) = original_name_x;
        *(i32*)(original_name_panel + 4u) = original_name_y;
        backlog_restore_sf2_cursor(original_name_panel, &name_cursor);
    }
}

/* 姓名缓冲只含 00 02 时表示本条没有可显示姓名。 */
static int backlog_entry_has_speaker_name(const BacklogEntry* entry) {
    if (!entry || entry->speaker == 0u) return 0;
    return !(entry->speaker_name[0] == 0u && entry->speaker_name[1] == 0x02u);
}

/*
 * 0x4049FF 原本只绘制当前页正文。
 *
 * v0.2.0 的错误是把姓名与正文画在同一行，再把正文向右推 96 像素。
 * 原版机器码实际坐标为：
 *
 *   正文：     X=base+0x3C，Y=base+0x17C
 *   左姓名：   X=base+0x20，Y=base+0x159  -> 相对正文 (-28, -35)
 *   右姓名：   X=base+0x1E8，Y=base+0x159 -> 相对正文 (+428, -35)
 *
 * 所以正文有没有姓名都必须使用同一个 X；姓名是独立画在正文上方的。
 */
static void BACKLOG_THISCALL Backlog_HookTextDraw(void* font,
                                                   i32 origin_x, i32 origin_y,
                                                   void* surface, i32 x, i32 y,
                                                   i32 draw_mode, const u8* text,
                                                   i32 draw_flags) {
    PFN_DialogueTextDraw previous_text = backlog_text_next(
        g_text_next_slot, g_previous_text_draw);
    PFN_DialogueTextDraw previous_name_text = backlog_text_next(
        g_name_text_next_slot, g_previous_name_text_draw);
    u32 visible;
    u32 order;

    if (!previous_text) return;

    /* 正常游戏状态完全透传。 */
    if (!g_active) {
        previous_text(font, origin_x, origin_y, surface, x, y,
                      draw_mode, text, draw_flags);
        return;
    }

    visible = backlog_visible_entry_count();

    for (order = 0u; order < visible; ++order) {
        BacklogEntry* entry;
        u32 panel_slot = 0u;
        i32 shift_y;

        entry = backlog_visible_entry(order, &panel_slot);
        if (!entry) continue;

        shift_y = backlog_panel_shift_y(panel_slot);

        /* 有姓名时，姓名独立画在正文上方，不占正文宽度。 */
        if (backlog_entry_has_speaker_name(entry) && previous_name_text) {
            i32 name_x;
            i32 name_y;

            /* 原版以 speaker_style < 0x10 区分左/右姓名文字。 */
            if (entry->speaker_style < 0x10u) {
                name_x = x + BACKLOG_NAME_TEXT_LEFT_OFFSET_X;
            } else {
                name_x = x + BACKLOG_NAME_TEXT_RIGHT_OFFSET_X;
            }
            name_y = y + shift_y + BACKLOG_NAME_TEXT_OFFSET_Y;
            /* 最终屏幕坐标再夹一次；任何配置/状态异常都不能把姓名交给负 Y。 */
            if (name_y < 0) name_y = 0;

            /* 姓名走原版 0x4048E6 的前一目标，不借用正文 CALL 的插件链。 */
            previous_name_text(font, origin_x, origin_y, surface,
                               name_x, name_y, draw_mode,
                               entry->speaker_name, draw_flags);
        }

        /*
         * 核心修复：正文始终使用原版 x。
         * 不再有 body_x，也不再 +96。
         */
        previous_text(font, origin_x, origin_y, surface,
                      x, y + shift_y, draw_mode,
                      entry->text, draw_flags);
    }
}

/*
 * 把选中历史复制进插件自己的 512 字节缓冲，再原子地让原版全局指针指向它。
 * 原消息自己的缓冲从不被覆盖，所以关闭时只要恢复指针就能得到打开前的逐字内容。
 */
static void backlog_apply_selected_entry(void) {
    BacklogEntry* entry = backlog_entry_from_newest(g_selected_from_newest);

    /*
     * 剧情旁路模式根本不需要把某条历史塞进原版消息缓冲。
     * 绘制 Hook 会直接从环形历史读取当前可见四条。
     *
     * 这一道 return 是剧情旁路的关键安全边界：
     * 只要打开前存在真实剧情，对 mode/total/visible/display/current/target 的写入全部禁止。
     */
    if (g_opened_over_live_dialogue) return;

    if (!entry) return;

    /*
     * 环形历史正常情况下 length 永远来自 1..511 的已验证原版页面。
     * 这里仍做第二道硬门：即使内存被别的错误踩坏，也不能拿一个异常 length 去复制。
     */
    if (entry->length == 0u || entry->length > DIALOGUE_PAYLOAD_MAX_BYTES) {
        Runtime_Log("[安全] 历史条目的正文长度异常；本次不切换该条，避免越界复制。");
        return;
    }

    backlog_zero_bytes(g_overlay_buffer, DIALOGUE_BUFFER_BYTES);
    backlog_copy_bytes(g_overlay_buffer, entry->text, (u32)entry->length);
    g_overlay_buffer[DIALOGUE_BUFFER_BYTES - 1u] = 0x02u;

    /*
     * 现代列表明确不显示人物图，也绝不在倒记录时调用 0x403C60 重建 %d-2.SF2。
     * 这里只把原版“本帧是否画人物/姓名框”的标志临时设 0；当前剧情已经加载的资源对象原封不动保留，
     * 从而修复实机出现的 FrameID >= TotalFrameNumber 跨人物动画错误。
     */
    *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_ACTIVE = 0u;

    *(volatile u32*)GLOBAL_DIALOGUE_MODE = BACKLOG_READ_ONLY_MODE;
    *(volatile u32*)GLOBAL_DIALOGUE_TOTAL_BYTES = (u32)entry->length;
    *(volatile u32*)GLOBAL_DIALOGUE_VISIBLE_BYTES = (u32)entry->length;
    *(u8* volatile*)GLOBAL_DIALOGUE_DISPLAY_BUFFER = g_overlay_buffer;

    /* target/current 最后才变成 4，避免绘制线程在缓冲与说话人尚未准备好时看到半成品。 */
    *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE = 4u;
    *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE = 4u;
}

/* 保存打开前的精确原版状态；每个字段都与关闭函数一一对应。 */
static void backlog_save_game_state(void) {
    g_saved.dialogue_id = *(volatile u32*)GLOBAL_DIALOGUE_ID;
    g_saved.dialogue_mode = *(volatile u32*)GLOBAL_DIALOGUE_MODE;
    g_saved.total_bytes = *(volatile u32*)GLOBAL_DIALOGUE_TOTAL_BYTES;
    g_saved.visible_bytes = *(volatile u32*)GLOBAL_DIALOGUE_VISIBLE_BYTES;
    g_saved.display_buffer = *(u8* volatile*)GLOBAL_DIALOGUE_DISPLAY_BUFFER;
    g_saved.speaker_name = *(u8* volatile*)GLOBAL_DIALOGUE_SPEAKER_NAME;
    g_saved.target_state = *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE;
    g_saved.current_state = *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE;
    g_saved.speaker = *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_ACTIVE;
    g_saved.speaker_style = *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_STYLE;
    g_saved.speaker_variant = *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_VARIANT;
    g_saved.event_yield = *(volatile u8*)GLOBAL_EVENT_YIELD_FLAG;
    g_saved.event_block = *(volatile u8*)GLOBAL_EVENT_BLOCK_FLAG;
    g_saved.map_input_gate = *(volatile u8*)GLOBAL_MAP_INPUT_GATE;
}

/*
 * 游戏线程执行真正打开：先再检查资格和历史数量，再建立原版同款模态门。
 * 在自由探索时用 synthetic ID 让宽屏插件也把本界面识别成消息 UI，避免左右 world 重放三份对话框。
 */
static void backlog_open_on_game_thread(void) {
    if (g_active || g_history_count == 0u || !backlog_can_open_now()) return;

    backlog_save_game_state();
    g_missing_name_panel_logged = 0;
    g_selected_from_newest = 0u;

    /*
     * dialogue_id 非零且不是 synthetic，说明此刻真的有一条原版剧情对话。
     * 这个判断使用的是刚保存的快照，后面任何临时写入都不会改变“本次打开属于哪种模式”。
     */
    g_opened_over_live_dialogue =
        g_saved.dialogue_id != 0u &&
        g_saved.dialogue_id != BACKLOG_SYNTHETIC_DIALOGUE_ID;

    /*
     * 从本函数返回后，Backlog_HookSceneUpdate 会看到 g_active=1，因此不再调用
     * 原版/前一个插件的 SceneWorld update；游戏逻辑停在打开前同一快照。
     *
     * 重要：剧情旁路和自由探索从这里开始严格分开。
     */
    g_active = 1;

    if (g_opened_over_live_dialogue) {
        /*
         * 真实剧情已经有可用的 F-Talk/F-Name/字体对象，不需要 synthetic dialogue。
         *
         * 下面这些打开前保存的值，在 Backlog 整个活动期间一个字节都不改：
         *   dialogue_id / total / visible / display_buffer
         *   speaker / speaker_style / speaker_variant / speaker_name
         *   current / target
         *   event_yield / event_block / map_input_gate
         *
         * 唯一例外是 mode：0x40B050 的绘制路径内部还会调用 0x403E30 更新消息。
         * 我们必须把 mode 暂时换成无效的只读值，让 0x403E30 跳过推进/选择业务；关闭时恢复。
         * 当前人物图、姓名牌和姓名文字则由 0x404859、0x404899、0x4048E6 三个绘制 Hook
         * 只跳过“当前剧情那一笔”，历史组合框由后续 Hook 自己画。
         */
        *(volatile u32*)GLOBAL_DIALOGUE_MODE = BACKLOG_READ_ONLY_MODE;
        Runtime_Log(
            "[剧情旁路] 已在稳定剧情对话上打开：只把 mode 置为只读；"
            "SceneWorld 逻辑冻结，原版有名字+对白组合绘制继续登记。"
        );
    } else {
        /*
         * 自由探索没有真实对话状态机，所以需要主动建立只读绘制载体。
         * 只有这一条路径才改事件/地图门和 dialogue 全局。
         */
        *(volatile u8*)GLOBAL_MAP_INPUT_GATE = 0u;
        *(volatile u8*)GLOBAL_EVENT_YIELD_FLAG = 1u;
        *(volatile u8*)GLOBAL_EVENT_BLOCK_FLAG = 1u;
        *(volatile u32*)GLOBAL_DIALOGUE_ID = BACKLOG_SYNTHETIC_DIALOGUE_ID;
        backlog_apply_selected_entry();

        Runtime_Log(
            "[自由探索载体] 已建立 synthetic dialogue；"
            "现行版只登记 SceneWorld draw 维持原版消息绘制载体，"
            "不会运行完整 SceneWorld update；地图/事件输入仍保持门控。"
        );
    }

    Runtime_Log("[Backlog] 已打开；当前显示最新一条记录，游戏逻辑已冻结，绘制链继续运行。");
}

/*
 * 关闭仍分成两个 Backlog tick，目的是避免“按下关闭键的同一瞬间”穿透到刚恢复的游戏。
 *
 * 但 v0.3.3-test3 明确区分：
 * - 剧情旁路：屏障只能存在于 Backlog 自己，绝不再清真实 dialogue_id；
 * - 自由探索 synthetic：synthetic ID 没有真实剧情所有权，仍允许先清零再恢复。
 */
static void backlog_begin_close_barrier(void) {
    if (!g_active || g_close_barrier_pending) return;

    /*
     * 剧情旁路绝不能再用旧版“dialogue_id 临时清零”技巧。
     * 用户已经实机证明剧情中打开会崩，而 dialogue_id 正是其它消息/输入插件会异步观察的核心状态。
     *
     * 这里仍保留一个 Backlog 自己的一帧关闭屏障，但只改插件自己的标志，不碰 RPG.exe。
     * 对话保持完整冻结；下一次 SceneWorld Hook 进入时再执行真正关闭。
     */
    if (g_opened_over_live_dialogue) {
        g_close_barrier_pending = 1;
        Runtime_Log(
            "[剧情旁路] 退出请求已进入一帧插件内部释放屏障；"
            "原版 dialogue_id 和其它剧情全局保持原值。"
        );
        return;
    }

    /*
     * 自由探索 synthetic 载体没有真实剧情所有权。
     * 这里仍可把 synthetic ID 清零，让其它输入插件先观察到“消息已结束”，再下一帧恢复打开前快照。
     */
    *(volatile u32*)GLOBAL_DIALOGUE_ID = 0u;
    g_close_barrier_pending = 1;
    Runtime_Log(
        "[自由探索载体] 退出请求已进入一帧释放屏障；synthetic dialogue_id 已清零。"
    );
}

/*
 * 恢复时先把 current/target 清零，阻止任何中间态被绘制；然后恢复缓冲、说话人资源和全部标量。
 * 人物显示标量和姓名指针只做直接恢复，因为人物资源对象从始至终都没有被卸载或替换。
 */
static void backlog_close_on_game_thread(void) {
    int was_live_dialogue;

    if (!g_active) return;
    was_live_dialogue = g_opened_over_live_dialogue;

    /*
     * 两种模式的关闭必须继续保持所有权边界：
     *
     * 剧情旁路：
     *   打开以来没有改过任何 RPG.exe 对话/事件/地图全局，因此这里也不需要“恢复”。
     *   只清自己的 active 标志即可；没有任何私有 SF2 对象需要销毁。
     *
     * 自由探索 synthetic：
     *   这条路径确实改过 dialogue/event/map 全局，所以仍按打开前快照完整恢复。
     */
    if (was_live_dialogue) {
        /* 打开期间唯一改过的真实剧情字段是 mode；先恢复，再解除 Backlog active。 */
        *(volatile u32*)GLOBAL_DIALOGUE_MODE = g_saved.dialogue_mode;
        g_close_barrier_pending = 0;
        g_opened_over_live_dialogue = 0;
        g_active = 0;

        Runtime_Log(
            "[剧情旁路] 已关闭：只读 mode 已恢复；"
            "真实对话继续保持打开前同一页面和人物资源。"
        );
        Runtime_Log("[Backlog] 已关闭；下一次 SceneWorld tick 恢复原游戏逻辑。");
        return;
    }

    /*
     * synthetic 模式自己的消息框仍可能正在绘制，所以先把 current/target 清零，
     * 再恢复打开前没有对话的真实状态；现行方案没有私有姓名框资源。
     */
    *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE = 0u;
    *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE = 0u;

    *(u8* volatile*)GLOBAL_DIALOGUE_DISPLAY_BUFFER = g_saved.display_buffer;
    *(u8* volatile*)GLOBAL_DIALOGUE_SPEAKER_NAME = g_saved.speaker_name;
    *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_ACTIVE = g_saved.speaker;
    *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_STYLE = g_saved.speaker_style;
    *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_VARIANT = g_saved.speaker_variant;

    *(volatile u32*)GLOBAL_DIALOGUE_MODE = g_saved.dialogue_mode;
    *(volatile u32*)GLOBAL_DIALOGUE_TOTAL_BYTES = g_saved.total_bytes;
    *(volatile u32*)GLOBAL_DIALOGUE_VISIBLE_BYTES = g_saved.visible_bytes;
    *(volatile u32*)GLOBAL_DIALOGUE_ID = g_saved.dialogue_id;

    *(volatile u8*)GLOBAL_EVENT_YIELD_FLAG = g_saved.event_yield;
    *(volatile u8*)GLOBAL_EVENT_BLOCK_FLAG = g_saved.event_block;
    *(volatile u8*)GLOBAL_MAP_INPUT_GATE = g_saved.map_input_gate;

    *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE = g_saved.target_state;
    *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE = g_saved.current_state;

    g_close_barrier_pending = 0;
    g_opened_over_live_dialogue = 0;
    g_active = 0;

    Runtime_Log(
        "[自由探索载体] synthetic dialogue 已销毁；打开前地图/事件/消息快照已恢复。"
    );
    Runtime_Log("[Backlog] 已关闭；下一次 SceneWorld tick 恢复原游戏逻辑。");
}

/* ↑：向更旧移动一格；已经在最旧处时环回最新。 */
static void backlog_move_up_once(void) {
    u32 count = g_history_count;
    if (count <= 1u) return;
    if (g_selected_from_newest + 1u >= count) g_selected_from_newest = 0u;
    else ++g_selected_from_newest;
}

/* ↓：向更新移动一格；已经在最新处时环回最旧。 */
static void backlog_move_down_once(void) {
    u32 count = g_history_count;
    if (count <= 1u) return;
    if (g_selected_from_newest == 0u) g_selected_from_newest = count - 1u;
    else --g_selected_from_newest;
}

/* ←：向旧记录跳一页；不足整页时停在最旧记录，不额外环回。 */
static void backlog_page_left_once(void) {
    u32 count = g_history_count;
    u32 step = Runtime_Config()->page_size;
    u32 capacity = backlog_visible_slot_capacity();
    if (step > capacity) step = capacity;
    if (count <= 1u) return;
    if (g_selected_from_newest + step >= count) g_selected_from_newest = count - 1u;
    else g_selected_from_newest += step;
}

/* →：向新记录跳一页；不足整页时停在最新记录。 */
static void backlog_page_right_once(void) {
    u32 step = Runtime_Config()->page_size;
    u32 capacity = backlog_visible_slot_capacity();
    if (step > capacity) step = capacity;
    if (g_selected_from_newest <= step) g_selected_from_newest = 0u;
    else g_selected_from_newest -= step;
}

/* 把 InterlockedExchange 取得的次数限制在 64，防止异常挂起后一次执行极长循环。 */
static u32 backlog_take_request_count(volatile LONG* counter) {
    LONG value = InterlockedExchange(counter, 0);
    if (value <= 0) return 0u;
    if ((u32)value > BACKLOG_MAX_COMMANDS_PER_TICK) return BACKLOG_MAX_COMMANDS_PER_TICK;
    return (u32)value;
}

/*
 * 这是 vtable[0] 包装器，也是唯一会调用 RPG.exe 内部函数的插件入口。
 * 先捕获/处理 Backlog，再始终链式调用安装前的函数，因此不吞掉宽屏、手柄或其它插件的更新链。
 */
static void BACKLOG_THISCALL Backlog_HookSceneUpdate(void* scene_world) {
    u32 requests;
    u32 index;
    int selection_changed = 0;

    if (!g_active) backlog_capture_current_page();

    /*
     * 关闭仍然使用一帧插件内部屏障，避免关闭键在同一时刻穿透给刚恢复的游戏。
     *
     * 剧情旁路绝不修改真实 dialogue_id；自由探索 synthetic 模式才会清理自己创建的
     * synthetic ID。由于本帧仍然 g_active，SceneWorld update 也不会运行；下一次来到这里
     * 才完成对应模式的关闭，然后恢复正常 SceneWorld update。
     */
    if (g_active && g_close_barrier_pending) {
        backlog_close_on_game_thread();
    } else if (g_active && backlog_take_request_count(&g_request_close) != 0u) {
        backlog_begin_close_barrier();
    } else if (!g_active && backlog_take_request_count(&g_request_open) != 0u) {
        /*
         * 当前页可能刚建立；打开前再捕获一次，保证第一条对话也能立即进入历史。
         * 剧情过渡态会由 backlog_can_open_now() 拒绝，不会强行切入。
         */
        backlog_capture_current_page();
        backlog_open_on_game_thread();
    }

    if (g_active && !g_close_barrier_pending) {
        requests = backlog_take_request_count(&g_request_up);
        for (index = 0u; index < requests; ++index) backlog_move_up_once();
        if (requests != 0u) selection_changed = 1;

        requests = backlog_take_request_count(&g_request_down);
        for (index = 0u; index < requests; ++index) backlog_move_down_once();
        if (requests != 0u) selection_changed = 1;

        requests = backlog_take_request_count(&g_request_left);
        for (index = 0u; index < requests; ++index) backlog_page_left_once();
        if (requests != 0u) selection_changed = 1;

        requests = backlog_take_request_count(&g_request_right);
        for (index = 0u; index < requests; ++index) backlog_page_right_once();
        if (requests != 0u) selection_changed = 1;

        /*
         * 剧情旁路的绘制 Hook 直接从历史数组读取当前选择，不需要也不允许改真实剧情缓冲。
         * 自由探索 synthetic 模式才需要同步载体正文长度/缓冲，确保 0x404800 保持可绘制。
         */
        if (selection_changed && !g_opened_over_live_dialogue) {
            backlog_apply_selected_entry();
        }
    } else if (!g_active) {
        /* 非活动状态不保留任何导航/关闭旧请求，防止下次打开立即跳页。 */
        InterlockedExchange(&g_request_close, 0);
        InterlockedExchange(&g_request_up, 0);
        InterlockedExchange(&g_request_down, 0);
        InterlockedExchange(&g_request_left, 0);
        InterlockedExchange(&g_request_right, 0);
    }

    /*
     * 不能用“return 不调 0x40B150”来假装只冻结逻辑。
     * 原版 0x40B150 的 0x40B16B～0x40B173 同时负责：
     *
     *   push 0xBB8
     *   push scene_world
     *   call 0x434500
     *
     * 也就是把 scene_world 登记进本帧绘制队列。旧版把整段截断后，0x40B050/0x404800
     * 没有任何调用入口，所以剧情中 Backlog 必然空白。
     *
     * 现行活动态只复刻这一笔已经证明的绘制登记，然后 return：
     * - Event VM、地图实体、遇敌和其它 SceneWorld 逻辑都不推进；
     * - 本帧仍会调用 vtable[1]=0x40B050；
     * - 0x404800 内的原版 F-Talk/F-Name/姓名/正文组合绘制 Hook 因而都有入口。
     *
     * 关闭函数若已把 g_active 清 0，本 tick 会自然回到完整原更新链。
     */
    if (g_active) {
        PFN_DrawQueueRegister register_draw = (PFN_DrawQueueRegister)FN_DRAW_QUEUE_REGISTER;
        if (Runtime_MemoryRangeReadable(scene_world, 4u)) {
            register_draw(scene_world, SCENE_WORLD_DRAW_QUEUE_KEY);
        }
        return;
    }

    {
        PFN_SceneWorldUpdate previous = backlog_scene_next();
        if (previous) previous(scene_world);
    }
}

/* 检查一个函数地址所在内存是否有执行权限，避免链到普通数据或空指针。 */
static int backlog_pointer_is_executable(const void* pointer) {
    MEMORY_BASIC_INFORMATION information;
    DWORD protection;
    if (!Runtime_PointerLooksReadable(pointer)) return 0;
    if (VirtualQuery(pointer, &information, sizeof(information)) != sizeof(information)) return 0;
    if (information.State != MEM_COMMIT || (information.Protect & PAGE_GUARD) != 0u) return 0;
    protection = information.Protect & 0xFFu;
    return protection == PAGE_EXECUTE || protection == PAGE_EXECUTE_READ ||
           protection == PAGE_EXECUTE_READWRITE || protection == PAGE_EXECUTE_WRITECOPY;
}

/* 从 x86 E8 rel32 CALL 计算当前实际目标；不是 E8 时返回 0。 */
static void* backlog_call_target(u32 call_address) {
    i32 relative;
    if (*(volatile u8*)call_address != 0xE8u) return NULL;
    relative = *(volatile i32*)(call_address + 1u);
    return (void*)(call_address + 5u + (u32)relative);
}

/*
 * 只改 E8 后面的四字节相对位移，CALL opcode 自身保持不变。
 * out_previous 保存安装时的当前目标；若别的兼容插件已经包过同一 CALL，现代列表会继续链到它。
 */
static int backlog_patch_call(u32 call_address, void* hook, void** out_previous) {
    void* previous = backlog_call_target(call_address);
    i32 relative;
    DWORD old_protection;
    DWORD ignored;

    if (!hook || !out_previous || !backlog_pointer_is_executable(previous)) return 0;
    *out_previous = previous;
    relative = (i32)((u32)(SIZE_T)hook - (call_address + 5u));

    if (!VirtualProtect((void*)call_address, 5u, PAGE_EXECUTE_READWRITE, &old_protection)) return 0;
    *(volatile i32*)(call_address + 1u) = relative;
    VirtualProtect((void*)call_address, 5u, old_protection, &ignored);
    FlushInstructionCache(GetCurrentProcess(), (const void*)call_address, 5u);
    return 1;
}

/* 只有 CALL 仍指向本插件时才恢复，避免卸载时覆盖后来安装的其它链。 */
static void backlog_restore_call(u32 call_address, void* hook, void* previous) {
    i32 relative;
    DWORD old_protection;
    DWORD ignored;

    if (!previous || backlog_call_target(call_address) != hook) return;
    relative = (i32)((u32)(SIZE_T)previous - (call_address + 5u));
    if (!VirtualProtect((void*)call_address, 5u, PAGE_EXECUTE_READWRITE, &old_protection)) return;
    *(volatile i32*)(call_address + 1u) = relative;
    VirtualProtect((void*)call_address, 5u, old_protection, &ignored);
    FlushInstructionCache(GetCurrentProcess(), (const void*)call_address, 5u);
}

/*
 * 安装只改 vtable[0] 的一个 32 位指针。
 * 不改 0x403E30、0x404800、0x40B050 的机器码，也不改手柄插件使用的任何 CALL 点。
 */
int Backlog_Install(void) {
    void** slot = (void**)VTABLE_SCENE_WORLD;
    void* previous;
    DWORD old_protection;
    DWORD ignored;

    if (!Runtime_Config()->enabled) return 1;
    if (g_hook_installed) return 1;
    g_scene_next_slot = NULL;
    g_speaker_portrait_next_slot = NULL;
    g_panel_next_slot = NULL;
    g_name_panel_next_slot = NULL;
    g_name_text_next_slot = NULL;
    g_text_next_slot = NULL;

    /*
     * 安装顺序按原版 0x404800 的绘制顺序排列。
     * 任意一步失败都会按相反顺序撤销已经装好的 CALL，避免留下“半套 Hook”。
     *
     * 每一次 backlog_patch_call 都先保存当前 CALL 的目标。因此如果某个兼容插件比 Backlog
     * 更早加载，它的 wrapper 会成为 previous；Backlog 未活动时继续透明链回它。
     */
    if (!backlog_patch_call(CALL_DIALOGUE_SPEAKER_PORTRAIT_DRAW,
                            (void*)Backlog_HookCurrentSpeakerPortraitDraw,
                            (void**)&g_previous_speaker_portrait_draw)) {
        Runtime_Log("[致命] 无法安装当前剧情人物图屏蔽 CALL；现代 Backlog 未启用。");
        return 0;
    }
    g_speaker_portrait_hook_installed = 1;

    if (!backlog_patch_call(CALL_DIALOGUE_PANEL_DRAW, (void*)Backlog_HookPanelDraw,
                            (void**)&g_previous_panel_draw)) {
        Runtime_Log("[致命] 无法安装 F-Talk 多框绘制 CALL；现代 Backlog 未启用。");
        goto fail_calls;
    }
    g_panel_hook_installed = 1;

    if (!backlog_patch_call(CALL_DIALOGUE_NAME_PANEL_DRAW,
                            (void*)Backlog_HookCurrentNamePanelDraw,
                            (void**)&g_previous_name_panel_draw)) {
        Runtime_Log("[致命] 无法安装当前剧情 F-Name 屏蔽 CALL；现代 Backlog 未启用。");
        goto fail_calls;
    }
    g_name_panel_hook_installed = 1;

    if (!backlog_patch_call(CALL_DIALOGUE_NAME_TEXT_DRAW,
                            (void*)Backlog_HookCurrentNameTextDraw,
                            (void**)&g_previous_name_text_draw)) {
        Runtime_Log("[致命] 无法安装当前剧情姓名文字屏蔽 CALL；现代 Backlog 未启用。");
        goto fail_calls;
    }
    g_name_text_hook_installed = 1;

    if (!backlog_patch_call(CALL_DIALOGUE_TEXT_DRAW, (void*)Backlog_HookTextDraw,
                            (void**)&g_previous_text_draw)) {
        Runtime_Log("[致命] 无法安装原字体多条正文 CALL；现代 Backlog 未启用。");
        goto fail_calls;
    }
    g_text_hook_installed = 1;

    previous = *slot;
    if (previous == (void*)Backlog_HookSceneUpdate) {
        Runtime_Log("[Hook] 场景更新包装器已经存在，不重复安装。");
        g_hook_installed = 1;
        return 1;
    }
    if (!backlog_pointer_is_executable(previous)) {
        Runtime_Log("[致命] 场景 vtable[0] 当前值不是可执行函数，拒绝覆盖。");
        goto fail_calls;
    }

    g_previous_scene_update = (PFN_SceneWorldUpdate)previous;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &old_protection)) {
        Runtime_Log("[致命] 无法把场景 vtable[0] 临时设为可写。");
        goto fail_calls;
    }
    InterlockedExchangePointer((PVOID volatile*)slot, (PVOID)Backlog_HookSceneUpdate);
    VirtualProtect(slot, sizeof(void*), old_protection, &ignored);
    FlushInstructionCache(GetCurrentProcess(), slot, sizeof(void*));

    g_accept_input = 1;
    g_hook_installed = 1;
    Runtime_Log(previous == (void*)FN_SCENE_WORLD_UPDATE
        ? "[Hook] 已安装场景更新链；前一目标是原版 0x40B150。"
        : "[Hook] 已安装场景更新链；前一目标来自其它兼容插件，将按 thiscall 链式调用。");
    Runtime_Log(
        "[Hook] 历史绘制链已安装：人物图/F-Name/姓名文字可独立屏蔽，"
        "F-Talk/正文展开四条；剧情旁路不再修改 speaker 全局。"
    );
    return 1;

fail_calls:
    if (g_text_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_TEXT_DRAW, (void*)Backlog_HookTextDraw,
                             (void*)g_previous_text_draw);
        g_text_hook_installed = 0;
    }
    if (g_name_text_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_NAME_TEXT_DRAW,
                             (void*)Backlog_HookCurrentNameTextDraw,
                             (void*)g_previous_name_text_draw);
        g_name_text_hook_installed = 0;
    }
    if (g_name_panel_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_NAME_PANEL_DRAW,
                             (void*)Backlog_HookCurrentNamePanelDraw,
                             (void*)g_previous_name_panel_draw);
        g_name_panel_hook_installed = 0;
    }
    if (g_panel_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_PANEL_DRAW, (void*)Backlog_HookPanelDraw,
                             (void*)g_previous_panel_draw);
        g_panel_hook_installed = 0;
    }
    if (g_speaker_portrait_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_SPEAKER_PORTRAIT_DRAW,
                             (void*)Backlog_HookCurrentSpeakerPortraitDraw,
                             (void*)g_previous_speaker_portrait_draw);
        g_speaker_portrait_hook_installed = 0;
    }
    return 0;
}

static CastleStringView backlog_sdk_view(const char* text, CastleU32 length) {
    CastleStringView view;
    view.data = text;
    view.length = length;
    return view;
}

static const CastleHookApiV1* backlog_query_hook_api(
    const CastleRuntimeApiV1* runtime_api) {
    static const char interface_id[] = CASTLE_HOOK_INTERFACE_ID;
    CastleInterfaceQueryV1 query = {0};
    CastleInterfaceResultV1 result = {0};
    query.magic = CASTLE_QUERY_MAGIC;
    query.struct_size = CASTLE_SIZEOF_INTERFACE_QUERY_V1;
    query.request_version = CASTLE_QUERY_VERSION_1;
    query.interface_id = backlog_sdk_view(interface_id,
        (CastleU32)(sizeof(interface_id) - 1u));
    query.requested_version = CASTLE_HOOK_API_VERSION_1;
    query.minimum_struct_size = CASTLE_SIZEOF_HOOK_API_V1;
    result.magic = CASTLE_INTERFACE_API_MAGIC;
    result.struct_size = CASTLE_SIZEOF_INTERFACE_RESULT_V1;
    result.result_version = CASTLE_QUERY_VERSION_1;
    if (!runtime_api || runtime_api->QueryInterface(&query, &result) != CASTLE_OK) {
        return NULL;
    }
    return (const CastleHookApiV1*)result.api_pointer;
}

static CastleResult backlog_add_runtime_hook(const CastleHookApiV1* hook_api,
    CastleTransactionHandle transaction, CastleModule game_module,
    CastleU32 target_rva, CastleU32 hook_kind, CastleAddress original_target,
    CastleAddress replacement, CastleStringView signature,
    CastleStringView label, CastleClaimHandle* out_claim) {
    CastleChainHookClaimV1 claim = {0};
    claim.magic = CASTLE_CHAIN_HOOK_MAGIC;
    claim.struct_size = CASTLE_SIZEOF_CHAIN_HOOK_V1;
    claim.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    claim.hook_kind = hook_kind;
    claim.target.module = game_module;
    claim.target.rva = target_rva;
    claim.target.size = hook_kind == CASTLE_HOOK_REL32_CALL ? 5u : 4u;
    claim.expected_original_target = original_target;
    claim.replacement_hook = replacement;
    claim.signature_id = signature;
    claim.phase = CASTLE_HOOK_PHASE_NORMAL;
    claim.priority = CASTLE_HOOK_PRIORITY_DEFAULT;
    claim.label = label;
    return hook_kind == CASTLE_HOOK_REL32_CALL ?
        hook_api->AddRelativeCallHook(transaction, &claim, out_claim) :
        hook_api->AddPointerHook(transaction, &claim, out_claim);
}

static CastleResult backlog_get_next_slot(const CastleHookApiV1* hook_api,
    CastleClaimHandle claim, void* volatile** out_slot) {
    CastleHookBindingV1 binding = {0};
    binding.magic = CASTLE_HOOK_BINDING_MAGIC;
    binding.struct_size = CASTLE_SIZEOF_HOOK_BINDING_V1;
    binding.version = CASTLE_HOOK_STRUCTURE_VERSION_1;
    if (hook_api->GetHookBinding(claim, &binding) != CASTLE_OK ||
        !binding.next_slot) return CASTLE_ERROR_NOT_READY;
    *out_slot = binding.next_slot;
    return CASTLE_OK;
}

/* Runtime 模式把五个 CALL 和 SceneWorld vtable[0] 作为一个原子链事务提交。 */
CastleResult Backlog_InstallIntegrated(const CastleRuntimeApiV1* runtime_api,
                                       CastlePluginHandle plugin_handle) {
    static const char transaction_label[] = "Backlog render/update chains";
    static const char panel_signature_text[] =
        "org.castlereforge.signature.dialogue-panel-draw.v1";
    static const char text_signature_text[] =
        "org.castlereforge.signature.dialogue-text-draw.v1";
    static const char scene_signature_text[] =
        "org.castlereforge.signature.scene-world-update.v1";
    static const char speaker_label[] = "current speaker portrait draw";
    static const char panel_label[] = "F-Talk panel draw";
    static const char name_panel_label[] = "F-Name panel draw";
    static const char name_text_label[] = "speaker name text draw";
    static const char text_label[] = "dialogue text draw";
    static const char scene_label[] = "SceneWorld update";
    CastleRuntimeInfoV1 info = {0};
    CastleTransactionHandle transaction = 0u;
    CastleClaimHandle claims[6] = {0};
    const CastleHookApiV1* hook_api = backlog_query_hook_api(runtime_api);
    CastleStringView panel_signature = backlog_sdk_view(panel_signature_text,
        (CastleU32)(sizeof(panel_signature_text) - 1u));
    CastleStringView text_signature = backlog_sdk_view(text_signature_text,
        (CastleU32)(sizeof(text_signature_text) - 1u));
    CastleStringView scene_signature = backlog_sdk_view(scene_signature_text,
        (CastleU32)(sizeof(scene_signature_text) - 1u));
    CastleResult result;

    if (!Runtime_Config()->enabled) return CASTLE_OK;
    if (!hook_api || !runtime_api) return CASTLE_ERROR_INTERFACE_NOT_FOUND;
    info.magic = CASTLE_RUNTIME_INFO_MAGIC;
    info.struct_size = CASTLE_SIZEOF_RUNTIME_INFO_V1;
    info.info_version = CASTLE_RUNTIME_INFO_VERSION_1;
    if (runtime_api->GetRuntimeInfo(&info) != CASTLE_OK) return CASTLE_ERROR_RUNTIME_FAULT;
    result = hook_api->BeginTransaction(plugin_handle,
        backlog_sdk_view(transaction_label,
            (CastleU32)(sizeof(transaction_label) - 1u)), 0u, &transaction);
    if (result < 0) return result;

#define BACKLOG_ADD_CALL_(index_value, address_value, original_value, hook_value, signature_value, label_value) \
    do { \
        result = backlog_add_runtime_hook(hook_api, transaction, info.game_module, \
            (CastleU32)((address_value) - 0x00400000u), CASTLE_HOOK_REL32_CALL, \
            (CastleAddress)(original_value), (CastleAddress)(SIZE_T)(hook_value), \
            (signature_value), backlog_sdk_view((label_value), \
                (CastleU32)(sizeof(label_value) - 1u)), &claims[(index_value)]); \
        if (result < 0) goto fail_runtime_transaction; \
    } while (0)

    BACKLOG_ADD_CALL_(0u, CALL_DIALOGUE_SPEAKER_PORTRAIT_DRAW,
        FN_DIALOGUE_PANEL_DRAW, Backlog_HookCurrentSpeakerPortraitDraw,
        panel_signature, speaker_label);
    BACKLOG_ADD_CALL_(1u, CALL_DIALOGUE_PANEL_DRAW,
        FN_DIALOGUE_PANEL_DRAW, Backlog_HookPanelDraw,
        panel_signature, panel_label);
    BACKLOG_ADD_CALL_(2u, CALL_DIALOGUE_NAME_PANEL_DRAW,
        FN_DIALOGUE_PANEL_DRAW, Backlog_HookCurrentNamePanelDraw,
        panel_signature, name_panel_label);
    BACKLOG_ADD_CALL_(3u, CALL_DIALOGUE_NAME_TEXT_DRAW,
        FN_DIALOGUE_TEXT_DRAW, Backlog_HookCurrentNameTextDraw,
        text_signature, name_text_label);
    BACKLOG_ADD_CALL_(4u, CALL_DIALOGUE_TEXT_DRAW,
        FN_DIALOGUE_TEXT_DRAW, Backlog_HookTextDraw,
        text_signature, text_label);
    result = backlog_add_runtime_hook(hook_api, transaction, info.game_module,
        (CastleU32)(VTABLE_SCENE_WORLD - 0x00400000u),
        CASTLE_HOOK_VTABLE_POINTER, (CastleAddress)FN_SCENE_WORLD_UPDATE,
        (CastleAddress)(SIZE_T)Backlog_HookSceneUpdate, scene_signature,
        backlog_sdk_view(scene_label, (CastleU32)(sizeof(scene_label) - 1u)),
        &claims[5]);
    if (result < 0) goto fail_runtime_transaction;
    result = hook_api->PreflightTransaction(transaction);
    if (result < 0) goto fail_runtime_transaction;
    result = hook_api->CommitTransaction(transaction);
    if (result < 0) return result;

    if (backlog_get_next_slot(hook_api, claims[0], &g_speaker_portrait_next_slot) < 0 ||
        backlog_get_next_slot(hook_api, claims[1], &g_panel_next_slot) < 0 ||
        backlog_get_next_slot(hook_api, claims[2], &g_name_panel_next_slot) < 0 ||
        backlog_get_next_slot(hook_api, claims[3], &g_name_text_next_slot) < 0 ||
        backlog_get_next_slot(hook_api, claims[4], &g_text_next_slot) < 0 ||
        backlog_get_next_slot(hook_api, claims[5], &g_scene_next_slot) < 0) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    g_speaker_portrait_hook_installed = 1;
    g_panel_hook_installed = 1;
    g_name_panel_hook_installed = 1;
    g_name_text_hook_installed = 1;
    g_text_hook_installed = 1;
    g_hook_installed = 1;
    g_accept_input = 1;
    Runtime_Log("[Hook] Runtime 已原子提交五个绘制 CALL 和 SceneWorld vtable 链。");
#undef BACKLOG_ADD_CALL_
    return CASTLE_OK;

fail_runtime_transaction:
#undef BACKLOG_ADD_CALL_
    hook_api->AbortTransaction(transaction);
    Runtime_Log("[致命] Runtime 拒绝 Backlog Hook 事务；没有留下半套链。");
    return result;
}

/* 返回按键这一刻是否按下；Win32 高位 0x8000 表示当前物理按住。 */
static int backlog_keyboard_down(int virtual_key) {
    return (GetAsyncKeyState(virtual_key) & (SHORT)0x8000) != 0;
}

/*
 * 更新一个按键的边沿/自动重复状态。
 * allow_repeat=0 时只在 0->1 的第一 tick 返回 1；方向键传 1 后支持长按滚动。
 */
static int backlog_repeat_key(RepeatKeyState* state, int down, int allow_repeat) {
    const RuntimeConfig* config = Runtime_Config();
    int fire = 0;

    if (!state) return 0;
    if (!down) {
        state->was_down = 0;
        state->held_ticks = 0u;
        return 0;
    }

    if (!state->was_down) {
        state->was_down = 1;
        state->held_ticks = 1u;
        return 1;
    }

    if (state->held_ticks < 0xFFFFFFFFu) ++state->held_ticks;
    if (allow_repeat && state->held_ticks >= config->repeat_delay_ticks) {
        u32 elapsed = state->held_ticks - config->repeat_delay_ticks;
        if ((elapsed % config->repeat_interval_ticks) == 0u) fire = 1;
    }
    return fire;
}

/* 活动状态刚变化时，把方向重复计时清零，避免“打开前一直按着方向”带入第一次滚动。 */
static void backlog_reset_navigation_repeat(void) {
    g_key_up.was_down = 0;
    g_key_up.held_ticks = 0u;
    g_key_down.was_down = 0;
    g_key_down.held_ticks = 0u;
    g_key_left.was_down = 0;
    g_key_left.held_ticks = 0u;
    g_key_right.was_down = 0;
    g_key_right.held_ticks = 0u;
}

/*
 * 检查 PadSupport 与 Backlog 有关的按钮是否已经全部松开。
 *
 * 这个函数只读 PadSupport 当前状态，不改变它的 consumed mask，也不产生动作。
 * 它专门服务于模式切换后的 release barrier。
 */
static int backlog_pad_all_relevant_buttons_released(void) {
    if (!PadBridge_Available()) return 1;
    if (PadBridge_Down(PAD_BRIDGE_LB)) return 0;
    if (PadBridge_Down(PAD_BRIDGE_CANCEL)) return 0;
    if (PadBridge_Down(PAD_BRIDGE_DPAD_UP)) return 0;
    if (PadBridge_Down(PAD_BRIDGE_DPAD_DOWN)) return 0;
    if (PadBridge_Down(PAD_BRIDGE_DPAD_LEFT)) return 0;
    if (PadBridge_Down(PAD_BRIDGE_DPAD_RIGHT)) return 0;
    return 1;
}

void Backlog_PollInput(void) {
    const RuntimeConfig* config = Runtime_Config();
    int active;
    int pad_allowed;
    int open_down;
    int exit_down;
    int up_down;
    int down_down;
    int left_down;
    int right_down;
    int open_pressed;
    int exit_pressed;
    int up_pressed;
    int down_pressed;
    int left_pressed;
    int right_pressed;
    i32 wheel_steps;
    u32 right_mouse_pressed;

    if (!g_accept_input || !config->enabled) return;

    /*
     * 鼠标消息由 RPG.exe 的 WndProc 在游戏 UI 线程里记录，这里只一次性取走计数。
     * 即使这一 tick 游戏不在前台，也先取走，避免旧滚轮在切回游戏后突然触发。
     */
    wheel_steps = MouseInput_TakeWheelSteps();
    right_mouse_pressed = MouseInput_TakeRightPressed();

    /*
     * 手柄是完全可选的协作能力：
     * - 没有 Castle_PadSupport.asi -> pad_allowed=0；
     * - PadSupport 正在调查/RT鼠标/Back鼠标模式 -> pad_allowed=0；
     * - 只有普通 Controller 模式才允许 Backlog 读取 PadSupport 的按钮状态。
     *
     * 另外还有一层 release barrier：
     * PadSupport 刚加载，或者刚从调查/鼠标模式回到普通手柄模式时，必须先把 LB、取消键、
     * 十字键全部松开一次。这样“模式里一直按着的键”不会在模式结束的第一帧误触发 Backlog。
     */
    if (!PadBridge_Available()) {
        g_pad_was_available = 0;
        g_pad_release_barrier = 1;
        pad_allowed = 0;
    } else if (PadBridge_BlocksBacklogInput()) {
        g_pad_was_available = 1;
        g_pad_release_barrier = 1;
        pad_allowed = 0;
    } else {
        if (!g_pad_was_available) {
            g_pad_was_available = 1;
            g_pad_release_barrier = 1;
        }

        if (g_pad_release_barrier) {
            if (backlog_pad_all_relevant_buttons_released()) {
                g_pad_release_barrier = 0;
                pad_allowed = 1;
            } else {
                pad_allowed = 0;
            }
        } else {
            pad_allowed = 1;
        }
    }

    open_down = backlog_keyboard_down(config->keyboard_open) ||
                (pad_allowed && PadBridge_Down(PAD_BRIDGE_LB));

    /*
     * 手柄退出有两个入口：
     * 1. LB：和打开键相同；
     * 2. CANCEL：物理 South/East 由 PadSupport 自己的 SwapConfirmCancel 决定。
     */
    exit_down = backlog_keyboard_down(config->keyboard_exit) ||
                (pad_allowed &&
                 (PadBridge_Down(PAD_BRIDGE_LB) || PadBridge_Down(PAD_BRIDGE_CANCEL)));

    up_down = backlog_keyboard_down(config->keyboard_up) ||
              (pad_allowed && PadBridge_Down(PAD_BRIDGE_DPAD_UP));
    down_down = backlog_keyboard_down(config->keyboard_down) ||
                (pad_allowed && PadBridge_Down(PAD_BRIDGE_DPAD_DOWN));
    left_down = backlog_keyboard_down(config->keyboard_left) ||
                (pad_allowed && PadBridge_Down(PAD_BRIDGE_DPAD_LEFT));
    right_down = backlog_keyboard_down(config->keyboard_right) ||
                 (pad_allowed && PadBridge_Down(PAD_BRIDGE_DPAD_RIGHT));

    open_pressed = backlog_repeat_key(&g_key_open, open_down, 0);
    exit_pressed = backlog_repeat_key(&g_key_exit, exit_down, 0);
    up_pressed = backlog_repeat_key(&g_key_up, up_down, 1);
    down_pressed = backlog_repeat_key(&g_key_down, down_down, 1);
    left_pressed = backlog_repeat_key(&g_key_left, left_down, 1);
    right_pressed = backlog_repeat_key(&g_key_right, right_down, 1);

    active = g_active != 0;
    if (active != g_worker_saw_active) {
        g_worker_saw_active = active;
        backlog_reset_navigation_repeat();
    }

    /* 后台时仍更新物理边沿状态，但不提交动作；切回窗口不会把一直按着的键当成新按下。 */
    if (!Runtime_GameIsForeground()) return;

    if (!active) {
        /*
         * 键盘 B、PadSupport 的 LB、鼠标滚轮向上都可以打开。
         * 滚轮向下在未打开时没有意义，因此直接忽略。
         */
        if ((open_pressed || wheel_steps > 0) &&
            g_history_count != 0u && backlog_can_open_now()) {
            InterlockedIncrement(&g_request_open);
        }
        return;
    }

    /*
     * 关闭优先级最高：键盘 Exit、PadSupport 的 LB/CANCEL、鼠标右键任一命中都关闭。
     * 关闭这一 tick 不再同时滚动，避免视觉上先跳一条再消失。
     */
    if (exit_pressed || right_mouse_pressed != 0u) {
        InterlockedIncrement(&g_request_close);
        return;
    }

    /*
     * 滚轮向上 = 与 Up 相同，查看更旧记录；滚轮向下 = 与 Down 相同，查看更新记录。
     * 一次消息只算一步；如果系统同一 tick 累积了多条消息，就按累计次数滚动。
     */
    if (wheel_steps > 0) {
        i32 i;
        for (i = 0; i < wheel_steps && i < (i32)BACKLOG_MAX_COMMANDS_PER_TICK; ++i) {
            InterlockedIncrement(&g_request_up);
        }
    } else if (wheel_steps < 0) {
        i32 count = -wheel_steps;
        i32 i;
        for (i = 0; i < count && i < (i32)BACKLOG_MAX_COMMANDS_PER_TICK; ++i) {
            InterlockedIncrement(&g_request_down);
        }
    }

    if (up_pressed) InterlockedIncrement(&g_request_up);
    if (down_pressed) InterlockedIncrement(&g_request_down);
    if (left_pressed) InterlockedIncrement(&g_request_left);
    if (right_pressed) InterlockedIncrement(&g_request_right);
}

int Backlog_IsActive(void) {
    return g_active != 0;
}

/*
 * 正常 ASI 会与 RPG.exe 同寿命，不会在游戏中途卸载。
 * 这里仍恢复 vtable（仅当槽位仍是我们自己），并在极端卸载时把最关键的标量/指针放回。
 * DllMain 卸载路径不调用任何 RPG.exe 内部资源函数。
 * 现行版没有私有 F-Name 池；主动卸载只需恢复 Hook 和 synthetic 标量。
 */
void Backlog_Shutdown(void) {
    void** slot = (void**)VTABLE_SCENE_WORLD;
    DWORD old_protection;
    DWORD ignored;

    g_accept_input = 0;

    /*
     * 正常 ASI 与 RPG.exe 同寿命，真正关闭 Backlog 应该始终走游戏线程的
     * backlog_close_on_game_thread()。这里是极端主动卸载兜底，不能调用 RPG.exe 的 SF2 析构。
     *
     * 如果此刻是剧情旁路，v0.3.3-test3 起从打开就没有写过剧情全局，因此这里也绝不写；
     * 如果是 synthetic 模式，才恢复插件自己曾修改的那组标量/指针。
     */
    if (g_active) {
        if (!g_opened_over_live_dialogue) {
            *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE = 0u;
            *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE = 0u;
            *(u8* volatile*)GLOBAL_DIALOGUE_DISPLAY_BUFFER = g_saved.display_buffer;
            *(volatile u32*)GLOBAL_DIALOGUE_MODE = g_saved.dialogue_mode;
            *(volatile u32*)GLOBAL_DIALOGUE_TOTAL_BYTES = g_saved.total_bytes;
            *(volatile u32*)GLOBAL_DIALOGUE_VISIBLE_BYTES = g_saved.visible_bytes;
            *(volatile u32*)GLOBAL_DIALOGUE_ID = g_saved.dialogue_id;
            *(volatile u8*)GLOBAL_EVENT_YIELD_FLAG = g_saved.event_yield;
            *(volatile u8*)GLOBAL_EVENT_BLOCK_FLAG = g_saved.event_block;
            *(volatile u8*)GLOBAL_MAP_INPUT_GATE = g_saved.map_input_gate;
            *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_ACTIVE = g_saved.speaker;
            *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_STYLE = g_saved.speaker_style;
            *(volatile u8*)GLOBAL_DIALOGUE_SPEAKER_VARIANT = g_saved.speaker_variant;
            *(u8* volatile*)GLOBAL_DIALOGUE_SPEAKER_NAME = g_saved.speaker_name;
            *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE = g_saved.target_state;
            *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE = g_saved.current_state;
        }
        g_close_barrier_pending = 0;
        g_opened_over_live_dialogue = 0;
        g_active = 0;
    }

    if (g_hook_installed && *slot == (void*)Backlog_HookSceneUpdate && g_previous_scene_update) {
        if (VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &old_protection)) {
            InterlockedExchangePointer((PVOID volatile*)slot, (PVOID)g_previous_scene_update);
            VirtualProtect(slot, sizeof(void*), old_protection, &ignored);
            FlushInstructionCache(GetCurrentProcess(), slot, sizeof(void*));
        }
    }

    /* CALL 按安装的相反顺序恢复；只有 CALL 仍然指向本插件时才会写回。 */
    if (g_text_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_TEXT_DRAW, (void*)Backlog_HookTextDraw,
                             (void*)g_previous_text_draw);
    }
    if (g_name_text_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_NAME_TEXT_DRAW,
                             (void*)Backlog_HookCurrentNameTextDraw,
                             (void*)g_previous_name_text_draw);
    }
    if (g_name_panel_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_NAME_PANEL_DRAW,
                             (void*)Backlog_HookCurrentNamePanelDraw,
                             (void*)g_previous_name_panel_draw);
    }
    if (g_panel_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_PANEL_DRAW, (void*)Backlog_HookPanelDraw,
                             (void*)g_previous_panel_draw);
    }
    if (g_speaker_portrait_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_SPEAKER_PORTRAIT_DRAW,
                             (void*)Backlog_HookCurrentSpeakerPortraitDraw,
                             (void*)g_previous_speaker_portrait_draw);
    }

    g_hook_installed = 0;
    g_text_hook_installed = 0;
    g_name_text_hook_installed = 0;
    g_name_panel_hook_installed = 0;
    g_panel_hook_installed = 0;
    g_speaker_portrait_hook_installed = 0;

    g_previous_text_draw = NULL;
    g_previous_name_text_draw = NULL;
    g_previous_name_panel_draw = NULL;
    g_previous_panel_draw = NULL;
    g_previous_speaker_portrait_draw = NULL;
}
