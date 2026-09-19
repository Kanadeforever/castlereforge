
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00413570 <.text+0x12570>:
  413570:	6a ff                	push   0xffffffff
  413572:	68 59 d6 45 00       	push   0x45d659
  413577:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41357d:	50                   	push   eax
  41357e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  413585:	83 ec 20             	sub    esp,0x20
  413588:	53                   	push   ebx
  413589:	55                   	push   ebp
  41358a:	56                   	push   esi
  41358b:	8b e9                	mov    ebp,ecx
  41358d:	57                   	push   edi
  41358e:	89 6c 24 14          	mov    DWORD PTR [esp+0x14],ebp
  413592:	e8 79 d9 01 00       	call   0x430f10
  413597:	33 db                	xor    ebx,ebx
  413599:	68 64 03 00 00       	push   0x364
  41359e:	89 5c 24 3c          	mov    DWORD PTR [esp+0x3c],ebx
  4135a2:	c7 45 00 d0 07 46 00 	mov    DWORD PTR [ebp+0x0],0x4607d0
  4135a9:	e8 b1 e0 03 00       	call   0x45165f
  4135ae:	83 c4 04             	add    esp,0x4
  4135b1:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  4135b5:	3b c3                	cmp    eax,ebx
  4135b7:	c6 44 24 38 01       	mov    BYTE PTR [esp+0x38],0x1
  4135bc:	74 0b                	je     0x4135c9
  4135be:	8b c8                	mov    ecx,eax
  4135c0:	e8 3b ad 02 00       	call   0x43e300
  4135c5:	8b f8                	mov    edi,eax
  4135c7:	eb 02                	jmp    0x4135cb
  4135c9:	33 ff                	xor    edi,edi
  4135cb:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  4135d0:	88 5c 24 38          	mov    BYTE PTR [esp+0x38],bl
  4135d4:	8b 48 68             	mov    ecx,DWORD PTR [eax+0x68]
  4135d7:	8b 50 6c             	mov    edx,DWORD PTR [eax+0x6c]
  4135da:	8b 70 28             	mov    esi,DWORD PTR [eax+0x28]
  4135dd:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  4135e1:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  4135e4:	8b 40 3c             	mov    eax,DWORD PTR [eax+0x3c]
  4135e7:	50                   	push   eax
  4135e8:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
  4135eb:	51                   	push   ecx
  4135ec:	52                   	push   edx
  4135ed:	8d 0c 48             	lea    ecx,[eax+ecx*2]
  4135f0:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  4135f3:	51                   	push   ecx
  4135f4:	8d 0c 50             	lea    ecx,[eax+edx*2]
  4135f7:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  4135fb:	51                   	push   ecx
  4135fc:	52                   	push   edx
  4135fd:	8b cf                	mov    ecx,edi
  4135ff:	e8 8c b8 02 00       	call   0x43ee90
  413604:	3b fb                	cmp    edi,ebx
  413606:	74 10                	je     0x413618
  413608:	8b cf                	mov    ecx,edi
  41360a:	e8 e1 ad 02 00       	call   0x43e3f0
  41360f:	57                   	push   edi
  413610:	e8 3b df 03 00       	call   0x451550
  413615:	83 c4 04             	add    esp,0x4
  413618:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  41361d:	53                   	push   ebx
  41361e:	68 48 85 1e 00       	push   0x1e8548
  413623:	55                   	push   ebp
  413624:	8b cd                	mov    ecx,ebp
  413626:	c6 80 48 02 00 00 01 	mov    BYTE PTR [eax+0x248],0x1
  41362d:	e8 fe 0d 02 00       	call   0x434430
  413632:	8d 8d 18 02 00 00    	lea    ecx,[ebp+0x218]
  413638:	e8 f3 16 ff ff       	call   0x404d30
  41363d:	68 94 94 46 00       	push   0x469494
  413642:	8d 8d 68 05 00 00    	lea    ecx,[ebp+0x568]
  413648:	53                   	push   ebx
  413649:	53                   	push   ebx
  41364a:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  41364e:	8d 95 6c 05 00 00    	lea    edx,[ebp+0x56c]
  413654:	53                   	push   ebx
  413655:	53                   	push   ebx
  413656:	8b cd                	mov    ecx,ebp
  413658:	89 44 24 40          	mov    DWORD PTR [esp+0x40],eax
  41365c:	89 54 24 30          	mov    DWORD PTR [esp+0x30],edx
  413660:	e8 7b da 01 00       	call   0x4310e0
  413665:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  41366a:	8b 88 d0 00 00 00    	mov    ecx,DWORD PTR [eax+0xd0]
  413670:	51                   	push   ecx
  413671:	8b 8d 64 05 00 00    	mov    ecx,DWORD PTR [ebp+0x564]
  413677:	e8 74 b0 02 00       	call   0x43e6f0
  41367c:	6a 2c                	push   0x2c
  41367e:	e8 dc df 03 00       	call   0x45165f
  413683:	83 c4 04             	add    esp,0x4
  413686:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41368a:	3b c3                	cmp    eax,ebx
  41368c:	c6 44 24 38 02       	mov    BYTE PTR [esp+0x38],0x2
  413691:	74 0e                	je     0x4136a1
  413693:	8d 54 24 18          	lea    edx,[esp+0x18]
  413697:	8b c8                	mov    ecx,eax
  413699:	52                   	push   edx
  41369a:	e8 b1 e4 01 00       	call   0x431b50
  41369f:	eb 02                	jmp    0x4136a3
  4136a1:	33 c0                	xor    eax,eax
  4136a3:	53                   	push   ebx
  4136a4:	68 80 94 46 00       	push   0x469480
  4136a9:	6a 5a                	push   0x5a
  4136ab:	68 f4 01 00 00       	push   0x1f4
  4136b0:	8b c8                	mov    ecx,eax
  4136b2:	88 5c 24 48          	mov    BYTE PTR [esp+0x48],bl
  4136b6:	89 85 84 05 00 00    	mov    DWORD PTR [ebp+0x584],eax
  4136bc:	e8 4f e5 01 00       	call   0x431c10
  4136c1:	8b 85 84 05 00 00    	mov    eax,DWORD PTR [ebp+0x584]
  4136c7:	53                   	push   ebx
  4136c8:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  4136cb:	e8 20 b0 02 00       	call   0x43e6f0
  4136d0:	bf 7e 00 00 00       	mov    edi,0x7e
  4136d5:	8d b5 88 05 00 00    	lea    esi,[ebp+0x588]
  4136db:	6a 48                	push   0x48
  4136dd:	e8 7d df 03 00       	call   0x45165f
  4136e2:	83 c4 04             	add    esp,0x4
  4136e5:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  4136e9:	3b c3                	cmp    eax,ebx
  4136eb:	c6 44 24 38 03       	mov    BYTE PTR [esp+0x38],0x3
  4136f0:	74 0e                	je     0x413700
  4136f2:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  4136f6:	51                   	push   ecx
  4136f7:	8b c8                	mov    ecx,eax
  4136f9:	e8 e2 da 01 00       	call   0x4311e0
  4136fe:	eb 02                	jmp    0x413702
  413700:	33 c0                	xor    eax,eax
  413702:	56                   	push   esi
  413703:	53                   	push   ebx
  413704:	6a 16                	push   0x16
  413706:	6a 6e                	push   0x6e
  413708:	57                   	push   edi
  413709:	68 00 02 00 00       	push   0x200
  41370e:	8b c8                	mov    ecx,eax
  413710:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  413714:	89 06                	mov    DWORD PTR [esi],eax
  413716:	e8 a5 db 01 00       	call   0x4312c0
  41371b:	83 c7 1a             	add    edi,0x1a
  41371e:	83 c6 04             	add    esi,0x4
  413721:	81 ff cc 00 00 00    	cmp    edi,0xcc
  413727:	7c b2                	jl     0x4136db
  413729:	8b 85 88 05 00 00    	mov    eax,DWORD PTR [ebp+0x588]
  41372f:	89 9d 7c 05 00 00    	mov    DWORD PTR [ebp+0x57c],ebx
  413735:	88 9d 80 05 00 00    	mov    BYTE PTR [ebp+0x580],bl
  41373b:	8b 0d 4c 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c4c
  413741:	8b 50 38             	mov    edx,DWORD PTR [eax+0x38]
  413744:	8b 40 34             	mov    eax,DWORD PTR [eax+0x34]
  413747:	83 c2 0f             	add    edx,0xf
  41374a:	83 c0 5f             	add    eax,0x5f
  41374d:	52                   	push   edx
  41374e:	50                   	push   eax
  41374f:	e8 dc a7 02 00       	call   0x43df30
  413754:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41375a:	5f                   	pop    edi
  41375b:	8b c5                	mov    eax,ebp
  41375d:	5e                   	pop    esi
  41375e:	c7 81 14 01 00 00 01 	mov    DWORD PTR [ecx+0x114],0x1
  413765:	00 00 00 
  413768:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  41376c:	5d                   	pop    ebp
  41376d:	5b                   	pop    ebx
  41376e:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  413775:	83 c4 2c             	add    esp,0x2c
  413778:	c3                   	ret
  413779:	90                   	nop
  41377a:	90                   	nop
  41377b:	90                   	nop
  41377c:	90                   	nop
  41377d:	90                   	nop
  41377e:	90                   	nop
  41377f:	90                   	nop
  413780:	6a ff                	push   0xffffffff
  413782:	68 78 d6 45 00       	push   0x45d678
  413787:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41378d:	50                   	push   eax
  41378e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  413795:	51                   	push   ecx
  413796:	53                   	push   ebx
  413797:	55                   	push   ebp
  413798:	8b e9                	mov    ebp,ecx
  41379a:	56                   	push   esi
  41379b:	57                   	push   edi
  41379c:	89 6c 24 10          	mov    DWORD PTR [esp+0x10],ebp
  4137a0:	c7 45 00 d0 07 46 00 	mov    DWORD PTR [ebp+0x0],0x4607d0
  4137a7:	c7 44 24 1c 00 00 00 	mov    DWORD PTR [esp+0x1c],0x0
  4137ae:	00 
  4137af:	8d bd 88 05 00 00    	lea    edi,[ebp+0x588]
  4137b5:	bb 03 00 00 00       	mov    ebx,0x3
  4137ba:	8b 37                	mov    esi,DWORD PTR [edi]
  4137bc:	85 f6                	test   esi,esi
  4137be:	74 16                	je     0x4137d6
  4137c0:	8b ce                	mov    ecx,esi
  4137c2:	e8 c9 da 01 00       	call   0x431290
  4137c7:	56                   	push   esi
  4137c8:	e8 83 dd 03 00       	call   0x451550
  4137cd:	83 c4 04             	add    esp,0x4
  4137d0:	c7 07 00 00 00 00    	mov    DWORD PTR [edi],0x0
  4137d6:	83 c7 04             	add    edi,0x4
  4137d9:	4b                   	dec    ebx
  4137da:	75 de                	jne    0x4137ba
  4137dc:	8b b5 84 05 00 00    	mov    esi,DWORD PTR [ebp+0x584]
  4137e2:	85 f6                	test   esi,esi
  4137e4:	74 1a                	je     0x413800
  4137e6:	8b ce                	mov    ecx,esi
  4137e8:	e8 f3 e3 01 00       	call   0x431be0
  4137ed:	56                   	push   esi
  4137ee:	e8 5d dd 03 00       	call   0x451550
  4137f3:	83 c4 04             	add    esp,0x4
  4137f6:	c7 85 84 05 00 00 00 	mov    DWORD PTR [ebp+0x584],0x0
  4137fd:	00 00 00 
  413800:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  413805:	c7 80 14 01 00 00 00 	mov    DWORD PTR [eax+0x114],0x0
  41380c:	00 00 00 
  41380f:	8b 0d 4c 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c4c
  413815:	c6 81 48 02 00 00 00 	mov    BYTE PTR [ecx+0x248],0x0
  41381c:	8b cd                	mov    ecx,ebp
  41381e:	e8 fd 0f 02 00       	call   0x434820
  413823:	8b cd                	mov    ecx,ebp
  413825:	c7 44 24 1c ff ff ff 	mov    DWORD PTR [esp+0x1c],0xffffffff
  41382c:	ff 
  41382d:	e8 0e d8 01 00       	call   0x431040
  413832:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  413836:	5f                   	pop    edi
  413837:	5e                   	pop    esi
  413838:	5d                   	pop    ebp
  413839:	5b                   	pop    ebx
  41383a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  413841:	83 c4 10             	add    esp,0x10
  413844:	c3                   	ret
  413845:	90                   	nop
  413846:	90                   	nop
  413847:	90                   	nop
  413848:	90                   	nop
  413849:	90                   	nop
  41384a:	90                   	nop
  41384b:	90                   	nop
  41384c:	90                   	nop
  41384d:	90                   	nop
  41384e:	90                   	nop
  41384f:	90                   	nop
  413850:	6a ff                	push   0xffffffff
  413852:	68 b7 d6 45 00       	push   0x45d6b7
  413857:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41385d:	50                   	push   eax
  41385e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  413865:	81 ec e4 00 00 00    	sub    esp,0xe4
  41386b:	53                   	push   ebx
  41386c:	56                   	push   esi
  41386d:	8b f1                	mov    esi,ecx
  41386f:	33 db                	xor    ebx,ebx
  413871:	88 5c 24 0b          	mov    BYTE PTR [esp+0xb],bl
  413875:	8b 86 7c 05 00 00    	mov    eax,DWORD PTR [esi+0x57c]
  41387b:	3b c3                	cmp    eax,ebx
  41387d:	a1 0c ed 8d 00       	mov    eax,ds:0x8ded0c
  413882:	74 46                	je     0x4138ca
  413884:	3b c3                	cmp    eax,ebx
  413886:	75 46                	jne    0x4138ce
  413888:	39 1d bc fc 89 00    	cmp    DWORD PTR ds:0x89fcbc,ebx
  41388e:	75 60                	jne    0x4138f0
  413890:	c6 86 79 05 00 00 01 	mov    BYTE PTR [esi+0x579],0x1
  413897:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  41389c:	c6 80 48 02 00 00 01 	mov    BYTE PTR [eax+0x248],0x1
  4138a3:	8a 86 80 05 00 00    	mov    al,BYTE PTR [esi+0x580]
  4138a9:	3a c3                	cmp    al,bl
  4138ab:	74 07                	je     0x4138b4
  4138ad:	c6 44 24 0b 01       	mov    BYTE PTR [esp+0xb],0x1
  4138b2:	eb 3c                	jmp    0x4138f0
  4138b4:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  4138ba:	83 b9 0c 01 00 00 01 	cmp    DWORD PTR [ecx+0x10c],0x1
  4138c1:	75 2d                	jne    0x4138f0
  4138c3:	c6 44 24 0b 01       	mov    BYTE PTR [esp+0xb],0x1
  4138c8:	eb 26                	jmp    0x4138f0
  4138ca:	3b c3                	cmp    eax,ebx
  4138cc:	74 22                	je     0x4138f0
  4138ce:	8b 90 54 06 00 00    	mov    edx,DWORD PTR [eax+0x654]
  4138d4:	8a 82 b9 05 00 00    	mov    al,BYTE PTR [edx+0x5b9]
  4138da:	3a c3                	cmp    al,bl
  4138dc:	88 86 80 05 00 00    	mov    BYTE PTR [esi+0x580],al
  4138e2:	74 0c                	je     0x4138f0
  4138e4:	c6 44 24 0b 01       	mov    BYTE PTR [esp+0xb],0x1
  4138e9:	c6 86 79 05 00 00 01 	mov    BYTE PTR [esi+0x579],0x1
  4138f0:	38 9e 79 05 00 00    	cmp    BYTE PTR [esi+0x579],bl
  4138f6:	0f 84 e7 01 00 00    	je     0x413ae3
  4138fc:	55                   	push   ebp
  4138fd:	57                   	push   edi
  4138fe:	33 ff                	xor    edi,edi
  413900:	8d ae 88 05 00 00    	lea    ebp,[esi+0x588]
  413906:	8b 4d 00             	mov    ecx,DWORD PTR [ebp+0x0]
  413909:	e8 02 da 01 00       	call   0x431310
  41390e:	84 c0                	test   al,al
  413910:	74 0f                	je     0x413921
  413912:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  413918:	57                   	push   edi
  413919:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41391c:	e8 cf ad 02 00       	call   0x43e6f0
  413921:	47                   	inc    edi
  413922:	83 c5 04             	add    ebp,0x4
  413925:	83 ff 03             	cmp    edi,0x3
  413928:	7c dc                	jl     0x413906
  41392a:	33 ff                	xor    edi,edi
  41392c:	8b 8c be 88 05 00 00 	mov    ecx,DWORD PTR [esi+edi*4+0x588]
  413933:	e8 48 da 01 00       	call   0x431380
  413938:	85 c0                	test   eax,eax
  41393a:	0f 84 16 01 00 00    	je     0x413a56
  413940:	3b fb                	cmp    edi,ebx
  413942:	75 60                	jne    0x4139a4
  413944:	39 1d 0c ed 8d 00    	cmp    DWORD PTR ds:0x8ded0c,ebx
  41394a:	0f 85 06 01 00 00    	jne    0x413a56
  413950:	56                   	push   esi
  413951:	8b ce                	mov    ecx,esi
  413953:	e8 58 0e 02 00       	call   0x4347b0
  413958:	68 60 06 00 00       	push   0x660
  41395d:	c7 86 7c 05 00 00 01 	mov    DWORD PTR [esi+0x57c],0x1
  413964:	00 00 00 
  413967:	e8 f3 dc 03 00       	call   0x45165f
  41396c:	83 c4 04             	add    esp,0x4
  41396f:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  413973:	3b c3                	cmp    eax,ebx
  413975:	89 9c 24 fc 00 00 00 	mov    DWORD PTR [esp+0xfc],ebx
  41397c:	74 09                	je     0x413987
  41397e:	8b c8                	mov    ecx,eax
  413980:	e8 9b 0f 02 00       	call   0x434920
  413985:	eb 02                	jmp    0x413989
  413987:	33 c0                	xor    eax,eax
  413989:	a3 0c ed 8d 00       	mov    ds:0x8ded0c,eax
  41398e:	c7 84 24 fc 00 00 00 	mov    DWORD PTR [esp+0xfc],0xffffffff
  413995:	ff ff ff ff 
  413999:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  41399f:	e9 b2 00 00 00       	jmp    0x413a56
  4139a4:	83 ff 01             	cmp    edi,0x1
  4139a7:	75 53                	jne    0x4139fc
  4139a9:	39 1d bc fc 89 00    	cmp    DWORD PTR ds:0x89fcbc,ebx
  4139af:	0f 85 a1 00 00 00    	jne    0x413a56
  4139b5:	8b ef                	mov    ebp,edi
  4139b7:	68 f0 05 00 00       	push   0x5f0
  4139bc:	89 ae 7c 05 00 00    	mov    DWORD PTR [esi+0x57c],ebp
  4139c2:	e8 98 dc 03 00       	call   0x45165f
  4139c7:	83 c4 04             	add    esp,0x4
  4139ca:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4139ce:	3b c3                	cmp    eax,ebx
  4139d0:	89 ac 24 fc 00 00 00 	mov    DWORD PTR [esp+0xfc],ebp
  4139d7:	74 09                	je     0x4139e2
  4139d9:	8b c8                	mov    ecx,eax
  4139db:	e8 f0 bc ff ff       	call   0x40f6d0
  4139e0:	eb 02                	jmp    0x4139e4
  4139e2:	33 c0                	xor    eax,eax
  4139e4:	a3 bc fc 89 00       	mov    ds:0x89fcbc,eax
  4139e9:	c7 84 24 fc 00 00 00 	mov    DWORD PTR [esp+0xfc],0xffffffff
  4139f0:	ff ff ff ff 
  4139f4:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  4139fa:	eb 5a                	jmp    0x413a56
  4139fc:	83 ff 02             	cmp    edi,0x2
  4139ff:	75 55                	jne    0x413a56
  413a01:	6a 01                	push   0x1
  413a03:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  413a07:	c6 44 24 17 01       	mov    BYTE PTR [esp+0x17],0x1
  413a0c:	e8 df dc 02 00       	call   0x4416f0
  413a11:	8d 4c 24 2c          	lea    ecx,[esp+0x2c]
  413a15:	89 bc 24 fc 00 00 00 	mov    DWORD PTR [esp+0xfc],edi
  413a1c:	51                   	push   ecx
  413a1d:	68 a8 94 46 00       	push   0x4694a8
  413a22:	b9 90 1c 8e 00       	mov    ecx,0x8e1c90
  413a27:	e8 c4 e1 02 00       	call   0x441bf0
  413a2c:	84 c0                	test   al,al
  413a2e:	74 12                	je     0x413a42
  413a30:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  413a36:	53                   	push   ebx
  413a37:	8d 54 24 30          	lea    edx,[esp+0x30]
  413a3b:	53                   	push   ebx
  413a3c:	52                   	push   edx
  413a3d:	e8 4e 18 ff ff       	call   0x405290
  413a42:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  413a46:	c7 84 24 fc 00 00 00 	mov    DWORD PTR [esp+0xfc],0xffffffff
  413a4d:	ff ff ff ff 
  413a51:	e8 ba dc 02 00       	call   0x441710
  413a56:	47                   	inc    edi
  413a57:	83 ff 03             	cmp    edi,0x3
  413a5a:	0f 8c cc fe ff ff    	jl     0x41392c
  413a60:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  413a64:	5f                   	pop    edi
  413a65:	3a c3                	cmp    al,bl
  413a67:	5d                   	pop    ebp
  413a68:	74 4b                	je     0x413ab5
  413a6a:	33 c0                	xor    eax,eax
  413a6c:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  413a72:	8b 91 a4 00 00 00    	mov    edx,DWORD PTR [ecx+0xa4]
  413a78:	8d 0c 02             	lea    ecx,[edx+eax*1]
  413a7b:	8b 54 02 2c          	mov    edx,DWORD PTR [edx+eax*1+0x2c]
  413a7f:	89 51 30             	mov    DWORD PTR [ecx+0x30],edx
  413a82:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  413a88:	8b 91 a4 00 00 00    	mov    edx,DWORD PTR [ecx+0xa4]
  413a8e:	8d 0c 02             	lea    ecx,[edx+eax*1]
  413a91:	05 50 03 00 00       	add    eax,0x350
  413a96:	3d 80 1a 00 00       	cmp    eax,0x1a80
  413a9b:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  413a9e:	89 51 38             	mov    DWORD PTR [ecx+0x38],edx
  413aa1:	7c c9                	jl     0x413a6c
  413aa3:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  413aa9:	3b f3                	cmp    esi,ebx
  413aab:	89 1d cc fc 89 00    	mov    DWORD PTR ds:0x89fccc,ebx
  413ab1:	74 30                	je     0x413ae3
  413ab3:	eb 1e                	jmp    0x413ad3
  413ab5:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  413aba:	83 b8 30 01 00 00 01 	cmp    DWORD PTR [eax+0x130],0x1
  413ac1:	75 20                	jne    0x413ae3
  413ac3:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  413ac9:	3b f3                	cmp    esi,ebx
  413acb:	89 1d cc fc 89 00    	mov    DWORD PTR ds:0x89fccc,ebx
  413ad1:	74 10                	je     0x413ae3
  413ad3:	8b ce                	mov    ecx,esi
  413ad5:	e8 a6 fc ff ff       	call   0x413780
  413ada:	56                   	push   esi
  413adb:	e8 70 da 03 00       	call   0x451550
  413ae0:	83 c4 04             	add    esp,0x4
  413ae3:	8b 8c 24 ec 00 00 00 	mov    ecx,DWORD PTR [esp+0xec]
  413aea:	5e                   	pop    esi
  413aeb:	5b                   	pop    ebx
  413aec:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  413af3:	81 c4 f0 00 00 00    	add    esp,0xf0
  413af9:	c3                   	ret
  413afa:	90                   	nop
  413afb:	90                   	nop
  413afc:	90                   	nop
  413afd:	90                   	nop
  413afe:	90                   	nop
  413aff:	90                   	nop
