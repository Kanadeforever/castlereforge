; ===== Battle构造时绑定公式层manager指针 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f2b0 <.text+0x1e2b0>:
  41f2b0:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41f2b4:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  41f2b8:	56                   	push   esi
  41f2b9:	8b f1                	mov    esi,ecx
  41f2bb:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  41f2bf:	68 08 fe 89 00       	push   0x89fe08
  41f2c4:	89 86 a4 0b 00 00    	mov    DWORD PTR [esi+0xba4],eax
  41f2ca:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41f2ce:	89 8e 2c 07 00 00    	mov    DWORD PTR [esi+0x72c],ecx
  41f2d4:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  41f2d8:	89 16                	mov    DWORD PTR [esi],edx
  41f2da:	89 86 9c 0b 00 00    	mov    DWORD PTR [esi+0xb9c],eax
  41f2e0:	89 86 a0 0b 00 00    	mov    DWORD PTR [esi+0xba0],eax
  41f2e6:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  41f2ec:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  41f2f0:	89 0d ec fd 89 00    	mov    DWORD PTR ds:0x89fdec,ecx
  41f2f6:	68 d0 ff 89 00       	push   0x89ffd0
  41f2fb:	52                   	push   edx
  41f2fc:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  41f301:	a3 f0 fd 89 00       	mov    ds:0x89fdf0,eax
  41f306:	e8 55 b4 00 00       	call   0x42a760
; ===== 0x42A760保存三条全局指针 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042a760 <.text+0x29760>:
  42a760:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  42a764:	85 d2                	test   edx,edx
  42a766:	74 26                	je     0x42a78e
  42a768:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  42a76c:	85 c9                	test   ecx,ecx
  42a76e:	74 1e                	je     0x42a78e
  42a770:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42a774:	85 c0                	test   eax,eax
  42a776:	74 16                	je     0x42a78e
  42a778:	a3 d0 01 8b 00       	mov    ds:0x8b01d0,eax
  42a77d:	89 15 c8 01 8b 00    	mov    DWORD PTR ds:0x8b01c8,edx
  42a783:	89 0d cc 01 8b 00    	mov    DWORD PTR ds:0x8b01cc,ecx
  42a789:	b0 01                	mov    al,0x1
  42a78b:	c2 0c 00             	ret    0xc
  42a78e:	32 c0                	xor    al,al
  42a790:	c2                   	.byte 0xc2
; ===== Battle两个显式别名指针 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442783 <.text+0x41783>:
  442783:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  442788:	33 ff                	xor    edi,edi
  44278a:	8b 48 3c             	mov    ecx,DWORD PTR [eax+0x3c]
  44278d:	c7 05 54 fd 89 00 08 	mov    DWORD PTR ds:0x89fd54,0x89fe08
  442794:	fe 89 00 
  442797:	89 0d d8 fc 89 00    	mov    DWORD PTR ds:0x89fcd8,ecx
  44279d:	c7 05 58 fd 89 00 d0 	mov    DWORD PTR ds:0x89fd58,0x89ffd0
  4427a4:	ff 89 00 
