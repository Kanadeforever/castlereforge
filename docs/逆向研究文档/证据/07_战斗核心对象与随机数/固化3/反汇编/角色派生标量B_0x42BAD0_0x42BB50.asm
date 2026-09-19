
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042bad0 <.text+0x2aad0>:
  42bad0:	56                   	push   esi
  42bad1:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  42bad5:	57                   	push   edi
  42bad6:	8b 46 40             	mov    eax,DWORD PTR [esi+0x40]
  42bad9:	8b 8e 90 00 00 00    	mov    ecx,DWORD PTR [esi+0x90]
  42badf:	99                   	cdq
  42bae0:	2b c2                	sub    eax,edx
  42bae2:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42bae8:	d1 f8                	sar    eax,1
  42baea:	85 c9                	test   ecx,ecx
  42baec:	7e 15                	jle    0x42bb03
  42baee:	8d 3c cd 00 00 00 00 	lea    edi,[ecx*8+0x0]
  42baf5:	2b f9                	sub    edi,ecx
  42baf7:	8d 3c b9             	lea    edi,[ecx+edi*4]
  42bafa:	c1 e7 04             	shl    edi,0x4
  42bafd:	2b f9                	sub    edi,ecx
  42baff:	03 44 7a 3c          	add    eax,DWORD PTR [edx+edi*2+0x3c]
  42bb03:	8b 8e 94 00 00 00    	mov    ecx,DWORD PTR [esi+0x94]
  42bb09:	85 c9                	test   ecx,ecx
  42bb0b:	7e 15                	jle    0x42bb22
  42bb0d:	8d 3c cd 00 00 00 00 	lea    edi,[ecx*8+0x0]
  42bb14:	2b f9                	sub    edi,ecx
  42bb16:	8d 3c b9             	lea    edi,[ecx+edi*4]
  42bb19:	c1 e7 04             	shl    edi,0x4
  42bb1c:	2b f9                	sub    edi,ecx
  42bb1e:	03 44 7a 3c          	add    eax,DWORD PTR [edx+edi*2+0x3c]
  42bb22:	8b 8e 98 00 00 00    	mov    ecx,DWORD PTR [esi+0x98]
  42bb28:	85 c9                	test   ecx,ecx
  42bb2a:	7e 15                	jle    0x42bb41
  42bb2c:	8d 34 cd 00 00 00 00 	lea    esi,[ecx*8+0x0]
  42bb33:	2b f1                	sub    esi,ecx
  42bb35:	8d 34 b1             	lea    esi,[ecx+esi*4]
  42bb38:	c1 e6 04             	shl    esi,0x4
  42bb3b:	2b f1                	sub    esi,ecx
  42bb3d:	03 44 72 3c          	add    eax,DWORD PTR [edx+esi*2+0x3c]
  42bb41:	5f                   	pop    edi
  42bb42:	5e                   	pop    esi
  42bb43:	c2 04 00             	ret    0x4
  42bb46:	90                   	nop
  42bb47:	90                   	nop
  42bb48:	90                   	nop
  42bb49:	90                   	nop
  42bb4a:	90                   	nop
  42bb4b:	90                   	nop
  42bb4c:	90                   	nop
  42bb4d:	90                   	nop
  42bb4e:	90                   	nop
  42bb4f:	90                   	nop
  42bb50:	56                   	push   esi
  42bb51:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  42bb55:	56                   	push   esi
  42bb56:	e8 75 ff ff ff       	call   0x42bad0
  42bb5b:	8b 8e 24 02 00 00    	mov    ecx,DWORD PTR [esi+0x224]
  42bb61:	85 c9                	test   ecx,ecx
  42bb63:	7e 09                	jle    0x42bb6e
  42bb65:	99                   	cdq
  42bb66:	2b c2                	sub    eax,edx
  42bb68:	5e                   	pop    esi
  42bb69:	d1 f8                	sar    eax,1
  42bb6b:	c2 04 00             	ret    0x4
  42bb6e:	8b 8e 4c 02 00 00    	mov    ecx,DWORD PTR [esi+0x24c]
  42bb74:	85 c9                	test   ecx,ecx
  42bb76:	7e 1d                	jle    0x42bb95
  42bb78:	8d 0c 40             	lea    ecx,[eax+eax*2]
  42bb7b:	5e                   	pop    esi
  42bb7c:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  42bb7f:	b8 67 66 66 66       	mov    eax,0x66666667
  42bb84:	f7 e9                	imul   ecx
  42bb86:	c1 fa 02             	sar    edx,0x2
  42bb89:	8b c2                	mov    eax,edx
  42bb8b:	c1 e8 1f             	shr    eax,0x1f
  42bb8e:	03 d0                	add    edx,eax
  42bb90:	8b c2                	mov    eax,edx
  42bb92:	c2 04 00             	ret    0x4
  42bb95:	8b 8e 00 02 00 00    	mov    ecx,DWORD PTR [esi+0x200]
  42bb9b:	85 c9                	test   ecx,ecx
  42bb9d:	7e 1c                	jle    0x42bbbb
  42bb9f:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  42bba6:	2b c8                	sub    ecx,eax
  42bba8:	b8 67 66 66 66       	mov    eax,0x66666667
  42bbad:	f7 e9                	imul   ecx
  42bbaf:	c1 fa 02             	sar    edx,0x2
  42bbb2:	8b ca                	mov    ecx,edx
  42bbb4:	c1 e9 1f             	shr    ecx,0x1f
  42bbb7:	03 d1                	add    edx,ecx
  42bbb9:	8b c2                	mov    eax,edx
  42bbbb:	5e                   	pop    esi
  42bbbc:	c2                   	.byte 0xc2
