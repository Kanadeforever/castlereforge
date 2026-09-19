
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004014e0 <.text+0x4e0>:
  4014e0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4014e4:	53                   	push   ebx
  4014e5:	56                   	push   esi
  4014e6:	8b f1                	mov    esi,ecx
  4014e8:	57                   	push   edi
  4014e9:	89 06                	mov    DWORD PTR [esi],eax
  4014eb:	a3 84 f3 46 00       	mov    ds:0x46f384,eax
  4014f0:	e8 4b fd ff ff       	call   0x401240
  4014f5:	e8 16 15 00 00       	call   0x402a10
  4014fa:	8d 7e 04             	lea    edi,[esi+0x4]
  4014fd:	6a 02                	push   0x2
  4014ff:	57                   	push   edi
  401500:	68 90 80 46 00       	push   0x468090
  401505:	e8 a6 10 00 00       	call   0x4025b0
  40150a:	8b 0f                	mov    ecx,DWORD PTR [edi]
  40150c:	83 c4 0c             	add    esp,0xc
  40150f:	8b d9                	mov    ebx,ecx
  401511:	8d 51 11             	lea    edx,[ecx+0x11]
  401514:	89 56 08             	mov    DWORD PTR [esi+0x8],edx
  401517:	42                   	inc    edx
  401518:	2b da                	sub    ebx,edx
  40151a:	89 56 0c             	mov    DWORD PTR [esi+0xc],edx
  40151d:	03 d8                	add    ebx,eax
  40151f:	b8 4f ec c4 4e       	mov    eax,0x4ec4ec4f
  401524:	f7 e3                	mul    ebx
  401526:	c1 ea 04             	shr    edx,0x4
  401529:	89 56 10             	mov    DWORD PTR [esi+0x10],edx
  40152c:	8a 19                	mov    bl,BYTE PTR [ecx]
  40152e:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401534:	84 db                	test   bl,bl
  401536:	0f 97 c0             	seta   al
  401539:	88 41 30             	mov    BYTE PTR [ecx+0x30],al
  40153c:	8b 17                	mov    edx,DWORD PTR [edi]
  40153e:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401544:	8b 42 01             	mov    eax,DWORD PTR [edx+0x1]
  401547:	89 41 6c             	mov    DWORD PTR [ecx+0x6c],eax
  40154a:	8b 17                	mov    edx,DWORD PTR [edi]
  40154c:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401552:	8b 42 05             	mov    eax,DWORD PTR [edx+0x5]
  401555:	89 41 70             	mov    DWORD PTR [ecx+0x70],eax
  401558:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  40155d:	8b 17                	mov    edx,DWORD PTR [edi]
  40155f:	8b 48 28             	mov    ecx,DWORD PTR [eax+0x28]
  401562:	8b 52 09             	mov    edx,DWORD PTR [edx+0x9]
  401565:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  401568:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  40156e:	8b 07                	mov    eax,DWORD PTR [edi]
  401570:	8b 51 28             	mov    edx,DWORD PTR [ecx+0x28]
  401573:	8b 40 0d             	mov    eax,DWORD PTR [eax+0xd]
  401576:	89 42 08             	mov    DWORD PTR [edx+0x8],eax
  401579:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  40157f:	e8 dc 43 00 00       	call   0x405960
  401584:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  401587:	8b 06                	mov    eax,DWORD PTR [esi]
  401589:	33 d2                	xor    edx,edx
  40158b:	8a 11                	mov    dl,BYTE PTR [ecx]
  40158d:	8b 0d b4 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6b4
  401593:	52                   	push   edx
  401594:	50                   	push   eax
  401595:	e8 a6 37 00 00       	call   0x404d40
  40159a:	8b 0d 00 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f600
  4015a0:	68 70 80 46 00       	push   0x468070
  4015a5:	6a 18                	push   0x18
  4015a7:	6a 18                	push   0x18
  4015a9:	e8 12 1c 00 00       	call   0x4031c0
  4015ae:	8b 0d c4 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c4
  4015b4:	68 70 80 46 00       	push   0x468070
  4015b9:	e8 62 22 03 00       	call   0x433820
  4015be:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4015c0:	51                   	push   ecx
  4015c1:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4015c7:	e8 a4 37 00 00       	call   0x404d70
  4015cc:	8b 0d f4 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e24f4
  4015d2:	a3 bc f6 46 00       	mov    ds:0x46f6bc,eax
  4015d7:	e8 44 8d 04 00       	call   0x44a320
  4015dc:	5f                   	pop    edi
  4015dd:	5e                   	pop    esi
  4015de:	5b                   	pop    ebx
  4015df:	c2 04 00             	ret    0x4
  4015e2:	90                   	nop
  4015e3:	90                   	nop
  4015e4:	90                   	nop
  4015e5:	90                   	nop
  4015e6:	90                   	nop
  4015e7:	90                   	nop
  4015e8:	90                   	nop
  4015e9:	90                   	nop
  4015ea:	90                   	nop
  4015eb:	90                   	nop
  4015ec:	90                   	nop
  4015ed:	90                   	nop
  4015ee:	90                   	nop
  4015ef:	90                   	nop
  4015f0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4015f4:	8b 0d f4 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e24f4
  4015fa:	50                   	push   eax
  4015fb:	e8 d0 8e 04 00       	call   0x44a4d0
  401600:	c2 04 00             	ret    0x4
  401603:	90                   	nop
  401604:	90                   	nop
  401605:	90                   	nop
  401606:	90                   	nop
  401607:	90                   	nop
  401608:	90                   	nop
  401609:	90                   	nop
  40160a:	90                   	nop
  40160b:	90                   	nop
  40160c:	90                   	nop
  40160d:	90                   	nop
  40160e:	90                   	nop
  40160f:	90                   	nop
  401610:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401616:	e8 d5 4b 00 00       	call   0x4061f0
  40161b:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  401620:	8a 48 30             	mov    cl,BYTE PTR [eax+0x30]
  401623:	84 c9                	test   cl,cl
  401625:	0f 94 c1             	sete   cl
  401628:	88 48 30             	mov    BYTE PTR [eax+0x30],cl
  40162b:	8b 15 c0 f6 89     	mov    edx,DWORD PTR ds:0x89f6c0



