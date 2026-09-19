; 固化23修正版2：音乐列表 record+0x00 直接consumer审计
; 0x403220构造：MP3LIST→object+8/count+4；WAVLIST→object+10/count+C。
; 0x4032C0和0x403380是仅有的51-byte record命名消费方法，均明确 index*51 + 1。
; 0x46F604全局对象的直接引用路径只调用这些方法/停止/清理或访问object+0x14运行时数组；未见record+0读取。


/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401320 <.text+0x320>:
  401320:	f6 46 00 e8          	test   BYTE PTR [esi+0x0],0xe8
  401324:	37                   	aaa
  401325:	03 05 00 83 c4 04    	add    eax,DWORD PTR ds:0x4c48300
  40132b:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  40132f:	85 c0                	test   eax,eax
  401331:	c7 44 24 10 04 00 00 	mov    DWORD PTR [esp+0x10],0x4
  401338:	00 
  401339:	74 09                	je     0x401344
  40133b:	8b c8                	mov    ecx,eax
  40133d:	e8 de 1e 00 00       	call   0x403220
  401342:	eb 02                	jmp    0x401346
  401344:	33 c0                	xor    eax,eax
  401346:	68 18 02 00 00       	push   0x218
  40134b:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  40134f:	a3 04 f6 46 00       	mov    ds:0x46f604,eax
  401354:	e8 06 03 05 00       	call   0x45165f
  401359:	83 c4 04             	add    esp,0x4
  40135c:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  401360:	85 c0                	test   eax,eax
  401362:	c7 44 24 10 05 00 00 	mov    DWORD PTR [esp+0x10],0x5
  401369:	00 
  40136a:	74 09                	je     0x401375
  40136c:	8b c8                	mov    ecx,eax
  40136e:	e8 8d 90 04 00       	call   0x44a400
  401373:	eb 02                	jmp    0x401377
  401375:	33 c0                	xor    eax,eax
  401377:	6a 04                	push   0x4
  401379:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  40137d:	a3 f4 24 8e 00       	mov    ds:0x8e24f4,eax
  401382:	e8 d8 02 05 00       	call   0x45165f
  401387:	83 c4 04             	add    esp,0x4
  40138a:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  40138e:	85 c0                	test   eax,eax
  401390:	c7 44 24 10 06 00 00 	mov    DWORD PTR [esp+0x10],0x6
  401397:	00 
  401398:	74 09                	je     0x4013a3
  40139a:	8b c8                	mov    ecx,eax
  40139c:	e8 8f 39 00 00       	call   0x404d30
  4013a1:	eb 02                	jmp    0x4013a5
  4013a3:	33 c0                	xor    eax,eax
  4013a5:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4013a9:	a3 b4 f6 46 00       	mov    ds:0x46f6b4,eax
  4013ae:	e8 ad 9c 04 00       	call   0x44b060
  4013b3:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  4013b7:	5e                   	pop    esi
  4013b8:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4013bf:	83 c4 10             	add    esp,0x10
  4013c2:	c3                   	ret
  4013c3:	90                   	nop
  4013c4:	90                   	nop
  4013c5:	90                   	nop
  4013c6:	90                   	nop
  4013c7:	90                   	nop
  4013c8:	90                   	nop
  4013c9:	90                   	nop
  4013ca:	90                   	nop
  4013cb:	90                   	nop
  4013cc:	90                   	nop
  4013cd:	90                   	nop
  4013ce:	90                   	nop
  4013cf:	90                   	nop
  4013d0:	53                   	push   ebx
  4013d1:	56                   	push   esi
  4013d2:	57                   	push   edi
  4013d3:	8b d9                	mov    ebx,ecx
  4013d5:	e8 b6 9c 04 00       	call   0x44b090
  4013da:	8b 35 b4 f6 46 00    	mov    esi,DWORD PTR ds:0x46f6b4
  4013e0:	33 ff                	xor    edi,edi
  4013e2:	3b f7                	cmp    esi,edi
  4013e4:	74 16                	je     0x4013fc
  4013e6:	8b ce                	mov    ecx,esi
  4013e8:	e8 63 36 00 00       	call   0x404a50
  4013ed:	56                   	push   esi
  4013ee:	e8 5d 01 05 00       	call   0x451550
  4013f3:	83 c4 04             	add    esp,0x4
  4013f6:	89 3d b4 f6 46 00    	mov    DWORD PTR ds:0x46f6b4,edi
  4013fc:	8b 35 f4 24 8e 00    	mov    esi,DWORD PTR ds:0x8e24f4
  401402:	3b f7                	cmp    esi,edi
  401404:	74 16                	je     0x40141c
  401406:	8b ce                	mov    ecx,esi
  401408:	e8 63 90 04 00       	call   0x44a470
  40140d:	56                   	push   esi
  40140e:	e8 3d 01 05 00       	call   0x451550
  401413:	83 c4 04             	add    esp,0x4
  401416:	89 3d f4 24 8e 00    	mov    DWORD PTR ds:0x8e24f4,edi
  40141c:	8b 35 04 f6 46 00    	mov    esi,DWORD PTR ds:0x46f604
  401422:	3b f7                	cmp    esi,edi
  401424:	74 16                	je     0x40143c
  401426:	8b ce                	mov    ecx,esi
  401428:	e8 53 1e 00 00       	call   0x403280
  40142d:	56                   	push   esi
  40142e:	e8 1d 01 05 00       	call   0x451550
  401433:	83 c4 04             	add    esp,0x4
  401436:	89 3d 04 f6 46 00    	mov    DWORD PTR ds:0x46f604,edi
  40143c:	8b 35 bc f6 46 00    	mov    esi,DWORD PTR ds:0x46f6bc
  401442:	3b f7                	cmp    esi,edi
  401444:	74 16                	je     0x40145c
  401446:	8b ce                	mov    ecx,esi
  401448:	e8 83 3a 00 00       	call   0x404ed0
  40144d:	56                   	push   esi
  40144e:	e8                   	.byte 0xe8
  40144f:	fd                   	std

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00403220 <.text+0x2220>:
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

; 0x46F604 direct absolute xrefs from .text:
  40134f:	a3 04 f6 46 00       	mov    ds:0x46f604,eax
  40141c:	8b 35 04 f6 46 00    	mov    esi,DWORD PTR ds:0x46f604
  401436:	89 3d 04 f6 46 00    	mov    DWORD PTR ds:0x46f604,edi
  403660:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40366c:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  403689:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  4036ad:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  409368:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  409375:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  409399:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d241:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d351:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d35e:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d386:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d393:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d459:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d6bc:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40efdc:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40efea:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40f001:	8b 15 04 f6 46 00    	mov    edx,DWORD PTR ds:0x46f604
  40f027:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40f039:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40f50c:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40f517:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
