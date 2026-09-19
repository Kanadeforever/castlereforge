===== raw DWORD候选 0x448B4D -> 0x0089FEE8：实际位于CALL相对位移字节中 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00448b10 <.text+0x47b10>:
  448b10:	00 00                	add    BYTE PTR [eax],al
  448b12:	83 c4 04             	add    esp,0x4
  448b15:	89 be 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],edi
  448b1b:	8b 9e 90 05 00 00    	mov    ebx,DWORD PTR [esi+0x590]
  448b21:	3b df                	cmp    ebx,edi
  448b23:	74 16                	je     0x448b3b
  448b25:	8b cb                	mov    ecx,ebx
  448b27:	e8 b4 90 fe ff       	call   0x431be0
  448b2c:	53                   	push   ebx
  448b2d:	e8 1e 8a 00 00       	call   0x451550
  448b32:	83 c4 04             	add    esp,0x4
  448b35:	89 be 90 05 00 00    	mov    DWORD PTR [esi+0x590],edi
  448b3b:	8b 9e 94 05 00 00    	mov    ebx,DWORD PTR [esi+0x594]
  448b41:	3b df                	cmp    ebx,edi
  448b43:	74 16                	je     0x448b5b
  448b45:	8b cb                	mov    ecx,ebx
  448b47:	e8 94 90 fe ff       	call   0x431be0
  448b4c:	53                   	push   ebx
  448b4d:	e8 fe 89 00 00       	call   0x451550
  448b52:	83 c4 04             	add    esp,0x4
  448b55:	89 be 94 05 00 00    	mov    DWORD PTR [esi+0x594],edi
  448b5b:	8b 9e 98 05 00 00    	mov    ebx,DWORD PTR [esi+0x598]
  448b61:	3b df                	cmp    ebx,edi
  448b63:	74 16                	je     0x448b7b
  448b65:	8b cb                	mov    ecx,ebx
  448b67:	e8 64 8a fe ff       	call   0x4315d0
  448b6c:	53                   	push   ebx
  448b6d:	e8 de 89 00 00       	call   0x451550
  448b72:	83 c4 04             	add    esp,0x4
  448b75:	89 be 98 05 00 00    	mov    DWORD PTR [esi+0x598],edi
  448b7b:	8b 9e 9c 05 00     	mov    ebx,DWORD PTR [esi+0x59c]

===== raw DWORD候选 0x44EC1E -> 0x008A0000：跨指令字节假阳性 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0044ebd0 <.text+0x4dbd0>:
  44ebd0:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  44ebd3:	85 c9                	test   ecx,ecx
  44ebd5:	74 2a                	je     0x44ec01
  44ebd7:	8b 40 14             	mov    eax,DWORD PTR [eax+0x14]
  44ebda:	85 c0                	test   eax,eax
  44ebdc:	74 0a                	je     0x44ebe8
  44ebde:	56                   	push   esi
  44ebdf:	50                   	push   eax
  44ebe0:	e8 ab 13 00 00       	call   0x44ff90
  44ebe5:	83 c4 08             	add    esp,0x8
  44ebe8:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  44ebeb:	8b 4e 28             	mov    ecx,DWORD PTR [esi+0x28]
  44ebee:	50                   	push   eax
  44ebef:	51                   	push   ecx
  44ebf0:	ff 56 24             	call   DWORD PTR [esi+0x24]
  44ebf3:	83 c4 08             	add    esp,0x8
  44ebf6:	c7 46 1c 00 00 00 00 	mov    DWORD PTR [esi+0x1c],0x0
  44ebfd:	33 c0                	xor    eax,eax
  44ebff:	5e                   	pop    esi
  44ec00:	c3                   	ret
  44ec01:	b8 fe ff ff ff       	mov    eax,0xfffffffe
  44ec06:	5e                   	pop    esi
  44ec07:	c3                   	ret
  44ec08:	90                   	nop
  44ec09:	90                   	nop
  44ec0a:	90                   	nop
  44ec0b:	90                   	nop
  44ec0c:	90                   	nop
  44ec0d:	90                   	nop
  44ec0e:	90                   	nop
  44ec0f:	90                   	nop
  44ec10:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  44ec14:	56                   	push   esi
  44ec15:	57                   	push   edi
  44ec16:	33 ff                	xor    edi,edi
  44ec18:	3b c7                	cmp    eax,edi
  44ec1a:	0f 84 f6 00 00 00    	je     0x44ed16
  44ec20:	8a 00                	mov    al,BYTE PTR [eax]
  44ec22:	8a 0d a4 ce 46 00    	mov    cl,BYTE PTR ds:0x46cea4
  44ec28:	3a c1                	cmp    al,cl
  44ec2a:	0f 85 e6 00 00 00    	jne    0x44ed16
  44ec30:	83 7c 24 18 38       	cmp    DWORD PTR [esp+0x18],0x38
  44ec35:	0f 85 db 00 00 00    	jne    0x44ed16
  44ec3b:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  44ec3f:	3b f7                	cmp    esi,edi
  44ec41:	75 08                	jne    0x44ec4b
  44ec43:	5f                   	pop    edi
  44ec44:	b8 fe ff ff ff       	mov    eax,0xfffffffe
  44ec49:	5e                   	pop    esi
  44ec4a:	c3                   	ret
  44ec4b:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  44ec4e:	89 7e 18             	mov    DWORD PTR [esi+0x18],edi
  44ec51:	3b c7                	cmp    eax,edi
  44ec53:	75 0a                	jne    0x44ec5f
  44ec55:	c7 46 20 00 01 45 00 	mov    DWORD PTR [esi+0x20],0x450100
  44ec5c:	89 7e 28             	mov    DWORD PTR [esi+0x28],edi
  44ec5f:	39               	cmp    DWORD PTR [esi+0x24],edi

