; ===== slot4公式局部布尔/结果抑制 A =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042a8ff <.text+0x298ff>:
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
; ===== slot4结果清零 B =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042ae43 <.text+0x29e43>:
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
; ===== slot4局部模式门 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042c0c4 <.text+0x2b0c4>:
  42c0c4:	8b 53 04             	mov    edx,DWORD PTR [ebx+0x4]
  42c0c7:	8b 6c 24 1c          	mov    ebp,DWORD PTR [esp+0x1c]
  42c0cb:	6a 04                	push   0x4
  42c0cd:	52                   	push   edx
  42c0ce:	8b cd                	mov    ecx,ebp
  42c0d0:	e8 6b fe ff ff       	call   0x42bf40
  42c0d5:	84 c0                	test   al,al
  42c0d7:	74 0d                	je     0x42c0e6
  42c0d9:	83 ff 04             	cmp    edi,0x4
  42c0dc:	75 08                	jne    0x42c0e6
  42c0de:	c7 44 24 30 00 00 00 	mov    DWORD PTR [esp+0x30],0x0
  42c0e5:	00 
; ===== slot4 helper输出抑制 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042c48b <.text+0x2b48b>:
  42c48b:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42c490:	6a 04                	push   0x4
  42c492:	f7 ea                	imul   edx
  42c494:	c1 fa 05             	sar    edx,0x5
  42c497:	8b c2                	mov    eax,edx
  42c499:	c1 e8 1f             	shr    eax,0x1f
  42c49c:	03 d0                	add    edx,eax
  42c49e:	8b f2                	mov    esi,edx
  42c4a0:	8b 57 4c             	mov    edx,DWORD PTR [edi+0x4c]
  42c4a3:	03 f2                	add    esi,edx
  42c4a5:	8b 53 04             	mov    edx,DWORD PTR [ebx+0x4]
  42c4a8:	52                   	push   edx
  42c4a9:	e8 92 fa ff ff       	call   0x42bf40
  42c4ae:	84 c0                	test   al,al
  42c4b0:	74 02                	je     0x42c4b4
  42c4b2:	33 f6                	xor    esi,esi
  42c4b4:	85 f6                	test   esi,esi
  42c4b6:	8b c6                	mov    eax,esi
  42c4b8:	7d 02                	jge    0x42c4bc
  42c4ba:	33 c0                	xor    eax,eax
; ===== slot3局部布尔门 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042c884 <.text+0x2b884>:
  42c884:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  42c887:	50                   	push   eax
  42c888:	8b cd                	mov    ecx,ebp
  42c88a:	e8 b1 f6 ff ff       	call   0x42bf40
  42c88f:	84 c0                	test   al,al
  42c891:	74 04                	je     0x42c897
  42c893:	b1 01                	mov    cl,0x1
  42c895:	eb 34                	jmp    0x42c8cb
  42c897:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42c89b:	8b 88 20 02 00 00    	mov    ecx,DWORD PTR [eax+0x220]
  42c8a1:	85 c9                	test   ecx,ecx
  42c8a3:	7e 04                	jle    0x42c8a9
  42c8a5:	32 c9                	xor    cl,cl
  42c8a7:	eb 22                	jmp    0x42c8cb
  42c8a9:	8b 88 24 02 00 00    	mov    ecx,DWORD PTR [eax+0x224]
  42c8af:	85 c9                	test   ecx,ecx
  42c8b1:	7e 04                	jle    0x42c8b7
  42c8b3:	b1 01                	mov    cl,0x1
  42c8b5:	eb 14                	jmp    0x42c8cb
  42c8b7:	e8 a9 4c 02 00       	call   0x451565
  42c8bc:	99                   	cdq
  42c8bd:	b9 64 00 00 00       	mov    ecx,0x64
  42c8c2:	f7 f9                	idiv   ecx
  42c8c4:	3b 54 24 2c          	cmp    edx,DWORD PTR [esp+0x2c]
  42c8c8:	0f 9c c1             	setl   cl
