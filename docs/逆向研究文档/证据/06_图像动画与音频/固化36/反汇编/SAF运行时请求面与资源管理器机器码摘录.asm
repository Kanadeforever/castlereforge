; 《幽城幻剑录》固化36：SAF运行时请求面与资源语料负证据——机器码摘录
; 来源：用户提供 RPG.exe.org，SHA-256 8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f
; 注意：本文件是只读反汇编证据摘录，不是可编译源码。地址均为当前EXE VA。

; ===== BaseRole 动态 FIGHT\...SF2 路径构造调用区 | 0x0041F780..0x0041F830 =====
  41f781:	8d 54 24 18          	lea    edx,[esp+0x18]
  41f785:	eb 15                	jmp    0x41f79c
  41f787:	8d bd 42 0b 00 00    	lea    edi,[ebp+0xb42]
  41f78d:	8d 54 24 18          	lea    edx,[esp+0x18]
  41f791:	eb 09                	jmp    0x41f79c
  41f793:	8d 54 24 18          	lea    edx,[esp+0x18]
  41f797:	bf 80 9f 46 00       	mov    edi,0x469f80
  41f79c:	83 c9 ff             	or     ecx,0xffffffff
  41f79f:	33 c0                	xor    eax,eax
  41f7a1:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  41f7a3:	f7 d1                	not    ecx
  41f7a5:	2b f9                	sub    edi,ecx
  41f7a7:	8b c1                	mov    eax,ecx
  41f7a9:	8b f7                	mov    esi,edi
  41f7ab:	8b fa                	mov    edi,edx
  41f7ad:	c1 e9 02             	shr    ecx,0x2
  41f7b0:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  41f7b2:	8b c8                	mov    ecx,eax
  41f7b4:	83 e1 03             	and    ecx,0x3
  41f7b7:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  41f7b9:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  41f7bd:	51                   	push   ecx
  41f7be:	8b cd                	mov    ecx,ebp
  41f7c0:	e8 6b 01 00 00       	call   0x41f930
  41f7c5:	84 c0                	test   al,al
  41f7c7:	74 0f                	je     0x41f7d8
  41f7c9:	5f                   	pop    edi
  41f7ca:	5e                   	pop    esi
  41f7cb:	5d                   	pop    ebp
  41f7cc:	b8 80 9f 46 00       	mov    eax,0x469f80
  41f7d1:	5b                   	pop    ebx
  41f7d2:	83 c4 18             	add    esp,0x18
  41f7d5:	c2 08 00             	ret    0x8
  41f7d8:	8d 54 24 18          	lea    edx,[esp+0x18]
  41f7dc:	6a 03                	push   0x3
  41f7de:	8d 44 24 14          	lea    eax,[esp+0x14]
  41f7e2:	52                   	push   edx
  41f7e3:	50                   	push   eax
  41f7e4:	e8 67 2b 03 00       	call   0x452350
  41f7e9:	8d 4c 24 24          	lea    ecx,[esp+0x24]
  41f7ed:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  41f7f1:	51                   	push   ecx
  41f7f2:	52                   	push   edx
  41f7f3:	8d b5 a8 0b 00 00    	lea    esi,[ebp+0xba8]
  41f7f9:	68 78 9f 46 00       	push   0x469f78
  41f7fe:	68 6c 9f 46 00       	push   0x469f6c
  41f803:	56                   	push   esi
  41f804:	e8 6a 22 03 00       	call   0x451a73
  41f809:	83 c4 20             	add    esp,0x20
  41f80c:	8b c6                	mov    eax,esi
  41f80e:	5f                   	pop    edi
  41f80f:	5e                   	pop    esi
  41f810:	5d                   	pop    ebp
  41f811:	5b                   	pop    ebx
  41f812:	83 c4 18             	add    esp,0x18
  41f815:	c2 08 00             	ret    0x8
  41f818:	6a 03                	push   0x3
  41f81a:	8d 44 24 14          	lea    eax,[esp+0x14]
  41f81e:	56                   	push   esi
  41f81f:	50                   	push   eax
  41f820:	e8 2b 2b 03 00       	call   0x452350
  41f825:	8d bd a8 0b 00 00    	lea    edi,[ebp+0xba8]
  41f82b:	83 c4 0c             	add    esp,0xc
  41f82e:	3b fb                	cmp    edi,ebx

; ===== Save\Save + %s%03d%s + .TSF 三位编号路径，排除为ITF生成器 | 0x00425760..0x004257C0 =====
  425760:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  425766:	6a ff                	push   0xffffffff
  425768:	68 14 e4 45 00       	push   0x45e414
  42576d:	50                   	push   eax
  42576e:	b8 e4 e1 00 00       	mov    eax,0xe1e4
  425773:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  42577a:	e8 21 ca 02 00       	call   0x4521a0
  42577f:	8b 84 24 f4 e1 00 00 	mov    eax,DWORD PTR [esp+0xe1f4]
  425786:	53                   	push   ebx
  425787:	56                   	push   esi
  425788:	68 3c a4 46 00       	push   0x46a43c
  42578d:	50                   	push   eax
  42578e:	68 30 a4 46 00       	push   0x46a430
  425793:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  425797:	68 24 a4 46 00       	push   0x46a424
  42579c:	51                   	push   ecx
  42579d:	bb 01 00 00 00       	mov    ebx,0x1
  4257a2:	e8 cc c2 02 00       	call   0x451a73
  4257a7:	6a 14                	push   0x14
  4257a9:	e8 b1 be 02 00       	call   0x45165f
  4257ae:	83 c4 18             	add    esp,0x18
  4257b1:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4257b5:	85 c0                	test   eax,eax
  4257b7:	c7 84 24 f4 e1 00 00 	mov    DWORD PTR [esp+0xe1f4],0x0
  4257be:	00 00 00 00 

; ===== 标准SF2构造器、严格SF2魔数门、资源缓存与canonical RLE链 | 0x00428B60..0x00428E70 =====
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
  428e10:	51                   	push   ecx
  428e11:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  428e14:	53                   	push   ebx
  428e15:	55                   	push   ebp
  428e16:	57                   	push   edi
  428e17:	8b 90 7c 42 00 00    	mov    edx,DWORD PTR [eax+0x427c]
  428e1d:	33 db                	xor    ebx,ebx
  428e1f:	66 8b 58 07          	mov    bx,WORD PTR [eax+0x7]
  428e23:	33 ed                	xor    ebp,ebp
  428e25:	66 8b 68 09          	mov    bp,WORD PTR [eax+0x9]
  428e29:	33 ff                	xor    edi,edi
  428e2b:	66 8b 78 28          	mov    di,WORD PTR [eax+0x28]
  428e2f:	83 fa 41             	cmp    edx,0x41
  428e32:	89 4c 24 0c          	mov    DWORD PTR [esp+0xc],ecx
  428e36:	74 2f                	je     0x428e67
  428e38:	56                   	push   esi
  428e39:	33 f6                	xor    esi,esi
  428e3b:	85 ff                	test   edi,edi
  428e3d:	7e 1a                	jle    0x428e59
  428e3f:	56                   	push   esi
  428e40:	e8 1b 01 00 00       	call   0x428f60
  428e45:	50                   	push   eax
  428e46:	55                   	push   ebp
  428e47:	53                   	push   ebx
  428e48:	e8 2e 3f 02 00       	call   0x44cd7b
  428e4d:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  428e51:	83 c4 0c             	add    esp,0xc
  428e54:	46                   	inc    esi
  428e55:	3b f7                	cmp    esi,edi
  428e57:	7c e6                	jl     0x428e3f
  428e59:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  428e5c:	5e                   	pop    esi
  428e5d:	c7 80 7c 42 00 00 41 	mov    DWORD PTR [eax+0x427c],0x41
  428e64:	00 00 00 
  428e67:	5f                   	pop    edi
  428e68:	5d                   	pop    ebp
  428e69:	5b                   	pop    ebx
  428e6a:	59                   	pop    ecx
  428e6b:	c3                   	ret
  428e6c:	90                   	nop
  428e6d:	90                   	nop
  428e6e:	90                   	nop
  428e6f:	90                   	nop

