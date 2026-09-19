
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00451583 <.text+0x50583>:
  451583:	55                   	push   ebp
  451584:	8b ec                	mov    ebp,esp
  451586:	81 ec cc 00 00 00    	sub    esp,0xcc
  45158c:	8d 45 f0             	lea    eax,[ebp-0x10]
  45158f:	50                   	push   eax
  451590:	ff 15 78 01 46 00    	call   DWORD PTR ds:0x460178
  451596:	8d 45 e0             	lea    eax,[ebp-0x20]
  451599:	50                   	push   eax
  45159a:	ff 15 7c 01 46 00    	call   DWORD PTR ds:0x46017c
  4515a0:	66 8b 45 ea          	mov    ax,WORD PTR [ebp-0x16]
  4515a4:	66 3b 05 9a 1f 98 00 	cmp    ax,WORD PTR ds:0x981f9a
  4515ab:	75 3b                	jne    0x4515e8
  4515ad:	66 8b 45 e8          	mov    ax,WORD PTR [ebp-0x18]
  4515b1:	66 3b 05 98 1f 98 00 	cmp    ax,WORD PTR ds:0x981f98
  4515b8:	75 2e                	jne    0x4515e8
  4515ba:	66 8b 45 e6          	mov    ax,WORD PTR [ebp-0x1a]
  4515be:	66 3b 05 96 1f 98 00 	cmp    ax,WORD PTR ds:0x981f96
  4515c5:	75 21                	jne    0x4515e8
  4515c7:	66 8b 45 e2          	mov    ax,WORD PTR [ebp-0x1e]
  4515cb:	66 3b 05 92 1f 98 00 	cmp    ax,WORD PTR ds:0x981f92
  4515d2:	75 14                	jne    0x4515e8
  4515d4:	66 8b 45 e0          	mov    ax,WORD PTR [ebp-0x20]
  4515d8:	66 3b 05 90 1f 98 00 	cmp    ax,WORD PTR ds:0x981f90
  4515df:	75 07                	jne    0x4515e8
  4515e1:	a1 88 1f 98 00       	mov    eax,ds:0x981f88
  4515e6:	eb 45                	jmp    0x45162d
  4515e8:	8d 85 34 ff ff ff    	lea    eax,[ebp-0xcc]
  4515ee:	50                   	push   eax
  4515ef:	ff 15 80 01 46 00    	call   DWORD PTR ds:0x460180
  4515f5:	83 f8 ff             	cmp    eax,0xffffffff
  4515f8:	74 1b                	je     0x451615
  4515fa:	83 f8 02             	cmp    eax,0x2
  4515fd:	75 12                	jne    0x451611
  4515ff:	66 83 7d ce 00       	cmp    WORD PTR [ebp-0x32],0x0
  451604:	74 0b                	je     0x451611
  451606:	83 7d dc 00          	cmp    DWORD PTR [ebp-0x24],0x0
  45160a:	74 05                	je     0x451611
  45160c:	6a 01                	push   0x1
  45160e:	58                   	pop    eax
  45160f:	eb 07                	jmp    0x451618
  451611:	33 c0                	xor    eax,eax
  451613:	eb 03                	jmp    0x451618
  451615:	83 c8 ff             	or     eax,0xffffffff
  451618:	56                   	push   esi
  451619:	57                   	push   edi
  45161a:	8d 75 e0             	lea    esi,[ebp-0x20]
  45161d:	bf 90 1f 98 00       	mov    edi,0x981f90
  451622:	a5                   	movs   DWORD PTR es:[edi],DWORD PTR ds:[esi]
  451623:	a5                   	movs   DWORD PTR es:[edi],DWORD PTR ds:[esi]
  451624:	a5                   	movs   DWORD PTR es:[edi],DWORD PTR ds:[esi]
  451625:	a5                   	movs   DWORD PTR es:[edi],DWORD PTR ds:[esi]
  451626:	5f                   	pop    edi
  451627:	a3 88 1f 98 00       	mov    ds:0x981f88,eax
  45162c:	5e                   	pop    esi
  45162d:	50                   	push   eax
  45162e:	0f b7 45 fc          	movzx  eax,WORD PTR [ebp-0x4]
  451632:	50                   	push   eax
  451633:	0f b7 45 fa          	movzx  eax,WORD PTR [ebp-0x6]
  451637:	50                   	push   eax
  451638:	0f b7 45 f8          	movzx  eax,WORD PTR [ebp-0x8]
  45163c:	50                   	push   eax
  45163d:	0f b7 45 f6          	movzx  eax,WORD PTR [ebp-0xa]
  451641:	50                   	push   eax
  451642:	0f b7 45 f2          	movzx  eax,WORD PTR [ebp-0xe]
  451646:	50                   	push   eax
  451647:	0f b7 45 f0          	movzx  eax,WORD PTR [ebp-0x10]
  45164b:	50                   	push   eax
  45164c:	e8 84 17 00 00       	call   0x452dd5
  451651:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  451654:	83 c4 1c             	add    esp,0x1c
  451657:	85 c9                	test   ecx,ecx
  451659:	74 02                	je     0x45165d
  45165b:	89 01                	mov    DWORD PTR [ecx],eax
  45165d:	c9                   	leave
  45165e:	c3                   	ret
