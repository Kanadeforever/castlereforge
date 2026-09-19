
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042afd0 <.text+0x29fd0>:
  42afd0:	57                   	push   edi
  42afd1:	08 8b 54 24 24 8b    	or     BYTE PTR [ebx-0x74dbdbac],cl
  42afd7:	4f                   	dec    edi
  42afd8:	08 53 8b             	or     BYTE PTR [ebx-0x75],dl
  42afdb:	6c                   	ins    BYTE PTR es:[edi],dx
  42afdc:	24 1c                	and    al,0x1c
  42afde:	8b 82 90 00 00 00    	mov    eax,DWORD PTR [edx+0x90]
  42afe4:	50                   	push   eax
  42afe5:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  42afe8:	03 c8                	add    ecx,eax
  42afea:	8d 47 3c             	lea    eax,[edi+0x3c]
  42afed:	51                   	push   ecx
  42afee:	50                   	push   eax
  42afef:	8d 47 38             	lea    eax,[edi+0x38]
  42aff2:	8b cd                	mov    ecx,ebp
  42aff4:	50                   	push   eax
  42aff5:	e8 a6 0e 00 00       	call   0x42bea0
  42affa:	83 bb d4 00 00 00 04 	cmp    DWORD PTR [ebx+0xd4],0x4
  42b001:	75 11                	jne    0x42b014
  42b003:	8b 86 28 02 00 00    	mov    eax,DWORD PTR [esi+0x228]
  42b009:	85 c0                	test   eax,eax
  42b00b:	7e 07                	jle    0x42b014
  42b00d:	c7 47 04 ff ff ff ff 	mov    DWORD PTR [edi+0x4],0xffffffff
  42b014:	8b 83 d4 00 00 00    	mov    eax,DWORD PTR [ebx+0xd4]
  42b01a:	83 f8 0a             	cmp    eax,0xa
  42b01d:	74 23                	je     0x42b042
  42b01f:	83 f8 0b             	cmp    eax,0xb
  42b022:	74 1e                	je     0x42b042
  42b024:	8b 83 c8 00 00 00    	mov    eax,DWORD PTR [ebx+0xc8]
  42b02a:	83 f8 02             	cmp    eax,0x2
  42b02d:	74 13                	je     0x42b042
  42b02f:	83 f8 03             	cmp    eax,0x3
  42b032:	74 0e                	je     0x42b042
  42b034:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42b038:	6a 00                	push   0x0
  42b03a:	57                   	push   edi
  42b03b:	53                   	push   ebx
  42b03c:	6a 00                	push   0x0
  42b03e:	56                   	push   esi
  42b03f:	52                   	push   edx
  42b040:	eb 0c                	jmp    0x42b04e
  42b042:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42b046:	6a 01                	push   0x1
  42b048:	57                   	push   edi
  42b049:	53                   	push   ebx
  42b04a:	6a 00                	push   0x0
  42b04c:	56                   	push   esi
  42b04d:	50                   	push   eax
  42b04e:	8b cd                	mov    ecx,ebp
  42b050:	e8 7b 00 00 00       	call   0x42b0d0
  42b055:	81 7b 20 08 01 00 00 	cmp    DWORD PTR [ebx+0x20],0x108
  42b05c:	75 10                	jne    0x42b06e
  42b05e:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  42b061:	f7 d9                	neg    ecx
  42b063:	89 4f 04             	mov    DWORD PTR [edi+0x4],ecx
  42b066:	8b 56 34             	mov    edx,DWORD PTR [esi+0x34]
  42b069:	f7 da                	neg    edx
  42b06b:	89 57 08             	mov    DWORD PTR [edi+0x8],edx
  42b06e:	8b 43 20             	mov    eax,DWORD PTR [ebx+0x20]
  42b071:	3d 0f 01 00 00       	cmp    eax,0x10f
  42b076:	74 0e                	je     0x42b086
  42b078:	3d 10 01 00 00       	cmp    eax,0x110
  42b07d:	74 07                	je     0x42b086
  42b07f:	3d 11 01 00 00       	cmp    eax,0x111
  42b084:	75 0b                	jne    0x42b091
  42b086:	c6 47 40 01          	mov    BYTE PTR [edi+0x40],0x1
  42b08a:	c7 46 30 01 00 00 00 	mov    DWORD PTR [esi+0x30],0x1
  42b091:	8b 83 c8 00 00 00    	mov    eax,DWORD PTR [ebx+0xc8]
  42b097:	83 f8 02             	cmp    eax,0x2
  42b09a:	74 16                	je     0x42b0b2
  42b09c:	83 f8 03             	cmp    eax,0x3
  42b09f:	74 11                	je     0x42b0b2
  42b0a1:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  42b0a4:	85 c0                	test   eax,eax
  42b0a6:	75 0a                	jne    0x42b0b2
  42b0a8:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  42b0ab:	85 c0                	test   eax,eax
  42b0ad:	75 03                	jne    0x42b0b2
  42b0af:	c6 07 00             	mov    BYTE PTR [edi],0x0
  42b0b2:	8b 83 7a 03 00 00    	mov    eax,DWORD PTR [ebx+0x37a]
  42b0b8:	89 47 44             	mov    DWORD PTR [edi+0x44],eax
  42b0bb:	5f                   	pop    edi
  42b0bc:	5e                   	pop    esi
  42b0bd:	5d                   	pop    ebp
  42b0be:	5b                   	pop    ebx
  42b0bf:	83 c4 10             	add    esp,0x10
  42b0c2:	c2 14 00             	ret    0x14
  42b0c5:	90                   	nop
  42b0c6:	90                   	nop
  42b0c7:	90                   	nop
  42b0c8:	90                   	nop
  42b0c9:	90                   	nop
  42b0ca:	90                   	nop
  42b0cb:	90                   	nop
  42b0cc:	90                   	nop
  42b0cd:	90                   	nop
  42b0ce:	90                   	nop
  42b0cf:	90                   	nop
