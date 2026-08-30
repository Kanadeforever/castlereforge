#include "runtime_internal.h"

/* 当前仓库已实机确认的 RPG.exe v1 公共 RVA。绝对地址统一由模块基址相加得到。 */
#define RUNTIME_RPG_ENTRY_RVA          0x00052C19u
#define RUNTIME_CAMERA_X_RVA           0x00578514u
#define RUNTIME_CAMERA_Y_RVA           0x00578518u
#define RUNTIME_CAMERA_BOUNDS_RVA      0x0004B300u
#define RUNTIME_CAMERA_CLAMP_RVA       0x0004B360u
#define RUNTIME_RENDER_QUEUE_RVA       0x00034710u
#define RUNTIME_DISPLAY_PRESENT_RVA    0x00005A10u

static int g_profile_supported;
static CastleU8* g_game_base;
static CastleU32 g_game_image_size;

static int symbols_range_valid_(CastleU32 rva, CastleU32 size) {
    return g_game_base && rva < g_game_image_size && size <= g_game_image_size - rva;
}

static int symbols_bytes_equal_(CastleU32 rva, const CastleU8* expected,
                                CastleU32 size) {
    CastleU32 index;
    if (!expected || size == 0u || !symbols_range_valid_(rva, size)) return 0;
    for (index = 0u; index < size; ++index) {
        if (g_game_base[rva + index] != expected[index]) return 0;
    }
    return 1;
}

void Runtime_SymbolsInitialize(void) {
    IMAGE_DOS_HEADER* dos_header;
    IMAGE_NT_HEADERS32* nt_headers;
    static const CastleU8 entry_signature[] = {0x55u,0x8Bu,0xECu,0x6Au,0xFFu};
    static const CastleU8 camera_bounds_signature[] = {
        0x8Bu,0x44u,0x24u,0x04u,0x8Bu,0x4Cu,0x24u,0x08u
    };
    static const CastleU8 camera_clamp_signature[] = {
        0xA1u,0x50u,0x85u,0x97u,0x00u,0x53u,0x56u,0x57u
    };
    static const CastleU8 render_queue_signature[] = {
        0x53u,0x56u,0x6Au,0x00u,0x8Bu,0xD9u,0x6Au,0x01u
    };
    static const CastleU8 present_signature[] = {
        0x56u,0x8Bu,0xF1u,0x6Au,0x01u,0x8Bu,0x46u,0x70u
    };

    g_profile_supported = 0;
    g_game_base = (CastleU8*)(ULONG_PTR)Runtime_GetGameModuleValue();
    g_game_image_size = 0u;
    if (!g_game_base) return;
    dos_header = (IMAGE_DOS_HEADER*)g_game_base;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE || dos_header->e_lfanew <= 0) return;
    nt_headers = (IMAGE_NT_HEADERS32*)(g_game_base + (CastleU32)dos_header->e_lfanew);
    if (nt_headers->Signature != IMAGE_NT_SIGNATURE ||
        nt_headers->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) return;
    g_game_image_size = nt_headers->OptionalHeader.SizeOfImage;

    /*
     * 五个锚点同时证明启动入口、Camera 数据语义和两条公共绘制函数都属于
     * 当前确认版本。任意一处变化都整体关闭固定地址能力，不做近似猜测。
     */
    if (!symbols_bytes_equal_(RUNTIME_RPG_ENTRY_RVA, entry_signature,
                              (CastleU32)sizeof(entry_signature)) ||
        !symbols_bytes_equal_(RUNTIME_CAMERA_BOUNDS_RVA, camera_bounds_signature,
                              (CastleU32)sizeof(camera_bounds_signature)) ||
        !symbols_bytes_equal_(RUNTIME_CAMERA_CLAMP_RVA, camera_clamp_signature,
                              (CastleU32)sizeof(camera_clamp_signature)) ||
        !symbols_bytes_equal_(RUNTIME_RENDER_QUEUE_RVA, render_queue_signature,
                              (CastleU32)sizeof(render_queue_signature)) ||
        !symbols_bytes_equal_(RUNTIME_DISPLAY_PRESENT_RVA, present_signature,
                              (CastleU32)sizeof(present_signature)) ||
        !symbols_range_valid_(RUNTIME_CAMERA_X_RVA, 8u)) {
        Runtime_DiagnosticAppend("[Symbols] unknown game build; fixed-address services disabled.");
        return;
    }
    g_profile_supported = 1;
    Runtime_DiagnosticAppend("[Symbols] confirmed RPG.exe v1 profile.");
}

int Runtime_GameProfileSupported(void) {
    return g_profile_supported;
}

int Runtime_ReadOriginalCamera(CastleS32* out_x, CastleS32* out_y) {
    if (!g_profile_supported || !out_x || !out_y ||
        !symbols_range_valid_(RUNTIME_CAMERA_X_RVA, 8u)) return 0;
    *out_x = *(volatile CastleS32*)(g_game_base + RUNTIME_CAMERA_X_RVA);
    *out_y = *(volatile CastleS32*)(g_game_base + RUNTIME_CAMERA_Y_RVA);
    return 1;
}

void* Runtime_GetOriginalRenderQueue(void) {
    return g_profile_supported && symbols_range_valid_(RUNTIME_RENDER_QUEUE_RVA, 1u) ?
        (void*)(g_game_base + RUNTIME_RENDER_QUEUE_RVA) : NULL;
}

void* Runtime_GetOriginalDisplayPresent(void) {
    return g_profile_supported && symbols_range_valid_(RUNTIME_DISPLAY_PRESENT_RVA, 1u) ?
        (void*)(g_game_base + RUNTIME_DISPLAY_PRESENT_RVA) : NULL;
}