/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401980 <.text+0x980>:
  401980:	8b 7d 10             	mov    edi,DWORD PTR [ebp+0x10]
  401983:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  401986:	c6 45 ef 00          	mov    BYTE PTR [ebp-0x11],0x0
  40198a:	3d 05 01 00 00       	cmp    eax,0x105
  40198f:	0f 87 93 01 00 00    	ja     0x401b28
  401995:	0f 84 0c 01 00 00    	je     0x401aa7
  40199b:	83 f8 05             	cmp    eax,0x5
  40199e:	0f 87 b3 00 00 00    	ja     0x401a57
  4019a4:	0f 84 83 00 00 00    	je     0x401a2d
  4019aa:	8b c8                	mov    ecx,eax
  4019ac:	49                   	dec    ecx
  4019ad:	74 38                	je     0x4019e7
  4019af:	49                   	dec    ecx
  4019b0:	0f 85 a3 01 00 00    	jne    0x401b59
  4019b6:	e8 05 98 00 00       	call   0x40b1c0
  4019bb:	8b 35 80 f3 46 00    	mov    esi,DWORD PTR ds:0x46f380
  4019c1:	85 f6                	test   esi,esi
  4019c3:	74 10                	je     0x4019d5
  4019c5:	8b ce                	mov    ecx,esi
  4019c7:	e8 64 f8 ff ff       	call   0x401230
  4019cc:	56                   	push   esi
  4019cd:	e8 7e fb 04 00       	call   0x451550
  4019d2:	83 c4 04             	add    esp,0x4
  4019d5:	e8 f6 f7 ff ff       	call   0x4011d0
  4019da:	6a 00                	push   0x0
  4019dc:	ff 15 dc 01 46 00    	call   DWORD PTR ds:0x4601dc
  4019e2:	e9 9f 01 00 00       	jmp    0x401b86
  4019e7:	6a 14                	push   0x14
  4019e9:	e8 71 fc 04 00       	call   0x45165f
  4019ee:	8b c8                	mov    ecx,eax
  4019f0:	83 c4 04             	add    esp,0x4
  4019f3:	89 4d 10             	mov    DWORD PTR [ebp+0x10],ecx
  4019f6:	85 c9                	test   ecx,ecx
  4019f8:	c6 45 fc 01          	mov    BYTE PTR [ebp-0x4],0x1
  4019fc:	74 09                	je     0x401a07
  4019fe:	e8 1d f8 ff ff       	call   0x401220
  401a03:	8b c8                	mov    ecx,eax
  401a05:	eb 02                	jmp    0x401a09
  401a07:	33 c9                	xor    ecx,ecx
  401a09:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  401a0c:	c6 45 fc 00          	mov    BYTE PTR [ebp-0x4],0x0
  401a10:	50                   	push   eax
  401a11:	89 0d 80 f3 46 00    	mov    DWORD PTR ds:0x46f380,ecx
  401a17:	e8 c4 fa ff ff       	call   0x4014e0
  401a1c:	c6 05 8c f3 46 00 01 	mov    BYTE PTR ds:0x46f38c,0x1
  401a23:	e8 d8 f5 ff ff       	call   0x401000
  401a28:	e9 59 01 00 00       	jmp    0x401b86
  401a2d:	83 ff 01             	cmp    edi,0x1
  401a30:	75 19                	jne    0x401a4b
  401a32:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  401a35:	57                   	push   edi
  401a36:	6a 00                	push   0x0
  401a38:	51                   	push   ecx
  401a39:	ff 15 e4 01 46 00    	call   DWORD PTR ds:0x4601e4
  401a3f:	c6 05 bc 80 46 00 00 	mov    BYTE PTR ds:0x4680bc,0x0
  401a46:	e9 3b 01 00 00       	jmp    0x401b86
  401a4b:	c6 05 bc 80 46 00 01 	mov    BYTE PTR ds:0x4680bc,0x1
  401a52:	e9 2f 01 00 00       	jmp    0x401b86
  401a57:	8b c8                	mov    ecx,eax
  401a59:	81 e9 00 01 00 00    	sub    ecx,0x100
  401a5f:	74 0c                	je     0x401a6d
  401a61:	49                   	dec    ecx
  401a62:	0f 84 1e 01 00 00    	je     0x401b86
  401a68:	e9 ec 00 00 00       	jmp    0x401b59
  401a6d:	83 ff 7b             	cmp    edi,0x7b
  401a70:	74 11                	je     0x401a83
  401a72:	8b 0d 80 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f380
  401a78:	57                   	push   edi
  401a79:	e8 72 fb ff ff       	call   0x4015f0
  401a7e:	e9 03 01 00 00       	jmp    0x401b86
  401a83:	8b 0d f4 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e24f4
  401a89:	e8 92 2d 03 00       	call   0x434820
  401a8e:	e8 0d 99 00 00       	call   0x40b3a0
  401a93:	8d 55 ef             	lea    edx,[ebp-0x11]
  401a96:	8b c8                	mov    ecx,eax
  401a98:	52                   	push   edx
  401a99:	e8 62 2e 03 00       	call   0x434900
  401a9e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  401aa1:	50                   	push   eax
  401aa2:	e9 d9 00 00 00       	jmp    0x401b80
  401aa7:	83 ff 09             	cmp    edi,0x9
  401aaa:	0f 84 d6 00 00 00    	je     0x401b86
  401ab0:	83 ff 0d             	cmp    edi,0xd
  401ab3:	0f 85 cd 00 00 00    	jne    0x401b86
  401ab9:	8b 0d 80 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f380
  401abf:	e8 4c fb ff ff       	call   0x401610
  401ac4:	e9 bd 00 00 00       	jmp    0x401b86
  401ac9:	68 00 01 00 00       	push   0x100
  401ace:	e8 8c          	call   0x45165f



