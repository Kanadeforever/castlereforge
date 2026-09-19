; 固化51：公式helper后级K==0主结果归零门。

/mnt/data/work50/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042c83e <.text+0x2b83e>:
  42c83e:	53                   	push   ebx
  42c83f:	8b cd                	mov    ecx,ebp
  42c841:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  42c845:	e8 06 05 00 00       	call   0x42cd50
  42c84a:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  42c84e:	03 c8                	add    ecx,eax
  42c850:	8b 47 40             	mov    eax,DWORD PTR [edi+0x40]
  42c853:	85 c0                	test   eax,eax
  42c855:	89 4c 24 20          	mov    DWORD PTR [esp+0x20],ecx
  42c859:	75 08                	jne    0x42c863
  42c85b:	c7 44 24 20 00 00 00 	mov    DWORD PTR [esp+0x20],0x0
  42c862:	00 
  42c863:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42c867:	c6 06 01             	mov    BYTE PTR [esi],0x1
  42c86a:	8b 43 4c             	mov    eax,DWORD PTR [ebx+0x4c]
  42c86d:	6a 03                	push   0x3
  42c86f:	2b 41 4c             	sub    eax,DWORD PTR [ecx+0x4c]
  42c872:	99                   	cdq
  42c873:	83 e2 07             	and    edx,0x7
  42c876:	03 c2                	add    eax,edx
  42c878:	8b 57 48             	mov    edx,DWORD PTR [edi+0x48]
  42c87b:	c1 f8 03             	sar    eax,0x3
  42c87e:	03 c2                	add    eax,edx
  42c880:	89 44 24 30          	mov    DWORD PTR [esp+0x30],eax
