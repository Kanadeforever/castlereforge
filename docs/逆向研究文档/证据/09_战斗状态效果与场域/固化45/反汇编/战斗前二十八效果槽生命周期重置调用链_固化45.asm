; ===== VA 0x0041F0D0..0x0041F2B0 =====
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

; ===== VA 0x0041F580..0x0041F6D0 =====
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
  41f6d0:	8d 4c 24 0c          	lea    ecx,[esp+0xc]

; ===== VA 0x004203E0..0x00421070 =====
  4203e3:	5e                   	pop    esi
  4203e4:	c3                   	ret
  4203e5:	90                   	nop
  4203e6:	90                   	nop
  4203e7:	90                   	nop
  4203e8:	90                   	nop
  4203e9:	90                   	nop
  4203ea:	90                   	nop
  4203eb:	90                   	nop
  4203ec:	90                   	nop
  4203ed:	90                   	nop
  4203ee:	90                   	nop
  4203ef:	90                   	nop
  4203f0:	8b 81 48 08 00 00    	mov    eax,DWORD PTR [ecx+0x848]
  4203f6:	85 c0                	test   eax,eax
  4203f8:	74 15                	je     0x42040f
  4203fa:	8b 00                	mov    eax,DWORD PTR [eax]
  4203fc:	8b 89 4c 08 00 00    	mov    ecx,DWORD PTR [ecx+0x84c]
  420402:	50                   	push   eax
  420403:	51                   	push   ecx
  420404:	8b 0d 80 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd80
  42040a:	e8 81 39 02 00       	call   0x443d90
  42040f:	c3                   	ret
  420410:	8b 81 a4 0b 00 00    	mov    eax,DWORD PTR [ecx+0xba4]
  420416:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  42041a:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  42041d:	03 ca                	add    ecx,edx
  42041f:	8b 50 18             	mov    edx,DWORD PTR [eax+0x18]
  420422:	51                   	push   ecx
  420423:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  420427:	03 d1                	add    edx,ecx
  420429:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  42042c:	52                   	push   edx
  42042d:	e8 ae eb 01 00       	call   0x43efe0
  420432:	c2 08 00             	ret    0x8
  420435:	90                   	nop
  420436:	90                   	nop
  420437:	90                   	nop
  420438:	90                   	nop
  420439:	90                   	nop
  42043a:	90                   	nop
  42043b:	90                   	nop
  42043c:	90                   	nop
  42043d:	90                   	nop
  42043e:	90                   	nop
  42043f:	90                   	nop
  420440:	56                   	push   esi
  420441:	8b f1                	mov    esi,ecx
  420443:	57                   	push   edi
  420444:	6a 09                	push   0x9
  420446:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  42044c:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  42044f:	8b 88 5c 03 00 00    	mov    ecx,DWORD PTR [eax+0x35c]
  420455:	e8 46 9d 00 00       	call   0x42a1a0
  42045a:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420460:	8b f8                	mov    edi,eax
  420462:	6a 08                	push   0x8
  420464:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  420467:	8b 88 5c 03 00 00    	mov    ecx,DWORD PTR [eax+0x35c]
  42046d:	e8 2e 9d 00 00       	call   0x42a1a0
  420472:	83 ff 01             	cmp    edi,0x1
  420475:	75 20                	jne    0x420497
  420477:	8b 86 a0 07 00 00    	mov    eax,DWORD PTR [esi+0x7a0]
  42047d:	8b 14 85 94 fd 89 00 	mov    edx,DWORD PTR [eax*4+0x89fd94]
  420484:	8b 8a a4 0b 00 00    	mov    ecx,DWORD PTR [edx+0xba4]
  42048a:	e8 61 cd 01 00       	call   0x43d1f0
  42048f:	05 e8 03 00 00       	add    eax,0x3e8
  420494:	57                   	push   edi
  420495:	eb 20                	jmp    0x4204b7
  420497:	83 ff 02             	cmp    edi,0x2
  42049a:	74 05                	je     0x4204a1
  42049c:	83 f8 01             	cmp    eax,0x1
  42049f:	75 25                	jne    0x4204c6
  4204a1:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4204a3:	8b 14 8d 94 fd 89 00 	mov    edx,DWORD PTR [ecx*4+0x89fd94]
  4204aa:	8b 8a a4 0b 00 00    	mov    ecx,DWORD PTR [edx+0xba4]
  4204b0:	e8 3b cd 01 00       	call   0x43d1f0
  4204b5:	6a 00                	push   0x0
  4204b7:	8b 0d 80 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd80
  4204bd:	50                   	push   eax
  4204be:	8b 06                	mov    eax,DWORD PTR [esi]
  4204c0:	50                   	push   eax
  4204c1:	e8 1a d2 01 00       	call   0x43d6e0
  4204c6:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  4204cc:	bf 64 00 00 00       	mov    edi,0x64
  4204d1:	83 f8 01             	cmp    eax,0x1
  4204d4:	75 10                	jne    0x4204e6
  4204d6:	6a 07                	push   0x7
  4204d8:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4204dd:	e8 6e d5 01 00       	call   0x43da50
  4204e2:	84 c0                	test   al,al
  4204e4:	75 1a                	jne    0x420500
  4204e6:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  4204ec:	85 c0                	test   eax,eax
  4204ee:	75 15                	jne    0x420505
  4204f0:	6a 07                	push   0x7
  4204f2:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4204f7:	e8 54 d5 01 00       	call   0x43da50
  4204fc:	84 c0                	test   al,al
  4204fe:	74 05                	je     0x420505
  420500:	bf c8 00 00 00       	mov    edi,0xc8
  420505:	83 be 50 08 00 00 01 	cmp    DWORD PTR [esi+0x850],0x1
  42050c:	75 10                	jne    0x42051e
  42050e:	6a 08                	push   0x8
  420510:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  420515:	e8 36 d5 01 00       	call   0x43da50
  42051a:	84 c0                	test   al,al
  42051c:	75 1a                	jne    0x420538
  42051e:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  420524:	85 c0                	test   eax,eax
  420526:	75 2b                	jne    0x420553
  420528:	6a 08                	push   0x8
  42052a:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  42052f:	e8 1c d5 01 00       	call   0x43da50
  420534:	84 c0                	test   al,al
  420536:	74 1b                	je     0x420553
  420538:	8d 04 bf             	lea    eax,[edi+edi*4]
  42053b:	8d 0c 80             	lea    ecx,[eax+eax*4]
  42053e:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420543:	d1 e1                	shl    ecx,1
  420545:	f7 e9                	imul   ecx
  420547:	c1 fa 05             	sar    edx,0x5
  42054a:	8b ca                	mov    ecx,edx
  42054c:	c1 e9 1f             	shr    ecx,0x1f
  42054f:	03 d1                	add    edx,ecx
  420551:	8b fa                	mov    edi,edx
  420553:	8b 86 38 0a 00 00    	mov    eax,DWORD PTR [esi+0xa38]
  420559:	85 c0                	test   eax,eax
  42055b:	7e 13                	jle    0x420570
  42055d:	b8 67 66 66 66       	mov    eax,0x66666667
  420562:	f7 ef                	imul   edi
  420564:	c1 fa 02             	sar    edx,0x2
  420567:	8b c2                	mov    eax,edx
  420569:	c1 e8 1f             	shr    eax,0x1f
  42056c:	03 d0                	add    edx,eax
  42056e:	8b fa                	mov    edi,edx
  420570:	8b 86 74 0a 00 00    	mov    eax,DWORD PTR [esi+0xa74]
  420576:	85 c0                	test   eax,eax
  420578:	7e 1c                	jle    0x420596
  42057a:	8d 04 7f             	lea    eax,[edi+edi*2]
  42057d:	8d 04 80             	lea    eax,[eax+eax*4]
  420580:	8d 0c 80             	lea    ecx,[eax+eax*4]
  420583:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420588:	f7 e9                	imul   ecx
  42058a:	c1 fa 05             	sar    edx,0x5
  42058d:	8b ca                	mov    ecx,edx
  42058f:	c1 e9 1f             	shr    ecx,0x1f
  420592:	03 d1                	add    edx,ecx
  420594:	8b fa                	mov    edi,edx
  420596:	8b 86 44 0a 00 00    	mov    eax,DWORD PTR [esi+0xa44]
  42059c:	85 c0                	test   eax,eax
  42059e:	7e 1b                	jle    0x4205bb
  4205a0:	8d 04 bf             	lea    eax,[edi+edi*4]
  4205a3:	8d 0c 80             	lea    ecx,[eax+eax*4]
  4205a6:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4205ab:	d1 e1                	shl    ecx,1
  4205ad:	f7 e9                	imul   ecx
  4205af:	c1 fa 05             	sar    edx,0x5
  4205b2:	8b c2                	mov    eax,edx
  4205b4:	c1 e8 1f             	shr    eax,0x1f
  4205b7:	03 d0                	add    edx,eax
  4205b9:	8b fa                	mov    edi,edx
  4205bb:	8b 86 90 0a 00 00    	mov    eax,DWORD PTR [esi+0xa90]
  4205c1:	85 c0                	test   eax,eax
  4205c3:	7e 1e                	jle    0x4205e3
  4205c5:	8d 04 7f             	lea    eax,[edi+edi*2]
  4205c8:	8d 04 80             	lea    eax,[eax+eax*4]
  4205cb:	8d 0c 80             	lea    ecx,[eax+eax*4]
  4205ce:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4205d3:	d1 e1                	shl    ecx,1
  4205d5:	f7 e9                	imul   ecx
  4205d7:	c1 fa 05             	sar    edx,0x5
  4205da:	8b ca                	mov    ecx,edx
  4205dc:	c1 e9 1f             	shr    ecx,0x1f
  4205df:	03 d1                	add    edx,ecx
  4205e1:	8b fa                	mov    edi,edx
  4205e3:	8b 86 58 0a 00 00    	mov    eax,DWORD PTR [esi+0xa58]
  4205e9:	85 c0                	test   eax,eax
  4205eb:	7e 04                	jle    0x4205f1
  4205ed:	33 ff                	xor    edi,edi
  4205ef:	eb 0d                	jmp    0x4205fe
  4205f1:	81 ff c8 00 00 00    	cmp    edi,0xc8
  4205f7:	7e 05                	jle    0x4205fe
  4205f9:	bf c8 00 00 00       	mov    edi,0xc8
  4205fe:	53                   	push   ebx
  4205ff:	8d 9e a8 0c 00 00    	lea    ebx,[esi+0xca8]
  420605:	6a 01                	push   0x1
  420607:	57                   	push   edi
  420608:	8b cb                	mov    ecx,ebx
  42060a:	e8 81 3a 00 00       	call   0x424090
  42060f:	6a 02                	push   0x2
  420611:	57                   	push   edi
  420612:	8b cb                	mov    ecx,ebx
  420614:	e8 77 3a 00 00       	call   0x424090
  420619:	8b 86 38 0a 00 00    	mov    eax,DWORD PTR [esi+0xa38]
  42061f:	ba 64 00 00 00       	mov    edx,0x64
  420624:	85 c0                	test   eax,eax
  420626:	7e 05                	jle    0x42062d
  420628:	ba 0a 00 00 00       	mov    edx,0xa
  42062d:	8b 86 74 0a 00 00    	mov    eax,DWORD PTR [esi+0xa74]
  420633:	85 c0                	test   eax,eax
  420635:	7e 1a                	jle    0x420651
  420637:	8d 04 52             	lea    eax,[edx+edx*2]
  42063a:	8d 04 80             	lea    eax,[eax+eax*4]
  42063d:	8d 0c 80             	lea    ecx,[eax+eax*4]
  420640:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420645:	f7 e9                	imul   ecx
  420647:	c1 fa 05             	sar    edx,0x5
  42064a:	8b c2                	mov    eax,edx
  42064c:	c1 e8 1f             	shr    eax,0x1f
  42064f:	03 d0                	add    edx,eax
  420651:	6a 03                	push   0x3
  420653:	52                   	push   edx
  420654:	8b cb                	mov    ecx,ebx
  420656:	e8 35 3a 00 00       	call   0x424090
  42065b:	8b cb                	mov    ecx,ebx
  42065d:	e8 2e 38 00 00       	call   0x423e90
  420662:	8d be 34 0a 00 00    	lea    edi,[esi+0xa34]
  420668:	33 d2                	xor    edx,edx
  42066a:	8b c7                	mov    eax,edi
  42066c:	5b                   	pop    ebx
  42066d:	8b 08                	mov    ecx,DWORD PTR [eax]
  42066f:	85 c9                	test   ecx,ecx
  420671:	7e 1d                	jle    0x420690
  420673:	83 fa 10             	cmp    edx,0x10
  420676:	75 09                	jne    0x420681
  420678:	85 c9                	test   ecx,ecx
  42067a:	7e 14                	jle    0x420690
  42067c:	83 fa 13             	cmp    edx,0x13
  42067f:	74 0f                	je     0x420690
  420681:	83 c1 f6             	add    ecx,0xfffffff6
  420684:	85 c9                	test   ecx,ecx
  420686:	89 08                	mov    DWORD PTR [eax],ecx
  420688:	7f 06                	jg     0x420690
  42068a:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
  420690:	42                   	inc    edx
  420691:	83 c0 04             	add    eax,0x4
  420694:	83 fa 27             	cmp    edx,0x27
  420697:	7c d4                	jl     0x42066d
  420699:	8b 3f                	mov    edi,DWORD PTR [edi]
  42069b:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
  4206a0:	8d 4f 0a             	lea    ecx,[edi+0xa]
  4206a3:	f7 e9                	imul   ecx
  4206a5:	8b ca                	mov    ecx,edx
  4206a7:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
  4206ac:	c1 f9 06             	sar    ecx,0x6
  4206af:	8b d1                	mov    edx,ecx
  4206b1:	c1 ea 1f             	shr    edx,0x1f
  4206b4:	03 ca                	add    ecx,edx
  4206b6:	f7 ef                	imul   edi
  4206b8:	c1 fa 06             	sar    edx,0x6
  4206bb:	8b c2                	mov    eax,edx
  4206bd:	c1 e8 1f             	shr    eax,0x1f
  4206c0:	03 d0                	add    edx,eax
  4206c2:	3b ca                	cmp    ecx,edx
  4206c4:	74 4d                	je     0x420713
  4206c6:	8b 8e 8c 08 00 00    	mov    ecx,DWORD PTR [esi+0x88c]
  4206cc:	b8 67 66 66 66       	mov    eax,0x66666667
  4206d1:	f7 e9                	imul   ecx
  4206d3:	c1 fa 03             	sar    edx,0x3
  4206d6:	8b ca                	mov    ecx,edx
  4206d8:	b8 67 66 66 66       	mov    eax,0x66666667
  4206dd:	c1 e9 1f             	shr    ecx,0x1f
  4206e0:	03 d1                	add    edx,ecx
  4206e2:	8b 8e 78 08 00 00    	mov    ecx,DWORD PTR [esi+0x878]
  4206e8:	8b fa                	mov    edi,edx
  4206ea:	f7 e9                	imul   ecx
  4206ec:	c1 fa 03             	sar    edx,0x3
  4206ef:	8b 8e 7c 08 00 00    	mov    ecx,DWORD PTR [esi+0x87c]
  4206f5:	8b c2                	mov    eax,edx
  4206f7:	c1 e8 1f             	shr    eax,0x1f
  4206fa:	03 d0                	add    edx,eax
  4206fc:	2b fa                	sub    edi,edx
  4206fe:	03 cf                	add    ecx,edi
  420700:	8b c1                	mov    eax,ecx
  420702:	89 8e 7c 08 00 00    	mov    DWORD PTR [esi+0x87c],ecx
  420708:	85 c0                	test   eax,eax
  42070a:	7f 07                	jg     0x420713
  42070c:	8b ce                	mov    ecx,esi
  42070e:	e8 dd 2a 00 00       	call   0x4231f0
  420713:	8b be 88 0a 00 00    	mov    edi,DWORD PTR [esi+0xa88]
  420719:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
  42071e:	8d 4f 0a             	lea    ecx,[edi+0xa]
  420721:	f7 e9                	imul   ecx
  420723:	8b ca                	mov    ecx,edx
  420725:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
  42072a:	c1 f9 06             	sar    ecx,0x6
  42072d:	8b d1                	mov    edx,ecx
  42072f:	c1 ea 1f             	shr    edx,0x1f
  420732:	03 ca                	add    ecx,edx
  420734:	f7 ef                	imul   edi
  420736:	c1 fa 06             	sar    edx,0x6
  420739:	8b c2                	mov    eax,edx
  42073b:	c1 e8 1f             	shr    eax,0x1f
  42073e:	03 d0                	add    edx,eax
  420740:	3b ca                	cmp    ecx,edx
  420742:	74 4c                	je     0x420790
  420744:	8b 8e 8c 08 00 00    	mov    ecx,DWORD PTR [esi+0x88c]
  42074a:	b8 67 66 66 66       	mov    eax,0x66666667
  42074f:	f7 e9                	imul   ecx
  420751:	8b be 78 08 00 00    	mov    edi,DWORD PTR [esi+0x878]
  420757:	8b ca                	mov    ecx,edx
  420759:	c1 f9 03             	sar    ecx,0x3
  42075c:	8b d1                	mov    edx,ecx
  42075e:	b8 67 66 66 66       	mov    eax,0x66666667
  420763:	c1 ea 1f             	shr    edx,0x1f
  420766:	03 ca                	add    ecx,edx
  420768:	f7 ef                	imul   edi
  42076a:	c1 fa 03             	sar    edx,0x3
  42076d:	8b c2                	mov    eax,edx
  42076f:	03 d1                	add    edx,ecx
  420771:	8b 8e 7c 08 00 00    	mov    ecx,DWORD PTR [esi+0x87c]
  420777:	c1 e8 1f             	shr    eax,0x1f
  42077a:	03 c2                	add    eax,edx
  42077c:	03 c8                	add    ecx,eax
  42077e:	8b c1                	mov    eax,ecx
  420780:	89 8e 7c 08 00 00    	mov    DWORD PTR [esi+0x87c],ecx
  420786:	3b c7                	cmp    eax,edi
  420788:	7e 06                	jle    0x420790
  42078a:	89 be 7c 08 00 00    	mov    DWORD PTR [esi+0x87c],edi
  420790:	81 be a4 07 00 00 e9 	cmp    DWORD PTR [esi+0x7a4],0x3e9
  420797:	03 00 00 
  42079a:	75 09                	jne    0x4207a5
  42079c:	6a 00                	push   0x0
  42079e:	8b ce                	mov    ecx,esi
  4207a0:	e8 0b 00 00 00       	call   0x4207b0
  4207a5:	5f                   	pop    edi
  4207a6:	5e                   	pop    esi
  4207a7:	c3                   	ret
  4207a8:	90                   	nop
  4207a9:	90                   	nop
  4207aa:	90                   	nop
  4207ab:	90                   	nop
  4207ac:	90                   	nop
  4207ad:	90                   	nop
  4207ae:	90                   	nop
  4207af:	90                   	nop
  4207b0:	a0 fc fd 89 00       	mov    al,ds:0x89fdfc
  4207b5:	83 ec 18             	sub    esp,0x18
  4207b8:	84 c0                	test   al,al
  4207ba:	53                   	push   ebx
  4207bb:	55                   	push   ebp
  4207bc:	56                   	push   esi
  4207bd:	57                   	push   edi
  4207be:	8b f1                	mov    esi,ecx
  4207c0:	75 57                	jne    0x420819
  4207c2:	33 d2                	xor    edx,edx
  4207c4:	33 ed                	xor    ebp,ebp
  4207c6:	8d bd b4 9a 46 00    	lea    edi,[ebp+0x469ab4]
  4207cc:	83 c9 ff             	or     ecx,0xffffffff
  4207cf:	33 c0                	xor    eax,eax
  4207d1:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  4207d3:	f7 d1                	not    ecx
  4207d5:	49                   	dec    ecx
  4207d6:	8d bd 00 9c 46 00    	lea    edi,[ebp+0x469c00]
  4207dc:	0f 95 c3             	setne  bl
  4207df:	83 c9 ff             	or     ecx,0xffffffff
  4207e2:	88 9a e0 fd 89 00    	mov    BYTE PTR [edx+0x89fde0],bl
  4207e8:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  4207ea:	f7 d1                	not    ecx
  4207ec:	49                   	dec    ecx
  4207ed:	0f 95 c0             	setne  al
  4207f0:	84 db                	test   bl,bl
  4207f2:	88 82 d4 fd 89 00    	mov    BYTE PTR [edx+0x89fdd4],al
  4207f8:	75 04                	jne    0x4207fe
  4207fa:	84 c0                	test   al,al
  4207fc:	74 02                	je     0x420800
  4207fe:	b0 01                	mov    al,0x1
  420800:	88 82 88 fd 89 00    	mov    BYTE PTR [edx+0x89fd88],al
  420806:	83 c5 1e             	add    ebp,0x1e
  420809:	42                   	inc    edx
  42080a:	81 fd 4a 01 00 00    	cmp    ebp,0x14a
  420810:	7c b4                	jl     0x4207c6
  420812:	c6 05 fc fd 89 00 01 	mov    BYTE PTR ds:0x89fdfc,0x1
  420819:	8b 86 58 0a 00 00    	mov    eax,DWORD PTR [esi+0xa58]
  42081f:	8b ae 6c 0a 00 00    	mov    ebp,DWORD PTR [esi+0xa6c]
  420825:	8b be 34 0a 00 00    	mov    edi,DWORD PTR [esi+0xa34]
  42082b:	33 db                	xor    ebx,ebx
  42082d:	3b c3                	cmp    eax,ebx
  42082f:	0f 9f c0             	setg   al
  420832:	3b eb                	cmp    ebp,ebx
  420834:	8b ae 38 0a 00 00    	mov    ebp,DWORD PTR [esi+0xa38]
  42083a:	0f 9f c1             	setg   cl
  42083d:	3b fb                	cmp    edi,ebx
  42083f:	88 44 24 1c          	mov    BYTE PTR [esp+0x1c],al
  420843:	0f 9f c2             	setg   dl
  420846:	88 54 24 1e          	mov    BYTE PTR [esp+0x1e],dl
  42084a:	8b 96 88 0a 00 00    	mov    edx,DWORD PTR [esi+0xa88]
  420850:	3b d3                	cmp    edx,ebx
  420852:	88 4c 24 1d          	mov    BYTE PTR [esp+0x1d],cl
  420856:	8b 8e 70 0a 00 00    	mov    ecx,DWORD PTR [esi+0xa70]
  42085c:	8b be 3c 0a 00 00    	mov    edi,DWORD PTR [esi+0xa3c]
  420862:	0f 9f c0             	setg   al
  420865:	88 44 24 1f          	mov    BYTE PTR [esp+0x1f],al
  420869:	8b 86 5c 0a 00 00    	mov    eax,DWORD PTR [esi+0xa5c]
  42086f:	3b cb                	cmp    ecx,ebx
  420871:	0f 9f c1             	setg   cl
  420874:	3b c3                	cmp    eax,ebx
  420876:	88 4c 24 20          	mov    BYTE PTR [esp+0x20],cl
  42087a:	0f 9f c2             	setg   dl
  42087d:	3b eb                	cmp    ebp,ebx
  42087f:	88 54 24 21          	mov    BYTE PTR [esp+0x21],dl
  420883:	8b 96 60 0a 00 00    	mov    edx,DWORD PTR [esi+0xa60]
  420889:	0f 9f c0             	setg   al
  42088c:	3b fb                	cmp    edi,ebx
  42088e:	88 44 24 22          	mov    BYTE PTR [esp+0x22],al
  420892:	0f 9f c1             	setg   cl
  420895:	88 4c 24 23          	mov    BYTE PTR [esp+0x23],cl
  420899:	8b 8e 68 0a 00 00    	mov    ecx,DWORD PTR [esi+0xa68]
  42089f:	3b d3                	cmp    edx,ebx
  4208a1:	0f 9f c2             	setg   dl
  4208a4:	3b cb                	cmp    ecx,ebx
  4208a6:	88 54 24 24          	mov    BYTE PTR [esp+0x24],dl
  4208aa:	0f 9f c0             	setg   al
  4208ad:	88 44 24 25          	mov    BYTE PTR [esp+0x25],al
  4208b1:	8b 86 80 0a 00 00    	mov    eax,DWORD PTR [esi+0xa80]
  4208b7:	3b c3                	cmp    eax,ebx
  4208b9:	0f 9f c1             	setg   cl
  4208bc:	88 4c 24 26          	mov    BYTE PTR [esp+0x26],cl
  4208c0:	33 ff                	xor    edi,edi
  4208c2:	8a 44 3c 1c          	mov    al,BYTE PTR [esp+edi*1+0x1c]
  4208c6:	84 c0                	test   al,al
  4208c8:	0f 85 d4 00 00 00    	jne    0x4209a2
  4208ce:	8a 84 37 10 07 00 00 	mov    al,BYTE PTR [edi+esi*1+0x710]
  4208d5:	84 c0                	test   al,al
  4208d7:	0f 84 be 00 00 00    	je     0x42099b
  4208dd:	8a 87 e0 fd 89 00    	mov    al,BYTE PTR [edi+0x89fde0]
  4208e3:	84 c0                	test   al,al
  4208e5:	74 0b                	je     0x4208f2
  4208e7:	53                   	push   ebx
  4208e8:	53                   	push   ebx
  4208e9:	57                   	push   edi
  4208ea:	53                   	push   ebx
  4208eb:	8b ce                	mov    ecx,esi
  4208ed:	e8 ae 03 00 00       	call   0x420ca0
  4208f2:	8a 87 d4 fd 89 00    	mov    al,BYTE PTR [edi+0x89fdd4]
  4208f8:	84 c0                	test   al,al
  4208fa:	74 0b                	je     0x420907
  4208fc:	53                   	push   ebx
  4208fd:	53                   	push   ebx
  4208fe:	57                   	push   edi
  4208ff:	53                   	push   ebx
  420900:	8b ce                	mov    ecx,esi
  420902:	e8 59 04 00 00       	call   0x420d60
  420907:	3b fb                	cmp    edi,ebx
  420909:	75 36                	jne    0x420941
  42090b:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  420911:	53                   	push   ebx
  420912:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  420915:	c7 80 80 00 00 00 ff 	mov    DWORD PTR [eax+0x80],0xffffffff
  42091c:	ff ff ff 
  42091f:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  420925:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  42092b:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  420931:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  420937:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  42093a:	e8 61 dd 01 00       	call   0x43e6a0
  42093f:	eb 52                	jmp    0x420993
  420941:	83 ff 01             	cmp    edi,0x1
  420944:	75 11                	jne    0x420957
  420946:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  42094c:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  42094f:	89 9a 60 03 00 00    	mov    DWORD PTR [edx+0x360],ebx
  420955:	eb 3c                	jmp    0x420993
  420957:	83 ff 06             	cmp    edi,0x6
  42095a:	75 17                	jne    0x420973
  42095c:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  420962:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  420965:	8b 88 60 03 00 00    	mov    ecx,DWORD PTR [eax+0x360]
  42096b:	81 e1 fb ff 00 00    	and    ecx,0xfffb
  420971:	eb 1a                	jmp    0x42098d
  420973:	83 ff 0a             	cmp    edi,0xa
  420976:	75 1b                	jne    0x420993
  420978:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  42097e:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  420981:	8b 88 60 03 00 00    	mov    ecx,DWORD PTR [eax+0x360]
  420987:	81 e1 f7 ff 00 00    	and    ecx,0xfff7
  42098d:	89 88 60 03 00 00    	mov    DWORD PTR [eax+0x360],ecx
  420993:	c6 84 37 10 07 00 00 	mov    BYTE PTR [edi+esi*1+0x710],0x0
  42099a:	00 
  42099b:	c6 44 3c 10 00       	mov    BYTE PTR [esp+edi*1+0x10],0x0
  4209a0:	eb 05                	jmp    0x4209a7
  4209a2:	c6 44 3c 10 01       	mov    BYTE PTR [esp+edi*1+0x10],0x1
  4209a7:	47                   	inc    edi
  4209a8:	83 ff 0b             	cmp    edi,0xb
  4209ab:	0f 8c 11 ff ff ff    	jl     0x4208c2
  4209b1:	33 c0                	xor    eax,eax
  4209b3:	8a 4c 04 10          	mov    cl,BYTE PTR [esp+eax*1+0x10]
  4209b7:	84 c9                	test   cl,cl
  4209b9:	74 05                	je     0x4209c0
  4209bb:	c6 44 04 10 01       	mov    BYTE PTR [esp+eax*1+0x10],0x1
  4209c0:	40                   	inc    eax
  4209c1:	83 f8 0b             	cmp    eax,0xb
  4209c4:	7c ed                	jl     0x4209b3
  4209c6:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  4209ca:	84 c0                	test   al,al
  4209cc:	74 05                	je     0x4209d3
  4209ce:	c6 44 24 12 00       	mov    BYTE PTR [esp+0x12],0x0
  4209d3:	8a 44 24 14          	mov    al,BYTE PTR [esp+0x14]
  4209d7:	84 c0                	test   al,al
  4209d9:	74 05                	je     0x4209e0
  4209db:	c6 44 24 15 00       	mov    BYTE PTR [esp+0x15],0x0
  4209e0:	8a 44 24 10          	mov    al,BYTE PTR [esp+0x10]
  4209e4:	84 c0                	test   al,al
  4209e6:	74 18                	je     0x420a00
  4209e8:	33 d2                	xor    edx,edx
  4209ea:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  4209ee:	c6 44 24 10 01       	mov    BYTE PTR [esp+0x10],0x1
  4209f3:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  4209f7:	66 89 54 24 18       	mov    WORD PTR [esp+0x18],dx
  4209fc:	88 54 24 1a          	mov    BYTE PTR [esp+0x1a],dl
  420a00:	8a 44 24 11          	mov    al,BYTE PTR [esp+0x11]
  420a04:	84 c0                	test   al,al
  420a06:	74 18                	je     0x420a20
  420a08:	33 c0                	xor    eax,eax
  420a0a:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  420a0e:	c6 44 24 11 01       	mov    BYTE PTR [esp+0x11],0x1
  420a13:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  420a17:	66 89 44 24 18       	mov    WORD PTR [esp+0x18],ax
  420a1c:	88 44 24 1a          	mov    BYTE PTR [esp+0x1a],al
  420a20:	8a 44 24 2c          	mov    al,BYTE PTR [esp+0x2c]
  420a24:	84 c0                	test   al,al
  420a26:	74 19                	je     0x420a41
  420a28:	33 c9                	xor    ecx,ecx
  420a2a:	88 86 0f 07 00 00    	mov    BYTE PTR [esi+0x70f],al
  420a30:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  420a34:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  420a38:	66 89 4c 24 18       	mov    WORD PTR [esp+0x18],cx
  420a3d:	88 4c 24 1a          	mov    BYTE PTR [esp+0x1a],cl
  420a41:	33 ff                	xor    edi,edi
  420a43:	bd 08 00 00 00       	mov    ebp,0x8
  420a48:	8a 5c 3c 10          	mov    bl,BYTE PTR [esp+edi*1+0x10]
  420a4c:	84 db                	test   bl,bl
  420a4e:	0f 85 e2 00 00 00    	jne    0x420b36
  420a54:	8a 84 37 10 07 00 00 	mov    al,BYTE PTR [edi+esi*1+0x710]
  420a5b:	84 c0                	test   al,al
  420a5d:	0f 84 cb 00 00 00    	je     0x420b2e
  420a63:	83 ff 0a             	cmp    edi,0xa
  420a66:	0f 87 c2 00 00 00    	ja     0x420b2e
  420a6c:	ff 24 bd 44 0c 42 00 	jmp    DWORD PTR [edi*4+0x420c44]
  420a73:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  420a79:	b9 10 00 00 00       	mov    ecx,0x10
  420a7e:	6a ff                	push   0xffffffff
  420a80:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  420a83:	c7 80 80 00 00 00 04 	mov    DWORD PTR [eax+0x80],0x4
  420a8a:	00 00 00 
  420a8d:	89 a8 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebp
  420a93:	89 88 88 00 00 00    	mov    DWORD PTR [eax+0x88],ecx
  420a99:	89 88 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ecx
  420a9f:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  420aa5:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  420aa8:	e8 43 dc 01 00       	call   0x43e6f0
  420aad:	8a 87 e0 fd 89 00    	mov    al,BYTE PTR [edi+0x89fde0]
  420ab3:	84 c0                	test   al,al
  420ab5:	74 0e                	je     0x420ac5
  420ab7:	6a 00                	push   0x0
  420ab9:	6a 00                	push   0x0
  420abb:	57                   	push   edi
  420abc:	6a 00                	push   0x0
  420abe:	8b ce                	mov    ecx,esi
  420ac0:	e8 db 01 00 00       	call   0x420ca0
  420ac5:	8a 87 d4 fd 89 00    	mov    al,BYTE PTR [edi+0x89fdd4]
  420acb:	84 c0                	test   al,al
  420acd:	74 57                	je     0x420b26
  420acf:	6a 00                	push   0x0
  420ad1:	6a 00                	push   0x0
  420ad3:	57                   	push   edi
  420ad4:	6a 00                	push   0x0
  420ad6:	8b ce                	mov    ecx,esi
  420ad8:	e8 83 02 00 00       	call   0x420d60
  420add:	eb 47                	jmp    0x420b26
  420adf:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420ae5:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  420ae8:	c7 82 60 03 00 00 00 	mov    DWORD PTR [edx+0x360],0x0
  420aef:	00 00 00 
  420af2:	eb 32                	jmp    0x420b26
  420af4:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  420afa:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  420afd:	8b 88 60 03 00 00    	mov    ecx,DWORD PTR [eax+0x360]
  420b03:	81 e1 fb ff 00 00    	and    ecx,0xfffb
  420b09:	eb 15                	jmp    0x420b20
  420b0b:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420b11:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  420b14:	8b 88 60 03 00 00    	mov    ecx,DWORD PTR [eax+0x360]
  420b1a:	81 e1 f7 ff 00 00    	and    ecx,0xfff7
  420b20:	89 88 60 03 00 00    	mov    DWORD PTR [eax+0x360],ecx
  420b26:	c6 84 37 10 07 00 00 	mov    BYTE PTR [edi+esi*1+0x710],0x0
  420b2d:	00 
  420b2e:	84 db                	test   bl,bl
  420b30:	0f 84 ed 00 00 00    	je     0x420c23
  420b36:	8a 84 37 10 07 00 00 	mov    al,BYTE PTR [edi+esi*1+0x710]
  420b3d:	84 c0                	test   al,al
  420b3f:	0f 85 de 00 00 00    	jne    0x420c23
  420b45:	83 ff 0a             	cmp    edi,0xa
  420b48:	0f 87 d5 00 00 00    	ja     0x420c23
  420b4e:	ff 24 bd 70 0c 42 00 	jmp    DWORD PTR [edi*4+0x420c70]
  420b55:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  420b5b:	33 c9                	xor    ecx,ecx
  420b5d:	51                   	push   ecx
  420b5e:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  420b61:	c7 80 80 00 00 00 ff 	mov    DWORD PTR [eax+0x80],0xffffffff
  420b68:	ff ff ff 
  420b6b:	89 88 84 00 00 00    	mov    DWORD PTR [eax+0x84],ecx
  420b71:	89 88 88 00 00 00    	mov    DWORD PTR [eax+0x88],ecx
  420b77:	89 88 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ecx
  420b7d:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  420b83:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  420b86:	e8 15 db 01 00       	call   0x43e6a0
  420b8b:	8a 87 e0 fd 89 00    	mov    al,BYTE PTR [edi+0x89fde0]
  420b91:	84 c0                	test   al,al
  420b93:	74 1d                	je     0x420bb2
  420b95:	8d 04 7f             	lea    eax,[edi+edi*2]
  420b98:	6a 01                	push   0x1
  420b9a:	8d 0c 80             	lea    ecx,[eax+eax*4]
  420b9d:	8d 14 4d b4 9a 46 00 	lea    edx,[ecx*2+0x469ab4]
  420ba4:	8b ce                	mov    ecx,esi
  420ba6:	52                   	push   edx
  420ba7:	57                   	push   edi
  420ba8:	68 80 02 00 00       	push   0x280
  420bad:	e8 ee 00 00 00       	call   0x420ca0
  420bb2:	8a 87 d4 fd 89 00    	mov    al,BYTE PTR [edi+0x89fdd4]
  420bb8:	84 c0                	test   al,al
  420bba:	74 5f                	je     0x420c1b
  420bbc:	8d 04 7f             	lea    eax,[edi+edi*2]
  420bbf:	6a 01                	push   0x1
  420bc1:	8d 04 80             	lea    eax,[eax+eax*4]
  420bc4:	8d 0c 45 00 9c 46 00 	lea    ecx,[eax*2+0x469c00]
  420bcb:	51                   	push   ecx
  420bcc:	57                   	push   edi
  420bcd:	68 80 fd ff ff       	push   0xfffffd80
  420bd2:	8b ce                	mov    ecx,esi
  420bd4:	e8 87 01 00 00       	call   0x420d60
  420bd9:	eb 40                	jmp    0x420c1b
  420bdb:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  420be1:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  420be4:	c7 80 60 03 00 00 02 	mov    DWORD PTR [eax+0x360],0x2
  420beb:	00 00 00 
  420bee:	eb 2b                	jmp    0x420c1b
  420bf0:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420bf6:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  420bf9:	8b 88 60 03 00 00    	mov    ecx,DWORD PTR [eax+0x360]
  420bff:	83 c9 04             	or     ecx,0x4
  420c02:	eb 11                	jmp    0x420c15
  420c04:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  420c0a:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  420c0d:	8b 88 60 03 00 00    	mov    ecx,DWORD PTR [eax+0x360]
  420c13:	0b cd                	or     ecx,ebp
  420c15:	89 88 60 03 00 00    	mov    DWORD PTR [eax+0x360],ecx
  420c1b:	c6 84 37 10 07 00 00 	mov    BYTE PTR [edi+esi*1+0x710],0x1
  420c22:	01 
  420c23:	47                   	inc    edi
  420c24:	83 ff 0b             	cmp    edi,0xb
  420c27:	0f 8c 1b fe ff ff    	jl     0x420a48
  420c2d:	8a 44 24 2c          	mov    al,BYTE PTR [esp+0x2c]
  420c31:	5f                   	pop    edi
  420c32:	88 86 0f 07 00 00    	mov    BYTE PTR [esi+0x70f],al
  420c38:	5e                   	pop    esi
  420c39:	5d                   	pop    ebp
  420c3a:	5b                   	pop    ebx
  420c3b:	83 c4 18             	add    esp,0x18
  420c3e:	c2 04 00             	ret    0x4
  420c41:	8d 49 00             	lea    ecx,[ecx+0x0]
  420c44:	73 0a                	jae    0x420c50
  420c46:	42                   	inc    edx
  420c47:	00 df                	add    bh,bl
  420c49:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c4c:	ad                   	lods   eax,DWORD PTR ds:[esi]
  420c4d:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c50:	ad                   	lods   eax,DWORD PTR ds:[esi]
  420c51:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c54:	ad                   	lods   eax,DWORD PTR ds:[esi]
  420c55:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c58:	ad                   	lods   eax,DWORD PTR ds:[esi]
  420c59:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c5c:	f4                   	hlt
  420c5d:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c60:	ad                   	lods   eax,DWORD PTR ds:[esi]
  420c61:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c64:	ad                   	lods   eax,DWORD PTR ds:[esi]
  420c65:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c68:	ad                   	lods   eax,DWORD PTR ds:[esi]
  420c69:	0a 42 00             	or     al,BYTE PTR [edx+0x0]
  420c6c:	0b 0b                	or     ecx,DWORD PTR [ebx]
  420c6e:	42                   	inc    edx
  420c6f:	00 55 0b             	add    BYTE PTR [ebp+0xb],dl
  420c72:	42                   	inc    edx
  420c73:	00 db                	add    bl,bl
  420c75:	0b 42 00             	or     eax,DWORD PTR [edx+0x0]
  420c78:	8b 0b                	mov    ecx,DWORD PTR [ebx]
  420c7a:	42                   	inc    edx
  420c7b:	00 8b 0b 42 00 8b    	add    BYTE PTR [ebx-0x74ffbdf5],cl
  420c81:	0b 42 00             	or     eax,DWORD PTR [edx+0x0]
  420c84:	8b 0b                	mov    ecx,DWORD PTR [ebx]
  420c86:	42                   	inc    edx
  420c87:	00 f0                	add    al,dh
  420c89:	0b 42 00             	or     eax,DWORD PTR [edx+0x0]
  420c8c:	8b 0b                	mov    ecx,DWORD PTR [ebx]
  420c8e:	42                   	inc    edx
  420c8f:	00 8b 0b 42 00 8b    	add    BYTE PTR [ebx-0x74ffbdf5],cl
  420c95:	0b 42 00             	or     eax,DWORD PTR [edx+0x0]
  420c98:	04 0c                	add    al,0xc
  420c9a:	42                   	inc    edx
  420c9b:	00 90 90 90 90 56    	add    BYTE PTR [eax+0x56909090],dl
  420ca1:	8b f1                	mov    esi,ecx
  420ca3:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  420ca9:	57                   	push   edi
  420caa:	85 c9                	test   ecx,ecx
  420cac:	0f 84 9c 00 00 00    	je     0x420d4e
  420cb2:	8a 44 24 18          	mov    al,BYTE PTR [esp+0x18]
  420cb6:	84 c0                	test   al,al
  420cb8:	75 29                	jne    0x420ce3
  420cba:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  420cbe:	8b 84 be 70 06 00 00 	mov    eax,DWORD PTR [esi+edi*4+0x670]
  420cc5:	85 c0                	test   eax,eax
  420cc7:	0f 8c 81 00 00 00    	jl     0x420d4e
  420ccd:	50                   	push   eax
  420cce:	e8 1d cb 00 00       	call   0x42d7f0
  420cd3:	c7 84 be 70 06 00 00 	mov    DWORD PTR [esi+edi*4+0x670],0xffffffff
  420cda:	ff ff ff ff 
  420cde:	5f                   	pop    edi
  420cdf:	5e                   	pop    esi
  420ce0:	c2 10 00             	ret    0x10
  420ce3:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420ce9:	e8 02 c5 01 00       	call   0x43d1f0
  420cee:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  420cf2:	8b 3e                	mov    edi,DWORD PTR [esi]
  420cf4:	03 c1                	add    eax,ecx
  420cf6:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420cfc:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  420cff:	8b 4a 24             	mov    ecx,DWORD PTR [edx+0x24]
  420d02:	8b 52 28             	mov    edx,DWORD PTR [edx+0x28]
  420d05:	81 c1 40 01 00 00    	add    ecx,0x140
  420d0b:	81 c2 04 01 00 00    	add    edx,0x104
  420d11:	83 ff 08             	cmp    edi,0x8
  420d14:	7d 06                	jge    0x420d1c
  420d16:	83 e9 36             	sub    ecx,0x36
  420d19:	83 ea 1b             	sub    edx,0x1b
  420d1c:	6a 01                	push   0x1
  420d1e:	6a 00                	push   0x0
  420d20:	50                   	push   eax
  420d21:	52                   	push   edx
  420d22:	51                   	push   ecx
  420d23:	52                   	push   edx
  420d24:	8b 54 24 2c          	mov    edx,DWORD PTR [esp+0x2c]
  420d28:	51                   	push   ecx
  420d29:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  420d2f:	52                   	push   edx
  420d30:	e8 9b c7 00 00       	call   0x42d4d0
  420d35:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  420d39:	6a 01                	push   0x1
  420d3b:	50                   	push   eax
  420d3c:	89 84 8e 70 06 00 00 	mov    DWORD PTR [esi+ecx*4+0x670],eax
  420d43:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  420d49:	e8 92 c9 00 00       	call   0x42d6e0
  420d4e:	5f                   	pop    edi
  420d4f:	5e                   	pop    esi
  420d50:	c2 10 00             	ret    0x10
  420d53:	90                   	nop
  420d54:	90                   	nop
  420d55:	90                   	nop
  420d56:	90                   	nop
  420d57:	90                   	nop
  420d58:	90                   	nop
  420d59:	90                   	nop
  420d5a:	90                   	nop
  420d5b:	90                   	nop
  420d5c:	90                   	nop
  420d5d:	90                   	nop
  420d5e:	90                   	nop
  420d5f:	90                   	nop
  420d60:	56                   	push   esi
  420d61:	8b f1                	mov    esi,ecx
  420d63:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  420d69:	57                   	push   edi
  420d6a:	85 c9                	test   ecx,ecx
  420d6c:	0f 84 9c 00 00 00    	je     0x420e0e
  420d72:	8a 44 24 18          	mov    al,BYTE PTR [esp+0x18]
  420d76:	84 c0                	test   al,al
  420d78:	75 29                	jne    0x420da3
  420d7a:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  420d7e:	8b 84 be ac 06 00 00 	mov    eax,DWORD PTR [esi+edi*4+0x6ac]
  420d85:	85 c0                	test   eax,eax
  420d87:	0f 8c 81 00 00 00    	jl     0x420e0e
  420d8d:	50                   	push   eax
  420d8e:	e8 5d ca 00 00       	call   0x42d7f0
  420d93:	c7 84 be ac 06 00 00 	mov    DWORD PTR [esi+edi*4+0x6ac],0xffffffff
  420d9a:	ff ff ff ff 
  420d9e:	5f                   	pop    edi
  420d9f:	5e                   	pop    esi
  420da0:	c2 10 00             	ret    0x10
  420da3:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420da9:	e8 42 c4 01 00       	call   0x43d1f0
  420dae:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  420db2:	8b 3e                	mov    edi,DWORD PTR [esi]
  420db4:	03 c1                	add    eax,ecx
  420db6:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  420dbc:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  420dbf:	8b 4a 24             	mov    ecx,DWORD PTR [edx+0x24]
  420dc2:	8b 52 28             	mov    edx,DWORD PTR [edx+0x28]
  420dc5:	81 c1 40 01 00 00    	add    ecx,0x140
  420dcb:	81 c2 04 01 00 00    	add    edx,0x104
  420dd1:	83 ff 08             	cmp    edi,0x8
  420dd4:	7d 06                	jge    0x420ddc
  420dd6:	83 e9 36             	sub    ecx,0x36
  420dd9:	83 ea 1b             	sub    edx,0x1b
  420ddc:	6a 01                	push   0x1
  420dde:	6a 00                	push   0x0
  420de0:	50                   	push   eax
  420de1:	52                   	push   edx
  420de2:	51                   	push   ecx
  420de3:	52                   	push   edx
  420de4:	8b 54 24 2c          	mov    edx,DWORD PTR [esp+0x2c]
  420de8:	51                   	push   ecx
  420de9:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  420def:	52                   	push   edx
  420df0:	e8 db c6 00 00       	call   0x42d4d0
  420df5:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  420df9:	6a 01                	push   0x1
  420dfb:	50                   	push   eax
  420dfc:	89 84 8e ac 06 00 00 	mov    DWORD PTR [esi+ecx*4+0x6ac],eax
  420e03:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  420e09:	e8 d2 c8 00 00       	call   0x42d6e0
  420e0e:	5f                   	pop    edi
  420e0f:	5e                   	pop    esi
  420e10:	c2 10 00             	ret    0x10
  420e13:	90                   	nop
  420e14:	90                   	nop
  420e15:	90                   	nop
  420e16:	90                   	nop
  420e17:	90                   	nop
  420e18:	90                   	nop
  420e19:	90                   	nop
  420e1a:	90                   	nop
  420e1b:	90                   	nop
  420e1c:	90                   	nop
  420e1d:	90                   	nop
  420e1e:	90                   	nop
  420e1f:	90                   	nop
  420e20:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  420e24:	53                   	push   ebx
  420e25:	32 db                	xor    bl,bl
  420e27:	8b 14 85 94 fd 89 00 	mov    edx,DWORD PTR [eax*4+0x89fd94]
  420e2e:	8b 82 9c 0b 00 00    	mov    eax,DWORD PTR [edx+0xb9c]
  420e34:	8a 54 24 0c          	mov    dl,BYTE PTR [esp+0xc]
  420e38:	84 d2                	test   dl,dl
  420e3a:	74 7f                	je     0x420ebb
  420e3c:	83 f8 03             	cmp    eax,0x3
  420e3f:	7f 3d                	jg     0x420e7e
  420e41:	85 c0                	test   eax,eax
  420e43:	7c 39                	jl     0x420e7e
  420e45:	83 c0 04             	add    eax,0x4
  420e48:	50                   	push   eax
  420e49:	e8 22 1c 00 00       	call   0x422a70
  420e4e:	85 c0                	test   eax,eax
  420e50:	7c 69                	jl     0x420ebb
  420e52:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  420e59:	8b 88 2c 07 00 00    	mov    ecx,DWORD PTR [eax+0x72c]
  420e5f:	85 c9                	test   ecx,ecx
  420e61:	7c 58                	jl     0x420ebb
  420e63:	8b 80 a4 0b 00 00    	mov    eax,DWORD PTR [eax+0xba4]
  420e69:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  420e6c:	8a 81 58 03 00 00    	mov    al,BYTE PTR [ecx+0x358]
  420e72:	84 c0                	test   al,al
  420e74:	74 45                	je     0x420ebb
  420e76:	b3 01                	mov    bl,0x1
  420e78:	8a c3                	mov    al,bl
  420e7a:	5b                   	pop    ebx
  420e7b:	c2 08 00             	ret    0x8
  420e7e:	83 f8 18             	cmp    eax,0x18
  420e81:	7c 38                	jl     0x420ebb
  420e83:	83 f8 1b             	cmp    eax,0x1b
  420e86:	7f 33                	jg     0x420ebb
  420e88:	83 c0 fc             	add    eax,0xfffffffc
  420e8b:	50                   	push   eax
  420e8c:	e8 df 1b 00 00       	call   0x422a70
  420e91:	85 c0                	test   eax,eax
  420e93:	7c 26                	jl     0x420ebb
  420e95:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  420e9c:	8b 88 2c 07 00 00    	mov    ecx,DWORD PTR [eax+0x72c]
  420ea2:	85 c9                	test   ecx,ecx
  420ea4:	7c 15                	jl     0x420ebb
  420ea6:	8b 90 a4 0b 00 00    	mov    edx,DWORD PTR [eax+0xba4]
  420eac:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  420eaf:	8a 88 58 03 00 00    	mov    cl,BYTE PTR [eax+0x358]
  420eb5:	84 c9                	test   cl,cl
  420eb7:	74 02                	je     0x420ebb
  420eb9:	b3 01                	mov    bl,0x1
  420ebb:	8a c3                	mov    al,bl
  420ebd:	5b                   	pop    ebx
  420ebe:	c2 08 00             	ret    0x8
  420ec1:	90                   	nop
  420ec2:	90                   	nop
  420ec3:	90                   	nop
  420ec4:	90                   	nop
  420ec5:	90                   	nop
  420ec6:	90                   	nop
  420ec7:	90                   	nop
  420ec8:	90                   	nop
  420ec9:	90                   	nop
  420eca:	90                   	nop
  420ecb:	90                   	nop
  420ecc:	90                   	nop
  420ecd:	90                   	nop
  420ece:	90                   	nop
  420ecf:	90                   	nop
  420ed0:	83 ec 10             	sub    esp,0x10
  420ed3:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  420ed7:	53                   	push   ebx
  420ed8:	55                   	push   ebp
  420ed9:	56                   	push   esi
  420eda:	57                   	push   edi
  420edb:	33 ed                	xor    ebp,ebp
  420edd:	8b f9                	mov    edi,ecx
  420edf:	3b c5                	cmp    eax,ebp
  420ee1:	89 7c 24 14          	mov    DWORD PTR [esp+0x14],edi
  420ee5:	0f 85 a5 00 00 00    	jne    0x420f90
  420eeb:	e8 c0 1b 00 00       	call   0x422ab0
  420ef0:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  420ef6:	c7 87 e4 0c 00 00 bb 	mov    DWORD PTR [edi+0xce4],0xbbb
  420efd:	0b 00 00 
  420f00:	3b c5                	cmp    eax,ebp
  420f02:	0f 8e 7a 07 00 00    	jle    0x421682
  420f08:	33 f6                	xor    esi,esi
  420f0a:	3b c5                	cmp    eax,ebp
  420f0c:	0f 8e 70 07 00 00    	jle    0x421682
  420f12:	8d 87 88 00 00 00    	lea    eax,[edi+0x88]
  420f18:	bb a1 0f 00 00       	mov    ebx,0xfa1
  420f1d:	bd 06 00 00 00       	mov    ebp,0x6
  420f22:	8b 08                	mov    ecx,DWORD PTR [eax]
  420f24:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  420f2b:	85 c9                	test   ecx,ecx
  420f2d:	74 49                	je     0x420f78
  420f2f:	8b 97 d0 0d 00 00    	mov    edx,DWORD PTR [edi+0xdd0]
  420f35:	85 d2                	test   edx,edx
  420f37:	7e 19                	jle    0x420f52
  420f39:	89 99 e8 0c 00 00    	mov    DWORD PTR [ecx+0xce8],ebx
  420f3f:	8b 10                	mov    edx,DWORD PTR [eax]
  420f41:	8b 0c 95 94 fd 89 00 	mov    ecx,DWORD PTR [edx*4+0x89fd94]
  420f48:	8b 17                	mov    edx,DWORD PTR [edi]
  420f4a:	89 91 e0 0d 00 00    	mov    DWORD PTR [ecx+0xde0],edx
  420f50:	eb 26                	jmp    0x420f78
  420f52:	8b 97 48 08 00 00    	mov    edx,DWORD PTR [edi+0x848]
  420f58:	85 d2                	test   edx,edx
  420f5a:	74 1c                	je     0x420f78
  420f5c:	39 6a 18             	cmp    DWORD PTR [edx+0x18],ebp
  420f5f:	74 17                	je     0x420f78
  420f61:	89 99 e8 0c 00 00    	mov    DWORD PTR [ecx+0xce8],ebx
  420f67:	8b 08                	mov    ecx,DWORD PTR [eax]
  420f69:	8b 14 8d 94 fd 89 00 	mov    edx,DWORD PTR [ecx*4+0x89fd94]
  420f70:	8b 0f                	mov    ecx,DWORD PTR [edi]
  420f72:	89 8a e0 0d 00 00    	mov    DWORD PTR [edx+0xde0],ecx
  420f78:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  420f7e:	46                   	inc    esi
  420f7f:	83 c0 04             	add    eax,0x4
  420f82:	3b f1                	cmp    esi,ecx
  420f84:	7c 9c                	jl     0x420f22
  420f86:	5f                   	pop    edi
  420f87:	5e                   	pop    esi
  420f88:	5d                   	pop    ebp
  420f89:	5b                   	pop    ebx
  420f8a:	83 c4 10             	add    esp,0x10
  420f8d:	c2 04 00             	ret    0x4
  420f90:	83 f8 01             	cmp    eax,0x1
  420f93:	0f 85 35 03 00 00    	jne    0x4212ce
  420f99:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  420f9f:	89 af 68 01 00 00    	mov    DWORD PTR [edi+0x168],ebp
  420fa5:	3b c5                	cmp    eax,ebp
  420fa7:	89 af 6c 01 00 00    	mov    DWORD PTR [edi+0x16c],ebp
  420fad:	c7 87 e4 0c 00 00 b9 	mov    DWORD PTR [edi+0xce4],0xbb9
  420fb4:	0b 00 00 
  420fb7:	0f 8e 48 01 00 00    	jle    0x421105
  420fbd:	8d 97 88 00 00 00    	lea    edx,[edi+0x88]
  420fc3:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  420fc7:	8d 9f 78 01 00 00    	lea    ebx,[edi+0x178]
  420fcd:	89 54 24 24          	mov    DWORD PTR [esp+0x24],edx
  420fd1:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  420fd5:	8b 30                	mov    esi,DWORD PTR [eax]
  420fd7:	3b f5                	cmp    esi,ebp
  420fd9:	0f 8c 01 01 00 00    	jl     0x4210e0
  420fdf:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  420fe6:	c7 81 e8 0c 00 00 a0 	mov    DWORD PTR [ecx+0xce8],0xfa0
  420fed:	0f 00 00 
  420ff0:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  420ff7:	c7 82 e0 0d 00 00 ff 	mov    DWORD PTR [edx+0xde0],0xffffffff
  420ffe:	ff ff ff 
  421001:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421008:	8b 81 7c 08 00 00    	mov    eax,DWORD PTR [ecx+0x87c]
  42100e:	3b c5                	cmp    eax,ebp
  421010:	7e 18                	jle    0x42102a
  421012:	2b 43 fc             	sub    eax,DWORD PTR [ebx-0x4]
  421015:	89 81 7c 08 00 00    	mov    DWORD PTR [ecx+0x87c],eax
  42101b:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421022:	8b 0b                	mov    ecx,DWORD PTR [ebx]
  421024:	29 88 84 08 00 00    	sub    DWORD PTR [eax+0x884],ecx
  42102a:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421031:	8b 81 7c 08 00 00    	mov    eax,DWORD PTR [ecx+0x87c]
  421037:	3b c5                	cmp    eax,ebp
  421039:	7f 0f                	jg     0x42104a
  42103b:	39 a9 2c 07 00 00    	cmp    DWORD PTR [ecx+0x72c],ebp
  421041:	7e 07                	jle    0x42104a
  421043:	e8 a8 21 00 00       	call   0x4231f0
  421048:	eb 10                	jmp    0x42105a
  42104a:	8b 91 78 08 00 00    	mov    edx,DWORD PTR [ecx+0x878]
  421050:	3b c2                	cmp    eax,edx
  421052:	7e 06                	jle    0x42105a
  421054:	89 91 7c 08 00 00    	mov    DWORD PTR [ecx+0x87c],edx
  42105a:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421061:	39 a8 7c 08 00 00    	cmp    DWORD PTR [eax+0x87c],ebp
  421067:	7d 06                	jge    0x42106f
  421069:	89 a8 7c 08 00 00    	mov    DWORD PTR [eax+0x87c],ebp
  42106f:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]

