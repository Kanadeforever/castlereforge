#ifndef CASTLE_RUNTIME_API_H
#define CASTLE_RUNTIME_API_H

/*
 * CastleRuntime_API.h
 *
 * 这是 CastleReforge RuntimeSDK 的最底层公共 ABI 头。
 * “ABI”可以理解为不同 DLL 之间约定好的插头形状：字段顺序、字段大小、函数调用约定
 * 只要有一项不一致，调用方就可能把普通数字误当成函数地址，最终让游戏直接崩溃。
 *
 * 这个头故意不包含 windows.h、标准库或 C++ 头。这样做有三个原因：
 * 1. 现有插件同时使用 C、C++、MSVC 和 clang-cl，公共头不能偏向某一种编译器；
 * 2. 项目使用 /NODEFAULTLIB，无意间引入 CRT 类型会破坏当前无 CRT 构建；
 * 3. 第三方作者只需包含这个头，不应该被迫继承 Runtime 内部的 Windows 类型和宏。
 *
 * 当前只支持 32 位 x86 Windows。所有指针在 ABI 检查中都必须是 4 字节。
 */

#if defined(_MSC_VER) || defined(__clang__)
#define CASTLE_RUNTIME_CALL __cdecl
#elif defined(__GNUC__) && defined(__i386__)
#define CASTLE_RUNTIME_CALL __attribute__((cdecl))
#else
#define CASTLE_RUNTIME_CALL
#endif

/*
 * 所有公开结构统一按 4 字节对齐。
 * push/pop 保证包含本头之前的工程对齐设置不会泄漏进来，本头也不会污染包含者后续代码。
 */
#pragma pack(push, 4)

/*
 * 固定宽度基础整数。
 * 在目标 Windows ABI 中 char=8位、short=16位、long=32位；测试器会机械验证，不能只靠注释相信。
 */
typedef unsigned char  CastleU8;
typedef signed char    CastleS8;
typedef unsigned short CastleU16;
typedef signed short   CastleS16;
typedef unsigned long  CastleU32;
typedef signed long    CastleS32;

/*
 * CastleAddress 用数字保存目标 x86 进程地址。
 * CastleModule 保存 HMODULE 的 32 位值，但公共头不暴露 Windows 的 HMODULE 类型。
 */
typedef CastleU32 CastleAddress;
typedef CastleU32 CastleModule;

/*
 * 所有句柄都由 Runtime 分配，0 永远表示“无效/尚未分配”。
 * 调用方只能把句柄原样交回 Runtime，不能把它当数组下标或内存地址解释。
 */
typedef CastleU32 CastlePluginHandle;
typedef CastleU32 CastleTransactionHandle;
typedef CastleU32 CastleClaimHandle;
typedef CastleU32 CastleProviderHandle;
typedef CastleU32 CastleLeaseHandle;
typedef CastleU32 CastleTaskHandle;

/* CastleResult 的 0/正数/负数规则写在下方结果码中。 */
typedef CastleS32 CastleResult;

/*
 * 公共字符串统一使用 UTF-8 字节视图。
 * length 不包含结尾 NUL，data[length] 也不保证一定存在 NUL。
 * Runtime 如果需要长期保存文字，必须在函数返回前复制，不能永久借用调用方缓冲区。
 */
typedef struct CastleStringView {
    const char* data;
    CastleU32 length;
} CastleStringView;

/* 根 ABI 和公共结构版本。已经发布的数字永远不能换成其它含义。 */
#define CASTLE_RUNTIME_ABI_V1              1u
#define CASTLE_PLUGIN_DESCRIPTOR_V1        1u
#define CASTLE_QUERY_VERSION_1             1u
#define CASTLE_RUNTIME_INFO_VERSION_1      1u
#define CASTLE_PLUGIN_STATE_VERSION_1      1u
#define CASTLE_DIAGNOSTIC_BUFFER_VERSION_1 1u
#define CASTLE_BOOTSTRAP_VERSION_1         1u

/*
 * magic 使用内存中的 ASCII 顺序帮助诊断错误指针。
 * 例如 0x4E555243 在小端 x86 内存中依次是 C、R、U、N。
 */
