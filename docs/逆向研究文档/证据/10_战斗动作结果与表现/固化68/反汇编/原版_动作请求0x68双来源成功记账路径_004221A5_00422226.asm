
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004221a5 <.text+0x211a5>:
  4221a5:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4221ab:	85 c0                	test   eax,eax
  4221ad:	74 22                	je     0x4221d1
  4221af:	8d b8 9c 00 00 00    	lea    edi,[eax+0x9c]
  4221b5:	8b ce                	mov    ecx,esi
  4221b7:	57                   	push   edi
  4221b8:	e8 73 d7 ff ff       	call   0x41f930
  4221bd:	84 c0                	test   al,al
  4221bf:	0f 85 29 01 00 00    	jne    0x4222ee
  4221c5:	57                   	push   edi
  4221c6:	8b ce                	mov    ecx,esi
  4221c8:	e8 b3 d7 ff ff       	call   0x41f980
  4221cd:	84 c0                	test   al,al
  4221cf:	74 16                	je     0x4221e7
  4221d1:	8d be 2e 0b 00 00    	lea    edi,[esi+0xb2e]
  4221d7:	8b ce                	mov    ecx,esi
  4221d9:	57                   	push   edi
  4221da:	e8 51 d7 ff ff       	call   0x41f930
  4221df:	84 c0                	test   al,al
  4221e1:	0f 85 07 01 00 00    	jne    0x4222ee
  4221e7:	6a 00                	push   0x0
  4221e9:	57                   	push   edi
  4221ea:	8b ce                	mov    ecx,esi
  4221ec:	e8 0f d5 ff ff       	call   0x41f700
  4221f1:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  4221f7:	50                   	push   eax
  4221f8:	e8 b3 af 01 00       	call   0x43d1b0
  4221fd:	84 c0                	test   al,al
  4221ff:	0f 84 e9 00 00 00    	je     0x4222ee
  422205:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  42220b:	6a 00                	push   0x0
  42220d:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  422210:	e8 8b c4 01 00       	call   0x43e6a0
  422215:	b3 01                	mov    bl,0x1
  422217:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  42221d:	5f                   	pop    edi
  42221e:	5d                   	pop    ebp
  42221f:	8a c3                	mov    al,bl
  422221:	5e                   	pop    esi
  422222:	5b                   	pop    ebx
  422223:	c2 0c 00             	ret    0xc
