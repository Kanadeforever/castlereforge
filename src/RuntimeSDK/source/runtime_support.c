#include "runtime_internal.h"

/* 0=未开始，1=某线程正在初始化，2=成功，-1=失败。 */
static volatile LONG g_initialize_state;

void Runtime_Lock(volatile LONG* lock_value) {
    /*
     * Runtime 的锁只保护很短的固定数组操作。
     * Sleep(0) 主动让出当前时间片，避免一个等待线程把 CPU 核心空转占满。
     */
    while (InterlockedCompareExchange(lock_value, 1, 0) != 0) {
        Sleep(0u);
    }
}

void Runtime_Unlock(volatile LONG* lock_value) {
    InterlockedExchange(lock_value, 0);
}

void Runtime_ByteCopy(void* destination, const void* source, CastleU32 size) {
    volatile CastleU8* output = (volatile CastleU8*)destination;
    const volatile CastleU8* input = (const volatile CastleU8*)source;
    CastleU32 index;

    if (!output || !input) return;
    for (index = 0u; index < size; ++index) output[index] = input[index];
}

void Runtime_ByteZero(void* destination, CastleU32 size) {
    /* volatile 明确禁止 MSVC 把循环重新合成为外部 memset，保持 /NODEFAULTLIB。 */
    volatile CastleU8* output = (volatile CastleU8*)destination;
    CastleU32 index;

    if (!output) return;
    for (index = 0u; index < size; ++index) output[index] = 0u;
}

CastleU32 Runtime_StringLength(const char* text) {
    CastleU32 length = 0u;
    if (!text) return 0u;
    while (text[length]) ++length;
    return length;
}

int Runtime_StringEquals(const char* left, CastleU32 left_length,
                         const char* right, CastleU32 right_length) {
    CastleU32 index;

    if (!left || !right || left_length != right_length) return 0;
    for (index = 0u; index < left_length; ++index) {
        if ((CastleU8)left[index] != (CastleU8)right[index]) return 0;
    }
    return 1;
}

int Runtime_EnsureInitialized(void) {
    LONG state = InterlockedCompareExchange(&g_initialize_state, 1, 0);

    if (state == 2) return 1;
    if (state == -1) return 0;

    if (state == 0) {
        HMODULE pinned_module = NULL;

        /*
         * 当前线程赢得初始化所有权。顺序先清空内存模块，再建立路径，
         * 这样路径失败时诊断/注册表仍处于明确的空状态。
         */
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_PIN,
                                (LPCWSTR)(const void*)&Runtime_EnsureInitialized,
                                &pinned_module)) {
            InterlockedExchange(&g_initialize_state, -1);
            return 0;
        }
        Runtime_DiagnosticsInitialize();
        Runtime_RegistryInitialize();
        Runtime_HookInitialize();
        Runtime_ScheduleInitialize();
        if (!Runtime_PathInitialize()) {
            InterlockedExchange(&g_initialize_state, -1);
            return 0;
        }
        Runtime_SymbolsInitialize();
        Runtime_DisplayInitialize();
        Runtime_RenderInitialize();
        if (!Runtime_WindowInitialize()) {
            InterlockedExchange(&g_initialize_state, -1);
            return 0;
        }
        Runtime_DiagnosticAppend("[Runtime] Castle_Runtime ABI v1 core initialized.");
        InterlockedExchange(&g_initialize_state, 2);
        return 1;
    }

    /* 另一个线程正在初始化。这里只等待固定内存初始化完成，不调用插件代码。 */
    for (;;) {
        state = InterlockedCompareExchange(&g_initialize_state, 0, 0);
        if (state == 2) return 1;
        if (state == -1) return 0;
        Sleep(0u);
    }
}
