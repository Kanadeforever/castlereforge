
/mnt/data/RPG.exe:	file format coff-i386

Disassembly of section .text:

00401000 <.text>:
  42bc50: 02 00                        	add	al, byte ptr [eax]
  42bc52: 00 03                        	add	byte ptr [ebx], al
  42bc54: ca 5e 85                     	retf	-0x7aa2
  42bc57: c0 7e 16 6b                  	sar	byte ptr [esi + 0x16], 0x6b
  42bc5b: c9                           	leave
  42bc5c: 0f b8                        	<unknown>
  42bc5e: 67 66 66 66 f7 e9            	addr16		imul	cx
  42bc64: 8b ca                        	mov	ecx, edx
  42bc66: c1 f9 02                     	sar	ecx, 0x2
  42bc69: 8b d1                        	mov	edx, ecx
  42bc6b: c1 ea 1f                     	shr	edx, 0x1f
  42bc6e: 03 ca                        	add	ecx, edx
  42bc70: 33 c0                        	xor	eax, eax
  42bc72: 85 c9                        	test	ecx, ecx
  42bc74: 0f 9c c0                     	setl	al
  42bc77: 48                           	dec	eax
  42bc78: 23 c1                        	and	eax, ecx
  42bc7a: c2 08 00                     	ret	0x8
  42bc7d: 90                           	nop
  42bc7e: 90                           	nop
  42bc7f: 90                           	nop
  42bc80: 8b 44 24 04                  	mov	eax, dword ptr [esp + 0x4]
  42bc84: 56                           	push	esi
  42bc85: 8b 70 3c                     	mov	esi, dword ptr [eax + 0x3c]
  42bc88: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42bc8d: f7 ee                        	imul	esi
  42bc8f: c1 fa 04                     	sar	edx, 0x4
  42bc92: 8b c2                        	mov	eax, edx
  42bc94: c1 e8 1f                     	shr	eax, 0x1f
  42bc97: 03 d0                        	add	edx, eax
  42bc99: b8 67 66 66 66               	mov	eax, 0x66666667
  42bc9e: 52                           	push	edx
  42bc9f: f7 ee                        	imul	esi
  42bca1: c1 fa 02                     	sar	edx, 0x2
  42bca4: 8b c2                        	mov	eax, edx
  42bca6: c1 e8 1f                     	shr	eax, 0x1f
  42bca9: 03 d0                        	add	edx, eax
  42bcab: 52                           	push	edx
  42bcac: e8 ff 0f 00 00               	call	0x42ccb0 <.text+0x2bcb0>
  42bcb1: 8b 4c 24 0c                  	mov	ecx, dword ptr [esp + 0xc]
  42bcb5: 8b 74 24 10                  	mov	esi, dword ptr [esp + 0x10]
  42bcb9: 2b ce                        	sub	ecx, esi
  42bcbb: 5e                           	pop	esi
  42bcbc: 03 c1                        	add	eax, ecx
  42bcbe: 85 c0                        	test	eax, eax
  42bcc0: 7f 05                        	jg	0x42bcc7 <.text+0x2acc7>
  42bcc2: b8 01 00 00 00               	mov	eax, 0x1
  42bcc7: c2 0c 00                     	ret	0xc
  42bcca: 90                           	nop
  42bccb: 90                           	nop
  42bccc: 90                           	nop
  42bccd: 90                           	nop
  42bcce: 90                           	nop
  42bccf: 90                           	nop
  42bcd0: 8b 44 24 04                  	mov	eax, dword ptr [esp + 0x4]
  42bcd4: 56                           	push	esi
  42bcd5: 8b 70 3c                     	mov	esi, dword ptr [eax + 0x3c]
  42bcd8: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42bcdd: f7 ee                        	imul	esi
  42bcdf: c1 fa 03                     	sar	edx, 0x3
  42bce2: 8b c2                        	mov	eax, edx
  42bce4: c1 e8 1f                     	shr	eax, 0x1f
  42bce7: 03 d0                        	add	edx, eax
  42bce9: b8 67 66 66 66               	mov	eax, 0x66666667
  42bcee: 52                           	push	edx
  42bcef: f7 ee                        	imul	esi
  42bcf1: c1 fa 02                     	sar	edx, 0x2
  42bcf4: 8b c2                        	mov	eax, edx
  42bcf6: c1 e8 1f                     	shr	eax, 0x1f
  42bcf9: 03 d0                        	add	edx, eax
  42bcfb: 52                           	push	edx
  42bcfc: e8 af 0f 00 00               	call	0x42ccb0 <.text+0x2bcb0>
  42bd01: 8b 4c 24 0c                  	mov	ecx, dword ptr [esp + 0xc]
  42bd05: 8b 74 24 10                  	mov	esi, dword ptr [esp + 0x10]
  42bd09: 2b ce                        	sub	ecx, esi
  42bd0b: 5e                           	pop	esi
  42bd0c: 03 c1                        	add	eax, ecx
  42bd0e: 85 c0                        	test	eax, eax
  42bd10: 7f 05                        	jg	0x42bd17 <.text+0x2ad17>
  42bd12: b8 01 00 00 00               	mov	eax, 0x1
  42bd17: c2 10 00                     	ret	0x10
  42bd1a: 90                           	nop
  42bd1b: 90                           	nop
  42bd1c: 90                           	nop
  42bd1d: 90                           	nop
  42bd1e: 90                           	nop
  42bd1f: 90                           	nop
  42bd20: 8b 54 24 04                  	mov	edx, dword ptr [esp + 0x4]
  42bd24: 33 c0                        	xor	eax, eax
  42bd26: 56                           	push	esi
  42bd27: 8b 35 c8 01 8b 00            	mov	esi, dword ptr [0x8b01c8]
  42bd2d: 8b 8a 90 00 00 00            	mov	ecx, dword ptr [edx + 0x90]
  42bd33: 85 c9                        	test	ecx, ecx
  42bd35: 7e 15                        	jle	0x42bd4c <.text+0x2ad4c>
  42bd37: 8d 04 cd 00 00 00 00         	lea	eax, [8*ecx]
  42bd3e: 2b c1                        	sub	eax, ecx
