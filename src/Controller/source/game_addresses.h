#ifndef CASTLE_PAD_GAME_ADDRESSES_H
#define CASTLE_PAD_GAME_ADDRESSES_H

/*
 * game_addresses.h
 *
 * 这个文件只保存“目标 RPG.exe 的地址、字段偏移和已经闭合的协议常量”。
 * 业务代码不再到处散落 0x004xxxxx 这样的魔法数字。
 *
 * 精确目标 RPG.exe SHA-256：
 * b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
 *
 * 注意：这些地址不是“猜测”。它们来自 dev3→dev20 的静态逆向和实机迭代。
 * 如果以后支持另一个 EXE，应新增另一套地址表，而不是直接修改这里让两个版本混在一起。
 */

/* RPG.exe IAT：插件从这里取得最早期的 Win32 API 地址。 */
#define IAT_GETMODULEHANDLEA   0x004600C8u
#define IAT_WRITEFILE          0x00460074u
#define IAT_CLOSEHANDLE        0x00460078u
#define IAT_GETPROCADDRESS     0x00460090u
#define IAT_LOADLIBRARYA       0x00460158u
#define IAT_CREATEFILEA        0x0046015Cu
#define IAT_SETCURSORPOS       0x0046019Cu
#define IAT_GETCURSORPOS       0x00460204u
#define IAT_GETASYNCKEYSTATE   0x004601A4u
#define IAT_GETKEYSTATE        0x004601A8u

/* 游戏中各大 UI/管理器的全局指针槽。这里存的是“指针所在地址”，不是对象本体。 */
#define GLOBAL_BATTLE_UI       0x0089FD74u
#define GLOBAL_RESULT_UI       0x0089FCC8u
#define GLOBAL_TARGET_SELECTOR 0x008E2410u
#define GLOBAL_MOUSE_MANAGER   0x008E1C4Cu
#define GLOBAL_TITLE_UI        0x008E241Cu
#define GLOBAL_BATTLE_CURSOR_Y_TABLE 0x0089FDECu
#define GLOBAL_BATTLE_CURSOR_X_TABLE 0x0089FDF0u

/*
 * 游戏内主 Interface。
 *
 * 入口研究累计纠错：
 * - refactor17：0x0089FCCC / 0x413850 那只“三按钮对象”属于休整/特殊地图动作菜单，
 *   第0项 0x413933 虽可进入 Interface，却不是普通探索通用入口，实机 FAIL；
 * - refactor18：尝试在 0x40CC73 -> 0x406F20 门控上叠加 Y，普通地图实机仍完全未命中，FAIL；
 * - refactor19：直接追原版 Space 键业务。基线 EXE 的 0x44A4D0 收到 VK_SPACE 后，经过 0x89F808 / 0x468BF0
 *   两个条件，最终 push 1 / call 0x40B230。
 * - refactor20a：用户遇到 0x40B230 整段硬签名误拒绝。现在只把 0x40B230 当“基线目标地址”记录；
 *   Runtime 会抓 Space/busy/mode/action=1 特征并从 CALL 现场解析当前真实目标，Y 跟随该目标，不发送 Space。
 *
 * 0x89F818 与 0x434920 仍是已确认 Interface 状态/构造证据，但不再被当作 Y 注入点。
 */
#define GLOBAL_INTERFACE_UI        0x008DED0Cu
#define GLOBAL_MAP_INTERFACE_STATE 0x0089F818u
#define GLOBAL_DATA_CENTER         0x008E1C48u

/*
 * 地图存档点/事件 opcode 0x3E 打开的“独立存档包装层”。
 *
 * refactor30 曾经根据画面相似，误以为存档点会进入 Interface state7。
 * 用户实机证明存档点页面完全收不到手柄后，重新对原版 RPG.exe 做交叉引用，闭合了真实对象链：
 *
 *   0x40DB2D -> 0x413B00                  构造 0x594 字节的独立包装对象；
 *   [0x89FCD0]                            保存该包装对象；
 *   0x413C3F -> 0x4245B0                  包装层内构造标准 SaveSlot；
 *   wrapper+0x580                         保存真正 SaveSlot 指针；
 *   wrapper+0x584                         0=稳定交互，1=展开动画，2=收起动画。
 *
 * 所以正确复用边界是“新增一个只解包 +0x580 的 owner Adapter，底层仍只有一份 save_slot.c”，
 * 而不是再写一套存档、分页或确认逻辑。
 */
#define GLOBAL_SAVE_POINT_UI             0x0089FCD0u
#define FN_SAVE_POINT_WRAPPER_CTOR       0x00413B00u
#define CALL_MAP_SAVE_POINT_WRAPPER_CTOR 0x0040DB2Du
#define CALL_SAVE_POINT_SLOT_CTOR        0x00413C3Fu
#define SIG_SAVE_POINT_WRAPPER_PUBLISH   0x0040DB32u
#define SIG_SAVE_POINT_WRAPPER_BIND      0x00413C48u
#define SIG_SAVE_POINT_WRAPPER_UPDATE    0x00413D89u

/*
 * 原版 Bink 电影包装对象。
 * RPG.exe 的窗口过程会先从这个全局槽取当前电影对象，再检查对象 +0x0A 是否为“正在播放”。
 * 我们只读这个状态，用来回答一个非常简单的问题：现在按 ESC 会不会进入原版的电影关闭分支？
 */
#define GLOBAL_MOVIE_OBJECT    0x0046F390u
#define MOVIE_ACTIVE_FLAG      0x0000000Au
#define SIG_MOVIE_ACTIVE_CHECK 0x004018ABu
#define SIG_MOVIE_ESC_CHECK    0x004018EFu

/* 已确认的 RPG.exe 内部函数入口。 */
#define FN_MOUSE_DRAW          0x0043E1B0u
#define FN_EXPLORE_CURSOR_DRAW 0x0040A6D0u
#define FN_MOUSE_SPRITE_DRAW   0x0043E940u
#define FN_BUTTON_HITTEST      0x00431310u
#define FN_TOP_PANEL_ANIM      0x0041C470u /* 战斗顶层展开/收起动画；原版每帧会把 marker Y 写回第1项。 */

/*
 * CMD1 / CMD2 各自拥有一套独立的列表展开/收起动画函数。
 * refactor4 静态闭合：这两个函数在“列表重新展开完成”时，会主动调用 0x43DF30，
 * 把 Windows 真实鼠标移到列表默认位置。鼠标设计下这是原版正常行为；手柄导航下却会造成两件事：
 *   1. 父列表返回时真实鼠标突然出现；
 *   2. 默认鼠标位置让原版视觉先短暂命中第 1 行，再追到手柄保存的真实逻辑行。
 * 因此 refactor4 只在“Battle 手柄导航确实拥有光标”时覆盖这两条列表动画协议。
 */
#define FN_CMD1_PANEL_ANIM     0x00418830u
#define FN_CMD2_PANEL_ANIM     0x0041A590u
#define FN_BUTTON_EVENT        0x00431380u
/*
 * Button+0x30 指向的 Sprite 反馈函数。
 * 0x4312C0 在 Button 初始化完成后会以参数 0 调它建立普通态；state5 按下时则以参数 1 调同一函数。
 * refactor26d 只把参数 0 作为 synthetic 点击结束后的原版 idle 恢复，不直接写 Sprite 动画字段。
 */
