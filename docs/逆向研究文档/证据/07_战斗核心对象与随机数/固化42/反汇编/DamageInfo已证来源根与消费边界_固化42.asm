; 固化42证据：canonical DamageInfo来源根与各自消费边界

/mnt/data/work42/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041fee0 <.text+0x1eee0>:
  41fee0:	00 00                	add    BYTE PTR [eax],al
  41fee2:	32 db                	xor    bl,bl
  41fee4:	33 c0                	xor    eax,eax
  41fee6:	85 ed                	test   ebp,ebp
  41fee8:	7e 7a                	jle    0x41ff64
  41feea:	8d be 70 01 00 00    	lea    edi,[esi+0x170]
  41fef0:	8d 96 88 00 00 00    	lea    edx,[esi+0x88]
  41fef6:	84 db                	test   bl,bl
  41fef8:	75 2c                	jne    0x41ff26
  41fefa:	8b 0a                	mov    ecx,DWORD PTR [edx]
  41fefc:	85 c9                	test   ecx,ecx
  41fefe:	7c 17                	jl     0x41ff17
  41ff00:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  41ff07:	83 b9 2c 07 00 00 00 	cmp    DWORD PTR [ecx+0x72c],0x0
  41ff0e:	7c 07                	jl     0x41ff17
  41ff10:	80 3f 00             	cmp    BYTE PTR [edi],0x0
  41ff13:	74 02                	je     0x41ff17
  41ff15:	b3 01                	mov    bl,0x1
  41ff17:	40                   	inc    eax
  41ff18:	83 c2 04             	add    edx,0x4
  41ff1b:	83 c7 50             	add    edi,0x50
  41ff1e:	3b c5                	cmp    eax,ebp
  41ff20:	7c d4                	jl     0x41fef6
  41ff22:	84 db                	test   bl,bl
  41ff24:	74 3e                	je     0x41ff64
  41ff26:	8b 96 9c 07 00 00    	mov    edx,DWORD PTR [esi+0x79c]
  41ff2c:	8b 0d c8 40      	mov    ecx,DWORD PTR ds:0x8c40c8


/mnt/data/work42/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420fb8 <.text+0x1ffb8>:
  420fb8:	8e 48 01             	mov    cs,WORD PTR [eax+0x1]
  420fbb:	00 00                	add    BYTE PTR [eax],al
  420fbd:	8d 97 88 00 00 00    	lea    edx,[edi+0x88]
  420fc3:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  420fc7:	8d 9f 78 01 00 00    	lea    ebx,[edi+0x178]
  420fcd:	89 54 24 24          	mov    DWORD PTR [esp+0x24],edx
  420fd1:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  420fd5:	8b 30                	mov    esi,DWORD PTR [eax]
  420fd7:	3b f5                	cmp    esi,ebp
  420fd9:	0f 8c 01 01 00 00    	jl     0x4210e0
  420fdf:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  420fe6:	c7 81 e8 0c 00 00 a0 	mov    DWORD PTR [ecx+0xce8],0xfa0
  420fed:	0f 00 00 
  420ff0:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  420ff7:	c7 82 e0 0d 00 00 ff 	mov    DWORD PTR [edx+0xde0],0xffffffff
  420ffe:	ff ff ff 
  421001:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421008:	8b 81 7c 08 00 00    	mov    eax,DWORD PTR [ecx+0x87c]
  42100e:	3b c5                	cmp    eax,ebp
  421010:	7e 18                	jle    0x42102a
  421012:	2b 43 fc             	sub    eax,DWORD PTR [ebx-0x4]
  421015:	89 81 7c 08 00 00    	mov    DWORD PTR [ecx+0x87c],eax
  42101b:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421022:	8b 0b                	mov    ecx,DWORD PTR [ebx]
  421024:	29 88 84 08 00 00    	sub    DWORD PTR [eax+0x884],ecx
  42102a:	8b 0c b5 94 fd 89  	mov    ecx,DWORD PTR [esi*4+0x89fd94]


/mnt/data/work42/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

004212f8 <.text+0x202f8>:
  4212f8:	86 03                	xchg   BYTE PTR [ebx],al
  4212fa:	00 00                	add    BYTE PTR [eax],al
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
  421374:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  42137b:	39 a8 80 0a 00     	cmp    DWORD PTR [eax+0xa80],ebp


