
/mnt/data/work54/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b9f0 <.text+0x2a9f0>:
  42b9f0:	85 c0                	test   eax,eax
  42b9f2:	b8 e8 03 00 00       	mov    eax,0x3e8
  42b9f7:	7f 02                	jg     0x42b9fb
  42b9f9:	8b c1                	mov    eax,ecx
  42b9fb:	c2 04 00             	ret    0x4
  42b9fe:	90                   	nop
  42b9ff:	90                   	nop
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
  42bac4:	c2 04 00             	ret    0x4
  42bac7:	90                   	nop
  42bac8:	90                   	nop
  42bac9:	90                   	nop
  42baca:	90                   	nop
  42bacb:	90                   	nop
  42bacc:	90                   	nop
  42bacd:	90                   	nop
  42bace:	90                   	nop
  42bacf:	90                   	nop
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
  42bbbc:	c2 04 00             	ret    0x4
  42bbbf:	90                   	nop
  42bbc0:	8b 15 c8 01 8b 00    	mov    edx,DWORD PTR ds:0x8b01c8
  42bbc6:	56                   	push   esi
  42bbc7:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  42bbcb:	57                   	push   edi
  42bbcc:	8b 86 90 00      	mov    eax,DWORD PTR [esi+0x90]
