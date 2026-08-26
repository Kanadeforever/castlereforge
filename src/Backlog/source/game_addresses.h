#ifndef CASTLE_BACKLOG_GAME_ADDRESSES_H
#define CASTLE_BACKLOG_GAME_ADDRESSES_H

/*
 * game_addresses.h
 *
 * 本文件只保存“已经由 RPG.exe 机器码和现有研究资料确认”的地址。
 * 业务代码看到的是有意义的名字，而不是散落一地的 0x004xxxxx 数字。
 *
 * 当前两份确认兼容的 RPG.exe：
 * - b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
 * - 8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f
 *
 * 插件不只相信文件名或版本文字。启动时还会检查 vtable、函数入口和消息全局的
 * 机器协议；任何关键项不一致都会停止安装，不把未知版本当成“应该也差不多”。
 */

/* 场景总管理对象的 vtable。第 0 项是每个逻辑步的更新函数，第 1 项是绘制函数。 */
#define VTABLE_SCENE_WORLD              0x00460298u
#define FN_SCENE_WORLD_UPDATE           0x0040B150u
#define FN_SCENE_WORLD_DRAW             0x0040B050u
/*
 * 0x40B16B～0x40B173 每个逻辑步执行：push 0xBB8、push scene_world、call 0x434500。
 * 0x434500 把 key/object 成对写入本帧绘制队列并 ret 8，不读取 ECX。
 * Backlog 活动时只复刻这一次登记，从而冻结 0x40B150 其余逻辑但保留 0x40B050 绘制。
 */
#define FN_DRAW_QUEUE_REGISTER          0x00434500u
#define SCENE_WORLD_DRAW_QUEUE_KEY      0x00000BB8u

/*
 * 原版“设置说话人表现”的函数身份锚点。
 * 参数依次是说话人编号、名字/头像样式编号和左右/变体标记。
 * 传入说话人 0 会回到没有姓名框的普通对话框。
 * v0.2.0 不再在浏览历史时调用它；旧版实机已证明跨人物重建会造成 SAF FrameID 越界。
 * 该地址保留用于确认 speaker/style/variant 三个捕获字段仍来自已知原版协议。
 */
#define FN_DIALOGUE_SET_SPEAKER         0x00403C60u

/*
 * 现代整屏列表只接管 0x404800 内部两处很窄的“画通用对话框 / 画正文”CALL。
 * 宽屏插件验证的是 0x404800 入口和 0x40B08C 外层 CALL；这两个既有检查点仍保持原样。
 */
/* 有姓名剧情中的人物 %d-2.SF2 绘制 CALL。Backlog 活动时只拦截这一个绘制点，不改 speaker_active。 */
#define CALL_DIALOGUE_SPEAKER_PORTRAIT_DRAW 0x00404859u
#define CALL_DIALOGUE_PANEL_DRAW            0x0040486Eu
/* 有姓名对话的 F-Name.SF2 独立绘制 CALL。 */
#define CALL_DIALOGUE_NAME_PANEL_DRAW       0x00404899u
/* 原版姓名文字独立使用 0x402EE0；Backlog 活动时拦截，避免真实剧情姓名叠到历史列表上。 */
#define CALL_DIALOGUE_NAME_TEXT_DRAW        0x004048E6u
#define CALL_DIALOGUE_TEXT_DRAW             0x004049FFu
#define FN_DIALOGUE_PANEL_DRAW          0x00407510u
#define FN_DIALOGUE_TEXT_DRAW           0x00402EE0u
/*
 * 原版当前剧情使用的 F-Name.SF2 对象槽。
 *
 * 0x40488D：mov ecx, dword ptr [0x46F658]
 * 0x404899：call 0x407510
 *
 * v0.3.2-test1 以后这个地址只保留为逆向证据，Backlog 运行代码禁止读取它。
 * 原因是它会随 NPC 切换由 0x403C60 析构/重建，历史系统不能借用这种短生命周期对象。
 */
#define GLOBAL_DIALOGUE_NAME_PANEL_OBJECT 0x0046F658u

