; Oracle: RPG.exe SHA256 b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; objdump -d -Mintel, VA 0x42d4d0..0x42d700

  42d4d0:	83 ec 08             	sub    esp,0x8
  42d4d3:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42d4d7:	53                   	push   ebx
  42d4d8:	55                   	push   ebp
  42d4d9:	56                   	push   esi
  42d4da:	57                   	push   edi
  42d4db:	8b f1                	mov    esi,ecx
  42d4dd:	50                   	push   eax
  42d4de:	c6 44 24 16 00       	mov    BYTE PTR [esp+0x16],0x0
  42d4e3:	c7 44 24 18 ff ff ff 	mov    DWORD PTR [esp+0x18],0xffffffff
  42d4ea:	ff 
  42d4eb:	32 db                	xor    bl,bl
  42d4ed:	e8 be 03 00 00       	call   0x42d8b0
  42d4f2:	88 44 24 13          	mov    BYTE PTR [esp+0x13],al
  42d4f6:	33 ed                	xor    ebp,ebp
  42d4f8:	8d be cc 07 00 00    	lea    edi,[esi+0x7cc]
  42d4fe:	8a 4c 24 12          	mov    cl,BYTE PTR [esp+0x12]
  42d502:	84 c9                	test   cl,cl
  42d504:	0f 85 b1 00 00 00    	jne    0x42d5bb
  42d50a:	8a 8c 2e 5c 09 00 00 	mov    cl,BYTE PTR [esi+ebp*1+0x95c]
  42d511:	84 c9                	test   cl,cl
  42d513:	0f 85 89 00 00 00    	jne    0x42d5a2
  42d519:	84 c0                	test   al,al
  42d51b:	c6 44 24 12 01       	mov    BYTE PTR [esp+0x12],0x1
  42d520:	89 6c 24 14          	mov    DWORD PTR [esp+0x14],ebp
  42d524:	c6 84 2e 5c 09 00 00 	mov    BYTE PTR [esi+ebp*1+0x95c],0x1
  42d52b:	01 
  42d52c:	74 42                	je     0x42d570
  42d52e:	8b 8e 4c 02 00 00    	mov    ecx,DWORD PTR [esi+0x24c]
  42d534:	8b 14 a9             	mov    edx,DWORD PTR [ecx+ebp*4]
  42d537:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  42d53a:	e8 51 10 01 00       	call   0x43e590
  42d53f:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42d543:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  42d547:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  42d54b:	50                   	push   eax
  42d54c:	8b 86 4c 02 00 00    	mov    eax,DWORD PTR [esi+0x24c]
  42d552:	51                   	push   ecx
  42d553:	52                   	push   edx
  42d554:	8b 0c a8             	mov    ecx,DWORD PTR [eax+ebp*4]
  42d557:	e8 64 fc 00 00       	call   0x43d1c0
  42d55c:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  42d560:	8b 54 24 2c          	mov    edx,DWORD PTR [esp+0x2c]
  42d564:	89 8f 70 fe ff ff    	mov    DWORD PTR [edi-0x190],ecx
  42d56a:	8a d8                	mov    bl,al
  42d56c:	89 17                	mov    DWORD PTR [edi],edx
  42d56e:	eb 1f                	jmp    0x42d58f
  42d570:	8b 44 24 2c          	mov    eax,DWORD PTR [esp+0x2c]
  42d574:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  42d578:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  42d57c:	50                   	push   eax
  42d57d:	8b 86 4c 02 00 00    	mov    eax,DWORD PTR [esi+0x24c]
  42d583:	51                   	push   ecx
  42d584:	52                   	push   edx
  42d585:	8b 0c a8             	mov    ecx,DWORD PTR [eax+ebp*4]
  42d588:	e8 33 fc 00 00       	call   0x43d1c0
  42d58d:	8a d8                	mov    bl,al
  42d58f:	8b 4c 24 30          	mov    ecx,DWORD PTR [esp+0x30]
  42d593:	6a 01                	push   0x1
  42d595:	51                   	push   ecx
  42d596:	55                   	push   ebp
  42d597:	8b ce                	mov    ecx,esi
  42d599:	e8 42 01 01 00       	call   0x43d6e0
  42d59e:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  42d5a2:	45                   	inc    ebp
  42d5a3:	83 c7 04             	add    edi,0x4
  42d5a6:	83 fd 64             	cmp    ebp,0x64
  42d5a9:	0f 8c 4f ff ff ff    	jl     0x42d4fe
  42d5af:	8a 4c 24 12          	mov    cl,BYTE PTR [esp+0x12]
  42d5b3:	84 c9                	test   cl,cl
  42d5b5:	0f 84 8a 00 00 00    	je     0x42d645
  42d5bb:	84 db                	test   bl,bl
  42d5bd:	0f 84 82 00 00 00    	je     0x42d645
  42d5c3:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  42d5c7:	33 c9                	xor    ecx,ecx
  42d5c9:	88 84 32 54 02 00 00 	mov    BYTE PTR [edx+esi*1+0x254],al
  42d5d0:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42d5d4:	89 84 96 c0 09 00 00 	mov    DWORD PTR [esi+edx*4+0x9c0],eax
  42d5db:	33 c0                	xor    eax,eax
  42d5dd:	8a 9c 06 5c 09 00 00 	mov    bl,BYTE PTR [esi+eax*1+0x95c]
  42d5e4:	84 db                	test   bl,bl
  42d5e6:	74 01                	je     0x42d5e9
  42d5e8:	41                   	inc    ecx
  42d5e9:	40                   	inc    eax
  42d5ea:	83 f8 64             	cmp    eax,0x64
  42d5ed:	7c ee                	jl     0x42d5dd
  42d5ef:	3b 0d e0 01 8b 00    	cmp    ecx,DWORD PTR ds:0x8b01e0
  42d5f5:	7e 06                	jle    0x42d5fd
  42d5f7:	89 0d e0 01 8b 00    	mov    DWORD PTR ds:0x8b01e0,ecx
  42d5fd:	8a 4c 24 38          	mov    cl,BYTE PTR [esp+0x38]
  42d601:	8d 04 52             	lea    eax,[edx+edx*2]
  42d604:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  42d608:	88 8c 32 b8 02 00 00 	mov    BYTE PTR [edx+esi*1+0x2b8],cl
  42d60f:	8d 04 80             	lea    eax,[eax+eax*4]
  42d612:	83 c9 ff             	or     ecx,0xffffffff
  42d615:	8d 04 80             	lea    eax,[eax+eax*4]
  42d618:	8d 9c 86 70 0e 00 00 	lea    ebx,[esi+eax*4+0xe70]
  42d61f:	33 c0                	xor    eax,eax
  42d621:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  42d623:	f7 d1                	not    ecx
  42d625:	2b f9                	sub    edi,ecx
  42d627:	8b c1                	mov    eax,ecx
  42d629:	8b f7                	mov    esi,edi
  42d62b:	8b fb                	mov    edi,ebx
  42d62d:	c1 e9 02             	shr    ecx,0x2
  42d630:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  42d632:	8b c8                	mov    ecx,eax
  42d634:	8b c2                	mov    eax,edx
  42d636:	83 e1 03             	and    ecx,0x3
  42d639:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  42d63b:	5f                   	pop    edi
  42d63c:	5e                   	pop    esi
  42d63d:	5d                   	pop    ebp
  42d63e:	5b                   	pop    ebx
  42d63f:	83 c4 08             	add    esp,0x8
  42d642:	c2 20 00             	ret    0x20
  42d645:	8b ce                	mov    ecx,esi
  42d647:	e8 14 02 00 00       	call   0x42d860
  42d64c:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42d652:	68 60 a9 46 00       	push   0x46a960
  42d657:	e8 14 49 00 00       	call   0x431f70
  42d65c:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  42d660:	51                   	push   ecx
  42d661:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42d667:	68 2c a9 46 00       	push   0x46a92c
  42d66c:	e8 df 49 00 00       	call   0x432050
  42d671:	5f                   	pop    edi
  42d672:	5e                   	pop    esi
  42d673:	5d                   	pop    ebp
  42d674:	83 c8 ff             	or     eax,0xffffffff
  42d677:	5b                   	pop    ebx
  42d678:	83 c4 08             	add    esp,0x8
  42d67b:	c2 20 00             	ret    0x20
  42d67e:	90                   	nop
  42d67f:	90                   	nop
  42d680:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  42d684:	32 c0                	xor    al,al
  42d686:	85 d2                	test   edx,edx
  42d688:	53                   	push   ebx
  42d689:	7c 47                	jl     0x42d6d2
  42d68b:	83 fa 64             	cmp    edx,0x64
  42d68e:	7d 42                	jge    0x42d6d2
  42d690:	8a 9c 0a 5c 09 00 00 	mov    bl,BYTE PTR [edx+ecx*1+0x95c]
  42d697:	84 db                	test   bl,bl
  42d699:	74 37                	je     0x42d6d2
  42d69b:	8a 9c 0a 54 02 00 00 	mov    bl,BYTE PTR [edx+ecx*1+0x254]
  42d6a2:	8b 89 4c 02 00 00    	mov    ecx,DWORD PTR [ecx+0x24c]
  42d6a8:	84 db                	test   bl,bl
  42d6aa:	8b 14 91             	mov    edx,DWORD PTR [ecx+edx*4]
  42d6ad:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  42d6b0:	74 0d                	je     0x42d6bf
  42d6b2:	8b 51 54             	mov    edx,DWORD PTR [ecx+0x54]
  42d6b5:	85 d2                	test   edx,edx
  42d6b7:	74 1b                	je     0x42d6d4
  42d6b9:	b0 01                	mov    al,0x1
  42d6bb:	5b                   	pop    ebx
  42d6bc:	c2 04 00             	ret    0x4
  42d6bf:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  42d6c2:	83 f9 02             	cmp    ecx,0x2
  42d6c5:	74 0d                	je     0x42d6d4
  42d6c7:	83 f9 07             	cmp    ecx,0x7
  42d6ca:	74 08                	je     0x42d6d4
  42d6cc:	b0 01                	mov    al,0x1
  42d6ce:	5b                   	pop    ebx
  42d6cf:	c2 04 00             	ret    0x4
  42d6d2:	32 c0                	xor    al,al
  42d6d4:	5b                   	pop    ebx
  42d6d5:	c2 04 00             	ret    0x4
  42d6d8:	90                   	nop
  42d6d9:	90                   	nop
  42d6da:	90                   	nop
  42d6db:	90                   	nop
  42d6dc:	90                   	nop
  42d6dd:	90                   	nop
  42d6de:	90                   	nop
  42d6df:	90                   	nop
  42d6e0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  42d6e4:	53                   	push   ebx
  42d6e5:	33 db                	xor    ebx,ebx
  42d6e7:	3b c3                	cmp    eax,ebx
  42d6e9:	0f 8c c7 00 00 00    	jl     0x42d7b6
  42d6ef:	83 f8 64             	cmp    eax,0x64
  42d6f2:	0f 8d be 00 00 00    	jge    0x42d7b6
  42d6f8:	38 9c 08 5c 09 00 00 	cmp    BYTE PTR [eax+ecx*1+0x95c],bl
  42d6ff:	0f 84 b1 00 00 00    	je     0x42d7b6
