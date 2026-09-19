
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
  443d31:	c2                   	.byte 0xc2
