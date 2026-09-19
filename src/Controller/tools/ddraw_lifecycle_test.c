/*
 * 用真实cnc-ddraw副本验证IAT安装时序。窗口不显示，不设置视频模式，不移动鼠标。
 * 仅改测试进程自己的GetCursorPos入口：模拟插件安装，然后观察SetCooperativeLevel后是否还在。
 * DLL和它可能保存的INI均位于启动器新建的临时目录，参考资料保持只读。
 */
#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#include <windows.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CastlePlugin_API.h"
#include "CastleHook_API.h"
#pragma comment(lib, "user32.lib")

typedef HRESULT (WINAPI *CreateDrawFn)(GUID*, LPDIRECTDRAW*, IUnknown*);
typedef BOOL (WINAPI *CursorFn)(LPPOINT);
static BYTE bridge_code[16];
static void* volatile* bridge_next[2];
static int bridge_calls[2];
static BOOL WINAPI bridge_one(LPPOINT point) {
    ++bridge_calls[0];return ((CursorFn)*bridge_next[0])(point);
}
static BOOL WINAPI bridge_two(LPPOINT point) {
    ++bridge_calls[1];return ((CursorFn)*bridge_next[1])(point);
}
static CastleStringView view(const char* text) {
    CastleStringView result;result.data=text;result.length=(CastleU32)strlen(text);return result;
}

/* 在兼容层建窗前安装真实Runtime调用链，两个测试插件使用同一个固定CALL站点。 */
static int install_bridge(DWORD* slot) {
    char runtime_path[MAX_PATH];
    HMODULE runtime;
    FARPROC entry;
    CastleRuntimeGetApiFn get_api=NULL;
    const CastleRuntimeApiV1* api;
    const CastleHookApiV1* hook;
    CastleInterfaceQueryV1 query={0};
    CastleInterfaceResultV1 result={0};
    DWORD slot_address=(DWORD)(ULONG_PTR)slot, old;
    unsigned int index;
    if(!GetEnvironmentVariableA("CASTLE_TEST_RUNTIME_DLL",runtime_path,sizeof(runtime_path))) return 0;
    runtime=LoadLibraryA(runtime_path);if(!runtime)return -1;
    entry=GetProcAddress(runtime,"CastleRuntime_GetApi");memcpy(&get_api,&entry,sizeof(get_api));
    if(!get_api || !(api=get_api(CASTLE_RUNTIME_ABI_V1))) return -2;
    query.magic=CASTLE_QUERY_MAGIC;query.struct_size=sizeof(query);query.request_version=1u;
    query.interface_id=view(CASTLE_HOOK_INTERFACE_ID);query.requested_version=1u;
    query.minimum_struct_size=CASTLE_SIZEOF_HOOK_API_V1;
    query.required_capabilities_low=CASTLE_HOOK_CAP_IMPORT_SITE;
    result.magic=CASTLE_INTERFACE_API_MAGIC;result.struct_size=sizeof(result);result.result_version=1u;
    if(api->QueryInterface(&query,&result)<0)return -3;
    hook=(const CastleHookApiV1*)result.api_pointer;
    bridge_code[0]=0xFFu;bridge_code[1]=0x74u;bridge_code[2]=0x24u;bridge_code[3]=4u;
    bridge_code[4]=0xFFu;bridge_code[5]=0x15u;memcpy(bridge_code+6u,&slot_address,4u);
    bridge_code[10]=0xC2u;bridge_code[11]=4u;
    if(!VirtualProtect(bridge_code,sizeof(bridge_code),PAGE_EXECUTE_READWRITE,&old))return -4;
    FlushInstructionCache(GetCurrentProcess(),bridge_code,sizeof(bridge_code));
    for(index=0u;index<2u;++index) {
        CastlePluginDescriptorV1 descriptor={0};
        CastlePluginHandle plugin=0u;
        CastleTransactionHandle transaction=0u;
        CastleClaimHandle claim_handle=0u;
        CastleChainHookClaimV1 claim={0};
        CastleHookBindingV1 binding={0};
        descriptor.magic=CASTLE_PLUGIN_DESC_MAGIC;descriptor.struct_size=sizeof(descriptor);
        descriptor.descriptor_version=1u;
        descriptor.module=(CastleModule)(ULONG_PTR)GetModuleHandleW(NULL);
        descriptor.plugin_id=view(index?"test.pointer.two":"test.pointer.one");
        descriptor.display_name=descriptor.plugin_id;descriptor.version_text=view("1");descriptor.build_id=view("test");
        if(api->RegisterPlugin(&descriptor,&plugin)<0)return -5;
        claim.magic=CASTLE_CHAIN_HOOK_MAGIC;claim.struct_size=sizeof(claim);claim.version=1u;
        claim.hook_kind=CASTLE_HOOK_IAT_CALL;claim.target.module=descriptor.module;
        claim.target.rva=(DWORD)(ULONG_PTR)(bridge_code+4u)-descriptor.module;claim.target.size=6u;
        claim.expected_original_target=slot_address;
        claim.replacement_hook=(CastleAddress)(ULONG_PTR)(index?bridge_two:bridge_one);
        claim.signature_id=view("test.pointer.function.v1");claim.label=claim.signature_id;
        claim.phase=index?CASTLE_HOOK_PHASE_POST:CASTLE_HOOK_PHASE_PRE;
        binding.magic=CASTLE_HOOK_BINDING_MAGIC;binding.struct_size=sizeof(binding);binding.version=1u;
        if(hook->BeginTransaction(plugin,claim.label,0u,&transaction)<0 ||
           hook->AddRelativeCallHook(transaction,&claim,&claim_handle)<0 ||
           hook->PreflightTransaction(transaction)<0 || hook->CommitTransaction(transaction)<0 ||
           hook->GetHookBinding(claim_handle,&binding)<0) return -6;
        bridge_next[index]=binding.next_slot;
    }
    return 1;
}

