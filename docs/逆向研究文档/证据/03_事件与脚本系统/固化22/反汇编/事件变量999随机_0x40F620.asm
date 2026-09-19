
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040f620 <.text+0xe620>:
  40f620:	56                   	push   esi
  40f621:	57                   	push   edi
  40f622:	8b f9                	mov    edi,ecx
  40f624:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  40f627:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  40f62a:	8b 70 04             	mov    esi,DWORD PTR [eax+0x4]
  40f62d:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  40f630:	2b f1                	sub    esi,ecx
  40f632:	46                   	inc    esi
  40f633:	e8 2d 1f 04 00       	call   0x451565
  40f638:	99                   	cdq
  40f639:	f7 fe                	idiv   esi
  40f63b:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  40f63e:	03 50 08             	add    edx,DWORD PTR [eax+0x8]
  40f641:	52                   	push   edx
  40f642:	68 e7 03 00 00       	push   0x3e7
  40f647:	e8 64 ba 03 00       	call   0x44b0b0
  40f64c:	8b 15 10 f8 89 00    	mov    edx,DWORD PTR ds:0x89f810
  40f652:	57                   	push   edi
  40f653:	42                   	inc    edx
  40f654:	c7 05 18 f8 89 00 00 	mov    DWORD PTR ds:0x89f818,0x0
  40f65b:	00 00 00 
  40f65e:	89 15 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,edx
  40f664:	e8 e7 1e 04 00       	call   0x451550
  40f669:	83 c4 0c             	add    esp,0xc
  40f66c:	33 c0                	xor    eax,eax
  40f66e:	5f                   	pop    edi
  40f66f:	5e                   	pop    esi
  40f670:	c3                   	ret
  40f671:	90                   	nop
  40f672:	90                   	nop
  40f673:	90                   	nop
  40f674:	90                   	nop
  40f675:	90                   	nop
  40f676:	90                   	nop
  40f677:	90                   	nop
  40f678:	90                   	nop
  40f679:	90                   	nop
  40f67a:	90                   	nop
  40f67b:	90                   	nop
  40f67c:	90                   	nop
  40f67d:	90                   	nop
  40f67e:	90                   	nop
  40f67f:	90                   	nop
