  422f01:	44                   	inc    esp
  422f02:	24 04                	and    al,0x4
  422f04:	56                   	push   esi
  422f05:	83 f8 01             	cmp    eax,0x1
  422f08:	57                   	push   edi
  422f09:	8b f1                	mov    esi,ecx
  422f0b:	75 4a                	jne    0x422f57
  422f0d:	8b be f4 08 00 00    	mov    edi,DWORD PTR [esi+0x8f4]
  422f13:	85 ff                	test   edi,edi
  422f15:	7e 40                	jle    0x422f57
  422f17:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  422f1b:	85 c0                	test   eax,eax
  422f1d:	7f 13                	jg     0x422f32
  422f1f:	e8 41 e6 02 00       	call   0x451565
  422f24:	99                   	cdq
  422f25:	f7 ff                	idiv   edi
  422f27:	8b 84 96 f8 08 00 00 	mov    eax,DWORD PTR [esi+edx*4+0x8f8]
  422f2e:	85 c0                	test   eax,eax
  422f30:	7e 25                	jle    0x422f57
  422f32:	8b 15 ac 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01ac
  422f38:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  422f3f:	2b c8                	sub    ecx,eax
  422f41:	89 86 a8 07 00 00    	mov    DWORD PTR [esi+0x7a8],eax
  422f47:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
  422f4a:	c1 e1 04             	shl    ecx,0x4
  422f4d:	03 ca                	add    ecx,edx
  422f4f:	89 8e 48 08 00 00    	mov    DWORD PTR [esi+0x848],ecx
  422f55:	eb 28                	jmp    0x422f7f
  422f57:	8b 8e 4c 08 00 00    	mov    ecx,DWORD PTR [esi+0x84c]
  422f5d:	89 8e a8 07 00 00    	mov    DWORD PTR [esi+0x7a8],ecx
  422f63:	8d 04 cd 00 00 00 00 	lea    eax,[ecx*8+0x0]
  422f6a:	2b c1                	sub    eax,ecx
  422f6c:	8d 14 80             	lea    edx,[eax+eax*4]
  422f6f:	a1 ac 01 8a 00       	mov    eax,ds:0x8a01ac
  422f74:	c1 e2 04             	shl    edx,0x4
  422f77:	03 d0                	add    edx,eax
  422f79:	89 96 48 08 00 00    	mov    DWORD PTR [esi+0x848],edx
  422f7f:	8b ce                	mov    ecx,esi
  422f81:	e8 2a fd ff ff       	call   0x422cb0
  422f86:	85 c0                	test   eax,eax
  422f88:	89 86 a0 07 00 00    	mov    DWORD PTR [esi+0x7a0],eax
  422f8e:	0f 8c d6 00 00 00    	jl     0x42306a
  422f94:	8b 96 48 08 00 00    	mov    edx,DWORD PTR [esi+0x848]
  422f9a:	8b 4a 18             	mov    ecx,DWORD PTR [edx+0x18]
  422f9d:	85 c9                	test   ecx,ecx
  422f9f:	75 41                	jne    0x422fe2
  422fa1:	8b 8e dc 08 00 00    	mov    ecx,DWORD PTR [esi+0x8dc]
  422fa7:	85 c9                	test   ecx,ecx
  422fa9:	7e 2f                	jle    0x422fda
  422fab:	8d 14 cd 00 00 00 00 	lea    edx,[ecx*8+0x0]
  422fb2:	33 ff                	xor    edi,edi
  422fb4:	2b d1                	sub    edx,ecx
  422fb6:	8d 14 91             	lea    edx,[ecx+edx*4]
  422fb9:	c1 e2 04             	shl    edx,0x4
  422fbc:	2b d1                	sub    edx,ecx
  422fbe:	8b 0d b0 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b0
  422fc4:	8d 0c 51             	lea    ecx,[ecx+edx*2]
  422fc7:	8b 49 34             	mov    ecx,DWORD PTR [ecx+0x34]
  422fca:	85 c9                	test   ecx,ecx
  422fcc:	74 1a                	je     0x422fe8
  422fce:	83 f9 01             	cmp    ecx,0x1
  422fd1:	75 04                	jne    0x422fd7
  422fd3:	33 c9                	xor    ecx,ecx
  422fd5:	eb 11                	jmp    0x422fe8
  422fd7:	49                   	dec    ecx
  422fd8:	eb 0e                	jmp    0x422fe8
  422fda:	8b 7a 20             	mov    edi,DWORD PTR [edx+0x20]
  422fdd:	8b 4a 24             	mov    ecx,DWORD PTR [edx+0x24]
  422fe0:	eb 06                	jmp    0x422fe8
  422fe2:	8b 4a 24             	mov    ecx,DWORD PTR [edx+0x24]
  422fe5:	8b 7a 20             	mov    edi,DWORD PTR [edx+0x20]
  422fe8:	8d 96 88 00 00 00    	lea    edx,[esi+0x88]
  422fee:	52                   	push   edx
  422fef:	6a 10                	push   0x10
  422ff1:	51                   	push   ecx
  422ff2:	57                   	push   edi
  422ff3:	50                   	push   eax
  422ff4:	8b ce                	mov    ecx,esi
  422ff6:	e8 55 f8 ff ff       	call   0x422850
  422ffb:	89 86 c8 00 00 00    	mov    DWORD PTR [esi+0xc8],eax
  423001:	8b 86 f0 0c 00 00    	mov    eax,DWORD PTR [esi+0xcf0]
  423007:	83 f8 04             	cmp    eax,0x4
  42300a:	75 28                	jne    0x423034
  42300c:	81 c6 a8 0c 00 00    	add    esi,0xca8
  423012:	6a 00                	push   0x0
  423014:	6a 02                	push   0x2
  423016:	6a 0c                	push   0xc
  423018:	8b ce                	mov    ecx,esi
  42301a:	e8 71 0f 00 00       	call   0x423f90
  42301f:	68 a0 86 01 00       	push   0x186a0
  423024:	6a 03                	push   0x3
  423026:	6a 0c                	push   0xc
  423028:	8b ce                	mov    ecx,esi
  42302a:	e8 61 0f 00 00       	call   0x423f90
  42302f:	5f                   	pop    edi
  423030:	5e                   	pop    esi
  423031:	c2 08 00             	ret    0x8
  423034:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  42303a:	8d be a8 0c 00 00    	lea    edi,[esi+0xca8]
  423040:	8b 88 8c 00 00 00    	mov    ecx,DWORD PTR [eax+0x8c]
  423046:	51                   	push   ecx
  423047:	6a 02                	push   0x2
  423049:	6a 0c                	push   0xc
  42304b:	8b cf                	mov    ecx,edi
  42304d:	e8 3e 0f 00 00       	call   0x423f90
  423052:	8b 96 48 08 00 00    	mov    edx,DWORD PTR [esi+0x848]
  423058:	8b cf                	mov    ecx,edi
  42305a:	8b 82 90 00 00 00    	mov    eax,DWORD PTR [edx+0x90]
  423060:	50                   	push   eax
  423061:	6a 03                	push   0x3
  423063:	6a 0c                	push   0xc
  423065:	e8 26 0f 00 00       	call   0x423f90
  42306a:	5f                   	pop    edi
