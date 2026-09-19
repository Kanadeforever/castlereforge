; v0.7D阶段中-固化3 D6 DamageInfo+0x44 状态传播后处理直接证据
; Oracle RPG.exe SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; 范围A: 0x0042150C..0x004215560 —— DamageInfo+0x44 的低字节作为 0x420E20 参数，返回真时两个结果分量被有符号除2。
; 范围B: 0x00420E20..0x00420EC0 —— 参数非零时检查与 BattleUnit+0xB9C、相关单位存活状态及关联对象 byte+0x358 有关的条件。
; 证据边界: 只冻结控制流、参数传播和整数后处理；不推断该 flag 的作者业务名或视觉/战斗语义。


/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042150c <.text+0x2050c>:
  42150c:	8a 43 fc             	mov    al,BYTE PTR [ebx-0x4]
  42150f:	8b 2b                	mov    ebp,DWORD PTR [ebx]
  421511:	8b 7b 04             	mov    edi,DWORD PTR [ebx+0x4]
  421514:	84 c0                	test   al,al
  421516:	74 48                	je     0x421560
  421518:	8a 4b 44             	mov    cl,BYTE PTR [ebx+0x44]
  42151b:	51                   	push   ecx
  42151c:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  421520:	56                   	push   esi
  421521:	e8 fa f8 ff ff       	call   0x420e20
  421526:	84 c0                	test   al,al
  421528:	74 36                	je     0x421560
  42152a:	85 ed                	test   ebp,ebp
  42152c:	74 17                	je     0x421545
  42152e:	33 c0                	xor    eax,eax
  421530:	85 ed                	test   ebp,ebp
  421532:	0f 9e c0             	setle  al
  421535:	48                   	dec    eax
  421536:	83 e0 04             	and    eax,0x4
  421539:	83 c0 fe             	add    eax,0xfffffffe
  42153c:	03 c5                	add    eax,ebp
  42153e:	99                   	cdq
  42153f:	2b c2                	sub    eax,edx
  421541:	d1 f8                	sar    eax,1
  421543:	8b e8                	mov    ebp,eax
  421545:	85 ff                	test   edi,edi
  421547:	74 17                	je     0x421560
  421549:	33 c0                	xor    eax,eax
  42154b:	85 ff                	test   edi,edi
  42154d:	0f 9e c0             	setle  al
  421550:	48                   	dec    eax
  421551:	83 e0 04             	and    eax,0x4
  421554:	83 c0 fe             	add    eax,0xfffffffe
  421557:	03 c7                	add    eax,edi
  421559:	99                   	cdq
  42155a:	2b c2                	sub    eax,edx
  42155c:	d1 f8                	sar    eax,1
  42155e:	8b f8                	mov    edi,eax

; ===== flag consumer 0x420E20 =====


/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420e20 <.text+0x1fe20>:
  420e20:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  420e24:	53                   	push   ebx
  420e25:	32 db                	xor    bl,bl
  420e27:	8b 14 85 94 fd 89 00 	mov    edx,DWORD PTR [eax*4+0x89fd94]
  420e2e:	8b 82 9c 0b 00 00    	mov    eax,DWORD PTR [edx+0xb9c]
  420e34:	8a 54 24 0c          	mov    dl,BYTE PTR [esp+0xc]
  420e38:	84 d2                	test   dl,dl
  420e3a:	74 7f                	je     0x420ebb
  420e3c:	83 f8 03             	cmp    eax,0x3
  420e3f:	7f 3d                	jg     0x420e7e
  420e41:	85 c0                	test   eax,eax
  420e43:	7c 39                	jl     0x420e7e
  420e45:	83 c0 04             	add    eax,0x4
  420e48:	50                   	push   eax
  420e49:	e8 22 1c 00 00       	call   0x422a70
  420e4e:	85 c0                	test   eax,eax
  420e50:	7c 69                	jl     0x420ebb
  420e52:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  420e59:	8b 88 2c 07 00 00    	mov    ecx,DWORD PTR [eax+0x72c]
  420e5f:	85 c9                	test   ecx,ecx
  420e61:	7c 58                	jl     0x420ebb
  420e63:	8b 80 a4 0b 00 00    	mov    eax,DWORD PTR [eax+0xba4]
  420e69:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  420e6c:	8a 81 58 03 00 00    	mov    al,BYTE PTR [ecx+0x358]
  420e72:	84 c0                	test   al,al
  420e74:	74 45                	je     0x420ebb
  420e76:	b3 01                	mov    bl,0x1
  420e78:	8a c3                	mov    al,bl
  420e7a:	5b                   	pop    ebx
  420e7b:	c2 08 00             	ret    0x8
  420e7e:	83 f8 18             	cmp    eax,0x18
  420e81:	7c 38                	jl     0x420ebb
  420e83:	83 f8 1b             	cmp    eax,0x1b
  420e86:	7f 33                	jg     0x420ebb
  420e88:	83 c0 fc             	add    eax,0xfffffffc
  420e8b:	50                   	push   eax
  420e8c:	e8 df 1b 00 00       	call   0x422a70
  420e91:	85 c0                	test   eax,eax
  420e93:	7c 26                	jl     0x420ebb
  420e95:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  420e9c:	8b 88 2c 07 00 00    	mov    ecx,DWORD PTR [eax+0x72c]
  420ea2:	85 c9                	test   ecx,ecx
  420ea4:	7c 15                	jl     0x420ebb
  420ea6:	8b 90 a4 0b 00 00    	mov    edx,DWORD PTR [eax+0xba4]
  420eac:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  420eaf:	8a 88 58 03 00 00    	mov    cl,BYTE PTR [eax+0x358]
  420eb5:	84 c9                	test   cl,cl
  420eb7:	74 02                	je     0x420ebb
  420eb9:	b3 01                	mov    bl,0x1
  420ebb:	8a c3                	mov    al,bl
  420ebd:	5b                   	pop    ebx
  420ebe:	c2                   	.byte 0xc2
  420ebf:	08                   	.byte 0x8
