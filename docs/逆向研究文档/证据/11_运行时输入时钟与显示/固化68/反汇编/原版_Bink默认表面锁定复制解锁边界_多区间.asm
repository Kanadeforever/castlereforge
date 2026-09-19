; 固化68补充直接反汇编证据。由当前工具从已校验输入只读生成。
; 业务结论仍以机器JSON和完整接档说明中的证据边界为准。

; ===== Bink逐帧复制 0x00401BA0..0x00401C49 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401ba0 <.text+0xba0>:
  401ba0:	56                   	push   esi
  401ba1:	8b f1                	mov    esi,ecx
  401ba3:	8a 46 08             	mov    al,BYTE PTR [esi+0x8]
  401ba6:	84 c0                	test   al,al
  401ba8:	75 06                	jne    0x401bb0
  401baa:	32 c0                	xor    al,al
  401bac:	5e                   	pop    esi
  401bad:	c2 08 00             	ret    0x8
  401bb0:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  401bb3:	50                   	push   eax
  401bb4:	ff 15 48 02 46 00    	call   DWORD PTR ds:0x460248
  401bba:	85 c0                	test   eax,eax
  401bbc:	0f 85 81 00 00 00    	jne    0x401c43
  401bc2:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  401bc5:	57                   	push   edi
  401bc6:	51                   	push   ecx
  401bc7:	ff 15 50 02 46 00    	call   DWORD PTR ds:0x460250
  401bcd:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401bd3:	bf 09 00 00 00       	mov    edi,0x9
  401bd8:	83 79 3c 10          	cmp    DWORD PTR [ecx+0x3c],0x10
  401bdc:	75 05                	jne    0x401be3
  401bde:	bf 0a 00 00 00       	mov    edi,0xa
  401be3:	6a 00                	push   0x0
  401be5:	e8 46 46 00 00       	call   0x406230
  401bea:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  401bee:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  401bf2:	57                   	push   edi
  401bf3:	52                   	push   edx
  401bf4:	51                   	push   ecx
  401bf5:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401bfb:	8b 79 28             	mov    edi,DWORD PTR [ecx+0x28]
  401bfe:	8b 51 70             	mov    edx,DWORD PTR [ecx+0x70]
  401c01:	8b 49 40             	mov    ecx,DWORD PTR [ecx+0x40]
  401c04:	8b 7f 08             	mov    edi,DWORD PTR [edi+0x8]
  401c07:	8d 14 57             	lea    edx,[edi+edx*2]
  401c0a:	52                   	push   edx
  401c0b:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  401c0e:	51                   	push   ecx
  401c0f:	50                   	push   eax
  401c10:	52                   	push   edx
  401c11:	ff 15 4c 02 46 00    	call   DWORD PTR ds:0x46024c
  401c17:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401c1d:	6a 00                	push   0x0
  401c1f:	e8 9c 46 00 00       	call   0x4062c0
  401c24:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  401c27:	5f                   	pop    edi
  401c28:	8b 48 0c             	mov    ecx,DWORD PTR [eax+0xc]
  401c2b:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  401c2e:	3b ca                	cmp    ecx,edx
  401c30:	75 0a                	jne    0x401c3c
  401c32:	c6 46 0a 00          	mov    BYTE PTR [esi+0xa],0x0
  401c36:	32 c0                	xor    al,al
  401c38:	5e                   	pop    esi
  401c39:	c2 08 00             	ret    0x8
  401c3c:	50                   	push   eax
  401c3d:	ff 15 38 02 46 00    	call   DWORD PTR ds:0x460238
  401c43:	b0 01                	mov    al,0x1
  401c45:	5e                   	pop    esi
  401c46:	c2 08 00             	ret    0x8


; ===== 表面锁定解锁helper 0x00406230..0x004062D9 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00406230 <.text+0x5230>:
  406230:	83 ec 7c             	sub    esp,0x7c
  406233:	53                   	push   ebx
  406234:	55                   	push   ebp
  406235:	8b ac 24 88 00 00 00 	mov    ebp,DWORD PTR [esp+0x88]
  40623c:	56                   	push   esi
  40623d:	85 ed                	test   ebp,ebp
  40623f:	57                   	push   edi
  406240:	75 03                	jne    0x406245
  406242:	8b 69 04             	mov    ebp,DWORD PTR [ecx+0x4]
  406245:	b9 1f 00 00 00       	mov    ecx,0x1f
  40624a:	33 c0                	xor    eax,eax
  40624c:	8d 7c 24 10          	lea    edi,[esp+0x10]
  406250:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  406252:	33 ff                	xor    edi,edi
  406254:	c7 44 24 10 7c 00 00 	mov    DWORD PTR [esp+0x10],0x7c
  40625b:	00 
  40625c:	32 db                	xor    bl,bl
  40625e:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  406261:	6a 00                	push   0x0
  406263:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  406267:	6a 00                	push   0x0
  406269:	51                   	push   ecx
  40626a:	6a 00                	push   0x0
  40626c:	55                   	push   ebp
  40626d:	ff 50 64             	call   DWORD PTR [eax+0x64]
  406270:	8b f0                	mov    esi,eax
  406272:	81 fe c2 01 76 88    	cmp    esi,0x887601c2
  406278:	75 09                	jne    0x406283
  40627a:	8b 55 00             	mov    edx,DWORD PTR [ebp+0x0]
  40627d:	55                   	push   ebp
  40627e:	ff 52 6c             	call   DWORD PTR [edx+0x6c]
  406281:	eb 06                	jmp    0x406289
  406283:	85 f6                	test   esi,esi
  406285:	75 02                	jne    0x406289
  406287:	b3 01                	mov    bl,0x1
  406289:	47                   	inc    edi
  40628a:	81 ff 00 01 00 00    	cmp    edi,0x100
  406290:	74 1b                	je     0x4062ad
  406292:	81 fe 1c 02 76 88    	cmp    esi,0x8876021c
  406298:	75 01                	jne    0x40629b
  40629a:	4f                   	dec    edi
  40629b:	84 db                	test   bl,bl
  40629d:	74 bf                	je     0x40625e
  40629f:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  4062a3:	5f                   	pop    edi
  4062a4:	5e                   	pop    esi
  4062a5:	5d                   	pop    ebp
  4062a6:	5b                   	pop    ebx
  4062a7:	83 c4 7c             	add    esp,0x7c
  4062aa:	c2 04 00             	ret    0x4
  4062ad:	5f                   	pop    edi
  4062ae:	5e                   	pop    esi
  4062af:	5d                   	pop    ebp
  4062b0:	33 c0                	xor    eax,eax
  4062b2:	5b                   	pop    ebx
  4062b3:	83 c4 7c             	add    esp,0x7c
  4062b6:	c2 04 00             	ret    0x4
  4062b9:	90                   	nop
  4062ba:	90                   	nop
  4062bb:	90                   	nop
  4062bc:	90                   	nop
  4062bd:	90                   	nop
  4062be:	90                   	nop
  4062bf:	90                   	nop
  4062c0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4062c4:	85 c0                	test   eax,eax
  4062c6:	75 03                	jne    0x4062cb
  4062c8:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  4062cb:	8b 08                	mov    ecx,DWORD PTR [eax]
  4062cd:	6a 00                	push   0x0
  4062cf:	50                   	push   eax
  4062d0:	ff 91 80 00 00 00    	call   DWORD PTR [ecx+0x80]
  4062d6:	c2 04 00             	ret    0x4

