
/mnt/data/work49/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b1b0 <.text+0x2a1b0>:
  42b1b0:	8b cf                	mov    ecx,edi
  42b1b2:	c1 e1 04             	shl    ecx,0x4
  42b1b5:	8b b1 0c a0 46 00    	mov    esi,DWORD PTR [ecx+0x46a00c]
  42b1bb:	eb 02                	jmp    0x42b1bf
  42b1bd:	33 f6                	xor    esi,esi
  42b1bf:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42b1c3:	85 c0                	test   eax,eax
  42b1c5:	0f 8e b2 03 00 00    	jle    0x42b57d
  42b1cb:	83 ff 14             	cmp    edi,0x14
  42b1ce:	0f 8f 3a 01 00 00    	jg     0x42b30e
  42b1d4:	8b             	mov    edx,DWORD PTR [esp+0x30]
