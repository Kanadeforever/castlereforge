
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040a7d0 <.text+0x97d0>:
  40a7d0:	51                   	push   ecx
  40a7d1:	53                   	push   ebx
  40a7d2:	55                   	push   ebp
  40a7d3:	56                   	push   esi
  40a7d4:	8b f1                	mov    esi,ecx
  40a7d6:	57                   	push   edi
  40a7d7:	8a 46 54             	mov    al,BYTE PTR [esi+0x54]
  40a7da:	84 c0                	test   al,al
  40a7dc:	0f 85 fa 02 00 00    	jne    0x40aadc
  40a7e2:	8b 46 70             	mov    eax,DWORD PTR [esi+0x70]
  40a7e5:	80 78 76 03          	cmp    BYTE PTR [eax+0x76],0x3
  40a7e9:	75 52                	jne    0x40a83d
  40a7eb:	8b 0d 14 85 97 00    	mov    ecx,DWORD PTR ds:0x978514
  40a7f1:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  40a7f4:	8b 3d 18 85 97 00    	mov    edi,DWORD PTR ds:0x978518
  40a7fa:	3b ca                	cmp    ecx,edx
  40a7fc:	0f 8f da 02 00 00    	jg     0x40aadc
  40a802:	81 c1 80 02 00 00    	add    ecx,0x280
  40a808:	3b ca                	cmp    ecx,edx
  40a80a:	0f 8c cc 02 00 00    	jl     0x40aadc
  40a810:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40a813:	3b f9                	cmp    edi,ecx
  40a815:	0f 8f c1 02 00 00    	jg     0x40aadc
  40a81b:	81 c7 e0 01 00 00    	add    edi,0x1e0
  40a821:	3b f9                	cmp    edi,ecx
  40a823:	0f 8c b3 02 00 00    	jl     0x40aadc
  40a829:	8b 7e 58             	mov    edi,DWORD PTR [esi+0x58]
  40a82c:	8b 56 5c             	mov    edx,DWORD PTR [esi+0x5c]
  40a82f:	47                   	inc    edi
  40a830:	8b cf                	mov    ecx,edi
  40a832:	89 7e 58             	mov    DWORD PTR [esi+0x58],edi
  40a835:	3b ca                	cmp    ecx,edx
  40a837:	0f 8c 9f 02 00 00    	jl     0x40aadc
  40a83d:	0f be 48 77          	movsx  ecx,BYTE PTR [eax+0x77]
  40a841:	83 c1 06             	add    ecx,0x6
  40a844:	89 4e 1c             	mov    DWORD PTR [esi+0x1c],ecx
  40a847:	0f be 50 78          	movsx  edx,BYTE PTR [eax+0x78]
  40a84b:	83 c2 04             	add    edx,0x4
  40a84e:	89 56 20             	mov    DWORD PTR [esi+0x20],edx
  40a851:	0f be 48 79          	movsx  ecx,BYTE PTR [eax+0x79]
  40a855:	83 c1 09             	add    ecx,0x9
  40a858:	89 4e 24             	mov    DWORD PTR [esi+0x24],ecx
  40a85b:	8b 4e 34             	mov    ecx,DWORD PTR [esi+0x34]
  40a85e:	0f be 50 7a          	movsx  edx,BYTE PTR [eax+0x7a]
  40a862:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40a865:	83 c2 06             	add    edx,0x6
  40a868:	25 00 ff 00 00       	and    eax,0xff00
  40a86d:	89 56 28             	mov    DWORD PTR [esi+0x28],edx
  40a870:	f7 d8                	neg    eax
  40a872:	1b c0                	sbb    eax,eax
  40a874:	33 ff                	xor    edi,edi
  40a876:	f7 d8                	neg    eax
  40a878:	2b cf                	sub    ecx,edi
  40a87a:	0f 84 d8 01 00 00    	je     0x40aa58
  40a880:	49                   	dec    ecx
  40a881:	0f 84 89 01 00 00    	je     0x40aa10
  40a887:	49                   	dec    ecx
  40a888:	74 17                	je     0x40a8a1
  40a88a:	8d 44 24 10          	lea    eax,[esp+0x10]
  40a88e:	68 d8 37 46 00       	push   0x4637d8
  40a893:	50                   	push   eax
  40a894:	c7 44 24 18 60 8c 46 	mov    DWORD PTR [esp+0x18],0x468c60
  40a89b:	00 
  40a89c:	e8 26 73 04 00       	call   0x451bc7
  40a8a1:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40a8a4:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  40a8a7:	3b ca                	cmp    ecx,edx
  40a8a9:	7e 5a                	jle    0x40a905
  40a8ab:	8b 5e 14             	mov    ebx,DWORD PTR [esi+0x14]
  40a8ae:	8b 6e 30             	mov    ebp,DWORD PTR [esi+0x30]
  40a8b1:	3b dd                	cmp    ebx,ebp
  40a8b3:	7e 20                	jle    0x40a8d5
  40a8b5:	2b ca                	sub    ecx,edx
  40a8b7:	8b 54 c6 20          	mov    edx,DWORD PTR [esi+eax*8+0x20]
  40a8bb:	3b ca                	cmp    ecx,edx
  40a8bd:	7c 08                	jl     0x40a8c7
  40a8bf:	8b cb                	mov    ecx,ebx
  40a8c1:	2b cd                	sub    ecx,ebp
  40a8c3:	3b ca                	cmp    ecx,edx
  40a8c5:	7d 07                	jge    0x40a8ce
  40a8c7:	89 4c c6 20          	mov    DWORD PTR [esi+eax*8+0x20],ecx
  40a8cb:	89 7e 34             	mov    DWORD PTR [esi+0x34],edi
  40a8ce:	a1 54 8c 46 00       	mov    eax,ds:0x468c54
  40a8d3:	eb 6a                	jmp    0x40a93f
  40a8d5:	3b ca                	cmp    ecx,edx
  40a8d7:	7e 2c                	jle    0x40a905
  40a8d9:	8b 5e 14             	mov    ebx,DWORD PTR [esi+0x14]
  40a8dc:	8b 6e 30             	mov    ebp,DWORD PTR [esi+0x30]
  40a8df:	3b dd                	cmp    ebx,ebp
  40a8e1:	7d 20                	jge    0x40a903
  40a8e3:	2b ca                	sub    ecx,edx
  40a8e5:	8b 54 c6 20          	mov    edx,DWORD PTR [esi+eax*8+0x20]
  40a8e9:	3b ca                	cmp    ecx,edx
  40a8eb:	7c 08                	jl     0x40a8f5
  40a8ed:	8b cd                	mov    ecx,ebp
  40a8ef:	2b cb                	sub    ecx,ebx
  40a8f1:	3b ca                	cmp    ecx,edx
  40a8f3:	7d 07                	jge    0x40a8fc
  40a8f5:	89 4c c6 20          	mov    DWORD PTR [esi+eax*8+0x20],ecx
  40a8f9:	89 7e 34             	mov    DWORD PTR [esi+0x34],edi
  40a8fc:	a1 5c 8c 46 00       	mov    eax,ds:0x468c5c
  40a901:	eb 3c                	jmp    0x40a93f
  40a903:	3b ca                	cmp    ecx,edx
  40a905:	0f 8d b1 00 00 00    	jge    0x40a9bc
  40a90b:	8b 5e 14             	mov    ebx,DWORD PTR [esi+0x14]
  40a90e:	8b 6e 30             	mov    ebp,DWORD PTR [esi+0x30]
  40a911:	3b dd                	cmp    ebx,ebp
  40a913:	7e 61                	jle    0x40a976
  40a915:	8b 7c c6 20          	mov    edi,DWORD PTR [esi+eax*8+0x20]
  40a919:	2b d1                	sub    edx,ecx
  40a91b:	3b d7                	cmp    edx,edi
  40a91d:	7d 06                	jge    0x40a925
  40a91f:	89 54 c6 20          	mov    DWORD PTR [esi+eax*8+0x20],edx
  40a923:	eb 0c                	jmp    0x40a931
  40a925:	8b cb                	mov    ecx,ebx
  40a927:	2b cd                	sub    ecx,ebp
  40a929:	3b cf                	cmp    ecx,edi
  40a92b:	7d 0b                	jge    0x40a938
  40a92d:	89 4c c6 20          	mov    DWORD PTR [esi+eax*8+0x20],ecx
  40a931:	c7 46 34 00 00 00 00 	mov    DWORD PTR [esi+0x34],0x0
  40a938:	a1 4c 8c 46 00       	mov    eax,ds:0x468c4c
  40a93d:	33 ff                	xor    edi,edi
  40a93f:	3b c7                	cmp    eax,edi
  40a941:	0f 85 81 01 00 00    	jne    0x40aac8
  40a947:	8b 46 38             	mov    eax,DWORD PTR [esi+0x38]
  40a94a:	83 f8 08             	cmp    eax,0x8
  40a94d:	0f 85 51 01 00 00    	jne    0x40aaa4
  40a953:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  40a956:	8b 56 0c             	mov    edx,DWORD PTR [esi+0xc]
  40a959:	33 d0                	xor    edx,eax
  40a95b:	5f                   	pop    edi
  40a95c:	81 e2 00 00 ff 00    	and    edx,0xff0000
  40a962:	33 d0                	xor    edx,eax
  40a964:	8b c2                	mov    eax,edx
  40a966:	89 56 18             	mov    DWORD PTR [esi+0x18],edx
  40a969:	25 00 00 ff ff       	and    eax,0xffff0000
  40a96e:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  40a971:	5e                   	pop    esi
  40a972:	5d                   	pop    ebp
  40a973:	5b                   	pop    ebx
  40a974:	59                   	pop    ecx
  40a975:	c3                   	ret
  40a976:	3b ca                	cmp    ecx,edx
  40a978:	7d 42                	jge    0x40a9bc
  40a97a:	8b 6e 14             	mov    ebp,DWORD PTR [esi+0x14]
  40a97d:	8b 5e 30             	mov    ebx,DWORD PTR [esi+0x30]
  40a980:	3b eb                	cmp    ebp,ebx
  40a982:	7d 36                	jge    0x40a9ba
  40a984:	8b 7c c6 20          	mov    edi,DWORD PTR [esi+eax*8+0x20]
  40a988:	2b d1                	sub    edx,ecx
  40a98a:	3b d7                	cmp    edx,edi
  40a98c:	7d 12                	jge    0x40a9a0
  40a98e:	89 54 c6 20          	mov    DWORD PTR [esi+eax*8+0x20],edx
  40a992:	c7 46 34 00 00 00 00 	mov    DWORD PTR [esi+0x34],0x0
  40a999:	a1 44 8c 46 00       	mov    eax,ds:0x468c44
  40a99e:	eb 9d                	jmp    0x40a93d
  40a9a0:	8b cb                	mov    ecx,ebx
  40a9a2:	2b cd                	sub    ecx,ebp
  40a9a4:	3b cf                	cmp    ecx,edi
  40a9a6:	7d 0b                	jge    0x40a9b3
  40a9a8:	89 4c c6 20          	mov    DWORD PTR [esi+eax*8+0x20],ecx
  40a9ac:	c7 46 34 00 00 00 00 	mov    DWORD PTR [esi+0x34],0x0
  40a9b3:	a1 44 8c 46 00       	mov    eax,ds:0x468c44
  40a9b8:	eb 83                	jmp    0x40a93d
  40a9ba:	3b ca                	cmp    ecx,edx
  40a9bc:	75 1a                	jne    0x40a9d8
  40a9be:	8b 46 14             	mov    eax,DWORD PTR [esi+0x14]
  40a9c1:	8b 5e 30             	mov    ebx,DWORD PTR [esi+0x30]
  40a9c4:	3b c3                	cmp    eax,ebx
  40a9c6:	75 0c                	jne    0x40a9d4
  40a9c8:	8b ce                	mov    ecx,esi
  40a9ca:	e8 a1 fd ff ff       	call   0x40a770
  40a9cf:	e9 73 ff ff ff       	jmp    0x40a947
  40a9d4:	3b ca                	cmp    ecx,edx
  40a9d6:	74 1c                	je     0x40a9f4
  40a9d8:	7e 0d                	jle    0x40a9e7
  40a9da:	a1 58 8c 46 00       	mov    eax,ds:0x468c58
  40a9df:	89 7e 34             	mov    DWORD PTR [esi+0x34],edi
  40a9e2:	e9 58 ff ff ff       	jmp    0x40a93f
  40a9e7:	a1 48 8c 46 00       	mov    eax,ds:0x468c48
  40a9ec:	89 7e 34             	mov    DWORD PTR [esi+0x34],edi
  40a9ef:	e9 4b ff ff ff       	jmp    0x40a93f
  40a9f4:	8b 4e 14             	mov    ecx,DWORD PTR [esi+0x14]
  40a9f7:	8b 46 30             	mov    eax,DWORD PTR [esi+0x30]
  40a9fa:	3b c8                	cmp    ecx,eax
  40a9fc:	a1 50 8c 46 00       	mov    eax,ds:0x468c50
  40aa01:	7f 05                	jg     0x40aa08
  40aa03:	a1 40 8c 46 00       	mov    eax,ds:0x468c40
  40aa08:	89 7e 34             	mov    DWORD PTR [esi+0x34],edi
  40aa0b:	e9 2f ff ff ff       	jmp    0x40a93f
  40aa10:	8b 56 14             	mov    edx,DWORD PTR [esi+0x14]
  40aa13:	8b 4e 30             	mov    ecx,DWORD PTR [esi+0x30]
  40aa16:	3b d1                	cmp    edx,ecx
  40aa18:	74 1a                	je     0x40aa34
  40aa1a:	7f 70                	jg     0x40aa8c
  40aa1c:	2b ca                	sub    ecx,edx
  40aa1e:	8b 54 c6 1c          	mov    edx,DWORD PTR [esi+eax*8+0x1c]
  40aa22:	3b ca                	cmp    ecx,edx
  40aa24:	7d 04                	jge    0x40aa2a
  40aa26:	89 4c c6 1c          	mov    DWORD PTR [esi+eax*8+0x1c],ecx
  40aa2a:	a1 40 8c 46 00       	mov    eax,ds:0x468c40
  40aa2f:	e9 0b ff ff ff       	jmp    0x40a93f
  40aa34:	8b 56 10             	mov    edx,DWORD PTR [esi+0x10]
  40aa37:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  40aa3a:	3b d1                	cmp    edx,ecx
  40aa3c:	74 8a                	je     0x40a9c8
  40aa3e:	7f 24                	jg     0x40aa64
  40aa40:	2b ca                	sub    ecx,edx
  40aa42:	8b 54 c6 1c          	mov    edx,DWORD PTR [esi+eax*8+0x1c]
  40aa46:	3b ca                	cmp    ecx,edx
  40aa48:	7d 04                	jge    0x40aa4e
  40aa4a:	89 4c c6 1c          	mov    DWORD PTR [esi+eax*8+0x1c],ecx
  40aa4e:	a1 48 8c 46 00       	mov    eax,ds:0x468c48
  40aa53:	e9 e7 fe ff ff       	jmp    0x40a93f
  40aa58:	8b 56 10             	mov    edx,DWORD PTR [esi+0x10]
  40aa5b:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  40aa5e:	3b d1                	cmp    edx,ecx
  40aa60:	74 1a                	je     0x40aa7c
  40aa62:	7e dc                	jle    0x40aa40
  40aa64:	2b d1                	sub    edx,ecx
  40aa66:	8b 4c c6 1c          	mov    ecx,DWORD PTR [esi+eax*8+0x1c]
  40aa6a:	3b d1                	cmp    edx,ecx
  40aa6c:	7d 04                	jge    0x40aa72
  40aa6e:	89 54 c6 1c          	mov    DWORD PTR [esi+eax*8+0x1c],edx
  40aa72:	a1 58 8c 46 00       	mov    eax,ds:0x468c58
  40aa77:	e9 c3 fe ff ff       	jmp    0x40a93f
  40aa7c:	8b 56 14             	mov    edx,DWORD PTR [esi+0x14]
  40aa7f:	8b 4e 30             	mov    ecx,DWORD PTR [esi+0x30]
  40aa82:	3b d1                	cmp    edx,ecx
  40aa84:	0f 84 3e ff ff ff    	je     0x40a9c8
  40aa8a:	7e 90                	jle    0x40aa1c
  40aa8c:	2b d1                	sub    edx,ecx
  40aa8e:	8b 4c c6 1c          	mov    ecx,DWORD PTR [esi+eax*8+0x1c]
  40aa92:	3b d1                	cmp    edx,ecx
  40aa94:	7d 04                	jge    0x40aa9a
  40aa96:	89 54 c6 1c          	mov    DWORD PTR [esi+eax*8+0x1c],edx
  40aa9a:	a1 50 8c 46 00       	mov    eax,ds:0x468c50
  40aa9f:	e9 9b fe ff ff       	jmp    0x40a93f
  40aaa4:	8b 4e 18             	mov    ecx,DWORD PTR [esi+0x18]
  40aaa7:	25 ff 00 00 00       	and    eax,0xff
  40aaac:	c1 e0 10             	shl    eax,0x10
  40aaaf:	81 e1 ff ff 00 ff    	and    ecx,0xff00ffff
  40aab5:	5f                   	pop    edi
  40aab6:	0b c1                	or     eax,ecx
  40aab8:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  40aabb:	25 00 00 ff ff       	and    eax,0xffff0000
  40aac0:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  40aac3:	5e                   	pop    esi
  40aac4:	5d                   	pop    ebp
  40aac5:	5b                   	pop    ebx
  40aac6:	59                   	pop    ecx
  40aac7:	c3                   	ret
  40aac8:	8b 56 18             	mov    edx,DWORD PTR [esi+0x18]
  40aacb:	50                   	push   eax
  40aacc:	80 e2 02             	and    dl,0x2
  40aacf:	8b ce                	mov    ecx,esi
  40aad1:	83 ca 02             	or     edx,0x2
  40aad4:	89 56 18             	mov    DWORD PTR [esi+0x18],edx
  40aad7:	e8 94 fa ff ff       	call   0x40a570
  40aadc:	5f                   	pop    edi
  40aadd:	5e                   	pop    esi
  40aade:	5d                   	pop    ebp
  40aadf:	5b                   	pop    ebx
  40aae0:	59                   	pop    ecx
  40aae1:	c3                   	ret
  40aae2:	90                   	nop
  40aae3:	90                   	nop
  40aae4:	90                   	nop
  40aae5:	90                   	nop
  40aae6:	90                   	nop
  40aae7:	90                   	nop
  40aae8:	90                   	nop
  40aae9:	90                   	nop
  40aaea:	90                   	nop
  40aaeb:	90                   	nop
  40aaec:	90                   	nop
  40aaed:	90                   	nop
  40aaee:	90                   	nop
  40aaef:	90                   	nop
  40aaf0:	56                   	push   esi
  40aaf1:	8b f1                	mov    esi,ecx
  40aaf3:	57                   	push   edi
  40aaf4:	8b 46 70             	mov    eax,DWORD PTR [esi+0x70]
  40aaf7:	80 78 76 03          	cmp    BYTE PTR [eax+0x76],0x3
  40aafb:	0f 85 ec 00 00 00    	jne    0x40abed
  40ab01:	8b 4e 3c             	mov    ecx,DWORD PTR [esi+0x3c]
  40ab04:	85 c9                	test   ecx,ecx
  40ab06:	0f 84 e1 00 00 00    	je     0x40abed
  40ab0c:	8a 4e 54             	mov    cl,BYTE PTR [esi+0x54]
  40ab0f:	84 c9                	test   cl,cl
  40ab11:	0f 85 d6 00 00 00    	jne    0x40abed
  40ab17:	8b 7e 60             	mov    edi,DWORD PTR [esi+0x60]
  40ab1a:	8b 56 64             	mov    edx,DWORD PTR [esi+0x64]
  40ab1d:	47                   	inc    edi
  40ab1e:	8b cf                	mov    ecx,edi
  40ab20:	89 7e 60             	mov    DWORD PTR [esi+0x60],edi
  40ab23:	3b ca                	cmp    ecx,edx
  40ab25:	0f 8c c2 00 00 00    	jl     0x40abed
  40ab2b:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  40ab2f:	85 c9                	test   ecx,ecx
  40ab31:	b9 01 00 00 00       	mov    ecx,0x1
  40ab36:	75 06                	jne    0x40ab3e
  40ab38:	2b 4e 68             	sub    ecx,DWORD PTR [esi+0x68]
  40ab3b:	89 4e 68             	mov    DWORD PTR [esi+0x68],ecx
  40ab3e:	83 e9 00             	sub    ecx,0x0
  40ab41:	0f 84 85 00 00 00    	je     0x40abcc
  40ab47:	49                   	dec    ecx
  40ab48:	0f 85 9f 00 00 00    	jne    0x40abed
  40ab4e:	8b b8 f7 00 00 00    	mov    edi,DWORD PTR [eax+0xf7]
  40ab54:	8b 90 f3 00 00 00    	mov    edx,DWORD PTR [eax+0xf3]
  40ab5a:	55                   	push   ebp
  40ab5b:	8b a8 ef 00 00 00    	mov    ebp,DWORD PTR [eax+0xef]
  40ab61:	2b fd                	sub    edi,ebp
  40ab63:	8b a8 fb 00 00 00    	mov    ebp,DWORD PTR [eax+0xfb]
  40ab69:	2b ea                	sub    ebp,edx
  40ab6b:	c7 46 3c 00 00 00 00 	mov    DWORD PTR [esi+0x3c],0x0
  40ab72:	e8 ee 69 04 00       	call   0x451565
  40ab77:	99                   	cdq
  40ab78:	f7 ff                	idiv   edi
  40ab7a:	8b 46 70             	mov    eax,DWORD PTR [esi+0x70]
  40ab7d:	03 90 ef 00 00 00    	add    edx,DWORD PTR [eax+0xef]
  40ab83:	89 56 2c             	mov    DWORD PTR [esi+0x2c],edx
  40ab86:	e8 da 69 04 00       	call   0x451565
  40ab8b:	99                   	cdq
  40ab8c:	f7 fd                	idiv   ebp
  40ab8e:	8b 4e 70             	mov    ecx,DWORD PTR [esi+0x70]
  40ab91:	03 91 f3 00 00 00    	add    edx,DWORD PTR [ecx+0xf3]
  40ab97:	89 56 30             	mov    DWORD PTR [esi+0x30],edx
  40ab9a:	e8 c6 69 04 00       	call   0x451565
  40ab9f:	25 01 00 00 80       	and    eax,0x80000001
  40aba4:	79 05                	jns    0x40abab
  40aba6:	48                   	dec    eax
  40aba7:	83 c8 fe             	or     eax,0xfffffffe
  40abaa:	40                   	inc    eax
  40abab:	8b 56 70             	mov    edx,DWORD PTR [esi+0x70]
  40abae:	89 46 34             	mov    DWORD PTR [esi+0x34],eax
  40abb1:	c7 46 38 08 00 00 00 	mov    DWORD PTR [esi+0x38],0x8
  40abb8:	5d                   	pop    ebp
  40abb9:	c6 42 74 00          	mov    BYTE PTR [edx+0x74],0x0
  40abbd:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  40abc0:	24 02                	and    al,0x2
  40abc2:	5f                   	pop    edi
  40abc3:	0c 02                	or     al,0x2
  40abc5:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  40abc8:	5e                   	pop    esi
  40abc9:	c2 04 00             	ret    0x4
  40abcc:	c7 46 60 00 00 00 00 	mov    DWORD PTR [esi+0x60],0x0
  40abd3:	e8 8d 69 04 00       	call   0x451565
  40abd8:	99                   	cdq
  40abd9:	b9 1e 00 00 00       	mov    ecx,0x1e
  40abde:	f7 f9                	idiv   ecx
  40abe0:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  40abe3:	03 d1                	add    edx,ecx
  40abe5:	24 00                	and    al,0x0
  40abe7:	89 56 64             	mov    DWORD PTR [esi+0x64],edx
  40abea:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  40abed:	5f                   	pop    edi
  40abee:	5e                   	pop    esi
  40abef:	c2 04 00             	ret    0x4
  40abf2:	90                   	nop
  40abf3:	90                   	nop
  40abf4:	90                   	nop
  40abf5:	90                   	nop
  40abf6:	90                   	nop
  40abf7:	90                   	nop
  40abf8:	90                   	nop
  40abf9:	90                   	nop
  40abfa:	90                   	nop
  40abfb:	90                   	nop
  40abfc:	90                   	nop
  40abfd:	90                   	nop
  40abfe:	90                   	nop
  40abff:	90                   	nop
  40ac00:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40ac06:	6a ff                	push   0xffffffff
  40ac08:	68 eb d0 45 00       	push   0x45d0eb
  40ac0d:	50                   	push   eax
  40ac0e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40ac15:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  40ac19:	53                   	push   ebx
  40ac1a:	55                   	push   ebp
  40ac1b:	56                   	push   esi
  40ac1c:	8b f1                	mov    esi,ecx
  40ac1e:	57                   	push   edi
  40ac1f:	6a 01                	push   0x1
  40ac21:	8d 6e 0c             	lea    ebp,[esi+0xc]
  40ac24:	55                   	push   ebp
  40ac25:	50                   	push   eax
  40ac26:	e8 85 79 ff ff       	call   0x4025b0
  40ac2b:	8b c8                	mov    ecx,eax
  40ac2d:	b8 d3 8d 13 73       	mov    eax,0x73138dd3
  40ac32:	f7 e1                	mul    ecx
  40ac34:	8b fa                	mov    edi,edx
  40ac36:	c1 ef 09             	shr    edi,0x9
  40ac39:	89 3e                	mov    DWORD PTR [esi],edi
  40ac3b:	8d 14 fd 00 00 00 00 	lea    edx,[edi*8+0x0]
  40ac42:	2b d7                	sub    edx,edi
  40ac44:	8d 04 d5 04 00 00 00 	lea    eax,[edx*8+0x4]
  40ac4b:	50                   	push   eax
  40ac4c:	e8 0e 6a 04 00       	call   0x45165f
  40ac51:	83 c4 10             	add    esp,0x10
  40ac54:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  40ac58:	33 db                	xor    ebx,ebx
  40ac5a:	3b c3                	cmp    eax,ebx
  40ac5c:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  40ac60:	74 18                	je     0x40ac7a
  40ac62:	68 80 8d 40 00       	push   0x408d80
  40ac67:	68 70 8d 40 00       	push   0x408d70
  40ac6c:	8d 58 04             	lea    ebx,[eax+0x4]
  40ac6f:	57                   	push   edi
  40ac70:	6a 38                	push   0x38
  40ac72:	53                   	push   ebx
  40ac73:	89 38                	mov    DWORD PTR [eax],edi
  40ac75:	e8 55 75 04 00       	call   0x4521cf
  40ac7a:	8b 06                	mov    eax,DWORD PTR [esi]
  40ac7c:	33 ff                	xor    edi,edi
  40ac7e:	85 c0                	test   eax,eax
  40ac80:	c7 44 24 18 ff ff ff 	mov    DWORD PTR [esp+0x18],0xffffffff
  40ac87:	ff 
  40ac88:	89 5e 10             	mov    DWORD PTR [esi+0x10],ebx
  40ac8b:	7e 32                	jle    0x40acbf
  40ac8d:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  40ac90:	33 db                	xor    ebx,ebx
  40ac92:	33 ed                	xor    ebp,ebp
  40ac94:	8d 0c 03             	lea    ecx,[ebx+eax*1]
  40ac97:	57                   	push   edi
  40ac98:	51                   	push   ecx
  40ac99:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40ac9c:	03 cd                	add    ecx,ebp
  40ac9e:	e8 dd e1 ff ff       	call   0x408e80
  40aca3:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40aca6:	80 3c 03 00          	cmp    BYTE PTR [ebx+eax*1],0x0
  40acaa:	74 03                	je     0x40acaf
  40acac:	89 7e 04             	mov    DWORD PTR [esi+0x4],edi
  40acaf:	8b 0e                	mov    ecx,DWORD PTR [esi]
  40acb1:	47                   	inc    edi
  40acb2:	83 c5 38             	add    ebp,0x38
  40acb5:	81 c3 73 04 00 00    	add    ebx,0x473
  40acbb:	3b f9                	cmp    edi,ecx
  40acbd:	7c d5                	jl     0x40ac94
  40acbf:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  40acc3:	5f                   	pop    edi
  40acc4:	5e                   	pop    esi
  40acc5:	5d                   	pop    ebp
  40acc6:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40accd:	5b                   	pop    ebx
  40acce:	83 c4 0c             	add    esp,0xc
  40acd1:	c2 04 00             	ret    0x4
  40acd4:	90                   	nop
  40acd5:	90                   	nop
  40acd6:	90                   	nop
  40acd7:	90                   	nop
  40acd8:	90                   	nop
  40acd9:	90                   	nop
  40acda:	90                   	nop
  40acdb:	90                   	nop
  40acdc:	90                   	nop
  40acdd:	90                   	nop
  40acde:	90                   	nop
  40acdf:	90                   	nop
  40ace0:	8b c1                	mov    eax,ecx
  40ace2:	33 c9                	xor    ecx,ecx
  40ace4:	89 08                	mov    DWORD PTR [eax],ecx
  40ace6:	89 48 04             	mov    DWORD PTR [eax+0x4],ecx
  40ace9:	89 48 0c             	mov    DWORD PTR [eax+0xc],ecx
  40acec:	89 48 10             	mov    DWORD PTR [eax+0x10],ecx
  40acef:	c3                   	ret
