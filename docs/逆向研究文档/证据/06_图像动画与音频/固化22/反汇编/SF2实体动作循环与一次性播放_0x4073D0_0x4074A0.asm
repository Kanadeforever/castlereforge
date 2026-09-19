
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004073d0 <.text+0x63d0>:
  4073d0:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  4073d4:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  4073d8:	89 01                	mov    DWORD PTR [ecx],eax
  4073da:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4073de:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  4073e1:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  4073e5:	50                   	push   eax
  4073e6:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  4073ea:	52                   	push   edx
  4073eb:	50                   	push   eax
  4073ec:	e8 0f 00 00 00       	call   0x407400
  4073f1:	c2 14 00             	ret    0x14
  4073f4:	90                   	nop
  4073f5:	90                   	nop
  4073f6:	90                   	nop
  4073f7:	90                   	nop
  4073f8:	90                   	nop
  4073f9:	90                   	nop
  4073fa:	90                   	nop
  4073fb:	90                   	nop
  4073fc:	90                   	nop
  4073fd:	90                   	nop
  4073fe:	90                   	nop
  4073ff:	90                   	nop
  407400:	8b 41 18             	mov    eax,DWORD PTR [ecx+0x18]
  407403:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  407407:	85 c0                	test   eax,eax
  407409:	56                   	push   esi
  40740a:	74 11                	je     0x40741d
  40740c:	39 51 10             	cmp    DWORD PTR [ecx+0x10],edx
  40740f:	75 0c                	jne    0x40741d
  407411:	48                   	dec    eax
  407412:	c6 41 2d 00          	mov    BYTE PTR [ecx+0x2d],0x0
  407416:	89 41 18             	mov    DWORD PTR [ecx+0x18],eax
  407419:	5e                   	pop    esi
  40741a:	c2 0c 00             	ret    0xc
  40741d:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  407420:	3b c2                	cmp    eax,edx
  407422:	74 1c                	je     0x407440
  407424:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  407428:	89 51 10             	mov    DWORD PTR [ecx+0x10],edx
  40742b:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  40742e:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  407431:	c7 41 18 01 00 00 00 	mov    DWORD PTR [ecx+0x18],0x1
  407438:	c6 41 2d 01          	mov    BYTE PTR [ecx+0x2d],0x1
  40743c:	5e                   	pop    esi
  40743d:	c2 0c 00             	ret    0xc
  407440:	8b 51 14             	mov    edx,DWORD PTR [ecx+0x14]
  407443:	8b 71 0c             	mov    esi,DWORD PTR [ecx+0xc]
  407446:	42                   	inc    edx
  407447:	03 f0                	add    esi,eax
  407449:	3b d6                	cmp    edx,esi
  40744b:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  40744e:	72 28                	jb     0x407478
  407450:	80 7c 24 10 00       	cmp    BYTE PTR [esp+0x10],0x0
  407455:	74 05                	je     0x40745c
  407457:	89 41 14             	mov    DWORD PTR [ecx+0x14],eax
  40745a:	eb 1c                	jmp    0x407478
  40745c:	8a 81 80 00 00 00    	mov    al,BYTE PTR [ecx+0x80]
  407462:	4a                   	dec    edx
  407463:	84 c0                	test   al,al
  407465:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  407468:	74 0e                	je     0x407478
  40746a:	c6 05 1e f8 89 00 00 	mov    BYTE PTR ds:0x89f81e,0x0
  407471:	c6 81 80 00 00 00 00 	mov    BYTE PTR [ecx+0x80],0x0
  407478:	8b 51 14             	mov    edx,DWORD PTR [ecx+0x14]
  40747b:	8b 41 38             	mov    eax,DWORD PTR [ecx+0x38]
  40747e:	33 f6                	xor    esi,esi
  407480:	8b 14 90             	mov    edx,DWORD PTR [eax+edx*4]
  407483:	8b 41 34             	mov    eax,DWORD PTR [ecx+0x34]
  407486:	66 8b 74 02 22       	mov    si,WORD PTR [edx+eax*1+0x22]
  40748b:	c6 41 2d 01          	mov    BYTE PTR [ecx+0x2d],0x1
  40748f:	89 71 18             	mov    DWORD PTR [ecx+0x18],esi
  407492:	5e                   	pop    esi
  407493:	c2 0c 00             	ret    0xc
  407496:	90                   	nop
  407497:	90                   	nop
  407498:	90                   	nop
  407499:	90                   	nop
  40749a:	90                   	nop
  40749b:	90                   	nop
  40749c:	90                   	nop
  40749d:	90                   	nop
  40749e:	90                   	nop
  40749f:	90                   	nop
