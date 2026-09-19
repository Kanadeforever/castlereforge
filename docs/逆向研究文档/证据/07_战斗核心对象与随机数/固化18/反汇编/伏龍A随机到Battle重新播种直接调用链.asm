; range 0x0040D3EE..0x0040D4E0
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

; range 0x004322C0..0x00432520
  4322c0:	6a ff                	push   0xffffffff
  4322c2:	68 62 ed 45 00       	push   0x45ed62
  4322c7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  4322cd:	50                   	push   eax
  4322ce:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  4322d5:	51                   	push   ecx
  4322d6:	56                   	push   esi
  4322d7:	57                   	push   edi
  4322d8:	8b f9                	mov    edi,ecx
  4322da:	68 1c 06 00 00       	push   0x61c
  4322df:	e8 7b f3 01 00       	call   0x45165f
  4322e4:	83 c4 04             	add    esp,0x4
  4322e7:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4322eb:	85 c0                	test   eax,eax
  4322ed:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  4322f4:	00 
  4322f5:	74 09                	je     0x432300
  4322f7:	8b c8                	mov    ecx,eax
  4322f9:	e8 b2 fc fd ff       	call   0x411fb0
  4322fe:	eb 02                	jmp    0x432302
  432300:	33 c0                	xor    eax,eax
  432302:	8b 0d a8 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01a8
  432308:	83 ce ff             	or     esi,0xffffffff
  43230b:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  43230f:	a3 c8 fc 89 00       	mov    ds:0x89fcc8,eax
  432314:	e8 97 60 ff ff       	call   0x4283b0
  432319:	6a 00                	push   0x0
  43231b:	68 50 c3 00 00       	push   0xc350
  432320:	57                   	push   edi
  432321:	8b cf                	mov    ecx,edi
  432323:	e8 08 21 00 00       	call   0x434430
  432328:	8b cf                	mov    ecx,edi
  43232a:	e8 b1 fe ff ff       	call   0x4321e0
  43232f:	84 c0                	test   al,al
  432331:	75 23                	jne    0x432356
  432333:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432339:	68 44 ac 46 00       	push   0x46ac44
  43233e:	6a 00                	push   0x0
  432340:	e8 2b fb ff ff       	call   0x431e70
  432345:	5f                   	pop    edi
  432346:	5e                   	pop    esi
  432347:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  43234b:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432352:	83 c4 10             	add    esp,0x10
  432355:	c3                   	ret
  432356:	8b cf                	mov    ecx,edi
  432358:	e8 b3 fe ff ff       	call   0x432210
  43235d:	84 c0                	test   al,al
  43235f:	75 23                	jne    0x432384
  432361:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432367:	68 34 ac 46 00       	push   0x46ac34
  43236c:	6a 00                	push   0x0
  43236e:	e8 fd fa ff ff       	call   0x431e70
  432373:	5f                   	pop    edi
  432374:	5e                   	pop    esi
  432375:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  432379:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432380:	83 c4 10             	add    esp,0x10
  432383:	c3                   	ret
  432384:	8b cf                	mov    ecx,edi
  432386:	e8 35 fe ff ff       	call   0x4321c0
  43238b:	84 c0                	test   al,al
  43238d:	75 23                	jne    0x4323b2
  43238f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432395:	68 24 ac 46 00       	push   0x46ac24
  43239a:	6a 00                	push   0x0
  43239c:	e8 cf fa ff ff       	call   0x431e70
  4323a1:	5f                   	pop    edi
  4323a2:	5e                   	pop    esi
  4323a3:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  4323a7:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4323ae:	83 c4 10             	add    esp,0x10
  4323b1:	c3                   	ret
  4323b2:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  4323b7:	53                   	push   ebx
  4323b8:	55                   	push   ebp
  4323b9:	bb 01 00 00 00       	mov    ebx,0x1
  4323be:	68 3c 02 00 00       	push   0x23c
  4323c3:	88 98 48 02 00 00    	mov    BYTE PTR [eax+0x248],bl
  4323c9:	e8 91 f2 01 00       	call   0x45165f
  4323ce:	83 c4 04             	add    esp,0x4
  4323d1:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  4323d5:	85 c0                	test   eax,eax
  4323d7:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
  4323db:	74 09                	je     0x4323e6
  4323dd:	8b c8                	mov    ecx,eax
  4323df:	e8 2c 7d 01 00       	call   0x44a110
  4323e4:	eb 02                	jmp    0x4323e8
  4323e6:	33 c0                	xor    eax,eax
  4323e8:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  4323ee:	89 74 24 1c          	mov    DWORD PTR [esp+0x1c],esi
  4323f2:	a3 28 24 8e 00       	mov    ds:0x8e2428,eax
  4323f7:	8b 72 28             	mov    esi,DWORD PTR [edx+0x28]
  4323fa:	8b 4a 68             	mov    ecx,DWORD PTR [edx+0x68]
  4323fd:	8b 5a 70             	mov    ebx,DWORD PTR [edx+0x70]
  432400:	8b 52 6c             	mov    edx,DWORD PTR [edx+0x6c]
  432403:	8b 6e 08             	mov    ebp,DWORD PTR [esi+0x8]
  432406:	8b 76 04             	mov    esi,DWORD PTR [esi+0x4]
  432409:	8d 5c 5d 00          	lea    ebx,[ebp+ebx*2+0x0]
  43240d:	8d 14 56             	lea    edx,[esi+edx*2]
  432410:	53                   	push   ebx
  432411:	52                   	push   edx
  432412:	51                   	push   ecx
  432413:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  432419:	68 e0 01 00 00       	push   0x1e0
  43241e:	68 80 02 00 00       	push   0x280
  432423:	8b 91 c0 00 00 00    	mov    edx,DWORD PTR [ecx+0xc0]
  432429:	8b c8                	mov    ecx,eax
  43242b:	52                   	push   edx
  43242c:	e8 bf 7d 01 00       	call   0x44a1f0
  432431:	5d                   	pop    ebp
  432432:	5b                   	pop    ebx
  432433:	84 c0                	test   al,al
  432435:	75 23                	jne    0x43245a
  432437:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  43243d:	68 10 ac 46 00       	push   0x46ac10
  432442:	6a 00                	push   0x0
  432444:	e8 27 fa ff ff       	call   0x431e70
  432449:	5f                   	pop    edi
  43244a:	5e                   	pop    esi
  43244b:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  43244f:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432456:	83 c4 10             	add    esp,0x10
  432459:	c3                   	ret
  43245a:	68 00 06 00 00       	push   0x600
  43245f:	e8 fb f1 01 00       	call   0x45165f
  432464:	83 c4 04             	add    esp,0x4
  432467:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  43246b:	85 c0                	test   eax,eax
  43246d:	c7 44 24 14 02 00 00 	mov    DWORD PTR [esp+0x14],0x2
  432474:	00 
  432475:	74 09                	je     0x432480
  432477:	8b c8                	mov    ecx,eax
  432479:	e8 32 8d fe ff       	call   0x41b1b0
  43247e:	eb 02                	jmp    0x432482
  432480:	33 c0                	xor    eax,eax
  432482:	83 ce ff             	or     esi,0xffffffff
  432485:	a3 74 fd 89 00       	mov    ds:0x89fd74,eax
  43248a:	85 c0                	test   eax,eax
  43248c:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  432490:	75 22                	jne    0x4324b4
  432492:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432498:	68 fc ab 46 00       	push   0x46abfc
  43249d:	50                   	push   eax
  43249e:	e8 cd f9 ff ff       	call   0x431e70
  4324a3:	5f                   	pop    edi
  4324a4:	5e                   	pop    esi
  4324a5:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  4324a9:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4324b0:	83 c4 10             	add    esp,0x10
  4324b3:	c3                   	ret
  4324b4:	68 7c e5 00 00       	push   0xe57c
  4324b9:	e8 a1 f1 01 00       	call   0x45165f
  4324be:	83 c4 04             	add    esp,0x4
  4324c1:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4324c5:	85 c0                	test   eax,eax
  4324c7:	c7 44 24 14 03 00 00 	mov    DWORD PTR [esp+0x14],0x3
  4324ce:	00 
  4324cf:	74 09                	je     0x4324da
  4324d1:	8b c8                	mov    ecx,eax
  4324d3:	e8 d8 f7 00 00       	call   0x441cb0
  4324d8:	eb 02                	jmp    0x4324dc
  4324da:	33 c0                	xor    eax,eax
  4324dc:	85 c0                	test   eax,eax
  4324de:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  4324e2:	89 87 18 02 00 00    	mov    DWORD PTR [edi+0x218],eax
  4324e8:	74 0b                	je     0x4324f5
  4324ea:	8b c8                	mov    ecx,eax
  4324ec:	e8 4f fb 00 00       	call   0x442040
  4324f1:	84 c0                	test   al,al
  4324f3:	75 23                	jne    0x432518
  4324f5:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4324fb:	68 e0 ab 46 00       	push   0x46abe0
  432500:	6a 00                	push   0x0
  432502:	e8 69 f9 ff ff       	call   0x431e70
  432507:	5f                   	pop    edi
  432508:	5e                   	pop    esi
  432509:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  43250d:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432514:	83 c4 10             	add    esp,0x10
  432517:	c3                   	ret
  432518:	8b cf                	mov    ecx,edi
  43251a:	e8 51 01 00 00       	call   0x432670
  43251f:	68 a4 03 00 00       	push   0x3a4

; range 0x00442040..0x00442090
  442040:	6a ff                	push   0xffffffff
  442042:	68 9e f9 45 00       	push   0x45f99e
  442047:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  44204d:	50                   	push   eax
  44204e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  442055:	83 ec 70             	sub    esp,0x70
  442058:	53                   	push   ebx
  442059:	55                   	push   ebp
  44205a:	56                   	push   esi
  44205b:	8b e9                	mov    ebp,ecx
  44205d:	33 db                	xor    ebx,ebx
  44205f:	57                   	push   edi
  442060:	53                   	push   ebx
  442061:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  442065:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  442069:	e8 82 f6 ff ff       	call   0x4416f0
  44206e:	53                   	push   ebx
  44206f:	89 9c 24 8c 00 00 00 	mov    DWORD PTR [esp+0x8c],ebx
  442076:	e8 08 f5 00 00       	call   0x451583
  44207b:	50                   	push   eax
  44207c:	e8 da f4 00 00       	call   0x45155b
  442081:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  442086:	83 c4 08             	add    esp,0x8
  442089:	8b 78 68             	mov    edi,DWORD PTR [eax+0x68]
  44208c:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  44208f:	8b 50 6c             	mov    edx,DWORD PTR [eax+0x6c]
