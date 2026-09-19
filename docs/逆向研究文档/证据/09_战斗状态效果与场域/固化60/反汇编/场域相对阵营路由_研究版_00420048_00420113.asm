
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420048 <.text+0x1f048>:
  420048:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  42004e:	c7 44 24 10 ed 03 00 	mov    DWORD PTR [esp+0x10],0x3ed
  420055:	00 
  420056:	8b 48 18             	mov    ecx,DWORD PTR [eax+0x18]
  420059:	3b cf                	cmp    ecx,edi
  42005b:	74 0e                	je     0x42006b
  42005d:	83 f9 01             	cmp    ecx,0x1
  420060:	74 09                	je     0x42006b
  420062:	83 f9 04             	cmp    ecx,0x4
  420065:	0f 85 a8 00 00 00    	jne    0x420113
  42006b:	8b 0e                	mov    ecx,DWORD PTR [esi]
  42006d:	83 f9 07             	cmp    ecx,0x7
  420070:	7f 07                	jg     0x420079
  420072:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  420075:	85 d2                	test   edx,edx
  420077:	74 10                	je     0x420089
  420079:	83 f9 08             	cmp    ecx,0x8
  42007c:	7c 06                	jl     0x420084
  42007e:	83 78 20 01          	cmp    DWORD PTR [eax+0x20],0x1
  420082:	74 05                	je     0x420089
  420084:	39 78 20             	cmp    DWORD PTR [eax+0x20],edi
  420087:	75 23                	jne    0x4200ac
  420089:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  42008f:	52                   	push   edx
  420090:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  420096:	52                   	push   edx
  420097:	8b 10                	mov    edx,DWORD PTR [eax]
  420099:	51                   	push   ecx
  42009a:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  4200a0:	51                   	push   ecx
  4200a1:	52                   	push   edx
  4200a2:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4200a7:	e8 74 da 01 00       	call   0x43db20
  4200ac:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4200ae:	83 f9 07             	cmp    ecx,0x7
  4200b1:	7f 0c                	jg     0x4200bf
  4200b3:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200b9:	83 78 20 01          	cmp    DWORD PTR [eax+0x20],0x1
  4200bd:	74 1d                	je     0x4200dc
  4200bf:	83 f9 08             	cmp    ecx,0x8
  4200c2:	7c 0d                	jl     0x4200d1
  4200c4:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200ca:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  4200cd:	85 d2                	test   edx,edx
  4200cf:	74 0b                	je     0x4200dc
  4200d1:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200d7:	39 78 20             	cmp    DWORD PTR [eax+0x20],edi
  4200da:	75 23                	jne    0x4200ff
  4200dc:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  4200e2:	52                   	push   edx
  4200e3:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  4200e9:	52                   	push   edx
  4200ea:	8b 10                	mov    edx,DWORD PTR [eax]
  4200ec:	51                   	push   ecx
  4200ed:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  4200f3:	51                   	push   ecx
  4200f4:	52                   	push   edx
  4200f5:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4200fa:	e8 21 da 01 00       	call   0x43db20
  4200ff:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  420105:	6a 00                	push   0x0
  420107:	e8 f4 40 01 00       	call   0x434200
  42010c:	c6 05 dc 96 46 00 01 	mov    BYTE PTR ds:0x4696dc,0x1
