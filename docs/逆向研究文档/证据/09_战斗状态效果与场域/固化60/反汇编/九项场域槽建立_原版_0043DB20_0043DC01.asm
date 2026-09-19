
/mnt/data/solid60_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0043db20 <.text+0x3cb20>:
  43db20:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  43db24:	56                   	push   esi
  43db25:	57                   	push   edi
  43db26:	8b f9                	mov    edi,ecx
  43db28:	83 ce ff             	or     esi,0xffffffff
  43db2b:	b1 01                	mov    cl,0x1
  43db2d:	b8 4c bc 46 00       	mov    eax,0x46bc4c
  43db32:	84 c9                	test   cl,cl
  43db34:	74 13                	je     0x43db49
  43db36:	39 50 fc             	cmp    DWORD PTR [eax-0x4],edx
  43db39:	75 04                	jne    0x43db3f
  43db3b:	8b 30                	mov    esi,DWORD PTR [eax]
  43db3d:	32 c9                	xor    cl,cl
  43db3f:	83 c0 08             	add    eax,0x8
  43db42:	3d 94 bc 46 00       	cmp    eax,0x46bc94
  43db47:	7c e9                	jl     0x43db32
  43db49:	85 f6                	test   esi,esi
  43db4b:	0f 8c ab 00 00 00    	jl     0x43dbfc
  43db51:	83 fe 08             	cmp    esi,0x8
  43db54:	0f 8f a2 00 00 00    	jg     0x43dbfc
  43db5a:	8b 44 b7 48          	mov    eax,DWORD PTR [edi+esi*4+0x48]
  43db5e:	85 c0                	test   eax,eax
  43db60:	0f 85 96 00 00 00    	jne    0x43dbfc
  43db66:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  43db6a:	89 44 b7 78          	mov    DWORD PTR [edi+esi*4+0x78],eax
  43db6e:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  43db72:	c7 44 b7 48 01 00 00 	mov    DWORD PTR [edi+esi*4+0x48],0x1
  43db79:	00 
  43db7a:	89 04 b7             	mov    DWORD PTR [edi+esi*4],eax
  43db7d:	85 c0                	test   eax,eax
  43db7f:	7e 0a                	jle    0x43db8b
  43db81:	c7 44 b7 24 b4 fb ff 	mov    DWORD PTR [edi+esi*4+0x24],0xfffffbb4
  43db88:	ff 
  43db89:	eb 08                	jmp    0x43db93
  43db8b:	c7 44 b7 24 4c 04 00 	mov    DWORD PTR [edi+esi*4+0x24],0x44c
  43db92:	00 
  43db93:	8b 87 c0 01 00 00    	mov    eax,DWORD PTR [edi+0x1c0]
  43db99:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  43db9d:	48                   	dec    eax
  43db9e:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  43dba2:	f7 d8                	neg    eax
  43dba4:	1b c0                	sbb    eax,eax
  43dba6:	6a 00                	push   0x0
  43dba8:	24 ec                	and    al,0xec
  43dbaa:	6a 00                	push   0x0
  43dbac:	83 c0 1b             	add    eax,0x1b
  43dbaf:	68 70 17 00 00       	push   0x1770
  43dbb4:	8b 0c 81             	mov    ecx,DWORD PTR [ecx+eax*4]
  43dbb7:	8b 04 82             	mov    eax,DWORD PTR [edx+eax*4]
  43dbba:	50                   	push   eax
  43dbbb:	51                   	push   ecx
  43dbbc:	50                   	push   eax
  43dbbd:	8d 04 b6             	lea    eax,[esi+esi*4]
  43dbc0:	51                   	push   ecx
  43dbc1:	8d 0c 85 94 bb 46 00 	lea    ecx,[eax*4+0x46bb94]
  43dbc8:	51                   	push   ecx
  43dbc9:	8b cf                	mov    ecx,edi
  43dbcb:	e8 e0 00 00 00       	call   0x43dcb0
  43dbd0:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  43dbd6:	50                   	push   eax
  43dbd7:	e8 f4 f8 fe ff       	call   0x42d4d0
  43dbdc:	85 c0                	test   eax,eax
  43dbde:	89 84 b7 9c 00 00 00 	mov    DWORD PTR [edi+esi*4+0x9c],eax
  43dbe5:	7c 0e                	jl     0x43dbf5
  43dbe7:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  43dbed:	6a 01                	push   0x1
  43dbef:	50                   	push   eax
  43dbf0:	e8 eb fa fe ff       	call   0x42d6e0
  43dbf5:	5f                   	pop    edi
  43dbf6:	5e                   	pop    esi
  43dbf7:	b0 01                	mov    al,0x1
  43dbf9:	c2 14 00             	ret    0x14
  43dbfc:	5f                   	pop    edi
  43dbfd:	5e                   	pop    esi
  43dbfe:	32 c0                	xor    al,al
  43dc00:	c2               	ret    0x14
