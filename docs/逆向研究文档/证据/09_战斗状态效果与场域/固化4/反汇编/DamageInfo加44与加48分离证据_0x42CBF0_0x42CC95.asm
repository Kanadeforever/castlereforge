
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042cbf0 <.text+0x2bbf0>:
  42cbf0:	3d 97 01 00 00       	cmp    eax,0x197
  42cbf5:	74 07                	je     0x42cbfe
  42cbf7:	3d d0 01 00 00       	cmp    eax,0x1d0
  42cbfc:	75 0f                	jne    0x42cc0d
  42cbfe:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42cc02:	c6 46 40 01          	mov    BYTE PTR [esi+0x40],0x1
  42cc06:	c7 40 30 01 00 00 00 	mov    DWORD PTR [eax+0x30],0x1
  42cc0d:	8b 47 44             	mov    eax,DWORD PTR [edi+0x44]
  42cc10:	83 f8 02             	cmp    eax,0x2
  42cc13:	74 16                	je     0x42cc2b
  42cc15:	83 f8 03             	cmp    eax,0x3
  42cc18:	74 11                	je     0x42cc2b
  42cc1a:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  42cc1d:	85 c0                	test   eax,eax
  42cc1f:	75 0a                	jne    0x42cc2b
  42cc21:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
  42cc24:	85 c0                	test   eax,eax
  42cc26:	75 03                	jne    0x42cc2b
  42cc28:	c6 06 00             	mov    BYTE PTR [esi],0x0
  42cc2b:	8b 47 18             	mov    eax,DWORD PTR [edi+0x18]
  42cc2e:	85 c0                	test   eax,eax
  42cc30:	75 2b                	jne    0x42cc5d
  42cc32:	8b 9b 90 00 00 00    	mov    ebx,DWORD PTR [ebx+0x90]
  42cc38:	85 db                	test   ebx,ebx
  42cc3a:	7e 21                	jle    0x42cc5d
  42cc3c:	8d 0c dd 00 00 00 00 	lea    ecx,[ebx*8+0x0]
  42cc43:	a1 b0 01 8a 00       	mov    eax,ds:0x8a01b0
  42cc48:	2b cb                	sub    ecx,ebx
  42cc4a:	8d 14 8b             	lea    edx,[ebx+ecx*4]
  42cc4d:	c1 e2 04             	shl    edx,0x4
  42cc50:	2b d3                	sub    edx,ebx
  42cc52:	83 7c 50 34 02       	cmp    DWORD PTR [eax+edx*2+0x34],0x2
  42cc57:	75 04                	jne    0x42cc5d
  42cc59:	c6 46 48 01          	mov    BYTE PTR [esi+0x48],0x1
  42cc5d:	8b 8f e8 01 00 00    	mov    ecx,DWORD PTR [edi+0x1e8]
  42cc63:	5f                   	pop    edi
  42cc64:	89 4e 44             	mov    DWORD PTR [esi+0x44],ecx
  42cc67:	5e                   	pop    esi
  42cc68:	5d                   	pop    ebp
  42cc69:	5b                   	pop    ebx
  42cc6a:	83 c4 0c             	add    esp,0xc
  42cc6d:	c2 14 00             	ret    0x14
  42cc70:	c6 46 0c 00          	mov    BYTE PTR [esi+0xc],0x0
  42cc74:	c6 06 01             	mov    BYTE PTR [esi],0x1
  42cc77:	89 4e 38             	mov    DWORD PTR [esi+0x38],ecx
  42cc7a:	89 4e 04             	mov    DWORD PTR [esi+0x4],ecx
  42cc7d:	89 4e 3c             	mov    DWORD PTR [esi+0x3c],ecx
  42cc80:	89 4e 08             	mov    DWORD PTR [esi+0x8],ecx
  42cc83:	8b 97 e8 01 00 00    	mov    edx,DWORD PTR [edi+0x1e8]
  42cc89:	89 56 44             	mov    DWORD PTR [esi+0x44],edx
  42cc8c:	5f                   	pop    edi
  42cc8d:	5e                   	pop    esi
  42cc8e:	5d                   	pop    ebp
  42cc8f:	5b                   	pop    ebx
  42cc90:	83 c4 0c             	add    esp,0xc
  42cc93:	c2 14              	ret    0x14
