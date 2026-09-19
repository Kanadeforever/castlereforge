
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f2b0 <.text+0x1e2b0>:
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
  41f48e:	00 00  
