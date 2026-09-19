
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00422850 <.text+0x21850>:
  422850:	51                   	push   ecx
  422851:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  422855:	53                   	push   ebx
  422856:	55                   	push   ebp
  422857:	56                   	push   esi
  422858:	85 c0                	test   eax,eax
  42285a:	89 4c 24 0c          	mov    DWORD PTR [esp+0xc],ecx
  42285e:	0f 8c d1 01 00 00    	jl     0x422a35
  422864:	83 f8 10             	cmp    eax,0x10
  422867:	0f 8d c8 01 00 00    	jge    0x422a35
  42286d:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  422874:	8b 6c 24 24          	mov    ebp,DWORD PTR [esp+0x24]
  422878:	85 ed                	test   ebp,ebp
  42287a:	8b b0 9c 0b 00 00    	mov    esi,DWORD PTR [eax+0xb9c]
  422880:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  422887:	00 
  422888:	bb 01 00 00 00       	mov    ebx,0x1
  42288d:	0f 84 a2 01 00 00    	je     0x422a35
  422893:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  422897:	3b cb                	cmp    ecx,ebx
  422899:	7e 02                	jle    0x42289d
  42289b:	8b cb                	mov    ecx,ebx
  42289d:	85 c9                	test   ecx,ecx
  42289f:	57                   	push   edi
  4228a0:	7e 07                	jle    0x4228a9
  4228a2:	83 c8 ff             	or     eax,0xffffffff
  4228a5:	8b fd                	mov    edi,ebp
  4228a7:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4228a9:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  4228ad:	83 f8 03             	cmp    eax,0x3
  4228b0:	77 2b                	ja     0x4228dd
  4228b2:	ff 24 85 40 2a 42 00 	jmp    DWORD PTR [eax*4+0x422a40]
  4228b9:	bb 01 00 00 00       	mov    ebx,0x1
  4228be:	eb 1d                	jmp    0x4228dd
  4228c0:	bb 02 00 00 00       	mov    ebx,0x2
  4228c5:	eb 16                	jmp    0x4228dd
  4228c7:	bb 04 00 00 00       	mov    ebx,0x4
  4228cc:	eb 0f                	jmp    0x4228dd
  4228ce:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  4228d2:	33 db                	xor    ebx,ebx
  4228d4:	83 f9 02             	cmp    ecx,0x2
  4228d7:	0f 94 c3             	sete   bl
  4228da:	83 c3 05             	add    ebx,0x5
  4228dd:	83 fb 01             	cmp    ebx,0x1
  4228e0:	7e 40                	jle    0x422922
  4228e2:	32 c9                	xor    cl,cl
  4228e4:	83 fe 14             	cmp    esi,0x14
  4228e7:	7c 05                	jl     0x4228ee
  4228e9:	b1 01                	mov    cl,0x1
  4228eb:	83 c6 ec             	add    esi,0xffffffec
  4228ee:	83 fb 02             	cmp    ebx,0x2
  4228f1:	75 0f                	jne    0x422902
  4228f3:	81 e6 03 00 00 80    	and    esi,0x80000003
  4228f9:	79 20                	jns    0x42291b
  4228fb:	4e                   	dec    esi
  4228fc:	83 ce fc             	or     esi,0xfffffffc
  4228ff:	46                   	inc    esi
  422900:	eb 19                	jmp    0x42291b
  422902:	83 fb 04             	cmp    ebx,0x4
  422905:	75 12                	jne    0x422919
  422907:	8b c6                	mov    eax,esi
  422909:	99                   	cdq
  42290a:	83 e2 03             	and    edx,0x3
  42290d:	03 c2                	add    eax,edx
  42290f:	c1 f8 02             	sar    eax,0x2
  422912:	c1 e0 02             	shl    eax,0x2
  422915:	8b f0                	mov    esi,eax
  422917:	eb 02                	jmp    0x42291b
  422919:	33 f6                	xor    esi,esi
  42291b:	84 c9                	test   cl,cl
  42291d:	74 03                	je     0x422922
  42291f:	83 c6 14             	add    esi,0x14
  422922:	4b                   	dec    ebx
  422923:	83 fb 05             	cmp    ebx,0x5
  422926:	0f 87 fd 00 00 00    	ja     0x422a29
  42292c:	ff 24 9d 50 2a 42 00 	jmp    DWORD PTR [ebx*4+0x422a50]
  422933:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  422937:	56                   	push   esi
  422938:	e8 33 01 00 00       	call   0x422a70
  42293d:	83 f8 ff             	cmp    eax,0xffffffff
  422940:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  422943:	0f 84 e0 00 00 00    	je     0x422a29
  422949:	5f                   	pop    edi
  42294a:	c7 44 24 14 01 00 00 	mov    DWORD PTR [esp+0x14],0x1
  422951:	00 
  422952:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  422956:	5e                   	pop    esi
  422957:	5d                   	pop    ebp
  422958:	5b                   	pop    ebx
  422959:	59                   	pop    ecx
  42295a:	c2 14 00             	ret    0x14
  42295d:	8b fd                	mov    edi,ebp
  42295f:	bb 02 00 00 00       	mov    ebx,0x2
  422964:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  422968:	56                   	push   esi
  422969:	e8 02 01 00 00       	call   0x422a70
  42296e:	83 f8 ff             	cmp    eax,0xffffffff
  422971:	89 07                	mov    DWORD PTR [edi],eax
  422973:	74 0c                	je     0x422981
  422975:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  422979:	41                   	inc    ecx
  42297a:	83 c7 04             	add    edi,0x4
  42297d:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  422981:	83 c6 04             	add    esi,0x4
  422984:	4b                   	dec    ebx
  422985:	75 dd                	jne    0x422964
  422987:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  42298b:	5f                   	pop    edi
  42298c:	5e                   	pop    esi
  42298d:	5d                   	pop    ebp
  42298e:	5b                   	pop    ebx
  42298f:	59                   	pop    ecx
  422990:	c2 14 00             	ret    0x14
  422993:	8b fd                	mov    edi,ebp
  422995:	bb 04 00 00 00       	mov    ebx,0x4
  42299a:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  42299e:	56                   	push   esi
  42299f:	e8 cc 00 00 00       	call   0x422a70
  4229a4:	83 f8 ff             	cmp    eax,0xffffffff
  4229a7:	89 07                	mov    DWORD PTR [edi],eax
  4229a9:	74 0c                	je     0x4229b7
  4229ab:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4229af:	41                   	inc    ecx
  4229b0:	83 c7 04             	add    edi,0x4
  4229b3:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  4229b7:	46                   	inc    esi
  4229b8:	4b                   	dec    ebx
  4229b9:	75 df                	jne    0x42299a
  4229bb:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4229bf:	5f                   	pop    edi
  4229c0:	5e                   	pop    esi
  4229c1:	5d                   	pop    ebp
  4229c2:	5b                   	pop    ebx
  4229c3:	59                   	pop    ecx
  4229c4:	c2 14 00             	ret    0x14
  4229c7:	8b fd                	mov    edi,ebp
  4229c9:	bb 08 00 00 00       	mov    ebx,0x8
  4229ce:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  4229d2:	56                   	push   esi
  4229d3:	e8 98 00 00 00       	call   0x422a70
  4229d8:	83 f8 ff             	cmp    eax,0xffffffff
  4229db:	89 07                	mov    DWORD PTR [edi],eax
  4229dd:	74 0c                	je     0x4229eb
  4229df:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4229e3:	41                   	inc    ecx
  4229e4:	83 c7 04             	add    edi,0x4
  4229e7:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  4229eb:	46                   	inc    esi
  4229ec:	4b                   	dec    ebx
  4229ed:	75 df                	jne    0x4229ce
  4229ef:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4229f3:	5f                   	pop    edi
  4229f4:	5e                   	pop    esi
  4229f5:	5d                   	pop    ebp
  4229f6:	5b                   	pop    ebx
  4229f7:	59                   	pop    ecx
  4229f8:	c2 14 00             	ret    0x14
  4229fb:	33 c9                	xor    ecx,ecx
  4229fd:	8b d5                	mov    edx,ebp
  4229ff:	b8 94 fd 89 00       	mov    eax,0x89fd94
  422a04:	8b 30                	mov    esi,DWORD PTR [eax]
  422a06:	8b be 2c 07 00 00    	mov    edi,DWORD PTR [esi+0x72c]
  422a0c:	85 ff                	test   edi,edi
  422a0e:	7c 0e                	jl     0x422a1e
  422a10:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  422a14:	89 0a                	mov    DWORD PTR [edx],ecx
  422a16:	47                   	inc    edi
  422a17:	83 c2 04             	add    edx,0x4
  422a1a:	89 7c 24 18          	mov    DWORD PTR [esp+0x18],edi
  422a1e:	83 c0 04             	add    eax,0x4
  422a21:	41                   	inc    ecx
  422a22:	3d d4 fd 89 00       	cmp    eax,0x89fdd4
  422a27:	7c db                	jl     0x422a04
  422a29:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  422a2d:	5f                   	pop    edi
  422a2e:	5e                   	pop    esi
  422a2f:	5d                   	pop    ebp
  422a30:	5b                   	pop    ebx
  422a31:	59                   	pop    ecx
  422a32:	c2 14 00             	ret    0x14
  422a35:	5e                   	pop    esi
  422a36:	5d                   	pop    ebp
  422a37:	33 c0                	xor    eax,eax
  422a39:	5b                   	pop    ebx
  422a3a:	59                   	pop    ecx
  422a3b:	c2 14 00             	ret    0x14
  422a3e:	8b ff                	mov    edi,edi
  422a40:	b9 28 42 00 c0       	mov    ecx,0xc0004228
  422a45:	28 42 00             	sub    BYTE PTR [edx+0x0],al
  422a48:	c7                   	(bad)
  422a49:	28 42 00             	sub    BYTE PTR [edx+0x0],al
  422a4c:	ce                   	into
  422a4d:	28 42 00             	sub    BYTE PTR [edx+0x0],al
  422a50:	33 29                	xor    ebp,DWORD PTR [ecx]
  422a52:	42                   	inc    edx
  422a53:	00 5d 29             	add    BYTE PTR [ebp+0x29],bl
  422a56:	42                   	inc    edx
  422a57:	00 29                	add    BYTE PTR [ecx],ch
  422a59:	2a 42 00             	sub    al,BYTE PTR [edx+0x0]
  422a5c:	93                   	xchg   ebx,eax
  422a5d:	29 42 00             	sub    DWORD PTR [edx+0x0],eax
  422a60:	c7                   	(bad)
  422a61:	29 42 00             	sub    DWORD PTR [edx+0x0],eax
  422a64:	fb                   	sti
  422a65:	29 42 00             	sub    DWORD PTR [edx+0x0],eax
  422a68:	90                   	nop
  422a69:	90                   	nop
  422a6a:	90                   	nop
  422a6b:	90                   	nop
  422a6c:	90                   	nop
  422a6d:	90                   	nop
  422a6e:	90                   	nop
  422a6f:	90                   	nop
  422a70:	56                   	push   esi
  422a71:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  422a75:	57                   	push   edi
  422a76:	33 c0                	xor    eax,eax
  422a78:	b9 94 fd 89 00       	mov    ecx,0x89fd94
  422a7d:	8b 11                	mov    edx,DWORD PTR [ecx]
  422a7f:	39 b2 9c 0b 00 00    	cmp    DWORD PTR [edx+0xb9c],esi
  422a85:	75 0a                	jne    0x422a91
  422a87:	8b ba 2c 07 00 00    	mov    edi,DWORD PTR [edx+0x72c]
  422a8d:	85 ff                	test   edi,edi
  422a8f:	7d 0f                	jge    0x422aa0
  422a91:	83 c1 04             	add    ecx,0x4
  422a94:	40                   	inc    eax
  422a95:	81 f9 d4 fd 89 00    	cmp    ecx,0x89fdd4
  422a9b:	7c e0                	jl     0x422a7d
  422a9d:	83 c8 ff             	or     eax,0xffffffff
  422aa0:	5f                   	pop    edi
  422aa1:	5e                   	pop    esi
  422aa2:	c2 04 00             	ret    0x4
  422aa5:	90                   	nop
  422aa6:	90                   	nop
  422aa7:	90                   	nop
  422aa8:	90                   	nop
  422aa9:	90                   	nop
  422aaa:	90                   	nop
  422aab:	90                   	nop
  422aac:	90                   	nop
  422aad:	90                   	nop
  422aae:	90                   	nop
  422aaf:	90                   	nop
