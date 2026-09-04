/*
 * abi_layout_test.c
 *
 * 这个文件不运行游戏，也不链接 Runtime。它只让不同编译器真实计算公共 ABI 的大小和偏移。
 * 如果任何断言失败，编译器会把数组长度变成负数并立即停止构建。
 *
 * 同一文件会分别按 C 和 C++ 编译，用来证明公共头没有偷偷依赖某一种语言模式。
 */

#include <stddef.h>

#include "CastleRuntime_API.h"
#include "CastlePlugin_API.h"
#include "CastleRuntime_Client.h"
#include "CastleHook_API.h"
#include "CastleDisplay_API.h"
#include "CastlePath_API.h"
#include "CastleSchedule_API.h"
#include "CastleWindow_API.h"
#include "CastleRender_API.h"
#include "CastleLog_API.h"
#include "CastleClock_API.h"
#include "CastleInput_API.h"
#include "CastleGameState_API.h"
#include "CastleSave_API.h"

/* 使用唯一 typedef 名制造编译期断言，不需要 C11 _Static_assert 或 C++ static_assert。 */
#define CASTLE_ABI_ASSERT(name, expression) \
    typedef char castle_abi_assert_##name[(expression) ? 1 : -1]

/* 先确认整个 SDK 最基础的“x86 指针=4字节”前提。 */
CASTLE_ABI_ASSERT(pointer_is_4, sizeof(void*) == 4u);
CASTLE_ABI_ASSERT(u8_is_1, sizeof(CastleU8) == 1u);
CASTLE_ABI_ASSERT(u16_is_2, sizeof(CastleU16) == 2u);
CASTLE_ABI_ASSERT(u32_is_4, sizeof(CastleU32) == 4u);
CASTLE_ABI_ASSERT(s32_is_4, sizeof(CastleS32) == 4u);

/* Runtime 根 ABI 大小。 */
CASTLE_ABI_ASSERT(string_view_size,
    sizeof(CastleStringView) == CASTLE_SIZEOF_STRING_VIEW_V1);
CASTLE_ABI_ASSERT(runtime_info_size,
    sizeof(CastleRuntimeInfoV1) == CASTLE_SIZEOF_RUNTIME_INFO_V1);
CASTLE_ABI_ASSERT(interface_query_size,
    sizeof(CastleInterfaceQueryV1) == CASTLE_SIZEOF_INTERFACE_QUERY_V1);
CASTLE_ABI_ASSERT(interface_result_size,
    sizeof(CastleInterfaceResultV1) == CASTLE_SIZEOF_INTERFACE_RESULT_V1);
CASTLE_ABI_ASSERT(plugin_state_size,
    sizeof(CastlePluginStateV1) == CASTLE_SIZEOF_PLUGIN_STATE_V1);
CASTLE_ABI_ASSERT(diagnostic_buffer_size,
    sizeof(CastleDiagnosticBufferV1) == CASTLE_SIZEOF_DIAGNOSTIC_BUFFER_V1);
CASTLE_ABI_ASSERT(bootstrap_request_size,
    sizeof(CastleBootstrapRequestV1) == CASTLE_SIZEOF_BOOTSTRAP_REQUEST_V1);
CASTLE_ABI_ASSERT(bootstrap_result_size,
    sizeof(CastleBootstrapResultV1) == CASTLE_SIZEOF_BOOTSTRAP_RESULT_V1);
CASTLE_ABI_ASSERT(runtime_api_size,
    sizeof(CastleRuntimeApiV1) == CASTLE_SIZEOF_RUNTIME_API_V1);

/* 根函数顺序。偏移变化意味着旧插件会调用错误函数，因此必须立刻失败。 */
CASTLE_ABI_ASSERT(runtime_api_get_info_offset,
    offsetof(CastleRuntimeApiV1, GetRuntimeInfo) == 32u);
