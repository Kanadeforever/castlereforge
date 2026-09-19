; Oracle: RPG.exe SHA256 b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; objdump -d -Mintel, VA 0x40d200..0x40d370

  40d200:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40d206:	6a ff                	push   0xffffffff
  40d208:	68 96 d1 45 00       	push   0x45d196
  40d20d:	50                   	push   eax
  40d20e:	a1 18 f8 89 00       	mov    eax,ds:0x89f818
  40d213:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40d21a:	83 ec 68             	sub    esp,0x68
  40d21d:	53                   	push   ebx
  40d21e:	33 db                	xor    ebx,ebx
  40d220:	55                   	push   ebp
  40d221:	2b c3                	sub    eax,ebx
  40d223:	8b e9                	mov    ebp,ecx
  40d225:	0f 84 c3 01 00 00    	je     0x40d3ee
  40d22b:	48                   	dec    eax
  40d22c:	0f 84 cb 01 00 00    	je     0x40d3fd
  40d232:	48                   	dec    eax
  40d233:	0f 85 18 01 00 00    	jne    0x40d351
  40d239:	53                   	push   ebx
  40d23a:	6a 10                	push   0x10
  40d23c:	e8 df 9c ff ff       	call   0x406f20
  40d241:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d247:	83 c4 08             	add    esp,0x8
  40d24a:	e8 e1 61 ff ff       	call   0x403430
  40d24f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d255:	83 b9 18 01 00 00 02 	cmp    DWORD PTR [ecx+0x118],0x2
  40d25c:	0f 85 e9 00 00 00    	jne    0x40d34b
  40d262:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  40d265:	39 58 08             	cmp    DWORD PTR [eax+0x8],ebx
  40d268:	0f 84 a7 00 00 00    	je     0x40d315
  40d26e:	8d 44 24 0c          	lea    eax,[esp+0xc]
  40d272:	68 84 84 46 00       	push   0x468484
  40d277:	50                   	push   eax
  40d278:	ff 15 94 01 46 00    	call   DWORD PTR ds:0x460194
  40d27e:	83 c4 08             	add    esp,0x8
  40d281:	e8 1a e1 ff ff       	call   0x40b3a0
  40d286:	8b c8                	mov    ecx,eax
  40d288:	e8 93 de ff ff       	call   0x40b120
  40d28d:	e8 3e 9c ff ff       	call   0x406ed0
  40d292:	68 84 02 00 00       	push   0x284
  40d297:	e8 c3 43 04 00       	call   0x45165f
  40d29c:	83 c4 04             	add    esp,0x4
  40d29f:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  40d2a3:	3b c3                	cmp    eax,ebx
  40d2a5:	c7 44 24 78 01 00 00 	mov    DWORD PTR [esp+0x78],0x1
  40d2ac:	00 
  40d2ad:	74 09                	je     0x40d2b8
  40d2af:	8b c8                	mov    ecx,eax
  40d2b1:	e8 ca db ff ff       	call   0x40ae80
  40d2b6:	eb 02                	jmp    0x40d2ba
  40d2b8:	33 c0                	xor    eax,eax
  40d2ba:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  40d2be:	c7 44 24 78 ff ff ff 	mov    DWORD PTR [esp+0x78],0xffffffff
  40d2c5:	ff 
  40d2c6:	51                   	push   ecx
  40d2c7:	8b c8                	mov    ecx,eax
  40d2c9:	e8 72 dc ff ff       	call   0x40af40
  40d2ce:	e8 6d e0 ff ff       	call   0x40b340
  40d2d3:	6a 0b                	push   0xb
  40d2d5:	e8 56 df ff ff       	call   0x40b230
  40d2da:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  40d2e0:	55                   	push   ebp
  40d2e1:	89 9a 18 01 00 00    	mov    DWORD PTR [edx+0x118],ebx
  40d2e7:	89 1d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,ebx
  40d2ed:	c6 05 1d f8 89 00 01 	mov    BYTE PTR ds:0x89f81d,0x1
  40d2f4:	88 1d 1e f8 89 00    	mov    BYTE PTR ds:0x89f81e,bl
  40d2fa:	e8 51 42 04 00       	call   0x451550
  40d2ff:	83 c4 08             	add    esp,0x8
  40d302:	33 c0                	xor    eax,eax
  40d304:	5d                   	pop    ebp
  40d305:	5b                   	pop    ebx
  40d306:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40d30a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40d311:	83 c4 74             	add    esp,0x74
  40d314:	c3                   	ret
  40d315:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  40d318:	55                   	push   ebp
  40d319:	a3 10 f8 89 00       	mov    ds:0x89f810,eax
  40d31e:	89 99 18 01 00 00    	mov    DWORD PTR [ecx+0x118],ebx
  40d324:	89 1d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,ebx
  40d32a:	88 1d 1e f8 89 00    	mov    BYTE PTR ds:0x89f81e,bl
  40d330:	e8 1b 42 04 00       	call   0x451550
  40d335:	83 c4 04             	add    esp,0x4
  40d338:	33 c0                	xor    eax,eax
  40d33a:	5d                   	pop    ebp
  40d33b:	5b                   	pop    ebx
  40d33c:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40d340:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40d347:	83 c4 74             	add    esp,0x74
  40d34a:	c3                   	ret
  40d34b:	89 99 18 01 00 00    	mov    DWORD PTR [ecx+0x118],ebx
  40d351:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d357:	56                   	push   esi
  40d358:	57                   	push   edi
  40d359:	e8 d2 60 ff ff       	call   0x403430
  40d35e:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d364:	e8 27 61 ff ff       	call   0x403490
  40d369:	e8 32 e0 ff ff       	call   0x40b3a0
  40d36e:	8b 88 80 02 00 00    	mov    ecx,DWORD PTR [eax+0x280]
