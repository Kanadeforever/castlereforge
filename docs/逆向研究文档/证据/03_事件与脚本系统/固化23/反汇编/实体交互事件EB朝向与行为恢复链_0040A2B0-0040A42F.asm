
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040a2b0 <.text+0x92b0>:
  40a2b0:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  40a2b4:	53                   	push   ebx
  40a2b5:	55                   	push   ebp
  40a2b6:	56                   	push   esi
  40a2b7:	81 fa 07 b2 01 00    	cmp    edx,0x1b207
  40a2bd:	57                   	push   edi
  40a2be:	8b f1                	mov    esi,ecx
  40a2c0:	75 03                	jne    0x40a2c5
  40a2c2:	8b 56 10             	mov    edx,DWORD PTR [esi+0x10]
  40a2c5:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  40a2c9:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  40a2cc:	81 f9 07 b2 01 00    	cmp    ecx,0x1b207
  40a2d2:	75 03                	jne    0x40a2d7
  40a2d4:	8b 4e 14             	mov    ecx,DWORD PTR [esi+0x14]
  40a2d7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  40a2db:	89 4e 08             	mov    DWORD PTR [esi+0x8],ecx
  40a2de:	3d 07 b2 01 00       	cmp    eax,0x1b207
  40a2e3:	75 03                	jne    0x40a2e8
  40a2e5:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  40a2e8:	89 4e 14             	mov    DWORD PTR [esi+0x14],ecx
  40a2eb:	8a 4e 54             	mov    cl,BYTE PTR [esi+0x54]
  40a2ee:	89 56 10             	mov    DWORD PTR [esi+0x10],edx
  40a2f1:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  40a2f4:	84 c9                	test   cl,cl
  40a2f6:	8b 4e 70             	mov    ecx,DWORD PTR [esi+0x70]
  40a2f9:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  40a2fc:	ba 01 00 00 00       	mov    edx,0x1
  40a301:	0f 84 c9 00 00 00    	je     0x40a3d0
  40a307:	8b 3d 08 f8 89 00    	mov    edi,DWORD PTR ds:0x89f808
  40a30d:	33 d2                	xor    edx,edx
  40a30f:	8a 91 eb 00 00 00    	mov    dl,BYTE PTR [ecx+0xeb]
  40a315:	3b fa                	cmp    edi,edx
  40a317:	0f 85 96 00 00 00    	jne    0x40a3b3
  40a31d:	80 b9 df 00 00 00 04 	cmp    BYTE PTR [ecx+0xdf],0x4
  40a324:	0f 82 89 00 00 00    	jb     0x40a3b3
  40a32a:	80 79 76 03          	cmp    BYTE PTR [ecx+0x76],0x3
  40a32e:	75 05                	jne    0x40a335
  40a330:	24 00                	and    al,0x0
  40a332:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  40a335:	e8 d6 e8 ff ff       	call   0x408c10
  40a33a:	8b 78 04             	mov    edi,DWORD PTR [eax+0x4]
  40a33d:	e8 ce e8 ff ff       	call   0x408c10
  40a342:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  40a345:	8b 5e 08             	mov    ebx,DWORD PTR [esi+0x8]
  40a348:	8b c1                	mov    eax,ecx
  40a34a:	8b 6e 04             	mov    ebp,DWORD PTR [esi+0x4]
  40a34d:	2b c3                	sub    eax,ebx
  40a34f:	99                   	cdq
  40a350:	33 c2                	xor    eax,edx
  40a352:	2b c2                	sub    eax,edx
  40a354:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  40a358:	8b c7                	mov    eax,edi
  40a35a:	2b c5                	sub    eax,ebp
  40a35c:	99                   	cdq
  40a35d:	33 c2                	xor    eax,edx
  40a35f:	2b c2                	sub    eax,edx
  40a361:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  40a365:	3b c2                	cmp    eax,edx
  40a367:	7e 2c                	jle    0x40a395
  40a369:	8b c5                	mov    eax,ebp
  40a36b:	3b f8                	cmp    edi,eax
  40a36d:	7e 12                	jle    0x40a381
  40a36f:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40a372:	25 ff ff 02 ff       	and    eax,0xff02ffff
  40a377:	0d 00 00 02 00       	or     eax,0x20000
  40a37c:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  40a37f:	eb 7f                	jmp    0x40a400
  40a381:	8b 4e 0c             	mov    ecx,DWORD PTR [esi+0xc]
  40a384:	81 e1 ff ff 06 ff    	and    ecx,0xff06ffff
  40a38a:	81 c9 00 00 06 00    	or     ecx,0x60000
  40a390:	89 4e 0c             	mov    DWORD PTR [esi+0xc],ecx
  40a393:	eb 6b                	jmp    0x40a400
  40a395:	3b cb                	cmp    ecx,ebx
  40a397:	7e 09                	jle    0x40a3a2
  40a399:	81 66 0c ff ff 00 ff 	and    DWORD PTR [esi+0xc],0xff00ffff
  40a3a0:	eb 5e                	jmp    0x40a400
  40a3a2:	8b 56 0c             	mov    edx,DWORD PTR [esi+0xc]
  40a3a5:	81 e2 ff ff 04 ff    	and    edx,0xff04ffff
  40a3ab:	81 ca 00 00 04 00    	or     edx,0x40000
  40a3b1:	eb 4a                	jmp    0x40a3fd
  40a3b3:	8a 46 57             	mov    al,BYTE PTR [esi+0x57]
  40a3b6:	33 d2                	xor    edx,edx
  40a3b8:	88 41 76             	mov    BYTE PTR [ecx+0x76],al
  40a3bb:	8b 4e 70             	mov    ecx,DWORD PTR [esi+0x70]
  40a3be:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40a3c1:	c6 46 54 00          	mov    BYTE PTR [esi+0x54],0x0
  40a3c5:	8a 91 03 01 00 00    	mov    dl,BYTE PTR [ecx+0x103]
  40a3cb:	c1 e2 10             	shl    edx,0x10
  40a3ce:	eb 26                	jmp    0x40a3f6
  40a3d0:	8a 59 76             	mov    bl,BYTE PTR [ecx+0x76]
  40a3d3:	84 db                	test   bl,bl
  40a3d5:	75 29                	jne    0x40a400
  40a3d7:	8a 99 df 00 00 00    	mov    bl,BYTE PTR [ecx+0xdf]
  40a3dd:	84 db                	test   bl,bl
  40a3df:	76 1f                	jbe    0x40a400
  40a3e1:	39 56 40             	cmp    DWORD PTR [esi+0x40],edx
  40a3e4:	75 1a                	jne    0x40a400
  40a3e6:	39 56 3c             	cmp    DWORD PTR [esi+0x3c],edx
  40a3e9:	75 15                	jne    0x40a400
  40a3eb:	33 d2                	xor    edx,edx
  40a3ed:	8a 91 03 01 00 00    	mov    dl,BYTE PTR [ecx+0x103]
  40a3f3:	c1 e2 10             	shl    edx,0x10
  40a3f6:	25 ff ff 00 ff       	and    eax,0xff00ffff
  40a3fb:	0b d0                	or     edx,eax
  40a3fd:	89 56 0c             	mov    DWORD PTR [esi+0xc],edx
  40a400:	8b 7e 70             	mov    edi,DWORD PTR [esi+0x70]
  40a403:	80 7f 76 03          	cmp    BYTE PTR [edi+0x76],0x3
  40a407:	75 14                	jne    0x40a41d
  40a409:	8b 46 58             	mov    eax,DWORD PTR [esi+0x58]
  40a40c:	8b 4e 5c             	mov    ecx,DWORD PTR [esi+0x5c]
  40a40f:	3b c1                	cmp    eax,ecx
  40a411:	74 0a                	je     0x40a41d
  40a413:	80 7e 0c 02          	cmp    BYTE PTR [esi+0xc],0x2
  40a417:	0f 84 db 00 00 00    	je     0x40a4f8
  40a41d:	8b 5e 0c             	mov    ebx,DWORD PTR [esi+0xc]
  40a420:	33 c0                	xor    eax,eax
  40a422:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  40a426:	33 c9                	xor    ecx,ecx
  40a428:	8a 4c 24 1a          	mov    cl,BYTE PTR [esp+0x1a]
  40a42c:	c7                   	.byte 0xc7
  40a42d:	46                   	inc    esi
  40a42e:	58                   	pop    eax
	...
