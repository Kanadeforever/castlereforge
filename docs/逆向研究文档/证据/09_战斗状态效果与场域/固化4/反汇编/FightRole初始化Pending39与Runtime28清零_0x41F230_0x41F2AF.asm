
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f230 <.text+0x1e230>:
  41f230:	53                   	push   ebx
  41f231:	56                   	push   esi
  41f232:	8b f1                	mov    esi,ecx
  41f234:	33 db                	xor    ebx,ebx
  41f236:	57                   	push   edi
  41f237:	c7 86 2c 07 00 00 ff 	mov    DWORD PTR [esi+0x72c],0xffffffff
  41f23e:	ff ff ff 
  41f241:	89 9e 9c 0b 00 00    	mov    DWORD PTR [esi+0xb9c],ebx
  41f247:	89 9e 24 07 00 00    	mov    DWORD PTR [esi+0x724],ebx
  41f24d:	c7 86 a4 07 00 00 e9 	mov    DWORD PTR [esi+0x7a4],0x3e9
  41f254:	03 00 00 
  41f257:	c7 86 e8 0c 00 00 a0 	mov    DWORD PTR [esi+0xce8],0xfa0
  41f25e:	0f 00 00 
  41f261:	89 9e 48 08 00 00    	mov    DWORD PTR [esi+0x848],ebx
  41f267:	89 9e a4 0b 00 00    	mov    DWORD PTR [esi+0xba4],ebx
  41f26d:	88 5e 04             	mov    BYTE PTR [esi+0x4],bl
  41f270:	c6 86 b0 07 00 00 01 	mov    BYTE PTR [esi+0x7b0],0x1
  41f277:	88 9e 28 07 00 00    	mov    BYTE PTR [esi+0x728],bl
  41f27d:	e8 4e 3f 00 00       	call   0x4231d0
  41f282:	33 c0                	xor    eax,eax
  41f284:	8d 8e cc 00 00 00    	lea    ecx,[esi+0xcc]
  41f28a:	89 19                	mov    DWORD PTR [ecx],ebx
  41f28c:	88 9c 06 e8 06 00 00 	mov    BYTE PTR [esi+eax*1+0x6e8],bl
  41f293:	40                   	inc    eax
  41f294:	83 c1 04             	add    ecx,0x4
  41f297:	83 f8 27             	cmp    eax,0x27
  41f29a:	7c ee                	jl     0x41f28a
  41f29c:	8d be 70 06 00 00    	lea    edi,[esi+0x670]
  41f2a2:	b9 0f 00 00 00       	mov    ecx,0xf
  41f2a7:	83 c8 ff             	or     eax,0xffffffff
  41f2aa:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  41f2ac:	5f                   	pop    edi
  41f2ad:	5e                   	pop    esi
  41f2ae:	5b                   	pop    ebx
  41f2af:	c3                   	ret
