
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00433c50 <.text+0x32c50>:
  433c50:	25 00 00 00 00       	and    eax,0x0
  433c55:	51                   	push   ecx
  433c56:	56                   	push   esi
  433c57:	8b f1                	mov    esi,ecx
  433c59:	89 74 24 04          	mov    DWORD PTR [esp+0x4],esi
  433c5d:	e8 0e 97 00 00       	call   0x43d370
  433c62:	68 88 13 00 00       	push   0x1388
  433c67:	33 c0                	xor    eax,eax
  433c69:	68 60 ea 00 00       	push   0xea60
  433c6e:	6a 10                	push   0x10
  433c70:	8b ce                	mov    ecx,esi
  433c72:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  433c76:	c7 06 f4 0d 46 00    	mov    DWORD PTR [esi],0x460df4
  433c7c:	89 86 50 02 00 00    	mov    DWORD PTR [esi+0x250],eax
  433c82:	89 86 9c 03 00 00    	mov    DWORD PTR [esi+0x39c],eax
  433c88:	c7 86 84 03 00 00 fe 	mov    DWORD PTR [esi+0x384],0xfffffffe
  433c8f:	ff ff ff 
  433c92:	89 86 88 03 00 00    	mov    DWORD PTR [esi+0x388],eax
  433c98:	89 86 8c 03 00 00    	mov    DWORD PTR [esi+0x38c],eax
  433c9e:	89 86 90 03 00 00    	mov    DWORD PTR [esi+0x390],eax
  433ca4:	89 86 94 03 00 00    	mov    DWORD PTR [esi+0x394],eax
  433caa:	89 86 98 03 00 00    	mov    DWORD PTR [esi+0x398],eax
  433cb0:	e8 7b 98 00 00       	call   0x43d530
  433cb5:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  433cb9:	8b c6                	mov    eax,esi
  433cbb:	5e                   	pop    esi
  433cbc:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  433cc3:	83 c4 10             	add    esp,0x10
  433cc6:	c3                   	ret
  433cc7:	90                   	nop
  433cc8:	90                   	nop
  433cc9:	90                   	nop
  433cca:	90                   	nop
  433ccb:	90                   	nop
  433ccc:	90                   	nop
  433ccd:	90                   	nop
  433cce:	90                   	nop
  433ccf:	90                   	nop
  433cd0:	6a ff                	push   0xffffffff
  433cd2:	68 48 ee 45 00       	push   0x45ee48
  433cd7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  433cdd:	50                   	push   eax
  433cde:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  433ce5:	51                   	push   ecx
  433ce6:	56                   	push   esi
  433ce7:	8b f1                	mov    esi,ecx
  433ce9:	57                   	push   edi
  433cea:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  433cee:	c7 06 f4 0d 46 00    	mov    DWORD PTR [esi],0x460df4
  433cf4:	8b be 50 02 00 00    	mov    edi,DWORD PTR [esi+0x250]
  433cfa:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  433d01:	00 
  433d02:	85 ff                	test   edi,edi
  433d04:	74 1a                	je     0x433d20
  433d06:	8b cf                	mov    ecx,edi
  433d08:	e8 63 53 fe ff       	call   0x419070
  433d0d:	57                   	push   edi
  433d0e:	e8 3d d8 01 00       	call   0x451550
  433d13:	83 c4 04             	add    esp,0x4
  433d16:	c7 86 50 02 00 00 00 	mov    DWORD PTR [esi+0x250],0x0
  433d1d:	00 00 00 
  433d20:	8b 86 9c 03 00 00    	mov    eax,DWORD PTR [esi+0x39c]
  433d26:	85 c0                	test   eax,eax
  433d28:	74 13                	je     0x433d3d
  433d2a:	50                   	push   eax
  433d2b:	e8 20 d8 01 00       	call   0x451550
  433d30:	83 c4 04             	add    esp,0x4
  433d33:	c7 86 9c 03 00 00 00 	mov    DWORD PTR [esi+0x39c],0x0
  433d3a:	00 00 00 
  433d3d:	8b ce                	mov    ecx,esi
  433d3f:	c7 44 24 14 ff ff ff 	mov    DWORD PTR [esp+0x14],0xffffffff
  433d46:	ff 
  433d47:	e8 74 96 00 00       	call   0x43d3c0
  433d4c:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  433d50:	5f                   	pop    edi
  433d51:	5e                   	pop    esi
  433d52:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  433d59:	83 c4 10             	add    esp,0x10
  433d5c:	c3                   	ret
  433d5d:	90                   	nop
  433d5e:	90                   	nop
  433d5f:	90                   	nop
  433d60:	6a ff                	push   0xffffffff
  433d62:	68 73 ee 45 00       	push   0x45ee73
  433d67:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  433d6d:	50                   	push   eax
  433d6e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  433d75:	83 ec 18             	sub    esp,0x18
  433d78:	53                   	push   ebx
  433d79:	55                   	push   ebp
  433d7a:	56                   	push   esi
  433d7b:	8b f1                	mov    esi,ecx
  433d7d:	57                   	push   edi
  433d7e:	6a 01                	push   0x1
  433d80:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  433d84:	e8 67 d9 00 00       	call   0x4416f0
  433d89:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  433d8e:	33 db                	xor    ebx,ebx
  433d90:	89 5c 24 30          	mov    DWORD PTR [esp+0x30],ebx
  433d94:	8b 68 68             	mov    ebp,DWORD PTR [eax+0x68]
  433d97:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  433d9a:	8b 50 6c             	mov    edx,DWORD PTR [eax+0x6c]
  433d9d:	8b 78 28             	mov    edi,DWORD PTR [eax+0x28]
  433da0:	8b 40 3c             	mov    eax,DWORD PTR [eax+0x3c]
  433da3:	50                   	push   eax
  433da4:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  433da7:	51                   	push   ecx
  433da8:	52                   	push   edx
  433da9:	8d 0c 48             	lea    ecx,[eax+ecx*2]
  433dac:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  433daf:	51                   	push   ecx
  433db0:	8d 0c 50             	lea    ecx,[eax+edx*2]
  433db3:	51                   	push   ecx
  433db4:	55                   	push   ebp
  433db5:	8b ce                	mov    ecx,esi
  433db7:	e8 54 99 00 00       	call   0x43d710
  433dbc:	39 9e 50 02 00 00    	cmp    DWORD PTR [esi+0x250],ebx
  433dc2:	75 2f                	jne    0x433df3
  433dc4:	68 14 01 00 00       	push   0x114
  433dc9:	e8 91 d8 01 00       	call   0x45165f
  433dce:	83 c4 04             	add    esp,0x4
  433dd1:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  433dd5:	3b c3                	cmp    eax,ebx
  433dd7:	c6 44 24 30 01       	mov    BYTE PTR [esp+0x30],0x1
  433ddc:	74 09                	je     0x433de7
  433dde:	8b c8                	mov    ecx,eax
  433de0:	e8 4b 0f fd ff       	call   0x404d30
  433de5:	eb 02                	jmp    0x433de9
  433de7:	33 c0                	xor    eax,eax
  433de9:	88 5c 24 30          	mov    BYTE PTR [esp+0x30],bl
  433ded:	89 86 50 02 00 00    	mov    DWORD PTR [esi+0x250],eax
  433df3:	39 9e 9c 03 00 00    	cmp    DWORD PTR [esi+0x39c],ebx
  433df9:	0f 85 81 00 00 00    	jne    0x433e80
  433dff:	53                   	push   ebx
  433e00:	6a 01                	push   0x1
  433e02:	68 ac ad 46 00       	push   0x46adac
  433e07:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  433e0b:	89 9e a0 03 00 00    	mov    DWORD PTR [esi+0x3a0],ebx
  433e11:	e8 aa d9 00 00       	call   0x4417c0
  433e16:	84 c0                	test   al,al
  433e18:	74 66                	je     0x433e80
  433e1a:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433e1e:	e8 0d 90 ff ff       	call   0x42ce30
  433e23:	8b f8                	mov    edi,eax
  433e25:	b8 5d 41 4c ae       	mov    eax,0xae4c415d
  433e2a:	f7 ef                	imul   edi
  433e2c:	03 d7                	add    edx,edi
  433e2e:	c1 fa 06             	sar    edx,0x6
  433e31:	8b c2                	mov    eax,edx
  433e33:	c1 e8 1f             	shr    eax,0x1f
  433e36:	03 d0                	add    edx,eax
  433e38:	89 96 a0 03 00 00    	mov    DWORD PTR [esi+0x3a0],edx
  433e3e:	8d 04 52             	lea    eax,[edx+edx*2]
  433e41:	c1 e0 04             	shl    eax,0x4
  433e44:	2b c2                	sub    eax,edx
  433e46:	d1 e0                	shl    eax,1
  433e48:	3b f8                	cmp    edi,eax
  433e4a:	75 7a                	jne    0x433ec6
  433e4c:	50                   	push   eax
  433e4d:	e8 0d d8 01 00       	call   0x45165f
  433e52:	83 c4 04             	add    esp,0x4
  433e55:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433e59:	89 86 9c 03 00 00    	mov    DWORD PTR [esi+0x39c],eax
  433e5f:	50                   	push   eax
  433e60:	57                   	push   edi
  433e61:	53                   	push   ebx
  433e62:	e8 c9 db 00 00       	call   0x441a30
  433e67:	8b 8e 9c 03 00 00    	mov    ecx,DWORD PTR [esi+0x39c]
  433e6d:	57                   	push   edi
  433e6e:	51                   	push   ecx
  433e6f:	e8 5c 46 ff ff       	call   0x4284d0
  433e74:	83 c4 08             	add    esp,0x8
  433e77:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433e7b:	e8 80 db 00 00       	call   0x441a00
  433e80:	8b 44 24 38          	mov    eax,DWORD PTR [esp+0x38]
  433e84:	8b 54 24 3c          	mov    edx,DWORD PTR [esp+0x3c]
  433e88:	3b c3                	cmp    eax,ebx
  433e8a:	89 96 84 03 00 00    	mov    DWORD PTR [esi+0x384],edx
  433e90:	6a fe                	push   0xfffffffe
  433e92:	75 6f                	jne    0x433f03
  433e94:	68 a0 ad 46 00       	push   0x46ada0
  433e99:	8b ce                	mov    ecx,esi
  433e9b:	e8 c0 00 00 00       	call   0x433f60
  433ea0:	84 c0                	test   al,al
  433ea2:	74 6b                	je     0x433f0f
  433ea4:	53                   	push   ebx
  433ea5:	8b ce                	mov    ecx,esi
  433ea7:	e8 54 03 00 00       	call   0x434200
  433eac:	68 68 ab 46 00       	push   0x46ab68
  433eb1:	8b ce                	mov    ecx,esi
  433eb3:	e8 c8 01 00 00       	call   0x434080
  433eb8:	68 68 ab 46 00       	push   0x46ab68
  433ebd:	8b ce                	mov    ecx,esi
  433ebf:	e8 0c 02 00 00       	call   0x4340d0
  433ec4:	eb 66                	jmp    0x433f2c
  433ec6:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  433ecc:	68 ac ad 46 00       	push   0x46adac
  433ed1:	68 8c ad 46 00       	push   0x46ad8c
  433ed6:	e8 75 e1 ff ff       	call   0x432050
  433edb:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  433ee1:	68 54 ad 46 00       	push   0x46ad54
  433ee6:	53                   	push   ebx
  433ee7:	e8 84 df ff ff       	call   0x431e70
  433eec:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433ef0:	8a d8                	mov    bl,al
  433ef2:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  433ef9:	ff 
  433efa:	e8 11 d8 00 00       	call   0x441710
  433eff:	8a c3                	mov    al,bl
  433f01:	eb 3c                	jmp    0x433f3f
  433f03:	50                   	push   eax
  433f04:	8b ce                	mov    ecx,esi
  433f06:	e8 55 00 00 00       	call   0x433f60
  433f0b:	84 c0                	test   al,al
  433f0d:	75 15                	jne    0x433f24
  433f0f:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433f13:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  433f1a:	ff 
  433f1b:	e8 f0 d7 00 00       	call   0x441710
  433f20:	32 c0                	xor    al,al
  433f22:	eb 1b                	jmp    0x433f3f
  433f24:	53                   	push   ebx
  433f25:	8b ce                	mov    ecx,esi
  433f27:	e8 d4 02 00 00       	call   0x434200
  433f2c:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  433f30:	c7 44 24 30 ff ff ff 	mov    DWORD PTR [esp+0x30],0xffffffff
  433f37:	ff 
  433f38:	e8 d3 d7 00 00       	call   0x441710
  433f3d:	b0 01                	mov    al,0x1
  433f3f:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  433f43:	5f                   	pop    edi
  433f44:	5e                   	pop    esi
  433f45:	5d                   	pop    ebp
  433f46:	5b                   	pop    ebx
  433f47:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  433f4e:	83 c4 24             	add    esp,0x24
  433f51:	c2 08 00             	ret    0x8
  433f54:	90                   	nop
  433f55:	90                   	nop
  433f56:	90                   	nop
  433f57:	90                   	nop
  433f58:	90                   	nop
  433f59:	90                   	nop
  433f5a:	90                   	nop
  433f5b:	90                   	nop
  433f5c:	90                   	nop
  433f5d:	90                   	nop
  433f5e:	90                   	nop
  433f5f:	90                   	nop
  433f60:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  433f64:	83 ec 64             	sub    esp,0x64
  433f67:	55                   	push   ebp
  433f68:	57                   	push   edi
  433f69:	8b 7c 24 70          	mov    edi,DWORD PTR [esp+0x70]
  433f6d:	8b e9                	mov    ebp,ecx
  433f6f:	57                   	push   edi
  433f70:	89 85 84 03 00 00    	mov    DWORD PTR [ebp+0x384],eax
  433f76:	e8 35 03 00 00       	call   0x4342b0
  433f7b:	8b 8d 4c 02 00 00    	mov    ecx,DWORD PTR [ebp+0x24c]
  433f81:	50                   	push   eax
  433f82:	8b 09                	mov    ecx,DWORD PTR [ecx]
  433f84:	e8 27 92 00 00       	call   0x43d1b0
  433f89:	84 c0                	test   al,al
  433f8b:	0f 84 a9 00 00 00    	je     0x43403a
  433f91:	8b 8d 4c 02 00 00    	mov    ecx,DWORD PTR [ebp+0x24c]
  433f97:	8b 11                	mov    edx,DWORD PTR [ecx]
  433f99:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  433f9c:	8b                   	.byte 0x8b
  433f9d:	80                   	.byte 0x80
  433f9e:	5c                   	pop    esp
  433f9f:	03                   	.byte 0x3
