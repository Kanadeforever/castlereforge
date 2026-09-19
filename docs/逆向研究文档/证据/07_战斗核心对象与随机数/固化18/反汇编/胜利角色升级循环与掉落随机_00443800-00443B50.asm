
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443820 <.text+0x42820>:
  443820:	0d c8 fc 89 00       	or     eax,0x89fcc8
  443825:	e8 96 ec fc ff       	call   0x4124c0
  44382a:	8d be 50 7b 00 00    	lea    edi,[esi+0x7b50]
  443830:	c7 44 24 18 08 00 00 	mov    DWORD PTR [esp+0x18],0x8
  443837:	00 
  443838:	8b 47 30             	mov    eax,DWORD PTR [edi+0x30]
  44383b:	85 c0                	test   eax,eax
  44383d:	0f 8e 52 01 00 00    	jle    0x443995
  443843:	8b 07                	mov    eax,DWORD PTR [edi]
  443845:	85 c0                	test   eax,eax
  443847:	0f 8e 48 01 00 00    	jle    0x443995
  44384d:	89 87 e0 fe ff ff    	mov    DWORD PTR [edi-0x120],eax
  443853:	8b 47 24             	mov    eax,DWORD PTR [edi+0x24]
  443856:	03 c3                	add    eax,ebx
  443858:	89 47 24             	mov    DWORD PTR [edi+0x24],eax
  44385b:	8b 15 b4 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b4
  443861:	8b 4f 20             	mov    ecx,DWORD PTR [edi+0x20]
  443864:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  443867:	3b 88 f4 01 00 00    	cmp    ecx,DWORD PTR [eax+0x1f4]
  44386d:	0f 8d 22 01 00 00    	jge    0x443995
  443873:	8d 54 24 38          	lea    edx,[esp+0x38]
  443877:	8b ce                	mov    ecx,esi
  443879:	52                   	push   edx
  44387a:	57                   	push   edi
  44387b:	e8 e0 02 00 00       	call   0x443b60
  443880:	84 c0                	test   al,al
  443882:	0f 84 0d 01 00 00    	je     0x443995
  443888:	33 ed                	xor    ebp,ebp
  44388a:	8d 8f b4 f7 ff ff    	lea    ecx,[edi-0x84c]
  443890:	55                   	push   ebp
  443891:	55                   	push   ebp
  443892:	6a 74                	push   0x74
  443894:	e8 87 e8 fd ff       	call   0x422120
  443899:	b8 39 8e e3 38       	mov    eax,0x38e38e39
  44389e:	89 6c 24 7c          	mov    DWORD PTR [esp+0x7c],ebp
  4438a2:	f7 2d 04 24 8e 00    	imul   DWORD PTR ds:0x8e2404
  4438a8:	d1 fa                	sar    edx,1
  4438aa:	8b c2                	mov    eax,edx
  4438ac:	c1 e8 1f             	shr    eax,0x1f
  4438af:	03 d0                	add    edx,eax
  4438b1:	85 d2                	test   edx,edx
  4438b3:	0f 8e c2 00 00 00    	jle    0x44397b
  4438b9:	8b 0d 00 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2400
  4438bf:	33 f6                	xor    esi,esi
  4438c1:	8b 07                	mov    eax,DWORD PTR [edi]
  4438c3:	83 f8 01             	cmp    eax,0x1
  4438c6:	75 10                	jne    0x4438d8
  4438c8:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4438ce:	8b 9a f8 00 00 00    	mov    ebx,DWORD PTR [edx+0xf8]
  4438d4:	85 db                	test   ebx,ebx
  4438d6:	75 7b                	jne    0x443953
  4438d8:	39 04 0e             	cmp    DWORD PTR [esi+ecx*1],eax
  4438db:	75 76                	jne    0x443953
  4438dd:	8b 47 20             	mov    eax,DWORD PTR [edi+0x20]
  4438e0:	8b 54 0e 04          	mov    edx,DWORD PTR [esi+ecx*1+0x4]
  4438e4:	3b c2                	cmp    eax,edx
  4438e6:	7c 37                	jl     0x44391f
  4438e8:	8b 57 54             	mov    edx,DWORD PTR [edi+0x54]
  4438eb:	8b 5c 0e 0c          	mov    ebx,DWORD PTR [esi+ecx*1+0xc]
  4438ef:	3b d3                	cmp    edx,ebx
  4438f1:	7c 2c                	jl     0x44391f
  4438f3:	8b 57 58             	mov    edx,DWORD PTR [edi+0x58]
  4438f6:	8b 5c 0e 10          	mov    ebx,DWORD PTR [esi+ecx*1+0x10]
  4438fa:	3b d3                	cmp    edx,ebx
  4438fc:	7c 21                	jl     0x44391f
  4438fe:	8b 57 5c             	mov    edx,DWORD PTR [edi+0x5c]
  443901:	8b 5c 0e 14          	mov    ebx,DWORD PTR [esi+ecx*1+0x14]
  443905:	3b d3                	cmp    edx,ebx
  443907:	7c 16                	jl     0x44391f
  443909:	8b 57 60             	mov    edx,DWORD PTR [edi+0x60]
  44390c:	8b 5c 0e 18          	mov    ebx,DWORD PTR [esi+ecx*1+0x18]
  443910:	3b d3                	cmp    edx,ebx
  443912:	7c 0b                	jl     0x44391f
  443914:	8b 57 64             	mov    edx,DWORD PTR [edi+0x64]
  443917:	8b 5c 0e 1c          	mov    ebx,DWORD PTR [esi+ecx*1+0x1c]
  44391b:	3b d3                	cmp    edx,ebx
  44391d:	7d 06                	jge    0x443925
  44391f:	3b 44 0e 08          	cmp    eax,DWORD PTR [esi+ecx*1+0x8]
  443923:	7c 2e                	jl     0x443953
  443925:	8b 5c 0e 20          	mov    ebx,DWORD PTR [esi+ecx*1+0x20]
  443929:	8d 8f b4 f7 ff ff    	lea    ecx,[edi-0x84c]
  44392f:	53                   	push   ebx
  443930:	e8 cb f7 fd ff       	call   0x423100
  443935:	84 c0                	test   al,al
  443937:	74 14                	je     0x44394d
  443939:	8b 44 24 7c          	mov    eax,DWORD PTR [esp+0x7c]
  44393d:	89 9c 84 80 00 00 00 	mov    DWORD PTR [esp+eax*4+0x80],ebx
  443944:	8b 44 24 7c          	mov    eax,DWORD PTR [esp+0x7c]
  443948:	40                   	inc    eax
  443949:	89 44 24 7c          	mov    DWORD PTR [esp+0x7c],eax
  44394d:	8b 0d 00 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2400
  443953:	b8 39 8e e3 38       	mov    eax,0x38e38e39
  443958:	45                   	inc    ebp
  443959:	f7 2d 04 24 8e 00    	imul   DWORD PTR ds:0x8e2404
  44395f:	d1 fa                	sar    edx,1
  443961:	8b c2                	mov    eax,edx
  443963:	83 c6 24             	add    esi,0x24
  443966:	c1 e8 1f             	shr    eax,0x1f
  443969:	03 d0                	add    edx,eax
  44396b:	3b ea                	cmp    ebp,edx
  44396d:	0f 8c 4e ff ff ff    	jl     0x4438c1
  443973:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443977:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  44397b:	8b 15 c8 fc 89 00    	mov    edx,DWORD PTR ds:0x89fcc8
  443981:	8d 4c 24 38          	lea    ecx,[esp+0x38]
  443985:	51                   	push   ecx
  443986:	8b 8a 00 06 00 00    	mov    ecx,DWORD PTR [edx+0x600]
  44398c:	e8 cf f2 fc ff       	call   0x412c60
  443991:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  443995:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  443999:	81 c7 e4 0d 00 00    	add    edi,0xde4
  44399f:	48                   	dec    eax
  4439a0:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4439a4:	0f 85 8e fe ff ff    	jne    0x443838
  4439aa:	a1 b0 b2 46 00       	mov    eax,ds:0x46b2b0
  4439af:	8b                   	.byte 0x8b
