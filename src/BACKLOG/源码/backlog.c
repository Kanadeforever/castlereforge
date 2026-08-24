#include "backlog.h"
#include "runtime.h"
#include "game_addresses.h"
#include "sdl_input.h"

/*
 * backlog.c
 *
 * 这是插件的核心文件。整体数据流如下：
 *
 * 原版场景更新（游戏线程）
 *   -> 读取当前消息页并复制到插件环形历史
 *   -> 消费 worker 提交的打开/移动/关闭请求
 *   -> 若正在查看，切换通用 F-Talk 多框与原字体姓名/正文列表
 *   -> 再链式调用原版或前一个插件的场景更新函数
 *
 * worker（每 8ms）
 *   -> 只采样键盘和可选 SDL
 *   -> 只增加动作计数，不解引用消息正文，不调用 RPG.exe 内部函数
 *
 * 这样安排有两个重要好处：
 * 1. 文本捕获和原版绘制 Hook 全部发生在 RPG.exe 原本的游戏线程；
 * 2. SDL/键盘线程不会一边复制文本，另一边正好卸载地图资源，避免跨线程悬空指针。
 */

#define BACKLOG_STATIC_CAPACITY 256u
#define BACKLOG_MAX_COMMANDS_PER_TICK 64u
#define BACKLOG_VISIBLE_ENTRIES 4u
#define BACKLOG_PANEL_STRIDE_Y 110
#define BACKLOG_NAME_COLUMN_WIDTH 96
#define BACKLOG_SPEAKER_NAME_BYTES 64u

typedef void (BACKLOG_THISCALL *PFN_SceneWorldUpdate)(void* scene_world);
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
 * 现代浏览只画姓名文字，不再用这些字段重建人物/姓名框资源。
 */
