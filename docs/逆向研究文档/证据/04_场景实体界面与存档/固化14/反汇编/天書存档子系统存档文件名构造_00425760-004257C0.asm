
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00425760 <.text+0x24760>:
  425760:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  425766:	6a ff                	push   0xffffffff
  425768:	68 14 e4 45 00       	push   0x45e414
  42576d:	50                   	push   eax
  42576e:	b8 e4 e1 00 00       	mov    eax,0xe1e4
  425773:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  42577a:	e8 21 ca 02 00       	call   0x4521a0
  42577f:	8b 84 24 f4 e1 00 00 	mov    eax,DWORD PTR [esp+0xe1f4]
  425786:	53                   	push   ebx
  425787:	56                   	push   esi
  425788:	68 3c a4 46 00       	push   0x46a43c
  42578d:	50                   	push   eax
  42578e:	68 30 a4 46 00       	push   0x46a430
  425793:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  425797:	68 24 a4 46 00       	push   0x46a424
  42579c:	51                   	push   ecx
  42579d:	bb 01 00 00 00       	mov    ebx,0x1
  4257a2:	e8 cc c2 02 00       	call   0x451a73
  4257a7:	6a 14                	push   0x14
  4257a9:	e8 b1 be 02 00       	call   0x45165f
  4257ae:	83 c4 18             	add    esp,0x18
  4257b1:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4257b5:	85 c0                	test   eax,eax
  4257b7:	c7 84 24 f4 e1 00 00 	mov    DWORD PTR [esp+0xe1f4],0x0
  4257be:	00 00   
