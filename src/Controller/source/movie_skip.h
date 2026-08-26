#ifndef CASTLE_PAD_MOVIE_SKIP_H
#define CASTLE_PAD_MOVIE_SKIP_H

/*
 * movie_skip.h
 *
 * 这个模块只做一件事：原版 Bink 电影正在播放时，把手柄 Start 的一次“新按下”变成一次原版 ESC。
 * 它不区分 LOGO、剧情、标题前后，也不保存任何历史状态。
 */

/* 启动时只读校验原版电影/ESC 协议；失败只禁用本功能。 */
void MovieSkip_Initialize(void);

/* 每个 worker tick 调一次。没有电影或没有新的 Start 按下时什么都不做。 */
void MovieSkip_Update(void);

/* 供日志/检查使用：1=本功能可用，0=协议/API 未通过。 */
int MovieSkip_Enabled(void);

#endif /* CASTLE_PAD_MOVIE_SKIP_H */
