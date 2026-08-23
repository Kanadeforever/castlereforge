#ifndef CASTLE_WIDESCREEN_RUNTIME_H
#define CASTLE_WIDESCREEN_RUNTIME_H

#include "platform.h"

/* 启动最小 Win32 API、日志；Shutdown 只负责关闭日志句柄。 */
int Runtime_Initialize(HMODULE self_module);
void Runtime_Shutdown(void);

/* 写一行 UTF-8 日志；Hex 版本把 32 位数字按 0xXXXXXXXX 输出，方便和反汇编地址直接核对。 */
void Runtime_Log(const char* text);
void Runtime_LogHex(const char* prefix, u32 value);

/* 以十进制输出 32 位无符号整数；INI 的毫秒值用十进制日志更容易直接阅读。 */
void Runtime_LogU32(const char* prefix, u32 value);

/*
 * 读取和 ASI 同目录、固定名为 Castle_Widescreen.ini 的一个整数。
 * - 缺少文件、节或键：返回 default_value；
 * - 数值超出 [min_value, max_value]：也回退 default_value；
 * - 这样用户把 INI 写坏时只会失去自定义设置，不会影响 Hook 安全。
 */
u32 Runtime_ReadPluginIniU32(
    const char* section, const char* key,
    u32 default_value, u32 min_value, u32 max_value);

/* 返回当前 Windows 毫秒计数；电影式推入/退出用它实现真正的“按毫秒配置”。 */
u32 Runtime_GetTickCountMs(void);

/*
 * 修改一个精确 E8 rel32 CALL。
 * 函数会先再次计算当前 CALL 的真实目标，只有它仍等于 expected_target 才写 5 字节。
 */
int Runtime_PatchCall(u32 call_address, u32 expected_target, const void* replacement, const char* label);

/*
 * 把已经由本插件改写的 E8 CALL 恢复到原目标。
 * 只有当前 CALL 仍然指向 expected_current_target 时才会恢复，避免误覆盖别的插件在我们之后做的修改。
 */
int Runtime_RestoreCall(u32 call_address, u32 expected_current_target, u32 restore_target);

/* 修改 32 位函数指针槽（本项目用于 Bink IAT），并把原指针交给调用者保存。 */
int Runtime_PatchPointer(u32 slot_address, const void* replacement, void** old_value, const char* label);

/* 对目标 EXE 的 Display/Camera/绘制队列/Event 协议做总预检；失败时 DllMain 不安装任何业务 Hook。 */
int Runtime_ExactBuildProtocolOk(void);

/* 用 VirtualAlloc 分配整段可读写内存；本 v0.11 用它建立当前16:9/21:9合成帧、对应Present staging、队列快照以及低分辨率模糊工作区。 */
void* Runtime_Alloc(SIZE_T size);

/* 自制字节复制/清零循环，避免编译器要求 CRT 的 memcpy/memset。 */
void Runtime_MemCopy(void* dst, const void* src, SIZE_T size);
void Runtime_MemZero(void* dst, SIZE_T size);

#endif /* CASTLE_WIDESCREEN_RUNTIME_H */
