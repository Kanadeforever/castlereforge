
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004212ce <.text+0x202ce>:
  4212ce:	83 f8 02             	cmp    eax,0x2
  4212d1:	0f 85 ab 03 00 00    	jne    0x421682
  4212d7:	50                   	push   eax
  4212d8:	8b cf                	mov    ecx,edi
  4212da:	89 2d 84 fd 89 00    	mov    DWORD PTR ds:0x89fd84,ebp
  4212e0:	e8 1b 07 00 00       	call   0x421a00
  4212e5:	6a 02                	push   0x2
  4212e7:	8b cf                	mov    ecx,edi
  4212e9:	e8 a2 03 00 00       	call   0x421690
  4212ee:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  4212f4:	3b c5                	cmp    eax,ebp
  4212f6:	0f 8e 86 03 00 00    	jle    0x421682
  4212fc:	8d 87 88 00 00 00    	lea    eax,[edi+0x88]
  421302:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  421306:	8d 9f 74 01 00 00    	lea    ebx,[edi+0x174]
  42130c:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  421310:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  421314:	8b 31                	mov    esi,DWORD PTR [ecx]
  421316:	3b f5                	cmp    esi,ebp
  421318:	0f 8c 3f 03 00 00    	jl     0x42165d
  42131e:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  421325:	39 aa 2c 07 00 00    	cmp    DWORD PTR [edx+0x72c],ebp
  42132b:	7d 0b                	jge    0x421338
  42132d:	8a 43 3c             	mov    al,BYTE PTR [ebx+0x3c]
  421330:	84 c0                	test   al,al
  421332:	0f 84 25 03 00 00    	je     0x42165d
  421338:	56                   	push   esi
  421339:	6a 03                	push   0x3
  42133b:	8b cf                	mov    ecx,edi
  42133d:	e8 2e 1d 00 00       	call   0x423070
  421342:	84 c0                	test   al,al
  421344:	74 1a                	je     0x421360
  421346:	a1 84 fd 89 00       	mov    eax,ds:0x89fd84
  42134b:	8b 13                	mov    edx,DWORD PTR [ebx]
  42134d:	03 c2                	add    eax,edx
  42134f:	a3 84 fd 89 00       	mov    ds:0x89fd84,eax
  421354:	8b 4b 04             	mov    ecx,DWORD PTR [ebx+0x4]
  421357:	03 c1                	add    eax,ecx
  421359:	a3 84 fd 89 00       	mov    ds:0x89fd84,eax
  42135e:	eb 0f                	jmp    0x42136f
  421360:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421367:	39 a8 6c 0a 00 00    	cmp    DWORD PTR [eax+0xa6c],ebp
  42136d:	7e 05                	jle    0x421374
  42136f:	89 2b                	mov    DWORD PTR [ebx],ebp
  421371:	89 6b 04             	mov    DWORD PTR [ebx+0x4],ebp
  421374:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  42137b:	39 a8 80 0a 00 00    	cmp    DWORD PTR [eax+0xa80],ebp
  421381:	7e 14                	jle    0x421397
  421383:	39 2b                	cmp    DWORD PTR [ebx],ebp
  421385:	7f 05                	jg     0x42138c
  421387:	39 6b 04             	cmp    DWORD PTR [ebx+0x4],ebp
  42138a:	7e 0b                	jle    0x421397
  42138c:	89 a8 80 0a 00 00    	mov    DWORD PTR [eax+0xa80],ebp
  421392:	89 2b                	mov    DWORD PTR [ebx],ebp
  421394:	89 6b 04             	mov    DWORD PTR [ebx+0x4],ebp
  421397:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  42139e:	39 a8 58 0a 00 00    	cmp    DWORD PTR [eax+0xa58],ebp
  4213a4:	7e 0f                	jle    0x4213b5
  4213a6:	39 2b                	cmp    DWORD PTR [ebx],ebp
  4213a8:	7d 05                	jge    0x4213af
  4213aa:	39 6b 04             	cmp    DWORD PTR [ebx+0x4],ebp
  4213ad:	7c 06                	jl     0x4213b5
  4213af:	89 a8 58 0a 00 00    	mov    DWORD PTR [eax+0xa58],ebp
  4213b5:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  4213bc:	39 a9 74 0a 00 00    	cmp    DWORD PTR [ecx+0xa74],ebp
  4213c2:	7e 1d                	jle    0x4213e1
  4213c4:	8b 03                	mov    eax,DWORD PTR [ebx]
  4213c6:	3b c5                	cmp    eax,ebp
  4213c8:	7e 04                	jle    0x4213ce
  4213ca:	89 2b                	mov    DWORD PTR [ebx],ebp
  4213cc:	eb 13                	jmp    0x4213e1
  4213ce:	7d 11                	jge    0x4213e1
  4213d0:	f7 d8                	neg    eax
  4213d2:	89 03                	mov    DWORD PTR [ebx],eax
  4213d4:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  4213db:	89 aa 74 0a 00 00    	mov    DWORD PTR [edx+0xa74],ebp
  4213e1:	8a 43 3c             	mov    al,BYTE PTR [ebx+0x3c]
  4213e4:	84 c0                	test   al,al
  4213e6:	0f 84 e7 00 00 00    	je     0x4214d3
  4213ec:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  4213f3:	55                   	push   ebp
  4213f4:	55                   	push   ebp
  4213f5:	6a 67                	push   0x67
  4213f7:	8b 88 a0 0b 00 00    	mov    ecx,DWORD PTR [eax+0xba0]
  4213fd:	89 88 9c 0b 00 00    	mov    DWORD PTR [eax+0xb9c],ecx
  421403:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  42140a:	8b 90 4c 08 00 00    	mov    edx,DWORD PTR [eax+0x84c]
  421410:	89 90 2c 07 00 00    	mov    DWORD PTR [eax+0x72c],edx
  421416:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  42141d:	e8 fe 0c 00 00       	call   0x422120
  421422:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421429:	55                   	push   ebp
  42142a:	6a 01                	push   0x1
  42142c:	6a 0a                	push   0xa
  42142e:	8b 88 a4 0b 00 00    	mov    ecx,DWORD PTR [eax+0xba4]
  421434:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  421437:	c6 82 58 03 00 00 01 	mov    BYTE PTR [edx+0x358],0x1
  42143e:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421445:	c7 80 7c 08 00 00 01 	mov    DWORD PTR [eax+0x87c],0x1
  42144c:	00 00 00 
  42144f:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421456:	c7 81 e4 0c 00 00 b9 	mov    DWORD PTR [ecx+0xce4],0xbb9
  42145d:	0b 00 00 
  421460:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  421467:	89 aa e8 08 00 00    	mov    DWORD PTR [edx+0x8e8],ebp
  42146d:	8b 15 f8 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf8
  421473:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  42147a:	4a                   	dec    edx
  42147b:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  421481:	89 15 f8 fd 89 00    	mov    DWORD PTR ds:0x89fdf8,edx
  421487:	e8 04 2b 00 00       	call   0x423f90
  42148c:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421493:	6a 01                	push   0x1
  421495:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  42149b:	e8 e0 2a 00 00       	call   0x423f80
  4214a0:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  4214a7:	c7 80 e0 0c 00 00 d1 	mov    DWORD PTR [eax+0xce0],0x7d1
  4214ae:	07 00 00 
  4214b1:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  4214b8:	c7 81 e8 0c 00 00 a0 	mov    DWORD PTR [ecx+0xce8],0xfa0
  4214bf:	0f 00 00 
  4214c2:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  4214c9:	c7 82 e4 0c 00 00 be 	mov    DWORD PTR [edx+0xce4],0xbbe
  4214d0:	0b 00 00 
  4214d3:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  4214da:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  4214e0:	8b 81 9c 0b 00 00    	mov    eax,DWORD PTR [ecx+0xb9c]
  4214e6:	c1 e0 02             	shl    eax,0x2
  4214e9:	8b 14 10             	mov    edx,DWORD PTR [eax+edx*1]
  4214ec:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  4214f0:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  4214f6:	8b 04 10             	mov    eax,DWORD PTR [eax+edx*1]
  4214f9:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  4214fd:	8b 81 e4 0c 00 00    	mov    eax,DWORD PTR [ecx+0xce4]
  421503:	3d c6 0b 00 00       	cmp    eax,0xbc6
  421508:	75 02                	jne    0x42150c
  42150a:	89 2b                	mov    DWORD PTR [ebx],ebp
  42150c:	8a 43 fc             	mov    al,BYTE PTR [ebx-0x4]
  42150f:	8b 2b                	mov    ebp,DWORD PTR [ebx]
  421511:	8b 7b 04             	mov    edi,DWORD PTR [ebx+0x4]
  421514:	84 c0                	test   al,al
  421516:	74 48                	je     0x421560
  421518:	8a 4b 44             	mov    cl,BYTE PTR [ebx+0x44]
  42151b:	51                   	push   ecx
  42151c:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  421520:	56                   	push   esi
  421521:	e8 fa f8 ff ff       	call   0x420e20
  421526:	84 c0                	test   al,al
  421528:	74 36                	je     0x421560
  42152a:	85 ed                	test   ebp,ebp
  42152c:	74 17                	je     0x421545
  42152e:	33 c0                	xor    eax,eax
  421530:	85 ed                	test   ebp,ebp
  421532:	0f 9e c0             	setle  al
  421535:	48                   	dec    eax
  421536:	83 e0 04             	and    eax,0x4
  421539:	83 c0 fe             	add    eax,0xfffffffe
  42153c:	03 c5                	add    eax,ebp
  42153e:	99                   	cdq
  42153f:	2b c2                	sub    eax,edx
  421541:	d1 f8                	sar    eax,1
  421543:	8b e8                	mov    ebp,eax
  421545:	85 ff                	test   edi,edi
  421547:	74 17                	je     0x421560
  421549:	33 c0                	xor    eax,eax
  42154b:	85 ff                	test   edi,edi
  42154d:	0f 9e c0             	setle  al
  421550:	48                   	dec    eax
  421551:	83 e0 04             	and    eax,0x4
  421554:	83 c0 fe             	add    eax,0xfffffffe
  421557:	03 c7                	add    eax,edi
  421559:	99                   	cdq
  42155a:	2b c2                	sub    eax,edx
  42155c:	d1 f8                	sar    eax,1
  42155e:	8b f8                	mov    edi,eax
  421560:	85 ed                	test   ebp,ebp
  421562:	7c 19                	jl     0x42157d
  421564:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  42156b:	8b 82 7c 08 00     	mov    eax,DWORD PTR [edx+0x87c]
