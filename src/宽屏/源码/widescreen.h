#ifndef CASTLE_WIDESCREEN_H
#define CASTLE_WIDESCREEN_H

/*
 * 安装所有宽屏业务 Hook。
 * 返回 1 表示本轮安装全部成功；返回 0 表示至少一个必要资源或 Hook 安装失败。
 */
int Widescreen_Install(void);

#endif /* CASTLE_WIDESCREEN_H */