CASTLE_ABI_ASSERT(runtime_api_bootstrap_offset,
    offsetof(CastleRuntimeApiV1, BootstrapLoadedPlugins) == 36u);
CASTLE_ABI_ASSERT(runtime_api_register_offset,
    offsetof(CastleRuntimeApiV1, RegisterPlugin) == 40u);
CASTLE_ABI_ASSERT(runtime_api_copy_diag_offset,
    offsetof(CastleRuntimeApiV1, CopyDiagnostics) == 56u);

/* 插件描述、Client 配置和查询导出。 */
CASTLE_ABI_ASSERT(plugin_descriptor_size,
    sizeof(CastlePluginDescriptorV1) == CASTLE_SIZEOF_PLUGIN_DESCRIPTOR_V1);
CASTLE_ABI_ASSERT(client_config_size,
    sizeof(CastleRuntimeClientConfigV1) == CASTLE_SIZEOF_CLIENT_CONFIG_V1);
CASTLE_ABI_ASSERT(plugin_export_size,
    sizeof(CastlePluginExportV1) == CASTLE_SIZEOF_PLUGIN_EXPORT_V1);
CASTLE_ABI_ASSERT(plugin_descriptor_module_offset,
    offsetof(CastlePluginDescriptorV1, module) == 16u);
CASTLE_ABI_ASSERT(plugin_descriptor_id_offset,
    offsetof(CastlePluginDescriptorV1, plugin_id) == 20u);
CASTLE_ABI_ASSERT(client_config_integrated_offset,
    offsetof(CastleRuntimeClientConfigV1, integrated_initialize) == 16u);
CASTLE_ABI_ASSERT(client_config_context_offset,
    offsetof(CastleRuntimeClientConfigV1, user_context) == 32u);
CASTLE_ABI_ASSERT(plugin_export_descriptor_offset,
    offsetof(CastlePluginExportV1, plugin_descriptor) == 16u);
CASTLE_ABI_ASSERT(plugin_export_gate_offset,
    offsetof(CastlePluginExportV1, entry_gate_thunk) == 24u);
CASTLE_ABI_ASSERT(plugin_export_bootstrap_offset,
    offsetof(CastlePluginExportV1, client_bootstrap) == 28u);

/* Hook/补丁 ABI。 */
CASTLE_ABI_ASSERT(target_address_size,
    sizeof(CastleTargetAddressV1) == CASTLE_SIZEOF_TARGET_ADDRESS_V1);
CASTLE_ABI_ASSERT(exclusive_patch_size,
    sizeof(CastleExclusivePatchClaimV1) == CASTLE_SIZEOF_EXCLUSIVE_PATCH_V1);
CASTLE_ABI_ASSERT(state_patch_size,
    sizeof(CastleStatePatchClaimV1) == CASTLE_SIZEOF_STATE_PATCH_V1);
CASTLE_ABI_ASSERT(chain_hook_size,
    sizeof(CastleChainHookClaimV1) == CASTLE_SIZEOF_CHAIN_HOOK_V1);
CASTLE_ABI_ASSERT(hook_binding_size,
    sizeof(CastleHookBindingV1) == CASTLE_SIZEOF_HOOK_BINDING_V1);
CASTLE_ABI_ASSERT(named_resource_size,
    sizeof(CastleNamedResourceClaimV1) == CASTLE_SIZEOF_NAMED_RESOURCE_V1);
CASTLE_ABI_ASSERT(order_constraint_size,
    sizeof(CastleOrderConstraintV1) == CASTLE_SIZEOF_ORDER_CONSTRAINT_V1);
CASTLE_ABI_ASSERT(claim_result_size,
    sizeof(CastleClaimResultV1) == CASTLE_SIZEOF_CLAIM_RESULT_V1);
CASTLE_ABI_ASSERT(hook_api_size,
    sizeof(CastleHookApiV1) == CASTLE_SIZEOF_HOOK_API_V1);
