
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443310 <.text+0x42310>:
  443310:	51                   	push   ecx
  443311:	53                   	push   ebx
  443312:	55                   	push   ebp
  443313:	56                   	push   esi
  443314:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
  443318:	8b e9                	mov    ebp,ecx
  44331a:	57                   	push   edi
  44331b:	8d 04 f6             	lea    eax,[esi+esi*8]
  44331e:	8d 04 86             	lea    eax,[esi+eax*4]
  443321:	8d 0c 40             	lea    ecx,[eax+eax*2]
  443324:	8d 14 ce             	lea    edx,[esi+ecx*8]
  443327:	8d 7c 95 00          	lea    edi,[ebp+edx*4+0x0]
  44332b:	8d 9f e4 03 00 00    	lea    ebx,[edi+0x3e4]
  443331:	8b cb                	mov    ecx,ebx
  443333:	e8 f8 f1 fd ff       	call   0x422530
  443338:	84 c0                	test   al,al
  44333a:	0f 84 19 01 00 00    	je     0x443459
  443340:	8d 8d d4 e2 00 00    	lea    ecx,[ebp+0xe2d4]
  443346:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  44334a:	e8 61 0e fe ff       	call   0x4241b0
  44334f:	8b 87 2c 0c 00 00    	mov    eax,DWORD PTR [edi+0xc2c]
  443355:	8b 8f 84 0b 00 00    	mov    ecx,DWORD PTR [edi+0xb84]
  44335b:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  44335f:	8b 40 18             	mov    eax,DWORD PTR [eax+0x18]
  443362:	83 f8 02             	cmp    eax,0x2
  443365:	0f 84 82 00 00 00    	je     0x4433ed
  44336b:	83 f8 01             	cmp    eax,0x1
  44336e:	74 7d                	je     0x4433ed
  443370:	83 f8 04             	cmp    eax,0x4
  443373:	74 78                	je     0x4433ed
  443375:	51                   	push   ecx
  443376:	56                   	push   esi
  443377:	6a 6a                	push   0x6a
  443379:	8b cb                	mov    ecx,ebx
  44337b:	e8 a0 ed fd ff       	call   0x422120
  443380:	84 c0                	test   al,al
  443382:	0f 84 c7 00 00 00    	je     0x44344f
  443388:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  44338c:	8b 40 18             	mov    eax,DWORD PTR [eax+0x18]
  44338f:	83 f8 05             	cmp    eax,0x5
  443392:	75 0c                	jne    0x4433a0
  443394:	c7 87 c8 10 00 00 c0 	mov    DWORD PTR [edi+0x10c8],0xbc0
  44339b:	0b 00 00 
  44339e:	eb 2c                	jmp    0x4433cc
  4433a0:	83 f8 06             	cmp    eax,0x6
  4433a3:	75 0c                	jne    0x4433b1
  4433a5:	c7 87 c8 10 00 00 c6 	mov    DWORD PTR [edi+0x10c8],0xbc6
  4433ac:	0b 00 00 
  4433af:	eb 1b                	jmp    0x4433cc
  4433b1:	83 f8 04             	cmp    eax,0x4
  4433b4:	75 0c                	jne    0x4433c2
  4433b6:	c7 87 c8 10 00 00 c7 	mov    DWORD PTR [edi+0x10c8],0xbc7
  4433bd:	0b 00 00 
  4433c0:	eb 0a                	jmp    0x4433cc
  4433c2:	c7 87 c8 10 00 00 bb 	mov    DWORD PTR [edi+0x10c8],0xbbb
  4433c9:	0b 00 00 
  4433cc:	6a 03                	push   0x3
  4433ce:	8d 8f 8c 10 00 00    	lea    ecx,[edi+0x108c]
  4433d4:	e8 a7 0b fe ff       	call   0x423f80
  4433d9:	c7 87 c4 10 00 00 d6 	mov    DWORD PTR [edi+0x10c4],0x7d6
  4433e0:	07 00 00 
  4433e3:	5f                   	pop    edi
  4433e4:	5e                   	pop    esi
  4433e5:	5d                   	pop    ebp
  4433e6:	b0 01                	mov    al,0x1
  4433e8:	5b                   	pop    ebx
  4433e9:	59                   	pop    ecx
  4433ea:	c2 04 00             	ret    0x4
  4433ed:	51                   	push   ecx
  4433ee:	56                   	push   esi
  4433ef:	6a 6a                	push   0x6a
  4433f1:	8b cb                	mov    ecx,ebx
  4433f3:	e8 28 ed fd ff       	call   0x422120
  4433f8:	84 c0                	test   al,al
  4433fa:	74 53                	je     0x44344f
  4433fc:	8a 85 2d e5 00 00    	mov    al,BYTE PTR [ebp+0xe52d]
  443402:	89 b5 24 e2 00 00    	mov    DWORD PTR [ebp+0xe224],esi
  443408:	84 c0                	test   al,al
  44340a:	75 13                	jne    0x44341f
  44340c:	8b 8d ac e2 00 00    	mov    ecx,DWORD PTR [ebp+0xe2ac]
  443412:	c6 85 2d e5 00 00 01 	mov    BYTE PTR [ebp+0xe52d],0x1
  443419:	89 8d 30 e5 00 00    	mov    DWORD PTR [ebp+0xe530],ecx
  44341f:	6a 03                	push   0x3
  443421:	8d 8f 8c 10 00 00    	lea    ecx,[edi+0x108c]
  443427:	c7 87 c8 10 00 00 bc 	mov    DWORD PTR [edi+0x10c8],0xbbc
  44342e:	0b 00 00 
  443431:	e8 4a 0b fe ff       	call   0x423f80
  443436:	c7 87 c4 10 00 00 d6 	mov    DWORD PTR [edi+0x10c4],0x7d6
  44343d:	07 00 00 
  443440:	5f                   	pop    edi
  443441:	32 c0                	xor    al,al
  443443:	5e                   	pop    esi
  443444:	5d                   	pop    ebp
  443445:	a2 dc 96 46 00       	mov    ds:0x4696dc,al
  44344a:	5b                   	pop    ebx
  44344b:	59                   	pop    ecx
  44344c:	c2 04 00             	ret    0x4
  44344f:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  443453:	56                   	push   esi
  443454:	e8 17 0d fe ff       	call   0x424170
  443459:	5f                   	pop    edi
  44345a:	5e                   	pop    esi
  44345b:	5d                   	pop    ebp
  44345c:	32 c0                	xor    al,al
  44345e:	5b                   	pop    ebx
  44345f:	59                   	pop    ecx
  443460:	c2 04 00             	ret    0x4
  443463:	90                   	nop
  443464:	90                   	nop
  443465:	90                   	nop
  443466:	90                   	nop
  443467:	90                   	nop
  443468:	90                   	nop
  443469:	90                   	nop
  44346a:	90                   	nop
  44346b:	90                   	nop
  44346c:	90                   	nop
  44346d:	90                   	nop
  44346e:	90                   	nop
  44346f:	90                   	nop
  443470:	56                   	push   esi
  443471:	8b f1                	mov    esi,ecx
  443473:	57                   	push   edi
  443474:	8d 8e d4 e2 00 00    	lea    ecx,[esi+0xe2d4]
  44347a:	e8 31 0d fe ff       	call   0x4241b0
  44347f:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  443483:	6a 00                	push   0x0
  443485:	6a 00                	push   0x0
  443487:	6a 69                	push   0x69
  443489:	8d 0c c0             	lea    ecx,[eax+eax*8]
  44348c:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  44348f:	8d 14 49             	lea    edx,[ecx+ecx*2]
  443492:	8d 04 d0             	lea    eax,[eax+edx*8]
  443495:	8d 34 86             	lea    esi,[esi+eax*4]
  443498:	8d 8e e4 03 00 00    	lea    ecx,[esi+0x3e4]
  44349e:	c7 86 c8 10 00 00 be 	mov    DWORD PTR [esi+0x10c8],0xbbe
  4434a5:	0b 00 00 
  4434a8:	e8 73 ec fd ff       	call   0x422120
  4434ad:	8d be 8c 10 00 00    	lea    edi,[esi+0x108c]
  4434b3:	68 a0 86 01 00       	push   0x186a0
  4434b8:	6a 03                	push   0x3
  4434ba:	6a 0c                	push   0xc
  4434bc:	8b cf                	mov    ecx,edi
  4434be:	e8 cd 0a fe ff       	call   0x423f90
  4434c3:	6a 03                	push   0x3
  4434c5:	8b cf                	mov    ecx,edi
  4434c7:	e8 b4 0a fe ff       	call   0x423f80
  4434cc:	c7 86 c4 10 00 00 d6 	mov    DWORD PTR [esi+0x10c4],0x7d6
  4434d3:	07 00 00 
  4434d6:	5f                   	pop    edi
  4434d7:	b0 01                	mov    al,0x1
  4434d9:	5e                   	pop    esi
  4434da:	c2 04 00             	ret    0x4
  4434dd:	90                   	nop
  4434de:	90                   	nop
  4434df:	90                   	nop
  4434e0:	81 c1 d4 e2 00 00    	add    ecx,0xe2d4
  4434e6:	e8 c5 0c fe ff       	call   0x4241b0
  4434eb:	b0 01                	mov    al,0x1
  4434ed:	c2 04 00             	ret    0x4
  4434f0:	53                   	push   ebx
  4434f1:	55                   	push   ebp
  4434f2:	56                   	push   esi
  4434f3:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  4434f7:	8b d9                	mov    ebx,ecx
  4434f9:	57                   	push   edi
  4434fa:	8d 04 f6             	lea    eax,[esi+esi*8]
  4434fd:	8d 04 86             	lea    eax,[esi+eax*4]
  443500:	8d 0c 40             	lea    ecx,[eax+eax*2]
  443503:	8d 14 ce             	lea    edx,[esi+ecx*8]
  443506:	8d 3c 93             	lea    edi,[ebx+edx*4]
  443509:	8d af e4 03 00 00    	lea    ebp,[edi+0x3e4]
  44350f:	8b cd                	mov    ecx,ebp
  443511:	e8 1a f0 fd ff       	call   0x422530
  443516:	84 c0                	test   al,al
  443518:	74 65                	je     0x44357f
  44351a:	8d 8b d4 e2 00 00    	lea    ecx,[ebx+0xe2d4]
  443520:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  443524:	e8 87 0c fe ff       	call   0x4241b0
  443529:	8b 87 84 0b 00 00    	mov    eax,DWORD PTR [edi+0xb84]
  44352f:	8b cd                	mov    ecx,ebp
  443531:	50                   	push   eax
  443532:	56                   	push   esi
  443533:	6a 6e                	push   0x6e
  443535:	e8 e6 eb fd ff       	call   0x422120
  44353a:	84 c0                	test   al,al
  44353c:	74 37                	je     0x443575
  44353e:	89 b3 24 e2 00 00    	mov    DWORD PTR [ebx+0xe224],esi
  443544:	6a 03                	push   0x3
  443546:	8d 8f 8c 10 00 00    	lea    ecx,[edi+0x108c]
  44354c:	c7 87 c8 10 00 00 c1 	mov    DWORD PTR [edi+0x10c8],0xbc1
  443553:	0b 00 00 
  443556:	e8 25 0a fe ff       	call   0x423f80
  44355b:	c7 87 c4 10 00 00 d6 	mov    DWORD PTR [edi+0x10c4],0x7d6
  443562:	07 00 00 
  443565:	5f                   	pop    edi
  443566:	5e                   	pop    esi
  443567:	5d                   	pop    ebp
  443568:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  44356f:	32 c0                	xor    al,al
  443571:	5b                   	pop    ebx
  443572:	c2 04 00             	ret    0x4
  443575:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  443579:	56                   	push   esi
  44357a:	e8 f1 0b fe ff       	call   0x424170
  44357f:	5f                   	pop    edi
  443580:	5e                   	pop    esi
  443581:	5d                   	pop    ebp
  443582:	32 c0                	xor    al,al
  443584:	5b                   	pop    ebx
  443585:	c2                   	.byte 0xc2
