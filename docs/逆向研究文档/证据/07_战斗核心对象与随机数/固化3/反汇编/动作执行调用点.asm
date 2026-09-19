
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420d80 <.text+0x1fd80>:
  420d80:	be ac 06 00 00       	mov    esi,0x6ac
  420d85:	85 c0                	test   eax,eax
  420d87:	0f 8c 81 00 00 00    	jl     0x420e0e
  420d8d:	50                   	push   eax
  420d8e:	e8 5d ca 00 00       	call   0x42d7f0
  420d93:	c7 84 be ac 06 00 00 	mov    DWORD PTR [esi+edi*4+0x6ac],0xffffffff
  420d9a:	ff ff ff ff 
  420d9e:	5f                   	pop    edi
  420d9f:	5e                   	pop    esi
  420da0:	c2 10 00             	ret    0x10
  420da3:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420da9:	e8 42 c4 01 00       	call   0x43d1f0
  420dae:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  420db2:	8b 3e                	mov    edi,DWORD PTR [esi]
  420db4:	03 c1                	add    eax,ecx
  420db6:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420dbc:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  420dbf:	8b 4a 24             	mov    ecx,DWORD PTR [edx+0x24]
  420dc2:	8b 52 28             	mov    edx,DWORD PTR [edx+0x28]
  420dc5:	81 c1 40 01 00 00    	add    ecx,0x140
  420dcb:	81 c2 04 01 00 00    	add    edx,0x104
  420dd1:	83 ff 08             	cmp    edi,0x8
  420dd4:	7d 06                	jge    0x420ddc
  420dd6:	83 e9 36             	sub    ecx,0x36
  420dd9:	83 ea 1b             	sub    edx,0x1b
  420ddc:	6a 01                	push   0x1
  420dde:	6a 00                	push   0x0
  420de0:	50                   	push   eax
  420de1:	52                   	push   edx
  420de2:	51                   	push   ecx
  420de3:	52                   	push   edx
  420de4:	8b 54 24 2c          	mov    edx,DWORD PTR [esp+0x2c]
  420de8:	51                   	push   ecx
  420de9:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  420def:	52                   	push   edx
  420df0:	e8 db c6 00 00       	call   0x42d4d0
  420df5:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  420df9:	6a 01                	push   0x1
  420dfb:	50                   	push   eax
  420dfc:	89 84 8e ac 06 00 00 	mov    DWORD PTR [esi+ecx*4+0x6ac],eax
  420e03:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  420e09:	e8 d2 c8 00 00       	call   0x42d6e0
  420e0e:	5f                   	pop    edi
  420e0f:	5e                   	pop    esi
  420e10:	c2 10 00             	ret    0x10
  420e13:	90                   	nop
  420e14:	90                   	nop
  420e15:	90                   	nop
  420e16:	90                   	nop
  420e17:	90                   	nop
  420e18:	90                   	nop
  420e19:	90                   	nop
  420e1a:	90                   	nop
  420e1b:	90                   	nop
  420e1c:	90                   	nop
  420e1d:	90                   	nop
  420e1e:	90                   	nop
  420e1f:	90                   	nop
  420e20:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  420e24:	53                   	push   ebx
  420e25:	32 db                	xor    bl,bl
  420e27:	8b 14 85 94 fd 89 00 	mov    edx,DWORD PTR [eax*4+0x89fd94]
  420e2e:	8b 82 9c 0b 00 00    	mov    eax,DWORD PTR [edx+0xb9c]
  420e34:	8a 54 24 0c          	mov    dl,BYTE PTR [esp+0xc]
  420e38:	84 d2                	test   dl,dl
  420e3a:	74 7f                	je     0x420ebb
  420e3c:	83 f8 03             	cmp    eax,0x3
  420e3f:	7f 3d                	jg     0x420e7e
  420e41:	85 c0                	test   eax,eax
  420e43:	7c 39                	jl     0x420e7e
  420e45:	83 c0 04             	add    eax,0x4
  420e48:	50                   	push   eax
  420e49:	e8 22 1c 00 00       	call   0x422a70
  420e4e:	85 c0                	test   eax,eax
  420e50:	7c 69                	jl     0x420ebb
  420e52:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  420e59:	8b 88 2c 07 00 00    	mov    ecx,DWORD PTR [eax+0x72c]
  420e5f:	85 c9                	test   ecx,ecx
  420e61:	7c 58                	jl     0x420ebb
  420e63:	8b 80 a4 0b 00 00    	mov    eax,DWORD PTR [eax+0xba4]
  420e69:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  420e6c:	8a 81 58 03 00 00    	mov    al,BYTE PTR [ecx+0x358]
  420e72:	84 c0                	test   al,al
  420e74:	74 45                	je     0x420ebb
  420e76:	b3 01                	mov    bl,0x1
  420e78:	8a c3                	mov    al,bl
  420e7a:	5b                   	pop    ebx
  420e7b:	c2 08 00             	ret    0x8
  420e7e:	83 f8 18             	cmp    eax,0x18
  420e81:	7c 38                	jl     0x420ebb
  420e83:	83 f8 1b             	cmp    eax,0x1b
  420e86:	7f 33                	jg     0x420ebb
  420e88:	83 c0 fc             	add    eax,0xfffffffc
  420e8b:	50                   	push   eax
  420e8c:	e8 df 1b 00 00       	call   0x422a70
  420e91:	85 c0                	test   eax,eax
  420e93:	7c 26                	jl     0x420ebb
  420e95:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  420e9c:	8b 88 2c 07 00 00    	mov    ecx,DWORD PTR [eax+0x72c]
  420ea2:	85 c9                	test   ecx,ecx
  420ea4:	7c 15                	jl     0x420ebb
  420ea6:	8b 90 a4 0b 00 00    	mov    edx,DWORD PTR [eax+0xba4]
  420eac:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  420eaf:	8a 88 58 03 00 00    	mov    cl,BYTE PTR [eax+0x358]
  420eb5:	84 c9                	test   cl,cl
  420eb7:	74 02                	je     0x420ebb
  420eb9:	b3 01                	mov    bl,0x1
  420ebb:	8a c3                	mov    al,bl
  420ebd:	5b                   	pop    ebx
  420ebe:	c2 08 00             	ret    0x8
  420ec1:	90                   	nop
  420ec2:	90                   	nop
  420ec3:	90                   	nop
  420ec4:	90                   	nop
  420ec5:	90                   	nop
  420ec6:	90                   	nop
  420ec7:	90                   	nop
  420ec8:	90                   	nop
  420ec9:	90                   	nop
  420eca:	90                   	nop
  420ecb:	90                   	nop
  420ecc:	90                   	nop
  420ecd:	90                   	nop
  420ece:	90                   	nop
  420ecf:	90                   	nop
  420ed0:	83 ec 10             	sub    esp,0x10
  420ed3:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  420ed7:	53                   	push   ebx
  420ed8:	55                   	push   ebp
  420ed9:	56                   	push   esi
  420eda:	57                   	push   edi
  420edb:	33 ed                	xor    ebp,ebp
  420edd:	8b f9                	mov    edi,ecx
  420edf:	3b c5                	cmp    eax,ebp
  420ee1:	89 7c 24 14          	mov    DWORD PTR [esp+0x14],edi
  420ee5:	0f 85 a5 00 00 00    	jne    0x420f90
  420eeb:	e8 c0 1b 00 00       	call   0x422ab0
  420ef0:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  420ef6:	c7 87 e4 0c 00 00 bb 	mov    DWORD PTR [edi+0xce4],0xbbb
  420efd:	0b 00 00 
  420f00:	3b c5                	cmp    eax,ebp
  420f02:	0f 8e 7a 07 00 00    	jle    0x421682
  420f08:	33 f6                	xor    esi,esi
  420f0a:	3b c5                	cmp    eax,ebp
  420f0c:	0f 8e 70 07 00 00    	jle    0x421682
  420f12:	8d 87 88 00 00 00    	lea    eax,[edi+0x88]
  420f18:	bb a1 0f 00 00       	mov    ebx,0xfa1
  420f1d:	bd 06 00 00 00       	mov    ebp,0x6
  420f22:	8b 08                	mov    ecx,DWORD PTR [eax]
  420f24:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  420f2b:	85 c9                	test   ecx,ecx
  420f2d:	74 49                	je     0x420f78
  420f2f:	8b 97 d0 0d 00 00    	mov    edx,DWORD PTR [edi+0xdd0]
  420f35:	85 d2                	test   edx,edx
  420f37:	7e 19                	jle    0x420f52
  420f39:	89 99 e8 0c 00 00    	mov    DWORD PTR [ecx+0xce8],ebx
  420f3f:	8b 10                	mov    edx,DWORD PTR [eax]
  420f41:	8b 0c 95 94 fd 89 00 	mov    ecx,DWORD PTR [edx*4+0x89fd94]
  420f48:	8b 17                	mov    edx,DWORD PTR [edi]
  420f4a:	89 91 e0 0d 00 00    	mov    DWORD PTR [ecx+0xde0],edx
  420f50:	eb 26                	jmp    0x420f78
  420f52:	8b 97 48 08 00 00    	mov    edx,DWORD PTR [edi+0x848]
  420f58:	85 d2                	test   edx,edx
  420f5a:	74 1c                	je     0x420f78
  420f5c:	39 6a 18             	cmp    DWORD PTR [edx+0x18],ebp
  420f5f:	74 17                	je     0x420f78
  420f61:	89 99 e8 0c 00 00    	mov    DWORD PTR [ecx+0xce8],ebx
  420f67:	8b 08                	mov    ecx,DWORD PTR [eax]
  420f69:	8b 14 8d 94 fd 89 00 	mov    edx,DWORD PTR [ecx*4+0x89fd94]
  420f70:	8b 0f                	mov    ecx,DWORD PTR [edi]
  420f72:	89 8a e0 0d 00 00    	mov    DWORD PTR [edx+0xde0],ecx
  420f78:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  420f7e:	46                   	inc    esi
  420f7f:	83                   	.byte 0x83
