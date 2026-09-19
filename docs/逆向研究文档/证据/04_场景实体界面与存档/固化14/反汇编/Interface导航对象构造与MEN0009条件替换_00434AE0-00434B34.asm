
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00434ae0 <.text+0x33ae0>:
  434ae0:	e8 7a cb 01 00       	call   0x45165f
  434ae5:	83 c4 04             	add    esp,0x4
  434ae8:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  434aec:	85 c0                	test   eax,eax
  434aee:	c6 44 24 3c 04       	mov    BYTE PTR [esp+0x3c],0x4
  434af3:	74 09                	je     0x434afe
  434af5:	8b c8                	mov    ecx,eax
  434af7:	e8 24 24 00 00       	call   0x436f20
  434afc:	eb 02                	jmp    0x434b00
  434afe:	33 c0                	xor    eax,eax
  434b00:	89 86 20 06 00 00    	mov    DWORD PTR [esi+0x620],eax
  434b06:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  434b0c:	88 5c 24 3c          	mov    BYTE PTR [esp+0x3c],bl
  434b10:	8b 91 08 01 00 00    	mov    edx,DWORD PTR [ecx+0x108]
  434b16:	85 d2                	test   edx,edx
  434b18:	74 14                	je     0x434b2e
  434b1a:	68 84 b0 46 00       	push   0x46b084
  434b1f:	6a 00                	push   0x0
  434b21:	6a 00                	push   0x0
  434b23:	6a 69                	push   0x69
  434b25:	6a 41                	push   0x41
  434b27:	8b c8                	mov    ecx,eax
  434b29:	e8 b2 c5 ff ff       	call   0x4310e0
  434b2e:	6a 2c                	push   0x2c
  434b30:	e8 2a cb 01        	call   0x45165f
