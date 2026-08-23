#ifndef CASTLE_NATIVE_LOCALE_H
#define CASTLE_NATIVE_LOCALE_H

#include "platform.h"

/*
 * Native Locale 层负责“公开 Win32 API 之下”的区域语义。
 *
 * v0.2.5 已经实机证明：真实 GetACP/GetOEMCP 可以达到 950/950，但游戏仍不等价于 Locale Emulator 的
 * “中文（台湾）+ 高级选项”运行环境。原因是 Windows 还有 ntdll 默认 Locale/UI Language、线程 TEB Locale、
 * 原生时区查询，以及 NtQueryValueKey 语言注册表读取等入口。
 *
 * 因此本模块对齐用户当前配置真正需要的原生语义：
 *   1. 默认 Locale / UI Language / Install UI Language = 0x0404；
 *   2. 当前线程 TEB.CurrentLocale = 0x0404；
 *   3. SystemCurrentTimeZoneInformation = Taipei UTC+8；
 *   4. NtInitializeNlsFiles 返回的默认 LocaleID = 0x0404；
 *   5. NtQueryValueKey 对 8 个指定语言注册表值返回 zh-TW/CP950 数据；
 *   6. RtlCustomCPToUnicodeN 遇到 KernelBase/系统 DLL 早期缓存的宿主 CPTABLEINFO 时，
 *      用 PEB 当前 c_950.nls 重新初始化该表，再让 Windows 原函数继续转换。
 *
 * 第 6 项是 v0.2.7 实机硬验收发现的缺口：GetACP=950 与 MultiByteToWideChar(CP_ACP) 已经正确，
 * 但 CreateFileA 仍然 ERROR_FILE_NOT_FOUND，说明真实 ANSI 文件 API 还在使用更早缓存的旧代码页表。
 * 它故意和 locale_layer.c 分开：locale_layer.c 管公开 KERNEL32/SHLWAPI API，本文件管 ntdll 原生层。
 */

/*
 * 第一次调用：校正当前线程 Locale、解析 ntdll、严格验证 syscall stub，并安装 Locale Emulator 等价的
 * RtlCustomCPToUnicodeN CPTABLEINFO 修复 Hook，然后执行运行态自检。
 * 任一关键 Hook 的机器码布局不符合预期都会 fail-closed，绝不猜测指令长度继续写内存。
 */
int NativeLocale_Initialize(void);

/*
 * 返回本次进程里“陈旧宿主 CPTABLEINFO 被重建为 CP950”的累计次数。
 * 这个值只用于日志诊断，不作为业务逻辑开关；最终是否真的可用仍由 CreateFileA 实体文件自检决定。
 */
LONG NativeLocale_GetCustomCpRepairCount(void);

/*
 * EntryPoint 前第二阶段调用。不会重新安装已存在的 Hook，只再次把当前线程 Locale 固定为 0x0404，
 * 并重跑默认 Locale/UI/时区自检，防止中间的 KERNEL32/USER32 初始化把线程状态改回宿主系统值。
 */
int NativeLocale_Refresh(void);

#endif
