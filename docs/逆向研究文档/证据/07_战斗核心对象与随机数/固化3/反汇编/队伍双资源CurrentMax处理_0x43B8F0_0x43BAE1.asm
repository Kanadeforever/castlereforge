; 幽城幻剑录 v0.7D阶段中-固化3 静态证据
; Oracle RPG.exe SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; VA范围: 0x0043B8F0..0x0043BAE0
; 用途: 跨 EVE/Battle 交叉证明 RoleDefinition +2C/+30 与 +34/+38 分别构成 max/current 资源对，并执行增减、百分比设置与 clamp。
; 边界: 结构语义可冻结；作者资源术语仍需字符串/UI/其他直接证据。

  43b8f0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  43b8f4:	53                   	push   ebx
  43b8f5:	55                   	push   ebp
  43b8f6:	56                   	push   esi
  43b8f7:	48                   	dec    eax
  43b8f8:	57                   	push   edi
  43b8f9:	0f 84 e6 00 00 00    	je     0x43b9e5
  43b8ff:	48                   	dec    eax
  43b900:	0f 85 db 01 00 00    	jne    0x43bae1
  43b906:	8b 41 3c             	mov    eax,DWORD PTR [ecx+0x3c]
  43b909:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  43b910:	00 
  43b911:	85 c0                	test   eax,eax
  43b913:	0f 8e c8 01 00 00    	jle    0x43bae1
  43b919:	8b 6c 24 24          	mov    ebp,DWORD PTR [esp+0x24]
  43b91d:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
  43b921:	8d 71 68             	lea    esi,[ecx+0x68]
  43b924:	85 db                	test   ebx,ebx
  43b926:	74 4c                	je     0x43b974
  43b928:	8b 06                	mov    eax,DWORD PTR [esi]
  43b92a:	8d 14 40             	lea    edx,[eax+eax*2]
  43b92d:	8d 14 d2             	lea    edx,[edx+edx*8]
  43b930:	d1 e2                	shl    edx,1
  43b932:	2b d0                	sub    edx,eax
  43b934:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43b93a:	c1 e2 04             	shl    edx,0x4
  43b93d:	8d 54 02 30          	lea    edx,[edx+eax*1+0x30]
  43b941:	8b 02                	mov    eax,DWORD PTR [edx]
  43b943:	03 c3                	add    eax,ebx
  43b945:	85 c0                	test   eax,eax
  43b947:	7f 05                	jg     0x43b94e
  43b949:	b8 01 00 00 00       	mov    eax,0x1
  43b94e:	89 02                	mov    DWORD PTR [edx],eax
  43b950:	8b 06                	mov    eax,DWORD PTR [esi]
  43b952:	8d 14 40             	lea    edx,[eax+eax*2]
  43b955:	8d 14 d2             	lea    edx,[edx+edx*8]
  43b958:	d1 e2                	shl    edx,1
  43b95a:	2b d0                	sub    edx,eax
  43b95c:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43b962:	c1 e2 04             	shl    edx,0x4
  43b965:	03 c2                	add    eax,edx
  43b967:	8b 50 2c             	mov    edx,DWORD PTR [eax+0x2c]
  43b96a:	8b 78 30             	mov    edi,DWORD PTR [eax+0x30]
  43b96d:	3b fa                	cmp    edi,edx
  43b96f:	7e 03                	jle    0x43b974
  43b971:	89 50 30             	mov    DWORD PTR [eax+0x30],edx
  43b974:	85 ed                	test   ebp,ebp
  43b976:	74 4d                	je     0x43b9c5
  43b978:	8b 06                	mov    eax,DWORD PTR [esi]
  43b97a:	8d 14 40             	lea    edx,[eax+eax*2]
  43b97d:	8d 14 d2             	lea    edx,[edx+edx*8]
  43b980:	d1 e2                	shl    edx,1
  43b982:	2b d0                	sub    edx,eax
  43b984:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43b98a:	c1 e2 04             	shl    edx,0x4
  43b98d:	8d 7c 02 38          	lea    edi,[edx+eax*1+0x38]
  43b991:	33 d2                	xor    edx,edx
  43b993:	8b 07                	mov    eax,DWORD PTR [edi]
  43b995:	03 c5                	add    eax,ebp
  43b997:	85 c0                	test   eax,eax
  43b999:	0f 9e c2             	setle  dl
  43b99c:	4a                   	dec    edx
  43b99d:	23 d0                	and    edx,eax
  43b99f:	89 17                	mov    DWORD PTR [edi],edx
  43b9a1:	8b 06                	mov    eax,DWORD PTR [esi]
  43b9a3:	8d 14 40             	lea    edx,[eax+eax*2]
  43b9a6:	8d 14 d2             	lea    edx,[edx+edx*8]
  43b9a9:	d1 e2                	shl    edx,1
  43b9ab:	2b d0                	sub    edx,eax
  43b9ad:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43b9b3:	c1 e2 04             	shl    edx,0x4
  43b9b6:	03 c2                	add    eax,edx
  43b9b8:	8b 50 34             	mov    edx,DWORD PTR [eax+0x34]
  43b9bb:	8b 78 38             	mov    edi,DWORD PTR [eax+0x38]
  43b9be:	3b fa                	cmp    edi,edx
  43b9c0:	7e 03                	jle    0x43b9c5
  43b9c2:	89 50 38             	mov    DWORD PTR [eax+0x38],edx
  43b9c5:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  43b9c9:	8b 51 3c             	mov    edx,DWORD PTR [ecx+0x3c]
  43b9cc:	40                   	inc    eax
  43b9cd:	83 c6 04             	add    esi,0x4
  43b9d0:	3b c2                	cmp    eax,edx
  43b9d2:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  43b9d6:	0f 8c 48 ff ff ff    	jl     0x43b924
  43b9dc:	5f                   	pop    edi
  43b9dd:	5e                   	pop    esi
  43b9de:	5d                   	pop    ebp
  43b9df:	32 c0                	xor    al,al
  43b9e1:	5b                   	pop    ebx
  43b9e2:	c2 14 00             	ret    0x14
  43b9e5:	8b 41 3c             	mov    eax,DWORD PTR [ecx+0x3c]
  43b9e8:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  43b9ef:	00 
  43b9f0:	85 c0                	test   eax,eax
  43b9f2:	0f 8e e9 00 00 00    	jle    0x43bae1
  43b9f8:	8b 6c 24 1c          	mov    ebp,DWORD PTR [esp+0x1c]
  43b9fc:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  43ba00:	8d 71 68             	lea    esi,[ecx+0x68]
  43ba03:	85 db                	test   ebx,ebx
  43ba05:	74 5f                	je     0x43ba66
  43ba07:	8b 06                	mov    eax,DWORD PTR [esi]
  43ba09:	8d 14 40             	lea    edx,[eax+eax*2]
  43ba0c:	8d 14 d2             	lea    edx,[edx+edx*8]
  43ba0f:	d1 e2                	shl    edx,1
  43ba11:	2b d0                	sub    edx,eax
  43ba13:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43ba19:	c1 e2 04             	shl    edx,0x4
  43ba1c:	8d 3c 02             	lea    edi,[edx+eax*1]
  43ba1f:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43ba24:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  43ba27:	0f af d3             	imul   edx,ebx
  43ba2a:	f7 ea                	imul   edx
  43ba2c:	c1 fa 05             	sar    edx,0x5
  43ba2f:	8b c2                	mov    eax,edx
  43ba31:	c1 e8 1f             	shr    eax,0x1f
  43ba34:	03 d0                	add    edx,eax
  43ba36:	85 d2                	test   edx,edx
  43ba38:	7f 05                	jg     0x43ba3f
  43ba3a:	ba 01 00 00 00       	mov    edx,0x1
  43ba3f:	89 57 30             	mov    DWORD PTR [edi+0x30],edx
  43ba42:	8b 06                	mov    eax,DWORD PTR [esi]
  43ba44:	8d 14 40             	lea    edx,[eax+eax*2]
  43ba47:	8d 14 d2             	lea    edx,[edx+edx*8]
  43ba4a:	d1 e2                	shl    edx,1
  43ba4c:	2b d0                	sub    edx,eax
  43ba4e:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43ba54:	c1 e2 04             	shl    edx,0x4
  43ba57:	03 c2                	add    eax,edx
  43ba59:	8b 50 2c             	mov    edx,DWORD PTR [eax+0x2c]
  43ba5c:	8b 78 30             	mov    edi,DWORD PTR [eax+0x30]
  43ba5f:	3b fa                	cmp    edi,edx
  43ba61:	7e 03                	jle    0x43ba66
  43ba63:	89 50 30             	mov    DWORD PTR [eax+0x30],edx
  43ba66:	85 ed                	test   ebp,ebp
  43ba68:	74 60                	je     0x43baca
  43ba6a:	8b 06                	mov    eax,DWORD PTR [esi]
  43ba6c:	8d 14 40             	lea    edx,[eax+eax*2]
  43ba6f:	8d 14 d2             	lea    edx,[edx+edx*8]
  43ba72:	d1 e2                	shl    edx,1
  43ba74:	2b d0                	sub    edx,eax
  43ba76:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43ba7c:	c1 e2 04             	shl    edx,0x4
  43ba7f:	8d 3c 02             	lea    edi,[edx+eax*1]
  43ba82:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43ba87:	8b 57 34             	mov    edx,DWORD PTR [edi+0x34]
  43ba8a:	0f af d5             	imul   edx,ebp
  43ba8d:	f7 ea                	imul   edx
  43ba8f:	c1 fa 05             	sar    edx,0x5
  43ba92:	8b c2                	mov    eax,edx
  43ba94:	c1 e8 1f             	shr    eax,0x1f
  43ba97:	03 d0                	add    edx,eax
  43ba99:	33 c0                	xor    eax,eax
  43ba9b:	85 d2                	test   edx,edx
  43ba9d:	0f 9e c0             	setle  al
  43baa0:	48                   	dec    eax
  43baa1:	23 c2                	and    eax,edx
  43baa3:	89 47 38             	mov    DWORD PTR [edi+0x38],eax
  43baa6:	8b 06                	mov    eax,DWORD PTR [esi]
  43baa8:	8d 14 40             	lea    edx,[eax+eax*2]
  43baab:	8d 14 d2             	lea    edx,[edx+edx*8]
  43baae:	d1 e2                	shl    edx,1
  43bab0:	2b d0                	sub    edx,eax
  43bab2:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43bab8:	c1 e2 04             	shl    edx,0x4
  43babb:	03 c2                	add    eax,edx
  43babd:	8b 50 34             	mov    edx,DWORD PTR [eax+0x34]
  43bac0:	8b 78 38             	mov    edi,DWORD PTR [eax+0x38]
  43bac3:	3b fa                	cmp    edi,edx
  43bac5:	7e 03                	jle    0x43baca
  43bac7:	89 50 38             	mov    DWORD PTR [eax+0x38],edx
  43baca:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  43bace:	8b 51 3c             	mov    edx,DWORD PTR [ecx+0x3c]
  43bad1:	40                   	inc    eax
  43bad2:	83 c6 04             	add    esi,0x4
  43bad5:	3b c2                	cmp    eax,edx
  43bad7:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  43badb:	0f 8c 22 ff ff ff    	jl     0x43ba03
