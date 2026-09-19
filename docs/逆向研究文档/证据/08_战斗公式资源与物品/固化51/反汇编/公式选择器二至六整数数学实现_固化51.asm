; 固化51：selector2..6五条公式机器实现。

/mnt/data/work50/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b630 <.text+0x2a630>:
  42b630:	53                   	push   ebx
  42b631:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b635:	56                   	push   esi
  42b636:	57                   	push   edi
  42b637:	8b f9                	mov    edi,ecx
  42b639:	53                   	push   ebx
  42b63a:	e8 41 04 00 00       	call   0x42ba80
  42b63f:	8b 4b 2c             	mov    ecx,DWORD PTR [ebx+0x2c]
  42b642:	8b 53 30             	mov    edx,DWORD PTR [ebx+0x30]
  42b645:	8b f0                	mov    esi,eax
  42b647:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  42b64b:	2b ca                	sub    ecx,edx
  42b64d:	8b 5b 3c             	mov    ebx,DWORD PTR [ebx+0x3c]
  42b650:	0f af 48 40          	imul   ecx,DWORD PTR [eax+0x40]
  42b654:	b8 67 66 66 66       	mov    eax,0x66666667
  42b659:	f7 e9                	imul   ecx
  42b65b:	c1 fa 02             	sar    edx,0x2
  42b65e:	8b ca                	mov    ecx,edx
  42b660:	b8 67 66 66 66       	mov    eax,0x66666667
  42b665:	c1 e9 1f             	shr    ecx,0x1f
  42b668:	03 d1                	add    edx,ecx
  42b66a:	03 f2                	add    esi,edx
  42b66c:	f7 eb                	imul   ebx
  42b66e:	c1 fa 02             	sar    edx,0x2
  42b671:	8b c2                	mov    eax,edx
  42b673:	c1 e8 1f             	shr    eax,0x1f
  42b676:	03 d0                	add    edx,eax
  42b678:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b67d:	52                   	push   edx
  42b67e:	f7 eb                	imul   ebx
  42b680:	c1 fa 04             	sar    edx,0x4
  42b683:	8b ca                	mov    ecx,edx
  42b685:	c1 e9 1f             	shr    ecx,0x1f
  42b688:	03 d1                	add    edx,ecx
  42b68a:	8b cf                	mov    ecx,edi
  42b68c:	52                   	push   edx
  42b68d:	e8 1e 16 00 00       	call   0x42ccb0
  42b692:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  42b696:	8b cf                	mov    ecx,edi
  42b698:	52                   	push   edx
  42b699:	03 f0                	add    esi,eax
  42b69b:	e8 b0 04 00 00       	call   0x42bb50
  42b6a0:	2b f0                	sub    esi,eax
  42b6a2:	b8 00 00 00 00       	mov    eax,0x0
  42b6a7:	0f 98 c0             	sets   al
  42b6aa:	48                   	dec    eax
  42b6ab:	5f                   	pop    edi
  42b6ac:	23 c6                	and    eax,esi
  42b6ae:	5e                   	pop    esi
  42b6af:	5b                   	pop    ebx
  42b6b0:	c2 0c 00             	ret    0xc
  42b6b3:	90                   	nop
  42b6b4:	90                   	nop
  42b6b5:	90                   	nop
  42b6b6:	90                   	nop
  42b6b7:	90                   	nop
  42b6b8:	90                   	nop
  42b6b9:	90                   	nop
  42b6ba:	90                   	nop
  42b6bb:	90                   	nop
  42b6bc:	90                   	nop
  42b6bd:	90                   	nop
  42b6be:	90                   	nop
  42b6bf:	90                   	nop
  42b6c0:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42b6c4:	53                   	push   ebx
  42b6c5:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b6c9:	55                   	push   ebp
  42b6ca:	56                   	push   esi
  42b6cb:	57                   	push   edi
  42b6cc:	8b f9                	mov    edi,ecx
  42b6ce:	53                   	push   ebx
  42b6cf:	8b 4b 2c             	mov    ecx,DWORD PTR [ebx+0x2c]
  42b6d2:	0f af 48 40          	imul   ecx,DWORD PTR [eax+0x40]
  42b6d6:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b6db:	f7 e9                	imul   ecx
  42b6dd:	c1 fa 05             	sar    edx,0x5
  42b6e0:	8b ca                	mov    ecx,edx
  42b6e2:	c1 e9 1f             	shr    ecx,0x1f
  42b6e5:	03 d1                	add    edx,ecx
  42b6e7:	8b cf                	mov    ecx,edi
  42b6e9:	8b ea                	mov    ebp,edx
  42b6eb:	e8 90 03 00 00       	call   0x42ba80
  42b6f0:	8b 5b 3c             	mov    ebx,DWORD PTR [ebx+0x3c]
  42b6f3:	8b f0                	mov    esi,eax
  42b6f5:	b8 67 66 66 66       	mov    eax,0x66666667
  42b6fa:	03 f5                	add    esi,ebp
  42b6fc:	f7 eb                	imul   ebx
  42b6fe:	c1 fa 02             	sar    edx,0x2
  42b701:	8b c2                	mov    eax,edx
  42b703:	c1 e8 1f             	shr    eax,0x1f
  42b706:	03 d0                	add    edx,eax
  42b708:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b70d:	52                   	push   edx
  42b70e:	f7 eb                	imul   ebx
  42b710:	c1 fa 04             	sar    edx,0x4
  42b713:	8b ca                	mov    ecx,edx
  42b715:	c1 e9 1f             	shr    ecx,0x1f
  42b718:	03 d1                	add    edx,ecx
  42b71a:	8b cf                	mov    ecx,edi
  42b71c:	52                   	push   edx
  42b71d:	e8 8e 15 00 00       	call   0x42ccb0
  42b722:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  42b726:	8b cf                	mov    ecx,edi
  42b728:	52                   	push   edx
  42b729:	03 f0                	add    esi,eax
  42b72b:	e8 20 04 00 00       	call   0x42bb50
  42b730:	2b f0                	sub    esi,eax
  42b732:	b8 00 00 00 00       	mov    eax,0x0
  42b737:	0f 98 c0             	sets   al
  42b73a:	48                   	dec    eax
  42b73b:	5f                   	pop    edi
  42b73c:	23 c6                	and    eax,esi
  42b73e:	5e                   	pop    esi
  42b73f:	5d                   	pop    ebp
  42b740:	5b                   	pop    ebx
  42b741:	c2 0c 00             	ret    0xc
  42b744:	90                   	nop
  42b745:	90                   	nop
  42b746:	90                   	nop
  42b747:	90                   	nop
  42b748:	90                   	nop
  42b749:	90                   	nop
  42b74a:	90                   	nop
  42b74b:	90                   	nop
  42b74c:	90                   	nop
  42b74d:	90                   	nop
  42b74e:	90                   	nop
  42b74f:	90                   	nop
  42b750:	53                   	push   ebx
  42b751:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b755:	56                   	push   esi
  42b756:	57                   	push   edi
  42b757:	8b f1                	mov    esi,ecx
  42b759:	53                   	push   ebx
  42b75a:	e8 21 03 00 00       	call   0x42ba80
  42b75f:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42b763:	8b 49 40             	mov    ecx,DWORD PTR [ecx+0x40]
  42b766:	0f af c8             	imul   ecx,eax
  42b769:	b8 67 66 66 66       	mov    eax,0x66666667
  42b76e:	f7 e9                	imul   ecx
  42b770:	8b 4b 3c             	mov    ecx,DWORD PTR [ebx+0x3c]
  42b773:	c1 fa 02             	sar    edx,0x2
  42b776:	8b c2                	mov    eax,edx
  42b778:	c1 e8 1f             	shr    eax,0x1f
  42b77b:	03 d0                	add    edx,eax
  42b77d:	b8 67 66 66 66       	mov    eax,0x66666667
  42b782:	8b fa                	mov    edi,edx
  42b784:	f7 e9                	imul   ecx
  42b786:	c1 fa 02             	sar    edx,0x2
  42b789:	8b c2                	mov    eax,edx
  42b78b:	c1 e8 1f             	shr    eax,0x1f
  42b78e:	03 d0                	add    edx,eax
  42b790:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b795:	52                   	push   edx
  42b796:	f7 e9                	imul   ecx
  42b798:	c1 fa 04             	sar    edx,0x4
  42b79b:	8b ca                	mov    ecx,edx
  42b79d:	c1 e9 1f             	shr    ecx,0x1f
  42b7a0:	03 d1                	add    edx,ecx
  42b7a2:	8b ce                	mov    ecx,esi
  42b7a4:	52                   	push   edx
  42b7a5:	e8 06 15 00 00       	call   0x42ccb0
  42b7aa:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  42b7ae:	8b ce                	mov    ecx,esi
  42b7b0:	52                   	push   edx
  42b7b1:	03 f8                	add    edi,eax
  42b7b3:	e8 98 03 00 00       	call   0x42bb50
  42b7b8:	2b f8                	sub    edi,eax
  42b7ba:	b8 00 00 00 00       	mov    eax,0x0
  42b7bf:	0f 98 c0             	sets   al
  42b7c2:	48                   	dec    eax
  42b7c3:	23 c7                	and    eax,edi
  42b7c5:	5f                   	pop    edi
  42b7c6:	5e                   	pop    esi
  42b7c7:	5b                   	pop    ebx
  42b7c8:	c2 0c 00             	ret    0xc
  42b7cb:	90                   	nop
  42b7cc:	90                   	nop
  42b7cd:	90                   	nop
  42b7ce:	90                   	nop
  42b7cf:	90                   	nop
  42b7d0:	53                   	push   ebx
  42b7d1:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b7d5:	56                   	push   esi
  42b7d6:	57                   	push   edi
  42b7d7:	8b f9                	mov    edi,ecx
  42b7d9:	53                   	push   ebx
  42b7da:	e8 a1 02 00 00       	call   0x42ba80
  42b7df:	8b 4b 3c             	mov    ecx,DWORD PTR [ebx+0x3c]
  42b7e2:	8b f0                	mov    esi,eax
  42b7e4:	b8 67 66 66 66       	mov    eax,0x66666667
  42b7e9:	f7 e9                	imul   ecx
  42b7eb:	c1 fa 02             	sar    edx,0x2
  42b7ee:	8b c2                	mov    eax,edx
  42b7f0:	c1 e8 1f             	shr    eax,0x1f
  42b7f3:	03 d0                	add    edx,eax
  42b7f5:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b7fa:	52                   	push   edx
  42b7fb:	f7 e9                	imul   ecx
  42b7fd:	c1 fa 04             	sar    edx,0x4
  42b800:	8b ca                	mov    ecx,edx
  42b802:	c1 e9 1f             	shr    ecx,0x1f
  42b805:	03 d1                	add    edx,ecx
  42b807:	8b cf                	mov    ecx,edi
  42b809:	52                   	push   edx
  42b80a:	e8 a1 14 00 00       	call   0x42ccb0
  42b80f:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  42b813:	8b cf                	mov    ecx,edi
  42b815:	52                   	push   edx
  42b816:	03 f0                	add    esi,eax
  42b818:	e8 33 03 00 00       	call   0x42bb50
  42b81d:	99                   	cdq
  42b81e:	f7 fe                	idiv   esi
  42b820:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42b824:	5f                   	pop    edi
  42b825:	0f af 41 40          	imul   eax,DWORD PTR [ecx+0x40]
  42b829:	2b f0                	sub    esi,eax
  42b82b:	b8 00 00 00 00       	mov    eax,0x0
  42b830:	0f 98 c0             	sets   al
  42b833:	48                   	dec    eax
  42b834:	23 c6                	and    eax,esi
  42b836:	5e                   	pop    esi
  42b837:	5b                   	pop    ebx
  42b838:	c2 0c 00             	ret    0xc
  42b83b:	90                   	nop
  42b83c:	90                   	nop
  42b83d:	90                   	nop
  42b83e:	90                   	nop
  42b83f:	90                   	nop
  42b840:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  42b844:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  42b848:	56                   	push   esi
  42b849:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  42b84d:	56                   	push   esi
  42b84e:	50                   	push   eax
  42b84f:	52                   	push   edx
  42b850:	e8 9b fd ff ff       	call   0x42b5f0
  42b855:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  42b858:	5e                   	pop    esi
  42b859:	0f af c8             	imul   ecx,eax
  42b85c:	b8 67 66 66 66       	mov    eax,0x66666667
  42b861:	f7 e9                	imul   ecx
  42b863:	c1 fa 02             	sar    edx,0x2
  42b866:	8b c2                	mov    eax,edx
  42b868:	c1 e8 1f             	shr    eax,0x1f
  42b86b:	03 d0                	add    edx,eax
  42b86d:	b8 00 00 00 00       	mov    eax,0x0
  42b872:	0f 98 c0             	sets   al
  42b875:	48                   	dec    eax
  42b876:	23 c2                	and    eax,edx
  42b878:	c2 0c 00             	ret    0xc
