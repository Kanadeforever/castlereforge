; 固化38证据：WAIT每逻辑步+1，以及场景实体到标准SF2 countdown的同tick链。

/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040c000 <.text+0xb000>:
  40c000:	8b c1                	mov    eax,ecx
  40c002:	33 d2                	xor    edx,edx
  40c004:	8b 0d 34 f8 89 00    	mov    ecx,DWORD PTR ds:0x89f834
  40c00a:	56                   	push   esi
  40c00b:	3b ca                	cmp    ecx,edx
  40c00d:	75 25                	jne    0x40c034
  40c00f:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  40c012:	5e                   	pop    esi
  40c013:	89 48 08             	mov    DWORD PTR [eax+0x8],ecx
  40c016:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  40c019:	c6 05 1d f8 89 00 01 	mov    BYTE PTR ds:0x89f81d,0x1
  40c020:	89 0d 2c f8 89 00    	mov    DWORD PTR ds:0x89f82c,ecx
  40c026:	8b 0d 34 f8 89 00    	mov    ecx,DWORD PTR ds:0x89f834
  40c02c:	41                   	inc    ecx
  40c02d:	89 0d 34 f8 89 00    	mov    DWORD PTR ds:0x89f834,ecx
  40c033:	c3                   	ret
  40c034:	3b 0d 2c f8 89 00    	cmp    ecx,DWORD PTR ds:0x89f82c
  40c03a:	74 10                	je     0x40c04c
  40c03c:	41                   	inc    ecx
  40c03d:	c6 05 1d f8 89 00 01 	mov    BYTE PTR ds:0x89f81d,0x1
  40c044:	89 0d 34 f8 89 00    	mov    DWORD PTR ds:0x89f834,ecx
  40c04a:	5e                   	pop    esi
  40c04b:	c3                   	ret
  40c04c:	8b 35 10 f8 89 00    	mov    esi,DWORD PTR ds:0x89f810
  40c052:	50                   	push   eax
  40c053:	46                   	inc    esi
  40c054:	89 15 34 f8 89 00    	mov    DWORD PTR ds:0x89f834,edx
  40c05a:	89 35 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,esi
  40c060:	89 15 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,edx
  40c066:	e8 e5 54 04 00       	call   0x451550
  40c06b:	83 c4 04             	add    esp,0x4
  40c06e:	33 c0                	xor    eax,eax


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040ada0 <.text+0x9da0>:
  40ada0:	56                   	push   esi
  40ada1:	8b f1                	mov    esi,ecx
  40ada3:	57                   	push   edi
  40ada4:	33 ff                	xor    edi,edi
  40ada6:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  40ada9:	7e 28                	jle    0x40add3
  40adab:	53                   	push   ebx
  40adac:	33 db                	xor    ebx,ebx
  40adae:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40adb1:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40adb4:	3b f8                	cmp    edi,eax
  40adb6:	75 09                	jne    0x40adc1
  40adb8:	03 cb                	add    ecx,ebx
  40adba:	e8 c1 e7 ff ff       	call   0x409580
  40adbf:	eb 07                	jmp    0x40adc8
  40adc1:	03 cb                	add    ecx,ebx
  40adc3:	e8 48 e7 ff ff       	call   0x409510
  40adc8:	8b 06                	mov    eax,DWORD PTR [esi]
  40adca:	47                   	inc    edi
  40adcb:	83 c3 38             	add    ebx,0x38
  40adce:	3b f8                	cmp    edi,eax
  40add0:	7c dc                	jl     0x40adae
  40add2:	5b                   	pop    ebx
  40add3:	5f                   	pop    edi
  40add4:	5e                   	pop    esi
  40add5:	c3                   	ret
  40add6:	90                   	nop
  40add7:	90                   	nop
  40add8:	90                   	nop
  40add9:	90                   	nop
  40adda:	90                   	nop
  40addb:	90                   	nop
  40addc:	90                   	nop
  40addd:	90                   	nop
  40adde:	90                   	nop
  40addf:	90                   	nop
  40ade0:	56                   	push   esi
  40ade1:	8b f1                	mov    esi,ecx
  40ade3:	57                   	push   edi
  40ade4:	33 ff                	xor    edi,edi
  40ade6:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  40ade9:	7e 28                	jle    0x40ae13
  40adeb:	53                   	push   ebx
  40adec:	33 db                	xor    ebx,ebx
  40adee:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40adf1:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40adf4:	3b f8                	cmp    edi,eax
  40adf6:	75 09                	jne    0x40ae01
  40adf8:	03 cb                	add    ecx,ebx
  40adfa:	e8 e1 ed ff ff       	call   0x409be0
  40adff:	eb 07                	jmp    0x40ae08
  40ae01:	03 cb                	add    ecx,ebx
  40ae03:	e8 48 ee ff ff       	call   0x409c50
  40ae08:	8b 06                	mov    eax,DWORD PTR [esi]
  40ae0a:	47                   	inc    edi
  40ae0b:	83 c3 38             	add    ebx,0x38
  40ae0e:	3b f8                	cmp    edi,eax
  40ae10:	7c dc                	jl     0x40adee
  40ae12:	5b                   	pop    ebx
  40ae13:	5f                   	pop    edi
  40ae14:	5e                   	pop    esi
  40ae15:	c3                   	ret
  40ae16:	90                   	nop
  40ae17:	90                   	nop
  40ae18:	90                   	nop
  40ae19:	90                   	nop
  40ae1a:	90                   	nop
  40ae1b:	90                   	nop
  40ae1c:	90                   	nop
  40ae1d:	90                   	nop
  40ae1e:	90                   	nop
  40ae1f:	90                   	nop


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00409510 <.text+0x8510>:
  409510:	56                   	push   esi
  409511:	8b f1                	mov    esi,ecx
  409513:	33 c9                	xor    ecx,ecx
  409515:	57                   	push   edi
  409516:	8b 06                	mov    eax,DWORD PTR [esi]
  409518:	8a 88 d8 00 00 00    	mov    cl,BYTE PTR [eax+0xd8]
  40951e:	83 e9 00             	sub    ecx,0x0
  409521:	74 17                	je     0x40953a
  409523:	49                   	dec    ecx
  409524:	75 27                	jne    0x40954d
  409526:	0f bf 48 72          	movsx  ecx,WORD PTR [eax+0x72]
  40952a:	0f bf 50 70          	movsx  edx,WORD PTR [eax+0x70]
  40952e:	51                   	push   ecx
  40952f:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  409532:	52                   	push   edx
  409533:	e8 78 f0 ff ff       	call   0x4085b0
  409538:	eb 13                	jmp    0x40954d
  40953a:	8a 46 35             	mov    al,BYTE PTR [esi+0x35]
  40953d:	84 c0                	test   al,al
  40953f:	74 0c                	je     0x40954d
  409541:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  409544:	6a 00                	push   0x0
  409546:	6a 00                	push   0x0
  409548:	e8 53 d2 ff ff       	call   0x4067a0
  40954d:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409550:	33 ff                	xor    edi,edi
  409552:	85 c0                	test   eax,eax
  409554:	7e 26                	jle    0x40957c
  409556:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  409559:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  40955c:	85 c9                	test   ecx,ecx
  40955e:	74 14                	je     0x409574
  409560:	68 07 b2 01 00       	push   0x1b207
  409565:	68 07 b2 01 00       	push   0x1b207
  40956a:	68 07 b2 01 00       	push   0x1b207
  40956f:	e8 3c 0d 00 00       	call   0x40a2b0
  409574:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409577:	47                   	inc    edi
  409578:	3b f8                	cmp    edi,eax
  40957a:	7c da                	jl     0x409556
  40957c:	5f                   	pop    edi
  40957d:	5e                   	pop    esi
  40957e:	c3                   	ret
  40957f:	90                   	nop
  409580:	83 ec 08             	sub    esp,0x8
  409583:	56                   	push   esi
  409584:	8b f1                	mov    esi,ecx
  409586:	57                   	push   edi
  409587:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  40958a:	85 c9                	test   ecx,ecx
  40958c:	74 09                	je     0x409597
  40958e:	6a 00                	push   0x0
  409590:	6a 00                	push   0x0
  409592:	e8 09 d2 ff ff       	call   0x4067a0
  409597:	e8 d4 f4 ff ff       	call   0x408a70
  40959c:	8b f8                	mov    edi,eax
  40959e:	8d 44 24 08          	lea    eax,[esp+0x8]
  4095a2:	50                   	push   eax
  4095a3:	e8 68 f6 ff ff       	call   0x408c10
  4095a8:	8b c8                	mov    ecx,eax
  4095aa:	e8 71 11 00 00       	call   0x40a720
  4095af:	a0 b9 8b 46 00       	mov    al,ds:0x468bb9
  4095b4:	84 c0                	test   al,al
  4095b6:	74 1e                	je     0x4095d6
  4095b8:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4095bc:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  4095c0:	57                   	push   edi
  4095c1:	51                   	push   ecx
  4095c2:	52                   	push   edx
  4095c3:	e8 68 f2 ff ff       	call   0x408830
  4095c8:	83 c4 08             	add    esp,0x8
  4095cb:	50                   	push   eax
  4095cc:	e8 6f f6 ff ff       	call   0x408c40
  4095d1:	83 c4 08             	add    esp,0x8
  4095d4:	eb 25                	jmp    0x4095fb
  4095d6:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  4095da:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  4095de:	50                   	push   eax
  4095df:	51                   	push   ecx
  4095e0:	c7 05 f4 f7 89 00 00 	mov    DWORD PTR ds:0x89f7f4,0x0
  4095e7:	00 00 00 
  4095ea:	e8 41 f2 ff ff       	call   0x408830
  4095ef:	6a 00                	push   0x0
  4095f1:	6a 00                	push   0x0
  4095f3:	e8 48 f6 ff ff       	call   0x408c40
  4095f8:	83 c4 10             	add    esp,0x10
  4095fb:	e8 10 f6 ff ff       	call   0x408c10
  409600:	8b 56 1c             	mov    edx,DWORD PTR [esi+0x1c]
  409603:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  409606:	89 04 91             	mov    DWORD PTR [ecx+edx*4],eax
  409609:	8b 56 1c             	mov    edx,DWORD PTR [esi+0x1c]
  40960c:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  40960f:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  409615:	89 4c 90 04          	mov    DWORD PTR [eax+edx*4+0x4],ecx
  409619:	8b ce                	mov    ecx,esi
  40961b:	e8 20 00 00 00       	call   0x409640
  409620:	57                   	push   edi
  409621:	8b ce                	mov    ecx,esi
  409623:	e8 38 02 00 00       	call   0x409860
  409628:	8b ce                	mov    ecx,esi
  40962a:	e8 61 00 00 00       	call   0x409690
  40962f:	5f                   	pop    edi
  409630:	5e                   	pop    esi
  409631:	83 c4 08             	add    esp,0x8
  409634:	c3                   	ret
  409635:	90                   	nop
  409636:	90                   	nop
  409637:	90                   	nop
  409638:	90                   	nop
  409639:	90                   	nop
  40963a:	90                   	nop
  40963b:	90                   	nop
  40963c:	90                   	nop
  40963d:	90                   	nop
  40963e:	90                   	nop
  40963f:	90                   	nop
  409640:	56                   	push   esi
  409641:	57                   	push   edi
  409642:	8b f9                	mov    edi,ecx
  409644:	33 f6                	xor    esi,esi
  409646:	8b 47 1c             	mov    eax,DWORD PTR [edi+0x1c]
  409649:	40                   	inc    eax
  40964a:	85 c0                	test   eax,eax
  40964c:	7e 36                	jle    0x409684
  40964e:	53                   	push   ebx
  40964f:	bb 01 00 00 00       	mov    ebx,0x1
  409654:	8b 4f 2c             	mov    ecx,DWORD PTR [edi+0x2c]
  409657:	8b 0c b1             	mov    ecx,DWORD PTR [ecx+esi*4]
  40965a:	85 c9                	test   ecx,ecx
  40965c:	74 1c                	je     0x40967a
  40965e:	6a 00                	push   0x0
  409660:	e8 8b 14 00 00       	call   0x40aaf0
  409665:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  409668:	8b 0c b2             	mov    ecx,DWORD PTR [edx+esi*4]
  40966b:	39 59 40             	cmp    DWORD PTR [ecx+0x40],ebx
  40966e:	75 05                	jne    0x409675
  409670:	39 59 3c             	cmp    DWORD PTR [ecx+0x3c],ebx
  409673:	74 05                	je     0x40967a
  409675:	e8 56 11 00 00       	call   0x40a7d0
  40967a:	8b 47 1c             	mov    eax,DWORD PTR [edi+0x1c]
  40967d:	46                   	inc    esi
  40967e:	40                   	inc    eax
  40967f:	3b f0                	cmp    esi,eax
  409681:	7c d1                	jl     0x409654
  409683:	5b                   	pop    ebx
  409684:	5f                   	pop    edi
  409685:	5e                   	pop    esi
  409686:	c3                   	ret
  409687:	90                   	nop
  409688:	90                   	nop
  409689:	90                   	nop
  40968a:	90                   	nop
  40968b:	90                   	nop
  40968c:	90                   	nop
  40968d:	90                   	nop
  40968e:	90                   	nop
  40968f:	90                   	nop
  409690:	83 ec 14             	sub    esp,0x14
  409693:	56                   	push   esi
  409694:	57                   	push   edi
  409695:	8b f9                	mov    edi,ecx
  409697:	33 f6                	xor    esi,esi
  409699:	8b 47 1c             	mov    eax,DWORD PTR [edi+0x1c]
  40969c:	40                   	inc    eax
  40969d:	85 c0                	test   eax,eax
  40969f:	0f 8e af 01 00 00    	jle    0x409854
  4096a5:	53                   	push   ebx
  4096a6:	55                   	push   ebp
  4096a7:	8b 4f 2c             	mov    ecx,DWORD PTR [edi+0x2c]
  4096aa:	8b 04 b1             	mov    eax,DWORD PTR [ecx+esi*4]
  4096ad:	85 c0                	test   eax,eax
  4096af:	0f 84 90 01 00 00    	je     0x409845
  4096b5:	8b 50 18             	mov    edx,DWORD PTR [eax+0x18]
  4096b8:	8b 40 70             	mov    eax,DWORD PTR [eax+0x70]
  4096bb:	8a 48 76             	mov    cl,BYTE PTR [eax+0x76]
  4096be:	80 f9 01             	cmp    cl,0x1
  4096c1:	74 09                	je     0x4096cc
  4096c3:	80 f9 03             	cmp    cl,0x3
  4096c6:	0f 85 5f 01 00 00    	jne    0x40982b
  4096cc:	80 fa 02             	cmp    dl,0x2
  4096cf:	0f 85 56 01 00 00    	jne    0x40982b
  4096d5:	8a 48 74             	mov    cl,BYTE PTR [eax+0x74]
  4096d8:	84 c9                	test   cl,cl
  4096da:	0f 85 4b 01 00 00    	jne    0x40982b
  4096e0:	56                   	push   esi
  4096e1:	8b cf                	mov    ecx,edi
  4096e3:	c6 44 24 17 00       	mov    BYTE PTR [esp+0x17],0x0
  4096e8:	e8 63 03 00 00       	call   0x409a50
  4096ed:	84 c0                	test   al,al
  4096ef:	0f 84 36 01 00 00    	je     0x40982b
  4096f5:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  4096f8:	8b 0c b2             	mov    ecx,DWORD PTR [edx+esi*4]
  4096fb:	8d 04 b2             	lea    eax,[edx+esi*4]
  4096fe:	8b 51 70             	mov    edx,DWORD PTR [ecx+0x70]
  409701:	80 7a 76 03          	cmp    BYTE PTR [edx+0x76],0x3
  409705:	75 3c                	jne    0x409743
  409707:	e8 64 10 00 00       	call   0x40a770
  40970c:	8b 47 2c             	mov    eax,DWORD PTR [edi+0x2c]
  40970f:	6a 01                	push   0x1
  409711:	8b 0c b0             	mov    ecx,DWORD PTR [eax+esi*4]
  409714:	e8 d7 13 00 00       	call   0x40aaf0
  409719:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  40971c:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  409720:	51                   	push   ecx
  409721:	8b 0c b2             	mov    ecx,DWORD PTR [edx+esi*4]
  409724:	e8 f7 0f 00 00       	call   0x40a720
  409729:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  40972d:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  409731:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  409734:	50                   	push   eax
  409735:	51                   	push   ecx
  409736:	8b 0c b2             	mov    ecx,DWORD PTR [edx+esi*4]
  409739:	e8 c2 0f 00 00       	call   0x40a700
  40973e:	e9 e8 00 00 00       	jmp    0x40982b
  409743:	8b 69 0c             	mov    ebp,DWORD PTR [ecx+0xc]
  409746:	33 db                	xor    ebx,ebx
  409748:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  40974c:	84 c0                	test   al,al
  40974e:	0f 85 d7 00 00 00    	jne    0x40982b
  409754:	8b cb                	mov    ecx,ebx
  409756:	81 e1 01 00 00 80    	and    ecx,0x80000001
  40975c:	79 05                	jns    0x409763
  40975e:	49                   	dec    ecx
  40975f:	83 c9 fe             	or     ecx,0xfffffffe
  409762:	41                   	inc    ecx
  409763:	8b c3                	mov    eax,ebx
  409765:	f7 d9                	neg    ecx
  409767:	99                   	cdq
  409768:	1b c9                	sbb    ecx,ecx
  40976a:	2b c2                	sub    eax,edx
  40976c:	83 e1 fe             	and    ecx,0xfffffffe
  40976f:	8b d5                	mov    edx,ebp
  409771:	d1 f8                	sar    eax,1
  409773:	41                   	inc    ecx
  409774:	40                   	inc    eax
  409775:	0f af c8             	imul   ecx,eax
  409778:	c1 ea 10             	shr    edx,0x10
  40977b:	03 ca                	add    ecx,edx
  40977d:	83 e1 07             	and    ecx,0x7
  409780:	8b 04 8d f8 8b 46 00 	mov    eax,DWORD PTR [ecx*4+0x468bf8]
  409787:	8b 4f 2c             	mov    ecx,DWORD PTR [edi+0x2c]
  40978a:	50                   	push   eax
  40978b:	8b 0c b1             	mov    ecx,DWORD PTR [ecx+esi*4]
  40978e:	e8 dd 0d 00 00       	call   0x40a570
  409793:	56                   	push   esi
  409794:	8b cf                	mov    ecx,edi
  409796:	e8 b5 02 00 00       	call   0x409a50
  40979b:	84 c0                	test   al,al
  40979d:	75 2e                	jne    0x4097cd
  40979f:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  4097a2:	8b 2d e0 f7 89 00    	mov    ebp,DWORD PTR ds:0x89f7e0
  4097a8:	33 c0                	xor    eax,eax
  4097aa:	c6 44 24 13 01       	mov    BYTE PTR [esp+0x13],0x1
  4097af:	8b 14 b2             	mov    edx,DWORD PTR [edx+esi*4]
  4097b2:	8a 45 1a             	mov    al,BYTE PTR [ebp+0x1a]
  4097b5:	25 ff 00 00 00       	and    eax,0xff
  4097ba:	8b 4a 18             	mov    ecx,DWORD PTR [edx+0x18]
  4097bd:	c1 e0 10             	shl    eax,0x10
  4097c0:	81 e1 ff ff 00 ff    	and    ecx,0xff00ffff
  4097c6:	0b c1                	or     eax,ecx
  4097c8:	8b e8                	mov    ebp,eax
  4097ca:	89 6a 18             	mov    DWORD PTR [edx+0x18],ebp
  4097cd:	43                   	inc    ebx
  4097ce:	83 fb 04             	cmp    ebx,0x4
  4097d1:	0f 8c 71 ff ff ff    	jl     0x409748
  4097d7:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  4097db:	84 c0                	test   al,al
  4097dd:	75 4c                	jne    0x40982b
  4097df:	8b 4f 2c             	mov    ecx,DWORD PTR [edi+0x2c]
  4097e2:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  4097e6:	50                   	push   eax
  4097e7:	8b 0c b1             	mov    ecx,DWORD PTR [ecx+esi*4]
  4097ea:	e8 31 0f 00 00       	call   0x40a720
  4097ef:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  4097f3:	8b 4f 2c             	mov    ecx,DWORD PTR [edi+0x2c]
  4097f6:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  4097fa:	52                   	push   edx
  4097fb:	8b 0c b1             	mov    ecx,DWORD PTR [ecx+esi*4]
  4097fe:	50                   	push   eax
  4097ff:	e8 fc 0e 00 00       	call   0x40a700
  409804:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  409807:	8b 1d e0 f7 89 00    	mov    ebx,DWORD PTR ds:0x89f7e0
  40980d:	33 c0                	xor    eax,eax
  40980f:	8b 14 b2             	mov    edx,DWORD PTR [edx+esi*4]
  409812:	8a 43 1a             	mov    al,BYTE PTR [ebx+0x1a]
  409815:	25 ff 00 00 00       	and    eax,0xff
  40981a:	8b 4a 18             	mov    ecx,DWORD PTR [edx+0x18]
  40981d:	c1 e0 10             	shl    eax,0x10
  409820:	81 e1 ff ff 00 ff    	and    ecx,0xff00ffff
  409826:	0b c1                	or     eax,ecx
  409828:	89 42 18             	mov    DWORD PTR [edx+0x18],eax
  40982b:	8b 47 2c             	mov    eax,DWORD PTR [edi+0x2c]
  40982e:	68 07 b2 01 00       	push   0x1b207
  409833:	68 07 b2 01 00       	push   0x1b207
  409838:	68 07 b2 01 00       	push   0x1b207
  40983d:	8b 0c b0             	mov    ecx,DWORD PTR [eax+esi*4]
  409840:	e8 6b 0a 00 00       	call   0x40a2b0
  409845:	8b 4f 1c             	mov    ecx,DWORD PTR [edi+0x1c]
  409848:	46                   	inc    esi
  409849:	41                   	inc    ecx
  40984a:	3b f1                	cmp    esi,ecx
  40984c:	0f 8c 55 fe ff ff    	jl     0x4096a7
  409852:	5d                   	pop    ebp
  409853:	5b                   	pop    ebx
  409854:	5f                   	pop    edi
  409855:	5e                   	pop    esi
  409856:	83 c4 14             	add    esp,0x14
  409859:	c3                   	ret
  40985a:	90                   	nop
  40985b:	90                   	nop
  40985c:	90                   	nop
  40985d:	90                   	nop
  40985e:	90                   	nop
  40985f:	90                   	nop
  409860:	83 ec 08             	sub    esp,0x8
  409863:	33 c0                	xor    eax,eax
  409865:	53                   	push   ebx
  409866:	56                   	push   esi
  409867:	8b f1                	mov    esi,ecx
  409869:	57                   	push   edi
  40986a:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  40986d:	c7 46 20 00 00 00 00 	mov    DWORD PTR [esi+0x20],0x0
  409874:	83 c1 02             	add    ecx,0x2
  409877:	85 c9                	test   ecx,ecx
  409879:	7e 25                	jle    0x4098a0
  40987b:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  40987e:	8b 0c              	mov    ecx,DWORD PTR [edx+eax*4]


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040a2b0 <.text+0x92b0>:
  40a2b0:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  40a2b4:	53                   	push   ebx
  40a2b5:	55                   	push   ebp
  40a2b6:	56                   	push   esi
  40a2b7:	81 fa 07 b2 01 00    	cmp    edx,0x1b207
  40a2bd:	57                   	push   edi
  40a2be:	8b f1                	mov    esi,ecx
  40a2c0:	75 03                	jne    0x40a2c5
  40a2c2:	8b 56 10             	mov    edx,DWORD PTR [esi+0x10]
  40a2c5:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  40a2c9:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  40a2cc:	81 f9 07 b2 01 00    	cmp    ecx,0x1b207
  40a2d2:	75 03                	jne    0x40a2d7
  40a2d4:	8b 4e 14             	mov    ecx,DWORD PTR [esi+0x14]
  40a2d7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  40a2db:	89 4e 08             	mov    DWORD PTR [esi+0x8],ecx
  40a2de:	3d 07 b2 01 00       	cmp    eax,0x1b207
  40a2e3:	75 03                	jne    0x40a2e8
  40a2e5:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  40a2e8:	89 4e 14             	mov    DWORD PTR [esi+0x14],ecx
  40a2eb:	8a 4e 54             	mov    cl,BYTE PTR [esi+0x54]
  40a2ee:	89 56 10             	mov    DWORD PTR [esi+0x10],edx
  40a2f1:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  40a2f4:	84 c9                	test   cl,cl
  40a2f6:	8b 4e 70             	mov    ecx,DWORD PTR [esi+0x70]
  40a2f9:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  40a2fc:	ba 01 00 00 00       	mov    edx,0x1
  40a301:	0f 84 c9 00 00 00    	je     0x40a3d0
  40a307:	8b 3d 08 f8 89 00    	mov    edi,DWORD PTR ds:0x89f808
  40a30d:	33 d2                	xor    edx,edx
  40a30f:	8a 91 eb 00 00 00    	mov    dl,BYTE PTR [ecx+0xeb]
  40a315:	3b fa                	cmp    edi,edx
  40a317:	0f 85 96 00 00 00    	jne    0x40a3b3
  40a31d:	80 b9 df 00 00 00 04 	cmp    BYTE PTR [ecx+0xdf],0x4
  40a324:	0f 82 89 00 00 00    	jb     0x40a3b3
  40a32a:	80 79 76 03          	cmp    BYTE PTR [ecx+0x76],0x3
  40a32e:	75 05                	jne    0x40a335
  40a330:	24 00                	and    al,0x0
  40a332:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  40a335:	e8 d6 e8 ff ff       	call   0x408c10
  40a33a:	8b 78 04             	mov    edi,DWORD PTR [eax+0x4]
  40a33d:	e8 ce e8 ff ff       	call   0x408c10
  40a342:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  40a345:	8b 5e 08             	mov    ebx,DWORD PTR [esi+0x8]
  40a348:	8b c1                	mov    eax,ecx
  40a34a:	8b 6e 04             	mov    ebp,DWORD PTR [esi+0x4]
  40a34d:	2b c3                	sub    eax,ebx
  40a34f:	99                   	cdq
  40a350:	33 c2                	xor    eax,edx
  40a352:	2b c2                	sub    eax,edx
  40a354:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  40a358:	8b c7                	mov    eax,edi
  40a35a:	2b c5                	sub    eax,ebp
  40a35c:	99                   	cdq
  40a35d:	33 c2                	xor    eax,edx
  40a35f:	2b c2                	sub    eax,edx
  40a361:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  40a365:	3b c2                	cmp    eax,edx
  40a367:	7e 2c                	jle    0x40a395
  40a369:	8b c5                	mov    eax,ebp
  40a36b:	3b f8                	cmp    edi,eax
  40a36d:	7e 12                	jle    0x40a381
  40a36f:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40a372:	25 ff ff 02 ff       	and    eax,0xff02ffff
  40a377:	0d 00 00 02 00       	or     eax,0x20000
  40a37c:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  40a37f:	eb 7f                	jmp    0x40a400
  40a381:	8b 4e 0c             	mov    ecx,DWORD PTR [esi+0xc]
  40a384:	81 e1 ff ff 06 ff    	and    ecx,0xff06ffff
  40a38a:	81 c9 00 00 06 00    	or     ecx,0x60000
  40a390:	89 4e 0c             	mov    DWORD PTR [esi+0xc],ecx
  40a393:	eb 6b                	jmp    0x40a400
  40a395:	3b cb                	cmp    ecx,ebx
  40a397:	7e 09                	jle    0x40a3a2
  40a399:	81 66 0c ff ff 00 ff 	and    DWORD PTR [esi+0xc],0xff00ffff
  40a3a0:	eb 5e                	jmp    0x40a400
  40a3a2:	8b 56 0c             	mov    edx,DWORD PTR [esi+0xc]
  40a3a5:	81 e2 ff ff 04 ff    	and    edx,0xff04ffff
  40a3ab:	81 ca 00 00 04 00    	or     edx,0x40000
  40a3b1:	eb 4a                	jmp    0x40a3fd
  40a3b3:	8a 46 57             	mov    al,BYTE PTR [esi+0x57]
  40a3b6:	33 d2                	xor    edx,edx
  40a3b8:	88 41 76             	mov    BYTE PTR [ecx+0x76],al
  40a3bb:	8b 4e 70             	mov    ecx,DWORD PTR [esi+0x70]
  40a3be:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40a3c1:	c6 46 54 00          	mov    BYTE PTR [esi+0x54],0x0
  40a3c5:	8a 91 03 01 00 00    	mov    dl,BYTE PTR [ecx+0x103]
  40a3cb:	c1 e2 10             	shl    edx,0x10
  40a3ce:	eb 26                	jmp    0x40a3f6
  40a3d0:	8a 59 76             	mov    bl,BYTE PTR [ecx+0x76]
  40a3d3:	84 db                	test   bl,bl
  40a3d5:	75 29                	jne    0x40a400
  40a3d7:	8a 99 df 00 00 00    	mov    bl,BYTE PTR [ecx+0xdf]
  40a3dd:	84 db                	test   bl,bl
  40a3df:	76 1f                	jbe    0x40a400
  40a3e1:	39 56 40             	cmp    DWORD PTR [esi+0x40],edx
  40a3e4:	75 1a                	jne    0x40a400
  40a3e6:	39 56 3c             	cmp    DWORD PTR [esi+0x3c],edx
  40a3e9:	75 15                	jne    0x40a400
  40a3eb:	33 d2                	xor    edx,edx
  40a3ed:	8a 91 03 01 00 00    	mov    dl,BYTE PTR [ecx+0x103]
  40a3f3:	c1 e2 10             	shl    edx,0x10
  40a3f6:	25 ff ff 00 ff       	and    eax,0xff00ffff
  40a3fb:	0b d0                	or     edx,eax
  40a3fd:	89 56 0c             	mov    DWORD PTR [esi+0xc],edx
  40a400:	8b 7e 70             	mov    edi,DWORD PTR [esi+0x70]
  40a403:	80 7f 76 03          	cmp    BYTE PTR [edi+0x76],0x3
  40a407:	75 14                	jne    0x40a41d
  40a409:	8b 46 58             	mov    eax,DWORD PTR [esi+0x58]
  40a40c:	8b 4e 5c             	mov    ecx,DWORD PTR [esi+0x5c]
  40a40f:	3b c1                	cmp    eax,ecx
  40a411:	74 0a                	je     0x40a41d
  40a413:	80 7e 0c 02          	cmp    BYTE PTR [esi+0xc],0x2
  40a417:	0f 84 db 00 00 00    	je     0x40a4f8
  40a41d:	8b 5e 0c             	mov    ebx,DWORD PTR [esi+0xc]
  40a420:	33 c0                	xor    eax,eax
  40a422:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  40a426:	33 c9                	xor    ecx,ecx
  40a428:	8a 4c 24 1a          	mov    cl,BYTE PTR [esp+0x1a]
  40a42c:	c7 46 58 00 00 00 00 	mov    DWORD PTR [esi+0x58],0x0
  40a433:	8a 87 df 00 00 00    	mov    al,BYTE PTR [edi+0xdf]
  40a439:	bd 01 00 00 00       	mov    ebp,0x1
  40a43e:	0f af c1             	imul   eax,ecx
  40a441:	8b cb                	mov    ecx,ebx
  40a443:	ba 07 b2 01 00       	mov    edx,0x1b207
  40a448:	81 e1 ff 00 00 00    	and    ecx,0xff
  40a44e:	c1 e8 03             	shr    eax,0x3
  40a451:	83 f9 03             	cmp    ecx,0x3
  40a454:	77 77                	ja     0x40a4cd
  40a456:	ff 24 8d 00 a5 40 00 	jmp    DWORD PTR [ecx*4+0x40a500]
  40a45d:	33 d2                	xor    edx,edx
  40a45f:	33 c9                	xor    ecx,ecx
  40a461:	8a 97 e3 00 00 00    	mov    dl,BYTE PTR [edi+0xe3]
  40a467:	8a 8f e2 00 00 00    	mov    cl,BYTE PTR [edi+0xe2]
  40a46d:	0f af c2             	imul   eax,edx
  40a470:	8b e9                	mov    ebp,ecx
  40a472:	03 e8                	add    ebp,eax
  40a474:	eb 57                	jmp    0x40a4cd
  40a476:	33 d2                	xor    edx,edx
  40a478:	33 c0                	xor    eax,eax
  40a47a:	8a 87 e8 00 00 00    	mov    al,BYTE PTR [edi+0xe8]
  40a480:	8a 97 e9 00 00 00    	mov    dl,BYTE PTR [edi+0xe9]
  40a486:	8b e8                	mov    ebp,eax
  40a488:	eb 43                	jmp    0x40a4cd
  40a48a:	33 c9                	xor    ecx,ecx
  40a48c:	8a cf                	mov    cl,bh
  40a48e:	83 e9 00             	sub    ecx,0x0
  40a491:	74 1c                	je     0x40a4af
  40a493:	49                   	dec    ecx
  40a494:	75 37                	jne    0x40a4cd
  40a496:	33 d2                	xor    edx,edx
  40a498:	33 c9                	xor    ecx,ecx
  40a49a:	8a 97 e7 00 00 00    	mov    dl,BYTE PTR [edi+0xe7]
  40a4a0:	8a 8f e6 00 00 00    	mov    cl,BYTE PTR [edi+0xe6]
  40a4a6:	0f af c2             	imul   eax,edx
  40a4a9:	8b e9                	mov    ebp,ecx
  40a4ab:	03 e8                	add    ebp,eax
  40a4ad:	eb 1e                	jmp    0x40a4cd
  40a4af:	33 d2                	xor    edx,edx
  40a4b1:	33 c9                	xor    ecx,ecx
  40a4b3:	8a 97 e5 00 00 00    	mov    dl,BYTE PTR [edi+0xe5]
  40a4b9:	8a 8f e4 00 00 00    	mov    cl,BYTE PTR [edi+0xe4]
  40a4bf:	0f af c2             	imul   eax,edx
  40a4c2:	8b e9                	mov    ebp,ecx
  40a4c4:	03 e8                	add    ebp,eax
  40a4c6:	eb 05                	jmp    0x40a4cd
  40a4c8:	bd 0e 64 03 00       	mov    ebp,0x3640e
  40a4cd:	8b 4e 6c             	mov    ecx,DWORD PTR [esi+0x6c]
  40a4d0:	85 c9                	test   ecx,ecx
  40a4d2:	74 24                	je     0x40a4f8
  40a4d4:	8a 47 76             	mov    al,BYTE PTR [edi+0x76]
  40a4d7:	8b 5e 08             	mov    ebx,DWORD PTR [esi+0x8]
  40a4da:	3c 02                	cmp    al,0x2
  40a4dc:	0f 95 c0             	setne  al
  40a4df:	50                   	push   eax
  40a4e0:	0f bf 47 6e          	movsx  eax,WORD PTR [edi+0x6e]
  40a4e4:	03 c3                	add    eax,ebx
  40a4e6:	8b 5e 04             	mov    ebx,DWORD PTR [esi+0x4]
  40a4e9:	50                   	push   eax
  40a4ea:	0f bf 47 6c          	movsx  eax,WORD PTR [edi+0x6c]
  40a4ee:	03 c3                	add    eax,ebx
  40a4f0:	50                   	push   eax
  40a4f1:	52                   	push   edx
  40a4f2:	55                   	push   ebp
  40a4f3:	e8 d8 ce ff ff       	call   0x4073d0
  40a4f8:	5f                   	pop    edi
  40a4f9:	5e                   	pop    esi
  40a4fa:	5d                   	pop    ebp
  40a4fb:	5b                   	pop    ebx
  40a4fc:	c2 0c 00             	ret    0xc
  40a4ff:	90                   	nop
  40a500:	5d                   	pop    ebp
  40a501:	a4                   	movs   BYTE PTR es:[edi],BYTE PTR ds:[esi]
  40a502:	40                   	inc    eax
  40a503:	00 76 a4             	add    BYTE PTR [esi-0x5c],dh
  40a506:	40                   	inc    eax
  40a507:	00 8a a4 40 00 c8    	add    BYTE PTR [edx-0x37ffbf5c],cl
  40a50d:	a4                   	movs   BYTE PTR es:[edi],BYTE PTR ds:[esi]
  40a50e:	40                   	inc    eax
  40a50f:	00 56 8b             	add    BYTE PTR [esi-0x75],dl
  40a512:	f1                   	int1
  40a513:	80 7e 0c 04          	cmp    BYTE PTR [esi+0xc],0x4
  40a517:	74 52                	je     0x40a56b
  40a519:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  40a51d:	85 c0                	test   eax,eax
  40a51f:	74                 	je     0x40a563


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

