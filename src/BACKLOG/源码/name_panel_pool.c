#include "name_panel_pool.h"
#include "game_addresses.h"
#include "runtime.h"

/*
 * name_panel_pool.c
 *
 * 这份代码只做一件事：照着原版 0x403C60 已确认的 F-Name.SF2 创建/销毁协议，
 * 给 Backlog 建立自己的姓名框对象。
 *
 * 原版创建 F-Name 的机器码顺序（0x403DA4..0x403DF9）是：
 *
 *   1. 分配 0x84 字节；
 *   2. 0x407080 构造对象；
 *   3. 0x4070D0(this, "F-Name.SF2", 0, 0, 2, 0, 0) 加载资源。
 *
 * 原版销毁顺序（0x403CFC..0x403D13）是：
 *
 *   1. 0x4070A0(this) 释放对象持有的 SF2 数据；
 *   2. 0x451550(this) 释放对象本身。
 *
 * 我们严格复用这套协议，但完全不触碰人物 %d-2.SF2。
 */

#define NAME_PANEL_SLOT_COUNT 4u
#define NAME_PANEL_OBJECT_BYTES 0x84u

/*
 * 这些 typedef 的“第一个参数”是 this 指针。
 * BACKLOG_THISCALL 会让编译器把它放进 ECX，后面的参数仍按原版顺序压栈。
 */
typedef void* (BACKLOG_CDECL *PFN_GameAlloc)(u32 bytes);
typedef void  (BACKLOG_CDECL *PFN_GameFree)(void* memory);
typedef void* (BACKLOG_THISCALL *PFN_Sf2Ctor)(void* object);
typedef void  (BACKLOG_THISCALL *PFN_Sf2Dtor)(void* object);
typedef void  (BACKLOG_THISCALL *PFN_Sf2Load)(void* object,
                                               const char* path,
                                               i32 x,
                                               i32 y,
                                               i32 mode,
                                               i32 flag,
                                               i32 extra);

/* 四个屏幕槽位各自拥有一个对象。NULL 表示该槽尚未建立。 */
static void* g_name_panels[NAME_PANEL_SLOT_COUNT];
static int g_pool_ready;

/*
 * 不依赖 strlen/memset：逐字节把对象指针数组清零。
 * 这里只清插件自己的四个指针，不接触游戏对象本体。
 */
static void name_panel_clear_slots(void) {
    u32 index;
    for (index = 0u; index < NAME_PANEL_SLOT_COUNT; ++index) {
        g_name_panels[index] = NULL;
    }
    g_pool_ready = 0;
}

/*
 * 0x4070D0 成功后，原版对象至少应满足：
 * - object+0x30 是加载后的 SF2 数据基址；
 * - object+0x34 是同一 SF2 基址；
 * - 这个基址至少能读取资源头。
 *
 * 这是“刚刚由我们自己创建”的对象，因此这里的检查不是为了识别陈旧堆指针，
 * 而是为了发现资源加载失败，避免把半初始化对象交给绘制函数。
 */
static int name_panel_loaded_ok(void* object) {
    void* resource_a;
    void* resource_b;

    if (!Runtime_MemoryRangeReadable(object, NAME_PANEL_OBJECT_BYTES)) return 0;

    resource_a = *(void**)((u8*)object + 0x30u);
    resource_b = *(void**)((u8*)object + 0x34u);

    if (!resource_a || resource_a != resource_b) return 0;
    if (!Runtime_MemoryRangeReadable(resource_a, 0x38u)) return 0;
    return 1;
}

/*
 * 销毁一个槽位。
 * 注意一定先调用 SF2 析构，再释放 0x84 字节对象；顺序与原版完全一致。
 */
static void name_panel_destroy_one(u32 slot) {
    PFN_Sf2Dtor dtor = (PFN_Sf2Dtor)FN_SF2_OBJECT_DTOR;
    PFN_GameFree free_fn = (PFN_GameFree)FN_GAME_FREE;
    void* object;

    if (slot >= NAME_PANEL_SLOT_COUNT) return;
    object = g_name_panels[slot];
    if (!object) return;

    /*
     * 先把插件槽清空。
     * 即使下面游戏函数内部发生异常，也不会让后续代码误以为这个对象仍由池持有。
     */
    g_name_panels[slot] = NULL;

    dtor(object);
    free_fn(object);
}

void NamePanelPool_Destroy(void) {
    u32 index;

    for (index = 0u; index < NAME_PANEL_SLOT_COUNT; ++index) {
        name_panel_destroy_one(index);
    }
    name_panel_clear_slots();
}

int NamePanelPool_Create(void) {
    PFN_GameAlloc alloc_fn = (PFN_GameAlloc)FN_GAME_ALLOC;
    PFN_GameFree free_fn = (PFN_GameFree)FN_GAME_FREE;
    PFN_Sf2Ctor ctor = (PFN_Sf2Ctor)FN_SF2_OBJECT_CTOR;
    PFN_Sf2Dtor dtor = (PFN_Sf2Dtor)FN_SF2_OBJECT_DTOR;
    PFN_Sf2Load load = (PFN_Sf2Load)FN_SF2_OBJECT_LOAD;
    static const char resource_path[] = "F-Name.SF2";
    u32 index;

    if (g_pool_ready) return 1;

    /*
     * 上一次若只建立了一半，先完整清理。
     * 正常运行不会走到这里，但这种写法能让“中途某个槽加载失败”也保持可重试。
     */
    NamePanelPool_Destroy();

    for (index = 0u; index < NAME_PANEL_SLOT_COUNT; ++index) {
        void* memory;
        void* object;

        /* 第一步：与原版一样，从游戏自己的堆分配 0x84 字节。 */
        memory = alloc_fn(NAME_PANEL_OBJECT_BYTES);
        if (!memory) {
            Runtime_Log("[姓名框池] 游戏分配器无法建立 F-Name 对象；本次退化为只画姓名文字。");
            NamePanelPool_Destroy();
            return 0;
        }

        /* 第二步：构造器会把 SF2 指针、标志等关键字段初始化为安全状态。 */
        object = ctor(memory);
        if (!object) {
            /* 理论上 0x407080 会原样返回 this；仍然保留这个 fail-safe。 */
            free_fn(memory);
            Runtime_Log("[姓名框池] F-Name 构造器返回空对象；本次退化为只画姓名文字。");
            NamePanelPool_Destroy();
            return 0;
        }

        /*
         * 第三步：严格复制原版 F-Name 调用参数。
         * x/y 先用 0；真正绘制每条历史时，Backlog 会在调用 0x407510 前写入对应坐标。
         */
        load(object, resource_path, 0, 0, 2, 0, 0);

        if (!name_panel_loaded_ok(object)) {
            /*
             * load 已经运行过，所以即使失败也必须先 dtor，再 free。
             * 否则 object+0x30 里已经分配的部分资源可能泄漏。
             */
            dtor(object);
            free_fn(object);
            Runtime_Log("[姓名框池] F-Name.SF2 加载后内部协议异常；本次退化为只画姓名文字。");
            NamePanelPool_Destroy();
            return 0;
        }

        g_name_panels[index] = object;
    }

    g_pool_ready = 1;
    Runtime_Log("[姓名框池] 已建立 4 个 Backlog 私有 F-Name.SF2；不再借用原版当前 NPC 的 0x46F658 对象。");
    return 1;
}

void* NamePanelPool_Get(u32 slot) {
    if (!g_pool_ready || slot >= NAME_PANEL_SLOT_COUNT) return NULL;
    return g_name_panels[slot];
}

int NamePanelPool_Ready(void) {
    return g_pool_ready;
}
