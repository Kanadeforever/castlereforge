#ifndef CASTLE_GAME_AUDIT_H
#define CASTLE_GAME_AUDIT_H

#include "platform.h"

/*
 * game_audit.h —— 原版游戏运行审计层的公共接口。
 *
 * 这个模块和 mod_loader.c 的职责完全不同：
 *   - modloader.log 只回答“Loader / Mod 做了什么”；
 *   - game.log 只回答“原版 RPG.exe 在运行时做了什么”。
 *
 * 因此其它模块如果只是想报告自己的 Mod 状态，不能调用这里；
 * 只有在“我们正在旁路观察原版游戏”时，才把事实送进 GameAudit。
 */

/*
 * 在 Locale 文件名实体自检通过、但 ASI 尚未加载时启动审计。
 * 这样 game.log 从 RPG.exe 第一条真正业务代码之前就准备好，同时不会把 Pre-Loader 自己的 I/O 混进来。
 */
int GameAudit_Initialize(void);

/*
 * 在正常进程卸载路径把还没到 5 秒窗口边界的 FDebug 聚合尾巴写完，并关闭 game.log。
 * 这不是必须依赖的“崩溃保存机制”：TerminateProcess/严重异常强杀未必会执行 DLL detach，
 * 所以真正关键的状态断点和异常记录仍然在发生时立即刷盘；这里主要保证正常退出时统计不缺最后一小段。
 */
void GameAudit_Shutdown(void);

/*
 * 给 RPG.exe 与原版静态依赖模块安装除 CreateFile/GetFileAttributes 之外的 I/O IAT Hook。
 * CreateFile/GetFileAttributes 由 OverrideLoader 统一接管，避免同一 IAT 槽被两套 Hook 争抢。
 */
UINT GameAudit_PatchOriginalIoModules(void);

/*
 * OverrideLoader 已经拥有 CreateFileA/W 与 GetFileAttributesA/W 的 IAT 入口。
 * 为避免两套 Hook 互相覆盖，OverrideLoader 在完成真实 API 调用后，把结果通过下面四个回调送给 GameAudit。
 * caller 是 Hook 刚进入时抓到的返回地址；GameAudit 会据此过滤 ASI/Mod，只接受注册过的原版模块。
 */
void GameAudit_RecordCreateFileA(LPVOID caller, LPCSTR path, DWORD access, DWORD share, DWORD creation,
                                 DWORD attrs, HANDLE result, DWORD error, int redirected_by_override);
void GameAudit_RecordCreateFileW(LPVOID caller, LPCWSTR path, DWORD access, DWORD share, DWORD creation,
                                 DWORD attrs, HANDLE result, DWORD error, int redirected_by_override);
void GameAudit_RecordGetFileAttributesA(LPVOID caller, LPCSTR path, DWORD result, DWORD error, int redirected_by_override);
void GameAudit_RecordGetFileAttributesW(LPVOID caller, LPCWSTR path, DWORD result, DWORD error, int redirected_by_override);

/*
 * 供 modloader.log 写一个“状态 Hook 安装了多少个”的简短摘要。
 * 详细的每个断点是否成功、为什么跳过，全写在 game.log，不污染 Loader 主日志。
 */
UINT GameAudit_StateHookCount(void);

#endif /* CASTLE_GAME_AUDIT_H */
