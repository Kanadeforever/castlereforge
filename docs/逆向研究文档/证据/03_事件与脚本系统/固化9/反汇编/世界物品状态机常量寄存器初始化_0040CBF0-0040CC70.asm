
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040cbf0 <.text+0xbbf0>:
  40cbf0:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40cbf6:	6a ff                	push   0xffffffff
  40cbf8:	68 56 d1 45 00       	push   0x45d156
  40cbfd:	50                   	push   eax
  40cbfe:	a1 d0 f7 89 00       	mov    eax,ds:0x89f7d0
  40cc03:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40cc0a:	83 ec 68             	sub    esp,0x68
  40cc0d:	53                   	push   ebx
  40cc0e:	56                   	push   esi
  40cc0f:	57                   	push   edi
  40cc10:	33 ff                	xor    edi,edi
  40cc12:	3b c7                	cmp    eax,edi
  40cc14:	8b f1                	mov    esi,ecx
  40cc16:	74 3d                	je     0x40cc55
  40cc18:	8b 15 10 f8 89 00    	mov    edx,DWORD PTR ds:0x89f810
  40cc1e:	56                   	push   esi
  40cc1f:	42                   	inc    edx
  40cc20:	89 3d d0 f7 89 00    	mov    DWORD PTR ds:0x89f7d0,edi
  40cc26:	89 15 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,edx
  40cc2c:	89 3d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,edi
  40cc32:	c6 05 1d f8 89 00 01 	mov    BYTE PTR ds:0x89f81d,0x1
  40cc39:	e8 12 49 04 00       	call   0x451550
  40cc3e:	83 c4 04             	add    esp,0x4
  40cc41:	33 c0                	xor    eax,eax
  40cc43:	5f                   	pop    edi
  40cc44:	5e                   	pop    esi
  40cc45:	5b                   	pop    ebx
  40cc46:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40cc4a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40cc51:	83 c4 74             	add    esp,0x74
  40cc54:	c3                   	ret
  40cc55:	a1 18 f8 89 00       	mov    eax,ds:0x89f818
  40cc5a:	bb 01 00 00 00       	mov    ebx,0x1
  40cc5f:	83 f8 03             	cmp    eax,0x3
  40cc62:	77 21                	ja     0x40cc85
  40cc64:	ff 24 85 dc d0 40 00 	jmp    DWORD PTR [eax*4+0x40d0dc]
  40cc6b:	e8 e0 54 ff ff       	call   0x402150
