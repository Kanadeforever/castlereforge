
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040cc55 <.text+0xbc55>:
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
