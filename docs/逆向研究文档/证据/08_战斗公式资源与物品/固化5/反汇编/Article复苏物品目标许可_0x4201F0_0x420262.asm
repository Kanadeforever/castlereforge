  4201f0:	8b 0c bd 94 fd 89 00 	mov    ecx,DWORD PTR [edi*4+0x89fd94]
  4201f7:	8b 91 2c 07 00 00    	mov    edx,DWORD PTR [ecx+0x72c]
  4201fd:	85 d2                	test   edx,edx
  4201ff:	7d 61                	jge    0x420262
  420201:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  420204:	3d 0f 01 00 00       	cmp    eax,0x10f
  420209:	74 57                	je     0x420262
  42020b:	3d 10 01 00 00       	cmp    eax,0x110
  420210:	74 50                	je     0x420262
  420212:	3d 11 01 00 00       	cmp    eax,0x111
  420217:	74 49                	je     0x420262
  420219:	8b 86 c8 00 00 00    	mov    eax,DWORD PTR [esi+0xc8]
  42021f:	32 d2                	xor    dl,dl
  420221:	33 c9                	xor    ecx,ecx
  420223:	85 c0                	test   eax,eax
  420225:	7e 32                	jle    0x420259
  420227:	8d 9e 88 00 00 00    	lea    ebx,[esi+0x88]
  42022d:	84 d2                	test   dl,dl
  42022f:	75 31                	jne    0x420262
  420231:	8b 03                	mov    eax,DWORD PTR [ebx]
  420233:	8b 2c 85 94 fd 89 00 	mov    ebp,DWORD PTR [eax*4+0x89fd94]
  42023a:	83 bd 2c 07 00 00 00 	cmp    DWORD PTR [ebp+0x72c],0x0
  420241:	7c 04                	jl     0x420247
  420243:	b2 01                	mov    dl,0x1
  420245:	8b f8                	mov    edi,eax
  420247:	8b 86 c8 00 00 00    	mov    eax,DWORD PTR [esi+0xc8]
  42024d:	41                   	inc    ecx
  42024e:	83 c3 04             	add    ebx,0x4
  420251:	3b c8                	cmp    ecx,eax
  420253:	7c d8                	jl     0x42022d
  420255:	84 d2                	test   dl,dl
  420257:	75 09                	jne    0x420262
  420259:	5f                   	pop    edi
  42025a:	5e                   	pop    esi
  42025b:	5d                   	pop    ebp
  42025c:	32 c0                	xor    al,al
  42025e:	5b                   	pop    ebx
  42025f:	c2 08 00             	ret    0x8
  420262:	89 be a0 07 00 00    	mov    DWORD PTR [esi+0x7a0],edi
