#include "dialogue_input.h"
#include "platform.h"
#include "runtime.h"
#include "game_addresses.h"
#include "input_router.h"
#include "confirm_dialog.h"

/*
 * 这里保存 RPG.exe 原本使用的 USER32!GetAsyncKeyState。
 *
 * refactor11 继续沿用 refactor10 已经实机证明“能真正命中公共消息引擎”的局部桥方案：
 * 我们不修改全游戏 IAT，只改 0x4041D7 与 0x40447E 两处公共消息代码自己的
 * “mov esi,[GetAsyncKeyState]”。这样标题、战斗、地图和其它系统仍完全调用原版 API。
 */
static PFN_GetAsyncKeyState g_original_get_async_key_state;

/*
 * 一颗 A 的“待消费纸条”。
 *
 * worker 线程只负责在 A 刚按下的那一帧把它置 1；真正应该把这颗 A 解释成什么，
 * 由游戏线程当前处于“逐字显示阶段”还是“整句等待阶段”来决定。
 *
 * refactor10 的问题正出在这里：两种阶段共用一个包装函数，而且包装函数一看到 VK_LBUTTON
 * 就立刻把纸条拿走。因此前面的逐字显示函数总能先吃掉 A，即使本句已经完整显示，
 * 后面的“下一句推进”函数也永远拿不到这一颗 A。
 */
static volatile int g_confirm_pending;
static int g_dialogue_enabled;

/*
 * 0x404440 这一条“整句已经显示完、等待用户继续”的原版路径，首先查询 VK_CONTROL(0x11)。
 * 如果 Ctrl 没按，而且窗口过程维护的鼠标释放标记也没置位，原版会在查询左键之前直接返回。
 *
 * 真正鼠标点击会经历 Windows 的按下/抬起消息，所以原版那个释放标记会自然变化；
 * 手柄 A 没有 Windows 鼠标消息，因此 refactor11 只在这一个局部包装函数里临时把 Ctrl 的
 * “当前按下”高位补出来，让原版继续走到紧接着的 VK_LBUTTON 查询。
 *
 * 这不会把 Ctrl 真的发给游戏，也不会影响其它系统：只有 0x40447E 这一个消息等待函数
 * 使用这个包装函数。
 */
#define DIALOGUE_VK_CONTROL 0x11

/*
 * 公共消息引擎把当前消息编号保存在 0x46F670；结束流程会把它清零。
 * 这里只读原版状态，不写消息 ID，也不猜地图、剧情编号或 UI 名称。
 */
int DialogueInput_IsActive(void) {
    if (!g_dialogue_enabled) return 0;
    return *(volatile u32*)GLOBAL_DIALOGUE_ID != 0u;
}

/*
 * 判断当前这一句是否还在“一个字一个字往外显示”。
 *
 * RPG.exe 自己在 0x404203～0x40420A 做的就是同一个比较：
 *   ECX = [0x46F688]  当前已经走到的位置
 *   EAX = [0x46F684]  本句总长度
 *   cmp ecx,eax
 *
 * 两者不相等时，原版左键的意义不是换下一句，而是立即把剩余文字补全。
 */
static int dialogue_text_is_still_revealing(void) {
    u32 visible_bytes = *(volatile u32*)GLOBAL_DIALOGUE_VISIBLE_BYTES;
    u32 total_bytes = *(volatile u32*)GLOBAL_DIALOGUE_TOTAL_BYTES;

    return visible_bytes != total_bytes;
}

/*
 * 第一阶段包装函数：只服务 0x4041D7 那条“逐字显示”输入读取。
 *
 * 规则非常简单：
 * - 真实鼠标输入永远原样保留；
 * - 没有 A pending 时完全透传；
 * - 本句还没显示完时，A = 原版左键一次，消费 pending，让本句立即完整显示；
 * - 本句已经显示完时，不消费 pending，也不伪造左键，把同一颗 A 留给第二阶段去推进下一句。
 *
 * 最后一条就是 refactor11 修复的核心。
 */
static SHORT WINAPI DialogueInput_TypewriterGetAsyncKeyState(int virtual_key) {
    SHORT real_value;

    if (!g_original_get_async_key_state) return 0;

    real_value = g_original_get_async_key_state(virtual_key);

    /* 这条原版路径真正关心的是左键。其它键查询全部原样透传。 */
    if (virtual_key != (int)VK_LBUTTON_) return real_value;

    if (!g_confirm_pending) return real_value;

    /* 消息已经结束时，旧 pending 不能带到下一句或其它 UI。 */
    if (!DialogueInput_IsActive()) {
        g_confirm_pending = 0;
        return real_value;
    }

    /* Yes/No 盖在文字上时，A 必须先交给询问框。 */
    if (ConfirmDialog_IsActive()) return real_value;

    /*
     * 本句已经完整显示：这里故意什么都不做，也绝对不能清 pending。
     * 随后 0x404440 的等待路径会拿到同一颗 A，再执行真正的“下一句”。
     */
    if (!dialogue_text_is_still_revealing()) return real_value;

    /*
     * 本句仍在逐字显示：这时 A 的正确原版语义就是“马上显示完整”。
     * 一颗 A 只做这一件事，所以在这里消费 pending。
     */
    g_confirm_pending = 0;
    Runtime_Log("[对话] A 已由逐字显示阶段消费：按原版左键语义立即补全当前句。");
    return (SHORT)(((u16)real_value) | 0x8000u);
}

