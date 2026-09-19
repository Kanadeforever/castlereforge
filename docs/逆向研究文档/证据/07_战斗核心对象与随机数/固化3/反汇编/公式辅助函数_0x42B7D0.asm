
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b7d0 <.text+0x2a7d0>:
  42b7d0:	53                   	push   ebx
  42b7d1:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b7d5:	56                   	push   esi
  42b7d6:	57                   	push   edi
  42b7d7:	8b f9                	mov    edi,ecx
  42b7d9:	53                   	push   ebx
  42b7da:	e8 a1 02 00 00       	call   0x42ba80
  42b7df:	8b 4b 3c             	mov    ecx,DWORD PTR [ebx+0x3c]
  42b7e2:	8b f0                	mov    esi,eax
  42b7e4:	b8 67 66 66 66       	mov    eax,0x66666667
  42b7e9:	f7 e9                	imul   ecx
  42b7eb:	c1 fa 02             	sar    edx,0x2
  42b7ee:	8b c2                	mov    eax,edx
  42b7f0:	c1 e8 1f             	shr    eax,0x1f
  42b7f3:	03 d0                	add    edx,eax
  42b7f5:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b7fa:	52                   	push   edx
  42b7fb:	f7 e9                	imul   ecx
  42b7fd:	c1 fa 04             	sar    edx,0x4
  42b800:	8b ca                	mov    ecx,edx
  42b802:	c1 e9 1f             	shr    ecx,0x1f
  42b805:	03 d1                	add    edx,ecx
  42b807:	8b cf                	mov    ecx,edi
  42b809:	52                   	push   edx
  42b80a:	e8 a1 14 00 00       	call   0x42ccb0
  42b80f:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  42b813:	8b cf                	mov    ecx,edi
  42b815:	52                   	push   edx
  42b816:	03 f0                	add    esi,eax
  42b818:	e8 33 03 00 00       	call   0x42bb50
  42b81d:	99                   	cdq
  42b81e:	f7 fe                	idiv   esi
  42b820:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42b824:	5f                   	pop    edi
  42b825:	0f af 41 40          	imul   eax,DWORD PTR [ecx+0x40]
  42b829:	2b f0                	sub    esi,eax
  42b82b:	b8 00 00 00 00       	mov    eax,0x0
  42b830:	0f 98 c0             	sets   al
  42b833:	48                   	dec    eax
  42b834:	23 c6                	and    eax,esi
  42b836:	5e                   	pop    esi
  42b837:	5b                   	pop    ebx
  42b838:	c2 0c 00             	ret    0xc
  42b83b:	90                   	nop
  42b83c:	90                   	nop
  42b83d:	90                   	nop
  42b83e:	90                   	nop
  42b83f:	90                   	nop
