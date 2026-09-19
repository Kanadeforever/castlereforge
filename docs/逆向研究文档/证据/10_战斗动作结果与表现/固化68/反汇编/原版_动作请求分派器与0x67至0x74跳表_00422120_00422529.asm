
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00422120 <.text+0x21120>:
  422120:	53                   	push   ebx
  422121:	56                   	push   esi
  422122:	8b f1                	mov    esi,ecx
  422124:	32 db                	xor    bl,bl
  422126:	8b 86 4c 08 00 00    	mov    eax,DWORD PTR [esi+0x84c]
  42212c:	85 c0                	test   eax,eax
  42212e:	7d 17                	jge    0x422147
  422130:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  422136:	68 80 9f 46 00       	push   0x469f80
  42213b:	e8 70 b0 01 00       	call   0x43d1b0
  422140:	5e                   	pop    esi
  422141:	32 c0                	xor    al,al
  422143:	5b                   	pop    ebx
  422144:	c2 0c 00             	ret    0xc
  422147:	55                   	push   ebp
  422148:	8b 6c 24 10          	mov    ebp,DWORD PTR [esp+0x10]
  42214c:	57                   	push   edi
  42214d:	8d 4d 99             	lea    ecx,[ebp-0x67]
  422150:	83 f9 0d             	cmp    ecx,0xd
  422153:	0f 87 95 01 00 00    	ja     0x4222ee
  422159:	ff 24 8d d8 24 42 00 	jmp    DWORD PTR [ecx*4+0x4224d8]
  422160:	8d 86 d4 0a 00 00    	lea    eax,[esi+0xad4]
  422166:	6a 00                	push   0x0
  422168:	50                   	push   eax
  422169:	8b ce                	mov    ecx,esi
  42216b:	e8 90 d5 ff ff       	call   0x41f700
  422170:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  422176:	50                   	push   eax
  422177:	e8 34 b0 01 00       	call   0x43d1b0
  42217c:	84 c0                	test   al,al
  42217e:	0f 84 6a 01 00 00    	je     0x4222ee
  422184:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  42218a:	6a 00                	push   0x0
  42218c:	8b 49 20             	mov    ecx,DWORD PTR [ecx+0x20]
  42218f:	e8 0c c5 01 00       	call   0x43e6a0
  422194:	b3 01                	mov    bl,0x1
  422196:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  42219c:	5f                   	pop    edi
  42219d:	5d                   	pop    ebp
  42219e:	8a c3                	mov    al,bl
  4221a0:	5e                   	pop    esi
  4221a1:	5b                   	pop    ebx
  4221a2:	c2 0c 00             	ret    0xc
  4221a5:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4221ab:	85 c0                	test   eax,eax
  4221ad:	74 22                	je     0x4221d1
  4221af:	8d b8 9c 00 00 00    	lea    edi,[eax+0x9c]
  4221b5:	8b ce                	mov    ecx,esi
  4221b7:	57                   	push   edi
  4221b8:	e8 73 d7 ff ff       	call   0x41f930
  4221bd:	84 c0                	test   al,al
  4221bf:	0f 85 29 01 00 00    	jne    0x4222ee
  4221c5:	57                   	push   edi
  4221c6:	8b ce                	mov    ecx,esi
  4221c8:	e8 b3 d7 ff ff       	call   0x41f980
  4221cd:	84 c0                	test   al,al
  4221cf:	74 16                	je     0x4221e7
  4221d1:	8d be 2e 0b 00 00    	lea    edi,[esi+0xb2e]
  4221d7:	8b ce                	mov    ecx,esi
  4221d9:	57                   	push   edi
  4221da:	e8 51 d7 ff ff       	call   0x41f930
  4221df:	84 c0                	test   al,al
  4221e1:	0f 85 07 01 00 00    	jne    0x4222ee
  4221e7:	6a 00                	push   0x0
  4221e9:	57                   	push   edi
  4221ea:	8b ce                	mov    ecx,esi
  4221ec:	e8 0f d5 ff ff       	call   0x41f700
  4221f1:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  4221f7:	50                   	push   eax
  4221f8:	e8 b3 af 01 00       	call   0x43d1b0
  4221fd:	84 c0                	test   al,al
  4221ff:	0f 84 e9 00 00 00    	je     0x4222ee
  422205:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  42220b:	6a 00                	push   0x0
  42220d:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  422210:	e8 8b c4 01 00       	call   0x43e6a0
  422215:	b3 01                	mov    bl,0x1
  422217:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  42221d:	5f                   	pop    edi
  42221e:	5d                   	pop    ebp
  42221f:	8a c3                	mov    al,bl
  422221:	5e                   	pop    esi
  422222:	5b                   	pop    ebx
  422223:	c2 0c 00             	ret    0xc
  422226:	8d be 06 0b 00 00    	lea    edi,[esi+0xb06]
  42222c:	8b ce                	mov    ecx,esi
  42222e:	57                   	push   edi
  42222f:	e8 fc d6 ff ff       	call   0x41f930
  422234:	84 c0                	test   al,al
  422236:	0f 85 b2 00 00 00    	jne    0x4222ee
  42223c:	6a 00                	push   0x0
  42223e:	57                   	push   edi
  42223f:	8b ce                	mov    ecx,esi
  422241:	e8 ba d4 ff ff       	call   0x41f700
  422246:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  42224c:	50                   	push   eax
  42224d:	e8 5e af 01 00       	call   0x43d1b0
  422252:	84 c0                	test   al,al
  422254:	0f 84 94 00 00 00    	je     0x4222ee
  42225a:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  422260:	6a 00                	push   0x0
  422262:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  422265:	e8 36 c4 01 00       	call   0x43e6a0
  42226a:	b3 01                	mov    bl,0x1
  42226c:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  422272:	5f                   	pop    edi
  422273:	5d                   	pop    ebp
  422274:	8a c3                	mov    al,bl
  422276:	5e                   	pop    esi
  422277:	5b                   	pop    ebx
  422278:	c2 0c 00             	ret    0xc
  42227b:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  422281:	81 39 ea 01 00 00    	cmp    DWORD PTR [ecx],0x1ea
  422287:	75 3e                	jne    0x4222c7
  422289:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  42228e:	8b 88 d8 00 00 00    	mov    ecx,DWORD PTR [eax+0xd8]
  422294:	51                   	push   ecx
  422295:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  42229b:	e8 c0 22 00 00       	call   0x424560
  4222a0:	8b f8                	mov    edi,eax
  4222a2:	e8 be f2 02 00       	call   0x451565
  4222a7:	99                   	cdq
  4222a8:	b9 64 00 00 00       	mov    ecx,0x64
  4222ad:	f7 f9                	idiv   ecx
  4222af:	3b d7                	cmp    edx,edi
  4222b1:	7d 14                	jge    0x4222c7
  4222b3:	8b 15 74 fd 89 00    	mov    edx,DWORD PTR ds:0x89fd74
  4222b9:	c6 82 fc 05 00 00 01 	mov    BYTE PTR [edx+0x5fc],0x1
  4222c0:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  4222c7:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  4222cb:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4222cf:	50                   	push   eax
  4222d0:	51                   	push   ecx
  4222d1:	8b ce                	mov    ecx,esi
  4222d3:	c7 86 f4 0c 00 00 00 	mov    DWORD PTR [esi+0xcf4],0x0
  4222da:	00 00 00 
  4222dd:	e8 ce d6 ff ff       	call   0x41f9b0
  4222e2:	8a d8                	mov    bl,al
  4222e4:	84 db                	test   bl,bl
  4222e6:	74 06                	je     0x4222ee
  4222e8:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  4222ee:	5f                   	pop    edi
  4222ef:	5d                   	pop    ebp
  4222f0:	8a c3                	mov    al,bl
  4222f2:	5e                   	pop    esi
  4222f3:	5b                   	pop    ebx
  4222f4:	c2 0c 00             	ret    0xc
  4222f7:	8b 86 e4 0c 00 00    	mov    eax,DWORD PTR [esi+0xce4]
  4222fd:	3d bb 0b 00 00       	cmp    eax,0xbbb
  422302:	74 ea                	je     0x4222ee
  422304:	3d c1 0b 00 00       	cmp    eax,0xbc1
  422309:	74 e3                	je     0x4222ee
  42230b:	8b 96 50 08 00 00    	mov    edx,DWORD PTR [esi+0x850]
  422311:	8d 86 f2 0a 00 00    	lea    eax,[esi+0xaf2]
  422317:	52                   	push   edx
  422318:	6a 00                	push   0x0
  42231a:	eb 59                	jmp    0x422375
  42231c:	8b 86 e4 0c 00 00    	mov    eax,DWORD PTR [esi+0xce4]
  422322:	3d bb 0b 00 00       	cmp    eax,0xbbb
  422327:	74 c5                	je     0x4222ee
  422329:	3d c1 0b 00 00       	cmp    eax,0xbc1
  42232e:	74 be                	je     0x4222ee
  422330:	8b 8e 50 08 00 00    	mov    ecx,DWORD PTR [esi+0x850]
  422336:	8d 96 06 0b 00 00    	lea    edx,[esi+0xb06]
  42233c:	51                   	push   ecx
  42233d:	6a 00                	push   0x0
  42233f:	52                   	push   edx
  422340:	eb 34                	jmp    0x422376
  422342:	8b 86 e4 0c 00 00    	mov    eax,DWORD PTR [esi+0xce4]
  422348:	3d bb 0b 00 00       	cmp    eax,0xbbb
  42234d:	74 9f                	je     0x4222ee
  42234f:	3d c1 0b 00 00       	cmp    eax,0xbc1
  422354:	74 98                	je     0x4222ee
  422356:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  42235c:	8d 86 06 0b 00 00    	lea    eax,[esi+0xb06]
  422362:	85 c9                	test   ecx,ecx
  422364:	74 06                	je     0x42236c
  422366:	8d 81 b0 00 00 00    	lea    eax,[ecx+0xb0]
  42236c:	8b 8e 50 08 00 00    	mov    ecx,DWORD PTR [esi+0x850]
  422372:	51                   	push   ecx
  422373:	6a 00                	push   0x0
  422375:	50                   	push   eax
  422376:	8b ce                	mov    ecx,esi
  422378:	e8 83 d3 ff ff       	call   0x41f700
  42237d:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  422383:	50                   	push   eax
  422384:	e8 77 af 01 00       	call   0x43d300
  422389:	b3 01                	mov    bl,0x1
  42238b:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  422391:	5f                   	pop    edi
  422392:	5d                   	pop    ebp
  422393:	8a c3                	mov    al,bl
  422395:	5e                   	pop    esi
  422396:	5b                   	pop    ebx
  422397:	c2 0c 00             	ret    0xc
  42239a:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  42239e:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4223a2:	52                   	push   edx
  4223a3:	50                   	push   eax
  4223a4:	8b ce                	mov    ecx,esi
  4223a6:	e8 c5 dd ff ff       	call   0x420170
  4223ab:	e9 32 ff ff ff       	jmp    0x4222e2
  4223b0:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4223b6:	6a 0b                	push   0xb
  4223b8:	05 a6 00 00 00       	add    eax,0xa6
  4223bd:	8b ce                	mov    ecx,esi
  4223bf:	50                   	push   eax
  4223c0:	e8 3b d3 ff ff       	call   0x41f700
  4223c5:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  4223cb:	50                   	push   eax
  4223cc:	e8 df ad 01 00       	call   0x43d1b0
  4223d1:	84 c0                	test   al,al
  4223d3:	0f 84 15 ff ff ff    	je     0x4222ee
  4223d9:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  4223dd:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  4223e1:	51                   	push   ecx
  4223e2:	52                   	push   edx
  4223e3:	8b ce                	mov    ecx,esi
  4223e5:	e8 26 e0 ff ff       	call   0x420410
  4223ea:	b3 01                	mov    bl,0x1
  4223ec:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  4223f2:	5f                   	pop    edi
  4223f3:	5d                   	pop    ebp
  4223f4:	8a c3                	mov    al,bl
  4223f6:	5e                   	pop    esi
  4223f7:	5b                   	pop    ebx
  4223f8:	c2 0c 00             	ret    0xc
  4223fb:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  422401:	6a 0c                	push   0xc
  422403:	05 c4 00 00 00       	add    eax,0xc4
  422408:	8b ce                	mov    ecx,esi
  42240a:	50                   	push   eax
  42240b:	e8 f0 d2 ff ff       	call   0x41f700
  422410:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  422416:	50                   	push   eax
  422417:	e8 94 ad 01 00       	call   0x43d1b0
  42241c:	84 c0                	test   al,al
  42241e:	0f 84 ca fe ff ff    	je     0x4222ee
  422424:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  422428:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42242c:	50                   	push   eax
  42242d:	51                   	push   ecx
  42242e:	8b ce                	mov    ecx,esi
  422430:	e8 db df ff ff       	call   0x420410
  422435:	b3 01                	mov    bl,0x1
  422437:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  42243d:	5f                   	pop    edi
  42243e:	5d                   	pop    ebp
  42243f:	8a c3                	mov    al,bl
  422441:	5e                   	pop    esi
  422442:	5b                   	pop    ebx
  422443:	c2 0c 00             	ret    0xc
  422446:	8d 14 80             	lea    edx,[eax+eax*4]
  422449:	6a 00                	push   0x0
  42244b:	8b ce                	mov    ecx,esi
  42244d:	8d 04 95 58 9d 46 00 	lea    eax,[edx*4+0x469d58]
  422454:	50                   	push   eax
  422455:	e8 a6 d2 ff ff       	call   0x41f700
  42245a:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  422460:	50                   	push   eax
  422461:	e8 4a ad 01 00       	call   0x43d1b0
  422466:	84 c0                	test   al,al
  422468:	0f 84 80 fe ff ff    	je     0x4222ee
  42246e:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  422474:	6a 00                	push   0x0
  422476:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  422479:	e8 f2 c1 01 00       	call   0x43e670
  42247e:	b3 01                	mov    bl,0x1
  422480:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  422486:	5f                   	pop    edi
  422487:	5d                   	pop    ebp
  422488:	8a c3                	mov    al,bl
  42248a:	5e                   	pop    esi
  42248b:	5b                   	pop    ebx
  42248c:	c2 0c 00             	ret    0xc
  42248f:	8d 0c 80             	lea    ecx,[eax+eax*4]
  422492:	6a 00                	push   0x0
  422494:	8d 04 8d f8 9d 46 00 	lea    eax,[ecx*4+0x469df8]
  42249b:	8b ce                	mov    ecx,esi
  42249d:	50                   	push   eax
  42249e:	e8 5d d2 ff ff       	call   0x41f700
  4224a3:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  4224a9:	50                   	push   eax
  4224aa:	e8 01 ad 01 00       	call   0x43d1b0
  4224af:	84 c0                	test   al,al
  4224b1:	0f 84 37 fe ff ff    	je     0x4222ee
  4224b7:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  4224bd:	6a 00                	push   0x0
  4224bf:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  4224c2:	e8 a9 c1 01 00       	call   0x43e670
  4224c7:	b3 01                	mov    bl,0x1
  4224c9:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  4224cf:	5f                   	pop    edi
  4224d0:	5d                   	pop    ebp
  4224d1:	8a c3                	mov    al,bl
  4224d3:	5e                   	pop    esi
  4224d4:	5b                   	pop    ebx
  4224d5:	c2 0c 00             	ret    0xc
  4224d8:	60                   	pusha
  4224d9:	21 42 00             	and    DWORD PTR [edx+0x0],eax
  4224dc:	a5                   	movs   DWORD PTR es:[edi],DWORD PTR ds:[esi]
  4224dd:	21 42 00             	and    DWORD PTR [edx+0x0],eax
  4224e0:	26 22 42 00          	and    al,BYTE PTR es:[edx+0x0]
  4224e4:	7b 22                	jnp    0x422508
  4224e6:	42                   	inc    edx
  4224e7:	00 ee                	add    dh,ch
  4224e9:	22 42 00             	and    al,BYTE PTR [edx+0x0]
  4224ec:	f7 22                	mul    DWORD PTR [edx]
  4224ee:	42                   	inc    edx
  4224ef:	00 1c 23             	add    BYTE PTR [ebx+eiz*1],bl
  4224f2:	42                   	inc    edx
  4224f3:	00 9a 23 42 00 ee    	add    BYTE PTR [edx-0x11ffbddd],bl
  4224f9:	22 42 00             	and    al,BYTE PTR [edx+0x0]
  4224fc:	b0 23                	mov    al,0x23
  4224fe:	42                   	inc    edx
  4224ff:	00 fb                	add    bl,bh
  422501:	23 42 00             	and    eax,DWORD PTR [edx+0x0]
  422504:	42                   	inc    edx
  422505:	23 42 00             	and    eax,DWORD PTR [edx+0x0]
  422508:	46                   	inc    esi
  422509:	24 42                	and    al,0x42
  42250b:	00 8f 24 42 00 8b    	add    BYTE PTR [edi-0x74ffbddc],cl
  422511:	81 a4 0b 00 00 8b 4c 	and    DWORD PTR [ebx+ecx*1+0x4c8b0000],0x8b510424
  422518:	24 04 51 8b 
  42251c:	40                   	inc    eax
  42251d:	20 8b 88 5c 03 00    	and    BYTE PTR [ebx+0x35c88],cl
  422523:	00 e8                	add    al,ch
  422525:	77 7c                	ja     0x4225a3
	...
