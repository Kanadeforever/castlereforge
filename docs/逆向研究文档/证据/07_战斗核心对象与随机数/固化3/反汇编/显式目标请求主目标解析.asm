
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423cb0 <.text+0x22cb0>:
  423cb0:	83 ec 50             	sub    esp,0x50
  423cb3:	53                   	push   ebx
  423cb4:	56                   	push   esi
  423cb5:	57                   	push   edi
  423cb6:	8b 7c 24 60          	mov    edi,DWORD PTR [esp+0x60]
  423cba:	85 ff                	test   edi,edi
  423cbc:	0f 84 a9 00 00 00    	je     0x423d6b
  423cc2:	8b 5c 24 64          	mov    ebx,DWORD PTR [esp+0x64]
  423cc6:	85 db                	test   ebx,ebx
  423cc8:	0f 84 9d 00 00 00    	je     0x423d6b
  423cce:	85 ff                	test   edi,edi
  423cd0:	7e 27                	jle    0x423cf9
  423cd2:	8b cb                	mov    ecx,ebx
  423cd4:	8d 54 24 0c          	lea    edx,[esp+0xc]
  423cd8:	2b ca                	sub    ecx,edx
  423cda:	8d 44 24 0c          	lea    eax,[esp+0xc]
  423cde:	8b d7                	mov    edx,edi
  423ce0:	8b 34 01             	mov    esi,DWORD PTR [ecx+eax*1]
  423ce3:	83 c0 04             	add    eax,0x4
  423ce6:	4a                   	dec    edx
  423ce7:	8b 34 b5 94 fd 89 00 	mov    esi,DWORD PTR [esi*4+0x89fd94]
  423cee:	8b b6 9c 0b 00 00    	mov    esi,DWORD PTR [esi+0xb9c]
  423cf4:	89 70 fc             	mov    DWORD PTR [eax-0x4],esi
  423cf7:	75 e7                	jne    0x423ce0
  423cf9:	83 ff 01             	cmp    edi,0x1
  423cfc:	75 0b                	jne    0x423d09
  423cfe:	8b 03                	mov    eax,DWORD PTR [ebx]
  423d00:	5f                   	pop    edi
  423d01:	5e                   	pop    esi
  423d02:	5b                   	pop    ebx
  423d03:	83 c4 50             	add    esp,0x50
  423d06:	c2 08 00             	ret    0x8
  423d09:	83 ff 02             	cmp    edi,0x2
  423d0c:	75 23                	jne    0x423d31
  423d0e:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  423d12:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  423d16:	3b c1                	cmp    eax,ecx
  423d18:	7e 0b                	jle    0x423d25
  423d1a:	8b 03                	mov    eax,DWORD PTR [ebx]
  423d1c:	5f                   	pop    edi
  423d1d:	5e                   	pop    esi
  423d1e:	5b                   	pop    ebx
  423d1f:	83 c4 50             	add    esp,0x50
  423d22:	c2 08 00             	ret    0x8
  423d25:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  423d28:	5f                   	pop    edi
  423d29:	5e                   	pop    esi
  423d2a:	5b                   	pop    ebx
  423d2b:	83 c4 50             	add    esp,0x50
  423d2e:	c2 08 00             	ret    0x8
  423d31:	83 ff 03             	cmp    edi,0x3
  423d34:	7c 35                	jl     0x423d6b
  423d36:	b8 01 00 00 00       	mov    eax,0x1
  423d3b:	33 f6                	xor    esi,esi
  423d3d:	3b f8                	cmp    edi,eax
  423d3f:	7e 1e                	jle    0x423d5f
  423d41:	55                   	push   ebp
  423d42:	33 d2                	xor    edx,edx
  423d44:	8d 0c 85 00 00 00 00 	lea    ecx,[eax*4+0x0]
  423d4b:	8b 6c 0c 10          	mov    ebp,DWORD PTR [esp+ecx*1+0x10]
  423d4f:	3b 6c 14 10          	cmp    ebp,DWORD PTR [esp+edx*1+0x10]
  423d53:	7e 04                	jle    0x423d59
  423d55:	8b f0                	mov    esi,eax
  423d57:	8b d1                	mov    edx,ecx
  423d59:	40                   	inc    eax
  423d5a:	3b c7                	cmp    eax,edi
  423d5c:	7c e6                	jl     0x423d44
  423d5e:	5d                   	pop    ebp
  423d5f:	8b 04 b3             	mov    eax,DWORD PTR [ebx+esi*4]
  423d62:	5f                   	pop    edi
  423d63:	5e                   	pop    esi
  423d64:	5b                   	pop    ebx
  423d65:	83 c4 50             	add    esp,0x50
  423d68:	c2 08 00             	ret    0x8
  423d6b:	5f                   	pop    edi
  423d6c:	5e                   	pop    esi
  423d6d:	83 c8 ff             	or     eax,0xffffffff
  423d70:	5b                   	pop    ebx
  423d71:	83 c4 50             	add    esp,0x50
  423d74:	c2 08 00             	ret    0x8
  423d77:	90                   	nop
  423d78:	90                   	nop
  423d79:	90                   	nop
  423d7a:	90                   	nop
  423d7b:	90                   	nop
  423d7c:	90                   	nop
  423d7d:	90                   	nop
  423d7e:	90                   	nop
  423d7f:	90                   	nop
