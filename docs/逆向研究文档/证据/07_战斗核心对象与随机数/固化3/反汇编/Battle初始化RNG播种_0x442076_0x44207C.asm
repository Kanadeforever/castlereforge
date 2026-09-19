
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0044204c <.text+0x4104c>:
  44204c:	00 50 64             	add    BYTE PTR [eax+0x64],dl
  44204f:	89 25 00 00 00 00    	mov    DWORD PTR ds:0x0,esp
  442055:	83 ec 70             	sub    esp,0x70
  442058:	53                   	push   ebx
  442059:	55                   	push   ebp
  44205a:	56                   	push   esi
  44205b:	8b e9                	mov    ebp,ecx
  44205d:	33 db                	xor    ebx,ebx
  44205f:	57                   	push   edi
  442060:	53                   	push   ebx
  442061:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  442065:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  442069:	e8 82 f6 ff ff       	call   0x4416f0
  44206e:	53                   	push   ebx
  44206f:	89 9c 24 8c 00 00 00 	mov    DWORD PTR [esp+0x8c],ebx
  442076:	e8 08 f5 00 00       	call   0x451583
  44207b:	50                   	push   eax
  44207c:	e8 da f4 00 00       	call   0x45155b
  442081:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  442086:	83 c4 08             	add    esp,0x8
  442089:	8b 78 68             	mov    edi,DWORD PTR [eax+0x68]
  44208c:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  44208f:	8b                   	.byte 0x8b
