
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
  421b4e:	83 f8              	cmp    eax,0x3
