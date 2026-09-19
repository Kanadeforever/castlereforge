
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00422cb0 <.text+0x21cb0>:
  422cb0:	81 ec a8 00 00 00    	sub    esp,0xa8
  422cb6:	53                   	push   ebx
  422cb7:	55                   	push   ebp
  422cb8:	33 c0                	xor    eax,eax
  422cba:	56                   	push   esi
  422cbb:	33 db                	xor    ebx,ebx
  422cbd:	57                   	push   edi
  422cbe:	8b e9                	mov    ebp,ecx
  422cc0:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  422cc4:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  422cc8:	8d 74 24 18          	lea    esi,[esp+0x18]
  422ccc:	ba 94 fd 89 00       	mov    edx,0x89fd94
  422cd1:	8d 7c 24 68          	lea    edi,[esp+0x68]
  422cd5:	8b 0a                	mov    ecx,DWORD PTR [edx]
  422cd7:	83 b9 2c 07 00 00 00 	cmp    DWORD PTR [ecx+0x72c],0x0
  422cde:	7c 25                	jl     0x422d05
  422ce0:	8b 89 50 08 00 00    	mov    ecx,DWORD PTR [ecx+0x850]
  422ce6:	85 c9                	test   ecx,ecx
  422ce8:	75 10                	jne    0x422cfa
  422cea:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  422cee:	89 07                	mov    DWORD PTR [edi],eax
  422cf0:	41                   	inc    ecx
  422cf1:	83 c7 04             	add    edi,0x4
  422cf4:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  422cf8:	eb 0b                	jmp    0x422d05
  422cfa:	83 f9 01             	cmp    ecx,0x1
  422cfd:	75 06                	jne    0x422d05
  422cff:	89 06                	mov    DWORD PTR [esi],eax
  422d01:	43                   	inc    ebx
  422d02:	83 c6 04             	add    esi,0x4
  422d05:	83 c2 04             	add    edx,0x4
  422d08:	40                   	inc    eax
  422d09:	81 fa d4 fd 89 00    	cmp    edx,0x89fdd4
  422d0f:	7c c4                	jl     0x422cd5
  422d11:	8b 85 f0 0c 00 00    	mov    eax,DWORD PTR [ebp+0xcf0]
  422d17:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  422d1b:	83 f8 07             	cmp    eax,0x7
  422d1e:	74 64                	je     0x422d84
  422d20:	8b 85 48 08 00 00    	mov    eax,DWORD PTR [ebp+0x848]
  422d26:	8b 48 18             	mov    ecx,DWORD PTR [eax+0x18]
  422d29:	85 c9                	test   ecx,ecx
  422d2b:	75 40                	jne    0x422d6d
  422d2d:	8b 8d dc 08 00 00    	mov    ecx,DWORD PTR [ebp+0x8dc]
  422d33:	85 c9                	test   ecx,ecx
  422d35:	7e 36                	jle    0x422d6d
  422d37:	8d 04 cd 00 00 00 00 	lea    eax,[ecx*8+0x0]
  422d3e:	2b c1                	sub    eax,ecx
  422d40:	8d 14 81             	lea    edx,[ecx+eax*4]
  422d43:	a1 b0 01 8a 00       	mov    eax,ds:0x8a01b0
  422d48:	c1 e2 04             	shl    edx,0x4
  422d4b:	2b d1                	sub    edx,ecx
  422d4d:	33 c9                	xor    ecx,ecx
  422d4f:	8d 04 50             	lea    eax,[eax+edx*2]
  422d52:	8b 40 34             	mov    eax,DWORD PTR [eax+0x34]
  422d55:	83 f8 01             	cmp    eax,0x1
  422d58:	0f 94 c3             	sete   bl
  422d5b:	85 c0                	test   eax,eax
  422d5d:	74 0a                	je     0x422d69
  422d5f:	83 f8 01             	cmp    eax,0x1
  422d62:	74 05                	je     0x422d69
  422d64:	8d 70 ff             	lea    esi,[eax-0x1]
  422d67:	eb 4a                	jmp    0x422db3
  422d69:	33 f6                	xor    esi,esi
  422d6b:	eb 46                	jmp    0x422db3
  422d6d:	8b 50 28             	mov    edx,DWORD PTR [eax+0x28]
  422d70:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  422d73:	8b 70 24             	mov    esi,DWORD PTR [eax+0x24]
  422d76:	85 d2                	test   edx,edx
  422d78:	0f 95 c3             	setne  bl
  422d7b:	83 f9 04             	cmp    ecx,0x4
  422d7e:	75 21                	jne    0x422da1
  422d80:	b3 01                	mov    bl,0x1
  422d82:	eb 1d                	jmp    0x422da1
  422d84:	8b 85 f4 0c 00 00    	mov    eax,DWORD PTR [ebp+0xcf4]
  422d8a:	8b 90 c0 00 00 00    	mov    edx,DWORD PTR [eax+0xc0]
  422d90:	8b b0 bc 00 00 00    	mov    esi,DWORD PTR [eax+0xbc]
  422d96:	8b 88 b8 00 00 00    	mov    ecx,DWORD PTR [eax+0xb8]
  422d9c:	85 d2                	test   edx,edx
  422d9e:	0f 95 c3             	setne  bl
  422da1:	83 f9 02             	cmp    ecx,0x2
  422da4:	0f 84 23 01 00 00    	je     0x422ecd
  422daa:	83 f9 04             	cmp    ecx,0x4
  422dad:	0f 84 1a 01 00 00    	je     0x422ecd
  422db3:	83 bd 50 08 00 00 01 	cmp    DWORD PTR [ebp+0x850],0x1
  422dba:	75 10                	jne    0x422dcc
  422dbc:	83 f9 03             	cmp    ecx,0x3
  422dbf:	0f 87 91 00 00 00    	ja     0x422e56
  422dc5:	ff 24 8d dc 2e 42 00 	jmp    DWORD PTR [ecx*4+0x422edc]
  422dcc:	83 f9 03             	cmp    ecx,0x3
  422dcf:	0f 87 81 00 00 00    	ja     0x422e56
  422dd5:	ff 24 8d ec 2e 42 00 	jmp    DWORD PTR [ecx*4+0x422eec]
  422ddc:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  422de0:	85 ff                	test   edi,edi
  422de2:	75 0e                	jne    0x422df2
  422de4:	5f                   	pop    edi
  422de5:	5e                   	pop    esi
  422de6:	5d                   	pop    ebp
  422de7:	83 c8 ff             	or     eax,0xffffffff
  422dea:	5b                   	pop    ebx
  422deb:	81 c4 a8 00 00 00    	add    esp,0xa8
  422df1:	c3                   	ret
  422df2:	e8 6e e7 02 00       	call   0x451565
  422df7:	99                   	cdq
  422df8:	f7 ff                	idiv   edi
  422dfa:	84 db                	test   bl,bl
  422dfc:	8b 7c 94 18          	mov    edi,DWORD PTR [esp+edx*4+0x18]
  422e00:	75 47                	jne    0x422e49
  422e02:	83 fe 02             	cmp    esi,0x2
  422e05:	75 24                	jne    0x422e2b
  422e07:	be 14 00 00 00       	mov    esi,0x14
  422e0c:	56                   	push   esi
  422e0d:	8b cd                	mov    ecx,ebp
  422e0f:	e8 5c fc ff ff       	call   0x422a70
  422e14:	85 c0                	test   eax,eax
  422e16:	7d 2f                	jge    0x422e47
  422e18:	46                   	inc    esi
  422e19:	83 fe 18             	cmp    esi,0x18
  422e1c:	7c ee                	jl     0x422e0c
  422e1e:	8b c7                	mov    eax,edi
  422e20:	5f                   	pop    edi
  422e21:	5e                   	pop    esi
  422e22:	5d                   	pop    ebp
  422e23:	5b                   	pop    ebx
  422e24:	81 c4 a8 00 00 00    	add    esp,0xa8
  422e2a:	c3                   	ret
  422e2b:	8b 0c bd 94 fd 89 00 	mov    ecx,DWORD PTR [edi*4+0x89fd94]
  422e32:	8b 81 9c 0b 00 00    	mov    eax,DWORD PTR [ecx+0xb9c]
  422e38:	8b cd                	mov    ecx,ebp
  422e3a:	83 e8 04             	sub    eax,0x4
  422e3d:	50                   	push   eax
  422e3e:	e8 2d fc ff ff       	call   0x422a70
  422e43:	85 c0                	test   eax,eax
  422e45:	7c 02                	jl     0x422e49
  422e47:	8b f8                	mov    edi,eax
  422e49:	8b c7                	mov    eax,edi
  422e4b:	5f                   	pop    edi
  422e4c:	5e                   	pop    esi
  422e4d:	5d                   	pop    ebp
  422e4e:	5b                   	pop    ebx
  422e4f:	81 c4 a8 00 00 00    	add    esp,0xa8
  422e55:	c3                   	ret
  422e56:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  422e5a:	85 ff                	test   edi,edi
  422e5c:	75 0e                	jne    0x422e6c
  422e5e:	5f                   	pop    edi
  422e5f:	5e                   	pop    esi
  422e60:	5d                   	pop    ebp
  422e61:	83 c8 ff             	or     eax,0xffffffff
  422e64:	5b                   	pop    ebx
  422e65:	81 c4 a8 00 00 00    	add    esp,0xa8
  422e6b:	c3                   	ret
  422e6c:	e8 f4 e6 02 00       	call   0x451565
  422e71:	99                   	cdq
  422e72:	f7 ff                	idiv   edi
  422e74:	84 db                	test   bl,bl
  422e76:	8b 7c 94 68          	mov    edi,DWORD PTR [esp+edx*4+0x68]
  422e7a:	75 44                	jne    0x422ec0
  422e7c:	83 fe 02             	cmp    esi,0x2
  422e7f:	75 21                	jne    0x422ea2
  422e81:	33 f6                	xor    esi,esi
  422e83:	56                   	push   esi
  422e84:	8b cd                	mov    ecx,ebp
  422e86:	e8 e5 fb ff ff       	call   0x422a70
  422e8b:	85 c0                	test   eax,eax
  422e8d:	7d 2f                	jge    0x422ebe
  422e8f:	46                   	inc    esi
  422e90:	83 fe 04             	cmp    esi,0x4
  422e93:	7c ee                	jl     0x422e83
  422e95:	8b c7                	mov    eax,edi
  422e97:	5f                   	pop    edi
  422e98:	5e                   	pop    esi
  422e99:	5d                   	pop    ebp
  422e9a:	5b                   	pop    ebx
  422e9b:	81 c4 a8 00 00 00    	add    esp,0xa8
  422ea1:	c3                   	ret
  422ea2:	8b 14 bd 94 fd 89 00 	mov    edx,DWORD PTR [edi*4+0x89fd94]
  422ea9:	8b cd                	mov    ecx,ebp
  422eab:	8b 82 9c 0b 00 00    	mov    eax,DWORD PTR [edx+0xb9c]
  422eb1:	83 c0 04             	add    eax,0x4
  422eb4:	50                   	push   eax
  422eb5:	e8 b6 fb ff ff       	call   0x422a70
  422eba:	85 c0                	test   eax,eax
  422ebc:	7c 02                	jl     0x422ec0
  422ebe:	8b f8                	mov    edi,eax
  422ec0:	8b c7                	mov    eax,edi
  422ec2:	5f                   	pop    edi
  422ec3:	5e                   	pop    esi
  422ec4:	5d                   	pop    ebp
  422ec5:	5b                   	pop    ebx
  422ec6:	81 c4 a8 00 00 00    	add    esp,0xa8
  422ecc:	c3                   	ret
  422ecd:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  422ed0:	5f                   	pop    edi
  422ed1:	5e                   	pop    esi
  422ed2:	5d                   	pop    ebp
  422ed3:	5b                   	pop    ebx
  422ed4:	81 c4 a8 00 00 00    	add    esp,0xa8
  422eda:	c3                   	ret
  422edb:	90                   	nop
  422edc:	56                   	push   esi
  422edd:	2e 42                	cs inc edx
  422edf:	00 dc                	add    ah,bl
  422ee1:	2d 42 00 56 2e       	sub    eax,0x2e560042
  422ee6:	42                   	inc    edx
  422ee7:	00 dc                	add    ah,bl
  422ee9:	2d 42 00 dc 2d       	sub    eax,0x2ddc0042
  422eee:	42                   	inc    edx
  422eef:	00 56 2e             	add    BYTE PTR [esi+0x2e],dl
  422ef2:	42                   	inc    edx
  422ef3:	00 dc                	add    ah,bl
  422ef5:	2d 42 00 56 2e       	sub    eax,0x2e560042
  422efa:	42                   	inc    edx
  422efb:	00                   	.byte 0
  422efc:	90                   	nop
  422efd:	90                   	nop
  422efe:	90                   	nop
  422eff:	90                   	nop
