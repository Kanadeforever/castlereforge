
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420440 <.text+0x1f440>:
  420440:	56                   	push   esi
  420441:	8b f1                	mov    esi,ecx
  420443:	57                   	push   edi
  420444:	6a 09                	push   0x9
  420446:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  42044c:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  42044f:	8b 88 5c 03 00 00    	mov    ecx,DWORD PTR [eax+0x35c]
  420455:	e8 46 9d 00 00       	call   0x42a1a0
  42045a:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420460:	8b f8                	mov    edi,eax
  420462:	6a 08                	push   0x8
  420464:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  420467:	8b 88 5c 03 00 00    	mov    ecx,DWORD PTR [eax+0x35c]
  42046d:	e8 2e 9d 00 00       	call   0x42a1a0
  420472:	83 ff 01             	cmp    edi,0x1
  420475:	75 20                	jne    0x420497
  420477:	8b 86 a0 07 00 00    	mov    eax,DWORD PTR [esi+0x7a0]
  42047d:	8b 14 85 94 fd 89 00 	mov    edx,DWORD PTR [eax*4+0x89fd94]
  420484:	8b 8a a4 0b 00 00    	mov    ecx,DWORD PTR [edx+0xba4]
  42048a:	e8 61 cd 01 00       	call   0x43d1f0
  42048f:	05 e8 03 00 00       	add    eax,0x3e8
  420494:	57                   	push   edi
  420495:	eb 20                	jmp    0x4204b7
  420497:	83 ff 02             	cmp    edi,0x2
  42049a:	74 05                	je     0x4204a1
  42049c:	83 f8 01             	cmp    eax,0x1
  42049f:	75 25                	jne    0x4204c6
  4204a1:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4204a3:	8b 14 8d 94 fd 89 00 	mov    edx,DWORD PTR [ecx*4+0x89fd94]
  4204aa:	8b 8a a4 0b 00 00    	mov    ecx,DWORD PTR [edx+0xba4]
  4204b0:	e8 3b cd 01 00       	call   0x43d1f0
  4204b5:	6a 00                	push   0x0
  4204b7:	8b 0d 80 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd80
  4204bd:	50                   	push   eax
  4204be:	8b 06                	mov    eax,DWORD PTR [esi]
  4204c0:	50                   	push   eax
  4204c1:	e8 1a d2 01 00       	call   0x43d6e0
  4204c6:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  4204cc:	bf 64 00 00 00       	mov    edi,0x64
  4204d1:	83 f8 01             	cmp    eax,0x1
  4204d4:	75 10                	jne    0x4204e6
  4204d6:	6a 07                	push   0x7
  4204d8:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4204dd:	e8 6e d5 01 00       	call   0x43da50
  4204e2:	84 c0                	test   al,al
  4204e4:	75 1a                	jne    0x420500
  4204e6:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  4204ec:	85 c0                	test   eax,eax
  4204ee:	75 15                	jne    0x420505
  4204f0:	6a 07                	push   0x7
  4204f2:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4204f7:	e8 54 d5 01 00       	call   0x43da50
  4204fc:	84 c0                	test   al,al
  4204fe:	74 05                	je     0x420505
  420500:	bf c8 00 00 00       	mov    edi,0xc8
  420505:	83 be 50 08 00 00 01 	cmp    DWORD PTR [esi+0x850],0x1
  42050c:	75 10                	jne    0x42051e
  42050e:	6a 08                	push   0x8
  420510:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  420515:	e8 36 d5 01 00       	call   0x43da50
  42051a:	84 c0                	test   al,al
  42051c:	75 1a                	jne    0x420538
  42051e:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  420524:	85 c0                	test   eax,eax
  420526:	75 2b                	jne    0x420553
  420528:	6a 08                	push   0x8
  42052a:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  42052f:	e8 1c d5 01 00       	call   0x43da50
  420534:	84 c0                	test   al,al
  420536:	74 1b                	je     0x420553
  420538:	8d 04 bf             	lea    eax,[edi+edi*4]
  42053b:	8d 0c 80             	lea    ecx,[eax+eax*4]
  42053e:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420543:	d1 e1                	shl    ecx,1
  420545:	f7 e9                	imul   ecx
  420547:	c1 fa 05             	sar    edx,0x5
  42054a:	8b ca                	mov    ecx,edx
  42054c:	c1 e9 1f             	shr    ecx,0x1f
  42054f:	03 d1                	add    edx,ecx
  420551:	8b fa                	mov    edi,edx
  420553:	8b 86 38 0a 00 00    	mov    eax,DWORD PTR [esi+0xa38]
  420559:	85 c0                	test   eax,eax
  42055b:	7e 13                	jle    0x420570
  42055d:	b8 67 66 66 66       	mov    eax,0x66666667
  420562:	f7 ef                	imul   edi
  420564:	c1 fa 02             	sar    edx,0x2
  420567:	8b c2                	mov    eax,edx
  420569:	c1 e8 1f             	shr    eax,0x1f
  42056c:	03 d0                	add    edx,eax
  42056e:	8b fa                	mov    edi,edx
  420570:	8b 86 74 0a 00 00    	mov    eax,DWORD PTR [esi+0xa74]
  420576:	85 c0                	test   eax,eax
  420578:	7e 1c                	jle    0x420596
  42057a:	8d 04 7f             	lea    eax,[edi+edi*2]
  42057d:	8d 04 80             	lea    eax,[eax+eax*4]
  420580:	8d 0c 80             	lea    ecx,[eax+eax*4]
  420583:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420588:	f7 e9                	imul   ecx
  42058a:	c1 fa 05             	sar    edx,0x5
  42058d:	8b ca                	mov    ecx,edx
  42058f:	c1 e9 1f             	shr    ecx,0x1f
  420592:	03 d1                	add    edx,ecx
  420594:	8b fa                	mov    edi,edx
  420596:	8b 86 44 0a 00 00    	mov    eax,DWORD PTR [esi+0xa44]
  42059c:	85 c0                	test   eax,eax
  42059e:	7e 1b                	jle    0x4205bb
  4205a0:	8d 04 bf             	lea    eax,[edi+edi*4]
  4205a3:	8d 0c 80             	lea    ecx,[eax+eax*4]
  4205a6:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4205ab:	d1 e1                	shl    ecx,1
  4205ad:	f7 e9                	imul   ecx
  4205af:	c1 fa 05             	sar    edx,0x5
  4205b2:	8b c2                	mov    eax,edx
  4205b4:	c1 e8 1f             	shr    eax,0x1f
  4205b7:	03 d0                	add    edx,eax
  4205b9:	8b fa                	mov    edi,edx
  4205bb:	8b 86 90 0a 00 00    	mov    eax,DWORD PTR [esi+0xa90]
  4205c1:	85 c0                	test   eax,eax
  4205c3:	7e 1e                	jle    0x4205e3
  4205c5:	8d 04 7f             	lea    eax,[edi+edi*2]
  4205c8:	8d 04 80             	lea    eax,[eax+eax*4]
  4205cb:	8d 0c 80             	lea    ecx,[eax+eax*4]
  4205ce:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4205d3:	d1 e1                	shl    ecx,1
  4205d5:	f7 e9                	imul   ecx
  4205d7:	c1 fa 05             	sar    edx,0x5
  4205da:	8b ca                	mov    ecx,edx
  4205dc:	c1 e9 1f             	shr    ecx,0x1f
  4205df:	03 d1                	add    edx,ecx
  4205e1:	8b fa                	mov    edi,edx
  4205e3:	8b 86 58 0a 00 00    	mov    eax,DWORD PTR [esi+0xa58]
  4205e9:	85 c0                	test   eax,eax
  4205eb:	7e 04                	jle    0x4205f1
  4205ed:	33 ff                	xor    edi,edi
  4205ef:	eb 0d                	jmp    0x4205fe
  4205f1:	81 ff c8 00 00 00    	cmp    edi,0xc8
  4205f7:	7e 05                	jle    0x4205fe
  4205f9:	bf c8 00 00 00       	mov    edi,0xc8
  4205fe:	53                   	push   ebx
  4205ff:	8d 9e a8 0c 00 00    	lea    ebx,[esi+0xca8]
  420605:	6a 01                	push   0x1
  420607:	57                   	push   edi
  420608:	8b cb                	mov    ecx,ebx
  42060a:	e8 81 3a 00 00       	call   0x424090
  42060f:	6a 02                	push   0x2
  420611:	57                   	push   edi
  420612:	8b cb                	mov    ecx,ebx
  420614:	e8 77 3a 00 00       	call   0x424090
  420619:	8b 86 38 0a 00 00    	mov    eax,DWORD PTR [esi+0xa38]
  42061f:	ba 64 00 00 00       	mov    edx,0x64
  420624:	85 c0                	test   eax,eax
  420626:	7e 05                	jle    0x42062d
  420628:	ba 0a 00 00 00       	mov    edx,0xa
  42062d:	8b 86 74 0a 00 00    	mov    eax,DWORD PTR [esi+0xa74]
  420633:	85 c0                	test   eax,eax
  420635:	7e 1a                	jle    0x420651
  420637:	8d 04 52             	lea    eax,[edx+edx*2]
  42063a:	8d 04 80             	lea    eax,[eax+eax*4]
  42063d:	8d 0c 80             	lea    ecx,[eax+eax*4]
  420640:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420645:	f7 e9                	imul   ecx
  420647:	c1 fa 05             	sar    edx,0x5
  42064a:	8b c2                	mov    eax,edx
  42064c:	c1 e8 1f             	shr    eax,0x1f
  42064f:	03 d0                	add    edx,eax
  420651:	6a 03                	push   0x3
  420653:	52                   	push   edx
  420654:	8b cb                	mov    ecx,ebx
  420656:	e8 35 3a 00 00       	call   0x424090
  42065b:	8b cb                	mov    ecx,ebx
  42065d:	e8 2e 38 00 00       	call   0x423e90
  420662:	8d be 34 0a 00 00    	lea    edi,[esi+0xa34]
  420668:	33 d2                	xor    edx,edx
  42066a:	8b c7                	mov    eax,edi
  42066c:	5b                   	pop    ebx
  42066d:	8b 08                	mov    ecx,DWORD PTR [eax]
  42066f:	85 c9                	test   ecx,ecx
  420671:	7e 1d                	jle    0x420690
  420673:	83 fa 10             	cmp    edx,0x10
  420676:	75 09                	jne    0x420681
  420678:	85 c9                	test   ecx,ecx
  42067a:	7e 14                	jle    0x420690
  42067c:	83 fa 13             	cmp    edx,0x13
  42067f:	74 0f                	je     0x420690
  420681:	83 c1 f6             	add    ecx,0xfffffff6
  420684:	85 c9                	test   ecx,ecx
  420686:	89 08                	mov    DWORD PTR [eax],ecx
  420688:	7f 06                	jg     0x420690
  42068a:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
  420690:	42                   	inc    edx
  420691:	83 c0 04             	add    eax,0x4
  420694:	83 fa 27             	cmp    edx,0x27
  420697:	7c d4                	jl     0x42066d
  420699:	8b 3f                	mov    edi,DWORD PTR [edi]
  42069b:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
