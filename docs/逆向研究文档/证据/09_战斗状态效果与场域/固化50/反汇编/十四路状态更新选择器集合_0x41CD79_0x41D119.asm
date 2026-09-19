
/mnt/data/work50/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041cd79 <.text+0x1bd79>:
  41cd79:	8b 86 e8 01 00 00    	mov    eax,DWORD PTR [esi+0x1e8]
  41cd7f:	85 c0                	test   eax,eax
  41cd81:	7e 13                	jle    0x41cd96
  41cd83:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cd89:	6a 01                	push   0x1
  41cd8b:	8b 88 98 05 00 00    	mov    ecx,DWORD PTR [eax+0x598]
  41cd91:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cd94:	eb 2e                	jmp    0x41cdc4
  41cd96:	8b 86 3c 02 00 00    	mov    eax,DWORD PTR [esi+0x23c]
  41cd9c:	85 c0                	test   eax,eax
  41cd9e:	7e 13                	jle    0x41cdb3
  41cda0:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cda6:	6a 0c                	push   0xc
  41cda8:	8b 82 98 05 00 00    	mov    eax,DWORD PTR [edx+0x598]
  41cdae:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cdb1:	eb 11                	jmp    0x41cdc4
  41cdb3:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cdb9:	6a 00                	push   0x0
  41cdbb:	8b 91 98 05 00 00    	mov    edx,DWORD PTR [ecx+0x598]
  41cdc1:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cdc4:	e8 27 19 02 00       	call   0x43e6f0
  41cdc9:	8b 86 fc 01 00 00    	mov    eax,DWORD PTR [esi+0x1fc]
  41cdcf:	85 c0                	test   eax,eax
  41cdd1:	7e 13                	jle    0x41cde6
  41cdd3:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cdd9:	6a 02                	push   0x2
  41cddb:	8b 88 9c 05 00 00    	mov    ecx,DWORD PTR [eax+0x59c]
  41cde1:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cde4:	eb 2e                	jmp    0x41ce14
  41cde6:	8b 86 48 02 00 00    	mov    eax,DWORD PTR [esi+0x248]
  41cdec:	85 c0                	test   eax,eax
  41cdee:	7e 13                	jle    0x41ce03
  41cdf0:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cdf6:	6a 03                	push   0x3
  41cdf8:	8b 82 9c 05 00 00    	mov    eax,DWORD PTR [edx+0x59c]
  41cdfe:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41ce01:	eb 11                	jmp    0x41ce14
  41ce03:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41ce09:	6a 00                	push   0x0
  41ce0b:	8b 91 9c 05 00 00    	mov    edx,DWORD PTR [ecx+0x59c]
  41ce11:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41ce14:	e8 d7 18 02 00       	call   0x43e6f0
  41ce19:	8b 86 f4 01 00 00    	mov    eax,DWORD PTR [esi+0x1f4]
  41ce1f:	85 c0                	test   eax,eax
  41ce21:	7e 13                	jle    0x41ce36
  41ce23:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41ce29:	6a 07                	push   0x7
  41ce2b:	8b 88 a0 05 00 00    	mov    ecx,DWORD PTR [eax+0x5a0]
  41ce31:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41ce34:	eb 2e                	jmp    0x41ce64
  41ce36:	8b 86 40 02 00 00    	mov    eax,DWORD PTR [esi+0x240]
  41ce3c:	85 c0                	test   eax,eax
  41ce3e:	7e 13                	jle    0x41ce53
  41ce40:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41ce46:	6a 06                	push   0x6
  41ce48:	8b 82 a0 05 00 00    	mov    eax,DWORD PTR [edx+0x5a0]
  41ce4e:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41ce51:	eb 11                	jmp    0x41ce64
  41ce53:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41ce59:	6a 00                	push   0x0
  41ce5b:	8b 91 a0 05 00 00    	mov    edx,DWORD PTR [ecx+0x5a0]
  41ce61:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41ce64:	e8 87 18 02 00       	call   0x43e6f0
  41ce69:	8b 86 f8 01 00 00    	mov    eax,DWORD PTR [esi+0x1f8]
  41ce6f:	85 c0                	test   eax,eax
  41ce71:	7e 13                	jle    0x41ce86
  41ce73:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41ce79:	6a 11                	push   0x11
  41ce7b:	8b 88 a4 05 00 00    	mov    ecx,DWORD PTR [eax+0x5a4]
  41ce81:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41ce84:	eb 2e                	jmp    0x41ceb4
  41ce86:	8b 86 44 02 00 00    	mov    eax,DWORD PTR [esi+0x244]
  41ce8c:	85 c0                	test   eax,eax
  41ce8e:	7e 13                	jle    0x41cea3
  41ce90:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41ce96:	6a 04                	push   0x4
  41ce98:	8b 82 a4 05 00 00    	mov    eax,DWORD PTR [edx+0x5a4]
  41ce9e:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cea1:	eb 11                	jmp    0x41ceb4
  41cea3:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cea9:	6a 00                	push   0x0
  41ceab:	8b 91 a4 05 00 00    	mov    edx,DWORD PTR [ecx+0x5a4]
  41ceb1:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41ceb4:	e8 37 18 02 00       	call   0x43e6f0
  41ceb9:	8b 86 00 02 00 00    	mov    eax,DWORD PTR [esi+0x200]
  41cebf:	85 c0                	test   eax,eax
  41cec1:	7e 13                	jle    0x41ced6
  41cec3:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cec9:	6a 0e                	push   0xe
  41cecb:	8b 88 a8 05 00 00    	mov    ecx,DWORD PTR [eax+0x5a8]
  41ced1:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41ced4:	eb 2e                	jmp    0x41cf04
  41ced6:	8b 86 4c 02 00 00    	mov    eax,DWORD PTR [esi+0x24c]
  41cedc:	85 c0                	test   eax,eax
  41cede:	7e 13                	jle    0x41cef3
  41cee0:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cee6:	6a 0d                	push   0xd
  41cee8:	8b 82 a8 05 00 00    	mov    eax,DWORD PTR [edx+0x5a8]
  41ceee:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cef1:	eb 11                	jmp    0x41cf04
  41cef3:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cef9:	6a 00                	push   0x0
  41cefb:	8b 91 a8 05 00 00    	mov    edx,DWORD PTR [ecx+0x5a8]
  41cf01:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cf04:	e8 e7 17 02 00       	call   0x43e6f0
  41cf09:	8b 86 04 02 00 00    	mov    eax,DWORD PTR [esi+0x204]
  41cf0f:	85 c0                	test   eax,eax
  41cf11:	7e 13                	jle    0x41cf26
  41cf13:	8b 87 c0 05 00 00    	mov    eax,DWORD PTR [edi+0x5c0]
  41cf19:	6a 0a                	push   0xa
  41cf1b:	8b 88 ac 05 00 00    	mov    ecx,DWORD PTR [eax+0x5ac]
  41cf21:	8b 49 1c             	mov    ecx,DWORD PTR [ecx+0x1c]
  41cf24:	eb 2e                	jmp    0x41cf54
  41cf26:	8b 86 50 02 00 00    	mov    eax,DWORD PTR [esi+0x250]
  41cf2c:	85 c0                	test   eax,eax
  41cf2e:	7e 13                	jle    0x41cf43
  41cf30:	8b 97 c0 05 00 00    	mov    edx,DWORD PTR [edi+0x5c0]
  41cf36:	6a 0b                	push   0xb
  41cf38:	8b 82 ac 05 00 00    	mov    eax,DWORD PTR [edx+0x5ac]
  41cf3e:	8b 48 1c             	mov    ecx,DWORD PTR [eax+0x1c]
  41cf41:	eb 11                	jmp    0x41cf54
  41cf43:	8b 8f c0 05 00 00    	mov    ecx,DWORD PTR [edi+0x5c0]
  41cf49:	6a 00                	push   0x0
  41cf4b:	8b 91 ac 05 00 00    	mov    edx,DWORD PTR [ecx+0x5ac]
  41cf51:	8b 4a 1c             	mov    ecx,DWORD PTR [edx+0x1c]
  41cf54:	e8 97 17 02 00       	call   0x43e6f0
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