/* 原版消息绘制用的 512 字节工作缓冲区指针和当前消息资源来源。 */
#define GLOBAL_DIALOGUE_SOURCE_BASE     0x0046F660u
#define GLOBAL_DIALOGUE_DISPLAY_BUFFER  0x0046F664u
#define GLOBAL_DIALOGUE_SPEAKER_NAME    0x0046F668u
#define GLOBAL_DIALOGUE_RECORD_OFFSET   0x0046F67Cu
#define GLOBAL_DIALOGUE_PAGE_OFFSET     0x0046F680u
#define GLOBAL_DIALOGUE_TOTAL_BYTES     0x0046F684u
#define GLOBAL_DIALOGUE_VISIBLE_BYTES   0x0046F688u

/*
 * 消息生命周期与显示状态。
 * target/current 都为 4 时，原版 0x404800 会绘制稳定展开的对话框。
 * mode 使用 0..3；Backlog 展示时会暂存原值并使用一个无效值，使 0x403E30
 * 仍能执行但不会进入任何“推进剧情/选择答案”的分支。
 */
#define GLOBAL_DIALOGUE_MODE            0x0046F640u
#define GLOBAL_DIALOGUE_ID              0x0046F670u
#define GLOBAL_DIALOGUE_TARGET_STATE    0x0046F678u
#define GLOBAL_DIALOGUE_CURRENT_STATE   0x0046F679u

/* 说话人三项参数正是 0x403C60 保存、0x404800 消费的原版字段。 */
#define GLOBAL_DIALOGUE_SPEAKER_STYLE   0x0046F674u
#define GLOBAL_DIALOGUE_SPEAKER_ACTIVE  0x0046F675u
#define GLOBAL_DIALOGUE_SPEAKER_VARIANT 0x0046F676u

/*
 * 场景事件和输入门。
 * SHOW_MESSAGE 会把 0x89F81D/1E 置 1；事件激活会把 0x468BB9 置 0。
 * Backlog 临时复用同一组“原版已经会暂停剧情与地图输入”的门，关闭时逐字节恢复。
 */
#define GLOBAL_MAP_ACTION_BUSY          0x0089F808u
#define GLOBAL_EVENT_YIELD_FLAG         0x0089F81Du
#define GLOBAL_EVENT_BLOCK_FLAG         0x0089F81Eu
#define GLOBAL_EVENT_TABLE              0x0089F820u
#define GLOBAL_MAP_INPUT_GATE           0x00468BB9u
#define GLOBAL_MAP_KEY_MODE             0x00468BF0u

/*
 * 以下全局槽用于排除标题、战斗和各种菜单。
 * Backlog 在“有对话框”时总可打开；没有对话框时则要求这些已知 UI 全部不存在，
 * 再同时通过事件空闲、地图表存在和输入门开放，才认定为自由探索。
 */
#define GLOBAL_BATTLE_UI                0x0089FD74u
#define GLOBAL_RESULT_UI                0x0089FCC8u
#define GLOBAL_TARGET_SELECTOR          0x008E2410u
#define GLOBAL_TITLE_UI                 0x008E241Cu
#define GLOBAL_INTERFACE_UI             0x008DED0Cu
#define GLOBAL_SAVE_POINT_UI            0x0089FCD0u
#define GLOBAL_INN_UI                   0x0089FCCCu
#define GLOBAL_SYNTHESIS_UI             0x0089FCBCu
#define GLOBAL_SHOP_UI                  0x0089FCD4u
#define GLOBAL_MOVIE_OBJECT             0x0046F390u
#define MOVIE_ACTIVE_FLAG_OFFSET        0x0000000Au

/* 消息缓冲的原版大小。最后一个字节固定放 0x02 结束标记。 */
#define DIALOGUE_BUFFER_BYTES           512u
#define DIALOGUE_PAYLOAD_MAX_BYTES      511u

/* Backlog 自由探索展示时使用的临时非零 ID；关闭时只在值仍相同的情况下恢复。 */
#define BACKLOG_SYNTHETIC_DIALOGUE_ID   0xFFFFFFFEu

/* 无效 mode 不会命中原版 0..3 的分发表，但仍让绘制函数正常使用 current state。 */
#define BACKLOG_READ_ONLY_MODE          0xFFFFFFFFu

#endif /* CASTLE_BACKLOG_GAME_ADDRESSES_H */
