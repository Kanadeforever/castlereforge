
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0043edc0 <.text+0x3ddc0>:
  43edc0:	56                   	push   esi
  43edc1:	8b f1                	mov    esi,ecx
  43edc3:	57                   	push   edi
  43edc4:	8a 46 08             	mov    al,BYTE PTR [esi+0x8]
  43edc7:	84 c0                	test   al,al
  43edc9:	0f 84 ad 00 00 00    	je     0x43ee7c
  43edcf:	a0 80 1c 8e 00       	mov    al,ds:0x8e1c80
  43edd4:	84 c0                	test   al,al
  43edd6:	0f 84 a0 00 00 00    	je     0x43ee7c
  43eddc:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  43ede0:	8b 46 18             	mov    eax,DWORD PTR [esi+0x18]
  43ede3:	3b f8                	cmp    edi,eax
  43ede5:	0f 8f 91 00 00 00    	jg     0x43ee7c
  43edeb:	8a 86 58 03 00 00    	mov    al,BYTE PTR [esi+0x358]
  43edf1:	84 c0                	test   al,al
  43edf3:	0f 84 83 00 00 00    	je     0x43ee7c
  43edf9:	8b 8e 5c 03 00 00    	mov    ecx,DWORD PTR [esi+0x35c]
  43edff:	57                   	push   edi
  43ee00:	e8 6b a0 fe ff       	call   0x428e70
  43ee05:	8a 44 24 2c          	mov    al,BYTE PTR [esp+0x2c]
  43ee09:	84 c0                	test   al,al
  43ee0b:	74 12                	je     0x43ee1f
  43ee0d:	8b                   	.byte 0x8b
  43ee0e:	8e                   	.byte 0x8e
  43ee0f:	5c                   	pop    esp
