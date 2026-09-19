
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00434920 <.text+0x33920>:
  434920:	6a ff                	push   0xffffffff
  434922:	68 7d ef 45 00       	push   0x45ef7d
  434927:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43492d:	50                   	push   eax
  43492e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  434935:	83 ec 24             	sub    esp,0x24
  434938:	53                   	push   ebx
  434939:	55                   	push   ebp
  43493a:	56                   	push   esi
  43493b:	8b f1                	mov    esi,ecx
  43493d:	57                   	push   edi
  43493e:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  434942:	e8 c9 c5 ff ff       	call   0x430f10
  434947:	bb 01 00 00 00       	mov    ebx,0x1
  43494c:	33 ed                	xor    ebp,ebp
  43494e:	53                   	push   ebx
  43494f:	8d 8e 7c 05 00 00    	lea    ecx,[esi+0x57c]
  434955:	89 6c 24 40          	mov    DWORD PTR [esp+0x40],ebp
  434959:	e8 92 cd 00 00       	call   0x4416f0
  43495e:	68 64 03 00 00       	push   0x364
  434963:	88 5c 24 40          	mov    BYTE PTR [esp+0x40],bl
  434967:	c7 06 24 0e 46 00    	mov    DWORD PTR [esi],0x460e24
  43496d:	e8 ed cc 01 00       	call   0x45165f
  434972:	83 c4 04             	add    esp,0x4
  434975:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  434979:	3b c5                	cmp    eax,ebp
  43497b:	c6 44 24 3c 02       	mov    BYTE PTR [esp+0x3c],0x2
  434980:	74 09                	je     0x43498b
  434982:	8b c8                	mov    ecx,eax
  434984:	e8 77 99 00 00       	call   0x43e300
  434989:	8b e8                	mov    ebp,eax
  43498b:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  434990:	88 5c 24 3c          	mov    BYTE PTR [esp+0x3c],bl
  434994:	8b 48 68             	mov    ecx,DWORD PTR [eax+0x68]
  434997:	8b 50 6c             	mov    edx,DWORD PTR [eax+0x6c]
  43499a:	8b 78 28             	mov    edi,DWORD PTR [eax+0x28]
  43499d:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  4349a1:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  4349a4:	8b 40 3c             	mov    eax,DWORD PTR [eax+0x3c]
  4349a7:	50                   	push   eax
  4349a8:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  4349ab:	51                   	push   ecx
  4349ac:	52                   	push   edx
  4349ad:	8d 0c 48             	lea    ecx,[eax+ecx*2]
  4349b0:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  4349b3:	51                   	push   ecx
  4349b4:	8d 0c 50             	lea    ecx,[eax+edx*2]
  4349b7:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  4349bb:	51                   	push   ecx
  4349bc:	52                   	push   edx
  4349bd:	8b cd                	mov    ecx,ebp
  4349bf:	e8 cc a4 00 00       	call   0x43ee90
  4349c4:	85 ed                	test   ebp,ebp
  4349c6:	74 10                	je     0x4349d8
  4349c8:	8b cd                	mov    ecx,ebp
  4349ca:	e8 21 9a 00 00       	call   0x43e3f0
  4349cf:	55                   	push   ebp
  4349d0:	e8 7b cb 01 00       	call   0x451550
  4349d5:	83 c4 04             	add    esp,0x4
  4349d8:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  4349dd:	88 98 48 02 00 00    	mov    BYTE PTR [eax+0x248],bl
  4349e3:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  4349e9:	e8 92 64 00 00       	call   0x43ae80
  4349ee:	6a 10                	push   0x10
  4349f0:	e8 6a cc 01 00       	call   0x45165f
  4349f5:	83 c4 04             	add    esp,0x4
  4349f8:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  4349fc:	85 c0                	test   eax,eax
  4349fe:	c6 44 24 3c 03       	mov    BYTE PTR [esp+0x3c],0x3
  434a03:	74 09                	je     0x434a0e
  434a05:	8b c8                	mov    ecx,eax
  434a07:	e8 14 c8 fc ff       	call   0x401220
  434a0c:	eb 02                	jmp    0x434a10
  434a0e:	33 c0                	xor    eax,eax
  434a10:	8b c8                	mov    ecx,eax
  434a12:	88 5c 24 3c          	mov    BYTE PTR [esp+0x3c],bl
  434a16:	a3 d4 01 8b 00       	mov    ds:0x8b01d4,eax
  434a1b:	e8 a0 83 ff ff       	call   0x42cdc0
  434a20:	84 c0                	test   al,al
  434a22:	75 27                	jne    0x434a4b
  434a24:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  434a2a:	68 98 b0 46 00       	push   0x46b098
  434a2f:	6a 00                	push   0x0
  434a31:	e8 3a d4 ff ff       	call   0x431e70
  434a36:	8b c6                	mov    eax,esi
  434a38:	5f                   	pop    edi
  434a39:	5e                   	pop    esi
  434a3a:	5d                   	pop    ebp
  434a3b:	5b                   	pop    ebx
  434a3c:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  434a40:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  434a47:	83 c4 30             	add    esp,0x30
  434a4a:	c3                   	ret
  434a4b:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  434a51:	33 c0                	xor    eax,eax
  434a53:	8b 49 3c             	mov    ecx,DWORD PTR [ecx+0x3c]
  434a56:	85 c9                	test   ecx,ecx
  434a58:	89 8e ec 05 00 00    	mov    DWORD PTR [esi+0x5ec],ecx
  434a5e:	7e 28                	jle    0x434a88
  434a60:	b9 68 00 00 00       	mov    ecx,0x68
  434a65:	8d 96 c8 05 00 00    	lea    edx,[esi+0x5c8]
  434a6b:	8b 3d 48 1c 8e 00    	mov    edi,DWORD PTR ds:0x8e1c48
  434a71:	40                   	inc    eax
  434a72:	83 c2 04             	add    edx,0x4
  434a75:	8b 3c 39             	mov    edi,DWORD PTR [ecx+edi*1]
  434a78:	83 c1 04             	add    ecx,0x4
  434a7b:	89 7a fc             	mov    DWORD PTR [edx-0x4],edi
  434a7e:	8b be ec 05 00 00    	mov    edi,DWORD PTR [esi+0x5ec]
  434a84:	3b c7                	cmp    eax,edi
  434a86:	7c e3                	jl     0x434a6b
  434a88:	6a 00                	push   0x0
  434a8a:	68 80 84 1e 00       	push   0x1e8480
  434a8f:	56                   	push   esi
  434a90:	8b ce                	mov    ecx,esi
  434a92:	e8 99 f9 ff ff       	call   0x434430
  434a97:	8d 8e 18 02 00 00    	lea    ecx,[esi+0x218]
  434a9d:	e8 8e 02 fd ff       	call   0x404d30
  434aa2:	68 6c 92 46 00       	push   0x46926c
  434aa7:	6a 00                	push   0x0
  434aa9:	89 44 24 38          	mov    DWORD PTR [esp+0x38],eax
  434aad:	6a 00                	push   0x0
  434aaf:	8d 96 68 05 00 00    	lea    edx,[esi+0x568]
  434ab5:	8d 86 6c 05 00 00    	lea    eax,[esi+0x56c]
  434abb:	6a 00                	push   0x0
  434abd:	6a 00                	push   0x0
  434abf:	8b ce                	mov    ecx,esi
  434ac1:	89 54 24 30          	mov    DWORD PTR [esp+0x30],edx
  434ac5:	89 44 24 34          	mov    DWORD PTR [esp+0x34],eax
  434ac9:	e8 12 c6 ff ff       	call   0x4310e0
  434ace:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  434ad4:	6a 00                	push   0x0
  434ad6:	e8 15 9c 00 00       	call   0x43e6f0
  434adb:	68 a0 05 00 00       	push   0x5a0
  434ae0:	e8 7a cb 01 00       	call   0x45165f
  434ae5:	83 c4 04             	add    esp,0x4
  434ae8:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  434aec:	85 c0                	test   eax,eax
  434aee:	c6 44 24 3c 04       	mov    BYTE PTR [esp+0x3c],0x4
  434af3:	74 09                	je     0x434afe
  434af5:	8b c8                	mov    ecx,eax
  434af7:	e8 24 24 00 00       	call   0x436f20
  434afc:	eb 02                	jmp    0x434b00
  434afe:	33 c0                	xor    eax,eax
  434b00:	89 86 20 06 00 00    	mov    DWORD PTR [esi+0x620],eax
  434b06:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  434b0c:	88 5c 24 3c          	mov    BYTE PTR [esp+0x3c],bl
  434b10:	8b 91 08 01 00 00    	mov    edx,DWORD PTR [ecx+0x108]
  434b16:	85 d2                	test   edx,edx
  434b18:	74 14                	je     0x434b2e
  434b1a:	68 84 b0 46 00       	push   0x46b084
  434b1f:	6a 00                	push   0x0
  434b21:	6a 00                	push   0x0
  434b23:	6a 69                	push   0x69
  434b25:	6a 41                	push   0x41
  434b27:	8b c8                	mov    ecx,eax
  434b29:	e8 b2 c5 ff ff       	call   0x4310e0
  434b2e:	6a 2c                	push   0x2c
  434b30:	e8 2a cb 01        	call   0x45165f
