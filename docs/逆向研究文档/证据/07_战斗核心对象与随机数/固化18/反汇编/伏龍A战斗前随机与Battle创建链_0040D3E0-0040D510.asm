
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040d3e0 <.text+0xc3e0>:
  40d3e0:	4c                   	dec    esp
  40d3e1:	24 68                	and    al,0x68
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
