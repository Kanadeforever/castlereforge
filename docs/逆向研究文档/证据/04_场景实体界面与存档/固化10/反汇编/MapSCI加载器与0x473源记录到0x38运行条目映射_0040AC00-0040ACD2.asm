
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040ac00 <.text+0x9c00>:
  40ac00:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40ac06:	6a ff                	push   0xffffffff
  40ac08:	68 eb d0 45 00       	push   0x45d0eb
  40ac0d:	50                   	push   eax
  40ac0e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40ac15:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  40ac19:	53                   	push   ebx
  40ac1a:	55                   	push   ebp
  40ac1b:	56                   	push   esi
  40ac1c:	8b f1                	mov    esi,ecx
  40ac1e:	57                   	push   edi
  40ac1f:	6a 01                	push   0x1
  40ac21:	8d 6e 0c             	lea    ebp,[esi+0xc]
  40ac24:	55                   	push   ebp
  40ac25:	50                   	push   eax
  40ac26:	e8 85 79 ff ff       	call   0x4025b0
  40ac2b:	8b c8                	mov    ecx,eax
  40ac2d:	b8 d3 8d 13 73       	mov    eax,0x73138dd3
  40ac32:	f7 e1                	mul    ecx
  40ac34:	8b fa                	mov    edi,edx
  40ac36:	c1 ef 09             	shr    edi,0x9
  40ac39:	89 3e                	mov    DWORD PTR [esi],edi
  40ac3b:	8d 14 fd 00 00 00 00 	lea    edx,[edi*8+0x0]
  40ac42:	2b d7                	sub    edx,edi
  40ac44:	8d 04 d5 04 00 00 00 	lea    eax,[edx*8+0x4]
  40ac4b:	50                   	push   eax
  40ac4c:	e8 0e 6a 04 00       	call   0x45165f
  40ac51:	83 c4 10             	add    esp,0x10
  40ac54:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  40ac58:	33 db                	xor    ebx,ebx
  40ac5a:	3b c3                	cmp    eax,ebx
  40ac5c:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  40ac60:	74 18                	je     0x40ac7a
  40ac62:	68 80 8d 40 00       	push   0x408d80
  40ac67:	68 70 8d 40 00       	push   0x408d70
  40ac6c:	8d 58 04             	lea    ebx,[eax+0x4]
  40ac6f:	57                   	push   edi
  40ac70:	6a 38                	push   0x38
  40ac72:	53                   	push   ebx
  40ac73:	89 38                	mov    DWORD PTR [eax],edi
  40ac75:	e8 55 75 04 00       	call   0x4521cf
  40ac7a:	8b 06                	mov    eax,DWORD PTR [esi]
  40ac7c:	33 ff                	xor    edi,edi
  40ac7e:	85 c0                	test   eax,eax
  40ac80:	c7 44 24 18 ff ff ff 	mov    DWORD PTR [esp+0x18],0xffffffff
  40ac87:	ff 
  40ac88:	89 5e 10             	mov    DWORD PTR [esi+0x10],ebx
  40ac8b:	7e 32                	jle    0x40acbf
  40ac8d:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  40ac90:	33 db                	xor    ebx,ebx
  40ac92:	33 ed                	xor    ebp,ebp
  40ac94:	8d 0c 03             	lea    ecx,[ebx+eax*1]
  40ac97:	57                   	push   edi
  40ac98:	51                   	push   ecx
  40ac99:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40ac9c:	03 cd                	add    ecx,ebp
  40ac9e:	e8 dd e1 ff ff       	call   0x408e80
  40aca3:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40aca6:	80 3c 03 00          	cmp    BYTE PTR [ebx+eax*1],0x0
  40acaa:	74 03                	je     0x40acaf
  40acac:	89 7e 04             	mov    DWORD PTR [esi+0x4],edi
  40acaf:	8b 0e                	mov    ecx,DWORD PTR [esi]
  40acb1:	47                   	inc    edi
  40acb2:	83 c5 38             	add    ebp,0x38
  40acb5:	81 c3 73 04 00 00    	add    ebx,0x473
  40acbb:	3b f9                	cmp    edi,ecx
  40acbd:	7c d5                	jl     0x40ac94
  40acbf:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  40acc3:	5f                   	pop    edi
  40acc4:	5e                   	pop    esi
  40acc5:	5d                   	pop    ebp
  40acc6:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  40accd:	5b                   	pop    ebx
  40acce:	83 c4 0c             	add    esp,0xc
  40acd1:	c2                   	.byte 0xc2
