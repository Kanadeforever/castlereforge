
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00434b06 <.text+0x33b06>:
  434b06:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  434b0c:	88 5c 24 3c          	mov    BYTE PTR [esp+0x3c],bl
  434b10:	8b 91 08 01 00 00    	mov    edx,DWORD PTR [ecx+0x108]
  434b16:	85 d2                	test   edx,edx
  434b18:	74 14                	je     0x434b2e
  434b1a:	68 84 b0 46 00       	push   0x46b084
  434b1f:	6a 00                	push   0x0
  434b21:	6a 00                	push   0x0
  434b23:	6a 69                	push   0x69
  434b25:	6a 41                	push   0x41
  434b27:	8b c8                	mov    ecx,eax
  434b29:	e8 b2 c5 ff ff       	call   0x4310e0
  434b2e:	6a 2c                	push   0x2c
  434b30:	e8                   	.byte 0xe8
  434b31:	2a cb                	sub    cl,bl
  434b33:	01                   	.byte 0x1
