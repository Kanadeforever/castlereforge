
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00413640 <.text+0x12640>:
  413640:	46                   	inc    esi
  413641:	00 8d 8d 68 05 00    	add    BYTE PTR [ebp+0x5688d],cl
  413647:	00 53 53             	add    BYTE PTR [ebx+0x53],dl
  41364a:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  41364e:	8d 95 6c 05 00 00    	lea    edx,[ebp+0x56c]
  413654:	53                   	push   ebx
  413655:	53                   	push   ebx
  413656:	8b cd                	mov    ecx,ebp
  413658:	89 44 24 40          	mov    DWORD PTR [esp+0x40],eax
  41365c:	89 54 24 30          	mov    DWORD PTR [esp+0x30],edx
  413660:	e8 7b da 01 00       	call   0x4310e0
  413665:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  41366a:	8b 88 d0 00 00 00    	mov    ecx,DWORD PTR [eax+0xd0]
  413670:	51                   	push   ecx
  413671:	8b 8d 64 05 00 00    	mov    ecx,DWORD PTR [ebp+0x564]
  413677:	e8 74 b0 02 00       	call   0x43e6f0
  41367c:	6a 2c                	push   0x2c
  41367e:	e8                   	.byte 0xe8
  41367f:	dc                   	.byte 0xdc
