; 幽城幻剑录 v0.7D阶段中-固化3 静态证据
; Oracle RPG.exe SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; VA范围: 0x004206C6..0x004207A7
; 用途: 证明主资源存在周期性增减、上限 clamp，并在<=0时进入同一零值状态转移。
; 边界: 仅证明当前 Oracle 指令结构；作者变量名/业务名需额外 consumer 或动态证据。

  4206c6:	8b 8e 8c 08 00 00    	mov    ecx,DWORD PTR [esi+0x88c]
  4206cc:	b8 67 66 66 66       	mov    eax,0x66666667
  4206d1:	f7 e9                	imul   ecx
  4206d3:	c1 fa 03             	sar    edx,0x3
  4206d6:	8b ca                	mov    ecx,edx
  4206d8:	b8 67 66 66 66       	mov    eax,0x66666667
  4206dd:	c1 e9 1f             	shr    ecx,0x1f
  4206e0:	03 d1                	add    edx,ecx
  4206e2:	8b 8e 78 08 00 00    	mov    ecx,DWORD PTR [esi+0x878]
  4206e8:	8b fa                	mov    edi,edx
  4206ea:	f7 e9                	imul   ecx
  4206ec:	c1 fa 03             	sar    edx,0x3
  4206ef:	8b 8e 7c 08 00 00    	mov    ecx,DWORD PTR [esi+0x87c]
  4206f5:	8b c2                	mov    eax,edx
  4206f7:	c1 e8 1f             	shr    eax,0x1f
  4206fa:	03 d0                	add    edx,eax
  4206fc:	2b fa                	sub    edi,edx
  4206fe:	03 cf                	add    ecx,edi
  420700:	8b c1                	mov    eax,ecx
  420702:	89 8e 7c 08 00 00    	mov    DWORD PTR [esi+0x87c],ecx
  420708:	85 c0                	test   eax,eax
  42070a:	7f 07                	jg     0x420713
  42070c:	8b ce                	mov    ecx,esi
  42070e:	e8 dd 2a 00 00       	call   0x4231f0
  420713:	8b be 88 0a 00 00    	mov    edi,DWORD PTR [esi+0xa88]
  420719:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
  42071e:	8d 4f 0a             	lea    ecx,[edi+0xa]
  420721:	f7 e9                	imul   ecx
  420723:	8b ca                	mov    ecx,edx
  420725:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
  42072a:	c1 f9 06             	sar    ecx,0x6
  42072d:	8b d1                	mov    edx,ecx
  42072f:	c1 ea 1f             	shr    edx,0x1f
  420732:	03 ca                	add    ecx,edx
  420734:	f7 ef                	imul   edi
  420736:	c1 fa 06             	sar    edx,0x6
  420739:	8b c2                	mov    eax,edx
  42073b:	c1 e8 1f             	shr    eax,0x1f
  42073e:	03 d0                	add    edx,eax
  420740:	3b ca                	cmp    ecx,edx
  420742:	74 4c                	je     0x420790
  420744:	8b 8e 8c 08 00 00    	mov    ecx,DWORD PTR [esi+0x88c]
  42074a:	b8 67 66 66 66       	mov    eax,0x66666667
  42074f:	f7 e9                	imul   ecx
  420751:	8b be 78 08 00 00    	mov    edi,DWORD PTR [esi+0x878]
  420757:	8b ca                	mov    ecx,edx
  420759:	c1 f9 03             	sar    ecx,0x3
  42075c:	8b d1                	mov    edx,ecx
  42075e:	b8 67 66 66 66       	mov    eax,0x66666667
  420763:	c1 ea 1f             	shr    edx,0x1f
  420766:	03 ca                	add    ecx,edx
  420768:	f7 ef                	imul   edi
  42076a:	c1 fa 03             	sar    edx,0x3
  42076d:	8b c2                	mov    eax,edx
  42076f:	03 d1                	add    edx,ecx
  420771:	8b 8e 7c 08 00 00    	mov    ecx,DWORD PTR [esi+0x87c]
  420777:	c1 e8 1f             	shr    eax,0x1f
  42077a:	03 c2                	add    eax,edx
  42077c:	03 c8                	add    ecx,eax
  42077e:	8b c1                	mov    eax,ecx
  420780:	89 8e 7c 08 00 00    	mov    DWORD PTR [esi+0x87c],ecx
  420786:	3b c7                	cmp    eax,edi
  420788:	7e 06                	jle    0x420790
  42078a:	89 be 7c 08 00 00    	mov    DWORD PTR [esi+0x87c],edi
  420790:	81 be a4 07 00 00 e9 	cmp    DWORD PTR [esi+0x7a4],0x3e9
  420797:	03 00 00 
  42079a:	75 09                	jne    0x4207a5
  42079c:	6a 00                	push   0x0
  42079e:	8b ce                	mov    ecx,esi
  4207a0:	e8 0b 00 00 00       	call   0x4207b0
  4207a5:	5f                   	pop    edi
  4207a6:	5e                   	pop    esi
  4207a7:	c3                   	ret
