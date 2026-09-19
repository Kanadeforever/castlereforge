
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040d6f9 <.text+0xc6f9>:
  40d6f9:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40d6fc:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  40d701:	68 94 05 00 00       	push   0x594
  40d706:	8b 51 04             	mov    edx,DWORD PTR [ecx+0x4]
  40d709:	89 90 d0 00 00 00    	mov    DWORD PTR [eax+0xd0],edx
  40d70f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d715:	89 b9 08 01 00 00    	mov    DWORD PTR [ecx+0x108],edi
  40d71b:	e8 3f 3f 04 00       	call   0x45165f
  40d720:	83 c4 04             	add    esp,0x4
  40d723:	89                   	.byte 0x89
