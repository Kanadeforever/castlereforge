
/mnt/data/work50/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041d720 <.text+0x1c720>:
  41d720:	6a ff                	push   0xffffffff
  41d722:	68 d8 e0 45 00       	push   0x45e0d8
  41d727:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41d72d:	50                   	push   eax
  41d72e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41d735:	83 ec 08             	sub    esp,0x8
  41d738:	53                   	push   ebx
  41d739:	55                   	push   ebp
  41d73a:	56                   	push   esi
  41d73b:	8b f1                	mov    esi,ecx
  41d73d:	57                   	push   edi
  41d73e:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  41d742:	c7 06 d0 0a 46 00    	mov    DWORD PTR [esi],0x460ad0
  41d748:	8b be 88 05 00 00    	mov    edi,DWORD PTR [esi+0x588]
  41d74e:	33 ed                	xor    ebp,ebp
  41d750:	3b fd                	cmp    edi,ebp
  41d752:	89 6c 24 20          	mov    DWORD PTR [esp+0x20],ebp
  41d756:	74 16                	je     0x41d76e
  41d758:	8b cf                	mov    ecx,edi
  41d75a:	e8 81 44 01 00       	call   0x431be0
  41d75f:	57                   	push   edi
  41d760:	e8 eb 3d 03 00       	call   0x451550
  41d765:	83 c4 04             	add    esp,0x4
  41d768:	89 ae 88 05 00 00    	mov    DWORD PTR [esi+0x588],ebp
  41d76e:	8b be 8c 05 00 00    	mov    edi,DWORD PTR [esi+0x58c]
  41d774:	3b fd                	cmp    edi,ebp
  41d776:	74 16                	je     0x41d78e
  41d778:	8b cf                	mov    ecx,edi
  41d77a:	e8 61 44 01 00       	call   0x431be0
  41d77f:	57                   	push   edi
  41d780:	e8 cb 3d 03 00       	call   0x451550
  41d785:	83 c4 04             	add    esp,0x4
  41d788:	89 ae 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],ebp
  41d78e:	8b be 90 05 00 00    	mov    edi,DWORD PTR [esi+0x590]
  41d794:	3b fd                	cmp    edi,ebp
  41d796:	74 16                	je     0x41d7ae
  41d798:	8b cf                	mov    ecx,edi
  41d79a:	e8 41 44 01 00       	call   0x431be0
  41d79f:	57                   	push   edi
  41d7a0:	e8 ab 3d 03 00       	call   0x451550
  41d7a5:	83 c4 04             	add    esp,0x4
  41d7a8:	89 ae 90 05 00 00    	mov    DWORD PTR [esi+0x590],ebp
  41d7ae:	8b be 94 05 00 00    	mov    edi,DWORD PTR [esi+0x594]
  41d7b4:	3b fd                	cmp    edi,ebp
  41d7b6:	74 16                	je     0x41d7ce
  41d7b8:	8b cf                	mov    ecx,edi
  41d7ba:	e8 21 44 01 00       	call   0x431be0
  41d7bf:	57                   	push   edi
  41d7c0:	e8 8b 3d 03 00       	call   0x451550
  41d7c5:	83 c4 04             	add    esp,0x4
  41d7c8:	89 ae 94 05 00 00    	mov    DWORD PTR [esi+0x594],ebp
  41d7ce:	8b be d0 05 00 00    	mov    edi,DWORD PTR [esi+0x5d0]
  41d7d4:	3b fd                	cmp    edi,ebp
  41d7d6:	74 16                	je     0x41d7ee
  41d7d8:	8b cf                	mov    ecx,edi
  41d7da:	e8 f1 3d 01 00       	call   0x4315d0
  41d7df:	57                   	push   edi
  41d7e0:	e8 6b 3d 03 00       	call   0x451550
  41d7e5:	83 c4 04             	add    esp,0x4
  41d7e8:	89 ae d0 05 00 00    	mov    DWORD PTR [esi+0x5d0],ebp
  41d7ee:	8b be d4 05 00 00    	mov    edi,DWORD PTR [esi+0x5d4]
  41d7f4:	3b fd                	cmp    edi,ebp
  41d7f6:	74 16                	je     0x41d80e
  41d7f8:	8b cf                	mov    ecx,edi
  41d7fa:	e8 d1 3d 01 00       	call   0x4315d0
  41d7ff:	57                   	push   edi
  41d800:	e8 4b 3d 03 00       	call   0x451550
  41d805:	83 c4 04             	add    esp,0x4
  41d808:	89 ae d4 05 00 00    	mov    DWORD PTR [esi+0x5d4],ebp
  41d80e:	8b be d8 05 00 00    	mov    edi,DWORD PTR [esi+0x5d8]
  41d814:	3b fd                	cmp    edi,ebp
  41d816:	74 16                	je     0x41d82e
  41d818:	8b cf                	mov    ecx,edi
  41d81a:	e8 b1 3d 01 00       	call   0x4315d0
  41d81f:	57                   	push   edi
  41d820:	e8 2b 3d 03 00       	call   0x451550
  41d825:	83 c4 04             	add    esp,0x4
  41d828:	89 ae d8 05 00 00    	mov    DWORD PTR [esi+0x5d8],ebp
  41d82e:	8b be dc 05 00 00    	mov    edi,DWORD PTR [esi+0x5dc]
  41d834:	3b fd                	cmp    edi,ebp
  41d836:	74 16                	je     0x41d84e
  41d838:	8b cf                	mov    ecx,edi
  41d83a:	e8 91 3d 01 00       	call   0x4315d0
  41d83f:	57                   	push   edi
  41d840:	e8 0b 3d 03 00       	call   0x451550
  41d845:	83 c4 04             	add    esp,0x4
  41d848:	89 ae dc 05 00 00    	mov    DWORD PTR [esi+0x5dc],ebp
  41d84e:	8b be e0 05 00 00    	mov    edi,DWORD PTR [esi+0x5e0]
  41d854:	3b fd                	cmp    edi,ebp
  41d856:	74 16                	je     0x41d86e
  41d858:	8b cf                	mov    ecx,edi
  41d85a:	e8 71 3d 01 00       	call   0x4315d0
  41d85f:	57                   	push   edi
  41d860:	e8 eb 3c 03 00       	call   0x451550
  41d865:	83 c4 04             	add    esp,0x4
  41d868:	89 ae e0 05 00 00    	mov    DWORD PTR [esi+0x5e0],ebp
  41d86e:	8b be e4 05 00 00    	mov    edi,DWORD PTR [esi+0x5e4]
  41d874:	3b fd                	cmp    edi,ebp
  41d876:	74 16                	je     0x41d88e
  41d878:	8b cf                	mov    ecx,edi
  41d87a:	e8 51 3d 01 00       	call   0x4315d0
  41d87f:	57                   	push   edi
  41d880:	e8 cb 3c 03 00       	call   0x451550
  41d885:	83 c4 04             	add    esp,0x4
  41d888:	89 ae e4 05 00 00    	mov    DWORD PTR [esi+0x5e4],ebp
  41d88e:	8d 9e 98 05 00 00    	lea    ebx,[esi+0x598]
  41d894:	c7 44 24 10 0e 00 00 	mov    DWORD PTR [esp+0x10],0xe
  41d89b:	00 
  41d89c:	8b 3b                	mov    edi,DWORD PTR [ebx]
  41d89e:	3b fd                	cmp    edi,ebp
  41d8a0:	74 12                	je     0x41d8b4
  41d8a2:	8b cf                	mov    ecx,edi
  41d8a4:	e8 37 43 01 00       	call   0x431be0
  41d8a9:	57                   	push   edi
  41d8aa:	e8 a1 3c 03 00       	call   0x451550
  41d8af:	83 c4 04             	add    esp,0x4
  41d8b2:	89 2b                	mov    DWORD PTR [ebx],ebp
  41d8b4:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41d8b8:	83 c3 04             	add    ebx,0x4
  41d8bb:	48                   	dec    eax
  41d8bc:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  41d8c0:	75 da                	jne    0x41d89c
  41d8c2:	8d 9e e8 05 00 00    	lea    ebx,[esi+0x5e8]
  41d8c8:	bd 08 00 00 00       	mov    ebp,0x8
  41d8cd:	8b 3b                	mov    edi,DWORD PTR [ebx]
  41d8cf:	85 ff                	test   edi,edi
  41d8d1:	74 16                	je     0x41d8e9
  41d8d3:	8b cf                	mov    ecx,edi
  41d8d5:	e8 f6 3c 01 00       	call   0x4315d0
  41d8da:	57                   	push   edi
  41d8db:	e8 70 3c 03 00       	call   0x451550
  41d8e0:	83 c4 04             	add    esp,0x4
  41d8e3:	c7 03 00 00 00 00    	mov    DWORD PTR [ebx],0x0
  41d8e9:	83 c3 04             	add    ebx,0x4
  41d8ec:	4d                   	dec    ebp
  41d8ed:	75 de                	jne    0x41d8cd
  41d8ef:	8b ce                	mov    ecx,esi
  41d8f1:	c7 44 24 20 ff ff ff 	mov    DWORD PTR [esp+0x20],0xffffffff
  41d8f8:	ff 
  41d8f9:	e8 42 37 01 00       	call   0x431040
  41d8fe:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  41d902:	5f                   	pop    edi
  41d903:	5e                   	pop    esi
  41d904:	5d                   	pop    ebp
  41d905:	5b                   	pop    ebx
  41d906:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41d90d:	83 c4 14             	add    esp,0x14
  41d910:	c3                   	ret
  41d911:	90                   	nop
  41d912:	90                   	nop
  41d913:	90                   	nop
  41d914:	90                   	nop
  41d915:	90                   	nop
  41d916:	90                   	nop
  41d917:	90                   	nop
  41d918:	90                   	nop
  41d919:	90                   	nop
  41d91a:	90                   	nop
  41d91b:	90                   	nop
  41d91c:	90                   	nop
  41d91d:	90                   	nop
  41d91e:	90                   	nop
  41d91f:	90                   	nop
