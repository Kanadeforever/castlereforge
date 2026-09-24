#ifndef CASTLE_SAVE_VISUAL_H
#define CASTLE_SAVE_VISUAL_H
#include "CastleRuntime_API.h"
#include "CastleOverlay_API.h"
#include "CastleDisplay_API.h"
#include "CastleClock_API.h"

// 可视反馈独立成编译单元，避免把Windows/DirectDraw头文件混进原来的无CRT存档核心。
// 接口只接收保存结果，不决定何时允许保存，也不读取/修改手柄状态。
namespace savevisual {
struct Options {
    bool notification;
    bool slotLabels;
    bool latestMark;
    bool pulse;
    bool bottom;
    unsigned int slotTextWeight; // 0～200表示额外0.00～2.00像素；小数部分做边缘颜色混合。
    int slotTextOffsetY; // 相对原槽号的垂直微调，默认-2；类型与“新”共用基线。
    bool slotTextOutline; // 槽位文字统一一像素深色描边；独立于笔画粗细和书卷提示。
};
using PathBuilder = bool (*)(const wchar_t* relative, wchar_t* out, unsigned int capacity);
using LogLine = void (*)(const char* text);
void Initialize(const CastleOverlayApiV1* overlay, const CastleDisplayApiV1* display,
    const CastleClockApiV1* clock, CastlePluginHandle plugin, unsigned char* exe,
    const Options& options, PathBuilder path, LogLine log);
void Shutdown();
void BeginSave(bool automatic);
void FinishSave(bool automatic, bool success);
void SavedSlot(unsigned int slot, bool readable);
void BeginLoad();
void EndLoad();
unsigned int LoadStatus(PathBuilder path, LogLine log);
unsigned int NextAutoSlot();
}
#endif
