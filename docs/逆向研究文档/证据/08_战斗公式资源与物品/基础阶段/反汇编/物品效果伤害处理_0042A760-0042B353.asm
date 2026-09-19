
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042a760 <.text+0x29760>:
  42a760:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  42a764:	85 d2                	test   edx,edx
  42a766:	74 26                	je     0x42a78e
  42a768:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  42a76c:	85 c9                	test   ecx,ecx
  42a76e:	74 1e                	je     0x42a78e
  42a770:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42a774:	85 c0                	test   eax,eax
  42a776:	74 16                	je     0x42a78e
  42a778:	a3 d0 01 8b 00       	mov    ds:0x8b01d0,eax
  42a77d:	89 15 c8 01 8b 00    	mov    DWORD PTR ds:0x8b01c8,edx
  42a783:	89 0d cc 01 8b 00    	mov    DWORD PTR ds:0x8b01cc,ecx
  42a789:	b0 01                	mov    al,0x1
  42a78b:	c2 0c 00             	ret    0xc
  42a78e:	32 c0                	xor    al,al
  42a790:	c2 0c 00             	ret    0xc
  42a793:	90                   	nop
  42a794:	90                   	nop
  42a795:	90                   	nop
  42a796:	90                   	nop
  42a797:	90                   	nop
  42a798:	90                   	nop
  42a799:	90                   	nop
  42a79a:	90                   	nop
  42a79b:	90                   	nop
  42a79c:	90                   	nop
  42a79d:	90                   	nop
  42a79e:	90                   	nop
  42a79f:	90                   	nop
  42a7a0:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42a7a4:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  42a7a8:	50                   	push   eax
  42a7a9:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42a7ad:	52                   	push   edx
  42a7ae:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  42a7b2:	50                   	push   eax
  42a7b3:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42a7b7:	52                   	push   edx
  42a7b8:	50                   	push   eax
  42a7b9:	e8 c2 1e 00 00       	call   0x42c680
  42a7be:	c2 14 00             	ret    0x14
  42a7c1:	90                   	nop
  42a7c2:	90                   	nop
  42a7c3:	90                   	nop
  42a7c4:	90                   	nop
  42a7c5:	90                   	nop
  42a7c6:	90                   	nop
  42a7c7:	90                   	nop
  42a7c8:	90                   	nop
  42a7c9:	90                   	nop
  42a7ca:	90                   	nop
  42a7cb:	90                   	nop
  42a7cc:	90                   	nop
  42a7cd:	90                   	nop
  42a7ce:	90                   	nop
  42a7cf:	90                   	nop
  42a7d0:	83 ec 10             	sub    esp,0x10
  42a7d3:	53                   	push   ebx
  42a7d4:	55                   	push   ebp
  42a7d5:	56                   	push   esi
  42a7d6:	57                   	push   edi
  42a7d7:	8b 7c 24 2c          	mov    edi,DWORD PTR [esp+0x2c]
  42a7db:	8b e9                	mov    ebp,ecx
  42a7dd:	33 c0                	xor    eax,eax
  42a7df:	33 c9                	xor    ecx,ecx
  42a7e1:	8d 57 10             	lea    edx,[edi+0x10]
  42a7e4:	c6 07 00             	mov    BYTE PTR [edi],0x0
  42a7e7:	89 47 04             	mov    DWORD PTR [edi+0x4],eax
  42a7ea:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  42a7ed:	88 47 0c             	mov    BYTE PTR [edi+0xc],al
  42a7f0:	89 0a                	mov    DWORD PTR [edx],ecx
  42a7f2:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  42a7f6:	89 4a 04             	mov    DWORD PTR [edx+0x4],ecx
  42a7f9:	89 4a 08             	mov    DWORD PTR [edx+0x8],ecx
  42a7fc:	89 4a 0c             	mov    DWORD PTR [edx+0xc],ecx
  42a7ff:	89 4a 10             	mov    DWORD PTR [edx+0x10],ecx
  42a802:	8d 57 24             	lea    edx,[edi+0x24]
  42a805:	89 4f 24             	mov    DWORD PTR [edi+0x24],ecx
  42a808:	89 4a 04             	mov    DWORD PTR [edx+0x4],ecx
  42a80b:	89 4a 08             	mov    DWORD PTR [edx+0x8],ecx
  42a80e:	89 4a 0c             	mov    DWORD PTR [edx+0xc],ecx
  42a811:	89 4a 10             	mov    DWORD PTR [edx+0x10],ecx
  42a814:	88 4f 40             	mov    BYTE PTR [edi+0x40],cl
  42a817:	88 4f 48             	mov    BYTE PTR [edi+0x48],cl
  42a81a:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42a81e:	3b c8                	cmp    ecx,eax
  42a820:	89 47 38             	mov    DWORD PTR [edi+0x38],eax
  42a823:	89 47 3c             	mov    DWORD PTR [edi+0x3c],eax
  42a826:	c7 47 44 01 00 00 00 	mov    DWORD PTR [edi+0x44],0x1
  42a82d:	c7 47 4c ff ff ff ff 	mov    DWORD PTR [edi+0x4c],0xffffffff
  42a834:	75 1f                	jne    0x42a855
  42a836:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42a83c:	68 30 a8 46 00       	push   0x46a830
  42a841:	68 0c a8 46 00       	push   0x46a80c
  42a846:	e8 05 78 00 00       	call   0x432050
  42a84b:	5f                   	pop    edi
  42a84c:	5e                   	pop    esi
  42a84d:	5d                   	pop    ebp
  42a84e:	5b                   	pop    ebx
  42a84f:	83 c4 10             	add    esp,0x10
  42a852:	c2 14 00             	ret    0x14
  42a855:	8b 74 24 28          	mov    esi,DWORD PTR [esp+0x28]
  42a859:	3b f0                	cmp    esi,eax
  42a85b:	75 1f                	jne    0x42a87c
  42a85d:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42a863:	68 30 a8 46 00       	push   0x46a830
  42a868:	68 e8 a7 46 00       	push   0x46a7e8
  42a86d:	e8 de 77 00 00       	call   0x432050
  42a872:	5f                   	pop    edi
  42a873:	5e                   	pop    esi
  42a874:	5d                   	pop    ebp
  42a875:	5b                   	pop    ebx
  42a876:	83 c4 10             	add    esp,0x10
  42a879:	c2 14 00             	ret    0x14
  42a87c:	3b f8                	cmp    edi,eax
  42a87e:	75 1f                	jne    0x42a89f
  42a880:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42a886:	68 30 a8 46 00       	push   0x46a830
  42a88b:	68 c4 a7 46 00       	push   0x46a7c4
  42a890:	e8 bb 77 00 00       	call   0x432050
  42a895:	5f                   	pop    edi
  42a896:	5e                   	pop    esi
  42a897:	5d                   	pop    ebp
  42a898:	5b                   	pop    ebx
  42a899:	83 c4 10             	add    esp,0x10
  42a89c:	c2 14 00             	ret    0x14
  42a89f:	8b 5c 24 30          	mov    ebx,DWORD PTR [esp+0x30]
  42a8a3:	3b d8                	cmp    ebx,eax
  42a8a5:	75 1f                	jne    0x42a8c6
  42a8a7:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42a8ad:	68 30 a8 46 00       	push   0x46a830
  42a8b2:	68 a0 a7 46 00       	push   0x46a7a0
  42a8b7:	e8 94 77 00 00       	call   0x432050
  42a8bc:	5f                   	pop    edi
  42a8bd:	5e                   	pop    esi
  42a8be:	5d                   	pop    ebp
  42a8bf:	5b                   	pop    ebx
  42a8c0:	83 c4 10             	add    esp,0x10
  42a8c3:	c2 14 00             	ret    0x14
  42a8c6:	56                   	push   esi
  42a8c7:	8b cd                	mov    ecx,ebp
  42a8c9:	89 44 24 2c          	mov    DWORD PTR [esp+0x2c],eax
  42a8cd:	e8 fe 11 00 00       	call   0x42bad0
  42a8d2:	8b 8b c4 00 00 00    	mov    ecx,DWORD PTR [ebx+0xc4]
  42a8d8:	2b c8                	sub    ecx,eax
  42a8da:	8a 44 24 34          	mov    al,BYTE PTR [esp+0x34]
  42a8de:	84 c0                	test   al,al
  42a8e0:	89 4c 24 2c          	mov    DWORD PTR [esp+0x2c],ecx
  42a8e4:	74 0d                	je     0x42a8f3
  42a8e6:	8b c1                	mov    eax,ecx
  42a8e8:	99                   	cdq
  42a8e9:	2b c2                	sub    eax,edx
  42a8eb:	d1 f8                	sar    eax,1
  42a8ed:	8b c8                	mov    ecx,eax
  42a8ef:	89 4c 24 2c          	mov    DWORD PTR [esp+0x2c],ecx
  42a8f3:	85 c9                	test   ecx,ecx
  42a8f5:	7d 08                	jge    0x42a8ff
  42a8f7:	c7 44 24 2c 00 00 00 	mov    DWORD PTR [esp+0x2c],0x0
  42a8fe:	00 
  42a8ff:	c6 07 01             	mov    BYTE PTR [edi],0x1
  42a902:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  42a905:	8b 83 cc 00 00 00    	mov    eax,DWORD PTR [ebx+0xcc]
  42a90b:	6a 03                	push   0x3
  42a90d:	51                   	push   ecx
  42a90e:	8b cd                	mov    ecx,ebp
  42a910:	c6 44 24 3c 00       	mov    BYTE PTR [esp+0x3c],0x0
  42a915:	89 44 24 38          	mov    DWORD PTR [esp+0x38],eax
  42a919:	e8 22 16 00 00       	call   0x42bf40
  42a91e:	84 c0                	test   al,al
  42a920:	bd 64 00 00 00       	mov    ebp,0x64
  42a925:	75 2b                	jne    0x42a952
  42a927:	8b 86 20 02 00 00    	mov    eax,DWORD PTR [esi+0x220]
  42a92d:	85 c0                	test   eax,eax
  42a92f:	7e 07                	jle    0x42a938
  42a931:	c6 44 24 34 00       	mov    BYTE PTR [esp+0x34],0x0
  42a936:	eb 1f                	jmp    0x42a957
  42a938:	8b 86 24 02 00 00    	mov    eax,DWORD PTR [esi+0x224]
  42a93e:	85 c0                	test   eax,eax
  42a940:	7f 10                	jg     0x42a952
  42a942:	e8 1e 6c 02 00       	call   0x451565
  42a947:	99                   	cdq
  42a948:	8b cd                	mov    ecx,ebp
  42a94a:	f7 f9                	idiv   ecx
  42a94c:	3b 54 24 30          	cmp    edx,DWORD PTR [esp+0x30]
  42a950:	7d 05                	jge    0x42a957
  42a952:	c6 44 24 34 01       	mov    BYTE PTR [esp+0x34],0x1
  42a957:	8b 83 c8 00 00 00    	mov    eax,DWORD PTR [ebx+0xc8]
  42a95d:	89 6c 24 30          	mov    DWORD PTR [esp+0x30],ebp
  42a961:	89 6c 24 10          	mov    DWORD PTR [esp+0x10],ebp
  42a965:	89 6c 24 14          	mov    DWORD PTR [esp+0x14],ebp
  42a969:	8b ab d4 00 00 00    	mov    ebp,DWORD PTR [ebx+0xd4]
  42a96f:	85 c0                	test   eax,eax
  42a971:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  42a975:	0f 85 41 02 00 00    	jne    0x42abbc
  42a97b:	8a 4c 24 34          	mov    cl,BYTE PTR [esp+0x34]
  42a97f:	84 c9                	test   cl,cl
  42a981:	0f 84 35 02 00 00    	je     0x42abbc
  42a987:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42a98b:	56                   	push   esi
  42a98c:	e8 5f 23 00 00       	call   0x42ccf0
  42a991:	8b 8b d0 00 00 00    	mov    ecx,DWORD PTR [ebx+0xd0]
  42a997:	2b c8                	sub    ecx,eax
  42a999:	89 4c 24 34          	mov    DWORD PTR [esp+0x34],ecx
  42a99d:	79 06                	jns    0x42a9a5
  42a99f:	33 c9                	xor    ecx,ecx
  42a9a1:	89 4c 24 34          	mov    DWORD PTR [esp+0x34],ecx
  42a9a5:	83 fd 01             	cmp    ebp,0x1
  42a9a8:	75 20                	jne    0x42a9ca
  42a9aa:	8b 86 f0 01 00 00    	mov    eax,DWORD PTR [esi+0x1f0]
  42a9b0:	85 c0                	test   eax,eax
  42a9b2:	7e 3d                	jle    0x42a9f1
  42a9b4:	c1 e1 03             	shl    ecx,0x3
  42a9b7:	b8 67 66 66 66       	mov    eax,0x66666667
  42a9bc:	f7 e9                	imul   ecx
  42a9be:	c1 fa 02             	sar    edx,0x2
  42a9c1:	8b c2                	mov    eax,edx
  42a9c3:	c1 e8 1f             	shr    eax,0x1f
  42a9c6:	03 d0                	add    edx,eax
  42a9c8:	eb 23                	jmp    0x42a9ed
  42a9ca:	83 fd 02             	cmp    ebp,0x2
  42a9cd:	75 22                	jne    0x42a9f1
  42a9cf:	8b 86 ec 01 00 00    	mov    eax,DWORD PTR [esi+0x1ec]
  42a9d5:	85 c0                	test   eax,eax
  42a9d7:	7e 18                	jle    0x42a9f1
  42a9d9:	c1 e1 03             	shl    ecx,0x3
  42a9dc:	b8 67 66 66 66       	mov    eax,0x66666667
  42a9e1:	f7 e9                	imul   ecx
  42a9e3:	c1 fa 02             	sar    edx,0x2
  42a9e6:	8b ca                	mov    ecx,edx
  42a9e8:	c1 e9 1f             	shr    ecx,0x1f
  42a9eb:	03 d1                	add    edx,ecx
  42a9ed:	89 54 24 34          	mov    DWORD PTR [esp+0x34],edx
  42a9f1:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  42a9f4:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42a9f8:	6a 04                	push   0x4
  42a9fa:	52                   	push   edx
  42a9fb:	e8 40 15 00 00       	call   0x42bf40
  42aa00:	84 c0                	test   al,al
  42aa02:	74 0d                	je     0x42aa11
  42aa04:	83 fd 04             	cmp    ebp,0x4
  42aa07:	75 08                	jne    0x42aa11
  42aa09:	c7 44 24 34 00 00 00 	mov    DWORD PTR [esp+0x34],0x0
  42aa10:	00 
  42aa11:	8b 44 ae 64          	mov    eax,DWORD PTR [esi+ebp*4+0x64]
  42aa15:	8b 0d b0 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b0
  42aa1b:	99                   	cdq
  42aa1c:	33 c2                	xor    eax,edx
  42aa1e:	2b c2                	sub    eax,edx
  42aa20:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  42aa24:	8b 86 90 00 00 00    	mov    eax,DWORD PTR [esi+0x90]
  42aa2a:	85 c0                	test   eax,eax
  42aa2c:	7e 1c                	jle    0x42aa4a
  42aa2e:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42aa35:	2b d0                	sub    edx,eax
  42aa37:	8d 14 90             	lea    edx,[eax+edx*4]
  42aa3a:	c1 e2 04             	shl    edx,0x4
  42aa3d:	2b d0                	sub    edx,eax
  42aa3f:	8d 04 51             	lea    eax,[ecx+edx*2]
  42aa42:	8b 54 a8 74          	mov    edx,DWORD PTR [eax+ebp*4+0x74]
  42aa46:	89 54 24 30          	mov    DWORD PTR [esp+0x30],edx
  42aa4a:	8b 86 94 00 00 00    	mov    eax,DWORD PTR [esi+0x94]
  42aa50:	85 c0                	test   eax,eax
  42aa52:	7e 1c                	jle    0x42aa70
  42aa54:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42aa5b:	2b d0                	sub    edx,eax
  42aa5d:	8d 14 90             	lea    edx,[eax+edx*4]
  42aa60:	c1 e2 04             	shl    edx,0x4
  42aa63:	2b d0                	sub    edx,eax
  42aa65:	8d 04 51             	lea    eax,[ecx+edx*2]
  42aa68:	8b 54 a8 74          	mov    edx,DWORD PTR [eax+ebp*4+0x74]
  42aa6c:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  42aa70:	8b 86 98 00 00 00    	mov    eax,DWORD PTR [esi+0x98]
  42aa76:	85 c0                	test   eax,eax
  42aa78:	7e 1c                	jle    0x42aa96
  42aa7a:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42aa81:	2b d0                	sub    edx,eax
  42aa83:	8d 14 90             	lea    edx,[eax+edx*4]
  42aa86:	c1 e2 04             	shl    edx,0x4
  42aa89:	2b d0                	sub    edx,eax
  42aa8b:	8d 04 51             	lea    eax,[ecx+edx*2]
  42aa8e:	8b 4c a8 74          	mov    ecx,DWORD PTR [eax+ebp*4+0x74]
  42aa92:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  42aa96:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42aa9a:	99                   	cdq
  42aa9b:	8b c8                	mov    ecx,eax
  42aa9d:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42aaa1:	33 ca                	xor    ecx,edx
  42aaa3:	2b ca                	sub    ecx,edx
  42aaa5:	99                   	cdq
  42aaa6:	33 c2                	xor    eax,edx
  42aaa8:	2b c2                	sub    eax,edx
  42aaaa:	0f af c8             	imul   ecx,eax
  42aaad:	8b 44 24 30          	mov    eax,DWORD PTR [esp+0x30]
  42aab1:	99                   	cdq
  42aab2:	33 c2                	xor    eax,edx
  42aab4:	2b c2                	sub    eax,edx
  42aab6:	0f af c8             	imul   ecx,eax
  42aab9:	0f af 4c 24 28       	imul   ecx,DWORD PTR [esp+0x28]
  42aabe:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42aac3:	f7 e9                	imul   ecx
  42aac5:	c1 fa 05             	sar    edx,0x5
  42aac8:	8b c2                	mov    eax,edx
  42aaca:	c1 e8 1f             	shr    eax,0x1f
  42aacd:	03 d0                	add    edx,eax
  42aacf:	89 54 24 28          	mov    DWORD PTR [esp+0x28],edx
  42aad3:	e8 8d 6a 02 00       	call   0x451565
  42aad8:	99                   	cdq
  42aad9:	b9 09 00 00 00       	mov    ecx,0x9
  42aade:	8b 6c 24 2c          	mov    ebp,DWORD PTR [esp+0x2c]
  42aae2:	f7 f9                	idiv   ecx
  42aae4:	8b c5                	mov    eax,ebp
  42aae6:	8b ca                	mov    ecx,edx
  42aae8:	99                   	cdq
  42aae9:	41                   	inc    ecx
  42aaea:	f7 f9                	idiv   ecx
  42aaec:	03 e8                	add    ebp,eax
  42aaee:	e8 72 6a 02 00       	call   0x451565
  42aaf3:	99                   	cdq
  42aaf4:	b9 13 00 00 00       	mov    ecx,0x13
  42aaf9:	f7 f9                	idiv   ecx
  42aafb:	8b 46 40             	mov    eax,DWORD PTR [esi+0x40]
  42aafe:	8b ca                	mov    ecx,edx
  42ab00:	99                   	cdq
  42ab01:	41                   	inc    ecx
  42ab02:	f7 f9                	idiv   ecx
  42ab04:	2b e8                	sub    ebp,eax
  42ab06:	89 6c 24 2c          	mov    DWORD PTR [esp+0x2c],ebp
  42ab0a:	79 08                	jns    0x42ab14
  42ab0c:	c7 44 24 2c 00 00 00 	mov    DWORD PTR [esp+0x2c],0x0
  42ab13:	00 
  42ab14:	e8 4c 6a 02 00       	call   0x451565
  42ab19:	99                   	cdq
  42ab1a:	b9 09 00 00 00       	mov    ecx,0x9
  42ab1f:	f7 f9                	idiv   ecx
  42ab21:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42ab25:	8b ca                	mov    ecx,edx
  42ab27:	99                   	cdq
  42ab28:	41                   	inc    ecx
  42ab29:	f7 f9                	idiv   ecx
  42ab2b:	8b e8                	mov    ebp,eax
  42ab2d:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42ab31:	03 e8                	add    ebp,eax
  42ab33:	e8 2d 6a 02 00       	call   0x451565
  42ab38:	99                   	cdq
  42ab39:	b9 13 00 00 00       	mov    ecx,0x13
  42ab3e:	f7 f9                	idiv   ecx
  42ab40:	8b 46 4c             	mov    eax,DWORD PTR [esi+0x4c]
  42ab43:	8b ca                	mov    ecx,edx
  42ab45:	99                   	cdq
  42ab46:	41                   	inc    ecx
  42ab47:	f7 f9                	idiv   ecx
  42ab49:	2b e8                	sub    ebp,eax
  42ab4b:	89 6c 24 34          	mov    DWORD PTR [esp+0x34],ebp
  42ab4f:	db 44 24 34          	fild   DWORD PTR [esp+0x34]
  42ab53:	da 4c 24 28          	fimul  DWORD PTR [esp+0x28]
  42ab57:	e8 3c 72 02 00       	call   0x451d98
  42ab5c:	8b c8                	mov    ecx,eax
  42ab5e:	b8 83 de 1b 43       	mov    eax,0x431bde83
  42ab63:	f7 e9                	imul   ecx
  42ab65:	c1 fa 12             	sar    edx,0x12
  42ab68:	8b c2                	mov    eax,edx
  42ab6a:	c1 e8 1f             	shr    eax,0x1f
  42ab6d:	03 d0                	add    edx,eax
  42ab6f:	89 54 24 28          	mov    DWORD PTR [esp+0x28],edx
  42ab73:	79 08                	jns    0x42ab7d
  42ab75:	c7 44 24 28 00 00 00 	mov    DWORD PTR [esp+0x28],0x0
  42ab7c:	00 
  42ab7d:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  42ab81:	85 c0                	test   eax,eax
  42ab83:	0f 8e e9 03 00 00    	jle    0x42af72
  42ab89:	8b 4c 86 64          	mov    ecx,DWORD PTR [esi+eax*4+0x64]
  42ab8d:	85 c9                	test   ecx,ecx
  42ab8f:	7c 1c                	jl     0x42abad
  42ab91:	8b 44 24 30          	mov    eax,DWORD PTR [esp+0x30]
  42ab95:	85 c0                	test   eax,eax
  42ab97:	7c 14                	jl     0x42abad
  42ab99:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42ab9d:	85 c0                	test   eax,eax
  42ab9f:	7c 0c                	jl     0x42abad
  42aba1:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42aba5:	85 c0                	test   eax,eax
  42aba7:	0f 8d c5 03 00 00    	jge    0x42af72
  42abad:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  42abb1:	f7 d9                	neg    ecx
  42abb3:	89 4c 24 28          	mov    DWORD PTR [esp+0x28],ecx
  42abb7:	e9 b6 03 00 00       	jmp    0x42af72
  42abbc:	83 f8 01             	cmp    eax,0x1
  42abbf:	0f 85 f0 01 00 00    	jne    0x42adb5
  42abc5:	8a 4c 24 34          	mov    cl,BYTE PTR [esp+0x34]
  42abc9:	84 c9                	test   cl,cl
  42abcb:	0f 84 e4 01 00 00    	je     0x42adb5
  42abd1:	8b 8b d0 00 00 00    	mov    ecx,DWORD PTR [ebx+0xd0]
  42abd7:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42abdc:	0f af 4e 2c          	imul   ecx,DWORD PTR [esi+0x2c]
  42abe0:	f7 e9                	imul   ecx
  42abe2:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42abe6:	56                   	push   esi
  42abe7:	c1 fa 05             	sar    edx,0x5
  42abea:	8b c2                	mov    eax,edx
  42abec:	c1 e8 1f             	shr    eax,0x1f
  42abef:	03 d0                	add    edx,eax
  42abf1:	89 54 24 38          	mov    DWORD PTR [esp+0x38],edx
  42abf5:	e8 f6 20 00 00       	call   0x42ccf0
  42abfa:	8b 56 40             	mov    edx,DWORD PTR [esi+0x40]
  42abfd:	8b 0d b0 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b0
  42ac03:	03 c2                	add    eax,edx
  42ac05:	99                   	cdq
  42ac06:	2b c2                	sub    eax,edx
  42ac08:	8b 54 24 34          	mov    edx,DWORD PTR [esp+0x34]
  42ac0c:	d1 f8                	sar    eax,1
  42ac0e:	f7 d8                	neg    eax
  42ac10:	03 d0                	add    edx,eax
  42ac12:	8b 44 ae 64          	mov    eax,DWORD PTR [esi+ebp*4+0x64]
  42ac16:	89 54 24 34          	mov    DWORD PTR [esp+0x34],edx
  42ac1a:	99                   	cdq
  42ac1b:	33 c2                	xor    eax,edx
  42ac1d:	2b c2                	sub    eax,edx
  42ac1f:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  42ac23:	8b 86 90 00 00 00    	mov    eax,DWORD PTR [esi+0x90]
  42ac29:	85 c0                	test   eax,eax
  42ac2b:	7e 1c                	jle    0x42ac49
  42ac2d:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42ac34:	2b d0                	sub    edx,eax
  42ac36:	8d 14 90             	lea    edx,[eax+edx*4]
  42ac39:	c1 e2 04             	shl    edx,0x4
  42ac3c:	2b d0                	sub    edx,eax
  42ac3e:	8d 04 51             	lea    eax,[ecx+edx*2]
  42ac41:	8b 54 a8 74          	mov    edx,DWORD PTR [eax+ebp*4+0x74]
  42ac45:	89 54 24 30          	mov    DWORD PTR [esp+0x30],edx
  42ac49:	8b 86 94 00 00 00    	mov    eax,DWORD PTR [esi+0x94]
  42ac4f:	85 c0                	test   eax,eax
  42ac51:	7e 1c                	jle    0x42ac6f
  42ac53:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42ac5a:	2b d0                	sub    edx,eax
  42ac5c:	8d 14 90             	lea    edx,[eax+edx*4]
  42ac5f:	c1 e2 04             	shl    edx,0x4
  42ac62:	2b d0                	sub    edx,eax
  42ac64:	8d 04 51             	lea    eax,[ecx+edx*2]
  42ac67:	8b 54 a8 74          	mov    edx,DWORD PTR [eax+ebp*4+0x74]
  42ac6b:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  42ac6f:	8b 86 98 00 00 00    	mov    eax,DWORD PTR [esi+0x98]
  42ac75:	85 c0                	test   eax,eax
  42ac77:	7e 1c                	jle    0x42ac95
  42ac79:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42ac80:	2b d0                	sub    edx,eax
  42ac82:	8d 14 90             	lea    edx,[eax+edx*4]
  42ac85:	c1 e2 04             	shl    edx,0x4
  42ac88:	2b d0                	sub    edx,eax
  42ac8a:	8d 04 51             	lea    eax,[ecx+edx*2]
  42ac8d:	8b 4c a8 74          	mov    ecx,DWORD PTR [eax+ebp*4+0x74]
  42ac91:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  42ac95:	e8 cb 68 02 00       	call   0x451565
  42ac9a:	99                   	cdq
  42ac9b:	b9 09 00 00 00       	mov    ecx,0x9
  42aca0:	8b 6c 24 2c          	mov    ebp,DWORD PTR [esp+0x2c]
  42aca4:	f7 f9                	idiv   ecx
  42aca6:	8b c5                	mov    eax,ebp
  42aca8:	8b ca                	mov    ecx,edx
  42acaa:	99                   	cdq
  42acab:	41                   	inc    ecx
  42acac:	f7 f9                	idiv   ecx
  42acae:	03 e8                	add    ebp,eax
  42acb0:	e8 b0 68 02 00       	call   0x451565
  42acb5:	99                   	cdq
  42acb6:	b9 13 00 00 00       	mov    ecx,0x13
  42acbb:	f7 f9                	idiv   ecx
  42acbd:	8b 46 40             	mov    eax,DWORD PTR [esi+0x40]
  42acc0:	8b ca                	mov    ecx,edx
  42acc2:	99                   	cdq
  42acc3:	41                   	inc    ecx
  42acc4:	f7 f9                	idiv   ecx
  42acc6:	2b e8                	sub    ebp,eax
  42acc8:	89 6c 24 2c          	mov    DWORD PTR [esp+0x2c],ebp
  42accc:	e8 94 68 02 00       	call   0x451565
  42acd1:	99                   	cdq
  42acd2:	b9 63 00 00 00       	mov    ecx,0x63
  42acd7:	f7 f9                	idiv   ecx
  42acd9:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42acdd:	8b ca                	mov    ecx,edx
  42acdf:	99                   	cdq
  42ace0:	41                   	inc    ecx
  42ace1:	f7 f9                	idiv   ecx
  42ace3:	8b e8                	mov    ebp,eax
  42ace5:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42ace9:	03 e8                	add    ebp,eax
  42aceb:	e8 75 68 02 00       	call   0x451565
  42acf0:	99                   	cdq
  42acf1:	b9 13 00 00 00       	mov    ecx,0x13
  42acf6:	f7 f9                	idiv   ecx
  42acf8:	8b 46 4c             	mov    eax,DWORD PTR [esi+0x4c]
  42acfb:	8b ca                	mov    ecx,edx
  42acfd:	99                   	cdq
  42acfe:	41                   	inc    ecx
  42acff:	f7 f9                	idiv   ecx
  42ad01:	2b e8                	sub    ebp,eax
  42ad03:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42ad07:	99                   	cdq
  42ad08:	8b c8                	mov    ecx,eax
  42ad0a:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42ad0e:	33 ca                	xor    ecx,edx
  42ad10:	89 6c 24 34          	mov    DWORD PTR [esp+0x34],ebp
  42ad14:	2b ca                	sub    ecx,edx
  42ad16:	99                   	cdq
  42ad17:	db 44 24 34          	fild   DWORD PTR [esp+0x34]
  42ad1b:	33 c2                	xor    eax,edx
  42ad1d:	2b c2                	sub    eax,edx
  42ad1f:	0f af c8             	imul   ecx,eax
  42ad22:	8b 44 24 30          	mov    eax,DWORD PTR [esp+0x30]
  42ad26:	99                   	cdq
  42ad27:	33 c2                	xor    eax,edx
  42ad29:	2b c2                	sub    eax,edx
  42ad2b:	0f af c8             	imul   ecx,eax
  42ad2e:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42ad33:	f7 e9                	imul   ecx
  42ad35:	c1 fa 05             	sar    edx,0x5
  42ad38:	8b c2                	mov    eax,edx
  42ad3a:	c1 e8 1f             	shr    eax,0x1f
  42ad3d:	03 d0                	add    edx,eax
  42ad3f:	0f af 54 24 28       	imul   edx,DWORD PTR [esp+0x28]
  42ad44:	89 54 24 34          	mov    DWORD PTR [esp+0x34],edx
  42ad48:	db 44 24 34          	fild   DWORD PTR [esp+0x34]
  42ad4c:	de c9                	fmulp  st(1),st
  42ad4e:	e8 45 70 02 00       	call   0x451d98
  42ad53:	8b c8                	mov    ecx,eax
  42ad55:	b8 83 de 1b 43       	mov    eax,0x431bde83
  42ad5a:	f7 e9                	imul   ecx
  42ad5c:	c1 fa 12             	sar    edx,0x12
  42ad5f:	8b ca                	mov    ecx,edx
  42ad61:	c1 e9 1f             	shr    ecx,0x1f
  42ad64:	03 d1                	add    edx,ecx
  42ad66:	89 54 24 28          	mov    DWORD PTR [esp+0x28],edx
  42ad6a:	79 08                	jns    0x42ad74
  42ad6c:	c7 44 24 28 00 00 00 	mov    DWORD PTR [esp+0x28],0x0
  42ad73:	00 
  42ad74:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  42ad78:	85 c0                	test   eax,eax
  42ad7a:	0f 8e f2 01 00 00    	jle    0x42af72
  42ad80:	8b d0                	mov    edx,eax
  42ad82:	8b 44 96 64          	mov    eax,DWORD PTR [esi+edx*4+0x64]
  42ad86:	85 c0                	test   eax,eax
  42ad88:	7c 1c                	jl     0x42ada6
  42ad8a:	8b 44 24 30          	mov    eax,DWORD PTR [esp+0x30]
  42ad8e:	85 c0                	test   eax,eax
  42ad90:	7c 14                	jl     0x42ada6
  42ad92:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42ad96:	85 c0                	test   eax,eax
  42ad98:	7c 0c                	jl     0x42ada6
  42ad9a:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42ad9e:	85 c0                	test   eax,eax
  42ada0:	0f 8d cc 01 00 00    	jge    0x42af72
  42ada6:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  42adaa:	f7 d8                	neg    eax
  42adac:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  42adb0:	e9 bd 01 00 00       	jmp    0x42af72
  42adb5:	83 f8 02             	cmp    eax,0x2
  42adb8:	0f 85 d8 00 00 00    	jne    0x42ae96
  42adbe:	8b ab d8 00 00 00    	mov    ebp,DWORD PTR [ebx+0xd8]
  42adc4:	85 ed                	test   ebp,ebp
  42adc6:	7e 3a                	jle    0x42ae02
  42adc8:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  42adcb:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42add0:	0f af 8b c4 00 00 00 	imul   ecx,DWORD PTR [ebx+0xc4]
  42add7:	f7 e9                	imul   ecx
  42add9:	c1 fa 05             	sar    edx,0x5
  42addc:	8b ca                	mov    ecx,edx
  42adde:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42ade3:	c1 e9 1f             	shr    ecx,0x1f
  42ade6:	03 d1                	add    edx,ecx
  42ade8:	8b 8b d0 00 00 00    	mov    ecx,DWORD PTR [ebx+0xd0]
  42adee:	03 d1                	add    edx,ecx
  42adf0:	0f af d5             	imul   edx,ebp
  42adf3:	f7 ea                	imul   edx
  42adf5:	c1 fa 05             	sar    edx,0x5
  42adf8:	8b c2                	mov    eax,edx
  42adfa:	c1 e8 1f             	shr    eax,0x1f
  42adfd:	03 d0                	add    edx,eax
  42adff:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  42ae02:	8b ab dc 00 00 00    	mov    ebp,DWORD PTR [ebx+0xdc]
  42ae08:	85 ed                	test   ebp,ebp
  42ae0a:	7e 3a                	jle    0x42ae46
  42ae0c:	8b 4e 34             	mov    ecx,DWORD PTR [esi+0x34]
  42ae0f:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42ae14:	0f af 8b c4 00 00 00 	imul   ecx,DWORD PTR [ebx+0xc4]
  42ae1b:	f7 e9                	imul   ecx
  42ae1d:	c1 fa 05             	sar    edx,0x5
  42ae20:	8b ca                	mov    ecx,edx
  42ae22:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42ae27:	c1 e9 1f             	shr    ecx,0x1f
  42ae2a:	03 d1                	add    edx,ecx
  42ae2c:	8b 8b d0 00 00 00    	mov    ecx,DWORD PTR [ebx+0xd0]
  42ae32:	03 d1                	add    edx,ecx
  42ae34:	0f af d5             	imul   edx,ebp
  42ae37:	f7 ea                	imul   edx
  42ae39:	c1 fa 05             	sar    edx,0x5
  42ae3c:	8b c2                	mov    eax,edx
  42ae3e:	c1 e8 1f             	shr    eax,0x1f
  42ae41:	03 d0                	add    edx,eax
  42ae43:	89 57 08             	mov    DWORD PTR [edi+0x8],edx
  42ae46:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  42ae49:	6a 04                	push   0x4
  42ae4b:	51                   	push   ecx
  42ae4c:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  42ae50:	e8 eb 10 00 00       	call   0x42bf40
  42ae55:	84 c0                	test   al,al
  42ae57:	74 0a                	je     0x42ae63
  42ae59:	33 c0                	xor    eax,eax
  42ae5b:	89 47 04             	mov    DWORD PTR [edi+0x4],eax
  42ae5e:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  42ae61:	eb 02                	jmp    0x42ae65
  42ae63:	33 c0                	xor    eax,eax
  42ae65:	39 47 04             	cmp    DWORD PTR [edi+0x4],eax
  42ae68:	7d 03                	jge    0x42ae6d
  42ae6a:	89 47 04             	mov    DWORD PTR [edi+0x4],eax
  42ae6d:	39 47 08             	cmp    DWORD PTR [edi+0x8],eax
  42ae70:	7d 03                	jge    0x42ae75
  42ae72:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  42ae75:	39 86 14 02 00 00    	cmp    DWORD PTR [esi+0x214],eax
  42ae7b:	0f 8f ed 00 00 00    	jg     0x42af6e
  42ae81:	8b 57 04             	mov    edx,DWORD PTR [edi+0x4]
  42ae84:	8b 4f 08             	mov    ecx,DWORD PTR [edi+0x8]
  42ae87:	f7 da                	neg    edx
  42ae89:	f7 d9                	neg    ecx
  42ae8b:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  42ae8e:	89 4f 08             	mov    DWORD PTR [edi+0x8],ecx
  42ae91:	e9 d8 00 00 00       	jmp    0x42af6e
  42ae96:	83 f8 03             	cmp    eax,0x3
  42ae99:	0f 85 d3 00 00 00    	jne    0x42af72
  42ae9f:	8b ab d8 00 00 00    	mov    ebp,DWORD PTR [ebx+0xd8]
  42aea5:	85 ed                	test   ebp,ebp
  42aea7:	7e 3a                	jle    0x42aee3
  42aea9:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  42aeac:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42aeb1:	0f af 8b c4 00 00 00 	imul   ecx,DWORD PTR [ebx+0xc4]
  42aeb8:	f7 e9                	imul   ecx
  42aeba:	8b 8b d0 00 00 00    	mov    ecx,DWORD PTR [ebx+0xd0]
  42aec0:	c1 fa 05             	sar    edx,0x5
  42aec3:	8b c2                	mov    eax,edx
  42aec5:	c1 e8 1f             	shr    eax,0x1f
  42aec8:	03 d0                	add    edx,eax
  42aeca:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42aecf:	03 d1                	add    edx,ecx
  42aed1:	0f af d5             	imul   edx,ebp
  42aed4:	f7 ea                	imul   edx
  42aed6:	c1 fa 05             	sar    edx,0x5
  42aed9:	8b ca                	mov    ecx,edx
  42aedb:	c1 e9 1f             	shr    ecx,0x1f
  42aede:	03 d1                	add    edx,ecx
  42aee0:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  42aee3:	8b ab dc 00 00 00    	mov    ebp,DWORD PTR [ebx+0xdc]
  42aee9:	85 ed                	test   ebp,ebp
  42aeeb:	7e 3a                	jle    0x42af27
  42aeed:	8b 4e 34             	mov    ecx,DWORD PTR [esi+0x34]
  42aef0:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42aef5:	0f af 8b c4 00 00 00 	imul   ecx,DWORD PTR [ebx+0xc4]
  42aefc:	f7 e9                	imul   ecx
  42aefe:	8b 8b d0 00 00 00    	mov    ecx,DWORD PTR [ebx+0xd0]
  42af04:	c1 fa 05             	sar    edx,0x5
  42af07:	8b c2                	mov    eax,edx
  42af09:	c1 e8 1f             	shr    eax,0x1f
  42af0c:	03 d0                	add    edx,eax
  42af0e:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42af13:	03 d1                	add    edx,ecx
  42af15:	0f af d5             	imul   edx,ebp
  42af18:	f7 ea                	imul   edx
  42af1a:	c1 fa 05             	sar    edx,0x5
  42af1d:	8b ca                	mov    ecx,edx
  42af1f:	c1 e9 1f             	shr    ecx,0x1f
  42af22:	03 d1                	add    edx,ecx
  42af24:	89 57 08             	mov    DWORD PTR [edi+0x8],edx
  42af27:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  42af2a:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42af2e:	6a 04                	push   0x4
  42af30:	52                   	push   edx
  42af31:	e8 0a 10 00 00       	call   0x42bf40
  42af36:	84 c0                	test   al,al
  42af38:	74 0a                	je     0x42af44
  42af3a:	33 c0                	xor    eax,eax
  42af3c:	89 47 04             	mov    DWORD PTR [edi+0x4],eax
  42af3f:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  42af42:	eb 02                	jmp    0x42af46
  42af44:	33 c0                	xor    eax,eax
  42af46:	39 47 04             	cmp    DWORD PTR [edi+0x4],eax
  42af49:	7d 03                	jge    0x42af4e
  42af4b:	89 47 04             	mov    DWORD PTR [edi+0x4],eax
  42af4e:	39 47 08             	cmp    DWORD PTR [edi+0x8],eax
  42af51:	7d 03                	jge    0x42af56
  42af53:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  42af56:	39 86 14 02 00 00    	cmp    DWORD PTR [esi+0x214],eax
  42af5c:	7f 10                	jg     0x42af6e
  42af5e:	8b 4f 04             	mov    ecx,DWORD PTR [edi+0x4]
  42af61:	8b 57 08             	mov    edx,DWORD PTR [edi+0x8]
  42af64:	f7 d9                	neg    ecx
  42af66:	f7 da                	neg    edx
  42af68:	89 4f 04             	mov    DWORD PTR [edi+0x4],ecx
  42af6b:	89 57 08             	mov    DWORD PTR [edi+0x8],edx
  42af6e:	89 44 24 2c          	mov    DWORD PTR [esp+0x2c],eax
  42af72:	8b 44 24 2c          	mov    eax,DWORD PTR [esp+0x2c]
  42af76:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  42af7a:	03 c8                	add    ecx,eax
  42af7c:	8b 83 c8 00 00 00    	mov    eax,DWORD PTR [ebx+0xc8]
  42af82:	83 f8 03             	cmp    eax,0x3
  42af85:	74 4b                	je     0x42afd2
  42af87:	83 f8 02             	cmp    eax,0x2
  42af8a:	74 46                	je     0x42afd2
  42af8c:	8b 83 d8 00 00 00    	mov    eax,DWORD PTR [ebx+0xd8]
  42af92:	85 c0                	test   eax,eax
  42af94:	7e 19                	jle    0x42afaf
  42af96:	0f af c1             	imul   eax,ecx
  42af99:	8b d0                	mov    edx,eax
  42af9b:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42afa0:	f7 ea                	imul   edx
  42afa2:	c1 fa 05             	sar    edx,0x5
  42afa5:	8b c2                	mov    eax,edx
  42afa7:	c1 e8 1f             	shr    eax,0x1f
  42afaa:	03 d0                	add    edx,eax
  42afac:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  42afaf:	8b 83 dc 00 00 00    	mov    eax,DWORD PTR [ebx+0xdc]
  42afb5:	85 c0                	test   eax,eax
  42afb7:	7e 19                	jle    0x42afd2
  42afb9:	0f af c1             	imul   eax,ecx
  42afbc:	8b c8                	mov    ecx,eax
  42afbe:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42afc3:	f7 e9                	imul   ecx
  42afc5:	c1 fa 05             	sar    edx,0x5
  42afc8:	8b ca                	mov    ecx,edx
  42afca:	c1 e9 1f             	shr    ecx,0x1f
  42afcd:	03 d1                	add    edx,ecx
  42afcf:	89 57 08             	mov    DWORD PTR [edi+0x8],edx
  42afd2:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42afd6:	8b 4f 08             	mov    ecx,DWORD PTR [edi+0x8]
  42afd9:	53                   	push   ebx
  42afda:	8b 6c 24 1c          	mov    ebp,DWORD PTR [esp+0x1c]
  42afde:	8b 82 90 00 00 00    	mov    eax,DWORD PTR [edx+0x90]
  42afe4:	50                   	push   eax
  42afe5:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  42afe8:	03 c8                	add    ecx,eax
  42afea:	8d 47 3c             	lea    eax,[edi+0x3c]
  42afed:	51                   	push   ecx
  42afee:	50                   	push   eax
  42afef:	8d 47 38             	lea    eax,[edi+0x38]
  42aff2:	8b cd                	mov    ecx,ebp
  42aff4:	50                   	push   eax
  42aff5:	e8 a6 0e 00 00       	call   0x42bea0
  42affa:	83 bb d4 00 00 00 04 	cmp    DWORD PTR [ebx+0xd4],0x4
  42b001:	75 11                	jne    0x42b014
  42b003:	8b 86 28 02 00 00    	mov    eax,DWORD PTR [esi+0x228]
  42b009:	85 c0                	test   eax,eax
  42b00b:	7e 07                	jle    0x42b014
  42b00d:	c7 47 04 ff ff ff ff 	mov    DWORD PTR [edi+0x4],0xffffffff
  42b014:	8b 83 d4 00 00 00    	mov    eax,DWORD PTR [ebx+0xd4]
  42b01a:	83 f8 0a             	cmp    eax,0xa
  42b01d:	74 23                	je     0x42b042
  42b01f:	83 f8 0b             	cmp    eax,0xb
  42b022:	74 1e                	je     0x42b042
  42b024:	8b 83 c8 00 00 00    	mov    eax,DWORD PTR [ebx+0xc8]
  42b02a:	83 f8 02             	cmp    eax,0x2
  42b02d:	74 13                	je     0x42b042
  42b02f:	83 f8 03             	cmp    eax,0x3
  42b032:	74 0e                	je     0x42b042
  42b034:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42b038:	6a 00                	push   0x0
  42b03a:	57                   	push   edi
  42b03b:	53                   	push   ebx
  42b03c:	6a 00                	push   0x0
  42b03e:	56                   	push   esi
  42b03f:	52                   	push   edx
  42b040:	eb 0c                	jmp    0x42b04e
  42b042:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42b046:	6a 01                	push   0x1
  42b048:	57                   	push   edi
  42b049:	53                   	push   ebx
  42b04a:	6a 00                	push   0x0
  42b04c:	56                   	push   esi
  42b04d:	50                   	push   eax
  42b04e:	8b cd                	mov    ecx,ebp
  42b050:	e8 7b 00 00 00       	call   0x42b0d0
  42b055:	81 7b 20 08 01 00 00 	cmp    DWORD PTR [ebx+0x20],0x108
  42b05c:	75 10                	jne    0x42b06e
  42b05e:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  42b061:	f7 d9                	neg    ecx
  42b063:	89 4f 04             	mov    DWORD PTR [edi+0x4],ecx
  42b066:	8b 56 34             	mov    edx,DWORD PTR [esi+0x34]
  42b069:	f7 da                	neg    edx
  42b06b:	89 57 08             	mov    DWORD PTR [edi+0x8],edx
  42b06e:	8b 43 20             	mov    eax,DWORD PTR [ebx+0x20]
  42b071:	3d 0f 01 00 00       	cmp    eax,0x10f
  42b076:	74 0e                	je     0x42b086
  42b078:	3d 10 01 00 00       	cmp    eax,0x110
  42b07d:	74 07                	je     0x42b086
  42b07f:	3d 11 01 00 00       	cmp    eax,0x111
  42b084:	75 0b                	jne    0x42b091
  42b086:	c6 47 40 01          	mov    BYTE PTR [edi+0x40],0x1
  42b08a:	c7 46 30 01 00 00 00 	mov    DWORD PTR [esi+0x30],0x1
  42b091:	8b 83 c8 00 00 00    	mov    eax,DWORD PTR [ebx+0xc8]
  42b097:	83 f8 02             	cmp    eax,0x2
  42b09a:	74 16                	je     0x42b0b2
  42b09c:	83 f8 03             	cmp    eax,0x3
  42b09f:	74 11                	je     0x42b0b2
  42b0a1:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  42b0a4:	85 c0                	test   eax,eax
  42b0a6:	75 0a                	jne    0x42b0b2
  42b0a8:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  42b0ab:	85 c0                	test   eax,eax
  42b0ad:	75 03                	jne    0x42b0b2
  42b0af:	c6 07 00             	mov    BYTE PTR [edi],0x0
  42b0b2:	8b 83 7a 03 00 00    	mov    eax,DWORD PTR [ebx+0x37a]
  42b0b8:	89 47 44             	mov    DWORD PTR [edi+0x44],eax
  42b0bb:	5f                   	pop    edi
  42b0bc:	5e                   	pop    esi
  42b0bd:	5d                   	pop    ebp
  42b0be:	5b                   	pop    ebx
  42b0bf:	83 c4 10             	add    esp,0x10
  42b0c2:	c2 14 00             	ret    0x14
  42b0c5:	90                   	nop
  42b0c6:	90                   	nop
  42b0c7:	90                   	nop
  42b0c8:	90                   	nop
  42b0c9:	90                   	nop
  42b0ca:	90                   	nop
  42b0cb:	90                   	nop
  42b0cc:	90                   	nop
  42b0cd:	90                   	nop
  42b0ce:	90                   	nop
  42b0cf:	90                   	nop
  42b0d0:	83 ec 1c             	sub    esp,0x1c
  42b0d3:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42b0d7:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  42b0db:	53                   	push   ebx
  42b0dc:	55                   	push   ebp
  42b0dd:	8b 88 28 02 00 00    	mov    ecx,DWORD PTR [eax+0x228]
  42b0e3:	56                   	push   esi
  42b0e4:	85 c9                	test   ecx,ecx
  42b0e6:	57                   	push   edi
  42b0e7:	0f 8f 9e 04 00 00    	jg     0x42b58b
  42b0ed:	8b 4c 24 30          	mov    ecx,DWORD PTR [esp+0x30]
  42b0f1:	33 db                	xor    ebx,ebx
  42b0f3:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  42b0f7:	b8 01 00 00 00       	mov    eax,0x1
  42b0fc:	8b a9 90 00 00 00    	mov    ebp,DWORD PTR [ecx+0x90]
  42b102:	89 6c 24 20          	mov    DWORD PTR [esp+0x20],ebp
  42b106:	eb 04                	jmp    0x42b10c
  42b108:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42b10c:	33 ff                	xor    edi,edi
  42b10e:	3b c7                	cmp    eax,edi
  42b110:	0f 8e 75 04 00 00    	jle    0x42b58b
  42b116:	8b 4c 24 38          	mov    ecx,DWORD PTR [esp+0x38]
  42b11a:	89 7c 24 14          	mov    DWORD PTR [esp+0x14],edi
  42b11e:	3b cf                	cmp    ecx,edi
  42b120:	89 7c 24 10          	mov    DWORD PTR [esp+0x10],edi
  42b124:	74 61                	je     0x42b187
  42b126:	8b 44 99 64          	mov    eax,DWORD PTR [ecx+ebx*4+0x64]
  42b12a:	8b 54 99 78          	mov    edx,DWORD PTR [ecx+ebx*4+0x78]
  42b12e:	3b c7                	cmp    eax,edi
  42b130:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42b134:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  42b138:	75 49                	jne    0x42b183
  42b13a:	3b ef                	cmp    ebp,edi
  42b13c:	7e 45                	jle    0x42b183
  42b13e:	39 79 18             	cmp    DWORD PTR [ecx+0x18],edi
  42b141:	75 40                	jne    0x42b183
  42b143:	8d 04 ed 00 00 00 00 	lea    eax,[ebp*8+0x0]
  42b14a:	8b 0d b0 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b0
  42b150:	2b c5                	sub    eax,ebp
  42b152:	8d 44 85 00          	lea    eax,[ebp+eax*4+0x0]
  42b156:	c1 e0 04             	shl    eax,0x4
  42b159:	2b c5                	sub    eax,ebp
  42b15b:	d1 e0                	shl    eax,1
  42b15d:	8d 54 98 50          	lea    edx,[eax+ebx*4+0x50]
  42b161:	8d 44 98 64          	lea    eax,[eax+ebx*4+0x64]
  42b165:	8b 14 0a             	mov    edx,DWORD PTR [edx+ecx*1]
  42b168:	8b 0c 08             	mov    ecx,DWORD PTR [eax+ecx*1]
  42b16b:	85 d2                	test   edx,edx
  42b16d:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  42b171:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  42b175:	7e 0c                	jle    0x42b183
  42b177:	c7 44 24 18 04 00 00 	mov    DWORD PTR [esp+0x18],0x4
  42b17e:	00 
  42b17f:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  42b183:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  42b187:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  42b18b:	85 c0                	test   eax,eax
  42b18d:	74 18                	je     0x42b1a7
  42b18f:	8b 94 98 e8 00 00 00 	mov    edx,DWORD PTR [eax+ebx*4+0xe8]
  42b196:	8b 84 98 fc 00 00 00 	mov    eax,DWORD PTR [eax+ebx*4+0xfc]
  42b19d:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  42b1a1:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  42b1a5:	8b f8                	mov    edi,eax
  42b1a7:	85 ff                	test   edi,edi
  42b1a9:	7c 12                	jl     0x42b1bd
  42b1ab:	83 ff 1b             	cmp    edi,0x1b
  42b1ae:	7f 0d                	jg     0x42b1bd
  42b1b0:	8b cf                	mov    ecx,edi
  42b1b2:	c1 e1 04             	shl    ecx,0x4
  42b1b5:	8b b1 0c a0 46 00    	mov    esi,DWORD PTR [ecx+0x46a00c]
  42b1bb:	eb 02                	jmp    0x42b1bf
  42b1bd:	33 f6                	xor    esi,esi
  42b1bf:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42b1c3:	85 c0                	test   eax,eax
  42b1c5:	0f 8e b2 03 00 00    	jle    0x42b57d
  42b1cb:	83 ff 14             	cmp    edi,0x14
  42b1ce:	0f 8f 3a 01 00 00    	jg     0x42b30e
  42b1d4:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  42b1d8:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42b1dc:	bf 64 00 00 00       	mov    edi,0x64
  42b1e1:	8b 4a 4c             	mov    ecx,DWORD PTR [edx+0x4c]
  42b1e4:	8b 50 4c             	mov    edx,DWORD PTR [eax+0x4c]
  42b1e7:	2b ca                	sub    ecx,edx
  42b1e9:	b8 67 66 66 66       	mov    eax,0x66666667
  42b1ee:	f7 e9                	imul   ecx
  42b1f0:	c1 fa 02             	sar    edx,0x2
  42b1f3:	8b ca                	mov    ecx,edx
  42b1f5:	8b ef                	mov    ebp,edi
  42b1f7:	c1 e9 1f             	shr    ecx,0x1f
  42b1fa:	03 d1                	add    edx,ecx
  42b1fc:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  42b200:	03 d1                	add    edx,ecx
  42b202:	8b df                	mov    ebx,edi
  42b204:	85 f6                	test   esi,esi
  42b206:	89 54 24 28          	mov    DWORD PTR [esp+0x28],edx
  42b20a:	89 7c 24 1c          	mov    DWORD PTR [esp+0x1c],edi
  42b20e:	0f 8e 80 00 00 00    	jle    0x42b294
  42b214:	8b 4c 24 34          	mov    ecx,DWORD PTR [esp+0x34]
  42b218:	8b 81 90 00 00 00    	mov    eax,DWORD PTR [ecx+0x90]
  42b21e:	8b 7c b1 64          	mov    edi,DWORD PTR [ecx+esi*4+0x64]
  42b222:	85 c0                	test   eax,eax
  42b224:	7e 20                	jle    0x42b246
  42b226:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42b22d:	2b d0                	sub    edx,eax
  42b22f:	8d 2c 90             	lea    ebp,[eax+edx*4]
  42b232:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42b238:	c1 e5 04             	shl    ebp,0x4
  42b23b:	2b e8                	sub    ebp,eax
  42b23d:	8d 04 6a             	lea    eax,[edx+ebp*2]
  42b240:	8b 6c b0 74          	mov    ebp,DWORD PTR [eax+esi*4+0x74]
  42b244:	eb 06                	jmp    0x42b24c
  42b246:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42b24c:	8b 81 94 00 00 00    	mov    eax,DWORD PTR [ecx+0x94]
  42b252:	85 c0                	test   eax,eax
  42b254:	7e 18                	jle    0x42b26e
  42b256:	8d 1c c5 00 00 00 00 	lea    ebx,[eax*8+0x0]
  42b25d:	2b d8                	sub    ebx,eax
  42b25f:	8d 1c 98             	lea    ebx,[eax+ebx*4]
  42b262:	c1 e3 04             	shl    ebx,0x4
  42b265:	2b d8                	sub    ebx,eax
  42b267:	8d 04 5a             	lea    eax,[edx+ebx*2]
  42b26a:	8b 5c b0 74          	mov    ebx,DWORD PTR [eax+esi*4+0x74]
  42b26e:	8b 81 98 00 00 00    	mov    eax,DWORD PTR [ecx+0x98]
  42b274:	85 c0                	test   eax,eax
  42b276:	7e 1c                	jle    0x42b294
  42b278:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  42b27f:	2b c8                	sub    ecx,eax
  42b281:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  42b284:	c1 e1 04             	shl    ecx,0x4
  42b287:	2b c8                	sub    ecx,eax
  42b289:	8d 14 4a             	lea    edx,[edx+ecx*2]
  42b28c:	8b 44 b2 74          	mov    eax,DWORD PTR [edx+esi*4+0x74]
  42b290:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  42b294:	8b c7                	mov    eax,edi
  42b296:	99                   	cdq
  42b297:	8b c8                	mov    ecx,eax
  42b299:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  42b29d:	33 ca                	xor    ecx,edx
  42b29f:	2b ca                	sub    ecx,edx
  42b2a1:	99                   	cdq
  42b2a2:	33 c2                	xor    eax,edx
  42b2a4:	2b c2                	sub    eax,edx
  42b2a6:	0f af c8             	imul   ecx,eax
  42b2a9:	8b c3                	mov    eax,ebx
  42b2ab:	99                   	cdq
  42b2ac:	33 c2                	xor    eax,edx
  42b2ae:	2b c2                	sub    eax,edx
  42b2b0:	0f af c8             	imul   ecx,eax
  42b2b3:	8b c5                	mov    eax,ebp
  42b2b5:	99                   	cdq
  42b2b6:	33 c2                	xor    eax,edx
  42b2b8:	2b c2                	sub    eax,edx
  42b2ba:	0f af c8             	imul   ecx,eax
  42b2bd:	85 f6                	test   esi,esi
  42b2bf:	7e 16                	jle    0x42b2d7
  42b2c1:	85 ff                	test   edi,edi
  42b2c3:	7c 10                	jl     0x42b2d5
  42b2c5:	85 ed                	test   ebp,ebp
  42b2c7:	7c 0c                	jl     0x42b2d5
  42b2c9:	85 db                	test   ebx,ebx
  42b2cb:	7c 08                	jl     0x42b2d5
  42b2cd:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  42b2d1:	85 c0                	test   eax,eax
  42b2d3:	7d 02                	jge    0x42b2d7
  42b2d5:	f7 d9                	neg    ecx
  42b2d7:	b8 83 de 1b 43       	mov    eax,0x431bde83
  42b2dc:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  42b2e0:	f7 e9                	imul   ecx
  42b2e2:	c1 fa 12             	sar    edx,0x12
  42b2e5:	8b ca                	mov    ecx,edx
  42b2e7:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b2ec:	c1 e9 1f             	shr    ecx,0x1f
  42b2ef:	03 d1                	add    edx,ecx
  42b2f1:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  42b2f5:	0f af 54 24 28       	imul   edx,DWORD PTR [esp+0x28]
  42b2fa:	f7 ea                	imul   edx
  42b2fc:	c1 fa 05             	sar    edx,0x5
  42b2ff:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  42b303:	8b c2                	mov    eax,edx
  42b305:	c1 e8 1f             	shr    eax,0x1f
  42b308:	03 d0                	add    edx,eax
  42b30a:	8b f2                	mov    esi,edx
  42b30c:	eb 05                	jmp    0x42b313
  42b30e:	be 64 00 00 00       	mov    esi,0x64
  42b313:	83 ff 1b             	cmp    edi,0x1b
  42b316:	7e 15                	jle    0x42b32d
  42b318:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  42b31c:	89 7c 98 10          	mov    DWORD PTR [eax+ebx*4+0x10],edi
  42b320:	c7 44 98 24 40 1f 00 	mov    DWORD PTR [eax+ebx*4+0x24],0x1f40
  42b327:	00 
  42b328:	e9 50 02 00 00       	jmp    0x42b57d
  42b32d:	e8 33 62 02 00       	call   0x451565
  42b332:	99                   	cdq
  42b333:	b9 64 00 00 00       	mov    ecx,0x64
  42b338:	f7 f9                	idiv   ecx
  42b33a:	3b d6                	cmp    edx,esi
  42b33c:	7c 0c                	jl     0x42b34a
  42b33e:	8a 44 24 44          	mov    al,BYTE PTR [esp+0x44]
  42b342:	84 c0                	test   al,al
  42b344:	0f 84 33 02 00 00    	je     0x42b57d
  42b34a:	8b 54 24 38          	mov    edx,DWORD PTR [esp+0x38]
  42b34e:	c1 e7 04             	shl    edi,0x4
  42b351:	85 d2                	test   edx,edx
  42b353:	8b         	mov    ecx,DWORD PTR [edi+0x46a000]