#define FN_BUTTON_SPRITE_FEEDBACK 0x0043E6F0u
#define FN_INTERFACE_OPEN_PREP 0x004347B0u /* 休整三项菜单第0项使用；普通探索Y不再依赖这条特殊入口。 */
#define FN_INTERFACE_CTOR      0x00434920u
#define FN_INTERFACE_TICK      0x00436E00u
/*
 * 基线 EXE 中，原版窗口键盘处理 0x44A4D0 收到 VK_SPACE 后的地图动作目标地址。
 * refactor20a 不再要求此地址函数体逐字节一致；实际运行目标由 Runtime 从 CALL_MAP_SPACE_EVENT 解析。
 */
#define FN_MAP_SPACE_EVENT     0x0040B230u
#define FN_SAVE_SLOT_CTOR      0x004245B0u /* state7 天书与标题读档共同使用的原版 SaveSlot 构造函数。 */
#define FN_INTERFACE_OPTIONS_CTOR 0x00428510u /* state8 机能页面原版构造函数；只用于协议预检。 */
#define FN_SAVE_SLOT_REFRESH   0x00425340u
#define FN_SAVE_SLOT_ANIM      0x00424F00u
#define FN_TARGET_GRID_INDEX   0x00446490u
#define FN_CMD0_CLOSE          0x00417C80u
#define FN_EXPLORATION_MOVE    0x00408950u
#define FN_MOUSE_ACTION        0x00408A70u
#define FN_GET_CONTROLLED_ACTOR 0x00408C10u

/* r37 LT 探索调查：原版互动目标 resolver 及其能力级协议锚点。 */
#define FN_EXPLORATION_TARGET_RESOLVE    0x00409B50u
#define CALL_EXPLORATION_TARGET_RESOLVE  0x004098CBu
#define SIG_EXPLORATION_TARGET_CALLER    0x004098B8u
#define SIG_EXPLORATION_TARGET_RESOLVER  0x00409B50u
#define SIG_EXPLORATION_TARGET_TABLE     0x00409B6Eu
#define SIG_EXPLORATION_TARGET_TYPE      0x00409903u
#define SIG_EXPLORATION_TARGET_DISTANCE  0x00409918u
#define CONST_EXPLORATION_DISTANCE_100   0x00460290u
#define GLOBAL_MAP_CAMERA_X              0x00978514u
#define GLOBAL_MAP_CAMERA_Y              0x00978518u

/*
 * 原版两种通用双按钮类的 vtable Update 槽。
 * refactor9 只 Hook 8 个直接 0x427480 open CALL，实机证明这仍然覆盖不到用户实际遇到的询问框。
 * refactor10 改为观察类本身真正的虚函数 Update，因此不再关心对象究竟从哪个业务函数被打开。
 *
 * vtable 0x460C08：+0x18 槽 0x460C20 -> 0x4272C0
 * vtable 0x460C28：+0x18 槽 0x460C40 -> 0x4276F0
 */
#define VTABLE_CONFIRM1_UPDATE  0x00460C20u
#define VTABLE_CONFIRM2_UPDATE  0x00460C40u
#define FN_CONFIRM1_UPDATE      0x004272C0u
#define FN_CONFIRM2_UPDATE      0x004276F0u

/* 第二种同构双按钮更新函数 0x4276F0 的 HitTest / Event 调用点。 */
#define CALL_CONFIRM2_HIT_YES   0x00427800u
#define CALL_CONFIRM2_HIT_NO    0x00427819u
#define CALL_CONFIRM2_EVENT_YES 0x0042782Eu
#define CALL_CONFIRM2_EVENT_NO  0x0042784Fu

/*
 * 公共消息引擎。
 * refactor10 不再修改 GetAsyncKeyState 全局 IAT；只把这两个 6 字节
 * “mov esi,[0x4601A4]”改成“mov esi,我们的局部包装函数”。
 */
#define GLOBAL_DIALOGUE_MODE           0x0046F640u /* 公共消息子模式；2=剧情多行选项，3=剧情“是/否”。 */

/*
 * mode=2 多行剧情选项使用的原版行范围。
 *
 * 0x4044F0 会把两项数值代入下面的真实鼠标命中公式：
 * - 第一行命中区上边界：359 + 22 * first_line；
 * - 最后一行命中区下边界：379 + 22 * last_line。
 *
 * 插件只读取它们来计算“第几项”的隐藏命中坐标，不写行号、不写 VAR999，
 * 最终结果仍由 RPG.exe 在收到原版左键后自行提交。
 */
#define GLOBAL_DIALOGUE_MULTI_FIRST_LINE 0x0046F628u
#define GLOBAL_DIALOGUE_MULTI_LAST_LINE  0x0046F644u

#define GLOBAL_DIALOGUE_CHOICE_VISUAL_STATE 0x0046F678u /* 原版显示/动画状态，mode=3 时使用 8/9/10。 */
#define GLOBAL_DIALOGUE_CHOICE_HOVER_STATE  0x0046F679u /* 原版鼠标当前指向：8=无、10=第一项、9=第二项。 */
#define SIG_DIALOGUE_CHOICE_DISPATCH   0x00403E5Au /* mode switch；index2/3 分别进入 0x4044F0/0x404600。 */
#define CALL_DIALOGUE_MULTI_CHOICE_UPDATE 0x00403E79u
#define FN_DIALOGUE_MULTI_CHOICE_UPDATE   0x004044F0u
#define SIG_DIALOGUE_MULTI_CHOICE_CURSOR  0x004044F0u
#define CALL_DIALOGUE_MULTI_CHOICE_RESULT 0x004045A9u
#define CALL_DIALOGUE_CHOICE_UPDATE    0x00403E80u
#define FN_DIALOGUE_CHOICE_UPDATE      0x00404600u
#define SIG_DIALOGUE_CHOICE_CURSOR     0x00404600u
#define SIG_DIALOGUE_CHOICE_RESULT_A   0x00404690u
#define SIG_DIALOGUE_CHOICE_RESULT_B   0x004046A8u
#define GLOBAL_DIALOGUE_ID             0x0046F670u
/*
 * 公共消息引擎当前已经“走过/显示到”的字节位置与本句总字节长度。
 * 原版 0x4041FD 会把 0x46F688 与 0x46F684 直接比较：
 * - 两者不同：文字还在逐字显示；左键的作用是立刻把本句补全；
 * - 两者相同：本句已经完整显示；这时真正的下一句推进由 0x404440 那条等待路径负责。
 *
 * refactor11 用这两个只读状态把一颗 A 分配给正确阶段，避免 refactor10 的一次性 pending
 * 总被前面的“补全文字”读取先吃掉，导致后面的“推进下一句”永远拿不到 A。
 */
#define GLOBAL_DIALOGUE_TOTAL_BYTES    0x0046F684u
#define GLOBAL_DIALOGUE_VISIBLE_BYTES  0x0046F688u
#define PATCH_DIALOGUE_GETASYNC_A      0x004041D7u
#define PATCH_DIALOGUE_GETASYNC_B      0x0040447Eu
#define SIG_DIALOGUE_ACTIVE_BEGIN   0x00403B90u
#define SIG_DIALOGUE_INPUT_A        0x004041D7u
#define SIG_DIALOGUE_INPUT_B        0x0040447Eu
#define SIG_DIALOGUE_ACTIVE_END     0x00403F28u

/* 战斗视觉 HitTest 的精确 CALL 点。 */
#define CALL_CMD1_VISUAL_HIT_A 0x0041821Bu
#define CALL_CMD1_VISUAL_HIT_B 0x00419092u
#define CALL_CMD2_VISUAL_HIT   0x00419C35u
#define CALL_TOP_VISUAL_HIT    0x0041C3C8u
#define CALL_TOP_PANEL_ANIM    0x0041C3B2u /* 0x41C380 动画分支中唯一调用 0x41C470 的 CALL。 */