004073d0 <.text+0x63d0>:
  4073d0:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  4073d4:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  4073d8:	89 01                	mov    DWORD PTR [ecx],eax
  4073da:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4073de:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  4073e1:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  4073e5:	50                   	push   eax
  4073e6:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  4073ea:	52                   	push   edx
  4073eb:	50                   	push   eax
  4073ec:	e8 0f 00 00 00       	call   0x407400
  4073f1:	c2 14 00             	ret    0x14
  4073f4:	90                   	nop
  4073f5:	90                   	nop
  4073f6:	90                   	nop
  4073f7:	90                   	nop
  4073f8:	90                   	nop
  4073f9:	90                   	nop
  4073fa:	90                   	nop
  4073fb:	90                   	nop
  4073fc:	90                   	nop
  4073fd:	90                   	nop
  4073fe:	90                   	nop
  4073ff:	90                   	nop
  407400:	8b 41 18             	mov    eax,DWORD PTR [ecx+0x18]
  407403:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  407407:	85 c0                	test   eax,eax
  407409:	56                   	push   esi
  40740a:	74 11                	je     0x40741d
  40740c:	39 51 10             	cmp    DWORD PTR [ecx+0x10],edx
  40740f:	75 0c                	jne    0x40741d
  407411:	48                   	dec    eax
  407412:	c6 41 2d 00          	mov    BYTE PTR [ecx+0x2d],0x0
  407416:	89 41 18             	mov    DWORD PTR [ecx+0x18],eax
  407419:	5e                   	pop    esi
  40741a:	c2 0c 00             	ret    0xc
  40741d:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  407420:	3b c2                	cmp    eax,edx
  407422:	74 1c                	je     0x407440
  407424:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  407428:	89 51 10             	mov    DWORD PTR [ecx+0x10],edx
  40742b:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  40742e:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  407431:	c7 41 18 01 00 00 00 	mov    DWORD PTR [ecx+0x18],0x1
  407438:	c6 41 2d 01          	mov    BYTE PTR [ecx+0x2d],0x1
  40743c:	5e                   	pop    esi
  40743d:	c2 0c 00             	ret    0xc
  407440:	8b 51 14             	mov    edx,DWORD PTR [ecx+0x14]
  407443:	8b 71 0c             	mov    esi,DWORD PTR [ecx+0xc]
  407446:	42                   	inc    edx
  407447:	03 f0                	add    esi,eax
  407449:	3b d6                	cmp    edx,esi
  40744b:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  40744e:	72 28                	jb     0x407478
  407450:	80 7c 24 10 00       	cmp    BYTE PTR [esp+0x10],0x0
  407455:	74 05                	je     0x40745c
  407457:	89 41 14             	mov    DWORD PTR [ecx+0x14],eax
  40745a:	eb 1c                	jmp    0x407478
  40745c:	8a 81 80 00 00 00    	mov    al,BYTE PTR [ecx+0x80]
  407462:	4a                   	dec    edx
  407463:	84 c0                	test   al,al
  407465:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  407468:	74 0e                	je     0x407478
  40746a:	c6 05 1e f8 89 00 00 	mov    BYTE PTR ds:0x89f81e,0x0
  407471:	c6 81 80 00 00 00 00 	mov    BYTE PTR [ecx+0x80],0x0
  407478:	8b 51 14             	mov    edx,DWORD PTR [ecx+0x14]
  40747b:	8b 41 38             	mov    eax,DWORD PTR [ecx+0x38]
  40747e:	33 f6                	xor    esi,esi
  407480:	8b 14 90             	mov    edx,DWORD PTR [eax+edx*4]
  407483:	8b 41 34             	mov    eax,DWORD PTR [ecx+0x34]
  407486:	66 8b 74 02 22       	mov    si,WORD PTR [edx+eax*1+0x22]
  40748b:	c6 41 2d 01          	mov    BYTE PTR [ecx+0x2d],0x1
  40748f:	89 71 18             	mov    DWORD PTR [ecx+0x18],esi
  407492:	5e                   	pop    esi
  407493:	c2 0c 00             	ret    0xc
  407496:	90                   	nop
  407497:	90                   	nop
  407498:	90                   	nop
  407499:	90                   	nop
  40749a:	90                   	nop
  40749b:	90                   	nop
  40749c:	90                   	nop
  40749d:	90                   	nop
  40749e:	90                   	nop
  40749f:	90                   	nop
