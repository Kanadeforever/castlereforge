; 固化68补充直接反汇编证据。由当前工具从已校验输入只读生成。
; 业务结论仍以机器JSON和完整接档说明中的证据边界为准。

; ===== 原版盘符定位 0x00402A10..0x00402AA0 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00402a10 <.text+0x1a10>:
  402a10:	83 ec 38             	sub    esp,0x38
  402a13:	53                   	push   ebx
  402a14:	55                   	push   ebp
  402a15:	8b 2d 24 01 46 00    	mov    ebp,DWORD PTR ds:0x460124
  402a1b:	56                   	push   esi
  402a1c:	57                   	push   edi
  402a1d:	8b 3d 94 01 46 00    	mov    edi,DWORD PTR ds:0x460194
  402a23:	b3 41                	mov    bl,0x41
  402a25:	0f be f3             	movsx  esi,bl
  402a28:	56                   	push   esi
  402a29:	8d 44 24 18          	lea    eax,[esp+0x18]
  402a2d:	68 c4 83 46 00       	push   0x4683c4
  402a32:	50                   	push   eax
  402a33:	ff d7                	call   edi
  402a35:	83 c4 0c             	add    esp,0xc
  402a38:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  402a3c:	51                   	push   ecx
  402a3d:	ff d5                	call   ebp
  402a3f:	83 f8 05             	cmp    eax,0x5
  402a42:	75 20                	jne    0x402a64
  402a44:	56                   	push   esi
  402a45:	8d 54 24 18          	lea    edx,[esp+0x18]
  402a49:	68 b0 83 46 00       	push   0x4683b0
  402a4e:	52                   	push   edx
  402a4f:	ff d7                	call   edi
  402a51:	8d 44 24 20          	lea    eax,[esp+0x20]
  402a55:	6a 00                	push   0x0
  402a57:	50                   	push   eax
  402a58:	e8 81 a2 05 00       	call   0x45ccde
  402a5d:	83 c4 14             	add    esp,0x14
  402a60:	85 c0                	test   eax,eax
  402a62:	74 2e                	je     0x402a92
  402a64:	fe c3                	inc    bl
  402a66:	80 fb 5b             	cmp    bl,0x5b
  402a69:	7c ba                	jl     0x402a25
  402a6b:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  402a71:	68 9c 83 46 00       	push   0x46839c
  402a76:	e8 f5 f4 02 00       	call   0x431f70
  402a7b:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  402a7f:	68 d8 37 46 00       	push   0x4637d8
  402a84:	51                   	push   ecx
  402a85:	c7 44 24 18 70 83 46 	mov    DWORD PTR [esp+0x18],0x468370
  402a8c:	00 
  402a8d:	e8 35 f1 04 00       	call   0x451bc7
  402a92:	5f                   	pop    edi
  402a93:	5e                   	pop    esi
  402a94:	88 1d c8 f5 46 00    	mov    BYTE PTR ds:0x46f5c8,bl
  402a9a:	5d                   	pop    ebp
  402a9b:	5b                   	pop    ebx
  402a9c:	83 c4 38             	add    esp,0x38
  402a9f:	c3                   	ret


