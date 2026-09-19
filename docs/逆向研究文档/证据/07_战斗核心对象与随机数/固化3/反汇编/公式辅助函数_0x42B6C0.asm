
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b6c0 <.text+0x2a6c0>:
  42b6c0:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42b6c4:	53                   	push   ebx
  42b6c5:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  42b6c9:	55                   	push   ebp
  42b6ca:	56                   	push   esi
  42b6cb:	57                   	push   edi
  42b6cc:	8b f9                	mov    edi,ecx
  42b6ce:	53                   	push   ebx
  42b6cf:	8b 4b 2c             	mov    ecx,DWORD PTR [ebx+0x2c]
  42b6d2:	0f af 48 40          	imul   ecx,DWORD PTR [eax+0x40]
  42b6d6:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b6db:	f7 e9                	imul   ecx
  42b6dd:	c1 fa 05             	sar    edx,0x5
  42b6e0:	8b ca                	mov    ecx,edx
  42b6e2:	c1 e9 1f             	shr    ecx,0x1f
  42b6e5:	03 d1                	add    edx,ecx
  42b6e7:	8b cf                	mov    ecx,edi
  42b6e9:	8b ea                	mov    ebp,edx
  42b6eb:	e8 90 03 00 00       	call   0x42ba80
  42b6f0:	8b 5b 3c             	mov    ebx,DWORD PTR [ebx+0x3c]
  42b6f3:	8b f0                	mov    esi,eax
  42b6f5:	b8 67 66 66 66       	mov    eax,0x66666667
  42b6fa:	03 f5                	add    esi,ebp
  42b6fc:	f7 eb                	imul   ebx
  42b6fe:	c1 fa 02             	sar    edx,0x2
  42b701:	8b c2                	mov    eax,edx
  42b703:	c1 e8 1f             	shr    eax,0x1f
  42b706:	03 d0                	add    edx,eax
  42b708:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b70d:	52                   	push   edx
  42b70e:	f7 eb                	imul   ebx
  42b710:	c1 fa 04             	sar    edx,0x4
  42b713:	8b ca                	mov    ecx,edx
  42b715:	c1 e9 1f             	shr    ecx,0x1f
  42b718:	03 d1                	add    edx,ecx
  42b71a:	8b cf                	mov    ecx,edi
  42b71c:	52                   	push   edx
  42b71d:	e8 8e 15 00 00       	call   0x42ccb0
  42b722:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  42b726:	8b cf                	mov    ecx,edi
  42b728:	52                   	push   edx
  42b729:	03 f0                	add    esi,eax
  42b72b:	e8 20 04 00 00       	call   0x42bb50
  42b730:	2b f0                	sub    esi,eax
  42b732:	b8 00 00 00 00       	mov    eax,0x0
  42b737:	0f 98 c0             	sets   al
  42b73a:	48                   	dec    eax
  42b73b:	5f                   	pop    edi
  42b73c:	23 c6                	and    eax,esi
  42b73e:	5e                   	pop    esi
  42b73f:	5d                   	pop    ebp
  42b740:	5b                   	pop    ebx
  42b741:	c2 0c 00             	ret    0xc
  42b744:	90                   	nop
  42b745:	90                   	nop
  42b746:	90                   	nop
  42b747:	90                   	nop
  42b748:	90                   	nop
  42b749:	90                   	nop
  42b74a:	90                   	nop
  42b74b:	90                   	nop
  42b74c:	90                   	nop
  42b74d:	90                   	nop
  42b74e:	90                   	nop
  42b74f:	90                   	nop
