
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00409690 <.text+0x8690>:
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
  40984c:	0f                   	.byte 0xf
  40984d:	8c 55 fe             	mov    WORD PTR [ebp-0x2],ss
