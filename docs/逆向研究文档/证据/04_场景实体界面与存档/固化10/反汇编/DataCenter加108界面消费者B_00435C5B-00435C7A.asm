
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00435c5b <.text+0x34c5b>:
  435c5b:	83 f8 02             	cmp    eax,0x2
  435c5e:	75 15                	jne    0x435c75
  435c60:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  435c66:	8b 82 08 01 00 00    	mov    eax,DWORD PTR [edx+0x108]
  435c6c:	85 c0                	test   eax,eax
  435c6e:	75 05                	jne    0x435c75
  435c70:	bb 07 00 00 00       	mov    ebx,0x7
  435c75:	8b                   	.byte 0x8b
  435c76:	86 20                	xchg   BYTE PTR [eax],ah
  435c78:	06                   	push   es
	...
