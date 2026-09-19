
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0043e460 <.text+0x3d460>:
  43e460:	6a ff                	push   0xffffffff
  43e462:	68 7b f6 45 00       	push   0x45f67b
  43e467:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43e46d:	50                   	push   eax
  43e46e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  43e475:	51                   	push   ecx
  43e476:	55                   	push   ebp
  43e477:	57                   	push   edi
  43e478:	8b e9                	mov    ebp,ecx
  43e47a:	e8 11 01 00 00       	call   0x43e590
  43e47f:	68 34 01 00 00       	push   0x134
  43e484:	e8 d6 31 01 00       	call   0x45165f
  43e489:	83 c4 04             	add    esp,0x4
  43e48c:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  43e490:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  43e494:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  43e49b:	00 
  43e49c:	85 c0                	test   eax,eax
  43e49e:	74 11                	je     0x43e4b1
  43e4a0:	8b 0d 74 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c74
  43e4a6:	51                   	push   ecx
  43e4a7:	57                   	push   edi
  43e4a8:	8b c8                	mov    ecx,eax
  43e4aa:	e8 b1 a6 fe ff       	call   0x428b60
  43e4af:	eb 02                	jmp    0x43e4b3
  43e4b1:	33 c0                	xor    eax,eax
  43e4b3:	85 c0                	test   eax,eax
  43e4b5:	0f 95 c1             	setne  cl
  43e4b8:	84 c9                	test   cl,cl
  43e4ba:	89 85 5c 03 00 00    	mov    DWORD PTR [ebp+0x35c],eax
  43e4c0:	88 4d 08             	mov    BYTE PTR [ebp+0x8],cl
  43e4c3:	0f 84 a4 00 00 00    	je     0x43e56d
  43e4c9:	8a 88 2d 01 00 00    	mov    cl,BYTE PTR [eax+0x12d]
  43e4cf:	84 c9                	test   cl,cl
  43e4d1:	75 04                	jne    0x43e4d7
  43e4d3:	33 c9                	xor    ecx,ecx
  43e4d5:	eb 09                	jmp    0x43e4e0
  43e4d7:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  43e4da:	33 c9                	xor    ecx,ecx
  43e4dc:	66 8b 4a 14          	mov    cx,WORD PTR [edx+0x14]
  43e4e0:	56                   	push   esi
  43e4e1:	8b 75 7c             	mov    esi,DWORD PTR [ebp+0x7c]
  43e4e4:	85 f6                	test   esi,esi
  43e4e6:	89 4d 18             	mov    DWORD PTR [ebp+0x18],ecx
  43e4e9:	c7 45 2c 00 00 00 00 	mov    DWORD PTR [ebp+0x2c],0x0
  43e4f0:	ba 01 00 00 00       	mov    edx,0x1
  43e4f5:	74 03                	je     0x43e4fa
  43e4f7:	89 55 2c             	mov    DWORD PTR [ebp+0x2c],edx
  43e4fa:	89 4d 30             	mov    DWORD PTR [ebp+0x30],ecx
  43e4fd:	8a 88 2d 01 00 00    	mov    cl,BYTE PTR [eax+0x12d]
  43e503:	84 c9                	test   cl,cl
  43e505:	75 04                	jne    0x43e50b
  43e507:	33 c0                	xor    eax,eax
  43e509:	eb 09                	jmp    0x43e514
  43e50b:	8b 48 34             	mov    ecx,DWORD PTR [eax+0x34]
  43e50e:	33 c0                	xor    eax,eax
  43e510:	66 8b 41 22          	mov    ax,WORD PTR [ecx+0x22]
  43e514:	89 45 0c             	mov    DWORD PTR [ebp+0xc],eax
  43e517:	83 c9 ff             	or     ecx,0xffffffff
  43e51a:	33 c0                	xor    eax,eax
  43e51c:	88 55 0a             	mov    BYTE PTR [ebp+0xa],dl
  43e51f:	c7 45 1c 02 00 00 00 	mov    DWORD PTR [ebp+0x1c],0x2
  43e526:	c7 45 54 00 00 00 00 	mov    DWORD PTR [ebp+0x54],0x0
  43e52d:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  43e52f:	f7 d1                	not    ecx
  43e531:	2b f9                	sub    edi,ecx
  43e533:	8d 95 90 00 00 00    	lea    edx,[ebp+0x90]
  43e539:	8b c1                	mov    eax,ecx
  43e53b:	8b f7                	mov    esi,edi
  43e53d:	8b fa                	mov    edi,edx
  43e53f:	c1 e9 02             	shr    ecx,0x2
  43e542:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43e544:	8b c8                	mov    ecx,eax
  43e546:	83 e1 03             	and    ecx,0x3
  43e549:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  43e54b:	8b 85 5c 03 00 00    	mov    eax,DWORD PTR [ebp+0x35c]
  43e551:	5e                   	pop    esi
  43e552:	8a 88 2d 01 00 00    	mov    cl,BYTE PTR [eax+0x12d]
  43e558:	84 c9                	test   cl,cl
  43e55a:	75 04                	jne    0x43e560
  43e55c:	33 c0                	xor    eax,eax
  43e55e:	eb 07                	jmp    0x43e567
  43e560:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  43e563:	66 8b 41 48          	mov    ax,WORD PTR [ecx+0x48]
  43e567:	0f bf d0             	movsx  edx,ax
  43e56a:	89 55 34             	mov    DWORD PTR [ebp+0x34],edx
  43e56d:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  43e571:	8a 45 08             	mov    al,BYTE PTR [ebp+0x8]
  43e574:	5f                   	pop    edi
  43e575:	5d                   	pop    ebp
  43e576:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43e57d:	83 c4 10             	add    esp,0x10
  43e580:	c2 04 00             	ret    0x4
  43e583:	90                   	nop
  43e584:	90                   	nop
  43e585:	90                   	nop
  43e586:	90                   	nop
  43e587:	90                   	nop
  43e588:	90                   	nop
  43e589:	90                   	nop
  43e58a:	90                   	nop
  43e58b:	90                   	nop
  43e58c:	90                   	nop
  43e58d:	90                   	nop
  43e58e:	90                   	nop
  43e58f:	90                   	nop
  43e590:	53                   	push   ebx
  43e591:	56                   	push   esi
  43e592:	8b f1                	mov    esi,ecx
  43e594:	57                   	push   edi
  43e595:	33 db                	xor    ebx,ebx
  43e597:	8b be 5c 03 00 00    	mov    edi,DWORD PTR [esi+0x35c]
  43e59d:	c7 46 1c 02 00 00 00 	mov    DWORD PTR [esi+0x1c],0x2
  43e5a4:	3b fb                	cmp    edi,ebx
  43e5a6:	74 16                	je     0x43e5be
  43e5a8:	8b cf                	mov    ecx,edi
  43e5aa:	e8 d1 a6 fe ff       	call   0x428c80
  43e5af:	57                   	push   edi
  43e5b0:	e8 9b 2f 01 00       	call   0x451550
  43e5b5:	83 c4 04             	add    esp,0x4
  43e5b8:	89 9e 5c 03 00 00    	mov    DWORD PTR [esi+0x35c],ebx
  43e5be:	8b 7e 04             	mov    edi,DWORD PTR [esi+0x4]
  43e5c1:	3b fb                	cmp    edi,ebx
  43e5c3:	74 13                	je     0x43e5d8
  43e5c5:	8b cf                	mov    ecx,edi
  43e5c7:	e8 b4 a6 fe ff       	call   0x428c80
  43e5cc:	57                   	push   edi
  43e5cd:	e8 7e 2f 01 00       	call   0x451550
  43e5d2:	83 c4 04             	add    esp,0x4
  43e5d5:	89 5e 04             	mov    DWORD PTR [esi+0x4],ebx
  43e5d8:	88 5e 08             	mov    BYTE PTR [esi+0x8],bl
  43e5db:	5f                   	pop    edi
  43e5dc:	5e                   	pop    esi
  43e5dd:	5b                   	pop    ebx
  43e5de:	c3                   	ret
  43e5df:	90                   	nop
  43e5e0:	8a 41 08             	mov    al,BYTE PTR [ecx+0x8]
  43e5e3:	84 c0                	test   al,al
  43e5e5:	74 48                	je     0x43e62f
  43e5e7:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  43e5eb:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  43e5ef:	3b c2                	cmp    eax,edx
  43e5f1:	56                   	push   esi
  43e5f2:	7e 06                	jle    0x43e5fa
  43e5f4:	8b f0                	mov    esi,eax
  43e5f6:	8b c2                	mov    eax,edx
  43e5f8:	8b d6                	mov    edx,esi
  43e5fa:	85 c0                	test   eax,eax
  43e5fc:	7d 02                	jge    0x43e600
  43e5fe:	33 c0                	xor    eax,eax
  43e600:	85 d2                	test   edx,edx
  43e602:	7d 03                	jge    0x43e607
  43e604:	8b 51 18             	mov    edx,DWORD PTR [ecx+0x18]
  43e607:	85 c0                	test   eax,eax
  43e609:	7c 0f                	jl     0x43e61a
  43e60b:	8b 71 18             	mov    esi,DWORD PTR [ecx+0x18]
  43e60e:	3b c6                	cmp    eax,esi
  43e610:	7e 05                	jle    0x43e617
  43e612:	89 71 2c             	mov    DWORD PTR [ecx+0x2c],esi
  43e615:	eb 03                	jmp    0x43e61a
  43e617:	89 41 2c             	mov    DWORD PTR [ecx+0x2c],eax
  43e61a:	85 d2                	test   edx,edx
  43e61c:	5e                   	pop    esi
  43e61d:	7c 10                	jl     0x43e62f
  43e61f:	8b 41 18             	mov    eax,DWORD PTR [ecx+0x18]
  43e622:	3b d0                	cmp    edx,eax
  43e624:	7e 06                	jle    0x43e62c
  43e626:	89 41 30             	mov    DWORD PTR [ecx+0x30],eax
  43e629:	c2 08 00             	ret    0x8
  43e62c:	89 51 30             	mov    DWORD PTR [ecx+0x30],edx
  43e62f:	c2 08 00             	ret    0x8
  43e632:	90                   	nop
  43e633:	90                   	nop
  43e634:	90                   	nop
  43e635:	90                   	nop
  43e636:	90                   	nop
  43e637:	90                   	nop
  43e638:	90                   	nop
  43e639:	90                   	nop
  43e63a:	90                   	nop
  43e63b:	90                   	nop
  43e63c:	90                   	nop
  43e63d:	90                   	nop
  43e63e:	90                   	nop
  43e63f:	90                   	nop
  43e640:	56                   	push   esi
  43e641:	8b f1                	mov    esi,ecx
  43e643:	8a 46 08             	mov    al,BYTE PTR [esi+0x8]
  43e646:	84 c0                	test   al,al
  43e648:	74 1c                	je     0x43e666
  43e64a:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  43e64e:	8d 46 10             	lea    eax,[esi+0x10]
  43e651:	50                   	push   eax
  43e652:	89 08                	mov    DWORD PTR [eax],ecx
  43e654:	8b ce                	mov    ecx,esi
  43e656:	e8 c5 00 00 00       	call   0x43e720
  43e65b:	b8 01 00 00 00       	mov    eax,0x1
  43e660:	89 46 1c             	mov    DWORD PTR [esi+0x1c],eax
  43e663:	88 46 38             	mov    BYTE PTR [esi+0x38],al
  43e666:	5e                   	pop    esi
  43e667:	c2 04 00             	ret    0x4
  43e66a:	90                   	nop
  43e66b:	90                   	nop
  43e66c:	90                   	nop
  43e66d:	90                   	nop
  43e66e:	90                   	nop
  43e66f:	90                   	nop
  43e670:	56                   	push   esi
  43e671:	8b f1                	mov    esi,ecx
  43e673:	8a 46 08             	mov    al,BYTE PTR [esi+0x8]
  43e676:	84 c0                	test   al,al
  43e678:	74 1c                	je     0x43e696
  43e67a:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  43e67e:	8d 46 10             	lea    eax,[esi+0x10]
  43e681:	50                   	push   eax
  43e682:	89 08                	mov    DWORD PTR [eax],ecx
  43e684:	8b ce                	mov    ecx,esi
  43e686:	e8 95 00 00 00       	call   0x43e720
  43e68b:	c7 46 1c 07 00 00 00 	mov    DWORD PTR [esi+0x1c],0x7
  43e692:	c6 46 38 01          	mov    BYTE PTR [esi+0x38],0x1
  43e696:	5e                   	pop    esi
  43e697:	c2 04 00             	ret    0x4
  43e69a:	90                   	nop
  43e69b:	90                   	nop
  43e69c:	90                   	nop
  43e69d:	90                   	nop
  43e69e:	90                   	nop
  43e69f:	90                   	nop
  43e6a0:	56                   	push   esi
  43e6a1:	8b f1                	mov    esi,ecx
  43e6a3:	8a 46 08             	mov    al,BYTE PTR [esi+0x8]
  43e6a6:	84 c0                	test   al,al
  43e6a8:	74 1c                	je     0x43e6c6
  43e6aa:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  43e6ae:	8d 46 10             	lea    eax,[esi+0x10]
  43e6b1:	50                   	push   eax
  43e6b2:	89 08                	mov    DWORD PTR [eax],ecx
  43e6b4:	8b ce                	mov    ecx,esi
  43e6b6:	e8 65 00 00 00       	call   0x43e720
  43e6bb:	c7 46 1c 05 00 00 00 	mov    DWORD PTR [esi+0x1c],0x5
  43e6c2:	c6 46 38 01          	mov    BYTE PTR [esi+0x38],0x1
  43e6c6:	5e                   	pop    esi
  43e6c7:	c2 04 00             	ret    0x4
  43e6ca:	90                   	nop
  43e6cb:	90                   	nop
  43e6cc:	90                   	nop
  43e6cd:	90                   	nop
  43e6ce:	90                   	nop
  43e6cf:	90                   	nop
  43e6d0:	8a 41 08             	mov    al,BYTE PTR [ecx+0x8]
  43e6d3:	84 c0                	test   al,al
  43e6d5:	74 0b                	je     0x43e6e2
  43e6d7:	c7 41 1c 02 00 00 00 	mov    DWORD PTR [ecx+0x1c],0x2
  43e6de:	c6 41 38 01          	mov    BYTE PTR [ecx+0x38],0x1
  43e6e2:	c3                   	ret
  43e6e3:	90                   	nop
  43e6e4:	90                   	nop
  43e6e5:	90                   	nop
  43e6e6:	90                   	nop
  43e6e7:	90                   	nop
  43e6e8:	90                   	nop
  43e6e9:	90                   	nop
  43e6ea:	90                   	nop
  43e6eb:	90                   	nop
  43e6ec:	90                   	nop
  43e6ed:	90                   	nop
  43e6ee:	90                   	nop
  43e6ef:	90                   	nop
  43e6f0:	56                   	push   esi
  43e6f1:	8b f1                	mov    esi,ecx
  43e6f3:	8a 46 08             	mov    al,BYTE PTR [esi+0x8]
  43e6f6:	84 c0                	test   al,al
  43e6f8:	74 20                	je     0x43e71a
  43e6fa:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  43e6fe:	85 c9                	test   ecx,ecx
  43e700:	7c 0d                	jl     0x43e70f
  43e702:	8d 46 10             	lea    eax,[esi+0x10]
  43e705:	50                   	push   eax
  43e706:	89 08                	mov    DWORD PTR [eax],ecx
  43e708:	8b ce                	mov    ecx,esi
  43e70a:	e8 11 00 00 00       	call   0x43e720
  43e70f:	c7 46 1c 03 00 00 00 	mov    DWORD PTR [esi+0x1c],0x3
  43e716:	c6 46 38 01          	mov    BYTE PTR [esi+0x38],0x1
  43e71a:	5e                   	pop    esi
  43e71b:	c2 04 00             	ret    0x4
  43e71e:	90                   	nop
  43e71f:	90                   	nop
  43e720:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  43e724:	8b 51 30             	mov    edx,DWORD PTR [ecx+0x30]
  43e727:	56                   	push   esi
  43e728:	8b 30                	mov    esi,DWORD PTR [eax]
  43e72a:	3b f2                	cmp    esi,edx
  43e72c:	5e                   	pop    esi
  43e72d:	7c 03                	jl     0x43e732
  43e72f:	4a                   	dec    edx
  43e730:	89 10                	mov    DWORD PTR [eax],edx
  43e732:	8b 49 2c             	mov    ecx,DWORD PTR [ecx+0x2c]
  43e735:	8b 10                	mov    edx,DWORD PTR [eax]
  43e737:	3b d1                	cmp    edx,ecx
  43e739:	7d 02                	jge    0x43e73d
  43e73b:	89 08                	mov    DWORD PTR [eax],ecx
  43e73d:	c2 04 00             	ret    0x4
