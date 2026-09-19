  40b1e1:	85 c0                	test   eax,eax
  40b1e3:	74 13                	je     0x40b1f8
  40b1e5:	50                   	push   eax
  40b1e6:	e8 65 63 04 00       	call   0x451550
  40b1eb:	83 c4 04             	add    esp,0x4
  40b1ee:	c7 05 20 f8 89 00 00 	mov    DWORD PTR ds:0x89f820,0x0
  40b1f5:	00 00 00 
  40b1f8:	c3                   	ret
  40b1f9:	90                   	nop
  40b1fa:	90                   	nop
  40b1fb:	90                   	nop
  40b1fc:	90                   	nop
  40b1fd:	90                   	nop
  40b1fe:	90                   	nop
  40b1ff:	90                   	nop
  40b200:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  40b204:	a3 04 f8 89 00       	mov    ds:0x89f804,eax
  40b209:	c3                   	ret
  40b20a:	90                   	nop
  40b20b:	90                   	nop
  40b20c:	90                   	nop
  40b20d:	90                   	nop
  40b20e:	90                   	nop
  40b20f:	90                   	nop
  40b210:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  40b214:	6a 01                	push   0x1
  40b216:	68 24 f8 89 00       	push   0x89f824
  40b21b:	50                   	push   eax
  40b21c:	c6 05 1c f8 89 00 01 	mov    BYTE PTR ds:0x89f81c,0x1
  40b223:	e8 88 73 ff ff       	call   0x4025b0
  40b228:	83 c4 0c             	add    esp,0xc
  40b22b:	c3                   	ret
  40b22c:	90                   	nop
  40b22d:	90                   	nop
  40b22e:	90                   	nop
  40b22f:	90                   	nop
  40b230:	56                   	push   esi
  40b231:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  40b235:	85 f6                	test   esi,esi
  40b237:	0f 84 8d 00 00 00    	je     0x40b2ca
  40b23d:	a1 08 f8 89 00       	mov    eax,ds:0x89f808
  40b242:	85 c0                	test   eax,eax
  40b244:	0f 85 80 00 00 00    	jne    0x40b2ca
  40b24a:	a0 1e f8 89 00       	mov    al,ds:0x89f81e
  40b24f:	84 c0                	test   al,al
  40b251:	75 77                	jne    0x40b2ca
  40b253:	e8 e8 00 00 00       	call   0x40b340
  40b258:	8b 0d 20 f8 89 00    	mov    ecx,DWORD PTR ds:0x89f820
  40b25e:	85 c9                	test   ecx,ecx
  40b260:	74 68                	je     0x40b2ca
  40b262:	89 35 08 f8 89 00    	mov    DWORD PTR ds:0x89f808,esi
  40b268:	c6 05 b9 8b 46 00 00 	mov    BYTE PTR ds:0x468bb9,0x0
  40b26f:	c1 e6 04             	shl    esi,0x4
  40b272:	8b 54 0e fc          	mov    edx,DWORD PTR [esi+ecx*1-0x4]
  40b276:	8d 44 0e 08          	lea    eax,[esi+ecx*1+0x8]
  40b27a:	89 15 0c f8 89 00    	mov    DWORD PTR ds:0x89f80c,edx
  40b280:	8b 15 d0 f7 89 00    	mov    edx,DWORD PTR ds:0x89f7d0
  40b286:	8b 40 f0             	mov    eax,DWORD PTR [eax-0x10]
  40b289:	85 d2                	test   edx,edx
  40b28b:	a3 14 f8 89 00       	mov    ds:0x89f814,eax
  40b290:	74 38                	je     0x40b2ca
  40b292:	8b 04 08             	mov    eax,DWORD PTR [eax+ecx*1]
  40b295:	03 c1                	add    eax,ecx
  40b297:	8b 00                	mov    eax,DWORD PTR [eax]
  40b299:	83 f8 35             	cmp    eax,0x35
  40b29c:	74 2c                	je     0x40b2ca
  40b29e:	3d 95 00 00 00       	cmp    eax,0x95
  40b2a3:	74 25                	je     0x40b2ca
  40b2a5:	6a 01                	push   0x1
  40b2a7:	6a 01                	push   0x1
  40b2a9:	6a 02                	push   0x2
  40b2ab:	e8 e0 88 ff ff       	call   0x403b90
  40b2b0:	6a 00                	push   0x0
  40b2b2:	6a 00                	push   0x0
  40b2b4:	6a 00                	push   0x0
  40b2b6:	e8 a5 89 ff ff       	call   0x403c60
  40b2bb:	83 c4 18             	add    esp,0x18
  40b2be:	c6 05 77 f6 46 00 01 	mov    BYTE PTR ds:0x46f677,0x1
  40b2c5:	e8 76 00 00 00       	call   0x40b340
  40b2ca:	5e                   	pop    esi
  40b2cb:	c3                   	ret
  40b2cc:	90                   	nop
  40b2cd:	90                   	nop
  40b2ce:	90                   	nop
  40b2cf:	90                   	nop
  40b2d0:	a1 08 f8 89 00       	mov    eax,ds:0x89f808
  40b2d5:	85 c0                	test   eax,eax
  40b2d7:	74 63                	je     0x40b33c
  40b2d9:	a0 1e f8 89 00       	mov    al,ds:0x89f81e
  40b2de:	84 c0                	test   al,al
  40b2e0:	75 5a                	jne    0x40b33c
  40b2e2:	a1 30 f8 89 00       	mov    eax,ds:0x89f830
  40b2e7:	c6 05 1d f8 89 00 00 	mov    BYTE PTR ds:0x89f81d,0x0
  40b2ee:	8b 0d 14 f8 89 00    	mov    ecx,DWORD PTR ds:0x89f814
  40b2f4:	8b 15 10 f8 89 00    	mov    edx,DWORD PTR ds:0x89f810
  40b2fa:	8d 14 91             	lea    edx,[ecx+edx*4]
  40b2fd:	8b 0d 20 f8 89 00    	mov    ecx,DWORD PTR ds:0x89f820
  40b303:	03 d1                	add    edx,ecx
  40b305:	85 c0                	test   eax,eax
  40b307:	75 16                	jne    0x40b31f
  40b309:	8b 02                	mov    eax,DWORD PTR [edx]
  40b30b:	03 c1                	add    eax,ecx
  40b30d:	50                   	push   eax
  40b30e:	e8 9d 00 00 00       	call   0x40b3b0
  40b313:	83 c4 04             	add    esp,0x4
  40b316:	a3 30 f8 89 00       	mov    ds:0x89f830,eax
  40b31b:	85 c0                	test   eax,eax
  40b31d:	74 0c                	je     0x40b32b
  40b31f:	8b 10                	mov    edx,DWORD PTR [eax]
  40b321:	8b c8                	mov    ecx,eax
  40b323:	ff 52 04             	call   DWORD PTR [edx+0x4]
  40b326:	a3 30 f8 89 00       	mov    ds:0x89f830,eax
  40b32b:	8a 0d 1d f8 89 00    	mov    cl,BYTE PTR ds:0x89f81d
  40b331:	84 c9                	test   cl,cl
  40b333:	74 b9                	je     0x40b2ee
  40b335:	c6 05 1d f8 89 00 00 	mov    BYTE PTR ds:0x89f81d,0x0
  40b33c:	c3                   	ret
  40b33d:	90                   	nop
  40b33e:	90                   	nop
  40b33f:	90                   	nop
  40b340:	a0 77 f6 46 00       	mov    al,ds:0x46f677
  40b345:	53                   	push   ebx
  40b346:	33 db                	xor    ebx,ebx
  40b348:	3a c3                	cmp    al,bl
  40b34a:	89 1d 08 f8 89 00    	mov    DWORD PTR ds:0x89f808,ebx
  40b350:	89 1d 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,ebx
  40b356:	75 07                	jne    0x40b35f
  40b358:	c6 05 b9 8b 46 00 01 	mov    BYTE PTR ds:0x468bb9,0x1
  40b35f:	38 1d 1c f8 89 00    	cmp    BYTE PTR ds:0x89f81c,bl
  40b365:	74 2d                	je     0x40b394
  40b367:	a1 20 f8 89 00       	mov    eax,ds:0x89f820
  40b36c:	3b c3                	cmp    eax,ebx
  40b36e:	74 09                	je     0x40b379
  40b370:	50                   	push   eax
  40b371:	e8 da 61 04 00       	call   0x451550
  40b376:	83 c4 04             	add    esp,0x4
  40b379:	a1 24 f8 89 00       	mov    eax,ds:0x89f824
  40b37e:	89 1d 24 f8 89 00    	mov    DWORD PTR ds:0x89f824,ebx
  40b384:	a3 20 f8 89 00       	mov    ds:0x89f820,eax
  40b389:	88 1d 1c f8 89 00    	mov    BYTE PTR ds:0x89f81c,bl
  40b38f:	e8 bc 87 ff ff       	call   0x403b50
  40b394:	5b                   	pop    ebx
  40b395:	c3                   	ret
  40b396:	90                   	nop
  40b397:	90                   	nop
  40b398:	90                   	nop
  40b399:	90                   	nop
  40b39a:	90                   	nop
  40b39b:	90                   	nop
  40b39c:	90                   	nop
  40b39d:	90                   	nop
  40b39e:	90                   	nop
  40b39f:	90                   	nop
  40b3a0:	a1 04 f8 89 00       	mov    eax,ds:0x89f804
  40b3a5:	c3                   	ret
  40b3a6:	90                   	nop
  40b3a7:	90                   	nop
  40b3a8:	90                   	nop
  40b3a9:	90                   	nop
  40b3aa:	90                   	nop
  40b3ab:	90                   	nop
  40b3ac:	90                   	nop
  40b3ad:	90                   	nop
  40b3ae:	90                   	nop
  40b3af:	90                   	nop
  40b3b0:	56                   	push   esi
  40b3b1:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  40b3b5:	8b 06                	mov    eax,DWORD PTR [esi]
  40b3b7:	3d 9a 00 00 00       	cmp    eax,0x9a
  40b3bc:	0f 87 ee 09 00 00    	ja     0x40bdb0
