; 固化38证据：DDDES +0x11 -> 多媒体定时器 -> WM_TIMER。
; 仅为原始RPG.exe机器码反汇编摘录，不含人工伪代码。

/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401570 <.text+0x570>:
  401570:	8b 51 28             	mov    edx,DWORD PTR [ecx+0x28]
  401573:	8b 40 0d             	mov    eax,DWORD PTR [eax+0xd]
  401576:	89 42 08             	mov    DWORD PTR [edx+0x8],eax
  401579:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  40157f:	e8 dc 43 00 00       	call   0x405960
  401584:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  401587:	8b 06                	mov    eax,DWORD PTR [esi]
  401589:	33 d2                	xor    edx,edx
  40158b:	8a 11                	mov    dl,BYTE PTR [ecx]
  40158d:	8b 0d b4 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6b4
  401593:	52                   	push   edx
  401594:	50                   	push   eax
  401595:	e8 a6 37 00 00       	call   0x404d40
  40159a:	8b 0d 00 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f600


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00404a60 <.text+0x3a60>:
  404a60:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  404a64:	56                   	push   esi
  404a65:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  404a69:	a3 ac f6 46 00       	mov    ds:0x46f6ac,eax
  404a6e:	57                   	push   edi
  404a6f:	c7 01 00 00 00 00    	mov    DWORD PTR [ecx],0x0
  404a75:	8b 3d 0c 85 46 00    	mov    edi,DWORD PTR ds:0x46850c
  404a7b:	33 d2                	xor    edx,edx
  404a7d:	8b c7                	mov    eax,edi
  404a7f:	89 35 a0 f6 46 00    	mov    DWORD PTR ds:0x46f6a0,esi
  404a85:	f7 f6                	div    esi
  404a87:	33 d2                	xor    edx,edx
  404a89:	a3 90 f6 46 00       	mov    ds:0x46f690,eax
  404a8e:	b8 e8 03 00 00       	mov    eax,0x3e8
  404a93:	f7 f7                	div    edi
  404a95:	a3 b8 f6 46 00       	mov    ds:0x46f6b8,eax
  404a9a:	e8 11 00 00 00       	call   0x404ab0
  404a9f:	5f                   	pop    edi
  404aa0:	5e                   	pop    esi
  404aa1:	c2 08 00             	ret    0x8
  404aa4:	90                   	nop
  404aa5:	90                   	nop
  404aa6:	90                   	nop
  404aa7:	90                   	nop
  404aa8:	90                   	nop
  404aa9:	90                   	nop
  404aaa:	90                   	nop
  404aab:	90                   	nop
  404aac:	90                   	nop
  404aad:	90                   	nop
  404aae:	90                   	nop
  404aaf:	90                   	nop
  404ab0:	83 ec 08             	sub    esp,0x8
  404ab3:	53                   	push   ebx
  404ab4:	56                   	push   esi
  404ab5:	8b f1                	mov    esi,ecx
  404ab7:	33 db                	xor    ebx,ebx
  404ab9:	39 1e                	cmp    DWORD PTR [esi],ebx
  404abb:	75 61                	jne    0x404b1e
  404abd:	8d 44 24 08          	lea    eax,[esp+0x8]
  404ac1:	6a 08                	push   0x8
  404ac3:	50                   	push   eax
  404ac4:	88 1d 94 f6 46 00    	mov    BYTE PTR ds:0x46f694,bl
  404aca:	89 1d a8 f6 46 00    	mov    DWORD PTR ds:0x46f6a8,ebx
  404ad0:	89 1d 9c f6 46 00    	mov    DWORD PTR ds:0x46f69c,ebx
  404ad6:	89 1d a4 f6 46 00    	mov    DWORD PTR ds:0x46f6a4,ebx
  404adc:	89 1d 98 f6 46 00    	mov    DWORD PTR ds:0x46f698,ebx
  404ae2:	ff 15 18 02 46 00    	call   DWORD PTR ds:0x460218
  404ae8:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  404aec:	51                   	push   ecx
  404aed:	ff 15 14 02 46 00    	call   DWORD PTR ds:0x460214
  404af3:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  404af7:	a1 b8 f6 46 00       	mov    eax,ds:0x46f6b8
  404afc:	33 d2                	xor    edx,edx
  404afe:	6a 01                	push   0x1
  404b00:	f7 f1                	div    ecx
  404b02:	53                   	push   ebx
  404b03:	68 90 4c 40 00       	push   0x404c90
  404b08:	51                   	push   ecx
  404b09:	50                   	push   eax
  404b0a:	ff 15 10 02 46 00    	call   DWORD PTR ds:0x460210
  404b10:	3b c3                	cmp    eax,ebx
  404b12:	89 06                	mov    DWORD PTR [esi],eax
  404b14:	75 08                	jne    0x404b1e
  404b16:	5e                   	pop    esi
  404b17:	32 c0                	xor    al,al
  404b19:	5b                   	pop    ebx
  404b1a:	83 c4 08             	add    esp,0x8
  404b1d:	c3                   	ret
  404b1e:	5e                   	pop    esi
  404b1f:	b0 01                	mov    al,0x1
  404b21:	5b                   	pop    ebx
  404b22:	83 c4 08             	add    esp,0x8
  404b25:	c3                   	ret
  404b26:	90                   	nop
  404b27:	90                   	nop
  404b28:	90                   	nop
  404b29:	90                   	nop
  404b2a:	90                   	nop
  404b2b:	90                   	nop
  404b2c:	90                   	nop
  404b2d:	90                   	nop
  404b2e:	90                   	nop
  404b2f:	90                   	nop
  404b30:	83 ec 08             	sub    esp,0x8
  404b33:	56                   	push   esi
  404b34:	8b f1                	mov    esi,ecx
  404b36:	8b 06                	mov    eax,DWORD PTR [esi]
  404b38:	85 c0                	test   eax,eax
  404b3a:	74 25                	je     0x404b61
  404b3c:	50                   	push   eax
  404b3d:	ff 15 2c 02 46 00    	call   DWORD PTR ds:0x46022c
  404b43:	8d 44 24 04          	lea    eax,[esp+0x4]
  404b47:	6a 08                	push   0x8
  404b49:	50                   	push   eax
  404b4a:	ff 15 18 02 46 00    	call   DWORD PTR ds:0x460218
  404b50:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  404b54:	51                   	push   ecx
  404b55:	ff 15 24 02 46 00    	call   DWORD PTR ds:0x460224
  404b5b:	c7 06 00 00 00 00    	mov    DWORD PTR [esi],0x0
  404b61:	5e                   	pop    esi
  404b62:	83 c4 08             	add    esp,0x8
  404b65:	c3                   	ret
  404b66:	90                   	nop
  404b67:	90                   	nop
  404b68:	90                   	nop
  404b69:	90                   	nop
  404b6a:	90                   	nop
  404b6b:	90                   	nop
  404b6c:	90                   	nop
  404b6d:	90                   	nop
  404b6e:	90                   	nop
  404b6f:	90                   	nop
  404b70:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  404b74:	83 f8 06             	cmp    eax,0x6
  404b77:	77 47                	ja     0x404bc0
  404b79:	ff 24 85 c8 4b 40 00 	jmp    DWORD PTR [eax*4+0x404bc8]
  404b80:	8a 0d 94 f6 46 00    	mov    cl,BYTE PTR ds:0x46f694
  404b86:	33 c0                	xor    eax,eax
  404b88:	84 c9                	test   cl,cl
  404b8a:	0f 95 c0             	setne  al
  404b8d:	c2 04 00             	ret    0x4
  404b90:	a1 a0 f6 46 00       	mov    eax,ds:0x46f6a0
  404b95:	c2 04 00             	ret    0x4
  404b98:	a1 98 f6 46 00       	mov    eax,ds:0x46f698
  404b9d:	c2 04 00             	ret    0x4
  404ba0:	a1 b8 f6 46 00       	mov    eax,ds:0x46f6b8
  404ba5:	c2 04 00             	ret    0x4
  404ba8:	a1 a8 f6 46 00       	mov    eax,ds:0x46f6a8
  404bad:	c2 04 00             	ret    0x4
  404bb0:	a1 9c f6 46 00       	mov    eax,ds:0x46f69c
  404bb5:	c2 04 00             	ret    0x4
  404bb8:	a1 a4 f6 46 00       	mov    eax,ds:0x46f6a4
  404bbd:	c2 04 00             	ret    0x4
  404bc0:	83 c8 ff             	or     eax,0xffffffff
  404bc3:	c2 04 00             	ret    0x4
  404bc6:	8b ff                	mov    edi,edi
  404bc8:	80 4b 40 00          	or     BYTE PTR [ebx+0x40],0x0
  404bcc:	90                   	nop
  404bcd:	4b                   	dec    ebx
  404bce:	40                   	inc    eax
  404bcf:	00 98 4b 40 00 a0    	add    BYTE PTR [eax-0x5fffbfb5],bl
  404bd5:	4b                   	dec    ebx
  404bd6:	40                   	inc    eax
  404bd7:	00 a8 4b 40 00 b0    	add    BYTE PTR [eax-0x4fffbfb5],ch
  404bdd:	4b                   	dec    ebx
  404bde:	40                   	inc    eax
  404bdf:	00 b8 4b 40 00 90    	add    BYTE PTR [eax-0x6fffbfb5],bh
  404be5:	90                   	nop
  404be6:	90                   	nop
  404be7:	90                   	nop
  404be8:	90                   	nop
  404be9:	90                   	nop
  404bea:	90                   	nop
  404beb:	90                   	nop
  404bec:	90                   	nop
  404bed:	90                   	nop
  404bee:	90                   	nop
  404bef:	90                   	nop
  404bf0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  404bf4:	83 f8 06             	cmp    eax,0x6
  404bf7:	77 70                	ja     0x404c69
  404bf9:	ff 24 85 6c 4c 40 00 	jmp    DWORD PTR [eax*4+0x404c6c]
  404c00:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  404c04:	85 c0                	test   eax,eax
  404c06:	0f 95 c0             	setne  al
  404c09:	a2 94 f6 46 00       	mov    ds:0x46f694,al
  404c0e:	c2 08 00             	ret    0x8
  404c11:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  404c15:	a1 0c 85 46 00       	mov    eax,ds:0x46850c
  404c1a:	33 d2                	xor    edx,edx
  404c1c:	89 0d a0 f6 46 00    	mov    DWORD PTR ds:0x46f6a0,ecx
  404c22:	f7 f1                	div    ecx
  404c24:	a3 90 f6 46 00       	mov    ds:0x46f690,eax
  404c29:	c2 08 00             	ret    0x8
  404c2c:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  404c30:	89 0d 98 f6 46 00    	mov    DWORD PTR ds:0x46f698,ecx
  404c36:	c2 08 00             	ret    0x8
  404c39:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  404c3d:	89 15 b8 f6 46 00    	mov    DWORD PTR ds:0x46f6b8,edx
  404c43:	c2 08 00             	ret    0x8
  404c46:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  404c4a:	a3 a8 f6 46 00       	mov    ds:0x46f6a8,eax
  404c4f:	c2 08 00             	ret    0x8
  404c52:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  404c56:	89 0d 9c f6 46 00    	mov    DWORD PTR ds:0x46f69c,ecx
  404c5c:	c2 08 00             	ret    0x8
  404c5f:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  404c63:	89 15 a4 f6 46 00    	mov    DWORD PTR ds:0x46f6a4,edx
  404c69:	c2 08 00             	ret    0x8
  404c6c:	00 4c 40 00          	add    BYTE PTR [eax+eax*2+0x0],cl
  404c70:	11 4c 40 00          	adc    DWORD PTR [eax+eax*2+0x0],ecx
  404c74:	2c 4c                	sub    al,0x4c
  404c76:	40                   	inc    eax
  404c77:	00 39                	add    BYTE PTR [ecx],bh
  404c79:	4c                   	dec    esp
  404c7a:	40                   	inc    eax
  404c7b:	00 46 4c             	add    BYTE PTR [esi+0x4c],al
  404c7e:	40                   	inc    eax
  404c7f:	00 52 4c             	add    BYTE PTR [edx+0x4c],dl
  404c82:	40                   	inc    eax
  404c83:	00 5f 4c             	add    BYTE PTR [edi+0x4c],bl
  404c86:	40                   	inc    eax
  404c87:	00 90 90 90 90 90    	add    BYTE PTR [eax-0x6f6f6f70],dl
  404c8d:	90                   	nop
  404c8e:	90                   	nop
  404c8f:	90                   	nop
  404c90:	8b 15 98 f6 46 00    	mov    edx,DWORD PTR ds:0x46f698
  404c96:	a1 a8 f6 46 00       	mov    eax,ds:0x46f6a8
  404c9b:	8b 0d 0c 85 46 00    	mov    ecx,DWORD PTR ds:0x46850c
  404ca1:	42                   	inc    edx
  404ca2:	89 15 98 f6 46 00    	mov    DWORD PTR ds:0x46f698,edx
  404ca8:	8b 15 b0 f6 46 00    	mov    edx,DWORD PTR ds:0x46f6b0
  404cae:	40                   	inc    eax
  404caf:	42                   	inc    edx
  404cb0:	3b c1                	cmp    eax,ecx
  404cb2:	a3 a8 f6 46 00       	mov    ds:0x46f6a8,eax
  404cb7:	89 15 b0 f6 46 00    	mov    DWORD PTR ds:0x46f6b0,edx
  404cbd:	75 1e                	jne    0x404cdd
  404cbf:	a1 9c f6 46 00       	mov    eax,ds:0x46f69c
  404cc4:	c7 05 a8 f6 46 00 00 	mov    DWORD PTR ds:0x46f6a8,0x0
  404ccb:	00 00 00 
  404cce:	a3 a4 f6 46 00       	mov    ds:0x46f6a4,eax
  404cd3:	c7 05 9c f6 46 00 00 	mov    DWORD PTR ds:0x46f69c,0x0
  404cda:	00 00 00 
  404cdd:	8b 0d b0 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6b0
  404ce3:	a1 90 f6 46 00       	mov    eax,ds:0x46f690
  404ce8:	3b c8                	cmp    ecx,eax
  404cea:	72 3b                	jb     0x404d27
  404cec:	a0 94 f6 46 00       	mov    al,ds:0x46f694
  404cf1:	84 c0                	test   al,al
  404cf3:	75 32                	jne    0x404d27
  404cf5:	8b 15 ac f6 46 00    	mov    edx,DWORD PTR ds:0x46f6ac
  404cfb:	6a 00                	push   0x0
  404cfd:	6a 00                	push   0x0
  404cff:	68 13 01 00 00       	push   0x113
  404d04:	52                   	push   edx
  404d05:	ff 15 90 01 46 00    	call   DWORD PTR ds:0x460190
  404d0b:	a1 9c f6 46 00       	mov    eax,ds:0x46f69c
  404d10:	c6 05 94 f6 46 00 01 	mov    BYTE PTR ds:0x46f694,0x1
  404d17:	40                   	inc    eax
  404d18:	c7 05 b0 f6 46 00 00 	mov    DWORD PTR ds:0x46f6b0,0x0
  404d1f:	00 00 00 
  404d22:	a3 9c f6 46 00       	mov    ds:0x46f69c,eax
  404d27:	c2 14 00             	ret    0x14
  404d2a:	90                   	nop
  404d2b:	90                   	nop
  404d2c:	90                   	nop
  404d2d:	90                   	nop
  404d2e:	90                   	nop
  404d2f:	90                   	nop
