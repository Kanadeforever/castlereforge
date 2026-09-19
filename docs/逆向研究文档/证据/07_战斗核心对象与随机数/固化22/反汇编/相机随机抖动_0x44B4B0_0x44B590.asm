
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0044b4b0 <.text+0x4a4b0>:
  44b4b0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  44b4b4:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  44b4b8:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  44b4bc:	a3 38 85 97 00       	mov    ds:0x978538,eax
  44b4c1:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  44b4c5:	89 0d 3c 85 97 00    	mov    DWORD PTR ds:0x97853c,ecx
  44b4cb:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  44b4cf:	c6 05 34 85 97 00 01 	mov    BYTE PTR ds:0x978534,0x1
  44b4d6:	89 15 40 85 97 00    	mov    DWORD PTR ds:0x978540,edx
  44b4dc:	a3 44 85 97 00       	mov    ds:0x978544,eax
  44b4e1:	c7 05 48 85 97 00 00 	mov    DWORD PTR ds:0x978548,0x0
  44b4e8:	00 00 00 
  44b4eb:	89 0d 4c 85 97 00    	mov    DWORD PTR ds:0x97854c,ecx
  44b4f1:	c3                   	ret
  44b4f2:	90                   	nop
  44b4f3:	90                   	nop
  44b4f4:	90                   	nop
  44b4f5:	90                   	nop
  44b4f6:	90                   	nop
  44b4f7:	90                   	nop
  44b4f8:	90                   	nop
  44b4f9:	90                   	nop
  44b4fa:	90                   	nop
  44b4fb:	90                   	nop
  44b4fc:	90                   	nop
  44b4fd:	90                   	nop
  44b4fe:	90                   	nop
  44b4ff:	90                   	nop
  44b500:	a0 34 85 97 00       	mov    al,ds:0x978534
  44b505:	84 c0                	test   al,al
  44b507:	0f 84 81 00 00 00    	je     0x44b58e
  44b50d:	a1 4c 85 97 00       	mov    eax,ds:0x97854c
  44b512:	85 c0                	test   eax,eax
  44b514:	74 15                	je     0x44b52b
  44b516:	a1 48 85 97 00       	mov    eax,ds:0x978548
  44b51b:	8b 0d 44 85 97 00    	mov    ecx,DWORD PTR ds:0x978544
  44b521:	40                   	inc    eax
  44b522:	3b c1                	cmp    eax,ecx
  44b524:	a3 48 85 97 00       	mov    ds:0x978548,eax
  44b529:	7c 63                	jl     0x44b58e
  44b52b:	56                   	push   esi
  44b52c:	e8 34 60 00 00       	call   0x451565
  44b531:	8b 0d 40 85 97 00    	mov    ecx,DWORD PTR ds:0x978540
  44b537:	99                   	cdq
  44b538:	8d 34 09             	lea    esi,[ecx+ecx*1]
  44b53b:	f7 fe                	idiv   esi
  44b53d:	2b d1                	sub    edx,ecx
  44b53f:	8b 0d 38 85 97 00    	mov    ecx,DWORD PTR ds:0x978538
  44b545:	03 d1                	add    edx,ecx
  44b547:	89 15 0c 85 97 00    	mov    DWORD PTR ds:0x97850c,edx
  44b54d:	e8 13 60 00 00       	call   0x451565
  44b552:	8b 0d 40 85 97 00    	mov    ecx,DWORD PTR ds:0x978540
  44b558:	99                   	cdq
  44b559:	8d 34 09             	lea    esi,[ecx+ecx*1]
  44b55c:	f7 fe                	idiv   esi
  44b55e:	a1 0c 85 97 00       	mov    eax,ds:0x97850c
  44b563:	8b 35 3c 85 97 00    	mov    esi,DWORD PTR ds:0x97853c
  44b569:	2b d1                	sub    edx,ecx
  44b56b:	8b 0d 14 85 97 00    	mov    ecx,DWORD PTR ds:0x978514
  44b571:	03 c8                	add    ecx,eax
  44b573:	a1 18 85 97 00       	mov    eax,ds:0x978518
  44b578:	03 d6                	add    edx,esi
  44b57a:	89 0d 14 85 97 00    	mov    DWORD PTR ds:0x978514,ecx
  44b580:	03 c2                	add    eax,edx
  44b582:	89 15 10 85 97 00    	mov    DWORD PTR ds:0x978510,edx
  44b588:	a3 18 85 97 00       	mov    ds:0x978518,eax
  44b58d:	5e                   	pop    esi
  44b58e:	c3                   	ret
  44b58f:	90                   	nop
