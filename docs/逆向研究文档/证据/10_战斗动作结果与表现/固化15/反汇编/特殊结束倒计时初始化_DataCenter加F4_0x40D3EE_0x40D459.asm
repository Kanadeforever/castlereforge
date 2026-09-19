
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040d3ee <.text+0xc3ee>:
  40d3ee:	e8 5d 4d ff ff       	call   0x402150
  40d3f3:	c7 05 18 f8 89 00 01 	mov    DWORD PTR ds:0x89f818,0x1
  40d3fa:	00 00 00 
  40d3fd:	8b 45 04             	mov    eax,DWORD PTR [ebp+0x4]
  40d400:	89 45 08             	mov    DWORD PTR [ebp+0x8],eax
  40d403:	e8 98 df ff ff       	call   0x40b3a0
  40d408:	8b c8                	mov    ecx,eax
  40d40a:	e8 91 dc ff ff       	call   0x40b0a0
  40d40f:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  40d412:	81 78 04 82 00 00 00 	cmp    DWORD PTR [eax+0x4],0x82
  40d419:	75 20                	jne    0x40d43b
  40d41b:	e8 45 41 04 00       	call   0x451565
  40d420:	99                   	cdq
  40d421:	b9 90 01 00 00       	mov    ecx,0x190
  40d426:	f7 f9                	idiv   ecx
  40d428:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  40d42d:	81 c2 20 03 00 00    	add    edx,0x320
  40d433:	89 90 f4 00 00 00    	mov    DWORD PTR [eax+0xf4],edx
  40d439:	eb 0c                	jmp    0x40d447
  40d43b:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d441:	89 99 f4 00 00 00    	mov    DWORD PTR [ecx+0xf4],ebx
  40d447:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
  40d44a:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d450:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  40d453:	89 81 d8 00 00 00    	mov    DWORD PTR [ecx+0xd8],eax
  40d459:	8b                   	.byte 0x8b