CASTLE_ABI_ASSERT(exclusive_label_offset,
    offsetof(CastleExclusivePatchClaimV1, label) == 44u);
CASTLE_ABI_ASSERT(state_desired_offset,
    offsetof(CastleStatePatchClaimV1, desired_state) == 44u);
CASTLE_ABI_ASSERT(chain_signature_offset,
    offsetof(CastleChainHookClaimV1, signature_id) == 40u);
CASTLE_ABI_ASSERT(chain_label_offset,
    offsetof(CastleChainHookClaimV1, label) == 56u);
CASTLE_ABI_ASSERT(binding_next_offset,
    offsetof(CastleHookBindingV1, next_slot) == 20u);
CASTLE_ABI_ASSERT(hook_api_describe_offset,
    offsetof(CastleHookApiV1, DescribeResource) == 64u);

/* Display/投影 ABI。 */
CASTLE_ABI_ASSERT(display_geometry_size,
    sizeof(CastleDisplayGeometryV1) == CASTLE_SIZEOF_DISPLAY_GEOMETRY_V1);
CASTLE_ABI_ASSERT(world_to_screen_size,
    sizeof(CastleWorldToScreenRequestV1) == CASTLE_SIZEOF_WORLD_TO_SCREEN_V1);
CASTLE_ABI_ASSERT(screen_projection_size,
    sizeof(CastleScreenProjectionV1) == CASTLE_SIZEOF_SCREEN_PROJECTION_V1);
CASTLE_ABI_ASSERT(screen_to_world_size,
    sizeof(CastleScreenToWorldRequestV1) == CASTLE_SIZEOF_SCREEN_TO_WORLD_V1);
CASTLE_ABI_ASSERT(world_projection_size,
    sizeof(CastleWorldProjectionV1) == CASTLE_SIZEOF_WORLD_PROJECTION_V1);
CASTLE_ABI_ASSERT(display_state_size,
    sizeof(CastleDisplayStateV1) == CASTLE_SIZEOF_DISPLAY_STATE_V1);
CASTLE_ABI_ASSERT(display_provider_info_size,
    sizeof(CastleDisplayProviderInfoV1) == CASTLE_SIZEOF_DISPLAY_PROVIDER_INFO_V1);
CASTLE_ABI_ASSERT(display_provider_size,
    sizeof(CastleDisplayProviderV1) == CASTLE_SIZEOF_DISPLAY_PROVIDER_V1);
CASTLE_ABI_ASSERT(display_api_size,
    sizeof(CastleDisplayApiV1) == CASTLE_SIZEOF_DISPLAY_API_V1);
CASTLE_ABI_ASSERT(display_effective_camera_offset,
    offsetof(CastleDisplayGeometryV1, effective_camera_x) == 56u);
CASTLE_ABI_ASSERT(display_mode_offset,
    offsetof(CastleDisplayGeometryV1, display_mode) == 80u);
CASTLE_ABI_ASSERT(world_request_x_offset,
    offsetof(CastleWorldToScreenRequestV1, world_x) == 20u);
CASTLE_ABI_ASSERT(screen_result_x_offset,
    offsetof(CastleScreenProjectionV1, screen_x) == 20u);
CASTLE_ABI_ASSERT(display_api_register_offset,
    offsetof(CastleDisplayApiV1, RegisterDisplayProvider) == 36u);
CASTLE_ABI_ASSERT(display_api_provider_state_offset,
    offsetof(CastleDisplayApiV1, GetDisplayProviderState) == 48u);

/* Path ABI 同时固定 UTF-8/UTF-16 视图和完全对称的函数顺序。 */
CASTLE_ABI_ASSERT(wide_string_view_size,
    sizeof(CastleWideStringView) == CASTLE_SIZEOF_WIDE_STRING_VIEW_V1);
CASTLE_ABI_ASSERT(path_info_size,
    sizeof(CastlePathInfoV1) == CASTLE_SIZEOF_PATH_INFO_V1);
