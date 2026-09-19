  439d70:	6a ff                	push   0xffffffff
  439d72:	68 96 f3 45 00       	push   0x45f396
  439d77:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  439d7d:	50                   	push   eax
  439d7e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  439d85:	81 ec d0 04 00 00    	sub    esp,0x4d0
  439d8b:	53                   	push   ebx
  439d8c:	55                   	push   ebp
  439d8d:	56                   	push   esi
  439d8e:	57                   	push   edi
  439d8f:	8b f1                	mov    esi,ecx
  439d91:	68 e8 5d 00 00       	push   0x5de8
  439d96:	c6 44 24 17 01       	mov    BYTE PTR [esp+0x17],0x1
  439d9b:	e8 bf 78 01 00       	call   0x45165f
  439da0:	8b f8                	mov    edi,eax
  439da2:	33 db                	xor    ebx,ebx
  439da4:	83 c4 04             	add    esp,0x4
  439da7:	3b fb                	cmp    edi,ebx
  439da9:	74 09                	je     0x439db4
  439dab:	8b cf                	mov    ecx,edi
  439dad:	e8 de f2 ff ff       	call   0x439090
  439db2:	eb 02                	jmp    0x439db6
  439db4:	33 ff                	xor    edi,edi
  439db6:	68 e8 5d 00 00       	push   0x5de8
  439dbb:	89 be 98 00 00 00    	mov    DWORD PTR [esi+0x98],edi
  439dc1:	e8 99 78 01 00       	call   0x45165f
  439dc6:	8b f8                	mov    edi,eax
  439dc8:	83 c4 04             	add    esp,0x4
  439dcb:	3b fb                	cmp    edi,ebx
  439dcd:	74 09                	je     0x439dd8
  439dcf:	8b cf                	mov    ecx,edi
  439dd1:	e8 ba f2 ff ff       	call   0x439090
  439dd6:	eb 02                	jmp    0x439dda
  439dd8:	33 ff                	xor    edi,edi
  439dda:	8b 86 98 00 00 00    	mov    eax,DWORD PTR [esi+0x98]
  439de0:	6a 14                	push   0x14
  439de2:	89 be 9c 00 00 00    	mov    DWORD PTR [esi+0x9c],edi
  439de8:	89 86 90 00 00 00    	mov    DWORD PTR [esi+0x90],eax
  439dee:	89 9e 94 00 00 00    	mov    DWORD PTR [esi+0x94],ebx
  439df4:	e8 66 78 01 00       	call   0x45165f
  439df9:	83 c4 04             	add    esp,0x4
  439dfc:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  439e00:	3b c3                	cmp    eax,ebx
  439e02:	89 9c 24 e8 04 00 00 	mov    DWORD PTR [esp+0x4e8],ebx
  439e09:	74 0d                	je     0x439e18
  439e0b:	6a 01                	push   0x1
  439e0d:	8b c8                	mov    ecx,eax
  439e0f:	e8 dc 78 00 00       	call   0x4416f0
  439e14:	8b f8                	mov    edi,eax
  439e16:	eb 02                	jmp    0x439e1a
  439e18:	33 ff                	xor    edi,edi
  439e1a:	83 cd ff             	or     ebp,0xffffffff
  439e1d:	68 8c 01 00 00       	push   0x18c
  439e22:	89 ac 24 ec 04 00 00 	mov    DWORD PTR [esp+0x4ec],ebp
  439e29:	e8 31 78 01 00       	call   0x45165f
  439e2e:	83 c4 04             	add    esp,0x4
  439e31:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  439e35:	3b c3                	cmp    eax,ebx
  439e37:	c7 84 24 e8 04 00 00 	mov    DWORD PTR [esp+0x4e8],0x1
  439e3e:	01 00 00 00 
  439e42:	74 09                	je     0x439e4d
  439e44:	8b c8                	mov    ecx,eax
  439e46:	e8 e5 fd 00 00       	call   0x449c30
  439e4b:	eb 02                	jmp    0x439e4f
  439e4d:	33 c0                	xor    eax,eax
  439e4f:	53                   	push   ebx
  439e50:	6a 01                	push   0x1
  439e52:	68 2c 9f 46 00       	push   0x469f2c
  439e57:	8b cf                	mov    ecx,edi
  439e59:	89 ac 24 f4 04 00 00 	mov    DWORD PTR [esp+0x4f4],ebp
  439e60:	a3 24 24 8e 00       	mov    ds:0x8e2424,eax
  439e65:	e8 56 79 00 00       	call   0x4417c0
  439e6a:	84 c0                	test   al,al
  439e6c:	74 63                	je     0x439ed1
  439e6e:	8b cf                	mov    ecx,edi
  439e70:	e8 bb 2f ff ff       	call   0x42ce30
  439e75:	8b e8                	mov    ebp,eax
  439e77:	b8 ed 73 48 4d       	mov    eax,0x4d4873ed
  439e7c:	f7 ed                	imul   ebp
  439e7e:	c1 fa 08             	sar    edx,0x8
  439e81:	8b ca                	mov    ecx,edx
  439e83:	c1 e9 1f             	shr    ecx,0x1f
  439e86:	03 d1                	add    edx,ecx
  439e88:	8d 04 52             	lea    eax,[edx+edx*2]
  439e8b:	8d 04 c0             	lea    eax,[eax+eax*8]
  439e8e:	d1 e0                	shl    eax,1
  439e90:	2b c2                	sub    eax,edx
  439e92:	c1 e0 04             	shl    eax,0x4
  439e95:	3b e8                	cmp    ebp,eax
  439e97:	75 38                	jne    0x439ed1
  439e99:	50                   	push   eax
  439e9a:	89 96 a0 00 00 00    	mov    DWORD PTR [esi+0xa0],edx
  439ea0:	e8 ba 77 01 00       	call   0x45165f
  439ea5:	83 c4 04             	add    esp,0x4
  439ea8:	8b cf                	mov    ecx,edi
  439eaa:	89 86 a4 00 00 00    	mov    DWORD PTR [esi+0xa4],eax
  439eb0:	50                   	push   eax
  439eb1:	55                   	push   ebp
  439eb2:	53                   	push   ebx
  439eb3:	e8 78 7b 00 00       	call   0x441a30
  439eb8:	8b 96 a4 00 00 00    	mov    edx,DWORD PTR [esi+0xa4]
  439ebe:	55                   	push   ebp
  439ebf:	52                   	push   edx
  439ec0:	e8 0b e6 fe ff       	call   0x4284d0
  439ec5:	83 c4 08             	add    esp,0x8
  439ec8:	8b cf                	mov    ecx,edi
  439eca:	e8 31 7b 00 00       	call   0x441a00
  439ecf:	eb 05                	jmp    0x439ed6
  439ed1:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  439ed6:	53                   	push   ebx
  439ed7:	6a 01                	push   0x1
  439ed9:	68 f4 ba 46 00       	push   0x46baf4
  439ede:	8b cf                	mov    ecx,edi
  439ee0:	e8 db 78 00 00       	call   0x4417c0
  439ee5:	84 c0                	test   al,al
  439ee7:	74 69                	je     0x439f52
  439ee9:	8b cf                	mov    ecx,edi
  439eeb:	e8 40 2f ff ff       	call   0x42ce30
  439ef0:	8b e8                	mov    ebp,eax
  439ef2:	b8 8d 2f 36 02       	mov    eax,0x2362f8d
  439ef7:	f7 ed                	imul   ebp
  439ef9:	c1 fa 03             	sar    edx,0x3
  439efc:	8b c2                	mov    eax,edx
  439efe:	c1 e8 1f             	shr    eax,0x1f
  439f01:	03 d0                	add    edx,eax
  439f03:	8d 0c d5 00 00 00 00 	lea    ecx,[edx*8+0x0]
  439f0a:	2b ca                	sub    ecx,edx
  439f0c:	8d 04 8a             	lea    eax,[edx+ecx*4]
  439f0f:	c1 e0 04             	shl    eax,0x4
  439f12:	2b c2                	sub    eax,edx
  439f14:	d1 e0                	shl    eax,1
  439f16:	3b e8                	cmp    ebp,eax
  439f18:	75 38                	jne    0x439f52
  439f1a:	50                   	push   eax
  439f1b:	89 96 a8 00 00 00    	mov    DWORD PTR [esi+0xa8],edx
  439f21:	e8 39 77 01 00       	call   0x45165f
  439f26:	83 c4 04             	add    esp,0x4
  439f29:	8b cf                	mov    ecx,edi
  439f2b:	89 86 ac 00 00 00    	mov    DWORD PTR [esi+0xac],eax
  439f31:	50                   	push   eax
  439f32:	55                   	push   ebp
  439f33:	53                   	push   ebx
  439f34:	e8 f7 7a 00 00       	call   0x441a30
  439f39:	8b 96 ac 00 00 00    	mov    edx,DWORD PTR [esi+0xac]
  439f3f:	55                   	push   ebp
  439f40:	52                   	push   edx
  439f41:	e8 8a e5 fe ff       	call   0x4284d0
  439f46:	83 c4 08             	add    esp,0x8
  439f49:	8b cf                	mov    ecx,edi
  439f4b:	e8 b0 7a 00 00       	call   0x441a00
  439f50:	eb 05                	jmp    0x439f57
  439f52:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  439f57:	53                   	push   ebx
  439f58:	6a 01                	push   0x1
  439f5a:	68 e0 ba 46 00       	push   0x46bae0
  439f5f:	8b cf                	mov    ecx,edi
  439f61:	e8 5a 78 00 00       	call   0x4417c0
  439f66:	84 c0                	test   al,al
  439f68:	74 67                	je     0x439fd1
  439f6a:	8b cf                	mov    ecx,edi
  439f6c:	e8 bf 2e ff ff       	call   0x42ce30
  439f71:	8b e8                	mov    ebp,eax
  439f73:	b8 eb a0 0e ea       	mov    eax,0xea0ea0eb
  439f78:	f7 ed                	imul   ebp
  439f7a:	03 d5                	add    edx,ebp
  439f7c:	c1 fa 09             	sar    edx,0x9
  439f7f:	8b c2                	mov    eax,edx
  439f81:	c1 e8 1f             	shr    eax,0x1f
  439f84:	03 d0                	add    edx,eax
  439f86:	8d 04 d5 00 00 00 00 	lea    eax,[edx*8+0x0]
  439f8d:	2b c2                	sub    eax,edx
  439f8f:	8d 04 80             	lea    eax,[eax+eax*4]
  439f92:	c1 e0 04             	shl    eax,0x4
  439f95:	3b e8                	cmp    ebp,eax
  439f97:	75 38                	jne    0x439fd1
  439f99:	50                   	push   eax
  439f9a:	89 96 b0 00 00 00    	mov    DWORD PTR [esi+0xb0],edx
  439fa0:	e8 ba 76 01 00       	call   0x45165f
  439fa5:	83 c4 04             	add    esp,0x4
  439fa8:	8b cf                	mov    ecx,edi
  439faa:	89 86 b4 00 00 00    	mov    DWORD PTR [esi+0xb4],eax
  439fb0:	50                   	push   eax
  439fb1:	55                   	push   ebp
  439fb2:	53                   	push   ebx
  439fb3:	e8 78 7a 00 00       	call   0x441a30
  439fb8:	8b 8e b4 00 00 00    	mov    ecx,DWORD PTR [esi+0xb4]
  439fbe:	55                   	push   ebp
  439fbf:	51                   	push   ecx
  439fc0:	e8 0b e5 fe ff       	call   0x4284d0
  439fc5:	83 c4 08             	add    esp,0x8
  439fc8:	8b cf                	mov    ecx,edi
  439fca:	e8 31 7a 00 00       	call   0x441a00
  439fcf:	eb 05                	jmp    0x439fd6
  439fd1:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  439fd6:	68 60 ae 0a 00       	push   0xaae60
  439fdb:	e8 7f 76 01 00       	call   0x45165f
  439fe0:	83 c4 04             	add    esp,0x4
  439fe3:	3b fb                	cmp    edi,ebx
  439fe5:	89 86 c0 00 00 00    	mov    DWORD PTR [esi+0xc0],eax
  439feb:	74 10                	je     0x439ffd
  439fed:	8b cf                	mov    ecx,edi
  439fef:	e8 1c 77 00 00       	call   0x441710
  439ff4:	57                   	push   edi
  439ff5:	e8 56 75 01 00       	call   0x451550
  439ffa:	83 c4 04             	add    esp,0x4
  439ffd:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  43a001:	84 c0                	test   al,al
  43a003:	0f 84 d9 0a 00 00    	je     0x43aae2
  43a009:	6a 14                	push   0x14
  43a00b:	e8 4f 76 01 00       	call   0x45165f
  43a010:	83 c4 04             	add    esp,0x4
  43a013:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  43a017:	3b c3                	cmp    eax,ebx
  43a019:	c7 84 24 e8 04 00 00 	mov    DWORD PTR [esp+0x4e8],0x2
  43a020:	02 00 00 00 
  43a024:	74 09                	je     0x43a02f
  43a026:	8b c8                	mov    ecx,eax
  43a028:	e8 d3 a1 fe ff       	call   0x424200
  43a02d:	eb 02                	jmp    0x43a031
  43a02f:	33 c0                	xor    eax,eax
  43a031:	83 cd ff             	or     ebp,0xffffffff
  43a034:	8b c8                	mov    ecx,eax
  43a036:	89 ac 24 e8 04 00 00 	mov    DWORD PTR [esp+0x4e8],ebp
  43a03d:	89 86 e8 00 00 00    	mov    DWORD PTR [esi+0xe8],eax
  43a043:	e8 18 a2 fe ff       	call   0x424260
  43a048:	6a 08                	push   0x8
  43a04a:	88 44 24 17          	mov    BYTE PTR [esp+0x17],al
  43a04e:	e8 0c 76 01 00       	call   0x45165f
  43a053:	83 c4 04             	add    esp,0x4
  43a056:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  43a05a:	3b c3                	cmp    eax,ebx
  43a05c:	c7 84 24 e8 04 00 00 	mov    DWORD PTR [esp+0x4e8],0x3
  43a063:	03 00 00 00 
  43a067:	74 09                	je     0x43a072
  43a069:	8b c8                	mov    ecx,eax
  43a06b:	e8 60 e3 fe ff       	call   0x4283d0
  43a070:	eb 02                	jmp    0x43a074
  43a072:	33 c0                	xor    eax,eax
  43a074:	8b c8                	mov    ecx,eax
