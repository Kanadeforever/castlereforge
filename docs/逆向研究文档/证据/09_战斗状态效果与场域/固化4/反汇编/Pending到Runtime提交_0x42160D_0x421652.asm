
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004215f0 <.text+0x205f0>:
  4215f0:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  4215f4:	6a 01                	push   0x1
  4215f6:	6a 01                	push   0x1
  4215f8:	83 c0 ce             	add    eax,0xffffffce
  4215fb:	6a 00                	push   0x0
  4215fd:	83 c1 ec             	add    ecx,0xffffffec
  421600:	50                   	push   eax
  421601:	51                   	push   ecx
  421602:	8b 0d 14 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2414
  421608:	e8 e3 57 02 00       	call   0x446df0
  42160d:	b8 cc 00 00 00       	mov    eax,0xcc
  421612:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421619:	8b 14 01             	mov    edx,DWORD PTR [ecx+eax*1]
  42161c:	85 d2                	test   edx,edx
  42161e:	7e 1f                	jle    0x42163f
  421620:	8b b9 6c 0a 00 00    	mov    edi,DWORD PTR [ecx+0xa6c]
  421626:	85 ff                	test   edi,edi
  421628:	7f 15                	jg     0x42163f
  42162a:	89 94 01 68 09 00 00 	mov    DWORD PTR [ecx+eax*1+0x968],edx
  421631:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  421638:	c7 04 02 00 00 00 00 	mov    DWORD PTR [edx+eax*1],0x0
  42163f:	83 c0 04             	add    eax,0x4
  421642:	3d 68 01 00 00       	cmp    eax,0x168
  421647:	7c c9                	jl     0x421612
  421649:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421650:	6a 00                	push   0x0
  421652:	e8 59 f1 ff ff       	call   0x4207b0
  421657:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  42165b:	33 ed                	xor    ebp,ebp
  42165d:	8b 44 24           	mov    eax,DWORD PTR [esp+0x1c]