; ===== 0x432B20资源缓存内部；包含0x441740两个直接调用点 | 0x00432B20..0x00432DD0 =====
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

; ===== 0x432EB0资源缓存包装；当前只由标准SF2构造器直接调用 | 0x00432EB0..0x00432ED0 =====
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

; ===== 0x433820唯一外部caller固定传入Font24.Fnt | 0x00401580..0x004015D0 =====
  401584:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  401587:	8b 06                	mov    eax,DWORD PTR [esi]
  401589:	33 d2                	xor    edx,edx
  40158b:	8a 11                	mov    dl,BYTE PTR [ecx]
  40158d:	8b 0d b4 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6b4
  401593:	52                   	push   edx
  401594:	50                   	push   eax
  401595:	e8 a6 37 00 00       	call   0x404d40
  40159a:	8b 0d 00 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f600
  4015a0:	68 70 80 46 00       	push   0x468070
  4015a5:	6a 18                	push   0x18
  4015a7:	6a 18                	push   0x18
  4015a9:	e8 12 1c 00 00       	call   0x4031c0
  4015ae:	8b 0d c4 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c4
  4015b4:	68 70 80 46 00       	push   0x468070
  4015b9:	e8 62 22 03 00       	call   0x433820
  4015be:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4015c0:	51                   	push   ecx
  4015c1:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4015c7:	e8 a4 37 00 00       	call   0x404d70
  4015cc:	8b 0d f4 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e24f4

; ===== 0x433820整文件读取使用者 | 0x00433820..0x00433910 =====
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

; ===== 0x441740整文件读取到新缓冲区包装 | 0x00441740..0x004417C0 =====
  441740:	53                   	push   ebx
  441741:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  441745:	56                   	push   esi
  441746:	57                   	push   edi
  441747:	85 db                	test   ebx,ebx
  441749:	8b f1                	mov    esi,ecx
  44174b:	75 08                	jne    0x441755
  44174d:	5f                   	pop    edi
  44174e:	5e                   	pop    esi
  44174f:	33 c0                	xor    eax,eax
  441751:	5b                   	pop    ebx
  441752:	c2 04 00             	ret    0x4
  441755:	6a 00                	push   0x0
  441757:	6a 01                	push   0x1
  441759:	53                   	push   ebx
  44175a:	8b ce                	mov    ecx,esi
  44175c:	e8 5f 00 00 00       	call   0x4417c0
  441761:	84 c0                	test   al,al
  441763:	75 08                	jne    0x44176d
  441765:	5f                   	pop    edi
  441766:	5e                   	pop    esi
  441767:	33 c0                	xor    eax,eax
  441769:	5b                   	pop    ebx
  44176a:	c2 04 00             	ret    0x4
  44176d:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  441770:	50                   	push   eax
  441771:	e8 fa fe ff ff       	call   0x441670
  441776:	8b f8                	mov    edi,eax
  441778:	83 c4 04             	add    esp,0x4
  44177b:	85 ff                	test   edi,edi
  44177d:	75 24                	jne    0x4417a3
  44177f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  441785:	68 7c c1 46 00       	push   0x46c17c
  44178a:	e8 e1 07 ff ff       	call   0x431f70
  44178f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  441795:	53                   	push   ebx
  441796:	e8 d5 07 ff ff       	call   0x431f70
  44179b:	5f                   	pop    edi
  44179c:	5e                   	pop    esi
  44179d:	33 c0                	xor    eax,eax
  44179f:	5b                   	pop    ebx
  4417a0:	c2 04 00             	ret    0x4
  4417a3:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  4417a6:	57                   	push   edi
  4417a7:	51                   	push   ecx
  4417a8:	6a 00                	push   0x0
  4417aa:	8b ce                	mov    ecx,esi
  4417ac:	e8 7f 02 00 00       	call   0x441a30
  4417b1:	8b ce                	mov    ecx,esi
  4417b3:	e8 48 02 00 00       	call   0x441a00
  4417b8:	8b c7                	mov    eax,edi
  4417ba:	5f                   	pop    edi
  4417bb:	5e                   	pop    esi
  4417bc:	5b                   	pop    ebx
  4417bd:	c2 04 00             	ret    0x4

