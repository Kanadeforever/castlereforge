
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004310e0 <.text+0x300e0>:
  4310e0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4310e4:	56                   	push   esi
  4310e5:	8b f1                	mov    esi,ecx
  4310e7:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  4310eb:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4310ef:	89 86 68 05 00 00    	mov    DWORD PTR [esi+0x568],eax
  4310f5:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4310f9:	89 86 74 05 00 00    	mov    DWORD PTR [esi+0x574],eax
  4310ff:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  431103:	85 c0                	test   eax,eax
  431105:	89 8e 6c 05 00 00    	mov    DWORD PTR [esi+0x56c],ecx
  43110b:	89 96 70 05 00 00    	mov    DWORD PTR [esi+0x570],edx
  431111:	74 23                	je     0x431136
  431113:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  431119:	50                   	push   eax
  43111a:	e8 41 d3 00 00       	call   0x43e460
  43111f:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  431125:	6a 00                	push   0x0
  431127:	c6 41 09 01          	mov    BYTE PTR [ecx+0x9],0x1
  43112b:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  431131:	e8 ba d5 00 00       	call   0x43e6f0
  431136:	5e                   	pop    esi
  431137:	c2               	ret    0x14
