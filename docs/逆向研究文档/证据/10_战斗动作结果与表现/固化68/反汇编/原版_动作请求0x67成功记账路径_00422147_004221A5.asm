
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00422147 <.text+0x21147>:
  422147:	55                   	push   ebp
  422148:	8b 6c 24 10          	mov    ebp,DWORD PTR [esp+0x10]
  42214c:	57                   	push   edi
  42214d:	8d 4d 99             	lea    ecx,[ebp-0x67]
  422150:	83 f9 0d             	cmp    ecx,0xd
  422153:	0f 87 95 01 00 00    	ja     0x4222ee
  422159:	ff 24 8d d8 24 42 00 	jmp    DWORD PTR [ecx*4+0x4224d8]
  422160:	8d 86 d4 0a 00 00    	lea    eax,[esi+0xad4]
  422166:	6a 00                	push   0x0
  422168:	50                   	push   eax
  422169:	8b ce                	mov    ecx,esi
  42216b:	e8 90 d5 ff ff       	call   0x41f700
  422170:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  422176:	50                   	push   eax
  422177:	e8 34 b0 01 00       	call   0x43d1b0
  42217c:	84 c0                	test   al,al
  42217e:	0f 84 6a 01 00 00    	je     0x4222ee
  422184:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  42218a:	6a 00                	push   0x0
  42218c:	8b 49 20             	mov    ecx,DWORD PTR [ecx+0x20]
  42218f:	e8 0c c5 01 00       	call   0x43e6a0
  422194:	b3 01                	mov    bl,0x1
  422196:	89 ae ec 0c 00 00    	mov    DWORD PTR [esi+0xcec],ebp
  42219c:	5f                   	pop    edi
  42219d:	5d                   	pop    ebp
  42219e:	8a c3                	mov    al,bl
  4221a0:	5e                   	pop    esi
  4221a1:	5b                   	pop    ebx
  4221a2:	c2 0c 00             	ret    0xc