; ===== 通用文件对象与挂载容器包装调用区 | 0x004417C0..0x00441A90 =====
  4417c0:	81 ec 2c 01 00 00    	sub    esp,0x12c
  4417c6:	55                   	push   ebp
  4417c7:	56                   	push   esi
  4417c8:	57                   	push   edi
  4417c9:	8b bc 24 3c 01 00 00 	mov    edi,DWORD PTR [esp+0x13c]
  4417d0:	8b e9                	mov    ebp,ecx
  4417d2:	85 ff                	test   edi,edi
  4417d4:	c6 45 09 00          	mov    BYTE PTR [ebp+0x9],0x0
  4417d8:	75 0e                	jne    0x4417e8
  4417da:	5f                   	pop    edi
  4417db:	5e                   	pop    esi
  4417dc:	32 c0                	xor    al,al
  4417de:	5d                   	pop    ebp
  4417df:	81 c4 2c 01 00 00    	add    esp,0x12c
  4417e5:	c2 0c 00             	ret    0xc
  4417e8:	8a 45 10             	mov    al,BYTE PTR [ebp+0x10]
  4417eb:	84 c0                	test   al,al
  4417ed:	74 50                	je     0x44183f
  4417ef:	8d 44 24 0c          	lea    eax,[esp+0xc]
  4417f3:	b9 90 1c 8e 00       	mov    ecx,0x8e1c90
  4417f8:	50                   	push   eax
  4417f9:	57                   	push   edi
  4417fa:	e8 f1 03 00 00       	call   0x441bf0
  4417ff:	84 c0                	test   al,al
  441801:	75 5d                	jne    0x441860
  441803:	80 bc 24 40 01 00 00 	cmp    BYTE PTR [esp+0x140],0x1
  44180a:	01 
  44180b:	75 53                	jne    0x441860
  44180d:	8b 0d 24 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2424
  441813:	85 c9                	test   ecx,ecx
  441815:	75 0e                	jne    0x441825
  441817:	5f                   	pop    edi
  441818:	5e                   	pop    esi
  441819:	32 c0                	xor    al,al
  44181b:	5d                   	pop    ebp
  44181c:	81 c4 2c 01 00 00    	add    esp,0x12c
  441822:	c2 0c 00             	ret    0xc
  441825:	8d 54 24 0c          	lea    edx,[esp+0xc]
  441829:	52                   	push   edx
  44182a:	e8 b1 87 00 00       	call   0x449fe0
  44182f:	84 c0                	test   al,al
  441831:	75 2d                	jne    0x441860
  441833:	5f                   	pop    edi
  441834:	5e                   	pop    esi
  441835:	5d                   	pop    ebp
  441836:	81 c4 2c 01 00 00    	add    esp,0x12c
  44183c:	c2 0c 00             	ret    0xc
  44183f:	83 c9 ff             	or     ecx,0xffffffff
  441842:	33 c0                	xor    eax,eax
  441844:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  441846:	f7 d1                	not    ecx
  441848:	2b f9                	sub    edi,ecx
  44184a:	8d 54 24 0c          	lea    edx,[esp+0xc]
  44184e:	8b c1                	mov    eax,ecx
  441850:	8b f7                	mov    esi,edi
  441852:	8b fa                	mov    edi,edx
  441854:	c1 e9 02             	shr    ecx,0x2
  441857:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  441859:	8b c8                	mov    ecx,eax
  44185b:	83 e1 03             	and    ecx,0x3
  44185e:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  441860:	8b cd                	mov    ecx,ebp
  441862:	e8 99 01 00 00       	call   0x441a00
  441867:	8b b4 24 44 01 00 00 	mov    esi,DWORD PTR [esp+0x144]
  44186e:	0f be 84 24 40 01 00 	movsx  eax,BYTE PTR [esp+0x140]
  441875:	00 
  441876:	f7 de                	neg    esi
  441878:	1b f6                	sbb    esi,esi
  44187a:	81 e6 00 80 ff ff    	and    esi,0xffff8000
  441880:	81 c6 00 80 00 00    	add    esi,0x8000
  441886:	48                   	dec    eax
  441887:	74 74                	je     0x4418fd
  441889:	48                   	dec    eax
  44188a:	74 41                	je     0x4418cd
  44188c:	48                   	dec    eax
  44188d:	74 16                	je     0x4418a5
  44188f:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  441893:	56                   	push   esi
  441894:	51                   	push   ecx
  441895:	e8 c6 92 01 00       	call   0x45ab60
  44189a:	83 c4 08             	add    esp,0x8
  44189d:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  4418a0:	e9 19 01 00 00       	jmp    0x4419be
  4418a5:	8d 54 24 0c          	lea    edx,[esp+0xc]
  4418a9:	68 80 01 00 00       	push   0x180
  4418ae:	52                   	push   edx
  4418af:	e8 fd 10 01 00       	call   0x4529b1
  4418b4:	83 ce 01             	or     esi,0x1
  4418b7:	8d 44 24 14          	lea    eax,[esp+0x14]
  4418bb:	56                   	push   esi
  4418bc:	50                   	push   eax
  4418bd:	e8 9e 92 01 00       	call   0x45ab60
  4418c2:	83 c4 10             	add    esp,0x10
  4418c5:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  4418c8:	e9 f1 00 00 00       	jmp    0x4419be
  4418cd:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  4418d1:	68 80 01 00 00       	push   0x180
  4418d6:	51                   	push   ecx
  4418d7:	e8 d5 10 01 00       	call   0x4529b1
  4418dc:	81 ce 01 01 00 00    	or     esi,0x101
  4418e2:	68 80 00 00 00       	push   0x80
  4418e7:	8d 54 24 18          	lea    edx,[esp+0x18]
  4418eb:	56                   	push   esi
  4418ec:	52                   	push   edx
  4418ed:	e8 6e 92 01 00       	call   0x45ab60
  4418f2:	83 c4 14             	add    esp,0x14
  4418f5:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  4418f8:	e9 c1 00 00 00       	jmp    0x4419be
  4418fd:	8d 44 24 0c          	lea    eax,[esp+0xc]
  441901:	56                   	push   esi
  441902:	50                   	push   eax
  441903:	e8 58 92 01 00       	call   0x45ab60
  441908:	83 c4 08             	add    esp,0x8
  44190b:	83 f8 ff             	cmp    eax,0xffffffff
  44190e:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  441911:	0f 85 a7 00 00 00    	jne    0x4419be
  441917:	8b 0d 24 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2424
  44191d:	85 c9                	test   ecx,ecx
  44191f:	0f 84 99 00 00 00    	je     0x4419be
  441925:	8d 54 24 0c          	lea    edx,[esp+0xc]
  441929:	52                   	push   edx
  44192a:	e8 b1 86 00 00       	call   0x449fe0
  44192f:	84 c0                	test   al,al
  441931:	0f 84 87 00 00 00    	je     0x4419be
  441937:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  44193a:	85 c0                	test   eax,eax
  44193c:	74 10                	je     0x44194e
  44193e:	50                   	push   eax
  44193f:	e8 0c fc 00 00       	call   0x451550
  441944:	83 c4 04             	add    esp,0x4
  441947:	c7 45 0c 00 00 00 00 	mov    DWORD PTR [ebp+0xc],0x0
  44194e:	8d 7c 24 0c          	lea    edi,[esp+0xc]
  441952:	83 c9 ff             	or     ecx,0xffffffff
  441955:	33 c0                	xor    eax,eax
  441957:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  441959:	f7 d1                	not    ecx
  44195b:	83 c1 02             	add    ecx,0x2
  44195e:	51                   	push   ecx
  44195f:	e8 fb fc 00 00       	call   0x45165f
  441964:	8b d0                	mov    edx,eax
  441966:	8d 7c 24 10          	lea    edi,[esp+0x10]
  44196a:	83 c9 ff             	or     ecx,0xffffffff
  44196d:	33 c0                	xor    eax,eax
  44196f:	83 c4 04             	add    esp,0x4
  441972:	89 55 0c             	mov    DWORD PTR [ebp+0xc],edx
  441975:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  441977:	f7 d1                	not    ecx
  441979:	2b f9                	sub    edi,ecx
  44197b:	8b c1                	mov    eax,ecx
  44197d:	8b f7                	mov    esi,edi
  44197f:	8b fa                	mov    edi,edx
  441981:	c1 e9 02             	shr    ecx,0x2
  441984:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  441986:	8b c8                	mov    ecx,eax
  441988:	33 c0                	xor    eax,eax
  44198a:	83 e1 03             	and    ecx,0x3
  44198d:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  44198f:	8d 7c 24 0c          	lea    edi,[esp+0xc]
  441993:	83 c9 ff             	or     ecx,0xffffffff
  441996:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  441998:	8b 55 0c             	mov    edx,DWORD PTR [ebp+0xc]
  44199b:	f7 d1                	not    ecx
  44199d:	49                   	dec    ecx
  44199e:	88 04 11             	mov    BYTE PTR [ecx+edx*1],al
  4419a1:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  4419a4:	c7 45 00 01 00 00 00 	mov    DWORD PTR [ebp+0x0],0x1
  4419ab:	c6 45 09 01          	mov    BYTE PTR [ebp+0x9],0x1
  4419af:	8b 0d 24 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2424
  4419b5:	50                   	push   eax
  4419b6:	e8 65 86 00 00       	call   0x44a020
  4419bb:	89 45 04             	mov    DWORD PTR [ebp+0x4],eax
  4419be:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  4419c1:	83 f8 ff             	cmp    eax,0xffffffff
  4419c4:	75 0e                	jne    0x4419d4
  4419c6:	5f                   	pop    edi
  4419c7:	5e                   	pop    esi
  4419c8:	32 c0                	xor    al,al
  4419ca:	5d                   	pop    ebp
  4419cb:	81 c4 2c 01 00 00    	add    esp,0x12c
  4419d1:	c2 0c 00             	ret    0xc
  4419d4:	8a 4d 09             	mov    cl,BYTE PTR [ebp+0x9]
  4419d7:	84 c9                	test   cl,cl
  4419d9:	75 0c                	jne    0x4419e7
  4419db:	50                   	push   eax
  4419dc:	e8 85 b2 01 00       	call   0x45cc66
  4419e1:	83 c4 04             	add    esp,0x4
  4419e4:	89 45 04             	mov    DWORD PTR [ebp+0x4],eax
  4419e7:	8a 8c 24 40 01 00 00 	mov    cl,BYTE PTR [esp+0x140]
  4419ee:	5f                   	pop    edi
  4419ef:	88 4d 08             	mov    BYTE PTR [ebp+0x8],cl
  4419f2:	5e                   	pop    esi
  4419f3:	b0 01                	mov    al,0x1
  4419f5:	5d                   	pop    ebp
  4419f6:	81 c4 2c 01 00 00    	add    esp,0x12c
  4419fc:	c2 0c 00             	ret    0xc
  4419ff:	90                   	nop
  441a00:	56                   	push   esi
  441a01:	8b f1                	mov    esi,ecx
  441a03:	8b 06                	mov    eax,DWORD PTR [esi]
  441a05:	83 f8 ff             	cmp    eax,0xffffffff
  441a08:	74 1f                	je     0x441a29
  441a0a:	8a 4e 09             	mov    cl,BYTE PTR [esi+0x9]
  441a0d:	84 c9                	test   cl,cl
  441a0f:	75 18                	jne    0x441a29
  441a11:	83 f8 01             	cmp    eax,0x1
  441a14:	74 09                	je     0x441a1f
  441a16:	50                   	push   eax
  441a17:	e8 88 56 01 00       	call   0x4570a4
  441a1c:	83 c4 04             	add    esp,0x4
  441a1f:	c7 06 ff ff ff ff    	mov    DWORD PTR [esi],0xffffffff
  441a25:	c6 46 08 00          	mov    BYTE PTR [esi+0x8],0x0
  441a29:	c6 46 09 00          	mov    BYTE PTR [esi+0x9],0x0
  441a2d:	5e                   	pop    esi
  441a2e:	c3                   	ret
  441a2f:	90                   	nop
  441a30:	53                   	push   ebx
  441a31:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  441a35:	56                   	push   esi
  441a36:	8b f1                	mov    esi,ecx
  441a38:	85 db                	test   ebx,ebx
  441a3a:	74 66                	je     0x441aa2
  441a3c:	80 7e 08 01          	cmp    BYTE PTR [esi+0x8],0x1
  441a40:	75 60                	jne    0x441aa2
  441a42:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  441a46:	85 c0                	test   eax,eax
  441a48:	7c 58                	jl     0x441aa2
  441a4a:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  441a4d:	3b c1                	cmp    eax,ecx
  441a4f:	7f 51                	jg     0x441aa2
  441a51:	57                   	push   edi
  441a52:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  441a56:	8d 14 38             	lea    edx,[eax+edi*1]
  441a59:	3b d1                	cmp    edx,ecx
  441a5b:	7e 04                	jle    0x441a61
  441a5d:	2b c8                	sub    ecx,eax
  441a5f:	8b f9                	mov    edi,ecx
  441a61:	8a 4e 09             	mov    cl,BYTE PTR [esi+0x9]
  441a64:	84 c9                	test   cl,cl
  441a66:	74 1a                	je     0x441a82
  441a68:	8b 0d 24 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2424
  441a6e:	57                   	push   edi
  441a6f:	50                   	push   eax
  441a70:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  441a73:	53                   	push   ebx
  441a74:	50                   	push   eax
  441a75:	e8 f6 85 00 00       	call   0x44a070
  441a7a:	5f                   	pop    edi
  441a7b:	5e                   	pop    esi
  441a7c:	b0 01                	mov    al,0x1
  441a7e:	5b                   	pop    ebx
  441a7f:	c2 0c 00             	ret    0xc
  441a82:	8b 0e                	mov    ecx,DWORD PTR [esi]
  441a84:	6a 00                	push   0x0
  441a86:	50                   	push   eax
  441a87:	51                   	push   ecx
  441a88:	e8 3f 6e 01 00       	call   0x4588cc
  441a8d:	8b 16                	mov    edx,DWORD PTR [esi]
  441a8f:	57                   	push   edi

