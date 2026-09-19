; 固化38证据：WM_TIMER只设置BYTE pending，主循环一次pending只调用一次0x44A970。

/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401650 <.text+0x650>:
  401650:	55                   	push   ebp
  401651:	8b ec                	mov    ebp,esp
  401653:	6a ff                	push   0xffffffff
  401655:	68 00 cf 45 00       	push   0x45cf00
  40165a:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  401660:	50                   	push   eax
  401661:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  401668:	83 ec 20             	sub    esp,0x20
  40166b:	53                   	push   ebx
  40166c:	56                   	push   esi
  40166d:	8b 75 08             	mov    esi,DWORD PTR [ebp+0x8]
  401670:	57                   	push   edi
  401671:	89 65 f0             	mov    DWORD PTR [ebp-0x10],esp
  401674:	56                   	push   esi
  401675:	e8 36 01 00 00       	call   0x4017b0
  40167a:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
  40167d:	50                   	push   eax
  40167e:	56                   	push   esi
  40167f:	e8 9c 01 00 00       	call   0x401820
  401684:	83 c4 0c             	add    esp,0xc
  401687:	85 c0                	test   eax,eax
  401689:	75 13                	jne    0x40169e
  40168b:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  40168e:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  401695:	5f                   	pop    edi
  401696:	5e                   	pop    esi
  401697:	5b                   	pop    ebx
  401698:	8b e5                	mov    esp,ebp
  40169a:	5d                   	pop    ebp
  40169b:	c2 10 00             	ret    0x10
  40169e:	33 db                	xor    ebx,ebx
  4016a0:	8d 4d d4             	lea    ecx,[ebp-0x2c]
  4016a3:	53                   	push   ebx
  4016a4:	53                   	push   ebx
  4016a5:	53                   	push   ebx
  4016a6:	53                   	push   ebx
  4016a7:	51                   	push   ecx
  4016a8:	ff 15 c0 01 46 00    	call   DWORD PTR ds:0x4601c0
  4016ae:	85 c0                	test   eax,eax
  4016b0:	74 3d                	je     0x4016ef
  4016b2:	53                   	push   ebx
  4016b3:	53                   	push   ebx
  4016b4:	8d 55 d4             	lea    edx,[ebp-0x2c]
  4016b7:	53                   	push   ebx
  4016b8:	52                   	push   edx
  4016b9:	ff 15 bc 01 46 00    	call   DWORD PTR ds:0x4601bc
  4016bf:	85 c0                	test   eax,eax
  4016c1:	75 16                	jne    0x4016d9
  4016c3:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  4016c6:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
  4016c9:	5f                   	pop    edi
  4016ca:	5e                   	pop    esi
  4016cb:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4016d2:	5b                   	pop    ebx
  4016d3:	8b e5                	mov    esp,ebp
  4016d5:	5d                   	pop    ebp
  4016d6:	c2 10 00             	ret    0x10
  4016d9:	8d 45 d4             	lea    eax,[ebp-0x2c]
  4016dc:	50                   	push   eax
  4016dd:	ff 15 ac 01 46 00    	call   DWORD PTR ds:0x4601ac
  4016e3:	8d 4d d4             	lea    ecx,[ebp-0x2c]
  4016e6:	51                   	push   ecx
  4016e7:	ff 15 b4 01 46 00    	call   DWORD PTR ds:0x4601b4
  4016ed:	eb af                	jmp    0x40169e
  4016ef:	38 1d 8c f3 46 00    	cmp    BYTE PTR ds:0x46f38c,bl
  4016f5:	0f 84 a1 00 00 00    	je     0x40179c
  4016fb:	38 1d bc 80 46 00    	cmp    BYTE PTR ds:0x4680bc,bl
  401701:	0f 84 95 00 00 00    	je     0x40179c
  401707:	38 1d 8d f3 46 00    	cmp    BYTE PTR ds:0x46f38d,bl
  40170d:	0f 84 89 00 00 00    	je     0x40179c
  401713:	8b 0d f4 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e24f4
  401719:	89 5d fc             	mov    DWORD PTR [ebp-0x4],ebx
  40171c:	e8 4f 92 04 00       	call   0x44a970
  401721:	8b 0d b4 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6b4
  401727:	53                   	push   ebx
  401728:	53                   	push   ebx
  401729:	e8 c2 34 00 00       	call   0x404bf0
  40172e:	88 1d 8d f3 46 00    	mov    BYTE PTR ds:0x46f38d,bl
  401734:	c7 45 fc ff ff ff ff 	mov    DWORD PTR [ebp-0x4],0xffffffff
  40173b:	e9 5e ff ff ff       	jmp    0x40169e
  401740:	68 00 01 00 00       	push   0x100
  401745:	e8 15 ff 04 00       	call   0x45165f
  40174a:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
  40174d:	8b f0                	mov    esi,eax
  40174f:	52                   	push   edx


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401b20 <.text+0xb20>:
  401b20:	b8 86 1b 40 00       	mov    eax,0x401b86
  401b25:	c3                   	ret
  401b26:	eb 5e                	jmp    0x401b86
  401b28:	3d 02 02 00 00       	cmp    eax,0x202
  401b2d:	77 23                	ja     0x401b52
  401b2f:	74 18                	je     0x401b49
  401b31:	8b c8                	mov    ecx,eax
  401b33:	81 e9 11 01 00 00    	sub    ecx,0x111
  401b39:	74 4b                	je     0x401b86
  401b3b:	83 e9 02             	sub    ecx,0x2
  401b3e:	75 19                	jne    0x401b59
  401b40:	c6 05 8d f3 46 00 01 	mov    BYTE PTR ds:0x46f38d,0x1
  401b47:	eb 3d                	jmp    0x401b86
  401b49:	c6 05 b9 84 46 00 01 	mov    BYTE PTR ds:0x4684b9,0x1
  401b50:	eb 34                	jmp    0x401b86
  401b52:	3d 01 04 00 00       	cmp    eax,0x401
  401b57:	74 23                	je     0x401b7c
  401b59:	8b 55 14             	mov    edx,DWORD PTR [ebp+0x14]
  401b5c:	52                   	push   edx
  401b5d:	57                   	push   edi
  401b5e:	50                   	push   eax
  401b5f:	8b               	mov    eax,DWORD PTR [ebp+0x8]


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0044a970 <.text+0x49970>:
  44a970:	a1 90 f3 46 00       	mov    eax,ds:0x46f390
  44a975:	83 ec 34             	sub    esp,0x34
  44a978:	85 c0                	test   eax,eax
  44a97a:	56                   	push   esi
  44a97b:	8b f1                	mov    esi,ecx
  44a97d:	74 0b                	je     0x44a98a
  44a97f:	8a 48 0a             	mov    cl,BYTE PTR [eax+0xa]
  44a982:	84 c9                	test   cl,cl
  44a984:	0f 85 01 01 00 00    	jne    0x44aa8b
  44a98a:	6a 00                	push   0x0
  44a98c:	8b ce                	mov    ecx,esi
  44a98e:	e8 6d 9f fe ff       	call   0x434900
  44a993:	84 c0                	test   al,al
  44a995:	75 1a                	jne    0x44a9b1
  44a997:	a1 84 f3 46 00       	mov    eax,ds:0x46f384
  44a99c:	6a 00                	push   0x0
  44a99e:	6a 00                	push   0x0
  44a9a0:	68 01 04 00 00       	push   0x401
  44a9a5:	50                   	push   eax
  44a9a6:	ff 15 90 01 46 00    	call   DWORD PTR ds:0x460190
  44a9ac:	5e                   	pop    esi
  44a9ad:	83 c4 34             	add    esp,0x34
  44a9b0:	c3                   	ret
  44a9b1:	8b ce                	mov    ecx,esi
  44a9b3:	e8 78 9e fe ff       	call   0x434830
  44a9b8:	8b ce                	mov    ecx,esi
  44a9ba:	e8 31 9c fe ff       	call   0x4345f0
  44a9bf:	e8 dc 76 fb ff       	call   0x4020a0
  44a9c4:	8b ce                	mov    ecx,esi
  44a9c6:	e8 45 9d fe ff       	call   0x434710
  44a9cb:	e8 20 77 fb ff       	call   0x4020f0
  44a9d0:	a0 04 85 97 00       	mov    al,ds:0x978504
  44a9d5:	84 c0                	test   al,al
  44a9d7:	74 07                	je     0x44a9e0
  44a9d9:	8b ce                	mov    ecx,esi
  44a9db:	e8 b0 00 00 00       	call   0x44aa90
  44a9e0:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  44a9e6:	e8 25 b0 fb ff       	call   0x405a10
  44a9eb:	a0 05 85 97 00       	mov    al,ds:0x978505
  44a9f0:	84 c0                	test   al,al
  44a9f2:	0f 84 93 00 00 00    	je     0x44aa8b
  44a9f8:	57                   	push   edi
  44a9f9:	b9 00 58 02 00       	mov    ecx,0x25800
  44a9fe:	33 c0                	xor    eax,eax
  44aa00:	bf f8 24 8e 00       	mov    edi,0x8e24f8
  44aa05:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  44aa07:	8b 0d c8 cc 46 00    	mov    ecx,DWORD PTR ds:0x46ccc8
  44aa0d:	8d 54 24           	lea    edx,[esp+0x8]
