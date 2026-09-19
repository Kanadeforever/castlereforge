
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004231d0 <.text+0x221d0>:
  4231d0:	57                   	push   edi
  4231d1:	8d b9 34 0a 00 00    	lea    edi,[ecx+0xa34]
  4231d7:	b9 1c 00 00 00       	mov    ecx,0x1c
  4231dc:	33 c0                	xor    eax,eax
  4231de:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4231e0:	5f                   	pop    edi
  4231e1:	c3                   	ret
  4231e2:	90                   	nop
  4231e3:	90                   	nop
  4231e4:	90                   	nop
  4231e5:	90                   	nop
  4231e6:	90                   	nop
  4231e7:	90                   	nop
  4231e8:	90                   	nop
  4231e9:	90                   	nop
  4231ea:	90                   	nop
  4231eb:	90                   	nop
  4231ec:	90                   	nop
  4231ed:	90                   	nop
  4231ee:	90                   	nop
  4231ef:	90                   	nop
  4231f0:	56                   	push   esi
  4231f1:	8b f1                	mov    esi,ecx
  4231f3:	8b 86 7c 08 00 00    	mov    eax,DWORD PTR [esi+0x87c]
  4231f9:	85 c0                	test   eax,eax
  4231fb:	0f 8f c2 00 00 00    	jg     0x4232c3
  423201:	6a 01                	push   0x1
  423203:	e8 a8 d5 ff ff       	call   0x4207b0
  423208:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  42320e:	c7 86 2c 07 00 00 ff 	mov    DWORD PTR [esi+0x72c],0xffffffff
  423215:	ff ff ff 
  423218:	c7 86 7c 08 00 00 00 	mov    DWORD PTR [esi+0x87c],0x0
  42321f:	00 00 00 
  423222:	6a ff                	push   0xffffffff
  423224:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  423227:	c6 81 58 03 00 00 00 	mov    BYTE PTR [ecx+0x358],0x0
  42322e:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  423234:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  423237:	e8 d4 bf 01 00       	call   0x43f210
  42323c:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  423242:	c7 86 e4 0c 00 00 b9 	mov    DWORD PTR [esi+0xce4],0xbb9
  423249:	0b 00 00 
  42324c:	85 c0                	test   eax,eax
  42324e:	75 06                	jne    0x423256
  423250:	ff 05 f8 fd 89 00    	inc    DWORD PTR ds:0x89fdf8
  423256:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  42325c:	55                   	push   ebp
  42325d:	57                   	push   edi
  42325e:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  423261:	8b 78 24             	mov    edi,DWORD PTR [eax+0x24]
  423264:	8b 68 28             	mov    ebp,DWORD PTR [eax+0x28]
  423267:	81 c7 25 01 00 00    	add    edi,0x125
  42326d:	81 c5 f6 00 00 00    	add    ebp,0xf6
  423273:	e8 78 9f 01 00       	call   0x43d1f0
  423278:	6a 01                	push   0x1
  42327a:	50                   	push   eax
  42327b:	55                   	push   ebp
  42327c:	57                   	push   edi
  42327d:	8d 86 d4 0a 00 00    	lea    eax,[esi+0xad4]
  423283:	6a 00                	push   0x0
  423285:	50                   	push   eax
  423286:	8b ce                	mov    ecx,esi
  423288:	e8 73 c4 ff ff       	call   0x41f700
  42328d:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  423293:	50                   	push   eax
  423294:	e8 27 a7 00 00       	call   0x42d9c0
  423299:	8b ce                	mov    ecx,esi
  42329b:	e8 30 ff ff ff       	call   0x4231d0
  4232a0:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4232a2:	51                   	push   ecx
  4232a3:	b9 08          	mov    ecx,0x89fe08
