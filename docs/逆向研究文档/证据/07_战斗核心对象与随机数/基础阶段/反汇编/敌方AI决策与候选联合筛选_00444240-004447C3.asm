
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00444240 <.text+0x43240>:
  444240:	83 ec 0c             	sub    esp,0xc
  444243:	53                   	push   ebx
  444244:	55                   	push   ebp
  444245:	56                   	push   esi
  444246:	8b 74 24 1c          	mov    esi,DWORD PTR [esp+0x1c]
  44424a:	8b d9                	mov    ebx,ecx
  44424c:	57                   	push   edi
  44424d:	8d 04 f6             	lea    eax,[esi+esi*8]
  444250:	8d 04 86             	lea    eax,[esi+eax*4]
  444253:	8d 0c 40             	lea    ecx,[eax+eax*2]
  444256:	8d 14 ce             	lea    edx,[esi+ecx*8]
  444259:	8b ac 93 30 0c 00 00 	mov    ebp,DWORD PTR [ebx+edx*4+0xc30]
  444260:	8d 3c 93             	lea    edi,[ebx+edx*4]
  444263:	89 6c 24 10          	mov    DWORD PTR [esp+0x10],ebp
  444267:	e8 f9 d2 00 00       	call   0x451565
  44426c:	99                   	cdq
  44426d:	b9 64 00 00 00       	mov    ecx,0x64
  444272:	c6 44 24 20 01       	mov    BYTE PTR [esp+0x20],0x1
  444277:	f7 f9                	idiv   ecx
  444279:	8b 83 34 e5 00 00    	mov    eax,DWORD PTR [ebx+0xe534]
  44427f:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  444283:	8d 14 ed 00 00 00 00 	lea    edx,[ebp*8+0x0]
  44428a:	2b d5                	sub    edx,ebp
  44428c:	8b 6c d0 20          	mov    ebp,DWORD PTR [eax+edx*8+0x20]
  444290:	8b 4c d0 1c          	mov    ecx,DWORD PTR [eax+edx*8+0x1c]
  444294:	8d 04 d0             	lea    eax,[eax+edx*8]
  444297:	8b 87 d8 0c 00 00    	mov    eax,DWORD PTR [edi+0xcd8]
  44429d:	03 e9                	add    ebp,ecx
  44429f:	85 c0                	test   eax,eax
  4442a1:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  4442a5:	75 02                	jne    0x4442a9
  4442a7:	33 ed                	xor    ebp,ebp
  4442a9:	8b 87 54 0e 00 00    	mov    eax,DWORD PTR [edi+0xe54]
  4442af:	85 c0                	test   eax,eax
  4442b1:	7f 0a                	jg     0x4442bd
  4442b3:	8b 87 20 0e 00 00    	mov    eax,DWORD PTR [edi+0xe20]
  4442b9:	85 c0                	test   eax,eax
  4442bb:	7e 05                	jle    0x4442c2
  4442bd:	c6 44 24 20 00       	mov    BYTE PTR [esp+0x20],0x0
  4442c2:	6a 02                	push   0x2
  4442c4:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4442c9:	e8 82 97 ff ff       	call   0x43da50
  4442ce:	84 c0                	test   al,al
  4442d0:	75 10                	jne    0x4442e2
  4442d2:	6a 02                	push   0x2
  4442d4:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4442d9:	e8 72 97 ff ff       	call   0x43da50
  4442de:	84 c0                	test   al,al
  4442e0:	74 05                	je     0x4442e7
  4442e2:	c6 44 24 20 00       	mov    BYTE PTR [esp+0x20],0x0
  4442e7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4442eb:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4442ef:	3b c1                	cmp    eax,ecx
  4442f1:	7d 05                	jge    0x4442f8
  4442f3:	83 c9 ff             	or     ecx,0xffffffff
  4442f6:	eb 19                	jmp    0x444311
  4442f8:	3b c5                	cmp    eax,ebp
  4442fa:	7d 11                	jge    0x44430d
  4442fc:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  444300:	51                   	push   ecx
  444301:	56                   	push   esi
  444302:	8b cb                	mov    ecx,ebx
  444304:	e8 27 00 00 00       	call   0x444330
  444309:	8b c8                	mov    ecx,eax
  44430b:	eb 04                	jmp    0x444311
  44430d:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  444311:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  444315:	8b c1                	mov    eax,ecx
  444317:	2b c7                	sub    eax,edi
  444319:	5f                   	pop    edi
  44431a:	f7 d8                	neg    eax
  44431c:	1b c0                	sbb    eax,eax
  44431e:	5e                   	pop    esi
  44431f:	5d                   	pop    ebp
  444320:	23 c1                	and    eax,ecx
  444322:	5b                   	pop    ebx
  444323:	83 c4 0c             	add    esp,0xc
  444326:	c2 04 00             	ret    0x4
  444329:	90                   	nop
  44432a:	90                   	nop
  44432b:	90                   	nop
  44432c:	90                   	nop
  44432d:	90                   	nop
  44432e:	90                   	nop
  44432f:	90                   	nop
  444330:	83 ec 70             	sub    esp,0x70
  444333:	53                   	push   ebx
  444334:	8b 5c 24 78          	mov    ebx,DWORD PTR [esp+0x78]
  444338:	55                   	push   ebp
  444339:	89 4c 24 0c          	mov    DWORD PTR [esp+0xc],ecx
  44433d:	8d 04 db             	lea    eax,[ebx+ebx*8]
  444340:	8d 04 83             	lea    eax,[ebx+eax*4]
  444343:	8d 14 40             	lea    edx,[eax+eax*2]
  444346:	8d 04 d3             	lea    eax,[ebx+edx*8]
  444349:	8b ac 81 d8 0c 00 00 	mov    ebp,DWORD PTR [ecx+eax*4+0xcd8]
  444350:	8d 14 81             	lea    edx,[ecx+eax*4]
  444353:	85 ed                	test   ebp,ebp
  444355:	8b 82 30 0c 00 00    	mov    eax,DWORD PTR [edx+0xc30]
  44435b:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  44435f:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  444363:	89 6c 24 20          	mov    DWORD PTR [esp+0x20],ebp
  444367:	0f 8e 1b 02 00 00    	jle    0x444588
  44436d:	8d 4c 24 28          	lea    ecx,[esp+0x28]
  444371:	8d 82 dc 0c 00 00    	lea    eax,[edx+0xcdc]
  444377:	56                   	push   esi
  444378:	57                   	push   edi
  444379:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  44437d:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  444381:	89 6c 24 24          	mov    DWORD PTR [esp+0x24],ebp
  444385:	eb 04                	jmp    0x44438b
  444387:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  44438b:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  44438f:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  444393:	c6 44 24 13 01       	mov    BYTE PTR [esp+0x13],0x1
  444398:	8b 30                	mov    esi,DWORD PTR [eax]
  44439a:	8a 84 24 88 00 00 00 	mov    al,BYTE PTR [esp+0x88]
  4443a1:	c7 01 00 00 00 00    	mov    DWORD PTR [ecx],0x0
  4443a7:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  4443ad:	84 c0                	test   al,al
  4443af:	75 26                	jne    0x4443d7
  4443b1:	8b b9 b4 00 00 00    	mov    edi,DWORD PTR [ecx+0xb4]
  4443b7:	8d 04 f5 00 00 00 00 	lea    eax,[esi*8+0x0]
  4443be:	2b c6                	sub    eax,esi
  4443c0:	8d 04 80             	lea    eax,[eax+eax*4]
  4443c3:	c1 e0 04             	shl    eax,0x4
  4443c6:	83 7c 38 18 02       	cmp    DWORD PTR [eax+edi*1+0x18],0x2
  4443cb:	75 0a                	jne    0x4443d7
  4443cd:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  4443d2:	e9 0a 01 00 00       	jmp    0x4444e1
  4443d7:	8b 89 b4 00 00 00    	mov    ecx,DWORD PTR [ecx+0xb4]
  4443dd:	8d 04 f5 00 00 00 00 	lea    eax,[esi*8+0x0]
  4443e4:	2b c6                	sub    eax,esi
  4443e6:	8b 9a 68 0c 00 00    	mov    ebx,DWORD PTR [edx+0xc68]
  4443ec:	8d 04 80             	lea    eax,[eax+eax*4]
  4443ef:	c1 e0 04             	shl    eax,0x4
  4443f2:	03 c1                	add    eax,ecx
  4443f4:	3b 58 38             	cmp    ebx,DWORD PTR [eax+0x38]
  4443f7:	7c 0d                	jl     0x444406
  4443f9:	8b ba 60 0c 00 00    	mov    edi,DWORD PTR [edx+0xc60]
  4443ff:	8b 48 34             	mov    ecx,DWORD PTR [eax+0x34]
  444402:	3b f9                	cmp    edi,ecx
  444404:	7d 0a                	jge    0x444410
  444406:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  44440b:	e9 d1 00 00 00       	jmp    0x4444e1
  444410:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  444414:	8b d6                	mov    edx,esi
  444416:	c1 e2 06             	shl    edx,0x6
  444419:	8b 88 38 e5 00 00    	mov    ecx,DWORD PTR [eax+0xe538]
  44441f:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  444424:	8d 2c 11             	lea    ebp,[ecx+edx*1]
  444427:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  44442b:	8b 8a 5c 0c 00 00    	mov    ecx,DWORD PTR [edx+0xc5c]
  444431:	0f af 4d 18          	imul   ecx,DWORD PTR [ebp+0x18]
  444435:	f7 e9                	imul   ecx
  444437:	c1 fa 05             	sar    edx,0x5
  44443a:	8b c2                	mov    eax,edx
  44443c:	c1 e8 1f             	shr    eax,0x1f
  44443f:	03 d0                	add    edx,eax
  444441:	3b fa                	cmp    edi,edx
  444443:	7e 0a                	jle    0x44444f
  444445:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  44444a:	e9 92 00 00 00       	jmp    0x4444e1
  44444f:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  444453:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  444458:	8b 89 64 0c 00 00    	mov    ecx,DWORD PTR [ecx+0xc64]
  44445e:	0f af 4d 1c          	imul   ecx,DWORD PTR [ebp+0x1c]
  444462:	f7 e9                	imul   ecx
  444464:	c1 fa 05             	sar    edx,0x5
  444467:	8b c2                	mov    eax,edx
  444469:	c1 e8 1f             	shr    eax,0x1f
  44446c:	03 d0                	add    edx,eax
  44446e:	3b da                	cmp    ebx,edx
  444470:	7e 07                	jle    0x444479
  444472:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  444477:	eb 68                	jmp    0x4444e1
  444479:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  44447d:	33 d2                	xor    edx,edx
  44447f:	33 ff                	xor    edi,edi
  444481:	bb 08 00 00 00       	mov    ebx,0x8
  444486:	8d 81 60 0c 00 00    	lea    eax,[ecx+0xc60]
  44448c:	8b 08                	mov    ecx,DWORD PTR [eax]
  44448e:	85 c9                	test   ecx,ecx
  444490:	7e 07                	jle    0x444499
  444492:	03 d1                	add    edx,ecx
  444494:	8b 48 fc             	mov    ecx,DWORD PTR [eax-0x4]
  444497:	03 f9                	add    edi,ecx
  444499:	05 e4 0d 00 00       	add    eax,0xde4
  44449e:	4b                   	dec    ebx
  44449f:	75 eb                	jne    0x44448c
  4444a1:	8b 4d 20             	mov    ecx,DWORD PTR [ebp+0x20]
  4444a4:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4444a9:	0f af ca             	imul   ecx,edx
  4444ac:	f7 e9                	imul   ecx
  4444ae:	c1 fa 05             	sar    edx,0x5
  4444b1:	8b c2                	mov    eax,edx
  4444b3:	c1 e8 1f             	shr    eax,0x1f
  4444b6:	03 d0                	add    edx,eax
  4444b8:	3b d7                	cmp    edx,edi
  4444ba:	7e 05                	jle    0x4444c1
  4444bc:	c6 44 24 13 01       	mov    BYTE PTR [esp+0x13],0x1
  4444c1:	83 7d 28 01          	cmp    DWORD PTR [ebp+0x28],0x1
  4444c5:	75 1a                	jne    0x4444e1
  4444c7:	8b 8c 24 84 00 00 00 	mov    ecx,DWORD PTR [esp+0x84]
  4444ce:	56                   	push   esi
  4444cf:	51                   	push   ecx
  4444d0:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  4444d4:	e8 b7 00 00 00       	call   0x444590
  4444d9:	83 f8 ff             	cmp    eax,0xffffffff
  4444dc:	0f 95 44 24 13       	setne  BYTE PTR [esp+0x13]
  4444e1:	e8 7f d0 00 00       	call   0x451565
  4444e6:	8a 4c 24 13          	mov    cl,BYTE PTR [esp+0x13]
  4444ea:	84 c9                	test   cl,cl
  4444ec:	74 18                	je     0x444506
  4444ee:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  4444f2:	c1 e6 06             	shl    esi,0x6
  4444f5:	8b 8a 38 e5 00 00    	mov    ecx,DWORD PTR [edx+0xe538]
  4444fb:	99                   	cdq
  4444fc:	f7 7c 31 24          	idiv   DWORD PTR [ecx+esi*1+0x24]
  444500:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  444504:	89 10                	mov    DWORD PTR [eax],edx
  444506:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  44450a:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  44450e:	b8 04 00 00 00       	mov    eax,0x4
  444513:	03 d0                	add    edx,eax
  444515:	03 c8                	add    ecx,eax
  444517:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  44451b:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
  44451f:	48                   	dec    eax
  444520:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  444524:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  444528:	0f 85 59 fe ff ff    	jne    0x444387
  44452e:	8b 6c 24 28          	mov    ebp,DWORD PTR [esp+0x28]
  444532:	8b 9c 24 84 00 00 00 	mov    ebx,DWORD PTR [esp+0x84]
  444539:	83 ce ff             	or     esi,0xffffffff
  44453c:	33 d2                	xor    edx,edx
  44453e:	33 c0                	xor    eax,eax
  444540:	85 ed                	test   ebp,ebp
  444542:	7e 1b                	jle    0x44455f
  444544:	8d 7c 24 30          	lea    edi,[esp+0x30]
  444548:	8b 0f                	mov    ecx,DWORD PTR [edi]
  44454a:	3b ca                	cmp    ecx,edx
  44454c:	7e 04                	jle    0x444552
  44454e:	8b d1                	mov    edx,ecx
  444550:	8b f0                	mov    esi,eax
  444552:	40                   	inc    eax
  444553:	83 c7 04             	add    edi,0x4
  444556:	3b c5                	cmp    eax,ebp
  444558:	7c ee                	jl     0x444548
  44455a:	83 fe ff             	cmp    esi,0xffffffff
  44455d:	75 0e                	jne    0x44456d
  44455f:	8b 44 24 2c          	mov    eax,DWORD PTR [esp+0x2c]
  444563:	5f                   	pop    edi
  444564:	5e                   	pop    esi
  444565:	5d                   	pop    ebp
  444566:	5b                   	pop    ebx
  444567:	83 c4 70             	add    esp,0x70
  44456a:	c2 08 00             	ret    0x8
  44456d:	8d 0c db             	lea    ecx,[ebx+ebx*8]
  444570:	03 f3                	add    esi,ebx
  444572:	5f                   	pop    edi
  444573:	8d 04 8b             	lea    eax,[ebx+ecx*4]
  444576:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  44457a:	8d 14 40             	lea    edx,[eax+eax*2]
  44457d:	8d 04 d6             	lea    eax,[esi+edx*8]
  444580:	5e                   	pop    esi
  444581:	8b 84 81 dc 0c 00 00 	mov    eax,DWORD PTR [ecx+eax*4+0xcdc]
  444588:	5d                   	pop    ebp
  444589:	5b                   	pop    ebx
  44458a:	83 c4 70             	add    esp,0x70
  44458d:	c2 08 00             	ret    0x8
  444590:	83 ec 08             	sub    esp,0x8
  444593:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  444597:	53                   	push   ebx
  444598:	8b d9                	mov    ebx,ecx
  44459a:	55                   	push   ebp
  44459b:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  4445a2:	56                   	push   esi
  4445a3:	2b d0                	sub    edx,eax
  4445a5:	57                   	push   edi
  4445a6:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  4445aa:	c7 44 24 10 ff ff ff 	mov    DWORD PTR [esp+0x10],0xffffffff
  4445b1:	ff 
  4445b2:	8d 0c 92             	lea    ecx,[edx+edx*4]
  4445b5:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4445bb:	c1 e1 04             	shl    ecx,0x4
  4445be:	8b 92 b4 00 00 00    	mov    edx,DWORD PTR [edx+0xb4]
  4445c4:	8d 3c 0a             	lea    edi,[edx+ecx*1]
  4445c7:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  4445cb:	8d 0c d2             	lea    ecx,[edx+edx*8]
  4445ce:	8d 34 8a             	lea    esi,[edx+ecx*4]
  4445d1:	8d 0c 76             	lea    ecx,[esi+esi*2]
  4445d4:	8b 77 38             	mov    esi,DWORD PTR [edi+0x38]
  4445d7:	8d 14 ca             	lea    edx,[edx+ecx*8]
  4445da:	8b 8c 93 68 0c 00 00 	mov    ecx,DWORD PTR [ebx+edx*4+0xc68]
  4445e1:	3b ce                	cmp    ecx,esi
  4445e3:	8d 14 93             	lea    edx,[ebx+edx*4]
  4445e6:	0f 8c 35 02 00 00    	jl     0x444821
  4445ec:	8b 92 60 0c 00 00    	mov    edx,DWORD PTR [edx+0xc60]
  4445f2:	8b 4f 34             	mov    ecx,DWORD PTR [edi+0x34]
  4445f5:	3b d1                	cmp    edx,ecx
  4445f7:	0f 8c 24 02 00 00    	jl     0x444821
  4445fd:	8b 8b 38 e5 00 00    	mov    ecx,DWORD PTR [ebx+0xe538]
  444603:	c1 e0 06             	shl    eax,0x6
  444606:	03 c8                	add    ecx,eax
  444608:	8b 41 2c             	mov    eax,DWORD PTR [ecx+0x2c]
  44460b:	85 c0                	test   eax,eax
  44460d:	74 09                	je     0x444618
  44460f:	83 f8 01             	cmp    eax,0x1
  444612:	0f 85 f6 00 00 00    	jne    0x44470e
  444618:	8b 51 30             	mov    edx,DWORD PTR [ecx+0x30]
  44461b:	8b f8                	mov    edi,eax
  44461d:	f7 df                	neg    edi
  44461f:	1b ff                	sbb    edi,edi
  444621:	83 e7 f8             	and    edi,0xfffffff8
  444624:	83 c7 10             	add    edi,0x10
  444627:	85 d2                	test   edx,edx
  444629:	0f 8e df 00 00 00    	jle    0x44470e
  44462f:	f7 d8                	neg    eax
  444631:	1b c0                	sbb    eax,eax
  444633:	24 f8                	and    al,0xf8
  444635:	83 c0 08             	add    eax,0x8
  444638:	8b f0                	mov    esi,eax
  44463a:	3b f7                	cmp    esi,edi
  44463c:	0f 8d cc 00 00 00    	jge    0x44470e
  444642:	8d 04 f6             	lea    eax,[esi+esi*8]
  444645:	03 d6                	add    edx,esi
  444647:	8d 04 86             	lea    eax,[esi+eax*4]
  44464a:	8d 04 40             	lea    eax,[eax+eax*2]
  44464d:	8d 04 c6             	lea    eax,[esi+eax*8]
  444650:	8d ac 83 60 0c 00 00 	lea    ebp,[ebx+eax*4+0xc60]
  444657:	8d 04 f6             	lea    eax,[esi+esi*8]
  44465a:	8d 04 86             	lea    eax,[esi+eax*4]
  44465d:	8d 04 40             	lea    eax,[eax+eax*2]
  444660:	8d 14 c2             	lea    edx,[edx+eax*8]
  444663:	8d 84 93 18 0e 00 00 	lea    eax,[ebx+edx*4+0xe18]
  44466a:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  44466e:	83 7c 24 10 ff       	cmp    DWORD PTR [esp+0x10],0xffffffff
  444673:	75 78                	jne    0x4446ed
  444675:	8b 41 30             	mov    eax,DWORD PTR [ecx+0x30]
  444678:	83 f8 1c             	cmp    eax,0x1c
  44467b:	75 26                	jne    0x4446a3
  44467d:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  444680:	8d 14 40             	lea    edx,[eax+eax*2]
  444683:	b8 67 66 66 66       	mov    eax,0x66666667
  444688:	d1 e2                	shl    edx,1
  44468a:	f7 ea                	imul   edx
  44468c:	c1 fa 02             	sar    edx,0x2
  44468f:	8b c2                	mov    eax,edx
  444691:	c1 e8 1f             	shr    eax,0x1f
  444694:	03 d0                	add    edx,eax
  444696:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  444699:	3b c2                	cmp    eax,edx
  44469b:	7d 37                	jge    0x4446d4
  44469d:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4446a1:	eb 31                	jmp    0x4446d4
  4446a3:	83 f8 1d             	cmp    eax,0x1d
  4446a6:	75 57                	jne    0x4446ff
  4446a8:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  4446ac:	33 db                	xor    ebx,ebx
  4446ae:	8d 82 60 0c 00 00    	lea    eax,[edx+0xc60]
  4446b4:	ba 08 00 00 00       	mov    edx,0x8
  4446b9:	83 38 00             	cmp    DWORD PTR [eax],0x0
  4446bc:	7e 01                	jle    0x4446bf
  4446be:	43                   	inc    ebx
  4446bf:	05 e4 0d 00 00       	add    eax,0xde4
  4446c4:	4a                   	dec    edx
  4446c5:	75 f2                	jne    0x4446b9
  4446c7:	83 fb 06             	cmp    ebx,0x6
  4446ca:	7d 04                	jge    0x4446d0
  4446cc:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4446d0:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  4446d4:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  4446d8:	46                   	inc    esi
  4446d9:	81 c5 e4 0d 00 00    	add    ebp,0xde4
  4446df:	81 c2 e4 0d 00 00    	add    edx,0xde4
  4446e5:	3b f7                	cmp    esi,edi
  4446e7:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
  4446eb:	7c 81                	jl     0x44466e
  4446ed:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  4446f1:	85 c0                	test   eax,eax
  4446f3:	7c 19                	jl     0x44470e
  4446f5:	5f                   	pop    edi
  4446f6:	5e                   	pop    esi
  4446f7:	5d                   	pop    ebp
  4446f8:	5b                   	pop    ebx
  4446f9:	83 c4 08             	add    esp,0x8
  4446fc:	c2 08 00             	ret    0x8
  4446ff:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  444703:	83 38 00             	cmp    DWORD PTR [eax],0x0
  444706:	7e cc                	jle    0x4446d4
  444708:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  44470c:	eb c6                	jmp    0x4446d4
  44470e:	8b 71 34             	mov    esi,DWORD PTR [ecx+0x34]
  444711:	85 f6                	test   esi,esi
  444713:	74 09                	je     0x44471e
  444715:	83 fe 01             	cmp    esi,0x1
  444718:	0f 85 d9 00 00 00    	jne    0x4447f7
  44471e:	8b 69 38             	mov    ebp,DWORD PTR [ecx+0x38]
  444721:	8b fe                	mov    edi,esi
  444723:	f7 df                	neg    edi
  444725:	1b ff                	sbb    edi,edi
  444727:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  44472b:	83 e7 f8             	and    edi,0xfffffff8
  44472e:	83 c7 10             	add    edi,0x10
  444731:	85 ed                	test   ebp,ebp
  444733:	0f 8e be 00 00 00    	jle    0x4447f7
  444739:	f7 de                	neg    esi
  44473b:	1b f6                	sbb    esi,esi
  44473d:	83 e6 f8             	and    esi,0xfffffff8
  444740:	83 c6 08             	add    esi,0x8
  444743:	3b f7                	cmp    esi,edi
  444745:	0f 8d ac 00 00 00    	jge    0x4447f7
  44474b:	8d 0c f6             	lea    ecx,[esi+esi*8]
  44474e:	8d 04 8e             	lea    eax,[esi+ecx*4]
  444751:	8d 0c f6             	lea    ecx,[esi+esi*8]
  444754:	8d 14 40             	lea    edx,[eax+eax*2]
  444757:	8d 04 d6             	lea    eax,[esi+edx*8]
  44475a:	8d 9c 83 60 0c 00 00 	lea    ebx,[ebx+eax*4+0xc60]
  444761:	8d 04 8e             	lea    eax,[esi+ecx*4]
  444764:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  444768:	8d 14 40             	lea    edx,[eax+eax*2]
  44476b:	8d 04 2e             	lea    eax,[esi+ebp*1]
  44476e:	8d 14 d0             	lea    edx,[eax+edx*8]
  444771:	8d 84 91 18 0e 00 00 	lea    eax,[ecx+edx*4+0xe18]
  444778:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  44477c:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  444780:	83 c8 ff             	or     eax,0xffffffff
  444783:	3b d0                	cmp    edx,eax
  444785:	75 70                	jne    0x4447f7
  444787:	83 fd 1c             	cmp    ebp,0x1c
  44478a:	75 25                	jne    0x4447b1
  44478c:	8b 43 fc             	mov    eax,DWORD PTR [ebx-0x4]
  44478f:	8d 14 40             	lea    edx,[eax+eax*2]
  444792:	b8 67 66 66 66       	mov    eax,0x66666667
  444797:	d1 e2                	shl    edx,1
  444799:	f7 ea                	imul   edx
  44479b:	c1 fa 02             	sar    edx,0x2
  44479e:	8b c2                	mov    eax,edx
  4447a0:	c1 e8 1f             	shr    eax,0x1f
  4447a3:	03 d0                	add    edx,eax
  4447a5:	8b 03                	mov    eax,DWORD PTR [ebx]
  4447a7:	3b c2                	cmp    eax,edx
  4447a9:	7d 33                	jge    0x4447de
  4447ab:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4447af:	eb 2d                	jmp    0x4447de
  4447b1:	83 fd 1d             	cmp    ebp,0x1d
  4447b4:	75 4f                	jne    0x444805
  4447b6:	33 ed                	xor    ebp,ebp
  4447b8:	8d 81 60 0c 00 00    	lea    eax,[ecx+0xc60]
  4447be:	ba 08 00 00 00       	mov    edx,0x8
  4447c3:	83               	cmp    DWORD PTR [eax],0x0
