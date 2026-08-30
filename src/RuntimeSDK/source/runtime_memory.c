#include "runtime_internal.h"
#include "../include/CastleHook_API.h"

static int runtime_range_committed_(const BYTE* begin, CastleU32 size) {
    const BYTE* current = begin;
    const BYTE* end = begin + size;

    while (current < end) {
        MEMORY_BASIC_INFORMATION information;
        const BYTE* region_end;
        if (VirtualQuery(current, &information, sizeof(information)) == 0u ||
            information.State != MEM_COMMIT ||
            (information.Protect & PAGE_GUARD) != 0u ||
            information.Protect == PAGE_NOACCESS) return 0;
        region_end = (const BYTE*)information.BaseAddress + information.RegionSize;
        if (region_end <= current) return 0;
        current = region_end < end ? region_end : end;
    }
    return 1;
}

void* Runtime_ResolveTarget(const CastleTargetAddressV1* target) {
    BYTE* base;
    BYTE* address;
    IMAGE_DOS_HEADER* dos_header;
    IMAGE_NT_HEADERS32* nt_headers;
    CastleU32 image_size;

    if (!target || target->module == 0u || target->size == 0u) return NULL;
    if (target->rva + target->size < target->rva) return NULL;
    base = (BYTE*)(ULONG_PTR)target->module;
    dos_header = (IMAGE_DOS_HEADER*)base;
    if (!runtime_range_committed_(base, (CastleU32)sizeof(IMAGE_DOS_HEADER)) ||
        dos_header->e_magic != IMAGE_DOS_SIGNATURE) return NULL;
    nt_headers = (IMAGE_NT_HEADERS32*)(base + dos_header->e_lfanew);
    if (!runtime_range_committed_((const BYTE*)nt_headers,
                                  (CastleU32)sizeof(IMAGE_NT_HEADERS32)) ||
        nt_headers->Signature != IMAGE_NT_SIGNATURE ||
        nt_headers->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) return NULL;
    image_size = nt_headers->OptionalHeader.SizeOfImage;
    if (target->rva >= image_size || target->size > image_size - target->rva) return NULL;
    address = base + target->rva;
    if (!runtime_range_committed_(address, target->size)) return NULL;
    return address;
}

int Runtime_MemoryEquals(const void* memory, const CastleU8* expected,
                         CastleU32 size) {
    const volatile CastleU8* actual = (const volatile CastleU8*)memory;
    CastleU32 index;
    if (!actual || !expected || size == 0u) return 0;
    for (index = 0u; index < size; ++index) {
        if (actual[index] != expected[index]) return 0;
    }
    return 1;
}

CastleResult Runtime_WriteMemory(void* target, const CastleU8* bytes,
                                 CastleU32 size, int executable) {
    DWORD old_protection = 0u;
    DWORD ignored = 0u;
    volatile CastleU8* output = (volatile CastleU8*)target;
    CastleU32 index;
    DWORD writable_protection = executable ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;

    if (!target || !bytes || size == 0u) return CASTLE_ERROR_INVALID_ARGUMENT;
    if (!VirtualProtect(target, size, writable_protection, &old_protection)) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }

    for (index = 0u; index < size; ++index) output[index] = bytes[index];

    if (!VirtualProtect(target, size, old_protection, &ignored)) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    if (executable && !FlushInstructionCache(GetCurrentProcess(), target, size)) {
        return CASTLE_ERROR_RUNTIME_FAULT;
    }
    return CASTLE_OK;
}
