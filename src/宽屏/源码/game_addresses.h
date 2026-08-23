#ifndef CASTLE_WIDESCREEN_GAME_ADDRESSES_H
#define CASTLE_WIDESCREEN_GAME_ADDRESSES_H

/*
 * game_addresses.h
 *
 * 这个文件集中保存“我们已经从目标 RPG.exe 静态确认过的绝对地址”。
 * 这样做的原因很简单：如果把 0x44A9C6、0x978514 之类的数字散落在很多 C 文件里，
 * 将来换 EXE 或继续逆向时，很容易只改到一半，最后得到一个能编译但会写错内存的补丁。
 *
 * 下面全部地址只适用于这一份目标程序：
 * SHA-256 = b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
 *
 * 本版启动时仍会核对关键函数机器码和 CALL 原目标。
 * 只要其中一个锚点不符合预期，插件就不安装宽屏 Hook。
 */

/* RPG.exe 自己已经导入的 Kernel32 API。ASI 启动极早期可以直接从这些 IAT 槽拿函数地址。 */
#define IAT_GETMODULEHANDLEA       0x004600C8u
#define IAT_GETMODULEFILENAMEA     0x0046006Cu
#define IAT_WRITEFILE              0x00460074u
#define IAT_CLOSEHANDLE            0x00460078u
#define IAT_VIRTUALALLOC           0x00460080u
#define IAT_GETPROCADDRESS         0x00460090u
#define IAT_GETCURRENTPROCESS      0x00460120u
#define IAT_CREATEFILEA            0x0046015Cu
#define IAT_BINK_COPYTOBUFFER      0x0046024Cu

/*
 * 显示对象以及场景状态。
 *
 * 历史说明：
 * v0.5-v0.8 曾经利用 0x409982 -> 0x40B230 区分“玩家主动消息”和“非玩家消息”。
 * v0.9 已根据实机视觉结论完全取消这套来源分类：任何原版消息 UI 都统一进入电影式侧区。
 *
 * 仍然必须保留的消息相关修复是“侧画消息隔离”：
 * 0x40B050 world manager 内部会继续调用 0x403E30 / 0x404800，所以左右 Camera 重放期间
 * 临时把 0x46F678/679 置 0，完成后立即恢复。中央原版绘制不改这两个状态。
 * 这层隔离已经在 v0.7 实机证明可以阻止第二套立绘、对话框和选择 UI。
 */
#define GLOBAL_DISPLAY_OBJECT      0x0089F6C0u
#define GLOBAL_ACTIVE_EVENT_ID     0x0089F808u
#define GLOBAL_CURRENT_EVENT_COMMAND 0x0089F830u  /* 固化49历史地址；v0.11 当前策略仍不读取它，仅保留作后续研究对照。 */
#define GLOBAL_INTERFACE_UI        0x008DED0Cu
#define GLOBAL_TITLE_UI            0x008E241Cu
#define GLOBAL_MOVIE_OBJECT        0x0046F390u

/*
 * 原版消息系统的当前 Event 槽。SHOW_MESSAGE 通过 0x403B90 写入当前 Event ID 的低 8 位，
 * 对话真正结束时由 0x403F28 / 0x4047CA 清零。v0.11 只把“该槽是否非零”当成消息 UI 生命周期：
 * 非零就统一进入电影式侧区，不再拿它和任何“玩家主动来源”做对照。
 */
#define GLOBAL_MESSAGE_EVENT_ID      0x0046F670u
#define GLOBAL_MESSAGE_TARGET_STATE   0x0046F678u
#define GLOBAL_MESSAGE_CURRENT_STATE  0x0046F679u

/*
 * 这两个不是 Hook 点，只是运行时预检使用的机器码锚点：
 * - 0x403BC2 附近把 Event ID 截成低8位后写入 0x46F670；
 * - 0x403F22 附近在对话完成路径把 0x46F670 清零。
 * v0.6 依赖这条生命周期语义，所以换 EXE 时必须先重新确认，不能只相信绝对地址碰巧还存在。
 */
#define ADDR_MESSAGE_EVENT_WRITE     0x00403BC2u
#define ADDR_MESSAGE_EVENT_CLEAR     0x00403F22u

