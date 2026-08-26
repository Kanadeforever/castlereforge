#include "save_point.h"
#include "runtime.h"
#include "game_addresses.h"
#include "save_slot.h"

/*
 * save_point.c
 *
 * refactor30 的错误假设是：“存档点确认后会进入 Interface state7”。
 * 实机结果是整个存档页面手柄无效，这说明 InterfaceTome_Update()
 * 根本看不到该对象。重新从原版 Oracle 追踪 0x4245B0 SaveSlot ctor 的全部直接调用后，
 * 确认存档点使用的是第三条独立生命周期：
 *
 *   事件 opcode 0x3E / 0x40DAA0
 *       -> new 0x594
 *       -> 0x413B00 包装层
 *       -> [0x89FCD0] = wrapper
 *       -> wrapper+0x580 = 0x4245B0 构造的标准 SaveSlot
 *
 * 所以本文件只做“拆包”，绝不复制 save_slot.c 中任何槽位/分页/三项动作/确认/存读档代码。
 * refactor32 已把共享三项动作独立安装，并由总调度放行 action+0x5B4 的深层 ConfirmDialog；
 * 这些仍不是 wrapper Adapter 的业务职责。
 */

typedef struct SavePointState {
    int enabled;      /* 独立协议预检是否通过；失败时仅让原版键鼠继续。 */
    int was_active;   /* 是否已经为当前 wrapper 配对调用 SaveSlot_Begin。 */
    u8* wrapper;      /* 0x89FCD0 当前发布的 0x413B00 对象。 */
    u8* save_slot;    /* wrapper+0x580 当前绑定的原版 SaveSlot。 */
} SavePointState;

static SavePointState g_save_point;

/*
 * 每 tick 重新读全局槽，不持有已经被 RPG.exe 释放的旧指针。
 * 0x413F84 会先把 0x89FCD0 清零，然后析构/free wrapper，所以这个全局槽就是最稳定的生命周期门。
 */
static u8* save_point_wrapper(void) {
    u8* wrapper = *(u8**)GLOBAL_SAVE_POINT_UI;
    return Runtime_PtrOk(wrapper) ? wrapper : NULL;
}

/*
 * 只取已经由 0x413B00 绑定的标准 SaveSlot。
 * 这里不要把 wrapper 自己当成 SaveSlot：两者对象大小和字段偏移完全不同，
 * refactor30 “页面看起来一样”的错误正是在这一层。
 */
static u8* save_point_inner_slot(u8* wrapper) {
    u8* save;
    if (!Runtime_PtrOk(wrapper)) return NULL;
    save = *(u8**)(wrapper + SAVE_POINT_SLOT);
    return Runtime_PtrOk(save) ? save : NULL;
}

int SavePoint_Active(void) {
    u8* wrapper = save_point_wrapper();

    if (!wrapper) return 0;

    /*
     * owner 的存在本身就是模态门，不要求 child 已经完成绑定：
     * - ctor/析构边缘可能短暂只有 wrapper；这时也不能把同一颗按键送到底层地图/菜单；
     * - capability 预检失败时更不能按 +0x580 解引用未获验证的布局，但仍应只把本页留给原版键鼠。
     */
    return 1;
}

/* 对象消失或重建时精确结束自己的共享 SaveSlot 会话。 */
static void save_point_end_session(void) {
    if (g_save_point.was_active) {
        SaveSlot_End();
        Runtime_Log("[存档点] 独立包装层已离开；共享 SaveSlot 会话已结束。");
    }
    g_save_point.was_active = 0;
    g_save_point.wrapper = NULL;
    g_save_point.save_slot = NULL;
}

void SavePoint_Update(void) {
    u8* wrapper = save_point_wrapper();
    u8* save;
    i32 phase;

    /* 预检失败时不读取任何 wrapper 字段；只清理可能存在的旧共享会话。 */
    if (!g_save_point.enabled) {
        save_point_end_session();
        return;
    }

    save = save_point_inner_slot(wrapper);

    if (!wrapper || !save) {
        save_point_end_session();
        return;
    }

    /*
     * 新 wrapper、新 child，或包装层内部重建了 SaveSlot，都必须重新 Begin。
     * from_pad 只传递当前光标所有权：
     * - 用手柄在剧情“是/否”中确认存档点，Cursor 仍属于手柄，第一槽立即可见；
     * - 用真实鼠标进入，保持原版 hover，直到用户真正按手柄方向/A/B。
     */
    if (!g_save_point.was_active || g_save_point.wrapper != wrapper || g_save_point.save_slot != save) {
        if (g_save_point.was_active) SaveSlot_End();
        g_save_point.wrapper = wrapper;
        g_save_point.save_slot = save;
        g_save_point.was_active = 1;
        SaveSlot_Begin(save, Cursor_ControllerOwnsPointer() ? 1 : 0);
        Runtime_Log("[存档点] 已绑定 wrapper+0x580：槽位、共享三项动作与深层询问按各自唯一控制器路由。");
    }

    phase = *(i32*)(wrapper + SAVE_POINT_PHASE);

    /*
     * phase==1/2 时是 0x413E40 自己的展开/收起动画。
     * 这时保留会话，但不再接受新按键：否则一颗 A/B 可能在按钮还没完成 enable/disable 切换时落入。
     * 任何未知 phase 也安全让路，绝不把未证实状态当成稳定交互层。
     */
    if (phase != SAVE_POINT_PHASE_STABLE) return;
    if (*(u8*)(save + SAVE_ACTIVE) == 0) return;

    SaveSlot_Update(save);
}

void SavePoint_OnPointerTakeover(CursorTakeoverEvent event_type) {
    if (event_type == CURSOR_TAKEOVER_NONE || !g_save_point.was_active) return;
    /* 只清理 SaveSlot 的手柄强制焦点，不改 wrapper phase，也不关闭存档页。 */
    SaveSlot_OnPointerTakeover();
}

int SavePoint_Install(void) {
    g_save_point.enabled = 0;
    g_save_point.was_active = 0;
    g_save_point.wrapper = NULL;
    g_save_point.save_slot = NULL;

    if (!Runtime_SavePointProtocolOk()) {
        Runtime_Log("[存档点] 独立包装层协议不匹配；本 Adapter 已 fail-closed，原版键鼠和已验收页面不受影响。");
        return 1;
    }

    g_save_point.enabled = 1;
    Runtime_Log("[存档点] 独立 wrapper Adapter 已启用；仅解包 +0x580，完整子层仍由共享控制器负责。");
    return 1;
}