; ===== VA 0x004231D0..0x004232D0 =====
  4231d0:	57                   	push   edi
  4231d1:	8d b9 34 0a 00 00    	lea    edi,[ecx+0xa34]
  4231d7:	b9 1c 00 00 00       	mov    ecx,0x1c
  4231dc:	33 c0                	xor    eax,eax
  4231de:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4231e0:	5f                   	pop    edi
  4231e1:	c3                   	ret
  4231e2:	90                   	nop
  4231e3:	90                   	nop
  4231e4:	90                   	nop
  4231e5:	90                   	nop
  4231e6:	90                   	nop
  4231e7:	90                   	nop
  4231e8:	90                   	nop
  4231e9:	90                   	nop
  4231ea:	90                   	nop
  4231eb:	90                   	nop
  4231ec:	90                   	nop
  4231ed:	90                   	nop
  4231ee:	90                   	nop
  4231ef:	90                   	nop
  4231f0:	56                   	push   esi
  4231f1:	8b f1                	mov    esi,ecx
  4231f3:	8b 86 7c 08 00 00    	mov    eax,DWORD PTR [esi+0x87c]
  4231f9:	85 c0                	test   eax,eax
  4231fb:	0f 8f c2 00 00 00    	jg     0x4232c3
  423201:	6a 01                	push   0x1
  423203:	e8 a8 d5 ff ff       	call   0x4207b0
  423208:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  42320e:	c7 86 2c 07 00 00 ff 	mov    DWORD PTR [esi+0x72c],0xffffffff
  423215:	ff ff ff 
  423218:	c7 86 7c 08 00 00 00 	mov    DWORD PTR [esi+0x87c],0x0
  42321f:	00 00 00 
  423222:	6a ff                	push   0xffffffff
  423224:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  423227:	c6 81 58 03 00 00 00 	mov    BYTE PTR [ecx+0x358],0x0
  42322e:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  423234:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  423237:	e8 d4 bf 01 00       	call   0x43f210
  42323c:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  423242:	c7 86 e4 0c 00 00 b9 	mov    DWORD PTR [esi+0xce4],0xbb9
  423249:	0b 00 00 
  42324c:	85 c0                	test   eax,eax
  42324e:	75 06                	jne    0x423256
  423250:	ff 05 f8 fd 89 00    	inc    DWORD PTR ds:0x89fdf8
  423256:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  42325c:	55                   	push   ebp
  42325d:	57                   	push   edi
  42325e:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  423261:	8b 78 24             	mov    edi,DWORD PTR [eax+0x24]
  423264:	8b 68 28             	mov    ebp,DWORD PTR [eax+0x28]
  423267:	81 c7 25 01 00 00    	add    edi,0x125
  42326d:	81 c5 f6 00 00 00    	add    ebp,0xf6
  423273:	e8 78 9f 01 00       	call   0x43d1f0
  423278:	6a 01                	push   0x1
  42327a:	50                   	push   eax
  42327b:	55                   	push   ebp
  42327c:	57                   	push   edi
  42327d:	8d 86 d4 0a 00 00    	lea    eax,[esi+0xad4]
  423283:	6a 00                	push   0x0
  423285:	50                   	push   eax
  423286:	8b ce                	mov    ecx,esi
  423288:	e8 73 c4 ff ff       	call   0x41f700
  42328d:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  423293:	50                   	push   eax
  423294:	e8 27 a7 00 00       	call   0x42d9c0
  423299:	8b ce                	mov    ecx,esi
  42329b:	e8 30 ff ff ff       	call   0x4231d0
  4232a0:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4232a2:	51                   	push   ecx
  4232a3:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4232a8:	e8 b3 a9 01 00       	call   0x43dc60
  4232ad:	8b 16                	mov    edx,DWORD PTR [esi]
  4232af:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4232b4:	52                   	push   edx
  4232b5:	e8 a6 a9 01 00       	call   0x43dc60
  4232ba:	8b ce                	mov    ecx,esi
  4232bc:	e8 af 01 00 00       	call   0x423470
  4232c1:	5f                   	pop    edi
  4232c2:	5d                   	pop    ebp
  4232c3:	5e                   	pop    esi
  4232c4:	c3                   	ret
  4232c5:	90                   	nop
  4232c6:	90                   	nop
  4232c7:	90                   	nop
  4232c8:	90                   	nop
  4232c9:	90                   	nop
  4232ca:	90                   	nop
  4232cb:	90                   	nop
  4232cc:	90                   	nop
  4232cd:	90                   	nop
  4232ce:	90                   	nop
  4232cf:	90                   	nop
  4232d0:	53                   	push   ebx

