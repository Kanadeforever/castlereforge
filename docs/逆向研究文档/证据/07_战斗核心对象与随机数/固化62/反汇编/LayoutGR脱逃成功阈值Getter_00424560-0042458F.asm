
/mnt/data/solid62_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

00424560 <.text+0x23560>:
  424560:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  424564:	b8 64 00 00 00       	mov    eax,0x64
  424569:	85 d2                	test   edx,edx
  42456b:	7f 08                	jg     0x424575
  42456d:	56                   	push   esi
  42456e:	8b 31                	mov    esi,DWORD PTR [ecx]
  424570:	3b d6                	cmp    edx,esi
  424572:	5e                   	pop    esi
  424573:	7d 11                	jge    0x424586
  424575:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  424578:	8b c2                	mov    eax,edx
  42457a:	c1 e0 07             	shl    eax,0x7
  42457d:	2b c2                	sub    eax,edx
  42457f:	8b 84 81 9c 00 00 00 	mov    eax,DWORD PTR [ecx+eax*4+0x9c]
  424586:	c2 04 00             	ret    0x4
  424589:	90                   	nop
  42458a:	90                   	nop
  42458b:	90                   	nop
  42458c:	90                   	nop
  42458d:	90                   	nop
  42458e:	90                   	nop
  42458f:	90                   	nop
