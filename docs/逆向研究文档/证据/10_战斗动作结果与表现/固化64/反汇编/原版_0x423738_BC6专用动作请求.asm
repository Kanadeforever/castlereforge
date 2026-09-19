
/mnt/data/solid64_inputs/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423738 <.text+0x22738>:
  423738:	8b 0f                	mov    ecx,DWORD PTR [edi]
  42373a:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  423741:	8b 81 e4 0c 00 00    	mov    eax,DWORD PTR [ecx+0xce4]
  423747:	3d c6 0b 00 00       	cmp    eax,0xbc6
  42374c:	75 08                	jne    0x423756
  42374e:	6a 00                	push   0x0
  423750:	6a 00                	push   0x0
  423752:	6a 72                	push   0x72
  423754:	eb 1a                	jmp    0x423770
  423756:	3d be 0b 00 00       	cmp    eax,0xbbe
  42375b:	74 0d                	je     0x42376a
  42375d:	80 3b 00             	cmp    BYTE PTR [ebx],0x0
  423760:	74 08                	je     0x42376a
  423762:	6a 00                	push   0x0
  423764:	6a 00                	push   0x0
  423766:	6a 6c                	push   0x6c
  423768:	eb 06                	jmp    0x423770
  42376a:	6a 00                	push   0x0
  42376c:	6a 00                	push   0x0
  42376e:	6a 6d                	push   0x6d
  423770:	e8 ab e9 ff ff       	call   0x422120
  423775:	8b 86 c8 00 00 00    	mov    eax,DWORD PTR [esi+0xc8]
  42377b:	45                   	inc    ebp
  42377c:	83 c7 04             	add    edi,0x4
  42377f:	83 c3 50             	add    ebx,0x50
  423782:	3b e8                	cmp    ebp,eax
  423784:	7c b2                	jl     0x423738
