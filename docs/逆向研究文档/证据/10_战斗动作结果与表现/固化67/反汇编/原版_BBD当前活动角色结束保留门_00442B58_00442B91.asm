
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442b58 <.text+0x41b58>:
  442b58:	8b 85 24 e2 00 00    	mov    eax,DWORD PTR [ebp+0xe224]
  442b5e:	83 cf ff             	or     edi,0xffffffff
  442b61:	3b c7                	cmp    eax,edi
  442b63:	0f 84 82 00 00 00    	je     0x442beb
  442b69:	8d 14 c0             	lea    edx,[eax+eax*8]
  442b6c:	8d 0c 90             	lea    ecx,[eax+edx*4]
  442b6f:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  442b72:	8d 14 c8             	lea    edx,[eax+ecx*8]
  442b75:	8b 84 95 c8 10 00 00 	mov    eax,DWORD PTR [ebp+edx*4+0x10c8]
  442b7c:	3d bd 0b 00 00       	cmp    eax,0xbbd
  442b81:	74 68                	je     0x442beb
  442b83:	3d bc 0b 00 00       	cmp    eax,0xbbc
  442b88:	74 61                	je     0x442beb
  442b8a:	3d c1 0b 00 00       	cmp    eax,0xbc1
  442b8f:	74 5a                	je     0x442beb
