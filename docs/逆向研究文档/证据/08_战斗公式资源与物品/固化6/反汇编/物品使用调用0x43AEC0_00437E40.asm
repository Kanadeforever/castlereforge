  437e40:	83 ec 74             	sub    esp,0x74
  437e43:	53                   	push   ebx
  437e44:	55                   	push   ebp
  437e45:	56                   	push   esi
  437e46:	8b f1                	mov    esi,ecx
  437e48:	8b 0d d4 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01d4
  437e4e:	57                   	push   edi
  437e4f:	e8 fc 4f ff ff       	call   0x42ce50
  437e54:	8a 8e 79 05 00 00    	mov    cl,BYTE PTR [esi+0x579]
  437e5a:	8b d8                	mov    ebx,eax
  437e5c:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  437e62:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  437e66:	88 88 78 05 00 00    	mov    BYTE PTR [eax+0x578],cl
  437e6c:	8b 96 88 05 00 00    	mov    edx,DWORD PTR [esi+0x588]
  437e72:	8a 86 79 05 00 00    	mov    al,BYTE PTR [esi+0x579]
  437e78:	88 82 78 05 00 00    	mov    BYTE PTR [edx+0x578],al
  437e7e:	8b 8e 8c 05 00 00    	mov    ecx,DWORD PTR [esi+0x58c]
  437e84:	8a 96 79 05 00 00    	mov    dl,BYTE PTR [esi+0x579]
  437e8a:	88 91 78 05 00 00    	mov    BYTE PTR [ecx+0x578],dl
  437e90:	8b 86 90 05 00 00    	mov    eax,DWORD PTR [esi+0x590]
  437e96:	8a 8e 79 05 00 00    	mov    cl,BYTE PTR [esi+0x579]
  437e9c:	88 88 78 05 00 00    	mov    BYTE PTR [eax+0x578],cl
  437ea2:	8a 86 79 05 00 00    	mov    al,BYTE PTR [esi+0x579]
  437ea8:	84 c0                	test   al,al
  437eaa:	0f 84 f7 04 00 00    	je     0x4383a7
  437eb0:	8b 8e b0 05 00 00    	mov    ecx,DWORD PTR [esi+0x5b0]
  437eb6:	8a 81 79 05 00 00    	mov    al,BYTE PTR [ecx+0x579]
  437ebc:	84 c0                	test   al,al
  437ebe:	74 69                	je     0x437f29
  437ec0:	e8 bb f9 fe ff       	call   0x427880
  437ec5:	83 f8 01             	cmp    eax,0x1
  437ec8:	0f 85 d9 04 00 00    	jne    0x4383a7
  437ece:	8b 96 80 05 00 00    	mov    edx,DWORD PTR [esi+0x580]
  437ed4:	6a 00                	push   0x0
  437ed6:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  437ed9:	e8 62 67 00 00       	call   0x43e640
  437ede:	8b 86 94 05 00 00    	mov    eax,DWORD PTR [esi+0x594]
  437ee4:	50                   	push   eax
  437ee5:	e8 16 fb ff ff       	call   0x437a00
  437eea:	8b 8e a0 05 00 00    	mov    ecx,DWORD PTR [esi+0x5a0]
  437ef0:	8b 96 98 05 00 00    	mov    edx,DWORD PTR [esi+0x598]
  437ef6:	83 c4 04             	add    esp,0x4
  437ef9:	8b f8                	mov    edi,eax
  437efb:	8d 04 ca             	lea    eax,[edx+ecx*8]
  437efe:	8b cb                	mov    ecx,ebx
  437f00:	57                   	push   edi
  437f01:	50                   	push   eax
  437f02:	e8 29 18 00 00       	call   0x439730
  437f07:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  437f0d:	57                   	push   edi
  437f0e:	50                   	push   eax
  437f0f:	8b 89 90 00 00 00    	mov    ecx,DWORD PTR [ecx+0x90]
  437f15:	e8 96 16 00 00       	call   0x4395b0
  437f1a:	8b ce                	mov    ecx,esi
  437f1c:	e8 8f 04 00 00       	call   0x4383b0
  437f21:	5f                   	pop    edi
  437f22:	5e                   	pop    esi
  437f23:	5d                   	pop    ebp
  437f24:	5b                   	pop    ebx
  437f25:	83 c4 74             	add    esp,0x74
  437f28:	c3                   	ret
  437f29:	8b 8e ac 05 00 00    	mov    ecx,DWORD PTR [esi+0x5ac]
  437f2f:	8a 81 79 05 00 00    	mov    al,BYTE PTR [ecx+0x579]
  437f35:	84 c0                	test   al,al
  437f37:	0f 84 35 01 00 00    	je     0x438072
  437f3d:	e8 3e f9 fe ff       	call   0x427880
  437f42:	83 f8 01             	cmp    eax,0x1
  437f45:	0f 85 5c 04 00 00    	jne    0x4383a7
  437f4b:	8b 96 94 05 00 00    	mov    edx,DWORD PTR [esi+0x594]
  437f51:	52                   	push   edx
  437f52:	e8 a9 fa ff ff       	call   0x437a00
  437f57:	8b 8e 98 05 00 00    	mov    ecx,DWORD PTR [esi+0x598]
  437f5d:	8b e8                	mov    ebp,eax
  437f5f:	8b 86 a0 05 00 00    	mov    eax,DWORD PTR [esi+0x5a0]
  437f65:	83 c4 04             	add    esp,0x4
  437f68:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  437f6c:	8d 14 c1             	lea    edx,[ecx+eax*8]
  437f6f:	55                   	push   ebp
  437f70:	52                   	push   edx
  437f71:	8b cb                	mov    ecx,ebx
  437f73:	e8 b8 17 00 00       	call   0x439730
  437f78:	8b f8                	mov    edi,eax
  437f7a:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  437f80:	8d 04 fd 00 00 00 00 	lea    eax,[edi*8+0x0]
  437f87:	2b c7                	sub    eax,edi
  437f89:	8d 0c 87             	lea    ecx,[edi+eax*4]
  437f8c:	8b 82 ac 00 00 00    	mov    eax,DWORD PTR [edx+0xac]
  437f92:	c1 e1 04             	shl    ecx,0x4
  437f95:	2b cf                	sub    ecx,edi
  437f97:	89 7c 24 24          	mov    DWORD PTR [esp+0x24],edi
  437f9b:	8d 0c 48             	lea    ecx,[eax+ecx*2]
  437f9e:	8b 86 a8 05 00 00    	mov    eax,DWORD PTR [esi+0x5a8]
  437fa4:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  437fa8:	89 4c 24 20          	mov    DWORD PTR [esp+0x20],ecx
  437fac:	83 b9 bc 00 00 00 03 	cmp    DWORD PTR [ecx+0xbc],0x3
  437fb3:	0f 85 98 00 00 00    	jne    0x438051
  437fb9:	8b 42 3c             	mov    eax,DWORD PTR [edx+0x3c]
  437fbc:	33 db                	xor    ebx,ebx
  437fbe:	85 c0                	test   eax,eax
  437fc0:	89 86 b4 05 00 00    	mov    DWORD PTR [esi+0x5b4],eax
  437fc6:	89 44 24 30          	mov    DWORD PTR [esp+0x30],eax
  437fca:	7e 4a                	jle    0x438016
  437fcc:	b9 68 00 00 00       	mov    ecx,0x68
  437fd1:	8d 6c 24 34          	lea    ebp,[esp+0x34]
  437fd5:	2b cd                	sub    ecx,ebp
  437fd7:	8d 44 24 34          	lea    eax,[esp+0x34]
  437fdb:	8d 96 b8 05 00 00    	lea    edx,[esi+0x5b8]
  437fe1:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  437fe5:	eb 04                	jmp    0x437feb
  437fe7:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  437feb:	8b 2d 48 1c 8e 00    	mov    ebp,DWORD PTR ds:0x8e1c48
  437ff1:	03 c8                	add    ecx,eax
  437ff3:	43                   	inc    ebx
  437ff4:	83 c2 04             	add    edx,0x4
  437ff7:	8b 0c 29             	mov    ecx,DWORD PTR [ecx+ebp*1]
  437ffa:	89 4a fc             	mov    DWORD PTR [edx-0x4],ecx
  437ffd:	89 08                	mov    DWORD PTR [eax],ecx
  437fff:	8b 8e b4 05 00 00    	mov    ecx,DWORD PTR [esi+0x5b4]
  438005:	83 c0 04             	add    eax,0x4
  438008:	3b d9                	cmp    ebx,ecx
  43800a:	89 be e0 05 00 00    	mov    DWORD PTR [esi+0x5e0],edi
  438010:	7c d5                	jl     0x437fe7
  438012:	8b 6c 24 1c          	mov    ebp,DWORD PTR [esp+0x1c]
  438016:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  43801a:	8d 4c 24 24          	lea    ecx,[esp+0x24]
  43801e:	51                   	push   ecx
  43801f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  438025:	e8 96 2e 00 00       	call   0x43aec0
  43802a:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  43802e:	83 ba b0 00 00 00 01 	cmp    DWORD PTR [edx+0xb0],0x1
  438035:	75 0b                	jne    0x438042
  438037:	55                   	push   ebp
  438038:	6a 01                	push   0x1
  43803a:	57                   	push   edi
  43803b:	8b cb                	mov    ecx,ebx
  43803d:	e8 7e 13 00 00       	call   0x4393c0
  438042:	8b ce                	mov    ecx,esi
  438044:	e8 67 03 00 00       	call   0x4383b0
  438049:	5f                   	pop    edi
  43804a:	5e                   	pop    esi
  43804b:	5d                   	pop    ebp
  43804c:	5b                   	pop    ebx
  43804d:	83 c4 74             	add    esp,0x74
  438050:	c3                   	ret
  438051:	b9 01 00 00 00       	mov    ecx,0x1
  438056:	89 44 24 34          	mov    DWORD PTR [esp+0x34],eax
  43805a:	89 4c 24 30          	mov    DWORD PTR [esp+0x30],ecx
  43805e:	89 8e b4 05 00 00    	mov    DWORD PTR [esi+0x5b4],ecx
  438064:	89 86 b8 05 00 00    	mov    DWORD PTR [esi+0x5b8],eax
  43806a:	89 be e0 05 00 00    	mov    DWORD PTR [esi+0x5e0],edi
  438070:	eb a8                	jmp    0x43801a
  438072:	83 cf ff             	or     edi,0xffffffff
  438075:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  43807a:	89 7c 24 14          	mov    DWORD PTR [esp+0x14],edi
  43807e:	33 db                	xor    ebx,ebx
  438080:	bd 7c 05 00 00       	mov    ebp,0x57c
  438085:	83 ff ff             	cmp    edi,0xffffffff
  438088:	75 26                	jne    0x4380b0
  43808a:	8b 86 84 05 00 00    	mov    eax,DWORD PTR [esi+0x584]
  438090:	8b 0c 28             	mov    ecx,DWORD PTR [eax+ebp*1]
  438093:	e8 e8 92 ff ff       	call   0x431380
  438098:	83 f8 02             	cmp    eax,0x2
  43809b:	75 02                	jne    0x43809f
  43809d:	8b fb                	mov    edi,ebx
  43809f:	83 c5 04             	add    ebp,0x4
  4380a2:	43                   	inc    ebx
  4380a3:	81 fd 94 05 00 00    	cmp    ebp,0x594
  4380a9:	7c da                	jl     0x438085
  4380ab:	83 ff ff             	cmp    edi,0xffffffff
  4380ae:	74 73                	je     0x438123
  4380b0:	39 be 94 05 00 00    	cmp    DWORD PTR [esi+0x594],edi
  4380b6:	74 6b                	je     0x438123
  4380b8:	8b 8e 84 05 00 00    	mov    ecx,DWORD PTR [esi+0x584]
  4380be:	57                   	push   edi
  4380bf:	8b 89 64 05 00 00    	mov    ecx,DWORD PTR [ecx+0x564]
  4380c5:	e8 26 66 00 00       	call   0x43e6f0
  4380ca:	8b 86 90 05 00 00    	mov    eax,DWORD PTR [esi+0x590]
  4380d0:	83 ff 05             	cmp    edi,0x5
  4380d3:	0f 94 c2             	sete   dl
  4380d6:	8b 88 e8 05 00 00    	mov    ecx,DWORD PTR [eax+0x5e8]
  4380dc:	83 ff 05             	cmp    edi,0x5
  4380df:	88 51 45             	mov    BYTE PTR [ecx+0x45],dl
  4380e2:	74 09                	je     0x4380ed
  4380e4:	83 ff 04             	cmp    edi,0x4
  4380e7:	74 04                	je     0x4380ed
  4380e9:	b0 01                	mov    al,0x1
  4380eb:	eb 02                	jmp    0x4380ef
  4380ed:	32 c0                	xor    al,al
  4380ef:	8b 96 90 05 00 00    	mov    edx,DWORD PTR [esi+0x590]
  4380f5:	8b 8a ec 05 00 00    	mov    ecx,DWORD PTR [edx+0x5ec]
  4380fb:	88 41 45             	mov    BYTE PTR [ecx+0x45],al
  4380fe:	33 c0                	xor    eax,eax
  438100:	8b ce                	mov    ecx,esi
  438102:	89 be 94 05 00 00    	mov    DWORD PTR [esi+0x594],edi
  438108:	89 86 98 05 00 00    	mov    DWORD PTR [esi+0x598],eax