/*
 * 第二阶段包装函数：只服务 0x40447E 那条“整句显示完以后等待继续”的输入读取。
 *
 * 这段原版代码的顺序是：
 * 1. 先查询 VK_CONTROL；
 * 2. 如果 Ctrl 与鼠标释放标记都没有，就直接返回；
 * 3. 通过门槛后才查询 VK_LBUTTON；
 * 4. 条件成立时，若本句已经完整，就调用 0x4046F0 切到下一段消息。
 *
 * 手柄 A 没有 WM_LBUTTONUP，所以不能指望原版鼠标释放标记自动帮助我们通过第 2 步。
 * 因此：
 * - pending 存在时，对 VK_CONTROL 只临时补高位，不消费 pending；
 * - 随后的 VK_LBUTTON 查询才真正消费 pending，并补一次左键高位；
 * - 原版自己的比较、0x4046F0 调用和消息生命周期全部保留。
 */
static SHORT WINAPI DialogueInput_AdvanceGetAsyncKeyState(int virtual_key) {
    SHORT real_value;

    if (!g_original_get_async_key_state) return 0;

    real_value = g_original_get_async_key_state(virtual_key);

    if (!g_confirm_pending) return real_value;

    if (!DialogueInput_IsActive()) {
        g_confirm_pending = 0;
        return real_value;
    }

    if (ConfirmDialog_IsActive()) return real_value;

    /*
     * 本句如果又回到了逐字显示状态，就不能在等待路径里越级推进。
     * 这颗 A 应留给第一阶段包装函数去做“补全文字”。
     */
    if (dialogue_text_is_still_revealing()) return real_value;

    if (virtual_key == DIALOGUE_VK_CONTROL) {
        /*
         * 只为这一条局部原版路径打开“继续检查左键”的门。
         * pending 仍然保留，真正动作要等紧接着的 VK_LBUTTON 查询。
         */
        return (SHORT)(((u16)real_value) | 0x8000u);
    }

    if (virtual_key != (int)VK_LBUTTON_) return real_value;

    /*
     * 终于来到整句等待阶段真正的左键读取。
     * 在这里消费 pending，原版随后会自己判断当前位置并进入 0x4046F0 推进流程。
     */
    g_confirm_pending = 0;
    Runtime_Log("[对话] A 已由整句等待阶段消费：按原版左键路径推进到下一句。");
    return (SHORT)(((u16)real_value) | 0x8000u);
}

int DialogueInput_InstallHook(void) {
    static const u8 expected_load[] = {0x8B,0x35,0xA4,0x01,0x46,0x00};

    if (!Runtime_DialogueProtocolOk()) {
        Runtime_Log("[对话] 公共消息引擎协议未通过；A 推进对话已单独禁用。");
        g_dialogue_enabled = 0;
        return 1;
    }

    /*
     * 这里只读取原 IAT 指针，不改 IAT 本身。
     * 两个消息阶段会分别装载不同的包装函数，因此 refactor11 能明确区分：
     * “这一颗 A 是补全文字，还是推进下一句”。
     */
    g_original_get_async_key_state = *(PFN_GetAsyncKeyState*)IAT_GETASYNCKEYSTATE;
    if (!g_original_get_async_key_state) {
        Runtime_Log("[对话] 原版 GetAsyncKeyState 指针无效；A 推进对话已单独禁用。");
        g_dialogue_enabled = 0;
        return 1;
    }

    /*
     * 两个地址原本都是 6 字节：8B 35 A4 01 46 00，即 mov esi,[0x4601A4]。
     * Runtime_PatchMovEsiFunction 会严格核对原机器码，再改成 mov esi,imm32 + nop。
     *
     * 关键区别：
     * - 0x4041D7 -> DialogueInput_TypewriterGetAsyncKeyState
     * - 0x40447E -> DialogueInput_AdvanceGetAsyncKeyState
     *
     * 后面的 call esi 不改，栈布局和 RPG.exe 原控制流也不改。
     */
    if (!Runtime_PatchMovEsiFunction(PATCH_DIALOGUE_GETASYNC_A,
                                     (void*)DialogueInput_TypewriterGetAsyncKeyState,
                                     expected_load) ||
        !Runtime_PatchMovEsiFunction(PATCH_DIALOGUE_GETASYNC_B,
                                     (void*)DialogueInput_AdvanceGetAsyncKeyState,
                                     expected_load)) {
        Runtime_Log("[对话] 两阶段局部输入桥安装失败；A 推进对话已单独禁用。");
        g_dialogue_enabled = 0;
        return 1;
    }

    g_confirm_pending = 0;
    g_dialogue_enabled = 1;
    Runtime_Log("[对话] 已启用两阶段 A：逐字显示时补全本句，完整显示后再次按 A 推进下一句。");
    return 1;
}

void DialogueInput_Update(void) {
    if (!g_dialogue_enabled) return;

    if (!DialogueInput_IsActive()) {
        g_confirm_pending = 0;
        return;
    }

    /* 询问框覆盖消息文字时，A 必须先给询问框，不能把底下的文字同时推进。 */
    if (ConfirmDialog_IsActive()) {
        g_confirm_pending = 0;
        return;
    }

    if (InputRouter_PressedOn(INPUT_CTX_DIALOGUE, INPUT_CONFIRM, INPUT_LAYER_OVERLAY)) {
        if (!g_confirm_pending) {
            g_confirm_pending = 1;
            Runtime_Log("[对话] A：已排队，将由当前原版文字阶段决定‘补全本句’或‘推进下一句’。");
        }
        InputRouter_Consume(INPUT_CONFIRM);
    }
}
