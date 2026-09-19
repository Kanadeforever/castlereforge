
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f140 <.text+0x1e140>:
  41f140:	01 00                	add    DWORD PTR [eax],eax
  41f142:	00 88 9e f8 0c 00    	add    BYTE PTR [eax+0xcf89e],cl
  41f148:	00 89 1e 88 5e 04    	add    BYTE PTR [ecx+0x45e881e],cl
  41f14e:	89 ae e0 0d 00 00    	mov    DWORD PTR [esi+0xde0],ebp
  41f154:	89 ae 1c 07 00 00    	mov    DWORD PTR [esi+0x71c],ebp
  41f15a:	89 9e 20 07 00 00    	mov    DWORD PTR [esi+0x720],ebx
  41f160:	89 86 10 07 00 00    	mov    DWORD PTR [esi+0x710],eax
  41f166:	b9 d4 00 00 00       	mov    ecx,0xd4
  41f16b:	89 86 14 07 00 00    	mov    DWORD PTR [esi+0x714],eax
  41f171:	8d be 4c 08 00 00    	lea    edi,[esi+0x84c]
  41f177:	66 89 86 18 07 00 00 	mov    WORD PTR [esi+0x718],ax
  41f17e:	88 86 1a 07 00 00    	mov    BYTE PTR [esi+0x71a],al
  41f184:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  41f186:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  41f18a:	8b c6                	mov    eax,esi
  41f18c:	5f                   	pop    edi
  41f18d:	5e                   	pop    esi
  41f18e:	5d                   	pop    ebp
  41f18f:	5b                   	pop    ebx
  41f190:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41f197:	83 c4 10             	add    esp,0x10
  41f19a:	c3                   	ret
  41f19b:	90                   	nop
  41f19c:	90                   	nop
  41f19d:	90                   	nop
  41f19e:	90                   	nop
  41f19f:	90                   	nop
  41f1a0:	6a ff                	push   0xffffffff
  41f1a2:	68 ca e2 45 00       	push   0x45e2ca
  41f1a7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41f1ad:	50                   	push   eax
  41f1ae:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41f1b5:	51                   	push   ecx
  41f1b6:	56                   	push   esi
  41f1b7:	8b f1                	mov    esi,ecx
  41f1b9:	89 74 24 04          	mov    DWORD PTR [esp+0x4],esi
  41f1bd:	8b 06                	mov    eax,DWORD PTR [esi]
  41f1bf:	c7 44 24 10 02 00 00 	mov    DWORD PTR [esp+0x10],0x2
  41f1c6:	00 
  41f1c7:	85 c0                	test   eax,eax
  41f1c9:	7c 10                	jl     0x41f1db
  41f1cb:	83 f8 10             	cmp    eax,0x10
  41f1ce:	7d 0b                	jge    0x41f1db
  41f1d0:	c7 04 85 94 fd 89 00 	mov    DWORD PTR [eax*4+0x89fd94],0x0
  41f1d7:	00 00 00 00 
  41f1db:	8b ce                	mov    ecx,esi
  41f1dd:	e8 4e 00 00 00       	call   0x41f230
  41f1e2:	8d 8e a8 0c 00 00    	lea    ecx,[esi+0xca8]
  41f1e8:	c6 44 24 10 01       	mov    BYTE PTR [esp+0x10],0x1
  41f1ed:	e8 9e 4b 00 00       	call   0x423d90
  41f1f2:	8d 8e b4 07 00 00    	lea    ecx,[esi+0x7b4]
  41f1f8:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
  41f1fd:	e8 2e 5f 02 00       	call   0x445130
  41f202:	8d 8e 30 07 00 00    	lea    ecx,[esi+0x730]
  41f208:	c7 44 24 10 ff ff ff 	mov    DWORD PTR [esp+0x10],0xffffffff
  41f20f:	ff 
  41f210:	e8 5b 9e ff ff       	call   0x419070
  41f215:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  41f219:	5e                   	pop    esi
  41f21a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41f221:	83 c4 10             	add    esp,0x10
  41f224:	c3                   	ret
  41f225:	90                   	nop
  41f226:	90                   	nop
  41f227:	90                   	nop
  41f228:	90                   	nop
  41f229:	90                   	nop
  41f22a:	90                   	nop
  41f22b:	90                   	nop
  41f22c:	90                   	nop
  41f22d:	90                   	nop
  41f22e:	90                   	nop
  41f22f:	90                   	nop
  41f230:	53                   	push   ebx
  41f231:	56                   	push   esi
  41f232:	8b f1                	mov    esi,ecx
  41f234:	33 db                	xor    ebx,ebx
  41f236:	57                   	push   edi
  41f237:	c7 86 2c 07 00 00 ff 	mov    DWORD PTR [esi+0x72c],0xffffffff
  41f23e:	ff ff ff 
  41f241:	89 9e 9c 0b 00 00    	mov    DWORD PTR [esi+0xb9c],ebx
  41f247:	89 9e 24 07 00 00    	mov    DWORD PTR [esi+0x724],ebx
  41f24d:	c7 86 a4 07 00 00 e9 	mov    DWORD PTR [esi+0x7a4],0x3e9
  41f254:	03 00 00 
  41f257:	c7 86 e8 0c 00 00 a0 	mov    DWORD PTR [esi+0xce8],0xfa0
  41f25e:	0f 00 00 
  41f261:	89 9e 48 08 00 00    	mov    DWORD PTR [esi+0x848],ebx
  41f267:	89 9e a4 0b 00 00    	mov    DWORD PTR [esi+0xba4],ebx
  41f26d:	88 5e 04             	mov    BYTE PTR [esi+0x4],bl
  41f270:	c6 86 b0 07 00 00 01 	mov    BYTE PTR [esi+0x7b0],0x1
  41f277:	88 9e 28 07 00 00    	mov    BYTE PTR [esi+0x728],bl
  41f27d:	e8 4e 3f 00 00       	call   0x4231d0
  41f282:	33 c0                	xor    eax,eax
  41f284:	8d 8e cc 00 00 00    	lea    ecx,[esi+0xcc]
  41f28a:	89 19                	mov    DWORD PTR [ecx],ebx
  41f28c:	88 9c 06 e8 06 00 00 	mov    BYTE PTR [esi+eax*1+0x6e8],bl
  41f293:	40                   	inc    eax
  41f294:	83 c1 04             	add    ecx,0x4
  41f297:	83 f8 27             	cmp    eax,0x27
  41f29a:	7c ee                	jl     0x41f28a
  41f29c:	8d be 70 06 00 00    	lea    edi,[esi+0x670]
  41f2a2:	b9 0f 00 00 00       	mov    ecx,0xf
  41f2a7:	83 c8 ff             	or     eax,0xffffffff
  41f2aa:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  41f2ac:	5f                   	pop    edi
  41f2ad:	5e                   	pop    esi
  41f2ae:	5b                   	pop    ebx
  41f2af:	c3                   	ret
  41f2b0:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41f2b4:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  41f2b8:	56                   	push   esi
  41f2b9:	8b f1                	mov    esi,ecx
  41f2bb:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  41f2bf:	68 08 fe 89 00       	push   0x89fe08
  41f2c4:	89 86 a4 0b 00 00    	mov    DWORD PTR [esi+0xba4],eax
  41f2ca:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41f2ce:	89 8e 2c 07 00 00    	mov    DWORD PTR [esi+0x72c],ecx
  41f2d4:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  41f2d8:	89 16                	mov    DWORD PTR [esi],edx
  41f2da:	89 86 9c 0b 00 00    	mov    DWORD PTR [esi+0xb9c],eax
  41f2e0:	89 86 a0 0b 00 00    	mov    DWORD PTR [esi+0xba0],eax
  41f2e6:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  41f2ec:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  41f2f0:	89 0d ec fd 89 00    	mov    DWORD PTR ds:0x89fdec,ecx
  41f2f6:	68 d0 ff 89 00       	push   0x89ffd0
  41f2fb:	52                   	push   edx
  41f2fc:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  41f301:	a3 f0 fd 89 00       	mov    ds:0x89fdf0,eax
  41f306:	e8 55 b4 00 00       	call   0x42a760
  41f30b:	8b ce                	mov    ecx,esi
  41f30d:	e8 6e 02 00 00       	call   0x41f580
  41f312:	84 c0                	test   al,al
  41f314:	0f 84 5d 02 00 00    	je     0x41f577
  41f31a:	6a 00                	push   0x0
  41f31c:	6a 00                	push   0x0
  41f31e:	6a 67                	push   0x67
  41f320:	8b ce                	mov    ecx,esi
  41f322:	e8 f9 2d 00 00       	call   0x422120
  41f327:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  41f32d:	c6 86 28 07 00 00 01 	mov    BYTE PTR [esi+0x728],0x1
  41f334:	6a 01                	push   0x1
  41f336:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  41f339:	e8 c2 01 02 00       	call   0x43f500
  41f33e:	e8 22 22 03 00       	call   0x451565
  41f343:	25 01 00 00 80       	and    eax,0x80000001
  41f348:	79 05                	jns    0x41f34f
  41f34a:	48                   	dec    eax
  41f34b:	83 c8 fe             	or     eax,0xfffffffe
  41f34e:	40                   	inc    eax
  41f34f:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  41f355:	50                   	push   eax
  41f356:	8b 49 20             	mov    ecx,DWORD PTR [ecx+0x20]
  41f359:	e8 42 f3 01 00       	call   0x43e6a0
  41f35e:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  41f364:	85 c0                	test   eax,eax
  41f366:	75 0c                	jne    0x41f374
  41f368:	68 0a ff ff ff       	push   0xffffff0a
  41f36d:	68 db fe ff ff       	push   0xfffffedb
  41f372:	eb 0f                	jmp    0x41f383
  41f374:	83 f8 01             	cmp    eax,0x1
  41f377:	75 15                	jne    0x41f38e
  41f379:	68 ef fe ff ff       	push   0xfffffeef
  41f37e:	68 a6 fe ff ff       	push   0xfffffea6
  41f383:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  41f389:	e8 a2 de 01 00       	call   0x43d230
  41f38e:	8b 86 9c 0b 00 00    	mov    eax,DWORD PTR [esi+0xb9c]
  41f394:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  41f39a:	c1 e0 02             	shl    eax,0x2
  41f39d:	57                   	push   edi
  41f39e:	8b 0c 10             	mov    ecx,DWORD PTR [eax+edx*1]
  41f3a1:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  41f3a7:	51                   	push   ecx
  41f3a8:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  41f3ae:	8b 04 10             	mov    eax,DWORD PTR [eax+edx*1]
  41f3b1:	50                   	push   eax
  41f3b2:	e8 99 de 01 00       	call   0x43d250
  41f3b7:	8d be a8 0c 00 00    	lea    edi,[esi+0xca8]
  41f3bd:	8b cf                	mov    ecx,edi
  41f3bf:	e8 dc 49 00 00       	call   0x423da0
  41f3c4:	68 80 af 02 00       	push   0x2af80
  41f3c9:	6a 01                	push   0x1
  41f3cb:	8b cf                	mov    ecx,edi
  41f3cd:	e8 6e 4b 00 00       	call   0x423f40
  41f3d2:	68 80 af 02 00       	push   0x2af80
  41f3d7:	6a 01                	push   0x1
  41f3d9:	6a 0c                	push   0xc
  41f3db:	8b cf                	mov    ecx,edi
  41f3dd:	e8 ae 4b 00 00       	call   0x423f90
  41f3e2:	8b 8e 94 08 00 00    	mov    ecx,DWORD PTR [esi+0x894]
  41f3e8:	8d 14 cd 4c 04 00 00 	lea    edx,[ecx*8+0x44c]
  41f3ef:	8b cf                	mov    ecx,edi
  41f3f1:	52                   	push   edx
  41f3f2:	6a 01                	push   0x1
  41f3f4:	6a 0b                	push   0xb
  41f3f6:	e8 95 4b 00 00       	call   0x423f90
  41f3fb:	8b 86 94 08 00 00    	mov    eax,DWORD PTR [esi+0x894]
  41f401:	8d 0c 85 4c 04 00 00 	lea    ecx,[eax*4+0x44c]
  41f408:	51                   	push   ecx
  41f409:	6a 02                	push   0x2
  41f40b:	6a 0b                	push   0xb
  41f40d:	8b cf                	mov    ecx,edi
  41f40f:	e8 7c 4b 00 00       	call   0x423f90
  41f414:	68 30 75 00 00       	push   0x7530
  41f419:	6a 02                	push   0x2
  41f41b:	6a 0c                	push   0xc
  41f41d:	8b cf                	mov    ecx,edi
  41f41f:	e8 6c 4b 00 00       	call   0x423f90
  41f424:	8b 96 94 08 00 00    	mov    edx,DWORD PTR [esi+0x894]
  41f42a:	8b cf                	mov    ecx,edi
  41f42c:	8d 04 95 4c 04 00 00 	lea    eax,[edx*4+0x44c]
  41f433:	50                   	push   eax
  41f434:	6a 03                	push   0x3
  41f436:	6a 0b                	push   0xb
  41f438:	e8 53 4b 00 00       	call   0x423f90
  41f43d:	68 90 5f 01 00       	push   0x15f90
  41f442:	6a 03                	push   0x3
  41f444:	6a 0c                	push   0xc
  41f446:	8b cf                	mov    ecx,edi
  41f448:	e8 43 4b 00 00       	call   0x423f90
  41f44d:	8b cf                	mov    ecx,edi
  41f44f:	e8 ec 49 00 00       	call   0x423e40
  41f454:	e8 0c 21 03 00       	call   0x451565
  41f459:	99                   	cdq
  41f45a:	b9 40 9c 00 00       	mov    ecx,0x9c40
  41f45f:	f7 f9                	idiv   ecx
  41f461:	8b cf                	mov    ecx,edi
  41f463:	81 c2 80 38 01 00    	add    edx,0x13880
  41f469:	52                   	push   edx
  41f46a:	6a 01                	push   0x1
  41f46c:	6a 0a                	push   0xa
  41f46e:	e8 1d 4b 00 00       	call   0x423f90
  41f473:	8b 06                	mov    eax,DWORD PTR [esi]
  41f475:	83 f8 10             	cmp    eax,0x10
  41f478:	7d 07                	jge    0x41f481
  41f47a:	89 34 85 94 fd 89 00 	mov    DWORD PTR [eax*4+0x89fd94],esi
  41f481:	8b 86 2c 07 00 00    	mov    eax,DWORD PTR [esi+0x72c]
  41f487:	c7 86 e8 08 00 00 00 	mov    DWORD PTR [esi+0x8e8],0x0
  41f48e:	00 00 00 
  41f491:	83 f8 0a             	cmp    eax,0xa
  41f494:	0f 8d d6 00 00 00    	jge    0x41f570
  41f49a:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f4a0:	6a ff                	push   0xffffffff
  41f4a2:	6a ff                	push   0xffffffff
  41f4a4:	8d be 4c 08 00 00    	lea    edi,[esi+0x84c]
  41f4aa:	6a ff                	push   0xffffffff
  41f4ac:	6a 00                	push   0x0
  41f4ae:	57                   	push   edi
  41f4af:	e8 6c c7 01 00       	call   0x43bc20
  41f4b4:	6a ff                	push   0xffffffff
  41f4b6:	6a ff                	push   0xffffffff
  41f4b8:	6a ff                	push   0xffffffff
  41f4ba:	89 86 b4 08 00 00    	mov    DWORD PTR [esi+0x8b4],eax
  41f4c0:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f4c6:	6a 01                	push   0x1
  41f4c8:	57                   	push   edi
  41f4c9:	e8 52 c7 01 00       	call   0x43bc20
  41f4ce:	6a ff                	push   0xffffffff
  41f4d0:	6a ff                	push   0xffffffff
  41f4d2:	6a ff                	push   0xffffffff
  41f4d4:	89 86 b8 08 00 00    	mov    DWORD PTR [esi+0x8b8],eax
  41f4da:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f4e0:	6a 02                	push   0x2
  41f4e2:	57                   	push   edi
  41f4e3:	e8 38 c7 01 00       	call   0x43bc20
  41f4e8:	6a ff                	push   0xffffffff
  41f4ea:	6a ff                	push   0xffffffff
  41f4ec:	6a ff                	push   0xffffffff
  41f4ee:	89 86 bc 08 00 00    	mov    DWORD PTR [esi+0x8bc],eax
  41f4f4:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f4fa:	6a 03                	push   0x3
  41f4fc:	57                   	push   edi
  41f4fd:	e8 1e c7 01 00       	call   0x43bc20
  41f502:	6a ff                	push   0xffffffff
  41f504:	6a ff                	push   0xffffffff
  41f506:	6a ff                	push   0xffffffff
  41f508:	89 86 c0 08 00 00    	mov    DWORD PTR [esi+0x8c0],eax
  41f50e:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f514:	6a 04                	push   0x4
  41f516:	57                   	push   edi
  41f517:	e8 04 c7 01 00       	call   0x43bc20
  41f51c:	6a ff                	push   0xffffffff
  41f51e:	6a ff                	push   0xffffffff
  41f520:	6a ff                	push   0xffffffff
  41f522:	89 86 c4 08 00 00    	mov    DWORD PTR [esi+0x8c4],eax
  41f528:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f52e:	6a 05                	push   0x5
  41f530:	57                   	push   edi
  41f531:	e8 ea c6 01 00       	call   0x43bc20
  41f536:	6a ff                	push   0xffffffff
  41f538:	6a ff                	push   0xffffffff
  41f53a:	6a ff                	push   0xffffffff
  41f53c:	89 86 c8 08 00 00    	mov    DWORD PTR [esi+0x8c8],eax
  41f542:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f548:	6a 06                	push   0x6
  41f54a:	57                   	push   edi
  41f54b:	e8 d0 c6 01 00       	call   0x43bc20
  41f550:	6a ff                	push   0xffffffff
  41f552:	6a ff                	push   0xffffffff
  41f554:	6a ff                	push   0xffffffff
  41f556:	89 86 cc 08 00 00    	mov    DWORD PTR [esi+0x8cc],eax
  41f55c:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  41f562:	6a 07                	push   0x7
  41f564:	57                   	push   edi
  41f565:	e8 b6 c6 01 00       	call   0x43bc20
  41f56a:	89 86 d0 08 00 00    	mov    DWORD PTR [esi+0x8d0],eax
  41f570:	5f                   	pop    edi
  41f571:	b0 01                	mov    al,0x1
  41f573:	5e                   	pop    esi
  41f574:	c2 18 00             	ret    0x18
  41f577:	32 c0                	xor    al,al
  41f579:	5e                   	pop    esi
  41f57a:	c2 18 00             	ret    0x18
  41f57d:	90                   	nop
  41f57e:	90                   	nop
  41f57f:	90                   	nop
  41f580:	6a ff                	push   0xffffffff
  41f582:	68 e8 e2 45 00       	push   0x45e2e8
  41f587:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41f58d:	50                   	push   eax
  41f58e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41f595:	83 ec 14             	sub    esp,0x14
  41f598:	53                   	push   ebx
  41f599:	56                   	push   esi
  41f59a:	57                   	push   edi
  41f59b:	8b f9                	mov    edi,ecx
  41f59d:	6a 01                	push   0x1
  41f59f:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  41f5a3:	e8 48 21 02 00       	call   0x4416f0
  41f5a8:	33 f6                	xor    esi,esi
  41f5aa:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f5ae:	56                   	push   esi
  41f5af:	6a 01                	push   0x1
  41f5b1:	68 2c 9f 46 00       	push   0x469f2c
  41f5b6:	c7 44 24 34 00 00 00 	mov    DWORD PTR [esp+0x34],0x0
  41f5bd:	00 
  41f5be:	e8 fd 21 02 00       	call   0x4417c0
  41f5c3:	84 c0                	test   al,al
  41f5c5:	75 0a                	jne    0x41f5d1
  41f5c7:	68 10 9f 46 00       	push   0x469f10
  41f5cc:	e9 ad 00 00 00       	jmp    0x41f67e
  41f5d1:	8b 87 2c 07 00 00    	mov    eax,DWORD PTR [edi+0x72c]
  41f5d7:	85 c0                	test   eax,eax
  41f5d9:	7c 37                	jl     0x41f612
  41f5db:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f5df:	e8 4c d8 00 00       	call   0x42ce30
  41f5e4:	8b 8f 2c 07 00 00    	mov    ecx,DWORD PTR [edi+0x72c]
  41f5ea:	8b f0                	mov    esi,eax
  41f5ec:	8d 04 49             	lea    eax,[ecx+ecx*2]
  41f5ef:	8d 04 c0             	lea    eax,[eax+eax*8]
  41f5f2:	d1 e0                	shl    eax,1
  41f5f4:	2b c1                	sub    eax,ecx
  41f5f6:	c1 e0 04             	shl    eax,0x4
  41f5f9:	05 50 03 00 00       	add    eax,0x350
  41f5fe:	3b c6                	cmp    eax,esi
  41f600:	7e 10                	jle    0x41f612
  41f602:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f606:	e8 f5 23 02 00       	call   0x441a00
  41f60b:	68 e4 9e 46 00       	push   0x469ee4
  41f610:	eb 6c                	jmp    0x41f67e
  41f612:	56                   	push   esi
  41f613:	e8 47 20 03 00       	call   0x45165f
  41f618:	83 c4 04             	add    esp,0x4
  41f61b:	8b d8                	mov    ebx,eax
  41f61d:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f621:	53                   	push   ebx
  41f622:	56                   	push   esi
  41f623:	6a 00                	push   0x0
  41f625:	e8 06 24 02 00       	call   0x441a30
  41f62a:	56                   	push   esi
  41f62b:	53                   	push   ebx
  41f62c:	e8 9f 8e 00 00       	call   0x4284d0
  41f631:	83 c4 08             	add    esp,0x8
  41f634:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f638:	e8 c3 23 02 00       	call   0x441a00
  41f63d:	8b 87 2c 07 00 00    	mov    eax,DWORD PTR [edi+0x72c]
  41f643:	8d 97 4c 08 00 00    	lea    edx,[edi+0x84c]
  41f649:	85 c0                	test   eax,eax
  41f64b:	89 02                	mov    DWORD PTR [edx],eax
  41f64d:	7c 63                	jl     0x41f6b2
  41f64f:	8d 0c 40             	lea    ecx,[eax+eax*2]
  41f652:	8b fa                	mov    edi,edx
  41f654:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  41f657:	d1 e1                	shl    ecx,1
  41f659:	2b c8                	sub    ecx,eax
  41f65b:	c1 e1 04             	shl    ecx,0x4
  41f65e:	8d 34 0b             	lea    esi,[ebx+ecx*1]
  41f661:	b9 d4 00 00 00       	mov    ecx,0xd4
  41f666:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  41f668:	39 02                	cmp    DWORD PTR [edx],eax
  41f66a:	74 57                	je     0x41f6c3
  41f66c:	85 db                	test   ebx,ebx
  41f66e:	74 09                	je     0x41f679
  41f670:	53                   	push   ebx
  41f671:	e8 da 1e 03 00       	call   0x451550
  41f676:	83 c4 04             	add    esp,0x4
  41f679:	68 ac 9e 46 00       	push   0x469eac
  41f67e:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  41f684:	6a 00                	push   0x0
  41f686:	e8 e5 27 01 00       	call   0x431e70
  41f68b:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f68f:	8a d8                	mov    bl,al
  41f691:	c7 44 24 28 ff ff ff 	mov    DWORD PTR [esp+0x28],0xffffffff
  41f698:	ff 
  41f699:	e8 72 20 02 00       	call   0x441710
  41f69e:	5f                   	pop    edi
  41f69f:	8a c3                	mov    al,bl
  41f6a1:	5e                   	pop    esi
  41f6a2:	5b                   	pop    ebx
  41f6a3:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  41f6a7:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41f6ae:	83 c4 20             	add    esp,0x20
  41f6b1:	c3                   	ret
  41f6b2:	b9 d4 00 00 00       	mov    ecx,0xd4
  41f6b7:	8b f3                	mov    esi,ebx
  41f6b9:	8b fa                	mov    edi,edx
  41f6bb:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  41f6bd:	c7 02 ff ff ff ff    	mov    DWORD PTR [edx],0xffffffff
  41f6c3:	85 db                	test   ebx,ebx
  41f6c5:	74 09                	je     0x41f6d0
  41f6c7:	53                   	push   ebx
  41f6c8:	e8 83 1e 03 00       	call   0x451550
  41f6cd:	83 c4 04             	add    esp,0x4
