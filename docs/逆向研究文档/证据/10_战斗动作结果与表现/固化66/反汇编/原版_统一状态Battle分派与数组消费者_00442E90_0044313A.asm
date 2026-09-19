
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442e90 <.text+0x41e90>:
  442e90:	03 0f                	add    ecx,DWORD PTR [edi]
  442e92:	87 ba 00 00 00 ff    	xchg   DWORD PTR [edx-0x1000000],edi
  442e98:	24 85                	and    al,0x85
  442e9a:	28 32                	sub    BYTE PTR [edx],dh
  442e9c:	44                   	inc    esp
  442e9d:	00 81 be c8 10 00    	add    BYTE PTR [ecx+0x10c8be],al
  442ea3:	00 bb 0b 00 00 74    	add    BYTE PTR [ebx+0x7400000b],bh
  442ea9:	46                   	inc    esi
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
  442fa0:	74 37                	je     0x442fd9
  442fa2:	8b 86 48 fa ff ff    	mov    eax,DWORD PTR [esi-0x5b8]
  442fa8:	85 c0                	test   eax,eax
  442faa:	7c 2d                	jl     0x442fd9
  442fac:	81 3e bb 0b 00 00    	cmp    DWORD PTR [esi],0xbbb
  442fb2:	74 25                	je     0x442fd9
  442fb4:	81 7e 04 a0 0f 00 00 	cmp    DWORD PTR [esi+0x4],0xfa0
  442fbb:	75 1c                	jne    0x442fd9
  442fbd:	6a 02                	push   0x2
  442fbf:	8d 4e c4             	lea    ecx,[esi-0x3c]
  442fc2:	e8 b9 0f fe ff       	call   0x423f80
  442fc7:	c7 46 fc d4 07 00 00 	mov    DWORD PTR [esi-0x4],0x7d4
  442fce:	c7 46 0c 01 00 00 00 	mov    DWORD PTR [esi+0xc],0x1
  442fd5:	c6 45 00 00          	mov    BYTE PTR [ebp+0x0],0x0
  442fd9:	81 c6 e4 0d 00 00    	add    esi,0xde4
  442fdf:	45                   	inc    ebp
  442fe0:	8d 04 2b             	lea    eax,[ebx+ebp*1]
  442fe3:	83 f8 10             	cmp    eax,0x10
  442fe6:	7c b4                	jl     0x442f9c
  442fe8:	8b 87 ac e2 00 00    	mov    eax,DWORD PTR [edi+0xe2ac]
  442fee:	85 c0                	test   eax,eax
  442ff0:	0f 8d aa 00 00 00    	jge    0x4430a0
  442ff6:	8a 87 a8 e2 00 00    	mov    al,BYTE PTR [edi+0xe2a8]
  442ffc:	84 c0                	test   al,al
  442ffe:	0f 85 9c 00 00 00    	jne    0x4430a0
  443004:	8b 87 24 e2 00 00    	mov    eax,DWORD PTR [edi+0xe224]
  44300a:	83 ce ff             	or     esi,0xffffffff
  44300d:	3b c6                	cmp    eax,esi
  44300f:	0f 85 8e 00 00 00    	jne    0x4430a3
  443015:	8a 87 30 e3 00 00    	mov    al,BYTE PTR [edi+0xe330]
  44301b:	32 db                	xor    bl,bl
  44301d:	84 c0                	test   al,al
  44301f:	be 64 00 00 00       	mov    esi,0x64
  443024:	75 7a                	jne    0x4430a0
  443026:	8a 87 2d e5 00 00    	mov    al,BYTE PTR [edi+0xe52d]
  44302c:	84 c0                	test   al,al
  44302e:	75 70                	jne    0x4430a0
  443030:	85 f6                	test   esi,esi
  443032:	7e 6c                	jle    0x4430a0
  443034:	8d 8f f8 e2 00 00    	lea    ecx,[edi+0xe2f8]
  44303a:	e8 71 11 fe ff       	call   0x4241b0
  44303f:	85 c0                	test   eax,eax
  443041:	89 87 ac e2 00 00    	mov    DWORD PTR [edi+0xe2ac],eax
  443047:	7c 30                	jl     0x443079
  443049:	8d 0c c0             	lea    ecx,[eax+eax*8]
  44304c:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  44304f:	8d 14 49             	lea    edx,[ecx+ecx*2]
  443052:	8d 04 d0             	lea    eax,[eax+edx*8]
  443055:	8b 8c 87 60 0c 00 00 	mov    ecx,DWORD PTR [edi+eax*4+0xc60]
  44305c:	8d 04 87             	lea    eax,[edi+eax*4]
  44305f:	85 c9                	test   ecx,ecx
  443061:	7e 22                	jle    0x443085
  443063:	8b 88 30 0c 00 00    	mov    ecx,DWORD PTR [eax+0xc30]
  443069:	51                   	push   ecx
  44306a:	8b 0d 74 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd74
  443070:	e8 bb 8e fd ff       	call   0x41bf30
  443075:	b3 01                	mov    bl,0x1
  443077:	eb 0c                	jmp    0x443085
  443079:	b3 01                	mov    bl,0x1
  44307b:	c7 87 ac e2 00 00 ff 	mov    DWORD PTR [edi+0xe2ac],0xffffffff
  443082:	ff ff ff 
  443085:	4e                   	dec    esi
  443086:	85 f6                	test   esi,esi
  443088:	7f 12                	jg     0x44309c
  44308a:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  443090:	68 cc c4 46 00       	push   0x46c4cc
  443095:	6a 00                	push   0x0
  443097:	e8 d4 ed fe ff       	call   0x431e70
  44309c:	84 db                	test   bl,bl
  44309e:	74 90                	je     0x443030
  4430a0:	83 ce ff             	or     esi,0xffffffff
  4430a3:	8a 87 30 e3 00 00    	mov    al,BYTE PTR [edi+0xe330]
  4430a9:	84 c0                	test   al,al
  4430ab:	0f 85 69 01 00 00    	jne    0x44321a
  4430b1:	8a 87 2d e5 00 00    	mov    al,BYTE PTR [edi+0xe52d]
  4430b7:	84 c0                	test   al,al
  4430b9:	0f 85 5b 01 00 00    	jne    0x44321a
  4430bf:	39 b7 24 e2 00 00    	cmp    DWORD PTR [edi+0xe224],esi
  4430c5:	0f 85 4f 01 00 00    	jne    0x44321a
  4430cb:	8b 87 ac e2 00 00    	mov    eax,DWORD PTR [edi+0xe2ac]
  4430d1:	85 c0                	test   eax,eax
  4430d3:	0f 8c 3b 01 00 00    	jl     0x443214
  4430d9:	8d 14 c0             	lea    edx,[eax+eax*8]
  4430dc:	8d 0c 90             	lea    ecx,[eax+edx*4]
  4430df:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  4430e2:	8d 14 c8             	lea    edx,[eax+ecx*8]
  4430e5:	8b 8c 97 c8 10 00 00 	mov    ecx,DWORD PTR [edi+edx*4+0x10c8]
  4430ec:	81 f9 bb 0b 00 00    	cmp    ecx,0xbbb
  4430f2:	8d 04 97             	lea    eax,[edi+edx*4]
  4430f5:	74 40                	je     0x443137
  4430f7:	8a 8f a8 e2 00 00    	mov    cl,BYTE PTR [edi+0xe2a8]
  4430fd:	84 c9                	test   cl,cl
  4430ff:	75 36                	jne    0x443137
  443101:	8b 80 30 0c 00 00    	mov    eax,DWORD PTR [eax+0xc30]
  443107:	8b 0d 74 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd74
  44310d:	50                   	push   eax
  44310e:	e8 1d 8e fd ff       	call   0x41bf30
  443113:	84 c0                	test   al,al
  443115:	74 1a                	je     0x443131
  443117:	8b 0d 74 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd74
  44311d:	8b 89 e8 05 00 00    	mov    ecx,DWORD PTR [ecx+0x5e8]
  443123:	e8 c8 92 fd ff       	call   0x41c3f0
  443128:	c6 87 a8 e2 00 00 01 	mov    BYTE PTR [edi+0xe2a8],0x1
  44312f:	eb 06                	jmp    0x443137
  443131:	89 b7 ac e2 00 00    	mov    DWORD PTR [edi+0xe2ac],esi
  443137:	8a                   	.byte 0x8a
  443138:	87                   	.byte 0x87
  443139:	a8                   	.byte 0xa8
