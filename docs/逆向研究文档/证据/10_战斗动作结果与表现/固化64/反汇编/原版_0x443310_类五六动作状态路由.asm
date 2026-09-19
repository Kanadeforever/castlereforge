
/mnt/data/solid64_inputs/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443310 <.text+0x42310>:
  443310:	51                   	push   ecx
  443311:	53                   	push   ebx
  443312:	55                   	push   ebp
  443313:	56                   	push   esi
  443314:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
  443318:	8b e9                	mov    ebp,ecx
  44331a:	57                   	push   edi
  44331b:	8d 04 f6             	lea    eax,[esi+esi*8]
  44331e:	8d 04 86             	lea    eax,[esi+eax*4]
  443321:	8d 0c 40             	lea    ecx,[eax+eax*2]
  443324:	8d 14 ce             	lea    edx,[esi+ecx*8]
  443327:	8d 7c 95 00          	lea    edi,[ebp+edx*4+0x0]
  44332b:	8d 9f e4 03 00 00    	lea    ebx,[edi+0x3e4]
  443331:	8b cb                	mov    ecx,ebx
  443333:	e8 f8 f1 fd ff       	call   0x422530
  443338:	84 c0                	test   al,al
  44333a:	0f 84 19 01 00 00    	je     0x443459
  443340:	8d 8d d4 e2 00 00    	lea    ecx,[ebp+0xe2d4]
  443346:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  44334a:	e8 61 0e fe ff       	call   0x4241b0
  44334f:	8b 87 2c 0c 00 00    	mov    eax,DWORD PTR [edi+0xc2c]
  443355:	8b 8f 84 0b 00 00    	mov    ecx,DWORD PTR [edi+0xb84]
  44335b:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  44335f:	8b 40 18             	mov    eax,DWORD PTR [eax+0x18]
  443362:	83 f8 02             	cmp    eax,0x2
  443365:	0f 84 82 00 00 00    	je     0x4433ed
  44336b:	83 f8 01             	cmp    eax,0x1
  44336e:	74 7d                	je     0x4433ed
  443370:	83 f8 04             	cmp    eax,0x4
  443373:	74 78                	je     0x4433ed
  443375:	51                   	push   ecx
  443376:	56                   	push   esi
  443377:	6a 6a                	push   0x6a
  443379:	8b cb                	mov    ecx,ebx
  44337b:	e8 a0 ed fd ff       	call   0x422120
  443380:	84 c0                	test   al,al
  443382:	0f 84 c7 00 00 00    	je     0x44344f
  443388:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  44338c:	8b 40 18             	mov    eax,DWORD PTR [eax+0x18]
  44338f:	83 f8 05             	cmp    eax,0x5
  443392:	75 0c                	jne    0x4433a0
  443394:	c7 87 c8 10 00 00 c0 	mov    DWORD PTR [edi+0x10c8],0xbc0
  44339b:	0b 00 00 
  44339e:	eb 2c                	jmp    0x4433cc
  4433a0:	83 f8 06             	cmp    eax,0x6
  4433a3:	75 0c                	jne    0x4433b1
  4433a5:	c7 87 c8 10 00 00 c6 	mov    DWORD PTR [edi+0x10c8],0xbc6
  4433ac:	0b 00 00 
  4433af:	eb 1b                	jmp    0x4433cc
  4433b1:	83 f8 04             	cmp    eax,0x4
  4433b4:	75 0c                	jne    0x4433c2
  4433b6:	c7 87 c8 10 00 00 c7 	mov    DWORD PTR [edi+0x10c8],0xbc7
  4433bd:	0b 00 00 
  4433c0:	eb 0a                	jmp    0x4433cc
  4433c2:	c7 87 c8 10 00 00 bb 	mov    DWORD PTR [edi+0x10c8],0xbbb
  4433c9:	0b 00 00 
  4433cc:	6a 03                	push   0x3
  4433ce:	8d 8f 8c 10 00 00    	lea    ecx,[edi+0x108c]
  4433d4:	e8 a7 0b fe ff       	call   0x423f80
  4433d9:	c7 87 c4 10 00 00 d6 	mov    DWORD PTR [edi+0x10c4],0x7d6
  4433e0:	07 00 00 
  4433e3:	5f                   	pop    edi
  4433e4:	5e                   	pop    esi
  4433e5:	5d                   	pop    ebp
  4433e6:	b0 01                	mov    al,0x1
  4433e8:	5b                   	pop    ebx
  4433e9:	59                   	pop    ecx
  4433ea:	c2 04 00             	ret    0x4
  4433ed:	51                   	push   ecx
  4433ee:	56                   	push   esi
  4433ef:	6a 6a                	push   0x6a
  4433f1:	8b cb                	mov    ecx,ebx
  4433f3:	e8 28 ed fd ff       	call   0x422120
  4433f8:	84 c0                	test   al,al
  4433fa:	74 53                	je     0x44344f
  4433fc:	8a 85 2d e5 00 00    	mov    al,BYTE PTR [ebp+0xe52d]
  443402:	89 b5 24 e2 00 00    	mov    DWORD PTR [ebp+0xe224],esi
  443408:	84 c0                	test   al,al
  44340a:	75 13                	jne    0x44341f
  44340c:	8b 8d ac e2 00 00    	mov    ecx,DWORD PTR [ebp+0xe2ac]
  443412:	c6 85 2d e5 00 00 01 	mov    BYTE PTR [ebp+0xe52d],0x1
  443419:	89 8d 30 e5 00 00    	mov    DWORD PTR [ebp+0xe530],ecx
  44341f:	6a 03                	push   0x3
  443421:	8d 8f 8c 10 00 00    	lea    ecx,[edi+0x108c]
  443427:	c7 87 c8 10 00 00 bc 	mov    DWORD PTR [edi+0x10c8],0xbbc
  44342e:	0b 00 00 
  443431:	e8 4a 0b fe ff       	call   0x423f80
  443436:	c7 87 c4 10 00 00 d6 	mov    DWORD PTR [edi+0x10c4],0x7d6
  44343d:	07 00 00 
  443440:	5f                   	pop    edi
  443441:	32 c0                	xor    al,al
  443443:	5e                   	pop    esi
  443444:	5d                   	pop    ebp
  443445:	a2 dc 96 46 00       	mov    ds:0x4696dc,al
  44344a:	5b                   	pop    ebx
  44344b:	59                   	pop    ecx
  44344c:	c2 04 00             	ret    0x4
  44344f:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  443453:	56                   	push   esi
  443454:	e8 17 0d fe ff       	call   0x424170
  443459:	5f                   	pop    edi
  44345a:	5e                   	pop    esi
  44345b:	5d                   	pop    ebp
  44345c:	32 c0                	xor    al,al
  44345e:	5b                   	pop    ebx
  44345f:	59                   	pop    ecx
  443460:	c2                   	.byte 0xc2
