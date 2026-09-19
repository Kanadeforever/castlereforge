
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00428cb0 <.text+0x27cb0>:
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
  428e70:	56                   	push   esi
  428e71:	8b f1                	mov    esi,ecx
  428e73:	57                   	push   edi
  428e74:	80 be 2d 01 00 00 01 	cmp    BYTE PTR [esi+0x12d],0x1
  428e7b:	75 4b                	jne    0x428ec8
  428e7d:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  428e80:	8b 7c 24 0c          	mov    edi,DWORD PTR [esp+0xc]
  428e84:	33 c9                	xor    ecx,ecx
  428e86:	66 8b 48 14          	mov    cx,WORD PTR [eax+0x14]
  428e8a:	3b f9                	cmp    edi,ecx
  428e8c:	7d 3a                	jge    0x428ec8
  428e8e:	85 ff                	test   edi,edi
  428e90:	7c 36                	jl     0x428ec8
  428e92:	8b 56 24             	mov    edx,DWORD PTR [esi+0x24]
  428e95:	8b 0c ba             	mov    ecx,DWORD PTR [edx+edi*4]
  428e98:	03 c1                	add    eax,ecx
  428e9a:	89 46 34             	mov    DWORD PTR [esi+0x34],eax
  428e9d:	66 83 78 54 00       	cmp    WORD PTR [eax+0x54],0x0
  428ea2:	75 11                	jne    0x428eb5
  428ea4:	89 7e 38             	mov    DWORD PTR [esi+0x38],edi
  428ea7:	c7 46 40 ff ff ff ff 	mov    DWORD PTR [esi+0x40],0xffffffff
  428eae:	5f                   	pop    edi
  428eaf:	b0 01                	mov    al,0x1
  428eb1:	5e                   	pop    esi
  428eb2:	c2 04 00             	ret    0x4
  428eb5:	6a 00                	push   0x0
  428eb7:	8b ce                	mov    ecx,esi
  428eb9:	e8 12 00 00 00       	call   0x428ed0
  428ebe:	89 7e 38             	mov    DWORD PTR [esi+0x38],edi
  428ec1:	5f                   	pop    edi
  428ec2:	b0 01                	mov    al,0x1
  428ec4:	5e                   	pop    esi
  428ec5:	c2 04 00             	ret    0x4
  428ec8:	5f                   	pop    edi
  428ec9:	32 c0                	xor    al,al
  428ecb:	5e                   	pop    esi
  428ecc:	c2 04 00             	ret    0x4
  428ecf:	90                   	nop