/mnt/data/work42/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423728 <.text+0x22728>:
  423728:	c1 02 00             	rol    DWORD PTR [edx],0x0
  42372b:	00 8d be 88 00 00    	add    BYTE PTR [ebp+0x88be],cl
  423731:	00 8d 9e 70 01 00    	add    BYTE PTR [ebp+0x1709e],cl
  423737:	00 8b 0f 8b 0c 8d    	add    BYTE PTR [ebx-0x72f374f1],cl
  42373d:	94                   	xchg   esp,eax
  42373e:	fd                   	std
  42373f:	89 00                	mov    DWORD PTR [eax],eax
  423741:	8b 81 e4 0c 00 00    	mov    eax,DWORD PTR [ecx+0xce4]
  423747:	3d c6 0b 00 00       	cmp    eax,0xbc6
  42374c:	75 08                	jne    0x423756
  42374e:	6a 00                	push   0x0
  423750:	6a 00                	push   0x0
  423752:	6a 72                	push   0x72
  423754:	eb 1a                	jmp    0x423770
  423756:	3d be 0b 00 00       	cmp    eax,0xbbe
  42375b:	74 0d                	je     0x42376a
  42375d:	80 3b 00             	cmp    BYTE PTR [ebx],0x0
  423760:	74 08                	je     0x42376a
  423762:	6a 00                	push   0x0
  423764:	6a 00                	push   0x0
  423766:	6a 6c                	push   0x6c
  423768:	eb 06                	jmp    0x423770
  42376a:	6a 00                	push   0x0
  42376c:	6a 00                	push   0x0
  42376e:	6a 6d                	push   0x6d
  423770:	e8 ab e9 ff ff       	call   0x422120
  423775:	8b 86 c8 00 00 00    	mov    eax,DWORD PTR [esi+0xc8]
  42377b:	45                   	inc    ebp
  42377c:	83 c7 04             	add    edi,0x4
  42377f:	83 c3 50             	add    ebx,0x50
  423782:	3b e8                	cmp    ebp,eax
  423784:	7c b2                	jl     0x423738
  423786:	33 ff                	xor    edi,edi
  423788:	89 be 24 07 00 00    	mov    DWORD PTR [esi+0x724],edi
  42378e:	5f                   	pop    edi
  42378f:	5e                   	pop    esi


/mnt/data/work42/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423800 <.text+0x22800>:
  423800:	c7                   	(bad)
  423801:	0f 8e e6 01 00 00    	jle    0x4239ed
  423807:	8d 8e 88 00 00 00    	lea    ecx,[esi+0x88]
  42380d:	8d be 70 01 00 00    	lea    edi,[esi+0x170]
  423813:	80 3f 00             	cmp    BYTE PTR [edi],0x0
  423816:	74 32                	je     0x42384a
  423818:	8b 01                	mov    eax,DWORD PTR [ecx]
  42381a:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  423821:	8b 80 a4 0b 00 00    	mov    eax,DWORD PTR [eax+0xba4]
  423827:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  42382a:	c7 80 80 00 00 00 1e 	mov    DWORD PTR [eax+0x80],0x1e
  423831:	00 00 00 
  423834:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  42383a:	89 a8 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebp
  423840:	c7 80 8c 00 00 00 00 	mov    DWORD PTR [eax+0x8c],0x0
  423847:	00 00 00 
  42384a:	8b 86 c8 00 00 00    	mov    eax,DWORD PTR [esi+0xc8]
  423850:	42                   	inc    edx
  423851:	83 c1 04             	add    ecx,0x4
  423854:	83 c7 50             	add    edi,0x50
  423857:	3b d0                	cmp    edx,eax
  423859:	7c b8                	jl     0x423813
  42385b:	33 ff                	xor    edi,edi
  42385d:	89 be 24 07 00 00    	mov    DWORD PTR [esi+0x724],edi
  423863:	5f                   	pop    edi
  423864:	5e                   	pop    esi
  423865:	5d                   	pop    ebp
  423866:	5b                   	pop    ebx
  423867:	c3                   	ret

