; Oracle: /mnt/data/RPG.exe
; SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf  /mnt/data/RPG.exe
; objdump -D -Mintel --start-address=0x434e40 --stop-address=0x434ea2 RPG.exe

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00434e40 <.text+0x33e40>:
  434e40:	80 79 05 00          	cmp    BYTE PTR [ecx+0x5],0x0
  434e44:	00 00                	add    BYTE PTR [eax],al
  434e46:	e8 14 c8 01 00       	call   0x45165f
  434e4b:	83 c4 04             	add    esp,0x4
  434e4e:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  434e52:	85 c0                	test   eax,eax
  434e54:	c6 44 24 3c 10       	mov    BYTE PTR [esp+0x3c],0x10
  434e59:	74 09                	je     0x434e64
  434e5b:	8b c8                	mov    ecx,eax
  434e5d:	e8 7e a9 00 00       	call   0x43f7e0
  434e62:	eb 02                	jmp    0x434e66
  434e64:	33 c0                	xor    eax,eax
  434e66:	89 86 50 06 00 00    	mov    DWORD PTR [esi+0x650],eax
  434e6c:	68 cc 05 00 00       	push   0x5cc
  434e71:	88 5c 24 40          	mov    BYTE PTR [esp+0x40],bl
  434e75:	c6 80 79 05 00 00 00 	mov    BYTE PTR [eax+0x579],0x0
  434e7c:	e8 de c7 01 00       	call   0x45165f
  434e81:	83 c4 04             	add    esp,0x4
  434e84:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  434e88:	85 c0                	test   eax,eax
  434e8a:	c6 44 24 3c 11       	mov    BYTE PTR [esp+0x3c],0x11
  434e8f:	74 09                	je     0x434e9a
  434e91:	8b c8                	mov    ecx,eax
  434e93:	e8 18 f7 fe ff       	call   0x4245b0
  434e98:	eb 02                	jmp    0x434e9c
  434e9a:	33 c0                	xor    eax,eax
  434e9c:	89 86 54 06 00 00    	mov    DWORD PTR [esi+0x654],eax