/*
 * refactor4 新增的四个“子菜单阶段切换”精确 CALL。
 * 前两条让插件能在 CMD1/CMD2 动画执行后恢复正确行视觉；
 * 后两条只负责在手柄导航拥有光标时拦住原版的“回到默认鼠标位置”动作。
 * 每一个地址都已用目标 RPG.exe 静态确认只有这一处对应调用，不能用范围扫描或模糊签名替代。
 */
#define CALL_CMD1_PANEL_ANIM        0x00418126u /* 唯一调用 FN_CMD1_PANEL_ANIM。 */
#define CALL_CMD2_PANEL_ANIM        0x00419B4Au /* 唯一调用 FN_CMD2_PANEL_ANIM。 */
#define CALL_CMD1_PANEL_CURSOR_WARP 0x004188CFu /* CMD1 展开完成后调用 0x43DF30。 */
#define CALL_CMD2_PANEL_CURSOR_WARP 0x0041A65Bu /* CMD2 展开完成后调用 0x43DF30。 */
#define CALL_CONFIRM_HIT_YES   0x004273F7u
#define CALL_CONFIRM_HIT_NO    0x00427410u

/* 战斗 ButtonEvent 的精确 CALL 点。return 2=确认/左释放，return 1=取消/右释放。 */
#define CALL_CMD0_EVENT          0x00417829u
#define CALL_CMD1_ROW_EVENT      0x00418175u
#define CALL_CMD1_PAGE_PREV      0x0041829Cu
#define CALL_CMD1_PAGE_NEXT      0x004182E1u
#define CALL_CMD1_CATEGORY_EVENT 0x00418331u
#define CALL_CMD1_CANCEL_EVENT   0x00418393u
#define CALL_CMD2_ROW_EVENT      0x00419BA9u
#define CALL_CMD2_CATEGORY_EVENT 0x00419CA6u
#define CALL_CMD2_CANCEL_EVENT   0x00419D81u
#define CALL_CMD2_PAGE_PREV      0x00419FB2u
#define CALL_CMD2_PAGE_NEXT      0x00419FDBu
#define CALL_TOP_EVENT           0x0041BAA6u
#define CALL_CMD3_EVENT          0x0041C7A2u
#define CALL_CONFIRM_EVENT_YES   0x00427425u
#define CALL_CONFIRM_EVENT_NO    0x00427446u
#define CALL_TARGET_GRID_INDEX   0x0044604Fu
#define CALL_EXPLORATION_MOVE    0x004088BDu
#define CALL_MOUSE_ACTION        0x00409597u
#define CALL_EXPLORE_CURSOR_DRAW 0x00408821u
#define CALL_RESULT_OUTER_EVENT  0x00412399u
#define CALL_RESULT_DETAIL_EVENT 0x00412F7Fu

/* 标题三项：dev17 已经用实机证明必须使用每个按钮自己的原生 Event CALL。 */
#define CALL_TITLE_HIT_0   0x00447C84u
#define CALL_TITLE_HIT_1   0x00447C9Au
#define CALL_TITLE_HIT_2   0x00447CB0u
#define CALL_TITLE_EVENT_0 0x00447D35u
#define CALL_TITLE_EVENT_1 0x00447D7Eu
#define CALL_TITLE_EVENT_2 0x00447D97u

/*
 * refactor19 的地图 Y 不再 patch 0x40CC73。
 * 下面三项用于验证“原版 Space -> 地图动作事件”的真实链：
 * - 0x44A4DF 比较 VK_SPACE；
 * - Space 分支检查 0x89F808==0 与 0x468BF0!=0；
 * - 0x44A57B 最终 CALL 0x40B230(1)。
 */
#define SIG_MAP_SPACE_BRANCH        0x0044A4DFu
#define CALL_MAP_SPACE_EVENT        0x0044A57Bu
#define GLOBAL_MAP_ACTION_BUSY      0x0089F808u
#define GLOBAL_MAP_KEY_MODE         0x00468BF0u

/*
 * 客栈/休整三项根菜单（诸态 / 炼化 / 歇息）。
 * 0x413850 原版 Update 每帧扫描三个 Button；这个对象也正是早期研究里误称“MapAction”的 0x89FCCC。
 * refactor29 起按真实业务重新命名，不再拿它当普通地图 Y 的入口。
 */
#define GLOBAL_INN_UI                 0x0089FCCCu
#define SIG_INN_UPDATE                0x00413850u
#define CALL_INN_BUTTON_HIT           0x00413909u
#define CALL_INN_BUTTON_EVENT         0x00413933u
#define INN_ACTIVE                    0x579u
#define INN_MODE                      0x57Cu
#define INN_BUTTON0                   0x588u
#define INN_BUTTON_COUNT              3

/*
 * 炼化主对象。客栈第二项由 0x4139DB 调用 0x40F6D0 构造，并写入 0x89FCBC。
 * 两层列表的所有当前行/页/类别字段都只读；插件永远通过真实 ButtonEvent 让 RPG.exe 自己修改。
 */
#define GLOBAL_SYNTHESIS_UI             0x0089FCBCu
#define FN_SYNTHESIS_CTOR               0x0040F6D0u
#define SIG_SYNTHESIS_UPDATE             0x0040FBE0u
#define CALL_SYNTHESIS_TOP_EVENT         0x0040FE04u
#define CALL_SYNTHESIS_PRIMARY_PREV      0x0040FF2Au
#define CALL_SYNTHESIS_PRIMARY_NEXT      0x0040FF94u
#define CALL_SYNTHESIS_SECONDARY_PREV    0x0040FFF7u
#define CALL_SYNTHESIS_SECONDARY_NEXT    0x00410061u
#define CALL_SYNTHESIS_SECONDARY_CANCEL  0x004100E4u
#define CALL_SYNTHESIS_SECONDARY_ROW     0x0041011Bu
#define CALL_SYNTHESIS_PRIMARY_ROW       0x00410212u

#define SYNTHESIS_ACTIVE                 0x579u
#define SYNTHESIS_TOPBAR                 0x588u
#define SYNTHESIS_PRIMARY_LIST           0x594u
#define SYNTHESIS_SECONDARY_LIST         0x598u
#define SYNTHESIS_PRIMARY_CATEGORY       0x5A0u
#define SYNTHESIS_PRIMARY_CURRENT_ROW    0x5A4u
#define SYNTHESIS_PRIMARY_TOTAL_PAGES    0x5A8u
#define SYNTHESIS_PRIMARY_CURRENT_PAGE   0x5ACu
#define SYNTHESIS_SECONDARY_CATEGORY     0x5B4u
#define SYNTHESIS_SECONDARY_CURRENT_ROW  0x5B8u
#define SYNTHESIS_SECONDARY_TOTAL_PAGES  0x5BCu
#define SYNTHESIS_SECONDARY_CURRENT_PAGE 0x5C0u
#define SYNTHESIS_CONFIRM_POPUP          0x5C8u

#define SYNTHESIS_CHILD_ACTIVE            0x579u
#define SYNTHESIS_TOP_BUTTON0             0x57Cu /* index0=用器右侧退出图标；index1..6=六类别。 */
#define SYNTHESIS_CATEGORY_COUNT          6
#define SYNTHESIS_ROW_BUTTON0             0x57Cu
#define SYNTHESIS_ROW_COUNT               9
#define SYNTHESIS_PRIMARY_RECORD0         0x5C4u
#define SYNTHESIS_SECONDARY_RECORD0       0x60Cu
#define SYNTHESIS_RECORD_ACTIVE           0x30u
#define SYNTHESIS_PRIMARY_PAGE_PREV       0x5ECu
#define SYNTHESIS_PRIMARY_PAGE_NEXT       0x5F0u
#define SYNTHESIS_SECONDARY_PAGE_PREV     0x634u
#define SYNTHESIS_SECONDARY_PAGE_NEXT     0x638u
#define SYNTHESIS_SECONDARY_CANCEL_BUTTON 0x63Cu