/* 按PE导入名称找到宿主自己的IAT槽，避免把函数入口误当成可写指针。 */
static DWORD* find_import(const char* name) {
    BYTE* base=(BYTE*)GetModuleHandleW(NULL);
    IMAGE_DOS_HEADER* dos=(IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS32* pe=(IMAGE_NT_HEADERS32*)(base+dos->e_lfanew);
    IMAGE_IMPORT_DESCRIPTOR* desc=(IMAGE_IMPORT_DESCRIPTOR*)(base+
        pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    for(;desc->Name;++desc) {
        IMAGE_THUNK_DATA32* names=(IMAGE_THUNK_DATA32*)(base+desc->OriginalFirstThunk);
        IMAGE_THUNK_DATA32* slots=(IMAGE_THUNK_DATA32*)(base+desc->FirstThunk);
        if(!desc->OriginalFirstThunk) continue;
        for(;names->u1.AddressOfData;++names,++slots) {
            IMAGE_IMPORT_BY_NAME* entry;
            if(IMAGE_SNAP_BY_ORDINAL32(names->u1.Ordinal)) continue;
            entry=(IMAGE_IMPORT_BY_NAME*)(base+names->u1.AddressOfData);
            if(strcmp((char*)entry->Name,name)==0) return &slots->u1.Function;
        }
    }
    return NULL;
}
static BOOL WINAPI marker_cursor(LPPOINT point) {
    if(point) {point->x=123;point->y=234;}
    return TRUE;
}

int main(void) {
    char path[MAX_PATH];
    DWORD* slot;
    DWORD old_protect, ignored, before;
    HMODULE compat;
    FARPROC address;
    CreateDrawFn create_draw=NULL;
    LPDIRECTDRAW draw=NULL;
    HWND window;
    HRESULT result;
    POINT point;
    int bridge;
    /* 强制链接GetCursorPos导入，此次读取只用于检查宿主IAT存在。 */
    if(!GetEnvironmentVariableA("CASTLE_TEST_DDRAW_DLL",path,sizeof(path)) || !GetCursorPos(&point)) return 1;
    slot=find_import("GetCursorPos");
    if(!slot) return 2;
    compat=LoadLibraryA(path);
    if(!compat) {printf("LoadLibrary error=%lu\n",GetLastError());return 3;}
    bridge=install_bridge(slot);
    if(bridge<0) {printf("Runtime bridge install=%d\n",bridge);return 8;}
    before=*slot;
    /* DLL初次加载之后安装模拟插件，正对应游戏入口前的Runtime初始化顺序。 */
    if(!VirtualProtect(slot,sizeof(*slot),PAGE_READWRITE,&old_protect)) return 4;
    *slot=(DWORD)(ULONG_PTR)marker_cursor;
    VirtualProtect(slot,sizeof(*slot),old_protect,&ignored);
    address=GetProcAddress(compat,"DirectDrawCreate");
    memcpy(&create_draw,&address,sizeof(create_draw));
    if(!create_draw || FAILED(create_draw(NULL,&draw,NULL))) return 5;
    window=CreateWindowExA(0,"STATIC","Castle pointer lifecycle test",WS_OVERLAPPED,
        0,0,320,240,NULL,NULL,GetModuleHandleW(NULL),NULL);
    if(!window) return 6;
    result=IDirectDraw_SetCooperativeLevel(draw,window,DDSCL_NORMAL);
    printf("SetCooperativeLevel=%08lX, before=%08lX, marker=%08lX, after=%08lX\n",
        (unsigned long)result,before,(DWORD)(ULONG_PTR)marker_cursor,*slot);
    printf("IAT overwritten after plugin install: %s\n",
        *slot!=(DWORD)(ULONG_PTR)marker_cursor?"YES":"NO");
    if(bridge) {
        CursorFn invoke=(CursorFn)bridge_code;
        BOOL called=invoke(&point);
        printf("Runtime chain after real ddraw rehook: result=%d, first=%d, second=%d\n",
            called,bridge_calls[0],bridge_calls[1]);
        if(!called || bridge_calls[0]!=1 || bridge_calls[1]!=1 ||
           *slot==(DWORD)(ULONG_PTR)marker_cursor) return 9;
    }
    /* 兼容层若覆盖回自己的函数，此读不会经过marker；返回值使外部可区分验证是否运行成功。 */
    IDirectDraw_Release(draw);
    DestroyWindow(window);
    FreeLibrary(compat);
    return FAILED(result)?7:0;
}
