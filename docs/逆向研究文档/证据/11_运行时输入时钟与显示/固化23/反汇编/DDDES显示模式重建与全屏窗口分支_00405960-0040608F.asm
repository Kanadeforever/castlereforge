
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00405960 <.text+0x4960>:
  405960:	56                   	push   esi
  405961:	8b f1                	mov    esi,ecx
  405963:	57                   	push   edi
  405964:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  405967:	8b 4e 70             	mov    ecx,DWORD PTR [esi+0x70]
  40596a:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  40596d:	8d 0c 4a             	lea    ecx,[edx+ecx*2]
  405970:	8b 56 6c             	mov    edx,DWORD PTR [esi+0x6c]
  405973:	89 4e 5c             	mov    DWORD PTR [esi+0x5c],ecx
  405976:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  405979:	8d 14 51             	lea    edx,[ecx+edx*2]
  40597c:	8b 4e 5c             	mov    ecx,DWORD PTR [esi+0x5c]
  40597f:	89 56 60             	mov    DWORD PTR [esi+0x60],edx
  405982:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  405985:	0f af 4e 60          	imul   ecx,DWORD PTR [esi+0x60]
  405989:	99                   	cdq
  40598a:	83 e2 07             	and    edx,0x7
  40598d:	03 c2                	add    eax,edx
  40598f:	c1 f8 03             	sar    eax,0x3
  405992:	0f af c8             	imul   ecx,eax
  405995:	51                   	push   ecx
  405996:	89 46 64             	mov    DWORD PTR [esi+0x64],eax
  405999:	e8 c1 bc 04 00       	call   0x45165f
  40599e:	8b 4e 64             	mov    ecx,DWORD PTR [esi+0x64]
  4059a1:	8b f8                	mov    edi,eax
  4059a3:	0f af 4e 5c          	imul   ecx,DWORD PTR [esi+0x5c]
  4059a7:	0f af 4e 60          	imul   ecx,DWORD PTR [esi+0x60]
  4059ab:	8b d1                	mov    edx,ecx
  4059ad:	33 c0                	xor    eax,eax
  4059af:	c1 e9 02             	shr    ecx,0x2
  4059b2:	89 7e 68             	mov    DWORD PTR [esi+0x68],edi
  4059b5:	83 c4 04             	add    esp,0x4
  4059b8:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4059ba:	8b ca                	mov    ecx,edx
  4059bc:	83 e1 03             	and    ecx,0x3
  4059bf:	f3 aa                	rep stos BYTE PTR es:[edi],al
  4059c1:	8b 4e 64             	mov    ecx,DWORD PTR [esi+0x64]
  4059c4:	8b 7e 68             	mov    edi,DWORD PTR [esi+0x68]
  4059c7:	0f af 4e 5c          	imul   ecx,DWORD PTR [esi+0x5c]
  4059cb:	0f af 4e 60          	imul   ecx,DWORD PTR [esi+0x60]
  4059cf:	8b d1                	mov    edx,ecx
  4059d1:	33 c0                	xor    eax,eax
  4059d3:	c1 e9 02             	shr    ecx,0x2
  4059d6:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4059d8:	8b ca                	mov    ecx,edx
  4059da:	83 e1 03             	and    ecx,0x3
  4059dd:	f3 aa                	rep stos BYTE PTR es:[edi],al
  4059df:	8b ce                	mov    ecx,esi
  4059e1:	e8 ea 01 00 00       	call   0x405bd0
  4059e6:	5f                   	pop    edi
  4059e7:	5e                   	pop    esi
  4059e8:	c3                   	ret
  4059e9:	90                   	nop
  4059ea:	90                   	nop
  4059eb:	90                   	nop
  4059ec:	90                   	nop
  4059ed:	90                   	nop
  4059ee:	90                   	nop
  4059ef:	90                   	nop
  4059f0:	56                   	push   esi
  4059f1:	8b f1                	mov    esi,ecx
  4059f3:	8b 46 68             	mov    eax,DWORD PTR [esi+0x68]
  4059f6:	85 c0                	test   eax,eax
  4059f8:	74 09                	je     0x405a03
  4059fa:	50                   	push   eax
  4059fb:	e8 50 bb 04 00       	call   0x451550
  405a00:	83 c4 04             	add    esp,0x4
  405a03:	8b ce                	mov    ecx,esi
  405a05:	e8 e6 07 00 00       	call   0x4061f0
  405a0a:	5e                   	pop    esi
  405a0b:	c3                   	ret
  405a0c:	90                   	nop
  405a0d:	90                   	nop
  405a0e:	90                   	nop
  405a0f:	90                   	nop
  405a10:	56                   	push   esi
  405a11:	8b f1                	mov    esi,ecx
  405a13:	6a 01                	push   0x1
  405a15:	8b 46 70             	mov    eax,DWORD PTR [esi+0x70]
  405a18:	8b 4e 6c             	mov    ecx,DWORD PTR [esi+0x6c]
  405a1b:	8b 56 68             	mov    edx,DWORD PTR [esi+0x68]
  405a1e:	50                   	push   eax
  405a1f:	51                   	push   ecx
  405a20:	52                   	push   edx
  405a21:	8b ce                	mov    ecx,esi
  405a23:	e8 08 09 00 00       	call   0x406330
  405a28:	8b ce                	mov    ecx,esi
  405a2a:	e8 b1 0a 00 00       	call   0x4064e0
  405a2f:	5e                   	pop    esi
  405a30:	c3                   	ret
  405a31:	90                   	nop
  405a32:	90                   	nop
  405a33:	90                   	nop
  405a34:	90                   	nop
  405a35:	90                   	nop
  405a36:	90                   	nop
  405a37:	90                   	nop
  405a38:	90                   	nop
  405a39:	90                   	nop
  405a3a:	90                   	nop
  405a3b:	90                   	nop
  405a3c:	90                   	nop
  405a3d:	90                   	nop
  405a3e:	90                   	nop
  405a3f:	90                   	nop
  405a40:	56                   	push   esi
  405a41:	8b f1                	mov    esi,ecx
  405a43:	e8 18 00 00 00       	call   0x405a60
  405a48:	8b c6                	mov    eax,esi
  405a4a:	5e                   	pop    esi
  405a4b:	c3                   	ret
  405a4c:	90                   	nop
  405a4d:	90                   	nop
  405a4e:	90                   	nop
  405a4f:	90                   	nop
  405a50:	e9 ab 00 00 00       	jmp    0x405b00
  405a55:	90                   	nop
  405a56:	90                   	nop
  405a57:	90                   	nop
  405a58:	90                   	nop
  405a59:	90                   	nop
  405a5a:	90                   	nop
  405a5b:	90                   	nop
  405a5c:	90                   	nop
  405a5d:	90                   	nop
  405a5e:	90                   	nop
  405a5f:	90                   	nop
  405a60:	53                   	push   ebx
  405a61:	56                   	push   esi
  405a62:	8b f1                	mov    esi,ecx
  405a64:	68 68 03 00 00       	push   0x368
  405a69:	e8 f1 bb 04 00       	call   0x45165f
  405a6e:	33 db                	xor    ebx,ebx
  405a70:	89 46 24             	mov    DWORD PTR [esi+0x24],eax
  405a73:	89 58 04             	mov    DWORD PTR [eax+0x4],ebx
  405a76:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  405a79:	05 00 03 00 00       	add    eax,0x300
  405a7e:	68 04 87 46 00       	push   0x468704
  405a83:	50                   	push   eax
  405a84:	e8 ea bf 04 00       	call   0x451a73
  405a89:	6a 78                	push   0x78
  405a8b:	e8 cf bb 04 00       	call   0x45165f
  405a90:	89 46 28             	mov    DWORD PTR [esi+0x28],eax
  405a93:	c7 40 04 40 01 00 00 	mov    DWORD PTR [eax+0x4],0x140
  405a9a:	8b 4e 28             	mov    ecx,DWORD PTR [esi+0x28]
  405a9d:	68 ec 86 46 00       	push   0x4686ec
  405aa2:	c7 41 08 f0 00 00 00 	mov    DWORD PTR [ecx+0x8],0xf0
  405aa9:	8b 56 28             	mov    edx,DWORD PTR [esi+0x28]
  405aac:	c7 42 0c 10 00 00 00 	mov    DWORD PTR [edx+0xc],0x10
  405ab3:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  405ab6:	83 c0 10             	add    eax,0x10
  405ab9:	50                   	push   eax
  405aba:	e8 b4 bf 04 00       	call   0x451a73
  405abf:	b8 01 00 00 00       	mov    eax,0x1
  405ac4:	83 c4 18             	add    esp,0x18
  405ac7:	88 46 30             	mov    BYTE PTR [esi+0x30],al
  405aca:	88 46 31             	mov    BYTE PTR [esi+0x31],al
  405acd:	89 46 34             	mov    DWORD PTR [esi+0x34],eax
  405ad0:	88 5e 38             	mov    BYTE PTR [esi+0x38],bl
  405ad3:	89 5e 2c             	mov    DWORD PTR [esi+0x2c],ebx
  405ad6:	89 1e                	mov    DWORD PTR [esi],ebx
  405ad8:	89 5e 04             	mov    DWORD PTR [esi+0x4],ebx
  405adb:	89 5e 08             	mov    DWORD PTR [esi+0x8],ebx
  405ade:	89 5e 0c             	mov    DWORD PTR [esi+0xc],ebx
  405ae1:	89 5e 10             	mov    DWORD PTR [esi+0x10],ebx
  405ae4:	89 5e 3c             	mov    DWORD PTR [esi+0x3c],ebx
  405ae7:	89 5e 40             	mov    DWORD PTR [esi+0x40],ebx
  405aea:	89 5e 20             	mov    DWORD PTR [esi+0x20],ebx
  405aed:	8b c6                	mov    eax,esi
  405aef:	5e                   	pop    esi
  405af0:	5b                   	pop    ebx
  405af1:	c3                   	ret
  405af2:	90                   	nop
  405af3:	90                   	nop
  405af4:	90                   	nop
  405af5:	90                   	nop
  405af6:	90                   	nop
  405af7:	90                   	nop
  405af8:	90                   	nop
  405af9:	90                   	nop
  405afa:	90                   	nop
  405afb:	90                   	nop
  405afc:	90                   	nop
  405afd:	90                   	nop
  405afe:	90                   	nop
  405aff:	90                   	nop
  405b00:	56                   	push   esi
  405b01:	8b f1                	mov    esi,ecx
  405b03:	e8 e8 06 00 00       	call   0x4061f0
  405b08:	8b 46 24             	mov    eax,DWORD PTR [esi+0x24]
  405b0b:	85 c0                	test   eax,eax
  405b0d:	74 09                	je     0x405b18
  405b0f:	50                   	push   eax
  405b10:	e8 3b ba 04 00       	call   0x451550
  405b15:	83 c4 04             	add    esp,0x4
  405b18:	8b 76 28             	mov    esi,DWORD PTR [esi+0x28]
  405b1b:	85 f6                	test   esi,esi
  405b1d:	74 09                	je     0x405b28
  405b1f:	56                   	push   esi
  405b20:	e8 2b ba 04 00       	call   0x451550
  405b25:	83 c4 04             	add    esp,0x4
  405b28:	5e                   	pop    esi
  405b29:	c3                   	ret
  405b2a:	90                   	nop
  405b2b:	90                   	nop
  405b2c:	90                   	nop
  405b2d:	90                   	nop
  405b2e:	90                   	nop
  405b2f:	90                   	nop
  405b30:	81 ec 80 00 00 00    	sub    esp,0x80
  405b36:	8b d1                	mov    edx,ecx
  405b38:	57                   	push   edi
  405b39:	b9 1f 00 00 00       	mov    ecx,0x1f
  405b3e:	33 c0                	xor    eax,eax
  405b40:	8d 7c 24 08          	lea    edi,[esp+0x8]
  405b44:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  405b46:	8b 84 24 90 00 00 00 	mov    eax,DWORD PTR [esp+0x90]
  405b4d:	c7 44 24 08 7c 00 00 	mov    DWORD PTR [esp+0x8],0x7c
  405b54:	00 
  405b55:	85 c0                	test   eax,eax
  405b57:	c7 44 24 0c 07 00 01 	mov    DWORD PTR [esp+0xc],0x10007
  405b5e:	00 
  405b5f:	c7 44 24 70 40 00 00 	mov    DWORD PTR [esp+0x70],0x40
  405b66:	00 
  405b67:	5f                   	pop    edi
  405b68:	74 08                	je     0x405b72
  405b6a:	c7 44 24 6c 40 08 00 	mov    DWORD PTR [esp+0x6c],0x840
  405b71:	00 
  405b72:	8b 8c 24 84 00 00 00 	mov    ecx,DWORD PTR [esp+0x84]
  405b79:	8b 84 24 88 00 00 00 	mov    eax,DWORD PTR [esp+0x88]
  405b80:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  405b84:	8d 4c 24 00          	lea    ecx,[esp+0x0]
  405b88:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  405b8c:	8b 02                	mov    eax,DWORD PTR [edx]
  405b8e:	6a 00                	push   0x0
  405b90:	51                   	push   ecx
  405b91:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  405b95:	c7 44 24 4c 00 00 00 	mov    DWORD PTR [esp+0x4c],0x0
  405b9c:	00 
  405b9d:	c7 44 24 50 00 00 00 	mov    DWORD PTR [esp+0x50],0x0
  405ba4:	00 
  405ba5:	8b 10                	mov    edx,DWORD PTR [eax]
  405ba7:	51                   	push   ecx
  405ba8:	50                   	push   eax
  405ba9:	ff 52 18             	call   DWORD PTR [edx+0x18]
  405bac:	85 c0                	test   eax,eax
  405bae:	74 0b                	je     0x405bbb
  405bb0:	33 c0                	xor    eax,eax
  405bb2:	81 c4 80 00 00 00    	add    esp,0x80
  405bb8:	c2 0c 00             	ret    0xc
  405bbb:	8b 44 24 00          	mov    eax,DWORD PTR [esp+0x0]
  405bbf:	81 c4 80 00 00 00    	add    esp,0x80
  405bc5:	c2 0c 00             	ret    0xc
  405bc8:	90                   	nop
  405bc9:	90                   	nop
  405bca:	90                   	nop
  405bcb:	90                   	nop
  405bcc:	90                   	nop
  405bcd:	90                   	nop
  405bce:	90                   	nop
  405bcf:	90                   	nop
  405bd0:	55                   	push   ebp
  405bd1:	8b ec                	mov    ebp,esp
  405bd3:	6a ff                	push   0xffffffff
  405bd5:	68 00 d0 45 00       	push   0x45d000
  405bda:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  405be0:	50                   	push   eax
  405be1:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  405be8:	81 ec 6c 01 00 00    	sub    esp,0x16c
  405bee:	53                   	push   ebx
  405bef:	56                   	push   esi
  405bf0:	8b f1                	mov    esi,ecx
  405bf2:	57                   	push   edi
  405bf3:	89 65 f0             	mov    DWORD PTR [ebp-0x10],esp
  405bf6:	8d 45 ec             	lea    eax,[ebp-0x14]
  405bf9:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  405bfc:	6a 00                	push   0x0
  405bfe:	50                   	push   eax
  405bff:	89 75 ac             	mov    DWORD PTR [ebp-0x54],esi
  405c02:	8b 51 04             	mov    edx,DWORD PTR [ecx+0x4]
  405c05:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [ebp-0x4],0x0
  405c0c:	52                   	push   edx
  405c0d:	e8 54 5b 04 00       	call   0x44b766
  405c12:	85 c0                	test   eax,eax
  405c14:	7d 15                	jge    0x405c2b
  405c16:	8d 45 98             	lea    eax,[ebp-0x68]
  405c19:	68 d8 37 46 00       	push   0x4637d8
  405c1e:	50                   	push   eax
  405c1f:	c7 45 98 0c 8a 46 00 	mov    DWORD PTR [ebp-0x68],0x468a0c
  405c26:	e8 9c bf 04 00       	call   0x451bc7
  405c2b:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  405c2e:	56                   	push   esi
  405c2f:	68 58 12 46 00       	push   0x461258
  405c34:	50                   	push   eax
  405c35:	8b 08                	mov    ecx,DWORD PTR [eax]
  405c37:	ff 11                	call   DWORD PTR [ecx]
  405c39:	85 c0                	test   eax,eax
  405c3b:	7d 15                	jge    0x405c52
  405c3d:	8d 55 e0             	lea    edx,[ebp-0x20]
  405c40:	68 d8 37 46 00       	push   0x4637d8
  405c45:	52                   	push   edx
  405c46:	c7 45 e0 e4 89 46 00 	mov    DWORD PTR [ebp-0x20],0x4689e4
  405c4d:	e8 75 bf 04 00       	call   0x451bc7
  405c52:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  405c55:	50                   	push   eax
  405c56:	8b 08                	mov    ecx,DWORD PTR [eax]
  405c58:	ff 51 08             	call   DWORD PTR [ecx+0x8]
  405c5b:	8a 46 30             	mov    al,BYTE PTR [esi+0x30]
  405c5e:	84 c0                	test   al,al
  405c60:	0f 84 3e 02 00 00    	je     0x405ea4
  405c66:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  405c69:	85 c0                	test   eax,eax
  405c6b:	75 13                	jne    0x405c80
  405c6d:	8a 56 38             	mov    dl,BYTE PTR [esi+0x38]
  405c70:	f6 da                	neg    dl
  405c72:	1b d2                	sbb    edx,edx
  405c74:	81 e2 00 08 00 00    	and    edx,0x800
  405c7a:	83 c2 13             	add    edx,0x13
  405c7d:	89 56 2c             	mov    DWORD PTR [esi+0x2c],edx
  405c80:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  405c83:	8b 06                	mov    eax,DWORD PTR [esi]
  405c85:	52                   	push   edx
  405c86:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  405c8c:	8b 08                	mov    ecx,DWORD PTR [eax]
  405c8e:	52                   	push   edx
  405c8f:	50                   	push   eax
  405c90:	ff 51 50             	call   DWORD PTR [ecx+0x50]
  405c93:	85 c0                	test   eax,eax
  405c95:	7d 15                	jge    0x405cac
  405c97:	8d 45 b0             	lea    eax,[ebp-0x50]
  405c9a:	68 d8 37 46 00       	push   0x4637d8
  405c9f:	50                   	push   eax
  405ca0:	c7 45 b0 b0 89 46 00 	mov    DWORD PTR [ebp-0x50],0x4689b0
  405ca7:	e8 1b bf 04 00       	call   0x451bc7
  405cac:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  405caf:	8b 0e                	mov    ecx,DWORD PTR [esi]
  405cb1:	6a 00                	push   0x0
  405cb3:	6a 00                	push   0x0
  405cb5:	8b 78 0c             	mov    edi,DWORD PTR [eax+0xc]
  405cb8:	8b 11                	mov    edx,DWORD PTR [ecx]
  405cba:	57                   	push   edi
  405cbb:	8b 78 08             	mov    edi,DWORD PTR [eax+0x8]
  405cbe:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  405cc1:	57                   	push   edi
  405cc2:	50                   	push   eax
  405cc3:	51                   	push   ecx
  405cc4:	ff 52 54             	call   DWORD PTR [edx+0x54]
  405cc7:	85 c0                	test   eax,eax
  405cc9:	7d 15                	jge    0x405ce0
  405ccb:	8d 4d d8             	lea    ecx,[ebp-0x28]
  405cce:	68 d8 37 46 00       	push   0x4637d8
  405cd3:	51                   	push   ecx
  405cd4:	c7 45 d8 8c 89 46 00 	mov    DWORD PTR [ebp-0x28],0x46898c
  405cdb:	e8 e7 be 04 00       	call   0x451bc7
  405ce0:	b9 1f 00 00 00       	mov    ecx,0x1f
  405ce5:	33 c0                	xor    eax,eax
  405ce7:	8d bd 04 ff ff ff    	lea    edi,[ebp-0xfc]
  405ced:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  405cef:	8a 46 31             	mov    al,BYTE PTR [esi+0x31]
  405cf2:	c7 85 04 ff ff ff 7c 	mov    DWORD PTR [ebp-0xfc],0x7c
  405cf9:	00 00 00 
  405cfc:	84 c0                	test   al,al
  405cfe:	74 39                	je     0x405d39
  405d00:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  405d03:	83 f8 01             	cmp    eax,0x1
  405d06:	89 85 18 ff ff ff    	mov    DWORD PTR [ebp-0xe8],eax
  405d0c:	73 15                	jae    0x405d23
  405d0e:	8d 55 84             	lea    edx,[ebp-0x7c]
  405d11:	68 d8 37 46 00       	push   0x4637d8
  405d16:	52                   	push   edx
  405d17:	c7 45 84 5c 89 46 00 	mov    DWORD PTR [ebp-0x7c],0x46895c
  405d1e:	e8 a4 be 04 00       	call   0x451bc7
  405d23:	c7 85 08 ff ff ff 21 	mov    DWORD PTR [ebp-0xf8],0x21
  405d2a:	00 00 00 
  405d2d:	c7 85 6c ff ff ff 18 	mov    DWORD PTR [ebp-0x94],0x218
  405d34:	02 00 00 
  405d37:	eb 14                	jmp    0x405d4d
  405d39:	c7 85 08 ff ff ff 01 	mov    DWORD PTR [ebp-0xf8],0x1
  405d40:	00 00 00 
  405d43:	c7 85 6c ff ff ff 00 	mov    DWORD PTR [ebp-0x94],0x200
  405d4a:	02 00 00 
  405d4d:	8a 46 38             	mov    al,BYTE PTR [esi+0x38]
  405d50:	84 c0                	test   al,al
  405d52:	74 0f                	je     0x405d63
  405d54:	8b 85 6c ff ff ff    	mov    eax,DWORD PTR [ebp-0x94]
  405d5a:	80 cc 20             	or     ah,0x20
  405d5d:	89 85 6c ff ff ff    	mov    DWORD PTR [ebp-0x94],eax
  405d63:	8b 06                	mov    eax,DWORD PTR [esi]
  405d65:	8d 5e 04             	lea    ebx,[esi+0x4]
  405d68:	6a 00                	push   0x0
  405d6a:	8d 95 04 ff ff ff    	lea    edx,[ebp-0xfc]
  405d70:	8b 08                	mov    ecx,DWORD PTR [eax]
  405d72:	53                   	push   ebx
  405d73:	52                   	push   edx
  405d74:	50                   	push   eax
  405d75:	ff 51 18             	call   DWORD PTR [ecx+0x18]
  405d78:	85 c0                	test   eax,eax
  405d7a:	7d 15                	jge    0x405d91
  405d7c:	8d 45 d0             	lea    eax,[ebp-0x30]
  405d7f:	68 d8 37 46 00       	push   0x4637d8
  405d84:	50                   	push   eax
  405d85:	c7 45 d0 30 89 46 00 	mov    DWORD PTR [ebp-0x30],0x468930
  405d8c:	e8 36 be 04 00       	call   0x451bc7
  405d91:	8a 46 31             	mov    al,BYTE PTR [esi+0x31]
  405d94:	84 c0                	test   al,al
  405d96:	74 30                	je     0x405dc8
  405d98:	8b 03                	mov    eax,DWORD PTR [ebx]
  405d9a:	8d 56 08             	lea    edx,[esi+0x8]
  405d9d:	52                   	push   edx
  405d9e:	8d 55 88             	lea    edx,[ebp-0x78]
  405da1:	c7 45 88 04 00 00 00 	mov    DWORD PTR [ebp-0x78],0x4
  405da8:	8b 08                	mov    ecx,DWORD PTR [eax]
  405daa:	52                   	push   edx
  405dab:	50                   	push   eax
  405dac:	ff 51 30             	call   DWORD PTR [ecx+0x30]
  405daf:	85 c0                	test   eax,eax
  405db1:	7d 15                	jge    0x405dc8
  405db3:	8d 45 a8             	lea    eax,[ebp-0x58]
  405db6:	68 d8 37 46 00       	push   0x4637d8
  405dbb:	50                   	push   eax
  405dbc:	c7 45 a8 08 89 46 00 	mov    DWORD PTR [ebp-0x58],0x468908
  405dc3:	e8 ff bd 04 00       	call   0x451bc7
  405dc8:	8a 46 38             	mov    al,BYTE PTR [esi+0x38]
  405dcb:	84 c0                	test   al,al
  405dcd:	0f 84 b1 02 00 00    	je     0x406084
  405dd3:	b9 1f 00 00 00       	mov    ecx,0x1f
  405dd8:	33 c0                	xor    eax,eax
  405dda:	8d bd 04 ff ff ff    	lea    edi,[ebp-0xfc]
  405de0:	6a 00                	push   0x0
  405de2:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  405de4:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  405de7:	c7 85 04 ff ff ff 7c 	mov    DWORD PTR [ebp-0xfc],0x7c
  405dee:	00 00 00 
  405df1:	c7 85 08 ff ff ff 47 	mov    DWORD PTR [ebp-0xf8],0x47
  405df8:	00 00 00 
  405dfb:	8d 7e 0c             	lea    edi,[esi+0xc]
  405dfe:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  405e01:	57                   	push   edi
  405e02:	89 8d 0c ff ff ff    	mov    DWORD PTR [ebp-0xf4],ecx
  405e08:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  405e0b:	89 95 10 ff ff ff    	mov    DWORD PTR [ebp-0xf0],edx
  405e11:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  405e14:	89 85 58 ff ff ff    	mov    DWORD PTR [ebp-0xa8],eax
  405e1a:	8b 06                	mov    eax,DWORD PTR [esi]
  405e1c:	8d 95 04 ff ff ff    	lea    edx,[ebp-0xfc]
  405e22:	c7 85 6c ff ff ff 00 	mov    DWORD PTR [ebp-0x94],0x20000
  405e29:	00 02 00 
  405e2c:	8b 08                	mov    ecx,DWORD PTR [eax]
  405e2e:	52                   	push   edx
  405e2f:	50                   	push   eax
  405e30:	ff 51 18             	call   DWORD PTR [ecx+0x18]
  405e33:	85 c0                	test   eax,eax
  405e35:	7d 15                	jge    0x405e4c
  405e37:	8d 45 c8             	lea    eax,[ebp-0x38]
  405e3a:	68 d8 37 46 00       	push   0x4637d8
  405e3f:	50                   	push   eax
  405e40:	c7 45 c8 dc 88 46 00 	mov    DWORD PTR [ebp-0x38],0x4688dc
  405e47:	e8 7b bd 04 00       	call   0x451bc7
  405e4c:	8a 46 31             	mov    al,BYTE PTR [esi+0x31]
  405e4f:	84 c0                	test   al,al
  405e51:	0f 84 2d 02 00 00    	je     0x406084
  405e57:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
  405e5a:	8b 17                	mov    edx,DWORD PTR [edi]
  405e5c:	52                   	push   edx
  405e5d:	50                   	push   eax
  405e5e:	8b 08                	mov    ecx,DWORD PTR [eax]
  405e60:	ff 51 0c             	call   DWORD PTR [ecx+0xc]
  405e63:	85 c0                	test   eax,eax
  405e65:	7d 15                	jge    0x405e7c
  405e67:	8d 45 9c             	lea    eax,[ebp-0x64]
  405e6a:	68 d8 37 46 00       	push   0x4637d8
  405e6f:	50                   	push   eax
  405e70:	c7 45 9c ac 88 46 00 	mov    DWORD PTR [ebp-0x64],0x4688ac
  405e77:	e8 4b bd 04 00       	call   0x451bc7
  405e7c:	8b 03                	mov    eax,DWORD PTR [ebx]
  405e7e:	8b 17                	mov    edx,DWORD PTR [edi]
  405e80:	52                   	push   edx
  405e81:	50                   	push   eax
  405e82:	8b 08                	mov    ecx,DWORD PTR [eax]
  405e84:	ff 51 0c             	call   DWORD PTR [ecx+0xc]
  405e87:	85 c0                	test   eax,eax
  405e89:	0f 8d f5 01 00 00    	jge    0x406084
  405e8f:	8d 45 c0             	lea    eax,[ebp-0x40]
  405e92:	68 d8 37 46 00       	push   0x4637d8
  405e97:	50                   	push   eax
  405e98:	c7 45 c0 ac 88 46 00 	mov    DWORD PTR [ebp-0x40],0x4688ac
  405e9f:	e8 23 bd 04 00       	call   0x451bc7
  405ea4:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  405ea7:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  405eaa:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  405ead:	51                   	push   ecx
  405eae:	52                   	push   edx
  405eaf:	6a 00                	push   0x0
  405eb1:	8d 45 88             	lea    eax,[ebp-0x78]
  405eb4:	6a 00                	push   0x0
  405eb6:	50                   	push   eax
  405eb7:	ff 15 f4 01 46 00    	call   DWORD PTR ds:0x4601f4
  405ebd:	8b 0d 84 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f384
  405ec3:	8b 3d ec 01 46 00    	mov    edi,DWORD PTR ds:0x4601ec
  405ec9:	6a ec                	push   0xffffffec
  405ecb:	51                   	push   ecx
  405ecc:	ff d7                	call   edi
  405ece:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  405ed4:	50                   	push   eax
  405ed5:	52                   	push   edx
  405ed6:	ff 15 fc 01 46 00    	call   DWORD PTR ds:0x4601fc
  405edc:	f7 d8                	neg    eax
  405ede:	1b c0                	sbb    eax,eax
  405ee0:	f7 d8                	neg    eax
  405ee2:	50                   	push   eax
  405ee3:	a1 84 f3 46 00       	mov    eax,ds:0x46f384
  405ee8:	6a f0                	push   0xfffffff0
  405eea:	50                   	push   eax
  405eeb:	ff d7                	call   edi
  405eed:	8d 4d 88             	lea    ecx,[ebp-0x78]
  405ef0:	50                   	push   eax
  405ef1:	51                   	push   ecx
  405ef2:	ff 15 00 02 46 00    	call   DWORD PTR ds:0x460200
  405ef8:	8b 55 94             	mov    edx,DWORD PTR [ebp-0x6c]
  405efb:	8b 5d 8c             	mov    ebx,DWORD PTR [ebp-0x74]
  405efe:	8b 45 90             	mov    eax,DWORD PTR [ebp-0x70]
  405f01:	2b d3                	sub    edx,ebx
  405f03:	8b 0d 84 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f384
  405f09:	6a 01                	push   0x1
  405f0b:	52                   	push   edx
  405f0c:	8b 55 88             	mov    edx,DWORD PTR [ebp-0x78]
  405f0f:	2b c2                	sub    eax,edx
  405f11:	50                   	push   eax
  405f12:	6a 00                	push   0x0
  405f14:	6a 00                	push   0x0
  405f16:	51                   	push   ecx
  405f17:	ff 15 f8 01 46 00    	call   DWORD PTR ds:0x4601f8
  405f1d:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  405f20:	85 c0                	test   eax,eax
  405f22:	75 13                	jne    0x405f37
  405f24:	8a 56 38             	mov    dl,BYTE PTR [esi+0x38]
  405f27:	f6 da                	neg    dl
  405f29:	1b d2                	sbb    edx,edx
  405f2b:	81 e2 00 08 00 00    	and    edx,0x800
  405f31:	83 c2 08             	add    edx,0x8
  405f34:	89 56 2c             	mov    DWORD PTR [esi+0x2c],edx
  405f37:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  405f3a:	8b 06                	mov    eax,DWORD PTR [esi]
  405f3c:	52                   	push   edx
  405f3d:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  405f43:	8b 08                	mov    ecx,DWORD PTR [eax]
  405f45:	52                   	push   edx
  405f46:	50                   	push   eax
  405f47:	ff 51 50             	call   DWORD PTR [ecx+0x50]
  405f4a:	85 c0                	test   eax,eax
  405f4c:	7d 15                	jge    0x405f63
  405f4e:	8d 45 a0             	lea    eax,[ebp-0x60]
  405f51:	68 d8 37 46 00       	push   0x4637d8
  405f56:	50                   	push   eax
  405f57:	c7 45 a0 7c 88 46 00 	mov    DWORD PTR [ebp-0x60],0x46887c
  405f5e:	e8 64 bc 04 00       	call   0x451bc7
  405f63:	b9 1f 00 00 00       	mov    ecx,0x1f
  405f68:	33 c0                	xor    eax,eax
  405f6a:	8d bd 04 ff ff ff    	lea    edi,[ebp-0xfc]
  405f70:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  405f72:	8a 46 38             	mov    al,BYTE PTR [esi+0x38]
  405f75:	c7 85 04 ff ff ff 7c 	mov    DWORD PTR [ebp-0xfc],0x7c
  405f7c:	00 00 00 
  405f7f:	84 c0                	test   al,al
  405f81:	c7 85 08 ff ff ff 01 	mov    DWORD PTR [ebp-0xf8],0x1
  405f88:	00 00 00 
  405f8b:	c7 85 6c ff ff ff 00 	mov    DWORD PTR [ebp-0x94],0x200
  405f92:	02 00 00 
  405f95:	74 0a                	je     0x405fa1
  405f97:	c7 85 6c ff ff ff 00 	mov    DWORD PTR [ebp-0x94],0x2200
  405f9e:	22 00 00 
  405fa1:	8b 06                	mov    eax,DWORD PTR [esi]
  405fa3:	8d 5e 04             	lea    ebx,[esi+0x4]
  405fa6:	6a 00                	push   0x0
  405fa8:	8d 95 04 ff ff ff    	lea    edx,[ebp-0xfc]
  405fae:	8b 08                	mov    ecx,DWORD PTR [eax]
  405fb0:	53                   	push   ebx
  405fb1:	52                   	push   edx
  405fb2:	50                   	push   eax
  405fb3:	ff 51 18             	call   DWORD PTR [ecx+0x18]
  405fb6:	85 c0                	test   eax,eax
  405fb8:	7d 15                	jge    0x405fcf
  405fba:	8d 45 b8             	lea    eax,[ebp-0x48]
  405fbd:	68 d8 37 46 00       	push   0x4637d8
  405fc2:	50                   	push   eax
  405fc3:	c7 45 b8 30 89 46 00 	mov    DWORD PTR [ebp-0x48],0x468930
  405fca:	e8 f8 bb 04 00       	call   0x451bc7
  405fcf:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  405fd2:	6a 00                	push   0x0
  405fd4:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  405fd7:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  405fda:	51                   	push   ecx
  405fdb:	52                   	push   edx
  405fdc:	8b ce                	mov    ecx,esi
  405fde:	e8 4d fb ff ff       	call   0x405b30
  405fe3:	85 c0                	test   eax,eax
  405fe5:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  405fe8:	75 15                	jne    0x405fff
  405fea:	8d 45 e8             	lea    eax,[ebp-0x18]
  405fed:	68 d8 37 46 00       	push   0x4637d8
  405ff2:	50                   	push   eax
  405ff3:	c7 45 e8 54 88 46 00 	mov    DWORD PTR [ebp-0x18],0x468854
  405ffa:	e8 c8 bb 04 00       	call   0x451bc7
  405fff:	8b 06                	mov    eax,DWORD PTR [esi]
  406001:	8d 7e 10             	lea    edi,[esi+0x10]
  406004:	6a 00                	push   0x0
  406006:	57                   	push   edi
  406007:	8b 08                	mov    ecx,DWORD PTR [eax]
  406009:	6a 00                	push   0x0
  40600b:	50                   	push   eax
  40600c:	ff 51 10             	call   DWORD PTR [ecx+0x10]
  40600f:	85 c0                	test   eax,eax
  406011:	7d 15                	jge    0x406028
  406013:	8d 55 e4             	lea    edx,[ebp-0x1c]
  406016:	68 d8 37 46 00       	push   0x4637d8
  40601b:	52                   	push   edx
  40601c:	c7 45 e4 34 88 46 00 	mov    DWORD PTR [ebp-0x1c],0x468834
  406023:	e8 9f bb 04 00       	call   0x451bc7
  406028:	8b 07                	mov    eax,DWORD PTR [edi]
  40602a:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  406030:	52                   	push   edx
  406031:	6a 00                	push   0x0
  406033:	8b 08                	mov    ecx,DWORD PTR [eax]
  406035:	50                   	push   eax
  406036:	ff 51 20             	call   DWORD PTR [ecx+0x20]
  406039:	85 c0                	test   eax,eax
  40603b:	7d 15                	jge    0x406052
  40603d:	8d 45 dc             	lea    eax,[ebp-0x24]
  406040:	68 d8 37 46 00       	push   0x4637d8
  406045:	50                   	push   eax
  406046:	c7 45 dc 0c 88 46 00 	mov    DWORD PTR [ebp-0x24],0x46880c
  40604d:	e8 75 bb 04 00       	call   0x451bc7
  406052:	8b 03                	mov    eax,DWORD PTR [ebx]
  406054:	8b 17                	mov    edx,DWORD PTR [edi]
  406056:	52                   	push   edx
  406057:	50                   	push   eax
  406058:	8b 08                	mov    ecx,DWORD PTR [eax]
  40605a:	ff 51 70             	call   DWORD PTR [ecx+0x70]
  40605d:	85 c0                	test   eax,eax
  40605f:	7d 15                	jge    0x406076
  406061:	8d 45 d4             	lea    eax,[ebp-0x2c]
  406064:	68 d8 37 46 00       	push   0x4637d8
  406069:	50                   	push   eax
  40606a:	c7 45 d4 ec 87 46 00 	mov    DWORD PTR [ebp-0x2c],0x4687ec
  406071:	e8 51 bb 04 00       	call   0x451bc7
  406076:	8b 07                	mov    eax,DWORD PTR [edi]
  406078:	50                   	push   eax
  406079:	8b 08                	mov    ecx,DWORD PTR [eax]
  40607b:	ff 51 08             	call   DWORD PTR [ecx+0x8]
  40607e:	c7 07 00 00 00 00    	mov    DWORD PTR [edi],0x0
  406084:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  406087:	6a 00                	push   0x0
  406089:	8b ce                	mov    ecx,esi
  40608b:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  40608e:	8b                   	.byte 0x8b
  40608f:	40                   	inc    eax
