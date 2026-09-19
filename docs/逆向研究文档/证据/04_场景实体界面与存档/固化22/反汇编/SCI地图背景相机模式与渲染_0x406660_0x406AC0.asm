
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00406660 <.text+0x5660>:
  406660:	b8 40 4e 00 00       	mov    eax,0x4e40
  406665:	e8 36 bb 04 00       	call   0x4521a0
  40666a:	8b 84 24 48 4e 00 00 	mov    eax,DWORD PTR [esp+0x4e48]
  406671:	53                   	push   ebx
  406672:	56                   	push   esi
  406673:	8b f1                	mov    esi,ecx
  406675:	8b 8c 24 54 4e 00 00 	mov    ecx,DWORD PTR [esp+0x4e54]
  40667c:	8b 94 24 58 4e 00 00 	mov    edx,DWORD PTR [esp+0x4e58]
  406683:	89 06                	mov    DWORD PTR [esi],eax
  406685:	33 c0                	xor    eax,eax
  406687:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  40668a:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  40668d:	8b 84 24 5c 4e 00 00 	mov    eax,DWORD PTR [esp+0x4e5c]
  406694:	89 4e 04             	mov    DWORD PTR [esi+0x4],ecx
  406697:	8b 8c 24 60 4e 00 00 	mov    ecx,DWORD PTR [esp+0x4e60]
  40669e:	89 46 14             	mov    DWORD PTR [esi+0x14],eax
  4066a1:	8b 84 24 68 4e 00 00 	mov    eax,DWORD PTR [esp+0x4e68]
  4066a8:	57                   	push   edi
  4066a9:	89 4e 18             	mov    DWORD PTR [esi+0x18],ecx
  4066ac:	89 46 20             	mov    DWORD PTR [esi+0x20],eax
  4066af:	b9 90 13 00 00       	mov    ecx,0x1390
  4066b4:	33 c0                	xor    eax,eax
  4066b6:	8d 7c 24 0c          	lea    edi,[esp+0xc]
  4066ba:	89 56 10             	mov    DWORD PTR [esi+0x10],edx
  4066bd:	8b 94 24 68 4e 00 00 	mov    edx,DWORD PTR [esp+0x4e68]
  4066c4:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4066c6:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  4066ca:	89 56 1c             	mov    DWORD PTR [esi+0x1c],edx
  4066cd:	51                   	push   ecx
  4066ce:	ff 15 5c 02 46 00    	call   DWORD PTR ds:0x46025c
  4066d4:	8b 84 24 50 4e 00 00 	mov    eax,DWORD PTR [esp+0x4e50]
  4066db:	8d 54 24 30          	lea    edx,[esp+0x30]
  4066df:	6a 01                	push   0x1
  4066e1:	52                   	push   edx
  4066e2:	50                   	push   eax
  4066e3:	e8 c8 be ff ff       	call   0x4025b0
  4066e8:	8b 1d 60 02 46 00    	mov    ebx,DWORD PTR ds:0x460260
  4066ee:	83 c4 0c             	add    esp,0xc
  4066f1:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  4066f5:	89 44 24 34          	mov    DWORD PTR [esp+0x34],eax
  4066f9:	6a 01                	push   0x1
  4066fb:	51                   	push   ecx
  4066fc:	ff d3                	call   ebx
  4066fe:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  406702:	0f af 44 24 38       	imul   eax,DWORD PTR [esp+0x38]
  406707:	8d 04 40             	lea    eax,[eax+eax*2]
  40670a:	50                   	push   eax
  40670b:	e8 4f af 04 00       	call   0x45165f
  406710:	8b 54 24 3c          	mov    edx,DWORD PTR [esp+0x3c]
  406714:	83 c4 04             	add    esp,0x4
  406717:	8b f8                	mov    edi,eax
  406719:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  40671d:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  406721:	6a 03                	push   0x3
  406723:	51                   	push   ecx
  406724:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
  406728:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  40672c:	c7 44 24 28 03 00 00 	mov    DWORD PTR [esp+0x28],0x3
  406733:	00 
  406734:	89 7c 24 18          	mov    DWORD PTR [esp+0x18],edi
  406738:	ff d3                	call   ebx
  40673a:	8b 4c 24 38          	mov    ecx,DWORD PTR [esp+0x38]
  40673e:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  406742:	89 7e 24             	mov    DWORD PTR [esi+0x24],edi
  406745:	89 4e 28             	mov    DWORD PTR [esi+0x28],ecx
  406748:	89 46 2c             	mov    DWORD PTR [esi+0x2c],eax
  40674b:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  406751:	83 7a 3c 10          	cmp    DWORD PTR [edx+0x3c],0x10
  406755:	75 0c                	jne    0x406763
  406757:	0f af c1             	imul   eax,ecx
  40675a:	50                   	push   eax
  40675b:	57                   	push   edi
  40675c:	e8 7f fe ff ff       	call   0x4065e0
  406761:	eb 0a                	jmp    0x40676d
  406763:	0f af c1             	imul   eax,ecx
  406766:	50                   	push   eax
  406767:	57                   	push   edi
  406768:	e8 23 fe ff ff       	call   0x406590
  40676d:	8b 44 24 38          	mov    eax,DWORD PTR [esp+0x38]
  406771:	83 c4 08             	add    esp,0x8
  406774:	50                   	push   eax
  406775:	e8 d6 ad 04 00       	call   0x451550
  40677a:	83 c4 04             	add    esp,0x4
  40677d:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  406781:	51                   	push   ecx
  406782:	ff 15 64 02 46 00    	call   DWORD PTR ds:0x460264
  406788:	5f                   	pop    edi
  406789:	5e                   	pop    esi
  40678a:	5b                   	pop    ebx
  40678b:	81 c4 40 4e 00 00    	add    esp,0x4e40
  406791:	c2 20 00             	ret    0x20
  406794:	90                   	nop
  406795:	90                   	nop
  406796:	90                   	nop
  406797:	90                   	nop
  406798:	90                   	nop
  406799:	90                   	nop
  40679a:	90                   	nop
  40679b:	90                   	nop
  40679c:	90                   	nop
  40679d:	90                   	nop
  40679e:	90                   	nop
  40679f:	90                   	nop
  4067a0:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  4067a3:	57                   	push   edi
  4067a4:	85 c0                	test   eax,eax
  4067a6:	0f 8e 94 00 00 00    	jle    0x406840
  4067ac:	83 f8 0a             	cmp    eax,0xa
  4067af:	0f 84 8b 00 00 00    	je     0x406840
  4067b5:	8b 15 14 85 97 00    	mov    edx,DWORD PTR ds:0x978514
  4067bb:	8b 3d 54 85 97 00    	mov    edi,DWORD PTR ds:0x978554
  4067c1:	2b d7                	sub    edx,edi
  4067c3:	89 11                	mov    DWORD PTR [ecx],edx
  4067c5:	8b 15 18 85 97 00    	mov    edx,DWORD PTR ds:0x978518
  4067cb:	8b 3d 58 85 97 00    	mov    edi,DWORD PTR ds:0x978558
  4067d1:	2b d7                	sub    edx,edi
  4067d3:	48                   	dec    eax
  4067d4:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  4067d7:	74 3d                	je     0x406816
  4067d9:	48                   	dec    eax
  4067da:	75 64                	jne    0x406840
  4067dc:	8b 01                	mov    eax,DWORD PTR [ecx]
  4067de:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  4067e5:	2b d0                	sub    edx,eax
  4067e7:	b8 67 66 66 66       	mov    eax,0x66666667
  4067ec:	d1 e2                	shl    edx,1
  4067ee:	f7 ea                	imul   edx
  4067f0:	c1 fa 02             	sar    edx,0x2
  4067f3:	8b c2                	mov    eax,edx
  4067f5:	c1 e8 1f             	shr    eax,0x1f
  4067f8:	03 d0                	add    edx,eax
  4067fa:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  4067fd:	89 11                	mov    DWORD PTR [ecx],edx
  4067ff:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  406806:	2b d0                	sub    edx,eax
  406808:	b8 67 66 66 66       	mov    eax,0x66666667
  40680d:	d1 e2                	shl    edx,1
  40680f:	f7 ea                	imul   edx
  406811:	c1 fa 02             	sar    edx,0x2
  406814:	eb 20                	jmp    0x406836
  406816:	8b 11                	mov    edx,DWORD PTR [ecx]
  406818:	b8 67 66 66 66       	mov    eax,0x66666667
  40681d:	f7 ea                	imul   edx
  40681f:	d1 fa                	sar    edx,1
  406821:	8b c2                	mov    eax,edx
  406823:	c1 e8 1f             	shr    eax,0x1f
  406826:	03 d0                	add    edx,eax
  406828:	b8 67 66 66 66       	mov    eax,0x66666667
  40682d:	89 11                	mov    DWORD PTR [ecx],edx
  40682f:	8b 51 04             	mov    edx,DWORD PTR [ecx+0x4]
  406832:	f7 ea                	imul   edx
  406834:	d1 fa                	sar    edx,1
  406836:	8b c2                	mov    eax,edx
  406838:	c1 e8 1f             	shr    eax,0x1f
  40683b:	03 d0                	add    edx,eax
  40683d:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  406840:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  406843:	85 c0                	test   eax,eax
  406845:	7e 52                	jle    0x406899
  406847:	8b 51 14             	mov    edx,DWORD PTR [ecx+0x14]
  40684a:	8b 79 08             	mov    edi,DWORD PTR [ecx+0x8]
  40684d:	03 fa                	add    edi,edx
  40684f:	8b c7                	mov    eax,edi
  406851:	89 79 08             	mov    DWORD PTR [ecx+0x8],edi
  406854:	3d 80 02 00 00       	cmp    eax,0x280
  406859:	7e 09                	jle    0x406864
  40685b:	c7 41 08 00 00 00 00 	mov    DWORD PTR [ecx+0x8],0x0
  406862:	eb 0b                	jmp    0x40686f
  406864:	85 c0                	test   eax,eax
  406866:	7d 07                	jge    0x40686f
  406868:	c7 41 08 80 02 00 00 	mov    DWORD PTR [ecx+0x8],0x280
  40686f:	8b 41 18             	mov    eax,DWORD PTR [ecx+0x18]
  406872:	8b 79 0c             	mov    edi,DWORD PTR [ecx+0xc]
  406875:	03 f8                	add    edi,eax
  406877:	8b c7                	mov    eax,edi
  406879:	89 79 0c             	mov    DWORD PTR [ecx+0xc],edi
  40687c:	3d e0 01 00 00       	cmp    eax,0x1e0
  406881:	7e 0b                	jle    0x40688e
  406883:	c7 41 0c 00 00 00 00 	mov    DWORD PTR [ecx+0xc],0x0
  40688a:	5f                   	pop    edi
  40688b:	c2 08 00             	ret    0x8
  40688e:	85 c0                	test   eax,eax
  406890:	7d 07                	jge    0x406899
  406892:	c7 41 0c e0 01 00 00 	mov    DWORD PTR [ecx+0xc],0x1e0
  406899:	5f                   	pop    edi
  40689a:	c2 08 00             	ret    0x8
  40689d:	90                   	nop
  40689e:	90                   	nop
  40689f:	90                   	nop
  4068a0:	53                   	push   ebx
  4068a1:	55                   	push   ebp
  4068a2:	56                   	push   esi
  4068a3:	8b f1                	mov    esi,ecx
  4068a5:	57                   	push   edi
  4068a6:	8b 46 10             	mov    eax,DWORD PTR [esi+0x10]
  4068a9:	85 c0                	test   eax,eax
  4068ab:	0f 84 36 01 00 00    	je     0x4069e7
  4068b1:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  4068b4:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  4068b7:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  4068bd:	6a 00                	push   0x0
  4068bf:	68 10 02 00 00       	push   0x210
  4068c4:	83 c0 30             	add    eax,0x30
  4068c7:	68 c0 02 00 00       	push   0x2c0
  4068cc:	50                   	push   eax
  4068cd:	8b 42 68             	mov    eax,DWORD PTR [edx+0x68]
  4068d0:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  4068d3:	83 c1 40             	add    ecx,0x40
  4068d6:	51                   	push   ecx
  4068d7:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  4068da:	50                   	push   eax
  4068db:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  4068de:	68 40 02 00 00       	push   0x240
  4068e3:	68 00 03 00 00       	push   0x300
  4068e8:	6a 00                	push   0x0
  4068ea:	6a 00                	push   0x0
  4068ec:	51                   	push   ecx
  4068ed:	52                   	push   edx
  4068ee:	50                   	push   eax
  4068ef:	e8 83 7d 04 00       	call   0x44e677
  4068f4:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
  4068f7:	8b 56 0c             	mov    edx,DWORD PTR [esi+0xc]
  4068fa:	6a 00                	push   0x0
  4068fc:	68 10 02 00 00       	push   0x210
  406901:	8d 48 40             	lea    ecx,[eax+0x40]
  406904:	83 c2 30             	add    edx,0x30
  406907:	51                   	push   ecx
  406908:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  40690e:	52                   	push   edx
  40690f:	6a 40                	push   0x40
  406911:	8b 51 68             	mov    edx,DWORD PTR [ecx+0x68]
  406914:	b9 80 02 00 00       	mov    ecx,0x280
  406919:	52                   	push   edx
  40691a:	8b 56 24             	mov    edx,DWORD PTR [esi+0x24]
  40691d:	68 40 02 00 00       	push   0x240
  406922:	68 00 03 00 00       	push   0x300
  406927:	2b c8                	sub    ecx,eax
  406929:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  40692c:	6a 00                	push   0x0
  40692e:	51                   	push   ecx
  40692f:	8b 4e 28             	mov    ecx,DWORD PTR [esi+0x28]
  406932:	52                   	push   edx
  406933:	50                   	push   eax
  406934:	51                   	push   ecx
  406935:	e8 3d 7d 04 00       	call   0x44e677
  40693a:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40693d:	83 c4 68             	add    esp,0x68
  406940:	85 c0                	test   eax,eax
  406942:	0f 84 6f 01 00 00    	je     0x406ab7
  406948:	3d e0 01 00 00       	cmp    eax,0x1e0
  40694d:	0f 84 64 01 00 00    	je     0x406ab7
  406953:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  406956:	8d 50 30             	lea    edx,[eax+0x30]
  406959:	6a 00                	push   0x0
  40695b:	52                   	push   edx
  40695c:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  406962:	68 c0 02 00 00       	push   0x2c0
  406967:	83 c1 40             	add    ecx,0x40
  40696a:	6a 30                	push   0x30
  40696c:	51                   	push   ecx
  40696d:	8b 4a 68             	mov    ecx,DWORD PTR [edx+0x68]
  406970:	ba e0 01 00 00       	mov    edx,0x1e0
  406975:	51                   	push   ecx
  406976:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  406979:	68 40 02 00 00       	push   0x240
  40697e:	2b d0                	sub    edx,eax
  406980:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  406983:	68 00 03 00 00       	push   0x300
  406988:	52                   	push   edx
  406989:	8b 56 28             	mov    edx,DWORD PTR [esi+0x28]
  40698c:	6a 00                	push   0x0
  40698e:	50                   	push   eax
  40698f:	51                   	push   ecx
  406990:	52                   	push   edx
  406991:	e8 e1 7c 04 00       	call   0x44e677
  406996:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  406999:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40699c:	6a 00                	push   0x0
  40699e:	8d 50 30             	lea    edx,[eax+0x30]
  4069a1:	52                   	push   edx
  4069a2:	8d 51 40             	lea    edx,[ecx+0x40]
  4069a5:	52                   	push   edx
  4069a6:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  4069ac:	6a 30                	push   0x30
  4069ae:	6a 40                	push   0x40
  4069b0:	8b 52 68             	mov    edx,DWORD PTR [edx+0x68]
  4069b3:	52                   	push   edx
  4069b4:	ba e0 01 00 00       	mov    edx,0x1e0
  4069b9:	2b d0                	sub    edx,eax
  4069bb:	b8 80 02 00 00       	mov    eax,0x280
  4069c0:	68 40 02 00 00       	push   0x240
  4069c5:	68 00 03 00 00       	push   0x300
  4069ca:	2b c1                	sub    eax,ecx
  4069cc:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  4069cf:	52                   	push   edx
  4069d0:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  4069d3:	50                   	push   eax
  4069d4:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  4069d7:	51                   	push   ecx
  4069d8:	52                   	push   edx
  4069d9:	50                   	push   eax
  4069da:	e8 98 7c 04 00       	call   0x44e677
  4069df:	83 c4 68             	add    esp,0x68
  4069e2:	5f                   	pop    edi
  4069e3:	5e                   	pop    esi
  4069e4:	5d                   	pop    ebp
  4069e5:	5b                   	pop    ebx
  4069e6:	c3                   	ret
  4069e7:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  4069ea:	83 f8 02             	cmp    eax,0x2
  4069ed:	74 6b                	je     0x406a5a
  4069ef:	83 f8 0a             	cmp    eax,0xa
  4069f2:	74 66                	je     0x406a5a
  4069f4:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  4069f9:	8b 5e 1c             	mov    ebx,DWORD PTR [esi+0x1c]
  4069fc:	85 db                	test   ebx,ebx
  4069fe:	8b 48 28             	mov    ecx,DWORD PTR [eax+0x28]
  406a01:	8b 78 70             	mov    edi,DWORD PTR [eax+0x70]
  406a04:	0f 9f c3             	setg   bl
  406a07:	8b 51 08             	mov    edx,DWORD PTR [ecx+0x8]
  406a0a:	8b 69 04             	mov    ebp,DWORD PTR [ecx+0x4]
  406a0d:	8b 48 6c             	mov    ecx,DWORD PTR [eax+0x6c]
  406a10:	53                   	push   ebx
  406a11:	8d 1c 17             	lea    ebx,[edi+edx*1]
  406a14:	83 c2 60             	add    edx,0x60
  406a17:	53                   	push   ebx
  406a18:	8d 1c 29             	lea    ebx,[ecx+ebp*1]
  406a1b:	53                   	push   ebx
  406a1c:	8b 1d 18 85 97 00    	mov    ebx,DWORD PTR ds:0x978518
  406a22:	57                   	push   edi
  406a23:	51                   	push   ecx
  406a24:	8b 48 68             	mov    ecx,DWORD PTR [eax+0x68]
  406a27:	8b 06                	mov    eax,DWORD PTR [esi]
  406a29:	51                   	push   ecx
  406a2a:	8b 0d 14 85 97 00    	mov    ecx,DWORD PTR ds:0x978514
  406a30:	52                   	push   edx
  406a31:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  406a34:	81 c5 80 00 00 00    	add    ebp,0x80
  406a3a:	03 d3                	add    edx,ebx
  406a3c:	55                   	push   ebp
  406a3d:	03 c1                	add    eax,ecx
  406a3f:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  406a42:	52                   	push   edx
  406a43:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  406a46:	50                   	push   eax
  406a47:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  406a4a:	51                   	push   ecx
  406a4b:	52                   	push   edx
  406a4c:	50                   	push   eax
  406a4d:	e8 25 7c 04 00       	call   0x44e677
  406a52:	83 c4 34             	add    esp,0x34
  406a55:	5f                   	pop    edi
  406a56:	5e                   	pop    esi
  406a57:	5d                   	pop    ebp
  406a58:	5b                   	pop    ebx
  406a59:	c3                   	ret
  406a5a:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  406a5f:	68 ff 07 00 00       	push   0x7ff
  406a64:	8b 48 28             	mov    ecx,DWORD PTR [eax+0x28]
  406a67:	8b 78 70             	mov    edi,DWORD PTR [eax+0x70]
  406a6a:	8b 58 6c             	mov    ebx,DWORD PTR [eax+0x6c]
  406a6d:	8b 40 68             	mov    eax,DWORD PTR [eax+0x68]
  406a70:	8b 51 08             	mov    edx,DWORD PTR [ecx+0x8]
  406a73:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  406a76:	8d 2c 17             	lea    ebp,[edi+edx*1]
  406a79:	83 c2 60             	add    edx,0x60
  406a7c:	55                   	push   ebp
  406a7d:	8d 2c 0b             	lea    ebp,[ebx+ecx*1]
  406a80:	55                   	push   ebp
  406a81:	57                   	push   edi
  406a82:	8b 2d 14 85 97 00    	mov    ebp,DWORD PTR ds:0x978514
  406a88:	53                   	push   ebx
  406a89:	50                   	push   eax
  406a8a:	81 c1 80 00 00 00    	add    ecx,0x80
  406a90:	52                   	push   edx
  406a91:	8b 15 18 85 97 00    	mov    edx,DWORD PTR ds:0x978518
  406a97:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  406a9a:	51                   	push   ecx
  406a9b:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  406a9e:	03 ca                	add    ecx,edx
  406aa0:	8b 16                	mov    edx,DWORD PTR [esi]
  406aa2:	03 d5                	add    edx,ebp
  406aa4:	51                   	push   ecx
  406aa5:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  406aa8:	52                   	push   edx
  406aa9:	8b 56 28             	mov    edx,DWORD PTR [esi+0x28]
  406aac:	50                   	push   eax
  406aad:	51                   	push   ecx
  406aae:	52                   	push   edx
  406aaf:	e8 6f 7c 04 00       	call   0x44e723
  406ab4:	83 c4 34             	add    esp,0x34
  406ab7:	5f                   	pop    edi
  406ab8:	5e                   	pop    esi
  406ab9:	5d                   	pop    ebp
  406aba:	5b                   	pop    ebx
  406abb:	c3                   	ret
  406abc:	90                   	nop
  406abd:	90                   	nop
  406abe:	90                   	nop
  406abf:	90                   	nop
