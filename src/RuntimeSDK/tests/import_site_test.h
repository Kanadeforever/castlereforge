/*
 * 执行真实机器码验证导入调用/装载链：同地址双插件串联、替换IAT后动态跟随、最后一字节NOP。
 * 代码缓冲在测试EXE自身映像中，符合Runtime目标边界校验；只修改本测试进程。
 */
typedef int (WINAPI *ImportTestFunction)(int);
static ImportTestFunction g_import_test_slot;
static void* volatile* g_import_test_next_one;
static void* volatile* g_import_test_next_two;
static BYTE g_import_test_code[2][32];
static BYTE g_import_rollback_code[6];
static BYTE g_import_rollback_data=1u;
static int WINAPI import_test_original(int value) { return value+1; }
static int WINAPI import_test_changed(int value) { return value+7; }
static int WINAPI import_test_one(int value) {
    return ((ImportTestFunction)*g_import_test_next_one)(value)+10;
}
static int WINAPI import_test_two(int value) {
    return ((ImportTestFunction)*g_import_test_next_two)(value)+100;
}
static void import_site_test(const CastleHookApiV1* hook, HMODULE self,
                             CastlePluginHandle first, CastlePluginHandle second) {
    unsigned int mode;
    if (!(hook->capability_flags & CASTLE_HOOK_CAP_IMPORT_SITE)) ExitProcess(230u);
    for(mode=0u;mode<2u;++mode) {
        CastleChainHookClaimV1 claim;
        CastleHookBindingV1 binding;
        CastleTransactionHandle transaction=0u;
        CastleClaimHandle handle=0u;
        static const char label[]="import-site-host-test";
        CastleStringView name;
        DWORD protect, ignored;
        DWORD slot=(DWORD)(ULONG_PTR)&g_import_test_slot;
        BYTE* code=g_import_test_code[mode];
        ImportTestFunction invoke=(ImportTestFunction)code;
        byte_zero_(&claim,sizeof(claim));byte_zero_(&binding,sizeof(binding));
        name.data=label;name.length=sizeof(label)-1u;
        /* 先复制参数，确保FF15与MOV/CALL两种指令均遵守stdcall的入栈/返回约定。 */
        code[0]=0xFFu;code[1]=0x74u;code[2]=0x24u;code[3]=0x04u;
        code[4]=mode?0x8Bu:0xFFu;code[5]=0x15u;
        byte_copy_(code+6u,&slot,4u);
        if(mode) { code[10]=0xFFu;code[11]=0xD2u; }
        code[mode?12:10]=0xC2u;code[mode?13:11]=4u;code[mode?14:12]=0u;
        if(!VirtualProtect(code,32u,PAGE_EXECUTE_READWRITE,&protect) ||
           !FlushInstructionCache(GetCurrentProcess(),code,32u)) ExitProcess(231u);
        g_import_test_slot=import_test_original;
        if(invoke(5)!=6) ExitProcess(232u);
        claim.magic=CASTLE_CHAIN_HOOK_MAGIC;claim.struct_size=sizeof(claim);claim.version=1u;
        claim.hook_kind=mode?CASTLE_HOOK_IAT_LOAD:CASTLE_HOOK_IAT_CALL;
        claim.target=target_from_address_(self,code+4u,6u);
        claim.expected_original_target=slot;
        claim.replacement_hook=(CastleAddress)(ULONG_PTR)import_test_one;
        claim.signature_id=name;claim.label=name;claim.phase=CASTLE_HOOK_PHASE_PRE;
        binding.magic=CASTLE_HOOK_BINDING_MAGIC;binding.struct_size=sizeof(binding);binding.version=1u;
        if(hook->BeginTransaction(first,name,0u,&transaction)<0 ||
           hook->AddRelativeCallHook(transaction,&claim,&handle)<0 ||
           hook->PreflightTransaction(transaction)<0 || hook->CommitTransaction(transaction)<0 ||
           hook->GetHookBinding(handle,&binding)<0) ExitProcess(233u);
        g_import_test_next_one=binding.next_slot;
        if(invoke(5)!=16 || g_import_test_slot!=import_test_original || code[9]!=0x90u) ExitProcess(234u);
        claim.phase=CASTLE_HOOK_PHASE_POST;
        claim.replacement_hook=(CastleAddress)(ULONG_PTR)import_test_two;
        if(hook->BeginTransaction(second,name,0u,&transaction)<0 ||
           hook->AddRelativeCallHook(transaction,&claim,&handle)<0 ||
           hook->PreflightTransaction(transaction)<0 || hook->CommitTransaction(transaction)<0 ||
           hook->GetHookBinding(handle,&binding)<0) ExitProcess(235u);
        g_import_test_next_two=binding.next_slot;
        if(invoke(5)!=116) ExitProcess(236u);
        /* 模拟cnc-ddraw重建IAT：不重新安装链，马上读到新底层函数，两个插件仍各执行一次。 */
        g_import_test_slot=import_test_changed;
        if(invoke(5)!=122) ExitProcess(237u);
        if(!VirtualProtect(code,32u,protect,&ignored)) ExitProcess(238u);
    }
    {
        CastleChainHookClaimV1 claim;
        CastleExclusivePatchClaimV1 patch;
        CastleTransactionHandle transaction;
        CastleClaimHandle claim_handle;
        CastleStringView name;
        DWORD slot=(DWORD)(ULONG_PTR)&g_import_test_slot;
        static const BYTE expected=1u, desired=2u;
        static const char label[]="import-rollback-test";
        byte_zero_(&claim,sizeof(claim));byte_zero_(&patch,sizeof(patch));
        name.data=label;name.length=sizeof(label)-1u;
        g_import_rollback_code[0]=0xFFu;g_import_rollback_code[1]=0x15u;
        byte_copy_(g_import_rollback_code+2u,&slot,4u);
        claim.magic=CASTLE_CHAIN_HOOK_MAGIC;claim.struct_size=sizeof(claim);claim.version=1u;
        claim.hook_kind=CASTLE_HOOK_IAT_CALL;
        claim.target=target_from_address_(self,g_import_rollback_code,6u);
        claim.expected_original_target=slot;
        claim.replacement_hook=(CastleAddress)(ULONG_PTR)import_test_one;
        claim.signature_id=name;claim.label=name;claim.phase=CASTLE_HOOK_PHASE_PRE;
        patch.magic=CASTLE_EXCLUSIVE_PATCH_MAGIC;patch.struct_size=sizeof(patch);patch.version=1u;
        patch.flags=CASTLE_PATCH_FLAG_DATA;
        patch.target=target_from_address_(self,&g_import_rollback_data,1u);
        patch.expected_bytes=&expected;patch.expected_size=1u;
        patch.replacement_bytes=&desired;patch.replacement_size=1u;patch.label=name;
        if(hook->BeginTransaction(first,name,0u,&transaction)<0 ||
           hook->AddRelativeCallHook(transaction,&claim,&claim_handle)<0 ||
           hook->AddExclusivePatch(transaction,&patch,&claim_handle)<0 ||
           hook->PreflightTransaction(transaction)<0) ExitProcess(239u);
        /* 让后一个声明在提交时失败，已写入的导入CALL必须恢复原始六字节。 */
        g_import_rollback_data=3u;
        if(hook->CommitTransaction(transaction)!=CASTLE_ERROR_TRANSACTION_ROLLED_BACK ||
           g_import_rollback_code[0]!=0xFFu || g_import_rollback_code[1]!=0x15u ||
           *(DWORD*)(g_import_rollback_code+2u)!=slot) ExitProcess(240u);
        /* 错IAT槽和错误指令尺寸必须明确拒绝，而不是创建可执行跳板再冒险执行。 */
        if(hook->BeginTransaction(first,name,0u,&transaction)<0) ExitProcess(241u);
        claim.target.size=5u;
        if(hook->AddRelativeCallHook(transaction,&claim,&claim_handle)!=CASTLE_ERROR_INVALID_ARGUMENT)
            ExitProcess(242u);
        claim.target.size=6u;claim.expected_original_target=slot+4u;
        if(hook->AddRelativeCallHook(transaction,&claim,&claim_handle)<0 ||
           hook->PreflightTransaction(transaction)!=CASTLE_ERROR_EXPECTED_BYTES ||
           hook->AbortTransaction(transaction)<0) ExitProcess(243u);
    }
}
