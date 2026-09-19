  40ce23:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  40ce28:	8b 88 ec 00 00 00    	mov    ecx,DWORD PTR [eax+0xec]
  40ce2e:	3b cf                	cmp    ecx,edi
  40ce30:	0f 8e 53 02 00 00    	jle    0x40d089
  40ce36:	8b 80 c4 00 00 00    	mov    eax,DWORD PTR [eax+0xc4]
  40ce3c:	83 c0 fd             	add    eax,0xfffffffd
  40ce3f:	83 f8 0e             	cmp    eax,0xe
  40ce42:	0f 87 f2 01 00 00    	ja     0x40d03a
  40ce48:	33 d2                	xor    edx,edx
  40ce4a:	8a 90 10 d1 40 00    	mov    dl,BYTE PTR [eax+0x40d110]
  40ce50:	ff 24 95 ec d0 40 00 	jmp    DWORD PTR [edx*4+0x40d0ec]
  40ce57:	a1 f8 f7 89 00       	mov    eax,ds:0x89f7f8
  40ce5c:	85 c0                	test   eax,eax
  40ce5e:	7e 15                	jle    0x40ce75
  40ce60:	39 0d 00 f8 89 00    	cmp    DWORD PTR ds:0x89f800,ecx
  40ce66:	75 0d                	jne    0x40ce75
  40ce68:	e8 d3 e4 ff ff       	call   0x40b340
  40ce6d:	a1 f8 f7 89 00       	mov    eax,ds:0x89f7f8
  40ce72:	50                   	push   eax
  40ce73:	eb 40                	jmp    0x40ceb5
  40ce75:	53                   	push   ebx
  40ce76:	6a 02                	push   0x2
  40ce78:	6a 02                	push   0x2
  40ce7a:	e8 11 6d ff ff       	call   0x403b90
  40ce7f:	6a 00                	push   0x0
  40ce81:	6a 00                	push   0x0
  40ce83:	53                   	push   ebx
  40ce84:	e8 d7 6d ff ff       	call   0x403c60
  40ce89:	83 c4 18             	add    esp,0x18
  40ce8c:	88 1d 77 f6 46 00    	mov    BYTE PTR ds:0x46f677,bl
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
  40cf12:	6a 00                	push   0x0
  40cf14:	53                   	push   ebx
  40cf15:	e8 46 6d ff ff       	call   0x403c60
  40cf1a:	83 c4 18             	add    esp,0x18
  40cf1d:	88 1d 77 f6 46 00    	mov    BYTE PTR ds:0x46f677,bl
