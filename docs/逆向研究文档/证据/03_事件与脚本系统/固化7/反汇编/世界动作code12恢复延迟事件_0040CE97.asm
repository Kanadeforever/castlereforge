  40ce92:	e9 a9 01 00 00       	jmp    0x40d040
  40ce97:	a1 f8 f7 89 00       	mov    eax,ds:0x89f7f8
  40ce9c:	85 c0                	test   eax,eax
  40ce9e:	7e 37                	jle    0x40ced7
  40cea0:	83 3d fc f7 89 00 0c 	cmp    DWORD PTR ds:0x89f7fc,0xc
  40cea7:	75 2e                	jne    0x40ced7
  40cea9:	e8 92 e4 ff ff       	call   0x40b340
  40ceae:	8b 0d f8 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7f8
  40ceb4:	51                   	push   ecx
  40ceb5:	e8 76 e3 ff ff       	call   0x40b230
  40ceba:	a1 10 f8 89 00       	mov    eax,ds:0x89f810
  40cebf:	83 c4 04             	add    esp,0x4
  40cec2:	40                   	inc    eax
  40cec3:	c7 05 18 f8 89 00 00 	mov    DWORD PTR ds:0x89f818,0x0
  40ceca:	00 00 00 
  40cecd:	a3 10 f8 89 00       	mov    ds:0x89f810,eax
  40ced2:	e9 69 01 00 00       	jmp    0x40d040
  40ced7:	e8 c4 e4 ff ff       	call   0x40b3a0
  40cedc:	8b 88 80 02 00 00    	mov    ecx,DWORD PTR [eax+0x280]
  40cee2:	e8 69 df ff ff       	call   0x40ae50
  40cee7:	8b c8                	mov    ecx,eax
  40cee9:	e8 e2 d1 ff ff       	call   0x40a0d0
  40ceee:	3b c3                	cmp    eax,ebx
  40cef0:	75 14                	jne    0x40cf06
  40cef2:	e8 49 e4 ff ff       	call   0x40b340
  40cef7:	6a 03                	push   0x3
  40cef9:	e8 32 e3 ff ff       	call   0x40b230
  40cefe:	83 c4 04             	add    esp,0x4
  40cf01:	e9 3a 01 00 00       	jmp    0x40d040
  40cf06:	53                   	push   ebx
  40cf07:	6a 03                	push   0x3
  40cf09:	6a 02                	push   0x2
  40cf0b:	e8 80 6c ff ff       	call   0x403b90
  40cf10:	6a 00                	push   0x0
