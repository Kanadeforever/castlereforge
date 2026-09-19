
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042ccb0 <.text+0x2bcb0>:
  42ccb0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  42ccb4:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  42ccb8:	56                   	push   esi
  42ccb9:	3b c1                	cmp    eax,ecx
  42ccbb:	57                   	push   edi
  42ccbc:	7e 08                	jle    0x42ccc6
  42ccbe:	2b c1                	sub    eax,ecx
  42ccc0:	8b f9                	mov    edi,ecx
  42ccc2:	8b f0                	mov    esi,eax
  42ccc4:	eb 06                	jmp    0x42cccc
  42ccc6:	2b c8                	sub    ecx,eax
  42ccc8:	8b f8                	mov    edi,eax
  42ccca:	8b f1                	mov    esi,ecx
  42cccc:	85 f6                	test   esi,esi
  42ccce:	75 09                	jne    0x42ccd9
  42ccd0:	33 c0                	xor    eax,eax
  42ccd2:	8b c7                	mov    eax,edi
  42ccd4:	5f                   	pop    edi
  42ccd5:	5e                   	pop    esi
  42ccd6:	c2 08 00             	ret    0x8
  42ccd9:	e8 87 48 02 00       	call   0x451565
  42ccde:	99                   	cdq
  42ccdf:	f7 fe                	idiv   esi
  42cce1:	8d 04 3a             	lea    eax,[edx+edi*1]
  42cce4:	5f                   	pop    edi
  42cce5:	5e                   	pop    esi
  42cce6:	c2 08 00             	ret    0x8