; ===== 资源容器按名称查找、按已解析索引读取及三个外层包装 | 0x00449B40..0x0044A0C0 =====
  449b40:	8b f8                	mov    edi,eax
  449b42:	7e b4                	jle    0x449af8
  449b44:	03 ef                	add    ebp,edi
  449b46:	eb b2                	jmp    0x449afa
  449b48:	90                   	nop
  449b49:	90                   	nop
  449b4a:	90                   	nop
  449b4b:	90                   	nop
  449b4c:	90                   	nop
  449b4d:	90                   	nop
  449b4e:	90                   	nop
  449b4f:	90                   	nop
  449b50:	8a 41 08             	mov    al,BYTE PTR [ecx+0x8]
  449b53:	84 c0                	test   al,al
  449b55:	75 06                	jne    0x449b5d
  449b57:	83 c8 ff             	or     eax,0xffffffff
  449b5a:	c2 04 00             	ret    0x4
  449b5d:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  449b61:	50                   	push   eax
  449b62:	e8 19 fe ff ff       	call   0x449980
  449b67:	c2 04 00             	ret    0x4
  449b6a:	90                   	nop
  449b6b:	90                   	nop
  449b6c:	90                   	nop
  449b6d:	90                   	nop
  449b6e:	90                   	nop
  449b6f:	90                   	nop
  449b70:	8a 51 08             	mov    dl,BYTE PTR [ecx+0x8]
  449b73:	33 c0                	xor    eax,eax
  449b75:	84 d2                	test   dl,dl
  449b77:	74 20                	je     0x449b99
  449b79:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  449b7d:	85 d2                	test   edx,edx
  449b7f:	7c 18                	jl     0x449b99
  449b81:	56                   	push   esi
  449b82:	8b 71 10             	mov    esi,DWORD PTR [ecx+0x10]
  449b85:	3b d6                	cmp    edx,esi
  449b87:	5e                   	pop    esi
  449b88:	7d 0f                	jge    0x449b99
  449b8a:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  449b8d:	8d 04 92             	lea    eax,[edx+edx*4]
  449b90:	c1 e0 03             	shl    eax,0x3
  449b93:	2b c2                	sub    eax,edx
  449b95:	8b 44 08 02          	mov    eax,DWORD PTR [eax+ecx*1+0x2]
  449b99:	c2 04 00             	ret    0x4
  449b9c:	90                   	nop
  449b9d:	90                   	nop
  449b9e:	90                   	nop
  449b9f:	90                   	nop
  449ba0:	8a 41 08             	mov    al,BYTE PTR [ecx+0x8]
  449ba3:	53                   	push   ebx
  449ba4:	55                   	push   ebp
  449ba5:	56                   	push   esi
  449ba6:	84 c0                	test   al,al
  449ba8:	57                   	push   edi
  449ba9:	74 76                	je     0x449c21
  449bab:	8b 6c 24 18          	mov    ebp,DWORD PTR [esp+0x18]
  449baf:	85 ed                	test   ebp,ebp
  449bb1:	74 6e                	je     0x449c21
  449bb3:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  449bb7:	85 c0                	test   eax,eax
  449bb9:	7c 66                	jl     0x449c21
  449bbb:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  449bbf:	85 c0                	test   eax,eax
  449bc1:	7c 5e                	jl     0x449c21
  449bc3:	3b 41 10             	cmp    eax,DWORD PTR [ecx+0x10]
  449bc6:	7d 59                	jge    0x449c21
  449bc8:	8d 14 80             	lea    edx,[eax+eax*4]
  449bcb:	6a 00                	push   0x0
  449bcd:	c1 e2 03             	shl    edx,0x3
  449bd0:	2b d0                	sub    edx,eax
  449bd2:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  449bd5:	8b 49 0c             	mov    ecx,DWORD PTR [ecx+0xc]
  449bd8:	03 c2                	add    eax,edx
  449bda:	51                   	push   ecx
  449bdb:	8b 78 02             	mov    edi,DWORD PTR [eax+0x2]
  449bde:	8b 58 23             	mov    ebx,DWORD PTR [eax+0x23]
  449be1:	e8 7a 0f 01 00       	call   0x45ab60
  449be6:	8b f0                	mov    esi,eax
  449be8:	83 c4 08             	add    esp,0x8
  449beb:	83 fe ff             	cmp    esi,0xffffffff
  449bee:	74 31                	je     0x449c21
  449bf0:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  449bf4:	85 c0                	test   eax,eax
  449bf6:	7e 06                	jle    0x449bfe
  449bf8:	3b c7                	cmp    eax,edi
  449bfa:	7d 02                	jge    0x449bfe
  449bfc:	8b f8                	mov    edi,eax
  449bfe:	6a 00                	push   0x0
  449c00:	53                   	push   ebx
  449c01:	56                   	push   esi
  449c02:	e8 c5 ec 00 00       	call   0x4588cc
  449c07:	57                   	push   edi
  449c08:	55                   	push   ebp
  449c09:	56                   	push   esi
  449c0a:	e8 c1 23 01 00       	call   0x45bfd0
  449c0f:	56                   	push   esi
  449c10:	e8 8f d4 00 00       	call   0x4570a4
  449c15:	83 c4 1c             	add    esp,0x1c
  449c18:	b0 01                	mov    al,0x1
  449c1a:	5f                   	pop    edi
  449c1b:	5e                   	pop    esi
  449c1c:	5d                   	pop    ebp
  449c1d:	5b                   	pop    ebx
  449c1e:	c2 10 00             	ret    0x10
  449c21:	5f                   	pop    edi
  449c22:	5e                   	pop    esi
  449c23:	5d                   	pop    ebp
  449c24:	32 c0                	xor    al,al
  449c26:	5b                   	pop    ebx
  449c27:	c2 10 00             	ret    0x10
  449c2a:	90                   	nop
  449c2b:	90                   	nop
  449c2c:	90                   	nop
  449c2d:	90                   	nop
  449c2e:	90                   	nop
  449c2f:	90                   	nop
  449c30:	8b c1                	mov    eax,ecx
  449c32:	56                   	push   esi
  449c33:	33 c9                	xor    ecx,ecx
  449c35:	33 f6                	xor    esi,esi
  449c37:	8d 90 9c 00 00 00    	lea    edx,[eax+0x9c]
  449c3d:	88 4c 30 04          	mov    BYTE PTR [eax+esi*1+0x4],cl
  449c41:	89 4a 88             	mov    DWORD PTR [edx-0x78],ecx
  449c44:	89 0a                	mov    DWORD PTR [edx],ecx
  449c46:	89 4a 78             	mov    DWORD PTR [edx+0x78],ecx
  449c49:	46                   	inc    esi
  449c4a:	83 c2 04             	add    edx,0x4
  449c4d:	83 fe 1e             	cmp    esi,0x1e
  449c50:	7c eb                	jl     0x449c3d
  449c52:	5e                   	pop    esi
  449c53:	c3                   	ret
  449c54:	90                   	nop
  449c55:	90                   	nop
  449c56:	90                   	nop
  449c57:	90                   	nop
  449c58:	90                   	nop
  449c59:	90                   	nop
  449c5a:	90                   	nop
  449c5b:	90                   	nop
  449c5c:	90                   	nop
  449c5d:	90                   	nop
  449c5e:	90                   	nop
  449c5f:	90                   	nop
  449c60:	51                   	push   ecx
  449c61:	b8 fc ff ff ff       	mov    eax,0xfffffffc
  449c66:	53                   	push   ebx
  449c67:	55                   	push   ebp
  449c68:	2b c1                	sub    eax,ecx
  449c6a:	56                   	push   esi
  449c6b:	57                   	push   edi
  449c6c:	8d 69 04             	lea    ebp,[ecx+0x4]
  449c6f:	8d b1 9c 00 00 00    	lea    esi,[ecx+0x9c]
  449c75:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  449c79:	33 db                	xor    ebx,ebx
  449c7b:	38 5d 00             	cmp    BYTE PTR [ebp+0x0],bl
  449c7e:	74 41                	je     0x449cc1
  449c80:	88 5d 00             	mov    BYTE PTR [ebp+0x0],bl
  449c83:	8b 46 88             	mov    eax,DWORD PTR [esi-0x78]
  449c86:	3b c3                	cmp    eax,ebx
  449c88:	74 0c                	je     0x449c96
  449c8a:	50                   	push   eax
  449c8b:	e8 c0 78 00 00       	call   0x451550
  449c90:	83 c4 04             	add    esp,0x4
  449c93:	89 5e 88             	mov    DWORD PTR [esi-0x78],ebx
  449c96:	8b 06                	mov    eax,DWORD PTR [esi]
  449c98:	3b c3                	cmp    eax,ebx
  449c9a:	74 0b                	je     0x449ca7
  449c9c:	50                   	push   eax
  449c9d:	e8 ae 78 00 00       	call   0x451550
  449ca2:	83 c4 04             	add    esp,0x4
  449ca5:	89 1e                	mov    DWORD PTR [esi],ebx
  449ca7:	8b 7e 78             	mov    edi,DWORD PTR [esi+0x78]
  449caa:	3b fb                	cmp    edi,ebx
  449cac:	74 13                	je     0x449cc1
  449cae:	8b cf                	mov    ecx,edi
  449cb0:	e8 6b fb ff ff       	call   0x449820
  449cb5:	57                   	push   edi
  449cb6:	e8 95 78 00 00       	call   0x451550
  449cbb:	83 c4 04             	add    esp,0x4
  449cbe:	89 5e 78             	mov    DWORD PTR [esi+0x78],ebx
  449cc1:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  449cc5:	83 c6 04             	add    esi,0x4
  449cc8:	45                   	inc    ebp
  449cc9:	03 c5                	add    eax,ebp
  449ccb:	83 f8 1e             	cmp    eax,0x1e
  449cce:	7c ab                	jl     0x449c7b
  449cd0:	5f                   	pop    edi
  449cd1:	5e                   	pop    esi
  449cd2:	5d                   	pop    ebp
  449cd3:	5b                   	pop    ebx
  449cd4:	59                   	pop    ecx
  449cd5:	c3                   	ret
  449cd6:	90                   	nop
  449cd7:	90                   	nop
  449cd8:	90                   	nop
  449cd9:	90                   	nop
  449cda:	90                   	nop
  449cdb:	90                   	nop
  449cdc:	90                   	nop
  449cdd:	90                   	nop
  449cde:	90                   	nop
  449cdf:	90                   	nop
  449ce0:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  449ce4:	57                   	push   edi
  449ce5:	8b fa                	mov    edi,edx
  449ce7:	83 c9 ff             	or     ecx,0xffffffff
  449cea:	33 c0                	xor    eax,eax
  449cec:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  449cee:	f7 d1                	not    ecx
  449cf0:	49                   	dec    ecx
  449cf1:	85 c9                	test   ecx,ecx
  449cf3:	7e 27                	jle    0x449d1c
  449cf5:	53                   	push   ebx
  449cf6:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  449cfa:	56                   	push   esi
  449cfb:	8b f3                	mov    esi,ebx
  449cfd:	2b f2                	sub    esi,edx
  449cff:	8b f9                	mov    edi,ecx
  449d01:	8a 02                	mov    al,BYTE PTR [edx]
  449d03:	3c 41                	cmp    al,0x41
  449d05:	7c 06                	jl     0x449d0d
  449d07:	3c 5a                	cmp    al,0x5a
  449d09:	7f 02                	jg     0x449d0d
  449d0b:	04 20                	add    al,0x20
  449d0d:	88 04 16             	mov    BYTE PTR [esi+edx*1],al
  449d10:	42                   	inc    edx
  449d11:	4f                   	dec    edi
  449d12:	75 ed                	jne    0x449d01
  449d14:	c6 04 19 00          	mov    BYTE PTR [ecx+ebx*1],0x0
  449d18:	5e                   	pop    esi
  449d19:	5b                   	pop    ebx
  449d1a:	5f                   	pop    edi
  449d1b:	c3                   	ret
  449d1c:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  449d20:	5f                   	pop    edi
  449d21:	c6 04 01 00          	mov    BYTE PTR [ecx+eax*1],0x0
  449d25:	c3                   	ret
  449d26:	90                   	nop
  449d27:	90                   	nop
  449d28:	90                   	nop
  449d29:	90                   	nop
  449d2a:	90                   	nop
  449d2b:	90                   	nop
  449d2c:	90                   	nop
  449d2d:	90                   	nop
  449d2e:	90                   	nop
  449d2f:	90                   	nop
  449d30:	6a ff                	push   0xffffffff
  449d32:	68 a6 fe 45 00       	push   0x45fea6
  449d37:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  449d3d:	50                   	push   eax
  449d3e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  449d45:	83 ec 70             	sub    esp,0x70
  449d48:	8b 84 24 80 00 00 00 	mov    eax,DWORD PTR [esp+0x80]
  449d4f:	53                   	push   ebx
  449d50:	32 db                	xor    bl,bl
  449d52:	89 4c 24 08          	mov    DWORD PTR [esp+0x8],ecx
  449d56:	85 c0                	test   eax,eax
  449d58:	0f 84 d4 01 00 00    	je     0x449f32
  449d5e:	8d 44 24 10          	lea    eax,[esp+0x10]
  449d62:	85 c0                	test   eax,eax
  449d64:	0f 84 c8 01 00 00    	je     0x449f32
  449d6a:	55                   	push   ebp
  449d6b:	56                   	push   esi
  449d6c:	57                   	push   edi
  449d6d:	33 ff                	xor    edi,edi
  449d6f:	89 7c 24 10          	mov    DWORD PTR [esp+0x10],edi
  449d73:	8d a9 14 01 00 00    	lea    ebp,[ecx+0x114]
  449d79:	eb 04                	jmp    0x449d7f
  449d7b:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  449d7f:	84 db                	test   bl,bl
  449d81:	0f 85 8f 01 00 00    	jne    0x449f16
  449d87:	8a 44 0f 04          	mov    al,BYTE PTR [edi+ecx*1+0x4]
  449d8b:	84 c0                	test   al,al
  449d8d:	0f 85 6a 01 00 00    	jne    0x449efd
  449d93:	6a 14                	push   0x14
  449d95:	e8 c5 78 00 00       	call   0x45165f
  449d9a:	83 c4 04             	add    esp,0x4
  449d9d:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  449da1:	85 c0                	test   eax,eax
  449da3:	c7 84 24 88 00 00 00 	mov    DWORD PTR [esp+0x88],0x0
  449daa:	00 00 00 00 
  449dae:	74 09                	je     0x449db9
  449db0:	8b c8                	mov    ecx,eax
  449db2:	e8 49 fa ff ff       	call   0x449800
  449db7:	eb 02                	jmp    0x449dbb
  449db9:	33 c0                	xor    eax,eax
  449dbb:	8b 8c 24 90 00 00 00 	mov    ecx,DWORD PTR [esp+0x90]
  449dc2:	c7 84 24 88 00 00 00 	mov    DWORD PTR [esp+0x88],0xffffffff
  449dc9:	ff ff ff ff 
  449dcd:	51                   	push   ecx
  449dce:	8b c8                	mov    ecx,eax
  449dd0:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  449dd3:	e8 98 fa ff ff       	call   0x449870
  449dd8:	84 c0                	test   al,al
  449dda:	0f 84 ff 00 00 00    	je     0x449edf
  449de0:	8b 84 24 94 00 00 00 	mov    eax,DWORD PTR [esp+0x94]
  449de7:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  449deb:	52                   	push   edx
  449dec:	50                   	push   eax
  449ded:	e8 ee fe ff ff       	call   0x449ce0
  449df2:	8d 7c 24 24          	lea    edi,[esp+0x24]
  449df6:	83 c9 ff             	or     ecx,0xffffffff
  449df9:	33 c0                	xor    eax,eax
  449dfb:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  449dfd:	f7 d1                	not    ecx
  449dff:	49                   	dec    ecx
  449e00:	8b d9                	mov    ebx,ecx
  449e02:	8d 4b 01             	lea    ecx,[ebx+0x1]
  449e05:	51                   	push   ecx
  449e06:	e8 54 78 00 00       	call   0x45165f
  449e0b:	8b d0                	mov    edx,eax
  449e0d:	8d 7c 24 28          	lea    edi,[esp+0x28]
  449e11:	83 c9 ff             	or     ecx,0xffffffff
  449e14:	33 c0                	xor    eax,eax
  449e16:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  449e18:	f7 d1                	not    ecx
  449e1a:	2b f9                	sub    edi,ecx
  449e1c:	89 95 10 ff ff ff    	mov    DWORD PTR [ebp-0xf0],edx
  449e22:	8b c1                	mov    eax,ecx
  449e24:	8b f7                	mov    esi,edi
  449e26:	8b fa                	mov    edi,edx
  449e28:	c1 e9 02             	shr    ecx,0x2
  449e2b:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  449e2d:	8b c8                	mov    ecx,eax
  449e2f:	33 c0                	xor    eax,eax
  449e31:	83 e1 03             	and    ecx,0x3
  449e34:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  449e36:	8b 8d 10 ff ff ff    	mov    ecx,DWORD PTR [ebp-0xf0]
  449e3c:	8b b4 24 9c 00 00 00 	mov    esi,DWORD PTR [esp+0x9c]
  449e43:	8b fe                	mov    edi,esi
  449e45:	c6 04 19 00          	mov    BYTE PTR [ecx+ebx*1],0x0
  449e49:	83 c9 ff             	or     ecx,0xffffffff
  449e4c:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  449e4e:	f7 d1                	not    ecx
  449e50:	49                   	dec    ecx
  449e51:	8b d9                	mov    ebx,ecx
  449e53:	8d 53 01             	lea    edx,[ebx+0x1]
  449e56:	52                   	push   edx
  449e57:	e8 03 78 00 00       	call   0x45165f
  449e5c:	8b d0                	mov    edx,eax
  449e5e:	8b fe                	mov    edi,esi
  449e60:	83 c9 ff             	or     ecx,0xffffffff
  449e63:	33 c0                	xor    eax,eax
  449e65:	89 55 88             	mov    DWORD PTR [ebp-0x78],edx
  449e68:	6a 14                	push   0x14
  449e6a:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  449e6c:	f7 d1                	not    ecx
  449e6e:	2b f9                	sub    edi,ecx
  449e70:	8b c1                	mov    eax,ecx
  449e72:	8b f7                	mov    esi,edi
  449e74:	8b fa                	mov    edi,edx
  449e76:	c1 e9 02             	shr    ecx,0x2
  449e79:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  449e7b:	8b c8                	mov    ecx,eax
  449e7d:	83 e1 03             	and    ecx,0x3
  449e80:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  449e82:	8b 4d 88             	mov    ecx,DWORD PTR [ebp-0x78]
  449e85:	c6 04 19 00          	mov    BYTE PTR [ecx+ebx*1],0x0
  449e89:	e8 d1 77 00 00       	call   0x45165f
  449e8e:	83 c4 14             	add    esp,0x14
  449e91:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  449e95:	85 c0                	test   eax,eax
  449e97:	c7 84 24 88 00 00 00 	mov    DWORD PTR [esp+0x88],0x1
  449e9e:	01 00 00 00 
  449ea2:	74 09                	je     0x449ead
  449ea4:	8b c8                	mov    ecx,eax
  449ea6:	e8 55 f9 ff ff       	call   0x449800
  449eab:	eb 02                	jmp    0x449eaf
  449ead:	33 c0                	xor    eax,eax
  449eaf:	8b 94 24 90 00 00 00 	mov    edx,DWORD PTR [esp+0x90]
  449eb6:	8b c8                	mov    ecx,eax
  449eb8:	52                   	push   edx
  449eb9:	c7 84 24 8c 00 00 00 	mov    DWORD PTR [esp+0x8c],0xffffffff
  449ec0:	ff ff ff ff 
  449ec4:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
  449ec7:	e8 a4 f9 ff ff       	call   0x449870
  449ecc:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  449ed0:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  449ed4:	b3 01                	mov    bl,0x1
  449ed6:	8b f8                	mov    edi,eax
  449ed8:	c6 44 08 04 01       	mov    BYTE PTR [eax+ecx*1+0x4],0x1
  449edd:	eb 1e                	jmp    0x449efd
  449edf:	8b 75 00             	mov    esi,DWORD PTR [ebp+0x0]
  449ee2:	85 f6                	test   esi,esi
  449ee4:	74 10                	je     0x449ef6
  449ee6:	8b ce                	mov    ecx,esi
  449ee8:	e8 33 f9 ff ff       	call   0x449820
  449eed:	56                   	push   esi
  449eee:	e8 5d 76 00 00       	call   0x451550
  449ef3:	83 c4 04             	add    esp,0x4
  449ef6:	c7 45 00 00 00 00 00 	mov    DWORD PTR [ebp+0x0],0x0
  449efd:	47                   	inc    edi
  449efe:	83 c5 04             	add    ebp,0x4
  449f01:	83 ff 1e             	cmp    edi,0x1e
  449f04:	89 7c 24 10          	mov    DWORD PTR [esp+0x10],edi
  449f08:	0f 8c 6d fe ff ff    	jl     0x449d7b
  449f0e:	84 db                	test   bl,bl
  449f10:	74 09                	je     0x449f1b
  449f12:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  449f16:	e8 55 f1 fc ff       	call   0x419070
  449f1b:	5f                   	pop    edi
  449f1c:	5e                   	pop    esi
  449f1d:	8a c3                	mov    al,bl
  449f1f:	5d                   	pop    ebp
  449f20:	5b                   	pop    ebx
  449f21:	8b 4c 24 70          	mov    ecx,DWORD PTR [esp+0x70]
  449f25:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  449f2c:	83 c4 7c             	add    esp,0x7c
  449f2f:	c2 08 00             	ret    0x8
  449f32:	8b 4c 24 74          	mov    ecx,DWORD PTR [esp+0x74]
  449f36:	32 c0                	xor    al,al
  449f38:	5b                   	pop    ebx
  449f39:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  449f40:	83 c4 7c             	add    esp,0x7c
  449f43:	c2 08 00             	ret    0x8
  449f46:	90                   	nop
  449f47:	90                   	nop
  449f48:	90                   	nop
  449f49:	90                   	nop
  449f4a:	90                   	nop
  449f4b:	90                   	nop
  449f4c:	90                   	nop
  449f4d:	90                   	nop
  449f4e:	90                   	nop
  449f4f:	90                   	nop
  449f50:	81 ec 58 01 00 00    	sub    esp,0x158
  449f56:	8d 44 24 2c          	lea    eax,[esp+0x2c]
  449f5a:	53                   	push   ebx
  449f5b:	55                   	push   ebp
  449f5c:	8b e9                	mov    ebp,ecx
  449f5e:	56                   	push   esi
  449f5f:	8b 8c 24 68 01 00 00 	mov    ecx,DWORD PTR [esp+0x168]
  449f66:	57                   	push   edi
  449f67:	50                   	push   eax
  449f68:	51                   	push   ecx
  449f69:	83 cb ff             	or     ebx,0xffffffff
  449f6c:	e8 6f fd ff ff       	call   0x449ce0
  449f71:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  449f75:	8d 44 24 28          	lea    eax,[esp+0x28]
  449f79:	52                   	push   edx
  449f7a:	8d 8c 24 ac 00 00 00 	lea    ecx,[esp+0xac]
  449f81:	50                   	push   eax
  449f82:	8d 54 24 20          	lea    edx,[esp+0x20]
  449f86:	51                   	push   ecx
  449f87:	8d 44 24 50          	lea    eax,[esp+0x50]
  449f8b:	52                   	push   edx
  449f8c:	50                   	push   eax
  449f8d:	e8 6f 7c 00 00       	call   0x451c01
  449f92:	83 c4 1c             	add    esp,0x1c
  449f95:	33 f6                	xor    esi,esi
  449f97:	8d 7d 24             	lea    edi,[ebp+0x24]
  449f9a:	83 fb ff             	cmp    ebx,0xffffffff
  449f9d:	75 2a                	jne    0x449fc9
  449f9f:	8a 44 2e 04          	mov    al,BYTE PTR [esi+ebp*1+0x4]
  449fa3:	84 c0                	test   al,al
  449fa5:	74 19                	je     0x449fc0
  449fa7:	8b 0f                	mov    ecx,DWORD PTR [edi]
  449fa9:	8d 94 24 a0 00 00 00 	lea    edx,[esp+0xa0]
  449fb0:	51                   	push   ecx
  449fb1:	52                   	push   edx
  449fb2:	e8 c9 85 00 00       	call   0x452580
  449fb7:	83 c4 08             	add    esp,0x8
  449fba:	85 c0                	test   eax,eax
  449fbc:	74 02                	je     0x449fc0
  449fbe:	8b de                	mov    ebx,esi
  449fc0:	46                   	inc    esi
  449fc1:	83 c7 04             	add    edi,0x4
  449fc4:	83 fe 1e             	cmp    esi,0x1e
  449fc7:	7c d1                	jl     0x449f9a
  449fc9:	5f                   	pop    edi
  449fca:	5e                   	pop    esi
  449fcb:	8b c3                	mov    eax,ebx
  449fcd:	5d                   	pop    ebp
  449fce:	5b                   	pop    ebx
  449fcf:	81 c4 58 01 00 00    	add    esp,0x158
  449fd5:	c2 04 00             	ret    0x4
  449fd8:	90                   	nop
  449fd9:	90                   	nop
  449fda:	90                   	nop
  449fdb:	90                   	nop
  449fdc:	90                   	nop
  449fdd:	90                   	nop
  449fde:	90                   	nop
  449fdf:	90                   	nop
  449fe0:	56                   	push   esi
  449fe1:	57                   	push   edi
  449fe2:	8b 7c 24 0c          	mov    edi,DWORD PTR [esp+0xc]
  449fe6:	8b f1                	mov    esi,ecx
  449fe8:	57                   	push   edi
  449fe9:	e8 62 ff ff ff       	call   0x449f50
  449fee:	83 f8 ff             	cmp    eax,0xffffffff
  449ff1:	74 12                	je     0x44a005
  449ff3:	8b 8c 86 14 01 00 00 	mov    ecx,DWORD PTR [esi+eax*4+0x114]
  449ffa:	57                   	push   edi
  449ffb:	e8 50 fb ff ff       	call   0x449b50
  44a000:	83 f8 ff             	cmp    eax,0xffffffff
  44a003:	75 07                	jne    0x44a00c
  44a005:	5f                   	pop    edi
  44a006:	32 c0                	xor    al,al
  44a008:	5e                   	pop    esi
  44a009:	c2 04 00             	ret    0x4
  44a00c:	5f                   	pop    edi
  44a00d:	b0 01                	mov    al,0x1
  44a00f:	5e                   	pop    esi
  44a010:	c2 04 00             	ret    0x4
  44a013:	90                   	nop
  44a014:	90                   	nop
  44a015:	90                   	nop
  44a016:	90                   	nop
  44a017:	90                   	nop
  44a018:	90                   	nop
  44a019:	90                   	nop
  44a01a:	90                   	nop
  44a01b:	90                   	nop
  44a01c:	90                   	nop
  44a01d:	90                   	nop
  44a01e:	90                   	nop
  44a01f:	90                   	nop
  44a020:	53                   	push   ebx
  44a021:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  44a025:	55                   	push   ebp
  44a026:	56                   	push   esi
  44a027:	57                   	push   edi
  44a028:	8b f1                	mov    esi,ecx
  44a02a:	53                   	push   ebx
  44a02b:	33 ed                	xor    ebp,ebp
  44a02d:	e8 1e ff ff ff       	call   0x449f50
  44a032:	8b f8                	mov    edi,eax
  44a034:	83 ff ff             	cmp    edi,0xffffffff
  44a037:	74 21                	je     0x44a05a
  44a039:	8b 8c be 14 01 00 00 	mov    ecx,DWORD PTR [esi+edi*4+0x114]
  44a040:	53                   	push   ebx
  44a041:	e8 0a fb ff ff       	call   0x449b50
  44a046:	8b 8c be 14 01 00 00 	mov    ecx,DWORD PTR [esi+edi*4+0x114]
  44a04d:	50                   	push   eax
  44a04e:	e8 1d fb ff ff       	call   0x449b70
  44a053:	5f                   	pop    edi
  44a054:	5e                   	pop    esi
  44a055:	5d                   	pop    ebp
  44a056:	5b                   	pop    ebx
  44a057:	c2 04 00             	ret    0x4
  44a05a:	5f                   	pop    edi
  44a05b:	8b c5                	mov    eax,ebp
  44a05d:	5e                   	pop    esi
  44a05e:	5d                   	pop    ebp
  44a05f:	5b                   	pop    ebx
  44a060:	c2 04 00             	ret    0x4
  44a063:	90                   	nop
  44a064:	90                   	nop
  44a065:	90                   	nop
  44a066:	90                   	nop
  44a067:	90                   	nop
  44a068:	90                   	nop
  44a069:	90                   	nop
  44a06a:	90                   	nop
  44a06b:	90                   	nop
  44a06c:	90                   	nop
  44a06d:	90                   	nop
  44a06e:	90                   	nop
  44a06f:	90                   	nop
  44a070:	53                   	push   ebx
  44a071:	55                   	push   ebp
  44a072:	8b 6c 24 0c          	mov    ebp,DWORD PTR [esp+0xc]
  44a076:	56                   	push   esi
  44a077:	57                   	push   edi
  44a078:	8b f1                	mov    esi,ecx
  44a07a:	55                   	push   ebp
  44a07b:	32 db                	xor    bl,bl
  44a07d:	e8 ce fe ff ff       	call   0x449f50
  44a082:	8b f8                	mov    edi,eax
  44a084:	85 ff                	test   edi,edi
  44a086:	7c 30                	jl     0x44a0b8
  44a088:	8b 8c be 14 01 00 00 	mov    ecx,DWORD PTR [esi+edi*4+0x114]
  44a08f:	55                   	push   ebp
  44a090:	e8 bb fa ff ff       	call   0x449b50
  44a095:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  44a099:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  44a09d:	51                   	push   ecx
  44a09e:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  44a0a2:	52                   	push   edx
  44a0a3:	51                   	push   ecx
  44a0a4:	8b 8c be 14 01 00 00 	mov    ecx,DWORD PTR [esi+edi*4+0x114]
  44a0ab:	50                   	push   eax
  44a0ac:	e8 ef fa ff ff       	call   0x449ba0
  44a0b1:	5f                   	pop    edi
  44a0b2:	5e                   	pop    esi
  44a0b3:	5d                   	pop    ebp
  44a0b4:	5b                   	pop    ebx
  44a0b5:	c2 10 00             	ret    0x10
  44a0b8:	5f                   	pop    edi
  44a0b9:	5e                   	pop    esi
  44a0ba:	8a c3                	mov    al,bl
  44a0bc:	5d                   	pop    ebp
  44a0bd:	5b                   	pop    ebx
  44a0be:	c2 10 00             	ret    0x10