; ===== 原版GetFileAttributesA包装helper 0x0045CCDE..0x0045CD22 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0045ccde <.text+0x5bcde>:
  45ccde:	ff 74 24 04          	push   DWORD PTR [esp+0x4]
  45cce2:	ff 15 cc 00 46 00    	call   DWORD PTR ds:0x4600cc
  45cce8:	83 f8 ff             	cmp    eax,0xffffffff
  45cceb:	75 11                	jne    0x45ccfe
  45cced:	ff 15 d4 00 46 00    	call   DWORD PTR ds:0x4600d4
  45ccf3:	50                   	push   eax
  45ccf4:	e8 04 a3 ff ff       	call   0x456ffd
  45ccf9:	59                   	pop    ecx
  45ccfa:	83 c8 ff             	or     eax,0xffffffff
  45ccfd:	c3                   	ret
  45ccfe:	a8 01                	test   al,0x1
  45cd00:	74 1d                	je     0x45cd1f
  45cd02:	f6 44 24 08 02       	test   BYTE PTR [esp+0x8],0x2
  45cd07:	74 16                	je     0x45cd1f
  45cd09:	c7 05 c8 1f 98 00 0d 	mov    DWORD PTR ds:0x981fc8,0xd
  45cd10:	00 00 00 
  45cd13:	c7 05 cc 1f 98 00 05 	mov    DWORD PTR ds:0x981fcc,0x5
  45cd1a:	00 00 00 
  45cd1d:	eb db                	jmp    0x45ccfa
  45cd1f:	33 c0                	xor    eax,eax
  45cd21:	c3                   	ret


; ===== 研究版盘符定位差异 0x00402A10..0x00402AA0 =====

/workspace/scratch/3f7398360f84/upload/RPG.exe:     file format pei-i386


Disassembly of section .text:

00402a10 <.text+0x1a10>:
  402a10:	83 ec 38             	sub    esp,0x38
  402a13:	53                   	push   ebx
  402a14:	55                   	push   ebp
  402a15:	8b 2d 24 01 46 00    	mov    ebp,DWORD PTR ds:0x460124
  402a1b:	56                   	push   esi
  402a1c:	57                   	push   edi
  402a1d:	8b 3d 94 01 46 00    	mov    edi,DWORD PTR ds:0x460194
  402a23:	b3 41                	mov    bl,0x41
  402a25:	0f be f3             	movsx  esi,bl
  402a28:	56                   	push   esi
  402a29:	8d 44 24 18          	lea    eax,[esp+0x18]
  402a2d:	68 c4 83 46 00       	push   0x4683c4
  402a32:	50                   	push   eax
  402a33:	ff d7                	call   edi
  402a35:	83 c4 0c             	add    esp,0xc
  402a38:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  402a3c:	51                   	push   ecx
  402a3d:	ff d5                	call   ebp
  402a3f:	83 f8 02             	cmp    eax,0x2
  402a42:	7c 20                	jl     0x402a64
  402a44:	56                   	push   esi
  402a45:	8d 54 24 18          	lea    edx,[esp+0x18]
  402a49:	68 b0 83 46 00       	push   0x4683b0
  402a4e:	52                   	push   edx
  402a4f:	ff d7                	call   edi
  402a51:	8d 44 24 20          	lea    eax,[esp+0x20]
  402a55:	6a 00                	push   0x0
  402a57:	50                   	push   eax
  402a58:	e8 81 a2 05 00       	call   0x45ccde
  402a5d:	83 c4 14             	add    esp,0x14
  402a60:	85 c0                	test   eax,eax
  402a62:	eb 2e                	jmp    0x402a92
  402a64:	fe c3                	inc    bl
  402a66:	80 fb 5b             	cmp    bl,0x5b
  402a69:	7c ba                	jl     0x402a25
  402a6b:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  402a71:	68 9c 83 46 00       	push   0x46839c
  402a76:	e8 f5 f4 02 00       	call   0x431f70
  402a7b:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  402a7f:	68 d8 37 46 00       	push   0x4637d8
  402a84:	51                   	push   ecx
  402a85:	c7 44 24 18 70 83 46 	mov    DWORD PTR [esp+0x18],0x468370
  402a8c:	00 
  402a8d:	e8 35 f1 04 00       	call   0x451bc7
  402a92:	5f                   	pop    edi
  402a93:	5e                   	pop    esi
  402a94:	88 1d c8 f5 46 00    	mov    BYTE PTR ds:0x46f5c8,bl
  402a9a:	5d                   	pop    ebp
  402a9b:	5b                   	pop    ebx
  402a9c:	83 c4 38             	add    esp,0x38
  402a9f:	c3                   	ret