/*
 * v0.7 起侧画消息隔离依赖的两个原版消息函数入口：
 * - 0x403E30 首先检查 0x46F678/679；两个都为 0 时立即返回，不推进消息状态；
 * - 0x404800 首先检查 0x46F679；为 0 时直接跳到函数尾，不绘制消息 UI。
 * 因而侧画时临时把这两个状态字节置 0，可以只屏蔽“本次侧画重复的消息 UI”，不会修改中央原版那一次。
 */
#define FN_MESSAGE_UPDATE_DRAW        0x00403E30u
#define FN_MESSAGE_RENDER             0x00404800u

/* Display 对象内部字段。它们来自 0x405960 / 0x405A10 / 0x406330 的静态闭合。 */
#define DISPLAY_MODE_OBJECT        0x28u
#define DISPLAY_BACKING_HEIGHT     0x5Cu
#define DISPLAY_BACKING_WIDTH      0x60u
#define DISPLAY_BYTES_PER_PIXEL    0x64u
#define DISPLAY_BACKING_PIXELS     0x68u
#define DISPLAY_EXTRA_X            0x6Cu
#define DISPLAY_EXTRA_Y            0x70u

/* DisplayMode 字段。原版在目标安装上是 640×480、16 bit。 */
#define MODE_WIDTH                 0x04u
#define MODE_HEIGHT                0x08u

/*
 * Camera 全局量。
 * 0x44B300 会依次写入：minX、minY、maxX、maxY、viewportW、viewportH。
 * 0x44B360 再根据跟随对象更新 0x978514/0x978518，并把相机夹在这些边界内。
 *
 * v0.3-poc3 仍不永久修改这些值。
 * 左右补画时只临时改 CAMERA_X，调用完原版绘制队列立刻恢复。
 */
#define GLOBAL_CAMERA_X            0x00978514u
#define GLOBAL_CAMERA_Y            0x00978518u
#define GLOBAL_CAMERA_VIEW_W       0x0097851Cu
#define GLOBAL_CAMERA_VIEW_H       0x00978520u
#define GLOBAL_CAMERA_MIN_X        0x00978524u
#define GLOBAL_CAMERA_MIN_Y        0x00978528u
#define GLOBAL_CAMERA_MAX_X        0x0097852Cu
#define GLOBAL_CAMERA_MAX_Y        0x00978530u

/*
 * 0x434710 使用的“排序后绘制队列”。
 * count 在 0x8DDA38；从 0x8DDA40 开始每项 8 字节：第一个 DWORD 是排序键，第二个 DWORD 是对象指针。
 * 原版相关初始化/清理代码给这块区域预留 200 项，所以本版绝不保存超过 200 项。
 */
#define GLOBAL_DRAW_QUEUE_COUNT    0x008DDA38u
#define GLOBAL_DRAW_QUEUE_ENTRIES  0x008DDA40u
#define DRAW_QUEUE_ENTRY_BYTES     8u
#define DRAW_QUEUE_MAX_ENTRIES     200u


/*
 * EVE Command 的 vtable 地址。
 * 这些值来自固化49《事件程序指令词典》，不是凭画面猜测。
 * SHOW_MESSAGE(0x05/0x33) 故意不放进这里：单独说一句话不应该自动触发 4:3。
 *
 * 这里继续保存固化49已经确认的镜头/视觉 Command vtable，纯粹作为历史研究与未来对照资料。
 * v0.6 曾用其中部分 Camera/Scene 命令把“玩家主动 +0xEB Event”二次升级为剧情；
 * v0.7 已按实机规则删除这套二次分类；v0.9 更进一步取消来源区分，当前运行时不再读取这些 vtable 来决定是否模糊。
 */
