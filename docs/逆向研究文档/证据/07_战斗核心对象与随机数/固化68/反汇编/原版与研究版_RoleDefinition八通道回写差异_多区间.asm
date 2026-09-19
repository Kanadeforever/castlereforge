; 固化68补充直接反汇编证据。由当前工具从已校验输入只读生成。
; 业务结论仍以机器JSON和完整接档说明中的证据边界为准。

; ===== 原版八通道运行时重算 0x0041F4A4..0x0041F570 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f4a4 <.text+0x1e4a4>:
  41f4a4:	8d be 4c 08 00 00    	lea    edi,[esi+0x84c]
  41f4aa:	6a ff                	push   0xffffffff
  41f4ac:	6a 00                	push   0x0
  41f4ae:	57                   	push   edi
  41f4af:	e8 6c c7 01 00       	call   0x43bc20
  41f4b4:	6a ff                	push   0xffffffff
  41f4b6:	6a ff                	push   0xffffffff
  41f4b8:	6a ff                	push   0xffffffff
  41f4ba:	89 86 b4 08 00 00    	mov    DWORD PTR [esi+0x8b4],eax
  41f4c0:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f4c6:	6a 01                	push   0x1
  41f4c8:	57                   	push   edi
  41f4c9:	e8 52 c7 01 00       	call   0x43bc20
  41f4ce:	6a ff                	push   0xffffffff
  41f4d0:	6a ff                	push   0xffffffff
  41f4d2:	6a ff                	push   0xffffffff
  41f4d4:	89 86 b8 08 00 00    	mov    DWORD PTR [esi+0x8b8],eax
  41f4da:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f4e0:	6a 02                	push   0x2
  41f4e2:	57                   	push   edi
  41f4e3:	e8 38 c7 01 00       	call   0x43bc20
  41f4e8:	6a ff                	push   0xffffffff
  41f4ea:	6a ff                	push   0xffffffff
  41f4ec:	6a ff                	push   0xffffffff
  41f4ee:	89 86 bc 08 00 00    	mov    DWORD PTR [esi+0x8bc],eax
  41f4f4:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f4fa:	6a 03                	push   0x3
  41f4fc:	57                   	push   edi
  41f4fd:	e8 1e c7 01 00       	call   0x43bc20
  41f502:	6a ff                	push   0xffffffff
  41f504:	6a ff                	push   0xffffffff
  41f506:	6a ff                	push   0xffffffff
  41f508:	89 86 c0 08 00 00    	mov    DWORD PTR [esi+0x8c0],eax
  41f50e:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f514:	6a 04                	push   0x4
  41f516:	57                   	push   edi
  41f517:	e8 04 c7 01 00       	call   0x43bc20
  41f51c:	6a ff                	push   0xffffffff
  41f51e:	6a ff                	push   0xffffffff
  41f520:	6a ff                	push   0xffffffff
  41f522:	89 86 c4 08 00 00    	mov    DWORD PTR [esi+0x8c4],eax
  41f528:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f52e:	6a 05                	push   0x5
  41f530:	57                   	push   edi
  41f531:	e8 ea c6 01 00       	call   0x43bc20
  41f536:	6a ff                	push   0xffffffff
  41f538:	6a ff                	push   0xffffffff
  41f53a:	6a ff                	push   0xffffffff
  41f53c:	89 86 c8 08 00 00    	mov    DWORD PTR [esi+0x8c8],eax
  41f542:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f548:	6a 06                	push   0x6
  41f54a:	57                   	push   edi
  41f54b:	e8 d0 c6 01 00       	call   0x43bc20
  41f550:	6a ff                	push   0xffffffff
  41f552:	6a ff                	push   0xffffffff
  41f554:	6a ff                	push   0xffffffff
  41f556:	89 86 cc 08 00 00    	mov    DWORD PTR [esi+0x8cc],eax
  41f55c:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f562:	6a 07                	push   0x7
  41f564:	57                   	push   edi
  41f565:	e8 b6 c6 01 00       	call   0x43bc20
  41f56a:	89 86 d0 08 00 00    	mov    DWORD PTR [esi+0x8d0],eax


