; ===== slot3 owner代价 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004211c5 <.text+0x201c5>:
  4211c5:	39 2d 84 fd 89 00    	cmp    DWORD PTR ds:0x89fd84,ebp
  4211cb:	7e 69                	jle    0x421236
  4211cd:	55                   	push   ebp
  4211ce:	6a 03                	push   0x3
  4211d0:	8b cf                	mov    ecx,edi
  4211d2:	e8 d9 1e 00 00       	call   0x4230b0
  4211d7:	8b f0                	mov    esi,eax
  4211d9:	3b f5                	cmp    esi,ebp
  4211db:	7c 59                	jl     0x421236
  4211dd:	8b 1d 84 fd 89 00    	mov    ebx,DWORD PTR ds:0x89fd84
  4211e3:	b8 77 77 77 77       	mov    eax,0x77777777
  4211e8:	f7 eb                	imul   ebx
  4211ea:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  4211f1:	2b d3                	sub    edx,ebx
  4211f3:	c1 fa 03             	sar    edx,0x3
  4211f6:	8b 99 84 08 00 00    	mov    ebx,DWORD PTR [ecx+0x884]
  4211fc:	8b c2                	mov    eax,edx
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
; ===== slot3 结果拦截 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004212ce <.text+0x202ce>:
  4212ce:	83 f8 02             	cmp    eax,0x2
  4212d1:	0f 85 ab 03 00 00    	jne    0x421682
  4212d7:	50                   	push   eax
  4212d8:	8b cf                	mov    ecx,edi
  4212da:	89 2d 84 fd 89 00    	mov    DWORD PTR ds:0x89fd84,ebp
  4212e0:	e8 1b 07 00 00       	call   0x421a00
  4212e5:	6a 02                	push   0x2
  4212e7:	8b cf                	mov    ecx,edi
  4212e9:	e8 a2 03 00 00       	call   0x421690
  4212ee:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  4212f4:	3b c5                	cmp    eax,ebp
  4212f6:	0f 8e 86 03 00 00    	jle    0x421682
  4212fc:	8d 87 88 00 00 00    	lea    eax,[edi+0x88]
  421302:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  421306:	8d 9f 74 01 00 00    	lea    ebx,[edi+0x174]
  42130c:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  421310:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  421314:	8b 31                	mov    esi,DWORD PTR [ecx]
  421316:	3b f5                	cmp    esi,ebp
  421318:	0f 8c 3f 03 00 00    	jl     0x42165d
  42131e:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  421325:	39 aa 2c 07 00 00    	cmp    DWORD PTR [edx+0x72c],ebp
  42132b:	7d 0b                	jge    0x421338
  42132d:	8a 43 3c             	mov    al,BYTE PTR [ebx+0x3c]
  421330:	84 c0                	test   al,al
  421332:	0f 84 25 03 00 00    	je     0x42165d
  421338:	56                   	push   esi
  421339:	6a 03                	push   0x3
  42133b:	8b cf                	mov    ecx,edi
  42133d:	e8 2e 1d 00 00       	call   0x423070
  421342:	84 c0                	test   al,al
  421344:	74 1a                	je     0x421360
  421346:	a1 84 fd 89 00       	mov    eax,ds:0x89fd84
  42134b:	8b 13                	mov    edx,DWORD PTR [ebx]
  42134d:	03 c2                	add    eax,edx
  42134f:	a3 84 fd 89 00       	mov    ds:0x89fd84,eax
  421354:	8b 4b 04             	mov    ecx,DWORD PTR [ebx+0x4]
  421357:	03 c1                	add    eax,ecx
  421359:	a3 84 fd 89 00       	mov    ds:0x89fd84,eax
  42135e:	eb 0f                	jmp    0x42136f
  421360:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421367:	39 a8 6c 0a 00 00    	cmp    DWORD PTR [eax+0xa6c],ebp
  42136d:	7e 05                	jle    0x421374
  42136f:	89 2b                	mov    DWORD PTR [ebx],ebp
  421371:	89 6b 04             	mov    DWORD PTR [ebx+0x4],ebp
