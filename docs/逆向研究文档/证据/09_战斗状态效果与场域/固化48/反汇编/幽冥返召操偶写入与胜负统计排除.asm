; 固化48：幽冥返召把raw16 runtime槽写1,000,000；胜负判定排除该槽非零单位。

/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00421fed <.text+0x20fed>:
  421fed:	39 9f c8 00 00 00    	cmp    DWORD PTR [edi+0xc8],ebx
  421ff3:	7e 42                	jle    0x422037
  421ff5:	8b bf 88 00 00 00    	mov    edi,DWORD PTR [edi+0x88]
  421ffb:	8b 0c bd 94 fd 89 00 	mov    ecx,DWORD PTR [edi*4+0x89fd94]
  422002:	c7 81 7c 08 00 00 01 	mov    DWORD PTR [ecx+0x87c],0x1
  422009:	00 00 00 
  42200c:	8b 14 bd 94 fd 89 00 	mov    edx,DWORD PTR [edi*4+0x89fd94]
  422013:	5f                   	pop    edi
  422014:	5e                   	pop    esi
  422015:	5d                   	pop    ebp
  422016:	c7 82 74 0a 00 00 40 	mov    DWORD PTR [edx+0xa74],0xf4240
  42201d:	42 0f 00 
  422020:	5b                   	pop    ebx
  422021:	83 c4 08             	add    esp,0x8
  422024:	c2               	ret    0x4


/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443590 <.text+0x42590>:
  443590:	53                   	push   ebx
  443591:	55                   	push   ebp
  443592:	56                   	push   esi
  443593:	57                   	push   edi
  443594:	8b f9                	mov    edi,ecx
  443596:	33 ed                	xor    ebp,ebp
  443598:	33 f6                	xor    esi,esi
  44359a:	32 db                	xor    bl,bl
  44359c:	8d 87 34 0c 00 00    	lea    eax,[edi+0xc34]
  4435a2:	ba 10 00 00 00       	mov    edx,0x10
  4435a7:	83 b8 dc fe ff ff ff 	cmp    DWORD PTR [eax-0x124],0xffffffff
  4435ae:	74 19                	je     0x4435c9
  4435b0:	8b 88 24 02 00 00    	mov    ecx,DWORD PTR [eax+0x224]
  4435b6:	85 c9                	test   ecx,ecx
  4435b8:	75 0f                	jne    0x4435c9
  4435ba:	8b 08                	mov    ecx,DWORD PTR [eax]
  4435bc:	85 c9                	test   ecx,ecx
  4435be:	75 03                	jne    0x4435c3
  4435c0:	46                   	inc    esi
  4435c1:	eb 06                	jmp    0x4435c9
  4435c3:	83 f9 01             	cmp    ecx,0x1
  4435c6:	75 01                	jne    0x4435c9
  4435c8:	45                   	inc    ebp
  4435c9:	05 e4 0d 00 00       	add    eax,0xde4
  4435ce:	4a                   	dec    edx
  4435cf:	75 d6                	jne    0x4435a7
  4435d1:	a1 48 1c 8e        	mov    eax,ds:0x8e1c48
