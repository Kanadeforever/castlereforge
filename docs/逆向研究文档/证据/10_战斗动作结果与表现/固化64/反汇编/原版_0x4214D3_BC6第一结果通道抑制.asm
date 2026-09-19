
/mnt/data/solid64_inputs/RPG.exe:     file format pei-i386


Disassembly of section .text:

004214d3 <.text+0x204d3>:
  4214d3:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  4214da:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  4214e0:	8b 81 9c 0b 00 00    	mov    eax,DWORD PTR [ecx+0xb9c]
  4214e6:	c1 e0 02             	shl    eax,0x2
  4214e9:	8b 14 10             	mov    edx,DWORD PTR [eax+edx*1]
  4214ec:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  4214f0:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  4214f6:	8b 04 10             	mov    eax,DWORD PTR [eax+edx*1]
  4214f9:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  4214fd:	8b 81 e4 0c 00 00    	mov    eax,DWORD PTR [ecx+0xce4]
  421503:	3d c6 0b 00 00       	cmp    eax,0xbc6
  421508:	75 02                	jne    0x42150c
  42150a:	89 2b                	mov    DWORD PTR [ebx],ebp
  42150c:	8a 43 fc             	mov    al,BYTE PTR [ebx-0x4]
  42150f:	8b 2b                	mov    ebp,DWORD PTR [ebx]
  421511:	8b 7b 04             	mov    edi,DWORD PTR [ebx+0x4]
  421514:	84 c0                	test   al,al
  421516:	74 48                	je     0x421560
  421518:	8a 4b 44             	mov    cl,BYTE PTR [ebx+0x44]
  42151b:	51                   	push   ecx
  42151c:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  421520:	56                   	push   esi
  421521:	e8 fa f8 ff ff       	call   0x420e20
  421526:	84 c0                	test   al,al
  421528:	74 36                	je     0x421560
  42152a:	85 ed                	test   ebp,ebp
  42152c:	74 17                	je     0x421545
  42152e:	33 c0                	xor    eax,eax
  421530:	85 ed                	test   ebp,ebp
  421532:	0f 9e c0             	setle  al
  421535:	48                   	dec    eax
  421536:	83 e0 04             	and    eax,0x4
  421539:	83 c0 fe             	add    eax,0xfffffffe
  42153c:	03 c5                	add    eax,ebp
  42153e:	99                   	cdq
  42153f:	2b c2                	sub    eax,edx
  421541:	d1 f8                	sar    eax,1
  421543:	8b e8                	mov    ebp,eax
  421545:	85 ff                	test   edi,edi
  421547:	74 17                	je     0x421560
  421549:	33 c0                	xor    eax,eax
  42154b:	85 ff                	test   edi,edi
  42154d:	0f 9e c0             	setle  al
  421550:	48                   	dec    eax
  421551:	83 e0 04             	and    eax,0x4
  421554:	83 c0 fe             	add    eax,0xfffffffe
  421557:	03 c7                	add    eax,edi
  421559:	99                   	cdq
  42155a:	2b c2                	sub    eax,edx
  42155c:	d1 f8                	sar    eax,1
  42155e:	8b f8                	mov    edi,eax
