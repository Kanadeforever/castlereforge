#ifndef CASTLE_WINDOW_API_H
#define CASTLE_WINDOW_API_H

/*
 * CastleWindow_API.h
 *
 * Window v1 让整合模式中的插件共享一个 Runtime WndProc 分发点。
 * Observer 永远先执行且不能吞消息；Filter 随后执行并可显式消费。插件实际加载顺序
 * 不参与排序，同阶段同优先级按稳定插件 ID 排列。
 */

#include "CastleRuntime_API.h"

#pragma pack(push, 4)

#define CASTLE_WINDOW_INTERFACE_ID "org.castlereforge.game.window"
#define CASTLE_WINDOW_API_VERSION_1 1u
#define CASTLE_WINDOW_STRUCTURE_VERSION_1 1u

#define CASTLE_WINDOW_API_MAGIC       0x4E495743ul /* CWIN */
#define CASTLE_WINDOW_CLIENT_MAGIC    0x4C435743ul /* CWCL */
#define CASTLE_WINDOW_MESSAGE_MAGIC   0x534D5743ul /* CWMS */
#define CASTLE_WINDOW_DECISION_MAGIC  0x44465743ul /* CWFD */
#define CASTLE_WINDOW_STATE_MAGIC     0x54535743ul /* CWST */

#define CASTLE_WINDOW_CAP_OBSERVER (1ul << 0)
#define CASTLE_WINDOW_CAP_FILTER   (1ul << 1)

#define CASTLE_WINDOW_PHASE_EARLY  0u
#define CASTLE_WINDOW_PHASE_NORMAL 1u
#define CASTLE_WINDOW_PHASE_LATE   2u
#define CASTLE_WINDOW_PRIORITY_EARLY   0u
#define CASTLE_WINDOW_PRIORITY_DEFAULT 1u
#define CASTLE_WINDOW_PRIORITY_LATE    2u

#define CASTLE_WINDOW_CLIENT_OBSERVER 1u
#define CASTLE_WINDOW_CLIENT_FILTER   2u

typedef struct CastleWindowMessageV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleAddress window;
    CastleU32 message;
    CastleAddress w_param;
    CastleAddress l_param;
    CastleU32 window_generation;
} CastleWindowMessageV1;

typedef struct CastleWindowFilterDecisionV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 consume;
    CastleS32 result;
} CastleWindowFilterDecisionV1;

typedef void (CASTLE_RUNTIME_CALL *CastleWindowObserverFn)(
    const CastleWindowMessageV1* message,
    void* user_context);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleWindowFilterFn)(
    const CastleWindowMessageV1* message,
    CastleWindowFilterDecisionV1* decision,
    void* user_context);

/* Observer/Filter 各自只读取与自己类型匹配的函数指针。 */
typedef struct CastleWindowClientV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleU32 phase;
    CastleU32 priority;
    CastleWindowObserverFn observer;
    CastleWindowFilterFn filter;
    void* user_context;
    CastleStringView label;
} CastleWindowClientV1;

typedef struct CastleWindowStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 version;
    CastleU32 flags;
    CastleAddress window;
    CastleU32 generation;
    CastleU32 ready;
    CastleU32 window_thread_id;
    CastleU32 observer_count;
    CastleU32 filter_count;
} CastleWindowStateV1;

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetGameWindowFn)(
    CastleWindowStateV1* out_state);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterWindowClientFn)(
    CastlePluginHandle plugin,
    const CastleWindowClientV1* client,
    CastleLeaseHandle* out_client);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleSetWindowClientReadyFn)(
    CastleLeaseHandle client,
    CastleU32 ready);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleUnregisterWindowClientFn)(
    CastleLeaseHandle client);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetWindowGenerationFn)(
    CastleU32* out_generation);

/* 16 字节表头 + 6 个函数指针 = 40 字节。 */
typedef struct CastleWindowApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 api_version;
    CastleU32 capability_flags;
    CastleGetGameWindowFn GetGameWindow;
    CastleRegisterWindowClientFn RegisterMessageObserver;
    CastleRegisterWindowClientFn RegisterMessageFilter;
    CastleSetWindowClientReadyFn SetWindowClientReady;
    CastleUnregisterWindowClientFn UnregisterWindowClient;
    CastleGetWindowGenerationFn GetWindowGeneration;
} CastleWindowApiV1;

#define CASTLE_SIZEOF_WINDOW_MESSAGE_V1  36u
#define CASTLE_SIZEOF_WINDOW_DECISION_V1 24u
#define CASTLE_SIZEOF_WINDOW_CLIENT_V1   44u
#define CASTLE_SIZEOF_WINDOW_STATE_V1    40u
#define CASTLE_SIZEOF_WINDOW_API_V1      40u

#pragma pack(pop)

#endif /* CASTLE_WINDOW_API_H */