/*
 * 商店主对象。EVE opcode 0x36 在 0x40D120 根据脚本传入的 mode 创建同一个
 * 0x413FA0 大型界面对象，并统一写入 0x89FCD4。不同城镇/商品配置只换数据，
 * 顶部六类、左买入列、右卖出列和数量窗的对象结构不变，因此只需一套 Shop Adapter。
 *
 * 下面所有 current/page/category 字段只读；插件仍只向真实 ButtonEvent CALL 投递 code=1/2。
 */
#define GLOBAL_SHOP_UI                   0x0089FCD4u
#define FN_SHOP_CTOR                     0x00413FA0u
#define SIG_SHOP_UPDATE                  0x004145B0u
#define FN_SHOP_QUANTITY_CTOR            0x00427920u
#define SIG_SHOP_QUANTITY_UPDATE         0x00427E80u
#define SIG_SHOP_ITEM_INFO_UPDATE        0x00417810u
#define FN_SHOP_ITEM_INFO_TRANSITION     0x004178A0u

#define CALL_SHOP_TOP_EVENT              0x0041479Fu
#define CALL_SHOP_SELL_PAGE_PREV         0x0041481Fu
#define CALL_SHOP_SELL_PAGE_NEXT         0x00414887u
#define CALL_SHOP_BUY_PAGE_PREV          0x0041493Eu
#define CALL_SHOP_BUY_PAGE_NEXT          0x004149A9u
#define CALL_SHOP_SELL_ROW_EVENT         0x00414A65u
#define CALL_SHOP_BUY_ROW_EVENT          0x00414CC6u
#define CALL_SHOP_QUANTITY_CONFIRM       0x00428016u
#define CALL_SHOP_QUANTITY_CANCEL        0x00428037u
#define CALL_SHOP_QUANTITY_INC           0x004280DAu
#define CALL_SHOP_QUANTITY_DEC           0x0042813Fu
#define CALL_SHOP_SELL_ITEM_INFO_OPEN    0x00414CB3u
#define CALL_SHOP_BUY_ITEM_INFO_OPEN     0x00415102u
#define CALL_SHOP_ITEM_INFO_CLOSE_EVENT  CALL_CMD0_EVENT /* 同一公共 Event CALL，只能由 UiBridge 安装一次。 */

#define SHOP_ACTIVE                      0x579u
#define SHOP_LAST_TRANSACTION_SIDE       0x580u /* 1=左买入，0=右卖出；鼠标 hover 也会同步它。 */
#define SHOP_TOPBAR                      0x59Cu
#define SHOP_SELL_LIST                   0x5A4u /* 画面右列：玩家库存/卖出。 */
#define SHOP_BUY_LIST                    0x5A8u /* 画面左列：店铺商品/买入。 */
#define SHOP_ITEM_INFO                   0x5B4u /* 右键/Y 呼出的道具信息子对象。 */
#define SHOP_CATEGORY                    0x5B8u /* 原版 raw index 1..6，增长方向与画面左到右相反。 */
#define SHOP_SELL_CURRENT_ROW            0x5BCu
#define SHOP_SELL_TOTAL_PAGES            0x5C0u
#define SHOP_SELL_CURRENT_PAGE           0x5C4u
#define SHOP_BUY_CURRENT_ROW             0x5CCu
#define SHOP_BUY_TOTAL_PAGES             0x5D0u
#define SHOP_BUY_CURRENT_PAGE            0x5D4u
#define SHOP_QUANTITY_POPUP              0x5E4u

#define SHOP_TOP_BUTTON0                 0x57Cu /* index0=“用器”右侧退出图标；index1..6=六类。 */
#define SHOP_CATEGORY_COUNT              6
#define SHOP_ROW_BUTTON0                 0x57Cu
#define SHOP_ROW_RECORD0                 0x5BCu
#define SHOP_ROW_COUNT                   8
#define SHOP_RECORD_ACTIVE               0x30u
#define SHOP_SELL_PAGE_PREV_BUTTON       0x5E0u
#define SHOP_SELL_PAGE_NEXT_BUTTON       0x5E4u
#define SHOP_BUY_PAGE_PREV_BUTTON        0x600u
#define SHOP_BUY_PAGE_NEXT_BUTTON        0x604u

#define SHOP_QUANTITY_ACTIVE             0x579u
#define SHOP_QUANTITY_CONFIRM_BUTTON     0x58Cu
#define SHOP_QUANTITY_CANCEL_BUTTON      0x590u
#define SHOP_QUANTITY_DEC_BUTTON         0x5A0u
#define SHOP_QUANTITY_INC_BUTTON         0x5A4u
#define SHOP_QUANTITY_RESULT             0x5B0u /* -1=等待，1=确认买/卖，0=取消。只读。 */
#define SHOP_QUANTITY_TRANSACTION_SIDE   0x5D8u /* 1=买入，0=卖出。只读。 */

#define SHOP_ITEM_INFO_CURRENT_POSITION  0x56Cu
#define SHOP_ITEM_INFO_MODE              0x580u /* 0=动画空闲，1=打开动画，2=关闭动画；0不等于关闭。 */
#define SHOP_ITEM_INFO_CLOSED_POSITION   0x588u
#define SHOP_ITEM_INFO_OPEN_POSITION     0x58Cu
#define SHOP_ITEM_INFO_CLOSE_BUTTON      0x598u

/* 主 Interface 根导航的原版 ButtonEvent CALL；这些 CALL 才复用 UiBridge 的 button/code 队列。 */
#define CALL_INTERFACE_CATEGORY_1     0x00435BBAu
#define CALL_INTERFACE_CATEGORY_2     0x00435BD2u
#define CALL_INTERFACE_CATEGORY_3     0x00435BEAu
#define CALL_INTERFACE_CATEGORY_4     0x00435C05u
#define CALL_INTERFACE_CATEGORY_5     0x00435C20u
#define CALL_INTERFACE_CATEGORY_6     0x00435C3Bu
#define CALL_INTERFACE_CATEGORY_7     0x00435C56u
#define CALL_INTERFACE_CATEGORY_8     0x00435C81u
#define CALL_INTERFACE_EXIT_EVENT     0x00435C9Cu
#define CALL_INTERFACE_ROLE_EVENT     0x00435E93u

/*
 * 主 Interface state2“法宝/道具”页的真实 ButtonEvent CALL。
 * 这些地址来自 0x437E..0x4383 的页面 Update，不与其它页面共享业务假设：
 * - SUBTYPE：6 个顶部子类型使用同一 CALL 扫描；
 * - PAGE_PREV/NEXT：右侧列表自己的上一页/下一页按钮；
 * - ROW：8 个物品行使用同一 CALL 扫描；
 * - SPECIAL_A/B：页面底部两个特殊按钮，原版通过 +0x45 动态决定当前哪个可用。
 */
#define CALL_INTERFACE_ITEMS_SUBTYPE       0x00438093u
#define CALL_INTERFACE_ITEMS_SPECIAL_A     0x00438136u
#define CALL_INTERFACE_ITEMS_SPECIAL_B     0x0043817Au
#define CALL_INTERFACE_ITEMS_PAGE_PREV     0x0043820Eu
#define CALL_INTERFACE_ITEMS_PAGE_NEXT     0x00438274u
#define CALL_INTERFACE_ITEMS_ROW           0x004382DBu
#define SIG_INTERFACE_ITEMS_SPECIAL_ENABLE  0x004380D0u /* 子类型切换后，原版动态启用“分发/弃置”两个特殊按钮的机器码。 */


