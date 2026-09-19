
/mnt/data/solid64_inputs/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442cc3 <.text+0x41cc3>:
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
  442d10:	3d c6 0b 00 00       	cmp    eax,0xbc6
  442d15:	0f 84 8f 00 00 00    	je     0x442daa
  442d1b:	3d c0 0b 00 00       	cmp    eax,0xbc0
  442d20:	0f 84 84 00 00 00    	je     0x442daa
  442d26:	57                   	push   edi
  442d27:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  442d2c:	e8 df ae ff ff       	call   0x43dc10
  442d31:	83 f8 ff             	cmp    eax,0xffffffff
  442d34:	75 74                	jne    0x442daa
  442d36:	57                   	push   edi
  442d37:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  442d3c:	e8 cf ae ff ff       	call   0x43dc10
  442d41:	83 f8 ff             	cmp    eax,0xffffffff
  442d44:	75 64                	jne    0x442daa
  442d46:	6a 01                	push   0x1
  442d48:	8b cb                	mov    ecx,ebx
  442d4a:	e8 31 12 fe ff       	call   0x423f80
  442d4f:	8b cd                	mov    ecx,ebp
  442d51:	c7 46 fc d1 07 00 00 	mov    DWORD PTR [esi-0x4],0x7d1
  442d58:	e8 13 07 fe ff       	call   0x423470
  442d5d:	eb                   	.byte 0xeb
