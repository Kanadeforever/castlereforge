
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00432518 <.text+0x31518>:
  432518:	8b cf                	mov    ecx,edi
  43251a:	e8 51 01 00 00       	call   0x432670
  43251f:	68 a4 03 00 00       	push   0x3a4
  432524:	e8 36 f1 01 00       	call   0x45165f
  432529:	83 c4 04             	add    esp,0x4
  43252c:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  432530:	85 c0                	test   eax,eax
  432532:	c7 44 24 14 04 00 00 	mov    DWORD PTR [esp+0x14],0x4
  432539:	00 
  43253a:	74 09                	je     0x432545
  43253c:	8b c8                	mov    ecx,eax
  43253e:	e8 fd 16 00 00       	call   0x433c40
  432543:	eb 02                	jmp    0x432547
  432545:	33 c0                	xor    eax,eax
  432547:	6a fe                	push   0xfffffffe
  432549:	6a 00                	push   0x0
  43254b:	8b c8                	mov    ecx,eax
  43254d:	89 74 24 1c          	mov    DWORD PTR [esp+0x1c],esi
  432551:	a3 c8 40 8c 00       	mov    ds:0x8c40c8,eax
  432556:	e8 05 18 00 00       	call   0x433d60
  43255b:	84 c0                	test   al,al
  43255d:	75 23                	jne    0x432582
  43255f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432565:	68 cc ab 46 00       	push   0x46abcc
  43256a:	6a 00                	push   0x0
  43256c:	e8 ff f8 ff ff       	call   0x431e70
  432571:	5f                   	pop    edi
  432572:	5e                   	pop    esi
  432573:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  432577:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43257e:	83 c4 10             	add    esp,0x10
  432581:	c3                   	ret
  432582:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  432587:	6a 00                	push   0x0
  432589:	8b 88 d8 00 00 00    	mov    ecx,DWORD PTR [eax+0xd8]
  43258f:	51                   	push   ecx
  432590:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  432596:	e8 95 1e ff ff       	call   0x424430
  43259b:	85 c0                	test   eax,eax
  43259d:	7d 1a                	jge    0x4325b9
  43259f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4325a5:	68 98 ab 46 00       	push   0x46ab98
  4325aa:	68 74 ab 46 00       	push   0x46ab74
  4325af:	e8 9c fa ff ff       	call   0x432050
  4325b4:	b8 01 00 00 00       	mov    eax,0x1
  4325b9:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  4325bf:	50                   	push   eax
  4325c0:	e8 4b 1b 00 00       	call   0x434110
  4325c5:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  4325cb:	68 68 ab 46 00       	push   0x46ab68
  4325d0:	e8 ab 1a 00 00       	call   0x434080
  4325d5:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  4325db:	68 68 ab 46 00       	push   0x46ab68
  4325e0:	e8 eb 1a 00 00       	call   0x4340d0
  4325e5:	68 a0 83 00 00       	push   0x83a0
  4325ea:	e8 70 f0 01 00       	call   0x45165f
  4325ef:	83 c4 04             	add    esp,0x4
  4325f2:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4325f6:	85 c0                	test   eax,eax
  4325f8:	c7 44 24 14 05 00 00 	mov    DWORD PTR [esp+0x14],0x5
  4325ff:	00 
  432600:	74 09                	je     0x43260b
  432602:	8b c8                	mov    ecx,eax
  432604:	e8 27 ac ff ff       	call   0x42d230
  432609:	eb 02                	jmp    0x43260d
  43260b:	33 c0                	xor    eax,eax
  43260d:	8b c8                	mov    ecx,eax
  43260f:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  432613:	a3 dc 01 8b 00       	mov    ds:0x8b01dc,eax
  432618:	e8 23 ad ff ff       	call   0x42d340
  43261d:	84 c0                	test   al,al
  43261f:	75 23                	jne    0x432644
  432621:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432627:	68 54 ab 46 00       	push   0x46ab54
  43262c:	6a 00                	push   0x0
  43262e:	e8 3d f8 ff ff       	call   0x431e70
  432633:	5f                   	pop    edi
  432634:	5e                   	pop    esi
  432635:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  432639:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432640:	83 c4 10             	add    esp,0x10
  432643:	c3                   	ret
  432644:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  43264a:	8b 0d d8 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01d8
  432650:	8b 82 c0 00 00 00    	mov    eax,DWORD PTR [edx+0xc0]
  432656:	50                   	push   eax
  432657:	e8 d4 a9 ff ff       	call   0x42d030
  43265c:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  432660:	5f                   	pop    edi
  432661:	b0 01                	mov    al,0x1
  432663:	5e                   	pop    esi
  432664:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43266b:	83 c4 10             	add    esp,0x10
  43266e:	c3                   	ret
  43266f:	90                   	nop
  432670:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  432676:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  43267c:	6a ff                	push   0xffffffff
  43267e:	68 7e ed 45 00       	push   0x45ed7e
  432683:	50                   	push   eax
  432684:	33 c0                	xor    eax,eax
  432686:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  43268d:	81 ec f8 01 00 00    	sub    esp,0x1f8
  432693:	85 c9                	test   ecx,ecx
  432695:	56                   	push   esi
  432696:	0f 84 d6 00 00 00    	je     0x432772
  43269c:	8b 0d b4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b4
  4326a2:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4326a8:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  4326ab:	8b 92 e4 00 00 00    	mov    edx,DWORD PTR [edx+0xe4]
  4326b1:	3b 91 f8 01 00 00    	cmp    edx,DWORD PTR [ecx+0x1f8]
  4326b7:	7d 06                	jge    0x4326bf
  4326b9:	85 d2                	test   edx,edx
  4326bb:	7c 02                	jl     0x4326bf
  4326bd:	8b c2                	mov    eax,edx
  4326bf:	8d 04 80             	lea    eax,[eax+eax*4]
  4326c2:	57                   	push   edi
  4326c3:	6a 14                	push   0x14
  4326c5:	8d 04 80             	lea    eax,[eax+eax*4]
  4326c8:	8d bc 41 fc 01 00 00 	lea    edi,[ecx+eax*2+0x1fc]
  4326cf:	e8 8b ef 01 00       	call   0x45165f
  4326d4:	83 c4 04             	add    esp,0x4
  4326d7:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4326db:	85 c0                	test   eax,eax
  4326dd:	c7 84 24 08 02 00 00 	mov    DWORD PTR [esp+0x208],0x0
  4326e4:	00 00 00 00 
  4326e8:	74 0d                	je     0x4326f7
  4326ea:	6a 01                	push   0x1
  4326ec:	8b c8                	mov    ecx,eax
  4326ee:	e8 fd ef 00 00       	call   0x4416f0
  4326f3:	8b f0                	mov    esi,eax
  4326f5:	eb 02                	jmp    0x4326f9
  4326f7:	33 f6                	xor    esi,esi
  4326f9:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  4326fd:	c7 84 24 08 02 00 00 	mov    DWORD PTR [esp+0x208],0xffffffff
  432704:	ff ff ff ff 
  432708:	51                   	push   ecx
  432709:	57                   	push   edi
  43270a:	b9 90 1c 8e 00       	mov    ecx,0x8e1c90
  43270f:	e8 dc f4 00 00       	call   0x441bf0
  432714:	84 c0                	test   al,al
  432716:	5f                   	pop    edi
  432717:	74 35                	je     0x43274e
  432719:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  43271f:	e8 8c 2f fd ff       	call   0x4056b0
  432724:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  43272a:	8d 54 24 08          	lea    edx,[esp+0x8]
  43272e:	6a 01                	push   0x1
  432730:	52                   	push   edx
  432731:	e8 3a 2f fd ff       	call   0x405670
  432736:	84 c0                	test   al,al
  432738:	75 24                	jne    0x43275e
  43273a:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432740:	68 88 ac 46 00       	push   0x46ac88
  432745:	6a 00                	push   0x0
  432747:	e8 24 f7 ff ff       	call   0x431e70
  43274c:	eb 10                	jmp    0x43275e
  43274e:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432754:	68 60 ac 46 00       	push   0x46ac60
  432759:	e8 12 f8 ff ff       	call   0x431f70
  43275e:	85 f6                	test   esi,esi
  432760:	74 10                	je     0x432772
  432762:	8b ce                	mov    ecx,esi
  432764:	e8 a7 ef 00 00       	call   0x441710
  432769:	56                   	push   esi
  43276a:	e8 e1 ed 01 00       	call   0x451550
  43276f:	83 c4 04             	add    esp,0x4
  432772:	8b 8c 24 fc 01 00 00 	mov    ecx,DWORD PTR [esp+0x1fc]
  432779:	5e                   	pop    esi
  43277a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432781:	81 c4 04 02 00 00    	add    esp,0x204
  432787:	c3                   	ret
  432788:	90                   	nop
  432789:	90                   	nop
  43278a:	90                   	nop
  43278b:	90                   	nop
  43278c:	90                   	nop
  43278d:	90                   	nop
  43278e:	90                   	nop
  43278f:	90                   	nop
  432790:	53                   	push   ebx
  432791:	56                   	push   esi
  432792:	8b 35 c8 40 8c 00    	mov    esi,DWORD PTR ds:0x8c40c8
  432798:	33 db                	xor    ebx,ebx
  43279a:	57                   	push   edi
  43279b:	3b f3                	cmp    esi,ebx
  43279d:	8b f9                	mov    edi,ecx
  43279f:	74 16                	je     0x4327b7
  4327a1:	8b ce                	mov    ecx,esi
  4327a3:	e8 28 15 00 00       	call   0x433cd0
  4327a8:	56                   	push   esi
  4327a9:	e8 a2 ed 01 00       	call   0x451550
  4327ae:	83 c4 04             	add    esp,0x4
  4327b1:	89 1d c8 40 8c 00    	mov    DWORD PTR ds:0x8c40c8,ebx
  4327b7:	8b b7 18 02 00 00    	mov    esi,DWORD PTR [edi+0x218]
  4327bd:	3b f3                	cmp    esi,ebx
  4327bf:	74 16                	je     0x4327d7
  4327c1:	8b ce                	mov    ecx,esi
  4327c3:	e8 48 f6 00 00       	call   0x441e10
  4327c8:	56                   	push   esi
  4327c9:	e8 82 ed 01 00       	call   0x451550
  4327ce:	83 c4 04             	add    esp,0x4
  4327d1:	89 9f 18 02 00 00    	mov    DWORD PTR [edi+0x218],ebx
  4327d7:	8b 35 dc 01 8b 00    	mov    esi,DWORD PTR ds:0x8b01dc
  4327dd:	3b f3                	cmp    esi,ebx
  4327df:	74 16                	je     0x4327f7
  4327e1:	8b ce                	mov    ecx,esi
  4327e3:	e8 e8 aa ff ff       	call   0x42d2d0
  4327e8:	56                   	push   esi
  4327e9:	e8 62 ed 01 00       	call   0x451550
  4327ee:	83 c4 04             	add    esp,0x4
  4327f1:	89 1d dc 01 8b 00    	mov    DWORD PTR ds:0x8b01dc,ebx
  4327f7:	8b 35 74 fd 89 00    	mov    esi,DWORD PTR ds:0x89fd74
  4327fd:	3b f3                	cmp    esi,ebx
  4327ff:	74 16                	je     0x432817
  432801:	8b ce                	mov    ecx,esi
  432803:	e8 28 8d fe ff       	call   0x41b530
  432808:	56                   	push   esi
  432809:	e8 42 ed 01 00       	call   0x451550
  43280e:	83 c4 04             	add    esp,0x4
  432811:	89 1d 74 fd 89 00    	mov    DWORD PTR ds:0x89fd74,ebx
  432817:	8b 35 a4 01 8a 00    	mov    esi,DWORD PTR ds:0x8a01a4
  43281d:	3b f3                	cmp    esi,ebx
  43281f:	74 16                	je     0x432837
  432821:	8b ce                	mov    ecx,esi
  432823:	e8 d8 40 ff ff       	call   0x426900
  432828:	56                   	push   esi
  432829:	e8 22 ed 01 00       	call   0x451550
  43282e:	83 c4 04             	add    esp,0x4
  432831:	89 1d a4 01 8a 00    	mov    DWORD PTR ds:0x8a01a4,ebx
  432837:	8b 35 28 24 8e 00    	mov    esi,DWORD PTR ds:0x8e2428
  43283d:	3b f3                	cmp    esi,ebx
  43283f:	74 16                	je     0x432857
  432841:	8b ce                	mov    ecx,esi
  432843:	e8 28 79 01 00       	call   0x44a170
  432848:	56                   	push   esi
  432849:	e8 02 ed 01 00       	call   0x451550
  43284e:	83 c4 04             	add    esp,0x4
  432851:	89 1d 28 24 8e 00    	mov    DWORD PTR ds:0x8e2428,ebx
  432857:	8b 0d 4c 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c4c
  43285d:	e8 de b9 00 00       	call   0x43e240
  432862:	8b cf                	mov    ecx,edi
  432864:	e8 97 f9 ff ff       	call   0x432200
  432869:	8b 35 c8 fc 89 00    	mov    esi,DWORD PTR ds:0x89fcc8
  43286f:	3b f3                	cmp    esi,ebx
  432871:	74 16                	je     0x432889
  432873:	8b ce                	mov    ecx,esi
  432875:	e8 16 f9 fd ff       	call   0x412190
  43287a:	56                   	push   esi
  43287b:	e8 d0 ec 01 00       	call   0x451550
  432880:	83 c4 04             	add    esp,0x4
  432883:	89 1d c8 fc 89 00    	mov    DWORD PTR ds:0x89fcc8,ebx
  432889:	88 9f 20 02 00 00    	mov    BYTE PTR [edi+0x220],bl
  43288f:	5f                   	pop    edi
  432890:	5e                   	pop    esi
  432891:	5b                   	pop    ebx
  432892:	c3                   	ret
  432893:	90                   	nop
  432894:	90                   	nop
  432895:	90                   	nop
  432896:	90                   	nop
  432897:	90                   	nop
  432898:	90                   	nop
  432899:	90                   	nop
  43289a:	90                   	nop
  43289b:	90                   	nop
  43289c:	90                   	nop
  43289d:	90                   	nop
  43289e:	90                   	nop
  43289f:	90                   	nop
  4328a0:	56                   	push   esi
  4328a1:	57                   	push   edi
  4328a2:	8b f1                	mov    esi,ecx
  4328a4:	6a 64                	push   0x64
  4328a6:	56                   	push   esi
  4328a7:	e8 54 1c 00 00       	call   0x434500
  4328ac:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  4328b2:	e8 d9 0c 01 00       	call   0x443590
  4328b7:	8b f8                	mov    edi,eax
  4328b9:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  4328be:	8a 88 fc 05 00 00    	mov    cl,BYTE PTR [eax+0x5fc]
  4328c4:	84 c9                	test   cl,cl
  4328c6:	74 05                	je     0x4328cd
  4328c8:	bf 03 00 00 00       	mov    edi,0x3
  4328cd:	8a 86 40 02 00 00    	mov    al,BYTE PTR [esi+0x240]
  4328d3:	84 c0                	test   al,al
  4328d5:	0f 85 9d 00 00 00    	jne    0x432978
  4328db:	83 ff 01             	cmp    edi,0x1
  4328de:	74 0f                	je     0x4328ef
  4328e0:	83 ff 02             	cmp    edi,0x2
  4328e3:	74 0a                	je     0x4328ef
  4328e5:	83 ff 04             	cmp    edi,0x4
  4328e8:	74 05                	je     0x4328ef
  4328ea:	83 ff 03             	cmp    edi,0x3
  4328ed:	75 7b                	jne    0x43296a
  4328ef:	83 ff 01             	cmp    edi,0x1
  4328f2:	c6 86 40 02 00 00 01 	mov    BYTE PTR [esi+0x240],0x1
  4328f9:	74 05                	je     0x432900
  4328fb:	83 ff 04             	cmp    edi,0x4
  4328fe:	75 1d                	jne    0x43291d
  432900:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432906:	6a 01                	push   0x1
  432908:	e8 53 0d 01 00       	call   0x443660
  43290d:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  432913:	c7 81 18 01 00 00 01 	mov    DWORD PTR [ecx+0x118],0x1
  43291a:	00 00 00 
  43291d:	83 ff 02             	cmp    edi,0x2
  432920:	75 1a                	jne    0x43293c
  432922:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432928:	57                   	push   edi
  432929:	e8 32 0d 01 00       	call   0x443660
  43292e:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  432934:	89 ba 18 01 00 00    	mov    DWORD PTR [edx+0x118],edi
  43293a:	eb 23                	jmp    0x43295f
  43293c:	83 ff 03             	cmp    edi,0x3
  43293f:	75 19                	jne    0x43295a
  432941:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432947:	57                   	push   edi
  432948:	e8 13 0d 01 00       	call   0x443660
  43294d:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  432952:	89 b8 18 01 00 00    	mov    DWORD PTR [eax+0x118],edi
  432958:	eb 05                	jmp    0x43295f
  43295a:	83 ff 04             	cmp    edi,0x4
  43295d:	74 0b                	je     0x43296a
  43295f:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  432965:	e8 d6 fa fd ff       	call   0x412440
  43296a:	8a 86 40 02 00 00    	mov    al,BYTE PTR [esi+0x240]
  432970:	84 c0                	test   al,al
  432972:	0f 84 a2 00 00 00    	je     0x432a1a
  432978:	a1 c8 fc 89 00       	mov    eax,ds:0x89fcc8
  43297d:	8a 88 79 05 00 00    	mov    cl,BYTE PTR [eax+0x579]
  432983:	84 c9                	test   cl,cl
  432985:	0f 85 8f 00 00 00    	jne    0x432a1a
  43298b:	8b 88 00 06 00 00    	mov    ecx,DWORD PTR [eax+0x600]
  432991:	8a 81 79 05 00 00    	mov    al,BYTE PTR [ecx+0x579]
  432997:	84 c0                	test   al,al
  432999:	75 7f                	jne    0x432a1a
  43299b:	8a 86 48 02 00 00    	mov    al,BYTE PTR [esi+0x248]
  4329a1:	84 c0                	test   al,al
  4329a3:	75 42                	jne    0x4329e7
  4329a5:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  4329ab:	6a 01                	push   0x1
  4329ad:	6a 00                	push   0x0
  4329af:	68 40 ac 27 00       	push   0x27ac40
  4329b4:	68 f6 00 00 00       	push   0xf6
  4329b9:	68 25 01 00 00       	push   0x125
  4329be:	68 f6 00 00 00       	push   0xf6
  4329c3:	68 25 01 00 00       	push   0x125
  4329c8:	68 98 a4 46 00       	push   0x46a498
  4329cd:	e8 fe aa ff ff       	call   0x42d4d0
  4329d2:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  4329d8:	6a 00                	push   0x0
  4329da:	50                   	push   eax
  4329db:	e8 00 ad ff ff       	call   0x42d6e0
  4329e0:	c6 86 48 02 00 00 01 	mov    BYTE PTR [esi+0x248],0x1
  4329e7:	8b 86 44 02 00 00    	mov    eax,DWORD PTR [esi+0x244]
  4329ed:	85 c0                	test   eax,eax
  4329ef:	7f 22                	jg     0x432a13
  4329f1:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  4329f7:	e8 64 f5 00 00       	call   0x441f60
  4329fc:	85 f6                	test   esi,esi
  4329fe:	74 1a                	je     0x432a1a
  432a00:	8b ce                	mov    ecx,esi
  432a02:	e8 29 f7 ff ff       	call   0x432130
  432a07:	56                   	push   esi
  432a08:	e8 43 eb 01 00       	call   0x451550
  432a0d:	83 c4 04             	add    esp,0x4
  432a10:	5f                   	pop    edi
  432a11:	5e                   	pop    esi
  432a12:	c3                   	ret
  432a13:	48                   	dec    eax
  432a14:	89 86 44 02 00 00    	mov    DWORD PTR [esi+0x244],eax
  432a1a:	5f                   	pop    edi
  432a1b:	5e                   	pop    esi
  432a1c:	c3                   	ret
  432a1d:	90                   	nop
  432a1e:	90                   	nop
  432a1f:	90                   	nop
  432a20:	8b 81 28 02 00 00    	mov    eax,DWORD PTR [ecx+0x228]
  432a26:	83 ec 14             	sub    esp,0x14
  432a29:	8d 4c 24 00          	lea    ecx,[esp+0x0]
  432a2d:	50                   	push   eax
  432a2e:	51                   	push   ecx
  432a2f:	8b 0d 4c 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c4c
  432a35:	e8 56 b8 00 00       	call   0x43e290
  432a3a:	83 c4 14             	add    esp,0x14
  432a3d:	c3                   	ret
  432a3e:	90                   	nop
  432a3f:	90                   	nop
  432a40:	a0 38 3d 8c 00       	mov    al,ds:0x8c3d38
  432a45:	8b d1                	mov    edx,ecx
  432a47:	84 c0                	test   al,al
  432a49:	75 2b                	jne    0x432a76
  432a4b:	57                   	push   edi
  432a4c:	b9 4b 00 00 00       	mov    ecx,0x4b
  432a51:	33 c0                	xor    eax,eax
  432a53:	bf 08 3c 8c 00       	mov    edi,0x8c3c08
  432a58:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  432a5a:	b9 84 4e 00 00       	mov    ecx,0x4e84
  432a5f:	bf f8 01 8b 00       	mov    edi,0x8b01f8
  432a64:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  432a66:	a3 3c 3d 8c 00       	mov    ds:0x8c3d3c,eax
  432a6b:	c6 05 38 3d 8c 00 01 	mov    BYTE PTR ds:0x8c3d38,0x1
  432a72:	8b c2                	mov    eax,edx
  432a74:	5f                   	pop    edi
  432a75:	c3                   	ret
  432a76:	8b c2                	mov    eax,edx
  432a78:	c3                   	ret
  432a79:	90                   	nop
  432a7a:	90                   	nop
  432a7b:	90                   	nop
  432a7c:	90                   	nop
  432a7d:	90                   	nop
  432a7e:	90                   	nop
  432a7f:	90                   	nop
  432a80:	53                   	push   ebx
  432a81:	e8 5a 04 00 00       	call   0x432ee0
  432a86:	a1 3c 3d 8c 00       	mov    eax,ds:0x8c3d3c
  432a8b:	33 db                	xor    ebx,ebx
  432a8d:	3b c3                	cmp    eax,ebx
  432a8f:	74 72                	je     0x432b03
  432a91:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432a97:	56                   	push   esi
  432a98:	68 fc ac 46 00       	push   0x46acfc
  432a9d:	e8 ce f4 ff ff       	call   0x431f70
  432aa2:	be f8 01 8b 00       	mov    esi,0x8b01f8
  432aa7:	39 1e                	cmp    DWORD PTR [esi],ebx
  432aa9:	7e 31                	jle    0x432adc
  432aab:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432ab1:	8d 46 04             	lea    eax,[esi+0x4]
  432ab4:	50                   	push   eax
  432ab5:	e8 b6 f4 ff ff       	call   0x431f70
  432aba:	89 1e                	mov    DWORD PTR [esi],ebx
  432abc:	8b 8e 08 01 00 00    	mov    ecx,DWORD PTR [esi+0x108]
  432ac2:	51                   	push   ecx
  432ac3:	e8 a9 f6 01 00       	call   0x452171
  432ac8:	a1 3c 3d 8c 00       	mov    eax,ds:0x8c3d3c
  432acd:	83 c4 04             	add    esp,0x4
  432ad0:	48                   	dec    eax
  432ad1:	89 9e 08 01 00 00    	mov    DWORD PTR [esi+0x108],ebx
  432ad7:	a3 3c 3d 8c 00       	mov    ds:0x8c3d3c,eax
  432adc:	81 c6 0c 01 00 00    	add    esi,0x10c
  432ae2:	81 fe 08 3c 8c 00    	cmp    esi,0x8c3c08
  432ae8:	7c bd                	jl     0x432aa7
  432aea:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432af0:	68 d4 ac 46 00       	push   0x46acd4
  432af5:	e8 76 f4 ff ff       	call   0x431f70
  432afa:	88 1d 38 3d 8c 00    	mov    BYTE PTR ds:0x8c3d38,bl
  432b00:	5e                   	pop    esi
  432b01:	5b                   	pop    ebx
  432b02:	c3                   	ret
  432b03:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432b09:	68 a4 ac 46 00       	push   0x46aca4
  432b0e:	e8 5d f4 ff ff       	call   0x431f70
  432b13:	88 1d 38 3d 8c 00    	mov    BYTE PTR ds:0x8c3d38,bl
  432b19:	5b                   	pop    ebx
  432b1a:	c3                   	ret
  432b1b:	90                   	nop
  432b1c:	90                   	nop
  432b1d:	90                   	nop
  432b1e:	90                   	nop
  432b1f:	90                   	nop
  432b20:	6a ff                	push   0xffffffff
  432b22:	68 98 ed 45 00       	push   0x45ed98
  432b27:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  432b2d:	50                   	push   eax
  432b2e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  432b35:	83 ec 18             	sub    esp,0x18
  432b38:	53                   	push   ebx
  432b39:	55                   	push   ebp
  432b3a:	56                   	push   esi
  432b3b:	8b f1                	mov    esi,ecx
  432b3d:	57                   	push   edi
  432b3e:	6a 01                	push   0x1
  432b40:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  432b44:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  432b48:	e8 a3 eb 00 00       	call   0x4416f0
  432b4d:	8b 54 24 38          	mov    edx,DWORD PTR [esp+0x38]
  432b51:	c7 06 ff ff ff ff    	mov    DWORD PTR [esi],0xffffffff
  432b57:	c7 44 24 30 00 00 00 	mov    DWORD PTR [esp+0x30],0x0
  432b5e:	00 
  432b5f:	be 80 9f 46 00       	mov    esi,0x469f80
  432b64:	8b c2                	mov    eax,edx
  432b66:	8a 18                	mov    bl,BYTE PTR [eax]
  432b68:	8a cb                	mov    cl,bl
  432b6a:	3a 1e                	cmp    bl,BYTE PTR [esi]
  432b6c:	75 1c                	jne    0x432b8a
  432b6e:	84 c9                	test   cl,cl
  432b70:	74 14                	je     0x432b86
  432b72:	8a 58 01             	mov    bl,BYTE PTR [eax+0x1]
  432b75:	8a cb                	mov    cl,bl
  432b77:	3a 5e 01             	cmp    bl,BYTE PTR [esi+0x1]
  432b7a:	75 0e                	jne    0x432b8a
  432b7c:	83 c0 02             	add    eax,0x2
  432b7f:	83 c6 02             	add    esi,0x2
  432b82:	84 c9                	test   cl,cl
  432b84:	75 e0                	jne    0x432b66
  432b86:	33 c0                	xor    eax,eax
  432b88:	eb 05                	jmp    0x432b8f
  432b8a:	1b c0                	sbb    eax,eax
  432b8c:	83 d8 ff             	sbb    eax,0xffffffff
  432b8f:	85 c0                	test   eax,eax
  432b91:	75 18                	jne    0x432bab
  432b93:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  432b97:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  432b9e:	ff 
  432b9f:	e8 6c eb 00 00       	call   0x441710
  432ba4:	33 c0                	xor    eax,eax
  432ba6:	e9 08 02 00 00       	jmp    0x432db3
  432bab:	85 d2                	test   edx,edx
  432bad:	75 18                	jne    0x432bc7
  432baf:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  432bb3:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  432bba:	ff 
  432bbb:	e8 50 eb 00 00       	call   0x441710
  432bc0:	33 c0                	xor    eax,eax
  432bc2:	e9 ec 01 00 00       	jmp    0x432db3
  432bc7:	33 ff                	xor    edi,edi
  432bc9:	bd fc 01 8b 00       	mov    ebp,0x8b01fc
  432bce:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  432bd1:	85 c0                	test   eax,eax
  432bd3:	7e 31                	jle    0x432c06
  432bd5:	8b f5                	mov    esi,ebp
  432bd7:	8b c2                	mov    eax,edx
  432bd9:	8a 18                	mov    bl,BYTE PTR [eax]
  432bdb:	8a cb                	mov    cl,bl
  432bdd:	3a 1e                	cmp    bl,BYTE PTR [esi]
  432bdf:	75 1c                	jne    0x432bfd
  432be1:	84 c9                	test   cl,cl
  432be3:	74 14                	je     0x432bf9
  432be5:	8a 58 01             	mov    bl,BYTE PTR [eax+0x1]
  432be8:	8a cb                	mov    cl,bl
  432bea:	3a 5e 01             	cmp    bl,BYTE PTR [esi+0x1]
  432bed:	75 0e                	jne    0x432bfd
  432bef:	83 c0 02             	add    eax,0x2
  432bf2:	83 c6 02             	add    esi,0x2
  432bf5:	84 c9                	test   cl,cl
  432bf7:	75 e0                	jne    0x432bd9
  432bf9:	33 c0                	xor    eax,eax
  432bfb:	eb 05                	jmp    0x432c02
  432bfd:	1b c0                	sbb    eax,eax
  432bff:	83 d8 ff             	sbb    eax,0xffffffff
  432c02:	85 c0                	test   eax,eax
  432c04:	74 3d                	je     0x432c43
  432c06:	81 c5 0c 01 00 00    	add    ebp,0x10c
  432c0c:	47                   	inc    edi
  432c0d:	81 fd 0c 3c 8c 00    	cmp    ebp,0x8c3c0c
  432c13:	7c b9                	jl     0x432bce
  432c15:	81 3d 3c 3d 8c 00 2c 	cmp    DWORD PTR ds:0x8c3d3c,0x12c
  432c1c:	01 00 00 
  432c1f:	0f 8d 6f 01 00 00    	jge    0x432d94
  432c25:	33 ed                	xor    ebp,ebp
  432c27:	b8 f8 01 8b 00       	mov    eax,0x8b01f8
  432c2c:	83 38 00             	cmp    DWORD PTR [eax],0x0
  432c2f:	74 71                	je     0x432ca2
  432c31:	05 0c 01 00 00       	add    eax,0x10c
  432c36:	45                   	inc    ebp
  432c37:	3d 08 3c 8c 00       	cmp    eax,0x8c3c08
  432c3c:	7c ee                	jl     0x432c2c
  432c3e:	e9 51 01 00 00       	jmp    0x432d94
  432c43:	8a 87 08 3c 8c 00    	mov    al,BYTE PTR [edi+0x8c3c08]
  432c49:	84 c0                	test   al,al
  432c4b:	74 17                	je     0x432c64
  432c4d:	8b c7                	mov    eax,edi
  432c4f:	c1 e0 05             	shl    eax,0x5
  432c52:	03 c7                	add    eax,edi
  432c54:	8d 04 47             	lea    eax,[edi+eax*2]
  432c57:	c1 e0 02             	shl    eax,0x2
  432c5a:	8b 88 f8 01 8b 00    	mov    ecx,DWORD PTR [eax+0x8b01f8]
  432c60:	85 c9                	test   ecx,ecx
  432c62:	75 13                	jne    0x432c77
  432c64:	8b cf                	mov    ecx,edi
  432c66:	c1 e1 05             	shl    ecx,0x5
  432c69:	03 cf                	add    ecx,edi
  432c6b:	8d 04 4f             	lea    eax,[edi+ecx*2]
  432c6e:	c1 e0 02             	shl    eax,0x2
  432c71:	ff 80 f8 01 8b 00    	inc    DWORD PTR [eax+0x8b01f8]
  432c77:	8a 54 24 40          	mov    dl,BYTE PTR [esp+0x40]
  432c7b:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  432c7f:	88 97 08 3c 8c 00    	mov    BYTE PTR [edi+0x8c3c08],dl
  432c85:	89 39                	mov    DWORD PTR [ecx],edi
  432c87:	8b 4c 24 3c          	mov    ecx,DWORD PTR [esp+0x3c]
  432c8b:	85 c9                	test   ecx,ecx
  432c8d:	74 08                	je     0x432c97
  432c8f:	8b 90 fc 02 8b 00    	mov    edx,DWORD PTR [eax+0x8b02fc]
  432c95:	89 11                	mov    DWORD PTR [ecx],edx
  432c97:	8b b0 00 03 8b 00    	mov    esi,DWORD PTR [eax+0x8b0300]
  432c9d:	e9 fe 00 00 00       	jmp    0x432da0
  432ca2:	8b 7c 24 38          	mov    edi,DWORD PTR [esp+0x38]
  432ca6:	8b c5                	mov    eax,ebp
  432ca8:	c1 e0 05             	shl    eax,0x5
  432cab:	03 c5                	add    eax,ebp
  432cad:	57                   	push   edi
  432cae:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  432cb2:	8d 5c 45 00          	lea    ebx,[ebp+eax*2+0x0]
  432cb6:	c1 e3 02             	shl    ebx,0x2
  432cb9:	e8 82 ea 00 00       	call   0x441740
  432cbe:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  432cc2:	89 83 00 03 8b 00    	mov    DWORD PTR [ebx+0x8b0300],eax
  432cc8:	e8 63 a1 ff ff       	call   0x42ce30
  432ccd:	89 83 fc 02 8b 00    	mov    DWORD PTR [ebx+0x8b02fc],eax
  432cd3:	8b 83 00 03 8b 00    	mov    eax,DWORD PTR [ebx+0x8b0300]
  432cd9:	85 c0                	test   eax,eax
  432cdb:	75 42                	jne    0x432d1f
  432cdd:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432ce3:	68 24 ad 46 00       	push   0x46ad24
  432ce8:	e8 83 f2 ff ff       	call   0x431f70
  432ced:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432cf3:	57                   	push   edi
  432cf4:	e8 77 f2 ff ff       	call   0x431f70
  432cf9:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  432cfd:	85 c0                	test   eax,eax
  432cff:	74 06                	je     0x432d07
  432d01:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
  432d07:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  432d0b:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  432d12:	ff 
  432d13:	e8 f8 e9 00 00       	call   0x441710
  432d18:	33 c0                	xor    eax,eax
  432d1a:	e9 94 00 00 00       	jmp    0x432db3
  432d1f:	83 c9 ff             	or     ecx,0xffffffff
  432d22:	33 c0                	xor    eax,eax
  432d24:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  432d26:	f7 d1                	not    ecx
  432d28:	2b f9                	sub    edi,ecx
  432d2a:	8d 93 fc 01 8b 00    	lea    edx,[ebx+0x8b01fc]
  432d30:	8b c1                	mov    eax,ecx
  432d32:	8b f7                	mov    esi,edi
  432d34:	8b fa                	mov    edi,edx
  432d36:	8b 15 3c 3d 8c 00    	mov    edx,DWORD PTR ds:0x8c3d3c
  432d3c:	c1 e9 02             	shr    ecx,0x2
  432d3f:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  432d41:	8b c8                	mov    ecx,eax
  432d43:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  432d47:	83 e1 03             	and    ecx,0x3
  432d4a:	42                   	inc    edx
  432d4b:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  432d4d:	8a 4c 24 40          	mov    cl,BYTE PTR [esp+0x40]
  432d51:	89 15 3c 3d 8c 00    	mov    DWORD PTR ds:0x8c3d3c,edx
  432d57:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  432d5b:	c7 83 f8 01 8b 00 01 	mov    DWORD PTR [ebx+0x8b01f8],0x1
  432d62:	00 00 00 
  432d65:	88 8d 08 3c 8c 00    	mov    BYTE PTR [ebp+0x8c3c08],cl
  432d6b:	85 c0                	test   eax,eax
  432d6d:	89 2a                	mov    DWORD PTR [edx],ebp
  432d6f:	74 08                	je     0x432d79
  432d71:	8b 8b fc 02 8b 00    	mov    ecx,DWORD PTR [ebx+0x8b02fc]
  432d77:	89 08                	mov    DWORD PTR [eax],ecx
  432d79:	8b 9b 00 03 8b 00    	mov    ebx,DWORD PTR [ebx+0x8b0300]
  432d7f:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  432d83:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  432d8a:	ff 
  432d8b:	e8 80 e9 00 00       	call   0x441710
  432d90:	8b c3                	mov    eax,ebx
  432d92:	eb 1f                	jmp    0x432db3
  432d94:	52                   	push   edx
  432d95:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  432d99:	e8 a2 e9 00 00       	call   0x441740
  432d9e:	8b f0                	mov    esi,eax
  432da0:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  432da4:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  432dab:	ff 
  432dac:	e8 5f e9 00 00       	call   0x441710
  432db1:	8b c6                	mov    eax,esi
  432db3:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  432db7:	5f                   	pop    edi
  432db8:	5e                   	pop    esi
  432db9:	5d                   	pop    ebp
  432dba:	5b                   	pop    ebx
  432dbb:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432dc2:	83 c4 24             	add    esp,0x24
  432dc5:	c2 0c 00             	ret    0xc
  432dc8:	90                   	nop
  432dc9:	90                   	nop
  432dca:	90                   	nop
  432dcb:	90                   	nop
  432dcc:	90                   	nop
  432dcd:	90                   	nop
  432dce:	90                   	nop
  432dcf:	90                   	nop
  432dd0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  432dd4:	56                   	push   esi
  432dd5:	85 c0                	test   eax,eax
  432dd7:	7c 43                	jl     0x432e1c
  432dd9:	3d 2c 01 00 00       	cmp    eax,0x12c
  432dde:	7d 3c                	jge    0x432e1c
  432de0:	8b c8                	mov    ecx,eax
  432de2:	c1 e1 05             	shl    ecx,0x5
  432de5:	03 c8                	add    ecx,eax
  432de7:	8d 34 48             	lea    esi,[eax+ecx*2]
  432dea:	c1 e6 02             	shl    esi,0x2
  432ded:	8b 86 f8 01 8b 00    	mov    eax,DWORD PTR [esi+0x8b01f8]
  432df3:	85 c0                	test   eax,eax
  432df5:	7e 25                	jle    0x432e1c
  432df7:	8b 96 00 03 8b 00    	mov    edx,DWORD PTR [esi+0x8b0300]
  432dfd:	52                   	push   edx
  432dfe:	e8 5d e8 00 00       	call   0x441660
  432e03:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  432e07:	50                   	push   eax
  432e08:	89 86 00 03 8b 00    	mov    DWORD PTR [esi+0x8b0300],eax
  432e0e:	e8 ed f7 01 00       	call   0x452600
  432e13:	83 c4 08             	add    esp,0x8
  432e16:	89 86 fc 02 8b 00    	mov    DWORD PTR [esi+0x8b02fc],eax
  432e1c:	5e                   	pop    esi
  432e1d:	c2 08 00             	ret    0x8
  432e20:	a1 3c 3d 8c 00       	mov    eax,ds:0x8c3d3c
  432e25:	56                   	push   esi
  432e26:	85 c0                	test   eax,eax
  432e28:	74 33                	je     0x432e5d
  432e2a:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  432e2e:	85 d2                	test   edx,edx
  432e30:	74 2b                	je     0x432e5d
  432e32:	33 c9                	xor    ecx,ecx
  432e34:	b8 00 03 8b 00       	mov    eax,0x8b0300
  432e39:	8b b0 f8 fe ff ff    	mov    esi,DWORD PTR [eax-0x108]
  432e3f:	85 f6                	test   esi,esi
  432e41:	7e 04                	jle    0x432e47
  432e43:	39 10                	cmp    DWORD PTR [eax],edx
  432e45:	74 1a                	je     0x432e61
  432e47:	05 0c 01 00 00       	add    eax,0x10c
  432e4c:	41                   	inc    ecx
  432e4d:	3d 10 3d 8c 00       	cmp    eax,0x8c3d10
  432e52:	7c e5                	jl     0x432e39
  432e54:	52                   	push   edx
  432e55:	e8 17 f3 01 00       	call   0x452171
  432e5a:	83 c4 04             	add    esp,0x4
  432e5d:	5e                   	pop    esi
  432e5e:	c2 08 00             	ret    0x8
  432e61:	8b c1                	mov    eax,ecx
  432e63:	c1 e0 05             	shl    eax,0x5
  432e66:	03 c1                	add    eax,ecx
  432e68:	8d 34 41             	lea    esi,[ecx+eax*2]
  432e6b:	c1 e6 02             	shl    esi,0x2
  432e6e:	8b 86 f8 01 8b 00    	mov    eax,DWORD PTR [esi+0x8b01f8]
  432e74:	48                   	dec    eax
  432e75:	89 86 f8 01 8b 00    	mov    DWORD PTR [esi+0x8b01f8],eax
  432e7b:	75 e0                	jne    0x432e5d
  432e7d:	8b 86 00 03 8b 00    	mov    eax,DWORD PTR [esi+0x8b0300]
  432e83:	85 c0                	test   eax,eax
  432e85:	74 09                	je     0x432e90
  432e87:	50                   	push   eax
  432e88:	e8 e4 f2 01 00       	call   0x452171
  432e8d:	83 c4 04             	add    esp,0x4
  432e90:	a1 3c 3d 8c 00       	mov    eax,ds:0x8c3d3c
  432e95:	c7 86 00 03 8b 00 00 	mov    DWORD PTR [esi+0x8b0300],0x0
  432e9c:	00 00 00 
  432e9f:	48                   	dec    eax
  432ea0:	5e                   	pop    esi
  432ea1:	a3 3c 3d 8c 00       	mov    ds:0x8c3d3c,eax
  432ea6:	c2 08 00             	ret    0x8
  432ea9:	90                   	nop
  432eaa:	90                   	nop
  432eab:	90                   	nop
  432eac:	90                   	nop
  432ead:	90                   	nop
  432eae:	90                   	nop
  432eaf:	90                   	nop
  432eb0:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  432eb4:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  432eb8:	6a 00                	push   0x0
  432eba:	50                   	push   eax
  432ebb:	52                   	push   edx
  432ebc:	e8 5f fc ff ff       	call   0x432b20
  432ec1:	c2 08 00             	ret    0x8
  432ec4:	90                   	nop
  432ec5:	90                   	nop
  432ec6:	90                   	nop
  432ec7:	90                   	nop
  432ec8:	90                   	nop
  432ec9:	90                   	nop
  432eca:	90                   	nop
  432ecb:	90                   	nop
  432ecc:	90                   	nop
  432ecd:	90                   	nop
  432ece:	90                   	nop
  432ecf:	90                   	nop
  432ed0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  432ed4:	6a 00                	push   0x0
  432ed6:	50                   	push   eax
  432ed7:	e8 44 ff ff ff       	call   0x432e20
  432edc:	c2 04 00             	ret    0x4
  432edf:	90                   	nop
  432ee0:	55                   	push   ebp
  432ee1:	56                   	push   esi
  432ee2:	57                   	push   edi
  432ee3:	8b e9                	mov    ebp,ecx
  432ee5:	33 ff                	xor    edi,edi
  432ee7:	be 00 03 8b 00       	mov    esi,0x8b0300
  432eec:	8a 87 08 3c 8c 00    	mov    al,BYTE PTR [edi+0x8c3c08]
  432ef2:	84 c0                	test   al,al
  432ef4:	74 13                	je     0x432f09
  432ef6:	8b 06                	mov    eax,DWORD PTR [esi]
  432ef8:	6a 01                	push   0x1
  432efa:	50                   	push   eax
  432efb:	8b cd                	mov    ecx,ebp
  432efd:	e8 1e ff ff ff       	call   0x432e20
  432f02:	c6 87 08 3c 8c 00 00 	mov    BYTE PTR [edi+0x8c3c08],0x0
  432f09:	81 c6 0c 01 00 00    	add    esi,0x10c
  432f0f:	47                   	inc    edi
  432f10:	81 fe 10 3d 8c 00    	cmp    esi,0x8c3d10
  432f16:	7c d4                	jl     0x432eec
  432f18:	5f                   	pop    edi
  432f19:	5e                   	pop    esi
  432f1a:	5d                   	pop    ebp
  432f1b:	c3                   	ret
  432f1c:	90                   	nop
  432f1d:	90                   	nop
  432f1e:	90                   	nop
  432f1f:	90                   	nop
  432f20:	e8 0b 00 00 00       	call   0x432f30
  432f25:	e9 16 00 00 00       	jmp    0x432f40
  432f2a:	90                   	nop
  432f2b:	90                   	nop
  432f2c:	90                   	nop
  432f2d:	90                   	nop
  432f2e:	90                   	nop
  432f2f:	90                   	nop
  432f30:	b9 40 3d 8c 00       	mov    ecx,0x8c3d40
  432f35:	e9 c6 b3 00 00       	jmp    0x43e300
  432f3a:	90                   	nop
  432f3b:	90                   	nop
  432f3c:	90                   	nop
  432f3d:	90                   	nop
  432f3e:	90                   	nop
  432f3f:	90                   	nop
  432f40:	68 50 2f 43 00       	push   0x432f50
  432f45:	e8 eb f5 01 00       	call   0x452535
  432f4a:	59                   	pop    ecx
  432f4b:	c3                   	ret
  432f4c:	90                   	nop
  432f4d:	90                   	nop
  432f4e:	90                   	nop
  432f4f:	90                   	nop
  432f50:	8a 0d a4 40 8c 00    	mov    cl,BYTE PTR ds:0x8c40a4
  432f56:	b0 01                	mov    al,0x1
  432f58:	84 c8                	test   al,cl
  432f5a:	75 12                	jne    0x432f6e
  432f5c:	0a c8                	or     cl,al
  432f5e:	88 0d a4 40 8c 00    	mov    BYTE PTR ds:0x8c40a4,cl
  432f64:	b9 40 3d 8c 00       	mov    ecx,0x8c3d40
  432f69:	e9 82 b4 00 00       	jmp    0x43e3f0
  432f6e:	c3                   	ret
  432f6f:	90                   	nop
  432f70:	56                   	push   esi
  432f71:	8b f1                	mov    esi,ecx
  432f73:	e8 38 00 00 00       	call   0x432fb0
  432f78:	8b c6                	mov    eax,esi
  432f7a:	5e                   	pop    esi
  432f7b:	c3                   	ret
  432f7c:	90                   	nop
  432f7d:	90                   	nop
  432f7e:	90                   	nop
  432f7f:	90                   	nop
  432f80:	56                   	push   esi
  432f81:	8b f1                	mov    esi,ecx
  432f83:	8b 06                	mov    eax,DWORD PTR [esi]
  432f85:	85 c0                	test   eax,eax
  432f87:	74 0f                	je     0x432f98
  432f89:	50                   	push   eax
  432f8a:	e8 e2 f1 01 00       	call   0x452171
  432f8f:	83 c4 04             	add    esp,0x4
  432f92:	c7 06 00 00 00 00    	mov    DWORD PTR [esi],0x0
  432f98:	8b ce                	mov    ecx,esi
  432f9a:	e8 11 00 00 00       	call   0x432fb0
  432f9f:	5e                   	pop    esi
  432fa0:	c3                   	ret
  432fa1:	90                   	nop
  432fa2:	90                   	nop
  432fa3:	90                   	nop
  432fa4:	90                   	nop
  432fa5:	90                   	nop
  432fa6:	90                   	nop
  432fa7:	90                   	nop
  432fa8:	90                   	nop
  432fa9:	90                   	nop
  432faa:	90                   	nop
  432fab:	90                   	nop
  432fac:	90                   	nop
  432fad:	90                   	nop
  432fae:	90                   	nop
  432faf:	90                   	nop
  432fb0:	33 c0                	xor    eax,eax
  432fb2:	c7 41 0c ff 7f 00 00 	mov    DWORD PTR [ecx+0xc],0x7fff
  432fb9:	89 41 28             	mov    DWORD PTR [ecx+0x28],eax
  432fbc:	89 41 10             	mov    DWORD PTR [ecx+0x10],eax
  432fbf:	89 41 14             	mov    DWORD PTR [ecx+0x14],eax
  432fc2:	89 01                	mov    DWORD PTR [ecx],eax
  432fc4:	89 41 04             	mov    DWORD PTR [ecx+0x4],eax
  432fc7:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
  432fca:	89 41 18             	mov    DWORD PTR [ecx+0x18],eax
  432fcd:	a1 c4 40 8c 00       	mov    eax,ds:0x8c40c4
  432fd2:	89 41 2c             	mov    DWORD PTR [ecx+0x2c],eax
  432fd5:	c3                   	ret
  432fd6:	90                   	nop
  432fd7:	90                   	nop
  432fd8:	90                   	nop
  432fd9:	90                   	nop
  432fda:	90                   	nop
  432fdb:	90                   	nop
  432fdc:	90                   	nop
  432fdd:	90                   	nop
  432fde:	90                   	nop
  432fdf:	90                   	nop
  432fe0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  432fe4:	89 41 2c             	mov    DWORD PTR [ecx+0x2c],eax
  432fe7:	c2 04 00             	ret    0x4
  432fea:	90                   	nop
  432feb:	90                   	nop
  432fec:	90                   	nop
  432fed:	90                   	nop
  432fee:	90                   	nop
  432fef:	90                   	nop
  432ff0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  432ff4:	85 c0                	test   eax,eax
  432ff6:	74 1f                	je     0x433017
  432ff8:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  432ffc:	a3 b8 40 8c 00       	mov    ds:0x8c40b8,eax
  433001:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  433005:	89 0d c0 40 8c 00    	mov    DWORD PTR ds:0x8c40c0,ecx
  43300b:	a3 bc 40 8c 00       	mov    ds:0x8c40bc,eax
  433010:	c6 05 b4 40 8c 00 01 	mov    BYTE PTR ds:0x8c40b4,0x1
  433017:	c2 0c 00             	ret    0xc
  43301a:	90                   	nop
  43301b:	90                   	nop
  43301c:	90                   	nop
  43301d:	90                   	nop
  43301e:	90                   	nop
  43301f:	90                   	nop
  433020:	53                   	push   ebx
  433021:	56                   	push   esi
  433022:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  433026:	8b d9                	mov    ebx,ecx
  433028:	85 f6                	test   esi,esi
  43302a:	74 4b                	je     0x433077
  43302c:	57                   	push   edi
  43302d:	8b fe                	mov    edi,esi
  43302f:	83 c9 ff             	or     ecx,0xffffffff
  433032:	33 c0                	xor    eax,eax
  433034:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  433036:	8b 03                	mov    eax,DWORD PTR [ebx]
  433038:	f7 d1                	not    ecx
  43303a:	83 c1 09             	add    ecx,0x9
  43303d:	85 c0                	test   eax,eax
  43303f:	51                   	push   ecx
  433040:	75 0a                	jne    0x43304c
  433042:	e8 b6 f0 01 00       	call   0x4520fd
  433047:	83 c4 04             	add    esp,0x4
  43304a:	eb 09                	jmp    0x433055
  43304c:	50                   	push   eax
  43304d:	e8 8b ef 01 00       	call   0x451fdd
  433052:	83 c4 08             	add    esp,0x8
  433055:	89 03                	mov    DWORD PTR [ebx],eax
  433057:	8b fe                	mov    edi,esi
  433059:	83 c9 ff             	or     ecx,0xffffffff
  43305c:	33 c0                	xor    eax,eax
  43305e:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  433060:	f7 d1                	not    ecx
  433062:	2b f9                	sub    edi,ecx
  433064:	8b c1                	mov    eax,ecx
  433066:	8b f7                	mov    esi,edi
  433068:	8b 3b                	mov    edi,DWORD PTR [ebx]
  43306a:	c1 e9 02             	shr    ecx,0x2
  43306d:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43306f:	8b c8                	mov    ecx,eax
  433071:	83 e1 03             	and    ecx,0x3
  433074:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  433076:	5f                   	pop    edi
  433077:	5e                   	pop    esi
  433078:	5b                   	pop    ebx
  433079:	c2 04 00             	ret    0x4
  43307c:	90                   	nop
  43307d:	90                   	nop
  43307e:	90                   	nop
  43307f:	90                   	nop
  433080:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  433084:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  433088:	89 41 04             	mov    DWORD PTR [ecx+0x4],eax
  43308b:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  43308f:	50                   	push   eax
  433090:	89 51 08             	mov    DWORD PTR [ecx+0x8],edx
  433093:	e8 88 ff ff ff       	call   0x433020
  433098:	c2 0c 00             	ret    0xc
  43309b:	90                   	nop
  43309c:	90                   	nop
  43309d:	90                   	nop
  43309e:	90                   	nop
  43309f:	90                   	nop
  4330a0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4330a4:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  4330a8:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  4330ab:	89 51 10             	mov    DWORD PTR [ecx+0x10],edx
  4330ae:	c2 08 00             	ret    0x8
  4330b1:	90                   	nop
  4330b2:	90                   	nop
  4330b3:	90                   	nop
  4330b4:	90                   	nop
  4330b5:	90                   	nop
  4330b6:	90                   	nop
  4330b7:	90                   	nop
  4330b8:	90                   	nop
  4330b9:	90                   	nop
  4330ba:	90                   	nop
  4330bb:	90                   	nop
  4330bc:	90                   	nop
  4330bd:	90                   	nop
  4330be:	90                   	nop
  4330bf:	90                   	nop
  4330c0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4330c4:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  4330c8:	89 41 04             	mov    DWORD PTR [ecx+0x4],eax
  4330cb:	89 51 08             	mov    DWORD PTR [ecx+0x8],edx
  4330ce:	c2 08 00             	ret    0x8
  4330d1:	90                   	nop
  4330d2:	90                   	nop
  4330d3:	90                   	nop
  4330d4:	90                   	nop
  4330d5:	90                   	nop
  4330d6:	90                   	nop
  4330d7:	90                   	nop
  4330d8:	90                   	nop
  4330d9:	90                   	nop
  4330da:	90                   	nop
  4330db:	90                   	nop
  4330dc:	90                   	nop
  4330dd:	90                   	nop
  4330de:	90                   	nop
  4330df:	90                   	nop
  4330e0:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  4330e4:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  4330e7:	89 02                	mov    DWORD PTR [edx],eax
  4330e9:	8b 41 08             	mov    eax,DWORD PTR [ecx+0x8]
  4330ec:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  4330f0:	89 01                	mov    DWORD PTR [ecx],eax
  4330f2:	c2 08 00             	ret    0x8
  4330f5:	90                   	nop
  4330f6:	90                   	nop
  4330f7:	90                   	nop
  4330f8:	90                   	nop
  4330f9:	90                   	nop
  4330fa:	90                   	nop
  4330fb:	90                   	nop
  4330fc:	90                   	nop
  4330fd:	90                   	nop
  4330fe:	90                   	nop
  4330ff:	90                   	nop
  433100:	83 ec 50             	sub    esp,0x50
  433103:	a0 b4 40 8c 00       	mov    al,ds:0x8c40b4
  433108:	53                   	push   ebx
  433109:	55                   	push   ebp
  43310a:	33 ed                	xor    ebp,ebp
  43310c:	33 db                	xor    ebx,ebx
  43310e:	56                   	push   esi
  43310f:	84 c0                	test   al,al
  433111:	57                   	push   edi
  433112:	8b f1                	mov    esi,ecx
  433114:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  433118:	89 6c 24 14          	mov    DWORD PTR [esp+0x14],ebp
  43311c:	89 6c 24 10          	mov    DWORD PTR [esp+0x10],ebp
  433120:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  433124:	0f 84 05 02 00 00    	je     0x43332f
  43312a:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  43312d:	3b c5                	cmp    eax,ebp
  43312f:	0f 84 fa 01 00 00    	je     0x43332f
  433135:	39 2e                	cmp    DWORD PTR [esi],ebp
  433137:	0f 84 f2 01 00 00    	je     0x43332f
  43313d:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  433140:	8b 56 0c             	mov    edx,DWORD PTR [esi+0xc]
  433143:	89 4c 24 20          	mov    DWORD PTR [esp+0x20],ecx
  433147:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  43314a:	0f af 08             	imul   ecx,DWORD PTR [eax]
  43314d:	89 50 0c             	mov    DWORD PTR [eax+0xc],edx
  433150:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  433153:	89 4c 24 28          	mov    DWORD PTR [esp+0x28],ecx
  433157:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  43315a:	89 48 10             	mov    DWORD PTR [eax+0x10],ecx
  43315d:	8b 3e                	mov    edi,DWORD PTR [esi]
  43315f:	83 c9 ff             	or     ecx,0xffffffff
  433162:	33 c0                	xor    eax,eax
  433164:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  433166:	f7 d1                	not    ecx
  433168:	49                   	dec    ecx
  433169:	3b cd                	cmp    ecx,ebp
  43316b:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  43316f:	0f 8e ba 01 00 00    	jle    0x43332f
  433175:	eb 02                	jmp    0x433179
  433177:	33 ed                	xor    ebp,ebp
  433179:	8b 16                	mov    edx,DWORD PTR [esi]
  43317b:	6a 02                	push   0x2
  43317d:	03 d3                	add    edx,ebx
  43317f:	68 40 ad 46 00       	push   0x46ad40
  433184:	52                   	push   edx
  433185:	e8 36 9c 02 00       	call   0x45cdc0
  43318a:	83 c4 0c             	add    esp,0xc
  43318d:	85 c0                	test   eax,eax
  43318f:	75 31                	jne    0x4331c2
  433191:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  433195:	8b 4e 20             	mov    ecx,DWORD PTR [esi+0x20]
  433198:	83 c3 02             	add    ebx,0x2
  43319b:	40                   	inc    eax
  43319c:	3b c1                	cmp    eax,ecx
  43319e:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4331a2:	7e 0a                	jle    0x4331ae
  4331a4:	89 6c 24 10          	mov    DWORD PTR [esp+0x10],ebp
  4331a8:	89 6c 24 14          	mov    DWORD PTR [esp+0x14],ebp
  4331ac:	eb 6b                	jmp    0x433219
  4331ae:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  4331b2:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  4331b6:	03 c8                	add    ecx,eax
  4331b8:	89 6c 24 14          	mov    DWORD PTR [esp+0x14],ebp
  4331bc:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  4331c0:	eb 57                	jmp    0x433219
  4331c2:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4331c4:	6a 02                	push   0x2
  4331c6:	03 cb                	add    ecx,ebx
  4331c8:	68 3c ad 46 00       	push   0x46ad3c
  4331cd:	51                   	push   ecx
  4331ce:	e8 ed 9b 02 00       	call   0x45cdc0
  4331d3:	83 c4 0c             	add    esp,0xc
  4331d6:	85 c0                	test   eax,eax
  4331d8:	75 3f                	jne    0x433219
  4331da:	8b 06                	mov    eax,DWORD PTR [esi]
  4331dc:	83 c3 02             	add    ebx,0x2
  4331df:	8d 54 24 2c          	lea    edx,[esp+0x2c]
  4331e3:	03 c3                	add    eax,ebx
  4331e5:	52                   	push   edx
  4331e6:	68 a8 81 46 00       	push   0x4681a8
  4331eb:	50                   	push   eax
  4331ec:	e8 5d f2 01 00       	call   0x45244e
  4331f1:	6a 10                	push   0x10
  4331f3:	8d 4c 24 3c          	lea    ecx,[esp+0x3c]
  4331f7:	55                   	push   ebp
  4331f8:	51                   	push   ecx
  4331f9:	e8 2b f4 01 00       	call   0x452629
  4331fe:	8b d0                	mov    edx,eax
  433200:	8d 7c 24 44          	lea    edi,[esp+0x44]
  433204:	83 c9 ff             	or     ecx,0xffffffff
  433207:	33 c0                	xor    eax,eax
  433209:	83 c4 18             	add    esp,0x18
  43320c:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  43320e:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  433211:	f7 d1                	not    ecx
  433213:	49                   	dec    ecx
  433214:	89 50 0c             	mov    DWORD PTR [eax+0xc],edx
  433217:	03 d9                	add    ebx,ecx
  433219:	8b 7e 04             	mov    edi,DWORD PTR [esi+0x4]
  43321c:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  433220:	8b 6e 08             	mov    ebp,DWORD PTR [esi+0x8]
  433223:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  433227:	8a 46 24             	mov    al,BYTE PTR [esi+0x24]
  43322a:	03 fa                	add    edi,edx
  43322c:	03 e9                	add    ebp,ecx
  43322e:	84 c0                	test   al,al
  433230:	74 1c                	je     0x43324e
  433232:	8b 0d 84 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c84
  433238:	8b 15 88 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c88
  43323e:	89 0d ac 40 8c 00    	mov    DWORD PTR ds:0x8c40ac,ecx
  433244:	89 15 b0 40 8c 00    	mov    DWORD PTR ds:0x8c40b0,edx
  43324a:	33 c9                	xor    ecx,ecx
  43324c:	eb 0e                	jmp    0x43325c
  43324e:	33 c9                	xor    ecx,ecx
  433250:	89 0d ac 40 8c 00    	mov    DWORD PTR ds:0x8c40ac,ecx
  433256:	89 0d b0 40 8c 00    	mov    DWORD PTR ds:0x8c40b0,ecx
  43325c:	39 4c 24 24          	cmp    DWORD PTR [esp+0x24],ecx
  433260:	7e 73                	jle    0x4332d5
  433262:	8b 06                	mov    eax,DWORD PTR [esi]
  433264:	03 c3                	add    eax,ebx
  433266:	80 38 80             	cmp    BYTE PTR [eax],0x80
  433269:	77 17                	ja     0x433282
  43326b:	6a 01                	push   0x1
  43326d:	8d 4c 24 30          	lea    ecx,[esp+0x30]
  433271:	50                   	push   eax
  433272:	51                   	push   ecx
  433273:	e8 d8 f0 01 00       	call   0x452350
  433278:	83 c4 0c             	add    esp,0xc
  43327b:	c6 44 24 2d 00       	mov    BYTE PTR [esp+0x2d],0x0
  433280:	eb 15                	jmp    0x433297
  433282:	6a 02                	push   0x2
  433284:	8d 54 24 30          	lea    edx,[esp+0x30]
  433288:	50                   	push   eax
  433289:	52                   	push   edx
  43328a:	e8 c1 f0 01 00       	call   0x452350
  43328f:	83 c4 0c             	add    esp,0xc
  433292:	c6 44 24 2e 00       	mov    BYTE PTR [esp+0x2e],0x0
  433297:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  43329a:	8b 15 b0 40 8c 00    	mov    edx,DWORD PTR ds:0x8c40b0
  4332a0:	50                   	push   eax
  4332a1:	a1 ac 40 8c 00       	mov    eax,ds:0x8c40ac
  4332a6:	8d 4c 24 30          	lea    ecx,[esp+0x30]
  4332aa:	2b ea                	sub    ebp,edx
  4332ac:	8b 15 b8 40 8c 00    	mov    edx,DWORD PTR ds:0x8c40b8
  4332b2:	51                   	push   ecx
  4332b3:	8b 0d bc 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40bc
  4332b9:	6a 01                	push   0x1
  4332bb:	2b f8                	sub    edi,eax
  4332bd:	a1 c0 40 8c 00       	mov    eax,ds:0x8c40c0
  4332c2:	55                   	push   ebp
  4332c3:	57                   	push   edi
  4332c4:	52                   	push   edx
  4332c5:	50                   	push   eax
  4332c6:	51                   	push   ecx
  4332c7:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  4332ca:	e8 81 06 00 00       	call   0x433950
  4332cf:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  4332d3:	33 c9                	xor    ecx,ecx
  4332d5:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4332d9:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  4332dd:	8b 54 24 28          	mov    edx,DWORD PTR [esp+0x28]
  4332e1:	03 c7                	add    eax,edi
  4332e3:	3b c2                	cmp    eax,edx
  4332e5:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4332e9:	7e 28                	jle    0x433313
  4332eb:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4332ef:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  4332f2:	40                   	inc    eax
  4332f3:	3b c2                	cmp    eax,edx
  4332f5:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4332f9:	7e 06                	jle    0x433301
  4332fb:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  4332ff:	eb 0e                	jmp    0x43330f
  433301:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  433305:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  433309:	03 c2                	add    eax,edx
  43330b:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  43330f:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  433313:	8d 7c 24 2c          	lea    edi,[esp+0x2c]
  433317:	83 c9 ff             	or     ecx,0xffffffff
  43331a:	33 c0                	xor    eax,eax
  43331c:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  43331e:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  433322:	f7 d1                	not    ecx
  433324:	49                   	dec    ecx
  433325:	03 d9                	add    ebx,ecx
  433327:	3b d8                	cmp    ebx,eax
  433329:	0f 8c 48 fe ff ff    	jl     0x433177
  43332f:	5f                   	pop    edi
  433330:	5e                   	pop    esi
  433331:	5d                   	pop    ebp
  433332:	5b                   	pop    ebx
  433333:	83 c4 50             	add    esp,0x50
  433336:	c3                   	ret
  433337:	90                   	nop
  433338:	90                   	nop
  433339:	90                   	nop
  43333a:	90                   	nop
  43333b:	90                   	nop
  43333c:	90                   	nop
  43333d:	90                   	nop
  43333e:	90                   	nop
  43333f:	90                   	nop
  433340:	e9 bb fd ff ff       	jmp    0x433100
  433345:	90                   	nop
  433346:	90                   	nop
  433347:	90                   	nop
  433348:	90                   	nop
  433349:	90                   	nop
  43334a:	90                   	nop
  43334b:	90                   	nop
  43334c:	90                   	nop
  43334d:	90                   	nop
  43334e:	90                   	nop
  43334f:	90                   	nop
  433350:	6a ff                	push   0xffffffff
  433352:	68 d0 ed 45 00       	push   0x45edd0
  433357:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43335d:	50                   	push   eax
  43335e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  433365:	51                   	push   ecx
  433366:	53                   	push   ebx
  433367:	55                   	push   ebp
  433368:	56                   	push   esi
  433369:	8b f1                	mov    esi,ecx
  43336b:	57                   	push   edi
  43336c:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  433370:	e8 6b 10 00 00       	call   0x4343e0
  433375:	68 80 2f 43 00       	push   0x432f80
  43337a:	68 70 2f 43 00       	push   0x432f70
  43337f:	8d 9e 38 05 00 00    	lea    ebx,[esi+0x538]
  433385:	6a 32                	push   0x32
  433387:	33 ff                	xor    edi,edi
  433389:	6a 30                	push   0x30
  43338b:	53                   	push   ebx
  43338c:	89 7c 24 30          	mov    DWORD PTR [esp+0x30],edi
  433390:	e8 3a ee 01 00       	call   0x4521cf
  433395:	c6 44 24 1c 01       	mov    BYTE PTR [esp+0x1c],0x1
  43339a:	c7 06 dc 0d 46 00    	mov    DWORD PTR [esi],0x460ddc
  4333a0:	c7 86 98 0e 00 00 30 	mov    DWORD PTR [esi+0xe98],0x13fd30
  4333a7:	fd 13 00 
  4333aa:	c7 86 9c 0e 00 00 80 	mov    DWORD PTR [esi+0xe9c],0x1e8480
  4333b1:	84 1e 00 
  4333b4:	8b eb                	mov    ebp,ebx
  4333b6:	8b cd                	mov    ecx,ebp
  4333b8:	e8 f3 fb ff ff       	call   0x432fb0
  4333bd:	c6 84 3e 75 0f 00 00 	mov    BYTE PTR [esi+edi*1+0xf75],0x0
  4333c4:	00 
  4333c5:	47                   	inc    edi
  4333c6:	83 c5 30             	add    ebp,0x30
  4333c9:	83 ff 32             	cmp    edi,0x32
  4333cc:	7c e8                	jl     0x4333b6
  4333ce:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  4333d3:	8b 48 28             	mov    ecx,DWORD PTR [eax+0x28]
  4333d6:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  4333d9:	8b 79 08             	mov    edi,DWORD PTR [ecx+0x8]
  4333dc:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  4333df:	8d 14 57             	lea    edx,[edi+edx*2]
  4333e2:	52                   	push   edx
  4333e3:	8b 50 6c             	mov    edx,DWORD PTR [eax+0x6c]
  4333e6:	8b 40 68             	mov    eax,DWORD PTR [eax+0x68]
  4333e9:	8d 14 51             	lea    edx,[ecx+edx*2]
  4333ec:	8b cb                	mov    ecx,ebx
  4333ee:	52                   	push   edx
  4333ef:	50                   	push   eax
  4333f0:	e8 fb fb ff ff       	call   0x432ff0
  4333f5:	8b 8e 98 0e 00 00    	mov    ecx,DWORD PTR [esi+0xe98]
  4333fb:	6a 00                	push   0x0
  4333fd:	51                   	push   ecx
  4333fe:	56                   	push   esi
  4333ff:	8b ce                	mov    ecx,esi
  433401:	e8 2a 10 00 00       	call   0x434430
  433406:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  43340a:	c6 86 74 0f 00 00 00 	mov    BYTE PTR [esi+0xf74],0x0
  433411:	8b c6                	mov    eax,esi
  433413:	5f                   	pop    edi
  433414:	5e                   	pop    esi
  433415:	5d                   	pop    ebp
  433416:	5b                   	pop    ebx
  433417:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43341e:	83 c4 10             	add    esp,0x10
  433421:	c3                   	ret
  433422:	90                   	nop
  433423:	90                   	nop
  433424:	90                   	nop
  433425:	90                   	nop
  433426:	90                   	nop
  433427:	90                   	nop
  433428:	90                   	nop
  433429:	90                   	nop
  43342a:	90                   	nop
  43342b:	90                   	nop
  43342c:	90                   	nop
  43342d:	90                   	nop
  43342e:	90                   	nop
  43342f:	90                   	nop
  433430:	6a ff                	push   0xffffffff
  433432:	68 e8 ed 45 00       	push   0x45ede8
  433437:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43343d:	50                   	push   eax
  43343e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  433445:	51                   	push   ecx
  433446:	56                   	push   esi
  433447:	8b f1                	mov    esi,ecx
  433449:	57                   	push   edi
  43344a:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  43344e:	c7 06 dc 0d 46 00    	mov    DWORD PTR [esi],0x460ddc
  433454:	8d be 75 0f 00 00    	lea    edi,[esi+0xf75]
  43345a:	b9 0c 00 00 00       	mov    ecx,0xc
  43345f:	33 c0                	xor    eax,eax
  433461:	68 80 2f 43 00       	push   0x432f80
  433466:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  433468:	66 ab                	stos   WORD PTR es:[edi],ax
  43346a:	6a 32                	push   0x32
  43346c:	8d 86 38 05 00 00    	lea    eax,[esi+0x538]
  433472:	6a 30                	push   0x30
  433474:	50                   	push   eax
  433475:	c7 44 24 24 00 00 00 	mov    DWORD PTR [esp+0x24],0x0
  43347c:	00 
  43347d:	e8 cf ed 01 00       	call   0x452251
  433482:	8b ce                	mov    ecx,esi
  433484:	c7 44 24 14 ff ff ff 	mov    DWORD PTR [esp+0x14],0xffffffff
  43348b:	ff 
  43348c:	e8 7f 0f 00 00       	call   0x434410
  433491:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  433495:	5f                   	pop    edi
  433496:	5e                   	pop    esi
  433497:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43349e:	83 c4 10             	add    esp,0x10
  4334a1:	c3                   	ret
  4334a2:	90                   	nop
  4334a3:	90                   	nop
  4334a4:	90                   	nop
  4334a5:	90                   	nop
  4334a6:	90                   	nop
  4334a7:	90                   	nop
  4334a8:	90                   	nop
  4334a9:	90                   	nop
  4334aa:	90                   	nop
  4334ab:	90                   	nop
  4334ac:	90                   	nop
  4334ad:	90                   	nop
  4334ae:	90                   	nop
  4334af:	90                   	nop
  4334b0:	8b 81 6c 0f 00 00    	mov    eax,DWORD PTR [ecx+0xf6c]
  4334b6:	83 f8 32             	cmp    eax,0x32
  4334b9:	7d 18                	jge    0x4334d3
  4334bb:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  4334bf:	89 94 81 a4 0e 00 00 	mov    DWORD PTR [ecx+eax*4+0xea4],edx
  4334c6:	8b 81 6c 0f 00 00    	mov    eax,DWORD PTR [ecx+0xf6c]
  4334cc:	40                   	inc    eax
  4334cd:	89 81 6c 0f 00 00    	mov    DWORD PTR [ecx+0xf6c],eax
  4334d3:	c2 04 00             	ret    0x4
  4334d6:	90                   	nop
  4334d7:	90                   	nop
  4334d8:	90                   	nop
  4334d9:	90                   	nop
  4334da:	90                   	nop
  4334db:	90                   	nop
  4334dc:	90                   	nop
  4334dd:	90                   	nop
  4334de:	90                   	nop
  4334df:	90                   	nop
  4334e0:	51                   	push   ecx
  4334e1:	8b 81 6c 0f 00 00    	mov    eax,DWORD PTR [ecx+0xf6c]
  4334e7:	83 f8 01             	cmp    eax,0x1
  4334ea:	7e 6f                	jle    0x43355b
  4334ec:	8d 50 ff             	lea    edx,[eax-0x1]
  4334ef:	85 d2                	test   edx,edx
  4334f1:	7e 68                	jle    0x43355b
  4334f3:	53                   	push   ebx
  4334f4:	55                   	push   ebp
  4334f5:	56                   	push   esi
  4334f6:	bb 01 00 00 00       	mov    ebx,0x1
  4334fb:	57                   	push   edi
  4334fc:	89 5c 24 10          	mov    DWORD PTR [esp+0x10],ebx
  433500:	8d b9 a4 0e 00 00    	lea    edi,[ecx+0xea4]
  433506:	3b d8                	cmp    ebx,eax
  433508:	7d 35                	jge    0x43353f
  43350a:	8d 77 04             	lea    esi,[edi+0x4]
  43350d:	8b 17                	mov    edx,DWORD PTR [edi]
  43350f:	8b 06                	mov    eax,DWORD PTR [esi]
  433511:	8b ac 91 fc 12 00 00 	mov    ebp,DWORD PTR [ecx+edx*4+0x12fc]
  433518:	3b ac 81 fc 12 00 00 	cmp    ebp,DWORD PTR [ecx+eax*4+0x12fc]
  43351f:	76 0c                	jbe    0x43352d
  433521:	85 d2                	test   edx,edx
  433523:	7c 08                	jl     0x43352d
  433525:	85 c0                	test   eax,eax
  433527:	7c 04                	jl     0x43352d
  433529:	89 07                	mov    DWORD PTR [edi],eax
  43352b:	89 16                	mov    DWORD PTR [esi],edx
  43352d:	8b 81 6c 0f 00 00    	mov    eax,DWORD PTR [ecx+0xf6c]
  433533:	43                   	inc    ebx
  433534:	83 c6 04             	add    esi,0x4
  433537:	3b d8                	cmp    ebx,eax
  433539:	7c d2                	jl     0x43350d
  43353b:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  43353f:	8b 81 6c 0f 00 00    	mov    eax,DWORD PTR [ecx+0xf6c]
  433545:	43                   	inc    ebx
  433546:	83 c7 04             	add    edi,0x4
  433549:	89 5c 24 10          	mov    DWORD PTR [esp+0x10],ebx
  43354d:	8d 53 ff             	lea    edx,[ebx-0x1]
  433550:	8d 70 ff             	lea    esi,[eax-0x1]
  433553:	3b d6                	cmp    edx,esi
  433555:	7c af                	jl     0x433506
  433557:	5f                   	pop    edi
  433558:	5e                   	pop    esi
  433559:	5d                   	pop    ebp
  43355a:	5b                   	pop    ebx
  43355b:	59                   	pop    ecx
  43355c:	c3                   	ret
  43355d:	90                   	nop
  43355e:	90                   	nop
  43355f:	90                   	nop
  433560:	55                   	push   ebp
  433561:	56                   	push   esi
  433562:	8b f1                	mov    esi,ecx
  433564:	57                   	push   edi
  433565:	6a 31                	push   0x31
  433567:	8b 86 9c 0e 00 00    	mov    eax,DWORD PTR [esi+0xe9c]
  43356d:	c7 86 6c 0f 00 00 00 	mov    DWORD PTR [esi+0xf6c],0x0
  433574:	00 00 00 
  433577:	c7 86 70 0f 00 00 00 	mov    DWORD PTR [esi+0xf70],0x0
  43357e:	00 00 00 
  433581:	89 86 c0 13 00 00    	mov    DWORD PTR [esi+0x13c0],eax
  433587:	e8 24 ff ff ff       	call   0x4334b0
  43358c:	8b 8e 9c 0e 00 00    	mov    ecx,DWORD PTR [esi+0xe9c]
  433592:	51                   	push   ecx
  433593:	56                   	push   esi
  433594:	8b ce                	mov    ecx,esi
  433596:	e8 65 0f 00 00       	call   0x434500
  43359b:	33 ff                	xor    edi,edi
  43359d:	8d ae fc 12 00 00    	lea    ebp,[esi+0x12fc]
  4335a3:	8a 84 3e 75 0f 00 00 	mov    al,BYTE PTR [esi+edi*1+0xf75]
  4335aa:	84 c0                	test   al,al
  4335ac:	74 16                	je     0x4335c4
  4335ae:	57                   	push   edi
  4335af:	8b ce                	mov    ecx,esi
  4335b1:	e8 fa fe ff ff       	call   0x4334b0
  4335b6:	8b 55 00             	mov    edx,DWORD PTR [ebp+0x0]
  4335b9:	8b ce                	mov    ecx,esi
  4335bb:	52                   	push   edx
  4335bc:	56                   	push   esi
  4335bd:	e8 3e 0f 00 00       	call   0x434500
  4335c2:	eb 07                	jmp    0x4335cb
  4335c4:	c7 45 00 ff ff ff 7f 	mov    DWORD PTR [ebp+0x0],0x7fffffff
  4335cb:	47                   	inc    edi
  4335cc:	83 c5 04             	add    ebp,0x4
  4335cf:	83 ff 31             	cmp    edi,0x31
  4335d2:	7c cf                	jl     0x4335a3
  4335d4:	8a 86 74 0f 00 00    	mov    al,BYTE PTR [esi+0xf74]
  4335da:	84 c0                	test   al,al
  4335dc:	75 0e                	jne    0x4335ec
  4335de:	8b ce                	mov    ecx,esi
  4335e0:	e8 fb fe ff ff       	call   0x4334e0
  4335e5:	c6 86 74 0f 00 00 01 	mov    BYTE PTR [esi+0xf74],0x1
  4335ec:	5f                   	pop    edi
  4335ed:	5e                   	pop    esi
  4335ee:	5d                   	pop    ebp
  4335ef:	c3                   	ret
  4335f0:	83 ec 0c             	sub    esp,0xc
  4335f3:	56                   	push   esi
  4335f4:	8b f1                	mov    esi,ecx
  4335f6:	8b 8e 6c 0f 00 00    	mov    ecx,DWORD PTR [esi+0xf6c]
  4335fc:	c6 86 74 0f 00 00 00 	mov    BYTE PTR [esi+0xf74],0x0
  433603:	85 c9                	test   ecx,ecx
  433605:	0f 84 59 01 00 00    	je     0x433764
  43360b:	8b 86 70 0f 00 00    	mov    eax,DWORD PTR [esi+0xf70]
  433611:	3b c1                	cmp    eax,ecx
  433613:	0f 8d 4b 01 00 00    	jge    0x433764
  433619:	8b 84 86 a4 0e 00 00 	mov    eax,DWORD PTR [esi+eax*4+0xea4]
  433620:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  433624:	8a 8c 30 75 0f 00 00 	mov    cl,BYTE PTR [eax+esi*1+0xf75]
  43362b:	84 c9                	test   cl,cl
  43362d:	0f 84 2b 01 00 00    	je     0x43375e
  433633:	83 f8 31             	cmp    eax,0x31
  433636:	0f 84 22 01 00 00    	je     0x43375e
  43363c:	8d 0c 40             	lea    ecx,[eax+eax*2]
  43363f:	c1 e1 04             	shl    ecx,0x4
  433642:	03 ce                	add    ecx,esi
  433644:	8b 91 64 05 00 00    	mov    edx,DWORD PTR [ecx+0x564]
  43364a:	85 d2                	test   edx,edx
  43364c:	75 16                	jne    0x433664
  43364e:	8b 15 c4 40 8c 00    	mov    edx,DWORD PTR ds:0x8c40c4
  433654:	81 c1 38 05 00 00    	add    ecx,0x538
  43365a:	52                   	push   edx
  43365b:	e8 80 f9 ff ff       	call   0x432fe0
  433660:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  433664:	8a 8c 30 a7 0f 00 00 	mov    cl,BYTE PTR [eax+esi*1+0xfa7]
  43366b:	84 c9                	test   cl,cl
  43366d:	0f 84 d9 00 00 00    	je     0x43374c
  433673:	8b 8c 86 34 12 00 00 	mov    ecx,DWORD PTR [esi+eax*4+0x1234]
  43367a:	8b 94 86 6c 11 00 00 	mov    edx,DWORD PTR [esi+eax*4+0x116c]
  433681:	3b ca                	cmp    ecx,edx
  433683:	0f 8d 84 00 00 00    	jge    0x43370d
  433689:	8d 54 24 08          	lea    edx,[esp+0x8]
  43368d:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  433691:	52                   	push   edx
  433692:	8d 14 40             	lea    edx,[eax+eax*2]
  433695:	c1 e2 04             	shl    edx,0x4
  433698:	51                   	push   ecx
  433699:	8d 8c 32 38 05 00 00 	lea    ecx,[edx+esi*1+0x538]
  4336a0:	e8 3b fa ff ff       	call   0x4330e0
  4336a5:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4336a9:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  4336ad:	8b 8c 86 dc 0f 00 00 	mov    ecx,DWORD PTR [esi+eax*4+0xfdc]
  4336b4:	03 d1                	add    edx,ecx
  4336b6:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  4336ba:	89 54 24 0c          	mov    DWORD PTR [esp+0xc],edx
  4336be:	8b 94 86 a4 10 00 00 	mov    edx,DWORD PTR [esi+eax*4+0x10a4]
  4336c5:	03 ca                	add    ecx,edx
  4336c7:	89 4c 24 08          	mov    DWORD PTR [esp+0x8],ecx
  4336cb:	8b 94 86 34 12 00 00 	mov    edx,DWORD PTR [esi+eax*4+0x1234]
  4336d2:	42                   	inc    edx
  4336d3:	89 94 86 34 12 00 00 	mov    DWORD PTR [esi+eax*4+0x1234],edx
  4336da:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  4336de:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4336e2:	50                   	push   eax
  4336e3:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  4336e7:	51                   	push   ecx
  4336e8:	8d 14 40             	lea    edx,[eax+eax*2]
  4336eb:	c1 e2 04             	shl    edx,0x4
  4336ee:	8d 8c 32 38 05 00 00 	lea    ecx,[edx+esi*1+0x538]
  4336f5:	e8 c6 f9 ff ff       	call   0x4330c0
  4336fa:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4336fe:	8d 04 40             	lea    eax,[eax+eax*2]
  433701:	c1 e0 04             	shl    eax,0x4
  433704:	8d 8c 30 38 05 00 00 	lea    ecx,[eax+esi*1+0x538]
  43370b:	eb 4c                	jmp    0x433759
  43370d:	8d 0c 40             	lea    ecx,[eax+eax*2]
  433710:	c1 e1 04             	shl    ecx,0x4
  433713:	8d 8c 31 38 05 00 00 	lea    ecx,[ecx+esi*1+0x538]
  43371a:	e8 21 fc ff ff       	call   0x433340
  43371f:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  433723:	8a 84 32 c4 13 00 00 	mov    al,BYTE PTR [edx+esi*1+0x13c4]
  43372a:	84 c0                	test   al,al
  43372c:	74 30                	je     0x43375e
  43372e:	8d 44 24 04          	lea    eax,[esp+0x4]
  433732:	8b ce                	mov    ecx,esi
  433734:	50                   	push   eax
  433735:	e8 36 00 00 00       	call   0x433770
  43373a:	8b 86 70 0f 00 00    	mov    eax,DWORD PTR [esi+0xf70]
  433740:	40                   	inc    eax
  433741:	89 86 70 0f 00 00    	mov    DWORD PTR [esi+0xf70],eax
  433747:	5e                   	pop    esi
  433748:	83 c4 0c             	add    esp,0xc
  43374b:	c3                   	ret
  43374c:	8d 0c 40             	lea    ecx,[eax+eax*2]
  43374f:	c1 e1 04             	shl    ecx,0x4
  433752:	8d 8c 31 38 05 00 00 	lea    ecx,[ecx+esi*1+0x538]
  433759:	e8 e2 fb ff ff       	call   0x433340
  43375e:	ff 86 70 0f 00 00    	inc    DWORD PTR [esi+0xf70]
  433764:	5e                   	pop    esi
  433765:	83 c4 0c             	add    esp,0xc
  433768:	c3                   	ret
  433769:	90                   	nop
  43376a:	90                   	nop
  43376b:	90                   	nop
  43376c:	90                   	nop
  43376d:	90                   	nop
  43376e:	90                   	nop
  43376f:	90                   	nop
  433770:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  433774:	8b 02                	mov    eax,DWORD PTR [edx]
  433776:	85 c0                	test   eax,eax
  433778:	7c 27                	jl     0x4337a1
  43377a:	83 f8 32             	cmp    eax,0x32
  43377d:	7d 22                	jge    0x4337a1
  43377f:	80 bc 08 75 0f 00 00 	cmp    BYTE PTR [eax+ecx*1+0xf75],0x0
  433786:	00 
  433787:	74 18                	je     0x4337a1
  433789:	c6 84 08 75 0f 00 00 	mov    BYTE PTR [eax+ecx*1+0xf75],0x0
  433790:	00 
  433791:	8b 02                	mov    eax,DWORD PTR [edx]
  433793:	c6 84 08 a7 0f 00 00 	mov    BYTE PTR [eax+ecx*1+0xfa7],0x0
  43379a:	00 
  43379b:	c7 02 ff ff ff ff    	mov    DWORD PTR [edx],0xffffffff
  4337a1:	c2 04 00             	ret    0x4
  4337a4:	90                   	nop
  4337a5:	90                   	nop
  4337a6:	90                   	nop
  4337a7:	90                   	nop
  4337a8:	90                   	nop
  4337a9:	90                   	nop
  4337aa:	90                   	nop
  4337ab:	90                   	nop
  4337ac:	90                   	nop
  4337ad:	90                   	nop
  4337ae:	90                   	nop
  4337af:	90                   	nop
  4337b0:	56                   	push   esi
  4337b1:	8b f1                	mov    esi,ecx
  4337b3:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  4337b7:	57                   	push   edi
  4337b8:	8b c1                	mov    eax,ecx
  4337ba:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  4337be:	99                   	cdq
  4337bf:	83 e2 07             	and    edx,0x7
  4337c2:	89 0e                	mov    DWORD PTR [esi],ecx
  4337c4:	03 c2                	add    eax,edx
  4337c6:	c6 46 28 00          	mov    BYTE PTR [esi+0x28],0x0
  4337ca:	c1 f8 03             	sar    eax,0x3
  4337cd:	81 e1 07 00 00 80    	and    ecx,0x80000007
  4337d3:	89 7e 04             	mov    DWORD PTR [esi+0x4],edi
  4337d6:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  4337d9:	79 05                	jns    0x4337e0
  4337db:	49                   	dec    ecx
  4337dc:	83 c9 f8             	or     ecx,0xfffffff8
  4337df:	41                   	inc    ecx
  4337e0:	74 04                	je     0x4337e6
  4337e2:	40                   	inc    eax
  4337e3:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  4337e6:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
  4337e9:	8b ce                	mov    ecx,esi
  4337eb:	0f af c7             	imul   eax,edi
  4337ee:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  4337f1:	e8 1a 00 00 00       	call   0x433810
  4337f6:	8b c6                	mov    eax,esi
  4337f8:	5f                   	pop    edi
  4337f9:	5e                   	pop    esi
  4337fa:	c2 08 00             	ret    0x8
  4337fd:	90                   	nop
  4337fe:	90                   	nop
  4337ff:	90                   	nop
  433800:	e9 0b 01 00 00       	jmp    0x433910
  433805:	90                   	nop
  433806:	90                   	nop
  433807:	90                   	nop
  433808:	90                   	nop
  433809:	90                   	nop
  43380a:	90                   	nop
  43380b:	90                   	nop
  43380c:	90                   	nop
  43380d:	90                   	nop
  43380e:	90                   	nop
  43380f:	90                   	nop
  433810:	33 c0                	xor    eax,eax
  433812:	c7 41 0c ff 7f 00 00 	mov    DWORD PTR [ecx+0xc],0x7fff
  433819:	89 41 14             	mov    DWORD PTR [ecx+0x14],eax
  43381c:	89 41 24             	mov    DWORD PTR [ecx+0x24],eax
  43381f:	c3                   	ret
  433820:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  433826:	6a ff                	push   0xffffffff
  433828:	68 08 ee 45 00       	push   0x45ee08
  43382d:	50                   	push   eax
  43382e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  433835:	83 ec 14             	sub    esp,0x14
  433838:	56                   	push   esi
  433839:	8b f1                	mov    esi,ecx
  43383b:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  43383e:	0f af 06             	imul   eax,DWORD PTR [esi]
  433841:	d1 e0                	shl    eax,1
  433843:	50                   	push   eax
  433844:	e8 27 de 00 00       	call   0x441670
  433849:	83 c4 04             	add    esp,0x4
  43384c:	89 46 14             	mov    DWORD PTR [esi+0x14],eax
  43384f:	85 c0                	test   eax,eax
  433851:	75 14                	jne    0x433867
  433853:	32 c0                	xor    al,al
  433855:	5e                   	pop    esi
  433856:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  43385a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  433861:	83 c4 20             	add    esp,0x20
  433864:	c2 04 00             	ret    0x4
  433867:	6a 01                	push   0x1
  433869:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  43386d:	c7 46 24 00 00 00 00 	mov    DWORD PTR [esi+0x24],0x0
  433874:	e8 77 de 00 00       	call   0x4416f0
  433879:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  43387d:	c7 44 24 20 00 00 00 	mov    DWORD PTR [esp+0x20],0x0
  433884:	00 
  433885:	51                   	push   ecx
  433886:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  43388a:	e8 b1 de 00 00       	call   0x441740
  43388f:	85 c0                	test   eax,eax
  433891:	89 46 24             	mov    DWORD PTR [esi+0x24],eax
  433894:	75 25                	jne    0x4338bb
  433896:	8d 4c 24 04          	lea    ecx,[esp+0x4]
  43389a:	c7 44 24 20 ff ff ff 	mov    DWORD PTR [esp+0x20],0xffffffff
  4338a1:	ff 
  4338a2:	e8 69 de 00 00       	call   0x441710
  4338a7:	32 c0                	xor    al,al
  4338a9:	5e                   	pop    esi
  4338aa:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  4338ae:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4338b5:	83 c4 20             	add    esp,0x20
  4338b8:	c2 04 00             	ret    0x4
  4338bb:	8b 56 08             	mov    edx,DWORD PTR [esi+0x8]
  4338be:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  4338c1:	8b ca                	mov    ecx,edx
  4338c3:	c6 46 28 01          	mov    BYTE PTR [esi+0x28],0x1
  4338c7:	c1 e1 08             	shl    ecx,0x8
  4338ca:	03 c8                	add    ecx,eax
  4338cc:	c7 44 24 20 ff ff ff 	mov    DWORD PTR [esp+0x20],0xffffffff
  4338d3:	ff 
  4338d4:	89 4e 1c             	mov    DWORD PTR [esi+0x1c],ecx
  4338d7:	8d 0c 92             	lea    ecx,[edx+edx*4]
  4338da:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  4338dd:	d1 e1                	shl    ecx,1
  4338df:	2b ca                	sub    ecx,edx
  4338e1:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  4338e4:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
  4338e7:	8d 14 89             	lea    edx,[ecx+ecx*4]
  4338ea:	8d 4c 24 04          	lea    ecx,[esp+0x4]
  4338ee:	8d 04 50             	lea    eax,[eax+edx*2]
  4338f1:	89 46 20             	mov    DWORD PTR [esi+0x20],eax
  4338f4:	e8 17 de 00 00       	call   0x441710
  4338f9:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4338fd:	b0 01                	mov    al,0x1
  4338ff:	5e                   	pop    esi
  433900:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  433907:	83 c4 20             	add    esp,0x20
  43390a:	c2 04 00             	ret    0x4
  43390d:	90                   	nop
  43390e:	90                   	nop
  43390f:	90                   	nop
  433910:	56                   	push   esi
  433911:	8b f1                	mov    esi,ecx
  433913:	8b 46 14             	mov    eax,DWORD PTR [esi+0x14]
  433916:	85 c0                	test   eax,eax
  433918:	74 10                	je     0x43392a
  43391a:	50                   	push   eax
  43391b:	e8 40 dd 00 00       	call   0x441660
  433920:	83 c4 04             	add    esp,0x4
  433923:	c7 46 14 00 00 00 00 	mov    DWORD PTR [esi+0x14],0x0
  43392a:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  43392d:	85 c0                	test   eax,eax
  43392f:	74 10                	je     0x433941
  433931:	50                   	push   eax
  433932:	e8 29 dd 00 00       	call   0x441660
  433937:	83 c4 04             	add    esp,0x4
  43393a:	c7 46 24 00 00 00 00 	mov    DWORD PTR [esi+0x24],0x0
  433941:	8b ce                	mov    ecx,esi
  433943:	e8 c8 fe ff ff       	call   0x433810
  433948:	5e                   	pop    esi
  433949:	c3                   	ret
  43394a:	90                   	nop
  43394b:	90                   	nop
  43394c:	90                   	nop
  43394d:	90                   	nop
  43394e:	90                   	nop
  43394f:	90                   	nop
  433950:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  433954:	83 ec 14             	sub    esp,0x14
  433957:	85 d2                	test   edx,edx
  433959:	56                   	push   esi
  43395a:	8b f1                	mov    esi,ecx
  43395c:	0f 84 cd 02 00 00    	je     0x433c2f
  433962:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  433966:	85 c0                	test   eax,eax
  433968:	0f 84 c1 02 00 00    	je     0x433c2f
  43396e:	8a 46 28             	mov    al,BYTE PTR [esi+0x28]
  433971:	84 c0                	test   al,al
  433973:	0f 84 b6 02 00 00    	je     0x433c2f
  433979:	53                   	push   ebx
  43397a:	55                   	push   ebp
  43397b:	57                   	push   edi
  43397c:	8b fa                	mov    edi,edx
  43397e:	83 c9 ff             	or     ecx,0xffffffff
  433981:	33 c0                	xor    eax,eax
  433983:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  433985:	8a 02                	mov    al,BYTE PTR [edx]
  433987:	8b 6c 24 34          	mov    ebp,DWORD PTR [esp+0x34]
  43398b:	f7 d1                	not    ecx
  43398d:	49                   	dec    ecx
  43398e:	89 6c 24 20          	mov    DWORD PTR [esp+0x20],ebp
  433992:	84 c0                	test   al,al
  433994:	8b da                	mov    ebx,edx
  433996:	89 4c 24 40          	mov    DWORD PTR [esp+0x40],ecx
  43399a:	0f 84 7d 02 00 00    	je     0x433c1d
  4339a0:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  4339a4:	85 c0                	test   eax,eax
  4339a6:	0f 8e 71 02 00 00    	jle    0x433c1d
  4339ac:	8a 03                	mov    al,BYTE PTR [ebx]
  4339ae:	3c 80                	cmp    al,0x80
  4339b0:	77 2c                	ja     0x4339de
  4339b2:	8b 7e 08             	mov    edi,DWORD PTR [esi+0x8]
  4339b5:	25 ff 00 00 00       	and    eax,0xff
  4339ba:	0f af f8             	imul   edi,eax
  4339bd:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  4339c0:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4339c2:	03 f8                	add    edi,eax
  4339c4:	8b c1                	mov    eax,ecx
  4339c6:	99                   	cdq
  4339c7:	2b c2                	sub    eax,edx
  4339c9:	d1 f8                	sar    eax,1
  4339cb:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  4339cf:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  4339d3:	43                   	inc    ebx
  4339d4:	48                   	dec    eax
  4339d5:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  4339d9:	e9 d4 00 00 00       	jmp    0x433ab2
  4339de:	8a 4b 01             	mov    cl,BYTE PTR [ebx+0x1]
  4339e1:	3c a4                	cmp    al,0xa4
  4339e3:	88 44 24 18          	mov    BYTE PTR [esp+0x18],al
  4339e7:	88 4c 24 10          	mov    BYTE PTR [esp+0x10],cl
  4339eb:	72 6a                	jb     0x433a57
  4339ed:	3c c9                	cmp    al,0xc9
  4339ef:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4339f3:	72 16                	jb     0x433a0b
  4339f5:	25 ff 00 00 00       	and    eax,0xff
  4339fa:	8d 14 80             	lea    edx,[eax+eax*4]
  4339fd:	c1 e2 03             	shl    edx,0x3
  433a00:	2b d0                	sub    edx,eax
  433a02:	8d 84 90 d4 99 ff ff 	lea    eax,[eax+edx*4-0x662c]
  433a09:	eb 15                	jmp    0x433a20
  433a0b:	25 ff 00 00 00       	and    eax,0xff
  433a10:	2d a4 00 00 00       	sub    eax,0xa4
  433a15:	8d 14 80             	lea    edx,[eax+eax*4]
  433a18:	c1 e2 03             	shl    edx,0x3
  433a1b:	2b d0                	sub    edx,eax
  433a1d:	8d 04 90             	lea    eax,[eax+edx*4]
  433a20:	80 f9 a1             	cmp    cl,0xa1
  433a23:	72 19                	jb     0x433a3e
  433a25:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  433a29:	8b 7e 08             	mov    edi,DWORD PTR [esi+0x8]
  433a2c:	81 e1 ff 00 00 00    	and    ecx,0xff
  433a32:	8d 44 08 9e          	lea    eax,[eax+ecx*1-0x62]
  433a36:	0f af f8             	imul   edi,eax
  433a39:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  433a3c:	eb 5e                	jmp    0x433a9c
  433a3e:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  433a42:	8b 7e 08             	mov    edi,DWORD PTR [esi+0x8]
  433a45:	81 e2 ff 00 00 00    	and    edx,0xff
  433a4b:	8d 44 10 c0          	lea    eax,[eax+edx*1-0x40]
  433a4f:	0f af f8             	imul   edi,eax
  433a52:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  433a55:	eb 45                	jmp    0x433a9c
  433a57:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  433a5b:	25 ff 00 00 00       	and    eax,0xff
  433a60:	2d a1 00 00 00       	sub    eax,0xa1
  433a65:	8d 14 80             	lea    edx,[eax+eax*4]
  433a68:	c1 e2 03             	shl    edx,0x3
  433a6b:	2b d0                	sub    edx,eax
  433a6d:	80 f9 a1             	cmp    cl,0xa1
  433a70:	8d 04 90             	lea    eax,[eax+edx*4]
  433a73:	72 10                	jb     0x433a85
  433a75:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  433a79:	81 e1 ff 00 00 00    	and    ecx,0xff
  433a7f:	8d 44 08 9e          	lea    eax,[eax+ecx*1-0x62]
  433a83:	eb 0e                	jmp    0x433a93
  433a85:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  433a89:	81 e2 ff 00 00 00    	and    edx,0xff
  433a8f:	8d 44 10 c0          	lea    eax,[eax+edx*1-0x40]
  433a93:	8b 7e 08             	mov    edi,DWORD PTR [esi+0x8]
  433a96:	0f af f8             	imul   edi,eax
  433a99:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  433a9c:	8b 0e                	mov    ecx,DWORD PTR [esi]
  433a9e:	03 f8                	add    edi,eax
  433aa0:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  433aa4:	83 c3 02             	add    ebx,0x2
  433aa7:	89 4c 24 1c          	mov    DWORD PTR [esp+0x1c],ecx
  433aab:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  433aaf:	83 e8 02             	sub    eax,0x2
  433ab2:	85 ed                	test   ebp,ebp
  433ab4:	89 44 24 40          	mov    DWORD PTR [esp+0x40],eax
  433ab8:	0f 8e 45 01 00 00    	jle    0x433c03
  433abe:	8b 5c 24 38          	mov    ebx,DWORD PTR [esp+0x38]
  433ac2:	85 db                	test   ebx,ebx
  433ac4:	0f 8e 35 01 00 00    	jle    0x433bff
  433aca:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  433ace:	03 cd                	add    ecx,ebp
  433ad0:	3b c8                	cmp    ecx,eax
  433ad2:	0f 8d 27 01 00 00    	jge    0x433bff
  433ad8:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  433adb:	8b 4c 24 2c          	mov    ecx,DWORD PTR [esp+0x2c]
  433adf:	8b c3                	mov    eax,ebx
  433ae1:	03 c2                	add    eax,edx
  433ae3:	3b c1                	cmp    eax,ecx
  433ae5:	0f 8d 14 01 00 00    	jge    0x433bff
  433aeb:	f6 44 24 44 04       	test   BYTE PTR [esp+0x44],0x4
  433af0:	74 47                	je     0x433b39
  433af2:	bd 44 ad 46 00       	mov    ebp,0x46ad44
  433af7:	8b 55 04             	mov    edx,DWORD PTR [ebp+0x4]
  433afa:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  433afd:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  433b01:	03 d3                	add    edx,ebx
  433b03:	51                   	push   ecx
  433b04:	8b 4c 24 34          	mov    ecx,DWORD PTR [esp+0x34]
  433b08:	52                   	push   edx
  433b09:	8b 55 00             	mov    edx,DWORD PTR [ebp+0x0]
  433b0c:	03 c2                	add    eax,edx
  433b0e:	8b 54 24 34          	mov    edx,DWORD PTR [esp+0x34]
  433b12:	50                   	push   eax
  433b13:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  433b17:	51                   	push   ecx
  433b18:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  433b1b:	52                   	push   edx
  433b1c:	8b 16                	mov    edx,DWORD PTR [esi]
  433b1e:	50                   	push   eax
  433b1f:	57                   	push   edi
  433b20:	51                   	push   ecx
  433b21:	52                   	push   edx
  433b22:	e8 16 ae 01 00       	call   0x44e93d
  433b27:	83 c5 08             	add    ebp,0x8
  433b2a:	83 c4 24             	add    esp,0x24
  433b2d:	81 fd 54 ad 46 00    	cmp    ebp,0x46ad54
  433b33:	7c c2                	jl     0x433af7
  433b35:	8b 6c 24 34          	mov    ebp,DWORD PTR [esp+0x34]
  433b39:	f6 44 24 44 08       	test   BYTE PTR [esp+0x44],0x8
  433b3e:	74 6f                	je     0x433baf
  433b40:	a1 b0 b2 46 00       	mov    eax,ds:0x46b2b0
  433b45:	8b 56 0c             	mov    edx,DWORD PTR [esi+0xc]
  433b48:	8b 18                	mov    ebx,DWORD PTR [eax]
  433b4a:	66 8b 48 04          	mov    cx,WORD PTR [eax+0x4]
  433b4e:	3b 50 08             	cmp    edx,DWORD PTR [eax+0x8]
  433b51:	75 04                	jne    0x433b57
  433b53:	66 8b 48 0c          	mov    cx,WORD PTR [eax+0xc]
  433b57:	3b 50 10             	cmp    edx,DWORD PTR [eax+0x10]
  433b5a:	75 04                	jne    0x433b60
  433b5c:	66 8b 48 14          	mov    cx,WORD PTR [eax+0x14]
  433b60:	0f bf d9             	movsx  ebx,cx
  433b63:	bd 44 ad 46 00       	mov    ebp,0x46ad44
  433b68:	8b 45 04             	mov    eax,DWORD PTR [ebp+0x4]
  433b6b:	8b 54 24 38          	mov    edx,DWORD PTR [esp+0x38]
  433b6f:	8b 4d 00             	mov    ecx,DWORD PTR [ebp+0x0]
  433b72:	03 c2                	add    eax,edx
  433b74:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  433b78:	53                   	push   ebx
  433b79:	50                   	push   eax
  433b7a:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  433b7e:	03 c8                	add    ecx,eax
  433b80:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  433b84:	51                   	push   ecx
  433b85:	8b 4c 24 34          	mov    ecx,DWORD PTR [esp+0x34]
  433b89:	52                   	push   edx
  433b8a:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  433b8d:	50                   	push   eax
  433b8e:	8b 06                	mov    eax,DWORD PTR [esi]
  433b90:	51                   	push   ecx
  433b91:	57                   	push   edi
  433b92:	52                   	push   edx
  433b93:	50                   	push   eax
  433b94:	e8 a4 ad 01 00       	call   0x44e93d
  433b99:	83 c5 08             	add    ebp,0x8
  433b9c:	83 c4 24             	add    esp,0x24
  433b9f:	81 fd 54 ad 46 00    	cmp    ebp,0x46ad54
  433ba5:	7c c1                	jl     0x433b68
  433ba7:	8b 5c 24 38          	mov    ebx,DWORD PTR [esp+0x38]
  433bab:	8b 6c 24 34          	mov    ebp,DWORD PTR [esp+0x34]
  433baf:	f6 44 24 44 01       	test   BYTE PTR [esp+0x44],0x1
  433bb4:	74 24                	je     0x433bda
  433bb6:	8b 4e 0c             	mov    ecx,DWORD PTR [esi+0xc]
  433bb9:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  433bbd:	8b 44 24 2c          	mov    eax,DWORD PTR [esp+0x2c]
  433bc1:	51                   	push   ecx
  433bc2:	8b 4c 24 2c          	mov    ecx,DWORD PTR [esp+0x2c]
  433bc6:	53                   	push   ebx
  433bc7:	55                   	push   ebp
  433bc8:	52                   	push   edx
  433bc9:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  433bcc:	50                   	push   eax
  433bcd:	8b 06                	mov    eax,DWORD PTR [esi]
  433bcf:	51                   	push   ecx
  433bd0:	57                   	push   edi
  433bd1:	52                   	push   edx
  433bd2:	50                   	push   eax
  433bd3:	e8 65 ad 01 00       	call   0x44e93d
  433bd8:	eb 22                	jmp    0x433bfc
  433bda:	8b 4e 14             	mov    ecx,DWORD PTR [esi+0x14]
  433bdd:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  433be1:	8b 44 24 2c          	mov    eax,DWORD PTR [esp+0x2c]
  433be5:	51                   	push   ecx
  433be6:	8b 4c 24 2c          	mov    ecx,DWORD PTR [esp+0x2c]
  433bea:	53                   	push   ebx
  433beb:	55                   	push   ebp
  433bec:	52                   	push   edx
  433bed:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  433bf0:	50                   	push   eax
  433bf1:	8b 06                	mov    eax,DWORD PTR [esi]
  433bf3:	51                   	push   ecx
  433bf4:	57                   	push   edi
  433bf5:	52                   	push   edx
  433bf6:	50                   	push   eax
  433bf7:	e8 fb ad 01 00       	call   0x44e9f7
  433bfc:	83 c4 24             	add    esp,0x24
  433bff:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  433c03:	8b 4c 24 3c          	mov    ecx,DWORD PTR [esp+0x3c]
  433c07:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  433c0b:	8a 03                	mov    al,BYTE PTR [ebx]
  433c0d:	03 d1                	add    edx,ecx
  433c0f:	03 ea                	add    ebp,edx
  433c11:	84 c0                	test   al,al
  433c13:	89 6c 24 34          	mov    DWORD PTR [esp+0x34],ebp
  433c17:	0f 85 83 fd ff ff    	jne    0x4339a0
  433c1d:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  433c21:	8b c5                	mov    eax,ebp
  433c23:	5f                   	pop    edi
  433c24:	5d                   	pop    ebp
  433c25:	5b                   	pop    ebx
  433c26:	2b c1                	sub    eax,ecx
  433c28:	5e                   	pop    esi
  433c29:	83 c4 14             	add    esp,0x14
  433c2c:	c2 20 00             	ret    0x20
  433c2f:	33 c0                	xor    eax,eax
  433c31:	5e                   	pop    esi
  433c32:	83 c4 14             	add    esp,0x14
  433c35:	c2 20 00             	ret    0x20
  433c38:	90                   	nop
  433c39:	90                   	nop
  433c3a:	90                   	nop
  433c3b:	90                   	nop
  433c3c:	90                   	nop
  433c3d:	90                   	nop
  433c3e:	90                   	nop
  433c3f:	90                   	nop
  433c40:	6a ff                	push   0xffffffff
  433c42:	68 28 ee 45 00       	push   0x45ee28
  433c47:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  433c4d:	50                   	push   eax
  433c4e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  433c55:	51                   	push   ecx
  433c56:	56                   	push   esi
  433c57:	8b f1                	mov    esi,ecx
  433c59:	89 74 24 04          	mov    DWORD PTR [esp+0x4],esi
  433c5d:	e8 0e 97 00 00       	call   0x43d370
  433c62:	68 88 13 00 00       	push   0x1388
  433c67:	33 c0                	xor    eax,eax
  433c69:	68 60 ea 00 00       	push   0xea60
  433c6e:	6a 10                	push   0x10
  433c70:	8b ce                	mov    ecx,esi
  433c72:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  433c76:	c7 06 f4 0d 46 00    	mov    DWORD PTR [esi],0x460df4
  433c7c:	89 86 50 02 00 00    	mov    DWORD PTR [esi+0x250],eax
  433c82:	89 86 9c 03 00 00    	mov    DWORD PTR [esi+0x39c],eax
  433c88:	c7 86 84 03 00 00 fe 	mov    DWORD PTR [esi+0x384],0xfffffffe
  433c8f:	ff ff ff 
  433c92:	89 86 88 03 00 00    	mov    DWORD PTR [esi+0x388],eax
  433c98:	89 86 8c 03 00 00    	mov    DWORD PTR [esi+0x38c],eax
  433c9e:	89 86 90 03 00 00    	mov    DWORD PTR [esi+0x390],eax
  433ca4:	89 86 94 03 00 00    	mov    DWORD PTR [esi+0x394],eax
  433caa:	89 86 98 03 00 00    	mov    DWORD PTR [esi+0x398],eax
  433cb0:	e8 7b 98 00 00       	call   0x43d530
  433cb5:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  433cb9:	8b c6                	mov    eax,esi
  433cbb:	5e                   	pop    esi
  433cbc:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  433cc3:	83 c4 10             	add    esp,0x10
  433cc6:	c3                   	ret
  433cc7:	90                   	nop
  433cc8:	90                   	nop
  433cc9:	90                   	nop
  433cca:	90                   	nop
  433ccb:	90                   	nop
  433ccc:	90                   	nop
  433ccd:	90                   	nop
  433cce:	90                   	nop
  433ccf:	90                   	nop
  433cd0:	6a ff                	push   0xffffffff
  433cd2:	68 48 ee 45 00       	push   0x45ee48
  433cd7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  433cdd:	50                   	push   eax
  433cde:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  433ce5:	51                   	push   ecx
  433ce6:	56                   	push   esi
  433ce7:	8b f1                	mov    esi,ecx
  433ce9:	57                   	push   edi
  433cea:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  433cee:	c7 06 f4 0d 46 00    	mov    DWORD PTR [esi],0x460df4
  433cf4:	8b be 50 02 00 00    	mov    edi,DWORD PTR [esi+0x250]
  433cfa:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  433d01:	00 
  433d02:	85 ff                	test   edi,edi
  433d04:	74 1a                	je     0x433d20
  433d06:	8b cf                	mov    ecx,edi
  433d08:	e8 63 53 fe ff       	call   0x419070
  433d0d:	57                   	push   edi
  433d0e:	e8 3d d8 01 00       	call   0x451550
  433d13:	83 c4 04             	add    esp,0x4
  433d16:	c7 86 50 02 00 00 00 	mov    DWORD PTR [esi+0x250],0x0
  433d1d:	00 00 00 
  433d20:	8b 86 9c 03 00 00    	mov    eax,DWORD PTR [esi+0x39c]
  433d26:	85 c0                	test   eax,eax
  433d28:	74 13                	je     0x433d3d
  433d2a:	50                   	push   eax
  433d2b:	e8 20 d8 01 00       	call   0x451550
  433d30:	83 c4 04             	add    esp,0x4
  433d33:	c7 86 9c 03 00 00 00 	mov    DWORD PTR [esi+0x39c],0x0
  433d3a:	00 00 00 
  433d3d:	8b ce                	mov    ecx,esi
  433d3f:	c7 44 24 14 ff ff ff 	mov    DWORD PTR [esp+0x14],0xffffffff
  433d46:	ff 
  433d47:	e8 74 96 00 00       	call   0x43d3c0
  433d4c:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  433d50:	5f                   	pop    edi
  433d51:	5e                   	pop    esi
  433d52:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  433d59:	83 c4 10             	add    esp,0x10
  433d5c:	c3                   	ret
  433d5d:	90                   	nop
  433d5e:	90                   	nop
  433d5f:	90                   	nop
  433d60:	6a ff                	push   0xffffffff
  433d62:	68 73 ee 45 00       	push   0x45ee73
  433d67:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  433d6d:	50                   	push   eax
  433d6e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  433d75:	83 ec 18             	sub    esp,0x18
  433d78:	53                   	push   ebx
  433d79:	55                   	push   ebp
  433d7a:	56                   	push   esi
  433d7b:	8b f1                	mov    esi,ecx
  433d7d:	57                   	push   edi
  433d7e:	6a 01                	push   0x1
  433d80:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  433d84:	e8 67 d9 00 00       	call   0x4416f0
  433d89:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  433d8e:	33 db                	xor    ebx,ebx
  433d90:	89 5c 24 30          	mov    DWORD PTR [esp+0x30],ebx
  433d94:	8b 68 68             	mov    ebp,DWORD PTR [eax+0x68]
  433d97:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  433d9a:	8b 50 6c             	mov    edx,DWORD PTR [eax+0x6c]
  433d9d:	8b 78 28             	mov    edi,DWORD PTR [eax+0x28]
  433da0:	8b 40 3c             	mov    eax,DWORD PTR [eax+0x3c]
  433da3:	50                   	push   eax
  433da4:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  433da7:	51                   	push   ecx
  433da8:	52                   	push   edx
  433da9:	8d 0c 48             	lea    ecx,[eax+ecx*2]
  433dac:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  433daf:	51                   	push   ecx
  433db0:	8d 0c 50             	lea    ecx,[eax+edx*2]
  433db3:	51                   	push   ecx
  433db4:	55                   	push   ebp
  433db5:	8b ce                	mov    ecx,esi
  433db7:	e8 54 99 00 00       	call   0x43d710
  433dbc:	39 9e 50 02 00 00    	cmp    DWORD PTR [esi+0x250],ebx
  433dc2:	75 2f                	jne    0x433df3
  433dc4:	68 14 01 00 00       	push   0x114
  433dc9:	e8 91 d8 01 00       	call   0x45165f
  433dce:	83 c4 04             	add    esp,0x4
  433dd1:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  433dd5:	3b c3                	cmp    eax,ebx
  433dd7:	c6 44 24 30 01       	mov    BYTE PTR [esp+0x30],0x1
  433ddc:	74 09                	je     0x433de7
  433dde:	8b c8                	mov    ecx,eax
  433de0:	e8 4b 0f fd ff       	call   0x404d30
  433de5:	eb 02                	jmp    0x433de9
  433de7:	33 c0                	xor    eax,eax
  433de9:	88 5c 24 30          	mov    BYTE PTR [esp+0x30],bl
  433ded:	89 86 50 02 00 00    	mov    DWORD PTR [esi+0x250],eax
  433df3:	39 9e 9c 03 00 00    	cmp    DWORD PTR [esi+0x39c],ebx
  433df9:	0f 85 81 00 00 00    	jne    0x433e80
  433dff:	53                   	push   ebx
  433e00:	6a 01                	push   0x1
  433e02:	68 ac ad 46 00       	push   0x46adac
  433e07:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  433e0b:	89 9e a0 03 00 00    	mov    DWORD PTR [esi+0x3a0],ebx
  433e11:	e8 aa d9 00 00       	call   0x4417c0
  433e16:	84 c0                	test   al,al
  433e18:	74 66                	je     0x433e80
  433e1a:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433e1e:	e8 0d 90 ff ff       	call   0x42ce30
  433e23:	8b f8                	mov    edi,eax
  433e25:	b8 5d 41 4c ae       	mov    eax,0xae4c415d
  433e2a:	f7 ef                	imul   edi
  433e2c:	03 d7                	add    edx,edi
  433e2e:	c1 fa 06             	sar    edx,0x6
  433e31:	8b c2                	mov    eax,edx
  433e33:	c1 e8 1f             	shr    eax,0x1f
  433e36:	03 d0                	add    edx,eax
  433e38:	89 96 a0 03 00 00    	mov    DWORD PTR [esi+0x3a0],edx
  433e3e:	8d 04 52             	lea    eax,[edx+edx*2]
  433e41:	c1 e0 04             	shl    eax,0x4
  433e44:	2b c2                	sub    eax,edx
  433e46:	d1 e0                	shl    eax,1
  433e48:	3b f8                	cmp    edi,eax
  433e4a:	75 7a                	jne    0x433ec6
  433e4c:	50                   	push   eax
  433e4d:	e8 0d d8 01 00       	call   0x45165f
  433e52:	83 c4 04             	add    esp,0x4
  433e55:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433e59:	89 86 9c 03 00 00    	mov    DWORD PTR [esi+0x39c],eax
  433e5f:	50                   	push   eax
  433e60:	57                   	push   edi
  433e61:	53                   	push   ebx
  433e62:	e8 c9 db 00 00       	call   0x441a30
  433e67:	8b 8e 9c 03 00 00    	mov    ecx,DWORD PTR [esi+0x39c]
  433e6d:	57                   	push   edi
  433e6e:	51                   	push   ecx
  433e6f:	e8 5c 46 ff ff       	call   0x4284d0
  433e74:	83 c4 08             	add    esp,0x8
  433e77:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433e7b:	e8 80 db 00 00       	call   0x441a00
  433e80:	8b 44 24 38          	mov    eax,DWORD PTR [esp+0x38]
  433e84:	8b 54 24 3c          	mov    edx,DWORD PTR [esp+0x3c]
  433e88:	3b c3                	cmp    eax,ebx
  433e8a:	89 96 84 03 00 00    	mov    DWORD PTR [esi+0x384],edx
  433e90:	6a fe                	push   0xfffffffe
  433e92:	75 6f                	jne    0x433f03
  433e94:	68 a0 ad 46 00       	push   0x46ada0
  433e99:	8b ce                	mov    ecx,esi
  433e9b:	e8 c0 00 00 00       	call   0x433f60
  433ea0:	84 c0                	test   al,al
  433ea2:	74 6b                	je     0x433f0f
  433ea4:	53                   	push   ebx
  433ea5:	8b ce                	mov    ecx,esi
  433ea7:	e8 54 03 00 00       	call   0x434200
  433eac:	68 68 ab 46 00       	push   0x46ab68
  433eb1:	8b ce                	mov    ecx,esi
  433eb3:	e8 c8 01 00 00       	call   0x434080
  433eb8:	68 68 ab 46 00       	push   0x46ab68
  433ebd:	8b ce                	mov    ecx,esi
  433ebf:	e8 0c 02 00 00       	call   0x4340d0
  433ec4:	eb 66                	jmp    0x433f2c
  433ec6:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  433ecc:	68 ac ad 46 00       	push   0x46adac
  433ed1:	68 8c ad 46 00       	push   0x46ad8c
  433ed6:	e8 75 e1 ff ff       	call   0x432050
  433edb:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  433ee1:	68 54 ad 46 00       	push   0x46ad54
  433ee6:	53                   	push   ebx
  433ee7:	e8 84 df ff ff       	call   0x431e70
  433eec:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433ef0:	8a d8                	mov    bl,al
  433ef2:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  433ef9:	ff 
  433efa:	e8 11 d8 00 00       	call   0x441710
  433eff:	8a c3                	mov    al,bl
  433f01:	eb 3c                	jmp    0x433f3f
  433f03:	50                   	push   eax
  433f04:	8b ce                	mov    ecx,esi
  433f06:	e8 55 00 00 00       	call   0x433f60
  433f0b:	84 c0                	test   al,al
  433f0d:	75 15                	jne    0x433f24
  433f0f:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433f13:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  433f1a:	ff 
  433f1b:	e8 f0 d7 00 00       	call   0x441710
  433f20:	32 c0                	xor    al,al
  433f22:	eb 1b                	jmp    0x433f3f
  433f24:	53                   	push   ebx
  433f25:	8b ce                	mov    ecx,esi
  433f27:	e8 d4 02 00 00       	call   0x434200
  433f2c:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433f30:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  433f37:	ff 
  433f38:	e8 d3 d7 00 00       	call   0x441710
  433f3d:	b0 01                	mov    al,0x1
  433f3f:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  433f43:	5f                   	pop    edi
  433f44:	5e                   	pop    esi
  433f45:	5d                   	pop    ebp
  433f46:	5b                   	pop    ebx
  433f47:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  433f4e:	83 c4 24             	add    esp,0x24
  433f51:	c2 08 00             	ret    0x8
  433f54:	90                   	nop
  433f55:	90                   	nop
  433f56:	90                   	nop
  433f57:	90                   	nop
  433f58:	90                   	nop
  433f59:	90                   	nop
  433f5a:	90                   	nop
  433f5b:	90                   	nop
  433f5c:	90                   	nop
  433f5d:	90                   	nop
  433f5e:	90                   	nop
  433f5f:	90                   	nop
  433f60:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  433f64:	83 ec 64             	sub    esp,0x64
  433f67:	55                   	push   ebp
  433f68:	57                   	push   edi
  433f69:	8b 7c 24 70          	mov    edi,DWORD PTR [esp+0x70]
  433f6d:	8b e9                	mov    ebp,ecx
  433f6f:	57                   	push   edi
  433f70:	89 85 84 03 00 00    	mov    DWORD PTR [ebp+0x384],eax
  433f76:	e8 35 03 00 00       	call   0x4342b0
  433f7b:	8b 8d 4c 02 00 00    	mov    ecx,DWORD PTR [ebp+0x24c]
  433f81:	50                   	push   eax
  433f82:	8b 09                	mov    ecx,DWORD PTR [ecx]
  433f84:	e8 27 92 00 00       	call   0x43d1b0
  433f89:	84 c0                	test   al,al
  433f8b:	0f 84 a9 00 00 00    	je     0x43403a
  433f91:	8b 8d 4c 02 00 00    	mov    ecx,DWORD PTR [ebp+0x24c]
  433f97:	8b 11                	mov    edx,DWORD PTR [ecx]
  433f99:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  433f9c:	8b 80 5c 03 00 00    	mov    eax,DWORD PTR [eax+0x35c]
  433fa2:	8a 90 2d 01 00 00    	mov    dl,BYTE PTR [eax+0x12d]
  433fa8:	84 d2                	test   dl,dl
  433faa:	75 04                	jne    0x433fb0
  433fac:	33 c0                	xor    eax,eax
  433fae:	eb 09                	jmp    0x433fb9
  433fb0:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  433fb3:	33 c0                	xor    eax,eax
  433fb5:	66 8b 42 14          	mov    ax,WORD PTR [edx+0x14]
  433fb9:	89 85 88 03 00 00    	mov    DWORD PTR [ebp+0x388],eax
  433fbf:	8b 01                	mov    eax,DWORD PTR [ecx]
  433fc1:	6a 01                	push   0x1
  433fc3:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  433fc6:	8b 89 5c 03 00 00    	mov    ecx,DWORD PTR [ecx+0x35c]
  433fcc:	e8 4f 63 ff ff       	call   0x42a320
  433fd1:	83 c9 ff             	or     ecx,0xffffffff
  433fd4:	33 c0                	xor    eax,eax
  433fd6:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  433fd8:	f7 d1                	not    ecx
  433fda:	2b f9                	sub    edi,ecx
  433fdc:	53                   	push   ebx
  433fdd:	56                   	push   esi
  433fde:	8d 54 24 10          	lea    edx,[esp+0x10]
  433fe2:	8b c1                	mov    eax,ecx
  433fe4:	8b f7                	mov    esi,edi
  433fe6:	8b fa                	mov    edi,edx
  433fe8:	c1 e9 02             	shr    ecx,0x2
  433feb:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  433fed:	8b c8                	mov    ecx,eax
  433fef:	83 e1 03             	and    ecx,0x3
  433ff2:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  433ff4:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  433ff8:	c6 44 24 13 37       	mov    BYTE PTR [esp+0x13],0x37
  433ffd:	51                   	push   ecx
  433ffe:	8b cd                	mov    ecx,ebp
  434000:	e8 ab 02 00 00       	call   0x4342b0
  434005:	8b 95 4c 02 00 00    	mov    edx,DWORD PTR [ebp+0x24c]
  43400b:	50                   	push   eax
  43400c:	8b 4a 0c             	mov    ecx,DWORD PTR [edx+0xc]
  43400f:	e8 9c 91 00 00       	call   0x43d1b0
  434014:	8a d8                	mov    bl,al
  434016:	84 db                	test   bl,bl
  434018:	74 4c                	je     0x434066
  43401a:	8b 8d 4c 02 00 00    	mov    ecx,DWORD PTR [ebp+0x24c]
  434020:	8b 41 0c             	mov    eax,DWORD PTR [ecx+0xc]
  434023:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  434026:	8b 82 5c 03 00 00    	mov    eax,DWORD PTR [edx+0x35c]
  43402c:	8a 90 2d 01 00 00    	mov    dl,BYTE PTR [eax+0x12d]
  434032:	84 d2                	test   dl,dl
  434034:	75 0e                	jne    0x434044
  434036:	33 c0                	xor    eax,eax
  434038:	eb 13                	jmp    0x43404d
  43403a:	5f                   	pop    edi
  43403b:	32 c0                	xor    al,al
  43403d:	5d                   	pop    ebp
  43403e:	83 c4 64             	add    esp,0x64
  434041:	c2 08 00             	ret    0x8
  434044:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  434047:	33 c0                	xor    eax,eax
  434049:	66 8b 42 14          	mov    ax,WORD PTR [edx+0x14]
  43404d:	89 85 88 03 00 00    	mov    DWORD PTR [ebp+0x388],eax
  434053:	8b 41 0c             	mov    eax,DWORD PTR [ecx+0xc]
  434056:	6a 01                	push   0x1
  434058:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  43405b:	8b 89 5c 03 00 00    	mov    ecx,DWORD PTR [ecx+0x35c]
  434061:	e8 ba 62 ff ff       	call   0x42a320
  434066:	8a c3                	mov    al,bl
  434068:	5e                   	pop    esi
  434069:	5b                   	pop    ebx
  43406a:	5f                   	pop    edi
  43406b:	5d                   	pop    ebp
  43406c:	83 c4 64             	add    esp,0x64
  43406f:	c2 08 00             	ret    0x8
  434072:	90                   	nop
  434073:	90                   	nop
  434074:	90                   	nop
  434075:	90                   	nop
  434076:	90                   	nop
  434077:	90                   	nop
  434078:	90                   	nop
  434079:	90                   	nop
  43407a:	90                   	nop
  43407b:	90                   	nop
  43407c:	90                   	nop
  43407d:	90                   	nop
  43407e:	90                   	nop
  43407f:	90                   	nop
  434080:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  434084:	56                   	push   esi
  434085:	8b f1                	mov    esi,ecx
  434087:	50                   	push   eax
  434088:	e8 23 02 00 00       	call   0x4342b0
  43408d:	8b 8e 4c 02 00 00    	mov    ecx,DWORD PTR [esi+0x24c]
  434093:	50                   	push   eax
  434094:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  434097:	e8 14 91 00 00       	call   0x43d1b0
  43409c:	84 c0                	test   al,al
  43409e:	74 10                	je     0x4340b0
  4340a0:	8b 96 4c 02 00 00    	mov    edx,DWORD PTR [esi+0x24c]
  4340a6:	8b 4a 04             	mov    ecx,DWORD PTR [edx+0x4]
  4340a9:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  4340ac:	c6 42 09 01          	mov    BYTE PTR [edx+0x9],0x1
  4340b0:	c7 86 8c 03 00 00 12 	mov    DWORD PTR [esi+0x38c],0x12
  4340b7:	00 00 00 
  4340ba:	c7 86 90 03 00 00 00 	mov    DWORD PTR [esi+0x390],0x0
  4340c1:	00 00 00 
  4340c4:	5e                   	pop    esi
  4340c5:	c2 04 00             	ret    0x4
  4340c8:	90                   	nop
  4340c9:	90                   	nop
  4340ca:	90                   	nop
  4340cb:	90                   	nop
  4340cc:	90                   	nop
  4340cd:	90                   	nop
  4340ce:	90                   	nop
  4340cf:	90                   	nop
  4340d0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4340d4:	56                   	push   esi
  4340d5:	8b f1                	mov    esi,ecx
  4340d7:	50                   	push   eax
  4340d8:	e8 d3 01 00 00       	call   0x4342b0
  4340dd:	8b 8e 4c 02 00 00    	mov    ecx,DWORD PTR [esi+0x24c]
  4340e3:	50                   	push   eax
  4340e4:	8b 49 08             	mov    ecx,DWORD PTR [ecx+0x8]
  4340e7:	e8 c4 90 00 00       	call   0x43d1b0
  4340ec:	84 c0                	test   al,al
  4340ee:	74 10                	je     0x434100
  4340f0:	8b 96 4c 02 00 00    	mov    edx,DWORD PTR [esi+0x24c]
  4340f6:	8b 4a 08             	mov    ecx,DWORD PTR [edx+0x8]
  4340f9:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  4340fc:	c6 42 09 01          	mov    BYTE PTR [edx+0x9],0x1
  434100:	5e                   	pop    esi
  434101:	c2 04 00             	ret    0x4
  434104:	90                   	nop
  434105:	90                   	nop
  434106:	90                   	nop
  434107:	90                   	nop
  434108:	90                   	nop
  434109:	90                   	nop
  43410a:	90                   	nop
  43410b:	90                   	nop
  43410c:	90                   	nop
  43410d:	90                   	nop
  43410e:	90                   	nop
  43410f:	90                   	nop
  434110:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  434114:	56                   	push   esi
  434115:	85 c0                	test   eax,eax
  434117:	8b f1                	mov    esi,ecx
  434119:	7c 2d                	jl     0x434148
  43411b:	3b 86 a0 03 00 00    	cmp    eax,DWORD PTR [esi+0x3a0]
  434121:	7d 25                	jge    0x434148
  434123:	8b 96 9c 03 00 00    	mov    edx,DWORD PTR [esi+0x39c]
  434129:	8d 0c 40             	lea    ecx,[eax+eax*2]
  43412c:	c1 e1 04             	shl    ecx,0x4
  43412f:	2b c8                	sub    ecx,eax
  434131:	6a fe                	push   0xfffffffe
  434133:	8d 44 4a 04          	lea    eax,[edx+ecx*2+0x4]
  434137:	8b ce                	mov    ecx,esi
  434139:	50                   	push   eax
  43413a:	e8 21 fc ff ff       	call   0x433d60
  43413f:	6a 00                	push   0x0
  434141:	8b ce                	mov    ecx,esi
  434143:	e8 b8 00 00 00       	call   0x434200
  434148:	32 c0                	xor    al,al
  43414a:	5e                   	pop    esi
  43414b:	c2 04 00             	ret    0x4
  43414e:	90                   	nop
  43414f:	90                   	nop
  434150:	56                   	push   esi
  434151:	57                   	push   edi
  434152:	33 ff                	xor    edi,edi
  434154:	8b f1                	mov    esi,ecx
  434156:	57                   	push   edi
  434157:	e8 14 98 00 00       	call   0x43d970
  43415c:	84 c0                	test   al,al
  43415e:	0f 84 99 00 00 00    	je     0x4341fd
  434164:	39 be 8c 03 00 00    	cmp    DWORD PTR [esi+0x38c],edi
  43416a:	7e 6b                	jle    0x4341d7
  43416c:	8b 8e 90 03 00 00    	mov    ecx,DWORD PTR [esi+0x390]
  434172:	83 f9 10             	cmp    ecx,0x10
  434175:	7c 1e                	jl     0x434195
  434177:	8b 86 4c 02 00 00    	mov    eax,DWORD PTR [esi+0x24c]
  43417d:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  434180:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  434183:	c7 80 80 00 00 00 ff 	mov    DWORD PTR [eax+0x80],0xffffffff
  43418a:	ff ff ff 
  43418d:	89 b8 84 00 00 00    	mov    DWORD PTR [eax+0x84],edi
  434193:	eb 1c                	jmp    0x4341b1
  434195:	8b 96 4c 02 00 00    	mov    edx,DWORD PTR [esi+0x24c]
  43419b:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  43419e:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  4341a1:	c7 80 80 00 00 00 01 	mov    DWORD PTR [eax+0x80],0x1
  4341a8:	00 00 00 
  4341ab:	89 88 84 00 00 00    	mov    DWORD PTR [eax+0x84],ecx
  4341b1:	89 b8 88 00 00 00    	mov    DWORD PTR [eax+0x88],edi
  4341b7:	89 b8 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],edi
  4341bd:	8b 8e 8c 03 00 00    	mov    ecx,DWORD PTR [esi+0x38c]
  4341c3:	8b 86 90 03 00 00    	mov    eax,DWORD PTR [esi+0x390]
  4341c9:	49                   	dec    ecx
  4341ca:	40                   	inc    eax
  4341cb:	89 8e 8c 03 00 00    	mov    DWORD PTR [esi+0x38c],ecx
  4341d1:	89 86 90 03 00 00    	mov    DWORD PTR [esi+0x390],eax
  4341d7:	83 be 94 03 00 00 02 	cmp    DWORD PTR [esi+0x394],0x2
  4341de:	75 1d                	jne    0x4341fd
  4341e0:	8b 86 98 03 00 00    	mov    eax,DWORD PTR [esi+0x398]
  4341e6:	3b c7                	cmp    eax,edi
  4341e8:	7e 13                	jle    0x4341fd
  4341ea:	48                   	dec    eax
  4341eb:	3b c7                	cmp    eax,edi
  4341ed:	89 86 98 03 00 00    	mov    DWORD PTR [esi+0x398],eax
  4341f3:	7f 08                	jg     0x4341fd
  4341f5:	57                   	push   edi
  4341f6:	8b ce                	mov    ecx,esi
  4341f8:	e8 03 00 00 00       	call   0x434200
  4341fd:	5f                   	pop    edi
  4341fe:	5e                   	pop    esi
  4341ff:	c3                   	ret
  434200:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  434204:	56                   	push   esi
  434205:	57                   	push   edi
  434206:	8b f9                	mov    edi,ecx
  434208:	85 c0                	test   eax,eax
  43420a:	75 08                	jne    0x434214
  43420c:	89 87 94 03 00 00    	mov    DWORD PTR [edi+0x394],eax
  434212:	eb 2f                	jmp    0x434243
  434214:	83 f8 03             	cmp    eax,0x3
  434217:	75 08                	jne    0x434221
  434219:	89 87 94 03 00 00    	mov    DWORD PTR [edi+0x394],eax
  43421f:	eb 22                	jmp    0x434243
  434221:	83 f8 01             	cmp    eax,0x1
  434224:	75 08                	jne    0x43422e
  434226:	89 87 94 03 00 00    	mov    DWORD PTR [edi+0x394],eax
  43422c:	eb 15                	jmp    0x434243
  43422e:	83 f8 02             	cmp    eax,0x2
  434231:	75 10                	jne    0x434243
  434233:	89 87 94 03 00 00    	mov    DWORD PTR [edi+0x394],eax
  434239:	c7 87 98 03 00 00 0a 	mov    DWORD PTR [edi+0x398],0xa
  434240:	00 00 00 
  434243:	33 f6                	xor    esi,esi
  434245:	39 b7 94 03 00 00    	cmp    DWORD PTR [edi+0x394],esi
  43424b:	75 28                	jne    0x434275
  43424d:	8b 87 4c 02 00 00    	mov    eax,DWORD PTR [edi+0x24c]
  434253:	6a 00                	push   0x0
  434255:	8b 0c b0             	mov    ecx,DWORD PTR [eax+esi*4]
  434258:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  43425b:	c6 82 58 03 00 00 01 	mov    BYTE PTR [edx+0x358],0x1
  434262:	8b 87 4c 02 00 00    	mov    eax,DWORD PTR [edi+0x24c]
  434268:	8b 0c b0             	mov    ecx,DWORD PTR [eax+esi*4]
  43426b:	8b 49 20             	mov    ecx,DWORD PTR [ecx+0x20]
  43426e:	e8 2d a4 00 00       	call   0x43e6a0
  434273:	eb 24                	jmp    0x434299
  434275:	8b 97 4c 02 00 00    	mov    edx,DWORD PTR [edi+0x24c]
  43427b:	8b 04 b2             	mov    eax,DWORD PTR [edx+esi*4]
  43427e:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  434281:	c6 81 58 03 00 00 00 	mov    BYTE PTR [ecx+0x358],0x0
  434288:	8b 97 4c 02 00 00    	mov    edx,DWORD PTR [edi+0x24c]
  43428e:	8b 04 b2             	mov    eax,DWORD PTR [edx+esi*4]
  434291:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  434294:	e8 37 a4 00 00       	call   0x43e6d0
  434299:	46                   	inc    esi
  43429a:	83 fe 04             	cmp    esi,0x4
  43429d:	7c a6                	jl     0x434245
  43429f:	5f                   	pop    edi
  4342a0:	5e                   	pop    esi
  4342a1:	c2               	ret    0x4
