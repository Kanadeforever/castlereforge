
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040f6d0 <.text+0xe6d0>:
  40f6d0:	6a ff                	push   0xffffffff
  40f6d2:	68 c0 d2 45 00       	push   0x45d2c0
  40f6d7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40f6dd:	50                   	push   eax
  40f6de:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40f6e5:	83 ec 20             	sub    esp,0x20
  40f6e8:	53                   	push   ebx
  40f6e9:	56                   	push   esi
  40f6ea:	8b f1                	mov    esi,ecx
  40f6ec:	57                   	push   edi
  40f6ed:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  40f6f1:	e8 1a 18 02 00       	call   0x430f10
  40f6f6:	33 db                	xor    ebx,ebx
  40f6f8:	8b ce                	mov    ecx,esi
  40f6fa:	53                   	push   ebx
  40f6fb:	68 48 85 1e 00       	push   0x1e8548
  40f700:	56                   	push   esi
  40f701:	89 5c 24 40          	mov    DWORD PTR [esp+0x40],ebx
  40f705:	c7 06 d0 06 46 00    	mov    DWORD PTR [esi],0x4606d0
  40f70b:	e8 20 4d 02 00       	call   0x434430
  40f710:	8d 8e 18 02 00 00    	lea    ecx,[esi+0x218]
  40f716:	e8 15 56 ff ff       	call   0x404d30
  40f71b:	68 6c 92 46 00       	push   0x46926c
  40f720:	8d 8e 6c 05 00 00    	lea    ecx,[esi+0x56c]
  40f726:	53                   	push   ebx
  40f727:	89 44 24 30          	mov    DWORD PTR [esp+0x30],eax
  40f72b:	53                   	push   ebx
  40f72c:	8d 86 68 05 00 00    	lea    eax,[esi+0x568]
  40f732:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  40f736:	53                   	push   ebx
  40f737:	53                   	push   ebx
  40f738:	8b ce                	mov    ecx,esi
  40f73a:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  40f73e:	e8 9d 19 02 00       	call   0x4310e0
  40f743:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  40f749:	53                   	push   ebx
  40f74a:	e8 a1 ef 02 00       	call   0x43e6f0
  40f74f:	68 98 05 00 00       	push   0x598
  40f754:	e8 06 1f 04 00       	call   0x45165f
  40f759:	83 c4 04             	add    esp,0x4
  40f75c:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  40f760:	3b c3                	cmp    eax,ebx
  40f762:	c6 44 24 34 01       	mov    BYTE PTR [esp+0x34],0x1
  40f767:	74 09                	je     0x40f772
  40f769:	8b c8                	mov    ecx,eax
  40f76b:	e8 a0 13 00 00       	call   0x410b10
  40f770:	eb 02                	jmp    0x40f774
  40f772:	33 c0                	xor    eax,eax
  40f774:	68 84 05 00 00       	push   0x584
  40f779:	88 5c 24 38          	mov    BYTE PTR [esp+0x38],bl
  40f77d:	89 86 88 05 00 00    	mov    DWORD PTR [esi+0x588],eax
  40f783:	e8 d7 1e 04 00       	call   0x45165f
  40f788:	83 c4 04             	add    esp,0x4
  40f78b:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  40f78f:	3b c3                	cmp    eax,ebx
  40f791:	c6 44 24 34 02       	mov    BYTE PTR [esp+0x34],0x2
  40f796:	74 09                	je     0x40f7a1
  40f798:	8b c8                	mov    ecx,eax
  40f79a:	e8 31 15 00 00       	call   0x410cd0
  40f79f:	eb 02                	jmp    0x40f7a3
  40f7a1:	33 c0                	xor    eax,eax
  40f7a3:	68 80 05 00 00       	push   0x580
  40f7a8:	88 5c 24 38          	mov    BYTE PTR [esp+0x38],bl
  40f7ac:	89 86 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],eax
  40f7b2:	e8 a8 1e 04 00       	call   0x45165f
  40f7b7:	83 c4 04             	add    esp,0x4
  40f7ba:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  40f7be:	3b c3                	cmp    eax,ebx
  40f7c0:	c6 44 24 34 03       	mov    BYTE PTR [esp+0x34],0x3
  40f7c5:	74 09                	je     0x40f7d0
  40f7c7:	8b c8                	mov    ecx,eax
  40f7c9:	e8 c2 16 00 00       	call   0x410e90
  40f7ce:	eb 02                	jmp    0x40f7d2
  40f7d0:	33 c0                	xor    eax,eax
  40f7d2:	68 f4 05 00 00       	push   0x5f4
  40f7d7:	88 5c 24 38          	mov    BYTE PTR [esp+0x38],bl
  40f7db:	89 86 90 05 00 00    	mov    DWORD PTR [esi+0x590],eax
  40f7e1:	e8 79 1e 04 00       	call   0x45165f
  40f7e6:	83 c4 04             	add    esp,0x4
  40f7e9:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  40f7ed:	3b c3                	cmp    eax,ebx
  40f7ef:	c6 44 24 34 04       	mov    BYTE PTR [esp+0x34],0x4
  40f7f4:	74 09                	je     0x40f7ff
  40f7f6:	8b c8                	mov    ecx,eax
  40f7f8:	e8 23 18 00 00       	call   0x411020
  40f7fd:	eb 02                	jmp    0x40f801
  40f7ff:	33 c0                	xor    eax,eax
  40f801:	68 40 06 00 00       	push   0x640
  40f806:	88 5c 24 38          	mov    BYTE PTR [esp+0x38],bl
  40f80a:	89 86 94 05 00 00    	mov    DWORD PTR [esi+0x594],eax
  40f810:	e8 4a 1e 04 00       	call   0x45165f
  40f815:	83 c4 04             	add    esp,0x4
  40f818:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  40f81c:	3b c3                	cmp    eax,ebx
  40f81e:	c6 44 24 34 05       	mov    BYTE PTR [esp+0x34],0x5
  40f823:	74 09                	je     0x40f82e
  40f825:	8b c8                	mov    ecx,eax
  40f827:	e8 c4 1b 00 00       	call   0x4113f0
  40f82c:	eb 02                	jmp    0x40f830
  40f82e:	33 c0                	xor    eax,eax
  40f830:	89 86 98 05 00 00    	mov    DWORD PTR [esi+0x598],eax
  40f836:	6a 2c                	push   0x2c
  40f838:	88 5c 24 38          	mov    BYTE PTR [esp+0x38],bl
  40f83c:	88 98 79 05 00 00    	mov    BYTE PTR [eax+0x579],bl
  40f842:	e8 18 1e 04 00       	call   0x45165f
  40f847:	83 c4 04             	add    esp,0x4
  40f84a:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  40f84e:	3b c3                	cmp    eax,ebx
  40f850:	c6 44 24 34 06       	mov    BYTE PTR [esp+0x34],0x6
  40f855:	74 0e                	je     0x40f865
  40f857:	8d 54 24 14          	lea    edx,[esp+0x14]
  40f85b:	8b c8                	mov    ecx,eax
  40f85d:	52                   	push   edx
  40f85e:	e8 ed 22 02 00       	call   0x431b50
  40f863:	eb 02                	jmp    0x40f867
  40f865:	33 c0                	xor    eax,eax
  40f867:	53                   	push   ebx
  40f868:	68 58 92 46 00       	push   0x469258
  40f86d:	53                   	push   ebx
  40f86e:	53                   	push   ebx
  40f86f:	8b c8                	mov    ecx,eax
  40f871:	88 5c 24 44          	mov    BYTE PTR [esp+0x44],bl
  40f875:	89 86 84 05 00 00    	mov    DWORD PTR [esi+0x584],eax
  40f87b:	e8 90 23 02 00       	call   0x431c10
  40f880:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  40f886:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  40f889:	e8 42 ee 02 00       	call   0x43e6d0
  40f88e:	68 b0 05 00 00       	push   0x5b0
  40f893:	e8 c7 1d 04 00       	call   0x45165f
  40f898:	83 c4 04             	add    esp,0x4
  40f89b:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  40f89f:	3b c3                	cmp    eax,ebx
  40f8a1:	c6 44 24 34 07       	mov    BYTE PTR [esp+0x34],0x7
  40f8a6:	74 09                	je     0x40f8b1
  40f8a8:	8b c8                	mov    ecx,eax
  40f8aa:	e8 31 7c 01 00       	call   0x4274e0
  40f8af:	eb 02                	jmp    0x40f8b3
  40f8b1:	33 c0                	xor    eax,eax
  40f8b3:	89 86 c8 05 00 00    	mov    DWORD PTR [esi+0x5c8],eax
  40f8b9:	88 98 79 05 00 00    	mov    BYTE PTR [eax+0x579],bl
  40f8bf:	8b ce                	mov    ecx,esi
  40f8c1:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  40f8c5:	89 9e a0 05 00 00    	mov    DWORD PTR [esi+0x5a0],ebx
  40f8cb:	89 9e a8 05 00 00    	mov    DWORD PTR [esi+0x5a8],ebx
  40f8d1:	89 9e ac 05 00 00    	mov    DWORD PTR [esi+0x5ac],ebx
  40f8d7:	89 9e b0 05 00 00    	mov    DWORD PTR [esi+0x5b0],ebx
  40f8dd:	89 9e a4 05 00 00    	mov    DWORD PTR [esi+0x5a4],ebx
  40f8e3:	89 9e b4 05 00 00    	mov    DWORD PTR [esi+0x5b4],ebx
  40f8e9:	89 9e bc 05 00 00    	mov    DWORD PTR [esi+0x5bc],ebx
  40f8ef:	89 9e c0 05 00 00    	mov    DWORD PTR [esi+0x5c0],ebx
  40f8f5:	89 9e c4 05 00 00    	mov    DWORD PTR [esi+0x5c4],ebx
  40f8fb:	89 9e b8 05 00 00    	mov    DWORD PTR [esi+0x5b8],ebx
  40f901:	89 9e 7c 05 00 00    	mov    DWORD PTR [esi+0x57c],ebx
  40f907:	89 9e 80 05 00 00    	mov    DWORD PTR [esi+0x580],ebx
  40f90d:	e8 3e 0a 00 00       	call   0x410350
  40f912:	6a 14                	push   0x14
  40f914:	e8 46 1d 04 00       	call   0x45165f
  40f919:	83 c4 04             	add    esp,0x4
  40f91c:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  40f920:	3b c3                	cmp    eax,ebx
  40f922:	c6 44 24 34 08       	mov    BYTE PTR [esp+0x34],0x8
  40f927:	74 0d                	je     0x40f936
  40f929:	6a 01                	push   0x1
  40f92b:	8b c8                	mov    ecx,eax
  40f92d:	e8 be 1d 03 00       	call   0x4416f0
  40f932:	8b f8                	mov    edi,eax
  40f934:	eb 02                	jmp    0x40f938
  40f936:	33 ff                	xor    edi,edi
  40f938:	53                   	push   ebx
  40f939:	6a 01                	push   0x1
  40f93b:	68 44 92 46 00       	push   0x469244
  40f940:	8b cf                	mov    ecx,edi
  40f942:	88 5c 24 40          	mov    BYTE PTR [esp+0x40],bl
  40f946:	e8 75 1e 03 00       	call   0x4417c0
  40f94b:	84 c0                	test   al,al
  40f94d:	74 33                	je     0x40f982
  40f94f:	68 38 f8 89 00       	push   0x89f838
  40f954:	8b cf                	mov    ecx,edi
  40f956:	e8 d5 d4 01 00       	call   0x42ce30
  40f95b:	50                   	push   eax
  40f95c:	53                   	push   ebx
  40f95d:	8b cf                	mov    ecx,edi
  40f95f:	e8 cc 20 03 00       	call   0x441a30
  40f964:	8b cf                	mov    ecx,edi
  40f966:	e8 c5 d4 01 00       	call   0x42ce30
  40f96b:	50                   	push   eax
  40f96c:	68 38 f8 89 00       	push   0x89f838
  40f971:	e8 5a 8b 01 00       	call   0x4284d0
  40f976:	83 c4 08             	add    esp,0x8
  40f979:	8b cf                	mov    ecx,edi
  40f97b:	e8 80 20 03 00       	call   0x441a00
  40f980:	eb 15                	jmp    0x40f997
  40f982:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  40f988:	68 44 92 46 00       	push   0x469244
  40f98d:	68 1c 92 46 00       	push   0x46921c
  40f992:	e8 b9 26 02 00       	call   0x432050
  40f997:	53                   	push   ebx
  40f998:	6a 01                	push   0x1
  40f99a:	68 08 92 46 00       	push   0x469208
  40f99f:	8b cf                	mov    ecx,edi
  40f9a1:	e8 1a 1e 03 00       	call   0x4417c0
  40f9a6:	84 c0                	test   al,al
  40f9a8:	0f 84 9c 00 00 00    	je     0x40fa4a
  40f9ae:	8b cf                	mov    ecx,edi
  40f9b0:	e8 7b d4 01 00       	call   0x42ce30
  40f9b5:	8b c8                	mov    ecx,eax
  40f9b7:	c1 e9 02             	shr    ecx,0x2
  40f9ba:	83 f9 03             	cmp    ecx,0x3
  40f9bd:	7e 6f                	jle    0x40fa2e
  40f9bf:	8b c1                	mov    eax,ecx
  40f9c1:	55                   	push   ebp
  40f9c2:	99                   	cdq
  40f9c3:	bd 03 00 00 00       	mov    ebp,0x3
  40f9c8:	f7 fd                	idiv   ebp
  40f9ca:	5d                   	pop    ebp
  40f9cb:	85 d2                	test   edx,edx
  40f9cd:	75 5f                	jne    0x40fa2e
  40f9cf:	b8 56 55 55 55       	mov    eax,0x55555556
  40f9d4:	f7 e9                	imul   ecx
  40f9d6:	8b c2                	mov    eax,edx
  40f9d8:	c1 e8 1f             	shr    eax,0x1f
  40f9db:	c1 e1 02             	shl    ecx,0x2
  40f9de:	03 d0                	add    edx,eax
  40f9e0:	51                   	push   ecx
  40f9e1:	89 15 c4 fc 89 00    	mov    DWORD PTR ds:0x89fcc4,edx
  40f9e7:	e8 73 1c 04 00       	call   0x45165f
  40f9ec:	83 c4 04             	add    esp,0x4
  40f9ef:	8b cf                	mov    ecx,edi
  40f9f1:	a3 c0 fc 89 00       	mov    ds:0x89fcc0,eax
  40f9f6:	50                   	push   eax
  40f9f7:	e8 34 d4 01 00       	call   0x42ce30
  40f9fc:	50                   	push   eax
  40f9fd:	53                   	push   ebx
  40f9fe:	8b cf                	mov    ecx,edi
  40fa00:	e8 2b 20 03 00       	call   0x441a30
  40fa05:	84 c0                	test   al,al
  40fa07:	75 0c                	jne    0x40fa15
  40fa09:	68 08 92 46 00       	push   0x469208
  40fa0e:	68 e0 91 46 00       	push   0x4691e0
  40fa13:	eb 23                	jmp    0x40fa38
  40fa15:	8b cf                	mov    ecx,edi
  40fa17:	e8 14 d4 01 00       	call   0x42ce30
  40fa1c:	8b 15 c0 fc 89 00    	mov    edx,DWORD PTR ds:0x89fcc0
  40fa22:	50                   	push   eax
  40fa23:	52                   	push   edx
  40fa24:	e8 a7 8a 01 00       	call   0x4284d0
  40fa29:	83 c4 08             	add    esp,0x8
  40fa2c:	eb 15                	jmp    0x40fa43
  40fa2e:	68 08 92 46 00       	push   0x469208
  40fa33:	68 1c 92 46 00       	push   0x46921c
  40fa38:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  40fa3e:	e8 0d 26 02 00       	call   0x432050
  40fa43:	8b cf                	mov    ecx,edi
  40fa45:	e8 b6 1f 03 00       	call   0x441a00
  40fa4a:	3b fb                	cmp    edi,ebx
  40fa4c:	74 10                	je     0x40fa5e
  40fa4e:	8b cf                	mov    ecx,edi
  40fa50:	e8 bb 1c 03 00       	call   0x441710
  40fa55:	57                   	push   edi
  40fa56:	e8 f5 1a 04 00       	call   0x451550
  40fa5b:	83 c4 04             	add    esp,0x4
  40fa5e:	8b 4c 24 2c          	mov    ecx,DWORD PTR [esp+0x2c]
  40fa62:	8b c6                	mov    eax,esi
  40fa64:	5f                   	pop    edi
  40fa65:	5e                   	pop    esi
  40fa66:	5b                   	pop    ebx
  40fa67:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40fa6e:	83 c4 2c             	add    esp,0x2c
  40fa71:	c3                   	ret
