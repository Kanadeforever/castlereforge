
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00409860 <.text+0x8860>:
  409860:	83 ec 08             	sub    esp,0x8
  409863:	33 c0                	xor    eax,eax
  409865:	53                   	push   ebx
  409866:	56                   	push   esi
  409867:	8b f1                	mov    esi,ecx
  409869:	57                   	push   edi
  40986a:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  40986d:	c7 46 20 00 00 00 00 	mov    DWORD PTR [esi+0x20],0x0
  409874:	83 c1 02             	add    ecx,0x2
  409877:	85 c9                	test   ecx,ecx
  409879:	7e 25                	jle    0x4098a0
  40987b:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  40987e:	8b 0c 82             	mov    ecx,DWORD PTR [edx+eax*4]
  409881:	85 c9                	test   ecx,ecx
  409883:	74 10                	je     0x409895
  409885:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  409888:	8b 7e 30             	mov    edi,DWORD PTR [esi+0x30]
  40988b:	89 0c 97             	mov    DWORD PTR [edi+edx*4],ecx
  40988e:	8b 4e 20             	mov    ecx,DWORD PTR [esi+0x20]
  409891:	41                   	inc    ecx
  409892:	89 4e 20             	mov    DWORD PTR [esi+0x20],ecx
  409895:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  409898:	40                   	inc    eax
  409899:	83 c1 02             	add    ecx,0x2
  40989c:	3b c1                	cmp    eax,ecx
  40989e:	7c db                	jl     0x40987b
  4098a0:	8b 5e 20             	mov    ebx,DWORD PTR [esi+0x20]
  4098a3:	8b 56 30             	mov    edx,DWORD PTR [esi+0x30]
  4098a6:	4b                   	dec    ebx
  4098a7:	8b ce                	mov    ecx,esi
  4098a9:	8b c3                	mov    eax,ebx
  4098ab:	89 5e 20             	mov    DWORD PTR [esi+0x20],ebx
  4098ae:	48                   	dec    eax
  4098af:	50                   	push   eax
  4098b0:	6a 00                	push   0x0
  4098b2:	52                   	push   edx
  4098b3:	e8 18 07 00 00       	call   0x409fd0
  4098b8:	a1 08 f8 89 00       	mov    eax,ds:0x89f808
  4098bd:	85 c0                	test   eax,eax
  4098bf:	0f 87 77 01 00 00    	ja     0x409a3c
  4098c5:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  4098c8:	8b ce                	mov    ecx,esi
  4098ca:	50                   	push   eax
  4098cb:	e8 80 02 00 00       	call   0x409b50
  4098d0:	8b d8                	mov    ebx,eax
  4098d2:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  4098d5:	3b d8                	cmp    ebx,eax
  4098d7:	0f 84 41 01 00 00    	je     0x409a1e
  4098dd:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  4098e0:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  4098e3:	6a 02                	push   0x2
  4098e5:	8b 44 8a 04          	mov    eax,DWORD PTR [edx+ecx*4+0x4]
  4098e9:	8b 4e 30             	mov    ecx,DWORD PTR [esi+0x30]
  4098ec:	8b 0c 99             	mov    ecx,DWORD PTR [ecx+ebx*4]
  4098ef:	8b 78 18             	mov    edi,DWORD PTR [eax+0x18]
  4098f2:	e8 b9 0e 00 00       	call   0x40a7b0
  4098f7:	85 c0                	test   eax,eax
  4098f9:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  4098fd:	0f 84 1b 01 00 00    	je     0x409a1e
  409903:	83 f8 05             	cmp    eax,0x5
  409906:	55                   	push   ebp
  409907:	7d 50                	jge    0x409959
  409909:	8b 56 1c             	mov    edx,DWORD PTR [esi+0x1c]
  40990c:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  40990f:	8b 4e 30             	mov    ecx,DWORD PTR [esi+0x30]
  409912:	8b 14 90             	mov    edx,DWORD PTR [eax+edx*4]
  409915:	8b 0c 99             	mov    ecx,DWORD PTR [ecx+ebx*4]
  409918:	8b 6a 10             	mov    ebp,DWORD PTR [edx+0x10]
  40991b:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  40991e:	8b 49 14             	mov    ecx,DWORD PTR [ecx+0x14]
  409921:	2b c5                	sub    eax,ebp
  409923:	8b 6a 14             	mov    ebp,DWORD PTR [edx+0x14]
  409926:	2b cd                	sub    ecx,ebp
  409928:	8b d1                	mov    edx,ecx
  40992a:	0f af d1             	imul   edx,ecx
  40992d:	8b c8                	mov    ecx,eax
  40992f:	0f af c8             	imul   ecx,eax
  409932:	03 d1                	add    edx,ecx
  409934:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  409938:	db 44 24 10          	fild   DWORD PTR [esp+0x10]
  40993c:	d9 fa                	fsqrt
  40993e:	dc 1d 90 02 46 00    	fcomp  QWORD PTR ds:0x460290
  409944:	df e0                	fnstsw ax
  409946:	f6 c4 01             	test   ah,0x1
  409949:	75 0e                	jne    0x409959
  40994b:	81 e7 ff 01 ff ff    	and    edi,0xffff01ff
  409951:	81 cf 00 01 00 00    	or     edi,0x100
  409957:	eb 06                	jmp    0x40995f
  409959:	81 e7 ff 00 ff ff    	and    edi,0xffff00ff
  40995f:	8b ef                	mov    ebp,edi
  409961:	f7 c5 00 ff 00 00    	test   ebp,0xff00
  409967:	75 5c                	jne    0x4099c5
  409969:	83 7c 24 1c 01       	cmp    DWORD PTR [esp+0x1c],0x1
  40996e:	75 55                	jne    0x4099c5
  409970:	8b 56 30             	mov    edx,DWORD PTR [esi+0x30]
  409973:	8b 04 9a             	mov    eax,DWORD PTR [edx+ebx*4]
  409976:	33 d2                	xor    edx,edx
  409978:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  40997b:	8a 91 eb 00 00 00    	mov    dl,BYTE PTR [ecx+0xeb]
  409981:	52                   	push   edx
  409982:	e8 a9 18 00 00       	call   0x40b230
  409987:	8b 46 30             	mov    eax,DWORD PTR [esi+0x30]
  40998a:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  40998d:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  409990:	83 c4 04             	add    esp,0x4
  409993:	8b 1c 98             	mov    ebx,DWORD PTR [eax+ebx*4]
  409996:	39 1c 8a             	cmp    DWORD PTR [edx+ecx*4],ebx
  409999:	74 2a                	je     0x4099c5
  40999b:	c6 43 54 01          	mov    BYTE PTR [ebx+0x54],0x1
  40999f:	e8 6c f2 ff ff       	call   0x408c10
  4099a4:	8b 3d d4 f7 89 00    	mov    edi,DWORD PTR ds:0x89f7d4
  4099aa:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  4099ad:	81 e7 ff 00 00 00    	and    edi,0xff
  4099b3:	25 ff ff 00 ff       	and    eax,0xff00ffff
  4099b8:	c1 e7 10             	shl    edi,0x10
  4099bb:	0b f8                	or     edi,eax
  4099bd:	e8 4e f2 ff ff       	call   0x408c10
  4099c2:	89 78 18             	mov    DWORD PTR [eax+0x18],edi
  4099c5:	a1 d0 f7 89 00       	mov    eax,ds:0x89f7d0
  4099ca:	81 e5 02 ff ff ff    	and    ebp,0xffffff02
  4099d0:	83 cd 02             	or     ebp,0x2
  4099d3:	85 c0                	test   eax,eax
  4099d5:	75 17                	jne    0x4099ee
  4099d7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4099db:	05 ff ff 00 00       	add    eax,0xffff
  4099e0:	c1 e0 10             	shl    eax,0x10
  4099e3:	33 c5                	xor    eax,ebp
  4099e5:	25 00 00 ff 00       	and    eax,0xff0000
  4099ea:	33 e8                	xor    ebp,eax
  4099ec:	eb 0c                	jmp    0x4099fa
  4099ee:	81 e5 ff ff 04 ff    	and    ebp,0xff04ffff
  4099f4:	81 cd 00 00 04 00    	or     ebp,0x40000
  4099fa:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  4099fd:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  409a00:	68 07 b2 01 00       	push   0x1b207
  409a05:	68 07 b2 01 00       	push   0x1b207
  409a0a:	8b 4c 8a 04          	mov    ecx,DWORD PTR [edx+ecx*4+0x4]
  409a0e:	55                   	push   ebp
  409a0f:	e8 9c 08 00 00       	call   0x40a2b0
  409a14:	5d                   	pop    ebp
  409a15:	5f                   	pop    edi
  409a16:	5e                   	pop    esi
  409a17:	5b                   	pop    ebx
  409a18:	83 c4 08             	add    esp,0x8
  409a1b:	c2 04 00             	ret    0x4
  409a1e:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409a21:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  409a24:	68 07 b2 01 00       	push   0x1b207
  409a29:	68 07 b2 01 00       	push   0x1b207
  409a2e:	8b 4c 81 04          	mov    ecx,DWORD PTR [ecx+eax*4+0x4]
  409a32:	68 07 b2 01 00       	push   0x1b207
  409a37:	e8 74 08 00 00       	call   0x40a2b0
  409a3c:	5f                   	pop    edi
  409a3d:	5e                   	pop    esi
  409a3e:	5b                   	pop    ebx
  409a3f:	83 c4 08             	add    esp,0x8
  409a42:	c2 04 00             	ret    0x4
  409a45:	90                   	nop
  409a46:	90                   	nop
  409a47:	90                   	nop
  409a48:	90                   	nop
  409a49:	90                   	nop
  409a4a:	90                   	nop
  409a4b:	90                   	nop
  409a4c:	90                   	nop
  409a4d:	90                   	nop
  409a4e:	90                   	nop
  409a4f:	90                   	nop
