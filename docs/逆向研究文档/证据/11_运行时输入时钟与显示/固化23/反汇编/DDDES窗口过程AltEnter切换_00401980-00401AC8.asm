
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401980 <.text+0x980>:
  401980:	8b 7d 10             	mov    edi,DWORD PTR [ebp+0x10]
  401983:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  401986:	c6 45 ef 00          	mov    BYTE PTR [ebp-0x11],0x0
  40198a:	3d 05 01 00 00       	cmp    eax,0x105
  40198f:	0f 87 93 01 00 00    	ja     0x401b28
  401995:	0f 84 0c 01 00 00    	je     0x401aa7
  40199b:	83 f8 05             	cmp    eax,0x5
  40199e:	0f 87 b3 00 00 00    	ja     0x401a57
  4019a4:	0f 84 83 00 00 00    	je     0x401a2d
  4019aa:	8b c8                	mov    ecx,eax
  4019ac:	49                   	dec    ecx
  4019ad:	74 38                	je     0x4019e7
  4019af:	49                   	dec    ecx
  4019b0:	0f 85 a3 01 00 00    	jne    0x401b59
  4019b6:	e8 05 98 00 00       	call   0x40b1c0
  4019bb:	8b 35 80 f3 46 00    	mov    esi,DWORD PTR ds:0x46f380
  4019c1:	85 f6                	test   esi,esi
  4019c3:	74 10                	je     0x4019d5
  4019c5:	8b ce                	mov    ecx,esi
  4019c7:	e8 64 f8 ff ff       	call   0x401230
  4019cc:	56                   	push   esi
  4019cd:	e8 7e fb 04 00       	call   0x451550
  4019d2:	83 c4 04             	add    esp,0x4
  4019d5:	e8 f6 f7 ff ff       	call   0x4011d0
  4019da:	6a 00                	push   0x0
  4019dc:	ff 15 dc 01 46 00    	call   DWORD PTR ds:0x4601dc
  4019e2:	e9 9f 01 00 00       	jmp    0x401b86
  4019e7:	6a 14                	push   0x14
  4019e9:	e8 71 fc 04 00       	call   0x45165f
  4019ee:	8b c8                	mov    ecx,eax
  4019f0:	83 c4 04             	add    esp,0x4
  4019f3:	89 4d 10             	mov    DWORD PTR [ebp+0x10],ecx
  4019f6:	85 c9                	test   ecx,ecx
  4019f8:	c6 45 fc 01          	mov    BYTE PTR [ebp-0x4],0x1
  4019fc:	74 09                	je     0x401a07
  4019fe:	e8 1d f8 ff ff       	call   0x401220
  401a03:	8b c8                	mov    ecx,eax
  401a05:	eb 02                	jmp    0x401a09
  401a07:	33 c9                	xor    ecx,ecx
  401a09:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  401a0c:	c6 45 fc 00          	mov    BYTE PTR [ebp-0x4],0x0
  401a10:	50                   	push   eax
  401a11:	89 0d 80 f3 46 00    	mov    DWORD PTR ds:0x46f380,ecx
  401a17:	e8 c4 fa ff ff       	call   0x4014e0
  401a1c:	c6 05 8c f3 46 00 01 	mov    BYTE PTR ds:0x46f38c,0x1
  401a23:	e8 d8 f5 ff ff       	call   0x401000
  401a28:	e9 59 01 00 00       	jmp    0x401b86
  401a2d:	83 ff 01             	cmp    edi,0x1
  401a30:	75 19                	jne    0x401a4b
  401a32:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  401a35:	57                   	push   edi
  401a36:	6a 00                	push   0x0
  401a38:	51                   	push   ecx
  401a39:	ff 15 e4 01 46 00    	call   DWORD PTR ds:0x4601e4
  401a3f:	c6 05 bc 80 46 00 00 	mov    BYTE PTR ds:0x4680bc,0x0
  401a46:	e9 3b 01 00 00       	jmp    0x401b86
  401a4b:	c6 05 bc 80 46 00 01 	mov    BYTE PTR ds:0x4680bc,0x1
  401a52:	e9 2f 01 00 00       	jmp    0x401b86
  401a57:	8b c8                	mov    ecx,eax
  401a59:	81 e9 00 01 00 00    	sub    ecx,0x100
  401a5f:	74 0c                	je     0x401a6d
  401a61:	49                   	dec    ecx
  401a62:	0f 84 1e 01 00 00    	je     0x401b86
  401a68:	e9 ec 00 00 00       	jmp    0x401b59
  401a6d:	83 ff 7b             	cmp    edi,0x7b
  401a70:	74 11                	je     0x401a83
  401a72:	8b 0d 80 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f380
  401a78:	57                   	push   edi
  401a79:	e8 72 fb ff ff       	call   0x4015f0
  401a7e:	e9 03 01 00 00       	jmp    0x401b86
  401a83:	8b 0d f4 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e24f4
  401a89:	e8 92 2d 03 00       	call   0x434820
  401a8e:	e8 0d 99 00 00       	call   0x40b3a0
  401a93:	8d 55 ef             	lea    edx,[ebp-0x11]
  401a96:	8b c8                	mov    ecx,eax
  401a98:	52                   	push   edx
  401a99:	e8 62 2e 03 00       	call   0x434900
  401a9e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  401aa1:	50                   	push   eax
  401aa2:	e9 d9 00 00 00       	jmp    0x401b80
  401aa7:	83 ff 09             	cmp    edi,0x9
  401aaa:	0f 84 d6 00 00 00    	je     0x401b86
  401ab0:	83 ff 0d             	cmp    edi,0xd
  401ab3:	0f 85 cd 00 00 00    	jne    0x401b86
  401ab9:	8b 0d 80 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f380
  401abf:	e8 4c fb ff ff       	call   0x401610
  401ac4:	e9 bd 00 00 00       	jmp    0x401b86
