#ifndef CASTLE_ABOUT_H
#define CASTLE_ABOUT_H

#include "platform.h"

/*
 * 《幽城幻剑录》Mod Loader —— “关于”页面内容接口。
 *
 * 这个头文件只描述“关于窗口要显示什么”，真正的窗口绘制、按钮布局、鼠标点击都由
 * launcher_gui.c 负责。把两层分开以后，未来只是改作者、版本、项目说明或链接时，
 * 只需要编辑 about.cpp；不会误碰已经封存的 dev9 游戏启动、Locale、ASI、Overrides
 * 或 DirectDraw 兼容代码。
 *
 * 本工程的 Launcher 不链接 C/C++ 运行库，所以这里保持最简单的 C ABI：
 *   - 所有文字都是只读 UTF-16 静态字符串；
 *   - 调用方不能 free，也不能修改返回指针；
 *   - 不使用 std::string / std::vector / new / delete；
 *   - 链接数量通过固定读取函数提供，GUI 自己按数量创建可点击项。
 */

/*
 * GUI 当前最多显示 8 个链接。
 * 这是为了让 About 窗口在不引入动态容器的情况下仍保持简单、可审计。
 * 如果以后确实需要超过 8 个链接，可以同时修改这个上限和 launcher_gui.c 的布局。
 */
#define ABOUT_MAX_LINKS 8u

#ifdef __cplusplus
extern "C" {
#endif

/* 返回 About 顶层窗口标题。 */
LPCWSTR About_GetDialogTitle(void);

/*
 * 返回 About 正文。
 * 可以写 \r\n 换行；GUI 会在自定义 About 窗口里按宽度自动换行，并根据折行后的真实高度自动扩展窗口，不再使用 MessageBox。
 */
LPCWSTR About_GetDialogText(void);

/* 返回当前配置了多少个可点击链接。0 表示不显示链接区。 */
UINT About_GetLinkCount(void);

/*
 * 返回第 index 个链接的显示文字和真正 URL。
 * index 从 0 开始；越界时返回空字符串，避免 GUI 因编辑 about.cpp 出错而读到非法地址。
 */
LPCWSTR About_GetLinkLabel(UINT index);
LPCWSTR About_GetLinkUrl(UINT index);

#ifdef __cplusplus
}
#endif

#endif
