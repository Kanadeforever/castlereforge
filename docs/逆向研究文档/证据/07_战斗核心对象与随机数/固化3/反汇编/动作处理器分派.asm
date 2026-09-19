
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00422ab0 <.text+0x21ab0>:
  422ab0:	83 ec 18             	sub    esp,0x18
  422ab3:	56                   	push   esi
  422ab4:	8b f1                	mov    esi,ecx
  422ab6:	57                   	push   edi
  422ab7:	33 ff                	xor    edi,edi
  422ab9:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  422abf:	c6 46 04 00          	mov    BYTE PTR [esi+0x4],0x0
  422ac3:	3b c7                	cmp    eax,edi
  422ac5:	89 be 68 01 00 00    	mov    DWORD PTR [esi+0x168],edi
  422acb:	89 be 6c 01 00 00    	mov    DWORD PTR [esi+0x16c],edi
  422ad1:	74 11                	je     0x422ae4
  422ad3:	39 be d0 0d 00 00    	cmp    DWORD PTR [esi+0xdd0],edi
  422ad9:	7d 09                	jge    0x422ae4
  422adb:	8b 40 18             	mov    eax,DWORD PTR [eax+0x18]
  422ade:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  422ae2:	eb 20                	jmp    0x422b04
  422ae4:	8b 8e d0 0d 00 00    	mov    ecx,DWORD PTR [esi+0xdd0]
  422aea:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  422af0:	51                   	push   ecx
  422af1:	c7 44 24 10 0a 00 00 	mov    DWORD PTR [esp+0x10],0xa
  422af8:	00 
  422af9:	8b 8a 90 00 00 00    	mov    ecx,DWORD PTR [edx+0x90]
  422aff:	e8 1c 6a 01 00       	call   0x439520
  422b04:	8b 86 c8 00 00 00    	mov    eax,DWORD PTR [esi+0xc8]
  422b0a:	89 7c 24 18          	mov    DWORD PTR [esp+0x18],edi
  422b0e:	3b c7                	cmp    eax,edi
  422b10:	0f 8e 93 01 00 00    	jle    0x422ca9
  422b16:	8d 46 48             	lea    eax,[esi+0x48]
  422b19:	53                   	push   ebx
  422b1a:	55                   	push   ebp
  422b1b:	c7 44 24 1c 60 00 00 	mov    DWORD PTR [esp+0x1c],0x60
  422b22:	00 
  422b23:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  422b27:	8d 9e 70 01 00 00    	lea    ebx,[esi+0x170]
  422b2d:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  422b31:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
  422b36:	8b 69 40             	mov    ebp,DWORD PTR [ecx+0x40]
  422b39:	89 6c 24 24          	mov    DWORD PTR [esp+0x24],ebp
  422b3d:	8b 04 ad 94 fd 89 00 	mov    eax,DWORD PTR [ebp*4+0x89fd94]
  422b44:	81 b8 e4 0c 00 00 be 	cmp    DWORD PTR [eax+0xce4],0xbbe
  422b4b:	0b 00 00 
  422b4e:	75 05                	jne    0x422b55
  422b50:	c6 44 24 10 01       	mov    BYTE PTR [esp+0x10],0x1
  422b55:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  422b59:	85 c9                	test   ecx,ecx
  422b5b:	75 29                	jne    0x422b86
  422b5d:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  422b61:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  422b67:	52                   	push   edx
  422b68:	51                   	push   ecx
  422b69:	05 4c 08 00 00       	add    eax,0x84c
  422b6e:	53                   	push   ebx
  422b6f:	8d 96 4c 08 00 00    	lea    edx,[esi+0x84c]
  422b75:	50                   	push   eax
  422b76:	52                   	push   edx
  422b77:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  422b7c:	e8 1f 7c 00 00       	call   0x42a7a0
  422b81:	e9 9b 00 00 00       	jmp    0x422c21
  422b86:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  422b8a:	83 f9 01             	cmp    ecx,0x1
  422b8d:	74 41                	je     0x422bd0
  422b8f:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  422b93:	83 f9 02             	cmp    ecx,0x2
  422b96:	74 38                	je     0x422bd0
  422b98:	83 f9 04             	cmp    ecx,0x4
  422b9b:	74 33                	je     0x422bd0
  422b9d:	83 f9 05             	cmp    ecx,0x5
  422ba0:	74 2e                	je     0x422bd0
  422ba2:	83 f9 06             	cmp    ecx,0x6
  422ba5:	74 29                	je     0x422bd0
  422ba7:	83 f9 0a             	cmp    ecx,0xa
  422baa:	75 48                	jne    0x422bf4
  422bac:	8b 96 f4 0c 00 00    	mov    edx,DWORD PTR [esi+0xcf4]
  422bb2:	8b cf                	mov    ecx,edi
  422bb4:	51                   	push   ecx
  422bb5:	52                   	push   edx
  422bb6:	05 4c 08 00 00       	add    eax,0x84c
  422bbb:	53                   	push   ebx
  422bbc:	50                   	push   eax
  422bbd:	8d 86 4c 08 00 00    	lea    eax,[esi+0x84c]
  422bc3:	50                   	push   eax
  422bc4:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  422bc9:	e8 02 7c 00 00       	call   0x42a7d0
  422bce:	eb 51                	jmp    0x422c21
  422bd0:	8b 8e 48 08 00 00    	mov    ecx,DWORD PTR [esi+0x848]
  422bd6:	57                   	push   edi
  422bd7:	51                   	push   ecx
  422bd8:	05 4c 08 00 00       	add    eax,0x84c
  422bdd:	53                   	push   ebx
  422bde:	8d 96 4c 08 00 00    	lea    edx,[esi+0x84c]
  422be4:	50                   	push   eax
  422be5:	52                   	push   edx
  422be6:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  422beb:	e8 b0 7b 00 00       	call   0x42a7a0
  422bf0:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  422bf4:	83 f9 04             	cmp    ecx,0x4
  422bf7:	75 28                	jne    0x422c21
  422bf9:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  422bff:	8b 0c ad 94 fd 89 00 	mov    ecx,DWORD PTR [ebp*4+0x89fd94]
  422c06:	57                   	push   edi
  422c07:	50                   	push   eax
  422c08:	81 c1 4c 08 00 00    	add    ecx,0x84c
  422c0e:	53                   	push   ebx
  422c0f:	8d 96 4c 08 00 00    	lea    edx,[esi+0x84c]
  422c15:	51                   	push   ecx
  422c16:	52                   	push   edx
  422c17:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  422c1c:	e8 7f 7b 00 00       	call   0x42a7a0
  422c21:	80 3b 00             	cmp    BYTE PTR [ebx],0x0
  422c24:	75 0c                	jne    0x422c32
  422c26:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  422c2a:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
  422c30:	eb 45                	jmp    0x422c77
  422c32:	8a 4b 0c             	mov    cl,BYTE PTR [ebx+0xc]
  422c35:	33 d2                	xor    edx,edx
  422c37:	88 4e 04             	mov    BYTE PTR [esi+0x4],cl
  422c3a:	8d 6b 24             	lea    ebp,[ebx+0x24]
  422c3d:	8b 7d 00             	mov    edi,DWORD PTR [ebp+0x0]
  422c40:	85 ff                	test   edi,edi
  422c42:	7e 2a                	jle    0x422c6e
  422c44:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  422c48:	03 c2                	add    eax,edx
  422c4a:	8b 04 86             	mov    eax,DWORD PTR [esi+eax*4]
  422c4d:	83 f8 1b             	cmp    eax,0x1b
  422c50:	7f 1c                	jg     0x422c6e
  422c52:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  422c56:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  422c5d:	83 bc 81 34 0a 00 00 	cmp    DWORD PTR [ecx+eax*4+0xa34],0x0
  422c64:	00 
  422c65:	7f 07                	jg     0x422c6e
  422c67:	89 bc 81 cc 00 00 00 	mov    DWORD PTR [ecx+eax*4+0xcc],edi
  422c6e:	42                   	inc    edx
  422c6f:	83 c5 04             	add    ebp,0x4
  422c72:	83 fa 05             	cmp    edx,0x5
  422c75:	7c c6                	jl     0x422c3d
  422c77:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  422c7b:	8b 6c 24 18          	mov    ebp,DWORD PTR [esp+0x18]
  422c7f:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  422c83:	8b 8e c8 00 00 00    	mov    ecx,DWORD PTR [esi+0xc8]
  422c89:	40                   	inc    eax
  422c8a:	83 c5 04             	add    ebp,0x4
  422c8d:	83 c3 50             	add    ebx,0x50
  422c90:	83 c2 14             	add    edx,0x14
  422c93:	3b c1                	cmp    eax,ecx
  422c95:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  422c99:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  422c9d:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
  422ca1:	0f 8c 86 fe ff ff    	jl     0x422b2d
  422ca7:	5d                   	pop    ebp
  422ca8:	5b                   	pop    ebx
  422ca9:	5f                   	pop    edi
  422caa:	5e                   	pop    esi
  422cab:	83 c4 18             	add    esp,0x18
  422cae:	c3                   	ret
  422caf:	90                   	nop
  422cb0:	81 ec a8 00 00 00    	sub    esp,0xa8
  422cb6:	53                   	push   ebx
  422cb7:	55                   	push   ebp
  422cb8:	33 c0                	xor    eax,eax
  422cba:	56                   	push   esi
  422cbb:	33 db                	xor    ebx,ebx
  422cbd:	57                   	push   edi
  422cbe:	8b e9                	mov    ebp,ecx
  422cc0:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  422cc4:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  422cc8:	8d 74 24 18          	lea    esi,[esp+0x18]
  422ccc:	ba                   	.byte 0xba
  422ccd:	94                   	xchg   esp,eax
  422cce:	fd                   	std
  422ccf:	89                   	.byte 0x89
