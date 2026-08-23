#include "platform.h"

/*
 * 本工程刻意不链接通用 C 运行库，因此编译器把小块清零/复制优化成 memset/memcpy 时，
 * 由这里提供最小实现。volatile 的目的不是“提高速度”，恰恰是阻止优化器再次把这个循环
 * 识别成对 memset/memcpy 自己的递归调用；代码量很小，但能让最终 EXE/DLL 只依赖系统 DLL。
 */
void* __cdecl memset(void* dst, int value, SIZE_T count) {
    volatile BYTE* p = (volatile BYTE*)dst;
    SIZE_T i;
    for (i = 0u; i < count; ++i) p[i] = (BYTE)value;
    return dst;
}

void* __cdecl memcpy(void* dst, const void* src, SIZE_T count) {
    volatile BYTE* d = (volatile BYTE*)dst;
    const volatile BYTE* s = (const volatile BYTE*)src;
    SIZE_T i;
    for (i = 0u; i < count; ++i) d[i] = s[i];
    return dst;
}

/*
 * x86 MSVC/clang-cl 的大栈帧探测辅助函数。
 *
 * 为什么这个函数必须由我们自己提供：
 *   只要某个函数一次需要在栈上预留超过一页（通常 4096 字节）的空间，x86 编译器就不会直接
 *   把 ESP 一口气减掉那么多，而是先调用 __chkstk，逐页“碰一下”即将进入的栈页。这样 Windows
 *   才有机会依次触发栈的 Guard Page 扩展；如果跳过 Guard Page，程序可能直接发生栈异常。
 *
 * 旧 v0.1 为了省事链接了 libvcruntime.lib。结果最终 EXE/DLL 又产生了对
 * KERNELBASE!__chkstk 的外部导入，用户实机在进程真正启动前就报“无法定位程序输入点 chkstk”。
 * 本工程本来就刻意不依赖 CRT/vcruntime，因此这里直接实现 x86 编译器约定要求的最小 helper。
 *
 * x86 __chkstk 的特殊约定：
 *   - 进入函数时 EAX = 需要分配的字节数；
 *   - helper 自己负责逐页探测，并把 ESP 真正移动到新栈顶；
 *   - 返回后 EAX 仍恢复成原始分配字节数；
 *   - ECX 被保存/恢复，其它通用寄存器不被改动；
 *   - 返回地址必须从旧栈搬到新栈，保证调用者后续像普通 call 一样继续执行。
 *
 * 这段代码不调用任何 Windows API，也不依赖任何 DLL，所以 Launcher 和 Core 最终都不会再因为
 * 栈探测 helper 多出 KERNELBASE 或 vcruntime 依赖。
 *
 * 注意：函数名在 C 源码里写作 _chkstk。x86 __cdecl 会自动在 COFF 符号前再加一个下划线，
 * 因而最终正好导出/解析为编译器请求的“__chkstk”符号。
 */
#if defined(_M_IX86) || defined(__i386__)
__declspec(naked) void __cdecl _chkstk(void) {
    __asm {
        /* 先保存 ECX。call 已经把返回地址压栈，所以保存后 ESP 比调用前低 8 字节。 */
        push ecx

        /*
         * ECX 指向“调用 __chkstk 之前”的旧 ESP。
         * 当前 ESP + 0 是保存的 ECX，+4 是返回地址，+8 才是调用前的栈顶。
         */
        lea ecx, [esp + 8]

        /* 小于一页时不需要循环，直接进入最后一段探测。 */
        cmp eax, 1000h
        jb chkstk_last_page_

    chkstk_probe_page_:
        /* 每次向下走一页并读取该页，促使 Windows 正常推进 Guard Page。 */
        sub ecx, 1000h
        test dword ptr [ecx], eax

        /* EAX 暂时当作“还剩多少字节未探测”的计数器。 */
        sub eax, 1000h
        cmp eax, 1000h
        ja chkstk_probe_page_

    chkstk_last_page_:
        /* 处理不足一整页的最后一段，并确保最终目标栈页也真正被访问。 */
        sub ecx, eax
        test dword ptr [ecx], eax

        /*
         * EAX 先记住旧的“返回地址槽”地址。之后 ESP 会被直接切到新栈顶，
         * 所以旧栈中的返回地址与保存的 ECX 都必须在切换前留有可访问的指针。
         */
        lea eax, [esp + 4]
        mov esp, ecx

        /* 从旧栈恢复 ECX。 */
        mov ecx, [eax - 4]

        /* 把旧返回地址重新压到新栈上，ret 才能回到调用者。 */
        push dword ptr [eax]

        /*
         * 现在：EAX = 旧返回地址槽地址，ESP = 新栈顶上的返回地址槽。
         * 两者相减恰好得到原始申请字节数，因此把 EAX 恢复为编译器期望的输入值。
         */
        sub eax, esp
        ret
    }
}
#endif
