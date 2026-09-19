
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004245b0 <.text+0x235b0>:
  4245b0:	6a ff                	push   0xffffffff
  4245b2:	68 a1 e3 45 00       	push   0x45e3a1
  4245b7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  4245bd:	50                   	push   eax
  4245be:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  4245c5:	83 ec 20             	sub    esp,0x20
  4245c8:	53                   	push   ebx
  4245c9:	56                   	push   esi
  4245ca:	8b f1                	mov    esi,ecx
  4245cc:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  4245d0:	e8 3b c9 00 00       	call   0x430f10
  4245d5:	33 db                	xor    ebx,ebx
  4245d7:	8b ce                	mov    ecx,esi
  4245d9:	53                   	push   ebx
  4245da:	68 48 85 1e 00       	push   0x1e8548
  4245df:	56                   	push   esi
  4245e0:	89 5c 24 3c          	mov    DWORD PTR [esp+0x3c],ebx
  4245e4:	c7 06 50 0b 46 00    	mov    DWORD PTR [esi],0x460b50
  4245ea:	e8 41 fe 00 00       	call   0x434430
  4245ef:	8d 8e 18 02 00 00    	lea    ecx,[esi+0x218]
  4245f5:	e8 36 07 fe ff       	call   0x404d30
  4245fa:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  4245fe:	8d 86 68 05 00 00    	lea    eax,[esi+0x568]
  424604:	8d 8e 6c 05 00 00    	lea    ecx,[esi+0x56c]
  42460a:	68 c0 05 00 00       	push   0x5c0
  42460f:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  424613:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  424617:	e8 43 d0 02 00       	call   0x45165f
  42461c:	83 c4 04             	add    esp,0x4
  42461f:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  424623:	3b c3                	cmp    eax,ebx
  424625:	c6 44 24 30 01       	mov    BYTE PTR [esp+0x30],0x1
  42462a:	74 09                	je     0x424635
  42462c:	8b c8                	mov    ecx,eax
  42462e:	e8 6d 12 00 00       	call   0x4258a0
  424633:	eb 02                	jmp    0x424637
  424635:	33 c0                	xor    eax,eax
  424637:	68 c0 05 00 00       	push   0x5c0
  42463c:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  424640:	89 86 84 05 00 00    	mov    DWORD PTR [esi+0x584],eax
  424646:	e8 14 d0 02 00       	call   0x45165f
  42464b:	83 c4 04             	add    esp,0x4
  42464e:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  424652:	3b c3                	cmp    eax,ebx
  424654:	c6 44 24 30 02       	mov    BYTE PTR [esp+0x30],0x2
  424659:	74 09                	je     0x424664
  42465b:	8b c8                	mov    ecx,eax
  42465d:	e8 3e 12 00 00       	call   0x4258a0
  424662:	eb 02                	jmp    0x424666
  424664:	33 c0                	xor    eax,eax
  424666:	68 c0 05 00 00       	push   0x5c0
  42466b:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  42466f:	89 86 88 05 00 00    	mov    DWORD PTR [esi+0x588],eax
  424675:	e8 e5 cf 02 00       	call   0x45165f
  42467a:	83 c4 04             	add    esp,0x4
  42467d:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  424681:	3b c3                	cmp    eax,ebx
  424683:	c6 44 24 30 03       	mov    BYTE PTR [esp+0x30],0x3
  424688:	74 09                	je     0x424693
  42468a:	8b c8                	mov    ecx,eax
  42468c:	e8 0f 12 00 00       	call   0x4258a0
  424691:	eb 02                	jmp    0x424695
  424693:	33 c0                	xor    eax,eax
  424695:	68 c0 05 00 00       	push   0x5c0
  42469a:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  42469e:	89 86 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],eax
  4246a4:	e8 b6 cf 02 00       	call   0x45165f
  4246a9:	83 c4 04             	add    esp,0x4
  4246ac:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  4246b0:	3b c3                	cmp    eax,ebx
  4246b2:	c6 44 24 30 04       	mov    BYTE PTR [esp+0x30],0x4
  4246b7:	74 09                	je     0x4246c2
  4246b9:	8b c8                	mov    ecx,eax
  4246bb:	e8 e0 11 00 00       	call   0x4258a0
  4246c0:	eb 02                	jmp    0x4246c4
  4246c2:	33 c0                	xor    eax,eax
  4246c4:	8b 96 84 05 00 00    	mov    edx,DWORD PTR [esi+0x584]
  4246ca:	89 86 90 05 00 00    	mov    DWORD PTR [esi+0x590],eax
  4246d0:	b8 14 00 00 00       	mov    eax,0x14
  4246d5:	6a 48                	push   0x48
  4246d7:	89 82 68 05 00 00    	mov    DWORD PTR [edx+0x568],eax
  4246dd:	8b 8e 84 05 00 00    	mov    ecx,DWORD PTR [esi+0x584]
  4246e3:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  4246e7:	c7 81 6c 05 00 00 4b 	mov    DWORD PTR [ecx+0x56c],0x4b
  4246ee:	00 00 00 
  4246f1:	8b 96 88 05 00 00    	mov    edx,DWORD PTR [esi+0x588]
  4246f7:	89 82 68 05 00 00    	mov    DWORD PTR [edx+0x568],eax
  4246fd:	8b 8e 88 05 00 00    	mov    ecx,DWORD PTR [esi+0x588]
  424703:	c7 81 6c 05 00 00 aa 	mov    DWORD PTR [ecx+0x56c],0xaa
  42470a:	00 00 00 
  42470d:	8b 96 8c 05 00 00    	mov    edx,DWORD PTR [esi+0x58c]
  424713:	89 82 68 05 00 00    	mov    DWORD PTR [edx+0x568],eax
  424719:	8b 8e 8c 05 00 00    	mov    ecx,DWORD PTR [esi+0x58c]
  42471f:	c7 81 6c 05 00 00 09 	mov    DWORD PTR [ecx+0x56c],0x109
  424726:	01 00 00 
  424729:	8b 96 90 05 00 00    	mov    edx,DWORD PTR [esi+0x590]
  42472f:	89 82 68 05 00 00    	mov    DWORD PTR [edx+0x568],eax
  424735:	8b 86 90 05 00 00    	mov    eax,DWORD PTR [esi+0x590]
  42473b:	c7 80 6c 05 00 00 68 	mov    DWORD PTR [eax+0x56c],0x168
  424742:	01 00 00 
  424745:	8b 8e 8c 05 00 00    	mov    ecx,DWORD PTR [esi+0x58c]
  42474b:	88 99 7c 05 00 00    	mov    BYTE PTR [ecx+0x57c],bl
  424751:	8b 96 90 05 00 00    	mov    edx,DWORD PTR [esi+0x590]
  424757:	88 9a 7c 05 00 00    	mov    BYTE PTR [edx+0x57c],bl
  42475d:	e8 fd ce 02 00       	call   0x45165f
  424762:	83 c4 04             	add    esp,0x4
  424765:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  424769:	3b c3                	cmp    eax,ebx
  42476b:	c6 44 24 30 05       	mov    BYTE PTR [esp+0x30],0x5
  424770:	74 0e                	je     0x424780
  424772:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  424776:	51                   	push   ecx
  424777:	8b c8                	mov    ecx,eax
  424779:	e8 62 ca 00 00       	call   0x4311e0
  42477e:	eb 02                	jmp    0x424782
  424780:	33 c0                	xor    eax,eax
  424782:	57                   	push   edi
  424783:	8d be 9c 05 00 00    	lea    edi,[esi+0x59c]
  424789:	57                   	push   edi
  42478a:	68 10 a4 46 00       	push   0x46a410
  42478f:	53                   	push   ebx
  424790:	53                   	push   ebx
  424791:	6a 2d                	push   0x2d
  424793:	68 2f 01 00 00       	push   0x12f
  424798:	8b c8                	mov    ecx,eax
  42479a:	88 5c 24 4c          	mov    BYTE PTR [esp+0x4c],bl
  42479e:	89 07                	mov    DWORD PTR [edi],eax
  4247a0:	e8 1b cb 00 00       	call   0x4312c0
  4247a5:	8b 17                	mov    edx,DWORD PTR [edi]
  4247a7:	53                   	push   ebx
  4247a8:	8b 4a 30             	mov    ecx,DWORD PTR [edx+0x30]
  4247ab:	e8 f0 9e 01 00       	call   0x43e6a0
  4247b0:	6a 48                	push   0x48
  4247b2:	e8 a8 ce 02 00       	call   0x45165f
  4247b7:	83 c4 04             	add    esp,0x4
  4247ba:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  4247be:	3b c3                	cmp    eax,ebx
  4247c0:	c6 44 24 34 06       	mov    BYTE PTR [esp+0x34],0x6
  4247c5:	74 0e                	je     0x4247d5
  4247c7:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  4247cb:	51                   	push   ecx
  4247cc:	8b c8                	mov    ecx,eax
  4247ce:	e8 0d ca 00 00       	call   0x4311e0
  4247d3:	eb 02                	jmp    0x4247d7
  4247d5:	33 c0                	xor    eax,eax
  4247d7:	8d be a0 05 00 00    	lea    edi,[esi+0x5a0]
  4247dd:	8b c8                	mov    ecx,eax
  4247df:	57                   	push   edi
  4247e0:	68 fc a3 46 00       	push   0x46a3fc
  4247e5:	53                   	push   ebx
  4247e6:	53                   	push   ebx
  4247e7:	68 c7 01 00 00       	push   0x1c7
  4247ec:	68 2f 01 00 00       	push   0x12f
  4247f1:	88 5c 24 4c          	mov    BYTE PTR [esp+0x4c],bl
  4247f5:	89 07                	mov    DWORD PTR [edi],eax
  4247f7:	e8 c4 ca 00 00       	call   0x4312c0
  4247fc:	8b 17                	mov    edx,DWORD PTR [edi]
  4247fe:	53                   	push   ebx
  4247ff:	8b 4a 30             	mov    ecx,DWORD PTR [edx+0x30]
  424802:	e8 99 9e 01 00       	call   0x43e6a0
  424807:	68 b8 05 00 00       	push   0x5b8
  42480c:	e8 4e ce 02 00       	call   0x45165f
  424811:	83 c4 04             	add    esp,0x4
  424814:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  424818:	3b c3                	cmp    eax,ebx
  42481a:	c6 44 24 34 07       	mov    BYTE PTR [esp+0x34],0x7
  42481f:	5f                   	pop    edi
  424820:	74 09                	je     0x42482b
  424822:	8b c8                	mov    ecx,eax
  424824:	e8 b7 17 00 00       	call   0x425fe0
  424829:	eb 02                	jmp    0x42482d
  42482b:	33 c0                	xor    eax,eax
  42482d:	b9 90 01 00 00       	mov    ecx,0x190
  424832:	89 86 a4 05 00 00    	mov    DWORD PTR [esi+0x5a4],eax
  424838:	89 88 a4 05 00 00    	mov    DWORD PTR [eax+0x5a4],ecx
  42483e:	8b 86 a4 05 00 00    	mov    eax,DWORD PTR [esi+0x5a4]
  424844:	68 b0 05 00 00       	push   0x5b0
  424849:	88 5c 24 34          	mov    BYTE PTR [esp+0x34],bl
  42484d:	c7 80 a8 05 00 00 04 	mov    DWORD PTR [eax+0x5a8],0x104
  424854:	01 00 00 
  424857:	8b 96 a4 05 00 00    	mov    edx,DWORD PTR [esi+0x5a4]
  42485d:	89 8a ac 05 00 00    	mov    DWORD PTR [edx+0x5ac],ecx
  424863:	8b 86 a4 05 00 00    	mov    eax,DWORD PTR [esi+0x5a4]
  424869:	c7 80 b0 05 00 00 26 	mov    DWORD PTR [eax+0x5b0],0x226
  424870:	02 00 00 
  424873:	e8 e7 cd 02 00       	call   0x45165f
  424878:	83 c4 04             	add    esp,0x4
  42487b:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  42487f:	3b c3                	cmp    eax,ebx
  424881:	c6 44 24 30 08       	mov    BYTE PTR [esp+0x30],0x8
  424886:	74 09                	je     0x424891
  424888:	8b c8                	mov    ecx,eax
  42488a:	e8 51 2c 00 00       	call   0x4274e0
  42488f:	eb 02                	jmp    0x424893
  424891:	33 c0                	xor    eax,eax
  424893:	b9 0e 01 00 00       	mov    ecx,0x10e
  424898:	89 86 ac 05 00 00    	mov    DWORD PTR [esi+0x5ac],eax
  42489e:	89 88        	mov    DWORD PTR [eax+0x594],ecx
