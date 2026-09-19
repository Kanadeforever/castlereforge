
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b840 <.text+0x2a840>:
  42b840:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  42b844:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  42b848:	56                   	push   esi
  42b849:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  42b84d:	56                   	push   esi
  42b84e:	50                   	push   eax
  42b84f:	52                   	push   edx
  42b850:	e8 9b fd ff ff       	call   0x42b5f0
  42b855:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  42b858:	5e                   	pop    esi
  42b859:	0f af c8             	imul   ecx,eax
  42b85c:	b8 67 66 66 66       	mov    eax,0x66666667
  42b861:	f7 e9                	imul   ecx
  42b863:	c1 fa 02             	sar    edx,0x2
  42b866:	8b c2                	mov    eax,edx
  42b868:	c1 e8 1f             	shr    eax,0x1f
  42b86b:	03 d0                	add    edx,eax
  42b86d:	b8 00 00 00 00       	mov    eax,0x0
  42b872:	0f 98 c0             	sets   al
  42b875:	48                   	dec    eax
  42b876:	23 c2                	and    eax,edx
  42b878:	c2 0c 00             	ret    0xc
  42b87b:	90                   	nop
  42b87c:	90                   	nop
  42b87d:	90                   	nop
  42b87e:	90                   	nop
  42b87f:	90                   	nop
