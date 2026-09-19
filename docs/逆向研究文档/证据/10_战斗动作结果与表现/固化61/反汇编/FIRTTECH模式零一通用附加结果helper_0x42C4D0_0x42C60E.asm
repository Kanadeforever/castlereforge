
/mnt/data/solid61_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

0042c4d0 <.text+0x2b4d0>:
  42c4d0:	83 ec 08             	sub    esp,0x8
  42c4d3:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42c4d7:	53                   	push   ebx
  42c4d8:	55                   	push   ebp
  42c4d9:	56                   	push   esi
  42c4da:	8b 80 90 00 00 00    	mov    eax,DWORD PTR [eax+0x90]
  42c4e0:	57                   	push   edi
  42c4e1:	bd 64 00 00 00       	mov    ebp,0x64
  42c4e6:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  42c4ed:	89 6c 24 10          	mov    DWORD PTR [esp+0x10],ebp
  42c4f1:	2b c8                	sub    ecx,eax
  42c4f3:	89 6c 24 14          	mov    DWORD PTR [esp+0x14],ebp
  42c4f7:	8d 14 88             	lea    edx,[eax+ecx*4]
  42c4fa:	c1 e2 04             	shl    edx,0x4
  42c4fd:	2b d0                	sub    edx,eax
  42c4ff:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  42c504:	8b 88 ac 00 00 00    	mov    ecx,DWORD PTR [eax+0xac]
  42c50a:	8b bc 51 d4 00 00 00 	mov    edi,DWORD PTR [ecx+edx*2+0xd4]
  42c511:	8b 5c 51 48          	mov    ebx,DWORD PTR [ecx+edx*2+0x48]
  42c515:	8d 04 51             	lea    eax,[ecx+edx*2]
  42c518:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  42c51c:	8b 44 b9 64          	mov    eax,DWORD PTR [ecx+edi*4+0x64]
  42c520:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  42c524:	99                   	cdq
  42c525:	8b f0                	mov    esi,eax
  42c527:	8b 81 90 00 00 00    	mov    eax,DWORD PTR [ecx+0x90]
  42c52d:	33 f2                	xor    esi,edx
  42c52f:	2b f2                	sub    esi,edx
  42c531:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42c537:	85 c0                	test   eax,eax
  42c539:	7e 18                	jle    0x42c553
  42c53b:	8d 2c c5 00 00 00 00 	lea    ebp,[eax*8+0x0]
  42c542:	2b e8                	sub    ebp,eax
  42c544:	8d 2c a8             	lea    ebp,[eax+ebp*4]
  42c547:	c1 e5 04             	shl    ebp,0x4
  42c54a:	2b e8                	sub    ebp,eax
  42c54c:	8d 04 6a             	lea    eax,[edx+ebp*2]
  42c54f:	8b 6c b8 74          	mov    ebp,DWORD PTR [eax+edi*4+0x74]
  42c553:	8b 81 94 00 00 00    	mov    eax,DWORD PTR [ecx+0x94]
  42c559:	85 c0                	test   eax,eax
  42c55b:	7e 24                	jle    0x42c581
  42c55d:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42c564:	2b d0                	sub    edx,eax
  42c566:	8d 14 90             	lea    edx,[eax+edx*4]
  42c569:	c1 e2 04             	shl    edx,0x4
  42c56c:	2b d0                	sub    edx,eax
  42c56e:	8b c2                	mov    eax,edx
  42c570:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42c576:	8d 04 42             	lea    eax,[edx+eax*2]
  42c579:	8b 44 b8 74          	mov    eax,DWORD PTR [eax+edi*4+0x74]
  42c57d:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  42c581:	8b 89 98 00 00 00    	mov    ecx,DWORD PTR [ecx+0x98]
  42c587:	85 c9                	test   ecx,ecx
  42c589:	7e 1c                	jle    0x42c5a7
  42c58b:	8d 04 cd 00 00 00 00 	lea    eax,[ecx*8+0x0]
  42c592:	2b c1                	sub    eax,ecx
  42c594:	8d 04 81             	lea    eax,[ecx+eax*4]
  42c597:	c1 e0 04             	shl    eax,0x4
  42c59a:	2b c1                	sub    eax,ecx
  42c59c:	8d 0c 42             	lea    ecx,[edx+eax*2]
  42c59f:	8b 54 b9 74          	mov    edx,DWORD PTR [ecx+edi*4+0x74]
  42c5a3:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  42c5a7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42c5ab:	99                   	cdq
  42c5ac:	8b c8                	mov    ecx,eax
  42c5ae:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  42c5b2:	33 ca                	xor    ecx,edx
  42c5b4:	2b ca                	sub    ecx,edx
  42c5b6:	99                   	cdq
  42c5b7:	33 c2                	xor    eax,edx
  42c5b9:	2b c2                	sub    eax,edx
  42c5bb:	0f af c8             	imul   ecx,eax
  42c5be:	8b c5                	mov    eax,ebp
  42c5c0:	99                   	cdq
  42c5c1:	33 c2                	xor    eax,edx
  42c5c3:	2b c2                	sub    eax,edx
  42c5c5:	0f af c8             	imul   ecx,eax
  42c5c8:	0f af ce             	imul   ecx,esi
  42c5cb:	b8 83 de 1b 43       	mov    eax,0x431bde83
  42c5d0:	f7 e9                	imul   ecx
  42c5d2:	c1 fa 12             	sar    edx,0x12
  42c5d5:	8b c2                	mov    eax,edx
  42c5d7:	c1 e8 1f             	shr    eax,0x1f
  42c5da:	03 d0                	add    edx,eax
  42c5dc:	2b da                	sub    ebx,edx
  42c5de:	8b c3                	mov    eax,ebx
  42c5e0:	79 02                	jns    0x42c5e4
  42c5e2:	33 c0                	xor    eax,eax
  42c5e4:	85 ff                	test   edi,edi
  42c5e6:	7e 1e                	jle    0x42c606
  42c5e8:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  42c5ec:	85 c9                	test   ecx,ecx
  42c5ee:	7c 14                	jl     0x42c604
  42c5f0:	85 ed                	test   ebp,ebp
  42c5f2:	7c 10                	jl     0x42c604
  42c5f4:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  42c5f8:	85 c9                	test   ecx,ecx
  42c5fa:	7c 08                	jl     0x42c604
  42c5fc:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  42c600:	85 c9                	test   ecx,ecx
  42c602:	7d 02                	jge    0x42c606
  42c604:	f7 d8                	neg    eax
  42c606:	5f                   	pop    edi
  42c607:	5e                   	pop    esi
  42c608:	5d                   	pop    ebp
  42c609:	5b                   	pop    ebx
  42c60a:	83 c4 08             	add    esp,0x8
  42c60d:	c2               	ret    0x8
