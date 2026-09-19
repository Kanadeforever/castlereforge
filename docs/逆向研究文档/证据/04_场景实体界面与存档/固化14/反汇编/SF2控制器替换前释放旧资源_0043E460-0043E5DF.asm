
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