; ===== 原版界面到RoleDefinition九次回写 0x00440C60..0x00440F60 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00440c60 <.text+0x3fc60>:
  440c60:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  440c64:	56                   	push   esi
  440c65:	85 c0                	test   eax,eax
  440c67:	8b f1                	mov    esi,ecx
  440c69:	0f 8c ef 02 00 00    	jl     0x440f5e
  440c6f:	53                   	push   ebx
  440c70:	55                   	push   ebp
  440c71:	89 86 84 05 00 00    	mov    DWORD PTR [esi+0x584],eax
  440c77:	8b 0d d4 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01d4
  440c7d:	57                   	push   edi
  440c7e:	e8 9d c1 fe ff       	call   0x42ce20
  440c83:	8b f8                	mov    edi,eax
  440c85:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440c8b:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  440c91:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440c94:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440c97:	d1 e1                	shl    ecx,1
  440c99:	2b c8                	sub    ecx,eax
  440c9b:	8b 82 a8 05 00 00    	mov    eax,DWORD PTR [edx+0x5a8]
  440ca1:	c1 e1 04             	shl    ecx,0x4
  440ca4:	8b 4c 39 54          	mov    ecx,DWORD PTR [ecx+edi*1+0x54]
  440ca8:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  440cab:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440cb1:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440cb4:	8d 14 c9             	lea    edx,[ecx+ecx*8]
  440cb7:	d1 e2                	shl    edx,1
  440cb9:	2b d0                	sub    edx,eax
  440cbb:	8b 86 7c 05 00 00    	mov    eax,DWORD PTR [esi+0x57c]
  440cc1:	c1 e2 04             	shl    edx,0x4
  440cc4:	8b 88 ac 05 00 00    	mov    ecx,DWORD PTR [eax+0x5ac]
  440cca:	8b 54 3a 58          	mov    edx,DWORD PTR [edx+edi*1+0x58]
  440cce:	89 51 44             	mov    DWORD PTR [ecx+0x44],edx
  440cd1:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440cd7:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  440cdd:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440ce0:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440ce3:	d1 e1                	shl    ecx,1
  440ce5:	2b c8                	sub    ecx,eax
  440ce7:	8b 82 b0 05 00 00    	mov    eax,DWORD PTR [edx+0x5b0]
  440ced:	c1 e1 04             	shl    ecx,0x4
  440cf0:	8b 4c 39 5c          	mov    ecx,DWORD PTR [ecx+edi*1+0x5c]
  440cf4:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  440cf7:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440cfd:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440d00:	8d 14 c9             	lea    edx,[ecx+ecx*8]
  440d03:	d1 e2                	shl    edx,1
  440d05:	2b d0                	sub    edx,eax
  440d07:	8b 86 7c 05 00 00    	mov    eax,DWORD PTR [esi+0x57c]
  440d0d:	c1 e2 04             	shl    edx,0x4
  440d10:	8b 88 b4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b4]
  440d16:	8b 54 3a 60          	mov    edx,DWORD PTR [edx+edi*1+0x60]
  440d1a:	89 51 44             	mov    DWORD PTR [ecx+0x44],edx
  440d1d:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440d23:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  440d29:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440d2c:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440d2f:	d1 e1                	shl    ecx,1
  440d31:	2b c8                	sub    ecx,eax
  440d33:	8b 82 b8 05 00 00    	mov    eax,DWORD PTR [edx+0x5b8]
  440d39:	c1 e1 04             	shl    ecx,0x4
  440d3c:	8b 4c 39 64          	mov    ecx,DWORD PTR [ecx+edi*1+0x64]
  440d40:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  440d43:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440d49:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440d4c:	8d 14 c9             	lea    edx,[ecx+ecx*8]
  440d4f:	d1 e2                	shl    edx,1
  440d51:	2b d0                	sub    edx,eax
  440d53:	8b 86 7c 05 00 00    	mov    eax,DWORD PTR [esi+0x57c]
  440d59:	c1 e2 04             	shl    edx,0x4
  440d5c:	8b 88 bc 05 00 00    	mov    ecx,DWORD PTR [eax+0x5bc]
  440d62:	8b 54 3a 50          	mov    edx,DWORD PTR [edx+edi*1+0x50]
  440d66:	89 51 44             	mov    DWORD PTR [ecx+0x44],edx
  440d69:	8b ce                	mov    ecx,esi
  440d6b:	e8 80 fd ff ff       	call   0x440af0
  440d70:	bb 80 05 00 00       	mov    ebx,0x580
  440d75:	8d ae a0 05 00 00    	lea    ebp,[esi+0x5a0]
  440d7b:	8b 8e 80 05 00 00    	mov    ecx,DWORD PTR [esi+0x580]
  440d81:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  440d84:	85 c0                	test   eax,eax
  440d86:	8b 14 0b             	mov    edx,DWORD PTR [ebx+ecx*1]
  440d89:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  440d8c:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  440d8f:	7f 13                	jg     0x440da4
  440d91:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440d97:	68 e0 a9 46 00       	push   0x46a9e0
  440d9c:	8b 0c 03             	mov    ecx,DWORD PTR [ebx+eax*1]
  440d9f:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  440da2:	eb 11                	jmp    0x440db5
  440da4:	8b 96 80 05 00 00    	mov    edx,DWORD PTR [esi+0x580]
  440daa:	68 cc a9 46 00       	push   0x46a9cc
  440daf:	8b 04 13             	mov    eax,DWORD PTR [ebx+edx*1]
  440db2:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  440db5:	e8 a6 d6 ff ff       	call   0x43e460
  440dba:	83 c3 08             	add    ebx,0x8
  440dbd:	83 c5 08             	add    ebp,0x8
  440dc0:	81 fb c0 05 00 00    	cmp    ebx,0x5c0
  440dc6:	7c b3                	jl     0x440d7b
  440dc8:	b8 a8 05 00 00       	mov    eax,0x5a8
  440dcd:	8b 8e 7c 05 00 00    	mov    ecx,DWORD PTR [esi+0x57c]
  440dd3:	83 c0 04             	add    eax,0x4
  440dd6:	3d bc 05 00 00       	cmp    eax,0x5bc
  440ddb:	8b 54 01 fc          	mov    edx,DWORD PTR [ecx+eax*1-0x4]
  440ddf:	8b 4a 44             	mov    ecx,DWORD PTR [edx+0x44]
  440de2:	89 4c 06 dc          	mov    DWORD PTR [esi+eax*1-0x24],ecx
  440de6:	7c e5                	jl     0x440dcd
  440de8:	8b ce                	mov    ecx,esi
  440dea:	e8 01 fd ff ff       	call   0x440af0
  440def:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  440df5:	c6 82 cb 05 00 00 00 	mov    BYTE PTR [edx+0x5cb],0x0
  440dfc:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e02:	8b 90 80 05 00 00    	mov    edx,DWORD PTR [eax+0x580]
  440e08:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440e0e:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440e11:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440e14:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440e17:	d1 e1                	shl    ecx,1
  440e19:	2b c8                	sub    ecx,eax
  440e1b:	c1 e1 04             	shl    ecx,0x4
  440e1e:	89 54 39 68          	mov    DWORD PTR [ecx+edi*1+0x68],edx
  440e22:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e28:	8b 90 88 05 00 00    	mov    edx,DWORD PTR [eax+0x588]
  440e2e:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440e34:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440e37:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440e3a:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440e3d:	d1 e1                	shl    ecx,1
  440e3f:	2b c8                	sub    ecx,eax
  440e41:	c1 e1 04             	shl    ecx,0x4
  440e44:	89 54 39 6c          	mov    DWORD PTR [ecx+edi*1+0x6c],edx
  440e48:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e4e:	8b 90 8c 05 00 00    	mov    edx,DWORD PTR [eax+0x58c]
  440e54:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440e5a:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440e5d:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440e60:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440e63:	d1 e1                	shl    ecx,1
  440e65:	2b c8                	sub    ecx,eax
  440e67:	c1 e1 04             	shl    ecx,0x4
  440e6a:	89 54 39 70          	mov    DWORD PTR [ecx+edi*1+0x70],edx
  440e6e:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e74:	8b 90 90 05 00 00    	mov    edx,DWORD PTR [eax+0x590]
  440e7a:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440e80:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440e83:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440e86:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440e89:	d1 e1                	shl    ecx,1
  440e8b:	2b c8                	sub    ecx,eax
  440e8d:	c1 e1 04             	shl    ecx,0x4
  440e90:	89 54 39 74          	mov    DWORD PTR [ecx+edi*1+0x74],edx
  440e94:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e9a:	8b 90 98 05 00 00    	mov    edx,DWORD PTR [eax+0x598]
  440ea0:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440ea6:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440ea9:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440eac:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440eaf:	d1 e1                	shl    ecx,1
  440eb1:	2b c8                	sub    ecx,eax
  440eb3:	c1 e1 04             	shl    ecx,0x4
  440eb6:	89 54 39 78          	mov    DWORD PTR [ecx+edi*1+0x78],edx
  440eba:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440ec0:	8b 90 a0 05 00 00    	mov    edx,DWORD PTR [eax+0x5a0]
  440ec6:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440ecc:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440ecf:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440ed2:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440ed5:	d1 e1                	shl    ecx,1
  440ed7:	2b c8                	sub    ecx,eax
  440ed9:	c1 e1 04             	shl    ecx,0x4
  440edc:	89 54 39 7c          	mov    DWORD PTR [ecx+edi*1+0x7c],edx
  440ee0:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440ee6:	8b 90 a8 05 00 00    	mov    edx,DWORD PTR [eax+0x5a8]
  440eec:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440ef2:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440ef5:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440ef8:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440efb:	d1 e1                	shl    ecx,1
  440efd:	2b c8                	sub    ecx,eax
  440eff:	c1 e1 04             	shl    ecx,0x4
  440f02:	89 94 39 80 00 00 00 	mov    DWORD PTR [ecx+edi*1+0x80],edx
  440f09:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440f0f:	8b 90 b0 05 00 00    	mov    edx,DWORD PTR [eax+0x5b0]
  440f15:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440f1b:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440f1e:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440f21:	d1 e1                	shl    ecx,1
  440f23:	2b c8                	sub    ecx,eax
  440f25:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440f28:	c1 e1 04             	shl    ecx,0x4
  440f2b:	89 94 39 84 00 00 00 	mov    DWORD PTR [ecx+edi*1+0x84],edx
  440f32:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440f38:	8b b6 84 05 00 00    	mov    esi,DWORD PTR [esi+0x584]
  440f3e:	8b 88 b8 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b8]
  440f44:	8d 04 76             	lea    eax,[esi+esi*2]
  440f47:	8d 14 c0             	lea    edx,[eax+eax*8]
  440f4a:	8b 41 44             	mov    eax,DWORD PTR [ecx+0x44]
  440f4d:	d1 e2                	shl    edx,1
  440f4f:	2b d6                	sub    edx,esi
  440f51:	c1 e2 04             	shl    edx,0x4
  440f54:	89 84 3a 88 00 00 00 	mov    DWORD PTR [edx+edi*1+0x88],eax
  440f5b:	5f                   	pop    edi
  440f5c:	5d                   	pop    ebp
  440f5d:	5b                   	pop    ebx
  440f5e:	5e                   	pop    esi
  440f5f:	c2                   	.byte 0xc2


