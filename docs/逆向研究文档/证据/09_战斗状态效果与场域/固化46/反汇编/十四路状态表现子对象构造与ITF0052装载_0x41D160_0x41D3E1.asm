; 《幽城幻剑录》固化46证据：14个状态表现child构造与ITF0052装载
; 用途：证明两排各7个child、全部加载Fight\\ITF\\ITF0052.SF2、位置和初始selector。


/mnt/data/work46/input/RPG.exe:	file format coff-i386

Disassembly of section .text:

00401000 <.text>:
  41d150: 44                           	inc	esp
  41d151: 24 04                        	and	al, 0x4
  41d153: c6 00 00                     	mov	byte ptr [eax], 0x0
  41d156: b0 01                        	mov	al, 0x1
  41d158: c2 04 00                     	ret	0x4
  41d15b: 90                           	nop
  41d15c: 90                           	nop
  41d15d: 90                           	nop
  41d15e: 90                           	nop
  41d15f: 90                           	nop
  41d160: 6a ff                        	push	-0x1
  41d162: 68 c2 e0 45 00               	push	0x45e0c2
  41d167: 64 a1 00 00 00 00            	mov	eax, dword ptr fs:[0x0]
  41d16d: 50                           	push	eax
  41d16e: 64 89 25 00 00 00 00         	mov	dword ptr fs:[0x0], esp
  41d175: 83 ec 24                     	sub	esp, 0x24
  41d178: 53                           	push	ebx
  41d179: 55                           	push	ebp
  41d17a: 56                           	push	esi
  41d17b: 8b f1                        	mov	esi, ecx
  41d17d: 57                           	push	edi
  41d17e: 89 74 24 14                  	mov	dword ptr [esp + 0x14], esi
  41d182: e8 89 3d 01 00               	call	0x430f10 <.text+0x2ff10>
  41d187: 33 db                        	xor	ebx, ebx
  41d189: 8b ce                        	mov	ecx, esi
  41d18b: 53                           	push	ebx
  41d18c: 68 48 4b 4c 00               	push	0x4c4b48
  41d191: 56                           	push	esi
  41d192: 89 5c 24 48                  	mov	dword ptr [esp + 0x48], ebx
  41d196: c7 06 d0 0a 46 00            	mov	dword ptr [esi], 0x460ad0
  41d19c: e8 8f 72 01 00               	call	0x434430 <.text+0x33430>
  41d1a1: 8d 8e 18 02 00 00            	lea	ecx, [esi + 0x218]
  41d1a7: e8 84 7b fe ff               	call	0x404d30 <.text+0x3d30>
  41d1ac: 68 dc 99 46 00               	push	0x4699dc
  41d1b1: 8d 8e 6c 05 00 00            	lea	ecx, [esi + 0x56c]
  41d1b7: 53                           	push	ebx
  41d1b8: 89 44 24 38                  	mov	dword ptr [esp + 0x38], eax
  41d1bc: 53                           	push	ebx
  41d1bd: 8d 86 68 05 00 00            	lea	eax, [esi + 0x568]
  41d1c3: 89 4c 24 2c                  	mov	dword ptr [esp + 0x2c], ecx
  41d1c7: 6a 79                        	push	0x79
  41d1c9: 68 0c fe ff ff               	push	0xfffffe0c
  41d1ce: 8b ce                        	mov	ecx, esi
  41d1d0: 89 44 24 30                  	mov	dword ptr [esp + 0x30], eax
  41d1d4: e8 07 3f 01 00               	call	0x4310e0 <.text+0x300e0>
  41d1d9: 8b 8e 64 05 00 00            	mov	ecx, dword ptr [esi + 0x564]
  41d1df: 53                           	push	ebx
  41d1e0: e8 0b 15 02 00               	call	0x43e6f0 <.text+0x3d6f0>
  41d1e5: 6a 2c                        	push	0x2c
  41d1e7: e8 73 44 03 00               	call	0x45165f <.text+0x5065f>
  41d1ec: 83 c4 04                     	add	esp, 0x4
  41d1ef: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  41d1f3: 3b c3                        	cmp	eax, ebx
  41d1f5: c6 44 24 3c 01               	mov	byte ptr [esp + 0x3c], 0x1
  41d1fa: 74 0e                        	je	0x41d20a <.text+0x1c20a>
  41d1fc: 8d 54 24 1c                  	lea	edx, [esp + 0x1c]
  41d200: 8b c8                        	mov	ecx, eax
  41d202: 52                           	push	edx
  41d203: e8 48 49 01 00               	call	0x431b50 <.text+0x30b50>
  41d208: eb 02                        	jmp	0x41d20c <.text+0x1c20c>
  41d20a: 33 c0                        	xor	eax, eax
  41d20c: 53                           	push	ebx
  41d20d: 68 f4 93 46 00               	push	0x4693f4
  41d212: 6a 20                        	push	0x20
  41d214: 6a 28                        	push	0x28
  41d216: 8b c8                        	mov	ecx, eax
  41d218: 88 5c 24 4c                  	mov	byte ptr [esp + 0x4c], bl
  41d21c: 89 86 88 05 00 00            	mov	dword ptr [esi + 0x588], eax
  41d222: e8 e9 49 01 00               	call	0x431c10 <.text+0x30c10>
  41d227: 6a 2c                        	push	0x2c
  41d229: e8 31 44 03 00               	call	0x45165f <.text+0x5065f>
  41d22e: 83 c4 04                     	add	esp, 0x4
  41d231: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  41d235: 3b c3                        	cmp	eax, ebx
  41d237: c6 44 24 3c 02               	mov	byte ptr [esp + 0x3c], 0x2
  41d23c: 74 0e                        	je	0x41d24c <.text+0x1c24c>
  41d23e: 8d 4c 24 1c                  	lea	ecx, [esp + 0x1c]
  41d242: 51                           	push	ecx
  41d243: 8b c8                        	mov	ecx, eax
  41d245: e8 06 49 01 00               	call	0x431b50 <.text+0x30b50>
  41d24a: eb 02                        	jmp	0x41d24e <.text+0x1c24e>
  41d24c: 33 c0                        	xor	eax, eax
  41d24e: 53                           	push	ebx
  41d24f: 68 dc 93 46 00               	push	0x4693dc
  41d254: 6a 20                        	push	0x20
  41d256: 6a 0e                        	push	0xe
  41d258: 8b c8                        	mov	ecx, eax
  41d25a: 88 5c 24 4c                  	mov	byte ptr [esp + 0x4c], bl
  41d25e: 89 86 8c 05 00 00            	mov	dword ptr [esi + 0x58c], eax
  41d264: e8 a7 49 01 00               	call	0x431c10 <.text+0x30c10>
  41d269: 6a 2c                        	push	0x2c
  41d26b: e8 ef 43 03 00               	call	0x45165f <.text+0x5065f>
  41d270: 83 c4 04                     	add	esp, 0x4
  41d273: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  41d277: 3b c3                        	cmp	eax, ebx
  41d279: c6 44 24 3c 03               	mov	byte ptr [esp + 0x3c], 0x3
  41d27e: 74 0e                        	je	0x41d28e <.text+0x1c28e>
  41d280: 8d 54 24 1c                  	lea	edx, [esp + 0x1c]
  41d284: 8b c8                        	mov	ecx, eax
  41d286: 52                           	push	edx
  41d287: e8 c4 48 01 00               	call	0x431b50 <.text+0x30b50>
  41d28c: eb 02                        	jmp	0x41d290 <.text+0x1c290>
  41d28e: 33 c0                        	xor	eax, eax
  41d290: 53                           	push	ebx
  41d291: 68 c4 93 46 00               	push	0x4693c4
  41d296: 6a 3e                        	push	0x3e
  41d298: 68 92 00 00 00               	push	0x92
  41d29d: 8b c8                        	mov	ecx, eax
  41d29f: 88 5c 24 4c                  	mov	byte ptr [esp + 0x4c], bl
  41d2a3: 89 86 90 05 00 00            	mov	dword ptr [esi + 0x590], eax
  41d2a9: e8 62 49 01 00               	call	0x431c10 <.text+0x30c10>
  41d2ae: 6a 2c                        	push	0x2c
  41d2b0: e8 aa 43 03 00               	call	0x45165f <.text+0x5065f>
  41d2b5: 83 c4 04                     	add	esp, 0x4
  41d2b8: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  41d2bc: 3b c3                        	cmp	eax, ebx
  41d2be: c6 44 24 3c 04               	mov	byte ptr [esp + 0x3c], 0x4
  41d2c3: 74 0e                        	je	0x41d2d3 <.text+0x1c2d3>
  41d2c5: 8d 4c 24 1c                  	lea	ecx, [esp + 0x1c]
  41d2c9: 51                           	push	ecx
  41d2ca: 8b c8                        	mov	ecx, eax
  41d2cc: e8 7f 48 01 00               	call	0x431b50 <.text+0x30b50>
  41d2d1: eb 02                        	jmp	0x41d2d5 <.text+0x1c2d5>
  41d2d3: 33 c0                        	xor	eax, eax
  41d2d5: 53                           	push	ebx
  41d2d6: 68 ac 93 46 00               	push	0x4693ac
  41d2db: 6a 5b                        	push	0x5b
  41d2dd: 68 91 00 00 00               	push	0x91
  41d2e2: 8b c8                        	mov	ecx, eax
  41d2e4: 88 5c 24 4c                  	mov	byte ptr [esp + 0x4c], bl
  41d2e8: 89 86 94 05 00 00            	mov	dword ptr [esi + 0x594], eax
  41d2ee: e8 1d 49 01 00               	call	0x431c10 <.text+0x30c10>
  41d2f3: 33 ff                        	xor	edi, edi
  41d2f5: c7 44 24 10 10 00 00 00      	mov	dword ptr [esp + 0x10], 0x10
  41d2fd: 8d ae 98 05 00 00            	lea	ebp, [esi + 0x598]
  41d303: 6a 2c                        	push	0x2c
  41d305: e8 55 43 03 00               	call	0x45165f <.text+0x5065f>
  41d30a: 83 c4 04                     	add	esp, 0x4
  41d30d: 89 44 24 18                  	mov	dword ptr [esp + 0x18], eax
  41d311: 3b c3                        	cmp	eax, ebx
  41d313: c6 44 24 3c 05               	mov	byte ptr [esp + 0x3c], 0x5
  41d318: 74 0e                        	je	0x41d328 <.text+0x1c328>
  41d31a: 8d 54 24 1c                  	lea	edx, [esp + 0x1c]
  41d31e: 8b c8                        	mov	ecx, eax
  41d320: 52                           	push	edx
  41d321: e8 2a 48 01 00               	call	0x431b50 <.text+0x30b50>
  41d326: eb 02                        	jmp	0x41d32a <.text+0x1c32a>
  41d328: 33 c0                        	xor	eax, eax
  41d32a: 8b 4c 24 10                  	mov	ecx, dword ptr [esp + 0x10]
  41d32e: 53                           	push	ebx
  41d32f: 68 c4 99 46 00               	push	0x4699c4
  41d334: 68 f9 00 00 00               	push	0xf9
  41d339: 51                           	push	ecx
  41d33a: 8b c8                        	mov	ecx, eax
  41d33c: 88 5c 24 4c                  	mov	byte ptr [esp + 0x4c], bl
  41d340: 89 45 00                     	mov	dword ptr [ebp], eax
  41d343: e8 c8 48 01 00               	call	0x431c10 <.text+0x30c10>
  41d348: 8b 55 00                     	mov	edx, dword ptr [ebp]
  41d34b: 47                           	inc	edi
  41d34c: 57                           	push	edi
  41d34d: 8b 4a 1c                     	mov	ecx, dword ptr [edx + 0x1c]
  41d350: e8 9b 13 02 00               	call	0x43e6f0 <.text+0x3d6f0>
  41d355: 8b 44 24 10                  	mov	eax, dword ptr [esp + 0x10]
  41d359: 83 c5 04                     	add	ebp, 0x4
  41d35c: 83 c0 1e                     	add	eax, 0x1e
  41d35f: 3d e2 00 00 00               	cmp	eax, 0xe2
  41d364: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  41d368: 7c 99                        	jl	0x41d303 <.text+0x1c303>
  41d36a: 8d 47 01                     	lea	eax, [edi + 0x1]
  41d36d: bd 10 00 00 00               	mov	ebp, 0x10
  41d372: 89 44 24 10                  	mov	dword ptr [esp + 0x10], eax
  41d376: 8d bc be 98 05 00 00         	lea	edi, [esi + 4*edi + 0x598]
  41d37d: 6a 2c                        	push	0x2c
  41d37f: e8 db 42 03 00               	call	0x45165f <.text+0x5065f>
  41d384: 83 c4 04                     	add	esp, 0x4
  41d387: 89 44 24 18                  	mov	dword ptr [esp + 0x18], eax
  41d38b: 3b c3                        	cmp	eax, ebx
  41d38d: c6 44 24 3c 06               	mov	byte ptr [esp + 0x3c], 0x6
  41d392: 74 0e                        	je	0x41d3a2 <.text+0x1c3a2>
  41d394: 8d 4c 24 1c                  	lea	ecx, [esp + 0x1c]
  41d398: 51                           	push	ecx
  41d399: 8b c8                        	mov	ecx, eax
  41d39b: e8 b0 47 01 00               	call	0x431b50 <.text+0x30b50>
  41d3a0: eb 02                        	jmp	0x41d3a4 <.text+0x1c3a4>
  41d3a2: 33 c0                        	xor	eax, eax
  41d3a4: 53                           	push	ebx
  41d3a5: 68 c4 99 46 00               	push	0x4699c4
  41d3aa: 68 16 01 00 00               	push	0x116
  41d3af: 55                           	push	ebp
  41d3b0: 8b c8                        	mov	ecx, eax
  41d3b2: 88 5c 24 4c                  	mov	byte ptr [esp + 0x4c], bl
  41d3b6: 89 07                        	mov	dword ptr [edi], eax
  41d3b8: e8 53 48 01 00               	call	0x431c10 <.text+0x30c10>
  41d3bd: 8b 07                        	mov	eax, dword ptr [edi]
  41d3bf: 8b 54 24 10                  	mov	edx, dword ptr [esp + 0x10]
  41d3c3: 52                           	push	edx
  41d3c4: 8b 48 1c                     	mov	ecx, dword ptr [eax + 0x1c]
  41d3c7: e8 24 13 02 00               	call	0x43e6f0 <.text+0x3d6f0>
  41d3cc: 8b 54 24 10                  	mov	edx, dword ptr [esp + 0x10]
  41d3d0: 83 c7 04                     	add	edi, 0x4
  41d3d3: 83 c5 1e                     	add	ebp, 0x1e
  41d3d6: 42                           	inc	edx
  41d3d7: 81 fd e2 00 00 00            	cmp	ebp, 0xe2
  41d3dd: 89 54 24 10                  	mov	dword ptr [esp + 0x10], edx
  41d3e1: 7c 9a                        	jl	0x41d37d <.text+0x1c37d>
  41d3e3: 6a 54                        	push	0x54
  41d3e5: e8 75 42 03 00               	call	0x45165f <.text+0x5065f>
  41d3ea: 83 c4 04                     	add	esp, 0x4
  41d3ed: 89 44 24 18                  	mov	dword ptr [esp + 0x18], eax
  41d3f1: 3b c3                        	cmp	eax, ebx
  41d3f3: c6 44 24 3c 07               	mov	byte ptr [esp + 0x3c], 0x7
  41d3f8: 74 0e                        	je	0x41d408 <.text+0x1c408>
  41d3fa: 8d 4c 24 1c                  	lea	ecx, [esp + 0x1c]
  41d3fe: 51                           	push	ecx
  41d3ff: 8b c8                        	mov	ecx, eax
  41d401: e8 0a 41 01 00               	call	0x431510 <.text+0x30510>
  41d406: eb 02                        	jmp	0x41d40a <.text+0x1c40a>
  41d408: 33 c0                        	xor	eax, eax
  41d40a: 53                           	push	ebx
  41d40b: 68 94 93 46 00               	push	0x469394
