
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420ed0 <.text+0x1fed0>:
  420ed0:	83 ec 10             	sub    esp,0x10
  420ed3:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  420ed7:	53                   	push   ebx
  420ed8:	55                   	push   ebp
  420ed9:	56                   	push   esi
  420eda:	57                   	push   edi
  420edb:	33 ed                	xor    ebp,ebp
  420edd:	8b f9                	mov    edi,ecx
  420edf:	3b c5                	cmp    eax,ebp
  420ee1:	89 7c 24 14          	mov    DWORD PTR [esp+0x14],edi
  420ee5:	0f 85 a5 00 00 00    	jne    0x420f90
  420eeb:	e8 c0 1b 00 00       	call   0x422ab0
  420ef0:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  420ef6:	c7 87 e4 0c 00 00 bb 	mov    DWORD PTR [edi+0xce4],0xbbb
  420efd:	0b 00 00 
  420f00:	3b c5                	cmp    eax,ebp
  420f02:	0f 8e 7a 07 00 00    	jle    0x421682
  420f08:	33 f6                	xor    esi,esi
  420f0a:	3b c5                	cmp    eax,ebp
  420f0c:	0f 8e 70 07 00 00    	jle    0x421682
  420f12:	8d 87 88 00 00 00    	lea    eax,[edi+0x88]
  420f18:	bb a1 0f 00 00       	mov    ebx,0xfa1
  420f1d:	bd 06 00 00 00       	mov    ebp,0x6
  420f22:	8b 08                	mov    ecx,DWORD PTR [eax]
  420f24:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  420f2b:	85 c9                	test   ecx,ecx
  420f2d:	74 49                	je     0x420f78
  420f2f:	8b 97 d0 0d 00 00    	mov    edx,DWORD PTR [edi+0xdd0]
  420f35:	85 d2                	test   edx,edx
  420f37:	7e 19                	jle    0x420f52
  420f39:	89 99 e8 0c 00 00    	mov    DWORD PTR [ecx+0xce8],ebx
  420f3f:	8b 10                	mov    edx,DWORD PTR [eax]
  420f41:	8b 0c 95 94 fd 89 00 	mov    ecx,DWORD PTR [edx*4+0x89fd94]
  420f48:	8b 17                	mov    edx,DWORD PTR [edi]
  420f4a:	89 91 e0 0d 00 00    	mov    DWORD PTR [ecx+0xde0],edx
  420f50:	eb 26                	jmp    0x420f78
  420f52:	8b 97 48 08 00 00    	mov    edx,DWORD PTR [edi+0x848]
  420f58:	85 d2                	test   edx,edx
  420f5a:	74 1c                	je     0x420f78
  420f5c:	39 6a 18             	cmp    DWORD PTR [edx+0x18],ebp
  420f5f:	74 17                	je     0x420f78
  420f61:	89 99 e8 0c 00 00    	mov    DWORD PTR [ecx+0xce8],ebx
  420f67:	8b 08                	mov    ecx,DWORD PTR [eax]
  420f69:	8b 14 8d 94 fd 89 00 	mov    edx,DWORD PTR [ecx*4+0x89fd94]
  420f70:	8b 0f                	mov    ecx,DWORD PTR [edi]
  420f72:	89 8a e0 0d 00 00    	mov    DWORD PTR [edx+0xde0],ecx
  420f78:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  420f7e:	46                   	inc    esi
  420f7f:	83 c0 04             	add    eax,0x4
  420f82:	3b f1                	cmp    esi,ecx
  420f84:	7c 9c                	jl     0x420f22
  420f86:	5f                   	pop    edi
  420f87:	5e                   	pop    esi
  420f88:	5d                   	pop    ebp
  420f89:	5b                   	pop    ebx
  420f8a:	83 c4 10             	add    esp,0x10
  420f8d:	c2 04 00             	ret    0x4
  420f90:	83 f8 01             	cmp    eax,0x1
  420f93:	0f 85 35 03 00 00    	jne    0x4212ce
  420f99:	8b 87 c8 00 00 00    	mov    eax,DWORD PTR [edi+0xc8]
  420f9f:	89 af 68 01 00 00    	mov    DWORD PTR [edi+0x168],ebp
  420fa5:	3b c5                	cmp    eax,ebp
  420fa7:	89 af 6c 01 00 00    	mov    DWORD PTR [edi+0x16c],ebp
  420fad:	c7 87 e4 0c 00 00 b9 	mov    DWORD PTR [edi+0xce4],0xbb9
  420fb4:	0b 00 00 
  420fb7:	0f 8e 48 01 00 00    	jle    0x421105
  420fbd:	8d 97 88 00 00 00    	lea    edx,[edi+0x88]
  420fc3:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  420fc7:	8d 9f 78 01 00 00    	lea    ebx,[edi+0x178]
  420fcd:	89 54 24 24          	mov    DWORD PTR [esp+0x24],edx
  420fd1:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  420fd5:	8b 30                	mov    esi,DWORD PTR [eax]
  420fd7:	3b f5                	cmp    esi,ebp
  420fd9:	0f 8c 01 01 00 00    	jl     0x4210e0
  420fdf:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  420fe6:	c7 81 e8 0c 00 00 a0 	mov    DWORD PTR [ecx+0xce8],0xfa0
  420fed:	0f 00 00 
  420ff0:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  420ff7:	c7 82 e0 0d 00 00 ff 	mov    DWORD PTR [edx+0xde0],0xffffffff
  420ffe:	ff ff ff 
  421001:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421008:	8b 81 7c 08 00 00    	mov    eax,DWORD PTR [ecx+0x87c]
  42100e:	3b c5                	cmp    eax,ebp
  421010:	7e 18                	jle    0x42102a
  421012:	2b 43 fc             	sub    eax,DWORD PTR [ebx-0x4]
  421015:	89 81 7c 08 00 00    	mov    DWORD PTR [ecx+0x87c],eax
  42101b:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421022:	8b 0b                	mov    ecx,DWORD PTR [ebx]
  421024:	29 88 84 08 00 00    	sub    DWORD PTR [eax+0x884],ecx
  42102a:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421031:	8b 81 7c 08 00 00    	mov    eax,DWORD PTR [ecx+0x87c]
  421037:	3b c5                	cmp    eax,ebp
  421039:	7f 0f                	jg     0x42104a
  42103b:	39 a9 2c 07 00 00    	cmp    DWORD PTR [ecx+0x72c],ebp
  421041:	7e 07                	jle    0x42104a
  421043:	e8 a8 21 00 00       	call   0x4231f0
  421048:	eb 10                	jmp    0x42105a
  42104a:	8b 91 78 08 00 00    	mov    edx,DWORD PTR [ecx+0x878]
  421050:	3b c2                	cmp    eax,edx
  421052:	7e 06                	jle    0x42105a
  421054:	89 91 7c 08 00 00    	mov    DWORD PTR [ecx+0x87c],edx
  42105a:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421061:	39 a8 7c 08 00 00    	cmp    DWORD PTR [eax+0x87c],ebp
  421067:	7d 06                	jge    0x42106f
  421069:	89 a8 7c 08 00 00    	mov    DWORD PTR [eax+0x87c],ebp
  42106f:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421076:	39 a8 84 08 00 00    	cmp    DWORD PTR [eax+0x884],ebp
  42107c:	7d 06                	jge    0x421084
  42107e:	89 a8 84 08 00 00    	mov    DWORD PTR [eax+0x884],ebp
  421084:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  42108b:	8b 88 78 08 00 00    	mov    ecx,DWORD PTR [eax+0x878]
  421091:	8b 90 7c 08 00 00    	mov    edx,DWORD PTR [eax+0x87c]
  421097:	3b d1                	cmp    edx,ecx
  421099:	7e 06                	jle    0x4210a1
  42109b:	89 88 7c 08 00 00    	mov    DWORD PTR [eax+0x87c],ecx
  4210a1:	8b 34 b5 94 fd 89 00 	mov    esi,DWORD PTR [esi*4+0x89fd94]
  4210a8:	8b 86 80 08 00 00    	mov    eax,DWORD PTR [esi+0x880]
  4210ae:	8b 8e 84 08 00 00    	mov    ecx,DWORD PTR [esi+0x884]
  4210b4:	3b c8                	cmp    ecx,eax
  4210b6:	7e 06                	jle    0x4210be
  4210b8:	89 86 84 08 00 00    	mov    DWORD PTR [esi+0x884],eax
  4210be:	8b 53 30             	mov    edx,DWORD PTR [ebx+0x30]
  4210c1:	8b b7 68 01 00 00    	mov    esi,DWORD PTR [edi+0x168]
  4210c7:	8b 8f 6c 01 00 00    	mov    ecx,DWORD PTR [edi+0x16c]
  4210cd:	03 f2                	add    esi,edx
  4210cf:	89 b7 68 01 00 00    	mov    DWORD PTR [edi+0x168],esi
  4210d5:	8b 43 34             	mov    eax,DWORD PTR [ebx+0x34]
  4210d8:	03 c8                	add    ecx,eax
  4210da:	89 8f 6c 01 00 00    	mov    DWORD PTR [edi+0x16c],ecx
  4210e0:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4210e4:	8b 74 24 24          	mov    esi,DWORD PTR [esp+0x24]
  4210e8:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  4210ee:	40                   	inc    eax
  4210ef:	83 c6 04             	add    esi,0x4
  4210f2:	83 c3 50             	add    ebx,0x50
  4210f5:	3b c1                	cmp    eax,ecx
  4210f7:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4210fb:	89 74 24 24          	mov    DWORD PTR [esp+0x24],esi
  4210ff:	0f 8c cc fe ff ff    	jl     0x420fd1
  421105:	8b 87 9c 0b 00 00    	mov    eax,DWORD PTR [edi+0xb9c]
  42110b:	8b 0d f0 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fdf0
  421111:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  421117:	c1 e0 02             	shl    eax,0x2
  42111a:	8b 34 01             	mov    esi,DWORD PTR [ecx+eax*1]
  42111d:	8b 1c 02             	mov    ebx,DWORD PTR [edx+eax*1]
  421120:	8b 87 68 01 00 00    	mov    eax,DWORD PTR [edi+0x168]
  421126:	3b c5                	cmp    eax,ebp
  421128:	7e 19                	jle    0x421143
  42112a:	6a 01                	push   0x1
  42112c:	55                   	push   ebp
  42112d:	f7 d8                	neg    eax
  42112f:	50                   	push   eax
  421130:	8d 43 ce             	lea    eax,[ebx-0x32]
  421133:	8d 4e ec             	lea    ecx,[esi-0x14]
  421136:	50                   	push   eax
  421137:	51                   	push   ecx
  421138:	8b 0d 14 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2414
  42113e:	e8 ad 5c 02 00       	call   0x446df0
  421143:	8b 87 6c 01 00 00    	mov    eax,DWORD PTR [edi+0x16c]
  421149:	3b c5                	cmp    eax,ebp
  42114b:	7e 18                	jle    0x421165
  42114d:	8b 0d 14 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2414
  421153:	55                   	push   ebp
  421154:	f7 d8                	neg    eax
  421156:	55                   	push   ebp
  421157:	83 c3 b5             	add    ebx,0xffffffb5
  42115a:	50                   	push   eax
  42115b:	83 c6 ec             	add    esi,0xffffffec
  42115e:	53                   	push   ebx
  42115f:	56                   	push   esi
  421160:	e8 8b 5c 02 00       	call   0x446df0
