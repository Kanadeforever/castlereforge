; ===== 0x423070..0x4230F3：按角色槽组查询slot与owner包装器 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423070 <.text+0x22070>:
  423070:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  423074:	85 c0                	test   eax,eax
  423076:	7c 17                	jl     0x42308f
  423078:	83 f8 07             	cmp    eax,0x7
  42307b:	7f 12                	jg     0x42308f
  42307d:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  423081:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  423086:	50                   	push   eax
  423087:	e8 c4 a9 01 00       	call   0x43da50
  42308c:	c2 08 00             	ret    0x8
  42308f:	83 f8 08             	cmp    eax,0x8
  423092:	7c 17                	jl     0x4230ab
  423094:	83 f8 0f             	cmp    eax,0xf
  423097:	7f 12                	jg     0x4230ab
  423099:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  42309d:	51                   	push   ecx
  42309e:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4230a3:	e8 a8 a9 01 00       	call   0x43da50
  4230a8:	c2 08 00             	ret    0x8
  4230ab:	32 c0                	xor    al,al
  4230ad:	c2 08 00             	ret    0x8
  4230b0:	8b 01                	mov    eax,DWORD PTR [ecx]
  4230b2:	8a 4c 24 08          	mov    cl,BYTE PTR [esp+0x8]
  4230b6:	85 c0                	test   eax,eax
  4230b8:	7c 09                	jl     0x4230c3
  4230ba:	83 f8 07             	cmp    eax,0x7
  4230bd:	7f 04                	jg     0x4230c3
  4230bf:	84 c9                	test   cl,cl
  4230c1:	74 0e                	je     0x4230d1
  4230c3:	83 f8 08             	cmp    eax,0x8
  4230c6:	7c 1b                	jl     0x4230e3
  4230c8:	83 f8 0f             	cmp    eax,0xf
  4230cb:	7f 16                	jg     0x4230e3
  4230cd:	84 c9                	test   cl,cl
  4230cf:	74 12                	je     0x4230e3
  4230d1:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4230d5:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4230da:	50                   	push   eax
  4230db:	e8 60 ab 01 00       	call   0x43dc40
  4230e0:	c2 08 00             	ret    0x8
  4230e3:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  4230e7:	51                   	push   ecx
  4230e8:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4230ed:	e8 4e ab 01 00       	call   0x43dc40
  4230f2:	c2                   	.byte 0xc2
; ===== 0x42BF40..0x42BF84：公式侧manager selector包装器 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042bf40 <.text+0x2af40>:
  42bf40:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  42bf44:	53                   	push   ebx
  42bf45:	32 db                	xor    bl,bl
  42bf47:	85 c0                	test   eax,eax
  42bf49:	75 1a                	jne    0x42bf65
  42bf4b:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42bf4f:	8b 0d cc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01cc
  42bf55:	50                   	push   eax
  42bf56:	e8 f5 1a 01 00       	call   0x43da50
  42bf5b:	84 c0                	test   al,al
  42bf5d:	74 21                	je     0x42bf80
  42bf5f:	b0 01                	mov    al,0x1
  42bf61:	5b                   	pop    ebx
  42bf62:	c2 08 00             	ret    0x8
  42bf65:	83 f8 01             	cmp    eax,0x1
  42bf68:	75 16                	jne    0x42bf80
  42bf6a:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  42bf6e:	51                   	push   ecx
  42bf6f:	8b 0d d0 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01d0
  42bf75:	e8 d6 1a 01 00       	call   0x43da50
  42bf7a:	84 c0                	test   al,al
  42bf7c:	b0 01                	mov    al,0x1
  42bf7e:	75 02                	jne    0x42bf82
  42bf80:	8a c3                	mov    al,bl
  42bf82:	5b                   	pop    ebx
  42bf83:	c2                   	.byte 0xc2
