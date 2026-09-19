
/mnt/data/solid61_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

0042bf90 <.text+0x2af90>:
  42bf90:	83 ec 18             	sub    esp,0x18
  42bf93:	53                   	push   ebx
  42bf94:	8b 5c 24 24          	mov    ebx,DWORD PTR [esp+0x24]
  42bf98:	55                   	push   ebp
  42bf99:	56                   	push   esi
  42bf9a:	8b 74 24 30          	mov    esi,DWORD PTR [esp+0x30]
  42bf9e:	b8 64 00 00 00       	mov    eax,0x64
  42bfa3:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  42bfa7:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  42bfab:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  42bfaf:	8b 46 3c             	mov    eax,DWORD PTR [esi+0x3c]
  42bfb2:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42bfb6:	8b 43 20             	mov    eax,DWORD PTR [ebx+0x20]
  42bfb9:	8b e9                	mov    ebp,ecx
  42bfbb:	57                   	push   edi
  42bfbc:	8b c8                	mov    ecx,eax
  42bfbe:	8b 7e 4c             	mov    edi,DWORD PTR [esi+0x4c]
  42bfc1:	c1 e1 04             	shl    ecx,0x4
  42bfc4:	03 c8                	add    ecx,eax
  42bfc6:	b8 67 66 66 66       	mov    eax,0x66666667
  42bfcb:	f7 e9                	imul   ecx
  42bfcd:	c1 fa 02             	sar    edx,0x2
  42bfd0:	8b ca                	mov    ecx,edx
  42bfd2:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  42bfd6:	c1 e9 1f             	shr    ecx,0x1f
  42bfd9:	03 d1                	add    edx,ecx
  42bfdb:	8b cd                	mov    ecx,ebp
  42bfdd:	03 d7                	add    edx,edi
  42bfdf:	8b 7c 24 2c          	mov    edi,DWORD PTR [esp+0x2c]
  42bfe3:	57                   	push   edi
  42bfe4:	8b f2                	mov    esi,edx
  42bfe6:	e8 35 fd ff ff       	call   0x42bd20
  42bfeb:	57                   	push   edi
  42bfec:	8b cd                	mov    ecx,ebp
  42bfee:	03 f0                	add    esi,eax
  42bff0:	e8 fb 0c 00 00       	call   0x42ccf0
  42bff5:	8b d0                	mov    edx,eax
  42bff7:	57                   	push   edi
  42bff8:	8b cd                	mov    ecx,ebp
  42bffa:	89 54 24 34          	mov    DWORD PTR [esp+0x34],edx
  42bffe:	e8 ed 0c 00 00       	call   0x42ccf0
  42c003:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  42c007:	0f af d0             	imul   edx,eax
  42c00a:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42c00e:	8b 48 50             	mov    ecx,DWORD PTR [eax+0x50]
  42c011:	8b 43 20             	mov    eax,DWORD PTR [ebx+0x20]
  42c014:	03 c8                	add    ecx,eax
  42c016:	8b c2                	mov    eax,edx
  42c018:	99                   	cdq
  42c019:	d1 e1                	shl    ecx,1
  42c01b:	f7 f9                	idiv   ecx
  42c01d:	03 f0                	add    esi,eax
  42c01f:	8b 87 48 02 00 00    	mov    eax,DWORD PTR [edi+0x248]
  42c025:	85 c0                	test   eax,eax
  42c027:	89 74 24 30          	mov    DWORD PTR [esp+0x30],esi
  42c02b:	7e 1d                	jle    0x42c04a
  42c02d:	8d 14 76             	lea    edx,[esi+esi*2]
  42c030:	b8 67 66 66 66       	mov    eax,0x66666667
  42c035:	8d 34 96             	lea    esi,[esi+edx*4]
  42c038:	f7 ee                	imul   esi
  42c03a:	c1 fa 02             	sar    edx,0x2
  42c03d:	8b c2                	mov    eax,edx
  42c03f:	c1 e8 1f             	shr    eax,0x1f
  42c042:	03 d0                	add    edx,eax
  42c044:	8b f2                	mov    esi,edx
  42c046:	89 74 24 30          	mov    DWORD PTR [esp+0x30],esi
  42c04a:	8b 87 fc 01 00 00    	mov    eax,DWORD PTR [edi+0x1fc]
  42c050:	85 c0                	test   eax,eax
  42c052:	7e 20                	jle    0x42c074
  42c054:	8d 0c f5 00 00 00 00 	lea    ecx,[esi*8+0x0]
  42c05b:	b8 67 66 66 66       	mov    eax,0x66666667
  42c060:	2b ce                	sub    ecx,esi
  42c062:	f7 e9                	imul   ecx
  42c064:	c1 fa 02             	sar    edx,0x2
  42c067:	8b ca                	mov    ecx,edx
  42c069:	c1 e9 1f             	shr    ecx,0x1f
  42c06c:	03 d1                	add    edx,ecx
  42c06e:	8b f2                	mov    esi,edx
  42c070:	89 74 24 30          	mov    DWORD PTR [esp+0x30],esi
  42c074:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  42c078:	83 ff 01             	cmp    edi,0x1
  42c07b:	75 20                	jne    0x42c09d
  42c07d:	8b 83 f0 01 00 00    	mov    eax,DWORD PTR [ebx+0x1f0]
  42c083:	85 c0                	test   eax,eax
  42c085:	7e 3d                	jle    0x42c0c4
  42c087:	c1 e6 03             	shl    esi,0x3
  42c08a:	b8 67 66 66 66       	mov    eax,0x66666667
  42c08f:	f7 ee                	imul   esi
  42c091:	c1 fa 02             	sar    edx,0x2
  42c094:	8b c2                	mov    eax,edx
  42c096:	c1 e8 1f             	shr    eax,0x1f
  42c099:	03 d0                	add    edx,eax
  42c09b:	eb 23                	jmp    0x42c0c0
  42c09d:	83 ff 02             	cmp    edi,0x2
  42c0a0:	75 22                	jne    0x42c0c4
  42c0a2:	8b 83 ec 01 00 00    	mov    eax,DWORD PTR [ebx+0x1ec]
  42c0a8:	85 c0                	test   eax,eax
  42c0aa:	7e 18                	jle    0x42c0c4
  42c0ac:	c1 e6 03             	shl    esi,0x3
  42c0af:	b8 67 66 66 66       	mov    eax,0x66666667
  42c0b4:	f7 ee                	imul   esi
  42c0b6:	c1 fa 02             	sar    edx,0x2
  42c0b9:	8b ca                	mov    ecx,edx
  42c0bb:	c1 e9 1f             	shr    ecx,0x1f
  42c0be:	03 d1                	add    edx,ecx
  42c0c0:	89 54 24 30          	mov    DWORD PTR [esp+0x30],edx
  42c0c4:	8b 53 04             	mov    edx,DWORD PTR [ebx+0x4]
  42c0c7:	8b 6c 24 1c          	mov    ebp,DWORD PTR [esp+0x1c]
  42c0cb:	6a 04                	push   0x4
  42c0cd:	52                   	push   edx
  42c0ce:	8b cd                	mov    ecx,ebp
  42c0d0:	e8 6b fe ff ff       	call   0x42bf40
  42c0d5:	84 c0                	test   al,al
  42c0d7:	74 0d                	je     0x42c0e6
  42c0d9:	83 ff 04             	cmp    edi,0x4
  42c0dc:	75 08                	jne    0x42c0e6
  42c0de:	c7 44 24 30 00 00 00 	mov    DWORD PTR [esp+0x30],0x0
  42c0e5:	00 
  42c0e6:	8b 44 bb 64          	mov    eax,DWORD PTR [ebx+edi*4+0x64]
  42c0ea:	8b 0d b0 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b0
  42c0f0:	99                   	cdq
  42c0f1:	8b f0                	mov    esi,eax
  42c0f3:	8b 83 90 00 00 00    	mov    eax,DWORD PTR [ebx+0x90]
  42c0f9:	33 f2                	xor    esi,edx
  42c0fb:	2b f2                	sub    esi,edx
  42c0fd:	85 c0                	test   eax,eax
  42c0ff:	7e 1c                	jle    0x42c11d
  42c101:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42c108:	2b d0                	sub    edx,eax
  42c10a:	8d 14 90             	lea    edx,[eax+edx*4]
  42c10d:	c1 e2 04             	shl    edx,0x4
  42c110:	2b d0                	sub    edx,eax
  42c112:	8d 04 51             	lea    eax,[ecx+edx*2]
  42c115:	8b 54 b8 74          	mov    edx,DWORD PTR [eax+edi*4+0x74]
  42c119:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  42c11d:	8b 83 94 00 00 00    	mov    eax,DWORD PTR [ebx+0x94]
  42c123:	85 c0                	test   eax,eax
  42c125:	7e 1c                	jle    0x42c143
  42c127:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42c12e:	2b d0                	sub    edx,eax
  42c130:	8d 14 90             	lea    edx,[eax+edx*4]
  42c133:	c1 e2 04             	shl    edx,0x4
  42c136:	2b d0                	sub    edx,eax
  42c138:	8d 04 51             	lea    eax,[ecx+edx*2]
  42c13b:	8b 54 b8 74          	mov    edx,DWORD PTR [eax+edi*4+0x74]
  42c13f:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  42c143:	8b 83 98 00 00 00    	mov    eax,DWORD PTR [ebx+0x98]
  42c149:	85 c0                	test   eax,eax
  42c14b:	7e 1a                	jle    0x42c167
  42c14d:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42c154:	2b d0                	sub    edx,eax
  42c156:	8d 14 90             	lea    edx,[eax+edx*4]
  42c159:	c1 e2 04             	shl    edx,0x4
  42c15c:	2b d0                	sub    edx,eax
  42c15e:	8d 04 51             	lea    eax,[ecx+edx*2]
  42c161:	8b 7c b8 74          	mov    edi,DWORD PTR [eax+edi*4+0x74]
  42c165:	eb 04                	jmp    0x42c16b
  42c167:	8b 7c 24 20          	mov    edi,DWORD PTR [esp+0x20]
  42c16b:	53                   	push   ebx
  42c16c:	8b cd                	mov    ecx,ebp
  42c16e:	e8 7d 0b 00 00       	call   0x42ccf0
  42c173:	89 44 24 2c          	mov    DWORD PTR [esp+0x2c],eax
  42c177:	db 44 24 2c          	fild   DWORD PTR [esp+0x2c]
  42c17b:	dd 5c 24 20          	fstp   QWORD PTR [esp+0x20]
  42c17f:	e8 e1 53 02 00       	call   0x451565
  42c184:	99                   	cdq
  42c185:	b9 13 00 00 00       	mov    ecx,0x13
  42c18a:	f7 f9                	idiv   ecx
  42c18c:	8b 43 4c             	mov    eax,DWORD PTR [ebx+0x4c]
  42c18f:	8b ca                	mov    ecx,edx
  42c191:	99                   	cdq
  42c192:	41                   	inc    ecx
  42c193:	f7 f9                	idiv   ecx
  42c195:	8b e8                	mov    ebp,eax
  42c197:	e8 c9 53 02 00       	call   0x451565
  42c19c:	99                   	cdq
  42c19d:	b9 09 00 00 00       	mov    ecx,0x9
  42c1a2:	f7 f9                	idiv   ecx
  42c1a4:	8b ca                	mov    ecx,edx
  42c1a6:	8b 54 24 34          	mov    edx,DWORD PTR [esp+0x34]
  42c1aa:	41                   	inc    ecx
  42c1ab:	8b 42 4c             	mov    eax,DWORD PTR [edx+0x4c]
  42c1ae:	99                   	cdq
  42c1af:	f7 f9                	idiv   ecx
  42c1b1:	03 e8                	add    ebp,eax
  42c1b3:	8b c7                	mov    eax,edi
  42c1b5:	99                   	cdq
  42c1b6:	8b c8                	mov    ecx,eax
  42c1b8:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42c1bc:	33 ca                	xor    ecx,edx
  42c1be:	89 6c 24 34          	mov    DWORD PTR [esp+0x34],ebp
  42c1c2:	2b ca                	sub    ecx,edx
  42c1c4:	8b 6c 24 14          	mov    ebp,DWORD PTR [esp+0x14]
  42c1c8:	99                   	cdq
  42c1c9:	db 44 24 34          	fild   DWORD PTR [esp+0x34]
  42c1cd:	33 c2                	xor    eax,edx
  42c1cf:	2b c2                	sub    eax,edx
  42c1d1:	0f af c8             	imul   ecx,eax
  42c1d4:	db 44 24 30          	fild   DWORD PTR [esp+0x30]
  42c1d8:	8b c5                	mov    eax,ebp
  42c1da:	99                   	cdq
  42c1db:	33 c2                	xor    eax,edx
  42c1dd:	2b c2                	sub    eax,edx
  42c1df:	0f af c8             	imul   ecx,eax
  42c1e2:	0f af ce             	imul   ecx,esi
  42c1e5:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42c1ea:	f7 e9                	imul   ecx
  42c1ec:	c1 fa 05             	sar    edx,0x5
  42c1ef:	8b c2                	mov    eax,edx
  42c1f1:	dc 64 24 20          	fsub   QWORD PTR [esp+0x20]
  42c1f5:	c1 e8 1f             	shr    eax,0x1f
  42c1f8:	03 d0                	add    edx,eax
  42c1fa:	89 54 24 34          	mov    DWORD PTR [esp+0x34],edx
  42c1fe:	de c1                	faddp  st(1),st
  42c200:	db 44 24 34          	fild   DWORD PTR [esp+0x34]
  42c204:	de c9                	fmulp  st(1),st
  42c206:	dc 0d 98 0c 46 00    	fmul   QWORD PTR ds:0x460c98
  42c20c:	dc 15 90 0c 46 00    	fcom   QWORD PTR ds:0x460c90
  42c212:	df e0                	fnstsw ax
  42c214:	f6 c4 01             	test   ah,0x1
  42c217:	74 08                	je     0x42c221
  42c219:	dd d8                	fstp   st(0)
  42c21b:	dd 05 90 0c 46 00    	fld    QWORD PTR ds:0x460c90
  42c221:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  42c225:	85 c0                	test   eax,eax
  42c227:	7e 1e                	jle    0x42c247
  42c229:	8b 4c 83 64          	mov    ecx,DWORD PTR [ebx+eax*4+0x64]
  42c22d:	85 c9                	test   ecx,ecx
  42c22f:	7c 10                	jl     0x42c241
  42c231:	85 ed                	test   ebp,ebp
  42c233:	7c 0c                	jl     0x42c241
  42c235:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42c239:	85 c0                	test   eax,eax
  42c23b:	7c 04                	jl     0x42c241
  42c23d:	85 ff                	test   edi,edi
  42c23f:	7d 06                	jge    0x42c247
  42c241:	dc 0d 88 0c 46 00    	fmul   QWORD PTR ds:0x460c88
  42c247:	e8 4c 5b 02 00       	call   0x451d98
  42c24c:	5f                   	pop    edi
  42c24d:	5e                   	pop    esi
  42c24e:	5d                   	pop    ebp
  42c24f:	5b                   	pop    ebx
  42c250:	83 c4 18             	add    esp,0x18
  42c253:	c2               	ret    0xc
