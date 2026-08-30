#ifndef CASTLE_RUNTIME_PLATFORM_H
#define CASTLE_RUNTIME_PLATFORM_H

/*
 * runtime_platform.h
 *
 * 这个头只供 Castle_Runtime.dll 私有源码使用，所以可以包含 Windows SDK。
 * 公共 ABI 头仍然完全不包含 windows.h，第三方插件不会继承这里的宏和结构。
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <tlhelp32.h>

#endif /* CASTLE_RUNTIME_PLATFORM_H */