; ===== EFF 动态SF2路径族末端；排除为SAF requester | 0x0043DCB0..0x0043DD40 =====
  43dcb0:	83 ec 08             	sub    esp,0x8
  43dcb3:	53                   	push   ebx
  43dcb4:	55                   	push   ebp
  43dcb5:	56                   	push   esi
  43dcb6:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
  43dcba:	33 db                	xor    ebx,ebx
  43dcbc:	8b e9                	mov    ebp,ecx
  43dcbe:	3b f3                	cmp    esi,ebx
  43dcc0:	88 5c 24 0c          	mov    BYTE PTR [esp+0xc],bl
  43dcc4:	88 5c 24 0d          	mov    BYTE PTR [esp+0xd],bl
  43dcc8:	88 5c 24 0e          	mov    BYTE PTR [esp+0xe],bl
  43dccc:	88 5c 24 0f          	mov    BYTE PTR [esp+0xf],bl
  43dcd0:	88 5c 24 10          	mov    BYTE PTR [esp+0x10],bl
  43dcd4:	88 5c 24 11          	mov    BYTE PTR [esp+0x11],bl
  43dcd8:	75 0e                	jne    0x43dce8
  43dcda:	5e                   	pop    esi
  43dcdb:	5d                   	pop    ebp
  43dcdc:	b8 80 9f 46 00       	mov    eax,0x469f80
  43dce1:	5b                   	pop    ebx
  43dce2:	83 c4 08             	add    esp,0x8
  43dce5:	c2 04 00             	ret    0x4
  43dce8:	57                   	push   edi
  43dce9:	6a 03                	push   0x3
  43dceb:	8d 44 24 14          	lea    eax,[esp+0x14]
  43dcef:	56                   	push   esi
  43dcf0:	50                   	push   eax
  43dcf1:	e8 5a 46 01 00       	call   0x452350
  43dcf6:	8d bd c0 00 00 00    	lea    edi,[ebp+0xc0]
  43dcfc:	83 c4 0c             	add    esp,0xc
  43dcff:	3b fb                	cmp    edi,ebx
  43dd01:	74 55                	je     0x43dd58
  43dd03:	56                   	push   esi
  43dd04:	8b cd                	mov    ecx,ebp
  43dd06:	e8 75 00 00 00       	call   0x43dd80
  43dd0b:	84 c0                	test   al,al
  43dd0d:	56                   	push   esi
  43dd0e:	75 24                	jne    0x43dd34
  43dd10:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  43dd14:	51                   	push   ecx
  43dd15:	68 78 9f 46 00       	push   0x469f78
  43dd1a:	68 6c 9f 46 00       	push   0x469f6c
  43dd1f:	57                   	push   edi
  43dd20:	e8 4e 3d 01 00       	call   0x451a73
  43dd25:	83 c4 14             	add    esp,0x14
  43dd28:	8b c7                	mov    eax,edi
  43dd2a:	5f                   	pop    edi
  43dd2b:	5e                   	pop    esi
  43dd2c:	5d                   	pop    ebp
  43dd2d:	5b                   	pop    ebx
  43dd2e:	83 c4 08             	add    esp,0x8
  43dd31:	c2 04 00             	ret    0x4
  43dd34:	8d 54 24 14          	lea    edx,[esp+0x14]
  43dd38:	52                   	push   edx
  43dd39:	68 78 9f 46 00       	push   0x469f78
  43dd3e:	68 64 9f 46 00       	push   0x469f64

