
/mnt/data/RPG.exe:	file format coff-i386

Disassembly of section .text:

00401000 <.text>:
  42c080: 01 00                        	add	dword ptr [eax], eax
  42c082: 00 85 c0 7e 3d c1            	add	byte ptr [ebp - 0x3ec28140], al
  42c088: e6 03                        	out	0x3, al
  42c08a: b8 67 66 66 66               	mov	eax, 0x66666667
  42c08f: f7 ee                        	imul	esi
  42c091: c1 fa 02                     	sar	edx, 0x2
  42c094: 8b c2                        	mov	eax, edx
  42c096: c1 e8 1f                     	shr	eax, 0x1f
  42c099: 03 d0                        	add	edx, eax
  42c09b: eb 23                        	jmp	0x42c0c0 <.text+0x2b0c0>
  42c09d: 83 ff 02                     	cmp	edi, 0x2
  42c0a0: 75 22                        	jne	0x42c0c4 <.text+0x2b0c4>
  42c0a2: 8b 83 ec 01 00 00            	mov	eax, dword ptr [ebx + 0x1ec]
  42c0a8: 85 c0                        	test	eax, eax
  42c0aa: 7e 18                        	jle	0x42c0c4 <.text+0x2b0c4>
  42c0ac: c1 e6 03                     	shl	esi, 0x3
  42c0af: b8 67 66 66 66               	mov	eax, 0x66666667
  42c0b4: f7 ee                        	imul	esi
  42c0b6: c1 fa 02                     	sar	edx, 0x2
  42c0b9: 8b ca                        	mov	ecx, edx
  42c0bb: c1 e9 1f                     	shr	ecx, 0x1f
  42c0be: 03 d1                        	add	edx, ecx
  42c0c0: 89 54 24 30                  	mov	dword ptr [esp + 0x30], edx
  42c0c4: 8b 53 04                     	mov	edx, dword ptr [ebx + 0x4]
  42c0c7: 8b 6c 24 1c                  	mov	ebp, dword ptr [esp + 0x1c]
  42c0cb: 6a 04                        	push	0x4
  42c0cd: 52                           	push	edx
  42c0ce: 8b cd                        	mov	ecx, ebp
  42c0d0: e8 6b fe ff ff               	call	0x42bf40 <.text+0x2af40>
  42c0d5: 84 c0                        	test	al, al
  42c0d7: 74 0d                        	je	0x42c0e6 <.text+0x2b0e6>
  42c0d9: 83 ff 04                     	cmp	edi, 0x4
  42c0dc: 75 08                        	jne	0x42c0e6 <.text+0x2b0e6>
  42c0de: c7 44 24 30 00 00 00 00      	mov	dword ptr [esp + 0x30], 0x0
  42c0e6: 8b 44 bb 64                  	mov	eax, dword ptr [ebx + 4*edi + 0x64]
  42c0ea: 8b 0d b0 01 8a 00            	mov	ecx, dword ptr [0x8a01b0]
  42c0f0: 99                           	cdq
  42c0f1: 8b f0                        	mov	esi, eax
  42c0f3: 8b 83 90 00 00 00            	mov	eax, dword ptr [ebx + 0x90]
  42c0f9: 33 f2                        	xor	esi, edx
  42c0fb: 2b f2                        	sub	esi, edx
  42c0fd: 85 c0                        	test	eax, eax
  42c0ff: 7e 1c                        	jle	0x42c11d <.text+0x2b11d>
  42c101: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42c108: 2b d0                        	sub	edx, eax
  42c10a: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42c10d: c1 e2 04                     	shl	edx, 0x4
  42c110: 2b d0                        	sub	edx, eax
  42c112: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42c115: 8b 54 b8 74                  	mov	edx, dword ptr [eax + 4*edi + 0x74]
  42c119: 89 54 24 14                  	mov	dword ptr [esp + 0x14], edx
  42c11d: 8b 83 94 00 00 00            	mov	eax, dword ptr [ebx + 0x94]
  42c123: 85 c0                        	test	eax, eax
  42c125: 7e 1c                        	jle	0x42c143 <.text+0x2b143>
  42c127: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42c12e: 2b d0                        	sub	edx, eax
  42c130: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42c133: c1 e2 04                     	shl	edx, 0x4
  42c136: 2b d0                        	sub	edx, eax
  42c138: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42c13b: 8b 54 b8 74                  	mov	edx, dword ptr [eax + 4*edi + 0x74]
  42c13f: 89 54 24 10                  	mov	dword ptr [esp + 0x10], edx
  42c143: 8b 83 98 00 00 00            	mov	eax, dword ptr [ebx + 0x98]
  42c149: 85 c0                        	test	eax, eax
  42c14b: 7e 1a                        	jle	0x42c167 <.text+0x2b167>
  42c14d: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42c154: 2b d0                        	sub	edx, eax
  42c156: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42c159: c1 e2 04                     	shl	edx, 0x4
  42c15c: 2b d0                        	sub	edx, eax
  42c15e: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42c161: 8b 7c b8 74                  	mov	edi, dword ptr [eax + 4*edi + 0x74]
  42c165: eb 04                        	jmp	0x42c16b <.text+0x2b16b>
  42c167: 8b 7c 24 20                  	mov	edi, dword ptr [esp + 0x20]
  42c16b: 53                           	push	ebx
  42c16c: 8b cd                        	mov	ecx, ebp
  42c16e: e8 7d 0b 00 00               	call	0x42ccf0 <.text+0x2bcf0>
  42c173: 89 44 24 2c                  	mov	dword ptr [esp + 0x2c], eax
  42c177: db 44 24 2c                  	fild	dword ptr [esp + 0x2c]
  42c17b: dd 5c 24 20                  	fstp	qword ptr [esp + 0x20]
  42c17f: e8 e1 53 02 00               	call	0x451565 <.text+0x50565>
  42c184: 99                           	cdq
  42c185: b9 13 00 00 00               	mov	ecx, 0x13
  42c18a: f7 f9                        	idiv	ecx
  42c18c: 8b 43 4c                     	mov	eax, dword ptr [ebx + 0x4c]
  42c18f: 8b ca                        	mov	ecx, edx
  42c191: 99                           	cdq
  42c192: 41                           	inc	ecx
  42c193: f7 f9                        	idiv	ecx
  42c195: 8b e8                        	mov	ebp, eax
  42c197: e8 c9 53 02 00               	call	0x451565 <.text+0x50565>
  42c19c: 99                           	cdq
  42c19d: b9 09 00 00 00               	mov	ecx, 0x9
  42c1a2: f7 f9                        	idiv	ecx
  42c1a4: 8b ca                        	mov	ecx, edx
  42c1a6: 8b 54 24 34                  	mov	edx, dword ptr [esp + 0x34]
  42c1aa: 41                           	inc	ecx
  42c1ab: 8b 42 4c                     	mov	eax, dword ptr [edx + 0x4c]
  42c1ae: 99                           	cdq
  42c1af: f7 f9                        	idiv	ecx
  42c1b1: 03 e8                        	add	ebp, eax
  42c1b3: 8b c7                        	mov	eax, edi
  42c1b5: 99                           	cdq
  42c1b6: 8b c8                        	mov	ecx, eax
  42c1b8: 8b 44 24 10                  	mov	eax, dword ptr [esp + 0x10]
  42c1bc: 33 ca                        	xor	ecx, edx
  42c1be: 89 6c 24 34                  	mov	dword ptr [esp + 0x34], ebp
  42c1c2: 2b ca                        	sub	ecx, edx
  42c1c4: 8b 6c 24 14                  	mov	ebp, dword ptr [esp + 0x14]
  42c1c8: 99                           	cdq
  42c1c9: db 44 24 34                  	fild	dword ptr [esp + 0x34]
  42c1cd: 33 c2                        	xor	eax, edx
  42c1cf: 2b c2                        	sub	eax, edx
  42c1d1: 0f af c8                     	imul	ecx, eax
  42c1d4: db 44 24 30                  	fild	dword ptr [esp + 0x30]
  42c1d8: 8b c5                        	mov	eax, ebp
  42c1da: 99                           	cdq
  42c1db: 33 c2                        	xor	eax, edx
  42c1dd: 2b c2                        	sub	eax, edx
  42c1df: 0f af c8                     	imul	ecx, eax
  42c1e2: 0f af ce                     	imul	ecx, esi
  42c1e5: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42c1ea: f7 e9                        	imul	ecx
  42c1ec: c1 fa 05                     	sar	edx, 0x5
  42c1ef: 8b c2                        	mov	eax, edx
  42c1f1: dc 64 24 20                  	fsub	qword ptr [esp + 0x20]
  42c1f5: c1 e8 1f                     	shr	eax, 0x1f
  42c1f8: 03 d0                        	add	edx, eax
  42c1fa: 89 54 24 34                  	mov	dword ptr [esp + 0x34], edx
  42c1fe: de c1                        	faddp	st(1), st
  42c200: db 44 24 34                  	fild	dword ptr [esp + 0x34]
  42c204: de c9                        	fmulp	st(1), st
  42c206: dc 0d 98 0c 46 00            	fmul	qword ptr [0x460c98]
  42c20c: dc 15 90 0c 46 00            	fcom	qword ptr [0x460c90]
  42c212: df e0                        	fnstsw	ax
  42c214: f6 c4 01                     	test	ah, 0x1
  42c217: 74 08                        	je	0x42c221 <.text+0x2b221>
  42c219: dd d8                        	fstp	st(0)
  42c21b: dd 05 90 0c 46 00            	fld	qword ptr [0x460c90]
  42c221: 8b 44 24 18                  	mov	eax, dword ptr [esp + 0x18]
  42c225: 85 c0                        	test	eax, eax
  42c227: 7e 1e                        	jle	0x42c247 <.text+0x2b247>
  42c229: 8b 4c 83 64                  	mov	ecx, dword ptr [ebx + 4*eax + 0x64]
  42c22d: 85 c9                        	test	ecx, ecx
  42c22f: 7c 10                        	jl	0x42c241 <.text+0x2b241>
  42c231: 85 ed                        	test	ebp, ebp
  42c233: 7c 0c                        	jl	0x42c241 <.text+0x2b241>
  42c235: 8b 44 24 10                  	mov	eax, dword ptr [esp + 0x10]
  42c239: 85 c0                        	test	eax, eax
  42c23b: 7c 04                        	jl	0x42c241 <.text+0x2b241>
  42c23d: 85 ff                        	test	edi, edi
  42c23f: 7d 06                        	jge	0x42c247 <.text+0x2b247>
  42c241: dc 0d 88 0c 46 00            	fmul	qword ptr [0x460c88]
  42c247: e8 4c 5b 02 00               	call	0x451d98 <.text+0x50d98>
  42c24c: 5f                           	pop	edi
  42c24d: 5e                           	pop	esi
  42c24e: 5d                           	pop	ebp
  42c24f: 5b                           	pop	ebx
  42c250: 83 c4 18                     	add	esp, 0x18
  42c253: c2 0c 00                     	ret	0xc
  42c256: 90                           	nop
  42c257: 90                           	nop
  42c258: 90                           	nop
  42c259: 90                           	nop
  42c25a: 90                           	nop
  42c25b: 90                           	nop
  42c25c: 90                           	nop
  42c25d: 90                           	nop
  42c25e: 90                           	nop
  42c25f: 90                           	nop
  42c260: 83 ec 10                     	sub	esp, 0x10
  42c263: 53                           	push	ebx
  42c264: b8 64 00 00 00               	mov	eax, 0x64
  42c269: 55                           	push	ebp
  42c26a: 56                           	push	esi
  42c26b: 8b 74 24 24                  	mov	esi, dword ptr [esp + 0x24]
  42c26f: 89 44 24 14                  	mov	dword ptr [esp + 0x14], eax
  42c273: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  42c277: 89 44 24 0c                  	mov	dword ptr [esp + 0xc], eax
  42c27b: 8b 44 24 28                  	mov	eax, dword ptr [esp + 0x28]
  42c27f: 57                           	push	edi
  42c280: 8b f9                        	mov	edi, ecx
  42c282: 8b 6c 24 24                  	mov	ebp, dword ptr [esp + 0x24]
  42c286: 8b 48 3c                     	mov	ecx, dword ptr [eax + 0x3c]
  42c289: 55                           	push	ebp
  42c28a: 89 4c 24 20                  	mov	dword ptr [esp + 0x20], ecx
  42c28e: 8b 4e 2c                     	mov	ecx, dword ptr [esi + 0x2c]
  42c291: 0f af 48 4c                  	imul	ecx, dword ptr [eax + 0x4c]
  42c295: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42c29a: f7 e9                        	imul	ecx
  42c29c: c1 fa 05                     	sar	edx, 0x5
  42c29f: 8b c2                        	mov	eax, edx
  42c2a1: 8b cf                        	mov	ecx, edi
  42c2a3: c1 e8 1f                     	shr	eax, 0x1f
  42c2a6: 03 d0                        	add	edx, eax
  42c2a8: 8b da                        	mov	ebx, edx
  42c2aa: e8 71 fa ff ff               	call	0x42bd20 <.text+0x2ad20>
  42c2af: 55                           	push	ebp
  42c2b0: 8b cf                        	mov	ecx, edi
  42c2b2: 03 d8                        	add	ebx, eax
  42c2b4: e8 37 0a 00 00               	call	0x42ccf0 <.text+0x2bcf0>
  42c2b9: 8b d0                        	mov	edx, eax
  42c2bb: 55                           	push	ebp
  42c2bc: 8b cf                        	mov	ecx, edi
  42c2be: 89 54 24 2c                  	mov	dword ptr [esp + 0x2c], edx
  42c2c2: e8 29 0a 00 00               	call	0x42ccf0 <.text+0x2bcf0>
  42c2c7: 8b 4c 24 28                  	mov	ecx, dword ptr [esp + 0x28]
  42c2cb: 8b 54 24 2c                  	mov	edx, dword ptr [esp + 0x2c]
  42c2cf: 0f af c8                     	imul	ecx, eax
  42c2d2: 8b 6a 50                     	mov	ebp, dword ptr [edx + 0x50]
  42c2d5: 8b 56 20                     	mov	edx, dword ptr [esi + 0x20]
  42c2d8: 8b c1                        	mov	eax, ecx
  42c2da: 03 ea                        	add	ebp, edx
  42c2dc: 99                           	cdq
  42c2dd: d1 e5                        	shl	ebp
  42c2df: f7 fd                        	idiv	ebp
  42c2e1: 56                           	push	esi
  42c2e2: 8b cf                        	mov	ecx, edi
  42c2e4: 03 d8                        	add	ebx, eax
  42c2e6: e8 05 0a 00 00               	call	0x42ccf0 <.text+0x2bcf0>
  42c2eb: 8b 4e 40                     	mov	ecx, dword ptr [esi + 0x40]
  42c2ee: 03 c1                        	add	eax, ecx
  42c2f0: 8b 4c 24 1c                  	mov	ecx, dword ptr [esp + 0x1c]
  42c2f4: 99                           	cdq
  42c2f5: 2b c2                        	sub	eax, edx
  42c2f7: d1 f8                        	sar	eax
  42c2f9: f7 d8                        	neg	eax
  42c2fb: 03 d8                        	add	ebx, eax
  42c2fd: 8b 44 8e 64                  	mov	eax, dword ptr [esi + 4*ecx + 0x64]
  42c301: 99                           	cdq
  42c302: 8b e8                        	mov	ebp, eax
  42c304: 8b 86 90 00 00 00            	mov	eax, dword ptr [esi + 0x90]
  42c30a: 33 ea                        	xor	ebp, edx
  42c30c: 2b ea                        	sub	ebp, edx
  42c30e: 8b 15 b0 01 8a 00            	mov	edx, dword ptr [0x8a01b0]
  42c314: 85 c0                        	test	eax, eax
  42c316: 7e 1c                        	jle	0x42c334 <.text+0x2b334>
  42c318: 8d 3c c5 00 00 00 00         	lea	edi, [8*eax]
  42c31f: 2b f8                        	sub	edi, eax
  42c321: 8d 3c b8                     	lea	edi, [eax + 4*edi]
  42c324: c1 e7 04                     	shl	edi, 0x4
  42c327: 2b f8                        	sub	edi, eax
  42c329: 8d 04 7a                     	lea	eax, [edx + 2*edi]
  42c32c: 8b 44 88 74                  	mov	eax, dword ptr [eax + 4*ecx + 0x74]
  42c330: 89 44 24 18                  	mov	dword ptr [esp + 0x18], eax
  42c334: 8b 86 94 00 00 00            	mov	eax, dword ptr [esi + 0x94]
  42c33a: 85 c0                        	test	eax, eax
  42c33c: 7e 1c                        	jle	0x42c35a <.text+0x2b35a>
  42c33e: 8d 3c c5 00 00 00 00         	lea	edi, [8*eax]
  42c345: 2b f8                        	sub	edi, eax
  42c347: 8d 3c b8                     	lea	edi, [eax + 4*edi]
  42c34a: c1 e7 04                     	shl	edi, 0x4
  42c34d: 2b f8                        	sub	edi, eax
  42c34f: 8d 04 7a                     	lea	eax, [edx + 2*edi]
  42c352: 8b 44 88 74                  	mov	eax, dword ptr [eax + 4*ecx + 0x74]
  42c356: 89 44 24 14                  	mov	dword ptr [esp + 0x14], eax
  42c35a: 8b 86 98 00 00 00            	mov	eax, dword ptr [esi + 0x98]
  42c360: 85 c0                        	test	eax, eax
  42c362: 7e 1c                        	jle	0x42c380 <.text+0x2b380>
  42c364: 8d 3c c5 00 00 00 00         	lea	edi, [8*eax]
  42c36b: 2b f8                        	sub	edi, eax
  42c36d: 8d 3c b8                     	lea	edi, [eax + 4*edi]
  42c370: c1 e7 04                     	shl	edi, 0x4
  42c373: 2b f8                        	sub	edi, eax
  42c375: 8d 14 7a                     	lea	edx, [edx + 2*edi]
  42c378: 8b 44 8a 74                  	mov	eax, dword ptr [edx + 4*ecx + 0x74]
  42c37c: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  42c380: e8 e0 51 02 00               	call	0x451565 <.text+0x50565>
  42c385: 8b c8                        	mov	ecx, eax
  42c387: b8 03 b5 7e a5               	mov	eax, 0xa57eb503
  42c38c: f7 e9                        	imul	ecx
  42c38e: 03 d1                        	add	edx, ecx
  42c390: c1 fa 06                     	sar	edx, 0x6
  42c393: 8b ca                        	mov	ecx, edx
  42c395: c1 e9 1f                     	shr	ecx, 0x1f
  42c398: 8d 4c 0a 01                  	lea	ecx, [edx + ecx + 0x1]
  42c39c: 8b 54 24 2c                  	mov	edx, dword ptr [esp + 0x2c]
  42c3a0: 8b 42 4c                     	mov	eax, dword ptr [edx + 0x4c]
  42c3a3: 99                           	cdq
  42c3a4: f7 f9                        	idiv	ecx
  42c3a6: 8b f8                        	mov	edi, eax
  42c3a8: 03 fb                        	add	edi, ebx
  42c3aa: e8 b6 51 02 00               	call	0x451565 <.text+0x50565>
  42c3af: 8b c8                        	mov	ecx, eax
  42c3b1: b8 f3 1a ca 6b               	mov	eax, 0x6bca1af3
  42c3b6: f7 e9                        	imul	ecx
  42c3b8: c1 fa 03                     	sar	edx, 0x3
  42c3bb: 8b c2                        	mov	eax, edx
  42c3bd: 8b 5c 24 14                  	mov	ebx, dword ptr [esp + 0x14]
  42c3c1: c1 e8 1f                     	shr	eax, 0x1f
  42c3c4: 8d 4c 02 01                  	lea	ecx, [edx + eax + 0x1]
  42c3c8: 8b 46 4c                     	mov	eax, dword ptr [esi + 0x4c]
  42c3cb: 99                           	cdq
  42c3cc: f7 f9                        	idiv	ecx
  42c3ce: 2b f8                        	sub	edi, eax
  42c3d0: 8b 44 24 10                  	mov	eax, dword ptr [esp + 0x10]
  42c3d4: 99                           	cdq
  42c3d5: 8b c8                        	mov	ecx, eax
  42c3d7: 8b c3                        	mov	eax, ebx
  42c3d9: 33 ca                        	xor	ecx, edx
  42c3db: 89 7c 24 2c                  	mov	dword ptr [esp + 0x2c], edi
  42c3df: 2b ca                        	sub	ecx, edx
  42c3e1: 8b 7c 24 18                  	mov	edi, dword ptr [esp + 0x18]
  42c3e5: 99                           	cdq
  42c3e6: db 44 24 2c                  	fild	dword ptr [esp + 0x2c]
  42c3ea: 33 c2                        	xor	eax, edx
  42c3ec: 2b c2                        	sub	eax, edx
  42c3ee: 0f af c8                     	imul	ecx, eax
  42c3f1: 8b c7                        	mov	eax, edi
  42c3f3: 99                           	cdq
  42c3f4: 33 c2                        	xor	eax, edx
  42c3f6: 2b c2                        	sub	eax, edx
  42c3f8: 0f af c8                     	imul	ecx, eax
  42c3fb: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42c400: f7 e9                        	imul	ecx
  42c402: c1 fa 05                     	sar	edx, 0x5
  42c405: 8b c2                        	mov	eax, edx
  42c407: c1 e8 1f                     	shr	eax, 0x1f
  42c40a: 03 d0                        	add	edx, eax
  42c40c: 0f af d5                     	imul	edx, ebp
  42c40f: 89 54 24 2c                  	mov	dword ptr [esp + 0x2c], edx
  42c413: db 44 24 2c                  	fild	dword ptr [esp + 0x2c]
  42c417: de c9                        	fmulp	st(1), st
  42c419: dc 0d 98 0c 46 00            	fmul	qword ptr [0x460c98]
  42c41f: e8 74 59 02 00               	call	0x451d98 <.text+0x50d98>
  42c424: 85 c0                        	test	eax, eax
  42c426: 7d 02                        	jge	0x42c42a <.text+0x2b42a>
  42c428: 33 c0                        	xor	eax, eax
  42c42a: 8b 4c 24 1c                  	mov	ecx, dword ptr [esp + 0x1c]
  42c42e: 85 c9                        	test	ecx, ecx
  42c430: 7e 1a                        	jle	0x42c44c <.text+0x2b44c>
  42c432: 8b 54 8e 64                  	mov	edx, dword ptr [esi + 4*ecx + 0x64]
  42c436: 85 d2                        	test	edx, edx
  42c438: 7c 10                        	jl	0x42c44a <.text+0x2b44a>
  42c43a: 85 ff                        	test	edi, edi
  42c43c: 7c 0c                        	jl	0x42c44a <.text+0x2b44a>
  42c43e: 85 db                        	test	ebx, ebx
  42c440: 7c 08                        	jl	0x42c44a <.text+0x2b44a>
  42c442: 8b 4c 24 10                  	mov	ecx, dword ptr [esp + 0x10]
  42c446: 85 c9                        	test	ecx, ecx
  42c448: 7d 02                        	jge	0x42c44c <.text+0x2b44c>
  42c44a: f7 d8                        	neg	eax
  42c44c: 5f                           	pop	edi
  42c44d: 5e                           	pop	esi
  42c44e: 5d                           	pop	ebp
  42c44f: 5b                           	pop	ebx
  42c450: 83 c4 10                     	add	esp, 0x10
  42c453: c2 0c 00                     	ret	0xc
  42c456: 90                           	nop
  42c457: 90                           	nop
  42c458: 90                           	nop
  42c459: 90                           	nop
  42c45a: 90                           	nop
  42c45b: 90                           	nop
  42c45c: 90                           	nop
  42c45d: 90                           	nop
  42c45e: 90                           	nop
  42c45f: 90                           	nop
  42c460: 8a 44 24 10                  	mov	al, byte ptr [esp + 0x10]
  42c464: 53                           	push	ebx
  42c465: 56                           	push	esi
  42c466: 57                           	push	edi
  42c467: 84 c0                        	test	al, al
  42c469: 74 11                        	je	0x42c47c <.text+0x2b47c>
  42c46b: 8b 7c 24 18                  	mov	edi, dword ptr [esp + 0x18]
  42c46f: 8b 5c 24 14                  	mov	ebx, dword ptr [esp + 0x14]
  42c473: 8b 57 40                     	mov	edx, dword ptr [edi + 0x40]
  42c476: 0f af 53 2c                  	imul	edx, dword ptr [ebx + 0x2c]
  42c47a: eb 0f                        	jmp	0x42c48b <.text+0x2b48b>
  42c47c: 8b 5c 24 14                  	mov	ebx, dword ptr [esp + 0x14]
  42c480: 8b 7c 24 18                  	mov	edi, dword ptr [esp + 0x18]
  42c484: 8b 53 34                     	mov	edx, dword ptr [ebx + 0x34]
  42c487: 0f af 57 40                  	imul	edx, dword ptr [edi + 0x40]
  42c48b: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42c490: 6a 04                        	push	0x4
  42c492: f7 ea                        	imul	edx
  42c494: c1 fa 05                     	sar	edx, 0x5
  42c497: 8b c2                        	mov	eax, edx
  42c499: c1 e8 1f                     	shr	eax, 0x1f
  42c49c: 03 d0                        	add	edx, eax
  42c49e: 8b f2                        	mov	esi, edx
  42c4a0: 8b 57 4c                     	mov	edx, dword ptr [edi + 0x4c]
  42c4a3: 03 f2                        	add	esi, edx
  42c4a5: 8b 53 04                     	mov	edx, dword ptr [ebx + 0x4]
  42c4a8: 52                           	push	edx
  42c4a9: e8 92 fa ff ff               	call	0x42bf40 <.text+0x2af40>
  42c4ae: 84 c0                        	test	al, al
  42c4b0: 74 02                        	je	0x42c4b4 <.text+0x2b4b4>
  42c4b2: 33 f6                        	xor	esi, esi
  42c4b4: 85 f6                        	test	esi, esi
  42c4b6: 8b c6                        	mov	eax, esi
  42c4b8: 7d 02                        	jge	0x42c4bc <.text+0x2b4bc>
  42c4ba: 33 c0                        	xor	eax, eax
  42c4bc: 8b 8b 14 02 00 00            	mov	ecx, dword ptr [ebx + 0x214]
  42c4c2: 5f                           	pop	edi
  42c4c3: 5e                           	pop	esi
  42c4c4: 5b                           	pop	ebx
  42c4c5: 85 c9                        	test	ecx, ecx
  42c4c7: 7f 02                        	jg	0x42c4cb <.text+0x2b4cb>
  42c4c9: f7 d8                        	neg	eax
  42c4cb: c2 10 00                     	ret	0x10
  42c4ce: 90                           	nop
  42c4cf: 90                           	nop
  42c4d0: 83 ec 08                     	sub	esp, 0x8
  42c4d3: 8b 44 24 0c                  	mov	eax, dword ptr [esp + 0xc]
  42c4d7: 53                           	push	ebx
  42c4d8: 55                           	push	ebp
  42c4d9: 56                           	push	esi
  42c4da: 8b 80 90 00 00 00            	mov	eax, dword ptr [eax + 0x90]
  42c4e0: 57                           	push	edi
  42c4e1: bd 64 00 00 00               	mov	ebp, 0x64
  42c4e6: 8d 0c c5 00 00 00 00         	lea	ecx, [8*eax]
  42c4ed: 89 6c 24 10                  	mov	dword ptr [esp + 0x10], ebp
  42c4f1: 2b c8                        	sub	ecx, eax
  42c4f3: 89 6c 24 14                  	mov	dword ptr [esp + 0x14], ebp
  42c4f7: 8d 14 88                     	lea	edx, [eax + 4*ecx]
  42c4fa: c1 e2 04                     	shl	edx, 0x4
  42c4fd: 2b d0                        	sub	edx, eax
  42c4ff: a1 48 1c 8e 00               	mov	eax, dword ptr [0x8e1c48]
  42c504: 8b 88 ac 00 00 00            	mov	ecx, dword ptr [eax + 0xac]
  42c50a: 8b bc 51 d4 00 00 00         	mov	edi, dword ptr [ecx + 2*edx + 0xd4]
  42c511: 8b 5c 51 48                  	mov	ebx, dword ptr [ecx + 2*edx + 0x48]
  42c515: 8d 04 51                     	lea	eax, [ecx + 2*edx]
  42c518: 8b 4c 24 20                  	mov	ecx, dword ptr [esp + 0x20]
  42c51c: 8b 44 b9 64                  	mov	eax, dword ptr [ecx + 4*edi + 0x64]
  42c520: 89 44 24 20                  	mov	dword ptr [esp + 0x20], eax
  42c524: 99                           	cdq
  42c525: 8b f0                        	mov	esi, eax
  42c527: 8b 81 90 00 00 00            	mov	eax, dword ptr [ecx + 0x90]
  42c52d: 33 f2                        	xor	esi, edx
  42c52f: 2b f2                        	sub	esi, edx
  42c531: 8b 15 b0 01 8a 00            	mov	edx, dword ptr [0x8a01b0]
  42c537: 85 c0                        	test	eax, eax
  42c539: 7e 18                        	jle	0x42c553 <.text+0x2b553>
  42c53b: 8d 2c c5 00 00 00 00         	lea	ebp, [8*eax]
  42c542: 2b e8                        	sub	ebp, eax
  42c544: 8d 2c a8                     	lea	ebp, [eax + 4*ebp]
  42c547: c1 e5 04                     	shl	ebp, 0x4
  42c54a: 2b e8                        	sub	ebp, eax
  42c54c: 8d 04 6a                     	lea	eax, [edx + 2*ebp]
  42c54f: 8b 6c b8 74                  	mov	ebp, dword ptr [eax + 4*edi + 0x74]
  42c553: 8b 81 94 00 00 00            	mov	eax, dword ptr [ecx + 0x94]
  42c559: 85 c0                        	test	eax, eax
  42c55b: 7e 24                        	jle	0x42c581 <.text+0x2b581>
  42c55d: 8d 14 c5 00 00 00 00         	lea	edx, [8*eax]
  42c564: 2b d0                        	sub	edx, eax
  42c566: 8d 14 90                     	lea	edx, [eax + 4*edx]
  42c569: c1 e2 04                     	shl	edx, 0x4
  42c56c: 2b d0                        	sub	edx, eax
  42c56e: 8b c2                        	mov	eax, edx
  42c570: 8b 15 b0 01 8a 00            	mov	edx, dword ptr [0x8a01b0]
  42c576: 8d 04 42                     	lea	eax, [edx + 2*eax]
  42c579: 8b 44 b8 74                  	mov	eax, dword ptr [eax + 4*edi + 0x74]
  42c57d: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  42c581: 8b 89 98 00 00 00            	mov	ecx, dword ptr [ecx + 0x98]
  42c587: 85 c9                        	test	ecx, ecx
  42c589: 7e 1c                        	jle	0x42c5a7 <.text+0x2b5a7>
  42c58b: 8d 04 cd 00 00 00 00         	lea	eax, [8*ecx]
  42c592: 2b c1                        	sub	eax, ecx
  42c594: 8d 04 81                     	lea	eax, [ecx + 4*eax]
  42c597: c1 e0 04                     	shl	eax, 0x4
  42c59a: 2b c1                        	sub	eax, ecx
  42c59c: 8d 0c 42                     	lea	ecx, [edx + 2*eax]
  42c59f: 8b 54 b9 74                  	mov	edx, dword ptr [ecx + 4*edi + 0x74]
  42c5a3: 89 54 24 14                  	mov	dword ptr [esp + 0x14], edx
  42c5a7: 8b 44 24 14                  	mov	eax, dword ptr [esp + 0x14]
  42c5ab: 99                           	cdq
  42c5ac: 8b c8                        	mov	ecx, eax
  42c5ae: 8b 44 24 10                  	mov	eax, dword ptr [esp + 0x10]
  42c5b2: 33 ca                        	xor	ecx, edx
  42c5b4: 2b ca                        	sub	ecx, edx
  42c5b6: 99                           	cdq
  42c5b7: 33 c2                        	xor	eax, edx
  42c5b9: 2b c2                        	sub	eax, edx
  42c5bb: 0f af c8                     	imul	ecx, eax
  42c5be: 8b c5                        	mov	eax, ebp
  42c5c0: 99                           	cdq
  42c5c1: 33 c2                        	xor	eax, edx
  42c5c3: 2b c2                        	sub	eax, edx
  42c5c5: 0f af c8                     	imul	ecx, eax
  42c5c8: 0f af ce                     	imul	ecx, esi
  42c5cb: b8 83 de 1b 43               	mov	eax, 0x431bde83
  42c5d0: f7 e9                        	imul	ecx
  42c5d2: c1 fa 12                     	sar	edx, 0x12
  42c5d5: 8b c2                        	mov	eax, edx
  42c5d7: c1 e8 1f                     	shr	eax, 0x1f
  42c5da: 03 d0                        	add	edx, eax
  42c5dc: 2b da                        	sub	ebx, edx
  42c5de: 8b c3                        	mov	eax, ebx
  42c5e0: 79 02                        	jns	0x42c5e4 <.text+0x2b5e4>
  42c5e2: 33 c0                        	xor	eax, eax
  42c5e4: 85 ff                        	test	edi, edi
  42c5e6: 7e 1e                        	jle	0x42c606 <.text+0x2b606>
  42c5e8: 8b 4c 24 20                  	mov	ecx, dword ptr [esp + 0x20]
  42c5ec: 85 c9                        	test	ecx, ecx
  42c5ee: 7c 14                        	jl	0x42c604 <.text+0x2b604>
  42c5f0: 85 ed                        	test	ebp, ebp
  42c5f2: 7c 10                        	jl	0x42c604 <.text+0x2b604>
  42c5f4: 8b 4c 24 10                  	mov	ecx, dword ptr [esp + 0x10]
  42c5f8: 85 c9                        	test	ecx, ecx
  42c5fa: 7c 08                        	jl	0x42c604 <.text+0x2b604>
  42c5fc: 8b 4c 24 14                  	mov	ecx, dword ptr [esp + 0x14]
  42c600: 85 c9                        	test	ecx, ecx
  42c602: 7d 02                        	jge	0x42c606 <.text+0x2b606>
  42c604: f7 d8                        	neg	eax
  42c606: 5f                           	pop	edi
  42c607: 5e                           	pop	esi
  42c608: 5d                           	pop	ebp
  42c609: 5b                           	pop	ebx
  42c60a: 83 c4 08                     	add	esp, 0x8
  42c60d: c2 08 00                     	ret	0x8
  42c610: 8b 54 24 04                  	mov	edx, dword ptr [esp + 0x4]
  42c614: 56                           	push	esi
  42c615: 33 c0                        	xor	eax, eax
  42c617: 57                           	push	edi
  42c618: 8b 8a 90 00 00 00            	mov	ecx, dword ptr [edx + 0x90]
  42c61e: 8b b2 94 00 00 00            	mov	esi, dword ptr [edx + 0x94]
  42c624: 8b 92 98 00 00 00            	mov	edx, dword ptr [edx + 0x98]
  42c62a: 8b 3d c8 01 8b 00            	mov	edi, dword ptr [0x8b01c8]
  42c630: 85 c9                        	test	ecx, ecx
  42c632: 7e 15                        	jle	0x42c649 <.text+0x2b649>
  42c634: 8d 04 cd 00 00 00 00         	lea	eax, [8*ecx]
  42c63b: 2b c1                        	sub	eax, ecx
  42c63d: 8d 04 81                     	lea	eax, [ecx + 4*eax]
  42c640: c1 e0 04                     	shl	eax, 0x4
  42c643: 2b c1                        	sub	eax, ecx
  42c645: 8b 44 47 48                  	mov	eax, dword ptr [edi + 2*eax + 0x48]
  42c649: 85 f6                        	test	esi, esi
  42c64b: 7e 15                        	jle	0x42c662 <.text+0x2b662>
  42c64d: 8d 0c f5 00 00 00 00         	lea	ecx, [8*esi]
  42c654: 2b ce                        	sub	ecx, esi
  42c656: 8d 0c 8e                     	lea	ecx, [esi + 4*ecx]
  42c659: c1 e1 04                     	shl	ecx, 0x4
  42c65c: 2b ce                        	sub	ecx, esi
  42c65e: 03 44 4f 48                  	add	eax, dword ptr [edi + 2*ecx + 0x48]
  42c662: 85 d2                        	test	edx, edx
  42c664: 7e 15                        	jle	0x42c67b <.text+0x2b67b>
  42c666: 8d 0c d5 00 00 00 00         	lea	ecx, [8*edx]
  42c66d: 2b ca                        	sub	ecx, edx
  42c66f: 8d 0c 8a                     	lea	ecx, [edx + 4*ecx]
  42c672: c1 e1 04                     	shl	ecx, 0x4
  42c675: 2b ca                        	sub	ecx, edx
  42c677: 03 44 4f 48                  	add	eax, dword ptr [edi + 2*ecx + 0x48]
  42c67b: 5f                           	pop	edi
  42c67c: 5e                           	pop	esi
  42c67d: c2 04 00                     	ret	0x4
  42c680: 83 ec 0c                     	sub	esp, 0xc
  42c683: 53                           	push	ebx
  42c684: 55                           	push	ebp
  42c685: 56                           	push	esi
  42c686: 8b 74 24 24                  	mov	esi, dword ptr [esp + 0x24]
  42c68a: 8b e9                        	mov	ebp, ecx
  42c68c: 33 c0                        	xor	eax, eax
  42c68e: 33 c9                        	xor	ecx, ecx
  42c690: 8d 56 10                     	lea	edx, [esi + 0x10]
  42c693: 88 0e                        	mov	byte ptr [esi], cl
  42c695: 89 4e 04                     	mov	dword ptr [esi + 0x4], ecx
  42c698: 89 4e 08                     	mov	dword ptr [esi + 0x8], ecx
  42c69b: 88 4e 0c                     	mov	byte ptr [esi + 0xc], cl
  42c69e: 89 02                        	mov	dword ptr [edx], eax
  42c6a0: 57                           	push	edi
  42c6a1: 8b 7c 24 2c                  	mov	edi, dword ptr [esp + 0x2c]
  42c6a5: 89 4c 24 14                  	mov	dword ptr [esp + 0x14], ecx
  42c6a9: 89 42 04                     	mov	dword ptr [edx + 0x4], eax
  42c6ac: 3b f9                        	cmp	edi, ecx
  42c6ae: 88 4c 24 13                  	mov	byte ptr [esp + 0x13], cl
  42c6b2: 89 42 08                     	mov	dword ptr [edx + 0x8], eax
  42c6b5: 89 42 0c                     	mov	dword ptr [edx + 0xc], eax
  42c6b8: 89 42 10                     	mov	dword ptr [edx + 0x10], eax
  42c6bb: 8d 56 24                     	lea	edx, [esi + 0x24]
  42c6be: 89 46 24                     	mov	dword ptr [esi + 0x24], eax
  42c6c1: 89 42 04                     	mov	dword ptr [edx + 0x4], eax
  42c6c4: 89 42 08                     	mov	dword ptr [edx + 0x8], eax
  42c6c7: 89 42 0c                     	mov	dword ptr [edx + 0xc], eax
  42c6ca: 89 42 10                     	mov	dword ptr [edx + 0x10], eax
  42c6cd: 89 4e 38                     	mov	dword ptr [esi + 0x38], ecx
  42c6d0: 89 4e 3c                     	mov	dword ptr [esi + 0x3c], ecx
  42c6d3: 88 46 40                     	mov	byte ptr [esi + 0x40], al
  42c6d6: c7 46 44 01 00 00 00         	mov	dword ptr [esi + 0x44], 0x1
  42c6dd: 88 46 48                     	mov	byte ptr [esi + 0x48], al
  42c6e0: c7 46 4c ff ff ff ff         	mov	dword ptr [esi + 0x4c], 0xffffffff
  42c6e7: 0f 84 9f 05 00 00            	je	0x42cc8c <.text+0x2bc8c>
  42c6ed: 8b 47 18                     	mov	eax, dword ptr [edi + 0x18]
  42c6f0: 3b c1                        	cmp	eax, ecx
  42c6f2: 89 44 24 28                  	mov	dword ptr [esp + 0x28], eax
  42c6f6: 89 46 4c                     	mov	dword ptr [esi + 0x4c], eax
  42c6f9: 0f 85 aa 00 00 00            	jne	0x42c7a9 <.text+0x2b7a9>
  42c6ff: 8b 44 24 20                  	mov	eax, dword ptr [esp + 0x20]
  42c703: 8b cd                        	mov	ecx, ebp
  42c705: 50                           	push	eax
  42c706: e8 75 f1 ff ff               	call	0x42b880 <.text+0x2a880>
  42c70b: 8b 4c 24 24                  	mov	ecx, dword ptr [esp + 0x24]
  42c70f: 8b d8                        	mov	ebx, eax
  42c711: 51                           	push	ecx
  42c712: 8b cd                        	mov	ecx, ebp
  42c714: e8 37 f2 ff ff               	call	0x42b950 <.text+0x2a950>
  42c719: 8b cb                        	mov	ecx, ebx
  42c71b: bb 00 00 00 00               	mov	ebx, 0x0
  42c720: 2b c8                        	sub	ecx, eax
  42c722: 0f 98 c3                     	sets	bl
  42c725: 4b                           	dec	ebx
  42c726: 23 d9                        	and	ebx, ecx
  42c728: e8 38 4e 02 00               	call	0x451565 <.text+0x50565>
  42c72d: 99                           	cdq
  42c72e: b9 64 00 00 00               	mov	ecx, 0x64
  42c733: f7 f9                        	idiv	ecx
  42c735: 3b d3                        	cmp	edx, ebx
  42c737: 0f 9c c0                     	setl	al
  42c73a: 84 c0                        	test	al, al
  42c73c: 88 06                        	mov	byte ptr [esi], al
  42c73e: 0f 84 48 05 00 00            	je	0x42cc8c <.text+0x2bc8c>
  42c744: 8b 54 24 24                  	mov	edx, dword ptr [esp + 0x24]
  42c748: 8b 44 24 20                  	mov	eax, dword ptr [esp + 0x20]
  42c74c: 52                           	push	edx
  42c74d: 50                           	push	eax
  42c74e: 8b cd                        	mov	ecx, ebp
  42c750: e8 6b f4 ff ff               	call	0x42bbc0 <.text+0x2abc0>
  42c755: 8b d8                        	mov	ebx, eax
  42c757: e8 09 4e 02 00               	call	0x451565 <.text+0x50565>
  42c75c: 99                           	cdq
  42c75d: b9 64 00 00 00               	mov	ecx, 0x64
  42c762: f7 f9                        	idiv	ecx
  42c764: 3b d3                        	cmp	edx, ebx
  42c766: 0f 9c c0                     	setl	al
  42c769: 88 44 24 13                  	mov	byte ptr [esp + 0x13], al
  42c76d: 88 46 0c                     	mov	byte ptr [esi + 0xc], al
  42c770: 8b 47 18                     	mov	eax, dword ptr [edi + 0x18]
  42c773: 83 f8 05                     	cmp	eax, 0x5
  42c776: 74 05                        	je	0x42c77d <.text+0x2b77d>
  42c778: 83 f8 06                     	cmp	eax, 0x6
  42c77b: 75 04                        	jne	0x42c781 <.text+0x2b781>
  42c77d: c6 46 0c 00                  	mov	byte ptr [esi + 0xc], 0x0
  42c781: 8b 54 24 24                  	mov	edx, dword ptr [esp + 0x24]
  42c785: 8b 5c 24 20                  	mov	ebx, dword ptr [esp + 0x20]
  42c789: 6a 00                        	push	0x0
  42c78b: 52                           	push	edx
  42c78c: 53                           	push	ebx
  42c78d: 8b cd                        	mov	ecx, ebp
  42c78f: e8 0c ee ff ff               	call	0x42b5a0 <.text+0x2a5a0>
  42c794: 53                           	push	ebx
  42c795: 8b cd                        	mov	ecx, ebp
  42c797: 89 44 24 24                  	mov	dword ptr [esp + 0x24], eax
  42c79b: e8 b0 05 00 00               	call	0x42cd50 <.text+0x2bd50>
  42c7a0: 01 44 24 20                  	add	dword ptr [esp + 0x20], eax
  42c7a4: e9 40 02 00 00               	jmp	0x42c9e9 <.text+0x2b9e9>
  42c7a9: 83 f8 04                     	cmp	eax, 0x4
  42c7ac: 0f 84 be 04 00 00            	je	0x42cc70 <.text+0x2bc70>
  42c7b2: 8b 47 30                     	mov	eax, dword ptr [edi + 0x30]
  42c7b5: 89 44 24 18                  	mov	dword ptr [esp + 0x18], eax
  42c7b9: 83 c0 fe                     	add	eax, -0x2
  42c7bc: 83 f8 04                     	cmp	eax, 0x4
  42c7bf: 77 6b                        	ja	0x42c82c <.text+0x2b82c>
  42c7c1: ff 24 85 98 cc 42 00         	jmp	dword ptr [4*eax + 0x42cc98]
  42c7c8: 8b 44 24 24                  	mov	eax, dword ptr [esp + 0x24]
  42c7cc: 8b 5c 24 20                  	mov	ebx, dword ptr [esp + 0x20]
  42c7d0: 57                           	push	edi
  42c7d1: 50                           	push	eax
  42c7d2: 53                           	push	ebx
  42c7d3: 8b cd                        	mov	ecx, ebp
  42c7d5: e8 56 ee ff ff               	call	0x42b630 <.text+0x2a630>
  42c7da: eb 62                        	jmp	0x42c83e <.text+0x2b83e>
  42c7dc: 8b 4c 24 24                  	mov	ecx, dword ptr [esp + 0x24]
  42c7e0: 8b 5c 24 20                  	mov	ebx, dword ptr [esp + 0x20]
  42c7e4: 57                           	push	edi
  42c7e5: 51                           	push	ecx
  42c7e6: 53                           	push	ebx
  42c7e7: 8b cd                        	mov	ecx, ebp
  42c7e9: e8 d2 ee ff ff               	call	0x42b6c0 <.text+0x2a6c0>
  42c7ee: eb 4e                        	jmp	0x42c83e <.text+0x2b83e>
  42c7f0: 8b 54 24 24                  	mov	edx, dword ptr [esp + 0x24]
  42c7f4: 8b 5c 24 20                  	mov	ebx, dword ptr [esp + 0x20]
  42c7f8: 57                           	push	edi
  42c7f9: 52                           	push	edx
  42c7fa: 53                           	push	ebx
  42c7fb: 8b cd                        	mov	ecx, ebp
  42c7fd: e8 4e ef ff ff               	call	0x42b750 <.text+0x2a750>
  42c802: eb 3a                        	jmp	0x42c83e <.text+0x2b83e>
  42c804: 8b 44 24 24                  	mov	eax, dword ptr [esp + 0x24]
  42c808: 8b 5c 24 20                  	mov	ebx, dword ptr [esp + 0x20]
  42c80c: 57                           	push	edi
  42c80d: 50                           	push	eax
  42c80e: 53                           	push	ebx
  42c80f: 8b cd                        	mov	ecx, ebp
  42c811: e8 ba ef ff ff               	call	0x42b7d0 <.text+0x2a7d0>
  42c816: eb 26                        	jmp	0x42c83e <.text+0x2b83e>
  42c818: 8b 4c 24 24                  	mov	ecx, dword ptr [esp + 0x24]
  42c81c: 8b 5c 24 20                  	mov	ebx, dword ptr [esp + 0x20]
  42c820: 57                           	push	edi
  42c821: 51                           	push	ecx
  42c822: 53                           	push	ebx
  42c823: 8b cd                        	mov	ecx, ebp
  42c825: e8 16 f0 ff ff               	call	0x42b840 <.text+0x2a840>
  42c82a: eb 12                        	jmp	0x42c83e <.text+0x2b83e>
  42c82c: 8b 54 24 24                  	mov	edx, dword ptr [esp + 0x24]
  42c830: 8b 5c 24 20                  	mov	ebx, dword ptr [esp + 0x20]
  42c834: 57                           	push	edi
  42c835: 52                           	push	edx
  42c836: 53                           	push	ebx
  42c837: 8b cd                        	mov	ecx, ebp
  42c839: e8 b2 ed ff ff               	call	0x42b5f0 <.text+0x2a5f0>
  42c83e: 53                           	push	ebx
  42c83f: 8b cd                        	mov	ecx, ebp
  42c841: 89 44 24 24                  	mov	dword ptr [esp + 0x24], eax
  42c845: e8 06 05 00 00               	call	0x42cd50 <.text+0x2bd50>
  42c84a: 8b 4c 24 20                  	mov	ecx, dword ptr [esp + 0x20]
  42c84e: 03 c8                        	add	ecx, eax
  42c850: 8b 47 40                     	mov	eax, dword ptr [edi + 0x40]
  42c853: 85 c0                        	test	eax, eax
  42c855: 89 4c 24 20                  	mov	dword ptr [esp + 0x20], ecx
  42c859: 75 08                        	jne	0x42c863 <.text+0x2b863>
  42c85b: c7 44 24 20 00 00 00 00      	mov	dword ptr [esp + 0x20], 0x0
  42c863: 8b 4c 24 24                  	mov	ecx, dword ptr [esp + 0x24]
  42c867: c6 06 01                     	mov	byte ptr [esi], 0x1
  42c86a: 8b 43 4c                     	mov	eax, dword ptr [ebx + 0x4c]
  42c86d: 6a 03                        	push	0x3
  42c86f: 2b 41 4c                     	sub	eax, dword ptr [ecx + 0x4c]
  42c872: 99                           	cdq
  42c873: 83 e2 07                     	and	edx, 0x7
  42c876: 03 c2                        	add	eax, edx
  42c878: 8b 57 48                     	mov	edx, dword ptr [edi + 0x48]
  42c87b: c1 f8 03                     	sar	eax, 0x3
  42c87e: 03 c2                        	add	eax, edx
  42c880: 89 44 24 30                  	mov	dword ptr [esp + 0x30], eax
  42c884: 8b 41 04                     	mov	eax, dword ptr [ecx + 0x4]
  42c887: 50                           	push	eax
  42c888: 8b cd                        	mov	ecx, ebp
  42c88a: e8 b1 f6 ff ff               	call	0x42bf40 <.text+0x2af40>
  42c88f: 84 c0                        	test	al, al
  42c891: 74 04                        	je	0x42c897 <.text+0x2b897>
  42c893: b1 01                        	mov	cl, 0x1
  42c895: eb 34                        	jmp	0x42c8cb <.text+0x2b8cb>
  42c897: 8b 44 24 24                  	mov	eax, dword ptr [esp + 0x24]
  42c89b: 8b 88 20 02 00 00            	mov	ecx, dword ptr [eax + 0x220]
  42c8a1: 85 c9                        	test	ecx, ecx
  42c8a3: 7e 04                        	jle	0x42c8a9 <.text+0x2b8a9>
  42c8a5: 32 c9                        	xor	cl, cl
  42c8a7: eb 22                        	jmp	0x42c8cb <.text+0x2b8cb>
  42c8a9: 8b 88 24 02 00 00            	mov	ecx, dword ptr [eax + 0x224]
  42c8af: 85 c9                        	test	ecx, ecx
  42c8b1: 7e 04                        	jle	0x42c8b7 <.text+0x2b8b7>
  42c8b3: b1 01                        	mov	cl, 0x1
  42c8b5: eb 14                        	jmp	0x42c8cb <.text+0x2b8cb>
  42c8b7: e8 a9 4c 02 00               	call	0x451565 <.text+0x50565>
  42c8bc: 99                           	cdq
  42c8bd: b9 64 00 00 00               	mov	ecx, 0x64
  42c8c2: f7 f9                        	idiv	ecx
  42c8c4: 3b 54 24 2c                  	cmp	edx, dword ptr [esp + 0x2c]
  42c8c8: 0f 9c c1                     	setl	cl
  42c8cb: 8b 44 24 28                  	mov	eax, dword ptr [esp + 0x28]
  42c8cf: 85 c0                        	test	eax, eax
  42c8d1: 0f 8e fc 00 00 00            	jle	0x42c9d3 <.text+0x2b9d3>
  42c8d7: 8b 47 44                     	mov	eax, dword ptr [edi + 0x44]
  42c8da: 85 c0                        	test	eax, eax
  42c8dc: 75 1b                        	jne	0x42c8f9 <.text+0x2b8f9>
  42c8de: 84 c9                        	test	cl, cl
  42c8e0: 74 17                        	je	0x42c8f9 <.text+0x2b8f9>
  42c8e2: 8b 54 24 24                  	mov	edx, dword ptr [esp + 0x24]
  42c8e6: 57                           	push	edi
  42c8e7: 52                           	push	edx
  42c8e8: 53                           	push	ebx
  42c8e9: 8b cd                        	mov	ecx, ebp
  42c8eb: e8 a0 f6 ff ff               	call	0x42bf90 <.text+0x2af90>
  42c8f0: 89 44 24 14                  	mov	dword ptr [esp + 0x14], eax
  42c8f4: e9 da 00 00 00               	jmp	0x42c9d3 <.text+0x2b9d3>
  42c8f9: 83 f8 01                     	cmp	eax, 0x1
  42c8fc: 75 1b                        	jne	0x42c919 <.text+0x2b919>
  42c8fe: 84 c9                        	test	cl, cl
  42c900: 74 17                        	je	0x42c919 <.text+0x2b919>
  42c902: 8b 44 24 24                  	mov	eax, dword ptr [esp + 0x24]
  42c906: 57                           	push	edi
  42c907: 50                           	push	eax
  42c908: 53                           	push	ebx
  42c909: 8b cd                        	mov	ecx, ebp
  42c90b: e8 50 f9 ff ff               	call	0x42c260 <.text+0x2b260>
  42c910: 89 44 24 14                  	mov	dword ptr [esp + 0x14], eax
  42c914: e9 ba 00 00 00               	jmp	0x42c9d3 <.text+0x2b9d3>
  42c919: 83 f8 02                     	cmp	eax, 0x2
  42c91c: 75 3e                        	jne	0x42c95c <.text+0x2b95c>
  42c91e: 8b 47 54                     	mov	eax, dword ptr [edi + 0x54]
  42c921: 85 c0                        	test	eax, eax
  42c923: 7e 2e                        	jle	0x42c953 <.text+0x2b953>
  42c925: 8b 4c 24 24                  	mov	ecx, dword ptr [esp + 0x24]
  42c929: 6a 01                        	push	0x1
  42c92b: 57                           	push	edi
  42c92c: 51                           	push	ecx
  42c92d: 53                           	push	ebx
  42c92e: 8b cd                        	mov	ecx, ebp
  42c930: e8 2b fb ff ff               	call	0x42c460 <.text+0x2b460>
  42c935: 8b 4f 54                     	mov	ecx, dword ptr [edi + 0x54]
  42c938: 89 44 24 14                  	mov	dword ptr [esp + 0x14], eax
  42c93c: 0f af c8                     	imul	ecx, eax
  42c93f: b8 1f 85 eb 51               	mov	eax, 0x51eb851f
  42c944: f7 e9                        	imul	ecx
  42c946: c1 fa 05                     	sar	edx, 0x5
  42c949: 8b c2                        	mov	eax, edx
  42c94b: c1 e8 1f                     	shr	eax, 0x1f
  42c94e: 03 d0                        	add	edx, eax
  42c950: 89 56 04                     	mov	dword ptr [esi + 0x4], edx
  42c953: 8b 47 58                     	mov	eax, dword ptr [edi + 0x58]
  42c956: 85 c0                        	test	eax, eax
  42c958: 7e 71                        	jle	0x42c9cb <.text+0x2b9cb>
  42c95a: eb 41                        	jmp	0x42c99d <.text+0x2b99d>
  42c95c: 83 f8 03                     	cmp	eax, 0x3
  42c95f: 75 72                        	jne	0x42c9d3 <.text+0x2b9d3>
  42c961: 8b 47 54                     	mov	eax, dword ptr [edi + 0x54]
  42c964: 85 c0                        	test	eax, eax
  42c966: 7e 2e                        	jle	0x42c996 <.text+0x2b996>
  42c968: 8b 4c 24 24                  	mov	ecx, dword ptr [esp + 0x24]
  42c96c: 6a 01                        	push	0x1
  42c96e: 57                           	push	edi
  42c96f: 51                           	push	ecx
  42c970: 53                           	push	ebx
  42c971: 8b cd                        	mov	ecx, ebp
  42c973: e8 e8 fa ff ff               	call	0x42c460 <.text+0x2b460>
  42c978: 8b 4f 54                     	mov	ecx, dword ptr [edi + 0x54]
  42c97b: 89 44 24 14                  	mov	dword ptr [esp + 0x14], eax
  42c97f: 0f af c8                     	imul	ecx, eax
