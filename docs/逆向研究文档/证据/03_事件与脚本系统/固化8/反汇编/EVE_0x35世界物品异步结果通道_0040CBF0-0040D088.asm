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
  40cd1c:	e9 64 ff ff ff       	jmp    0x40cc85
  40cd21:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  40cd26:	39 98 30 01 00 00    	cmp    DWORD PTR [eax+0x130],ebx
  40cd2c:	0f 85 91 00 00 00    	jne    0x40cdc3
  40cd32:	89 b8 30 01 00 00    	mov    DWORD PTR [eax+0x130],edi
  40cd38:	e8 93 a1 ff ff       	call   0x406ed0
  40cd3d:	e8 fe e5 ff ff       	call   0x40b340
  40cd42:	8d 44 24 10          	lea    eax,[esp+0x10]
  40cd46:	68 84 84 46 00       	push   0x468484
  40cd4b:	50                   	push   eax
  40cd4c:	ff 15 94 01 46 00    	call   DWORD PTR ds:0x460194
  40cd52:	83 c4 08             	add    esp,0x8
  40cd55:	e8 46 e6 ff ff       	call   0x40b3a0
  40cd5a:	8b c8                	mov    ecx,eax
  40cd5c:	e8 bf e3 ff ff       	call   0x40b120
  40cd61:	68 84 02 00 00       	push   0x284
  40cd66:	e8 f4 48 04 00       	call   0x45165f
  40cd6b:	83 c4 04             	add    esp,0x4
  40cd6e:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  40cd72:	3b c7                	cmp    eax,edi
  40cd74:	89 5c 24 7c          	mov    DWORD PTR [esp+0x7c],ebx
  40cd78:	74 09                	je     0x40cd83
  40cd7a:	8b c8                	mov    ecx,eax
  40cd7c:	e8 ff e0 ff ff       	call   0x40ae80
  40cd81:	eb 02                	jmp    0x40cd85
  40cd83:	33 c0                	xor    eax,eax
  40cd85:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  40cd89:	c7 44 24 7c ff ff ff 	mov    DWORD PTR [esp+0x7c],0xffffffff
  40cd90:	ff 
  40cd91:	51                   	push   ecx
  40cd92:	8b c8                	mov    ecx,eax
  40cd94:	e8 a7 e1 ff ff       	call   0x40af40
  40cd99:	6a 0a                	push   0xa
  40cd9b:	e8 90 e4 ff ff       	call   0x40b230
  40cda0:	56                   	push   esi
  40cda1:	89 3d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,edi
  40cda7:	e8 a4 47 04 00       	call   0x451550
  40cdac:	83 c4 08             	add    esp,0x8
  40cdaf:	33 c0                	xor    eax,eax
  40cdb1:	5f                   	pop    edi
  40cdb2:	5e                   	pop    esi
  40cdb3:	5b                   	pop    ebx
  40cdb4:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40cdb8:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40cdbf:	83 c4 74             	add    esp,0x74
  40cdc2:	c3                   	ret
  40cdc3:	39 98 0c 01 00 00    	cmp    DWORD PTR [eax+0x10c],ebx
  40cdc9:	75 58                	jne    0x40ce23
  40cdcb:	e8 20 e4 03 00       	call   0x44b1f0
  40cdd0:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  40cdd6:	68 e6 03 00 00       	push   0x3e6
  40cddb:	89 ba 0c 01 00 00    	mov    DWORD PTR [edx+0x10c],edi
  40cde1:	e8 ea e2 03 00       	call   0x44b0d0
  40cde6:	83 c4 04             	add    esp,0x4
  40cde9:	85 c0                	test   eax,eax
  40cdeb:	7e 31                	jle    0x40ce1e
  40cded:	e8 4e e5 ff ff       	call   0x40b340
  40cdf2:	e8 a9 e5 ff ff       	call   0x40b3a0
  40cdf7:	8b 88 80 02 00 00    	mov    ecx,DWORD PTR [eax+0x280]
  40cdfd:	e8 4e e0 ff ff       	call   0x40ae50
  40ce02:	8b c8                	mov    ecx,eax
  40ce04:	e8 f7 c5 ff ff       	call   0x409400
  40ce09:	68 e6 03 00 00       	push   0x3e6
  40ce0e:	e8 bd e2 03 00       	call   0x44b0d0
  40ce13:	50                   	push   eax
  40ce14:	e8 17 e4 ff ff       	call   0x40b230
  40ce19:	83 c4 08             	add    esp,0x8
  40ce1c:	eb 05                	jmp    0x40ce23
  40ce1e:	e8 ad a0 ff ff       	call   0x406ed0
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
  40cfa4:	e9 97 00 00 00       	jmp    0x40d040
  40cfa9:	53                   	push   ebx
  40cfaa:	53                   	push   ebx
  40cfab:	eb 1c                	jmp    0x40cfc9
  40cfad:	39 1d f0 8b 46 00    	cmp    DWORD PTR ds:0x468bf0,ebx
  40cfb3:	75 11                	jne    0x40cfc6
  40cfb5:	e8 86 e3 ff ff       	call   0x40b340
  40cfba:	6a 04                	push   0x4
  40cfbc:	e8 6f e2 ff ff       	call   0x40b230
  40cfc1:	83 c4 04             	add    esp,0x4
  40cfc4:	eb 7a                	jmp    0x40d040
  40cfc6:	53                   	push   ebx
  40cfc7:	6a 04                	push   0x4
  40cfc9:	6a 02                	push   0x2
  40cfcb:	e8 c0 6b ff ff       	call   0x403b90
  40cfd0:	6a 00                	push   0x0
  40cfd2:	53                   	push   ebx
  40cfd3:	53                   	push   ebx
  40cfd4:	e8 87 6c ff ff       	call   0x403c60
  40cfd9:	83 c4 18             	add    esp,0x18
  40cfdc:	88 1d 77 f6 46 00    	mov    BYTE PTR ds:0x46f677,bl
  40cfe2:	eb 5c                	jmp    0x40d040
  40cfe4:	6a 19                	push   0x19
  40cfe6:	e8 e5 e0 03 00       	call   0x44b0d0
  40cfeb:	83 c4 04             	add    esp,0x4
  40cfee:	83 f8 03             	cmp    eax,0x3
  40cff1:	7c 1f                	jl     0x40d012
  40cff3:	53                   	push   ebx
  40cff4:	6a 07                	push   0x7
  40cff6:	6a 02                	push   0x2
  40cff8:	e8 93 6b ff ff       	call   0x403b90
  40cffd:	6a 00                	push   0x0
  40cfff:	6a 07                	push   0x7
  40d001:	53                   	push   ebx
  40d002:	e8 59 6c ff ff       	call   0x403c60
  40d007:	83 c4 18             	add    esp,0x18
  40d00a:	88 1d 77 f6 46 00    	mov    BYTE PTR ds:0x46f677,bl
  40d010:	eb 2e                	jmp    0x40d040
  40d012:	6a 03                	push   0x3
  40d014:	6a 19                	push   0x19
  40d016:	e8 95 e0 03 00       	call   0x44b0b0
  40d01b:	53                   	push   ebx
  40d01c:	6a 06                	push   0x6
  40d01e:	6a 02                	push   0x2
  40d020:	e8 6b 6b ff ff       	call   0x403b90
  40d025:	6a 00                	push   0x0
  40d027:	6a 05                	push   0x5
  40d029:	53                   	push   ebx
  40d02a:	e8 31 6c ff ff       	call   0x403c60
  40d02f:	83 c4 20             	add    esp,0x20
  40d032:	88 1d 77 f6 46 00    	mov    BYTE PTR ds:0x46f677,bl
  40d038:	eb 06                	jmp    0x40d040
  40d03a:	89 0d d0 f7 89 00    	mov    DWORD PTR ds:0x89f7d0,ecx
  40d040:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  40d046:	33 ff                	xor    edi,edi
  40d048:	89 ba ec 00 00 00    	mov    DWORD PTR [edx+0xec],edi
  40d04e:	e8 7d 9e ff ff       	call   0x406ed0
  40d053:	8b 15 10 f8 89 00    	mov    edx,DWORD PTR ds:0x89f810
  40d059:	56                   	push   esi
  40d05a:	42                   	inc    edx
  40d05b:	89 3d 18 f8 89 00    	mov    DWORD PTR ds:0x89f818,edi
  40d061:	89 15 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,edx
  40d067:	88 1d 1d f8 89 00    	mov    BYTE PTR ds:0x89f81d,bl
  40d06d:	e8 de 44 04 00       	call   0x451550
  40d072:	83 c4 04             	add    esp,0x4
  40d075:	33 c0                	xor    eax,eax
  40d077:	5f                   	pop    edi
  40d078:	5e                   	pop    esi
  40d079:	5b                   	pop    ebx
  40d07a:	8b 4c 24 68          	mov    ecx,DWORD PTR [esp+0x68]
  40d07e:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40d085:	83 c4 74             	add    esp,0x74
  40d088:	c3                   	ret