typedef struct BacklogEntry {
    u16 length;
    u8 speaker;
    u8 speaker_style;
    u8 speaker_variant;
    u8 reserved;
    /*
     * 姓名按原版 00 02 结束协议复制成独立小缓冲。
     * 现代列表只把姓名画成左侧文字列，不加载/显示人物 %d-2.SF2。
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

static BacklogEntry g_history[BACKLOG_STATIC_CAPACITY];
static u8 g_overlay_buffer[DIALOGUE_BUFFER_BYTES];
static SavedGameState g_saved;
static CaptureKey g_last_capture;

static volatile u32 g_history_count;
static u32 g_history_head;
static u32 g_selected_from_newest;
static volatile LONG g_active;
static volatile LONG g_accept_input;

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

static PFN_SceneWorldUpdate g_previous_scene_update;
static PFN_DialoguePanelDraw g_previous_panel_draw;
static PFN_DialogueTextDraw g_previous_text_draw;
static int g_hook_installed;
static int g_panel_hook_installed;
static int g_text_hook_installed;
static int g_oversized_page_logged;
/*
 * 关闭分成两次场景更新：第一次只把消息 ID 清零，给手柄插件至少一个 worker 周期
 * 清掉普通对话 A pending；下一次才恢复真实 ID 和剧情状态。
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
    return Runtime_PointerLooksReadable(pointer);
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
        if (Runtime_PointerLooksReadable(movie) && *(volatile u8*)(movie + MOVIE_ACTIVE_FLAG_OFFSET) != 0u) {
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
    if (*(volatile u32*)GLOBAL_DIALOGUE_ID != 0u) return 1;
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
    if (!Runtime_PointerLooksReadable((const void*)source)) return;

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
    if (!Runtime_PointerLooksReadable((const void*)source_address) ||
        !Runtime_PointerLooksReadable((const void*)(source_address + key.total_bytes - 1u))) return;

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
static u32 backlog_visible_entry_count(void) {
    u32 count = g_history_count;
    u32 available;
    if (count == 0u || g_selected_from_newest >= count) return 0u;
    available = count - g_selected_from_newest;
    return available < BACKLOG_VISIBLE_ENTRIES ? available : BACKLOG_VISIBLE_ENTRIES;
}

/*
 * order=0 表示当前画面最上方的记录；最后一个 order 是最下方、也是当前滚动锚点。
 * 记录不足四条时向屏幕底部对齐，保留原游戏“消息从下方出现”的视觉方向。
 */
static BacklogEntry* backlog_visible_entry(u32 order, u32* out_panel_slot) {
    u32 visible = backlog_visible_entry_count();
    u32 offset;
    if (visible == 0u || order >= visible) return NULL;

    offset = g_selected_from_newest + (visible - 1u - order);
    if (out_panel_slot) *out_panel_slot = BACKLOG_VISIBLE_ENTRIES - visible + order;
    return backlog_entry_from_newest(offset);
}

/*
 * 0x40486E 原本只画一次底部 F-Talk.SF2。
 * 现代模式让同一个原版通用对话框对象在四个纵向位置各画一次；每次只临时改变 object+4 的世界 Y，
 * 调用后立即恢复。没有复制图片，也没有人物资源，四个框都来自游戏已经加载的 F-Talk 对象。
 */
static void BACKLOG_THISCALL Backlog_HookPanelDraw(void* panel,
                                                    i32 arg1, i32 arg2, i32 arg3,
                                                    i32 arg4, i32 arg5) {
    i32 original_x;
    i32 original_y;
    u32 visible;
    u32 order;

    if (!g_previous_panel_draw) return;
    if (!g_active || !Runtime_PointerLooksReadable(panel)) {
        g_previous_panel_draw(panel, arg1, arg2, arg3, arg4, arg5);
        return;
    }

    original_x = *(i32*)((u8*)panel + 0u);
    original_y = *(i32*)((u8*)panel + 4u);
    visible = backlog_visible_entry_count();

    for (order = 0u; order < visible; ++order) {
        u32 panel_slot = 0u;
        i32 shift_y;
        if (!backlog_visible_entry(order, &panel_slot)) continue;
        shift_y = ((i32)panel_slot - ((i32)BACKLOG_VISIBLE_ENTRIES - 1)) * BACKLOG_PANEL_STRIDE_Y;
        *(i32*)((u8*)panel + 0u) = original_x;
        *(i32*)((u8*)panel + 4u) = original_y + shift_y;
        g_previous_panel_draw(panel, arg1, arg2, arg3, arg4, arg5);
    }

    *(i32*)((u8*)panel + 0u) = original_x;
    *(i32*)((u8*)panel + 4u) = original_y;
}

/* 姓名缓冲只含 00 02 时表示本条没有可显示姓名。 */
static int backlog_entry_has_speaker_name(const BacklogEntry* entry) {
    if (!entry || entry->speaker == 0u) return 0;
    return !(entry->speaker_name[0] == 0u && entry->speaker_name[1] == 0x02u);
}

/*
 * 0x4049FF 原本只把当前页正文画在底框。
 * 包装器复用它已经准备好的字体对象、目标表面、裁剪原点和颜色参数，只改变 x/y/text：
 * - 有姓名：左侧 96 像素列画 NameList 原文字节，正文从右侧开始；
 * - 无姓名：正文保持原版 x；
 * - 每条 y 与对应 F-Talk 框使用相同的 110 像素步长。
 */
static void BACKLOG_THISCALL Backlog_HookTextDraw(void* font,
                                                   i32 origin_x, i32 origin_y,
                                                   void* surface, i32 x, i32 y,
                                                   i32 draw_mode, const u8* text,
                                                   i32 draw_flags) {
    u32 visible;
    u32 order;

    if (!g_previous_text_draw) return;
    if (!g_active) {
        g_previous_text_draw(font, origin_x, origin_y, surface, x, y,
                             draw_mode, text, draw_flags);
        return;
    }

    visible = backlog_visible_entry_count();
    for (order = 0u; order < visible; ++order) {
        BacklogEntry* entry;
        u32 panel_slot = 0u;
        i32 shift_y;
        i32 body_x = x;

        entry = backlog_visible_entry(order, &panel_slot);
        if (!entry) continue;
        shift_y = ((i32)panel_slot - ((i32)BACKLOG_VISIBLE_ENTRIES - 1)) * BACKLOG_PANEL_STRIDE_Y;

        if (backlog_entry_has_speaker_name(entry)) {
            g_previous_text_draw(font, origin_x, origin_y, surface, x, y + shift_y,
                                 draw_mode, entry->speaker_name, draw_flags);
            body_x += BACKLOG_NAME_COLUMN_WIDTH;
        }
        g_previous_text_draw(font, origin_x, origin_y, surface, body_x, y + shift_y,
                             draw_mode, entry->text, draw_flags);
    }
}

/*
 * 把选中历史复制进插件自己的 512 字节缓冲，再原子地让原版全局指针指向它。
 * 原消息自己的缓冲从不被覆盖，所以关闭时只要恢复指针就能得到打开前的逐字内容。
 */
static void backlog_apply_selected_entry(void) {
    BacklogEntry* entry = backlog_entry_from_newest(g_selected_from_newest);

    if (!entry) return;
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
    g_selected_from_newest = 0u;
    g_active = 1;

    *(volatile u8*)GLOBAL_MAP_INPUT_GATE = 0u;
    *(volatile u8*)GLOBAL_EVENT_YIELD_FLAG = 1u;
    *(volatile u8*)GLOBAL_EVENT_BLOCK_FLAG = 1u;
    if (g_saved.dialogue_id == 0u) {
        *(volatile u32*)GLOBAL_DIALOGUE_ID = BACKLOG_SYNTHETIC_DIALOGUE_ID;
    }

    backlog_apply_selected_entry();
    Runtime_Log("[Backlog] 已打开；当前显示最新一条记录，原剧情和地图输入已进入只读暂停。");
}

/*
 * 退出的第一阶段只清消息 ID，仍保持只读对话框、事件门和地图输入门。
 *
 * 手柄插件的普通对话模块把 A 保存成一次 pending，直到原版 0x403E30 的正确阶段消费。
 * 如果玩家在 Backlog 内误按 A 后立刻退出，直接恢复真实消息会让这张 pending 纸条推进下一句。
 * ID=0 会让手柄插件在它下一次 8ms 更新时主动清 pending；等下一次场景逻辑步再正式恢复，
 * 中间通常有几十毫秒，足够完成这道跨插件释放屏障。
 */
static void backlog_begin_close_barrier(void) {
    if (!g_active || g_close_barrier_pending) return;
    *(volatile u32*)GLOBAL_DIALOGUE_ID = 0u;
    g_close_barrier_pending = 1;
    Runtime_Log("[Backlog] 退出请求已进入一帧释放屏障；正在清理其它输入插件可能保留的对话确认。");
}

/*
 * 恢复时先把 current/target 清零，阻止任何中间态被绘制；然后恢复缓冲、说话人资源和全部标量。
 * 人物显示标量和姓名指针只做直接恢复，因为人物资源对象从始至终都没有被卸载或替换。
 */
static void backlog_close_on_game_thread(void) {
    if (!g_active) return;

    *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE = 0u;
    *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE = 0u;
    *(u8* volatile*)GLOBAL_DIALOGUE_DISPLAY_BUFFER = g_saved.display_buffer;

    /* 人物资源从未卸载/重建；直接恢复原标量和姓名指针即可，不再触碰 SAF/SF2 FrameID。 */
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

    /* 原版视觉状态最后恢复；到此为止缓冲、资源、消息 ID 和事件门已经全部就绪。 */
    *(volatile u8*)GLOBAL_DIALOGUE_TARGET_STATE = g_saved.target_state;
    *(volatile u8*)GLOBAL_DIALOGUE_CURRENT_STATE = g_saved.current_state;
    g_close_barrier_pending = 0;
    g_active = 0;
    Runtime_Log("[Backlog] 已关闭；打开前的对话进度、说话人资源和地图/事件输入门已恢复。");
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
    if (count <= 1u) return;
    if (g_selected_from_newest + step >= count) g_selected_from_newest = count - 1u;
    else g_selected_from_newest += step;
}

/* →：向新记录跳一页；不足整页时停在最新记录。 */
static void backlog_page_right_once(void) {
    u32 step = Runtime_Config()->page_size;
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

    /* pending 在上一个场景逻辑步已经清过 ID；现在才执行完整恢复。 */
    if (g_active && g_close_barrier_pending) {
        backlog_close_on_game_thread();
    } else if (g_active && backlog_take_request_count(&g_request_close) != 0u) {
        backlog_begin_close_barrier();
    } else if (!g_active && backlog_take_request_count(&g_request_open) != 0u) {
        /* 当前页可能刚建立；打开前再捕获一次，保证第一条对话也能立即进入历史。 */
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

        if (selection_changed) backlog_apply_selected_entry();
    } else {
        /* 非活动状态不保留任何导航/关闭旧请求，防止下次打开立即跳页。 */
        InterlockedExchange(&g_request_close, 0);
        InterlockedExchange(&g_request_up, 0);
        InterlockedExchange(&g_request_down, 0);
        InterlockedExchange(&g_request_left, 0);
        InterlockedExchange(&g_request_right, 0);
    }

    if (g_previous_scene_update) g_previous_scene_update(scene_world);
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

    if (!backlog_patch_call(CALL_DIALOGUE_PANEL_DRAW, (void*)Backlog_HookPanelDraw,
                            (void**)&g_previous_panel_draw)) {
        Runtime_Log("[致命] 无法安装 F-Talk 多框绘制 CALL；现代 Backlog 未启用。");
        return 0;
    }
    g_panel_hook_installed = 1;

    if (!backlog_patch_call(CALL_DIALOGUE_TEXT_DRAW, (void*)Backlog_HookTextDraw,
                            (void**)&g_previous_text_draw)) {
        Runtime_Log("[致命] 无法安装原字体多条正文 CALL；现代 Backlog 未启用。");
        backlog_restore_call(CALL_DIALOGUE_PANEL_DRAW, (void*)Backlog_HookPanelDraw,
                             (void*)g_previous_panel_draw);
        g_panel_hook_installed = 0;
        return 0;
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
        backlog_restore_call(CALL_DIALOGUE_TEXT_DRAW, (void*)Backlog_HookTextDraw,
                             (void*)g_previous_text_draw);
        backlog_restore_call(CALL_DIALOGUE_PANEL_DRAW, (void*)Backlog_HookPanelDraw,
                             (void*)g_previous_panel_draw);
        g_text_hook_installed = 0;
        g_panel_hook_installed = 0;
        return 0;
    }

    g_previous_scene_update = (PFN_SceneWorldUpdate)previous;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &old_protection)) {
        Runtime_Log("[致命] 无法把场景 vtable[0] 临时设为可写。");
        backlog_restore_call(CALL_DIALOGUE_TEXT_DRAW, (void*)Backlog_HookTextDraw,
                             (void*)g_previous_text_draw);
        backlog_restore_call(CALL_DIALOGUE_PANEL_DRAW, (void*)Backlog_HookPanelDraw,
                             (void*)g_previous_panel_draw);
        g_text_hook_installed = 0;
        g_panel_hook_installed = 0;
        return 0;
    }
    InterlockedExchangePointer((PVOID volatile*)slot, (PVOID)Backlog_HookSceneUpdate);
    VirtualProtect(slot, sizeof(void*), old_protection, &ignored);
    FlushInstructionCache(GetCurrentProcess(), slot, sizeof(void*));

    g_accept_input = 1;
    g_hook_installed = 1;
    Runtime_Log(previous == (void*)FN_SCENE_WORLD_UPDATE
        ? "[Hook] 已安装场景更新链；前一目标是原版 0x40B150。"
        : "[Hook] 已安装场景更新链；前一目标来自其它兼容插件，将按 thiscall 链式调用。");
    Runtime_Log("[Hook] 现代整屏展示已安装：同屏四个原版 F-Talk 框 + 原字体姓名/正文，不加载人物图。");
    return 1;
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

