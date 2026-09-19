
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0043d300 <.text+0x3c300>:
  43d300:	56                   	push   esi
  43d301:	8b f1                	mov    esi,ecx
  43d303:	8a 46 14             	mov    al,BYTE PTR [esi+0x14]
  43d306:	84 c0                	test   al,al
  43d308:	74 12                	je     0x43d31c
  43d30a:	8b 46 10             	mov    eax,DWORD PTR [esi+0x10]
  43d30d:	85 c0                	test   eax,eax
  43d30f:	75 59                	jne    0x43d36a
  43d311:	c7 46 10 01 00 00 00 	mov    DWORD PTR [esi+0x10],0x1
  43d318:	5e                   	pop    esi
  43d319:	c2 08 00             	ret    0x8
  43d31c:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  43d320:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  43d324:	51                   	push   ecx
  43d325:	8b 4e 20             	mov    ecx,DWORD PTR [esi+0x20]
  43d328:	89 46 0c             	mov    DWORD PTR [esi+0xc],eax
  43d32b:	e8 30 11 00 00       	call   0x43e460
  43d330:	8b 4e 20             	mov    ecx,DWORD PTR [esi+0x20]
  43d333:	6a 03                	push   0x3
  43d335:	6a 01                	push   0x1
  43d337:	8b 51 24             	mov    edx,DWORD PTR [ecx+0x24]
  43d33a:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  43d33d:	8b 41 28             	mov    eax,DWORD PTR [ecx+0x28]
  43d340:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  43d343:	e8 98 12 00 00       	call   0x43e5e0
  43d348:	8b 4e 20             	mov    ecx,DWORD PTR [esi+0x20]
  43d34b:	6a 00                	push   0x0
  43d34d:	e8 4e 13 00 00       	call   0x43e6a0
  43d352:	e8 0e 42 01 00       	call   0x451565
  43d357:	25 01 00 00 80       	and    eax,0x80000001
  43d35c:	79 05                	jns    0x43d363
  43d35e:	48                   	dec    eax
  43d35f:	83 c8 fe             	or     eax,0xfffffffe
  43d362:	40                   	inc    eax
  43d363:	89 46 10             	mov    DWORD PTR [esi+0x10],eax
  43d366:	c6 46 14 01          	mov    BYTE PTR [esi+0x14],0x1
  43d36a:	5e                   	pop    esi
  43d36b:	c2               	ret    0x8
