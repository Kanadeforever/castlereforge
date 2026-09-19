
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f2f6 <.text+0x1e2f6>:
  41f2f6:	68 d0 ff 89 00       	push   0x89ffd0
  41f2fb:	52                   	push   edx
  41f2fc:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  41f301:	a3 f0 fd 89 00       	mov    ds:0x89fdf0,eax
  41f306:	e8 55 b4 00 00       	call   0x42a760
  41f30b:	8b ce                	mov    ecx,esi
  41f30d:	e8 6e 02 00 00       	call   0x41f580
  41f312:	84 c0                	test   al,al
  41f314:	0f 84 5d 02 00 00    	je     0x41f577
  41f31a:	6a 00                	push   0x0
  41f31c:	6a 00                	push   0x0
  41f31e:	6a 67                	push   0x67
  41f320:	8b ce                	mov    ecx,esi
  41f322:	e8 f9 2d 00 00       	call   0x422120
  41f327:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004200ff <.text+0x1f0ff>:
  4200ff:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  420105:	6a 00                	push   0x0
  420107:	e8 f4 40 01 00       	call   0x434200
  42010c:	c6 05 dc 96 46 00 01 	mov    BYTE PTR ds:0x4696dc,0x1
  420113:	6a 00                	push   0x0
  420115:	6a 00                	push   0x0
  420117:	6a 67                	push   0x67
  420119:	8b ce                	mov    ecx,esi
  42011b:	e8 00 20 00 00       	call   0x422120
  420120:	6a 01                	push   0x1
  420122:	8b ce                	mov    ecx,esi
  420124:	e8 a7 0d 00 00       	call   0x420ed0

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042039a <.text+0x1f39a>:
  42039a:	00 6a 00             	add    BYTE PTR [edx+0x0],ch
  42039d:	e8 5e 3e 01 00       	call   0x434200
  4203a2:	6a 00                	push   0x0
  4203a4:	6a 00                	push   0x0
  4203a6:	6a 67                	push   0x67
  4203a8:	8b ce                	mov    ecx,esi
  4203aa:	c7 86 a4 07 00 00 ed 	mov    DWORD PTR [esi+0x7a4],0x3ed
  4203b1:	03 00 00 
  4203b4:	e8 67 1d 00 00       	call   0x422120
  4203b9:	6a 01                	push   0x1
  4203bb:	8b ce                	mov    ecx,esi
  4203bd:	e8 0e 0b 00 00       	call   0x420ed0

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004213e1 <.text+0x203e1>:
  4213e1:	8a 43 3c             	mov    al,BYTE PTR [ebx+0x3c]
  4213e4:	84 c0                	test   al,al
  4213e6:	0f 84 e7 00 00 00    	je     0x4214d3
  4213ec:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  4213f3:	55                   	push   ebp
  4213f4:	55                   	push   ebp
  4213f5:	6a 67                	push   0x67
  4213f7:	8b 88 a0 0b 00 00    	mov    ecx,DWORD PTR [eax+0xba0]
  4213fd:	89 88 9c 0b 00 00    	mov    DWORD PTR [eax+0xb9c],ecx
  421403:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  42140a:	8b 90 4c 08 00 00    	mov    edx,DWORD PTR [eax+0x84c]
  421410:	89 90 2c 07 00 00    	mov    DWORD PTR [eax+0x72c],edx
  421416:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  42141d:	e8 fe 0c 00 00       	call   0x422120

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443f30 <.text+0x42f30>:
  443f30:	8d 9d e4 03 00 00    	lea    ebx,[ebp+0x3e4]
  443f36:	51                   	push   ecx
  443f37:	8b cb                	mov    ecx,ebx
  443f39:	e8 72 b3 fd ff       	call   0x41f2b0
  443f3e:	8b cb                	mov    ecx,ebx
  443f40:	e8 8b f2 fd ff       	call   0x4231d0
  443f45:	6a 00                	push   0x0
  443f47:	6a 00                	push   0x0
  443f49:	6a 67                	push   0x67
  443f4b:	8b cb                	mov    ecx,ebx
  443f4d:	e8 ce e1 fd ff       	call   0x422120