; ===== 研究版界面到RoleDefinition回写 0x00440C60..0x00440F60 =====

/workspace/scratch/3f7398360f84/upload/RPG.exe:     file format pei-i386


Disassembly of section .text:

00440c60 <.text+0x3fc60>:
  440c60:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  440c64:	56                   	push   esi
  440c65:	85 c0                	test   eax,eax
  440c67:	8b f1                	mov    esi,ecx
  440c69:	0f 8c ef 02 00 00    	jl     0x440f5e
  440c6f:	53                   	push   ebx
  440c70:	55                   	push   ebp
  440c71:	89 86 84 05 00 00    	mov    DWORD PTR [esi+0x584],eax
  440c77:	8b 0d d4 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01d4
  440c7d:	57                   	push   edi
  440c7e:	e8 9d c1 fe ff       	call   0x42ce20
  440c83:	8b f8                	mov    edi,eax
  440c85:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440c8b:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  440c91:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440c94:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440c97:	d1 e1                	shl    ecx,1
  440c99:	2b c8                	sub    ecx,eax
  440c9b:	8b 82 a8 05 00 00    	mov    eax,DWORD PTR [edx+0x5a8]
  440ca1:	c1 e1 04             	shl    ecx,0x4
  440ca4:	8b 4c 39 54          	mov    ecx,DWORD PTR [ecx+edi*1+0x54]
  440ca8:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  440cab:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440cb1:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440cb4:	8d 14 c9             	lea    edx,[ecx+ecx*8]
  440cb7:	d1 e2                	shl    edx,1
  440cb9:	2b d0                	sub    edx,eax
  440cbb:	8b 86 7c 05 00 00    	mov    eax,DWORD PTR [esi+0x57c]
  440cc1:	c1 e2 04             	shl    edx,0x4
  440cc4:	8b 88 ac 05 00 00    	mov    ecx,DWORD PTR [eax+0x5ac]
  440cca:	8b 54 3a 58          	mov    edx,DWORD PTR [edx+edi*1+0x58]
  440cce:	89 51 44             	mov    DWORD PTR [ecx+0x44],edx
  440cd1:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440cd7:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  440cdd:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440ce0:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440ce3:	d1 e1                	shl    ecx,1
  440ce5:	2b c8                	sub    ecx,eax
  440ce7:	8b 82 b0 05 00 00    	mov    eax,DWORD PTR [edx+0x5b0]
  440ced:	c1 e1 04             	shl    ecx,0x4
  440cf0:	8b 4c 39 5c          	mov    ecx,DWORD PTR [ecx+edi*1+0x5c]
  440cf4:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  440cf7:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440cfd:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440d00:	8d 14 c9             	lea    edx,[ecx+ecx*8]
  440d03:	d1 e2                	shl    edx,1
  440d05:	2b d0                	sub    edx,eax
  440d07:	8b 86 7c 05 00 00    	mov    eax,DWORD PTR [esi+0x57c]
  440d0d:	c1 e2 04             	shl    edx,0x4
  440d10:	8b 88 b4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b4]
  440d16:	8b 54 3a 60          	mov    edx,DWORD PTR [edx+edi*1+0x60]
  440d1a:	89 51 44             	mov    DWORD PTR [ecx+0x44],edx
  440d1d:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440d23:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  440d29:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440d2c:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440d2f:	d1 e1                	shl    ecx,1
  440d31:	2b c8                	sub    ecx,eax
  440d33:	8b 82 b8 05 00 00    	mov    eax,DWORD PTR [edx+0x5b8]
  440d39:	c1 e1 04             	shl    ecx,0x4
  440d3c:	8b 4c 39 64          	mov    ecx,DWORD PTR [ecx+edi*1+0x64]
  440d40:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  440d43:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440d49:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440d4c:	8d 14 c9             	lea    edx,[ecx+ecx*8]
  440d4f:	d1 e2                	shl    edx,1
  440d51:	2b d0                	sub    edx,eax
  440d53:	8b 86 7c 05 00 00    	mov    eax,DWORD PTR [esi+0x57c]
  440d59:	c1 e2 04             	shl    edx,0x4
  440d5c:	8b 88 bc 05 00 00    	mov    ecx,DWORD PTR [eax+0x5bc]
  440d62:	8b 54 3a 50          	mov    edx,DWORD PTR [edx+edi*1+0x50]
  440d66:	89 51 44             	mov    DWORD PTR [ecx+0x44],edx
  440d69:	8b ce                	mov    ecx,esi
  440d6b:	e8 80 fd ff ff       	call   0x440af0
  440d70:	bb 80 05 00 00       	mov    ebx,0x580
  440d75:	8d ae a0 05 00 00    	lea    ebp,[esi+0x5a0]
  440d7b:	8b 8e 80 05 00 00    	mov    ecx,DWORD PTR [esi+0x580]
  440d81:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  440d84:	85 c0                	test   eax,eax
  440d86:	8b 14 0b             	mov    edx,DWORD PTR [ebx+ecx*1]
  440d89:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  440d8c:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  440d8f:	7f 13                	jg     0x440da4
  440d91:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440d97:	68 e0 a9 46 00       	push   0x46a9e0
  440d9c:	8b 0c 03             	mov    ecx,DWORD PTR [ebx+eax*1]
  440d9f:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  440da2:	eb 11                	jmp    0x440db5
  440da4:	8b 96 80 05 00 00    	mov    edx,DWORD PTR [esi+0x580]
  440daa:	68 cc a9 46 00       	push   0x46a9cc
  440daf:	8b 04 13             	mov    eax,DWORD PTR [ebx+edx*1]
  440db2:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  440db5:	e8 a6 d6 ff ff       	call   0x43e460
  440dba:	83 c3 08             	add    ebx,0x8
  440dbd:	83 c5 08             	add    ebp,0x8
  440dc0:	81 fb c0 05 00 00    	cmp    ebx,0x5c0
  440dc6:	7c b3                	jl     0x440d7b
  440dc8:	b8 a8 05 00 00       	mov    eax,0x5a8
  440dcd:	8b 8e 7c 05 00 00    	mov    ecx,DWORD PTR [esi+0x57c]
  440dd3:	83 c0 04             	add    eax,0x4
  440dd6:	3d bc 05 00 00       	cmp    eax,0x5bc
  440ddb:	8b 54 01 fc          	mov    edx,DWORD PTR [ecx+eax*1-0x4]
  440ddf:	8b 4a 44             	mov    ecx,DWORD PTR [edx+0x44]
  440de2:	89 4c 06 dc          	mov    DWORD PTR [esi+eax*1-0x24],ecx
  440de6:	7c e5                	jl     0x440dcd
  440de8:	8b ce                	mov    ecx,esi
  440dea:	e8 01 fd ff ff       	call   0x440af0
  440def:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  440df5:	c6 82 cb 05 00 00 00 	mov    BYTE PTR [edx+0x5cb],0x0
  440dfc:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e02:	8b 90 80 05 00 00    	mov    edx,DWORD PTR [eax+0x580]
  440e08:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440e0e:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440e11:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440e14:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440e17:	d1 e1                	shl    ecx,1
  440e19:	2b c8                	sub    ecx,eax
  440e1b:	c1 e1 04             	shl    ecx,0x4
  440e1e:	89 54 39 68          	mov    DWORD PTR [ecx+edi*1+0x68],edx
  440e22:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e28:	8b 90 88 05 00 00    	mov    edx,DWORD PTR [eax+0x588]
  440e2e:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440e34:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440e37:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440e3a:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440e3d:	d1 e1                	shl    ecx,1
  440e3f:	2b c8                	sub    ecx,eax
  440e41:	c1 e1 04             	shl    ecx,0x4
  440e44:	89 54 39 6c          	mov    DWORD PTR [ecx+edi*1+0x6c],edx
  440e48:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e4e:	8b 90 8c 05 00 00    	mov    edx,DWORD PTR [eax+0x58c]
  440e54:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440e5a:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440e5d:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440e60:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440e63:	d1 e1                	shl    ecx,1
  440e65:	2b c8                	sub    ecx,eax
  440e67:	c1 e1 04             	shl    ecx,0x4
  440e6a:	89 54 39 70          	mov    DWORD PTR [ecx+edi*1+0x70],edx
  440e6e:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e74:	8b 90 90 05 00 00    	mov    edx,DWORD PTR [eax+0x590]
  440e7a:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440e80:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440e83:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440e86:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440e89:	d1 e1                	shl    ecx,1
  440e8b:	2b c8                	sub    ecx,eax
  440e8d:	c1 e1 04             	shl    ecx,0x4
  440e90:	89 54 39 70          	mov    DWORD PTR [ecx+edi*1+0x70],edx
  440e94:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440e9a:	8b 90 98 05 00 00    	mov    edx,DWORD PTR [eax+0x598]
  440ea0:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440ea6:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440ea9:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440eac:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440eaf:	d1 e1                	shl    ecx,1
  440eb1:	2b c8                	sub    ecx,eax
  440eb3:	c1 e1 04             	shl    ecx,0x4
  440eb6:	89 54 39 74          	mov    DWORD PTR [ecx+edi*1+0x74],edx
  440eba:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440ec0:	8b 90 a0 05 00 00    	mov    edx,DWORD PTR [eax+0x5a0]
  440ec6:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440ecc:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440ecf:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440ed2:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440ed5:	d1 e1                	shl    ecx,1
  440ed7:	2b c8                	sub    ecx,eax
  440ed9:	c1 e1 04             	shl    ecx,0x4
  440edc:	89 54 39 78          	mov    DWORD PTR [ecx+edi*1+0x78],edx
  440ee0:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440ee6:	8b 90 a8 05 00 00    	mov    edx,DWORD PTR [eax+0x5a8]
  440eec:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440ef2:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440ef5:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440ef8:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440efb:	d1 e1                	shl    ecx,1
  440efd:	2b c8                	sub    ecx,eax
  440eff:	c1 e1 04             	shl    ecx,0x4
  440f02:	89 94 39 7c 00 00 00 	mov    DWORD PTR [ecx+edi*1+0x7c],edx
  440f09:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440f0f:	8b 90 b0 05 00 00    	mov    edx,DWORD PTR [eax+0x5b0]
  440f15:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  440f1b:	8d 0c 40             	lea    ecx,[eax+eax*2]
  440f1e:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  440f21:	d1 e1                	shl    ecx,1
  440f23:	2b c8                	sub    ecx,eax
  440f25:	8b 52 44             	mov    edx,DWORD PTR [edx+0x44]
  440f28:	c1 e1 04             	shl    ecx,0x4
  440f2b:	89 94 39 80 00 00 00 	mov    DWORD PTR [ecx+edi*1+0x80],edx
  440f32:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  440f38:	8b b6 84 05 00 00    	mov    esi,DWORD PTR [esi+0x584]
  440f3e:	8b 88 b8 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b8]
  440f44:	8d 04 76             	lea    eax,[esi+esi*2]
  440f47:	8d 14 c0             	lea    edx,[eax+eax*8]
  440f4a:	8b 41 44             	mov    eax,DWORD PTR [ecx+0x44]
  440f4d:	d1 e2                	shl    edx,1
  440f4f:	2b d6                	sub    edx,esi
  440f51:	c1 e2 04             	shl    edx,0x4
  440f54:	89 84 3a 84 00 00 00 	mov    DWORD PTR [edx+edi*1+0x84],eax
  440f5b:	5f                   	pop    edi
  440f5c:	5d                   	pop    ebp
  440f5d:	5b                   	pop    ebx
  440f5e:	5e                   	pop    esi
  440f5f:	c2                   	.byte 0xc2

