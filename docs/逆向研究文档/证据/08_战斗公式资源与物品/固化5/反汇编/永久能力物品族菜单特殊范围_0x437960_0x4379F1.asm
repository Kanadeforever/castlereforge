  437960:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  437964:	56                   	push   esi
  437965:	8b f1                	mov    esi,ecx
  437967:	57                   	push   edi
  437968:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  43796f:	2b c8                	sub    ecx,eax
  437971:	8d 14 88             	lea    edx,[eax+ecx*4]
  437974:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  43797a:	c1 e2 04             	shl    edx,0x4
  43797d:	8b 89 ac 00 00 00    	mov    ecx,DWORD PTR [ecx+0xac]
  437983:	2b d0                	sub    edx,eax
  437985:	8b 7c 51 28          	mov    edi,DWORD PTR [ecx+edx*2+0x28]
  437989:	85 ff                	test   edi,edi
  43798b:	7f 61                	jg     0x4379ee
  43798d:	3d 2d 01 00 00       	cmp    eax,0x12d
  437992:	7c 07                	jl     0x43799b
  437994:	3d 33 01 00 00       	cmp    eax,0x133
  437999:	7e 2c                	jle    0x4379c7
  43799b:	8b 96 ac 05 00 00    	mov    edx,DWORD PTR [esi+0x5ac]
  4379a1:	6a 00                	push   0x0
  4379a3:	6a 00                	push   0x0
  4379a5:	68 34 b1 46 00       	push   0x46b134
  4379aa:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  4379ad:	e8 7e 6a 00 00       	call   0x43e430
  4379b2:	8b 86 ac 05 00 00    	mov    eax,DWORD PTR [esi+0x5ac]
  4379b8:	6a 00                	push   0x0
  4379ba:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  4379bd:	e8 7e 6c 00 00       	call   0x43e640
  4379c2:	5f                   	pop    edi
  4379c3:	5e                   	pop    esi
  4379c4:	c2 04 00             	ret    0x4
  4379c7:	8b 8e ac 05 00 00    	mov    ecx,DWORD PTR [esi+0x5ac]
  4379cd:	6a 00                	push   0x0
  4379cf:	6a 00                	push   0x0
  4379d1:	68 20 b1 46 00       	push   0x46b120
  4379d6:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  4379d9:	e8 52 6a 00 00       	call   0x43e430
  4379de:	8b 96 ac 05 00 00    	mov    edx,DWORD PTR [esi+0x5ac]
  4379e4:	6a 00                	push   0x0
  4379e6:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  4379e9:	e8 52 6c 00 00       	call   0x43e640
  4379ee:	5f                   	pop    edi
  4379ef:	5e                   	pop    esi
  4379f0:	c2 04 00             	ret    0x4
