
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00432890 <.text+0x31890>:
  432890:	5e                   	pop    esi
  432891:	5b                   	pop    ebx
  432892:	c3                   	ret
  432893:	90                   	nop
  432894:	90                   	nop
  432895:	90                   	nop
  432896:	90                   	nop
  432897:	90                   	nop
  432898:	90                   	nop
  432899:	90                   	nop
  43289a:	90                   	nop
  43289b:	90                   	nop
  43289c:	90                   	nop
  43289d:	90                   	nop
  43289e:	90                   	nop
  43289f:	90                   	nop
  4328a0:	56                   	push   esi
  4328a1:	57                   	push   edi
  4328a2:	8b f1                	mov    esi,ecx
  4328a4:	6a 64                	push   0x64
  4328a6:	56                   	push   esi
  4328a7:	e8 54 1c 00 00       	call   0x434500
  4328ac:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  4328b2:	e8 d9 0c 01 00       	call   0x443590
  4328b7:	8b f8                	mov    edi,eax
  4328b9:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  4328be:	8a 88 fc 05 00 00    	mov    cl,BYTE PTR [eax+0x5fc]
  4328c4:	84 c9                	test   cl,cl
  4328c6:	74 05                	je     0x4328cd
  4328c8:	bf 03 00 00 00       	mov    edi,0x3
  4328cd:	8a 86 40 02 00 00    	mov    al,BYTE PTR [esi+0x240]
  4328d3:	84 c0                	test   al,al
  4328d5:	0f 85 9d 00 00 00    	jne    0x432978
  4328db:	83 ff 01             	cmp    edi,0x1
  4328de:	74 0f                	je     0x4328ef
  4328e0:	83 ff 02             	cmp    edi,0x2
  4328e3:	74 0a                	je     0x4328ef
  4328e5:	83 ff 04             	cmp    edi,0x4
  4328e8:	74 05                	je     0x4328ef
  4328ea:	83 ff 03             	cmp    edi,0x3
  4328ed:	75 7b                	jne    0x43296a
  4328ef:	83 ff 01             	cmp    edi,0x1
  4328f2:	c6 86 40 02 00 00 01 	mov    BYTE PTR [esi+0x240],0x1
  4328f9:	74 05                	je     0x432900
  4328fb:	83 ff 04             	cmp    edi,0x4
  4328fe:	75 1d                	jne    0x43291d
  432900:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432906:	6a 01                	push   0x1
  432908:	e8 53 0d 01 00       	call   0x443660
  43290d:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  432913:	c7 81 18 01 00 00 01 	mov    DWORD PTR [ecx+0x118],0x1
  43291a:	00 00 00 
  43291d:	83 ff 02             	cmp    edi,0x2
  432920:	75 1a                	jne    0x43293c
  432922:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432928:	57                   	push   edi
  432929:	e8 32 0d 01 00       	call   0x443660
  43292e:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  432934:	89 ba 18 01 00 00    	mov    DWORD PTR [edx+0x118],edi
  43293a:	eb 23                	jmp    0x43295f
  43293c:	83 ff 03             	cmp    edi,0x3
  43293f:	75 19                	jne    0x43295a
  432941:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432947:	57                   	push   edi
  432948:	e8 13 0d 01 00       	call   0x443660
  43294d:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  432952:	89 b8 18 01 00 00    	mov    DWORD PTR [eax+0x118],edi
  432958:	eb 05                	jmp    0x43295f
  43295a:	83 ff 04             	cmp    edi,0x4
  43295d:	74 0b                	je     0x43296a
  43295f:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  432965:	e8 d6 fa fd ff       	call   0x412440
  43296a:	8a 86 40 02 00 00    	mov    al,BYTE PTR [esi+0x240]
  432970:	84 c0                	test   al,al
  432972:	0f 84 a2 00 00 00    	je     0x432a1a
  432978:	a1 c8 fc 89 00       	mov    eax,ds:0x89fcc8
  43297d:	8a 88 79 05 00 00    	mov    cl,BYTE PTR [eax+0x579]
  432983:	84 c9                	test   cl,cl
  432985:	0f 85 8f 00 00 00    	jne    0x432a1a
  43298b:	8b 88 00 06 00 00    	mov    ecx,DWORD PTR [eax+0x600]
  432991:	8a 81 79 05 00 00    	mov    al,BYTE PTR [ecx+0x579]
  432997:	84 c0                	test   al,al
  432999:	75 7f                	jne    0x432a1a
  43299b:	8a 86 48 02 00 00    	mov    al,BYTE PTR [esi+0x248]
  4329a1:	84 c0                	test   al,al
  4329a3:	75 42                	jne    0x4329e7
  4329a5:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  4329ab:	6a 01                	push   0x1
  4329ad:	6a 00                	push   0x0
  4329af:	68 40 ac 27 00       	push   0x27ac40
  4329b4:	68 f6 00 00 00       	push   0xf6
  4329b9:	68 25 01 00 00       	push   0x125
  4329be:	68 f6 00 00 00       	push   0xf6
  4329c3:	68 25 01 00 00       	push   0x125
  4329c8:	68 98 a4 46 00       	push   0x46a498
  4329cd:	e8 fe aa ff ff       	call   0x42d4d0
  4329d2:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  4329d8:	6a 00                	push   0x0
  4329da:	50                   	push   eax
  4329db:	e8 00 ad ff ff       	call   0x42d6e0
  4329e0:	c6 86 48 02 00 00 01 	mov    BYTE PTR [esi+0x248],0x1
  4329e7:	8b 86 44 02 00 00    	mov    eax,DWORD PTR [esi+0x244]
  4329ed:	85 c0                	test   eax,eax
  4329ef:	7f 22                	jg     0x432a13
  4329f1:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  4329f7:	e8 64 f5 00 00       	call   0x441f60
  4329fc:	85 f6                	test   esi,esi
  4329fe:	74 1a                	je     0x432a1a
  432a00:	8b ce                	mov    ecx,esi
  432a02:	e8 29 f7 ff ff       	call   0x432130
  432a07:	56                   	push   esi
  432a08:	e8 43 eb 01 00       	call   0x451550
  432a0d:	83 c4 04             	add    esp,0x4
  432a10:	5f                   	pop    edi
  432a11:	5e                   	pop    esi
  432a12:	c3                   	ret
  432a13:	48                   	dec    eax
  432a14:	89 86 44 02 00 00    	mov    DWORD PTR [esi+0x244],eax
  432a1a:	5f                   	pop    edi
  432a1b:	5e                   	pop    esi
  432a1c:	c3                   	ret
  432a1d:	90                   	nop
  432a1e:	90                   	nop
  432a1f:	90                   	nop
