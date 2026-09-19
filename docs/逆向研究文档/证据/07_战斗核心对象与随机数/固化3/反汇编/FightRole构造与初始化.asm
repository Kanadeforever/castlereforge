
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f050 <.text+0x1e050>:
  41f050:	00 b9 03 00 00 00    	add    BYTE PTR [ecx+0x3],bh
  41f056:	8b 10                	mov    edx,DWORD PTR [eax]
  41f058:	83 c0 04             	add    eax,0x4
  41f05b:	49                   	dec    ecx
  41f05c:	c6 42 28 00          	mov    BYTE PTR [edx+0x28],0x0
  41f060:	75 f4                	jne    0x41f056
  41f062:	5e                   	pop    esi
  41f063:	83 c4 08             	add    esp,0x8
  41f066:	c3                   	ret
  41f067:	90                   	nop
  41f068:	90                   	nop
  41f069:	90                   	nop
  41f06a:	90                   	nop
  41f06b:	90                   	nop
  41f06c:	90                   	nop
  41f06d:	90                   	nop
  41f06e:	90                   	nop
  41f06f:	90                   	nop
  41f070:	6a ff                	push   0xffffffff
  41f072:	68 8a e2 45 00       	push   0x45e28a
  41f077:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41f07d:	50                   	push   eax
  41f07e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41f085:	51                   	push   ecx
  41f086:	53                   	push   ebx
  41f087:	55                   	push   ebp
  41f088:	56                   	push   esi
  41f089:	8b f1                	mov    esi,ecx
  41f08b:	57                   	push   edi
  41f08c:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  41f090:	8d 8e 30 07 00 00    	lea    ecx,[esi+0x730]
  41f096:	e8 95 57 02 00       	call   0x444830
  41f09b:	33 db                	xor    ebx,ebx
  41f09d:	8d 8e b4 07 00 00    	lea    ecx,[esi+0x7b4]
  41f0a3:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
  41f0a7:	e8 74 60 02 00       	call   0x445120
  41f0ac:	8d 8e a8 0c 00 00    	lea    ecx,[esi+0xca8]
  41f0b2:	c6 44 24 1c 01       	mov    BYTE PTR [esp+0x1c],0x1
  41f0b7:	e8 c4 4c 00 00       	call   0x423d80
  41f0bc:	83 cd ff             	or     ebp,0xffffffff
  41f0bf:	88 9e d8 0d 00 00    	mov    BYTE PTR [esi+0xdd8],bl
  41f0c5:	89 9e 04 0d 00 00    	mov    DWORD PTR [esi+0xd04],ebx
  41f0cb:	89 ae d0 0d 00 00    	mov    DWORD PTR [esi+0xdd0],ebp
  41f0d1:	89 ae d4 0d 00 00    	mov    DWORD PTR [esi+0xdd4],ebp
  41f0d7:	c7 86 fc 0c 00 00 04 	mov    DWORD PTR [esi+0xcfc],0x4
  41f0de:	00 00 00 
  41f0e1:	a0 f4 fd 89 00       	mov    al,ds:0x89fdf4
  41f0e6:	c6 44 24 1c 02       	mov    BYTE PTR [esp+0x1c],0x2
  41f0eb:	3a c3                	cmp    al,bl
  41f0ed:	75 15                	jne    0x41f104
  41f0ef:	b9 10 00 00 00       	mov    ecx,0x10
  41f0f4:	33 c0                	xor    eax,eax
  41f0f6:	bf 94 fd 89 00       	mov    edi,0x89fd94
  41f0fb:	c6 05 f4 fd 89 00 01 	mov    BYTE PTR ds:0x89fdf4,0x1
  41f102:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  41f104:	8b ce                	mov    ecx,esi
  41f106:	e8 25 01 00 00       	call   0x41f230
  41f10b:	33 c0                	xor    eax,eax
  41f10d:	c7 86 e0 0c 00 00 d1 	mov    DWORD PTR [esi+0xce0],0x7d1
  41f114:	07 00 00 
  41f117:	c7 86 e4 0c 00 00 b9 	mov    DWORD PTR [esi+0xce4],0xbb9
  41f11e:	0b 00 00 
  41f121:	c7 86 e8 0c 00 00 a0 	mov    DWORD PTR [esi+0xce8],0xfa0
  41f128:	0f 00 00 
  41f12b:	89 9e f0 0c 00 00    	mov    DWORD PTR [esi+0xcf0],ebx
  41f131:	89 9e c8 00 00 00    	mov    DWORD PTR [esi+0xc8],ebx
  41f137:	89 9e 68 01 00 00    	mov    DWORD PTR [esi+0x168],ebx
  41f13d:	89 9e 6c 01 00 00    	mov    DWORD PTR [esi+0x16c],ebx
  41f143:	88 9e f8 0c 00 00    	mov    BYTE PTR [esi+0xcf8],bl
  41f149:	89 1e                	mov    DWORD PTR [esi],ebx
  41f14b:	88 5e 04             	mov    BYTE PTR [esi+0x4],bl
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
