
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00435bae <.text+0x34bae>:
  435bae:	8b 8e 20 06 00 00    	mov    ecx,DWORD PTR [esi+0x620]
  435bb4:	8b 89 7c 05 00 00    	mov    ecx,DWORD PTR [ecx+0x57c]
  435bba:	e8 c1 b7 ff ff       	call   0x431380
  435bbf:	83 f8 02             	cmp    eax,0x2
  435bc2:	75 02                	jne    0x435bc6
  435bc4:	8b dd                	mov    ebx,ebp
  435bc6:	8b 96 20 06 00 00    	mov    edx,DWORD PTR [esi+0x620]
  435bcc:	8b 8a 80 05 00 00    	mov    ecx,DWORD PTR [edx+0x580]
  435bd2:	e8 a9 b7 ff ff       	call   0x431380
  435bd7:	83 f8 02             	cmp    eax,0x2
  435bda:	75 02                	jne    0x435bde
  435bdc:	8b d8                	mov    ebx,eax
  435bde:	8b 86 20 06 00 00    	mov    eax,DWORD PTR [esi+0x620]
  435be4:	8b 88 84 05 00 00    	mov    ecx,DWORD PTR [eax+0x584]
  435bea:	e8 91 b7 ff ff       	call   0x431380
  435bef:	83 f8 02             	cmp    eax,0x2
  435bf2:	75 05                	jne    0x435bf9
  435bf4:	bb 03 00 00 00       	mov    ebx,0x3
  435bf9:	8b 8e 20 06 00 00    	mov    ecx,DWORD PTR [esi+0x620]
  435bff:	8b 89 88 05 00 00    	mov    ecx,DWORD PTR [ecx+0x588]
  435c05:	e8 76 b7 ff ff       	call   0x431380
  435c0a:	83 f8 02             	cmp    eax,0x2
  435c0d:	75 05                	jne    0x435c14
  435c0f:	bb 04 00 00 00       	mov    ebx,0x4
  435c14:	8b 96 20 06 00 00    	mov    edx,DWORD PTR [esi+0x620]
  435c1a:	8b 8a 8c 05 00 00    	mov    ecx,DWORD PTR [edx+0x58c]
  435c20:	e8 5b b7 ff ff       	call   0x431380
  435c25:	83 f8 02             	cmp    eax,0x2
  435c28:	75 05                	jne    0x435c2f
  435c2a:	bb 05 00 00 00       	mov    ebx,0x5
  435c2f:	8b 86 20 06 00 00    	mov    eax,DWORD PTR [esi+0x620]
  435c35:	8b 88 90 05 00 00    	mov    ecx,DWORD PTR [eax+0x590]
  435c3b:	e8 40 b7 ff ff       	call   0x431380
  435c40:	83 f8 02             	cmp    eax,0x2
  435c43:	75 05                	jne    0x435c4a
  435c45:	bb 06 00 00 00       	mov    ebx,0x6
  435c4a:	8b 8e 20 06 00 00    	mov    ecx,DWORD PTR [esi+0x620]
  435c50:	8b 89 94 05 00 00    	mov    ecx,DWORD PTR [ecx+0x594]
  435c56:	e8 25 b7 ff ff       	call   0x431380
  435c5b:	83 f8 02             	cmp    eax,0x2
  435c5e:	75 15                	jne    0x435c75
  435c60:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  435c66:	8b 82 08 01 00 00    	mov    eax,DWORD PTR [edx+0x108]
  435c6c:	85 c0                	test   eax,eax
  435c6e:	75 05                	jne    0x435c75
  435c70:	bb 07 00 00 00       	mov    ebx,0x7
  435c75:	8b 86 20 06 00 00    	mov    eax,DWORD PTR [esi+0x620]
  435c7b:	8b 88 98 05 00 00    	mov    ecx,DWORD PTR [eax+0x598]
  435c81:	e8 fa b6 ff ff       	call   0x431380
  435c86:	83 f8 02             	cmp    eax,0x2
  435c89:	75 05                	jne    0x435c90
  435c8b:	bb 08 00 00 00       	mov    ebx,0x8
  435c90:	8b 8e 20 06 00 00    	mov    ecx,DWORD PTR [esi+0x620]
  435c96:	8b 89 9c 05 00 00    	mov    ecx,DWORD PTR [ecx+0x59c]
  435c9c:	e8 df b6 ff ff       	call   0x431380
  435ca1:	3b c5                	cmp    eax,ebp
  435ca3:	75 05                	jne    0x435caa
  435ca5:	bb 09 00 00 00       	mov    ebx,0x9
  435caa:	8b 86 bc 05 00 00    	mov    eax,DWORD PTR [esi+0x5bc]
  435cb0:	57                   	push   edi
  435cb1:	3b d8                	cmp    ebx,eax
  435cb3:	0f 84 74 01 00 00    	je     0x435e2d
  435cb9:	85 db                	test   ebx,ebx
  435cbb:	0f 84 6c 01 00 00    	je     0x435e2d
  435cc1:	8b ce                	mov    ecx,esi
  435cc3:	e8 68 06 00 00       	call   0x436330
  435cc8:	84 c0                	test   al,al
  435cca:	0f 84 5d 01 00 00    	je     0x435e2d
  435cd0:	8b 96 20 06 00 00    	mov    edx,DWORD PTR [esi+0x620]
  435cd6:	c7 82 6c 05 00 00 69 	mov    DWORD PTR [edx+0x56c],0x69
  435cdd:	00 00 00 
  435ce0:	8a 86 c0 05 00 00    	mov    al,BYTE PTR [esi+0x5c0]
  435ce6:	84 c0                	test   al,al
  435ce8:	75 0f                	jne    0x435cf9
  435cea:	83 fb 07             	cmp    ebx,0x7
  435ced:	74 0a                	je     0x435cf9
  435cef:	55                   	push   ebp
  435cf0:	6a 00                	push   0x0
  435cf2:	8b ce                	mov    ecx,esi
  435cf4:	e8 17 06 00 00       	call   0x436310
  435cf9:	8d 7b ff             	lea    edi,[ebx-0x1]
  435cfc:	83 ff 08             	cmp    edi,0x8
  435cff:	0f 87 ed 00 00 00    	ja     0x435df2
  435d05:	ff 24 bd 9c 61 43 00 	jmp    DWORD PTR [edi*4+0x43619c]
  435d0c:	8b 86 3c 06 00 00    	mov    eax,DWORD PTR [esi+0x63c]
  435d12:	c6 80 79 05 00 00 01 	mov    BYTE PTR [eax+0x579],0x1
  435d19:	8b 8e 3c 06 00 00    	mov    ecx,DWORD PTR [esi+0x63c]
  435d1f:	e8 bc 35 01 00       	call   0x4492e0
  435d24:	e9 c9 00 00 00       	jmp    0x435df2
  435d29:	8b 8e 40 06 00 00    	mov    ecx,DWORD PTR [esi+0x640]
  435d2f:	c6 81 79 05 00 00 01 	mov    BYTE PTR [ecx+0x579],0x1
  435d36:	8b 8e 40 06 00 00    	mov    ecx,DWORD PTR [esi+0x640]
  435d3c:	e8 6f 26 00 00       	call   0x4383b0
  435d41:	e9 ac 00 00 00       	jmp    0x435df2
  435d46:	8b 96 44 06 00 00    	mov    edx,DWORD PTR [esi+0x644]
  435d4c:	c6 82 79 05 00 00 01 	mov    BYTE PTR [edx+0x579],0x1
  435d53:	8b 8e 44 06 00 00    	mov    ecx,DWORD PTR [esi+0x644]
  435d59:	e8 d2 67 00 00       	call   0x43c530
  435d5e:	e9 8f 00 00 00       	jmp    0x435df2
  435d63:	8b 86 48 06 00 00    	mov    eax,DWORD PTR [esi+0x648]
  435d69:	c6 80 79 05 00 00 01 	mov    BYTE PTR [eax+0x579],0x1
  435d70:	8b 8e 48 06 00 00    	mov    ecx,DWORD PTR [esi+0x648]
  435d76:	e8 d5 85 ff ff       	call   0x42e350
  435d7b:	eb 75                	jmp    0x435df2
  435d7d:	8b 8e 4c 06 00 00    	mov    ecx,DWORD PTR [esi+0x64c]
  435d83:	c6 81 79 05 00 00 01 	mov    BYTE PTR [ecx+0x579],0x1
  435d8a:	8b 8e 4c 06 00 00    	mov    ecx,DWORD PTR [esi+0x64c]
  435d90:	e8 5b ad 00 00       	call   0x440af0
  435d95:	eb 5b                	jmp    0x435df2
  435d97:	8b 96 50 06 00 00    	mov    edx,DWORD PTR [esi+0x650]
  435d9d:	6a 00                	push   0x0
  435d9f:	c6 82 79 05 00 00 01 	mov    BYTE PTR [edx+0x579],0x1
  435da6:	8b 8e 50 06 00 00    	mov    ecx,DWORD PTR [esi+0x650]
  435dac:	e8 2f 9e 00 00       	call   0x43fbe0
  435db1:	eb 3f                	jmp    0x435df2
  435db3:	8b 86 54 06 00 00    	mov    eax,DWORD PTR [esi+0x654]
  435db9:	6a 02                	push   0x2
  435dbb:	6a 00                	push   0x0
  435dbd:	c6 80 79 05 00 00 01 	mov    BYTE PTR [eax+0x579],0x1
  435dc4:	8b 8e 20 06 00 00    	mov    ecx,DWORD PTR [esi+0x620]
  435dca:	c7 81 6c 05 00 00 0a 	mov    DWORD PTR [ecx+0x56c],0xa
  435dd1:	00 00 00 
  435dd4:	8b ce                	mov    ecx,esi
  435dd6:	e8 35 05 00 00       	call   0x436310
  435ddb:	eb 15                	jmp    0x435df2
  435ddd:	8b 96 58 06 00 00    	mov    edx,DWORD PTR [esi+0x658]
  435de3:	c6 82 79 05 00 00 01 	mov    BYTE PTR [edx+0x579],0x1
  435dea:	eb 06                	jmp    0x435df2
  435dec:	89 ae f8 05 00 00    	mov    DWORD PTR [esi+0x5f8],ebp
  435df2:	8b 86 20 06 00 00    	mov    eax,DWORD PTR [esi+0x620]
  435df8:	57                   	push   edi
  435df9:	8b 88 64 05 00 00    	mov    ecx,DWORD PTR [eax+0x564]
  435dff:	e8 ec 88 00 00       	call   0x43e6f0
  435e04:	8b 8e 38 06 00 00    	mov    ecx,DWORD PTR [esi+0x638]
  435e0a:	6a 00                	push   0x0
  435e0c:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  435e0f:	e8           	call   0x43e640
