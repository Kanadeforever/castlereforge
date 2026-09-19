
upload/RPG.exe:     file format pei-i386


Disassembly of section .text:

00428b60 <.text+0x27b60>:
  428b60:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  428b64:	53                   	push   ebx
  428b65:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  428b69:	55                   	push   ebp
  428b6a:	56                   	push   esi
  428b6b:	57                   	push   edi
  428b6c:	8b e9                	mov    ebp,ecx
  428b6e:	33 ff                	xor    edi,edi
  428b70:	3b df                	cmp    ebx,edi
  428b72:	89 7d 1c             	mov    DWORD PTR [ebp+0x1c],edi
  428b75:	c6 85 2d 01 00 00 00 	mov    BYTE PTR [ebp+0x12d],0x0
  428b7c:	89 7d 10             	mov    DWORD PTR [ebp+0x10],edi
  428b7f:	89 7d 14             	mov    DWORD PTR [ebp+0x14],edi
  428b82:	89 7d 5c             	mov    DWORD PTR [ebp+0x5c],edi
  428b85:	89 7d 18             	mov    DWORD PTR [ebp+0x18],edi
  428b88:	89 85 30 01 00 00    	mov    DWORD PTR [ebp+0x130],eax
  428b8e:	75 12                	jne    0x428ba2
  428b90:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  428b96:	68 30 a6 46 00       	push   0x46a630
  428b9b:	e8 d0 93 00 00       	call   0x431f70
  428ba0:	eb 32                	jmp    0x428bd4
  428ba2:	8b fb                	mov    edi,ebx
  428ba4:	83 c9 ff             	or     ecx,0xffffffff
  428ba7:	33 c0                	xor    eax,eax
  428ba9:	8d 55 64             	lea    edx,[ebp+0x64]
  428bac:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  428bae:	f7 d1                	not    ecx
  428bb0:	2b f9                	sub    edi,ecx
  428bb2:	68 2c a6 46 00       	push   0x46a62c
  428bb7:	8b c1                	mov    eax,ecx
  428bb9:	8b f7                	mov    esi,edi
  428bbb:	8b fa                	mov    edi,edx
  428bbd:	53                   	push   ebx
  428bbe:	c1 e9 02             	shr    ecx,0x2
  428bc1:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  428bc3:	8b c8                	mov    ecx,eax
  428bc5:	83 e1 03             	and    ecx,0x3
  428bc8:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  428bca:	e8 b1 99 02 00       	call   0x452580
  428bcf:	83 c4 08             	add    esp,0x8
  428bd2:	33 ff                	xor    edi,edi
  428bd4:	8b 0d 34 3d 8c 00    	mov    ecx,DWORD PTR ds:0x8c3d34
  428bda:	8d 45 5c             	lea    eax,[ebp+0x5c]
  428bdd:	50                   	push   eax
  428bde:	53                   	push   ebx
  428bdf:	e8 cc a2 00 00       	call   0x432eb0
  428be4:	8b 0d 34 3d 8c 00    	mov    ecx,DWORD PTR ds:0x8c3d34
  428bea:	8b f0                	mov    esi,eax
  428bec:	3b f7                	cmp    esi,edi
  428bee:	8b 11                	mov    edx,DWORD PTR [ecx]
  428bf0:	89 55 60             	mov    DWORD PTR [ebp+0x60],edx
  428bf3:	75 2c                	jne    0x428c21
  428bf5:	68 88 9f 46 00       	push   0x469f88
  428bfa:	53                   	push   ebx
  428bfb:	e8 30 41 03 00       	call   0x45cd30
  428c00:	83 c4 08             	add    esp,0x8
  428c03:	85 c0                	test   eax,eax
  428c05:	74 6b                	je     0x428c72
  428c07:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  428c0d:	53                   	push   ebx
  428c0e:	68 0c a6 46 00       	push   0x46a60c
  428c13:	e8 38 94 00 00       	call   0x432050
  428c18:	5f                   	pop    edi
  428c19:	8b c5                	mov    eax,ebp
  428c1b:	5e                   	pop    esi
  428c1c:	5d                   	pop    ebp
  428c1d:	5b                   	pop    ebx
  428c1e:	c2 08 00             	ret    0x8
  428c21:	6a 03                	push   0x3
  428c23:	68 08 a6 46 00       	push   0x46a608
  428c28:	56                   	push   esi
  428c29:	e8 92 41 03 00       	call   0x45cdc0
  428c2e:	83 c4 0c             	add    esp,0xc
  428c31:	85 c0                	test   eax,eax
  428c33:	75 22                	jne    0x428c57
  428c35:	56                   	push   esi
  428c36:	8b cd                	mov    ecx,ebp
  428c38:	c6 85 2d 01 00 00 01 	mov    BYTE PTR [ebp+0x12d],0x1
  428c3f:	e8 6c 00 00 00       	call   0x428cb0
  428c44:	89 7d 54             	mov    DWORD PTR [ebp+0x54],edi
  428c47:	5f                   	pop    edi
  428c48:	c6 85 2c 01 00 00 00 	mov    BYTE PTR [ebp+0x12c],0x0
  428c4f:	8b c5                	mov    eax,ebp
  428c51:	5e                   	pop    esi
  428c52:	5d                   	pop    ebp
  428c53:	5b                   	pop    ebx
  428c54:	c2 08 00             	ret    0x8
  428c57:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  428c5d:	53                   	push   ebx
  428c5e:	68 e8 a5 46 00       	push   0x46a5e8
  428c63:	e8 e8 93 00 00       	call   0x432050
  428c68:	89 7d 54             	mov    DWORD PTR [ebp+0x54],edi
  428c6b:	c6 85 2c 01 00 00 00 	mov    BYTE PTR [ebp+0x12c],0x0
  428c72:	5f                   	pop    edi
  428c73:	8b c5                	mov    eax,ebp
  428c75:	5e                   	pop    esi
  428c76:	5d                   	pop    ebp
  428c77:	5b                   	pop    ebx
  428c78:	c2 08 00             	ret    0x8
  428c7b:	90                   	nop
  428c7c:	90                   	nop
  428c7d:	90                   	nop
  428c7e:	90                   	nop
  428c7f:	90                   	nop
  428c80:	56                   	push   esi
  428c81:	8b f1                	mov    esi,ecx
  428c83:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  428c86:	85 c0                	test   eax,eax
  428c88:	74 13                	je     0x428c9d
  428c8a:	8b 0d 34 3d 8c 00    	mov    ecx,DWORD PTR ds:0x8c3d34
  428c90:	50                   	push   eax
  428c91:	e8 3a a2 00 00       	call   0x432ed0
  428c96:	c7 46 1c 00 00 00 00 	mov    DWORD PTR [esi+0x1c],0x0
  428c9d:	c6 86 2d 01 00 00 00 	mov    BYTE PTR [esi+0x12d],0x0
  428ca4:	5e                   	pop    esi
  428ca5:	c3                   	ret
  428ca6:	90                   	nop
  428ca7:	90                   	nop
  428ca8:	90                   	nop
  428ca9:	90                   	nop
  428caa:	90                   	nop
  428cab:	90                   	nop
  428cac:	90                   	nop
  428cad:	90                   	nop
  428cae:	90                   	nop
  428caf:	90                   	nop
  428cb0:	51                   	push   ecx
  428cb1:	53                   	push   ebx
  428cb2:	55                   	push   ebp
  428cb3:	8b 6c 24 10          	mov    ebp,DWORD PTR [esp+0x10]
  428cb7:	56                   	push   esi
  428cb8:	85 ed                	test   ebp,ebp
  428cba:	8b d9                	mov    ebx,ecx
  428cbc:	75 0a                	jne    0x428cc8
  428cbe:	89 6b 1c             	mov    DWORD PTR [ebx+0x1c],ebp
  428cc1:	5e                   	pop    esi
  428cc2:	5d                   	pop    ebp
  428cc3:	5b                   	pop    ebx
  428cc4:	59                   	pop    ecx
  428cc5:	c2 04 00             	ret    0x4
  428cc8:	89 6b 20             	mov    DWORD PTR [ebx+0x20],ebp
  428ccb:	8a 45 0b             	mov    al,BYTE PTR [ebp+0xb]
  428cce:	3c 6f                	cmp    al,0x6f
  428cd0:	75 7b                	jne    0x428d4d
  428cd2:	8b 85 bc 42 00 00    	mov    eax,DWORD PTR [ebp+0x42bc]
  428cd8:	8b 8d c0 42 00 00    	mov    ecx,DWORD PTR [ebp+0x42c0]
  428cde:	3b c1                	cmp    eax,ecx
  428ce0:	0f 85 1b 01 00 00    	jne    0x428e01
  428ce6:	57                   	push   edi
  428ce7:	50                   	push   eax
  428ce8:	e8 83 89 01 00       	call   0x441670
  428ced:	b9 af 10 00 00       	mov    ecx,0x10af
  428cf2:	8b f5                	mov    esi,ebp
  428cf4:	8b f8                	mov    edi,eax
  428cf6:	81 c5 c4 42 00 00    	add    ebp,0x42c4
  428cfc:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  428cfe:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
  428d01:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  428d05:	81 e9 bc 42 00 00    	sub    ecx,0x42bc
  428d0b:	8d 54 24 14          	lea    edx,[esp+0x14]
  428d0f:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  428d13:	8b 4b 5c             	mov    ecx,DWORD PTR [ebx+0x5c]
  428d16:	81 e9 c4 42 00 00    	sub    ecx,0x42c4
  428d1c:	05 bc 42 00 00       	add    eax,0x42bc
  428d21:	51                   	push   ecx
  428d22:	55                   	push   ebp
  428d23:	52                   	push   edx
  428d24:	50                   	push   eax
  428d25:	e8 a6 5d 02 00       	call   0x44ead0
  428d2a:	83 c4 14             	add    esp,0x14
  428d2d:	85 c0                	test   eax,eax
  428d2f:	5f                   	pop    edi
  428d30:	75 50                	jne    0x428d82
  428d32:	8b 6c 24 14          	mov    ebp,DWORD PTR [esp+0x14]
  428d36:	8b 43 60             	mov    eax,DWORD PTR [ebx+0x60]
  428d39:	8b 0d 34 3d 8c 00    	mov    ecx,DWORD PTR ds:0x8c3d34
  428d3f:	55                   	push   ebp
  428d40:	50                   	push   eax
  428d41:	e8 8a a0 00 00       	call   0x432dd0
  428d46:	89 6b 20             	mov    DWORD PTR [ebx+0x20],ebp
  428d49:	c6 45 0b 00          	mov    BYTE PTR [ebp+0xb],0x0
  428d4d:	89 6b 20             	mov    DWORD PTR [ebx+0x20],ebp
  428d50:	89 6b 1c             	mov    DWORD PTR [ebx+0x1c],ebp
  428d53:	8b 45 16             	mov    eax,DWORD PTR [ebp+0x16]
  428d56:	83 ce ff             	or     esi,0xffffffff
  428d59:	03 c5                	add    eax,ebp
  428d5b:	89 43 24             	mov    DWORD PTR [ebx+0x24],eax
  428d5e:	8b 4d 20             	mov    ecx,DWORD PTR [ebp+0x20]
  428d61:	03 cd                	add    ecx,ebp
  428d63:	89 4b 28             	mov    DWORD PTR [ebx+0x28],ecx
  428d66:	8b 55 2a             	mov    edx,DWORD PTR [ebp+0x2a]
  428d69:	03 d5                	add    edx,ebp
  428d6b:	89 53 2c             	mov    DWORD PTR [ebx+0x2c],edx
  428d6e:	8b 45 34             	mov    eax,DWORD PTR [ebp+0x34]
  428d71:	03 c5                	add    eax,ebp
  428d73:	89 43 30             	mov    DWORD PTR [ebx+0x30],eax
  428d76:	66 83 7d 14 00       	cmp    WORD PTR [ebp+0x14],0x0
  428d7b:	75 3b                	jne    0x428db8
  428d7d:	89 73 38             	mov    DWORD PTR [ebx+0x38],esi
  428d80:	eb 3f                	jmp    0x428dc1
  428d82:	8d 4b 64             	lea    ecx,[ebx+0x64]
  428d85:	51                   	push   ecx
  428d86:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  428d8c:	68 50 a6 46 00       	push   0x46a650
  428d91:	e8 ba 92 00 00       	call   0x432050
  428d96:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  428d9a:	52                   	push   edx
  428d9b:	e8 c0 88 01 00       	call   0x441660
  428da0:	83 c4 04             	add    esp,0x4
  428da3:	c6 83 2d 01 00 00 00 	mov    BYTE PTR [ebx+0x12d],0x0
  428daa:	c7 43 20 00 00 00 00 	mov    DWORD PTR [ebx+0x20],0x0
  428db1:	5e                   	pop    esi
  428db2:	5d                   	pop    ebp
  428db3:	5b                   	pop    ebx
  428db4:	59                   	pop    ecx
  428db5:	c2 04 00             	ret    0x4
  428db8:	6a 00                	push   0x0
  428dba:	8b cb                	mov    ecx,ebx
  428dbc:	e8 af 00 00 00       	call   0x428e70
  428dc1:	8b 4b 20             	mov    ecx,DWORD PTR [ebx+0x20]
  428dc4:	66 83 79 1e 00       	cmp    WORD PTR [ecx+0x1e],0x0
  428dc9:	75 05                	jne    0x428dd0
  428dcb:	89 73 48             	mov    DWORD PTR [ebx+0x48],esi
  428dce:	eb 09                	jmp    0x428dd9
  428dd0:	6a 00                	push   0x0
  428dd2:	8b cb                	mov    ecx,ebx
  428dd4:	e8 47 01 00 00       	call   0x428f20
  428dd9:	8b 53 20             	mov    edx,DWORD PTR [ebx+0x20]
  428ddc:	66 83 7a 32 00       	cmp    WORD PTR [edx+0x32],0x0
  428de1:	75 05                	jne    0x428de8
  428de3:	89 73 50             	mov    DWORD PTR [ebx+0x50],esi
  428de6:	eb 09                	jmp    0x428df1
  428de8:	6a 00                	push   0x0
  428dea:	8b cb                	mov    ecx,ebx
  428dec:	e8 af 01 00 00       	call   0x428fa0
  428df1:	83 bb 30 01 00 00 10 	cmp    DWORD PTR [ebx+0x130],0x10
  428df8:	75 07                	jne    0x428e01
  428dfa:	8b cb                	mov    ecx,ebx
  428dfc:	e8 0f 00 00 00       	call   0x428e10
  428e01:	5e                   	pop    esi
  428e02:	5d                   	pop    ebp
  428e03:	5b                   	pop    ebx
  428e04:	59                   	pop    ecx
  428e05:	c2 04 00             	ret    0x4
  428e08:	90                   	nop
  428e09:	90                   	nop
  428e0a:	90                   	nop
  428e0b:	90                   	nop
  428e0c:	90                   	nop
  428e0d:	90                   	nop
  428e0e:	90                   	nop
  428e0f:	90                   	nop
