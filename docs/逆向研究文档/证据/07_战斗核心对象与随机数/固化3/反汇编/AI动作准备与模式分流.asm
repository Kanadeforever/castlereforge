
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442d90 <.text+0x41d90>:
  442d90:	05 fe ff 8b 54       	add    eax,0x548bfffe
  442d95:	24 10                	and    al,0x10
  442d97:	57                   	push   edi
  442d98:	8d 8a b0 e2 00 00    	lea    ecx,[edx+0xe2b0]
  442d9e:	e8 cd 13 fe ff       	call   0x424170
  442da3:	c7 46 fc d2 07 00 00 	mov    DWORD PTR [esi-0x4],0x7d2
  442daa:	8b 6c 24 10          	mov    ebp,DWORD PTR [esp+0x10]
  442dae:	47                   	inc    edi
  442daf:	81 c6 e4 0d 00 00    	add    esi,0xde4
  442db5:	83 ff 10             	cmp    edi,0x10
  442db8:	0f 8c 07 ff ff ff    	jl     0x442cc5
  442dbe:	8b cd                	mov    ecx,ebp
  442dc0:	e8 1b 00 00 00       	call   0x442de0
  442dc5:	83 bd 24 e2 00 00 ff 	cmp    DWORD PTR [ebp+0xe224],0xffffffff
  442dcc:	75 07                	jne    0x442dd5
  442dce:	8b cd                	mov    ecx,ebp
  442dd0:	e8 6b 04 00 00       	call   0x443240
  442dd5:	5f                   	pop    edi
  442dd6:	5e                   	pop    esi
  442dd7:	5d                   	pop    ebp
  442dd8:	5b                   	pop    ebx
  442dd9:	83 c4 10             	add    esp,0x10
  442ddc:	c3                   	ret
  442ddd:	90                   	nop
  442dde:	90                   	nop
  442ddf:	90                   	nop
  442de0:	81 ec e4 00 00 00    	sub    esp,0xe4
  442de6:	53                   	push   ebx
  442de7:	55                   	push   ebp
  442de8:	56                   	push   esi
  442de9:	83 cb ff             	or     ebx,0xffffffff
  442dec:	57                   	push   edi
  442ded:	8b f9                	mov    edi,ecx
  442def:	c6 84 24 ec 00 00 00 	mov    BYTE PTR [esp+0xec],0x0
  442df6:	00 
  442df7:	c7 44 24 18 00 00 00 	mov    DWORD PTR [esp+0x18],0x0
  442dfe:	00 
  442dff:	89 9c 24 e4 00 00 00 	mov    DWORD PTR [esp+0xe4],ebx
  442e06:	89 9c 24 e8 00 00 00 	mov    DWORD PTR [esp+0xe8],ebx
  442e0d:	c7 44 24 10 04 00 00 	mov    DWORD PTR [esp+0x10],0x4
  442e14:	00 
  442e15:	8d 8f b0 e2 00 00    	lea    ecx,[edi+0xe2b0]
  442e1b:	e8 90 13 fe ff       	call   0x4241b0
  442e20:	8b e8                	mov    ebp,eax
  442e22:	85 ed                	test   ebp,ebp
  442e24:	0f 8c 5f 01 00 00    	jl     0x442f89
  442e2a:	83 fd 10             	cmp    ebp,0x10
  442e2d:	7d 26                	jge    0x442e55
  442e2f:	a1 08 24 8e 00       	mov    eax,ds:0x8e2408
  442e34:	85 c0                	test   eax,eax
  442e36:	75 1d                	jne    0x442e55
  442e38:	8d 44 ed 00          	lea    eax,[ebp+ebp*8+0x0]
  442e3c:	8d 44 85 00          	lea    eax,[ebp+eax*4+0x0]
  442e40:	8d 0c 40             	lea    ecx,[eax+eax*2]
  442e43:	8d 54 cd 00          	lea    edx,[ebp+ecx*8+0x0]
  442e47:	83 bc 97 34 0c 00 00 	cmp    DWORD PTR [edi+edx*4+0xc34],0x1
  442e4e:	01 
  442e4f:	0f 84 28 01 00 00    	je     0x442f7d
  442e55:	55                   	push   ebp
  442e56:	8b cf                	mov    ecx,edi
  442e58:	e8 e3 13 00 00       	call   0x444240
  442e5d:	3b c3                	cmp    eax,ebx
  442e5f:	8b d0                	mov    edx,eax
  442e61:	75 07                	jne    0x442e6a
  442e63:	b8 01 00 00 00       	mov    eax,0x1
  442e68:	eb 07                	jmp    0x442e71
  442e6a:	f7 d8                	neg    eax
  442e6c:	1b c0                	sbb    eax,eax
  442e6e:	83 e0 02             	and    eax,0x2
  442e71:	8d 4c ed 00          	lea    ecx,[ebp+ebp*8+0x0]
  442e75:	8d 4c 8d 00          	lea    ecx,[ebp+ecx*4+0x0]
  442e79:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  442e7c:	8d 4c cd 00          	lea    ecx,[ebp+ecx*8+0x0]
  442e80:	8d 34 8f             	lea    esi,[edi+ecx*4]
  442e83:	8b 8c 8f 10 0b 00 00 	mov    ecx,DWORD PTR [edi+ecx*4+0xb10]
  442e8a:	85 c9                	test   ecx,ecx
  442e8c:	7c 87                	jl     0x442e15
  442e8e:	83 f8 03             	cmp    eax,0x3
  442e91:	0f 87 ba 00 00 00    	ja     0x442f51
  442e97:	ff 24 85 28 32 44 00 	jmp    DWORD PTR [eax*4+0x443228]
  442e9e:	81 be c8 10 00 00 bb 	cmp    DWORD PTR [esi+0x10c8],0xbbb
  442ea5:	0b 00 00 
  442ea8:	74 46                	je     0x442ef0
  442eaa:	81 be cc 10 00 00 a0 	cmp    DWORD PTR [esi+0x10cc],0xfa0
  442eb1:	0f 00 00 
  442eb4:	75 3a                	jne    0x442ef0
  442eb6:	52                   	push   edx
  442eb7:	33 d2                	xor    edx,edx
  442eb9:	85 c0                	test   eax,eax
  442ebb:	0f 95 c2             	setne  dl
  442ebe:	c7 86 d4 10 00 00 01 	mov    DWORD PTR [esi+0x10d4],0x1
  442ec5:	00 00 00 
  442ec8:	52                   	push   edx
  442ec9:	8d 8e e4 03 00 00    	lea    ecx,[esi+0x3e4]
  442ecf:	e8 2c 00 fe ff       	call   0x422f00
  442ed4:	6a 02                	push   0x2
  442ed6:	8d 8e 8c 10 00 00    	lea    ecx,[esi+0x108c]
  442edc:	e8 9f 10 fe ff       	call   0x423f80
  442ee1:	c7 86 c4 10 00 00 d4 	mov    DWORD PTR [esi+0x10c4],0x7d4
  442ee8:	07 00 00 
  442eeb:	e9 25 ff ff ff       	jmp    0x442e15
  442ef0:	c6 84 2f 1c e3 00 00 	mov    BYTE PTR [edi+ebp*1+0xe31c],0x1
  442ef7:	01 
  442ef8:	e9 18 ff ff ff       	jmp    0x442e15
  442efd:	81 be c8 10 00 00 bb 	cmp    DWORD PTR [esi+0x10c8],0xbbb
  442f04:	0b 00 00 
  442f07:	74 e7                	je     0x442ef0
  442f09:	81 be cc 10 00 00 a0 	cmp    DWORD PTR [esi+0x10cc],0xfa0
  442f10:	0f 00 00 
  442f13:	75 db                	jne    0x442ef0
  442f15:	6a 00                	push   0x0
  442f17:	c7 86 d4 10 00 00 04 	mov    DWORD PTR [esi+0x10d4],0x4
  442f1e:	00 00 00 
  442f21:	6a 00                	push   0x0
  442f23:	eb a4                	jmp    0x442ec9
  442f25:	c7 86 d4 10 00 00 07 	mov    DWORD PTR [esi+0x10d4],0x7
  442f2c:	00 00 00 
  442f2f:	c7 86 c8 10 00 00 c1 	mov    DWORD PTR [esi+0x10c8],0xbc1
  442f36:	0b 00 00 
  442f39:	a1 08 24 8e 00       	mov    eax,ds:0x8e2408
  442f3e:	85 c0                	test   eax,eax
  442f40:	74 92                	je     0x442ed4
  442f42:	6a 00                	push   0x0
  442f44:	8d 8e e4 03 00 00    	lea    ecx,[esi+0x3e4]
  442f4a:	e8 51 0c fe ff       	call   0x423ba0
  442f4f:	eb 83                	jmp    0x442ed4
  442f51:	81 be c8 10 00 00 bb 	cmp    DWORD PTR [esi+0x10c8],0xbbb
  442f58:	0b 00 00 
  442f5b:	74 93                	je     0x442ef0
  442f5d:	81 be cc 10 00 00 a0 	cmp    DWORD PTR [esi+0x10cc],0xfa0
  442f64:	0f 00 00 
  442f67:	75 87                	jne    0x442ef0
  442f69:	6a 00                	push   0x0
  442f6b:	6a 00                	push   0x0
  442f6d:	8d 8e e4 03 00 00    	lea    ecx,[esi+0x3e4]
  442f73:	e8 88 ff fd ff       	call   0x422f00
  442f78:	e9 98 fe ff ff       	jmp    0x442e15
  442f7d:	55                   	push   ebp
  442f7e:	8d 8f f8 e2 00 00    	lea    ecx,[edi+0xe2f8]
  442f84:	e8 e7 11 fe ff       	call   0x424170
  442f89:	bb e4 1c ff ff       	mov    ebx,0xffff1ce4
  442f8e:	8d af 1c e3 00 00    	lea    ebp,[edi+0xe31c]
  442f94:	8d b7 c8 10 00 00    	lea    esi,[edi+0x10c8]
  442f9a:	2b df                	sub    ebx,edi
  442f9c:	80 7d 00 00          	cmp    BYTE PTR [ebp+0x0],0x0
