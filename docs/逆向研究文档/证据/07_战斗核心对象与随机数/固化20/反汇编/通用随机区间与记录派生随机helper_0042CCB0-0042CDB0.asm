
/mnt/data/RPG.exe:	file format coff-i386

Disassembly of section .text:

00401000 <.text>:
  42ccb0: 8b 44 24 04                  	mov	eax, dword ptr [esp + 0x4]
  42ccb4: 8b 4c 24 08                  	mov	ecx, dword ptr [esp + 0x8]
  42ccb8: 56                           	push	esi
  42ccb9: 3b c1                        	cmp	eax, ecx
  42ccbb: 57                           	push	edi
  42ccbc: 7e 08                        	jle	0x42ccc6 <.text+0x2bcc6>
  42ccbe: 2b c1                        	sub	eax, ecx
  42ccc0: 8b f9                        	mov	edi, ecx
  42ccc2: 8b f0                        	mov	esi, eax
  42ccc4: eb 06                        	jmp	0x42cccc <.text+0x2bccc>
  42ccc6: 2b c8                        	sub	ecx, eax
  42ccc8: 8b f8                        	mov	edi, eax
  42ccca: 8b f1                        	mov	esi, ecx
  42cccc: 85 f6                        	test	esi, esi
  42ccce: 75 09                        	jne	0x42ccd9 <.text+0x2bcd9>
  42ccd0: 33 c0                        	xor	eax, eax
  42ccd2: 8b c7                        	mov	eax, edi
  42ccd4: 5f                           	pop	edi
  42ccd5: 5e                           	pop	esi
  42ccd6: c2 08 00                     	ret	0x8
  42ccd9: e8 87 48 02 00               	call	0x451565 <.text+0x50565>
  42ccde: 99                           	cdq
  42ccdf: f7 fe                        	idiv	esi
  42cce1: 8d 04 3a                     	lea	eax, [edx + edi]
  42cce4: 5f                           	pop	edi
  42cce5: 5e                           	pop	esi
  42cce6: c2 08 00                     	ret	0x8
  42cce9: 90                           	nop
  42ccea: 90                           	nop
  42cceb: 90                           	nop
  42ccec: 90                           	nop
  42cced: 90                           	nop
  42ccee: 90                           	nop
  42ccef: 90                           	nop
  42ccf0: 56                           	push	esi
  42ccf1: 8b 74 24 08                  	mov	esi, dword ptr [esp + 0x8]
  42ccf5: 8b 8e 48 02 00 00            	mov	ecx, dword ptr [esi + 0x248]
  42ccfb: 8b 46 44                     	mov	eax, dword ptr [esi + 0x44]
  42ccfe: 85 c9                        	test	ecx, ecx
  42cd00: 7e 19                        	jle	0x42cd1b <.text+0x2bd1b>
  42cd02: 8d 0c 40                     	lea	ecx, [eax + 2*eax]
  42cd05: 8d 0c 88                     	lea	ecx, [eax + 4*ecx]
  42cd08: b8 67 66 66 66               	mov	eax, 0x66666667
  42cd0d: f7 e9                        	imul	ecx
  42cd0f: 8b c2                        	mov	eax, edx
  42cd11: c1 f8 02                     	sar	eax, 0x2
  42cd14: 8b d0                        	mov	edx, eax
  42cd16: c1 ea 1f                     	shr	edx, 0x1f
  42cd19: 03 c2                        	add	eax, edx
  42cd1b: 8b 8e fc 01 00 00            	mov	ecx, dword ptr [esi + 0x1fc]
  42cd21: 5e                           	pop	esi
  42cd22: 85 c9                        	test	ecx, ecx
  42cd24: 7e 1c                        	jle	0x42cd42 <.text+0x2bd42>
  42cd26: 8d 0c c5 00 00 00 00         	lea	ecx, [8*eax]
  42cd2d: 2b c8                        	sub	ecx, eax
  42cd2f: b8 67 66 66 66               	mov	eax, 0x66666667
  42cd34: f7 e9                        	imul	ecx
  42cd36: c1 fa 02                     	sar	edx, 0x2
  42cd39: 8b c2                        	mov	eax, edx
  42cd3b: c1 e8 1f                     	shr	eax, 0x1f
  42cd3e: 03 d0                        	add	edx, eax
  42cd40: 8b c2                        	mov	eax, edx
  42cd42: c2 04 00                     	ret	0x4
  42cd45: 90                           	nop
  42cd46: 90                           	nop
  42cd47: 90                           	nop
  42cd48: 90                           	nop
  42cd49: 90                           	nop
  42cd4a: 90                           	nop
  42cd4b: 90                           	nop
  42cd4c: 90                           	nop
  42cd4d: 90                           	nop
  42cd4e: 90                           	nop
  42cd4f: 90                           	nop
  42cd50: 8b 44 24 04                  	mov	eax, dword ptr [esp + 0x4]
  42cd54: 56                           	push	esi
  42cd55: 57                           	push	edi
  42cd56: 33 ff                        	xor	edi, edi
  42cd58: 8b 80 90 00 00 00            	mov	eax, dword ptr [eax + 0x90]
  42cd5e: 85 c0                        	test	eax, eax
  42cd60: 7e 40                        	jle	0x42cda2 <.text+0x2bda2>
  42cd62: 8d 0c c5 00 00 00 00         	lea	ecx, [8*eax]
  42cd69: 2b c8                        	sub	ecx, eax
  42cd6b: 8d 14 88                     	lea	edx, [eax + 4*ecx]
  42cd6e: c1 e2 04                     	shl	edx, 0x4
  42cd71: 2b d0                        	sub	edx, eax
  42cd73: a1 c8 01 8b 00               	mov	eax, dword ptr [0x8b01c8]
  42cd78: 8b 4c 50 38                  	mov	ecx, dword ptr [eax + 2*edx + 0x38]
  42cd7c: b8 67 66 66 66               	mov	eax, 0x66666667
  42cd81: f7 e9                        	imul	ecx
  42cd83: c1 fa 03                     	sar	edx, 0x3
  42cd86: 8b ca                        	mov	ecx, edx
  42cd88: c1 e9 1f                     	shr	ecx, 0x1f
  42cd8b: 03 d1                        	add	edx, ecx
  42cd8d: 8b f2                        	mov	esi, edx
  42cd8f: 85 f6                        	test	esi, esi
  42cd91: 7e 0f                        	jle	0x42cda2 <.text+0x2bda2>
  42cd93: e8 cd 47 02 00               	call	0x451565 <.text+0x50565>
  42cd98: 99                           	cdq
  42cd99: f7 fe                        	idiv	esi
  42cd9b: 5f                           	pop	edi
  42cd9c: 5e                           	pop	esi
  42cd9d: 8b c2                        	mov	eax, edx
  42cd9f: c2 04 00                     	ret	0x4
  42cda2: 8b c7                        	mov	eax, edi
  42cda4: 5f                           	pop	edi
  42cda5: 5e                           	pop	esi
  42cda6: c2 04 00                     	ret	0x4
  42cda9: 90                           	nop
  42cdaa: 90                           	nop
  42cdab: 90                           	nop
  42cdac: 90                           	nop
  42cdad: 90                           	nop
  42cdae: 90                           	nop
  42cdaf: 90                           	nop