/*
 * 主 Interface state3“绝学/法术”页的真实 ButtonEvent CALL。
 * 这些地址来自 0x43BE40 构造出的 state3 页面对象在 0x43C1F0..0x43C525 的 Update：
 * - SUBTYPE：两个顶部子类型按钮（绝技/咒法）共用一个 CALL；
 * - PAGE_PREV/NEXT：技能列表自己的上一页/下一页按钮；
 * - ROW：8 行技能按钮共用一个 CALL。
 * 这里仍然只接原版 ButtonEvent，不直接写页码、行号或技能结果。
 */
#define CALL_INTERFACE_SKILLS_SUBTYPE      0x0043C363u
#define CALL_INTERFACE_SKILLS_PAGE_PREV    0x0043C3B6u
#define CALL_INTERFACE_SKILLS_PAGE_NEXT    0x0043C3E8u
#define CALL_INTERFACE_SKILLS_ROW          0x0043C461u
/*
 * state3 治疗法术确认“使用”以后，RPG.exe 会进入一个独立的鼠标选人阶段：
 * - 0x43C933 在 5 个角色透明 Button 上循环调用 0x431380，左键(code=2)选中目标；
 * - 0x43C1F7 对全屏取消 Button 调用 0x431380，右键(code=1)退出目标选择。
 * refactor28 只把这两处接进既有 UiBridge，绝不直接写目标角色 ID。
 */
#define CALL_INTERFACE_SKILLS_TARGET_ROW    0x0043C933u
#define CALL_INTERFACE_SKILLS_TARGET_CANCEL 0x0043C1F7u
#define SIG_INTERFACE_SKILLS_TARGET_UPDATE  0x0043C19Eu

/*
 * 主 Interface state4“及身/装备”页的 6 个真实 ButtonEvent CALL。
 *
 * 这些地址来自 state4 对象构造函数 0x42DC90 对应的页面 Update 0x42DEF0：
 * - SLOT_0/1/2：三个装备栏位按钮，原版分别把 page+0x5A0 切成 0/1/2；
 * - PAGE_PREV/NEXT：候选装备列表自己的上一页/下一页；
 * - ROW：9 行候选装备共用同一个 ButtonEvent CALL。
 *
 * 手柄层只点击这些原版按钮，不直接修改角色 +0x90/+0x94/+0x98，也不直接调用装备写入函数。
 * 这样装备替换、旧装备返还库存、刷新角色属性等副作用仍完全由 RPG.exe 自己处理。
 */
#define CALL_INTERFACE_EQUIPMENT_PAGE_PREV 0x0042DF52u
#define CALL_INTERFACE_EQUIPMENT_PAGE_NEXT 0x0042DFA6u
#define CALL_INTERFACE_EQUIPMENT_ROW       0x0042DFF1u
#define CALL_INTERFACE_EQUIPMENT_SLOT_0    0x0042E072u
#define CALL_INTERFACE_EQUIPMENT_SLOT_1    0x0042E09Fu
#define CALL_INTERFACE_EQUIPMENT_SLOT_2    0x0042E0CBu
#define SIG_INTERFACE_EQUIPMENT_UPDATE     0x0042DEF0u

/*
 * state5“五内”页面：0x440610 的循环只用一个 ButtonEvent CALL 扫描 11 个真实按钮。
 * panel+0x57C..+0x5A0 = 五组减/加按钮（共10个），panel+0x5A4 = “蕴魂”提交按钮。
 */
#define CALL_INTERFACE_INNER_STATS_MOUSE  0x0044066Eu /* state5 每个按钮先走 0x431400 读取真实鼠标按键边沿。 */
#define CALL_INTERFACE_INNER_STATS_EVENT  0x004406A8u

/* 主 Interface state6“阵形”页面：8 个阵位共用一个循环 ButtonEvent CALL。 */
#define CALL_INTERFACE_FORMATION_EVENT      0x0043FA05u
#define SIG_INTERFACE_FORMATION_UPDATE      0x0043F9A0u
#define SIG_INTERFACE_FORMATION_BUTTON_LOOP 0x0043F9E4u

/*
 * 主 Interface 构造函数中两个页面对象的真实构造 CALL。
 * 这两处只用于“页面身份”预检，不会被 Hook：
 * - state7 在 0x434E93 调 0x4245B0，随后保存到 Interface+0x654；
 * - state8 在 0x434EC9 调 0x428510，随后保存到 Interface+0x658。
 */
#define CALL_INTERFACE_TOME_CTOR          0x00434E93u
#define CALL_INTERFACE_OPTIONS_CTOR       0x00434EC9u

/*
 * 0x4245B0 SaveSlot 被标题、主 Interface state7“天书”和地图存档点 wrapper 共同复用。
 * 槽位本身继续使用上面的 CALL_SAVE_*；这里两处 CALL 属于 SaveSlot 内嵌的共享三项动作窗口：
 * - 0x426365：逐个检查“存档/读档/取消”三个真实 Button 的 HitTest；
 * - 0x426387：紧接着调用同一 Button 的 0x431380 ButtonEvent。
 *
 * 三项窗口对象位于 SaveSlot+0x5A4，不是 state7 私有能力。它自己的 +0x5B4 又挂着原版 Yes/No，
 * 因此二次确认继续交给 ConfirmDialog 的真实 vtable Update 生命周期，不复制第三套询问框。
 */
#define SIG_SAVE_ACTION_UPDATE              0x004262C0u
#define CALL_SAVE_ACTION_HIT                 0x00426365u
#define CALL_SAVE_ACTION_EVENT               0x00426387u

/*
 * 主 Interface state8“机能”页面 0x428950 的五个真实 ButtonEvent CALL。
 * 原版依次处理：音乐减、音乐加、音效减、音效加、空明流转。
 * 手柄只在这些 CALL 里返回一次 code=2；真正的 0..9 数值、音量应用和 Yes/No 打开都仍由 RPG.exe 修改。
 */
#define SIG_INTERFACE_OPTIONS_UPDATE         0x00428950u
#define CALL_INTERFACE_OPTIONS_MUSIC_DEC     0x0042898Cu
#define CALL_INTERFACE_OPTIONS_MUSIC_INC     0x004289BFu
#define CALL_INTERFACE_OPTIONS_SOUND_DEC     0x004289F3u
#define CALL_INTERFACE_OPTIONS_SOUND_INC     0x00428A26u
#define CALL_INTERFACE_OPTIONS_KARMA         0x00428A5Au
#define FN_BUTTON_MOUSE_EDGE              0x00431400u /* 只用于 state5 的鼠标按键边沿读取；不是通用 ButtonEvent。 */
#define SIG_INTERFACE_INNER_STATS_UPDATE  0x00440610u

/* 共享 SaveSlot：游戏线程动画安全点、4 行事件、取消、上下页。 */
#define CALL_SAVE_TICK_ANIM    0x00424BE2u
#define CALL_SAVE_ROW_EVENT    0x00424C90u
#define CALL_SAVE_CANCEL_EVENT 0x00424E23u
#define CALL_SAVE_PAGE_PREV    0x00424E86u
#define CALL_SAVE_PAGE_NEXT    0x00424EB2u

