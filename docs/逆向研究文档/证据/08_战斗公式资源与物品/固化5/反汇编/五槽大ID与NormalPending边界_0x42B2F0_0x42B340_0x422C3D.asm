  42b2f1:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  42b2f5:	0f af 54 24 28       	imul   edx,DWORD PTR [esp+0x28]
  42b2fa:	f7 ea                	imul   edx
  42b2fc:	c1 fa 05             	sar    edx,0x5
  42b2ff:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  42b303:	8b c2                	mov    eax,edx
  42b305:	c1 e8 1f             	shr    eax,0x1f
  42b308:	03 d0                	add    edx,eax
  42b30a:	8b f2                	mov    esi,edx
  42b30c:	eb 05                	jmp    0x42b313
  42b30e:	be 64 00 00 00       	mov    esi,0x64
  42b313:	83 ff 1b             	cmp    edi,0x1b
  42b316:	7e 15                	jle    0x42b32d
  42b318:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  42b31c:	89 7c 98 10          	mov    DWORD PTR [eax+ebx*4+0x10],edi
  42b320:	c7 44 98 24 40 1f 00 	mov    DWORD PTR [eax+ebx*4+0x24],0x1f40
  42b327:	00 
  42b328:	e9 50 02 00 00       	jmp    0x42b57d
  42b32d:	e8 33 62 02 00       	call   0x451565
  42b332:	99                   	cdq
  42b333:	b9 64 00 00 00       	mov    ecx,0x64
  42b338:	f7 f9                	idiv   ecx
  42b33a:	3b d6                	cmp    edx,esi
  42b33c:	7c 0c                	jl     0x42b34a
  42b33e:	8a 44 24 44          	mov    al,BYTE PTR [esp+0x44]

; ---- normal pending writer ----
  422c3d:	8b 7d 00             	mov    edi,DWORD PTR [ebp+0x0]
  422c40:	85 ff                	test   edi,edi
  422c42:	7e 2a                	jle    0x422c6e
  422c44:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  422c48:	03 c2                	add    eax,edx
  422c4a:	8b 04 86             	mov    eax,DWORD PTR [esi+eax*4]
  422c4d:	83 f8 1b             	cmp    eax,0x1b
  422c50:	7f 1c                	jg     0x422c6e
  422c52:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  422c56:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  422c5d:	83 bc 81 34 0a 00 00 	cmp    DWORD PTR [ecx+eax*4+0xa34],0x0
  422c64:	00 
  422c65:	7f 07                	jg     0x422c6e
  422c67:	89 bc 81 cc 00 00 00 	mov    DWORD PTR [ecx+eax*4+0xcc],edi
  422c6e:	42                   	inc    edx
  422c6f:	83 c5 04             	add    ebp,0x4
