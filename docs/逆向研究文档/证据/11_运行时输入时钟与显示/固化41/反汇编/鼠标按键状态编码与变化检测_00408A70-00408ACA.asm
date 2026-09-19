; 固化41：鼠标键状态编码器，IAT 0x4601A8=USER32.GetKeyState
  408a15:	3b c1                	cmp    eax,ecx
  408a17:	0f 9d c2             	setge  dl
  408a1a:	42                   	inc    edx
  408a1b:	eb 21                	jmp    0x408a3e
  408a1d:	85 c9                	test   ecx,ecx
  408a1f:	7c 0e                	jl     0x408a2f
  408a21:	f7 d8                	neg    eax
  408a23:	33 d2                	xor    edx,edx
  408a25:	3b c1                	cmp    eax,ecx
  408a27:	0f 9d c2             	setge  dl
  408a2a:	83 c2 05             	add    edx,0x5
  408a2d:	eb 0f                	jmp    0x408a3e
  408a2f:	f7 d9                	neg    ecx
  408a31:	f7 d8                	neg    eax
  408a33:	33 d2                	xor    edx,edx
  408a35:	3b c1                	cmp    eax,ecx
  408a37:	0f 9c c2             	setl   dl
  408a3a:	4a                   	dec    edx
  408a3b:	83 e2 07             	and    edx,0x7
  408a3e:	8b 74 24 20          	mov    esi,DWORD PTR [esp+0x20]
  408a42:	6a 02                	push   0x2
  408a44:	89 16                	mov    DWORD PTR [esi],edx
  408a46:	ff d7                	call   edi
  408a48:	0f bf c0             	movsx  eax,ax
  408a4b:	a9 00 80 ff ff       	test   eax,0xffff8000
  408a50:	74 09                	je     0x408a5b
  408a52:	8b 0e                	mov    ecx,DWORD PTR [esi]
  408a54:	8b 2c 8d 98 8b 46 00 	mov    ebp,DWORD PTR [ecx*4+0x468b98]
  408a5b:	5f                   	pop    edi
  408a5c:	8b c5                	mov    eax,ebp
  408a5e:	5e                   	pop    esi
  408a5f:	5d                   	pop    ebp
  408a60:	c3                   	ret
  408a61:	90                   	nop
  408a62:	90                   	nop
  408a63:	90                   	nop
  408a64:	90                   	nop
  408a65:	90                   	nop
  408a66:	90                   	nop
  408a67:	90                   	nop
  408a68:	90                   	nop
  408a69:	90                   	nop
  408a6a:	90                   	nop
  408a6b:	90                   	nop
  408a6c:	90                   	nop
  408a6d:	90                   	nop
  408a6e:	90                   	nop
  408a6f:	90                   	nop
  408a70:	56                   	push   esi
  408a71:	57                   	push   edi
  408a72:	8b 3d a8 01 46 00    	mov    edi,DWORD PTR ds:0x4601a8
  408a78:	6a 02                	push   0x2
  408a7a:	33 f6                	xor    esi,esi
  408a7c:	ff d7                	call   edi
  408a7e:	0f bf c0             	movsx  eax,ax
  408a81:	a9 00 80 ff ff       	test   eax,0xffff8000
  408a86:	6a 01                	push   0x1
  408a88:	74 17                	je     0x408aa1
  408a8a:	ff d7                	call   edi
  408a8c:	0f bf f0             	movsx  esi,ax
  408a8f:	81 e6 00 80 ff ff    	and    esi,0xffff8000
  408a95:	f7 de                	neg    esi
  408a97:	1b f6                	sbb    esi,esi
  408a99:	83 e6 50             	and    esi,0x50
  408a9c:	83 c6 02             	add    esi,0x2
  408a9f:	eb 12                	jmp    0x408ab3
  408aa1:	ff d7                	call   edi
  408aa3:	0f bf c8             	movsx  ecx,ax
  408aa6:	f7 c1 00 80 ff ff    	test   ecx,0xffff8000
  408aac:	74 05                	je     0x408ab3
  408aae:	be 01 00 00 00       	mov    esi,0x1
  408ab3:	3b 35 e4 f7 89 00    	cmp    esi,DWORD PTR ds:0x89f7e4
  408ab9:	74 0b                	je     0x408ac6
  408abb:	89 35 e4 f7 89 00    	mov    DWORD PTR ds:0x89f7e4,esi
  408ac1:	8b c6                	mov    eax,esi
  408ac3:	5f                   	pop    edi
  408ac4:	5e                   	pop    esi
  408ac5:	c3                   	ret
  408ac6:	5f                   	pop    edi
  408ac7:	33 c0                	xor    eax,eax
  408ac9:	5e                   	pop    esi
  408aca:	c3                   	ret
  408acb:	90                   	nop
  408acc:	90                   	nop
  408acd:	90                   	nop
  408ace:	90                   	nop
  408acf:	90                   	nop
  408ad0:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  408ad6:	6a ff                	push   0xffffffff
  408ad8:	68 3b d0 45 00       	push   0x45d03b
  408add:	50                   	push   eax
