#ifndef CASTLE_RESOURCE_H
#define CASTLE_RESOURCE_H

/*
 * 这个头文件只保存“Windows 资源 ID”，也就是资源表里每个项目的数字编号。
 *
 * 为什么要单独放一个头文件：
 *   1. launcher_gui.c 需要知道图标的编号，才能在窗口类注册时用 LoadIconW 把它取出来；
 *   2. launcher.rc 也必须使用完全相同的编号，才能把 RPG.ico 编译到同一个资源槽位；
 *   3. 如果两边各自手写一个数字，将来只改了一边，就会出现“EXE 文件图标存在，但标题栏还是空白”的问题。
 *
 * 因此这里只定义一次 IDI_RPG_ICON，C 代码与 RC 资源脚本共同包含本文件。
 */
#define IDI_RPG_ICON 101

#endif
