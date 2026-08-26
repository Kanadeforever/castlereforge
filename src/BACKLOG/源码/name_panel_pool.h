#ifndef CASTLE_BACKLOG_NAME_PANEL_POOL_H
#define CASTLE_BACKLOG_NAME_PANEL_POOL_H

#include "platform.h"

/*
 * name_panel_pool.h
 *
 * 这个小模块专门管理 Backlog 自己的 F-Name.SF2 姓名框对象。
 *
 * 为什么不能继续直接使用 RPG.exe 全局地址 0x46F658：
 * - 那个指针属于“当前剧情对话”的生命周期；
 * - NPC 改变时，原版 0x403C60 会析构旧对象并重新分配新对象；
 * - 回到自由探索后，那里可能仍然残留旧地址，但“地址还像指针”并不代表对象还活着；
 * - Backlog 保存的是历史，历史条目当然可能比当前 NPC 对象活得更久。
 *
 * 所以 v0.3.2-test1 改成：
 * - Backlog 打开时，在游戏线程自己创建 4 个 F-Name.SF2；
 * - 四个可见槽各用一个独立对象，同一帧不会反复推进同一个 SF2 对象；
 * - Backlog 关闭时，在游戏线程析构并释放这 4 个对象；
 * - 从头到尾都不读取 0x46F658，也不调用 0x403C60，更不会加载人物 %d-2.SF2。
 */

/* 建立最多四个私有姓名框。已经建立时再次调用会直接成功，不重复分配。 */
int NamePanelPool_Create(void);

/* 返回指定可见槽位的私有姓名框；slot 必须是 0..3，异常时返回 NULL。 */
void* NamePanelPool_Get(u32 slot);

/*
 * 在游戏线程销毁全部私有姓名框。
 * 只应该从 Backlog 的场景更新 Hook 中调用，不能在 DllMain Loader Lock 中调用游戏函数。
 */
void NamePanelPool_Destroy(void);

/* 当前是否完整拥有四个可用对象。主要用于日志和 fail-safe 判断。 */
int NamePanelPool_Ready(void);

#endif /* CASTLE_BACKLOG_NAME_PANEL_POOL_H */
