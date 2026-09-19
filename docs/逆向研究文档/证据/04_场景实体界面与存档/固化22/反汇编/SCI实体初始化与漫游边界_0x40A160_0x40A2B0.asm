
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040a160 <.text+0x9160>:
  40a160:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40a166:	6a ff                	push   0xffffffff
  40a168:	68 cb d0 45 00       	push   0x45d0cb
  40a16d:	50                   	push   eax
  40a16e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40a175:	53                   	push   ebx
  40a176:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  40a17a:	56                   	push   esi
  40a17b:	57                   	push   edi
  40a17c:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  40a180:	8b f1                	mov    esi,ecx
  40a182:	89 1e                	mov    DWORD PTR [esi],ebx
  40a184:	89 7e 70             	mov    DWORD PTR [esi+0x70],edi
  40a187:	8a 47 74             	mov    al,BYTE PTR [edi+0x74]
  40a18a:	88 46 56             	mov    BYTE PTR [esi+0x56],al
  40a18d:	8a 4f 76             	mov    cl,BYTE PTR [edi+0x76]
  40a190:	88 4e 57             	mov    BYTE PTR [esi+0x57],cl
  40a193:	0f be 57 77          	movsx  edx,BYTE PTR [edi+0x77]
  40a197:	83 c2 06             	add    edx,0x6
  40a19a:	89 56 1c             	mov    DWORD PTR [esi+0x1c],edx
  40a19d:	0f be 47 78          	movsx  eax,BYTE PTR [edi+0x78]
  40a1a1:	83 c0 04             	add    eax,0x4
  40a1a4:	89 46 20             	mov    DWORD PTR [esi+0x20],eax
  40a1a7:	0f be 4f 79          	movsx  ecx,BYTE PTR [edi+0x79]
  40a1ab:	83 c1 09             	add    ecx,0x9
  40a1ae:	89 4e 24             	mov    DWORD PTR [esi+0x24],ecx
  40a1b1:	0f be 57 7a          	movsx  edx,BYTE PTR [edi+0x7a]
  40a1b5:	83 c2 06             	add    edx,0x6
  40a1b8:	89 56 28             	mov    DWORD PTR [esi+0x28],edx
  40a1bb:	8b 47 64             	mov    eax,DWORD PTR [edi+0x64]
  40a1be:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  40a1c1:	8b 4f 64             	mov    ecx,DWORD PTR [edi+0x64]
  40a1c4:	89 4e 10             	mov    DWORD PTR [esi+0x10],ecx
  40a1c7:	8b 57 68             	mov    edx,DWORD PTR [edi+0x68]
  40a1ca:	8b 4e 0c             	mov    ecx,DWORD PTR [esi+0xc]
  40a1cd:	89 56 08             	mov    DWORD PTR [esi+0x8],edx
  40a1d0:	8b 47 68             	mov    eax,DWORD PTR [edi+0x68]
  40a1d3:	89 4e 18             	mov    DWORD PTR [esi+0x18],ecx
  40a1d6:	89 46 14             	mov    DWORD PTR [esi+0x14],eax
  40a1d9:	8a 47 76             	mov    al,BYTE PTR [edi+0x76]
  40a1dc:	3c 03                	cmp    al,0x3
  40a1de:	75 4d                	jne    0x40a22d
  40a1e0:	8b 97 ef 00 00 00    	mov    edx,DWORD PTR [edi+0xef]
  40a1e6:	8b 47 64             	mov    eax,DWORD PTR [edi+0x64]
  40a1e9:	03 d0                	add    edx,eax
  40a1eb:	89 97 f7 00 00 00    	mov    DWORD PTR [edi+0xf7],edx
  40a1f1:	8b 46 70             	mov    eax,DWORD PTR [esi+0x70]
  40a1f4:	8b 88 f3 00 00 00    	mov    ecx,DWORD PTR [eax+0xf3]
  40a1fa:	8b 50 68             	mov    edx,DWORD PTR [eax+0x68]
  40a1fd:	03 ca                	add    ecx,edx
  40a1ff:	89 88 fb 00 00 00    	mov    DWORD PTR [eax+0xfb],ecx
  40a205:	8b 46 70             	mov    eax,DWORD PTR [esi+0x70]
  40a208:	8b 50 64             	mov    edx,DWORD PTR [eax+0x64]
  40a20b:	8b 88 ef 00 00 00    	mov    ecx,DWORD PTR [eax+0xef]
  40a211:	2b d1                	sub    edx,ecx
  40a213:	89 90 ef 00 00 00    	mov    DWORD PTR [eax+0xef],edx
  40a219:	8b 46 70             	mov    eax,DWORD PTR [esi+0x70]
  40a21c:	8b 48 68             	mov    ecx,DWORD PTR [eax+0x68]
  40a21f:	8b 90 f3 00 00 00    	mov    edx,DWORD PTR [eax+0xf3]
  40a225:	2b ca                	sub    ecx,edx
  40a227:	89 88 f3 00 00 00    	mov    DWORD PTR [eax+0xf3],ecx
  40a22d:	68 84 00 00 00       	push   0x84
  40a232:	e8 28 74 04 00       	call   0x45165f
  40a237:	83 c4 04             	add    esp,0x4
  40a23a:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  40a23e:	85 c0                	test   eax,eax
  40a240:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  40a247:	00 
  40a248:	74 09                	je     0x40a253
  40a24a:	8b c8                	mov    ecx,eax
  40a24c:	e8 2f ce ff ff       	call   0x407080
  40a251:	eb 02                	jmp    0x40a255
  40a253:	33 c0                	xor    eax,eax
  40a255:	33 d2                	xor    edx,edx
  40a257:	89 46 6c             	mov    DWORD PTR [esi+0x6c],eax
  40a25a:	8a 97 e1 00 00 00    	mov    dl,BYTE PTR [edi+0xe1]
  40a260:	8a 8f ee 00 00 00    	mov    cl,BYTE PTR [edi+0xee]
  40a266:	52                   	push   edx
  40a267:	51                   	push   ecx
  40a268:	0f bf 57 6e          	movsx  edx,WORD PTR [edi+0x6e]
  40a26c:	0f bf 4f 6c          	movsx  ecx,WORD PTR [edi+0x6c]
  40a270:	53                   	push   ebx
  40a271:	8b 5e 14             	mov    ebx,DWORD PTR [esi+0x14]
  40a274:	03 d3                	add    edx,ebx
  40a276:	8b 5e 10             	mov    ebx,DWORD PTR [esi+0x10]
  40a279:	03 cb                	add    ecx,ebx
  40a27b:	52                   	push   edx
  40a27c:	83 c7 7b             	add    edi,0x7b
  40a27f:	51                   	push   ecx
  40a280:	57                   	push   edi
  40a281:	8b c8                	mov    ecx,eax
  40a283:	c7 44 24 2c ff ff ff 	mov    DWORD PTR [esp+0x2c],0xffffffff
  40a28a:	ff 
  40a28b:	e8 40 ce ff ff       	call   0x4070d0
  40a290:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  40a294:	5f                   	pop    edi
  40a295:	5e                   	pop    esi
  40a296:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40a29d:	5b                   	pop    ebx
  40a29e:	83 c4 0c             	add    esp,0xc
  40a2a1:	c2 08 00             	ret    0x8
  40a2a4:	90                   	nop
  40a2a5:	90                   	nop
  40a2a6:	90                   	nop
  40a2a7:	90                   	nop
  40a2a8:	90                   	nop
  40a2a9:	90                   	nop
  40a2aa:	90                   	nop
  40a2ab:	90                   	nop
  40a2ac:	90                   	nop
  40a2ad:	90                   	nop
  40a2ae:	90                   	nop
  40a2af:	90                   	nop
