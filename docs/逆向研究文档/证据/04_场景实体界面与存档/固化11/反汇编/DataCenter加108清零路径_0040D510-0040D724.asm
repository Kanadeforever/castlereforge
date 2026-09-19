; Oracle: /mnt/data/RPG.exe
; SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf  /mnt/data/RPG.exe
; objdump -D -Mintel --start-address=0x40d510 --stop-address=0x40d724 RPG.exe

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040d510 <.text+0xc510>:
  40d510:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40d516:	6a ff                	push   0xffffffff
  40d518:	68 b6 d1 45 00       	push   0x45d1b6
  40d51d:	50                   	push   eax
  40d51e:	a1 18 f8 89 00       	mov    eax,ds:0x89f818
  40d523:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40d52a:	83 ec 68             	sub    esp,0x68
  40d52d:	53                   	push   ebx
  40d52e:	56                   	push   esi
  40d52f:	57                   	push   edi
  40d530:	33 ff                	xor    edi,edi
  40d532:	2b c7                	sub    eax,edi
  40d534:	8b f1                	mov    esi,ecx
  40d536:	0f 84 80 01 00 00    	je     0x40d6bc
  40d53c:	48                   	dec    eax
  40d53d:	bb 01 00 00 00       	mov    ebx,0x1
  40d542:	74 68                	je     0x40d5ac
  40d544:	48                   	dec    eax
  40d545:	0f 85 48 01 00 00    	jne    0x40d693
  40d54b:	68 e6 03 00 00       	push   0x3e6
  40d550:	e8 7b db 03 00       	call   0x44b0d0
  40d555:	83 c4 04             	add    esp,0x4
  40d558:	85 c0                	test   eax,eax
  40d55a:	7e 41                	jle    0x40d59d
  40d55c:	e8 df dd ff ff       	call   0x40b340
  40d561:	e8 3a de ff ff       	call   0x40b3a0
  40d566:	8b 88 80 02 00 00    	mov    ecx,DWORD PTR [eax+0x280]
  40d56c:	e8 df d8 ff ff       	call   0x40ae50
  40d571:	8b c8                	mov    ecx,eax
  40d573:	e8 88 be ff ff       	call   0x409400
  40d578:	68 e6 03 00 00       	push   0x3e6
  40d57d:	e8 4e db 03 00       	call   0x44b0d0
  40d582:	50                   	push   eax
  40d583:	e8 a8 dc ff ff       	call   0x40b230
  40d588:	a1 10 f8 89 00       	mov    eax,ds:0x89f810
  40d58d:	83 c4 08             	add    esp,0x8
  40d590:	83 c0 02             	add    eax,0x2
  40d593:	a3 10 f8 89 00       	mov    ds:0x89f810,eax
  40d598:	e9 f6 00 00 00       	jmp    0x40d693
  40d59d:	e8 2e 99 ff ff       	call   0x406ed0
  40d5a2:	e8 99 dd ff ff       	call   0x40b340
  40d5a7:	e9 e7 00 00 00       	jmp    0x40d693
  40d5ac:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  40d5b1:	39 98 30 01 00 00    	cmp    DWORD PTR [eax+0x130],ebx
  40d5b7:	75 7e                	jne    0x40d637
  40d5b9:	89 b8 30 01 00 00    	mov    DWORD PTR [eax+0x130],edi
  40d5bf:	e8 0c 99 ff ff       	call   0x406ed0
  40d5c4:	e8 77 dd ff ff       	call   0x40b340
  40d5c9:	8d 44 24 10          	lea    eax,[esp+0x10]
  40d5cd:	68 84 84 46 00       	push   0x468484
  40d5d2:	50                   	push   eax
  40d5d3:	ff 15 94 01 46 00    	call   DWORD PTR ds:0x460194
  40d5d9:	83 c4 08             	add    esp,0x8
  40d5dc:	e8 bf dd ff ff       	call   0x40b3a0
  40d5e1:	8b c8                	mov    ecx,eax
  40d5e3:	e8 38 db ff ff       	call   0x40b120
  40d5e8:	68 84 02 00 00       	push   0x284
  40d5ed:	e8 6d 40 04 00       	call   0x45165f
  40d5f2:	83 c4 04             	add    esp,0x4
  40d5f5:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  40d5f9:	3b c7                	cmp    eax,edi
  40d5fb:	89 5c 24 7c          	mov    DWORD PTR [esp+0x7c],ebx
  40d5ff:	74 09                	je     0x40d60a
  40d601:	8b c8                	mov    ecx,eax
  40d603:	e8 78 d8 ff ff       	call   0x40ae80
  40d608:	eb 02                	jmp    0x40d60c
  40d60a:	33 c0                	xor    eax,eax
  40d60c:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  40d610:	c7 44 24 7c ff ff ff 	mov    DWORD PTR [esp+0x7c],0xffffffff
  40d617:	ff 
  40d618:	51                   	push   ecx
  40d619:	8b c8                	mov    ecx,eax
  40d61b:	e8 20 d9 ff ff       	call   0x40af40
  40d620:	6a 0a                	push   0xa
  40d622:	e8 09 dc ff ff       	call   0x40b230
  40d627:	a1 10 f8 89 00       	mov    eax,ds:0x89f810
  40d62c:	83 c4 04             	add    esp,0x4
  40d62f:	40                   	inc    eax
  40d630:	a3 10 f8 89 00       	mov    ds:0x89f810,eax
  40d635:	eb 5c                	jmp    0x40d693
  40d637:	39 98 0c 01 00 00    	cmp    DWORD PTR [eax+0x10c],ebx
  40d63d:	75 35                	jne    0x40d674
  40d63f:	e8 ac db 03 00       	call   0x44b1f0
  40d644:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  40d64a:	8b c6                	mov    eax,esi
  40d64c:	89 ba 0c 01 00 00    	mov    DWORD PTR [edx+0x10c],edi
  40d652:	5f                   	pop    edi
  40d653:	88 1d 1d f8 89 00    	mov    BYTE PTR ds:0x89f81d,bl
  40d659:	5e                   	pop    esi
  40d65a:	c7 05 18 f8 89 00 02 	mov    DWORD PTR ds:0x89f818,0x2
  40d661:	00 00 00 
  40d664:	5b                   	pop    ebx
  40d665:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40d669:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40d670:	83 c4 74             	add    esp,0x74
  40d673:	c3                   	ret
  40d674:	a1 10 f8 89 00       	mov    eax,ds:0x89f810
  40d679:	57                   	push   edi
  40d67a:	40                   	inc    eax
  40d67b:	68 e6 03 00 00       	push   0x3e6
  40d680:	a3 10 f8 89 00       	mov    ds:0x89f810,eax
  40d685:	89 3d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,edi
  40d68b:	e8 20 da 03 00       	call   0x44b0b0
  40d690:	83 c4 08             	add    esp,0x8
  40d693:	56                   	push   esi
  40d694:	89 3d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,edi
  40d69a:	88 1d 1d f8 89 00    	mov    BYTE PTR ds:0x89f81d,bl
  40d6a0:	e8 ab 3e 04 00       	call   0x451550
  40d6a5:	83 c4 04             	add    esp,0x4
  40d6a8:	33 c0                	xor    eax,eax
  40d6aa:	5f                   	pop    edi
  40d6ab:	5e                   	pop    esi
  40d6ac:	5b                   	pop    ebx
  40d6ad:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40d6b1:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40d6b8:	83 c4 74             	add    esp,0x74
  40d6bb:	c3                   	ret
  40d6bc:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40d6c2:	bb 01 00 00 00       	mov    ebx,0x1
  40d6c7:	53                   	push   ebx
  40d6c8:	6a 1f                	push   0x1f
  40d6ca:	e8 f1 5b ff ff       	call   0x4032c0
  40d6cf:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40d6d2:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  40d6d5:	a1 08 f8 89 00       	mov    eax,ds:0x89f808
  40d6da:	50                   	push   eax
  40d6db:	68 e6 03 00 00       	push   0x3e6
  40d6e0:	e8 cb d9 03 00       	call   0x44b0b0
  40d6e5:	83 c4 08             	add    esp,0x8
  40d6e8:	e8 63 da 03 00       	call   0x44b150
  40d6ed:	e8 ae dc ff ff       	call   0x40b3a0
  40d6f2:	8b c8                	mov    ecx,eax
  40d6f4:	e8 a7 d9 ff ff       	call   0x40b0a0
  40d6f9:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40d6fc:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  40d701:	68 94 05 00 00       	push   0x594
  40d706:	8b 51 04             	mov    edx,DWORD PTR [ecx+0x4]
  40d709:	89 90 d0 00 00 00    	mov    DWORD PTR [eax+0xd0],edx
  40d70f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40d715:	89 b9 08 01 00 00    	mov    DWORD PTR [ecx+0x108],edi
  40d71b:	e8 3f 3f 04 00       	call   0x45165f
  40d720:	83 c4 04             	add    esp,0x4
  40d723:	89             	mov    DWORD PTR [esp+0xc],eax
