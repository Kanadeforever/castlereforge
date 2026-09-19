
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00421a00 <.text+0x20a00>:
  421a00:	83 ec 08             	sub    esp,0x8
  421a03:	53                   	push   ebx
  421a04:	55                   	push   ebp
  421a05:	56                   	push   esi
  421a06:	57                   	push   edi
  421a07:	8b f9                	mov    edi,ecx
  421a09:	33 db                	xor    ebx,ebx
  421a0b:	39 9f d0 0d 00 00    	cmp    DWORD PTR [edi+0xdd0],ebx
  421a11:	0f 8f 20 06 00 00    	jg     0x422037
  421a17:	39 9f 48 08 00 00    	cmp    DWORD PTR [edi+0x848],ebx
  421a1d:	0f 84 14 06 00 00    	je     0x422037
  421a23:	8b 97 e4 08 00 00    	mov    edx,DWORD PTR [edi+0x8e4]
  421a29:	33 f6                	xor    esi,esi
  421a2b:	3b d3                	cmp    edx,ebx
  421a2d:	0f 8e 17 01 00 00    	jle    0x421b4a
  421a33:	8b 0d b4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b4
  421a39:	b8 01 00 00 00       	mov    eax,0x1
  421a3e:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  421a41:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  421a45:	81 c1 6c 01 00 00    	add    ecx,0x16c
  421a4b:	39 51 fc             	cmp    DWORD PTR [ecx-0x4],edx
  421a4e:	74 04                	je     0x421a54
  421a50:	39 11                	cmp    DWORD PTR [ecx],edx
  421a52:	75 02                	jne    0x421a56
  421a54:	8b f0                	mov    esi,eax
  421a56:	40                   	inc    eax
  421a57:	83 c1 08             	add    ecx,0x8
  421a5a:	83 f8 05             	cmp    eax,0x5
  421a5d:	7e ec                	jle    0x421a4b
  421a5f:	83 fe 01             	cmp    esi,0x1
  421a62:	0f 8c e2 00 00 00    	jl     0x421b4a
  421a68:	83 fe 05             	cmp    esi,0x5
  421a6b:	0f 8f d9 00 00 00    	jg     0x421b4a
  421a71:	33 d2                	xor    edx,edx
  421a73:	33 c9                	xor    ecx,ecx
  421a75:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  421a79:	3b d3                	cmp    edx,ebx
  421a7b:	75 29                	jne    0x421aa6
  421a7d:	8d 46 13             	lea    eax,[esi+0x13]
  421a80:	8d 2c 81             	lea    ebp,[ecx+eax*4]
  421a83:	03 c5                	add    eax,ebp
  421a85:	8b 6c 24 14          	mov    ebp,DWORD PTR [esp+0x14]
  421a89:	8b 6c 85 00          	mov    ebp,DWORD PTR [ebp+eax*4+0x0]
  421a8d:	8b 87 48 08 00 00    	mov    eax,DWORD PTR [edi+0x848]
  421a93:	3b 28                	cmp    ebp,DWORD PTR [eax]
  421a95:	75 07                	jne    0x421a9e
  421a97:	8d 51 01             	lea    edx,[ecx+0x1]
  421a9a:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  421a9e:	41                   	inc    ecx
  421a9f:	83 f9 05             	cmp    ecx,0x5
  421aa2:	7c d5                	jl     0x421a79
  421aa4:	3b d3                	cmp    edx,ebx
  421aa6:	0f 8e 9e 00 00 00    	jle    0x421b4a
  421aac:	8d 42 ff             	lea    eax,[edx-0x1]
  421aaf:	99                   	cdq
  421ab0:	2b c2                	sub    eax,edx
  421ab2:	8b e8                	mov    ebp,eax
  421ab4:	d1 fd                	sar    ebp,1
  421ab6:	e8 aa fa 02 00       	call   0x451565
  421abb:	8b c8                	mov    ecx,eax
  421abd:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  421ac1:	99                   	cdq
  421ac2:	2b c2                	sub    eax,edx
  421ac4:	8b d1                	mov    edx,ecx
  421ac6:	8b c8                	mov    ecx,eax
  421ac8:	d1 f9                	sar    ecx,1
  421aca:	8b c2                	mov    eax,edx
  421acc:	2b cd                	sub    ecx,ebp
  421ace:	99                   	cdq
  421acf:	41                   	inc    ecx
  421ad0:	f7 f9                	idiv   ecx
  421ad2:	03 d5                	add    edx,ebp
  421ad4:	8b ac b7 98 09 00 00 	mov    ebp,DWORD PTR [edi+esi*4+0x998]
  421adb:	03 ea                	add    ebp,edx
  421add:	8b c5                	mov    eax,ebp
  421adf:	89 ac b7 98 09 00 00 	mov    DWORD PTR [edi+esi*4+0x998],ebp
  421ae6:	83 f8 64             	cmp    eax,0x64
  421ae9:	7e 0b                	jle    0x421af6
  421aeb:	c7 84 b7 98 09 00 00 	mov    DWORD PTR [edi+esi*4+0x998],0x64
  421af2:	64 00 00 00 
  421af6:	8b ac b7 84 09 00 00 	mov    ebp,DWORD PTR [edi+esi*4+0x984]
  421afd:	8b 84 b7 98 09 00 00 	mov    eax,DWORD PTR [edi+esi*4+0x998]
  421b04:	8d 4c 28 e7          	lea    ecx,[eax+ebp*1-0x19]
  421b08:	b8 67 66 66 66       	mov    eax,0x66666667
  421b0d:	f7 e9                	imul   ecx
  421b0f:	d1 fa                	sar    edx,1
  421b11:	8b ca                	mov    ecx,edx
  421b13:	c1 e9 1f             	shr    ecx,0x1f
  421b16:	03 d1                	add    edx,ecx
  421b18:	3b d3                	cmp    edx,ebx
  421b1a:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  421b1e:	7e 2a                	jle    0x421b4a
  421b20:	e8 40 fa 02 00       	call   0x451565
  421b25:	99                   	cdq
  421b26:	b9 64 00 00 00       	mov    ecx,0x64
  421b2b:	f7 f9                	idiv   ecx
  421b2d:	39 54 24 14          	cmp    DWORD PTR [esp+0x14],edx
  421b31:	7e 17                	jle    0x421b4a
  421b33:	8d 45 01             	lea    eax,[ebp+0x1]
  421b36:	3b c1                	cmp    eax,ecx
  421b38:	7e 02                	jle    0x421b3c
  421b3a:	8b c1                	mov    eax,ecx
  421b3c:	89 84 b7 84 09 00 00 	mov    DWORD PTR [edi+esi*4+0x984],eax
  421b43:	89 9c b7 98 09 00 00 	mov    DWORD PTR [edi+esi*4+0x998],ebx
  421b4a:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  421b4e:	83 f8 03             	cmp    eax,0x3
  421b51:	75 45                	jne    0x421b98
  421b53:	8b 97 48 08 00 00    	mov    edx,DWORD PTR [edi+0x848]
  421b59:	81 3a 43 01 00 00    	cmp    DWORD PTR [edx],0x143
  421b5f:	0f 85 d2 04 00 00    	jne    0x422037
  421b65:	8b 87 a4 0b 00 00    	mov    eax,DWORD PTR [edi+0xba4]
  421b6b:	89 9f 7c 08 00 00    	mov    DWORD PTR [edi+0x87c],ebx
  421b71:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  421b74:	88 99 58 03 00 00    	mov    BYTE PTR [ecx+0x358],bl
  421b7a:	c7 87 2c 07 00 00 ff 	mov    DWORD PTR [edi+0x72c],0xffffffff
  421b81:	ff ff ff 
  421b84:	c7 87 e4 0c 00 00 b9 	mov    DWORD PTR [edi+0xce4],0xbb9
  421b8b:	0b 00 00 
  421b8e:	5f                   	pop    edi
  421b8f:	5e                   	pop    esi
  421b90:	5d                   	pop    ebp
  421b91:	5b                   	pop    ebx
  421b92:	83 c4 08             	add    esp,0x8
  421b95:	c2 04 00             	ret    0x4
  421b98:	83 f8 02             	cmp    eax,0x2
  421b9b:	0f 85 96 04 00 00    	jne    0x422037
  421ba1:	8b 87 48 08 00 00    	mov    eax,DWORD PTR [edi+0x848]
  421ba7:	8b 08                	mov    ecx,DWORD PTR [eax]
  421ba9:	8d b1 d1 fe ff ff    	lea    esi,[ecx-0x12f]
  421baf:	81 fe a4 00 00 00    	cmp    esi,0xa4
  421bb5:	0f 87 7c 04 00 00    	ja     0x422037
  421bbb:	33 d2                	xor    edx,edx
  421bbd:	8a 96 74 20 42 00    	mov    dl,BYTE PTR [esi+0x422074]
  421bc3:	ff 24 95 44 20 42 00 	jmp    DWORD PTR [edx*4+0x422044]
  421bca:	8b 48 34             	mov    ecx,DWORD PTR [eax+0x34]
  421bcd:	b8 e1 7a 14 ae       	mov    eax,0xae147ae1
  421bd2:	0f af 8f 78 08 00 00 	imul   ecx,DWORD PTR [edi+0x878]
  421bd9:	f7 e9                	imul   ecx
  421bdb:	8b 8f 7c 08 00 00    	mov    ecx,DWORD PTR [edi+0x87c]
  421be1:	c1 fa 05             	sar    edx,0x5
  421be4:	8b c2                	mov    eax,edx
  421be6:	c1 e8 1f             	shr    eax,0x1f
  421be9:	03 d0                	add    edx,eax
  421beb:	03 ca                	add    ecx,edx
  421bed:	8b c1                	mov    eax,ecx
  421bef:	89 8f 7c 08 00 00    	mov    DWORD PTR [edi+0x87c],ecx
  421bf5:	3b c3                	cmp    eax,ebx
  421bf7:	0f 8f 3a 04 00 00    	jg     0x422037
  421bfd:	c7 87 7c 08 00 00 01 	mov    DWORD PTR [edi+0x87c],0x1
  421c04:	00 00 00 
  421c07:	5f                   	pop    edi
  421c08:	5e                   	pop    esi
  421c09:	5d                   	pop    ebp
  421c0a:	5b                   	pop    ebx
  421c0b:	83 c4 08             	add    esp,0x8
  421c0e:	c2 04 00             	ret    0x4
  421c11:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421c17:	3b c3                	cmp    eax,ebx
  421c19:	0f 8e 18 04 00 00    	jle    0x422037
  421c1f:	33 c9                	xor    ecx,ecx
  421c21:	3b c3                	cmp    eax,ebx
  421c23:	0f 8e 0e 04 00 00    	jle    0x422037
  421c29:	8d 97 88 00 00 00    	lea    edx,[edi+0x88]
  421c2f:	8b 02                	mov    eax,DWORD PTR [edx]
  421c31:	41                   	inc    ecx
  421c32:	83 c2 04             	add    edx,0x4
  421c35:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421c3c:	89 9e 88 0a 00 00    	mov    DWORD PTR [esi+0xa88],ebx
  421c42:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421c49:	89 9e 8c 0a 00 00    	mov    DWORD PTR [esi+0xa8c],ebx
  421c4f:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421c56:	89 9e 90 0a 00 00    	mov    DWORD PTR [esi+0xa90],ebx
  421c5c:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421c63:	89 9e 94 0a 00 00    	mov    DWORD PTR [esi+0xa94],ebx
  421c69:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421c70:	89 9e 98 0a 00 00    	mov    DWORD PTR [esi+0xa98],ebx
  421c76:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421c7d:	89 9e 9c 0a 00 00    	mov    DWORD PTR [esi+0xa9c],ebx
  421c83:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421c8a:	89 9e 70 0a 00 00    	mov    DWORD PTR [esi+0xa70],ebx
  421c90:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  421c97:	89 98 6c 0a 00 00    	mov    DWORD PTR [eax+0xa6c],ebx
  421c9d:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421ca3:	3b c8                	cmp    ecx,eax
  421ca5:	7c 88                	jl     0x421c2f
  421ca7:	5f                   	pop    edi
  421ca8:	5e                   	pop    esi
  421ca9:	5d                   	pop    ebp
  421caa:	5b                   	pop    ebx
  421cab:	83 c4 08             	add    esp,0x8
  421cae:	c2 04 00             	ret    0x4
  421cb1:	39 9f c8 00 00 00    	cmp    DWORD PTR [edi+0xc8],ebx
  421cb7:	0f 8e 7a 03 00 00    	jle    0x422037
  421cbd:	8b 8f 88 00 00 00    	mov    ecx,DWORD PTR [edi+0x88]
  421cc3:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  421cc8:	8b 14 8d 94 fd 89 00 	mov    edx,DWORD PTR [ecx*4+0x89fd94]
  421ccf:	8b 88 f4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5f4]
  421cd5:	81 c2 4c 08 00 00    	add    edx,0x84c
  421cdb:	52                   	push   edx
  421cdc:	e8 2f ee 00 00       	call   0x430b10
  421ce1:	5f                   	pop    edi
  421ce2:	5e                   	pop    esi
  421ce3:	5d                   	pop    ebp
  421ce4:	5b                   	pop    ebx
  421ce5:	83 c4 08             	add    esp,0x8
  421ce8:	c2 04 00             	ret    0x4
  421ceb:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421cf1:	3b c3                	cmp    eax,ebx
  421cf3:	0f 8e 3e 03 00 00    	jle    0x422037
  421cf9:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
  421cfd:	8d af 88 00 00 00    	lea    ebp,[edi+0x88]
  421d03:	8b 75 00             	mov    esi,DWORD PTR [ebp+0x0]
  421d06:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421d0d:	39 98 98 0b 00 00    	cmp    DWORD PTR [eax+0xb98],ebx
  421d13:	75 2b                	jne    0x421d40
  421d15:	6a 01                	push   0x1
  421d17:	6a 0a                	push   0xa
  421d19:	8d 88 a8 0c 00 00    	lea    ecx,[eax+0xca8]
  421d1f:	e8 ec 22 00 00       	call   0x424010
  421d24:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421d2b:	99                   	cdq
  421d2c:	2b c2                	sub    eax,edx
  421d2e:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  421d34:	d1 f8                	sar    eax,1
  421d36:	50                   	push   eax
  421d37:	6a 01                	push   0x1
  421d39:	6a 0a                	push   0xa
  421d3b:	e8 50 22 00 00       	call   0x423f90
  421d40:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  421d44:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  421d4a:	40                   	inc    eax
  421d4b:	83 c5 04             	add    ebp,0x4
  421d4e:	3b c1                	cmp    eax,ecx
  421d50:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  421d54:	7c ad                	jl     0x421d03
  421d56:	5f                   	pop    edi
  421d57:	5e                   	pop    esi
  421d58:	5d                   	pop    ebp
  421d59:	5b                   	pop    ebx
  421d5a:	83 c4 08             	add    esp,0x8
  421d5d:	c2 04 00             	ret    0x4
  421d60:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421d66:	3b c3                	cmp    eax,ebx
  421d68:	0f 8e c9 02 00 00    	jle    0x422037
  421d6e:	33 ed                	xor    ebp,ebp
  421d70:	3b c3                	cmp    eax,ebx
  421d72:	0f 8e bf 02 00 00    	jle    0x422037
  421d78:	8d b7 88 00 00 00    	lea    esi,[edi+0x88]
  421d7e:	8b 06                	mov    eax,DWORD PTR [esi]
  421d80:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  421d87:	39 98 98 0b 00 00    	cmp    DWORD PTR [eax+0xb98],ebx
  421d8d:	75 10                	jne    0x421d9f
  421d8f:	53                   	push   ebx
  421d90:	6a 01                	push   0x1
  421d92:	6a 0a                	push   0xa
  421d94:	8d 88 a8 0c 00 00    	lea    ecx,[eax+0xca8]
  421d9a:	e8 f1 21 00 00       	call   0x423f90
  421d9f:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421da5:	45                   	inc    ebp
  421da6:	83 c6 04             	add    esi,0x4
  421da9:	3b e8                	cmp    ebp,eax
  421dab:	7c d1                	jl     0x421d7e
  421dad:	5f                   	pop    edi
  421dae:	5e                   	pop    esi
  421daf:	5d                   	pop    ebp
  421db0:	5b                   	pop    ebx
  421db1:	83 c4 08             	add    esp,0x8
  421db4:	c2 04 00             	ret    0x4
  421db7:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421dbd:	3b c3                	cmp    eax,ebx
  421dbf:	0f 8e 72 02 00 00    	jle    0x422037
  421dc5:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
  421dc9:	8d af 88 00 00 00    	lea    ebp,[edi+0x88]
  421dcf:	8b 75 00             	mov    esi,DWORD PTR [ebp+0x0]
  421dd2:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421dd9:	39 98 98 0b 00 00    	cmp    DWORD PTR [eax+0xb98],ebx
  421ddf:	75 35                	jne    0x421e16
  421de1:	6a 01                	push   0x1
  421de3:	6a 0c                	push   0xc
  421de5:	8d 88 a8 0c 00 00    	lea    ecx,[eax+0xca8]
  421deb:	e8 20 22 00 00       	call   0x424010
  421df0:	8b c8                	mov    ecx,eax
  421df2:	99                   	cdq
  421df3:	2b c2                	sub    eax,edx
  421df5:	d1 f8                	sar    eax,1
  421df7:	f7 d8                	neg    eax
  421df9:	03 c8                	add    ecx,eax
  421dfb:	79 02                	jns    0x421dff
  421dfd:	33 c9                	xor    ecx,ecx
  421dff:	51                   	push   ecx
  421e00:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421e07:	6a 01                	push   0x1
  421e09:	6a 0a                	push   0xa
  421e0b:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  421e11:	e8 7a 21 00 00       	call   0x423f90
  421e16:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  421e1a:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  421e20:	40                   	inc    eax
  421e21:	83 c5 04             	add    ebp,0x4
  421e24:	3b c1                	cmp    eax,ecx
  421e26:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  421e2a:	7c a3                	jl     0x421dcf
  421e2c:	5f                   	pop    edi
  421e2d:	5e                   	pop    esi
  421e2e:	5d                   	pop    ebp
  421e2f:	5b                   	pop    ebx
  421e30:	83 c4 08             	add    esp,0x8
  421e33:	c2 04 00             	ret    0x4
  421e36:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421e3c:	3b c3                	cmp    eax,ebx
  421e3e:	0f 8e f3 01 00 00    	jle    0x422037
  421e44:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
  421e48:	8d af 88 00 00 00    	lea    ebp,[edi+0x88]
  421e4e:	8b 75 00             	mov    esi,DWORD PTR [ebp+0x0]
  421e51:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421e58:	39 98 98 0b 00 00    	cmp    DWORD PTR [eax+0xb98],ebx
  421e5e:	0f 85 80 00 00 00    	jne    0x421ee4
  421e64:	6a 01                	push   0x1
  421e66:	6a 0c                	push   0xc
  421e68:	8d 88 a8 0c 00 00    	lea    ecx,[eax+0xca8]
  421e6e:	e8 9d 21 00 00       	call   0x424010
  421e73:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421e7a:	50                   	push   eax
  421e7b:	6a 01                	push   0x1
  421e7d:	6a 0a                	push   0xa
  421e7f:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  421e85:	e8 06 21 00 00       	call   0x423f90
  421e8a:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421e91:	6a 02                	push   0x2
  421e93:	6a 0c                	push   0xc
  421e95:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  421e9b:	e8 70 21 00 00       	call   0x424010
  421ea0:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421ea7:	50                   	push   eax
  421ea8:	6a 02                	push   0x2
  421eaa:	6a 0a                	push   0xa
  421eac:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  421eb2:	e8 d9 20 00 00       	call   0x423f90
  421eb7:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421ebe:	6a 03                	push   0x3
  421ec0:	6a 0c                	push   0xc
  421ec2:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  421ec8:	e8 43 21 00 00       	call   0x424010
  421ecd:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421ed4:	50                   	push   eax
  421ed5:	6a 03                	push   0x3
  421ed7:	6a 0a                	push   0xa
  421ed9:	81 c1 a8 0c 00 00    	add    ecx,0xca8
  421edf:	e8 ac 20 00 00       	call   0x423f90
  421ee4:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  421ee8:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  421eee:	40                   	inc    eax
  421eef:	83 c5 04             	add    ebp,0x4
  421ef2:	3b c1                	cmp    eax,ecx
  421ef4:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  421ef8:	0f 8c 50 ff ff ff    	jl     0x421e4e
  421efe:	5f                   	pop    edi
  421eff:	5e                   	pop    esi
  421f00:	5d                   	pop    ebp
  421f01:	5b                   	pop    ebx
  421f02:	83 c4 08             	add    esp,0x8
  421f05:	c2 04 00             	ret    0x4
  421f08:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421f0e:	3b c3                	cmp    eax,ebx
  421f10:	0f 8e 21 01 00 00    	jle    0x422037
  421f16:	33 c9                	xor    ecx,ecx
  421f18:	3b c3                	cmp    eax,ebx
  421f1a:	0f 8e 17 01 00 00    	jle    0x422037
  421f20:	8d 97 88 00 00 00    	lea    edx,[edi+0x88]
  421f26:	8b 02                	mov    eax,DWORD PTR [edx]
  421f28:	41                   	inc    ecx
  421f29:	83 c2 04             	add    edx,0x4
  421f2c:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f33:	89 9e 34 0a 00 00    	mov    DWORD PTR [esi+0xa34],ebx
  421f39:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f40:	89 9e 38 0a 00 00    	mov    DWORD PTR [esi+0xa38],ebx
  421f46:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f4d:	89 9e 3c 0a 00 00    	mov    DWORD PTR [esi+0xa3c],ebx
  421f53:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f5a:	89 9e 40 0a 00 00    	mov    DWORD PTR [esi+0xa40],ebx
  421f60:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f67:	89 9e 44 0a 00 00    	mov    DWORD PTR [esi+0xa44],ebx
  421f6d:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f74:	89 9e 48 0a 00 00    	mov    DWORD PTR [esi+0xa48],ebx
  421f7a:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f81:	89 9e 4c 0a 00 00    	mov    DWORD PTR [esi+0xa4c],ebx
  421f87:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f8e:	89 9e 50 0a 00 00    	mov    DWORD PTR [esi+0xa50],ebx
  421f94:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421f9b:	89 9e 54 0a 00 00    	mov    DWORD PTR [esi+0xa54],ebx
  421fa1:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421fa8:	89 9e 58 0a 00 00    	mov    DWORD PTR [esi+0xa58],ebx
  421fae:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421fb5:	89 9e 5c 0a 00 00    	mov    DWORD PTR [esi+0xa5c],ebx
  421fbb:	8b 34 85 94 fd 89 00 	mov    esi,DWORD PTR [eax*4+0x89fd94]
  421fc2:	89 9e 60 0a 00 00    	mov    DWORD PTR [esi+0xa60],ebx
  421fc8:	8b 04 85 94 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd94]
  421fcf:	89 98 68 0a 00 00    	mov    DWORD PTR [eax+0xa68],ebx
  421fd5:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  421fdb:	3b c8                	cmp    ecx,eax
  421fdd:	0f 8c 43 ff ff ff    	jl     0x421f26
  421fe3:	5f                   	pop    edi
  421fe4:	5e                   	pop    esi
  421fe5:	5d                   	pop    ebp
  421fe6:	5b                   	pop    ebx
  421fe7:	83 c4 08             	add    esp,0x8
  421fea:	c2 04 00             	ret    0x4
  421fed:	39 9f c8 00 00 00    	cmp    DWORD PTR [edi+0xc8],ebx
  421ff3:	7e 42                	jle    0x422037
  421ff5:	8b bf 88 00 00 00    	mov    edi,DWORD PTR [edi+0x88]
  421ffb:	8b 0c bd 94 fd 89 00 	mov    ecx,DWORD PTR [edi*4+0x89fd94]
  422002:	c7 81 7c 08 00 00 01 	mov    DWORD PTR [ecx+0x87c],0x1
  422009:	00 00 00 
  42200c:	8b 14 bd 94 fd 89 00 	mov    edx,DWORD PTR [edi*4+0x89fd94]
  422013:	5f                   	pop    edi
  422014:	5e                   	pop    esi
  422015:	5d                   	pop    ebp
  422016:	c7 82 74 0a 00 00 40 	mov    DWORD PTR [edx+0xa74],0xf4240
  42201d:	42 0f 00 
  422020:	5b                   	pop    ebx
  422021:	83 c4 08             	add    esp,0x8
  422024:	c2 04 00             	ret    0x4
  422027:	6a 01                	push   0x1
  422029:	eb 01                	jmp    0x42202c
  42202b:	53                   	push   ebx
  42202c:	8b 0d 80 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd80
  422032:	e8 a9 1f 02 00       	call   0x443fe0
  422037:	5f                   	pop    edi
  422038:	5e                   	pop    esi
  422039:	5d                   	pop    ebp
  42203a:	5b                   	pop    ebx
  42203b:	83 c4 08             	add    esp,0x8
  42203e:	c2 04              	ret    0x4
