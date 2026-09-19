; 《幽城幻剑录》固化46证据：selector setter与范围夹取
; 用途：证明0x43E6F0写入selector后进入0x43E720，并按对象内Section0范围夹取。


/mnt/data/work46/input/RPG.exe:	file format coff-i386

Disassembly of section .text:

00401000 <.text>:
  43e450: 01 5e c2                     	add	dword ptr [esi - 0x3e], ebx
  43e453: 0c 00                        	or	al, 0x0
  43e455: 32 c0                        	xor	al, al
  43e457: 5e                           	pop	esi
  43e458: c2 0c 00                     	ret	0xc
  43e45b: 90                           	nop
  43e45c: 90                           	nop
  43e45d: 90                           	nop
  43e45e: 90                           	nop
  43e45f: 90                           	nop
  43e460: 6a ff                        	push	-0x1
  43e462: 68 7b f6 45 00               	push	0x45f67b
  43e467: 64 a1 00 00 00 00            	mov	eax, dword ptr fs:[0x0]
  43e46d: 50                           	push	eax
  43e46e: 64 89 25 00 00 00 00         	mov	dword ptr fs:[0x0], esp
  43e475: 51                           	push	ecx
  43e476: 55                           	push	ebp
  43e477: 57                           	push	edi
  43e478: 8b e9                        	mov	ebp, ecx
  43e47a: e8 11 01 00 00               	call	0x43e590 <.text+0x3d590>
  43e47f: 68 34 01 00 00               	push	0x134
  43e484: e8 d6 31 01 00               	call	0x45165f <.text+0x5065f>
  43e489: 83 c4 04                     	add	esp, 0x4
  43e48c: 89 44 24 08                  	mov	dword ptr [esp + 0x8], eax
  43e490: 8b 7c 24 1c                  	mov	edi, dword ptr [esp + 0x1c]
  43e494: c7 44 24 14 00 00 00 00      	mov	dword ptr [esp + 0x14], 0x0
  43e49c: 85 c0                        	test	eax, eax
  43e49e: 74 11                        	je	0x43e4b1 <.text+0x3d4b1>
  43e4a0: 8b 0d 74 1c 8e 00            	mov	ecx, dword ptr [0x8e1c74]
  43e4a6: 51                           	push	ecx
  43e4a7: 57                           	push	edi
  43e4a8: 8b c8                        	mov	ecx, eax
  43e4aa: e8 b1 a6 fe ff               	call	0x428b60 <.text+0x27b60>
  43e4af: eb 02                        	jmp	0x43e4b3 <.text+0x3d4b3>
  43e4b1: 33 c0                        	xor	eax, eax
  43e4b3: 85 c0                        	test	eax, eax
  43e4b5: 0f 95 c1                     	setne	cl
  43e4b8: 84 c9                        	test	cl, cl
  43e4ba: 89 85 5c 03 00 00            	mov	dword ptr [ebp + 0x35c], eax
  43e4c0: 88 4d 08                     	mov	byte ptr [ebp + 0x8], cl
  43e4c3: 0f 84 a4 00 00 00            	je	0x43e56d <.text+0x3d56d>
  43e4c9: 8a 88 2d 01 00 00            	mov	cl, byte ptr [eax + 0x12d]
  43e4cf: 84 c9                        	test	cl, cl
  43e4d1: 75 04                        	jne	0x43e4d7 <.text+0x3d4d7>
  43e4d3: 33 c9                        	xor	ecx, ecx
  43e4d5: eb 09                        	jmp	0x43e4e0 <.text+0x3d4e0>
  43e4d7: 8b 50 20                     	mov	edx, dword ptr [eax + 0x20]
  43e4da: 33 c9                        	xor	ecx, ecx
  43e4dc: 66 8b 4a 14                  	mov	cx, word ptr [edx + 0x14]
  43e4e0: 56                           	push	esi
  43e4e1: 8b 75 7c                     	mov	esi, dword ptr [ebp + 0x7c]
  43e4e4: 85 f6                        	test	esi, esi
  43e4e6: 89 4d 18                     	mov	dword ptr [ebp + 0x18], ecx
  43e4e9: c7 45 2c 00 00 00 00         	mov	dword ptr [ebp + 0x2c], 0x0
  43e4f0: ba 01 00 00 00               	mov	edx, 0x1
  43e4f5: 74 03                        	je	0x43e4fa <.text+0x3d4fa>
  43e4f7: 89 55 2c                     	mov	dword ptr [ebp + 0x2c], edx
  43e4fa: 89 4d 30                     	mov	dword ptr [ebp + 0x30], ecx
  43e4fd: 8a 88 2d 01 00 00            	mov	cl, byte ptr [eax + 0x12d]
  43e503: 84 c9                        	test	cl, cl
  43e505: 75 04                        	jne	0x43e50b <.text+0x3d50b>
  43e507: 33 c0                        	xor	eax, eax
  43e509: eb 09                        	jmp	0x43e514 <.text+0x3d514>
  43e50b: 8b 48 34                     	mov	ecx, dword ptr [eax + 0x34]
  43e50e: 33 c0                        	xor	eax, eax
  43e510: 66 8b 41 22                  	mov	ax, word ptr [ecx + 0x22]
  43e514: 89 45 0c                     	mov	dword ptr [ebp + 0xc], eax
  43e517: 83 c9 ff                     	or	ecx, -0x1
  43e51a: 33 c0                        	xor	eax, eax
  43e51c: 88 55 0a                     	mov	byte ptr [ebp + 0xa], dl
  43e51f: c7 45 1c 02 00 00 00         	mov	dword ptr [ebp + 0x1c], 0x2
  43e526: c7 45 54 00 00 00 00         	mov	dword ptr [ebp + 0x54], 0x0
  43e52d: f2 ae                        	repne		scasb	al, byte ptr es:[edi]
  43e52f: f7 d1                        	not	ecx
  43e531: 2b f9                        	sub	edi, ecx
  43e533: 8d 95 90 00 00 00            	lea	edx, [ebp + 0x90]
  43e539: 8b c1                        	mov	eax, ecx
  43e53b: 8b f7                        	mov	esi, edi
  43e53d: 8b fa                        	mov	edi, edx
  43e53f: c1 e9 02                     	shr	ecx, 0x2
  43e542: f3 a5                        	rep		movsd	dword ptr es:[edi], dword ptr [esi]
  43e544: 8b c8                        	mov	ecx, eax
  43e546: 83 e1 03                     	and	ecx, 0x3
  43e549: f3 a4                        	rep		movsb	byte ptr es:[edi], byte ptr [esi]
  43e54b: 8b 85 5c 03 00 00            	mov	eax, dword ptr [ebp + 0x35c]
  43e551: 5e                           	pop	esi
  43e552: 8a 88 2d 01 00 00            	mov	cl, byte ptr [eax + 0x12d]
  43e558: 84 c9                        	test	cl, cl
  43e55a: 75 04                        	jne	0x43e560 <.text+0x3d560>
  43e55c: 33 c0                        	xor	eax, eax
  43e55e: eb 07                        	jmp	0x43e567 <.text+0x3d567>
  43e560: 8b 48 20                     	mov	ecx, dword ptr [eax + 0x20]
  43e563: 66 8b 41 48                  	mov	ax, word ptr [ecx + 0x48]
  43e567: 0f bf d0                     	movsx	edx, ax
  43e56a: 89 55 34                     	mov	dword ptr [ebp + 0x34], edx
  43e56d: 8b 4c 24 0c                  	mov	ecx, dword ptr [esp + 0xc]
  43e571: 8a 45 08                     	mov	al, byte ptr [ebp + 0x8]
  43e574: 5f                           	pop	edi
  43e575: 5d                           	pop	ebp
  43e576: 64 89 0d 00 00 00 00         	mov	dword ptr fs:[0x0], ecx
  43e57d: 83 c4 10                     	add	esp, 0x10
  43e580: c2 04 00                     	ret	0x4
  43e583: 90                           	nop
  43e584: 90                           	nop
  43e585: 90                           	nop
  43e586: 90                           	nop
  43e587: 90                           	nop
  43e588: 90                           	nop
  43e589: 90                           	nop
  43e58a: 90                           	nop
  43e58b: 90                           	nop
  43e58c: 90                           	nop
  43e58d: 90                           	nop
  43e58e: 90                           	nop
  43e58f: 90                           	nop
  43e590: 53                           	push	ebx
  43e591: 56                           	push	esi
  43e592: 8b f1                        	mov	esi, ecx
  43e594: 57                           	push	edi
  43e595: 33 db                        	xor	ebx, ebx
  43e597: 8b be 5c 03 00 00            	mov	edi, dword ptr [esi + 0x35c]
  43e59d: c7 46 1c 02 00 00 00         	mov	dword ptr [esi + 0x1c], 0x2
  43e5a4: 3b fb                        	cmp	edi, ebx
  43e5a6: 74 16                        	je	0x43e5be <.text+0x3d5be>
  43e5a8: 8b cf                        	mov	ecx, edi
  43e5aa: e8 d1 a6 fe ff               	call	0x428c80 <.text+0x27c80>
  43e5af: 57                           	push	edi
  43e5b0: e8 9b 2f 01 00               	call	0x451550 <.text+0x50550>
  43e5b5: 83 c4 04                     	add	esp, 0x4
  43e5b8: 89 9e 5c 03 00 00            	mov	dword ptr [esi + 0x35c], ebx
  43e5be: 8b 7e 04                     	mov	edi, dword ptr [esi + 0x4]
  43e5c1: 3b fb                        	cmp	edi, ebx
  43e5c3: 74 13                        	je	0x43e5d8 <.text+0x3d5d8>
  43e5c5: 8b cf                        	mov	ecx, edi
  43e5c7: e8 b4 a6 fe ff               	call	0x428c80 <.text+0x27c80>
  43e5cc: 57                           	push	edi
  43e5cd: e8 7e 2f 01 00               	call	0x451550 <.text+0x50550>
  43e5d2: 83 c4 04                     	add	esp, 0x4
  43e5d5: 89 5e 04                     	mov	dword ptr [esi + 0x4], ebx
  43e5d8: 88 5e 08                     	mov	byte ptr [esi + 0x8], bl
  43e5db: 5f                           	pop	edi
  43e5dc: 5e                           	pop	esi
  43e5dd: 5b                           	pop	ebx
  43e5de: c3                           	ret
  43e5df: 90                           	nop
  43e5e0: 8a 41 08                     	mov	al, byte ptr [ecx + 0x8]
  43e5e3: 84 c0                        	test	al, al
  43e5e5: 74 48                        	je	0x43e62f <.text+0x3d62f>
  43e5e7: 8b 44 24 04                  	mov	eax, dword ptr [esp + 0x4]
  43e5eb: 8b 54 24 08                  	mov	edx, dword ptr [esp + 0x8]
  43e5ef: 3b c2                        	cmp	eax, edx
  43e5f1: 56                           	push	esi
  43e5f2: 7e 06                        	jle	0x43e5fa <.text+0x3d5fa>
  43e5f4: 8b f0                        	mov	esi, eax
  43e5f6: 8b c2                        	mov	eax, edx
  43e5f8: 8b d6                        	mov	edx, esi
  43e5fa: 85 c0                        	test	eax, eax
  43e5fc: 7d 02                        	jge	0x43e600 <.text+0x3d600>
  43e5fe: 33 c0                        	xor	eax, eax
  43e600: 85 d2                        	test	edx, edx
  43e602: 7d 03                        	jge	0x43e607 <.text+0x3d607>
  43e604: 8b 51 18                     	mov	edx, dword ptr [ecx + 0x18]
  43e607: 85 c0                        	test	eax, eax
  43e609: 7c 0f                        	jl	0x43e61a <.text+0x3d61a>
  43e60b: 8b 71 18                     	mov	esi, dword ptr [ecx + 0x18]
  43e60e: 3b c6                        	cmp	eax, esi
  43e610: 7e 05                        	jle	0x43e617 <.text+0x3d617>
  43e612: 89 71 2c                     	mov	dword ptr [ecx + 0x2c], esi
  43e615: eb 03                        	jmp	0x43e61a <.text+0x3d61a>
  43e617: 89 41 2c                     	mov	dword ptr [ecx + 0x2c], eax
  43e61a: 85 d2                        	test	edx, edx
  43e61c: 5e                           	pop	esi
  43e61d: 7c 10                        	jl	0x43e62f <.text+0x3d62f>
  43e61f: 8b 41 18                     	mov	eax, dword ptr [ecx + 0x18]
  43e622: 3b d0                        	cmp	edx, eax
  43e624: 7e 06                        	jle	0x43e62c <.text+0x3d62c>
  43e626: 89 41 30                     	mov	dword ptr [ecx + 0x30], eax
  43e629: c2 08 00                     	ret	0x8
  43e62c: 89 51 30                     	mov	dword ptr [ecx + 0x30], edx
  43e62f: c2 08 00                     	ret	0x8
  43e632: 90                           	nop
  43e633: 90                           	nop
  43e634: 90                           	nop
  43e635: 90                           	nop
  43e636: 90                           	nop
  43e637: 90                           	nop
  43e638: 90                           	nop
  43e639: 90                           	nop
  43e63a: 90                           	nop
  43e63b: 90                           	nop
  43e63c: 90                           	nop
  43e63d: 90                           	nop
  43e63e: 90                           	nop
  43e63f: 90                           	nop
  43e640: 56                           	push	esi
  43e641: 8b f1                        	mov	esi, ecx
  43e643: 8a 46 08                     	mov	al, byte ptr [esi + 0x8]
  43e646: 84 c0                        	test	al, al
  43e648: 74 1c                        	je	0x43e666 <.text+0x3d666>
  43e64a: 8b 4c 24 08                  	mov	ecx, dword ptr [esp + 0x8]
  43e64e: 8d 46 10                     	lea	eax, [esi + 0x10]
  43e651: 50                           	push	eax
  43e652: 89 08                        	mov	dword ptr [eax], ecx
  43e654: 8b ce                        	mov	ecx, esi
  43e656: e8 c5 00 00 00               	call	0x43e720 <.text+0x3d720>
  43e65b: b8 01 00 00 00               	mov	eax, 0x1
  43e660: 89 46 1c                     	mov	dword ptr [esi + 0x1c], eax
  43e663: 88 46 38                     	mov	byte ptr [esi + 0x38], al
  43e666: 5e                           	pop	esi
  43e667: c2 04 00                     	ret	0x4
  43e66a: 90                           	nop
  43e66b: 90                           	nop
  43e66c: 90                           	nop
  43e66d: 90                           	nop
  43e66e: 90                           	nop
  43e66f: 90                           	nop
  43e670: 56                           	push	esi
  43e671: 8b f1                        	mov	esi, ecx
  43e673: 8a 46 08                     	mov	al, byte ptr [esi + 0x8]
  43e676: 84 c0                        	test	al, al
  43e678: 74 1c                        	je	0x43e696 <.text+0x3d696>
  43e67a: 8b 4c 24 08                  	mov	ecx, dword ptr [esp + 0x8]
  43e67e: 8d 46 10                     	lea	eax, [esi + 0x10]
  43e681: 50                           	push	eax
  43e682: 89 08                        	mov	dword ptr [eax], ecx
  43e684: 8b ce                        	mov	ecx, esi
  43e686: e8 95 00 00 00               	call	0x43e720 <.text+0x3d720>
  43e68b: c7 46 1c 07 00 00 00         	mov	dword ptr [esi + 0x1c], 0x7
  43e692: c6 46 38 01                  	mov	byte ptr [esi + 0x38], 0x1
  43e696: 5e                           	pop	esi
  43e697: c2 04 00                     	ret	0x4
  43e69a: 90                           	nop
  43e69b: 90                           	nop
  43e69c: 90                           	nop
  43e69d: 90                           	nop
  43e69e: 90                           	nop
  43e69f: 90                           	nop
  43e6a0: 56                           	push	esi
  43e6a1: 8b f1                        	mov	esi, ecx
  43e6a3: 8a 46 08                     	mov	al, byte ptr [esi + 0x8]
  43e6a6: 84 c0                        	test	al, al
  43e6a8: 74 1c                        	je	0x43e6c6 <.text+0x3d6c6>
  43e6aa: 8b 4c 24 08                  	mov	ecx, dword ptr [esp + 0x8]
  43e6ae: 8d 46 10                     	lea	eax, [esi + 0x10]
  43e6b1: 50                           	push	eax
  43e6b2: 89 08                        	mov	dword ptr [eax], ecx
  43e6b4: 8b ce                        	mov	ecx, esi
  43e6b6: e8 65 00 00 00               	call	0x43e720 <.text+0x3d720>
  43e6bb: c7 46 1c 05 00 00 00         	mov	dword ptr [esi + 0x1c], 0x5
  43e6c2: c6 46 38 01                  	mov	byte ptr [esi + 0x38], 0x1
  43e6c6: 5e                           	pop	esi
  43e6c7: c2 04 00                     	ret	0x4
  43e6ca: 90                           	nop
  43e6cb: 90                           	nop
  43e6cc: 90                           	nop
  43e6cd: 90                           	nop
  43e6ce: 90                           	nop
  43e6cf: 90                           	nop
  43e6d0: 8a 41 08                     	mov	al, byte ptr [ecx + 0x8]
  43e6d3: 84 c0                        	test	al, al
  43e6d5: 74 0b                        	je	0x43e6e2 <.text+0x3d6e2>
  43e6d7: c7 41 1c 02 00 00 00         	mov	dword ptr [ecx + 0x1c], 0x2
  43e6de: c6 41 38 01                  	mov	byte ptr [ecx + 0x38], 0x1
  43e6e2: c3                           	ret
  43e6e3: 90                           	nop
  43e6e4: 90                           	nop
  43e6e5: 90                           	nop
  43e6e6: 90                           	nop
  43e6e7: 90                           	nop
  43e6e8: 90                           	nop
  43e6e9: 90                           	nop
  43e6ea: 90                           	nop
  43e6eb: 90                           	nop
  43e6ec: 90                           	nop
  43e6ed: 90                           	nop
  43e6ee: 90                           	nop
  43e6ef: 90                           	nop
  43e6f0: 56                           	push	esi
  43e6f1: 8b f1                        	mov	esi, ecx
  43e6f3: 8a 46 08                     	mov	al, byte ptr [esi + 0x8]
  43e6f6: 84 c0                        	test	al, al
  43e6f8: 74 20                        	je	0x43e71a <.text+0x3d71a>
  43e6fa: 8b 4c 24 08                  	mov	ecx, dword ptr [esp + 0x8]
  43e6fe: 85 c9                        	test	ecx, ecx
  43e700: 7c 0d                        	jl	0x43e70f <.text+0x3d70f>
  43e702: 8d 46 10                     	lea	eax, [esi + 0x10]
  43e705: 50                           	push	eax
  43e706: 89 08                        	mov	dword ptr [eax], ecx
  43e708: 8b ce                        	mov	ecx, esi
  43e70a: e8 11 00 00 00               	call	0x43e720 <.text+0x3d720>
  43e70f: c7 46 1c 03 00 00 00         	mov	dword ptr [esi + 0x1c], 0x3
  43e716: c6 46 38 01                  	mov	byte ptr [esi + 0x38], 0x1
  43e71a: 5e                           	pop	esi
  43e71b: c2 04 00                     	ret	0x4
  43e71e: 90                           	nop
  43e71f: 90                           	nop
  43e720: 8b 44 24 04                  	mov	eax, dword ptr [esp + 0x4]
  43e724: 8b 51 30                     	mov	edx, dword ptr [ecx + 0x30]
  43e727: 56                           	push	esi
  43e728: 8b 30                        	mov	esi, dword ptr [eax]
  43e72a: 3b f2                        	cmp	esi, edx
  43e72c: 5e                           	pop	esi
  43e72d: 7c 03                        	jl	0x43e732 <.text+0x3d732>
  43e72f: 4a                           	dec	edx
  43e730: 89 10                        	mov	dword ptr [eax], edx
  43e732: 8b 49 2c                     	mov	ecx, dword ptr [ecx + 0x2c]
  43e735: 8b 10                        	mov	edx, dword ptr [eax]
  43e737: 3b d1                        	cmp	edx, ecx
  43e739: 7d 02                        	jge	0x43e73d <.text+0x3d73d>
  43e73b: 89 08                        	mov	dword ptr [eax], ecx
  43e73d: c2 04 00                     	ret	0x4
  43e740: 8a 41 08                     	mov	al, byte ptr [ecx + 0x8]
  43e743: 84 c0                        	test	al, al
  43e745: 75 03                        	jne	0x43e74a <.text+0x3d74a>
  43e747: 32 c0                        	xor	al, al
  43e749: c3                           	ret
  43e74a: 8b 41 54                     	mov	eax, dword ptr [ecx + 0x54]
  43e74d: 85 c0                        	test	eax, eax
  43e74f: 74 08                        	je	0x43e759 <.text+0x3d759>
  43e751: e8 ea 00 00 00               	call	0x43e840 <.text+0x3d840>
  43e756: b0 01                        	mov	al, 0x1
  43e758: c3                           	ret
  43e759: e8 12 00 00 00               	call	0x43e770 <.text+0x3d770>
  43e75e: b0 01                        	mov	al, 0x1
  43e760: c3                           	ret
  43e761: 90                           	nop
  43e762: 90                           	nop
  43e763: 90                           	nop
  43e764: 90                           	nop
  43e765: 90                           	nop
  43e766: 90                           	nop
  43e767: 90                           	nop
  43e768: 90                           	nop
  43e769: 90                           	nop
  43e76a: 90                           	nop
  43e76b: 90                           	nop
  43e76c: 90                           	nop
  43e76d: 90                           	nop
  43e76e: 90                           	nop
  43e76f: 90                           	nop
  43e770: 8a 51 0a                     	mov	dl, byte ptr [ecx + 0xa]
  43e773: 8b 41 1c                     	mov	eax, dword ptr [ecx + 0x1c]
  43e776: 84 d2                        	test	dl, dl
  43e778: 89 41 20                     	mov	dword ptr [ecx + 0x20], eax
  43e77b: 0f 84 94 00 00 00            	je	0x43e815 <.text+0x3d815>
  43e781: 8d 50 ff                     	lea	edx, [eax - 0x1]
  43e784: 83 fa 06                     	cmp	edx, 0x6
  43e787: 0f 87 88 00 00 00            	ja	0x43e815 <.text+0x3d815>
  43e78d: ff 24 95 1c e8 43 00         	jmp	dword ptr [4*edx + 0x43e81c]
