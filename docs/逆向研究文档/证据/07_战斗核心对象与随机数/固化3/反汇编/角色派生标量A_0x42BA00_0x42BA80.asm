
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042ba00 <.text+0x2aa00>:
  42ba00:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  42ba04:	56                   	push   esi
  42ba05:	8b 35 c8 01 8b 00    	mov    esi,DWORD PTR ds:0x8b01c8
  42ba0b:	57                   	push   edi
  42ba0c:	8b 8a 90 00 00 00    	mov    ecx,DWORD PTR [edx+0x90]
  42ba12:	8b 42 3c             	mov    eax,DWORD PTR [edx+0x3c]
  42ba15:	85 c9                	test   ecx,ecx
  42ba17:	7e 15                	jle    0x42ba2e
  42ba19:	8d 3c cd 00 00 00 00 	lea    edi,[ecx*8+0x0]
  42ba20:	2b f9                	sub    edi,ecx
  42ba22:	8d 3c b9             	lea    edi,[ecx+edi*4]
  42ba25:	c1 e7 04             	shl    edi,0x4
  42ba28:	2b f9                	sub    edi,ecx
  42ba2a:	03 44 7e 38          	add    eax,DWORD PTR [esi+edi*2+0x38]
  42ba2e:	8b 8a 94 00 00 00    	mov    ecx,DWORD PTR [edx+0x94]
  42ba34:	85 c9                	test   ecx,ecx
  42ba36:	7e 15                	jle    0x42ba4d
  42ba38:	8d 3c cd 00 00 00 00 	lea    edi,[ecx*8+0x0]
  42ba3f:	2b f9                	sub    edi,ecx
  42ba41:	8d 3c b9             	lea    edi,[ecx+edi*4]
  42ba44:	c1 e7 04             	shl    edi,0x4
  42ba47:	2b f9                	sub    edi,ecx
  42ba49:	03 44 7e 38          	add    eax,DWORD PTR [esi+edi*2+0x38]
  42ba4d:	8b 8a 98 00 00 00    	mov    ecx,DWORD PTR [edx+0x98]
  42ba53:	85 c9                	test   ecx,ecx
  42ba55:	7e 15                	jle    0x42ba6c
  42ba57:	8d 14 cd 00 00 00 00 	lea    edx,[ecx*8+0x0]
  42ba5e:	2b d1                	sub    edx,ecx
  42ba60:	8d 14 91             	lea    edx,[ecx+edx*4]
  42ba63:	c1 e2 04             	shl    edx,0x4
  42ba66:	2b d1                	sub    edx,ecx
  42ba68:	03 44 56 38          	add    eax,DWORD PTR [esi+edx*2+0x38]
  42ba6c:	5f                   	pop    edi
  42ba6d:	5e                   	pop    esi
  42ba6e:	c2 04 00             	ret    0x4
  42ba71:	90                   	nop
  42ba72:	90                   	nop
  42ba73:	90                   	nop
  42ba74:	90                   	nop
  42ba75:	90                   	nop
  42ba76:	90                   	nop
  42ba77:	90                   	nop
  42ba78:	90                   	nop
  42ba79:	90                   	nop
  42ba7a:	90                   	nop
  42ba7b:	90                   	nop
  42ba7c:	90                   	nop
  42ba7d:	90                   	nop
  42ba7e:	90                   	nop
  42ba7f:	90                   	nop
  42ba80:	56                   	push   esi
  42ba81:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  42ba85:	56                   	push   esi
  42ba86:	e8 75 ff ff ff       	call   0x42ba00
  42ba8b:	8b 8e f4 01 00 00    	mov    ecx,DWORD PTR [esi+0x1f4]
  42ba91:	85 c9                	test   ecx,ecx
  42ba93:	7e 0b                	jle    0x42baa0
  42ba95:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  42ba9c:	2b c8                	sub    ecx,eax
  42ba9e:	eb 10                	jmp    0x42bab0
  42baa0:	8b 8e 40 02 00 00    	mov    ecx,DWORD PTR [esi+0x240]
  42baa6:	85 c9                	test   ecx,ecx
  42baa8:	7e 19                	jle    0x42bac3
  42baaa:	8d 0c 40             	lea    ecx,[eax+eax*2]
  42baad:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  42bab0:	b8 67 66 66 66       	mov    eax,0x66666667
  42bab5:	f7 e9                	imul   ecx
  42bab7:	c1 fa 02             	sar    edx,0x2
  42baba:	8b c2                	mov    eax,edx
  42babc:	c1 e8 1f             	shr    eax,0x1f
  42babf:	03 d0                	add    edx,eax
  42bac1:	8b c2                	mov    eax,edx
  42bac3:	5e                   	pop    esi
  42bac4:	c2                   	.byte 0xc2
