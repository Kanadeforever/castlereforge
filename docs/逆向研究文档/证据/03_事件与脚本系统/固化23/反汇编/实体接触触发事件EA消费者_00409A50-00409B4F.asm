
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00409a50 <.text+0x8a50>:
  409a50:	53                   	push   ebx
  409a51:	55                   	push   ebp
  409a52:	8b 6c 24 0c          	mov    ebp,DWORD PTR [esp+0xc]
  409a56:	56                   	push   esi
  409a57:	8b f1                	mov    esi,ecx
  409a59:	57                   	push   edi
  409a5a:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  409a5d:	8b 0c a8             	mov    ecx,DWORD PTR [eax+ebp*4]
  409a60:	8b 59 6c             	mov    ebx,DWORD PTR [ecx+0x6c]
  409a63:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  409a66:	85 c9                	test   ecx,ecx
  409a68:	74 1f                	je     0x409a89
  409a6a:	6a 01                	push   0x1
  409a6c:	53                   	push   ebx
  409a6d:	e8 5e e6 ff ff       	call   0x4080d0
  409a72:	84 c0                	test   al,al
  409a74:	74 13                	je     0x409a89
  409a76:	a1 f0 8b 46 00       	mov    eax,ds:0x468bf0
  409a7b:	85 c0                	test   eax,eax
  409a7d:	75 59                	jne    0x409ad8
  409a7f:	6a 00                	push   0x0
  409a81:	e8 9a f2 ff ff       	call   0x408d20
  409a86:	83 c4 04             	add    esp,0x4
  409a89:	8b 56 1c             	mov    edx,DWORD PTR [esi+0x1c]
  409a8c:	33 ff                	xor    edi,edi
  409a8e:	42                   	inc    edx
  409a8f:	85 d2                	test   edx,edx
  409a91:	7e 61                	jle    0x409af4
  409a93:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  409a96:	8b 04 b8             	mov    eax,DWORD PTR [eax+edi*4]
  409a99:	85 c0                	test   eax,eax
  409a9b:	74 29                	je     0x409ac6
  409a9d:	3b fd                	cmp    edi,ebp
  409a9f:	74 25                	je     0x409ac6
  409aa1:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  409aa4:	8a 51 74             	mov    dl,BYTE PTR [ecx+0x74]
  409aa7:	84 d2                	test   dl,dl
  409aa9:	74 0a                	je     0x409ab5
  409aab:	8a 91 ea 00 00 00    	mov    dl,BYTE PTR [ecx+0xea]
  409ab1:	84 d2                	test   dl,dl
  409ab3:	74 11                	je     0x409ac6
  409ab5:	8b 40 6c             	mov    eax,DWORD PTR [eax+0x6c]
  409ab8:	6a 01                	push   0x1
  409aba:	53                   	push   ebx
  409abb:	8b c8                	mov    ecx,eax
  409abd:	e8 0e e6 ff ff       	call   0x4080d0
  409ac2:	84 c0                	test   al,al
  409ac4:	75 1b                	jne    0x409ae1
  409ac6:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  409ac9:	47                   	inc    edi
  409aca:	41                   	inc    ecx
  409acb:	3b f9                	cmp    edi,ecx
  409acd:	7c c4                	jl     0x409a93
  409acf:	5f                   	pop    edi
  409ad0:	5e                   	pop    esi
  409ad1:	5d                   	pop    ebp
  409ad2:	32 c0                	xor    al,al
  409ad4:	5b                   	pop    ebx
  409ad5:	c2 04 00             	ret    0x4
  409ad8:	5f                   	pop    edi
  409ad9:	5e                   	pop    esi
  409ada:	5d                   	pop    ebp
  409adb:	b0 01                	mov    al,0x1
  409add:	5b                   	pop    ebx
  409ade:	c2 04 00             	ret    0x4
  409ae1:	a1 f0 8b 46 00       	mov    eax,ds:0x468bf0
  409ae6:	85 c0                	test   eax,eax
  409ae8:	75 13                	jne    0x409afd
  409aea:	6a 00                	push   0x0
  409aec:	e8 2f f2 ff ff       	call   0x408d20
  409af1:	83 c4 04             	add    esp,0x4
  409af4:	5f                   	pop    edi
  409af5:	5e                   	pop    esi
  409af6:	5d                   	pop    ebp
  409af7:	32 c0                	xor    al,al
  409af9:	5b                   	pop    ebx
  409afa:	c2 04 00             	ret    0x4
  409afd:	e8 0e f1 ff ff       	call   0x408c10
  409b02:	39 58 6c             	cmp    DWORD PTR [eax+0x6c],ebx
  409b05:	75 24                	jne    0x409b2b
  409b07:	c7 05 d0 f7 89 00 00 	mov    DWORD PTR ds:0x89f7d0,0x0
  409b0e:	00 00 00 
  409b11:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  409b14:	8b 04 ba             	mov    eax,DWORD PTR [edx+edi*4]
  409b17:	33 d2                	xor    edx,edx
  409b19:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  409b1c:	8a 91 ea 00 00 00    	mov    dl,BYTE PTR [ecx+0xea]
  409b22:	52                   	push   edx
  409b23:	e8 08 17 00 00       	call   0x40b230
  409b28:	83 c4 04             	add    esp,0x4
  409b2b:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  409b2e:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  409b31:	5f                   	pop    edi
  409b32:	5e                   	pop    esi
  409b33:	5d                   	pop    ebp
  409b34:	8b 51 70             	mov    edx,DWORD PTR [ecx+0x70]
  409b37:	5b                   	pop    ebx
  409b38:	8a 42 74             	mov    al,BYTE PTR [edx+0x74]
  409b3b:	84 c0                	test   al,al
  409b3d:	0f 94 c0             	sete   al
  409b40:	c2 04 00             	ret    0x4
  409b43:	90                   	nop
  409b44:	90                   	nop
  409b45:	90                   	nop
  409b46:	90                   	nop
  409b47:	90                   	nop
  409b48:	90                   	nop
  409b49:	90                   	nop
  409b4a:	90                   	nop
  409b4b:	90                   	nop
  409b4c:	90                   	nop
  409b4d:	90                   	nop
  409b4e:	90                   	nop
  409b4f:	90                   	nop