/* Target 原版为了把阵位坐标同步到鼠标，存在三处调用同一个 SetCursorPos 包装器。
 * refactor1 不改写这三处 CALL；把它们纳入预检只是为了证明 Target 坐标协议仍是 dev20 已验证的版本。 */
#define FN_MOUSE_SET_CURSOR_WRAPPER 0x0043DF30u
#define CALL_TARGET_MOUSE_POS_A     0x00446673u
#define CALL_TARGET_MOUSE_POS_B     0x004466E1u
#define CALL_TARGET_MOUSE_POS_C     0x004467B9u

/* ---------------- 游戏内主 Interface 字段 ---------------- */
/*
 * 0x413850 的休整/特殊地图三按钮对象字段不再作为普通探索 Y 的运行时协议。
 * 相关地址只保留在文档/证据里用于说明 refactor17 为什么失败，避免以后再次误用。
 */

/* 主 Interface 0x434920 / 0x435AA0。 */
#define INTERFACE_READY_COUNTER      0x590u /* 0x436330：>8 才允许根导航事件扫描。 */
#define INTERFACE_STATE              0x5BCu
#define INTERFACE_SELECTED_ROLE      0x5C4u
#define INTERFACE_ROLE_COUNT         0x5ECu
#define INTERFACE_CLOSE_STATE        0x5F8u
#define INTERFACE_ROLE_OBJECTS       0x60Cu
#define INTERFACE_MAIN_NAV           0x620u
#define INTERFACE_PAGE_1             0x63Cu
#define INTERFACE_PAGE_2             0x640u
#define INTERFACE_PAGE_3             0x644u
#define INTERFACE_PAGE_4             0x648u
#define INTERFACE_PAGE_5             0x64Cu
#define INTERFACE_PAGE_6             0x650u
#define INTERFACE_PAGE_7             0x654u
#define INTERFACE_PAGE_8             0x658u
#define INTERFACE_MAIN_BUTTON0       0x57Cu
#define INTERFACE_EXIT_BUTTON        0x59Cu
#define INTERFACE_ROLE_BUTTON        0x5A0u
#define INTERFACE_CATEGORY_COUNT     8
#define INTERFACE_ROLE_MAX           5

/* ---------------- 主 Interface state2“法宝/道具”页字段 ---------------- */
#define INTERFACE_ITEMS_SUBPANEL          0x584u /* 6 个子类型 Button 的父面板。 */
#define INTERFACE_ITEMS_LIST              0x590u /* 8 行列表、翻页和底部特殊按钮的父对象。 */
#define INTERFACE_ITEMS_SUBINDEX          0x594u /* 原版当前子类型索引，范围 0..5。 */
#define INTERFACE_ITEMS_CURRENT_ROW       0x598u /* 当前页选中行，范围 0..7。 */
#define INTERFACE_ITEMS_TOTAL_PAGES       0x59Cu /* 当前子类型总页数。 */
#define INTERFACE_ITEMS_CURRENT_PAGE      0x5A0u /* 当前页，0 基。 */
#define INTERFACE_ITEMS_POPUP_PRIMARY     0x5ACu /* 物品主操作产生的页面专属弹窗。 */
#define INTERFACE_ITEMS_POPUP_SECONDARY   0x5B0u /* 底部特殊操作产生的页面专属弹窗。 */

#define INTERFACE_ITEMS_SUB_BUTTON0       0x57Cu /* 子类型面板第 0 个真实 Button。 */
#define INTERFACE_ITEMS_ROW_BUTTON0       0x57Cu /* 列表第 0 行真实 Button。 */
#define INTERFACE_ITEMS_ROW_RECORD0       0x5BCu /* 列表第 0 行数据/可见记录指针。 */
#define INTERFACE_ITEMS_PAGE_PREV_BUTTON  0x5E0u
#define INTERFACE_ITEMS_PAGE_NEXT_BUTTON  0x5E4u
#define INTERFACE_ITEMS_SPECIAL_BUTTON_A  0x5E8u /* 原版仅在对应子类型启用；截图语义为“分发”。 */
#define INTERFACE_ITEMS_SPECIAL_BUTTON_B  0x5ECu /* 原版在允许弃置的子类型启用；会进入页面自己的确认弹窗。 */
#define INTERFACE_ITEMS_SUBTYPE_COUNT      6
#define INTERFACE_ITEMS_ROW_COUNT          8
#define INTERFACE_ITEMS_RECORD_ACTIVE      0x30u


/* ---------------- 主 Interface state3“绝学/法术”页字段 ---------------- */
#define INTERFACE_SKILLS_LIST              0x5DCu /* 8 行技能、上一页/下一页按钮的父对象。 */
#define INTERFACE_SKILLS_SUBPANEL          0x5E4u /* 2 个子类型按钮的父对象。 */
#define INTERFACE_SKILLS_CURRENT_PAGE      0x5ECu /* 当前页，0 基。 */
#define INTERFACE_SKILLS_TOTAL_PAGES       0x5F0u /* 当前子类型总页数。 */
#define INTERFACE_SKILLS_CURRENT_ROW       0x5F4u /* 当前页选中行，0..7。 */
#define INTERFACE_SKILLS_TOTAL_COUNT       0x5F8u /* 当前子类型总技能数。 */
#define INTERFACE_SKILLS_POPUP             0x5FCu /* 使用技能时出现的页面专属双按钮确认框。 */
#define INTERFACE_SKILLS_SUB_BUTTON0       0x57Cu /* 子类型面板第 0 个真实 Button。 */
#define INTERFACE_SKILLS_SUBINDEX          0x584u /* 子类型面板当前索引，0..1。 */
#define INTERFACE_SKILLS_ROW_RECORD0       0x5A0u /* 列表第 0 行记录指针。 */
#define INTERFACE_SKILLS_ROW_BUTTON0       0x5C0u /* 列表第 0 行真实 Button。 */
#define INTERFACE_SKILLS_PAGE_PREV_BUTTON  0x600u
#define INTERFACE_SKILLS_PAGE_NEXT_BUTTON  0x604u
#define INTERFACE_SKILLS_SUBTYPE_COUNT      2
#define INTERFACE_SKILLS_ROW_COUNT          8
#define INTERFACE_SKILLS_RECORD_ACTIVE      0x30u

/*
 * state3 治疗法术的原版“确认后选角色”状态。
 * 这些字段来自 0x43C160 Update / 0x43C920 角色命中循环：
 * - +0x768 非 0：正在等待玩家用鼠标选择治疗对象；
 * - +0x770..+0x780：5 个与顶部角色头像对齐的透明 Button；
 * - +0x784：覆盖页面的取消 Button，原版右键时 ButtonEvent 返回 code=1。
 * 插件只读取这些字段、点击真实 ButtonEvent，不改 +0x58C/+0x588/+0x788/+0x78C 的业务目标数据。
 */
#define INTERFACE_SKILLS_TARGET_ACTIVE       0x768u
#define INTERFACE_SKILLS_TARGET_BUTTON0      0x770u
#define INTERFACE_SKILLS_TARGET_CANCEL       0x784u
#define INTERFACE_SKILLS_TARGET_BUTTON_COUNT 5


/* ---------------- 主 Interface state4“及身/装备”页字段 ---------------- */
/*
 * state4 页面对象由 0x42DC90 构造，Interface+0x648 持有。
 * 下面每个偏移都来自 0x42DEF0 页面 Update 的直接读写，不根据截图猜。
 */
