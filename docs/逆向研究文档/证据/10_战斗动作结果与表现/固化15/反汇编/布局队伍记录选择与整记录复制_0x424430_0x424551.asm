
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00424430 <.text+0x23430>:
  424430:	51                   	push   ecx
  424431:	53                   	push   ebx
  424432:	55                   	push   ebp
  424433:	56                   	push   esi
  424434:	8b f1                	mov    esi,ecx
  424436:	57                   	push   edi
  424437:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  42443b:	8b 3e                	mov    edi,DWORD PTR [esi]
  42443d:	85 ff                	test   edi,edi
  42443f:	0f 84 9d 00 00 00    	je     0x4244e2
  424445:	8b 56 08             	mov    edx,DWORD PTR [esi+0x8]
  424448:	85 d2                	test   edx,edx
  42444a:	0f 84 92 00 00 00    	je     0x4244e2
  424450:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  424454:	3b df                	cmp    ebx,edi
  424456:	0f 8d 86 00 00 00    	jge    0x4244e2
  42445c:	8b 0d a0 a2 46 00    	mov    ecx,DWORD PTR ds:0x46a2a0
  424462:	85 c9                	test   ecx,ecx
  424464:	7c 1c                	jl     0x424482
  424466:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  424469:	8b eb                	mov    ebp,ebx
  42446b:	c1 e5 07             	shl    ebp,0x7
  42446e:	2b eb                	sub    ebp,ebx
  424470:	39 0c a8             	cmp    DWORD PTR [eax+ebp*4],ecx
  424473:	7e 0d                	jle    0x424482
  424475:	8b eb                	mov    ebp,ebx
  424477:	c1 e5 07             	shl    ebp,0x7
  42447a:	2b eb                	sub    ebp,ebx
  42447c:	03 e9                	add    ebp,ecx
  42447e:	8b 5c a8 04          	mov    ebx,DWORD PTR [eax+ebp*4+0x4]
  424482:	3b df                	cmp    ebx,edi
  424484:	7d 5c                	jge    0x4244e2
  424486:	85 db                	test   ebx,ebx
  424488:	7c 58                	jl     0x4244e2
  42448a:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  42448d:	8b eb                	mov    ebp,ebx
  42448f:	c1 e5 07             	shl    ebp,0x7
  424492:	2b eb                	sub    ebp,ebx
  424494:	c1 e5 02             	shl    ebp,0x2
  424497:	8b 7c 28 18          	mov    edi,DWORD PTR [eax+ebp*1+0x18]
  42449b:	85 ff                	test   edi,edi
  42449d:	7c 43                	jl     0x4244e2
  42449f:	3b fa                	cmp    edi,edx
  4244a1:	7d 3f                	jge    0x4244e2
  4244a3:	85 ff                	test   edi,edi
  4244a5:	75 45                	jne    0x4244ec
  4244a7:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4244ad:	68 d0 a3 46 00       	push   0x46a3d0
  4244b2:	68 b0 a3 46 00       	push   0x46a3b0
  4244b7:	e8 94 db 00 00       	call   0x432050
  4244bc:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4244c2:	68 9b 00 00 00       	push   0x9b
  4244c7:	68 90 a3 46 00       	push   0x46a390
  4244cc:	e8 3f db 00 00       	call   0x432010
  4244d1:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4244d7:	53                   	push   ebx
  4244d8:	68 68 a3 46 00       	push   0x46a368
  4244dd:	e8 2e db 00 00       	call   0x432010
  4244e2:	5f                   	pop    edi
  4244e3:	5e                   	pop    esi
  4244e4:	5d                   	pop    ebp
  4244e5:	33 c0                	xor    eax,eax
  4244e7:	5b                   	pop    ebx
  4244e8:	59                   	pop    ecx
  4244e9:	c2 08 00             	ret    0x8
  4244ec:	e8 74 d0 02 00       	call   0x451565
  4244f1:	99                   	cdq
  4244f2:	f7 ff                	idiv   edi
  4244f4:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  4244f8:	85 ff                	test   edi,edi
  4244fa:	74 28                	je     0x424524
  4244fc:	8b cb                	mov    ecx,ebx
  4244fe:	c1 e1 07             	shl    ecx,0x7
  424501:	2b cb                	sub    ecx,ebx
  424503:	03 ca                	add    ecx,edx
  424505:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  424508:	8b 44 8a 1c          	mov    eax,DWORD PTR [edx+ecx*4+0x1c]
  42450c:	8b 56 0c             	mov    edx,DWORD PTR [esi+0xc]
  42450f:	8b c8                	mov    ecx,eax
  424511:	c1 e1 07             	shl    ecx,0x7
  424514:	2b c8                	sub    ecx,eax
  424516:	8d 34 8a             	lea    esi,[edx+ecx*4]
  424519:	b9 7f 00 00 00       	mov    ecx,0x7f
  42451e:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  424520:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  424524:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  424527:	8b 6c 28 6c          	mov    ebp,DWORD PTR [eax+ebp*1+0x6c]
  42452b:	85 ed                	test   ebp,ebp
  42452d:	7e 0a                	jle    0x424539
  42452f:	e8 31 d0 02 00       	call   0x451565
  424534:	99                   	cdq
  424535:	f7 fd                	idiv   ebp
  424537:	8b ea                	mov    ebp,edx
  424539:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  42453c:	8b cb                	mov    ecx,ebx
  42453e:	c1 e1 07             	shl    ecx,0x7
  424541:	2b cb                	sub    ecx,ebx
  424543:	5f                   	pop    edi
  424544:	03 cd                	add    ecx,ebp
  424546:	8b 44 8a 70          	mov    eax,DWORD PTR [edx+ecx*4+0x70]
  42454a:	89 5e 10             	mov    DWORD PTR [esi+0x10],ebx
  42454d:	5e                   	pop    esi
  42454e:	5d                   	pop    ebp
  42454f:	5b                   	pop    ebx
  424550:	59                   	pop    ecx
  424551:	c2                   	.byte 0xc2
