
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00403200 <.text+0x2200>:
  403200:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  403204:	8b ce                	mov    ecx,esi
  403206:	50                   	push   eax
  403207:	e8 c4 f8 ff ff       	call   0x402ad0
  40320c:	5f                   	pop    edi
  40320d:	5e                   	pop    esi
  40320e:	c2 0c 00             	ret    0xc
  403211:	90                   	nop
  403212:	90                   	nop
  403213:	90                   	nop
  403214:	90                   	nop
  403215:	90                   	nop
  403216:	90                   	nop
  403217:	90                   	nop
  403218:	90                   	nop
  403219:	90                   	nop
  40321a:	90                   	nop
  40321b:	90                   	nop
  40321c:	90                   	nop
  40321d:	90                   	nop
  40321e:	90                   	nop
  40321f:	90                   	nop
  403220:	56                   	push   esi
  403221:	8b f1                	mov    esi,ecx
  403223:	6a 03                	push   0x3
  403225:	8d 46 08             	lea    eax,[esi+0x8]
  403228:	50                   	push   eax
  403229:	68 30 84 46 00       	push   0x468430
  40322e:	e8 7d f3 ff ff       	call   0x4025b0
  403233:	8b c8                	mov    ecx,eax
  403235:	b8 a1 a0 a0 a0       	mov    eax,0xa0a0a0a1
  40323a:	f7 e1                	mul    ecx
  40323c:	c1 ea 05             	shr    edx,0x5
  40323f:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  403242:	8d 56 10             	lea    edx,[esi+0x10]
  403245:	6a 03                	push   0x3
  403247:	52                   	push   edx
  403248:	68 24 84 46 00       	push   0x468424
  40324d:	c7 06 00 00 00 00    	mov    DWORD PTR [esi],0x0
  403253:	e8 58 f3 ff ff       	call   0x4025b0
  403258:	8b c8                	mov    ecx,eax
  40325a:	b8 a1 a0 a0 a0       	mov    eax,0xa0a0a0a1
  40325f:	f7 e1                	mul    ecx
  403261:	c1 ea 05             	shr    edx,0x5
  403264:	83 c4 18             	add    esp,0x18
  403267:	8b ce                	mov    ecx,esi
  403269:	89 56 0c             	mov    DWORD PTR [esi+0xc],edx
  40326c:	e8 2f 02 00 00       	call   0x4034a0
  403271:	8b c6                	mov    eax,esi
  403273:	5e                   	pop    esi
  403274:	c3                   	ret
  403275:	90                   	nop
  403276:	90                   	nop
  403277:	90                   	nop
  403278:	90                   	nop
  403279:	90                   	nop
  40327a:	90                   	nop
  40327b:	90                   	nop
  40327c:	90                   	nop
  40327d:	90                   	nop
  40327e:	90                   	nop
  40327f:	90                   	nop
  403280:	56                   	push   esi
  403281:	8b f1                	mov    esi,ecx
  403283:	57                   	push   edi
  403284:	33 ff                	xor    edi,edi
  403286:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
  403289:	89 7e 04             	mov    DWORD PTR [esi+0x4],edi
  40328c:	3b c7                	cmp    eax,edi
  40328e:	74 0c                	je     0x40329c
  403290:	50                   	push   eax
  403291:	e8 ba e2 04 00       	call   0x451550
  403296:	83 c4 04             	add    esp,0x4
  403299:	89 7e 08             	mov    DWORD PTR [esi+0x8],edi
  40329c:	8b 46 10             	mov    eax,DWORD PTR [esi+0x10]
  40329f:	89 7e 0c             	mov    DWORD PTR [esi+0xc],edi
  4032a2:	3b c7                	cmp    eax,edi
  4032a4:	74 0c                	je     0x4032b2
  4032a6:	50                   	push   eax
  4032a7:	e8 a4 e2 04 00       	call   0x451550
  4032ac:	83 c4 04             	add    esp,0x4
  4032af:	89 7e 10             	mov    DWORD PTR [esi+0x10],edi
  4032b2:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4032b8:	e8 f3 23 00 00       	call   0x4056b0
  4032bd:	5f                   	pop    edi
  4032be:	5e                   	pop    esi
  4032bf:	c3                   	ret
  4032c0:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  4032c4:	83 ec 64             	sub    esp,0x64
  4032c7:	85 d2                	test   edx,edx
  4032c9:	56                   	push   esi
  4032ca:	8b f1                	mov    esi,ecx
  4032cc:	0f 84 9c 00 00 00    	je     0x40336e
  4032d2:	3b 16                	cmp    edx,DWORD PTR [esi]
  4032d4:	0f 84 94 00 00 00    	je     0x40336e
  4032da:	83 fa 32             	cmp    edx,0x32
  4032dd:	89 16                	mov    DWORD PTR [esi],edx
  4032df:	7e 0b                	jle    0x4032ec
  4032e1:	52                   	push   edx
  4032e2:	e8 e9 7d 04 00       	call   0x44b0d0
  4032e7:	83 c4 04             	add    esp,0x4
  4032ea:	8b d0                	mov    edx,eax
  4032ec:	57                   	push   edi
  4032ed:	b9 19 00 00 00       	mov    ecx,0x19
  4032f2:	33 c0                	xor    eax,eax
  4032f4:	8d 7c 24 08          	lea    edi,[esp+0x8]
  4032f8:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4032fa:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  4032fd:	8b c2                	mov    eax,edx
  4032ff:	c1 e0 04             	shl    eax,0x4
  403302:	03 c2                	add    eax,edx
  403304:	8d 04 40             	lea    eax,[eax+eax*2]
  403307:	8d 54 08 01          	lea    edx,[eax+ecx*1+0x1]
  40330b:	8d 44 24 08          	lea    eax,[esp+0x8]
  40330f:	52                   	push   edx
  403310:	68 3c 84 46 00       	push   0x46843c
  403315:	50                   	push   eax
  403316:	ff 15 94 01 46 00    	call   DWORD PTR ds:0x460194
  40331c:	8d 4c 24 7c          	lea    ecx,[esp+0x7c]
  403320:	6a 03                	push   0x3
  403322:	8d 54 24 18          	lea    edx,[esp+0x18]
  403326:	51                   	push   ecx
  403327:	52                   	push   edx
  403328:	e8 83 f2 ff ff       	call   0x4025b0
  40332d:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  403333:	83 c4 18             	add    esp,0x18
  403336:	50                   	push   eax
  403337:	8b 44 24 74          	mov    eax,DWORD PTR [esp+0x74]
  40333b:	50                   	push   eax
  40333c:	e8 ef 21 00 00       	call   0x405530
  403341:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  403347:	6a 01                	push   0x1
  403349:	e8 52 22 00 00       	call   0x4055a0
  40334e:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  403354:	8b 81 2c 01 00 00    	mov    eax,DWORD PTR [ecx+0x12c]
  40335a:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  403360:	8d 14 80             	lea    edx,[eax+eax*4]
  403363:	d1 e2                	shl    edx,1
  403365:	52                   	push   edx
  403366:	6a 00                	push   0x0
  403368:	e8 93 1c 00 00       	call   0x405000
  40336d:	5f                   	pop    edi
  40336e:	5e                   	pop    esi
  40336f:	83 c4 64             	add    esp,0x64
  403372:	c2 08 00             	ret    0x8
  403375:	90                   	nop
  403376:	90                   	nop
  403377:	90                   	nop
  403378:	90                   	nop
  403379:	90                   	nop
  40337a:	90                   	nop
  40337b:	90                   	nop
  40337c:	90                   	nop
  40337d:	90                   	nop
  40337e:	90                   	nop
  40337f:	90                   	nop
  403380:	83 ec 64             	sub    esp,0x64
  403383:	53                   	push   ebx
  403384:	56                   	push   esi
  403385:	8b 74 24 70          	mov    esi,DWORD PTR [esp+0x70]
  403389:	8b d9                	mov    ebx,ecx
  40338b:	85 f6                	test   esi,esi
  40338d:	0f 84 95 00 00 00    	je     0x403428
  403393:	57                   	push   edi
  403394:	b9 19 00 00 00       	mov    ecx,0x19
  403399:	33 c0                	xor    eax,eax
  40339b:	8d 7c 24 0c          	lea    edi,[esp+0xc]
  40339f:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4033a1:	8b 4b 10             	mov    ecx,DWORD PTR [ebx+0x10]
  4033a4:	8b c6                	mov    eax,esi
  4033a6:	c1 e0 04             	shl    eax,0x4
  4033a9:	03 c6                	add    eax,esi
  4033ab:	8d 04 40             	lea    eax,[eax+eax*2]
  4033ae:	8d 54 08 01          	lea    edx,[eax+ecx*1+0x1]
  4033b2:	8d 44 24 0c          	lea    eax,[esp+0xc]
  4033b6:	52                   	push   edx
  4033b7:	68 44 84 46 00       	push   0x468444
  4033bc:	50                   	push   eax
  4033bd:	e8 b1 e6 04 00       	call   0x451a73
  4033c2:	8d 8c 24 80 00 00 00 	lea    ecx,[esp+0x80]
  4033c9:	6a 03                	push   0x3
  4033cb:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  4033cf:	51                   	push   ecx
  4033d0:	52                   	push   edx
  4033d1:	e8 da f1 ff ff       	call   0x4025b0
  4033d6:	8b 8c 24 90 00 00 00 	mov    ecx,DWORD PTR [esp+0x90]
  4033dd:	83 c4 18             	add    esp,0x18
  4033e0:	85 c9                	test   ecx,ecx
  4033e2:	5f                   	pop    edi
  4033e3:	74 0f                	je     0x4033f4
  4033e5:	8b 54 24 70          	mov    edx,DWORD PTR [esp+0x70]
  4033e9:	8d 4c b3 14          	lea    ecx,[ebx+esi*4+0x14]
  4033ed:	51                   	push   ecx
  4033ee:	6a 01                	push   0x1
  4033f0:	50                   	push   eax
  4033f1:	52                   	push   edx
  4033f2:	eb 0a                	jmp    0x4033fe
  4033f4:	6a 00                	push   0x0
  4033f6:	6a 00                	push   0x0
  4033f8:	50                   	push   eax
  4033f9:	8b 44 24 7c          	mov    eax,DWORD PTR [esp+0x7c]
  4033fd:	50                   	push   eax
  4033fe:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  403404:	e8 b7 22 00 00       	call   0x4056c0
  403409:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40340f:	8b 81 28 01 00 00    	mov    eax,DWORD PTR [ecx+0x128]
  403415:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  40341b:	8d 14 80             	lea    edx,[eax+eax*4]
  40341e:	d1 e2                	shl    edx,1
  403420:	52                   	push   edx
  403421:	6a 0a                	push   0xa
  403423:	e8 d8 1b 00 00       	call   0x405000
  403428:	5e                   	pop    esi
  403429:	5b                   	pop    ebx
  40342a:	83 c4 64             	add    esp,0x64
  40342d:	c2 08 00             	ret    0x8
  403430:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  403436:	e8 75 22 00 00       	call   0x4056b0
  40343b:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  403441:	6a ff                	push   0xffffffff
  403443:	e8 c8 22 00 00       	call   0x405710
  403448:	c3                   	ret
  403449:	90                   	nop
  40344a:	90                   	nop
  40344b:	90                   	nop
  40344c:	90                   	nop
  40344d:	90                   	nop
  40344e:	90                   	nop
  40344f:	90                   	nop
  403450:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  403456:	e9 55 22 00 00       	jmp    0x4056b0
  40345b:	90                   	nop
  40345c:	90                   	nop
  40345d:	90                   	nop
  40345e:	90                   	nop
  40345f:	90                   	nop
  403460:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  403464:	85 c0                	test   eax,eax
  403466:	75 10                	jne    0x403478
  403468:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  40346e:	6a ff                	push   0xffffffff
  403470:	e8 9b 22 00 00       	call   0x405710
  403475:	c2 04 00             	ret    0x4
  403478:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  40347e:	50                   	push   eax
  40347f:	e8 8c 22 00 00       	call   0x405710
  403484:	c2 04 00             	ret    0x4
  403487:	90                   	nop
  403488:	90                   	nop
  403489:	90                   	nop
  40348a:	90                   	nop
  40348b:	90                   	nop
  40348c:	90                   	nop
  40348d:	90                   	nop
  40348e:	90                   	nop
  40348f:	90                   	nop
  403490:	c7 01 00 00 00 00    	mov    DWORD PTR [ecx],0x0
  403496:	e9 05 00 00 00       	jmp    0x4034a0
  40349b:	90                   	nop
  40349c:	90                   	nop
  40349d:	90                   	nop
  40349e:	90                   	nop
  40349f:	90                   	nop
  4034a0:	8b d1                	mov    edx,ecx
  4034a2:	57                   	push   edi
  4034a3:	b9 32 00 00 00       	mov    ecx,0x32
  4034a8:	33 c0                	xor    eax,eax
  4034aa:	8d 7a 14             	lea    edi,[edx+0x14]
  4034ad:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4034af:	5f                   	pop    edi
  4034b0:	c3                   	ret
  4034b1:	90                   	nop
  4034b2:	90                   	nop
  4034b3:	90                   	nop
  4034b4:	90                   	nop
  4034b5:	90                   	nop
  4034b6:	90                   	nop
  4034b7:	90                   	nop
  4034b8:	90                   	nop
  4034b9:	90                   	nop
  4034ba:	90                   	nop
  4034bb:	90                   	nop
  4034bc:	90                   	nop
  4034bd:	90                   	nop
  4034be:	90                   	nop
  4034bf:	90                   	nop
  4034c0:	a1 14 f6 46 00       	mov    eax,ds:0x46f614
  4034c5:	33 d2                	xor    edx,edx
  4034c7:	3b c2                	cmp    eax,edx
  4034c9:	74 12                	je     0x4034dd
  4034cb:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4034cf:	8b 0d 18 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f618
  4034d5:	03 c8                	add    ecx,eax
  4034d7:	89 0d 18 f6 46 00    	mov    DWORD PTR ds:0x46f618,ecx
  4034dd:	8b 0d 1c f6 46 00    	mov    ecx,DWORD PTR ds:0x46f61c
  4034e3:	3b ca                	cmp    ecx,edx
  4034e5:	74 1b                	je     0x403502
  4034e7:	a1 20 f6 46 00       	mov    eax,ds:0x46f620
  4034ec:	40                   	inc    eax
  4034ed:	3b c1                	cmp    eax,ecx
  4034ef:	a3 20 f6 46 00       	mov    ds:0x46f620,eax
  4034f4:	7e 0c                	jle    0x403502
  4034f6:	89 15 1c f6 46 00    	mov    DWORD PTR ds:0x46f61c,edx
  4034fc:	89 15 20 f6 46 00    	mov    DWORD PTR ds:0x46f620,edx
  403502:	c3                   	ret
  403503:	90                   	nop
  403504:	90                   	nop
  403505:	90                   	nop
  403506:	90                   	nop
  403507:	90                   	nop
  403508:	90                   	nop
  403509:	90                   	nop
  40350a:	90                   	nop
  40350b:	90                   	nop
  40350c:	90                   	nop
  40350d:	90                   	nop
  40350e:	90                   	nop
  40350f:	90                   	nop
  403510:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  403514:	85 c0                	test   eax,eax
  403516:	75 0b                	jne    0x403523
  403518:	c7                   	.byte 0xc7
  403519:	05 14 f6 46 00       	add    eax,0x46f614
	...
