  42227b:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  422281:	81 39 ea 01 00 00    	cmp    DWORD PTR [ecx],0x1ea
  422287:	75 3e                	jne    0x4222c7
  422289:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  42228e:	8b 88 d8 00 00 00    	mov    ecx,DWORD PTR [eax+0xd8]
  422294:	51                   	push   ecx
  422295:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  42229b:	e8 c0 22 00 00       	call   0x424560
  4222a0:	8b f8                	mov    edi,eax
  4222a2:	e8 be f2 02 00       	call   0x451565
  4222a7:	99                   	cdq
  4222a8:	b9 64 00 00 00       	mov    ecx,0x64
  4222ad:	f7 f9                	idiv   ecx
  4222af:	3b d7                	cmp    edx,edi
  4222b1:	7d 14                	jge    0x4222c7
  4222b3:	8b 15 74 fd 89 00    	mov    edx,DWORD PTR ds:0x89fd74
  4222b9:	c6 82 fc 05 00 00 01 	mov    BYTE PTR [edx+0x5fc],0x1
  4222c0:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  4222c7:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
