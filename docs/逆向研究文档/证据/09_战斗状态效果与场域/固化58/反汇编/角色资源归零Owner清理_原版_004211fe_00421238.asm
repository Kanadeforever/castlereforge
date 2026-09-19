
/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004211fe <.text+0x201fe>:
  4211fe:	c1 e8 1f             	shr    eax,0x1f
  421201:	03 d0                	add    edx,eax
  421203:	03 da                	add    ebx,edx
  421205:	89 99 84 08 00 00    	mov    DWORD PTR [ecx+0x884],ebx
  42120b:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421212:	39 a8 84 08 00 00    	cmp    DWORD PTR [eax+0x884],ebp
  421218:	7f 1c                	jg     0x421236
  42121a:	56                   	push   esi
  42121b:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  421220:	89 a8 84 08 00 00    	mov    DWORD PTR [eax+0x884],ebp
  421226:	e8 35 ca 01 00       	call   0x43dc60
  42122b:	56                   	push   esi
  42122c:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  421231:	e8 2a ca 01 00       	call   0x43dc60
  421236:	8b                   	.byte 0x8b
  421237:	87                   	.byte 0x87
