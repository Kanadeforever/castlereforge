
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00408830 <.text+0x7830>:
  408830:	68 c0 f7 89 00       	push   0x89f7c0
  408835:	ff 15 04 02 46 00    	call   DWORD PTR ds:0x460204
  40883b:	85 c0                	test   eax,eax
  40883d:	75 17                	jne    0x408856
  40883f:	8d 44 24 04          	lea    eax,[esp+0x4]
  408843:	68 d8 37 46 00       	push   0x4637d8
  408848:	50                   	push   eax
  408849:	c7 44 24 0c bc 8b 46 	mov    DWORD PTR [esp+0xc],0x468bbc
  408850:	00 
  408851:	e8 71 93 04 00       	call   0x451bc7
  408856:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  40885c:	8b 4a 28             	mov    ecx,DWORD PTR [edx+0x28]
  40885f:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  408862:	8b 0d c0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7c0
  408868:	3b c8                	cmp    ecx,eax
  40886a:	7e 08                	jle    0x408874
  40886c:	8b c8                	mov    ecx,eax
  40886e:	89 0d c0 f7 89 00    	mov    DWORD PTR ds:0x89f7c0,ecx
  408874:	8b 52 28             	mov    edx,DWORD PTR [edx+0x28]
  408877:	a1 c4 f7 89 00       	mov    eax,ds:0x89f7c4
  40887c:	8b 52 08             	mov    edx,DWORD PTR [edx+0x8]
  40887f:	3b c2                	cmp    eax,edx
  408881:	7e 02                	jle    0x408885
  408883:	8b c2                	mov    eax,edx
  408885:	8b 15 14 85 97 00    	mov    edx,DWORD PTR ds:0x978514
  40888b:	56                   	push   esi
  40888c:	57                   	push   edi
  40888d:	8b 3d 18 85 97 00    	mov    edi,DWORD PTR ds:0x978518
  408893:	03 c7                	add    eax,edi
  408895:	03 ca                	add    ecx,edx
  408897:	68 d4 f7 89 00       	push   0x89f7d4
  40889c:	a3 c4 f7 89 00       	mov    ds:0x89f7c4,eax
  4088a1:	50                   	push   eax
  4088a2:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4088a6:	89 0d c0 f7 89 00    	mov    DWORD PTR ds:0x89f7c0,ecx
  4088ac:	51                   	push   ecx
  4088ad:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4088b1:	50                   	push   eax
  4088b2:	51                   	push   ecx
  4088b3:	c7 05 d4 f7 89 00 00 	mov    DWORD PTR ds:0x89f7d4,0x0
  4088ba:	00 00 00 
  4088bd:	e8 8e 00 00 00       	call   0x408950
  4088c2:	8b f8                	mov    edi,eax
  4088c4:	a1 d0 f7 89 00       	mov    eax,ds:0x89f7d0
  4088c9:	83 c4 14             	add    esp,0x14
  4088cc:	85 c0                	test   eax,eax
  4088ce:	75 11                	jne    0x4088e1
  4088d0:	8b 35 d4 f7 89 00    	mov    esi,DWORD PTR ds:0x89f7d4
  4088d6:	81 e6 ff 00 00 00    	and    esi,0xff
  4088dc:	c1 e6 10             	shl    esi,0x10
  4088df:	eb 05                	jmp    0x4088e6
  4088e1:	be 02 00 05 00       	mov    esi,0x50002
  4088e6:	8b 15 c4 f7 89 00    	mov    edx,DWORD PTR ds:0x89f7c4
  4088ec:	a1 c0 f7 89 00       	mov    eax,ds:0x89f7c0
  4088f1:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  4088f7:	52                   	push   edx
  4088f8:	50                   	push   eax
  4088f9:	e8 02 1e 00 00       	call   0x40a700
  4088fe:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  408904:	8b c7                	mov    eax,edi
  408906:	5f                   	pop    edi
  408907:	89 71 18             	mov    DWORD PTR [ecx+0x18],esi
  40890a:	5e                   	pop    esi
  40890b:	c3                   	ret
  40890c:	90                   	nop
  40890d:	90                   	nop
  40890e:	90                   	nop
  40890f:	90                   	nop
  408910:	51                   	push   ecx
  408911:	a1 d0 f7 89 00       	mov    eax,ds:0x89f7d0
  408916:	85 c0                	test   eax,eax
  408918:	75 2d                	jne    0x408947
  40891a:	8b 44 24 00          	mov    eax,DWORD PTR [esp+0x0]
  40891e:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  408924:	25 00 00 07 ff       	and    eax,0xff070000
  408929:	68 07 b2 01 00       	push   0x1b207
  40892e:	0d 00 00 07 00       	or     eax,0x70000
  408933:	68 07 b2 01 00       	push   0x1b207
  408938:	89 41 18             	mov    DWORD PTR [ecx+0x18],eax
  40893b:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  408941:	50                   	push   eax
  408942:	e8 69 19 00 00       	call   0x40a2b0
  408947:	59                   	pop    ecx
  408948:	c3                   	ret
  408949:	90                   	nop
  40894a:	90                   	nop
  40894b:	90                   	nop
  40894c:	90                   	nop
  40894d:	90                   	nop
  40894e:	90                   	nop
  40894f:	90                   	nop
  408950:	55                   	push   ebp
  408951:	56                   	push   esi
  408952:	57                   	push   edi
  408953:	8b 3d a8 01 46 00    	mov    edi,DWORD PTR ds:0x4601a8
  408959:	33 ed                	xor    ebp,ebp
  40895b:	be 21 00 00 00       	mov    esi,0x21
  408960:	56                   	push   esi
  408961:	ff d7                	call   edi
  408963:	0f bf c0             	movsx  eax,ax
  408966:	a9 00 80 ff ff       	test   eax,0xffff8000
  40896b:	74 02                	je     0x40896f
  40896d:	8b ee                	mov    ebp,esi
  40896f:	46                   	inc    esi
  408970:	8d 4e df             	lea    ecx,[esi-0x21]
  408973:	83 f9 08             	cmp    ecx,0x8
  408976:	7c e8                	jl     0x408960
  408978:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  40897c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  408980:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
  408984:	ba 40 00 00 00       	mov    edx,0x40
  408989:	2b c8                	sub    ecx,eax
  40898b:	2b d6                	sub    edx,esi
  40898d:	8b 74 24 1c          	mov    esi,DWORD PTR [esp+0x1c]
  408991:	8b c1                	mov    eax,ecx
  408993:	03 f2                	add    esi,edx
  408995:	99                   	cdq
  408996:	33 c2                	xor    eax,edx
  408998:	2b c2                	sub    eax,edx
  40899a:	83 f8 14             	cmp    eax,0x14
  40899d:	7d 12                	jge    0x4089b1
  40899f:	8b c6                	mov    eax,esi
  4089a1:	99                   	cdq
  4089a2:	33 c2                	xor    eax,edx
  4089a4:	2b c2                	sub    eax,edx
  4089a6:	83 f8 14             	cmp    eax,0x14
  4089a9:	7d 06                	jge    0x4089b1
  4089ab:	5f                   	pop    edi
  4089ac:	5e                   	pop    esi
  4089ad:	33 c0                	xor    eax,eax
  4089af:	5d                   	pop    ebp
  4089b0:	c3                   	ret
  4089b1:	8d 04 c9             	lea    eax,[ecx+ecx*8]
  4089b4:	c1 e0 06             	shl    eax,0x6
  4089b7:	03 c1                	add    eax,ecx
  4089b9:	8d 14 81             	lea    edx,[ecx+eax*4]
  4089bc:	8d 04 51             	lea    eax,[ecx+edx*2]
  4089bf:	8d 14 b6             	lea    edx,[esi+esi*4]
  4089c2:	c1 e2 06             	shl    edx,0x6
  4089c5:	2b d6                	sub    edx,esi
  4089c7:	8d 04 41             	lea    eax,[ecx+eax*2]
  4089ca:	8d 14 52             	lea    edx,[edx+edx*2]
  4089cd:	c1 e2 02             	shl    edx,0x2
  4089d0:	2b d6                	sub    edx,esi
  4089d2:	2b c2                	sub    eax,edx
  4089d4:	8d 14 f6             	lea    edx,[esi+esi*8]
  4089d7:	c1 e2 06             	shl    edx,0x6
  4089da:	03 d6                	add    edx,esi
  4089dc:	8d 14 96             	lea    edx,[esi+edx*4]
  4089df:	8d 14 56             	lea    edx,[esi+edx*2]
  4089e2:	8d 14 56             	lea    edx,[esi+edx*2]
  4089e5:	8b f2                	mov    esi,edx
  4089e7:	8d 14 89             	lea    edx,[ecx+ecx*4]
  4089ea:	c1 e2 06             	shl    edx,0x6
  4089ed:	2b d1                	sub    edx,ecx
  4089ef:	8d 14 52             	lea    edx,[edx+edx*2]
  4089f2:	c1 e2 02             	shl    edx,0x2
  4089f5:	2b d1                	sub    edx,ecx
  4089f7:	f7 da                	neg    edx
  4089f9:	8b ca                	mov    ecx,edx
  4089fb:	2b ce                	sub    ecx,esi
  4089fd:	85 c0                	test   eax,eax
  4089ff:	7c 1c                	jl     0x408a1d
  408a01:	85 c9                	test   ecx,ecx
  408a03:	7c 0c                	jl     0x408a11
  408a05:	33 d2                	xor    edx,edx
  408a07:	3b c1                	cmp    eax,ecx
  408a09:	0f 9c c2             	setl   dl
  408a0c:	83 c2 03             	add    edx,0x3
  408a0f:	eb 2d                	jmp    0x408a3e
  408a11:	f7 d9                	neg    ecx
  408a13:	33 d2                	xor    edx,edx
  408a15:	3b c1                	cmp    eax,ecx
  408a17:	0f 9d c2             	setge  dl
  408a1a:	42                   	inc    edx
  408a1b:	eb 21                	jmp    0x408a3e
  408a1d:	85 c9                	test   ecx,ecx
  408a1f:	7c 0e                	jl     0x408a2f
  408a21:	f7 d8                	neg    eax
  408a23:	33 d2                	xor    edx,edx
  408a25:	3b c1                	cmp    eax,ecx
  408a27:	0f 9d c2             	setge  dl
  408a2a:	83 c2 05             	add    edx,0x5
  408a2d:	eb 0f                	jmp    0x408a3e
  408a2f:	f7 d9                	neg    ecx
  408a31:	f7 d8                	neg    eax
  408a33:	33 d2                	xor    edx,edx
  408a35:	3b c1                	cmp    eax,ecx
  408a37:	0f 9c c2             	setl   dl
  408a3a:	4a                   	dec    edx
  408a3b:	83 e2 07             	and    edx,0x7
  408a3e:	8b 74 24 20          	mov    esi,DWORD PTR [esp+0x20]
  408a42:	6a 02                	push   0x2
  408a44:	89 16                	mov    DWORD PTR [esi],edx
  408a46:	ff d7                	call   edi
  408a48:	0f bf c0             	movsx  eax,ax
  408a4b:	a9 00 80 ff ff       	test   eax,0xffff8000
  408a50:	74 09                	je     0x408a5b
  408a52:	8b 0e                	mov    ecx,DWORD PTR [esi]
  408a54:	8b 2c 8d 98 8b 46 00 	mov    ebp,DWORD PTR [ecx*4+0x468b98]
  408a5b:	5f                   	pop    edi
  408a5c:	8b c5                	mov    eax,ebp
  408a5e:	5e                   	pop    esi
  408a5f:	5d                   	pop    ebp
  408a60:	c3                   	ret
  408a61:	90                   	nop
  408a62:	90                   	nop
  408a63:	90                   	nop
  408a64:	90                   	nop
  408a65:	90                   	nop
  408a66:	90                   	nop
  408a67:	90                   	nop
  408a68:	90                   	nop
  408a69:	90                   	nop
  408a6a:	90                   	nop
  408a6b:	90                   	nop
  408a6c:	90                   	nop
  408a6d:	90                   	nop
  408a6e:	90                   	nop
  408a6f:	90                   	nop
  408a70:	56                   	push   esi
  408a71:	57                   	push   edi
  408a72:	8b 3d a8 01 46 00    	mov    edi,DWORD PTR ds:0x4601a8
  408a78:	6a 02                	push   0x2
  408a7a:	33 f6                	xor    esi,esi
  408a7c:	ff d7                	call   edi
  408a7e:	0f bf c0             	movsx  eax,ax
  408a81:	a9 00 80 ff ff       	test   eax,0xffff8000
  408a86:	6a 01                	push   0x1
  408a88:	74 17                	je     0x408aa1
  408a8a:	ff d7                	call   edi
  408a8c:	0f bf f0             	movsx  esi,ax
  408a8f:	81 e6 00 80 ff ff    	and    esi,0xffff8000
  408a95:	f7 de                	neg    esi
  408a97:	1b f6                	sbb    esi,esi
  408a99:	83 e6 50             	and    esi,0x50
  408a9c:	83 c6 02             	add    esi,0x2
  408a9f:	eb 12                	jmp    0x408ab3
  408aa1:	ff d7                	call   edi
  408aa3:	0f bf c8             	movsx  ecx,ax
  408aa6:	f7 c1 00 80 ff ff    	test   ecx,0xffff8000
  408aac:	74 05                	je     0x408ab3
  408aae:	be 01 00 00 00       	mov    esi,0x1
  408ab3:	3b 35 e4 f7 89 00    	cmp    esi,DWORD PTR ds:0x89f7e4
  408ab9:	74 0b                	je     0x408ac6
  408abb:	89 35 e4 f7 89 00    	mov    DWORD PTR ds:0x89f7e4,esi
  408ac1:	8b c6                	mov    eax,esi
  408ac3:	5f                   	pop    edi
  408ac4:	5e                   	pop    esi
  408ac5:	c3                   	ret
  408ac6:	5f                   	pop    edi
  408ac7:	33 c0                	xor    eax,eax
  408ac9:	5e                   	pop    esi
  408aca:	c3                   	ret
  408acb:	90                   	nop
  408acc:	90                   	nop
  408acd:	90                   	nop
  408ace:	90                   	nop
  408acf:	90                   	nop
  408ad0:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  408ad6:	6a ff                	push   0xffffffff
  408ad8:	68 3b d0 45 00       	push   0x45d03b
  408add:	50                   	push   eax
  408ade:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  408ae2:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  408ae9:	53                   	push   ebx
  408aea:	56                   	push   esi
  408aeb:	57                   	push   edi
  408aec:	6a 02                	push   0x2
  408aee:	68 ec f7 89 00       	push   0x89f7ec
  408af3:	50                   	push   eax
  408af4:	e8 b7 9a ff ff       	call   0x4025b0
  408af9:	8b c8                	mov    ecx,eax
  408afb:	b8 7d 55 78 3b       	mov    eax,0x3b78557d
  408b00:	f7 e1                	mul    ecx
  408b02:	c1 ea 07             	shr    edx,0x7
  408b05:	8b f2                	mov    esi,edx
  408b07:	89 15 e8 f7 89 00    	mov    DWORD PTR ds:0x89f7e8,edx
  408b0d:	8d 14 f5 00 00 00 00 	lea    edx,[esi*8+0x0]
  408b14:	2b d6                	sub    edx,esi
  408b16:	8d 04 96             	lea    eax,[esi+edx*4]
  408b19:	8d 0c 85 04 00 00 00 	lea    ecx,[eax*4+0x4]
  408b20:	51                   	push   ecx
  408b21:	e8 39 8b 04 00       	call   0x45165f
  408b26:	83 c4 10             	add    esp,0x10
  408b29:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  408b2d:	33 db                	xor    ebx,ebx
  408b2f:	3b c3                	cmp    eax,ebx
  408b31:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  408b35:	74 1a                	je     0x408b51
  408b37:	68 30 a1 40 00       	push   0x40a130
  408b3c:	68 e0 a0 40 00       	push   0x40a0e0
  408b41:	8d 78 04             	lea    edi,[eax+0x4]
  408b44:	56                   	push   esi
  408b45:	6a 74                	push   0x74
  408b47:	57                   	push   edi
  408b48:	89 30                	mov    DWORD PTR [eax],esi
  408b4a:	e8 80 96 04 00       	call   0x4521cf
  408b4f:	eb 02                	jmp    0x408b53
  408b51:	33 ff                	xor    edi,edi
  408b53:	a1 e8 f7 89 00       	mov    eax,ds:0x89f7e8
  408b58:	c7 44 24 14 ff ff ff 	mov    DWORD PTR [esp+0x14],0xffffffff
  408b5f:	ff 
  408b60:	3b c3                	cmp    eax,ebx
  408b62:	89 3d f0 f7 89 00    	mov    DWORD PTR ds:0x89f7f0,edi
  408b68:	76 30                	jbe    0x408b9a
  408b6a:	33 ff                	xor    edi,edi
  408b6c:	33 f6                	xor    esi,esi
  408b6e:	8b 15 ec f7 89 00    	mov    edx,DWORD PTR ds:0x89f7ec
  408b74:	8b 0d f0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7f0
  408b7a:	6a 02                	push   0x2
  408b7c:	03 cf                	add    ecx,edi
  408b7e:	8d 04 16             	lea    eax,[esi+edx*1]
  408b81:	50                   	push   eax
  408b82:	e8 d9 15 00 00       	call   0x40a160
  408b87:	a1 e8 f7 89 00       	mov    eax,ds:0x89f7e8
  408b8c:	43                   	inc    ebx
  408b8d:	81 c6 27 02 00 00    	add    esi,0x227
  408b93:	83 c7 74             	add    edi,0x74
  408b96:	3b d8                	cmp    ebx,eax
  408b98:	72 d4                	jb     0x408b6e
  408b9a:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  408b9e:	5f                   	pop    edi
  408b9f:	5e                   	pop    esi
  408ba0:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  408ba7:	5b                   	pop    ebx
  408ba8:	83 c4 0c             	add    esp,0xc
  408bab:	c3                   	ret
  408bac:	90                   	nop
  408bad:	90                   	nop
  408bae:	90                   	nop
  408baf:	90                   	nop
  408bb0:	a1 ec f7 89 00       	mov    eax,ds:0x89f7ec
  408bb5:	c7 05 e8 f7 89 00 00 	mov    DWORD PTR ds:0x89f7e8,0x0
  408bbc:	00 00 00 
  408bbf:	85 c0                	test   eax,eax
  408bc1:	74 13                	je     0x408bd6
  408bc3:	50                   	push   eax
  408bc4:	e8 87 89 04 00       	call   0x451550
  408bc9:	83 c4 04             	add    esp,0x4
  408bcc:	c7 05 ec f7 89 00 00 	mov    DWORD PTR ds:0x89f7ec,0x0
  408bd3:	00 00 00 
  408bd6:	a1 f0 f7 89 00       	mov    eax,ds:0x89f7f0
  408bdb:	85 c0                	test   eax,eax
  408bdd:	74 29                	je     0x408c08
  408bdf:	8b 48 fc             	mov    ecx,DWORD PTR [eax-0x4]
  408be2:	56                   	push   esi
  408be3:	8d 70 fc             	lea    esi,[eax-0x4]
  408be6:	68 30 a1 40 00       	push   0x40a130
  408beb:	51                   	push   ecx
  408bec:	6a 74                	push   0x74
  408bee:	50                   	push   eax
  408bef:	e8 5d 96 04 00       	call   0x452251
  408bf4:	56                   	push   esi
  408bf5:	e8 56 89 04 00       	call   0x451550
  408bfa:	83 c4 04             	add    esp,0x4
  408bfd:	c7 05 f0 f7 89 00 00 	mov    DWORD PTR ds:0x89f7f0,0x0
  408c04:	00 00 00 
  408c07:	5e                   	pop    esi
  408c08:	c3                   	ret
  408c09:	90                   	nop
  408c0a:	90                   	nop
  408c0b:	90                   	nop
  408c0c:	90                   	nop
  408c0d:	90                   	nop
  408c0e:	90                   	nop
  408c0f:	90                   	nop
  408c10:	a1 f0 8b 46 00       	mov    eax,ds:0x468bf0
  408c15:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  408c1c:	2b c8                	sub    ecx,eax
  408c1e:	8d 14 88             	lea    edx,[eax+ecx*4]
  408c21:	a1 f0 f7 89 00       	mov    eax,ds:0x89f7f0
  408c26:	8d 04 90             	lea    eax,[eax+edx*4]
  408c29:	c3                   	ret
  408c2a:	90                   	nop
  408c2b:	90                   	nop
  408c2c:	90                   	nop
  408c2d:	90                   	nop
  408c2e:	90                   	nop
  408c2f:	90                   	nop
  408c30:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  408c34:	a3 f0 8b 46 00       	mov    ds:0x468bf0,eax
  408c39:	c3                   	ret
  408c3a:	90                   	nop
  408c3b:	90                   	nop
  408c3c:	90                   	nop
  408c3d:	90                   	nop
  408c3e:	90                   	nop
  408c3f:	90                   	nop
  408c40:	56                   	push   esi
  408c41:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  408c45:	85 f6                	test   esi,esi
  408c47:	74 0a                	je     0x408c53
  408c49:	c7 05 f8 f7 89 00 00 	mov    DWORD PTR ds:0x89f7f8,0x0
  408c50:	00 00 00 
  408c53:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  408c57:	50                   	push   eax
  408c58:	a1 f0 8b 46 00       	mov    eax,ds:0x468bf0
  408c5d:	56                   	push   esi
  408c5e:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  408c65:	2b c8                	sub    ecx,eax
  408c67:	8d 14 88             	lea    edx,[eax+ecx*4]
  408c6a:	a1 f0 f7 89 00       	mov    eax,ds:0x89f7f0
  408c6f:	8d 0c 90             	lea    ecx,[eax+edx*4]
  408c72:	e8 99 18 00 00       	call   0x40a510
  408c77:	85 f6                	test   esi,esi
  408c79:	5e                   	pop    esi
  408c7a:	74 49                	je     0x408cc5
  408c7c:	a1 f0 8b 46 00       	mov    eax,ds:0x468bf0
  408c81:	c7 05 f4 f7 89 00 00 	mov    DWORD PTR ds:0x89f7f4,0x0
  408c88:	00 00 00 
  408c8b:	85 c0                	test   eax,eax
  408c8d:	75 0a                	jne    0x408c99
  408c8f:	6a 01                	push   0x1
  408c91:	e8 8a 00 00 00       	call   0x408d20
  408c96:	83 c4 04             	add    esp,0x4
  408c99:	e8 72 ff ff ff       	call   0x408c10
  408c9e:	8b 40 18             	mov    eax,DWORD PTR [eax+0x18]
  408ca1:	33 c9                	xor    ecx,ecx
  408ca3:	8a cc                	mov    cl,ah
  408ca5:	8b c1                	mov    eax,ecx
  408ca7:	83 e8 00             	sub    eax,0x0
  408caa:	74 0e                	je     0x408cba
  408cac:	48                   	dec    eax
  408cad:	75 66                	jne    0x408d15
  408caf:	6a 02                	push   0x2
  408cb1:	e8 0a a8 ff ff       	call   0x4034c0
  408cb6:	83 c4 04             	add    esp,0x4
  408cb9:	c3                   	ret
  408cba:	6a 01                	push   0x1
  408cbc:	e8 ff a7 ff ff       	call   0x4034c0
  408cc1:	83 c4 04             	add    esp,0x4
  408cc4:	c3                   	ret
  408cc5:	81 3d f4     	cmp    DWORD PTR ds:0x89f7f4,0x960
  408ccc:	   
