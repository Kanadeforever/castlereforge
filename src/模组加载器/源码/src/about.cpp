#include "about.h"

/*
 * 《幽城幻剑录》Mod Loader —— “关于”页面可编辑内容。
 *
 * 以后如果只是想改 About，通常只需要改本文件下面三块内容：
 *   1. kAboutDialogTitle_ —— 窗口标题；
 *   2. kAboutDialogText_  —— 正文；
 *   3. kAboutLinks_       —— 可点击链接。
 *
 * 写法说明（给只上过一天编程课也能直接改的人看）：
 *   - L"文字" 前面的 L 表示这是 Windows Unicode 宽字符串，不能删；
 *   - 想换行就写 \r\n；
 *   - 相邻的 L"..." 会在编译时自动拼成一整段，所以可以一行写一句；
 *   - 链接左边是显示名称，右边是真正打开的网址；
 *   - 删除某一整行 { L"名称", L"网址" } 就等于删除一个链接；
 *   - 新增链接时照着现有格式复制一行，并确保总数不要超过 ABOUT_MAX_LINKS；
 *   - About 窗口会按这里正文实际折行后的高度自动扩展，正常增删几行说明不需要再改 GUI 坐标；
 *   - 本文件不使用 STL、异常、new/delete，也不参与任何游戏 Hook。
 */

static const LPCWSTR kAboutDialogTitle_ =
    (LPCWSTR)L"关于 - 《幽城幻剑录》Mod Loader";

static const LPCWSTR kAboutDialogText_ =
    (LPCWSTR)L"《幽城幻剑录》Mod Loader\r\n"
    L"by Luminous\r\n"
    L"v0.3.0-dev9\r\n"
    L"\r\n"
    L"面向《幽城幻剑录》台湾第三版的便携 Mod Loader。\r\n"
    L"支持 ASI 插件、Overrides 文件覆写、台湾繁中运行环境与插件 INI 编辑。\r\n"
    L"\r\n"
    L"运行时基线：v0.3.0-dev9。";

/*
 * 一个链接就是“用户看到的文字 + 点击后打开的网址”两项。
 * 这些默认链接对应本项目文档已经明确提到的第三方项目，方便直接验证点击功能；
 * 以后可以完全替换成自己的项目主页、发布页、作者主页、许可证页面等。
 */
typedef struct AboutLink_ {
    LPCWSTR label;
    LPCWSTR url;
} AboutLink_;

static const AboutLink_ kAboutLinks_[] = {
    { (LPCWSTR)L"Ultimate ASI Loader", (LPCWSTR)L"https://github.com/ThirteenAG/Ultimate-ASI-Loader" },
    { (LPCWSTR)L"cnc-ddraw",           (LPCWSTR)L"https://github.com/FunkyFr3sh/cnc-ddraw" }
};

extern "C" LPCWSTR About_GetDialogTitle(void) {
    return kAboutDialogTitle_;
}

extern "C" LPCWSTR About_GetDialogText(void) {
    return kAboutDialogText_;
}

extern "C" UINT About_GetLinkCount(void) {
    UINT count = (UINT)(sizeof(kAboutLinks_) / sizeof(kAboutLinks_[0]));
    return count > ABOUT_MAX_LINKS ? ABOUT_MAX_LINKS : count;
}

extern "C" LPCWSTR About_GetLinkLabel(UINT index) {
    if (index >= About_GetLinkCount()) return (LPCWSTR)L"";
    return kAboutLinks_[index].label;
}

extern "C" LPCWSTR About_GetLinkUrl(UINT index) {
    if (index >= About_GetLinkCount()) return (LPCWSTR)L"";
    return kAboutLinks_[index].url;
}
