
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420170 <.text+0x1f170>:
  420170:	53                   	push   ebx
  420171:	55                   	push   ebp
  420172:	56                   	push   esi
  420173:	57                   	push   edi
  420174:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  420178:	8b f1                	mov    esi,ecx
  42017a:	85 ff                	test   edi,edi
  42017c:	7d 09                	jge    0x420187
  42017e:	5f                   	pop    edi
  42017f:	5e                   	pop    esi
  420180:	5d                   	pop    ebp
  420181:	32 c0                	xor    al,al
  420183:	5b                   	pop    ebx
  420184:	c2 08 00             	ret    0x8
  420187:	8b 04 bd 94 fd 89 00 	mov    eax,DWORD PTR [edi*4+0x89fd94]
  42018e:	85 c0                	test   eax,eax
  420190:	75 09                	jne    0x42019b
  420192:	5f                   	pop    edi
  420193:	5e                   	pop    esi
  420194:	5d                   	pop    ebp
  420195:	32 c0                	xor    al,al
  420197:	5b                   	pop    ebx
  420198:	c2 08 00             	ret    0x8
  42019b:	8b 86 2c 07 00 00    	mov    eax,DWORD PTR [esi+0x72c]
  4201a1:	85 c0                	test   eax,eax
  4201a3:	7d 09                	jge    0x4201ae
  4201a5:	5f                   	pop    edi
  4201a6:	5e                   	pop    esi
  4201a7:	5d                   	pop    ebp
  4201a8:	32 c0                	xor    al,al
  4201aa:	5b                   	pop    ebx
  4201ab:	c2 08 00             	ret    0x8
  4201ae:	8b 86 d0 0d 00 00    	mov    eax,DWORD PTR [esi+0xdd0]
  4201b4:	85 c0                	test   eax,eax
  4201b6:	7d 19                	jge    0x4201d1
  4201b8:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4201be:	68 d8 9f 46 00       	push   0x469fd8
  4201c3:	6a 00                	push   0x0
  4201c5:	e8 a6 1c 01 00       	call   0x431e70
  4201ca:	5f                   	pop    edi
  4201cb:	5e                   	pop    esi
  4201cc:	5d                   	pop    ebp
  4201cd:	5b                   	pop    ebx
  4201ce:	c2 08 00             	ret    0x8
  4201d1:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  4201d8:	2b c8                	sub    ecx,eax
  4201da:	8d 14 88             	lea    edx,[eax+ecx*4]
  4201dd:	c1 e2 04             	shl    edx,0x4
  4201e0:	2b d0                	sub    edx,eax
  4201e2:	a1 b0 01 8a 00       	mov    eax,ds:0x8a01b0
  4201e7:	8d 04 50             	lea    eax,[eax+edx*2]
  4201ea:	89 86 f4 0c 00 00    	mov    DWORD PTR [esi+0xcf4],eax
  4201f0:	8b 0c bd 94 fd 89 00 	mov    ecx,DWORD PTR [edi*4+0x89fd94]
  4201f7:	8b 91 2c 07 00 00    	mov    edx,DWORD PTR [ecx+0x72c]
  4201fd:	85 d2                	test   edx,edx
  4201ff:	7d 61                	jge    0x420262
  420201:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  420204:	3d 0f 01 00 00       	cmp    eax,0x10f
  420209:	74 57                	je     0x420262
  42020b:	3d 10 01 00 00       	cmp    eax,0x110
