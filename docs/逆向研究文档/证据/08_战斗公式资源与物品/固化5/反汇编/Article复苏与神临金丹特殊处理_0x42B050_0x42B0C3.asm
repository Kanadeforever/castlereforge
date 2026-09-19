  42b050:	e8 7b 00 00 00       	call   0x42b0d0
  42b055:	81 7b 20 08 01 00 00 	cmp    DWORD PTR [ebx+0x20],0x108
  42b05c:	75 10                	jne    0x42b06e
  42b05e:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  42b061:	f7 d9                	neg    ecx
  42b063:	89 4f 04             	mov    DWORD PTR [edi+0x4],ecx
  42b066:	8b 56 34             	mov    edx,DWORD PTR [esi+0x34]
  42b069:	f7 da                	neg    edx
  42b06b:	89 57 08             	mov    DWORD PTR [edi+0x8],edx
  42b06e:	8b 43 20             	mov    eax,DWORD PTR [ebx+0x20]
  42b071:	3d 0f 01 00 00       	cmp    eax,0x10f
  42b076:	74 0e                	je     0x42b086
  42b078:	3d 10 01 00 00       	cmp    eax,0x110
  42b07d:	74 07                	je     0x42b086
  42b07f:	3d 11 01 00 00       	cmp    eax,0x111
  42b084:	75 0b                	jne    0x42b091
  42b086:	c6 47 40 01          	mov    BYTE PTR [edi+0x40],0x1
  42b08a:	c7 46 30 01 00 00 00 	mov    DWORD PTR [esi+0x30],0x1
  42b091:	8b 83 c8 00 00 00    	mov    eax,DWORD PTR [ebx+0xc8]
  42b097:	83 f8 02             	cmp    eax,0x2
  42b09a:	74 16                	je     0x42b0b2
  42b09c:	83 f8 03             	cmp    eax,0x3
  42b09f:	74 11                	je     0x42b0b2
  42b0a1:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  42b0a4:	85 c0                	test   eax,eax
  42b0a6:	75 0a                	jne    0x42b0b2
  42b0a8:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  42b0ab:	85 c0                	test   eax,eax
  42b0ad:	75 03                	jne    0x42b0b2
  42b0af:	c6 07 00             	mov    BYTE PTR [edi],0x0
  42b0b2:	8b 83 7a 03 00 00    	mov    eax,DWORD PTR [ebx+0x37a]
  42b0b8:	89 47 44             	mov    DWORD PTR [edi+0x44],eax
  42b0bb:	5f                   	pop    edi
  42b0bc:	5e                   	pop    esi
  42b0bd:	5d                   	pop    ebp
  42b0be:	5b                   	pop    ebx
  42b0bf:	83 c4 10             	add    esp,0x10
  42b0c2:	c2 14 00             	ret    0x14
