
/mnt/data/RPG.exe:     file format pei-i386


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
  422024:	c2                   	.byte 0xc2
