
/mnt/data/RPG.exe:	file format coff-i386

Disassembly of section .text:

00401000 <.text>:
  42a820: 89 47 38                     	mov	dword ptr [edi + 0x38], eax
  42a823: 89 47 3c                     	mov	dword ptr [edi + 0x3c], eax
  42a826: c7 47 44 01 00 00 00         	mov	dword ptr [edi + 0x44], 0x1
  42a82d: c7 47 4c ff ff ff ff         	mov	dword ptr [edi + 0x4c], 0xffffffff
  42a834: 75 1f                        	jne	0x42a855 <.text+0x29855>
  42a836: 8b 0d ec 01 8b 00            	mov	ecx, dword ptr [0x8b01ec]
  42a83c: 68 30 a8 46 00               	push	0x46a830
  42a841: 68 0c a8 46 00               	push	0x46a80c
  42a846: e8 05 78 00 00               	call	0x432050 <.text+0x31050>
  42a84b: 5f                           	pop	edi
  42a84c: 5e                           	pop	esi
  42a84d: 5d                           	pop	ebp
  42a84e: 5b                           	pop	ebx
  42a84f: 83 c4 10                     	add	esp, 0x10
  42a852: c2 14 00                     	ret	0x14
  42a855: 8b 74 24 28                  	mov	esi, dword ptr [esp + 0x28]
  42a859: 3b f0                        	cmp	esi, eax
  42a85b: 75 1f                        	jne	0x42a87c <.text+0x2987c>
  42a85d: 8b 0d ec 01 8b 00            	mov	ecx, dword ptr [0x8b01ec]
  42a863: 68 30 a8 46 00               	push	0x46a830
  42a868: 68 e8 a7 46 00               	push	0x46a7e8
  42a86d: e8 de 77 00 00               	call	0x432050 <.text+0x31050>
  42a872: 5f                           	pop	edi
  42a873: 5e                           	pop	esi
  42a874: 5d                           	pop	ebp
  42a875: 5b                           	pop	ebx
  42a876: 83 c4 10                     	add	esp, 0x10
  42a879: c2 14 00                     	ret	0x14
  42a87c: 3b f8                        	cmp	edi, eax
  42a87e: 75 1f                        	jne	0x42a89f <.text+0x2989f>
  42a880: 8b 0d ec 01 8b 00            	mov	ecx, dword ptr [0x8b01ec]
  42a886: 68 30 a8 46 00               	push	0x46a830
  42a88b: 68 c4 a7 46 00               	push	0x46a7c4
  42a890: e8 bb 77 00 00               	call	0x432050 <.text+0x31050>
  42a895: 5f                           	pop	edi
  42a896: 5e                           	pop	esi
  42a897: 5d                           	pop	ebp
  42a898: 5b                           	pop	ebx
  42a899: 83 c4 10                     	add	esp, 0x10
  42a89c: c2 14 00                     	ret	0x14
  42a89f: 8b 5c 24 30                  	mov	ebx, dword ptr [esp + 0x30]
  42a8a3: 3b d8                        	cmp	ebx, eax
  42a8a5: 75 1f                        	jne	0x42a8c6 <.text+0x298c6>
  42a8a7: 8b 0d ec 01 8b 00            	mov	ecx, dword ptr [0x8b01ec]
  42a8ad: 68 30 a8 46 00               	push	0x46a830
  42a8b2: 68 a0 a7 46 00               	push	0x46a7a0
  42a8b7: e8 94 77 00 00               	call	0x432050 <.text+0x31050>
  42a8bc: 5f                           	pop	edi
  42a8bd: 5e                           	pop	esi
  42a8be: 5d                           	pop	ebp
  42a8bf: 5b                           	pop	ebx
  42a8c0: 83 c4 10                     	add	esp, 0x10
  42a8c3: c2 14 00                     	ret	0x14
  42a8c6: 56                           	push	esi
  42a8c7: 8b cd                        	mov	ecx, ebp
  42a8c9: 89 44 24 2c                  	mov	dword ptr [esp + 0x2c], eax
  42a8cd: e8 fe 11 00 00               	call	0x42bad0 <.text+0x2aad0>
  42a8d2: 8b 8b c4 00 00 00            	mov	ecx, dword ptr [ebx + 0xc4]
  42a8d8: 2b c8                        	sub	ecx, eax
  42a8da: 8a 44 24 34                  	mov	al, byte ptr [esp + 0x34]
  42a8de: 84 c0                        	test	al, al
  42a8e0: 89 4c 24 2c                  	mov	dword ptr [esp + 0x2c], ecx
  42a8e4: 74 0d                        	je	0x42a8f3 <.text+0x298f3>
  42a8e6: 8b c1                        	mov	eax, ecx
  42a8e8: 99                           	cdq
  42a8e9: 2b c2                        	sub	eax, edx
  42a8eb: d1 f8                        	sar	eax
  42a8ed: 8b c8                        	mov	ecx, eax
  42a8ef: 89 4c 24 2c                  	mov	dword ptr [esp + 0x2c], ecx
  42a8f3: 85 c9                        	test	ecx, ecx
  42a8f5: 7d 08                        	jge	0x42a8ff <.text+0x298ff>
  42a8f7: c7 44 24 2c 00 00 00 00      	mov	dword ptr [esp + 0x2c], 0x0
  42a8ff: c6 07 01                     	mov	byte ptr [edi], 0x1
  42a902: 8b 4e 04                     	mov	ecx, dword ptr [esi + 0x4]
  42a905: 8b 83 cc 00 00 00            	mov	eax, dword ptr [ebx + 0xcc]
  42a90b: 6a 03                        	push	0x3
  42a90d: 51                           	push	ecx
  42a90e: 8b cd                        	mov	ecx, ebp
  42a910: c6 44 24 3c 00               	mov	byte ptr [esp + 0x3c], 0x0
  42a915: 89 44 24 38                  	mov	dword ptr [esp + 0x38], eax
  42a919: e8 22 16 00 00               	call	0x42bf40 <.text+0x2af40>
  42a91e: 84 c0                        	test	al, al
  42a920: bd 64 00 00 00               	mov	ebp, 0x64
  42a925: 75 2b                        	jne	0x42a952 <.text+0x29952>
  42a927: 8b 86 20 02 00 00            	mov	eax, dword ptr [esi + 0x220]
  42a92d: 85 c0                        	test	eax, eax
  42a92f: 7e 07                        	jle	0x42a938 <.text+0x29938>
  42a931: c6 44 24 34 00               	mov	byte ptr [esp + 0x34], 0x0
  42a936: eb 1f                        	jmp	0x42a957 <.text+0x29957>
  42a938: 8b 86 24 02 00 00            	mov	eax, dword ptr [esi + 0x224]
  42a93e: 85 c0                        	test	eax, eax
  42a940: 7f 10                        	jg	0x42a952 <.text+0x29952>
  42a942: e8 1e 6c 02 00               	call	0x451565 <.text+0x50565>
  42a947: 99                           	cdq
  42a948: 8b cd                        	mov	ecx, ebp
  42a94a: f7 f9                        	idiv	ecx
  42a94c: 3b 54 24 30                  	cmp	edx, dword ptr [esp + 0x30]
  42a950: 7d 05                        	jge	0x42a957 <.text+0x29957>
  42a952: c6 44 24 34 01               	mov	byte ptr [esp + 0x34], 0x1
  42a957: 8b 83 c8 00 00 00            	mov	eax, dword ptr [ebx + 0xc8]
  42a95d: 89 6c 24 30                  	mov	dword ptr [esp + 0x30], ebp
  42a961: 89 6c 24 10                  	mov	dword ptr [esp + 0x10], ebp
  42a965: 89 6c 24 14                  	mov	dword ptr [esp + 0x14], ebp
  42a969: 8b ab d4 00 00 00            	mov	ebp, dword ptr [ebx + 0xd4]
  42a96f: 85 c0                        	test	eax, eax
  42a971: 89 6c 24 1c                  	mov	dword ptr [esp + 0x1c], ebp
  42a975: 0f 85 41 02 00 00            	jne	0x42abbc <.text+0x29bbc>
  42a97b: 8a 4c 24 34                  	mov	cl, byte ptr [esp + 0x34]
  42a97f: 84 c9                        	test	cl, cl
  42a981: 0f 84 35 02 00 00            	je	0x42abbc <.text+0x29bbc>
  42a987: 8b 4c 24 18                  	mov	ecx, dword ptr [esp + 0x18]
  42a98b: 56                           	push	esi
  42a98c: e8 5f 23 00 00               	call	0x42ccf0 <.text+0x2bcf0>
  42a991: 8b 8b d0 00 00 00            	mov	ecx, dword ptr [ebx + 0xd0]
  42a997: 2b c8                        	sub	ecx, eax
  42a999: 89 4c 24 34                  	mov	dword ptr [esp + 0x34], ecx
  42a99d: 79 06                        	jns	0x42a9a5 <.text+0x299a5>
  42a99f: 33 c9                        	xor	ecx, ecx
  42a9a1: 89 4c 24 34                  	mov	dword ptr [esp + 0x34], ecx
  42a9a5: 83 fd 01                     	cmp	ebp, 0x1
  42a9a8: 75 20                        	jne	0x42a9ca <.text+0x299ca>
  42a9aa: 8b 86 f0 01 00 00            	mov	eax, dword ptr [esi + 0x1f0]
  42a9b0: 85 c0                        	test	eax, eax
  42a9b2: 7e 3d                        	jle	0x42a9f1 <.text+0x299f1>
  42a9b4: c1 e1 03                     	shl	ecx, 0x3
  42a9b7: b8 67 66 66 66               	mov	eax, 0x66666667
  42a9bc: f7 e9                        	imul	ecx
  42a9be: c1 fa 02                     	sar	edx, 0x2
  42a9c1: 8b c2                        	mov	eax, edx
  42a9c3: c1 e8 1f                     	shr	eax, 0x1f
  42a9c6: 03 d0                        	add	edx, eax
  42a9c8: eb 23                        	jmp	0x42a9ed <.text+0x299ed>
  42a9ca: 83 fd 02                     	cmp	ebp, 0x2
  42a9cd: 75 22                        	jne	0x42a9f1 <.text+0x299f1>
  42a9cf: 8b 86 ec 01 00 00            	mov	eax, dword ptr [esi + 0x1ec]
  42a9d5: 85 c0                        	test	eax, eax
  42a9d7: 7e 18                        	jle	0x42a9f1 <.text+0x299f1>
  42a9d9: c1 e1 03                     	shl	ecx, 0x3
  42a9dc: b8 67 66 66 66               	mov	eax, 0x66666667
  42a9e1: f7 e9                        	imul	ecx
  42a9e3: c1 fa 02                     	sar	edx, 0x2
  42a9e6: 8b ca                        	mov	ecx, edx
  42a9e8: c1 e9 1f                     	shr	ecx, 0x1f
  42a9eb: 03 d1                        	add	edx, ecx
  42a9ed: 89 54 24 34                  	mov	dword ptr [esp + 0x34], edx
  42a9f1: 8b 56 04                     	mov	edx, dword ptr [esi + 0x4]
  42a9f4: 8b 4c 24 18                  	mov	ecx, dword ptr [esp + 0x18]
  42a9f8: 6a 04                        	push	0x4
  42a9fa: 52                           	push	edx
  42a9fb: e8 40 15 00 00               	call	0x42bf40 <.text+0x2af40>
  42aa00: 84 c0                        	test	al, al
  42aa02: 74 0d                        	je	0x42aa11 <.text+0x29a11>
  42aa04: 83 fd 04                     	cmp	ebp, 0x4
  42aa07: 75 08                        	jne	0x42aa11 <.text+0x29a11>
  42aa09: c7 44 24 34 00 00 00 00      	mov	dword ptr [esp + 0x34], 0x0
  42aa11: 8b 44 ae 64                  	mov	eax, dword ptr [esi + 4*ebp + 0x64]
  42aa15: 8b 0d b0 01 8a 00            	mov	ecx, dword ptr [0x8a01b0]
  42aa1b: 99                           	cdq
  42aa1c: 33 c2                        	xor	eax, edx
  42aa1e: 2b c2                        	sub	eax, edx
  42aa20: 89 44 24 28                  	mov	dword ptr [esp + 0x28], eax
  42aa24: 8b 86 90 00 00 00            	mov	eax, dword ptr [esi + 0x90]
  42aa2a: 85 c0                        	test	eax, eax
  42aa2c: 7e 1c                        	jle	0x42aa4a <.text+0x29a4a>
  42aa2e: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42aa35: 2b d0                        	sub	edx, eax
  42aa37: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42aa3a: c1 e2 04                     	shl	edx, 0x4
  42aa3d: 2b d0                        	sub	edx, eax
  42aa3f: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42aa42: 8b 54 a8 74                  	mov	edx, dword ptr [eax + 4*ebp + 0x74]
  42aa46: 89 54 24 30                  	mov	dword ptr [esp + 0x30], edx
  42aa4a: 8b 86 94 00 00 00            	mov	eax, dword ptr [esi + 0x94]
  42aa50: 85 c0                        	test	eax, eax
  42aa52: 7e 1c                        	jle	0x42aa70 <.text+0x29a70>
  42aa54: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42aa5b: 2b d0                        	sub	edx, eax
  42aa5d: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42aa60: c1 e2 04                     	shl	edx, 0x4
  42aa63: 2b d0                        	sub	edx, eax
  42aa65: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42aa68: 8b 54 a8 74                  	mov	edx, dword ptr [eax + 4*ebp + 0x74]
  42aa6c: 89 54 24 10                  	mov	dword ptr [esp + 0x10], edx
  42aa70: 8b 86 98 00 00 00            	mov	eax, dword ptr [esi + 0x98]
  42aa76: 85 c0                        	test	eax, eax
  42aa78: 7e 1c                        	jle	0x42aa96 <.text+0x29a96>
  42aa7a: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42aa81: 2b d0                        	sub	edx, eax
  42aa83: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42aa86: c1 e2 04                     	shl	edx, 0x4
  42aa89: 2b d0                        	sub	edx, eax
  42aa8b: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42aa8e: 8b 4c a8 74                  	mov	ecx, dword ptr [eax + 4*ebp + 0x74]
  42aa92: 89 4c 24 14                  	mov	dword ptr [esp + 0x14], ecx
  42aa96: 8b 44 24 14                  	mov	eax, dword ptr [esp + 0x14]
  42aa9a: 99                           	cdq
  42aa9b: 8b c8                        	mov	ecx, eax
  42aa9d: 8b 44 24 10                  	mov	eax, dword ptr [esp + 0x10]
  42aaa1: 33 ca                        	xor	ecx, edx
  42aaa3: 2b ca                        	sub	ecx, edx
  42aaa5: 99                           	cdq
  42aaa6: 33 c2                        	xor	eax, edx
  42aaa8: 2b c2                        	sub	eax, edx
  42aaaa: 0f af c8                     	imul	ecx, eax
  42aaad: 8b 44 24 30                  	mov	eax, dword ptr [esp + 0x30]
  42aab1: 99                           	cdq
  42aab2: 33 c2                        	xor	eax, edx
  42aab4: 2b c2                        	sub	eax, edx
  42aab6: 0f af c8                     	imul	ecx, eax
  42aab9: 0f af 4c 24 28               	imul	ecx, dword ptr [esp + 0x28]
  42aabe: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42aac3: f7 e9                        	imul	ecx
  42aac5: c1 fa 05                     	sar	edx, 0x5
  42aac8: 8b c2                        	mov	eax, edx
  42aaca: c1 e8 1f                     	shr	eax, 0x1f
  42aacd: 03 d0                        	add	edx, eax
  42aacf: 89 54 24 28                  	mov	dword ptr [esp + 0x28], edx
  42aad3: e8 8d 6a 02 00               	call	0x451565 <.text+0x50565>
  42aad8: 99                           	cdq
  42aad9: b9 09 00 00 00               	mov	ecx, 0x9
  42aade: 8b 6c 24 2c                  	mov	ebp, dword ptr [esp + 0x2c]
  42aae2: f7 f9                        	idiv	ecx
  42aae4: 8b c5                        	mov	eax, ebp
  42aae6: 8b ca                        	mov	ecx, edx
  42aae8: 99                           	cdq
  42aae9: 41                           	inc	ecx
  42aaea: f7 f9                        	idiv	ecx
  42aaec: 03 e8                        	add	ebp, eax
  42aaee: e8 72 6a 02 00               	call	0x451565 <.text+0x50565>
  42aaf3: 99                           	cdq
  42aaf4: b9 13 00 00 00               	mov	ecx, 0x13
  42aaf9: f7 f9                        	idiv	ecx
  42aafb: 8b 46 40                     	mov	eax, dword ptr [esi + 0x40]
  42aafe: 8b ca                        	mov	ecx, edx
  42ab00: 99                           	cdq
  42ab01: 41                           	inc	ecx
  42ab02: f7 f9                        	idiv	ecx
  42ab04: 2b e8                        	sub	ebp, eax
  42ab06: 89 6c 24 2c                  	mov	dword ptr [esp + 0x2c], ebp
  42ab0a: 79 08                        	jns	0x42ab14 <.text+0x29b14>
  42ab0c: c7 44 24 2c 00 00 00 00      	mov	dword ptr [esp + 0x2c], 0x0
  42ab14: e8 4c 6a 02 00               	call	0x451565 <.text+0x50565>
  42ab19: 99                           	cdq
  42ab1a: b9 09 00 00 00               	mov	ecx, 0x9
  42ab1f: f7 f9                        	idiv	ecx
  42ab21: 8b 44 24 34                  	mov	eax, dword ptr [esp + 0x34]
  42ab25: 8b ca                        	mov	ecx, edx
  42ab27: 99                           	cdq
  42ab28: 41                           	inc	ecx
  42ab29: f7 f9                        	idiv	ecx
  42ab2b: 8b e8                        	mov	ebp, eax
  42ab2d: 8b 44 24 34                  	mov	eax, dword ptr [esp + 0x34]
  42ab31: 03 e8                        	add	ebp, eax
  42ab33: e8 2d 6a 02 00               	call	0x451565 <.text+0x50565>
  42ab38: 99                           	cdq
  42ab39: b9 13 00 00 00               	mov	ecx, 0x13
  42ab3e: f7 f9                        	idiv	ecx
  42ab40: 8b 46 4c                     	mov	eax, dword ptr [esi + 0x4c]
  42ab43: 8b ca                        	mov	ecx, edx
  42ab45: 99                           	cdq
  42ab46: 41                           	inc	ecx
  42ab47: f7 f9                        	idiv	ecx
  42ab49: 2b e8                        	sub	ebp, eax
  42ab4b: 89 6c 24 34                  	mov	dword ptr [esp + 0x34], ebp
  42ab4f: db 44 24 34                  	fild	dword ptr [esp + 0x34]
  42ab53: da 4c 24 28                  	fimul	dword ptr [esp + 0x28]
  42ab57: e8 3c 72 02 00               	call	0x451d98 <.text+0x50d98>
  42ab5c: 8b c8                        	mov	ecx, eax
  42ab5e: b8 83 de 1b 43               	mov	eax, 0x431bde83
  42ab63: f7 e9                        	imul	ecx
  42ab65: c1 fa 12                     	sar	edx, 0x12
  42ab68: 8b c2                        	mov	eax, edx
  42ab6a: c1 e8 1f                     	shr	eax, 0x1f
  42ab6d: 03 d0                        	add	edx, eax
  42ab6f: 89 54 24 28                  	mov	dword ptr [esp + 0x28], edx
  42ab73: 79 08                        	jns	0x42ab7d <.text+0x29b7d>
  42ab75: c7 44 24 28 00 00 00 00      	mov	dword ptr [esp + 0x28], 0x0
  42ab7d: 8b 44 24 1c                  	mov	eax, dword ptr [esp + 0x1c]
  42ab81: 85 c0                        	test	eax, eax
  42ab83: 0f 8e e9 03 00 00            	jle	0x42af72 <.text+0x29f72>
  42ab89: 8b 4c 86 64                  	mov	ecx, dword ptr [esi + 4*eax + 0x64]
  42ab8d: 85 c9                        	test	ecx, ecx
  42ab8f: 7c 1c                        	jl	0x42abad <.text+0x29bad>
  42ab91: 8b 44 24 30                  	mov	eax, dword ptr [esp + 0x30]
  42ab95: 85 c0                        	test	eax, eax
  42ab97: 7c 14                        	jl	0x42abad <.text+0x29bad>
  42ab99: 8b 44 24 10                  	mov	eax, dword ptr [esp + 0x10]
  42ab9d: 85 c0                        	test	eax, eax
  42ab9f: 7c 0c                        	jl	0x42abad <.text+0x29bad>
  42aba1: 8b 44 24 14                  	mov	eax, dword ptr [esp + 0x14]
  42aba5: 85 c0                        	test	eax, eax
  42aba7: 0f 8d c5 03 00 00            	jge	0x42af72 <.text+0x29f72>
  42abad: 8b 4c 24 28                  	mov	ecx, dword ptr [esp + 0x28]
  42abb1: f7 d9                        	neg	ecx
  42abb3: 89 4c 24 28                  	mov	dword ptr [esp + 0x28], ecx
  42abb7: e9 b6 03 00 00               	jmp	0x42af72 <.text+0x29f72>
  42abbc: 83 f8 01                     	cmp	eax, 0x1
  42abbf: 0f 85 f0 01 00 00            	jne	0x42adb5 <.text+0x29db5>
  42abc5: 8a 4c 24 34                  	mov	cl, byte ptr [esp + 0x34]
  42abc9: 84 c9                        	test	cl, cl
  42abcb: 0f 84 e4 01 00 00            	je	0x42adb5 <.text+0x29db5>
  42abd1: 8b 8b d0 00 00 00            	mov	ecx, dword ptr [ebx + 0xd0]
  42abd7: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42abdc: 0f af 4e 2c                  	imul	ecx, dword ptr [esi + 0x2c]
  42abe0: f7 e9                        	imul	ecx
  42abe2: 8b 4c 24 18                  	mov	ecx, dword ptr [esp + 0x18]
  42abe6: 56                           	push	esi
  42abe7: c1 fa 05                     	sar	edx, 0x5
  42abea: 8b c2                        	mov	eax, edx
  42abec: c1 e8 1f                     	shr	eax, 0x1f
  42abef: 03 d0                        	add	edx, eax
  42abf1: 89 54 24 38                  	mov	dword ptr [esp + 0x38], edx
  42abf5: e8 f6 20 00 00               	call	0x42ccf0 <.text+0x2bcf0>
  42abfa: 8b 56 40                     	mov	edx, dword ptr [esi + 0x40]
  42abfd: 8b 0d b0 01 8a 00            	mov	ecx, dword ptr [0x8a01b0]
  42ac03: 03 c2                        	add	eax, edx
  42ac05: 99                           	cdq
  42ac06: 2b c2                        	sub	eax, edx
  42ac08: 8b 54 24 34                  	mov	edx, dword ptr [esp + 0x34]
  42ac0c: d1 f8                        	sar	eax
  42ac0e: f7 d8                        	neg	eax
  42ac10: 03 d0                        	add	edx, eax
  42ac12: 8b 44 ae 64                  	mov	eax, dword ptr [esi + 4*ebp + 0x64]
  42ac16: 89 54 24 34                  	mov	dword ptr [esp + 0x34], edx
  42ac1a: 99                           	cdq
  42ac1b: 33 c2                        	xor	eax, edx
  42ac1d: 2b c2                        	sub	eax, edx
  42ac1f: 89 44 24 28                  	mov	dword ptr [esp + 0x28], eax
  42ac23: 8b 86 90 00 00 00            	mov	eax, dword ptr [esi + 0x90]
  42ac29: 85 c0                        	test	eax, eax
  42ac2b: 7e 1c                        	jle	0x42ac49 <.text+0x29c49>
  42ac2d: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42ac34: 2b d0                        	sub	edx, eax
  42ac36: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42ac39: c1 e2 04                     	shl	edx, 0x4
  42ac3c: 2b d0                        	sub	edx, eax
  42ac3e: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42ac41: 8b 54 a8 74                  	mov	edx, dword ptr [eax + 4*ebp + 0x74]
  42ac45: 89 54 24 30                  	mov	dword ptr [esp + 0x30], edx
  42ac49: 8b 86 94 00 00 00            	mov	eax, dword ptr [esi + 0x94]
  42ac4f: 85 c0                        	test	eax, eax
  42ac51: 7e 1c                        	jle	0x42ac6f <.text+0x29c6f>
  42ac53: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42ac5a: 2b d0                        	sub	edx, eax
  42ac5c: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42ac5f: c1 e2 04                     	shl	edx, 0x4
  42ac62: 2b d0                        	sub	edx, eax
  42ac64: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42ac67: 8b 54 a8 74                  	mov	edx, dword ptr [eax + 4*ebp + 0x74]
  42ac6b: 89 54 24 10                  	mov	dword ptr [esp + 0x10], edx
  42ac6f: 8b 86 98 00 00 00            	mov	eax, dword ptr [esi + 0x98]
  42ac75: 85 c0                        	test	eax, eax
  42ac77: 7e 1c                        	jle	0x42ac95 <.text+0x29c95>
  42ac79: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42ac80: 2b d0                        	sub	edx, eax
  42ac82: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42ac85: c1 e2 04                     	shl	edx, 0x4
  42ac88: 2b d0                        	sub	edx, eax
  42ac8a: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42ac8d: 8b 4c a8 74                  	mov	ecx, dword ptr [eax + 4*ebp + 0x74]
  42ac91: 89 4c 24 14                  	mov	dword ptr [esp + 0x14], ecx
  42ac95: e8 cb 68 02 00               	call	0x451565 <.text+0x50565>
  42ac9a: 99                           	cdq
  42ac9b: b9 09 00 00 00               	mov	ecx, 0x9
  42aca0: 8b 6c 24 2c                  	mov	ebp, dword ptr [esp + 0x2c]
  42aca4: f7 f9                        	idiv	ecx
  42aca6: 8b c5                        	mov	eax, ebp
  42aca8: 8b ca                        	mov	ecx, edx
  42acaa: 99                           	cdq
  42acab: 41                           	inc	ecx
  42acac: f7 f9                        	idiv	ecx
  42acae: 03 e8                        	add	ebp, eax
  42acb0: e8 b0 68 02 00               	call	0x451565 <.text+0x50565>
  42acb5: 99                           	cdq
  42acb6: b9 13 00 00 00               	mov	ecx, 0x13
  42acbb: f7 f9                        	idiv	ecx
  42acbd: 8b 46 40                     	mov	eax, dword ptr [esi + 0x40]
  42acc0: 8b ca                        	mov	ecx, edx
  42acc2: 99                           	cdq
  42acc3: 41                           	inc	ecx
  42acc4: f7 f9                        	idiv	ecx
  42acc6: 2b e8                        	sub	ebp, eax
  42acc8: 89 6c 24 2c                  	mov	dword ptr [esp + 0x2c], ebp
  42accc: e8 94 68 02 00               	call	0x451565 <.text+0x50565>
  42acd1: 99                           	cdq
  42acd2: b9 63 00 00 00               	mov	ecx, 0x63
  42acd7: f7 f9                        	idiv	ecx
  42acd9: 8b 44 24 34                  	mov	eax, dword ptr [esp + 0x34]
  42acdd: 8b ca                        	mov	ecx, edx
  42acdf: 99                           	cdq
  42ace0: 41                           	inc	ecx
  42ace1: f7 f9                        	idiv	ecx
  42ace3: 8b e8                        	mov	ebp, eax
  42ace5: 8b 44 24 34                  	mov	eax, dword ptr [esp + 0x34]
  42ace9: 03 e8                        	add	ebp, eax
  42aceb: e8 75 68 02 00               	call	0x451565 <.text+0x50565>
  42acf0: 99                           	cdq
  42acf1: b9 13 00 00 00               	mov	ecx, 0x13
  42acf6: f7 f9                        	idiv	ecx
  42acf8: 8b 46 4c                     	mov	eax, dword ptr [esi + 0x4c]
  42acfb: 8b ca                        	mov	ecx, edx
  42acfd: 99                           	cdq
  42acfe: 41                           	inc	ecx
  42acff: f7 f9                        	idiv	ecx
  42ad01: 2b e8                        	sub	ebp, eax
  42ad03: 8b 44 24 14                  	mov	eax, dword ptr [esp + 0x14]
  42ad07: 99                           	cdq
  42ad08: 8b c8                        	mov	ecx, eax
  42ad0a: 8b 44 24 10                  	mov	eax, dword ptr [esp + 0x10]
  42ad0e: 33 ca                        	xor	ecx, edx
  42ad10: 89 6c 24 34                  	mov	dword ptr [esp + 0x34], ebp
  42ad14: 2b ca                        	sub	ecx, edx
  42ad16: 99                           	cdq
  42ad17: db 44 24 34                  	fild	dword ptr [esp + 0x34]
  42ad1b: 33 c2                        	xor	eax, edx
  42ad1d: 2b c2                        	sub	eax, edx
  42ad1f: 0f af c8                     	imul	ecx, eax
  42ad22: 8b 44 24 30                  	mov	eax, dword ptr [esp + 0x30]
  42ad26: 99                           	cdq
  42ad27: 33 c2                        	xor	eax, edx
  42ad29: 2b c2                        	sub	eax, edx
  42ad2b: 0f af c8                     	imul	ecx, eax
  42ad2e: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42ad33: f7 e9                        	imul	ecx
  42ad35: c1 fa 05                     	sar	edx, 0x5
  42ad38: 8b c2                        	mov	eax, edx
  42ad3a: c1 e8 1f                     	shr	eax, 0x1f
  42ad3d: 03 d0                        	add	edx, eax
  42ad3f: 0f af 54 24 28               	imul	edx, dword ptr [esp + 0x28]
