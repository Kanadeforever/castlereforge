#ifndef CASTLE_BACKLOG_RUNTIME_H
#define CASTLE_BACKLOG_RUNTIME_H

#include "platform.h"

/*
 * RuntimeConfig 是从 Castle_Backlog.ini 读取后的最终配置快照。
 * 业务代码只读这个结构，不在运行中反复打开 INI 文件。
 */
typedef struct RuntimeConfig {
    int enabled;
    u32 max_entries;
    u32 page_size;
    u32 repeat_delay_ticks;
    u32 repeat_interval_ticks;

    int keyboard_open;
    int keyboard_exit;
    int keyboard_up;
    int keyboard_down;
    int keyboard_left;
    int keyboard_right;

    int gamepad_open;
    int gamepad_exit;
    int gamepad_up;
    int gamepad_down;
    int gamepad_left;
    int gamepad_right;
} RuntimeConfig;

/* 绑定模块路径、读取 INI、打开日志并执行精确游戏协议预检。 */
int Runtime_Initialize(HMODULE plugin_module);

/* 只读取得已经裁剪到安全范围的配置。 */
const RuntimeConfig* Runtime_Config(void);

/* 返回插件自身完整路径的同目录文件，用于寻找 INI、日志和可选 SDL3.dll。 */
int Runtime_BuildSiblingPath(const char* file_name, char* output, u32 output_size);

/* 日志函数只接收已经写好的 ASCII/UTF-8 文本，不依赖 printf 或 C 运行库。 */
void Runtime_Log(const char* text);

/* 读取一个键的配置字符串；找不到时把 fallback 原样复制到 output。 */
void Runtime_ReadIniText(const char* section, const char* key, const char* fallback,
                         char* output, u32 output_size);

/* ASCII 工具：用于解析 INI 数字和 SDL 标准按钮名字。 */
int Runtime_ParseU32(const char* text, u32* output);
int Runtime_TextEqualsLoose(const char* left, const char* right);

/* 判断当前前台窗口是否属于 RPG.exe；后台时所有新输入都被忽略。 */
int Runtime_GameIsForeground(void);

/* 对工作线程的毫秒参数做向上取整，保证非零配置至少占一个 8ms tick。 */
u32 Runtime_MsToTicks(u32 milliseconds);

/* 指针只做保守的 32 位用户地址范围检查；业务层仍需验证相关对象状态。 */
int Runtime_PointerLooksReadable(const void* pointer);

#endif /* CASTLE_BACKLOG_RUNTIME_H */
