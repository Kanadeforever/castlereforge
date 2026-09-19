
/mnt/data/solid61_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

0042c260 <.text+0x2b260>:
  42c260:	83 ec 10             	sub    esp,0x10
  42c263:	53                   	push   ebx
  42c264:	b8 64 00 00 00       	mov    eax,0x64
  42c269:	55                   	push   ebp
  42c26a:	56                   	push   esi
  42c26b:	8b 74 24 24          	mov    esi,DWORD PTR [esp+0x24]
  42c26f:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42c273:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  42c277:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  42c27b:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  42c27f:	57                   	push   edi
  42c280:	8b f9                	mov    edi,ecx
  42c282:	8b 6c 24 24          	mov    ebp,DWORD PTR [esp+0x24]
  42c286:	8b 48 3c             	mov    ecx,DWORD PTR [eax+0x3c]
  42c289:	55                   	push   ebp
  42c28a:	89 4c 24 20          	mov    DWORD PTR [esp+0x20],ecx
  42c28e:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  42c291:	0f af 48 4c          	imul   ecx,DWORD PTR [eax+0x4c]
  42c295:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42c29a:	f7 e9                	imul   ecx
  42c29c:	c1 fa 05             	sar    edx,0x5
  42c29f:	8b c2                	mov    eax,edx
  42c2a1:	8b cf                	mov    ecx,edi
  42c2a3:	c1 e8 1f             	shr    eax,0x1f
  42c2a6:	03 d0                	add    edx,eax
  42c2a8:	8b da                	mov    ebx,edx
  42c2aa:	e8 71 fa ff ff       	call   0x42bd20
  42c2af:	55                   	push   ebp
  42c2b0:	8b cf                	mov    ecx,edi
  42c2b2:	03 d8                	add    ebx,eax
  42c2b4:	e8 37 0a 00 00       	call   0x42ccf0
  42c2b9:	8b d0                	mov    edx,eax
  42c2bb:	55                   	push   ebp
  42c2bc:	8b cf                	mov    ecx,edi
  42c2be:	89 54 24 2c          	mov    DWORD PTR [esp+0x2c],edx
  42c2c2:	e8 29 0a 00 00       	call   0x42ccf0
  42c2c7:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  42c2cb:	8b 54 24 2c          	mov    edx,DWORD PTR [esp+0x2c]
  42c2cf:	0f af c8             	imul   ecx,eax
  42c2d2:	8b 6a 50             	mov    ebp,DWORD PTR [edx+0x50]
  42c2d5:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  42c2d8:	8b c1                	mov    eax,ecx
  42c2da:	03 ea                	add    ebp,edx
  42c2dc:	99                   	cdq
  42c2dd:	d1 e5                	shl    ebp,1
  42c2df:	f7 fd                	idiv   ebp
  42c2e1:	56                   	push   esi
  42c2e2:	8b cf                	mov    ecx,edi
  42c2e4:	03 d8                	add    ebx,eax
  42c2e6:	e8 05 0a 00 00       	call   0x42ccf0
  42c2eb:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  42c2ee:	03 c1                	add    eax,ecx
  42c2f0:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  42c2f4:	99                   	cdq
  42c2f5:	2b c2                	sub    eax,edx
  42c2f7:	d1 f8                	sar    eax,1
  42c2f9:	f7 d8                	neg    eax
  42c2fb:	03 d8                	add    ebx,eax
  42c2fd:	8b 44 8e 64          	mov    eax,DWORD PTR [esi+ecx*4+0x64]
  42c301:	99                   	cdq
  42c302:	8b e8                	mov    ebp,eax
  42c304:	8b 86 90 00 00 00    	mov    eax,DWORD PTR [esi+0x90]
  42c30a:	33 ea                	xor    ebp,edx
  42c30c:	2b ea                	sub    ebp,edx
  42c30e:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42c314:	85 c0                	test   eax,eax
  42c316:	7e 1c                	jle    0x42c334
  42c318:	8d 3c c5 00 00 00 00 	lea    edi,[eax*8+0x0]
  42c31f:	2b f8                	sub    edi,eax
  42c321:	8d 3c b8             	lea    edi,[eax+edi*4]
  42c324:	c1 e7 04             	shl    edi,0x4
  42c327:	2b f8                	sub    edi,eax
  42c329:	8d 04 7a             	lea    eax,[edx+edi*2]
  42c32c:	8b 44 88 74          	mov    eax,DWORD PTR [eax+ecx*4+0x74]
  42c330:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  42c334:	8b 86 94 00 00 00    	mov    eax,DWORD PTR [esi+0x94]
  42c33a:	85 c0                	test   eax,eax
  42c33c:	7e 1c                	jle    0x42c35a
  42c33e:	8d 3c c5 00 00 00 00 	lea    edi,[eax*8+0x0]
  42c345:	2b f8                	sub    edi,eax
  42c347:	8d 3c b8             	lea    edi,[eax+edi*4]
  42c34a:	c1 e7 04             	shl    edi,0x4
  42c34d:	2b f8                	sub    edi,eax
  42c34f:	8d 04 7a             	lea    eax,[edx+edi*2]
  42c352:	8b 44 88 74          	mov    eax,DWORD PTR [eax+ecx*4+0x74]
  42c356:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42c35a:	8b 86 98 00 00 00    	mov    eax,DWORD PTR [esi+0x98]
  42c360:	85 c0                	test   eax,eax
  42c362:	7e 1c                	jle    0x42c380
  42c364:	8d 3c c5 00 00 00 00 	lea    edi,[eax*8+0x0]
  42c36b:	2b f8                	sub    edi,eax
  42c36d:	8d 3c b8             	lea    edi,[eax+edi*4]
  42c370:	c1 e7 04             	shl    edi,0x4
  42c373:	2b f8                	sub    edi,eax
  42c375:	8d 14 7a             	lea    edx,[edx+edi*2]
  42c378:	8b 44 8a 74          	mov    eax,DWORD PTR [edx+ecx*4+0x74]
  42c37c:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  42c380:	e8 e0 51 02 00       	call   0x451565
  42c385:	8b c8                	mov    ecx,eax
  42c387:	b8 03 b5 7e a5       	mov    eax,0xa57eb503
  42c38c:	f7 e9                	imul   ecx
  42c38e:	03 d1                	add    edx,ecx
  42c390:	c1 fa 06             	sar    edx,0x6
  42c393:	8b ca                	mov    ecx,edx
  42c395:	c1 e9 1f             	shr    ecx,0x1f
  42c398:	8d 4c 0a 01          	lea    ecx,[edx+ecx*1+0x1]
  42c39c:	8b 54 24 2c          	mov    edx,DWORD PTR [esp+0x2c]
  42c3a0:	8b 42 4c             	mov    eax,DWORD PTR [edx+0x4c]
  42c3a3:	99                   	cdq
  42c3a4:	f7 f9                	idiv   ecx
  42c3a6:	8b f8                	mov    edi,eax
  42c3a8:	03 fb                	add    edi,ebx
  42c3aa:	e8 b6 51 02 00       	call   0x451565
  42c3af:	8b c8                	mov    ecx,eax
  42c3b1:	b8 f3 1a ca 6b       	mov    eax,0x6bca1af3
  42c3b6:	f7 e9                	imul   ecx
  42c3b8:	c1 fa 03             	sar    edx,0x3
  42c3bb:	8b c2                	mov    eax,edx
  42c3bd:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  42c3c1:	c1 e8 1f             	shr    eax,0x1f
  42c3c4:	8d 4c 02 01          	lea    ecx,[edx+eax*1+0x1]
  42c3c8:	8b 46 4c             	mov    eax,DWORD PTR [esi+0x4c]
  42c3cb:	99                   	cdq
  42c3cc:	f7 f9                	idiv   ecx
  42c3ce:	2b f8                	sub    edi,eax
  42c3d0:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42c3d4:	99                   	cdq
  42c3d5:	8b c8                	mov    ecx,eax
  42c3d7:	8b c3                	mov    eax,ebx
  42c3d9:	33 ca                	xor    ecx,edx
  42c3db:	89 7c 24 2c          	mov    DWORD PTR [esp+0x2c],edi
  42c3df:	2b ca                	sub    ecx,edx
  42c3e1:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  42c3e5:	99                   	cdq
  42c3e6:	db 44 24 2c          	fild   DWORD PTR [esp+0x2c]
  42c3ea:	33 c2                	xor    eax,edx
  42c3ec:	2b c2                	sub    eax,edx
  42c3ee:	0f af c8             	imul   ecx,eax
  42c3f1:	8b c7                	mov    eax,edi
  42c3f3:	99                   	cdq
  42c3f4:	33 c2                	xor    eax,edx
  42c3f6:	2b c2                	sub    eax,edx
  42c3f8:	0f af c8             	imul   ecx,eax
  42c3fb:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42c400:	f7 e9                	imul   ecx
  42c402:	c1 fa 05             	sar    edx,0x5
  42c405:	8b c2                	mov    eax,edx
  42c407:	c1 e8 1f             	shr    eax,0x1f
  42c40a:	03 d0                	add    edx,eax
  42c40c:	0f af d5             	imul   edx,ebp
  42c40f:	89 54 24 2c          	mov    DWORD PTR [esp+0x2c],edx
  42c413:	db 44 24 2c          	fild   DWORD PTR [esp+0x2c]
  42c417:	de c9                	fmulp  st(1),st
  42c419:	dc 0d 98 0c 46 00    	fmul   QWORD PTR ds:0x460c98
  42c41f:	e8 74 59 02 00       	call   0x451d98
  42c424:	85 c0                	test   eax,eax
  42c426:	7d 02                	jge    0x42c42a
  42c428:	33 c0                	xor    eax,eax
  42c42a:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  42c42e:	85 c9                	test   ecx,ecx
  42c430:	7e 1a                	jle    0x42c44c
  42c432:	8b 54 8e 64          	mov    edx,DWORD PTR [esi+ecx*4+0x64]
  42c436:	85 d2                	test   edx,edx
  42c438:	7c 10                	jl     0x42c44a
  42c43a:	85 ff                	test   edi,edi
  42c43c:	7c 0c                	jl     0x42c44a
  42c43e:	85 db                	test   ebx,ebx
  42c440:	7c 08                	jl     0x42c44a
  42c442:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  42c446:	85 c9                	test   ecx,ecx
  42c448:	7d 02                	jge    0x42c44c
  42c44a:	f7 d8                	neg    eax
  42c44c:	5f                   	pop    edi
  42c44d:	5e                   	pop    esi
  42c44e:	5d                   	pop    ebp
  42c44f:	5b                   	pop    ebx
  42c450:	83 c4 10             	add    esp,0x10
  42c453:	c2               	ret    0xc