; ===== VA 0x00442650..0x00442730 =====
  442653:	8d 8d c4 02 00 00    	lea    ecx,[ebp+0x2c4]
  442659:	51                   	push   ecx
  44265a:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  44265e:	03 c1                	add    eax,ecx
  442660:	8d 95 54 02 00 00    	lea    edx,[ebp+0x254]
  442666:	52                   	push   edx
  442667:	8b 16                	mov    edx,DWORD PTR [esi]
  442669:	8b 0c 30             	mov    ecx,DWORD PTR [eax+esi*1]
  44266c:	8b 86 68 1e ff ff    	mov    eax,DWORD PTR [esi-0xe198]
  442672:	51                   	push   ecx
  442673:	53                   	push   ebx
  442674:	52                   	push   edx
  442675:	50                   	push   eax
  442676:	8d 8f 64 f4 ff ff    	lea    ecx,[edi-0xb9c]
  44267c:	e8 2f cc fd ff       	call   0x41f2b0
  442681:	8b 0e                	mov    ecx,DWORD PTR [esi]
  442683:	43                   	inc    ebx
  442684:	89 0f                	mov    DWORD PTR [edi],ecx
  442686:	8b 16                	mov    edx,DWORD PTR [esi]
  442688:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  44268b:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  442691:	83 c6 04             	add    esi,0x4
  442694:	81 c7 e4 0d 00 00    	add    edi,0xde4
  44269a:	83 fb 10             	cmp    ebx,0x10
  44269d:	88 44 24 13          	mov    BYTE PTR [esp+0x13],al
  4426a1:	7c a2                	jl     0x442645
  4426a3:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4426a7:	85 c0                	test   eax,eax
  4426a9:	0f 85 a0 00 00 00    	jne    0x44274f
  4426af:	8b 42 3c             	mov    eax,DWORD PTR [edx+0x3c]
  4426b2:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  4426b9:	00 
  4426ba:	85 c0                	test   eax,eax
  4426bc:	0f 8e 8d 00 00 00    	jle    0x44274f
  4426c2:	bb 68 00 00 00       	mov    ebx,0x68
  4426c7:	8b 04 13             	mov    eax,DWORD PTR [ebx+edx*1]
  4426ca:	85 c0                	test   eax,eax
  4426cc:	7e 41                	jle    0x44270f
  4426ce:	83 f8 09             	cmp    eax,0x9
  4426d1:	7d 3c                	jge    0x44270f
  4426d3:	8b 44 13 d8          	mov    eax,DWORD PTR [ebx+edx*1-0x28]
  4426d7:	8b 74 13 ac          	mov    esi,DWORD PTR [ebx+edx*1-0x54]
  4426db:	8d 0c c0             	lea    ecx,[eax+eax*8]
  4426de:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  4426e1:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  4426e4:	8d 04 c8             	lea    eax,[eax+ecx*8]
  4426e7:	b9 d4 00 00 00       	mov    ecx,0xd4
  4426ec:	8d 44 85 00          	lea    eax,[ebp+eax*4+0x0]
  4426f0:	8d b8 50 7b 00 00    	lea    edi,[eax+0x7b50]
  4426f6:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  4426f8:	8d 88 04 73 00 00    	lea    ecx,[eax+0x7304]
  4426fe:	c7 80 ec 7b 00 00 00 	mov    DWORD PTR [eax+0x7bec],0x0
  442705:	00 00 00 
  442708:	e8 c3 0a fe ff       	call   0x4231d0
  44270d:	eb 23                	jmp    0x442732
  44270f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442715:	50                   	push   eax
  442716:	68 fc c2 46 00       	push   0x46c2fc
  44271b:	e8 f0 f8 fe ff       	call   0x432010
  442720:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442726:	68 d8 c2 46 00       	push   0x46c2d8
  44272b:	6a 00                	push   0x0
  44272d:	e8 3e f7 fe ff       	call   0x431e70

