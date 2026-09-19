
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004423a0 <.text+0x413a0>:
  4423a0:	89 11                	mov    DWORD PTR [ecx],edx
  4423a2:	40                   	inc    eax
  4423a3:	83 c6 04             	add    esi,0x4
  4423a6:	83 c1 04             	add    ecx,0x4
  4423a9:	83 f8 10             	cmp    eax,0x10
  4423ac:	0f 8c 01 ff ff ff    	jl     0x4422b3
  4423b2:	8d 85 31 e3 00 00    	lea    eax,[ebp+0xe331]
  4423b8:	50                   	push   eax
  4423b9:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4423be:	8b 88 d8 00 00 00    	mov    ecx,DWORD PTR [eax+0xd8]
  4423c4:	51                   	push   ecx
  4423c5:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  4423cb:	e8 60 20 fe ff       	call   0x424430
  4423d0:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4423d6:	89 82 dc 00 00 00    	mov    DWORD PTR [edx+0xdc],eax
  4423dc:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4423e1:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  4423e7:	e8 a4 21 fe ff       	call   0x424590
  4423ec:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  4423f2:	89 81 e4 00 00 00    	mov    DWORD PTR [ecx+0xe4],eax
  4423f8:	8b                   	.byte 0x8b
