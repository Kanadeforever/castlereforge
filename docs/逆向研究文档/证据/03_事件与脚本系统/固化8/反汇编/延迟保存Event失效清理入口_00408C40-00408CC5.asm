  408c40:	56                   	push   esi
  408c41:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  408c45:	85 f6                	test   esi,esi
  408c47:	74 0a                	je     0x408c53
  408c49:	c7 05 f8 f7 89 00 00 	mov    DWORD PTR ds:0x89f7f8,0x0
  408c50:	00 00 00 
  408c53:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  408c57:	50                   	push   eax
  408c58:	a1 f0 8b 46 00       	mov    eax,ds:0x468bf0
  408c5d:	56                   	push   esi
  408c5e:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  408c65:	2b c8                	sub    ecx,eax
  408c67:	8d 14 88             	lea    edx,[eax+ecx*4]
  408c6a:	a1 f0 f7 89 00       	mov    eax,ds:0x89f7f0
  408c6f:	8d 0c 90             	lea    ecx,[eax+edx*4]
  408c72:	e8 99 18 00 00       	call   0x40a510
  408c77:	85 f6                	test   esi,esi
  408c79:	5e                   	pop    esi
  408c7a:	74 49                	je     0x408cc5
  408c7c:	a1 f0 8b 46 00       	mov    eax,ds:0x468bf0
  408c81:	c7 05 f4 f7 89 00 00 	mov    DWORD PTR ds:0x89f7f4,0x0
  408c88:	00 00 00 
  408c8b:	85 c0                	test   eax,eax
  408c8d:	75 0a                	jne    0x408c99
  408c8f:	6a 01                	push   0x1
  408c91:	e8 8a 00 00 00       	call   0x408d20
  408c96:	83 c4 04             	add    esp,0x4
  408c99:	e8 72 ff ff ff       	call   0x408c10
  408c9e:	8b 40 18             	mov    eax,DWORD PTR [eax+0x18]
  408ca1:	33 c9                	xor    ecx,ecx
  408ca3:	8a cc                	mov    cl,ah
  408ca5:	8b c1                	mov    eax,ecx
  408ca7:	83 e8 00             	sub    eax,0x0
  408caa:	74 0e                	je     0x408cba
  408cac:	48                   	dec    eax
  408cad:	75 66                	jne    0x408d15
  408caf:	6a 02                	push   0x2
  408cb1:	e8 0a a8 ff ff       	call   0x4034c0
  408cb6:	83 c4 04             	add    esp,0x4
  408cb9:	c3                   	ret
  408cba:	6a 01                	push   0x1
  408cbc:	e8 ff a7 ff ff       	call   0x4034c0
  408cc1:	83 c4 04             	add    esp,0x4
  408cc4:	c3                   	ret
  408cc5:	81 3d f4 f7 89 00 60 	cmp    DWORD PTR ds:0x89f7f4,0x960