#define CASTLE_RUNTIME_API_MAGIC        0x4E555243ul /* CRUN */
#define CASTLE_PLUGIN_DESC_MAGIC        0x474C5043ul /* CPLG */
#define CASTLE_QUERY_MAGIC              0x59525143ul /* CQRY */
#define CASTLE_INTERFACE_API_MAGIC      0x49504143ul /* CAPI */
#define CASTLE_RUNTIME_INFO_MAGIC       0x464E4943ul /* CINF */
#define CASTLE_PLUGIN_STATE_MAGIC       0x54535043ul /* CPST */
#define CASTLE_DIAGNOSTIC_BUFFER_MAGIC  0x41494443ul /* CDIA */
#define CASTLE_BOOTSTRAP_REQUEST_MAGIC  0x51524243ul /* CBRQ */
#define CASTLE_BOOTSTRAP_RESULT_MAGIC   0x53524243ul /* CBRS */

/*
 * 结果码：
 * - 0 表示完整成功；
 * - 正数表示调用成功，但调用方应知道一个可说明状态；
 * - 负数表示失败，失败调用不能假装已经修改成功。
 */
#define CASTLE_OK                                  ((CastleResult)0)
#define CASTLE_STATUS_ALREADY_DONE                 ((CastleResult)1)
#define CASTLE_STATUS_DEFAULT_BACKEND              ((CastleResult)2)
#define CASTLE_STATUS_OPTIONAL_UNAVAILABLE         ((CastleResult)3)
#define CASTLE_ERROR_INVALID_ARGUMENT              ((CastleResult)-1)
#define CASTLE_ERROR_ABI_MISMATCH                  ((CastleResult)-2)
#define CASTLE_ERROR_RUNTIME_FAULT                 ((CastleResult)-3)
#define CASTLE_ERROR_UNKNOWN_GAME_BUILD            ((CastleResult)-4)
#define CASTLE_ERROR_DUPLICATE_PLUGIN_ID           ((CastleResult)-5)
#define CASTLE_ERROR_INTERFACE_NOT_FOUND           ((CastleResult)-6)
#define CASTLE_ERROR_INTERFACE_VERSION             ((CastleResult)-7)
#define CASTLE_ERROR_RESOURCE_CONFLICT             ((CastleResult)-8)
#define CASTLE_ERROR_SIGNATURE_MISMATCH            ((CastleResult)-9)
#define CASTLE_ERROR_EXPECTED_BYTES                ((CastleResult)-10)
#define CASTLE_ERROR_TRANSACTION_STATE             ((CastleResult)-11)
#define CASTLE_ERROR_TRANSACTION_ROLLED_BACK       ((CastleResult)-12)
#define CASTLE_ERROR_NOT_READY                     ((CastleResult)-13)
#define CASTLE_ERROR_STALE_GENERATION              ((CastleResult)-14)
#define CASTLE_ERROR_UNLOAD_UNSUPPORTED            ((CastleResult)-15)
#define CASTLE_ERROR_LATE_LOAD_UNSUPPORTED         ((CastleResult)-16)
#define CASTLE_ERROR_ENTRY_GATE_CONFLICT           ((CastleResult)-17)
#define CASTLE_ERROR_BUFFER_TOO_SMALL              ((CastleResult)-18)
#define CASTLE_ERROR_RUNTIME_REQUIRED               ((CastleResult)-19)

/* 根能力位 low 32 位。某个功能没有真实实现时，对应位必须保持 0。 */
#define CASTLE_RUNTIME_CAP_PLUGIN_REGISTRY           (1ul << 0)
#define CASTLE_RUNTIME_CAP_QUERY_INTERFACE           (1ul << 1)
#define CASTLE_RUNTIME_CAP_DIAGNOSTICS                (1ul << 2)
#define CASTLE_RUNTIME_CAP_HOOK_TRANSACTION           (1ul << 3)
#define CASTLE_RUNTIME_CAP_HOOK_CHAIN                 (1ul << 4)
#define CASTLE_RUNTIME_CAP_SERVICE_PROVIDER           (1ul << 5)
#define CASTLE_RUNTIME_CAP_DISPLAY_V1                 (1ul << 6)
#define CASTLE_RUNTIME_CAP_BACKGROUND_SCHEDULE_V1     (1ul << 7)
#define CASTLE_RUNTIME_CAP_PATH_V1                    (1ul << 8)
#define CASTLE_RUNTIME_CAP_CLOCK_V1                   (1ul << 9)
#define CASTLE_RUNTIME_CAP_WINDOW_V1                  (1ul << 10)
#define CASTLE_RUNTIME_CAP_RENDER_V1                  (1ul << 11)
#define CASTLE_RUNTIME_CAP_GAME_PHASE_SCHEDULE_V1     (1ul << 12)
#define CASTLE_RUNTIME_CAP_LOG_V1                     (1ul << 13)
#define CASTLE_RUNTIME_CAP_INPUT_V1                   (1ul << 14)
#define CASTLE_RUNTIME_CAP_GAME_STATE_V1              (1ul << 15)
#define CASTLE_RUNTIME_CAP_SAVE_V1                    (1ul << 16)

