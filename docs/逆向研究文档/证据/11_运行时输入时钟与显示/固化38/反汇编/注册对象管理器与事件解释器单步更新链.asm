; 固化38证据：全局对象注册器/管理器与EVE vtable[0]单步链。

/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

004343e0 <.text+0x333e0>:
  4343e0:	56                   	push   esi
  4343e1:	8b f1                	mov    esi,ecx
  4343e3:	c7 06 0c 0e 46 00    	mov    DWORD PTR [esi],0x460e0c
  4343e9:	c6 86 14 02 00 00 00 	mov    BYTE PTR [esi+0x214],0x0
  4343f0:	a1 00 ed 8d 00       	mov    eax,ds:0x8ded00
  4343f5:	85 c0                	test   eax,eax
  4343f7:	75 05                	jne    0x4343fe
  4343f9:	e8 a2 ff ff ff       	call   0x4343a0
  4343fe:	a1 00 ed 8d 00       	mov    eax,ds:0x8ded00
  434403:	40                   	inc    eax
  434404:	a3 00 ed 8d 00       	mov    ds:0x8ded00,eax
  434409:	8b c6                	mov    eax,esi
  43440b:	5e                   	pop    esi
  43440c:	c3                   	ret
  43440d:	90                   	nop
  43440e:	90                   	nop
  43440f:	90                   	nop
  434410:	51                   	push   ecx
  434411:	c7 01 0c 0e 46 00    	mov    DWORD PTR [ecx],0x460e0c
  434417:	e8 74 03 00 00       	call   0x434790
  43441c:	ff 0d 00 ed 8d 00    	dec    DWORD PTR ds:0x8ded00
  434422:	c3                   	ret
  434423:	90                   	nop
  434424:	90                   	nop
  434425:	90                   	nop
  434426:	90                   	nop
  434427:	90                   	nop
  434428:	90                   	nop
  434429:	90                   	nop
  43442a:	90                   	nop
  43442b:	90                   	nop
  43442c:	90                   	nop
  43442d:	90                   	nop
  43442e:	90                   	nop
  43442f:	90                   	nop
  434430:	a1 d4 40 8c 00       	mov    eax,ds:0x8c40d4
  434435:	3d c8 00 00 00       	cmp    eax,0xc8
  43443a:	7c 17                	jl     0x434453
  43443c:	8d 44 24 0c          	lea    eax,[esp+0xc]
  434440:	68 d8 37 46 00       	push   0x4637d8
  434445:	50                   	push   eax
  434446:	c7 44 24 14 c4 ad 46 	mov    DWORD PTR [esp+0x14],0x46adc4
  43444d:	00 
  43444e:	e8 74 d7 01 00       	call   0x451bc7
  434453:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  434457:	8a 54 24 0c          	mov    dl,BYTE PTR [esp+0xc]
  43445b:	89 0c c5 c0 e6 8d 00 	mov    DWORD PTR [eax*8+0x8de6c0],ecx
  434462:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  434466:	89 0c c5 c4 e6 8d 00 	mov    DWORD PTR [eax*8+0x8de6c4],ecx
  43446d:	88 91 14 02 00 00    	mov    BYTE PTR [ecx+0x214],dl
  434473:	a1 d4 40 8c 00       	mov    eax,ds:0x8c40d4
  434478:	40                   	inc    eax
  434479:	a3 d4 40 8c 00       	mov    ds:0x8c40d4,eax
  43447e:	c2 0c 00             	ret    0xc
  434481:	90                   	nop
  434482:	90                   	nop
  434483:	90                   	nop
  434484:	90                   	nop
  434485:	90                   	nop
  434486:	90                   	nop
  434487:	90                   	nop
  434488:	90                   	nop
  434489:	90                   	nop
  43448a:	90                   	nop
  43448b:	90                   	nop
  43448c:	90                   	nop
  43448d:	90                   	nop
  43448e:	90                   	nop
  43448f:	90                   	nop
  434490:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  434494:	53                   	push   ebx
  434495:	85 c0                	test   eax,eax
  434497:	57                   	push   edi
  434498:	8b d9                	mov    ebx,ecx
  43449a:	74 5e                	je     0x4344fa
  43449c:	a1 d4 40 8c 00       	mov    eax,ds:0x8c40d4
  4344a1:	33 ff                	xor    edi,edi
  4344a3:	85 c0                	test   eax,eax
  4344a5:	7e 53                	jle    0x4344fa
  4344a7:	56                   	push   esi
  4344a8:	be c4 e6 8d 00       	mov    esi,0x8de6c4
  4344ad:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  4344b1:	8b 16                	mov    edx,DWORD PTR [esi]
  4344b3:	3b ca                	cmp    ecx,edx
  4344b5:	75 3a                	jne    0x4344f1
  4344b7:	48                   	dec    eax
  4344b8:	c7 06 00 00 00 00    	mov    DWORD PTR [esi],0x0
  4344be:	85 c0                	test   eax,eax
  4344c0:	c7 46 fc ff ff ff 7f 	mov    DWORD PTR [esi-0x4],0x7fffffff
  4344c7:	a3 d4 40 8c 00       	mov    ds:0x8c40d4,eax
  4344cc:	7e 23                	jle    0x4344f1
  4344ce:	8b 14 c5 c4 e6 8d 00 	mov    edx,DWORD PTR [eax*8+0x8de6c4]
  4344d5:	6a 00                	push   0x0
  4344d7:	89 16                	mov    DWORD PTR [esi],edx
  4344d9:	8b 04 c5 c0 e6 8d 00 	mov    eax,DWORD PTR [eax*8+0x8de6c0]
  4344e0:	6a 00                	push   0x0
  4344e2:	8b cb                	mov    ecx,ebx
  4344e4:	89 46 fc             	mov    DWORD PTR [esi-0x4],eax
  4344e7:	e8 64 01 00 00       	call   0x434650
  4344ec:	a1 d4 40 8c 00       	mov    eax,ds:0x8c40d4
  4344f1:	47                   	inc    edi
  4344f2:	83 c6 08             	add    esi,0x8
  4344f5:	3b f8                	cmp    edi,eax
  4344f7:	7c b4                	jl     0x4344ad
  4344f9:	5e                   	pop    esi
  4344fa:	5f                   	pop    edi
  4344fb:	5b                   	pop    ebx
  4344fc:	c2 04 00             	ret    0x4
  4344ff:	90                   	nop
  434500:	a1 38 da 8d 00       	mov    eax,ds:0x8dda38
  434505:	3d c8 00 00 00       	cmp    eax,0xc8
  43450a:	7c 17                	jl     0x434523
  43450c:	8d 44 24 08          	lea    eax,[esp+0x8]
  434510:	68 d8 37 46 00       	push   0x4637d8
  434515:	50                   	push   eax
  434516:	c7 44 24 10 e8 ad 46 	mov    DWORD PTR [esp+0x10],0x46ade8
  43451d:	00 
  43451e:	e8 a4 d6 01 00       	call   0x451bc7
  434523:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  434527:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  43452b:	89 0c c5 40 da 8d 00 	mov    DWORD PTR [eax*8+0x8dda40],ecx
  434532:	89 14 c5 44 da 8d 00 	mov    DWORD PTR [eax*8+0x8dda44],edx
  434539:	40                   	inc    eax
  43453a:	a3 38 da 8d 00       	mov    ds:0x8dda38,eax
  43453f:	c2 08 00             	ret    0x8
  434542:	90                   	nop
  434543:	90                   	nop
  434544:	90                   	nop
  434545:	90                   	nop
  434546:	90                   	nop
  434547:	90                   	nop
  434548:	90                   	nop
  434549:	90                   	nop
  43454a:	90                   	nop
  43454b:	90                   	nop
  43454c:	90                   	nop
  43454d:	90                   	nop
  43454e:	90                   	nop
  43454f:	90                   	nop
  434550:	57                   	push   edi
  434551:	b9 90 01 00 00       	mov    ecx,0x190
  434556:	33 c0                	xor    eax,eax
  434558:	bf c0 e6 8d 00       	mov    edi,0x8de6c0
  43455d:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  43455f:	c7 05 d4 40 8c 00 00 	mov    DWORD PTR ds:0x8c40d4,0x0
  434566:	00 00 00 
  434569:	5f                   	pop    edi
  43456a:	c3                   	ret
  43456b:	90                   	nop
  43456c:	90                   	nop
  43456d:	90                   	nop
  43456e:	90                   	nop
  43456f:	90                   	nop
  434570:	57                   	push   edi
  434571:	b9 90 01 00 00       	mov    ecx,0x190
  434576:	33 c0                	xor    eax,eax
  434578:	bf 40 da 8d 00       	mov    edi,0x8dda40
  43457d:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  43457f:	c7 05 38 da 8d 00 00 	mov    DWORD PTR ds:0x8dda38,0x0
  434586:	00 00 00 
  434589:	5f                   	pop    edi
  43458a:	c3                   	ret
  43458b:	90                   	nop
  43458c:	90                   	nop
  43458d:	90                   	nop
  43458e:	90                   	nop
  43458f:	90                   	nop
  434590:	57                   	push   edi
  434591:	b9 90 65 00 00       	mov    ecx,0x6590
  434596:	33 c0                	xor    eax,eax
  434598:	bf d8 40 8c 00       	mov    edi,0x8c40d8
  43459d:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  43459f:	c7 05 d0 40 8c 00 00 	mov    DWORD PTR ds:0x8c40d0,0x0
  4345a6:	00 00 00 
  4345a9:	5f                   	pop    edi
  4345aa:	c3                   	ret
  4345ab:	90                   	nop
  4345ac:	90                   	nop
  4345ad:	90                   	nop
  4345ae:	90                   	nop
  4345af:	90                   	nop
  4345b0:	57                   	push   edi
  4345b1:	b9 c8 00 00 00       	mov    ecx,0xc8
  4345b6:	33 c0                	xor    eax,eax
  4345b8:	bf 18 d7 8d 00       	mov    edi,0x8dd718
  4345bd:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4345bf:	c7 05 3c da 8d 00 00 	mov    DWORD PTR ds:0x8dda3c,0x0
  4345c6:	00 00 00 
  4345c9:	5f                   	pop    edi
  4345ca:	c3                   	ret
  4345cb:	90                   	nop
  4345cc:	90                   	nop
  4345cd:	90                   	nop
  4345ce:	90                   	nop
  4345cf:	90                   	nop
  4345d0:	57                   	push   edi
  4345d1:	b9 90 01 00 00       	mov    ecx,0x190
  4345d6:	33 c0                	xor    eax,eax
  4345d8:	bf 80 e0 8d 00       	mov    edi,0x8de080
  4345dd:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4345df:	c7 05 04 ed 8d 00 00 	mov    DWORD PTR ds:0x8ded04,0x0
  4345e6:	00 00 00 
  4345e9:	5f                   	pop    edi
  4345ea:	c3                   	ret
  4345eb:	90                   	nop
  4345ec:	90                   	nop
  4345ed:	90                   	nop
  4345ee:	90                   	nop
  4345ef:	90                   	nop
  4345f0:	8b 15 d4 40 8c 00    	mov    edx,DWORD PTR ds:0x8c40d4
  4345f6:	56                   	push   esi
  4345f7:	33 f6                	xor    esi,esi
  4345f9:	85 d2                	test   edx,edx
  4345fb:	7e 29                	jle    0x434626
  4345fd:	57                   	push   edi
  4345fe:	bf c4 e6 8d 00       	mov    edi,0x8de6c4
  434603:	8b 0f                	mov    ecx,DWORD PTR [edi]
  434605:	85 c9                	test   ecx,ecx
  434607:	74 14                	je     0x43461d
  434609:	8a 81 14 02 00 00    	mov    al,BYTE PTR [ecx+0x214]
  43460f:	84 c0                	test   al,al
  434611:	74 0a                	je     0x43461d
  434613:	8b 01                	mov    eax,DWORD PTR [ecx]
  434615:	ff 10                	call   DWORD PTR [eax]
  434617:	8b 15 d4 40 8c 00    	mov    edx,DWORD PTR ds:0x8c40d4
  43461d:	46                   	inc    esi
  43461e:	83 c7 08             	add    edi,0x8
  434621:	3b f2                	cmp    esi,edx
  434623:	7c de                	jl     0x434603
  434625:	5f                   	pop    edi
  434626:	33 f6                	xor    esi,esi
  434628:	85 d2                	test   edx,edx
  43462a:	7e 20                	jle    0x43464c
  43462c:	b9 c4 e6 8d 00       	mov    ecx,0x8de6c4
  434631:	8b 01                	mov    eax,DWORD PTR [ecx]
  434633:	85 c0                	test   eax,eax
  434635:	74 0d                	je     0x434644
  434637:	c6 80 14 02 00 00 01 	mov    BYTE PTR [eax+0x214],0x1
  43463e:	8b 15 d4 40 8c 00    	mov    edx,DWORD PTR ds:0x8c40d4
  434644:	46                   	inc    esi
  434645:	83 c1 08             	add    ecx,0x8
  434648:	3b f2                	cmp    esi,edx
  43464a:	7c e5                	jl     0x434631
  43464c:	5e                   	pop    esi
  43464d:	c3                   	ret
  43464e:	90                   	nop
  43464f:	90                   	nop
  434650:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  434654:	83 ec 08             	sub    esp,0x8
  434657:	83 e8 00             	sub    eax,0x0
  43465a:	53                   	push   ebx
  43465b:	74 2b                	je     0x434688
  43465d:	48                   	dec    eax
  43465e:	74 17                	je     0x434677
  434660:	8d 44 24 10          	lea    eax,[esp+0x10]
  434664:	68 d8 37 46 00       	push   0x4637d8
  434669:	50                   	push   eax
  43466a:	c7 44 24 18 0c ae 46 	mov    DWORD PTR [esp+0x18],0x46ae0c
  434671:	00 
  434672:	e8 50 d5 01 00       	call   0x451bc7
  434677:	8b 0d 38 da 8d 00    	mov    ecx,DWORD PTR ds:0x8dda38
  43467d:	bb 40 da         	mov    ebx,0x8dda40


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040ae80 <.text+0x9e80>:
  40ae80:	56                   	push   esi
  40ae81:	8b f1                	mov    esi,ecx
  40ae83:	e8 58 95 02 00       	call   0x4343e0
  40ae88:	33 c0                	xor    eax,eax
  40ae8a:	c7 06 98 02 46 00    	mov    DWORD PTR [esi],0x460298
  40ae90:	88 86 19 02 00 00    	mov    BYTE PTR [esi+0x219],al
  40ae96:	89 86 80 02 00 00    	mov    DWORD PTR [esi+0x280],eax
  40ae9c:	c6 86 18 02 00 00 01 	mov    BYTE PTR [esi+0x218],0x1
  40aea3:	8b c6                	mov    eax,esi
  40aea5:	5e                   	pop    esi
  40aea6:	c3                   	ret
  40aea7:	90                   	nop
  40aea8:	90                   	nop
  40aea9:	90                   	nop
  40aeaa:	90                   	nop
  40aeab:	90                   	nop
  40aeac:	90                   	nop
  40aead:	90                   	nop
  40aeae:	90                   	nop
  40aeaf:	90                   	nop
  40aeb0:	6a ff                	push   0xffffffff
  40aeb2:	68 08 d1 45 00       	push   0x45d108
  40aeb7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40aebd:	50                   	push   eax
  40aebe:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40aec5:	51                   	push   ecx
  40aec6:	56                   	push   esi
  40aec7:	8b f1                	mov    esi,ecx
  40aec9:	89 74 24 04          	mov    DWORD PTR [esp+0x4],esi
  40aecd:	c7 06 98 02 46 00    	mov    DWORD PTR [esi],0x460298
  40aed3:	c7 44 24 10 00 00 00 	mov    DWORD PTR [esp+0x10],0x0
  40aeda:	00 
  40aedb:	e8 30 00 00 00       	call   0x40af10
  40aee0:	8b ce                	mov    ecx,esi
  40aee2:	e8 39 99 02 00       	call   0x434820
  40aee7:	8b ce                	mov    ecx,esi
  40aee9:	c7 44 24 10 ff ff ff 	mov    DWORD PTR [esp+0x10],0xffffffff
  40aef0:	ff 
  40aef1:	e8 1a 95 02 00       	call   0x434410
  40aef6:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  40aefa:	5e                   	pop    esi
  40aefb:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40af02:	83 c4 10             	add    esp,0x10
  40af05:	c3                   	ret
  40af06:	90                   	nop
  40af07:	90                   	nop
  40af08:	90                   	nop
  40af09:	90                   	nop
  40af0a:	90                   	nop
  40af0b:	90                   	nop
  40af0c:	90                   	nop
  40af0d:	90                   	nop
  40af0e:	90                   	nop
  40af0f:	90                   	nop
  40af10:	56                   	push   esi
  40af11:	57                   	push   edi
  40af12:	8b f9                	mov    edi,ecx
  40af14:	8b b7 80 02 00 00    	mov    esi,DWORD PTR [edi+0x280]
  40af1a:	85 f6                	test   esi,esi
  40af1c:	74 1a                	je     0x40af38
  40af1e:	8b ce                	mov    ecx,esi
  40af20:	e8 cb fd ff ff       	call   0x40acf0
  40af25:	56                   	push   esi
  40af26:	e8 25 66 04 00       	call   0x451550
  40af2b:	83 c4 04             	add    esp,0x4
  40af2e:	c7 87 80 02 00 00 00 	mov    DWORD PTR [edi+0x280],0x0
  40af35:	00 00 00 
  40af38:	5f                   	pop    edi
  40af39:	5e                   	pop    esi
  40af3a:	c3                   	ret
  40af3b:	90                   	nop
  40af3c:	90                   	nop
  40af3d:	90                   	nop
  40af3e:	90                   	nop
  40af3f:	90                   	nop
  40af40:	6a ff                	push   0xffffffff
  40af42:	68 2e d1 45 00       	push   0x45d12e
  40af47:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40af4d:	50                   	push   eax
  40af4e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40af55:	81 ec 8c 00 00 00    	sub    esp,0x8c
  40af5b:	56                   	push   esi
  40af5c:	57                   	push   edi
  40af5d:	8b f1                	mov    esi,ecx
  40af5f:	6a 00                	push   0x0
  40af61:	68 b8 0b 00 00       	push   0xbb8
  40af66:	56                   	push   esi
  40af67:	e8 c4 94 02 00       	call   0x434430
  40af6c:	56                   	push   esi
  40af6d:	e8 8e 02 00 00       	call   0x40b200
  40af72:	6a 14                	push   0x14
  40af74:	e8 e6 66 04 00       	call   0x45165f
  40af79:	83 c4 08             	add    esp,0x8
  40af7c:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  40af80:	85 c0                	test   eax,eax
  40af82:	c7 84 24 9c 00 00 00 	mov    DWORD PTR [esp+0x9c],0x0
  40af89:	00 00 00 00 
  40af8d:	74 09                	je     0x40af98
  40af8f:	8b c8                	mov    ecx,eax
  40af91:	e8 4a fd ff ff       	call   0x40ace0
  40af96:	eb 02                	jmp    0x40af9a
  40af98:	33 c0                	xor    eax,eax
  40af9a:	8b bc 24 a4 00 00 00 	mov    edi,DWORD PTR [esp+0xa4]
  40afa1:	89 86 80 02 00 00    	mov    DWORD PTR [esi+0x280],eax
  40afa7:	8d 44 24 14          	lea    eax,[esp+0x14]
  40afab:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  40afaf:	50                   	push   eax
  40afb0:	8d 54 24 50          	lea    edx,[esp+0x50]
  40afb4:	51                   	push   ecx
  40afb5:	8d 44 24 14          	lea    eax,[esp+0x14]
  40afb9:	52                   	push   edx
  40afba:	50                   	push   eax
  40afbb:	57                   	push   edi
  40afbc:	c7 84 24 b0 00 00 00 	mov    DWORD PTR [esp+0xb0],0xffffffff
  40afc3:	ff ff ff ff 
  40afc7:	e8 35 6c 04 00       	call   0x451c01
  40afcc:	8d 4c 24 32          	lea    ecx,[esp+0x32]
  40afd0:	51                   	push   ecx
  40afd1:	e8 ba 71 ff ff       	call   0x402190
  40afd6:	8b 8e 80 02 00 00    	mov    ecx,DWORD PTR [esi+0x280]
  40afdc:	83 c4 18             	add    esp,0x18
  40afdf:	57                   	push   edi
  40afe0:	e8 1b fc ff ff       	call   0x40ac00
  40afe5:	57                   	push   edi
  40afe6:	e8 45 01 04 00       	call   0x44b130
  40afeb:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  40aff1:	83 c4 04             	add    esp,0x4
  40aff4:	8b 42 28             	mov    eax,DWORD PTR [edx+0x28]
  40aff7:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  40affa:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  40affd:	51                   	push   ecx
  40affe:	8b 8e 80 02 00 00    	mov    ecx,DWORD PTR [esi+0x280]
  40b004:	8d 44 24 40          	lea    eax,[esp+0x40]
  40b008:	52                   	push   edx
  40b009:	50                   	push   eax
  40b00a:	e8 41 fd ff ff       	call   0x40ad50
  40b00f:	8b 10                	mov    edx,DWORD PTR [eax]
  40b011:	83 ec 10             	sub    esp,0x10
  40b014:	8b cc                	mov    ecx,esp
  40b016:	89 11                	mov    DWORD PTR [ecx],edx
  40b018:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  40b01b:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  40b01e:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  40b021:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  40b024:	89 51 08             	mov    DWORD PTR [ecx+0x8],edx
  40b027:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  40b02a:	e8 d1 02 04 00       	call   0x44b300
  40b02f:	8b 8c 24 ac 00 00 00 	mov    ecx,DWORD PTR [esp+0xac]
  40b036:	83 c4 18             	add    esp,0x18
  40b039:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40b040:	5f                   	pop    edi
  40b041:	5e                   	pop    esi
  40b042:	81 c4 98 00 00 00    	add    esp,0x98
  40b048:	c2 04 00             	ret    0x4
  40b04b:	90                   	nop
  40b04c:	90                   	nop
  40b04d:	90                   	nop
  40b04e:	90                   	nop
  40b04f:	90                   	nop
  40b050:	56                   	push   esi
  40b051:	8b f1                	mov    esi,ecx
  40b053:	8a 86 19 02 00 00    	mov    al,BYTE PTR [esi+0x219]
  40b059:	84 c0                	test   al,al
  40b05b:	75 39                	jne    0x40b096
  40b05d:	e8 9e 04 04 00       	call   0x44b500
  40b062:	a1 b8 f7 89 00       	mov    eax,ds:0x89f7b8
  40b067:	85 c0                	test   eax,eax
  40b069:	7e 07                	jle    0x40b072
  40b06b:	e8 b0 bf ff ff       	call   0x407020
  40b070:	eb 0b                	jmp    0x40b07d
  40b072:	8b 8e 80 02 00 00    	mov    ecx,DWORD PTR [esi+0x280]
  40b078:	e8 63 fd ff ff       	call   0x40ade0
  40b07d:	e8 1e bd ff ff       	call   0x406da0
  40b082:	e8 09 05 04 00       	call   0x44b590
  40b087:	e8 a4 8d ff ff       	call   0x403e30
  40b08c:	e8 6f 97 ff ff       	call   0x404800
  40b091:	e8 7a d7 ff ff       	call   0x408810
  40b096:	5e                   	pop    esi
  40b097:	c3                   	ret
  40b098:	90                   	nop
  40b099:	90                   	nop
  40b09a:	90                   	nop
  40b09b:	90                   	nop
  40b09c:	90                   	nop
  40b09d:	90                   	nop
  40b09e:	90                   	nop
  40b09f:	90                   	nop
  40b0a0:	51                   	push   ecx
  40b0a1:	c6 81 19 02 00 00 01 	mov    BYTE PTR [ecx+0x219],0x1
  40b0a8:	e8 03 97 02 00       	call   0x4347b0
  40b0ad:	c3                   	ret
  40b0ae:	90                   	nop
  40b0af:	90                   	nop
  40b0b0:	83 ec 10             	sub    esp,0x10
  40b0b3:	56                   	push   esi
  40b0b4:	8b f1                	mov    esi,ecx
  40b0b6:	c6 86 19 02 00 00 00 	mov    BYTE PTR [esi+0x219],0x0
  40b0bd:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  40b0c2:	8b 40 28             	mov    eax,DWORD PTR [eax+0x28]
  40b0c5:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  40b0c8:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  40b0cb:	51                   	push   ecx
  40b0cc:	8b 8e 80 02 00 00    	mov    ecx,DWORD PTR [esi+0x280]
  40b0d2:	8d 44 24 08          	lea    eax,[esp+0x8]
  40b0d6:	52                   	push   edx
  40b0d7:	50                   	push   eax
  40b0d8:	e8 73 fc ff ff       	call   0x40ad50
  40b0dd:	8b 10                	mov    edx,DWORD PTR [eax]
  40b0df:	83 ec 10             	sub    esp,0x10
  40b0e2:	8b cc                	mov    ecx,esp
  40b0e4:	89 11                	mov    DWORD PTR [ecx],edx
  40b0e6:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  40b0e9:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  40b0ec:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  40b0ef:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  40b0f2:	89 51 08             	mov    DWORD PTR [ecx+0x8],edx
  40b0f5:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  40b0f8:	e8 03 02 04 00       	call   0x44b300
  40b0fd:	56                   	push   esi
  40b0fe:	e8 fd 00 00 00       	call   0x40b200
  40b103:	8b 8e 80 02 00 00    	mov    ecx,DWORD PTR [esi+0x280]
  40b109:	83 c4 1c             	add    esp,0x1c
  40b10c:	e8 0f fd ff ff       	call   0x40ae20
  40b111:	c6 05 1e f8 89 00 00 	mov    BYTE PTR ds:0x89f81e,0x0
  40b118:	5e                   	pop    esi
  40b119:	83 c4 10             	add    esp,0x10
  40b11c:	c3                   	ret
  40b11d:	90                   	nop
  40b11e:	90                   	nop
  40b11f:	90                   	nop
  40b120:	c6 81 18 02 00 00 00 	mov    BYTE PTR [ecx+0x218],0x0
  40b127:	c3                   	ret
  40b128:	90                   	nop
  40b129:	90                   	nop
  40b12a:	90                   	nop
  40b12b:	90                   	nop
  40b12c:	90                   	nop
  40b12d:	90                   	nop
  40b12e:	90                   	nop
  40b12f:	90                   	nop
  40b130:	56                   	push   esi
  40b131:	8b f1                	mov    esi,ecx
  40b133:	85 f6                	test   esi,esi
  40b135:	74 0e                	je     0x40b145
  40b137:	e8 74 fd ff ff       	call   0x40aeb0
  40b13c:	56                   	push   esi
  40b13d:	e8 0e 64 04 00       	call   0x451550
  40b142:	83 c4 04             	add    esp,0x4
  40b145:	33 c0                	xor    eax,eax
  40b147:	5e                   	pop    esi
  40b148:	c3                   	ret
  40b149:	90                   	nop
  40b14a:	90                   	nop
  40b14b:	90                   	nop
  40b14c:	90                   	nop
  40b14d:	90                   	nop
  40b14e:	90                   	nop
  40b14f:	90                   	nop
  40b150:	56                   	push   esi
  40b151:	8b f1                	mov    esi,ecx
  40b153:	e8 78 01 00 00       	call   0x40b2d0
  40b158:	e8 43 84 ff ff       	call   0x4035a0
  40b15d:	84 c0                	test   al,al
  40b15f:	75 59                	jne    0x40b1ba
  40b161:	8a 86 18 02 00 00    	mov    al,BYTE PTR [esi+0x218]
  40b167:	84 c0                	test   al,al
  40b169:	74 48                	je     0x40b1b3
  40b16b:	68 b8 0b 00 00       	push   0xbb8
  40b170:	56                   	push   esi
  40b171:	8b ce                	mov    ecx,esi
  40b173:	e8 88 93 02 00       	call   0x434500
  40b178:	8a 86 19 02 00 00    	mov    al,BYTE PTR [esi+0x219]
  40b17e:	84 c0                	test   al,al
  40b180:	75 0b                	jne    0x40b18d
  40b182:	8b 8e 80 02 00 00    	mov    ecx,DWORD PTR [esi+0x280]
  40b188:	e8 13 fc ff ff       	call   0x40ada0
  40b18d:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  40b193:	68 07 b2 01 00       	push   0x1b207
  40b198:	68 07 b2 01 00       	push   0x1b207
  40b19d:	68 07 b2 01 00       	push   0x1b207
  40b1a2:	e8 09 f1 ff ff       	call   0x40a2b0
  40b1a7:	e8 b4 01 04 00       	call   0x44b360
  40b1ac:	e8 3f ba ff ff       	call   0x406bf0
  40b1b1:	5e                   	pop    esi
  40b1b2:	c3                   	ret
  40b1b3:	8b 06                	mov    eax,DWORD PTR [esi]
  40b1b5:	8b ce                	mov    ecx,esi
  40b1b7:	ff 50 14             	call   DWORD PTR [eax+0x14]
  40b1ba:	5e                   	pop    esi
  40b1bb:	c3                   	ret
  40b1bc:	90                   	nop
  40b1bd:	90                   	nop
  40b1be:	90                   	nop
  40b1bf:	90                   	nop
