#ifndef CASTLE_BACKLOG_RUNTIME_H
#define CASTLE_BACKLOG_RUNTIME_H

#include "platform.h"
#include "CastleRuntime_API.h"

/*
 * RuntimeConfig 是从 Castle_Backlog.ini 读取后的最终配置快照。
 * 业务代码只读这个结构，不在运行中反复打开 INI 文件。
 */
typedef struct RuntimeConfig {
    int enabled;
    u32 max_entries;
    u32 page_size;

    /* 相邻历史对话框的纵向距离，直接来自 INI 的 PanelStrideY。 */
    u32 panel_stride_y;

    u32 repeat_delay_ticks;
    u32 repeat_interval_ticks;

    int keyboard_open;
    int keyboard_exit;
    int keyboard_up;
    int keyboard_down;
    int keyboard_left;
    int keyboard_right;

} RuntimeConfig;

/* 绑定模块路径、读取 INI、打开日志并执行精确游戏协议预检。 */
int Runtime_Initialize(HMODULE plugin_module);

/* 整合模式从 Runtime Path 取得插件路径，并把共享 Hook 点交给 Hook 事务预检。 */
int Runtime_InitializeIntegrated(HMODULE plugin_module,
                                 const CastleRuntimeApiV1* runtime_api,
                                 CastlePluginHandle plugin_handle);

/* 只读取得已经裁剪到安全范围的配置。 */
const RuntimeConfig* Runtime_Config(void);

/* 返回插件自身完整路径的同目录文件，用于寻找 INI、日志和协作插件配置。 */
int Runtime_BuildSiblingPath(const char* file_name, char* output, u32 output_size);

/* 日志函数只接收已经写好的 ASCII/UTF-8 文本，不依赖 printf 或 C 运行库。 */
void Runtime_Log(const char* text);

/* 读取一个键的配置字符串；找不到时把 fallback 原样复制到 output。 */
void Runtime_ReadIniText(const char* section, const char* key, const char* fallback,
                         char* output, u32 output_size);

/* ASCII 工具：用于解析 INI 数字和 Virtual-Key 名称。 */
int Runtime_ParseU32(const char* text, u32* output);
int Runtime_TextEqualsLoose(const char* left, const char* right);

/* 判断当前前台窗口是否属于 RPG.exe；后台时所有新输入都被忽略。 */
int Runtime_GameIsForeground(void);

/* 对工作线程的毫秒参数做向上取整，保证非零配置至少占一个 8ms tick。 */
u32 Runtime_MsToTicks(u32 milliseconds);

/*
 * 第一层快速地址检查：只判断数值是否落在 32 位用户地址范围。
 * 它非常便宜，但不能证明页面当前仍然已提交。
 */
int Runtime_PointerLooksReadable(const void* pointer);

/*
 * 真正检查一整段内存是否处于 MEM_COMMIT 且允许读取。
 * 捕获 NPC 姓名、正文以及读取原版 UI 对象前都应优先使用这一版。
 */
int Runtime_MemoryRangeReadable(const void* pointer, u32 size);

#endif /* CASTLE_BACKLOG_RUNTIME_H */
