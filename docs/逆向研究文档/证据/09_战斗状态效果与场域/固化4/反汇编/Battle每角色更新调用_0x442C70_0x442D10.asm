
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442c70 <.text+0x41c70>:
  442c70:	c7 85 e4 02 00 00 b9 	mov    DWORD PTR [ebp+0x2e4],0xbb9
  442c77:	0b 00 00 
  442c7a:	8d bd e4 03 00 00    	lea    edi,[ebp+0x3e4]
  442c80:	bb 10 00 00 00       	mov    ebx,0x10
  442c85:	8b cf                	mov    ecx,edi
  442c87:	e8 04 0a fe ff       	call   0x423690
  442c8c:	81 c7 e4 0d 00 00    	add    edi,0xde4
  442c92:	4b                   	dec    ebx
  442c93:	75 f0                	jne    0x442c85
  442c95:	83 bd 24 e2 00 00 ff 	cmp    DWORD PTR [ebp+0xe224],0xffffffff
  442c9c:	0f 85 1c 01 00 00    	jne    0x442dbe
  442ca2:	a0 dc 96 46 00       	mov    al,ds:0x4696dc
  442ca7:	84 c0                	test   al,al
  442ca9:	0f 84 0f 01 00 00    	je     0x442dbe
  442caf:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  442cb4:	e8 f7 ad ff ff       	call   0x43dab0
  442cb9:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  442cbe:	e8 ed ad ff ff       	call   0x43dab0
  442cc3:	33 ff                	xor    edi,edi
  442cc5:	8b 86 48 fa ff ff    	mov    eax,DWORD PTR [esi-0x5b8]
  442ccb:	85 c0                	test   eax,eax
  442ccd:	0f 8c db 00 00 00    	jl     0x442dae
  442cd3:	8d ae 1c f3 ff ff    	lea    ebp,[esi-0xce4]
  442cd9:	8b cd                	mov    ecx,ebp
  442cdb:	e8 60 d7 fd ff       	call   0x420440
  442ce0:	8d 5e c4             	lea    ebx,[esi-0x3c]
  442ce3:	8b cb                	mov    ecx,ebx
  442ce5:	e8 f6 10 fe ff       	call   0x423de0
  442cea:	83 e8 02             	sub    eax,0x2
  442ced:	0f 84 8e 00 00 00    	je     0x442d81
  442cf3:	83 e8 02             	sub    eax,0x2
  442cf6:	74 67                	je     0x442d5f
  442cf8:	83 e8 02             	sub    eax,0x2
  442cfb:	0f 85 a9 00 00 00    	jne    0x442daa
  442d01:	81 7e fc d6 07 00 00 	cmp    DWORD PTR [esi-0x4],0x7d6
  442d08:	0f 85 9c 00 00 00    	jne    0x442daa
  442d0e:	8b 06                	mov    eax,DWORD PTR [esi]
