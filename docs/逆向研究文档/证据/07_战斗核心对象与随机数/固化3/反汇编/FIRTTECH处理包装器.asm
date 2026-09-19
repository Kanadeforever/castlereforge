
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042a780 <.text+0x29780>:
  42a780:	01 8b 00 89 0d cc    	add    DWORD PTR [ebx-0x33f27700],ecx
  42a786:	01 8b 00 b0 01 c2    	add    DWORD PTR [ebx-0x3dfe5000],ecx
  42a78c:	0c 00                	or     al,0x0
  42a78e:	32 c0                	xor    al,al
  42a790:	c2 0c 00             	ret    0xc
  42a793:	90                   	nop
  42a794:	90                   	nop
  42a795:	90                   	nop
  42a796:	90                   	nop
  42a797:	90                   	nop
  42a798:	90                   	nop
  42a799:	90                   	nop
  42a79a:	90                   	nop
  42a79b:	90                   	nop
  42a79c:	90                   	nop
  42a79d:	90                   	nop
  42a79e:	90                   	nop
  42a79f:	90                   	nop
  42a7a0:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42a7a4:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  42a7a8:	50                   	push   eax
  42a7a9:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42a7ad:	52                   	push   edx
  42a7ae:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  42a7b2:	50                   	push   eax
  42a7b3:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42a7b7:	52                   	push   edx
  42a7b8:	50                   	push   eax
  42a7b9:	e8 c2 1e 00 00       	call   0x42c680
  42a7be:	c2 14 00             	ret    0x14
  42a7c1:	90                   	nop
  42a7c2:	90                   	nop
  42a7c3:	90                   	nop
  42a7c4:	90                   	nop
  42a7c5:	90                   	nop
  42a7c6:	90                   	nop
  42a7c7:	90                   	nop
  42a7c8:	90                   	nop
  42a7c9:	90                   	nop
  42a7ca:	90                   	nop
  42a7cb:	90                   	nop
  42a7cc:	90                   	nop
  42a7cd:	90                   	nop
  42a7ce:	90                   	nop
  42a7cf:	90                   	nop
  42a7d0:	83 ec 10             	sub    esp,0x10
  42a7d3:	53                   	push   ebx
  42a7d4:	55                   	push   ebp
  42a7d5:	56                   	push   esi
  42a7d6:	57                   	push   edi
  42a7d7:	8b 7c 24 2c          	mov    edi,DWORD PTR [esp+0x2c]
  42a7db:	8b e9                	mov    ebp,ecx
  42a7dd:	33 c0                	xor    eax,eax
  42a7df:	33 c9                	xor    ecx,ecx
  42a7e1:	8d 57 10             	lea    edx,[edi+0x10]
  42a7e4:	c6 07 00             	mov    BYTE PTR [edi],0x0
  42a7e7:	89 47 04             	mov    DWORD PTR [edi+0x4],eax
  42a7ea:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  42a7ed:	88 47 0c             	mov    BYTE PTR [edi+0xc],al
  42a7f0:	89 0a                	mov    DWORD PTR [edx],ecx
  42a7f2:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  42a7f6:	89 4a 04             	mov    DWORD PTR [edx+0x4],ecx
  42a7f9:	89 4a 08             	mov    DWORD PTR [edx+0x8],ecx
  42a7fc:	89 4a 0c             	mov    DWORD PTR [edx+0xc],ecx
  42a7ff:	89                   	.byte 0x89