===== raw DWORD候选 0x457729 -> 0x008A0046：异常/错误处理代码中的跨字节假阳性 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004576f0 <.text+0x566f0>:
  4576f0:	46                   	inc    esi
  4576f1:	00 85 00 00 00 eb    	add    BYTE PTR [ebp-0x15000000],al
  4576f7:	37                   	aaa
  4576f8:	3d 8d 00 00 c0       	cmp    eax,0xc000008d
  4576fd:	75 0c                	jne    0x45770b
  4576ff:	c7 05 24 ea 46 00 82 	mov    DWORD PTR ds:0x46ea24,0x82
  457706:	00 00 00 
  457709:	eb 24                	jmp    0x45772f
  45770b:	3d 8f 00 00 c0       	cmp    eax,0xc000008f
  457710:	75 0c                	jne    0x45771e
  457712:	c7 05 24 ea 46 00 86 	mov    DWORD PTR ds:0x46ea24,0x86
  457719:	00 00 00 
  45771c:	eb 11                	jmp    0x45772f
  45771e:	3d 92 00 00 c0       	cmp    eax,0xc0000092
  457723:	75 0a                	jne    0x45772f
  457725:	c7 05 24 ea 46 00 8a 	mov    DWORD PTR ds:0x46ea24,0x8a
  45772c:	00 00 00 
  45772f:	ff 35 24 ea 46 00    	push   DWORD PTR ds:0x46ea24
  457735:	6a 08                	push   0x8
  457737:	ff d3                	call   ebx
  457739:	59                   	pop    ecx
  45773a:	89 35 24 ea 46 00    	mov    DWORD PTR ds:0x46ea24,esi
  457740:	59                   	pop    ecx
  457741:	5e                   	pop    esi
  457742:	eb 08                	jmp    0x45774c
  457744:	83 60 08 00          	and    DWORD PTR [eax+0x8],0x0
  457748:	51                   	push   ecx
  457749:	ff d3                	call   ebx
  45774b:	59                   	pop    ecx
  45774c:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  45774f:	a3 80 20 98 00       	mov    ds:0x982080,eax
  457754:	83 c8 ff             	or     eax,0xffffffff
  457757:	eb 09                	jmp    0x457762
  457759:	ff 75 0c             	push   DWORD PTR [ebp+0xc]
  45775c:	ff 15 7c 00      	call   DWORD PTR ds:0x46007c
