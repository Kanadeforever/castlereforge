
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040b150 <.text+0xa150>:
  40b150:	56                   	push   esi
  40b151:	8b f1                	mov    esi,ecx
  40b153:	e8 78 01 00 00       	call   0x40b2d0
  40b158:	e8 43 84 ff ff       	call   0x4035a0
  40b15d:	84 c0                	test   al,al
  40b15f:	75 59                	jne    0x40b1ba
  40b161:	8a 86 18 02 00 00    	mov    al,BYTE PTR [esi+0x218]
  40b167:	84 c0                	test   al,al
  40b169:	74 48                	je     0x40b1b3
  40b16b:	68 b8 0b 00 00       	push   0xbb8
  40b170:	56                   	push   esi
  40b171:	8b ce                	mov    ecx,esi
  40b173:	e8 88 93 02 00       	call   0x434500
  40b178:	8a 86 19 02 00 00    	mov    al,BYTE PTR [esi+0x219]
  40b17e:	84 c0                	test   al,al
  40b180:	75 0b                	jne    0x40b18d
  40b182:	8b 8e 80 02 00 00    	mov    ecx,DWORD PTR [esi+0x280]
  40b188:	e8 13 fc ff ff       	call   0x40ada0
  40b18d:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  40b193:	68 07 b2 01 00       	push   0x1b207
  40b198:	68 07 b2 01 00       	push   0x1b207
  40b19d:	68 07 b2 01 00       	push   0x1b207
  40b1a2:	e8 09 f1 ff ff       	call   0x40a2b0
  40b1a7:	e8 b4 01 04 00       	call   0x44b360
  40b1ac:	e8 3f ba ff ff       	call   0x406bf0
  40b1b1:	5e                   	pop    esi
  40b1b2:	c3                   	ret
  40b1b3:	8b 06                	mov    eax,DWORD PTR [esi]
  40b1b5:	8b ce                	mov    ecx,esi
  40b1b7:	ff 50 14             	call   DWORD PTR [eax+0x14]
  40b1ba:	5e                   	pop    esi
  40b1bb:	c3                   	ret
