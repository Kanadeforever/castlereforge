
/mnt/data/work49/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b52c <.text+0x2a52c>:
  42b52c:	79 08                	jns    0x42b536
  42b52e:	c7 44 9f 24 00 00 00 	mov    DWORD PTR [edi+ebx*4+0x24],0x0
  42b535:	00 
  42b536:	8b 4c 9f 10          	mov    ecx,DWORD PTR [edi+ebx*4+0x10]
  42b53a:	8b 14 cd 68 a6 46 00 	mov    edx,DWORD PTR [ecx*8+0x46a668]
  42b541:	85 d2                	test   edx,edx
  42b543:	7c 24                	jl     0x42b569
  42b545:	8b 74 24 34          	mov    esi,DWORD PTR [esp+0x34]
  42b549:	8b 4c 9f 24          	mov    ecx,DWORD PTR [edi+ebx*4+0x24]
  42b54d:	8b 84 96 e8 01 00 00 	mov    eax,DWORD PTR [esi+edx*4+0x1e8]
  42b554:	3b c1                	cmp    eax,ecx
  42b556:	7e 0b                	jle    0x42b563
  42b558:	2b c1                	sub    eax,ecx
  42b55a:	89 84 96 e8 01 00 00 	mov    DWORD PTR [esi+edx*4+0x1e8],eax
  42b561:	eb 06                	jmp    0x42b569
  42b563:	2b c8                	sub    ecx,eax
  42b565:	89 4c 9f 24          	mov    DWORD PTR [edi+ebx*4+0x24],ecx
  42b569:	8b 44 9f 24          	mov    eax,DWORD PTR [edi+ebx*4+0x24]
  42b56d:	85 c0                	test   eax,eax
  42b56f:	7d 08                	jge    0x42b579
  42b571:	c7 44 9f 24    	mov    DWORD PTR [edi+ebx*4+0x24],0x0
  42b578:	 
