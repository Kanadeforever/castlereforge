
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040d200 <.text+0xc200>:
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
  40d374:	e8 d7 da ff ff       	call   0x40ae50
  40d379:	8b 38                	mov    edi,DWORD PTR [eax]
  40d37b:	33 c9                	xor    ecx,ecx
  40d37d:	6a 01                	push   0x1
  40d37f:	8a 8f 31 03 00 00    	mov    cl,BYTE PTR [edi+0x331]
  40d385:	51                   	push   ecx
  40d386:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d38c:	e8 2f 5f ff ff       	call   0x4032c0
  40d391:	33 f6                	xor    esi,esi
  40d393:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d399:	33 d2                	xor    edx,edx
  40d39b:	8a 94 37 73 03 00 00 	mov    dl,BYTE PTR [edi+esi*1+0x373]
  40d3a2:	6a 01                	push   0x1
  40d3a4:	52                   	push   edx
  40d3a5:	e8 d6 5f ff ff       	call   0x403380
  40d3aa:	46                   	inc    esi
  40d3ab:	83 fe 05             	cmp    esi,0x5
  40d3ae:	7c e3                	jl     0x40d393
  40d3b0:	8b 15 10 f8 89 00    	mov    edx,DWORD PTR ds:0x89f810
  40d3b6:	55                   	push   ebp
  40d3b7:	42                   	inc    edx
  40d3b8:	89 1d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,ebx
  40d3be:	89 15 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,edx
  40d3c4:	c6 05 1d f8 89 00 01 	mov    BYTE PTR ds:0x89f81d,0x1
  40d3cb:	88 1d 1e f8 89 00    	mov    BYTE PTR ds:0x89f81e,bl
  40d3d1:	e8 7a 41 04 00       	call   0x451550
  40d3d6:	83 c4 04             	add    esp,0x4
  40d3d9:	33 c0                	xor    eax,eax
  40d3db:	5f                   	pop    edi
  40d3dc:	5e                   	pop    esi
  40d3dd:	5d                   	pop    ebp
  40d3de:	5b                   	pop    ebx
  40d3df:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40d3e3:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40d3ea:	83 c4 74             	add    esp,0x74
  40d3ed:	c3                   	ret
  40d3ee:	e8 5d 4d ff ff       	call   0x402150
  40d3f3:	c7 05 18 f8 89 00 01 	mov    DWORD PTR ds:0x89f818,0x1
  40d3fa:	00 00 00 
  40d3fd:	8b 45 04             	mov    eax,DWORD PTR [ebp+0x4]
  40d400:	89 45 08             	mov    DWORD PTR [ebp+0x8],eax
  40d403:	e8 98 df ff ff       	call   0x40b3a0
  40d408:	8b c8                	mov    ecx,eax
  40d40a:	e8 91 dc ff ff       	call   0x40b0a0
  40d40f:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  40d412:	81 78 04 82 00 00 00 	cmp    DWORD PTR [eax+0x4],0x82
  40d419:	75 20                	jne    0x40d43b
  40d41b:	e8 45 41 04 00       	call   0x451565
  40d420:	99                   	cdq
  40d421:	b9 90 01 00 00       	mov    ecx,0x190
  40d426:	f7 f9                	idiv   ecx
  40d428:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  40d42d:	81 c2 20 03 00 00    	add    edx,0x320
  40d433:	89 90 f4 00 00 00    	mov    DWORD PTR [eax+0xf4],edx
  40d439:	eb 0c                	jmp    0x40d447
  40d43b:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d441:	89 99 f4 00 00 00    	mov    DWORD PTR [ecx+0xf4],ebx
  40d447:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
  40d44a:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d450:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  40d453:	89 81 d8 00 00 00    	mov    DWORD PTR [ecx+0xd8],eax
  40d459:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d45f:	e8 2c 60 ff ff       	call   0x403490
  40d464:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  40d46a:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d470:	53                   	push   ebx
  40d471:	68 df 01 00 00       	push   0x1df
  40d476:	8b 42 68             	mov    eax,DWORD PTR [edx+0x68]
  40d479:	8b 91 c0 00 00 00    	mov    edx,DWORD PTR [ecx+0xc0]
  40d47f:	68 7f 02 00 00       	push   0x27f
  40d484:	53                   	push   ebx
  40d485:	53                   	push   ebx
  40d486:	52                   	push   edx
  40d487:	68 e0 01 00 00       	push   0x1e0
  40d48c:	68 80 02 00 00       	push   0x280
  40d491:	6a 30                	push   0x30
  40d493:	6a 40                	push   0x40
  40d495:	50                   	push   eax
  40d496:	68 40 02 00 00       	push   0x240
  40d49b:	68 00 03 00 00       	push   0x300
  40d4a0:	e8 d2 11 04 00       	call   0x44e677
  40d4a5:	68 4c 02 00 00       	push   0x24c
  40d4aa:	e8 b0 41 04 00       	call   0x45165f
  40d4af:	83 c4 38             	add    esp,0x38
  40d4b2:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  40d4b6:	3b c3                	cmp    eax,ebx
  40d4b8:	89 5c 24 78          	mov    DWORD PTR [esp+0x78],ebx
  40d4bc:	74 09                	je     0x40d4c7
  40d4be:	8b c8                	mov    ecx,eax
  40d4c0:	e8 cb 4b 02 00       	call   0x432090
  40d4c5:	eb 02                	jmp    0x40d4c9
  40d4c7:	33 c0                	xor    eax,eax
  40d4c9:	8b c8                	mov    ecx,eax
  40d4cb:	c7 44 24 78 ff ff ff 	mov    DWORD PTR [esp+0x78],0xffffffff
  40d4d2:	ff 
  40d4d3:	a3 f0 01 8b 00       	mov    ds:0x8b01f0,eax
  40d4d8:	e8 e3 4d 02 00       	call   0x4322c0
  40d4dd:	8b 4c 24 70          	mov    ecx,DWORD PTR [esp+0x70]
  40d4e1:	8b c5                	mov    eax,ebp
  40d4e3:	5d                   	pop    ebp
  40d4e4:	c6 05 1e f8 89 00 01 	mov    BYTE PTR ds:0x89f81e,0x1
  40d4eb:	c6 05 1d f8 89 00 01 	mov    BYTE PTR ds:0x89f81d,0x1
  40d4f2:	c7 05 18 f8 89 00 02 	mov    DWORD PTR ds:0x89f818,0x2
  40d4f9:	00 00 00 
  40d4fc:	5b                   	pop    ebx
  40d4fd:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40d504:	83 c4 74             	add    esp,0x74
  40d507:	c3                   	ret
  40d508:	90                   	nop
  40d509:	90                   	nop
  40d50a:	90                   	nop
  40d50b:	90                   	nop
  40d50c:	90                   	nop
  40d50d:	90                   	nop
  40d50e:	90                   	nop
  40d50f:	90                   	nop
