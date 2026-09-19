
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00422530 <.text+0x21530>:
  422530:	83 ec 08             	sub    esp,0x8
  422533:	53                   	push   ebx
  422534:	55                   	push   ebp
  422535:	57                   	push   edi
  422536:	8b f9                	mov    edi,ecx
  422538:	b3 01                	mov    bl,0x1
  42253a:	83 cd ff             	or     ebp,0xffffffff
  42253d:	8b 87 e8 0c 00 00    	mov    eax,DWORD PTR [edi+0xce8]
  422543:	88 5c 24 0f          	mov    BYTE PTR [esp+0xf],bl
  422547:	3d a0 0f 00 00       	cmp    eax,0xfa0
  42254c:	c7 44 24 10 00 00 00 	mov    DWORD PTR [esp+0x10],0x0
  422553:	00 
  422554:	74 3f                	je     0x422595
  422556:	3d a1 0f 00 00       	cmp    eax,0xfa1
  42255b:	75 41                	jne    0x42259e
  42255d:	8b 87 e0 0d 00 00    	mov    eax,DWORD PTR [edi+0xde0]
  422563:	3b c5                	cmp    eax,ebp
  422565:	74 2e                	je     0x422595
  422567:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  42256e:	39 a8 2c 07 00 00    	cmp    DWORD PTR [eax+0x72c],ebp
  422574:	74 1f                	je     0x422595
  422576:	8b 88 7c 08 00 00    	mov    ecx,DWORD PTR [eax+0x87c]
  42257c:	85 c9                	test   ecx,ecx
  42257e:	7e 15                	jle    0x422595
  422580:	81 b8 e4 0c 00 00 bb 	cmp    DWORD PTR [eax+0xce4],0xbbb
  422587:	0b 00 00 
  42258a:	75 09                	jne    0x422595
  42258c:	5f                   	pop    edi
  42258d:	5d                   	pop    ebp
  42258e:	32 c0                	xor    al,al
  422590:	5b                   	pop    ebx
  422591:	83 c4 08             	add    esp,0x8
  422594:	c3                   	ret
  422595:	a0 dc 96 46 00       	mov    al,ds:0x4696dc
  42259a:	84 c0                	test   al,al
  42259c:	75 09                	jne    0x4225a7
  42259e:	5f                   	pop    edi
  42259f:	5d                   	pop    ebp
  4225a0:	32 c0                	xor    al,al
  4225a2:	5b                   	pop    ebx
  4225a3:	83 c4 08             	add    esp,0x8
  4225a6:	c3                   	ret
  4225a7:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  4225ad:	32 d2                	xor    dl,dl
  4225af:	33 c0                	xor    eax,eax
  4225b1:	56                   	push   esi
  4225b2:	85 c9                	test   ecx,ecx
  4225b4:	7e 29                	jle    0x4225df
  4225b6:	8d b7 88 00 00 00    	lea    esi,[edi+0x88]
  4225bc:	84 d2                	test   dl,dl
  4225be:	75 1f                	jne    0x4225df
  4225c0:	8b 1e                	mov    ebx,DWORD PTR [esi]
  4225c2:	8b 1c 9d 94 fd 89 00 	mov    ebx,DWORD PTR [ebx*4+0x89fd94]
  4225c9:	39 ab 2c 07 00 00    	cmp    DWORD PTR [ebx+0x72c],ebp
  4225cf:	74 02                	je     0x4225d3
  4225d1:	b2 01                	mov    dl,0x1
  4225d3:	8a 5c 24 13          	mov    bl,BYTE PTR [esp+0x13]
  4225d7:	40                   	inc    eax
  4225d8:	83 c6 04             	add    esi,0x4
  4225db:	3b c1                	cmp    eax,ecx
  4225dd:	7c dd                	jl     0x4225bc
  4225df:	8b 8f f4 0c 00 00    	mov    ecx,DWORD PTR [edi+0xcf4]
  4225e5:	8b 6c 24 14          	mov    ebp,DWORD PTR [esp+0x14]
  4225e9:	85 c9                	test   ecx,ecx
  4225eb:	74 11                	je     0x4225fe
  4225ed:	8b 87 d0 0d 00 00    	mov    eax,DWORD PTR [edi+0xdd0]
  4225f3:	85 c0                	test   eax,eax
  4225f5:	7e 07                	jle    0x4225fe
  4225f7:	bd 0a 00 00 00       	mov    ebp,0xa
  4225fc:	eb 2e                	jmp    0x42262c
  4225fe:	8b 87 48 08 00 00    	mov    eax,DWORD PTR [edi+0x848]
  422604:	85 c0                	test   eax,eax
  422606:	74 4c                	je     0x422654
  422608:	8b 68 18             	mov    ebp,DWORD PTR [eax+0x18]
  42260b:	8b 00                	mov    eax,DWORD PTR [eax]
  42260d:	3b 87 4c 08 00 00    	cmp    eax,DWORD PTR [edi+0x84c]
  422613:	75 02                	jne    0x422617
  422615:	33 ed                	xor    ebp,ebp
  422617:	3d 97 01 00 00       	cmp    eax,0x197
  42261c:	74 07                	je     0x422625
  42261e:	3d d0 01 00 00       	cmp    eax,0x1d0
  422623:	75 02                	jne    0x422627
  422625:	b2 01                	mov    dl,0x1
  422627:	83 fd 0a             	cmp    ebp,0xa
  42262a:	75 28                	jne    0x422654
  42262c:	85 c9                	test   ecx,ecx
  42262e:	74 24                	je     0x422654
  422630:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  422633:	3d 0f 01 00 00       	cmp    eax,0x10f
  422638:	0f 84 52 01 00 00    	je     0x422790
  42263e:	3d 10 01 00 00       	cmp    eax,0x110
  422643:	0f 84 47 01 00 00    	je     0x422790
  422649:	3d 11 01 00 00       	cmp    eax,0x111
  42264e:	0f 84 3c 01 00 00    	je     0x422790
  422654:	84 d2                	test   dl,dl
  422656:	75 0b                	jne    0x422663
  422658:	6a 00                	push   0x0
  42265a:	6a 00                	push   0x0
  42265c:	8b cf                	mov    ecx,edi
  42265e:	e8 9d 08 00 00       	call   0x422f00
  422663:	85 ed                	test   ebp,ebp
  422665:	0f 85 08 01 00 00    	jne    0x422773
  42266b:	b3 01                	mov    bl,0x1
  42266d:	b9 94 fd 89 00       	mov    ecx,0x89fd94
  422672:	84 db                	test   bl,bl
  422674:	74 58                	je     0x4226ce
  422676:	8b 01                	mov    eax,DWORD PTR [ecx]
  422678:	8b 80 e4 0c 00 00    	mov    eax,DWORD PTR [eax+0xce4]
  42267e:	3d b9 0b 00 00       	cmp    eax,0xbb9
  422683:	74 3c                	je     0x4226c1
  422685:	3d ba 0b 00 00       	cmp    eax,0xbba
  42268a:	74 35                	je     0x4226c1
  42268c:	3d bb 0b 00 00       	cmp    eax,0xbbb
  422691:	74 2e                	je     0x4226c1
  422693:	3d be 0b 00 00       	cmp    eax,0xbbe
  422698:	74 27                	je     0x4226c1
  42269a:	3d bf 0b 00 00       	cmp    eax,0xbbf
  42269f:	74 20                	je     0x4226c1
  4226a1:	3d c0 0b 00 00       	cmp    eax,0xbc0
  4226a6:	74 19                	je     0x4226c1
  4226a8:	3d c6 0b 00 00       	cmp    eax,0xbc6
  4226ad:	74 12                	je     0x4226c1
  4226af:	3d c4 0b 00 00       	cmp    eax,0xbc4
  4226b4:	74 0b                	je     0x4226c1
  4226b6:	3d c5 0b 00 00       	cmp    eax,0xbc5
  4226bb:	74 04                	je     0x4226c1
  4226bd:	32 db                	xor    bl,bl
  4226bf:	eb 02                	jmp    0x4226c3
  4226c1:	b3 01                	mov    bl,0x1
  4226c3:	83 c1 04             	add    ecx,0x4
  4226c6:	81 f9 d4 fd 89 00    	cmp    ecx,0x89fdd4
  4226cc:	7c a4                	jl     0x422672
  4226ce:	8b b7 c8 00 00 00    	mov    esi,DWORD PTR [edi+0xc8]
  4226d4:	33 d2                	xor    edx,edx
  4226d6:	85 f6                	test   esi,esi
  4226d8:	0f 8e 67 01 00 00    	jle    0x422845
  4226de:	81 c7 88 00 00 00    	add    edi,0x88
  4226e4:	bd bb 0b 00 00       	mov    ebp,0xbbb
  4226e9:	84 db                	test   bl,bl
  4226eb:	0f 84 54 01 00 00    	je     0x422845
  4226f1:	8b 0f                	mov    ecx,DWORD PTR [edi]
  4226f3:	8b 04 8d 94 fd 89 00 	mov    eax,DWORD PTR [ecx*4+0x89fd94]
  4226fa:	81 b8 e8 0c 00 00 a0 	cmp    DWORD PTR [eax+0xce8],0xfa0
  422701:	0f 00 00 
  422704:	75 2d                	jne    0x422733
  422706:	8b 80 e4 0c 00 00    	mov    eax,DWORD PTR [eax+0xce4]
  42270c:	3d b9 0b 00 00       	cmp    eax,0xbb9
  422711:	74 1c                	je     0x42272f
  422713:	3d ba 0b 00 00       	cmp    eax,0xbba
  422718:	74 15                	je     0x42272f
  42271a:	3d be 0b 00 00       	cmp    eax,0xbbe
  42271f:	74 0e                	je     0x42272f
  422721:	3d c6 0b 00 00       	cmp    eax,0xbc6
  422726:	74 07                	je     0x42272f
  422728:	3d bf 0b 00 00       	cmp    eax,0xbbf
  42272d:	75 30                	jne    0x42275f
  42272f:	b3 01                	mov    bl,0x1
  422731:	eb 2e                	jmp    0x422761
  422733:	8b 88 e0 0d 00 00    	mov    ecx,DWORD PTR [eax+0xde0]
  422739:	85 c9                	test   ecx,ecx
  42273b:	7c 24                	jl     0x422761
  42273d:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  422744:	39 a9 e4 0c 00 00    	cmp    DWORD PTR [ecx+0xce4],ebp
  42274a:	75 15                	jne    0x422761
  42274c:	83 b9 7c 08 00 00 00 	cmp    DWORD PTR [ecx+0x87c],0x0
  422753:	7e 0c                	jle    0x422761
  422755:	8b 88 7c 08 00 00    	mov    ecx,DWORD PTR [eax+0x87c]
  42275b:	85 c9                	test   ecx,ecx
  42275d:	7e 02                	jle    0x422761
  42275f:	32 db                	xor    bl,bl
  422761:	42                   	inc    edx
  422762:	83 c7 04             	add    edi,0x4
  422765:	3b d6                	cmp    edx,esi
  422767:	7c 80                	jl     0x4226e9
  422769:	5e                   	pop    esi
  42276a:	5f                   	pop    edi
  42276b:	8a c3                	mov    al,bl
  42276d:	5d                   	pop    ebp
  42276e:	5b                   	pop    ebx
  42276f:	83 c4 08             	add    esp,0x8
  422772:	c3                   	ret
  422773:	83 fd 01             	cmp    ebp,0x1
  422776:	74 18                	je     0x422790
  422778:	83 fd 02             	cmp    ebp,0x2
  42277b:	74 13                	je     0x422790
  42277d:	83 fd 04             	cmp    ebp,0x4
  422780:	74 0e                	je     0x422790
  422782:	83 fd 03             	cmp    ebp,0x3
  422785:	74 09                	je     0x422790
  422787:	83 fd 0a             	cmp    ebp,0xa
  42278a:	0f 85 b5 00 00 00    	jne    0x422845
  422790:	b3 01                	mov    bl,0x1
  422792:	be 94 fd 89 00       	mov    esi,0x89fd94
  422797:	bf a0 0f 00 00       	mov    edi,0xfa0
  42279c:	84 db                	test   bl,bl
  42279e:	0f 84 96 00 00 00    	je     0x42283a
  4227a4:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4227a6:	39 b9 e8 0c 00 00    	cmp    DWORD PTR [ecx+0xce8],edi
  4227ac:	75 42                	jne    0x4227f0
  4227ae:	8b 81 e4 0c 00 00    	mov    eax,DWORD PTR [ecx+0xce4]
  4227b4:	3d b9 0b 00 00       	cmp    eax,0xbb9
  4227b9:	74 31                	je     0x4227ec
  4227bb:	3d ba 0b 00 00       	cmp    eax,0xbba
  4227c0:	74 2a                	je     0x4227ec
  4227c2:	3d be 0b 00 00       	cmp    eax,0xbbe
  4227c7:	74 23                	je     0x4227ec
  4227c9:	3d bf 0b 00 00       	cmp    eax,0xbbf
  4227ce:	74 1c                	je     0x4227ec
  4227d0:	3d c0 0b 00 00       	cmp    eax,0xbc0
  4227d5:	74 15                	je     0x4227ec
  4227d7:	3d c4 0b 00 00       	cmp    eax,0xbc4
  4227dc:	74 0e                	je     0x4227ec
  4227de:	3d c6 0b 00 00       	cmp    eax,0xbc6
  4227e3:	74 07                	je     0x4227ec
  4227e5:	3d c5 0b 00 00       	cmp    eax,0xbc5
  4227ea:	75 3d                	jne    0x422829
  4227ec:	b3 01                	mov    bl,0x1
  4227ee:	eb 3b                	jmp    0x42282b
  4227f0:	8b 81 e0 0d 00 00    	mov    eax,DWORD PTR [ecx+0xde0]
  4227f6:	85 c0                	test   eax,eax
  4227f8:	7c 31                	jl     0x42282b
  4227fa:	8b 14 85 94 fd 89 00 	mov    edx,DWORD PTR [eax*4+0x89fd94]
  422801:	8b 82 e4 0c 00 00    	mov    eax,DWORD PTR [edx+0xce4]
  422807:	3d bb 0b 00 00       	cmp    eax,0xbbb
  42280c:	74 07                	je     0x422815
  42280e:	3d c0 0b 00 00       	cmp    eax,0xbc0
  422813:	75 16                	jne    0x42282b
  422815:	8b 82 7c 08 00 00    	mov    eax,DWORD PTR [edx+0x87c]
  42281b:	85 c0                	test   eax,eax
  42281d:	7e 0c                	jle    0x42282b
  42281f:	8b 81 7c 08 00 00    	mov    eax,DWORD PTR [ecx+0x87c]
  422825:	85 c0                	test   eax,eax
  422827:	7e 02                	jle    0x42282b
  422829:	32 db                	xor    bl,bl
  42282b:	83 c6 04             	add    esi,0x4
  42282e:	81 fe d4 fd 89 00    	cmp    esi,0x89fdd4
  422834:	0f 8c 62 ff ff ff    	jl     0x42279c
  42283a:	83 fd 0a             	cmp    ebp,0xa
  42283d:	75 06                	jne    0x422845
  42283f:	84 db                	test   bl,bl
  422841:	74 02                	je     0x422845
  422843:	b3 01                	mov    bl,0x1
  422845:	5e                   	pop    esi
  422846:	5f                   	pop    edi
  422847:	8a c3                	mov    al,bl
  422849:	5d                   	pop    ebp
  42284a:	5b                   	pop    ebx
  42284b:	83 c4 08             	add    esp,0x8
  42284e:	c3                   	ret
  42284f:	90                   	nop
