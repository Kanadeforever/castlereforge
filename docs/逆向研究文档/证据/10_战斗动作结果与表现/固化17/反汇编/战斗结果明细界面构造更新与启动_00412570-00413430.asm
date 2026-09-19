; Oracle: RPG.exe SHA256 b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; objdump -d -Mintel, VA 0x412570..0x413430

  412570:	6a ff                	push   0xffffffff
  412572:	68 dc d5 45 00       	push   0x45d5dc
  412577:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41257d:	50                   	push   eax
  41257e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  412585:	83 ec 20             	sub    esp,0x20
  412588:	53                   	push   ebx
  412589:	55                   	push   ebp
  41258a:	56                   	push   esi
  41258b:	8b f1                	mov    esi,ecx
  41258d:	57                   	push   edi
  41258e:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  412592:	e8 79 e9 01 00       	call   0x430f10
  412597:	33 db                	xor    ebx,ebx
  412599:	8b ce                	mov    ecx,esi
  41259b:	53                   	push   ebx
  41259c:	68 c0 91 21 00       	push   0x2191c0
  4125a1:	56                   	push   esi
  4125a2:	89 5c 24 44          	mov    DWORD PTR [esp+0x44],ebx
  4125a6:	c7 06 b0 07 46 00    	mov    DWORD PTR [esi],0x4607b0
  4125ac:	e8 7f 1e 02 00       	call   0x434430
  4125b1:	8d 8e 18 02 00 00    	lea    ecx,[esi+0x218]
  4125b7:	e8 74 27 ff ff       	call   0x404d30
  4125bc:	89 44 24 2c          	mov    DWORD PTR [esp+0x2c],eax
  4125c0:	8d 86 68 05 00 00    	lea    eax,[esi+0x568]
  4125c6:	68 64 93 46 00       	push   0x469364
  4125cb:	53                   	push   ebx
  4125cc:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  4125d0:	53                   	push   ebx
  4125d1:	8d 86 6c 05 00 00    	lea    eax,[esi+0x56c]
  4125d7:	6a 64                	push   0x64
  4125d9:	6a 5a                	push   0x5a
  4125db:	8b ce                	mov    ecx,esi
  4125dd:	89 44 24 30          	mov    DWORD PTR [esp+0x30],eax
  4125e1:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  4125e7:	e8 f4 ea 01 00       	call   0x4310e0
  4125ec:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  4125f2:	bd 01 00 00 00       	mov    ebp,0x1
  4125f7:	55                   	push   ebp
  4125f8:	e8 f3 c0 02 00       	call   0x43e6f0
  4125fd:	6a 2c                	push   0x2c
  4125ff:	e8 5b f0 03 00       	call   0x45165f
  412604:	83 c4 04             	add    esp,0x4
  412607:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  41260b:	3b c3                	cmp    eax,ebx
  41260d:	c6 44 24 38 01       	mov    BYTE PTR [esp+0x38],0x1
  412612:	74 0e                	je     0x412622
  412614:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  412618:	51                   	push   ecx
  412619:	8b c8                	mov    ecx,eax
  41261b:	e8 30 f5 01 00       	call   0x431b50
  412620:	eb 02                	jmp    0x412624
  412622:	33 c0                	xor    eax,eax
  412624:	53                   	push   ebx
  412625:	68 f4 93 46 00       	push   0x4693f4
  41262a:	6a 20                	push   0x20
  41262c:	6a 28                	push   0x28
  41262e:	8b c8                	mov    ecx,eax
  412630:	88 5c 24 48          	mov    BYTE PTR [esp+0x48],bl
  412634:	89 86 00 06 00 00    	mov    DWORD PTR [esi+0x600],eax
  41263a:	e8 d1 f5 01 00       	call   0x431c10
  41263f:	6a 2c                	push   0x2c
  412641:	e8 19 f0 03 00       	call   0x45165f
  412646:	83 c4 04             	add    esp,0x4
  412649:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  41264d:	3b c3                	cmp    eax,ebx
  41264f:	c6 44 24 38 02       	mov    BYTE PTR [esp+0x38],0x2
  412654:	74 0e                	je     0x412664
  412656:	8d 54 24 18          	lea    edx,[esp+0x18]
  41265a:	8b c8                	mov    ecx,eax
  41265c:	52                   	push   edx
  41265d:	e8 ee f4 01 00       	call   0x431b50
  412662:	eb 02                	jmp    0x412666
  412664:	33 c0                	xor    eax,eax
  412666:	53                   	push   ebx
  412667:	68 dc 93 46 00       	push   0x4693dc
  41266c:	6a 20                	push   0x20
  41266e:	6a 0e                	push   0xe
  412670:	8b c8                	mov    ecx,eax
  412672:	88 5c 24 48          	mov    BYTE PTR [esp+0x48],bl
  412676:	89 86 04 06 00 00    	mov    DWORD PTR [esi+0x604],eax
  41267c:	e8 8f f5 01 00       	call   0x431c10
  412681:	6a 2c                	push   0x2c
  412683:	e8 d7 ef 03 00       	call   0x45165f
  412688:	83 c4 04             	add    esp,0x4
  41268b:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  41268f:	3b c3                	cmp    eax,ebx
  412691:	c6 44 24 38 03       	mov    BYTE PTR [esp+0x38],0x3
  412696:	74 0e                	je     0x4126a6
  412698:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  41269c:	51                   	push   ecx
  41269d:	8b c8                	mov    ecx,eax
  41269f:	e8 ac f4 01 00       	call   0x431b50
  4126a4:	eb 02                	jmp    0x4126a8
  4126a6:	33 c0                	xor    eax,eax
  4126a8:	53                   	push   ebx
  4126a9:	68 c4 93 46 00       	push   0x4693c4
  4126ae:	6a 3e                	push   0x3e
  4126b0:	68 92 00 00 00       	push   0x92
  4126b5:	8b c8                	mov    ecx,eax
  4126b7:	88 5c 24 48          	mov    BYTE PTR [esp+0x48],bl
  4126bb:	89 86 08 06 00 00    	mov    DWORD PTR [esi+0x608],eax
  4126c1:	e8 4a f5 01 00       	call   0x431c10
  4126c6:	6a 2c                	push   0x2c
  4126c8:	e8 92 ef 03 00       	call   0x45165f
  4126cd:	83 c4 04             	add    esp,0x4
  4126d0:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4126d4:	3b c3                	cmp    eax,ebx
  4126d6:	c6 44 24 38 04       	mov    BYTE PTR [esp+0x38],0x4
  4126db:	74 0e                	je     0x4126eb
  4126dd:	8d 54 24 18          	lea    edx,[esp+0x18]
  4126e1:	8b c8                	mov    ecx,eax
  4126e3:	52                   	push   edx
  4126e4:	e8 67 f4 01 00       	call   0x431b50
  4126e9:	eb 02                	jmp    0x4126ed
  4126eb:	33 c0                	xor    eax,eax
  4126ed:	53                   	push   ebx
  4126ee:	68 ac 93 46 00       	push   0x4693ac
  4126f3:	6a 5b                	push   0x5b
  4126f5:	68 91 00 00 00       	push   0x91
  4126fa:	8b c8                	mov    ecx,eax
  4126fc:	88 5c 24 48          	mov    BYTE PTR [esp+0x48],bl
  412700:	89 86 0c 06 00 00    	mov    DWORD PTR [esi+0x60c],eax
  412706:	e8 05 f5 01 00       	call   0x431c10
  41270b:	6a 54                	push   0x54
  41270d:	e8 4d ef 03 00       	call   0x45165f
  412712:	83 c4 04             	add    esp,0x4
  412715:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  412719:	3b c3                	cmp    eax,ebx
  41271b:	c6 44 24 38 05       	mov    BYTE PTR [esp+0x38],0x5
  412720:	74 0e                	je     0x412730
  412722:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  412726:	51                   	push   ecx
  412727:	8b c8                	mov    ecx,eax
  412729:	e8 e2 ed 01 00       	call   0x431510
  41272e:	eb 02                	jmp    0x412732
  412730:	33 c0                	xor    eax,eax
  412732:	53                   	push   ebx
  412733:	68 94 93 46 00       	push   0x469394
  412738:	6a 12                	push   0x12
  41273a:	6a 12                	push   0x12
  41273c:	6a 20                	push   0x20
  41273e:	68 ab 00 00 00       	push   0xab
  412743:	8b c8                	mov    ecx,eax
  412745:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  412749:	89 86 10 06 00 00    	mov    DWORD PTR [esi+0x610],eax
  41274f:	e8 ac ee 01 00       	call   0x431600
  412754:	8b 96 10 06 00 00    	mov    edx,DWORD PTR [esi+0x610]
  41275a:	6a 54                	push   0x54
  41275c:	c7 42 38 02 00 00 00 	mov    DWORD PTR [edx+0x38],0x2
  412763:	8b 86 10 06 00 00    	mov    eax,DWORD PTR [esi+0x610]
  412769:	89 68 4c             	mov    DWORD PTR [eax+0x4c],ebp
  41276c:	e8 ee ee 03 00       	call   0x45165f
  412771:	83 c4 04             	add    esp,0x4
  412774:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  412778:	3b c3                	cmp    eax,ebx
  41277a:	c6 44 24 38 06       	mov    BYTE PTR [esp+0x38],0x6
  41277f:	74 0e                	je     0x41278f
  412781:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  412785:	51                   	push   ecx
  412786:	8b c8                	mov    ecx,eax
  412788:	e8 83 ed 01 00       	call   0x431510
  41278d:	eb 02                	jmp    0x412791
  41278f:	33 c0                	xor    eax,eax
  412791:	53                   	push   ebx
  412792:	68 94 93 46 00       	push   0x469394
  412797:	6a 12                	push   0x12
  412799:	6a 12                	push   0x12
  41279b:	6a 7a                	push   0x7a
  41279d:	6a 43                	push   0x43
  41279f:	8b c8                	mov    ecx,eax
  4127a1:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  4127a5:	89 86 14 06 00 00    	mov    DWORD PTR [esi+0x614],eax
  4127ab:	e8 50 ee 01 00       	call   0x431600
  4127b0:	8b 96 14 06 00 00    	mov    edx,DWORD PTR [esi+0x614]
  4127b6:	6a 54                	push   0x54
  4127b8:	c7 42 38 0a 00 00 00 	mov    DWORD PTR [edx+0x38],0xa
  4127bf:	e8 9b ee 03 00       	call   0x45165f
  4127c4:	83 c4 04             	add    esp,0x4
  4127c7:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4127cb:	3b c3                	cmp    eax,ebx
  4127cd:	c6 44 24 38 07       	mov    BYTE PTR [esp+0x38],0x7
  4127d2:	74 0e                	je     0x4127e2
  4127d4:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  4127d8:	51                   	push   ecx
  4127d9:	8b c8                	mov    ecx,eax
  4127db:	e8 30 ed 01 00       	call   0x431510
  4127e0:	eb 02                	jmp    0x4127e4
  4127e2:	33 c0                	xor    eax,eax
  4127e4:	53                   	push   ebx
  4127e5:	68 7c 93 46 00       	push   0x46937c
  4127ea:	6a 06                	push   0x6
  4127ec:	6a 06                	push   0x6
  4127ee:	6a 45                	push   0x45
  4127f0:	68 98 00 00 00       	push   0x98
  4127f5:	8b c8                	mov    ecx,eax
  4127f7:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  4127fb:	89 86 18 06 00 00    	mov    DWORD PTR [esi+0x618],eax
  412801:	e8 fa ed 01 00       	call   0x431600
  412806:	8b 96 18 06 00 00    	mov    edx,DWORD PTR [esi+0x618]
  41280c:	bf 04 00 00 00       	mov    edi,0x4
  412811:	6a 54                	push   0x54
  412813:	89 7a 38             	mov    DWORD PTR [edx+0x38],edi
  412816:	8b 86 18 06 00 00    	mov    eax,DWORD PTR [esi+0x618]
  41281c:	89 68 4c             	mov    DWORD PTR [eax+0x4c],ebp
  41281f:	e8 3b ee 03 00       	call   0x45165f
  412824:	83 c4 04             	add    esp,0x4
  412827:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  41282b:	3b c3                	cmp    eax,ebx
  41282d:	c6 44 24 38 08       	mov    BYTE PTR [esp+0x38],0x8
  412832:	74 0e                	je     0x412842
  412834:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  412838:	51                   	push   ecx
  412839:	8b c8                	mov    ecx,eax
  41283b:	e8 d0 ec 01 00       	call   0x431510
  412840:	eb 02                	jmp    0x412844
  412842:	33 c0                	xor    eax,eax
  412844:	53                   	push   ebx
  412845:	68 7c 93 46 00       	push   0x46937c
  41284a:	6a 06                	push   0x6
  41284c:	6a 06                	push   0x6
  41284e:	6a 45                	push   0x45
  412850:	68 be 00 00 00       	push   0xbe
  412855:	8b c8                	mov    ecx,eax
  412857:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  41285b:	89 86 1c 06 00 00    	mov    DWORD PTR [esi+0x61c],eax
  412861:	e8 9a ed 01 00       	call   0x431600
  412866:	8b 96 1c 06 00 00    	mov    edx,DWORD PTR [esi+0x61c]
  41286c:	6a 54                	push   0x54
  41286e:	89 7a 38             	mov    DWORD PTR [edx+0x38],edi
  412871:	8b 86 1c 06 00 00    	mov    eax,DWORD PTR [esi+0x61c]
  412877:	89 68 4c             	mov    DWORD PTR [eax+0x4c],ebp
  41287a:	e8 e0 ed 03 00       	call   0x45165f
  41287f:	83 c4 04             	add    esp,0x4
  412882:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  412886:	3b c3                	cmp    eax,ebx
  412888:	c6 44 24 38 09       	mov    BYTE PTR [esp+0x38],0x9
  41288d:	74 0e                	je     0x41289d
  41288f:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  412893:	51                   	push   ecx
  412894:	8b c8                	mov    ecx,eax
  412896:	e8 75 ec 01 00       	call   0x431510
  41289b:	eb 02                	jmp    0x41289f
  41289d:	33 c0                	xor    eax,eax
  41289f:	53                   	push   ebx
  4128a0:	68 7c 93 46 00       	push   0x46937c
  4128a5:	6a 06                	push   0x6
  4128a7:	6a 06                	push   0x6
  4128a9:	6a 61                	push   0x61
  4128ab:	68 98 00 00 00       	push   0x98
  4128b0:	8b c8                	mov    ecx,eax
  4128b2:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  4128b6:	89 86 20 06 00 00    	mov    DWORD PTR [esi+0x620],eax
  4128bc:	e8 3f ed 01 00       	call   0x431600
  4128c1:	8b 96 20 06 00 00    	mov    edx,DWORD PTR [esi+0x620]
  4128c7:	6a 54                	push   0x54
  4128c9:	89 7a 38             	mov    DWORD PTR [edx+0x38],edi
  4128cc:	8b 86 20 06 00 00    	mov    eax,DWORD PTR [esi+0x620]
  4128d2:	89 68 4c             	mov    DWORD PTR [eax+0x4c],ebp
  4128d5:	e8 85 ed 03 00       	call   0x45165f
  4128da:	83 c4 04             	add    esp,0x4
  4128dd:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4128e1:	3b c3                	cmp    eax,ebx
  4128e3:	c6 44 24 38 0a       	mov    BYTE PTR [esp+0x38],0xa
  4128e8:	74 0e                	je     0x4128f8
  4128ea:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  4128ee:	51                   	push   ecx
  4128ef:	8b c8                	mov    ecx,eax
  4128f1:	e8 1a ec 01 00       	call   0x431510
  4128f6:	eb 02                	jmp    0x4128fa
  4128f8:	33 c0                	xor    eax,eax
  4128fa:	53                   	push   ebx
  4128fb:	68 7c 93 46 00       	push   0x46937c
  412900:	6a 06                	push   0x6
  412902:	6a 06                	push   0x6
  412904:	6a 61                	push   0x61
  412906:	68 be 00 00 00       	push   0xbe
  41290b:	8b c8                	mov    ecx,eax
  41290d:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  412911:	89 86 24 06 00 00    	mov    DWORD PTR [esi+0x624],eax
  412917:	e8 e4 ec 01 00       	call   0x431600
  41291c:	8b 96 24 06 00 00    	mov    edx,DWORD PTR [esi+0x624]
  412922:	6a 48                	push   0x48
  412924:	89 7a 38             	mov    DWORD PTR [edx+0x38],edi
  412927:	8b 86 24 06 00 00    	mov    eax,DWORD PTR [esi+0x624]
  41292d:	89 68 4c             	mov    DWORD PTR [eax+0x4c],ebp
  412930:	e8 2a ed 03 00       	call   0x45165f
  412935:	83 c4 04             	add    esp,0x4
  412938:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  41293c:	3b c3                	cmp    eax,ebx
  41293e:	c6 44 24 38 0b       	mov    BYTE PTR [esp+0x38],0xb
  412943:	74 0e                	je     0x412953
  412945:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  412949:	51                   	push   ecx
  41294a:	8b c8                	mov    ecx,eax
  41294c:	e8 8f e8 01 00       	call   0x4311e0
  412951:	eb 02                	jmp    0x412955
  412953:	33 c0                	xor    eax,eax
  412955:	8b 8e 68 05 00 00    	mov    ecx,DWORD PTR [esi+0x568]
  41295b:	8d 96 28 06 00 00    	lea    edx,[esi+0x628]
  412961:	52                   	push   edx
  412962:	53                   	push   ebx
  412963:	89 02                	mov    DWORD PTR [edx],eax
  412965:	8b 96 6c 05 00 00    	mov    edx,DWORD PTR [esi+0x56c]
  41296b:	68 e0 01 00 00       	push   0x1e0
  412970:	68 80 02 00 00       	push   0x280
  412975:	f7 da                	neg    edx
  412977:	f7 d9                	neg    ecx
  412979:	52                   	push   edx
  41297a:	51                   	push   ecx
  41297b:	8b c8                	mov    ecx,eax
  41297d:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  412981:	e8 3a e9 01 00       	call   0x4312c0
  412986:	bd a0 00 00 00       	mov    ebp,0xa0
  41298b:	8d be 2c 06 00 00    	lea    edi,[esi+0x62c]
  412991:	6a 48                	push   0x48
  412993:	e8 c7 ec 03 00       	call   0x45165f
  412998:	83 c4 04             	add    esp,0x4
  41299b:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  41299f:	3b c3                	cmp    eax,ebx
  4129a1:	c6 44 24 38 0c       	mov    BYTE PTR [esp+0x38],0xc
  4129a6:	74 0e                	je     0x4129b6
  4129a8:	8d 54 24 18          	lea    edx,[esp+0x18]
  4129ac:	8b c8                	mov    ecx,eax
  4129ae:	52                   	push   edx
  4129af:	e8 3c ef 01 00       	call   0x4318f0
  4129b4:	eb 02                	jmp    0x4129b8
  4129b6:	33 c0                	xor    eax,eax
  4129b8:	57                   	push   edi
  4129b9:	68 bc 92 46 00       	push   0x4692bc
  4129be:	55                   	push   ebp
  4129bf:	6a 19                	push   0x19
  4129c1:	8b c8                	mov    ecx,eax
  4129c3:	88 5c 24 48          	mov    BYTE PTR [esp+0x48],bl
  4129c7:	89 07                	mov    DWORD PTR [edi],eax
  4129c9:	e8 22 f0 01 00       	call   0x4319f0
  4129ce:	8b 0d b0 b2 46 00    	mov    ecx,DWORD PTR ds:0x46b2b0
  4129d4:	8b 07                	mov    eax,DWORD PTR [edi]
  4129d6:	6a 30                	push   0x30
  4129d8:	6a 40                	push   0x40
  4129da:	8b 11                	mov    edx,DWORD PTR [ecx]
  4129dc:	89 50 38             	mov    DWORD PTR [eax+0x38],edx
  4129df:	8b 07                	mov    eax,DWORD PTR [edi]
  4129e1:	c7 40 30 14 00 00 00 	mov    DWORD PTR [eax+0x30],0x14
  4129e8:	8b 0f                	mov    ecx,DWORD PTR [edi]
  4129ea:	e8 a1 f0 01 00       	call   0x431a90
  4129ef:	8b 0f                	mov    ecx,DWORD PTR [edi]
  4129f1:	83 c5 1c             	add    ebp,0x1c
  4129f4:	83 c7 04             	add    edi,0x4
  4129f7:	81 fd 2c 01 00 00    	cmp    ebp,0x12c
  4129fd:	c7 41 40 09 00 00 00 	mov    DWORD PTR [ecx+0x40],0x9
  412a04:	7c 8b                	jl     0x412991
  412a06:	8d be 7c 05 00 00    	lea    edi,[esi+0x57c]
  412a0c:	bd 1e 00 00 00       	mov    ebp,0x1e
  412a11:	6a 28                	push   0x28
  412a13:	e8 47 ec 03 00       	call   0x45165f
  412a18:	89 07                	mov    DWORD PTR [edi],eax
  412a1a:	83 c4 04             	add    esp,0x4
  412a1d:	83 c7 04             	add    edi,0x4
  412a20:	4d                   	dec    ebp
  412a21:	75 ee                	jne    0x412a11
  412a23:	8b 4c 24 30          	mov    ecx,DWORD PTR [esp+0x30]
  412a27:	89 9e 40 06 00 00    	mov    DWORD PTR [esi+0x640],ebx
  412a2d:	89 9e 44 06 00 00    	mov    DWORD PTR [esi+0x644],ebx
  412a33:	89 9e 48 06 00 00    	mov    DWORD PTR [esi+0x648],ebx
  412a39:	89 9e 4c 06 00 00    	mov    DWORD PTR [esi+0x64c],ebx
  412a3f:	89 9e fc 05 00 00    	mov    DWORD PTR [esi+0x5fc],ebx
  412a45:	8b c6                	mov    eax,esi
  412a47:	5f                   	pop    edi
  412a48:	5e                   	pop    esi
  412a49:	5d                   	pop    ebp
  412a4a:	5b                   	pop    ebx
  412a4b:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  412a52:	83 c4 2c             	add    esp,0x2c
  412a55:	c3                   	ret
  412a56:	90                   	nop
  412a57:	90                   	nop
  412a58:	90                   	nop
  412a59:	90                   	nop
  412a5a:	90                   	nop
  412a5b:	90                   	nop
  412a5c:	90                   	nop
  412a5d:	90                   	nop
  412a5e:	90                   	nop
  412a5f:	90                   	nop
  412a60:	6a ff                	push   0xffffffff
  412a62:	68 f8 d5 45 00       	push   0x45d5f8
  412a67:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  412a6d:	50                   	push   eax
  412a6e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  412a75:	51                   	push   ecx
  412a76:	53                   	push   ebx
  412a77:	55                   	push   ebp
  412a78:	56                   	push   esi
  412a79:	8b f1                	mov    esi,ecx
  412a7b:	57                   	push   edi
  412a7c:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  412a80:	c7 06 b0 07 46 00    	mov    DWORD PTR [esi],0x4607b0
  412a86:	8b be 00 06 00 00    	mov    edi,DWORD PTR [esi+0x600]
  412a8c:	33 ed                	xor    ebp,ebp
  412a8e:	3b fd                	cmp    edi,ebp
  412a90:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  412a94:	74 16                	je     0x412aac
  412a96:	8b cf                	mov    ecx,edi
  412a98:	e8 43 f1 01 00       	call   0x431be0
  412a9d:	57                   	push   edi
  412a9e:	e8 ad ea 03 00       	call   0x451550
  412aa3:	83 c4 04             	add    esp,0x4
  412aa6:	89 ae 00 06 00 00    	mov    DWORD PTR [esi+0x600],ebp
  412aac:	8b be 04 06 00 00    	mov    edi,DWORD PTR [esi+0x604]
  412ab2:	3b fd                	cmp    edi,ebp
  412ab4:	74 16                	je     0x412acc
  412ab6:	8b cf                	mov    ecx,edi
  412ab8:	e8 23 f1 01 00       	call   0x431be0
  412abd:	57                   	push   edi
  412abe:	e8 8d ea 03 00       	call   0x451550
  412ac3:	83 c4 04             	add    esp,0x4
  412ac6:	89 ae 04 06 00 00    	mov    DWORD PTR [esi+0x604],ebp
  412acc:	8b be 08 06 00 00    	mov    edi,DWORD PTR [esi+0x608]
  412ad2:	3b fd                	cmp    edi,ebp
  412ad4:	74 16                	je     0x412aec
  412ad6:	8b cf                	mov    ecx,edi
  412ad8:	e8 03 f1 01 00       	call   0x431be0
  412add:	57                   	push   edi
  412ade:	e8 6d ea 03 00       	call   0x451550
  412ae3:	83 c4 04             	add    esp,0x4
  412ae6:	89 ae 08 06 00 00    	mov    DWORD PTR [esi+0x608],ebp
  412aec:	8b be 0c 06 00 00    	mov    edi,DWORD PTR [esi+0x60c]
  412af2:	3b fd                	cmp    edi,ebp
  412af4:	74 16                	je     0x412b0c
  412af6:	8b cf                	mov    ecx,edi
  412af8:	e8 e3 f0 01 00       	call   0x431be0
  412afd:	57                   	push   edi
  412afe:	e8 4d ea 03 00       	call   0x451550
  412b03:	83 c4 04             	add    esp,0x4
  412b06:	89 ae 0c 06 00 00    	mov    DWORD PTR [esi+0x60c],ebp
  412b0c:	8b be 10 06 00 00    	mov    edi,DWORD PTR [esi+0x610]
  412b12:	3b fd                	cmp    edi,ebp
  412b14:	74 16                	je     0x412b2c
  412b16:	8b cf                	mov    ecx,edi
  412b18:	e8 b3 ea 01 00       	call   0x4315d0
  412b1d:	57                   	push   edi
  412b1e:	e8 2d ea 03 00       	call   0x451550
  412b23:	83 c4 04             	add    esp,0x4
  412b26:	89 ae 10 06 00 00    	mov    DWORD PTR [esi+0x610],ebp
  412b2c:	8b be 14 06 00 00    	mov    edi,DWORD PTR [esi+0x614]
  412b32:	3b fd                	cmp    edi,ebp
  412b34:	74 16                	je     0x412b4c
  412b36:	8b cf                	mov    ecx,edi
  412b38:	e8 93 ea 01 00       	call   0x4315d0
  412b3d:	57                   	push   edi
  412b3e:	e8 0d ea 03 00       	call   0x451550
  412b43:	83 c4 04             	add    esp,0x4
  412b46:	89 ae 14 06 00 00    	mov    DWORD PTR [esi+0x614],ebp
  412b4c:	8b be 18 06 00 00    	mov    edi,DWORD PTR [esi+0x618]
  412b52:	3b fd                	cmp    edi,ebp
  412b54:	74 16                	je     0x412b6c
  412b56:	8b cf                	mov    ecx,edi
  412b58:	e8 73 ea 01 00       	call   0x4315d0
  412b5d:	57                   	push   edi
  412b5e:	e8 ed e9 03 00       	call   0x451550
  412b63:	83 c4 04             	add    esp,0x4
  412b66:	89 ae 18 06 00 00    	mov    DWORD PTR [esi+0x618],ebp
  412b6c:	8b be 1c 06 00 00    	mov    edi,DWORD PTR [esi+0x61c]
  412b72:	3b fd                	cmp    edi,ebp
  412b74:	74 16                	je     0x412b8c
  412b76:	8b cf                	mov    ecx,edi
  412b78:	e8 53 ea 01 00       	call   0x4315d0
  412b7d:	57                   	push   edi
  412b7e:	e8 cd e9 03 00       	call   0x451550
  412b83:	83 c4 04             	add    esp,0x4
  412b86:	89 ae 1c 06 00 00    	mov    DWORD PTR [esi+0x61c],ebp
  412b8c:	8b be 20 06 00 00    	mov    edi,DWORD PTR [esi+0x620]
  412b92:	3b fd                	cmp    edi,ebp
  412b94:	74 16                	je     0x412bac
  412b96:	8b cf                	mov    ecx,edi
  412b98:	e8 33 ea 01 00       	call   0x4315d0
  412b9d:	57                   	push   edi
  412b9e:	e8 ad e9 03 00       	call   0x451550
  412ba3:	83 c4 04             	add    esp,0x4
  412ba6:	89 ae 20 06 00 00    	mov    DWORD PTR [esi+0x620],ebp
  412bac:	8b be 24 06 00 00    	mov    edi,DWORD PTR [esi+0x624]
  412bb2:	3b fd                	cmp    edi,ebp
  412bb4:	74 16                	je     0x412bcc
  412bb6:	8b cf                	mov    ecx,edi
  412bb8:	e8 13 ea 01 00       	call   0x4315d0
  412bbd:	57                   	push   edi
  412bbe:	e8 8d e9 03 00       	call   0x451550
  412bc3:	83 c4 04             	add    esp,0x4
  412bc6:	89 ae 24 06 00 00    	mov    DWORD PTR [esi+0x624],ebp
  412bcc:	8b be 28 06 00 00    	mov    edi,DWORD PTR [esi+0x628]
  412bd2:	3b fd                	cmp    edi,ebp
  412bd4:	74 16                	je     0x412bec
  412bd6:	8b cf                	mov    ecx,edi
  412bd8:	e8 b3 e6 01 00       	call   0x431290
  412bdd:	57                   	push   edi
  412bde:	e8 6d e9 03 00       	call   0x451550
  412be3:	83 c4 04             	add    esp,0x4
  412be6:	89 ae 28 06 00 00    	mov    DWORD PTR [esi+0x628],ebp
  412bec:	8d be 7c 05 00 00    	lea    edi,[esi+0x57c]
  412bf2:	bb 1e 00 00 00       	mov    ebx,0x1e
  412bf7:	8b 07                	mov    eax,DWORD PTR [edi]
  412bf9:	3b c5                	cmp    eax,ebp
  412bfb:	74 0b                	je     0x412c08
  412bfd:	50                   	push   eax
  412bfe:	e8 4d e9 03 00       	call   0x451550
  412c03:	83 c4 04             	add    esp,0x4
  412c06:	89 2f                	mov    DWORD PTR [edi],ebp
  412c08:	83 c7 04             	add    edi,0x4
  412c0b:	4b                   	dec    ebx
  412c0c:	75 e9                	jne    0x412bf7
  412c0e:	8d 9e 2c 06 00 00    	lea    ebx,[esi+0x62c]
  412c14:	bd 05 00 00 00       	mov    ebp,0x5
  412c19:	8b 3b                	mov    edi,DWORD PTR [ebx]
  412c1b:	85 ff                	test   edi,edi
  412c1d:	74 16                	je     0x412c35
  412c1f:	8b cf                	mov    ecx,edi
  412c21:	e8 7a ed 01 00       	call   0x4319a0
  412c26:	57                   	push   edi
  412c27:	e8 24 e9 03 00       	call   0x451550
  412c2c:	83 c4 04             	add    esp,0x4
  412c2f:	c7 03 00 00 00 00    	mov    DWORD PTR [ebx],0x0
  412c35:	83 c3 04             	add    ebx,0x4
  412c38:	4d                   	dec    ebp
  412c39:	75 de                	jne    0x412c19
  412c3b:	8b ce                	mov    ecx,esi
  412c3d:	c7 44 24 1c ff ff ff 	mov    DWORD PTR [esp+0x1c],0xffffffff
  412c44:	ff 
  412c45:	e8 f6 e3 01 00       	call   0x431040
  412c4a:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  412c4e:	5f                   	pop    edi
  412c4f:	5e                   	pop    esi
  412c50:	5d                   	pop    ebp
  412c51:	5b                   	pop    ebx
  412c52:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  412c59:	83 c4 10             	add    esp,0x10
  412c5c:	c3                   	ret
  412c5d:	90                   	nop
  412c5e:	90                   	nop
  412c5f:	90                   	nop
  412c60:	8b c1                	mov    eax,ecx
  412c62:	8b 88 4c 06 00 00    	mov    ecx,DWORD PTR [eax+0x64c]
  412c68:	83 f9 08             	cmp    ecx,0x8
  412c6b:	7d 2f                	jge    0x412c9c
  412c6d:	8d 14 cd 00 00 00 00 	lea    edx,[ecx*8+0x0]
  412c74:	56                   	push   esi
  412c75:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  412c79:	2b d1                	sub    edx,ecx
  412c7b:	c1 e2 04             	shl    edx,0x4
  412c7e:	57                   	push   edi
  412c7f:	b9 1c 00 00 00       	mov    ecx,0x1c
  412c84:	8d bc 02 50 06 00 00 	lea    edi,[edx+eax*1+0x650]
  412c8b:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  412c8d:	8b 88 4c 06 00 00    	mov    ecx,DWORD PTR [eax+0x64c]
  412c93:	5f                   	pop    edi
  412c94:	41                   	inc    ecx
  412c95:	5e                   	pop    esi
  412c96:	89 88 4c 06 00 00    	mov    DWORD PTR [eax+0x64c],ecx
  412c9c:	c2 04 00             	ret    0x4
  412c9f:	90                   	nop
  412ca0:	53                   	push   ebx
  412ca1:	56                   	push   esi
  412ca2:	8b f1                	mov    esi,ecx
  412ca4:	57                   	push   edi
  412ca5:	8b 86 28 06 00 00    	mov    eax,DWORD PTR [esi+0x628]
  412cab:	8a 8e 79 05 00 00    	mov    cl,BYTE PTR [esi+0x579]
  412cb1:	8d be 2c 06 00 00    	lea    edi,[esi+0x62c]
  412cb7:	88 48 45             	mov    BYTE PTR [eax+0x45],cl
  412cba:	8b c7                	mov    eax,edi
  412cbc:	b9 05 00 00 00       	mov    ecx,0x5
  412cc1:	8b 10                	mov    edx,DWORD PTR [eax]
  412cc3:	8a 9e 79 05 00 00    	mov    bl,BYTE PTR [esi+0x579]
  412cc9:	83 c0 04             	add    eax,0x4
  412ccc:	49                   	dec    ecx
  412ccd:	88 5a 34             	mov    BYTE PTR [edx+0x34],bl
  412cd0:	75 ef                	jne    0x412cc1
  412cd2:	8a 86 79 05 00 00    	mov    al,BYTE PTR [esi+0x579]
  412cd8:	33 db                	xor    ebx,ebx
  412cda:	3a c3                	cmp    al,bl
  412cdc:	0f 84 11 03 00 00    	je     0x412ff3
  412ce2:	8b 86 44 06 00 00    	mov    eax,DWORD PTR [esi+0x644]
  412ce8:	3b c3                	cmp    eax,ebx
  412cea:	0f 84 89 02 00 00    	je     0x412f79
  412cf0:	b9 01 00 00 00       	mov    ecx,0x1
  412cf5:	3b c1                	cmp    eax,ecx
  412cf7:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412cfd:	8d 14 40             	lea    edx,[eax+eax*2]
  412d00:	0f 85 aa 00 00 00    	jne    0x412db0
  412d06:	8b 86 64 05 00 00    	mov    eax,DWORD PTR [esi+0x564]
  412d0c:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412d12:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412d18:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412d1e:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412d24:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412d2a:	8d 14 40             	lea    edx,[eax+eax*2]
  412d2d:	8b 86 00 06 00 00    	mov    eax,DWORD PTR [esi+0x600]
  412d33:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412d36:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412d3c:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412d42:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412d48:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412d4e:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412d54:	8d 14 40             	lea    edx,[eax+eax*2]
  412d57:	8b 86 04 06 00 00    	mov    eax,DWORD PTR [esi+0x604]
  412d5d:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412d60:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412d66:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412d6c:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412d72:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412d78:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412d7e:	8d 14 40             	lea    edx,[eax+eax*2]
  412d81:	8b 86 08 06 00 00    	mov    eax,DWORD PTR [esi+0x608]
  412d87:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412d8a:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412d90:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412d96:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412d9c:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412da2:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412da8:	8d 14 40             	lea    edx,[eax+eax*2]
  412dab:	e9 d2 00 00 00       	jmp    0x412e82
  412db0:	8b c2                	mov    eax,edx
  412db2:	ba 10 00 00 00       	mov    edx,0x10
  412db7:	2b d0                	sub    edx,eax
  412db9:	8b 86 64 05 00 00    	mov    eax,DWORD PTR [esi+0x564]
  412dbf:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412dc5:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412dcb:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412dd1:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412dd7:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412ddd:	8d 14 40             	lea    edx,[eax+eax*2]
  412de0:	8b c2                	mov    eax,edx
  412de2:	ba 10 00 00 00       	mov    edx,0x10
  412de7:	2b d0                	sub    edx,eax
  412de9:	8b 86 00 06 00 00    	mov    eax,DWORD PTR [esi+0x600]
  412def:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412df2:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412df8:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412dfe:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412e04:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412e0a:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412e10:	8d 14 40             	lea    edx,[eax+eax*2]
  412e13:	8b c2                	mov    eax,edx
  412e15:	ba 10 00 00 00       	mov    edx,0x10
  412e1a:	2b d0                	sub    edx,eax
  412e1c:	8b 86 04 06 00 00    	mov    eax,DWORD PTR [esi+0x604]
  412e22:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412e25:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412e2b:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412e31:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412e37:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412e3d:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412e43:	8d 14 40             	lea    edx,[eax+eax*2]
  412e46:	8b c2                	mov    eax,edx
  412e48:	ba 10 00 00 00       	mov    edx,0x10
  412e4d:	2b d0                	sub    edx,eax
  412e4f:	8b 86 08 06 00 00    	mov    eax,DWORD PTR [esi+0x608]
  412e55:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412e58:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412e5e:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412e64:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412e6a:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412e70:	8b 86 40 06 00 00    	mov    eax,DWORD PTR [esi+0x640]
  412e76:	8d 14 40             	lea    edx,[eax+eax*2]
  412e79:	8b c2                	mov    eax,edx
  412e7b:	ba 10 00 00 00       	mov    edx,0x10
  412e80:	2b d0                	sub    edx,eax
  412e82:	8b 86 0c 06 00 00    	mov    eax,DWORD PTR [esi+0x60c]
  412e88:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412e8b:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412e91:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  412e97:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412e9d:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412ea3:	8b 8e 40 06 00 00    	mov    ecx,DWORD PTR [esi+0x640]
  412ea9:	41                   	inc    ecx
  412eaa:	8b c1                	mov    eax,ecx
  412eac:	89 8e 40 06 00 00    	mov    DWORD PTR [esi+0x640],ecx
  412eb2:	83 f8 05             	cmp    eax,0x5
  412eb5:	0f 8c 38 01 00 00    	jl     0x412ff3
  412ebb:	8b 86 64 05 00 00    	mov    eax,DWORD PTR [esi+0x564]
  412ec1:	83 c9 ff             	or     ecx,0xffffffff
  412ec4:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412eca:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  412ed0:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412ed6:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412edc:	8b 96 00 06 00 00    	mov    edx,DWORD PTR [esi+0x600]
  412ee2:	8b 42 1c             	mov    eax,DWORD PTR [edx+0x1c]
  412ee5:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412eeb:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  412ef1:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412ef7:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412efd:	8b 86 04 06 00 00    	mov    eax,DWORD PTR [esi+0x604]
  412f03:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412f06:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412f0c:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  412f12:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412f18:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412f1e:	8b 96 08 06 00 00    	mov    edx,DWORD PTR [esi+0x608]
  412f24:	8b 42 1c             	mov    eax,DWORD PTR [edx+0x1c]
  412f27:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412f2d:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  412f33:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412f39:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412f3f:	8b 86 0c 06 00 00    	mov    eax,DWORD PTR [esi+0x60c]
  412f45:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  412f48:	89 88 80 00 00 00    	mov    DWORD PTR [eax+0x80],ecx
  412f4e:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  412f54:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412f5a:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412f60:	83 be 44 06 00 00 02 	cmp    DWORD PTR [esi+0x644],0x2
  412f67:	75 06                	jne    0x412f6f
  412f69:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  412f6f:	89 9e 44 06 00 00    	mov    DWORD PTR [esi+0x644],ebx
  412f75:	5f                   	pop    edi
  412f76:	5e                   	pop    esi
  412f77:	5b                   	pop    ebx
  412f78:	c3                   	ret
  412f79:	8b 8e 28 06 00 00    	mov    ecx,DWORD PTR [esi+0x628]
  412f7f:	e8 fc e3 01 00       	call   0x431380
  412f84:	85 c0                	test   eax,eax
  412f86:	74 33                	je     0x412fbb
  412f88:	8b 86 f8 05 00 00    	mov    eax,DWORD PTR [esi+0x5f8]
  412f8e:	8b 8e f4 05 00 00    	mov    ecx,DWORD PTR [esi+0x5f4]
  412f94:	83 c0 05             	add    eax,0x5
  412f97:	3b c1                	cmp    eax,ecx
  412f99:	7c 14                	jl     0x412faf
  412f9b:	8b ce                	mov    ecx,esi
  412f9d:	e8 6e 00 00 00       	call   0x413010
  412fa2:	84 c0                	test   al,al
  412fa4:	75 0f                	jne    0x412fb5
  412fa6:	8b ce                	mov    ecx,esi
  412fa8:	e8 63 04 00 00       	call   0x413410
  412fad:	eb 06                	jmp    0x412fb5
  412faf:	89 86 f8 05 00 00    	mov    DWORD PTR [esi+0x5f8],eax
  412fb5:	89 9e fc 05 00 00    	mov    DWORD PTR [esi+0x5fc],ebx
  412fbb:	ff 86 fc 05 00 00    	inc    DWORD PTR [esi+0x5fc]
  412fc1:	8b 8e f8 05 00 00    	mov    ecx,DWORD PTR [esi+0x5f8]
  412fc7:	8d 04 0b             	lea    eax,[ebx+ecx*1]
  412fca:	8b 8e f4 05 00 00    	mov    ecx,DWORD PTR [esi+0x5f4]
  412fd0:	3b c1                	cmp    eax,ecx
  412fd2:	7d 0a                	jge    0x412fde
  412fd4:	8b 94 86 7c 05 00 00 	mov    edx,DWORD PTR [esi+eax*4+0x57c]
  412fdb:	52                   	push   edx
  412fdc:	eb 05                	jmp    0x412fe3
  412fde:	68 bc 92 46 00       	push   0x4692bc
  412fe3:	8b 0f                	mov    ecx,DWORD PTR [edi]
  412fe5:	e8 36 ea 01 00       	call   0x431a20
  412fea:	43                   	inc    ebx
  412feb:	83 c7 04             	add    edi,0x4
  412fee:	83 fb 05             	cmp    ebx,0x5
  412ff1:	7c ce                	jl     0x412fc1
  412ff3:	5f                   	pop    edi
  412ff4:	5e                   	pop    esi
  412ff5:	5b                   	pop    ebx
  412ff6:	c3                   	ret
  412ff7:	90                   	nop
  412ff8:	90                   	nop
  412ff9:	90                   	nop
  412ffa:	90                   	nop
  412ffb:	90                   	nop
  412ffc:	90                   	nop
  412ffd:	90                   	nop
  412ffe:	90                   	nop
  412fff:	90                   	nop
  413000:	c3                   	ret
  413001:	90                   	nop
  413002:	90                   	nop
  413003:	90                   	nop
  413004:	90                   	nop
  413005:	90                   	nop
  413006:	90                   	nop
  413007:	90                   	nop
  413008:	90                   	nop
  413009:	90                   	nop
  41300a:	90                   	nop
  41300b:	90                   	nop
  41300c:	90                   	nop
  41300d:	90                   	nop
  41300e:	90                   	nop
  41300f:	90                   	nop
  413010:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  413016:	6a ff                	push   0xffffffff
  413018:	68 1b d6 45 00       	push   0x45d61b
  41301d:	50                   	push   eax
  41301e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  413025:	81 ec 40 01 00 00    	sub    esp,0x140
  41302b:	56                   	push   esi
  41302c:	8b f1                	mov    esi,ecx
  41302e:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  413034:	8b 8e 4c 06 00 00    	mov    ecx,DWORD PTR [esi+0x64c]
  41303a:	3b c1                	cmp    eax,ecx
  41303c:	0f 8d 58 03 00 00    	jge    0x41339a
  413042:	57                   	push   edi
  413043:	6a 01                	push   0x1
  413045:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  413049:	e8 a2 e6 02 00       	call   0x4416f0
  41304e:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  413052:	33 ff                	xor    edi,edi
  413054:	51                   	push   ecx
  413055:	68 68 94 46 00       	push   0x469468
  41305a:	b9 90 1c 8e 00       	mov    ecx,0x8e1c90
  41305f:	89 bc 24 58 01 00 00 	mov    DWORD PTR [esp+0x158],edi
  413066:	e8 85 eb 02 00       	call   0x441bf0
  41306b:	84 c0                	test   al,al
  41306d:	74 12                	je     0x413081
  41306f:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  413075:	57                   	push   edi
  413076:	8d 54 24 20          	lea    edx,[esp+0x20]
  41307a:	57                   	push   edi
  41307b:	52                   	push   edx
  41307c:	e8 0f 22 ff ff       	call   0x405290
  413081:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  413087:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  41308e:	2b c8                	sub    ecx,eax
  413090:	8b 86 00 06 00 00    	mov    eax,DWORD PTR [esi+0x600]
  413096:	c1 e1 04             	shl    ecx,0x4
  413099:	8b 94 31 50 06 00 00 	mov    edx,DWORD PTR [ecx+esi*1+0x650]
  4130a0:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  4130a3:	4a                   	dec    edx
  4130a4:	52                   	push   edx
  4130a5:	e8 46 b6 02 00       	call   0x43e6f0
  4130aa:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  4130b0:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  4130b7:	2b c8                	sub    ecx,eax
  4130b9:	8b 86 04 06 00 00    	mov    eax,DWORD PTR [esi+0x604]
  4130bf:	c1 e1 04             	shl    ecx,0x4
  4130c2:	8b 94 31 50 06 00 00 	mov    edx,DWORD PTR [ecx+esi*1+0x650]
  4130c9:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  4130cc:	4a                   	dec    edx
  4130cd:	52                   	push   edx
  4130ce:	e8 1d b6 02 00       	call   0x43e6f0
  4130d3:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  4130d9:	8b 96 18 06 00 00    	mov    edx,DWORD PTR [esi+0x618]
  4130df:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  4130e6:	2b c8                	sub    ecx,eax
  4130e8:	c1 e1 04             	shl    ecx,0x4
  4130eb:	8b 84 31 5c 06 00 00 	mov    eax,DWORD PTR [ecx+esi*1+0x65c]
  4130f2:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  4130f5:	8b 8e 18 06 00 00    	mov    ecx,DWORD PTR [esi+0x618]
  4130fb:	8b 96 1c 06 00 00    	mov    edx,DWORD PTR [esi+0x61c]
  413101:	8b 41 44             	mov    eax,DWORD PTR [ecx+0x44]
  413104:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  413107:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  41310d:	8b 96 20 06 00 00    	mov    edx,DWORD PTR [esi+0x620]
  413113:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  41311a:	2b c8                	sub    ecx,eax
  41311c:	c1 e1 04             	shl    ecx,0x4
  41311f:	8b 84 31 60 06 00 00 	mov    eax,DWORD PTR [ecx+esi*1+0x660]
  413126:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  413129:	8b 8e 20 06 00 00    	mov    ecx,DWORD PTR [esi+0x620]
  41312f:	8b 96 24 06 00 00    	mov    edx,DWORD PTR [esi+0x624]
  413135:	8b 41 44             	mov    eax,DWORD PTR [ecx+0x44]
  413138:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  41313b:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  413141:	8b 96 14 06 00 00    	mov    edx,DWORD PTR [esi+0x614]
  413147:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  41314e:	2b c8                	sub    ecx,eax
  413150:	c1 e1 04             	shl    ecx,0x4
  413153:	8b 84 31 58 06 00 00 	mov    eax,DWORD PTR [ecx+esi*1+0x658]
  41315a:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  41315d:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  413163:	8b 96 10 06 00 00    	mov    edx,DWORD PTR [esi+0x610]
  413169:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  413170:	2b c8                	sub    ecx,eax
  413172:	c1 e1 04             	shl    ecx,0x4
  413175:	8b 84 31 54 06 00 00 	mov    eax,DWORD PTR [ecx+esi*1+0x654]
  41317c:	8b ce                	mov    ecx,esi
  41317e:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  413181:	e8 aa 02 00 00       	call   0x413430
  413186:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  41318c:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  413193:	2b c8                	sub    ecx,eax
  413195:	c1 e1 04             	shl    ecx,0x4
  413198:	8b 94 31 6c 06 00 00 	mov    edx,DWORD PTR [ecx+esi*1+0x66c]
  41319f:	8d 04 31             	lea    eax,[ecx+esi*1]
  4131a2:	52                   	push   edx
  4131a3:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  4131a7:	8b 80 80 06 00 00    	mov    eax,DWORD PTR [eax+0x680]
  4131ad:	50                   	push   eax
  4131ae:	68 58 94 46 00       	push   0x469458
  4131b3:	51                   	push   ecx
  4131b4:	e8 ba e8 03 00       	call   0x451a73
  4131b9:	83 c4 10             	add    esp,0x10
  4131bc:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  4131c0:	8b ce                	mov    ecx,esi
  4131c2:	52                   	push   edx
  4131c3:	e8 78 02 00 00       	call   0x413440
  4131c8:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  4131ce:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  4131d5:	2b c8                	sub    ecx,eax
  4131d7:	c1 e1 04             	shl    ecx,0x4
  4131da:	8d 04 31             	lea    eax,[ecx+esi*1]
  4131dd:	8b 90 74 06 00 00    	mov    edx,DWORD PTR [eax+0x674]
  4131e3:	8b 80 88 06 00 00    	mov    eax,DWORD PTR [eax+0x688]
  4131e9:	52                   	push   edx
  4131ea:	50                   	push   eax
  4131eb:	8d 4c 24 24          	lea    ecx,[esp+0x24]
  4131ef:	68 48 94 46 00       	push   0x469448
  4131f4:	51                   	push   ecx
  4131f5:	e8 79 e8 03 00       	call   0x451a73
  4131fa:	83 c4 10             	add    esp,0x10
  4131fd:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  413201:	8b ce                	mov    ecx,esi
  413203:	52                   	push   edx
  413204:	e8 37 02 00 00       	call   0x413440
  413209:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  41320f:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  413216:	2b c8                	sub    ecx,eax
  413218:	c1 e1 04             	shl    ecx,0x4
  41321b:	8b 94 31 70 06 00 00 	mov    edx,DWORD PTR [ecx+esi*1+0x670]
  413222:	8d 04 31             	lea    eax,[ecx+esi*1]
  413225:	52                   	push   edx
  413226:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  41322a:	8b 80 84 06 00 00    	mov    eax,DWORD PTR [eax+0x684]
  413230:	50                   	push   eax
  413231:	68 38 94 46 00       	push   0x469438
  413236:	51                   	push   ecx
  413237:	e8 37 e8 03 00       	call   0x451a73
  41323c:	83 c4 10             	add    esp,0x10
  41323f:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  413243:	8b ce                	mov    ecx,esi
  413245:	52                   	push   edx
  413246:	e8 f5 01 00 00       	call   0x413440
  41324b:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  413251:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  413258:	2b c8                	sub    ecx,eax
  41325a:	c1 e1 04             	shl    ecx,0x4
  41325d:	8b 94 31 78 06 00 00 	mov    edx,DWORD PTR [ecx+esi*1+0x678]
  413264:	8d 04 31             	lea    eax,[ecx+esi*1]
  413267:	52                   	push   edx
  413268:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  41326c:	8b 80 8c 06 00 00    	mov    eax,DWORD PTR [eax+0x68c]
  413272:	50                   	push   eax
  413273:	68 28 94 46 00       	push   0x469428
  413278:	51                   	push   ecx
  413279:	e8 f5 e7 03 00       	call   0x451a73
  41327e:	83 c4 10             	add    esp,0x10
  413281:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  413285:	8b ce                	mov    ecx,esi
  413287:	52                   	push   edx
  413288:	e8 b3 01 00 00       	call   0x413440
  41328d:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  413293:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  41329a:	2b c8                	sub    ecx,eax
  41329c:	83 c0 0f             	add    eax,0xf
  41329f:	c1 e1 04             	shl    ecx,0x4
  4132a2:	8b 94 31 7c 06 00 00 	mov    edx,DWORD PTR [ecx+esi*1+0x67c]
  4132a9:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  4132b0:	2b c8                	sub    ecx,eax
  4132b2:	52                   	push   edx
  4132b3:	c1 e1 04             	shl    ecx,0x4
  4132b6:	8d 44 24 20          	lea    eax,[esp+0x20]
  4132ba:	8b 14 31             	mov    edx,DWORD PTR [ecx+esi*1]
  4132bd:	52                   	push   edx
  4132be:	68 18 94 46 00       	push   0x469418
  4132c3:	50                   	push   eax
  4132c4:	e8 aa e7 03 00       	call   0x451a73
  4132c9:	83 c4 10             	add    esp,0x10
  4132cc:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  4132d0:	51                   	push   ecx
  4132d1:	8b ce                	mov    ecx,esi
  4132d3:	e8 68 01 00 00       	call   0x413440
  4132d8:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  4132de:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  4132e5:	2b d0                	sub    edx,eax
  4132e7:	c1 e2 04             	shl    edx,0x4
  4132ea:	39 bc 32 94 06 00 00 	cmp    DWORD PTR [edx+esi*1+0x694],edi
  4132f1:	7e 6d                	jle    0x413360
  4132f3:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  4132fa:	2b c8                	sub    ecx,eax
  4132fc:	8d 14 8f             	lea    edx,[edi+ecx*4]
  4132ff:	8b 8c 96 98 06 00 00 	mov    ecx,DWORD PTR [esi+edx*4+0x698]
  413306:	8d 04 cd 00 00 00 00 	lea    eax,[ecx*8+0x0]
  41330d:	2b c1                	sub    eax,ecx
  41330f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  413315:	8b 91 b4 00 00 00    	mov    edx,DWORD PTR [ecx+0xb4]
  41331b:	8d 04 80             	lea    eax,[eax+eax*4]
  41331e:	c1 e0 04             	shl    eax,0x4
  413321:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  413325:	8d 44 10 04          	lea    eax,[eax+edx*1+0x4]
  413329:	50                   	push   eax
  41332a:	68 0c 94 46 00       	push   0x46940c
  41332f:	51                   	push   ecx
  413330:	e8 3e e7 03 00       	call   0x451a73
  413335:	83 c4 0c             	add    esp,0xc
  413338:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  41333c:	8b ce                	mov    ecx,esi
  41333e:	52                   	push   edx
  41333f:	e8 fc 00 00 00       	call   0x413440
  413344:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  41334a:	47                   	inc    edi
  41334b:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  413352:	2b c8                	sub    ecx,eax
  413354:	c1 e1 04             	shl    ecx,0x4
  413357:	3b bc 31 94 06 00 00 	cmp    edi,DWORD PTR [ecx+esi*1+0x694]
  41335e:	7c 93                	jl     0x4132f3
  413360:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  413366:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  41336a:	40                   	inc    eax
  41336b:	c7 84 24 50 01 00 00 	mov    DWORD PTR [esp+0x150],0xffffffff
  413372:	ff ff ff ff 
  413376:	89 86 48 06 00 00    	mov    DWORD PTR [esi+0x648],eax
  41337c:	e8 8f e3 02 00       	call   0x441710
  413381:	5f                   	pop    edi
  413382:	b0 01                	mov    al,0x1
  413384:	5e                   	pop    esi
  413385:	8b 8c 24 40 01 00 00 	mov    ecx,DWORD PTR [esp+0x140]
  41338c:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  413393:	81 c4 4c 01 00 00    	add    esp,0x14c
  413399:	c3                   	ret
  41339a:	8b 8c 24 44 01 00 00 	mov    ecx,DWORD PTR [esp+0x144]
  4133a1:	32 c0                	xor    al,al
  4133a3:	5e                   	pop    esi
  4133a4:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4133ab:	81 c4 4c 01 00 00    	add    esp,0x14c
  4133b1:	c3                   	ret
  4133b2:	90                   	nop
  4133b3:	90                   	nop
  4133b4:	90                   	nop
  4133b5:	90                   	nop
  4133b6:	90                   	nop
  4133b7:	90                   	nop
  4133b8:	90                   	nop
  4133b9:	90                   	nop
  4133ba:	90                   	nop
  4133bb:	90                   	nop
  4133bc:	90                   	nop
  4133bd:	90                   	nop
  4133be:	90                   	nop
  4133bf:	90                   	nop
  4133c0:	8b 81 64 05 00 00    	mov    eax,DWORD PTR [ecx+0x564]
  4133c6:	33 d2                	xor    edx,edx
  4133c8:	89 91 40 06 00 00    	mov    DWORD PTR [ecx+0x640],edx
  4133ce:	c7 80 80 00 00 00 01 	mov    DWORD PTR [eax+0x80],0x1
  4133d5:	00 00 00 
  4133d8:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  4133de:	89 90 88 00 00 00    	mov    DWORD PTR [eax+0x88],edx
  4133e4:	89 90 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],edx
  4133ea:	c6 81 79 05 00 00 01 	mov    BYTE PTR [ecx+0x579],0x1
  4133f1:	c7 81 44 06 00 00 01 	mov    DWORD PTR [ecx+0x644],0x1
  4133f8:	00 00 00 
  4133fb:	89 91 f8 05 00 00    	mov    DWORD PTR [ecx+0x5f8],edx
  413401:	e9 0a fc ff ff       	jmp    0x413010
  413406:	90                   	nop
  413407:	90                   	nop
  413408:	90                   	nop
  413409:	90                   	nop
  41340a:	90                   	nop
  41340b:	90                   	nop
  41340c:	90                   	nop
  41340d:	90                   	nop
  41340e:	90                   	nop
  41340f:	90                   	nop
  413410:	c7 81 40 06 00 00 00 	mov    DWORD PTR [ecx+0x640],0x0
  413417:	00 00 00 
  41341a:	c7 81 44 06 00 00 02 	mov    DWORD PTR [ecx+0x644],0x2
  413421:	00 00 00 
  413424:	c3                   	ret
  413425:	90                   	nop
  413426:	90                   	nop
  413427:	90                   	nop
  413428:	90                   	nop
  413429:	90                   	nop
  41342a:	90                   	nop
  41342b:	90                   	nop
  41342c:	90                   	nop
  41342d:	90                   	nop
  41342e:	90                   	nop
  41342f:	90                   	nop
