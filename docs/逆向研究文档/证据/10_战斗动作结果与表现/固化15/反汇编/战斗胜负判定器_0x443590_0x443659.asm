
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443590 <.text+0x42590>:
  443590:	53                   	push   ebx
  443591:	55                   	push   ebp
  443592:	56                   	push   esi
  443593:	57                   	push   edi
  443594:	8b f9                	mov    edi,ecx
  443596:	33 ed                	xor    ebp,ebp
  443598:	33 f6                	xor    esi,esi
  44359a:	32 db                	xor    bl,bl
  44359c:	8d 87 34 0c 00 00    	lea    eax,[edi+0xc34]
  4435a2:	ba 10 00 00 00       	mov    edx,0x10
  4435a7:	83 b8 dc fe ff ff ff 	cmp    DWORD PTR [eax-0x124],0xffffffff
  4435ae:	74 19                	je     0x4435c9
  4435b0:	8b 88 24 02 00 00    	mov    ecx,DWORD PTR [eax+0x224]
  4435b6:	85 c9                	test   ecx,ecx
  4435b8:	75 0f                	jne    0x4435c9
  4435ba:	8b 08                	mov    ecx,DWORD PTR [eax]
  4435bc:	85 c9                	test   ecx,ecx
  4435be:	75 03                	jne    0x4435c3
  4435c0:	46                   	inc    esi
  4435c1:	eb 06                	jmp    0x4435c9
  4435c3:	83 f9 01             	cmp    ecx,0x1
  4435c6:	75 01                	jne    0x4435c9
  4435c8:	45                   	inc    ebp
  4435c9:	05 e4 0d 00 00       	add    eax,0xde4
  4435ce:	4a                   	dec    edx
  4435cf:	75 d6                	jne    0x4435a7
  4435d1:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4435d6:	83 b8 f4 00 00 00 01 	cmp    DWORD PTR [eax+0xf4],0x1
  4435dd:	75 0a                	jne    0x4435e9
  4435df:	33 f6                	xor    esi,esi
  4435e1:	b3 01                	mov    bl,0x1
  4435e3:	89 b0 f4 00 00 00    	mov    DWORD PTR [eax+0xf4],esi
  4435e9:	85 ed                	test   ebp,ebp
  4435eb:	74 08                	je     0x4435f5
  4435ed:	85 f6                	test   esi,esi
  4435ef:	74 04                	je     0x4435f5
  4435f1:	84 db                	test   bl,bl
  4435f3:	74 34                	je     0x443629
  4435f5:	8b 0d 74 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd74
  4435fb:	6a ff                	push   0xffffffff
  4435fd:	e8 2e 89 fd ff       	call   0x41bf30
  443602:	8b 0d 74 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd74
  443608:	e8 53 88 fd ff       	call   0x41be60
  44360d:	84 db                	test   bl,bl
  44360f:	c6 87 30 e3 00 00 01 	mov    BYTE PTR [edi+0xe330],0x1
  443616:	74 11                	je     0x443629
  443618:	5f                   	pop    edi
  443619:	5e                   	pop    esi
  44361a:	5d                   	pop    ebp
  44361b:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  443622:	b8 04 00 00 00       	mov    eax,0x4
  443627:	5b                   	pop    ebx
  443628:	c3                   	ret
  443629:	85 ed                	test   ebp,ebp
  44362b:	75 11                	jne    0x44363e
  44362d:	5f                   	pop    edi
  44362e:	5e                   	pop    esi
  44362f:	5d                   	pop    ebp
  443630:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  443637:	b8 02 00 00 00       	mov    eax,0x2
  44363c:	5b                   	pop    ebx
  44363d:	c3                   	ret
  44363e:	85 f6                	test   esi,esi
  443640:	75 11                	jne    0x443653
  443642:	5f                   	pop    edi
  443643:	5e                   	pop    esi
  443644:	5d                   	pop    ebp
  443645:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  44364c:	b8 01 00 00 00       	mov    eax,0x1
  443651:	5b                   	pop    ebx
  443652:	c3                   	ret
  443653:	5f                   	pop    edi
  443654:	5e                   	pop    esi
  443655:	5d                   	pop    ebp
  443656:	33 c0                	xor    eax,eax
  443658:	5b                   	pop    ebx
  443659:	c3                   	ret
