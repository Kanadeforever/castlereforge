
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00408e80 <.text+0x7e80>:
  408e80:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  408e86:	6a ff                	push   0xffffffff
  408e88:	68 92 d0 45 00       	push   0x45d092
  408e8d:	50                   	push   eax
  408e8e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  408e95:	83 ec 64             	sub    esp,0x64
  408e98:	53                   	push   ebx
  408e99:	8b 5c 24 78          	mov    ebx,DWORD PTR [esp+0x78]
  408e9d:	55                   	push   ebp
  408e9e:	56                   	push   esi
  408e9f:	57                   	push   edi
  408ea0:	8b f1                	mov    esi,ecx
  408ea2:	8d 7b 74             	lea    edi,[ebx+0x74]
  408ea5:	89 1e                	mov    DWORD PTR [esi],ebx
  408ea7:	80 3f 00             	cmp    BYTE PTR [edi],0x0
  408eaa:	0f 84 e1 00 00 00    	je     0x408f91
  408eb0:	33 c0                	xor    eax,eax
  408eb2:	33 ed                	xor    ebp,ebp
  408eb4:	8a 83 d8 00 00 00    	mov    al,BYTE PTR [ebx+0xd8]
  408eba:	2b c5                	sub    eax,ebp
  408ebc:	74 55                	je     0x408f13
  408ebe:	48                   	dec    eax
  408ebf:	0f 85 f1 00 00 00    	jne    0x408fb6
  408ec5:	68 84 00 00 00       	push   0x84
  408eca:	e8 90 87 04 00       	call   0x45165f
  408ecf:	83 c4 04             	add    esp,0x4
  408ed2:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  408ed9:	3b c5                	cmp    eax,ebp
  408edb:	c7 44 24 7c 01 00 00 	mov    DWORD PTR [esp+0x7c],0x1
  408ee2:	00 
  408ee3:	74 09                	je     0x408eee
  408ee5:	8b c8                	mov    ecx,eax
  408ee7:	e8 94 e1 ff ff       	call   0x407080
  408eec:	eb 02                	jmp    0x408ef0
  408eee:	33 c0                	xor    eax,eax
  408ef0:	55                   	push   ebp
  408ef1:	55                   	push   ebp
  408ef2:	6a 01                	push   0x1
  408ef4:	6a 30                	push   0x30
  408ef6:	6a 40                	push   0x40
  408ef8:	57                   	push   edi
  408ef9:	8b c8                	mov    ecx,eax
  408efb:	c7 84 24 94 00 00 00 	mov    DWORD PTR [esp+0x94],0xffffffff
  408f02:	ff ff ff ff 
  408f06:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  408f09:	e8 c2 e1 ff ff       	call   0x4070d0
  408f0e:	e9 a3 00 00 00       	jmp    0x408fb6
  408f13:	6a 30                	push   0x30
  408f15:	e8 45 87 04 00       	call   0x45165f
  408f1a:	83 c4 04             	add    esp,0x4
  408f1d:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  408f24:	3b c5                	cmp    eax,ebp
  408f26:	89 6c 24 7c          	mov    DWORD PTR [esp+0x7c],ebp
  408f2a:	74 09                	je     0x408f35
  408f2c:	8b c8                	mov    ecx,eax
  408f2e:	e8 fd d6 ff ff       	call   0x406630
  408f33:	eb 02                	jmp    0x408f37
  408f35:	33 c0                	xor    eax,eax
  408f37:	33 c9                	xor    ecx,ecx
  408f39:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  408f3c:	8a 4b 65             	mov    cl,BYTE PTR [ebx+0x65]
  408f3f:	c7 44 24 7c ff ff ff 	mov    DWORD PTR [esp+0x7c],0xffffffff
  408f46:	ff 
  408f47:	0f be 53 6e          	movsx  edx,BYTE PTR [ebx+0x6e]
  408f4b:	51                   	push   ecx
  408f4c:	52                   	push   edx
  408f4d:	0f bf 4b 72          	movsx  ecx,WORD PTR [ebx+0x72]
  408f51:	0f bf 53 70          	movsx  edx,WORD PTR [ebx+0x70]
  408f55:	51                   	push   ecx
  408f56:	33 c9                	xor    ecx,ecx
  408f58:	8a 4b 6f             	mov    cl,BYTE PTR [ebx+0x6f]
  408f5b:	52                   	push   edx
  408f5c:	0f bf 53 68          	movsx  edx,WORD PTR [ebx+0x68]
  408f60:	51                   	push   ecx
  408f61:	52                   	push   edx
  408f62:	0f bf 4b 66          	movsx  ecx,WORD PTR [ebx+0x66]
  408f66:	51                   	push   ecx
  408f67:	57                   	push   edi
  408f68:	8b c8                	mov    ecx,eax
  408f6a:	e8 f1 d6 ff ff       	call   0x406660
  408f6f:	8b 06                	mov    eax,DWORD PTR [esi]
  408f71:	33 d2                	xor    edx,edx
  408f73:	33 c9                	xor    ecx,ecx
  408f75:	66 8b 50 6c          	mov    dx,WORD PTR [eax+0x6c]
  408f79:	66 8b 48 6a          	mov    cx,WORD PTR [eax+0x6a]
  408f7d:	52                   	push   edx
  408f7e:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  408f81:	51                   	push   ecx
  408f82:	52                   	push   edx
  408f83:	e8 78 db ff ff       	call   0x406b00
  408f88:	83 c4 0c             	add    esp,0xc
  408f8b:	c6 46 35 01          	mov    BYTE PTR [esi+0x35],0x1
  408f8f:	eb 25                	jmp    0x408fb6
  408f91:	66 a1 e4 f6 89 00    	mov    ax,ds:0x89f6e4
  408f97:	33 ed                	xor    ebp,ebp
  408f99:	66 89 43 6a          	mov    WORD PTR [ebx+0x6a],ax
  408f9d:	8b 0e                	mov    ecx,DWORD PTR [esi]
  408f9f:	66 8b 15 94 f7 89 00 	mov    dx,WORD PTR ds:0x89f794
  408fa6:	66 89 51 6c          	mov    WORD PTR [ecx+0x6c],dx
  408faa:	a1 9c f7 89 00       	mov    eax,ds:0x89f79c
  408faf:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  408fb2:	c6 46 35 00          	mov    BYTE PTR [esi+0x35],0x0
  408fb6:	8a 83 d9 00 00 00    	mov    al,BYTE PTR [ebx+0xd9]
  408fbc:	8d bb d9 00 00 00    	lea    edi,[ebx+0xd9]
  408fc2:	84 c0                	test   al,al
  408fc4:	74 51                	je     0x409017
  408fc6:	68 84 00 00 00       	push   0x84
  408fcb:	e8 8f 86 04 00       	call   0x45165f
  408fd0:	83 c4 04             	add    esp,0x4
  408fd3:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  408fda:	3b c5                	cmp    eax,ebp
  408fdc:	c7 44 24 7c 02 00 00 	mov    DWORD PTR [esp+0x7c],0x2
  408fe3:	00 
  408fe4:	74 09                	je     0x408fef
  408fe6:	8b c8                	mov    ecx,eax
  408fe8:	e8 93 e0 ff ff       	call   0x407080
  408fed:	eb 02                	jmp    0x408ff1
  408fef:	33 c0                	xor    eax,eax
  408ff1:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  408ff4:	55                   	push   ebp
  408ff5:	0f bf 4b 68          	movsx  ecx,WORD PTR [ebx+0x68]
  408ff9:	0f bf 53 66          	movsx  edx,WORD PTR [ebx+0x66]
  408ffd:	55                   	push   ebp
  408ffe:	6a 01                	push   0x1
  409000:	51                   	push   ecx
  409001:	52                   	push   edx
  409002:	57                   	push   edi
  409003:	8b c8                	mov    ecx,eax
  409005:	c7 84 24 94 00 00 00 	mov    DWORD PTR [esp+0x94],0xffffffff
  40900c:	ff ff ff ff 
  409010:	e8 bb e0 ff ff       	call   0x4070d0
  409015:	eb 03                	jmp    0x40901a
  409017:	89 6e 0c             	mov    DWORD PTR [esi+0xc],ebp
  40901a:	8a 83 3d 01 00 00    	mov    al,BYTE PTR [ebx+0x13d]
  409020:	8d bb 3d 01 00 00    	lea    edi,[ebx+0x13d]
  409026:	84 c0                	test   al,al
  409028:	74 51                	je     0x40907b
  40902a:	68 84 00 00 00       	push   0x84
  40902f:	e8 2b 86 04 00       	call   0x45165f
  409034:	83 c4 04             	add    esp,0x4
  409037:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  40903e:	3b c5                	cmp    eax,ebp
  409040:	c7 44 24 7c 03 00 00 	mov    DWORD PTR [esp+0x7c],0x3
  409047:	00 
  409048:	74 09                	je     0x409053
  40904a:	8b c8                	mov    ecx,eax
  40904c:	e8 2f e0 ff ff       	call   0x407080
  409051:	eb 02                	jmp    0x409055
  409053:	33 c0                	xor    eax,eax
  409055:	89 46 10             	mov    DWORD PTR [esi+0x10],eax
  409058:	55                   	push   ebp
  409059:	0f bf 4b 68          	movsx  ecx,WORD PTR [ebx+0x68]
  40905d:	0f bf 53 66          	movsx  edx,WORD PTR [ebx+0x66]
  409061:	55                   	push   ebp
  409062:	6a 01                	push   0x1
  409064:	51                   	push   ecx
  409065:	52                   	push   edx
  409066:	57                   	push   edi
  409067:	8b c8                	mov    ecx,eax
  409069:	c7 84 24 94 00 00 00 	mov    DWORD PTR [esp+0x94],0xffffffff
  409070:	ff ff ff ff 
  409074:	e8 57 e0 ff ff       	call   0x4070d0
  409079:	eb 03                	jmp    0x40907e
  40907b:	89 6e 10             	mov    DWORD PTR [esi+0x10],ebp
  40907e:	8d 46 28             	lea    eax,[esi+0x28]
  409081:	6a 02                	push   0x2
  409083:	50                   	push   eax
  409084:	68 20 8c 46 00       	push   0x468c20
  409089:	e8 22 95 ff ff       	call   0x4025b0
  40908e:	8b c8                	mov    ecx,eax
  409090:	b8 7d 55 78 3b       	mov    eax,0x3b78557d
  409095:	f7 e1                	mul    ecx
  409097:	c1 ea 07             	shr    edx,0x7
  40909a:	8d 83 05 02 00 00    	lea    eax,[ebx+0x205]
  4090a0:	89 56 18             	mov    DWORD PTR [esi+0x18],edx
  4090a3:	83 c4 0c             	add    esp,0xc
  4090a6:	80 38 00             	cmp    BYTE PTR [eax],0x0
  4090a9:	74 32                	je     0x4090dd
  4090ab:	8d 56 24             	lea    edx,[esi+0x24]
  4090ae:	6a 01                	push   0x1
  4090b0:	52                   	push   edx
  4090b1:	50                   	push   eax
  4090b2:	e8 f9 94 ff ff       	call   0x4025b0
  4090b7:	8b c8                	mov    ecx,eax
  4090b9:	b8 7d 55 78 3b       	mov    eax,0x3b78557d
  4090be:	f7 e1                	mul    ecx
  4090c0:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  4090c3:	c1 ea 07             	shr    edx,0x7
  4090c6:	89 56 14             	mov    DWORD PTR [esi+0x14],edx
  4090c9:	03 d0                	add    edx,eax
  4090cb:	8d 14 95 08 00 00 00 	lea    edx,[edx*4+0x8]
  4090d2:	52                   	push   edx
  4090d3:	e8 87 85 04 00       	call   0x45165f
  4090d8:	83 c4 10             	add    esp,0x10
  4090db:	eb 13                	jmp    0x4090f0
  4090dd:	8d 04 95 08 00 00 00 	lea    eax,[edx*4+0x8]
  4090e4:	89 6e 14             	mov    DWORD PTR [esi+0x14],ebp
  4090e7:	50                   	push   eax
  4090e8:	e8 72 85 04 00       	call   0x45165f
  4090ed:	83 c4 04             	add    esp,0x4
  4090f0:	8b 56 14             	mov    edx,DWORD PTR [esi+0x14]
  4090f3:	89 46 2c             	mov    DWORD PTR [esi+0x2c],eax
  4090f6:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  4090f9:	33 ff                	xor    edi,edi
  4090fb:	8b c8                	mov    ecx,eax
  4090fd:	03 ca                	add    ecx,edx
  4090ff:	3b c5                	cmp    eax,ebp
  409101:	89 4e 1c             	mov    DWORD PTR [esi+0x1c],ecx
  409104:	7e 73                	jle    0x409179
  409106:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  409109:	c7 04 ba 00 00 00 00 	mov    DWORD PTR [edx+edi*4],0x0
  409110:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  409113:	8a 4c 28 70          	mov    cl,BYTE PTR [eax+ebp*1+0x70]
  409117:	84 c9                	test   cl,cl
  409119:	74 4e                	je     0x409169
  40911b:	6a 74                	push   0x74
  40911d:	e8 3d 85 04 00       	call   0x45165f
  409122:	83 c4 04             	add    esp,0x4
  409125:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  40912c:	85 c0                	test   eax,eax
  40912e:	c7 44 24 7c 04 00 00 	mov    DWORD PTR [esp+0x7c],0x4
  409135:	00 
  409136:	74 09                	je     0x409141
  409138:	8b c8                	mov    ecx,eax
  40913a:	e8 a1 0f 00 00       	call   0x40a0e0
  40913f:	eb 02                	jmp    0x409143
  409141:	33 c0                	xor    eax,eax
  409143:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  409146:	8b d5                	mov    edx,ebp
  409148:	6a 02                	push   0x2
  40914a:	c7 84 24 80 00 00 00 	mov    DWORD PTR [esp+0x80],0xffffffff
  409151:	ff ff ff ff 
  409155:	89 04 b9             	mov    DWORD PTR [ecx+edi*4],eax
  409158:	8b 4e 28             	mov    ecx,DWORD PTR [esi+0x28]
  40915b:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  40915e:	03 d1                	add    edx,ecx
  409160:	52                   	push   edx
  409161:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  409164:	e8 f7 0f 00 00       	call   0x40a160
  409169:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  40916c:	47                   	inc    edi
  40916d:	81 c5 27 02 00 00    	add    ebp,0x227
  409173:	3b f8                	cmp    edi,eax
  409175:	7c 8f                	jl     0x409106
  409177:	33 ed                	xor    ebp,ebp
  409179:	8b 46 14             	mov    eax,DWORD PTR [esi+0x14]
  40917c:	85 c0                	test   eax,eax
  40917e:	0f 8e 23 01 00 00    	jle    0x4092a7
  409184:	33 ff                	xor    edi,edi
  409186:	8b 4e 18             	mov    ecx,DWORD PTR [esi+0x18]
  409189:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  40918c:	03 cd                	add    ecx,ebp
  40918e:	c7 04 8a 00 00 00 00 	mov    DWORD PTR [edx+ecx*4],0x0
  409195:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  409198:	8b 84 07 ff 00 00 00 	mov    eax,DWORD PTR [edi+eax*1+0xff]
  40919f:	85 c0                	test   eax,eax
  4091a1:	74 1e                	je     0x4091c1
  4091a3:	50                   	push   eax
  4091a4:	e8 27 1f 04 00       	call   0x44b0d0
  4091a9:	83 c4 04             	add    esp,0x4
  4091ac:	85 c0                	test   eax,eax
  4091ae:	75 09                	jne    0x4091b9
  4091b0:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  4091b3:	88 44 0f 70          	mov    BYTE PTR [edi+ecx*1+0x70],al
  4091b7:	eb 08                	jmp    0x4091c1
  4091b9:	8b 56 24             	mov    edx,DWORD PTR [esi+0x24]
  4091bc:	c6 44 17 70 01       	mov    BYTE PTR [edi+edx*1+0x70],0x1
  4091c1:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  4091c4:	8a 4c 07 70          	mov    cl,BYTE PTR [edi+eax*1+0x70]
  4091c8:	84 c9                	test   cl,cl
  4091ca:	0f 84 c5 00 00 00    	je     0x409295
  4091d0:	6a 74                	push   0x74
  4091d2:	e8 88 84 04 00       	call   0x45165f
  4091d7:	83 c4 04             	add    esp,0x4
  4091da:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  4091e1:	85 c0                	test   eax,eax
  4091e3:	c7 44 24 7c 05 00 00 	mov    DWORD PTR [esp+0x7c],0x5
  4091ea:	00 
  4091eb:	74 09                	je     0x4091f6
  4091ed:	8b c8                	mov    ecx,eax
  4091ef:	e8 ec 0e 00 00       	call   0x40a0e0
  4091f4:	eb 02                	jmp    0x4091f8
  4091f6:	33 c0                	xor    eax,eax
  4091f8:	8b 4e 18             	mov    ecx,DWORD PTR [esi+0x18]
  4091fb:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  4091fe:	03 cd                	add    ecx,ebp
  409200:	6a 01                	push   0x1
  409202:	c7 84 24 80 00 00 00 	mov    DWORD PTR [esp+0x80],0xffffffff
  409209:	ff ff ff ff 
  40920d:	89 04 8a             	mov    DWORD PTR [edx+ecx*4],eax
  409210:	8b 56 24             	mov    edx,DWORD PTR [esi+0x24]
  409213:	8b 4e 18             	mov    ecx,DWORD PTR [esi+0x18]
  409216:	8b c7                	mov    eax,edi
  409218:	03 c2                	add    eax,edx
  40921a:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  40921d:	03 cd                	add    ecx,ebp
  40921f:	50                   	push   eax
  409220:	8b 0c 8a             	mov    ecx,DWORD PTR [edx+ecx*4]
  409223:	e8 38 0f 00 00       	call   0x40a160
  409228:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  40922b:	8b 84 07 ff 00 00 00 	mov    eax,DWORD PTR [edi+eax*1+0xff]
  409232:	85 c0                	test   eax,eax
  409234:	74 5f                	je     0x409295
  409236:	50                   	push   eax
  409237:	e8 94 1e 04 00       	call   0x44b0d0
  40923c:	83 c4 04             	add    esp,0x4
  40923f:	2d fe 00 00 00       	sub    eax,0xfe
  409244:	74 2e                	je     0x409274
  409246:	48                   	dec    eax
  409247:	75 4c                	jne    0x409295
  409249:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  40924c:	c6 84 0f e2 00 00 00 	mov    BYTE PTR [edi+ecx*1+0xe2],0x1
  409253:	01 
  409254:	8b 56 24             	mov    edx,DWORD PTR [esi+0x24]
  409257:	c6 84 17 e3 00 00 00 	mov    BYTE PTR [edi+edx*1+0xe3],0x1
  40925e:	01 
  40925f:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  409262:	c6 84 07 eb 00 00 00 	mov    BYTE PTR [edi+eax*1+0xeb],0x0
  409269:	00 
  40926a:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  40926d:	c6 44 0f 73 00       	mov    BYTE PTR [edi+ecx*1+0x73],0x0
  409272:	eb 21                	jmp    0x409295
  409274:	8b 56 24             	mov    edx,DWORD PTR [esi+0x24]
  409277:	c6 84 17 e2 00 00 00 	mov    BYTE PTR [edi+edx*1+0xe2],0x2
  40927e:	02 
  40927f:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  409282:	c6 84 07 e3 00 00 00 	mov    BYTE PTR [edi+eax*1+0xe3],0x1
  409289:	01 
  40928a:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  40928d:	c6 84 0f ea 00 00 00 	mov    BYTE PTR [edi+ecx*1+0xea],0x0
  409294:	00 
  409295:	8b 46 14             	mov    eax,DWORD PTR [esi+0x14]
  409298:	45                   	inc    ebp
  409299:	81 c7 27 02 00 00    	add    edi,0x227
  40929f:	3b e8                	cmp    ebp,eax
  4092a1:	0f 8c df fe ff ff    	jl     0x409186
  4092a7:	e8 64 f9 ff ff       	call   0x408c10
  4092ac:	8b 56 1c             	mov    edx,DWORD PTR [esi+0x1c]
  4092af:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  4092b2:	89 04 91             	mov    DWORD PTR [ecx+edx*4],eax
  4092b5:	e8 56 f9 ff ff       	call   0x408c10
  4092ba:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  4092bd:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  4092c0:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  4092c3:	8b 2d 94 01 46 00    	mov    ebp,DWORD PTR ds:0x460194
  4092c9:	52                   	push   edx
  4092ca:	68 a8 81 46 00       	push   0x4681a8
  4092cf:	8b 14 81             	mov    edx,DWORD PTR [ecx+eax*4]
  4092d2:	8b 42 70             	mov    eax,DWORD PTR [edx+0x70]
  4092d5:	50                   	push   eax
  4092d6:	ff d5                	call   ebp
  4092d8:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  4092db:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  4092de:	c7 44 8a 04 00 00 00 	mov    DWORD PTR [edx+ecx*4+0x4],0x0
  4092e5:	00 
  4092e6:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  4092e9:	8d 0c 85 08 00 00 00 	lea    ecx,[eax*4+0x8]
  4092f0:	51                   	push   ecx
  4092f1:	e8 69 83 04 00       	call   0x45165f
  4092f6:	89 46 30             	mov    DWORD PTR [esi+0x30],eax
  4092f9:	8a 83 cd 02 00 00    	mov    al,BYTE PTR [ebx+0x2cd]
  4092ff:	8d b3 cd 02 00 00    	lea    esi,[ebx+0x2cd]
  409305:	83 c4 10             	add    esp,0x10
  409308:	84 c0                	test   al,al
  40930a:	74 51                	je     0x40935d
  40930c:	b9 19 00 00 00       	mov    ecx,0x19
  409311:	33 c0                	xor    eax,eax
  409313:	8d 7c 24 10          	lea    edi,[esp+0x10]
  409317:	56                   	push   esi
  409318:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  40931a:	e8 f1 1e 00 00       	call   0x40b210
  40931f:	8b fe                	mov    edi,esi
  409321:	83 c9 ff             	or     ecx,0xffffffff
  409324:	33 c0                	xor    eax,eax
  409326:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  409328:	f7 d1                	not    ecx
  40932a:	83 c1 fc             	add    ecx,0xfffffffc
  40932d:	8d 7c 24 14          	lea    edi,[esp+0x14]
  409331:	8b d1                	mov    edx,ecx
  409333:	8d 44 24 14          	lea    eax,[esp+0x14]
  409337:	c1 e9 02             	shr    ecx,0x2
  40933a:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  40933c:	8b ca                	mov    ecx,edx
  40933e:	50                   	push   eax
  40933f:	83 e1 03             	and    ecx,0x3
  409342:	68 18 8c 46 00       	push   0x468c18
  409347:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  409349:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  40934d:	51                   	push   ecx
  40934e:	ff d5                	call   ebp
  409350:	8d 54 24 20          	lea    edx,[esp+0x20]
  409354:	52                   	push   edx
  409355:	e8 c6 a7 ff ff       	call   0x403b20
  40935a:	83 c4 14             	add    esp,0x14
  40935d:	8b 84 24 88 00 00 00 	mov    eax,DWORD PTR [esp+0x88]
  409364:	85 c0                	test   eax,eax
  409366:	75 74                	jne    0x4093dc
  409368:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40936e:	6a 00                	push   0x0
  409370:	e8 eb a0 ff ff       	call   0x403460
  409375:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40937b:	33 c0                	xor    eax,eax
  40937d:	8a 83 31 03 00 00    	mov    al,BYTE PTR [ebx+0x331]
  409383:	6a 01                	push   0x1
  409385:	50                   	push   eax
  409386:	e8 35 9f ff ff       	call   0x4032c0
  40938b:	33 f6                	xor    esi,esi
  40938d:	33 c9                	xor    ecx,ecx
  40938f:	6a 01                	push   0x1
  409391:	8a 8c 33 73 03 00 00 	mov    cl,BYTE PTR [ebx+esi*1+0x373]
  409398:	51                   	push   ecx
  409399:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40939f:	e8 dc 9f ff ff       	call   0x403380
  4093a4:	46                   	inc    esi
  4093a5:	83 fe 05             	cmp    esi,0x5
  4093a8:	7c e3                	jl     0x40938d
  4093aa:	8a 83 a1 01 00 00    	mov    al,BYTE PTR [ebx+0x1a1]
  4093b0:	84 c0                	test   al,al
  4093b2:	0f 95 c2             	setne  dl
  4093b5:	88 15 24 f6 46 00    	mov    BYTE PTR ds:0x46f624,dl
  4093bb:	8b 83 78 03 00 00    	mov    eax,DWORD PTR [ebx+0x378]
  4093c1:	50                   	push   eax
  4093c2:	e8 49 a1 ff ff       	call   0x403510
  4093c7:	8b 8b 7c 03 00 00    	mov    ecx,DWORD PTR [ebx+0x37c]
  4093cd:	51                   	push   ecx
  4093ce:	e8 fd 1c 04 00       	call   0x44b0d0
  4093d3:	50                   	push   eax
  4093d4:	e8 b7 a1 ff ff       	call   0x403590
  4093d9:	83 c4 0c             	add    esp,0xc
  4093dc:	8b 4c 24 74          	mov    ecx,DWORD PTR [esp+0x74]
  4093e0:	5f                   	pop    edi
  4093e1:	5e                   	pop    esi
  4093e2:	5d                   	pop    ebp
  4093e3:	5b                   	pop    ebx
  4093e4:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4093eb:	83 c4 70             	add    esp,0x70
  4093ee:	c2                   	.byte 0xc2
  4093ef:	08                   	.byte 0x8
