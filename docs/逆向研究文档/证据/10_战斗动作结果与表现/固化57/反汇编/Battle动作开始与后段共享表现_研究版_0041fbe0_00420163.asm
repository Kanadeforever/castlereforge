
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041fbe0 <.text+0x1ebe0>:
  41fbe0:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  41fbe6:	8b 41 18             	mov    eax,DWORD PTR [ecx+0x18]
  41fbe9:	83 f8 02             	cmp    eax,0x2
  41fbec:	74 0a                	je     0x41fbf8
  41fbee:	83 f8 01             	cmp    eax,0x1
  41fbf1:	74 05                	je     0x41fbf8
  41fbf3:	83 f8 04             	cmp    eax,0x4
  41fbf6:	75 0d                	jne    0x41fc05
  41fbf8:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  41fbfe:	6a 03                	push   0x3
  41fc00:	e8 fb 45 01 00       	call   0x434200
  41fc05:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  41fc0b:	6a ff                	push   0xffffffff
  41fc0d:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  41fc10:	e8 fb f5 01 00       	call   0x43f210
  41fc15:	8b 04 ad 94 fd 89 00 	mov    eax,DWORD PTR [ebp*4+0x89fd94]
  41fc1c:	8b 0d f0 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fdf0
  41fc22:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  41fc28:	6a 01                	push   0x1
  41fc2a:	8b 80 9c 0b 00 00    	mov    eax,DWORD PTR [eax+0xb9c]
  41fc30:	8b 3c 81             	mov    edi,DWORD PTR [ecx+eax*4]
  41fc33:	8b 1c 82             	mov    ebx,DWORD PTR [edx+eax*4]
  41fc36:	8b ce                	mov    ecx,esi
  41fc38:	e8 73 0b 00 00       	call   0x4207b0
  41fc3d:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  41fc41:	8b 86 9c 0b 00 00    	mov    eax,DWORD PTR [esi+0xb9c]
  41fc47:	8b 16                	mov    edx,DWORD PTR [esi]
  41fc49:	51                   	push   ecx
  41fc4a:	8b 0d ec fd 89 00    	mov    ecx,DWORD PTR ds:0x89fdec
  41fc50:	52                   	push   edx
  41fc51:	c1 e0 02             	shl    eax,0x2
  41fc54:	53                   	push   ebx
  41fc55:	57                   	push   edi
  41fc56:	8b 14 08             	mov    edx,DWORD PTR [eax+ecx*1]
  41fc59:	8b 0d f0 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fdf0
  41fc5f:	52                   	push   edx
  41fc60:	8d ae 30 07 00 00    	lea    ebp,[esi+0x730]
  41fc66:	8b 14 08             	mov    edx,DWORD PTR [eax+ecx*1]
  41fc69:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  41fc6f:	52                   	push   edx
  41fc70:	8b 96 48 08 00 00    	mov    edx,DWORD PTR [esi+0x848]
  41fc76:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  41fc79:	56                   	push   esi
  41fc7a:	51                   	push   ecx
  41fc7b:	52                   	push   edx
  41fc7c:	8b cd                	mov    ecx,ebp
  41fc7e:	e8 dd 4b 02 00       	call   0x444860
  41fc83:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  41fc89:	8b 41 18             	mov    eax,DWORD PTR [ecx+0x18]
  41fc8c:	83 f8 01             	cmp    eax,0x1
  41fc8f:	74 0a                	je     0x41fc9b
  41fc91:	83 f8 02             	cmp    eax,0x2
  41fc94:	74 05                	je     0x41fc9b
  41fc96:	83 f8 04             	cmp    eax,0x4
  41fc99:	75 0f                	jne    0x41fcaa
  41fc9b:	83 c1 04             	add    ecx,0x4
  41fc9e:	51                   	push   ecx
  41fc9f:	8b 0d 18 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2418
  41fca5:	e8 16 76 02 00       	call   0x4472c0
  41fcaa:	8b 86 c8 00 00 00    	mov    eax,DWORD PTR [esi+0xc8]
  41fcb0:	85 c0                	test   eax,eax
  41fcb2:	75 11                	jne    0x41fcc5
  41fcb4:	8b cd                	mov    ecx,ebp
  41fcb6:	e8 a5 52 02 00       	call   0x444f60
  41fcbb:	5f                   	pop    edi
  41fcbc:	5e                   	pop    esi
  41fcbd:	5d                   	pop    ebp
  41fcbe:	32 c0                	xor    al,al
  41fcc0:	5b                   	pop    ebx
  41fcc1:	59                   	pop    ecx
  41fcc2:	c2 08 00             	ret    0x8
  41fcc5:	6a 00                	push   0x0
  41fcc7:	8b ce                	mov    ecx,esi
  41fcc9:	e8 02 12 00 00       	call   0x420ed0
  41fcce:	5f                   	pop    edi
  41fccf:	5e                   	pop    esi
  41fcd0:	5d                   	pop    ebp
  41fcd1:	b0 01                	mov    al,0x1
  41fcd3:	5b                   	pop    ebx
  41fcd4:	59                   	pop    ecx
  41fcd5:	c2 08 00             	ret    0x8
  41fcd8:	03 fb                	add    edi,ebx
  41fcda:	41                   	inc    ecx
  41fcdb:	00 08                	add    BYTE PTR [eax],cl
  41fcdd:	fb                   	sti
  41fcde:	41                   	inc    ecx
  41fcdf:	00 0d fb 41 00 12    	add    BYTE PTR ds:0x120041fb,cl
  41fce5:	fb                   	sti
  41fce6:	41                   	inc    ecx
  41fce7:	00 54 fb 41          	add    BYTE PTR [ebx+edi*8+0x41],dl
  41fceb:	00 33                	add    BYTE PTR [ebx],dh
  41fced:	fb                   	sti
  41fcee:	41                   	inc    ecx
  41fcef:	00 38                	add    BYTE PTR [eax],bh
  41fcf1:	fb                   	sti
  41fcf2:	41                   	inc    ecx
  41fcf3:	00 3d fb 41 00 42    	add    BYTE PTR ds:0x420041fb,bh
  41fcf9:	fb                   	sti
  41fcfa:	41                   	inc    ecx
  41fcfb:	00 54 fb 41          	add    BYTE PTR [ebx+edi*8+0x41],dl
  41fcff:	00 8b 44 24 04 56    	add    BYTE PTR [ebx+0x56042444],cl
  41fd05:	8b f1                	mov    esi,ecx
  41fd07:	85 c0                	test   eax,eax
  41fd09:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  41fd0f:	75 43                	jne    0x41fd54
  41fd11:	05 a6 00 00 00       	add    eax,0xa6
  41fd16:	50                   	push   eax
  41fd17:	e8 14 fc ff ff       	call   0x41f930
  41fd1c:	84 c0                	test   al,al
  41fd1e:	0f 85 d6 00 00 00    	jne    0x41fdfa
  41fd24:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  41fd2a:	81 c1 a6 00 00 00    	add    ecx,0xa6
  41fd30:	51                   	push   ecx
  41fd31:	8b ce                	mov    ecx,esi
  41fd33:	e8 48 fc ff ff       	call   0x41f980
  41fd38:	84 c0                	test   al,al
  41fd3a:	74 57                	je     0x41fd93
  41fd3c:	8d 96 1a 0b 00 00    	lea    edx,[esi+0xb1a]
  41fd42:	8b ce                	mov    ecx,esi
  41fd44:	52                   	push   edx
  41fd45:	e8 e6 fb ff ff       	call   0x41f930
  41fd4a:	84 c0                	test   al,al
  41fd4c:	0f 85 a8 00 00 00    	jne    0x41fdfa
  41fd52:	eb 3f                	jmp    0x41fd93
  41fd54:	05 c4 00 00 00       	add    eax,0xc4
  41fd59:	8b ce                	mov    ecx,esi
  41fd5b:	50                   	push   eax
  41fd5c:	e8 cf fb ff ff       	call   0x41f930
  41fd61:	84 c0                	test   al,al
  41fd63:	0f 85 91 00 00 00    	jne    0x41fdfa
  41fd69:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  41fd6f:	81 c1 c4 00 00 00    	add    ecx,0xc4
  41fd75:	51                   	push   ecx
  41fd76:	8b ce                	mov    ecx,esi
  41fd78:	e8 03 fc ff ff       	call   0x41f980
  41fd7d:	84 c0                	test   al,al
  41fd7f:	74 12                	je     0x41fd93
  41fd81:	8d 96 24 0b 00 00    	lea    edx,[esi+0xb24]
  41fd87:	8b ce                	mov    ecx,esi
  41fd89:	52                   	push   edx
  41fd8a:	e8 a1 fb ff ff       	call   0x41f930
  41fd8f:	84 c0                	test   al,al
  41fd91:	75 67                	jne    0x41fdfa
  41fd93:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  41fd99:	57                   	push   edi
  41fd9a:	6a ff                	push   0xffffffff
  41fd9c:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  41fd9f:	e8 6c f4 01 00       	call   0x43f210
  41fda4:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  41fdaa:	6a 01                	push   0x1
  41fdac:	6a 00                	push   0x0
  41fdae:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  41fdb1:	8b 51 18             	mov    edx,DWORD PTR [ecx+0x18]
  41fdb4:	8b 70 24             	mov    esi,DWORD PTR [eax+0x24]
  41fdb7:	8b 78 28             	mov    edi,DWORD PTR [eax+0x28]
  41fdba:	2b f2                	sub    esi,edx
  41fdbc:	8b 51 1c             	mov    edx,DWORD PTR [ecx+0x1c]
  41fdbf:	2b fa                	sub    edi,edx
  41fdc1:	83 ee 36             	sub    esi,0x36
  41fdc4:	83 ef 1b             	sub    edi,0x1b
  41fdc7:	e8 24 d4 01 00       	call   0x43d1f0
  41fdcc:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  41fdd2:	2d 40 01 00 00       	sub    eax,0x140
  41fdd7:	50                   	push   eax
  41fdd8:	57                   	push   edi
  41fdd9:	56                   	push   esi
  41fdda:	57                   	push   edi
  41fddb:	56                   	push   esi
  41fddc:	68 c0 9f 46 00       	push   0x469fc0
  41fde1:	e8 ea d6 00 00       	call   0x42d4d0
  41fde6:	83 f8 ff             	cmp    eax,0xffffffff
  41fde9:	5f                   	pop    edi
  41fdea:	74 0e                	je     0x41fdfa
  41fdec:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  41fdf2:	6a 00                	push   0x0
  41fdf4:	50                   	push   eax
  41fdf5:	e8 e6 d8 00 00       	call   0x42d6e0
  41fdfa:	5e                   	pop    esi
  41fdfb:	c2 04 00             	ret    0x4
  41fdfe:	90                   	nop
  41fdff:	90                   	nop
  41fe00:	51                   	push   ecx
  41fe01:	56                   	push   esi
  41fe02:	8b f1                	mov    esi,ecx
  41fe04:	8b 86 9c 07 00 00    	mov    eax,DWORD PTR [esi+0x79c]
  41fe0a:	85 c0                	test   eax,eax
  41fe0c:	0f 84 4e 03 00 00    	je     0x420160
  41fe12:	8b 86 a4 07 00 00    	mov    eax,DWORD PTR [esi+0x7a4]
  41fe18:	3d e9 03 00 00       	cmp    eax,0x3e9
  41fe1d:	0f 84 3d 03 00 00    	je     0x420160
  41fe23:	53                   	push   ebx
  41fe24:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  41fe28:	55                   	push   ebp
  41fe29:	2d ea 03 00 00       	sub    eax,0x3ea
  41fe2e:	57                   	push   edi
  41fe2f:	74 1e                	je     0x41fe4f
  41fe31:	48                   	dec    eax
  41fe32:	0f 84 2c 01 00 00    	je     0x41ff64
  41fe38:	48                   	dec    eax
  41fe39:	0f 84 c0 01 00 00    	je     0x41ffff
  41fe3f:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  41fe43:	5f                   	pop    edi
  41fe44:	5d                   	pop    ebp
  41fe45:	89 8e a4 07 00 00    	mov    DWORD PTR [esi+0x7a4],ecx
  41fe4b:	5b                   	pop    ebx
  41fe4c:	5e                   	pop    esi
  41fe4d:	59                   	pop    ecx
  41fe4e:	c3                   	ret
  41fe4f:	8a 86 b0 07 00 00    	mov    al,BYTE PTR [esi+0x7b0]
  41fe55:	84 c0                	test   al,al
  41fe57:	74 14                	je     0x41fe6d
  41fe59:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  41fe5f:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  41fe62:	8b 41 54             	mov    eax,DWORD PTR [ecx+0x54]
  41fe65:	85 c0                	test   eax,eax
  41fe67:	0f 85 e6 02 00 00    	jne    0x420153
  41fe6d:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  41fe73:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  41fe76:	8b 80 5c 03 00 00    	mov    eax,DWORD PTR [eax+0x35c]
  41fe7c:	8a 88 2d 01 00 00    	mov    cl,BYTE PTR [eax+0x12d]
  41fe82:	84 c9                	test   cl,cl
  41fe84:	75 04                	jne    0x41fe8a
  41fe86:	33 c0                	xor    eax,eax
  41fe88:	eb 07                	jmp    0x41fe91
  41fe8a:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  41fe8d:	66 8b 41 46          	mov    ax,WORD PTR [ecx+0x46]
  41fe91:	0f bf c0             	movsx  eax,ax
  41fe94:	83 f8 02             	cmp    eax,0x2
  41fe97:	74 05                	je     0x41fe9e
  41fe99:	83 f8 03             	cmp    eax,0x3
  41fe9c:	75 09                	jne    0x41fea7
  41fe9e:	6a 00                	push   0x0
  41fea0:	8b ce                	mov    ecx,esi
  41fea2:	e8 59 fe ff ff       	call   0x41fd00
  41fea7:	8d 8e 30 07 00 00    	lea    ecx,[esi+0x730]
  41fead:	c7 44 24 10 eb 03 00 	mov    DWORD PTR [esp+0x10],0x3eb
  41feb4:	00 
  41feb5:	e8 66 4a 02 00       	call   0x444920
  41feba:	8b 96 9c 07 00 00    	mov    edx,DWORD PTR [esi+0x79c]
  41fec0:	6a 00                	push   0x0
  41fec2:	81 c2 d8 00 00 00    	add    edx,0xd8
  41fec8:	8b ce                	mov    ecx,esi
  41feca:	52                   	push   edx
  41fecb:	e8 30 f8 ff ff       	call   0x41f700
  41fed0:	50                   	push   eax
  41fed1:	8b ce                	mov    ecx,esi
  41fed3:	e8 58 fa ff ff       	call   0x41f930
  41fed8:	84 c0                	test   al,al
  41feda:	75 71                	jne    0x41ff4d
  41fedc:	8b ae c8 00 00 00    	mov    ebp,DWORD PTR [esi+0xc8]
  41fee2:	32 db                	xor    bl,bl
  41fee4:	33 c0                	xor    eax,eax
  41fee6:	85 ed                	test   ebp,ebp
  41fee8:	7e 7a                	jle    0x41ff64
  41feea:	8d be 70 01 00 00    	lea    edi,[esi+0x170]
  41fef0:	8d 96 88 00 00 00    	lea    edx,[esi+0x88]
  41fef6:	84 db                	test   bl,bl
  41fef8:	75 2c                	jne    0x41ff26
  41fefa:	8b 0a                	mov    ecx,DWORD PTR [edx]
  41fefc:	85 c9                	test   ecx,ecx
  41fefe:	7c 17                	jl     0x41ff17
  41ff00:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  41ff07:	83 b9 2c 07 00 00 00 	cmp    DWORD PTR [ecx+0x72c],0x0
  41ff0e:	7c 07                	jl     0x41ff17
  41ff10:	80 3f 00             	cmp    BYTE PTR [edi],0x0
  41ff13:	74 02                	je     0x41ff17
  41ff15:	b3 01                	mov    bl,0x1
  41ff17:	40                   	inc    eax
  41ff18:	83 c2 04             	add    edx,0x4
  41ff1b:	83 c7 50             	add    edi,0x50
  41ff1e:	3b c5                	cmp    eax,ebp
  41ff20:	7c d4                	jl     0x41fef6
  41ff22:	84 db                	test   bl,bl
  41ff24:	74 3e                	je     0x41ff64
  41ff26:	8b 96 9c 07 00 00    	mov    edx,DWORD PTR [esi+0x79c]
  41ff2c:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  41ff32:	81 c2 d8 00 00 00    	add    edx,0xd8
  41ff38:	52                   	push   edx
  41ff39:	e8 42 41 01 00       	call   0x434080
  41ff3e:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  41ff44:	6a 01                	push   0x1
  41ff46:	e8 b5 42 01 00       	call   0x434200
  41ff4b:	eb 17                	jmp    0x41ff64
  41ff4d:	80 7e 04 01          	cmp    BYTE PTR [esi+0x4],0x1
  41ff51:	75 11                	jne    0x41ff64
  41ff53:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  41ff59:	6a 02                	push   0x2
  41ff5b:	e8 a0 42 01 00       	call   0x434200
  41ff60:	c6 46 04 00          	mov    BYTE PTR [esi+0x4],0x0
  41ff64:	8d 8e 30 07 00 00    	lea    ecx,[esi+0x730]
  41ff6a:	e8 61 4a 02 00       	call   0x4449d0
  41ff6f:	84 c0                	test   al,al
  41ff71:	0f 85 dc 01 00 00    	jne    0x420153
  41ff77:	6a 02                	push   0x2
  41ff79:	8b ce                	mov    ecx,esi
  41ff7b:	e8 50 0f 00 00       	call   0x420ed0
  41ff80:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  41ff86:	6a 00                	push   0x0
  41ff88:	c7 44 24 14 ec 03 00 	mov    DWORD PTR [esp+0x14],0x3ec
  41ff8f:	00 
  41ff90:	e8 6b 42 01 00       	call   0x434200
  41ff95:	8a 86 b0 07 00 00    	mov    al,BYTE PTR [esi+0x7b0]
  41ff9b:	84 c0                	test   al,al
  41ff9d:	74 60                	je     0x41ffff
  41ff9f:	8b 86 9c 0b 00 00    	mov    eax,DWORD PTR [esi+0xb9c]
  41ffa5:	8b 0d ec fd 89 00    	mov    ecx,DWORD PTR ds:0x89fdec
  41ffab:	c1 e0 02             	shl    eax,0x2
  41ffae:	8b 14 08             	mov    edx,DWORD PTR [eax+ecx*1]
  41ffb1:	8b 0d f0 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fdf0
  41ffb7:	52                   	push   edx
  41ffb8:	8b 14 08             	mov    edx,DWORD PTR [eax+ecx*1]
  41ffbb:	8b ce                	mov    ecx,esi
  41ffbd:	52                   	push   edx
  41ffbe:	6a 71                	push   0x71
  41ffc0:	e8 5b 21 00 00       	call   0x422120
  41ffc5:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  41ffcb:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  41ffce:	8b 81 5c 03 00 00    	mov    eax,DWORD PTR [ecx+0x35c]
  41ffd4:	8a 88 2d 01 00 00    	mov    cl,BYTE PTR [eax+0x12d]
  41ffda:	84 c9                	test   cl,cl
  41ffdc:	75 04                	jne    0x41ffe2
  41ffde:	33 c0                	xor    eax,eax
  41ffe0:	eb 07                	jmp    0x41ffe9
  41ffe2:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  41ffe5:	66 8b 42 46          	mov    ax,WORD PTR [edx+0x46]
  41ffe9:	0f bf c0             	movsx  eax,ax
  41ffec:	83 f8 01             	cmp    eax,0x1
  41ffef:	74 05                	je     0x41fff6
  41fff1:	83 f8 03             	cmp    eax,0x3
  41fff4:	75 09                	jne    0x41ffff
  41fff6:	6a 01                	push   0x1
  41fff8:	8b ce                	mov    ecx,esi
  41fffa:	e8 01 fd ff ff       	call   0x41fd00
  41ffff:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  420005:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
  420008:	8b 48 54             	mov    ecx,DWORD PTR [eax+0x54]
  42000b:	85 c9                	test   ecx,ecx
  42000d:	0f 85 40 01 00 00    	jne    0x420153
  420013:	8b 80 5c 03 00 00    	mov    eax,DWORD PTR [eax+0x35c]
  420019:	8a 88 2d 01 00 00    	mov    cl,BYTE PTR [eax+0x12d]
  42001f:	84 c9                	test   cl,cl
  420021:	75 04                	jne    0x420027
  420023:	33 c0                	xor    eax,eax
  420025:	eb 07                	jmp    0x42002e
  420027:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  42002a:	66 8b 41 46          	mov    ax,WORD PTR [ecx+0x46]
  42002e:	0f bf c0             	movsx  eax,ax
  420031:	bf 02 00 00 00       	mov    edi,0x2
  420036:	3b c7                	cmp    eax,edi
  420038:	74 05                	je     0x42003f
  42003a:	83 f8 03             	cmp    eax,0x3
  42003d:	75 09                	jne    0x420048
  42003f:	6a 01                	push   0x1
  420041:	8b ce                	mov    ecx,esi
  420043:	e8 b8 fc ff ff       	call   0x41fd00
  420048:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  42004e:	c7 44 24 10 ed 03 00 	mov    DWORD PTR [esp+0x10],0x3ed
  420055:	00 
  420056:	8b 48 18             	mov    ecx,DWORD PTR [eax+0x18]
  420059:	3b cf                	cmp    ecx,edi
  42005b:	74 0e                	je     0x42006b
  42005d:	83 f9 01             	cmp    ecx,0x1
  420060:	74 09                	je     0x42006b
  420062:	83 f9 04             	cmp    ecx,0x4
  420065:	0f 85 a8 00 00 00    	jne    0x420113
  42006b:	8b 0e                	mov    ecx,DWORD PTR [esi]
  42006d:	83 f9 07             	cmp    ecx,0x7
  420070:	7f 07                	jg     0x420079
  420072:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  420075:	85 d2                	test   edx,edx
  420077:	74 10                	je     0x420089
  420079:	83 f9 08             	cmp    ecx,0x8
  42007c:	7c 06                	jl     0x420084
  42007e:	83 78 20 01          	cmp    DWORD PTR [eax+0x20],0x1
  420082:	74 05                	je     0x420089
  420084:	39 78 20             	cmp    DWORD PTR [eax+0x20],edi
  420087:	75 23                	jne    0x4200ac
  420089:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  42008f:	52                   	push   edx
  420090:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  420096:	52                   	push   edx
  420097:	8b 10                	mov    edx,DWORD PTR [eax]
  420099:	51                   	push   ecx
  42009a:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  4200a0:	51                   	push   ecx
  4200a1:	52                   	push   edx
  4200a2:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4200a7:	e8 74 da 01 00       	call   0x43db20
  4200ac:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4200ae:	83 f9 07             	cmp    ecx,0x7
  4200b1:	7f 0c                	jg     0x4200bf
  4200b3:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200b9:	83 78 20 01          	cmp    DWORD PTR [eax+0x20],0x1
  4200bd:	74 1d                	je     0x4200dc
  4200bf:	83 f9 08             	cmp    ecx,0x8
  4200c2:	7c 0d                	jl     0x4200d1
  4200c4:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200ca:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  4200cd:	85 d2                	test   edx,edx
  4200cf:	74 0b                	je     0x4200dc
  4200d1:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200d7:	39 78 20             	cmp    DWORD PTR [eax+0x20],edi
  4200da:	75 23                	jne    0x4200ff
  4200dc:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  4200e2:	52                   	push   edx
  4200e3:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  4200e9:	52                   	push   edx
  4200ea:	8b 10                	mov    edx,DWORD PTR [eax]
  4200ec:	51                   	push   ecx
  4200ed:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  4200f3:	51                   	push   ecx
  4200f4:	52                   	push   edx
  4200f5:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4200fa:	e8 21 da 01 00       	call   0x43db20
  4200ff:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  420105:	6a 00                	push   0x0
  420107:	e8 f4 40 01 00       	call   0x434200
  42010c:	c6 05 dc 96 46 00 01 	mov    BYTE PTR ds:0x4696dc,0x1
  420113:	6a 00                	push   0x0
  420115:	6a 00                	push   0x0
  420117:	6a 67                	push   0x67
  420119:	8b ce                	mov    ecx,esi
  42011b:	e8 00 20 00 00       	call   0x422120
  420120:	6a 01                	push   0x1
  420122:	8b ce                	mov    ecx,esi
  420124:	e8 a7 0d 00 00       	call   0x420ed0
  420129:	8b ce                	mov    ecx,esi
  42012b:	c7 86 e4 0c 00 00 b9 	mov    DWORD PTR [esi+0xce4],0xbb9
  420132:	0b 00 00 
  420135:	e8 b6 02 00 00       	call   0x4203f0
  42013a:	6a 00                	push   0x0
  42013c:	8b ce                	mov    ecx,esi
  42013e:	e8 6d 06 00 00       	call   0x4207b0
  420143:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  420149:	6a ff                	push   0xffffffff
  42014b:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  42014e:	e8 bd f0 01 00       	call   0x43f210
  420153:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  420157:	5f                   	pop    edi
  420158:	5d                   	pop    ebp
  420159:	89 8e a4 07 00 00    	mov    DWORD PTR [esi+0x7a4],ecx
  42015f:	5b                   	pop    ebx
  420160:	5e                   	pop    esi
  420161:	59                   	pop    ecx
  420162:	c3                   	ret