CASTLE_ABI_ASSERT(path_api_size,
    sizeof(CastlePathApiV1) == CASTLE_SIZEOF_PATH_API_V1);
CASTLE_ABI_ASSERT(path_info_asi_wide_offset,
    offsetof(CastlePathInfoV1, asi_directory_wide) == 28u);
CASTLE_ABI_ASSERT(path_info_runtime_utf8_offset,
    offsetof(CastlePathInfoV1, runtime_path_utf8) == 36u);
CASTLE_ABI_ASSERT(path_api_get_info_offset,
    offsetof(CastlePathApiV1, GetPathInfo) == 16u);
CASTLE_ABI_ASSERT(path_api_build_utf8_offset,
    offsetof(CastlePathApiV1, BuildPluginRelativePathUtf8) == 28u);
CASTLE_ABI_ASSERT(path_api_equals_wide_offset,
    offsetof(CastlePathApiV1, PathsEqualWide) == 48u);

/* Schedule ABI 固定后台任务、统计和未启用游戏阶段的查询形状。 */
CASTLE_ABI_ASSERT(scheduled_task_size,
    sizeof(CastleScheduledTaskV1) == CASTLE_SIZEOF_SCHEDULED_TASK_V1);
CASTLE_ABI_ASSERT(schedule_stats_size,
    sizeof(CastleScheduleTaskStatsV1) == CASTLE_SIZEOF_SCHEDULE_STATS_V1);
CASTLE_ABI_ASSERT(game_phase_state_size,
    sizeof(CastleGamePhaseStateV1) == CASTLE_SIZEOF_GAME_PHASE_STATE_V1);
CASTLE_ABI_ASSERT(schedule_api_size,
    sizeof(CastleScheduleApiV1) == CASTLE_SIZEOF_SCHEDULE_API_V1);
CASTLE_ABI_ASSERT(scheduled_task_callback_offset,
    offsetof(CastleScheduledTaskV1, callback) == 32u);
CASTLE_ABI_ASSERT(schedule_stats_run_count_offset,
    offsetof(CastleScheduleTaskStatsV1, run_count) == 40u);
CASTLE_ABI_ASSERT(schedule_api_post_offset,
    offsetof(CastleScheduleApiV1, PostBackgroundTask) == 32u);
CASTLE_ABI_ASSERT(schedule_api_game_state_offset,
    offsetof(CastleScheduleApiV1, GetGamePhaseState) == 44u);

/* Window ABI 固定消息、消费结果、客户端登记与注销位置。 */
CASTLE_ABI_ASSERT(window_message_size,
    sizeof(CastleWindowMessageV1) == CASTLE_SIZEOF_WINDOW_MESSAGE_V1);
CASTLE_ABI_ASSERT(window_decision_size,
    sizeof(CastleWindowFilterDecisionV1) == CASTLE_SIZEOF_WINDOW_DECISION_V1);
CASTLE_ABI_ASSERT(window_client_size,
    sizeof(CastleWindowClientV1) == CASTLE_SIZEOF_WINDOW_CLIENT_V1);
CASTLE_ABI_ASSERT(window_state_size,
    sizeof(CastleWindowStateV1) == CASTLE_SIZEOF_WINDOW_STATE_V1);
CASTLE_ABI_ASSERT(window_api_size,
    sizeof(CastleWindowApiV1) == CASTLE_SIZEOF_WINDOW_API_V1);
CASTLE_ABI_ASSERT(window_client_observer_offset,
    offsetof(CastleWindowClientV1, observer) == 24u);
CASTLE_ABI_ASSERT(window_client_context_offset,
    offsetof(CastleWindowClientV1, user_context) == 32u);
CASTLE_ABI_ASSERT(window_state_observer_count_offset,
    offsetof(CastleWindowStateV1, observer_count) == 32u);
