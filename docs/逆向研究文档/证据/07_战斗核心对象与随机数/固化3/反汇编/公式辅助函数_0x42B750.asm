
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b750 <.text+0x2a750>:
  42b750:	53                   	push   ebx
  42b751:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b755:	56                   	push   esi
  42b756:	57                   	push   edi
  42b757:	8b f1                	mov    esi,ecx
  42b759:	53                   	push   ebx
  42b75a:	e8 21 03 00 00       	call   0x42ba80
  42b75f:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  42b763:	8b 49 40             	mov    ecx,DWORD PTR [ecx+0x40]
  42b766:	0f af c8             	imul   ecx,eax
  42b769:	b8 67 66 66 66       	mov    eax,0x66666667
  42b76e:	f7 e9                	imul   ecx
  42b770:	8b 4b 3c             	mov    ecx,DWORD PTR [ebx+0x3c]
  42b773:	c1 fa 02             	sar    edx,0x2
  42b776:	8b c2                	mov    eax,edx
  42b778:	c1 e8 1f             	shr    eax,0x1f
  42b77b:	03 d0                	add    edx,eax
  42b77d:	b8 67 66 66 66       	mov    eax,0x66666667
  42b782:	8b fa                	mov    edi,edx
  42b784:	f7 e9                	imul   ecx
  42b786:	c1 fa 02             	sar    edx,0x2
  42b789:	8b c2                	mov    eax,edx
  42b78b:	c1 e8 1f             	shr    eax,0x1f
  42b78e:	03 d0                	add    edx,eax
  42b790:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b795:	52                   	push   edx
  42b796:	f7 e9                	imul   ecx
  42b798:	c1 fa 04             	sar    edx,0x4
  42b79b:	8b ca                	mov    ecx,edx
  42b79d:	c1 e9 1f             	shr    ecx,0x1f
  42b7a0:	03 d1                	add    edx,ecx
  42b7a2:	8b ce                	mov    ecx,esi
  42b7a4:	52                   	push   edx
  42b7a5:	e8 06 15 00 00       	call   0x42ccb0
  42b7aa:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  42b7ae:	8b ce                	mov    ecx,esi
  42b7b0:	52                   	push   edx
  42b7b1:	03 f8                	add    edi,eax
  42b7b3:	e8 98 03 00 00       	call   0x42bb50
  42b7b8:	2b f8                	sub    edi,eax
  42b7ba:	b8 00 00 00 00       	mov    eax,0x0
  42b7bf:	0f 98 c0             	sets   al
  42b7c2:	48                   	dec    eax
  42b7c3:	23 c7                	and    eax,edi
  42b7c5:	5f                   	pop    edi
  42b7c6:	5e                   	pop    esi
  42b7c7:	5b                   	pop    ebx
  42b7c8:	c2 0c 00             	ret    0xc
  42b7cb:	90                   	nop
  42b7cc:	90                   	nop
  42b7cd:	90                   	nop
  42b7ce:	90                   	nop
  42b7cf:	90                   	nop
