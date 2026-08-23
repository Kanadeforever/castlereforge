#ifndef CASTLE_LOCALE_BOOTSTRAP_H
#define CASTLE_LOCALE_BOOTSTRAP_H

/*
 * CastleModCore.dll 只需要读取早期 Bootstrap 的最终状态。
 * Bootstrap DLL 本身不通过这个头静态链接；Core 会用 GetProcAddress 动态查询，
 * 这样普通 Mod Core 仍然可以继续只静态依赖 KERNEL32。
 */

#define CASTLE_LOCALE_BOOTSTRAP_DLL_ L"CastleLocaleBootstrap.dll"
#define CASTLE_LOCALE_BOOTSTRAP_STATUS_EXPORT_ "CastleLocaleBootstrap_GetStatus"
#define CASTLE_LOCALE_BOOTSTRAP_OK_ 1

#endif
