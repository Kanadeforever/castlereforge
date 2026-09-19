
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040d6cf <.text+0xc6cf>:
  40d6cf:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40d6d2:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  40d6d5:	a1 08 f8 89 00       	mov    eax,ds:0x89f808
  40d6da:	50                   	push   eax
  40d6db:	68 e6 03 00 00       	push   0x3e6
  40d6e0:	e8 cb d9 03 00       	call   0x44b0b0
  40d6e5:	83 c4 08             	add    esp,0x8
  40d6e8:	e8 63 da 03 00       	call   0x44b150
  40d6ed:	e8 ae dc ff ff       	call   0x40b3a0
  40d6f2:	8b c8                	mov    ecx,eax
  40d6f4:	e8 a7 d9 ff ff       	call   0x40b0a0
  40d6f9:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40d6fc:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  40d701:	68 94 05 00 00       	push   0x594
  40d706:	8b 51 04             	mov    edx,DWORD PTR [ecx+0x4]
  40d709:	89 90 d0 00 00 00    	mov    DWORD PTR [eax+0xd0],edx
  40d70f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d715:	89 b9 08 01 00 00    	mov    DWORD PTR [ecx+0x108],edi