void Backlog_PollInput(void) {
    const RuntimeConfig* config = Runtime_Config();
    int active;
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

    if (!g_accept_input || !config->enabled) return;

    open_down = backlog_keyboard_down(config->keyboard_open) || SdlInput_Down(config->gamepad_open);
    exit_down = backlog_keyboard_down(config->keyboard_exit) || SdlInput_Down(config->gamepad_exit);
    up_down = backlog_keyboard_down(config->keyboard_up) || SdlInput_Down(config->gamepad_up);
    down_down = backlog_keyboard_down(config->keyboard_down) || SdlInput_Down(config->gamepad_down);
    left_down = backlog_keyboard_down(config->keyboard_left) || SdlInput_Down(config->gamepad_left);
    right_down = backlog_keyboard_down(config->keyboard_right) || SdlInput_Down(config->gamepad_right);

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
        if (open_pressed && g_history_count != 0u && backlog_can_open_now()) {
            InterlockedIncrement(&g_request_open);
        }
        return;
    }

    if (exit_pressed) {
        InterlockedIncrement(&g_request_close);
        return;
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
 * DllMain 卸载路径不调用任何 RPG.exe 内部资源函数；现代模式本来也不重建人物资源。
 */
void Backlog_Shutdown(void) {
    void** slot = (void**)VTABLE_SCENE_WORLD;
    DWORD old_protection;
    DWORD ignored;

    g_accept_input = 0;
    if (g_active) {
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
        g_close_barrier_pending = 0;
        g_active = 0;
    }

    if (g_hook_installed && *slot == (void*)Backlog_HookSceneUpdate && g_previous_scene_update) {
        if (VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &old_protection)) {
            InterlockedExchangePointer((PVOID volatile*)slot, (PVOID)g_previous_scene_update);
            VirtualProtect(slot, sizeof(void*), old_protection, &ignored);
        }
    }
    if (g_text_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_TEXT_DRAW, (void*)Backlog_HookTextDraw,
                             (void*)g_previous_text_draw);
    }
    if (g_panel_hook_installed) {
        backlog_restore_call(CALL_DIALOGUE_PANEL_DRAW, (void*)Backlog_HookPanelDraw,
                             (void*)g_previous_panel_draw);
    }
    g_hook_installed = 0;
    g_text_hook_installed = 0;
    g_panel_hook_installed = 0;
}