#define INTERFACE_EQUIPMENT_SLOT_PANEL       0x580u /* 三个栏位按钮的父对象。 */
#define INTERFACE_EQUIPMENT_LIST             0x584u /* 9 行候选 + 上一页/下一页的父对象。 */
#define INTERFACE_EQUIPMENT_CURRENT_PAGE     0x590u /* 当前候选页，0 基。 */
#define INTERFACE_EQUIPMENT_TOTAL_PAGES      0x594u /* 当前栏位候选总页数。 */
#define INTERFACE_EQUIPMENT_TOTAL_COUNT      0x598u /* 当前栏位候选装备总数。 */
#define INTERFACE_EQUIPMENT_CURRENT_ROW      0x59Cu /* 当前页原版选中行，0..8。 */
#define INTERFACE_EQUIPMENT_CURRENT_SLOT     0x5A0u /* 0/1/2，对应三个及身栏位。 */

/* slot panel 内三个真实 Button。原版 0x42E072/9F/CB 分别扫描它们。 */
#define INTERFACE_EQUIPMENT_SLOT_BUTTON0     0x5B4u
#define INTERFACE_EQUIPMENT_SLOT_COUNT       3

/* list 内 9 个真实候选 Button；原版循环从 +0x5D0 扫到 +0x5F0。 */
#define INTERFACE_EQUIPMENT_ROW_BUTTON0      0x5D0u
#define INTERFACE_EQUIPMENT_ROW_COUNT        9
#define INTERFACE_EQUIPMENT_PAGE_PREV_BUTTON 0x5F4u
#define INTERFACE_EQUIPMENT_PAGE_NEXT_BUTTON 0x5F8u

/* ---------------- 主 Interface state5“五内”页面 ---------------- */
#define INTERFACE_INNER_STATS_PANEL          0x57Cu /* 保存 11 个真实 Button 的内部面板对象。 */
#define INTERFACE_INNER_STATS_ROLE_INDEX     0x584u /* 当前显示角色索引；由原版 Interface 自己更新。 */
#define INTERFACE_INNER_STATS_BUTTON0        0x57Cu /* 面板内第0个按钮指针。 */
#define INTERFACE_INNER_STATS_BUTTON_COUNT   11     /* 5组减/加 + 1个“蕴魂”。 */
#define INTERFACE_INNER_STATS_COMMIT_INDEX   10     /* 第11个按钮：原版提交当前五项临时值。 */
#define INTERFACE_INNER_STATS_NODE_COUNT     5

/*
 * 原版内部五项顺序由 0x440F70 的按钮布局坐标直接闭合：
 *   0=迅（左） 1=烈（上） 2=神（右） 3=魔（右下） 4=魂（左下）。
 * 用户希望 X 的视觉循环为 烈→神→魔→魂→迅，因此 Adapter 使用 1,2,3,4,0 的顺序表。
 */
#define INTERFACE_INNER_NODE_SWIFT           0
#define INTERFACE_INNER_NODE_FIERCE          1
#define INTERFACE_INNER_NODE_DIVINE          2
#define INTERFACE_INNER_NODE_DEMON           3
#define INTERFACE_INNER_NODE_SOUL            4

/* 主 Interface state6“阵形”页面。 */
#define INTERFACE_FORMATION_PANEL             0x580u /* 8阵位 Button 与原版选择/拖动图示的父面板。 */
#define INTERFACE_FORMATION_SWAP_PENDING      0x584u /* 0=尚未选来源；非0=已选来源，等待目标阵位。 */
#define INTERFACE_FORMATION_SOURCE_SLOT       0x588u /* 原版第一次 code=2 后保存的来源阵位。 */
#define INTERFACE_FORMATION_TARGET_SLOT       0x58Cu /* 原版第二次 code=2 时保存的目标阵位。 */
#define INTERFACE_FORMATION_HOVER_SLOT        0x590u /* 原版实体鼠标 hover 记录；插件不直接写。 */
#define INTERFACE_FORMATION_ROLE_MAP          0x594u /* 原版阵位映射指针；业务仍由原函数维护。 */
#define INTERFACE_FORMATION_BUTTON0           0x59Cu /* panel 内第0个阵位 Button。 */
#define INTERFACE_FORMATION_SLOT_COUNT        8

/*
 * state6 原版自己用于“来源阵位/目标阵位指示图示”的 8 项布局表。
 * 0x43F9A0 在阵形选择事务里直接以 slot*4 读取这两张表，所以它们比插件从 Button 外观猜拓扑更权威。
 * 注意：这些值只用于“谁在谁的左/右/上/下”这样的相对空间关系；真正把手形鼠标放到屏幕上时，
 * 仍使用 Button 的完整屏幕矩形，因为表里的数值属于页面局部布局坐标，不包含窗口/父对象基准。
 */
#define INTERFACE_FORMATION_LAYOUT_X_TABLE     0x0046BFF8u
#define INTERFACE_FORMATION_LAYOUT_Y_TABLE     0x0046C018u

/* Button 几何字段。0x431310 会用这些字段计算 HitTest 的屏幕矩形。 */
#define BUTTON_BASE_X_PTR                    0x20u
#define BUTTON_BASE_Y_PTR                    0x24u
#define BUTTON_LOCAL_X                       0x34u
#define BUTTON_WIDTH                         0x3Cu
#define BUTTON_HEIGHT                        0x40u

/*
 * 0x431310 Button HitTest 的“宽高为 0 时从 Sprite 取真实尺寸”回退链。
 * Button+0x30 -> Sprite；Sprite+0x35C -> frame owner；frame owner+0x34 -> frame geometry；
 * frame geometry+0x08/+0x0C 分别是本次 HitTest 真正使用的宽/高。
 * 五内/阵形如果只读 Button+0x3C/+0x40，会把合法的 0 当成“没有尺寸”，这是 refactor23 两个页面失效的共同根因。
 */
#define BUTTON_SPRITE_PTR                    0x30u
#define BUTTON_SPRITE_FRAME_OWNER            0x35Cu
#define BUTTON_SPRITE_FRAME_GEOMETRY         0x34u
#define BUTTON_SPRITE_FRAME_WIDTH            0x08u
#define BUTTON_SPRITE_FRAME_HEIGHT           0x0Cu

/* ---------------- Battle 对象字段 ---------------- */
#define BATTLE_CURRENT_ACTOR   0x59Cu
#define BATTLE_ACTION_READY    0x5A4u
#define BATTLE_SUBMENU_ACTIVE  0x5B0u
#define BATTLE_ACTIVE_COMMAND  0x5ACu
#define BATTLE_COMMAND_PANEL   0x5E8u
#define BATTLE_CMD0_SUBUI      0x5DCu
#define BATTLE_CMD1_SUBUI      0x5E4u
#define BATTLE_CMD2_SUBUI      0x5E0u
#define BATTLE_CMD3_SUBUI      0x5ECu
#define COMMAND_MARKER_OBJECT  0x588u /* 原版 0x41C380：顶层当前命中标记对象。 */
#define COMMAND_BUTTONS        0x58Cu /* 六个顶层真实 Button 指针数组起点。 */
#define COMMAND_COUNT          6
#define BUTTON_LOCAL_Y         0x38u  /* 原版 Button 自身局部 Y。 */
#define BUTTON_HOVER_ACTIVE    0x44u  /* CMD1/CMD2 原版行 HitTest：1=当前行视觉激活，0=其它行。 */
#define MARKER_LOCAL_Y         0x24u  /* 原版 ITF0026 marker 的局部 Y。 */

