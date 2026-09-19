
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00421690 <.text+0x20690>:
  421690:	8b 81 d0 0d 00 00    	mov    eax,DWORD PTR [ecx+0xdd0]
  421696:	33 d2                	xor    edx,edx
  421698:	53                   	push   ebx
  421699:	56                   	push   esi
  42169a:	3b c2                	cmp    eax,edx
  42169c:	57                   	push   edi
  42169d:	0f 8e c1 02 00 00    	jle    0x421964
  4216a3:	8b b1 f4 0c 00 00    	mov    esi,DWORD PTR [ecx+0xcf4]
  4216a9:	3b f2                	cmp    esi,edx
  4216ab:	0f 84 b3 02 00 00    	je     0x421964
  4216b1:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  4216b5:	2b c2                	sub    eax,edx
  4216b7:	74 0e                	je     0x4216c7
  4216b9:	48                   	dec    eax
  4216ba:	0f 84 a4 02 00 00    	je     0x421964
  4216c0:	48                   	dec    eax
  4216c1:	0f 85 9d 02 00 00    	jne    0x421964
  4216c7:	8b 7e 20             	mov    edi,DWORD PTR [esi+0x20]
  4216ca:	8d b7 f8 fe ff ff    	lea    esi,[edi-0x108]
  4216d0:	83 fe 64             	cmp    esi,0x64
  4216d3:	0f 87 8b 02 00 00    	ja     0x421964
  4216d9:	33 c0                	xor    eax,eax
  4216db:	8a 86 90 19 42 00    	mov    al,BYTE PTR [esi+0x421990]
  4216e1:	ff 24 85 6c 19 42 00 	jmp    DWORD PTR [eax*4+0x42196c]
  4216e8:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  4216ee:	33 c0                	xor    eax,eax
  4216f0:	3b f2                	cmp    esi,edx
  4216f2:	0f 8e 6c 02 00 00    	jle    0x421964
  4216f8:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  4216fe:	8b 3e                	mov    edi,DWORD PTR [esi]
  421700:	40                   	inc    eax
  421701:	83 c6 04             	add    esi,0x4
  421704:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  42170b:	89 97 34 0a 00 00    	mov    DWORD PTR [edi+0xa34],edx
  421711:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  421717:	3b c7                	cmp    eax,edi
  421719:	7c e3                	jl     0x4216fe
  42171b:	5f                   	pop    edi
  42171c:	5e                   	pop    esi
  42171d:	5b                   	pop    ebx
  42171e:	c2 04 00             	ret    0x4
  421721:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  421727:	33 c0                	xor    eax,eax
  421729:	3b f2                	cmp    esi,edx
  42172b:	0f 8e 33 02 00 00    	jle    0x421964
  421731:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  421737:	8b 3e                	mov    edi,DWORD PTR [esi]
  421739:	40                   	inc    eax
  42173a:	83 c6 04             	add    esi,0x4
  42173d:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  421744:	89 97 38 0a 00 00    	mov    DWORD PTR [edi+0xa38],edx
  42174a:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  421750:	3b c7                	cmp    eax,edi
  421752:	7c e3                	jl     0x421737
  421754:	5f                   	pop    edi
  421755:	5e                   	pop    esi
  421756:	5b                   	pop    ebx
  421757:	c2 04 00             	ret    0x4
  42175a:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  421760:	33 c0                	xor    eax,eax
  421762:	3b f2                	cmp    esi,edx
  421764:	0f 8e fa 01 00 00    	jle    0x421964
  42176a:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  421770:	8b 3e                	mov    edi,DWORD PTR [esi]
  421772:	40                   	inc    eax
  421773:	83 c6 04             	add    esi,0x4
  421776:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  42177d:	89 97 3c 0a 00 00    	mov    DWORD PTR [edi+0xa3c],edx
  421783:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  421789:	3b c7                	cmp    eax,edi
  42178b:	7c e3                	jl     0x421770
  42178d:	5f                   	pop    edi
  42178e:	5e                   	pop    esi
  42178f:	5b                   	pop    ebx
  421790:	c2 04 00             	ret    0x4
  421793:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  421799:	33 c0                	xor    eax,eax
  42179b:	3b f2                	cmp    esi,edx
  42179d:	0f 8e c1 01 00 00    	jle    0x421964
  4217a3:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  4217a9:	8b 3e                	mov    edi,DWORD PTR [esi]
  4217ab:	40                   	inc    eax
  4217ac:	83 c6 04             	add    esi,0x4
  4217af:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  4217b6:	89 97 60 0a 00 00    	mov    DWORD PTR [edi+0xa60],edx
  4217bc:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  4217c2:	3b c7                	cmp    eax,edi
  4217c4:	7c e3                	jl     0x4217a9
  4217c6:	5f                   	pop    edi
  4217c7:	5e                   	pop    esi
  4217c8:	5b                   	pop    ebx
  4217c9:	c2 04 00             	ret    0x4
  4217cc:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  4217d2:	33 c0                	xor    eax,eax
  4217d4:	3b f2                	cmp    esi,edx
  4217d6:	0f 8e 88 01 00 00    	jle    0x421964
  4217dc:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  4217e2:	8b 3e                	mov    edi,DWORD PTR [esi]
  4217e4:	40                   	inc    eax
  4217e5:	83 c6 04             	add    esi,0x4
  4217e8:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  4217ef:	89 97 40 0a 00 00    	mov    DWORD PTR [edi+0xa40],edx
  4217f5:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  4217fb:	3b c7                	cmp    eax,edi
  4217fd:	7c e3                	jl     0x4217e2
  4217ff:	5f                   	pop    edi
  421800:	5e                   	pop    esi
  421801:	5b                   	pop    ebx
  421802:	c2 04 00             	ret    0x4
  421805:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  42180b:	33 c0                	xor    eax,eax
  42180d:	3b f2                	cmp    esi,edx
  42180f:	0f 8e 4f 01 00 00    	jle    0x421964
  421815:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  42181b:	8b 3e                	mov    edi,DWORD PTR [esi]
  42181d:	40                   	inc    eax
  42181e:	83 c6 04             	add    esi,0x4
  421821:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  421828:	89 97 34 0a 00 00    	mov    DWORD PTR [edi+0xa34],edx
  42182e:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  421834:	3b c7                	cmp    eax,edi
  421836:	7c e3                	jl     0x42181b
  421838:	5f                   	pop    edi
  421839:	5e                   	pop    esi
  42183a:	5b                   	pop    ebx
  42183b:	c2 04 00             	ret    0x4
  42183e:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  421844:	33 c0                	xor    eax,eax
  421846:	3b f2                	cmp    esi,edx
  421848:	0f 8e 16 01 00 00    	jle    0x421964
  42184e:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  421854:	8b 3e                	mov    edi,DWORD PTR [esi]
  421856:	40                   	inc    eax
  421857:	83 c6 04             	add    esi,0x4
  42185a:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  421861:	89 97 88 0a 00 00    	mov    DWORD PTR [edi+0xa88],edx
  421867:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  42186d:	3b c7                	cmp    eax,edi
  42186f:	7c e3                	jl     0x421854
  421871:	5f                   	pop    edi
  421872:	5e                   	pop    esi
  421873:	5b                   	pop    ebx
  421874:	c2 04 00             	ret    0x4
  421877:	8b 81 c8 00 00 00    	mov    eax,DWORD PTR [ecx+0xc8]
  42187d:	3b c2                	cmp    eax,edx
  42187f:	0f 8e df 00 00 00    	jle    0x421964
  421885:	33 db                	xor    ebx,ebx
  421887:	3b c2                	cmp    eax,edx
  421889:	0f 8e d5 00 00 00    	jle    0x421964
  42188f:	55                   	push   ebp
  421890:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  421896:	8b 06                	mov    eax,DWORD PTR [esi]
  421898:	81 ff 1b 01 00 00    	cmp    edi,0x11b
  42189e:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  4218a5:	89 95 34 0a 00 00    	mov    DWORD PTR [ebp+0xa34],edx
  4218ab:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  4218b2:	89 95 38 0a 00 00    	mov    DWORD PTR [ebp+0xa38],edx
  4218b8:	74 0d                	je     0x4218c7
  4218ba:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  4218c1:	89 95 3c 0a 00 00    	mov    DWORD PTR [ebp+0xa3c],edx
  4218c7:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  4218ce:	81 ff 1b 01 00 00    	cmp    edi,0x11b
  4218d4:	89 95 40 0a 00 00    	mov    DWORD PTR [ebp+0xa40],edx
  4218da:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  4218e1:	89 95 44 0a 00 00    	mov    DWORD PTR [ebp+0xa44],edx
  4218e7:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  4218ee:	89 95 48 0a 00 00    	mov    DWORD PTR [ebp+0xa48],edx
  4218f4:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  4218fb:	89 95 4c 0a 00 00    	mov    DWORD PTR [ebp+0xa4c],edx
  421901:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  421908:	89 95 50 0a 00 00    	mov    DWORD PTR [ebp+0xa50],edx
  42190e:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  421915:	89 95 54 0a 00 00    	mov    DWORD PTR [ebp+0xa54],edx
  42191b:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  421922:	89 95 58 0a 00 00    	mov    DWORD PTR [ebp+0xa58],edx
  421928:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  42192f:	89 95 5c 0a 00 00    	mov    DWORD PTR [ebp+0xa5c],edx
  421935:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  42193c:	89 95 60 0a 00 00    	mov    DWORD PTR [ebp+0xa60],edx
  421942:	74 0d                	je     0x421951
  421944:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  42194b:	89 90 68 0a 00 00    	mov    DWORD PTR [eax+0xa68],edx
  421951:	8b 81 c8 00 00 00    	mov    eax,DWORD PTR [ecx+0xc8]
  421957:	43                   	inc    ebx
  421958:	83 c6 04             	add    esi,0x4
  42195b:	3b d8                	cmp    ebx,eax
  42195d:	0f 8c 33 ff ff ff    	jl     0x421896
  421963:	5d                   	pop    ebp
  421964:	5f                   	pop    edi
  421965:	5e                   	pop    esi
  421966:	5b                   	pop    ebx
  421967:	c2 04 00             	ret    0x4
  42196a:	8b ff                	mov    edi,edi
  42196c:	77 18                	ja     0x421986
  42196e:	42                   	inc    edx
  42196f:	00 e8                	add    al,ch
  421971:	16                   	push   ss
  421972:	42                   	inc    edx
  421973:	00 21                	add    BYTE PTR [ecx],ah
  421975:	17                   	pop    ss
  421976:	42                   	inc    edx
  421977:	00 5a 17             	add    BYTE PTR [edx+0x17],bl
  42197a:	42                   	inc    edx
  42197b:	00 93 17 42 00 cc    	add    BYTE PTR [ebx-0x33ffbde9],dl
  421981:	17                   	pop    ss
  421982:	42                   	inc    edx
  421983:	00 05 18 42 00 3e    	add    BYTE PTR ds:0x3e004218,al
  421989:	18 42 00             	sbb    BYTE PTR [edx+0x0],al
  42198c:	64 19 42 00          	sbb    DWORD PTR fs:[edx+0x0],eax
