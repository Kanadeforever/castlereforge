; 固化41：world action code13/code14写入1500与模式位
  40cf01:	e9 3a 01 00 00       	jmp    0x40d040
  40cf06:	53                   	push   ebx
  40cf07:	6a 03                	push   0x3
  40cf09:	6a 02                	push   0x2
  40cf0b:	e8 80 6c ff ff       	call   0x403b90
  40cf10:	6a 00                	push   0x0
  40cf12:	6a 00                	push   0x0
  40cf14:	53                   	push   ebx
  40cf15:	e8 46 6d ff ff       	call   0x403c60
  40cf1a:	83 c4 18             	add    esp,0x18
  40cf1d:	88 1d 77 f6 46 00    	mov    BYTE PTR ds:0x46f677,bl
  40cf23:	e9 18 01 00 00       	jmp    0x40d040
  40cf28:	53                   	push   ebx
  40cf29:	6a 09                	push   0x9
  40cf2b:	6a 02                	push   0x2
  40cf2d:	c7 05 1c f6 46 00 dc 	mov    DWORD PTR ds:0x46f61c,0x5dc
  40cf34:	05 00 00 
  40cf37:	c6 05 6d 84 46 00 00 	mov    BYTE PTR ds:0x46846d,0x0
  40cf3e:	e8 4d 6c ff ff       	call   0x403b90
  40cf43:	6a 00                	push   0x0
  40cf45:	6a 00                	push   0x0
  40cf47:	53                   	push   ebx
  40cf48:	e8 13 6d ff ff       	call   0x403c60
  40cf4d:	83 c4 18             	add    esp,0x18
  40cf50:	88 1d 77 f6 46 00    	mov    BYTE PTR ds:0x46f677,bl
  40cf56:	e9 e5 00 00 00       	jmp    0x40d040
  40cf5b:	53                   	push   ebx
  40cf5c:	6a 08                	push   0x8
  40cf5e:	6a 02                	push   0x2
  40cf60:	c7 05 1c f6 46 00 dc 	mov    DWORD PTR ds:0x46f61c,0x5dc
  40cf67:	05 00 00 
  40cf6a:	88 1d 6d 84 46 00    	mov    BYTE PTR ds:0x46846d,bl
  40cf70:	e8 1b 6c ff ff       	call   0x403b90
  40cf75:	6a 00                	push   0x0
  40cf77:	6a 00                	push   0x0
  40cf79:	53                   	push   ebx
  40cf7a:	e8 e1 6c ff ff       	call   0x403c60
  40cf7f:	83 c4 18             	add    esp,0x18
  40cf82:	88 1d 77 f6 46 00    	mov    BYTE PTR ds:0x46f677,bl
  40cf88:	e9 b3 00 00 00       	jmp    0x40d040
  40cf8d:	39 1d f0 8b 46 00    	cmp    DWORD PTR ds:0x468bf0,ebx
  40cf93:	75 14                	jne    0x40cfa9
  40cf95:	e8 a6 e3 ff ff       	call   0x40b340
  40cf9a:	6a 05                	push   0x5
  40cf9c:	e8 8f e2 ff ff       	call   0x40b230
  40cfa1:	83 c4 04             	add    esp,0x4
