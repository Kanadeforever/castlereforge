; 固化53：D5内部 A/B 派生量。
; A: RoleDefinition+0x3C（膂力）+ equipment Ail2+0x38，再受 raw3/raw22 状态倍率修正。
; B: trunc(RoleDefinition+0x40（體魄）/2)+ equipment Ail2+0x3C，再受 raw15/raw25/raw6 状态倍率修正。

/mnt/data/work53/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042ba00 <.text+0x2aa00>:
  42ba00:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  42ba04:	56                   	push   esi
  42ba05:	8b 35 c8 01 8b 00    	mov    esi,DWORD PTR ds:0x8b01c8
  42ba0b:	57                   	push   edi
  42ba0c:	8b 8a 90 00 00 00    	mov    ecx,DWORD PTR [edx+0x90]
  42ba12:	8b 42 3c             	mov    eax,DWORD PTR [edx+0x3c]
  42ba15:	85 c9                	test   ecx,ecx
  42ba17:	7e 15                	jle    0x42ba2e
  42ba19:	8d 3c cd 00 00 00 00 	lea    edi,[ecx*8+0x0]
  42ba20:	2b f9                	sub    edi,ecx
  42ba22:	8d 3c b9             	lea    edi,[ecx+edi*4]
  42ba25:	c1 e7 04             	shl    edi,0x4
  42ba28:	2b f9                	sub    edi,ecx
  42ba2a:	03 44 7e 38          	add    eax,DWORD PTR [esi+edi*2+0x38]
  42ba2e:	8b 8a 94 00 00 00    	mov    ecx,DWORD PTR [edx+0x94]
  42ba34:	85 c9                	test   ecx,ecx
  42ba36:	7e 15                	jle    0x42ba4d
  42ba38:	8d 3c cd 00 00 00 00 	lea    edi,[ecx*8+0x0]
  42ba3f:	2b f9                	sub    edi,ecx
  42ba41:	8d 3c b9             	lea    edi,[ecx+edi*4]
  42ba44:	c1 e7 04             	shl    edi,0x4
  42ba47:	2b f9                	sub    edi,ecx
  42ba49:	03 44 7e 38          	add    eax,DWORD PTR [esi+edi*2+0x38]
  42ba4d:	8b 8a 98 00 00 00    	mov    ecx,DWORD PTR [edx+0x98]
  42ba53:	85 c9                	test   ecx,ecx
  42ba55:	7e 15                	jle    0x42ba6c
  42ba57:	8d 14 cd 00 00 00 00 	lea    edx,[ecx*8+0x0]
  42ba5e:	2b d1                	sub    edx,ecx
  42ba60:	8d 14 91             	lea    edx,[ecx+edx*4]
  42ba63:	c1 e2 04             	shl    edx,0x4
  42ba66:	2b d1                	sub    edx,ecx
  42ba68:	03 44 56 38          	add    eax,DWORD PTR [esi+edx*2+0x38]
  42ba6c:	5f                   	pop    edi
  42ba6d:	5e                   	pop    esi
  42ba6e:	c2 04 00             	ret    0x4
  42ba71:	90                   	nop
  42ba72:	90                   	nop
  42ba73:	90                   	nop
  42ba74:	90                   	nop
  42ba75:	90                   	nop
  42ba76:	90                   	nop
  42ba77:	90                   	nop
  42ba78:	90                   	nop
  42ba79:	90                   	nop
  42ba7a:	90                   	nop
  42ba7b:	90                   	nop
  42ba7c:	90                   	nop
  42ba7d:	90                   	nop
  42ba7e:	90                   	nop
  42ba7f:	90                   	nop
  42ba80:	56                   	push   esi
  42ba81:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  42ba85:	56                   	push   esi
  42ba86:	e8 75 ff ff ff       	call   0x42ba00
  42ba8b:	8b 8e f4 01 00 00    	mov    ecx,DWORD PTR [esi+0x1f4]
  42ba91:	85 c9                	test   ecx,ecx
  42ba93:	7e 0b                	jle    0x42baa0
  42ba95:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  42ba9c:	2b c8                	sub    ecx,eax
  42ba9e:	eb 10                	jmp    0x42bab0
  42baa0:	8b 8e 40 02 00 00    	mov    ecx,DWORD PTR [esi+0x240]
  42baa6:	85 c9                	test   ecx,ecx
  42baa8:	7e 19                	jle    0x42bac3
  42baaa:	8d 0c 40             	lea    ecx,[eax+eax*2]
  42baad:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  42bab0:	b8 67 66 66 66       	mov    eax,0x66666667
  42bab5:	f7 e9                	imul   ecx
  42bab7:	c1 fa 02             	sar    edx,0x2
  42baba:	8b c2                	mov    eax,edx
  42babc:	c1 e8 1f             	shr    eax,0x1f
  42babf:	03 d0                	add    edx,eax
  42bac1:	8b c2                	mov    eax,edx
  42bac3:	5e                   	pop    esi
  42bac4:	c2 04 00             	ret    0x4
  42bac7:	90                   	nop
  42bac8:	90                   	nop
  42bac9:	90                   	nop
  42baca:	90                   	nop
  42bacb:	90                   	nop
  42bacc:	90                   	nop
  42bacd:	90                   	nop
  42bace:	90                   	nop
  42bacf:	90                   	nop
  42bad0:	56                   	push   esi
  42bad1:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  42bad5:	57                   	push   edi
  42bad6:	8b 46 40             	mov    eax,DWORD PTR [esi+0x40]
  42bad9:	8b 8e 90 00 00 00    	mov    ecx,DWORD PTR [esi+0x90]
  42badf:	99                   	cdq
  42bae0:	2b c2                	sub    eax,edx
  42bae2:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42bae8:	d1 f8                	sar    eax,1
  42baea:	85 c9                	test   ecx,ecx
  42baec:	7e 15                	jle    0x42bb03
  42baee:	8d 3c cd 00 00 00 00 	lea    edi,[ecx*8+0x0]
  42baf5:	2b f9                	sub    edi,ecx
  42baf7:	8d 3c b9             	lea    edi,[ecx+edi*4]
  42bafa:	c1 e7 04             	shl    edi,0x4
  42bafd:	2b f9                	sub    edi,ecx
  42baff:	03 44 7a 3c          	add    eax,DWORD PTR [edx+edi*2+0x3c]
  42bb03:	8b 8e 94 00 00 00    	mov    ecx,DWORD PTR [esi+0x94]
  42bb09:	85 c9                	test   ecx,ecx
  42bb0b:	7e 15                	jle    0x42bb22
  42bb0d:	8d 3c cd 00 00 00 00 	lea    edi,[ecx*8+0x0]
  42bb14:	2b f9                	sub    edi,ecx
  42bb16:	8d 3c b9             	lea    edi,[ecx+edi*4]
  42bb19:	c1 e7 04             	shl    edi,0x4
  42bb1c:	2b f9                	sub    edi,ecx
  42bb1e:	03 44 7a 3c          	add    eax,DWORD PTR [edx+edi*2+0x3c]
  42bb22:	8b 8e 98 00 00 00    	mov    ecx,DWORD PTR [esi+0x98]
  42bb28:	85 c9                	test   ecx,ecx
  42bb2a:	7e 15                	jle    0x42bb41
  42bb2c:	8d 34 cd 00 00 00 00 	lea    esi,[ecx*8+0x0]
  42bb33:	2b f1                	sub    esi,ecx
  42bb35:	8d 34 b1             	lea    esi,[ecx+esi*4]
  42bb38:	c1 e6 04             	shl    esi,0x4
  42bb3b:	2b f1                	sub    esi,ecx
  42bb3d:	03 44 72 3c          	add    eax,DWORD PTR [edx+esi*2+0x3c]
  42bb41:	5f                   	pop    edi
  42bb42:	5e                   	pop    esi
  42bb43:	c2 04 00             	ret    0x4
  42bb46:	90                   	nop
  42bb47:	90                   	nop
  42bb48:	90                   	nop
  42bb49:	90                   	nop
  42bb4a:	90                   	nop
  42bb4b:	90                   	nop
  42bb4c:	90                   	nop
  42bb4d:	90                   	nop
  42bb4e:	90                   	nop
  42bb4f:	90                   	nop
  42bb50:	56                   	push   esi
  42bb51:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  42bb55:	56                   	push   esi
  42bb56:	e8 75 ff ff ff       	call   0x42bad0
  42bb5b:	8b 8e 24 02 00 00    	mov    ecx,DWORD PTR [esi+0x224]
  42bb61:	85 c9                	test   ecx,ecx
  42bb63:	7e 09                	jle    0x42bb6e
  42bb65:	99                   	cdq
  42bb66:	2b c2                	sub    eax,edx
  42bb68:	5e                   	pop    esi
  42bb69:	d1 f8                	sar    eax,1
  42bb6b:	c2 04 00             	ret    0x4
  42bb6e:	8b 8e 4c 02 00 00    	mov    ecx,DWORD PTR [esi+0x24c]
  42bb74:	85 c9                	test   ecx,ecx
  42bb76:	7e 1d                	jle    0x42bb95
  42bb78:	8d 0c 40             	lea    ecx,[eax+eax*2]
  42bb7b:	5e                   	pop    esi
  42bb7c:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  42bb7f:	b8 67 66 66 66       	mov    eax,0x66666667
  42bb84:	f7 e9                	imul   ecx
  42bb86:	c1 fa 02             	sar    edx,0x2
  42bb89:	8b c2                	mov    eax,edx
  42bb8b:	c1 e8 1f             	shr    eax,0x1f
  42bb8e:	03 d0                	add    edx,eax
  42bb90:	8b c2                	mov    eax,edx
  42bb92:	c2 04 00             	ret    0x4
  42bb95:	8b 8e 00 02 00 00    	mov    ecx,DWORD PTR [esi+0x200]
  42bb9b:	85 c9                	test   ecx,ecx
  42bb9d:	7e 1c                	jle    0x42bbbb
  42bb9f:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  42bba6:	2b c8                	sub    ecx,eax
  42bba8:	b8 67 66 66 66       	mov    eax,0x66666667
  42bbad:	f7 e9                	imul   ecx
  42bbaf:	c1 fa 02             	sar    edx,0x2
  42bbb2:	8b ca                	mov    ecx,edx
  42bbb4:	c1 e9 1f             	shr    ecx,0x1f
  42bbb7:	03 d1                	add    edx,ecx
  42bbb9:	8b c2                	mov    eax,edx
  42bbbb:	5e                   	pop    esi
  42bbbc:	c2 04 00             	ret    0x4
  42bbbf:	90                   	nop

