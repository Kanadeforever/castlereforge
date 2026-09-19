
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
  44365a:	90                   	nop
  44365b:	90                   	nop
  44365c:	90                   	nop
  44365d:	90                   	nop
  44365e:	90                   	nop
  44365f:	90                   	nop
  443660:	6a ff                	push   0xffffffff
  443662:	68 bb f9 45 00       	push   0x45f9bb
  443667:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  44366d:	50                   	push   eax
  44366e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  443675:	81 ec 94 01 00 00    	sub    esp,0x194
  44367b:	b8 70 00 00 00       	mov    eax,0x70
  443680:	8b d1                	mov    edx,ecx
  443682:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  443687:	33 c9                	xor    ecx,ecx
  443689:	57                   	push   edi
  44368a:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
  44368e:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  443694:	8b 84 24 a8 01 00 00 	mov    eax,DWORD PTR [esp+0x1a8]
  44369b:	83 f8 01             	cmp    eax,0x1
  44369e:	8d 7c 24 2c          	lea    edi,[esp+0x2c]
  4436a2:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  4436a6:	0f 85 1f 04 00 00    	jne    0x443acb
  4436ac:	53                   	push   ebx
  4436ad:	55                   	push   ebp
  4436ae:	56                   	push   esi
  4436af:	8d b2 38 7d 00 00    	lea    esi,[edx+0x7d38]
  4436b5:	bd 08 00 00 00       	mov    ebp,0x8
  4436ba:	bb c8 0b 00 00       	mov    ebx,0xbc8
  4436bf:	8b 86 48 fe ff ff    	mov    eax,DWORD PTR [esi-0x1b8]
  4436c5:	85 c0                	test   eax,eax
  4436c7:	7e 31                	jle    0x4436fa
  4436c9:	b9 1c 00 00 00       	mov    ecx,0x1c
  4436ce:	33 c0                	xor    eax,eax
  4436d0:	8b fe                	mov    edi,esi
  4436d2:	6a 01                	push   0x1
  4436d4:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4436d6:	8d be cc f5 ff ff    	lea    edi,[esi-0xa34]
  4436dc:	8b cf                	mov    ecx,edi
  4436de:	e8 cd d0 fd ff       	call   0x4207b0
  4436e3:	6a 00                	push   0x0
  4436e5:	6a 00                	push   0x0
  4436e7:	6a 73                	push   0x73
  4436e9:	8b cf                	mov    ecx,edi
  4436eb:	89 9e b0 02 00 00    	mov    DWORD PTR [esi+0x2b0],ebx
  4436f1:	e8 2a ea fd ff       	call   0x422120
  4436f6:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  4436fa:	81 c6 e4 0d 00 00    	add    esi,0xde4
  443700:	4d                   	dec    ebp
  443701:	75 bc                	jne    0x4436bf
  443703:	8b b2 31 e3 00 00    	mov    esi,DWORD PTR [edx+0xe331]
  443709:	33 db                	xor    ebx,ebx
  44370b:	85 f6                	test   esi,esi
  44370d:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  443711:	7e 34                	jle    0x443747
  443713:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  443718:	81 c2 35 e3 00 00    	add    edx,0xe335
  44371e:	8b b8 a4 00 00 00    	mov    edi,DWORD PTR [eax+0xa4]
  443724:	8b 02                	mov    eax,DWORD PTR [edx]
  443726:	83 c2 04             	add    edx,0x4
  443729:	8d 0c 40             	lea    ecx,[eax+eax*2]
  44372c:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  44372f:	d1 e1                	shl    ecx,1
  443731:	2b c8                	sub    ecx,eax
  443733:	c1 e1 04             	shl    ecx,0x4
  443736:	8b 6c 39 24          	mov    ebp,DWORD PTR [ecx+edi*1+0x24]
  44373a:	03 dd                	add    ebx,ebp
  44373c:	4e                   	dec    esi
  44373d:	75 e5                	jne    0x443724
  44373f:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  443743:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  443747:	8b 82 a9 e3 00 00    	mov    eax,DWORD PTR [edx+0xe3a9]
  44374d:	85 c0                	test   eax,eax
  44374f:	75 08                	jne    0x443759
  443751:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443755:	33 d2                	xor    edx,edx
  443757:	eb 10                	jmp    0x443769
  443759:	e8 07 de 00 00       	call   0x451565
  44375e:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443762:	99                   	cdq
  443763:	f7 be a9 e3 00 00    	idiv   DWORD PTR [esi+0xe3a9]
  443769:	8b ae a5 e3 00 00    	mov    ebp,DWORD PTR [esi+0xe3a5]
  44376f:	6a 01                	push   0x1
  443771:	03 ea                	add    ebp,edx
  443773:	8d 4c 24 28          	lea    ecx,[esp+0x28]
  443777:	89 6c 24 24          	mov    DWORD PTR [esp+0x24],ebp
  44377b:	e8 70 df ff ff       	call   0x4416f0
  443780:	8d 94 24 0c 01 00 00 	lea    edx,[esp+0x10c]
  443787:	b9 90 1c 8e 00       	mov    ecx,0x8e1c90
  44378c:	52                   	push   edx
  44378d:	68 a4 c5 46 00       	push   0x46c5a4
  443792:	c7 84 24 b4 01 00 00 	mov    DWORD PTR [esp+0x1b4],0x0
  443799:	00 00 00 00 
  44379d:	e8 4e e4 ff ff       	call   0x441bf0
  4437a2:	84 c0                	test   al,al
  4437a4:	74 36                	je     0x4437dc
  4437a6:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4437ac:	e8 ff 1e fc ff       	call   0x4056b0
  4437b1:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4437b7:	8d 84 24 0c 01 00 00 	lea    eax,[esp+0x10c]
  4437be:	6a 01                	push   0x1
  4437c0:	50                   	push   eax
  4437c1:	e8 aa 1e fc ff       	call   0x405670
  4437c6:	84 c0                	test   al,al
  4437c8:	75 12                	jne    0x4437dc
  4437ca:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4437d0:	68 88 c5 46 00       	push   0x46c588
  4437d5:	6a 00                	push   0x0
  4437d7:	e8 94 e6 fe ff       	call   0x431e70
  4437dc:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  4437e2:	e8 c9 ec fc ff       	call   0x4124b0
  4437e7:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  4437ed:	68 6c c5 46 00       	push   0x46c56c
  4437f2:	e8 c9 ec fc ff       	call   0x4124c0
  4437f7:	8b 0d b0 b2 46 00    	mov    ecx,DWORD PTR ds:0x46b2b0
  4437fd:	53                   	push   ebx
  4437fe:	8d 84 24 ac 00 00 00 	lea    eax,[esp+0xac]
  443805:	8b 51 08             	mov    edx,DWORD PTR [ecx+0x8]
  443808:	52                   	push   edx
  443809:	68 58 c5 46 00       	push   0x46c558
  44380e:	50                   	push   eax
  44380f:	e8 5f e2 00 00       	call   0x451a73
  443814:	83 c4 10             	add    esp,0x10
  443817:	8d 8c 24 a8 00 00 00 	lea    ecx,[esp+0xa8]
  44381e:	51                   	push   ecx
  44381f:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443825:	e8 96 ec fc ff       	call   0x4124c0
  44382a:	8d be 50 7b 00 00    	lea    edi,[esi+0x7b50]
  443830:	c7 44 24 18 08 00 00 	mov    DWORD PTR [esp+0x18],0x8
  443837:	00 
  443838:	8b 47 30             	mov    eax,DWORD PTR [edi+0x30]
  44383b:	85 c0                	test   eax,eax
  44383d:	0f 8e 52 01 00 00    	jle    0x443995
  443843:	8b 07                	mov    eax,DWORD PTR [edi]
  443845:	85 c0                	test   eax,eax
  443847:	0f 8e 48 01 00 00    	jle    0x443995
  44384d:	89 87 e0 fe ff ff    	mov    DWORD PTR [edi-0x120],eax
  443853:	8b 47 24             	mov    eax,DWORD PTR [edi+0x24]
  443856:	03 c3                	add    eax,ebx
  443858:	89 47 24             	mov    DWORD PTR [edi+0x24],eax
  44385b:	8b 15 b4 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b4
  443861:	8b 4f 20             	mov    ecx,DWORD PTR [edi+0x20]
  443864:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  443867:	3b 88 f4 01 00 00    	cmp    ecx,DWORD PTR [eax+0x1f4]
  44386d:	0f 8d 22 01 00 00    	jge    0x443995
  443873:	8d 54 24 38          	lea    edx,[esp+0x38]
  443877:	8b ce                	mov    ecx,esi
  443879:	52                   	push   edx
  44387a:	57                   	push   edi
  44387b:	e8 e0 02 00 00       	call   0x443b60
  443880:	84 c0                	test   al,al
  443882:	0f 84 0d 01 00 00    	je     0x443995
  443888:	33 ed                	xor    ebp,ebp
  44388a:	8d 8f b4 f7 ff ff    	lea    ecx,[edi-0x84c]
  443890:	55                   	push   ebp
  443891:	55                   	push   ebp
  443892:	6a 74                	push   0x74
  443894:	e8 87 e8 fd ff       	call   0x422120
  443899:	b8 39 8e e3 38       	mov    eax,0x38e38e39
  44389e:	89 6c 24 7c          	mov    DWORD PTR [esp+0x7c],ebp
  4438a2:	f7 2d 04 24 8e 00    	imul   DWORD PTR ds:0x8e2404
  4438a8:	d1 fa                	sar    edx,1
  4438aa:	8b c2                	mov    eax,edx
  4438ac:	c1 e8 1f             	shr    eax,0x1f
  4438af:	03 d0                	add    edx,eax
  4438b1:	85 d2                	test   edx,edx
  4438b3:	0f 8e c2 00 00 00    	jle    0x44397b
  4438b9:	8b 0d 00 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2400
  4438bf:	33 f6                	xor    esi,esi
  4438c1:	8b 07                	mov    eax,DWORD PTR [edi]
  4438c3:	83 f8 01             	cmp    eax,0x1
  4438c6:	75 10                	jne    0x4438d8
  4438c8:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4438ce:	8b 9a f8 00 00 00    	mov    ebx,DWORD PTR [edx+0xf8]
  4438d4:	85 db                	test   ebx,ebx
  4438d6:	75 7b                	jne    0x443953
  4438d8:	39 04 0e             	cmp    DWORD PTR [esi+ecx*1],eax
  4438db:	75 76                	jne    0x443953
  4438dd:	8b 47 20             	mov    eax,DWORD PTR [edi+0x20]
  4438e0:	8b 54 0e 04          	mov    edx,DWORD PTR [esi+ecx*1+0x4]
  4438e4:	3b c2                	cmp    eax,edx
  4438e6:	7c 37                	jl     0x44391f
  4438e8:	8b 57 54             	mov    edx,DWORD PTR [edi+0x54]
  4438eb:	8b 5c 0e 0c          	mov    ebx,DWORD PTR [esi+ecx*1+0xc]
  4438ef:	3b d3                	cmp    edx,ebx
  4438f1:	7c 2c                	jl     0x44391f
  4438f3:	8b 57 58             	mov    edx,DWORD PTR [edi+0x58]
  4438f6:	8b 5c 0e 10          	mov    ebx,DWORD PTR [esi+ecx*1+0x10]
  4438fa:	3b d3                	cmp    edx,ebx
  4438fc:	7c 21                	jl     0x44391f
  4438fe:	8b 57 5c             	mov    edx,DWORD PTR [edi+0x5c]
  443901:	8b 5c 0e 14          	mov    ebx,DWORD PTR [esi+ecx*1+0x14]
  443905:	3b d3                	cmp    edx,ebx
  443907:	7c 16                	jl     0x44391f
  443909:	8b 57 60             	mov    edx,DWORD PTR [edi+0x60]
  44390c:	8b 5c 0e 18          	mov    ebx,DWORD PTR [esi+ecx*1+0x18]
  443910:	3b d3                	cmp    edx,ebx
  443912:	7c 0b                	jl     0x44391f
  443914:	8b 57 64             	mov    edx,DWORD PTR [edi+0x64]
  443917:	8b 5c 0e 1c          	mov    ebx,DWORD PTR [esi+ecx*1+0x1c]
  44391b:	3b d3                	cmp    edx,ebx
  44391d:	7d 06                	jge    0x443925
  44391f:	3b 44 0e 08          	cmp    eax,DWORD PTR [esi+ecx*1+0x8]
  443923:	7c 2e                	jl     0x443953
  443925:	8b 5c 0e 20          	mov    ebx,DWORD PTR [esi+ecx*1+0x20]
  443929:	8d 8f b4 f7 ff ff    	lea    ecx,[edi-0x84c]
  44392f:	53                   	push   ebx
  443930:	e8 cb f7 fd ff       	call   0x423100
  443935:	84 c0                	test   al,al
  443937:	74 14                	je     0x44394d
  443939:	8b 44 24 7c          	mov    eax,DWORD PTR [esp+0x7c]
  44393d:	89 9c 84 80 00 00 00 	mov    DWORD PTR [esp+eax*4+0x80],ebx
  443944:	8b 44 24 7c          	mov    eax,DWORD PTR [esp+0x7c]
  443948:	40                   	inc    eax
  443949:	89 44 24 7c          	mov    DWORD PTR [esp+0x7c],eax
  44394d:	8b 0d 00 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2400
  443953:	b8 39 8e e3 38       	mov    eax,0x38e38e39
  443958:	45                   	inc    ebp
  443959:	f7 2d 04 24 8e 00    	imul   DWORD PTR ds:0x8e2404
  44395f:	d1 fa                	sar    edx,1
  443961:	8b c2                	mov    eax,edx
  443963:	83 c6 24             	add    esi,0x24
  443966:	c1 e8 1f             	shr    eax,0x1f
  443969:	03 d0                	add    edx,eax
  44396b:	3b ea                	cmp    ebp,edx
  44396d:	0f 8c 4e ff ff ff    	jl     0x4438c1
  443973:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443977:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  44397b:	8b 15 c8 fc 89 00    	mov    edx,DWORD PTR ds:0x89fcc8
  443981:	8d 4c 24 38          	lea    ecx,[esp+0x38]
  443985:	51                   	push   ecx
  443986:	8b 8a 00 06 00 00    	mov    ecx,DWORD PTR [edx+0x600]
  44398c:	e8 cf f2 fc ff       	call   0x412c60
  443991:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  443995:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  443999:	81 c7 e4 0d 00 00    	add    edi,0xde4
  44399f:	48                   	dec    eax
  4439a0:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4439a4:	0f 85 8e fe ff ff    	jne    0x443838
  4439aa:	a1 b0 b2 46 00       	mov    eax,ds:0x46b2b0
  4439af:	8b 08                	mov    ecx,DWORD PTR [eax]
  4439b1:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  4439b4:	51                   	push   ecx
  4439b5:	55                   	push   ebp
  4439b6:	52                   	push   edx
  4439b7:	8d 84 24 b4 00 00 00 	lea    eax,[esp+0xb4]
  4439be:	68 40 c5 46 00       	push   0x46c540
  4439c3:	50                   	push   eax
  4439c4:	e8 aa e0 00 00       	call   0x451a73
  4439c9:	83 c4 14             	add    esp,0x14
  4439cc:	8d 8c 24 a8 00 00 00 	lea    ecx,[esp+0xa8]
  4439d3:	51                   	push   ecx
  4439d4:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  4439da:	e8 e1 ea fc ff       	call   0x4124c0
  4439df:	55                   	push   ebp
  4439e0:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  4439e4:	8b cd                	mov    ecx,ebp
  4439e6:	e8 35 5e ff ff       	call   0x439820
  4439eb:	8b 86 ad e3 00 00    	mov    eax,DWORD PTR [esi+0xe3ad]
  4439f1:	85 c0                	test   eax,eax
  4439f3:	0f 8e b9 00 00 00    	jle    0x443ab2
  4439f9:	33 db                	xor    ebx,ebx
  4439fb:	85 c0                	test   eax,eax
  4439fd:	0f 8e af 00 00 00    	jle    0x443ab2
  443a03:	81 c6 b1 e3 00 00    	add    esi,0xe3b1
  443a09:	83 fb 04             	cmp    ebx,0x4
  443a0c:	0f 8d a0 00 00 00    	jge    0x443ab2
  443a12:	8b 46 10             	mov    eax,DWORD PTR [esi+0x10]
  443a15:	85 c0                	test   eax,eax
  443a17:	0f 8e 81 00 00 00    	jle    0x443a9e
  443a1d:	e8 43 db 00 00       	call   0x451565
  443a22:	99                   	cdq
  443a23:	b9 64 00 00 00       	mov    ecx,0x64
  443a28:	f7 f9                	idiv   ecx
  443a2a:	3b 56 10             	cmp    edx,DWORD PTR [esi+0x10]
  443a2d:	7f 6f                	jg     0x443a9e
  443a2f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  443a35:	8b 06                	mov    eax,DWORD PTR [esi]
  443a37:	3b 81 a8 00 00 00    	cmp    eax,DWORD PTR [ecx+0xa8]
  443a3d:	7d 5f                	jge    0x443a9e
  443a3f:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  443a46:	2b d0                	sub    edx,eax
  443a48:	8d 14 90             	lea    edx,[eax+edx*4]
  443a4b:	c1 e2 04             	shl    edx,0x4
  443a4e:	2b d0                	sub    edx,eax
  443a50:	8b 81 ac 00 00 00    	mov    eax,DWORD PTR [ecx+0xac]
  443a56:	83 c9 ff             	or     ecx,0xffffffff
  443a59:	8d 14 50             	lea    edx,[eax+edx*2]
  443a5c:	33 c0                	xor    eax,eax
  443a5e:	8b fa                	mov    edi,edx
  443a60:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  443a62:	f7 d1                	not    ecx
  443a64:	49                   	dec    ecx
  443a65:	74 16                	je     0x443a7d
  443a67:	52                   	push   edx
  443a68:	8d 8c 24 ac 00 00 00 	lea    ecx,[esp+0xac]
  443a6f:	68 30 c5 46 00       	push   0x46c530
  443a74:	51                   	push   ecx
  443a75:	e8 f9 df 00 00       	call   0x451a73
  443a7a:	83 c4 0c             	add    esp,0xc
  443a7d:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443a83:	8d 94 24 a8 00 00 00 	lea    edx,[esp+0xa8]
  443a8a:	52                   	push   edx
  443a8b:	e8 30 ea fc ff       	call   0x4124c0
  443a90:	8b 06                	mov    eax,DWORD PTR [esi]
  443a92:	6a 00                	push   0x0
  443a94:	6a 01                	push   0x1
  443a96:	50                   	push   eax
  443a97:	8b cd                	mov    ecx,ebp
  443a99:	e8 82 56 ff ff       	call   0x439120
  443a9e:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  443aa2:	43                   	inc    ebx
  443aa3:	83 c6 04             	add    esi,0x4
  443aa6:	3b 99 ad e3 00 00    	cmp    ebx,DWORD PTR [ecx+0xe3ad]
  443aac:	0f 8c 57 ff ff ff    	jl     0x443a09
  443ab2:	8d 4c 24 24          	lea    ecx,[esp+0x24]
  443ab6:	c7 84 24 ac 01 00 00 	mov    DWORD PTR [esp+0x1ac],0xffffffff
  443abd:	ff ff ff ff 
  443ac1:	e8 4a dc ff ff       	call   0x441710
  443ac6:	5e                   	pop    esi
  443ac7:	5d                   	pop    ebp
  443ac8:	5b                   	pop    ebx
  443ac9:	eb 6e                	jmp    0x443b39
  443acb:	83 f8 02             	cmp    eax,0x2
  443ace:	75 12                	jne    0x443ae2
  443ad0:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443ad6:	e8 d5 e9 fc ff       	call   0x4124b0
  443adb:	68 14 c5 46 00       	push   0x46c514
  443ae0:	eb 2c                	jmp    0x443b0e
  443ae2:	83 f8 03             	cmp    eax,0x3
  443ae5:	75 12                	jne    0x443af9
  443ae7:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443aed:	e8 be e9 fc ff       	call   0x4124b0
  443af2:	68 f8 c4 46 00       	push   0x46c4f8
  443af7:	eb 15                	jmp    0x443b0e
  443af9:	83 f8 04             	cmp    eax,0x4
  443afc:	75 3b                	jne    0x443b39
  443afe:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443b04:	e8 a7 e9 fc ff       	call   0x4124b0
  443b09:	68 dc c4 46 00       	push   0x46c4dc
  443b0e:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443b14:	e8 a7 e9 fc ff       	call   0x4124c0
  443b19:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443b1f:	68 bc 92 46 00       	push   0x4692bc
  443b24:	e8 97 e9 fc ff       	call   0x4124c0
  443b29:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443b2f:	68 bc 92 46 00       	push   0x4692bc
  443b34:	e8 87 e9 fc ff       	call   0x4124c0
  443b39:	8b 8c 24 98 01 00 00 	mov    ecx,DWORD PTR [esp+0x198]
  443b40:	5f                   	pop    edi
  443b41:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  443b48:	81 c4 a0 01 00 00    	add    esp,0x1a0
  443b4e:	c2 04 00             	ret    0x4
  443b51:	90                   	nop
  443b52:	90                   	nop
  443b53:	90                   	nop
  443b54:	90                   	nop
  443b55:	90                   	nop
  443b56:	90                   	nop
  443b57:	90                   	nop
  443b58:	90                   	nop
  443b59:	90                   	nop
  443b5a:	90                   	nop
  443b5b:	90                   	nop
  443b5c:	90                   	nop
  443b5d:	90                   	nop
  443b5e:	90                   	nop
  443b5f:	90                   	nop
  443b60:	53                   	push   ebx
  443b61:	55                   	push   ebp
  443b62:	56                   	push   esi
  443b63:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443b67:	8b e9                	mov    ebp,ecx
  443b69:	32 db                	xor    bl,bl
  443b6b:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  443b6e:	8b 0d f8 23 8e 00    	mov    ecx,DWORD PTR ds:0x8e23f8
  443b74:	85 f6                	test   esi,esi
  443b76:	8b 14 81             	mov    edx,DWORD PTR [ecx+eax*4]
  443b79:	89 56 28             	mov    DWORD PTR [esi+0x28],edx
  443b7c:	75 08                	jne    0x443b86
  443b7e:	5e                   	pop    esi
  443b7f:	5d                   	pop    ebp
  443b80:	32 c0                	xor    al,al
  443b82:	5b                   	pop    ebx
  443b83:	c2 08 00             	ret    0x8
  443b86:	8b 4e 30             	mov    ecx,DWORD PTR [esi+0x30]
  443b89:	85 c9                	test   ecx,ecx
  443b8b:	7f 08                	jg     0x443b95
  443b8d:	5e                   	pop    esi
  443b8e:	5d                   	pop    ebp
  443b8f:	32 c0                	xor    al,al
  443b91:	5b                   	pop    ebx
  443b92:	c2 08 00             	ret    0x8
  443b95:	8b 15 f8 23 8e 00    	mov    edx,DWORD PTR ds:0x8e23f8
  443b9b:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  443b9e:	57                   	push   edi
  443b9f:	3b 0c 82             	cmp    ecx,DWORD PTR [edx+eax*4]
  443ba2:	0f 8c 83 01 00 00    	jl     0x443d2b
  443ba8:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  443bac:	40                   	inc    eax
  443bad:	89 46 20             	mov    DWORD PTR [esi+0x20],eax
  443bb0:	8b 06                	mov    eax,DWORD PTR [esi]
  443bb2:	89 07                	mov    DWORD PTR [edi],eax
  443bb4:	e8 ac d9 00 00       	call   0x451565
  443bb9:	99                   	cdq
  443bba:	b9 03 00 00 00       	mov    ecx,0x3
  443bbf:	f7 f9                	idiv   ecx
  443bc1:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  443bc4:	b8 67 66 66 66       	mov    eax,0x66666667
  443bc9:	8b da                	mov    ebx,edx
  443bcb:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  443bce:	2b ca                	sub    ecx,edx
  443bd0:	f7 e9                	imul   ecx
  443bd2:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  443bd5:	c1 fa 02             	sar    edx,0x2
  443bd8:	8b c2                	mov    eax,edx
  443bda:	03 d3                	add    edx,ebx
  443bdc:	c1 e8 1f             	shr    eax,0x1f
  443bdf:	89 4f 14             	mov    DWORD PTR [edi+0x14],ecx
  443be2:	8b 5e 2c             	mov    ebx,DWORD PTR [esi+0x2c]
  443be5:	8d 54 10 04          	lea    edx,[eax+edx*1+0x4]
  443be9:	03 da                	add    ebx,edx
  443beb:	8b c3                	mov    eax,ebx
  443bed:	89 5e 2c             	mov    DWORD PTR [esi+0x2c],ebx
  443bf0:	89 46 30             	mov    DWORD PTR [esi+0x30],eax
  443bf3:	89 47 0c             	mov    DWORD PTR [edi+0xc],eax
  443bf6:	e8 6a d9 00 00       	call   0x451565
  443bfb:	99                   	cdq
  443bfc:	b9 03 00 00 00       	mov    ecx,0x3
  443c01:	6a 00                	push   0x0
  443c03:	f7 f9                	idiv   ecx
  443c05:	8b 4e 44             	mov    ecx,DWORD PTR [esi+0x44]
  443c08:	b8 89 88 88 88       	mov    eax,0x88888889
  443c0d:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
  443c10:	d1 e1                	shl    ecx,1
  443c12:	8b da                	mov    ebx,edx
  443c14:	f7 e9                	imul   ecx
  443c16:	03 d1                	add    edx,ecx
  443c18:	c1 fa 03             	sar    edx,0x3
  443c1b:	8b c2                	mov    eax,edx
  443c1d:	c1 e8 1f             	shr    eax,0x1f
  443c20:	03 d0                	add    edx,eax
  443c22:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  443c25:	2b d0                	sub    edx,eax
  443c27:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  443c2c:	f7 ea                	imul   edx
  443c2e:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  443c31:	c1 fa 03             	sar    edx,0x3
  443c34:	8b ca                	mov    ecx,edx
  443c36:	03 d3                	add    edx,ebx
  443c38:	c1 e9 1f             	shr    ecx,0x1f
  443c3b:	89 47 18             	mov    DWORD PTR [edi+0x18],eax
  443c3e:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  443c41:	8d 54 11 03          	lea    edx,[ecx+edx*1+0x3]
  443c45:	03 c2                	add    eax,edx
  443c47:	89 46 34             	mov    DWORD PTR [esi+0x34],eax
  443c4a:	89 46 38             	mov    DWORD PTR [esi+0x38],eax
  443c4d:	89 47 10             	mov    DWORD PTR [edi+0x10],eax
  443c50:	8b 0e                	mov    ecx,DWORD PTR [esi]
  443c52:	51                   	push   ecx
  443c53:	8b cd                	mov    ecx,ebp
  443c55:	e8 e6 00 00 00       	call   0x443d40
  443c5a:	8b 56 3c             	mov    edx,DWORD PTR [esi+0x3c]
  443c5d:	6a 01                	push   0x1
  443c5f:	89 57 30             	mov    DWORD PTR [edi+0x30],edx
  443c62:	8b 4e 3c             	mov    ecx,DWORD PTR [esi+0x3c]
  443c65:	03 c8                	add    ecx,eax
  443c67:	8b c1                	mov    eax,ecx
  443c69:	89 4e 3c             	mov    DWORD PTR [esi+0x3c],ecx
  443c6c:	89 47 1c             	mov    DWORD PTR [edi+0x1c],eax
  443c6f:	8b 06                	mov    eax,DWORD PTR [esi]
  443c71:	50                   	push   eax
  443c72:	8b cd                	mov    ecx,ebp
  443c74:	e8 c7 00 00 00       	call   0x443d40
  443c79:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  443c7c:	89 4f 34             	mov    DWORD PTR [edi+0x34],ecx
  443c7f:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  443c82:	03 c8                	add    ecx,eax
  443c84:	89 4e 40             	mov    DWORD PTR [esi+0x40],ecx
  443c87:	8b c1                	mov    eax,ecx
  443c89:	6a 02                	push   0x2
  443c8b:	89 47 20             	mov    DWORD PTR [edi+0x20],eax
  443c8e:	8b 16                	mov    edx,DWORD PTR [esi]
  443c90:	52                   	push   edx
  443c91:	8b cd                	mov    ecx,ebp
  443c93:	e8 a8 00 00 00       	call   0x443d40
  443c98:	8b 4e 44             	mov    ecx,DWORD PTR [esi+0x44]
  443c9b:	6a 03                	push   0x3
  443c9d:	89 4f 38             	mov    DWORD PTR [edi+0x38],ecx
  443ca0:	8b 4e 44             	mov    ecx,DWORD PTR [esi+0x44]
  443ca3:	03 c8                	add    ecx,eax
  443ca5:	8b c1                	mov    eax,ecx
  443ca7:	89 4e 44             	mov    DWORD PTR [esi+0x44],ecx
  443caa:	89 47 24             	mov    DWORD PTR [edi+0x24],eax
  443cad:	8b 16                	mov    edx,DWORD PTR [esi]
  443caf:	52                   	push   edx
  443cb0:	8b cd                	mov    ecx,ebp
  443cb2:	e8 89 00 00 00       	call   0x443d40
  443cb7:	8b 4e 48             	mov    ecx,DWORD PTR [esi+0x48]
  443cba:	6a 04                	push   0x4
  443cbc:	89 4f 3c             	mov    DWORD PTR [edi+0x3c],ecx
  443cbf:	8b 56 48             	mov    edx,DWORD PTR [esi+0x48]
  443cc2:	03 d0                	add    edx,eax
  443cc4:	8b cd                	mov    ecx,ebp
  443cc6:	8b c2                	mov    eax,edx
  443cc8:	89 56 48             	mov    DWORD PTR [esi+0x48],edx
  443ccb:	89 47 28             	mov    DWORD PTR [edi+0x28],eax
  443cce:	8b 16                	mov    edx,DWORD PTR [esi]
  443cd0:	52                   	push   edx
  443cd1:	e8 6a 00 00 00       	call   0x443d40
  443cd6:	8b 4e 4c             	mov    ecx,DWORD PTR [esi+0x4c]
  443cd9:	89 4f 40             	mov    DWORD PTR [edi+0x40],ecx
  443cdc:	8b 5e 4c             	mov    ebx,DWORD PTR [esi+0x4c]
  443cdf:	03 d8                	add    ebx,eax
  443ce1:	8b c3                	mov    eax,ebx
  443ce3:	89 5e 4c             	mov    DWORD PTR [esi+0x4c],ebx
  443ce6:	89 47 2c             	mov    DWORD PTR [edi+0x2c],eax
  443ce9:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  443cec:	a1 f8 23 8e 00       	mov    eax,ds:0x8e23f8
  443cf1:	b3 01                	mov    bl,0x1
  443cf3:	8b 0c 90             	mov    ecx,DWORD PTR [eax+edx*4]
  443cf6:	8b 56 50             	mov    edx,DWORD PTR [esi+0x50]
  443cf9:	89 4e 28             	mov    DWORD PTR [esi+0x28],ecx
  443cfc:	8b c1                	mov    eax,ecx
  443cfe:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  443d01:	83 c2 03             	add    edx,0x3
  443d04:	2b c1                	sub    eax,ecx
  443d06:	89 56 50             	mov    DWORD PTR [esi+0x50],edx
  443d09:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  443d0c:	79 07                	jns    0x443d15
  443d0e:	c7 47 08 00 00 00 00 	mov    DWORD PTR [edi+0x8],0x0
  443d15:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  443d18:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  443d1b:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  443d20:	8b 88 f0 05 00 00    	mov    ecx,DWORD PTR [eax+0x5f0]
  443d26:	e8 75 a4 fd ff       	call   0x41e1a0
  443d2b:	5f                   	pop    edi
  443d2c:	5e                   	pop    esi
  443d2d:	8a c3                	mov    al,bl
  443d2f:	5d                   	pop    ebp
  443d30:	5b                   	pop    ebx
  443d31:	c2 08 00             	ret    0x8
  443d34:	90                   	nop
  443d35:	90                   	nop
  443d36:	90                   	nop
  443d37:	90                   	nop
  443d38:	90                   	nop
  443d39:	90                   	nop
  443d3a:	90                   	nop
  443d3b:	90                   	nop
  443d3c:	90                   	nop
  443d3d:	90                   	nop
  443d3e:	90                   	nop
  443d3f:	90                   	nop
  443d40:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  443d44:	85 c0                	test   eax,eax
  443d46:	7e 40                	jle    0x443d88
  443d48:	83 f8 06             	cmp    eax,0x6
  443d4b:	7f 3b                	jg     0x443d88
  443d4d:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  443d51:	85 c9                	test   ecx,ecx
  443d53:	7c 33                	jl     0x443d88
  443d55:	83 f9 04             	cmp    ecx,0x4
  443d58:	7f 2e                	jg     0x443d88
  443d5a:	8d 0c 81             	lea    ecx,[ecx+eax*4]
  443d5d:	56                   	push   esi
  443d5e:	03 c1                	add    eax,ecx
  443d60:	57                   	push   edi
  443d61:	8b 04 85 c4 c1 46 00 	mov    eax,DWORD PTR [eax*4+0x46c1c4]
  443d68:	8b 34 85 c4 c1 46 00 	mov    esi,DWORD PTR [eax*4+0x46c1c4]
  443d6f:	8b 3c 85 b0 c1 46 00 	mov    edi,DWORD PTR [eax*4+0x46c1b0]
  443d76:	46                   	inc    esi
  443d77:	e8 e9 d7 00 00       	call   0x451565
  443d7c:	99                   	cdq
  443d7d:	f7 fe                	idiv   esi
  443d7f:	8b c2                	mov    eax,edx
  443d81:	03 c7                	add    eax,edi
  443d83:	5f                   	pop    edi
  443d84:	5e                   	pop    esi
  443d85:	c2 08 00             	ret    0x8
  443d88:	33 c0                	xor    eax,eax
  443d8a:	c2 08 00             	ret    0x8
  443d8d:	90                   	nop
  443d8e:	90                   	nop
  443d8f:	90                   	nop
  443d90:	83 ec 1c             	sub    esp,0x1c
  443d93:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  443d97:	53                   	push   ebx
  443d98:	33 db                	xor    ebx,ebx
  443d9a:	57                   	push   edi
  443d9b:	3d 61 01 00 00       	cmp    eax,0x161
  443da0:	8b f9                	mov    edi,ecx
  443da2:	89 5c 24 08          	mov    DWORD PTR [esp+0x8],ebx
  443da6:	74 32                	je     0x443dda
  443da8:	3d 69 01 00 00       	cmp    eax,0x169
  443dad:	0f 85 09 02 00 00    	jne    0x443fbc
  443db3:	e8 ad d7 00 00       	call   0x451565
  443db8:	25 03 00 00 80       	and    eax,0x80000003
  443dbd:	79 05                	jns    0x443dc4
  443dbf:	48                   	dec    eax
  443dc0:	83 c8 fc             	or     eax,0xfffffffc
  443dc3:	40                   	inc    eax
  443dc4:	83 f8 03             	cmp    eax,0x3
  443dc7:	77 59                	ja     0x443e22
  443dc9:	ff 24 85 c4 3f 44 00 	jmp    DWORD PTR [eax*4+0x443fc4]
  443dd0:	c7 44 24 08 91 00 00 	mov    DWORD PTR [esp+0x8],0x91
  443dd7:	00 
  443dd8:	eb 48                	jmp    0x443e22
  443dda:	e8 86 d7 00 00       	call   0x451565
  443ddf:	25 01 00 00 80       	and    eax,0x80000001
  443de4:	79 05                	jns    0x443deb
  443de6:	48                   	dec    eax
  443de7:	83 c8 fe             	or     eax,0xfffffffe
  443dea:	40                   	inc    eax
  443deb:	2b c3                	sub    eax,ebx
  443ded:	74 0d                	je     0x443dfc
  443def:	48                   	dec    eax
  443df0:	75 30                	jne    0x443e22
  443df2:	c7 44 24 08 7e 00 00 	mov    DWORD PTR [esp+0x8],0x7e
  443df9:	00 
  443dfa:	eb 26                	jmp    0x443e22
  443dfc:	c7 44 24 08 7d 00 00 	mov    DWORD PTR [esp+0x8],0x7d
  443e03:	00 
  443e04:	eb 1c                	jmp    0x443e22
  443e06:	c7 44 24 08 92 00 00 	mov    DWORD PTR [esp+0x8],0x92
  443e0d:	00 
  443e0e:	eb 12                	jmp    0x443e22
  443e10:	c7 44 24 08 93 00 00 	mov    DWORD PTR [esp+0x8],0x93
  443e17:	00 
  443e18:	eb 08                	jmp    0x443e22
  443e1a:	c7 44 24 08 94 00 00 	mov    DWORD PTR [esp+0x8],0x94
  443e21:	00 
  443e22:	55                   	push   ebp
  443e23:	56                   	push   esi
  443e24:	e8 3c d7 00 00       	call   0x451565
  443e29:	25 01 00 00 80       	and    eax,0x80000001
  443e2e:	79 05                	jns    0x443e35
  443e30:	48                   	dec    eax
  443e31:	83 c8 fe             	or     eax,0xfffffffe
  443e34:	40                   	inc    eax
  443e35:	33 c9                	xor    ecx,ecx
  443e37:	33 d2                	xor    edx,edx
  443e39:	40                   	inc    eax
  443e3a:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  443e3e:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
  443e42:	89 44 24 34          	mov    DWORD PTR [esp+0x34],eax
  443e46:	89 4c 24 28          	mov    DWORD PTR [esp+0x28],ecx
  443e4a:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
  443e4e:	8d b7 60 0c 00 00    	lea    esi,[edi+0xc60]
  443e54:	83 cd ff             	or     ebp,0xffffffff
  443e57:	39 ae b0 fe ff ff    	cmp    DWORD PTR [esi-0x150],ebp
  443e5d:	74 63                	je     0x443ec2
  443e5f:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  443e62:	7e 5e                	jle    0x443ec2
  443e64:	8b 8e 20 03 00 00    	mov    ecx,DWORD PTR [esi+0x320]
  443e6a:	85 c9                	test   ecx,ecx
  443e6c:	7c 59                	jl     0x443ec7
  443e6e:	83 f9 08             	cmp    ecx,0x8
  443e71:	7d 54                	jge    0x443ec7
  443e73:	8b 96 54 02 00 00    	mov    edx,DWORD PTR [esi+0x254]
  443e79:	c6 44 0c 1c 01       	mov    BYTE PTR [esp+ecx*1+0x1c],0x1
  443e7e:	4a                   	dec    edx
  443e7f:	74 33                	je     0x443eb4
  443e81:	4a                   	dec    edx
  443e82:	74 22                	je     0x443ea6
  443e84:	4a                   	dec    edx
  443e85:	75 40                	jne    0x443ec7
  443e87:	8d 51 ff             	lea    edx,[ecx-0x1]
  443e8a:	85 d2                	test   edx,edx
  443e8c:	7c 39                	jl     0x443ec7
  443e8e:	8d 51 fc             	lea    edx,[ecx-0x4]
  443e91:	85 d2                	test   edx,edx
  443e93:	7c 32                	jl     0x443ec7
  443e95:	c6 44 0c 1b 01       	mov    BYTE PTR [esp+ecx*1+0x1b],0x1
  443e9a:	c6 44 0c 18 01       	mov    BYTE PTR [esp+ecx*1+0x18],0x1
  443e9f:	c6 44 0c 17 01       	mov    BYTE PTR [esp+ecx*1+0x17],0x1
  443ea4:	eb 21                	jmp    0x443ec7
  443ea6:	8d 51 fc             	lea    edx,[ecx-0x4]
  443ea9:	85 d2                	test   edx,edx
  443eab:	7c 1a                	jl     0x443ec7
  443ead:	c6 44 0c 18 01       	mov    BYTE PTR [esp+ecx*1+0x18],0x1
  443eb2:	eb 13                	jmp    0x443ec7
  443eb4:	8d 51 ff             	lea    edx,[ecx-0x1]
  443eb7:	85 d2                	test   edx,edx
  443eb9:	7c 0c                	jl     0x443ec7
  443ebb:	c6 44 0c 1b 01       	mov    BYTE PTR [esp+ecx*1+0x1b],0x1
  443ec0:	eb 05                	jmp    0x443ec7
  443ec2:	c6 44 1c 24 01       	mov    BYTE PTR [esp+ebx*1+0x24],0x1
  443ec7:	43                   	inc    ebx
  443ec8:	81 c6 e4 0d 00 00    	add    esi,0xde4
  443ece:	83 fb 08             	cmp    ebx,0x8
  443ed1:	7c 84                	jl     0x443e57
  443ed3:	33 c9                	xor    ecx,ecx
  443ed5:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  443ed9:	8a 54 0c 1c          	mov    dl,BYTE PTR [esp+ecx*1+0x1c]
  443edd:	84 d2                	test   dl,dl
  443edf:	0f 85 c7 00 00 00    	jne    0x443fac
  443ee5:	85 c0                	test   eax,eax
  443ee7:	0f 8e bf 00 00 00    	jle    0x443fac
  443eed:	33 f6                	xor    esi,esi
  443eef:	8a 5c 34 24          	mov    bl,BYTE PTR [esp+esi*1+0x24]
  443ef3:	8d 54 34 24          	lea    edx,[esp+esi*1+0x24]
  443ef7:	84 db                	test   bl,bl
  443ef9:	89 54 24 18          	mov    DWORD PTR [esp+0x18],edx
  443efd:	0f 84 9f 00 00 00    	je     0x443fa2
  443f03:	8d 04 f6             	lea    eax,[esi+esi*8]
  443f06:	8d 04 86             	lea    eax,[esi+eax*4]
  443f09:	8d 14 40             	lea    edx,[eax+eax*2]
  443f0c:	8d 04 d6             	lea    eax,[esi+edx*8]
  443f0f:	8d 97 c4 02 00 00    	lea    edx,[edi+0x2c4]
  443f15:	52                   	push   edx
  443f16:	8b 97 4c 02 00 00    	mov    edx,DWORD PTR [edi+0x24c]
  443f1c:	8d 2c 87             	lea    ebp,[edi+eax*4]
  443f1f:	8d 87 54 02 00 00    	lea    eax,[edi+0x254]
  443f25:	50                   	push   eax
  443f26:	8b 04 b2             	mov    eax,DWORD PTR [edx+esi*4]
  443f29:	50                   	push   eax
  443f2a:	56                   	push   esi
  443f2b:	51                   	push   ecx
  443f2c:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  443f30:	8d 9d e4 03 00 00    	lea    ebx,[ebp+0x3e4]
  443f36:	51                   	push   ecx
  443f37:	8b cb                	mov    ecx,ebx
  443f39:	e8 72 b3 fd ff       	call   0x41f2b0
  443f3e:	8b cb                	mov    ecx,ebx
  443f40:	e8 8b f2 fd ff       	call   0x4231d0
  443f45:	6a 00                	push   0x0
  443f47:	6a 00                	push   0x0
  443f49:	6a 67                	push   0x67
  443f4b:	8b cb                	mov    ecx,ebx
  443f4d:	e8 ce e1 fd ff       	call   0x422120
  443f52:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  443f56:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  443f5a:	89 94 b7 a4 03 00 00 	mov    DWORD PTR [edi+esi*4+0x3a4],edx
  443f61:	89 84 b7 3c e5 00 00 	mov    DWORD PTR [edi+esi*4+0xe53c],eax
  443f68:	8b 8d 5c 0c 00 00    	mov    ecx,DWORD PTR [ebp+0xc5c]
  443f6e:	c6 44 04 1c 01       	mov    BYTE PTR [esp+eax*1+0x1c],0x1
  443f73:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  443f77:	89 8d 60 0c 00 00    	mov    DWORD PTR [ebp+0xc60],ecx
  443f7d:	8b 95 88 0f 00 00    	mov    edx,DWORD PTR [ebp+0xf88]
  443f83:	48                   	dec    eax
  443f84:	89 44 24 34          	mov    DWORD PTR [esp+0x34],eax
  443f88:	be 08 00 00 00       	mov    esi,0x8
  443f8d:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  443f90:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  443f94:	c6 81 58 03 00 00 01 	mov    BYTE PTR [ecx+0x358],0x1
  443f9b:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  443f9f:	c6 02 00             	mov    BYTE PTR [edx],0x0
  443fa2:	46                   	inc    esi
  443fa3:	83 fe 08             	cmp    esi,0x8
  443fa6:	0f 8c 43 ff ff ff    	jl     0x443eef
  443fac:	41                   	inc    ecx
  443fad:	83 f9 08             	cmp    ecx,0x8
  443fb0:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  443fb4:	0f 8c 1f ff ff ff    	jl     0x443ed9
  443fba:	5e                   	pop    esi
  443fbb:	5d                   	pop    ebp
  443fbc:	5f                   	pop    edi
  443fbd:	5b                   	pop    ebx
  443fbe:	83 c4 1c             	add    esp,0x1c
  443fc1:	c2 08 00             	ret    0x8
  443fc4:	d0 3d 44 00 06 3e    	sar    BYTE PTR ds:0x3e060044,1
  443fca:	44                   	inc    esp
  443fcb:	00 10                	add    BYTE PTR [eax],dl
  443fcd:	3e 44                	ds inc esp
  443fcf:	00 1a                	add    BYTE PTR [edx],bl
  443fd1:	3e 44                	ds inc esp
  443fd3:	00 90 90 90 90 90    	add    BYTE PTR [eax-0x6f6f6f70],dl
  443fd9:	90                   	nop
  443fda:	90                   	nop
  443fdb:	90                   	nop
  443fdc:	90                   	nop
  443fdd:	90                   	nop
  443fde:	90                   	nop
  443fdf:	90                   	nop
  443fe0:	8a 44 24 04          	mov    al,BYTE PTR [esp+0x4]
  443fe4:	53                   	push   ebx
  443fe5:	56                   	push   esi
  443fe6:	57                   	push   edi
  443fe7:	84 c0                	test   al,al
  443fe9:	8b f9                	mov    edi,ecx
  443feb:	74 67                	je     0x444054
  443fed:	8d b7 a0 7e 00 00    	lea    esi,[edi+0x7ea0]
  443ff3:	bb 08 00 00 00       	mov    ebx,0x8
  443ff8:	8b 86 34 ff ff ff    	mov    eax,DWORD PTR [esi-0xcc]
  443ffe:	83 f8 03             	cmp    eax,0x3
  444001:	74 25                	je     0x444028
  444003:	83 f8 04             	cmp    eax,0x4
  444006:	74 20                	je     0x444028
  444008:	8b 06                	mov    eax,DWORD PTR [esi]
  44400a:	83 f8 14             	cmp    eax,0x14
  44400d:	7c 0a                	jl     0x444019
  44400f:	83 f8 17             	cmp    eax,0x17
  444012:	7f 05                	jg     0x444019
  444014:	83 c0 04             	add    eax,0x4
  444017:	eb 0d                	jmp    0x444026
  444019:	83 f8 18             	cmp    eax,0x18
  44401c:	7c 0a                	jl     0x444028
  44401e:	83 f8 1b             	cmp    eax,0x1b
  444021:	7f 05                	jg     0x444028
  444023:	83 c0 fc             	add    eax,0xfffffffc
  444026:	89 06                	mov    DWORD PTR [esi],eax
  444028:	8b 06                	mov    eax,DWORD PTR [esi]
  44402a:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  44402d:	8b 8c 87 c4 02 00 00 	mov    ecx,DWORD PTR [edi+eax*4+0x2c4]
  444034:	8b 94 87 54 02 00 00 	mov    edx,DWORD PTR [edi+eax*4+0x254]
  44403b:	51                   	push   ecx
  44403c:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  44403f:	52                   	push   edx
  444040:	e8 0b 92 ff ff       	call   0x43d250
  444045:	81 c6 e4 0d 00 00    	add    esi,0xde4
  44404b:	4b                   	dec    ebx
  44404c:	75 aa                	jne    0x443ff8
  44404e:	5f                   	pop    edi
  44404f:	5e                   	pop    esi
  444050:	5b                   	pop    ebx
  444051:	c2 04 00             	ret    0x4
  444054:	8d b7 80 0f 00 00    	lea    esi,[edi+0xf80]
  44405a:	bb 08 00 00 00       	mov    ebx,0x8
  44405f:	8b                   	.byte 0x8b
