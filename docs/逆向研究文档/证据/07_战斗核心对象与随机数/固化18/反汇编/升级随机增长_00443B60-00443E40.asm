
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443b60 <.text+0x42b60>:
  443b60:	53                   	push   ebx
  443b61:	55                   	push   ebp
  443b62:	56                   	push   esi
  443b63:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443b67:	8b e9                	mov    ebp,ecx
  443b69:	32 db                	xor    bl,bl
  443b6b:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  443b6e:	8b 0d f8 23 8e 00    	mov    ecx,DWORD PTR ds:0x8e23f8
  443b74:	85 f6                	test   esi,esi
  443b76:	8b 14 81             	mov    edx,DWORD PTR [ecx+eax*4]
  443b79:	89 56 28             	mov    DWORD PTR [esi+0x28],edx
  443b7c:	75 08                	jne    0x443b86
  443b7e:	5e                   	pop    esi
  443b7f:	5d                   	pop    ebp
  443b80:	32 c0                	xor    al,al
  443b82:	5b                   	pop    ebx
  443b83:	c2 08 00             	ret    0x8
  443b86:	8b 4e 30             	mov    ecx,DWORD PTR [esi+0x30]
  443b89:	85 c9                	test   ecx,ecx
  443b8b:	7f 08                	jg     0x443b95
  443b8d:	5e                   	pop    esi
  443b8e:	5d                   	pop    ebp
  443b8f:	32 c0                	xor    al,al
  443b91:	5b                   	pop    ebx
  443b92:	c2 08 00             	ret    0x8
  443b95:	8b 15 f8 23 8e 00    	mov    edx,DWORD PTR ds:0x8e23f8
  443b9b:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  443b9e:	57                   	push   edi
  443b9f:	3b 0c 82             	cmp    ecx,DWORD PTR [edx+eax*4]
  443ba2:	0f 8c 83 01 00 00    	jl     0x443d2b
  443ba8:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  443bac:	40                   	inc    eax
  443bad:	89 46 20             	mov    DWORD PTR [esi+0x20],eax
  443bb0:	8b 06                	mov    eax,DWORD PTR [esi]
  443bb2:	89 07                	mov    DWORD PTR [edi],eax
  443bb4:	e8 ac d9 00 00       	call   0x451565
  443bb9:	99                   	cdq
  443bba:	b9 03 00 00 00       	mov    ecx,0x3
  443bbf:	f7 f9                	idiv   ecx
  443bc1:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  443bc4:	b8 67 66 66 66       	mov    eax,0x66666667
  443bc9:	8b da                	mov    ebx,edx
  443bcb:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  443bce:	2b ca                	sub    ecx,edx
  443bd0:	f7 e9                	imul   ecx
  443bd2:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  443bd5:	c1 fa 02             	sar    edx,0x2
  443bd8:	8b c2                	mov    eax,edx
  443bda:	03 d3                	add    edx,ebx
  443bdc:	c1 e8 1f             	shr    eax,0x1f
  443bdf:	89 4f 14             	mov    DWORD PTR [edi+0x14],ecx
  443be2:	8b 5e 2c             	mov    ebx,DWORD PTR [esi+0x2c]
  443be5:	8d 54 10 04          	lea    edx,[eax+edx*1+0x4]
  443be9:	03 da                	add    ebx,edx
  443beb:	8b c3                	mov    eax,ebx
  443bed:	89 5e 2c             	mov    DWORD PTR [esi+0x2c],ebx
  443bf0:	89 46 30             	mov    DWORD PTR [esi+0x30],eax
  443bf3:	89 47 0c             	mov    DWORD PTR [edi+0xc],eax
  443bf6:	e8 6a d9 00 00       	call   0x451565
  443bfb:	99                   	cdq
  443bfc:	b9 03 00 00 00       	mov    ecx,0x3
  443c01:	6a 00                	push   0x0
  443c03:	f7 f9                	idiv   ecx
  443c05:	8b 4e 44             	mov    ecx,DWORD PTR [esi+0x44]
  443c08:	b8 89 88 88 88       	mov    eax,0x88888889
  443c0d:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
  443c10:	d1 e1                	shl    ecx,1
  443c12:	8b da                	mov    ebx,edx
  443c14:	f7 e9                	imul   ecx
  443c16:	03 d1                	add    edx,ecx
  443c18:	c1 fa 03             	sar    edx,0x3
  443c1b:	8b c2                	mov    eax,edx
  443c1d:	c1 e8 1f             	shr    eax,0x1f
  443c20:	03 d0                	add    edx,eax
  443c22:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  443c25:	2b d0                	sub    edx,eax
  443c27:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  443c2c:	f7 ea                	imul   edx
  443c2e:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  443c31:	c1 fa 03             	sar    edx,0x3
  443c34:	8b ca                	mov    ecx,edx
  443c36:	03 d3                	add    edx,ebx
  443c38:	c1 e9 1f             	shr    ecx,0x1f
  443c3b:	89 47 18             	mov    DWORD PTR [edi+0x18],eax
  443c3e:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  443c41:	8d 54 11 03          	lea    edx,[ecx+edx*1+0x3]
  443c45:	03 c2                	add    eax,edx
  443c47:	89 46 34             	mov    DWORD PTR [esi+0x34],eax
  443c4a:	89 46 38             	mov    DWORD PTR [esi+0x38],eax
  443c4d:	89 47 10             	mov    DWORD PTR [edi+0x10],eax
  443c50:	8b 0e                	mov    ecx,DWORD PTR [esi]
  443c52:	51                   	push   ecx
  443c53:	8b cd                	mov    ecx,ebp
  443c55:	e8 e6 00 00 00       	call   0x443d40
  443c5a:	8b 56 3c             	mov    edx,DWORD PTR [esi+0x3c]
  443c5d:	6a 01                	push   0x1
  443c5f:	89 57 30             	mov    DWORD PTR [edi+0x30],edx
  443c62:	8b 4e 3c             	mov    ecx,DWORD PTR [esi+0x3c]
  443c65:	03 c8                	add    ecx,eax
  443c67:	8b c1                	mov    eax,ecx
  443c69:	89 4e 3c             	mov    DWORD PTR [esi+0x3c],ecx
  443c6c:	89 47 1c             	mov    DWORD PTR [edi+0x1c],eax
  443c6f:	8b 06                	mov    eax,DWORD PTR [esi]
  443c71:	50                   	push   eax
  443c72:	8b cd                	mov    ecx,ebp
  443c74:	e8 c7 00 00 00       	call   0x443d40
  443c79:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  443c7c:	89 4f 34             	mov    DWORD PTR [edi+0x34],ecx
  443c7f:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  443c82:	03 c8                	add    ecx,eax
  443c84:	89 4e 40             	mov    DWORD PTR [esi+0x40],ecx
  443c87:	8b c1                	mov    eax,ecx
  443c89:	6a 02                	push   0x2
  443c8b:	89 47 20             	mov    DWORD PTR [edi+0x20],eax
  443c8e:	8b 16                	mov    edx,DWORD PTR [esi]
  443c90:	52                   	push   edx
  443c91:	8b cd                	mov    ecx,ebp
  443c93:	e8 a8 00 00 00       	call   0x443d40
  443c98:	8b 4e 44             	mov    ecx,DWORD PTR [esi+0x44]
  443c9b:	6a 03                	push   0x3
  443c9d:	89 4f 38             	mov    DWORD PTR [edi+0x38],ecx
  443ca0:	8b 4e 44             	mov    ecx,DWORD PTR [esi+0x44]
  443ca3:	03 c8                	add    ecx,eax
  443ca5:	8b c1                	mov    eax,ecx
  443ca7:	89 4e 44             	mov    DWORD PTR [esi+0x44],ecx
  443caa:	89 47 24             	mov    DWORD PTR [edi+0x24],eax
  443cad:	8b 16                	mov    edx,DWORD PTR [esi]
  443caf:	52                   	push   edx
  443cb0:	8b cd                	mov    ecx,ebp
  443cb2:	e8 89 00 00 00       	call   0x443d40
  443cb7:	8b 4e 48             	mov    ecx,DWORD PTR [esi+0x48]
  443cba:	6a 04                	push   0x4
  443cbc:	89 4f 3c             	mov    DWORD PTR [edi+0x3c],ecx
  443cbf:	8b 56 48             	mov    edx,DWORD PTR [esi+0x48]
  443cc2:	03 d0                	add    edx,eax
  443cc4:	8b cd                	mov    ecx,ebp
  443cc6:	8b c2                	mov    eax,edx
  443cc8:	89 56 48             	mov    DWORD PTR [esi+0x48],edx
  443ccb:	89 47 28             	mov    DWORD PTR [edi+0x28],eax
  443cce:	8b 16                	mov    edx,DWORD PTR [esi]
  443cd0:	52                   	push   edx
  443cd1:	e8 6a 00 00 00       	call   0x443d40
  443cd6:	8b 4e 4c             	mov    ecx,DWORD PTR [esi+0x4c]
  443cd9:	89 4f 40             	mov    DWORD PTR [edi+0x40],ecx
  443cdc:	8b 5e 4c             	mov    ebx,DWORD PTR [esi+0x4c]
  443cdf:	03 d8                	add    ebx,eax
  443ce1:	8b c3                	mov    eax,ebx
  443ce3:	89 5e 4c             	mov    DWORD PTR [esi+0x4c],ebx
  443ce6:	89 47 2c             	mov    DWORD PTR [edi+0x2c],eax
  443ce9:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  443cec:	a1 f8 23 8e 00       	mov    eax,ds:0x8e23f8
  443cf1:	b3 01                	mov    bl,0x1
  443cf3:	8b 0c 90             	mov    ecx,DWORD PTR [eax+edx*4]
  443cf6:	8b 56 50             	mov    edx,DWORD PTR [esi+0x50]
  443cf9:	89 4e 28             	mov    DWORD PTR [esi+0x28],ecx
  443cfc:	8b c1                	mov    eax,ecx
  443cfe:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  443d01:	83 c2 03             	add    edx,0x3
  443d04:	2b c1                	sub    eax,ecx
  443d06:	89 56 50             	mov    DWORD PTR [esi+0x50],edx
  443d09:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  443d0c:	79 07                	jns    0x443d15
  443d0e:	c7 47 08 00 00 00 00 	mov    DWORD PTR [edi+0x8],0x0
  443d15:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  443d18:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  443d1b:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  443d20:	8b 88 f0 05 00 00    	mov    ecx,DWORD PTR [eax+0x5f0]
  443d26:	e8 75 a4 fd ff       	call   0x41e1a0
  443d2b:	5f                   	pop    edi
  443d2c:	5e                   	pop    esi
  443d2d:	8a c3                	mov    al,bl
  443d2f:	5d                   	pop    ebp
  443d30:	5b                   	pop    ebx
  443d31:	c2 08 00             	ret    0x8
  443d34:	90                   	nop
  443d35:	90                   	nop
  443d36:	90                   	nop
  443d37:	90                   	nop
  443d38:	90                   	nop
  443d39:	90                   	nop
  443d3a:	90                   	nop
  443d3b:	90                   	nop
  443d3c:	90                   	nop
  443d3d:	90                   	nop
  443d3e:	90                   	nop
  443d3f:	90                   	nop
  443d40:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  443d44:	85 c0                	test   eax,eax
  443d46:	7e 40                	jle    0x443d88
  443d48:	83 f8 06             	cmp    eax,0x6
  443d4b:	7f 3b                	jg     0x443d88
  443d4d:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  443d51:	85 c9                	test   ecx,ecx
  443d53:	7c 33                	jl     0x443d88
  443d55:	83 f9 04             	cmp    ecx,0x4
  443d58:	7f 2e                	jg     0x443d88
  443d5a:	8d 0c 81             	lea    ecx,[ecx+eax*4]
  443d5d:	56                   	push   esi
  443d5e:	03 c1                	add    eax,ecx
  443d60:	57                   	push   edi
  443d61:	8b 04 85 c4 c1 46 00 	mov    eax,DWORD PTR [eax*4+0x46c1c4]
  443d68:	8b 34 85 c4 c1 46 00 	mov    esi,DWORD PTR [eax*4+0x46c1c4]
  443d6f:	8b 3c 85 b0 c1 46 00 	mov    edi,DWORD PTR [eax*4+0x46c1b0]
  443d76:	46                   	inc    esi
  443d77:	e8 e9 d7 00 00       	call   0x451565
  443d7c:	99                   	cdq
  443d7d:	f7 fe                	idiv   esi
  443d7f:	8b c2                	mov    eax,edx
  443d81:	03 c7                	add    eax,edi
  443d83:	5f                   	pop    edi
  443d84:	5e                   	pop    esi
  443d85:	c2 08 00             	ret    0x8
  443d88:	33 c0                	xor    eax,eax
  443d8a:	c2 08 00             	ret    0x8
  443d8d:	90                   	nop
  443d8e:	90                   	nop
  443d8f:	90                   	nop
  443d90:	83 ec 1c             	sub    esp,0x1c
  443d93:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  443d97:	53                   	push   ebx
  443d98:	33 db                	xor    ebx,ebx
  443d9a:	57                   	push   edi
  443d9b:	3d 61 01 00 00       	cmp    eax,0x161
  443da0:	8b f9                	mov    edi,ecx
  443da2:	89 5c 24 08          	mov    DWORD PTR [esp+0x8],ebx
  443da6:	74 32                	je     0x443dda
  443da8:	3d 69 01 00 00       	cmp    eax,0x169
  443dad:	0f 85 09 02 00 00    	jne    0x443fbc
  443db3:	e8 ad d7 00 00       	call   0x451565
  443db8:	25 03 00 00 80       	and    eax,0x80000003
  443dbd:	79 05                	jns    0x443dc4
  443dbf:	48                   	dec    eax
  443dc0:	83 c8 fc             	or     eax,0xfffffffc
  443dc3:	40                   	inc    eax
  443dc4:	83 f8 03             	cmp    eax,0x3
  443dc7:	77 59                	ja     0x443e22
  443dc9:	ff 24 85 c4 3f 44 00 	jmp    DWORD PTR [eax*4+0x443fc4]
  443dd0:	c7 44 24 08 91 00 00 	mov    DWORD PTR [esp+0x8],0x91
  443dd7:	00 
  443dd8:	eb 48                	jmp    0x443e22
  443dda:	e8 86 d7 00 00       	call   0x451565
  443ddf:	25 01 00 00 80       	and    eax,0x80000001
  443de4:	79 05                	jns    0x443deb
  443de6:	48                   	dec    eax
  443de7:	83 c8 fe             	or     eax,0xfffffffe
  443dea:	40                   	inc    eax
  443deb:	2b c3                	sub    eax,ebx
  443ded:	74 0d                	je     0x443dfc
  443def:	48                   	dec    eax
  443df0:	75 30                	jne    0x443e22
  443df2:	c7 44 24 08 7e 00 00 	mov    DWORD PTR [esp+0x8],0x7e
  443df9:	00 
  443dfa:	eb 26                	jmp    0x443e22
  443dfc:	c7 44 24 08 7d 00 00 	mov    DWORD PTR [esp+0x8],0x7d
  443e03:	00 
  443e04:	eb 1c                	jmp    0x443e22
  443e06:	c7 44 24 08 92 00 00 	mov    DWORD PTR [esp+0x8],0x92
  443e0d:	00 
  443e0e:	eb 12                	jmp    0x443e22
  443e10:	c7 44 24 08 93 00 00 	mov    DWORD PTR [esp+0x8],0x93
  443e17:	00 
  443e18:	eb 08                	jmp    0x443e22
  443e1a:	c7 44 24 08 94 00 00 	mov    DWORD PTR [esp+0x8],0x94
  443e21:	00 
  443e22:	55                   	push   ebp
  443e23:	56                   	push   esi
  443e24:	e8 3c d7 00 00       	call   0x451565
  443e29:	25 01 00 00 80       	and    eax,0x80000001
  443e2e:	79 05                	jns    0x443e35
  443e30:	48                   	dec    eax
  443e31:	83 c8 fe             	or     eax,0xfffffffe
  443e34:	40                   	inc    eax
  443e35:	33 c9                	xor    ecx,ecx
  443e37:	33 d2                	xor    edx,edx
  443e39:	40                   	inc    eax
  443e3a:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  443e3e:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
  443e42:	89 44 24 34          	mov    DWORD PTR [esp+0x34],eax
  443e46:	89 4c 24 28          	mov    DWORD PTR [esp+0x28],ecx
  443e4a:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
  443e4e:	8d b7 60 0c 00 00    	lea    esi,[edi+0xc60]
  443e54:	83 cd ff             	or     ebp,0xffffffff
  443e57:	39 ae b0 fe ff ff    	cmp    DWORD PTR [esi-0x150],ebp
  443e5d:	74 63                	je     0x443ec2
  443e5f:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  443e62:	7e 5e                	jle    0x443ec2
  443e64:	8b 8e 20 03 00 00    	mov    ecx,DWORD PTR [esi+0x320]
  443e6a:	85 c9                	test   ecx,ecx
  443e6c:	7c 59                	jl     0x443ec7
  443e6e:	83 f9 08             	cmp    ecx,0x8
  443e71:	7d 54                	jge    0x443ec7
  443e73:	8b 96 54 02 00 00    	mov    edx,DWORD PTR [esi+0x254]
  443e79:	c6 44 0c 1c 01       	mov    BYTE PTR [esp+ecx*1+0x1c],0x1
  443e7e:	4a                   	dec    edx
  443e7f:	74 33                	je     0x443eb4
  443e81:	4a                   	dec    edx
  443e82:	74 22                	je     0x443ea6
  443e84:	4a                   	dec    edx
  443e85:	75 40                	jne    0x443ec7
  443e87:	8d 51 ff             	lea    edx,[ecx-0x1]
  443e8a:	85 d2                	test   edx,edx
  443e8c:	7c 39                	jl     0x443ec7
  443e8e:	8d 51 fc             	lea    edx,[ecx-0x4]
  443e91:	85 d2                	test   edx,edx
  443e93:	7c 32                	jl     0x443ec7
  443e95:	c6 44 0c 1b 01       	mov    BYTE PTR [esp+ecx*1+0x1b],0x1
  443e9a:	c6 44 0c 18 01       	mov    BYTE PTR [esp+ecx*1+0x18],0x1
  443e9f:	c6 44 0c 17 01       	mov    BYTE PTR [esp+ecx*1+0x17],0x1
  443ea4:	eb 21                	jmp    0x443ec7
  443ea6:	8d 51 fc             	lea    edx,[ecx-0x4]
  443ea9:	85 d2                	test   edx,edx
  443eab:	7c 1a                	jl     0x443ec7
  443ead:	c6 44 0c 18 01       	mov    BYTE PTR [esp+ecx*1+0x18],0x1
  443eb2:	eb 13                	jmp    0x443ec7
  443eb4:	8d 51 ff             	lea    edx,[ecx-0x1]
  443eb7:	85 d2                	test   edx,edx
  443eb9:	7c 0c                	jl     0x443ec7
  443ebb:	c6 44 0c 1b 01       	mov    BYTE PTR [esp+ecx*1+0x1b],0x1
  443ec0:	eb 05                	jmp    0x443ec7
  443ec2:	c6 44 1c 24 01       	mov    BYTE PTR [esp+ebx*1+0x24],0x1
  443ec7:	43                   	inc    ebx
  443ec8:	81 c6 e4 0d 00 00    	add    esi,0xde4
  443ece:	83 fb 08             	cmp    ebx,0x8
  443ed1:	7c 84                	jl     0x443e57
  443ed3:	33 c9                	xor    ecx,ecx
  443ed5:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  443ed9:	8a 54 0c 1c          	mov    dl,BYTE PTR [esp+ecx*1+0x1c]
  443edd:	84 d2                	test   dl,dl
  443edf:	0f 85 c7 00 00 00    	jne    0x443fac
  443ee5:	85 c0                	test   eax,eax
  443ee7:	0f 8e bf 00 00 00    	jle    0x443fac
  443eed:	33 f6                	xor    esi,esi
  443eef:	8a 5c 34 24          	mov    bl,BYTE PTR [esp+esi*1+0x24]
  443ef3:	8d 54 34 24          	lea    edx,[esp+esi*1+0x24]
  443ef7:	84 db                	test   bl,bl
  443ef9:	89 54 24 18          	mov    DWORD PTR [esp+0x18],edx
  443efd:	0f 84 9f 00 00 00    	je     0x443fa2
  443f03:	8d 04 f6             	lea    eax,[esi+esi*8]
  443f06:	8d 04 86             	lea    eax,[esi+eax*4]
  443f09:	8d 14 40             	lea    edx,[eax+eax*2]
  443f0c:	8d 04 d6             	lea    eax,[esi+edx*8]
  443f0f:	8d 97 c4 02 00 00    	lea    edx,[edi+0x2c4]
  443f15:	52                   	push   edx
  443f16:	8b 97 4c 02 00 00    	mov    edx,DWORD PTR [edi+0x24c]
  443f1c:	8d 2c 87             	lea    ebp,[edi+eax*4]
  443f1f:	8d                   	.byte 0x8d
