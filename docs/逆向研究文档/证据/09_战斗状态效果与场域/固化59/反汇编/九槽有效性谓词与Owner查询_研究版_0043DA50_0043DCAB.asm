
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0043da50 <.text+0x3ca50>:
  43da50:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  43da54:	85 d2                	test   edx,edx
  43da56:	7c 4e                	jl     0x43daa6
  43da58:	83 fa 08             	cmp    edx,0x8
  43da5b:	7f 49                	jg     0x43daa6
  43da5d:	8b 44 91 48          	mov    eax,DWORD PTR [ecx+edx*4+0x48]
  43da61:	85 c0                	test   eax,eax
  43da63:	74 41                	je     0x43daa6
  43da65:	56                   	push   esi
  43da66:	8b 74 91 24          	mov    esi,DWORD PTR [ecx+edx*4+0x24]
  43da6a:	32 c0                	xor    al,al
  43da6c:	85 f6                	test   esi,esi
  43da6e:	7d 1a                	jge    0x43da8a
  43da70:	83 3c 91 00          	cmp    DWORD PTR [ecx+edx*4],0x0
  43da74:	7c 06                	jl     0x43da7c
  43da76:	b0 01                	mov    al,0x1
  43da78:	5e                   	pop    esi
  43da79:	c2 04 00             	ret    0x4
  43da7c:	32 c0                	xor    al,al
  43da7e:	c7 44 91 48 00 00 00 	mov    DWORD PTR [ecx+edx*4+0x48],0x0
  43da85:	00 
  43da86:	5e                   	pop    esi
  43da87:	c2 04 00             	ret    0x4
  43da8a:	7e 16                	jle    0x43daa2
  43da8c:	83 3c 91 00          	cmp    DWORD PTR [ecx+edx*4],0x0
  43da90:	7c 0e                	jl     0x43daa0
  43da92:	b0 01                	mov    al,0x1
  43da94:	c7 44 91 48 00 00 00 	mov    DWORD PTR [ecx+edx*4+0x48],0x0
  43da9b:	00 
  43da9c:	5e                   	pop    esi
  43da9d:	c2 04 00             	ret    0x4
  43daa0:	32 c0                	xor    al,al
  43daa2:	5e                   	pop    esi
  43daa3:	c2 04 00             	ret    0x4
  43daa6:	32 c0                	xor    al,al
  43daa8:	c2 04 00             	ret    0x4
  43daab:	90                   	nop
  43daac:	90                   	nop
  43daad:	90                   	nop
  43daae:	90                   	nop
  43daaf:	90                   	nop
  43dab0:	56                   	push   esi
  43dab1:	57                   	push   edi
  43dab2:	8d 71 48             	lea    esi,[ecx+0x48]
  43dab5:	bf 09 00 00 00       	mov    edi,0x9
  43daba:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  43dabd:	74 4f                	je     0x43db0e
  43dabf:	8b 46 b8             	mov    eax,DWORD PTR [esi-0x48]
  43dac2:	8b 4e dc             	mov    ecx,DWORD PTR [esi-0x24]
  43dac5:	03 c1                	add    eax,ecx
  43dac7:	89 46 b8             	mov    DWORD PTR [esi-0x48],eax
  43daca:	8b 4e dc             	mov    ecx,DWORD PTR [esi-0x24]
  43dacd:	85 c9                	test   ecx,ecx
  43dacf:	7d 0a                	jge    0x43dadb
  43dad1:	85 c0                	test   eax,eax
  43dad3:	7f 06                	jg     0x43dadb
  43dad5:	c7 06 00 00 00 00    	mov    DWORD PTR [esi],0x0
  43dadb:	8b 46 dc             	mov    eax,DWORD PTR [esi-0x24]
  43dade:	85 c0                	test   eax,eax
  43dae0:	7e 0d                	jle    0x43daef
  43dae2:	8b 46 b8             	mov    eax,DWORD PTR [esi-0x48]
  43dae5:	85 c0                	test   eax,eax
  43dae7:	7c 06                	jl     0x43daef
  43dae9:	c7 06 00 00 00 00    	mov    DWORD PTR [esi],0x0
  43daef:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  43daf2:	75 1a                	jne    0x43db0e
  43daf4:	8b 46 54             	mov    eax,DWORD PTR [esi+0x54]
  43daf7:	85 c0                	test   eax,eax
  43daf9:	7c 13                	jl     0x43db0e
  43dafb:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  43db01:	50                   	push   eax
  43db02:	e8 e9 fc fe ff       	call   0x42d7f0
  43db07:	c7 46 54 ff ff ff ff 	mov    DWORD PTR [esi+0x54],0xffffffff
  43db0e:	83 c6 04             	add    esi,0x4
  43db11:	4f                   	dec    edi
  43db12:	75 a6                	jne    0x43daba
  43db14:	5f                   	pop    edi
  43db15:	5e                   	pop    esi
  43db16:	c3                   	ret
  43db17:	90                   	nop
  43db18:	90                   	nop
  43db19:	90                   	nop
  43db1a:	90                   	nop
  43db1b:	90                   	nop
  43db1c:	90                   	nop
  43db1d:	90                   	nop
  43db1e:	90                   	nop
  43db1f:	90                   	nop
  43db20:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  43db24:	56                   	push   esi
  43db25:	57                   	push   edi
  43db26:	8b f9                	mov    edi,ecx
  43db28:	83 ce ff             	or     esi,0xffffffff
  43db2b:	b1 01                	mov    cl,0x1
  43db2d:	b8 4c bc 46 00       	mov    eax,0x46bc4c
  43db32:	84 c9                	test   cl,cl
  43db34:	74 13                	je     0x43db49
  43db36:	39 50 fc             	cmp    DWORD PTR [eax-0x4],edx
  43db39:	75 04                	jne    0x43db3f
  43db3b:	8b 30                	mov    esi,DWORD PTR [eax]
  43db3d:	32 c9                	xor    cl,cl
  43db3f:	83 c0 08             	add    eax,0x8
  43db42:	3d 94 bc 46 00       	cmp    eax,0x46bc94
  43db47:	7c e9                	jl     0x43db32
  43db49:	85 f6                	test   esi,esi
  43db4b:	0f 8c ab 00 00 00    	jl     0x43dbfc
  43db51:	83 fe 08             	cmp    esi,0x8
  43db54:	0f 8f a2 00 00 00    	jg     0x43dbfc
  43db5a:	8b 44 b7 48          	mov    eax,DWORD PTR [edi+esi*4+0x48]
  43db5e:	85 c0                	test   eax,eax
  43db60:	0f 85 96 00 00 00    	jne    0x43dbfc
  43db66:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  43db6a:	89 44 b7 78          	mov    DWORD PTR [edi+esi*4+0x78],eax
  43db6e:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  43db72:	c7 44 b7 48 01 00 00 	mov    DWORD PTR [edi+esi*4+0x48],0x1
  43db79:	00 
  43db7a:	89 04 b7             	mov    DWORD PTR [edi+esi*4],eax
  43db7d:	85 c0                	test   eax,eax
  43db7f:	7e 0a                	jle    0x43db8b
  43db81:	c7 44 b7 24 b4 fb ff 	mov    DWORD PTR [edi+esi*4+0x24],0xfffffbb4
  43db88:	ff 
  43db89:	eb 08                	jmp    0x43db93
  43db8b:	c7 44 b7 24 4c 04 00 	mov    DWORD PTR [edi+esi*4+0x24],0x44c
  43db92:	00 
  43db93:	8b 87 c0 01 00 00    	mov    eax,DWORD PTR [edi+0x1c0]
  43db99:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  43db9d:	48                   	dec    eax
  43db9e:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  43dba2:	f7 d8                	neg    eax
  43dba4:	1b c0                	sbb    eax,eax
  43dba6:	6a 00                	push   0x0
  43dba8:	24 ec                	and    al,0xec
  43dbaa:	6a 00                	push   0x0
  43dbac:	83 c0 1b             	add    eax,0x1b
  43dbaf:	68 70 17 00 00       	push   0x1770
  43dbb4:	8b 0c 81             	mov    ecx,DWORD PTR [ecx+eax*4]
  43dbb7:	8b 04 82             	mov    eax,DWORD PTR [edx+eax*4]
  43dbba:	50                   	push   eax
  43dbbb:	51                   	push   ecx
  43dbbc:	50                   	push   eax
  43dbbd:	8d 04 b6             	lea    eax,[esi+esi*4]
  43dbc0:	51                   	push   ecx
  43dbc1:	8d 0c 85 94 bb 46 00 	lea    ecx,[eax*4+0x46bb94]
  43dbc8:	51                   	push   ecx
  43dbc9:	8b cf                	mov    ecx,edi
  43dbcb:	e8 e0 00 00 00       	call   0x43dcb0
  43dbd0:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  43dbd6:	50                   	push   eax
  43dbd7:	e8 f4 f8 fe ff       	call   0x42d4d0
  43dbdc:	85 c0                	test   eax,eax
  43dbde:	89 84 b7 9c 00 00 00 	mov    DWORD PTR [edi+esi*4+0x9c],eax
  43dbe5:	7c 0e                	jl     0x43dbf5
  43dbe7:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  43dbed:	6a 01                	push   0x1
  43dbef:	50                   	push   eax
  43dbf0:	e8 eb fa fe ff       	call   0x42d6e0
  43dbf5:	5f                   	pop    edi
  43dbf6:	5e                   	pop    esi
  43dbf7:	b0 01                	mov    al,0x1
  43dbf9:	c2 14 00             	ret    0x14
  43dbfc:	5f                   	pop    edi
  43dbfd:	5e                   	pop    esi
  43dbfe:	32 c0                	xor    al,al
  43dc00:	c2 14 00             	ret    0x14
  43dc03:	90                   	nop
  43dc04:	90                   	nop
  43dc05:	90                   	nop
  43dc06:	90                   	nop
  43dc07:	90                   	nop
  43dc08:	90                   	nop
  43dc09:	90                   	nop
  43dc0a:	90                   	nop
  43dc0b:	90                   	nop
  43dc0c:	90                   	nop
  43dc0d:	90                   	nop
  43dc0e:	90                   	nop
  43dc0f:	90                   	nop
  43dc10:	56                   	push   esi
  43dc11:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  43dc15:	83 c8 ff             	or     eax,0xffffffff
  43dc18:	33 d2                	xor    edx,edx
  43dc1a:	57                   	push   edi
  43dc1b:	83 c1 78             	add    ecx,0x78
  43dc1e:	83 f8 ff             	cmp    eax,0xffffffff
  43dc21:	75 16                	jne    0x43dc39
  43dc23:	8b 79 d0             	mov    edi,DWORD PTR [ecx-0x30]
  43dc26:	85 ff                	test   edi,edi
  43dc28:	74 06                	je     0x43dc30
  43dc2a:	39 31                	cmp    DWORD PTR [ecx],esi
  43dc2c:	75 02                	jne    0x43dc30
  43dc2e:	8b c2                	mov    eax,edx
  43dc30:	42                   	inc    edx
  43dc31:	83 c1 04             	add    ecx,0x4
  43dc34:	83 fa 09             	cmp    edx,0x9
  43dc37:	7c e5                	jl     0x43dc1e
  43dc39:	5f                   	pop    edi
  43dc3a:	5e                   	pop    esi
  43dc3b:	c2 04 00             	ret    0x4
  43dc3e:	90                   	nop
  43dc3f:	90                   	nop
  43dc40:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  43dc44:	8b 54 81 48          	mov    edx,DWORD PTR [ecx+eax*4+0x48]
  43dc48:	85 d2                	test   edx,edx
  43dc4a:	74 07                	je     0x43dc53
  43dc4c:	8b 44 81 78          	mov    eax,DWORD PTR [ecx+eax*4+0x78]
  43dc50:	c2 04 00             	ret    0x4
  43dc53:	83 c8 ff             	or     eax,0xffffffff
  43dc56:	c2 04 00             	ret    0x4
  43dc59:	90                   	nop
  43dc5a:	90                   	nop
  43dc5b:	90                   	nop
  43dc5c:	90                   	nop
  43dc5d:	90                   	nop
  43dc5e:	90                   	nop
  43dc5f:	90                   	nop
  43dc60:	53                   	push   ebx
  43dc61:	56                   	push   esi
  43dc62:	57                   	push   edi
  43dc63:	8d 71 78             	lea    esi,[ecx+0x78]
  43dc66:	bb 09 00 00 00       	mov    ebx,0x9
  43dc6b:	33 ff                	xor    edi,edi
  43dc6d:	39 7e d0             	cmp    DWORD PTR [esi-0x30],edi
  43dc70:	74 30                	je     0x43dca2
  43dc72:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  43dc76:	8b 0e                	mov    ecx,DWORD PTR [esi]
  43dc78:	3b c8                	cmp    ecx,eax
  43dc7a:	75 26                	jne    0x43dca2
  43dc7c:	89 7e d0             	mov    DWORD PTR [esi-0x30],edi
  43dc7f:	c7 06 ff ff ff ff    	mov    DWORD PTR [esi],0xffffffff
  43dc85:	89 7e 88             	mov    DWORD PTR [esi-0x78],edi
  43dc88:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  43dc8b:	3b c7                	cmp    eax,edi
  43dc8d:	7c 13                	jl     0x43dca2
  43dc8f:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  43dc95:	50                   	push   eax
  43dc96:	e8 55 fb fe ff       	call   0x42d7f0
  43dc9b:	c7 46 24 ff ff ff ff 	mov    DWORD PTR [esi+0x24],0xffffffff
  43dca2:	83 c6 04             	add    esi,0x4
  43dca5:	4b                   	dec    ebx
  43dca6:	75 c5                	jne    0x43dc6d
  43dca8:	5f                   	pop    edi
  43dca9:	5e                   	pop    esi
  43dcaa:	5b                   	pop    ebx