; ---- formula helpers calling A/B ----

/mnt/data/work53/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b5a0 <.text+0x2a5a0>:
  42b5a0:	53                   	push   ebx
  42b5a1:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b5a5:	56                   	push   esi
  42b5a6:	57                   	push   edi
  42b5a7:	8b f1                	mov    esi,ecx
  42b5a9:	53                   	push   ebx
  42b5aa:	e8 d1 04 00 00       	call   0x42ba80
  42b5af:	8b f8                	mov    edi,eax
  42b5b1:	8a 44 24 18          	mov    al,BYTE PTR [esp+0x18]
  42b5b5:	84 c0                	test   al,al
  42b5b7:	74 02                	je     0x42b5bb
  42b5b9:	03 ff                	add    edi,edi
  42b5bb:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42b5bf:	8b ce                	mov    ecx,esi
  42b5c1:	50                   	push   eax
  42b5c2:	e8 89 05 00 00       	call   0x42bb50
  42b5c7:	50                   	push   eax
  42b5c8:	57                   	push   edi
  42b5c9:	53                   	push   ebx
  42b5ca:	8b ce                	mov    ecx,esi
  42b5cc:	e8 af 06 00 00       	call   0x42bc80
  42b5d1:	33 c9                	xor    ecx,ecx
  42b5d3:	5f                   	pop    edi
  42b5d4:	85 c0                	test   eax,eax
  42b5d6:	0f 9c c1             	setl   cl
  42b5d9:	49                   	dec    ecx
  42b5da:	5e                   	pop    esi
  42b5db:	23 c1                	and    eax,ecx
  42b5dd:	5b                   	pop    ebx
  42b5de:	c2 0c 00             	ret    0xc
  42b5e1:	90                   	nop
  42b5e2:	90                   	nop
  42b5e3:	90                   	nop
  42b5e4:	90                   	nop
  42b5e5:	90                   	nop
  42b5e6:	90                   	nop
  42b5e7:	90                   	nop
  42b5e8:	90                   	nop
  42b5e9:	90                   	nop
  42b5ea:	90                   	nop
  42b5eb:	90                   	nop
  42b5ec:	90                   	nop
  42b5ed:	90                   	nop
  42b5ee:	90                   	nop
  42b5ef:	90                   	nop
  42b5f0:	53                   	push   ebx
  42b5f1:	56                   	push   esi
  42b5f2:	57                   	push   edi
  42b5f3:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  42b5f7:	8b f1                	mov    esi,ecx
  42b5f9:	57                   	push   edi
  42b5fa:	e8 81 04 00 00       	call   0x42ba80
  42b5ff:	8b d8                	mov    ebx,eax
  42b601:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42b605:	50                   	push   eax
  42b606:	8b ce                	mov    ecx,esi
  42b608:	e8 43 05 00 00       	call   0x42bb50
  42b60d:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42b611:	51                   	push   ecx
  42b612:	50                   	push   eax
  42b613:	53                   	push   ebx
  42b614:	57                   	push   edi
  42b615:	8b ce                	mov    ecx,esi
  42b617:	e8 b4 06 00 00       	call   0x42bcd0
  42b61c:	33 d2                	xor    edx,edx
  42b61e:	5f                   	pop    edi
  42b61f:	85 c0                	test   eax,eax
  42b621:	0f 9c c2             	setl   dl
  42b624:	4a                   	dec    edx
  42b625:	5e                   	pop    esi
  42b626:	23 c2                	and    eax,edx
  42b628:	5b                   	pop    ebx
  42b629:	c2 0c 00             	ret    0xc
  42b62c:	90                   	nop
  42b62d:	90                   	nop
  42b62e:	90                   	nop
  42b62f:	90                   	nop
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
