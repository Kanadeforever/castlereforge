
/mnt/data/work50/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041c520 <.text+0x1b520>:
  41c520:	6a ff                	push   0xffffffff
  41c522:	68 f4 df 45 00       	push   0x45dff4
  41c527:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41c52d:	50                   	push   eax
  41c52e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41c535:	83 ec 20             	sub    esp,0x20
  41c538:	53                   	push   ebx
  41c539:	56                   	push   esi
  41c53a:	8b f1                	mov    esi,ecx
  41c53c:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  41c540:	e8 cb 49 01 00       	call   0x430f10
  41c545:	33 db                	xor    ebx,ebx
  41c547:	8b ce                	mov    ecx,esi
  41c549:	53                   	push   ebx
  41c54a:	68 48 4b 4c 00       	push   0x4c4b48
  41c54f:	56                   	push   esi
  41c550:	89 5c 24 3c          	mov    DWORD PTR [esp+0x3c],ebx
  41c554:	c7 06 b0 0a 46 00    	mov    DWORD PTR [esi],0x460ab0
  41c55a:	e8 d1 7e 01 00       	call   0x434430
  41c55f:	8d 8e 18 02 00 00    	lea    ecx,[esi+0x218]
  41c565:	e8 c6 87 fe ff       	call   0x404d30
  41c56a:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  41c56e:	8d 86 68 05 00 00    	lea    eax,[esi+0x568]
  41c574:	8d 8e 6c 05 00 00    	lea    ecx,[esi+0x56c]
  41c57a:	68 08 06 00 00       	push   0x608
  41c57f:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  41c583:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  41c587:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  41c58d:	e8 cd 50 03 00       	call   0x45165f
  41c592:	83 c4 04             	add    esp,0x4
  41c595:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  41c599:	3b c3                	cmp    eax,ebx
  41c59b:	c6 44 24 30 01       	mov    BYTE PTR [esp+0x30],0x1
  41c5a0:	74 09                	je     0x41c5ab
  41c5a2:	8b c8                	mov    ecx,eax
  41c5a4:	e8 b7 0b 00 00       	call   0x41d160
  41c5a9:	eb 02                	jmp    0x41c5ad
  41c5ab:	33 c0                	xor    eax,eax
  41c5ad:	68 98 05 00 00       	push   0x598
  41c5b2:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  41c5b6:	89 86 c0 05 00 00    	mov    DWORD PTR [esi+0x5c0],eax
  41c5bc:	e8 9e 50 03 00       	call   0x45165f
  41c5c1:	83 c4 04             	add    esp,0x4
  41c5c4:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  41c5c8:	3b c3                	cmp    eax,ebx
  41c5ca:	c6 44 24 30 02       	mov    BYTE PTR [esp+0x30],0x2
  41c5cf:	74 09                	je     0x41c5da
  41c5d1:	8b c8                	mov    ecx,eax
  41c5d3:	e8 c8 15 00 00       	call   0x41dba0
  41c5d8:	eb 02                	jmp    0x41c5dc
  41c5da:	33 c0                	xor    eax,eax
  41c5dc:	6a 18                	push   0x18
  41c5de:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  41c5e2:	89 86 c4 05 00 00    	mov    DWORD PTR [esi+0x5c4],eax
  41c5e8:	e8 72 50 03 00       	call   0x45165f
  41c5ed:	83 c4 04             	add    esp,0x4
  41c5f0:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  41c5f4:	3b c3                	cmp    eax,ebx
  41c5f6:	c6 44 24 30 03       	mov    BYTE PTR [esp+0x30],0x3
  41c5fb:	74 09                	je     0x41c606
  41c5fd:	8b c8                	mov    ecx,eax
  41c5ff:	e8 ac 56 01 00       	call   0x431cb0
  41c604:	eb 02                	jmp    0x41c608
  41c606:	33 c0                	xor    eax,eax
  41c608:	6a 48                	push   0x48
  41c60a:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  41c60e:	89 86 bc 05 00 00    	mov    DWORD PTR [esi+0x5bc],eax
  41c614:	e8 46 50 03 00       	call   0x45165f
  41c619:	83 c4 04             	add    esp,0x4
  41c61c:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  41c620:	3b c3                	cmp    eax,ebx
  41c622:	c6 44 24 30 04       	mov    BYTE PTR [esp+0x30],0x4
  41c627:	74 0e                	je     0x41c637
  41c629:	8d 54 24 10          	lea    edx,[esp+0x10]
  41c62d:	8b c8                	mov    ecx,eax
  41c62f:	52                   	push   edx
  41c630:	e8 ab 4b 01 00       	call   0x4311e0
  41c635:	eb 02                	jmp    0x41c639
  41c637:	33 c0                	xor    eax,eax
  41c639:	53                   	push   ebx
  41c63a:	53                   	push   ebx
  41c63b:	68 e0 01 00 00       	push   0x1e0
  41c640:	68 80 02 00 00       	push   0x280
  41c645:	53                   	push   ebx
  41c646:	53                   	push   ebx
  41c647:	8b c8                	mov    ecx,eax
  41c649:	88 5c 24 48          	mov    BYTE PTR [esp+0x48],bl
  41c64d:	89 86 7c 05 00 00    	mov    DWORD PTR [esi+0x57c],eax
  41c653:	e8 68 4c 01 00       	call   0x4312c0
  41c658:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  41c65c:	89 9e 80 05 00 00    	mov    DWORD PTR [esi+0x580],ebx
  41c662:	89 9e 84 05 00 00    	mov    DWORD PTR [esi+0x584],ebx
  41c668:	89 9e 88 05 00 00    	mov    DWORD PTR [esi+0x588],ebx
  41c66e:	89 9e 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],ebx
  41c674:	89 9e b8 05 00 00    	mov    DWORD PTR [esi+0x5b8],ebx
  41c67a:	8b c6                	mov    eax,esi
  41c67c:	5e                   	pop    esi
  41c67d:	5b                   	pop    ebx
  41c67e:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41c685:	83 c4 2c             	add    esp,0x2c
  41c688:	c3                   	ret
  41c689:	90                   	nop
  41c68a:	90                   	nop
  41c68b:	90                   	nop
  41c68c:	90                   	nop
  41c68d:	90                   	nop
  41c68e:	90                   	nop
  41c68f:	90                   	nop
  41c690:	6a ff                	push   0xffffffff
  41c692:	68 08 e0 45 00       	push   0x45e008
  41c697:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41c69d:	50                   	push   eax
  41c69e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41c6a5:	51                   	push   ecx
  41c6a6:	53                   	push   ebx
  41c6a7:	56                   	push   esi
  41c6a8:	8b f1                	mov    esi,ecx
  41c6aa:	57                   	push   edi
  41c6ab:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  41c6af:	c7 06 b0 0a 46 00    	mov    DWORD PTR [esi],0x460ab0
  41c6b5:	8b be c0 05 00 00    	mov    edi,DWORD PTR [esi+0x5c0]
  41c6bb:	33 db                	xor    ebx,ebx
  41c6bd:	3b fb                	cmp    edi,ebx
  41c6bf:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  41c6c3:	74 16                	je     0x41c6db
  41c6c5:	8b cf                	mov    ecx,edi
  41c6c7:	e8 54 10 00 00       	call   0x41d720
  41c6cc:	57                   	push   edi
  41c6cd:	e8 7e 4e 03 00       	call   0x451550
  41c6d2:	83 c4 04             	add    esp,0x4
  41c6d5:	89 9e c0 05 00 00    	mov    DWORD PTR [esi+0x5c0],ebx
  41c6db:	8b be c4 05 00 00    	mov    edi,DWORD PTR [esi+0x5c4]
  41c6e1:	3b fb                	cmp    edi,ebx
  41c6e3:	74 16                	je     0x41c6fb
  41c6e5:	8b cf                	mov    ecx,edi
  41c6e7:	e8 34 16 00 00       	call   0x41dd20
  41c6ec:	57                   	push   edi
  41c6ed:	e8 5e 4e 03 00       	call   0x451550
  41c6f2:	83 c4 04             	add    esp,0x4
  41c6f5:	89 9e c4 05 00 00    	mov    DWORD PTR [esi+0x5c4],ebx
  41c6fb:	8b be 7c 05 00 00    	mov    edi,DWORD PTR [esi+0x57c]
  41c701:	3b fb                	cmp    edi,ebx
  41c703:	74 16                	je     0x41c71b
  41c705:	8b cf                	mov    ecx,edi
  41c707:	e8 84 4b 01 00       	call   0x431290
  41c70c:	57                   	push   edi
  41c70d:	e8 3e 4e 03 00       	call   0x451550
  41c712:	83 c4 04             	add    esp,0x4
  41c715:	89 9e 7c 05 00 00    	mov    DWORD PTR [esi+0x57c],ebx
  41c71b:	8b be bc 05 00 00    	mov    edi,DWORD PTR [esi+0x5bc]
  41c721:	3b fb                	cmp    edi,ebx
  41c723:	74 16                	je     0x41c73b
  41c725:	8b cf                	mov    ecx,edi
  41c727:	e8 44 c9 ff ff       	call   0x419070
  41c72c:	57                   	push   edi
  41c72d:	e8 1e 4e 03 00       	call   0x451550
  41c732:	83 c4 04             	add    esp,0x4
  41c735:	89 9e bc 05 00 00    	mov    DWORD PTR [esi+0x5bc],ebx
  41c73b:	8b ce                	mov    ecx,esi
  41c73d:	c7 44 24 18 ff ff ff 	mov    DWORD PTR [esp+0x18],0xffffffff
  41c744:	ff 
  41c745:	e8 f6 48 01 00       	call   0x431040
  41c74a:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  41c74e:	5f                   	pop    edi
  41c74f:	5e                   	pop    esi
  41c750:	5b                   	pop    ebx
  41c751:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41c758:	83 c4 10             	add    esp,0x10
  41c75b:	c3                   	ret
  41c75c:	90                   	nop
  41c75d:	90                   	nop
  41c75e:	90                   	nop
  41c75f:	90                   	nop
  41c760:	56                   	push   esi
  41c761:	8b f1                	mov    esi,ecx
  41c763:	8b 86 c0 05 00 00    	mov    eax,DWORD PTR [esi+0x5c0]
  41c769:	8a 8e 79 05 00 00    	mov    cl,BYTE PTR [esi+0x579]
  41c76f:	88 88 78 05 00 00    	mov    BYTE PTR [eax+0x578],cl
  41c775:	8b 96 c4 05 00 00    	mov    edx,DWORD PTR [esi+0x5c4]
  41c77b:	8a 86 79 05 00 00    	mov    al,BYTE PTR [esi+0x579]
  41c781:	8b ce                	mov    ecx,esi
  41c783:	88 82 78 05 00 00    	mov    BYTE PTR [edx+0x578],al
  41c789:	e8 f2 01 00 00       	call   0x41c980
  41c78e:	8a 86 79 05 00 00    	mov    al,BYTE PTR [esi+0x579]
  41c794:	84 c0                	test   al,al
  41c796:	0f 84 22 01 00 00    	je     0x41c8be
  41c79c:	8b 8e 7c 05 00 00    	mov    ecx,DWORD PTR [esi+0x57c]
  41c7a2:	e8 d9 4b 01 00       	call   0x431380
  41c7a7:	83 f8 02             	cmp    eax,0x2
  41c7aa:	75 3f                	jne    0x41c7eb
  41c7ac:	8b 96 b8 05 00 00    	mov    edx,DWORD PTR [esi+0x5b8]
  41c7b2:	8b 8e 8c 05 00 00    	mov    ecx,DWORD PTR [esi+0x58c]
  41c7b8:	42                   	inc    edx
  41c7b9:	8b c2                	mov    eax,edx
  41c7bb:	89 96 b8 05 00 00    	mov    DWORD PTR [esi+0x5b8],edx
  41c7c1:	3b c1                	cmp    eax,ecx
  41c7c3:	7c 0a                	jl     0x41c7cf
  41c7c5:	c7 86 b8 05 00 00 00 	mov    DWORD PTR [esi+0x5b8],0x0
  41c7cc:	00 00 00 
  41c7cf:	8b 8e b8 05 00 00    	mov    ecx,DWORD PTR [esi+0x5b8]
  41c7d5:	8b 94 8e 90 05 00 00 	mov    edx,DWORD PTR [esi+ecx*4+0x590]
  41c7dc:	8b ce                	mov    ecx,esi
  41c7de:	89 96 88 05 00 00    	mov    DWORD PTR [esi+0x588],edx
  41c7e4:	e8 47 02 00 00       	call   0x41ca30
  41c7e9:	eb 0c                	jmp    0x41c7f7
  41c7eb:	83 f8 01             	cmp    eax,0x1
  41c7ee:	75 07                	jne    0x41c7f7
  41c7f0:	8b ce                	mov    ecx,esi
  41c7f2:	e8 69 01 00 00       	call   0x41c960
  41c7f7:	8b 86 b8 05 00 00    	mov    eax,DWORD PTR [esi+0x5b8]
  41c7fd:	8b 8e c0 05 00 00    	mov    ecx,DWORD PTR [esi+0x5c0]
  41c803:	53                   	push   ebx
  41c804:	57                   	push   edi
  41c805:	8b 3c 85 04 fd 89 00 	mov    edi,DWORD PTR [eax*4+0x89fd04]
  41c80c:	8b 91 dc 05 00 00    	mov    edx,DWORD PTR [ecx+0x5dc]
  41c812:	8b 47 2c             	mov    eax,DWORD PTR [edi+0x2c]
  41c815:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  41c818:	8b 8e c0 05 00 00    	mov    ecx,DWORD PTR [esi+0x5c0]
  41c81e:	8b 47 30             	mov    eax,DWORD PTR [edi+0x30]
  41c821:	8b 91 d8 05 00 00    	mov    edx,DWORD PTR [ecx+0x5d8]
  41c827:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  41c82a:	8b 8e c0 05 00 00    	mov    ecx,DWORD PTR [esi+0x5c0]
  41c830:	8b 47 34             	mov    eax,DWORD PTR [edi+0x34]
  41c833:	8b 91 e4 05 00 00    	mov    edx,DWORD PTR [ecx+0x5e4]
  41c839:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  41c83c:	8b 8e c0 05 00 00    	mov    ecx,DWORD PTR [esi+0x5c0]
  41c842:	8b 47 38             	mov    eax,DWORD PTR [edi+0x38]
  41c845:	8b 91 e0 05 00 00    	mov    edx,DWORD PTR [ecx+0x5e0]
  41c84b:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  41c84e:	8b 5f 2c             	mov    ebx,DWORD PTR [edi+0x2c]
  41c851:	85 db                	test   ebx,ebx
  41c853:	75 04                	jne    0x41c859
  41c855:	33 c0                	xor    eax,eax
  41c857:	eb 10                	jmp    0x41c869
  41c859:	8b 4f 30             	mov    ecx,DWORD PTR [edi+0x30]
  41c85c:	8b c1                	mov    eax,ecx
  41c85e:	c1 e0 04             	shl    eax,0x4
  41c861:	03 c1                	add    eax,ecx
  41c863:	c1 e0 02             	shl    eax,0x2
  41c866:	99                   	cdq
  41c867:	f7 fb                	idiv   ebx
  41c869:	8b 96 c0 05 00 00    	mov    edx,DWORD PTR [esi+0x5c0]
  41c86f:	b9 44 00 00 00       	mov    ecx,0x44
  41c874:	2b c8                	sub    ecx,eax
  41c876:	8b 82 90 05 00 00    	mov    eax,DWORD PTR [edx+0x590]
  41c87c:	51                   	push   ecx
  41c87d:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41c880:	e8 6b 1e 02 00       	call   0x43e6f0
  41c885:	8b 4f 34             	mov    ecx,DWORD PTR [edi+0x34]
  41c888:	85 c9                	test   ecx,ecx
  41c88a:	75 04                	jne    0x41c890
  41c88c:	33 c0                	xor    eax,eax
  41c88e:	eb 10                	jmp    0x41c8a0
  41c890:	8b 7f 38             	mov    edi,DWORD PTR [edi+0x38]
  41c893:	8b c7                	mov    eax,edi
  41c895:	c1 e0 04             	shl    eax,0x4
  41c898:	03 c7                	add    eax,edi
  41c89a:	c1 e0 02             	shl    eax,0x2
  41c89d:	99                   	cdq
  41c89e:	f7 f9                	idiv   ecx
  41c8a0:	8b 96 c0 05 00 00    	mov    edx,DWORD PTR [esi+0x5c0]
  41c8a6:	b9 44 00 00 00       	mov    ecx,0x44
  41c8ab:	2b c8                	sub    ecx,eax
  41c8ad:	8b 82 94 05 00 00    	mov    eax,DWORD PTR [edx+0x594]
  41c8b3:	51                   	push   ecx
  41c8b4:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41c8b7:	e8 34 1e 02 00       	call   0x43e6f0
  41c8bc:	5f                   	pop    edi
  41c8bd:	5b                   	pop    ebx
  41c8be:	5e                   	pop    esi
  41c8bf:	c3                   	ret
  41c8c0:	a1 d8 fc 89 00       	mov    eax,ds:0x89fcd8
  41c8c5:	56                   	push   esi
  41c8c6:	33 f6                	xor    esi,esi
  41c8c8:	57                   	push   edi
  41c8c9:	85 c0                	test   eax,eax
  41c8cb:	89 81 8c 05 00 00    	mov    DWORD PTR [ecx+0x58c],eax
  41c8d1:	7e 20                	jle    0x41c8f3
  41c8d3:	b8 dc fc 89 00       	mov    eax,0x89fcdc
  41c8d8:	8d 91 90 05 00 00    	lea    edx,[ecx+0x590]
  41c8de:	8b 38                	mov    edi,DWORD PTR [eax]
  41c8e0:	46                   	inc    esi
  41c8e1:	89 3a                	mov    DWORD PTR [edx],edi
  41c8e3:	8b b9 8c 05 00 00    	mov    edi,DWORD PTR [ecx+0x58c]
  41c8e9:	83 c0 04             	add    eax,0x4
  41c8ec:	83 c2 04             	add    edx,0x4
  41c8ef:	3b f7                	cmp    esi,edi
  41c8f1:	7c eb                	jl     0x41c8de
  41c8f3:	8b b1 8c 05 00 00    	mov    esi,DWORD PTR [ecx+0x58c]
  41c8f9:	33 c0                	xor    eax,eax
  41c8fb:	85 f6                	test   esi,esi
  41c8fd:	c7 81 b8 05 00 00 ff 	mov    DWORD PTR [ecx+0x5b8],0xffffffff
  41c904:	ff ff ff 
  41c907:	7e 1e                	jle    0x41c927
  41c909:	8b b9 88 05 00 00    	mov    edi,DWORD PTR [ecx+0x588]
  41c90f:	8d 91 90 05 00 00    	lea    edx,[ecx+0x590]
  41c915:	39 3a                	cmp    DWORD PTR [edx],edi
  41c917:	75 06                	jne    0x41c91f
  41c919:	89 81 b8 05 00 00    	mov    DWORD PTR [ecx+0x5b8],eax
  41c91f:	40                   	inc    eax
  41c920:	83 c2 04             	add    edx,0x4
  41c923:	3b c6                	cmp    eax,esi
  41c925:	7c ee                	jl     0x41c915
  41c927:	8b 81 b8 05 00 00    	mov    eax,DWORD PTR [ecx+0x5b8]
  41c92d:	5f                   	pop    edi
  41c92e:	85 c0                	test   eax,eax
  41c930:	5e                   	pop    esi
  41c931:	7d 0a                	jge    0x41c93d
  41c933:	c7 81 b8 05 00 00 00 	mov    DWORD PTR [ecx+0x5b8],0x0
  41c93a:	00 00 00 
  41c93d:	b8 01 00 00 00       	mov    eax,0x1
  41c942:	c7 81 84 05 00 00 00 	mov    DWORD PTR [ecx+0x584],0x0
  41c949:	00 00 00 
  41c94c:	88 81 79 05 00 00    	mov    BYTE PTR [ecx+0x579],al
  41c952:	89 81 80 05 00 00    	mov    DWORD PTR [ecx+0x580],eax
  41c958:	e9 d3 00 00 00       	jmp    0x41ca30
  41c95d:	90                   	nop
  41c95e:	90                   	nop
  41c95f:	90                   	nop
  41c960:	c7 81 80 05 00 00 02 	mov    DWORD PTR [ecx+0x580],0x2
  41c967:	00 00 00 
  41c96a:	c7 81 84 05 00 00 00 	mov    DWORD PTR [ecx+0x584],0x0
  41c971:	00 00 00 
  41c974:	c3                   	ret
  41c975:	90                   	nop
  41c976:	90                   	nop
  41c977:	90                   	nop
  41c978:	90                   	nop
  41c979:	90                   	nop
  41c97a:	90                   	nop
  41c97b:	90                   	nop
  41c97c:	90                   	nop
  41c97d:	90                   	nop
  41c97e:	90                   	nop
  41c97f:	90                   	nop
  41c980:	56                   	push   esi
  41c981:	8b f1                	mov    esi,ecx
  41c983:	8b 86 80 05 00 00    	mov    eax,DWORD PTR [esi+0x580]
  41c989:	85 c0                	test   eax,eax
  41c98b:	75 04                	jne    0x41c991
  41c98d:	32 c0                	xor    al,al
  41c98f:	5e                   	pop    esi
  41c990:	c3                   	ret
  41c991:	8b 8e bc 05 00 00    	mov    ecx,DWORD PTR [esi+0x5bc]
  41c997:	50                   	push   eax
  41c998:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  41c99e:	50                   	push   eax
  41c99f:	6a 1a                	push   0x1a
  41c9a1:	68 0c fe ff ff       	push   0xfffffe0c
  41c9a6:	e8 25 53 01 00       	call   0x431cd0
  41c9ab:	8b 8e c0 05 00 00    	mov    ecx,DWORD PTR [esi+0x5c0]
  41c9b1:	89 81 68 05 00 00    	mov    DWORD PTR [ecx+0x568],eax
  41c9b7:	8b 96 80 05 00 00    	mov    edx,DWORD PTR [esi+0x580]
  41c9bd:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  41c9c3:	8b 8e bc 05 00 00    	mov    ecx,DWORD PTR [esi+0x5bc]
  41c9c9:	52                   	push   edx
  41c9ca:	50                   	push   eax
  41c9cb:	68 1b 01 00 00       	push   0x11b
  41c9d0:	68 bc 02 00 00       	push   0x2bc
  41c9d5:	e8 f6 52 01 00       	call   0x431cd0
  41c9da:	8b 8e c4 05 00 00    	mov    ecx,DWORD PTR [esi+0x5c4]
  41c9e0:	89 81 68 05 00 00    	mov    DWORD PTR [ecx+0x568],eax
  41c9e6:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  41c9ec:	83 f8 08             	cmp    eax,0x8
  41c9ef:	7e 29                	jle    0x41ca1a
  41c9f1:	83 be 80 05 00 00 02 	cmp    DWORD PTR [esi+0x580],0x2
  41c9f8:	75 07                	jne    0x41ca01
  41c9fa:	c6 86 79 05 00 00 00 	mov    BYTE PTR [esi+0x579],0x0
  41ca01:	8b 8e 7c 05 00 00    	mov    ecx,DWORD PTR [esi+0x57c]
  41ca07:	c7 86 80 05 00 00 00 	mov    DWORD PTR [esi+0x580],0x0
  41ca0e:	00 00 00 
  41ca11:	e8 6a 49 01 00       	call   0x431380
  41ca16:	b0 01                	mov    al,0x1
  41ca18:	5e                   	pop    esi
  41ca19:	c3                   	ret
  41ca1a:	8b 8e 7c 05 00 00    	mov    ecx,DWORD PTR [esi+0x57c]
  41ca20:	40                   	inc    eax
  41ca21:	89 86 84 05 00 00    	mov    DWORD PTR [esi+0x584],eax
  41ca27:	e8 54 49 01 00       	call   0x431380
  41ca2c:	b0 01                	mov    al,0x1
  41ca2e:	5e                   	pop    esi
  41ca2f:	c3                   	ret
  41ca30:	83 ec 1c             	sub    esp,0x1c
  41ca33:	53                   	push   ebx
  41ca34:	55                   	push   ebp
  41ca35:	56                   	push   esi
  41ca36:	57                   	push   edi
  41ca37:	8b f9                	mov    edi,ecx
  41ca39:	8b 87 b8 05 00 00    	mov    eax,DWORD PTR [edi+0x5b8]
  41ca3f:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41ca45:	8b 8f 88 05 00 00    	mov    ecx,DWORD PTR [edi+0x588]
  41ca4b:	8b 34 85 04 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd04]
  41ca52:	8b 82 88 05 00 00    	mov    eax,DWORD PTR [edx+0x588]
  41ca58:	49                   	dec    ecx
  41ca59:	51                   	push   ecx
  41ca5a:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41ca5d:	e8 8e 1c 02 00       	call   0x43e6f0
  41ca62:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41ca68:	8b 8f 88 05 00 00    	mov    ecx,DWORD PTR [edi+0x588]
  41ca6e:	49                   	dec    ecx
  41ca6f:	8b 82 8c 05 00 00    	mov    eax,DWORD PTR [edx+0x58c]
  41ca75:	51                   	push   ecx
  41ca76:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41ca79:	e8 72 1c 02 00       	call   0x43e6f0
  41ca7e:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41ca84:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  41ca87:	33 ed                	xor    ebp,ebp
  41ca89:	8b 91 d0 05 00 00    	mov    edx,DWORD PTR [ecx+0x5d0]
  41ca8f:	89 42 44             	mov    DWORD PTR [edx+0x44],eax
  41ca92:	8b 4e 28             	mov    ecx,DWORD PTR [esi+0x28]
  41ca95:	8b 56 24             	mov    edx,DWORD PTR [esi+0x24]
  41ca98:	2b ca                	sub    ecx,edx
  41ca9a:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41caa0:	8b 82 d4 05 00 00    	mov    eax,DWORD PTR [edx+0x5d4]
  41caa6:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  41caa9:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41caaf:	8b 81 d4 05 00 00    	mov    eax,DWORD PTR [ecx+0x5d4]
  41cab5:	39 68 44             	cmp    DWORD PTR [eax+0x44],ebp
  41cab8:	7d 03                	jge    0x41cabd
  41caba:	89 68 44             	mov    DWORD PTR [eax+0x44],ebp
  41cabd:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41cac3:	6a ff                	push   0xffffffff
  41cac5:	6a ff                	push   0xffffffff
  41cac7:	6a ff                	push   0xffffffff
  41cac9:	55                   	push   ebp
  41caca:	56                   	push   esi
  41cacb:	e8 50 f1 01 00       	call   0x43bc20
  41cad0:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cad6:	6a ff                	push   0xffffffff
  41cad8:	6a ff                	push   0xffffffff
  41cada:	6a ff                	push   0xffffffff
  41cadc:	8b 8a e8 05 00 00    	mov    ecx,DWORD PTR [edx+0x5e8]
  41cae2:	6a 01                	push   0x1
  41cae4:	56                   	push   esi
  41cae5:	89 41 44             	mov    DWORD PTR [ecx+0x44],eax
  41cae8:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41caee:	e8 2d f1 01 00       	call   0x43bc20
  41caf3:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41caf9:	6a ff                	push   0xffffffff
  41cafb:	6a ff                	push   0xffffffff
  41cafd:	6a ff                	push   0xffffffff
  41caff:	8b 8a ec 05 00 00    	mov    ecx,DWORD PTR [edx+0x5ec]
  41cb05:	6a 02                	push   0x2
  41cb07:	56                   	push   esi
  41cb08:	89 41 44             	mov    DWORD PTR [ecx+0x44],eax
  41cb0b:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41cb11:	e8 0a f1 01 00       	call   0x43bc20
  41cb16:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cb1c:	6a ff                	push   0xffffffff
  41cb1e:	6a ff                	push   0xffffffff
  41cb20:	6a ff                	push   0xffffffff
  41cb22:	8b 8a f0 05 00 00    	mov    ecx,DWORD PTR [edx+0x5f0]
  41cb28:	6a 03                	push   0x3
  41cb2a:	56                   	push   esi
  41cb2b:	89 41 44             	mov    DWORD PTR [ecx+0x44],eax
  41cb2e:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41cb34:	e8 e7 f0 01 00       	call   0x43bc20
  41cb39:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cb3f:	6a ff                	push   0xffffffff
  41cb41:	6a ff                	push   0xffffffff
  41cb43:	6a ff                	push   0xffffffff
  41cb45:	8b 8a f4 05 00 00    	mov    ecx,DWORD PTR [edx+0x5f4]
  41cb4b:	6a 04                	push   0x4
  41cb4d:	56                   	push   esi
  41cb4e:	89 41 44             	mov    DWORD PTR [ecx+0x44],eax
  41cb51:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41cb57:	e8 c4 f0 01 00       	call   0x43bc20
  41cb5c:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cb62:	6a ff                	push   0xffffffff
  41cb64:	6a ff                	push   0xffffffff
  41cb66:	6a ff                	push   0xffffffff
  41cb68:	8b 8a f8 05 00 00    	mov    ecx,DWORD PTR [edx+0x5f8]
  41cb6e:	6a 05                	push   0x5
  41cb70:	56                   	push   esi
  41cb71:	89 41 44             	mov    DWORD PTR [ecx+0x44],eax
  41cb74:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41cb7a:	e8 a1 f0 01 00       	call   0x43bc20
  41cb7f:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cb85:	6a ff                	push   0xffffffff
  41cb87:	6a ff                	push   0xffffffff
  41cb89:	6a ff                	push   0xffffffff
  41cb8b:	8b 8a fc 05 00 00    	mov    ecx,DWORD PTR [edx+0x5fc]
  41cb91:	6a 06                	push   0x6
  41cb93:	56                   	push   esi
  41cb94:	89 41 44             	mov    DWORD PTR [ecx+0x44],eax
  41cb97:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41cb9d:	e8 7e f0 01 00       	call   0x43bc20
  41cba2:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cba8:	6a ff                	push   0xffffffff
  41cbaa:	6a ff                	push   0xffffffff
  41cbac:	6a ff                	push   0xffffffff
  41cbae:	8b 8a 00 06 00 00    	mov    ecx,DWORD PTR [edx+0x600]
  41cbb4:	6a 07                	push   0x7
  41cbb6:	56                   	push   esi
  41cbb7:	89 41 44             	mov    DWORD PTR [ecx+0x44],eax
  41cbba:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41cbc0:	e8 5b f0 01 00       	call   0x43bc20
  41cbc5:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cbcb:	8b 8a 04 06 00 00    	mov    ecx,DWORD PTR [edx+0x604]
  41cbd1:	89 41 44             	mov    DWORD PTR [ecx+0x44],eax
  41cbd4:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  41cbda:	8b 82 ac 00 00 00    	mov    eax,DWORD PTR [edx+0xac]
  41cbe0:	8b 96 94 00 00 00    	mov    edx,DWORD PTR [esi+0x94]
  41cbe6:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41cbea:	8b 86 98 00 00 00    	mov    eax,DWORD PTR [esi+0x98]
  41cbf0:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  41cbf4:	8b 46 40             	mov    eax,DWORD PTR [esi+0x40]
  41cbf7:	89 54 24 24          	mov    DWORD PTR [esp+0x24],edx
  41cbfb:	8b 8e 90 00 00 00    	mov    ecx,DWORD PTR [esi+0x90]
  41cc01:	99                   	cdq
  41cc02:	2b c2                	sub    eax,edx
  41cc04:	89 4c 24 20          	mov    DWORD PTR [esp+0x20],ecx
  41cc08:	8b d8                	mov    ebx,eax
  41cc0a:	8b 46 4c             	mov    eax,DWORD PTR [esi+0x4c]
  41cc0d:	8b 4e 3c             	mov    ecx,DWORD PTR [esi+0x3c]
  41cc10:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  41cc14:	99                   	cdq
  41cc15:	83 e2 03             	and    edx,0x3
  41cc18:	89 4c 24 1c          	mov    DWORD PTR [esp+0x1c],ecx
  41cc1c:	03 c2                	add    eax,edx
  41cc1e:	8d 54 24 20          	lea    edx,[esp+0x20]
  41cc22:	8b e8                	mov    ebp,eax
  41cc24:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  41cc28:	d1 fb                	sar    ebx,1
  41cc2a:	c1 fd 02             	sar    ebp,0x2
  41cc2d:	c7 44 24 1c 03 00 00 	mov    DWORD PTR [esp+0x1c],0x3
  41cc34:	00 
  41cc35:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  41cc39:	8b 00                	mov    eax,DWORD PTR [eax]
  41cc3b:	85 c0                	test   eax,eax
  41cc3d:	7e 2e                	jle    0x41cc6d
  41cc3f:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  41cc46:	2b d0                	sub    edx,eax
  41cc48:	8d 14 90             	lea    edx,[eax+edx*4]
  41cc4b:	c1 e2 04             	shl    edx,0x4
  41cc4e:	2b d0                	sub    edx,eax
  41cc50:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41cc54:	8d 04 50             	lea    eax,[eax+edx*2]
  41cc57:	8b 50 38             	mov    edx,DWORD PTR [eax+0x38]
  41cc5a:	03 ca                	add    ecx,edx
  41cc5c:	8b 50 3c             	mov    edx,DWORD PTR [eax+0x3c]
  41cc5f:	03 da                	add    ebx,edx
  41cc61:	8b 50 40             	mov    edx,DWORD PTR [eax+0x40]
  41cc64:	01 54 24 18          	add    DWORD PTR [esp+0x18],edx
  41cc68:	8b 50 44             	mov    edx,DWORD PTR [eax+0x44]
  41cc6b:	03 ea                	add    ebp,edx
  41cc6d:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  41cc71:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  41cc75:	83 c2 04             	add    edx,0x4
  41cc78:	48                   	dec    eax
  41cc79:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  41cc7d:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  41cc81:	75 b2                	jne    0x41cc35
  41cc83:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  41cc87:	89 4c 24 1c          	mov    DWORD PTR [esp+0x1c],ecx
  41cc8b:	85 c0                	test   eax,eax
  41cc8d:	7e 1a                	jle    0x41cca9
  41cc8f:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  41cc96:	2b c8                	sub    ecx,eax
  41cc98:	8d 14 88             	lea    edx,[eax+ecx*4]
  41cc9b:	c1 e2 04             	shl    edx,0x4
  41cc9e:	2b d0                	sub    edx,eax
  41cca0:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41cca4:	8d 04 50             	lea    eax,[eax+edx*2]
  41cca7:	eb 05                	jmp    0x41ccae
  41cca9:	b8 bc 92 46 00       	mov    eax,0x4692bc
  41ccae:	8b 8f c4 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c4]
  41ccb4:	50                   	push   eax
  41ccb5:	8b 89 7c 05 00 00    	mov    ecx,DWORD PTR [ecx+0x57c]
  41ccbb:	e8 60 4d 01 00       	call   0x431a20
  41ccc0:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  41ccc4:	85 c0                	test   eax,eax
  41ccc6:	7e 1a                	jle    0x41cce2
  41ccc8:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  41cccf:	2b d0                	sub    edx,eax
  41ccd1:	8d 0c 90             	lea    ecx,[eax+edx*4]
  41ccd4:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  41ccd8:	c1 e1 04             	shl    ecx,0x4
  41ccdb:	2b c8                	sub    ecx,eax
  41ccdd:	8d 04 4a             	lea    eax,[edx+ecx*2]
  41cce0:	eb 05                	jmp    0x41cce7
  41cce2:	b8 bc 92 46 00       	mov    eax,0x4692bc
  41cce7:	50                   	push   eax
  41cce8:	8b 87 c4 05 00 00    	mov    eax,DWORD PTR [edi+0x5c4]
  41ccee:	8b 88 80 05 00 00    	mov    ecx,DWORD PTR [eax+0x580]
  41ccf4:	e8 27 4d 01 00       	call   0x431a20
  41ccf9:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  41ccfd:	85 c0                	test   eax,eax
  41ccff:	7e 1a                	jle    0x41cd1b
  41cd01:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  41cd08:	2b c8                	sub    ecx,eax
  41cd0a:	8d 14 88             	lea    edx,[eax+ecx*4]
  41cd0d:	c1 e2 04             	shl    edx,0x4
  41cd10:	2b d0                	sub    edx,eax
  41cd12:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41cd16:	8d 04 50             	lea    eax,[eax+edx*2]
  41cd19:	eb 05                	jmp    0x41cd20
  41cd1b:	b8 bc 92 46 00       	mov    eax,0x4692bc
  41cd20:	8b 8f c4 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c4]
  41cd26:	50                   	push   eax
  41cd27:	8b 89 84 05 00 00    	mov    ecx,DWORD PTR [ecx+0x584]
  41cd2d:	e8 ee 4c 01 00       	call   0x431a20
  41cd32:	8b 97 c4 05 00 00    	mov    edx,DWORD PTR [edi+0x5c4]
  41cd38:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  41cd3c:	8b 82 88 05 00 00    	mov    eax,DWORD PTR [edx+0x588]
  41cd42:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  41cd45:	8b 97 c4 05 00 00    	mov    edx,DWORD PTR [edi+0x5c4]
  41cd4b:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  41cd4f:	8b 82 8c 05 00 00    	mov    eax,DWORD PTR [edx+0x58c]
  41cd55:	83 c1 64             	add    ecx,0x64
  41cd58:	89 58 44             	mov    DWORD PTR [eax+0x44],ebx
  41cd5b:	8b 97 c4 05 00 00    	mov    edx,DWORD PTR [edi+0x5c4]
  41cd61:	8b 82 90 05 00 00    	mov    eax,DWORD PTR [edx+0x590]
  41cd67:	89 48 44             	mov    DWORD PTR [eax+0x44],ecx
  41cd6a:	8b 8f c4 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c4]
  41cd70:	8b 91 94 05 00 00    	mov    edx,DWORD PTR [ecx+0x594]
  41cd76:	89 6a 44             	mov    DWORD PTR [edx+0x44],ebp
  41cd79:	8b 86 e8 01 00 00    	mov    eax,DWORD PTR [esi+0x1e8]
  41cd7f:	85 c0                	test   eax,eax
  41cd81:	7e 13                	jle    0x41cd96
  41cd83:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cd89:	6a 01                	push   0x1
  41cd8b:	8b 88 98 05 00 00    	mov    ecx,DWORD PTR [eax+0x598]
  41cd91:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cd94:	eb 2e                	jmp    0x41cdc4
  41cd96:	8b 86 3c 02 00 00    	mov    eax,DWORD PTR [esi+0x23c]
  41cd9c:	85 c0                	test   eax,eax
  41cd9e:	7e 13                	jle    0x41cdb3
  41cda0:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cda6:	6a 0c                	push   0xc
  41cda8:	8b 82 98 05 00 00    	mov    eax,DWORD PTR [edx+0x598]
  41cdae:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cdb1:	eb 11                	jmp    0x41cdc4
  41cdb3:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cdb9:	6a 00                	push   0x0
  41cdbb:	8b 91 98 05 00 00    	mov    edx,DWORD PTR [ecx+0x598]
  41cdc1:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cdc4:	e8 27 19 02 00       	call   0x43e6f0
  41cdc9:	8b 86 fc 01 00 00    	mov    eax,DWORD PTR [esi+0x1fc]
  41cdcf:	85 c0                	test   eax,eax
  41cdd1:	7e 13                	jle    0x41cde6
  41cdd3:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cdd9:	6a 02                	push   0x2
  41cddb:	8b 88 9c 05 00 00    	mov    ecx,DWORD PTR [eax+0x59c]
  41cde1:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cde4:	eb 2e                	jmp    0x41ce14
  41cde6:	8b 86 48 02 00 00    	mov    eax,DWORD PTR [esi+0x248]
  41cdec:	85 c0                	test   eax,eax
  41cdee:	7e 13                	jle    0x41ce03
  41cdf0:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cdf6:	6a 03                	push   0x3
  41cdf8:	8b 82 9c 05 00 00    	mov    eax,DWORD PTR [edx+0x59c]
  41cdfe:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41ce01:	eb 11                	jmp    0x41ce14
  41ce03:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41ce09:	6a 00                	push   0x0
  41ce0b:	8b 91 9c 05 00 00    	mov    edx,DWORD PTR [ecx+0x59c]
  41ce11:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41ce14:	e8 d7 18 02 00       	call   0x43e6f0
  41ce19:	8b 86 f4 01 00 00    	mov    eax,DWORD PTR [esi+0x1f4]
  41ce1f:	85 c0                	test   eax,eax
  41ce21:	7e 13                	jle    0x41ce36
  41ce23:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41ce29:	6a 07                	push   0x7
  41ce2b:	8b 88 a0 05 00 00    	mov    ecx,DWORD PTR [eax+0x5a0]
  41ce31:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41ce34:	eb 2e                	jmp    0x41ce64
  41ce36:	8b 86 40 02 00 00    	mov    eax,DWORD PTR [esi+0x240]
  41ce3c:	85 c0                	test   eax,eax
  41ce3e:	7e 13                	jle    0x41ce53
  41ce40:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41ce46:	6a 06                	push   0x6
  41ce48:	8b 82 a0 05 00 00    	mov    eax,DWORD PTR [edx+0x5a0]
  41ce4e:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41ce51:	eb 11                	jmp    0x41ce64
  41ce53:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41ce59:	6a 00                	push   0x0
  41ce5b:	8b 91 a0 05 00 00    	mov    edx,DWORD PTR [ecx+0x5a0]
  41ce61:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41ce64:	e8 87 18 02 00       	call   0x43e6f0
  41ce69:	8b 86 f8 01 00 00    	mov    eax,DWORD PTR [esi+0x1f8]
  41ce6f:	85 c0                	test   eax,eax
  41ce71:	7e 13                	jle    0x41ce86
  41ce73:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41ce79:	6a 11                	push   0x11
  41ce7b:	8b 88 a4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5a4]
  41ce81:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41ce84:	eb 2e                	jmp    0x41ceb4
  41ce86:	8b 86 44 02 00 00    	mov    eax,DWORD PTR [esi+0x244]
  41ce8c:	85 c0                	test   eax,eax
  41ce8e:	7e 13                	jle    0x41cea3
  41ce90:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41ce96:	6a 04                	push   0x4
  41ce98:	8b 82 a4 05 00 00    	mov    eax,DWORD PTR [edx+0x5a4]
  41ce9e:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cea1:	eb 11                	jmp    0x41ceb4
  41cea3:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cea9:	6a 00                	push   0x0
  41ceab:	8b 91 a4 05 00 00    	mov    edx,DWORD PTR [ecx+0x5a4]
  41ceb1:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41ceb4:	e8 37 18 02 00       	call   0x43e6f0
  41ceb9:	8b 86 00 02 00 00    	mov    eax,DWORD PTR [esi+0x200]
  41cebf:	85 c0                	test   eax,eax
  41cec1:	7e 13                	jle    0x41ced6
  41cec3:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cec9:	6a 0e                	push   0xe
  41cecb:	8b 88 a8 05 00 00    	mov    ecx,DWORD PTR [eax+0x5a8]
  41ced1:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41ced4:	eb 2e                	jmp    0x41cf04
  41ced6:	8b 86 4c 02 00 00    	mov    eax,DWORD PTR [esi+0x24c]
  41cedc:	85 c0                	test   eax,eax
  41cede:	7e 13                	jle    0x41cef3
  41cee0:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cee6:	6a 0d                	push   0xd
  41cee8:	8b 82 a8 05 00 00    	mov    eax,DWORD PTR [edx+0x5a8]
  41ceee:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cef1:	eb 11                	jmp    0x41cf04
  41cef3:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cef9:	6a 00                	push   0x0
  41cefb:	8b 91 a8 05 00 00    	mov    edx,DWORD PTR [ecx+0x5a8]
  41cf01:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cf04:	e8 e7 17 02 00       	call   0x43e6f0
  41cf09:	8b 86 04 02 00 00    	mov    eax,DWORD PTR [esi+0x204]
  41cf0f:	85 c0                	test   eax,eax
  41cf11:	7e 13                	jle    0x41cf26
  41cf13:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cf19:	6a 0a                	push   0xa
  41cf1b:	8b 88 ac 05 00 00    	mov    ecx,DWORD PTR [eax+0x5ac]
  41cf21:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cf24:	eb 2e                	jmp    0x41cf54
  41cf26:	8b 86 50 02 00 00    	mov    eax,DWORD PTR [esi+0x250]
  41cf2c:	85 c0                	test   eax,eax
  41cf2e:	7e 13                	jle    0x41cf43
  41cf30:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cf36:	6a 0b                	push   0xb
  41cf38:	8b 82 ac 05 00 00    	mov    eax,DWORD PTR [edx+0x5ac]
  41cf3e:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cf41:	eb 11                	jmp    0x41cf54
  41cf43:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cf49:	6a 00                	push   0x0
  41cf4b:	8b 91 ac 05 00 00    	mov    edx,DWORD PTR [ecx+0x5ac]
  41cf51:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cf54:	e8 97 17 02 00       	call   0x43e6f0
  41cf59:	8b 86 ec 01 00 00    	mov    eax,DWORD PTR [esi+0x1ec]
  41cf5f:	85 c0                	test   eax,eax
  41cf61:	7e 13                	jle    0x41cf76
  41cf63:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cf69:	6a 05                	push   0x5
  41cf6b:	8b 88 b0 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b0]
  41cf71:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cf74:	eb 11                	jmp    0x41cf87
  41cf76:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cf7c:	6a 00                	push   0x0
  41cf7e:	8b 82 b0 05 00 00    	mov    eax,DWORD PTR [edx+0x5b0]
  41cf84:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cf87:	e8 64 17 02 00       	call   0x43e6f0
  41cf8c:	8b 86 f0 01 00 00    	mov    eax,DWORD PTR [esi+0x1f0]
  41cf92:	85 c0                	test   eax,eax
  41cf94:	7e 13                	jle    0x41cfa9
  41cf96:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cf9c:	6a 12                	push   0x12
  41cf9e:	8b 91 b4 05 00 00    	mov    edx,DWORD PTR [ecx+0x5b4]
  41cfa4:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cfa7:	eb 11                	jmp    0x41cfba
  41cfa9:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cfaf:	6a 00                	push   0x0
  41cfb1:	8b 88 b4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b4]
  41cfb7:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cfba:	e8 31 17 02 00       	call   0x43e6f0
  41cfbf:	8b 86 24 02 00 00    	mov    eax,DWORD PTR [esi+0x224]
  41cfc5:	85 c0                	test   eax,eax
  41cfc7:	7e 13                	jle    0x41cfdc
  41cfc9:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cfcf:	6a 08                	push   0x8
  41cfd1:	8b 82 b8 05 00 00    	mov    eax,DWORD PTR [edx+0x5b8]
  41cfd7:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cfda:	eb 2e                	jmp    0x41d00a
  41cfdc:	8b 86 10 02 00 00    	mov    eax,DWORD PTR [esi+0x210]
  41cfe2:	85 c0                	test   eax,eax
  41cfe4:	7e 13                	jle    0x41cff9
  41cfe6:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cfec:	6a 13                	push   0x13
  41cfee:	8b 91 b8 05 00 00    	mov    edx,DWORD PTR [ecx+0x5b8]
  41cff4:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cff7:	eb 11                	jmp    0x41d00a
  41cff9:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cfff:	6a 00                	push   0x0
  41d001:	8b 88 b8 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b8]
  41d007:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41d00a:	e8 e1 16 02 00       	call   0x43e6f0
  41d00f:	8b 86 14 02 00 00    	mov    eax,DWORD PTR [esi+0x214]
  41d015:	85 c0                	test   eax,eax
  41d017:	7e 13                	jle    0x41d02c
  41d019:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41d01f:	6a 14                	push   0x14
  41d021:	8b 82 bc 05 00 00    	mov    eax,DWORD PTR [edx+0x5bc]
  41d027:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d02a:	eb 11                	jmp    0x41d03d
  41d02c:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41d032:	6a 00                	push   0x0
  41d034:	8b 91 bc 05 00 00    	mov    edx,DWORD PTR [ecx+0x5bc]
  41d03a:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41d03d:	e8 ae 16 02 00       	call   0x43e6f0
  41d042:	8b 86 1c 02 00 00    	mov    eax,DWORD PTR [esi+0x21c]
  41d048:	85 c0                	test   eax,eax
  41d04a:	7e 13                	jle    0x41d05f
  41d04c:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41d052:	6a 15                	push   0x15
  41d054:	8b 88 c0 05 00 00    	mov    ecx,DWORD PTR [eax+0x5c0]
  41d05a:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41d05d:	eb 11                	jmp    0x41d070
  41d05f:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41d065:	6a 00                	push   0x0
  41d067:	8b 82 c0 05 00 00    	mov    eax,DWORD PTR [edx+0x5c0]
  41d06d:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d070:	e8 7b 16 02 00       	call   0x43e6f0
  41d075:	8b 86 20 02 00 00    	mov    eax,DWORD PTR [esi+0x220]
  41d07b:	85 c0                	test   eax,eax
  41d07d:	7e 13                	jle    0x41d092
  41d07f:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41d085:	6a 09                	push   0x9
  41d087:	8b 91 c4 05 00 00    	mov    edx,DWORD PTR [ecx+0x5c4]
  41d08d:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41d090:	eb 11                	jmp    0x41d0a3
  41d092:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41d098:	6a 00                	push   0x0
  41d09a:	8b 88 c4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5c4]
  41d0a0:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41d0a3:	e8 48 16 02 00       	call   0x43e6f0
  41d0a8:	8b 86 28 02 00 00    	mov    eax,DWORD PTR [esi+0x228]
  41d0ae:	85 c0                	test   eax,eax
  41d0b0:	7e 13                	jle    0x41d0c5
  41d0b2:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41d0b8:	6a 10                	push   0x10
  41d0ba:	8b 82 c8 05 00 00    	mov    eax,DWORD PTR [edx+0x5c8]
  41d0c0:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d0c3:	eb 11                	jmp    0x41d0d6
  41d0c5:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41d0cb:	6a 00                	push   0x0
  41d0cd:	8b 91 c8 05 00 00    	mov    edx,DWORD PTR [ecx+0x5c8]
  41d0d3:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41d0d6:	e8 15 16 02 00       	call   0x43e6f0
  41d0db:	8b 86 0c 02 00 00    	mov    eax,DWORD PTR [esi+0x20c]
  41d0e1:	85 c0                	test   eax,eax
  41d0e3:	7e 1e                	jle    0x41d103
  41d0e5:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41d0eb:	6a 16                	push   0x16
  41d0ed:	8b 88 cc 05 00 00    	mov    ecx,DWORD PTR [eax+0x5cc]
  41d0f3:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41d0f6:	e8 f5 15 02 00       	call   0x43e6f0
  41d0fb:	5f                   	pop    edi
  41d0fc:	5e                   	pop    esi
  41d0fd:	5d                   	pop    ebp
  41d0fe:	5b                   	pop    ebx
  41d0ff:	83 c4 1c             	add    esp,0x1c
  41d102:	c3                   	ret
  41d103:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41d109:	6a 00                	push   0x0
  41d10b:	8b 82 cc 05 00 00    	mov    eax,DWORD PTR [edx+0x5cc]
  41d111:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d114:	e8 d7 15 02 00       	call   0x43e6f0
  41d119:	5f                   	pop    edi
  41d11a:	5e                   	pop    esi
  41d11b:	5d                   	pop    ebp
  41d11c:	5b                   	pop    ebx
  41d11d:	83 c4 1c             	add    esp,0x1c
  41d120:	c3                   	ret
  41d121:	90                   	nop
  41d122:	90                   	nop
  41d123:	90                   	nop
  41d124:	90                   	nop
  41d125:	90                   	nop
  41d126:	90                   	nop
  41d127:	90                   	nop
  41d128:	90                   	nop
  41d129:	90                   	nop
  41d12a:	90                   	nop
  41d12b:	90                   	nop
  41d12c:	90                   	nop
  41d12d:	90                   	nop
  41d12e:	90                   	nop
  41d12f:	90                   	nop
  41d130:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  41d134:	89 81 88 05 00 00    	mov    DWORD PTR [ecx+0x588],eax
  41d13a:	c2 04 00             	ret    0x4
  41d13d:	90                   	nop
  41d13e:	90                   	nop
  41d13f:	90                   	nop
  41d140:	8a 81 79 05 00 00    	mov    al,BYTE PTR [ecx+0x579]
  41d146:	84 c0                	test   al,al
  41d148:	74 05                	je     0x41d14f
  41d14a:	32 c0                	xor    al,al
  41d14c:	c2 04 00             	ret    0x4
  41d14f:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  41d153:	c6 00 00             	mov    BYTE PTR [eax],0x0
  41d156:	b0 01                	mov    al,0x1
  41d158:	c2 04 00             	ret    0x4
  41d15b:	90                   	nop
  41d15c:	90                   	nop
  41d15d:	90                   	nop
  41d15e:	90                   	nop
  41d15f:	90                   	nop
  41d160:	6a ff                	push   0xffffffff
  41d162:	68 c2 e0 45 00       	push   0x45e0c2
  41d167:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41d16d:	50                   	push   eax
  41d16e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41d175:	83 ec 24             	sub    esp,0x24
  41d178:	53                   	push   ebx
  41d179:	55                   	push   ebp
  41d17a:	56                   	push   esi
  41d17b:	8b f1                	mov    esi,ecx
  41d17d:	57                   	push   edi
  41d17e:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  41d182:	e8 89 3d 01 00       	call   0x430f10
  41d187:	33 db                	xor    ebx,ebx
  41d189:	8b ce                	mov    ecx,esi
  41d18b:	53                   	push   ebx
  41d18c:	68 48 4b 4c 00       	push   0x4c4b48
  41d191:	56                   	push   esi
  41d192:	89 5c 24 48          	mov    DWORD PTR [esp+0x48],ebx
  41d196:	c7 06 d0 0a 46 00    	mov    DWORD PTR [esi],0x460ad0
  41d19c:	e8 8f 72 01 00       	call   0x434430
  41d1a1:	8d 8e 18 02 00 00    	lea    ecx,[esi+0x218]
  41d1a7:	e8 84 7b fe ff       	call   0x404d30
  41d1ac:	68 dc 99 46 00       	push   0x4699dc
  41d1b1:	8d 8e 6c 05 00 00    	lea    ecx,[esi+0x56c]
  41d1b7:	53                   	push   ebx
  41d1b8:	89 44 24 38          	mov    DWORD PTR [esp+0x38],eax
  41d1bc:	53                   	push   ebx
  41d1bd:	8d 86 68 05 00 00    	lea    eax,[esi+0x568]
  41d1c3:	89 4c 24 2c          	mov    DWORD PTR [esp+0x2c],ecx
  41d1c7:	6a 79                	push   0x79
  41d1c9:	68 0c fe ff ff       	push   0xfffffe0c
  41d1ce:	8b ce                	mov    ecx,esi
  41d1d0:	89 44 24 30          	mov    DWORD PTR [esp+0x30],eax
  41d1d4:	e8 07 3f 01 00       	call   0x4310e0
  41d1d9:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  41d1df:	53                   	push   ebx
  41d1e0:	e8 0b 15 02 00       	call   0x43e6f0
  41d1e5:	6a 2c                	push   0x2c
  41d1e7:	e8 73 44 03 00       	call   0x45165f
  41d1ec:	83 c4 04             	add    esp,0x4
  41d1ef:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41d1f3:	3b c3                	cmp    eax,ebx
  41d1f5:	c6 44 24 3c 01       	mov    BYTE PTR [esp+0x3c],0x1
  41d1fa:	74 0e                	je     0x41d20a
  41d1fc:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  41d200:	8b c8                	mov    ecx,eax
  41d202:	52                   	push   edx
  41d203:	e8 48 49 01 00       	call   0x431b50
  41d208:	eb 02                	jmp    0x41d20c
  41d20a:	33 c0                	xor    eax,eax
  41d20c:	53                   	push   ebx
  41d20d:	68 f4 93 46 00       	push   0x4693f4
  41d212:	6a 20                	push   0x20
  41d214:	6a 28                	push   0x28
  41d216:	8b c8                	mov    ecx,eax
  41d218:	88 5c 24 4c          	mov    BYTE PTR [esp+0x4c],bl
  41d21c:	89 86 88 05 00 00    	mov    DWORD PTR [esi+0x588],eax
  41d222:	e8 e9 49 01 00       	call   0x431c10
  41d227:	6a 2c                	push   0x2c
  41d229:	e8 31 44 03 00       	call   0x45165f
  41d22e:	83 c4 04             	add    esp,0x4
  41d231:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41d235:	3b c3                	cmp    eax,ebx
  41d237:	c6 44 24 3c 02       	mov    BYTE PTR [esp+0x3c],0x2
  41d23c:	74 0e                	je     0x41d24c
  41d23e:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  41d242:	51                   	push   ecx
  41d243:	8b c8                	mov    ecx,eax
  41d245:	e8 06 49 01 00       	call   0x431b50
  41d24a:	eb 02                	jmp    0x41d24e
  41d24c:	33 c0                	xor    eax,eax
  41d24e:	53                   	push   ebx
  41d24f:	68 dc 93 46 00       	push   0x4693dc
  41d254:	6a 20                	push   0x20
  41d256:	6a 0e                	push   0xe
  41d258:	8b c8                	mov    ecx,eax
  41d25a:	88 5c 24 4c          	mov    BYTE PTR [esp+0x4c],bl
  41d25e:	89 86 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],eax
  41d264:	e8 a7 49 01 00       	call   0x431c10
  41d269:	6a 2c                	push   0x2c
  41d26b:	e8 ef 43 03 00       	call   0x45165f
  41d270:	83 c4 04             	add    esp,0x4
  41d273:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41d277:	3b c3                	cmp    eax,ebx
  41d279:	c6 44 24 3c 03       	mov    BYTE PTR [esp+0x3c],0x3
  41d27e:	74 0e                	je     0x41d28e
  41d280:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  41d284:	8b c8                	mov    ecx,eax
  41d286:	52                   	push   edx
  41d287:	e8 c4 48 01 00       	call   0x431b50
  41d28c:	eb 02                	jmp    0x41d290
  41d28e:	33 c0                	xor    eax,eax
  41d290:	53                   	push   ebx
  41d291:	68 c4 93 46 00       	push   0x4693c4
  41d296:	6a 3e                	push   0x3e
  41d298:	68 92 00 00 00       	push   0x92
  41d29d:	8b c8                	mov    ecx,eax
  41d29f:	88 5c 24 4c          	mov    BYTE PTR [esp+0x4c],bl
  41d2a3:	89 86 90 05 00 00    	mov    DWORD PTR [esi+0x590],eax
  41d2a9:	e8 62 49 01 00       	call   0x431c10
  41d2ae:	6a 2c                	push   0x2c
  41d2b0:	e8 aa 43 03 00       	call   0x45165f
  41d2b5:	83 c4 04             	add    esp,0x4
  41d2b8:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41d2bc:	3b c3                	cmp    eax,ebx
  41d2be:	c6 44 24 3c 04       	mov    BYTE PTR [esp+0x3c],0x4
  41d2c3:	74 0e                	je     0x41d2d3
  41d2c5:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  41d2c9:	51                   	push   ecx
  41d2ca:	8b c8                	mov    ecx,eax
  41d2cc:	e8 7f 48 01 00       	call   0x431b50
  41d2d1:	eb 02                	jmp    0x41d2d5
  41d2d3:	33 c0                	xor    eax,eax
  41d2d5:	53                   	push   ebx
  41d2d6:	68 ac 93 46 00       	push   0x4693ac
  41d2db:	6a 5b                	push   0x5b
  41d2dd:	68 91 00 00 00       	push   0x91
  41d2e2:	8b c8                	mov    ecx,eax
  41d2e4:	88 5c 24 4c          	mov    BYTE PTR [esp+0x4c],bl
  41d2e8:	89 86 94 05 00 00    	mov    DWORD PTR [esi+0x594],eax
  41d2ee:	e8 1d 49 01 00       	call   0x431c10
  41d2f3:	33 ff                	xor    edi,edi
  41d2f5:	c7 44 24 10 10 00 00 	mov    DWORD PTR [esp+0x10],0x10
  41d2fc:	00 
  41d2fd:	8d ae 98 05 00 00    	lea    ebp,[esi+0x598]
  41d303:	6a 2c                	push   0x2c
  41d305:	e8 55 43 03 00       	call   0x45165f
  41d30a:	83 c4 04             	add    esp,0x4
  41d30d:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  41d311:	3b c3                	cmp    eax,ebx
  41d313:	c6 44 24 3c 05       	mov    BYTE PTR [esp+0x3c],0x5
  41d318:	74 0e                	je     0x41d328
  41d31a:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  41d31e:	8b c8                	mov    ecx,eax
  41d320:	52                   	push   edx
  41d321:	e8 2a 48 01 00       	call   0x431b50
  41d326:	eb 02                	jmp    0x41d32a
  41d328:	33 c0                	xor    eax,eax
  41d32a:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  41d32e:	53                   	push   ebx
  41d32f:	68 c4 99 46 00       	push   0x4699c4
  41d334:	68 f9 00 00 00       	push   0xf9
  41d339:	51                   	push   ecx
  41d33a:	8b c8                	mov    ecx,eax
  41d33c:	88 5c 24 4c          	mov    BYTE PTR [esp+0x4c],bl
  41d340:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  41d343:	e8 c8 48 01 00       	call   0x431c10
  41d348:	8b 55 00             	mov    edx,DWORD PTR [ebp+0x0]
  41d34b:	47                   	inc    edi
  41d34c:	57                   	push   edi
  41d34d:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41d350:	e8 9b 13 02 00       	call   0x43e6f0
  41d355:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41d359:	83 c5 04             	add    ebp,0x4
  41d35c:	83 c0 1e             	add    eax,0x1e
  41d35f:	3d e2 00 00 00       	cmp    eax,0xe2
  41d364:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41d368:	7c 99                	jl     0x41d303
  41d36a:	8d 47 01             	lea    eax,[edi+0x1]
  41d36d:	bd 10 00 00 00       	mov    ebp,0x10
  41d372:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41d376:	8d bc be 98 05 00 00 	lea    edi,[esi+edi*4+0x598]
  41d37d:	6a 2c                	push   0x2c
  41d37f:	e8 db 42 03 00       	call   0x45165f
  41d384:	83 c4 04             	add    esp,0x4
  41d387:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  41d38b:	3b c3                	cmp    eax,ebx
  41d38d:	c6 44 24 3c 06       	mov    BYTE PTR [esp+0x3c],0x6
  41d392:	74 0e                	je     0x41d3a2
  41d394:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  41d398:	51                   	push   ecx
  41d399:	8b c8                	mov    ecx,eax
  41d39b:	e8 b0 47 01 00       	call   0x431b50
  41d3a0:	eb 02                	jmp    0x41d3a4
  41d3a2:	33 c0                	xor    eax,eax
  41d3a4:	53                   	push   ebx
  41d3a5:	68 c4 99 46 00       	push   0x4699c4
  41d3aa:	68 16 01 00 00       	push   0x116
  41d3af:	55                   	push   ebp
  41d3b0:	8b c8                	mov    ecx,eax
  41d3b2:	88 5c 24 4c          	mov    BYTE PTR [esp+0x4c],bl
  41d3b6:	89 07                	mov    DWORD PTR [edi],eax
  41d3b8:	e8 53 48 01 00       	call   0x431c10
  41d3bd:	8b 07                	mov    eax,DWORD PTR [edi]
  41d3bf:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  41d3c3:	52                   	push   edx
  41d3c4:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d3c7:	e8 24 13 02 00       	call   0x43e6f0
  41d3cc:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  41d3d0:	83 c7 04             	add    edi,0x4
  41d3d3:	83 c5 1e             	add    ebp,0x1e
  41d3d6:	42                   	inc    edx
  41d3d7:	81 fd e2 00 00 00    	cmp    ebp,0xe2
  41d3dd:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  41d3e1:	7c 9a                	jl     0x41d37d
