  43ab30:	51                   	push   ecx
  43ab31:	53                   	push   ebx
  43ab32:	8b 5c 24 0c          	mov    ebx,DWORD PTR [esp+0xc]
  43ab36:	55                   	push   ebp
  43ab37:	8b e9                	mov    ebp,ecx
  43ab39:	8b 83 70 dd 00 00    	mov    eax,DWORD PTR [ebx+0xdd70]
  43ab3f:	56                   	push   esi
  43ab40:	8d 4d 40             	lea    ecx,[ebp+0x40]
  43ab43:	89 45 3c             	mov    DWORD PTR [ebp+0x3c],eax
  43ab46:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  43ab4a:	8d 8b 00 bc 00 00    	lea    ecx,[ebx+0xbc00]
  43ab50:	57                   	push   edi
  43ab51:	33 c0                	xor    eax,eax
  43ab53:	8d 93 20 dd 00 00    	lea    edx,[ebx+0xdd20]
  43ab59:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  43ab5d:	8b bd a4 00 00 00    	mov    edi,DWORD PTR [ebp+0xa4]
  43ab63:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  43ab67:	03 f8                	add    edi,eax
  43ab69:	b9 d4 00 00 00       	mov    ecx,0xd4
  43ab6e:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43ab70:	8b 0a                	mov    ecx,DWORD PTR [edx]
  43ab72:	85 c9                	test   ecx,ecx
  43ab74:	7e 11                	jle    0x43ab87
  43ab76:	83 f9 09             	cmp    ecx,0x9
  43ab79:	7d 0c                	jge    0x43ab87
  43ab7b:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
  43ab7f:	89 4e 28             	mov    DWORD PTR [esi+0x28],ecx
  43ab82:	8b 4a 28             	mov    ecx,DWORD PTR [edx+0x28]
  43ab85:	89 0e                	mov    DWORD PTR [esi],ecx
  43ab87:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  43ab8b:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  43ab8f:	05 50 03 00 00       	add    eax,0x350
  43ab94:	83 c2 04             	add    edx,0x4
  43ab97:	83 c1 04             	add    ecx,0x4
  43ab9a:	81 c6 50 03 00 00    	add    esi,0x350
  43aba0:	3d 80 1a 00 00       	cmp    eax,0x1a80
  43aba5:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  43aba9:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  43abad:	7c ae                	jl     0x43ab5d
  43abaf:	8b bd 98 00 00 00    	mov    edi,DWORD PTR [ebp+0x98]
  43abb5:	b9 7a 17 00 00       	mov    ecx,0x177a
  43abba:	8d 73 2c             	lea    esi,[ebx+0x2c]
  43abbd:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43abbf:	8b bd 9c 00 00 00    	mov    edi,DWORD PTR [ebp+0x9c]
  43abc5:	b9 7a 17 00 00       	mov    ecx,0x177a
  43abca:	8d b3 14 5e 00 00    	lea    esi,[ebx+0x5e14]
  43abd0:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43abd2:	8b 83 fc bb 00 00    	mov    eax,DWORD PTR [ebx+0xbbfc]
  43abd8:	8b cd                	mov    ecx,ebp
  43abda:	50                   	push   eax
  43abdb:	89 85 94 00 00 00    	mov    DWORD PTR [ebp+0x94],eax
  43abe1:	e8 aa 00 00 00       	call   0x43ac90
  43abe6:	8b 83 91 dd 00 00    	mov    eax,DWORD PTR [ebx+0xdd91]
  43abec:	33 c9                	xor    ecx,ecx
  43abee:	3b c1                	cmp    eax,ecx
  43abf0:	7d 06                	jge    0x43abf8
  43abf2:	89 8b 91 dd 00 00    	mov    DWORD PTR [ebx+0xdd91],ecx
  43abf8:	39 8b 95 dd 00 00    	cmp    DWORD PTR [ebx+0xdd95],ecx
  43abfe:	7d 06                	jge    0x43ac06
  43ac00:	89 8b 95 dd 00 00    	mov    DWORD PTR [ebx+0xdd95],ecx
  43ac06:	8b 93 91 dd 00 00    	mov    edx,DWORD PTR [ebx+0xdd91]
  43ac0c:	b8 09 00 00 00       	mov    eax,0x9
  43ac11:	3b d0                	cmp    edx,eax
  43ac13:	7e 06                	jle    0x43ac1b
  43ac15:	89 83 91 dd 00 00    	mov    DWORD PTR [ebx+0xdd91],eax
  43ac1b:	39 83 95 dd 00 00    	cmp    DWORD PTR [ebx+0xdd95],eax
  43ac21:	7e 06                	jle    0x43ac29
  43ac23:	89 83 95 dd 00 00    	mov    DWORD PTR [ebx+0xdd95],eax
  43ac29:	8b 93 91 dd 00 00    	mov    edx,DWORD PTR [ebx+0xdd91]
  43ac2f:	89 95 2c 01 00 00    	mov    DWORD PTR [ebp+0x12c],edx
  43ac35:	8b 83 95 dd 00 00    	mov    eax,DWORD PTR [ebx+0xdd95]
  43ac3b:	89 85 28 01 00 00    	mov    DWORD PTR [ebp+0x128],eax
  43ac41:	8b 83 91 dd 00 00    	mov    eax,DWORD PTR [ebx+0xdd91]
  43ac47:	8d 14 80             	lea    edx,[eax+eax*4]
  43ac4a:	d1 e2                	shl    edx,1
  43ac4c:	52                   	push   edx
  43ac4d:	51                   	push   ecx
  43ac4e:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  43ac54:	e8 a7 a3 fc ff       	call   0x405000
  43ac59:	8b 9b 95 dd 00 00    	mov    ebx,DWORD PTR [ebx+0xdd95]
  43ac5f:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  43ac65:	8d 04 9b             	lea    eax,[ebx+ebx*4]
  43ac68:	d1 e0                	shl    eax,1
  43ac6a:	50                   	push   eax
  43ac6b:	6a 0a                	push   0xa
  43ac6d:	e8 8e a3 fc ff       	call   0x405000
  43ac72:	8b cd                	mov    ecx,ebp
  43ac74:	e8 07 02 00 00       	call   0x43ae80
  43ac79:	5f                   	pop    edi
  43ac7a:	5e                   	pop    esi
  43ac7b:	5d                   	pop    ebp
  43ac7c:	5b                   	pop    ebx
  43ac7d:	59                   	pop    ecx
  43ac7e:	c2 04 00             	ret    0x4
  43ac81:	90                   	nop
  43ac82:	90                   	nop
  43ac83:	90                   	nop
  43ac84:	90                   	nop
  43ac85:	90                   	nop
  43ac86:	90                   	nop
  43ac87:	90                   	nop
  43ac88:	90                   	nop
  43ac89:	90                   	nop
  43ac8a:	90                   	nop
  43ac8b:	90                   	nop
  43ac8c:	90                   	nop
  43ac8d:	90                   	nop
  43ac8e:	90                   	nop
  43ac8f:	90                   	nop
  43ac90:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  43ac94:	33 c0                	xor    eax,eax
  43ac96:	3b d0                	cmp    edx,eax
  43ac98:	75 17                	jne    0x43acb1
  43ac9a:	89 81 94 00 00 00    	mov    DWORD PTR [ecx+0x94],eax
  43aca0:	8b 81 98 00 00 00    	mov    eax,DWORD PTR [ecx+0x98]
  43aca6:	89 81 90 00 00 00    	mov    DWORD PTR [ecx+0x90],eax
  43acac:	b0 01                	mov    al,0x1
  43acae:	c2 04 00             	ret    0x4
  43acb1:	83 fa 01             	cmp    edx,0x1
  43acb4:	75 14                	jne    0x43acca
  43acb6:	89 91 94 00 00 00    	mov    DWORD PTR [ecx+0x94],edx
  43acbc:	8b 91 9c 00 00 00    	mov    edx,DWORD PTR [ecx+0x9c]
  43acc2:	89 91 90 00 00 00    	mov    DWORD PTR [ecx+0x90],edx
  43acc8:	b0 01                	mov    al,0x1
  43acca:	c2 04 00             	ret    0x4
  43accd:	90                   	nop
  43acce:	90                   	nop
  43accf:	90                   	nop
  43acd0:	51                   	push   ecx
  43acd1:	53                   	push   ebx
  43acd2:	8b 5c 24 0c          	mov    ebx,DWORD PTR [esp+0xc]
  43acd6:	55                   	push   ebp
  43acd7:	8b e9                	mov    ebp,ecx
  43acd9:	8d 8b 48 dd 00 00    	lea    ecx,[ebx+0xdd48]
  43acdf:	56                   	push   esi
  43ace0:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  43ace4:	8d 8b 00 bc 00 00    	lea    ecx,[ebx+0xbc00]
  43acea:	57                   	push   edi
  43aceb:	33 c0                	xor    eax,eax
  43aced:	8d 55 68             	lea    edx,[ebp+0x68]
  43acf0:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  43acf4:	8b b5 a4 00 00 00    	mov    esi,DWORD PTR [ebp+0xa4]
  43acfa:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  43acfe:	03 f0                	add    esi,eax
  43ad00:	b9 d4 00 00 00       	mov    ecx,0xd4
  43ad05:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43ad07:	8b 0a                	mov    ecx,DWORD PTR [edx]
  43ad09:	85 c9                	test   ecx,ecx
  43ad0b:	7e 11                	jle    0x43ad1e
  43ad0d:	83 f9 09             	cmp    ecx,0x9
  43ad10:	7d 0c                	jge    0x43ad1e
  43ad12:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
  43ad16:	89 4e d8             	mov    DWORD PTR [esi-0x28],ecx
  43ad19:	8b 4a d8             	mov    ecx,DWORD PTR [edx-0x28]
  43ad1c:	89 0e                	mov    DWORD PTR [esi],ecx
  43ad1e:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  43ad22:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  43ad26:	05 50 03 00 00       	add    eax,0x350
  43ad2b:	83 c2 04             	add    edx,0x4
  43ad2e:	83 c1 04             	add    ecx,0x4
  43ad31:	81 c6 50 03 00 00    	add    esi,0x350
  43ad37:	3d d0 1d 00 00       	cmp    eax,0x1dd0
  43ad3c:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  43ad40:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  43ad44:	7c ae                	jl     0x43acf4
  43ad46:	8b 55 3c             	mov    edx,DWORD PTR [ebp+0x3c]
  43ad49:	b9 7a 17 00 00       	mov    ecx,0x177a
  43ad4e:	89 93 70 dd 00 00    	mov    DWORD PTR [ebx+0xdd70],edx
  43ad54:	8b b5 98 00 00 00    	mov    esi,DWORD PTR [ebp+0x98]
  43ad5a:	8d 7b 2c             	lea    edi,[ebx+0x2c]
  43ad5d:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43ad5f:	8b b5 9c 00 00 00    	mov    esi,DWORD PTR [ebp+0x9c]
  43ad65:	b9 7a 17 00 00       	mov    ecx,0x177a
  43ad6a:	8d bb 14 5e 00 00    	lea    edi,[ebx+0x5e14]
  43ad70:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43ad72:	8b 85 94 00 00 00    	mov    eax,DWORD PTR [ebp+0x94]
  43ad78:	8d bd 4c 01 00 00    	lea    edi,[ebp+0x14c]
  43ad7e:	89 83 fc bb 00 00    	mov    DWORD PTR [ebx+0xbbfc],eax
  43ad84:	83 c9 ff             	or     ecx,0xffffffff
  43ad87:	33 c0                	xor    eax,eax
  43ad89:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  43ad8b:	f7 d1                	not    ecx
  43ad8d:	2b f9                	sub    edi,ecx
  43ad8f:	8b d1                	mov    edx,ecx
  43ad91:	8b f7                	mov    esi,edi
  43ad93:	8b fb                	mov    edi,ebx
  43ad95:	c1 e9 02             	shr    ecx,0x2
  43ad98:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43ad9a:	8b ca                	mov    ecx,edx
  43ad9c:	83 e1 03             	and    ecx,0x3
  43ad9f:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
  43ada1:	8b 85 2c 01 00 00    	mov    eax,DWORD PTR [ebp+0x12c]
  43ada7:	89 83 91 dd 00 00    	mov    DWORD PTR [ebx+0xdd91],eax
  43adad:	8b 8d 28 01 00 00    	mov    ecx,DWORD PTR [ebp+0x128]
  43adb3:	89 8b 95 dd 00 00    	mov    DWORD PTR [ebx+0xdd95],ecx
  43adb9:	8b 8b 91 dd 00 00    	mov    ecx,DWORD PTR [ebx+0xdd91]
  43adbf:	33 c0                	xor    eax,eax
  43adc1:	3b c8                	cmp    ecx,eax
  43adc3:	7d 06                	jge    0x43adcb
  43adc5:	89 83 91 dd 00 00    	mov    DWORD PTR [ebx+0xdd91],eax
  43adcb:	39 83 95 dd 00 00    	cmp    DWORD PTR [ebx+0xdd95],eax
  43add1:	7d 06                	jge    0x43add9
  43add3:	89 83 95 dd 00 00    	mov    DWORD PTR [ebx+0xdd95],eax
  43add9:	8b 8b 91 dd 00 00    	mov    ecx,DWORD PTR [ebx+0xdd91]
  43addf:	b8 09 00 00 00       	mov    eax,0x9
  43ade4:	3b c8                	cmp    ecx,eax
  43ade6:	7e 06                	jle    0x43adee
  43ade8:	89 83 91 dd 00 00    	mov    DWORD PTR [ebx+0xdd91],eax
  43adee:	39 83 95 dd 00 00    	cmp    DWORD PTR [ebx+0xdd95],eax
  43adf4:	7e 06                	jle    0x43adfc
  43adf6:	89 83 95 dd 00 00    	mov    DWORD PTR [ebx+0xdd95],eax
  43adfc:	8d 54 24 18          	lea    edx,[esp+0x18]
  43ae00:	52                   	push   edx
  43ae01:	e8 7d 67 01 00       	call   0x451583
  43ae06:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  43ae0a:	50                   	push   eax
  43ae0b:	e8 a8 7a 01 00       	call   0x4528b8
  43ae10:	8b c8                	mov    ecx,eax
  43ae12:	be 64 00 00 00       	mov    esi,0x64
  43ae17:	83 c4 08             	add    esp,0x8
  43ae1a:	8b 41 14             	mov    eax,DWORD PTR [ecx+0x14]
  43ae1d:	05 6c 07 00 00       	add    eax,0x76c
  43ae22:	99                   	cdq
  43ae23:	f7 fe                	idiv   esi
  43ae25:	89 53 20             	mov    DWORD PTR [ebx+0x20],edx
  43ae28:	8b 51 10             	mov    edx,DWORD PTR [ecx+0x10]
  43ae2b:	42                   	inc    edx
  43ae2c:	89 53 1c             	mov    DWORD PTR [ebx+0x1c],edx
  43ae2f:	8b 41 0c             	mov    eax,DWORD PTR [ecx+0xc]
  43ae32:	89 43 18             	mov    DWORD PTR [ebx+0x18],eax
  43ae35:	8b 41 08             	mov    eax,DWORD PTR [ecx+0x8]
  43ae38:	83 c0 08             	add    eax,0x8
  43ae3b:	83 f8 18             	cmp    eax,0x18
  43ae3e:	89 43 24             	mov    DWORD PTR [ebx+0x24],eax
  43ae41:	7c 06                	jl     0x43ae49
  43ae43:	83 c0 e8             	add    eax,0xffffffe8
  43ae46:	89 43 24             	mov    DWORD PTR [ebx+0x24],eax
  43ae49:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  43ae4c:	5f                   	pop    edi
  43ae4d:	89 4b 28             	mov    DWORD PTR [ebx+0x28],ecx
  43ae50:	8b 95 a4 00 00 00    	mov    edx,DWORD PTR [ebp+0xa4]
  43ae56:	5e                   	pop    esi
  43ae57:	8b 82 70 03 00 00    	mov    eax,DWORD PTR [edx+0x370]
  43ae5d:	89 43 14             	mov    DWORD PTR [ebx+0x14],eax
  43ae60:	8b 8d 3c 01 00 00    	mov    ecx,DWORD PTR [ebp+0x13c]
  43ae66:	89 8b ad e0 00 00    	mov    DWORD PTR [ebx+0xe0ad],ecx
  43ae6c:	8b 95 44 01 00 00    	mov    edx,DWORD PTR [ebp+0x144]
  43ae72:	89 93 a9 e0 00 00    	mov    DWORD PTR [ebx+0xe0a9],edx
  43ae78:	5d                   	pop    ebp
  43ae79:	5b                   	pop    ebx
  43ae7a:	59                   	pop    ecx
  43ae7b:	c2 04 00             	ret    0x4
  43ae7e:	90                   	nop
  43ae7f:	90                   	nop
  43ae80:	56                   	push   esi
  43ae81:	57                   	push   edi
  43ae82:	8d 71 14             	lea    esi,[ecx+0x14]
  43ae85:	bf 09 00 00 00       	mov    edi,0x9
  43ae8a:	8b 46 54             	mov    eax,DWORD PTR [esi+0x54]
  43ae8d:	85 c0                	test   eax,eax
  43ae8f:	7e 1c                	jle    0x43aead
  43ae91:	83 f8 09             	cmp    eax,0x9
  43ae94:	7d 17                	jge    0x43aead
  43ae96:	8d 14 40             	lea    edx,[eax+eax*2]
  43ae99:	8d 14 d2             	lea    edx,[edx+edx*8]
  43ae9c:	d1 e2                	shl    edx,1
  43ae9e:	2b d0                	sub    edx,eax
  43aea0:	8b 81 a4 00 00 00    	mov    eax,DWORD PTR [ecx+0xa4]
  43aea6:	c1 e2 04             	shl    edx,0x4
  43aea9:	03 d0                	add    edx,eax
  43aeab:	89 16                	mov    DWORD PTR [esi],edx
  43aead:	83 c6 04             	add    esi,0x4
  43aeb0:	4f                   	dec    edi
  43aeb1:	75 d7                	jne    0x43ae8a
  43aeb3:	5f                   	pop    edi
  43aeb4:	5e                   	pop    esi
  43aeb5:	c3                   	ret
  43aeb6:	90                   	nop
  43aeb7:	90                   	nop
  43aeb8:	90                   	nop
  43aeb9:	90                   	nop
  43aeba:	90                   	nop
  43aebb:	90                   	nop
  43aebc:	90                   	nop
  43aebd:	90                   	nop
  43aebe:	90                   	nop
