
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004014e0 <.text+0x4e0>:
  4014e0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4014e4:	53                   	push   ebx
  4014e5:	56                   	push   esi
  4014e6:	8b f1                	mov    esi,ecx
  4014e8:	57                   	push   edi
  4014e9:	89 06                	mov    DWORD PTR [esi],eax
  4014eb:	a3 84 f3 46 00       	mov    ds:0x46f384,eax
  4014f0:	e8 4b fd ff ff       	call   0x401240
  4014f5:	e8 16 15 00 00       	call   0x402a10
  4014fa:	8d 7e 04             	lea    edi,[esi+0x4]
  4014fd:	6a 02                	push   0x2
  4014ff:	57                   	push   edi
  401500:	68 90 80 46 00       	push   0x468090
  401505:	e8 a6 10 00 00       	call   0x4025b0
  40150a:	8b 0f                	mov    ecx,DWORD PTR [edi]
  40150c:	83 c4 0c             	add    esp,0xc
  40150f:	8b d9                	mov    ebx,ecx
  401511:	8d 51 11             	lea    edx,[ecx+0x11]
  401514:	89 56 08             	mov    DWORD PTR [esi+0x8],edx
  401517:	42                   	inc    edx
  401518:	2b da                	sub    ebx,edx
  40151a:	89 56 0c             	mov    DWORD PTR [esi+0xc],edx
  40151d:	03 d8                	add    ebx,eax
  40151f:	b8 4f ec c4 4e       	mov    eax,0x4ec4ec4f
  401524:	f7 e3                	mul    ebx
  401526:	c1 ea 04             	shr    edx,0x4
  401529:	89 56 10             	mov    DWORD PTR [esi+0x10],edx
  40152c:	8a 19                	mov    bl,BYTE PTR [ecx]
  40152e:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401534:	84 db                	test   bl,bl
  401536:	0f 97 c0             	seta   al
  401539:	88 41 30             	mov    BYTE PTR [ecx+0x30],al
  40153c:	8b 17                	mov    edx,DWORD PTR [edi]
  40153e:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401544:	8b 42 01             	mov    eax,DWORD PTR [edx+0x1]
  401547:	89 41 6c             	mov    DWORD PTR [ecx+0x6c],eax
  40154a:	8b 17                	mov    edx,DWORD PTR [edi]
  40154c:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401552:	8b 42 05             	mov    eax,DWORD PTR [edx+0x5]
  401555:	89 41 70             	mov    DWORD PTR [ecx+0x70],eax
  401558:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  40155d:	8b 17                	mov    edx,DWORD PTR [edi]
  40155f:	8b 48 28             	mov    ecx,DWORD PTR [eax+0x28]
  401562:	8b 52 09             	mov    edx,DWORD PTR [edx+0x9]
  401565:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  401568:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  40156e:	8b 07                	mov    eax,DWORD PTR [edi]
  401570:	8b 51 28             	mov    edx,DWORD PTR [ecx+0x28]
  401573:	8b 40 0d             	mov    eax,DWORD PTR [eax+0xd]
  401576:	89 42 08             	mov    DWORD PTR [edx+0x8],eax
  401579:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  40157f:	e8 dc 43 00 00       	call   0x405960
  401584:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  401587:	8b 06                	mov    eax,DWORD PTR [esi]
  401589:	33 d2                	xor    edx,edx
  40158b:	8a 11                	mov    dl,BYTE PTR [ecx]
  40158d:	8b 0d b4 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6b4
  401593:	52                   	push   edx
  401594:	50                   	push   eax
  401595:	e8 a6 37 00 00       	call   0x404d40
  40159a:	8b 0d 00 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f600
  4015a0:	68 70 80 46 00       	push   0x468070
  4015a5:	6a 18                	push   0x18
  4015a7:	6a 18                	push   0x18
  4015a9:	e8 12 1c 00 00       	call   0x4031c0
  4015ae:	8b 0d c4 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c4
  4015b4:	68 70 80 46 00       	push   0x468070
  4015b9:	e8 62 22 03 00       	call   0x433820
  4015be:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4015c0:	51                   	push   ecx
  4015c1:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4015c7:	e8 a4 37 00 00       	call   0x404d70
  4015cc:	8b 0d f4 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e24f4
  4015d2:	a3 bc f6 46 00       	mov    ds:0x46f6bc,eax
  4015d7:	e8 44 8d 04 00       	call   0x44a320
  4015dc:	5f                   	pop    edi
  4015dd:	5e                   	pop    esi
  4015de:	5b                   	pop    ebx
  4015df:	c2 04 00             	ret    0x4
  4015e2:	90                   	nop
  4015e3:	90                   	nop
  4015e4:	90                   	nop
  4015e5:	90                   	nop
  4015e6:	90                   	nop
  4015e7:	90                   	nop
  4015e8:	90                   	nop
  4015e9:	90                   	nop
  4015ea:	90                   	nop
  4015eb:	90                   	nop
  4015ec:	90                   	nop
  4015ed:	90                   	nop
  4015ee:	90                   	nop
  4015ef:	90                   	nop
  4015f0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4015f4:	8b 0d f4 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e24f4
  4015fa:	50                   	push   eax
  4015fb:	e8 d0 8e 04 00       	call   0x44a4d0
  401600:	c2 04 00             	ret    0x4
  401603:	90                   	nop
  401604:	90                   	nop
  401605:	90                   	nop
  401606:	90                   	nop
  401607:	90                   	nop
  401608:	90                   	nop
  401609:	90                   	nop
  40160a:	90                   	nop
  40160b:	90                   	nop
  40160c:	90                   	nop
  40160d:	90                   	nop
  40160e:	90                   	nop
  40160f:	90                   	nop
  401610:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  401616:	e8 d5 4b 00 00       	call   0x4061f0
  40161b:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  401620:	8a 48 30             	mov    cl,BYTE PTR [eax+0x30]
  401623:	84 c9                	test   cl,cl
  401625:	0f 94 c1             	sete   cl
  401628:	88 48 30             	mov    BYTE PTR [eax+0x30],cl
  40162b:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  401631:	c7 42 2c 00 00 00 00 	mov    DWORD PTR [edx+0x2c],0x0
  401638:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  40163e:	e9                   	.byte 0xe9
  40163f:	8d                   	.byte 0x8d
