
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004239e0 <.text+0x229e0>:
  4239e0:	86 c8                	xchg   al,cl
  4239e2:	00 00                	add    BYTE PTR [eax],al
  4239e4:	00 42 83             	add    BYTE PTR [edx-0x7d],al
  4239e7:	c1 04 3b d0          	rol    DWORD PTR [ebx+edi*1],0xd0
  4239eb:	7c c0                	jl     0x4239ad
  4239ed:	89 be 24 07 00 00    	mov    DWORD PTR [esi+0x724],edi
  4239f3:	5f                   	pop    edi
  4239f4:	5e                   	pop    esi
  4239f5:	5d                   	pop    ebp
  4239f6:	5b                   	pop    ebx
  4239f7:	c3                   	ret
  4239f8:	90                   	nop
  4239f9:	90                   	nop
  4239fa:	90                   	nop
  4239fb:	90                   	nop
  4239fc:	90                   	nop
  4239fd:	90                   	nop
  4239fe:	90                   	nop
  4239ff:	90                   	nop
  423a00:	53                   	push   ebx
  423a01:	55                   	push   ebp
  423a02:	8b 6c 24 0c          	mov    ebp,DWORD PTR [esp+0xc]
  423a06:	8b d9                	mov    ebx,ecx
  423a08:	56                   	push   esi
  423a09:	57                   	push   edi
  423a0a:	8d bb fc 0c 00 00    	lea    edi,[ebx+0xcfc]
  423a10:	b9 39 00 00 00       	mov    ecx,0x39
  423a15:	8b f5                	mov    esi,ebp
  423a17:	33 c0                	xor    eax,eax
  423a19:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  423a1b:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  423a1e:	85 c9                	test   ecx,ecx
  423a20:	89 8b c8 00 00 00    	mov    DWORD PTR [ebx+0xc8],ecx
  423a26:	7e 1e                	jle    0x423a46
  423a28:	8d 93 88 00 00 00    	lea    edx,[ebx+0x88]
  423a2e:	8d 4d 0c             	lea    ecx,[ebp+0xc]
  423a31:	8b 31                	mov    esi,DWORD PTR [ecx]
  423a33:	40                   	inc    eax
  423a34:	89 32                	mov    DWORD PTR [edx],esi
  423a36:	8b b3 c8 00 00 00    	mov    esi,DWORD PTR [ebx+0xc8]
  423a3c:	83 c1 04             	add    ecx,0x4
  423a3f:	83 c2 04             	add    edx,0x4
  423a42:	3b c6                	cmp    eax,esi
  423a44:	7c eb                	jl     0x423a31
  423a46:	8b 8b f4 08 00 00    	mov    ecx,DWORD PTR [ebx+0x8f4]
  423a4c:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  423a4f:	85 c9                	test   ecx,ecx
  423a51:	89 83 f0 0c 00 00    	mov    DWORD PTR [ebx+0xcf0],eax
  423a57:	7e 05                	jle    0x423a5e
  423a59:	83 f8 03             	cmp    eax,0x3
  423a5c:	74 0c                	je     0x423a6a
  423a5e:	81 bd d8 00 00 00 ea 	cmp    DWORD PTR [ebp+0xd8],0x1ea
  423a65:	01 00 00 
  423a68:	75 2f                	jne    0x423a99
  423a6a:	8b ad d8 00 00 00    	mov    ebp,DWORD PTR [ebp+0xd8]
  423a70:	85 ed                	test   ebp,ebp
  423a72:	7e 7f                	jle    0x423af3
  423a74:	8d 04 ed 00 00 00 00 	lea    eax,[ebp*8+0x0]
  423a7b:	89 ab a8 07 00 00    	mov    DWORD PTR [ebx+0x7a8],ebp
  423a81:	8b 0d ac 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01ac
  423a87:	2b c5                	sub    eax,ebp
  423a89:	8d 04 80             	lea    eax,[eax+eax*4]
  423a8c:	c1 e0 04             	shl    eax,0x4
  423a8f:	03 c1                	add    eax,ecx
  423a91:	89 83 48 08 00 00    	mov    DWORD PTR [ebx+0x848],eax
  423a97:	eb 5a                	jmp    0x423af3
  423a99:	83 f8 07             	cmp    eax,0x7
  423a9c:	75 28                	jne    0x423ac6
  423a9e:	8b ad d4 00 00 00    	mov    ebp,DWORD PTR [ebp+0xd4]
  423aa4:	a1 b0 01 8a 00       	mov    eax,ds:0x8a01b0
  423aa9:	8d 0c ed 00 00 00 00 	lea    ecx,[ebp*8+0x0]
  423ab0:	2b cd                	sub    ecx,ebp
  423ab2:	8d 54 8d 00          	lea    edx,[ebp+ecx*4+0x0]
  423ab6:	c1 e2 04             	shl    edx,0x4
  423ab9:	2b d5                	sub    edx,ebp
  423abb:	8d 0c 50             	lea    ecx,[eax+edx*2]
  423abe:	89 8b f4 0c 00 00    	mov    DWORD PTR [ebx+0xcf4],ecx
  423ac4:	eb 2d                	jmp    0x423af3
  423ac6:	83 f8 01             	cmp    eax,0x1
  423ac9:	75 28                	jne    0x423af3
  423acb:	8b 8b 4c 08 00 00    	mov    ecx,DWORD PTR [ebx+0x84c]
  423ad1:	89 8b a8 07 00 00    	mov    DWORD PTR [ebx+0x7a8],ecx
  423ad7:	8d 04 cd 00 00 00 00 	lea    eax,[ecx*8+0x0]
  423ade:	2b c1                	sub    eax,ecx
  423ae0:	8d 14 80             	lea    edx,[eax+eax*4]
  423ae3:	a1 ac 01 8a 00       	mov    eax,ds:0x8a01ac
  423ae8:	c1 e2 04             	shl    edx,0x4
  423aeb:	03 d0                	add    edx,eax
  423aed:	89 93 48 08 00 00    	mov    DWORD PTR [ebx+0x848],edx
  423af3:	8b 8b c8 00 00 00    	mov    ecx,DWORD PTR [ebx+0xc8]
  423af9:	8d 83 88 00 00 00    	lea    eax,[ebx+0x88]
  423aff:	50                   	push   eax
  423b00:	51                   	push   ecx
  423b01:	8b cb                	mov    ecx,ebx
  423b03:	e8 a8 01 00 00       	call   0x423cb0
  423b08:	85 c0                	test   eax,eax
  423b0a:	89 83 a0 07 00 00    	mov    DWORD PTR [ebx+0x7a0],eax
  423b10:	7d 13                	jge    0x423b25
  423b12:	8b 93 c8 00 00 00    	mov    edx,DWORD PTR [ebx+0xc8]
  423b18:	8b 84 93 84 00 00 00 	mov    eax,DWORD PTR [ebx+edx*4+0x84]
  423b1f:	89 83 a0 07 00 00    	mov    DWORD PTR [ebx+0x7a0],eax
  423b25:	8b 83 f0 0c 00 00    	mov    eax,DWORD PTR [ebx+0xcf0]
  423b2b:	83 f8 04             	cmp    eax,0x4
  423b2e:	75 08                	jne    0x423b38
  423b30:	8d b3 a8 0c 00 00    	lea    esi,[ebx+0xca8]
  423b36:	eb 0b                	jmp    0x423b43
  423b38:	83 f8 07             	cmp    eax,0x7
  423b3b:	8d b3 a8 0c 00 00    	lea    esi,[ebx+0xca8]
  423b41:	75 24                	jne    0x423b67
  423b43:	6a 00                	push   0x0
  423b45:	6a 02                	push   0x2
  423b47:	6a 0c                	push   0xc
  423b49:	8b ce                	mov    ecx,esi
  423b4b:	e8 40 04 00 00       	call   0x423f90
  423b50:	68 10 27 00 00       	push   0x2710
  423b55:	6a 03                	push   0x3
  423b57:	6a 0c                	push   0xc
  423b59:	8b ce                	mov    ecx,esi
  423b5b:	e8 30 04 00 00       	call   0x423f90
  423b60:	5f                   	pop    edi
  423b61:	5e                   	pop    esi
  423b62:	5d                   	pop    ebp
  423b63:	5b                   	pop    ebx
  423b64:	c2 04 00             	ret    0x4
  423b67:	8b 8b 48 08 00 00    	mov    ecx,DWORD PTR [ebx+0x848]
  423b6d:	8b 91 8c 00 00 00    	mov    edx,DWORD PTR [ecx+0x8c]
  423b73:	8b ce                	mov    ecx,esi
  423b75:	52                   	push   edx
  423b76:	6a 02                	push   0x2
  423b78:	6a 0c                	push   0xc
  423b7a:	e8 11 04 00 00       	call   0x423f90
  423b7f:	8b 83 48 08 00 00    	mov    eax,DWORD PTR [ebx+0x848]
  423b85:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  423b8b:	51                   	push   ecx
  423b8c:	6a 03                	push   0x3
  423b8e:	6a 0c                	push   0xc
  423b90:	8b ce                	mov    ecx,esi
  423b92:	e8 f9 03 00 00       	call   0x423f90
  423b97:	5f                   	pop    edi
  423b98:	5e                   	pop    esi
  423b99:	5d                   	pop    ebp
  423b9a:	5b                   	pop    ebx
  423b9b:	c2 04 00             	ret    0x4
  423b9e:	90                   	nop
  423b9f:	90                   	nop
  423ba0:	56                   	push   esi
  423ba1:	8b f1                	mov    esi,ecx
  423ba3:	b8 07 00 00 00       	mov    eax,0x7
  423ba8:	89 86 fc 0c 00 00    	mov    DWORD PTR [esi+0xcfc],eax
  423bae:	89 86 f0 0c 00 00    	mov    DWORD PTR [esi+0xcf0],eax
  423bb4:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  423bb8:	89 86 d0 0d 00 00    	mov    DWORD PTR [esi+0xdd0],eax
  423bbe:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  423bc5:	2b c8                	sub    ecx,eax
  423bc7:	8d 14 88             	lea    edx,[eax+ecx*4]
  423bca:	c1 e2 04             	shl    edx,0x4
  423bcd:	2b d0                	sub    edx,eax
  423bcf:	a1 b0 01 8a 00       	mov    eax,ds:0x8a01b0
  423bd4:	8d 0c 50             	lea    ecx,[eax+edx*2]
  423bd7:	89 8e f4 0c 00 00    	mov    DWORD PTR [esi+0xcf4],ecx
  423bdd:	8b ce                	mov    ecx,esi
  423bdf:	e8 cc f0 ff ff       	call   0x422cb0
  423be4:	85 c0                	test   eax,eax
  423be6:	89 86 a0 07 00 00    	mov    DWORD PTR [esi+0x7a0],eax
  423bec:	7c 78                	jl     0x423c66
  423bee:	8b 8e f4 0c 00 00    	mov    ecx,DWORD PTR [esi+0xcf4]
  423bf4:	57                   	push   edi
  423bf5:	8d be 88 00 00 00    	lea    edi,[esi+0x88]
  423bfb:	8b 91 bc 00 00 00    	mov    edx,DWORD PTR [ecx+0xbc]
  423c01:	8b 89 b8 00 00 00    	mov    ecx,DWORD PTR [ecx+0xb8]
  423c07:	57                   	push   edi
  423c08:	6a 10                	push   0x10
  423c0a:	52                   	push   edx
  423c0b:	51                   	push   ecx
  423c0c:	50                   	push   eax
  423c0d:	8b ce                	mov    ecx,esi
  423c0f:	e8 3c ec ff ff       	call   0x422850
  423c14:	33 c9                	xor    ecx,ecx
  423c16:	89 86 c8 00 00 00    	mov    DWORD PTR [esi+0xc8],eax
  423c1c:	85 c0                	test   eax,eax
  423c1e:	89 86 04 0d 00 00    	mov    DWORD PTR [esi+0xd04],eax
  423c24:	5f                   	pop    edi
  423c25:	7e 1c                	jle    0x423c43
  423c27:	8d 86 08 0d 00 00    	lea    eax,[esi+0xd08]
  423c2d:	8b 90 80 f3 ff ff    	mov    edx,DWORD PTR [eax-0xc80]
  423c33:	41                   	inc    ecx
  423c34:	89 10                	mov    DWORD PTR [eax],edx
  423c36:	8b 96 c8 00 00 00    	mov    edx,DWORD PTR [esi+0xc8]
  423c3c:	83 c0 04             	add    eax,0x4
  423c3f:	3b ca                	cmp    ecx,edx
  423c41:	7c ea                	jl     0x423c2d
  423c43:	81 c6 a8 0c 00 00    	add    esi,0xca8
  423c49:	6a 00                	push   0x0
  423c4b:	6a 02                	push   0x2
  423c4d:	6a 0c                	push   0xc
  423c4f:	8b ce                	mov    ecx,esi
  423c51:	e8 3a 03 00 00       	call   0x423f90
  423c56:	68 a0 86 01 00       	push   0x186a0
  423c5b:	6a 03                	push   0x3
  423c5d:	6a 0c                	push   0xc
  423c5f:	8b ce                	mov    ecx,esi
  423c61:	e8 2a 03 00 00       	call   0x423f90
  423c66:	5e                   	pop    esi
  423c67:	c2 04 00             	ret    0x4
  423c6a:	90                   	nop
  423c6b:	90                   	nop
  423c6c:	90                   	nop
  423c6d:	90                   	nop
  423c6e:	90                   	nop
  423c6f:	90                   	nop