CASTLE_ABI_ASSERT(window_api_unregister_offset,
    offsetof(CastleWindowApiV1, UnregisterWindowClient) == 32u);

/* Render ABI 固定调用上下文、Display 绑定、额外帧租约和 Provider 入口。 */
CASTLE_ABI_ASSERT(render_call_size,
    sizeof(CastleRenderCallV1) == CASTLE_SIZEOF_RENDER_CALL_V1);
CASTLE_ABI_ASSERT(render_state_size,
    sizeof(CastleRenderStateV1) == CASTLE_SIZEOF_RENDER_STATE_V1);
CASTLE_ABI_ASSERT(render_provider_size,
    sizeof(CastleRenderProviderV1) == CASTLE_SIZEOF_RENDER_PROVIDER_V1);
CASTLE_ABI_ASSERT(render_api_size,
    sizeof(CastleRenderApiV1) == CASTLE_SIZEOF_RENDER_API_V1);
CASTLE_ABI_ASSERT(render_call_context_offset,
    offsetof(CastleRenderCallV1, render_context) == 16u);
CASTLE_ABI_ASSERT(render_call_lease_offset,
    offsetof(CastleRenderCallV1, extra_frame_lease) == 24u);
CASTLE_ABI_ASSERT(render_state_display_generation_offset,
    offsetof(CastleRenderStateV1, display_provider_generation) == 32u);
CASTLE_ABI_ASSERT(render_api_begin_extra_offset,
    offsetof(CastleRenderApiV1, BeginExtraWorldFrame) == 28u);
CASTLE_ABI_ASSERT(render_api_provider_state_offset,
    offsetof(CastleRenderApiV1, GetRenderProviderState) == 44u);

/* Log ABI 只传 UTF-8 视图和插件句柄，不跨 DLL 共享 FILE/HANDLE。 */
CASTLE_ABI_ASSERT(log_record_size,
    sizeof(CastleLogRecordV1) == CASTLE_SIZEOF_LOG_RECORD_V1);
CASTLE_ABI_ASSERT(log_api_size,
    sizeof(CastleLogApiV1) == CASTLE_SIZEOF_LOG_API_V1);
CASTLE_ABI_ASSERT(log_record_message_offset,
    offsetof(CastleLogRecordV1, message) == 16u);
CASTLE_ABI_ASSERT(log_api_write_offset,
    offsetof(CastleLogApiV1, WritePluginLine) == 16u);
CASTLE_ABI_ASSERT(log_api_directory_offset,
    offsetof(CastleLogApiV1, GetLogDirectoryUtf8) == 28u);

/* Clock ABI 只暴露数值和 Runtime 租约，不把 WinMM 类型泄漏给插件。 */
CASTLE_ABI_ASSERT(clock_state_size,
    sizeof(CastleClockStateV1) == CASTLE_SIZEOF_CLOCK_STATE_V1);
CASTLE_ABI_ASSERT(clock_api_size,
    sizeof(CastleClockApiV1) == CASTLE_SIZEOF_CLOCK_API_V1);
CASTLE_ABI_ASSERT(clock_state_lease_count_offset,
    offsetof(CastleClockStateV1, active_lease_count) == 24u);
CASTLE_ABI_ASSERT(clock_api_acquire_offset,
    offsetof(CastleClockApiV1, AcquireTimerResolution) == 20u);

/* Input ABI 的快照是纯 32 位标量，允许 Controller worker 无锁发布稳定副本。 */
CASTLE_ABI_ASSERT(input_snapshot_size,
    sizeof(CastleInputSnapshotV1) == CASTLE_SIZEOF_INPUT_SNAPSHOT_V1);
CASTLE_ABI_ASSERT(input_focus_request_size,
    sizeof(CastleInputFocusRequestV1) == CASTLE_SIZEOF_INPUT_FOCUS_REQUEST_V1);
