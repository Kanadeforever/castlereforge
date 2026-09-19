
/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0044283e <.text+0x4183e>:
  44283e:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  442843:	89 35 f8 fd 89 00    	mov    DWORD PTR ds:0x89fdf8,esi
  442849:	e8 e2 b1 ff ff       	call   0x43da30
  44284e:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  442853:	c7 05 c8 ff 89 00 01 	mov    DWORD PTR ds:0x89ffc8,0x1
  44285a:	00 00 00 
  44285d:	e8 ce b1 ff ff       	call   0x43da30
  442862:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  442866:	89 35 90 01 8a 00    	mov    DWORD PTR ds:0x8a0190,esi
  44286c:	c7                   	.byte 0xc7
  44286d:	84 24 88             	test   BYTE PTR [eax+ecx*4],ah
  442870:	00 00                	add    BYTE PTR [eax],al
  442872:	00 ff                	add    bh,bh
  442874:	ff                   	.byte 0xff
