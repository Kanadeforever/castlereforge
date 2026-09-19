  40f2f0:	56                   	push   esi
  40f2f1:	8b f1                	mov    esi,ecx
  40f2f3:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40f2f6:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  40f2f9:	8b 0d d0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7d0
  40f2ff:	3b 48 04             	cmp    ecx,DWORD PTR [eax+0x4]
  40f302:	75 20                	jne    0x40f324
  40f304:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  40f307:	56                   	push   esi
  40f308:	89 15 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,edx
  40f30e:	c7 05 d0 f7 89 00 00 	mov    DWORD PTR ds:0x89f7d0,0x0
  40f315:	00 00 00 
  40f318:	e8 33 22 04 00       	call   0x451550
  40f31d:	83 c4 04             	add    esp,0x4
  40f320:	33 c0                	xor    eax,eax
  40f322:	5e                   	pop    esi
  40f323:	c3                   	ret
  40f324:	8b 48 0c             	mov    ecx,DWORD PTR [eax+0xc]
  40f327:	85 c9                	test   ecx,ecx
  40f329:	75 22                	jne    0x40f34d
  40f32b:	a1 10 f8 89 00       	mov    eax,ds:0x89f810
  40f330:	56                   	push   esi
  40f331:	40                   	inc    eax
  40f332:	c7 05 18 f8 89 00 00 	mov    DWORD PTR ds:0x89f818,0x0
  40f339:	00 00 00 
  40f33c:	a3 10 f8 89 00       	mov    ds:0x89f810,eax
  40f341:	e8 0a 22 04 00       	call   0x451550
  40f346:	83 c4 04             	add    esp,0x4
  40f349:	33 c0                	xor    eax,eax
  40f34b:	5e                   	pop    esi
  40f34c:	c3                   	ret
  40f34d:	6a 01                	push   0x1
  40f34f:	6a 01                	push   0x1
  40f351:	6a 02                	push   0x2
  40f353:	e8 38 48 ff ff       	call   0x403b90
  40f358:	6a 00                	push   0x0
  40f35a:	6a 00                	push   0x0
  40f35c:	6a 00                	push   0x0
  40f35e:	e8 fd 48 ff ff       	call   0x403c60
  40f363:	83 c4 18             	add    esp,0x18
  40f366:	c6 05 1e f8 89 00 01 	mov    BYTE PTR ds:0x89f81e,0x1
  40f36d:	c6 05 1d f8 89 00 01 	mov    BYTE PTR ds:0x89f81d,0x1
  40f374:	e8 c7 bf ff ff       	call   0x40b340
  40f379:	56                   	push   esi
  40f37a:	c6 05 b9 8b 46 00 00 	mov    BYTE PTR ds:0x468bb9,0x0
  40f381:	e8 ca 21 04 00       	call   0x451550
  40f386:	83 c4 04             	add    esp,0x4
  40f389:	33 c0                	xor    eax,eax
  40f38b:	5e                   	pop    esi
  40f38c:	c3                   	ret
  40f38d:	90                   	nop
  40f38e:	90                   	nop
  40f38f:	90                   	nop
  40f390:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  40f393:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
  40f396:	8b 15 d0 f7 89 00    	mov    edx,DWORD PTR ds:0x89f7d0
  40f39c:	85 d2                	test   edx,edx
  40f39e:	75 21                	jne    0x40f3c1
  40f3a0:	a1 10 f8 89 00       	mov    eax,ds:0x89f810
  40f3a5:	51                   	push   ecx
  40f3a6:	40                   	inc    eax
  40f3a7:	c7 05 18 f8 89 00 00 	mov    DWORD PTR ds:0x89f818,0x0
  40f3ae:	00 00 00 
  40f3b1:	a3 10 f8 89 00       	mov    ds:0x89f810,eax
  40f3b6:	e8 95 21 04 00       	call   0x451550
  40f3bb:	83 c4 04             	add    esp,0x4
  40f3be:	33 c0                	xor    eax,eax
  40f3c0:	c3                   	ret
  40f3c1:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  40f3c4:	51                   	push   ecx
  40f3c5:	a3 10 f8 89 00       	mov    ds:0x89f810,eax
  40f3ca:	e8 81 21 04 00       	call   0x451550
  40f3cf:	83 c4 04             	add    esp,0x4
  40f3d2:	33 c0                	xor    eax,eax
  40f3d4:	c3                   	ret
  40f3d5:	90                   	nop
  40f3d6:	90                   	nop
  40f3d7:	90                   	nop
  40f3d8:	90                   	nop
  40f3d9:	90                   	nop
  40f3da:	90                   	nop
  40f3db:	90                   	nop
  40f3dc:	90                   	nop
  40f3dd:	90                   	nop
  40f3de:	90                   	nop
  40f3df:	90                   	nop
  40f3e0:	56                   	push   esi
  40f3e1:	8b f1                	mov    esi,ecx
  40f3e3:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40f3e6:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  40f3e9:	8b 0d 08 f8 89 00    	mov    ecx,DWORD PTR ds:0x89f808
  40f3ef:	89 0d f8 f7 89 00    	mov    DWORD PTR ds:0x89f7f8,ecx
  40f3f5:	8b 56 08             	mov    edx,DWORD PTR [esi+0x8]
  40f3f8:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  40f3fb:	a3 fc f7 89 00       	mov    ds:0x89f7fc,eax
  40f400:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40f403:	8b 51 08             	mov    edx,DWORD PTR [ecx+0x8]
  40f406:	89 15 00 f8 89 00    	mov    DWORD PTR ds:0x89f800,edx
  40f40c:	e8 2f bf ff ff       	call   0x40b340
  40f411:	56                   	push   esi
  40f412:	c6 05 1d f8 89 00 01 	mov    BYTE PTR ds:0x89f81d,0x1
  40f419:	e8 32 21 04 00       	call   0x451550
  40f41e:	83 c4 04             	add    esp,0x4
  40f421:	33 c0                	xor    eax,eax
  40f423:	5e                   	pop    esi
  40f424:	c3                   	ret