/* Bootstrap 触发来源。数字只用于诊断和测试，不能改变最终激活语义。 */
#define CASTLE_BOOTSTRAP_TRIGGER_ENTRY_GATE     1u
#define CASTLE_BOOTSTRAP_TRIGGER_INITIALIZE_ASI 2u
#define CASTLE_BOOTSTRAP_TRIGGER_TEST_HOST      3u
#define CASTLE_BOOTSTRAP_TRIGGER_LOADER_READY   4u

/* Runtime/Standalone 模式编号。 */
#define CASTLE_BOOTSTRAP_MODE_STANDALONE 1u
#define CASTLE_BOOTSTRAP_MODE_INTEGRATED 2u
#define CASTLE_BOOTSTRAP_MODE_FAULT       3u

/* 插件生命周期状态。调用方只能读取，不能自行写入 Runtime 的状态表。 */
#define CASTLE_PLUGIN_UNKNOWN       0u
#define CASTLE_PLUGIN_REGISTERED    1u
#define CASTLE_PLUGIN_PREFLIGHTING  2u
#define CASTLE_PLUGIN_ACTIVATING    3u
#define CASTLE_PLUGIN_ACTIVE        4u
#define CASTLE_PLUGIN_DEGRADED      5u
#define CASTLE_PLUGIN_FAILED        6u
#define CASTLE_PLUGIN_PROCESS_EXIT  7u

/* 前置声明打破 Runtime 根头与插件描述头之间的循环包含。 */
struct CastlePluginDescriptorV1;

/*
 * Runtime 信息快照。
 * 两个字符串视图指向 Runtime 持有的只读内存，进程结束前保持稳定。
 */
typedef struct CastleRuntimeInfoV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 info_version;
    CastleU32 flags;

    CastleU32 runtime_version_major;
    CastleU32 runtime_version_minor;
    CastleU32 runtime_version_patch;
    CastleU32 abi_version;

    CastleU32 capability_flags_low;
    CastleU32 capability_flags_high;
    CastleU32 process_id;
    CastleModule runtime_module;
    CastleModule game_module;
    CastleStringView game_build_id;
    CastleStringView runtime_path;
    CastleU32 diagnostic_generation;
} CastleRuntimeInfoV1;

/*
 * 接口查询请求。
 * required_capabilities 让调用方一次说明必需能力，避免拿到接口后才在中途发现函数缺失。
 */
typedef struct CastleInterfaceQueryV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 request_version;
    CastleU32 flags;
    CastleStringView interface_id;
    CastleU32 requested_version;
    CastleU32 minimum_struct_size;
    CastleU32 required_capabilities_low;
    CastleU32 required_capabilities_high;
} CastleInterfaceQueryV1;

/*
 * 接口查询结果永远返回 Runtime 门面，不返回具体后端插件的私有结构。
 * provider_generation 在后端切换时增加，消费者可用它识别过期快照。
 */
typedef struct CastleInterfaceResultV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 result_version;
    CastleU32 flags;
    const void* api_pointer;
    CastleU32 actual_version;
    CastleU32 actual_struct_size;
    CastleU32 capabilities_low;
    CastleU32 capabilities_high;
    CastleU32 provider_generation;
} CastleInterfaceResultV1;

/* 插件状态快照。active_claim_count/provider_count 只用于诊断，不授予调用方修改权限。 */
typedef struct CastlePluginStateV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 state_version;
    CastleU32 flags;
    CastlePluginHandle plugin_handle;
    CastleU32 state;
    CastleU32 state_flags;
    CastleResult last_result;
    CastleU32 plugin_generation;
    CastleU32 active_claim_count;
    CastleU32 provider_count;
} CastlePluginStateV1;

/*
 * 诊断复制缓冲区由调用方提供。
 * 容量不足时 Runtime 填 required_capacity，不会跨 DLL malloc 一块内存让调用方释放。
 */
