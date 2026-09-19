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
