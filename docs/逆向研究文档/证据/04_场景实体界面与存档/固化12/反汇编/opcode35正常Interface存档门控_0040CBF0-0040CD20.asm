
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
  40cc70:	57                   	push   edi
  40cc71:	6a 04                	push   0x4
  40cc73:	e8 a8 a2 ff ff       	call   0x406f20
  40cc78:	83 c4 08             	add    esp,0x8
  40cc7b:	84 c0                	test   al,al
  40cc7d:	74 06                	je     0x40cc85
  40cc7f:	89 1d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,ebx
  40cc85:	8b c6                	mov    eax,esi
  40cc87:	5f                   	pop    edi
  40cc88:	88 1d 1d f8 89 00    	mov    BYTE PTR ds:0x89f81d,bl
  40cc8e:	5e                   	pop    esi
  40cc8f:	5b                   	pop    ebx
  40cc90:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40cc94:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40cc9b:	83 c4 74             	add    esp,0x74
  40cc9e:	c3                   	ret
  40cc9f:	e8 ac e4 03 00       	call   0x44b150
  40cca4:	e8 f7 e6 ff ff       	call   0x40b3a0
  40cca9:	8b c8                	mov    ecx,eax
  40ccab:	e8 f0 e3 ff ff       	call   0x40b0a0
  40ccb0:	e8 eb e6 ff ff       	call   0x40b3a0
  40ccb5:	8b 88 80 02 00 00    	mov    ecx,DWORD PTR [eax+0x280]
  40ccbb:	e8 b0 e1 ff ff       	call   0x40ae70
  40ccc0:	8b c8                	mov    ecx,eax
  40ccc2:	e8 f9 d3 ff ff       	call   0x40a0c0
  40ccc7:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  40cccd:	8b cb                	mov    ecx,ebx
  40cccf:	2b c8                	sub    ecx,eax
  40ccd1:	68 60 06 00 00       	push   0x660
  40ccd6:	89 8a 08 01 00 00    	mov    DWORD PTR [edx+0x108],ecx
  40ccdc:	e8 7e 49 04 00       	call   0x45165f
  40cce1:	83 c4 04             	add    esp,0x4
  40cce4:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  40cce8:	3b c7                	cmp    eax,edi
  40ccea:	89 7c 24 7c          	mov    DWORD PTR [esp+0x7c],edi
  40ccee:	74 1b                	je     0x40cd0b
  40ccf0:	8b c8                	mov    ecx,eax
  40ccf2:	e8 29 7c 02 00       	call   0x434920
  40ccf7:	a3 0c ed 8d 00       	mov    ds:0x8ded0c,eax
  40ccfc:	c7 05 18 f8 89 00 02 	mov    DWORD PTR ds:0x89f818,0x2
  40cd03:	00 00 00 
  40cd06:	e9 7a ff ff ff       	jmp    0x40cc85
  40cd0b:	33 c0                	xor    eax,eax
  40cd0d:	c7 05 18 f8 89 00 02 	mov    DWORD PTR ds:0x89f818,0x2
  40cd14:	00 00 00 
  40cd17:	a3 0c ed 8d 00       	mov    ds:0x8ded0c,eax
  40cd1c:	e9 64 ff ff        	jmp    0x40cc85
