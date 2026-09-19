
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00422360 <.text+0x21360>:
  422360:	00 00                	add    BYTE PTR [eax],al
  422362:	85 c9                	test   ecx,ecx
  422364:	74 06                	je     0x42236c
  422366:	8d 81 b0 00 00 00    	lea    eax,[ecx+0xb0]
  42236c:	8b 8e 50 08 00 00    	mov    ecx,DWORD PTR [esi+0x850]
  422372:	51                   	push   ecx
  422373:	6a 00                	push   0x0
  422375:	50                   	push   eax
  422376:	8b ce                	mov    ecx,esi
  422378:	e8 83 d3 ff ff       	call   0x41f700
  42237d:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  422383:	50                   	push   eax
  422384:	e8 77 af 01 00       	call   0x43d300
  422389:	b3 01                	mov    bl,0x1
  42238b:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  422391:	5f                   	pop    edi
  422392:	5d                   	pop    ebp
  422393:	8a c3                	mov    al,bl
  422395:	5e                   	pop    esi
  422396:	5b                   	pop    ebx
  422397:	c2               	ret    0xc
