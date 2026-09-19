; 固化48：raw1与raw9在Battle每角色更新中的直接运行时行为。

/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420540 <.text+0x1f540>:
  420540:	85 eb                	test   ebx,ebp
  420542:	51                   	push   ecx
  420543:	d1 e1                	shl    ecx,1
  420545:	f7 e9                	imul   ecx
  420547:	c1 fa 05             	sar    edx,0x5
  42054a:	8b ca                	mov    ecx,edx
  42054c:	c1 e9 1f             	shr    ecx,0x1f
  42054f:	03 d1                	add    edx,ecx
  420551:	8b fa                	mov    edi,edx
  420553:	8b 86 38 0a 00 00    	mov    eax,DWORD PTR [esi+0xa38]
  420559:	85 c0                	test   eax,eax
  42055b:	7e 13                	jle    0x420570
  42055d:	b8 67 66 66 66       	mov    eax,0x66666667
  420562:	f7 ef                	imul   edi
  420564:	c1 fa 02             	sar    edx,0x2
  420567:	8b c2                	mov    eax,edx
  420569:	c1 e8 1f             	shr    eax,0x1f
  42056c:	03 d0                	add    edx,eax
  42056e:	8b fa                	mov    edi,edx
  420570:	8b 86 74 0a 00 00    	mov    eax,DWORD PTR [esi+0xa74]
  420576:	85 c0                	test   eax,eax
  420578:	7e 1c                	jle    0x420596
  42057a:	8d 04 7f             	lea    eax,[edi+edi*2]
  42057d:	8d 04 80             	lea    eax,[eax+eax*4]
  420580:	8d 0c 80             	lea    ecx,[eax+eax*4]
  420583:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420588:	f7 e9                	imul   ecx
  42058a:	c1 fa 05             	sar    edx,0x5
  42058d:	8b ca                	mov    ecx,edx
  42058f:	c1 e9 1f             	shr    ecx,0x1f
  420592:	03 d1                	add    edx,ecx
  420594:	8b fa                	mov    edi,edx
  420596:	8b 86 44 0a 00 00    	mov    eax,DWORD PTR [esi+0xa44]
  42059c:	85 c0                	test   eax,eax
  42059e:	7e 1b                	jle    0x4205bb
  4205a0:	8d 04 bf             	lea    eax,[edi+edi*4]
  4205a3:	8d 0c 80             	lea    ecx,[eax+eax*4]
  4205a6:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4205ab:	d1 e1                	shl    ecx,1
  4205ad:	f7 e9                	imul   ecx
  4205af:	c1 fa 05             	sar    edx,0x5
  4205b2:	8b c2                	mov    eax,edx
  4205b4:	c1 e8 1f             	shr    eax,0x1f
  4205b7:	03 d0                	add    edx,eax
  4205b9:	8b fa                	mov    edi,edx
  4205bb:	8b 86 90 0a 00 00    	mov    eax,DWORD PTR [esi+0xa90]
  4205c1:	85 c0                	test   eax,eax
  4205c3:	7e 1e                	jle    0x4205e3
  4205c5:	8d 04 7f             	lea    eax,[edi+edi*2]
  4205c8:	8d 04 80             	lea    eax,[eax+eax*4]
  4205cb:	8d 0c 80             	lea    ecx,[eax+eax*4]
  4205ce:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4205d3:	d1 e1                	shl    ecx,1
  4205d5:	f7 e9                	imul   ecx
  4205d7:	c1 fa 05             	sar    edx,0x5
  4205da:	8b ca                	mov    ecx,edx
  4205dc:	c1 e9 1f             	shr    ecx,0x1f
  4205df:	03 d1                	add    edx,ecx
  4205e1:	8b fa                	mov    edi,edx
  4205e3:	8b 86 58 0a 00 00    	mov    eax,DWORD PTR [esi+0xa58]
  4205e9:	85 c0                	test   eax,eax
  4205eb:	7e 04                	jle    0x4205f1
  4205ed:	33 ff                	xor    edi,edi
  4205ef:	eb 0d                	jmp    0x4205fe
  4205f1:	81 ff c8 00 00 00    	cmp    edi,0xc8
  4205f7:	7e 05                	jle    0x4205fe
  4205f9:	bf c8 00 00 00       	mov    edi,0xc8
  4205fe:	53                   	push   ebx
  4205ff:	8d 9e a8 0c 00 00    	lea    ebx,[esi+0xca8]
  420605:	6a 01                	push   0x1
  420607:	57                   	push   edi
  420608:	8b cb                	mov    ecx,ebx
  42060a:	e8 81 3a 00 00       	call   0x424090
  42060f:	6a 02                	push   0x2
  420611:	57                   	push   edi
  420612:	8b cb                	mov    ecx,ebx
  420614:	e8 77 3a 00 00       	call   0x424090
  420619:	8b 86 38 0a 00 00    	mov    eax,DWORD PTR [esi+0xa38]
  42061f:	ba 64 00 00 00       	mov    edx,0x64
  420624:	85 c0                	test   eax,eax
  420626:	7e 05                	jle    0x42062d
  420628:	ba 0a 00 00 00       	mov    edx,0xa
  42062d:	8b 86 74 0a 00 00    	mov    eax,DWORD PTR [esi+0xa74]
  420633:	85 c0                	test   eax,eax
  420635:	7e 1a                	jle    0x420651
  420637:	8d 04 52             	lea    eax,[edx+edx*2]
  42063a:	8d 04 80             	lea    eax,[eax+eax*4]
  42063d:	8d 0c 80             	lea    ecx,[eax+eax*4]
