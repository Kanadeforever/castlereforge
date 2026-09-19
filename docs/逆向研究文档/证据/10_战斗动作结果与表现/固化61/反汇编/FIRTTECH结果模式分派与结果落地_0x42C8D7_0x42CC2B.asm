
/mnt/data/solid61_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

0042c8d7 <.text+0x2b8d7>:
  42c8d7:	8b 47 44             	mov    eax,DWORD PTR [edi+0x44]
  42c8da:	85 c0                	test   eax,eax
  42c8dc:	75 1b                	jne    0x42c8f9
  42c8de:	84 c9                	test   cl,cl
  42c8e0:	74 17                	je     0x42c8f9
  42c8e2:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42c8e6:	57                   	push   edi
  42c8e7:	52                   	push   edx
  42c8e8:	53                   	push   ebx
  42c8e9:	8b cd                	mov    ecx,ebp
  42c8eb:	e8 a0 f6 ff ff       	call   0x42bf90
  42c8f0:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42c8f4:	e9 da 00 00 00       	jmp    0x42c9d3
  42c8f9:	83 f8 01             	cmp    eax,0x1
  42c8fc:	75 1b                	jne    0x42c919
  42c8fe:	84 c9                	test   cl,cl
  42c900:	74 17                	je     0x42c919
  42c902:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42c906:	57                   	push   edi
  42c907:	50                   	push   eax
  42c908:	53                   	push   ebx
  42c909:	8b cd                	mov    ecx,ebp
  42c90b:	e8 50 f9 ff ff       	call   0x42c260
  42c910:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42c914:	e9 ba 00 00 00       	jmp    0x42c9d3
  42c919:	83 f8 02             	cmp    eax,0x2
  42c91c:	75 3e                	jne    0x42c95c
  42c91e:	8b 47 54             	mov    eax,DWORD PTR [edi+0x54]
  42c921:	85 c0                	test   eax,eax
  42c923:	7e 2e                	jle    0x42c953
  42c925:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42c929:	6a 01                	push   0x1
  42c92b:	57                   	push   edi
  42c92c:	51                   	push   ecx
  42c92d:	53                   	push   ebx
  42c92e:	8b cd                	mov    ecx,ebp
  42c930:	e8 2b fb ff ff       	call   0x42c460
  42c935:	8b 4f 54             	mov    ecx,DWORD PTR [edi+0x54]
  42c938:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42c93c:	0f af c8             	imul   ecx,eax
  42c93f:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42c944:	f7 e9                	imul   ecx
  42c946:	c1 fa 05             	sar    edx,0x5
  42c949:	8b c2                	mov    eax,edx
  42c94b:	c1 e8 1f             	shr    eax,0x1f
  42c94e:	03 d0                	add    edx,eax
  42c950:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  42c953:	8b 47 58             	mov    eax,DWORD PTR [edi+0x58]
  42c956:	85 c0                	test   eax,eax
  42c958:	7e 71                	jle    0x42c9cb
  42c95a:	eb 41                	jmp    0x42c99d
  42c95c:	83 f8 03             	cmp    eax,0x3
  42c95f:	75 72                	jne    0x42c9d3
  42c961:	8b 47 54             	mov    eax,DWORD PTR [edi+0x54]
  42c964:	85 c0                	test   eax,eax
  42c966:	7e 2e                	jle    0x42c996
  42c968:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42c96c:	6a 01                	push   0x1
  42c96e:	57                   	push   edi
  42c96f:	51                   	push   ecx
  42c970:	53                   	push   ebx
  42c971:	8b cd                	mov    ecx,ebp
  42c973:	e8 e8 fa ff ff       	call   0x42c460
  42c978:	8b 4f 54             	mov    ecx,DWORD PTR [edi+0x54]
  42c97b:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42c97f:	0f af c8             	imul   ecx,eax
  42c982:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42c987:	f7 e9                	imul   ecx
  42c989:	c1 fa 05             	sar    edx,0x5
  42c98c:	8b c2                	mov    eax,edx
  42c98e:	c1 e8 1f             	shr    eax,0x1f
  42c991:	03 d0                	add    edx,eax
  42c993:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  42c996:	8b 47 58             	mov    eax,DWORD PTR [edi+0x58]
  42c999:	85 c0                	test   eax,eax
  42c99b:	7e 2e                	jle    0x42c9cb
  42c99d:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42c9a1:	6a 00                	push   0x0
  42c9a3:	57                   	push   edi
  42c9a4:	51                   	push   ecx
  42c9a5:	53                   	push   ebx
  42c9a6:	8b cd                	mov    ecx,ebp
  42c9a8:	e8 b3 fa ff ff       	call   0x42c460
  42c9ad:	8b 4f 54             	mov    ecx,DWORD PTR [edi+0x54]
  42c9b0:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42c9b4:	0f af c8             	imul   ecx,eax
  42c9b7:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42c9bc:	f7 e9                	imul   ecx
  42c9be:	c1 fa 05             	sar    edx,0x5
  42c9c1:	8b c2                	mov    eax,edx
  42c9c3:	c1 e8 1f             	shr    eax,0x1f
  42c9c6:	03 d0                	add    edx,eax
  42c9c8:	89 56 08             	mov    DWORD PTR [esi+0x8],edx
  42c9cb:	c7 44 24 20 00 00 00 	mov    DWORD PTR [esp+0x20],0x0
  42c9d2:	00 
  42c9d3:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  42c9d7:	83 f8 02             	cmp    eax,0x2
  42c9da:	7c 0d                	jl     0x42c9e9
  42c9dc:	83 f8 06             	cmp    eax,0x6
  42c9df:	7f 08                	jg     0x42c9e9
  42c9e1:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  42c9e8:	00 
  42c9e9:	8b 47 44             	mov    eax,DWORD PTR [edi+0x44]
  42c9ec:	83 f8 02             	cmp    eax,0x2
  42c9ef:	74 16                	je     0x42ca07
  42c9f1:	83 f8 03             	cmp    eax,0x3
  42c9f4:	74 11                	je     0x42ca07
  42c9f6:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42c9fa:	51                   	push   ecx
  42c9fb:	53                   	push   ebx
  42c9fc:	8b cd                	mov    ecx,ebp
  42c9fe:	e8 cd fa ff ff       	call   0x42c4d0
  42ca03:	01 44 24 14          	add    DWORD PTR [esp+0x14],eax
  42ca07:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42ca0b:	6a 02                	push   0x2
  42ca0d:	8b cd                	mov    ecx,ebp
  42ca0f:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  42ca12:	50                   	push   eax
  42ca13:	e8 28 f5 ff ff       	call   0x42bf40
  42ca18:	84 c0                	test   al,al
  42ca1a:	74 16                	je     0x42ca32
  42ca1c:	83 7f 18 02          	cmp    DWORD PTR [edi+0x18],0x2
  42ca20:	75 10                	jne    0x42ca32
  42ca22:	33 c0                	xor    eax,eax
  42ca24:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  42ca28:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42ca2c:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  42ca2f:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  42ca32:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42ca36:	6a 06                	push   0x6
  42ca38:	8b 51 04             	mov    edx,DWORD PTR [ecx+0x4]
  42ca3b:	8b cd                	mov    ecx,ebp
  42ca3d:	52                   	push   edx
  42ca3e:	e8 fd f4 ff ff       	call   0x42bf40
  42ca43:	84 c0                	test   al,al
  42ca45:	74 1a                	je     0x42ca61
  42ca47:	8b 4f 18             	mov    ecx,DWORD PTR [edi+0x18]
  42ca4a:	33 c0                	xor    eax,eax
  42ca4c:	3b c8                	cmp    ecx,eax
  42ca4e:	74 05                	je     0x42ca55
  42ca50:	83 f9 01             	cmp    ecx,0x1
  42ca53:	75 0c                	jne    0x42ca61
  42ca55:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42ca59:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  42ca5c:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  42ca5f:	eb 04                	jmp    0x42ca65
  42ca61:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  42ca65:	8a 54 24 30          	mov    dl,BYTE PTR [esp+0x30]
  42ca69:	b9 0a 00 00 00       	mov    ecx,0xa
  42ca6e:	84 d2                	test   dl,dl
  42ca70:	74 0a                	je     0x42ca7c
  42ca72:	39 4f 3c             	cmp    DWORD PTR [edi+0x3c],ecx
  42ca75:	74 0a                	je     0x42ca81
  42ca77:	99                   	cdq
  42ca78:	2b c2                	sub    eax,edx
  42ca7a:	d1 f8                	sar    eax,1
  42ca7c:	39 4f 3c             	cmp    DWORD PTR [edi+0x3c],ecx
  42ca7f:	75 02                	jne    0x42ca83
  42ca81:	33 c0                	xor    eax,eax
  42ca83:	8b 8b 24 02 00 00    	mov    ecx,DWORD PTR [ebx+0x224]
  42ca89:	85 c9                	test   ecx,ecx
  42ca8b:	7e 02                	jle    0x42ca8f
  42ca8d:	03 c0                	add    eax,eax
  42ca8f:	8b 8b 10 02 00 00    	mov    ecx,DWORD PTR [ebx+0x210]
  42ca95:	85 c9                	test   ecx,ecx
  42ca97:	7e 05                	jle    0x42ca9e
  42ca99:	99                   	cdq
  42ca9a:	2b c2                	sub    eax,edx
  42ca9c:	d1 f8                	sar    eax,1
  42ca9e:	8a 4c 24 13          	mov    cl,BYTE PTR [esp+0x13]
  42caa2:	84 c9                	test   cl,cl
  42caa4:	74 02                	je     0x42caa8
  42caa6:	03 c0                	add    eax,eax
  42caa8:	8b 4f 44             	mov    ecx,DWORD PTR [edi+0x44]
  42caab:	83 f9 02             	cmp    ecx,0x2
  42caae:	0f 84 bd 00 00 00    	je     0x42cb71
  42cab4:	83 f9 03             	cmp    ecx,0x3
  42cab7:	0f 84 b4 00 00 00    	je     0x42cb71
  42cabd:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  42cac1:	03 c8                	add    ecx,eax
  42cac3:	8b 47 18             	mov    eax,DWORD PTR [edi+0x18]
  42cac6:	83 f8 02             	cmp    eax,0x2
  42cac9:	89 4c 24 30          	mov    DWORD PTR [esp+0x30],ecx
  42cacd:	75 2a                	jne    0x42caf9
  42cacf:	53                   	push   ebx
  42cad0:	8b cd                	mov    ecx,ebp
  42cad2:	e8 39 fb ff ff       	call   0x42c610
  42cad7:	8b c8                	mov    ecx,eax
  42cad9:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42cade:	0f af 4c 24 30       	imul   ecx,DWORD PTR [esp+0x30]
  42cae3:	f7 e9                	imul   ecx
  42cae5:	c1 fa 05             	sar    edx,0x5
  42cae8:	8b c2                	mov    eax,edx
  42caea:	c1 e8 1f             	shr    eax,0x1f
  42caed:	03 d0                	add    edx,eax
  42caef:	8b 44 24 30          	mov    eax,DWORD PTR [esp+0x30]
  42caf3:	03 c2                	add    eax,edx
  42caf5:	89 44 24 30          	mov    DWORD PTR [esp+0x30],eax
  42caf9:	8b 47 54             	mov    eax,DWORD PTR [edi+0x54]
  42cafc:	8b 4f 58             	mov    ecx,DWORD PTR [edi+0x58]
  42caff:	85 c0                	test   eax,eax
  42cb01:	7e 1e                	jle    0x42cb21
  42cb03:	0f af 44 24 30       	imul   eax,DWORD PTR [esp+0x30]
  42cb08:	8b d0                	mov    edx,eax
  42cb0a:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42cb0f:	f7 ea                	imul   edx
  42cb11:	c1 fa 05             	sar    edx,0x5
  42cb14:	8b c2                	mov    eax,edx
  42cb16:	c1 e8 1f             	shr    eax,0x1f
  42cb19:	03 d0                	add    edx,eax
  42cb1b:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
  42cb1f:	eb 08                	jmp    0x42cb29
  42cb21:	c7 44 24 20 00 00 00 	mov    DWORD PTR [esp+0x20],0x0
  42cb28:	00 
  42cb29:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  42cb2d:	85 c9                	test   ecx,ecx
  42cb2f:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  42cb32:	7e 18                	jle    0x42cb4c
  42cb34:	0f af 4c 24 30       	imul   ecx,DWORD PTR [esp+0x30]
  42cb39:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42cb3e:	f7 e9                	imul   ecx
  42cb40:	c1 fa 05             	sar    edx,0x5
  42cb43:	8b c2                	mov    eax,edx
  42cb45:	c1 e8 1f             	shr    eax,0x1f
  42cb48:	03 d0                	add    edx,eax
  42cb4a:	eb 02                	jmp    0x42cb4e
  42cb4c:	33 d2                	xor    edx,edx
  42cb4e:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42cb52:	89 56 08             	mov    DWORD PTR [esi+0x8],edx
  42cb55:	8b 41 30             	mov    eax,DWORD PTR [ecx+0x30]
  42cb58:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  42cb5c:	3b c8                	cmp    ecx,eax
  42cb5e:	7e 03                	jle    0x42cb63
  42cb60:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  42cb63:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42cb67:	8b 40 38             	mov    eax,DWORD PTR [eax+0x38]
  42cb6a:	3b d0                	cmp    edx,eax
  42cb6c:	7e 03                	jle    0x42cb71
  42cb6e:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  42cb71:	8b 8b 90 00 00 00    	mov    ecx,DWORD PTR [ebx+0x90]
  42cb77:	8b 56 08             	mov    edx,DWORD PTR [esi+0x8]
  42cb7a:	57                   	push   edi
  42cb7b:	51                   	push   ecx
  42cb7c:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  42cb7f:	8d 46 3c             	lea    eax,[esi+0x3c]
  42cb82:	03 d1                	add    edx,ecx
  42cb84:	8b cd                	mov    ecx,ebp
  42cb86:	52                   	push   edx
  42cb87:	50                   	push   eax
  42cb88:	8d 46 38             	lea    eax,[esi+0x38]
  42cb8b:	50                   	push   eax
  42cb8c:	e8 0f f2 ff ff       	call   0x42bda0
  42cb91:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  42cb95:	85 c0                	test   eax,eax
  42cb97:	7e 1b                	jle    0x42cbb4
  42cb99:	83 7f 3c 04          	cmp    DWORD PTR [edi+0x3c],0x4
  42cb9d:	75 15                	jne    0x42cbb4
  42cb9f:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42cba3:	8b 88 28 02 00 00    	mov    ecx,DWORD PTR [eax+0x228]
  42cba9:	85 c9                	test   ecx,ecx
  42cbab:	7e 07                	jle    0x42cbb4
  42cbad:	c7 46 04 ff ff ff ff 	mov    DWORD PTR [esi+0x4],0xffffffff
  42cbb4:	8b 47 3c             	mov    eax,DWORD PTR [edi+0x3c]
  42cbb7:	83 f8 0a             	cmp    eax,0xa
  42cbba:	74 1f                	je     0x42cbdb
  42cbbc:	83 f8 0b             	cmp    eax,0xb
  42cbbf:	74 1a                	je     0x42cbdb
  42cbc1:	8b 47 44             	mov    eax,DWORD PTR [edi+0x44]
  42cbc4:	83 f8 02             	cmp    eax,0x2
  42cbc7:	74 12                	je     0x42cbdb
  42cbc9:	83 f8 03             	cmp    eax,0x3
  42cbcc:	74 0d                	je     0x42cbdb
  42cbce:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42cbd2:	6a 00                	push   0x0
  42cbd4:	56                   	push   esi
  42cbd5:	6a 00                	push   0x0
  42cbd7:	57                   	push   edi
  42cbd8:	51                   	push   ecx
  42cbd9:	eb 0b                	jmp    0x42cbe6
  42cbdb:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42cbdf:	6a 01                	push   0x1
  42cbe1:	56                   	push   esi
  42cbe2:	6a 00                	push   0x0
  42cbe4:	57                   	push   edi
  42cbe5:	52                   	push   edx
  42cbe6:	53                   	push   ebx
  42cbe7:	8b cd                	mov    ecx,ebp
  42cbe9:	e8 e2 e4 ff ff       	call   0x42b0d0
  42cbee:	8b 07                	mov    eax,DWORD PTR [edi]
  42cbf0:	3d 97 01 00 00       	cmp    eax,0x197
  42cbf5:	74 07                	je     0x42cbfe
  42cbf7:	3d d0 01 00 00       	cmp    eax,0x1d0
  42cbfc:	75 0f                	jne    0x42cc0d
  42cbfe:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42cc02:	c6 46 40 01          	mov    BYTE PTR [esi+0x40],0x1
  42cc06:	c7 40 30 01 00 00 00 	mov    DWORD PTR [eax+0x30],0x1
  42cc0d:	8b 47 44             	mov    eax,DWORD PTR [edi+0x44]
  42cc10:	83 f8 02             	cmp    eax,0x2
  42cc13:	74 16                	je     0x42cc2b
  42cc15:	83 f8 03             	cmp    eax,0x3
  42cc18:	74 11                	je     0x42cc2b
  42cc1a:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  42cc1d:	85 c0                	test   eax,eax
  42cc1f:	75 0a                	jne    0x42cc2b
  42cc21:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
  42cc24:	85 c0                	test   eax,eax
  42cc26:	75 03                	jne    0x42cc2b
  42cc28:	c6 06 00             	mov    BYTE PTR [esi],0x0
