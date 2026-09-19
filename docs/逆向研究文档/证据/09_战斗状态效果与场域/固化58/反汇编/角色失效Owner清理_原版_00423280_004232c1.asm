
/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423280 <.text+0x22280>:
  423280:	0a 00                	or     al,BYTE PTR [eax]
  423282:	00 6a 00             	add    BYTE PTR [edx+0x0],ch
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
  4232a3:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4232a8:	e8 b3 a9 01 00       	call   0x43dc60
  4232ad:	8b 16                	mov    edx,DWORD PTR [esi]
  4232af:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4232b4:	52                   	push   edx
  4232b5:	e8 a6 a9 01 00       	call   0x43dc60
  4232ba:	8b ce                	mov    ecx,esi
  4232bc:	e8 af 01 00 00       	call   0x423470