typedef struct CastleDiagnosticBufferV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 buffer_version;
    CastleU32 flags;
    char* utf8_buffer;
    CastleU32 buffer_capacity;
    CastleU32 bytes_written;
    CastleU32 first_generation;
    CastleU32 last_generation;
    CastleU32 required_capacity;
} CastleDiagnosticBufferV1;

/* Bootstrap 请求由 Entry Gate、InitializeASI 或测试宿主创建。reserved_zero 必须写 0。 */
typedef struct CastleBootstrapRequestV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 request_version;
    CastleU32 flags;
    CastleU32 trigger_kind;
    CastleModule trigger_module;
    CastleAddress gate_entry;
    CastleU32 reserved_zero;
} CastleBootstrapRequestV1;

/* Bootstrap 结果汇总本次发现和激活数量，便于 Loader/测试器输出明确结论。 */
typedef struct CastleBootstrapResultV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 result_version;
    CastleU32 flags;
    CastleU32 mode;
    CastleU32 total_plugins;
    CastleU32 active_plugins;
    CastleU32 degraded_plugins;
    CastleU32 failed_plugins;
    CastleU32 bootstrap_generation;
} CastleBootstrapResultV1;

/* 根 API 函数类型。所有函数统一使用 __cdecl，避免 x86 调用方和被调用方清栈规则不一致。 */
typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetRuntimeInfoFn)(
    CastleRuntimeInfoV1* out_info);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleBootstrapLoadedPluginsFn)(
    const CastleBootstrapRequestV1* request,
    CastleBootstrapResultV1* out_result);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleRegisterPluginFn)(
    const struct CastlePluginDescriptorV1* descriptor,
    CastlePluginHandle* out_handle);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleGetPluginStateFn)(
    CastlePluginHandle handle,
    CastlePluginStateV1* out_state);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleQueryInterfaceFn)(
    const CastleInterfaceQueryV1* query,
    CastleInterfaceResultV1* out_result);

typedef CastleU32 (CASTLE_RUNTIME_CALL *CastleGetDiagnosticGenerationFn)(void);

typedef CastleResult (CASTLE_RUNTIME_CALL *CastleCopyDiagnosticsFn)(
    CastleU32 after_generation,
    CastleDiagnosticBufferV1* output);

/*
 * Runtime 根函数表。
 * 已发布字段只允许在尾部追加；消费者访问任何字段前都要检查 struct_size 是否覆盖该字段。
 */
typedef struct CastleRuntimeApiV1 {
    CastleU32 magic;
    CastleU32 struct_size;
    CastleU32 abi_version;
    CastleU32 runtime_version_major;
    CastleU32 runtime_version_minor;
    CastleU32 runtime_version_patch;
    CastleU32 capability_flags_low;
    CastleU32 capability_flags_high;

    CastleGetRuntimeInfoFn GetRuntimeInfo;
    CastleBootstrapLoadedPluginsFn BootstrapLoadedPlugins;
    CastleRegisterPluginFn RegisterPlugin;
    CastleGetPluginStateFn GetPluginState;
    CastleQueryInterfaceFn QueryInterface;
    CastleGetDiagnosticGenerationFn GetDiagnosticGeneration;
    CastleCopyDiagnosticsFn CopyDiagnostics;
} CastleRuntimeApiV1;

/* 唯一稳定根导出的函数类型。请求未知 ABI 时，真实导出返回空指针。 */
typedef const CastleRuntimeApiV1* (CASTLE_RUNTIME_CALL *CastleRuntimeGetApiFn)(
    CastleU32 requested_abi_version);

/* 文档冻结的 x86 结构大小，供工具和调用方做最小边界检查。 */
#define CASTLE_SIZEOF_STRING_VIEW_V1          8u
#define CASTLE_SIZEOF_RUNTIME_INFO_V1         72u
#define CASTLE_SIZEOF_INTERFACE_QUERY_V1      40u
#define CASTLE_SIZEOF_INTERFACE_RESULT_V1     40u
#define CASTLE_SIZEOF_PLUGIN_STATE_V1         44u
#define CASTLE_SIZEOF_DIAGNOSTIC_BUFFER_V1    40u
#define CASTLE_SIZEOF_BOOTSTRAP_REQUEST_V1    32u
#define CASTLE_SIZEOF_BOOTSTRAP_RESULT_V1     40u
#define CASTLE_SIZEOF_RUNTIME_API_V1          60u

#pragma pack(pop)

#endif /* CASTLE_RUNTIME_API_H */
