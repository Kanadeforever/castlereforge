
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040ada0 <.text+0x9da0>:
  40ada0:	56                   	push   esi
  40ada1:	8b f1                	mov    esi,ecx
  40ada3:	57                   	push   edi
  40ada4:	33 ff                	xor    edi,edi
  40ada6:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  40ada9:	7e 28                	jle    0x40add3
  40adab:	53                   	push   ebx
  40adac:	33 db                	xor    ebx,ebx
  40adae:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40adb1:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40adb4:	3b f8                	cmp    edi,eax
  40adb6:	75 09                	jne    0x40adc1
  40adb8:	03 cb                	add    ecx,ebx
  40adba:	e8 c1 e7 ff ff       	call   0x409580
  40adbf:	eb 07                	jmp    0x40adc8
  40adc1:	03 cb                	add    ecx,ebx
  40adc3:	e8 48 e7 ff ff       	call   0x409510
  40adc8:	8b 06                	mov    eax,DWORD PTR [esi]
  40adca:	47                   	inc    edi
  40adcb:	83 c3 38             	add    ebx,0x38
  40adce:	3b f8                	cmp    edi,eax
  40add0:	7c dc                	jl     0x40adae
  40add2:	5b                   	pop    ebx
  40add3:	5f                   	pop    edi
  40add4:	5e                   	pop    esi
  40add5:	c3                   	ret
  40add6:	90                   	nop
  40add7:	90                   	nop
  40add8:	90                   	nop
  40add9:	90                   	nop
  40adda:	90                   	nop
  40addb:	90                   	nop
  40addc:	90                   	nop
  40addd:	90                   	nop
  40adde:	90                   	nop
  40addf:	90                   	nop
  40ade0:	56                   	push   esi
  40ade1:	8b f1                	mov    esi,ecx
  40ade3:	57                   	push   edi
  40ade4:	33 ff                	xor    edi,edi
  40ade6:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  40ade9:	7e 28                	jle    0x40ae13
  40adeb:	53                   	push   ebx
  40adec:	33 db                	xor    ebx,ebx
  40adee:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40adf1:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40adf4:	3b f8                	cmp    edi,eax
  40adf6:	75 09                	jne    0x40ae01
  40adf8:	03 cb                	add    ecx,ebx
  40adfa:	e8 e1 ed ff ff       	call   0x409be0
  40adff:	eb 07                	jmp    0x40ae08
  40ae01:	03 cb                	add    ecx,ebx
  40ae03:	e8 48 ee ff ff       	call   0x409c50
  40ae08:	8b 06                	mov    eax,DWORD PTR [esi]
  40ae0a:	47                   	inc    edi
  40ae0b:	83 c3 38             	add    ebx,0x38
  40ae0e:	3b f8                	cmp    edi,eax
  40ae10:	7c dc                	jl     0x40adee
  40ae12:	5b                   	pop    ebx
  40ae13:	5f                   	pop    edi
  40ae14:	5e                   	pop    esi
  40ae15:	c3                   	ret
  40ae16:	90                   	nop
  40ae17:	90                   	nop
  40ae18:	90                   	nop
  40ae19:	90                   	nop
  40ae1a:	90                   	nop
  40ae1b:	90                   	nop
  40ae1c:	90                   	nop
  40ae1d:	90                   	nop
  40ae1e:	90                   	nop
  40ae1f:	90                   	nop
  40ae20:	56                   	push   esi
  40ae21:	8b f1                	mov    esi,ecx
  40ae23:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40ae26:	8b 56 10             	mov    edx,DWORD PTR [esi+0x10]
  40ae29:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  40ae30:	2b c8                	sub    ecx,eax
  40ae32:	8d 0c ca             	lea    ecx,[edx+ecx*8]
  40ae35:	e8 c6 e5 ff ff       	call   0x409400
  40ae3a:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40ae3d:	e8 4e e6 ff ff       	call   0x409490
  40ae42:	5e                   	pop    esi
  40ae43:	c3                   	ret
  40ae44:	90                   	nop
  40ae45:	90                   	nop
  40ae46:	90                   	nop
  40ae47:	90                   	nop
  40ae48:	90                   	nop
  40ae49:	90                   	nop
  40ae4a:	90                   	nop
  40ae4b:	90                   	nop
  40ae4c:	90                   	nop
  40ae4d:	90                   	nop
  40ae4e:	90                   	nop
  40ae4f:	90                   	nop
  40ae50:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  40ae53:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  40ae5a:	2b d0                	sub    edx,eax
  40ae5c:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  40ae5f:	8d 04 d0             	lea    eax,[eax+edx*8]
  40ae62:	c3                   	ret
  40ae63:	90                   	nop
  40ae64:	90                   	nop
  40ae65:	90                   	nop
  40ae66:	90                   	nop
  40ae67:	90                   	nop
  40ae68:	90                   	nop
  40ae69:	90                   	nop
  40ae6a:	90                   	nop
  40ae6b:	90                   	nop
  40ae6c:	90                   	nop
  40ae6d:	90                   	nop
  40ae6e:	90                   	nop
  40ae6f:	90                   	nop
  40ae70:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  40ae73:	c3                   	ret
