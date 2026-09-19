
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004232d0 <.text+0x222d0>:
  4232d0:	53                   	push   ebx
  4232d1:	55                   	push   ebp
  4232d2:	56                   	push   esi
  4232d3:	8b f1                	mov    esi,ecx
  4232d5:	57                   	push   edi
  4232d6:	8b 86 4c 08 00 00    	mov    eax,DWORD PTR [esi+0x84c]
  4232dc:	83 f8 02             	cmp    eax,0x2
  4232df:	0f 84 7f 01 00 00    	je     0x423464
  4232e5:	83 f8 05             	cmp    eax,0x5
  4232e8:	0f 84 76 01 00 00    	je     0x423464
  4232ee:	83 f8 08             	cmp    eax,0x8
  4232f1:	0f 84 6d 01 00 00    	je     0x423464
  4232f7:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  4232fd:	85 c0                	test   eax,eax
  4232ff:	0f 84 5f 01 00 00    	je     0x423464
  423305:	8b be e4 08 00 00    	mov    edi,DWORD PTR [esi+0x8e4]
  42330b:	85 ff                	test   edi,edi
  42330d:	7e 2d                	jle    0x42333c
  42330f:	a1 b4 01 8a 00       	mov    eax,ds:0x8a01b4
  423314:	ba 01 00 00 00       	mov    edx,0x1
  423319:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  42331c:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  423320:	81 c1 6c 01 00 00    	add    ecx,0x16c
  423326:	39 79 fc             	cmp    DWORD PTR [ecx-0x4],edi
  423329:	74 04                	je     0x42332f
  42332b:	39 39                	cmp    DWORD PTR [ecx],edi
  42332d:	75 02                	jne    0x423331
  42332f:	8b c2                	mov    eax,edx
  423331:	42                   	inc    edx
  423332:	83 c1 08             	add    ecx,0x8
  423335:	83 fa 05             	cmp    edx,0x5
  423338:	7e ec                	jle    0x423326
  42333a:	eb 04                	jmp    0x423340
  42333c:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  423340:	85 c0                	test   eax,eax
  423342:	0f 84 1c 01 00 00    	je     0x423464
  423348:	83 f8 01             	cmp    eax,0x1
  42334b:	0f 8c 13 01 00 00    	jl     0x423464
  423351:	83 f8 05             	cmp    eax,0x5
  423354:	0f 8f 0a 01 00 00    	jg     0x423464
  42335a:	89 86 20 07 00 00    	mov    DWORD PTR [esi+0x720],eax
  423360:	8b 0d b4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b4
  423366:	33 ff                	xor    edi,edi
  423368:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  42336b:	8b 9c c1 30 01 00 00 	mov    ebx,DWORD PTR [ecx+eax*8+0x130]
  423372:	8b ac c1 34 01 00 00 	mov    ebp,DWORD PTR [ecx+eax*8+0x134]
  423379:	8b 8e 80 08 00 00    	mov    ecx,DWORD PTR [esi+0x880]
  42337f:	85 c9                	test   ecx,ecx
  423381:	7e 12                	jle    0x423395
  423383:	8b 84 86 84 09 00 00 	mov    eax,DWORD PTR [esi+eax*4+0x984]
  42338a:	99                   	cdq
  42338b:	83 e2 03             	and    edx,0x3
  42338e:	03 c2                	add    eax,edx
  423390:	8b f8                	mov    edi,eax
  423392:	c1 ff 02             	sar    edi,0x2
  423395:	e8 cb e1 02 00       	call   0x451565
  42339a:	99                   	cdq
  42339b:	b9 64 00 00 00       	mov    ecx,0x64
  4233a0:	f7 f9                	idiv   ecx
  4233a2:	3b fa                	cmp    edi,edx
  4233a4:	0f 8c ba 00 00 00    	jl     0x423464
  4233aa:	85 db                	test   ebx,ebx
  4233ac:	7e 08                	jle    0x4233b6
  4233ae:	53                   	push   ebx
  4233af:	8b ce                	mov    ecx,esi
  4233b1:	e8 4a fd ff ff       	call   0x423100
  4233b6:	85 ed                	test   ebp,ebp
  4233b8:	7e 08                	jle    0x4233c2
  4233ba:	55                   	push   ebp
  4233bb:	8b ce                	mov    ecx,esi
  4233bd:	e8 3e fd ff ff       	call   0x423100
  4233c2:	85 db                	test   ebx,ebx
  4233c4:	75 11                	jne    0x4233d7
  4233c6:	85 ed                	test   ebp,ebp
  4233c8:	75 0d                	jne    0x4233d7
  4233ca:	89 ae 20 07 00 00    	mov    DWORD PTR [esi+0x720],ebp
  4233d0:	5f                   	pop    edi
  4233d1:	5e                   	pop    esi
  4233d2:	5d                   	pop    ebp
  4233d3:	5b                   	pop    ebx
  4233d4:	c2 04 00             	ret    0x4
  4233d7:	8b         	mov    eax,DWORD PTR [esi+0xb9c]
