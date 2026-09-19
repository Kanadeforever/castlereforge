
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004466f0 <.text+0x456f0>:
  4466f0:	8b 87 18 02 00 00    	mov    eax,DWORD PTR [edi+0x218]
  4466f6:	32 d2                	xor    dl,dl
  4466f8:	32 db                	xor    bl,bl
  4466fa:	33 c9                	xor    ecx,ecx
  4466fc:	55                   	push   ebp
  4466fd:	be 64 00 00 00       	mov    esi,0x64
  446702:	05 a4 0b 00 00       	add    eax,0xba4
  446707:	8b a8 88 fb ff ff    	mov    ebp,DWORD PTR [eax-0x478]
  44670d:	85 ed                	test   ebp,ebp
  44670f:	7c 21                	jl     0x446732
  446711:	8b 28                	mov    ebp,DWORD PTR [eax]
  446713:	8b 6d 20             	mov    ebp,DWORD PTR [ebp+0x20]
  446716:	80 bd 58 03 00 00 00 	cmp    BYTE PTR [ebp+0x358],0x0
  44671d:	74 13                	je     0x446732
  44671f:	8b a8 d8 fc ff ff    	mov    ebp,DWORD PTR [eax-0x328]
  446725:	85 ed                	test   ebp,ebp
  446727:	7e 09                	jle    0x446732
  446729:	c6 44 0c 10 01       	mov    BYTE PTR [esp+ecx*1+0x10],0x1
  44672e:	b2 01                	mov    dl,0x1
  446730:	eb 05                	jmp    0x446737
  446732:	c6 44 0c 10 00       	mov    BYTE PTR [esp+ecx*1+0x10],0x0
  446737:	41                   	inc    ecx
  446738:	05 e4 0d 00 00       	add    eax,0xde4
  44673d:	83 f9 08             	cmp    ecx,0x8
  446740:	7c c5                	jl     0x446707
  446742:	84 d2                	test   dl,dl
  446744:	5d                   	pop    ebp
  446745:	74 77                	je     0x4467be
  446747:	85 f6                	test   esi,esi
  446749:	7e 73                	jle    0x4467be
  44674b:	e8 15 ae 00 00       	call   0x451565
  446750:	25 07 00 00 80       	and    eax,0x80000007
  446755:	79 05                	jns    0x44675c
  446757:	48                   	dec    eax
  446758:	83 c8 f8             	or     eax,0xfffffff8
  44675b:	40                   	inc    eax
  44675c:	8a 4c 04 0c          	mov    cl,BYTE PTR [esp+eax*1+0xc]
  446760:	4e                   	dec    esi
  446761:	83 f8 04             	cmp    eax,0x4
  446764:	7d 10                	jge    0x446776
  446766:	84 c9                	test   cl,cl
  446768:	74 14                	je     0x44677e
  44676a:	8a 4c 04 10          	mov    cl,BYTE PTR [esp+eax*1+0x10]
  44676e:	84 c9                	test   cl,cl
  446770:	75 0c                	jne    0x44677e
  446772:	b3 01                	mov    bl,0x1
  446774:	eb 0c                	jmp    0x446782
  446776:	84 c9                	test   cl,cl
  446778:	74 04                	je     0x44677e
  44677a:	b3 01                	mov    bl,0x1
  44677c:	eb 04                	jmp    0x446782
  44677e:	84 db                	test   bl,bl
  446780:	74 c5                	je     0x446747
  446782:	8d 0c c0             	lea    ecx,[eax+eax*8]
  446785:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  446788:	8d 14 49             	lea    edx,[ecx+ecx*2]
  44678b:	8b 8f 18 02 00 00    	mov    ecx,DWORD PTR [edi+0x218]
  446791:	8d 04 d0             	lea    eax,[eax+edx*8]
  446794:	8b 84 81 9c 0b 00 00 	mov    eax,DWORD PTR [ecx+eax*4+0xb9c]
  44679b:	85 c0                	test   eax,eax
  44679d:	7c a8                	jl     0x446747
  44679f:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  4467a5:	8b 0c 82             	mov    ecx,DWORD PTR [edx+eax*4]
  4467a8:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  4467ae:	51                   	push   ecx
  4467af:	8b 0d 4c 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c4c
  4467b5:	8b 04 82             	mov    eax,DWORD PTR [edx+eax*4]
  4467b8:	50                   	push   eax
  4467b9:	e8 72 77 ff ff       	call   0x43df30
  4467be:	5f                   	pop    edi
  4467bf:	5e                   	pop    esi
  4467c0:	5b                   	pop    ebx
  4467c1:	83 c4 08             	add    esp,0x8
  4467c4:	c3                   	ret
