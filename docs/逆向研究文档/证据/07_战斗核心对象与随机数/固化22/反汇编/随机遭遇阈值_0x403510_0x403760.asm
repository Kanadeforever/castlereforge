
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00403510 <.text+0x2510>:
  403510:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  403514:	85 c0                	test   eax,eax
  403516:	75 0b                	jne    0x403523
  403518:	c7 05 14 f6 46 00 00 	mov    DWORD PTR ds:0x46f614,0x0
  40351f:	00 00 00 
  403522:	c3                   	ret
  403523:	56                   	push   esi
  403524:	c7 05 10 f6 46 00 96 	mov    DWORD PTR ds:0x46f610,0x96
  40352b:	00 00 00 
  40352e:	e8 32 e0 04 00       	call   0x451565
  403533:	8b 0d 10 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f610
  403539:	99                   	cdq
  40353a:	8d 34 09             	lea    esi,[ecx+ecx*1]
  40353d:	f7 fe                	idiv   esi
  40353f:	a1 1c f6 46 00       	mov    eax,ds:0x46f61c
  403544:	5e                   	pop    esi
  403545:	03 d1                	add    edx,ecx
  403547:	83 f8 05             	cmp    eax,0x5
  40354a:	89 15 14 f6 46 00    	mov    DWORD PTR ds:0x46f614,edx
  403550:	7e 30                	jle    0x403582
  403552:	a0 6d 84 46 00       	mov    al,ds:0x46846d
  403557:	84 c0                	test   al,al
  403559:	74 13                	je     0x40356e
  40355b:	8d 04 12             	lea    eax,[edx+edx*1]
  40355e:	c7 05 18 f6 46 00 00 	mov    DWORD PTR ds:0x46f618,0x0
  403565:	00 00 00 
  403568:	a3 14 f6 46 00       	mov    ds:0x46f614,eax
  40356d:	c3                   	ret
  40356e:	b8 56 55 55 55       	mov    eax,0x55555556
  403573:	f7 ea                	imul   edx
  403575:	8b ca                	mov    ecx,edx
  403577:	c1 e9 1f             	shr    ecx,0x1f
  40357a:	03 d1                	add    edx,ecx
  40357c:	89 15 14 f6 46 00    	mov    DWORD PTR ds:0x46f614,edx
  403582:	c7 05 18 f6 46 00 00 	mov    DWORD PTR ds:0x46f618,0x0
  403589:	00 00 00 
  40358c:	c3                   	ret
  40358d:	90                   	nop
  40358e:	90                   	nop
  40358f:	90                   	nop
  403590:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  403594:	a3 08 f6 46 00       	mov    ds:0x46f608,eax
  403599:	c3                   	ret
  40359a:	90                   	nop
  40359b:	90                   	nop
  40359c:	90                   	nop
  40359d:	90                   	nop
  40359e:	90                   	nop
  40359f:	90                   	nop
  4035a0:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  4035a6:	6a ff                	push   0xffffffff
  4035a8:	68 66 cf 45 00       	push   0x45cf66
  4035ad:	50                   	push   eax
  4035ae:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4035b3:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  4035ba:	83 ec 6c             	sub    esp,0x6c
  4035bd:	8b 80 18 01 00 00    	mov    eax,DWORD PTR [eax+0x118]
  4035c3:	53                   	push   ebx
  4035c4:	33 db                	xor    ebx,ebx
  4035c6:	3b c3                	cmp    eax,ebx
  4035c8:	56                   	push   esi
  4035c9:	0f 84 0f 01 00 00    	je     0x4036de
  4035cf:	83 f8 02             	cmp    eax,0x2
  4035d2:	0f 85 88 00 00 00    	jne    0x403660
  4035d8:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  4035dc:	68 84 84 46 00       	push   0x468484
  4035e1:	51                   	push   ecx
  4035e2:	ff 15 94 01 46 00    	call   DWORD PTR ds:0x460194
  4035e8:	83 c4 08             	add    esp,0x8
  4035eb:	e8 b0 7d 00 00       	call   0x40b3a0
  4035f0:	8b c8                	mov    ecx,eax
  4035f2:	e8 29 7b 00 00       	call   0x40b120
  4035f7:	e8 d4 38 00 00       	call   0x406ed0
  4035fc:	68 84 02 00 00       	push   0x284
  403601:	e8 59 e0 04 00       	call   0x45165f
  403606:	83 c4 04             	add    esp,0x4
  403609:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  40360d:	3b c3                	cmp    eax,ebx
  40360f:	89 5c 24 7c          	mov    DWORD PTR [esp+0x7c],ebx
  403613:	74 09                	je     0x40361e
  403615:	8b c8                	mov    ecx,eax
  403617:	e8 64 78 00 00       	call   0x40ae80
  40361c:	eb 02                	jmp    0x403620
  40361e:	33 c0                	xor    eax,eax
  403620:	8d 54 24 10          	lea    edx,[esp+0x10]
  403624:	8b c8                	mov    ecx,eax
  403626:	52                   	push   edx
  403627:	c7 84 24 80 00 00 00 	mov    DWORD PTR [esp+0x80],0xffffffff
  40362e:	ff ff ff ff 
  403632:	e8 09 79 00 00       	call   0x40af40
  403637:	6a 0b                	push   0xb
  403639:	e8 f2 7b 00 00       	call   0x40b230
  40363e:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  403644:	83 c4 04             	add    esp,0x4
  403647:	32 c0                	xor    al,al
  403649:	89 9a 18 01 00 00    	mov    DWORD PTR [edx+0x118],ebx
  40364f:	5e                   	pop    esi
  403650:	5b                   	pop    ebx
  403651:	8b 4c 24 6c          	mov    ecx,DWORD PTR [esp+0x6c]
  403655:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40365c:	83 c4 78             	add    esp,0x78
  40365f:	c3                   	ret
  403660:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  403666:	57                   	push   edi
  403667:	e8 c4 fd ff ff       	call   0x403430
  40366c:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  403672:	e8 19 fe ff ff       	call   0x403490
  403677:	e8 24 7d 00 00       	call   0x40b3a0
  40367c:	8b 88 80 02 00 00    	mov    ecx,DWORD PTR [eax+0x280]
  403682:	e8 e9 77 00 00       	call   0x40ae70
  403687:	8b 38                	mov    edi,DWORD PTR [eax]
  403689:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  40368f:	33 c0                	xor    eax,eax
  403691:	6a 01                	push   0x1
  403693:	8a 87 31 03 00 00    	mov    al,BYTE PTR [edi+0x331]
  403699:	50                   	push   eax
  40369a:	e8 21 fc ff ff       	call   0x4032c0
  40369f:	33 f6                	xor    esi,esi
  4036a1:	33 c9                	xor    ecx,ecx
  4036a3:	6a 01                	push   0x1
  4036a5:	8a 8c 37 73 03 00 00 	mov    cl,BYTE PTR [edi+esi*1+0x373]
  4036ac:	51                   	push   ecx
  4036ad:	8b 0d 04 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f604
  4036b3:	e8 c8 fc ff ff       	call   0x403380
  4036b8:	46                   	inc    esi
  4036b9:	83 fe 05             	cmp    esi,0x5
  4036bc:	7c e3                	jl     0x4036a1
  4036be:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4036c4:	5f                   	pop    edi
  4036c5:	5e                   	pop    esi
  4036c6:	32 c0                	xor    al,al
  4036c8:	89 9a 18 01 00 00    	mov    DWORD PTR [edx+0x118],ebx
  4036ce:	5b                   	pop    ebx
  4036cf:	8b 4c 24 6c          	mov    ecx,DWORD PTR [esp+0x6c]
  4036d3:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4036da:	83 c4 78             	add    esp,0x78
  4036dd:	c3                   	ret
  4036de:	39 1d 08 f8 89 00    	cmp    DWORD PTR ds:0x89f808,ebx
  4036e4:	0f 85 bf 01 00 00    	jne    0x4038a9
  4036ea:	a1 14 f6 46 00       	mov    eax,ds:0x46f614
  4036ef:	8b 0d 18 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f618
  4036f5:	3b c8                	cmp    ecx,eax
  4036f7:	0f 8c ac 01 00 00    	jl     0x4038a9
  4036fd:	3b c3                	cmp    eax,ebx
  4036ff:	0f 84 a4 01 00 00    	je     0x4038a9
  403705:	a0 6c 84 46 00       	mov    al,ds:0x46846c
  40370a:	89 1d 18 f6 46 00    	mov    DWORD PTR ds:0x46f618,ebx
  403710:	3a c3                	cmp    al,bl
  403712:	0f 84 91 01 00 00    	je     0x4038a9
  403718:	e8 48 de 04 00       	call   0x451565
  40371d:	8b 0d 10 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f610
  403723:	99                   	cdq
  403724:	8d 34 09             	lea    esi,[ecx+ecx*1]
  403727:	f7 fe                	idiv   esi
  403729:	a1 1c f6 46 00       	mov    eax,ds:0x46f61c
  40372e:	03 d1                	add    edx,ecx
  403730:	83 f8 05             	cmp    eax,0x5
  403733:	89 15 14 f6 46 00    	mov    DWORD PTR ds:0x46f614,edx
  403739:	7e 26                	jle    0x403761
  40373b:	38 1d 6d 84 46 00    	cmp    BYTE PTR ds:0x46846d,bl
  403741:	74 0a                	je     0x40374d
  403743:	8d 04 12             	lea    eax,[edx+edx*1]
  403746:	a3 14 f6 46 00       	mov    ds:0x46f614,eax
  40374b:	eb 14                	jmp    0x403761
  40374d:	b8 56 55 55 55       	mov    eax,0x55555556
  403752:	f7 ea                	imul   edx
  403754:	8b ca                	mov    ecx,edx
  403756:	c1 e9 1f             	shr    ecx,0x1f
  403759:	03 d1                	add    edx,ecx
  40375b:	89                   	.byte 0x89
  40375c:	15                   	.byte 0x15
  40375d:	14 f6                	adc    al,0xf6
  40375f:	46                   	inc    esi
