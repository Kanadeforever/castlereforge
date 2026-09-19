
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443d90 <.text+0x42d90>:
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
  443f1f:	8d 87 54 02 00 00    	lea    eax,[edi+0x254]
  443f25:	50                   	push   eax
  443f26:	8b 04 b2             	mov    eax,DWORD PTR [edx+esi*4]
  443f29:	50                   	push   eax
  443f2a:	56                   	push   esi
  443f2b:	51                   	push   ecx
  443f2c:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  443f30:	8d 9d e4 03 00 00    	lea    ebx,[ebp+0x3e4]
  443f36:	51                   	push   ecx
  443f37:	8b cb                	mov    ecx,ebx
  443f39:	e8 72 b3 fd ff       	call   0x41f2b0
  443f3e:	8b cb                	mov    ecx,ebx
  443f40:	e8 8b f2 fd ff       	call   0x4231d0
  443f45:	6a 00                	push   0x0
  443f47:	6a 00                	push   0x0
  443f49:	6a 67                	push   0x67
  443f4b:	8b cb                	mov    ecx,ebx
  443f4d:	e8 ce e1 fd ff       	call   0x422120
  443f52:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  443f56:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  443f5a:	89 94 b7 a4 03 00 00 	mov    DWORD PTR [edi+esi*4+0x3a4],edx
  443f61:	89 84 b7 3c e5 00 00 	mov    DWORD PTR [edi+esi*4+0xe53c],eax
  443f68:	8b 8d 5c 0c 00 00    	mov    ecx,DWORD PTR [ebp+0xc5c]
  443f6e:	c6 44 04 1c 01       	mov    BYTE PTR [esp+eax*1+0x1c],0x1
  443f73:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  443f77:	89 8d 60 0c 00 00    	mov    DWORD PTR [ebp+0xc60],ecx
  443f7d:	8b 95 88 0f 00 00    	mov    edx,DWORD PTR [ebp+0xf88]
  443f83:	48                   	dec    eax
  443f84:	89 44 24 34          	mov    DWORD PTR [esp+0x34],eax
  443f88:	be 08 00 00 00       	mov    esi,0x8
  443f8d:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  443f90:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  443f94:	c6 81 58 03 00 00 01 	mov    BYTE PTR [ecx+0x358],0x1
  443f9b:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  443f9f:	c6 02 00             	mov    BYTE PTR [edx],0x0
  443fa2:	46                   	inc    esi
  443fa3:	83 fe 08             	cmp    esi,0x8
  443fa6:	0f 8c 43 ff ff ff    	jl     0x443eef
  443fac:	41                   	inc    ecx
  443fad:	83 f9 08             	cmp    ecx,0x8
  443fb0:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  443fb4:	0f 8c 1f ff ff ff    	jl     0x443ed9
  443fba:	5e                   	pop    esi
  443fbb:	5d                   	pop    ebp
  443fbc:	5f                   	pop    edi
  443fbd:	5b                   	pop    ebx
  443fbe:	83 c4 1c             	add    esp,0x1c
  443fc1:	c2               	ret    0x8
