; 固化48：七个单分支状态表现槽的字段读取与selector压入。
; 仅保存原版RPG.exe反汇编证据，不修改代码。

/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041cf50 <.text+0x1bf50>:
  41cf50:	00 8b 4a 1c e8 97    	add    BYTE PTR [ebx-0x6817e3b6],cl
  41cf56:	17                   	pop    ss
  41cf57:	02 00                	add    al,BYTE PTR [eax]
  41cf59:	8b 86 ec 01 00 00    	mov    eax,DWORD PTR [esi+0x1ec]
  41cf5f:	85 c0                	test   eax,eax
  41cf61:	7e 13                	jle    0x41cf76
  41cf63:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cf69:	6a 05                	push   0x5
  41cf6b:	8b 88 b0 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b0]
  41cf71:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cf74:	eb 11                	jmp    0x41cf87
  41cf76:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cf7c:	6a 00                	push   0x0
  41cf7e:	8b 82 b0 05 00 00    	mov    eax,DWORD PTR [edx+0x5b0]
  41cf84:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cf87:	e8 64 17 02 00       	call   0x43e6f0
  41cf8c:	8b 86 f0 01 00 00    	mov    eax,DWORD PTR [esi+0x1f0]
  41cf92:	85 c0                	test   eax,eax
  41cf94:	7e 13                	jle    0x41cfa9
  41cf96:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cf9c:	6a 12                	push   0x12
  41cf9e:	8b 91 b4 05 00 00    	mov    edx,DWORD PTR [ecx+0x5b4]
  41cfa4:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cfa7:	eb 11                	jmp    0x41cfba
  41cfa9:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cfaf:	6a 00                	push   0x0
  41cfb1:	8b 88 b4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b4]
  41cfb7:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cfba:	e8 31 17 02 00       	call   0x43e6f0
  41cfbf:	8b 86 24 02 00 00    	mov    eax,DWORD PTR [esi+0x224]
  41cfc5:	85 c0                	test   eax,eax
  41cfc7:	7e 13                	jle    0x41cfdc
  41cfc9:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cfcf:	6a 08                	push   0x8
  41cfd1:	8b 82 b8 05 00 00    	mov    eax,DWORD PTR [edx+0x5b8]
  41cfd7:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cfda:	eb 2e                	jmp    0x41d00a
  41cfdc:	8b 86 10 02 00 00    	mov    eax,DWORD PTR [esi+0x210]
  41cfe2:	85 c0                	test   eax,eax
  41cfe4:	7e 13                	jle    0x41cff9
  41cfe6:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cfec:	6a 13                	push   0x13
  41cfee:	8b 91 b8 05 00 00    	mov    edx,DWORD PTR [ecx+0x5b8]
  41cff4:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cff7:	eb 11                	jmp    0x41d00a
  41cff9:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cfff:	6a 00                	push   0x0
  41d001:	8b 88 b8 05 00 00    	mov    ecx,DWORD PTR [eax+0x5b8]
  41d007:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41d00a:	e8 e1 16 02 00       	call   0x43e6f0
  41d00f:	8b 86 14 02 00 00    	mov    eax,DWORD PTR [esi+0x214]
  41d015:	85 c0                	test   eax,eax
  41d017:	7e 13                	jle    0x41d02c
  41d019:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41d01f:	6a 14                	push   0x14
  41d021:	8b 82 bc 05 00 00    	mov    eax,DWORD PTR [edx+0x5bc]
  41d027:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d02a:	eb 11                	jmp    0x41d03d
  41d02c:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41d032:	6a 00                	push   0x0
  41d034:	8b 91 bc 05 00 00    	mov    edx,DWORD PTR [ecx+0x5bc]
  41d03a:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41d03d:	e8 ae 16 02 00       	call   0x43e6f0
  41d042:	8b 86 1c 02 00 00    	mov    eax,DWORD PTR [esi+0x21c]
  41d048:	85 c0                	test   eax,eax
  41d04a:	7e 13                	jle    0x41d05f
  41d04c:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41d052:	6a 15                	push   0x15
  41d054:	8b 88 c0 05 00 00    	mov    ecx,DWORD PTR [eax+0x5c0]
  41d05a:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41d05d:	eb 11                	jmp    0x41d070
  41d05f:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41d065:	6a 00                	push   0x0
  41d067:	8b 82 c0 05 00 00    	mov    eax,DWORD PTR [edx+0x5c0]
  41d06d:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d070:	e8 7b 16 02 00       	call   0x43e6f0
  41d075:	8b 86 20 02 00 00    	mov    eax,DWORD PTR [esi+0x220]
  41d07b:	85 c0                	test   eax,eax
  41d07d:	7e 13                	jle    0x41d092
  41d07f:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41d085:	6a 09                	push   0x9
  41d087:	8b 91 c4 05 00 00    	mov    edx,DWORD PTR [ecx+0x5c4]
  41d08d:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41d090:	eb 11                	jmp    0x41d0a3
  41d092:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41d098:	6a 00                	push   0x0
  41d09a:	8b 88 c4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5c4]
  41d0a0:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41d0a3:	e8 48 16 02 00       	call   0x43e6f0
  41d0a8:	8b 86 28 02 00 00    	mov    eax,DWORD PTR [esi+0x228]
  41d0ae:	85 c0                	test   eax,eax
  41d0b0:	7e 13                	jle    0x41d0c5
  41d0b2:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41d0b8:	6a 10                	push   0x10
  41d0ba:	8b 82 c8 05 00 00    	mov    eax,DWORD PTR [edx+0x5c8]
  41d0c0:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d0c3:	eb 11                	jmp    0x41d0d6
  41d0c5:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41d0cb:	6a 00                	push   0x0
  41d0cd:	8b 91 c8 05 00 00    	mov    edx,DWORD PTR [ecx+0x5c8]
  41d0d3:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41d0d6:	e8 15 16 02 00       	call   0x43e6f0
  41d0db:	8b 86 0c 02 00 00    	mov    eax,DWORD PTR [esi+0x20c]
  41d0e1:	85 c0                	test   eax,eax
  41d0e3:	7e 1e                	jle    0x41d103
  41d0e5:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41d0eb:	6a 16                	push   0x16
  41d0ed:	8b 88 cc 05 00 00    	mov    ecx,DWORD PTR [eax+0x5cc]
  41d0f3:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41d0f6:	e8 f5 15 02 00       	call   0x43e6f0
  41d0fb:	5f                   	pop    edi
  41d0fc:	5e                   	pop    esi
  41d0fd:	5d                   	pop    ebp
  41d0fe:	5b                   	pop    ebx
  41d0ff:	83 c4 1c             	add    esp,0x1c
  41d102:	c3                   	ret
  41d103:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41d109:	6a 00                	push   0x0
  41d10b:	8b 82 cc 05 00 00    	mov    eax,DWORD PTR [edx+0x5cc]
  41d111:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41d114:	e8 d7 15 02 00       	call   0x43e6f0
  41d119:	5f                   	pop    edi
  41d11a:	5e                   	pop    esi
  41d11b:	5d                   	pop    ebp
  41d11c:	5b                   	pop    ebx
  41d11d:	83 c4 1c             	add    esp,0x1c