/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004025b0 <.text+0x15b0>:
  4025b0:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  4025b4:	81 ec 84 01 00 00    	sub    esp,0x184
  4025ba:	83 f8 03             	cmp    eax,0x3
  4025bd:	56                   	push   esi
  4025be:	57                   	push   edi
  4025bf:	0f 87 fc 00 00 00    	ja     0x4026c1
  4025c5:	ff 24 85 d0 27 40 00 	jmp    DWORD PTR [eax*4+0x4027d0]
  4025cc:	8b 84 24 94 01 00 00 	mov    eax,DWORD PTR [esp+0x194]
  4025d3:	8b b4 24 90 01 00 00 	mov    esi,DWORD PTR [esp+0x190]
  4025da:	50                   	push   eax
  4025db:	56                   	push   esi
  4025dc:	e8 ff 01 00 00       	call   0x4027e0
  4025e1:	83 c4 08             	add    esp,0x8
  4025e4:	83 f8 ff             	cmp    eax,0xffffffff
  4025e7:	0f 85 d4 00 00 00    	jne    0x4026c1
  4025ed:	56                   	push   esi
  4025ee:	8d 4c 24 48          	lea    ecx,[esp+0x48]
  4025f2:	68 50 83 46 00       	push   0x468350
  4025f7:	51                   	push   ecx
  4025f8:	e8 76 f4 04 00       	call   0x451a73
  4025fd:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  402603:	83 c4 0c             	add    esp,0xc
  402606:	8d 54 24 44          	lea    edx,[esp+0x44]
  40260a:	52                   	push   edx
  40260b:	e8 60 f9 02 00       	call   0x431f70
  402610:	8d 44 24 0c          	lea    eax,[esp+0xc]
  402614:	68 d8 37 46 00       	push   0x4637d8
  402619:	50                   	push   eax
  40261a:	c7 44 24 14 fc f5 46 	mov    DWORD PTR [esp+0x14],0x46f5fc
  402621:	00 
  402622:	e8 a0 f5 04 00       	call   0x451bc7
  402627:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  40262b:	8d 54 24 24          	lea    edx,[esp+0x24]
  40262f:	51                   	push   ecx
  402630:	8d 84 24 48 01 00 00 	lea    eax,[esp+0x148]
  402637:	52                   	push   edx
  402638:	8b 94 24 98 01 00 00 	mov    edx,DWORD PTR [esp+0x198]
  40263f:	8d 4c 24 24          	lea    ecx,[esp+0x24]
  402643:	50                   	push   eax
  402644:	51                   	push   ecx
  402645:	52                   	push   edx
  402646:	e8 b6 f5 04 00       	call   0x451c01
  40264b:	a1 dc f5 46 00       	mov    eax,ds:0x46f5dc
  402650:	83 c4 14             	add    esp,0x14
  402653:	83 f8 02             	cmp    eax,0x2
  402656:	7c 69                	jl     0x4026c1
  402658:	83 f8 03             	cmp    eax,0x3
  40265b:	7f 64                	jg     0x4026c1
  40265d:	8d 44 24 14          	lea    eax,[esp+0x14]
  402661:	8d 4c 24 24          	lea    ecx,[esp+0x24]
  402665:	50                   	push   eax
  402666:	8d 94 24 48 01 00 00 	lea    edx,[esp+0x148]
  40266d:	51                   	push   ecx
  40266e:	52                   	push   edx
  40266f:	68 34 83 46 00       	push   0x468334
  402674:	68 c8 f3 46 00       	push   0x46f3c8
  402679:	e8 f5 f3 04 00       	call   0x451a73
  40267e:	8b b4 24 a8 01 00 00 	mov    esi,DWORD PTR [esp+0x1a8]
  402685:	56                   	push   esi
  402686:	68 c8 f3 46 00       	push   0x46f3c8
  40268b:	e8 50 01 00 00       	call   0x4027e0
  402690:	83 c4 1c             	add    esp,0x1c
  402693:	83 f8 ff             	cmp    eax,0xffffffff
  402696:	75 29                	jne    0x4026c1
  402698:	8d 44 24 14          	lea    eax,[esp+0x14]
  40269c:	8d 4c 24 24          	lea    ecx,[esp+0x24]
  4026a0:	50                   	push   eax
  4026a1:	51                   	push   ecx
  4026a2:	68 ac 81 46 00       	push   0x4681ac
  4026a7:	68 c8 f3 46 00       	push   0x46f3c8
  4026ac:	e8 c2 f3 04 00       	call   0x451a73
  4026b1:	6a 01                	push   0x1
  4026b3:	56                   	push   esi
  4026b4:	68 c8 f3 46 00       	push   0x46f3c8
  4026b9:	e8 92 01 00 00       	call   0x402850
  4026be:	83 c4 1c             	add    esp,0x1c
  4026c1:	a1 cc f5 46 00       	mov    eax,ds:0x46f5cc
  4026c6:	5f                   	pop    edi
  4026c7:	5e                   	pop    esi
  4026c8:	81 c4 84 01 00 00    	add    esp,0x184
  4026ce:	c3                   	ret
  4026cf:	8b b4 24 90 01 00 00 	mov    esi,DWORD PTR [esp+0x190]
  4026d6:	8d 54 24 44          	lea    edx,[esp+0x44]
  4026da:	56                   	push   esi
  4026db:	68 1c 83 46 00       	push   0x46831c
  4026e0:	52                   	push   edx
  4026e1:	e8 8d f3 04 00       	call   0x451a73
  4026e6:	8b bc 24 a0 01 00 00 	mov    edi,DWORD PTR [esp+0x1a0]
  4026ed:	8d 44 24 50          	lea    eax,[esp+0x50]
  4026f1:	57                   	push   edi
  4026f2:	50                   	push   eax
  4026f3:	e8 e8 00 00 00       	call   0x4027e0
  4026f8:	83 c4 14             	add    esp,0x14
  4026fb:	83 f8 ff             	cmp    eax,0xffffffff
  4026fe:	75 c1                	jne    0x4026c1
  402700:	6a 02                	push   0x2
  402702:	57                   	push   edi
  402703:	56                   	push   esi
  402704:	e8 47 01 00 00       	call   0x402850
  402709:	83 c4 0c             	add    esp,0xc
  40270c:	83 f8 ff             	cmp    eax,0xffffffff
  40270f:	75 b0                	jne    0x4026c1
  402711:	56                   	push   esi
  402712:	8d 4c 24 48          	lea    ecx,[esp+0x48]
  402716:	68 00 83 46 00       	push   0x468300
  40271b:	51                   	push   ecx
  40271c:	e8 52 f3 04 00       	call   0x451a73
  402721:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  402727:	83 c4 0c             	add    esp,0xc
  40272a:	8d 54 24 44          	lea    edx,[esp+0x44]
  40272e:	52                   	push   edx
  40272f:	e8 3c f8 02 00       	call   0x431f70
  402734:	8d 44 24 08          	lea    eax,[esp+0x8]
  402738:	68 d8 37 46 00       	push   0x4637d8
  40273d:	50                   	push   eax
  40273e:	c7 44 24 10 fc f5 46 	mov    DWORD PTR [esp+0x10],0x46f5fc
  402745:	00 
  402746:	e8 7c f4 04 00       	call   0x451bc7
  40274b:	8b b4 24 90 01 00 00 	mov    esi,DWORD PTR [esp+0x190]
  402752:	8d 4c 24 44          	lea    ecx,[esp+0x44]
  402756:	56                   	push   esi
  402757:	68 e8 82 46 00       	push   0x4682e8
  40275c:	51                   	push   ecx
  40275d:	e8 11 f3 04 00       	call   0x451a73
  402762:	8b bc 24 a0 01 00 00 	mov    edi,DWORD PTR [esp+0x1a0]
  402769:	8d 54 24 50          	lea    edx,[esp+0x50]
  40276d:	57                   	push   edi
  40276e:	52                   	push   edx
  40276f:	e8 6c 00 00 00       	call   0x4027e0
  402774:	83 c4 14             	add    esp,0x14
  402777:	83 f8 ff             	cmp    eax,0xffffffff
  40277a:	0f 85 41 ff ff ff    	jne    0x4026c1
  402780:	6a 03                	push   0x3
  402782:	57                   	push   edi
  402783:	56                   	push   esi
  402784:	e8 c7 00 00 00       	call   0x402850
  402789:	83 c4 0c             	add    esp,0xc
  40278c:	83 f8 ff             	cmp    eax,0xffffffff
  40278f:	0f 85 2c ff ff ff    	jne    0x4026c1
  402795:	56                   	push   esi
  402796:	8d 44 24 48          	lea    eax,[esp+0x48]
  40279a:	68 c8 82 46 00       	push   0x4682c8
  40279f:	50                   	push   eax
  4027a0:	e8 ce f2 04 00       	call   0x451a73
  4027a5:	83 c4 0c             	add    esp,0xc
  4027a8:	8d 4c 24 44          	lea    ecx,[esp+0x44]
  4027ac:	51                   	push   ecx
  4027ad:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4027b3:	e8 b8 f7 02 00       	call   0x431f70
  4027b8:	8d 54 24 10          	lea    edx,[esp+0x10]
  4027bc:	68 d8 37 46 00       	push   0x4637d8
  4027c1:	52                   	push   edx
  4027c2:	c7 44 24 18 fc f5 46 	mov    DWORD PTR [esp+0x18],0x46f5fc
  4027c9:	00 
  4027ca:	e8 f8 f3 04 00       	call   0x451bc7
  4027cf:	90                   	nop
  4027d0:	cc                   	int3
  4027d1:	25 40 00 27 26       	and    eax,0x26270040
  4027d6:	40                   	inc    eax
  4027d7:	00 cf                	add    bh,cl
  4027d9:	26 40                	es inc eax
  4027db:	00 4b 27             	add    BYTE PTR [ebx+0x27],cl
  4027de:	40                   	inc    eax
  4027df:	00 56 8b             	add    BYTE PTR [esi-0x75],dl
  4027e2:	74 24                	je     0x402808
  4027e4:	08 6a 00             	or     BYTE PTR [edx+0x0],ch
  4027e7:	56                   	push   esi
  4027e8:	e8 f1 a4 05 00       	call   0x45ccde
  4027ed:	83 c4 08             	add    esp,0x8
  4027f0:	85 c0                	test   eax,eax
  4027f2:	74 05                	je     0x4027f9
  4027f4:	83 c8 ff             	or     eax,0xffffffff
  4027f7:	5e                   	pop    esi
  4027f8:	c3                   	ret
  4027f9:	57                   	push   edi
  4027fa:	68 51 01 00 00       	push   0x151
  4027ff:	68 c8 81 46 00       	push   0x4681c8
  402804:	68 00 80 00 00       	push   0x8000
  402809:	56                   	push   esi
  40280a:	e8 e1 8d 04 00       	call   0x44b5f0
  40280f:	8b f0                	mov    esi,eax
  402811:	56                   	push   esi
  402812:	e8 4f a4 05 00       	call   0x45cc66
  402817:	8b f8                	mov    edi,eax
  402819:	57                   	push   edi
  40281a:	e8 40 ee 04 00       	call   0x45165f
  40281f:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  402823:	57                   	push   edi
  402824:	50                   	push   eax
  402825:	56                   	push   esi
  402826:	89 01                	mov    DWORD PTR [ecx],eax
  402828:	e8 a3 97 05 00       	call   0x45bfd0
  40282d:	68 55 01 00 00       	push   0x155
  402832:	68 c8 81 46 00       	push   0x4681c8
  402837:	56                   	push   esi
  402838:	a3 cc f5 46 00       	mov    ds:0x46f5cc,eax
  40283d:	e8 3e 8e 04 00       	call   0x44b680
  402842:	83 c4 30             	add    esp,0x30
  402845:	8b c7                	mov    eax,edi
  402847:	5f                   	pop    edi
  402848:	5e                   	pop    esi
  402849:	c3                   	ret
  40284a:	90                   	nop
  40284b:	90                   	nop
  40284c:	90                   	nop
  40284d:	90                   	nop
  40284e:	90                   	nop
  40284f:	90                   	nop
  402850:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  402854:	56                   	push   esi
  402855:	48                   	dec    eax
  402856:	0f 84 e6 00 00 00    	je     0x402942
  40285c:	48                   	dec    eax
  40285d:	74 78                	je     0x4028d7
  40285f:	48                   	dec    eax
  402860:	0f 85 46 01 00 00    	jne    0x4029ac
  402866:	a1 e4 f5 46 00       	mov    eax,ds:0x46f5e4
  40286b:	8b 0d f0 f5 46 00    	mov    ecx,DWORD PTR ds:0x46f5f0
  402871:	50                   	push   eax
  402872:	8d 54 24 14          	lea    edx,[esp+0x14]
  402876:	51                   	push   ecx
  402877:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  40287b:	8d 44 24 10          	lea    eax,[esp+0x10]
  40287f:	52                   	push   edx
  402880:	50                   	push   eax
  402881:	51                   	push   ecx
  402882:	e8 39 01 00 00       	call   0x4029c0
  402887:	83 c4 14             	add    esp,0x14
  40288a:	84 c0                	test   al,al
  40288c:	0f 84 1a 01 00 00    	je     0x4029ac
  402892:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  402896:	52                   	push   edx
  402897:	e8 c3 ed 04 00       	call   0x45165f
  40289c:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  4028a0:	6a 00                	push   0x0
  4028a2:	89 06                	mov    DWORD PTR [esi],eax
  4028a4:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4028a8:	8b 0d d4 f5 46 00    	mov    ecx,DWORD PTR ds:0x46f5d4
  4028ae:	50                   	push   eax
  4028af:	51                   	push   ecx
  4028b0:	e8 17 60 05 00       	call   0x4588cc
  4028b5:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  4028b9:	8b 0d d4 f5 46 00    	mov    ecx,DWORD PTR ds:0x46f5d4
  4028bf:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  4028c3:	8b 06                	mov    eax,DWORD PTR [esi]
  4028c5:	52                   	push   edx
  4028c6:	50                   	push   eax
  4028c7:	51                   	push   ecx
  4028c8:	e8 03 97 05 00       	call   0x45bfd0
  4028cd:	83 c4 1c             	add    esp,0x1c
  4028d0:	a3 cc f5 46 00       	mov    ds:0x46f5cc,eax
  4028d5:	5e                   	pop    esi
  4028d6:	c3                   	ret
  4028d7:	8b 15 e0 f5 46 00    	mov    edx,DWORD PTR ds:0x46f5e0
  4028dd:	a1 ec f5 46 00       	mov    eax,ds:0x46f5ec
  4028e2:	52                   	push   edx
  4028e3:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  4028e7:	50                   	push   eax
  4028e8:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  4028ec:	8d 54 24 10          	lea    edx,[esp+0x10]
  4028f0:	51                   	push   ecx
  4028f1:	52                   	push   edx
  4028f2:	50                   	push   eax
  4028f3:	e8 c8 00 00 00       	call   0x4029c0
  4028f8:	83 c4 14             	add    esp,0x14
  4028fb:	84 c0                	test   al,al
  4028fd:	0f 84 a9 00 00 00    	je     0x4029ac
  402903:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  402907:	51                   	push   ecx
  402908:	e8 52 ed 04 00       	call   0x45165f
  40290d:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  402911:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  402915:	6a 00                	push   0x0
  402917:	52                   	push   edx
  402918:	89 06                	mov    DWORD PTR [esi],eax
  40291a:	a1 d0 f5 46 00       	mov    eax,ds:0x46f5d0
  40291f:	50                   	push   eax
  402920:	e8 a7 5f 05 00       	call   0x4588cc
  402925:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  402929:	8b 16                	mov    edx,DWORD PTR [esi]
  40292b:	a1 d0 f5 46 00       	mov    eax,ds:0x46f5d0
  402930:	51                   	push   ecx
  402931:	52                   	push   edx
  402932:	50                   	push   eax
  402933:	e8 98 96 05 00       	call   0x45bfd0
  402938:	83 c4 1c             	add    esp,0x1c
  40293b:	a3 cc f5 46 00       	mov    ds:0x46f5cc,eax
  402940:	5e                   	pop    esi
  402941:	c3                   	ret
  402942:	8b 0d e8 f5 46 00    	mov    ecx,DWORD PTR ds:0x46f5e8
  402948:	8b 15 f4 f5 46 00    	mov    edx,DWORD PTR ds:0x46f5f4
  40294e:	51                   	push   ecx
  40294f:	8d 44 24 14          	lea    eax,[esp+0x14]
  402953:	52                   	push   edx
  402954:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  402958:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  40295c:	50                   	push   eax
  40295d:	51                   	push   ecx
  40295e:	52                   	push   edx
  40295f:	e8 5c 00 00 00       	call   0x4029c0
  402964:	83 c4 14             	add    esp,0x14
  402967:	84 c0                	test   al,al
  402969:	74 41                	je     0x4029ac
  40296b:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  40296f:	50                   	push   eax
  402970:	e8 ea ec 04 00       	call   0x45165f
  402975:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  402979:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  40297d:	6a 00                	push   0x0
  40297f:	51                   	push   ecx
  402980:	89 06                	mov    DWORD PTR [esi],eax
  402982:	8b 15 d8 f5 46 00    	mov    edx,DWORD PTR ds:0x46f5d8
  402988:	52                   	push   edx
  402989:	e8 3e 5f 05 00       	call   0x4588cc
  40298e:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  402992:	8b 0e                	mov    ecx,DWORD PTR [esi]
  402994:	8b 15 d8 f5 46 00    	mov    edx,DWORD PTR ds:0x46f5d8
  40299a:	50                   	push   eax
  40299b:	51                   	push   ecx
  40299c:	52                   	push   edx
  40299d:	e8 2e 96 05 00       	call   0x45bfd0
  4029a2:	83 c4 1c             	add    esp,0x1c
  4029a5:	a3 cc f5 46 00       	mov    ds:0x46f5cc,eax
  4029aa:	5e                   	pop    esi
  4029ab:	c3                   	ret
  4029ac:	83 c8 ff             	or     eax,0xffffffff
  4029af:	5e                   	pop    esi
  4029b0:	c3                   	ret
