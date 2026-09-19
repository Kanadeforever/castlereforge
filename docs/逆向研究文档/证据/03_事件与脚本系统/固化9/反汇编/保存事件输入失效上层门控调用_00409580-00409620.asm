
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00409580 <.text+0x8580>:
  409580:	83 ec 08             	sub    esp,0x8
  409583:	56                   	push   esi
  409584:	8b f1                	mov    esi,ecx
  409586:	57                   	push   edi
  409587:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  40958a:	85 c9                	test   ecx,ecx
  40958c:	74 09                	je     0x409597
  40958e:	6a 00                	push   0x0
  409590:	6a 00                	push   0x0
  409592:	e8 09 d2 ff ff       	call   0x4067a0
  409597:	e8 d4 f4 ff ff       	call   0x408a70
  40959c:	8b f8                	mov    edi,eax
  40959e:	8d 44 24 08          	lea    eax,[esp+0x8]
  4095a2:	50                   	push   eax
  4095a3:	e8 68 f6 ff ff       	call   0x408c10
  4095a8:	8b c8                	mov    ecx,eax
  4095aa:	e8 71 11 00 00       	call   0x40a720
  4095af:	a0 b9 8b 46 00       	mov    al,ds:0x468bb9
  4095b4:	84 c0                	test   al,al
  4095b6:	74 1e                	je     0x4095d6
  4095b8:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4095bc:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  4095c0:	57                   	push   edi
  4095c1:	51                   	push   ecx
  4095c2:	52                   	push   edx
  4095c3:	e8 68 f2 ff ff       	call   0x408830
  4095c8:	83 c4 08             	add    esp,0x8
  4095cb:	50                   	push   eax
  4095cc:	e8 6f f6 ff ff       	call   0x408c40
  4095d1:	83 c4 08             	add    esp,0x8
  4095d4:	eb 25                	jmp    0x4095fb
  4095d6:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  4095da:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  4095de:	50                   	push   eax
  4095df:	51                   	push   ecx
  4095e0:	c7 05 f4 f7 89 00 00 	mov    DWORD PTR ds:0x89f7f4,0x0
  4095e7:	00 00 00 
  4095ea:	e8 41 f2 ff ff       	call   0x408830
  4095ef:	6a 00                	push   0x0
  4095f1:	6a 00                	push   0x0
  4095f3:	e8 48 f6 ff ff       	call   0x408c40
  4095f8:	83 c4 10             	add    esp,0x10
  4095fb:	e8 10 f6 ff ff       	call   0x408c10
  409600:	8b 56 1c             	mov    edx,DWORD PTR [esi+0x1c]
  409603:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  409606:	89 04 91             	mov    DWORD PTR [ecx+edx*4],eax
  409609:	8b 56 1c             	mov    edx,DWORD PTR [esi+0x1c]
  40960c:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  40960f:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  409615:	89 4c 90 04          	mov    DWORD PTR [eax+edx*4+0x4],ecx
  409619:	8b ce                	mov    ecx,esi
  40961b:	e8 20 00 00 00       	call   0x409640