#define VTABLE_CMD_ENABLE_SCENE_RENDER_OVERRIDE 0x00460664u
#define VTABLE_CMD_SET_CAMERA_TRACK_ENTITY       0x004605F8u
#define VTABLE_CMD_RESET_CAMERA_TRACK_PLAYER     0x004605ECu
#define VTABLE_CMD_PAN_CAMERA_PROXY_TO           0x004605D4u
#define VTABLE_CMD_VISUAL_TRANSITION_HOLD        0x004604B4u
#define VTABLE_CMD_VISUAL_TRANSITION_CUSTOM      0x004604A8u
#define VTABLE_CMD_RESET_VISUAL_TRANSITION       0x0046049Cu
#define VTABLE_CMD_CONFIGURE_CAMERA_JITTER       0x00460490u
#define VTABLE_CMD_SET_SCENE_RENDER_OVERRIDE     0x00460484u
#define VTABLE_CMD_VISUAL_TRANSITION_RAMP_A      0x00460478u
#define VTABLE_CMD_VISUAL_TRANSITION_RAMP_B      0x0046046Cu

/*
 * 0x40B150 会把场景总管理对象以 key=0xBB8 注册进 0x434710 队列；它的 vtable[1] 是 0x40B050。
 * 0x40B050 内部串行调用背景、实体、Camera 后处理等世界绘制链。
 * v0.3 的左右补画只重放这个“世界总管理 draw”，不再重放外层 SF2/UI 队列项。
 */
#define FN_SCENE_WORLD_DRAW         0x0040B050u

/*
 * 0x40B050 中的 `mov ecx,[esi+0x280]` 会把当前 Map/Scene runtime 交给世界背景链。
 * 因此 world manager 对象 +0x280 可以作为“场景身份”使用：
 * 同一剧情中对白暂时消失时这个指针不变；真正切换地图/场景时它会换成另一块 runtime。
 */
#define SCENE_WORLD_RUNTIME_OFFSET    0x280u
#define ADDR_SCENE_WORLD_RUNTIME_READ 0x0040B072u

/*
 * BattleManager 构造函数 0x441D41 把 vptr 写成 0x4610D4；
 * 0x434710 固定调用 vtable[1]，所以该对象在排序绘制队列中的实际 draw 方法就是 0x4429F0。
 * 0x43D870 又把 BattleManager 自身注册进 0x434500 队列，因而可以在不猜全局 flag 的情况下可靠识别战斗帧。
 */
#define VTABLE_BATTLE_MANAGER         0x004610D4u
#define FN_BATTLE_MANAGER_DRAW        0x004429F0u

/*
 * Event 激活入口。0x409982 的主动交互路径会调用这里。
 * v0.9 已取消玩家/非玩家视觉分类，v0.11 继续保持，因此当前运行时不再 Hook 或调用这个入口；
 * 地址只保留为 v0.5-v0.8 的历史研究证据，方便以后追溯来源分类为什么曾经存在。
 */
#define FN_EVENT_ACTIVATE           0x0040B230u

/* 已静态确认的原版函数入口。 */
#define FN_DISPLAY_ALLOCATE        0x00405960u
#define FN_DISPLAY_REBUILD         0x00405BD0u
#define FN_DISPLAY_PRESENT         0x00405A10u
#define FN_RENDER_QUEUE            0x00434710u
#define FN_CAMERA_SET_BOUNDS       0x0044B300u
#define FN_CAMERA_UPDATE_CLAMP     0x0044B360u

/*
 * v0.11 当前仍只改 4 个精确 CALL：
 * 1/2. DirectDraw 初建和 Surface lost 重建：让物理目标变成 INI 选择的854×480或1120×480；
 * 3. 每帧 0x434710：在原版 768×576 backing 上额外补画左右 Camera；
 * 4. 每帧 Present：把插件自己的动态“仅输出 staging”（16:9为982×576、21:9为1248×576）交给 0x405A10。
 *
 * 0x409982 主动交互 Event CALL 仅作为历史地址保留，v0.11 也不再修改。
 * POC1 的 backing 分配 CALL 和 11 个低层 Blit CALL 也继续全部不碰。
 */
#define CALL_DISPLAY_REBUILD_INIT  0x004059E1u
#define CALL_DISPLAY_REBUILD_LOST  0x00406307u
#define CALL_RENDER_QUEUE           0x0044A9C6u
#define CALL_DISPLAY_PRESENT       0x0044A9E6u
#define CALL_MANUAL_INTERACTION_EVENT 0x00409982u

#endif /* CASTLE_WIDESCREEN_GAME_ADDRESSES_H */
