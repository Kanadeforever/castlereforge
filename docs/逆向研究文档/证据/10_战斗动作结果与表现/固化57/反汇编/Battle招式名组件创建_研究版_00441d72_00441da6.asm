
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00441d72 <.text+0x40d72>:
  441d72:	68 94 05 00 00       	push   0x594
  441d77:	e8 e3 f8 00 00       	call   0x45165f
  441d7c:	83 c4 04             	add    esp,0x4
  441d7f:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  441d83:	3b c3                	cmp    eax,ebx
  441d85:	c6 44 24 18 05       	mov    BYTE PTR [esp+0x18],0x5
  441d8a:	74 09                	je     0x441d95
  441d8c:	8b c8                	mov    ecx,eax
  441d8e:	e8 7d 53 00 00       	call   0x447110
  441d93:	eb 02                	jmp    0x441d97
  441d95:	33 c0                	xor    eax,eax
  441d97:	a3 18 24 8e 00       	mov    ds:0x8e2418,eax
  441d9c:	c7 86 24 e2 00 00 ff 	mov    DWORD PTR [esi+0xe224],0xffffffff
  441da3:	ff ff ff 