#define SUB0_MAIN_BUTTON       0x598u
#define SUB1_ROW_PANEL         0x618u
#define SUB1_CATEGORY_PANEL    0x61Cu
#define SUB1_CANCEL_BUTTON     0x628u
#define SUB1_CONFIRM_POPUP     0x62Cu
#define SUB1_TOTAL_PAGES       0x600u
#define SUB1_CURRENT_PAGE      0x604u
#define SUB1_TOTAL_ITEMS       0x608u
#define SUB1_CURRENT_ROW       0x60Cu
#define SUB1_CURRENT_CATEGORY  0x5F8u
#define SUB2_ROW_PANEL         0x77Cu
#define SUB2_CATEGORY_PANEL    0x780u
#define SUB2_CONFIRM_POPUP     0x784u
#define SUB2_CANCEL_BUTTON     0x788u
#define SUB2_CURRENT_PAGE      0x580u
#define SUB2_TOTAL_PAGES       0x584u
#define SUB2_CURRENT_ROW       0x588u
#define SUB2_TOTAL_ITEMS       0x58Cu
#define SUB3_MAIN_BUTTON       0x57Cu
#define SUB3_SELECTED_INDEX    0x5B8u
#define SUB3_COUNT             0x58Cu
#define SUB_ROWS_BASE          0x57Cu
#define SUB_ROWS_COUNT         8

/* 技能/道具的双按钮确认框。 */
#define POPUP_ACTIVE           0x579u
#define POPUP_BUTTON_YES       0x584u
#define POPUP_BUTTON_NO        0x588u
#define POPUP_NATIVE_SELECTION 0x58Cu
#define POPUP_RESULT           0x590u
#define POPUP_TRANSITION       0x57Cu /* 0=稳定显示；1=展开；2=关闭。 */
#define POPUP_ANIM_STEP        0x5ACu
#define POPUP_ANIM_LIMIT       0x5A8u

/* Battle Result/Reward UI。 */
#define RESULT_ACTIVE          0x579u
#define RESULT_DETAIL_PTR      0x600u
#define RESULT_OUTER_BUTTON    0x604u
#define RESULT_DETAIL_BUTTON   0x628u

/* ---------------- Title / SaveSlot 对象字段 ---------------- */
#define TITLE_SELECTED_VISUAL  0x57Cu
#define TITLE_STATE            0x58Cu
#define TITLE_SAVE_UI          0x5B4u
#define TITLE_BUTTON0          0x5A8u
#define TITLE_BUTTON1          0x5ACu
#define TITLE_BUTTON2          0x5B0u
#define TITLE_BUTTON_COUNT     3

#define SAVE_ACTIVE            0x579u
#define SAVE_SELECTED_ROW      0x594u
#define SAVE_PAGE_BASE         0x598u
#define SAVE_ROWS_BASE         0x584u
#define SAVE_ROW_COUNT         4
#define SAVE_PAGE_PREV_BUTTON  0x59Cu
#define SAVE_PAGE_NEXT_BUTTON  0x5A0u
#define SAVE_CONFIRM_POPUP     0x5ACu
#define SAVE_CANCEL_BUTTON     0x5B4u
#define SAVE_MODE              0x5A8u
#define SAVE_ROW_INNER_BUTTON  0x584u
/* row+0x57C 仅作为已经逆向确认的“存在有效存档”字段留档。
 * 重要：dev19/dev20 已确认所有显示槽都必须允许手柄选择，因此现行导航绝不能拿它过滤候选。 */
#define SAVE_ROW_HAS_DATA      0x57Cu
#define SAVE_ANIM_COUNTDOWN    0x5BCu
#define SAVE_ANIM_ROW          0x5C0u

/* 独立存档包装层（0x413B00）的两个已闭合字段。 */
#define SAVE_POINT_SLOT         0x580u
#define SAVE_POINT_PHASE        0x584u
#define SAVE_POINT_PHASE_STABLE 0
#define SAVE_POINT_PHASE_OPEN   1
#define SAVE_POINT_PHASE_CLOSE  2

/*
 * SaveSlot 内嵌“存档/读档/取消”动作窗口（0x425FE0 构造）。
 * 三个 Button 的内存 index 不是视觉从上到下的顺序：构造坐标证明 index2 在最上、index1 居中、index0 在最下。
 * index0 点击后不会打开 Yes/No，因此它就是“取消”；index1/2 会进入原版二次确认。
 */
#define SAVE_ACTION_UI             0x5A4u
#define SAVE_ACTION_ACTIVE         0x579u
#define SAVE_ACTION_BUTTONS        0x58Cu
#define SAVE_ACTION_COUNT          3
#define SAVE_ACTION_CANCEL_INDEX   0
#define SAVE_ACTION_SELECTED_INDEX 0x598u
#define SAVE_ACTION_RESULT_INDEX   0x5A0u
#define SAVE_ACTION_POPUP          0x5B4u

/*
 * state8“机能”页字段（Interface+0x658 -> page）。
 * +0x57C/+0x580 是音乐/音效的 0..9 原版值；插件只留档，不直接写。
 * +0x594/+0x598 是音乐减/加，+0x59C/+0x5A0 是音效减/加，+0x590 是“空明流转”。
 * +0x5A8 是点“空明流转”后打开的原版 Yes/No 对象。
 */
#define INTERFACE_OPTIONS_MUSIC_VALUE   0x57Cu
#define INTERFACE_OPTIONS_SOUND_VALUE   0x580u
#define INTERFACE_OPTIONS_KARMA_BUTTON  0x590u
#define INTERFACE_OPTIONS_MUSIC_DEC     0x594u
#define INTERFACE_OPTIONS_MUSIC_INC     0x598u
#define INTERFACE_OPTIONS_SOUND_DEC     0x59Cu
#define INTERFACE_OPTIONS_SOUND_INC     0x5A0u
#define INTERFACE_OPTIONS_POPUP         0x5A8u
#define INTERFACE_OPTIONS_ROW_COUNT     3

/* ---------------- Cursor / Target 对象字段 ---------------- */
#define MOUSE_DRAW_ENABLE      0x248u
#define MOUSE_POS_X            0x238u
#define MOUSE_POS_Y            0x23Cu
#define MOUSE_SPRITE           0x240u

#define TARGET_MAP_BASE        0x0FB4u
#define TARGET_MAP_COUNT       28
#define TARGET_CURRENT_ID      0x1024u
#define TARGET_MODE            0x1028u
#define TARGET_SUBTYPE         0x107Cu
#define TARGET_EVENT_CODE      0x10E0u /* Target 一次性 A/B 动作码；由游戏线程选择器消费。 */
#define TARGET_ACTIVE          0x10F0u
#define TARGET_X_TABLE_PTR     0x0FACu
#define TARGET_Y_TABLE_PTR     0x0FB0u
#define TARGET_X_OFFSET        0x1030u
#define TARGET_Y_OFFSET        0x1034u
#define TARGET_GRID_ROWS       4
#define TARGET_GRID_COLS       7
#define TARGET_ROLE_BASE_PTR   0x0218u
#define FIGHTROLE_STRIDE       0x0DE4u
#define FIGHTROLE_FORMATION_SLOT 0x0B9Cu
#define FIGHTROLE_SPECIAL_STATE 0x0CE4u
#define FIGHTROLE_SPECIAL_EXCLUDE 0x00000BC0u
#define MAX_BATTLE_ROLE_ID     16

/* 通用 Button 结构。这里只记录当前代码真正会读取的字段。 */
#define BTN_PARENT_X_PTR       0x20u
#define BTN_PARENT_Y_PTR       0x24u
#define BTN_SPRITE             0x30u
#define BTN_OFS_X              0x34u
#define BTN_OFS_Y              0x38u
#define BTN_WIDTH_OVERRIDE     0x3Cu
#define BTN_HEIGHT_OVERRIDE    0x40u

#endif /* CASTLE_PAD_GAME_ADDRESSES_H */
