
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b5f0 <.text+0x2a5f0>:
  42b5f0:	53                   	push   ebx
  42b5f1:	56                   	push   esi
  42b5f2:	57                   	push   edi
  42b5f3:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  42b5f7:	8b f1                	mov    esi,ecx
  42b5f9:	57                   	push   edi
  42b5fa:	e8 81 04 00 00       	call   0x42ba80
  42b5ff:	8b d8                	mov    ebx,eax
  42b601:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42b605:	50                   	push   eax
  42b606:	8b ce                	mov    ecx,esi
  42b608:	e8 43 05 00 00       	call   0x42bb50
  42b60d:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42b611:	51                   	push   ecx
  42b612:	50                   	push   eax
  42b613:	53                   	push   ebx
  42b614:	57                   	push   edi
  42b615:	8b ce                	mov    ecx,esi
  42b617:	e8 b4 06 00 00       	call   0x42bcd0
  42b61c:	33 d2                	xor    edx,edx
  42b61e:	5f                   	pop    edi
  42b61f:	85 c0                	test   eax,eax
  42b621:	0f 9c c2             	setl   dl
  42b624:	4a                   	dec    edx
  42b625:	5e                   	pop    esi
  42b626:	23 c2                	and    eax,edx
  42b628:	5b                   	pop    ebx
  42b629:	c2 0c 00             	ret    0xc
  42b62c:	90                   	nop
  42b62d:	90                   	nop
  42b62e:	90                   	nop
  42b62f:	90                   	nop