CASTLE_ABI_ASSERT(input_focus_state_size,
    sizeof(CastleInputFocusStateV1) == CASTLE_SIZEOF_INPUT_FOCUS_STATE_V1);
CASTLE_ABI_ASSERT(input_provider_size,
    sizeof(CastleInputProviderV1) == CASTLE_SIZEOF_INPUT_PROVIDER_V1);
CASTLE_ABI_ASSERT(input_api_size,
    sizeof(CastleInputApiV1) == CASTLE_SIZEOF_INPUT_API_V1);
CASTLE_ABI_ASSERT(input_snapshot_axes_offset,
    offsetof(CastleInputSnapshotV1, axes) == 64u);
CASTLE_ABI_ASSERT(input_api_register_offset,
    offsetof(CastleInputApiV1, RegisterInputProvider) == 32u);

/* GameState 快照只保存值和 x86 地址，不把 Runtime 私有对象指针跨 ABI 借出。 */
CASTLE_ABI_ASSERT(game_state_snapshot_size,
    sizeof(CastleGameStateSnapshotV1) == CASTLE_SIZEOF_GAME_STATE_SNAPSHOT_V1);
CASTLE_ABI_ASSERT(game_mutation_request_size,
    sizeof(CastleGameMutationRequestV1) == CASTLE_SIZEOF_GAME_MUTATION_REQUEST_V1);
CASTLE_ABI_ASSERT(game_mutation_state_size,
    sizeof(CastleGameMutationStateV1) == CASTLE_SIZEOF_GAME_MUTATION_STATE_V1);
CASTLE_ABI_ASSERT(game_state_api_size,
    sizeof(CastleGameStateApiV1) == CASTLE_SIZEOF_GAME_STATE_API_V1);
CASTLE_ABI_ASSERT(game_snapshot_camera_offset,
    offsetof(CastleGameStateSnapshotV1, camera_x) == 36u);
CASTLE_ABI_ASSERT(game_snapshot_ui_offset,
    offsetof(CastleGameStateSnapshotV1, battle_ui) == 104u);
CASTLE_ABI_ASSERT(game_state_api_acquire_offset,
    offsetof(CastleGameStateApiV1, AcquireMutation) == 20u);

/* Save ABI 只接收槽位策略；原版 SaveAction 指针永远留在 Runtime 内部。 */
CASTLE_ABI_ASSERT(save_policy_size,
    sizeof(CastleManualSavePolicyV1) == CASTLE_SIZEOF_MANUAL_SAVE_POLICY_V1);
CASTLE_ABI_ASSERT(save_ui_state_size,
    sizeof(CastleSaveUiStateV1) == CASTLE_SIZEOF_SAVE_UI_STATE_V1);
CASTLE_ABI_ASSERT(save_api_size,
    sizeof(CastleSaveApiV1) == CASTLE_SIZEOF_SAVE_API_V1);
CASTLE_ABI_ASSERT(save_policy_label_offset,
    offsetof(CastleManualSavePolicyV1, label) == 28u);
CASTLE_ABI_ASSERT(save_api_ui_state_offset,
    offsetof(CastleSaveApiV1, GetSaveUiState) == 28u);

/*
 * 生成一个外部函数，防止极端编译器把整个测试翻译单元当成“完全空文件”特殊处理。
 * 返回值本身没有业务意义，测试成功的标准是本文件能通过编译。
 */
int castle_runtime_abi_layout_anchor(void) {
    return (int)(CASTLE_RUNTIME_ABI_V1 + CASTLE_HOOK_API_VERSION_1 +
                 CASTLE_DISPLAY_API_VERSION_1 + CASTLE_PATH_API_VERSION_1 +
                 CASTLE_SCHEDULE_API_VERSION_1 + CASTLE_WINDOW_API_VERSION_1 +
                 CASTLE_RENDER_API_VERSION_1 + CASTLE_LOG_API_VERSION_1 +
                 CASTLE_CLOCK_API_VERSION_1);
}