; ===== VA 0x00443D90..0x00443F70 =====
  443d90:	83 ec 1c             	sub    esp,0x1c
  443d93:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  443d97:	53                   	push   ebx
  443d98:	33 db                	xor    ebx,ebx
  443d9a:	57                   	push   edi
  443d9b:	3d 61 01 00 00       	cmp    eax,0x161
  443da0:	8b f9                	mov    edi,ecx
  443da2:	89 5c 24 08          	mov    DWORD PTR [esp+0x8],ebx
  443da6:	74 32                	je     0x443dda
  443da8:	3d 69 01 00 00       	cmp    eax,0x169
  443dad:	0f 85 09 02 00 00    	jne    0x443fbc
  443db3:	e8 ad d7 00 00       	call   0x451565
  443db8:	25 03 00 00 80       	and    eax,0x80000003
  443dbd:	79 05                	jns    0x443dc4
  443dbf:	48                   	dec    eax
  443dc0:	83 c8 fc             	or     eax,0xfffffffc
  443dc3:	40                   	inc    eax
  443dc4:	83 f8 03             	cmp    eax,0x3
  443dc7:	77 59                	ja     0x443e22
  443dc9:	ff 24 85 c4 3f 44 00 	jmp    DWORD PTR [eax*4+0x443fc4]
  443dd0:	c7 44 24 08 91 00 00 	mov    DWORD PTR [esp+0x8],0x91
  443dd7:	00 
  443dd8:	eb 48                	jmp    0x443e22
  443dda:	e8 86 d7 00 00       	call   0x451565
  443ddf:	25 01 00 00 80       	and    eax,0x80000001
  443de4:	79 05                	jns    0x443deb
  443de6:	48                   	dec    eax
  443de7:	83 c8 fe             	or     eax,0xfffffffe
  443dea:	40                   	inc    eax
  443deb:	2b c3                	sub    eax,ebx
  443ded:	74 0d                	je     0x443dfc
  443def:	48                   	dec    eax
  443df0:	75 30                	jne    0x443e22
  443df2:	c7 44 24 08 7e 00 00 	mov    DWORD PTR [esp+0x8],0x7e
  443df9:	00 
  443dfa:	eb 26                	jmp    0x443e22
  443dfc:	c7 44 24 08 7d 00 00 	mov    DWORD PTR [esp+0x8],0x7d
  443e03:	00 
  443e04:	eb 1c                	jmp    0x443e22
  443e06:	c7 44 24 08 92 00 00 	mov    DWORD PTR [esp+0x8],0x92
  443e0d:	00 
  443e0e:	eb 12                	jmp    0x443e22
  443e10:	c7 44 24 08 93 00 00 	mov    DWORD PTR [esp+0x8],0x93
  443e17:	00 
  443e18:	eb 08                	jmp    0x443e22
  443e1a:	c7 44 24 08 94 00 00 	mov    DWORD PTR [esp+0x8],0x94
  443e21:	00 
  443e22:	55                   	push   ebp
  443e23:	56                   	push   esi
  443e24:	e8 3c d7 00 00       	call   0x451565
  443e29:	25 01 00 00 80       	and    eax,0x80000001
  443e2e:	79 05                	jns    0x443e35
  443e30:	48                   	dec    eax
  443e31:	83 c8 fe             	or     eax,0xfffffffe
  443e34:	40                   	inc    eax
  443e35:	33 c9                	xor    ecx,ecx
  443e37:	33 d2                	xor    edx,edx
  443e39:	40                   	inc    eax
  443e3a:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  443e3e:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
  443e42:	89 44 24 34          	mov    DWORD PTR [esp+0x34],eax
  443e46:	89 4c 24 28          	mov    DWORD PTR [esp+0x28],ecx
  443e4a:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
  443e4e:	8d b7 60 0c 00 00    	lea    esi,[edi+0xc60]
  443e54:	83 cd ff             	or     ebp,0xffffffff
  443e57:	39 ae b0 fe ff ff    	cmp    DWORD PTR [esi-0x150],ebp
  443e5d:	74 63                	je     0x443ec2
  443e5f:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  443e62:	7e 5e                	jle    0x443ec2
  443e64:	8b 8e 20 03 00 00    	mov    ecx,DWORD PTR [esi+0x320]
  443e6a:	85 c9                	test   ecx,ecx
  443e6c:	7c 59                	jl     0x443ec7
  443e6e:	83 f9 08             	cmp    ecx,0x8
  443e71:	7d 54                	jge    0x443ec7
  443e73:	8b 96 54 02 00 00    	mov    edx,DWORD PTR [esi+0x254]
  443e79:	c6 44 0c 1c 01       	mov    BYTE PTR [esp+ecx*1+0x1c],0x1
  443e7e:	4a                   	dec    edx
  443e7f:	74 33                	je     0x443eb4
  443e81:	4a                   	dec    edx
  443e82:	74 22                	je     0x443ea6
  443e84:	4a                   	dec    edx
  443e85:	75 40                	jne    0x443ec7
  443e87:	8d 51 ff             	lea    edx,[ecx-0x1]
  443e8a:	85 d2                	test   edx,edx
  443e8c:	7c 39                	jl     0x443ec7
  443e8e:	8d 51 fc             	lea    edx,[ecx-0x4]
  443e91:	85 d2                	test   edx,edx
  443e93:	7c 32                	jl     0x443ec7
  443e95:	c6 44 0c 1b 01       	mov    BYTE PTR [esp+ecx*1+0x1b],0x1
  443e9a:	c6 44 0c 18 01       	mov    BYTE PTR [esp+ecx*1+0x18],0x1
  443e9f:	c6 44 0c 17 01       	mov    BYTE PTR [esp+ecx*1+0x17],0x1
  443ea4:	eb 21                	jmp    0x443ec7
  443ea6:	8d 51 fc             	lea    edx,[ecx-0x4]
  443ea9:	85 d2                	test   edx,edx
  443eab:	7c 1a                	jl     0x443ec7
  443ead:	c6 44 0c 18 01       	mov    BYTE PTR [esp+ecx*1+0x18],0x1
  443eb2:	eb 13                	jmp    0x443ec7
  443eb4:	8d 51 ff             	lea    edx,[ecx-0x1]
  443eb7:	85 d2                	test   edx,edx
  443eb9:	7c 0c                	jl     0x443ec7
  443ebb:	c6 44 0c 1b 01       	mov    BYTE PTR [esp+ecx*1+0x1b],0x1
  443ec0:	eb 05                	jmp    0x443ec7
  443ec2:	c6 44 1c 24 01       	mov    BYTE PTR [esp+ebx*1+0x24],0x1
  443ec7:	43                   	inc    ebx
  443ec8:	81 c6 e4 0d 00 00    	add    esi,0xde4
  443ece:	83 fb 08             	cmp    ebx,0x8
  443ed1:	7c 84                	jl     0x443e57
  443ed3:	33 c9                	xor    ecx,ecx
  443ed5:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  443ed9:	8a 54 0c 1c          	mov    dl,BYTE PTR [esp+ecx*1+0x1c]
  443edd:	84 d2                	test   dl,dl
  443edf:	0f 85 c7 00 00 00    	jne    0x443fac
  443ee5:	85 c0                	test   eax,eax
  443ee7:	0f 8e bf 00 00 00    	jle    0x443fac
  443eed:	33 f6                	xor    esi,esi
  443eef:	8a 5c 34 24          	mov    bl,BYTE PTR [esp+esi*1+0x24]
  443ef3:	8d 54 34 24          	lea    edx,[esp+esi*1+0x24]
  443ef7:	84 db                	test   bl,bl
  443ef9:	89 54 24 18          	mov    DWORD PTR [esp+0x18],edx
  443efd:	0f 84 9f 00 00 00    	je     0x443fa2
  443f03:	8d 04 f6             	lea    eax,[esi+esi*8]
  443f06:	8d 04 86             	lea    eax,[esi+eax*4]
  443f09:	8d 14 40             	lea    edx,[eax+eax*2]
  443f0c:	8d 04 d6             	lea    eax,[esi+edx*8]
  443f0f:	8d 97 c4 02 00 00    	lea    edx,[edi+0x2c4]
  443f15:	52                   	push   edx
  443f16:	8b 97 4c 02 00 00    	mov    edx,DWORD PTR [edi+0x24c]
  443f1c:	8d 2c 87             	lea    ebp,[edi+eax*4]
  443f1f:	8d 87 54 02 00 00    	lea    eax,[edi+0x254]
  443f25:	50                   	push   eax
  443f26:	8b 04 b2             	mov    eax,DWORD PTR [edx+esi*4]
  443f29:	50                   	push   eax
  443f2a:	56                   	push   esi
  443f2b:	51                   	push   ecx
  443f2c:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  443f30:	8d 9d e4 03 00 00    	lea    ebx,[ebp+0x3e4]
  443f36:	51                   	push   ecx
  443f37:	8b cb                	mov    ecx,ebx
  443f39:	e8 72 b3 fd ff       	call   0x41f2b0
  443f3e:	8b cb                	mov    ecx,ebx
  443f40:	e8 8b f2 fd ff       	call   0x4231d0
  443f45:	6a 00                	push   0x0
  443f47:	6a 00                	push   0x0
  443f49:	6a 67                	push   0x67
  443f4b:	8b cb                	mov    ecx,ebx
  443f4d:	e8 ce e1 fd ff       	call   0x422120
  443f52:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  443f56:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  443f5a:	89 94 b7 a4 03 00 00 	mov    DWORD PTR [edi+esi*4+0x3a4],edx
  443f61:	89 84 b7 3c e5 00 00 	mov    DWORD PTR [edi+esi*4+0xe53c],eax
  443f68:	8b 8d 5c 0c 00 00    	mov    ecx,DWORD PTR [ebp+0xc5c]
  443f6e:	c6 44 04 1c 01       	mov    BYTE PTR [esp+eax*1+0x1c],0x1

