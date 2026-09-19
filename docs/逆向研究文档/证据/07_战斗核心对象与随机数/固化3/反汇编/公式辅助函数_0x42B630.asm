
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b630 <.text+0x2a630>:
  42b630:	53                   	push   ebx
  42b631:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b635:	56                   	push   esi
  42b636:	57                   	push   edi
  42b637:	8b f9                	mov    edi,ecx
  42b639:	53                   	push   ebx
  42b63a:	e8 41 04 00 00       	call   0x42ba80
  42b63f:	8b 4b 2c             	mov    ecx,DWORD PTR [ebx+0x2c]
  42b642:	8b 53 30             	mov    edx,DWORD PTR [ebx+0x30]
  42b645:	8b f0                	mov    esi,eax
  42b647:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  42b64b:	2b ca                	sub    ecx,edx
  42b64d:	8b 5b 3c             	mov    ebx,DWORD PTR [ebx+0x3c]
  42b650:	0f af 48 40          	imul   ecx,DWORD PTR [eax+0x40]
  42b654:	b8 67 66 66 66       	mov    eax,0x66666667
  42b659:	f7 e9                	imul   ecx
  42b65b:	c1 fa 02             	sar    edx,0x2
  42b65e:	8b ca                	mov    ecx,edx
  42b660:	b8 67 66 66 66       	mov    eax,0x66666667
  42b665:	c1 e9 1f             	shr    ecx,0x1f
  42b668:	03 d1                	add    edx,ecx
  42b66a:	03 f2                	add    esi,edx
  42b66c:	f7 eb                	imul   ebx
  42b66e:	c1 fa 02             	sar    edx,0x2
  42b671:	8b c2                	mov    eax,edx
  42b673:	c1 e8 1f             	shr    eax,0x1f
  42b676:	03 d0                	add    edx,eax
  42b678:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b67d:	52                   	push   edx
  42b67e:	f7 eb                	imul   ebx
  42b680:	c1 fa 04             	sar    edx,0x4
  42b683:	8b ca                	mov    ecx,edx
  42b685:	c1 e9 1f             	shr    ecx,0x1f
  42b688:	03 d1                	add    edx,ecx
  42b68a:	8b cf                	mov    ecx,edi
  42b68c:	52                   	push   edx
  42b68d:	e8 1e 16 00 00       	call   0x42ccb0
  42b692:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  42b696:	8b cf                	mov    ecx,edi
  42b698:	52                   	push   edx
  42b699:	03 f0                	add    esi,eax
  42b69b:	e8 b0 04 00 00       	call   0x42bb50
  42b6a0:	2b f0                	sub    esi,eax
  42b6a2:	b8 00 00 00 00       	mov    eax,0x0
  42b6a7:	0f 98 c0             	sets   al
  42b6aa:	48                   	dec    eax
  42b6ab:	5f                   	pop    edi
  42b6ac:	23 c6                	and    eax,esi
  42b6ae:	5e                   	pop    esi
  42b6af:	5b                   	pop    ebx
  42b6b0:	c2 0c 00             	ret    0xc
  42b6b3:	90                   	nop
  42b6b4:	90                   	nop
  42b6b5:	90                   	nop
  42b6b6:	90                   	nop
  42b6b7:	90                   	nop
  42b6b8:	90                   	nop
  42b6b9:	90                   	nop
  42b6ba:	90                   	nop
  42b6bb:	90                   	nop
  42b6bc:	90                   	nop
  42b6bd:	90                   	nop
  42b6be:	90                   	nop
  42b6bf:	90                   	nop
